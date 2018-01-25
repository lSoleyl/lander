#include "stdafx.h"
#include "FontLoader.hpp"

#include <iomanip>
#include <chrono>

namespace Lander {





/** Dummy enumerator to provide an enumerator when there is only one object to enumerate
 */
template<typename Interface>
class SingleFileEnumerator : public COMBase<IDWriteFontFileEnumerator> {
public:
  SingleFileEnumerator(Interface* file) : file(file), pos(-1) {
    file->AddRef();
  }

  //IDWriteFontFileEnumerator interface
  STDMETHOD(MoveNext)(BOOL* hasCurrentFile) override {
    if (!hasCurrentFile)
      return E_POINTER;

    *hasCurrentFile = (++pos == 0);
    return S_OK;      
  }

  STDMETHOD(GetCurrentFontFile)(IDWriteFontFile** fontFile) override {
    if (!fontFile)
      return E_POINTER;

    if (pos == 0) {
      file->AddRef();
      *fontFile = file;
    }
  }

private:
  Resource<Interface> file;
  int pos; // faked enumerator position to implement interface correct
};

// font file stream class implementation adapted to memory buffers
class MemoryFileStream : public COMBase<IDWriteFontFileStream> {
public:
  MemoryFileStream(const void* data, UINT32 size) : data(static_cast<const BYTE*>(data)), size(size) {}

  //IDWriteFontFileStream interface
  STDMETHOD(GetFileSize)(UINT64* fileSize) override {
    if (!fileSize)
      return E_POINTER;

    *fileSize = size;
    return S_OK;
  }

  STDMETHOD(GetLastWriteTime)(UINT64* lastWriteTime) override {
    if (!lastWriteTime)
      return E_POINTER;

    //This method is acutally used to determine which font suits better, so we have to return a recent timepoint (now)
    //lastwritetime is the number of 100ns since 01.01.1601
    std::tm tm = {};
    std::istringstream ss("01.01.1601 00:00:01");
    ss >> std::get_time(&tm,"%d.%m.%Y %H:%M:%S");
    auto t0 = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    auto now = std::chrono::system_clock::now();

    *lastWriteTime = std::chrono::duration_cast<std::chrono::microseconds>(now - t0).count() * 10;
    return S_OK;
  }


  STDMETHOD(ReadFileFragment)(const void** fragmentStart, UINT64 fileOffset, UINT64 fragmentSize, void** fragmentContext) override {
    if (!fragmentStart || !fragmentContext)
      return E_POINTER;

    if (fileOffset + fragmentSize > size) //check whether read is out of bounds
      return E_FAIL;
    
    *fragmentStart = data + fileOffset;
    *fragmentContext = nullptr; //relase isn't necessary
    return S_OK;
  }

  //Release is not necessary when holding the whole file in a memory buffer
  virtual void STDMETHODCALLTYPE ReleaseFileFragment(void* fragmentContext) override {}



private:
  const BYTE* data;
  UINT64 size;
};


//This class implements the font file loader interface and loads the font from the given memory position
class FontFileMemoryLoader : public COMBase<IDWriteFontFileLoader> {
public:
  FontFileMemoryLoader(IDWriteFactory* factory) : factory(factory) {}

  //IDWriteFontFileLoader interface
  STDMETHOD(CreateStreamFromKey)(const void* key, UINT32 keySize, IDWriteFontFileStream** fontFileStream) override {
    if (!fontFileStream)
      return E_POINTER;

    Data data = *static_cast<const Data*>(key);
    *fontFileStream = new MemoryFileStream(data.data, data.size);
    return S_OK;
  }

  IDWriteFactory* factory;
};



FontLoader* FontLoader::instance = nullptr;

FontLoader* FontLoader::Instance() {
  if (!instance) {
    instance = new FontLoader();
  }
    
  return instance;
}

FontLoader::~FontLoader() {
  // clean up
  if (fontFileLoader) {
    fontFileLoader.as<FontFileMemoryLoader*>()->factory->UnregisterFontFileLoader(fontFileLoader);
  }

}


STDMETHODIMP FontLoader::CreateEnumeratorFromKey(IDWriteFactory* factory, const void* key, UINT32 keySize, IDWriteFontFileEnumerator** fontFileEnumerator) {
  if (!fontFileEnumerator || !factory)
    return E_POINTER;

  // we need a font loader for memory loading
  if (!fontFileLoader) {
    fontFileLoader = new FontFileMemoryLoader(factory);
    auto res = factory->RegisterFontFileLoader(fontFileLoader);
    if (!SUCCEEDED(res))
      return res;
  }

  // create a font file by passing the memory to the created font file loader
  IDWriteFontFile* file = nullptr;
  auto res = factory->CreateCustomFontFileReference(key, keySize, fontFileLoader, &file);
  if (!SUCCEEDED(res))
    return res;



  *fontFileEnumerator = new SingleFileEnumerator<IDWriteFontFile>(file);
  return S_OK;
}



}
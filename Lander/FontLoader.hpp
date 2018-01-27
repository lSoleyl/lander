#pragma once

#include <Dwrite.h>

/** This file defines the necessary boilerplate COM classes load fonts from files
 */
namespace Lander {

  /** This class must be registered with the DirectWriteFactory to support loading fonts from files/memory
   *  It should be deregistered, when the font loader is no longer needed.
   */
  class FontLoader : public COMBase<IDWriteFontCollectionLoader> {
  public:
    ~FontLoader();

    static FontLoader* Instance();

    /** Registers a fontname with a resource id. The registered resourceId can later be
     *  fetched with GetFontResource.
     */
    void RegisterFontResource(const wchar_t* fontName, int resourceId);
    
    /** If a font resource has been registered before with the given font name, then this method will return the 
     *  resource id for the font file. Otherwise NO_RESOURCE will be returned.
     */
    int GetFontResource(const wchar_t* fontName);

    //IDWriteFontCollectionLoader interface
    STDMETHOD(CreateEnumeratorFromKey)(IDWriteFactory* factory,  void const* collectionKey, UINT32 collectionKeySize, IDWriteFontFileEnumerator** fontFileEnumerator) override;

    static const int NO_RESOURCE = -1;
  private:
    FontLoader() {}
    Resource<IDWriteFontFileLoader> fontFileLoader;

    std::unordered_map<std::wstring/*fontName*/, int/*resourceId*/> fontResources;
    static FontLoader* instance;    
  };

}

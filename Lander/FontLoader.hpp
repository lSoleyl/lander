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

    //IDWriteFontCollectionLoader interface
    STDMETHOD(CreateEnumeratorFromKey)(IDWriteFactory* factory,  void const* collectionKey, UINT32 collectionKeySize, IDWriteFontFileEnumerator** fontFileEnumerator) override;

  private:
    FontLoader() {}
    Resource<IDWriteFontFileLoader> fontFileLoader;
    static FontLoader* instance;
  };

}

#pragma once

/** This file defines a generic COM base implementation for IUnknown
 */
template<typename Interface>
class COMBase : public Interface {
public:
  COMBase() : refCount(1) {}
  //IUnknown implementation
  STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) override {
    if (!ppvObject)
      return E_POINTER;

    if (riid == __uuidof(Interface) || riid == IID_IUnknown) {
      AddRef();
      *ppvObject = this;
      return S_OK;
    }

    return E_NOINTERFACE;  
  }

  virtual ULONG STDMETHODCALLTYPE AddRef() override { return ++refCount; }
  virtual ULONG STDMETHODCALLTYPE Release() override { if (--refCount == 0) { delete this; return 0; } return refCount; }


private:
  ULONG refCount;
};

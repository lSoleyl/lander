#pragma once

#include <comdef.h>

/** This utility function handles HRESULT COM errors and throws them as lander::COMException
 */
void HandleCOMError(HRESULT result, const wchar_t* context = nullptr);

namespace Lander {

/** Custom exception class for com errors to be able to throw the UTF-16 error message instead of converting it to 
  *  UTF-8 or CP1252 first to throw a regular std::exception
  */
class COMException {
  public:
    COMException(std::wstring&& message);
    std::wstring message;
};

}

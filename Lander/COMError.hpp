#pragma once

#include <comdef.h>

/** This utility function handles HRESULT COM errors
 */
inline void HandleCOMError(HRESULT result, const char* context = nullptr) {
  if (SUCCEEDED(result))
    return;

  _com_error err(result);
  std::stringstream errorMessage;
  errorMessage << "COM error (0x" << std::hex << result << ") occurred";
  if (context)
    errorMessage << " during '" << context << "': ";
  else 
    errorMessage << ": ";
  errorMessage << err.ErrorMessage();
  throw std::exception(errorMessage.str().c_str());
}
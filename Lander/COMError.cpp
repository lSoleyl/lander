#include "stdafx.h"
#include "COMError.hpp"

void HandleCOMError(HRESULT result, const wchar_t* context) {
  if (SUCCEEDED(result)) {
    return;
  }

  _com_error err(result);
  std::wstringstream errorMessage;
  errorMessage << L"COM error (0x" << std::hex << result << L") occurred";
  if (context) {
    errorMessage << L" during '" << context << L"': ";
  } else {
    errorMessage << L": ";
  }
  errorMessage << err.ErrorMessage();
  throw Lander::COMException(errorMessage.str());
}


Lander::COMException::COMException(std::wstring&& message) : message(std::move(message)) {}

#include "stdafx.h"
#include "FPSCounter.hpp"

FPSCounter::FPSCounter() : avgFps(0) {}

void FPSCounter::Initialize() {
  // Initialization for text usage (direct write)
  auto result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));    

  if (SUCCEEDED(result)) {
    result = writeFactory->CreateTextFormat(
      L"Consolas",
      NULL,
      DWRITE_FONT_WEIGHT_REGULAR,
      DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL,
      72.0f,
      L"en-us",
      &textFormat);
  }

  if (!SUCCEEDED(result)) //TODO add error handling (FormatMessage)
    throw std::exception("FPS counter initialization failed!", result);
}


void FPSCounter::Draw(RenderInterface& renderInterface, double secondsPassed) {
  long fps = static_cast<long>(1.0 / secondsPassed);
  avgFps = (0.9*avgFps) + (0.1*fps); //Take the average of ten frames
  
  //Create the FPS text
  std::wstring fpsText(L"FPS: ");
  fpsText += std::to_wstring(static_cast<int>(avgFps));    
   
  //FIXME: The renderInterface should render the text we give it relative to the view object's position
  //renderInterface.RenderTarget().DrawText(fpsText.c_str(), fpsText.length(), textFormat, D2D1::RectF(0, 0, rtSize.width, rtSize.height), GetSolidBrush(ColorF::CornflowerBlue));
}

void FPSCounter::Deinitialize() {
  textFormat.reset();
  writeFactory.reset();  
}
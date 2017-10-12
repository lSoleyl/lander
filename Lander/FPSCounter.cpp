#include "stdafx.h"
#include "FPSCounter.hpp"

namespace Lander {

FPSCounter::FPSCounter() : avgFps(0), textFormat(NULL) {
  size = Size(100, 25);
}

void FPSCounter::Draw(RenderInterface& renderInterface, double secondsPassed) {
  long fps = static_cast<long>(1.0 / secondsPassed);
  avgFps = (0.9*avgFps) + (0.1*fps); //Take the average of ten frames
  
  // Create the FPS text
  std::wstring fpsText(L"FPS: ");
  fpsText += std::to_wstring(static_cast<int>(avgFps));    
  
  // Create text format upon first use
  if (!textFormat)
    textFormat = renderInterface.CreateTextFormat(L"Consolas", 20);

  renderInterface.DrawText(fpsText, textFormat, Rectangle(Vector(), size), Color::LightGoldenrodYellow);
}

}
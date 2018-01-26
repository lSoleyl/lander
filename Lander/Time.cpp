#include "stdafx.h"
#include "Time.hpp"
#include <sstream>

namespace Lander {

  TimeCounter::TimeCounter() : textFormat(NULL) {
    size = Size(280, 30);
  }

  int TimeCounter::RenderPriority() const { return -99; }

  void TimeCounter::StartCount() {
    //TODO: TimeCounters "Nullzeitpunkt" setzen (in Rocket)
    started = true;
    startTime = std::chrono::high_resolution_clock::now();
  }

  void TimeCounter::StopCount() {
    started = false;
  }

  void TimeCounter::ResetCount() {
    StopCount();
    passedSeconds = 0;
    passedMinutes = 0;
  }

  void TimeCounter::Initialize(Size _size) {
    windowSize = _size;
    pos = Vector() + Vector::Right * (windowSize.width - size.width);
  }

  void TimeCounter::Draw(RenderInterface& renderInterface, double secondsPassed) {

    if (started) {   //Calculate passed time
      passedTime = startTime - std::chrono::high_resolution_clock::now();
      passedSeconds += -1*(std::chrono::duration_cast<std::chrono::milliseconds>(passedTime).count() / 1000.0);
      startTime = std::chrono::high_resolution_clock::now();

      if (passedSeconds >= 60) {
        passedSeconds -= 60;
        passedMinutes += 1;
      }
    }

    //Build passed time in format 00:00.000
    std::wstring textString;
    textString += (L"Time passed: ");
    if (passedMinutes < 10)
      textString += (L"0");
    textString += std::to_wstring(passedMinutes);
    textString += (L":");
    if (passedSeconds < 10)
      textString += (L"0");
    textString += std::to_wstring(passedSeconds);
    textString.resize(22);

    // Create text format upon first use
    if (!textFormat)
      textFormat = renderInterface.CreateTextFormat(L"NI7SEG", 20, IDR_NI7SEG_FONT);

    renderInterface.DrawText(textString, textFormat, Rectangle(Vector(), size), Color::LightGoldenrodYellow);
  }

}
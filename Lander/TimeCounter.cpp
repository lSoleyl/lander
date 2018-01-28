#include "stdafx.h"
#include "TimeCounter.hpp"
#include <sstream>
#include <iomanip>

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
    passedMilliSeconds = 0;
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

      passedMilliSeconds = -1 * (std::chrono::duration_cast<std::chrono::milliseconds>(passedTime).count());
      passedSeconds = passedMilliSeconds / 1000;
      passedMilliSeconds %= 1000;
      passedMinutes = passedSeconds / 60;
      passedSeconds %= 60;
    }

    //Build passed time in format 00:00.000
    std::wstringstream textStream;
    textStream << std::setfill(L'0');
    textStream << (L"Time passed: ") << std::setw(2) << std::to_wstring(passedMinutes); // Time passed: 00
    textStream << (L":") << std::setw(2) << std::to_wstring(passedSeconds); // :00
    textStream << (L".") << std::setw(3) << std::to_wstring(passedMilliSeconds); // .000

    // Create text format upon first use
    if (!textFormat)
      textFormat = renderInterface.CreateTextFormat(L"NI7SEG", 20);

    renderInterface.DrawText(textStream.str(), textFormat, Rectangle(Vector(), size), Color::LightGoldenrodYellow);
  }

}
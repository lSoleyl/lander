#pragma once
#include <chrono>

namespace Lander {

  class TimeCounter : public OverlayObject {
  public:
    TimeCounter();

    void StartCount();
    void StopCount();
    void ResetCount();

    virtual void Initialize(Size size) override;

    virtual void Draw(RenderInterface& renderInterface, const Rectangle& visibleRect, double secondsPassed) override;

    /** Specify low render priority to be always visible
    */
    virtual int RenderPriority() const override;

  private:
    bool started = false;
    Size windowSize;

    std::chrono::high_resolution_clock::time_point startTime;
    int passedMilliSeconds = 0;
    int passedSeconds = 0;
    int passedMinutes = 0;

    RenderInterface::TextFormat textFormat; //The format used, to write the text
  };

}
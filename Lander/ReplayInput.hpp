#pragma once

#include "KeyboardInput.hpp"
#include "ViewObject.hpp"
#include "Recorder.hpp"

namespace Lander {
  /** Implements the Input interface by replaying inputs from a previously recorded save file.
   *  It inherits from the regular KeybaordInput to fallback to regular user input once the recording is finished
   *  or if the user presses escape in between
   */
  class ReplayInput : public KeyboardInput {
  public:
    ReplayInput(TCHAR* filePath);

    virtual bool IsActive(Type type) const override;

    virtual void Tick() override;

  private:
    int tick = -1;
    std::vector<Recorder::Entry> recording;
    Recorder::Entry* recordingPos = nullptr;
    Recorder::Entry* recordingEnd = nullptr;
  };


}
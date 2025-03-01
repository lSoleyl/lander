#include "stdafx.h"
#include "ReplayInput.hpp"

#include <fstream>

namespace Lander {

ReplayInput::ReplayInput(TCHAR* filePath) {
  std::ifstream file(filePath, std::ios::binary);
  if (file) {
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    recording.resize((fileSize / sizeof(Recorder::Entry)) + 1);
    file.read(reinterpret_cast<char*>(recording.data()+1), recording.size()*sizeof(Recorder::Entry));
    file.close();

    // Always start each recording with a reset input
    recording[0].inputs = static_cast<uint8_t>(Input::Reset);
    recording[0].ticks = 1;

    recordingPos = recording.data();
    recordingEnd = recordingPos + recording.size();
  } else {
    throw std::exception("Failed to open the file");
  }
}


bool ReplayInput::IsActive(Type type) const {
  if (recordingPos == recordingEnd) {
    // Recording ended -> behave like regular keyboard input
    return KeyboardInput::IsActive(type);
  }

  // return whether button was pressed accoring to the recording
  return (recordingPos->inputs & type) != 0;
}


void ReplayInput::Tick() {
  if (KeyboardInput::IsActive(Input::Reset)) {
    // disable replay input if user preses ESC
    recordingPos = recordingEnd;
  }


  if (recordingPos == recordingEnd) {
    return;
  }
  
  ++tick;
  if (tick == recordingPos->ticks) {
    // read next input
    ++recordingPos;
    tick = 0;
  }
}


}
#include "stdafx.h"
#include "Recorder.hpp"
#include "Input.hpp"

#include <chrono>
#include <fstream>

namespace Lander {

void Recorder::RecordInput(const Input& input) {
  if (stopped) {
    return;
  }

  ++ticks;
  if (ticks == std::numeric_limits<uint8_t>::max()) {
    // input held 256 ticks -> we must split the entry into two
    RecordEntry();
  }
  
  auto currentInputs = input.AllActiveInputs();
  if (currentInputs != lastInputs) {
    // Inputs changed -> record it
    RecordEntry();
    lastInputs = currentInputs;
  }
}

void Recorder::StartRecording() {
  if (stopped) {
    ticks = 0;
    lastInputs = 0;
    stopped = false;
    recording.clear();
  }
}

void Recorder::StopRecording() {
  if (!stopped) {
    ++ticks;
    RecordEntry();
    stopped = true;
  }
}


void Recorder::RecordEntry() {
  if (ticks > 0) {
    recording.push_back({ static_cast<uint8_t>(ticks), static_cast<uint8_t>(lastInputs) });
    assert(recording.back().inputs != 0xFF);
    ticks = 0;
  }
}



void Recorder::SaveReplay() {
  if (!stopped || recording.empty()) {
    return; // nothing to save
  }
  auto now = std::chrono::system_clock::now();
  auto tt = std::chrono::system_clock::to_time_t(now);
  tm tmBuf;
  localtime_s(&tmBuf, &tt);
  std::ostringstream filename;
  filename << std::put_time(&tmBuf, "saves\\%FT%H%M%S.sav");
  CreateDirectory(_T("saves"), nullptr); // create the saves directory unless it already exists

  std::ofstream file(filename.str().c_str(), std::ios::binary);
  std::vector<Entry> copy(recording.begin(), recording.end());
  file.write(reinterpret_cast<char*>(copy.data()), copy.size() * sizeof(Entry));
  file.close();
}

}
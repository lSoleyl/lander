#pragma once



namespace Lander {
class Input;
class Recorder {
public:
  /** Records the input for the current tick
   */
  void RecordInput(const Input& input);

  void StartRecording();
  void StopRecording();

  void SaveReplay();

  struct Entry {
    uint8_t ticks; // for how many ticks was the given input held down
    uint8_t inputs; // all keys fit into one byte
  };

private:
  /** Adds a new recording entry for the current input and ticks and resets the ticks value accordingly
   */
  void RecordEntry();

  bool stopped = true;
  int ticks = 0; // for how many ticks have the last inputs been held down
  int lastInputs = 0; // the last held down inputs

  std::list<Entry> recording;
};

}
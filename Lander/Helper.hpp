#pragma once

#include <sstream>

inline bool KeyPressed(int key) {
  // Don't know why, but GetAsyncKeyState() doesn't work correctly when called to often
  return (GetKeyState(key) & (1 << 7)) != 0;
}


#define OutputDebugStream(text) do { \
    std::ostringstream out; \
    out << text << std::endl; \
    OutputDebugString(out.str().c_str()); \
  } while(0)

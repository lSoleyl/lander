#include "stdafx.h"

bool KeyPressed(int key) {
  // Don't know why, but GetAsyncKeyState() doesn't work correctly when called to often
  return (GetKeyState(key) & (1 << 7)) != 0;
}

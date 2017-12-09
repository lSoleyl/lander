#include "stdafx.h"

bool KeyPressed(int key) {

  if (GetKeyState(key) & (1 << 7)) //Don't know why, but GetAsyncKeyState() doesn't work correctly when called to often
    return true;
  return false;

}
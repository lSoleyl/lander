#include "stdafx.h"
#include "KeyboardInput.hpp"


bool KeyPressed(int key) {
  // Don't know why, but GetAsyncKeyState() doesn't work correctly when called too often
  return (GetKeyState(key) & (1 << 7)) != 0;
}


bool Lander::KeyboardInput::IsActive(Type input) const {
  switch (input) {
    case Type::Reset:
      return KeyPressed(VK_ESCAPE);
      
    case Type::Thrust:
      return KeyPressed(VK_SPACE) || KeyPressed(VK_UP);

    case Type::RollLeft:
      return KeyPressed(VK_LEFT) && !KeyPressed(VK_RIGHT);

    case Type::RollRight:
      return KeyPressed(VK_RIGHT) && !KeyPressed(VK_LEFT);
  }

  // only for unsupported inputs (which shouldn't exist)
  return false;
}

int Lander::KeyboardInput::AllActiveInputs() const {
  int result = 0;

  for (auto input : { Type::Reset, Type::Thrust, Type::RollLeft, Type::RollRight }) {
    if (IsActive(input)) {
      result |= input;
    }
  }

  return result;
}


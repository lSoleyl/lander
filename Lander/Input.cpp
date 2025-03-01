#include "stdafx.h"
#include "Input.hpp"

int Lander::Input::AllActiveInputs() const {
  int result = 0;

  for (auto input : { Type::Reset, Type::Thrust, Type::RollLeft, Type::RollRight, Type::SaveReplay }) {
    if (IsActive(input)) {
      result |= input;
    }
  }

  return result;
}

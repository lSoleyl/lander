#include "stdafx.h"
#include "HoverAIInput.hpp"

extern bool KeyPressed(int key);

namespace Lander {


HoverAIInput::HoverAIInput(Rocket& rocket) : rocket(rocket) {}

bool HoverAIInput::IsActive(Type type) const {
  if (type == Type::Thrust) {
    if (rocket.pos.y > 300) {
      if (rocket.velocity.y < 0) { // rocket is moving up -> feather thrust
        // Simulate feathering thrust by randomly returning true with a decreasing chance the closer we get
        auto thrustChance = std::min<int>(rocket.pos.y - 300, 200) / 200.0;
        return (rand() * 1.0 / RAND_MAX <= thrustChance);
      } else {
        return true; // prevent rocket from moving down
      }
    }
  }

  if (type == Type::Reset) {
    return KeyPressed(VK_ESCAPE);
  }

  return false;
}



}
#pragma once

#include "Input.hpp"
#include "Rocket.hpp"

namespace Lander {

  /** Implements the Input interface by hovering the rocket in mid air
   */
  class HoverAIInput : public Input {
  public:
    HoverAIInput(Rocket& rocket);
    virtual bool IsActive(Type type) const override;

  private:
    Rocket& rocket;
  };


}
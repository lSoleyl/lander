#pragma once

#include "Input.hpp"

namespace Lander {

/** Implements the input interface by checking keyboard key press states
 *  ESC, UP, SPACE, LEFT, RIGHT
 */
class KeyboardInput : public Input {
public:
  virtual bool IsActive(Type type) const override;
};


}
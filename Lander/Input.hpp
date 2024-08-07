#pragma once

namespace Lander {

/** Basic input interface abstracting away concrete key presses into input
 */
class Input {
public:
  /** The input values are defined in a way to make them combinable as a bitmask
   */
  enum Type : int {
    Reset     = 0x01, // Esc key
    Thrust    = 0x02, // Up or Space key
    RollLeft  = 0x04, // Left key
    RollRight = 0x08  // Right key
  };

  /** Checks whether a specific input is made
   */
  virtual bool IsActive(Type type) const = 0;

  /** Returns a combination of all currently made inputs in a single number.
   *  Use the Input::Type values to check for single inputs.
   */
  virtual int AllActiveInputs() const = 0;
};


}

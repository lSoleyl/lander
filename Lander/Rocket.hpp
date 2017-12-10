#pragma once
#include "FuelTank.hpp"

namespace Lander {
class Platform;
class Rocket : public PhysicsObject {
public:
  Rocket(const Platform& startPlatform);

  /** Update method used to adapt own position to the platforms's position if it changes.
   *  This is only necessary until the rocket receives it's first user input (thrust)
   */
  virtual void PhysicsUpdate(double secondsSinceLastFrame) override;

  virtual void Draw(RenderInterface& renderTarget, double secondsSinceLastFrame) override;

private:
  /** Handle collisions
   */
  virtual void OnCollision(Collider& collider) override;

  /** Moves the rocket to it's start position
   */
  void Reposition();

  const Platform& startPlatform;
  int trailIndex = 0;

  double secondsSinceLastAnimation = 0;

  const float verticalAcceleration = 15; // m/s�
  const float angularAcceleration = 10;  // �/s�

  const float baseMass = 14109.6f; //kg - The rocket's base mass without the mass of the fuel tanks and the fuel itself.

  FuelTank Tank;

  enum class STATE {LANDED, STARTED, CRASHED};
  STATE state = STATE::LANDED;
};


}
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
  /** Checks whether this object collides with any Collider of this game.
   *
   * @returns true if the collision check has detected a collision with an object (hasCollision is true)
   */
  bool CheckCollisions();

  const Platform& startPlatform;
  int trailIndex = 0;
  bool thrustCheck = true; //While this is true, the rocket will reposition itself onto the platform. This will be set to false by adding thrust.
  bool hasCollision = false; //Will be set to true by CollisionCheck() if it detects a collision with another collider

  double secondsSinceLastAnimation = 0;

  const float verticalAcceleration = 15; // m/s�
  const float angularAcceleration = 10;  // �/s�

  const float baseMass = 14109.6f; //kg - The rocket's base mass without the mass of the fuel tanks and the fuel itself.

  FuelTank Tank;
};


}
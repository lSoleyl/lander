#include "stdafx.h"
#include "PhysicsObject.hpp"

namespace Lander {

// The average rocket is 50m height, ours is 100px height...
const float PhysicsObject::PIXEL_PER_METER = 2;


PhysicsObject::PhysicsObject() : angularVelocity(0), angularAcceleration(0), mass(0) {}


void PhysicsObject::Update(double secondsSinceLastFrame) {
  PhysicsUpdate(secondsSinceLastFrame);
  const float secondsPassed = static_cast<float>(secondsSinceLastFrame);
  
  // Apply acceleration (calculate velocities at end of frame)
  velocity += acceleration * secondsPassed;
  angularVelocity += angularAcceleration * secondsPassed;

  // To update the position correctly we must calculate the average velocity of this frame
  // and not just take the updated velocity because we just reached that velocity at the end of the frame.
  auto avgVelocity = velocity - (acceleration * (secondsPassed / 2));
  auto avgAngularVelocity = angularVelocity - (angularAcceleration * (secondsPassed / 2));

  // Apply velocity
  pos += avgVelocity * secondsPassed * PIXEL_PER_METER;
  rotation += avgAngularVelocity * secondsPassed;
  
  // Reset acceleration values
  acceleration = Vector::Zero;
  angularAcceleration = 0;
}

void PhysicsObject::Stop() {
  velocity = Vector::Zero;
  acceleration = Vector::Zero;
  angularVelocity = 0;
  angularAcceleration = 0;
}


void PhysicsObject::ApplyAcceleration(Vector acceleration) {
  this->acceleration += acceleration;
}

void PhysicsObject::ApplyAngularAcceleration(float angularAcceleration) {
  this->angularAcceleration += angularAcceleration;
}

void PhysicsObject::ApplyForce(Vector force) {
  assert(mass != 0); // Cannot apply a force to an object without mass
  acceleration += force / mass; // F = m*a;  a = F/m
}

void PhysicsObject::ApplyGravity(Vector direction) {
  this->acceleration += direction * 9.81f;
}


}

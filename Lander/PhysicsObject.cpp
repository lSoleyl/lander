#include "stdafx.h"
#include "PhysicsObject.hpp"

namespace Lander {

// The average rocket is 50m height, ours is 100px height...
const float PhysicsObject::PIXEL_PER_METER = 2;


PhysicsObject::PhysicsObject() : angularVelocity(0), angularAcceleration(0), mass(0) {}


void PhysicsObject::Update(double secondsSinceLastFrame) {
  PhysicsUpdate(secondsSinceLastFrame);  
  const float secondsPassed = static_cast<float>(secondsSinceLastFrame);
  
  //Apply acceleration
  velocity += acceleration * secondsPassed;
  angularVelocity += angularAcceleration * secondsPassed;

  //Apply velocity
  pos += velocity * secondsPassed * PIXEL_PER_METER;
  rotation += angularVelocity * secondsPassed;
  
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
  assert(mass != 0); //Cannot apply a force to an object without mass
  acceleration += force / mass; // F = m*a;  a = F/m
}

void PhysicsObject::ApplyGravity(Vector direction) {
  this->acceleration += direction * 9.81f;
}


}

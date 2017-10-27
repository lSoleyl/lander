#pragma once

namespace Lander {

/** This class implements an extended view object, which has a mass, a linear and angular velocitiy to which
 *  accelerations and forces can be applied.
 */
class PhysicsObject : public ViewObject {
public:
  PhysicsObject();

  /** This constant defines a conversion ratio to convert between pixels and meters.
   */
  static const float PIXEL_PER_METER;

  /** This method applies the defined accelerations to the current velocities 
   *  and applies the velocities to the current position of the object.
   *  The method is declared final to not override this behavior. To implement an
   *  update method, override the PhysicsUpdate() method, which gets called from within 
   *  this method.
   */
  virtual void Update(double secondsSinceLastFrame) override final;

  /** This function should be overwritten by any class, deriving from this one to apply
   *  forces, change velocitiy, ...
   */
  virtual void PhysicsUpdate(double secondsSinceLastFrame) {}

  ///////
  ///  Physics functions
  ///////

  /** Applies the given acceleration (in m/s²) to the current object at the end of the 
   *  Update() call. After applying the acceleration to the velocity, it gets reset back to 0.
   *  To create a constant acceleration, ApplyAcceleration() must be called in each PhysicsUpdate() call.
   *
   * @param acceleration the accelartion to apply (m/s²)
   */
  void ApplyAcceleration(Vector acceleration);

  /** Applies the given angular acceleration (in °/s²) to the current object at the end of the 
   *  Update() call. 
   *
   * @param angularAcceleration the clockwise angular acceleration to apply to this object (°/s²)
   */
  void ApplyAngularAcceleration(float angularAcceleration);

  /** Similar to ApplyAcceleration, but also takes into account the object's mass. For this to work, the
   *  must must be assigned with a non zero mass.
   * 
   * @param force the force to apply (N)
   */
  void ApplyForce(Vector force);



  /////
  //  Physical state
  ///// 
  Vector velocity;        // m/s
  float  angularVelocity; // °/s

  Vector acceleration;        // m/s² 
  float  angularAcceleration; // m/s²

  float mass; // kg
};

}

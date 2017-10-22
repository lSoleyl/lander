#pragma once

namespace Lander {

class Platform;
class Rocket : public ViewObject {
public:
  Rocket(const Platform& startPlatform);

  /** Update method used to adapt own position to the platforms's position if it changes.
   *  This is only necessary until the rocket receives it's first user input (thrust)
   */
  virtual void Update(double secondsSinceLastFrame) override;

  virtual void Draw(RenderInterface& renderTarget, double secondsSinceLastFrame) override;

private:
  const Platform& startPlatform;
  int trailIndex = 0;
  bool thrustCheck = true; //While this is true, the rocket will reposition itself onto the platform. This will be set to false by adding thrust.

  double secondsSinceLastAnimation = 0;

  const double verticalSpeed = 150;  //Speed of the rocket when thrusting in pixels/second
  const double angularSpeed = 60;    //Rotation speed of the rocket when turning in degrees/second
};


}
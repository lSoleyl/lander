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
  Size trailSize;
  int trailAlternateSize[2];
  int index = 0;
  Vector trailVector;
  bool thrustCheck = true; //While this is true, the rocket will reposition itself onto the platform. This will be set to false by adding thrust.

  double secondsSinceLastUpdate = 0;
  double secondsSinceLastAnimation = 0;
  double rocketSpeedValue = 0.001;  //the smaller the number, the faster the Rocket will fly. However, the speed is still caped by the frame rate.
};


}
#pragma once

namespace Lander {

class Terrain;

/** This class represents one of the platforms for the rocket (start/target)
 */
class Platform : public Collider {
public:
  Platform(const Terrain& terrain, float xPos);

  /** Update method used to adapt own position to the terrain's position if necessary
   */
  virtual void Update(double secondsSinceLastFrame) override;


  virtual void Draw(RenderInterface& renderTarget, const Rectangle& visibleRect, double secondsSinceLastFrame) override;

private:
  float xPos;
  const Terrain& terrain;
};




}
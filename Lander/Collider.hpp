#pragma once

namespace Lander {

/** This class represents all objects, which take place in collision detection.
  */
class Collider : public ViewObject {
public:

  /** This method can be called on any collider to check whether a given point is 
   *  inside the collider object. The point should be given as vector in world coordinates.
   *  
   * @param point the point in world coordinates.
   */
  virtual bool IsPointInside(Vector point) const;
};


}

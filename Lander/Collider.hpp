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

protected:
  /** Checks whether this object collides with any Collider of this game.
   *  The check is implemented simply as checking 8 points of the bounding box to be inside
   *  the other collider. If both objects collide, OnCollision() gets called with that collider.
   *  This works only for objects of rectangular shape. If this object has a different shape, CheckCollision()
   *  should be overwritten in that class.
   */
  virtual void CheckCollisions();

private:
  /** Gets called in CheckCollisions() for each collider, this object intersects with.
   *
   * @param collider the collider, this object intersects with.
   */
  virtual void OnCollision(Collider& collider) {}
};


}

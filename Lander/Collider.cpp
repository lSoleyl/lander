#include "stdafx.h"
#include "Game.hpp"

#include <array>

namespace Lander {

//check whether the point is inside the rectangle of this object (pos,size)
//this check can be skipped if the point is outside this object's radius.
bool Collider::IsPointInside(Vector worldPoint) const {

  Vector objectPoint = WorldToObject(worldPoint);
  Rectangle collRec(Vector::Zero, size);

  Vector topLeft = collRec.topLeft;
  Vector bottomLeft = collRec.BottomLeft();
  Vector topRight = collRec.TopRight();

  if ((topLeft.x <= objectPoint.x && objectPoint.x <= topRight.x) && (topLeft.y <= objectPoint.y && objectPoint.y <= bottomLeft.y)) //Point is inside this objects unrotated-rectangle
    return true;

  return false;
}


void Collider::CheckCollisions() {
  Rectangle rect(Vector::Zero, size);

  std::array<Vector, 8> collisionPoints;

  collisionPoints[0] = ObjectToWorld(rect.topLeft);
  collisionPoints[1] = ObjectToWorld(rect.TopCenter());
  collisionPoints[2] = ObjectToWorld(rect.TopRight());
  collisionPoints[3] = ObjectToWorld(rect.LeftCenter());
  collisionPoints[4] = ObjectToWorld(rect.RightCenter());
  collisionPoints[5] = ObjectToWorld(rect.BottomLeft());
  collisionPoints[6] = ObjectToWorld(rect.BottomCenter());
  collisionPoints[7] = ObjectToWorld(rect.bottomRight);

  for(auto collider : Game::Instance()->GetColliders()) {
    if (collider != this) { //ignore the object itself

      Vector distance = ObjectToWorld(this->Center()) - ObjectToWorld(collider->Center()); //Vector connecting both objects center
      float totalRadius = Center().Length() + collider->Center().Length();

      if (totalRadius > distance.Length()) { //if distance is smaller than the object's radiuses: collision is possible
        
        //check whether one of the eight points of the rectangle is inside the other rectangle. (if so, call OnCollision())
        if (std::any_of(collisionPoints.begin(), collisionPoints.end(), [collider](const Vector& point) { 
          return collider->IsPointInside(point);
        })) {
          OnCollision(*collider);
        }
      }
    }  
  }
}

}

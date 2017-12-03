#include "stdafx.h"

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


}

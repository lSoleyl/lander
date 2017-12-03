#include "stdafx.h"

namespace Lander {

Vector ViewObject::WorldToObject(Vector worldVector) const {
  // We have to first subtract the object's offset and then rotate back around it's center
  return (worldVector - pos).Rotate(-rotation, Center());
}

  
Vector ViewObject::ObjectToWorld(Vector objectVector) const {
  // We have to apply the object's rotation around the center to the given vector and then add the object's offset
  return objectVector.Rotate(rotation, Center()) + pos;
}

Vector ViewObject::Center() const {
  return Vector::Right * size.width/2 + Vector::Down * size.height/2;
}

double ViewObject::VectorValue(Vector v) const {

  return sqrt(pow(v.y, 2) + pow(v.x, 2)); //Get Vector's value through sentence of the pythagoras

}

}

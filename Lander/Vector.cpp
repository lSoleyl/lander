#include "stdafx.h"

namespace {
  const float PI = 3.14159265359f;
}

namespace Lander {

const Vector Vector::Zero(0,0);
const Vector Vector::Up(0,-1); // Viewport's vertical axis is reversed
const Vector Vector::Down(0,1);
const Vector Vector::Left(-1,0);
const Vector Vector::Right(1,0);

Vector::Vector() : x(0), y(0) {}
Vector::Vector(float x, float y) : x(x), y(y) {}
Vector::Vector(const Vector& other) : x(other.x), y(other.y) {}

Vector Vector::Rotate(float angle) const {  // Rotate vector according to the angle

  float radAngle = angle*PI / 180;
  return Vector((x * cos(radAngle)) - (y * sin(radAngle)),
                (x * sin(radAngle)) + (y * cos(radAngle)));
}

Vector Vector::Rotate(float angle, Vector rotationPoint) const {
  return (*this - rotationPoint).Rotate(angle) + rotationPoint;
}

Vector Vector::Rotate90CW() const {
  return Vector(-y, x);
}

Vector Vector::Rotate90CCW() const {
  return Vector(y, -x);
}

Vector Vector::Rotate180() const {
  return Vector(-x, -y);
}


float Vector::AngleBetween(Vector other) const {
  return acosf(*this * other / (Length() * other.Length())) / PI * 180;
}


float Vector::AngleTo(Vector other) const {
  // We project other onto this vector rotate 90� clockwise and check wether
  // the scalar product is positive or negative to determine whether other is right or
  // left of this vector and thus whether to return a positive or negative angle
  return ((Rotate90CW() * other > 0) ? 1 : -1) * AngleBetween(other);
}


Vector& Vector::operator=(const Vector& other) {
  x = other.x;
  y = other.y;
  return *this;
}

Vector& Vector::operator+=(const Vector& other) {
  x += other.x;
  y += other.y;
  return *this;
}

Vector& Vector::operator-=(const Vector& other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

Vector& Vector::operator*=(float factor) {
  x *= factor;
  y *= factor;
  return *this;
}

Vector& Vector::operator/=(float divisor) {
  return *this *= (1 / divisor);
}

Vector Vector::operator+(const Vector& other) const {
  return Vector(x+other.x, y+other.y);
}
  
Vector Vector::operator-(const Vector& other) const {
  return Vector(x-other.x, y-other.y);
}
  
Vector Vector::operator*(float factor) const {
  return Vector(x*factor, y*factor);
}

Vector Vector::operator/(float divisor) const {
  return *this * (1/divisor);
}

float Vector::operator*(const Vector& other) const {
  return x * other.x + y * other.y;
}

float Vector::Length() const {
  return sqrt(x*x + y*y);
}

Vector operator*(float factor, const Vector& v) {
  return v * factor;
}

Vector::operator D2D1_POINT_2F() const {
  return D2D1::Point2F(x,y);
}

Vector Vector::FromSize(const Size& size) {
  return Vector(size.width, size.height);
}

Size::Size() : width(0), height(0) {}
Size::Size(float width, float height) : width(width), height(height) {}
Size::Size(const Size& other) : width(other.width), height(other.height) {}
Size::Size(const D2D1_SIZE_F& other) : width(other.width), height(other.height) {}

Size Size::Abs() const {
  return Size(abs(width), abs(height));
}

Rectangle::Rectangle() {}
Rectangle::Rectangle(Vector topLeft, Vector bottomRight) : topLeft(topLeft), bottomRight(bottomRight) {}
Rectangle::Rectangle(Vector topLeft, vec::Size size) : topLeft(topLeft), bottomRight((Vector::Right * size.width) + (Vector::Down * size.height) + topLeft) {}
Rectangle::Rectangle(const RECT& rc) : topLeft(static_cast<float>(rc.left), static_cast<float>(rc.top)), bottomRight(static_cast<float>(rc.right),static_cast<float>(rc.bottom)) {}
Rectangle::Rectangle(const Rectangle& other) : topLeft(other.topLeft), bottomRight(other.bottomRight) {}


Rectangle::operator D2D1_RECT_F() const {
  return D2D1::RectF(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
}

Size Rectangle::Size() const {
  return vec::Size(bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);
}

Vector Rectangle::TopRight() const {
  return Vector(bottomRight.x, topLeft.y);
}

Vector Rectangle::BottomLeft() const {
  return Vector(topLeft.x, bottomRight.y);
}

Vector Rectangle::Center() const {
  return Vector((bottomRight.x + topLeft.x) / 2, (topLeft.y + bottomRight.y) / 2);
}

Vector Rectangle::TopCenter() const {
  return Vector(Center().x, topLeft.y);
}



Vector Rectangle::RightCenter() const {
  return Vector(bottomRight.x, Center().y);
}
 
Vector Rectangle::BottomCenter() const {
  return Vector(Center().x, bottomRight.y);
}
  
Vector Rectangle::LeftCenter() const {
  return Vector(topLeft.x, Center().y);
}

void Rectangle::operator+=(Vector offset) {
  topLeft += offset;
  bottomRight += offset;
}

bool Rectangle::Contains(Vector pos) const {
  // Our check is based on the following assumption about the coordinate system
  assert(Vector::Up.y == -1 && Vector::Right.x == 1);
  return pos.x >= topLeft.x && pos.x <= bottomRight.x && pos.y >= topLeft.y && pos.y <= bottomRight.y;
}

bool Rectangle::Contains(const Rectangle& other) const {
  return Contains(other.topLeft) && Contains(other.bottomRight) && Contains(other.TopRight()) && Contains(other.BottomLeft());
}

bool Rectangle::Intersects(const Rectangle& other) const {
  return Contains(other.topLeft)
      || Contains(other.bottomRight)
      || Contains(other.TopRight())
      || Contains(other.BottomLeft());
}

}
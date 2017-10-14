#include "stdafx.h"

namespace Lander {

const Vector Vector::Zero(0,0);
const Vector Vector::Up(0,-1); //Viewport's vertical axis is reversed
const Vector Vector::Down(0,1);
const Vector Vector::Left(-1,0);
const Vector Vector::Right(1,0);

Vector::Vector() : x(0), y(0) {}
Vector::Vector(float x, float y) : x(x), y(y) {}
Vector::Vector(const Vector& other) : x(other.x), y(other.y) {}

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
Rectangle::Rectangle(const RECT& rc) : topLeft(rc.left, rc.top), bottomRight(rc.right, rc.bottom) {} 
Rectangle::Rectangle(const Rectangle& other) : topLeft(other.topLeft), bottomRight(other.bottomRight) {}


Rectangle::operator D2D1_RECT_F() const {
  return D2D1::RectF(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
}

Size Rectangle::Size() const {
  return vec::Size(bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);
}

}
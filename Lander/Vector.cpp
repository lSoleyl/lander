#include "stdafx.h"

Vector::Vector(float x, float y) : x(x), y(y) {}
Vector::Vector(const Vector& other) : x(other.x), y(other.y) {}
Vector::Vector(Vector&& other) : x(other.x), y(other.y) {}

Vector& Vector::operator=(const Vector& other) {
  x = other.x;
  y = other.y;
  return *this;
}

Vector& Vector::operator=(Vector&& other) {
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
  return (*this += other * -1);
}

Vector& Vector::operator*=(float factor) {
  x *= factor;
  y *= factor;
  return *this;
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

float Vector::Length() const {
  return sqrt(x*x + y*y);
}

Vector operator*(float factor, const Vector& v) {
  return v * factor;
}

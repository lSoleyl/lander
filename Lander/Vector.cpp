#include "stdafx.h"


const Vector Vector::Zero(0,0);
const Vector Vector::Up(0,-1); //Viewport's vertical axis is reversed
const Vector Vector::Down(0,1);
const Vector Vector::Left(-1,0);
const Vector Vector::Right(1,0);

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

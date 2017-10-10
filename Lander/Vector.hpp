#pragma once
 
/** This class represents a 2D Float vector
 */
class Vector {
public:

  Vector(float x, float y); //A vector (x, y)
  Vector(const Vector& other);
  Vector(Vector&& other);

  Vector& operator=(const Vector& other);
  Vector& operator=(Vector&& other);

  //Mathematical operations
  Vector& operator+=(const Vector& other);
  Vector& operator-=(const Vector& other);
  Vector& operator*=(float factor);
  Vector& operator/=(float divisor);

  Vector operator+(const Vector& other) const;
  Vector operator-(const Vector& other) const;
  Vector operator*(float factor) const;
  Vector operator/(float divisor) const;


  //Vector length
  float Length() const;

  float x,y;

  //Constant vectors
  static const Vector Zero;
  static const Vector Up;
  static const Vector Down;
  static const Vector Left;
  static const Vector Right;
};

Vector operator*(float factor, const Vector& v);
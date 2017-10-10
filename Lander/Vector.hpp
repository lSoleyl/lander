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

  Vector operator+(const Vector& other) const;
  Vector operator-(const Vector& other) const;
  Vector operator*(float factor) const;


  //Vector length
  float Length() const;

  float x,y;
};

Vector operator*(float factor, const Vector& v);
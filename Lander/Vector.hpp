#pragma once
 
namespace Lander {

class Size;
class Rectangle;

/** This class represents a 2D Float vector
 */
class Vector {
public:
  Vector(); // (0,0)
  Vector(float x, float y); //A vector (x, y)
  Vector(const Vector& other);

  Vector& operator=(const Vector& other);

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

  float x, y;


  //Conversion from a size 
  static Vector FromSize(const Size& size);

  //Constant vectors
  static const Vector Zero;
  static const Vector Up;
  static const Vector Down;
  static const Vector Left;
  static const Vector Right;
};

Vector operator*(float factor, const Vector& v);

/** A size class to represent sizes. We could use a vector for this too, but it would be a bit confusing, because
 *  when describing a rectangle with two vectors it would be unclear, whether it the second vector is the bottom right position
 *  or the width and height.
 */
class Size {
public:
  Size(); //width=0, height=0
  Size(float width, float height);
  Size(const Size& other);
  Size(const D2D1_SIZE_F& other); //conversion from Direct2D type


  float width, height;
};

/** This class represents a rectangle... we have to call it Rect because WinApi already defines a function Rectangle()
 */
class Rectangle {
public:
  Rectangle(); //(0,0) - (0,0)
  Rectangle(Vector topLeft, Vector bottomRight);
  Rectangle(Vector topLeft, Size size);
  Rectangle(const Rectangle& other);

  /** Implicit conversion to Direct2D type 
   */
  operator D2D1_RECT_F() const;

  Vector topLeft, bottomRight;
};

  //Namespace with typedefs to resolve namespace conflicts by writing vec::Size instead of Size
  namespace vec {
    typedef Lander::Vector Vector;
    typedef Lander::Size Size;
    typedef Lander::Rectangle Rectangle;
  }
}
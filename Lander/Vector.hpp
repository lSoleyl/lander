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

  /** Rotates the vector around (0,0) with the given angle (in degrees)
   *  positive angles correspond to a clockwise rotation
   */
  Vector Rotate(float angle) const;

  /** Rotates the vector around the given rotation point
   */
  Vector Rotate(float angle, Vector rotationPoint) const;

  /** Rotates the vector by 90° clockwise 
   */
  Vector Rotate90CW() const;

  /** Rotates the vector by 90° counter clockwise
   */
  Vector Rotate90CCW() const;

  /** Rotates the vector by 180°
   */
  Vector Rotate180() const;

  /** Returns the smallest angle (in degrees) between the two vectors, which will always be positive
   *  None of them must be the Zero vector
   */
  float AngleBetween(Vector other) const;

  /** Returns the angle in degrees, this vector needs to be rotated (positive = clockwise, negative = counterclockwise)
   *  for it to point into the same direction as other.
   */
  float AngleTo(Vector other) const;

  Vector& operator=(const Vector& other);

  // Mathematical operations
  Vector& operator+=(const Vector& other);
  Vector& operator-=(const Vector& other);
  Vector& operator*=(float factor);
  Vector& operator/=(float divisor);

  Vector operator+(const Vector& other) const;
  Vector operator-(const Vector& other) const;
  Vector operator*(float factor) const;
  Vector operator/(float divisor) const;

  /** Scalar product */
  float operator*(const Vector& other) const;


  // Vector length
  float Length() const;

  float x, y;

  // Implicit conversion to a Direct2D point
  operator D2D1_POINT_2F() const;

  // Conversion from a size 
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

  /** Returns a size with absolute values (no negative ones)
   */
  Size Abs() const;

  float width, height;
};

/** This class represents a rectangle... we have to call it Rect because WinApi already defines a function Rectangle()
 */
class Rectangle {
public:
  Rectangle(); //(0,0) - (0,0)
  Rectangle(Vector topLeft, Vector bottomRight);
  Rectangle(Vector topLeft, Size size);
  Rectangle(const RECT& winRect);
  Rectangle(const Rectangle& other);

  /** Implicit conversion to Direct2D type 
   */
  operator D2D1_RECT_F() const;

  /** Returns the width&height of the rectangle
   */
  Size Size() const;

  Vector TopRight() const;
  Vector BottomLeft() const;
  Vector Center() const;

  Vector TopCenter() const;
  Vector RightCenter() const;
  Vector BottomCenter() const;
  Vector LeftCenter() const;

  /** Moves the whole rectangle by the specified vector offset
   */
  void operator+=(Vector offset);

  /** True if the specified position is inside this rectangle
   */
  bool Contains(Vector pos) const;
  /** True if this rectangle contains all corner points of the given rectangle
   */
  bool Contains(const Rectangle& other) const;
  bool Intersects(const Rectangle& other) const;
  

  Vector topLeft, bottomRight;
};

  //Namespace with typedefs to resolve namespace conflicts by writing vec::Size instead of Size
  namespace vec {
    typedef Lander::Vector Vector;
    typedef Lander::Size Size;
    typedef Lander::Rectangle Rectangle;
  }
}
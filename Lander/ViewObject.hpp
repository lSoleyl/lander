#pragma once

class RenderInterface;

/** Each object, which wants to be drawn must be derived from this class
 */
class ViewObject {
public:
  /** Gets called once on each view object before the object gets drawn and after the game has initialized to 
   *  initialize the view object's resources.
   */
  virtual void Initialize() {}

  /** This function gets called once per frame to 
   */
  virtual void Draw(RenderInterface& renderTarget, double secondsSinceLastFrame) = 0;
  
  /** All ViewObjects in the render queue are ordered in descending order by the render priority
   *  Returning a high priority value means that the object is drawn first. 0 is the default priority.
   */
  virtual int RenderPriority() const { return 0; }

  //TODO replace this with own types
  virtual D2D1_POINT_2F Pos() { return D2D1::Point2F(); }
  virtual D2D1_SIZE_F Size() { return D2D1::SizeF(); }


  /** This function gets called upon the game's destruction. Dynamically allocated ViewObjects
   *  can use this call to delete themselves. They are not being referenced by the game object after this call.
   */
  virtual void Deinitialize() {}
};
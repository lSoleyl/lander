#pragma once

namespace Lander {

class RenderInterface;
class Camera;

/** Each object, which wants to be drawn must be derived from this class
 */
class ViewObject {
public:
  /** Gets called once on each view object before the object gets drawn and after the game has initialized to 
   *  initialize the view object's resources.
   *
   * @param size the renderTarget's size to which an object can adapt it's own size if necessary
   */
  virtual void Initialize(Size size) {}

  /** Gets called on each frame before draw. Changes in size and position of the object are reflected in the next draw call
   */
  virtual void Update(double secondsSinceLastFrame) {}

  /** This function gets called once per frame to let the object draw itself onto the screen using the game renderer
   *  The gameRender already applied transform matricies to the render target so that all draw calls should be made in 
   *  coordinates relative to this ViewObject.
   * 
   * @param renderTarget perform your draw calls on this object
   * @param visibleRect the visible rectangle in world coordinates. This may be used to skip drawing altogether if the object is not visible.
   * @param secondsSinceLastFrame how much time passed since rendering the last frame.
   */
  virtual void Draw(RenderInterface& renderTarget, const Rectangle& visibleRect, double secondsSinceLastFrame) = 0;
  
  /** All ViewObjects in the render queue are ordered in descending order by the render priority
   *  Returning a high priority value means that the object is drawn first. 0 is the default priority.
   */
  virtual int RenderPriority() const { return 0; }

  /** Return true to automatically draw the object's bounding box before calling Draw() on it.
   *  The default implementation draws the bounding box when compiling with Debug.
   */
  virtual bool DrawBoundingBox() { 
#ifdef _DEBUG
    return true;
#else 
    return false;
#endif
  }

  /** Transforms a vector from a (0,0) position relative to the top left of the screen into a vector, where 
   *  (0,0) is the top left of the view object (with applied rotation).
   * 
   * @param worldVector the vector to transform
   */
  Vector WorldToObject(Vector worldVector) const;

  /** Transforms a vector from a (0,0) position relative to the top left of this view object into a vector, where (0,0)
   *  is the top left of the screen.
   * 
   * @param objectVector the vector to transform
   */
  Vector ObjectToWorld(Vector objectVector) const;

  /** Returns the object's center position in object coordinates, relative to the top left corner.
   *  To get center in world coordinates, simply add pos to it.
   */
  Vector Center() const;


  /** Returns the screen coordinates (render interface coordinates) after applying the camera's 
   *  coordinate transformation.
   */
  virtual Vector GetScreenPosition(const Camera& camera) const;



  /** This function gets called upon the game's destruction. Dynamically allocated ViewObjects
   *  can use this call to delete themselves. They are not being referenced by the game object after this call.
   */
  virtual void Deinitialize() {}

  /** State of the view object position and size can be changed in Update() to be reflected in the next Draw().
   *  All drawing positions are relative to pos. pos is the top-left position of the view object in world-coordinates.
   */
  vec::Vector pos;
  vec::Size size;
  float rotation = 0; //rightward rotation angle (degrees) around the object's center point of the view object

  bool enabled = true; //if set to false, the Update() and Draw() functions won't be called anymore. Drawn with a red bounding box
  bool visible = true; //if set to false the Draw() function won't be called anymore. Drawn with a magenta bounding box
};

}
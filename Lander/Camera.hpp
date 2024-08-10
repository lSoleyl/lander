#pragma once

namespace Lander {

/** This class represents the camera pointing at the currently visible part of the game field
 *  and is used by the GameRenderer to scroll the field of view and could also be used to later 
 *  zoom the visible field of view.
 */
class Camera {
public:
  /** Initialize the camera with the visible rectangle in screen, which initially also equal world coordinates.
   *  This rectangle will be modified by moving the camera.
   */
  Camera(Rectangle screenRect);
   
  /** Converts the given vector from world coordinates into screen coordinates by applying the camera's 
   *  movement.
   */
  Vector WorldToScreen(Vector worldVector) const;


  const Rectangle& GetVisibleRect() const;

  /** The camera will track the given view object to always keep it in view.
   */
  void TrackObject(ViewObject& object, double secondsSinceLastUpdate);


 private:
   /** The currently rectangle in world coordinates, which is currently visible through the camera.
    */
   Rectangle visibleRect;
   const Rectangle screenRect;
};

}
#pragma once

#include "ViewObject.hpp"

namespace Lander {


/** This class represents object, which are lying on top of the camera and whose position should always be interpreted in 
 *  screen coordinates rather than world coordinates.
 */
class OverlayObject : public ViewObject {
  /** Overlay objects perform no coordinate transformation depending on the camera position
   */
  virtual Vector GetScreenPosition(const Camera& camera) const override { return pos; }
};



}
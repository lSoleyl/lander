#pragma once

/** This objects of this class can be used by ViewObjects to draw themselves. They are
 *  only valid within the current Render() function.
 */
class RenderInterface {
public:
  /** This method returns the native render target, used internally if the RenderInterface doen't provide enough functionality
   *  This should not be used if not really necessary.
   */
  virtual ID2D1RenderTarget& RenderTarget() = 0;

  /** Returns the size of the render target.
   */
  virtual D2D1_SIZE_F Size() = 0; //TODO use own size/point classes
};
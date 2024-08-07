#pragma once

// undef winapi macro to be able to jump to the correct definition for DrawText()
#undef DrawText

namespace Lander {

/** Typedef for simpler access to colors
 */
typedef D2D1::ColorF::Enum Color;

/** This objects of this class can be used by ViewObjects to draw themselves. They are
 *  only valid within the current Render() function.
 */
class RenderInterface {
public:
  typedef int TextFormat;


  /** This method returns the native render target, used internally if the RenderInterface doesn't provide enough functionality
   *  This should not be used if not really necessary.
   */
  virtual ID2D1RenderTarget& RenderTarget() = 0;

  /** Returns the size of the render target.
   */
  virtual Size Size() = 0;

  /** Creates a new text format with the given arguments, or returns an already existing format, which has been created with the same parameters.
   *  If a font resource file has been registered for this fontName, the font should be loaded from the resource file. Otherwise the
   *  system font library should be used.
   *
   * @param fontName the name of the font to create
   * @param fontSize the size of the text format to create
   *
   * @return a new TextFormat != NULL if successful
   */
  virtual TextFormat CreateTextFormat(const wchar_t* fontName, float fontSize) = 0;

  /** Draws a text into the specified area with the previously created text format (CreateTextFormat)
   *
   * @param text the text to draw
   * @param format the text format (font,size) which has been previously created with CreateTextFormat()
   * @param targetRect the rectangle to draw the text into
   * @param color the color to draw the text in
   */
  virtual void DrawText(const std::wstring& text, TextFormat format, Rectangle targetRect, Color color) = 0;


  /** Draws a line between the specified two points.
   *
   * @param from the start point of the line
   * @param to the end point of the line
   * @param color the color to draw the line with
   * @param strokeWidth the thickness of the line
   */
  virtual void DrawLine(Vector from, Vector to, Color color, float strokeWidth = 1.0f) = 0;

  /** Draws a rectangle (only the borders) of the specified color into the given target rectangle.
   *
   * @param target the rectangle to draw into
   * @param color the color of the rectangle
   * @param strokeWidth the thickness of the line
   */
  virtual void DrawRectangle(Rectangle target, Color color, float strokeWidth = 1.0f) = 0;

  /** Fills the target rectangle with the specified color
   *
   * @param target the rectangle to fill
   * @param color the color to fill the rectangle with
   */
  virtual void FillRectangle(Rectangle target, Color color) = 0;

  /** Loads the image from the resources if necessary and draws it into the provided target rectangle
   *
   * @param resourceId the id, with which the image gets referenced in the resource.rc
   * @param targetRectangle the rectangle to draw the image into
   */
  virtual void DrawImage(int resourceId, Rectangle targetRectangle) = 0;

  /** Draws the image with the given resource id into the target rectangle. The image is then rotated around it's top left corner or center.
   * 
   * @param resourceId the id, with which the image gets referecned in the resource.rc
   * @param targetRectangle the destination to draw the image into before applying the roation
   * @param rotationAngle the angle (in degrees) to rotate (rightwards) the image and target rectangle
   * @param rotateCenter if this is set to true, then the image gets rotated around it's own center, otherwise it gets rotated around it's top left corner.
   */
  virtual void DrawImage(int resourceId, Rectangle targetRectangle, float rotationAngle, bool rotateCenter = false) = 0;


  /** Draws an arc line from a given start point for a specified angle around the given center point.
   * 
   * @param startPoint the point where to start drawing the arc
   * @param center the arc's center point (of the imagined circle we draw on)
   * @param angle the angle to draw on the arc (can be negative to draw counter clockwise)
   * @param color the stroke color
   * @param strokeWidth the stroke width
   */
  virtual void DrawArc(Vector startPoint, Vector center, float angle, Color color, float strokeWidth = 1.0f) = 0;


  /** Draws an ellipse around the specified center point with the given x radius and y radius.
   */
  virtual void DrawEllipse(Vector center, float radiusX, float radiusY, Color color, float strokeWidth = 1.0f) = 0;
};
}
#pragma once

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


  /** This method returns the native render target, used internally if the RenderInterface doen't provide enough functionality
   *  This should not be used if not really necessary.
   */
  virtual ID2D1RenderTarget& RenderTarget() = 0;

  /** Returns the size of the render target.
   */
  virtual Size Size() = 0;

  /** Creates a new text format with the given arguments, or returns an already existing format, which has been created with the same parameters.
   *
   * @return a new TextFormat != NULL if successful
   */
  virtual TextFormat CreateTextFormat(const wchar_t* fontName, float fontSize) = 0;

  /** Draws a text into the specified area with the previously created text format (CreateTextFormat)
   */
  virtual void DrawText(const std::wstring& text, TextFormat format, Rectangle targetRect, Color color) = 0;


  /** Draws a line between the specified two points
   */
  virtual void DrawLine(Vector from, Vector to, Color color, float strokeWidth = 1.0f) = 0;
};
}
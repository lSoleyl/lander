#pragma once

namespace Lander {

class Game;
class Camera;
class FontLoader;

/** This class implements the RenderInterface interface for the Game class and draws onto the 
 *  Game's renderTarget.
 */
class GameRenderer : public RenderInterface {
  public:
  GameRenderer(Game& game, Camera& camera, ID2D1HwndRenderTarget** ppRenderTarget);
  ~GameRenderer();

  /** Returns the current size of the renderTarget
   */
  virtual Lander::Size Size() override;

  /** Creates a text format, which can be used with DrawText.
   */
  virtual TextFormat CreateTextFormat(const wchar_t* fontName, float fontSize) override;

  /** Draws the given text, using the previously created text format
   */
  virtual void DrawText(const std::wstring& text, TextFormat format, Rectangle rectangle, Color color) override;

  /** Draws a line between the specified two points with the given color and stroke Width
   */
  virtual void DrawLine(Vector from, Vector to, Color color, float strokeWidth = 1.0f) override;

  /** Draws a rectanlge in the specified color
   */
  virtual void DrawRectangle(Rectangle target, Color color, float strokeWidth = 1.0f) override;

  /** Fills the target rectangle with the specified color
   */
  virtual void FillRectangle(Rectangle target, Color color) override;

  virtual void DrawImage(int resourceId, Rectangle targetRectangle) override;

  
  virtual void DrawImage(int resourceId, Rectangle targetRectangle, float rotationAngle, bool rotateCenter = false) override;

  
  virtual void DrawArc(Vector startPoint, Vector center, float angle, Color color, float strokeWidth = 1.0f) override;


  virtual void DrawEllipse(Vector center, float radiusX, float radiusY, Color color, float strokeWidth = 1.0f) override;


  /** Provide access to internal renderTarget... should be removed in the future if possible
   */
  virtual ID2D1RenderTarget& RenderTarget() override;

  /** Sets the given object as current view object, draws the bounding box and calls Draw() upon it.
   */
  void DrawObject(ViewObject* viewObject, double secondsPassed);

private:  

  /** Loads a binary resource into memory and returns a pointer to it
   */
  Data LoadBinaryResource(int resourceId);

  /** Returns the rotation center of the currently active view object.
   */
  Vector RotationCenter() const;

  /** Loads the given image from resource and converts it into a renderable image type.
   *
   * @throws std::exception if anything fails
   */
  Resource<ID2D1Bitmap> LoadImageResource(int resourceId);


  /** Returns the translation matrix for the current view object, which should be applied before the rotation matrix.
   *  This already includes the cameraOffset.
   */
  D2D1::Matrix3x2F TranslationMatrix() const;

  /** Returns the current view object's rotation matrix
   */
  D2D1::Matrix3x2F RotationMatrix() const;



  struct FONT_ENTRY { 
    FONT_ENTRY() = default;
    FONT_ENTRY(const wchar_t* fontName, float fontSize, IDWriteTextFormat* format);
    FONT_ENTRY(FONT_ENTRY&& other) = default;

    FONT_ENTRY& operator=(FONT_ENTRY&& other) = default;


    std::wstring fontName; 
    float fontSize; 
    Resource<IDWriteTextFormat> textFormat; 
  };

  Game& game;
  Camera& camera;
  ID2D1HwndRenderTarget** ppRenderTarget; //The GameRender doesn't own the renderTarget, so it isn't wrapped inside a Resource<>
  Resource<IWICImagingFactory> imageFactory;
  Resource<IDWriteFactory> writeFactory;
  Resource<FontLoader> fontLoader;
  std::vector<FONT_ENTRY> textFormats;
  std::unordered_map<int/*resourceId*/, Resource<ID2D1Bitmap>> loadedImages;
  
  
  ViewObject* viewObject; //the currently drawn view object

};

}
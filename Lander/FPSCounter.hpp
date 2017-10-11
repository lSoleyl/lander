#pragma once

class FPSCounter : public ViewObject {
public:
  FPSCounter();

  /** Creates the writeFactory and the textFormat
   */
  virtual void Initialize() override;

  /** Draw the FPS counter and update the FPS count
   */
  virtual void Draw(RenderInterface& renderInterface, double secondsPassed) override;

  virtual D2D1_SIZE_F Size() override { return D2D1::SizeF(200, 50); }

  /** Releases the writeFactory and textFormat
   */
  virtual void Deinitialize() override;
private:
  Resource<IDWriteFactory> writeFactory;   //factory, used to create text formats
  Resource<IDWriteTextFormat> textFormat;  //text format used to draw the counter
  float avgFps;
};
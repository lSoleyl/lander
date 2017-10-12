#pragma once

namespace Lander {

class FPSCounter : public ViewObject {
public:
  FPSCounter();

  /** Draw the FPS counter and update the FPS count
   */
  virtual void Draw(RenderInterface& renderInterface, double secondsPassed) override;

  /** Specify low render priority to be always visible
   */
  virtual int RenderPriority() const override;

private:
  RenderInterface::TextFormat textFormat; //The format used, to write the text
  float avgFps;
};

}
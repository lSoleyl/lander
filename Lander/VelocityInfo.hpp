#pragma once
#include "Rocket.hpp"

namespace Lander {

  class VelocityInfo : public ViewObject {
  public:
    VelocityInfo(Rocket& rocket);

    virtual void Initialize(Size size) override;

    virtual void Draw(RenderInterface& renderInterface, double secondsPassed) override;

    /** Specify low render priority to be always visible
    */
    virtual int RenderPriority() const override;

  private:
    Size windowSize;
    Rocket& rocket;

    int milliVelocity;
    int velocity;

    int milliRotation = 0;
    int rotation = 0;

    Size arrowSize;

    RenderInterface::TextFormat textFormat; //The format used, to write the text
  };

}
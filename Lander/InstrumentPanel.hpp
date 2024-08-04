#pragma once
#include "Rocket.hpp"

namespace Lander {

  class InstrumentPanel : public ViewObject {
  public:
    InstrumentPanel(Rocket& rocket);

    virtual void Initialize(Size size) override;

    virtual void Draw(RenderInterface& renderInterface, double secondsPassed) override;

    /** Specify low render priority to be always visible
     */
    virtual int RenderPriority() const override;

  private:
    Size windowSize;
    Rocket& rocket;


    void DrawInstrumentBackground(RenderInterface& renderInterface);

    /** Position and size of the circular main instrument display
     */
    const Rectangle instrument;

    RenderInterface::TextFormat textFormat; //The format used, to write the text
  };

}
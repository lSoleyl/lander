#include "stdafx.h"
#include "InstrumentPanel.hpp"
#include <sstream>
#include <iomanip>

namespace {

/** A helper struct to format a flot value in the format -###.# onto std::wostream
 */
struct FormattedNumber {
  FormattedNumber(float value) : value(value) {};
  float value;
};

std::wostream& operator<<(std::wostream& out, FormattedNumber number) {
  if (number.value < 0) {
    out << L'-';
  }

  out << std::setw(3) << std::abs(static_cast<int>(number.value)) << L"." << std::setw(1) << std::abs(static_cast<int>(number.value * 10) % 10);
  return out;
}

}



namespace Lander {

  InstrumentPanel::InstrumentPanel(Rocket& _rocket) : textFormat(NULL), rocket(_rocket), instrument(Vector::Down*80 + Vector::Right*50, Size(100,100)) {
  size = Size(200, 40);
}

int InstrumentPanel::RenderPriority() const { return -98; }

void InstrumentPanel::Initialize(Size _size) {
  windowSize = _size;
  pos = Vector() + Vector::Right * (windowSize.width - size.width) + Vector::Down * 30;
}

void InstrumentPanel::Draw(RenderInterface& renderInterface, double secondsPassed) {
  
  auto velocity = rocket.velocity.Length();
  auto rotation = rocket.rotation;

  while (rotation > 180) {
    rotation -= 360;
  }
  while (rotation < -180) {
    rotation += 360;
  }

  // Build current velocity and angle in format: "Velocity: ###.# m/s, Rotation: ###.#°"
  std::wstringstream textStream;
  textStream << L"Velocity: " << FormattedNumber(velocity) << L" m/s" << std::endl; // Velocity
  textStream << L"Angle: "    << FormattedNumber(rotation) << L"°"    << std::endl; // Angle

  // Create text format upon first use
  if (!textFormat) {
    textFormat = renderInterface.CreateTextFormat(L"NI7SEG", 15);
  }

  renderInterface.DrawText(textStream.str(), textFormat, Rectangle(Vector::Zero, size), Color::LightGoldenrodYellow);


  DrawInstrumentBackground(renderInterface);



  ///
  // Arc indicating rotation speed
  ///
  const Vector indicatorArm = instrument.TopCenter() - instrument.Center();
  renderInterface.DrawArc(instrument.TopCenter(), instrument.Center(), rocket.angularVelocity * 10, Lander::Color::DeepSkyBlue, 5.f);


  ///
  // Line indicating current orientation
  ///
  const Vector rotationArm = indicatorArm.Rotate(rocket.rotation);
  renderInterface.DrawLine(instrument.Center() - rotationArm, instrument.Center() + rotationArm, Lander::Color::LightGreen, 2.f);



  /// 
  // Line indicating velocity vector
  ///
  Vector scaledVelocityVector = indicatorArm.Rotate(Vector::Up.AngleTo(rocket.velocity)) * std::min(velocity / 50.f, 1.0f);
  renderInterface.DrawLine(instrument.Center(), instrument.Center() + scaledVelocityVector, Lander::Color::Yellow, 3.f);


}


void InstrumentPanel::DrawInstrumentBackground(RenderInterface& renderInterface) {
  float radius = instrument.Size().Abs().height / 2;

  // First draw the main circle
  renderInterface.DrawEllipse(instrument.Center(), radius, radius, Lander::Color::White);
   
  // Now draw the tick lines
  const float tickLength = radius / 5;
  renderInterface.DrawLine(instrument.TopCenter(),    instrument.TopCenter() + Vector::Down * tickLength, Lander::Color::White, 2.0f);
  renderInterface.DrawLine(instrument.RightCenter(),  instrument.RightCenter() + Vector::Left * tickLength, Lander::Color::White, 2.0f);
  renderInterface.DrawLine(instrument.BottomCenter(), instrument.BottomCenter() + Vector::Up * tickLength, Lander::Color::White, 2.0f);
  renderInterface.DrawLine(instrument.LeftCenter(),   instrument.LeftCenter() + Vector::Right * tickLength, Lander::Color::White, 2.0f);
}


}
#include "stdafx.h"
#include "VelocityInfo.hpp"
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

VelocityInfo::VelocityInfo(Rocket& _rocket) : textFormat(NULL), rocket(_rocket), arrowSize(30, 100) {
  size = Size(200, 40);
}

int VelocityInfo::RenderPriority() const { return -98; }

void VelocityInfo::Initialize(Size _size) {
  windowSize = _size;
  pos = Vector() + Vector::Right * (windowSize.width - size.width) + Vector::Down * 30;
}

void VelocityInfo::Draw(RenderInterface& renderInterface, double secondsPassed) {
  
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

  ///
  // Arrow indicating direction
  ///

  // TODO: Draw indicator for current orientation and angular velocity

  auto arrowPosition = Vector::Down * 80 + Vector::Right * 50;
  float arrowRotation = Vector::Up.AngleTo(rocket.velocity);

  // Scale the arrow size with the current velocity (reaching full size at 100m/s)
  auto scaledArrowSize = arrowSize;
  scaledArrowSize.height *= std::min(velocity / 100.f, 1.0f);
  
  // Move the arrow down by half the removed size. Otherwise the arrow seems to move 
  // up and down when growing and shrinking.
  arrowPosition += Vector::Down * (arrowSize.height - scaledArrowSize.height) / 2;

  renderInterface.DrawImage(IDR_ARROW_IMAGE, Rectangle(arrowPosition, scaledArrowSize), arrowRotation, true);
}

}
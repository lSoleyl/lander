#include "stdafx.h"
#include "VelocityInfo.hpp"
#include <sstream>
#include <iomanip>

namespace Lander {

  VelocityInfo::VelocityInfo(Rocket& _rocket) : textFormat(NULL), rocket(_rocket), arrowSize(100, 30) {
    size = Size(200, 40);
  }

  int VelocityInfo::RenderPriority() const { return -98; }

  void VelocityInfo::Initialize(Size _size) {
    windowSize = _size;
    pos = Vector() + Vector::Right * (windowSize.width - size.width) + Vector::Down * 30;
  }


  const double pi = std::atan(1.0) * 4;

  double prd_vect(Vector u, Vector v)
  {
    return u.x * v.y - u.y * v.x;
  }
  double angle_rad(Vector u, Vector v)
  {
    return (prd_vect(u, v) < 0 ? -1 : 1) * acos((u.x * v.x + u.y * v.y) / (u.Length()*v.Length()));
  }

  double angle_deg(Vector u, Vector v)
  {
    return angle_rad(u, v) / pi * 180;
  }

  void VelocityInfo::Draw(RenderInterface& renderInterface, double secondsPassed) {


    double velocityIntegral;
    double velocityFractional = modf(rocket.velocity.Length(), &velocityIntegral);
    velocity = static_cast<int>(velocityIntegral);
    milliVelocity = velocityFractional * 10;

    double rotationIntegral;
    double rotationFractional = modf(rocket.rotation, &rotationIntegral);
    milliRotation = abs(rotationFractional * 10);
    
    rotation = static_cast<int>(rotationIntegral);

    while (rotation > 180)
      rotation -= 360;
    while (rotation < -180)
      rotation += 360;

    //Build current velocity and angle in format: "Velocity: ###.# m/s, Rotation: ###.#°"
    std::wstringstream textStream;
    textStream << (L"Velocity: ") << std::setw(3) << velocity << (L".") << std::setw(1) << milliVelocity << (L" m/s") << std::endl; //Velocity
    textStream << (L"Angle: ")    << std::setw(3) << rotation << (L".") << std::setw(1) << milliRotation << (L"°")    << std::endl; //Angle

    // Create text format upon first use
    if (!textFormat)
      textFormat = renderInterface.CreateTextFormat(L"NI7SEG", 15);

    renderInterface.DrawText(textStream.str(), textFormat, Rectangle(Vector(), size), Color::LightGoldenrodYellow);

    ///
    // Arrow indicating direction
    ///

    auto arrowPosition = Vector() + Vector::Down * 80 + Vector::Right * 50;
    float arrowRotation = angle_deg(Vector::Up, rocket.velocity);
    renderInterface.DrawImage(IDR_ARROW_IMAGE, Rectangle(arrowPosition, arrowSize), (-90 + arrowRotation), true); //needs to be rotated 90 degrees
  }

}
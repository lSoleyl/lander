#include "stdafx.h"
#include "Rocket.hpp"
#include "Platform.hpp"

namespace Lander {
Rocket::Rocket(const Platform& startPlatform) : startPlatform(startPlatform) {
  size = Size(startPlatform.size.width*2/3, 100); //Rocket has smaller width than the platform, it is starting from
}


void Rocket::Update(double secondsSinceLastFrame) {
  if (GetKeyState(VK_SPACE) & (1 << 7)) { //Don't know why, but GetAsyncKeyState() doesn't work correctly when called to often
    thrustCheck = false;  //Do not position Rocket on platform anymore
    //FIXME: The rocket's speed is currently bound to the frame rate...
    pos += Vector::Up;  //Rocket keeps flying up as long as the Space-Key is pressed
  }

  // Only adapt position so long until the user adds thrust to the rocket
  if (thrustCheck) {
    pos = startPlatform.pos + Vector::Up * size.height; //Calculate top position of rocket
    pos += Vector::Right * (startPlatform.size.width - size.width) / 2; //Center rocket on start platform
  }

}

void Rocket::Draw(RenderInterface& renderTarget, double secondsSinceLastFrame) {
  renderTarget.DrawImage(IDR_ROCKET_IMAGE, Rectangle(Vector(), size));
}




}
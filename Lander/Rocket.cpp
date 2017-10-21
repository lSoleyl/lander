#include "stdafx.h"
#include "Rocket.hpp"
#include "Platform.hpp"

namespace Lander {
Rocket::Rocket(const Platform& startPlatform) : startPlatform(startPlatform) {
  size = Size(startPlatform.size.width*2/3, 100); //Rocket has smaller width than the platform it is starting from
  trailSize = Size(size.width*0.5, 75); //trail has half the rocket's width and is 75px high
  trailAlternateSize[0] = 75;
  trailAlternateSize[1] = 100;
}


void Rocket::Update(double secondsSinceLastFrame) {
  if (GetKeyState(VK_SPACE) & (1 << 7)) { //Don't know why, but GetAsyncKeyState() doesn't work correctly when called to often
    thrustCheck = false;  //Do not position Rocket on platform anymore
    secondsSinceLastUpdate += secondsSinceLastFrame;
    if (secondsSinceLastUpdate >= rocketSpeedValue) {
      pos += Vector::Up;  //Rocket keeps flying up as long as the Space-Key is pressed
      secondsSinceLastUpdate -= rocketSpeedValue;
    }
  }

  // Only adapt position so long until the user adds thrust to the rocket
  if (thrustCheck) {
    pos = startPlatform.pos + Vector::Up * size.height; //Calculate top position of rocket
    pos += Vector::Right * (startPlatform.size.width - size.width) / 2; //Center rocket on start platform
  }

}

void Rocket::Draw(RenderInterface& renderTarget, double secondsSinceLastFrame) {
  renderTarget.DrawImage(IDR_ROCKET_IMAGE, Rectangle(Vector(), size));
  if (thrustCheck == false) {  //Only draw the trail if the rocket has started

    secondsSinceLastAnimation += secondsSinceLastFrame;
    if (secondsSinceLastAnimation >= 0.15) {
      index = (index + 1) % 2; //alternate between 0 and 1
      trailSize.height = trailAlternateSize[index];
      secondsSinceLastAnimation -= 0.15;
    }

    trailVector = Vector();  //the trail updates with the rockets position
    trailVector += Vector::Right * (size.width / 4);
    trailVector += Vector::Down * size.height;

    renderTarget.DrawImage(IDR_ROCKET_TRAIL_IMAGE, Rectangle(trailVector, trailSize));
  }
}




}
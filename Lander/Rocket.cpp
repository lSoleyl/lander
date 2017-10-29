#include "stdafx.h"
#include "Rocket.hpp"
#include "Platform.hpp"

namespace Lander {
Rocket::Rocket(const Platform& startPlatform) : startPlatform(startPlatform) {
  size = Size(startPlatform.size.width*2/3, 100); //Rocket has smaller width than the platform it is starting from
}


void Rocket::Update(double secondsSinceLastFrame) {
  if (GetKeyState(VK_SPACE) & (1 << 7) || GetKeyState(VK_UP) & (1 << 7)) { //Don't know why, but GetAsyncKeyState() doesn't work correctly when called to often
    thrustCheck = false;  //Do not position Rocket on platform anymore
    pos += Vector::Up.Rotate(rotation) * verticalSpeed * secondsSinceLastFrame; //Rocket keeps flying forward as long as the Space-Key is pressed
  }

  if (GetKeyState(VK_LEFT) & (1 << 7) && !(GetKeyState(VK_RIGHT) & (1 << 7))) {  //only rotate if left key is pressed and right key unpressed
    rotation -= angularSpeed * secondsSinceLastFrame; //rotate rocket to the left side
  } 
  
  if (GetKeyState(VK_RIGHT) & (1 << 7) && !(GetKeyState(VK_LEFT) & (1 << 7))) {  //only rotate if right key is pressed and left key is unpressed
    rotation += angularSpeed * secondsSinceLastFrame; //rotate rocket to the right side
  }

  //Only adapt position so long until the user adds thrust to the rocket
  if (thrustCheck) {
    pos = startPlatform.pos + Vector::Up * size.height; //Calculate top position of rocket
    pos += Vector::Right * (startPlatform.size.width - size.width) / 2; //Center rocket on start platform
  }

}

void Rocket::Draw(RenderInterface& renderTarget, double secondsSinceLastFrame) {
  Rectangle rocketRect(Vector(), size);
  Size rcsSize(10, 5);

  renderTarget.DrawImage(IDR_ROCKET_IMAGE, rocketRect);

  static int trailHeight[] = { 75, 100 };

  if (GetKeyState(VK_SPACE) & (1 << 7) || GetKeyState(VK_UP) & (1 << 7)) {  //Only draw the trail if the rocket has started

    secondsSinceLastAnimation += secondsSinceLastFrame;
    if (secondsSinceLastAnimation >= 0.15) {
      trailIndex = (trailIndex + 1) % 2; //alternate between 0 and 1
      secondsSinceLastAnimation -= 0.15;
    }

    auto trailPosition = Vector::Right * (size.width / 4) + Vector::Down * size.height;
    renderTarget.DrawImage(IDR_ROCKET_TRAIL_IMAGE, Rectangle(trailPosition, Size(size.width*0.5, trailHeight[trailIndex])));
  }

  if (GetKeyState(VK_LEFT) & (1 << 7)) {
    renderTarget.DrawImage(IDR_ROCKET_THRUST_IMAGE, Rectangle(rocketRect.TopRight() + Vector::Down * 10 + Vector::Left * 10, rcsSize)); //Top right
    renderTarget.DrawImage(IDR_ROCKET_THRUST_IMAGE, Rectangle(rocketRect.BottomLeft() + Vector::Up * 10 + Vector::Left * 10, rcsSize), 180, true); //Bottom left, needs to be rotated 180 degrees
  }

  if (GetKeyState(VK_RIGHT) & (1 << 7)) {
    renderTarget.DrawImage(IDR_ROCKET_THRUST_IMAGE, Rectangle(rocketRect.topLeft + Vector::Down * 10, rcsSize), 180, true); //Top left, needs to be rotated 180 degrees
    renderTarget.DrawImage(IDR_ROCKET_THRUST_IMAGE, Rectangle(rocketRect.bottomRight + Vector::Up * 10, rcsSize)); //Bottom right
  }
 }


}
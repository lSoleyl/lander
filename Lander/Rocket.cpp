#include "stdafx.h"
#include "Rocket.hpp"
#include "Platform.hpp"
#include "Helper.hpp"

namespace Lander {
Rocket::Rocket(const Platform& startPlatform, const Platform& landingPlatform, ScreenText& screenText, TimeCounter& timeCounter) : startPlatform(startPlatform), landingPlatform(landingPlatform), screenText(screenText), timeCounter(timeCounter) {
  size = Size(startPlatform.size.width*2/3, 100); //Rocket has smaller width than the platform it is starting from
}


void Rocket::Reposition() {
  pos = startPlatform.pos + Vector::Up * (size.height+1); //Calculate top position of rocket
  pos += Vector::Right * (startPlatform.size.width - size.width) / 2; //Center rocket on start platform

  rotation = 0;
  Stop();
  screenText.SetRunning();
}

void Rocket::PhysicsUpdate(double secondsSinceLastFrame) {
  mass = baseMass + Tank.Mass();

  if (KeyPressed(VK_ESCAPE)) {
    Reposition();
    state = STATE::UNSTARTED;
    Tank.Refill();
    timeCounter.ResetCount();
  }

  switch (state) {

    case STATE::CRASHED:
      Stop();
      screenText.SetGameOver();
      timeCounter.StopCount();
      break;

    case STATE::SUCCESS:
      Stop();
      screenText.SetVictory();
      timeCounter.StopCount();
      break;

    case STATE::LANDED:
      Tank.Fill(2*secondsSinceLastAnimation);

    case STATE::UNSTARTED:

      Reposition(); //Adapt position until the user adds thrust to the rocket
      if (KeyPressed(VK_SPACE) || KeyPressed(VK_UP)) {
        state = STATE::STARTED;  //Do not position Rocket on platform anymore
        timeCounter.StartCount();
      }
      break;

    case STATE::STARTED:

      CheckCollisions();
      if (KeyPressed(VK_SPACE) || KeyPressed(VK_UP))
        ApplyForce((Vector::Up * Tank.GetThrust(secondsSinceLastFrame)).Rotate(rotation));

      if (KeyPressed(VK_LEFT) && !(KeyPressed(VK_RIGHT)))
        ApplyAngularAcceleration(-angularAcceleration);

      if (KeyPressed(VK_RIGHT) && !(KeyPressed(VK_LEFT)))
        ApplyAngularAcceleration(angularAcceleration);

      ApplyGravity();  //pull rocket towards the ground with 9.81 m/s²
      break;

  }

}

void Rocket::Draw(RenderInterface& renderTarget, double secondsSinceLastFrame) {
  ///
  // Rocket
  ///
  Rectangle rocketRect(Vector(), size);
  renderTarget.DrawImage(IDR_ROCKET_IMAGE, rocketRect);

  Rectangle explosionRect((Vector() + Vector::Left * 341 + Vector::Up * 310), Size(717, 720));

  ///
  // Fuel tanks
  ///
  Size rcsSize(10, 5);
  Size fullFuelSize(4, 47);
  Size emptyFuelSize = fullFuelSize;
  emptyFuelSize.height *= 1.0 - Tank.CurrentVolume();  

  Rectangle fuelRectLeft(Vector::Down * 35 + Vector::Right * 5, fullFuelSize);
  Rectangle emptyRectLeft(Vector::Down * 35 + Vector::Right * 5, emptyFuelSize);

  Rectangle fuelRectRight(Vector::Down * 35 + Vector::Right * 24, fullFuelSize);
  Rectangle emptyRectRight(Vector::Down * 35 + Vector::Right * 24, emptyFuelSize);

  renderTarget.FillRectangle(fuelRectLeft, Color::LightGray);
  renderTarget.FillRectangle(fuelRectRight, Color::LightGray);

  renderTarget.FillRectangle(emptyRectLeft, Color::Black);
  renderTarget.FillRectangle(emptyRectRight, Color::Black);


  // Don't draw thrust animations if we had a collision
  if (state != STATE::CRASHED) {    

    ///
    // Main thrust (animated)
    ///
    static float trailHeight[] = { 75, 100 };

    if (KeyPressed(VK_SPACE) || KeyPressed(VK_UP) && !Tank.IsEmpty() && (state != STATE::SUCCESS)) {  //Only draw the trail if the rocket has started

      secondsSinceLastAnimation += secondsSinceLastFrame;
      if (secondsSinceLastAnimation >= 0.15) {
        trailIndex = (trailIndex + 1) % 2; //alternate between 0 and 1
        secondsSinceLastAnimation -= 0.15;
      }

      auto trailPosition = Vector::Right * (size.width / 4) + Vector::Down * size.height;
      renderTarget.DrawImage(IDR_ROCKET_TRAIL_IMAGE, Rectangle(trailPosition, Size(size.width*0.5f, trailHeight[trailIndex])));
    }


    /// 
    // RCS thrusters
    ///
    if (KeyPressed(VK_LEFT)) {
      renderTarget.DrawImage(IDR_ROCKET_THRUST_IMAGE, Rectangle(rocketRect.TopRight() + Vector::Down * 10 + Vector::Left * 10, rcsSize)); //Top right
      renderTarget.DrawImage(IDR_ROCKET_THRUST_IMAGE, Rectangle(rocketRect.BottomLeft() + Vector::Up * 10 + Vector::Left * 10, rcsSize), 180, true); //Bottom left, needs to be rotated 180 degrees
    }

    if (KeyPressed(VK_RIGHT)) {
      renderTarget.DrawImage(IDR_ROCKET_THRUST_IMAGE, Rectangle(rocketRect.topLeft + Vector::Down * 10, rcsSize), 180, true); //Top left, needs to be rotated 180 degrees
      renderTarget.DrawImage(IDR_ROCKET_THRUST_IMAGE, Rectangle(rocketRect.bottomRight + Vector::Up * 10, rcsSize)); //Bottom right
    }
  }
  else {
    renderTarget.DrawImage(IDR_EXPLOSION_IMAGE, explosionRect);
  }
}

void Rocket::OnCollision(Collider& collider) {
  float rotationValue = rotation;

  while (rotationValue >= 357) //Adjust rotationValue
    rotationValue -= 360;
  while (rotationValue <= -357)
    rotationValue += 360;

  //8 m/s seems achievable, 1 m/s does not    //Rotation has to be between 3° and -3°
  if ((velocity.Length() < 8) && (rotationValue <= 3 && rotationValue >= -3) && (&collider == &startPlatform)) {
    state = STATE::LANDED;
    return;
  }

  if ((velocity.Length() < 8) && (rotationValue <= 3 && rotationValue >= -3) && (&collider == &landingPlatform)) {
    state = STATE::SUCCESS;
    return;
  }

  state = STATE::CRASHED;
}


}
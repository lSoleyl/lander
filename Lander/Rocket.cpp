#include "stdafx.h"
#include "Rocket.hpp"
#include "Platform.hpp"
#include "Game.hpp"
#include "Helper.hpp"

#include <array>

namespace Lander {
Rocket::Rocket(const Platform& startPlatform) : startPlatform(startPlatform) {
  size = Size(startPlatform.size.width*2/3, 100); //Rocket has smaller width than the platform it is starting from
}


void Rocket::Reposition() {
  pos = startPlatform.pos + Vector::Up * (size.height+1); //Calculate top position of rocket
  pos += Vector::Right * (startPlatform.size.width - size.width) / 2; //Center rocket on start platform
  
  rotation = 0;

  state = STATE::LANDED;

  Stop();
}

void Rocket::PhysicsUpdate(double secondsSinceLastFrame) {
  mass = baseMass + Tank.Mass();

  if (KeyPressed(VK_ESCAPE)) {
    Reposition();    
    Tank.Refill();
  }

  CheckCollisions();

  switch (state) {

    case STATE::LANDED:

      Reposition(); //Adapt position until the user adds thrust to the rocket
      if (KeyPressed(VK_SPACE) || KeyPressed(VK_UP))
        state = STATE::STARTED;  //Do not position Rocket on platform anymore
      break;

    case STATE::STARTED:

      if (KeyPressed(VK_SPACE) || KeyPressed(VK_UP))
        ApplyForce((Vector::Up * Tank.GetThrust(secondsSinceLastFrame)).Rotate(rotation));

      if (KeyPressed(VK_LEFT) && !(KeyPressed(VK_RIGHT)))
        ApplyAngularAcceleration(-angularAcceleration);

      if (KeyPressed(VK_RIGHT) && !(KeyPressed(VK_LEFT)))
        ApplyAngularAcceleration(angularAcceleration);

      ApplyGravity();  //pull rocket towards the ground with 9.81 m/s²
      break;

    case STATE::CRASHED:
      Stop();
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

    if (KeyPressed(VK_SPACE) || KeyPressed(VK_UP) && !Tank.IsEmpty()) {  //Only draw the trail if the rocket has started

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


void Rocket::CheckCollisions() {
  Rectangle rocketRect(Vector::Zero, size);

  std::array<Vector, 8> collisionPoints;

  collisionPoints[0] = ObjectToWorld(rocketRect.topLeft);
  collisionPoints[1] = ObjectToWorld(rocketRect.TopCenter());
  collisionPoints[2] = ObjectToWorld(rocketRect.TopRight());
  collisionPoints[3] = ObjectToWorld(rocketRect.LeftCenter());
  collisionPoints[4] = ObjectToWorld(rocketRect.RightCenter());
  collisionPoints[5] = ObjectToWorld(rocketRect.BottomLeft());
  collisionPoints[6] = ObjectToWorld(rocketRect.BottomCenter());
  collisionPoints[7] = ObjectToWorld(rocketRect.bottomRight);

  for(auto collider : Game::Instance()->GetColliders()) {
    if (collider != this) { //ignore the rocket itself

      Vector Distance = ObjectToWorld(this->Center()) - ObjectToWorld(collider->Center()); //Vector connecting both objects center
      float DistanceValue = Distance.Length();
      float ValueRocket = Center().Length(); //Rockets radius-value
      float ValueCollider = collider->Center().Length(); //Colliders radius-value

      if ((ValueRocket + ValueCollider) > DistanceValue) { //if distance is smaller than the object's radiuses: collision is possible
        
        //check whether one of the eight points of the rectangle is inside the other rectangle. (if so, set sate to CRASHED)
        if (std::any_of(collisionPoints.begin(), collisionPoints.end(), [collider](const Vector& point) { 
          return collider->IsPointInside(point);
        })) {
          state = STATE::CRASHED;
        }
      }
    }  
  }
}


}
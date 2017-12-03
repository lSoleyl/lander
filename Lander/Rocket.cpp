#include "stdafx.h"
#include "Rocket.hpp"
#include "Platform.hpp"
#include "Game.hpp"

namespace Lander {
Rocket::Rocket(const Platform& startPlatform) : startPlatform(startPlatform) {
  size = Size(startPlatform.size.width*2/3, 100); //Rocket has smaller width than the platform it is starting from
}


void Rocket::PhysicsUpdate(double secondsSinceLastFrame) {
  mass = baseMass + Tank.Mass();

  if (GetKeyState(VK_ESCAPE) & (1 << 7)) {
    thrustCheck = true;
    rotation = 0;
    Tank.Refill();
    Stop();
    hasCollision = false;
  }
  
  if (!hasCollision && !CheckCollisions()) { //Collision check
    //No collisions -> regular logic
    if (GetKeyState(VK_SPACE) & (1 << 7) || GetKeyState(VK_UP) & (1 << 7)) { //Don't know why, but GetAsyncKeyState() doesn't work correctly when called to often
      thrustCheck = false;  //Do not position Rocket on platform anymore
      ApplyForce((Vector::Up * Tank.GetThrust(secondsSinceLastFrame)).Rotate(rotation));
    }

    if (GetKeyState(VK_LEFT) & (1 << 7) && !(GetKeyState(VK_RIGHT) & (1 << 7)) && !thrustCheck) {  //only rotate if left key is pressed and right key unpressed
      ApplyAngularAcceleration(-angularAcceleration);
    }

    if (GetKeyState(VK_RIGHT) & (1 << 7) && !(GetKeyState(VK_LEFT) & (1 << 7)) && !thrustCheck) {  //only rotate if right key is pressed and left key is unpressed
      ApplyAngularAcceleration(angularAcceleration);
    }

    //Only adapt position so long until the user adds thrust to the rocket
    if (thrustCheck) {
      pos = startPlatform.pos + Vector::Up * (size.height+1); //Calculate top position of rocket
      pos += Vector::Right * (startPlatform.size.width - size.width) / 2; //Center rocket on start platform
    } else {
      ApplyGravity();  //pull rocket towards the ground with 9.81 m/s²
    }
  } else {
    //Collision -> stop movement
    Stop();
  }  
}

void Rocket::Draw(RenderInterface& renderTarget, double secondsSinceLastFrame) {
  ///
  // Rocket
  ///
  Rectangle rocketRect(Vector(), size);
  renderTarget.DrawImage(IDR_ROCKET_IMAGE, rocketRect);

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
  if (!hasCollision) {    

    ///
    // Main thrust (animated)
    ///
    static float trailHeight[] = { 75, 100 };

    if ((GetKeyState(VK_SPACE) & (1 << 7) || GetKeyState(VK_UP) & (1 << 7)) && !Tank.IsEmpty()) {  //Only draw the trail if the rocket has started

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


bool Rocket::CheckCollisions() {
  for(auto collider : Game::Instance()->GetColliders()) {
    if (collider != this) { //ignore the rocket itself

      Vector Distance = ObjectToWorld(this->Center()) - ObjectToWorld(collider->Center()); //Vector connecting both objects center
      double DistanceValue = VectorValue(Distance);
      double ValueRocket = VectorValue(Center()); //Rockets radius-value
      double ValueCollider = VectorValue(collider->Center()); //Colliders radius-value

      if ((ValueRocket + ValueCollider) > DistanceValue) { //if distance is smaller than the object's radiuses: collision is possible
        
        //check whether one of the eight points of the rectangle is inside the other rectangle. (if so, set hasCollision to true)

        Rectangle rocketRec(pos, Size(size.width, size.height*2));

        Vector topLeftCorner = pos.Rotate(-rotation, Center());
        Vector bottomLeftCorner = topLeftCorner + (Vector::Down * size.height * 2).Rotate(-rotation);//rocketRec.BottomLeft().Rotate(-rotation, Center());
        Vector topRightCorner = rocketRec.TopRight().Rotate(-rotation, Center());
        Vector bottomRightCorner = rocketRec.bottomRight.Rotate(-rotation, Center());

        Vector leftCenter = topLeftCorner + (Vector::Down*size.height).Rotate(-rotation);
        Vector topCenter = topLeftCorner + (Vector::Right * size.width).Rotate(-rotation);
        Vector rightCenter = topLeftCorner + (Vector::Down * size.height).Rotate(-rotation) + (Vector::Right * size.width).Rotate(-rotation);
        Vector bottomCenter = topLeftCorner + (Vector::Right * size.width).Rotate(-rotation) + ((Vector::Down * size.height).Rotate(-rotation) * 2);

        if (collider->IsPointInside(ObjectToWorld(topLeftCorner)))
          hasCollision = true;
        if (collider->IsPointInside(ObjectToWorld(bottomRightCorner)))
          hasCollision = true;
        if (collider->IsPointInside(ObjectToWorld(bottomLeftCorner)))
          hasCollision = true;
        if (collider->IsPointInside(ObjectToWorld(topRightCorner)))
          hasCollision = true;
        if (collider->IsPointInside(ObjectToWorld(leftCenter)))
          hasCollision = true;
        if (collider->IsPointInside(ObjectToWorld(topCenter)))
          hasCollision = true;
        if (collider->IsPointInside(ObjectToWorld(rightCenter)))
          hasCollision = true;
        if (collider->IsPointInside(ObjectToWorld(bottomCenter)))
          hasCollision = true;
      }
    }  
  }

  return hasCollision;
}


}
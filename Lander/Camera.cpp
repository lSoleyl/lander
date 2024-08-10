
#include "stdafx.h"
#include "Camera.hpp"

Lander::Camera::Camera(Rectangle screenRect) : visibleRect(screenRect), screenRect(screenRect) {}

Lander::Vector Lander::Camera::WorldToScreen(Vector worldVector) const {
  // FIXME: At the moment this simple calculation doesn't take zooming into account!
  // Apply camera translation
  return worldVector + screenRect.topLeft - visibleRect.topLeft;
}

const Lander::Rectangle& Lander::Camera::GetVisibleRect() const {
  return visibleRect;
}

void Lander::Camera::TrackObject(ViewObject& object, double secondsSinceLastUpdate) {
  // Try to keep the tracked object somehow in the camera's view

  Rectangle objectRect(object.pos, object.size);

  auto screenTopLeft10Percent = screenRect.topLeft + Vector::Down * screenRect.Size().height * 0.1f;

  if (Rectangle(screenTopLeft10Percent, screenRect.bottomRight).Contains(objectRect)) {
    // The object is in the lower 90% of the screen in world coordinates -> set our camera to the initial setting
    visibleRect = screenRect;
    return;
  }


  // object is above upper 10% of the screen -> move camera there
  auto cameraTopLeft10Percent = visibleRect.topLeft + Vector::Down * visibleRect.Size().height * 0.1f;
  visibleRect += Vector::Up * (cameraTopLeft10Percent - object.pos).y;


}


#include "stdafx.h"
#include "Platform.hpp"
#include "Terrain.hpp"

namespace Lander {


Platform::Platform(const Terrain& terrain, float xPos) : terrain(terrain), xPos(xPos) {
  size = Size(50,3);
}

  
void Platform::Update(double secondsSinceLastFrame) {
  pos = terrain.GetTerrainPos(xPos);
  pos += Vector::Left * size.width/2; //Platform should be centered at that position
  pos += Vector::Up * size.height;
}


void Platform::Draw(RenderInterface& renderTarget, const Rectangle& visibleRect, double secondsSinceLastFrame) {
  // only draw if the platform is visible
  if (visibleRect.Intersects(Rectangle(pos, size))) {
    //TODO: Replace line with Image
    renderTarget.DrawLine(Vector::Down, Vector::Down + Vector::Right*size.width, Color::YellowGreen, 2);
  }
}




}
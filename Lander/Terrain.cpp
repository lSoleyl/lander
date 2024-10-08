#include "stdafx.h"
#include "Terrain.hpp"

#undef min
#undef max

namespace Lander {

void Terrain::Initialize(Size size) {
  this->size = size; // The terrain spans the whole display
}

void Terrain::Draw(RenderInterface& renderTarget, const Rectangle& visibleRect, double secondsSinceLastFrame) {
  Vector baseLine = Vector::Down * size.height;
  for (int x = visibleRect.topLeft.x; x <= visibleRect.bottomRight.x; ++x) {
    baseLine.x = static_cast<float>(x);
    renderTarget.DrawLine(baseLine, baseLine + (Vector::Up*GetTerrainHeight(baseLine.x)), Color::LightSlateGray);
  }
}

int Terrain::RenderPriority() const { return 1000; }



Vector Terrain::GetTerrainPos(float x) const {
  Vector baseLine = Vector::Down * size.height;
  baseLine.x = x;
  return baseLine + (Vector::Up*GetTerrainHeight(x));
}

bool Terrain::IsPointInside(Vector worldPoint) const {
  float terrainHeight = size.height - GetTerrainHeight(worldPoint.x);

  return (terrainHeight < worldPoint.y);
}


float Terrain::GetTerrainHeight(float x) const {
  const double amplitude = size.height/7.0;
  const double horizScale = 0.02;
  double height = amplitude/2;
  // Generate terrain by adding some curves
  height += amplitude*sin(x*horizScale);
  height += abs(amplitude*0.75*cos(x*horizScale*1.7));
  height += amplitude/2*cos(x*horizScale/3);
  height += amplitude/4*sin(x*horizScale*1.5);
  

  height += x/size.width * size.height/3; //Add ascending slope

  return static_cast<float>(std::max(height, 0.0));
}


}
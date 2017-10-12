#include "stdafx.h"
#include "Terrain.hpp"

#undef min
#undef max

namespace Lander {

void Terrain::Initialize(Size size) {
  this->size = size; //The terrain spans the whole display
}

void Terrain::Draw(RenderInterface& renderTarget, double secondsSinceLastFrame) {
  Vector baseLine = Vector::Down * size.height;
  for(int x = 0; x < size.width; ++x) {
    baseLine.x = x;
    renderTarget.DrawLine(baseLine, baseLine + (Vector::Up*GetTerrainHeight(x)), Color::LightSlateGray);
  }
}

int Terrain::RenderPriority() const { return 1000; }



Vector Terrain::GetTerrainPos(float x) const {
  Vector baseLine = Vector::Down * size.height;
  baseLine.x = x;
  return baseLine + (Vector::Up*GetTerrainHeight(x));
}


float Terrain::GetTerrainHeight(float x) const {
  const float amplitude = size.height/5.0;
  const float horizScale = 0.02;
  auto height = amplitude/2;
  //Generate terrain by adding some curves
  height += amplitude*sin(x*horizScale);
  height += abs(amplitude*0.75*cos(x*horizScale*1.7));
  height += amplitude/2*cos(x*horizScale/3);
  height += amplitude/4*sin(x*horizScale*1.5);
  

  height += x/size.width * size.height/2; //Add ascending slope

  return std::max<float>(height, 0);
}


}
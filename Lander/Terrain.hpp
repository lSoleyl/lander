#pragma once

namespace Lander {

class Terrain : public ViewObject {
public:
  virtual void Initialize(Size size) override;

  virtual void Draw(RenderInterface& renderTarget, double secondsSinceLastFrame) override;

  /** Specify high render priority to first draw the terrain.
   */
  virtual int RenderPriority() const override;

private:
  /** Returns the terrain height at the given position
   */
  float GetTerrainHeight(float x) const;

};

}
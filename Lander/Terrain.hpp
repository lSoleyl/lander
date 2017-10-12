#pragma once

namespace Lander {

class Terrain : public ViewObject {
public:
  virtual void Initialize(Size size) override;

  virtual void Draw(RenderInterface& renderTarget, double secondsSinceLastFrame) override;

  /** Specify high render priority to first draw the terrain.
   */
  virtual int RenderPriority() const override;

  /** Returns the topmost position of the terrain at the specified x position.
   *  The position is specified relative to pos
   */
  virtual Vector GetTerrainPos(float x) const;

private:
  /** Returns the terrain height at the given position
   */
  float GetTerrainHeight(float x) const;

};

}
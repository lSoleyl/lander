#pragma once

namespace Lander {

class Terrain : public Collider {
public:
  virtual void Initialize(Size size) override;

  virtual void Draw(RenderInterface& renderTarget, const Rectangle& visibleRect, double secondsSinceLastFrame) override;

  /** Specify high render priority to first draw the terrain.
   */
  virtual int RenderPriority() const override;

  /** Returns the topmost position of the terrain at the specified x position.
   *  The position is specified relative to pos
   */
  virtual Vector GetTerrainPos(float x) const;

  virtual bool IsPointInside(Vector point) const override;

private:
  /** Returns the terrain height at the given position
   */
  float GetTerrainHeight(float x) const;

};

}
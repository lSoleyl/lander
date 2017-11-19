#pragma once

 class FuelTank {

   public:
    FuelTank(float);
    ~FuelTank();

    float Mass() const;
    bool IsEmpty() const;
    float GetThrust(float secondsSinceLastFrame);
   private:

    float emptyMass = 94064;
    float fuelMass;
    float maxVolume = 1988.23;
    float currentVolume = maxVolume;
 };
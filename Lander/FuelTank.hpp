#pragma once

 class FuelTank {

   public:
    FuelTank();
    ~FuelTank();

    float Mass() const;
    bool IsEmpty() const;
    float GetThrust(float secondsSinceLastFrame);

    /** Returns currentVolume/maxVolume (0-1)
     */
    float CurrentVolume();

   private:
    float maxVolume = 1988.23;
    float currentVolume;
    float emptyMass = 94064;  //empty mass in kg
 };
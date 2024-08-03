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
    void Refill(); //completely refills the tank
    void Fill(float percent); //refills the tank by a percentage value

   private:
    float maxVolume = 1988.23f;
    float currentVolume;
    float emptyMass = 94064.f;  //empty mass in kg
 };
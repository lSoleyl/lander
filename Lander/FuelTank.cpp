#include "stdafx.h"
#include "FuelTank.hpp"

namespace {
 const float p = 280;  //Volume of H² in kg/m³
 const float ls = 3830;  //Amount of energy every kg of fuel burn provides, in Newton for a second
 const float Q = 10.714; //Amount of fuel that is burnt every second, in m³

}

FuelTank::FuelTank() : currentVolume(maxVolume) {}

  FuelTank::~FuelTank(){}

  float FuelTank::Mass() const {
    return emptyMass + (currentVolume*p);  //returns the FuelTanks mass in kg
  }

  bool FuelTank::IsEmpty() const {
    if (currentVolume <= 0)
      return true;
    else
      return false;
  }

  float FuelTank::GetThrust(float secondsSinceLastFrame) {

    if (IsEmpty())
      return 0;

    currentVolume -= (Q*secondsSinceLastFrame);

    return Q*p*ls;  //thrust per second, in Newton
  }
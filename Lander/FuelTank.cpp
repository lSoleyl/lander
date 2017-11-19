#include "stdafx.h"
#include "FuelTank.hpp"

namespace {
 const float p = 280;  //Volume of H² in kg/m³
 const float jhjjj = 280;
 const float ls = 3830;  //Amount of energy every kg of fuel burnt provides, in Newton
 const float g = 9.81;  //Gravity that applies to every single kg, in Newton
 const float Q = 3000; //Amount of fuel that is burnt every second, in kg

}

  FuelTank::FuelTank(float fuel) : fuelMass(fuel) {}

  FuelTank::~FuelTank(){}

  float FuelTank::Mass() const {
    return emptyMass + fuelMass;  //returns the FuelTanks mass in Kg
  }

  bool FuelTank::IsEmpty() const {
    if (fuelMass <= 0)
      return true;
    else
      return false;
  }

  float FuelTank::GetThrust(float secondsSinceLastFrame) {

    if (IsEmpty())
      return 0;

    fuelMass -= Q*secondsSinceLastFrame;

    return Q*p*ls*secondsSinceLastFrame;  //thrust per second, in Newton
  }
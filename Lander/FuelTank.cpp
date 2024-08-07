#include "stdafx.h"
#include "FuelTank.hpp"

namespace {
 const float p = 280.f;   // Volume of H� in kg/m�
 const float ls = 3830.f; // Amount of energy every kg of fuel burn provides, in Newton for a second
 const float Q = 10.714f; // Amount of fuel that is burnt every second, in m�

}

FuelTank::FuelTank() : currentVolume(maxVolume) {}

  FuelTank::~FuelTank(){}

  float FuelTank::Mass() const {
    return emptyMass + (currentVolume*p);  //returns the FuelTanks mass in kg
  }

  bool FuelTank::IsEmpty() const {
    if (currentVolume <= 0) {
      return true;
    } else {
      return false;
    }
  }

  float FuelTank::GetThrust(float secondsSinceLastFrame) {

    if (IsEmpty()) {
      return 0;
    }

    currentVolume -= (Q*secondsSinceLastFrame);

    return Q*p*ls;  // thrust per second, in Newton
  }

  float FuelTank::CurrentVolume() {
    return currentVolume/maxVolume;
  }

  void FuelTank::Refill() {
    currentVolume = maxVolume;
  }

  void FuelTank::Fill(float percent) {
    if (currentVolume >= maxVolume) {
      Refill();
      return;
    }
    currentVolume = currentVolume + (maxVolume / 100 * percent);
  }
#include "stdafx.h"
#include "Game.hpp"

//Game objects
#include "FPSCounter.hpp"
#include "FuelTank.hpp"
#include "Terrain.hpp"
#include "Platform.hpp"
#include "Rocket.hpp"
#include "ScreenText.hpp"
#include "TimeCounter.hpp"
#include "InstrumentPanel.hpp"

#include "HoverAIInput.hpp"

#include "resources.h"

using namespace Lander;


/** main() for windows applications
 */
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR /*cmdline*/, int) {
  // Use HeapSetInformation to specify that the process should
  // terminate if the heap manager detects an error in any heap used
  // by the process.
  // The return value is ignored, because we want to continue running in the
  // unlikely event that HeapSetInformation fails.
  HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

  if (SUCCEEDED(CoInitialize(NULL))) {
    registerFonts();

    {
      Game app;
            
      //Add Game/View objects
      FPSCounter fpsCounter;
      app.AddObject(fpsCounter);
            
      Terrain terrain;
      app.AddObject(terrain);

      Platform startPlatform(terrain, 162/*starting xPos*/);
      app.AddObject(startPlatform);

      Platform landingPlatform(terrain, 835/*target xPos*/);
      app.AddObject(landingPlatform);
            
      ScreenText screenText;
      app.AddObject(screenText);

      TimeCounter timeCounter;
      app.AddObject(timeCounter);

      Rocket rocket(startPlatform, landingPlatform, screenText, timeCounter);
      app.AddObject(rocket);
      app.TrackObject(rocket);

      InstrumentPanel panel(rocket);
      app.AddObject(panel);

      // Comment in to hover the rocket in place until it runs out of fuel
      //app.SetInput(std::make_unique<HoverAIInput>(rocket));

      if (SUCCEEDED(app.Initialize())) {
        app.RunMessageLoop();
      }
    }
    CoUninitialize();
  }

  return 0;
}
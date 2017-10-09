#include "stdafx.h"
#include "Game.hpp"
#include "FPSCounter.hpp"


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
        {
            Game app;
            
            //Add Game/View objects
            FPSCounter fpsCounter;
            app.AddObject(fpsCounter);

            if (SUCCEEDED(app.Initialize())) {
                app.RunMessageLoop();
            }
        }
        CoUninitialize();
    }

    return 0;
}
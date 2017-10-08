#pragma once

#include <unordered_map>

class Game
{
public:
    Game();
    ~Game();

    // Register the window class and call methods for instantiating drawing resources
    HRESULT Initialize();

    // Process and dispatch messages
    void RunMessageLoop();

    /** Returns the current instance of the game object
     */
    static Game* Instance();

    /** Returns a (possibly new) brush for the given color
     */
    ID2D1Brush* GetSolidBrush(D2D1::ColorF::Enum color);

private:
    // Initialize device-independent resources.
    HRESULT CreateDeviceIndependentResources();

    /** (Re-)Initialize device-dependent resources. This mainly initializes the renderTarget.
     */
    HRESULT CreateDeviceResources();

    // Release device-dependent resource.
    void DiscardDeviceResources();

    /** Releases all brushes and clears the brush map
     */
    void ReleaseBrushes();

    // Draw content.
    HRESULT OnRender();

    // Resize the render target.
    void OnResize(UINT width, UINT height);

    // The windows procedure.
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,LPARAM lParam);

    //The current game instance
    static Game* instance;

    HWND hWnd;
    ID2D1Factory* direct2DFactory;
    ID2D1HwndRenderTarget* renderTarget;
    IDWriteFactory* writeFactory; //factory, used to create text formats
    IDWriteTextFormat* fpsTextFormat; //text format for FPS counter

    std::unordered_map<D2D1::ColorF::Enum, ID2D1Brush*> brushMap; //map of color -> brush
};
#pragma once

#include "Input.hpp"
#include "GameRenderer.hpp"

namespace Lander {

class Game
{
public:
    friend class GameRenderer;

    Game();
    ~Game();

    // This will be the size of the game window
    static const int WINDOW_HEIGHT = 960;
    static const int WINDOW_WIDTH = 1280;

    // Register the window class and call methods for instantiating drawing resources
    HRESULT Initialize();

    // Process and dispatch messages
    void RunMessageLoop();

    /** Returns the current instance of the game object
     */
    static Game* Instance();

    /** Adds a view object to the game's render queue. The view objects's Initialize() gets called
     *  after the game initializes. If the object is added after the game's initialization, Initialize() is called immediately.
     *
     * @param viewObject the viewObject to add. The Game doesn't take ownership of this object and the object has to make sure, it 
     *                   exists at least until Deinitialize() gets called.
     */
    void AddObject(ViewObject& viewObject);

    /** Returns the internal list of colliders for collision checks.
     */
    const std::vector<Collider*>& GetColliders() const;

    /** Returns a reference to the currently active input instance
     */
    const Input& GetInput() const;
    
private:
    /** Returns a (possibly new) brush for the given color. Used by Rendersurface
     */
    ID2D1Brush* GetSolidBrush(Color color);

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
    Resource<ID2D1Factory> direct2DFactory;
    Resource<ID2D1HwndRenderTarget> renderTarget;
    std::unique_ptr<GameRenderer> gameRenderer;
    bool initialized;

    // The currently active input instance
    std::unique_ptr<Input> input;

    std::unordered_map<D2D1::ColorF::Enum, Resource<ID2D1Brush>> brushMap; //map of color -> brush

    std::deque<ViewObject*> renderQueue; //List of objects, which get rendered on each draw
    std::vector<Collider*> colliders; //List of colliders for faster direct access
};
}
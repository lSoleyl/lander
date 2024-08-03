#include "stdafx.h"
#include "Game.hpp"
#include <chrono>

using namespace std;
namespace Lander {

Game* Game::instance = nullptr;

Game::Game() : hWnd(NULL), initialized(false) {
  Game::instance = this;
}

Game* Game::Instance() {
  return instance;
}

Game::~Game()
{
  //First clear colliders, we don't want objects to access deleted objects in Deinitialize()
  colliders.clear();

  //Deinitialize all view objects
  for (auto viewObject : renderQueue) {
    viewObject->Deinitialize();
  }
  renderQueue.clear();

  DiscardDeviceResources();
  
  Game::instance = nullptr;
}


void Game::RunMessageLoop()
{

  auto lastFrame = chrono::high_resolution_clock::now();
  double msElapsed = 0; //Elapsed milliseconds since last frame

  // MessageLoop:
	while(true)    
	{
    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) { //Program quitted
        break;
      }
      
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      //Frame timing
      auto currentTime = chrono::high_resolution_clock::now();
      msElapsed = (chrono::duration_cast<chrono::microseconds>(currentTime - lastFrame).count() / 1000.0);
      
      if (msElapsed < 10) { //Max framerate = 100 fps
        continue;
      }

      lastFrame = currentTime;
      OnRender(); //Render frame
    }
  }
}


HRESULT Game::Initialize()
{
  // Initialize device-indpendent resources, such
  // as the Direct2D factory.
  HRESULT hr = CreateDeviceIndependentResources();
  gameRenderer.reset(new GameRenderer(*this,&renderTarget));

  if (SUCCEEDED(hr)) {
    // Register the window class.
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = Game::WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = sizeof(LONG_PTR);
    wcex.hInstance     = HINST_THISCOMPONENT;
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName  = NULL;
    wcex.hCursor       = LoadCursor(NULL, IDI_APPLICATION);
    wcex.lpszClassName = _T("Lander Game Window");

    RegisterClassEx(&wcex);


    // Because the CreateWindow function takes its size in pixels,
    // obtain the system DPI and use it to scale the window size.
    // The function returns the current system DPI. This is also the value it will use
    // to create its own windows.
    uint32_t dpi = GetDpiForSystem();

    // Create the window.
    hWnd = CreateWindow(_T("Lander Game Window"), _T("Lander Game"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        static_cast<UINT>(ceil(WINDOW_WIDTH * dpi / 96.f)),
        static_cast<UINT>(ceil(WINDOW_HEIGHT * dpi / 96.f)),
        NULL, NULL, HINST_THISCOMPONENT, this);

    hr = hWnd ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
      ShowWindow(hWnd, SW_SHOWNORMAL);
      UpdateWindow(hWnd);
    }
  }

  //Initialize all already registered view objects and create the RenderSurface
  if (SUCCEEDED(hr)) {
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    auto size = Rectangle(rcClient).Size().Abs();
    for (auto viewObject : renderQueue) {
      viewObject->Initialize(size);
    }
  }
   
  initialized = true;

  return hr;
}

const std::vector<Collider*>& Game::GetColliders() const {
  return colliders;
}

void Game::AddObject(ViewObject& viewObject) {
  if (renderQueue.empty()) {
    renderQueue.push_back(&viewObject);
  } else { //!empty
    if (viewObject.RenderPriority() <= renderQueue.back()->RenderPriority()) {
      renderQueue.push_back(&viewObject);
    } else if (viewObject.RenderPriority() >= renderQueue.front()->RenderPriority()) {
      renderQueue.push_front(&viewObject);
    } else { //insertion + sort
      renderQueue.push_back(&viewObject);
      std::stable_sort(renderQueue.begin(), renderQueue.end(), [](ViewObject* v1, ViewObject* v2) { return v1->RenderPriority() > v2->RenderPriority(); });
    }
  }

  // Add to collider list if it is a collider
  if (auto collider = dynamic_cast<Collider*>(&viewObject)) {
    colliders.push_back(collider);
  }

  // If initialization already took place, initialize the object upon insertion
  if (initialized) {
    viewObject.Initialize(gameRenderer->Size());
  }
}


HRESULT Game::CreateDeviceIndependentResources() {
  // Create a Direct2D factory.
  return D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &direct2DFactory);
}


HRESULT Game::CreateDeviceResources() {
  if (renderTarget) {
    return S_OK;
  }


  RECT rc;
  GetClientRect(hWnd, &rc);
  D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

  // Create a Direct2D render target.
  return direct2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWnd, size), &renderTarget);
}

ID2D1Brush* Game::GetSolidBrush(D2D1::ColorF::Enum color) {
  auto pos = brushMap.find(color);
  if (pos != brushMap.end()) {
    return pos->second;
  }

  //Brush not yet created, we have to create it
  if (!renderTarget) {
    throw std::exception("Illegal use of GetSolidBrush() with uninitialized renderTarget");
  }

  ID2D1SolidColorBrush* brush = nullptr;
  auto result = renderTarget->CreateSolidColorBrush(D2D1::ColorF(color), &brush);
  HandleCOMError(result, L"Brush creation");
  
  brushMap[color] = brush;
  return brush;
}

void Game::ReleaseBrushes() {
  brushMap.clear();
}


void Game::DiscardDeviceResources() {
  ReleaseBrushes();  
  renderTarget.reset();
}



LRESULT CALLBACK Game::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  LRESULT result = 0;

  auto game = Game::Instance();

  if (game) {
    switch (message) {
    case WM_SIZE:
      // Handle resizing of the window
      game->OnResize(LOWORD(lParam), HIWORD(lParam));
      return 0;

    case WM_DISPLAYCHANGE:
      InvalidateRect(hWnd, NULL, FALSE);
      return 0;

    case WM_PAINT:
      //Rendering here is unnecessary
      ValidateRect(hWnd, NULL);
      return 0;

    // prevent size change by grabbing the border by returning a non resizable border
    case WM_NCHITTEST:
    {
      auto result = DefWindowProc(hWnd, message, wParam, lParam);
      switch (result) {
      case HTSIZE: 
      case HTBOTTOM:
      case HTBOTTOMLEFT:
      case HTBOTTOMRIGHT:
      case HTLEFT:
      case HTRIGHT:
      case HTTOP:
        return HTBORDER;
      }

      return result;
    }

    // prevent size change when maximizing by returning the window's height/width as max size
    case WM_GETMINMAXINFO:
    {
      auto minmax = reinterpret_cast<MINMAXINFO*>(lParam);
      minmax->ptMaxSize.x = WINDOW_WIDTH;
      minmax->ptMaxSize.y = WINDOW_HEIGHT;
      return 0;    
    }


    case WM_DESTROY:
      PostQuitMessage(0);
      return 1;
    }
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}

HRESULT Game::OnRender()
{
  using namespace D2D1;
  static chrono::steady_clock::time_point lastUpdate = chrono::steady_clock::now();

  //Create device resources if not already done
  HRESULT res = CreateDeviceResources();
  if (SUCCEEDED(res)) {
    //Get the time since the last frame
    auto now = chrono::steady_clock::now();
    auto secondsPassed = chrono::duration<double>(now-lastUpdate).count();
    lastUpdate = now;

    //Give objects time to update positions
    for (auto viewObject : renderQueue) {
      if (viewObject->enabled) {
        viewObject->Update(secondsPassed);
      }
    }


    renderTarget->BeginDraw();  //Initiate drawing
    renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black)); //Set background color


    //Render each object of the render queue
    for (auto viewObject : renderQueue) {
      gameRenderer->DrawObject(viewObject, secondsPassed);
    }
    
    
    res = renderTarget->EndDraw();
    if (res == D2DERR_RECREATE_TARGET) {
      res = S_OK;
      DiscardDeviceResources();    
    }
  }


  return res;
}


void Game::OnResize(UINT width, UINT height) {
  if (renderTarget) {
    // Note: This method can fail, but it's okay to ignore the
    // error here, because the error will be returned again
    // the next time EndDraw is called.
    renderTarget->Resize(D2D1::SizeU(width, height));
  }
}

}

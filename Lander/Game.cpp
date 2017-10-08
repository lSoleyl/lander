#include "stdafx.h"
#include "Game.hpp"
#include <chrono>

using namespace std;

Game* Game::instance = nullptr;

Game::Game() : 
  hWnd(NULL),
  direct2DFactory(nullptr),
  renderTarget(nullptr),
  writeFactory(nullptr),
  fpsTextFormat(nullptr) {
  
  Game::instance = this;
}

Game* Game::Instance() {
  return instance;
}

Game::~Game()
{
  SafeRelease(&direct2DFactory);
  DiscardDeviceResources();
  SafeRelease(&writeFactory);
  SafeRelease(&fpsTextFormat);

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
      if(msg.message == WM_QUIT) //Program quitted
        break;
      
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      //Frame timing
      auto currentTime = chrono::high_resolution_clock::now();
      msElapsed = (chrono::duration_cast<chrono::microseconds>(currentTime - lastFrame).count() / 1000.0);
      
      if(msElapsed < 10) //Max framerate = 100 fps
        continue;

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
    wcex.lpszClassName = "Lander Game Window";

    RegisterClassEx(&wcex);


    // Because the CreateWindow function takes its size in pixels,
    // obtain the system DPI and use it to scale the window size.
    FLOAT dpiX, dpiY;

    // The factory returns the current system DPI. This is also the value it will use
    // to create its own windows.
    direct2DFactory->GetDesktopDpi(&dpiX, &dpiY);


    // Create the window.
    hWnd = CreateWindow("Lander Game Window", "Lander Game", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        static_cast<UINT>(ceil(WINDOW_WIDTH * dpiX / 96.f)),
        static_cast<UINT>(ceil(WINDOW_HEIGHT * dpiY / 96.f)),
        NULL, NULL, HINST_THISCOMPONENT, this);

    hr = hWnd ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
      ShowWindow(hWnd, SW_SHOWNORMAL);
      UpdateWindow(hWnd);
    }
  }

  return hr;
}


HRESULT Game::CreateDeviceIndependentResources() {
  // Create a Direct2D factory.
  auto result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &direct2DFactory);
    
  // Initialization for text usage
  if (SUCCEEDED(result)) {
    result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));    
  }

  if (SUCCEEDED(result)) {
    result = writeFactory->CreateTextFormat(
      L"Consolas",
      NULL,
      DWRITE_FONT_WEIGHT_REGULAR,
      DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL,
      72.0f,
      L"en-us",
      &fpsTextFormat);
  }
    
  return result;
}


HRESULT Game::CreateDeviceResources() {
  if (renderTarget)
    return S_OK;


  RECT rc;
  GetClientRect(hWnd, &rc);
  D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

  // Create a Direct2D render target.
  return direct2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWnd, size), &renderTarget);
}

ID2D1Brush* Game::GetSolidBrush(D2D1::ColorF::Enum color) {
  auto pos = brushMap.find(color);
  if (pos != brushMap.end())
    return pos->second;

  //Brush not yet created, we have to create it
  if (!renderTarget)
    throw std::exception("Illegal use of GetSolidBrush() with uninitialized renderTarget");

  ID2D1SolidColorBrush* brush = nullptr;
  if (SUCCEEDED(renderTarget->CreateSolidColorBrush(D2D1::ColorF(color), &brush))) {
    brushMap[color] = brush;
    return brush;
  }

  throw std::exception("Brush creation failed!");
}

void Game::ReleaseBrushes() {
  for(auto& entry : brushMap) {
    entry.second->Release();
  }

  brushMap.clear();
}


void Game::DiscardDeviceResources()
{
  ReleaseBrushes();  
  SafeRelease(&renderTarget);
}



LRESULT CALLBACK Game::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  LRESULT result = 0;

  auto game = Game::Instance();

  bool wasHandled = false;

  if (game) {
    switch (message) {
    case WM_SIZE:
      // Handle resizing of the window
      game->OnResize(LOWORD(lParam), HIWORD(lParam));
      result = 0;
      wasHandled = true;
      break;

    case WM_DISPLAYCHANGE:
      InvalidateRect(hWnd, NULL, FALSE);
      result = 0;
      wasHandled = true;
      break;

    case WM_PAINT:
      // When the game has to redraw
      game->OnRender();
      ValidateRect(hWnd, NULL);
      result = 0;
      wasHandled = true;
      break;

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
      result = 1;
      wasHandled = true;
      break;
    }
  }

  if (!wasHandled) {
    result = DefWindowProc(hWnd, message, wParam, lParam);
  }
    
  return result;
}


HRESULT Game::OnRender()
{
  using namespace D2D1;
  static chrono::steady_clock::time_point lastUpdate = chrono::steady_clock::now();
  static float avgFps = 0;


  //Create device resources if not already done
  HRESULT res = CreateDeviceResources();
  if (SUCCEEDED(res)) {

    //Collect time for FPS counter
    auto now = chrono::steady_clock::now();
    chrono::duration<double> secondsPassed = now-lastUpdate;
    long fps = static_cast<long>(1.0 / secondsPassed.count());
    avgFps = (0.9*avgFps) + (0.1*fps); //Take the average of ten frames
    lastUpdate = now;

    renderTarget->BeginDraw();  //Initiate drawing
    
    renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black)); //Set background color
    D2D1_SIZE_F rtSize = renderTarget->GetSize();

    // Draw a grid background.
    int width = static_cast<int>(rtSize.width);
    int height = static_cast<int>(rtSize.height);



    for(int x = 0; x < width; x += 1) {
      const float amplitude = height/5;
      const float horizScale = 0.03;
      renderTarget->DrawLine(Point2F(x, rtSize.height/2 + sin(x*horizScale)*amplitude), Point2F(x, rtSize.height), GetSolidBrush(ColorF::ForestGreen));
    }



    // Draw two rectangles.
    D2D1_RECT_F rectangle1 = D2D1::RectF(
        rtSize.width/2 - 50.0f,
        rtSize.height/2 - 50.0f,
        rtSize.width/2 + 50.0f,
        rtSize.height/2 + 50.0f
        );

    D2D1_RECT_F rectangle2 = D2D1::RectF(
        rtSize.width/2 - 100.0f,
        rtSize.height/2 - 100.0f,
        rtSize.width/2 + 100.0f,
        rtSize.height/2 + 100.0f
        );

    // Draw a filled rectangle.
    //renderTarget->FillRectangle(&rectangle1,m_pLightSlateGrayBrush);

    // Draw the outline of a rectangle.
    renderTarget->DrawRectangle(&rectangle2, GetSolidBrush(ColorF::CornflowerBlue));

    
    //Draw FPS counter
    std::wstring fpsStr(L"FPS: ");
    fpsStr += to_wstring(static_cast<int>(avgFps));
    renderTarget->DrawText(fpsStr.c_str(), fpsStr.length(), fpsTextFormat, D2D1::RectF(0, 0, rtSize.width, rtSize.height), GetSolidBrush(ColorF::CornflowerBlue));


    
    res = renderTarget->EndDraw();
    if (res == D2DERR_RECREATE_TARGET) {
      res = S_OK;
      DiscardDeviceResources();    
    }
  }


  return res;
}


void Game::OnResize(UINT width, UINT height)
{
    if (renderTarget)
    {
        // Note: This method can fail, but it's okay to ignore the
        // error here, because the error will be returned again
        // the next time EndDraw is called.
        renderTarget->Resize(D2D1::SizeU(width, height));
    }
}
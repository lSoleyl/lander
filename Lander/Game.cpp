#include "stdafx.h"
#include "Game.hpp"
#include <chrono>

using namespace std;

Game* Game::instance = nullptr;

Game::Game() : 
  m_hwnd(NULL),
  m_pDirect2dFactory(nullptr),
  m_pRenderTarget(nullptr),
  m_pLightSlateGrayBrush(nullptr),
  m_pCornflowerBlueBrush(nullptr),
  writeFactory(nullptr),
  fpsTextFormat(nullptr) {
  
  Game::instance = this;
}

Game* Game::Instance() {
  return instance;
}

Game::~Game()
{
  SafeRelease(&m_pDirect2dFactory);
  SafeRelease(&m_pRenderTarget);
  SafeRelease(&m_pLightSlateGrayBrush);
  SafeRelease(&m_pCornflowerBlueBrush);
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
    m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);


    // Create the window.
    m_hwnd = CreateWindow("Lander Game Window", "Lander Game", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
        static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
        NULL, NULL, HINST_THISCOMPONENT, this);

    hr = m_hwnd ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
      ShowWindow(m_hwnd, SW_SHOWNORMAL);
      UpdateWindow(m_hwnd);
    }
  }

  return hr;
}


HRESULT Game::CreateDeviceIndependentResources() {
    // Create a Direct2D factory.
    auto result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
    
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
  if (m_pRenderTarget)
    return S_OK;


  RECT rc;
  GetClientRect(m_hwnd, &rc);
  D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

  // Create a Direct2D render target.
  HRESULT hr = m_pDirect2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

  
  if (SUCCEEDED(hr)) {
    // Create a gray brush.
    hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightSlateGray), &m_pLightSlateGrayBrush);
  }

  if (SUCCEEDED(hr)) {
    // Create a blue brush.
    hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &m_pCornflowerBlueBrush);
  }

  return hr;
}


void Game::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pLightSlateGrayBrush);
    SafeRelease(&m_pCornflowerBlueBrush);
}



LRESULT CALLBACK Game::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
      InvalidateRect(hwnd, NULL, FALSE);
      result = 0;
      wasHandled = true;
      break;

    case WM_PAINT:
      // When the game has to redraw
      game->OnRender();
      ValidateRect(hwnd, NULL);
      result = 0;
      wasHandled = true;
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      result = 1;
      wasHandled = true;
      break;
    }
  }

  if (!wasHandled) {
    result = DefWindowProc(hwnd, message, wParam, lParam);
  }
    
  return result;
}


HRESULT Game::OnRender()
{
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

    m_pRenderTarget->BeginDraw();  //Initiate drawing
    
    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White)); //Set background color
    D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

    // Draw a grid background.
    int width = static_cast<int>(rtSize.width);
    int height = static_cast<int>(rtSize.height);

    for (int x = 0; x < width; x += 10)
    {
        m_pRenderTarget->DrawLine(
            D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
            D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
            m_pLightSlateGrayBrush,
            0.5f
            );
    }

    for (int y = 0; y < height; y += 10)
    {
        m_pRenderTarget->DrawLine(
            D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
            D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
            m_pLightSlateGrayBrush,
            0.5f
            );
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
    m_pRenderTarget->FillRectangle(&rectangle1,m_pLightSlateGrayBrush);

    // Draw the outline of a rectangle.
    m_pRenderTarget->DrawRectangle(&rectangle2, m_pCornflowerBlueBrush);

    
    //Draw FPS counter
    std::wstring fpsStr(L"FPS: ");
    fpsStr += to_wstring(static_cast<int>(avgFps));
    m_pRenderTarget->DrawText(fpsStr.c_str(), fpsStr.length(), fpsTextFormat, D2D1::RectF(0, 0, rtSize.width, rtSize.height), m_pCornflowerBlueBrush);


    
    res = m_pRenderTarget->EndDraw();
    if (res == D2DERR_RECREATE_TARGET) {
      res = S_OK;
      DiscardDeviceResources();    
    }
  }


  return res;
}


void Game::OnResize(UINT width, UINT height)
{
    if (m_pRenderTarget)
    {
        // Note: This method can fail, but it's okay to ignore the
        // error here, because the error will be returned again
        // the next time EndDraw is called.
        m_pRenderTarget->Resize(D2D1::SizeU(width, height));
    }
}
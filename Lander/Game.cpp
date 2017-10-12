#include "stdafx.h"
#include "Game.hpp"
#include <chrono>

using namespace std;
namespace Lander {

Game* Game::instance = nullptr;

Game::Game() : hWnd(NULL), initialized(false), renderSurface(nullptr) {
  Game::instance = this;
}

Game* Game::Instance() {
  return instance;
}

Game::~Game()
{
  //Deinitialize all view objects
  for (auto viewObject : renderQueue) {
    viewObject->Deinitialize();
  }
  renderQueue.clear();

  DiscardDeviceResources();
  
  delete renderSurface;
  Game::instance = nullptr;
}


/** Local implementation of the RenderInterface for this game
 */
class RenderSurface : public RenderInterface {
public:
  RenderSurface(Game& game, ID2D1HwndRenderTarget** ppRenderTarget) : game(game), ppRenderTarget(ppRenderTarget) {}

  virtual ID2D1RenderTarget& RenderTarget() override { return **ppRenderTarget; }

  virtual Lander::Size Size() override { return RenderTarget().GetSize(); }

  
  virtual TextFormat CreateTextFormat(const wchar_t* fontName, float fontSize) override {   
    for(size_t i = 0; i < textFormats.size(); ++i)
      if (textFormats[i].fontName == fontName && textFormats[i].fontSize == fontSize)
        return static_cast<TextFormat>(i+1);


    //if not found
    IDWriteTextFormat* textFormat = nullptr;
    auto result = game.writeFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-us", &textFormat);
    HandleCOMError(result, "TextFormat creation");
    
    textFormats.push_back(FONT_ENTRY());
    textFormats.back() = FONT_ENTRY(fontName, fontSize, textFormat);
    return static_cast<TextFormat>(textFormats.size());
  }

  virtual void DrawText(const std::wstring& text, TextFormat format, Rectangle rectangle, Color color) override {
    assert(format > 0 && format <= textFormats.size()); //Ensure the text format is valid
    RenderTarget().DrawText(text.c_str(), static_cast<UINT32>(text.length()), textFormats[format-1].textFormat, rectangle, game.GetSolidBrush(color));
  }

private: 
  struct FONT_ENTRY { 
    FONT_ENTRY() {}
    FONT_ENTRY(const wchar_t* fontName, float fontSize, IDWriteTextFormat* format): fontName(fontName), fontSize(fontSize), textFormat(format) {}
    FONT_ENTRY(FONT_ENTRY&& other) : fontName(std::move(fontName)), fontSize(fontSize), textFormat(std::move(textFormat)) {}

    FONT_ENTRY& operator=(FONT_ENTRY&& other) {
      fontSize = other.fontSize;
      fontName.swap(other.fontName);      
      textFormat = std::move(other.textFormat);
      return *this;
    }


    std::wstring fontName; 
    float fontSize; 
    Resource<IDWriteTextFormat> textFormat; 
  };

  Game& game;
  ID2D1HwndRenderTarget** ppRenderTarget;
  std::vector<FONT_ENTRY> textFormats;
};


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
  renderSurface = new RenderSurface(*this,&renderTarget);

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

  //Initialize all already registered view objects and create the RenderSurface
  if (SUCCEEDED(hr)) {
    for (auto viewObject : renderQueue) {
      viewObject->Initialize();
    }
  }
   
  initialized = true;

  return hr;
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

  // If initialization already took place, initialize the object upon insertion
  if (initialized)
    viewObject.Initialize();
}


HRESULT Game::CreateDeviceIndependentResources() {
  // Create a Direct2D factory.
  auto result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &direct2DFactory);

  if (SUCCEEDED(result))
    result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));  
    
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
  auto result = renderTarget->CreateSolidColorBrush(D2D1::ColorF(color), &brush);
  HandleCOMError(result, "Brush creation");
  
  brushMap[color] = brush;
  return brush;
}

void Game::ReleaseBrushes() {
  brushMap.clear();
}


void Game::DiscardDeviceResources()
{
  ReleaseBrushes();  
  renderTarget.reset();
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

  //Create device resources if not already done
  HRESULT res = CreateDeviceResources();
  if (SUCCEEDED(res)) {
    //Get the time since the last frame
    auto now = chrono::steady_clock::now();
    auto secondsPassed = chrono::duration<double>(now-lastUpdate).count();
    lastUpdate = now;

    //Give objects time to update positions
    for (auto viewObject : renderQueue) 
      viewObject->Update(secondsPassed);


    renderTarget->BeginDraw();  //Initiate drawing
    renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black)); //Set background color
    D2D1_SIZE_F rtSize = renderTarget->GetSize();


    //Render each object of the render queue
    for (auto viewObject : renderQueue) {
      //First draw bounding box (Only a debugging measure)
      if (viewObject->DrawBoundingBox())
        renderTarget->DrawRectangle(Rectangle(viewObject->pos,viewObject->size),GetSolidBrush(Color::Cyan),1.2);

      //Now draw the actual object
      viewObject->Draw(*renderSurface, secondsPassed);
    }


    


    
    
    //TODO factor out this process into own view object


    // Draw a grid background.
    int width = static_cast<int>(rtSize.width);
    int height = static_cast<int>(rtSize.height);



    for (int x = 0; x < width; ++x) {
      const float amplitude = height/5;
      const float horizScale = 0.03;
      renderTarget->DrawLine(Point2F(x, rtSize.height/2 + sin(x*horizScale)*amplitude), Point2F(x, rtSize.height), GetSolidBrush(ColorF::ForestGreen));
    }



    D2D1_RECT_F rectangle2 = D2D1::RectF(
        rtSize.width/2 - 100.0f,
        rtSize.height/2 - 100.0f,
        rtSize.width/2 + 100.0f,
        rtSize.height/2 + 100.0f
        );


    // Draw the outline of a rectangle.
    renderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(45, Point2F(rtSize.width/2, rtSize.height/2))); //Rotate following rectangle by 45 degrees
    renderTarget->DrawRectangle(&rectangle2, GetSolidBrush(Color::CornflowerBlue));

    renderTarget->SetTransform(D2D1::Matrix3x2F::Identity()); //Reset transform (rotation)
    
    
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


}
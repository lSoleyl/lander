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


/** Local implementation of the RenderInterface for this game.
 *  This class provides the implementation of all Draw calls, which are used within the game
 */
class RenderSurface : public RenderInterface {
public:
  RenderSurface(Game& game, ID2D1HwndRenderTarget** ppRenderTarget) : game(game), ppRenderTarget(ppRenderTarget) {}

  /** Returns the current size of the renderTarget
   */
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

  virtual void DrawLine(Vector from, Vector to, Color color, float strokeWidth = 1.0f) override {
    RenderTarget().DrawLine(from, to, game.GetSolidBrush(color), strokeWidth);
  }


  virtual void DrawImage(int resourceId, Rectangle targetRectangle) override {
    auto pos = loadedImages.find(resourceId);
    if (pos != loadedImages.end()) {
      // image already converted, render it
      RenderTarget().DrawBitmap(pos->second, targetRectangle);
    } else {
      // we have to load and prepare the image (boilerplate code -_-)
      if (!imageFactory) {
        HandleCOMError(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&imageFactory), "image factory creation");
      }

      // Load the rescoure
      auto resourceHandle = FindResource(NULL, MAKEINTRESOURCE(resourceId), _T("Image")/*type*/);
      if (!resourceHandle)
        throw std::exception("Couldn't find the image resource");

      auto loadedResource = LoadResource(NULL, resourceHandle);
      if (!loadedResource)
        throw std::exception("Failed to load the resource");

      auto resourceMemory = LockResource(loadedResource);
      if (!resourceMemory)
        throw std::exception("Failed to retrieve resource memory pointer");

      auto resourceMemSize = SizeofResource(NULL, resourceHandle);
      if (!resourceMemSize)
        throw std::exception("Failed to retrieve the resource's size");

      Resource<IWICStream> imageStream;
      HandleCOMError(imageFactory->CreateStream(&imageStream), "image stream creation");
      HandleCOMError(imageStream->InitializeFromMemory(reinterpret_cast<BYTE*>(resourceMemory),resourceMemSize), "image stream load");

      Resource<IWICBitmapDecoder> streamDecoder;
      HandleCOMError(imageFactory->CreateDecoderFromStream(imageStream, NULL, WICDecodeMetadataCacheOnLoad, &streamDecoder),"creating stream decoder");
      
      Resource<IWICBitmapFrameDecode> decoderFrame;
      HandleCOMError(streamDecoder->GetFrame(0, &decoderFrame), "decoding image frame");

      Resource<IWICFormatConverter> formatConverter;
      HandleCOMError(imageFactory->CreateFormatConverter(&formatConverter), "creating format converter");
      HandleCOMError(formatConverter->Initialize(
        decoderFrame,                    // Input bitmap to convert
        GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
        WICBitmapDitherTypeNone,         // Specified dither pattern
        NULL,                            // Specify a particular palette 
        0.f,                             // Alpha threshold
        WICBitmapPaletteTypeCustom       // Palette translation type
       ), "image decoding");

      Resource<ID2D1Bitmap> convertedBitmap;
      HandleCOMError(RenderTarget().CreateBitmapFromWicBitmap(formatConverter, NULL, &convertedBitmap), "conversion to direct2d image");

      // Finally insert loaded image into map of loaded images
      loadedImages[resourceId] = std::move(convertedBitmap);

      // Now that we made sure, the image is loaded, we can recall the DrawImage() method
      DrawImage(resourceId, targetRectangle);
    }
  }

  /** Provide access to internal renderTarget... should be removed in the future if possible
   */
  virtual ID2D1RenderTarget& RenderTarget() override { return **ppRenderTarget; }

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
  Resource<IWICImagingFactory> imageFactory;
  std::vector<FONT_ENTRY> textFormats;
  std::unordered_map<int/*resourceId*/, Resource<ID2D1Bitmap>> loadedImages; 
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
    viewObject.Initialize(renderSurface->Size());
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
    for (auto viewObject : renderQueue) 
      viewObject->Update(secondsPassed);


    renderTarget->BeginDraw();  //Initiate drawing
    renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black)); //Set background color
    D2D1_SIZE_F rtSize = renderTarget->GetSize();


    //Render each object of the render queue
    for (auto viewObject : renderQueue) {
      // Set the object's translation to have object relative coordinates
      // TODO add rotation later
      renderTarget->SetTransform(D2D1::Matrix3x2F::Translation(viewObject->pos.x, viewObject->pos.y));


      // First draw bounding box (Only a debugging measure)
      if (viewObject->DrawBoundingBox())
        renderTarget->DrawRectangle(Rectangle(Vector(0,0),viewObject->size),GetSolidBrush(Color::Cyan),1.2);

      // Now draw the actual object
      viewObject->Draw(*renderSurface, secondsPassed);
    }
    
    
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
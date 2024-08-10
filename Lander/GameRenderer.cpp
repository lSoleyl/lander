#include "stdafx.h"
#include "Game.hpp"
#include "Camera.hpp"
#include "GameRenderer.hpp"
#include "FontLoader.hpp"

#include <cmath>

namespace Lander {

GameRenderer::GameRenderer(Game& game, Camera& camera, ID2D1HwndRenderTarget** ppRenderTarget) : game(game), camera(camera), ppRenderTarget(ppRenderTarget), viewObject(nullptr) {}

GameRenderer::~GameRenderer() {
  // cleanup
  if (fontLoader) {
    writeFactory->UnregisterFontCollectionLoader(fontLoader);
  }
}

Lander::Size GameRenderer::Size() { 
  return RenderTarget().GetSize(); 
}

RenderInterface::TextFormat GameRenderer::CreateTextFormat(const wchar_t* fontName, float fontSize) {
  for(size_t i = 0; i < textFormats.size(); ++i) {
    if (textFormats[i].fontName == fontName && textFormats[i].fontSize == fontSize) {
      return static_cast<TextFormat>(i+1);
    }
  }
   
  // if not found
  // create write factory if this is the first use of CreateTextFormat (throw an exception if this fails)
  if (!writeFactory) {
    HandleCOMError(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory)), L"write factory creation");
  }

  if (!fontLoader) {
    fontLoader = FontLoader::Instance();
    HandleCOMError(writeFactory->RegisterFontCollectionLoader(fontLoader), L"Failed to register FontCollectionLoader");
  }

  Resource<IDWriteFontCollection> fontCollection; //null is the system font collection

  auto resourceId = fontLoader->GetFontResource(fontName);
  if (resourceId != FontLoader::NO_RESOURCE) {
    // we should load the font from the given resource
    auto resource = LoadBinaryResource(resourceId);
    HandleCOMError(writeFactory->CreateCustomFontCollection(fontLoader, &resource, sizeof(resource), &fontCollection), L"Failed to create custom font collection");  
  }
  

  IDWriteTextFormat* textFormat = nullptr;
  auto result = writeFactory->CreateTextFormat(fontName, fontCollection, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-us", &textFormat);
  HandleCOMError(result, L"TextFormat creation");

  textFormats.push_back(FONT_ENTRY(fontName, fontSize, textFormat));
  return static_cast<TextFormat>(textFormats.size());
}

void GameRenderer::DrawText(const std::wstring& text, TextFormat format, Rectangle rectangle, Color color) {
  assert(format > 0 && format <= textFormats.size()); //Ensure the text format is valid
  RenderTarget().DrawTextW(text.c_str(), static_cast<UINT32>(text.length()), textFormats[format-1].textFormat, rectangle, game.GetSolidBrush(color));
}


void GameRenderer::DrawLine(Vector from, Vector to, Color color, float strokeWidth) {
  RenderTarget().DrawLine(from, to, game.GetSolidBrush(color), strokeWidth); //TODO move GetSolidBrush() into GameRenderer class
}

void GameRenderer::DrawRectangle(Rectangle target, Color color, float strokeWidth) {
  RenderTarget().DrawRectangle(target, game.GetSolidBrush(color), strokeWidth);
}


void GameRenderer::FillRectangle(Rectangle target, Color color) {
  RenderTarget().FillRectangle(target, game.GetSolidBrush(color));
};


void GameRenderer::DrawImage(int resourceId, Rectangle targetRectangle) {
  auto pos = loadedImages.find(resourceId);
    if (pos != loadedImages.end()) {
      // image already converted, render it
      RenderTarget().DrawBitmap(pos->second, targetRectangle);
    } else {
      // Load the image from resource and insert it into the loaded images
      loadedImages[resourceId] = LoadImageResource(resourceId);

      // Now that we made sure, the image is loaded, we can recall the DrawImage() method
      DrawImage(resourceId, targetRectangle);
    }
}

void GameRenderer::DrawImage(int resourceId, Rectangle targetRectangle, float rotationAngle, bool rotateCenter) {
  D2D1::Matrix3x2F originalTransform;
  RenderTarget().GetTransform(&originalTransform);
  auto rotationPoint = camera.WorldToScreen(viewObject->pos) + targetRectangle.topLeft;
  if (rotateCenter) {
    rotationPoint += (targetRectangle.bottomRight - targetRectangle.topLeft) / 2; //rotation around center
  }

  RenderTarget().SetTransform(TranslationMatrix() * D2D1::Matrix3x2F::Rotation(rotationAngle, rotationPoint) * RotationMatrix());
  DrawImage(resourceId, targetRectangle);
  RenderTarget().SetTransform(originalTransform); //Restore original transform
}

void GameRenderer::DrawArc(Vector startPoint, Vector center, float angle, Color color, float strokeWidth) {
  angle = std::clamp(angle, -359.f, 359.f);

  // Do we really have to create a geometry object in each draw call for this?
  Resource<ID2D1PathGeometry> arcGeometry;
  Resource<ID2D1GeometrySink> geometrySink;
  HandleCOMError(game.direct2DFactory->CreatePathGeometry(&arcGeometry), L"createPathGeometry");
  HandleCOMError(arcGeometry->Open(&geometrySink), L"pathGeometry->Open()");

  // The vector from center to the start point, which will be rotated
  auto armVector = startPoint - center;
  float arcRadius = armVector.Length();

  D2D1_ARC_SEGMENT arc;
  // Calculate the arc's end point by rotating the arm vector by the specified angle and drawing the arg
  arc.point = armVector.Rotate(angle) + center;
  arc.size = { arcRadius, arcRadius };
  // This is not the arc angle, this is the rotation angle of the elipsis, which is used to draw the arc and is specified by the x and y radius
  arc.rotationAngle = 0;
  arc.sweepDirection = (angle >= 0) ? D2D1_SWEEP_DIRECTION::D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION::D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;
  arc.arcSize = (std::abs(angle) <= 180) ? D2D1_ARC_SIZE::D2D1_ARC_SIZE_SMALL : D2D1_ARC_SIZE::D2D1_ARC_SIZE_LARGE;

  geometrySink->BeginFigure(startPoint, D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_HOLLOW);
  geometrySink->AddArc(arc);
  geometrySink->EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_OPEN);
  HandleCOMError(geometrySink->Close(), L"geometrySink->Close()");

  // Since we don't use any transforms for drawing the arc, we can rely on the object's transforms being already correctly applied
  RenderTarget().DrawGeometry(arcGeometry, game.GetSolidBrush(color), strokeWidth);
}


void GameRenderer::DrawEllipse(Vector center, float radiusX, float radiusY, Color color, float strokeWidth) {
  D2D1_ELLIPSE ellipse;
  ellipse.point = center;
  ellipse.radiusX = radiusX;
  ellipse.radiusY = radiusY;

  RenderTarget().DrawEllipse(ellipse, game.GetSolidBrush(color), strokeWidth);
}



Resource<ID2D1Bitmap> GameRenderer::LoadImageResource(int resourceId) {
  // we have to load and prepare the image (boilerplate code -_-)
  if (!imageFactory) {
    HandleCOMError(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&imageFactory), L"image factory creation");
  }

  // Load the rescoure
  auto resource = LoadBinaryResource(resourceId);
  

  Resource<IWICStream> imageStream;
  HandleCOMError(imageFactory->CreateStream(&imageStream), L"image stream creation");
  HandleCOMError(imageStream->InitializeFromMemory(static_cast<BYTE*>(resource.data), static_cast<DWORD>(resource.size)), L"image stream load");

  Resource<IWICBitmapDecoder> streamDecoder;
  HandleCOMError(imageFactory->CreateDecoderFromStream(imageStream, NULL, WICDecodeMetadataCacheOnLoad, &streamDecoder), L"creating stream decoder");
      
  Resource<IWICBitmapFrameDecode> decoderFrame;
  HandleCOMError(streamDecoder->GetFrame(0, &decoderFrame), L"decoding image frame");

  Resource<IWICFormatConverter> formatConverter;
  HandleCOMError(imageFactory->CreateFormatConverter(&formatConverter), L"creating format converter");
  HandleCOMError(formatConverter->Initialize(
    decoderFrame,                    // Input bitmap to convert
    GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
    WICBitmapDitherTypeNone,         // Specified dither pattern
    NULL,                            // Specify a particular palette 
    0.f,                             // Alpha threshold
    WICBitmapPaletteTypeCustom       // Palette translation type
    ), L"image decoding");

  Resource<ID2D1Bitmap> convertedBitmap;
  HandleCOMError(RenderTarget().CreateBitmapFromWicBitmap(formatConverter, NULL, &convertedBitmap), L"conversion to direct2d image");

  return convertedBitmap;
}

Data GameRenderer::LoadBinaryResource(int resourceId) {
  // Load the rescoure
  auto resourceHandle = FindResource(NULL, MAKEINTRESOURCE(resourceId), _T("Image")/*type*/);
  if (!resourceHandle) {
    throw std::exception("Couldn't find the image resource");
  }

  auto loadedResource = LoadResource(NULL, resourceHandle);
  if (!loadedResource) {
    throw std::exception("Failed to load the resource");
  }

  Data resource = {0};

  resource.data = static_cast<BYTE*>(LockResource(loadedResource));
  if (!resource.data) {
    throw std::exception("Failed to retrieve resource memory pointer");
  }

  resource.size = SizeofResource(NULL, resourceHandle);
  if (!resource.size) {
    throw std::exception("Failed to retrieve the resource's size");
  }

  return resource;
}

/** Provide access to internal renderTarget... should be removed in the future if possible
  */
ID2D1RenderTarget& GameRenderer::RenderTarget() {
  return **ppRenderTarget;
}

D2D1::Matrix3x2F GameRenderer::TranslationMatrix() const {
  auto position = viewObject->GetScreenPosition(camera);
  return D2D1::Matrix3x2F::Translation(position.x, position.y);
}

D2D1::Matrix3x2F GameRenderer::RotationMatrix() const {
  return D2D1::Matrix3x2F::Rotation(viewObject->rotation, RotationCenter());
}

void GameRenderer::DrawObject(ViewObject* currentObject, double secondsPassed) {
  viewObject = currentObject;

  // Set the object's translation to have object relative coordinates
  RenderTarget().SetTransform(TranslationMatrix() * RotationMatrix());
  

  // First draw bounding box (Only a debugging measure)
  if (viewObject->DrawBoundingBox()) {
    auto color = (!viewObject->enabled) ? Color::Red : (!viewObject->visible) ? Color::Magenta : Color::Cyan;
    DrawRectangle(Rectangle(Vector(0,0),viewObject->size), color, 1.2f);
  }

  if (viewObject->enabled && viewObject->visible) {
    // Now draw the actual object
    viewObject->Draw(*this, camera.GetVisibleRect(), secondsPassed);
  }
}

Vector GameRenderer::RotationCenter() const {
  return camera.WorldToScreen(viewObject->pos) + (viewObject->size.height/2 * Vector::Down) + (viewObject->size.width/2 * Vector::Right);
}

GameRenderer::FONT_ENTRY::FONT_ENTRY(const wchar_t* fontName, float fontSize, IDWriteTextFormat* format) : fontName(fontName), fontSize(fontSize), textFormat(format) {}
















}


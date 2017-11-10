#include "stdafx.h"
#include "Game.hpp"
#include "GameRenderer.hpp"

namespace Lander {

GameRenderer::GameRenderer(Game& game, ID2D1HwndRenderTarget** ppRenderTarget) : game(game), ppRenderTarget(ppRenderTarget), viewObject(nullptr) {}

Lander::Size GameRenderer::Size() { 
  return RenderTarget().GetSize(); 
}

RenderInterface::TextFormat GameRenderer::CreateTextFormat(const wchar_t* fontName, float fontSize) {
  for(size_t i = 0; i < textFormats.size(); ++i)
    if (textFormats[i].fontName == fontName && textFormats[i].fontSize == fontSize)
      return static_cast<TextFormat>(i+1);

  // if not found...
  // create write factory if this is the first use of CreateTextFormat (throw an exception if this fails)
  if (!writeFactory) {
    HandleCOMError(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory)), "write factory creation");
  }

  // no create the specified text format
  IDWriteTextFormat* textFormat = nullptr;
  auto result = writeFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-us", &textFormat);
  HandleCOMError(result, "TextFormat creation");
    
  textFormats.push_back(FONT_ENTRY());
  textFormats.back() = FONT_ENTRY(fontName, fontSize, textFormat);
  return static_cast<TextFormat>(textFormats.size());
}

void GameRenderer::DrawText(const std::wstring& text, TextFormat format, Rectangle rectangle, Color color) {
  assert(format > 0 && format <= textFormats.size()); //Ensure the text format is valid
  RenderTarget().DrawText(text.c_str(), static_cast<UINT32>(text.length()), textFormats[format-1].textFormat, rectangle, game.GetSolidBrush(color));
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
  auto rotationPoint = viewObject->pos + targetRectangle.topLeft;
  if (rotateCenter)
    rotationPoint += (targetRectangle.bottomRight - targetRectangle.topLeft) / 2; //rotation around center

  RenderTarget().SetTransform(D2D1::Matrix3x2F::Translation(viewObject->pos.x, viewObject->pos.y) * D2D1::Matrix3x2F::Rotation(rotationAngle, rotationPoint) * D2D1::Matrix3x2F::Rotation(viewObject->rotation, RotationCenter()));
  DrawImage(resourceId, targetRectangle);
  RenderTarget().SetTransform(originalTransform); //Restore original transform
}

Resource<ID2D1Bitmap> GameRenderer::LoadImageResource(int resourceId) {
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

  return std::move(convertedBitmap);
}

/** Provide access to internal renderTarget... should be removed in the future if possible
  */
ID2D1RenderTarget& GameRenderer::RenderTarget() {
  return **ppRenderTarget;
}

void GameRenderer::DrawObject(ViewObject* currentObject, double secondsPassed) {
  viewObject = currentObject;

  // Set the object's translation to have object relative coordinates
  RenderTarget().SetTransform(D2D1::Matrix3x2F::Translation(viewObject->pos.x, viewObject->pos.y) * D2D1::Matrix3x2F::Rotation(viewObject->rotation, RotationCenter()));
  

  // First draw bounding box (Only a debugging measure)
  if (viewObject->DrawBoundingBox()) {
    auto color = (!viewObject->enabled) ? Color::Red : (!viewObject->visible) ? Color::Magenta : Color::Cyan;
    DrawRectangle(Rectangle(Vector(0,0),viewObject->size), color, 1.2f);
  }

  if (viewObject->enabled && viewObject->visible) {
    // Now draw the actual object
    viewObject->Draw(*this, secondsPassed);
  }
}

Vector GameRenderer::RotationCenter() const {
  return viewObject->pos + (viewObject->size.height/2 * Vector::Down) + (viewObject->size.width/2 * Vector::Right);
}

GameRenderer::FONT_ENTRY::FONT_ENTRY() {};
GameRenderer::FONT_ENTRY::FONT_ENTRY(const wchar_t* fontName, float fontSize, IDWriteTextFormat* format) : fontName(fontName), fontSize(fontSize), textFormat(format) {}
GameRenderer::FONT_ENTRY::FONT_ENTRY(FONT_ENTRY&& other) : fontName(std::move(fontName)), fontSize(fontSize), textFormat(std::move(textFormat)) {}

GameRenderer::FONT_ENTRY& GameRenderer::FONT_ENTRY::operator=(FONT_ENTRY&& other) {
  fontSize = other.fontSize;
  fontName.swap(other.fontName);      
  textFormat = std::move(other.textFormat);
  return *this;
}
















}


#include "stdafx.h"
#include "Rocket.hpp"
#include "Platform.hpp"

namespace Lander {
Rocket::Rocket(const Platform& startPlatform) : startPlatform(startPlatform) {
  size = Size(startPlatform.size.width*2/3, 100); //Rocket has smaller width than the platform, it is starting from
}


void Rocket::Update(double secondsSinceLastFrame) {
  // TODO: only adapt position so long until the user adds thrust to the rocket
  pos = startPlatform.pos + Vector::Up * size.height; //Calculate top position of rocket
  pos += Vector::Right * (startPlatform.size.width - size.width)/2; //Center rocket on start platform
}

void Rocket::Draw(RenderInterface& renderTarget, double secondsSinceLastFrame) {
  

  // WIC interface pointers.ge
  IWICImagingFactory *m_pIWICFactory = NULL;
  IWICFormatConverter* m_pConvertedSourceBitmap = NULL;
  IWICStream *pIWICStream = NULL;
  IWICBitmapDecoder *pIDecoder = NULL;
  IWICBitmapFrameDecode *pIDecoderFrame = NULL;
  static ID2D1Bitmap* direct2DBitmap = NULL;

  // Resource management.
  HRSRC imageResHandle = NULL;
  HGLOBAL imageResDataHandle = NULL;
  void *pImageFile = NULL;
  DWORD imageFileSize = 0;

  // Draws an image and scales it to the current window size
  if (direct2DBitmap) {
      renderTarget.RenderTarget().DrawBitmap(direct2DBitmap, Rectangle(Vector(), size));
      return;
  } 
    



  HRESULT hr = CoCreateInstance(
    CLSID_WICImagingFactory,
    NULL,
    CLSCTX_INPROC_SERVER,
    IID_IWICImagingFactory,
    (LPVOID*)&m_pIWICFactory
    );

  if (SUCCEEDED(hr)) {
    // Locate the resource in the application's executable.
    imageResHandle = FindResource(NULL, MAKEINTRESOURCE(IDR_ROCKET_IMAGE), _T("Image")/*type*/);


    hr = (imageResHandle ? S_OK : E_FAIL);
  }

  // Load the resource to the HGLOBAL.
  if (SUCCEEDED(hr)){
     imageResDataHandle = LoadResource(NULL, imageResHandle);
     hr = (imageResDataHandle ? S_OK : E_FAIL);
  }

  // Lock the resource to retrieve memory pointer.
  if (SUCCEEDED(hr)){
     pImageFile = LockResource(imageResDataHandle);
     hr = (pImageFile ? S_OK : E_FAIL);
  }

  // Calculate the size.
  if (SUCCEEDED(hr)){
     imageFileSize = SizeofResource(NULL, imageResHandle);
     hr = (imageFileSize ? S_OK : E_FAIL);
  }

  // Create a WIC stream to map onto the memory.
  if (SUCCEEDED(hr)){
     hr = m_pIWICFactory->CreateStream(&pIWICStream);
  }

  // Initialize the stream with the memory pointer and size.
  if (SUCCEEDED(hr)){
     hr = pIWICStream->InitializeFromMemory(
           reinterpret_cast<BYTE*>(pImageFile),
           imageFileSize);
  }

  // Create a decoder for the stream.
  if (SUCCEEDED(hr)){
     hr = m_pIWICFactory->CreateDecoderFromStream(
        pIWICStream,                   // The stream to use to create the decoder
        NULL,                          // Do not prefer a particular vendor
        WICDecodeMetadataCacheOnLoad,  // Cache metadata when needed
        &pIDecoder);                   // Pointer to the decoder
  }

  // Retrieve the initial frame.
  if (SUCCEEDED(hr)){
     hr = pIDecoder->GetFrame(0, &pIDecoderFrame);
  }

   // Format convert the frame to 32bppPBGRA
   if (SUCCEEDED(hr))
   {
       hr = m_pIWICFactory->CreateFormatConverter(&m_pConvertedSourceBitmap);
   }

   if (SUCCEEDED(hr))
   {
       hr = m_pConvertedSourceBitmap->Initialize(
           pIDecoderFrame,                  // Input bitmap to convert
           GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
           WICBitmapDitherTypeNone,         // Specified dither pattern
           NULL,                            // Specify a particular palette 
           0.f,                             // Alpha threshold
           WICBitmapPaletteTypeCustom       // Palette translation type
           );
   }


   if (SUCCEEDED(hr)) {
     hr = renderTarget.RenderTarget().CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap,NULL,&direct2DBitmap);
   }

}




}
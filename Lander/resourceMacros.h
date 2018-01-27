#ifdef RC_INVOKED 
// Definitions for the resource compiler

#define START_IMAGES
#define DEFINE_IMAGE(name, number, path) number IMAGE path
#define END_IMAGES


#define START_FONTS
#define DEFINE_FONT(name, number, path) number IMAGE path
#define END_FONTS



#else

  // Only define the symbols once in the resources.cpp
  #ifdef RESOURCE_CPP
    #include "FontLoader.hpp"

    #undef START_IMAGES
    #undef DEFINE_IMAGE
    #undef END_IMAGES
    #undef START_FONTS
    #undef DEFINE_FONT
    #undef END_FONTS


    #define START_IMAGES namespace Lander {
    #define DEFINE_IMAGE(name, number, path) const int IDR_ ## name ## _IMAGE = number;
    #define END_IMAGES }




    #define START_FONTS namespace Lander { void registerFonts() { \
      auto fontLoader = Lander::FontLoader::Instance();

    #define DEFINE_FONT(name, number, path) fontLoader->RegisterFontResource(L##name, number);

    #define END_FONTS } }

  #else
  // How the resource.h looks to every other including cpp/hpp file (declarations)

    #define START_IMAGES namespace Lander {
    #define DEFINE_IMAGE(name, number, path) extern const int IDR_ ## name ## _IMAGE;
    #define END_IMAGES }


    #define START_FONTS namespace Lander { extern void registerFonts(); }
    #define DEFINE_FONT(name, number, path)
    #define END_FONTS

  #endif

#endif

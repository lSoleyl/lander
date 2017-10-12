#pragma once

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>

// C++ stl headers
#include <cmath>
#include <cassert>
#include <algorithm>

#include <string>
#include <sstream>
#include <tchar.h>

//STL collections
#include <unordered_map>
#include <vector>
#include <deque>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

//Utility classes
#include "Resource.hpp"
#include "COMError.hpp"
#include "Vector.hpp"


#include "RenderInterface.hpp"
#include "ViewObject.hpp"

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

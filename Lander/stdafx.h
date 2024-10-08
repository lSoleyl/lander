#pragma once

// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>

// C++ stl headers
#include <cmath>
#include <cassert>
#include <algorithm>
#include <memory>

#include <string>
#include <sstream>
#include <tchar.h>

//STL collections
#include <unordered_map>
#include <vector>
#include <deque>

// Windows Header Files:
#include <windows.h>

//Direct2D stuff
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include "resources.h" //Our resources

//Utility classes
#include "Resource.hpp"
#include "COMError.hpp"
#include "COMBase.hpp"
#include "Vector.hpp"
#include "Data.hpp"

#include "RenderInterface.hpp"
#include "ViewObject.hpp"
#include "OverlayObject.hpp"
#include "Collider.hpp"
#include "PhysicsObject.hpp"

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

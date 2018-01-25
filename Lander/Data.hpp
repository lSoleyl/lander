#pragma once

/** Simply a small utitlity class for handling binary data
 */

#include <windef.h>

namespace Lander {

  struct Data {
    BYTE* data;
    size_t size;
  };
}
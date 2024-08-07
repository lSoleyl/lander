#pragma once

#include <sstream>


#define OutputDebugStream(text) do { \
    std::ostringstream out; \
    out << text << std::endl; \
    OutputDebugString(out.str().c_str()); \
  } while(0)

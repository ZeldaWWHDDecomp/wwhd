// GameCube API bindings for verified HD callers.
#pragma once
#include "bindings.h"
/* 027EC9E8 [v d_a_bmd]: the integer report overload. */
inline void JUTReport(s32 x, s32 y, const char* format, s32 value) { gabi::call(0x027EC9E8, x, y, format, value); }

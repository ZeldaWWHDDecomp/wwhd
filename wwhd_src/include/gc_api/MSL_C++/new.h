/* C++ array allocation, WWHD binding (GameCube: MSL_C++/MSL_Common/Include/new.h). */
#pragma once
#include "wwhd.h"

/* 0273ADAC operator new[] [v d_bg_w_deform] */
inline void* operator_new_array(u32 size) { return gabi::call<void*>(0x0273ADAC, size); }

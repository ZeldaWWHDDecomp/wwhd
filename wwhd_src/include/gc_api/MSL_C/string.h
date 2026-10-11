/* C library memory API (GameCube: MSL_C/MSL_Common/Include/string.h). */
#pragma once
#include "bindings.h"

/* 028F521C bzero thunk (tail-calls memset) [v c_m2d_g_box] */
inline void bzero(void* dst, u32 size) { gabi::call(0x028F521C, dst, size); }

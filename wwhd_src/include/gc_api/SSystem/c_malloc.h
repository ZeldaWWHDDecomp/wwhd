/* c_malloc / s_basic (SSystem), GameCube names -> WWHD.  [v f_pc_base] */
#pragma once
#include "wwhd.h"

struct cMl {
    static void* memalignB(int align, u32 size) { return gabi::call<void*>(0x02019430, align, size); }
    static void free(void* p) { gabi::call(0x0201945C, p); }
};
inline void sBs_ClearArea(void* p, u32 size) { gabi::call(0x0201B680, p, size); }

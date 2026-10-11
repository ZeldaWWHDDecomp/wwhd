/* Switch trigger, WWHD layout. No members beyond the 0x3AC actor base.
 * GameCube has the same empty derived class; HD adds a virtual destructor. */
#pragma once
#include "gc_api/gc_api.h"

class swc00_class : public fopAc_ac_c {
public:
    swc00_class();
    enum : u32 { __vt = 0x1003EAC4 };
};
WWHD_SIZE(swc00_class, 0x3AC);

static inline int daSwc00_getSw1No(swc00_class* i_this) { return fopAcM_GetParam(i_this) & 0xFF; }
static inline int daSwc00_getSw2No(swc00_class* i_this) { return (fopAcM_GetParam(i_this) >> 8) & 0xFF; }
static inline int daSwc00_getType(swc00_class* i_this) { return (fopAcM_GetParam(i_this) >> 0x10) & 0x3; }

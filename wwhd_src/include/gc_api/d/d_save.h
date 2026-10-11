// Stage-memory mutation for verified HD callers.
#pragma once
#include "d/d_save.h"
/* 025B9098 [v d_a_bmd] */
inline void dSv_memBit_onDungeonItem(dSv_memBit_c* memory, s32 item) { gabi::call(0x025B9098, memory, item); }

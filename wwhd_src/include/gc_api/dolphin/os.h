/* Dolphin cache API [v d_ky_thunder]. */
#pragma once
#include "bindings.h"
/* HD: C00088B8 Wii U cache syscall corresponding to DCStoreRangeNoSync. */
inline void DCStoreRangeNoSync(void* start, u32 size) { gabi::call(0xC00088B8, start, size); }

// Range-cache ordering and release (AGL#18 functions on AGL#17's agl::cache::RangeCache / RangeRecord):
// the records link into the cache's lists through RangeRecord::mLink (+0x14, Lead01:23).
#pragma once
#include "agl/RangeCache.h"

namespace agl {
// 0279F6B0 compares three caches 0x2E8 (sizeof(RangeCache)) apart: the left cache's first available range
// against the right cache's last one.
u32 isAvailableRangeAfter(const cache::RangeCache* left, const cache::RangeCache* right);
}

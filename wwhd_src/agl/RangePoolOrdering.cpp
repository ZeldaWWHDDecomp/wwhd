// Compare the low available range of one range cache against the high available range of another.
#include "agl/RangePoolOrdering.h"

namespace agl {
/* 027CB744 */
u32 isAvailableRangeAfter(const cache::RangeCache* left, const cache::RangeCache* right) {
    u32 firstStart = 0;
    if (s32(left->mAvailableRanges.mCount) > 0) {
        sead::Link* first = left->mAvailableRanges.mPrevious;
        if (gabi::ea(first) != 0) {
            u32 offset = left->mAvailableRanges.mOffset;
            u32 rangeAddress = gabi::ea(first) - offset;
            if (rangeAddress != 0)
                firstStart = gabi::at<cache::RangeRecord>(rangeAddress)->mBegin;
        }
    }
    if (s32(right->mAvailableRanges.mCount) > 0) {
        sead::Link* last = right->mAvailableRanges.mNext;
        if (gabi::ea(last) != 0) {
            u32 offset = right->mAvailableRanges.mOffset;
            u32 rangeAddress = gabi::ea(last) - offset;
            if (rangeAddress != 0) {
                cache::RangeRecord* range = gabi::at<cache::RangeRecord>(rangeAddress);
                u32 start = range->mBegin;
                u32 bytes = range->mSize;
                return firstStart > start + bytes;
            }
        }
    }
    return firstStart > 0;
}
}

/* WWHD glue
*/

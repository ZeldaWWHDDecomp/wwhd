/* Partial geometry-storage views; no external layout reference used. */
#pragma once
#include "wwhd.h"
#include "sead/HeapRegistry.h"
namespace agl { namespace gx2util {
struct GeometryTriple { be<f32> x, y, z; };
struct BufferedGeometryText { gptr<be<char>> mText; be<u32> mVtable; be<u32> mCapacity; void assureTermination(); };
struct GeometryAllocation { be<u32> mCapacity; gptr<void> mData; be<u32> mCount; };
struct GeometryOwner { be<s32> mCapacity; gptr<gptr<GeometryAllocation>> mEntries; be<u32> mCount; };
using GeometryHeap = sead::RegistryHeap;
struct GeometryPointList {
    be<u32> mCapacity; gptr<void> mData; be<s32> mCount;
};
struct GeometryGroupSet;
struct GeometryPointGroups {
    be<u32> mCapacity; gptr<gptr<GeometryPointList>> mEntries; be<s32> mCount;
    u8 mReserved[0x80]; be<f32> mTolerance;
    s32 findPoint(GeometryPointList* points, const GeometryTriple* query) const;
    void swap(u32 first, u32 second);
    s32 moveMatchingPair(const void* query);
    u32 joinPairs(GeometryGroupSet* destination);
};
WWHD_SIZE(GeometryTriple, 0xC);
WWHD_SIZE(BufferedGeometryText, 0xC);
WWHD_OFFSET(BufferedGeometryText, mCapacity, 8);
WWHD_SIZE(GeometryAllocation, 0xC);
WWHD_OFFSET(GeometryAllocation, mData, 4);
WWHD_SIZE(GeometryOwner, 0xC);
WWHD_OFFSET(GeometryOwner, mEntries, 4);
WWHD_OFFSET(GeometryHeap, mObject, 0);
WWHD_OFFSET(sead::ObjectHeader, mTable, 0xC);
} }

// HD: GPU buffer objects and fixed pools emitted into the JPA translation units.
#pragma once
#include "wwhd.h"
struct JPAGpuVertexBuffer {
    u8 mPrefix[0x14C];
    gptr<void> mData;
    u8 mPadding[4];
};
WWHD_SIZE(JPAGpuVertexBuffer, 0x154);
struct JPAGpuBuffer {
    union { be<u32> mInUse; gptr<void> mBufferData; };
    JPAGpuVertexBuffer mVertexBuffer;
    u8 mShape[0xF4];
    union { be<u32> mData; be<u32> mCapacity; };
    union { be<u32> mCount; gptr<void> mAllocation; };
};
struct JPAGpuPoolState {
    be<u32> mSwapIndex, mCursor, mElementSize;
    union { u8 mReserved[4]; gptr<void> mFormat; };
    union { be<u32> mShapeEnabled; gptr<void> mMaterial; };
    be<u8> mInitialized;
    u8 mPadding[3];
};
template <u32 N> struct JPAGpuPool {
    JPAGpuBuffer mBuffers[N];
    JPAGpuPoolState mState;
};
WWHD_SIZE(JPAGpuBuffer, 0x254);
WWHD_OFFSET(JPAGpuBuffer, mVertexBuffer, 4);
WWHD_OFFSET(JPAGpuBuffer, mShape, 0x158);
WWHD_OFFSET(JPAGpuBuffer, mData, 0x24C);
WWHD_OFFSET(JPAGpuBuffer, mCount, 0x250);
WWHD_SIZE(JPAGpuPoolState, 0x18);
WWHD_OFFSET(JPAGpuPoolState, mInitialized, 0x14);
WWHD_SIZE(JPAGpuPool<2>, 0x4C0);
WWHD_SIZE(JPAGpuPool<4>, 0x968);
WWHD_SIZE(JPAGpuPool<10>, 0x1760);
WWHD_SIZE(JPAGpuPool<300>, 0x2BA88);

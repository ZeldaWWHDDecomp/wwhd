/* WWHD vertex-buffer attribute layout from this unit's constructors and field accesses. */
#pragma once
#include "sead/FileDevice.h"
namespace agl {
struct FetchFormatTable { be<u32> mValues[20]; };
WWHD_SIZE(FetchFormatTable,80);
struct BufferAttribute {
    be<u32> mFormat;
    be<u32> mOffset;
    be<u8> mEnabled;
    be<u8> mBufferIndex;
    u8 mPadding[2];
    be<u32> mFetchFormat;
    be<u32> mIndexMode;
    static BufferAttribute* construct(BufferAttribute* attribute);
};
using AttributeConstructor = BufferAttribute* (*)(BufferAttribute*);
struct VertexBufferLayout {
    BufferAttribute mAttributes[16];
    gptr<void> mStorage;
    be<u32> mStride;
    be<u32> mCount;
    be<u32> mStorageSize;
    gptr<sead::DisposerVtable> mVtable;
    VertexBufferLayout* construct();
    void clear();
    void destroy(u32 flags);
    void setStorage(void* storage, u32 stride, u32 bytes);
    void setAttribute(u32 index, u32 format, u32 offset, u32 bufferIndex);
    void flush(u32 offset, u32 bytes);
};
WWHD_OFFSET(BufferAttribute,mFormat,0);
WWHD_OFFSET(BufferAttribute,mOffset,4);
WWHD_OFFSET(BufferAttribute,mEnabled,8);
WWHD_OFFSET(BufferAttribute,mBufferIndex,9);
WWHD_OFFSET(BufferAttribute,mFetchFormat,12);
WWHD_OFFSET(BufferAttribute,mIndexMode,16);
WWHD_SIZE(BufferAttribute,20);
WWHD_OFFSET(VertexBufferLayout,mStorage,0x140);
WWHD_OFFSET(VertexBufferLayout,mStride,0x144);
WWHD_OFFSET(VertexBufferLayout,mCount,0x148);
WWHD_OFFSET(VertexBufferLayout,mStorageSize,0x14C);
WWHD_OFFSET(VertexBufferLayout,mVtable,0x150);
WWHD_SIZE(VertexBufferLayout,0x154);
}

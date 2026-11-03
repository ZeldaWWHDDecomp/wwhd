/* WWHD index-buffer storage prefix, reconstructed from this unit's instructions. */
#pragma once
#include "sead/FileDevice.h"
namespace agl {
struct IndexBufferLayout {
    be<u32> mFormat;
    be<u32> mPrimitiveMode;
    gptr<void> mStorage;
    be<u32> mIndexCount;
    be<u32> mIndexBytes;
    gptr<sead::DisposerVtable> mVtable;
    IndexBufferLayout* construct();
    void clear();
    void destroy(u32 flags);
    void setStorage(void* storage, u32 format, u32 count);
};
WWHD_OFFSET(IndexBufferLayout,mFormat,0);
WWHD_OFFSET(IndexBufferLayout,mPrimitiveMode,4);
WWHD_OFFSET(IndexBufferLayout,mStorage,8);
WWHD_OFFSET(IndexBufferLayout,mIndexCount,12);
WWHD_OFFSET(IndexBufferLayout,mIndexBytes,16);
WWHD_OFFSET(IndexBufferLayout,mVtable,20);
WWHD_SIZE(IndexBufferLayout,24);
}

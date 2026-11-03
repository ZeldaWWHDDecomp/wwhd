// AGL recording and capture-adjacent records reconstructed from WWHD instructions.
#pragma once
#include "sead/HeapRegistry.h"
namespace agl {
struct DisplayListStorage {
    gptr<void> mData;
    be<u32> mCapacity;
    be<u32> mUsed;
};
struct DisplayListRecording : DisplayListStorage {
    sead::SafeText mName;
    be<u8> mFlags;
    u8 mAlignment[3];
    gptr<sead::DisposerVtable> mVtable;
    static DisplayListRecording* create(DisplayListRecording* storage);
    void setStorage(void* data,u32 capacity);
    void clear();
    void release();
    void destroy(u32 flags);
    void copyStorage(DisplayListStorage* destination) const;
    u32 begin(u32 index);
    u32 end(u32 index);
};
// Only lifecycle interfaces are known for these policy objects; no complete extent is claimed.
struct CaptureEncodingPolicy { void destroy(u32 flags); void finish(); };
struct RecordingPolicy { void destroy(u32 flags); void finish(); static u32 alignment(); };
struct RenderStorageHandle {
    u8 mState[8];
    static RenderStorageHandle* create(RenderStorageHandle* storage);
    void destroy(u32 flags);
};
struct RenderAddressRange {
    gptr<void> mData;
    be<u32> mCount;
    be<u32> mCapacity;
    static RenderAddressRange* create(RenderAddressRange* storage);
    void destroy(u32 flags);
    void finish();
};
struct VertexBufferPrefix_l;
struct VertexAttributeBinding {
    gptr<VertexBufferPrefix_l> mSource;
    be<s32> mAttribute;
    be<s32> mBufferSlot;
    static VertexAttributeBinding* create(VertexAttributeBinding* storage);
};
void waitForGraphicsCompletion();
}
WWHD_SIZE(agl::DisplayListStorage,12);
WWHD_OFFSET(agl::DisplayListStorage,mCapacity,4);
WWHD_OFFSET(agl::DisplayListStorage,mUsed,8);
WWHD_SIZE(agl::DisplayListRecording,0x1C);
WWHD_OFFSET(agl::DisplayListRecording,mName,0xC);
WWHD_OFFSET(agl::DisplayListRecording,mFlags,0x14);
WWHD_OFFSET(agl::DisplayListRecording,mVtable,0x18);
WWHD_SIZE(agl::RenderStorageHandle,8);
WWHD_SIZE(agl::RenderAddressRange,12);
WWHD_OFFSET(agl::RenderAddressRange,mCount,4);
WWHD_OFFSET(agl::RenderAddressRange,mCapacity,8);
WWHD_SIZE(agl::VertexAttributeBinding,12);
WWHD_OFFSET(agl::VertexAttributeBinding,mAttribute,4);
WWHD_OFFSET(agl::VertexAttributeBinding,mBufferSlot,8);

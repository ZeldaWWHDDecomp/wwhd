// Render command pool: per-frame recording slices over one GPU allocation plus 64-byte timed command records.
// Layouts from the pool's own constructor (0x48 bytes, 027D5064), initializer 027D527C and destructor 027D514C;
// RenderQueue's RenderDependencyGraphStorage_l is this object. The command records are gx2util::TimedCounter
// (constructor 027DA694, which first builds an agl::DisplayListRecording at the record's start).
#pragma once
#include "wwhd.h"
#include "sead/FileDevice.h"
#include "sead/HeapRegistry.h"
#include "agl/gx2/RuntimeUtility.h"
namespace agl { namespace render {
// One frame's slice of a ring's recording memory.
struct RenderCommandSlice {
    be<u32> mSize;
    gptr<void> mData;
};
// One recording ring: a buffer of per-frame slices, the fill offset of the current slice and its open command.
struct RenderCommandRing {
    be<u32> mCapacity;
    gptr<RenderCommandSlice> mSlices;
    be<u32> mOffset;
    gptr<gx2util::TimedCounter> mCurrentCommand;
};
struct RenderCommandPool {
    gptr<void> mAllocation;
    be<u32> mNextCommand;
    RenderCommandRing mRings[3];
    be<u32> mCommandCapacity;
    gptr<gx2util::TimedCounter> mCommands;
    be<u8> mFrameCount;
    be<u8> mCurrentFrame;
    be<u8> mFlags;
    u8 mPadding;
    gptr<sead::DisposerVtable> mVtable;
    void advanceFrame(u32 advance);
    void initialize(u32 bytesPerFrame,s32 commandCount,u32 frameCount,sead::RegistryHeap* heap);
    u32 beginCommand(const sead::SafeText* name,u32 request,u32 ring);
    gx2util::TimedCounter* endCommand(u32 index,u32 ring);
};
WWHD_SIZE(RenderCommandSlice,8);
WWHD_OFFSET(RenderCommandSlice,mData,4);
WWHD_SIZE(RenderCommandRing,16);
WWHD_OFFSET(RenderCommandRing,mSlices,4);
WWHD_OFFSET(RenderCommandRing,mOffset,8);
WWHD_OFFSET(RenderCommandRing,mCurrentCommand,0xC);
WWHD_SIZE(RenderCommandPool,0x48);
WWHD_OFFSET(RenderCommandPool,mNextCommand,4);
WWHD_OFFSET(RenderCommandPool,mRings,8);
WWHD_OFFSET(RenderCommandPool,mCommandCapacity,0x38);
WWHD_OFFSET(RenderCommandPool,mCommands,0x3C);
WWHD_OFFSET(RenderCommandPool,mFrameCount,0x40);
WWHD_OFFSET(RenderCommandPool,mCurrentFrame,0x41);
WWHD_OFFSET(RenderCommandPool,mFlags,0x42);
WWHD_OFFSET(RenderCommandPool,mVtable,0x44);
RenderCommandRing* createRenderCommandRing(RenderCommandRing* ring);
RenderCommandPool* createRenderCommandPool(RenderCommandPool* pool);
void destroyRenderCommandPool(RenderCommandPool* pool,u32 flags);
} }

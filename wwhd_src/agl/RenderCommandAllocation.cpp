// Render command pool initialization: one allocation split into per-ring, per-frame slices, then the command records.
// Every heap allocation resolves the heap again (the given one, else the registry's current heap).
#include "agl/RenderCommandPool.h"
#include "agl/RenderRecording.h"
using namespace agl::render;
void* allocateArrayFromHeap(u32 bytes,sead::RegistryHeap* heap,s32 alignment);
agl::gx2util::TimedCounter* constructTimedCounter(agl::gx2util::TimedCounter* counter);
void flushDataCache(void* data,u32 bytes);
static void* allocateFromHeap(sead::RegistryHeap* heap,u32 bytes) {
    if (!heap) heap = registryCurrentHeap(sRegistryManager);
    return gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction,heap,bytes,4);
}
/* 027D527C */
void agl::render::RenderCommandPool::initialize(u32 bytesPerFrame,s32 commandCount,u32 frameCount,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mFrameCount = frameCount;
    u32 sliceBytes = bytesPerFrame + agl::RecordingPolicy::alignment() - 1u;
    sliceBytes &= ~(agl::RecordingPolicy::alignment() - 1u);
    u32 total = (u32)(u8)mFrameCount * sliceBytes * 3u;
    if (total) {
        void* allocation = allocateArrayFromHeap(total,heap,0x40);
        mAllocation = allocation;
        be<u32>* words = static_cast<be<u32>*>(allocation);
        for (u32 word = 0; word < (total >> 2); ++word) words[word] = 0xBEEF2929u;
        u32 offset = 0;
        for (RenderCommandRing* ring = mRings; ring != mRings + 3; ++ring) {
            ring->mOffset = 0;
            u32 count = (u8)mFrameCount;
            if (count) {
                RenderCommandSlice* slices = static_cast<RenderCommandSlice*>(allocateFromHeap(heap,count * sizeof(RenderCommandSlice)));
                for (u32 index = 0; index < count; ++index) {
                    RenderCommandSlice* slice = &slices[index];
                    if (gabi::ea(slice)) { slice->mData = nullptr; slice->mSize = 0; }
                }
                if (slices) { ring->mSlices = slices; ring->mCapacity = count; }
            }
            RenderCommandSlice* slice = ring->mSlices;
            RenderCommandSlice* end = slice + (u32)ring->mCapacity;
            ring->mCurrentCommand = nullptr;
            for (; slice != end; ++slice) {
                u8* data = static_cast<u8*>(static_cast<void*>(mAllocation)) + offset;
                if ((s32)sliceBytes > 0 && gabi::ea(data)) {
                    slice->mData = data;
                    slice->mSize = sliceBytes;
                }
                offset += sliceBytes;
            }
        }
        flushDataCache(mAllocation,total);
    }
    if (commandCount && commandCount > 0) {
        gx2util::TimedCounter* commands = static_cast<gx2util::TimedCounter*>(allocateFromHeap(heap,(u32)commandCount * sizeof(gx2util::TimedCounter)));
        for (u32 index = 0; index < (u32)commandCount; ++index) {
            gx2util::TimedCounter* command = &commands[index];
            if (gabi::ea(command)) constructTimedCounter(command);
        }
        if (commands) { mCommands = commands; mCommandCapacity = commandCount; }
    }
}
/* WWHD glue
ext 0273B0D4 void* allocateArrayFromHeap(u32 bytes,sead::RegistryHeap* heap,s32 alignment)
ext 027DA694 agl::gx2util::TimedCounter* constructTimedCounter(agl::gx2util::TimedCounter* counter)
ext C0008898 void flushDataCache(void* data,u32 bytes)
*/

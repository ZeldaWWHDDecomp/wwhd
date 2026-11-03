// Render command pool destruction: slices go back to the heap that owns them, the recording allocation is
// deleted, every command record runs its own destructor and the record array is released.
#include "agl/RenderCommandPool.h"
using namespace agl::render;
static sead::DisposerVtable sRenderCommandPoolMethods;
void releaseArray(void* storage);
static void releaseToOwningHeap(void* storage) {
    sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager,storage);
    gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,storage);
}
/* 027D514C */
void agl::render::destroyRenderCommandPool(RenderCommandPool* pool,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!pool) return;
    pool->mVtable = &sRenderCommandPoolMethods;
    for (RenderCommandRing* ring = pool->mRings; ring != pool->mRings + 3; ++ring) {
        if (ring->mSlices) {
            sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager,ring->mSlices);
            gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,(RenderCommandSlice*)ring->mSlices);
            ring->mCapacity = 0;
            ring->mSlices = nullptr;
        }
    }
    if (pool->mAllocation) releaseArray(pool->mAllocation);
    if (pool->mCommands) {
        for (s32 index = 0; index < (s32)(u32)pool->mCommandCapacity; ++index) {
            gx2util::TimedCounter* command = &pool->mCommands.get()[index];
            sead::DisposerVtable* methods = static_cast<sead::DisposerVtable*>(static_cast<void*>(command->mVtable));
            gabi::call_ptr<void>(methods->mDestroy.mFunction,command,2);
        }
        releaseToOwningHeap(pool->mCommands);
        pool->mCommandCapacity = 0;
        pool->mCommands = nullptr;
    }
    if (flags & 1u) releaseRegistryStorage(pool);
}
/* WWHD glue
data sRenderCommandPoolMethods 1016C0B4
ext 0273AFC8 void releaseArray(void* storage)
*/

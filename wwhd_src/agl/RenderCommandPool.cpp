// Render command pool construction and per-frame reset.
#include "agl/RenderCommandPool.h"
using namespace agl::render;
static sead::DisposerVtable sRenderCommandPoolMethods;
void constructArray(void* objects,u32 count,u32 stride,u32 constructor);

/* 027D5064 */
agl::render::RenderCommandPool* agl::render::createRenderCommandPool(RenderCommandPool* pool) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!pool) pool = static_cast<RenderCommandPool*>(allocateRegistryStorage(sizeof(RenderCommandPool)));
    if (!pool) return pool;
    pool->mAllocation = nullptr;
    pool->mVtable = &sRenderCommandPoolMethods;
    be<u32>* next = &pool->mNextCommand;
    if (!gabi::ea(next)) next = static_cast<be<u32>*>(allocateRegistryStorage(sizeof(be<u32>)));
    if (next) *next = 0;
    void* rings = pool->mRings;
    if (!gabi::ea(rings)) rings = allocateRegistryStorage(sizeof(pool->mRings));
    if (rings) constructArray(rings,3,sizeof(RenderCommandRing),gabi::guest_fn((const void*)createRenderCommandRing));
    be<u32>* commands = &pool->mCommandCapacity;
    if (!gabi::ea(commands)) commands = static_cast<be<u32>*>(allocateRegistryStorage(8));
    if (commands) {
        commands[1] = 0;
        commands[0] = 0;
    }
    pool->mFrameCount = 0;
    pool->mCurrentFrame = 0;
    be<u8>* flags = &pool->mFlags;
    if (!gabi::ea(flags)) flags = static_cast<be<u8>*>(allocateRegistryStorage(1));
    if (flags) *flags = 0;
    return pool;
}
/* 027D54A4 */
void agl::render::RenderCommandPool::advanceFrame(u32 advance) {
    for (RenderCommandRing* ring = mRings; ring != mRings + 3; ++ring) ring->mOffset = 0;
    mNextCommand = 0;
    if (!advance) return;
    s32 next = (u8)mCurrentFrame + 1;
    s32 count = (u8)mFrameCount;
    mCurrentFrame = next - ppc_divw(next,count) * count;
}
/* 027D56A4 */
agl::render::RenderCommandRing* agl::render::createRenderCommandRing(RenderCommandRing* ring) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!ring) ring = static_cast<RenderCommandRing*>(allocateRegistryStorage(sizeof(RenderCommandRing)));
    if (ring) {
        ring->mSlices = nullptr;
        ring->mCapacity = 0;
    }
    return ring;
}
/* WWHD glue
data sRenderCommandPoolMethods 1016C0B4
ext 028EFFD0 void constructArray(void* objects,u32 count,u32 stride,u32 constructor)
*/

// Pending layers of a render view: cleared per frame (with the deferred command count, reset atomically), appended by
// the queue manager; a view with flag bit 1 shuffles them with a random comparator.
#include "agl/DisplayFrameState.h"
#include "sead/Random.h"
using namespace agl::render;
static sead::GlobalRandom* sGlobalRandom;
/* 027D646C */
void agl::render::DisplayFrameState::clearPending() {
    mPending.mCount = 0;
    u32 counter = gabi::ea(&mDeferred.mCount);
    do { gabi::load_reserved(counter); } while (!gabi::store_conditional(counter,0));
    mCurrentJob = -1;
}
/* 027D6498 */
s32 agl::render::randomizeRenderOrder() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 random = sGlobalRandom->getU32();
    return (s32)((random & 2u) - 1u);
}
/* 027D64C8 */
void agl::render::DisplayFrameState::appendPending(agl::RenderLayerPrefix* layer) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 count = mPending.mCount;
    if (count < (s32)mPending.mCapacity) {
        mPending.entries()[count] = gabi::ea(layer);
        mPending.mCount = (u32)(s32)mPending.mCount + 1u;
    }
    if ((u32)mFlags & 2u) mPending.heapSort(gabi::guest_fn((const void*)randomizeRenderOrder));
}
/* WWHD glue
data sGlobalRandom 101F8B60
ext 0275CEE4 u32 sead::Random::getU32()
ext 0273B7B4 void sead::PointerBuffer::heapSort(u32)
*/

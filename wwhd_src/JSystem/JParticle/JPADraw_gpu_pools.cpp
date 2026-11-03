// Cafe FIFO recycling around the JPA1 draw buffers.
// HD: each size class owns an independent bounded index queue.
#include "JSystem/JParticle/JPADraw_gpu_pools.h"
using namespace JPADrawGpu;
/* 0282FA58 */
JPAFieldPool* JPAManagerGpuPools_l::acquireFieldPool(be<u32>& index) {
    if (s32(mFieldQueue.mCount) > 0) {
        s32 cursor = mFieldQueue.mCursor;
        s32 capacity = mFieldQueue.mCapacity;
        if (cursor >= capacity) cursor = s32(u32(cursor) - u32(capacity));
        index = u32(mFieldQueue.mIndices.get()[u32(cursor)]);
        s32 next = s32(u32(mFieldQueue.mCursor) + 1);
        capacity = mFieldQueue.mCapacity;
        mFieldQueue.mCursor = next;
        s32 count = mFieldQueue.mCount;
        if (next >= capacity) mFieldQueue.mCursor = 0;
        mFieldQueue.mCount = s32(u32(count) - 1);
    }
    u32 selected = index;
    return selected < 100 ? &mFieldPools[selected] : &mFieldPools[0];
}
/* 0282FAD4 */
void JPAManagerGpuPools_l::releaseFieldPool(u32 index) {
    s32 count = mFieldQueue.mCount;
    s32 capacity = mFieldQueue.mCapacity;
    if (count >= capacity) return;
    s32 cursor = mFieldQueue.mCursor;
    capacity = mFieldQueue.mCapacity;
    s32 next = s32(u32(cursor) + u32(count));
    count = s32(u32(count) + 1);
    mFieldQueue.mCount = count;
    if (next >= capacity) next = s32(u32(next) - u32(capacity));
    mFieldQueue.mIndices.get()[u32(next)] = index;
}
/* 0282FB1C */
JPAParticlePool* JPAManagerGpuPools_l::acquireParticlePool(be<u32>& index) {
    if (s32(mParticleQueue.mCount) > 0) {
        s32 cursor = mParticleQueue.mCursor;
        s32 capacity = mParticleQueue.mCapacity;
        if (cursor >= capacity) cursor = s32(u32(cursor) - u32(capacity));
        index = u32(mParticleQueue.mIndices.get()[u32(cursor)]);
        s32 next = s32(u32(mParticleQueue.mCursor) + 1);
        capacity = mParticleQueue.mCapacity;
        mParticleQueue.mCursor = next;
        s32 count = mParticleQueue.mCount;
        if (next >= capacity) mParticleQueue.mCursor = 0;
        mParticleQueue.mCount = s32(u32(count) - 1);
    }
    u32 selected = index;
    return selected < 1500 ? &mParticlePools[selected] : &mParticlePools[0];
}
/* 0282FBA8 */
void JPAManagerGpuPools_l::releaseParticlePool(u32 index) {
    s32 count = mParticleQueue.mCount;
    s32 capacity = mParticleQueue.mCapacity;
    if (count >= capacity) return;
    s32 cursor = mParticleQueue.mCursor;
    capacity = mParticleQueue.mCapacity;
    s32 next = s32(u32(cursor) + u32(count));
    count = s32(u32(count) + 1);
    mParticleQueue.mCount = count;
    if (next >= capacity) next = s32(u32(next) - u32(capacity));
    mParticleQueue.mIndices.get()[u32(next)] = index;
}
/* 0282FBF0 */
JPAMediumPool* JPAManagerGpuPools_l::acquireMediumPool(be<u32>& index) {
    if (s32(mMediumQueue.mCount) > 0) {
        s32 cursor = mMediumQueue.mCursor;
        s32 capacity = mMediumQueue.mCapacity;
        if (cursor >= capacity) cursor = s32(u32(cursor) - u32(capacity));
        index = u32(mMediumQueue.mIndices.get()[u32(cursor)]);
        s32 next = s32(u32(mMediumQueue.mCursor) + 1);
        capacity = mMediumQueue.mCapacity;
        mMediumQueue.mCursor = next;
        s32 count = mMediumQueue.mCount;
        if (next >= capacity) mMediumQueue.mCursor = 0;
        mMediumQueue.mCount = s32(u32(count) - 1);
    }
    u32 selected = index;
    return selected < 20 ? &mMediumPools[selected] : &mMediumPools[0];
}
/* 0282FC7C */
void JPAManagerGpuPools_l::releaseMediumPool(u32 index) {
    s32 count = mMediumQueue.mCount;
    s32 capacity = mMediumQueue.mCapacity;
    if (count >= capacity) return;
    s32 cursor = mMediumQueue.mCursor;
    capacity = mMediumQueue.mCapacity;
    s32 next = s32(u32(cursor) + u32(count));
    count = s32(u32(count) + 1);
    mMediumQueue.mCount = count;
    if (next >= capacity) next = s32(u32(next) - u32(capacity));
    mMediumQueue.mIndices.get()[u32(next)] = index;
}
/* 0282FCC4 */
JPALargePool* JPAManagerGpuPools_l::acquireLargePool(be<u32>& index) {
    if (s32(mLargeQueue.mCount) > 0) {
        s32 cursor = mLargeQueue.mCursor;
        s32 capacity = mLargeQueue.mCapacity;
        if (cursor >= capacity) cursor = s32(u32(cursor) - u32(capacity));
        index = u32(mLargeQueue.mIndices.get()[u32(cursor)]);
        s32 next = s32(u32(mLargeQueue.mCursor) + 1);
        capacity = mLargeQueue.mCapacity;
        mLargeQueue.mCursor = next;
        s32 count = mLargeQueue.mCount;
        if (next >= capacity) mLargeQueue.mCursor = 0;
        mLargeQueue.mCount = s32(u32(count) - 1);
    }
    u32 selected = index;
    return selected < 20 ? &mLargePools[selected] : &mLargePools[0];
}
/* 0282FD58 */
void JPAManagerGpuPools_l::releaseLargePool(u32 index) {
    s32 count = mLargeQueue.mCount;
    s32 capacity = mLargeQueue.mCapacity;
    if (count >= capacity) return;
    s32 cursor = mLargeQueue.mCursor;
    capacity = mLargeQueue.mCapacity;
    s32 next = s32(u32(cursor) + u32(count));
    count = s32(u32(count) + 1);
    mLargeQueue.mCount = count;
    if (next >= capacity) next = s32(u32(next) - u32(capacity));
    mLargeQueue.mIndices.get()[u32(next)] = index;
}

/* WWHD glue
*/

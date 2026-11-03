// HD: Cafe JPA GPU buffer-pool construction; these helpers have no GC body.
#include "JSystem/JParticle/JPA_gpu_pool.h"
void* JPAGpuAllocate(u32);
void JPAGpuDelete(void*);
void JPAGpuVertexConstruct(void*);
void JPAGpuShapeConstruct(void*);
void JPAGpuConstructArray(void*, u32, u32, u32);

static JPAGpuBuffer* constructBuffer(JPAGpuBuffer* buffer) {
    if (!buffer) {
        buffer = static_cast<JPAGpuBuffer*>(JPAGpuAllocate(sizeof(JPAGpuBuffer)));
        if (!buffer) return nullptr;
    }
    JPAGpuVertexConstruct(&buffer->mVertexBuffer);
    JPAGpuShapeConstruct(buffer->mShape);
    buffer->mCount = 0;
    buffer->mData = 0;
    return buffer;
}

/* 0282237C */
JPAGpuBuffer* JPAGpuFieldBufferConstruct(JPAGpuBuffer* buffer) { return constructBuffer(buffer); }
/* 02822478 */
JPAGpuBuffer* JPAGpuParticleBufferConstruct(JPAGpuBuffer* buffer) { return constructBuffer(buffer); }
/* 0282255C */
JPAGpuBuffer* JPAGpuEmitterBufferConstruct(JPAGpuBuffer* buffer) { return constructBuffer(buffer); }
/* 02822658 */
JPAGpuBuffer* JPAGpuDrawBufferConstruct(JPAGpuBuffer* buffer) { return constructBuffer(buffer); }
/* 028269E8 */
JPAGpuBuffer* JPAGpuResourceBufferConstruct(JPAGpuBuffer* buffer) { return constructBuffer(buffer); }

template <u32 N> static JPAGpuPool<N>* constructPool(JPAGpuPool<N>* pool, u32 elementSize, u32 constructor) {
    if (!pool) {
        pool = static_cast<JPAGpuPool<N>*>(JPAGpuAllocate(sizeof(JPAGpuPool<N>)));
        if (!pool) return nullptr;
    }
    JPAGpuConstructArray(pool, N, sizeof(JPAGpuBuffer), constructor);
    pool->mState.mSwapIndex = 0;
    pool->mState.mElementSize = elementSize;
    pool->mState.mInitialized = 0;
    pool->mState.mCursor = 0;
    pool->mState.mShapeEnabled = 0;
    // HD: GHS clears the even buffer slots before the odd ones.
    for (u32 phase = 0; phase < 2; ++phase)
        for (u32 i = phase; i < N; i += 2) pool->mBuffers[i].mInUse = 0;
    return pool;
}

/* 028223D8 */
JPAGpuPool<4>* JPAGpuFieldPoolConstruct(JPAGpuPool<4>* pool) {
    return constructPool(pool, 20, gabi::guest_fn(reinterpret_cast<const void*>(JPAGpuFieldBufferConstruct)));
}
/* 028224D4 */
JPAGpuPool<2>* JPAGpuParticlePoolConstruct(JPAGpuPool<2>* pool) {
    return constructPool(pool, 48, gabi::guest_fn(reinterpret_cast<const void*>(JPAGpuParticleBufferConstruct)));
}
/* 028225B8 */
JPAGpuPool<10>* JPAGpuEmitterPoolConstruct(JPAGpuPool<10>* pool) {
    return constructPool(pool, 20, gabi::guest_fn(reinterpret_cast<const void*>(JPAGpuEmitterBufferConstruct)));
}
/* 028226B4 */
JPAGpuPool<300>* JPAGpuDrawPoolConstruct(JPAGpuPool<300>* pool) {
    return constructPool(pool, 20, gabi::guest_fn(reinterpret_cast<const void*>(JPAGpuDrawBufferConstruct)));
}
/* 02822368 */
void JPAFieldSafeStringDestroy(void* pool, u32 deleting) {
    if (pool && (deleting & 1)) JPAGpuDelete(pool);
}
/* 028269D4 */
void JPAResourceSafeStringDestroy(void* pool, u32 deleting) {
    if (pool && (deleting & 1)) JPAGpuDelete(pool);
}
/* 02826A44 */
void* JPAResourceRecordConstruct(void* state) {
    if (!state) state = JPAGpuAllocate(0x10);
    return state;
}
/* 02826A70 */
void JPAResourceRecordDestroy(void*) {}

/* WWHD glue
ext 0273AD10 void* JPAGpuAllocate(u32)
ext 0273AF40 void JPAGpuDelete(void*)
ext 027B5BD8 void JPAGpuVertexConstruct(void*)
ext 027BF734 void JPAGpuShapeConstruct(void*)
ext 028EFFD0 void JPAGpuConstructArray(void*,u32,u32,u32)
*/

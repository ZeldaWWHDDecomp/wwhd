// J3D HD renderer: the array of per-frame uniform pairs (one per view) and its lifetime.
// Names are our own; written from the WWHD binary.
#include "JSystem/J3DGraph/RenderFrameArray.h"

static j3d::RenderUniformPairVtable_l sRenderFrameUniformTable;

/* 027FD6F4 */
j3d::RenderFrameArray* j3d::RenderFrameArray::construct(RenderFrameArray* storage) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage == NULL) storage = static_cast<RenderFrameArray*>(allocateRenderUniformObject(sizeof(RenderFrameArray)));
    if (storage == NULL) return NULL;
    RenderStorageSpan* span = storage;
    if (span == NULL) span = static_cast<RenderStorageSpan*>(allocateRenderUniformObject(sizeof(RenderStorageSpan)));
    if (span != NULL) {
        span->mStorage = NULL;
        span->mCount = 0;
    }
    storage->mArrayReady = 0;
    return storage;
}

/* 027FD764 */
void j3d::RenderFrameArray::destroy(RenderFrameArray* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage == NULL) return;
    if (storage->mStorage != NULL) {
        s32 count = storage->mCount;
        for (s32 index = 0; index < count; ++index) {
            RenderFrameUniforms* object = &storage->frames()[index];
            gabi::call_ptr<void>(object->mDisposer.mVtable->mDestroy.mFunction, object, 2u);
            count = storage->mCount;
        }
        sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager.get(), storage->mStorage);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, storage->mStorage.get());
        storage->mCount = 0;
        storage->mStorage = NULL;
    }
    if (flags & 1) releaseRenderUniformObject(storage);
}

/* 027FD838 */
void j3d::RenderFrameArray::prepare(s32 count, sead::RegistryHeap* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (mStorage != NULL) {
        s32 oldCount = mCount;
        for (s32 index = 0; index < oldCount; ++index) {
            RenderFrameUniforms* object = &frames()[index];
            gabi::call_ptr<void>(object->mDisposer.mVtable->mDestroy.mFunction, object, 2u);
            oldCount = mCount;
        }
        sead::RegistryHeap* oldHeap = findRegistryHeap(sRegistryManager.get(), mStorage);
        gabi::call_ptr<void>(oldHeap->dispatch()->mRelease.mFunction, oldHeap, mStorage.get());
        mCount = 0;
        mStorage = NULL;
    }
    if (count > 0) {
        sead::RegistryHeap* allocationHeap = heap;
        if (allocationHeap == NULL) allocationHeap = registryCurrentHeap(sRegistryManager.get());
        u32 size = (u32)count * (u32)sizeof(RenderFrameUniforms);
        auto* allocated = static_cast<RenderFrameUniforms*>(gabi::call_ptr<void*>(
            allocationHeap->dispatch()->mAllocate.mFunction, allocationHeap, size, 4));
        for (s32 index = 0; index < count; ++index) {
            RenderFrameUniforms* object = guestElement(allocated, index);
            if (object != NULL) {
                RenderUniformPair::construct(object);
                object->mDisposer.mVtable = reinterpret_cast<sead::DisposerVtable*>(&sRenderFrameUniformTable);
                Mtx34* primary = &object->mPrimaryMatrix;
                prepareRenderFrameStorage(primary, 0x1C8);
                // Matrix members have no constructor; GHS still allocates for a NULL member address.
                if (memberWrapsToNull(primary, 0)) allocateRenderUniformObject(sizeof(Mtx34));
                if (memberWrapsToNull(primary, sizeof(Mtx34))) allocateRenderUniformObject(sizeof(Mtx44));
                constructRenderFloat3Array(object->mVectors, 8, sizeof(RenderFloat3), initializeRenderFloat3);
                constructRenderFloat4Array(object->mWideValues, 8, sizeof(RenderFloat4), initializeRenderFloat4);
                constructRenderFloat4Array(object->mExtraValues, 2, sizeof(RenderFloat4), initializeRenderFloat4);
                constructRenderFloat4Array(&object->mReferenceValue, 1, sizeof(RenderFloat4), initializeRenderFloat4);
                if (memberWrapsToNull(object, offsetof(RenderFrameUniforms, mFinalMatrix))) allocateRenderUniformObject(sizeof(Mtx44));
            }
        }
        if (allocated != NULL) {
            mCount = count;
            mStorage = allocated;
        }
    }
    RenderFrameUniforms* current = frames();
    RenderFrameUniforms* end = current + (s32)mCount;
    while (current != end) {
        current->RenderUniformPair::prepare(heap);
        ++current;
    }
    mArrayReady = 1;
}

/* 027FDFF4 */
void j3d::RenderFrameArray::advance(u32 index) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RenderFrameUniforms* object = frames();
    if (index < (u32)mCount) object += index;
    object->RenderUniformPair::advance();
}

/* 027FE010 */
void j3d::RenderFrameArray::advanceAll() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    s32 count = mCount;
    for (s32 index = 0; index < count; ++index) {
        RenderFrameUniforms* object = frames();
        if ((u32)index < (u32)count) object += index;
        object->RenderUniformPair::advance();
        count = mCount;
    }
}

/* WWHD glue
 data sRenderFrameUniformTable 1016EF54
 ext 02755FEC sead::RegistryHeap* findRegistryHeap(void*,void*)
 ext 02756140 sead::RegistryHeap* registryCurrentHeap(void*)
 ext 028F521C void prepareRenderFrameStorage(void*,u32)
 ext 028EFFD0 void constructRenderFloat3Array(void*,u32,u32,RenderFloat3Construct)
 ext 028EFFD0 void constructRenderFloat4Array(void*,u32,u32,RenderFloat4Construct)
 ext 027FF64C void initializeRenderFloat3(j3d::RenderFloat3*) callback
 ext 027FF67C void initializeRenderFloat4(j3d::RenderFloat4*) callback
*/

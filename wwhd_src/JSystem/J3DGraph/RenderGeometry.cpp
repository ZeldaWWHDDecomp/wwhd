// J3D HD renderer: the double-buffered geometry pool (vertex buffers + attribute states per entry).
// Names are our own; written from the WWHD binary.
#include "JSystem/J3DGraph/RenderGeometry.h"

struct RenderAttributeFormats_l { be<u32> mWords[28]; };
static RenderAttributeFormats_l sAttributeFetchFormats;


static inline j3d::RenderGeometryEntry_l* selectGeometry(j3d::RenderStorageSpan& span, u32 index) {
    auto* entry = static_cast<j3d::RenderGeometryEntry_l*>(span.mStorage.get());
    if (index < (u32)span.mCount) entry += index;
    return entry;
}

static inline void clearGeometryStorage(j3d::RenderGeometryPool* pool) {
    for (u32 index = 0; index < (u32)pool->mGeometryCount; ++index) {
        for (u32 set = 0; set < 2; ++set) {
            resetRenderAttributes(&selectGeometry(pool->mSets[set], index)->mAttributes);
            auto* entry = selectGeometry(pool->mSets[set], index);
            if (entry->mVertices.mStorage != NULL) {
                sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager.get(), entry->mVertices.mStorage);
                gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, entry->mVertices.mStorage.get());
                entry->mVertices.mCount = 0;
                entry->mVertices.mStorage = NULL;
            }
        }
    }
    for (u32 set = 0; set < 2; ++set) {
        auto& span = pool->mSets[set];
        if (span.mStorage != NULL) {
            s32 count = span.mCount;
            for (s32 index = 0; index < count; ++index) {
                auto* entry = &static_cast<j3d::RenderGeometryEntry_l*>(span.mStorage.get())[index];
                if (entry != NULL) {
                    destroyRenderAttributes(&entry->mAttributes, 2);
                    destroyRenderVertexDescriptor(&entry->mVertexDescriptor, 2);
                    count = span.mCount;
                }
            }
            sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager.get(), span.mStorage);
            gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, span.mStorage.get());
            span.mCount = 0;
            span.mStorage = NULL;
        }
    }
}

/* 027FE64C */
j3d::RenderGeometryPool* j3d::RenderGeometryPool::construct(RenderGeometryPool* storage) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage == NULL) storage = static_cast<RenderGeometryPool*>(allocateRenderGeometryObject(sizeof(RenderGeometryPool)));
    if (storage == NULL) return NULL;
    constructRenderGeometrySpans(storage, 2, sizeof(RenderStorageSpan), initializeRenderGeometrySpan);
    storage->mVertexCount = 0;
    storage->mGeometryCount = 0;
    storage->mActiveSet = 0;
    storage->mDirty = 0;
    storage->mPrepared = 0;
    return storage;
}

/* 027FEE5C */
void j3d::RenderGeometryEntries::configureVertex(agl::VertexBufferLayout_l* state, void* data, u32 count) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    setRenderVertexStorage(state, data, sizeof(RenderVertex_l), count * (u32)sizeof(RenderVertex_l));
    const u32 formats[14] = {0x811,0x811,0x813,0x312,0x80D,0x80D,0x80D,0x80D,0x80D,0x80D,0x80D,0x80D,0x813,0x813};
    const u32 offsets[14] = {0,0xC,0x18,0x28,0x38,0x40,0x48,0x50,0x58,0x60,0x68,0x70,0x78,0x88};
    for (u32 slot = 0; slot < 14; ++slot) setRenderVertexAttribute(state, slot, formats[slot], offsets[slot], 0);
}

/* 027FEFE0 */
void j3d::RenderGeometryEntries::configureAttributes(const RenderAttributeLocations_l* locations,
    RenderAttributes_l* state, agl::VertexBufferLayout_l* vertex, sead::RegistryHeap* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    resetRenderAttributes(state);
    initializeRenderAttributes(state, 1, heap);
    for (u32 slot = 0; slot < 14; ++slot) {
        const RenderAttributeLocation_l* record = guestNull<RenderAttributeLocation_l>();
        if (slot < (u32)locations->mCount) record = &locations->mRecords.get()[slot];
        addRenderAttribute(state, record->mLocation, vertex, slot);
    }
    finalizeRenderAttributes(state);
}

/* 027FF478 */
void j3d::RenderGeometryEntries::configureCustomVertex(agl::VertexBufferLayout_l* state, void* data,
    u32 count, const RenderVertexLayout_l* layout) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (layout->mMask == 0) {
        configureVertex(state, data, count);
        return;
    }
    u32 stride = layout->mStride;
    setRenderVertexStorage(state, data, stride, count * stride);
    u32 packed = 0;
    for (u32 slot = 0; slot < 14; ++slot) {
        if ((u32)layout->mMask & (1u << slot)) {
            u32 offset = layout->mOffsets.get()[packed];
            ++packed;
            u32 format = sAttributeFetchFormats.mWords[slot * 2];
            setRenderVertexAttribute(state, slot, format, offset, 0);
        }
    }
}

/* 027FF530 */
void j3d::RenderGeometryEntries::configureCustomAttributes(const RenderAttributeLocations_l* locations,
    RenderAttributes_l* state, agl::VertexBufferLayout_l* vertex, const RenderVertexLayout_l* layout, sead::RegistryHeap* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (layout->mMask == 0) {
        configureAttributes(locations, state, vertex, heap);
        return;
    }
    resetRenderAttributes(state);
    initializeRenderAttributes(state, 1, heap);
    for (u32 slot = 0; slot < 14; ++slot) {
        if ((u32)layout->mMask & (1u << slot)) {
            const RenderAttributeLocation_l* record = guestNull<RenderAttributeLocation_l>();
            if (slot < (u32)locations->mCount) record = &locations->mRecords.get()[slot];
            addRenderAttribute(state, record->mLocation, vertex, slot);
        }
    }
    finalizeRenderAttributes(state);
}

/* 027FEDB0 */
void j3d::RenderGeometryPool::draw(const agl::IndexBufferLayout_l* indices, u32 repetitions, u32 indexCount) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RenderStorageSpan* opposite = &mSets[mActiveSet == 0];
    u32 index = 0;
    while (repetitions != 0) {
        if (mDirty != 0) {
            auto* object = static_cast<RenderGeometryEntry_l*>(opposite->mStorage.get());
            if (index < (u32)opposite->mCount) object += index;
            applyRenderAttributes(&object->mAttributes);
        }
        if (indexCount != 0) GX2DrawIndexedEx(indices->mPrimitiveMode, indexCount, indices->mFormat, indices->mStorage, 0, 1);
        ++index;
        --repetitions;
    }
}

/* 027FF1D8 */
void j3d::RenderGeometryPool::flush(void*, u32 count) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 active = mActiveSet;
    RenderStorageSpan* span = &mSets[active];
    for (u32 index = 0; index < count; ++index) {
        auto* object = static_cast<RenderGeometryEntry_l*>(span->mStorage.get());
        if (index < (u32)span->mCount) object += index;
        flushRenderVertex(&object->mVertexDescriptor, 0, object->mVertexDescriptor.mStorageSize);
        active = mActiveSet;
    }
    mActiveSet = active == 0;
}

/* 027FF260 */
void j3d::RenderGeometryPool::copyCurrent() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    // Copies every vertex of the active set into the other set. The counts are reloaded only
    // after a vertex copy (the copies could reach them), as in the original.
    RenderStorageSpan* opposite = &mSets[mActiveSet == 0];
    u32 geometryCount = mGeometryCount;
    u32 vertexCount = mVertexCount;
    for (u32 geometry = 0; geometry < geometryCount; ++geometry) {
        RenderStorageSpan* active = &mSets[(u32)mActiveSet];
        RenderGeometryEntry_l* source = guestNull<RenderGeometryEntry_l>();
        RenderGeometryEntry_l* destination = guestNull<RenderGeometryEntry_l>();
        if (geometry < (u32)active->mCount) source = &static_cast<RenderGeometryEntry_l*>(active->mStorage.get())[geometry];
        if (geometry < (u32)opposite->mCount) destination = &static_cast<RenderGeometryEntry_l*>(opposite->mStorage.get())[geometry];
        if (vertexCount == 0) continue;
        for (u32 vertex = 0; vertex < vertexCount; ++vertex) {
            auto* to = static_cast<RenderVertex_l*>(destination->mVertices.mStorage.get());
            if (vertex < (u32)destination->mVertices.mCount) to += vertex;
            auto* from = static_cast<RenderVertex_l*>(source->mVertices.mStorage.get());
            if (vertex < (u32)source->mVertices.mCount) from += vertex;
            for (u32 word = 0; word < 10; ++word) to->mLeadingValues[word] = from->mLeadingValues[word];
            for (u32 word = 0; word < 4; ++word) to->mPackedValues[word] = from->mPackedValues[word];
            for (u32 word = 0; word < 24; ++word) to->mTrailingValues[word] = from->mTrailingValues[word];
            vertexCount = mVertexCount;
        }
        geometryCount = mGeometryCount;
    }
}

/* 027FE6BC */
void j3d::RenderGeometryPool::prepare(s32 geometryCount, s32 vertexCount, sead::RegistryHeap* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    clearGeometryStorage(this);
    mGeometryCount = 0;
    mDirty = 0;
    mVertexCount = 0;
    for (u32 set = 0; set < 2; ++set) {
        auto& span = mSets[set];
        if (geometryCount > 0) {
            auto* allocationHeap = heap;
            if (allocationHeap == NULL) allocationHeap = registryCurrentHeap(sRegistryManager.get());
            auto* allocated = static_cast<RenderGeometryEntry_l*>(gabi::call_ptr<void*>(
                allocationHeap->dispatch()->mAllocate.mFunction, allocationHeap,
                (u32)geometryCount * (u32)sizeof(RenderGeometryEntry_l), 4));
            for (s32 index = 0; index < geometryCount; ++index) {
                auto* entry = guestElement(allocated, index);
                if (entry != NULL) {
                    entry->mVertices.mCount = 0;
                    entry->mVertices.mStorage = NULL;
                    constructRenderVertexDescriptor(&entry->mVertexDescriptor);
                    constructRenderAttributes(&entry->mAttributes);
                }
            }
            if (allocated != NULL) {
                span.mStorage = allocated;
                span.mCount = geometryCount;
            }
        }
        // The original uses a decrement-to-zero count, without clamping negatives.
        u32 remaining = (u32)geometryCount;
        u32 index = 0;
        while (remaining != 0) {
            RenderGeometryEntry_l* entry = guestNull<RenderGeometryEntry_l>();
            if (index < (u32)span.mCount) entry = &static_cast<RenderGeometryEntry_l*>(span.mStorage.get())[index];
            if (vertexCount > 0) {
                auto* allocationHeap = heap;
                if (allocationHeap == NULL) allocationHeap = registryCurrentHeap(sRegistryManager.get());
                void* allocated = gabi::call_ptr<void*>(allocationHeap->dispatch()->mAllocate.mFunction,
                    allocationHeap, (u32)vertexCount * (u32)sizeof(RenderVertex_l), 4);
                if (allocated != NULL) {
                    entry->mVertices.mStorage = allocated;
                    entry->mVertices.mCount = vertexCount;
                }
            }
            --remaining;
            ++index;
        }
    }
    mGeometryCount = geometryCount;
    mVertexCount = vertexCount;
    mPrepared = 1;
}

/* 027FEB60 */
void j3d::RenderGeometryPool::destroy(RenderGeometryPool* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage == NULL) return;
    clearGeometryStorage(storage);
    storage->mVertexCount = 0;
    storage->mDirty = 0;
    storage->mGeometryCount = 0;
    if (flags & 1) releaseRenderGeometryObject(storage);
}

/* 027FF078 */
void j3d::RenderGeometryPool::configure(const RenderAttributeLocations_l* locations, u32 count, sead::RegistryHeap* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    for (u32 set = 0; set < 2; ++set) {
        u32 runningIndex = 0;
        u32 byteOffset = 0;
        for (u32 remaining = count; remaining != 0; --remaining) {
            auto& span = mSets[set];
            auto* entry = static_cast<RenderGeometryEntry_l*>(span.mStorage.get());
            if (runningIndex < (u32)span.mCount) entry = reinterpret_cast<RenderGeometryEntry_l*>(reinterpret_cast<u8*>(entry) + byteOffset);
            auto* childEntry = static_cast<RenderGeometryEntry_l*>(span.mStorage.get());
            if (runningIndex < (u32)span.mCount) childEntry = reinterpret_cast<RenderGeometryEntry_l*>(reinterpret_cast<u8*>(childEntry) + byteOffset);
            RenderGeometryEntries::configureVertex(&entry->mVertexDescriptor, childEntry->mVertices.mStorage, mVertexCount);
            if (locations != NULL) {
                auto* attributes = static_cast<RenderGeometryEntry_l*>(span.mStorage.get());
                if (runningIndex < (u32)span.mCount) attributes = reinterpret_cast<RenderGeometryEntry_l*>(reinterpret_cast<u8*>(attributes) + byteOffset);
                auto* vertex = static_cast<RenderGeometryEntry_l*>(span.mStorage.get());
                if (runningIndex < (u32)span.mCount) vertex = reinterpret_cast<RenderGeometryEntry_l*>(reinterpret_cast<u8*>(vertex) + byteOffset);
                RenderGeometryEntries::configureAttributes(locations, &attributes->mAttributes, &vertex->mVertexDescriptor, heap);
                mDirty = 1;
            }
            ++runningIndex;
            byteOffset += sizeof(RenderGeometryEntry_l);
        }
    }
}

/* WWHD glue
 ext 0273AF40 void releaseRenderGeometryObject(void*)
 ext 027B5BD8 void constructRenderVertexDescriptor(agl::VertexBufferLayout_l*)
 ext 027BF734 void constructRenderAttributes(j3d::RenderAttributes_l*)
 ext 027B5CBC void destroyRenderVertexDescriptor(agl::VertexBufferLayout_l*,u32)
 ext 027BF880 void destroyRenderAttributes(j3d::RenderAttributes_l*,u32)
 ext 028EFFD0 void constructRenderGeometrySpans(void*,u32,u32,RenderSpanConstruct)
 ext 027FFC7C void initializeRenderGeometrySpan(j3d::RenderStorageSpan*) callback
 data sAttributeFetchFormats 1016EE78
 ext 0273AD10 void* allocateRenderGeometryObject(u32)
 ext 027B5CFC void setRenderVertexStorage(agl::VertexBufferLayout_l*,void*,u32,u32)
 ext 027B5D60 void setRenderVertexAttribute(agl::VertexBufferLayout_l*,u32,u32,u32,u32)
 ext 027BF7E8 void resetRenderAttributes(j3d::RenderAttributes_l*)
 ext 027BF960 void initializeRenderAttributes(j3d::RenderAttributes_l*,u32,sead::RegistryHeap*)
 ext 027BFB60 void addRenderAttribute(j3d::RenderAttributes_l*,s32,agl::VertexBufferLayout_l*,u32)
 ext 027BFC74 void finalizeRenderAttributes(j3d::RenderAttributes_l*)
 ext 027BFE5C void applyRenderAttributes(j3d::RenderAttributes_l*)
 ext 027B5E94 void flushRenderVertex(agl::VertexBufferLayout_l*,u32,u32)
 ext C0006178 void GX2DrawIndexedEx(u32,u32,u32,const void*,u32,u32)
*/

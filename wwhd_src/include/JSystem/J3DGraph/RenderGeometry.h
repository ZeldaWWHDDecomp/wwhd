// J3D HD renderer: geometry pool layouts and local views of the agl vertex/index/attribute objects.
// Names are our own; written from the WWHD binary.
#pragma once
#include "JSystem/J3DGraph/RenderValueStorage.h"
#include "sead/HeapRegistry.h"

// GX2 enum values used here (Cafe SDK facts).
enum {
    GX2_INDEX_TYPE_U32 = 9,
    GX2_PRIMITIVE_MODE_TRIANGLE_STRIP = 6,
};

// Local views of agl objects this code builds and passes on, named as in their owner unit (AGL#14:
// agl/VertexBufferLayout.h, agl/IndexBufferLayout.h); the merge replaces them with those headers.
namespace agl {
struct BufferAttribute_l {
    be<u32> mFormat;
    be<u32> mOffset;
    be<u8> mEnabled;
    be<u8> mBufferIndex;
    u8 mPadding[2];
    be<u32> mFetchFormat;
    be<u32> mIndexMode;
};
struct VertexBufferLayout_l {
    BufferAttribute_l mAttributes[16];
    gptr<void> mStorage;
    be<u32> mStride;
    be<u32> mCount;
    be<u32> mStorageSize;
    gptr<void> mVtable;
};
struct IndexBufferLayout_l {
    be<u32> mFormat;
    be<u32> mPrimitiveMode;
    gptr<void> mStorage;
    be<u32> mIndexCount;
    be<u32> mIndexBytes;
    gptr<void> mVtable;
};
}

namespace j3d {
struct RenderVertex_l {
    be<f32> mLeadingValues[10];
    be<u32> mPackedValues[4];
    be<f32> mTrailingValues[24];
    static RenderVertex_l* construct(RenderVertex_l* storage);
};
struct RenderAttributeVtable_l { u8 mPrefix[12]; be<u32> mDestroy; };
struct RenderAttributeBinding_l {
    gptr<agl::VertexBufferLayout_l> mDescriptor;
    be<s32> mSlot, mBufferIndex;
};
struct RenderAttributes_l {
    RenderAttributeBinding_l mEntries[16];
    be<u32> mBufferCount;
    gptr<gptr<agl::VertexBufferLayout_l>> mBuffers;
    be<u8> mFinalized, mInitialized;
    u8 mPadding[2];
    u8 mFetchShader[0x20];
    gptr<void> mProgram;
    gptr<RenderAttributeVtable_l> mVtable;
};
struct RenderGeometryEntry_l {
    RenderStorageSpan mVertices;
    agl::VertexBufferLayout_l mVertexDescriptor;
    RenderAttributes_l mAttributes;
};
struct RenderAttributeLocation_l { u8 mPrefix[12]; be<s16> mLocation; u8 mTail[2]; };
struct RenderAttributeLocations_l { be<u32> mPrefix; be<u32> mCount; gptr<RenderAttributeLocation_l> mRecords; };
struct RenderVertexLayout_l { be<u32> mMask, mStride; gptr<be<u32>> mOffsets; };
struct RenderGeometryPool {
    RenderStorageSpan mSets[2];
    be<u32> mGeometryCount, mVertexCount;
    be<u8> mPrepared, mDirty;
    u8 mPadding[2];
    be<u32> mActiveSet;
    static RenderGeometryPool* construct(RenderGeometryPool* storage);
    void prepare(s32 geometryCount, s32 vertexCount, sead::RegistryHeap* heap);
    static void destroy(RenderGeometryPool* storage, u32 flags);
    void draw(const agl::IndexBufferLayout_l* indices, u32 repetitions, u32 indexCount);
    void configure(const RenderAttributeLocations_l* locations, u32 count, sead::RegistryHeap* heap);
    void flush(void* unused, u32 count);
    void copyCurrent();
};
struct RenderGeometryEntries {
    static void configureVertex(agl::VertexBufferLayout_l* state, void* data, u32 count);
    static void configureAttributes(const RenderAttributeLocations_l* locations, RenderAttributes_l* state,
                                    agl::VertexBufferLayout_l* vertex, sead::RegistryHeap* heap);
    static void configureCustomVertex(agl::VertexBufferLayout_l* state, void* data, u32 count,
                                      const RenderVertexLayout_l* layout);
    static void configureCustomAttributes(const RenderAttributeLocations_l* locations, RenderAttributes_l* state,
                                          agl::VertexBufferLayout_l* vertex, const RenderVertexLayout_l* layout, sead::RegistryHeap* heap);
};
}
WWHD_SIZE(j3d::RenderVertex_l, 0x98);
WWHD_OFFSET(j3d::RenderVertex_l, mPackedValues, 0x28);
WWHD_OFFSET(j3d::RenderVertex_l, mTrailingValues, 0x38);
WWHD_SIZE(agl::VertexBufferLayout_l, 0x154);
WWHD_OFFSET(agl::VertexBufferLayout_l, mStorageSize, 0x14C);
WWHD_OFFSET(agl::VertexBufferLayout_l, mVtable, 0x150);
WWHD_SIZE(j3d::RenderAttributes_l, 0xF4);
WWHD_OFFSET(j3d::RenderAttributes_l, mVtable, 0xF0);
WWHD_SIZE(j3d::RenderGeometryEntry_l, 0x250);
WWHD_OFFSET(j3d::RenderGeometryEntry_l, mAttributes, 0x15C);
WWHD_OFFSET(j3d::RenderGeometryPool, mGeometryCount, 0x10);
WWHD_OFFSET(j3d::RenderGeometryPool, mActiveSet, 0x1C);
WWHD_SIZE(j3d::RenderGeometryPool, 0x20);
WWHD_SIZE(j3d::RenderAttributeLocation_l, 16);
WWHD_SIZE(j3d::RenderAttributeLocations_l, 12);
WWHD_SIZE(j3d::RenderVertexLayout_l, 12);
WWHD_SIZE(agl::IndexBufferLayout_l, 24);

void setRenderVertexStorage(agl::VertexBufferLayout_l*,void*,u32,u32);
void setRenderVertexAttribute(agl::VertexBufferLayout_l*,u32,u32,u32,u32);
void resetRenderAttributes(j3d::RenderAttributes_l*);
void initializeRenderAttributes(j3d::RenderAttributes_l*,u32,sead::RegistryHeap*);
void addRenderAttribute(j3d::RenderAttributes_l*,s32,agl::VertexBufferLayout_l*,u32);
void finalizeRenderAttributes(j3d::RenderAttributes_l*);
void applyRenderAttributes(j3d::RenderAttributes_l*);
void flushRenderVertex(agl::VertexBufferLayout_l*,u32,u32);
void GX2DrawIndexedEx(u32 mode, u32 count, u32 indexType, const void* indices, u32 baseVertex, u32 numInstances);
void* allocateRenderGeometryObject(u32);

using RenderSpanConstruct = void (*)(j3d::RenderStorageSpan*);
void constructRenderGeometrySpans(void*,u32,u32,RenderSpanConstruct);
void initializeRenderGeometrySpan(j3d::RenderStorageSpan*);
void constructRenderVertexDescriptor(agl::VertexBufferLayout_l*);
void constructRenderAttributes(j3d::RenderAttributes_l*);
void destroyRenderVertexDescriptor(agl::VertexBufferLayout_l*,u32);
void destroyRenderAttributes(j3d::RenderAttributes_l*,u32);
void releaseRenderGeometryObject(void*);

WWHD_SIZE(agl::BufferAttribute_l, 0x14);
WWHD_OFFSET(agl::VertexBufferLayout_l, mStorage, 0x140);
WWHD_OFFSET(agl::VertexBufferLayout_l, mStride, 0x144);
WWHD_SIZE(j3d::RenderAttributeBinding_l, 12);
WWHD_OFFSET(j3d::RenderAttributes_l, mBufferCount, 0xC0);
WWHD_OFFSET(j3d::RenderAttributes_l, mBuffers, 0xC4);
WWHD_OFFSET(j3d::RenderAttributes_l, mFetchShader, 0xCC);
WWHD_OFFSET(j3d::RenderAttributes_l, mProgram, 0xEC);
WWHD_OFFSET(agl::IndexBufferLayout_l, mPrimitiveMode, 4);
WWHD_OFFSET(agl::IndexBufferLayout_l, mIndexCount, 0xC);
WWHD_OFFSET(agl::IndexBufferLayout_l, mIndexBytes, 0x10);
WWHD_OFFSET(agl::IndexBufferLayout_l, mVtable, 0x14);

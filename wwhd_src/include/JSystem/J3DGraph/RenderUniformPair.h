// J3D HD renderer: uniform pair layouts and a local view of the agl uniform buffer.
// Names are our own; written from the WWHD binary.
#pragma once
#include "JSystem/J3DGraph/RenderValueStorage.h"

namespace agl {
// Local view of the agl uniform buffer (owned by the agl unit): field table, CPU storage, sizes.
struct RenderUniformBuffer_l {
    gptr<void> mFormat;
    gptr<void> mCpuStorage;
    gptr<void> mGpuStorage;
    be<u32> mStride;
    be<u32> mSize;
    be<u8> mFlags;
    be<u8> mCount;
    u8 mPadding[2];
    gptr<void> mMethods;
};
}
namespace j3d {
struct RenderCallbackQueue_l;
struct RenderUniformPairVtable_l {
    sead::VtableSlot mHeader;
    sead::VtableSlot mDestroy;
    sead::VtableSlot mConfigure;
    sead::VtableSlot mAfterSetup;
    sead::VtableSlot mUpload;
    sead::VtableSlot mBind;
};
struct RenderUniformPair {
    sead::Disposer mDisposer;
    agl::RenderUniformBuffer_l mBuffers[2];
    be<u32> mActiveIndex;
    be<u32> mPreviousIndex;
    be<u8> mReady;
    u8 mPadding[3];
    RenderCallbackNode_l mCallback;

    static RenderUniformPair* construct(RenderUniformPair* storage);
    static void destroy(RenderUniformPair* storage, u32 flags);
    void prepare(void* heap);
    void advance();
    void uploadPrevious();
    void configureFrameFields(agl::RenderUniformBuffer_l* buffer, void* heap);
    void configureMatrixFields(agl::RenderUniformBuffer_l* buffer, void* heap);
};
struct RenderWideWords {
    be<u32> mWords[4];
};
struct RenderFrameUniforms : RenderUniformPair {
    Mtx34 mPrimaryMatrix;
    Mtx44 mSecondaryMatrix;
    RenderFloat3 mVectors[8];
    RenderWideWords mWideValues[8];
    RenderWideWords mExtraValues[2];
    RenderWideWords mReferenceValue;
    be<f32> mFirstLimit;
    be<f32> mSecondLimit;
    Mtx44 mFinalMatrix;
    static void destroy(RenderFrameUniforms* storage, u32 flags);
    void resetFrameValues();
    void uploadFrameValues(agl::RenderUniformBuffer_l* buffer);
};
struct RenderMatrixUniforms : RenderUniformPair {
    Mtx34 mMatrix;
    be<u32> mValue;
    static void destroy(RenderMatrixUniforms* storage, u32 flags);
    void resetMatrixValues();
    void uploadMatrixValues(agl::RenderUniformBuffer_l* buffer);
};
}
WWHD_OFFSET(agl::RenderUniformBuffer_l, mCpuStorage, 4);
WWHD_OFFSET(agl::RenderUniformBuffer_l, mMethods, 24);
WWHD_SIZE(agl::RenderUniformBuffer_l, 28);
WWHD_OFFSET(j3d::RenderUniformPairVtable_l, mUpload, 32);
WWHD_OFFSET(j3d::RenderUniformPair, mBuffers, 16);
WWHD_OFFSET(j3d::RenderUniformPair, mActiveIndex, 72);
WWHD_OFFSET(j3d::RenderUniformPair, mPreviousIndex, 76);
WWHD_OFFSET(j3d::RenderUniformPair, mReady, 80);
WWHD_OFFSET(j3d::RenderUniformPair, mCallback, 84);
WWHD_SIZE(j3d::RenderUniformPair, 116);

void resetRenderUniform(agl::RenderUniformBuffer_l* buffer);
void defineRenderUniformFields(agl::RenderUniformBuffer_l* buffer, u32 count, void* heap);
void appendRenderUniformField(agl::RenderUniformBuffer_l* buffer, u32 type, u32 count);
void allocateRenderUniform(agl::RenderUniformBuffer_l* buffer, void* heap, u32 mode);
void queueRenderUniformCallback(j3d::RenderCallbackQueue_l* queue, j3d::RenderCallbackNode_l* callback);

WWHD_SIZE(j3d::RenderWideWords, 16);
WWHD_OFFSET(j3d::RenderFrameUniforms, mPrimaryMatrix, 0x74);
WWHD_OFFSET(j3d::RenderFrameUniforms, mSecondaryMatrix, 0xA4);
WWHD_OFFSET(j3d::RenderFrameUniforms, mVectors, 0xE4);
WWHD_OFFSET(j3d::RenderFrameUniforms, mWideValues, 0x144);
WWHD_OFFSET(j3d::RenderFrameUniforms, mExtraValues, 0x1C4);
WWHD_OFFSET(j3d::RenderFrameUniforms, mReferenceValue, 0x1E4);
WWHD_OFFSET(j3d::RenderFrameUniforms, mFirstLimit, 0x1F4);
WWHD_OFFSET(j3d::RenderFrameUniforms, mSecondLimit, 0x1F8);
WWHD_OFFSET(j3d::RenderFrameUniforms, mFinalMatrix, 0x1FC);
WWHD_SIZE(j3d::RenderFrameUniforms, 0x23C);
WWHD_OFFSET(j3d::RenderMatrixUniforms, mMatrix, 0x74);
WWHD_OFFSET(j3d::RenderMatrixUniforms, mValue, 0xA4);
WWHD_SIZE(j3d::RenderMatrixUniforms, 0xA8);

void resetRenderMatrix34(Mtx34* matrix);
void resetRenderMatrix44(Mtx44* matrix);
void beginRenderUniformWrite(agl::RenderUniformBuffer_l* buffer);
void writeRenderUniformField(agl::RenderUniformBuffer_l* buffer, void* storage, u32 field, const void* value, u32 element, u32 count);
void endRenderUniformWrite(agl::RenderUniformBuffer_l* buffer, void* storage, u32 flags);

WWHD_OFFSET(j3d::RenderUniformPairVtable_l, mBind, 40);
WWHD_SIZE(j3d::RenderUniformPairVtable_l, 48);
using RenderUniformDestroy = void (*)(agl::RenderUniformBuffer_l*, u32);
void destroyRenderUniformBuffer(agl::RenderUniformBuffer_l* buffer, u32 flags);
void destroyRenderUniformArray(agl::RenderUniformBuffer_l* buffers, u32 count, u32 size, RenderUniformDestroy destroy, u32 flags, u32 mode);
void destroyRenderDisposer(sead::Disposer* disposer, u32 flags);
void releaseRenderUniformObject(void* storage);

using RenderUniformConstruct = void (*)(agl::RenderUniformBuffer_l*);
void initializeRenderUniformBuffer(agl::RenderUniformBuffer_l* buffer);
void constructRenderUniformArray(agl::RenderUniformBuffer_l* buffers, u32 count, u32 size, RenderUniformConstruct construct);
void* allocateRenderUniformObject(u32 size);
void constructRenderDisposer(sead::Disposer* disposer);
// Address-only callback binding to the single FF6D0 implementation above.
void uploadRenderUniformPrevious(j3d::RenderUniformPair* storage);

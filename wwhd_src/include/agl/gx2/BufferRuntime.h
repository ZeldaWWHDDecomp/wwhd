/* Views of the graphics utility buffer descriptors and allocation counts. */
#pragma once
#include "wwhd.h"
#include "agl/gx2/ResourceLayout.h"
#include "agl/gx2/MatrixUtility.h"
namespace agl { namespace gx2util {
struct StridedBuffer {
    gptr<u8> mData;
    be<u32> mSliceSize;
    u8 mReserved[4];
    be<u16> mElementStride;
    be<u16> mMode;
    void bind(void* data, u32 stride, u32 mode);
    void* slice(u32 index) const;
    void* mutableSlice(u32 index);
    void flush(u32 index);
    void bindAttribute(u32 slot, u32 index);
    void bindVertexUniform(u32 slot, u32 index);
    void bindGeometryUniform(u32 slot, u32 index);
    void bindPixelUniform(u32 slot, u32 index);
};
struct PaletteResource {
    u8 mPrefix[0xA];
    be<u16> mSmoothCount;
    be<u16> mRigidCount; u8 mReserved[0xA];
    ResourceLink<be<s16>> mIndices; ResourceLink<MatrixUtility> mInverseBind;
};
// Prefix view: only the receiver's first vtable pointer is established here.
struct PoseCallback { gptr<void> mVtable; };
struct PaletteBuffer {
    gptr<PaletteResource> mResource;
    be<u16> mFlags;
    be<u8> mViewCount;
    u8 mReserved;
    gptr<void> mBones; gptr<void> mLocal; gptr<void> mWorld; gptr<void> mScale;
    StridedBuffer mPalette; be<u32> mGpuReserved;
    be<u16> mBoneCount; be<u16> mCallbackBone; gptr<PoseCallback> mCallback;
    gptr<void> mLocalBase; gptr<void> mWorldBase; gptr<void> mPaletteOverride;
    be<u32> mReserved40; gptr<void> mWorkStorage; gptr<void> mStorage;
    u32 allocationSize() const;
    u32 attach(void* data, u32 size);
    u32 initialize(PoseBuilder* builder, void* storage, u32 size);
    void update(void* context);
    void resetValues();
    void updatePalette(u32 view);
    void adjustBoneMatrix(MatrixUtility* output, const MatrixUtility* input, u32 bone, u32 retainWorld);
    void updateRigidWorld(const MatrixUtility* base);
    void updateScaledWorld(const MatrixUtility* base);
    void updateCompensatedWorld(const MatrixUtility* base);
    void updateAccumulatedWorld(const MatrixUtility* base);
};
using UniformResource = UniformDescriptor;
struct UniformBuffer {
    gptr<UniformResource> mResource;
    be<u16> mFlags;
    be<u8> mDirtyMask;
    be<u8> mViewCount;
    be<u16> mWordCount;
    u8 mReserved[2];
    gptr<be<u32>> mDirty;
    gptr<be<u32>> mViewDirty;
    StridedBuffer mGpu;
    be<u32> mGpuReserved;
    gptr<u8> mData;
    gptr<gptr<void>> mBindings;
    be<u32> mState;
    gptr<void> mStorage;
    gptr<void> mGpuStorage;
    u32 allocationSize() const;
    u32 initialize(UniformBuilder* builder, void* storage, u32 size);
    void relocateData();
    void rebuildDirty();
    u32 attach(void* data, u32 size);
    void updateView(u32 view);
};
struct ViewDescriptor { u8 mPrefix[0x10]; be<u16> mBone; u8 mReserved[4]; be<u8> mMode; };
struct ViewGpu { StridedBuffer mBuffer; be<u32> mReserved; };
struct ViewAllocation {
    gptr<ViewDescriptor> mResource; be<u32> mFlags;
    be<u8> mInstanceCount; be<u8> mMultiBuffer;
    be<u8> mArrayCount; be<u8> mViewCount;
    gptr<ViewGpu> mBuffers; u8 mReserved[0xC];
    gptr<be<u32>> mCustom; be<u32> mCustomBytes;
    be<u32> mTail; gptr<void> mStorage; gptr<void> mGpuStorage;
    u32 allocationSize() const;
    u32 attach(void* data, u32 size);
    void update(u32 instance, const be<u32>* matrix, u32 view);
};
struct PoseBoneFlags { u8 mPrefix[0x10]; be<u32> mFlags; u8 mTail[0x2C]; };
struct PaletteRuntimeView { u8 mPrefix[8]; gptr<PoseBoneFlags> mBones; u8 mReserved[0x20]; be<u16> mBoneCount; };
struct UniformVisibility { u8 mPrefix[8]; be<u32> mFlags; };
struct ModelBuilder;
struct ModelRuntime {
    gptr<void> mResource; gptr<be<u32>> mBoneVisibility; gptr<be<u32>> mUniformVisibility;
    be<u8> mInstanceCount; be<u8> mMultiBuffer; be<u16> mFlags; be<u32> mState;
    be<u16> mViewCount; be<u16> mUniformCount;
    gptr<PaletteBuffer> mPose; gptr<ViewAllocation> mViews;
    gptr<UniformBuffer> mUniforms; gptr<void> mCache; be<u32> mCacheState; gptr<void> mStorage; gptr<void> mGpuStorage;
    u32 initialize(ModelBuilder* builder, void* storage, u32 size);
    void resetBoneVisibility();
    void resetUniformVisibility();
    u32 allocationSize() const;
    u32 attach(void* data, u32 size);
    void updatePalette(u32 view);
    void updateViews(u32 view);
    void updateUniforms(u32 view);
    void updateBillboardViews(u32 instance, const MatrixUtility* camera, u32 view);
};
WWHD_OFFSET(StridedBuffer, mSliceSize, 4);
WWHD_OFFSET(StridedBuffer, mElementStride, 0xC);
WWHD_OFFSET(StridedBuffer, mMode, 0xE);
WWHD_OFFSET(ViewDescriptor, mBone, 0x10);
WWHD_OFFSET(ViewDescriptor, mMode, 0x16);
WWHD_SIZE(StridedBuffer, 0x10);
WWHD_OFFSET(PaletteResource, mSmoothCount, 0xA);
WWHD_OFFSET(PaletteResource, mRigidCount, 0xC);
WWHD_SIZE(PaletteResource, 0x20);
WWHD_OFFSET(PaletteBuffer, mViewCount, 6);
WWHD_OFFSET(PaletteBuffer, mPalette, 0x18);
WWHD_OFFSET(PoseCallback, mVtable, 0);
WWHD_OFFSET(PaletteBuffer, mCallbackBone, 0x2E);
WWHD_OFFSET(PaletteBuffer, mCallback, 0x30);
WWHD_OFFSET(PaletteBuffer, mReserved40, 0x40);
WWHD_OFFSET(PaletteBuffer, mStorage, 0x48);
WWHD_SIZE(PaletteBuffer, 0x4C);
WWHD_OFFSET(UniformResource, mByteCount, 0x18);
WWHD_SIZE(UniformResource, 0x40);
WWHD_OFFSET(UniformBuffer, mViewCount, 7);
WWHD_OFFSET(UniformBuffer, mDirty, 0xC);
WWHD_OFFSET(UniformBuffer, mGpu, 0x14);
WWHD_OFFSET(UniformBuffer, mData, 0x28);
WWHD_OFFSET(UniformBuffer, mBindings, 0x2C);
WWHD_OFFSET(UniformBuffer, mStorage, 0x34);
WWHD_SIZE(UniformBuffer, 0x3C);
WWHD_OFFSET(ViewAllocation, mBuffers, 0xC);
WWHD_OFFSET(ViewAllocation, mCustomBytes, 0x20);
WWHD_SIZE(ViewAllocation, 0x30);
WWHD_SIZE(ViewGpu, 0x14);
} }

/* Storage planning for the observed model pose, parameters and uniforms. */
#include "agl/gx2/ModelRuntime.h"
#include "agl/gx2/PoseRecords.h"
using namespace agl::gx2util;
struct ModelSubBuilder { u8 mStorage[sizeof(ParameterBuilder)]; };

/* 027DDD10 */
void agl::gx2util::ModelLayout::calculate(ModelBuilder* builder) {
    gabi::AutoFrame frame{gabi::current_function()};
    ModelDescriptor* resource = builder->mResource;
    PoseResource* pose = resource->mPose.get();
    u32 poseBytes = 0;
    gabi::FrameLocal<ModelSubBuilder> temporaryStorage(0x8);
    auto& temporary = *temporaryStorage;
    if (builder->mExternalPose == NULL) {
        auto sub = reinterpret_cast<PoseBuilder*>(&temporary);
        sub->mResource = pose;
        sub->mFlags = builder->mPoseViews;
        sub->mLayout.mTotal = 0;
        sub->mLayout.mBlocks = NULL;
        poseBytes = (sub->workSize() + 63) & ~63u;
    }
    u32 views = resource->mViewCount;
    u32 instances = builder->mInstanceCount;
    u32 uniforms = resource->mUniformCount;
    u32 bones = pose->mBoneCount;
    u32 viewBytes = 0;
    for (u32 index = 0; index < views; ++index) {
        ParameterDescriptor* parameter = static_cast<ParameterDescriptor*>(resource->mViews.get()->mNodes[index + 1u].mValue.get());
        auto sub = reinterpret_cast<ParameterBuilder*>(&temporary);
        u32 paramViews = builder->mParameterViews;
        sub->mLayout.mTotal = 0;
        sub->mViewCount = paramViews;
        sub->mCached = 0;
        sub->mLayout.mBlocks = NULL;
        sub->mInstanceCount = 1;
        sub->mMultiBuffer = 0;
        sub->mCustomBytes = 0;
        sub->mResource = parameter;
        auto boneData = static_cast<const PoseBoneDescriptor*>(pose->mBones.get()) + (u32)parameter->mBoneIndex;
        u32 boneFlags = boneData->mFlags;
        u32 cached = builder->mCached;
        u32 custom = builder->mCustomBytes;
        sub->mMultiBuffer = (boneFlags & 0x70000) != 0;
        sub->mInstanceCount = instances;
        sub->mLayout.mBlocks = NULL;
        sub->mLayout.mTotal = 0;
        sub->mCached = cached != 0;
        sub->mCustomBytes = custom;
        viewBytes += (sub->workSize() + 31) & ~31u;
    }
    u32 uniformBytes = 0;
    for (u32 index = 0; index < uniforms; ++index) {
        UniformDescriptor* uniform = static_cast<UniformDescriptor*>(resource->mUniforms.get()->mNodes[index + 1u].mValue.get());
        auto sub = reinterpret_cast<UniformBuilder*>(&temporary);
        sub->mResource = uniform;
        sub->mViewCount = builder->mUniformViews;
        sub->mLayout.mTotal = 0;
        sub->mLayout.mBlocks = NULL;
        uniformBytes += (sub->workSize() + 3) & ~3u;
    }
    mSlots[0].mSize = poseBytes;
    mSlots[1].mSize = viewBytes;
    mSlots[2].mSize = uniformBytes;
    mSlots[3].mSize = 0x4C;
    mSlots[4].mSize = views * 0x30;
    mSlots[5].mSize = uniforms * 0x3C;
    mSlots[6].mSize = ((bones + 31) >> 5) * 4;
    mSlots[7].mSize = ((uniforms + 31) >> 5) * 4;
    mSlots[8].mSize = builder->mCached == 0 ? 0 : 16;
    mSlots[0].mOffset = 0;
    for (u32 index = 0; index < 8; ++index) mSlots[index + 1].mOffset = (u32)mSlots[index].mOffset + (u32)mSlots[index].mSize;
    u32 lastSize = mSlots[8].mSize;
    u32 lastOffset = mSlots[8].mOffset;
    mBlocks = mSlots;
    mTotal = lastOffset + lastSize;
}

/* 027DDFA4 */
u32 agl::gx2util::ModelBuilder::workSize() {
    gabi::AutoFrame frame{gabi::current_function()};
    mLayout.calculate(this);
    return mLayout.mTotal;
}

extern void* constructModelBuffer(void*);
extern void clearGraphicsWords(void*, u32);
static void* modelBlockAddress(LayoutBlock* block, void* storage) {
    return (u32)block->mSize == 0 ? NULL : static_cast<u8*>(storage) + (u32)block->mOffset;
}

/* 027DE0AC */
u32 agl::gx2util::ModelRuntime::initialize(ModelBuilder* builder, void* storage, u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (builder->mLayout.mBlocks == NULL) builder->mLayout.calculate(builder);
    if ((u32)builder->mLayout.mTotal > size) return 0;
    ModelDescriptor* resource = builder->mResource;
    u32 instances = builder->mInstanceCount;
    PoseResource* poseResource = resource->mPose.get();
    u32 uniforms = resource->mUniformCount;
    u32 views = resource->mViewCount;
    mResource = resource;
    mBoneVisibility = static_cast<be<u32>*>(modelBlockAddress(builder->mLayout.mBlocks.get() + 6, storage));
    auto uniformVisibility = static_cast<be<u32>*>(modelBlockAddress(builder->mLayout.mBlocks.get() + 7, storage));
    mViewCount = views;
    mInstanceCount = instances;
    mFlags = 0;
    mUniformVisibility = uniformVisibility;
    mPose = NULL;
    mUniformCount = uniforms;
    mViews = static_cast<ViewAllocation*>(modelBlockAddress(builder->mLayout.mBlocks.get() + 4, storage));
    mUniforms = static_cast<UniformBuffer*>(modelBlockAddress(builder->mLayout.mBlocks.get() + 5, storage));
    void* cache = modelBlockAddress(builder->mLayout.mBlocks.get() + 8, storage);
    mGpuStorage = NULL;
    mCache = cache;
    mStorage = storage;
    mCacheState = 0;
    gabi::FrameLocal<ModelSubBuilder> temporaryStorage(0x8);
    auto& temporary = *temporaryStorage;
    PaletteBuffer* external = builder->mExternalPose;
    if (external != NULL) mPose = external;
    else {
        auto sub = reinterpret_cast<PoseBuilder*>(&temporary);
        sub->mResource = poseResource;
        sub->mFlags = builder->mPoseViews;
        sub->mLayout.mTotal = 0;
        sub->mLayout.mBlocks = NULL;
        u32 bytes = sub->workSize();
        auto pose = static_cast<PaletteBuffer*>(modelBlockAddress(builder->mLayout.mBlocks.get() + 3, storage));
        if (pose != NULL) {
            pose->mResource = NULL;
            constructModelBuffer(&pose->mPalette);
            pose->mReserved40 = 0;
            pose->mStorage = NULL;
            pose->mWorkStorage = NULL;
        }
        mPose = pose;
        void* data = modelBlockAddress(builder->mLayout.mBlocks.get(), storage);
        pose->initialize(sub, data, bytes);
    }
    auto parameterData = static_cast<u8*>(modelBlockAddress(builder->mLayout.mBlocks.get() + 1, storage));
    u32 multi = 0;
    for (u32 index = 0; index < views; ++index) {
        ParameterDescriptor* parameter = static_cast<ParameterDescriptor*>(resource->mViews.get()->mNodes[index + 1u].mValue.get());
        auto sub = reinterpret_cast<ParameterBuilder*>(&temporary);
        sub->mInstanceCount = instances;
        sub->mViewCount = builder->mParameterViews;
        sub->mLayout.mTotal = 0;
        sub->mMultiBuffer = 0;
        sub->mCached = 0;
        sub->mCustomBytes = 0;
        sub->mLayout.mBlocks = NULL;
        sub->mResource = parameter;
        auto bones = static_cast<u8*>(mPose->mBones.get());
        u32 boneFlags = *reinterpret_cast<be<u32>*>(bones + (u32)parameter->mBoneIndex * 64 + 16);
        u32 cached = builder->mCached;
        if (boneFlags & 0x70000) { multi = 1; sub->mMultiBuffer = 1; }
        else sub->mMultiBuffer = 0;
        u32 custom = builder->mCustomBytes;
        sub->mLayout.mBlocks = NULL;
        sub->mLayout.mTotal = 0;
        sub->mCached = cached != 0;
        sub->mCustomBytes = custom;
        u32 bytes = (sub->workSize() + 31) & ~31u;
        auto target = reinterpret_cast<ParameterRuntime*>(mViews.get() + index);
        if (target != NULL) {
            target->mCacheState = 0;
            target->mStorage = NULL;
            target->mResource = NULL;
            target->mReserved = 0;
        }
        target->initialize(sub, parameterData, bytes);
        parameterData += bytes;
    }
    mMultiBuffer = multi;
    auto uniformData = static_cast<u8*>(modelBlockAddress(builder->mLayout.mBlocks.get() + 2, storage));
    for (u32 index = 0; index < uniforms; ++index) {
        UniformDescriptor* uniform = static_cast<UniformDescriptor*>(resource->mUniforms.get()->mNodes[index + 1u].mValue.get());
        auto sub = reinterpret_cast<UniformBuilder*>(&temporary);
        sub->mResource = uniform;
        sub->mViewCount = builder->mUniformViews;
        sub->mLayout.mTotal = 0;
        sub->mLayout.mBlocks = NULL;
        u32 bytes = (sub->workSize() + 3) & ~3u;
        UniformBuffer* target = mUniforms.get() + index;
        if (target != NULL) {
            target->mResource = NULL;
            constructModelBuffer(&target->mGpu);
            target->mState = 0;
            target->mGpuStorage = NULL;
            target->mStorage = NULL;
        }
        target->initialize(sub, uniformData, bytes);
        uniformData += bytes;
    }
    resetBoneVisibility();
    resetUniformVisibility();
    if (mCache != NULL) clearGraphicsWords(mCache, 4);
    return 1;
}

/* WWHD glue
 ext 027DD6EC void* constructModelBuffer(void*)
*/

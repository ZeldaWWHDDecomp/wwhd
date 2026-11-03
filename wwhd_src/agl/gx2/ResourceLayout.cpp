/* Graphics-resource work-memory layouts. Field loads and prefix writes preserve aliasing. */
#include "agl/gx2/ResourceLayout.h"
using namespace agl::gx2util;
extern void* graphicsAllocate(u32 size);
extern void clearGraphicsWords(void* data, u32 count);

/* 027DAB24 */
void agl::gx2util::PoseLayout::calculate(const PoseBuilder* builder) {
    gabi::AutoFrame frame{gabi::current_function()};
    const PoseResource* resource = builder->mResource;
    u32 bones = resource->mBoneCount;
    u32 flags = resource->mFlags;
    u32 matrices = (bones * 0x30 + 0x3F) & ~0x3Fu;
    u32 poses = (bones * 0x40 + 0x3F) & ~0x3Fu;
    mSlots[0].mSize = matrices;
    mSlots[1].mSize = poses;
    u32 scales = (flags & 0x300) == 0x300 ? bones * 0xC : 0;
    mSlots[0].mOffset = 0;
    mSlots[2].mSize = scales;
    for (u32 index = 0; index < 2; ++index) {
        u32 offset = mSlots[index].mOffset;
        u32 size = mSlots[index].mSize;
        mSlots[index + 1].mOffset = offset + size;
    }
    u32 size = mSlots[2].mSize;
    u32 offset = mSlots[2].mOffset;
    mBlocks = mSlots;
    mTotal = offset + size;
}

/* 027DABB4 */
u32 agl::gx2util::PoseBuilder::workSize() {
    gabi::AutoFrame frame{gabi::current_function()};
    mLayout.calculate(this);
    return mLayout.mTotal;
}

/* 027DCF64 */
void agl::gx2util::UniformLayout::calculate(const UniformBuilder* builder) {
    gabi::AutoFrame frame{gabi::current_function()};
    const UniformDescriptor* resource = builder->mResource;
    u32 bits = resource->mBitCount;
    s32 views = builder->mViewCount;
    u32 perView = ((bits + 31) >> 5) * 4;
    mSlots[0].mSize = perView;
    mSlots[1].mSize = views > 1 ? perView * (u32)views : 0;
    u32 dataBytes = resource->mDataBytes;
    mSlots[2].mSize = (dataBytes + 3) & ~3u;
    u32 words = resource->mWordCount;
    mSlots[0].mOffset = 0;
    mSlots[3].mSize = words << 2;
    for (u32 index = 0; index < 3; ++index) {
        u32 offset = mSlots[index].mOffset;
        u32 size = mSlots[index].mSize;
        mSlots[index + 1].mOffset = offset + size;
    }
    u32 size = mSlots[3].mSize;
    u32 offset = mSlots[3].mOffset;
    mBlocks = mSlots;
    mTotal = offset + size;
}

/* 027DCFF8 */
u32 agl::gx2util::UniformBuilder::workSize() {
    gabi::AutoFrame frame{gabi::current_function()};
    mLayout.calculate(this);
    return mLayout.mTotal;
}

/* 027DD5E4 */
void agl::gx2util::ParameterLayout::calculate(const ParameterBuilder* builder) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 cached = builder->mCached;
    u32 instances = builder->mInstanceCount;
    const ParameterDescriptor* resource = builder->mResource;
    u32 multiBuffer = builder->mMultiBuffer;
    u32 curveBytes = 0;
    u32 floats;
    u32 customBytes;
    if (cached != 0) {
        u32 curves = resource->mCurveCount;
        floats = resource->mFloatCount;
        curveBytes = (curves * 0x18 + 0x1F) & ~0x1Fu;
        customBytes = builder->mCustomBytes;
    } else {
        customBytes = builder->mCustomBytes;
        floats = resource->mFloatCount;
    }
    u32 buffers = multiBuffer == 0 ? 1 : instances;
    u32 scratchBytes = buffers * 0x14;
    u32 customAligned = (customBytes + 3) & ~3u;
    mSlots[0].mSize = curveBytes;
    mSlots[1].mSize = scratchBytes;
    mSlots[2].mSize = floats << 2;
    mSlots[0].mOffset = 0;
    mSlots[3].mSize = cached == 0 ? 0 : 0x20;
    mSlots[4].mSize = customAligned;
    for (u32 index = 0; index < 4; ++index) {
        u32 offset = mSlots[index].mOffset;
        u32 size = mSlots[index].mSize;
        mSlots[index + 1].mOffset = offset + size;
    }
    u32 size = mSlots[4].mSize;
    u32 offset = mSlots[4].mOffset;
    mBlocks = mSlots;
    mTotal = offset + size;
}

/* 027DD6C0 */
u32 agl::gx2util::ParameterBuilder::workSize() {
    gabi::AutoFrame frame{gabi::current_function()};
    mLayout.calculate(this);
    return mLayout.mTotal;
}

/* 027DD6EC */
static ParameterScratch* constructParameterScratch(ParameterScratch* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<ParameterScratch*>(graphicsAllocate(sizeof(ParameterScratch)));
    if (object != NULL) clearGraphicsWords(object, 5);
    return object;
}

/* 027DD738 */
void agl::gx2util::ParameterRuntime::clearValues() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = mResource->mFloatCount;
    for (u32 index = 0; index < count; ++index) mFloats[index] = 0.0f;
    mFlags = (u32)mFlags & ~4u;
}



static void* parameterBlockAddress(LayoutBlock* block, void* storage) {
    u32 size = block->mSize;
    return size == 0 ? NULL : static_cast<u8*>(storage) + (u32)block->mOffset;
}

/* 027DD778 */
u32 agl::gx2util::ParameterRuntime::initialize(ParameterBuilder* builder, void* storage, u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (builder->mLayout.mBlocks == NULL) builder->mLayout.calculate(builder);
    if ((u32)builder->mLayout.mTotal > size) return 0;
    ParameterDescriptor* resource = builder->mResource;
    u32 instances = builder->mInstanceCount;
    mResource = resource;
    mInstanceCount = instances;
    mFlags = 2;
    mMultiBuffer = builder->mMultiBuffer;
    mBufferCount = builder->mMultiBuffer == 0 ? 1 : (u32)mInstanceCount;
    mViewCount = builder->mViewCount;
    mScratch = static_cast<ParameterScratch*>(parameterBlockAddress(builder->mLayout.mBlocks.get() + 1, storage));
    mFloats = static_cast<be<f32>*>(parameterBlockAddress(builder->mLayout.mBlocks.get() + 2, storage));
    mDefaults = parameterBlockAddress(builder->mLayout.mBlocks.get() + 3, storage);
    mCurves = parameterBlockAddress(builder->mLayout.mBlocks.get(), storage);
    mCustom = parameterBlockAddress(builder->mLayout.mBlocks.get() + 4, storage);
    u32 custom = builder->mCustomBytes;
    mStorage = storage;
    be<u32>* defaults = static_cast<be<u32>*>(mDefaults.get());
    mCacheState = 0;
    mReserved = 0;
    mCustomBytes = (custom + 3) & ~3u;
    if (defaults != NULL) {
        be<u32>* source = mResource->mDefaults.get();
        be<u32>* next = defaults + 4;
        u32 first = source[0];
        next[0] = first;
        next[1] = (u32)source[1];
        next[2] = (u32)source[2];
        next[3] = *reinterpret_cast<be<u32>*>(&mResource->mDefaultValue);
        defaults = static_cast<be<u32>*>(mDefaults.get());
        defaults[0] = first;
        defaults[1] = (u32)next[1];
        defaults[2] = (u32)next[2];
        defaults[3] = (u32)next[3];
    }
    u32 buffers = mBufferCount;
    for (u32 index = 0; index < buffers; ++index) {
        ParameterScratch* scratch = mScratch.get() + index;
        if (scratch != NULL) {
            constructParameterScratch(scratch);
            buffers = mBufferCount;
        }
    }
    clearValues();
    return 1;
}

/* WWHD glue
 ext 028F5914 void clearGraphicsWords(void*, u32)
*/

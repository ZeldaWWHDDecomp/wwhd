/* Graphics palette allocations, strided buffer views and cache boundary calls. */
#include "agl/gx2/BufferRuntime.h"
#include "agl/gx2/PoseRecords.h"
#include "sead/ElementAt.h"
using namespace agl::gx2util;
extern void invalidateGraphicsRange(void* data, u32 size);
extern void flushGraphicsRange(void* data, u32 size);
extern u32 queryGraphicsAddressRange(void* data, u32 size);
extern void zeroGraphicsRange(void* data, u32 size);

/* 027DB10C */
u32 agl::gx2util::PaletteBuffer::allocationSize() const {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 matrices = (u32)mResource->mSmoothCount + (u32)mResource->mRigidCount;
    u32 perView = (matrices * 0x30 + 0xFF) & ~0xFFu;
    return perView * (u32)mViewCount;
}

/* 027DB134 */
void agl::gx2util::StridedBuffer::bind(void* data, u32 stride, u32 mode) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* chosen = stride == 0 ? NULL : data;
    mSliceSize = stride;
    mMode = mode;
    mData = static_cast<u8*>(chosen);
}

/* 027DB154 */
u32 agl::gx2util::PaletteBuffer::attach(void* data, u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 required = allocationSize();
    if (required > size) return 0;
    u32 views = mViewCount;
    u32 stride = views == 0 ? 0 : required / views;
    mStorage = data;
    mPalette.bind(data, stride, views);
    invalidateGraphicsRange(mPalette.mData, required);
    mFlags = (u16)mFlags | 1;
    return 1;
}

/* 027DBBB0 */
void* agl::gx2util::StridedBuffer::slice(u32 index) const {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 offset = (u32)mSliceSize * index;
    return static_cast<u8*>(mData) + offset;
}

/* 027DBBC4 */
static u32 isGraphicsRangeValid(void* data, u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    return queryGraphicsAddressRange(data, size) != 0;
}

/* 027DBBEC */
void* agl::gx2util::StridedBuffer::mutableSlice(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 offset = (u32)mSliceSize * index;
    return static_cast<u8*>(mData) + offset;
}

/* 027DBC00 */
void agl::gx2util::StridedBuffer::flush(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* data = mutableSlice(index);
    flushGraphicsRange(data, mSliceSize);
}

/* 027DD27C */
u32 agl::gx2util::UniformBuffer::allocationSize() const {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 bytes = mResource->mByteCount;
    return ((bytes + 0xFF) & ~0xFFu) * (u32)mViewCount;
}

/* 027DD998 */
u32 agl::gx2util::ViewAllocation::allocationSize() const {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 views = mViewCount;
    u32 arrays = mArrayCount;
    return (views * arrays) << 8;
}

/* 027DD9AC */
static void clearGraphicsRange(void* data, u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    zeroGraphicsRange(data, size);
}

extern void clearGraphicsWords(void* data, u32 count);
extern void* copyUniformBytes(void* destination, const void* source, u32 count);
extern void* fillUniformBytes(void* destination, u32 value, u32 count);
extern void updateUniformReversed(UniformBuffer* object, void* destination, be<u32>* dirty);
extern void updateUniformNative(UniformBuffer* object, void* destination, be<u32>* dirty);

static void* uniformBlockAddress(const LayoutBlock* block, void* storage) {
    u32 size = block->mSize;
    return size == 0 ? NULL : static_cast<u8*>(storage) + (u32)block->mOffset;
}

/* 027DD024 */
void agl::gx2util::UniformBuffer::relocateData() {
    gabi::AutoFrame frame{gabi::current_function()};
    UniformDescriptor* resource = mResource;
    u32 count = resource->mBitCount;
    for (u32 index = 0; index < count; ++index) {
        UniformFixup* entry = resource->mFixups.get() + index;
        u32 target = entry->mTarget;
        if (target != index) {
            u32 offset = entry->mDataOffset;
            u32 extra = entry->mExtraOffset;
            u8* data = mData;
            UniformFixup* targetEntry = resource->mFixups.get() + target;
            u16 targetOffset = targetEntry->mDataOffset;
            auto slot = reinterpret_cast<gptr<u8>*>(data + (u32)(offset + extra - 4));
            *slot = data + targetOffset;
        }
        resource = mResource;
    }
}

/* 027DD0BC */
u32 agl::gx2util::UniformBuffer::initialize(UniformBuilder* builder, void* storage, u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (builder->mLayout.mBlocks == NULL) builder->mLayout.calculate(builder);
    if ((u32)builder->mLayout.mTotal > size) return 0;
    UniformDescriptor* resource = builder->mResource;
    u32 count = resource->mBitCount;
    u32 bindings = resource->mWordCount;
    mResource = resource;
    mDirtyMask = 0;
    mFlags = 2;
    u32 initialViews = builder->mViewCount;
    mWordCount = (count + 31) >> 5;
    mViewCount = initialViews;
    mDirty = static_cast<be<u32>*>(uniformBlockAddress(builder->mLayout.mBlocks.get(), storage));
    mViewDirty = static_cast<be<u32>*>(uniformBlockAddress(builder->mLayout.mBlocks.get() + 1, storage));
    clearGraphicsWords(&mGpu, 5);
    mData = static_cast<u8*>(uniformBlockAddress(builder->mLayout.mBlocks.get() + 2, storage));
    void* initial = resource->mInitialData.get();
    u32 bytes = resource->mDataBytes;
    copyUniformBytes(mData, initial, bytes);
    auto bound = static_cast<gptr<void>*>(uniformBlockAddress(builder->mLayout.mBlocks.get() + 3, storage));
    u32 views = mViewCount;
    mStorage = storage;
    mGpuStorage = NULL;
    mState = 0;
    mBindings = bound;
    if (views == 1) mViewDirty = mDirty.get();
    for (u32 index = 0; index < bindings; ++index) {
        UniformBinding* binding = resource->mBindings.get() + index;
        void* data = binding->mTexture.get();
        mBindings[index] = data;
    }
    relocateData();
    return 1;
}

struct DirtyCountWords { be<u32> mLimit; be<u32> mRemaining; };
WWHD_OFFSET(DirtyCountWords, mLimit, 0);
WWHD_OFFSET(DirtyCountWords, mRemaining, 4);
WWHD_SIZE(DirtyCountWords, 8);

/* 027DD298 */
void agl::gx2util::UniformBuffer::rebuildDirty() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 remaining = mResource->mBitCount;
    u32 groups = (remaining + 31) >> 5;
    fillUniformBytes(mDirty, 0, groups * 4);
    gabi::FrameLocal<DirtyCountWords> countStorage(0x08);
    u32 first = 0;
    for (u32 group = 0; group < groups; ++group) {
        countStorage->mRemaining = remaining;
        countStorage->mLimit = 32;
        u32 mask = 0;
        u32 count = remaining < 32 ? remaining : 32;
        UniformDescriptor* resource = mResource;
        for (u32 bit = 0; bit < count; ++bit) {
            UniformFixup* entries = resource->mFixups.get();
            if ((s32)entries[first + bit].mFlags >= 0) mask |= 1u << bit;
        }
        first += 32;
        remaining -= 32;
        mDirty[group] = mask;
    }
    mFlags = (u16)mFlags | 4;
}

/* 027DD394 */
u32 agl::gx2util::UniformBuffer::attach(void* data, u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 required = allocationSize();
    if (required > size) return 0;
    mGpuStorage = data;
    rebuildDirty();
    u32 views = mViewCount;
    if (views > 1) {
        u32 words = mWordCount;
        be<u32>* next = mDirty.get() + words;
        s32 count = words * views;
        while (count > 0) { *next++ = 0; --count; }
        views = mViewCount;
    }
    u32 stride = views == 0 ? 0 : required / views;
    mGpu.bind(data, stride, views);
    mFlags = (u16)mFlags | 1;
    return 1;
}

/* 027DD468 */
void agl::gx2util::UniformBuffer::updateView(u32 view) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 flags = mFlags;
    if (flags & 4) {
        u32 views = mViewCount;
        if (views > 1) {
            u32 words = mWordCount;
            for (u32 word = 0; word < words; ++word) {
                u32 dirty = mDirty[word];
                u32 count = mViewCount;
                for (u32 slot = 0; slot < count; ++slot) {
                    u32 offset = slot * words + word;
                    mViewDirty[offset] = (u32)mViewDirty[offset] | dirty;
                    count = mViewCount;
                    words = mWordCount;
                }
                mDirty[word] = 0;
                words = mWordCount;
            }
        }
        flags = mFlags;
        mDirtyMask = 0xFF;
        mFlags = flags ^ 4;
    }
    u32 shift = view & 63;
    u32 selected = shift < 32 ? 1u << shift : 0;
    if (((u32)mDirtyMask & selected) != 0 || mResource->mDynamicCount != 0) {
        void* destination = mGpu.slice(view);
        u32 words = mWordCount;
        be<u32>* dirty = mViewDirty.get() + view * words;
        if (flags & 2) updateUniformReversed(this, destination, dirty);
        else updateUniformNative(this, destination, dirty);
        mDirtyMask = (u8)mDirtyMask & ~selected;
        mGpu.flush(view);
    }
}



extern void* copyUniformBytes(void*, const void*, u32);
extern void clearGraphicsRange(void*, u32);
extern u32 isGraphicsRangeValid(void*, u32);

static u32 reverseGraphicsWord(u32 value) {
    return (value >> 24) | ((value >> 8) & 0xFF00) | ((value << 8) & 0xFF0000) | (value << 24);
}

struct ViewInitialData { be<u32> mMode; be<u32> mMatrix[12]; };
struct ViewModeData { be<u32> mValue; };

/* 027DD9B0 */
u32 agl::gx2util::ViewAllocation::attach(void* data, u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (allocationSize() > size) return 0;
    ViewDescriptor* resource = mResource;
    mGpuStorage = data;
    gabi::FrameLocal<ViewInitialData> initialStorage(0x8);
    auto& initial = *initialStorage;
    for (u32 i = 0; i < 12; ++i) initial.mMatrix[i] = (i == 0 || i == 5 || i == 10) ? 0x3F800000 : 0;
    initial.mMode = resource->mMode;
    u32 bytes = ((u32)mCustomBytes + 31) & ~31u;
    u32 count = mArrayCount;
    for (u32 array = 0; array < count; ++array) {
        u32 views = mViewCount;
        ViewGpu* gpu = mBuffers.get() + array;
        gpu->mBuffer.bind(static_cast<u8*>(data) + ((array * views) << 8), 256, views);
        views = mViewCount;
        for (u32 view = 0; view < views; ++view) {
            auto destination = static_cast<be<u32>*>(gpu->mBuffer.slice(view));
            if (isGraphicsRangeValid(destination, bytes + 64)) clearGraphicsRange(destination, bytes + 64);
            if ((u32)mFlags & 2) {
                for (u32 i = 0; i < 12; ++i) destination[i] = reverseGraphicsWord(initial.mMatrix[i]);
                destination[12] = reverseGraphicsWord(initial.mMode);
            } else {
                copyUniformBytes(destination, initial.mMatrix, 48);
                copyUniformBytes(destination + 12, &initial.mMode, 4);
            }
            gpu->mBuffer.flush(view);
            views = mViewCount;
        }
    }
    mFlags = (u32)mFlags | 1;
    return 1;
}

/* 027DDBB0 */
void agl::gx2util::ViewAllocation::update(u32 instance, const be<u32>* matrix, u32 view) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 array = instance * (u32)mMultiBuffer;
    ViewGpu* gpu = mBuffers.get() + array;
    auto destination = static_cast<be<u32>*>(gpu->mBuffer.slice(view));
    u32 bytes = ((u32)mCustomBytes + 31) & ~31u;
    gabi::FrameLocal<ViewModeData> modeStorage(0x8);
    auto& mode = *modeStorage;
    mode.mValue = mResource->mMode;
    if (isGraphicsRangeValid(destination, bytes + 64)) clearGraphicsRange(destination, bytes + 64);
    if ((u32)mFlags & 2) {
        for (u32 i = 0; i < 12; ++i) destination[i] = reverseGraphicsWord(matrix[i]);
        destination[12] = reverseGraphicsWord(mode.mValue);
        u32 words = (u32)mCustomBytes >> 2;
        be<u32>* custom = mCustom;
        for (u32 i = 0; i < words; ++i) destination[16 + i] = reverseGraphicsWord(custom[i]);
    } else {
        copyUniformBytes(destination, matrix, 48);
        copyUniformBytes(destination + 12, &mode.mValue, 4);
        u32 bytes = (u32)mCustomBytes & ~3u;
        copyUniformBytes(destination + 16, mCustom, bytes);
    }
    gpu->mBuffer.flush(view);
}

/* 027DDFDC */
void agl::gx2util::ModelRuntime::resetBoneVisibility() {
    gabi::AutoFrame frame{gabi::current_function()};
    auto pose = reinterpret_cast<PaletteRuntimeView*>(mPose.get());
    u32 count = pose->mBoneCount;
    for (u32 index = 0; index < count; ++index) {
        u32 visible = (u32)pose->mBones[index].mFlags & 1;
        be<u32>* target = mBoneVisibility.get() + (index >> 5);
        u32 previous = *target;
        u32 shift = index & 31;
        *target = (previous & ~(1u << shift)) | (visible << shift);
        pose = reinterpret_cast<PaletteRuntimeView*>(mPose.get());
    }
}

/* 027DE048 */
void agl::gx2util::ModelRuntime::resetUniformVisibility() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = mUniformCount;
    for (u32 index = 0; index < count; ++index) {
        auto resource = reinterpret_cast<UniformVisibility*>(mUniforms[index].mResource.get());
        u32 visible = (u32)resource->mFlags & 1;
        be<u32>* target = mUniformVisibility.get() + (index >> 5);
        u32 previous = *target;
        u32 shift = index & 31;
        *target = (previous & ~(1u << shift)) | (visible << shift);
    }
}

/* 027DE4D8 */
u32 agl::gx2util::ModelRuntime::allocationSize() const {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 total = 0;
    PaletteBuffer* pose = mPose;
    if (pose != NULL && !((u32)pose->mFlags & 1)) total = (pose->allocationSize() + 255) & ~255u;
    u32 views = mViewCount;
    ViewAllocation* entries = mViews;
    for (u32 index = 0; index < views; ++index) {
        ViewAllocation* entry = entries + index;
        if (!((u32)entry->mFlags & 1)) total += (entry->allocationSize() + 255) & ~255u;
    }
    u32 uniforms = mUniformCount;
    UniformBuffer* buffers = mUniforms;
    for (u32 index = 0; index < uniforms; ++index) {
        UniformBuffer* entry = buffers + index;
        if (!((u32)entry->mFlags & 1)) total += (entry->allocationSize() + 255) & ~255u;
    }
    return total;
}

/* 027DE5BC */
u32 agl::gx2util::ModelRuntime::attach(void* data, u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (allocationSize() > size) return 0;
    u8* next = static_cast<u8*>(data);
    PaletteBuffer* pose = mPose;
    mGpuStorage = data;
    u32 success = 1;
    if (pose != NULL && !((u32)pose->mFlags & 1)) {
        u32 bytes = pose->allocationSize();
        u32 aligned = (bytes + 255) & ~255u;
        success = pose->attach(next, bytes) != 0;
        next += aligned;
    }
    u32 views = mViewCount;
    for (u32 index = 0; index < views; ++index) {
        ViewAllocation* entry = mViews.get() + index;
        if (!((u32)entry->mFlags & 1)) {
            u32 bytes = entry->allocationSize();
            u32 aligned = (bytes + 255) & ~255u;
            u32 result = entry->attach(next, bytes);
            success = result != 0 && success != 0;
            views = mViewCount;
            next += aligned;
        }
    }
    u32 uniforms = mUniformCount;
    for (u32 index = 0; index < uniforms; ++index) {
        UniformBuffer* entry = mUniforms.get() + index;
        if (!((u32)entry->mFlags & 1)) {
            u32 bytes = entry->allocationSize();
            u32 aligned = (bytes + 255) & ~255u;
            u32 result = entry->attach(next, bytes);
            success = result != 0 && success != 0;
            uniforms = mUniformCount;
            next += aligned;
        }
    }
    if (success) mFlags = (u16)mFlags | 1;
    return success;
}



/* 027DAFB4 */
u32 agl::gx2util::PaletteBuffer::initialize(PoseBuilder* builder, void* storage, u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (builder->mLayout.mBlocks == NULL) builder->mLayout.calculate(builder);
    if ((u32)builder->mLayout.mTotal > size) return 0;
    PoseResource* resource = builder->mResource;
    mResource = reinterpret_cast<PaletteResource*>(resource);
    mFlags = (u32)resource->mFlags | 0x16;
    mViewCount = builder->mFlags;
    mBones = resource->mBones.get();
    void* local = uniformBlockAddress(builder->mLayout.mBlocks.get() + 1, storage);
    mLocal = local;
    mLocalBase = local;
    void* world = uniformBlockAddress(builder->mLayout.mBlocks.get(), storage);
    mWorld = world;
    mWorldBase = world;
    mScale = uniformBlockAddress(builder->mLayout.mBlocks.get() + 2, storage);
    u32 bones = resource->mBoneCount;
    mCallback = NULL;
    mPaletteOverride = NULL;
    mCallbackBone = 0xFFFF;
    mWorkStorage = storage;
    mReserved40 = 0;
    mBoneCount = bones;
    mStorage = NULL;
    clearGraphicsWords(&mPalette, 5);
    resetValues();
    return 1;
}

struct PoseDispatch {
    be<s16> mAdjustment; be<s16> mIndex;
    union { be<u32> mFunction; struct { be<u16> mHigh; be<s16> mVtableOffset; }; };
};
struct PoseDispatchTable { PoseDispatch mEntries[4]; };
static PoseDispatchTable sPoseDefaultDispatch;
static PoseDispatchTable sPoseCallbackDispatch;

/* 027DB1D4 */
void agl::gx2util::PaletteBuffer::update(void* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* world = mWorld;
    u32 flags = mFlags;
    void* base = mWorldBase;
    u32 mode = (flags >> 8) & 3;
    mFlags = flags | 0x14;
    if (world == base) zeroGraphicsRange(world, (u32)mBoneCount * 48);
    PoseDispatch* dispatch = mCallback == NULL ? sPoseDefaultDispatch.mEntries + mode : sPoseCallbackDispatch.mEntries + mode;
    s32 index = dispatch->mIndex;
    s32 adjustment = dispatch->mAdjustment;
    u8* object = reinterpret_cast<u8*>(this) + adjustment;
    u32 function;
    if (index < 0) function = dispatch->mFunction;
    else {
        s32 offset = dispatch->mVtableOffset;
        be<u32>* table = *reinterpret_cast<gptr<be<u32>>*>(object + offset);
        function = table[(u32)index * 2 + 1];
    }
    gabi::call_ptr<void>(function, object, context);
}

static void multiplyPaletteMatrix(MatrixUtility* output, const MatrixUtility* world, const MatrixUtility* inverse) {
    f32 bind[12];
    auto source = reinterpret_cast<const be<f32>*>(inverse);
    for (u32 i = 0; i < 12; ++i) bind[i] = source[i];
    for (u32 row = 0; row < 3; ++row) {
        auto input = reinterpret_cast<const be<f32>*>(world) + row * 4;
        f32 x = input[0]; f32 y = input[1]; f32 z = input[2]; f32 w = input[3];
        f32 a = gabi::fmadds(bind[8], z, gabi::fmadds(bind[4], y, gabi::fmuls_ppc(bind[0], x)));
        f32 b = gabi::fmadds(bind[9], z, gabi::fmadds(bind[5], y, gabi::fmuls_ppc(bind[1], x)));
        f32 c = gabi::fmadds(bind[10], z, gabi::fmadds(bind[6], y, gabi::fmuls_ppc(bind[2], x)));
        f32 d = gabi::fmadds(bind[11], z, gabi::fmadds(bind[7], y, gabi::fmuls_ppc(bind[3], x)));
        output->mRows[row].x.mValue = a;
        output->mRows[row].y.mValue = b;
        output->mRows[row].z.mValue = gabi::fmadds(0.0f, w, c);
        output->mRows[row].w.mValue = gabi::fmadds(1.0f, w, d);
    }
}

/* 027DBC2C */
void agl::gx2util::PaletteBuffer::updatePalette(u32 view) {
    gabi::AutoFrame frame{gabi::current_function()};
    if ((u32)mPalette.mSliceSize == 0) return;
    u32 flags = (u32)mFlags | 0x10;
    PaletteResource* resource = mResource;
    mFlags = flags;
    be<s16>* indices = resource->mIndices.get();
    MatrixUtility* inverse = resource->mInverseBind.get();
    auto destination = static_cast<be<u32>*>(mPaletteOverride.get());
    if (destination == NULL) {
        destination = static_cast<be<u32>*>(mPalette.slice(view));
        u32 bytes = mPalette.mSliceSize;
        if (isGraphicsRangeValid(destination, bytes)) zeroGraphicsRange(destination, bytes);
        flags = mFlags;
        resource = mResource;
    }
    bool reversed = (flags & 2) != 0;
    u32 smooth = resource->mSmoothCount;
    gabi::FrameLocal<MatrixUtility> temporaryStorage(0x8);
    auto& temporary = *temporaryStorage;
    for (u32 index = 0; index < smooth; ++index) {
        s32 bone = indices[index];
        auto world = static_cast<MatrixUtility*>(mWorld.get()) + bone;
        auto target = reinterpret_cast<MatrixUtility*>(destination + index * 12);
        if (reversed) {
            multiplyPaletteMatrix(&temporary, world, inverse + index);
            const be<u32>* words = reinterpret_cast<const be<u32>*>(&temporary);
            for (u32 word = 0; word < 12; ++word) destination[index * 12 + word] = reverseGraphicsWord(words[word]);
        } else multiplyPaletteMatrix(target, world, inverse + index);
    }
    if (smooth != 0) resource = mResource;
    u32 total = (u32)resource->mSmoothCount + (u32)resource->mRigidCount;
    for (u32 index = smooth; index < total; ++index) {
        s32 bone = indices[index];
        auto world = static_cast<MatrixUtility*>(mWorld.get()) + bone;
        const be<u32>* words = reinterpret_cast<const be<u32>*>(world);
        for (u32 word = 0; word < 12; ++word) {
            u32 value = words[word];
            destination[index * 12 + word] = reversed ? reverseGraphicsWord(value) : value;
        }
    }
    if (mPaletteOverride == NULL) mPalette.flush(view);
}

extern void buildScaledPaletteMatrix(u32, MatrixUtility*, const MatrixUtility*, const MatrixUtility*, void*);
static void inversePaletteMatrix(MatrixUtility* output, const MatrixUtility* input) {
    const be<f32>* m = reinterpret_cast<const be<f32>*>(input);
    f32 a=m[0], b=m[1], c=m[2], d=m[4], e=m[5], f=m[6], g=m[8], h=m[9], i=m[10];
    f32 aa = gabi::fmsubs(e, i, gabi::fmuls_ppc(h, f));
    f32 ab = -gabi::fmsubs(b, i, gabi::fmuls_ppc(h, c));
    f32 ac = gabi::fmsubs(b, f, gabi::fmuls_ppc(e, c));
    f32 crossFG = gabi::fmuls_ppc(f, g);
    f32 crossDI = gabi::fmuls_ppc(d, i);
    f32 determinantMinor = gabi::fsubs_ppc(crossFG, crossDI);
    f32 ba = -gabi::fsubs_ppc(crossDI, crossFG);
    f32 bb = gabi::fmsubs(a, i, gabi::fmuls_ppc(g, c));
    f32 bc = -gabi::fmsubs(a, f, gabi::fmuls_ppc(d, c));
    f32 ca = gabi::fmsubs(d, h, gabi::fmuls_ppc(g, e));
    f32 cb = -gabi::fmsubs(a, h, gabi::fmuls_ppc(g, b));
    f32 cc = gabi::fmsubs(a, e, gabi::fmuls_ppc(d, b));
    f32 determinant = gabi::fmadds(c, ca, gabi::fmadds(a, aa, gabi::fmuls_ppc(b, determinantMinor)));
    f32 tx=m[3], ty=m[7], tz=m[11];
    f32 ax = -gabi::fmadds(ac, tz, gabi::fmadds(aa, tx, gabi::fmuls_ppc(ab, ty)));
    f32 bx = -gabi::fmadds(bc, tz, gabi::fmadds(ba, tx, gabi::fmuls_ppc(bb, ty)));
    f32 cx = -gabi::fmadds(cc, tz, gabi::fmadds(ca, tx, gabi::fmuls_ppc(cb, ty)));
    f32 estimate = gabi::fres(determinant);
    f32 product = gabi::fmuls_ppc(determinant, estimate);
    f32 inverse = gabi::fnmsubs(product, estimate, gabi::fadds_ppc(estimate, estimate));
    auto target = reinterpret_cast<be<f32>*>(output);
    target[0]=gabi::fmuls_ppc(aa, inverse); target[1]=gabi::fmuls_ppc(ab, inverse); target[2]=gabi::fmuls_ppc(ac, inverse); target[3]=gabi::fmuls_ppc(ax, inverse);
    target[4]=gabi::fmuls_ppc(ba, inverse); target[5]=gabi::fmuls_ppc(bb, inverse); target[6]=gabi::fmuls_ppc(bc, inverse); target[7]=gabi::fmuls_ppc(bx, inverse);
    target[8]=gabi::fmuls_ppc(ca, inverse); target[9]=gabi::fmuls_ppc(cb, inverse); target[10]=gabi::fmuls_ppc(cc, inverse); target[11]=gabi::fmuls_ppc(cx, inverse);
}

/* 027DB590 */
void agl::gx2util::PaletteBuffer::adjustBoneMatrix(MatrixUtility* output, const MatrixUtility* input, u32 bone, u32 retainWorld) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 flags = mFlags;
    auto world = sead::elementPtr(static_cast<MatrixUtility*>(mWorld.get()), bone); /* guest EA arithmetic: wraps at 32 bits as on the console */
    auto local = sead::elementPtr(static_cast<const PoseBoneDescriptor*>(mBones.get()), bone);
    mFlags = flags | 0x10;
    gabi::FrameLocal<MatrixUtility> composedStorage(0x38);
    auto& composed = *composedStorage;
    multiplyPaletteMatrix(&composed, input, world);
    u32 boneFlags = local->mFlags;
    auto bases = &sead::elementAt(static_cast<PoseLocal*>(mLocal.get()), bone).mMatrix;
    buildScaledPaletteMatrix(boneFlags & 0x70000, &composed, input, world, bases);
    u32 updated = mFlags;
    mFlags = updated | ((boneFlags & 0x60000) == 0x40000 ? 0x14 : 0x10);
    gabi::FrameLocal<MatrixUtility> inverseStorage(0x8);
    auto& inverse = *inverseStorage;
    inversePaletteMatrix(&inverse, input);
    multiplyPaletteMatrix(output, &inverse, &composed);
    if (retainWorld == 0) {
        inversePaletteMatrix(&inverse, world);
        multiplyPaletteMatrix(output, output, &inverse);
    }
}

/* WWHD glue
 ext 027DB320 void buildScaledPaletteMatrix(u32, agl::gx2util::MatrixUtility*, const agl::gx2util::MatrixUtility*, const agl::gx2util::MatrixUtility*, void*)
 data sPoseDefaultDispatch 1016C6A0
 data sPoseCallbackDispatch 1016C6C0
 ext C000A848 void* copyUniformBytes(void*, const void*, u32)
 ext C000A858 void* fillUniformBytes(void*, u32, u32)
 ext 027E64B4 void updateUniformReversed(agl::gx2util::UniformBuffer*, void*, be<u32>*)
 ext 027E66B4 void updateUniformNative(agl::gx2util::UniformBuffer*, void*, be<u32>*)
 ext C00088A8 void invalidateGraphicsRange(void*, u32)
 ext C00088A0 void flushGraphicsRange(void*, u32)
 ext C0009DB0 u32 queryGraphicsAddressRange(void*, u32)
 ext C00088C8 void zeroGraphicsRange(void*, u32)
*/

/* agl SSAO: construction and destruction of the effect and its target sets, the resource preparation, the
 * reprojection transforms and the target-set surfaces. */
// GX2 surface values: wut facts.
#include "agl/SsaoTargetSet.h"
#include "agl/ParameterConstruction.h"
using namespace agl;
// The dynamic texture cache singleton (pointer word owned by agl/DynamicTextureCache.cpp).
static TextureCacheWord sTextureCache;
void freeSsaoObject(void* storage);
void* allocateSsaoObject(u32 size);
/* 0279313C */
void agl::SsaoTargetSet::destroy(SsaoTargetSet* set, u32 flags) {
    gabi::AutoFrame frame{0x0279313C};
    if (set != NULL) {
        TextureCacheEntry* history = set->mHistoryTexture;
        if (history != NULL) {
            sTextureCache->releaseShared(history);
            set->mHistoryTexture = NULL;
        }
        TextureSampler::destroy(&set->mFinalSampler, 2);
        TextureSampler::destroy(&set->mSecondarySampler, 2);
        set->mSecondaryFrame.destroy(2);
        TextureSampler::destroy(&set->mPrimarySamplers[1], 2);
        TextureSampler::destroy(&set->mPrimarySamplers[0], 2);
        set->mPrimaryFrame.destroy(2);
        if ((flags & 1) != 0) freeSsaoObject(set);
    }
}
/* 02793384 */
agl::SsaoTargetSet* agl::SsaoTargetSet::construct(SsaoTargetSet* set) {
    gabi::AutoFrame frame{0x02793384};
    if (set == NULL) set = (SsaoTargetSet*)allocateSsaoObject(sizeof(SsaoTargetSet));
    if (set != NULL) {
        set->mHistoryTexture = NULL;
        set->mWorkTexture = NULL;
        set->mPrimaryFrame.construct();
        ColorSurface* primaryColor = &set->mPrimaryColor;
        primaryColor->construct();
        TextureSampler::create(&set->mPrimarySamplers[0]);
        TextureSampler::create(&set->mPrimarySamplers[1]);
        set->mPrimaryPassTexture = NULL;
        set->mSecondaryFrame.construct();
        set->mSecondaryColor.construct();
        TextureSampler::create(&set->mSecondarySampler);
        set->mSecondaryPassTexture = NULL;
        TextureSampler::create(&set->mFinalSampler);
        set->mExternalFrame = NULL;
        set->mSampleIndex = 0;
        // GHS member null guard of the embedded transform (allocates when set + 0x988 wraps to 0).
        u32 transformAddress = gabi::ea(set) + (u32)offsetof(SsaoTargetSet, mPreviousViewProjection);
        if (transformAddress == 0) allocateSsaoObject(sizeof(set->mPreviousViewProjection));
        set->mHistoryValid = 0;
        set->mPrimaryFrame.mColors[0] = primaryColor;
        set->mSecondarySampler.overrideEnabled = 1;
        set->mPrimaryFrame.mDepth = NULL;
        set->mPrimarySamplers[0].overrideEnabled = 1;
    }
    return set;
}
void initializeSsaoSampleData(agl::SsaoEffect*);
void initializeSsaoKernel(agl::SsaoEffect*, u32);
/* 02795DA4 */
void agl::initializeSsaoSampling(SsaoEffect* effect) {
    gabi::AutoFrame frame{0x02795DA4};
    initializeSsaoSampleData(effect);
    initializeSsaoKernel(effect, 128);
}
// Vtables this source stores: the effect's two bases, its parameter object, and the resource record's two bases.
static u8 sSsaoEffectMethods;
static u8 sSsaoRenderMethods;
static u8 sSsaoParameterObjectMethods;
static u8 sResourceRecordMethods;
static u8 sResourceRecordInterfaceMethods;
void freeSsaoArray(void* storage);
void destructSsaoArray(void* storage, u32 count, u32 size, u32 destructor, u32 unused0, u32 unused1);
/* 027931EC */
void agl::SsaoEffect::destroy(SsaoEffect* effect, u32 flags) {
    gabi::AutoFrame frame{0x027931EC};
    if (effect != NULL) {
        SsaoTargetSet* sets = effect->mTargetSets;
        effect->mRoot.mTree.mTable = gabi::ea(&sSsaoEffectMethods);
        effect->mRenderMethods = &sSsaoRenderMethods;
        if (sets != NULL) {
            for (s32 index = 0; index < (s32)effect->mTargetSetCount; ++index) {
                SsaoTargetSet::destroy(&sets[index], 2);
                sets = effect->mTargetSets;
            }
            sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager, sets);
            gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap,
                                 (SsaoTargetSet*)effect->mTargetSets);
            effect->mTargetSetCount = 0;
            effect->mTargetSets = NULL;
        }
        for (u32 index = 0; index < 16; ++index) {
            freeSsaoArray(effect->mSampleImages[index]);
            effect->mSampleImages[index] = NULL;
        }
        effect->mResource.detach();
        destructSsaoArray(effect->mSampleSamplers, 16, sizeof(TextureSampler),
                          gabi::guest_fn((const void*)TextureSampler::destroy), 0, 0);
        // ~ResourceRecord, inlined
        effect->mResource.mVtable = (sead::DisposerVtable*)&sResourceRecordMethods;
        effect->mResource.mInterfaceMethods = &sResourceRecordInterfaceMethods;
        effect->mResource.detach();
        effect->mResource.sead::RegisteredObject::destroy(0);
        effect->mParameters.mVtable = (parameter_tree::ValueVtable*)&sSsaoParameterObjectMethods;
        if ((flags & 1) != 0) freeSsaoObject(effect);
    }
}
// The clip-space <-> texture-space transforms this source's static initialiser writes.
static agl::SsaoTransform sClipFromTexture;
static agl::SsaoTransform sTextureFromClip;
/* 02795DDC */
void agl::initializeSsaoTransforms() {
    const f32 unitToClip[4][4] = {
        {2.0f,0.0f,0.0f,-1.0f},
        {0.0f,-2.0f,0.0f,1.0f},
        {0.0f,0.0f,2.0f,-1.0f},
        {0.0f,0.0f,0.0f,1.0f}
    };
    const f32 clipToUnit[4][4] = {
        {0.5f,0.0f,0.0f,0.5f},
        {0.0f,-0.5f,0.0f,0.5f},
        {0.0f,0.0f,0.5f,0.5f},
        {0.0f,0.0f,0.0f,1.0f}
    };
    for (u32 row = 0; row < 4; ++row)
        for (u32 column = 0; column < 4; ++column)
            if (row != 3 || column != 3) sClipFromTexture.mRows[row][column] = unitToClip[row][column];
    for (u32 row = 0; row < 4; ++row)
        for (u32 column = 0; column < 4; ++column)
            if (row != 3 || column != 3) sTextureFromClip.mRows[row][column] = clipToUnit[row][column];
    // Both homogeneous components are written after the other matrix entries.
    sClipFromTexture.mRows[3][3] = 1.0f;
    sTextureFromClip.mRows[3][3] = 1.0f;
}
/* 02795FAC */
// This source's copy of the resource record's deleting destructor.
void agl::destroyResourceRecord(ResourceRecord* record, u32 flags) {
    gabi::AutoFrame frame{0x02795FAC};
    if (record != NULL) {
        record->mVtable = (sead::DisposerVtable*)&sResourceRecordMethods;
        record->mInterfaceMethods = &sResourceRecordInterfaceMethods;
        record->detach();
        record->sead::RegisteredObject::destroy(0);
        if ((flags & 1) != 0) freeSsaoObject(record);
    }
}
// This source's SafeString vtable, its parameter vtables and the parameters' meta texts.
static sead::SafeTextVtable sSsaoTextMethods;
static TextByte sSsaoBooleanMeta;
static TextByte sSsaoNumericMeta;
static TextByte sSsaoMinimumZeroMeta;
static TextByte sSsaoMinimumOneMeta;
static u8 sSsaoOwnedObjectMethods;
static u8 sSsaoBooleanMethods;
static u8 sSsaoScalarMethods;
static u8 sSsaoIntegerMethods;
void constructSsaoArray(void* storage, u32 count, u32 size, u32 constructor);
/* inlined into the SSAO and shadow-map constructors */
agl::ParameterBase* agl::effect_construction::constructParameter(ParameterBase* parameter, u32 size, u32 methods,
        sead::SafeTextVtable* textMethods, const char* name, const char* label, be<u8>* meta,
        parameter_tree::Value* owner, TextSlots slots, bool metaFirst) {
    gabi::FrameLocal<sead::SafeText> nameText(slots.mName);
    gabi::FrameLocal<sead::SafeText> labelText(slots.mLabel);
    gabi::FrameLocal<sead::SafeText> metaText(slots.mMeta);
    nameText->mText.v = gabi::guest_str(name);
    nameText->mVtable = textMethods;
    labelText->mText.v = gabi::guest_str(label);
    labelText->mVtable = textMethods;
    if (metaFirst) {
        metaText->mText = meta;
        metaText->mVtable = textMethods;
    }
    if (gabi::ea(parameter) == 0) parameter = (ParameterBase*)allocateEffectParameter(size);
    if (parameter != NULL) {
        ParameterBase::create(parameter);
        parameter->mMethods.v = methods;
        if (!metaFirst) {
            metaText->mText = meta;
            metaText->mVtable = textMethods;
        }
        parameter->initialize(nameText.get(), labelText.get(), metaText.get(), owner);
    }
    return parameter;
}
// A member array constructed in place (GHS keeps the allocation path for a NULL address).
static void constructMemberArray(void* storage, u32 count, u32 stride, u32 constructor) {
    if (gabi::ea(storage) == 0) storage = allocateSsaoObject(count * stride);
    if (storage != NULL) constructSsaoArray(storage, count, stride, constructor);
}
/* 02792A08 */
agl::SsaoEffect* agl::SsaoEffect::construct(SsaoEffect* effect) {
    gabi::AutoFrame frame{0x02792A08};
    using namespace agl::effect_construction;
    if (effect == NULL) effect = (SsaoEffect*)allocateSsaoObject(sizeof(SsaoEffect));
    if (effect != NULL) {
        gabi::FrameLocal<sead::SafeText> effectName(8);
        effectName->mText.v = gabi::guest_str("aglssao");
        effectName->mVtable = &sSsaoTextMethods;
        ParameterRoot::create(&effect->mRoot, effectName.get(), 0);
        effect->mRoot.mTree.mTable = gabi::ea(&sSsaoEffectMethods);
        effect->mRenderMethods = &sSsaoRenderMethods;
        // GHS member null guard of the embedded count/array pair.
        if (gabi::ea(&effect->mTargetSetCount) == 0) {
            auto* list = (SsaoTargetSetList*)allocateSsaoObject(sizeof(SsaoTargetSetList));
            if (list != NULL) {
                list->mSets = NULL;
                list->mCount = 0;
            }
        } else {
            effect->mTargetSets = NULL;
            effect->mTargetSetCount = 0;
        }
        // The parameter object of this source derives from the tree value; agl::ParameterObject has its layout.
        ParameterObject::create(reinterpret_cast<agl::ParameterObject*>(&effect->mParameters));
        effect->mParameters.mOwner = &effect->mRoot.mTree;
        effect->mParameters.mVtable = (parameter_tree::ValueVtable*)&sSsaoOwnedObjectMethods;
        u32 base = gabi::ea(effect);
        parameter_tree::Value* parent = &effect->mParameters;
        u32 booleanMethods = gabi::ea(&sSsaoBooleanMethods);
        u32 scalarMethods = gabi::ea(&sSsaoScalarMethods);
        u32 integerMethods = gabi::ea(&sSsaoIntegerMethods);
        auto* enabled = (ssao_parameters::BooleanParameter*)constructParameter(
            &effect->mEnabled,sizeof(ssao_parameters::BooleanParameter),booleanMethods,&sSsaoTextMethods,
            "is_enable","有効",&sSsaoBooleanMeta,parent,{0x10,0x18,0xF0});
        if (enabled != NULL) enabled->mValue = 1;
        auto* radius = (ssao_parameters::ScalarParameter*)constructParameter(
            &effect->mRadius,sizeof(ssao_parameters::ScalarParameter),scalarMethods,&sSsaoTextMethods,
            "radius","半径",&sSsaoNumericMeta,parent,{0x20,0x28,0xF8});
        if (radius != NULL) radius->mValue = 0.05f;
        auto* farDistance = (ssao_parameters::ScalarParameter*)constructParameter(
            &effect->mFarDistance,sizeof(ssao_parameters::ScalarParameter),scalarMethods,&sSsaoTextMethods,
            "ao_far","AOの半径が0になる距離",&sSsaoNumericMeta,parent,{0x30,0x38,0x100});
        if (farDistance != NULL) farDistance->mValue = 400.0f;
        auto* attenuationDistance = (ssao_parameters::ScalarParameter*)constructParameter(
            &effect->mAttenuationDistance,sizeof(ssao_parameters::ScalarParameter),scalarMethods,&sSsaoTextMethods,
            "dist_attn","遮蔽計算が影響する最大距離",&sSsaoNumericMeta,parent,{0x40,0x48,0x108});
        if (attenuationDistance != NULL) attenuationDistance->mValue = 40.0f;
        auto* distanceAttenuation = (ssao_parameters::BooleanParameter*)constructParameter(
            &effect->mDistanceAttenuation,sizeof(ssao_parameters::BooleanParameter),booleanMethods,&sSsaoTextMethods,
            "enable_dist_attn","距離による遮蔽計算の減衰",&sSsaoBooleanMeta,parent,{0x50,0x58,0x110});
        if (distanceAttenuation != NULL) distanceAttenuation->mValue = 1;
        auto* reprojection = (ssao_parameters::BooleanParameter*)constructParameter(
            &effect->mReprojection,sizeof(ssao_parameters::BooleanParameter),booleanMethods,&sSsaoTextMethods,
            "enable_reprojection","前フレームの情報を使う",&sSsaoBooleanMeta,parent,{0x60,0x68,0x118});
        if (reprojection != NULL) reprojection->mValue = 1;
        auto* mixRate = (ssao_parameters::ScalarParameter*)constructParameter(
            &effect->mMixRate,sizeof(ssao_parameters::ScalarParameter),scalarMethods,&sSsaoTextMethods,
            "mix_rate","前のフレームの割合",&sSsaoNumericMeta,parent,{0x70,0x78,0x120});
        if (mixRate != NULL) mixRate->mValue = 0.8f;
        auto* density = (ssao_parameters::ScalarParameter*)constructParameter(
            &effect->mDensity,sizeof(ssao_parameters::ScalarParameter),scalarMethods,&sSsaoTextMethods,
            "density","濃さ調整",&sSsaoNumericMeta,parent,{0x80,0x88,0x128});
        if (density != NULL) density->mValue = 1.0f;
        effect->mCenterWeight = 0.0f;
        // GHS member null guards of the embedded arrays.
        if (base + (u32)offsetof(SsaoEffect,mKernel) == 0)
            allocateSsaoObject(sizeof(effect->mKernel));
        if (base + (u32)offsetof(SsaoEffect,mKernelDepths) == 0)
            allocateSsaoObject(sizeof(effect->mKernelDepths));
        ResourceRecord::create(&effect->mResource);
        effect->mReductionLevel = 1;
        effect->mFilterLevel = 3;
        effect->mFilterEnabled = 0;
        constructMemberArray(&effect->mSampleTextures[0],16,sizeof(TextureData),
                             gabi::guest_fn((const void*)TextureData::create));
        constructMemberArray(&effect->mSampleSamplers[0],16,sizeof(TextureSampler),
                             gabi::guest_fn((const void*)TextureSampler::create));
        if (gabi::ea(&effect->mSampleImages[0]) == 0) allocateSsaoObject(sizeof(effect->mSampleImages));
        auto* rotateSamples = (ssao_parameters::BooleanParameter*)constructParameter(
            &effect->mRotateSamples,sizeof(ssao_parameters::BooleanParameter),booleanMethods,&sSsaoTextMethods,
            "enable_rotate","サンプル点の回転",&sSsaoBooleanMeta,parent,{0x90,0x98,0x130});
        if (rotateSamples != NULL) rotateSamples->mValue = 1;
        auto* blurCount = (ssao_parameters::IntegerParameter*)constructParameter(
            &effect->mBlurCount,sizeof(ssao_parameters::IntegerParameter),integerMethods,&sSsaoTextMethods,
            "blur_num","ぼかし回数",&sSsaoMinimumZeroMeta,parent,{0xA0,0xA8,0xB0},true);
        if (blurCount != NULL) blurCount->mValue = 1;
        auto* depthOffset = (ssao_parameters::ScalarParameter*)constructParameter(
            &effect->mDepthOffset,sizeof(ssao_parameters::ScalarParameter),scalarMethods,&sSsaoTextMethods,
            "depth_offset","基準深度オフセット",&sSsaoMinimumZeroMeta,parent,{0xB8,0xC0,0xC8},true);
        if (depthOffset != NULL) depthOffset->mValue = 0.0001f;
        auto* samplePairs = (ssao_parameters::IntegerParameter*)constructParameter(
            &effect->mSamplePairCount,sizeof(ssao_parameters::IntegerParameter),integerMethods,&sSsaoTextMethods,
            "sample_pair_num","サンプルペアの数",&sSsaoMinimumOneMeta,parent,{0xD0,0xD8,0xE0},true);
        if (samplePairs != NULL) samplePairs->mValue = 3;
        gabi::FrameLocal<sead::SafeText> objectName(0xE8);
        objectName->mText.v = gabi::guest_str("ssao");
        objectName->mVtable = &sSsaoTextMethods;
        effect->mRoot.mTree.appendValue(parent, objectName.get());
        for (u32 index = 0; index < 16; ++index)
            effect->mSampleImages[index] = NULL;
    }
    return effect;
}
// Names of the target set's two textures.
static TextByte sSsaoHistoryName;
static TextByte sSsaoWorkName;
/* 02794068 */
void agl::SsaoTargetSet::prepareSurfaces(u32 format,s32 width,s32 height) {
    gabi::AutoFrame frame{0x02794068};
    struct IntToFloatSlot { be<u32> mHigh, mLow; };
    gabi::FrameLocal<IntToFloatSlot> dimensionConversion(0x20);
    dimensionConversion->mLow = (u32)width ^ 0x80000000u;
    dimensionConversion->mHigh = 0x43300000u;
    auto readDimension = [&]() -> f64 {
        u64 bits = ((u64)(u32)dimensionConversion->mHigh << 32) | (u32)dimensionConversion->mLow;
        f64 encoded;
        __builtin_memcpy(&encoded,&bits,sizeof(encoded));
        return encoded;
    };
    f64 encodedWidth = readDimension();
    dimensionConversion->mLow = (u32)height ^ 0x80000000u;
    f64 encodedHeight = readDimension();
    f32 floatWidth = (f32)(encodedWidth - 4503601774854144.0);
    f32 floatHeight = (f32)(encodedHeight - 4503601774854144.0);
    DynamicTextureCache* cache = sTextureCache;
    struct Rect { be<f32> mMinimumX,mMinimumY,mMaximumX,mMaximumY; };
    gabi::FrameLocal<Rect> rectangle(0x10);
    if (0.0f < floatWidth) {
        rectangle->mMaximumX = floatWidth;
        rectangle->mMinimumX = 0.0f;
    } else {
        rectangle->mMaximumX = 0.0f;
        rectangle->mMinimumX = floatWidth;
    }
    if (!(0.0f < floatHeight)) {
        f32 minimumX = rectangle->mMinimumX;
        rectangle->mMaximumY = 0.0f;
        mPrimaryFrame.mBase.mArea.mMinX = minimumX;
        f32 maximumX = rectangle->mMaximumX;
        mPrimaryFrame.mBase.mArea.mMinY = floatHeight;
        mPrimaryFrame.mBase.mArea.mMaxX = maximumX;
        rectangle->mMinimumY = floatHeight;
        mPrimaryFrame.mBase.mArea.mMaxY = 0.0f;
    } else {
        mPrimaryFrame.mBase.mArea.mMinY = 0.0f;
        f32 minimumX = rectangle->mMinimumX;
        f32 maximumX = rectangle->mMaximumX;
        mPrimaryFrame.mBase.mArea.mMinX = minimumX;
        mPrimaryFrame.mBase.mArea.mMaxX = maximumX;
        rectangle->mMaximumY = floatHeight;
        mPrimaryFrame.mBase.mArea.mMaxY = floatHeight;
        rectangle->mMinimumY = 0.0f;
    }
    // Primary sampler: clamp modes 1/1/0.
    u32 dirty = mPrimarySamplers[0].dirty;
    TextureCacheEntry* history = mHistoryTexture;
    mPrimaryFrame.mBase.mSize.mHeight = floatHeight;
    mPrimarySamplers[0].settings.modes[0] = 1;
    mPrimarySamplers[0].settings.modes[1] = 1;
    mPrimarySamplers[0].dirty = dirty | 4;
    mPrimaryFrame.mBase.mSize.mWidth = floatWidth;
    mPrimarySamplers[0].settings.modes[2] = 0;
    if (history == NULL) {
        gabi::FrameLocal<sead::SafeText> name(0x28);
        name->mVtable = &sSsaoTextMethods;
        name->mText = &sSsaoHistoryName;
        mHistoryTexture = cache->acquireShared2D(name.get(),format,width,height,1,0,1,1,0);
        mHistoryValid = 1;
    } else {
        // The history only survives a frame of the same size and format (the larger of the surface size and the
        // texture's minimum size, as TextureData reports it).
        u32 storedWidth = history->minimumWidth;
        s32 surfaceWidth = (u32)history->surface.mWidth;
        if (surfaceWidth > (s32)storedWidth) storedWidth = (u32)surfaceWidth;
        bool matches = storedWidth == (u32)width;
        if (matches) {
            u32 storedHeight = history->minimumHeight;
            s32 surfaceHeight = (u32)history->surface.mHeight;
            if (surfaceHeight > (s32)storedHeight) storedHeight = (u32)surfaceHeight;
            matches = storedHeight == (u32)height;
            if (matches) matches = (u32)history->format == format;
        }
        if (!matches) mHistoryValid = 0;
    }
    if (mWorkTexture.get() == NULL) {
        gabi::FrameLocal<sead::SafeText> name(0x30);
        name->mVtable = &sSsaoTextMethods;
        name->mText = &sSsaoWorkName;
        mWorkTexture = cache->acquire2D(name.get(),format,width,height,1,0,1,1,0);
    }
}
sead::RegistryHeap* currentHeap(sead::RegistryManagerState* manager);
void* allocateSsaoImage(u32 size, sead::RegistryHeap* heap, s32 alignment);
static TextByte sSsaoResourceName;
// The resource record's empty attach hook, shared by identical-code folding (merged as TextureReference::notify).
void attachResourceRecord(ResourceRecord* record, s32 count, sead::SafeText* name, sead::RegistryHeap* heap);
/* 02793E88 */
void agl::SsaoEffect::prepareResources(s32 targetCount,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{0x02793E88};
    if (targetCount > 0) {
        sead::RegistryHeap* allocationHeap = heap;
        if (allocationHeap == NULL) allocationHeap = currentHeap(sRegistryManager);
        u32 bytes = (u32)targetCount * sizeof(SsaoTargetSet);
        void* storage = gabi::call_ptr<void*>(allocationHeap->dispatch()->mAllocate.mFunction,allocationHeap,bytes,(s32)4);
        u32 remaining = (u32)targetCount;
        auto* sets = static_cast<SsaoTargetSet*>(storage);
        u32 index = 0;
        do {
            if (sets != NULL) SsaoTargetSet::construct(&sets[index]);
            // A failed allocation still constructs at NULL + index * stride (index 0 allocates its own).
            else if (index != 0) SsaoTargetSet::construct(gabi::at<SsaoTargetSet>(index * sizeof(SsaoTargetSet)));
            ++index;
        } while (--remaining != 0);
        if (storage != NULL) {
            mTargetSets = (SsaoTargetSet*)storage;
            mTargetSetCount = targetCount;
        }
    }
    // Sixteen 8x8 sample textures (dimension 2D, format 0xA) and their images.
    for (u32 index = 0; index < 16; ++index) {
        TextureData* texture = &mSampleTextures[index];
        texture->configure(1,0xA,8,8,1,1,0,0);
        u32 size = texture->surface.mImageBytes;
        s32 alignment = texture->surface.mAlignment;
        void* image = allocateSsaoImage(size,heap,alignment);
        mSampleImages[index] = image;
    }
    initializeSsaoSampleData(this);
    initializeSsaoKernel(this,128);
    gabi::FrameLocal<sead::SafeText> label(0xC);
    label->mVtable = &sSsaoTextMethods;
    label->mText = &sSsaoResourceName;
    attachResourceRecord(&mResource,targetCount,label.get(),heap);
}
/* WWHD glue
str 10166760
str 101667CC
str 10166780
str 10166788
str 10166790
str 10166798
str 101667D8
str 101667F0
str 1016680C
str 10166828
str 1016683C
str 101667B8
str 10166858
str 10166870
str 1016687C
str 10166768
str 10166890
str 101667FC
str 1016689C
str 101668B0
str 101668BC
str 101668C8
str 101668D8
str 101667A8
str 101668EC
str 101667A0
data sTextureCache 101F8DD4
data sSsaoTextMethods 1016650C
data sSsaoBooleanMeta 10166508
data sSsaoNumericMeta 10166509
data sSsaoMinimumZeroMeta 10166770
data sSsaoMinimumOneMeta 10166778
data sSsaoOwnedObjectMethods 101665A4
data sSsaoBooleanMethods 101665E4
data sSsaoScalarMethods 101666D4
data sSsaoIntegerMethods 1016665C
data sClipFromTexture 104A1F90
data sTextureFromClip 104A1FD0
data sSsaoEffectMethods 101669BC
data sSsaoRenderMethods 101669AC
data sResourceRecordMethods 10166584
data sResourceRecordInterfaceMethods 10166594
data sSsaoParameterObjectMethods 10166544
data sSsaoHistoryName 10166944
data sSsaoWorkName 1016693C
data sSsaoResourceName 10166934
ext 0273AD10 void* allocateSsaoObject(u32)
ext 0273AF40 void freeSsaoObject(void*)
ext 0273AFC8 void freeSsaoArray(void*)
ext 028EFFD0 void constructSsaoArray(void*,u32,u32,u32)
ext 028F0164 void destructSsaoArray(void*,u32,u32,u32,u32,u32)
ext 02756140 sead::RegistryHeap* currentHeap(sead::RegistryManagerState*)
ext 0273B0D4 void* allocateSsaoImage(u32,sead::RegistryHeap*,s32)
ext 027A063C void attachResourceRecord(agl::ResourceRecord*,s32,sead::SafeText*,sead::RegistryHeap*)
*/

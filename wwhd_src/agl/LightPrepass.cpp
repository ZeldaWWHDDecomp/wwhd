/* agl light prepass: construction and configuration, the per-frame update that pairs coincident point lights, the
 * per-view uniform upload, target preparation and the draw pass over the four light collections, and the specular
 * curve texture.
 * References: WWHD's own instructions, strings and data only. */
#include "agl/LightPrepass.h"
#include "agl/GraphicsAccess.h"
#include "agl/BitmapTransfer.h"
#include "sead/GraphicsRuntime.h"

// Light-collection interface runtime types (lazily initialised, compared by address), borrowed with no initializer:
// AGL#18/#19 share them; their owner is sead/RuntimeStatics.cpp once those units merge.
static u32 sPointExTypeGuard_l;
static sead::RuntimeType sPointExType_l;
static u32 sPointExTypeMethods;
static u32 sSelectionExTypeGuard_l;
static sead::RuntimeType sSelectionExType_l;
static u32 sSelectionExTypeMethods;
static u32 sCurveExTypeGuard_l;
static sead::RuntimeType sCurveExType_l;
static u32 sCurveExTypeMethods;
// The four light collections' base and derived method tables (configure installs both, the derived one last).
static u32 sPointCollectionBaseMethods;
static u32 sPointCollectionMethods;
static u32 sDirectedCollectionBaseMethods;
static u32 sDirectedCollectionMethods;
static u32 sSpatialCollectionBaseMethods;
static u32 sSpatialCollectionMethods;
static u32 sPairedCollectionBaseMethods;
static u32 sPairedCollectionMethods;
static agl::SpecularKeys sSpecularKeys;
static u32 sLightPrepassMethods;
static u32 sCurveParameterMethods_l;
static u32 sPropertyDisposerMethods_l;
static u32 sPropertyObjectMethods_l;

// The primitive-texture resources (AGL#14's GraphicsAccess export) hold the fallback light buffer at +0x34.
struct PrimitiveTextureResources_l { u8 mPrefix[0x34]; gptr<agl::TextureData> mFallbackSurface; };
WWHD_OFFSET(PrimitiveTextureResources_l, mFallbackSurface, 0x34);

// Callees of other units (glue block): names follow their owners where those are known. The uniform-buffer,
// heap-registry and facade-release bindings shared with ParameterCollections.cpp are bound there.
void* allocateLightStorage(u32 size);
void deleteLightStorage(void* storage);
void deleteLightArray(void* storage);
void* allocateFromHeap_l(u32 size, sead::RegistryHeap* heap, s32 alignment);
void* allocateAlignedFromHeap_l(u32 size, sead::RegistryHeap* heap, s32 alignment);
void clearBytes(void* storage, u32 size);
void constructArray(void* storage, u32 count, u32 stride, u32 constructor);
void createViewCamera_l(agl::LightViewCamera_l* camera);
void setViewCamera_l(agl::LightView* view, const void* camera, const void* viewport);
void setViewProjection_l(agl::LightView* view, void* projection);
void createPostprocess_l(void* state);
void destroyPostprocess_l(void* state, u32 flags);
void initializeFacade_l(agl::CollectionFacade_l* facade);
void configureFacade_l(agl::CollectionFacade_l* facade, u32 count, const sead::SafeText* name, sead::RegistryHeap* heap);
void releaseCollectionFacade_l(void* facade);
void initializeParameter_l(agl::CurveParameter_l* parameter, const sead::SafeText* key, const sead::SafeText* description,
                           u32 options);
void setParameterAnnotation_l(void* owner, const sead::SafeText* annotation);
void finishLightResources_l();
f64 evaluateCurve_l(agl::CurveParameter_l* curve, u32 index, f64 position);
void clearRenderBuffer_l(agl::RenderBuffer* target, u32 colorIndex, u32 flags, const agl::LightColorBits* color,
                         u32 stencil, f64 depth);
agl::TextureData* acquireTexture_l(void* resources, const sead::SafeText* name, u32 format, u32 width, u32 height,
                                     u32 slices, u32 mipCount, u32 tileMode, u32 reserved, u32 samples, u32 usage);
void releaseTexture_l(void* resources, agl::TextureData* texture);
void attachColorTexture_l(agl::ColorSurface* color, agl::TextureData* texture, u32 mip, u32 slice);
void attachDepthTexture_l(agl::ColorSurface* color, agl::TextureData* texture);
void copyRegistryMatrix_l(const sead::RegistryMatrix34* source, sead::RegistryMatrix34* destination);
void normalizeDirection_l(be<f32>* direction);

// inlined into the light setters: GHS copies the colour words as integers (bit-exact, signalling NaNs kept)
static void storeFloatBits(be<f32>* destination, u32 bits) {
    gabi::store<u32>(gabi::ea(destination), bits);
}
// inlined into create: render-state copies move the float words as integers
static void copyFloatBits(be<f32>* destination, const be<f32>* source) {
    storeFloatBits(destination, *(const be<u32>*)source);
}
// stfs of a full incoming FPR truncates it rather than rounding it first as frsp would
static f32 storedSingle(f64 value) {
    return gabi::f32_from_bits(ppc_stfs_bits(value));
}
// inlined into updateView: the view's slot of the uniform buffer ring becomes the upload target
static void selectUniformSlot(agl::UniformBuffer* uniform, u32 slot) {
    u32 stride = uniform->stride;
    uniform->cpuData.v = uniform->gpuData.v + stride * slot;
}
// The native texture's surface record (the head of the sampler's native texture).
static sead::GraphicsSurface* nativeSurface(agl::TextureSampler* sampler) {
    return (sead::GraphicsSurface*)sampler->nativeTexture;
}
// inlined into the texture updates: the surface fields a texture is compared on before it is rebuilt
static bool sameSurfaceShape(const agl::TextureData* texture, const agl::TextureData* source) {
    const sead::GraphicsSurface& a = texture->surface;
    const sead::GraphicsSurface& b = source->surface;
    if (a.mDimension != b.mDimension) return false;
    if (a.mWidth != b.mWidth) return false;
    if (a.mHeight != b.mHeight) return false;
    if (a.mDepth != b.mDepth) return false;
    if (a.mMipCount != b.mMipCount) return false;
    if (a.mFormat != b.mFormat) return false;
    if (a.mSwizzle != b.mSwizzle) return false;
    if (a.mTileMode != b.mTileMode) return false;
    return a.mSamples == b.mSamples;
}
// inlined into the texture updates: the same shape only takes the new image pointers
static void bindSurfaceImages(agl::TextureSampler* sampler, const agl::TextureData* source) {
    void* image = source->surface.mImage;
    void* mipmaps = source->surface.mMipImage;
    sampler->texture.surface.mImage = image;
    nativeSurface(sampler)->mMipImage = mipmaps;
    nativeSurface(sampler)->mImage = image;
    sampler->texture.surface.mMipImage = mipmaps;
}

/* 027AE718 */
agl::LightPrepass* agl::LightPrepass::create(LightPrepass* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = (LightPrepass*)allocateLightStorage(sizeof(LightPrepass));
    if (object == NULL) return object;
    object->mMethods = &sLightPrepassMethods;
    object->mFlags = 0x164220;
    object->mDirty = 0;
    object->mEnabledKinds = 0;
    object->mViews = NULL;
    object->mViewCount = 0;
    object->mExtraCollections = NULL;
    object->mExtraCount = 0;
    object->mMode = 1;
    object->mHeightScale = 1.0f;
    object->mWidthScale = 1.0f;
    sead::CompactRenderState::create(&object->mMainState);
    sead::CompactRenderState::create(&object->mMainDepthState);
    sead::RenderState::create(&object->mSeparateState);
    sead::RenderState::create(&object->mSeparateDepthState);
    sead::CompactRenderState::create(&object->mFollowingState);
    sead::RenderState::create(&object->mFollowingSeparateState);
    initializeFacade_l(&object->mProperties);
    object->mCurveSamples = NULL;
    object->mCurveSampleState = 0;
    TextureData::create(&object->mCurveSurface);
    TextureSampler::create(&object->mCurveTexture);

    // The specular curve parameter: key "SpecularCurve" and its (Japanese) description.
    gabi::FrameLocal<sead::SafeText> key(8);
    gabi::FrameLocal<sead::SafeText> description(0x10);
    description->mVtable = &sParameterTextMethods;
    key->mVtable = &sParameterTextMethods;
    description->mText.v = gabi::guest_str("\203\211\203\164\203\154\203\130\221\316\203\130\203\171\203\114\203\205\203\211\217\346\220\224\057\213\255\223\170\202\314\203\112\201\133\203\165");
    key->mText.v = gabi::guest_str("SpecularCurve");
    initializeParameter_l(&object->mSpecularCurve, key, description, 0);
    object->mSpecularCurve.mMethods = (ParameterMethods_l*)&sCurveParameterMethods_l;
    constructArray(object->mSpecularCurve.mDescriptors, 2, sizeof(CurveDescriptor),
                   gabi::guest_fn((const void*)&createCurveDescriptor));
    object->mSpecularCurve.initializeCurves();
    object->mAfterDraw = NULL;
    object->mOutputIndex = 0;
    object->mBeforeDraw = NULL;
    object->mThreshold = 0.1f;
    gabi::FrameLocal<sead::SafeText> annotation(0x18);
    annotation->mVtable = &sParameterTextMethods;
    annotation->mText.v = gabi::guest_str("Icon=LIGHT");
    setParameterAnnotation_l(object, annotation);

    // Light accumulation: additive blending, no depth test or write; the depth variant tests depth (GREATER-EQUAL).
    sead::CompactRenderState* main = &object->mMainState;
    sead::CompactRenderState* mainDepth = &object->mMainDepthState;
    main->mBlend.mColorEquation = 0;
    main->mBlend.mAlphaEquation = 0;
    main->mCullMode = 0;
    main->mWriteBlue = 1;
    main->mBlend.mAlphaDestination = 1;
    main->mWriteGreen = 1;
    main->mWriteRed = 1;
    main->mBlend.mAlphaSource = 1;
    main->mDepthWrite = 0;
    main->mWriteAlpha = 1;
    main->mDepthTest = 0;
    main->mBlend.mColorSource = 1;
    main->mBlendEnable = 1;
    main->mBlend.mColorDestination = 1;
    // copy of the main state (float words as integers), then the depth settings
    mainDepth->mCullMode = (u32)main->mCullMode;
    mainDepth->mBlend.mColorSource = (u32)main->mBlend.mColorSource;
    mainDepth->mBlend.mAlphaSource = (u32)main->mBlend.mAlphaSource;
    mainDepth->mBlend.mColorDestination = (u32)main->mBlend.mColorDestination;
    mainDepth->mBlend.mAlphaDestination = (u32)main->mBlend.mAlphaDestination;
    mainDepth->mBlend.mColorEquation = (u32)main->mBlend.mColorEquation;
    mainDepth->mBlend.mAlphaEquation = (u32)main->mBlend.mAlphaEquation;
    mainDepth->mAlphaComparison = (u32)main->mAlphaComparison;
    mainDepth->mStencilComparison = (u32)main->mStencilComparison;
    mainDepth->mStencilReference = (u32)main->mStencilReference;
    mainDepth->mStencilMask = (u32)main->mStencilMask;
    mainDepth->mStencilFail = (u32)main->mStencilFail;
    mainDepth->mStencilDepthFail = (u32)main->mStencilDepthFail;
    mainDepth->mStencilPass = (u32)main->mStencilPass;
    mainDepth->mFrontPolygon = (u32)main->mFrontPolygon;
    mainDepth->mBackPolygon = (u32)main->mBackPolygon;
    copyFloatBits(&mainDepth->mBlendRed, &main->mBlendRed);
    copyFloatBits(&mainDepth->mBlendGreen, &main->mBlendGreen);
    copyFloatBits(&mainDepth->mBlendBlue, &main->mBlendBlue);
    copyFloatBits(&mainDepth->mBlendAlpha, &main->mBlendAlpha);
    copyFloatBits(&mainDepth->mAlphaReference, &main->mAlphaReference);
    mainDepth->mBlendEnable = (u8)main->mBlendEnable;
    mainDepth->mAlphaTest = (u8)main->mAlphaTest;
    mainDepth->mWriteRed = (u8)main->mWriteRed;
    mainDepth->mWriteGreen = (u8)main->mWriteGreen;
    mainDepth->mWriteBlue = (u8)main->mWriteBlue;
    mainDepth->mWriteAlpha = (u8)main->mWriteAlpha;
    mainDepth->mStencilEnable = (u8)main->mStencilEnable;
    mainDepth->mPolygonOffset = (u8)main->mPolygonOffset;
    mainDepth->mLineOffset = (u8)main->mLineOffset;
    mainDepth->mPointOffset = (u8)main->mPointOffset;
    mainDepth->mDepthComparison = 4;
    mainDepth->mDepthTest = 1;
    mainDepth->mDepthWrite = 0;

    // The separate light buffers: additive colour blending on the first two targets, all channels written.
    sead::RenderState* separate = &object->mSeparateState;
    sead::RenderState* separateDepth = &object->mSeparateDepthState;
    separate->mChannelMask = 0;
    separate->mCullMode = 0;
    separate->mDepthTest = 0;
    separate->mDepthWrite = 0;
    for (u32 target = 0; target != 2; ++target) {
        separate->mBlendMask = (u32)separate->mBlendMask | (1u << target);
        separate->mBlend[target].mAlphaEquation = 0;
        separate->mBlend[target].mColorEquation = 0;
        separate->mBlend[target].mColorSource = 1;
        separate->mBlend[target].mColorDestination = 1;
        u32 mask = separate->mChannelMask;
        u32 channels = 15u << (target * 4);
        separate->mChannelMask = (mask & ~channels) + channels;
    }
    separateDepth->mDepthWrite = (u8)separate->mDepthWrite;
    separateDepth->mCullMode = (u32)separate->mCullMode;
    separateDepth->mBlendMask = (u32)separate->mBlendMask;
    separateDepth->mDepthTest = (u8)separate->mDepthTest;
    separateDepth->mDepthComparison = (u32)separate->mDepthComparison;
    for (u32 target = 0; target != 8; ++target) {
        separateDepth->mBlend[target].mColorSource = (u32)separate->mBlend[target].mColorSource;
        separateDepth->mBlend[target].mAlphaSource = (u32)separate->mBlend[target].mAlphaSource;
        separateDepth->mBlend[target].mColorDestination = (u32)separate->mBlend[target].mColorDestination;
        separateDepth->mBlend[target].mAlphaDestination = (u32)separate->mBlend[target].mAlphaDestination;
        separateDepth->mBlend[target].mColorEquation = (u32)separate->mBlend[target].mColorEquation;
        separateDepth->mBlend[target].mAlphaEquation = (u32)separate->mBlend[target].mAlphaEquation;
    }
    separateDepth->mAlphaTest = (u8)separate->mAlphaTest;
    separateDepth->mStencilEnable = (u8)separate->mStencilEnable;
    separateDepth->mPolygonOffset = (u8)separate->mPolygonOffset;
    separateDepth->mLineOffset = (u8)separate->mLineOffset;
    separateDepth->mPointOffset = (u8)separate->mPointOffset;
    separateDepth->mAlphaComparison = (u32)separate->mAlphaComparison;
    separateDepth->mChannelMask = (u32)separate->mChannelMask;
    separateDepth->mStencilComparison = (u32)separate->mStencilComparison;
    separateDepth->mStencilReference = (u32)separate->mStencilReference;
    separateDepth->mStencilMask = (u32)separate->mStencilMask;
    separateDepth->mStencilFail = (u32)separate->mStencilFail;
    separateDepth->mStencilDepthFail = (u32)separate->mStencilDepthFail;
    separateDepth->mStencilPass = (u32)separate->mStencilPass;
    separateDepth->mFrontPolygon = (u32)separate->mFrontPolygon;
    separateDepth->mBackPolygon = (u32)separate->mBackPolygon;
    copyFloatBits(&separateDepth->mBlendRed, &separate->mBlendRed);
    copyFloatBits(&separateDepth->mBlendGreen, &separate->mBlendGreen);
    copyFloatBits(&separateDepth->mBlendBlue, &separate->mBlendBlue);
    copyFloatBits(&separateDepth->mBlendAlpha, &separate->mBlendAlpha);
    copyFloatBits(&separateDepth->mAlphaReference, &separate->mAlphaReference);
    separateDepth->mDepthWrite = 0;
    separateDepth->mDepthTest = 1;
    separateDepth->mDepthComparison = 4;
    finishLightResources_l();
    return object;
}

/* 027AEC5C */
void agl::LightPrepass::destroy(LightPrepass* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) return;
    u32 count = object->mViewCount;
    LightView* views = object->mViews;
    object->mMethods = &sLightPrepassMethods;
    LightView* end = guestElement(views, count);
    // GHS keeps an empty walk over the views before the array is destroyed.
    for (LightView* cursor = views; cursor != end; ++cursor) {}
    if (views != NULL) {
        // members in reverse order, re-reading the array after each view
        s32 bound = count;
        for (s32 index = 0; index < bound; ++index) {
            LightView* view = guestElement(views, (u32)index);
            if (view != NULL) {
                destroyPostprocess_l(view->mPostprocess, 2);
                view->mTarget.destroy(2);
                TextureSampler::destroy(&view->mFollowingTexture, 2);
                TextureSampler::destroy(&view->mSpecularTexture, 2);
                TextureSampler::destroy(&view->mLightingTexture, 2);
                TextureSampler::destroy(&view->mPrimaryTexture, 2);
                UniformBuffer::destroy(&view->mUniform, 2);
                bound = object->mViewCount;
                views = object->mViews;
            }
        }
        sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager, views);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, (LightView*)object->mViews);
        object->mViewCount = 0;
        object->mViews = NULL;
    }
    // The collections are released, then their pointer slots (not the collections) are passed to operator delete.
    for (u32 index = 0; index != LightCollection_Count; ++index) {
        ParameterCollection* collection = object->mCollections[index];
        gabi::call_ptr<void>(collection->mTable->mRelease.mFunction, collection);
        deleteLightStorage(&object->mCollections[index]);
    }
    gptr<ParameterCollection>* extra = object->mExtraCollections;
    gptr<ParameterCollection>* extraEnd = guestElement(extra, object->mExtraCount);
    for (; extra != extraEnd; ++extra) {
        ParameterCollection* collection = *extra;
        if (collection != NULL) {
            gabi::call_ptr<void>(collection->mTable->mRelease.mFunction, collection);
            deleteLightStorage(extra);
        }
    }
    be<u32>* samples = object->mCurveSamples;
    if (samples != NULL) {
        deleteLightArray(samples);
        object->mCurveSamples = NULL;
    }
    releaseCollectionFacade_l(&object->mProperties);
    TextureSampler::destroy(&object->mCurveTexture, 2);
    // the property facade's own destructor, inlined
    object->mProperties.mDisposer.mVtable = (sead::DisposerVtable*)&sPropertyDisposerMethods_l;
    object->mProperties.mObjectMethods = &sPropertyObjectMethods_l;
    releaseCollectionFacade_l(&object->mProperties);
    ((sead::RegisteredObject*)&object->mProperties)->destroy(0);
    if (flags & 1) deleteLightStorage(object);
}

/* 027AEE5C */
void agl::LightPrepass::dirtyTextures() {
    u32 flags = mFlags;
    u32 count = mViewCount;
    u32 lighting = (flags >> 3) & 1;
    u32 following = (flags >> 4) & 1;
    LightView* views = mViews;
    LightView* end = guestElement(views, count);
    for (LightView* view = views; view != end; ++view) {
        u8 lightingDirty = view->mLightingTexture.dirty;
        u8 followingDirty = view->mFollowingTexture.dirty;
        view->mFollowingTexture.settings.modes[0] = following;
        view->mLightingTexture.settings.modes[2] = 0;
        view->mLightingTexture.dirty = lightingDirty | SamplerDirty_Modes;
        view->mFollowingTexture.settings.modes[1] = following;
        view->mLightingTexture.settings.modes[1] = lighting;
        view->mLightingTexture.settings.modes[0] = lighting;
        view->mFollowingTexture.settings.modes[2] = 0;
        view->mFollowingTexture.dirty = followingDirty | SamplerDirty_Modes;
    }
}

// The curve value as the original's fmadds computes it: the callee's full FPR times frC rounded to 25 bits, plus frB.
static f32 scaledCurveValue(f64 value, f64 scale, f64 offset) {
    return gabi::f32_from_fpr(gabi::ppc_nan3(value, offset, scale, value * round25(scale) + offset, gabi::ppc_dnan64()));
}
// The texture is little-endian: each float is stored byte-swapped through the frame's conversion slot.
static f32 swappedSample(be<u32>* scratch, f32 value) {
    *scratch = gabi::f32_bits(value);
    u32 bits = *scratch;
    *scratch = __builtin_bswap32(bits);
    return gabi::f32_from_bits(*scratch);
}
/* 027AEEC4 */
void agl::LightPrepass::buildCurveTexture() {
    gabi::AutoFrame frame{gabi::current_function()};
    // Two rows of samples: row 1 is curve 0 (scaled by X), row 0 is curve 1 (scaled by Y).
    u32 index = 0;
    u32 count = mCurveSampleCount;
    gabi::FrameLocal<be<u32>> scratch(0xC);
    while (index < count) {
        f32 position = gabi::fdivs_ppc((f32)index, (f32)count);
        f64 first = evaluateCurve_l(&mSpecularCurve, 0, gabi::fpr_from_f32(position));
        f32 x = scaledCurveValue(first, gabi::fpr_from_f32(mCurveScaleX), gabi::fpr_from_f32(mCurveOffsetX));
        f64 second = evaluateCurve_l(&mSpecularCurve, 1, gabi::fpr_from_f32(position));
        be<u32>* samples = mCurveSamples;
        f64 scaleY = gabi::fpr_from_f32(mCurveScaleY);
        count = mCurveSampleCount;
        u32 secondRow = index + count;
        f64 offsetY = gabi::fpr_from_f32(mCurveOffsetY);
        f32 firstSample = swappedSample(scratch, x);
        f32 y = scaledCurveValue(second, scaleY, offsetY);
        gabi::store<f32>(gabi::ea(samples) + secondRow * 4, firstSample);
        samples = mCurveSamples;
        f32 secondSample = swappedSample(scratch, y);
        gabi::store<f32>(gabi::ea(samples) + index * 4, secondSample);
        count = mCurveSampleCount;
        ++index;
    }
    mCurveSurface.configure(1, 20, count, 2, 1, 1, 0, 0);
    void* image = mCurveSamples;
    mCurveSurface.surface.mImage = image;
    replaceBitmapMip((SurfaceData_l*)&mCurveSurface, image, 0);  // AGL#14 declares it on its view of the texture data
    if (!sameSurfaceShape(&mCurveTexture.texture, &mCurveSurface)) {
        mCurveTexture.copyTexture(&mCurveSurface);
        u8 dirty = mCurveTexture.dirty;
        mCurveTexture.settings.modes[3] = 1;
        mCurveTexture.settings.modes[4] = 1;
        mCurveTexture.settings.modes[5] = 1;
        mCurveTexture.settings.modes[0] = 0;
        mCurveTexture.settings.modes[1] = 0;
        mCurveTexture.dirty = dirty | SamplerDirty_Texture | SamplerDirty_Modes;
        mCurveTexture.settings.modes[2] = 0;
    } else {
        image = mCurveSurface.surface.mImage;
        void* mipmaps = mCurveSurface.surface.mMipImage;
        nativeSurface(&mCurveTexture)->mImage = image;
        mCurveTexture.texture.surface.mImage = image;
        u8 dirty = mCurveTexture.dirty;
        nativeSurface(&mCurveTexture)->mMipImage = mipmaps;
        mCurveTexture.texture.surface.mMipImage = mipmaps;
        mCurveTexture.dirty = dirty | SamplerDirty_Texture | SamplerDirty_Modes;
        mCurveTexture.settings.modes[3] = 1;
        mCurveTexture.settings.modes[4] = 1;
        mCurveTexture.settings.modes[5] = 1;
        mCurveTexture.settings.modes[0] = 0;
        mCurveTexture.settings.modes[1] = 0;
        mCurveTexture.settings.modes[2] = 0;
    }
}

// inlined into configure: allocation from the given heap, or the current one (fetched for each allocation)
static void* allocateFromConfigHeap(sead::RegistryHeap* heap, u32 size) {
    if (heap == NULL) heap = registryCurrentHeap(sRegistryManager);
    return gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction, heap, size, 4);
}
// inlined into configure: a light collection's constructor (cleared storage, empty base prefix, base then derived
// method table, vertex input)
template <class Collection>
static Collection* constructLightCollection(sead::RegistryHeap* heap, void* baseMethods, void* methods) {
    Collection* collection = (Collection*)allocateFromHeap_l(sizeof(Collection), heap, 4);
    if (collection != NULL) {
        clearBytes(collection, sizeof(Collection));
        collection->mBase.mTable = (agl::CollectionTable*)baseMethods;
        collection->mBase.mRecords = NULL;
        collection->mBase.mCapacity = 0;
        collection->mBase.mValueCounts = NULL;
        collection->mBase.mValueCapacity = 0;
        collection->mBase.mTable = (agl::CollectionTable*)methods;
        agl::VertexInputSet::create(&collection->mVertexInput);
    }
    return collection;
}
/* 027AF16C */
void agl::LightPrepass::configure(const LightPrepassConfig* config, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mViewportWidth = 1280;
    mThumbnailWidth = 96;
    mTileSize = 32;
    mThumbnailHeight = 96;
    mViewportHeight = 720;
    s32 requested = config->mViewCount;
    if (requested > 0) {
        LightView* views = (LightView*)allocateFromConfigHeap(heap, (u32)requested * sizeof(LightView));
        for (u32 index = 0; index != (u32)requested; ++index) {
            LightView* view = guestElement(views, index);
            if (view == NULL) continue;
            createViewCamera_l(&view->mCamera);
            UniformBuffer::create(&view->mUniform);
            TextureSampler::create(&view->mPrimaryTexture);
            TextureSampler::create(&view->mLightingTexture);
            TextureSampler::create(&view->mSpecularTexture);
            TextureSampler::create(&view->mFollowingTexture);
            view->mTarget.construct();
            view->mPrimaryColor.construct();
            view->mFollowingColor.construct();
            view->mDepth.construct();
            sead::RenderState::create(&view->mResolveState);
            view->mFlags = 0;
            view->mPhase = 0;
            createPostprocess_l(view->mPostprocess);
            view->mPostprocessStorage[0] = 0;
            view->mPostprocessStorage[4] = 0;
            view->mPostprocessStorage[5] = 0;
            view->mPostprocessStorage[3] = 0;
            view->mPostprocessStorage[2] = 0;
            view->mPostprocessStorage[1] = 0;
        }
        if (views != NULL) {
            mViewCount = requested;
            mViews = views;
        }
    }
    LightView* begin = mViews;
    LightView* end = guestElement(begin, mViewCount);
    for (LightView* view = begin; view != end; ++view) {
        u32 index = (gabi::ea(view) - gabi::ea(begin)) / sizeof(LightView);
        view->mTargetHasDepth = 0;
        view->mFlags = 0;
        view->mPhase = 0;
        view->mSeparateEnabled = 0;
        view->mTemporaryTexture = NULL;
        view->mFollowingEnabled = 0;
        if (index == 0) {
            // view matrix, base projection, inverse projection; near, range, inverse range, fade; two pairs; scale
            view->mUniform.allocateFormat(10, heap);
            view->mUniform.appendField(5, 4);
            view->mUniform.appendField(5, 3);
            view->mUniform.appendField(5, 3);
            view->mUniform.appendField(2, 1);
            view->mUniform.appendField(2, 1);
            view->mUniform.appendField(2, 1);
            view->mUniform.appendField(2, 1);
            view->mUniform.appendField(3, 1);
            view->mUniform.appendField(3, 1);
            view->mUniform.appendField(2, 1);
            view->mUniform.allocateStorage(heap, 2);
        } else {
            LightView* first = mViews;
            view->mUniform.adoptFormat(&first->mUniform);
            view->mUniform.allocateStorage(heap, 2);
        }
        view->mResolveState.mDepthTest = 0;
        view->mResolveState.mDepthWrite = 0;
        view->mResolveState.mBlendMask = view->mResolveState.mBlendMask & ~1u;
        u32 mask = view->mResolveState.mBlendMask;
        view->mResolveState.mDepthComparison = 7;
        view->mResolveState.mChannelMask = 255;
        view->mResolveState.mBlendMask = mask & ~2u;
    }
    mCollections[LightCollection_Point] = (ParameterCollection*)constructLightCollection<PointLightCollection_l>(
        heap, (void*)&sPointCollectionBaseMethods, (void*)&sPointCollectionMethods);
    mCollections[LightCollection_Directed] = (ParameterCollection*)constructLightCollection<DirectedLightCollection_l>(
        heap, (void*)&sDirectedCollectionBaseMethods, (void*)&sDirectedCollectionMethods);
    mCollections[LightCollection_Spatial] = (ParameterCollection*)constructLightCollection<SpatialLightCollection_l>(
        heap, (void*)&sSpatialCollectionBaseMethods, (void*)&sSpatialCollectionMethods);
    mCollections[LightCollection_Paired] = (ParameterCollection*)constructLightCollection<PairedLightCollection_l>(
        heap, (void*)&sPairedCollectionBaseMethods, (void*)&sPairedCollectionMethods);
    // initialize(owner, record capacity, value capacity = one value per view, heap); half as many point pairs
    for (u32 kind = 0; kind != LightCollection_Count; ++kind) {
        ParameterCollection* collection = mCollections[kind];
        u32 initialize = collection->mTable->mInitialize.mFunction;
        s32 capacity = kind == LightCollection_Point      ? (s32)config->mPointCount
                       : kind == LightCollection_Directed ? (s32)config->mDirectedCount
                       : kind == LightCollection_Spatial  ? (s32)config->mSpatialCount
                                                          : (s32)config->mPointCount / 2;
        s32 views = config->mViewCount;
        gabi::call_ptr<void>(initialize, collection, this, capacity, views, heap);
    }
    requested = config->mExtraCount;
    if (requested > 0) {
        gptr<ParameterCollection>* extra =
            (gptr<ParameterCollection>*)allocateFromConfigHeap(heap, (u32)requested * sizeof(gptr<ParameterCollection>));
        if (extra != NULL) {
            mExtraCount = requested;
            mExtraCollections = extra;
        }
    }
    gptr<ParameterCollection>* extra = mExtraCollections;
    gptr<ParameterCollection>* extraEnd = guestElement(extra, mExtraCount);
    for (; extra != extraEnd; ++extra) *extra = NULL;
    mEnabledKinds = 255;
    dirtyTextures();
    mCurveEnabled = 1;
    mPairThreshold = 0.55f;
    gabi::FrameLocal<SpecularKeys> keys(0x14);
    for (u32 word = 0; word != 6; ++word) keys->mWords[word] = (u32)sSpecularKeys.mWords[word];
    mSpecularCurve.setCurve(0, 7, (const be<f32>*)keys->mWords, 6);
    mSpecularCurve.setCurve(1, 7, (const be<f32>*)keys->mWords, 6);
    mCurveScaleX = 120.0f;
    mCurveScaleY = 1.0f;
    mCurveOffsetX = 0.0f;
    mCurveOffsetY = 0.0f;
    mCurveSampleCount = 256;
    mCurveSurface.configure(1, 20, 256, 2, 1, 1, 0, 0);
    u32 bytes = mCurveSurface.surface.mImageBytes;
    mCurveSamples = (be<u32>*)allocateAlignedFromHeap_l(bytes, heap, 0x2000);
    buildCurveTexture();
    u32 views = config->mViewCount;
    gabi::FrameLocal<sead::SafeText> name(0xC);
    name->mVtable = &sParameterTextMethods;
    name->mText.v = gabi::guest_str("LightPrePass");
    configureFacade_l(&mProperties, views, name, heap);
}

// inlined into update: the derived interface of a collection slot, or NULL when the collection is not one
static agl::ParameterCollection* castCollection(agl::ParameterCollection* collection, const sead::RuntimeType* type) {
    if (collection == NULL ||
        gabi::call_ptr<u32>(collection->mTable->mMatchesType.mFunction, collection, type) == 0)
        return NULL;
    return collection;
}
/* 027AF91C */
void agl::LightPrepass::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 output = mOutputIndex;
    u32 flags = mFlags;
    mOutputIndex = output == 0;
    bool pairPoints = (flags & LightPrepass_PairPoints) != 0;
    u32 guard = sPointExTypeGuard_l;
    ParameterCollection* pointSlot = mCollections[LightCollection_Point];
    if (guard == 0) {
        sPointExTypeGuard_l = 1;
        sPointExType_l.mTable = &sPointExTypeMethods;
    }
    ParameterCollection* pointInterface = castCollection(pointSlot, &sPointExType_l);
    guard = sCurveExTypeGuard_l;
    ParameterCollection* pairSlot = mCollections[LightCollection_Paired];
    if (guard == 0) {
        sCurveExTypeGuard_l = 1;
        sCurveExType_l.mTable = &sCurveExTypeMethods;
    }
    ParameterCollection* pairInterface = castCollection(pairSlot, &sCurveExType_l);
    CompactCollection* points = (CompactCollection*)pointInterface;
    CurveCollection* pairs = (CurveCollection*)pairInterface;
    gabi::call_ptr<void>(pairs->mTable->mSetCount.mFunction, pairs, 0u);

    // Clear every point's paired mark.
    s32 count = (s32)gabi::call_ptr<u32>(pointInterface->mTable->mCount.mFunction, pointInterface);
    if (count > 0) {
        for (u32 index = 0; index != (u32)count; ++index) {
            CompactRecord* record = clampedElement((CompactRecord*)points->mRecords, points->mCapacity, index);
            record->mFlags = record->mFlags & ~PointRecord_Paired;
        }
    }
    // Pair each unpaired, unattenuated point light with the next one at the same position that has the other shadow
    // setting, the same mask and (with falloff) a similar radius, until the pair collection is full.
    if (pairPoints) {
        s32 total = (s32)gabi::call_ptr<u32>(pointInterface->mTable->mCount.mFunction, pointInterface);
        s32 remaining = (s32)((u32)total - 1);
        u32 primaryIndex = 0;
        while (remaining > 0) {
            u32 capacity = points->mCapacity;
            CompactRecord* records = points->mRecords;
            CompactRecord* primary = clampedElement(records, capacity, primaryIndex);
            u16 primaryFlags = primary->mFlags;
            if ((primaryFlags & PointRecord_Paired) == 0) {
                f32 primaryAttenuation = primary->mAttenuation;
                f32 primaryRadius = primary->mRadius;
                bool primaryEligible = primaryAttenuation == 0.0f;
                u32 primaryMask = primary->mMask;
                u32 primaryShadow = (primaryFlags & PointRecord_Shadow) != 0;
                if (primaryEligible) {
                    bool found = false;
                    bool full = false;
                    for (s32 secondaryIndex = (s32)(primaryIndex + 1); secondaryIndex < total; ++secondaryIndex) {
                        CompactRecord* secondary = clampedElement(records, capacity, (u32)secondaryIndex);
                        u16 secondaryFlags = secondary->mFlags;
                        if ((secondaryFlags & PointRecord_Paired) != 0) continue;
                        f32 secondaryAttenuation = secondary->mAttenuation;
                        f32 secondaryX = secondary->mPosition[0];
                        bool secondaryEligible = secondaryAttenuation == 0.0f;
                        f32 primaryX = primary->mPosition[0];
                        bool sameX = primaryX == secondaryX;
                        u32 secondaryMask = secondary->mMask;
                        u32 secondaryShadow = (secondaryFlags & PointRecord_Shadow) != 0;
                        f32 secondaryRadius = secondary->mRadius;
                        if (!sameX) continue;
                        if (!((f32)primary->mPosition[1] == (f32)secondary->mPosition[1])) continue;
                        if (!((f32)primary->mPosition[2] == (f32)secondary->mPosition[2])) continue;
                        if (primaryShadow == secondaryShadow) continue;
                        if (!secondaryEligible) continue;
                        if (primaryMask != secondaryMask) continue;
                        if (((primaryFlags | secondaryFlags) & PointRecord_Falloff) != 0) {
                            if (primaryRadius < secondaryRadius) {
                                f32 ratio = gabi::fdivs_ppc(primaryRadius, secondaryRadius);
                                if (ratio < (f32)mPairThreshold) continue;
                            } else if (primaryRadius > secondaryRadius) {
                                f32 ratio = gabi::fdivs_ppc(secondaryRadius, primaryRadius);
                                if (ratio < (f32)mPairThreshold) continue;
                            }
                        }
                        // append a pair (the method words are read before the calls)
                        u32 pairIndex = gabi::call_ptr<u32>(pairs->mTable->mCount.mFunction, pairs);
                        u32 pairCapacity = pairs->mCapacity;
                        CollectionTable* table = pairs->mTable;
                        CurveRecord* pairRecords = pairs->mRecords;
                        u32 countMethod = table->mCount.mFunction;
                        CurveRecord* pair = clampedElement(pairRecords, pairCapacity, pairIndex);
                        u32 setCountMethod = table->mSetCount.mFunction;
                        u32 current = gabi::call_ptr<u32>(countMethod, pairs);
                        gabi::call_ptr<void>(setCountMethod, pairs, current + 1);
                        u8 pairFlags = pair->mFlags;
                        pair->mMask = primaryMask;
                        pair->mFlags = pairFlags | Record_Enabled;
                        // the shadow-casting light goes second
                        pair->mPrimary = primaryShadow != 0 ? secondary : primary;
                        pair->mSecondary = primaryShadow != 0 ? primary : secondary;
                        primary->mFlags = primary->mFlags | PointRecord_Paired;
                        secondary->mFlags = secondary->mFlags | PointRecord_Paired;
                        table = pairs->mTable;
                        u32 used = gabi::call_ptr<u32>(table->mCount.mFunction, pairs);
                        u32 available = gabi::call_ptr<u32>(table->mCapacity.mFunction, pairs);
                        full = used == available;
                        found = true;
                        break;
                    }
                    if (!found) {
                        CollectionTable* table = pairs->mTable;
                        u32 used = gabi::call_ptr<u32>(table->mCount.mFunction, pairs);
                        u32 available = gabi::call_ptr<u32>(table->mCapacity.mFunction, pairs);
                        full = used == available;
                    }
                    if (full) break;
                }
            }
            --remaining;
            ++primaryIndex;
        }
    }
    for (u32 index = 0; index != LightCollection_Count; ++index) {
        ParameterCollection* collection = mCollections[index];
        gabi::call_ptr<void>(collection->mTable->mUpdate.mFunction, collection);
    }
    gptr<ParameterCollection>* extra = mExtraCollections;
    gptr<ParameterCollection>* end = guestElement(extra, mExtraCount);
    for (; extra != end; ++extra) {
        ParameterCollection* collection = *extra;
        if (collection != NULL) gabi::call_ptr<void>(collection->mTable->mUpdate.mFunction, collection);
    }
    u32 dirty = mDirty;
    if ((dirty & (LightDirty_Tracking | LightDirty_Changed)) == (LightDirty_Tracking | LightDirty_Changed))
        mDirty = dirty & ~(u32)(LightDirty_Tracking | LightDirty_Changed);
}

/* 027AFEEC */
void agl::LightPrepass::updateView(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    LightView* view = clampedElement((LightView*)mViews, mViewCount, index);
    f32 far = view->mCamera.mFar;
    f32 near = view->mCamera.mNear;
    u32 phase = view->mPhase;
    f32 ratio = gabi::fdivs_ppc(near, far);
    view->mPhase = phase & ~(u32)LightPhase_Mask;
    view->mFade = gabi::fsubs_ppc(1.0f, ratio);
    copyRegistryMatrix_l(&view->mCamera.mView, &view->mViewCopy);
    combineRegistryMatrices(&view->mViewProjection, &view->mCamera.mProjection, &view->mCamera.mView);
    near = view->mCamera.mNear;
    far = view->mCamera.mFar;
    f32 width = view->mCamera.mProjectionWidth;
    f32 range = gabi::fsubs_ppc(far, near);
    f32 aspect = view->mCamera.mProjectionAspect;
    f32 projectionScale = gabi::fmuls_ppc(width, aspect);
    view->mDepthRange = range;
    view->mClipSign = -1.0f;
    f32 inverseRange = gabi::fdivs_ppc(1.0f, range);
    f32 horizontal = view->mCamera.mHorizontalScale;
    view->mInverseDepthRange = inverseRange;
    f32 vertical = view->mCamera.mVerticalScale;
    horizontal = gabi::fadds_ppc(horizontal, horizontal);
    vertical = gabi::fadds_ppc(vertical, vertical);
    aspect = view->mCamera.mProjectionAspect;
    f32 scaleX = gabi::fmuls_ppc(horizontal, projectionScale);
    f32 scaleY = gabi::fmuls_ppc(vertical, aspect);
    view->mScaleX = scaleX;
    view->mProjectionScale = projectionScale;
    view->mScaleY = scaleY;
    selectUniformSlot(&view->mUniform, mOutputIndex);
    // the collections prepare their values for this view; a view without any prepared value is empty
    u32 prepared = 0;
    for (u32 kind = 0; kind != LightCollection_Count; ++kind) {
        ParameterCollection* collection = mCollections[kind];
        u32 output = mOutputIndex;
        prepared += gabi::call_ptr<u32>(collection->mTable->mPrepare.mFunction, collection, index, view, output);
    }
    gptr<ParameterCollection>* extra = mExtraCollections;
    gptr<ParameterCollection>* end = guestElement(extra, mExtraCount);
    for (; extra != end; ++extra) {
        ParameterCollection* collection = *extra;
        if (collection != NULL) {
            u32 output = mOutputIndex;
            prepared += gabi::call_ptr<u32>(collection->mTable->mPrepare.mFunction, collection, index, view, output);
        }
    }
    if (prepared == 0) view->mFlags = view->mFlags | LightView_Empty;
    else view->mFlags = view->mFlags & ~(u32)LightView_Empty;
}
/* 027B00D0 */
void agl::LightPrepass::updateViewInputs(u32 index, void* projection, const void* camera, const void* viewport) {
    gabi::AutoFrame frame{gabi::current_function()};
    LightView* view = clampedElement((LightView*)mViews, mViewCount, index);
    setViewCamera_l(view, camera, viewport);
    setViewProjection_l(view, projection);
    updateView(index);
}
/* 027B0158 */
void agl::LightPrepass::refreshCollections(void* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    for (u32 kind = 0; kind != LightCollection_Count; ++kind) {
        ParameterCollection* collection = mCollections[kind];
        gabi::call_ptr<void>(collection->mTable->mRefresh.mFunction, collection, context);
    }
    gptr<ParameterCollection>* extra = mExtraCollections;
    gptr<ParameterCollection>* end = guestElement(extra, mExtraCount);
    for (; extra != end; ++extra) {
        ParameterCollection* collection = *extra;
        if (collection != NULL) gabi::call_ptr<void>(collection->mTable->mRefresh.mFunction, collection, context);
    }
}
/* 027B0214 */
u32 agl::LightPrepass::skipView(u32 index) const {
    LightView* view = clampedElement((LightView*)mViews, mViewCount, index);
    if ((view->mFlags & LightView_Empty) != 0 && (mFlags & LightPrepass_SkipEmpty) != 0) return 1;
    return 0;
}
/* 027B0254 */
void agl::LightPrepass::uploadView(u32 index, u32 invalidateGpu) {
    gabi::AutoFrame frame{gabi::current_function()};
    LightView* view = clampedElement((LightView*)mViews, mViewCount, index);
    if (skipView(index) != 0) return;
    UniformBuffer* uniform = &view->mUniform;
    uniform->clear();
    uniform->writeWords(view->mUniform.cpuData, 0, (const be<u32>*)(&view->mViewProjection), 0, 4);
    uniform->writeWords(view->mUniform.cpuData, 1, (const be<u32>*)(&view->mCamera.mBaseProjection), 0, 3);
    // the scalars go through their own frame slots
    gabi::FrameLocal<be<f32>> near(0x10);
    f32 value = view->mCamera.mNear;
    void* data = view->mUniform.cpuData;
    *near = value;
    uniform->writeWords(data, 3, (const be<u32>*)near.get(), 0, 1);
    gabi::FrameLocal<be<f32>> range(0x14);
    value = view->mDepthRange;
    data = view->mUniform.cpuData;
    *range = value;
    uniform->writeWords(data, 4, (const be<u32>*)range.get(), 0, 1);
    gabi::FrameLocal<be<f32>> inverseRange(0x18);
    value = view->mInverseDepthRange;
    data = view->mUniform.cpuData;
    *inverseRange = value;
    uniform->writeWords(data, 5, (const be<u32>*)inverseRange.get(), 0, 1);
    gabi::FrameLocal<be<f32>> fade(0x1C);
    value = view->mFade;
    data = view->mUniform.cpuData;
    *fade = value;
    uniform->writeWords(data, 6, (const be<u32>*)fade.get(), 0, 1);
    f32 heightScale = mHeightScale;
    f32 inverseHeight = 0.0f;
    data = view->mUniform.cpuData;
    if (0.0f < heightScale) inverseHeight = gabi::fdivs_ppc(1.0f, heightScale);
    gabi::FrameLocal<be<f32>> inverseHeightSlot(0x20);
    *inverseHeightSlot = inverseHeight;
    uniform->writeWords(data, 9, (const be<u32>*)inverseHeightSlot.get(), 0, 1);
    f32 aspect = view->mCamera.mProjectionAspect;
    f32 clipSign = view->mClipSign;
    f32 projectionScale = view->mProjectionScale;
    f32 slope = gabi::fmuls_ppc(clipSign, aspect);
    gabi::FrameLocal<LightScalarPair> projection(8);
    projection->first = projectionScale;
    projection->second = slope;
    uniform->writeWords(uniform->cpuData, 7, (const be<u32>*)projection.get(), 0, 1);
    uniform->writeWords(uniform->cpuData, 8, (const be<u32>*)(&view->mScaleX), 0, 1);
    uniform->flush(uniform->cpuData, invalidateGpu);
    for (u32 kind = 0; kind != LightCollection_Count; ++kind) {
        ParameterCollection* collection = mCollections[kind];
        gabi::call_ptr<void>(collection->mTable->mApply.mFunction, collection, index, view, invalidateGpu);
    }
    gptr<ParameterCollection>* extra = mExtraCollections;
    gptr<ParameterCollection>* end = guestElement(extra, mExtraCount);
    for (; extra != end; ++extra) {
        ParameterCollection* collection = *extra;
        if (collection != NULL)
            gabi::call_ptr<void>(collection->mTable->mApply.mFunction, collection, index, view, invalidateGpu);
    }
}
/* 027B04B0 */
void agl::LightPrepass::clearView(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    LightView* view = clampedElement((LightView*)mViews, mViewCount, index);
    sead::activateFrameBuffer((sead::DisplayFrame*)&view->mTarget);
    gabi::FrameLocal<sead::Viewport> viewport(0x28);
    sead::Viewport::createSurface(viewport, (const sead::RenderVector2*)&view->mTarget.mBase.mSize);
    viewport->apply((const sead::RenderSurface*)&view->mTarget.mBase);
    u32 flags = mFlags;
    if ((flags & (LightPrepass_Separate | LightPrepass_DepthSeparate)) == (LightPrepass_Separate | LightPrepass_DepthSeparate) &&
        (flags & LightPrepass_Combined) == 0) {
        // both light buffers to black
        gabi::FrameLocal<LightColorBits> color(8);
        color->red = 0;
        color->green = 0;
        color->blue = 0;
        color->alpha = 0;
        clearRenderBuffer_l(&view->mTarget, 0, 1, color, 0, 1.0);
        color->blue = 0;
        color->green = 0;
        color->red = 0;
        color->alpha = 0;
        clearRenderBuffer_l(&view->mTarget, 1, 1, color, 0, 1.0);
    } else {
        gabi::FrameLocal<LightColorBits> color(0x18);
        color->red = 0;
        color->green = 0;
        color->blue = 0;
        color->alpha = 0;
        clearRenderBuffer_l(&view->mTarget, 0, 1, color, 0, 1.0);
    }
    u32 dirty = mDirty;
    if ((dirty & LightDirty_Tracking) != 0) mDirty = dirty | LightDirty_Changed;
}
/* 027B0620 */
agl::TextureData* agl::LightPrepass::prepareTarget(u32 index, u32 width, u32 height, u32 clear) {
    gabi::AutoFrame frame{gabi::current_function()};
    LightView* view = clampedElement((LightView*)mViews, mViewCount, index);
    view->mPhase = view->mPhase | LightPhase_Target;
    f32 extentX = (f32)width;
    f32 extentY = (f32)height;
    view->mTarget.mBase.mSize.mHeight = extentY;
    view->mTarget.mBase.mSize.mWidth = extentX;
    gabi::FrameLocal<sead::FrameBufferArea> area(0x14);
    if (0.0f < extentX) {
        area->mMaxX = extentX;
        area->mMinX = 0.0f;
    } else {
        area->mMaxX = 0.0f;
        area->mMinX = extentX;
    }
    if (0.0f < extentY) {
        area->mMinY = 0.0f;
        area->mMaxY = extentY;
    } else {
        area->mMinY = extentY;
        area->mMaxY = 0.0f;
    }
    view->mTarget.mBase.mArea.mMinX = (f32)area->mMinX;
    view->mTarget.mBase.mArea.mMinY = (f32)area->mMinY;
    view->mTarget.mBase.mArea.mMaxX = (f32)area->mMaxX;
    view->mTarget.mBase.mArea.mMaxY = (f32)area->mMaxY;
    // Separate light buffers use a colour format (two slices when depth goes separately too); otherwise a depth one.
    u32 flags = mFlags;
    bool separate = (flags & LightPrepass_Separate) != 0;
    bool twoSlices = separate && (flags & LightPrepass_DepthSeparate) != 0 && (flags & LightPrepass_Combined) == 0;
    u32 nameSlot = twoSlices ? 0x30 : separate ? 0x38 : 0x40;
    gabi::FrameLocal<sead::SafeText> name(nameSlot);
    name->mVtable = &sParameterTextMethods;
    void* resources = sDynamicTextureResources_l;
    name->mText.v = gabi::guest_str("light_prepass_light_buffer");
    TextureData* texture = acquireTexture_l(resources, name, separate ? 26 : 29, width, height, twoSlices ? 2 : 1, 1, 0, 0, 1, 0);
    view->mTemporaryTexture = texture;
    bool same = sameSurfaceShape(&view->mPrimaryTexture.texture, texture);
    ColorSurface* primaryColor = &view->mPrimaryColor;
    ColorSurface* followingColor = NULL;
    if (separate) {
        if (!same) view->mPrimaryTexture.copyTexture(texture);
        else bindSurfaceImages(&view->mPrimaryTexture, texture);
        texture = view->mTemporaryTexture;
        attachColorTexture_l(primaryColor, texture, 0, 0);
        if (twoSlices) {
            followingColor = &view->mFollowingColor;
            texture = view->mTemporaryTexture;
            attachColorTexture_l(followingColor, texture, 0, 1);
        }
    } else {
        if (!same) {
            view->mPrimaryTexture.copyTexture(texture);
            texture = view->mTemporaryTexture;
        } else {
            void* mipmaps = texture->surface.mMipImage;
            void* image = texture->surface.mImage;
            texture = view->mTemporaryTexture;
            nativeSurface(&view->mPrimaryTexture)->mImage = image;
            view->mPrimaryTexture.texture.surface.mImage = image;
            view->mPrimaryTexture.texture.surface.mMipImage = mipmaps;
            nativeSurface(&view->mPrimaryTexture)->mMipImage = mipmaps;
        }
        attachDepthTexture_l(primaryColor, texture);
    }
    view->mTarget.mColors[0] = primaryColor;
    view->mTarget.mColors[1] = followingColor;
    u32 dirty = mDirty;
    if ((dirty & LightDirty_Tracking) != 0) mDirty = dirty | LightDirty_Changed;
    if (clear != 0) clearView(index);
    return view->mTemporaryTexture;
}
/* 027B0B9C */
void agl::LightPrepass::selectState(const LightDrawContext* context, u32 separate, u32 force, u32 following) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 index = context->mViewIndex;
    LightView* view = clampedElement((LightView*)mViews, mViewCount, index);
    RenderBuffer* target = context->mTarget;
    u32 flags = mFlags;
    u32 hasDepth = target->mDepth.get() != NULL;
    if ((flags & LightPrepass_DepthSeparate) == 0) separate = 0;
    if (force == 0 && (u8)view->mTargetHasDepth == hasDepth && (u8)view->mSeparateEnabled == separate &&
        (u8)view->mFollowingEnabled == following)
        return;
    view->mFollowingEnabled = following;
    view->mTargetHasDepth = hasDepth;
    view->mSeparateEnabled = separate;
    flags = mFlags;
    if (following != 0) {
        if ((flags & LightPrepass_Separate) != 0 && separate != 0) mFollowingSeparateState.apply();
        else mFollowingState.apply();
        return;
    }
    if ((flags & LightPrepass_Separate) != 0 && separate != 0 && (flags & LightPrepass_Combined) == 0) {
        if (hasDepth != 0) mSeparateDepthState.apply();
        else mSeparateState.apply();
        return;
    }
    if (hasDepth != 0) mMainDepthState.apply();
    else mMainState.apply();
}
/* 027B0C84 */
u32 agl::LightPrepass::drawView(u32 index, u32 reservedContext, u32 reservedFlags, u8 following, u32 user) {
    gabi::AutoFrame frame{gabi::current_function()};
    LightView* view = clampedElement((LightView*)mViews, mViewCount, index);
    gabi::FrameLocal<LightDrawContext> context(8);
    context->mOwner = this;
    context->mView = view;
    context->mTarget = &view->mTarget;
    context->mFollowing = following;
    context->mSpecularTexture = &view->mSpecularTexture;
    context->mUniform = &view->mUniform;
    context->mFollowingTexture = &view->mFollowingTexture;
    context->mCurveTexture = &mCurveTexture;
    context->mViewIndex = index;
    context->mUser = user;
    context->mLightingTexture = &view->mLightingTexture;
    selectState(context, 1, 1, 0);
    LightCallback_l* callback = mBeforeDraw;
    if (callback != NULL) gabi::call_ptr<void>(callback->mMethods->mInvoke, callback, context.get());
    selectState(context, 1, 1, 0);
    // Each collection draws when its kind is enabled (with the kind filter, kind 0 never draws).
    for (u32 slot = 0; slot != LightCollection_Count; ++slot) {
        ParameterCollection* collection = mCollections[slot];
        u32 kind = gabi::call_ptr<u32>(collection->mTable->mKind.mFunction, collection);
        u32 kindBit = (kind & 32) ? 0 : 1u << (kind & 31);
        u8 enabled = mEnabledKinds;
        if ((enabled & (kindBit & 255)) == 0) continue;
        if ((mFlags & LightPrepass_KindFilter) != 0) {
            collection = mCollections[slot];
            kind = gabi::call_ptr<u32>(collection->mTable->mKind.mFunction, collection);
            if (kind == 0) continue;
        }
        collection = mCollections[slot];
        gabi::call_ptr<void>(collection->mTable->mApplyWithContext.mFunction, collection, index, view, context.get());
    }
    gptr<ParameterCollection>* extra = mExtraCollections;
    gptr<ParameterCollection>* end = guestElement(extra, mExtraCount);
    for (; extra != end; ++extra) {
        ParameterCollection* collection = *extra;
        if (collection != NULL)
            gabi::call_ptr<void>(collection->mTable->mApplyWithContext.mFunction, collection, index, view, context.get());
    }
    callback = mAfterDraw;
    if (callback != NULL) gabi::call_ptr<void>(callback->mMethods->mInvoke, callback, context.get());
    return context->mUser;
}
/* 027B0E5C */
void agl::LightPrepass::commitColors(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    LightView* view = clampedElement((LightView*)mViews, mViewCount, index);
    view->mPrimaryColor.invalidate();
    if ((mFlags & LightPrepass_Separate) != 0) view->mFollowingColor.invalidate();
}
// inlined into render: the depth surface's copy assignment (everything but its method table, word by word forward)
static void copyDepthSurface(agl::DepthSurface* to, const agl::DepthSurface* from) {
    be<u32>* destination = (be<u32>*)to;
    const be<u32>* source = (const be<u32>*)from;
    for (u32 word = 0; word != 0x78 / 4; ++word) destination[word] = (u32)source[word];
    to->mBase.mMinimumWidth = (u8)from->mBase.mMinimumWidth;
    to->mBase.mMinimumHeight = (u8)from->mBase.mMinimumHeight;
    to->mBase.mMinimumDepth = (u8)from->mBase.mMinimumDepth;
    to->mBase.mMaximumMipCount = (u8)from->mBase.mMaximumMipCount;
    for (u32 word = 0; word != 4; ++word) to->mBase.mComponentMapping[word] = (u32)from->mBase.mComponentMapping[word];
    to->mBase.mDirty = (u8)from->mBase.mDirty;
    destination = (be<u32>*)&to->mBase.mViewMip;
    source = (const be<u32>*)&from->mBase.mViewMip;
    for (u32 word = 0; word != (sizeof(agl::DepthSurface) - 0x94) / 4; ++word) destination[word] = (u32)source[word];
}
/* 027B0EC0 */
u32 agl::LightPrepass::render(u32 index, u32 drawContext, u32 drawFlags, const TextureData* following,
                              const DepthSurface* depth, const TextureData* lighting, u32 user,
                              const TextureData* specular) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 dirty = mDirty;
    u32 count = mViewCount;
    if ((dirty & LightDirty_Tracking) != 0) mDirty = dirty | LightDirty_Changed;
    LightView* view = clampedElement((LightView*)mViews, count, index);
    view->mPhase = view->mPhase | LightPhase_Rendered;
    if (!sameSurfaceShape(&view->mLightingTexture.texture, lighting)) view->mLightingTexture.copyTexture(lighting);
    else bindSurfaceImages(&view->mLightingTexture, lighting);
    if (!sameSurfaceShape(&view->mFollowingTexture.texture, following)) view->mFollowingTexture.copyTexture(following);
    else bindSurfaceImages(&view->mFollowingTexture, following);
    if (specular != NULL) {
        if (!sameSurfaceShape(&view->mSpecularTexture.texture, specular)) view->mSpecularTexture.copyTexture(specular);
        else bindSurfaceImages(&view->mSpecularTexture, specular);
    }
    copyDepthSurface(&view->mDepth, depth);
    view->mTarget.mDepth = (mFlags & LightPrepass_Resolve) != 0 ? &view->mDepth : NULL;
    if ((view->mPhase & LightPhase_Target) == 0) {
        if (skipView(index) != 0) {
            // nothing to light: sample the fallback light buffer
            u32 phase = view->mPhase;
            view->mTemporaryTexture = NULL;
            view->mPhase = phase | LightPhase_Target;
            TextureData* fallback = ((PrimitiveTextureResources_l*)(void*)sPrimitiveTextureResources_l)->mFallbackSurface;
            if (!sameSurfaceShape(&view->mPrimaryTexture.texture, fallback)) view->mPrimaryTexture.copyTexture(fallback);
            else bindSurfaceImages(&view->mPrimaryTexture, fallback);
            dirty = mDirty;
            if ((dirty & LightDirty_Tracking) != 0) mDirty = dirty | LightDirty_Changed;
        } else {
            s32 width = (s32)(u32)depth->mBase.mSurface.mWidth;
            u32 minimumWidth = depth->mBase.mMinimumWidth;
            s32 height = (s32)(u32)depth->mBase.mSurface.mHeight;
            u32 minimumHeight = depth->mBase.mMinimumHeight;
            if ((s32)minimumWidth > width) width = minimumWidth;
            if ((s32)minimumHeight > height) height = minimumHeight;
            prepareTarget(index, (u32)width, (u32)height, 1);
        }
    } else {
        dirty = mDirty;
        if ((dirty & LightDirty_Tracking) != 0) mDirty = dirty | LightDirty_Changed;
        sead::activateFrameBuffer((sead::DisplayFrame*)&view->mTarget);
        gabi::FrameLocal<sead::Viewport> viewport(8);
        sead::Viewport::createSurface(viewport, (const sead::RenderVector2*)&view->mTarget.mBase.mSize);
        viewport->apply((const sead::RenderSurface*)&view->mTarget.mBase);
    }
    if (skipView(index) == 0) user = drawView(index, drawContext, drawFlags, 1, user);
    commitColors(index);
    view->mTarget.mDepth = NULL;
    dirty = mDirty;
    if ((dirty & LightDirty_Tracking) != 0) mDirty = dirty | LightDirty_Changed;
    u32 phase = view->mPhase;
    if ((phase & LightPhase_Released) != 0) view->mPhase = phase & ~(u32)LightPhase_Mask;
    return user;
}
/* 027B1498 */
void agl::LightPrepass::releaseTemporary(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    LightView* view = clampedElement((LightView*)mViews, mViewCount, index);
    TextureData* texture = view->mTemporaryTexture;
    if (texture != NULL) {
        releaseTexture_l(sDynamicTextureResources_l, texture);
        view->mTemporaryTexture = NULL;
        u32 dirty = mDirty;
        if ((dirty & LightDirty_Tracking) != 0) mDirty = dirty | LightDirty_Changed;
    }
    u32 phase = view->mPhase;
    if ((phase & LightPhase_Rendered) != 0) view->mPhase = phase & ~(u32)LightPhase_Mask;
    else view->mPhase = phase | LightPhase_Released;
}
/* 027B1548 */
void agl::LightPrepass::setPoint(u32 index, const LightVector* position, const LightColorBits* color, u32 useFalloff,
                                 const LightColorBits* falloff, u32 castsShadow, u32 visible, u8 dynamic, f64 radius,
                                 f64 intensity, f64 attenuation) {
    gabi::AutoFrame frame{gabi::current_function()};
    ParameterCollection* slot = mCollections[LightCollection_Point];
    if (sPointExTypeGuard_l == 0) {
        sPointExTypeGuard_l = 1;
        sPointExType_l.mTable = &sPointExTypeMethods;
    }
    CompactCollection* points = (CompactCollection*)castCollection(slot, &sPointExType_l);
    u32 capacity = points->mCapacity;
    f32 x = position->x;
    CompactRecord* record = clampedElement((CompactRecord*)points->mRecords, capacity, index);
    record->mPosition[0] = x;
    record->mPosition[1] = (f32)position->y;
    f32 z = position->z;
    record->mRadius = storedSingle(radius);
    record->mPosition[2] = z;
    storeFloatBits(&record->mColor[0], color->red);
    storeFloatBits(&record->mColor[1], color->green);
    u32 blue = color->blue;
    u16 flags = record->mFlags;
    storeFloatBits(&record->mColor[2], blue);
    u32 alpha = color->alpha;
    record->mAttenuation = storedSingle(attenuation);
    storeFloatBits(&record->mColor[3], alpha);
    record->mIntensity = storedSingle(intensity);
    flags = useFalloff != 0 ? flags | PointRecord_Falloff : flags & ~PointRecord_Falloff;
    flags = castsShadow != 0 ? flags | PointRecord_Shadow : flags & ~PointRecord_Shadow;
    flags = visible != 0 ? flags | PointRecord_Visible : flags & ~PointRecord_Visible;
    flags = dynamic != 0 ? flags | PointRecord_Dynamic : flags & ~PointRecord_Dynamic;
    record->mFlags = flags;
    record->mMask = 0xFFFFFFFF;
    if (useFalloff != 0) {
        storeFloatBits(&record->mFalloff[0], falloff->red);
        storeFloatBits(&record->mFalloff[1], falloff->green);
        storeFloatBits(&record->mFalloff[2], falloff->blue);
        storeFloatBits(&record->mFalloff[3], falloff->alpha);
    }
}
/* 027B1734 */
void agl::LightPrepass::setDirected(u32 index, const LightVector* position, const LightVector* direction,
                                    const LightColorBits* color, u32 visible, const LightColorBits* falloff, f64 inner,
                                    f64 outer, f64 intensity, f64 attenuation) {
    gabi::AutoFrame frame{gabi::current_function()};
    ParameterCollection* slot = mCollections[LightCollection_Directed];
    if (sSelectionExTypeGuard_l == 0) {
        sSelectionExTypeGuard_l = 1;
        sSelectionExType_l.mTable = &sSelectionExTypeMethods;
    }
    ExtendedCollection* directed = (ExtendedCollection*)castCollection(slot, &sSelectionExType_l);
    u32 capacity = directed->mCapacity;
    ExtendedRecord* records = directed->mRecords;
    f32 x = position->x;
    ExtendedRecord* record = clampedElement(records, capacity, index);
    record->mPosition[0] = x;
    record->mPosition[1] = (f32)position->y;
    record->mPosition[2] = (f32)position->z;
    record->mDirection[0] = (f32)direction->x;
    record->mDirection[1] = (f32)direction->y;
    f32 z = direction->z;
    record->mShape[0] = storedSingle(inner);
    record->mDirection[2] = z;
    record->mShape[1] = storedSingle(outer);
    storeFloatBits(&record->mColor[0], color->red);
    storeFloatBits(&record->mColor[1], color->green);
    storeFloatBits(&record->mColor[2], color->blue);
    storeFloatBits(&record->mColor[3], color->alpha);
    storeFloatBits(&record->mFalloff[0], falloff->red);
    storeFloatBits(&record->mFalloff[1], falloff->green);
    u32 blue = falloff->blue;
    u8 flags = record->mFlags;
    storeFloatBits(&record->mFalloff[2], blue);
    u32 alpha = falloff->alpha;
    record->mShape[3] = storedSingle(attenuation);
    storeFloatBits(&record->mFalloff[3], alpha);
    record->mShape[2] = storedSingle(intensity);
    record->mFlags = visible != 0 ? flags | DirectedRecord_Visible : flags & ~DirectedRecord_Visible;
    ExtendedValue* values = record->mValues;
    ExtendedValue* end = guestElement(values, record->mValueCount);
    record->mMode = 1;
    record->mMask = 0xFFFFFFFF;
    record->mThreshold = 0.5f;
    for (ExtendedValue* value = values; value != end; ++value) value->mViewState = 0;
    normalizeDirection_l(record->mDirection);
}
/* WWHD glue
str 10169678
str 10169660
str 101695F0
str 10169600
str 10169628
data sPointExTypeGuard_l 101FD808
data sPointExType_l 101FDFE4
data sPointExTypeMethods 10168F74
data sSelectionExTypeGuard_l 101FD80C
data sSelectionExType_l 101FDFE8
data sSelectionExTypeMethods 10168F84
data sCurveExTypeGuard_l 101FD814
data sCurveExType_l 101FDFE0
data sCurveExTypeMethods 10168FA4
data sPointCollectionBaseMethods 1016900C
data sPointCollectionMethods 1016AD80
data sDirectedCollectionBaseMethods 10169184
data sDirectedCollectionMethods 1016AEF8
data sSpatialCollectionBaseMethods 101692FC
data sSpatialCollectionMethods 1016B070
data sPairedCollectionBaseMethods 10169474
data sPairedCollectionMethods 1016B1E8
data sSpecularKeys 10169648
data sLightPrepassMethods 101696A4
data sCurveParameterMethods_l 101696BC
data sPropertyDisposerMethods_l 10168F54
data sPropertyObjectMethods_l 10168F64
ext 0273AD10 void* allocateLightStorage(u32)
ext 0273AF40 void deleteLightStorage(void*)
ext 0273AFC8 void deleteLightArray(void*)
ext 0273B050 void* allocateFromHeap_l(u32,sead::RegistryHeap*,s32)
ext 0273B0D4 void* allocateAlignedFromHeap_l(u32,sead::RegistryHeap*,s32)
ext 028F521C void clearBytes(void*,u32)
ext 028EFFD0 void constructArray(void*,u32,u32,u32)
ext 0274FDBC sead::CompactRenderState* sead::CompactRenderState::create(sead::CompactRenderState*)
ext 02750250 sead::RenderState* sead::RenderState::create(sead::RenderState*)
ext 0274FEB0 void sead::CompactRenderState::apply()
ext 02750370 void sead::RenderState::apply()
ext 0274F5F0 sead::Viewport* sead::Viewport::createSurface(sead::Viewport*,const sead::RenderVector2*)
ext 0274F964 void sead::Viewport::apply(const sead::RenderSurface*) const
ext 0274D690 void sead::activateFrameBuffer(sead::DisplayFrame*)
ext 0272D680 void combineRegistryMatrices(sead::RegistryMatrix44*,const sead::RegistryMatrix44*,const sead::RegistryMatrix34*)
ext 028E90D4 void copyRegistryMatrix_l(const sead::RegistryMatrix34*,sead::RegistryMatrix34*)
ext 025155D8 void normalizeDirection_l(be<f32>*)
ext 0278D0D0 void createViewCamera_l(agl::LightViewCamera_l*)
ext 0278CB8C void setViewCamera_l(agl::LightView*,const void*,const void*)
ext 0278D07C void setViewProjection_l(agl::LightView*,void*)
ext 027C6C54 void createPostprocess_l(void*)
ext 027C6E10 void destroyPostprocess_l(void*,u32)
ext 027A03A0 void initializeFacade_l(agl::CollectionFacade_l*)
ext 027A063C void configureFacade_l(agl::CollectionFacade_l*,u32,const sead::SafeText*,sead::RegistryHeap*)
ext 0279E710 void initializeParameter_l(agl::CurveParameter_l*,const sead::SafeText*,const sead::SafeText*,u32)
ext 02785CB4 void setParameterAnnotation_l(void*,const sead::SafeText*)
ext 027CBD54 void finishLightResources_l()
ext 027B4FEC void agl::CurveParameter_l::initializeCurves()
ext 027B50BC void agl::CurveParameter_l::setCurve(u32,u32,const be<f32>*,u32)
ext 027B5108 f64 evaluateCurve_l(agl::CurveParameter_l*,u32,f64)
ext 027B6030 void agl::replaceBitmapMip(agl::SurfaceData_l*,const void*,u32)
ext 027B95B4 agl::RenderBuffer* agl::RenderBuffer::construct()
ext 027B9694 void agl::RenderBuffer::destroy(u32)
ext 027B6CB8 agl::ColorSurface* agl::ColorSurface::construct()
ext 027B6D64 void agl::ColorSurface::invalidate()
ext 027B6E64 agl::DepthSurface* agl::DepthSurface::construct()
ext 027BEF54 void agl::UniformBuffer::clear()
ext 027BECEC void agl::UniformBuffer::allocateFormat(u32,void*)
ext 027BED84 void agl::UniformBuffer::appendField(u32,u32)
ext 027BF048 void agl::UniformBuffer::writeWords(void*,u32,const be<u32>*,u32,s32) const
ext 027BDEB4 void agl::TextureSampler::copyTexture(const agl::TextureData*)
ext 027BE930 void agl::TextureData::configure(u32,u32,u32,u32,u32,s32,u32,u32)
ext 027BF734 agl::VertexInputSet* agl::VertexInputSet::create(agl::VertexInputSet*)
ext 027B997C void clearRenderBuffer_l(agl::RenderBuffer*,u32,u32,const agl::LightColorBits*,u32,f64)
ext 0279FD18 agl::TextureData* acquireTexture_l(void*,const sead::SafeText*,u32,u32,u32,u32,u32,u32,u32,u32,u32)
ext 027A0040 void releaseTexture_l(void*,agl::TextureData*)
ext 025C0848 void attachColorTexture_l(agl::ColorSurface*,agl::TextureData*,u32,u32)
ext 025C0928 void attachDepthTexture_l(agl::ColorSurface*,agl::TextureData*)
*/

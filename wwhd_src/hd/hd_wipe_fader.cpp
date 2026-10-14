/* HD WipeFader draws four mirrored texture quadrants on each output. Private decomp source. */
#include "hd/hd_wipe_fader.h"
static hdWipeFaderHeaderZero_c sWipeFaderHeaderZero;
static hdWipeFaderHeaderDescriptor_c sWipeFaderHeaderDescriptor;
static hdWipeFaderHeaderDescriptor_c sWipeFaderRotationDescriptor;
static hdWipeFaderHeaderDescriptor_c sWipeFaderPositiveRotationDescriptor;
static f32 sWipeFaderMinRotation;
static f32 sWipeFaderMaxRotation;
static u8 sWipeFaderRotationGuard;
static u8 sWipeFaderPositiveRotationGuard;
static void initializeRotationGuard(void* guard);
static void initializePositiveRotationGuard(void* guard);
static u32 sWipeFaderSafeStringVtable;
static u32 sWipeFaderStepVtable;
static u32 sWipeFaderDelegateVtable;
static u32 sWipeFaderDefaultDelegateVtable;
static u32 sWipeFaderActiveDelegateVtable;
static hdRenderSize_c sWipeFaderWipeTvSize;
static hdRenderSize_c sWipeFaderWipeRenderSize;
static void* sWipeFaderWipeResourceManager;
static void* sWipeFaderWipeRenderContext;
static hdMaskScreenMgr_l* sWipeFaderWipeScreens;
static hdWipeLayerList_l* sWipeFaderWipeLayers;
static f32 sWipeFaderWipeScale;
static u8 sWipeFaderWipeEnabled;
static void* sWipeFaderWipeRenderer;
static u8 sWipeFaderWipeDebugRenderer;
static void* allocateWipeFader(u32 size);
static void deleteWipeFader(hdWipeFader_c* fader);
static void constructWipeStep(hdWipeStep_c* step);
static void destroyWipeStep(hdWipeStep_c* step, s32 flags);
static void constructWipeTexture(hdFilterTexture_c* texture);
static void constructWipeTextureDescriptor(hdWipeTextureDescriptor_c* descriptor);
static void destroyWipeTexture(hdFilterTexture_c* texture, s32 flags);
static void* loadWipeResource(void* manager, SafeString* archive, SafeString* name, u32 flags);
static void beginWipeResource(void* context);
static void endWipeResource(void* context);
static void loadWipeTextureDescriptor(hdWipeTextureDescriptor_c* descriptor, void* resource, const char* name);
static void assignWipeTexture(hdFilterTexture_c* texture, hdWipeTextureDescriptor_c* descriptor);
static void resetWipeProgress();
static void drawDrcWipe(hdWipeFader_c* fader, hdMaskRenderInfo_c* info);
static void addWipeStep(void* layer, u32 index, hdWipeStep_c* step);
static void calculateWipeBounds(hdMaskBounds_c* bounds, hdMaskViewport_l* viewport);
static void applyWipeViewport(hdMaskBounds_c* bounds, hdMaskViewport_l* viewport);
static void constructWipeProjection(hdMaskProjection_c* projection, hdMaskBounds_c* bounds, f32 nearPlane, f32 farPlane);
static void constructWipeCamera(hdMaskCamera_c* camera, hdMaskProjection_c* projection);
static void constructWipeState(hdWipeDrawState_c* state);
static void applyWipeState(hdWipeDrawState_c* state);
static hdFilterDebugRenderer_l* setupWipeDebugRenderer(void* renderer);
static void makeWipeQuadMatrix(hdMaskMatrix_c* matrix, cXyz* scale, cXyz* translation);
static void concatenateWipeMatrix(hdMaskCamera_c* camera, hdMaskMatrix_c* matrix, hdMaskMatrix_c* result);
static void* getWipeProjectionParameters(hdMaskProjection_c* projection);
static u32 drawWipeTexture(void* renderer, hdFilterTexture_c* texture, hdMaskMatrix_c* matrix, void* projection, hdWipeTextureScale_c* scale, u32 quadrant, u32 slot);
static void destroyWipeCamera(hdMaskCamera_c* camera, s32 flags);
static void destroyWipeProjection(hdMaskProjection_c* projection, s32 flags);
/* 0273971C */
static hdRenderSize_c* getWipeTvSize() { return &sWipeFaderWipeTvSize; }
/* 02739728 */
static hdRenderSize_c* getWipeRenderSize() { return &sWipeFaderWipeRenderSize; }
/* inlined into constructWipeFader */
static void initializeWipeStep(hdWipeStep_c* step) {
    constructWipeStep(step);
    SafeString* name = gabi::at<SafeString>(gabi::ea(step)+offsetof(hdWipeStep_c, mName));
    if (name == NULL) name = (SafeString*)allocateWipeFader(sizeof(SafeString));
    if (name != NULL) {
        name->mStringTop = gabi::guest_addr("");
        name->__vtbl = gabi::ea(&sWipeFaderSafeStringVtable);
    }
    step->mTarget = 0;
    hdWipeDelegate_c* delegate = gabi::at<hdWipeDelegate_c>(gabi::ea(step)+offsetof(hdWipeStep_c, mDelegate));
    step->mVtable = gabi::ea(&sWipeFaderStepVtable);
    step->mFlags = 0;
    if (delegate != NULL) {
        hdWipeDelegate_c* object = delegate;
        if (object == NULL) object = (hdWipeDelegate_c*)allocateWipeFader(sizeof(hdWipeDelegate_c));
        if (object != NULL) {
            object->mFunction = 0;
            object->mVtable = gabi::ea(&sWipeFaderDelegateVtable);
            object->mAdjustment = 0;
            object->mVirtualSlot = 0;
            object->mpOwner = NULL;
        }
        delegate->mVtable = gabi::ea(&sWipeFaderDefaultDelegateVtable);
    }
}
/* 02739734 */
static hdWipeFader_c* constructWipeFader(hdWipeFader_c* fader) {
    if (fader == NULL) {
        fader = (hdWipeFader_c*)allocateWipeFader(sizeof(hdWipeFader_c));
        if (fader == NULL) return NULL;
    }
    initializeWipeStep(&fader->mTvStep);
    initializeWipeStep(&fader->mDrcStep);
    fader->mTvEnabled = 0;
    fader->mDrcEnabled = 0;
    constructWipeTexture(&fader->mTexture);
    constructWipeTextureDescriptor(&fader->mTextureDescriptor);
    return fader;
}
/* 02739898 */
static void destroyWipeFader(hdWipeFader_c* fader, u32 flags) {
    if (fader != NULL) {
        destroyWipeTexture(&fader->mTexture, 2);
        destroyWipeStep(&fader->mDrcStep, 2);
        destroyWipeStep(&fader->mTvStep, 2);
        if (flags & 1) deleteWipeFader(fader);
    }
}
/* 02739904 */
void hdWipeFader_c::loadTexture() {
    SafeString archive;
    SafeString name;
    archive.__vtbl = gabi::ea(&sWipeFaderSafeStringVtable);
    name.__vtbl = gabi::ea(&sWipeFaderSafeStringVtable);
    void* manager = sWipeFaderWipeResourceManager;
    name.mStringTop = gabi::guest_addr("Misc.bfres");
    archive.mStringTop = gabi::guest_addr("Misc");
    void* resource = loadWipeResource(manager, &archive, &name, 0);
    beginWipeResource(sWipeFaderWipeRenderContext);
    loadWipeTextureDescriptor(&mTextureDescriptor, resource, "wipe_00");
    endWipeResource(sWipeFaderWipeRenderContext);
    bool same = true;
    /* Descriptor identity fields, in binary comparison order. Allocation and mip
     * addresses are deliberately handled separately below. */
    const u32 comparedWords[] = {hdWipeImageWidth, hdWipeImageHeight, hdWipeImageDepth,
        hdWipeImageMipCount, hdWipeImageFormat, hdWipeImageSampleMode,
        hdWipeImageAlignment, hdWipeImageSwizzle, hdWipeImageUsage};
    for (u32 index : comparedWords) {
        if (mTexture.mImageWords[index] != mTextureDescriptor.mWords[index]) { same = false; break; }
    }
    if (!same) {
        assignWipeTexture(&mTexture, &mTextureDescriptor);
        u8 flags = mTexture.mFlags;
        mTexture.mSamplerParameters[3] = 2;
        mTexture.mSamplerParameters[4] = 2;
        mTexture.mFlags = flags | 2;
    } else {
        u32 image = mTextureDescriptor.mWords[12];
        u32 mipmap = mTextureDescriptor.mWords[10];
        mTexture.mMipWords[12] = image;
        mTexture.mMipWords[10] = mipmap;
        mTexture.mImageWords[10] = mipmap;
        mTexture.mImageWords[12] = image;
        u8 flags = mTexture.mFlags;
        mTexture.mSamplerParameters[4] = 2;
        mTexture.mSamplerParameters[3] = 2;
        mTexture.mFlags = flags | 2;
    }
}
/* 02739A90 */
static void resetWipeFader() { resetWipeProgress(); }
/* 02739A94 */
void hdWipeFader_c::draw(hdMaskRenderInfo_c* info) {
    hdMaskScreenMgr_l* screens = sWipeFaderWipeScreens;
    u32 screen = info->mScreen;
    hdMaskViewport_l* viewport = &screens->mScreens[screen].mViewport;
    hdMaskBounds_c bounds;
    calculateWipeBounds(&bounds, viewport);
    applyWipeViewport(&bounds, viewport);
    hdMaskProjection_c projection;
    constructWipeProjection(&projection, &bounds, 0.0f, 1.0f);
    hdMaskCamera_c camera;
    constructWipeCamera(&camera, &projection);
    hdWipeDrawState_c state;
    constructWipeState(&state);
    state.mWriteBlue = 1;
    state.mEnabled = 1;
    state.mWriteAlpha = 0;
    state.mWriteGreen = 1;
    state.mAlphaTestEnabled = 1;
    state.mDepthMode = 7;
    state.mAlphaFunction = 4;
    state.mWriteRed = 1;
    state.mAlphaEnabled = 0;
    state.mAlphaReference = 0.1f;
    state.mBlend = 0;
    applyWipeState(&state);
    hdFilterDebugRenderer_l* debug = setupWipeDebugRenderer(&sWipeFaderWipeDebugRenderer);
    f32 wipeScale = sWipeFaderWipeScale;
    u32 slot = debug->mTarget;
    hdWipeTextureScale_c textureScale;
    f32 reciprocal = wipeScale == 0.0f ? 0.0f : 1.0f / wipeScale;
    textureScale.mX = reciprocal;
    textureScale.mY = reciprocal;
    f32 extent = viewport->mExtent;
    f32 negativeHalf = gabi::fmuls_ppc(-0.5f, extent);
    f32 positiveHalf = gabi::fmuls_ppc(0.5f, extent);
    cXyz scale;
    cXyz translation;
    hdMaskMatrix_c matrix;
    for (u32 quadrant = 0; quadrant < 4; ++quadrant) {
        scale.x = extent;
        scale.y = extent;
        scale.z = 1.0f;
        translation.x = quadrant == 0 || quadrant == 2 ? negativeHalf : positiveHalf;
        translation.y = quadrant < 2 ? positiveHalf : negativeHalf;
        translation.z = 0.0f;
        makeWipeQuadMatrix(&matrix, &scale, &translation);
        concatenateWipeMatrix(&camera, &matrix, &matrix);
        void* parameters = getWipeProjectionParameters(&projection);
        void* renderer = sWipeFaderWipeRenderer;
        slot = drawWipeTexture(renderer, &mTexture, &matrix, parameters, &textureScale, quadrant, slot);
    }
    destroyWipeCamera(&camera, 2);
    destroyWipeProjection(&projection, 2);
}
/* 02739E28 */
void hdWipeFader_c::drawIfEnabled(hdMaskRenderInfo_c* info) { if (sWipeFaderWipeEnabled != 0) draw(info); }
/* 02739E3C */
void hdWipeFader_c::initialize() {
    hdWipeDelegate_c* delegate = gabi::at<hdWipeDelegate_c>(gabi::ea(this)+offsetof(hdWipeFader_c, mTvStep)+offsetof(hdWipeStep_c, mDelegate));
    if (delegate != NULL) {
        delegate->mVtable = gabi::ea(&sWipeFaderActiveDelegateVtable);
        delegate->mpOwner = this;
        delegate->mVirtualSlot = 0xFFFF;
        delegate->mFunction = 0x02739E28;
        delegate->mAdjustment = 0;
    }
    mTvStep.mName.mStringTop = gabi::guest_addr("WipeFader");
    mTvStep.mTarget = 2;
    hdWipeLayerList_l* manager = sWipeFaderWipeLayers;
    u32 count = manager->mLayerCount;
    gptr<void>* layers = manager->mpLayers;
    addWipeStep(layers[count > 6 ? 6 : 0], 1, &mTvStep);
    delegate = gabi::at<hdWipeDelegate_c>(gabi::ea(this)+offsetof(hdWipeFader_c, mDrcStep)+offsetof(hdWipeStep_c, mDelegate));
    if (delegate != NULL) {
        delegate->mVtable = gabi::ea(&sWipeFaderActiveDelegateVtable);
        delegate->mpOwner = this;
        delegate->mVirtualSlot = 0xFFFF;
        delegate->mFunction = gabi::guest_fn((const void*)drawDrcWipe);
        delegate->mAdjustment = 0;
    }
    mDrcStep.mName.mStringTop = gabi::guest_addr("WipeFader");
    mDrcStep.mTarget = 2;
    manager = sWipeFaderWipeLayers;
    count = manager->mLayerCount;
    layers = manager->mpLayers;
    addWipeStep(layers[count > 10 ? 10 : 0], 1, &mDrcStep);
    mTvEnabled = 1;
    mDrcEnabled = 1;
}
/* 02739F60 */
static void __sinit_hd_wipe_fader_cpp() {
    for (u32 i = 0; i < 4; ++i) sWipeFaderHeaderZero.mWords[i] = 0;
    __register_global_object(gabi::ea(&sWipeFaderHeaderDescriptor));
    sWipeFaderMinRotation = -3.1415927410125732f;
    sWipeFaderMaxRotation = 3.1415927410125732f;
    initializeRotationGuard(&sWipeFaderRotationGuard);
    __register_global_object(gabi::ea(&sWipeFaderRotationDescriptor));
    initializePositiveRotationGuard(&sWipeFaderPositiveRotationGuard);
    __register_global_object(gabi::ea(&sWipeFaderPositiveRotationDescriptor));
}
/* WWHD glue
 data sWipeFaderSafeStringVtable 10142E88 nocheck
 data sWipeFaderStepVtable 10169E3C nocheck
 data sWipeFaderDelegateVtable 10142E70 nocheck
 data sWipeFaderDefaultDelegateVtable 10142EA0 nocheck
 data sWipeFaderActiveDelegateVtable 10142EB8 nocheck
 data sWipeFaderHeaderZero 1049FB90
 data sWipeFaderHeaderDescriptor 101F8AC0
 data sWipeFaderRotationDescriptor 101F8ACC
 data sWipeFaderPositiveRotationDescriptor 101F8AD8
 data sWipeFaderMinRotation 1049FB84
 data sWipeFaderMaxRotation 1049FB88
 data sWipeFaderRotationGuard 1049FB8C
 data sWipeFaderPositiveRotationGuard 1049FB8D
 ext 028ED6F8 void initializeRotationGuard(void* guard)
 ext 028EAB2C void initializePositiveRotationGuard(void* guard)
 data sWipeFaderWipeTvSize 1049FB6C
 data sWipeFaderWipeRenderSize 1049FB74
 data sWipeFaderWipeResourceManager 101F4F7C
 data sWipeFaderWipeRenderContext 101F8B18
 data sWipeFaderWipeScreens 101F95D0
 data sWipeFaderWipeLayers 101F95D0
 data sWipeFaderWipeScale 101D6160
 data sWipeFaderWipeEnabled 101D616C
 data sWipeFaderWipeRenderer 101F8710
 data sWipeFaderWipeDebugRenderer 104B45C0
 str 10142EC8 "" in 02739734
 str 10142ED4
 str 10142EDC
 str 10142ECC
 str 10142EF4
 ext 0273AD10 void* allocateWipeFader(u32 size)
 ext 0273AF40 void deleteWipeFader(hdWipeFader_c* fader)
 ext 02752A84 void constructWipeStep(hdWipeStep_c* step)
 ext 027C0804 void destroyWipeStep(hdWipeStep_c* step,s32 flags)
 ext 027BDF7C void constructWipeTexture(hdFilterTexture_c* texture)
 ext 027BE6B8 void constructWipeTextureDescriptor(hdWipeTextureDescriptor_c* descriptor)
 ext 027BE2B0 void destroyWipeTexture(hdFilterTexture_c* texture,s32 flags)
 ext 026124B0 void* loadWipeResource(void* manager,SafeString* archive,SafeString* name,u32 flags)
 ext 0274FBF8 void beginWipeResource(void* context)
 ext 0274FCCC void endWipeResource(void* context)
 ext 02773870 void loadWipeTextureDescriptor(hdWipeTextureDescriptor_c* descriptor,void* resource,const char* name)
 ext 027BDEB4 void assignWipeTexture(hdFilterTexture_c* texture,hdWipeTextureDescriptor_c* descriptor)
 ext 0252F5A0 void resetWipeProgress()
 ext 0273A0B4 void drawDrcWipe(hdWipeFader_c* fader,hdMaskRenderInfo_c* info) callback
 ext 027C23F8 void addWipeStep(void* layer,u32 index,hdWipeStep_c* step)
 ext 0274F5F0 void calculateWipeBounds(hdMaskBounds_c* bounds,hdMaskViewport_l* viewport)
 ext 0274F964 void applyWipeViewport(hdMaskBounds_c* bounds,hdMaskViewport_l* viewport)
 ext 0274E364 void constructWipeProjection(hdMaskProjection_c* projection,hdMaskBounds_c* bounds,f32 nearPlane,f32 farPlane)
 ext 0274CE34 void constructWipeCamera(hdMaskCamera_c* camera,hdMaskProjection_c* projection)
 ext 0274FDBC void constructWipeState(hdWipeDrawState_c* state)
 ext 0274FEB0 void applyWipeState(hdWipeDrawState_c* state)
 ext 027F29D4 hdFilterDebugRenderer_l* setupWipeDebugRenderer(void* renderer)
 ext 025C0AC4 void makeWipeQuadMatrix(hdMaskMatrix_c* matrix,cXyz* scale,cXyz* translation)
 ext 028E9108 void concatenateWipeMatrix(hdMaskCamera_c* camera,hdMaskMatrix_c* matrix,hdMaskMatrix_c* result)
 ext 0274D80C void* getWipeProjectionParameters(hdMaskProjection_c* projection)
 ext 0272CDF4 u32 drawWipeTexture(void* renderer,hdFilterTexture_c* texture,hdMaskMatrix_c* matrix,void* projection,hdWipeTextureScale_c* scale,u32 quadrant,u32 slot)
 ext 0274CF04 void destroyWipeCamera(hdMaskCamera_c* camera,s32 flags)
 ext 0274E474 void destroyWipeProjection(hdMaskProjection_c* projection,s32 flags)
*/

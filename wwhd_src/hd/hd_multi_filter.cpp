/* HD MultiFilter chooses and draws a postprocessing surface for each output. Private decomp source. */
#include "hd/hd_multi_filter.h"
static u32 sMultiFilterBaseDrawVtable;
static u32 sMultiFilterSafeStringVtable;
static u32 sMultiFilterLayerVtable;
static u32 sMultiFilterDrawVtable;
static u32 sMultiFilterStepVtable;
static u32 sMultiFilterDelegateVtable;
static u32 sMultiFilterTextureVtable;
static u32 sMultiFilterTextureImageVtable;
static hdFilterTargetMgr_l* sMultiFilterTargetManager;
static u32 sMultiFilterOutputMode;
static u8 sMultiFilterFilterOverride;
static void* sMultiFilterRenderManager;
static u8 sMultiFilterDebugRenderer;
static cXyz sMultiFilterQuadScale;
static cXyz sMultiFilterQuadTranslation;
static u8 sMultiFilterQuadColor;
static f32 sMultiFilterNearDistance;
static f32 sMultiFilterFarDistance;
static f32 sMultiFilterDrcNearDistance;
static f32 sMultiFilterDrcFarDistance;
static hdFilterHeaderZero_c sMultiFilterHeaderZero;
static f32 sMultiFilterMinRotation;
static f32 sMultiFilterMaxRotation;
static u8 sMultiFilterRotationGuard;
static u8 sMultiFilterPositiveRotationGuard;
static hdFilterGlobalDescriptor_c sMultiFilterHeaderDescriptor;
static hdFilterGlobalDescriptor_c sMultiFilterRotationDescriptor;
static hdFilterGlobalDescriptor_c sMultiFilterPositiveRotationDescriptor;
static void initializeRotationGuard(void* guard);
static void initializePositiveRotationGuard(void* guard);
static void* allocateMultiFilter(u32 size);
static void deleteMultiFilter(hdMultiFilter_c* layer);
static void constructMultiFilterBase(hdMultiFilter_c* layer, SafeString* name);
static void destroyMultiFilterBase(hdMultiFilter_c* layer, s32 flags);
static void constructFilterSurface(hdFilterSurface_c* surface);
static void destroyFilterSurface(hdFilterSurface_c* surface, s32 flags);
static void initializeFilterSurface(hdFilterSurface_c* surface, void* heap);
static hdFilterTargetArg_c* getFilterTarget(hdFilterTargetArg_c* argument, void* target);
static void setFilterTarget(hdFilterBinding_l* binding, hdFilterTargetArg_c* argument);
static void constructFilterStep(hdFilterStep_c* step);
static void addFilterStep(hdMultiFilter_c* layer, hdFilterStep_c* step);
static BOOL hasRenderPass(void* manager, u32 pass);
static void prepareRenderPass(void* manager, u32 pass, hdFilterRenderInfo_c* info, u32 enabled);
static void clearRenderPass(void* manager, u32 pass);
static void finishRenderPass(void* manager, u32 pass, hdFilterRenderInfo_c* info);
static hdFilterDebugRenderer_l* setupFilterDebugRenderer(void* renderer);
static u32 beginFilterSurface(hdFilterSurface_c* surface, void* target, u32 slot);
static void resetFilterViewport(hdFilterViewport_l* viewport);
static void applyFilterViewport(void* bounds, hdFilterViewport_l* viewport);
static void calculateFilterBounds(hdFilterBounds_c* bounds, hdFilterViewport_l* viewport);
static void constructFilterState(hdFilterState_c* state);
static void applyFilterState(hdFilterState_c* state);
static void makeFilterQuadMatrix(hdFilterMatrix_c* matrix, cXyz* scale, cXyz* translation);
static void constructFilterProjection(hdFilterProjection_c* projection);
static void* getFilterProjectionMatrix(hdFilterProjection_c* projection);
static u32 drawFilterTexture(hdFilterTexture_c* texture, hdFilterMatrix_c* matrix, void* projection, void* color, u32 slot);
static void flushFilterTarget(void* target);
static void invalidateFilterTarget(void* target);
static void destroyFilterProjection(hdFilterProjection_c* projection, s32 flags);
static void destroyFilterTexture(hdFilterTexture_c* texture, s32 flags);
static void endFilterSurface(hdFilterSurface_c* surface);
/* 027352BC */
static s32 multiFilterNoDraw() { return 0; }
/* 027352C4 */
static s32 multiFilterNoCalc() { return 0; }
/* 02735394 */
static void destroyMultiFilter(hdMultiFilter_c* layer, u32 flags) {
    if (layer != NULL) {
        layer->mDrawVtable = gabi::ea(&sMultiFilterBaseDrawVtable);
        destroyMultiFilterBase(layer, 0);
        if (flags & 1) deleteMultiFilter(layer);
    }
}
/* 027353F4 */
static s32 multiFilterDrawOrder() { return 3; }
/* 027354A4 */
static hdMultiFilter_c* constructMultiFilter(hdMultiFilter_c* layer) {
    if (layer == NULL) {
        layer = (hdMultiFilter_c*)allocateMultiFilter(sizeof(hdMultiFilter_c));
        if (layer == NULL) return NULL;
    }
    SafeString name;
    name.__vtbl = gabi::ea(&sMultiFilterSafeStringVtable);
    name.mStringTop = gabi::guest_addr("MultiFilter");
    constructMultiFilterBase(layer, &name);
    layer->mVtable = gabi::ea(&sMultiFilterLayerVtable);
    layer->mDrawVtable = gabi::ea(&sMultiFilterDrawVtable);
    constructFilterSurface(&layer->mTvSurface);
    constructFilterSurface(&layer->mDrcSurface);
    layer->mpActiveSurface = NULL;
    return layer;
}
/* 02735538 */
void hdMultiFilter_c::setRenderTarget(void* target) {
    if (target == NULL) {
        mDrcSurface.mEnabled = 0;
        mTvSurface.mEnabled = 0;
        return;
    }
    hdFilterTargetMgr_l* manager = sMultiFilterTargetManager;
    hdFilterTargetArg_c argument;
    hdFilterTargetArg_c binding;
    if (target == manager->mpDrcTarget.get()) {
        hdFilterTargetArg_c* result = getFilterTarget(&argument, target);
        binding.mTarget = result->mTarget;
        setFilterTarget(&mDrcSurface.mBinding, &binding);
    } else {
        hdFilterTargetArg_c* result = getFilterTarget(&argument, target);
        binding.mTarget = result->mTarget;
        setFilterTarget(&mTvSurface.mBinding, &binding);
    }
}
/* 027355E8 */
void hdMultiFilter_c::initialize(void* heap) {
    initializeFilterSurface(&mTvSurface, heap);
    u32 target = mTarget;
    mTvSurface.mEnabled = 0;
    if (target != 4) {
        initializeFilterSurface(&mDrcSurface, heap);
        mDrcSurface.mEnabled = 0;
    }
    hdFilterStep_c* step = (hdFilterStep_c*)allocateMultiFilter(sizeof(hdFilterStep_c));
    if (step != NULL) {
        constructFilterStep(step);
        SafeString* name = gabi::at<SafeString>(gabi::ea(step)+offsetof(hdFilterStep_c, mName));
        if (name == NULL) name = (SafeString*)allocateMultiFilter(sizeof(SafeString));
        if (name != NULL) {
            name->__vtbl = gabi::ea(&sMultiFilterSafeStringVtable);
            name->mStringTop = gabi::guest_addr("");
        }
        step->mFlags = 0;
        step->mVtable = gabi::ea(&sMultiFilterStepVtable);
        if (u32(gabi::ea(step)+offsetof(hdFilterStep_c, mDelegateVtable)) != 0) {
            step->mpLayer = this;
            step->mDelegateAdjustment = 0x0000FFFF;
            step->mDelegateVtable = gabi::ea(&sMultiFilterDelegateVtable);
            step->mDrawFunction = 0x02735BA4;
        }
        step->mTarget = 2;
        step->mName.mStringTop = gabi::guest_addr("LayerMultiFilter");
    }
    addFilterStep(this, step);
}
/* 027356FC */
void hdMultiFilter_c::prepare(hdFilterRenderInfo_c* info) {
    u32 target = info->mTarget;
    u32 outputMode = sMultiFilterOutputMode;
    void* manager = sMultiFilterRenderManager;
    if (target == 4) {
        if (!hasRenderPass(manager, 7)) {
            hdFilterSurface_c* surface = NULL;
            if (outputMode == 2 || sMultiFilterFilterOverride != 0) surface = &mTvSurface;
            mpActiveSurface = surface;
            if (surface != NULL) surface->mEnabled = 1;
        } else {
            prepareRenderPass(manager, 7, info, 1);
        }
    } else if (target == 8) {
        if (!hasRenderPass(manager, 8)) {
            hdFilterSurface_c* surface = NULL;
            if (outputMode == 4) surface = &mDrcSurface;
            else if (sMultiFilterFilterOverride != 0) surface = &mTvSurface;
            mpActiveSurface = surface;
            if (surface != NULL) surface->mEnabled = 1;
        } else {
            prepareRenderPass(manager, 8, info, 1);
        }
    } else return;
    target = info->mTarget;
    if (target == 4) {
        if (outputMode != 2 && sMultiFilterFilterOverride == 0) clearRenderPass(manager, 7);
    } else if (target == 8) {
        if (outputMode != 4 && sMultiFilterFilterOverride == 0) clearRenderPass(manager, 8);
    }
}
/* inlined into hdMultiFilter_c::draw */
static void copyFilterTexture(hdFilterTexture_c* destination, hdFilterTexture_c* source) {
    destination->mVtable = gabi::ea(&sMultiFilterTextureVtable);
    destination->mTextureVtable = gabi::ea(&sMultiFilterTextureImageVtable);
    for (u32 i = 0; i < 30; ++i) destination->mImageWords[i] = source->mImageWords[i];
    for (u32 i = 0; i < 4; ++i) destination->mChannels[i] = source->mChannels[i];
    for (u32 i = 0; i < 4; ++i) destination->mImageInfo[i] = source->mImageInfo[i];
    destination->mEnabled = source->mEnabled;
    destination->mHasMipmaps = source->mHasMipmaps;
    for (u32 i = 0; i < 6; ++i) destination->mSamplerWords[i] = source->mSamplerWords[i];
    for (u32 i = 0; i < 40; ++i) destination->mMipWords[i] = source->mMipWords[i];
    destination->mSamplerState = source->mSamplerState;
    for (u32 i = 0; i < 16; ++i) destination->mSamplerParameters[i] = source->mSamplerParameters[i];
    u8 flags = source->mFlags;
    destination->mSamplerParameters[2] = 0;
    destination->mSamplerParameters[1] = 0;
    destination->mSamplerParameters[0] = 0;
    destination->mFlags = flags | 4;
}
/* 027358B8 */
void hdMultiFilter_c::draw(hdFilterRenderInfo_c* info) {
    hdFilterSurface_c* surface = mpActiveSurface;
    if (surface == NULL || surface->mEnabled == 0) return;
    hdFilterDebugRenderer_l* debug = setupFilterDebugRenderer(&sMultiFilterDebugRenderer);
    hdFilterViewport_l* viewport = info->mpViewport;
    u32 slot = debug->mTarget;
    surface = mpActiveSurface;
    void* target = viewport->mpTarget;
    u32 nextSlot = beginFilterSurface(surface, target, slot);
    surface = mpActiveSurface;
    hdFilterTexture_c* source = surface->mHasTexture != 0 ? &surface->mTexture : NULL;
    u32 finalSlot = nextSlot;
    if (source != NULL) {
        hdFilterTexture_c texture;
        copyFilterTexture(&texture, source);
        resetFilterViewport(info->mpViewport);
        void* boundsPointer = info->mpViewportBounds;
        hdFilterViewport_l* currentViewport = info->mpViewport;
        applyFilterViewport(boundsPointer, currentViewport);
        hdFilterBounds_c bounds;
        calculateFilterBounds(&bounds, viewport);
        applyFilterViewport(&bounds, viewport);
        hdFilterState_c state;
        constructFilterState(&state);
        state.mEnabled = 0;
        state.mBlend = 0;
        state.mAlphaEnabled = 0;
        applyFilterState(&state);
        hdFilterMatrix_c matrix;
        makeFilterQuadMatrix(&matrix, &sMultiFilterQuadScale, &sMultiFilterQuadTranslation);
        hdFilterProjection_c projection;
        constructFilterProjection(&projection);
        void* projectionMatrix = getFilterProjectionMatrix(&projection);
        finalSlot = drawFilterTexture(&texture, &matrix, projectionMatrix, &sMultiFilterQuadColor, nextSlot);
        flushFilterTarget(viewport->mpTarget);
        invalidateFilterTarget(viewport->mpTarget);
        destroyFilterProjection(&projection, 2);
        destroyFilterTexture(&texture, 2);
        surface = mpActiveSurface;
    }
    endFilterSurface(surface);
    u32 targetId = info->mTarget;
    void* manager = sMultiFilterRenderManager;
    if (targetId == 4) finishRenderPass(manager, 7, info);
    else if (targetId == 8) finishRenderPass(manager, 8, info);
    debug = setupFilterDebugRenderer(&sMultiFilterDebugRenderer);
    debug->mTarget = finalSlot;
    debug->mFlags = 0;
    surface = mpActiveSurface;
    surface->mEnabled = 0;
    mpActiveSurface = NULL;
}
/* 02735BA4 */
void hdMultiFilter_c::execute(hdFilterRenderInfo_c* info) {
    u32 pass = info->mPass;
    if (pass == 0) prepare(info);
    else if (pass == 1) draw(info);
}
/* 02735BC0 */
static void __sinit_hd_multi_filter_cpp() {
    for (u32 i = 0; i < 4; ++i) sMultiFilterHeaderZero.mWords[i] = 0;
    __register_global_object(gabi::ea(&sMultiFilterHeaderDescriptor));
    sMultiFilterMinRotation = -3.1415927410125732f;
    sMultiFilterMaxRotation = 3.1415927410125732f;
    initializeRotationGuard(&sMultiFilterRotationGuard);
    __register_global_object(gabi::ea(&sMultiFilterRotationDescriptor));
    initializePositiveRotationGuard(&sMultiFilterPositiveRotationGuard);
    __register_global_object(gabi::ea(&sMultiFilterPositiveRotationDescriptor));
    sMultiFilterNearDistance = 50000.0f;
    sMultiFilterFarDistance = 10000.0f;
    sMultiFilterDrcNearDistance = 50000.0f;
    sMultiFilterDrcFarDistance = 10000.0f;
}
/* WWHD glue
 data sMultiFilterBaseDrawVtable 10142658 nocheck
 data sMultiFilterSafeStringVtable 10142684 nocheck
 data sMultiFilterLayerVtable 10142738 nocheck
 data sMultiFilterDrawVtable 101427E0 nocheck
 data sMultiFilterStepVtable 10169E3C nocheck
 data sMultiFilterDelegateVtable 101426DC nocheck
 data sMultiFilterTextureVtable 10169D2C nocheck
 data sMultiFilterTextureImageVtable 101426AC nocheck
 data sMultiFilterHeaderZero 1049FAA4
 data sMultiFilterMinRotation 1049FA88
 data sMultiFilterMaxRotation 1049FA8C
 data sMultiFilterRotationGuard 1049FAA0
 data sMultiFilterPositiveRotationGuard 1049FAA1
 data sMultiFilterHeaderDescriptor 101F8934
 data sMultiFilterRotationDescriptor 101F8940
 data sMultiFilterPositiveRotationDescriptor 101F894C
 ext 028ED6F8 void initializeRotationGuard(void* guard)
 ext 028EAB2C void initializePositiveRotationGuard(void* guard)
 data sMultiFilterTargetManager 101F8664
 data sMultiFilterOutputMode 101F8348
 data sMultiFilterFilterOverride 101F8351
 data sMultiFilterRenderManager 101F9968
 data sMultiFilterDebugRenderer 104B45C0
 data sMultiFilterQuadScale 104A0958
 data sMultiFilterQuadTranslation 104A0928
 data sMultiFilterQuadColor 104A01EC
 data sMultiFilterNearDistance 1049FA90
 data sMultiFilterFarDistance 1049FA98
 data sMultiFilterDrcNearDistance 1049FA94
 data sMultiFilterDrcFarDistance 1049FA9C
 str 10142704
 str 10142710
 str 10142680 "" in 027355E8
 ext 0273AD10 void* allocateMultiFilter(u32 size)
 ext 0273AF40 void deleteMultiFilter(hdMultiFilter_c* layer)
 ext 027340D8 void constructMultiFilterBase(hdMultiFilter_c* layer, SafeString* name)
 ext 02734248 void destroyMultiFilterBase(hdMultiFilter_c* layer, s32 flags)
 ext 0279DFE0 void constructFilterSurface(hdFilterSurface_c* surface)
 ext 0279D92C void destroyFilterSurface(hdFilterSurface_c* surface, s32 flags)
 ext 0279DDA4 void initializeFilterSurface(hdFilterSurface_c* surface, void* heap)
 ext 027A7558 hdFilterTargetArg_c* getFilterTarget(hdFilterTargetArg_c* argument, void* target)
 ext 0279A308 void setFilterTarget(hdFilterBinding_l* binding, hdFilterTargetArg_c* argument)
 ext 02752A84 void constructFilterStep(hdFilterStep_c* step)
 ext 027C2450 void addFilterStep(hdMultiFilter_c* layer, hdFilterStep_c* step)
 ext 027F81A4 BOOL hasRenderPass(void* manager, u32 pass)
 ext 027F8974 void prepareRenderPass(void* manager, u32 pass, hdFilterRenderInfo_c* info, u32 enabled)
 ext 027F88C8 void clearRenderPass(void* manager, u32 pass)
 ext 027F84A4 void finishRenderPass(void* manager, u32 pass, hdFilterRenderInfo_c* info)
 ext 027F29D4 hdFilterDebugRenderer_l* setupFilterDebugRenderer(void* renderer)
 ext 0279DB64 u32 beginFilterSurface(hdFilterSurface_c* surface, void* target, u32 slot)
 ext 0274D690 void resetFilterViewport(hdFilterViewport_l* viewport)
 ext 0274F964 void applyFilterViewport(void* bounds, hdFilterViewport_l* viewport)
 ext 0274F5F0 void calculateFilterBounds(hdFilterBounds_c* bounds, hdFilterViewport_l* viewport)
 ext 0274FDBC void constructFilterState(hdFilterState_c* state)
 ext 0274FEB0 void applyFilterState(hdFilterState_c* state)
 ext 025C0AC4 void makeFilterQuadMatrix(hdFilterMatrix_c* matrix, cXyz* scale, cXyz* translation)
 ext 0274E1C8 void constructFilterProjection(hdFilterProjection_c* projection)
 ext 0274D83C void* getFilterProjectionMatrix(hdFilterProjection_c* projection)
 ext 027AAD8C u32 drawFilterTexture(hdFilterTexture_c* texture, hdFilterMatrix_c* matrix, void* projection, void* color, u32 slot)
 ext 027B6D64 void flushFilterTarget(void* target)
 ext 027BE794 void invalidateFilterTarget(void* target)
 ext 0274E474 void destroyFilterProjection(hdFilterProjection_c* projection, s32 flags)
 ext 027BE2B0 void destroyFilterTexture(hdFilterTexture_c* texture, s32 flags)
 ext 0279D8E0 void endFilterSurface(hdFilterSurface_c* surface)
*/

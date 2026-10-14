/* HD Layer3D routes scene targets and render passes. Private decomp source. */
#include "hd/hd_layer_3d.h"
static hdLayer3DHeaderZero_c sLayer3DHeaderZero;
static hdLayer3DHeaderDescriptor_c sLayer3DHeaderDescriptor;
static hdLayer3DHeaderDescriptor_c sLayer3DRotationDescriptor;
static hdLayer3DHeaderDescriptor_c sLayer3DPositiveRotationDescriptor;
static f32 sLayer3DMinRotation;
static f32 sLayer3DMaxRotation;
static u8 sLayer3DRotationGuard;
static u8 sLayer3DPositiveRotationGuard;
static void initializeRotationGuard(void* guard);
static void initializePositiveRotationGuard(void* guard);
static u32 sLayer3dLayerBaseDrawVtable;
static u32 sLayer3dSafeStringVtable;
static u32 sLayer3dLayerVtable;
static u32 sLayer3dDrawVtable;
static u32 sLayer3dPerspectiveName;
static u32 sLayer3dStepVtable;
static u32 sLayer3dDelegateVtable;
static constexpr u32 kLayer3dRenderFunction = 0x02733190;
static u32 sLayer3dViewportName;
static u32 sLayer3dParticleProjectionName;
static hdSceneSize_l sLayer3DTvSize;
static void* allocateSceneLayer(u32 size);
static void constructSceneLayerBase(hdLayer3D_c* layer, SafeString* name);
static void constructScenePrimary(hdScenePrimaryBinding_l* binding);
static void constructScenePacket(hdScenePacket_l* packet);
static void constructSceneSecondary(hdSceneSecondaryBinding_l* binding);
static void constructSceneBlur(hdSceneBlur_l* blur);
static void constructSceneFilter(hdSceneFilter_l* filter);
static void constructScenePostprocess(hdScenePostprocess_l* postprocess);
static void constructSceneDescriptor(void* descriptor);
static void destroyScenePrimary(hdScenePrimaryBinding_l* binding, s32 flags);
static void destroyScenePacket(hdScenePacket_l* packet, s32 flags);
static void destroySceneSecondary(hdSceneSecondaryBinding_l* binding, s32 flags);
static void destroySceneBlur(hdSceneBlur_l* blur, s32 flags);
static void destroySceneFilter(hdSceneFilter_l* filter, s32 flags);
static void destroyScenePostprocess(hdScenePostprocess_l* postprocess, s32 flags);
static void* sLayer3DHeapManager;
static void* allocateSceneObject(u32 size, void* heap, u32 alignment);
static void* constructSceneViewportObject(void* object);
static void* constructSceneColorObject(void* object);
static void* constructSceneDepthObject(void* object);
static void constructSceneStep(hdSceneRenderStep_c* step);
static void addSceneStep(hdLayer3D_c* layer, hdSceneRenderStep_c* step);
static void initializeSceneSecondary(hdSceneSecondaryBinding_l* binding, hdSceneSecondaryArg_c* argument, u32 flags);
static void initializeSceneBlur(hdSceneBlur_l* blur, u32 enabled, void* heap);
static void initializeSceneFilter(hdSceneFilter_l* filter, u32 enabled, void* heap);
static void initializeScenePrimary(hdScenePrimaryBinding_l* primary, u32 enabled, void* heap);
static void constructScenePacketArg(hdScenePacketArg_c* argument);
static void initializeScenePacket(hdScenePacket_l* packet, hdScenePacketArg_c* argument, void* heap);
static void initializeScenePostprocess(hdScenePostprocess_l* postprocess, void* heap, u32 enabled);
static u32 setSceneHeap(void* manager, void* heap);
static void createSceneRenderManager(u32 flags);
static void initializeSceneRenderManager(void* manager, void* heap);
static void destroySceneCamera(hdSceneCamera_l* camera, s32 flags);
static void destroyScenePerspective(hdScenePerspective_l* projection, s32 flags);
static void destroySceneProjection(hdSceneProjection_l* projection, s32 flags);
static void destroySceneLayerBase(void* layer, s32 flags);
static void deleteSceneLayer(void* layer);
static u32 sLayer3DPerspectiveGuard;
static hdSceneType_l sLayer3DPerspectiveType;
static u32 drawScenePacket(hdScenePacket_l* packet, u32 index, hdFilterViewport_l* viewport, hdFilterBounds_c* bounds, void* primary, void* target, void* secondary, u32 slot, u32 flags);
static u32 drawSceneBlur(hdSceneBlur_l* blur, u32 index, hdFilterViewport_l* viewport, u32 slot, f32 nearPlane, f32 farPlane);
static hdSceneScreens_l* sLayer3DScreens;
static u8 sLayer3DDebugRenderer;
static hdFilterDebugRenderer_l* setupSceneDebugRenderer(void* renderer);
static u32 prepareSceneShadow(void* layer, hdFilterRenderInfo_c* info);
static void* sLayer3DRenderManager;
static hdSceneDisplay_l* sLayer3DDisplay;
static hdSceneViewMatrix_c sLayer3DViewMatrix;
static hdSceneColor_c sLayer3DDrawColor;
static void translateSceneView(hdSceneViewMatrix_c* destination, hdSceneViewMatrix_c* source, hdSceneVector_c* translation);
static void constructSceneOrthographic(hdSceneProjection_l* projection);
static void* getSceneProjectionMatrix(hdSceneProjection_l* projection);
static u32 drawSceneTexture(hdFilterTexture_c* texture, hdSceneViewMatrix_c* view, void* projection, hdSceneColor_c* color, u32 slot);
static void resetScenePacketDraw(hdScenePacket_l* packet, u32 index, u32 flags);
static void finishScenePacketDraw(hdScenePacket_l* packet, u32 index);
static void setScenePassDimensions(void* manager, u32 pass, void* dimensions);
static void* sLayer3DOverlayManager;
static void drawSceneOverlayManager(void* manager);
static hdSceneColor_c sLayer3DAuxiliaryClearColor;
static void clearSceneViewport(hdSceneViewport_l* viewport, u32 first, u32 second, void* color, u32 flags, f32 depth);
static void drawSceneManagedPass(void* manager, u32 index, hdFilterRenderInfo_c* info, u32 flags);
static void remapScenePass(void* manager, u32 source, u32 destination);
static u8 sLayer3DParticleFlags;
static u8 sLayer3DSuppressLists;
static hdLayerMatrix_c sLayer3DParticleWorldView;
static void resetSceneDepthBias(hdSceneDepthBias_c* bias);
static void setSceneDepthBias(hdSceneDepthBias_c* bias, f32 value);
static void setSceneDepthSlope(hdSceneDepthBias_c* bias, f32 value);
static void applySceneDepthBias(hdSceneDepthBias_c* bias);
static hdSceneViewport_l* prepareSceneShadowViewport(hdSceneShadowRenderer_l* renderer, u32 index, u32 width, u32 height);
static u32 drawScenePrimary(hdScenePrimaryBinding_l* binding, u32 index, u32 width, u32 height, void* texture, u32 slot, f32 nearPlane, f32 farPlane);
static hdLayerMatrix_c sLayer3DDefaultView;
static hdRenderPerspective_l sLayer3DDefaultProjection;
static u32 sLayer3DParticleProjectionGuard;
static hdSceneType_l sLayer3DParticleProjectionType;
static u8 sLayer3DParticleDebugState;
static void resetSceneParticleDebug(void* state);
static BOOL isSceneParticleDrawSuppressed();
static void copySceneProjectionMatrix(void* source, hdSceneProjectionMatrix_c* destination);
static void drawSceneParticles(void* particles, hdSceneParticleArg_c* argument, u32 mode);
static u8 sLayer3DOverlayEnabled;
static u8 sLayer3DOverlayAlpha;
static hdSceneViewMatrix_c sLayer3DOverlayMatrix;
static hdSceneColor_c sLayer3DOverlayColor;
static void copySceneMatrix(hdSceneViewMatrix_c* destination, hdSceneViewMatrix_c* source);
static u32 drawSceneOverlay(void* drawer, hdFilterTexture_c* texture, hdSceneViewMatrix_c* matrix, hdSceneColor_c* color, u32 slot);
static u32 drawSceneScaledTexture(hdFilterTexture_c* texture, hdFilterBounds_c* bounds, hdSceneSize_l* scale, hdSceneColor_c* color, u32 slot);
static u32 sLayer3DViewportGuard;
static hdSceneType_l sLayer3DViewportType;
static void* sLayer3DGraphicsContext;
static void* sLayer3DDepthDrawer;
static void resolveSceneDepth(hdSceneTexture_l* depth);
static void expandSceneDepth(void* format);
static void synchronizeSceneGraphics(void* context);
static u32 getScenePassFormat(void* manager, u32 pass);
static u32 drawSceneDepthCopy(void* drawer, hdFilterTexture_c* texture, u32 slot, f32 nearPlane, f32 farPlane);
static hdRenderPerspective_l* getScenePerspective(hdLayerProjection_c* projection);
static hdSceneAngle_l* sLayer3DAngle;
static void approachScenePhase(be<f32>* phase, f32 target, f32 step);
static void* finishScenePass(void* manager, u32 pass, hdFilterRenderInfo_c* info);
static void constructSceneTexture(hdFilterTexture_c* texture, void* source);
static void destroySceneTexture(hdFilterTexture_c* texture, s32 flags);
static u32 drawSceneSecondary(hdSceneSecondaryBinding_l* binding, u32 index, hdFilterViewport_l* viewport, hdFilterBounds_c* bounds, hdFilterTexture_c* texture, u32 first, u32 second, u32 third, u32 slot);
static hdFilterTargetArg_c* getSceneTarget(hdFilterTargetArg_c* argument, void* target);
static void setSceneTarget(void* binding, hdFilterTargetArg_c* argument);
static void* bindScenePass(void* manager, u32 index, void* pass);
static void setSceneBlendTarget(void* binding, hdFilterTargetArg_c* first, hdFilterTargetArg_c* second, f32 blend);
static void bindScenePassResource(void* resource, void* pass);
static void flushSceneTarget(void* texture);
static void invalidateSceneTarget(void* texture);
static u32 drawSceneFilter(hdSceneFilter_l* filter, u32 index, hdSceneFilterArg_c* argument, u32 slot);
static u32 prepareScenePostprocess(hdScenePostprocess_l* postprocess, u32 slot);
static u32 drawScenePostprocess(hdScenePostprocess_l* postprocess, hdFilterViewport_l* viewport, u32 slot);
static void flushSceneDepth(hdSceneTexture_l* texture);
static void invalidateSceneDepth(hdSceneTexture_l* texture);
static void initializeSceneViewport(hdSceneViewport_l* viewport);
static void bindSceneDepth(hdSceneTexture_l* destination, hdSceneTexture_l* source);
static void setSceneDepthFormat(void* state, BOOL enabled);
static hdFilterBounds_c* calculateSceneBounds(hdFilterBounds_c* bounds, hdSceneViewport_l* viewport);
static void applyCalculatedSceneBounds(hdFilterBounds_c* bounds, hdSceneViewport_l* viewport);
static void resetCalculatedSceneViewport(hdSceneViewport_l* viewport);
static void* getScenePass(void* manager, u32 index);
static void clearScenePass(void* manager, u32 index);
static void drawSceneForeground(hdLayer3D_c* layer, hdFilterRenderInfo_c* info);
static void drawSceneBackground(hdLayer3D_c* layer, hdFilterRenderInfo_c* info);
static hdScenePlay_l* getScenePlay();
static void resetSceneViewport(hdFilterViewport_l* viewport);
static void applySceneViewport(void* bounds, hdFilterViewport_l* viewport);
static void executeScenePass(void* manager, u32 index, hdFilterRenderInfo_c* info);
static void drawSceneList(void* list, hdSceneDrawArgument_c* argument);
static void constructSceneColorState(hdSceneColorState_c* state);
static void applySceneColorState(hdSceneColorState_c* state);
/* 0272F8C0 */
static s32 sceneNoDraw() { return 0; }
/* 0272F8C8 */
static s32 sceneNoUpdate() { return 0; }
/* 0272F9D8 */
static void destroySceneBase(hdSceneLayerBase_c* layer, u32 flags) {
 if(layer != NULL) {
  layer->mBase.mDrawVtable = gabi::ea(&sLayer3dLayerBaseDrawVtable);
  destroySceneCamera(&layer->mCamera, 2);
  destroyScenePerspective(&layer->mPerspective, 2);
  destroySceneProjection(&layer->mDrcProjection, 2);
  destroySceneProjection(&layer->mProjection, 2);
  destroySceneLayerBase(layer, 0);
  if(flags&1) deleteSceneLayer(layer);
 }
}
/* 0272FA68 */
static s32 sceneTargetCount() { return 2; }
static void initializeSceneWord(be<u32>* word) {
 if(word == NULL) word = (be<u32>*)allocateSceneLayer(4);
 if(word != NULL) *word = 0;
}
/* 0272FB18 */
static hdLayer3D_c* constructLayer3D(hdLayer3D_c* layer) {
 if(layer == NULL) {
  layer = (hdLayer3D_c*)allocateSceneLayer(sizeof(hdLayer3D_c));
  if(layer == NULL) return NULL;
 }
 SafeString name;
 name.__vtbl = gabi::ea(&sLayer3dSafeStringVtable);
 name.mStringTop = gabi::guest_addr("3D");
 constructSceneLayerBase(layer, &name);
 layer->mBase.mpVtable.v = gabi::ea(&sLayer3dLayerVtable);
 layer->mBase.mDrawVtable = gabi::ea(&sLayer3dDrawVtable);
 constructScenePrimary(&layer->mPrimaryBinding);
 constructScenePacket(&layer->mPacket);
 constructSceneSecondary(&layer->mSecondaryBinding);
 constructSceneBlur(&layer->mBlur);
 constructSceneFilter(&layer->mFilter);
 constructScenePostprocess(&layer->mPostprocess);
 layer->mpSceneTexture = NULL;
 layer->mpShadowTexture = NULL;
 layer->mpAuxiliaryTexture = NULL;
 if(u32(gabi::ea(layer)+offsetof(hdLayer3D_c, mOwnedContexts)) == 0) allocateSceneLayer(28);
 if(u32(gabi::ea(layer)+offsetof(hdLayer3D_c, mPassResources)) == 0) allocateSceneLayer(32);
 if(u32(gabi::ea(layer)+offsetof(hdLayer3D_c, mAuxiliaryResources)) == 0) allocateSceneLayer(8);
 constructSceneDescriptor(layer->mDescriptor);
 layer->mpAuxiliary0 = NULL;
 layer->mpShadowMatrix = NULL;
 initializeSceneWord(gabi::at<be<u32>>(gabi::ea(layer)+offsetof(hdLayer3D_c, mState)));
 initializeSceneWord(gabi::at<be<u32>>(gabi::ea(layer)+offsetof(hdLayer3D_c, mDrawFlags)));
 layer->mViewCount = 4;
 layer->mAngle = 180.0f;
 layer->mPhase = 0.0f;
 if(u32(gabi::ea(layer)+offsetof(hdLayer3D_c, mViewScales)) == 0) allocateSceneLayer(16);
 layer->mNearBias = -0.5f;
 layer->mFarBias = -2.0f;
 f32 width = sLayer3DTvSize.mWidth;
 f32 renderWidth = sLayer3DDisplay->mRenderWidth;
 f32 scale = width/renderWidth;
 for(u32 i = 0; i<4; ++i) layer->mViewScales[i] = scale;
 return layer;
}
/* 0272FCC8 */
static void destroyLayer3D(hdLayer3D_c* layer, u32 flags) {
 if(layer == NULL) return;
 layer->mBase.mpVtable.v = gabi::ea(&sLayer3dLayerVtable);
 layer->mBase.mDrawVtable = gabi::ea(&sLayer3dDrawVtable);
 for(u32 i = 0; i<7; ++i) {
  hdSceneOwnedResource_l* resource = (hdSceneOwnedResource_l*)layer->mOwnedContexts[i].get();
  if(resource != NULL) gabi::call_ptr(resource->mpVtable->mDestroy, resource, 3);
  layer->mOwnedContexts[i] = NULL;
 }
 for(u32 i = 0; i<8; ++i) {
  deleteSceneLayer(layer->mPassResources[i]);
  layer->mPassResources[i] = NULL;
 }
 for(u32 i = 0; i<2; ++i) {
  deleteSceneLayer(layer->mAuxiliaryResources[i]);
  layer->mAuxiliaryResources[i] = NULL;
 }
 clearScenePass(sLayer3DRenderManager, 6);
 destroyScenePostprocess(&layer->mPostprocess, 2);
 destroySceneFilter(&layer->mFilter, 2);
 destroySceneBlur(&layer->mBlur, 2);
 destroySceneSecondary(&layer->mSecondaryBinding, 2);
 destroyScenePacket(&layer->mPacket, 2);
 destroyScenePrimary(&layer->mPrimaryBinding, 2);
 destroySceneLayerBase(layer, 0);
 if(flags&1) deleteSceneLayer(layer);
}
/* 0272FEE0 */
static void setLayer3DBlurTarget(hdLayer3D_c* layer, void* target, void* second, f32 blend) {
 if(target == NULL) { layer->mBlur.mEnabled = 0; return; }
 if(second != NULL) {
  hdFilterTargetArg_c firstTemporary;
  hdFilterTargetArg_c secondTemporary;
  hdFilterTargetArg_c firstArgument;
  hdFilterTargetArg_c secondArgument;
  firstArgument.mTarget = getSceneTarget(&firstTemporary, target)->mTarget;
  secondArgument.mTarget = getSceneTarget(&secondTemporary, second)->mTarget;
  setSceneBlendTarget(layer->mBlur.mBinding, &firstArgument, &secondArgument, blend);
 } else {
  hdFilterTargetArg_c temporary;
  hdFilterTargetArg_c argument;
  argument.mTarget = getSceneTarget(&temporary, target)->mTarget;
  setSceneTarget(layer->mBlur.mBinding, &argument);
 }
}
/* 0272FFE4 */
void hdLayer3D_c::setFilterTarget(void* target) {
 if(target == NULL) { mFilter.mEnabled = 0; return; }
 hdFilterTargetArg_c temporary;
 hdFilterTargetArg_c argument;
 argument.mTarget = getSceneTarget(&temporary, target)->mTarget;
 setSceneTarget(mFilter.mBinding, &argument);
}
/* 0273004C */
void hdLayer3D_c::setPrimaryTarget(void* target) {
 if(target == NULL) { mPrimaryBinding.mEnabled = 0; return; }
 hdFilterTargetArg_c temporary;
 hdFilterTargetArg_c argument;
 argument.mTarget = getSceneTarget(&temporary, target)->mTarget;
 setSceneTarget(&mPrimaryBinding, &argument);
}
/* 027300B4 */
void hdLayer3D_c::setSecondaryTarget(void* target) {
 if(target == NULL) { mSecondaryBinding.mEnabled = 0; return; }
 hdFilterTargetArg_c temporary;
 hdFilterTargetArg_c argument;
 argument.mTarget = getSceneTarget(&temporary, target)->mTarget;
 setSceneTarget(&mSecondaryBinding, &argument);
}
/* 0273011C */
void hdLayer3D_c::bindPass(void* pass) {
 void* bound = bindScenePass(sLayer3DRenderManager, 3, pass);
 bindScenePassResource(mPassResources[0], bound);
}
/* 02730164 */
static void clearPrimaryScenePass() { clearScenePass(sLayer3DRenderManager, 3); }
static void copySceneViewport(hdSceneViewport_l* destination, hdSceneViewport_l* source) {
 /* The viewport copy uses integer loads/stores, preserving signaling NaN bits. */
 for(u32 i = 0; i<6; ++i) destination->mWords[i] = source->mWords[i];
}
static void configureSceneDepthFormat(hdSceneTexture_l* destination, u32 format) {
 destination->mFormat = format;
 destination->mStoredFormat = format;
 setSceneDepthFormat(gabi::at<void>(gabi::ea(destination)+offsetof(hdSceneTexture_l, mFormatState)), format != 0);
}
/* 02730174 */
static void drawSceneForeground(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 hdSceneViewport_l* source = (hdSceneViewport_l*)info->mpViewport.get();
 flushSceneDepth(source->mpDepth);
 invalidateSceneDepth(source->mpDepth);
 f32 width = gabi::fsubs_ppc(source->mMaxX, source->mMinX);
 f32 height = gabi::fsubs_ppc(source->mMaxY, source->mMinY);
 hdSceneViewport_l* destination = (hdSceneViewport_l*)layer->mOwnedContexts[0].get();
 /* The reference lowering forwards X unchanged; Y converts through its FPR across the constant load. */
 destination->mWords[0] = source->mWords[0];
 destination->mY = source->mY;
 f32 minX, maxX, minY, maxY;
 if(!(0.0f<width)) {
  minX = width; maxX = 0.0f;
  if(!(0.0f<height)) { minY = height; maxY = 0.0f; }
  else { minY = 0.0f; maxY = height; }
 } else {
  minX = 0.0f; maxX = width;
  if(0.0f<height) { minY = 0.0f; maxY = height; }
  else { minY = height; maxY = 0.0f; }
 }
 destination->mMinX = minX;
 destination->mMinY = minY;
 destination->mMaxX = maxX;
 destination->mMaxY = maxY;
 initializeSceneViewport(destination);
 destination->mpDepth = NULL;
 destination->mpColor = layer->mPassResources[0];
 hdSceneTexture_l* depth = (hdSceneTexture_l*)layer->mAuxiliaryResources[0].get();
 bindSceneDepth(depth, source->mpDepth);
 configureSceneDepthFormat(depth, source->mpDepth->mFormat);
 destination->mpDepth = depth;
 resetCalculatedSceneViewport(destination);
 hdFilterBounds_c bounds;
 hdFilterBounds_c* calculated = calculateSceneBounds(&bounds, destination);
 applyCalculatedSceneBounds(calculated, destination);
}
/* 027313BC */
static void prepareLayer3DViewport(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 hdSceneViewport_l* source = (hdSceneViewport_l*)info->mpViewport.get();
 void* color = layer->mPassResources[3];
 hdSceneTexture_l* depth = (hdSceneTexture_l*)layer->mAuxiliaryResources[0].get();
 bindScenePassResource(color, source->mpColor);
 bindSceneDepth(depth, source->mpDepth);
 configureSceneDepthFormat(depth, source->mpDepth->mFormat);
 void* auxiliary = layer->mPassResources[4];
 bindScenePassResource(auxiliary, getScenePass(sLayer3DRenderManager, 12));
 hdSceneViewport_l* destination = (hdSceneViewport_l*)layer->mExtraContexts[0].get();
 copySceneViewport(destination, source);
 initializeSceneViewport(destination);
 destination->mpColor = color;
 destination->mpDepth = depth;
 destination->mpAuxiliaryColor = auxiliary;
 resetCalculatedSceneViewport(destination);
 hdFilterBounds_c bounds;
 calculateSceneBounds(&bounds, destination);
 applyCalculatedSceneBounds(&bounds, destination);
}
/* 027314B0 */
static void prepareLayer3DViewportAgain(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) { prepareLayer3DViewport(layer, info); }
/* 027302F4 */
static void drawSceneBackground(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 s32 index = getScenePlay()->mBackgroundIndex;
 if(getScenePlay()->mBackgrounds[index].mEnabled == 0) return;
 hdSceneDrawArgument_c argument;
 argument.mpTarget = info; argument.mEnabled = 1; argument.mUnknown0 = 0; argument.mIndex = 1;
 for(u32 i = 0; i<9; ++i) argument.mWords[i] = 0;
 hdSceneColorState_c state;
 constructSceneColorState(&state);
 state.mAlpha = 0; state.mGreen = 1; state.mBlue = 1; state.mRed = 1;
 applySceneColorState(&state);
 drawSceneList(getScenePlay()->mpEffectList, &argument);
 drawSceneList(getScenePlay()->mpModelList, &argument);
 hdSceneViewport_l* target = (hdSceneViewport_l*)layer->mContexts[0].get();
 drawSceneList(getScenePlay()->mpLastList, &argument);
 drawSceneList(getScenePlay()->mpFinalList, &argument);
 u8 reverse = getScenePlay()->mReverseOrder;
 resetSceneViewport(info->mpViewport);
 applyCalculatedSceneBounds((hdFilterBounds_c*)info->mpViewportBounds.get(), (hdSceneViewport_l*)info->mpViewport.get());
 argument.mIndex = 2;
 hdScenePlay_l* play = getScenePlay();
 drawSceneList(reverse?play->mpEarlyList.get():play->mpLateList.get(), &argument);
 argument.mIndex = 1;
 resetSceneViewport((hdFilterViewport_l*)target);
 hdFilterBounds_c bounds;
 applyCalculatedSceneBounds(calculateSceneBounds(&bounds, target), target);
 hdSceneDrawList_l* first = reverse?getScenePlay()->mpOpaqueList.get():getScenePlay()->mpMainList.get();
 first->mMode = 1; drawSceneList(first, &argument); first->mMode = 0;
 drawSceneList(reverse?getScenePlay()->mpTranslucentList.get():getScenePlay()->mpExtraList.get(), &argument);
 resetSceneViewport(info->mpViewport);
 applyCalculatedSceneBounds((hdFilterBounds_c*)info->mpViewportBounds.get(), (hdSceneViewport_l*)info->mpViewport.get());
 argument.mIndex = 2;
 drawSceneList(reverse?getScenePlay()->mpLateList.get():getScenePlay()->mpEarlyList.get(), &argument);
 argument.mIndex = 1;
 resetSceneViewport((hdFilterViewport_l*)target);
 hdFilterBounds_c secondBounds;
 applyCalculatedSceneBounds(calculateSceneBounds(&secondBounds, target), target);
 hdSceneDrawList_l* second = reverse?getScenePlay()->mpMainList.get():getScenePlay()->mpOpaqueList.get();
 second->mMode = 1; drawSceneList(second, &argument); second->mMode = 0;
 drawSceneList(reverse?getScenePlay()->mpExtraList.get():getScenePlay()->mpTranslucentList.get(), &argument);
 hdSceneViewport_l* viewport = (hdSceneViewport_l*)info->mpViewport.get();
 flushSceneTarget(viewport->mpColor);
 invalidateSceneTarget(viewport->mpColor);
 flushSceneDepth(viewport->mpDepth);
 invalidateSceneDepth(viewport->mpDepth);
}
/* 027314B4 */
static void drawSceneAuxiliaryTextures(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 u32 slot = setupSceneDebugRenderer(&sLayer3DDebugRenderer)->mTarget;
 hdSceneColorState_c state;
 constructSceneColorState(&state);
 state.mMode = 2; state.mBlue = 1; state.mBlend = 0; state.mRed = 1;
 state.mAlphaEnabled = 0; state.mEnabled = 1; state.mAlpha = 0; state.mGreen = 1;
 applySceneColorState(&state);
 hdSceneViewport_l* source = (hdSceneViewport_l*)info->mpViewport.get();
 hdSceneViewport_l* viewport = (hdSceneViewport_l*)layer->mExtraContexts[0].get();
 copySceneViewport(viewport, source);
 initializeSceneViewport(viewport);
 viewport->mpDepth = NULL;
 hdSceneTexture_l* depth = (hdSceneTexture_l*)layer->mAuxiliaryResources[0].get();
 viewport->mpDepth = depth;
 bindSceneDepth(depth, source->mpDepth);
 configureSceneDepthFormat(depth, source->mpDepth->mFormat);
 hdFilterBounds_c bounds;
 calculateSceneBounds(&bounds, viewport);
 applyCalculatedSceneBounds(&bounds, viewport);
 hdSceneVector_c translation;
 translation.x = 0; translation.y = 0; translation.z = -1;
 hdSceneViewMatrix_c view;
 translateSceneView(&view, &sLayer3DViewMatrix, &translation);
 hdSceneProjection_l projection;
 constructSceneOrthographic(&projection);
 void* manager = sLayer3DRenderManager;
 flushSceneTarget(layer->mPassResources[6]);
 invalidateSceneTarget(layer->mPassResources[6]);
 void* color = layer->mPassResources[3];
 bindScenePassResource(color, source->mpColor);
 viewport->mpColor = color;
 resetSceneViewport((hdFilterViewport_l*)viewport);
 hdFilterTexture_c texture;
 constructSceneTexture(&texture, getScenePass(manager, 15));
 slot = drawSceneTexture(&texture, &view, getSceneProjectionMatrix(&projection), &sLayer3DDrawColor, slot);
 destroySceneTexture(&texture, 2);
 state.mAlpha = 0; state.mBlue = 0; state.mRed = 1; state.mGreen = 0;
 applySceneColorState(&state);
 flushSceneTarget(layer->mPassResources[7]);
 invalidateSceneTarget(layer->mPassResources[7]);
 color = layer->mPassResources[4];
 bindScenePassResource(color, getScenePass(manager, 12));
 viewport->mpColor = color;
 resetSceneViewport((hdFilterViewport_l*)viewport);
 constructSceneTexture(&texture, getScenePass(manager, 16));
 slot = drawSceneTexture(&texture, &view, getSceneProjectionMatrix(&projection), &sLayer3DDrawColor, slot);
 destroySceneTexture(&texture, 2);
 hdFilterDebugRenderer_l* debug = setupSceneDebugRenderer(&sLayer3DDebugRenderer);
 debug->mTarget = slot; debug->mFlags = 0;
 destroySceneProjection(&projection, 2);
}
/* 0273098C */
static void copyLayer3DSceneDepth(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 void* manager = sLayer3DRenderManager;
 hdSceneTexture_l* color = (hdSceneTexture_l*)getScenePass(manager, 1);
 hdSceneTexture_l* depth = (hdSceneTexture_l*)getScenePass(manager, 2);
 hdRenderPerspective_l* projection = getScenePerspective(layer->mBase.mpProjection);
 if(projection == NULL || depth == NULL || color == NULL) return;
 hdSceneViewport_l* source = (hdSceneViewport_l*)info->mpViewport.get();
 if(sLayer3DViewportGuard == 0) {
  sLayer3DViewportGuard = 1;
  sLayer3DViewportType.mpName.v = gabi::ea(&sLayer3dViewportName);
 }
 if(source == NULL || !gabi::call_ptr<BOOL>(source->mpVtable->mIsKindOf, source, &sLayer3DViewportType)) source = NULL;
 hdSceneTexture_l* originalDepth = gabi::at<gptr<hdSceneTexture_l>>(gabi::ea(source)+offsetof(hdSceneViewport_l, mpDepth))->get();
 flushSceneDepth(originalDepth);
 hdSceneTexture_l* currentDepth = gabi::at<gptr<hdSceneTexture_l>>(gabi::ea(source)+offsetof(hdSceneViewport_l, mpDepth))->get();
 if(currentDepth->mFormat != 0) {
  if(currentDepth->mNeedsResolve) { resolveSceneDepth(currentDepth); currentDepth->mNeedsResolve = 0; }
  expandSceneDepth(gabi::at<void>(gabi::ea(currentDepth)+offsetof(hdSceneTexture_l, mFormatState)));
  synchronizeSceneGraphics(sLayer3DGraphicsContext);
  currentDepth = gabi::at<gptr<hdSceneTexture_l>>(gabi::ea(source)+offsetof(hdSceneViewport_l, mpDepth))->get();
 }
 invalidateSceneDepth(currentDepth);
 u32 width = depth->mBlockWidth;
 if((s32)depth->mWidth>(s32)width) width = depth->mWidth;
 u32 height = depth->mBlockHeight;
 if((s32)depth->mHeight>(s32)height) height = depth->mHeight;
 hdSceneViewport_l* viewport = (hdSceneViewport_l*)layer->mpDrawTarget.get();
 u32 originalWidth = originalDepth->mWidth;
 if((s32)originalDepth->mBlockWidth>(s32)originalWidth) originalWidth = originalDepth->mBlockWidth;
 u32 originalHeight = originalDepth->mBlockHeight;
 if((s32)originalDepth->mHeight>(s32)originalHeight) originalHeight = originalDepth->mHeight;
 viewport->mX = (f32)originalWidth; viewport->mY = (f32)originalHeight;
 f32 w = (f32)width, h = (f32)height;
 viewport->mMinX = w>0.0f?0.0f:w; viewport->mMaxX = w>0.0f?w:0.0f;
 viewport->mMinY = h>0.0f?0.0f:h; viewport->mMaxY = h>0.0f?h:0.0f;
 initializeSceneViewport(viewport);
 viewport->mpDepth = NULL;
 hdFilterTexture_c texture;
 constructSceneTexture(&texture, gabi::at<gptr<hdSceneTexture_l>>(gabi::ea(source)+offsetof(hdSceneViewport_l, mpDepth))->get());
 texture.mSamplerParameters[0] = 0; texture.mSamplerParameters[1] = 0; texture.mSamplerParameters[2] = 0;
 texture.mFlags = (u8)texture.mFlags|4;
 bindScenePassResource(layer->mPassResources[2], color);
 viewport->mpColor = layer->mPassResources[2];
 bindSceneDepth((hdSceneTexture_l*)layer->mAuxiliaryResources[1].get(), depth);
 hdSceneTexture_l* destination = (hdSceneTexture_l*)layer->mAuxiliaryResources[1].get();
 configureSceneDepthFormat(destination, getScenePassFormat(manager, 2));
 viewport->mpDepth = (hdSceneTexture_l*)layer->mAuxiliaryResources[1].get();
 resetSceneViewport((hdFilterViewport_l*)viewport);
 hdFilterBounds_c bounds;
 calculateSceneBounds(&bounds, viewport);
 applyCalculatedSceneBounds(&bounds, viewport);
 u32 slot = setupSceneDebugRenderer(&sLayer3DDebugRenderer)->mTarget;
 slot = drawSceneDepthCopy(sLayer3DDepthDrawer, &texture, slot, projection->mNearPlane, projection->mFarPlane);
 flushSceneTarget(viewport->mpColor); invalidateSceneTarget(viewport->mpColor);
 flushSceneDepth(viewport->mpDepth); invalidateSceneDepth(viewport->mpDepth);
 hdFilterDebugRenderer_l* debug = setupSceneDebugRenderer(&sLayer3DDebugRenderer);
 debug->mTarget = slot; debug->mFlags = 0;
 destroySceneTexture(&texture, 2);
}
/* 02732D3C */
static void drawSceneOverlayPass(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 void* manager = sLayer3DRenderManager;
 if(sLayer3DOverlayEnabled == 0) { clearScenePass(manager, 13); return; }
 hdSceneTexture_l* target = (hdSceneTexture_l*)getScenePass(manager, 13);
 if(target == NULL) { finishScenePass(manager, 13, info); return; }
 hdSceneViewport_l* source = (hdSceneViewport_l*)info->mpViewport.get();
 flushSceneTarget(source->mpColor); invalidateSceneTarget(source->mpColor);
 hdFilterBounds_c sourceBounds;
 calculateSceneBounds(&sourceBounds, source); applyCalculatedSceneBounds(&sourceBounds, source);
 hdFilterTexture_c overlay;
 constructSceneTexture(&overlay, target);
 u32 slot = setupSceneDebugRenderer(&sLayer3DDebugRenderer)->mTarget;
 hdSceneColor_c tint;
 tint.r = 0; tint.g = 0; tint.b = 0; tint.a = (f32)sLayer3DOverlayAlpha/255.0f;
 hdSceneViewMatrix_c matrix;
 copySceneMatrix(&matrix, &sLayer3DOverlayMatrix);
 slot = drawSceneOverlay(sLayer3DDepthDrawer, &overlay, &matrix, &tint, slot);
 hdFilterDebugRenderer_l* debug = setupSceneDebugRenderer(&sLayer3DDebugRenderer);
 debug->mTarget = slot; debug->mFlags = 0;
 destroySceneTexture(&overlay, 2);
 flushSceneTarget(source->mpColor); invalidateSceneTarget(source->mpColor);
 hdSceneViewport_l* viewport = (hdSceneViewport_l*)layer->mExtraContexts[1].get();
 void* color = layer->mPassResources[5];
 bindScenePassResource(color, target);
 viewport->mWords[0] = source->mWords[0]; viewport->mWords[1] = source->mWords[1];
 u32 width = target->mBlockWidth;
 if((s32)target->mWidth>(s32)width) width = target->mWidth;
 u32 height = target->mBlockHeight;
 if((s32)target->mHeight>(s32)height) height = target->mHeight;
 f32 w = (f32)width, h = (f32)height;
 viewport->mMinX = w>0.0f?0.0f:w; viewport->mMaxX = w>0.0f?w:0.0f;
 viewport->mMinY = h>0.0f?0.0f:h; viewport->mMaxY = h>0.0f?h:0.0f;
 initializeSceneViewport(viewport); viewport->mpDepth = NULL; viewport->mpColor = color;
 resetSceneViewport((hdFilterViewport_l*)viewport);
 hdFilterBounds_c bounds;
 calculateSceneBounds(&bounds, viewport); applyCalculatedSceneBounds(&bounds, viewport);
 hdFilterTexture_c texture;
 constructSceneTexture(&texture, source->mpColor);
 hdSceneColorState_c state;
 constructSceneColorState(&state);
 state.mBlend = 0; state.mAlphaEnabled = 0; state.mEnabled = 0;
 applySceneColorState(&state);
 hdSceneSize_l scale;
 scale.mWidth = (f32)source->mX/(f32)source->mMaxX;
 scale.mHeight = (f32)source->mY/(f32)source->mMaxY;
 slot = setupSceneDebugRenderer(&sLayer3DDebugRenderer)->mTarget;
 slot = drawSceneScaledTexture(&texture, &bounds, &scale, &sLayer3DOverlayColor, slot);
 debug = setupSceneDebugRenderer(&sLayer3DDebugRenderer);
 debug->mTarget = slot; debug->mFlags = 0;
 destroySceneTexture(&texture, 2);
 resetSceneViewport(info->mpViewport);
 applyCalculatedSceneBounds((hdFilterBounds_c*)info->mpViewportBounds.get(), (hdSceneViewport_l*)info->mpViewport.get());
}
/* 027305EC */
void hdLayer3D_c::drawConfiguredPass(hdFilterRenderInfo_c* info) {
 drawSceneForeground(this, info);
 if(getScenePlay()->mDrawWorld) drawSceneBackground(this, info);
}
/* 02730640 */
void hdLayer3D_c::resetDrawFlags() { mDrawFlags = 0; }
/* 0273064C */
static void resetLayer3DViewport(void*, hdFilterRenderInfo_c* info) {
 resetSceneViewport(info->mpViewport);
 applySceneViewport(info->mpViewportBounds, info->mpViewport);
}
/* 02730EA8 */
static void clearSecondaryScenePass() { clearScenePass(sLayer3DRenderManager, 3); }
/* 02730EB8 */
f32 hdLayer3D_c::getViewScale(u32 index) {
 f32 width = sLayer3DDisplay->mWidth;
 f32 renderWidth = sLayer3DDisplay->mRenderWidth;
 if(width == renderWidth) {
  u32 address = gabi::ea(&mViewScales[0]);
  if(index<4) address += index*4;
  return *gabi::at<be<f32>>(address);
 }
 return 1.0f;
}
static SafeString* initializeSceneStepName(hdSceneRenderStep_c* step) {
 SafeString* name = gabi::at<SafeString>(gabi::ea(step)+offsetof(hdSceneRenderStep_c, mName));
 if(name == NULL) name = (SafeString*)allocateSceneLayer(sizeof(SafeString));
 if(name != NULL) { name->__vtbl = gabi::ea(&sLayer3dSafeStringVtable); name->mStringTop = gabi::guest_addr(""); }
 return name;
}
/* 02730688 */
static void initializeLayer3D(hdLayer3D_c* layer, void* heap) {
 layer->mBase.mFlags = u32(layer->mBase.mFlags)|0x80;
 hdSceneRenderStep_c* step = (hdSceneRenderStep_c*)allocateSceneLayer(sizeof(hdSceneRenderStep_c));
 if(step != NULL) {
  constructSceneStep(step);
  initializeSceneStepName(step);
  step->mFlags = 0;
  step->mVtable = gabi::ea(&sLayer3dStepVtable);
  hdSceneDelegate_c* delegate = gabi::at<hdSceneDelegate_c>(gabi::ea(step)+offsetof(hdSceneRenderStep_c, mDelegate));
  if(delegate != NULL) {
   delegate->mpLayer = layer;
   delegate->mAdjustment = 0;
   delegate->mVirtualOffset = -1;
   delegate->mVtable = gabi::ea(&sLayer3dDelegateVtable);
   delegate->mFunction = kLayer3dRenderFunction;
  }
  step->mTarget = 2;
  step->mName.mStringTop = gabi::guest_addr("Layer3D");
 }
 addSceneStep(layer, step);
 for(u32 i = 0; i<7; ++i) {
  void* object = allocateSceneObject(0x40, heap, 4);
  if(object != NULL) object = constructSceneViewportObject(object);
  layer->mOwnedContexts[i] = object;
 }
 for(u32 i = 0; i<8; ++i) {
  void* object = allocateSceneObject(0x148, heap, 4);
  if(object != NULL) object = constructSceneColorObject(object);
  layer->mPassResources[i] = object;
 }
 for(u32 i = 0; i<2; ++i) {
  void* object = allocateSceneObject(0x154, heap, 4);
  if(object != NULL) object = constructSceneDepthObject(object);
  layer->mAuxiliaryResources[i] = object;
 }
 hdSceneSecondaryArg_c argument;
 argument.mEnabled = 1;
 argument.mFlags = 0;
 initializeSceneSecondary(&layer->mSecondaryBinding, &argument, 0);
 initializeSceneBlur(&layer->mBlur, 1, heap);
 initializeSceneFilter(&layer->mFilter, 1, heap);
 initializeScenePrimary(&layer->mPrimaryBinding, 1, heap);
 hdScenePacketArg_c packetArgument;
 constructScenePacketArg(&packetArgument);
 initializeScenePacket(&layer->mPacket, &packetArgument, heap);
 layer->mPacket.mFlags = (u32(layer->mPacket.mFlags)&~0x200u)|0x100;
 initializeScenePostprocess(&layer->mPostprocess, heap, 1);
 layer->mPostprocess.mEnabled = 0;
 u32 previousHeap = 1;
 if(heap != NULL) previousHeap = setSceneHeap(sLayer3DHeapManager, heap);
 createSceneRenderManager(0);
 initializeSceneRenderManager(sLayer3DRenderManager, heap);
 hdSceneSize_l size;
 size.mWidth = 960.0f;
 size.mHeight = 540.0f;
 bindScenePass(sLayer3DRenderManager, 6, &size);
 if(previousHeap != 1) setSceneHeap(sLayer3DHeapManager, gabi::at<void>(previousHeap));
}
static hdRenderPerspective_l* getScenePerspective(hdLayerProjection_c* untyped) {
 hdRenderPerspective_l* projection = (hdRenderPerspective_l*)untyped;
 if(sLayer3DPerspectiveGuard == 0) {
  sLayer3DPerspectiveGuard = 1;
  sLayer3DPerspectiveType.mpName.v = gabi::ea(&sLayer3dPerspectiveName);
 }
 if(projection == NULL) return NULL;
 u32 target = projection->mpVtable->mIsKindOf;
 if(!gabi::call_ptr<BOOL>(target, projection, &sLayer3DPerspectiveType)) return NULL;
 return projection;
}
/* 02730D74 */
static void drawLayer3DScenePacket(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 void* manager = sLayer3DRenderManager;
 void* primary = getScenePass(manager, 3);
 void* secondary = getScenePass(manager, 1);
 hdRenderPerspective_l* projection = getScenePerspective(layer->mBase.mpProjection);
 if(projection == NULL || primary == NULL || secondary == NULL) return;
 void* target = layer->mpDrawTarget->mpTarget;
 u32 slot = setupSceneDebugRenderer(&sLayer3DDebugRenderer)->mTarget;
 hdFilterViewport_l* viewport = info->mpViewport;
 hdFilterBounds_c bounds;
 calculateSceneBounds(&bounds, (hdSceneViewport_l*)viewport);
 slot = drawScenePacket(&layer->mPacket, 0, viewport, &bounds, primary, target, secondary, slot, 0);
 hdFilterDebugRenderer_l* debug = setupSceneDebugRenderer(&sLayer3DDebugRenderer);
 debug->mTarget = slot;
 debug->mFlags = 0;
 layer->mDrawFlags = u32(layer->mDrawFlags)|1;
}
/* 02732374 */
static void drawLayer3DBlur(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 hdRenderPerspective_l* projection = getScenePerspective(layer->mBase.mpProjection);
 if(projection == NULL || layer->mBlur.mEnabled == 0) return;
 layer->mBlur.mViewScale = layer->getViewScale(1);
 u32 slot = setupSceneDebugRenderer(&sLayer3DDebugRenderer)->mTarget;
 f32 nearPlane = projection->mNearPlane;
 f32 farPlane = projection->mFarPlane;
 slot = drawSceneBlur(&layer->mBlur, 0, info->mpViewport, slot, nearPlane, farPlane);
 hdFilterDebugRenderer_l* debug = setupSceneDebugRenderer(&sLayer3DDebugRenderer);
 debug->mTarget = slot;
 debug->mFlags = 0;
}
/* 0273135C */
static void prepareLayer3DShadow(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 if(layer->mDrawFlags&2) {
  void* shadow = sLayer3DScreens->mpLayers.get()[0];
  u32 slot = prepareSceneShadow(shadow, info);
  hdFilterDebugRenderer_l* debug = setupSceneDebugRenderer(&sLayer3DDebugRenderer);
  debug->mTarget = slot;
  debug->mFlags = 0;
 }
}
/* 02732818 */
static void updateLayer3DFilterScale(hdLayer3D_c* layer) {
 f32 scale = layer->getViewScale(0);
 layer->mFilter.mpMaterial->mViewScale = scale;
}
/* 02732848 */
static void drawLayer3DFilter(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 if(layer->mFilter.mEnabled == 0 || layer->mFilter.mReady == 0) return;
 hdSceneViewport_l* viewport = (hdSceneViewport_l*)layer->mExtraContexts[0].get();
 void* target = viewport->mpAuxiliaryColor;
 if(target != NULL) { flushSceneTarget(target); invalidateSceneTarget(target); }
 u32 slot = setupSceneDebugRenderer(&sLayer3DDebugRenderer)->mTarget;
 hdSceneViewport_l* source = (hdSceneViewport_l*)info->mpViewport.get();
 hdFilterBounds_c bounds;
 calculateSceneBounds(&bounds, source);
 applyCalculatedSceneBounds(&bounds, source);
 hdSceneFilterArg_c argument;
 argument.mpViewport = source;
 argument.mpBounds = &bounds;
 argument.mFlags = 0;
 argument.mReserved = 0;
 argument.mEnabled = 0;
 argument.mpAuxiliary = NULL;
 argument.mpAuxiliary = getScenePass(sLayer3DRenderManager, 12);
 slot = drawSceneFilter(&layer->mFilter, 0, &argument, slot);
 hdFilterDebugRenderer_l* debug = setupSceneDebugRenderer(&sLayer3DDebugRenderer);
 debug->mFlags = 0;
 debug->mTarget = slot;
}
/* 0273294C */
static void drawLayer3DPostprocess(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 u32 slot = setupSceneDebugRenderer(&sLayer3DDebugRenderer)->mTarget;
 slot = prepareScenePostprocess(&layer->mPostprocess, slot);
 slot = drawScenePostprocess(&layer->mPostprocess, info->mpViewport, slot);
 hdFilterDebugRenderer_l* debug = setupSceneDebugRenderer(&sLayer3DDebugRenderer);
 debug->mTarget = slot;
 debug->mFlags = 0;
}
/* 027329C8 */
static void updateSceneSecondaryPhase(hdLayer3D_c* layer) {
 BOOL easeIn = sLayer3DDisplay->mWidth == sLayer3DDisplay->mRenderWidth;
 if(!easeIn) {
  u8 suspended = sLayer3DDisplay->mSuspendSecondary;
  hdSceneAngle_l* camera = sLayer3DAngle;
  if(suspended) {
   f32 reference = layer->mSecondaryBinding.mReference;
   layer->mSecondaryBinding.mPhase = 0.0f;
   layer->mPhase = 0.0f;
   layer->mSecondaryBinding.mReference = reference;
   return;
  }
  easeIn = camera != NULL && camera->mAngle<layer->mAngle;
 }
 f32 step = 1.0f/(f32)(s32)layer->mViewCount;
 approachScenePhase(&layer->mPhase, easeIn?1.0f:0.0f, step);
 layer->mSecondaryBinding.mPhase = layer->mPhase;
 f32 reference = layer->mSecondaryBinding.mReference;
 layer->mSecondaryBinding.mReference = reference;
}
/* 02732B4C */
static void drawLayer3DSecondary(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 if(layer->mSecondaryBinding.mEnabled == 0) return;
 if(!(layer->mSecondaryBinding.mPhase>0.0f)) return;
 u32 slot = setupSceneDebugRenderer(&sLayer3DDebugRenderer)->mTarget;
 BOOL managed = (layer->mState&1) != 0;
 void* source;
 if(managed) source = finishScenePass(sLayer3DRenderManager, 5, info);
 else source = ((hdSceneViewport_l*)info->mpViewport.get())->mpColor;
 hdFilterTexture_c texture;
 constructSceneTexture(&texture, source);
 texture.mHasMipmaps = 1;
 slot = drawSceneSecondary(&layer->mSecondaryBinding, 0, info->mpViewport, (hdFilterBounds_c*)info->mpViewportBounds.get(), &texture, 0, 0, 0, slot);
 if(managed) clearScenePass(sLayer3DRenderManager, 5);
 destroySceneTexture(&texture, 2);
 hdFilterDebugRenderer_l* debug = setupSceneDebugRenderer(&sLayer3DDebugRenderer);
 debug->mTarget = slot;
 debug->mFlags = 0;
}
/* 02732470 */
static void drawSceneBlurObjects(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 drawLayer3DBlur(layer, info);
 prepareLayer3DViewport(layer, info);
 hdSceneBlurDrawStorage_c drawStorage;
 hdSceneDrawArgument_c& argument = drawStorage.mArgument;
 argument.mpTarget = info; argument.mEnabled = 1; argument.mUnknown0 = 0; argument.mIndex = 2;
 argument.mReserved0 = 0; argument.mReserved1 = 0;
 argument.mpScene = layer->mpSceneTexture;
 argument.mpPrimary = layer->mPrimaryBinding.mpResource->mDescriptor;
 argument.mpShadow = layer->mpShadowTexture;
 argument.mpAuxiliary = NULL;
 argument.mpPacket = layer->mPacket.mpResource->mDescriptor;
 argument.mReserved2 = 0; argument.mpTexture = layer->mpAuxiliaryTexture;
 hdSceneAuxiliaryResource_l* auxiliary = (hdSceneAuxiliaryResource_l*)layer->mpAuxiliary0.get();
 if(auxiliary != NULL) argument.mpAuxiliary = auxiliary->mDescriptor;
 resetSceneParticleDebug(&sLayer3DParticleDebugState);
 if(isSceneParticleDrawSuppressed()) return;
 u8 reverse = getScenePlay()->mReverseOrder;
 hdScenePlay_l* play = getScenePlay();
 hdSceneDrawList_l* first = reverse?play->mpOpaqueList.get():play->mpMainList.get();
 first->mMode = 1; drawSceneList(first, &argument); first->mMode = 0;
 drawSceneList(reverse?getScenePlay()->mpTranslucentList.get():getScenePlay()->mpExtraList.get(), &argument);
 hdSceneDrawList_l* second = reverse?getScenePlay()->mpMainList.get():getScenePlay()->mpOpaqueList.get();
 second->mMode = 1; drawSceneList(second, &argument); second->mMode = 0;
 drawSceneList(reverse?getScenePlay()->mpExtraList.get():getScenePlay()->mpTranslucentList.get(), &argument);
 drawSceneList(getScenePlay()->mpMiddleList, &argument);
 drawSceneList(getScenePlay()->mpLastList, &argument);
 u32 flags = layer->mBase.mFlags;
 hdLayerMatrix_c* view;
 if((flags&0xC0) == 0xC0) view = gabi::at<hdLayerMatrix_c>(gabi::ea(layer)+offsetof(hdLayerBase_c, mViewMatrix));
 else { view = layer->mBase.mpViewMatrix; if(view == NULL) view = &sLayer3DDefaultView; }
 hdRenderPerspective_l* projection;
 if(flags&0x1000) projection = (hdRenderPerspective_l*)layer->mBase.mpInternalProjection.get();
 else { projection = (hdRenderPerspective_l*)layer->mBase.mpProjection.get(); if(projection == NULL) projection = &sLayer3DDefaultProjection; }
 if(view != NULL && projection != NULL) {
  if(sLayer3DParticleProjectionGuard == 0) {
   sLayer3DParticleProjectionGuard = 1;
   sLayer3DParticleProjectionType.mpName.v = gabi::ea(&sLayer3dParticleProjectionName);
  }
  if(gabi::call_ptr<BOOL>(projection->mpVtable->mIsKindOf, projection, &sLayer3DParticleProjectionType)) {
   hdSceneParticleArg_c& particles = drawStorage.mParticles;
   particles.mpContext = NULL; particles.mpView = view;
   particles.mFieldOfView = projection->mFieldOfView;
   particles.mAspect = projection->mAspect;
   copySceneProjectionMatrix(getSceneProjectionMatrix((hdSceneProjection_l*)projection), &particles.mProjection);
   particles.mpContext = &argument;
   if(getScenePlay()->mpParticles != NULL) drawSceneParticles(getScenePlay()->mpParticles, &particles, 1);
   if(getScenePlay()->mpParticles != NULL) drawSceneParticles(getScenePlay()->mpParticles, &particles, 3);
  }
 }
 hdSceneViewport_l* viewport = (hdSceneViewport_l*)info->mpViewport.get();
 flushSceneDepth(viewport->mpDepth);
 hdSceneTexture_l* depth = viewport->mpDepth;
 if(depth->mFormat != 0) {
  if(depth->mNeedsResolve) { resolveSceneDepth(depth); depth->mNeedsResolve = 0; }
  expandSceneDepth(gabi::at<void>(gabi::ea(depth)+offsetof(hdSceneTexture_l, mFormatState)));
  synchronizeSceneGraphics(sLayer3DGraphicsContext);
  depth = viewport->mpDepth;
 }
 invalidateSceneDepth(depth);
}
static u32 sceneDimensionToUnsigned(f32 value) {
 if(value<2147483648.0f) return u32(gabi::ftoi(value));
 return u32(gabi::ftoi(gabi::fsubs_ppc(value, 2147483648.0f)))+0x80000000u;
}
/* 02730EF8 */
static void drawLayer3DPrimary(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 hdSceneTexture_l* texture = (hdSceneTexture_l*)getScenePass(sLayer3DRenderManager, 1);
 hdRenderPerspective_l* projection = getScenePerspective(layer->mBase.mpProjection);
 hdSceneShadowLayer_l* shadow = (hdSceneShadowLayer_l*)sLayer3DScreens->mpLayers[0].get();
 hdSceneShadowRenderer_l* renderer = gabi::at<hdSceneShadowRenderer_l>(gabi::ea(shadow)+offsetof(hdSceneShadowLayer_l, mRenderer));
 if(layer->mPrimaryBinding.mEnabled == 0 || texture == NULL || projection == NULL) {
  renderer->mFlags = (u32)renderer->mFlags&~1u;
  renderer->mpOwner->mpPrimaryDescriptor = NULL;
  return;
 }
 u32 slot = setupSceneDebugRenderer(&sLayer3DDebugRenderer)->mTarget;
 hdSceneViewport_l* source = (hdSceneViewport_l*)info->mpViewport.get();
 hdSceneViewport_l* viewport = NULL;
 if(renderer != NULL && renderer->mEnabled) {
  renderer->mpOwner->mViewScale = layer->getViewScale(3);
  u32 width = sceneDimensionToUnsigned(gabi::fsubs_ppc(source->mMaxX, source->mMinX));
  u32 height = sceneDimensionToUnsigned(gabi::fsubs_ppc(source->mMaxY, source->mMinY));
  viewport = prepareSceneShadowViewport(renderer, 0, width, height);
 }
 layer->mPrimaryBinding.mpResource->mpShadowViewport = viewport;
 u32 width, height;
 if(viewport != NULL) {
  width = sceneDimensionToUnsigned(gabi::fsubs_ppc(viewport->mMaxX, viewport->mMinX));
  height = sceneDimensionToUnsigned(gabi::fsubs_ppc(viewport->mMaxY, viewport->mMinY));
 } else {
  f32 scale = layer->getViewScale(2);
  u32 imageWidth = texture->mBlockWidth;
  if((s32)texture->mWidth>(s32)imageWidth) imageWidth = texture->mWidth;
  width = sceneDimensionToUnsigned(gabi::fmuls_ppc((f32)imageWidth, scale));
  u32 imageHeight = texture->mBlockHeight;
  if((s32)texture->mHeight>(s32)imageHeight) imageHeight = texture->mHeight;
  height = sceneDimensionToUnsigned(gabi::fmuls_ppc((f32)imageHeight, scale));
 }
 slot = drawScenePrimary(&layer->mPrimaryBinding, 0, width, height, texture, slot, projection->mNearPlane, projection->mFarPlane);
 renderer->mFlags = (u32)renderer->mFlags|1;
 renderer->mpOwner->mpPrimaryDescriptor = layer->mPrimaryBinding.mpResource->mDescriptor;
 hdFilterDebugRenderer_l* debug = setupSceneDebugRenderer(&sLayer3DDebugRenderer);
 debug->mTarget = slot; debug->mFlags = 0;
 layer->mDrawFlags = (u32)layer->mDrawFlags|2;
}
static void drawSceneParticleMode(hdSceneParticleArg_c* argument, u32 mode) {
 if(getScenePlay()->mpParticles != NULL) drawSceneParticles(getScenePlay()->mpParticles, argument, mode);
}
static void applyLayer3DDepthBias(hdLayer3D_c* layer, hdSceneDepthBias_c* bias) {
 setSceneDepthBias(bias, layer->mNearBias);
 setSceneDepthSlope(bias, layer->mFarBias);
 applySceneDepthBias(bias);
}
/* 02731E50 */
static void drawSceneTranslucentObjects(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 u32 flags = layer->mBase.mFlags;
 hdLayerMatrix_c* view;
 if((flags&0xC0) == 0xC0) view = gabi::at<hdLayerMatrix_c>(gabi::ea(layer)+offsetof(hdLayerBase_c, mViewMatrix));
 else { view = layer->mBase.mpViewMatrix; if(view == NULL) view = &sLayer3DDefaultView; }
 hdRenderPerspective_l* projection;
 if(flags&0x1000) projection = (hdRenderPerspective_l*)layer->mBase.mpInternalProjection.get();
 else { projection = (hdRenderPerspective_l*)layer->mBase.mpProjection.get(); if(projection == NULL) projection = &sLayer3DDefaultProjection; }
 if(view == NULL || projection == NULL) return;
 if(sLayer3DParticleProjectionGuard == 0) { sLayer3DParticleProjectionGuard = 1; sLayer3DParticleProjectionType.mpName.v = gabi::ea(&sLayer3dParticleProjectionName); }
 if(!gabi::call_ptr<BOOL>(projection->mpVtable->mIsKindOf, projection, &sLayer3DParticleProjectionType)) return;
 prepareLayer3DViewport(layer, info);
 hdSceneTranslucentDrawStorage_c drawStorage;
 hdSceneDrawArgument_c& argument = drawStorage.mArgument;
 hdFilterTexture_c& texture = drawStorage.mTexture;
 argument.mpTarget = info; argument.mEnabled = 1; argument.mUnknown0 = 0; argument.mIndex = 2;
 argument.mReserved0 = 0; argument.mReserved1 = 0;
 argument.mpScene = layer->mpSceneTexture; argument.mpPrimary = layer->mPrimaryBinding.mpResource->mDescriptor;
 argument.mpShadow = layer->mpShadowTexture; argument.mpPacket = NULL;
 argument.mReserved2 = 0; argument.mpTexture = layer->mpAuxiliaryTexture;
 hdSceneAuxiliaryResource_l* auxiliary = (hdSceneAuxiliaryResource_l*)layer->mpAuxiliary0.get();
 argument.mpAuxiliary = auxiliary != NULL?auxiliary->mDescriptor:NULL;
 constructSceneTexture(&texture, getScenePass(sLayer3DRenderManager, 1));
 argument.mReserved2 = gabi::ea(&texture);
 resetSceneParticleDebug(&sLayer3DParticleDebugState);
 if(!isSceneParticleDrawSuppressed()) {
  hdSceneDepthBias_c bias;
  resetSceneDepthBias(&bias); applyLayer3DDepthBias(layer, &bias);
  drawSceneList(getScenePlay()->mpPreParticleList, &argument);
  resetSceneDepthBias(&bias); applySceneDepthBias(&bias);
  hdSceneParticleArg_c& particles = drawStorage.mParticles;
  particles.mpView = view; particles.mpContext = NULL;
  particles.mFieldOfView = projection->mFieldOfView; particles.mAspect = projection->mAspect;
  copySceneProjectionMatrix(getSceneProjectionMatrix((hdSceneProjection_l*)projection), &particles.mProjection);
  particles.mpContext = &argument;
  if(!(sLayer3DParticleFlags&1)) drawSceneParticleMode(&particles, 5);
  applyLayer3DDepthBias(layer, &bias);
  if(sLayer3DSuppressLists == 0) drawSceneList(getScenePlay()->mpMiddleList, &argument);
  drawSceneList(getScenePlay()->mpParticleList0, &argument);
  if(sLayer3DSuppressLists == 0) drawSceneList(getScenePlay()->mpLastList, &argument);
  resetSceneDepthBias(&bias); applySceneDepthBias(&bias);
  drawSceneParticleMode(&particles, 0);
  if(sLayer3DSuppressLists == 0) drawSceneParticleMode(&particles, 1);
  hdSceneParticleArg_c& worldParticles = drawStorage.mWorldParticles;
  worldParticles.mpView = &sLayer3DParticleWorldView;
  worldParticles.mFieldOfView = projection->mFieldOfView; worldParticles.mAspect = projection->mAspect;
  worldParticles.mpContext = &argument;
  drawSceneParticleMode(&worldParticles, 6);
  drawSceneParticleMode(&particles, 2);
  if(sLayer3DSuppressLists == 0) drawSceneParticleMode(&particles, 3);
  applyLayer3DDepthBias(layer, &bias);
  drawSceneList(getScenePlay()->mpParticleList1, &argument);
  drawSceneList(getScenePlay()->mpParticleList2, &argument);
  drawSceneList(getScenePlay()->mpParticleList3, &argument);
  resetSceneDepthBias(&bias); applySceneDepthBias(&bias);
  executeScenePass(sLayer3DRenderManager, 6, info);
  resetSceneParticleDebug(&sLayer3DParticleDebugState);
  prepareLayer3DViewport(layer, info);
  drawSceneParticleMode(&particles, 4);
  applyLayer3DDepthBias(layer, &bias);
  drawSceneList(getScenePlay()->mpParticleList4, &argument);
  drawSceneList(getScenePlay()->mpParticleList5, &argument);
  resetSceneDepthBias(&bias); applySceneDepthBias(&bias);
 }
 destroySceneTexture(&texture, 2);
}
/* 0273172C */
static void drawSceneOpaqueObjects(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 u32 flags = layer->mBase.mFlags;
 hdLayerMatrix_c* view;
 if((flags&0xC0) == 0xC0) view = gabi::at<hdLayerMatrix_c>(gabi::ea(layer)+offsetof(hdLayerBase_c, mViewMatrix));
 else { view = layer->mBase.mpViewMatrix; if(view == NULL) view = &sLayer3DDefaultView; }
 hdRenderPerspective_l* projection;
 if(flags&0x1000) projection = (hdRenderPerspective_l*)layer->mBase.mpInternalProjection.get();
 else { projection = (hdRenderPerspective_l*)layer->mBase.mpProjection.get(); if(projection == NULL) projection = &sLayer3DDefaultProjection; }
 if(view == NULL || projection == NULL) return;
 if(sLayer3DParticleProjectionGuard == 0) { sLayer3DParticleProjectionGuard = 1; sLayer3DParticleProjectionType.mpName.v = gabi::ea(&sLayer3dParticleProjectionName); }
 if(!gabi::call_ptr<BOOL>(projection->mpVtable->mIsKindOf, projection, &sLayer3DParticleProjectionType)) return;
 prepareLayer3DViewportAgain(layer, info);
 hdSceneOpaqueDrawStorage_c drawStorage;
 hdSceneDrawArgument_c& argument = drawStorage.mArgument;
 hdFilterTexture_c& texture = drawStorage.mTexture;
 argument.mpTarget = info; argument.mEnabled = 1; argument.mUnknown0 = 0; argument.mIndex = 2;
 argument.mReserved0 = 0; argument.mReserved1 = 0;
 argument.mpScene = layer->mpSceneTexture; argument.mpPrimary = layer->mPrimaryBinding.mpResource->mDescriptor;
 argument.mpShadow = layer->mpShadowTexture; argument.mpPacket = layer->mPacket.mpResource->mDescriptor;
 argument.mReserved2 = 0; argument.mpTexture = layer->mpAuxiliaryTexture;
 hdSceneAuxiliaryResource_l* auxiliary = (hdSceneAuxiliaryResource_l*)layer->mpAuxiliary0.get();
 argument.mpAuxiliary = auxiliary != NULL?auxiliary->mDescriptor:NULL;
 constructSceneTexture(&texture, getScenePass(sLayer3DRenderManager, 1));
 argument.mReserved2 = gabi::ea(&texture);
 resetSceneParticleDebug(&sLayer3DParticleDebugState);
 if(!isSceneParticleDrawSuppressed()) {
  hdSceneOpaqueScratch_c& scratch = drawStorage.mScratch;
  if(sLayer3DDisplay->mUseAuxiliary == 0) {
   drawSceneList(getScenePlay()->mpInitialList0, &argument);
   drawSceneList(getScenePlay()->mpInitialList1, &argument);
   drawSceneList(getScenePlay()->mpInitialList2, &argument);
   drawSceneList(getScenePlay()->mpFinalList, &argument);
  } else {
   void* manager = sLayer3DRenderManager;
   hdSceneTexture_l* first = (hdSceneTexture_l*)getScenePass(manager, 15);
   void* second = getScenePass(manager, 16);
   void* secondColor = layer->mPassResources[7]; bindScenePassResource(secondColor, second);
   void* firstColor = layer->mPassResources[6]; bindScenePassResource(firstColor, first);
   hdSceneViewport_l* source = (hdSceneViewport_l*)info->mpViewport.get();
   hdSceneViewport_l* viewport = (hdSceneViewport_l*)layer->mExtraContexts[2].get();
   viewport->mWords[0] = source->mWords[0]; viewport->mWords[1] = source->mWords[1];
   u32 width = first->mBlockWidth; if((s32)first->mWidth>(s32)width) width = first->mWidth;
   u32 height = first->mBlockHeight; if((s32)first->mHeight>(s32)height) height = first->mHeight;
   f32 w = (f32)width, h = (f32)height;
   viewport->mMinX = w>0.0f?0.0f:w; viewport->mMaxX = w>0.0f?w:0.0f;
   viewport->mMinY = h>0.0f?0.0f:h; viewport->mMaxY = h>0.0f?h:0.0f;
   initializeSceneViewport(viewport); viewport->mpDepth = NULL;
   viewport->mpColor = firstColor; viewport->mpAuxiliaryColor = secondColor;
   viewport->mpDepth = (hdSceneTexture_l*)layer->mAuxiliaryResources[1].get();
   resetSceneViewport((hdFilterViewport_l*)viewport);
   hdFilterBounds_c bounds;
   calculateSceneBounds(&bounds, viewport); applyCalculatedSceneBounds(&bounds, viewport);
   gabi::call_ptr<void>(viewport->mpVtable->mClear, viewport, 2, &sLayer3DAuxiliaryClearColor, 0, 1.0f);
   clearSceneViewport(viewport, 0, 1, sLayer3DDisplay->mClearColor, 0, 1.0f);
   drawSceneList(getScenePlay()->mpInitialList0, &argument);
   drawSceneList(getScenePlay()->mpInitialList1, &argument);
   drawSceneList(getScenePlay()->mpInitialList2, &argument);
   flushSceneTarget(viewport->mpColor); invalidateSceneTarget(viewport->mpColor);
   flushSceneTarget(viewport->mpAuxiliaryColor); invalidateSceneTarget(viewport->mpAuxiliaryColor);
   if(sLayer3DDisplay->mDeferAuxiliary == 0) {
    prepareLayer3DViewport(layer, info);
    hdSceneColorState_c& state = scratch.mColorState;
    constructSceneColorState(&state);
    state.mRed = 1; state.mGreen = 1; state.mEnabled = 1; state.mMode = 7;
    state.mBlue = 1; state.mBlend = 0; state.mAlpha = 0; state.mAlphaEnabled = 0;
    applySceneColorState(&state);
    drawSceneManagedPass(manager, 15, info, 0);
    remapScenePass(manager, 16, 12);
   }
   resetSceneParticleDebug(&sLayer3DParticleDebugState);
   prepareLayer3DViewport(layer, info);
   drawSceneList(getScenePlay()->mpFinalList, &argument);
  }
  if(sLayer3DParticleFlags&1) {
   hdSceneParticleArg_c& particles = scratch.mParticles;
   particles.mpView = view; particles.mpContext = NULL;
   particles.mFieldOfView = projection->mFieldOfView; particles.mAspect = projection->mAspect;
   copySceneProjectionMatrix(getSceneProjectionMatrix((hdSceneProjection_l*)projection), &particles.mProjection);
   particles.mpContext = &argument;
   drawSceneParticleMode(&particles, 5);
  }
  drawSceneList(getScenePlay()->mpModelList, &argument);
  if(sLayer3DSuppressLists == 0) {
   u8 reverse = getScenePlay()->mReverseOrder;
   hdScenePlay_l* play = getScenePlay();
   hdSceneDrawList_l* first = reverse?play->mpOpaqueList.get():play->mpMainList.get();
   first->mMode = 1; drawSceneList(first, &argument); first->mMode = 0;
   drawSceneList(reverse?getScenePlay()->mpTranslucentList.get():getScenePlay()->mpExtraList.get(), &argument);
   hdSceneDrawList_l* second = reverse?getScenePlay()->mpMainList.get():getScenePlay()->mpOpaqueList.get();
   second->mMode = 1; drawSceneList(second, &argument); second->mMode = 0;
   drawSceneList(reverse?getScenePlay()->mpExtraList.get():getScenePlay()->mpTranslucentList.get(), &argument);
  }
  hdSceneDisplay_l* display = sLayer3DDisplay;
  if(display->mUseAuxiliary != 0 && display->mDeferAuxiliary != 0) drawSceneAuxiliaryTextures(layer, info);
  resetSceneParticleDebug(&sLayer3DParticleDebugState);
  prepareLayer3DViewport(layer, info);
  drawSceneList(getScenePlay()->mpEffectList, &argument);
 }
 destroySceneTexture(&texture, 2);
}
/* 027330FC */
static void drawSceneSpecialPass(void*, hdFilterRenderInfo_c* info) {
 if(sLayer3DDisplay->mSpecialPass == 1) executeScenePass(sLayer3DRenderManager, 14, info);
}
/* 02733124 */
static void drawSceneAuxiliary(void*, void* target) {
 hdSceneDrawArgument_c argument;
 argument.mpTarget = target;
 argument.mEnabled = 1;
 argument.mUnknown0 = 0;
 argument.mIndex = 2;
 for(u32 i = 0; i<9; ++i) argument.mWords[i] = 0;
 drawSceneList(getScenePlay()->mpAuxiliaryDrawList, &argument);
}
/* 02733190 */
static void drawLayer3DPass(hdLayer3D_c* layer, hdFilterRenderInfo_c* info) {
 void* manager = sLayer3DRenderManager;
 switch((u32)info->mPass) {
 case 0: resetScenePacketDraw(&layer->mPacket, 0, 0); break;
 case 1: {
  void* dimensions = gabi::at<void>(gabi::ea(info->mpViewport)+offsetof(hdSceneViewport_l, mMaxX));
  setScenePassDimensions(manager, 2, dimensions); setScenePassDimensions(manager, 1, dimensions);
  if(getScenePlay()->mDrawWorld) { copyLayer3DSceneDepth(layer, info); drawLayer3DScenePacket(layer, info); }
  clearSecondaryScenePass(); break;
 }
 case 2: drawLayer3DPrimary(layer, info); break;
 case 3: prepareLayer3DShadow(layer, info); break;
 case 4: {
  void* dimensions = gabi::at<void>(gabi::ea(info->mpViewport)+offsetof(hdSceneViewport_l, mMaxX));
  setScenePassDimensions(manager, 12, dimensions); setScenePassDimensions(manager, 16, dimensions); setScenePassDimensions(manager, 15, dimensions);
  if(getScenePlay()->mDrawWorld) drawSceneOpaqueObjects(layer, info);
  clearScenePass(manager, 2); clearScenePass(manager, 15); clearScenePass(manager, 16); break;
 }
 case 5:
  if(getScenePlay()->mDrawWorld) {
   drawSceneTranslucentObjects(layer, info);
   hdSceneTexture_l* depth = ((hdSceneViewport_l*)info->mpViewport.get())->mpDepth;
   if(depth->mFormat != 0) {
    if(depth->mNeedsResolve) { resolveSceneDepth(depth); depth->mNeedsResolve = 0; }
    expandSceneDepth(gabi::at<void>(gabi::ea(depth)+offsetof(hdSceneTexture_l, mFormatState)));
    synchronizeSceneGraphics(sLayer3DGraphicsContext);
   }
   u32 sourceAddress = gabi::at<gptr<void>>(gabi::ea(layer)+offsetof(hdLayer3D_c, mAuxiliaryResources))->v;
   u32 destinationAddress = gabi::ea(layer)+offsetof(hdLayer3D_c, mDescriptor);
   for(u32 i = 0; i<30; ++i) *gabi::at<be<u32>>(destinationAddress+i*4) = gabi::at<be<u32>>(sourceAddress+i*4)->get();
   for(u32 i = 0x78; i<0x7C; ++i) *gabi::at<be<u8>>(destinationAddress+i) = gabi::at<be<u8>>(sourceAddress+i)->get();
   for(u32 i = 31; i<35; ++i) *gabi::at<be<u32>>(destinationAddress+i*4) = gabi::at<be<u32>>(sourceAddress+i*4)->get();
  }
  clearScenePass(manager, 1); break;
 case 6:
  if(sLayer3DSuppressLists != 0) drawSceneBlurObjects(layer, info);
  if(getScenePlay()->mDrawWorld && (layer->mDrawFlags&1)) finishScenePacketDraw(&layer->mPacket, 0);
  break;
 case 8: if(sLayer3DSuppressLists == 0) drawLayer3DBlur(layer, info); break;
 case 9: updateLayer3DFilterScale(layer); drawLayer3DFilter(layer, info); clearScenePass(manager, 12); break;
 case 10: {
  hdSceneDrawArgument_c argument;
  argument.mpTarget = info; argument.mEnabled = 1; argument.mUnknown0 = 0; argument.mIndex = 2;
  for(u32 i = 0; i<9; ++i) argument.mWords[i] = 0;
  drawSceneList(getScenePlay()->mpFinalOverlayList, &argument); break;
 }
 case 11: drawLayer3DPostprocess(layer, info); break;
 case 12: updateSceneSecondaryPhase(layer); drawLayer3DSecondary(layer, info); break;
 case 13: drawSceneOverlayPass(layer, info); break;
 case 14: drawSceneSpecialPass(layer, info); break;
 case 15: drawSceneAuxiliary(layer, info); break;
 case 16: drawSceneOverlayManager(sLayer3DOverlayManager); break;
 default: break;
 }
}
/* 02733708 */
static void __sinit_hd_layer_3d_cpp() {
 for (u32 i = 0; i < 4; ++i) sLayer3DHeaderZero.mWords[i] = 0;
    __register_global_object(gabi::ea(&sLayer3DHeaderDescriptor));
    sLayer3DMinRotation = -3.1415927410125732f;
    sLayer3DMaxRotation = 3.1415927410125732f;
    initializeRotationGuard(&sLayer3DRotationGuard);
    __register_global_object(gabi::ea(&sLayer3DRotationDescriptor));
    initializePositiveRotationGuard(&sLayer3DPositiveRotationGuard);
    __register_global_object(gabi::ea(&sLayer3DPositiveRotationDescriptor));
}
/* WWHD glue
 data sLayer3dLayerBaseDrawVtable 10141C34 nocheck
 data sLayer3dSafeStringVtable 10141D98 nocheck
 data sLayer3dLayerVtable 10141EF8 nocheck
 data sLayer3dDrawVtable 10141FA0 nocheck
 data sLayer3dPerspectiveName 10141DC0 nocheck
 data sLayer3dStepVtable 10169E3C nocheck
 data sLayer3dDelegateVtable 10141E20 nocheck
 data sLayer3dViewportName 10141DE0 nocheck
 data sLayer3dParticleProjectionName 10141E10 nocheck
 data sLayer3DHeapManager 101F8B4C
 str 10141D7C "" in 02730688
 str 10141EBC
 ext 0273B050 void* allocateSceneObject(u32 size,void* heap,u32 alignment)
 ext 027B95B4 void* constructSceneViewportObject(void* object)
 ext 027B6CB8 void* constructSceneColorObject(void* object)
 ext 027B6E64 void* constructSceneDepthObject(void* object)
 ext 02752A84 void constructSceneStep(hdSceneRenderStep_c* step)
 ext 027C2450 void addSceneStep(hdLayer3D_c* layer,hdSceneRenderStep_c* step)
 ext 0277A634 void initializeSceneSecondary(hdSceneSecondaryBinding_l* binding,hdSceneSecondaryArg_c* argument,u32 flags)
 ext 0277F32C void initializeSceneBlur(hdSceneBlur_l* blur,u32 enabled,void* heap)
 ext 02775AD8 void initializeSceneFilter(hdSceneFilter_l* filter,u32 enabled,void* heap)
 ext 02793E88 void initializeScenePrimary(hdScenePrimaryBinding_l* primary,u32 enabled,void* heap)
 ext 027B19CC void constructScenePacketArg(hdScenePacketArg_c* argument)
 ext 027AF16C void initializeScenePacket(hdScenePacket_l* packet,hdScenePacketArg_c* argument,void* heap)
 ext 02784400 void initializeScenePostprocess(hdScenePostprocess_l* postprocess,void* heap,u32 enabled)
 ext 02756170 u32 setSceneHeap(void* manager,void* heap)
 ext 027F7E08 void createSceneRenderManager(u32 flags)
 ext 027F7F08 void initializeSceneRenderManager(void* manager,void* heap)
 data sLayer3DPerspectiveGuard 101FD6A0
 data sLayer3DPerspectiveType 101FDCDC
 ext 027B0EC0 u32 drawScenePacket(hdScenePacket_l* packet,u32 index,hdFilterViewport_l* viewport,hdFilterBounds_c* bounds,void* primary,void* target,void* secondary,u32 slot,u32 flags)
 ext 02782660 u32 drawSceneBlur(hdSceneBlur_l* blur,u32 index,hdFilterViewport_l* viewport,u32 slot,f32 nearPlane,f32 farPlane)
 ext 027B6D64 void flushSceneTarget(void* texture)
 ext 027BE794 void invalidateSceneTarget(void* texture)
 ext 02779120 u32 drawSceneFilter(hdSceneFilter_l* filter,u32 index,hdSceneFilterArg_c* argument,u32 slot)
 ext 02784778 u32 prepareScenePostprocess(hdScenePostprocess_l* postprocess,u32 slot)
 ext 0278519C u32 drawScenePostprocess(hdScenePostprocess_l* postprocess,hdFilterViewport_l* viewport,u32 slot)
 ext 027B6F18 void flushSceneDepth(hdSceneTexture_l* texture)
 ext 027BE794 void invalidateSceneDepth(hdSceneTexture_l* texture)
 ext 027B954C void initializeSceneViewport(hdSceneViewport_l* viewport)
 ext 025C0AB8 void bindSceneDepth(hdSceneTexture_l* destination,hdSceneTexture_l* source)
 ext C00063E8 void setSceneDepthFormat(void* state,BOOL enabled)
 ext 0274F5F0 hdFilterBounds_c* calculateSceneBounds(hdFilterBounds_c* bounds,hdSceneViewport_l* viewport)
 ext 0274F964 void applyCalculatedSceneBounds(hdFilterBounds_c* bounds,hdSceneViewport_l* viewport)
 ext 0274D690 void resetCalculatedSceneViewport(hdSceneViewport_l* viewport)
 ext 027F81A4 void* getScenePass(void* manager,u32 index)
 data sLayer3DScreens 101F95D0
 data sLayer3DDebugRenderer 104B45C0
 ext 027F29D4 hdFilterDebugRenderer_l* setupSceneDebugRenderer(void* renderer)
 ext 02736278 u32 prepareSceneShadow(void* layer,hdFilterRenderInfo_c* info)
 ext 0279A390 void setSceneBlendTarget(void* binding,hdFilterTargetArg_c* first,hdFilterTargetArg_c* second,f32 blend)
 data sLayer3DTvSize 1049FB6C
 str 10141EB0
 ext 0273AD10 void* allocateSceneLayer(u32 size)
 ext 027340D8 void constructSceneLayerBase(hdLayer3D_c* layer,SafeString* name)
 ext 02792A08 void constructScenePrimary(hdScenePrimaryBinding_l* binding)
 ext 027AE718 void constructScenePacket(hdScenePacket_l* packet)
 ext 02779BFC void constructSceneSecondary(hdSceneSecondaryBinding_l* binding)
 ext 0277EA5C void constructSceneBlur(hdSceneBlur_l* blur)
 ext 027759BC void constructSceneFilter(hdSceneFilter_l* filter)
 ext 02783A70 void constructScenePostprocess(hdScenePostprocess_l* postprocess)
 ext 027BE6B8 void constructSceneDescriptor(void* descriptor)
 ext 027931EC void destroyScenePrimary(hdScenePrimaryBinding_l* binding,s32 flags)
 ext 027AEC5C void destroyScenePacket(hdScenePacket_l* packet,s32 flags)
 ext 0277A488 void destroySceneSecondary(hdSceneSecondaryBinding_l* binding,s32 flags)
 ext 0277D290 void destroySceneBlur(hdSceneBlur_l* blur,s32 flags)
 ext 02774CA4 void destroySceneFilter(hdSceneFilter_l* filter,s32 flags)
 ext 027837F4 void destroyScenePostprocess(hdScenePostprocess_l* postprocess,s32 flags)
 ext 0274CCB0 void destroySceneCamera(hdSceneCamera_l* camera,s32 flags)
 ext 0274E038 void destroyScenePerspective(hdScenePerspective_l* projection,s32 flags)
 ext 0274E474 void destroySceneProjection(hdSceneProjection_l* projection,s32 flags)
 ext 02734248 void destroySceneLayerBase(void* layer,s32 flags)
 ext 0273AF40 void deleteSceneLayer(void* layer)
 data sLayer3DHeaderZero 1049F9EC
 data sLayer3DHeaderDescriptor 101F8834
 data sLayer3DRotationDescriptor 101F8840
 data sLayer3DPositiveRotationDescriptor 101F884C
 data sLayer3DMinRotation 1049F9E0
 data sLayer3DMaxRotation 1049F9E4
 data sLayer3DRotationGuard 1049F9E8
 data sLayer3DPositiveRotationGuard 1049F9E9
 ext 028ED6F8 void initializeRotationGuard(void* guard)
 ext 028EAB2C void initializePositiveRotationGuard(void* guard)
 data sLayer3DRenderManager 101F9968
 ext 027F84A4 void* finishScenePass(void* manager,u32 pass,hdFilterRenderInfo_c* info)
 ext 027BE114 void constructSceneTexture(hdFilterTexture_c* texture,void* source)
 ext 027BE2B0 void destroySceneTexture(hdFilterTexture_c* texture,s32 flags)
 ext 0277C578 u32 drawSceneSecondary(hdSceneSecondaryBinding_l* binding,u32 index,hdFilterViewport_l* viewport,hdFilterBounds_c* bounds,hdFilterTexture_c* texture,u32 first,u32 second,u32 third,u32 slot)
 ext 0268B070 void approachScenePhase(be<f32>* phase,f32 target,f32 step)
 data sLayer3DAngle 101F8D74
 data sLayer3DDisplay 101F86E8
 ext 027A7558 hdFilterTargetArg_c* getSceneTarget(hdFilterTargetArg_c* argument,void* target)
 ext 0279A308 void setSceneTarget(void* binding,hdFilterTargetArg_c* argument)
 ext 027F81C0 void* bindScenePass(void* manager,u32 index,void* pass)
 ext 025C0928 void bindScenePassResource(void* resource,void* pass)
 ext 027F88C8 void clearScenePass(void* manager,u32 index)
 ext 025200D4 hdScenePlay_l* getScenePlay()
 ext 0274D690 void resetSceneViewport(hdFilterViewport_l* viewport)
 ext 0274F964 void applySceneViewport(void* bounds,hdFilterViewport_l* viewport)
 ext 027F869C void executeScenePass(void* manager,u32 index,hdFilterRenderInfo_c* info)
 ext 0278FC80 hdSceneViewport_l* prepareSceneShadowViewport(hdSceneShadowRenderer_l* renderer,u32 index,u32 width,u32 height)
 ext 02795D7C u32 drawScenePrimary(hdScenePrimaryBinding_l* binding,u32 index,u32 width,u32 height,void* texture,u32 slot,f32 nearPlane,f32 farPlane)
 data sLayer3DOverlayManager 101F97D4
 ext 027B0254 void resetScenePacketDraw(hdScenePacket_l* packet,u32 index,u32 flags)
 ext 027B1498 void finishScenePacketDraw(hdScenePacket_l* packet,u32 index)
 ext 027F81C0 void setScenePassDimensions(void* manager,u32 pass,void* dimensions)
 ext 027ECCE0 void drawSceneOverlayManager(void* manager)
 data sLayer3DAuxiliaryClearColor 104A01CC
 ext 027B997C void clearSceneViewport(hdSceneViewport_l* viewport,u32 first,u32 second,void* color,u32 flags,f32 depth)
 ext 027F8974 void drawSceneManagedPass(void* manager,u32 index,hdFilterRenderInfo_c* info,u32 flags)
 ext 027F8B20 void remapScenePass(void* manager,u32 source,u32 destination)
 data sLayer3DParticleFlags 101EA4E4
 data sLayer3DSuppressLists 101F4829
 data sLayer3DParticleWorldView 1047B2A4
 ext 027E1A0C void resetSceneDepthBias(hdSceneDepthBias_c* bias)
 ext 027E1A14 void setSceneDepthBias(hdSceneDepthBias_c* bias,f32 value)
 ext 027E1A28 void setSceneDepthSlope(hdSceneDepthBias_c* bias,f32 value)
 ext 027E1A08 void applySceneDepthBias(hdSceneDepthBias_c* bias)
 data sLayer3DDefaultView 104A2098
 data sLayer3DDefaultProjection 104A20FC
 data sLayer3DParticleProjectionGuard 101FD9F0
 data sLayer3DParticleProjectionType 101FDCD4
 data sLayer3DParticleDebugState 104B474C
 ext 02750370 void resetSceneParticleDebug(void* state)
 ext 025986BC BOOL isSceneParticleDrawSuppressed()
 ext 028E8970 void copySceneProjectionMatrix(void* source,hdSceneProjectionMatrix_c* destination)
 ext 025A83AC void drawSceneParticles(void* particles,hdSceneParticleArg_c* argument,u32 mode)
 data sLayer3DOverlayEnabled 101F4825
 data sLayer3DOverlayAlpha 101F4826
 data sLayer3DOverlayMatrix 1048CF84
 data sLayer3DOverlayColor 104A0908
 ext 02732C9C void copySceneMatrix(hdSceneViewMatrix_c* destination,hdSceneViewMatrix_c* source)
 ext 0272CC50 u32 drawSceneOverlay(void* drawer,hdFilterTexture_c* texture,hdSceneViewMatrix_c* matrix,hdSceneColor_c* color,u32 slot)
 ext 027A90AC u32 drawSceneScaledTexture(hdFilterTexture_c* texture,hdFilterBounds_c* bounds,hdSceneSize_l* scale,hdSceneColor_c* color,u32 slot)
 data sLayer3DViewportGuard 101FD7BC
 data sLayer3DViewportType 101FDCF8
 data sLayer3DGraphicsContext 101F8BD8
 data sLayer3DDepthDrawer 101F8710
 ext 027B6F90 void resolveSceneDepth(hdSceneTexture_l* depth)
 ext C00061A0 void expandSceneDepth(void* format)
 ext 0276B114 void synchronizeSceneGraphics(void* context)
 ext 027F8E5C u32 getScenePassFormat(void* manager,u32 pass)
 ext 0272C4E0 u32 drawSceneDepthCopy(void* drawer,hdFilterTexture_c* texture,u32 slot,f32 nearPlane,f32 farPlane)
 data sLayer3DViewMatrix 104A0958
 data sLayer3DDrawColor 104A01EC
 ext 025C0AC4 void translateSceneView(hdSceneViewMatrix_c* destination,hdSceneViewMatrix_c* source,hdSceneVector_c* translation)
 ext 0274E1C8 void constructSceneOrthographic(hdSceneProjection_l* projection)
 ext 0274D83C void* getSceneProjectionMatrix(hdSceneProjection_l* projection)
 ext 027AAD8C u32 drawSceneTexture(hdFilterTexture_c* texture,hdSceneViewMatrix_c* view,void* projection,hdSceneColor_c* color,u32 slot)
 ext 0274FDBC void constructSceneColorState(hdSceneColorState_c* state)
 ext 0274FEB0 void applySceneColorState(hdSceneColorState_c* state)
 ext 027F1088 void drawSceneList(void* list,hdSceneDrawArgument_c* argument)
*/

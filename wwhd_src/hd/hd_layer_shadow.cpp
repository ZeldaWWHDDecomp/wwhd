/* HD shadow layer owns the cascaded shadow renderer and scene binding. Private decomp source. */
#include "hd/hd_layer_shadow.h"
static hdLayerShadowHeaderZero_c sLayerShadowHeaderZero;
static hdLayerShadowHeaderDescriptor_c sLayerShadowHeaderDescriptor;
static hdLayerShadowHeaderDescriptor_c sLayerShadowRotationDescriptor;
static hdLayerShadowHeaderDescriptor_c sLayerShadowPositiveRotationDescriptor;
static f32 sLayerShadowMinRotation;
static f32 sLayerShadowMaxRotation;
static u8 sLayerShadowRotationGuard;
static u8 sLayerShadowPositiveRotationGuard;
static void initializeRotationGuard(void* guard);
static void initializePositiveRotationGuard(void* guard);
static u32 sLayerShadowMultiFilterDrawVtable;
static u32 sLayerShadowSafeStringVtable;
static u32 sLayerShadowLayerVtable;
static u32 sLayerShadowDrawVtable;
static u32 sLayerShadowStepVtable;
static u32 sLayerShadowBaseDelegateVtable;
static u32 sLayerShadowDelegateVtable;
static constexpr u32 kLayerShadowBufferConstructor = 0x027FD6F4;
static constexpr u32 kLayerShadowBufferDestructor = 0x027FD764;
static u32 sLayerShadowDrawDelegateVtable;
static u32 sLayerShadowStepDelegateVtable;
static constexpr u32 kLayerShadowDrawFunction = 0x02736C10;
static constexpr u32 kLayerShadowFinishFunction = 0x02737F5C;
static u32 sLayerShadowProjectionName;
static u32 sLayerShadowViewportName;
static constexpr u32 kLayerShadowMatrixConstructor = 0x02738024;
static u32 sLayerShadowLineObjectVtable;
static u32 sLayerShadowLineVtable;
static u32 sLayerShadowLineCollisionVtable;
static u32 sLayerShadowLineGroupVtable;
static u32 sLayerShadowLineObjectBaseVtable;
static u32 sLayerShadowLineGroupBaseVtable;
static u32 sLayerShadowLineCollisionBaseVtable;
static void* allocateShadowLayer(u32 size);
static void constructShadowBase(hdLayerShadow_c* layer, SafeString* name);
static void constructShadowSurface(hdShadowSurface_l* surface);
static void constructShadowStep(hdShadowStep_c* step);
static void constructShadowBuffers(hdShadowBuffer_l* buffers, u32 count, u32 size, u32 constructor);
static void constructShadowRenderer(hdShadowRenderer_l* renderer);
static void* sLayerShadowRenderContext;
static cXyz sLayerShadowDefaultDirection;
static void prepareShadowResources(void* context);
static void finishShadowResources(void* context);
static void initializeShadowSurface(hdShadowSurface_l* surface, hdShadowPassArg_c* pass, void* heap);
static void setShadowResolution(void* resolution, hdShadowResolution_c* size);
static void setShadowDirection(cXyz* destination, cXyz* direction);
static void applyShadowSurface(hdShadowSurface_l* surface);
static void initializeShadowEnvironment(hdShadowEnvironmentParameters_l* environment, void* heap);
static void initializeShadowBuffer(hdShadowBuffer_l* buffer, u32 enabled, void* heap);
static void addShadowRenderStep(hdLayerShadow_c* layer, hdShadowStep_c* step);
static void initializeShadowRenderer(hdShadowRenderer_l* renderer, u32 enabled, void* heap);
static void drawShadowCascade(hdLayerShadow_c* layer, hdShadowCascadeInfo_c* info);
static hdShadowDebugRenderer_l sLayerShadowDebugState;
static u8 sLayerShadowDrawExtraCascades;
static void constructShadowDrawState(hdShadowDrawState_c* state);
static void applyShadowDrawState(hdShadowDrawState_c* state);
static void configureShadowBuffer(hdShadowBuffer_l* buffer, u32 index, void* view, void* projection, void* matrix);
static void bindShadowBuffer(hdShadowBuffer_l* buffer, u32 index);
static void drawShadowList(hdShadowDrawList_l* list, hdShadowDrawContext_l* context);
static void clearShadowDrawState(void* state);
static u8 sLayerShadowStageName;
static u32 sLayerShadowDemoFrame;
static hdShadowStageRange_c sLayerShadowStageRange;
static f32 sLayerShadowStageDepth;
static void normalizeShadowString(SafeString* name);
static u8 sLayerShadowDebugRenderer;
static hdFilterDebugRenderer_l* setupShadowDebugRenderer(void* renderer);
static void releaseShadowSurface(hdShadowSurface_l* surface);
static void destroyShadowSurface(hdShadowSurface_l* surface, s32 flags);
static void destroyShadowRenderer(hdShadowRenderer_l* renderer, s32 flags);
static void destroyShadowBuffers(hdShadowBuffer_l* buffers, u32 count, u32 size, u32 destructor, u32 extra, u32 flags);
static void destroyShadowStep(hdShadowStep_c* step, s32 flags);
static u32 bindShadowRenderer(hdShadowRenderer_l* renderer, u32 index, hdFilterViewport_l* viewport, void* target, void* target2, void* target3, u32 textureCount, u32 flags, u32 cascadeCount, u32 slot);
static void destroyShadowFilterSurface(hdFilterSurface_c* surface, s32 flags);
static void destroyShadowLayerBase(void* layer, s32 flags);
static void deleteShadowLayer(void* layer);
static void registerShadowStep(hdLayer3D_c* scene, u32 index, hdShadowStep_c* step);
static hdFilterTargetArg_c* getShadowTarget(hdFilterTargetArg_c* argument, void* target);
static void setShadowTarget(hdShadowBinding_l* binding, hdFilterTargetArg_c* argument);
static void finishShadowRenderer(hdShadowRenderer_l* renderer, u32 index);
static hdShadowLight_l* getShadowLight();
static hdShadowPlay_l* getShadowPlay();
static BOOL useShadowLightDirection(hdShadowLight_l* light);
static void addShadowVectors(cXyz* first, cXyz* second, cXyz* result);
static void calculateShadowDirection(cXyz* first, cXyz* second, cXyz* result);
static hdLayerMatrix_c sLayerShadowDefaultView;
static hdShadowPerspective_l sLayerShadowDefaultProjection;
static u32 sLayerShadowProjectionGuard;
static hdShadowType_c sLayerShadowProjectionType;
static u32 sLayerShadowViewportGuard;
static hdShadowType_c sLayerShadowViewportType;
static hdShadowTuning_c sLayerShadowTuning;
static f32 sLayerShadowDefaultDepth;
static hdShadowSave_l* sLayerShadowSave;
static void shadowVectorAdd(cXyz* first, cXyz* result, cXyz* second);
static void shadowVectorSubtract(cXyz* first, cXyz* result, cXyz* second);
static void shadowVectorScale(cXyz* first, cXyz* result, f32 scale);
static f32 shadowVectorSquareMagnitude(cXyz* vector);
static f32 shadowSquareRoot(f32 value);
static f32 clampShadowRange(f32 value, f32 minimum, f32 maximum);
static BOOL isShadowEvent(void* events, u32 bit);
static void constructShadowLineCheck(hdShadowLineCheck_c* line);
static void setShadowLineCheck(hdShadowLineCheck_c* line, cXyz* start, cXyz* end, void* actor);
static BOOL crossShadowLine(void* background, hdShadowLineCheck_c* line);
static void destroyShadowLineCheck(hdShadowLineCheck_c* line, s32 flags);
static void constructShadowMatrices(hdShadowMatrices_c* matrices, u32 count, u32 size, u32 constructor);
static void copyShadowMatrix(void* source, void* destination);
static void* getShadowProjectionMatrix(hdShadowPerspective_l* projection);
static void initializeShadowVolume(hdShadowVolume_c* volume, void* matrix, void* frustum, f32 nearPlane, f32 farPlane, f32 fieldOfView, f32 aspect);
static void transformShadowVolume(hdShadowVolume_c* volume, hdLayerMatrix_c* view);
static f32 shadowTangent(f32 value);
static void configureShadowVolume(hdShadowRenderer_l* renderer, u32 index, hdLayerMatrix_c* view, void* projection, hdShadowMatrices_c* matrices, hdShadowStageRange_c* distances, hdShadowVolume_c* volume, f32 nearPlane, f32 farPlane, f32 tangent, f32 aspect);
static void updateShadowSurface(hdShadowSurface_l* surface, hdLayerMatrix_c* view, void* projection);
static void prepareShadowSurface(hdShadowSurface_l* surface);
static void finalizeShadowSurface(hdShadowSurface_l* surface);
static BOOL isShadowSpecialStage();
static void uploadShadowEnvironment(hdShadowEnvironmentParameters_l* environment);
static void resetShadowDepthBias(hdShadowDepthBias_c* bias);
static void setShadowDepthBias(hdShadowDepthBias_c* bias, f32 value);
static void setShadowDepthSlope(hdShadowDepthBias_c* bias, f32 value);
static void applyShadowDepthBias(hdShadowDepthBias_c* bias);
static u32 drawShadowSurface(hdShadowSurface_l* surface, u32 slot);
static void resetShadowViewport(hdFilterViewport_l* viewport);
/* 02735D0C */
static s32 shadowNoDraw() { return 0; }
/* 02735D14 */
static s32 shadowNoUpdate() { return 0; }
/* 02735DE4 */
static void destroyShadowFilter(hdMultiFilter_c* layer, u32 flags) {
 if(layer != NULL) {
  layer->mDrawVtable = gabi::ea(&sLayerShadowMultiFilterDrawVtable);
  destroyShadowFilterSurface(&layer->mDrcSurface, 2);
  destroyShadowFilterSurface(&layer->mTvSurface, 2);
  destroyShadowLayerBase(layer, 0);
  if(flags&1) deleteShadowLayer(layer);
 }
}
/* 02735E5C */
static s32 shadowTargetCount() { return 2; }
/* Inlined constructors retain wrapping guest subobject addresses. */
static SafeString* initializeShadowName(hdShadowStep_c* step) {
 SafeString* name = gabi::at<SafeString>(gabi::ea(step)+offsetof(hdShadowStep_c, mName));
 if(name == NULL) name = (SafeString*)allocateShadowLayer(8);
 if(name != NULL) { name->__vtbl = gabi::ea(&sLayerShadowSafeStringVtable); name->mStringTop = gabi::guest_addr(""); }
 return name;
}
static void initializeShadowParameters(hdShadowParameters_c* parameters, f32 a, f32 b, f32 c, f32 d) {
 if(parameters == NULL) parameters = (hdShadowParameters_c*)allocateShadowLayer(16);
 if(parameters != NULL) {
  parameters->mValues[0] = a; parameters->mValues[1] = b;
  parameters->mValues[2] = c; parameters->mValues[3] = d;
 }
}
/* 02735F0C */
static hdLayerShadow_c* constructLayerShadow(hdLayerShadow_c* layer) {
 if(layer == NULL) {
  layer = (hdLayerShadow_c*)allocateShadowLayer(sizeof(hdLayerShadow_c));
  if(layer == NULL) return NULL;
 }
 SafeString name;
 name.__vtbl = gabi::ea(&sLayerShadowSafeStringVtable);
 name.mStringTop = gabi::guest_addr("Shadow");
 constructShadowBase(layer, &name);
 layer->mBase.mpVtable.v = gabi::ea(&sLayerShadowLayerVtable);
 layer->mBase.mDrawVtable = gabi::ea(&sLayerShadowDrawVtable);
 constructShadowSurface(&layer->mSurface);
 layer->mpDrawContext = NULL;
 constructShadowStep(&layer->mFinishStep);
 initializeShadowName(&layer->mFinishStep);
 layer->mFinishStep.mTarget = 0;
 layer->mFinishStep.mFlags = 0;
 layer->mFinishStep.mVtable = gabi::ea(&sLayerShadowStepVtable);
 hdShadowDelegate_c* delegate = gabi::at<hdShadowDelegate_c>(gabi::ea(layer)+offsetof(hdLayerShadow_c, mFinishStep)+offsetof(hdShadowStep_c, mDelegate));
 if(delegate != NULL) {
  hdShadowDelegate_c* constructed = delegate;
  if(constructed == NULL) constructed = (hdShadowDelegate_c*)allocateShadowLayer(16);
  if(constructed != NULL) {
   constructed->mFunction = 0;
   constructed->mAdjustment = 0;
   constructed->mVtable = gabi::ea(&sLayerShadowBaseDelegateVtable);
   constructed->mVirtualOffset = 0;
   constructed->mpOwner = NULL;
  }
  delegate->mVtable = gabi::ea(&sLayerShadowDelegateVtable);
 }
 layer->mpEnvironmentShadow = gabi::at<hdShadowEnvironmentParameters_l>(gabi::ea(getShadowLight())+offsetof(hdShadowEnvironment_l, mShadow));
 constructShadowBuffers(layer->mBuffers, 4, 12, kLayerShadowBufferConstructor);
 layer->mpScene = NULL;
 constructShadowRenderer(&layer->mRenderer);
 layer->mDistance = 80.0f;
 layer->mFadeDistance = 3.0f;
 layer->mCascadeCount = 3;
 initializeShadowParameters(gabi::at<hdShadowParameters_c>(gabi::ea(layer)+offsetof(hdLayerShadow_c, mCascadeDistances)), 1400.0f, 1600.0f, 3900.0f, 4100.0f);
 initializeShadowParameters(gabi::at<hdShadowParameters_c>(gabi::ea(layer)+offsetof(hdLayerShadow_c, mCascadeScales)), 0.5f, 2.0f, 4.0f, 2.0f);
 initializeShadowParameters(gabi::at<hdShadowParameters_c>(gabi::ea(layer)+offsetof(hdLayerShadow_c, mBiasParameters)), 1.25f, 1.75f, 0.0010000000474974513f, 0.00009999999747378752f);
 layer->mDepthBias = -0.20000000298023224f;
 layer->mBlend = 0.44999998807907104f;
 hdShadowResolution_c* resolution = gabi::at<hdShadowResolution_c>(gabi::ea(layer)+offsetof(hdLayerShadow_c, mResolution));
 if(resolution == NULL) resolution = (hdShadowResolution_c*)allocateShadowLayer(8);
 if(resolution != NULL) { resolution->mHeight = 1024; resolution->mWidth = 1024; }
 return layer;
}
/* 02736198 */
static void destroyLayerShadow(hdLayerShadow_c* layer, u32 flags) {
 if(layer == NULL) return;
 layer->mBase.mpVtable.v = gabi::ea(&sLayerShadowLayerVtable);
 layer->mBase.mDrawVtable = gabi::ea(&sLayerShadowDrawVtable);
 releaseShadowSurface(&layer->mSurface);
 layer->mSurface.mpDelegate = NULL;
 destroyShadowRenderer(&layer->mRenderer, 2);
 destroyShadowBuffers(layer->mBuffers, 4, 12, kLayerShadowBufferDestructor, 0, 0);
 destroyShadowStep(&layer->mFinishStep, 2);
 destroyShadowSurface(&layer->mSurface, 2);
 destroyShadowLayerBase(layer, 0);
 if(flags&1) deleteShadowLayer(layer);
}
/* 02736260 */
void hdLayerShadow_c::setScene(hdLayer3D_c* scene) {
 mpScene = scene;
 registerShadowStep(scene, 6, &mFinishStep);
}
/* 02736278 */
static u32 prepareShadowRenderer(hdLayerShadow_c* layer, hdFilterRenderInfo_c* info) {
 hdFilterDebugRenderer_l* debug = setupShadowDebugRenderer(&sLayerShadowDebugRenderer);
 u32 slot = debug->mTarget;
 if(layer->mpScene != NULL) {
  hdFilterViewport_l* viewport = info->mpViewport;
  void* target = ((hdShadowViewport_l*)viewport)->mpTarget;
  u32 textureCount = layer->mSurface.mTextureCount;
  u32 cascadeCount = layer->mSurface.mCascadeCount;
  slot = bindShadowRenderer(&layer->mRenderer, 0, viewport, target, target, target, textureCount, 0, cascadeCount, slot);
  hdLayer3D_c* scene = layer->mpScene;
  hdSceneShadowTextureOwner_l* owner = layer->mRenderer.mpTextureOwner;
  scene->mpShadowTexture = &owner->mTextures[layer->mRenderer.mUseFirstView ? 0 : 1];
 }
 return slot;
}
/* 02736328 */
void hdLayerShadow_c::setRenderTarget(void* target) {
 if(target == NULL) {
  mRenderer.mBinding.mEnabled = 0;
  return;
 }
 hdFilterTargetArg_c temporary;
 hdFilterTargetArg_c argument;
 argument.mTarget = getShadowTarget(&temporary, target)->mTarget;
 setShadowTarget(&mRenderer.mBinding, &argument);
}
/* 02737F5C */
void hdLayerShadow_c::finish() {
 if(mpScene != NULL) mpScene->mpShadowTexture = NULL;
 finishShadowRenderer(&mRenderer, 0);
}
/* 02736390 */
static void initializeLayerShadow(hdLayerShadow_c* layer, void* heap) {
 layer->mBase.mFlags = u32(layer->mBase.mFlags)|0x80;
 hdShadowPassArg_c pass;
 pass.mPass = 3;
 prepareShadowResources(sLayerShadowRenderContext);
 initializeShadowSurface(&layer->mSurface, &pass, heap);
 hdShadowResolution_c resolution;
 resolution.mWidth = layer->mResolution.mWidth;
 resolution.mHeight = layer->mResolution.mHeight;
 setShadowResolution(layer->mSurface.mResolution, &resolution);
 s32 count = layer->mCascadeCount;
 s32 capacity = layer->mSurface.mCapacity;
 if(capacity<count) count = capacity;
 layer->mSurface.mMode = 2;
 if(count<1) count = 1;
 layer->mSurface.mCascadeCount = count;
 finishShadowResources(sLayerShadowRenderContext);
 hdShadowDelegate_c* delegate = gabi::at<hdShadowDelegate_c>(gabi::ea(layer)+offsetof(hdLayerShadow_c, mSurface)+offsetof(hdShadowSurface_l, mDrawDelegate));
 if(delegate != NULL) {
  delegate->mpOwner = layer;
  delegate->mAdjustment = 0;
  delegate->mVtable = gabi::ea(&sLayerShadowDrawDelegateVtable);
  delegate->mFunction = gabi::guest_fn((const void*)drawShadowCascade);
  delegate->mVirtualOffset = -1;
 }
 layer->mSurface.mpDelegate = delegate;
 cXyz direction;
 direction.x = -sLayerShadowDefaultDirection.x;
 direction.y = -sLayerShadowDefaultDirection.y;
 direction.z = -sLayerShadowDefaultDirection.z;
 layer->mSurface.mDirectionEnabled = 1;
 setShadowDirection(&layer->mSurface.mDirection, &direction);
 layer->mSurface.mFlag1 = 1;
 layer->mSurface.mFlag2 = 1;
 layer->mSurface.mFlag4 = 0;
 layer->mSurface.mFlag3 = 1;
 layer->mSurface.mFlag0 = 1;
 applyShadowSurface(&layer->mSurface);
 initializeShadowEnvironment(layer->mpEnvironmentShadow, heap);
 for(u32 i = 0; i<3; ++i) initializeShadowBuffer(&layer->mBuffers[i], 1, heap);
 hdShadowStep_c* step = (hdShadowStep_c*)allocateShadowLayer(sizeof(hdShadowStep_c));
 if(step != NULL) {
  constructShadowStep(step);
  initializeShadowName(step);
  step->mFlags = 0;
  step->mVtable = gabi::ea(&sLayerShadowStepVtable);
  hdShadowDelegate_c* draw = gabi::at<hdShadowDelegate_c>(gabi::ea(step)+offsetof(hdShadowStep_c, mDelegate));
  if(draw != NULL) {
   draw->mpOwner = layer;
   draw->mAdjustment = 0;
   draw->mVirtualOffset = -1;
   draw->mVtable = gabi::ea(&sLayerShadowStepDelegateVtable);
   draw->mFunction = kLayerShadowDrawFunction;
  }
  step->mTarget = 2;
  step->mName.mStringTop = gabi::guest_addr("LayerShadow");
 }
 addShadowRenderStep(layer, step);
 delegate = gabi::at<hdShadowDelegate_c>(gabi::ea(layer)+offsetof(hdLayerShadow_c, mFinishStep)+offsetof(hdShadowStep_c, mDelegate));
 if(delegate != NULL) {
  delegate->mVtable = gabi::ea(&sLayerShadowStepDelegateVtable);
  delegate->mpOwner = layer;
  delegate->mVirtualOffset = -1;
  delegate->mFunction = kLayerShadowFinishFunction;
  delegate->mAdjustment = 0;
 }
 layer->mFinishStep.mTarget = 2;
 layer->mFinishStep.mName.mStringTop = gabi::guest_addr("LayerShadow::freeShadowTexture_");
 layer->mFinishStep.mFlags = 100;
 initializeShadowRenderer(&layer->mRenderer, 1, heap);
 hdShadowEnvironmentParameters_l* environment = layer->mpEnvironmentShadow;
 for(u32 i = 0; i<4; ++i) environment->mBiasParameters[i] = layer->mBiasParameters.mValues[i];
 environment = layer->mpEnvironmentShadow;
 environment->mDepthBias = layer->mDepthBias;
 environment = layer->mpEnvironmentShadow;
 for(u32 i = 0; i<4; ++i) environment->mCascadeScales[i] = layer->mCascadeScales.mValues[i];
 environment = layer->mpEnvironmentShadow;
 environment->mColors[0][1] = -0.8f;
 environment->mColors[0][3] = 0.15f;
 environment->mColors[0][0] = -0.1f;
 environment->mColors[0][2] = 0.6f;
 environment = layer->mpEnvironmentShadow;
 environment->mColors[1][2] = 0.5f;
 environment->mColors[1][0] = -0.5f;
 environment->mColors[1][1] = -0.1f;
 environment->mColors[1][3] = 0.7f;
 environment = layer->mpEnvironmentShadow;
 environment->mColors[2][0] = -0.4f;
 environment->mColors[2][2] = 1.0f;
 environment->mColors[2][3] = -0.3f;
 environment->mColors[2][1] = 0.2f;
 environment = layer->mpEnvironmentShadow;
 environment->mColors[3][0] = 2.0f;
 environment->mColors[3][1] = 4.0f;
 environment->mColors[3][2] = 0.8f;
 environment->mColors[3][3] = 0.3f;
}
/* 02736750 */
static void getShadowDirection(void*, cXyz* direction) {
 hdShadowLight_l* light = getShadowLight();
 hdShadowCamera_l* camera = getShadowPlay()->mpCamera;
 if(light != NULL && camera != NULL) {
  cXyz result;
  if(useShadowLightDirection(light)) {
   cXyz endpoint;
   endpoint.x = light->mDirection.x;
   endpoint.y = light->mDirection.y;
   endpoint.z = light->mDirection.z;
   if(endpoint.y < 0.0f) endpoint.y = -endpoint.y;
   addShadowVectors(&endpoint, &camera->mPosition, &endpoint);
   calculateShadowDirection(&camera->mPosition, &endpoint, &result);
  } else {
   calculateShadowDirection(&camera->mPosition, &light->mPosition, &result);
  }
  direction->x = result.x;
  direction->y = result.y;
  direction->z = result.z;
 } else {
  direction->x = 0.0f;
  direction->y = -1.0f;
  direction->z = -1.0f;
 }
}
static BOOL shadowStageEquals(SafeString* name, SafeString* stage) {
 normalizeShadowString(name);
 hdShadowSafeStringVtable_l* nameVtable = gabi::at<hdShadowSafeStringVtable_l>(name->__vtbl);
 gabi::call_ptr(nameVtable->mEnsureTerminated, name);
 hdShadowSafeStringVtable_l* stageVtable = gabi::at<hdShadowSafeStringVtable_l>(stage->__vtbl);
 u32 first = name->mStringTop;
 gabi::call_ptr(stageVtable->mEnsureTerminated, stage);
 u32 second = stage->mStringTop;
 if(first == second) return TRUE;
 for(u32 i = 0; i<0x40001; ++i) {
  u8 a = *gabi::at<be<u8>>(first+i);
  u8 b = *gabi::at<be<u8>>(second+i);
  if(a != b) return FALSE;
  if(a == 0) return TRUE;
 }
 return FALSE;
}
/* 0273687C */
static BOOL configureShadowDemoRange() {
 SafeString firstName;
 SafeString firstStage;
 firstName.mStringTop = gabi::guest_addr("Demo02");
 firstName.__vtbl = gabi::ea(&sLayerShadowSafeStringVtable);
 firstStage.mStringTop = gabi::ea(&sLayerShadowStageName);
 firstStage.__vtbl = gabi::ea(&sLayerShadowSafeStringVtable);
 if(shadowStageEquals(&firstName, &firstStage)) {
  u32 frame = sLayerShadowDemoFrame;
  if(frame-0x469u<0x82u || frame-0x596u<0x8Du) {
   sLayerShadowStageRange.mMiddle = 300.0f;
   sLayerShadowStageRange.mNear = 30.0f;
   return TRUE;
  }
 }
 SafeString secondName;
 SafeString secondStage;
 secondName.mStringTop = gabi::guest_addr("Demo06");
 secondName.__vtbl = gabi::ea(&sLayerShadowSafeStringVtable);
 secondStage.mStringTop = gabi::ea(&sLayerShadowStageName);
 secondStage.__vtbl = gabi::ea(&sLayerShadowSafeStringVtable);
 if(shadowStageEquals(&secondName, &secondStage)) {
  u32 frame = sLayerShadowDemoFrame;
  if(frame-1u<0x5Au) {
   sLayerShadowStageRange.mNear = 30.0f;
   sLayerShadowStageRange.mFar = 1000.0f;
   sLayerShadowStageRange.mMiddle = 200.0f;
   sLayerShadowStageDepth = 2000.0f;
  }
  if(frame-0x39Du<0x63u) {
   sLayerShadowStageRange.mNear = 30.0f;
   sLayerShadowStageRange.mMiddle = 300.0f;
   sLayerShadowStageRange.mFar = 700.0f;
   return TRUE;
  }
 }
 SafeString thirdName;
 SafeString thirdStage;
 thirdName.mStringTop = gabi::guest_addr("Demo25");
 thirdName.__vtbl = gabi::ea(&sLayerShadowSafeStringVtable);
 thirdStage.mStringTop = gabi::ea(&sLayerShadowStageName);
 thirdStage.__vtbl = gabi::ea(&sLayerShadowSafeStringVtable);
 if(shadowStageEquals(&thirdName, &thirdStage)) {
  if(u32(sLayerShadowDemoFrame)-0xB40u<0x78u) {
   sLayerShadowStageRange.mNear = 30.0f;
   sLayerShadowStageRange.mMiddle = 70.0f;
   sLayerShadowStageRange.mFar = 500.0f;
   return TRUE;
  }
 }
 return FALSE;
}
static hdShadowDrawPlay_l* getShadowDrawPlay() { return (hdShadowDrawPlay_l*)getShadowPlay(); }
static hdShadowDrawContext_l* getShadowDrawContext(hdLayerShadow_c* layer) { return (hdShadowDrawContext_l*)layer->mpDrawContext.get(); }
static void calculateShadowRanges(hdLayerShadow_c* layer, hdShadowPerspective_l* projection, hdShadowRangeState_c* range) {
 range->mNearRange = 1000.0f; range->mFarRange = 4000.0f; range->mScale = 1.0f;
 range->mAdaptive = 0; range->mDynamic = 0; range->mSpecialCamera = 0;
 s32 index = getShadowPlay()->mBackgroundIndex;
 gptr<hdShadowCamera_l>* cameraSlot = gabi::at<gptr<hdShadowCamera_l>>(gabi::ea(getShadowPlay())+offsetof(hdShadowPlay_l, mpCamera)+u32(index)*0x34);
 if(cameraSlot->get() == NULL) return;
 hdShadowCamera_l* camera = getShadowPlay()->mpCamera;
 gabi::Local<cXyz> eyeStorage;
 cXyz& eye = *eyeStorage;
 gabi::Local<cXyz> targetStorage;
 cXyz& target = *targetStorage;
 gabi::Local<cXyz> directionStorage;
 cXyz& direction = *directionStorage;
 shadowVectorAdd(&camera->mEye, &eye, &camera->mEyeOffset);
 shadowVectorAdd(&camera->mTarget, &target, &camera->mTargetOffset);
 shadowVectorSubtract(&eye, &direction, &target);
 f32 height = gabi::fsubs_ppc(gabi::fadds_ppc(camera->mHeight, camera->mHeightOffset), sLayerShadowTuning.mHeightOffset);
 u32 mode = camera->mMode;
 BOOL special = mode == 4 || mode == 10 || mode == 11 || mode == 14;
 BOOL specialCamera = mode == 14;
 u32 playFlags = getShadowPlay()->mEventFlags;
 BOOL event = isShadowEvent(&sLayerShadowSave->mEvents, 0x408) != 0;
 if(sLayerShadowTuning.mDisableSpecial) { if(specialCamera) special = 0; specialCamera = 0; }
 range->mSpecialCamera = specialCamera;
 if(configureShadowDemoRange()) { range->mAdaptive = 1; range->mDynamic = 1; return; }
 if(event) {
  sLayerShadowStageRange.mNear = 30.0f; sLayerShadowStageRange.mMiddle = 300.0f; sLayerShadowStageRange.mFar = 1000.0f;
  range->mFarRange = 1000.0f; return;
 }
 if(playFlags&0x100000) {
  sLayerShadowStageRange.mMiddle = 300.0f; sLayerShadowStageRange.mFar = 800.0f;
  range->mFarRange = 800.0f; range->mDynamic = 1; return;
 }
 gabi::Local<hdShadowLineCheck_c> lineStorage;
 hdShadowLineCheck_c& line = *lineStorage;
 constructShadowLineCheck(&line);
 line.mpObjectVtable = &line.mObjectVtable;
 line.mpGroupVtable = &line.mGroupVtable;
 line.mObjectVtable = gabi::ea(&sLayerShadowLineObjectVtable);
 line.mGroupVtable = gabi::ea(&sLayerShadowLineGroupVtable); line.mGroupMask = 1;
 line.mLineVtable = gabi::ea(&sLayerShadowLineVtable); line.mCollisionVtable = gabi::ea(&sLayerShadowLineCollisionVtable);
 for(u32 i = 0; i<7; ++i) line.mFlags[i] = 0;
 line.mFlags[2] = 1;
 gabi::Local<cXyz> lineStartStorage;
 cXyz& lineStart = *lineStartStorage;
 gabi::Local<cXyz> lineOriginStorage;
 cXyz& lineOrigin = *lineOriginStorage;
 gabi::Local<cXyz> lineDirectionStorage;
 cXyz& lineDirection = *lineDirectionStorage;
 gabi::Local<cXyz> lineEndStorage;
 cXyz& lineEnd = *lineEndStorage;
 shadowVectorAdd(&camera->mTarget, &lineStart, &camera->mTargetOffset);
 shadowVectorAdd(&camera->mEye, &lineOrigin, &camera->mEyeOffset);
 shadowVectorScale(&direction, &lineDirection, 1000.0f);
 shadowVectorAdd(&lineOrigin, &lineEnd, &lineDirection);
 setShadowLineCheck(&line, &lineStart, &lineEnd, NULL);
 BOOL hit = 0;
 f32 hitDistance = -1.0f;
 if(crossShadowLine(getShadowPlay()->mBackground, &line)) {
  gabi::Local<cXyz> originStorage;
 cXyz& origin = *originStorage;
  gabi::Local<cXyz> differenceStorage;
 cXyz& difference = *differenceStorage;
  shadowVectorAdd(&camera->mTarget, &origin, &camera->mTargetOffset);
  shadowVectorSubtract(&line.mHit, &difference, &origin);
  hitDistance = shadowSquareRoot(shadowVectorSquareMagnitude(&difference));
  if(hitDistance<1600.0f && specialCamera) { height = -10.0f; specialCamera = 0; range->mSpecialCamera = 0; }
  if(projection != NULL) hitDistance = gabi::fsubs_ppc(hitDistance, projection->mNear);
  hit = 1;
 }
 line.mObjectVtable = gabi::ea(&sLayerShadowLineObjectBaseVtable); line.mGroupVtable = gabi::ea(&sLayerShadowLineGroupBaseVtable);
 line.mCollisionVtable = gabi::ea(&sLayerShadowLineCollisionBaseVtable);
 destroyShadowLineCheck(&line, 0);
 BOOL high = sLayerShadowTuning.mHighMode != 0;
 f32 heightAdjustment;
 if(high) heightAdjustment = clampShadowRange(-gabi::fmuls_ppc(height, sLayerShadowTuning.mHighHeightScale), sLayerShadowTuning.mHighClampMinimum, sLayerShadowTuning.mHighClampMaximum);
 else heightAdjustment = clampShadowRange(-gabi::fmuls_ppc(height, sLayerShadowTuning.mNormalHeightScale), sLayerShadowTuning.mNormalClampMinimum, sLayerShadowTuning.mMiddleMaximum);
 f32 middle;
 if(special) {
  if(specialCamera) {
   if(hit) {
    sLayerShadowStageRange.mMiddle = 1000.0f;
    f32 distance = gabi::fsubs_ppc(hitDistance, 1000.0f);
    if(!(gabi::fsubs_ppc(sLayerShadowStageDepth, hitDistance)<distance)) distance = gabi::fadds_ppc(hitDistance, 1000.0f);
    sLayerShadowStageRange.mFar = distance;
    sLayerShadowDrawExtraCascades = 1; range->mFarRange = 1000.0f; range->mDynamic = 1; return;
   }
   sLayerShadowStageRange.mMiddle = 300.0f; sLayerShadowStageRange.mFar = 1000.0f;
   range->mNearRange = 300.0f; range->mFarRange = 1000.0f; range->mAdaptive = 1; range->mDynamic = 1; return;
  }
  sLayerShadowStageRange.mNear = 40.0f;
  middle = 200.0f;
 } else {
  sLayerShadowStageRange.mNear = high?sLayerShadowTuning.mHighNear:sLayerShadowTuning.mDefaultNear;
  middle = gabi::fmuls_ppc(shadowSquareRoot(shadowVectorSquareMagnitude(&direction)), sLayerShadowTuning.mNormalDistanceScale);
  if(specialCamera) { sLayerShadowStageRange.mMiddle = middle; return; }
  high = sLayerShadowTuning.mHighMode != 0;
 }
 if(high) {
  middle = clampShadowRange(gabi::fsubs_ppc(sLayerShadowTuning.mHighMiddleBase, heightAdjustment), sLayerShadowTuning.mHighMiddleMinimum, sLayerShadowTuning.mHighMiddleMaximum);
  range->mAdaptive = 1; range->mNearRange = middle; sLayerShadowStageRange.mMiddle = middle;
 } else if(special) {
  range->mNearRange = middle; range->mFarRange = sLayerShadowTuning.mMiddleLimit;
  sLayerShadowStageRange.mMiddle = middle;
  if(middle<(f32)range->mFarRange) range->mAdaptive = 1;
 } else {
  middle = clampShadowRange(gabi::fnmsubs(heightAdjustment, 4.0f, middle), sLayerShadowTuning.mMiddleMinimum, sLayerShadowTuning.mMiddleLimit);
  range->mNearRange = middle; range->mFarRange = sLayerShadowTuning.mMiddleLimit;
  sLayerShadowStageRange.mMiddle = middle;
  if(middle<(f32)range->mFarRange) range->mAdaptive = 1;
 }
 range->mDynamic = 1;
 if(special) range->mFarRange = gabi::fadds_ppc(middle, sLayerShadowTuning.mFarExtension);
 else {
  f32 farLimit = sLayerShadowTuning.mFarLimit;
  f32 distance = gabi::fmuls_ppc(gabi::fsubs_ppc(sLayerShadowStageRange.mMiddle, sLayerShadowStageRange.mNear), (farLimit/(f32)sLayerShadowTuning.mMiddleLimit));
  range->mScale = (distance/farLimit);
  range->mFarRange = gabi::fadds_ppc(middle, clampShadowRange(distance, sLayerShadowTuning.mFarExtension, farLimit));
 }
 sLayerShadowStageRange.mFar = range->mFarRange;
}
/* 02736C10 */
static void drawShadowPass(hdLayerShadow_c* layer, hdFilterRenderInfo_c* info) {
 u32 flags = layer->mBase.mFlags;
 hdLayerMatrix_c* view;
 if((flags&0xC0) == 0xC0) view = gabi::at<hdLayerMatrix_c>(gabi::ea(layer)+offsetof(hdLayerBase_c, mViewMatrix));
 else { view = layer->mBase.mpViewMatrix; if(view == NULL) view = &sLayerShadowDefaultView; }
 hdShadowPerspective_l* projection;
 if(flags&0x1000) projection = (hdShadowPerspective_l*)layer->mBase.mpInternalProjection.get();
 else { projection = (hdShadowPerspective_l*)layer->mBase.mpProjection.get(); if(projection == NULL) projection = &sLayerShadowDefaultProjection; }
 if(sLayerShadowProjectionGuard == 0) { sLayerShadowProjectionGuard = 1; sLayerShadowProjectionType.mName = gabi::ea(&sLayerShadowProjectionName); }
 if(projection != NULL && !gabi::call_ptr<BOOL>(projection->mpVtable->mIsKindOf, projection, &sLayerShadowProjectionType)) projection = NULL;
 u32 pass = info->mPass;
 if(pass == 3) { hdLayer3D_c* scene = layer->mpScene; if(scene != NULL) scene->drawConfiguredPass(info); return; }
 if(pass == 2) {
  if(view == NULL || projection == NULL) return;
  hdShadowMatrices_c matrices;
  constructShadowMatrices(&matrices, 3, sizeof(hdShadowMatrix_c), kLayerShadowMatrixConstructor);
  hdShadowStageRange_c distances;
  for(u32 i = 0; i<3; ++i) {
   hdShadowView_l* cascade = layer->mSurface.mpViews;
   if(i<(u32)layer->mSurface.mCapacity) cascade += i;
   copyShadowMatrix(cascade->mShadowMatrix, &matrices.mMatrices[i]);
   distances.mValues[i] = layer->mSurface.mpDistances[i+1];
  }
  void* projectionMatrix = getShadowProjectionMatrix(projection);
  hdShadowVolume_c volume;
  volume.mMaximum.x = -3.4028234663852886e+38f; volume.mMaximum.z = -3.4028234663852886e+38f;
  volume.mMinimum.x = 3.4028234663852886e+38f; volume.mMaximum.y = -3.4028234663852886e+38f;
  volume.mMinimum.y = 3.4028234663852886e+38f; volume.mMinimum.z = 3.4028234663852886e+38f;
  initializeShadowVolume(&volume, projectionMatrix, projection->mFrustum, projection->mNear, projection->mFar, projection->mFieldOfView, projection->mAspect);
  transformShadowVolume(&volume, view);
  f32 tangent = shadowTangent(gabi::fmuls_ppc(projection->mFieldOfView, 0.5f));
  projectionMatrix = getShadowProjectionMatrix(projection);
  configureShadowVolume(&layer->mRenderer, 0, view, projectionMatrix, &matrices, &distances, &volume, projection->mNear, projection->mFar, tangent, projection->mAspect);
  cXyz direction;
  getShadowDirection(layer, &direction);
  layer->mRenderer.mLightDirection.z = direction.z; layer->mRenderer.mLightDirection.x = direction.x;
  layer->mRenderer.mLightDirection.y = direction.y; layer->mRenderer.mDirectionWeight = 1.0f;
  return;
 }
 if(pass != 0 || getShadowDrawPlay()->mDrawWorld == 0) return;
 auto nativeFrame = gabi::AutoFrame(gabi::guest_fn((const void*)drawShadowPass));
 gabi::FrameLocal<hdShadowDrawArgument_c> contextStorage(0x30);
 hdShadowDrawArgument_c& context = *contextStorage;
 context.mpInfo = info; context.mEnabled = 1; context.mUnknown = 0; context.mIndex = 0;
 for(u32 i = 0; i<9; ++i) context.mWords[i] = 0;
 layer->mpDrawContext = &context;
 u32 slot = setupShadowDebugRenderer(&sLayerShadowDebugRenderer)->mTarget;
 cXyz direction;
 getShadowDirection(layer, &direction);
 layer->mSurface.mDirectionEnabled = 1;
 direction.z = -(f32)direction.z;
 direction.x = gabi::fmuls_ppc(direction.x, -1.0f); direction.y = gabi::fmuls_ppc(direction.y, -1.0f);
 setShadowDirection(&layer->mSurface.mDirection, &direction);
 s32 cascadeCount = layer->mSurface.mCascadeCount;
 sLayerShadowStageRange.mNear = sLayerShadowTuning.mDefaultRange.mNear;
 sLayerShadowStageRange.mMiddle = sLayerShadowTuning.mDefaultRange.mMiddle;
 sLayerShadowStageDepth = sLayerShadowDefaultDepth;
 sLayerShadowStageRange.mFar = sLayerShadowTuning.mDefaultRange.mFar;
 hdShadowRangeState_c range;
 calculateShadowRanges(layer, projection, &range);
 if(view != NULL && projection != NULL) {
  for(s32 i = 0; i<cascadeCount; ++i) {
   layer->mSurface.mpDistances[i] = gabi::at<be<f32>>(gabi::ea(&sLayerShadowStageRange)+u32(i)*4)->get();
   u32 capacity = layer->mSurface.mCapacity;
   hdShadowView_l* cascade = layer->mSurface.mpViews;
   u32 mode;
   if(i <= 0) mode = sLayerShadowTuning.mFirstMode?1:0;
   else mode = sLayerShadowTuning.mOtherMode?0:2;
   if(u32(i)<capacity) cascade += i;
   cascade->mMode = mode;
  }
  layer->mSurface.mpDistances[(u32)layer->mSurface.mCascadeCount] = sLayerShadowStageDepth;
  updateShadowSurface(&layer->mSurface, view, getShadowProjectionMatrix(projection));
  f32 nearPlane = projection->mNear, farPlane = projection->mFar;
  f32 tangent = shadowTangent(gabi::fmuls_ppc(projection->mFieldOfView, 0.5f));
  f32 distance = gabi::fsubs_ppc(farPlane, nearPlane);
  hdShadowEnvironmentParameters_l* environment = layer->mpEnvironmentShadow;
  environment->mRange = distance; environment->mTanHalfFieldOfView = tangent;
  environment->mNear = nearPlane; environment->mInverseRange = 1.0f/distance;
 }
 prepareShadowSurface(&layer->mSurface); finalizeShadowSurface(&layer->mSurface);
 hdShadowEnvironmentParameters_l* environment = layer->mpEnvironmentShadow;
 be<f32>* environmentDistances = environment->mDistances;
 for(u32 i = 0; i<3; ++i) {
  hdShadowView_l* cascade = layer->mSurface.mpViews;
  if(i<(u32)layer->mSurface.mCapacity) cascade += i;
  copyShadowMatrix(cascade->mShadowMatrix, environment->mMatrices[i]);
  f32 distance = 3.4028234663852886e+38f;
  if((s32)(i+1)<(s32)layer->mSurface.mCascadeCount) distance = layer->mSurface.mpDistances[i+1];
  environmentDistances[i] = distance;
  if(i<2) environment = layer->mpEnvironmentShadow;
 }
 f32 nearStart = layer->mCascadeDistances.mValues[0], nearEnd = layer->mCascadeDistances.mValues[1];
 f32 farStart = layer->mCascadeDistances.mValues[2], farEnd = layer->mCascadeDistances.mValues[3];
 if(range.mAdaptive) { nearStart = range.mNearRange; nearEnd = gabi::fadds_ppc(range.mNearRange, sLayerShadowTuning.mNearExtension); }
 if(range.mDynamic) {
  farEnd = range.mFarRange;
  if(range.mSpecialCamera) farStart = gabi::fsubs_ppc(farEnd, 300.0f);
  else farStart = gabi::fnmsubs(1000.0f, range.mScale, farEnd);
 }
 hdShadowPlay_l* play = getShadowPlay();
 void* stageInfo = gabi::call_ptr<void*>(play->mpStageVtable->mGetStageInfo, &play->mpStageVtable);
 if(stageInfo != NULL) {
  BOOL specialStage = isShadowSpecialStage();
  environment = layer->mpEnvironmentShadow;
  if(specialStage) {
   environment->mFadeParameters[1] = 1; environment->mFadeParameters[2] = -1;
   environment->mFadeParameters[0] = -1; environment->mFadeParameters[3] = 1;
  } else {
   environment->mFadeParameters[2] = farStart; environment->mFadeParameters[3] = 1.0f/gabi::fsubs_ppc(farEnd, farStart);
   if(range.mSpecialCamera) { environment->mFadeParameters[1] = 1; environment->mFadeParameters[0] = -1; }
   else { environment->mFadeParameters[0] = nearStart; environment->mFadeParameters[1] = 1.0f/gabi::fsubs_ppc(nearEnd, nearStart); }
  }
 } else {
  environment = layer->mpEnvironmentShadow;
  environment->mFadeParameters[0] = nearStart; environment->mFadeParameters[1] = 1.0f/gabi::fsubs_ppc(nearEnd, nearStart);
  environment->mFadeParameters[2] = farStart; environment->mFadeParameters[3] = 1.0f/gabi::fsubs_ppc(farEnd, farStart);
 }
 void* matrix = layer->mpScene->mpShadowMatrix;
 if(matrix != NULL) copyShadowMatrix(matrix, layer->mpEnvironmentShadow->mSceneMatrix);
 layer->mpEnvironmentShadow->mOcclusion = 0;
 layer->mpEnvironmentShadow->mIntensity = 0;
 layer->mpEnvironmentShadow->mFade = 0.5f;
 hdShadowLight_l* light = getShadowLight();
 environment = layer->mpEnvironmentShadow;
 environment->mFade = gabi::fmuls_ppc(environment->mFade, gabi::fsubs_ppc(1.0f, light->mFade));
 hdShadowLight_l* firstLight = getShadowLight();
 hdShadowLight_l* secondLight = getShadowLight();
 environment = layer->mpEnvironmentShadow;
 f32 intensity = gabi::fmuls_ppc(firstLight->mIntensity, secondLight->mBlend);
 environment->mIntensity = gabi::fnmsubs(intensity, gabi::fsubs_ppc(1.0f, environment->mIntensity), 1.0f);
 environment = layer->mpEnvironmentShadow;
 f32 occlusion = gabi::fsubs_ppc(1.0f, environment->mOcclusion);
 occlusion = gabi::fmuls_ppc(occlusion, gabi::fsubs_ppc(1.0f, getShadowLight()->mOcclusion));
 environment->mOcclusion = gabi::fnmsubs(occlusion, getShadowLight()->mBlend, 1.0f);
 u32 viewportGuard = sLayerShadowViewportGuard;
 hdShadowViewport_l* viewport = (hdShadowViewport_l*)info->mpViewport.get();
 if(viewportGuard == 0) { sLayerShadowViewportGuard = 1; sLayerShadowViewportType.mName = gabi::ea(&sLayerShadowViewportName); }
 if(viewport != NULL && !gabi::call_ptr<BOOL>(viewport->mpVtable->mIsKindOf, viewport, &sLayerShadowViewportType)) viewport = NULL;
 f32 width = gabi::fsubs_ppc(gabi::at<be<f32>>(gabi::ea(viewport)+offsetof(hdShadowViewport_l, mMaxX))->get(), gabi::at<be<f32>>(gabi::ea(viewport)+offsetof(hdShadowViewport_l, mMinX))->get());
 f32 height = gabi::fsubs_ppc(gabi::at<be<f32>>(gabi::ea(viewport)+offsetof(hdShadowViewport_l, mMaxY))->get(), gabi::at<be<f32>>(gabi::ea(viewport)+offsetof(hdShadowViewport_l, mMinY))->get());
 environment = layer->mpEnvironmentShadow;
 environment->mWidth = width; environment->mInverseWidth = 1.0f/width;
 environment->mHeight = height; environment->mInverseHeight = 1.0f/height;
 uploadShadowEnvironment(layer->mpEnvironmentShadow);
 hdShadowDepthBias_c bias;
 resetShadowDepthBias(&bias); setShadowDepthBias(&bias, layer->mFadeDistance); setShadowDepthSlope(&bias, layer->mDistance); applyShadowDepthBias(&bias);
 slot = drawShadowSurface(&layer->mSurface, slot);
 resetShadowDepthBias(&bias); applyShadowDepthBias(&bias);
 layer->mpScene->mpSceneTexture.v = layer->mSurface.mTextureCount;
 layer->mpScene->mpAuxiliaryTexture = layer->mpEnvironmentShadow.get();
 hdFilterDebugRenderer_l* debug = setupShadowDebugRenderer(&sLayerShadowDebugRenderer);
 debug->mTarget = slot; debug->mFlags = 0;
 resetShadowViewport(info->mpViewport);
 layer->mpDrawContext = NULL;
}
/* 02737D54 */
static void drawShadowCascade(hdLayerShadow_c* layer, hdShadowCascadeInfo_c* info) {
 /* The context retains this pointer when a callback replaces mpDrawContext. */
 auto nativeFrame = gabi::AutoFrame(gabi::guest_fn((const void*)drawShadowCascade));
 gabi::FrameLocal<hdShadowDrawState_c> stateStorage(8);
 hdShadowDrawState_c& state = *stateStorage;
 constructShadowDrawState(&state);
 state.mEnabled = 1;
 state.mMaskGreen = 1;
 state.mAlpha = 0;
 state.mBlend = 1;
 state.mMaskBlue = 0;
 state.mRed = 0;
 state.mAlphaEnabled = 0;
 state.mMaskRed = 1;
 state.mGreen = 0;
 state.mBlue = 0;
 state.mDepthEnabled = 1;
 state.mMode = 1;
 applyShadowDrawState(&state);
 hdShadowDebugState_l* shadow = sLayerShadowDebugState.mpShadowState;
 if(shadow == NULL) return;
 hdShadowViewGroup_l* group = info->mpGroup;
 u32 index = info->mIndex;
 u32 viewAddress = gabi::ea(group->mpViews);
 if(index<u32(group->mCount)) viewAddress += index*sizeof(hdShadowView_l);
 hdShadowView_l* view = gabi::at<hdShadowView_l>(viewAddress);
 /* The binary independently selects the view again for the projection pointer. */
 u32 projectionAddress = gabi::ea(group->mpViews);
 if(index<u32(group->mCount)) projectionAddress += index*sizeof(hdShadowView_l);
 hdShadowView_l* projection = gabi::at<hdShadowView_l>(projectionAddress);
 hdShadowBuffer_l* buffer = gabi::at<hdShadowBuffer_l>(gabi::ea(&layer->mBuffers[0])+index*sizeof(hdShadowBuffer_l));
 configureShadowBuffer(buffer, 0, view->mViewMatrix, projection->mProjectionMatrix, &shadow->mMatrix);
 index = info->mIndex;
 buffer = gabi::at<hdShadowBuffer_l>(gabi::ea(&layer->mBuffers[0])+index*sizeof(hdShadowBuffer_l));
 getShadowDrawContext(layer)->mpBuffer = buffer;
 bindShadowBuffer(getShadowDrawContext(layer)->mpBuffer, 0);
 getShadowDrawContext(layer)->mpState = &state;
 getShadowDrawContext(layer)->mFlags = 0;
 getShadowDrawContext(layer)->mIndex = info->mIndex;
 if(getShadowDrawPlay()->mDrawWorld) {
  hdShadowDrawContext_l* context = getShadowDrawContext(layer);
  hdShadowDrawPlay_l* play = getShadowDrawPlay();
  drawShadowList(play->mpModelList, context);
  context = getShadowDrawContext(layer);
  if(s32(context->mIndex)<2 || sLayerShadowDrawExtraCascades) {
   play = getShadowDrawPlay();
   hdShadowDrawList_l* main = play->mpMainList;
   main->mDrawMode = 1;
   drawShadowList(main, context);
   main->mDrawMode = 0;
   context = getShadowDrawContext(layer);
   play = getShadowDrawPlay();
   drawShadowList(play->mpExtraList, context);
   context = getShadowDrawContext(layer);
   play = getShadowDrawPlay();
   hdShadowDrawList_l* opaque = play->mpOpaqueList;
   opaque->mDrawMode = 1;
   drawShadowList(opaque, context);
   opaque->mDrawMode = 0;
   context = getShadowDrawContext(layer);
   play = getShadowDrawPlay();
   drawShadowList(play->mpTranslucentList, context);
   context = getShadowDrawContext(layer);
   play = getShadowDrawPlay();
   drawShadowList(play->mpEffectList, context);
   context = getShadowDrawContext(layer);
   play = getShadowDrawPlay();
   drawShadowList(play->mpLastList, context);
  }
 }
 getShadowDrawContext(layer)->mpState = NULL;
 clearShadowDrawState(&sLayerShadowDebugState.mCleanup);
}
/* 02737F7C */
static void __sinit_hd_layer_shadow_cpp() {
 for (u32 i = 0; i < 4; ++i) sLayerShadowHeaderZero.mWords[i] = 0;
    __register_global_object(gabi::ea(&sLayerShadowHeaderDescriptor));
    sLayerShadowMinRotation = -3.1415927410125732f;
    sLayerShadowMaxRotation = 3.1415927410125732f;
    initializeRotationGuard(&sLayerShadowRotationGuard);
    __register_global_object(gabi::ea(&sLayerShadowRotationDescriptor));
    initializePositiveRotationGuard(&sLayerShadowPositiveRotationGuard);
    __register_global_object(gabi::ea(&sLayerShadowPositiveRotationDescriptor));
}
/* WWHD glue
 data sLayerShadowMultiFilterDrawVtable 101427E0 nocheck
 data sLayerShadowSafeStringVtable 10142828 nocheck
 data sLayerShadowLayerVtable 10142AAC nocheck
 data sLayerShadowDrawVtable 10142B54 nocheck
 data sLayerShadowStepVtable 10169E3C nocheck
 data sLayerShadowBaseDelegateVtable 10142810 nocheck
 data sLayerShadowDelegateVtable 10142880 nocheck
 data sLayerShadowDrawDelegateVtable 10142968 nocheck
 data sLayerShadowStepDelegateVtable 10142978 nocheck
 data sLayerShadowProjectionName 10142958 nocheck
 data sLayerShadowViewportName 10142870 nocheck
 data sLayerShadowLineObjectVtable 10142928 nocheck
 data sLayerShadowLineVtable 101428F8 nocheck
 data sLayerShadowLineCollisionVtable 10142908 nocheck
 data sLayerShadowLineGroupVtable 10142918 nocheck
 data sLayerShadowLineObjectBaseVtable 101428E8 nocheck
 data sLayerShadowLineGroupBaseVtable 101428A8 nocheck
 data sLayerShadowLineCollisionBaseVtable 10142898 nocheck
 data sLayerShadowDefaultView 104A2098
 data sLayerShadowDefaultProjection 104A20FC
 data sLayerShadowProjectionGuard 101FD9F0
 data sLayerShadowProjectionType 101FDCD4
 data sLayerShadowViewportGuard 101FD7BC
 data sLayerShadowViewportType 101FDCF8
 data sLayerShadowTuning 101F8994
 data sLayerShadowDefaultDepth 101F898C
 data sLayerShadowSave 101F84DC
 ext 0201AD78 void shadowVectorAdd(cXyz* first,cXyz* result,cXyz* second)
 ext 0201ADE0 void shadowVectorSubtract(cXyz* first,cXyz* result,cXyz* second)
 ext 0201AE48 void shadowVectorScale(cXyz* first,cXyz* result,f32 scale)
 ext 028E8DD0 f32 shadowVectorSquareMagnitude(cXyz* vector)
 ext 028F4384 f32 shadowSquareRoot(f32 value)
 ext 027382E8 f32 clampShadowRange(f32 value,f32 minimum,f32 maximum)
 ext 025B8B94 BOOL isShadowEvent(void* events,u32 bit)
 ext 02008FEC void constructShadowLineCheck(hdShadowLineCheck_c* line)
 ext 024F1AFC void setShadowLineCheck(hdShadowLineCheck_c* line,cXyz* start,cXyz* end,void* actor)
 ext 02008860 BOOL crossShadowLine(void* background,hdShadowLineCheck_c* line)
 ext 02008B4C void destroyShadowLineCheck(hdShadowLineCheck_c* line,s32 flags)
 ext 028EFFD0 void constructShadowMatrices(hdShadowMatrices_c* matrices,u32 count,u32 size,u32 constructor)
 ext 028E8970 void copyShadowMatrix(void* source,void* destination)
 ext 0274D83C void* getShadowProjectionMatrix(hdShadowPerspective_l* projection)
 ext 0278CB8C void initializeShadowVolume(hdShadowVolume_c* volume,void* matrix,void* frustum,f32 nearPlane,f32 farPlane,f32 fieldOfView,f32 aspect)
 ext 0278D07C void transformShadowVolume(hdShadowVolume_c* volume,hdLayerMatrix_c* view)
 ext 028F423C f32 shadowTangent(f32 value)
 ext 0278FF3C void configureShadowVolume(hdShadowRenderer_l* renderer,u32 index,hdLayerMatrix_c* view,void* projection,hdShadowMatrices_c* matrices,hdShadowStageRange_c* distances,hdShadowVolume_c* volume,f32 nearPlane,f32 farPlane,f32 tangent,f32 aspect)
 ext 027987BC void updateShadowSurface(hdShadowSurface_l* surface,hdLayerMatrix_c* view,void* projection)
 ext 02798978 void prepareShadowSurface(hdShadowSurface_l* surface)
 ext 027989F0 void finalizeShadowSurface(hdShadowSurface_l* surface)
 ext 0255BE0C BOOL isShadowSpecialStage()
 ext 027FB678 void uploadShadowEnvironment(hdShadowEnvironmentParameters_l* environment)
 ext 027E1A0C void resetShadowDepthBias(hdShadowDepthBias_c* bias)
 ext 027E1A14 void setShadowDepthBias(hdShadowDepthBias_c* bias,f32 value)
 ext 027E1A28 void setShadowDepthSlope(hdShadowDepthBias_c* bias,f32 value)
 ext 027E1A08 void applyShadowDepthBias(hdShadowDepthBias_c* bias)
 ext 02798B0C u32 drawShadowSurface(hdShadowSurface_l* surface,u32 slot)
 ext 0274D690 void resetShadowViewport(hdFilterViewport_l* viewport)
 data sLayerShadowRenderContext 101F8B18
 data sLayerShadowDefaultDirection 104A0940
 str 10142804 "" in 02736390
 str 10142A3C
 str 10142A1C
 ext 0274FBF8 void prepareShadowResources(void* context)
 ext 0274FCCC void finishShadowResources(void* context)
 ext 027984FC void initializeShadowSurface(hdShadowSurface_l* surface,hdShadowPassArg_c* pass,void* heap)
 ext 027967A0 void setShadowResolution(void* resolution,hdShadowResolution_c* size)
 ext 0278D2E8 void setShadowDirection(cXyz* destination,cXyz* direction)
 ext 02798AE4 void applyShadowSurface(hdShadowSurface_l* surface)
 ext 027FB5D4 void initializeShadowEnvironment(hdShadowEnvironmentParameters_l* environment,void* heap)
 ext 027FD838 void initializeShadowBuffer(hdShadowBuffer_l* buffer,u32 enabled,void* heap)
 ext 027C2450 void addShadowRenderStep(hdLayerShadow_c* layer,hdShadowStep_c* step)
 ext 0278E730 void initializeShadowRenderer(hdShadowRenderer_l* renderer,u32 enabled,void* heap)
 data sLayerShadowDebugState 104B45C0
 data sLayerShadowDrawExtraCascades 101F89DC
 ext 0274FDBC void constructShadowDrawState(hdShadowDrawState_c* state)
 ext 0274FEB0 void applyShadowDrawState(hdShadowDrawState_c* state)
 ext 027FDA54 void configureShadowBuffer(hdShadowBuffer_l* buffer,u32 index,void* view,void* projection,void* matrix)
 ext 027FDFF4 void bindShadowBuffer(hdShadowBuffer_l* buffer,u32 index)
 ext 027F1088 void drawShadowList(hdShadowDrawList_l* list,hdShadowDrawContext_l* context)
 ext 02750370 void clearShadowDrawState(void* state)
 data sLayerShadowStageName 1047E6B8
 data sLayerShadowDemoFrame 101D600C
 data sLayerShadowStageRange 101F89F0
 data sLayerShadowStageDepth 101F8990
 str 10142A70
 str 10142A78
 str 10142A80
 ext 02738050 void normalizeShadowString(SafeString* name)
 data sLayerShadowDebugRenderer 104B45C0
 ext 027F29D4 hdFilterDebugRenderer_l* setupShadowDebugRenderer(void* renderer)
 ext 027983D4 void releaseShadowSurface(hdShadowSurface_l* surface)
 ext 027983DC void destroyShadowSurface(hdShadowSurface_l* surface,s32 flags)
 ext 0278D8D4 void destroyShadowRenderer(hdShadowRenderer_l* renderer,s32 flags)
 ext 028F0164 void destroyShadowBuffers(hdShadowBuffer_l* buffers,u32 count,u32 size,u32 destructor,u32 extra,u32 flags)
 ext 027C0804 void destroyShadowStep(hdShadowStep_c* step,s32 flags)
 ext 02791230 u32 bindShadowRenderer(hdShadowRenderer_l* renderer,u32 index,hdFilterViewport_l* viewport,void* target,void* target2,void* target3,u32 textureCount,u32 flags,u32 cascadeCount,u32 slot)
 str 101429E4 "" in 02735F0C
 str 101429E8
 ext 0273AD10 void* allocateShadowLayer(u32 size)
 ext 027340D8 void constructShadowBase(hdLayerShadow_c* layer,SafeString* name)
 ext 02797EEC void constructShadowSurface(hdShadowSurface_l* surface)
 ext 02752A84 void constructShadowStep(hdShadowStep_c* step)
 ext 028EFFD0 void constructShadowBuffers(hdShadowBuffer_l* buffers,u32 count,u32 size,u32 constructor)
 ext 0278DAC0 void constructShadowRenderer(hdShadowRenderer_l* renderer)
 data sLayerShadowHeaderZero 1049FAC0
 data sLayerShadowHeaderDescriptor 101F8968
 data sLayerShadowRotationDescriptor 101F8974
 data sLayerShadowPositiveRotationDescriptor 101F8980
 data sLayerShadowMinRotation 1049FAB4
 data sLayerShadowMaxRotation 1049FAB8
 data sLayerShadowRotationGuard 1049FABC
 data sLayerShadowPositiveRotationGuard 1049FABD
 ext 028ED6F8 void initializeRotationGuard(void* guard)
 ext 028EAB2C void initializePositiveRotationGuard(void* guard)
 ext 02555D0C hdShadowLight_l* getShadowLight()
 ext 025200D4 hdShadowPlay_l* getShadowPlay()
 ext 02557614 BOOL useShadowLightDirection(hdShadowLight_l* light)
 ext 028E8D88 void addShadowVectors(cXyz* first,cXyz* second,cXyz* result)
 ext 02563F64 void calculateShadowDirection(cXyz* first,cXyz* second,cXyz* result)
 ext 0279D92C void destroyShadowFilterSurface(hdFilterSurface_c* surface,s32 flags)
 ext 02734248 void destroyShadowLayerBase(void* layer,s32 flags)
 ext 0273AF40 void deleteShadowLayer(void* layer)
 ext 027C23F8 void registerShadowStep(hdLayer3D_c* scene,u32 index,hdShadowStep_c* step)
 ext 027A7558 hdFilterTargetArg_c* getShadowTarget(hdFilterTargetArg_c* argument,void* target)
 ext 0279A308 void setShadowTarget(hdShadowBinding_l* binding,hdFilterTargetArg_c* argument)
 ext 0278EEF8 void finishShadowRenderer(hdShadowRenderer_l* renderer,u32 index)
*/

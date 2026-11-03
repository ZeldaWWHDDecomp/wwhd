// AGL layer method dispatch, reconstructed from the assigned game instructions.
#include "agl/RenderQueue.h"
#include "sead/ViewTypeIdentity.h"
#include "sead/RenderInlineInstances.h"
#include "sead/Random.h"
namespace agl {}
using namespace agl;
void createLayerDispatchContext(RenderLayerDispatchContext_l*,u32,u32,u32,RenderLayerPrefix*);
static RenderCameraMethods_l* layerCameraMethods(const sead::ViewCamera* camera) {
    return static_cast<RenderCameraMethods_l*>(static_cast<void*>(camera->mVtable));
}
static RenderProjectionMethods_l* layerProjectionMethods(const sead::ProjectionState* projection) {
    return static_cast<RenderProjectionMethods_l*>(static_cast<void*>(projection->mVtable));
}
/* 027C25AC */
void agl::RenderLayerPrefix::drawMethods(RenderMethodResource_l* resource,RenderLayerDispatchContext_l* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::call_ptr<void>(methods()->mBeginMethods.mFunction,this,context);
    u32 cursor=gabi::ea(static_cast<gptr<RenderMethodTarget_l>*>(resource->mMethods));
    u32 end=cursor+static_cast<u32>(resource->mMethodCount)*4;
    while(cursor!=end) {
        RenderMethodTarget_l* method=*gabi::at<gptr<RenderMethodTarget_l>>(cursor);
        gabi::call_ptr<void>(methods()->mBeforeMethod.mFunction,this,context,method);
        method=*gabi::at<gptr<RenderMethodTarget_l>>(cursor);
        u32 target=method->mDispatch.mVtable->mExecute;
        gabi::call_ptr<void>(target,&method->mDispatch,context);
        method=*gabi::at<gptr<RenderMethodTarget_l>>(cursor);
        gabi::call_ptr<void>(methods()->mAfterMethod.mFunction,this,context,method);
        cursor+=4;
    }
    gabi::call_ptr<void>(methods()->mEndMethods.mFunction,this,context);
}
/* 027C4538 */
void agl::RenderLayerPrefix::dispatch(u32 a,u32 b,u32 c) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<RenderLayerDispatchContext_l> context(8);
    createLayerDispatchContext(context,a,b,c,this);
    u32 start=gabi::ea(static_cast<RenderMethodResource_l*>(mResources)),cursor=start;
    u32 end=start+static_cast<u32>(mResourceCount)*sizeof(RenderMethodResource_l);
    while(cursor!=end) {
        auto* resource=gabi::at<RenderMethodResource_l>(cursor);
        if(static_cast<u32>(resource->mFlags)&1) {
            u32 index=(cursor-start)/sizeof(RenderMethodResource_l);
            if(gabi::call_ptr<u32>(methods()->mFilter.mFunction,this,index)) {
                context->mIndex=index;drawMethods(resource,context);
            }
        }
        cursor+=sizeof(RenderMethodResource_l);
    }
}
static const sead::ViewTransform sLayerIdentity;
static const sead::ViewVector sLayerDefaultEye;
static const sead::ViewVector sLayerNegativeTarget;
static sead::DisposerVtable sLayerMethods;
static sead::DisposerVtable sLayerSecondaryMethods;
static sead::SafeTextVtable sLayerNameBaseMethods;
static sead::SafeTextVtable sLayerNameIntermediateMethods;
static sead::SafeTextVtable sLayerNameMethods;
static sead::DisposerVtable sLayerCameraBaseMethods;
static sead::DisposerVtable sLayerCameraMethods;
static sead::DisposerVtable sStoredRenderViewMethods;
static StoredRenderView_l sLayerDefaultView;
static sead::ProjectionState sLayerDefaultProjection;
struct RenderGlobalRegistration_l { u8 mStorage[12]; };
static RenderGlobalRegistration_l sLayerViewRegistration;
static RenderGlobalRegistration_l sLayerProjectionRegistration;
void initializeLayerDisposer(sead::Disposer*);
void initializeLayerPerspective(sead::PerspectiveState*);
void initializeLayerStoredProjection(sead::StoredProjection*);
void initializeLayerOrthographic(sead::ProjectionState*);
void* clearCaptureBytes(void*,u32,u32);
void registerLayerGlobal(RenderGlobalRegistration_l*);
template<class T> static T* initializeLayerMember(T* member) {
    if(!gabi::ea(member)) return static_cast<T*>(allocateRegistryStorage(sizeof(T)));
    return member;
}
static void initializeLayerVector(sead::ViewVector* vector,f32 x,f32 y,f32 z) {
    vector=initializeLayerMember(vector);
    if(vector) { vector->y.value=y;vector->x.value=x;vector->z.value=z; }
}
static void copyLayerMatrix(sead::ViewTransform* output,const sead::ViewTransform* input) {
    auto* out=reinterpret_cast<be<u32>*>(output);
    auto* in=reinterpret_cast<const be<u32>*>(input);
    for(u32 i=0;i<12;++i) out[i]=in[i];
}
static void initializeLayerCamera(sead::ViewCamera* camera) {
    camera->mVtable=&sLayerCameraBaseMethods;copyLayerMatrix(&camera->mTransform,&sLayerIdentity);
    camera->mVtable=&sLayerCameraMethods;
    initializeLayerVector(&camera->mEye,0.0f,0.0f,10.0f);
    initializeLayerVector(&camera->mTarget,0.0f,0.0f,0.0f);
    initializeLayerVector(&camera->mUp,0.0f,1.0f,0.0f);
}
static void initializeLayerVector2(sead::RenderVector2* vector,f32 value) {
    vector=initializeLayerMember(vector);
    if(vector) { vector->y=value;vector->x=value; }
}
/* 027C2DF0 */
agl::RenderLayerWorkspace* agl::RenderLayerWorkspace::create(RenderLayerWorkspace* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!storage) storage=static_cast<RenderLayerWorkspace*>(allocateRegistryStorage(sizeof(RenderLayerWorkspace)));
    if(storage) {
        storage->mFlashFrames=0;
        initializeLayerCamera(&storage->mCamera);initializeLayerCamera(&storage->mDrawCamera);
        storage->mDistance=10.0f;
        initializeLayerVector(&storage->mCameraOffset,0.0f,0.0f,0.0f);
        initializeLayerMember(&storage->mPendingCameraOffset);
        storage->mNear=0.1f;storage->mFar=10000.0f;storage->mFieldOfView=45.0f;
        initializeLayerVector2(&storage->mProjectionOffset,0.0f);storage->mActiveProjection=nullptr;
        initializeLayerPerspective(&storage->mPerspective);
        initializeLayerStoredProjection(&storage->mStoredProjection);
        initializeLayerOrthographic(&storage->mOrthographic);
        initializeLayerVector2(&storage->mViewportOrigin,0.0f);initializeLayerVector2(&storage->mViewportExtent,1.0f);
        storage->mRatio=1.0f;
        struct TextArrayFields_l { be<u32> mCount;gptr<void> mStorage; };
        auto* texts=initializeLayerMember(reinterpret_cast<TextArrayFields_l*>(&storage->mTextCount));
        if(texts) { texts->mStorage=nullptr;texts->mCount=0; }
        storage->mUpdateCount=0;storage->mMovementScale=100.0f;
        sead::CriticalSection::construct(&storage->mLock);
        auto* name=initializeLayerMember(&storage->mName);
        if(name) {
            auto* base=initializeLayerMember(static_cast<sead::SafeTextBuffer*>(name));
            if(base) {
                base->mText=name->mStorage;base->mVtable=&sLayerNameBaseMethods;base->mCapacity=256;
                name->mStorage[255]=0;
            }
            name->mVtable=&sLayerNameIntermediateMethods;
            name->mText[0]=0;name->mVtable=&sLayerNameMethods;
        }
        clearCaptureBytes(&storage->mMovement,0,12);
        storage->mCamera.mEye.x.value=sLayerDefaultEye.x.value;
        storage->mCamera.mEye.y.value=sLayerDefaultEye.y.value;
        storage->mCamera.mEye.z.value=sLayerDefaultEye.z.value;
        storage->mCamera.mTarget.x.bits=static_cast<u32>(sLayerNegativeTarget.x.bits)^0x80000000;
        storage->mCamera.mTarget.y.bits=static_cast<u32>(sLayerNegativeTarget.y.bits)^0x80000000;
        storage->mCamera.mTarget.z.bits=static_cast<u32>(sLayerNegativeTarget.z.bits)^0x80000000;
        u32 target=layerCameraMethods(&storage->mCamera)->mBuild.mFunction;
        gabi::call_ptr<void>(target,&storage->mCamera,&storage->mCamera);
        seadCopyViewMatrix(&storage->mCamera.mTransform,&storage->mDrawCamera.mTransform);
        storage->mDrawCamera.mTarget.x.value=storage->mCamera.mTarget.x.value;
        f32 targetY=storage->mCamera.mTarget.y.value,targetZ=storage->mCamera.mTarget.z.value;
        storage->mDrawCamera.mTarget.y.value=targetY;storage->mDrawCamera.mTarget.z.value=targetZ;
        f32 upY=storage->mCamera.mUp.y.value,upZ=storage->mCamera.mUp.z.value,upX=storage->mCamera.mUp.x.value;
        storage->mDrawCamera.mUp.y.value=upY;
        f32 eyeZ=storage->mCamera.mEye.z.value,eyeY=storage->mCamera.mEye.y.value;
        storage->mDrawCamera.mEye.z.value=eyeZ;storage->mDrawCamera.mEye.y.value=eyeY;
        storage->mDrawCamera.mUp.z.value=upZ;
        f32 eyeX=storage->mCamera.mEye.x.value;
        storage->mDrawCamera.mUp.x.value=upX;storage->mDrawCamera.mEye.x.value=eyeX;
    }
    return storage;
}
/* 027C3280 */
agl::RenderLayerPrefix* agl::RenderLayerPrefix::create(RenderLayerPrefix* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!storage) storage=static_cast<RenderLayerPrefix*>(allocateRegistryStorage(sizeof(RenderLayerPrefix)));
    if(storage) {
        initializeLayerDisposer(storage);storage->mVtable=&sLayerMethods;storage->mLayerMethods=&sLayerSecondaryMethods;
        sead::Viewport::createEmpty(&storage->mViewport);sead::Viewport::createEmpty(&storage->mScissor);
        storage->mCamera=nullptr;storage->mProjection=nullptr;
        auto* flags=initializeLayerMember(&storage->mFlags);if(flags) *flags=0x183;
        storage->mView=0;storage->mViewOverride=-1;
        auto* debug=initializeLayerMember(&storage->mDebugClear);if(debug) *debug=0;
        storage->mRenderState=0;storage->mManagerIndex=0;
        storage->mClearColor.x=0.0f;storage->mClearDepth=1.0f;
        storage->mClearColor.z=0.0f;storage->mClearColor.y=0.0f;storage->mClearColor.w=0.0f;
        struct ResourceFields_l { be<u32> mCount;gptr<RenderMethodResource_l> mStorage; };
        auto* resources=initializeLayerMember(reinterpret_cast<ResourceFields_l*>(&storage->mResourceCount));
        if(resources) { resources->mStorage=nullptr;resources->mCount=0; }
        storage->mDebugFlags=0;RenderLayerWorkspace::create(&storage->mWorkspace);
        u32 target=reinterpret_cast<RenderCameraMethods_l*>(static_cast<void*>(sLayerDefaultView.mVtable))->mBuild.mFunction;
        gabi::call_ptr<void>(target,&sLayerDefaultView,&sLayerDefaultView);
    }
    return storage;
}
/* 027C48E0 */
void initializeRenderLayerGlobals() {
    gabi::AutoFrame frame{gabi::current_function()};
    copyLayerMatrix(&sLayerDefaultView.mTransform,&sLayerIdentity);
    sLayerDefaultView.mVtable=&sStoredRenderViewMethods;
    copyLayerMatrix(&sLayerDefaultView.mStored,&sLayerIdentity);
    registerLayerGlobal(&sLayerViewRegistration);
    initializeLayerOrthographic(&sLayerDefaultProjection);
    registerLayerGlobal(&sLayerProjectionRegistration);
}
sead::RegistryHeap* containingVertexInputHeap(sead::RegistryManagerState*,void*);
void destroyLayerOrthographic(sead::ProjectionState*,u32);
void destroyLayerStoredProjection(sead::StoredProjection*,u32);
void destroyLayerPerspective(sead::PerspectiveState*,u32);
void destroyLayerCamera(sead::ViewCamera*,u32);
void finishRenderDisposer(sead::Disposer*,u32);
/* 027C33C0 */
void agl::RenderLayerPrefix::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!gabi::ea(this)) return;
    mVtable=&sLayerMethods;mLayerMethods=&sLayerSecondaryMethods;
    mManager->removeLayer(this);
    if(mResources) {
        s32 index=0;
        while(index<static_cast<s32>(mResourceCount)) {
            auto* resource=gabi::at<RenderMethodResource_l>(gabi::ea(static_cast<RenderMethodResource_l*>(mResources))+static_cast<u32>(index)*sizeof(RenderMethodResource_l));
            u32 target=resource->mVtable->mDestroy.mFunction;
            gabi::call_ptr<void>(target,resource,2);++index;
        }
        sead::RegistryHeap* heap=containingVertexInputHeap(sRegistryManager,mResources);
        u32 target=heap->dispatch()->mRelease.mFunction;
        gabi::call_ptr<void>(target,heap,static_cast<void*>(mResources));mResourceCount=0;mResources=nullptr;
    }
    if(mWorkspace.mTexts) {
        s32 index=0;
        while(index<static_cast<s32>(mWorkspace.mTextCount)) {
            auto* text=gabi::at<sead::FixedSafeText<512>>(gabi::ea(static_cast<sead::FixedSafeText<512>*>(mWorkspace.mTexts))+static_cast<u32>(index)*sizeof(sead::FixedSafeText<512>));
            gabi::call_ptr<void>(text->mVtable->mDestroy.mFunction,text,2);++index;
        }
        sead::RegistryHeap* heap=containingVertexInputHeap(sRegistryManager,mWorkspace.mTexts);
        u32 target=heap->dispatch()->mRelease.mFunction;
        gabi::call_ptr<void>(target,heap,static_cast<void*>(mWorkspace.mTexts));mWorkspace.mTextCount=0;mWorkspace.mTexts=nullptr;
    }
    mWorkspace.mLock.destroy(2);
    destroyLayerOrthographic(&mWorkspace.mOrthographic,2);
    destroyLayerStoredProjection(&mWorkspace.mStoredProjection,2);
    destroyLayerPerspective(&mWorkspace.mPerspective,2);
    destroyLayerCamera(&mWorkspace.mDrawCamera,2);destroyLayerCamera(&mWorkspace.mCamera,2);
    finishRenderDisposer(this,0);if(flags&1) releaseRegistryStorage(this);
}
static sead::DisposerVtable sLayerLookAtTypeMethods;
void normalizeLayerUp(sead::ViewVector*);
static f32 negateLayerFloat(f32 value) { return __builtin_bit_cast(f32,__builtin_bit_cast(u32,value)^0x80000000u); }
static void updateLayerLookAtType() {
    if(!sLookAtGuard) { sLookAtGuard=1;sLookAtTag.mTable=&sLayerLookAtTypeMethods; }
}
static bool layerCameraMatches(sead::ViewCamera* camera,sead::RuntimeType* type) {
    if(!camera) return false;
    u32 target=layerCameraMethods(camera)->mMatchesType.mFunction;
    return gabi::call_ptr<u32>(target,camera,type)!=0;
}
/* 027C45E8 */
u32 agl::RenderLayerPrefix::setCameraMode(u32 enabled) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!enabled || !mCamera) { mFlags=static_cast<u32>(mFlags)&~0x40u;return 0; }
    gabi::FrameLocal<sead::ViewVector> eye(0x20),up(0x44),target(8);
    mCamera->mTransform.inverseTranslation(eye);
    mCamera->mTransform.copyUp(up);
    sead::ViewCamera* camera=mCamera;
    updateLayerLookAtType();
    if(layerCameraMatches(camera,&sLookAtTag)) {
        target->x.value=camera->mTarget.x.value;target->y.value=camera->mTarget.y.value;target->z.value=camera->mTarget.z.value;
    } else {
        mCamera->mTransform.copyForward(target);
        f32 distance=mWorkspace.mDistance;
        f32 x=gabi::fmuls_ppc(negateLayerFloat(target->x.value),distance);
        f32 y=gabi::fmuls_ppc(negateLayerFloat(target->y.value),distance);
        f32 z=gabi::fmuls_ppc(negateLayerFloat(target->z.value),distance);
        target->x.value=gabi::fadds_ppc(x,eye->x.value);
        target->y.value=gabi::fadds_ppc(y,eye->y.value);
        target->z.value=gabi::fadds_ppc(z,eye->z.value);
    }
    mFlags=static_cast<u32>(mFlags)|0x8000u;
    mWorkspace.mCameraOffset.x.value=sLayerDefaultEye.x.value;
    mWorkspace.mCameraOffset.y.value=sLayerDefaultEye.y.value;
    mWorkspace.mCameraOffset.z.value=sLayerDefaultEye.z.value;
    f32 targetZ=target->z.value,targetX=target->x.value;
    mWorkspace.mCameraOffset.z.value=targetZ;mWorkspace.mCameraOffset.x.value=targetX;
    f32 targetY=target->y.value;
    mWorkspace.mDrawCamera.mTarget.z.value=targetZ;mWorkspace.mCameraOffset.y.value=targetY;
    f32 eyeZ=eye->z.value,eyeY=eye->y.value;
    targetX=target->x.value;f32 eyeX=eye->x.value;
    mWorkspace.mDrawCamera.mTarget.x.value=targetX;
    f32 upX=up->x.value;
    mWorkspace.mDrawCamera.mEye.x.value=eyeX;mWorkspace.mDrawCamera.mEye.y.value=eyeY;
    f32 upZ=up->z.value;targetY=target->y.value;
    mWorkspace.mDrawCamera.mUp.x.value=upX;mWorkspace.mDrawCamera.mTarget.y.value=targetY;
    f32 upY=up->y.value;
    mWorkspace.mDrawCamera.mUp.z.value=upZ;mWorkspace.mDrawCamera.mEye.z.value=eyeZ;mWorkspace.mDrawCamera.mUp.y.value=upY;
    normalizeLayerUp(&mWorkspace.mDrawCamera.mUp);
    u32 build=layerCameraMethods(&mWorkspace.mDrawCamera)->mBuild.mFunction;
    gabi::call_ptr<void>(build,&mWorkspace.mDrawCamera,&mWorkspace.mDrawCamera);
    seadCopyViewMatrix(&mWorkspace.mDrawCamera.mTransform,&mWorkspace.mCamera.mTransform);
    f32 drawTargetY=mWorkspace.mDrawCamera.mTarget.y.value,drawEyeX=mWorkspace.mDrawCamera.mEye.x.value;
    mWorkspace.mCamera.mTarget.y.value=drawTargetY;mWorkspace.mCamera.mEye.x.value=drawEyeX;
    f32 drawUpY=mWorkspace.mDrawCamera.mUp.y.value,drawTargetZ=mWorkspace.mDrawCamera.mTarget.z.value;
    mWorkspace.mCamera.mUp.y.value=drawUpY;mWorkspace.mCamera.mTarget.z.value=drawTargetZ;
    f32 drawTargetX=mWorkspace.mDrawCamera.mTarget.x.value;
    u32 flags=mFlags;f32 drawEyeY=mWorkspace.mDrawCamera.mEye.y.value;
    mWorkspace.mCamera.mTarget.x.value=drawTargetX;mFlags=flags|0x40;
    f32 drawEyeZ=mWorkspace.mDrawCamera.mEye.z.value,drawUpX=mWorkspace.mDrawCamera.mUp.x.value;
    mWorkspace.mCamera.mEye.z.value=drawEyeZ;mWorkspace.mCamera.mUp.x.value=drawUpX;
    f32 drawUpZ=mWorkspace.mDrawCamera.mUp.z.value;
    mWorkspace.mCamera.mEye.y.value=drawEyeY;mWorkspace.mCamera.mUp.z.value=drawUpZ;
    return 1;
}
static sead::SafeTextVtable sLayerPlainTextMethods;
static sead::SafeTextVtable sLayerResourceTextIntermediateMethods;
static sead::SafeTextVtable sLayerResourceTextMethods;
typedef be<u8> LayerLabelByte;
static const LayerLabelByte* sLayerDefaultName;
static const LayerLabelByte sLayerPrepareLabelFirst;
void constructLayerResource(RenderMethodResource_l*);
void prepareLayerNameText(sead::SafeText*);
void* copyCaptureBytes(void*,const void*,u32,u32);
sead::RegistryHeap* currentVertexInputHeap(sead::RegistryManagerState*);
static s32 layerTextLength(const be<u8>* text) {
    s32 length=0;
    while(text[length]) { if(++length>0x40000) return 0; }
    return length;
}
static void assureLayerText(sead::SafeText* text) {
    gabi::call_ptr<void>(text->mVtable->mAssureTermination.mFunction,text);
}
/* 027C356C */
void agl::RenderLayerPrefix::prepareResources(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 count=gabi::call_ptr<s32>(methods()->mResourceCount.mFunction,this);
    if(count>0) {
        sead::RegistryHeap* allocator=heap ? heap:currentVertexInputHeap(sRegistryManager);
        u32 allocate=allocator->dispatch()->mAllocate.mFunction;
        void* storage=gabi::call_ptr<void*>(allocate,allocator,static_cast<u32>(count)*sizeof(RenderMethodResource_l),4);
        for(s32 i=0;i<count;++i) {
            auto* resource=gabi::at<RenderMethodResource_l>(gabi::ea(storage)+static_cast<u32>(i)*sizeof(RenderMethodResource_l));
            if(resource) constructLayerResource(resource);
        }
        if(storage) { mResources=static_cast<RenderMethodResource_l*>(storage);mResourceCount=count; }
    }
    sead::RegistryHeap* allocator=heap ? heap:currentVertexInputHeap(sRegistryManager);
    u32 allocate=allocator->dispatch()->mAllocate.mFunction;
    void* storage=gabi::call_ptr<void*>(allocate,allocator,2*sizeof(sead::FixedSafeText<512>),4);
    for(u32 i=0;i<2;++i) {
        auto* text=gabi::at<sead::FixedSafeText<512>>(gabi::ea(storage)+i*sizeof(sead::FixedSafeText<512>));
        if(text) {
            auto* base=initializeLayerMember(static_cast<sead::SafeTextBuffer*>(text));
            if(base) {
                base->mCapacity=512;base->mText=text->mStorage;base->mVtable=&sLayerNameBaseMethods;
                text->mStorage[511]=0;
            }
            text->mVtable=&sLayerResourceTextIntermediateMethods;text->mText[0]=0;text->mVtable=&sLayerResourceTextMethods;
        }
    }
    u32 countNow=mResourceCount;
    if(storage) { mWorkspace.mTexts=static_cast<sead::FixedSafeText<512>*>(storage);mWorkspace.mTextCount=2; }
    u32 start=gabi::ea(static_cast<RenderMethodResource_l*>(mResources)),cursor=start;
    u32 end=start+countNow*sizeof(RenderMethodResource_l);
    while(cursor!=end) {
        gabi::FrameLocal<sead::SafeText> name(0x10);
        gabi::call_ptr<void>(methods()->mResourceName.mFunction,this,static_cast<sead::SafeText*>(name),(cursor-start)/sizeof(RenderMethodResource_l));
        cursor+=sizeof(RenderMethodResource_l);
    }
    updateDebugText(3);
    gabi::FrameLocal<sead::SafeText> label(8);
    label->mVtable=&sLayerPlainTextMethods;label->mText=const_cast<LayerLabelByte*>(&sLayerPrepareLabelFirst);
    gabi::call_ptr<void>(methods()->mPrepare.mFunction,this,heap);
    label->mVtable=&sLayerPlainTextMethods;label->mText=static_cast<LayerLabelByte*>(sLayerDefaultName);
    be<u8>* destination=mWorkspace.mName.mText;
    prepareLayerNameText(label);
    s32 length=layerTextLength(label->mText);
    s32 capacity=mWorkspace.mName.mCapacity;
    u32 assure=label->mVtable->mAssureTermination.mFunction;
    if(length>=capacity) length=capacity-1;
    gabi::call_ptr<void>(assure,static_cast<sead::SafeText*>(label));
    copyCaptureBytes(destination,static_cast<be<u8>*>(label->mText),length,0);
    *gabi::at<be<u8>>(gabi::ea(destination)+static_cast<u32>(length))=0;
}
static sead::DisposerVtable sLayerProjectionTypeMethods;
static const LayerLabelByte sLayerCameraFormatFirst;
static const LayerLabelByte sLayerPerspectiveLabelFirst;
static const LayerLabelByte sLayerStoredLabelFirst;
static const LayerLabelByte sLayerFrustumLabelFirst;
static const LayerLabelByte sLayerOrthographicLabelFirst;
static const LayerLabelByte sLayerUnknownProjectionLabelFirst;
static const LayerLabelByte sLayerTextSeparatorFirst;
static const LayerLabelByte sLayerProjectionFormatFirst;
void formatLayerText(sead::SafeTextBuffer*,const LayerLabelByte*);
void formatLayerCameraText(sead::SafeTextBuffer*,const LayerLabelByte*,f64,f64,f64,f64,f64,f64,f64,f64);
void queryLayerProjectionParameters(sead::ProjectionState*,be<f32>*,be<f32>*,be<f32>*,be<f32>*,sead::RenderVector2*);
sead::SafeText* formatLayerProjectionText(sead::FixedSafeText<1024>*,const LayerLabelByte*,f64,f64,f64,f64,f64,f64);
static bool layerProjectionMatches(sead::ProjectionState* projection,sead::RuntimeType* type) {
    if(!projection) return false;
    u32 target=layerProjectionMethods(projection)->mMatchesType.mFunction;
    return gabi::call_ptr<u32>(target,projection,type)!=0;
}
static void appendLayerText(sead::SafeTextBuffer* destination,sead::SafeText* source) {
    be<u8>* destinationBytes=destination->mText;
    assureLayerText(destination);
    s32 length=layerTextLength(destination->mText);
    s32 position=length<0 ? 0:length;
    assureLayerText(source);
    s32 append=layerTextLength(source->mText);
    s32 available=static_cast<s32>(destination->mCapacity)-position;
    if(append>=available) append=available-1;
    if(append>0) {
        be<u8>* output=gabi::at<be<u8>>(gabi::ea(destinationBytes)+static_cast<u32>(position));
        assureLayerText(source);
        copyCaptureBytes(output,static_cast<be<u8>*>(source->mText),append,0);
        s32 end=static_cast<s32>(static_cast<u32>(position)+static_cast<u32>(append));
        if(end>length) *gabi::at<be<u8>>(gabi::ea(destinationBytes)+static_cast<u32>(end))=0;
    }
}
static sead::FixedSafeText<512>* projectionLayerText(RenderLayerPrefix* layer) {
    u32 offset=static_cast<u32>(layer->mWorkspace.mTextCount)>1 ? sizeof(sead::FixedSafeText<512>):0;
    return gabi::at<sead::FixedSafeText<512>>(gabi::ea(static_cast<sead::FixedSafeText<512>*>(layer->mWorkspace.mTexts))+offset);
}
/* 027C2698 */
void agl::RenderLayerPrefix::updateDebugText(u32 options) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(options&1) {
        sead::ViewCamera* camera=mCamera;
        if(!camera) camera=reinterpret_cast<sead::ViewCamera*>(&sLayerDefaultView);
        if(static_cast<u32>(mFlags)&0x400) camera=&mWorkspace.mCamera;
        gabi::FrameLocal<sead::ViewVector> eye(0x30),up(0x6C),target(0x10);
        camera->mTransform.inverseTranslation(eye);camera->mTransform.copyUp(up);
        updateLayerLookAtType();
        if(layerCameraMatches(camera,&sLookAtTag)) {
            target->x.value=camera->mTarget.x.value;target->y.value=camera->mTarget.y.value;target->z.value=camera->mTarget.z.value;
        } else {
            camera->mTransform.copyForward(target);
            f32 distance=mWorkspace.mDistance;
            f32 x=gabi::fmuls_ppc(negateLayerFloat(target->x.value),distance);
            f32 y=gabi::fmuls_ppc(negateLayerFloat(target->y.value),distance);
            f32 z=gabi::fmuls_ppc(negateLayerFloat(target->z.value),distance);
            target->x.value=gabi::fadds_ppc(x,eye->x.value);
            target->y.value=gabi::fadds_ppc(y,eye->y.value);
            target->z.value=gabi::fadds_ppc(z,eye->z.value);
        }
        // The ninth promoted floating variadic argument is in the EABI stack area, stored as its lfs image (lfs/stfd keep signalling NaNs).
        gabi::FrameLocal<be<f64>> lastComponent(8);*lastComponent=gabi::fpr_from_f32(up->z.value);
        gabi::cpu->cr[6]=1;
        formatLayerCameraText(mWorkspace.mTexts,&sLayerCameraFormatFirst,
            eye->x.value,eye->y.value,eye->z.value,target->x.value,target->y.value,target->z.value,up->x.value,up->y.value);
    }
    if(!(options&2)) return;
    sead::ProjectionState* projection=mProjection;
    if(!projection) projection=&sLayerDefaultProjection;
    if(static_cast<u32>(mFlags)&0x1000) projection=mWorkspace.mActiveProjection;
    const LayerLabelByte* label=&sLayerUnknownProjectionLabelFirst;
    if(!sStreamGuard) { sStreamGuard=1;sStreamType.mTable=&sLayerProjectionTypeMethods; }
    if(layerProjectionMatches(projection,&sStreamType)) label=&sLayerPerspectiveLabelFirst;
    else {
        if(!sInputGuard) { sInputGuard=1;sInputType.mTable=&sLayerProjectionTypeMethods; }
        if(layerProjectionMatches(projection,&sInputType)) label=&sLayerStoredLabelFirst;
        else {
            if(!sStoredProjectionGuard) { sStoredProjectionTag.mTable=&sLayerProjectionTypeMethods;sStoredProjectionGuard=1; }
            if(layerProjectionMatches(projection,&sStoredProjectionTag)) label=&sLayerFrustumLabelFirst;
            else {
                if(!sOutputGuard) { sOutputType.mTable=&sLayerProjectionTypeMethods;sOutputGuard=1; }
                if(layerProjectionMatches(projection,&sOutputType)) label=&sLayerOrthographicLabelFirst;
            }
        }
    }
    gabi::cpu->cr[6]=0;formatLayerText(projectionLayerText(this),label);
    sead::FixedSafeText<512>* destination=projectionLayerText(this);
    gabi::FrameLocal<sead::SafeText> separator(0x1C);
    separator->mVtable=&sLayerPlainTextMethods;separator->mText=const_cast<LayerLabelByte*>(&sLayerTextSeparatorFirst);
    appendLayerText(destination,separator);
    gabi::FrameLocal<be<f32>> first(0x3C),second(0x40),third(0x44),angle(0x48);
    gabi::FrameLocal<sead::RenderVector2> offsets(0x64);
    queryLayerProjectionParameters(projection,first,second,third,angle,offsets);
    destination=projectionLayerText(this);
    gabi::FrameLocal<sead::FixedSafeText<1024>> temporary(0x9C);
    f32 degrees=gabi::fmuls_ppc(*angle,57.2957763671875f);
    gabi::cpu->cr[6]=1;
    sead::SafeText* formatted=formatLayerProjectionText(temporary,&sLayerProjectionFormatFirst,*first,*second,*third,degrees,offsets->x,offsets->y);
    appendLayerText(destination,formatted);
}
typedef be<s32> LayerDebugPasses;
static LayerDebugPasses sLayerDebugPasses;
static LayerLabelByte sLayerDebugUseQuad;
void createLayerQuadTransform(sead::ViewTransform*,const sead::ViewVector*,const sead::ViewVector*);
sead::ProjectionState* createLayerQuadProjection(sead::ProjectionState*,const sead::ViewportBounds*,f32,f32);
void drawLayerClearQuad(const sead::RenderQuaternion*,const sead::ViewTransform*,const sead::ProjectionMatrix*,u32);
/* 027C424C */
void agl::RenderLayerPrefix::render(u32 phase,sead::RenderSurface* surface,u32 option,u32 filter) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<RenderLayerDispatchContext_l> context(0x94);
    createLayerDispatchContext(context,phase,gabi::ea(surface),option,this);
    if(surface) context->mViewport->apply(surface);
    gabi::call_ptr<void>(methods()->mBeginRender.mFunction,this,static_cast<RenderLayerDispatchContext_l*>(context));
    u32 debug=mDebugClear;
    if(debug && surface) {
        s32 pass=0;
        while(pass<static_cast<s32>(sLayerDebugPasses)) {
            if(sLayerDebugUseQuad) {
                sead::Viewport* viewport=context->mViewport;
                u32 depth=(debug>>2)&1,color=debug&1;
                gabi::FrameLocal<sead::CompactRenderState> state(0x20);
                sead::CompactRenderState::create(state);
                state->mWriteRed=color;state->mWriteGreen=color;state->mWriteAlpha=(static_cast<u32>(mDebugClear)>>1)&1;
                state->mDepthComparison=7;state->mWriteBlue=color;state->mDepthWrite=depth;
                state->mBlendEnable=0;state->mDepthTest=depth;state->apply();
                f32 bottom=viewport->mBottom,top=viewport->mTop,right=viewport->mRight,left=viewport->mLeft;
                gabi::FrameLocal<sead::ViewVector> scale(8),position(0x14);
                scale->y.value=gabi::fsubs_ppc(top,bottom);
                position->x.value=0.0f;scale->x.value=gabi::fsubs_ppc(right,left);
                position->y.value=0.0f;scale->z.value=1.0f;position->z.value=negateLayerFloat(mClearDepth);
                gabi::FrameLocal<sead::ViewTransform> transform(0xB8);
                createLayerQuadTransform(transform,scale,position);
                gabi::FrameLocal<sead::ProjectionState> projection(0xE8);
                createLayerQuadProjection(projection,reinterpret_cast<const sead::ViewportBounds*>(viewport),0.0f,1.0f);
                sead::ProjectionMatrix* matrix=projection->deviceProjection();
                drawLayerClearQuad(&mClearColor,transform,matrix,3);
                state->mWriteBlue=1;state->mWriteAlpha=1;state->mWriteRed=1;state->mWriteGreen=1;state->apply();
                destroyLayerOrthographic(projection,2);
            } else {
                u32 clearFlags=(debug&1)|((debug&4) ? 2:0);
                auto* output=reinterpret_cast<RenderOutputPrefix_l*>(surface);
                u32 target=output->mVtable->mClear.mFunction;
                gabi::call_ptr<void>(target,surface,clearFlags,&mClearColor,0,static_cast<f32>(mClearDepth));
            }
            ++pass;
            if(pass<static_cast<s32>(sLayerDebugPasses)) debug=mDebugClear;
        }
    }
    u32 start=gabi::ea(static_cast<RenderMethodResource_l*>(mResources)),cursor=start;
    u32 end=start+static_cast<u32>(mResourceCount)*sizeof(RenderMethodResource_l);
    while(cursor!=end) {
        auto* resource=gabi::at<RenderMethodResource_l>(cursor);
        if(static_cast<u32>(resource->mFlags)&1) {
            u32 index=(cursor-start)/sizeof(RenderMethodResource_l);
            bool draw=!filter;
            if(filter) draw=!gabi::call_ptr<u32>(methods()->mFilter.mFunction,this,index);
            if(draw) { context->mIndex=index;drawMethods(resource,context); }
        }
        cursor+=sizeof(RenderMethodResource_l);
    }
    context->mIndex=0xFFFFFFFF;
    gabi::call_ptr<void>(methods()->mEndRender.mFunction,this,static_cast<RenderLayerDispatchContext_l*>(context));
}
static const LayerLabelByte sLayerNotificationFormatFirst;
typedef be<f32> LayerScissorFactor;
static const LayerScissorFactor sLayerScissorFactorFirst;
static sead::GlobalRandom* sGlobalRandom;
sead::SafeText* formatLayerNotification(sead::FixedSafeText<1024>*,const LayerLabelByte*,const be<u8>*,u32);
void notifyLayerName(void*,sead::SafeText*);
void advanceLayerCamera(sead::ViewCamera*,u32);
void moveLayerCamera(sead::ViewCamera*,sead::ViewVector*,sead::Viewport*);
void copyLayerProjectionMatrix(const sead::ProjectionMatrix*,sead::ProjectionMatrix*);
void updateLayerResource(RenderMethodResource_l*);
static void buildLayerCamera(sead::ViewCamera* camera) {
    u32 target=layerCameraMethods(camera)->mBuild.mFunction;
    gabi::call_ptr<void>(target,camera,camera);
}
static void copyLayerProjectionPrefix(sead::ProjectionState* output,const sead::ProjectionState* input) {
    output->mProjectionDirty=input->mProjectionDirty;output->mDeviceChanged=input->mDeviceChanged;
    copyLayerProjectionMatrix(&input->mProjection,&output->mProjection);
    copyLayerProjectionMatrix(&input->mDeviceProjection,&output->mDeviceProjection);
    output->mDeviceMode=input->mDeviceMode;output->mDepthScale.value=input->mDepthScale.value;
    output->mDepthOffset.value=input->mDepthOffset.value;output->mNear.value=input->mNear.value;
    output->mFar.value=input->mFar.value;output->mTop.value=input->mTop.value;
    output->mBottom.value=input->mBottom.value;output->mLeft.value=input->mLeft.value;output->mRight.value=input->mRight.value;
}
/* 027C3804 */
void agl::RenderLayerPrefix::update(u32 control,const sead::RenderVector2* scale,const sead::RenderVector2* offset) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 flash=mWorkspace.mFlashFrames;
    if(flash) {
        s32 remainder=flash-(flash/20)*20;
        u32 flags=mFlags;
        if(remainder>=10) flags|=0x10;else flags&=~0x10u;
        mFlags=flags;flash=static_cast<s32>(static_cast<u32>(flash)-1);mWorkspace.mFlashFrames=flash;
        if(!flash) mFlags=static_cast<u32>(mFlags)&~0x10u;
    }
    u32 flags=mFlags;
    if(flags&0x20) {
        assureLayerText(&mWorkspace.mName);
        gabi::FrameLocal<sead::FixedSafeText<1024>> temporary(0x98);
        gabi::cpu->cr[6]=0;
        sead::SafeText* text=formatLayerNotification(temporary,&sLayerNotificationFormatFirst,mWorkspace.mName.mText,0xFF-static_cast<u32>(mManagerIndex));
        notifyLayerName(&mLayerMethods,text);mFlags=static_cast<u32>(mFlags)&~0x20u;flags=mFlags;
    }
    if((flags&0xC0)==0xC0 && control) {
        advanceLayerCamera(&mWorkspace.mDrawCamera,control);
        moveLayerCamera(&mWorkspace.mDrawCamera,&mWorkspace.mMovement,&mViewport);flags=mFlags;
        if(flags&0x8000) {
            f32 x=gabi::fsubs_ppc(mWorkspace.mDrawCamera.mEye.x.value,mWorkspace.mDrawCamera.mTarget.x.value);
            f32 y=gabi::fsubs_ppc(mWorkspace.mDrawCamera.mEye.y.value,mWorkspace.mDrawCamera.mTarget.y.value);
            f32 z=gabi::fsubs_ppc(mWorkspace.mDrawCamera.mEye.z.value,mWorkspace.mDrawCamera.mTarget.z.value);
            mWorkspace.mDrawCamera.mEye.y.value=y;mWorkspace.mDrawCamera.mEye.z.value=z;mWorkspace.mDrawCamera.mEye.x.value=x;
            mWorkspace.mDrawCamera.mTarget.x.value=sLayerDefaultEye.x.value;
            mWorkspace.mDrawCamera.mTarget.y.value=sLayerDefaultEye.y.value;
            mWorkspace.mDrawCamera.mTarget.z.value=sLayerDefaultEye.z.value;flags=mFlags;
        }
        if(flags&0x20000) {
            sead::GlobalRandom* random=sGlobalRandom;
            f32 x=gabi::fmuls_ppc(static_cast<f32>(random->getU32()),0x1p-32f);
            f32 y=gabi::fmuls_ppc(static_cast<f32>(random->getU32()),0x1p-32f);
            u32 randomZ=random->getU32();f32 movement=mWorkspace.mMovementScale;
            f32 eyeX=gabi::fmuls_ppc(movement,x),eyeY=gabi::fmuls_ppc(movement,y);
            f32 z=gabi::fmuls_ppc(static_cast<f32>(randomZ),0x1p-32f);
            mWorkspace.mDrawCamera.mEye.x.value=eyeX;
            mWorkspace.mDrawCamera.mEye.y.value=eyeY;mWorkspace.mDrawCamera.mEye.z.value=gabi::fmuls_ppc(movement,z);
            f32 directionX=gabi::fmsubs(static_cast<f32>(random->getU32()),0x1p-32f,0.5f);
            f32 directionY=gabi::fmsubs(static_cast<f32>(random->getU32()),0x1p-32f,0.5f);
            f32 directionZ=gabi::fmsubs(static_cast<f32>(random->getU32()),0x1p-32f,0.5f);
            gabi::FrameLocal<sead::ViewVector> direction(0x28);
            direction->y.value=directionY;direction->x.value=directionX;direction->z.value=directionZ;normalizeLayerUp(direction);
            f32 targetZ=gabi::fadds_ppc(mWorkspace.mDrawCamera.mEye.z.value,direction->z.value);
            f32 targetX=gabi::fadds_ppc(mWorkspace.mDrawCamera.mEye.x.value,direction->x.value);
            f32 targetY=gabi::fadds_ppc(mWorkspace.mDrawCamera.mEye.y.value,direction->y.value);
            mWorkspace.mDrawCamera.mTarget.y.value=targetY;mWorkspace.mDrawCamera.mTarget.z.value=targetZ;mWorkspace.mDrawCamera.mTarget.x.value=targetX;
        }
        buildLayerCamera(&mWorkspace.mDrawCamera);
        f32 targetX=gabi::fadds_ppc(mWorkspace.mDrawCamera.mTarget.x.value,mWorkspace.mCameraOffset.x.value);
        f32 targetY=gabi::fadds_ppc(mWorkspace.mDrawCamera.mTarget.y.value,mWorkspace.mCameraOffset.y.value);
        f32 targetZ=gabi::fadds_ppc(mWorkspace.mDrawCamera.mTarget.z.value,mWorkspace.mCameraOffset.z.value);
        f32 eyeX=gabi::fadds_ppc(mWorkspace.mDrawCamera.mEye.x.value,mWorkspace.mCameraOffset.x.value);
        f32 eyeY=gabi::fadds_ppc(mWorkspace.mDrawCamera.mEye.y.value,mWorkspace.mCameraOffset.y.value);
        f32 eyeZ=gabi::fadds_ppc(mWorkspace.mDrawCamera.mEye.z.value,mWorkspace.mCameraOffset.z.value);
        mWorkspace.mCamera.mTarget.x.value=targetX;mWorkspace.mCamera.mTarget.y.value=targetY;mWorkspace.mCamera.mTarget.z.value=targetZ;
        mWorkspace.mCamera.mEye.y.value=eyeY;mWorkspace.mCamera.mEye.z.value=eyeZ;
        mWorkspace.mCamera.mUp.y.value=mWorkspace.mDrawCamera.mUp.y.value;
        mWorkspace.mCamera.mEye.x.value=eyeX;mWorkspace.mCamera.mUp.x.value=mWorkspace.mDrawCamera.mUp.x.value;
        mWorkspace.mCamera.mUp.z.value=mWorkspace.mDrawCamera.mUp.z.value;
        normalizeLayerUp(&mWorkspace.mCamera.mUp);buildLayerCamera(&mWorkspace.mCamera);
    }
    mFlags=static_cast<u32>(mFlags)&~0x8000u;flags=mFlags;
    if(mProjection && (flags&0x1000)) {
        sead::ProjectionState* projection=mProjection;
        u32 kindTarget=layerProjectionMethods(projection)->mKind.mFunction;
        u32 kind=gabi::call_ptr<u32>(kindTarget,projection);
        projection=mProjection;
        if(kind==1) {
            if(!sStoredProjectionGuard) { sStoredProjectionTag.mTable=&sLayerProjectionTypeMethods;sStoredProjectionGuard=1; }
            if(!layerProjectionMatches(projection,&sStoredProjectionTag)) projection=nullptr;
            copyLayerProjectionPrefix(&mWorkspace.mOrthographic,projection);
            f32 nearDistance=mWorkspace.mNear,farDistance=mWorkspace.mFar;
            mWorkspace.mOrthographic.mProjectionDirty=1;
            mWorkspace.mOrthographic.mNear.value=nearDistance;mWorkspace.mOrthographic.mFar.value=farDistance;
            mWorkspace.mActiveProjection=&mWorkspace.mOrthographic;
        } else {
            if(!sStreamGuard) { sStreamGuard=1;sStreamType.mTable=&sLayerProjectionTypeMethods; }
            if(layerProjectionMatches(projection,&sStreamType)) {
                projection=mProjection;
                if(!sStreamGuard) { sStreamType.mTable=&sLayerProjectionTypeMethods;sStreamGuard=1; }
                if(!layerProjectionMatches(projection,&sStreamType)) projection=nullptr;
                auto* input=reinterpret_cast<sead::PerspectiveState*>(projection);
                auto* output=&mWorkspace.mPerspective;
                auto* base=reinterpret_cast<sead::ProjectionState*>(output);
                copyLayerProjectionPrefix(base,projection);
                output->mAspect.value=input->mAspect.value;output->mShiftX.value=input->mShiftX.value;output->mShiftY.value=input->mShiftY.value;
                f32 fieldOfView=mWorkspace.mFieldOfView,nearDistance=mWorkspace.mNear,farDistance=mWorkspace.mFar;
                base->mProjectionDirty=1;output->mNear.value=nearDistance;output->mFar.value=farDistance;
                output->setAngle(gabi::fmuls_ppc(fieldOfView,0.01745329238474369f));
                base->mProjectionDirty=1;
                f32 shiftY=mWorkspace.mProjectionOffset.y,shiftX=mWorkspace.mProjectionOffset.x;
                flags=mFlags;output->mShiftY.value=shiftY;output->mShiftX.value=shiftX;
                if(flags&0x2000) {
                    f32 nearDistance2=output->mNear.value;
                    reinterpret_cast<sead::ProjectionState*>(input)->mProjectionDirty=1;input->mNear.value=nearDistance2;
                    input->mFar.value=output->mFar.value;input->setAngle(output->mAngle.value);
                    input->mShiftX.value=output->mShiftX.value;
                    f32 shift=output->mShiftY.value;
                    reinterpret_cast<sead::ProjectionState*>(input)->mProjectionDirty=1;input->mShiftY.value=shift;
                }
                mWorkspace.mActiveProjection=base;
            } else {
                sead::ProjectionMatrix* matrix=mProjection->projection();
                mWorkspace.mStoredProjection.setMatrix(matrix,0);
                mWorkspace.mActiveProjection=&mWorkspace.mStoredProjection.mBase;
            }
        }
        flags=mFlags;
    }
    u32 updates=mWorkspace.mUpdateCount;
    if(flags&8) {
        updateDebugText(3);mFlags=static_cast<u32>(mFlags)&~8u;updates=mWorkspace.mUpdateCount;
    } else {
        if((flags&0x200) && !(updates&3)) { updateDebugText(1);flags=mFlags;updates=mWorkspace.mUpdateCount; }
        if((flags&0x800) && (updates&3)==2) { updateDebugText(2);updates=mWorkspace.mUpdateCount; }
    }
    flags=mFlags;mWorkspace.mUpdateCount=updates+1;
    if(flags&0x10000) {
        f32 width=mWorkspace.mViewportExtent.x,height=mWorkspace.mViewportExtent.y;
        f32 left=mWorkspace.mViewportOrigin.x,bottom=mWorkspace.mViewportOrigin.y;
        f32 right=gabi::fadds_ppc(left,width),top=gabi::fadds_ppc(bottom,height);
        mViewport.mLeft=left;mViewport.mBottom=bottom;mViewport.mRight=right;mViewport.mTop=top;
    } else {
        mWorkspace.mViewportOrigin.x=mViewport.mLeft;mWorkspace.mViewportOrigin.y=mViewport.mBottom;
        mWorkspace.mViewportExtent.x=gabi::fsubs_ppc(mViewport.mRight,mViewport.mLeft);
        mWorkspace.mViewportExtent.y=gabi::fsubs_ppc(mViewport.mTop,mViewport.mBottom);
    }
    mScissor.mLeft=mViewport.mLeft;mScissor.mBottom=mViewport.mBottom;
    mScissor.mRight=mViewport.mRight;mScissor.mTop=mViewport.mTop;mScissor.mOrientation=mViewport.mOrientation;
    f32 sy=scale->y,sx=scale->x;
    f32 bottom=gabi::fmuls_ppc(mScissor.mBottom,sy),left=gabi::fmuls_ppc(mScissor.mLeft,sx);
    f32 top=gabi::fmuls_ppc(mScissor.mTop,sy),right=gabi::fmuls_ppc(mScissor.mRight,sx);
    left=gabi::fadds_ppc(left,offset->x);bottom=gabi::fadds_ppc(bottom,offset->y);
    mScissor.mLeft=left;mScissor.mBottom=bottom;
    right=gabi::fadds_ppc(right,offset->x);top=gabi::fadds_ppc(top,offset->y);
    s32 factorIndex=static_cast<s8>(static_cast<u8>(mDebugFlags));
    mScissor.mRight=right;mScissor.mTop=top;
    f32 factor=*gabi::at<const LayerScissorFactor>(gabi::ea(&sLayerScissorFactorFirst)+static_cast<u32>(factorIndex)*4);
    f32 halfFactor=gabi::fmuls_ppc(factor,0.5f);
    f32 middleX=gabi::fmuls_ppc(gabi::fadds_ppc(left,right),0.5f);
    f32 halfWidth=gabi::fmuls_ppc(gabi::fsubs_ppc(right,left),halfFactor);
    bottom=mScissor.mBottom;top=mScissor.mTop;
    f32 halfHeight=gabi::fmuls_ppc(gabi::fsubs_ppc(top,bottom),halfFactor);
    f32 middleY=gabi::fmuls_ppc(gabi::fadds_ppc(bottom,top),0.5f);
    left=gabi::fsubs_ppc(middleX,halfWidth);right=gabi::fadds_ppc(middleX,halfWidth);
    bottom=gabi::fsubs_ppc(middleY,halfHeight);top=gabi::fadds_ppc(middleY,halfHeight);
    u32 cursor=gabi::ea(static_cast<RenderMethodResource_l*>(mResources));
    u32 end=cursor+static_cast<u32>(mResourceCount)*sizeof(RenderMethodResource_l);
    mScissor.mLeft=left;mScissor.mRight=right;mScissor.mBottom=bottom;mScissor.mTop=top;
    while(cursor!=end) { updateLayerResource(gabi::at<RenderMethodResource_l>(cursor));cursor+=sizeof(RenderMethodResource_l); }
    gabi::call_ptr<void>(methods()->mUpdate.mFunction,this);
}
/* WWHD glue
ext 027C4A38 sead::SafeText* formatLayerNotification(sead::FixedSafeText<1024>*,const LayerLabelByte*,const be<u8>*,u32)
ext 02785CB4 void notifyLayerName(void*,sead::SafeText*)
ext 027AAA2C void advanceLayerCamera(sead::ViewCamera*,u32)
ext 027AAAB8 void moveLayerCamera(sead::ViewCamera*,sead::ViewVector*,sead::Viewport*)
ext 0275CEE4 u32 sead::Random::getU32()
ext 028E8970 void copyLayerProjectionMatrix(const sead::ProjectionMatrix*,sead::ProjectionMatrix*)
ext 027D57A8 void updateLayerResource(agl::RenderMethodResource_l*)
ext 0274DE2C void sead::PerspectiveState::setAngle(f32)
ext 0274D80C sead::ProjectionMatrix* sead::ProjectionState::projection()
ext 0274E910 void sead::StoredProjection::setMatrix(const sead::ProjectionMatrix*,u32)
data sLayerNotificationFormatFirst 1016A258
data sLayerScissorFactorFirst 1016A234
data sGlobalRandom 101F8B60
ext 025C0AC4 void createLayerQuadTransform(sead::ViewTransform*,const sead::ViewVector*,const sead::ViewVector*)
ext 0274E364 sead::ProjectionState* createLayerQuadProjection(sead::ProjectionState*,const sead::ViewportBounds*,f32,f32)
ext 0274D83C sead::ProjectionMatrix* sead::ProjectionState::deviceProjection()
ext 027AAABC void drawLayerClearQuad(const sead::RenderQuaternion*,const sead::ViewTransform*,const sead::ProjectionMatrix*,u32)
data sLayerDebugPasses 101F95F0
data sLayerDebugUseQuad 101F95F4
ext 02759C28 void formatLayerText(sead::SafeTextBuffer*,const LayerLabelByte*)
ext 02759C28 void formatLayerCameraText(sead::SafeTextBuffer*,const LayerLabelByte*,f64,f64,f64,f64,f64,f64,f64,f64)
ext 027C4C5C void queryLayerProjectionParameters(sead::ProjectionState*,be<f32>*,be<f32>*,be<f32>*,be<f32>*,sead::RenderVector2*)
ext 027C4A38 sead::SafeText* formatLayerProjectionText(sead::FixedSafeText<1024>*,const LayerLabelByte*,f64,f64,f64,f64,f64,f64)
data sLayerProjectionTypeMethods 10169F4C
data sLayerCameraFormatFirst 1016A1B0
data sLayerPerspectiveLabelFirst 1016A178
data sLayerStoredLabelFirst 1016A134
data sLayerFrustumLabelFirst 1016A124
data sLayerOrthographicLabelFirst 1016A190
data sLayerUnknownProjectionLabelFirst 1016A1A4
data sLayerTextSeparatorFirst 1016A120
data sLayerProjectionFormatFirst 1016A148
ext 027D56E4 void constructLayerResource(agl::RenderMethodResource_l*)
ext 027C49B8 void prepareLayerNameText(sead::SafeText*)
data sLayerPlainTextMethods 10169EEC
data sLayerResourceTextIntermediateMethods 10169FFC
data sLayerResourceTextMethods 1016A014
data sLayerDefaultName 104A0CD8
data sLayerPrepareLabelFirst 1016A210
ext 025155D8 void normalizeLayerUp(sead::ViewVector*)
data sLayerLookAtTypeMethods 10169F5C
ext 0274E474 void destroyLayerOrthographic(sead::ProjectionState*,u32)
ext 0274EB7C void destroyLayerStoredProjection(sead::StoredProjection*,u32)
ext 0274E038 void destroyLayerPerspective(sead::PerspectiveState*,u32)
ext 0274CCB0 void destroyLayerCamera(sead::ViewCamera*,u32)
ext 027D4EA4 void createLayerDispatchContext(agl::RenderLayerDispatchContext_l*,u32,u32,u32,agl::RenderLayerPrefix*)
ext 02752A84 void initializeLayerDisposer(sead::Disposer*)
ext 0274DEA8 void initializeLayerPerspective(sead::PerspectiveState*)
ext 0274E87C void initializeLayerStoredProjection(sead::StoredProjection*)
ext 0274E1C8 void initializeLayerOrthographic(sead::ProjectionState*)
ext 0274F358 sead::Viewport* sead::Viewport::createEmpty(sead::Viewport*)
ext 028F026C void registerLayerGlobal(RenderGlobalRegistration_l*)
data sLayerIdentity 104A041C
data sLayerDefaultEye 104A0928
data sLayerNegativeTarget 104A094C
data sLayerMethods 1016A2A8
data sLayerSecondaryMethods 1016A330
data sLayerCameraBaseMethods 101450D0
data sLayerCameraMethods 101450F8
data sLayerNameBaseMethods 10169F04
data sLayerNameIntermediateMethods 10169F6C
data sLayerNameMethods 10169F84
data sStoredRenderViewMethods 10145148
data sLayerDefaultView 104A2098
data sLayerDefaultProjection 104A20FC
data sLayerViewRegistration 101F95D8
data sLayerProjectionRegistration 101F95E4
*/

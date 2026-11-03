// AGL render queue records, reconstructed from assigned WWHD instructions.
#pragma once
#include "agl/RenderRecording.h"
#include "sead/Viewport.h"
#include "sead/ProjectionState.h"
#include "sead/PerspectiveState.h"
#include "sead/FileDevice.h"
#include "sead/PoolHeap.h"
#include "sead/Thread.h"
namespace agl {
struct RenderQueueManager;
struct RenderQueueConfiguration;
struct RenderDependencyGraphStorage_l { u8 mStorage[0x48]; };
// Qualified dispatch prefixes: only slots reached by this unit are named.
struct RenderLayerMethods_l {
    sead::DisposerVtable mBase;
    u8 mBeforeResources[0x10];
    sead::VtableSlot mResourceCount,mResourceName,mPrepare,mUpdate;
    sead::VtableSlot mBeginRender,mBeginMethods,mBeforeMethod,mAfterMethod;
    sead::VtableSlot mEndMethods,mEndRender,mFilter,mSelect;
};
struct RenderCameraMethods_l {
    sead::VtableSlot mHeader,mMatchesType;
    u8 mBeforeBuild[0x10];sead::VtableSlot mBuild;
};
struct RenderProjectionMethods_l {
    sead::VtableSlot mHeader,mMatchesType;
    u8 mBeforeKind[0x10];sead::VtableSlot mKind;
};
struct RenderResourceMethods_l { u8 mBeforeDestroy[0x10];sead::VtableSlot mDestroy; };
struct RenderMethodTargetTable_l { u8 mBeforeExecute[0xC];be<u32> mExecute; };
struct RenderMethodDispatchLink_l { gptr<RenderMethodTargetTable_l> mVtable; };
struct RenderMethodTarget_l { u8 mBeforeDispatch[0x20];RenderMethodDispatchLink_l mDispatch; };
struct RenderMethodResource_l {
    gptr<RenderResourceMethods_l> mVtable;be<u32> mMethodCount;u8 mReserved[4];gptr<gptr<RenderMethodTarget_l>> mMethods;
    u8 mState[0x400];be<u32> mFlags;
};
struct RenderOutputMethods_l {
    u8 mBeforeDestroy[0x18];sead::VtableSlot mDestroy;
    u8 mBeforeClear[8];sead::VtableSlot mClear;
};
struct RenderOutputPrefix_l {
    sead::RenderSurface mSurface;gptr<RenderOutputMethods_l> mVtable;
    u8 mExtendedState[0x20];be<u32> mDepthState;
};
struct RenderLayerDispatchContext_l { be<u32> mIndex;u8 mState[0x18];gptr<sead::Viewport> mViewport;be<u32> mReserved; };
struct RenderLayerWorkspace {
    be<s32> mFlashFrames;
    sead::ViewCamera mCamera,mDrawCamera;
    be<f32> mDistance;
    sead::ViewVector mCameraOffset,mPendingCameraOffset;
    be<f32> mNear,mFar,mFieldOfView;
    sead::RenderVector2 mProjectionOffset;
    gptr<sead::ProjectionState> mActiveProjection;
    sead::PerspectiveState mPerspective;
    sead::StoredProjection mStoredProjection;
    sead::ProjectionState mOrthographic;
    sead::RenderVector2 mViewportOrigin,mViewportExtent;
    be<f32> mRatio;
    be<u32> mTextCount;
    gptr<sead::FixedSafeText<512>> mTexts;
    be<u32> mUpdateCount;
    sead::ViewVector mMovement;
    be<f32> mMovementScale;
    sead::CriticalSection mLock;
    sead::FixedSafeText<256> mName;
    static RenderLayerWorkspace* create(RenderLayerWorkspace*);
};
struct StoredRenderView_l { sead::ViewTransform mTransform;gptr<void> mVtable;sead::ViewTransform mStored; };
struct RenderLayerPrefix : sead::Disposer {
    gptr<void> mLayerMethods;
    gptr<RenderQueueManager> mManager;
    sead::Viewport mViewport,mScissor;
    gptr<sead::ViewCamera> mCamera;
    gptr<sead::ProjectionState> mProjection;
    be<u32> mFlags;
    be<u8> mView;
    be<s8> mViewOverride;
    be<u8> mSelectedViews;   // bit per view the last update queued this layer in
    be<u8> mDebugClear;      // debug clear: bit 0 colour, bit 1 alpha, bit 2 depth
    be<u32> mManagerIndex;
    be<u32> mRenderState;
    sead::RenderQuaternion mClearColor;
    be<f32> mClearDepth;
    be<u32> mResourceCount;
    gptr<RenderMethodResource_l> mResources;
    be<u8> mDebugFlags;u8 mWorkspacePadding[3];
    RenderLayerWorkspace mWorkspace;
    static RenderLayerPrefix* create(RenderLayerPrefix*);
    void destroy(u32 flags);
    RenderLayerMethods_l* methods() const {
        return reinterpret_cast<RenderLayerMethods_l*>(static_cast<sead::DisposerVtable*>(mVtable));
    }
    void drawMethods(RenderMethodResource_l*,RenderLayerDispatchContext_l*);
    void dispatch(u32,u32,u32);
    void setView(u32 view);
    void* registerResourceMethod(u32 index,void* token);
    void* registerMethodAcrossResources(void* token);
    u32 countResourceReferences(void* token);
    u32 countResourceReference(u32 index,void* token);
    void* viewStateHandle() const;
    void prepareResources(sead::RegistryHeap* heap);
    void updateDebugText(u32 options);
    void update(u32,const sead::RenderVector2*,const sead::RenderVector2*);
    void render(u32,sead::RenderSurface*,u32,u32);
    u32 setCameraMode(u32 enabled);
};
struct RenderViewContext_l {
    be<u32> mState,mSelection,mReserved;gptr<void> mFrame;
    u8 mBeforeWeight[0x738-0x10];be<f32> mWeight;be<u32> mReservedWeight;gptr<RenderDependencyGraphStorage_l> mGraph;
    sead::RenderVector2 mAccessorOffset;u8 mAccessorState[0x14];
    sead::ViewportBounds mBounds;
    be<u32> mScaledState;sead::ViewportBounds mScaledBounds;be<u32> mReservedFlags,mRenderFlags;u8 mAfterFlags[0x60];
    be<u32> mPrimaryOutput,mSecondaryOutput,mSharedOutput;
};
struct RenderQueueManager {
    gptr<void> mVtable;
    sead::Disposer mDisposer;
    gptr<void> mContext;
    RenderViewContext_l mViews[2];
    gptr<void> mOutputs[2];
    be<u32> mLayerCount;
    gptr<gptr<RenderLayerPrefix>> mLayers;
    RenderDependencyGraphStorage_l mDependencyGraph;
    be<u32> mColorFormat,mDepthFormat;
    be<u16> mFlags;
    be<u8> mState;
    u8 mAlignment;
    u8 mBounds[0x14];
    sead::CriticalSection mQueueLock;
    sead::JobQueue mQueues[3];
    sead::PointerBuffer mPendingQueues[3];
    be<u32> mDrawCallback,mDrawArgument,mMode,mFrameState;
    be<f32> mExposure;
    be<f32> mJitter[2];
    be<u32> mFrameToken;
    be<f32> mRetention;
    be<u8> mPendingFrames,mFrameIndex,mFrameInterval,mEnabled,mSingleThread,mInitialized;
    u8 mTail[2];
    static RenderQueueManager* instance(sead::RegistryHeap* heap);
    static RenderQueueManager* create(RenderQueueManager* storage);
    void destroy(u32 flags);
    sead::SafeText* createName(sead::SafeText* storage,u32 index);
    sead::JobQueue* selectQueue(u32 group);
    void update(u32 execute);
    void collectJobs(u32 index);
    u32 drawView(u32 index);
    void initialize(const RenderQueueConfiguration*,sead::RegistryHeap*);
    u32 isReady() const;
    void prepareView(u32 index);
    void executeView(u32 index);
    void setViewOutput(u32 kind,u32 output);
    s32 findLayer(RenderLayerPrefix* layer) const;
    u32 removeLayer(RenderLayerPrefix* layer);
    void retireOwner(void* token);
    void setMode(u32 mode);
    void attachLayer(RenderLayerPrefix* layer,u32 index,u32 reserved,u32 view,sead::RegistryHeap* heap);
};
// Lifecycle interfaces; no additional complete derived extent is claimed.
struct RenderOwnerDisposer : sead::Disposer { void destroy(u32 flags); };
struct RenderManagerDisposer : sead::Disposer { void destroy(u32 flags); };
struct RenderDimension { be<u32> mWidth,mHeight; };
struct RenderQueueConfiguration {
    RenderDimension mOutputSizes[2],mTargetSizes[2];
    be<u32> mLayerCapacity,mCommandBytes,mMode,mFlags;
    gptr<void> mCallback;
    gptr<void> mVtable;
    static RenderQueueConfiguration* create(RenderQueueConfiguration* storage);
    void setDimensions(u32 index,const RenderDimension* output,const RenderDimension* target);
};
struct RenderOrderPrefix { u8 mState[0x1C]; be<u32> mSortKey; };
u32 compareRenderOrder(const RenderOrderPrefix*,const RenderOrderPrefix*);
}
WWHD_SIZE(agl::RenderMethodResource_l,0x414);
WWHD_OFFSET(agl::RenderLayerPrefix,mManager,0x14);
WWHD_OFFSET(agl::RenderLayerPrefix,mViewport,0x18);
WWHD_OFFSET(agl::RenderLayerPrefix,mScissor,0x30);
WWHD_OFFSET(agl::RenderLayerPrefix,mCamera,0x48);
WWHD_OFFSET(agl::RenderLayerPrefix,mProjection,0x4C);
WWHD_OFFSET(agl::RenderLayerPrefix,mFlags,0x50);
WWHD_OFFSET(agl::RenderLayerPrefix,mView,0x54);
WWHD_OFFSET(agl::RenderLayerPrefix,mViewOverride,0x55);
WWHD_OFFSET(agl::RenderLayerPrefix,mSelectedViews,0x56);
WWHD_OFFSET(agl::RenderLayerPrefix,mDebugClear,0x57);
WWHD_OFFSET(agl::RenderLayerPrefix,mManagerIndex,0x58);
WWHD_OFFSET(agl::RenderLayerPrefix,mResourceCount,0x74);
WWHD_OFFSET(agl::RenderLayerPrefix,mResources,0x78);
WWHD_SIZE(agl::RenderViewContext_l,0x800);
WWHD_OFFSET(agl::RenderViewContext_l,mBounds,0x760);
WWHD_OFFSET(agl::RenderViewContext_l,mPrimaryOutput,0x7F4);
WWHD_SIZE(agl::RenderQueueManager,0x1260);
WWHD_OFFSET(agl::RenderQueueManager,mContext,0x14);
WWHD_OFFSET(agl::RenderQueueManager,mViews,0x18);
WWHD_OFFSET(agl::RenderQueueManager,mOutputs,0x1018);
WWHD_OFFSET(agl::RenderQueueManager,mLayerCount,0x1020);
WWHD_OFFSET(agl::RenderQueueManager,mLayers,0x1024);
WWHD_OFFSET(agl::RenderQueueManager,mFlags,0x1078);
WWHD_OFFSET(agl::RenderQueueManager,mState,0x107A);
WWHD_OFFSET(agl::RenderQueueManager,mQueues,0x10CC);
WWHD_OFFSET(agl::RenderQueueManager,mPendingQueues,0x1210);
WWHD_OFFSET(agl::RenderQueueManager,mMode,0x123C);
WWHD_OFFSET(agl::RenderQueueManager,mPendingFrames,0x1258);
WWHD_OFFSET(agl::RenderQueueManager,mFrameIndex,0x1259);
WWHD_OFFSET(agl::RenderQueueManager,mFrameInterval,0x125A);
WWHD_SIZE(agl::RenderQueueConfiguration,0x38);
WWHD_OFFSET(agl::RenderQueueConfiguration,mLayerCapacity,0x20);
WWHD_OFFSET(agl::RenderQueueConfiguration,mVtable,0x34);
WWHD_OFFSET(agl::RenderOrderPrefix,mSortKey,0x1C);

extern gptr<agl::RenderQueueManager>& sAglRenderQueueManager;
extern gptr<sead::Disposer>& sAglRegisteredRenderManager;

WWHD_SIZE(agl::RenderLayerWorkspace,0x498);
WWHD_OFFSET(agl::RenderLayerWorkspace,mCamera,4);
WWHD_OFFSET(agl::RenderLayerWorkspace,mDrawCamera,0x5C);
WWHD_OFFSET(agl::RenderLayerWorkspace,mPerspective,0xE8);
WWHD_OFFSET(agl::RenderLayerWorkspace,mTextCount,0x334);
WWHD_OFFSET(agl::RenderLayerWorkspace,mLock,0x350);
WWHD_OFFSET(agl::RenderLayerWorkspace,mName,0x38C);
WWHD_OFFSET(agl::RenderLayerPrefix,mWorkspace,0x80);
WWHD_SIZE(agl::RenderLayerPrefix,0x518);
WWHD_SIZE(agl::StoredRenderView_l,0x64);

WWHD_SIZE(agl::RenderLayerDispatchContext_l,0x24);

WWHD_OFFSET(agl::RenderViewContext_l,mWeight,0x738);
WWHD_OFFSET(agl::RenderViewContext_l,mAccessorOffset,0x744);
WWHD_OFFSET(agl::RenderViewContext_l,mScaledBounds,0x778);
WWHD_OFFSET(agl::RenderViewContext_l,mRenderFlags,0x790);
WWHD_OFFSET(agl::RenderMethodResource_l,mMethodCount,4);
WWHD_OFFSET(agl::RenderMethodResource_l,mMethods,0xC);
WWHD_OFFSET(agl::RenderMethodResource_l,mFlags,0x410);
WWHD_OFFSET(agl::RenderLayerDispatchContext_l,mViewport,0x1C);

WWHD_OFFSET(agl::RenderLayerMethods_l,mResourceCount,0x20);
WWHD_OFFSET(agl::RenderLayerMethods_l,mSelect,0x78);
WWHD_OFFSET(agl::RenderCameraMethods_l,mBuild,0x20);
WWHD_OFFSET(agl::RenderProjectionMethods_l,mKind,0x20);
WWHD_OFFSET(agl::RenderMethodTarget_l,mDispatch,0x20);
WWHD_OFFSET(agl::RenderOutputPrefix_l,mVtable,0x18);
WWHD_OFFSET(agl::RenderOutputPrefix_l,mDepthState,0x3C);

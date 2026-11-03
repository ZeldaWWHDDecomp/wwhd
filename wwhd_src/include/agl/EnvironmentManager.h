// Full manager storage views derived from its 0x400-byte constructor and member accesses.
#pragma once
#include "agl/EnvironmentReferences.h"
#include "agl/EnvironmentSet.h"
#include "agl/EnvironmentBlendResource.h"
#include "agl/HierarchyNode.h"
#include "agl/ParameterRoot.h"
namespace agl { namespace env {
struct EnvironmentManager;
struct EnvironmentClassIconMethods { u8 mOtherMethods[0x40]; sead::VtableSlot mDestroy; };
// A registered class: a parameter-tree node (the manager's root lists the classes as its children; the next class is
// the next sibling and the node's table word is the class's method table) followed by its I/O interface.
struct EnvironmentClassIcon : parameter_tree::Node {
    gptr<sead::VtableSlot> mIoMethods;
    be<u32> mTypeId;
    gptr<EnvironmentManager> mManager;
    EnvironmentClassIconMethods* methods() const { return gabi::at<EnvironmentClassIconMethods>(u32(mTable)); }
    EnvironmentClassIcon* nextClass() const { return static_cast<EnvironmentClassIcon*>(static_cast<parameter_tree::Node*>(mNextSibling)); }
    static EnvironmentClassIcon* construct(EnvironmentClassIcon* object);
    void bind(u32 typeId,EnvironmentManager* manager);
    u32 supportsParameters() const;
    void onRefresh();
    u32 matchesClassResource(const EnvironmentBlendListReference* resource) const;
    void beforeResourceRead();
    void afterResourceRead();
    void onUpdateComplete();
    void destroy(u32 flags);
};
struct EnvironmentManagerRecord_l {
    sead::RenderMatrix34 mView;
    sead::RenderMatrix44 mProjection;
    be<f32> mNearPlane;
    be<f32> mFarPlane;
    be<u32> mFlags;
    gptr<void> mAttachment;
    be<u32> mOtherState;
    be<f32> mFieldOfView;
};
struct EnvironmentManagerInitialization : EnvironmentReferenceCapacity {
    be<u32> mParameterStorageArgument;
    be<s32> mRecordCount;
    static EnvironmentManagerInitialization* construct(EnvironmentManagerInitialization* object);
    void destroy(u32 flags);
};
struct EnvironmentManagerRecordBuffer { be<s32> mCount; gptr<EnvironmentManagerRecord_l> mData; };
struct EnvironmentManagerIconBuffer { be<s32> mCount; gptr<EnvironmentClassIcon> mData; };
struct EnvironmentBlendValues_l { be<f32> mValues[4]; };
struct EnvironmentManager {
    EnvironmentReferenceList mReferences;
    HierarchyNode mParameters;
    ParameterRoot mIo;
    be<u32> mDirtyFlags;
    sead::PointerBuffer mPendingObjects;
    EnvironmentManagerRecordBuffer mRecords;
    be<f32> mBlendWeight;
    EnvironmentSet mEnvironmentSet;
    EnvironmentBlendValues_l mBlendValues;
    be<u32> mMode;
    EnvironmentManagerIconBuffer mIcons;
    sead::PointerBuffer mScratchObjects;
    be<u32> mOtherStateA,mOtherStateB;
    static EnvironmentManager* construct(EnvironmentManager* object);
    void destroy(u32 flags);
    void update();
    void refreshPendingObjects();
    void setMatrices(const sead::RenderMatrix34* view,const sead::RenderMatrix44* projection,u32 recordIndex);
    const sead::SafeText* objectName(s32 objectIndex,u32 typeId) const;
    u32 classObjectCount(u32 typeId) const;
    void applyResource(const EnvironmentArchiveReference* resource);
    void blendResources(const EnvironmentArchiveReference* first,const EnvironmentArchiveReference* second,s32 selectedEntry,f64 weight);
    void initialize(const EnvironmentManagerInitialization* settings,sead::RegistryHeap* heap);
};
}}
WWHD_SIZE(agl::env::EnvironmentClassIcon,0x70);
WWHD_SIZE(agl::env::EnvironmentSet,0x190);
WWHD_SIZE(agl::env::EnvironmentManager,0x400);
WWHD_OFFSET(agl::env::EnvironmentManager,mParameters,0x20);
WWHD_OFFSET(agl::env::EnvironmentManager,mIo,0x5C);
WWHD_OFFSET(agl::env::EnvironmentManager,mDirtyFlags,0x224);
WWHD_OFFSET(agl::env::EnvironmentManager,mPendingObjects,0x228);
WWHD_OFFSET(agl::env::EnvironmentManager,mRecords,0x234);
WWHD_OFFSET(agl::env::EnvironmentManager,mEnvironmentSet,0x240);
WWHD_OFFSET(agl::env::EnvironmentManager,mBlendValues,0x3D0);
WWHD_OFFSET(agl::env::EnvironmentManager,mMode,0x3E0);
WWHD_OFFSET(agl::env::EnvironmentManager,mIcons,0x3E4);
WWHD_OFFSET(agl::env::EnvironmentManager,mScratchObjects,0x3EC);
WWHD_OFFSET(agl::env::EnvironmentClassIcon,mIoMethods,0x64);
WWHD_OFFSET(agl::env::EnvironmentClassIcon,mTypeId,0x68);
WWHD_SIZE(agl::env::EnvironmentManagerRecord_l,0x88);
WWHD_SIZE(agl::env::EnvironmentManagerInitialization,0x210);
WWHD_OFFSET(agl::env::EnvironmentManagerInitialization,mParameterStorageArgument,0x208);
WWHD_OFFSET(agl::env::EnvironmentManagerInitialization,mRecordCount,0x20C);
WWHD_OFFSET(agl::env::EnvironmentManagerRecord_l,mProjection,0x30);
WWHD_OFFSET(agl::env::EnvironmentManagerRecord_l,mAttachment,0x7C);

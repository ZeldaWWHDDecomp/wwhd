// Common environment-object fields checked against the local E8-byte constructor.
// Unused parameter-owner and parameter-I/O fields remain opaque.
#pragma once
#include "sead/FileDevice.h"
#include "agl/EnvironmentParameters.h"
#include "agl/ProjectionParameters.h"
#include "agl/ProjectionProduct.h"
#include "sead/HeapRegistry.h"
namespace agl { namespace env {
// A NULL guest pointer that the original dereferences anyway: the view of guest address 0 (low guest memory), so the
// read happens as on the console without a host null dereference.
template <class T> inline T* lowMemoryView(T* pointer) { return pointer != NULL ? pointer : reinterpret_cast<T*>(PPC_MEM_BASE); }
struct EnvironmentObjectMethods {
    u8 mOtherMethods[0x50];
    sead::VtableSlot mDestroy;
    sead::VtableSlot mPostInitialize;
    sead::VtableSlot mReset;
    sead::VtableSlot mMatricesUpdated;
    sead::VtableSlot mOtherUpdateMethod;
    // Slot names from the table words: 0x78 is a bare `blr` in every table (base
    // 027709DC, Projector 0276D8C8, lights 027728A8); 0x80 returns this+0x80 = mGroup.mValue (base 027709F0, Projector
    // 0276D8DC, lights 027728BC); 0x88 returns this+0xB8 = mName.mValue (base 027706EC, Projector 0276D804).
    sead::VtableSlot mEmptyHook;
    sead::VtableSlot mGroupText;
    sead::VtableSlot mNameText;
    u8 mOtherQueries[0x18];
    sead::VtableSlot mTypeId;
};
struct EnvironmentTextParameter {
    ParameterBinding mBinding;
    sead::FixedSafeText<32> mValue;
    void onValueChanged();
    u32 valueKind() const;
    be<u8>* value();
    const be<u8>* value() const;
    u32 valueBytes() const;
};
// The text-parameter method table 1016090C (installed by EnvironmentObject::construct 0276FCA0 on mGroup and mName)
// points at AGL#2's copies of the accessors (027708B0-02770974); the table at 101602B0/101602B8 (installed by the
// environment-set constructor for its own text parameters) points at the merged copies above (0276ED54-0276EDE0). GHS
// emitted one copy per translation unit that instantiates the class, so the two are instruction-identical; this
// layout-only twin carries AGL#2's addresses.
struct EnvironmentObjectTextParameter : EnvironmentTextParameter {
    void onValueChanged();
    u32 zeroCode();
    sead::FixedSafeText<32>* valueText();
    const sead::FixedSafeText<32>* valueText() const;
    u32 unitCode();
    u32 valueKind() const;
    be<u8>* value();
    const be<u8>* value() const;
    u32 valueBytes() const;
    u32 firstUnitCode();
    u32 secondUnitCode();
    void onFinalize();
};
struct EnvironmentResourceObject { u8 mResourceHeader[8]; be<u32> mNameHash; };
struct EnvironmentResourceReference { gptr<EnvironmentResourceObject> mObject; };
struct EnvironmentRttiTable { be<u32> mHeader[2]; };
struct EnvironmentTypeInfo { gptr<EnvironmentRttiTable> mMethods; };
// Descriptor prefix view: only its leading signed ID is read here.
struct EnvironmentClassDescriptor { be<s16> mTypeId; };
struct ProjectorClassDescriptor : EnvironmentClassDescriptor { void destroy(u32 flags); };
struct FogClassDescriptor : EnvironmentClassDescriptor { void destroy(u32 flags); };
struct EnvironmentManagerState { u8 mOtherState[0x224]; be<u32> mDirtyFlags; };
struct EnvironmentBooleanParameter { ParameterBinding mBinding; be<u8> mValue; u8 mReserved[3]; };
struct EnvironmentObject {
    u8 mParameterOwner[0x50];
    be<u32> mNameHash;
    u8 mParameterOwnerTail[4];
    gptr<EnvironmentObjectMethods> mMethods;
    gptr<sead::VtableSlot> mParameterIoMethods;
    gptr<EnvironmentManagerState> mManager;
    EnvironmentBooleanParameter mEnabled;
    EnvironmentTextParameter mGroup;
    EnvironmentTextParameter mName;
    be<u16> mObjectIndex;
    be<u8> mFlags;
    u8 mReserved;
    static EnvironmentObject* construct(EnvironmentObject* object);
    void destroy(u32 flags);
    void setEnabled(u32 enabled);
    void resetNameAndDisable();
    void refreshGroupName();
    void initialize(u32 objectIndex,void* creationArgument,EnvironmentManagerState* manager,void* userArgument);
    void finishReset();
    // The base class's own method set (table 10160A2C, sEnvironmentObjectMethods; AGL#2 027706DC-027709F0).
    const sead::SafeText* groupText() const;
    const sead::SafeText* nameData() const;
    s32 typeId() const;
    u32 enabled() const;
    void emptyQuery();
    u32 matchesResource(const EnvironmentResourceReference*) const;
    u32 isType(const EnvironmentTypeInfo*) const;
    void onPrepare();
    void onUpdate();
    void onPostUpdate();
    void onEmptyHook();
    const sead::SafeText* emptyHook() const;
    const sead::SafeText* groupData() const;
};

struct Projector : EnvironmentObject {
    ProjectionIntegerParameter mProjectionType;
    ProjectionVectorParameter mViewPosition;
    ProjectionVectorParameter mViewTarget;
    ProjectionVectorParameter mViewUp;
    ProjectionScalarParameter mNearPlane;
    ProjectionScalarParameter mFarPlane;
    ProjectionScalarParameter mAspect;
    ProjectionScalarParameter mFieldOfView;
    ProjectionScalarParameter mHeight;
    sead::RenderMatrix34 mView;
    sead::RenderMatrix44 mProjection;
    ProjectionProduct mViewProjection;
    static Projector* construct(Projector* object,u32 projectionKind);
    static Projector* create(sead::RegistryHeap* heap);
    static void registerClass();
    void updateView();
    void updateProjection();
    void updateMatrices();
    void updateViewProjection();
    void destroy(u32 flags);
    const sead::SafeText* groupText() const;
    const sead::SafeText* emptyHook() const;
    s32 typeId() const;
    u32 matchesResource(const EnvironmentResourceReference*) const;
    const sead::SafeText* nameData() const;
    const sead::SafeText* groupData() const;
    u32 enabled() const;
    u32 isType(const EnvironmentTypeInfo*) const;
    static s32 registeredTypeId();
    u32 supportsParameters() const;
    u32 acceptsResourceLists() const;
    void beforeResourceRead();
    void onInitialize(void* creationArgument,void* userArgument);
    void onMatricesUpdated(const sead::RenderMatrix34* view,const sead::RenderMatrix44* projection,u32 recordIndex);
    void onRefresh();

};
struct Fog : EnvironmentObject {
    EnvironmentScalarParameter mStart;
    EnvironmentScalarParameter mEnd;
    EnvironmentColorParameter mColor;
    EnvironmentVectorParameter mDirection;
    static Fog* construct(Fog* object);
    static Fog* create(sead::RegistryHeap* heap);
    static void registerClass();
    void destroy(u32 flags);
    const sead::SafeText* groupText() const;
    const sead::SafeText* emptyHook() const;
    s32 typeId() const;
    u32 matchesResource(const EnvironmentResourceReference*) const;
    const sead::SafeText* nameData() const;
    const sead::SafeText* groupData() const;
    u32 enabled() const;
    u32 isType(const EnvironmentTypeInfo*) const;
    static s32 registeredTypeId();
    u32 supportsParameters() const;
    u32 acceptsResourceLists() const;
    void beforeResourceRead();
    void onInitialize(void* creationArgument,void* userArgument);
    void onReset();
    void onMatricesUpdated(const sead::RenderMatrix34* view,const sead::RenderMatrix44* projection,u32 recordIndex);
    void onRefresh();

};
}}
// The base class's runtime type and its lazy-initialisation word: data owned by agl/EnvironmentObjects.cpp (one
// definition); the light classes' isType copies (AGL#2) test the same words.
extern be<u32>& sEnvironmentTypeReady;
extern agl::env::EnvironmentTypeInfo& sEnvironmentType;
WWHD_SIZE(agl::env::EnvironmentObjectMethods,176);
WWHD_SIZE(agl::env::EnvironmentTextParameter,56);
WWHD_SIZE(agl::env::EnvironmentResourceObject,12);
WWHD_SIZE(agl::env::EnvironmentResourceReference,4);
WWHD_SIZE(agl::env::EnvironmentRttiTable,8);
WWHD_SIZE(agl::env::EnvironmentTypeInfo,4);
WWHD_SIZE(agl::env::EnvironmentClassDescriptor,2);
WWHD_SIZE(agl::env::EnvironmentObject,232);
WWHD_SIZE(agl::env::Projector,576);
WWHD_SIZE(agl::env::Fog,316);
WWHD_OFFSET(agl::env::EnvironmentObjectMethods,mEmptyHook,120);
WWHD_OFFSET(agl::env::EnvironmentObjectMethods,mGroupText,128);
WWHD_OFFSET(agl::env::EnvironmentObjectMethods,mTypeId,168);
WWHD_OFFSET(agl::env::EnvironmentTextParameter,mValue,12);
WWHD_OFFSET(agl::env::EnvironmentResourceObject,mNameHash,8);
WWHD_OFFSET(agl::env::EnvironmentObject,mNameHash,80);
WWHD_OFFSET(agl::env::EnvironmentObject,mMethods,88);
WWHD_OFFSET(agl::env::EnvironmentObject,mGroup,116);
WWHD_OFFSET(agl::env::EnvironmentObject,mName,172);
WWHD_OFFSET(agl::env::EnvironmentObject,mObjectIndex,228);
WWHD_OFFSET(agl::env::EnvironmentObject,mFlags,230);
WWHD_OFFSET(agl::env::Fog,mStart,0xE8);
WWHD_OFFSET(agl::env::Fog,mEnd,0xF8);
WWHD_OFFSET(agl::env::Fog,mColor,0x108);
WWHD_OFFSET(agl::env::Fog,mDirection,0x124);
WWHD_OFFSET(agl::env::Projector,mView,0x190);
WWHD_OFFSET(agl::env::Projector,mProjection,0x1C0);
WWHD_OFFSET(agl::env::Projector,mViewProjection,0x200);
WWHD_OFFSET(agl::env::Projector,mProjectionType,0xE8);
WWHD_OFFSET(agl::env::Projector,mViewPosition,0xF8);
WWHD_OFFSET(agl::env::Projector,mViewTarget,0x110);
WWHD_OFFSET(agl::env::Projector,mViewUp,0x128);
WWHD_OFFSET(agl::env::Projector,mNearPlane,0x140);
WWHD_OFFSET(agl::env::Projector,mFarPlane,0x150);
WWHD_OFFSET(agl::env::Projector,mAspect,0x160);
WWHD_OFFSET(agl::env::Projector,mFieldOfView,0x170);
WWHD_OFFSET(agl::env::Projector,mHeight,0x180);

WWHD_SIZE(agl::env::EnvironmentBooleanParameter,16);
WWHD_OFFSET(agl::env::EnvironmentObject,mManager,0x60);
WWHD_OFFSET(agl::env::EnvironmentObject,mEnabled,0x64);
WWHD_OFFSET(agl::env::EnvironmentManagerState,mDirtyFlags,0x224);
WWHD_OFFSET(agl::env::EnvironmentObjectMethods,mReset,0x60);
WWHD_OFFSET(agl::env::EnvironmentObjectMethods,mPostInitialize,0x58);
WWHD_OFFSET(agl::env::EnvironmentObjectMethods,mDestroy,0x50);
WWHD_OFFSET(agl::env::EnvironmentObjectMethods,mMatricesUpdated,0x68);
WWHD_OFFSET(agl::env::EnvironmentObjectMethods,mNameText,0x88);

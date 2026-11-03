// Environment manager and class-icon construction, preserving placement and callback order.

// settings (128 per-class counts, total, two settings words with their defaults), each checked against this game's
// own constructors of both.
#include "agl/EnvironmentManager.h"
using namespace agl::env;
typedef be<u8> EnvironmentManagerTextByte;
static sead::VtableSlot sEnvironmentManagerReferenceMethods;
static agl::HierarchyNodeDispatch sEnvironmentManagerParameterMethods;
static sead::VtableSlot sEnvironmentManagerIoMethods;
static sead::SafeTextVtable sEnvironmentManagerTextMethods;
static EnvironmentManagerTextByte sEnvironmentManagerName;
static EnvironmentManagerTextByte sEnvironmentClassIconName;
static sead::VtableSlot sEnvironmentClassIconIoMethods;
static EnvironmentClassIconMethods sEnvironmentClassIconMethods;
static f32 sEnvironmentManagerBlendDefault;
void* allocateEnvironmentManagerStorage(u32);
void constructManagerReferences(EnvironmentReferenceList*);
void constructManagerSet(EnvironmentSet*);
void constructEnvironmentClassIconBase(agl::parameter_tree::Node*);
void processEnvironmentClassIconName(sead::VtableSlot*,const sead::SafeText*);

/* inlined into EnvironmentManager::construct */
template<class T> static T* managerPlacement(T* object) {
    if (gabi::ea(object) == 0) object = static_cast<T*>(allocateEnvironmentManagerStorage(sizeof(T)));
    return object;
}
/* inlined into EnvironmentManager::construct */
static void initializeManagerPointerBuffer(sead::PointerBuffer* buffer) {
    buffer = managerPlacement(buffer);
    if (buffer == NULL) return;
    buffer->mCapacity = 0;
    buffer->mCount = 0;
    buffer->mStorage = NULL;
}
/* inlined into EnvironmentManager::construct */
template<class T> static void initializeManagerArray(T* buffer) {
    buffer = managerPlacement(buffer);
    if (buffer == NULL) return;
    buffer->mData = NULL;
    buffer->mCount = 0;
}

/* 0276C1DC */
EnvironmentManager* EnvironmentManager::construct(EnvironmentManager* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<EnvironmentManager*>(allocateEnvironmentManagerStorage(sizeof(EnvironmentManager)));
    if (object == NULL) return NULL;
    constructManagerReferences(&object->mReferences);
    agl::HierarchyNode::create(&object->mParameters);
    gabi::FrameLocal<sead::SafeText> name(8);
    name->mVtable = &sEnvironmentManagerTextMethods;
    name->mText = &sEnvironmentManagerName;
    agl::ParameterRoot::create(&object->mIo,name.get(),0);
    object->mReferences.mMethods = &sEnvironmentManagerReferenceMethods;
    object->mIo.mTree.mTable = gabi::ea(&sEnvironmentManagerIoMethods);
    object->mParameters.mTable = &sEnvironmentManagerParameterMethods;
    be<u32>* flags = managerPlacement(&object->mDirtyFlags);
    if (flags != NULL) *flags = 0;
    initializeManagerPointerBuffer(&object->mPendingObjects);
    initializeManagerArray(&object->mRecords);
    object->mBlendWeight = sEnvironmentManagerBlendDefault;
    constructManagerSet(&object->mEnvironmentSet);
    if (gabi::ea(&object->mBlendValues) == 0) allocateEnvironmentManagerStorage(sizeof(EnvironmentBlendValues_l));
    object->mMode = 2;
    initializeManagerArray(&object->mIcons);
    initializeManagerPointerBuffer(&object->mScratchObjects);
    object->mOtherStateB = 0;
    object->mOtherStateA = 0;
    return object;
}
/* 0276C35C */
EnvironmentClassIcon* EnvironmentClassIcon::construct(EnvironmentClassIcon* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<EnvironmentClassIcon*>(allocateEnvironmentManagerStorage(sizeof(EnvironmentClassIcon)));
    if (object == NULL) return NULL;
    constructEnvironmentClassIconBase(object);
    object->mManager = NULL;
    object->mIoMethods = &sEnvironmentClassIconIoMethods;
    object->mTypeId = 0;
    object->mTable = gabi::ea(&sEnvironmentClassIconMethods);
    gabi::FrameLocal<sead::SafeText> name(8);
    name->mVtable = &sEnvironmentManagerTextMethods;
    name->mText = &sEnvironmentClassIconName;
    processEnvironmentClassIconName(reinterpret_cast<sead::VtableSlot*>(&object->mIoMethods),name.get());
    return object;
}
/* 0276C3EC */
void EnvironmentClassIcon::bind(u32 typeId,EnvironmentManager* manager) {
    mTypeId = typeId;
    mManager = manager;
}


void releaseEnvironmentClassIcon(void*);
void releaseManagerPendingObjects(sead::PointerBuffer*);
sead::RegistryHeap* managerStorageHeap(void*,void*);
void clearManagerSetReferences(EnvironmentReferenceList*);
void destroyManagerSet(EnvironmentSet*,u32);
void destroyManagerReferences(EnvironmentReferenceList*,u32);
void releaseEnvironmentManager(void*);

/* 0276CDD0 */
void EnvironmentClassIcon::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    mIoMethods = &sEnvironmentClassIconIoMethods;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEnvironmentClassIcon),this);
}
/* 0276B21C */
void EnvironmentManager::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    EnvironmentReference* reference = mReferences.mReferences.mData;
    u32 referenceCount = u32(mReferences.mReferences.mCount);
    mReferences.mMethods = &sEnvironmentManagerReferenceMethods;
    EnvironmentReference* referenceEnd = reference + referenceCount;
    mParameters.mTable = &sEnvironmentManagerParameterMethods;
    mIo.mTree.mTable = gabi::ea(&sEnvironmentManagerIoMethods);
    while (gabi::ea(reference) != gabi::ea(referenceEnd)) {
        EnvironmentObject* object = reference->mObject;
        if (object != NULL) gabi::call_ptr<void>(object->mMethods->mDestroy.mFunction,object,u32(3));
        ++reference;
    }
    u32 recordAddress = gabi::ea(static_cast<EnvironmentManagerRecord_l*>(mRecords.mData));
    u32 recordEnd = recordAddress + u32(mRecords.mCount) * u32(sizeof(EnvironmentManagerRecord_l));
    // These records have trivial destructors; the original still walks their extents.
    while (recordAddress != recordEnd) recordAddress += sizeof(EnvironmentManagerRecord_l);
    releaseManagerPendingObjects(&mPendingObjects);
    EnvironmentClassIcon* icons = mIcons.mData;
    if (icons != NULL) {
        for (s32 index = 0; index < s32(mIcons.mCount); ++index) {
            EnvironmentClassIcon* icon = icons + index;
            gabi::call_ptr<void>(icon->methods()->mDestroy.mFunction,icon,u32(2));
            icons = mIcons.mData;
        }
        sead::RegistryHeap* heap = managerStorageHeap(sRegistryManager,mIcons.mData);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,static_cast<EnvironmentClassIcon*>(mIcons.mData));
        mIcons.mCount = 0;
        mIcons.mData = NULL;
    }
    if (mRecords.mData != NULL) {
        sead::RegistryHeap* heap = managerStorageHeap(sRegistryManager,mRecords.mData);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,static_cast<EnvironmentManagerRecord_l*>(mRecords.mData));
        mRecords.mCount = 0;
        mRecords.mData = NULL;
    }
    clearManagerSetReferences(&mEnvironmentSet.mReferences);
    destroyManagerSet(&mEnvironmentSet,2);
    mParameters.destroy(0);
    destroyManagerReferences(&mReferences,0);
    if (flags & 1) releaseEnvironmentManager(this);
}

/* WWHD glue
 ext 0273AF40 void releaseEnvironmentClassIcon(void*) callback
 ext 0273B5DC void releaseManagerPendingObjects(sead::PointerBuffer*)
 ext 02755FEC sead::RegistryHeap* managerStorageHeap(void*,void*)
 ext 0276F448 void clearManagerSetReferences(EnvironmentReferenceList*)
 ext 0276DE44 void destroyManagerSet(EnvironmentSet*,u32)
 ext 0279B8D0 void agl::HierarchyNode::destroy(u32 flags)
 ext 0276F4F8 void destroyManagerReferences(EnvironmentReferenceList*,u32)
 ext 0273AF40 void releaseEnvironmentManager(void*)
 data sEnvironmentManagerReferenceMethods 1015FCB4
 data sEnvironmentManagerParameterMethods 1015FD0C
 data sEnvironmentManagerIoMethods 1015FD4C
 data sEnvironmentManagerTextMethods 1015FB48
 data sEnvironmentManagerName 1015FC68
 data sEnvironmentClassIconName 1015FC70
 data sEnvironmentClassIconIoMethods 1015FBA8
 data sEnvironmentClassIconMethods 1015FC00
 data sEnvironmentManagerBlendDefault 1015FC50
 ext 0273AD10 void* allocateEnvironmentManagerStorage(u32)
 ext 0276F3A8 void constructManagerReferences(EnvironmentReferenceList*)
 ext 0279B7B4 agl::HierarchyNode* agl::HierarchyNode::create(HierarchyNode* node)
 ext 0279A07C agl::ParameterRoot* agl::ParameterRoot::create(ParameterRoot* root, sead::SafeText* name, u32 context)
 ext 0276DC4C void constructManagerSet(EnvironmentSet*)
 ext 0279CA34 void constructEnvironmentClassIconBase(agl::parameter_tree::Node*)
 ext 02785CB4 void processEnvironmentClassIconName(sead::VtableSlot*,const sead::SafeText*)
*/

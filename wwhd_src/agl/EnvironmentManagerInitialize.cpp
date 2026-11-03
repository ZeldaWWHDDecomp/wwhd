// Manager initialization, with layouts derived from this game's member and allocation accesses.

// settings (128 per-class counts, total, two settings words with their defaults), each checked against this game's
// own constructors of both.
#include "agl/EnvironmentManager.h"
#include <atomic>
using namespace agl::env;
typedef be<s32> ManagerInitializationCount;
typedef be<u8> ManagerInitializationTextByte;
struct ManagerInitializationMethods_l { u8 mPrefix[0x10]; sead::VtableSlot mInitialize; };
static ManagerInitializationCount sManagerInitializationClassCount;
static EnvironmentClassEntry sManagerInitializationEntries;
static sead::SafeTextVtable sManagerInitializationTextMethods;
static ManagerInitializationTextByte sManagerInitializationObjectFormat;
static ManagerInitializationTextByte sManagerInitializationName;
static f32 sManagerInitializationNear;
static f32 sManagerInitializationFar;
static f32 sManagerInitializationFieldOfView;
void initializeManagerEnvironmentSet(EnvironmentSet*,const EnvironmentReferenceCapacity*,sead::RegistryHeap*);
void bindInitializedManagerSet(EnvironmentSet*,EnvironmentManager*);
void allocateManagerPendingObjects(sead::PointerBuffer*,u32,sead::RegistryHeap*,u32);
sead::RegistryHeap* currentManagerInitializationHeap(void*);
void constructManagerInitializationIcon(EnvironmentClassIcon*);
void bindManagerInitializationIcon(EnvironmentClassIcon*,u32,EnvironmentManager*);
void addManagerInitializationList(agl::parameter_tree::Node*,agl::parameter_tree::Node*,const sead::SafeText*);
void initializeManagerEnvironmentObject(EnvironmentObject*,u32,u32,EnvironmentManagerState*,sead::RegistryHeap*);
u32 insertManagerInitializationObject(EnvironmentSet*,EnvironmentObject*);
void prepareManagerInitializationText(const sead::SafeText*);
sead::SafeText* formatManagerInitializationObject(sead::FixedSafeText<1024>*,const be<u8>*,const be<u8>*,u32);
void addManagerInitializationObject(agl::parameter_tree::Node*,agl::parameter_tree::Value*,const sead::SafeText*);
void initializeManagerViewMatrix(sead::RenderMatrix34*);
void initializeManagerProjectionMatrix(sead::RenderMatrix44*);
void setManagerInitializationName(agl::HierarchyNode*,const sead::SafeText*);
void* allocateManagerInitializationStorage(u32);
/* 0276C3F8 */
void EnvironmentManager::initialize(const EnvironmentManagerInitialization* settings,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    ManagerInitializationMethods_l* methods = reinterpret_cast<ManagerInitializationMethods_l*>(static_cast<sead::VtableSlot*>(mReferences.mMethods));
    gabi::call_ptr<void>(methods->mInitialize.mFunction,this,settings,heap);
    initializeManagerEnvironmentSet(&mEnvironmentSet,settings,heap);
    bindInitializedManagerSet(&mEnvironmentSet,this);
    allocateManagerPendingObjects(&mPendingObjects,u32(mReferences.mReferences.mCount),heap,4);
    s32 classCount = sManagerInitializationClassCount;
    if (classCount > 0) {
        sead::RegistryHeap* allocationHeap = heap;
        if (allocationHeap == NULL) allocationHeap = currentManagerInitializationHeap(sRegistryManager);
        EnvironmentClassIcon* storage = gabi::call_ptr<EnvironmentClassIcon*>(allocationHeap->dispatch()->mAllocate.mFunction,
            allocationHeap,u32(classCount) * u32(sizeof(EnvironmentClassIcon)),4);
        u32 count = classCount > 0 ? u32(classCount) : 1;
        for (u32 index = 0; index < count; ++index) {
            EnvironmentClassIcon* icon = storage + index;
            if (gabi::ea(icon) != 0) constructManagerInitializationIcon(icon);
        }
        if (storage != NULL) { mIcons.mData = storage; mIcons.mCount = classCount; }
    }
    s32 recordCount = settings->mRecordCount;
    if (recordCount > 0) {
        sead::RegistryHeap* allocationHeap = heap;
        if (allocationHeap == NULL) allocationHeap = currentManagerInitializationHeap(sRegistryManager);
        EnvironmentManagerRecord_l* storage = gabi::call_ptr<EnvironmentManagerRecord_l*>(allocationHeap->dispatch()->mAllocate.mFunction,
            allocationHeap,u32(recordCount) * u32(sizeof(EnvironmentManagerRecord_l)),4);
        u32 count = recordCount > 0 ? u32(recordCount) : 1;
        for (u32 index = 0; index < count; ++index) {
            EnvironmentManagerRecord_l* record = storage + index;
            if (gabi::ea(record) == 0) continue;
            gptr<void>* attachment = &record->mAttachment;
            if (gabi::ea(attachment) == 0) attachment = static_cast<gptr<void>*>(allocateManagerInitializationStorage(sizeof(gptr<void>)));
            if (attachment != NULL) *attachment = NULL;
        }
        if (storage != NULL) { mRecords.mData = storage; mRecords.mCount = recordCount; }
    }
    EnvironmentClassIcon* firstIcon = mIcons.mData;
    EnvironmentClassIcon* iconEnd = firstIcon + u32(mIcons.mCount);
    for (EnvironmentClassIcon* icon = firstIcon; gabi::ea(icon) != gabi::ea(iconEnd); ++icon) {
        u32 typeId = (gabi::ea(icon) - gabi::ea(firstIcon)) / sizeof(EnvironmentClassIcon);
        bindManagerInitializationIcon(icon,typeId,this);
        EnvironmentClassEntry* entry = (&sManagerInitializationEntries) + typeId;
        gabi::FrameLocal<sead::SafeText> iconName(0x10);
        iconName->mVtable = &sManagerInitializationTextMethods;
        iconName->mText = entry->mName;
        addManagerInitializationList(&mIo.mTree,icon,iconName.get());
        u32 rangeCount = u32(mReferences.mRanges.mCount);
        EnvironmentReferenceRange* range = mReferences.mRanges.mData;
        if (typeId < rangeCount) range += typeId;
        if (range->mCount == 0) continue;
        u32 objectCount = range->mCount;
        for (u32 objectIndex = 0; objectIndex < objectCount; ++objectIndex) {
            EnvironmentObject* object = gabi::call_ptr<EnvironmentObject*>(entry->mFactory,heap);
            initializeManagerEnvironmentObject(object,objectIndex,u32(settings->mRecordCount),
                reinterpret_cast<EnvironmentManagerState*>(this),heap);
            rangeCount = u32(mReferences.mRanges.mCount);
            range = mReferences.mRanges.mData;
            if (typeId < rangeCount) range += typeId;
            u32 index = u32(range->mFirst) + objectIndex;
            EnvironmentReference* reference = mReferences.mReferences.mData;
            if (index < u32(mReferences.mReferences.mCount)) reference += index;
            reference->mObject = object;
            insertManagerInitializationObject(&mEnvironmentSet,object);
            u32 objectType = gabi::call_ptr<u32>(object->mMethods->mTypeId.mFunction,object);
            EnvironmentClassEntry* objectEntry = (&sManagerInitializationEntries) + objectType;
            gabi::FrameLocal<sead::SafeText> className(8);
            className->mVtable = &sManagerInitializationTextMethods;
            className->mText = objectEntry->mName;
            prepareManagerInitializationText(className.get());
            gabi::FrameLocal<sead::FixedSafeText<1024>> objectName(0x48);
            gabi::cpu->cr[6] = 0;
            sead::SafeText* name = formatManagerInitializationObject(objectName.get(),&sManagerInitializationObjectFormat,className->mText,objectIndex);
            addManagerInitializationObject(icon,reinterpret_cast<agl::parameter_tree::Value*>(object),name);
        }
    }
    EnvironmentManagerRecord_l* record = mRecords.mData;
    EnvironmentManagerRecord_l* recordEnd = record + u32(mRecords.mCount);
    if (gabi::ea(record) != gabi::ea(recordEnd)) {
        f32 nearPlane = sManagerInitializationNear;
        f32 farPlane = sManagerInitializationFar;
        f32 fieldOfView = sManagerInitializationFieldOfView;
        for (; gabi::ea(record) != gabi::ea(recordEnd); ++record) {
            initializeManagerViewMatrix(&record->mView);
            initializeManagerProjectionMatrix(&record->mProjection);
            record->mAttachment = NULL;
            record->mNearPlane = nearPlane;
            record->mOtherState = 0;
            record->mFieldOfView = fieldOfView;
            record->mFlags = 0;
            record->mFarPlane = farPlane;
        }
    }
    for (u32 index = 0; index < 4; ++index) mBlendValues.mValues[index] = 0.0f;
    mParameters.allocateEntries(s32(u32(mReferences.mReferences.mCount)),s32(u32(settings->mParameterStorageArgument)),heap);
    EnvironmentReference* reference = mReferences.mReferences.mData;
    EnvironmentReference* referenceEnd = reference + u32(mReferences.mReferences.mCount);
    for (; gabi::ea(reference) != gabi::ea(referenceEnd); ++reference) {
        EnvironmentObject* object = reference->mObject;
        void* io = object == NULL ? NULL : &object->mParameterIoMethods;
        mParameters.appendChild(static_cast<agl::HierarchyLeafPrefix*>(io));
    }
    gabi::FrameLocal<sead::SafeText> name(0x18);
    name->mVtable = &sManagerInitializationTextMethods;
    name->mText = &sManagerInitializationName;
    setManagerInitializationName(&mParameters,name.get());
    u32 dirtyFlags = mDirtyFlags;
    u32 parameterFlags = mParameters.mFlags;
    mDirtyFlags = dirtyFlags | 1;
    mParameters.mFlags = u16(parameterFlags | 1);
    std::atomic_thread_fence(std::memory_order_acquire);
}
/* WWHD glue
 data sManagerInitializationClassCount 101F8BF4
 data sManagerInitializationEntries 104A1564
 data sManagerInitializationTextMethods 1015FB48
 data sManagerInitializationObjectFormat 1015FC90
 data sManagerInitializationName 1015FC98
 data sManagerInitializationNear 1015FC8C
 data sManagerInitializationFar 1015FC88
 data sManagerInitializationFieldOfView 1015FC84
 ext 0276DF64 void initializeManagerEnvironmentSet(EnvironmentSet*,const EnvironmentReferenceCapacity*,sead::RegistryHeap*)
 ext 0276DDD0 void bindInitializedManagerSet(EnvironmentSet*,EnvironmentManager*)
 ext 0273B584 void allocateManagerPendingObjects(sead::PointerBuffer*,u32,sead::RegistryHeap*,u32)
 ext 02756140 sead::RegistryHeap* currentManagerInitializationHeap(void*)
 ext 0276C35C void constructManagerInitializationIcon(EnvironmentClassIcon*)
 ext 0276C3EC void bindManagerInitializationIcon(EnvironmentClassIcon*,u32,EnvironmentManager*)
 ext 0279CB24 void addManagerInitializationList(agl::parameter_tree::Node*,agl::parameter_tree::Node*,const sead::SafeText*)
 ext 02770564 void initializeManagerEnvironmentObject(EnvironmentObject*,u32,u32,EnvironmentManagerState*,sead::RegistryHeap*)
 ext 0276E774 u32 insertManagerInitializationObject(EnvironmentSet*,EnvironmentObject*)
 ext 0276CC10 void prepareManagerInitializationText(const sead::SafeText*)
 ext 0276CC68 sead::SafeText* formatManagerInitializationObject(sead::FixedSafeText<1024>*,const be<u8>*,const be<u8>*,u32)
 ext 0279CB9C void addManagerInitializationObject(agl::parameter_tree::Node*,agl::parameter_tree::Value*,const sead::SafeText*)
 ext 028E9098 void initializeManagerViewMatrix(sead::RenderMatrix34*)
 ext 028E892C void initializeManagerProjectionMatrix(sead::RenderMatrix44*)
 ext 0279BC70 void agl::HierarchyNode::allocateEntries(s32 childCapacity, s32 itemCount, sead::RegistryHeap* heap)
 ext 0279BD8C void agl::HierarchyNode::appendChild(HierarchyLeafPrefix* child)
 ext 02785CB4 void setManagerInitializationName(agl::HierarchyNode*,const sead::SafeText*)
 ext 0273AD10 void* allocateManagerInitializationStorage(u32)
*/

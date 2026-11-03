// Pending-object refresh, matrix updates and class queries from the local manager routines.
#include "agl/EnvironmentManager.h"
using namespace agl::env;
static f32 sManagerOperationOne;
static f32 sManagerOperationHalf;
static sead::VtableSlot sManagerInitializationMethods;
void finishManagerPendingObject(EnvironmentObject*);
void copyManagerViewMatrix(const sead::RenderMatrix34*,sead::RenderMatrix34*);
void copyManagerProjectionMatrix(const sead::RenderMatrix44*,sead::RenderMatrix44*);
void* allocateManagerSettings(u32);
void constructManagerReferenceCapacity(EnvironmentReferenceCapacity*);
void releaseManagerSettings(void*);
void applyManagerResourcePair(EnvironmentManager*,const EnvironmentArchiveReference*,const EnvironmentArchiveReference*,s32,f32);
/* 0276C8B8 */
void EnvironmentManager::refreshPendingObjects() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (u32(mDirtyFlags) & 1) {
        EnvironmentReference* reference = mReferences.mReferences.mData;
        EnvironmentReference* end = reference + u32(mReferences.mReferences.mCount);
        mPendingObjects.mCount = 0;
        for (; gabi::ea(reference) != gabi::ea(end); ++reference) {
            EnvironmentObject* object = reference->mObject;
            if (object->mEnabled.mValue == 0) continue;
            finishManagerPendingObject(object);
            s32 count = mPendingObjects.mCount;
            s32 capacity = mPendingObjects.mCapacity;
            object = reference->mObject;
            if (count < capacity) {
                gptr<EnvironmentObject>* storage = static_cast<gptr<EnvironmentObject>*>(static_cast<void*>(mPendingObjects.mStorage));
                storage[u32(count)] = object;
                mPendingObjects.mCount = s32(u32(mPendingObjects.mCount) + 1);
            }
        }
        mDirtyFlags = u32(mDirtyFlags) & ~u32(1);
        mParameters.clearFirstFlag();
    } else {
        gptr<EnvironmentObject>* storage = static_cast<gptr<EnvironmentObject>*>(static_cast<void*>(mPendingObjects.mStorage));
        gptr<EnvironmentObject>* object = reinterpret_cast<gptr<EnvironmentObject>*>(storage);
        gptr<EnvironmentObject>* end = object + u32(mPendingObjects.mCount);
        for (; gabi::ea(object) != gabi::ea(end); ++object) finishManagerPendingObject(*object);
        mParameters.clearFirstFlag();
    }
}
/* 0276C9B4 */
void EnvironmentManager::setMatrices(const sead::RenderMatrix34* view,const sead::RenderMatrix44* projection,u32 recordIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    EnvironmentManagerRecord_l* record = mRecords.mData;
    if (recordIndex < u32(mRecords.mCount)) record += recordIndex;
    copyManagerViewMatrix(view,&record->mView);
    copyManagerProjectionMatrix(projection,&record->mProjection);
    f32 one = sManagerOperationOne;
    record->mFlags = 0;
    f32 projectionDepth = projection->mRows[2][2];
    f32 half = sManagerOperationHalf;
    f32 numerator = gabi::fsubs_ppc(one,projectionDepth);
    f32 depthTranslation = projection->mRows[2][3];
    f32 denominator = gabi::fadds_ppc(projectionDepth,one);
    f32 scaledTranslation = gabi::fmuls_ppc(half,depthTranslation);
    f32 ratio = gabi::fdivs_ppc(numerator,denominator);
    f32 sum = gabi::fadds_ppc(ratio,one);
    f32 farPlane = gabi::fmuls_ppc(scaledTranslation,sum);
    f32 nearPlane = -gabi::fdivs_ppc(farPlane,ratio);
    record->mFarPlane = farPlane;
    record->mNearPlane = nearPlane;
    gptr<EnvironmentObject>* storage = static_cast<gptr<EnvironmentObject>*>(static_cast<void*>(mPendingObjects.mStorage));
    gptr<EnvironmentObject>* object = reinterpret_cast<gptr<EnvironmentObject>*>(storage);
    gptr<EnvironmentObject>* end = object + u32(mPendingObjects.mCount);
    for (; gabi::ea(object) != gabi::ea(end); ++object) {
        EnvironmentObject* current = *object;
        gabi::call_ptr<void>(current->mMethods->mMatricesUpdated.mFunction,current,view,projection,recordIndex);
    }
}
/* 0276CAA4 */
EnvironmentManagerInitialization* EnvironmentManagerInitialization::construct(EnvironmentManagerInitialization* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<EnvironmentManagerInitialization*>(allocateManagerSettings(sizeof(EnvironmentManagerInitialization)));
    if (object == NULL) return NULL;
    constructManagerReferenceCapacity(object);
    object->mParameterStorageArgument = 256;
    object->mMethods = &sManagerInitializationMethods;
    object->mRecordCount = 1;
    return object;
}
/* 0276CB08 */
void EnvironmentManagerInitialization::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseManagerSettings),this);
}
/* 0276CB1C */
const sead::SafeText* EnvironmentManager::objectName(s32 objectIndex,u32 typeId) const {
    EnvironmentObject* object = NULL;
    if (objectIndex >= 0) {
        u32 rangeCount = u32(mReferences.mRanges.mCount);
        const EnvironmentReferenceRange* range = mReferences.mRanges.mData;
        if (typeId < rangeCount) range += typeId;
        if (objectIndex < s32(u32(range->mCount))) {
            u32 index = u32(range->mFirst) + u32(objectIndex);
            const EnvironmentReference* reference = NULL;
            if (index < u32(mReferences.mReferences.mCount)) reference = static_cast<EnvironmentReference*>(mReferences.mReferences.mData) + index;
            // An index outside the extent reads the object word through a NULL reference, as the original does.
            object = lowMemoryView(reference)->mObject;
        }
    }
    // The method lookup runs through a NULL object too, reading low guest memory like the original.
    u32 nameFunction = lowMemoryView(object)->mMethods->mGroupText.mFunction;
    return gabi::tail_ptr<const sead::SafeText*>(nameFunction,object);
}
/* 0276CBB0 */
u32 EnvironmentManager::classObjectCount(u32 typeId) const {
    u32 count = u32(mReferences.mRanges.mCount);
    const EnvironmentReferenceRange* range = mReferences.mRanges.mData;
    if (typeId < count) range += typeId;
    return range->mCount;
}
/* 0276CBD0 */
void EnvironmentManager::applyResource(const EnvironmentArchiveReference* resource) {
    gabi::AutoFrame frame{gabi::current_function()};
    agl::ParameterRootInput_l* archive = resource->mArchive;
    gabi::FrameLocal<EnvironmentArchiveReference> first(8);
    gabi::FrameLocal<EnvironmentArchiveReference> second(0xC);
    first->mArchive = archive;
    second->mArchive = archive;
    applyManagerResourcePair(this,first.get(),second.get(),-1,sManagerOperationOne);
}
/* WWHD glue
 data sManagerOperationOne 1015FC50
 data sManagerOperationHalf 1015FCA8
 data sManagerInitializationMethods 1015FB98
 ext 02770638 void finishManagerPendingObject(EnvironmentObject*)
 ext 0279BE24 void agl::HierarchyNode::clearFirstFlag()
 ext 028E90D4 void copyManagerViewMatrix(const sead::RenderMatrix34*,sead::RenderMatrix34*)
 ext 028E8970 void copyManagerProjectionMatrix(const sead::RenderMatrix44*,sead::RenderMatrix44*)
 ext 0273AD10 void* allocateManagerSettings(u32)
 ext 0276F968 void constructManagerReferenceCapacity(EnvironmentReferenceCapacity*)
 ext 0273AF40 void releaseManagerSettings(void*) callback
 ext 0276B710 void applyManagerResourcePair(EnvironmentManager*,const EnvironmentArchiveReference*,const EnvironmentArchiveReference*,s32,f32)
*/

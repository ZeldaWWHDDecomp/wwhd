// Environment-set record reference callbacks and observed subobject adjustments.
#include "agl/EnvironmentSet.h"
#include "agl/EnvironmentManager.h"
using namespace agl::env;
static sead::SafeText sSetResetRecordName;
void moveSetResetRecordText(void*,const void*,u32,u32);
void refreshSetRecordIo(EnvironmentSetRecordIo_l*);
void dispatchSetRecordChange(EnvironmentSet*,EnvironmentSetRecordIo_l*);
u32 dispatchSetRecordReset(EnvironmentSet*);
void dispatchSetRecordRefresh(EnvironmentSet*);
/* 0276D9A8 */
void EnvironmentSet::recordChanged(EnvironmentSetRecordIo_l* changed) {
    EnvironmentSetRecord* first = mRecords.mData;
    EnvironmentSetRecord* end = first + u32(mRecords.mCount);
    for (EnvironmentSetRecord* record = first; gabi::ea(record) != gabi::ea(end); ++record) {
        if (gabi::ea(changed) != gabi::ea(&record->mIo)) continue;
        u32 index = (gabi::ea(record) - gabi::ea(first)) / sizeof(EnvironmentSetRecord);
        s32 referenceIndex = changed->mReferenceIndex;
        EnvironmentObject* object = NULL;
        if (referenceIndex >= 0) {
            EnvironmentManager* manager = mManager;
            u32 typeId = changed->mTypeId;
            u32 rangeCount = u32(manager->mReferences.mRanges.mCount);
            const EnvironmentReferenceRange* range = manager->mReferences.mRanges.mData;
            if (typeId < rangeCount) range += typeId;
            if (referenceIndex < s32(u32(range->mCount))) {
                u32 sourceIndex = u32(range->mFirst) + u32(referenceIndex);
                const EnvironmentReference* reference = NULL;
                if (sourceIndex < u32(manager->mReferences.mReferences.mCount))
                    reference = static_cast<EnvironmentReference*>(manager->mReferences.mReferences.mData) + sourceIndex;
                // Preserve the original zero-address read on an invalid reference extent.
                object = lowMemoryView(reference)->mObject;
            }
        }
        EnvironmentReference* destination = mReferences.mReferences.mData;
        if (index < u32(mReferences.mReferences.mCount)) destination += index;
        destination->mObject = object;
        return;
    }
}
/* 0276DAE4 */
void EnvironmentSetRecordOwnerView_l::recordChanged(EnvironmentSetRecordIo_l* record) {
    EnvironmentSet* owner = reinterpret_cast<EnvironmentSet*>(reinterpret_cast<u8*>(this) - offsetof(EnvironmentSet,mMethods));
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)dispatchSetRecordChange),owner,record);
}
/* 0276DAEC */
u32 EnvironmentSet::resetRecordBindings() {
    gabi::AutoFrame frame{gabi::current_function()};
    EnvironmentSetRecord* record = mRecords.mData;
    EnvironmentSetRecord* end = record + u32(mRecords.mCount);
    if (gabi::ea(record) != gabi::ea(end)) {
        sead::VtableSlot* assure = &sSetResetRecordName.mVtable->mAssureTermination;
        const be<u8>* source = sSetResetRecordName.mText;
        for (; gabi::ea(record) != gabi::ea(end); ++record) {
            record->mIo.mReferenceIndex = -1;
            be<u8>* destination = record->mIo.mName.mText;
            gabi::call_ptr<void>(assure->mFunction,&sSetResetRecordName);
            s32 length = 0;
            while (source[length] != 0) {
                if (++length > sead::kMaxTextLength) { length = 0; break; }
            }
            s32 capacity = record->mIo.mName.mCapacity;
            if (length >= capacity) length = capacity - 1;
            gabi::call_ptr<void>(assure->mFunction,&sSetResetRecordName);
            moveSetResetRecordText(destination,source,u32(length),0);
            destination[length] = 0;
        }
    }
    return 1;
}
/* 0276DBE4 */
u32 EnvironmentSetParameterObject::resetSetRecordBindings() {
    EnvironmentSet* owner = reinterpret_cast<EnvironmentSet*>(reinterpret_cast<u8*>(this) - offsetof(EnvironmentSet,mParameters));
    return gabi::tail_ptr<u32>(gabi::guest_fn((const void*)dispatchSetRecordReset),owner);
}
/* 0276DBEC */
void EnvironmentSet::refreshRecordBindings() {
    gabi::AutoFrame frame{gabi::current_function()};
    EnvironmentSetRecord* record = mRecords.mData;
    EnvironmentSetRecord* end = record + u32(mRecords.mCount);
    for (; gabi::ea(record) != gabi::ea(end); ++record) refreshSetRecordIo(&record->mIo);
}
/* 0276DC44 */
void EnvironmentSetParameterObject::refreshSetRecordBindings() {
    EnvironmentSet* owner = reinterpret_cast<EnvironmentSet*>(reinterpret_cast<u8*>(this) - offsetof(EnvironmentSet,mParameters));
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)dispatchSetRecordRefresh),owner);
}
/* WWHD glue
 data sSetResetRecordName 104A0CD8
 ext C0009988 void moveSetResetRecordText(void*,const void*,u32,u32)
 ext 027A80FC void refreshSetRecordIo(EnvironmentSetRecordIo_l*)
 ext 0276D9A8 void dispatchSetRecordChange(EnvironmentSet*,EnvironmentSetRecordIo_l*) callback
 ext 0276DAEC u32 dispatchSetRecordReset(EnvironmentSet*) callback
 ext 0276DBEC void dispatchSetRecordRefresh(EnvironmentSet*) callback
*/

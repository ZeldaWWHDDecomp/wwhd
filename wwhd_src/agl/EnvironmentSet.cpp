// Environment-set initialization, parent attachment, and record cleanup.
#include "agl/EnvironmentSet.h"
#include "agl/EnvironmentManager.h"
using namespace agl::env;
typedef be<u8> EnvironmentSetTextByte;
static sead::VtableSlot sEnvironmentSetInitialMethods;
static sead::VtableSlot sEnvironmentSetReferenceMethods;
static sead::VtableSlot sEnvironmentSetMethods;
static sead::VtableSlot sEnvironmentSetParameterMethods;
static sead::VtableSlot sEnvironmentSetIoMethods;
static sead::VtableSlot sEnvironmentSetTextParameterMethods;
static sead::VtableSlot sEnvironmentSetRecordMethods;
static sead::VtableSlot sEnvironmentSetParameterBaseMethods;
static sead::SafeTextVtable sEnvironmentSetBufferBaseMethods;
static sead::SafeTextVtable sEnvironmentSetBufferMethods;
static sead::SafeTextVtable sEnvironmentSetFixedTextMethods;
static sead::SafeTextVtable sEnvironmentSetTextMethods;
static EnvironmentSetTextByte sEnvironmentSetName;
void* allocateEnvironmentSetStorage(u32);
void constructSetReferences(EnvironmentReferenceList*);
void constructSetParameterObject(agl::parameter_tree::Node*);
void constructSetOwner(agl::parameter_tree::Value*);
void constructSetBinding(ParameterBinding*);
void processEnvironmentSetName(sead::VtableSlot*,const sead::SafeText*);
void attachEnvironmentSetRecord(EnvironmentSetRecordIo_l*,agl::HierarchyNode*);
void destroyEnvironmentSetRecordIo(EnvironmentSetRecordIo_l*,u32);
sead::RegistryHeap* environmentSetStorageHeap(void*,void*);
void bindEnvironmentSetParent(EnvironmentSet*,EnvironmentManager*);
void destroyEnvironmentSetReferences(EnvironmentReferenceList*,u32);
void releaseEnvironmentSet(void*);

/* 0276DC4C */
EnvironmentSet* EnvironmentSet::construct(EnvironmentSet* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<EnvironmentSet*>(allocateEnvironmentSetStorage(sizeof(EnvironmentSet)));
    if (object == NULL) return NULL;
    constructSetReferences(&object->mReferences);
    object->mMethods = &sEnvironmentSetInitialMethods;
    constructSetParameterObject(&object->mParameters);
    object->mReferences.mMethods = &sEnvironmentSetReferenceMethods;
    object->mMethods = &sEnvironmentSetMethods;
    object->mParameters.mTable = gabi::ea(&sEnvironmentSetParameterMethods);
    object->mIoMethods = &sEnvironmentSetIoMethods;
    EnvironmentTextParameter* name = &object->mName;
    if (gabi::ea(name) == 0) name = static_cast<EnvironmentTextParameter*>(allocateEnvironmentSetStorage(sizeof(EnvironmentTextParameter)));
    if (name != NULL) {
        constructSetBinding(&name->mBinding);
        name->mBinding.mMethods = &sEnvironmentSetTextParameterMethods;
        sead::SafeTextBuffer* value = &name->mValue;
        if (gabi::ea(value) == 0) value = static_cast<sead::SafeTextBuffer*>(allocateEnvironmentSetStorage(sizeof(sead::SafeTextBuffer)));
        if (value != NULL) {
            value->mText = name->mValue.mStorage;
            value->mVtable = &sEnvironmentSetBufferBaseMethods;
            value->mCapacity = 32;
            name->mValue.mStorage[31] = 0;
        }
        be<u8>* firstCharacter = name->mValue.mText;
        name->mValue.mVtable = &sEnvironmentSetBufferMethods;
        firstCharacter[0] = 0;
        name->mValue.mVtable = &sEnvironmentSetFixedTextMethods;
    }
    constructSetParameterObject(&object->mSecondaryParameters);
    object->mManager = NULL;
    constructSetOwner(&object->mOwner);
    EnvironmentSetRecordBuffer* records = &object->mRecords;
    if (gabi::ea(records) == 0) records = static_cast<EnvironmentSetRecordBuffer*>(allocateEnvironmentSetStorage(sizeof(EnvironmentSetRecordBuffer)));
    if (records != NULL) {
        records->mData = NULL;
        records->mCount = 0;
    }
    gabi::FrameLocal<sead::SafeText> debugName(8);
    debugName->mVtable = &sEnvironmentSetTextMethods;
    debugName->mText = &sEnvironmentSetName;
    processEnvironmentSetName(reinterpret_cast<sead::VtableSlot*>(&object->mIoMethods),debugName.get());
    return object;
}
/* 0276DDD0 */
void EnvironmentSet::bindManager(EnvironmentManager* manager) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = u32(mRecords.mCount);
    EnvironmentSetRecord* record = mRecords.mData;
    EnvironmentSetRecord* end = record + count;
    mManager = manager;
    while (gabi::ea(record) != gabi::ea(end)) {
        agl::HierarchyNode* parent = manager == NULL ? NULL : &manager->mParameters;
        attachEnvironmentSetRecord(&record->mIo,parent);
        ++record;
    }
}
/* 0276DE44 */
void EnvironmentSet::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    EnvironmentSetRecord* records = mRecords.mData;
    mMethods = &sEnvironmentSetMethods;
    mReferences.mMethods = &sEnvironmentSetReferenceMethods;
    mParameters.mTable = gabi::ea(&sEnvironmentSetParameterMethods);
    mIoMethods = &sEnvironmentSetIoMethods;
    if (records != NULL) {
        s32 count = mRecords.mCount;
        for (s32 index = 0; index < count; ++index) {
            EnvironmentSetRecord* record = records + index;
            if (gabi::ea(record) != 0) {
                destroyEnvironmentSetRecordIo(&record->mIo,0);
                record->mVtable = reinterpret_cast<agl::parameter_tree::ValueVtable*>(&sEnvironmentSetRecordMethods);
                count = mRecords.mCount;
                records = mRecords.mData;
            }
        }
        sead::RegistryHeap* heap = environmentSetStorageHeap(sRegistryManager,records);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,static_cast<EnvironmentSetRecord*>(mRecords.mData));
        mRecords.mCount = 0;
        mRecords.mData = NULL;
    }
    bindEnvironmentSetParent(this,NULL);
    mOwner.mVtable = reinterpret_cast<agl::parameter_tree::ValueVtable*>(&sEnvironmentSetRecordMethods);
    mSecondaryParameters.mTable = gabi::ea(&sEnvironmentSetParameterBaseMethods);
    mParameters.mTable = gabi::ea(&sEnvironmentSetParameterBaseMethods);
    destroyEnvironmentSetReferences(&mReferences,0);
    if (flags & 1) releaseEnvironmentSet(this);
}

void moveEnvironmentSetName(be<u8>*,const be<u8>*,u32,u32);
void refreshEnvironmentSetRecord(EnvironmentSetRecordIo_l*);
/* 0276E774 */
u32 EnvironmentSet::insertObject(EnvironmentObject* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) return 0;
    s32 offset = 0;
    for (;;) {
        u32 typeId = gabi::call_ptr<u32>(object->mMethods->mTypeId.mFunction,object);
        u32 rangeCount = u32(mReferences.mRanges.mCount);
        EnvironmentReferenceRange* range = mReferences.mRanges.mData;
        if (typeId < rangeCount) range += typeId;
        if (offset >= s32(u32(range->mCount))) return 0;
        typeId = gabi::call_ptr<u32>(object->mMethods->mTypeId.mFunction,object);
        rangeCount = u32(mReferences.mRanges.mCount);
        range = mReferences.mRanges.mData;
        if (typeId < rangeCount) range += typeId;
        u32 index = u32(range->mFirst) + u32(offset);
        u32 recordCount = u32(mRecords.mCount);
        EnvironmentSetRecord* record = mRecords.mData;
        if (index < recordCount) record += index;
        s32 referenceIndex = record->mIo.mReferenceIndex;
        if (referenceIndex < 0 && referenceIndex != -2) {
            u32 referenceCount = u32(mReferences.mReferences.mCount);
            EnvironmentReference* reference = mReferences.mReferences.mData;
            if (index < referenceCount) reference += index;
            reference->mObject = object;
            sead::SafeText* name = gabi::call_ptr<sead::SafeText*>(object->mMethods->mGroupText.mFunction,object);
            be<u8>* destination = record->mIo.mName.mText;
            name->assureTermination();
            const be<u8>* text = name->mText;
            s32 length = 0;
            while (text[length] != 0) {
                if (++length > sead::kMaxTextLength) { length = 0; break; }
            }
            s32 capacity = record->mIo.mName.mCapacity;
            if (length >= capacity) length = capacity - 1;
            name->assureTermination();
            moveEnvironmentSetName(destination,name->mText,u32(length),0);
            destination[length] = 0;
            refreshEnvironmentSetRecord(&record->mIo);
            return 1;
        }
        ++offset;
    }
}

static sead::SafeText sExpectedEnvironmentRecordName;
static EnvironmentSetTextByte sEnvironmentRecordTypeKey;
u32 hashEnvironmentRecordTypeKey(const sead::SafeText*);
/* inlined resource-label comparison */
static bool sameEnvironmentRecordText(const be<u8>* left,const be<u8>* right) {
    for (u32 index = 0; index <= u32(sead::kMaxTextLength); ++index) {
        u32 character = left[index];
        if (character != u32(right[index])) return false;
        if (character == 0) return true;
    }
    return false;
}
/* 0276E95C */
u32 EnvironmentSetRecord::matchesResource(const EnvironmentSetResourceReference* resource) const {
    gabi::AutoFrame frame{gabi::current_function()};
    EnvironmentObjectRecord* object = resource->mObject;
    sead::SafeTextBuffer* name = const_cast<sead::SafeTextBuffer*>(&mIo.mName);
    name->assureTermination();
    name->assureTermination();
    const be<u8>* originalName = name->mText;
    sExpectedEnvironmentRecordName.assureTermination();
    if (gabi::ea(originalName) != gabi::ea(static_cast<be<u8>*>(sExpectedEnvironmentRecordName.mText))) {
        if (!sameEnvironmentRecordText(name->mText,sExpectedEnvironmentRecordName.mText)) return 0;
    }
    gabi::FrameLocal<sead::SafeText> typeKey(0x10);
    typeKey->mText = &sEnvironmentRecordTypeKey;
    typeKey->mVtable = &sEnvironmentSetTextMethods;
    u32 nameHash = hashEnvironmentRecordTypeKey(typeKey.get());
    u32 entryCount = object->mParameterCount;
    EnvironmentSetResourceEntry* first = object->firstParameter();
    EnvironmentSetResourceEntry* entry = first;
    u32 foundIndex = 0xFFFFFFFF;
    for (u32 index = 0; index != entryCount; ++index) {
        if (nameHash == u32(entry->mNameHash)) { foundIndex = index; break; }
        entry = entry->next();
    }
    entry = first;
    // The original requires the type entry to exist and walks its relative-offset chain again.
    for (u32 index = 0; index != foundIndex; ++index) {
        entry = entry->next();
    }
    gabi::FrameLocal<sead::SafeText> resourceType(8);
    resourceType->mText = entry->text();
    resourceType->mVtable = &sEnvironmentSetTextMethods;
    sead::SafeText* typeName = const_cast<sead::FixedSafeText<32>*>(&mIo.mTypeName.mValue);
    typeName->assureTermination();
    typeName->assureTermination();
    const be<u8>* originalType = typeName->mText;
    resourceType->assureTermination();
    if (gabi::ea(originalType) == gabi::ea(static_cast<be<u8>*>(resourceType->mText))) return 1;
    return sameEnvironmentRecordText(typeName->mText,resourceType->mText) ? 1 : 0;
}

/* WWHD glue
 data sExpectedEnvironmentRecordName 104A0CD8
 data sEnvironmentRecordTypeKey 10160350
 ext 0279E57C u32 hashEnvironmentRecordTypeKey(const sead::SafeText*)
 ext C0009988 void moveEnvironmentSetName(be<u8>*,const be<u8>*,u32,u32)
 ext 027A80FC void refreshEnvironmentSetRecord(EnvironmentSetRecordIo_l*)
 data sEnvironmentSetInitialMethods 101601DC
 data sEnvironmentSetReferenceMethods 10160358
 data sEnvironmentSetMethods 101603A0
 data sEnvironmentSetParameterMethods 101603B8
 data sEnvironmentSetIoMethods 101603F8
 data sEnvironmentSetTextParameterMethods 1016027C
 data sEnvironmentSetRecordMethods 1016019C
 data sEnvironmentSetParameterBaseMethods 101601F4
 data sEnvironmentSetBufferBaseMethods 10160134
 data sEnvironmentSetBufferMethods 1016016C
 data sEnvironmentSetFixedTextMethods 10160184
 data sEnvironmentSetTextMethods 1016011C
 data sEnvironmentSetName 101602F4
 ext 0273AD10 void* allocateEnvironmentSetStorage(u32)
 ext 0276F3A8 void constructSetReferences(EnvironmentReferenceList*)
 ext 0279CA34 void constructSetParameterObject(agl::parameter_tree::Node*)
 ext 0279E674 void constructSetBinding(ParameterBinding*)
 ext 027A84B8 void constructSetOwner(agl::parameter_tree::Value*)
 ext 02785CB4 void processEnvironmentSetName(sead::VtableSlot*,const sead::SafeText*)
 ext 027A7FF4 void attachEnvironmentSetRecord(EnvironmentSetRecordIo_l*,agl::HierarchyNode*)
 ext 027A8084 void destroyEnvironmentSetRecordIo(EnvironmentSetRecordIo_l*,u32)
 ext 02755FEC sead::RegistryHeap* environmentSetStorageHeap(void*,void*)
 ext 0276DDD0 void bindEnvironmentSetParent(EnvironmentSet*,EnvironmentManager*)
 ext 0276F4F8 void destroyEnvironmentSetReferences(EnvironmentReferenceList*,u32)
 ext 0273AF40 void releaseEnvironmentSet(void*)
*/

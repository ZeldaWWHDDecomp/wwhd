// Environment-set record construction and parameter registration from the local initializer.
#include "agl/EnvironmentSet.h"
using namespace agl::env;
typedef be<u8> SetInitializationTextByte;
typedef be<s32> SetInitializationClassCount;
static SetInitializationClassCount sSetInitializationClassCount;
static EnvironmentClassEntry sSetInitializationClassEntries;
static sead::SafeText sSetInitializationRecordName;
static sead::VtableSlot sSetInitializationRecordMethods;
static sead::VtableSlot sSetInitializationIoBindingMethods;
static sead::VtableSlot sSetInitializationTextBindingMethods;
static sead::SafeTextVtable sSetInitializationTextMethods;
static sead::SafeTextVtable sSetInitializationBufferBaseMethods;
static sead::SafeTextVtable sSetInitializationBufferMethods;
static sead::SafeTextVtable sSetInitializationFixedMethods;
static SetInitializationTextByte sSetInitializationDefaultName;
static SetInitializationTextByte sSetInitializationNameKey;
static SetInitializationTextByte sSetInitializationNameDescription;
static SetInitializationTextByte sSetInitializationTypeKey;
static SetInitializationTextByte sSetInitializationTypeDescription;
static SetInitializationTextByte sSetInitializationEmptyText;
static SetInitializationTextByte sSetInitializationRecordFormat;
static SetInitializationTextByte sSetInitializationOwnerName;
static SetInitializationTextByte sSetInitializationObjectsName;
void initializeSetReferenceArrays(EnvironmentReferenceList*,const EnvironmentReferenceCapacity*,sead::RegistryHeap*);
sead::RegistryHeap* currentSetInitializationHeap(void*);
void constructSetRecordOwner(agl::parameter_tree::Value*);
void constructSetRecordIo(EnvironmentSetRecordIo_l*);
void constructSetRecordTextBinding(ParameterBinding*);
void* allocateSetInitializationStorage(u32);
void prepareSetInitializationText(const sead::SafeText*);
void moveSetInitializationText(void*,const void*,u32,u32);
void attachSetInitializationBinding(ParameterBinding*,const sead::SafeText*,const sead::SafeText*,const sead::SafeText*,void*,u32);
s32 classForSetInitializationReference(const EnvironmentReferenceList*,s32);
sead::SafeText* formatSetInitializationRecord(sead::FixedSafeText<1024>*,const be<u8>*,u32);
void registerSetInitializationObject(agl::parameter_tree::Node*,agl::parameter_tree::Value*,const sead::SafeText*);
void registerSetInitializationList(agl::parameter_tree::Node*,agl::parameter_tree::Node*,const sead::SafeText*);

/* inlined text operations; callbacks can change pointers and capacities */
static s32 setInitializationLength(const be<u8>* text) {
    s32 length = 0;
    while (text[length] != 0) {
        if (++length > sead::kMaxTextLength) return 0;
    }
    return length;
}
static void copySetInitializationText(be<u8>* destination,const be<s32>* capacity,sead::SafeText* source) {
    source->assureTermination();
    s32 length = setInitializationLength(source->mText);
    s32 maximum = *capacity;
    if (length >= maximum) length = maximum - 1;
    source->assureTermination();
    moveSetInitializationText(destination,source->mText,u32(length),0);
    destination[length] = 0;
}
static void prepareSetInitializationBuffer(sead::FixedSafeText<32>* buffer) {
    buffer->mStorage[31] = 0;
    buffer->mVtable = &sSetInitializationBufferMethods;
    buffer->mText = buffer->mStorage;
    buffer->mCapacity = 32;
}
static void copyPreparedSetInitializationText(sead::FixedSafeText<32>* destination,sead::SafeText* source) {
    prepareSetInitializationText(source);
    s32 length = setInitializationLength(source->mText);
    s32 maximum = destination->mCapacity;
    if (length >= maximum) length = maximum - 1;
    source->assureTermination();
    moveSetInitializationText(destination->mStorage,source->mText,u32(length),0);
    destination->mStorage[length] = 0;
    destination->mVtable = &sSetInitializationFixedMethods;
}
/* 0276DF64 */
void EnvironmentSet::initialize(const EnvironmentReferenceCapacity* capacity,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    initializeSetReferenceArrays(&mReferences,capacity,heap);
    s32 totalCount = s32(u32(capacity->mTotalCount));
    if (totalCount > 0) {
        if (heap == NULL) heap = currentSetInitializationHeap(sRegistryManager);
        EnvironmentSetRecord* storage = gabi::call_ptr<EnvironmentSetRecord*>(heap->dispatch()->mAllocate.mFunction,
            heap,u32(totalCount) * u32(sizeof(EnvironmentSetRecord)),4);
        u32 constructionCount = totalCount > 0 ? u32(totalCount) : 1;
        for (u32 index = 0; index < constructionCount; ++index) {
            EnvironmentSetRecord* record = storage + index;
            if (gabi::ea(record) == 0) continue;
            constructSetRecordOwner(record);
            record->mVtable = reinterpret_cast<agl::parameter_tree::ValueVtable*>(&sSetInitializationRecordMethods);
            constructSetRecordIo(&record->mIo);
            record->mIo.mBinding.mMethods = &sSetInitializationIoBindingMethods;
            record->mIo.mTypeId = 0;
            EnvironmentTextParameter* typeName = &record->mIo.mTypeName;
            constructSetRecordTextBinding(&typeName->mBinding);
            typeName->mBinding.mMethods = &sSetInitializationTextBindingMethods;
            sead::SafeTextBuffer* value = &typeName->mValue;
            if (gabi::ea(value) == 0) value = static_cast<sead::SafeTextBuffer*>(allocateSetInitializationStorage(sizeof(sead::SafeTextBuffer)));
            if (value != NULL) {
                value->mVtable = &sSetInitializationBufferBaseMethods;
                value->mCapacity = 32;
                value->mText = typeName->mValue.mStorage;
                typeName->mValue.mStorage[31] = 0;
            }
            be<u8>* firstCharacter = typeName->mValue.mText;
            typeName->mValue.mVtable = &sSetInitializationBufferMethods;
            firstCharacter[0] = 0;
            typeName->mValue.mVtable = &sSetInitializationFixedMethods;
        }
        if (storage != NULL) {
            mRecords.mData = storage;
            mRecords.mCount = totalCount;
        }
    }
    s32 classCount = sSetInitializationClassCount;
    u32 recordIndex = 0;
    for (s32 typeId = 0; typeId < classCount; ++typeId) {
        for (s32 offset = 0;; ++offset) {
            u32 rangeCount = u32(mReferences.mRanges.mCount);
            EnvironmentReferenceRange* range = mReferences.mRanges.mData;
            if (u32(typeId) < rangeCount) range += typeId;
            if (offset >= s32(u32(range->mCount))) break;
            EnvironmentSetRecord* record = mRecords.mData;
            if (recordIndex < u32(mRecords.mCount)) record += recordIndex;
            ++recordIndex;
            record->mIo.mTypeId = u32(typeId);
            record->mIo.mSetOwner = &mMethods;
        }
    }
    gabi::FrameLocal<sead::SafeText> defaultName(0x74);
    defaultName->mText = &sSetInitializationDefaultName;
    defaultName->mVtable = &sSetInitializationTextMethods;
    gabi::FrameLocal<sead::FixedSafeText<32>> setName(0x8C);
    prepareSetInitializationBuffer(setName.get());
    copyPreparedSetInitializationText(setName.get(),defaultName.get());
    gabi::FrameLocal<sead::SafeText> nameKey(0x7C);
    nameKey->mText = &sSetInitializationNameKey;
    nameKey->mVtable = &sSetInitializationTextMethods;
    gabi::FrameLocal<sead::SafeText> description(0x6C);
    description->mText = &sSetInitializationNameDescription;
    description->mVtable = &sSetInitializationTextMethods;
    gabi::FrameLocal<sead::SafeText> hint(0x84);
    hint->mText = &sSetInitializationEmptyText;
    hint->mVtable = &sSetInitializationTextMethods;
    attachSetInitializationBinding(&mName.mBinding,nameKey.get(),description.get(),hint.get(),&mOwner,0);
    copySetInitializationText(mName.mValue.mText,&mName.mValue.mCapacity,setName.get());
    EnvironmentSetRecord* first = mRecords.mData;
    EnvironmentSetRecord* end = first + u32(mRecords.mCount);
    if (gabi::ea(first) != gabi::ea(end)) {
        sead::VtableSlot* recordNameAssure = &sSetInitializationRecordName.mVtable->mAssureTermination;
        const be<u8>* recordNameText = sSetInitializationRecordName.mText;
        for (EnvironmentSetRecord* record = first; gabi::ea(record) != gabi::ea(end); ++record) {
            u32 index = (gabi::ea(record) - gabi::ea(first)) / u32(sizeof(EnvironmentSetRecord));
            s32 typeId = classForSetInitializationReference(&mReferences,s32(index));
            EnvironmentClassEntry* entry = (&sSetInitializationClassEntries) + u32(typeId);
            gabi::FrameLocal<sead::SafeText> typeName(0x28);
            typeName->mText = entry->mName;
            typeName->mVtable = &sSetInitializationTextMethods;
            gabi::FrameLocal<sead::FixedSafeText<32>> temporary(0x30);
            prepareSetInitializationBuffer(temporary.get());
            copyPreparedSetInitializationText(temporary.get(),typeName.get());
            gabi::FrameLocal<sead::SafeText> typeKey(0x18);
            typeKey->mText = &sSetInitializationTypeKey;
            typeKey->mVtable = &sSetInitializationTextMethods;
            gabi::FrameLocal<sead::SafeText> typeDescription(0x20);
            typeDescription->mText = &sSetInitializationTypeDescription;
            typeDescription->mVtable = &sSetInitializationTextMethods;
            gabi::FrameLocal<sead::SafeText> typeHint(0x5C);
            typeHint->mText = &sSetInitializationEmptyText;
            typeHint->mVtable = &sSetInitializationTextMethods;
            attachSetInitializationBinding(&record->mIo.mTypeName.mBinding,typeKey.get(),typeDescription.get(),typeHint.get(),record,0);
            copySetInitializationText(record->mIo.mTypeName.mValue.mText,&record->mIo.mTypeName.mValue.mCapacity,temporary.get());
            prepareSetInitializationBuffer(temporary.get());
            gabi::call_ptr<void>(recordNameAssure->mFunction,&sSetInitializationRecordName);
            s32 length = setInitializationLength(recordNameText);
            s32 maximum = temporary->mCapacity;
            if (length >= maximum) length = maximum - 1;
            gabi::call_ptr<void>(recordNameAssure->mFunction,&sSetInitializationRecordName);
            moveSetInitializationText(temporary->mStorage,recordNameText,u32(length),0);
            temporary->mStorage[length] = 0;
            temporary->mVtable = &sSetInitializationFixedMethods;
            typeKey->mText = &sSetInitializationNameDescription;
            typeKey->mVtable = &sSetInitializationTextMethods;
            typeDescription->mText = &sSetInitializationNameKey;
            typeDescription->mVtable = &sSetInitializationTextMethods;
            gabi::FrameLocal<sead::SafeText> nameHint(0x64);
            nameHint->mText = &sSetInitializationEmptyText;
            nameHint->mVtable = &sSetInitializationTextMethods;
            attachSetInitializationBinding(&record->mIo.mBinding,typeDescription.get(),typeKey.get(),nameHint.get(),record,0);
            copySetInitializationText(record->mIo.mName.mText,&record->mIo.mName.mCapacity,temporary.get());
            gabi::FrameLocal<sead::FixedSafeText<1024>> recordLabel(0xC8);
            gabi::cpu->cr[6] = 0;
            sead::SafeText* label = formatSetInitializationRecord(recordLabel.get(),&sSetInitializationRecordFormat,index);
            registerSetInitializationObject(&mSecondaryParameters,record,label);
        }
    }
    description->mText = &sSetInitializationOwnerName;
    description->mVtable = &sSetInitializationTextMethods;
    registerSetInitializationObject(&mParameters,&mOwner,description.get());
    description->mText = &sSetInitializationObjectsName;
    description->mVtable = &sSetInitializationTextMethods;
    registerSetInitializationList(&mParameters,&mSecondaryParameters,description.get());
}

/* WWHD glue
 data sSetInitializationClassCount 101F8BF4
 data sSetInitializationClassEntries 104A1564
 data sSetInitializationRecordName 104A0CD8
 data sSetInitializationRecordMethods 10160408
 data sSetInitializationIoBindingMethods 10160B14
 data sSetInitializationTextBindingMethods 1016027C
 data sSetInitializationTextMethods 1016011C
 data sSetInitializationBufferBaseMethods 10160134
 data sSetInitializationBufferMethods 1016016C
 data sSetInitializationFixedMethods 10160184
 data sSetInitializationDefaultName 10160310
 data sSetInitializationNameKey 10160318
 data sSetInitializationNameDescription 10160320
 data sSetInitializationTypeKey 10160328
 data sSetInitializationTypeDescription 10160330
 data sSetInitializationEmptyText 10160118
 data sSetInitializationRecordFormat 10160338
 data sSetInitializationOwnerName 10160308
 data sSetInitializationObjectsName 1016033C
 ext 0276F554 void initializeSetReferenceArrays(EnvironmentReferenceList*,const EnvironmentReferenceCapacity*,sead::RegistryHeap*)
 ext 02756140 sead::RegistryHeap* currentSetInitializationHeap(void*)
 ext 027A84B8 void constructSetRecordOwner(agl::parameter_tree::Value*)
 ext 027A7E6C void constructSetRecordIo(EnvironmentSetRecordIo_l*)
 ext 0279E674 void constructSetRecordTextBinding(ParameterBinding*)
 ext 0273AD10 void* allocateSetInitializationStorage(u32)
 ext 0276EBB4 void prepareSetInitializationText(const sead::SafeText*)
 ext C0009988 void moveSetInitializationText(void*,const void*,u32,u32)
 ext 0279E618 void attachSetInitializationBinding(ParameterBinding*,const sead::SafeText*,const sead::SafeText*,const sead::SafeText*,void*,u32)
 ext 0276F8B4 s32 classForSetInitializationReference(const EnvironmentReferenceList*,s32)
 ext 0276EC20 sead::SafeText* formatSetInitializationRecord(sead::FixedSafeText<1024>*,const be<u8>*,u32)
 ext 0279CB9C void registerSetInitializationObject(agl::parameter_tree::Node*,agl::parameter_tree::Value*,const sead::SafeText*)
 ext 0279CB24 void registerSetInitializationList(agl::parameter_tree::Node*,agl::parameter_tree::Node*,const sead::SafeText*)
*/

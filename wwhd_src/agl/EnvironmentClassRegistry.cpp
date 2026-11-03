// Class registration uses category order, then a bounded bytewise name comparison.
#include "agl/EnvironmentClassRegistry.h"
#include "agl/EnvironmentReferences.h"
using namespace agl::env;
static s16 sNextEnvironmentClassId;
static s32 sEnvironmentClassCount;
// These two symbols are address views of the first records, not one-element arrays.
static EnvironmentClassEntry sEnvironmentClassEntries;
static EnvironmentClassDescriptor sEnvironmentClassIds;
static sead::SafeTextVtable sEnvironmentClassTextMethods;
void prepareEnvironmentClassText(const sead::SafeText*);

/* inlined into EnvironmentClassRegistry::registerClass */
static EnvironmentClassEntry* environmentClassEntry(u32 index) {
    return &(&sEnvironmentClassEntries)[index];
}
/* inlined into EnvironmentClassRegistry::registerClass */
static EnvironmentClassDescriptor* environmentClassId(u32 index) {
    return &(&sEnvironmentClassIds)[index];
}
/* inlined into EnvironmentClassRegistry::registerClass */
static bool environmentClassNameAfter(const be<u8>* left,const be<u8>* right) {
    if (gabi::ea(left) == gabi::ea(right)) return false;
    for (u32 count = 0; count < u32(sead::kMaxTextLength); ++count) {
        u32 leftCharacter = *left;
        if (leftCharacter == 0) return false;
        u32 rightCharacter = *right;
        if (rightCharacter == 0 || leftCharacter > rightCharacter) return true;
        if (leftCharacter < rightCharacter) return false;
        ++left;
        ++right;
    }
    return false;
}

/* 02770250 */
EnvironmentClassDescriptor* EnvironmentClassRegistry::registerClass(const sead::SafeText* name,const sead::SafeText* description,u32 factory,s32 category) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 id = sNextEnvironmentClassId;
    sNextEnvironmentClassId = s16(id + 1);
    sEnvironmentClassCount = s16(id + 1);
    EnvironmentClassEntry* entry = environmentClassEntry(u32(id));
    const_cast<sead::SafeText*>(name)->assureTermination();
    entry->mName = name->mText;
    const_cast<sead::SafeText*>(description)->assureTermination();
    be<u8>* descriptionCharacters = description->mText;
    entry->mFactory = factory;
    entry->mDescription = descriptionCharacters;
    entry->mCategory = category;
    EnvironmentClassDescriptor* registeredId = environmentClassId(u32(id));
    registeredId->mTypeId = s16(id);
    entry->mId = u32(id);
    for (s32 position = id; position > 0; --position) {
        EnvironmentClassEntry* left = environmentClassEntry(u32(position - 1));
        EnvironmentClassEntry* right = environmentClassEntry(u32(position));
        s32 rightCategory = right->mCategory;
        s32 leftCategory = left->mCategory;
        bool swapEntries = leftCategory > rightCategory;
        if (leftCategory == rightCategory) {
            gabi::FrameLocal<sead::SafeText> rightName(8),leftName(16);
            rightName->mText = right->mName;
            leftName->mText = left->mName;
            leftName->mVtable = &sEnvironmentClassTextMethods;
            rightName->mVtable = &sEnvironmentClassTextMethods;
            prepareEnvironmentClassText(rightName.get());
            rightName->assureTermination();
            const be<u8>* rightCharacters = rightName->mText;
            leftName->assureTermination();
            swapEntries = environmentClassNameAfter(leftName->mText,rightCharacters);
        }
        if (!swapEntries) break;
        u32 rightId = right->mId;
        be<u8>* leftName = left->mName;
        s32 rightIndex = environmentClassId(rightId)->mTypeId;
        left->mName = right->mName;
        rightCategory = right->mCategory;
        u32 leftId = left->mId;
        left->mId = rightId;
        u32 leftFactory = left->mFactory;
        environmentClassId(rightId)->mTypeId = s16(rightIndex - 1);
        s32 leftIndex = environmentClassId(leftId)->mTypeId;
        u32 rightFactory = right->mFactory;
        leftCategory = left->mCategory;
        left->mFactory = rightFactory;
        right->mCategory = leftCategory;
        left->mCategory = rightCategory;
        right->mName = leftName;
        right->mFactory = leftFactory;
        be<u8>* rightDescription = right->mDescription;
        be<u8>* leftDescription = left->mDescription;
        left->mDescription = rightDescription;
        right->mId = leftId;
        environmentClassId(leftId)->mTypeId = s16(leftIndex + 1);
        right->mDescription = leftDescription;
    }
    return registeredId;
}


typedef be<u8> EnvironmentClassFormatByte;
static EnvironmentClassFormatByte sEnvironmentClassGroupFormat;
void formatEnvironmentGroupName(sead::SafeTextBuffer*,const be<u8>*,const be<u8>*,u32);

/* 027701D4 */
void EnvironmentObject::refreshGroupName() {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 typeId = gabi::call_ptr<s32>(mMethods->mTypeId.mFunction,this);
    gabi::FrameLocal<sead::SafeText> className(8);
    className->mVtable = &sEnvironmentClassTextMethods;
    className->mText = environmentClassEntry(u32(typeId))->mName;
    prepareEnvironmentClassText(className.get());
    const be<u8>* characters = className->mText;
    u32 objectIndex = mObjectIndex;
    // GHS marks this integer-only variadic call by clearing cr1.eq.
    gabi::cpu->cr[6] = 0;
    formatEnvironmentGroupName(&mGroup.mValue,&sEnvironmentClassGroupFormat,characters,objectIndex);
}


typedef gptr<be<u8>> EnvironmentCategoryText;
static EnvironmentCategoryText sEnvironmentCategoryNames;
static EnvironmentClassFormatByte sEnvironmentIoNameFormat;
sead::SafeTextBuffer* formatEnvironmentIoName(sead::FixedSafeText<1024>*,const be<u8>*,const be<u8>*);
void setEnvironmentIoName(sead::VtableSlot*,const sead::SafeText*);
void resetInitializedEnvironment(EnvironmentObject*);

/* 02770564 */
void EnvironmentObject::initialize(u32 objectIndex,void* creationArgument,EnvironmentManagerState* manager,void* userArgument) {
    gabi::AutoFrame frame{gabi::current_function()};
    EnvironmentObjectMethods* methods = mMethods;
    mManager = manager;
    mObjectIndex = u16(objectIndex);
    gabi::call_ptr<void>(methods->mGroupText.mFunction,this);
    s32 typeId = gabi::call_ptr<s32>(mMethods->mTypeId.mFunction,this);
    u32 category = u32(environmentClassEntry(u32(typeId))->mCategory);
    const be<u8>* categoryName = (&sEnvironmentCategoryNames)[category];
    gabi::FrameLocal<sead::FixedSafeText<1024>> ioName(8);
    gabi::cpu->cr[6] = 0;
    const sead::SafeText* formatted = formatEnvironmentIoName(ioName.get(),&sEnvironmentIoNameFormat,categoryName);
    setEnvironmentIoName(reinterpret_cast<sead::VtableSlot*>(&mParameterIoMethods),formatted);
    gabi::call_ptr<void>(mMethods->mPostInitialize.mFunction,this,creationArgument,userArgument);
    resetInitializedEnvironment(this);
}


sead::RegistryHeap* currentEnvironmentReferenceHeap(void*);

/* 0276F554 */
void EnvironmentReferenceList::initialize(const EnvironmentReferenceCapacity* capacity,sead::RegistryHeap* requestedHeap) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 referenceCount = s32(u32(capacity->mTotalCount));
    if (referenceCount > 0) {
        sead::RegistryHeap* heap = requestedHeap;
        if (heap == NULL) heap = currentEnvironmentReferenceHeap(sRegistryManager);
        EnvironmentReference* references = gabi::call_ptr<EnvironmentReference*>(heap->dispatch()->mAllocate.mFunction,heap,u32(referenceCount) * sizeof(EnvironmentReference),s32(4));
        if (references != NULL) {
            mReferences.mData = references;
            mReferences.mCount = referenceCount;
        }
    }
    s32 classCount = sEnvironmentClassCount;
    EnvironmentReferenceRange* ranges;
    if (classCount > 0) {
        sead::RegistryHeap* heap = requestedHeap;
        if (heap == NULL) heap = currentEnvironmentReferenceHeap(sRegistryManager);
        ranges = gabi::call_ptr<EnvironmentReferenceRange*>(heap->dispatch()->mAllocate.mFunction,heap,u32(classCount) * sizeof(EnvironmentReferenceRange),s32(4));
        if (ranges != NULL) {
            mRanges.mCount = classCount;
            mRanges.mData = ranges;
        } else ranges = mRanges.mData;
    } else ranges = mRanges.mData;
    EnvironmentReferenceRange* rangesEnd = sead::elementPtr(ranges, u32(mRanges.mCount));  // 32-bit end and steps, as the original's `cmplw p,end`
    u32 first = 0;
    for (EnvironmentReferenceRange* range = ranges; range != rangesEnd; range = sead::nextElement(range)) {
        u32 index = (gabi::ea(range) - gabi::ea(ranges)) / u32(sizeof(EnvironmentReferenceRange));
        range->mFirst = u16(first);
        u32 count = capacity->mCounts[index < 128 ? index : 0];
        range->mCount = u16(count);
        first += u16(count);
    }
    EnvironmentReference* reference = mReferences.mData;
    EnvironmentReference* referencesEnd = sead::elementPtr(reference, u32(mReferences.mCount));
    for (; reference != referencesEnd; reference = sead::nextElement(reference)) reference->mObject = NULL;
}


/* 0276F8B4 */
s32 EnvironmentReferenceList::classForReference(s32 referenceIndex) const {
    s32 classCount = sEnvironmentClassCount;
    if (classCount <= 0) return -1;
    u32 rangeCount = u32(mRanges.mCount);
    const EnvironmentReferenceRange* ranges = mRanges.mData;
    for (u32 typeId = 0; typeId < u32(classCount); ++typeId) {
        const EnvironmentReferenceRange* range = typeId < rangeCount ? ranges + typeId : ranges;
        s32 first = range->mFirst;
        if (first > referenceIndex) continue;
        s32 count = range->mCount;
        if (referenceIndex < first + count) return s32(typeId);
    }
    return -1;
}

/* WWHD glue
 ext 02756140 sead::RegistryHeap* currentEnvironmentReferenceHeap(void*)
 data sEnvironmentCategoryNames 101F8BE4
 data sEnvironmentIoNameFormat 101609E4
 ext 0277077C sead::SafeTextBuffer* formatEnvironmentIoName(sead::FixedSafeText<1024>*,const be<u8>*,const be<u8>*)
 ext 02785CB4 void setEnvironmentIoName(sead::VtableSlot*,const sead::SafeText*)
 ext 02770480 void resetInitializedEnvironment(EnvironmentObject*)
 data sEnvironmentClassGroupFormat 101609DC
 ext 02759C28 void formatEnvironmentGroupName(sead::SafeTextBuffer*,const be<u8>*,const be<u8>*,u32)
 data sNextEnvironmentClassId 101F8BE0
 data sEnvironmentClassCount 101F8BF4
 data sEnvironmentClassEntries 104A1564
 data sEnvironmentClassIds 104A1464
 data sEnvironmentClassTextMethods 101607DC
 ext 02770710 void prepareEnvironmentClassText(const sead::SafeText*)
*/

// Owned reference arrays and the original asymmetric name-ordering callback.

// settings (128 per-class counts, total, two settings words with their defaults), each checked against this game's
// own constructors of both.
#include "agl/EnvironmentReferences.h"
#include "sead/ElementAt.h"
using namespace agl::env;
static sead::VtableSlot sEnvironmentReferenceMethods;
static sead::VtableSlot sEnvironmentCapacityMethods;
void* allocateEnvironmentReferenceStorage(u32);
sead::RegistryHeap* environmentStorageHeap(void*,void*);
void clearEnvironmentReferenceBuffers(EnvironmentReferenceList*);
void releaseEnvironmentReferenceList(void*);

/* 0276F3A8 */
EnvironmentReferenceList* EnvironmentReferenceList::construct(EnvironmentReferenceList* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<EnvironmentReferenceList*>(allocateEnvironmentReferenceStorage(sizeof(EnvironmentReferenceList)));
    if (object == NULL) return NULL;
    object->mOwner = NULL;
    object->mReserved = 0;
    object->mMethods = &sEnvironmentReferenceMethods;
    EnvironmentRangeBuffer* ranges = &object->mRanges;
    if (gabi::ea(ranges) == 0) ranges = static_cast<EnvironmentRangeBuffer*>(allocateEnvironmentReferenceStorage(sizeof(EnvironmentRangeBuffer)));
    if (ranges != NULL) {
        ranges->mData = NULL;
        ranges->mCount = 0;
    }
    EnvironmentReferenceBuffer* references = &object->mReferences;
    if (gabi::ea(references) == 0) references = static_cast<EnvironmentReferenceBuffer*>(allocateEnvironmentReferenceStorage(sizeof(EnvironmentReferenceBuffer)));
    if (references != NULL) {
        references->mData = NULL;
        references->mCount = 0;
    }
    return object;
}
/* 0276F448 */
void EnvironmentReferenceList::clear() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mReferences.mData != NULL) {
        sead::RegistryHeap* heap = environmentStorageHeap(sRegistryManager,mReferences.mData);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,static_cast<EnvironmentReference*>(mReferences.mData));
        mReferences.mCount = 0;
        mReferences.mData = NULL;
    }
    if (mRanges.mData != NULL) {
        sead::RegistryHeap* heap = environmentStorageHeap(sRegistryManager,mRanges.mData);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,static_cast<EnvironmentReferenceRange*>(mRanges.mData));
        mRanges.mCount = 0;
        mRanges.mData = NULL;
    }
}
/* 0276F4F8 */
void EnvironmentReferenceList::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mMethods = &sEnvironmentReferenceMethods;
    clearEnvironmentReferenceBuffers(this);
    if (flags & 1) releaseEnvironmentReferenceList(this);
}
/* 0276F968 */
EnvironmentReferenceCapacity* EnvironmentReferenceCapacity::construct(EnvironmentReferenceCapacity* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<EnvironmentReferenceCapacity*>(allocateEnvironmentReferenceStorage(sizeof(EnvironmentReferenceCapacity)));
    if (object == NULL) return NULL;
    object->mMethods = &sEnvironmentCapacityMethods;
    object->mTotalCount = 0;
    for (u32 index = 0; index < 128; ++index) object->mCounts[index] = 0;
    return object;
}
/* 0276F9F0 */
void EnvironmentReferenceCapacity::set(u32 typeId,u32 count) {
    u32 index = typeId < 128 ? typeId : 0;
    mTotalCount = u32(mTotalCount) - u32(mCounts[index]);
    mCounts[index] = count;
    mTotalCount = u32(mTotalCount) + count;
}
/* 0276F6DC */
s32 EnvironmentReferenceList::compareReferences(const EnvironmentReference* left,const EnvironmentReference* right) {
    gabi::AutoFrame frame{gabi::current_function()};
    EnvironmentObject* leftObject = left->mObject;
    if (leftObject == NULL) return 1;
    EnvironmentObject* rightObject = right->mObject;
    if (rightObject == NULL) return -1;
    EnvironmentObjectMethods* rightMethods = rightObject->mMethods;
    sead::SafeText* leftName = gabi::call_ptr<sead::SafeText*>(leftObject->mMethods->mGroupText.mFunction,leftObject);
    sead::SafeText* rightName = gabi::call_ptr<sead::SafeText*>(rightMethods->mGroupText.mFunction,static_cast<EnvironmentObject*>(right->mObject));
    leftName->assureTermination();
    leftName->assureTermination();
    const be<u8>* leftCharacters = leftName->mText;
    rightName->assureTermination();
    if (gabi::ea(leftCharacters) == gabi::ea(static_cast<be<u8>*>(rightName->mText))) return 1;
    const be<u8>* rightCharacters = rightName->mText;
    for (u32 index = 0; index < u32(sead::kMaxTextLength); ++index) {
        u32 rightCharacter = rightCharacters[index];
        if (rightCharacter == 0) return 1;
        const be<u8>* currentLeft = leftName->mText;
        u32 leftCharacter = currentLeft[index];
        if (leftCharacter == 0 || rightCharacter > leftCharacter) return -1;
        if (rightCharacter < leftCharacter) return 1;
    }
    return 1;
}


void sortEnvironmentReferences(EnvironmentReferenceBuffer*,u32,u32,u32);
s32 compareEnvironmentReferenceObjects(const EnvironmentReference*,const EnvironmentReference*);

/* 0276F814 */
void EnvironmentReferenceList::sortClassRange(u32 typeId) {
    u32 rangeCount = u32(mRanges.mCount);
    /* guest EA arithmetic: mData + typeId * 4 wraps at 32 bits as on the
     * console; a host pointer index on a NULL table reached host addresses past 4 GB */
    const EnvironmentReferenceRange* range =
        &sead::elementAt<EnvironmentReferenceRange>(mRanges.mData, typeId < rangeCount ? typeId : 0u);
    if (range->mCount == 0) return;
    u32 first = range->mFirst;
    u32 count = range->mCount;
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)sortEnvironmentReferences),&mReferences,
        gabi::guest_fn((const void*)compareEnvironmentReferenceObjects),first,first + count - 1);
}
/* 0276FA58 */
void EnvironmentReferenceBuffer::sortRange(u32 compare,u32 first,u32 last) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = u32(mCount);
    if (first >= count || last >= count) return;
    u32 length = last - first + 1;
    if (s32(length) <= 1) return;
    EnvironmentReference* references = static_cast<EnvironmentReference*>(mData) + first;
    gabi::FrameLocal<EnvironmentReference> temporary(8);
    // Build the heap from the last parent toward the root; positions are one-based.
    for (u32 parent = length / 2; parent != 0; --parent) {
        u32 hole = parent;
        temporary->mObject = references[parent - 1].mObject;
        u32 child = hole * 2;
        while (child <= length) {
            EnvironmentReference* childEnd = references + child;
            if (child < length && gabi::call_ptr<s32>(compare,childEnd - 1,childEnd) < 0) {
                ++child;
                childEnd = references + child;
            }
            if (gabi::call_ptr<s32>(compare,temporary.get(),childEnd - 1) >= 0) break;
            references[hole - 1].mObject = (childEnd - 1)->mObject;
            hole = child;
            child = hole * 2;
        }
        references[hole - 1].mObject = temporary->mObject;
    }
    // Move each root to the end and sift the saved end element into the reduced heap.
    do {
        temporary->mObject = references[length - 1].mObject;
        --length;
        references[length].mObject = references[0].mObject;
        u32 hole = 1;
        u32 child = 2;
        while (child <= length) {
            EnvironmentReference* childEnd = references + child;
            if (child < length && gabi::call_ptr<s32>(compare,childEnd - 1,childEnd) < 0) {
                ++child;
                childEnd = references + child;
            }
            if (gabi::call_ptr<s32>(compare,temporary.get(),childEnd - 1) >= 0) break;
            references[hole - 1].mObject = (childEnd - 1)->mObject;
            hole = child;
            child = hole * 2;
        }
        references[hole - 1].mObject = temporary->mObject;
    } while (length > 1);
}

/* WWHD glue
 ext 0276FA58 void sortEnvironmentReferences(EnvironmentReferenceBuffer*,u32,u32,u32) callback
 ext 0276F6DC s32 compareEnvironmentReferenceObjects(const EnvironmentReference*,const EnvironmentReference*) callback
 data sEnvironmentReferenceMethods 101607B8
 data sEnvironmentCapacityMethods 101607A8
 ext 0273AD10 void* allocateEnvironmentReferenceStorage(u32)
 ext 02755FEC sead::RegistryHeap* environmentStorageHeap(void*,void*)
 ext 0276F448 void clearEnvironmentReferenceBuffers(EnvironmentReferenceList*)
 ext 0273AF40 void releaseEnvironmentReferenceList(void*)
*/

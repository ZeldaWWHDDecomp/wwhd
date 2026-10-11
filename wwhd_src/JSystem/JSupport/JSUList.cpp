/** JSU intrusive pointer lists, using the GameCube layout and names. Private decomp source. */
#include "bindings.h"
#include "JSystem/JSupport/JSUList.h"

static JSUListStaticBlock sListStaticBlock;
static u32 sListStaticRegister;

/* 027EC50C */
static JSUPtrLink* JSUPtrLink__ct(JSUPtrLink* i_this, void* object) {
    if (i_this == NULL) i_this = (JSUPtrLink*)operator_new(sizeof(JSUPtrLink));
    if (i_this != NULL) {
        // HD: the allocation entry writes the object before clearing the list and links.
        i_this->mObject = object;
        i_this->mList = NULL;
        i_this->mPrev = NULL;
        i_this->mNext = NULL;
    }
    return i_this;
}

/* 027EC560 */
u32 JSUPtrList::remove(JSUPtrLink* ptr) {
    if (ptr->mList != this) return 0;
    if (mLength == 1) {
        // HD: preserve tail/head write order when the arguments alias list fields.
        mTail = NULL;
        mHead = NULL;
    } else if (ptr == mHead) {
        ptr->mNext->mPrev = NULL;
        mHead = ptr->mNext;
    } else if (ptr == mTail) {
        ptr->mPrev->mNext = NULL;
        mTail = ptr->mPrev;
    } else {
        JSUPtrLink* previous = ptr->mPrev;
        previous->mNext = ptr->mNext;
        JSUPtrLink* next = ptr->mNext;
        next->mPrev = ptr->mPrev;
    }
    ptr->mList = NULL;
    mLength = (u32)mLength - 1;
    return 1;
}

// HD: deleting-destructor entry points also accept null objects and allocation flags.
/* 027EC62C */
static void JSUPtrLink__dt(JSUPtrLink* i_this, u32 flags) {
    if (i_this == NULL) return;
    JSUPtrList* list = i_this->mList;
    if (list != NULL) list->remove(i_this);
    if (flags & 1) operator_delete(i_this);
}

/* 027EC678 */
void JSUPtrList::initiate() {
    mHead = NULL;
    mTail = NULL;
    mLength = 0;
}

/* 027EC68C */
static void JSUPtrList__dt(JSUPtrList* i_this, u32 flags) {
    if (i_this == NULL) return;
    u32 count = i_this->mLength;
    u32 removed = 0;
    JSUPtrLink* node = i_this->mHead;
    while (removed < count) {
        node->mList = NULL;
        // HD: reload the length before advancing to the next node (alias-sensitive).
        count = i_this->mLength;
        ++removed;
        node = node->mNext;
    }
    if (flags & 1) operator_delete(i_this);
}

/* 027EC6D0 */
void JSUPtrList::setFirst(JSUPtrLink* first) {
    first->mList = this;
    first->mPrev = NULL;
    first->mNext = NULL;
    mTail = first;
    mHead = first;
    mLength = 1;
}

/* 027EC6F4 */
u32 JSUPtrList::append(JSUPtrLink* ptr) {
    JSUPtrList* list = ptr->mList;
    u32 result = list != NULL ? list->remove(ptr) : 1;
    if (result != 0) {
        if (mLength == 0) {
            setFirst(ptr);
        } else {
            ptr->mList = this;
            JSUPtrLink* tail = mTail;
            ptr->mNext = NULL;
            ptr->mPrev = tail;
            mTail->mNext = ptr;
            // HD: GHS keeps this count across the tail assignment.
            u32 count = mLength;
            mTail = ptr;
            mLength = count + 1;
        }
    }
    return result;
}

/* 027EC778 */
u32 JSUPtrList::prepend(JSUPtrLink* ptr) {
    JSUPtrList* list = ptr->mList;
    u32 result = list != NULL ? list->remove(ptr) : 1;
    if (result != 0) {
        if (mLength == 0) {
            setFirst(ptr);
        } else {
            ptr->mList = this;
            ptr->mPrev = NULL;
            ptr->mNext = mHead;
            mHead->mPrev = ptr;
            u32 count = mLength;
            mHead = ptr;
            mLength = count + 1;
        }
    }
    return result;
}

/* 027EC7FC */
u32 JSUPtrList::insert(JSUPtrLink* before, JSUPtrLink* ptr) {
    if (before == mHead) return prepend(ptr);
    if (before == NULL) return append(ptr);
    if (before->mList != this) return 0;
    JSUPtrList* list = ptr->mList;
    u32 result = list != NULL ? list->remove(ptr) : 1;
    if (result != 0) {
        JSUPtrLink* previous = before->mPrev;
        ptr->mList = this;
        ptr->mPrev = previous;
        ptr->mNext = before;
        previous->mNext = ptr;
        before->mPrev = ptr;
        mLength = (u32)mLength + 1;
    }
    return result;
}

// HD: compiler-generated initialization of this translation unit's platform statics.
/* 027EC8E0 */
static void __sinit_JSUList_cpp() {
    sListStaticBlock.mWords[2] = 0;
    sListStaticBlock.mWords[0] = 0;
    sListStaticBlock.mWords[3] = 0;
    sListStaticBlock.mWords[1] = 0;
    JSUListConstructRegister(&sListStaticRegister);
}

/* WWHD glue
 data sListStaticBlock 104A2470
 data sListStaticRegister 101F97B0
 ext 028F026C void JSUListConstructRegister(void*)
*/

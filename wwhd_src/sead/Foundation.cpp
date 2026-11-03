// WWHD intrusive links and pointer storage, reconstructed from the game binary.
#include "sead/Foundation.h"
#include "sead/ElementAt.h"

/* 0273B2D0 */
sead::Link* sead::Link::insertAfter(Link* node) {
    Link* next = mNext;
    mNext = node;
    node->mNext = next;
    node->mPrevious = this;
    if (next != NULL) next->mPrevious = node;
    return this;
}

/* 0273B2F0 */
sead::Link* sead::Link::insertBefore(Link* node) {
    Link* previous = mPrevious;
    mPrevious = node;
    node->mNext = this;
    node->mPrevious = previous;
    if (previous != NULL) previous->mNext = node;
    return this;
}

/* 0273B310 */
sead::Link* sead::Link::detach() {
    Link* previous = mPrevious;
    if (previous != NULL) previous->mNext = mNext;
    Link* next = mNext;
    if (next != NULL) next->mPrevious = mPrevious;
    mNext = NULL;
    mPrevious = NULL;
    return this;
}

/* 0273B348 */
sead::Link* sead::LinkList::get(s32 index) {
    if ((u32)index >= (u32)mCount) return NULL;
    Link* node = mNext;
    for (s32 remaining = index; remaining > 0; --remaining) node = node->mNext;
    return node;
}

/* 0273B440 */
sead::Link* sead::LinkList::popBack() {
    if ((s32)mCount <= 0) return NULL;
    Link* result = mPrevious->detach();
    mCount = (u32)mCount - 1;
    return result;
}

/* 0273B494 */
sead::Link* sead::LinkList::popFront() {
    if ((s32)mCount <= 0) return NULL;
    Link* result = mNext->detach();
    mCount = (u32)mCount - 1;
    return result;
}

/* 0273B4E8 */
void sead::LinkList::moveAfter(Link* position, Link* node) {
    if (position == node) return;
    Link* removed = node->detach();
    position->insertAfter(removed);
}

/* 0273B524 */
void sead::LinkList::clear() {
    Link* next = mNext;
    while (next != this) {
        Link* node = next;
        next = next->mNext;
        node->mPrevious = NULL;
        node->mNext = NULL;
    }
    mPrevious = this;
    mNext = this;
    mCount = 0;
}

/* 0273B560 */
void sead::PointerBuffer::setStorage(s32 capacity, void* storage) {
    if (capacity <= 0 || storage == NULL) return;
    mCapacity = capacity;
    mStorage = storage;
    mCount = 0;
}

/* 0273B584 */
void sead::PointerBuffer::allocate(s32 capacity, void* heap, s32 alignment) {
    if (capacity <= 0) return;
    void* storage = allocateFoundationArray((u32)capacity * 4, heap, alignment);
    setStorage(capacity, storage);
}

/* 0273B5DC */
void sead::PointerBuffer::release() {
    void* storage = mStorage;
    if (storage == NULL) return;
    releaseFoundationArray(storage);
    mStorage = NULL;
    mCapacity = 0;
    mCount = 0;
}

/* 027256BC */
void sead::BufferedText::terminate() {
    be<u8>* text = mText;
    text[(u32)mCapacity - 1] = 0;
}

/* 0270C518 */
void sead::BufferedWideText::terminate() {
    elementAt(mText, (u32)mCapacity - 1) = 0;
}


/* 027269F0 */
static void terminateBoardWideText(sead::BufferedWideText* text) {
    sead::elementAt(text->mText, (u32)text->mCapacity - 1) = 0;
}

/* 0272759C */
static void terminateInputWideText(sead::BufferedWideText* text) {
    sead::elementAt(text->mText, (u32)text->mCapacity - 1) = 0;
}

/* 027269C0 */
static void terminateBoardText(sead::BufferedText* text) {
    be<u8>* storage = text->mText;
    storage[(u32)text->mCapacity - 1] = 0;
}

/* 0272756C */
static void terminateInputText(sead::BufferedText* text) {
    be<u8>* storage = text->mText;
    storage[(u32)text->mCapacity - 1] = 0;
}

/* 027345C4 */
static void terminateSceneText(sead::BufferedText* text) {
    be<u8>* storage = text->mText;
    storage[(u32)text->mCapacity - 1] = 0;
}

/* 0273897C */
static void terminateTaskText(sead::BufferedText* text) {
    be<u8>* storage = text->mText;
    storage[(u32)text->mCapacity - 1] = 0;
}
/* 0273B378 */
void sead::LinkList::sort(s32 offset, u32 compare) {
    if ((s32)mCount <= 1) return;
    Link* node = get(1);
    while (node != this) {
        Link* previous = node->mPrevious;
        be<u8>* value = reinterpret_cast<be<u8>*>(node) - (u32)offset;
        Link* next = node->mNext;
        s32 order = gabi::call_ptr<s32>(compare,
            reinterpret_cast<be<u8>*>(previous) - (u32)offset, value);
        if (order > 0) {
            previous = previous->mPrevious;
            while (previous != this) {
                order = gabi::call_ptr<s32>(compare,
                    reinterpret_cast<be<u8>*>(previous) - (u32)offset, value);
                if (order <= 0) break;
                previous = previous->mPrevious;
            }
            node->detach();
            previous->insertAfter(node);
        }
        node = next;
    }
}

/* 0273B624 */
void sead::PointerBuffer::erase(s32 first, s32 count) {
    if (first < 0 || count < 0) return;
    s32 end = (u32)first + (u32)count;
    s32 size = mCount;
    if (end > size) return;
    if (end < size) {
        be<u32>* storage = reinterpret_cast<be<u32>*>(mStorage.get());
        moveFoundationBytes(storage + first, storage + end, ((u32)size - end) * 4, 0);
        size = mCount;
    }
    mCount = (u32)size - count;
}

/* 0273B6AC */
void sead::PointerBuffer::cocktailSort(u32 compare) {
    s32 size = mCount;
    be<u32>* storage = reinterpret_cast<be<u32>*>(mStorage.get());
    if (size <= 1) return;
    s32 upper = size - 1;
    s32 lower = 0;
    s32 changed = 0;
    for (;;) {
        changed = lower;
        for (s32 index = lower; index < upper; ++index) {
            u32 right = storage[index + 1];
            u32 left = storage[index];
            if (gabi::call_ptr<s32>(compare, left, right) > 0) {
                left = storage[index];
                right = storage[index + 1];
                storage[index + 1] = left;
                changed = index;
                storage[index] = right;
            }
        }
        if (lower == changed) return;
        upper = changed;
        changed = upper;
        for (s32 index = upper; index > lower; --index) {
            u32 left = storage[index - 1];
            u32 right = storage[index];
            if (gabi::call_ptr<s32>(compare, right, left) < 0) {
                right = storage[index];
                left = storage[index - 1];
                storage[index - 1] = right;
                changed = index;
                storage[index] = left;
            }
        }
        if (changed == upper) return;
        lower = changed;
    }
}

/* 0273B7B4 */
void sead::PointerBuffer::heapSort(u32 compare) {
    s32 size = mCount;
    be<u32>* storage = reinterpret_cast<be<u32>*>(mStorage.get());
    for (s32 root = size / 2; root > 0; --root) {
        s32 index = root;
        u32 value = storage[root - 1];
        s32 child = index * 2;
        while (child <= size) {
            if (child < size) {
                u32 right = storage[child];
                u32 left = storage[child - 1];
                if (gabi::call_ptr<s32>(compare, left, right) < 0) ++child;
            }
            u32 item = storage[child - 1];
            if (gabi::call_ptr<s32>(compare, value, item) >= 0) break;
            storage[index - 1] = storage[child - 1];
            index = child;
            child = index * 2;
        }
        storage[index - 1] = value;
    }
    while (size > 1) {
        u32 first = storage[0];
        --size;
        u32 value = storage[size];
        storage[size] = first;
        s32 child = 2;
        s32 index = 1;
        while (child <= size) {
            if (child < size) {
                u32 right = storage[child];
                u32 left = storage[child - 1];
                if (gabi::call_ptr<s32>(compare, left, right) < 0) ++child;
            }
            u32 item = storage[child - 1];
            if (gabi::call_ptr<s32>(compare, value, item) >= 0) break;
            storage[index - 1] = storage[child - 1];
            index = child;
            child = index * 2;
        }
        storage[index - 1] = value;
    }
}

/* WWHD glue
 ext C0009988 void moveFoundationBytes(void*, const void*, u32, u32)
 ext 0273B0D4 void* allocateFoundationArray(u32, void*, s32)
 ext 0273AFC8 void releaseFoundationArray(void*)
*/

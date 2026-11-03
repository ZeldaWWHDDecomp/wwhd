// JAudio heap tree, reconstructed from the WWHD binary.
#include "JSystem/JAudio/JASHeapCtrl.h"
using namespace JASystem::Kernel;
extern void* allocateAudioHeapObject(u32);
extern void constructAudioHeapLink(JSUPtrLink*, void*);
extern void clearAudioHeapMutex(void*, u32);
extern void initializeAudioHeapMutex(HeapMutex*);
extern void lockAudioHeapMutex(HeapMutex*);
extern void unlockAudioHeapMutex(HeapMutex*);
namespace JASystem { namespace Kernel {
/* 0280FBE0 */
THeap* constructAudioHeap(THeap* heap, TDisposer* disposer) {
    if (!heap) {
        heap = static_cast<THeap*>(allocateAudioHeapObject(sizeof(THeap)));
        if (!heap)
            return nullptr;
    }
    heap->mChildren.initiate();
    constructAudioHeapLink(&heap->mLink, heap);
    clearAudioHeapMutex(&heap->mMutex, sizeof(heap->mMutex));
    heap->mDisposer = disposer;
    heap->mTail = nullptr;
    heap->mBase = 0;
    heap->mSize = 0;
    initializeAudioHeapMutex(&heap->mMutex);
    return heap;
}
}}
/* 0280FC88 */
void THeap::initRootHeap(u32 base, u32 size) {
    lockAudioHeapMutex(&mMutex);
    u32 aligned = (base + 63) & ~63u;
    mTail = nullptr;
    mBase = aligned;
    mSize = size - (aligned - base);
    unlockAudioHeapMutex(&mMutex);
}
/* 0280FD04 */
u32 THeap::getCurOffset() {
    lockAudioHeapMutex(&mMutex);
    THeap* tail = mTail;
    u32 offset = tail ? (u32)tail->mBase + (u32)tail->mSize - (u32)mBase : 0;
    unlockAudioHeapMutex(&mMutex);
    return offset;
}
/* 0280FD74 */
THeap* THeap::getTailHeap() {
    lockAudioHeapMutex(&mMutex);
    THeap* tail = mTail;
    JSUPtrLink* link = tail ? tail->mLink.mNext.get() : mChildren.mHead.get();
    THeap* heap = link ? static_cast<THeap*>(link->mObject.get()) : nullptr;
    unlockAudioHeapMutex(&mMutex);
    return heap;
}
/* 0280FE20 */
u32 THeap::getTailOffset() {
    lockAudioHeapMutex(&mMutex);
    THeap* heap = getTailHeap();
    u32 offset = heap ? (u32)heap->mBase - (u32)mBase : (u32)mSize;
    unlockAudioHeapMutex(&mMutex);
    return offset;
}
/* 0280FE98 */
void THeap::insertChild(THeap* heap, THeap* next, u32 base, u32 size, u32 fromTail) {
    lockAudioHeapMutex(&mMutex);
    if (fromTail == 0) {
        JSUPtrLink* previous = next ? next->mLink.mPrev.get() : mChildren.mTail.get();
        THeap* previousHeap = previous ? static_cast<THeap*>(previous->mObject.get()) : nullptr;
        if (mTail == previousHeap)
            mTail = heap;
    }
    heap->mBase = base;
    heap->mSize = size;
    heap->mTail = nullptr;
    mChildren.insert(next ? &next->mLink : nullptr, heap ? &heap->mLink : nullptr);
    unlockAudioHeapMutex(&mMutex);
}
/* 0280FF88 */
u32 THeap::alloc(THeap* mother, u32 size) {
    lockAudioHeapMutex(&mMutex);
    if (mBase != 0 || mother->mBase == 0) {
        unlockAudioHeapMutex(&mMutex);
        return 0;
    }
    size = (size + 63) & ~63u;
    u32 current = mother->getCurOffset();
    u32 tail = mother->getTailOffset();
    if (current + size <= tail) {
        THeap* next = mother->getTailHeap();
        mother->insertChild(this, next, (u32)mother->mBase + current, size, 0);
        unlockAudioHeapMutex(&mMutex);
        return 1;
    }
    u32 base = mother->mBase;
    u32 candidate = base;
    u32 best = 0xFFFFFFFF;
    bool found = false;
    THeap* next = nullptr;
    u32 chosen = 0;
    JSUPtrLink* link = mother->mChildren.mHead;
    while (link && candidate < base + tail) {
        THeap* child = static_cast<THeap*>(link->mObject.get());
        u32 gap = (u32)child->mBase - candidate;
        if (gap >= size && gap < best) {
            next = child;
            chosen = candidate;
            best = gap;
            found = true;
        }
        candidate = (u32)child->mBase + (u32)child->mSize;
        link = link->mNext;
    }
    if (candidate != base && candidate < base + tail) {
        u32 gap = base + (u32)mother->mSize - candidate;
        if (gap >= size && gap < best) {
            mother->insertChild(this, nullptr, candidate, size, 0);
            unlockAudioHeapMutex(&mMutex);
            return 1;
        }
    }
    if (!found) {
        unlockAudioHeapMutex(&mMutex);
        return 0;
    }
    mother->insertChild(this, next, chosen, size, 0);
    unlockAudioHeapMutex(&mMutex);
    return 1;
}
/* 0281017C */
u32 THeap::free() {
    lockAudioHeapMutex(&mMutex);
    if (mBase == 0) {
        unlockAudioHeapMutex(&mMutex);
        return 0;
    }
    JSUPtrLink* link = mChildren.mHead;
    while (link) {
        JSUPtrLink* next = link->mNext;
        static_cast<THeap*>(link->mObject.get())->free();
        link = next;
    }
    JSUPtrList* parent = mLink.mList;
    if (parent) {
        // The tree starts with the list; its object sits in the following link.
        THeap* parentHeap = static_cast<THeap*>(reinterpret_cast<THeap*>(parent)->mLink.mObject.get());
        if (parentHeap->mTail == this) {
            JSUPtrLink* previous = mLink.mPrev;
            parentHeap->mTail = previous ? static_cast<THeap*>(previous->mObject.get()) : nullptr;
        }
        parent->remove(gabi::ea(this) ? &mLink : nullptr);
    }
    TDisposer* disposer = mDisposer;
    mSize = 0;
    mTail = nullptr;
    mBase = 0;
    if (disposer)
        gabi::call_ptr<void>(disposer->mVtable->onDispose.get(), disposer);
    unlockAudioHeapMutex(&mMutex);
    return 1;
}
/* WWHD glue
ext 027EC678 void JSUPtrList::initiate()
ext 027EC7FC u32 JSUPtrList::insert(JSUPtrLink*, JSUPtrLink*)
ext 027EC560 u32 JSUPtrList::remove(JSUPtrLink*)
ext 0273AD10 void* allocateAudioHeapObject(u32)
ext 027EC50C void constructAudioHeapLink(JSUPtrLink*, void*)
ext 028F521C void clearAudioHeapMutex(void*, u32)
ext C0009D68 void initializeAudioHeapMutex(HeapMutex*)
ext C0009E40 void lockAudioHeapMutex(HeapMutex*)
ext C000A1C8 void unlockAudioHeapMutex(HeapMutex*)
*/

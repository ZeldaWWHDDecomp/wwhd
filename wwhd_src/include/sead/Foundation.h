// WWHD foundation helpers, reconstructed from this game's instructions.
#pragma once
#include "wwhd.h"
namespace sead {
struct Link {
    gptr<Link> mPrevious;
    gptr<Link> mNext;
    Link* insertAfter(Link* node);
    Link* insertBefore(Link* node);
    Link* detach();
};
struct LinkList : Link {
    be<s32> mCount;
    Link* get(s32 index);
    Link* popBack();
    Link* popFront();
    void moveAfter(Link* position, Link* node);
    void clear();
    void sort(s32 offset, u32 compare);
};
struct PointerBuffer {
    be<s32> mCount;
    be<s32> mCapacity;
    gptr<void> mStorage;
    void setStorage(s32 capacity, void* storage);
    void allocate(s32 capacity, void* heap, s32 alignment);
    void release();
    void erase(s32 first, s32 count);
    void cocktailSort(u32 compare);
    void heapSort(u32 compare);
};
struct BufferedText {
    gptr<be<u8>> mText;
    be<u32> mVtable;
    be<s32> mCapacity;
    void terminate();
};
struct BufferedWideText {
    gptr<be<u16>> mText;
    be<u32> mVtable;
    be<s32> mCapacity;
    void terminate();
};
}
WWHD_OFFSET(sead::Link,mPrevious,0);
WWHD_OFFSET(sead::Link,mNext,4);
WWHD_SIZE(sead::Link,8);
WWHD_OFFSET(sead::LinkList,mCount,8);
WWHD_SIZE(sead::LinkList,12);
WWHD_OFFSET(sead::PointerBuffer,mCount,0);
WWHD_OFFSET(sead::PointerBuffer,mCapacity,4);
WWHD_OFFSET(sead::PointerBuffer,mStorage,8);
WWHD_SIZE(sead::PointerBuffer,12);
WWHD_OFFSET(sead::BufferedText,mText,0);
WWHD_OFFSET(sead::BufferedText,mCapacity,8);
WWHD_SIZE(sead::BufferedText,12);
WWHD_OFFSET(sead::BufferedWideText,mText,0);
WWHD_OFFSET(sead::BufferedWideText,mCapacity,8);
WWHD_SIZE(sead::BufferedWideText,12);
void* allocateFoundationArray(u32 size, void* heap, s32 alignment);
void releaseFoundationArray(void* storage);

void moveFoundationBytes(void* destination, const void* source, u32 size, u32 flags);

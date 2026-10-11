/** JSU pointer links and lists. GameCube layout is unchanged in HD (16/12 bytes). */
#pragma once
#include "wwhd.h"
class JSUPtrList;
class JSUPtrLink {
public:
    /* 0x00 */ gptr<void> mObject;
    /* 0x04 */ gptr<JSUPtrList> mList;
    /* 0x08 */ gptr<JSUPtrLink> mPrev;
    /* 0x0C */ gptr<JSUPtrLink> mNext;
};
class JSUPtrList {
public:
    void initiate();
    void setFirst(JSUPtrLink* first);
    // HD: preserve the full callee result register rather than narrowing it to bool.
    u32 append(JSUPtrLink* ptr);
    u32 prepend(JSUPtrLink* ptr);
    u32 insert(JSUPtrLink* before, JSUPtrLink* ptr);
    u32 remove(JSUPtrLink* ptr);
    /* 0x00 */ gptr<JSUPtrLink> mHead;
    /* 0x04 */ gptr<JSUPtrLink> mTail;
    /* 0x08 */ be<u32> mLength;
};
struct JSUListStaticBlock { be<u32> mWords[4]; };
void JSUListConstructRegister(void* object);
WWHD_OFFSET(JSUPtrLink,mObject,0x0);
WWHD_OFFSET(JSUPtrLink,mList,0x4);
WWHD_OFFSET(JSUPtrLink,mPrev,0x8);
WWHD_OFFSET(JSUPtrLink,mNext,0xC);
WWHD_SIZE(JSUPtrLink,0x10);
WWHD_OFFSET(JSUPtrList,mHead,0x0);
WWHD_OFFSET(JSUPtrList,mTail,0x4);
WWHD_OFFSET(JSUPtrList,mLength,0x8);
WWHD_SIZE(JSUPtrList,0xC);
WWHD_SIZE(JSUListStaticBlock,0x10);

/* JAudio 1 automatic and persistent sequence heaps. */
#pragma once
#include "JSystem/JAudio/JAIGlobalParameter.h"
namespace JAInter {
struct HeapBlock {
    HeapBlock();
    be<u8> mStatus;
    u8 padding[3];
    gptr<void> mPointer;
    be<u32> mDataNumber;
    be<u32> mEndAddress;
    be<u32> mUseCounter;
};
WWHD_OFFSET(HeapBlock, mStatus, 0);
WWHD_OFFSET(HeapBlock, mPointer, 4);
WWHD_OFFSET(HeapBlock, mDataNumber, 8);
WWHD_OFFSET(HeapBlock, mEndAddress, 12);
WWHD_OFFSET(HeapBlock, mUseCounter, 16);
WWHD_SIZE(HeapBlock, 20);
namespace HeapMgr {
void init(u32 stayCount, u32 staySize, u32 autoCount, u32 autoSize);
void* checkOnMemory(u32 data, be<u8>* index);
void releaseAutoHeapPointer(u32 index);
u8 checkUsefulAutoHeapPosition();
void* getFreeAutoHeapPointer(u32 index, u32 data);
void* getFreeStayHeapPointer(u32 size, u32 data);
void setAutoHeapLoadedFlag(u32 index, u32 status);
}}
u32 getAutoHeapMax();
u32 getStayHeapMax();
u32 getStayHeapSize();
void heapAssert(const char*, u32, const char*);
void* allocateSequenceArray(u32, JKRHeap*, s32);
using SequenceElementConstructor = JAInter::HeapBlock* (*)(JAInter::HeapBlock*);
JAInter::HeapBlock* constructSequenceArray(void*, u32, u32, SequenceElementConstructor, u32);
void registerSequenceStatics(AudioDestructorRecord*);
// Audio1 owner exports for JS-JAudio#2: the auto-heap block (104B5024) and its accessor (02808084).
namespace JAInter { namespace HeapMgr {
extern gptr<HeapBlock>& sAutoHeap;
HeapBlock* getAutoHeapPointer();
}}

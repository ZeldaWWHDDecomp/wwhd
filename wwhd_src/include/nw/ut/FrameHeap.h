#pragma once
#include "wwhd.h"
namespace nw { namespace ut {
struct Heap;
struct HeapLink { gptr<HeapLink> next; gptr<HeapLink> previous; };
struct HeapChildren { be<u32> count; HeapLink root; };
struct HeapCheckpoint { be<u32> tag; be<u32> head; be<u32> tail; gptr<HeapCheckpoint> previous; };
struct Heap {
    enum Option { ClearAllocated = 1, FillDebugPattern = 2 };
    HeapLink sibling;
    be<u32> vtable;
    be<u32> begin;
    be<u32> end;
    be<u32> signature;
    HeapChildren children;
    be<u32> options;
    void setOptions(u32);
    u32 getOptions();
    u32 fillPattern(u32);
    void fillInitial(void*,u32);
    void fillFreed(void*,u32);
    void fillAllocated(void*,u32);
    void initialize(u32,u32,u32,u32);
    void finalize();
    void lock();
    void unlock();
};
struct FrameHeap : Heap {
    enum ResetDirection { ResetHead = 1, ResetTail = 2 };
    be<u32> head;
    be<u32> tail;
    gptr<HeapCheckpoint> checkpoint;
    void* allocateHead(u32,u32);
    void* allocateTail(u32,u32);
    void* allocate(u32,s32);
    void resetHead();
    void resetTail();
    void reset(u32);
    FrameHeap* destroy();
    static FrameHeap* create(u32,u32,u32);
};
Heap* findContaining(HeapChildren*,u32);
HeapChildren* findParent(u32);
void initializeHeapRoots();
u32 appendText(be<u8>*,u32,const be<u8>*,u32);
WWHD_SIZE(HeapLink,8); WWHD_SIZE(HeapChildren,12); WWHD_SIZE(HeapCheckpoint,16);
WWHD_OFFSET(Heap,begin,0xC); WWHD_OFFSET(Heap,children,0x18); WWHD_OFFSET(Heap,options,0x24); WWHD_SIZE(Heap,0x28);
WWHD_OFFSET(FrameHeap,head,0x28); WWHD_OFFSET(FrameHeap,tail,0x2C); WWHD_OFFSET(FrameHeap,checkpoint,0x30); WWHD_SIZE(FrameHeap,0x34);
} }
void* utAllocateHeap(u32);
void utFill(void*,u32,u32);
void utInsertChild(nw::ut::HeapChildren*,const gptr<nw::ut::HeapLink>*,nw::ut::Heap*);
void utRemoveChild(nw::ut::HeapChildren*,nw::ut::Heap*);

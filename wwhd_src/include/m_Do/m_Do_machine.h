/** Machine startup views; local private decomp source. */
#pragma once
#include "wwhd.h"

struct mDoMch_HeapVtable {
    u8 _padding00[0x74];
    gfn mGetFreeSize;
};
WWHD_OFFSET(mDoMch_HeapVtable, mGetFreeSize, 0x74);
struct mDoMch_Heap {
    u8 _padding00[0xC];
    gptr<mDoMch_HeapVtable> mVtable;
    u8 _padding10[0x14];
    gptr<mDoMch_Heap> mParent;
    u32 getFreeSize() { return gabi::call_ptr<u32>(mVtable->mGetFreeSize.get(), this); }
};
WWHD_OFFSET(mDoMch_Heap, mVtable, 0xC);
WWHD_OFFSET(mDoMch_Heap, mParent, 0x24);
struct mDoMch_FrameworkSettings {
    be<u32> mMaxStdHeap;
    be<u32> mSysHeapSize;
    be<u32> mFifoBufSize;
    be<u32> mAramAudioBufSize;
    be<u32> mAramGraphBufSize;
    be<u8> mInitialized;
};
WWHD_OFFSET(mDoMch_FrameworkSettings, mInitialized, 0x14);
struct mDoMch_DbPrint {
    u8 _padding00[0xC];
    be<u8> mVisible;
};
WWHD_OFFSET(mDoMch_DbPrint, mVisible, 0xC);

bool mDoMch_Create();
void JFWSystem_init();
u32 mDoMch_setThreadHeap(void* manager, u32 heap);
mDoMch_Heap* mDoMch_getCurrentHeap();
mDoMch_Heap* mDoMch_getSystemHeap();
void* mDoExt_createCommandHeap(u32 size, mDoMch_Heap* parent);
void* mDoExt_createArchiveHeap(u32 size, mDoMch_Heap* parent);
void* mDoExt_createGameHeap(u32 index, u32 size, mDoMch_Heap* parent);
void* mDoExt_createZeldaHeap(u32 size, mDoMch_Heap* parent);
void mDoMch_setCurrentHeap(void* heap);
void* mDoExt_getZeldaHeap();
void cMl_initFromHeapReturnRegister();
void cM_initRnd(s32, s32, s32);
void mDoDvdThd_create(u32 priority);

namespace JFWSystem {
inline void init() { JFWSystem_init(); }
}
namespace mDoDvdThd {
inline void create(u32 priority) { mDoDvdThd_create(priority); }
}
namespace cMl {
// HD: preserve the incoming return register instead of supplying a new argument.
inline void init() { cMl_initFromHeapReturnRegister(); }
}

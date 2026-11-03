/* HD fixed-size audio work storage and its heap-disposal prefix. */
#pragma once
#include "bindings.h"
namespace JASystem { namespace Kernel {
struct WorkPoolBlock;
struct WorkPoolLink { gptr<WorkPoolLink> mPrevious,mNext; };
struct WorkPoolBlock {
    union { gptr<WorkPoolBlock> mNextFree;u8 mPayload[0xA80]; };
    WorkPoolLink mLink;
};
struct WorkPoolStorage {
    WorkPoolLink mSentinel;
    be<u32> mCount;
    gptr<WorkPoolBlock> mFreeBlocks,mBlocks;
    be<u32> mCapacity;
};
struct WorkPoolDisposerVtable { be<u32> mPrefix[3];be<u32> mDestroy; };
struct WorkPoolDisposer { u8 mHeapLinks[12];gptr<WorkPoolDisposerVtable> mVtable; };
class TWorkPool {
public:
    static TWorkPool* create(JKRHeap*);
    void activate();
    void deactivate();
    void destroy(u32);
    WorkPoolDisposer mDisposer;
    gptr<JKRHeap> mHeap;
    gptr<void> mContext;
    be<u8> mActive;
    u8 padding[3];
    WorkPoolStorage mPool;
};
extern gptr<TWorkPool>& workPoolInstance;
extern gptr<WorkPoolDisposer>& workPoolDisposer;
WWHD_OFFSET(WorkPoolBlock,mLink,0xA80);
WWHD_SIZE(WorkPoolBlock,0xA88);
WWHD_SIZE(WorkPoolStorage,0x18);
WWHD_OFFSET(TWorkPool,mHeap,0x10);
WWHD_OFFSET(TWorkPool,mContext,0x14);
WWHD_OFFSET(TWorkPool,mActive,0x18);
WWHD_OFFSET(TWorkPool,mPool,0x1C);
WWHD_SIZE(TWorkPool,0x34);
}}
void* workPoolAllocate(u32);
void* workPoolHeapAllocate(u32,JKRHeap*,s32);
void workPoolConstructDisposer(void*,JKRHeap*,u32);
void workPoolUnlink(JASystem::Kernel::WorkPoolLink*);
void workPoolFreeArray(void*);
void workPoolDelete(void*);

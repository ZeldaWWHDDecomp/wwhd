/* Cafe audio work-pool ownership; fixed blocks carry their list link after the payload. */
#include "JSystem/JAudio/JASWorkPool.h"
using namespace JASystem::Kernel;
namespace JASystem { namespace Kernel {
static TWorkPool* workPoolInstance;
static WorkPoolDisposer* workPoolDisposer;
}}
struct AudioRuntimePrefix { u8 mState[0x24];u8 mWorkContext; };
static AudioRuntimePrefix* poolRuntime;
static JKRHeap* poolDefaultHeap;
static JKRHeap* poolSystemHeap;
static WorkPoolDisposerVtable disposerVtable;
/* 0280F080 */
static TWorkPool* constructWorkPool(TWorkPool* work) {
    if(!work) work=static_cast<TWorkPool*>(workPoolAllocate(sizeof(TWorkPool)));
    if(work) {
        work->mActive=0;work->mHeap=poolDefaultHeap;work->mContext=nullptr;
        WorkPoolStorage* pool=&work->mPool;
        if(gabi::ea(pool)==0) pool=static_cast<WorkPoolStorage*>(workPoolAllocate(sizeof(WorkPoolStorage)));
        if(pool) {
            pool->mCount=0;pool->mBlocks=nullptr;pool->mCapacity=0;pool->mFreeBlocks=nullptr;
            pool->mSentinel.mPrevious=&pool->mSentinel;pool->mSentinel.mNext=&pool->mSentinel;
        }
        WorkPoolBlock* blocks=static_cast<WorkPoolBlock*>(workPoolHeapAllocate(32*sizeof(WorkPoolBlock),poolSystemHeap,64));
        if(blocks) {
            work->mPool.mFreeBlocks=blocks;
            for(u32 i=0;i<31;++i) blocks[i].mNextFree=&blocks[i+1];
            blocks[31].mNextFree=nullptr;
            work->mPool.mBlocks=blocks;work->mPool.mCapacity=32;
        }
    }
    return work;
}
namespace JASystem { namespace Kernel {
/* 0280F168 */
TWorkPool* TWorkPool::create(JKRHeap* heap) {
    if(!workPoolInstance) {
        TWorkPool* work=static_cast<TWorkPool*>(workPoolHeapAllocate(sizeof(TWorkPool),heap,4));
        if(work) {
            workPoolConstructDisposer(work,heap,3);
            work->mDisposer.mVtable=&disposerVtable;
        }
        workPoolDisposer=work?&work->mDisposer:nullptr;
        workPoolInstance=work?constructWorkPool(work):nullptr;
    }
    return workPoolInstance;
}
/* 0280F1FC */
void TWorkPool::deactivate() { if(mActive) mActive=0; }
/* 0280F214 */
void TWorkPool::destroy(u32 flags) {
    if(!this) return;
    deactivate();
    WorkPoolStorage* pool=&mPool;
    if(pool->mBlocks) {
        WorkPoolLink* link=pool->mSentinel.mNext;
        while(link!=&pool->mSentinel) {
            WorkPoolLink* next=link->mNext;
            workPoolUnlink(link);
            pool->mCount=static_cast<u32>(pool->mCount)-1;
            // Intrusive links occupy the final member of each block.
            WorkPoolBlock* block=reinterpret_cast<WorkPoolBlock*>(reinterpret_cast<u8*>(link)-offsetof(WorkPoolBlock,mLink));
            block->mNextFree=pool->mFreeBlocks;pool->mFreeBlocks=block;
            link=next;
        }
        workPoolFreeArray(pool->mBlocks);
        pool->mCapacity=0;pool->mBlocks=nullptr;pool->mFreeBlocks=nullptr;
    }
    if(flags&1) workPoolDelete(this);
}
/* 0280F2C4 */
void TWorkPool::activate() {
    if(mActive) return;
    mActive=1;mContext=&poolRuntime->mWorkContext;
}
}}
/* WWHD glue
data workPoolInstance 101F9D6C
data workPoolDisposer 101F9D70
data poolDefaultHeap 101F9D74
data poolSystemHeap 101F9DA8
data poolRuntime 101F8B08
data disposerVtable 1016FDA4
ext 0273AD10 void* workPoolAllocate(u32)
ext 0273B0D4 void* workPoolHeapAllocate(u32,JKRHeap*,s32)
ext 02752B0C void workPoolConstructDisposer(void*,JKRHeap*,u32)
ext 0273B310 void workPoolUnlink(JASystem::Kernel::WorkPoolLink*)
ext 0273AFC8 void workPoolFreeArray(void*)
ext 0273AF40 void workPoolDelete(void*)
*/

#ifndef WWHD_JASSYSTEMHEAP_H
#define WWHD_JASSYSTEMHEAP_H
#include "JSystem/JAudio/JASHeapCtrl.h"
using JASHeapPointer = gptr<void>;
extern JASHeapPointer& JASDram;
namespace JASystem { namespace Kernel {
struct MessageBlock {
    gptr<MessageBlock> mPrevious;
    be<u32> mUsed, mReferences;
    u8 mData[0x400];
};
struct MessagePool { gptr<MessageBlock> mHead; HeapMutex mMutex; };
struct PoolNode { gptr<PoolNode> mNext; };
class TPool {
public:
    gptr<PoolNode> mHead, mTail;
    be<u32> mCount;
    TPool();
    PoolNode* pop();
    void push(PoolNode* node);
    void newMemPool(u32 size, s32 count);
};
void* getSystemHeap();
void* getMessagePool();
THeap* getAramHeap();
void sysDramSetup(void* heap, u32 size);
void sysAramSetup(void* heap, u32 size);
}}
WWHD_OFFSET(JASystem::Kernel::TPool, mHead, 0);
WWHD_OFFSET(JASystem::Kernel::TPool, mTail, 4);
WWHD_OFFSET(JASystem::Kernel::TPool, mCount, 8);
WWHD_SIZE(JASystem::Kernel::TPool, 0xC);
#endif

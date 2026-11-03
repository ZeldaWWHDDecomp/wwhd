// HD: JAudio system heaps and intrusive allocation pools.
#include "JSystem/JAudio/JASSystemHeap.h"
using namespace JASystem::Kernel;
using HeapPointer = gptr<void>;
using PoolPointer = gptr<MessagePool>;
struct HeapName { gptr<const char> mText; gptr<void> mVtable; };
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
static JASHeapPointer JASDram;
static HeapPointer sSystemHeap;
static HeapPointer sAramStorage;
static PoolPointer sMessagePool;
static THeap sAramHeap;
static HeapName sHeapName;
static u32 sHeapNameVtable;
static Initialization sInitialization;
static Registration sRegistration;
static Registration sHeapRegistration;
extern void* allocateSystemHeapArray(u32, void*, u32);
extern void* allocateSystemHeapStorage(u32, void*, u32);
extern void* createAudioSystemHeap(u32, HeapName*, void*, u32, u32);
extern void initializePoolMutex(HeapMutex*);
extern void registerSystemHeapGlobals(void*);
extern void deleteSystemHeapObject(void*);
extern void destroySystemHeapLink(JSUPtrLink*, u32);
extern void destroySystemHeapList(JSUPtrList*, u32);
/* 028102C0 */
TPool::TPool() {
    mHead = nullptr;
    mCount = 0;
    mTail = nullptr;
}
/* 02810304 */
PoolNode* TPool::pop() {
    PoolNode* node = mHead;
    if (!node)
        return nullptr;
    PoolNode* next = node->mNext;
    u32 count = mCount;
    mHead = next;
    mCount = count - 1;
    if (!next)
        mTail = nullptr;
    return node;
}
/* 02810344 */
void TPool::push(PoolNode* node) {
    node->mNext = nullptr;
    PoolNode* tail = mTail;
    if (tail)
        tail->mNext = node;
    else
        mHead = node;
    u32 count = mCount;
    mTail = node;
    mCount = count + 1;
}
/* 02810374 */
void TPool::newMemPool(u32 size, s32 count) {
    for (s32 i = 0; i < count; ++i) {
        auto node = static_cast<PoolNode*>(allocateSystemHeapArray(size, JASDram, 4));
        node->mNext = mHead;
        PoolNode* tail = mTail;
        mHead = node;
        if (!tail)
            mTail = node;
    }
    // HD: a negative count still adjusts the stored count after skipping allocation.
    mCount = (u32)mCount + (u32)count;
}
namespace JASystem { namespace Kernel {
/* 028103F0 */
void syJASDramSetup(void* heap, u32 size) {
    sSystemHeap = createAudioSystemHeap(size, &sHeapName, heap, 1, 0);
    auto pool = static_cast<MessagePool*>(allocateSystemHeapStorage(sizeof(MessagePool), heap, 4));
    if (pool) {
        pool->mHead = nullptr;
        initializePoolMutex(&pool->mMutex);
        MessageBlock* block = pool->mHead;
        if (block && block->mReferences == 0) {
            block->mUsed = 0;
        } else {
            auto newBlock = static_cast<MessageBlock*>(allocateSystemHeapStorage(sizeof(MessageBlock), sSystemHeap, 4));
            if (newBlock) {
                newBlock->mUsed = 0;
                newBlock->mPrevious = block;
                newBlock->mReferences = 0;
            }
            pool->mHead = newBlock;
        }
    }
    sMessagePool = pool;
    JASDram = heap;
}
/* 028104E8 */
void sysAramSetup(void* heap, u32 size) {
    void* storage = allocateSystemHeapArray(size, heap, 64);
    sAramStorage = storage;
    sAramHeap.initRootHeap(gabi::ea(storage), size);
}
}}
/* 0281053C */
void __sinit_JASSystemHeap_cpp() {
    sInitialization.words[3] = 0;
    sInitialization.words[2] = 0;
    sInitialization.words[1] = 0;
    sInitialization.words[0] = 0;
    registerSystemHeapGlobals(&sRegistration);
    sHeapName.mVtable = &sHeapNameVtable;
    sHeapName.mText = gabi::at<const char>(gabi::guest_str("AudioSystemHeap"));
    constructAudioHeap(&sAramHeap, nullptr);
    registerSystemHeapGlobals(&sHeapRegistration);
}
/* 028105C4 */
void destroyAudioHeapName(HeapName* name, u32 flags) {
    if (name && (flags & 1))
        deleteSystemHeapObject(name);
}
/* 028105D8 */
void destroyAudioHeapTree(THeap* heap, u32 flags) {
    if (!heap)
        return;
    destroySystemHeapLink(&heap->mLink, 0);
    destroySystemHeapList(&heap->mChildren, 0);
    if (flags & 1)
        deleteSystemHeapObject(heap);
}
namespace JASystem { namespace Kernel {
/* 02810638 */
void* getSystemHeap() { return sSystemHeap; }
/* 02810648 */
void* getMessagePool() { return sMessagePool; }
/* 02810654 */
THeap* getAramHeap() { return &sAramHeap; }
}}
/* 02810644 */
void assureAudioHeapNameTermination(HeapName*) {}
/* WWHD glue
ext 0273AF40 void deleteSystemHeapObject(void*)
ext 027EC62C void destroySystemHeapLink(JSUPtrLink*, u32)
ext 027EC68C void destroySystemHeapList(JSUPtrList*, u32)
data JASDram 101F9DA8
data sSystemHeap 101F9D88
data sMessagePool 101F9D8C
data sAramStorage 101F9D84
data sAramHeap 104B521C
data sHeapName 104B5204
data sHeapNameVtable 1016FE38
data sInitialization 104B520C
data sRegistration 101F9D90
data sHeapRegistration 101F9D9C
str 1016FE54
ext 0273B0D4 void* allocateSystemHeapArray(u32, void*, u32)
ext 0273B050 void* allocateSystemHeapStorage(u32, void*, u32)
ext 02753004 void* createAudioSystemHeap(u32, HeapName*, void*, u32, u32)
ext C0009D68 void initializePoolMutex(HeapMutex*)
ext 028F026C void registerSystemHeapGlobals(void*)
*/

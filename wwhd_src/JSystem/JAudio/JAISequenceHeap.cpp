/* Based on CC0 tww JAISequenceHeap.cpp; WWHD fields and call ordering. */
#include "JSystem/JAudio/JAISequenceHeap.h"
namespace JAInter { namespace HeapMgr {
static HeapBlock* sAutoHeap;
static HeapBlock* sStayHeap;
static u32 sAutoHeapCount;
static u32 sStayHeapCount;
static JKRHeap* msCurrentHeap;
static u32 arrayCookieSize;
}}
using namespace JAInter::HeapMgr;
using JAInter::HeapBlock;
static AudioHeaderStatics sequenceHeaderStatics;
static AudioDestructorRecord sequenceDestructorRecord;
namespace JAInter {
/* 028079AC */
HeapBlock::HeapBlock() {
    mEndAddress = 0;
    mUseCounter = -1;
    mDataNumber = -1;
}
}
namespace JAInter { namespace HeapMgr {
/* 028079F4 */
void init(u32 stayCount, u32 staySize, u32 autoCount, u32 autoSize) {
    JKRHeap* heap = msCurrentHeap;
    if (!heap) { heapAssert("JAIBasic.h", 169, "msCurrentHeap"); heap = msCurrentHeap; }
    void* storage = allocateSequenceArray(autoCount * 20 + arrayCookieSize, heap, 32);
    if (storage) {
        sAutoHeap = constructSequenceArray((u8*)storage + arrayCookieSize, autoCount, 20,
                                           HeapBlock__ct, 0);
    } else sAutoHeap = nullptr;
    if (!sAutoHeap) heapAssert("JAISequenceHeap.cpp", 38, "sAutoHeap");
    for (u32 i = 0; i < autoCount; ++i) {
        sAutoHeap[i].mStatus = 0;
        sAutoHeap[i].mEndAddress = 0;
        sAutoHeap[i].mDataNumber = -1;
        sAutoHeap[i].mUseCounter = -1;
        heap = msCurrentHeap;
        HeapBlock* block = sAutoHeap + i;
        if (!heap) { heapAssert("JAIBasic.h", 169, "msCurrentHeap"); heap = msCurrentHeap; }
        block->mPointer = allocateSequenceArray(autoSize, heap, 32);
    }
    heap = msCurrentHeap;
    if (!heap) { heapAssert("JAIBasic.h", 169, "msCurrentHeap"); heap = msCurrentHeap; }
    storage = allocateSequenceArray(stayCount * 20 + arrayCookieSize, heap, 32);
    HeapBlock* stay;
    if (storage) stay = constructSequenceArray((u8*)storage + arrayCookieSize, stayCount, 20,
                                              HeapBlock__ct, 0);
    else stay = nullptr;
    sStayHeap = stay;
    if (!stay) {
        heapAssert("JAISequenceHeap.cpp", 48, "sStayHeap");
        stay = sStayHeap;
    }
    heap = msCurrentHeap;
    if (!heap) { heapAssert("JAIBasic.h", 169, "msCurrentHeap"); heap = msCurrentHeap; }
    stay->mPointer = allocateSequenceArray(staySize, heap, 32);
    for (u32 i = 0; i < stayCount; ++i) {
        sStayHeap[i].mStatus = 0;
        sStayHeap[i].mEndAddress = 0;
        sStayHeap[i].mDataNumber = -1;
        sStayHeap[i].mUseCounter = -1;
    }
}
/* 02807C80 */
void* checkOnMemory(u32 data, be<u8>* index) {
    for (u8 i = 0; i < getAutoHeapMax(); ++i) {
        HeapBlock* block = sAutoHeap + i;
        if (data != block->mDataNumber) continue;
        if (block->mStatus == 1) return (void*)0xFFFFFFFFu;
        if (index) { *index = i; block = sAutoHeap + i; }
        return block->mPointer;
    }
    HeapBlock* blocks = sStayHeap;
    for (u8 i = 0; i < sStayHeapCount; ++i) {
        HeapBlock* block = blocks + i;
        if (data != block->mDataNumber) continue;
        if (index) { *index = 255; block = sStayHeap + i; }
        return block->mPointer;
    }
    return nullptr;
}
/* 02807DF0 */
void releaseAutoHeapPointer(u32 index) {
    if (index != 255) sAutoHeap[index].mUseCounter = -1;
}
/* 02807E14 */
u8 checkUsefulAutoHeapPosition() {
    u32 index = 0;
    u32 oldest = -1;
    u32 oldestIndex = 0;
    for (; index < getAutoHeapMax(); ++index) {
        if (sAutoHeap[index].mDataNumber == 0xFFFFFFFFu) break;
    }
    if (index == getAutoHeapMax()) {
        for (index = 0; index < getAutoHeapMax(); ++index) {
            HeapBlock* block = sAutoHeap + index;
            if (oldest > block->mEndAddress && block->mUseCounter == 0xFFFFFFFFu) {
                oldest = block->mEndAddress;
                oldestIndex = index;
            }
        }
        if (oldest != 0xFFFFFFFFu) index = oldestIndex;
    }
    return index;
}
/* 02807EEC */
void* getFreeAutoHeapPointer(u32 index, u32 data) {
    sAutoHeap[index].mDataNumber = data;
    HeapBlock* block = sAutoHeap + index;
    block->mUseCounter = sAutoHeapCount;
    HeapBlock* endBlock = sAutoHeap + index;
    u32 count = sAutoHeapCount;
    void* result = block->mPointer;
    endBlock->mEndAddress = count;
    ++sAutoHeapCount;
    return result;
}
/* 02807F38 */
void* getFreeStayHeapPointer(u32 size, u32 data) {
    u32 max = getStayHeapMax();
    if (sStayHeapCount >= max) return nullptr;
    HeapBlock* blocks = sStayHeap;
    void* buffer = blocks[sStayHeapCount].mPointer;
    u32 end = gabi::ea(buffer) + size;
    u32 capacity = getStayHeapSize();
    u32 base = gabi::ea((void*)blocks[0].mPointer);
    if (end >= base + capacity) return nullptr;
    max = getStayHeapMax();
    if (sStayHeapCount >= max) return nullptr;
    HeapBlock* block = sStayHeap + sStayHeapCount;
    block->mDataNumber = data;
    u32 count = sStayHeapCount;
    void* current = sStayHeap[count].mPointer;
    buffer = block->mPointer;
    u8* next = (u8*)current + (size & ~31u);
    if (size & 31) next += 32;
    ++count;
    sStayHeapCount = count;
    max = getStayHeapMax();
    if (count < max) sStayHeap[sStayHeapCount].mPointer = next;
    return buffer;
}
/* 02808048 */
void setAutoHeapLoadedFlag(u32 index, u32 status) { sAutoHeap[index].mStatus = status; }
}}
/* 0280805C */
void __sinit_JAISequenceHeap_cpp() {
    sequenceHeaderStatics.mWords[2] = 0;
    sequenceHeaderStatics.mWords[0] = 0;
    sequenceHeaderStatics.mWords[3] = 0;
    sequenceHeaderStatics.mWords[1] = 0;
    registerSequenceStatics(&sequenceDestructorRecord);
}
using namespace JAInter;
/* WWHD glue
data sAutoHeap 104B5024
data sStayHeap 104B5028
data sAutoHeapCount 101F9C80
data sStayHeapCount 101F9C84
data msCurrentHeap 101F9B54
data arrayCookieSize 101FCD08
data sequenceHeaderStatics 104B502C
data sequenceDestructorRecord 101F9C88
ext 028046F4 u32 getAutoHeapMax()
ext 02804700 u32 getStayHeapMax()
ext 02804748 u32 getStayHeapSize()
str 1016F818 "JAIBasic.h"
str 1016F7F0 "msCurrentHeap"
str 1016F7DC "JAISequenceHeap.cpp"
str 1016F800 "sAutoHeap"
str 1016F80C "sStayHeap"
ext 0273AA24 void heapAssert(const char*, u32, const char*)
ext 0273B0D4 void* allocateSequenceArray(u32, JKRHeap*, s32)
ext 028EFF8C JAInter::HeapBlock* constructSequenceArray(void*, u32, u32, SequenceElementConstructor, u32)
ext 028F026C void registerSequenceStatics(AudioDestructorRecord*)
*/

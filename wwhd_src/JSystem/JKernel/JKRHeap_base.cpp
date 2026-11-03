/** JKRHeap_base platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JKernel/JKRHeap_base.h"
static JKRHeap_baseStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 027EC404 */
static void __sinit_JKRHeap_base_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JKRHeap_baseConstructRegister(&sStaticRegister);
}


static JKRHeap_l* sSystemHeap;
/* 027EC0B4 */
void* JKRHeap_l::allocFromSystemHeap(u32 size,s32 alignment) {
    JKRHeap_l* heap = sSystemHeap;
    return gabi::call_ptr<void*>(heap->mVtable->mAlloc,heap,size,alignment);
}
/* 027EC0D8 */
void* JKRHeap_l::alloc(u32 size,s32 alignment) {
    JKRHeap_l* heap = this;
    if (heap == NULL) return JKRHeapFallbackAlloc(size,alignment);
    return gabi::call_ptr<void*>(heap->mVtable->mAlloc,heap,size,alignment);
}
/* 027EC104 */
void JKRHeap_l::free(void* memory) {
    JKRHeap_l* heap = this;
    if (heap == NULL) JKRHeapFallbackFree(memory);
    else gabi::call_ptr<void>(heap->mVtable->mFree,heap,memory);
}
/* 027EC1FC */
u32 JKRHeap_l::getTotalFreeSize() {
    JKRHeap_l* heap = this;
    return gabi::call_ptr<u32>(heap->mVtable->mSize,heap);
}
/* 027EC20C */
JKRHeap_l* JKRHeap_l::becomeSystemHeap() {
    JKRHeap_l* heap = this;
    JKRHeap_l* previous = sSystemHeap;
    sSystemHeap = heap;
    return previous;
}
/* 027EC3D4 */
u32 JKRHeap_l::dump() {
    JKRHeap_l* heap = this;
    gabi::call_ptr<void>(heap->mVtable->mDump,heap);
    return 1;
}


static u32 sHeapTypeMarker;
static JKRHeapTypeInfoRef sExpHeapType;
static JKRHeapTypeInfoRef sSolidHeapType;
static JKRHeapTypeInfoRef sUnitHeapType;
static u32 sExpHeapTypeInitialized;
static u32 sSolidHeapTypeInitialized;
static u32 sUnitHeapTypeInitialized;
/* 027EC23C */
u32 JKRHeap_l::getHeapType() {
    JKRHeap_l* heap = this;
    // HD: the platform heap's runtime-type query replaces GC getHeapType virtuals.
    if (sExpHeapTypeInitialized == 0) {
        sExpHeapTypeInitialized = 1;
        sExpHeapType.mMarker = &sHeapTypeMarker;
    }
    if (heap != NULL && gabi::call_ptr<u32>(heap->mVtable->mIsKind,heap,&sExpHeapType) != 0 && heap != NULL)
        return HEAP_TYPE_EXP;
    if (sSolidHeapTypeInitialized == 0) {
        sSolidHeapType.mMarker = &sHeapTypeMarker;
        sSolidHeapTypeInitialized = 1;
    }
    if (heap != NULL && gabi::call_ptr<u32>(heap->mVtable->mIsKind,heap,&sSolidHeapType) != 0 && heap != NULL)
        return HEAP_TYPE_SOLID;
    if (sUnitHeapTypeInitialized == 0) {
        sUnitHeapType.mMarker = &sHeapTypeMarker;
        sUnitHeapTypeInitialized = 1;
    }
    if (heap != NULL && gabi::call_ptr<u32>(heap->mVtable->mIsKind,heap,&sUnitHeapType) != 0 && heap != NULL)
        return HEAP_TYPE_UNIT;
    return 0;
}

/* WWHD glue
 data sHeapTypeMarker 1016DC78
 data sExpHeapType 101FDF98
 data sSolidHeapType 101FDFA0
 data sUnitHeapType 101FDFA4
 data sExpHeapTypeInitialized 101FD764
 data sSolidHeapTypeInitialized 101FD76C
 data sUnitHeapTypeInitialized 101FD770
 data sSystemHeap 104A244C
 ext 027EC0AC void* JKRHeapFallbackAlloc(u32,s32)
 ext 027EC0B0 void JKRHeapFallbackFree(void*)
 data sStaticBlock 104A243C
 data sStaticRegister 101F978C
 ext 028F026C void JKRHeap_baseConstructRegister(void*)
*/

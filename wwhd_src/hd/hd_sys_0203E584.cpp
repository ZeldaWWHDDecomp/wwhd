/** HD root-heap setup and accessors. No GameCube counterpart.
 * Private decomp source; original heap accessor names are unknown. */
#include "bindings.h"
#include "hd/hd_sys_0203E584.h"

static hdRootHeapRegistry sRootHeaps;
static u32 sSeadRootHeapCount;
static hdSeadRootHeapArray sSeadRootHeaps;
enum { ROOT_HEAP_COUNT = 13 };

/* 0203E584 */
static void resetRootHeaps() {
    for (int i = 0; i < ROOT_HEAP_COUNT; i++) {
        sRootHeaps.mHeaps[i] = NULL;
    }
}

/* 0203E5F4 */
static void getRootHeapSizes(hdRootHeapSizes* sizes) {
    const u32 heapSizes[ROOT_HEAP_COUNT] = {
        0x04000000, 0x01100000, 0x01100000, 0x01400000, 0x00400000,
        0x1F200000, 0x08000000, 0x00100000, 0x00A00000, 0x03000000,
        0x00F00000, 0x00200000, 0x00100000,
    };
    for (int i = 0; i < ROOT_HEAP_COUNT; i++) {
        sizes->mSizes[i] = heapSizes[i];
    }
}

/* 0203E67C */
static void* createRootHeap(u32 size, hdRootHeapName* name) {
    void* parent = NULL;
    if (sSeadRootHeapCount != 0) {
        parent = sSeadRootHeaps[0];
    }
    return hdCreateExpandedHeap(size, name, parent, 1, 1);
}

/* 0203E6A4 */
static void createRootHeaps() {
    struct HeapEntry {
        u32 mSizeIndex;
        u32 mNameAddress;
    };
    const HeapEntry entries[ROOT_HEAP_COUNT] = {
        {0, 0x10006320}, {1, 0x10006350}, {2, 0x10006360},
        {3, 0x10006330}, {4, 0x10006374}, {6, 0x10006390},
        {5, 0x1000639C}, {7, 0x100063A8}, {8, 0x100063B8},
        {9, 0x100063C8}, {10, 0x10006310}, {11, 0x100063D8},
        {12, 0x10006340},
    };
    hdRootHeapName name;
    hdRootHeapSizes sizes;
    getRootHeapSizes(&sizes);
    for (int i = 0; i < ROOT_HEAP_COUNT; i++) {
        // HD: sead safe-string representation; size entries five and six are exchanged.
        name.mVtable = 0x100062F8;
        name.mText = STR(entries[i].mNameAddress);
        sRootHeaps.mHeaps[i] = createRootHeap(sizes.mSizes[entries[i].mSizeIndex], &name);
    }
}

/* 0203E8AC */
static void* getFirstRootHeap() {
    return sRootHeaps.mHeaps[0];
}

/* 0203E8B8 */
static void* getSecondRootHeap() {
    return sRootHeaps.mHeaps[1];
}

/* 0203E8C4 */
static void* getThirdRootHeap() {
    return sRootHeaps.mHeaps[2];
}

/* 0203E8D0 */
static void* getFourthRootHeap() {
    return sRootHeaps.mHeaps[3];
}

/* 0203E8DC */
static void* getFifthRootHeap() {
    return sRootHeaps.mHeaps[4];
}

/* 0203E8E8 */
static void* getSixthRootHeap() {
    return sRootHeaps.mHeaps[5];
}

/* 0203E8F4 */
static void* getSeventhRootHeap() {
    return sRootHeaps.mHeaps[6];
}

/* 0203E900 */
static void* getEighthRootHeap() {
    return sRootHeaps.mHeaps[7];
}

/* 0203E90C */
static void* getNinthRootHeap() {
    return sRootHeaps.mHeaps[8];
}

/* 0203E918 */
static void* getTenthRootHeap() {
    return sRootHeaps.mHeaps[9];
}

/* 0203E924 */
static void* getThirteenthRootHeap() {
    return sRootHeaps.mHeaps[12];
}

/* 0203E930 */
static void __sinit_hd_root_heap_cpp() {
    sinit_header_statics_rodata(0x1046124C, 0x1018F53C, 0x10461258, 0x100063EC);
}

/* 0203E9C4 */
static void destroyRootHeapCompanion(void* i_this, u32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 0203E9D8 */
static void rootHeapCompanionEmpty() {}

/* 0203E9DC */
// HD: secondary SystemTask base at +0x10; retain wrapping guest-pointer adjustment.
static void destroySystemTaskSecondary(void* i_this, u32 flags) {
    hdDestroySystemTask(gabi::at<void>(gabi::ea(i_this) - 0x10), flags);
}

/* 0203E9E4 */
// HD: the other SystemTask base is at +0x14.
static void destroySystemTaskOtherBase(void* i_this, u32 flags) {
    hdDestroySystemTaskBase(gabi::at<void>(gabi::ea(i_this) - 0x14), flags);
}

/* WWHD glue (tools/verify/autoglue.py): private HD globals and calls
 data sRootHeaps 10461268
 data sSeadRootHeapCount 104A030C
 data sSeadRootHeaps 104A0314
 ext 02753004 void* hdCreateExpandedHeap(u32, hdRootHeapName*, void*, s32, s32)
 ext 0203F8CC void hdDestroySystemTask(void*, u32)
 ext 0203F92C void hdDestroySystemTaskBase(void*, u32)
*/

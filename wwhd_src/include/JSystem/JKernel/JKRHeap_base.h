#pragma once
#include "wwhd.h"
struct JKRHeap_baseStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(JKRHeap_baseStaticBlock,16);
void JKRHeap_baseConstructRegister(void*);


struct JKRHeapVtable_l {
    be<u32> mType[2]; be<u32> mDestructor[2]; be<u32> mIsKindThis; be<u32> mIsKind;
    u8 mPaddingAlloc[0x1c]; be<u32> mAlloc;
    be<u32> mFreeThis; be<u32> mFree;
    u8 mPaddingSize[0x34]; be<u32> mSize;
    u8 mPaddingDump[0x2c]; be<u32> mDump;
};
class JKRHeap_l {
public:
    static void* allocFromSystemHeap(u32,s32);
    void* alloc(u32,s32);
    void free(void*);
    u32 getTotalFreeSize();
    u32 getHeapType();
    JKRHeap_l* becomeSystemHeap();
    u32 dump();
    static JKRHeap_l* getSystemHeap();
    u8 mHeader[0xc]; gptr<JKRHeapVtable_l> mVtable;
};
WWHD_OFFSET(JKRHeap_l,mVtable,0xc);
WWHD_SIZE(JKRHeap_l,0x10);
WWHD_OFFSET(JKRHeapVtable_l,mIsKind,0x14);
WWHD_OFFSET(JKRHeapVtable_l,mAlloc,0x34);
WWHD_OFFSET(JKRHeapVtable_l,mFree,0x3c);
WWHD_OFFSET(JKRHeapVtable_l,mSize,0x74);
WWHD_OFFSET(JKRHeapVtable_l,mDump,0xa4);
WWHD_SIZE(JKRHeapVtable_l,0xa8);
void* JKRHeapFallbackAlloc(u32,s32);
void JKRHeapFallbackFree(void*);



enum JKRHeapType { HEAP_TYPE_EXP = 0x45585048, HEAP_TYPE_SOLID = 0x534c4944, HEAP_TYPE_UNIT = 0x554e4954 };


struct JKRHeapTypeInfoRef { gptr<void> mMarker; };
WWHD_SIZE(JKRHeapTypeInfoRef,4);

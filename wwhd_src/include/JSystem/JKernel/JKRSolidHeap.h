#pragma once
#include "wwhd.h"
struct JKRSolidHeapStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(JKRSolidHeapStaticBlock,16);
void JKRSolidHeapConstructRegister(void*);


struct JKRSolidHeap_l {
    static JKRSolidHeap_l* create(u32,void*);
    void* adjustSize();
    u8 mPrefix[0x90];
    be<u32> mFlags;
};
struct JKRSolidHeapName { gptr<const char> mName; gptr<void> mVtable; };
WWHD_OFFSET(JKRSolidHeap_l,mFlags,0x90);
WWHD_SIZE(JKRSolidHeap_l,0x94);
WWHD_SIZE(JKRSolidHeapName,8);
JKRSolidHeap_l* JKRSolidHeapCreateAdapter(u32,JKRSolidHeapName*,void*,u32,u32);
void* JKRSolidHeapAdjustAdapter(JKRSolidHeap_l*);

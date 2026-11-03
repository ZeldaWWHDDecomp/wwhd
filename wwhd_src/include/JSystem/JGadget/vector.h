#pragma once
#include "wwhd.h"
struct vectorStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(vectorStaticBlock,16);
void vectorConstructRegister(void*);


namespace JGadget {
struct TVector_pointer {
    be<u32>* insert(be<u32>*,const be<u32>*);
    be<u32> mAllocator;
    gptr<be<u32>> mBegin;
    gptr<be<u32>> mEnd;
    be<u32> mCapacity;
    be<u32> mExtend;
};
}
WWHD_OFFSET(JGadget::TVector_pointer,mBegin,4);
WWHD_OFFSET(JGadget::TVector_pointer,mEnd,8);
WWHD_OFFSET(JGadget::TVector_pointer,mCapacity,0xc);
WWHD_OFFSET(JGadget::TVector_pointer,mExtend,0x10);
WWHD_SIZE(JGadget::TVector_pointer,0x14);
u32 JGadgetVectorExtendDefault(u32,u32,u32);

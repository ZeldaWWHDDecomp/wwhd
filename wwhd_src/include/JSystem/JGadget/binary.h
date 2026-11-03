#pragma once
#include "wwhd.h"
struct binaryStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(binaryStaticBlock,16);
void binaryConstructRegister(void*);


namespace JGadget { namespace binary {
struct TParseVtable {
    be<u32> mType[2];
    be<u32> mDestructor[2];
    be<u32> mCheckThis;
    be<u32> mCheck;
    be<u32> mBlockThis;
    be<u32> mBlock;
};
class TParse_header_block {
public:
    u32 parse_next(gptr<const void>*,u32);
    gptr<TParseVtable> mVtable;
};
struct VariableUInt {
    be<u16> mFirst;
    be<u16> mSecond;
    be<u32> mFollowing;
};
const void* parseVariableUInt_16_32_following(const VariableUInt*,be<u32>*,be<u32>*,be<u32>*);
} }
WWHD_SIZE(JGadget::binary::TParseVtable,0x20);
WWHD_OFFSET(JGadget::binary::TParse_header_block,mVtable,0);
WWHD_SIZE(JGadget::binary::TParse_header_block,4);
WWHD_SIZE(JGadget::binary::VariableUInt,8);

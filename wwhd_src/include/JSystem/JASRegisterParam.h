/* Audio register parameters, tww names/layout. */
#pragma once
#include "wwhd.h"
namespace JASystem {
struct TRegisterParam {
    be<u16> field_0x0[6];
    union {
        be<u16> field_0xc;
        struct {
            be<u8> bank, program;
        } mBankProgram;
    };
    be<u16> field_0xe;
    be<u16> mPanPower[5];
    be<u16> field_0x1a;
    be<s32> field_0x1c;
    be<u32> field_0x20[4];
    void init();
    u8 getBankNumber();
    u8 getProgramNumber();
    void inherit(const TRegisterParam*);
};
WWHD_OFFSET(TRegisterParam, field_0xc, 0xC);
WWHD_OFFSET(TRegisterParam, mPanPower, 0x10);
WWHD_OFFSET(TRegisterParam, field_0x20, 0x20);
WWHD_SIZE(TRegisterParam, 0x30);
} // namespace JASystem
void registerParamStatic(void*);

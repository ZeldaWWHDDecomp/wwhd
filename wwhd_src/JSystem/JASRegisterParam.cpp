/* Audio parameter register setup.
 * HD: preserve interleaved defaults and inherit order for overlapping source ranges. */
#include "JSystem/JASRegisterParam.h"
static u32 l_firstLow;
static u32 l_firstHigh;
static u32 l_secondLow;
static u32 l_secondHigh;
struct RegisterRecord {
    u8 bytes[1];
};
static RegisterRecord l_record;
/* 028457C0 */
void JASystem::TRegisterParam::init() {

    field_0x20[3] = 0;
    field_0xc = 0xF0;
    field_0xe = 12;
    mPanPower[2] = 1;
    field_0x20[0] = 0;
    mPanPower[3] = 0x7FFF;
    field_0x1a = 0x40;
    field_0x0[1] = 0;
    mPanPower[1] = 1;
    field_0x0[2] = 0;
    mPanPower[4] = 0x4000;
    field_0x0[3] = 0;
    field_0x20[2] = 0;
    field_0x0[4] = 0;
    mPanPower[0] = 0;
    field_0x0[5] = 0;
    field_0x0[0] = 0;
    field_0x20[1] = 0;
}

/* 02845828 */
u8 JASystem::TRegisterParam::getBankNumber() { return u8(field_0xc >> 8); }

/* 02845834 */
u8 JASystem::TRegisterParam::getProgramNumber() { return mBankProgram.program; }

/* 02845840 */
void JASystem::TRegisterParam::inherit(const TRegisterParam* source) {

    field_0x0[1] = 0;
    field_0x0[2] = 0;
    field_0x0[3] = 0;
    field_0x0[4] = 0;
    field_0x0[5] = 0;
    field_0x0[0] = 0;
    field_0xc = source->field_0xc;
    field_0xe = source->field_0xe;
    field_0x1a = source->field_0x1a;
    for (u32 i = 0; i < 5; ++i)
        mPanPower[i] = source->mPanPower[i];
    field_0x20[2] = 0;
    field_0x20[1] = 0;
    field_0x20[3] = 0;
    field_0x20[0] = 0;
}

/* 028458AC */
static void __sinit_JASRegisterParam_cpp() {
    l_firstLow = 0;
    l_secondLow = 0;
    l_firstHigh = 0;
    l_secondHigh = 0;
    registerParamStatic(&l_record);
}

// clang-format off
/* WWHD glue
 data l_firstLow 104B665C
 data l_firstHigh 104B6660
 data l_secondLow 104B6654
 data l_secondHigh 104B6658
 data l_record 101FC6FC
 ext 028F026C void registerParamStatic(void*)
*/

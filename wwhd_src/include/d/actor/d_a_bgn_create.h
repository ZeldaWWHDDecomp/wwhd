#pragma once
#include "d/actor/d_a_bgn.h"
#include "gc_api/f_op/f_op_actor_mng.h"

inline f32 REG8_F(int index) { return REG_F(8, index); }

// HD: light cache blocks and padding are excluded from tev assignment.
struct BgnCreateTev {
    be<u8> mLight0[0x44];
    u8 mHdLightCache0[0x40];
    be<u8> mParameters[0x39];
    u8 mPadding[3];
    be<u8> mLight1[0x44];
    u8 mHdLightCache1[0x40];
    be<u8> mLight2[0x44];
    u8 mHdLightCache2[0x40];
};
WWHD_SIZE(BgnCreateTev, 0x1C8);
WWHD_OFFSET(BgnCreateTev, mParameters, 0x84);
WWHD_OFFSET(BgnCreateTev, mLight1, 0xC0);
WWHD_OFFSET(BgnCreateTev, mLight2, 0x144);
struct BgnCreateSmoke {
    u8 mPrefix[0x12];
    be<u8> field_0x12;
    u8 mTail[0xD];
    void setFollowOff() { field_0x12 = 1; }
};
WWHD_SIZE(BgnCreateSmoke, 0x20);
struct BgnCreateActors {
    gptr<bgn_class> bgn;
    gptr<void> bgn2;
    gptr<void> bgn3;
};
enum BgnCreateActorName {
    fpcNm_HIMO3_e = 0x1BF,
    fpcNm_KUI_e = 0xFA,
    fpcNm_ATT_e = 0x1C0,
    fpcNm_BGN2_e = 0xF4,
    fpcNm_BGN3_e = 0xF5,
};
enum BgnCreateSound { JA_BGM_BGN_KUGUTSU = 0x80000059 };
bgn_class* BgnCreate_construct(bgn_class*);
// HD: useHeapInit is static in another translation unit; its guest callback address
// uses bindings.h heapCallbackFunc until autoglue registers external callbacks.
constexpr heapCallbackFunc useHeapInit = 0x0207FE14;

struct BgnCreateSave { u8 mPrefix[0x20]; dSv_info_c mSave; };
struct BgnCreateSavePointer { gptr<BgnCreateSave> info; };

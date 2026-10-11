// Grandma fairy tag: WWHD actor/HIO layouts.
#pragma once
#include "gc_api/gc_api.h"
struct daTag_Ba1_HIO_c {
    struct hio_prm_c { be<u8> field_0x0; };
    /* 0x0 */ be<s8> mNo;
    /* 0x1 */ u8 _1[3];
    /* 0x4 */ be<s32> mRefCount;
    /* 0x8 */ hio_prm_c mPrm;
    /* 0x9 */ u8 _9[3];
    /* 0xC */ be<u32> __vtbl;
};
WWHD_OFFSET(daTag_Ba1_HIO_c, __vtbl, 0xC);

class daTag_Ba1_c : public fopAc_ac_c {
public:
    daTag_Ba1_c() { fopAc_ac_c_ct(this); __vtbl = 0x1003EF5C; } // HD: virtual destructor

    s16 XyCheck_cB(int);
    s16 XyEvent_cB(int);
    bool createInit();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();

    /* 0x3AC */ be<s16> mEventIds[1];
    /* 0x3AE */ be<s16> mEventIdx;
};
WWHD_OFFSET(daTag_Ba1_c, mEventIdx, 0x3AE);
WWHD_SIZE(daTag_Ba1_c, 0x3B0);


enum { dItemNo_FAIRY_BOTTLE_e=0x57, UNK_0520=0x0520, GRANDMA_HEALED=0x2A20 };

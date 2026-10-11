#pragma once
#include "gc_api/gc_api.h"
/* HD: GHS lays out the vtable pointer after the members (+0xC); size 0x10 */
struct daTag_So_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ u8 m06[0xC - 0x2];
    /* 0x0C */ be<u32> __vtbl;
};
WWHD_SIZE(daTag_So_HIO_c, 0x10);

struct daTag_So_c : fopAc_ac_c {
    daTag_So_c();
    enum : u32 { __vt = 0x1003FC64 };
    bool _execute();
    void debugDraw();
    bool _draw();
    void getArg();
    cPhs_State _create();
    bool _delete();

    /* 0x3AC */ be<u8> mRndNum;
    /* 0x3AD */ u8 _3AD[3];
    /* 0x3B0 */ be<f32> mJumpRange;
    /* 0x3B4 */ be<u8> mType;
    /* 0x3B5 */ u8 m299[0x3C0 - 0x3B5];
};
WWHD_OFFSET(daTag_So_c, mJumpRange, 0x3B0);
WWHD_OFFSET(daTag_So_c, mType, 0x3B4);

WWHD_SIZE(daTag_So_c, 0x3C0);
void dLib_debugDrawFan(cXyz*, s16, s16, f32, const GXColor*);
void tag_so_actor_dt(void*, s32);

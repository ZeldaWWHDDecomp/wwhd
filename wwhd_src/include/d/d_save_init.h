/** Header statics emitted by the HD compiler. */
#pragma once
#include "gc_api/gc_api.h"

struct dHeaderStatics_c {
    /* 0x00 */ be<f32> mFloatPair[2];
    /* 0x08 */ be<u8> mFlag;
    /* 0x09 */ be<u8> mByte;
    u8 padding[2];
    /* 0x0C */ be<u32> mZeroWords[4];
};
struct dHeaderStaticRecord_c { u8 opaque[12]; };
struct dHeaderStaticRecords_c {
    dHeaderStaticRecord_c mFirst, mSecond, mThird;
};
WWHD_OFFSET(dHeaderStatics_c, mFlag, 8);
WWHD_OFFSET(dHeaderStatics_c, mZeroWords, 12);
WWHD_SIZE(dHeaderStatics_c, 0x1C);
WWHD_SIZE(dHeaderStaticRecord_c, 12);
WWHD_SIZE(dHeaderStaticRecords_c, 36);

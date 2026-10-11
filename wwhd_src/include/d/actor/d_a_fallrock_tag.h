/* Falling-rock spawner tag, WWHD layout.
 * GameCube -> WWHD: actor base grows by 0x11C; member offsets move by that amount.
 * HD folds the data table into constants and has a C++ virtual destructor. */
#pragma once
#include "gc_api/gc_api.h"

struct daFallRockTag_Data_c {
    f32 mRange;
    f32 mScaleMin;
    f32 mScaleMax;
    s16 mTimerOffset;
    s16 mRockNumPerSec;
};

class daFallRockTag_c : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x1000E8C4 };
    daFallRockTag_c();
    inline cPhs_State create();
    inline BOOL draw();
    inline BOOL execute();
    void createRock(cXyz*, cXyz*, csXyz*, int, u32);
    const daFallRockTag_Data_c* getData() { return &m_data; }
    static const daFallRockTag_Data_c m_data;
    static const f32 m_div_num;

    /* 0x3AC */ u8 _3AC[8];
    /* 0x3B4 */ be<s32> field_0x298;
    /* 0x3B8 */ u8 _3B8[2];
    /* 0x3BA */ be<u8> mSchBit;
    /* 0x3BB */ u8 _3BB;
};
WWHD_OFFSET(daFallRockTag_c, field_0x298, 0x3B4);
WWHD_OFFSET(daFallRockTag_c, mSchBit, 0x3BA);
WWHD_SIZE(daFallRockTag_c, 0x3BC);

// HD: this unit uses only the schedule byte of stage information.
struct FallRockStageInfo {
    /* 0x00 */ u8 _00[0xC];
    /* 0x0C */ be<u32> mStageTypeAndSchbit;
};
WWHD_OFFSET(FallRockStageInfo, mStageTypeAndSchbit, 0xC);
WWHD_SIZE(FallRockStageInfo, 0x10);
inline u8 fallRock_getScheduleSeconds(void* info) {
    return ((FallRockStageInfo*)info)->mStageTypeAndSchbit & 0xFF;
}
u8 dKy_get_schbit();
s32 dKy_get_schbit_timer();

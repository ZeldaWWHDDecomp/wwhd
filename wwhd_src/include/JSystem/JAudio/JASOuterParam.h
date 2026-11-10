/* JASystem::TTrack::TOuterParam, WWHD layout. Only the nested parameter class is
 * emitted by this unit, so its short GameCube name is used here. */
#pragma once
#include "gc_api/gc_api.h"

enum JASOuterParamFlag {
    OUTERPARAM_Volume = 0x01,
    OUTERPARAM_Pitch = 0x02,
    OUTERPARAM_Fxmix = 0x04,
    OUTERPARAM_Pan = 0x08,
    OUTERPARAM_Dolby = 0x10,
    OUTERPARAM_Tempo = 0x40,
    OUTERPARAM_FIR8Filter = 0x80,
};
class TOuterParam {
public:
    TOuterParam();
    void initExtBuffer();
    // HD: full argument registers remain live, unlike tww's u16/u8 declarations.
    void setOuterSwitch(u32 param_1);
    u32 checkOuterSwitch(u32 param_1);
    void setOuterUpdate(u32 param_1);
    u32 getOuterUpdate();
    s32 getIntFirFilter(u32 param_1);
    void setParam(u32 param_1, f32 param_2);
    void onSwitch(u32 param_1);
    void setFirFilter(be<s16>* param_1);
    /* 0x00 */ be<u16> mOuterSwitch;  /* was field_0x0: setOuterSwitch 02816C68 / checkOuterSwitch 02816C70: the OUTERPARAM_* flags that are switched on */
    /* 0x02 */ be<u16> mOuterUpdate;  /* was field_0x2: setOuterUpdate 02816C84 / getOuterUpdate 02816C8C: the OUTERPARAM_* flags to update */
    /* 0x04 */ be<f32> mVolume;
    /* 0x08 */ be<f32> mPitch;
    /* 0x0C */ be<f32> mFxmix;
    /* 0x10 */ be<f32> mDolby;
    /* 0x14 */ be<f32> mPan;
    /* 0x18 */ be<f32> mTempo;
    /* 0x1C */ be<s16> mFirFilter[8];  /* was field_0x1c: setFirFilter 02816D7C / getIntFirFilter 02816C94: 8 FIR8 filter words; initExtBuffer 02816C58 clears */
};
WWHD_OFFSET(TOuterParam, mOuterSwitch, 0x00);
WWHD_OFFSET(TOuterParam, mOuterUpdate, 0x02);
WWHD_OFFSET(TOuterParam, mVolume, 0x04);
WWHD_OFFSET(TOuterParam, mPitch, 0x08);
WWHD_OFFSET(TOuterParam, mFxmix, 0x0C);
WWHD_OFFSET(TOuterParam, mDolby, 0x10);
WWHD_OFFSET(TOuterParam, mPan, 0x14);
WWHD_OFFSET(TOuterParam, mTempo, 0x18);
WWHD_OFFSET(TOuterParam, mFirFilter, 0x1C);
WWHD_SIZE(TOuterParam, 0x2C);

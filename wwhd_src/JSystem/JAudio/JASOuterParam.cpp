/**
 * JASOuterParam.cpp - Audio track external parameter buffer.
 * WWHD, verified against the original; GameCube names/layout from zeldaret/tww.
 */
#include "JSystem/JAudio/JASOuterParam.h"

static const f32 zero = 0.0f;

/* 02816BE0 */
TOuterParam::TOuterParam() {
    // HD: GHS captures the shared zero before any field is written.
    f32 value = zero;
    field_0x0 = 0;
    field_0x2 = 0;
    mVolume = value;
    mPitch = value;
    mFxmix = value;
    mDolby = value;
    mPan = value;
    mTempo = value;
    for (u32 i = 0; i < 8; i++) {
        field_0x1c[i] = 0;
    }
}

/* 02816C58 */
void TOuterParam::initExtBuffer() {
    field_0x0 = 0;
    field_0x2 = 0;
}

/* 02816C68 */
void TOuterParam::setOuterSwitch(u32 param_1) {
    field_0x0 = (u16)param_1;
}

/* 02816C70 */
u32 TOuterParam::checkOuterSwitch(u32 param_1) {
    return (field_0x0 & param_1) != 0;
}

/* 02816C84 */
void TOuterParam::setOuterUpdate(u32 param_1) {
    field_0x2 = (u16)param_1;
}

/* 02816C8C */
u32 TOuterParam::getOuterUpdate() {
    return field_0x2;
}

/* 02816C94 */
s32 TOuterParam::getIntFirFilter(u32 param_1) {
    return field_0x1c[param_1];
}

/* 02816CA4 */
void TOuterParam::setParam(u32 param_1, f32 param_2) {
    be<f32>* var1;
    switch (param_1) {
    case OUTERPARAM_Volume:
        var1 = &mVolume;
        break;
    case OUTERPARAM_Pitch:
        var1 = &mPitch;
        break;
    case OUTERPARAM_Fxmix:
        var1 = &mFxmix;
        break;
    case OUTERPARAM_Pan:
        var1 = &mPan;
        break;
    case OUTERPARAM_Dolby:
        var1 = &mDolby;
        break;
    case OUTERPARAM_Tempo:
        var1 = &mTempo;
        break;
    default:
        return;
    }
    // HD: capture the update mask before the parameter write (the inputs may alias).
    u16 update = field_0x2;
    *var1 = param_2;
    field_0x2 = (u16)(update | param_1);
}

/* 02816D60 */
void TOuterParam::onSwitch(u32 param_1) {
    // HD: both masks are loaded before either store.
    u16 state = field_0x0;
    u16 update = field_0x2;
    field_0x0 = (u16)(state | param_1);
    field_0x2 = (u16)(update | param_1);
}

/* 02816D7C */
void TOuterParam::setFirFilter(be<s16>* param_1) {
    // HD: capture both masks before storing the update mask followed by the switch mask.
    u16 update = field_0x2;
    u16 state = field_0x0;
    field_0x2 = (u16)(update | OUTERPARAM_FIR8Filter);
    field_0x0 = (u16)(state | OUTERPARAM_FIR8Filter);
    for (u32 i = 0; i < 8; i++) {
        field_0x1c[i] = (s16)param_1[i];
    }
}

/* WWHD glue (tools/verify/autoglue.py)
data zero 10170520
*/

// d_a_dai_item.h (WWHD) - stand item actor layout. 
#pragma once
#include "bindings.h"
struct daStandItem_c : fopAc_ac_c {
    request_of_phase_process_class mPhsDai, mPhsCloth;
    gptr<J3DModel> mpModel;
    gptr<u8> mpBckAnm;
    dCcD_Stts mStts;
    dCcD_Cyl mCyl;
    dBgS_ObjAcch mAcch;
    dBgS_AcchCir mAcchCir;
    be<u8> mItemNo, mItemType;
    u8 pad736[2];
    be<s32> mTimer, mMode;
    be<u8> mCarry;
    u8 pad741[3];
    be<s16> mBckPlayTimer, mBckStopTimer;
    be<f32> mBckSpeed;
    be<f32> mBaseEmitterMatrix[12], mHeadEmitterMatrix[12];
    gptr<u8> mIdleEmitter, mHeadEmitter, mBaseEmitter, mpCloth;
    be<u8> mClothType;
    u8 pad7BD;
    be<s16> mWindAngle;
    cXyz mWindVector;
    be<u8> mbBckDidPlay, mPreviousBckDidPlay;
    be<s16> mRotationSpeed, mRotation;
    u8 pad7D2[6];
    be<f32> mWindPower;
    u8 pad7DC[4];
    be<f32> mSpinPower;
};
WWHD_OFFSET(daStandItem_c, mPhsDai, 0x3AC);
WWHD_OFFSET(daStandItem_c, mpModel, 0x3BC);
WWHD_OFFSET(daStandItem_c, mStts, 0x3C4);
WWHD_OFFSET(daStandItem_c, mCyl, 0x400);
WWHD_OFFSET(daStandItem_c, mAcch, 0x530);
WWHD_OFFSET(daStandItem_c, mAcchCir, 0x6F4);
WWHD_OFFSET(daStandItem_c, mClothType, 0x7BC);
WWHD_OFFSET(daStandItem_c, mWindVector, 0x7C0);
WWHD_OFFSET(daStandItem_c, mItemNo, 0x734);
WWHD_OFFSET(daStandItem_c, mMode, 0x73C);
WWHD_OFFSET(daStandItem_c, mBckPlayTimer, 0x744);
WWHD_OFFSET(daStandItem_c, mbBckDidPlay, 0x7CC);
WWHD_OFFSET(daStandItem_c, mSpinPower, 0x7E0);
WWHD_SIZE(daStandItem_c, 0x7E4);

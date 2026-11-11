/**
 * HUD map control, draw/delete lifetime and HIO configuration.
 * The map-control part covers seven functions and the layout header.
 * The weapon range adds HIO/static setup, overlay drawing and button transitions.
 * HD differences preserve the verified call order, allocating constructors and expanded HUD state.
 */
#include "d/d_meter_01.h"
#include <initializer_list>
static MeterHIOFloats l_hioValues;
static MeterHDState l_hdState;
static u32 sMainParts1;
static u32 sMainParts2;
static u32 sMainParts3;
static u32 sScrTimer1;
static u32 sScrTimer2;
static u32 sChoiceRoad;
static u32 moveItemPane;
static u32 dMeter_windID;
static u8 rupy_soundOnFlag;
static u8 rupy_soundSetFlag;
static u8 dMeter_windStatus;
static dMeter_map_HIO_c g_meter_mapHIO;
static MeterMapState l_mapState;
static f32 l_iconFreeScale;
static f32 l_iconSelfScale;
static f32 l_zero;
static f32 l_one;
static f32 l_half;
static f32 l_mapRange;
static f32 l_createScreenWidth;
static f32 l_createScreenHeight;
static f32 l_createEnemyRange;
static f32 l_createEnemyBase;
static f32 l_createBlinkBase;
static f32 l_createMagicScale;
static f32 l_createMagicUnit;
static f32 l_createMagicMaximum;
static u32 l_createHeartTags[20] = {0x68743030, 0x68743031, 0x68743032, 0x68743033, 0x68743034, 0x68743035, 0x68743036, 0x68743037, 0x68743038, 0x68743039, 0x68743130, 0x68743131, 0x68743132, 0x68743133, 0x68743134, 0x68743135, 0x68743136, 0x68743137, 0x68743138, 0x68743139};
static u32 l_createHeartShadowTags[20] = {0x686B3030, 0x686B3031, 0x686B3032, 0x686B3033, 0x686B3034, 0x686B3035, 0x686B3036, 0x686B3037, 0x686B3038, 0x686B3039, 0x686B3130, 0x686B3131, 0x686B3132, 0x686B3133, 0x686B3134, 0x686B3135, 0x686B3136, 0x686B3137, 0x686B3138, 0x686B3139};

// HD: retain the allocating GHS constructor, zeroing and store order.
/* 025974DC */
static u32 dMeter_HIO_c__ct(u32 self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!self) {
        self = meter01_allocate(0x158);
        if (!self)
            return 0;
    }
    dMeter_HIO_c* i_this = gabi::at<dMeter_HIO_c>(self);
    i_this->mCompassFadeFrames = 0x0;
    i_this->mBossLifeRatioDefault = l_zero;
    i_this->mVtable = 0x100510b4;
    i_this->mReserved6 = 0x0;
    i_this->mHeartScale = l_zero;
    i_this->mScopeWipeAlpha = 0x0;
    i_this->field_0x14 = l_zero;
    i_this->field_0x2e = 0x0;
    i_this->field_0x18 = l_zero;
    i_this->field_0x30 = 0x0;
    i_this->field_0x1c = l_zero;
    i_this->mButtonIconScale = 0x0;
    i_this->field_0x6a = 0x0;
    i_this->mButtonTextScale = 0x0;
    i_this->field_0x42 = 0x0;
    i_this->mButtonOffsetX = 0x0;
    i_this->mBattleLifePosX = l_zero;
    i_this->mReserved8 = 0x0;
    i_this->mButtonScale = 0x0;
    i_this->mButtonOffsetY = 0x0;
    i_this->mBattleHeartScale = l_zero;
    i_this->field_0x54 = 0x0;
    i_this->field_0x38 = 0x0;
    i_this->field_0x56 = 0x0;
    i_this->mHeartLightScale = l_zero;
    i_this->mBattleLifeStepX = l_zero;
    i_this->field_0x36 = 0x0;
    i_this->mBattleLifePosY = l_zero;
    i_this->mScopeWipeMaxScale = l_zero;
    i_this->field_0x60 = 0x0;
    i_this->field_0x34 = 0x0;
    i_this->field_0x61 = 0x0;
    i_this->mShutterBladeAngle = l_zero;
    i_this->mIgnoreArrowNumCount = 0x0;
    i_this->field_0x32 = 0x0;
    i_this->mIgnoreBombNumCount = 0x0;
    i_this->field_0x20 = l_zero;
    i_this->field_0x68 = 0x0;
    i_this->mMapWindArrowOffsetY = 0x0;
    i_this->field_0x2d = 0x0;
    i_this->mMapWindArrowOffsetX = 0x0;
    meter01_clear(&i_this->mClockCloudPosX[0], 0x6);
    meter01_clear(&i_this->mClockCloudPosY[0], 0x6);
    meter01_clear(&i_this->mClockCloudScale[0], 0x6);
    i_this->mReserved86 = 0x0;
    i_this->mKeyLightStep2 = 0x0;
    i_this->mKeyLightStep0 = 0x0;
    i_this->mReserved82 = 0x0;
    i_this->mSwimLightSize = 0x0;
    i_this->mMapCrossKeyPosX = 0x0;
    i_this->mKeyLightStepRange0 = 0x0;
    i_this->mKeyLightStart = 0x0;
    i_this->mKeyLightStartRange = 0x0;
    i_this->mKeyLightStepRange1 = 0x0;
    i_this->mKeyLightEndRange = 0x0;
    i_this->mKeyLightStepRange2 = 0x0;
    i_this->mReserved80 = 0x0;
    i_this->mKeyLightStep1 = 0x0;
    i_this->mMapCrossKeyPosY = 0x0;
    i_this->mReserved84 = 0x0;
    i_this->mKeyLightEndStep = 0x0;
    meter01_clear(&i_this->mButtonPos[0][0][0], 0x8);
    meter01_clear(&i_this->mButtonPos[0][1][0], 0x8);
    meter01_clear(&i_this->mButtonPos[1][0][0], 0x8);
    meter01_clear(&i_this->mButtonPos[1][1][0], 0x8);
    meter01_clear(&i_this->mButtonPos[2][0][0], 0x8);
    meter01_clear(&i_this->mButtonPos[2][1][0], 0x8);
    meter01_clear(&i_this->mButtonPos[3][0][0], 0x8);
    meter01_clear(&i_this->mButtonPos[3][1][0], 0x8);
    meter01_clear(&i_this->mButtonPos[4][0][0], 0x8);
    meter01_clear(&i_this->mButtonPos[4][1][0], 0x8);
    meter01_clear(&i_this->mButtonPos[5][0][0], 0x8);
    meter01_clear(&i_this->mButtonPos[5][1][0], 0x8);
    meter01_clear(&i_this->mButtonPos[6][0][0], 0x8);
    meter01_clear(&i_this->mButtonPos[6][1][0], 0x8);
    meter01_clear(&i_this->mButtonPos[7][0][0], 0x8);
    meter01_clear(&i_this->mButtonPos[7][1][0], 0x8);
    i_this->field_0x12a = 0x0;
    i_this->field_0x13e = 0x0;
    i_this->field_0x121 = 0x0;
    i_this->field_0x128 = 0x0;
    i_this->field_0x122 = 0x0;
    i_this->mSwimLightFadeFrames = 0x0;
    i_this->mSwimBlinkPeriodStart = 0x0;
    i_this->field_0x12c = 0x0;
    i_this->field_0x140 = 0x0;
    i_this->mSwimSoulTurnFrames = 0x0;
    i_this->field_0x138 = 0x0;
    i_this->mShutterFrames = 0x0;
    i_this->field_0x12e = 0x0;
    i_this->field_0x126 = 0x0;
    i_this->mSwimBlinkPeriodEnd = 0x0;
    i_this->mSwimTimerOffsetY = 0x0;
    i_this->field_0x123 = 0x0;
    i_this->mShutterLineAlpha = 0x0;
    i_this->mSwimLightStartY = 0x0;
    meter01_clear(&i_this->field_0x142, 0x4);
    meter01_clear(&i_this->field_0x146, 0x4);
    meter01_clear(&i_this->mSwimLowTimeR, 0x4);
    i_this->field_0x30 = 0x0;
    i_this->mIgnoreBombNumCount = 0x0;
    i_this->field_0x14 = l_zero;
    i_this->field_0x38 = 0x0;
    i_this->field_0x36 = 0x16;
    i_this->mBattleLifePosY = l_hioValues.mValues[6];
    i_this->mBossLifeRatioDefault = l_one;
    i_this->field_0x1c = l_one;
    i_this->field_0x32 = 0x0;
    i_this->mButtonScale = 0x5a;
    i_this->field_0x18 = l_hioValues.mValues[1];
    i_this->mScopeWipeAlpha = 0xff;
    i_this->mHeartLightScale = l_hioValues.mValues[2];
    i_this->field_0x20 = l_hioValues.mValues[3];
    i_this->mIgnoreArrowNumCount = 0x0;
    i_this->mMapWindArrowOffsetX = 0x0;
    i_this->mCompassFadeFrames = 0xa;
    i_this->mBattleHeartScale = l_hioValues.mValues[5];
    i_this->field_0x68 = 0x0;
    i_this->mButtonTextScale = 0x64;
    i_this->field_0x2e = 0x0;
    i_this->mShutterBladeAngle = l_hioValues.mValues[4];
    i_this->mBattleLifeStepX = l_hioValues.mValues[7];
    i_this->field_0x61 = 0x64;
    i_this->field_0x34 = 0x0;
    i_this->field_0x6a = 0x0;
    i_this->mHeartScale = l_hioValues.mValues[0];
    i_this->field_0x42 = 0x64;
    i_this->mButtonOffsetY = 0xfff4;
    i_this->mButtonOffsetX = 0x7;
    i_this->field_0x56 = 0xffa8;
    i_this->mBattleLifePosX = l_hioValues.mValues[6];
    i_this->field_0x2d = 0x8c;
    i_this->mMapWindArrowOffsetY = 0x0;
    i_this->field_0x60 = 0x96;
    i_this->field_0x54 = 0x0;
    i_this->mKeyLightStep0 = 0xf;
    i_this->mButtonIconScale = 0x5a;
    i_this->mKeyLightStepRange2 = 0x3;
    i_this->mSwimSoulTurnFrames = 0x27;
    i_this->mSwimLightStartY = 0xa;
    i_this->field_0x12e = 0x2;
    i_this->field_0x13e = 0x0;
    i_this->mKeyLightEndStep = 0x5;
    i_this->mKeyLightStepRange0 = 0xc;
    i_this->field_0x147 = 0x0;
    i_this->mSwimLowTimeB = 0x32;
    i_this->field_0x123 = 0x1;
    i_this->field_0x143 = 0x0;
    i_this->mKeyLightStartRange = 0x0;
    i_this->mSwimLowTimeR = 0xff;
    i_this->field_0x121 = 0xc8;
    i_this->mSwimTekariSweepFrames = 0xa;
    i_this->mSwimTimerOffsetY = 0x0;
    i_this->mSwimBlinkPeriodStart = 0x14;
    i_this->field_0x144 = 0x0;
    i_this->field_0x122 = 0x96;
    i_this->field_0x149 = 0x0;
    i_this->mButtonPos[0][1][2] = 0x0;
    i_this->mSwimLightFadeFrames = 0x4;
    i_this->mScopeWipeMaxScale = l_hioValues.mValues[8];
    i_this->mKeyLightStep1 = 0x5;
    i_this->mKeyLightStart = 0x14;
    i_this->field_0x145 = 0xff;
    i_this->field_0x148 = 0x0;
    i_this->mSwimBlinkPeriodEnd = 0x3;
    i_this->mSwimLightSize = 0x14;
    i_this->field_0x12c = 0x3;
    i_this->field_0x126 = 0xa;
    i_this->mShutterLineAlpha = 0x8c;
    i_this->field_0x138 = 0xa;
    i_this->field_0x140 = 0x0;
    i_this->mKeyLightEndRange = 0x3;
    i_this->mButtonPos[0][0][0] = 0x0;
    i_this->mKeyLightStep2 = 0x5;
    i_this->field_0x12a = 0x3;
    i_this->mShutterFrames = 0x5;
    i_this->field_0x142 = 0xb4;
    i_this->mButtonPos[0][0][2] = 0x0;
    i_this->field_0x128 = 0x6;
    i_this->mButtonPos[0][0][3] = 0x0;
    i_this->field_0x146 = 0x0;
    i_this->mSwimTekariPause2 = 0x4;
    i_this->mSwimLowTimeG = 0x32;
    i_this->mButtonPos[0][1][0] = 0x0;
    i_this->mButtonPos[2][0][3] = 0x0;
    i_this->mButtonPos[1][0][0] = 0x19;
    i_this->mButtonPos[1][1][0] = 0xffe2;
    i_this->mButtonPos[1][0][1] = 0x23;
    i_this->mButtonPos[1][1][1] = 0xffea;
    i_this->mButtonPos[5][1][0] = 0x0;
    i_this->mButtonPos[3][0][1] = 0x28;
    i_this->mButtonPos[1][0][2] = 0xffdf;
    i_this->mButtonPos[0][1][3] = 0x0;
    i_this->mButtonPos[0][0][1] = 0x3c;
    i_this->mButtonPos[3][1][1] = 0xffc4;
    i_this->mButtonPos[2][0][1] = 0x0;
    i_this->mButtonPos[5][1][3] = 0x0;
    i_this->mButtonPos[1][1][3] = 0x0;
    i_this->mButtonPos[2][0][2] = 0x0;
    i_this->mKeyLightStepRange1 = 0x3;
    i_this->mButtonPos[1][1][2] = 0xfffb;
    i_this->mButtonPos[1][0][3] = 0xfffa;
    i_this->mButtonPos[4][0][0] = 0x19;
    i_this->mButtonPos[6][1][2] = 0xffb8;
    i_this->mButtonPos[4][0][2] = 0x0;
    i_this->mButtonPos[2][0][0] = 0x0;
    i_this->mButtonPos[6][0][1] = 0x0;
    i_this->mButtonPos[3][1][0] = 0xffc4;
    i_this->mButtonPos[5][1][2] = 0x0;
    i_this->mButtonPos[2][1][2] = 0xffec;
    i_this->mButtonPos[3][1][3] = 0x0;
    i_this->mButtonPos[0][1][1] = 0xffc4;
    i_this->mButtonPos[4][1][2] = 0xffc4;
    i_this->mButtonPos[2][1][1] = 0xffec;
    i_this->mButtonPos[6][1][1] = 0xffc4;
    i_this->mSwimTekariPause1 = 0x14;
    i_this->mButtonPos[4][1][1] = 0xffea;
    i_this->mClockCloudPosX[0] = 0xffea;
    i_this->mButtonPos[4][1][3] = 0x0;
    i_this->mButtonPos[3][0][0] = 0x28;
    i_this->mClockCloudPosY[1] = 0xfffd;
    i_this->mButtonPos[5][0][2] = 0x0;
    i_this->mClockCloudPosX[1] = 0xffdd;
    i_this->mSwimTekariPauseEnd = 0x32;
    i_this->mClockCloudPosY[2] = 0xfff3;
    i_this->mClockCloudScale[1] = 0x82;
    i_this->mButtonPos[4][0][3] = 0xfffa;
    i_this->mButtonPos[7][1][2] = 0xfff5;
    i_this->mButtonPos[5][0][0] = 0x0;
    i_this->mMapCrossKeyPosY = 0x0;
    i_this->mButtonPos[2][1][3] = 0x0;
    i_this->mButtonPos[6][0][2] = 0x32;
    i_this->mButtonPos[5][1][1] = 0x0;
    i_this->mClockCloudScale[0] = 0x8c;
    i_this->mButtonPos[3][0][3] = 0x0;
    i_this->mButtonPos[2][1][0] = 0xffec;
    i_this->mButtonPos[7][1][3] = 0x7;
    i_this->mButtonPos[6][0][3] = 0x32;
    i_this->mClockCloudPosY[0] = 0xffd5;
    i_this->mClockCloudPosX[2] = 0xfffa;
    i_this->mMapCrossKeyPosX = 0x0;
    i_this->mButtonPos[6][0][0] = 0x0;
    i_this->mButtonPos[3][1][2] = 0x0;
    i_this->mButtonPos[6][1][3] = 0xffaf;
    i_this->mButtonPos[5][0][3] = 0x0;
    i_this->mButtonPos[7][0][0] = 0x0;
    i_this->mButtonPos[7][1][1] = 0xfff4;
    i_this->mButtonPos[4][0][1] = 0x23;
    i_this->mClockCloudScale[2] = 0x8c;
    i_this->mButtonPos[3][0][2] = 0x0;
    i_this->mButtonPos[7][0][3] = 0xff9e;
    i_this->mButtonPos[5][0][1] = 0x0;
    i_this->mButtonPos[7][0][2] = 0xffe2;
    i_this->mButtonPos[7][0][1] = 0x11;
    i_this->mButtonPos[6][1][0] = 0xffc4;
    i_this->mButtonPos[4][1][0] = 0xffe2;
    i_this->mButtonPos[7][1][0] = 0xffeb;
    return self;
}

// HD: retain the allocating GHS constructor, zeroing and store order.
/* 02597B00 */
static u32 dMeter_menuHIO_c__ct(u32 self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!self) {
        self = meter01_allocate(0xc8);
        if (!self)
            return 0;
    }
    dMeter_menuHIO_c* i_this = gabi::at<dMeter_menuHIO_c>(self);
    i_this->mVtable = 0x100510cc;
    i_this->field_0x30 = 0x0;
    i_this->field_0xc = l_zero;
    i_this->field_0x28 = 0x0;
    i_this->field_0x3a = 0x0;
    i_this->field_0x18 = l_zero;
    i_this->field_0x1c = l_zero;
    i_this->field_0x2a = 0x0;
    i_this->field_0x38 = 0x0;
    i_this->field_0x2e = 0x0;
    i_this->field_0x34 = 0x0;
    i_this->mItemScale = l_zero;
    i_this->field_0x20 = l_zero;
    i_this->field_0x32 = 0x0;
    i_this->field_0x36 = 0x0;
    i_this->field_0x2c = 0x0;
    i_this->mStarterScale = l_zero;
    i_this->field_0x10 = l_zero;
    i_this->field_0x24 = l_zero;
    meter01_clear(&i_this->field_0x6c, 0x4);
    meter01_clear(&i_this->field_0x70, 0x4);
    meter01_clear(&i_this->field_0x74, 0x4);
    meter01_clear(&i_this->field_0x78, 0x4);
    i_this->field_0xbe = 0x0;
    i_this->mTimerRupeePosY = 0x0;
    i_this->field_0x4c = l_zero;
    i_this->field_0x7e = 0x3;
    i_this->field_0x10 = l_hioValues.mValues[9];
    i_this->field_0xc0 = 0x1;
    i_this->field_0xc = l_mapRange;
    i_this->field_0x40 = l_hioValues.mValues[10];
    i_this->field_0x54 = l_hioValues.mValues[13];
    i_this->mItemScale = l_hioValues.mValues[0];
    i_this->field_0x44 = l_hioValues.mValues[11];
    i_this->field_0x58 = l_hioValues.mValues[14];
    i_this->field_0x76 = 0xff;
    i_this->field_0x50 = l_hioValues.mValues[12];
    i_this->field_0x6f = 0xff;
    i_this->field_0xc2 = 0xa;
    i_this->field_0x3c = l_zero;
    i_this->field_0x80 = 0x3;
    i_this->field_0x73 = 0xff;
    i_this->field_0xc4 = 0x0;
    i_this->field_0xc1 = 0x0;
    i_this->field_0x7c = 0x0;
    i_this->field_0x6c = 0x32;
    i_this->field_0x72 = 0xff;
    i_this->field_0x6e = 0x32;
    i_this->field_0x6d = 0x32;
    i_this->field_0x77 = 0xff;
    i_this->field_0x70 = 0xff;
    i_this->field_0x74 = 0xff;
    i_this->field_0x71 = 0xff;
    i_this->field_0x5c = l_hioValues.mValues[15];
    i_this->field_0x48 = l_zero;
    i_this->field_0x60 = l_one;
    i_this->field_0xc3 = 0x4;
    i_this->field_0x18 = l_hioValues.mValues[17];
    i_this->field_0x64 = l_hioValues.mValues[16];
    i_this->field_0x7b = 0xff;
    i_this->field_0x68 = l_hioValues.mValues[16];
    i_this->field_0xc7 = 0x1;
    i_this->field_0x8a = 0x0;
    i_this->field_0x84 = 0x5;
    i_this->field_0x78 = 0xc8;
    i_this->mStarterScale = l_hioValues.mValues[19];
    i_this->field_0x7a = 0xc8;
    i_this->field_0x79 = 0xc8;
    i_this->field_0xc5 = 0x2;
    i_this->mItemMoveScaleMode = 0x0;
    i_this->field_0x1c = l_hioValues.mValues[18];
    i_this->field_0x82 = 0x8;
    i_this->field_0xc6 = 0x0;
    i_this->field_0x88 = 0x3;
    i_this->mMenuOpenFrames = 0x7;
    i_this->field_0xb4 = 0x6;
    i_this->field_0x75 = 0xff;
    i_this->field_0x38 = 0x1a;
    i_this->field_0x28 = 0xffec;
    i_this->field_0x98 = 0xff;
    i_this->field_0x99 = 0xff;
    i_this->field_0xb2 = 0x0;
    i_this->field_0x8c = 0xff38;
    i_this->field_0x30 = 0x0;
    i_this->mReserved82 = 0x0;
    i_this->mItemMoveFrames = 0x9;
    i_this->mTimerRupeePosX = 0x0;
    i_this->field_0x20 = l_half;
    i_this->field_0xca = 0x0;
    i_this->mTimerClockScale = l_one;
    i_this->mGameOverDelay = 0x46;
    i_this->field_0x3a = 0xfffd;
    i_this->mMenuOpenDelay = 0x3;
    i_this->field_0x36 = 0x7;
    i_this->field_0x34 = 0x5;
    i_this->field_0xbf = 0x0;
    i_this->field_0xac = 0x2;
    i_this->field_0xa4 = 0x6;
    i_this->field_0xba = 0x5;
    i_this->field_0xb8 = 0x96;
    i_this->field_0x2a = 0x10;
    i_this->field_0xb0 = 0x3c;
    i_this->field_0xbc = 0x3;
    i_this->field_0xae = 0x1e;
    i_this->field_0x24 = l_hioValues.mValues[0];
    i_this->field_0xa8 = 0x2;
    i_this->field_0x32 = 0x8;
    i_this->field_0x8e = 0x15e;
    i_this->field_0xaa = 0x2;
    i_this->mTimerRupeeScale = l_one;
    i_this->field_0x2c = 0x18;
    i_this->field_0x2e = 0x17;
    i_this->field_0xa6 = 0x2;
    return self;
}

// HD: retain the allocating GHS constructor, zeroing and store order.
/* 02597E34 */
static u32 dMeter_msg_HIO_c__ct(u32 self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!self) {
        self = meter01_allocate(0x88);
        if (!self)
            return 0;
    }
    dMeter_msg_HIO_c* i_this = gabi::at<dMeter_msg_HIO_c>(self);
    i_this->mTalkWindowWhite.g = 0x1e;
    i_this->field_0x12 = 0x1e;
    i_this->mVtable = 0x100510e4;
    i_this->mTalkWindowBlack.b = 0x4b;
    i_this->field_0x20 = 0xff;
    i_this->field_0x14 = 0x0;
    i_this->mTalkWindowWhite.a = 0xd7;
    i_this->mRubyCharSpace = 0x0;
    i_this->field_0x19 = 0x78;
    i_this->field_0xd = 0x1e;
    i_this->field_0x8a = 0x0;
    i_this->field_0x10 = 0xdc;
    i_this->field_0x13 = 0xb4;
    i_this->field_0x11 = 0x1e;
    i_this->mRubyFontSize = 0xf;
    i_this->mTalkWindowWhite.b = 0x1e;
    i_this->field_0x1f = 0x3c;
    i_this->field_0xf = 0x1e;
    i_this->field_0x23 = 0xff;
    i_this->mTalkWindowBlack.r = 0x1e;
    i_this->field_0x1a = 0x78;
    i_this->field_0x28 = 0xff;
    i_this->field_0x22 = 0xff;
    i_this->mStickStartDelay = 0x0;
    i_this->field_0x17 = 0x0;
    i_this->mCharSpace = 0x0;
    i_this->mTalkWindowOffsetY = 0x0;
    i_this->mTalkWindowWhite.r = 0x1e;
    i_this->field_0x15 = 0x0;
    i_this->field_0x18 = 0xff;
    i_this->field_0x86 = 0x0;
    i_this->field_0xe = 0x1e;
    i_this->field_0x21 = 0x0;
    i_this->mTalkWindowBlack.g = 0x1e;
    i_this->field_0x1d = 0xff;
    i_this->field_0x27 = 0xff;
    i_this->field_0x16 = 0xff;
    i_this->field_0x64 = 0x0;
    i_this->mTalkWindowBlack.a = 0x0;
    i_this->mStickHoldFrames = 0x0;
    i_this->mSendSpeed = 0x2;
    i_this->mLineSpace = 0x1e;
    i_this->field_0x26 = 0x0;
    i_this->field_0x72 = 0x0;
    i_this->mSpaceTimer = 0x0;
    i_this->field_0x7e = 0xa;
    i_this->mFontSize = 0x17;
    i_this->mSpaceFlag = 0x0;
    i_this->field_0x85 = 0x0;
    i_this->field_0x25 = 0xff;
    i_this->field_0x1e = 0xff;
    i_this->mSelectWindowAlpha = 0xb4;
    i_this->field_0x24 = 0xff;
    i_this->mSelectWidthAdd = 0x0;
    i_this->mRubyOffsetY = 0xd;
    i_this->field_0x1b = 0xff;
    i_this->field_0x84 = 0x1;
    i_this->mCornerColorFrames = 0xaa;
    i_this->field_0x54 = l_hioValues.mValues[21];
    i_this->mDemoFontSize = 0x1d;
    i_this->mStickBlendFrames = 0xa;
    i_this->mOutFontOffsetY = 0x0;
    i_this->field_0x1c = 0xff;
    i_this->mTalkWindowSplitY = 0xf0;
    i_this->field_0x4c = 0x4;
    i_this->mSelectCloseFrames = 0x4;
    i_this->mTextOffsetY = 0x0;
    i_this->field_0x2d = 0xff;
    i_this->field_0x2b = 0x50;
    i_this->mSelectOffsetY = 0x46;
    i_this->field_0x4a = 0x2;
    i_this->field_0x2f = 0x96;
    i_this->mSelectHeightAdd = 0x0;
    i_this->field_0x74 = 0x80;
    i_this->field_0x2a = 0x50;
    i_this->field_0x38 = l_hioValues.mValues[22];
    i_this->field_0x3c = l_hioValues.mValues[23];
    i_this->field_0x81 = 0x46;
    i_this->field_0x2e = 0x96;
    i_this->mSelectCloseScale = l_half;
    i_this->mSelectOpenFrames = 0x3;
    i_this->mMsgArrowAlphaA = 0x96;
    i_this->field_0x50 = l_hioValues.mValues[20];
    i_this->mSelectOpenDelay = 0x5;
    i_this->mMsgArrowAlphaB = 0xff;
    i_this->field_0x29 = 0xff;
    i_this->field_0x76 = 0xa;
    return self;
}

/* 02593630 */
static s32 dMeter_Draw(sub_meter_class* i_Meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auto* prefix = reinterpret_cast<MeterPrefixHD*>(i_Meter);
    fopMsgM_pane_class* panes[] = {
        &prefix->mMapWindArrow,      &prefix->mMapCrossArrow[0],   &prefix->mMapCrossArrow[1],
        &prefix->mMapCrossArrow[2],   &prefix->mMenuArrowR,      &prefix->mMenuArrowL,
        &prefix->mMapCrossKey,      &prefix->mBossIconEye1,      &prefix->mBossIconEye2,
        &prefix->mBossIconTop,      &prefix->mBossIconBottom,      &prefix->mBossLifeBar[0],
        &prefix->mBossLifeBar[1],   &prefix->mBossLifeBar[2],   &prefix->mBossLifeBar[3],
        &prefix->mEnemyLifePart3,      &prefix->mEnemyLifeBar,      &prefix->mEnemyLifePart1,
        &prefix->mEnemyLifeShadow,      &i_Meter->mHeart[0],        &i_Meter->mHeartShadow[0],
        &i_Meter->mHeart[1],        &i_Meter->mHeartShadow[1],  &i_Meter->mHeart[2],
        &i_Meter->mHeartShadow[2],  &i_Meter->mHeart[3],        &i_Meter->mHeartShadow[3],
        &i_Meter->mHeart[4],        &i_Meter->mHeartShadow[4],  &i_Meter->mHeart[5],
        &i_Meter->mHeartShadow[5],  &i_Meter->mHeart[6],        &i_Meter->mHeartShadow[6],
        &i_Meter->mHeart[7],        &i_Meter->mHeartShadow[7],  &i_Meter->mHeart[8],
        &i_Meter->mHeartShadow[8],  &i_Meter->mHeart[9],        &i_Meter->mHeartShadow[9],
        &i_Meter->mHeart[10],       &i_Meter->mHeartShadow[10], &i_Meter->mHeart[11],
        &i_Meter->mHeartShadow[11], &i_Meter->mHeart[12],       &i_Meter->mHeartShadow[12],
        &i_Meter->mHeart[13],       &i_Meter->mHeartShadow[13], &i_Meter->mHeart[14],
        &i_Meter->mHeartShadow[14], &i_Meter->mHeart[15],       &i_Meter->mHeartShadow[15],
        &i_Meter->mHeart[16],       &i_Meter->mHeartShadow[16], &i_Meter->mHeart[17],
        &i_Meter->mHeartShadow[17], &i_Meter->mHeart[18],       &i_Meter->mHeartShadow[18],
        &i_Meter->mHeart[19],       &i_Meter->mHeartShadow[19], &i_Meter->mMagicBar[0],
        &i_Meter->mMagicBar[1],  &i_Meter->mMagicBar[2],  &i_Meter->mMagicBar[3],
        &i_Meter->mMagicBar[4],  &i_Meter->mMagicBar[5],  &i_Meter->mMagicBar[6],
        &i_Meter->mMagicBar[7],  &i_Meter->mMagicGauge3,     &i_Meter->mMagicGauge1,
        &i_Meter->mMagicGaugeShadow1,     &i_Meter->field_0x1210,     &i_Meter->mCompassBase,
        &i_Meter->mClockBar,     &i_Meter->mClockRing,     &i_Meter->mClockHorizon1,
        &i_Meter->mClockHorizon2,     &i_Meter->mClockStar[0],  &i_Meter->mFixedClockCloud[0],
        &i_Meter->mClockStar[1],  &i_Meter->mFixedClockCloud[0],  &i_Meter->mClockStar[2],
        &i_Meter->mFixedClockCloud[0],  &i_Meter->mClockMoon,     &i_Meter->mClockSun,
        &i_Meter->mClockCircle[0],  &i_Meter->mFixedClockStar[0],  &i_Meter->mClockCircle[1],
        &i_Meter->mFixedClockStar[0],  &i_Meter->mClockCircle[2],  &i_Meter->mFixedClockStar[0],
        &i_Meter->mClockCircle[3],  &i_Meter->mFixedClockStar[0],  &i_Meter->mClockTimeDial,
        &i_Meter->mClockCenter,     &i_Meter->mFixedClockRing,     &i_Meter->mFixedClockMoon,
        &i_Meter->mFixedClockCloudClip,     &i_Meter->mFixedClockSun,     &i_Meter->mCompassClock,
        &i_Meter->mKeyLight1,     &i_Meter->mKeyLight2,     &i_Meter->mKeyDigitA[0],
        &i_Meter->mKeyDigitB[0],  &i_Meter->mKeyDigitA[1],  &i_Meter->mKeyDigitB[1],
        &i_Meter->mKeyIcon,     &i_Meter->mWeaponText,     &i_Meter->mWeaponIcon,
        &i_Meter->mWeaponIconShadow,     &i_Meter->mWeaponButton,     &i_Meter->mWeaponButton2,
        &i_Meter->mActionText,     &i_Meter->mActionButton2[0],  &i_Meter->mActionButton[0],
        &i_Meter->mActionButton2[1],  &i_Meter->mActionButton[1],  &i_Meter->mArrowDigit10,
        &i_Meter->mArrowDigit1,     &i_Meter->mActionBowIcon,     &i_Meter->mActionBowIconShadow,
        &i_Meter->mButtonCenter,     &i_Meter->mItemDigit1[0],  &i_Meter->mItemDigit10[0],
        &i_Meter->mItemIcon[0],  &i_Meter->mItemIconShadow[0],  &i_Meter->mBowLight1[0],
        &i_Meter->mBowLight2[0],  &i_Meter->mBowLight3[0],  &i_Meter->mItemDigit1[1],
        &i_Meter->mItemDigit10[1],  &i_Meter->mItemIcon[1],  &i_Meter->mItemIconShadow[1],
        &i_Meter->mBowLight1[1],  &i_Meter->mBowLight2[1],  &i_Meter->mBowLight3[1],
        &i_Meter->mItemDigit1[2],  &i_Meter->mItemDigit10[2],  &i_Meter->mItemIcon[2],
        &i_Meter->mItemIconShadow[2],  &i_Meter->mBowLight1[2],  &i_Meter->mBowLight2[2],
        &i_Meter->mBowLight3[2],  &i_Meter->mZButton,     &i_Meter->mXYButton[0],
        &i_Meter->field_0x25c0[0],  &i_Meter->field_0x2630[0],  &i_Meter->mXYButton[1],
        &i_Meter->field_0x25c0[1],  &i_Meter->field_0x2630[1],  &i_Meter->mRActionIcon,
        &i_Meter->mRButton,     &i_Meter->field_0x26d8,     &i_Meter->mRupyDigitA[0],
        &i_Meter->mRupyDigitB[0],  &i_Meter->mRupyDigitA[1],  &i_Meter->mRupyDigitB[1],
        &i_Meter->mRupyDigitA[2],  &i_Meter->mRupyDigitB[2],  &i_Meter->mRupyDigitA[3],
        &i_Meter->mRupyDigitB[3],  &i_Meter->mRupyIcon,     &i_Meter->mRupyIconB};
    for (auto* pane : panes)
        meter01_setAlpha(pane);
    if (sScrTimer2) {
        for (int i = 0; i < 9; i++)
            meter01_setAlpha(&i_Meter->mSwimTimerLight[i]);
        meter01_setAlpha(&i_Meter->mSwimTimerFrame);
        meter01_setAlpha(&i_Meter->mSwimTimerMask2);
    }
    if (sScrTimer1) {
        meter01_setAlpha(&i_Meter->mSwimTimerSoul);
        meter01_setAlpha(&i_Meter->mSwimTimerMain);
        meter01_setAlpha(&i_Meter->mSwimTimerGauge);
        meter01_setAlpha(&i_Meter->mSwimTimerShadow);
    }
    for (int i = 0; i < 4; i++)
        meter01_setAlpha(&i_Meter->mRoadArrow[i]);
    if (!meter01_isOverlapRequest()) {
        meter01_setDrawList(0x1047A994, 0x1DC);
        for (int i = 0; i < 3; i++)
            meter01_setDrawList(0x1047AAA0 + 0xC4 * i, 0x1DC);
        if (sScrTimer1 && sScrTimer2)
            meter01_setDrawList(0x1047A9DC, 0x1DC);
        meter01_setDrawList(0x1047A998, 0xD4);
    }
    return 1;
}

/* 02595848 */
static s32 dMeter_IsDelete() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 1;
}

/* 02595850 */
static s32 dMeter_Delete(sub_meter_class* i_Meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auto* prefix = reinterpret_cast<MeterPrefixHD*>(i_Meter);
    u32 oldHeap = meter01_setCurrentHeap(prefix->heap);
    u8 setting = meter01_mapSetting();
    meter01_restoreMapSetting(setting);
    if (rupy_soundOnFlag) {
        meter01_soundStart(0x83A);
        rupy_soundOnFlag = 0;
        rupy_soundSetFlag = 0;
    }
    if (dMeter_windID != 0xFFFFFFFF) {
        if (meter01_searchMessage(dMeter_windID))
            meter01_deleteMessage(meter01_searchMessage(dMeter_windID));
        dMeter_windStatus = 0;
        dMeter_windID = 0xFFFFFFFF;
    }
    be<u32>* emitters[] = {&i_Meter->mpRupyParticle, &i_Meter->mpArrowTypeParticle[0], &i_Meter->mpArrowTypeParticle[1],
                           &i_Meter->mpForestWaterParticle[0], &i_Meter->mpForestWaterParticle[1]};
    for (auto* emitterSlot : emitters) {
        auto* emitter = gabi::at<meter_detail::Particle>(*emitterSlot);
        if (emitter) {
            u32 flags = emitter->mFlags;
            emitter->mMaxFrame = -1;
            emitter->mFlags = flags | 1;
            emitter = gabi::at<meter_detail::Particle>(*emitterSlot);
            emitter->mFlags = (u32)emitter->mFlags & ~0x40u;
        }
    }
    for (int i = 0; i < 3; i++) {
        meter01_freeTexture(i_Meter, i_Meter->actionTex[i]);
        meter01_freeTexture(i_Meter, i_Meter->xyIconTex[i]);
    }
    meter01_freeTexture(i_Meter, i_Meter->arrowTex[0]);
    meter01_freeTexture(i_Meter, i_Meter->moveIconTex[0]);
    meter01_freeTexture(i_Meter, i_Meter->arrowTex[1]);
    meter01_destroyScreen(sMainParts1);
    meter01_destroyScreen(sMainParts2);
    meter01_destroyScreen(sMainParts3);
    if (sScrTimer1 && sScrTimer2) {
        meter01_removeSwimResources();
        meter01_destroyScreen(sScrTimer1);
        meter01_destroyScreen(sScrTimer2);
        sScrTimer2 = 0;
        sScrTimer1 = 0;
    }
    meter01_destroyScreen(sChoiceRoad);
    u32 last = moveItemPane;
    sChoiceRoad = 0;
    meter01_destroyScreen(last);
    moveItemPane = 0;
    meter01_setCurrentHeap(oldHeap);
    meter01_destroyExpHeap(prefix->heap);
    meter01_deleteHIOChild((s8)g_meter_mapHIO.mNo);
    meter01_clearHudState();
    return 1;
}

/* 0259359C */
void mapCtrlDisp_c::initMapCtrlDisp() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    l_mapState.mAlpha = 0;
    l_mapState.mIconFreeAlpha = 0;
    l_mapState.mIconFreePosX = g_meter_mapHIO.mIconFreePosX;
    l_mapState.mIconDisplayMode = 0;
    l_mapState.mPosLeftUpY = g_meter_mapHIO.mMapPosY;
    l_mapState.mIconFreePosY = g_meter_mapHIO.mIconFreePosY;
    l_mapState.mPosLeftUpX = g_meter_mapHIO.mMapPosX;
    l_mapState.mDisplayMode = 1;
    f32 zero = l_zero;
    l_iconFreeScale = zero;
    l_mapState.mSelfAlpha = 0;
    l_iconSelfScale = zero;
    field_0x0 = 0;
    field_0x2 = 0;
    field_0x4 = 0;
    field_0x1 = 0;
}

// The other verified functions keep their original address-level form.
static float loadf(u32 p) {return gabi::load<float>(p);}
static void storef(u32 p,float v){gabi::store<float>(p,v);}




/* 02598254 */
static u32 dMeter_map_HIO_c__ct(u32 self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!self) {
        self = meter01_allocate(sizeof(dMeter_map_HIO_c));
        if (!self)
            return 0;
    }
    auto i_this = gabi::at<dMeter_map_HIO_c>(self);
    i_this->mMapHiddenPosX = -180;
    i_this->mHDScale = l_zero;
    i_this->mIconFreePosY = 150;
    i_this->field_0x7 = 1;
    i_this->mVtable = 0x10051114;
    i_this->mMapPosX = 35;
    reinterpret_cast<MeterWeaponMapHD *>(i_this)->mHDDisplay = 0;
    i_this->mMapPosY = 338;
    i_this->field_0x5 = 0;
    i_this->mIconFreeAlpha = 200;
    i_this->mIconFreeScale = l_mapIconScale;
    i_this->mNo = 0;
    i_this->mIconFreePosX = 590;
    return self;
}
// HD: the draw-list receiver is unused in these two generated adapters.
/* 025982F4 */
static void dDlst_2DMETER1_c_draw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auto graf = MeterWeaponApi::getGrafPort();
    graf->setPort();
    f32 zero = l_zero;
    MeterWeaponApi::drawScreen(sMainParts3, graf, zero, zero);
    MeterWeaponApi::drawScreen(sMainParts1, graf, zero, zero);
    u32 timer = sScrTimer1;
    if (timer) {
        gabi::at<MeterWeaponScreenHD>(timer)->mScissor = 1;
        MeterWeaponApi::drawScreen(sScrTimer1, graf, zero, zero);
    }
    u32 timer2 = sScrTimer2;
    if (timer2)
        MeterWeaponApi::drawScreen(timer2, graf, zero, zero);
    MeterWeaponApi::drawScreen(sChoiceRoad, graf, zero, zero);
}
/* 025983E8 */
static void dDlst_2DMETER2_c_draw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    auto graf = MeterWeaponApi::getGrafPort();
    graf->setPort();
    f32 zero = l_zero;
    MeterWeaponApi::drawScreen(sMainParts2, graf, zero, zero);
}
/* 0259868C */
static void destroyMeterClockArray() {
    // HD: three clock display objects use the GHS array destructor.
    MeterWeaponApi::destroyArray(&l_meterClock, 3, 0xC4, MeterWeaponApi::destroyClock, 0, 0);
}
/* 025986B0 */
static u8 dMeter_itemMoveFlagCheck() { return l_itemMoveFlag; }
/* 025986BC */
static u8 dMenu_flag() { return l_menuPause; }
/* 025986C8 */
static void dMeter_map_HIO_c__dt(dMeter_map_HIO_c *i_this, s32 flags) {
    if (i_this != NULL && (flags & 1))
        operator_delete(i_this);
}


/* 02598058 */
static u32 dMeter_message_HIO_c__ct(u32 self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!self) {
        self = meter01_allocate(sizeof(dMeter_message_HIO_c));
        if (!self)
            return 0;
    }
    auto i_this = gabi::at<dMeter_message_HIO_c>(self);
    i_this->mTextColor.r = 0x0;
    i_this->field_0x2c = 0xff;
    i_this->mShadowColor.b = 0xff;
    i_this->mVtable = 0x100510fc;
    i_this->mTextWhite.g = 0xff;
    i_this->mAimBrightness = 0x80;
    i_this->mTextWhite.b = 0xff;
    i_this->field_0x2e = 0xb4;
    i_this->mRubyWhite.a = 0xff;
    i_this->mTextColor.g = 0x0;
    i_this->mShadowColor.g = 0xff;
    i_this->mRubyBlack.a = 0x0;
    i_this->mTextWhite.r = 0xff;
    i_this->mRubyColor.r = 0x0;
    i_this->field_0x2b = 0xff;
    i_this->field_0x36 = 0x2a;
    i_this->field_0x2a = 0xff;
    i_this->mRubyWhite.b = 0xff;
    i_this->mRubyBlack.r = 0xff;
    i_this->mRubyBlack.g = 0xff;
    i_this->mShadowWhite.b = 0xff;
    i_this->mShadowColor.r = 0xff;
    i_this->mShadowWhite.r = 0xff;
    i_this->mShadowColor.a = 0xff;
    i_this->mTextColor.a = 0xff;
    i_this->mTextBlack.r = 0xff;
    i_this->mRubyWhite.r = 0xff;
    i_this->mShadowWhite.a = 0xff;
    i_this->mRubyWhite.g = 0xff;
    i_this->field_0x2d = 0xff;
    i_this->mRubyColor.a = 0xff;
    i_this->mTextBlack.b = 0xff;
    i_this->field_0x30 = 0x4b;
    i_this->mTextWhite.a = 0xff;
    i_this->mTextBlack.g = 0xff;
    i_this->field_0x3a = 0x1;
    i_this->mRubyBlack.b = 0xff;
    i_this->mShadowBlack.g = 0xff;
    i_this->mTextBlack.a = 0x0;
    i_this->field_0x34 = 0xfffe;
    i_this->mMsg2RubyOffsetY = 0xf;
    i_this->mLineHeightAdd = 0x16;
    i_this->mRubyColor.b = 0x0;
    i_this->mTextColor.b = 0x0;
    i_this->field_0x2f = 0xa0;
    i_this->mShadowBlack.a = 0x0;
    i_this->field_0x4a = 0x0;
    i_this->mShadowWhite.g = 0xff;
    i_this->mPopSpeed = 0xa;
    i_this->field_0x44 = 0x0;
    i_this->field_0x40 = l_messageScale;
    i_this->mShadowBlack.r = 0xff;
    i_this->mMsg2FontSize = 0x1b;
    i_this->field_0x76 = 0x1;
    i_this->field_0x7a = 0x4;
    i_this->field_0x7c = 0x4;
    i_this->field_0x82 = 0x18c;
    i_this->field_0x62 = 0x0;
    i_this->field_0x7e = 0x2;
    i_this->field_0x4d = 0xff;
    i_this->field_0x78 = 0x0;
    i_this->field_0x50 = 0x0;
    i_this->field_0x4e = 0xff;
    i_this->field_0x31 = 0x0;
    i_this->field_0x46 = 0x1;
    i_this->field_0x70 = 0x1;
    i_this->field_0x5d = 0x0;
    i_this->field_0x80 = 0x258;
    i_this->field_0x5e = 0x0;
    i_this->field_0x54 = 0x0;
    i_this->field_0x4c = 0xff;
    i_this->field_0x5c = 0x0;
    i_this->field_0x53 = 0x0;
    i_this->field_0x5a = 0x0;
    i_this->field_0x51 = 0x0;
    i_this->field_0x72 = 0x1;
    i_this->field_0x74 = 0x1;
    i_this->field_0x68 = 0x0;
    i_this->field_0x48 = 0x0;
    i_this->field_0x60 = 0x0;
    i_this->mShadowBlack.b = 0xff;
    i_this->mTextOffsetX = 0x0;
    i_this->field_0x6a = 0x0;
    i_this->field_0x6c = 0x1;
    i_this->field_0x64 = 0x0;
    i_this->field_0x6e = 0x1;
    i_this->mShowRuby = 0x1;
    i_this->field_0x66 = 0x0;
    i_this->mRubyColor.g = 0x0;
    i_this->field_0x56 = 0x0;
    i_this->field_0x4f = 0xff;
    i_this->field_0x52 = 0x0;
    i_this->mMultiTexAlpha = 0xff;

    return self;
}








/* 02598444 */
static void __sinit_d_meter_cpp() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    for (int i = 3; i >= 0; i--)
        l_meterInitializeWords.mWords[i] = 0;
    MeterWeaponApi::registerGlobal(&l_meterRegistrations.mEntries[0]);
    l_meterTextureState.mRateX = l_meterRateX;
    l_meterTextureState.mRateY = l_meterRateY;
    MeterWeaponApi::initializeHeaderByte(&l_meterHeaderBytes.mFirst);
    MeterWeaponApi::registerGlobal(&l_meterRegistrations.mEntries[1]);
    MeterWeaponApi::initializeHeaderFlag(&l_meterHeaderBytes.mSecond);
    MeterWeaponApi::registerGlobal(&l_meterRegistrations.mEntries[2]);
    l_meterTextureState.mScaleY = l_meterScale;
    l_meterTextureState.mScaleMax = l_meterScaleMax;
    l_meterTextureState.mScaleMin = l_meterScaleMax;
    l_hdState.mStatusFlags = 0;
    for (u32 offset : {0x62u, 0x63u, 0x3Eu, 0x3Fu, 0x40u, 0x41u, 0x42u, 0x43u, 0x44u, 0x5Au, 0x5Bu, 0x5Cu,
                       0x5Du, 0x5Eu, 0x5Fu})
        l_hdState.mStateBytes[offset - 0x3E] = 0;
    for (int i = 0; i < 8; i++)
        l_hdState.mPaneScales[i] = l_zero;
    l_hdState.mRoomNumber = 0;
    l_meterTextureState.mScaleX = l_meterScale;
    l_hdState.mStateBytes[0x60 - 0x3E] = 0;
    l_hdState.mStateBytes[0x61 - 0x3E] = 0;
    for (int i = 0; i < 7; i++) {
        l_hdState.mStateBytes[0x53 - 0x3E + i] = 0;
        l_hdState.mStateBytes[0x45 - 0x3E + i] = 0;
        l_hdState.mStateBytes[0x4C - 0x3E + i] = 0;
    }
    dMeter_map_HIO_c__ct(gabi::ea(&g_meter_mapHIO));
    dMeter_HIO_c__ct(gabi::ea(&g_meterHIO));
    dMeter_menuHIO_c__ct(gabi::ea(&g_menuHIO));
    dMeter_msg_HIO_c__ct(gabi::ea(&g_msgHIO));
    dMeter_message_HIO_c__ct(gabi::ea(&g_messageHIO));
    MeterWeaponApi::constructDrawList(&l_meter1);
    l_meter1.mVtable = 0x10051518;
    MeterWeaponApi::registerGlobal(&l_meterRegistrations.mEntries[3]);
    MeterWeaponApi::constructDrawList(&l_meter2);
    l_meter2.mVtable = 0x10051530;
    MeterWeaponApi::registerGlobal(&l_meterRegistrations.mEntries[4]);
    MeterWeaponApi::constructDrawList(&l_meterTekari);
    l_meterTekari.mVtable = 0x1004CBD8;
    MeterWeaponApi::registerGlobal(&l_meterRegistrations.mEntries[5]);
    MeterWeaponApi::constructArray(&l_meterClock, 3, 0xC4, MeterWeaponApi::constructClock);
    MeterWeaponApi::registerGlobal(&l_meterRegistrations.mEntries[6]);
}



// Updates HUD flags, transition timers and per-frame pane behavior.
/* inlined into dMeter_Execute */
static void meterUpdateStatusCheck(sub_meter_class *i_Meter) {
  using namespace meter_detail;
  dComIfGp_get(); // HD: the inlined prologue retains this play-object lookup.
  i_Meter->mStatusFlags = 0;
  bool hide = !meterUpdate2DState()->mShow;
  if (!hide)
    hide = (meterUpdateCameraBody()->mFlags & 0x02000000) != 0;
  if (hide) {
    i_Meter->mStatusFlags |= dMtrStts_UNK4000_e;
  } else {
    bool auction = meterUpdateMenu()->mAuctionFlag != 0;
    if (auction)
      auction = meterUpdatePlay()->mMesgStatus == 0;
    bool puzzle = auction;
    if (!puzzle) {
      puzzle = dComIfGs_isTmpBit(0x408) != 0;
      if (puzzle)
        puzzle = meterUpdatePlay()->mMesgStatus == 0;
    }
    if (!puzzle)
      puzzle = meterUpdateStartCheckOld(&meterUpdatePlay()->mEvtManager,
                                        "PUZZLE_GAME") != 0;
    if (!puzzle)
      puzzle = meterUpdateStartCheckOld(&meterUpdatePlay()->mEvtManager,
                                        "PUZZLE_RUPEE") != 0;
    if (puzzle) {
      i_Meter->mStatusFlags |= dMtrStts_UNK80000_e;
      meterUpdatePlay()->mAStatus = MeterUpdate::dActStts_BLANK_e;
      meterUpdatePlay()->mDoStatus = MeterUpdate::dActStts_BLANK_e;
    } else {
      u8 wind = meterUpdatePlay()->mOperateWind;
      dComIfG_play_c *play = meterUpdatePlay();
      if (wind == 2) {
        play->mDoStatusForce = MeterUpdate::dActStts_CHOOSE_e;
        meterUpdatePlay()->mAStatusForce = MeterUpdate::dActStts_RETURN_e;
        i_Meter->mStatusFlags |= dMtrStts_UNK400000_e;
      } else {
        bool miniGame = play->mMiniGameType == 8;
        if (miniGame)
          miniGame = (meterUpdatePlay()->mPlayerStatus[0][0] & 0x10) == 0;
        if (miniGame) {
          meterUpdatePlay()->mDoStatusForce = MeterUpdate::dActStts_bow_01;
          meterUpdatePlay()->mAStatusForce = MeterUpdate::dActStts_HIDDEN_e;
          i_Meter->mStatusFlags |= dMtrStts_HD1000000_e;
        } else {
          bool conducting = (meterUpdatePlay()->mPlayerStatus[0][1] & 1) != 0;
          if (conducting)
            conducting =
                meterUpdatePlay()->mAStatus == MeterUpdate::dActStts_RETURN_e;
          if (conducting) {
            i_Meter->mStatusFlags |= dMtrStts_UNK200000_e;
          } else {
            bool event = meterUpdatePlay()->mEvtCtrl.mMode != 0;
            if (event)
              event = meterUpdateMenu()->mMenuStatus != 4;
            if (event) {
              bool message = meterUpdateDemoMode() != 1;
              if (message)
                message = meterUpdatePlay()->mMesgStatus != 0;
              if (message)
                message = meterUpdatePlay()->mMetronome == 0;
              if (message) {
                i_Meter->mStatusFlags |= dMtrStts_UNK100_e;
              } else {
                bool metronome = meterUpdateDemoMode() != 1;
                if (metronome)
                  metronome = meterUpdatePlay()->mMetronome != 0;
                if (metronome) {
                  i_Meter->mStatusFlags |= dMtrStts_UNK200000_e;
                  meterUpdatePlay()->mAStatusForce =
                      MeterUpdate::dActStts_HIDDEN_e;
                } else {
                  i_Meter->mStatusFlags |= dMtrStts_UNK40_e;
                }
              }
            }
          }
        }
      }
    }
  }

  u8 menu = meterUpdateMenu()->mWindowFlag;
  dComIfG_play_c *play = meterUpdatePlay();
  if (menu) {
    bool picture = (play->mPlayerStatus[0][1] & 8) != 0;
    if (!picture)
      picture = meterUpdatePlay()->mPictureStatus == 2;
    if (!picture)
      picture = meterUpdatePlay()->mPictureStatus == 3;
    if (picture) {
      i_Meter->mStatusFlags |= dMtrStts_UNK20000_e;
      if (meterUpdatePlay()->mPictureStatus == 2) {
        meterUpdatePlay()->mDoStatusForce = MeterUpdate::dActStts_CHOOSE_e;
        meterUpdatePlay()->mAStatusForce = MeterUpdate::dActStts_RETURN_e;
      }
    }
  } else {
    fopAc_ac_c *partner = play->mpPlayerPtr[1];
    fopAc_ac_c *player = meterUpdatePlay()->mPlayerInfo[0].mpPlayer;
    bool possession = partner == player;
    if (!possession)
      possession = meterUpdateKamomeState() != 0;
    if (possession) {
      i_Meter->mStatusFlags |= dMtrStts_UNK40000_e;
      if (meterUpdatePlay()->mAStatus == 0x35)
        meterUpdatePlay()->mAStatus = MeterUpdate::dActStts_BLANK_e;
    } else {
      bool subject = (meterUpdatePlay()->mPlayerStatus[0][0] & 0x2000) != 0;
      if (!subject) {
        subject = (meterUpdateCameraAttention()->mStatus & 0x100) != 0;
        if (subject)
          subject = (meterUpdatePlay()->mPlayerStatus[0][0] & 0x08000000) != 0;
      }
      if (subject)
        i_Meter->mStatusFlags |= dMtrStts_UNK100000_e;
    }
    if (meterUpdatePlay()->mPlayerStatus[0][0] & 0x10) {
      i_Meter->mStatusFlags |= dMtrStts_UNK100_e;
    } else if (meterUpdatePlay()->mPlayerStatus[0][0] & 0x200000) {
      i_Meter->mStatusFlags |= dMtrStts_UNK80_e;
    } else {
      bool picture = (meterUpdatePlay()->mPlayerStatus[0][1] & 8) != 0;
      if (!picture)
        picture = meterUpdatePlay()->mPictureStatus == 2;
      if (!picture)
        picture = meterUpdatePlay()->mPictureStatus == 3;
      if (picture) {
        i_Meter->mStatusFlags |= dMtrStts_UNK20000_e;
      } else if (meterUpdatePlay()->mPlayerStatus[0][0] & 0x10000) {
        i_Meter->mStatusFlags |= dMtrStts_UNK400_e;
      } else if (meterUpdatePlay()->mPlayerStatus[0][0] & 0x08000000) {
        i_Meter->mStatusFlags |= dMtrStts_UNK200_e;
      } else {
        u32 rope = meterUpdatePlay()->mPlayerStatus[0][0] & 0x800000;
        play = meterUpdatePlay();
        if (rope) {
          player = play->mPlayerInfo[0].mpPlayer;
          u32 flags;
          // HD: the virtual query runs before the HUD status load.
          BOOL tagged = meterUpdateCheckRopeTag(player);
          flags = i_Meter->mStatusFlags;
          i_Meter->mStatusFlags =
              flags | (tagged ? dMtrStts_UNK800_e : dMtrStts_UNK1000_e);
        } else if (play->mPlayerStatus[0][0] & 0x100000) {
          i_Meter->mStatusFlags |= dMtrStts_UNK2000_e;
        } else if (meterUpdatePlay()->mPlayerStatus[0][0] & 0x02000100) {
          i_Meter->mStatusFlags |= dMtrStts_UNK8000_e;
        } else if (meterUpdatePlay()->mPlayerStatus[0][0] & 0x04000000) {
          i_Meter->mStatusFlags |= dMtrStts_UNK10000_e;
        }
      }
    }
  }
  u32 stageType = (meterUpdateStage()->mTypeFlags >> 16) & 7;
  if (stageType == 1) {
    i_Meter->mStatusFlags |= dMtrStts_UNK4_e;
  } else {
    stageType = (meterUpdateStage()->mTypeFlags >> 16) & 7;
    i_Meter->mStatusFlags |= stageType == 2 ? dMtrStts_UNK2_e : dMtrStts_UNK1_e;
  }
  u32 picture = meterUpdatePictureRecord();
  if (meterUpdatePictureActive(picture))
    i_Meter->mStatusFlags |= dMtrStts_HD2000000_e;
}

/* inlined into dMeter_Execute */
static f32 meterUpdateAlphaControl(sub_meter_class *i_Meter) {
  u8 show = meterUpdate2DState()->mShow;
  f32 zero = l_zero;
  if (!show) {
    if (i_Meter->mCompassAlpha > zero) {
      s16 timer = i_Meter->mCompassFadeTimer;
      s16 max = meterUpdateHIO()->mCompassFadeFrames;
      f32 alpha = meterUpdateValueIncrease(max, timer, 0);
      timer = i_Meter->mCompassFadeTimer;
      i_Meter->mCompassAlpha = alpha;
      i_Meter->mCompassFadeTimer = timer - 1;
    } else {
      i_Meter->mCompassAlpha = zero;
      i_Meter->mCompassFadeTimer = 0;
    }
  } else {
    u8 menu = meterUpdateMenu()->mWindowFlag;
    f32 one = l_one;
    if (menu) {
      s32 timer = meterUpdateMenuTimer();
      f32 half = l_half;
      if (timer <= 5) {
        f32 alpha = meterUpdateValueIncrease(5, timer, 0);
        u8 status = meterUpdateMenu()->mMenuStatus;
        f32 current = i_Meter->mCompassAlpha;
        if (status != 3) {
          f32 inverse = one - alpha;
          alpha = gabi::fmadds(inverse, half, half);
        }
        if (current > alpha)
          i_Meter->mCompassAlpha = alpha;
      } else {
        u8 status = meterUpdateMenu()->mMenuStatus;
        i_Meter->mCompassAlpha = status == 3 ? zero : half;
      }
    } else if (i_Meter->mCompassAlpha < one) {
      s16 timer = i_Meter->mCompassFadeTimer;
      s16 max = meterUpdateHIO()->mCompassFadeFrames;
      f32 alpha = meterUpdateValueIncrease(max, timer, 0);
      timer = i_Meter->mCompassFadeTimer;
      i_Meter->mCompassAlpha = alpha;
      i_Meter->mCompassFadeTimer = timer + 1;
    } else {
      i_Meter->mCompassAlpha = one;
      s16 max = meterUpdateHIO()->mCompassFadeFrames;
      i_Meter->mCompassFadeTimer = max;
    }
  }
  return zero;
}

/* inlined into dMeter_Execute */
static void meterUpdateHeartMove(sub_meter_class *i_Meter,
                                 MeterUpdateScratch *scratch, f32 zero) {
  if (meterUpdatePlay()->mMiniGameType == 6) {
    meterUpdateBattleFlag() = 1;
    u8 battleState = meterUpdateBattleState();
    meterUpdateSharedState()->mBattleMode = 3;
    meterUpdateSharedState()->mBattleState = battleState;
  } else if (meterUpdateBattleFlag()) {
    meterUpdateResetHearts(i_Meter);
    meterUpdateBattleFlag() = 0;
  } else {
    bool change = meterUpdatePlay()->mItemMaxLifeCount != 0;
    if (!change) {
      f32 value = meterUpdatePlay()->mItemLifeCount;
      scratch->mConvertedLife = static_cast<u32>(ppc_fctiwz(value));
      change = scratch->mLifeHalfword != 0;
    }
    if (change) {
      dComIfG_play_c *play = meterUpdatePlay();
      dSv_player_status_a_c *status = meterUpdatePlayerStatus();
      s16 maxCount = play->mItemMaxLifeCount;
      s16 maxHP = static_cast<s16>(status->mMaxLife + maxCount);
      s16 newHP;
      if (meterUpdatePlay()->mItemMaxLifeCount > 0) {
        newHP = static_cast<s16>((maxHP / 4) * 4);
      } else {
        f32 value = meterUpdatePlay()->mItemLifeCount;
        status = meterUpdatePlayerStatus();
        u32 converted = static_cast<u32>(ppc_fctiwz(value));
        u16 life = status->mLife;
        scratch->mConvertedLife = converted;
        newHP = static_cast<s16>(life + scratch->mLifeHalfword);
      }
      if (meterUpdatePlay()->mItemMaxLifeCount != 0) {
        u8 oldMax = i_Meter->mMaxHP;
        if (maxHP > 80) {
          maxHP = 80;
          i_Meter->mMaxLifeChange = maxHP - oldMax;
          meterUpdatePlayerStatus()->mMaxLife = maxHP;
        } else {
          if (maxHP < 0)
            maxHP = 0;
          i_Meter->mMaxLifeChange = maxHP - oldMax;
          meterUpdatePlayerStatus()->mMaxLife = static_cast<u8>(maxHP);
        }
        meterUpdatePlay()->mItemMaxLifeCount = 0;
        i_Meter->mMagicBar[5].mUserArea = 1;
      }
      s32 fullHearts = (maxHP / 4) * 4;
      u8 oldLife = i_Meter->mCurrHP;
      if (newHP > fullHearts) {
        i_Meter->mAdjustHp = fullHearts - oldLife;
        newHP = static_cast<u8>(fullHearts);
      } else {
        if (newHP < 0)
          newHP = 0;
        i_Meter->mAdjustHp = newHP - oldLife;
        newHP = static_cast<u8>(newHP);
      }
      meterUpdatePlayerStatus()->mLife = newHP;
      meterUpdatePlay()->mItemLifeCount = zero;
      if (i_Meter->mAdjustHp >= 4)
        meterUpdateHeartSoundFlag() = 1;
    }
    s16 maxAdjust = i_Meter->mMaxLifeChange;
    s16 lifeAdjust = i_Meter->mAdjustHp;
    if (maxAdjust < 0) {
      u8 maxHP = i_Meter->mMaxHP;
      maxAdjust = i_Meter->mMaxLifeChange;
      i_Meter->mMaxHP = maxHP - 1;
      i_Meter->mMaxLifeChange = maxAdjust + 1;
    } else if (maxAdjust > 0) {
      u8 maxHP = i_Meter->mMaxHP;
      maxAdjust = i_Meter->mMaxLifeChange;
      i_Meter->mMaxHP = maxHP + 1;
      i_Meter->mMaxLifeChange = maxAdjust - 1;
    }
    if (lifeAdjust != 0) {
      if (lifeAdjust > 0) {
        u8 life = i_Meter->mCurrHP;
        lifeAdjust = i_Meter->mAdjustHp;
        i_Meter->mCurrHP = life + 1;
        i_Meter->mAdjustHp = lifeAdjust - 1;
        lifeAdjust = i_Meter->mAdjustHp;
        if (meterUpdateHeartSoundFlag()) {
          if ((i_Meter->mCurrHP & 3) == 0) {
            meter01_soundStart(0x87C);
            lifeAdjust = i_Meter->mAdjustHp;
          }
          if (lifeAdjust == 0)
            meterUpdateHeartSoundFlag() = 0;
        }
      } else {
        u8 life = i_Meter->mCurrHP;
        lifeAdjust = i_Meter->mAdjustHp;
        i_Meter->mCurrHP = life - 1;
        i_Meter->mAdjustHp = lifeAdjust + 1;
      }
      i_Meter->mHeartLight.mUserArea = 0;
      u8 life = i_Meter->mCurrHP;
      meterUpdatePlay()->mItemNowLife = life;
    }
  }
  auto player = reinterpret_cast<MeterUpdatePlayer *>(
      static_cast<fopAc_ac_c *>(meterUpdatePlay()->mPlayerInfo[0].mpPlayer));
  bool soup = (player->mSoupPowerFlags & 0x8000) != 0;
  if (soup)
    soup = meterUpdatePlay()->mMiniGameType != 6;
  if (soup) {
    s16 active = i_Meter->mHeartShadow[0].mUserArea;
    s16 timer = i_Meter->mHeartShadow[1].mUserArea;
    if (active == 0)
      i_Meter->mHeartShadow[0].mUserArea = 1;
    if (timer >= 29) {
      i_Meter->mHeartShadow[1].mUserArea = 0;
      meterUpdateValueIncrease(15, 0, 0);
    } else {
      timer = static_cast<s16>(timer + 1);
      i_Meter->mHeartShadow[1].mUserArea = timer;
      meterUpdateValueIncrease(15, timer < 15 ? timer : 30 - timer, 0);
    }
  } else if (i_Meter->mHeartShadow[0].mUserArea == 1) {
    i_Meter->mHeartShadow[0].mUserArea = 0;
  }
}

/* inlined into dMeter_Execute */
static void meterUpdateActionFade(sub_meter_class *meter) {
  u32 flags = meter->mStatusFlags;
  bool hide = (flags & (meter_detail::dMtrStts_UNK4000_e)) != 0;
  if (!hide && (flags & (meter_detail::dMtrStts_UNK40_e)) != 0) {
    if (meterUpdatePlay()->mEvtCtrl.mMode != 0 &&
        (meterUpdatePlay()->mEvtCtrl.mHindFlag & 0x10) != 0) {
      hide = (meterUpdatePlay()->mPlayerStatus[0][1] & 0x2000) == 0;
    }
  }
  if (!hide) {
    hide = meterUpdateCollectMode() == 4;
    if (!hide) {
      flags = meter->mStatusFlags;
      hide = (flags &
              (meter_detail::dMtrStts_UNK20_e | meter_detail::dMtrStts_UNK80_e |
               meter_detail::dMtrStts_UNK20000_e |
               meter_detail::dMtrStts_UNK80000_e |
               meter_detail::dMtrStts_UNK100000_e |
               meter_detail::dMtrStts_UNK200000_e |
               meter_detail::dMtrStts_HD1000000_e)) != 0;
      if (!hide) {
        hide = meterUpdateMenu()->mAuctionFlag != 0;
        if (!hide)
          hide = (flags & (meter_detail::dMtrStts_UNK400000_e |
                           meter_detail::dMtrStts_UNK800000_e)) != 0;
      }
    }
  }
  s16 frame;
  if (hide) {
    meterUpdateHeartScaleCountdown(0);
    frame = meter->mHeartAlphaStep;
    frame = frame > 0 ? s16(frame - 1) : 0;
  } else if ((flags & (meter_detail::dMtrStts_UNK8_e |
                       meter_detail::dMtrStts_UNK10_e)) != 0) {
    meterUpdateHeartScaleCountdown(0);
    frame = meter->mHeartAlphaStep;
    if (frame > 3)
      frame = s16(frame - 1);
    else if (frame < 3)
      frame = s16(frame + 1);
    else
      frame = 3;
  } else {
    meterUpdateMaxLifeCountdown(0);
    frame = meter->mHeartAlphaStep;
    if (frame < 5)
      frame = s16(frame + 1);
    else
      frame = 5;
    meter->mHeartAlphaStep = frame;
    if (meterUpdatePlay()->mMiniGameType != 6)
      meterUpdateActionAudio();
    // Reload: the audio callback can change the HUD interpolation frame.
    frame = meter->mHeartAlphaStep;
  }
  meter->mHeartAlphaStep = frame;
  meter->mHeartAlpha = meterUpdateValueIncrease(5, frame, 0);
}

/* inlined into dMeter_Execute */
static void meterUpdateActionForce(sub_meter_class *meter) {
  u8 forcedStatus = meterUpdatePlay()->mDoStatusForce;
  dComIfG_play_c *play = meterUpdatePlay();
  if (forcedStatus != 0) {
    u8 status = play->mDoStatusForce;
    meterUpdatePlay()->mDoStatus = status;
    return;
  }
  u32 flags = meter->mStatusFlags;
  bool blank = (flags & (meter_detail::dMtrStts_UNK4000_e)) != 0;
  if (!blank && (flags & (meter_detail::dMtrStts_UNK40_e)) != 0) {
    if (meterUpdatePlay()->mEvtCtrl.mMode != 0 &&
        (meterUpdatePlay()->mEvtCtrl.mHindFlag & 4) != 0) {
      flags = meter->mStatusFlags;
      blank = (flags & (meter_detail::dMtrStts_UNK20_e)) == 0;
    } else {
      flags = meter->mStatusFlags;
    }
  }
  if (blank) {
    meterUpdatePlay()->mDoStatus = MeterUpdate::dActStts_BLANK_e;
    return;
  }
  if ((flags & (meter_detail::dMtrStts_UNK8_e | meter_detail::dMtrStts_UNK10_e |
                meter_detail::dMtrStts_UNK20_e |
                meter_detail::dMtrStts_UNK800000_e)) != 0) {
    if (meterUpdateCollectMode() != 5) {
      meterUpdatePlay()->mAStatus = MeterUpdate::dActStts_RETURN_e;
      flags = meter->mStatusFlags;
    }
    if ((flags & (meter_detail::dMtrStts_UNK800000_e)) != 0)
      meterUpdatePlay()->mDoStatus = MeterUpdate::dActStts_CHOOSE_e;
    return;
  }
  if ((flags & (meter_detail::dMtrStts_UNK100_e)) == 0)
    return;
  u8 messageMode = meterUpdateMessageMode();
  if (messageMode == 1)
    meterUpdatePlay()->mDoStatus = MeterUpdate::dActStts_NEXT_e;
  else if (messageMode == 3)
    meterUpdatePlay()->mDoStatus = MeterUpdate::dActStts_CHOOSE_e;
  else
    meterUpdatePlay()->mDoStatus = messageMode == 4 ? 0x46 : 0;
  if (meterUpdatePlay()->mDoStatusForce != MeterUpdate::dActStts_BLANK_e)
    meterUpdatePlay()->mDoStatusForce = MeterUpdate::dActStts_BLANK_e;
  if (meterUpdatePlay()->mAStatusForce != MeterUpdate::dActStts_BLANK_e) {
    u8 status = meterUpdatePlay()->mDoStatusForce;
    meterUpdatePlay()->mAStatus = status;
    meterUpdatePlay()->mAStatusForce = MeterUpdate::dActStts_BLANK_e;
  }
}

/* inlined into dMeter_Execute */
static void meterUpdateActionChange(sub_meter_class *meter) {
  if (meter->mActionBowIcon.mUserArea != 0) {
    if (meterUpdatePlay()->mDoStatus == MeterUpdate::dActStts_BLANK_e) {
      meter->mActionButton2[0].mUserArea = 1;
    } else if (meterUpdatePlay()->mDoStatus == MeterUpdate::dActStts_PARRY_e) {
      meter01_soundStart(0x85F);
    } else if (meterUpdatePlay()->mDoStatus !=
                   MeterUpdate::dActStts_ba_sake__dupe_31 &&
               meterUpdatePlay()->mDoStatus !=
                   MeterUpdate::dActStts_ba_nageru__dupe_2D &&
               meterUpdatePlay()->mDoStatus !=
                   MeterUpdate::dActStts_ba_motu__dupe_2E &&
               meterUpdatePlay()->mDoStatus != MeterUpdate::dActStts_bow_01) {
      // HD: retain the archive lookups even though texture selection is
      // separate.
      u8 status = meterUpdatePlay()->mDoStatus;
      if (status != 7)
        meterUpdatePlay();
      meterUpdatePlay();
      meter->mActionButton2[0].mUserArea = 1;
    }
    u8 status = meterUpdatePlay()->mDoStatus;
    meter->mActionBowIcon.mUserArea = 0;
    meter->mLastDoStatus = status;
  }
  dComIfG_play_c *play = meterUpdatePlay();
  u8 oldStatus = meter->mLastDoStatus;
  u8 status = play->mDoStatus;
  if (oldStatus != status)
    meter->mActionBowIcon.mUserArea = 1;
}

/* inlined into dMeter_Execute */
static void meterUpdateSetActionMode(sub_meter_class *meter, s16 mode,
                                     MeterUpdateScratch *scratch,
                                     bool clearButtons = false) {
  if (meterUpdateActionMode() != mode) {
    meterUpdateOffBit(&meter->mButtonMoveFlags, 1);
    if (clearButtons)
      meterUpdateOffBit(&scratch->mButtonFlags, 1);
  }
  meterUpdateActionMode() = mode;
}

/* inlined into dMeter_Execute */
static void meterUpdateSelectActionMode(sub_meter_class *meter,
                                        MeterUpdateScratch *scratch) {
  u32 flags = meter->mStatusFlags;
  bool hidden = (flags & (meter_detail::dMtrStts_UNK4000_e)) != 0;
  if (!hidden && (flags & (meter_detail::dMtrStts_UNK40_e)) != 0) {
    if (meterUpdatePlay()->mEvtCtrl.mMode != 0 &&
        (meterUpdatePlay()->mEvtCtrl.mHindFlag & 2) != 0) {
      flags = meter->mStatusFlags;
      hidden = (flags & (meter_detail::dMtrStts_UNK20_e)) == 0;
    } else
      flags = meter->mStatusFlags;
  }
  if (!hidden)
    hidden = (flags & (meter_detail::dMtrStts_UNK80_e |
                       meter_detail::dMtrStts_UNK100000_e |
                       meter_detail::dMtrStts_HD1000000_e)) != 0;
  if (!hidden && (flags & (meter_detail::dMtrStts_UNK200000_e)) != 0)
    hidden = (flags & (meter_detail::dMtrStts_UNK10_e |
                       meter_detail::dMtrStts_UNK20_e)) == 0;
  if (!hidden)
    hidden = meterUpdatePlay()->mDoStatus == MeterUpdate::dActStts_HIDDEN_e;
  if (hidden) {
    meterUpdateSetActionMode(meter, 1, scratch, true);
    return;
  }
  flags = meter->mStatusFlags;
  if ((flags & (meter_detail::dMtrStts_UNK20_e)) != 0) {
    if ((meterUpdateStage()->mStageFlags & 3) == 1) {
      meterUpdateSetActionMode(meter, 9, scratch);
      return;
    }
    if ((meterUpdateStage()->mStageFlags & 3) != 0)
      return;
    u8 iconMode = meterUpdateButtonIconMode();
    s16 mode;
    if (iconMode <= 3)
      mode = 8;
    else if (iconMode < 6)
      mode = 9;
    else if (iconMode == 6)
      mode = 1;
    else if (iconMode < 11)
      mode = 11;
    else if (iconMode == 11)
      mode = 1;
    else
      mode = 9;
    meterUpdateSetActionMode(meter, mode, scratch, mode == 1);
    return;
  }
  if ((flags & (meter_detail::dMtrStts_UNK20000_e)) != 0) {
    meterUpdateSetActionMode(meter, 2, scratch);
    return;
  }
  if (meterUpdateMenu()->mWindowFlag != 0) {
    u8 button = meterUpdateMenuButton();
    u8 status = 0;
    bool itemMenu = button == 1;
    if (!itemMenu) {
      status = meterUpdateMenu()->mMenuStatus;
      itemMenu = status == 1 && button == 0;
    }
    if (itemMenu) {
      meterUpdateSetActionMode(meter, meterUpdateItemMode() != 0 ? 4 : 3,
                               scratch);
      return;
    }
    if (button == 2 || (status == 2 && button == 0)) {
      meterUpdateSetActionMode(meter, 4, scratch);
      return;
    }
  }
  s16 mode;
  if ((flags & (meter_detail::dMtrStts_UNK800000_e)) != 0)
    mode = 4;
  else if ((flags & (meter_detail::dMtrStts_UNK40000_e)) != 0)
    mode = 5;
  else if ((flags & (meter_detail::dMtrStts_UNK100_e)) != 0)
    mode = 7;
  else if ((flags & (meter_detail::dMtrStts_UNK80000_e)) != 0)
    mode = 6;
  else
    mode = (flags & (meter_detail::dMtrStts_UNK400000_e)) != 0 ? 4 : 0;
  meterUpdateSetActionMode(meter, mode, scratch);
}

/* inlined into dMeter_Execute */
static void meterUpdateActionTrans(sub_meter_class *meter,
                                   MeterUpdateScratch *scratch) {
  scratch->mButtonFlags = meterUpdateButtonFlags();
  meterUpdateSelectActionMode(meter, scratch);
  u8 flags = meter->mButtonMoveFlags;
  if (flags == 15) {
    s16 frame = meterUpdateActionFrame();
    meterUpdateActionFrame() = frame > 0 ? s16(frame - 1) : 0;
  } else if (!meterUpdateIsBit(&meter->mButtonMoveFlags, 1)) {
    if (meterUpdateActionMode() != 1)
      meterUpdateOnBit(&scratch->mButtonFlags, 1);
    s16 frame = meterUpdateActionFrame();
    if (frame >= 5) {
      meterUpdateActionFrame() = 5;
      meterUpdateOnBit(&meter->mButtonMoveFlags, 1);
    } else {
      // HD: keep the scratch-byte load before incrementing the shared frame.
      u8 buttonFlags = scratch->mButtonFlags;
      u8 oldFlags = meterUpdateButtonFlags();
      meterUpdateActionFrame() = frame + 1;
      if (buttonFlags != oldFlags)
        meterUpdateButtonFlags() = buttonFlags;
      return;
    }
  }
  u8 buttonFlags = scratch->mButtonFlags;
  u8 oldFlags = meterUpdateButtonFlags();
  if (buttonFlags != oldFlags)
    meterUpdateButtonFlags() = buttonFlags;
}

/* inlined into dMeter_Execute */
static f32 meterUpdateActionAlpha(sub_meter_class *meter,
                                  MeterUpdateScratch *scratch) {
  bool dim = false;
  if ((meter->mStatusFlags & (meter_detail::dMtrStts_UNK20_e)) != 0 &&
      (meterUpdateStage()->mStageFlags & 3) == 0) {
    u8 mode = meterUpdateButtonIconMode();
    dim = (mode >= 2 && mode <= 3) || (mode >= 7 && mode <= 9);
  }
  s16 frame;
  if (dim) {
    frame = meterUpdateActionAlphaFrame();
    if (frame < 3)
      frame = s16(frame + 1);
    else if (frame > 3)
      frame = s16(frame - 1);
    else
      frame = 3;
    meterUpdateActionAlphaFrame() = frame;
  } else {
    meter->mButtonActionMode = meter->mButtonActionMode | 1;
    frame = meterUpdateActionAlphaFrame();
    meterUpdateActionAlphaFrame() = frame < 5 ? s16(frame + 1) : 5;
    meter->mButtonShowFlags = meter->mButtonShowFlags | 1;
    frame = meterUpdateActionAlphaFrame();
  }
  f32 alpha = meterUpdateValueIncrease(5, frame, 0);
  if (meterUpdatePlay()->mMiniGameType == 8) {
    scratch->mSound = 0x76;
    meterUpdateActorJudge(meterUpdateActorNameJudge, &scratch->mSound);
  }
  return alpha;
}

/* inlined into dMeter_Execute */
static void meterUpdateActionPanes(sub_meter_class *meter, f32 alpha) {
  if (meterUpdatePlay()->mDoStatus == MeterUpdate::dActStts_BLANK_e) {
    meterUpdateSetAlphaZero(&meter->mActionText);
    meterUpdateSetAlphaZero(&meter->mActionButton2[1]);
    meterUpdateSetAlphaZero(&meter->mActionButton[1]);
    meterUpdateSetAlphaZero(&meter->mArrowDigit10);
    meterUpdateSetAlphaZero(&meter->mArrowDigit1);
    meterUpdateSetAlphaZero(&meter->mActionBowIcon);
    meterUpdateSetAlphaZero(&meter->mActionBowIconShadow);
    meterUpdateSetAlpha(&meter->mActionButton2[0], alpha);
    meterUpdateSetAlpha(&meter->mActionButton[0], alpha);
  } else if (meterUpdatePlay()->mDoStatus == MeterUpdate::dActStts_PARRY_e ||
             meterUpdatePlay()->mDoStatus ==
                 MeterUpdate::dActStts_ba_sake__dupe_31 ||
             meterUpdatePlay()->mDoStatus ==
                 MeterUpdate::dActStts_ba_nageru__dupe_2D ||
             meterUpdatePlay()->mDoStatus ==
                 MeterUpdate::dActStts_ba_motu__dupe_2E) {
    meterUpdateSetAlphaZero(&meter->mActionText);
    meterUpdateSetAlpha(&meter->mActionButton2[1], alpha);
    meterUpdateSetAlpha(&meter->mActionButton[1], alpha);
    meterUpdateSetAlphaZero(&meter->mActionButton2[0]);
    meterUpdateSetAlphaZero(&meter->mActionButton[0]);
    meterUpdateSetAlphaZero(&meter->mArrowDigit10);
    meterUpdateSetAlphaZero(&meter->mArrowDigit1);
    meterUpdateSetAlphaZero(&meter->mActionBowIcon);
    meterUpdateSetAlphaZero(&meter->mActionBowIconShadow);
  } else if (meterUpdatePlay()->mDoStatus == MeterUpdate::dActStts_bow_01) {
    meterUpdateSetAlphaZero(&meter->mActionText);
    meterUpdateSetAlphaZero(&meter->mActionButton2[1]);
    meterUpdateSetAlphaZero(&meter->mActionButton[1]);
    meterUpdateSetAlpha(&meter->mActionButton2[0], alpha);
    meterUpdateSetAlpha(&meter->mActionButton[0], alpha);
    meterUpdateSetAlpha(&meter->mArrowDigit10, alpha);
    meterUpdateSetAlpha(&meter->mArrowDigit1, alpha);
    meterUpdateSetAlpha(&meter->mActionBowIcon, alpha);
    meterUpdateSetAlpha(&meter->mActionBowIconShadow, alpha);
  } else {
    meterUpdateSetAlpha(&meter->mActionText, alpha);
    meterUpdateSetAlphaZero(&meter->mActionButton2[1]);
    meterUpdateSetAlphaZero(&meter->mActionButton[1]);
    meterUpdateSetAlpha(&meter->mActionButton2[0], alpha);
    meterUpdateSetAlpha(&meter->mActionButton[0], alpha);
    meterUpdateSetAlphaZero(&meter->mArrowDigit10);
    meterUpdateSetAlphaZero(&meter->mArrowDigit1);
    meterUpdateSetAlphaZero(&meter->mActionBowIcon);
    meterUpdateSetAlphaZero(&meter->mActionBowIconShadow);
  }
  meterUpdateSetAlpha(&meter->mButtonCenter, alpha);
  meterUpdatePlay()->mDoStatusForce = MeterUpdate::dActStts_BLANK_e;
}

/* 02593B10 */
static void dMeter_Execute(sub_meter_class *meter) {
  u32 self = gabi::ea(meter);
  gabi::Local<MeterUpdateScratch> scratch;
  auto *work = gabi::at<MeterUpdateScratch>(scratch.a);
  u32 previousHeap =
      meter01_setCurrentHeap(gabi::at<MeterPrefixHD>(self)->heap);
  meter->mButtonActionMode = 0;
  meter->mButtonShowFlags = 0;
  meterUpdateStatusCheck(meter);
  f32 zero = meterUpdateAlphaControl(meter);
  meterUpdateHeartMove(meter, work, zero);
  meterUpdateActionFade(meter);
  meterUpdateActionForce(meter);
  meterUpdateActionChange(meter);
  meterUpdateActionTrans(meter, work);
  f32 actionAlpha = meterUpdateActionAlpha(meter, work);
  meterUpdateActionPanes(meter, actionAlpha);
  meterUpdateWeaponMove(meter);
  meterUpdateXYMove(meter);
  meterUpdateRAlpha(meter);
  meterUpdateEnemyMove(meter);
  meterUpdateMagicGaugeMove(meter);
  meterUpdateMagicAlpha(meter);
  meterUpdateMetronomeVisibility(meter);
  meterUpdateRupyMove(meter);
  meterUpdateKeyMove(meter);
  meterUpdateCompassMove(meter);
  meterUpdateRecollectBossData();
  meterUpdateMetronomeMove(meter);
  meterUpdateGaugeAlpha(meter);

  bool maskButtons = false;
  if ((meter->mStatusFlags & 2) != 0) {
    auto *system = meterUpdateSystem();
    maskButtons = meterUpdateHudContextActive(system->mHudContext) != 0;
  }
  auto *buttonState = gabi::at<MeterUpdateButtonState>(self);
  if (maskButtons) {
    u8 mode = meter->mButtonActionMode;
    buttonState->mMaskFrames = 2;
    u8 flags = meter->mButtonShowFlags;
    meter->mButtonActionMode = mode & 0x13;
    meter->mButtonShowFlags =
        flags & (meter_detail::dMtrStts_UNK1_e | meter_detail::dMtrStts_UNK2_e |
                 meter_detail::dMtrStts_UNK10_e);
  } else {
    u8 frames = buttonState->mMaskFrames;
    if (frames != 0) {
      u8 mode = meter->mButtonActionMode;
      buttonState->mMaskFrames = frames - 1;
      u8 flags = meter->mButtonShowFlags;
      meter->mButtonActionMode = mode & 0x13;
      meter->mButtonShowFlags = flags & (meter_detail::dMtrStts_UNK1_e |
                                         meter_detail::dMtrStts_UNK2_e |
                                         meter_detail::dMtrStts_UNK10_e);
    }
  }
  u8 mode = meter->mButtonActionMode;
  meterUpdatePlay()->mButtonMode = mode;
  meterUpdateWindMove(meter);
  if (!meterUpdateGetterB() && !meterUpdateIsPause() &&
      meterUpdatePlay()->mEvtCtrl.mMode == 0) {
    if ((meter->mStatusFlags & (meter_detail::dMtrStts_UNK80_e |
                                meter_detail::dMtrStts_UNK20000_e)) == 0) {
      meterUpdateDecTimer(&dComIfGs_info()->mSavedata.mPlayer.mItemRecord);
    }
    dComIfG_play_c *play = meterUpdatePlay();
    if (play->mStartItemTimer == 1) {
      s16 timer = play->mItemTimer;
      if (timer > 0)
        play->mItemTimer = timer - 1;
      else {
        play->mStartItemTimer = 0;
        play->mItemTimer = 0;
      }
    }
  }
  meter01_setCurrentHeap(previousHeap);
  auto *state = meterUpdateSharedState();
  state->mStatusFlags = meter->mStatusFlags;
  state->mButtonShowFlags = meter->mButtonShowFlags;
  state->mHeartAlpha = meter->mHeartAlpha;
  state->mMagicAlpha = meter->mMagicAlpha;
  state->mRupyAlpha = meter->mRupyAlpha;
  state->mKeyAlpha = meter->mKeyAlpha;
  u8 minigame = meterUpdatePlay()->mMiniGameType;
  state->mMiniGameType = minigame;
}


/* inlined into dMeter_Create */
static void dMeter_weponInit(sub_meter_class *i_Meter) {
    i_Meter->mWeaponButton2.mUserArea = 0;
    i_Meter->mLastAStatus = 0;
    dComIfGp_setAStatus(0);
    if (dComIfGp_getAStatus() != 7) dComIfGp_get();
    dComIfGp_get();
    i_Meter->mWeaponIcon.mUserArea = 0;
}

/* inlined into dMeter_Create */
static void dMeter_actionInit(sub_meter_class *i_Meter) {
    i_Meter->mActionButton2[0].mUserArea = 0;
    i_Meter->mActionBowIcon.mUserArea = 0;
    i_Meter->mLastDoStatus = 0;
    dComIfGp_setDoStatus(0);
}

/* inlined into dMeter_Create */
static void dMeter_xyInit(sub_meter_class *i_Meter) {
    i_Meter->mXYButton[0].mUserArea = 0;
    i_Meter->mpForestWaterParticle[0] = 0;
    i_Meter->mpForestWaterParticle[1] = 0;
    for (s32 i = 0; i < 8; i++)
        meterCreateBaitNum(i) = meterCreateSave()->mSavedata.mPlayer.mBagItemRecord.mBaitNum[i];
    for (s32 itemBtn = 0; itemBtn < 3; itemBtn++) {
        u8 slot = meterCreateSave()->mSavedata.mPlayer.mPlayerStatusA.mSelectItem[itemBtn];
        dComIfG_play_c *play = meterUpdatePlay();
        if (slot == 0xFF) {
            play->mSelectItem[itemBtn] = 0xFF;
        } else {
            slot = meterCreateSave()->mSavedata.mPlayer.mPlayerStatusA.mSelectItem[itemBtn];
            play->mSelectItem[itemBtn] = meterCreateSelectedItem(slot);
            slot = meterCreateSave()->mSavedata.mPlayer.mPlayerStatusA.mSelectItem[itemBtn];
            if (meterCreateSelectedItem(slot) == 0xFF)
                meterCreateSave()->mSavedata.mPlayer.mPlayerStatusA.mSelectItem[itemBtn] = 0xFF;
        }
        i_Meter->mLastSelectItem[itemBtn] = dComIfGp_getSelectItem(itemBtn);
        meterCreateXYItemChange(i_Meter, itemBtn);
    }
}

/* inlined into dMeter_Create */
static void dMeter_rInit(sub_meter_class *i_Meter) {
    i_Meter->mRActionIcon.mUserArea = 0;
    i_Meter->mRStatusShown = 0;
    i_Meter->field_0x26d8.mUserArea = 0;
    dComIfGp_setRStatus(0);
    if (dComIfGp_getRStatus() != 7) dComIfGp_get();
    dComIfGp_get();
}

/* inlined into dMeter_Create */
static void dMeter_enemyInit(sub_meter_class *i_Meter, MeterCreateScratch *scratch, f32 &magicScale) {
    auto *prefix = reinterpret_cast<MeterPrefixHD *>(i_Meter);
    // HD: GHS caches the following magic scale during enemy initialization.
    prefix->mBossIconTop.mUserArea = 0;
    prefix->mEnemyLifeBar.mUserArea = 0;
    prefix->mBossLifeBar[1].mUserArea = 0;
    f32 enemyFrame = meterCreateRndF(l_createEnemyRange) + l_createEnemyBase;
    prefix->mBossIconEye1.mUserArea = 0;
    scratch->mInteger = static_cast<u32>(ppc_fctiwz(enemyFrame));
    prefix->mBossIconBottom.mUserArea = scratch->mIntegerLow;
    f32 blinkFrame = meterCreateRndF(l_mapRange) + l_createBlinkBase;
    magicScale = l_createMagicScale;
    scratch->mInteger = static_cast<u32>(ppc_fctiwz(blinkFrame));
    i_Meter->mMagicBar[0].mSizeOrig.x = magicScale;
    prefix->mBossIconEye2.mUserArea = static_cast<u32>(scratch->mInteger) << 1;
}

/* inlined into dMeter_Create */
static void dMeter_magicInit(sub_meter_class *i_Meter, MeterCreateScratch *scratch, f32 zero, f32 magicScale) {
    if (meterCreateMagicFlags() & 2) {
        i_Meter->mDispMaxMagic = meterCreateSave()->mSavedata.mPlayer.mPlayerStatusA.mMaxMagic;
        f32 magic = meterCreateSave()->mSavedata.mPlayer.mPlayerStatusA.mMagic;
        f32 width = i_Meter->mMagicBar[0].mSizeOrig.x;
        f32 length = magic * round25(width);
        length = length * round25(magicScale);
        scratch->mInteger = static_cast<u32>(ppc_fctiwz(length));
        i_Meter->mDispMagic = scratch->mIntegerLow;
    } else {
        i_Meter->mDispMaxMagic = 0;
        i_Meter->mDispMagic = 0;
        be<u8> &flags = meterCreateMagicFlags();
        flags = (u8)flags | 2;
    }
    i_Meter->mMagicBar[0].mUserArea = 0;
    u8 maxMagic = i_Meter->mDispMaxMagic;
    i_Meter->mMagicBar[1].mUserArea = 0;
    i_Meter->mMagicBar[2].mUserArea = 0;
    i_Meter->mMagicBar[3].mUserArea = 0;
    i_Meter->mMagicBar[4].mUserArea = 0;
    i_Meter->mMagicBar[5].mUserArea = 1;
    i_Meter->mMagicGauge1.mUserArea = 0;
    i_Meter->mMagicGauge3.mUserArea = 0;
    i_Meter->mMagicAlpha = zero;
    i_Meter->mMagicGauge2.mUserArea = 0;
    i_Meter->mMagicGaugeShadow1.mUserArea = 0;
    i_Meter->field_0x3019 = 0;
    meterCreateMagicLength(i_Meter, maxMagic * round25(l_createMagicUnit));
    u16 magicLength = i_Meter->mDispMagic;
    meterCreateMagicChange(i_Meter, magicLength / l_createMagicMaximum);
    meterCreateMagicTransNowInit(i_Meter);
    meterCreateSetNowAlphaZero(&i_Meter->mMagicGauge3);
    meterCreateSetNowAlphaZero(&i_Meter->mMagicGauge1);
    meterCreateSetNowAlphaZero(&i_Meter->mMagicGaugeShadow1);
}

/* inlined into dMeter_Create */
static void dMeter_menuInit(sub_meter_class *i_Meter) {
    auto *prefix = reinterpret_cast<MeterPrefixHD *>(i_Meter);
    prefix->mMenuArrowR.mUserArea = 10;
    prefix->mMenuArrowL.mUserArea = 0;
    prefix->mMapCrossKey.mUserArea = 0;
    prefix->mMapCrossArrow[0].mUserArea = meterCreateIsDungeonItemCompass(meterCreateDungeonMemory(), 1);
}

/* inlined into dMeter_Create */
static void dMeter_windInit(sub_meter_class *i_Meter) {
    dMeter_windStatus = 0;
    dMeter_windID = 0xFFFFFFFF;
    meterCreateWindCancelFlag() = 0;
}

/* inlined into dMeter_Create */
static void dMeter_rupyInit(sub_meter_class *i_Meter, f32 zero) {
    i_Meter->mRupyIcon.mUserArea = 0;
    i_Meter->mRupyDigitA[0].mUserArea = 1;
    u16 rupees = meterCreateSave()->mSavedata.mPlayer.mPlayerStatusA.mRupee;
    i_Meter->mRupyCount = rupees;
    meterUpdatePlay()->mItemNowRupee = rupees;
    i_Meter->mRupyAlpha = zero;
}

/* inlined into dMeter_Create */
static void dMeter_keyInit(sub_meter_class *i_Meter, f32 zero) {
    i_Meter->mKeyIcon.mUserArea = 0;
    i_Meter->mKeyDigitA[0].mUserArea = 0;
    i_Meter->mKeyLight2.mUserArea = 1;
    i_Meter->mKeyLight1.mUserArea = 1;
    i_Meter->mKeyAlpha = zero;
    i_Meter->mDispKeyNum = meterCreateKeyNum();
}

/* inlined into dMeter_Create */
static void dMeter_arwInit(sub_meter_class *i_Meter) {
    u32 arrowType = meterCreateArrowType();
    if (arrowType > 3) arrowType = 0;
    for (int i = 0; i < 2; i++) {
        u32 texture = i_Meter->arrowTex[i];
        u32 filename = meterCreateArrowTextureName(arrowType);
        u32 archive = meterCreateItemIconArchive();
        meterCreateReadTexture(texture, 0xC00, 'TIMG', filename, archive);
        meterCreateStoreTexture(i_Meter->arrowTex[i], 0xC00);
        meterUpdateSetAlphaZero(&i_Meter->mArrowTypeIcon[i]);
        meterUpdateSetAlphaZero(&i_Meter->mArrowTypeSlot[i]);
        meter01_setAlpha(&i_Meter->mArrowTypeIcon[i]);
        meter01_setAlpha(&i_Meter->mArrowTypeSlot[i]);
        i_Meter->mpArrowTypeParticle[i] = 0;
    }
}

/* inlined into dMeter_Create */
static void dMeter_screenDataSet(sub_meter_class* i_Meter, MeterCreateScratch *scratch) {
    auto *prefix = reinterpret_cast<MeterPrefixHD *>(i_Meter);
    // HD: pane hiding and picture-color changes are omitted here.
    meterCreateSetPaneData(&prefix->mMapWindArrow, sMainParts1, 'wiy1');
    meterCreateSetPaneData(&prefix->mMapCrossArrow[0], sMainParts1, 'yrig');
    meterCreateSetPaneData(&prefix->mMapCrossArrow[1], sMainParts1, 'yup');
    meterCreateSetPaneData(&prefix->mMapCrossArrow[2], sMainParts1, 'ylef');
    meterCreateSetPaneData(&prefix->mMenuArrowR, sMainParts1, 'r');
    meterCreateSetPaneData(&prefix->mMenuArrowL, sMainParts1, 'l');
    meterCreateSetPaneData(&prefix->mMapCrossKey, sMainParts1, 'crs1');
    prefix->mMapCrossArrow[0].mPosCenterOrig.x -= 2.0f;
    prefix->mMapCrossArrow[1].mPosCenterOrig.y -= 2.0f;
    prefix->mMapCrossArrow[2].mPosCenterOrig.x -= 1.0f;
    meterCreateSetPaneData(&prefix->mBossIconEye1, sMainParts1, 'bey1');
    meterCreateSetPaneData(&prefix->mBossIconEye2, sMainParts1, 'bey2');
    meterCreateSetPaneData(&prefix->mBossIconTop, sMainParts1, 'enem');
    meterCreateSetPaneData(&prefix->mBossIconBottom, sMainParts1, 'enm2');
    meterCreateSetPaneData(&prefix->mBossLifeBar[0], sMainParts1, 'ben1');
    meterCreateSetPaneData(&prefix->mBossLifeBar[1], sMainParts1, 'ben2');
    meterCreateSetPaneData(&prefix->mBossLifeBar[2], sMainParts1, 'ben3');
    meterCreateSetPaneData(&prefix->mBossLifeBar[3], sMainParts1, 'ben4');
    meterCreateSetPaneData(&prefix->mEnemyLifePart3, sMainParts1, 'ene3');
    meterCreateSetPaneData(&prefix->mEnemyLifeBar, sMainParts1, 'ene2');
    meterCreateSetPaneData(&prefix->mEnemyLifePart1, sMainParts1, 'ene1');
    meterCreateSetPaneData(&prefix->mEnemyLifeShadow2, sMainParts1, 'enk2');
    meterCreateSetPaneData(&prefix->mEnemyLifeShadow, sMainParts1, 'enek');
    // HD: copy both native tag tables into the original guest stack slots.
    for (int i = 0; i < 20; i++) scratch->mHeartTags[i] = l_createHeartTags[i];
    for (int i = 0; i < 20; i++) scratch->mHeartShadowTags[i] = l_createHeartShadowTags[i];
    for (s32 i = 0; i < 20; i++) {
        meterCreateSetPaneData(&i_Meter->mHeart[i], sMainParts1, scratch->mHeartTags[i]);
        meterCreateSetPaneData(&i_Meter->mHeartShadow[i], sMainParts1, scratch->mHeartShadowTags[i]);
    }
    meterCreateSetPaneData(&i_Meter->mHeartLight, sMainParts1, 'htfl');
    meterCreateSetPaneData(&i_Meter->mMagicBar[0], sMainParts1, 'mbr1');
    meterCreateSetPaneData(&i_Meter->mMagicBar[1], sMainParts1, 'mbr2');
    meterCreateSetPaneData(&i_Meter->mMagicBar[2], sMainParts1, 'mbr3');
    meterCreateSetPaneData(&i_Meter->mMagicBar[3], sMainParts1, 'mbr4');
    meterCreateSetPaneData(&i_Meter->mMagicBar[4], sMainParts1, 'mbr5');
    meterCreateSetPaneData(&i_Meter->mMagicBar[5], sMainParts1, 'mbr6');
    meterCreateSetPaneData(&i_Meter->mMagicBar[6], sMainParts1, 'mbr7');
    meterCreateSetPaneData(&i_Meter->mMagicBar[7], sMainParts1, 'mbr8');
    meterCreateSetPaneData(&i_Meter->mMagicGauge3, sMainParts1, 'mbb3');
    meterCreateSetPaneData(&i_Meter->mMagicGauge2, sMainParts1, 'mbb2');
    meterCreateSetPaneData(&i_Meter->mMagicGauge1, sMainParts1, 'mbb1');
    meterCreateSetPaneData(&i_Meter->mMagicGaugeShadow2, sMainParts1, 'mbk2');
    meterCreateSetPaneData(&i_Meter->mMagicGaugeShadow1, sMainParts1, 'mbk1');
    meterCreateSetPaneData(&i_Meter->field_0x1210, sMainParts1, 'cmcv');
    meterCreateSetPaneData(&i_Meter->mCompassNeedle, sMainParts1, 'cmmg');
    meterCreateSetPaneData(&i_Meter->mCompassNeedleShadow, sMainParts1, 'cmmk');
    meterCreateSetPaneData(&i_Meter->field_0x12b8, sMainParts1, 'cmrg');
    meterCreateSetPaneData(&i_Meter->mCompassBase, sMainParts1, 'cmb1');
    meterCreateSetPaneData(&i_Meter->mClockBar, sMainParts3, 'bar');
    meterCreateSetPaneData(&i_Meter->mClockRing, sMainParts3, 'rng2');
    meterCreateSetPaneData(&i_Meter->mClockHorizon1, sMainParts3, 'hrz1');
    meterCreateSetPaneData(&i_Meter->mClockHorizon2, sMainParts3, 'hrz2');
    meterCreateSetPaneData(&i_Meter->mClockStar[0], sMainParts3, 'str5');
    meterCreateSetPaneData(&i_Meter->mClockStar[1], sMainParts3, 'str6');
    meterCreateSetPaneData(&i_Meter->mClockStar[2], sMainParts3, 'str7');
    meterCreateSetPaneData(&i_Meter->mClockMoon, sMainParts3, 'mon2');
    meterCreateSetPaneData(&i_Meter->mClockSun, sMainParts3, 'sun2');
    meterCreateSetPaneData(&i_Meter->mClockCircle[0], sMainParts3, 'cir1');
    meterCreateSetPaneData(&i_Meter->mClockCircle[1], sMainParts3, 'cir2');
    meterCreateSetPaneData(&i_Meter->mClockCircle[2], sMainParts3, 'cir3');
    meterCreateSetPaneData(&i_Meter->mClockCircle[3], sMainParts3, 'cir4');
    meterCreateSetPaneData(&i_Meter->mClockTimeDial, sMainParts3, 'cnt2');
    meterCreateSetPaneData(&i_Meter->mClockCenter, sMainParts3, 'cnt1');
    meterCreateSetPaneData(&i_Meter->mFixedClockRing, sMainParts3, 'rng1');
    meterCreateSetPaneData(&i_Meter->mFixedClockStar[0], sMainParts3, 'str1');
    meterCreateSetPaneData(&i_Meter->mFixedClockStar[1], sMainParts3, 'str2');
    meterCreateSetPaneData(&i_Meter->mFixedClockStar[2], sMainParts3, 'str3');
    meterCreateSetPaneData(&i_Meter->mFixedClockStar[3], sMainParts3, 'str4');
    meterCreateSetPaneData(&i_Meter->mFixedClockMoon, sMainParts3, 'mon1');
    meterCreateSetPaneData(&i_Meter->mFixedClockCloudClip, sMainParts3, 'clp');
    meterCreateSetPaneData(&i_Meter->mFixedClockCloud[0], sMainParts3, 'cld1');
    meterCreateSetPaneData(&i_Meter->mFixedClockCloud[1], sMainParts3, 'cld2');
    meterCreateSetPaneData(&i_Meter->mFixedClockCloud[2], sMainParts3, 'cld3');
    meterCreateSetPaneData(&i_Meter->mFixedClockSun, sMainParts3, 'sun1');
    meterCreateSetPaneData(&i_Meter->mCompassClock, sMainParts3, 'cmtm');
    meterCreateSetPaneData(&i_Meter->mKeyLight1, sMainParts1, 'kyl1');
    meterCreateSetPaneData(&i_Meter->mKeyLight2, sMainParts1, 'kyl2');
    meterCreateSetPaneData(&i_Meter->mKeyDigitA[0], sMainParts1, 'nm03');
    meterCreateSetPaneData(&i_Meter->mKeyDigitA[1], sMainParts1, 'nm04');
    meterCreateSetPaneData(&i_Meter->mKeyDigitB[0], sMainParts1, 'nm08');
    meterCreateSetPaneData(&i_Meter->mKeyDigitB[1], sMainParts1, 'nm09');
    meterCreateSetPaneData(&i_Meter->mKeyIcon, sMainParts1, 'key0');
    meterCreateSetPaneData(&i_Meter->mRActionIcon, sMainParts1, 'basv');
    meterCreateSetPaneData(&i_Meter->mWeaponText, sMainParts1, 'bawd');
    meterCreateSetPaneData(&i_Meter->mActionText, sMainParts1, 'bawp');
    meterCreateSetPaneData(&i_Meter->mActionButton[1], sMainParts1, 'baa2');
    meterCreateSetPaneData(&i_Meter->mActionButton2[1], sMainParts1, 'bat2');
    meterCreateSetPaneData(&i_Meter->mItemDigit1[1], sMainParts1, 'y01');
    meterCreateSetPaneData(&i_Meter->mItemDigit10[1], sMainParts1, 'y10');
    meterCreateSetPaneData(&i_Meter->mItemIcon[1], sMainParts1, 'yitm');
    meterCreateSetPaneData(&i_Meter->mItemIconShadow[1], sMainParts1, 'yitk');
    meterCreateSetPaneData(&i_Meter->mBowLight1[1], sMainParts1, 'bly1');
    meterCreateSetPaneData(&i_Meter->mBowLight2[1], sMainParts1, 'bly2');
    meterCreateSetPaneData(&i_Meter->mBowLight3[1], sMainParts1, 'bly3');
    meterCreateSetPaneData(&i_Meter->mItemDigit1[0], sMainParts1, 'x01');
    meterCreateSetPaneData(&i_Meter->mItemDigit10[0], sMainParts1, 'x10');
    meterCreateSetPaneData(&i_Meter->mItemIcon[0], sMainParts1, 'xitm');
    meterCreateSetPaneData(&i_Meter->mItemIconShadow[0], sMainParts1, 'xitk');
    meterCreateSetPaneData(&i_Meter->mBowLight1[0], sMainParts1, 'blx1');
    meterCreateSetPaneData(&i_Meter->mBowLight2[0], sMainParts1, 'blx2');
    meterCreateSetPaneData(&i_Meter->mBowLight3[0], sMainParts1, 'blx3');
    meterCreateSetPaneData(&i_Meter->mItemDigit1[2], sMainParts1, 'z01');
    meterCreateSetPaneData(&i_Meter->mItemDigit10[2], sMainParts1, 'z10');
    meterCreateSetPaneData(&i_Meter->mItemIcon[2], sMainParts1, 'zitm');
    meterCreateSetPaneData(&i_Meter->mItemIconShadow[2], sMainParts1, 'zitk');
    meterCreateSetPaneData(&i_Meter->mBowLight1[2], sMainParts1, 'blz1');
    meterCreateSetPaneData(&i_Meter->mBowLight2[2], sMainParts1, 'blz2');
    meterCreateSetPaneData(&i_Meter->mBowLight3[2], sMainParts1, 'blz3');
    meterCreateSetPaneData(&i_Meter->mZButton, sMainParts1, 'baz');
    meterCreateSetPaneData(&i_Meter->mXYButton[1], sMainParts1, 'bay');
    meterCreateSetPaneData(&i_Meter->field_0x25c0[1], sMainParts1, 'b002');
    meterCreateSetPaneData(&i_Meter->field_0x2630[1], sMainParts1, 'ba02');
    meterCreateSetPaneData(&i_Meter->mXYButton[0], sMainParts1, 'bax1');
    meterCreateSetPaneData(&i_Meter->field_0x25c0[0], sMainParts1, 'b001');
    meterCreateSetPaneData(&i_Meter->field_0x2630[0], sMainParts1, 'ba01');
    meterCreateSetPaneData(&i_Meter->mActionButton[0], sMainParts1, 'baa');
    meterCreateSetPaneData(&i_Meter->mActionButton2[0], sMainParts1, 'bawe');
    meterCreateSetPaneData(&i_Meter->mRButton, sMainParts1, 'bar1');
    meterCreateSetPaneData(&i_Meter->field_0x26d8, sMainParts1, 'ba0r');
    meterCreateSetPaneData(&i_Meter->mArrowDigit10, sMainParts1, 'a10');
    meterCreateSetPaneData(&i_Meter->mArrowDigit1, sMainParts1, 'a01');
    meterCreateSetPaneData(&i_Meter->mActionBowIcon, sMainParts1, 'yum');
    meterCreateSetPaneData(&i_Meter->mActionBowIconShadow, sMainParts1, 'yumk');
    meterCreateSetPaneData(&i_Meter->mWeaponIcon, sMainParts1, 'weit');
    meterCreateSetPaneData(&i_Meter->mWeaponIconShadow, sMainParts1, 'witk');
    meterCreateSetPaneData(&i_Meter->mWeaponButton, sMainParts1, 'bab');
    meterCreateSetPaneData(&i_Meter->mWeaponButton2, sMainParts1, 'baat');
    meterCreateSetPaneData(&i_Meter->mButtonCenter, sMainParts1, 'cent');
    meterCreateSetPaneData(&i_Meter->mArrowTypeIcon[0], sMainParts1, 'arw1');
    meterCreateSetPaneData(&i_Meter->mArrowTypeSlot[0], sMainParts1, 'ip00');
    meterCreateSetPaneData(&i_Meter->mArrowTypeIcon[1], sMainParts1, 'arw2');
    meterCreateSetPaneData(&i_Meter->mArrowTypeSlot[1], sMainParts1, 'ip02');
    meterCreateSetPaneData(&i_Meter->mRupyIcon, sMainParts2, 'rup1');
    meterCreateSetPaneData(&i_Meter->mRupyIconB, sMainParts2, 'rup2');
    meterCreateSetPaneData(&i_Meter->mRupyDigitA[0], sMainParts2, 'nm00');
    meterCreateSetPaneData(&i_Meter->mRupyDigitA[1], sMainParts2, 'nm01');
    meterCreateSetPaneData(&i_Meter->mRupyDigitA[2], sMainParts2, 'nm02');
    meterCreateSetPaneData(&i_Meter->mRupyDigitA[3], sMainParts2, 'nm03');
    meterCreateSetPaneData(&i_Meter->mRupyDigitB[0], sMainParts2, 'nm04');
    meterCreateSetPaneData(&i_Meter->mRupyDigitB[1], sMainParts2, 'nm05');
    meterCreateSetPaneData(&i_Meter->mRupyDigitB[2], sMainParts2, 'nm06');
    meterCreateSetPaneData(&i_Meter->mRupyDigitB[3], sMainParts2, 'nm07');
    i_Meter->mRupyDigitA[1].mUserArea = meterCreateSave()->mSavedata.mPlayer.mPlayerStatusA.mWalletSize;
    meterCreateWalletChange(i_Meter);
}
/* inlined into dMeter_Create */
static void dMeter_screenDataArrowSet(sub_meter_class *i_Meter) {
    meterCreateSetPaneData(&i_Meter->mRoadArrow[0], sChoiceRoad, 'yaz4');
    meterCreateSetPaneData(&i_Meter->mRoadArrow[1], sChoiceRoad, 'yaz3');
    meterCreateSetPaneData(&i_Meter->mRoadArrow[2], sChoiceRoad, 'yaz2');
    meterCreateSetPaneData(&i_Meter->mRoadArrow[3], sChoiceRoad, 'yaz1');
}
/* inlined into dMeter_Create */
static void meterCreateClearSharedState(f32 zero) {
    auto *state = meterUpdateSharedState();
    l_hdState.mStateBytes[0x62 - 0x3E] = 0;
    l_hdState.mStateBytes[0x63 - 0x3E] = 0;
    for (int i = 0; i < 7; i++) l_hdState.mStateBytes[i] = 0;
    for (int i = 0; i < 6; i++) l_hdState.mStateBytes[0x5A - 0x3E + i] = 0;
    l_hdState.mPaneScales[0] = zero;
    l_hdState.mStateBytes[0x5F - 0x3E] = 0;
    l_hdState.mStateBytes[0x60 - 0x3E] = 0;
    state->mStatusFlags = 0;
    for (int i = 1; i < 7; i++) l_hdState.mPaneScales[i] = zero;
    l_hdState.mRoomNumber = 0;
    l_hdState.mPaneScales[7] = zero;
    l_hdState.mStateBytes[0x61 - 0x3E] = 0;
    for (int i = 0; i < 7; i++) {
        l_hdState.mStateBytes[0x53 - 0x3E + i] = 0;
        l_hdState.mStateBytes[0x45 - 0x3E + i] = 0;
        l_hdState.mStateBytes[0x4C - 0x3E + i] = 0;
    }
}

/* 02595C6C */
static s32 dMeter_Create(sub_meter_class *i_Meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<MeterCreateScratch> scratchSlot;
    MeterCreateScratch *scratch = gabi::at<MeterCreateScratch>(scratchSlot.a);
    auto *prefix = reinterpret_cast<MeterPrefixHD *>(i_Meter);
    f32 zero = l_zero;
    auto *menu = meterCreateMenuState();
    menu->mapAlpha = 0;
    menu->texRateY = zero;
    menu->dMeter_place_name = 0;
    menu->texRateX = zero;
    menu->menu_status_old = 0;
    menu->texScaleX = zero;
    menu->dMeter_itemNum = 0;
    menu->rupy_soundSetFlag = 0;
    menu->dMenu_menuFlag = 0;
    menu->subWinFlag = 0;
    menu->dMeter_auctionFlag = 0;
    menu->dMeter_itemMode = 0;
    menu->dMeter_btn_chk = 0;
    menu->menu_status = 0;
    menu->texScaleY = zero;
    menu->dMenu_menuButton = 0;
    menu->rupy_soundOnFlag = 0;
    menu->dMeter_itemTimer = 0;
    menu->dMeter_collectMode = 0;
    menu->dMenu_frame_timer = 0;
    menu->dMeter_itemMoveFlag = 0;
    menu->dMenu_pause = 0;

    prefix->heap = meterCreateExpHeap(0x2A819);
    u32 heap = prefix->heap;
    if (!heap) {
        meterCreateAssert("d_meter.cpp", 0x3894, "i_Meter->heap != (0)");
        heap = prefix->heap;
    }
    u32 oldHeap = meter01_setCurrentHeap(heap);
    // HD: retain allocating J2DPane constructors and the two screen-vtable variants.
    f32 width = 0.0f, height = 0.0f;
    for (int i = 0; i < 4; i++) {
        u32 screen = meterCreateAllocate(0xD4, prefix->heap, 4);
        if (i == 0) {
            width = l_createScreenWidth;
            height = l_createScreenHeight;
        }
        if (screen) {
            J2DPane::Bounds *bounds = &scratch->mScreenBounds[i];
            bounds->i.x = zero;
            bounds->i.y = zero;
            bounds->f.x = width;
            bounds->f.y = height;
            meterCreatePaneConstruct(screen, 0, 1, 'root', bounds);
            auto *pane = gabi::at<MeterCreateScreen>(screen);
            pane->mShow = 0;
            if (i == 0) {
                pane->mColor = 0xFFFFFFFF;
                pane->mVtable = l_createMyScreenVtable;
            } else {
                pane->mVtable = l_createScreenVtable;
                pane->mColor = 0xFFFFFFFF;
            }
        }
        be<u32> *screenSlots[] = {&sMainParts1, &sMainParts2, &sMainParts3, &sChoiceRoad};
        *screenSlots[i] = screen;
        const char *assertions[] = {"sMainParts1 != (0)", "sMainParts2 != (0)",
                                    "sMainParts3 != (0)", "sChoiceRoad != (0)"};
        const char *resources[] = {"main_parts1.blo", "main_parts2.blo",
                                   "main_parts3.blo", "choice_road.blo"};
        const int lines[] = {0x3899, 0x389D, 0x38A1, 0x38A5};
        if (!screen) meterCreateAssert("d_meter.cpp", lines[i], assertions[i]);
        u32 archive = meterCreateMenuArchive();
        meterCreateScreenSet(*screenSlots[i], resources[i], archive);
    }
    for (int i = 0; i < 3; i++) {
        auto *heapObject = gabi::at<MeterCreateHeap>(prefix->heap);
        i_Meter->actionTex[i] = heapObject->alloc(0xC00, 0x20);
        if (!i_Meter->actionTex[i])
            meterCreateAssert("d_meter.cpp", 0x38AA, "i_Meter->actionTex[i] != (0)");
        heapObject = gabi::at<MeterCreateHeap>(prefix->heap);
        i_Meter->xyIconTex[i] = heapObject->alloc(0xC00, 0x20);
        if (!i_Meter->xyIconTex[i])
            meterCreateAssert("d_meter.cpp", 0x38AC, "i_Meter->xyIconTex[i] != (0)");
    }
    for (int i = 0; i < 2; i++) {
        if (i == 0) {
            auto *heapObject = gabi::at<MeterCreateHeap>(prefix->heap);
            i_Meter->moveIconTex[0] = heapObject->alloc(0xC00, 0x20);
            if (!i_Meter->moveIconTex[0])
                meterCreateAssert("d_meter.cpp", 0x38B3, "i_Meter->moveIconTex[i] != (0)");
        }
        auto *heapObject = gabi::at<MeterCreateHeap>(prefix->heap);
        i_Meter->arrowTex[i] = heapObject->alloc(0xC00, 0x20);
        if (!i_Meter->arrowTex[i])
            meterCreateAssert("d_meter.cpp", 0x38B6, "i_Meter->arrowTex[i] != (0)");
    }
    // HD: menu/message processes are created elsewhere; only the map HIO child remains.
    dComIfGp_setDoStatusForce(0);
    dComIfGp_setRStatusForce(0);
    dComIfGp_setAStatusForce(0);
    meterCreateHudMode() = 0x4D;
    g_meter_mapHIO.mNo = meterCreateHIOChild("\x83\x7D\x83\x62\x83\x76\x82\x51", &g_meter_mapHIO);
    dMeter_screenDataSet(i_Meter, scratch);
    dMeter_screenDataArrowSet(i_Meter);
    meterUpdate2DState()->mShow = 1;
    i_Meter->mCompassAlpha = l_one;
    i_Meter->field_0x3027 = 0xFF;
    i_Meter->mCompassFadeTimer = meterUpdateHIO()->mCompassFadeFrames;
    u32 compass = meterCreateIsDungeonItemCompass(meterCreateDungeonMemory(), 1);
    prefix->mMapCrossArrow[0].mUserArea = compass;
    s16 mapStart = g_meter_mapHIO.mMapPosX;
    s16 mapEnd = g_meter_mapHIO.mMapHiddenPosX;
    meterCreateMapControl()->field_0x0 = 0;
    meterCreateMapControl()->field_0x4 = mapEnd - mapStart;
    meterCreateMapControl()->initMapCtrlDisp();
    meterCreateCompassInit(i_Meter);
    for (int i = 0; i < 4; i++) i_Meter->mRoadArrow[i].mUserArea = 0;
    i_Meter->mRoadArrowFrame = 0;
    meter01_setCurrentHeap(prefix->heap);
    meterCreateHeartInit(i_Meter);

    // HD: weapon/action texture work is deferred; preserve the status-lookup sequence.
    dMeter_weponInit(i_Meter);
    dMeter_actionInit(i_Meter);
    dMeter_xyInit(i_Meter);
    dMeter_rInit(i_Meter);
    f32 magicScale;
    dMeter_enemyInit(i_Meter, scratch, magicScale);
    dMeter_magicInit(i_Meter, scratch, zero, magicScale);
    dMeter_menuInit(i_Meter);
    dMeter_windInit(i_Meter);
    i_Meter->mMetronomeState = 0;
    dMeter_rupyInit(i_Meter, zero);
    dMeter_keyInit(i_Meter, zero);
    dMeter_arwInit(i_Meter);
    meterCreateStockTimer();
    meter01_setCurrentHeap(oldHeap);
    meterCreateSetStageLayer(i_Meter);
    i_Meter->mWindItem = 0;
    i_Meter->mWindID = 0xFFFFFFFF;
    meterCreateItemOpenFlag() = 0;
    if (meterCreateGetTimer(&meterCreateSave()->mSavedata.mPlayer.mItemRecord))
        meterCreateTimerFlag() = 1;
    meterCreateClearSharedState(zero);
    reinterpret_cast<MeterUpdateButtonState *>(i_Meter)->mMaskFrames = 0;
    return cPhs_COMPLEATE_e;
}


/* inlined into dMeter_weponMove */
static bool meterWeaponStageEquals(MeterWeaponScratch *scratch, int pair, u32 name) {
    auto left = &scratch->mNames[pair];
    auto right = &scratch->mStages[pair];
    left->text = name;
    left->vtable = MeterWeaponApi::safeStringVtable;
    u32 state = dComIfGp_ea();
    right->vtable = MeterWeaponApi::safeStringVtable;
    right->text = gabi::ea(&gabi::at<dComIfG_inf_c>(state)->play.mCurStage.mName);
    gabi::at<meter_detail::SafeStringVtable>((u32)left->vtable)->cleanup(left);
    gabi::at<meter_detail::SafeStringVtable>((u32)left->vtable)->cleanup(left);
    u32 old = left->text;
    gabi::at<meter_detail::SafeStringVtable>((u32)right->vtable)->cleanup(right);
    if (old == (u32)right->text)
        return true;
    auto a = gabi::at<const be<u8>>((u32)left->text);
    auto b = gabi::at<const be<u8>>((u32)right->text);
    for (u32 i = 0; i < 0x40001; i++) {
        u8 x = a[i];
        u8 y = b[i];
        if (x != y)
            return false;
        if (x == 0)
            return true;
    }
    return false;
}

/* inlined into dMeter_weponMove */
static void meterWeaponBlink(MeterWeaponScratch *scratch, sub_meter_class *i_Meter, f32 ratio) {
    u8 alpha = i_Meter->mWeaponText.mInitAlpha;
    f64 unsignedBias = l_weaponUnsignedBias;
    u32 difference = (u32)alpha - 120;
    scratch->mConversionHigh = 0x43300000;
    u32 signedBits = difference ^ 0x80000000;
    f64 signedBias = l_weaponSignedBias;
    scratch->mConversionLow = alpha;
    f64 convertedAlpha = *reinterpret_cast<be<f64> *>(&scratch->mConversionHigh);
    scratch->mConversionLow = signedBits;
    f64 convertedDifference = *reinterpret_cast<be<f64> *>(&scratch->mConversionHigh);
    f32 base = to_single(convertedAlpha - unsignedBias);
    f32 delta = to_single(convertedDifference - signedBias);
    // HD: GHS keeps the rounded multiply and subtract separate here.
    f32 result = to_single(-(delta * round25(ratio) - base));
    scratch->mConversionHigh = (u32)ppc_fctiwz(result);
    i_Meter->mActionText.mNowAlpha = scratch->mConversionBytes[3];
}

/* inlined into dMeter_weponMove */
static void meterWeaponMoveStatus(sub_meter_class *i_Meter, MeterWeaponScratch *scratch, s16 status) {
    if (l_weaponMoveStatus != status) {
        dMeter_alphaFlagOff(&i_Meter->mButtonMoveFlags, 2);
        if (status == 1)
            dMeter_alphaFlagOff(&scratch->mMoveFlag, 2);
    }
    l_weaponMoveStatus = status;
}

/* 025986DC */
static void dMeter_weponMove(sub_meter_class *i_Meter) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<MeterWeaponScratch> scratch;
    if (MeterWeaponApi::getAStatusForce() != 0) {
        u8 force = MeterWeaponApi::getAStatusForce();
        dComIfGp_setAStatus(force);
    }
    if (i_Meter->mWeaponIcon.mUserArea != 0) {
        if (i_Meter->mLastAStatus == meter_weapon::dActStts_BID_e)
            i_Meter->mWeaponText.mUserArea = 0;
        if (dComIfGp_getAStatus() != meter_weapon::dActStts_BLANK_e &&
            dComIfGp_getAStatus() != meter_weapon::dActStts_sword_01 &&
            dComIfGp_getAStatus() != meter_weapon::dActStts_boko_stick &&
            dComIfGp_getAStatus() != meter_weapon::dActStts_hatchet &&
            dComIfGp_getAStatus() != meter_weapon::dActStts_iron_club &&
            dComIfGp_getAStatus() != meter_weapon::dActStts_longsword &&
            dComIfGp_getAStatus() != meter_weapon::dActStts_spear &&
            dComIfGp_getAStatus() != meter_weapon::dActStts_spear__dupe_3C) {
            if (dComIfGp_getAStatus() != meter_weapon::dActStts_RETURN_e)
                dComIfGp_ea();
            dComIfGp_ea(); // HD: retained resource-archive lookups, even though this path has no texture
                           // calls.
        }
        i_Meter->mWeaponIcon.mUserArea = 0;
    }
    u32 state = dComIfGp_ea();
    u8 oldStatus = i_Meter->mLastAStatus;
    u8 newStatus = gabi::at<dComIfG_inf_c>(state)->play.mAStatus;
    if (oldStatus != newStatus) {
        i_Meter->mWeaponIcon.mUserArea = 1;
        i_Meter->mWeaponButton2.mUserArea = 1;
        i_Meter->mLastAStatus = dComIfGp_getAStatus();
    }
    u8 doStatus = dComIfGp_getDoStatus();
    s16 timer = i_Meter->mActionText.mUserArea;
    if (doStatus == meter_weapon::dActStts_BID_e) {
        timer = (s16)(timer + 1);
        if (timer >= 20)
            timer = 0;
        i_Meter->mActionText.mUserArea = timer;
        f32 ratio = fopMsgM_valueIncrease(10, timer < 10 ? timer : 20 - timer, 0);
        meterWeaponBlink(scratch.get(), i_Meter, ratio);
    } else if (timer != 0) {
        u8 alpha = i_Meter->mWeaponText.mInitAlpha;
        i_Meter->mActionText.mUserArea = 0;
        i_Meter->mActionText.mNowAlpha = alpha;
    }
    if (dComIfGp_getDoStatus() == meter_weapon::dActStts_ba_sake__dupe_31 ||
        dComIfGp_getDoStatus() == meter_weapon::dActStts_ba_nageru__dupe_2D ||
        dComIfGp_getDoStatus() == meter_weapon::dActStts_ba_motu__dupe_2E) {
        timer = (s16)((s16)i_Meter->mActionButton2[1].mUserArea + 1);
        if (timer < 20) {
            i_Meter->mActionButton2[1].mUserArea = timer;
        } else {
            i_Meter->mActionButton2[1].mUserArea = 0;
            if (dComIfGp_getDoStatus() == meter_weapon::dActStts_ba_nageru__dupe_2D) {
                mDoAud_seStart(0x8F8);
            } else {
                u8 status = dComIfGp_getDoStatus();
                mDoAud_seStart(status == meter_weapon::dActStts_ba_motu__dupe_2E ? 0x90A : 0x852);
            }
            timer = i_Meter->mActionButton2[1].mUserArea;
        }
        fopMsgM_valueIncrease(10, timer < 10 ? timer : 20 - timer, 0);
    } else {
        dComIfGp_ea(); // HD: the PARRY-status lookup remains even though its color write was removed.
    }
    s16 animation = i_Meter->mWeaponButton2.mUserArea;
    if (animation != 0 && animation <= 12) {
        animation = (s16)(animation + 1);
        i_Meter->mWeaponButton2.mUserArea = animation > 12 ? 0 : animation;
    }
    u32 flags = i_Meter->mStatusFlags;
    scratch->mMoveFlag = l_hdState.mStateBytes[0];
    bool hidden = (flags & meter_detail::dMtrStts_UNK4000_e) != 0;
    if (!hidden && (flags & meter_detail::dMtrStts_UNK40_e)) {
        if (MeterWeaponApi::eventHind() && !(i_Meter->mStatusFlags & meter_detail::dMtrStts_UNK20_e))
            hidden = true;
        flags = i_Meter->mStatusFlags;
    }
    if (!hidden && (flags & meter_detail::dMtrStts_UNK80_e)) {
        if (dComIfGp_getScopeMesgStatus() != 11)
            hidden = true;
        else
            flags = i_Meter->mStatusFlags;
    }
    if (!hidden && (flags & meter_detail::dMtrStts_UNK100_e) &&
        dComIfGp_getAStatus() != meter_weapon::dActStts_CANCEL_e)
        hidden = true;
    if (!hidden && dComIfGp_getAStatus() == meter_weapon::dActStts_HIDDEN_e)
        hidden = true;
    if (hidden) {
        meterWeaponMoveStatus(i_Meter, scratch.get(), 1);
    } else {
        flags = i_Meter->mStatusFlags;
        if (flags & (meter_detail::dMtrStts_UNK80000_e | meter_detail::dMtrStts_HD1000000_e)) {
            meterWeaponMoveStatus(i_Meter, scratch.get(), 11);
        } else if (flags & meter_detail::dMtrStts_UNK20_e) {
            u16 stage = MeterWeaponApi::stageInfo()->mStageFlags;
            if ((stage & 3) == 1) {
                meterWeaponMoveStatus(i_Meter, scratch.get(), 9);
            } else if ((MeterWeaponApi::stageInfo()->mStageFlags & 3) == 0) {
                u8 mode = l_hdState.mStateBytes[0x25];
                if (mode <= 3)
                    meterWeaponMoveStatus(i_Meter, scratch.get(), 8);
                else if (mode >= 7 && mode <= 10)
                    meterWeaponMoveStatus(i_Meter, scratch.get(), 10);
                else if (mode == 6)
                    meterWeaponMoveStatus(i_Meter, scratch.get(), 1);
                else
                    meterWeaponMoveStatus(i_Meter, scratch.get(), 9);
            }
        } else if ((flags & (meter_detail::dMtrStts_UNK100000_e | meter_detail::dMtrStts_UNK80_e)) ||
                   ((flags & meter_detail::dMtrStts_UNK200000_e) &&
                    !(flags & meter_detail::dMtrStts_UNK10_e))) {
            meterWeaponMoveStatus(i_Meter, scratch.get(), 2);
        } else if (flags & meter_detail::dMtrStts_UNK20000_e) {
            meterWeaponMoveStatus(i_Meter, scratch.get(), 3);
        } else {
            // HD: menu reads follow the same short-circuit and retain the loaded mode.
            u8 menuButton = 0, menuStatus = 0;
            bool menu = false;
            if (l_menuPause) {
                menuButton = l_weaponMenuButton;
                if (menuButton == 1)
                    menu = true;
                else {
                    menuStatus = l_weaponMenuStatus;
                    menu = menuStatus == 1 && menuButton == 0;
                }
            }
            if (menu && l_weaponItemMode == 0)
                meterWeaponMoveStatus(i_Meter, scratch.get(), 4);
            else if (menu || (l_menuPause && (menuButton == 2 || (menuStatus == 2 && menuButton == 0))) ||
                     (flags & meter_detail::dMtrStts_UNK800000_e))
                meterWeaponMoveStatus(i_Meter, scratch.get(), 5);
            else if (flags & meter_detail::dMtrStts_UNK40000_e)
                meterWeaponMoveStatus(i_Meter, scratch.get(), 6);
            else if (flags & meter_detail::dMtrStts_UNK400000_e)
                meterWeaponMoveStatus(i_Meter, scratch.get(), 5);
            else if ((flags & meter_detail::dMtrStts_UNK100_e) &&
                     dComIfGp_getAStatus() == meter_weapon::dActStts_CANCEL_e)
                meterWeaponMoveStatus(i_Meter, scratch.get(), 7);
            else
                meterWeaponMoveStatus(i_Meter, scratch.get(), 0);
        }
    }
    if (i_Meter->mButtonMoveFlags != 15) {
        if (!dMeter_alphaFlagCheck(&i_Meter->mButtonMoveFlags, 2)) {
            if (l_weaponMoveStatus != 1)
                dMeter_alphaFlagOn(&scratch->mMoveFlag, 2);
            s16 frame = l_weaponMoveFrame;
            if (frame >= 5) {
                l_weaponMoveFrame = 5;
                dMeter_alphaFlagOn(&i_Meter->mButtonMoveFlags, 2);
            } else
                l_weaponMoveFrame = frame + 1;
        }
    } else {
        s16 frame = l_weaponMoveFrame;
        l_weaponMoveFrame = frame > 0 ? frame - 1 : 0;
    }
    if (scratch->mMoveFlag != (u8)l_hdState.mStateBytes[0])
        l_hdState.mStateBytes[0] = scratch->mMoveFlag;

    flags = i_Meter->mStatusFlags;
    bool dim = true;
    if ((flags & meter_detail::dMtrStts_UNK400_e) &&
        dComIfGp_getAStatus() == meter_weapon::dActStts_sword_01) {
    } else {
        if (flags & meter_detail::dMtrStts_UNK400_e)
            flags = i_Meter->mStatusFlags;
        if ((flags & meter_detail::dMtrStts_UNK2000_e) &&
            dComIfGp_getAStatus() != meter_weapon::dActStts_SPEAK_e) {
        } else {
            if (flags & meter_detail::dMtrStts_UNK2000_e)
                flags = i_Meter->mStatusFlags;
            bool stageDim = false;
            if ((flags & meter_detail::dMtrStts_UNK2_e) &&
                dComIfGp_getAStatus() == meter_weapon::dActStts_sword_01) {
                stageDim = !meterWeaponStageEquals(scratch.get(), 0, gabi::guest_addr("Ojhous")) &&
                           !meterWeaponStageEquals(scratch.get(), 1, gabi::guest_addr("Orichh"));
            }
            if (!stageDim) {
                flags = i_Meter->mStatusFlags;
                dim = (flags & (meter_detail::dMtrStts_UNK8000_e | meter_detail::dMtrStts_UNK10000_e)) != 0;
            }
        }
    }
    f32 alpha;
    if (dim) {
        s16 frame = l_weaponAlphaFrame;
        if (frame < 3)
            frame = (s16)(frame + 1);
        else if (frame > 3)
            frame = (s16)(frame - 1);
        l_weaponAlphaFrame = frame;
        alpha = fopMsgM_valueIncrease(5, frame, 0);
    } else {
        flags = i_Meter->mStatusFlags;
        bool restricted = false;
        if ((flags & meter_detail::dMtrStts_UNK2_e) &&
            (dComIfGp_getAStatus() == meter_weapon::dActStts_sword_01 ||
             dComIfGp_getAStatus() == meter_weapon::dActStts_BLANK_e || l_menuPause)) {
            restricted = !meterWeaponStageEquals(scratch.get(), 2, gabi::guest_addr("Ojhous")) &&
                         !meterWeaponStageEquals(scratch.get(), 3, gabi::guest_addr("Orichh"));
        }
        if (!restricted)
            i_Meter->mButtonActionMode |= 2;
        s16 frame = l_weaponAlphaFrame;
        l_weaponAlphaFrame = frame < 5 ? frame + 1 : 5;
        i_Meter->mButtonShowFlags |= 2;
        alpha = fopMsgM_valueIncrease(5, l_weaponAlphaFrame, 0);
    }
    if (dComIfGp_getAStatus() == meter_weapon::dActStts_BLANK_e) {
        fopMsgM_setNowAlphaZero(&i_Meter->mWeaponText);
        fopMsgM_setNowAlphaZero(&i_Meter->mWeaponIcon);
        fopMsgM_setNowAlphaZero(&i_Meter->mWeaponIconShadow);
        fopMsgM_setNowAlpha(&i_Meter->mWeaponButton, alpha);
        fopMsgM_setNowAlpha(&i_Meter->mWeaponButton2, alpha);
    } else if (dComIfGp_getAStatus() == meter_weapon::dActStts_sword_01 ||
               dComIfGp_getAStatus() == meter_weapon::dActStts_boko_stick ||
               dComIfGp_getAStatus() == meter_weapon::dActStts_hatchet ||
               dComIfGp_getAStatus() == meter_weapon::dActStts_iron_club ||
               dComIfGp_getAStatus() == meter_weapon::dActStts_longsword ||
               dComIfGp_getAStatus() == meter_weapon::dActStts_spear ||
               dComIfGp_getAStatus() == meter_weapon::dActStts_spear__dupe_3C) {
        fopMsgM_setNowAlphaZero(&i_Meter->mWeaponText);
        fopMsgM_setNowAlpha(&i_Meter->mWeaponIcon, alpha);
        fopMsgM_setNowAlpha(&i_Meter->mWeaponIconShadow, alpha);
        fopMsgM_setNowAlpha(&i_Meter->mWeaponButton, alpha);
        fopMsgM_setNowAlpha(&i_Meter->mWeaponButton2, alpha);
        fopMsgM_setNowAlpha(&i_Meter->mButtonCenter, alpha);
    } else {
        fopMsgM_setNowAlpha(&i_Meter->mWeaponText, alpha);
        fopMsgM_setNowAlphaZero(&i_Meter->mWeaponIcon);
        fopMsgM_setNowAlphaZero(&i_Meter->mWeaponIconShadow);
        fopMsgM_setNowAlpha(&i_Meter->mWeaponButton, alpha);
        fopMsgM_setNowAlpha(&i_Meter->mWeaponButton2, alpha);
        fopMsgM_setNowAlpha(&i_Meter->mButtonCenter, alpha);
    }
    dComIfGp_setAStatusForce(meter_weapon::dActStts_BLANK_e);
}


static MeterWeaponInitializeWords l_meterInitializeWords;
static MeterWeaponRegistrations l_meterRegistrations;
static MeterWeaponHeaderBytes l_meterHeaderBytes;
static MeterWeaponTextureState l_meterTextureState;
static MeterWeaponDrawList l_meter1;
static MeterWeaponDrawList l_meter2;
static MeterWeaponDrawList l_meterTekari;
static dMeter_HIO_c g_meterHIO;
static dMeter_menuHIO_c g_menuHIO;
static dMeter_msg_HIO_c g_msgHIO;
static dMeter_message_HIO_c g_messageHIO;
static f32 l_meterRateX;
static f32 l_meterRateY;
static f32 l_meterScale;
static f32 l_meterScaleMax;
static f64 l_weaponUnsignedBias;
static f64 l_weaponSignedBias;
static s16 l_weaponMoveFrame;
static s16 l_weaponMoveStatus;
static s16 l_weaponAlphaFrame;
static u8 l_weaponMenuButton;
static u8 l_weaponMenuStatus;
static u8 l_weaponItemMode;
static f32 l_messageScale;
static f32 l_mapIconScale;
static u8 l_itemMoveFlag;
static u8 l_menuPause;
static MeterWeaponClockArray l_meterClock;

// clang-format off
/* WWHD glue
str 1005103C "Ojhous"
str 10051044 "Orichh"
ext 028F026C void MeterWeaponApi::registerGlobal(void*)
ext 028ED6F8 void MeterWeaponApi::initializeHeaderByte(void*)
ext 028EAB2C void MeterWeaponApi::initializeHeaderFlag(void*)
ext 0252CCBC void MeterWeaponApi::constructDrawList(MeterWeaponDrawList*)
ext 028EFFD0 void MeterWeaponApi::constructArray(void*,u32,u32,u32)
data l_meterScaleMax 100514FC
data l_meterScale 100514F8
data l_meterRateY 100514F4
data l_meterRateX 100514F0
data g_messageHIO 1047AFBC
data g_msgHIO 1047AF34
data g_menuHIO 1047AE6C
data g_meterHIO 1047AD14
data l_meterTekari 1047A9DC
data l_meter2 1047A998
data l_meter1 1047A994
data l_meterTextureState 1047A99C
data l_meterHeaderBytes 1047A9C4
data l_meterRegistrations 101EA0A0
data l_meterInitializeWords 1047A9CC
ext 025DB690 void fopMsgM_setNowAlpha(fopMsgM_pane_class*,f32)
ext 025DB6E0 void fopMsgM_setNowAlphaZero(fopMsgM_pane_class*)
ext 025DB6F4 f32 fopMsgM_valueIncrease(s32,s32,u32)
ext 025E1988 void mDoAud_seStart(u32)
ext 0259173C void dMeter_alphaFlagOff(void*,u32)
ext 02591718 u32 dMeter_alphaFlagCheck(void*,u32)
ext 0259172C void dMeter_alphaFlagOn(void*,u32)
data l_weaponUnsignedBias 10051340
data l_weaponSignedBias 10051308
data l_weaponMoveFrame 101EA046
data l_weaponMoveStatus 101EA048
data l_weaponAlphaFrame 101EA04A
data l_weaponMenuButton 101EA06A
data l_weaponMenuStatus 101EA067
data l_weaponItemMode 101EA072
ext 027F0688 void MeterWeaponApi::drawScreen(u32, MeterWeaponGraf*, f32, f32)
ext 028F0164 void MeterWeaponApi::destroyArray(void*, u32, u32, u32, u32, u32)
data l_messageScale 100514E8
data l_mapIconScale 100514EC
data l_itemMoveFlag 101EA06C
data l_menuPause 101EA069
data l_meterClock 1047AAA0
data l_createHeartTags 10051214
data l_createHeartShadowTags 10051264
data l_createMagicMaximum 10051348
data l_createMagicUnit 100512C0
data l_createMagicScale 10051310
data l_createBlinkBase 100512BC
data l_createEnemyBase 10051338
data l_createEnemyRange 10051334
data l_createScreenHeight 1005132C
data l_createScreenWidth 10051328
data l_hdState 1047B054
data sMainParts1 1047B0A0
data sMainParts2 1047B0A4
data sMainParts3 1047B0A8
data sScrTimer1 101EA0F4
data sScrTimer2 101EA0F8
data sChoiceRoad 101EA0FC
data moveItemPane 101EA100
data dMeter_windID 101EA040
data rupy_soundOnFlag 101EA076
data rupy_soundSetFlag 101EA075
data dMeter_windStatus 101EA074
data g_meter_mapHIO 1047ACEC
data l_mapState 1047A948
data l_iconFreeScale 1047A88C
data l_iconSelfScale 1047A890
data l_hioValues 10051488
data l_zero 100512B8
data l_one 100512B4
data l_half 100512C8
data l_mapRange 1005133C
ext 024F8044 MeterUpdateCamera* meterUpdateCameraBody()
ext 025445B8 s32 meterUpdateStartCheckOld(dEvent_manager_c*,const char*)
ext 027200D0 u32 meterUpdateGetPictureRecord(u32)
ext 0271FC8C u32 meterUpdatePictureActive(u32)
str 10051124 "PUZZLE_GAME"
str 10051194 "PUZZLE_RUPEE"
ext 025DB6F4 f32 meterUpdateValueIncrease(s32,s32,u32)
ext 02591B4C void meterUpdateResetHearts(sub_meter_class*)
ext 02591AB4 void meterUpdateHeartScaleCountdown(u32)
ext 02591B00 void meterUpdateMaxLifeCountdown(u32)
ext 025E1C84 void meterUpdateActionAudio()
ext 025986DC void meterUpdateWeaponMove(sub_meter_class*)
ext 02599584 void meterUpdateXYMove(sub_meter_class*)
ext 0259A5B0 void meterUpdateRAlpha(sub_meter_class*)
ext 0259ACD4 void meterUpdateEnemyMove(sub_meter_class*)
ext 0259B698 void meterUpdateMagicGaugeMove(sub_meter_class*)
ext 0259C0EC void meterUpdateMagicAlpha(sub_meter_class*)
ext 0259C3CC void meterUpdateMetronomeVisibility(sub_meter_class*)
ext 0259C454 void meterUpdateRupyMove(sub_meter_class*)
ext 0259C7FC void meterUpdateKeyMove(sub_meter_class*)
ext 0259CCA8 void meterUpdateCompassMove(sub_meter_class*)
ext 0259188C void meterUpdateRecollectBossData()
ext 0259CDC4 void meterUpdateMetronomeMove(sub_meter_class*)
ext 0259C2BC void meterUpdateGaugeAlpha(sub_meter_class*)
ext 0259CEA0 void meterUpdateWindMove(sub_meter_class*)
ext 025986BC BOOL meterUpdateGetterB()
ext 025AF2A4 BOOL meterUpdateIsPause()
ext 025B614C void meterUpdateDecTimer(dSv_player_item_record_c*)
ext 02693E50 BOOL meterUpdateHudContextActive(u32)
ext 025DB690 void meterUpdateSetAlpha(fopMsgM_pane_class*,f32)
ext 025DB6E0 void meterUpdateSetAlphaZero(fopMsgM_pane_class*)
ext 025E121C void* meterUpdateActorNameJudge(void*,void*) callback
ext 025D5218 void* meterUpdateActorJudge(MeterUpdateActorJudgeFn,void*)
ext 02591718 BOOL meterUpdateIsBit(be<u8>*,u8)
ext 0259172C void meterUpdateOnBit(be<u8>*,u8)
ext 0259173C void meterUpdateOffBit(be<u8>*,u8)
ext 025DB8A4 u32 meterCreateExpHeap(u32)
ext 0273B050 u32 meterCreateAllocate(u32,u32,s32)
ext 027EE5DC u32 meterCreatePaneConstruct(u32,u32,u32,u32,J2DPane::Bounds*)
ext 027F05BC void meterCreateScreenSet(u32,const char*,u32)
ext 0273AA24 u32 meterCreateAssert(const char*,s32,const char*)
ext 025F0A10 u32 meterCreateHIOChild(const char*,dMeter_map_HIO_c*)
ext 025DB5C0 void meterCreateSetPaneData(fopMsgM_pane_class*,u32,u32)
ext 025931CC void meterCreateWalletChange(sub_meter_class*)
ext 025B9100 u32 meterCreateIsDungeonItemCompass(u32,u32)
ext 02593434 void meterCreateCompassInit(sub_meter_class*)
ext 02591B4C void meterCreateHeartInit(sub_meter_class*)
ext 02591F68 void meterCreateXYItemChange(sub_meter_class*,u32)
ext 020198D8 f32 meterCreateRndF(f32)
ext 02592888 void meterCreateMagicLength(sub_meter_class*,f32)
ext 02592A88 void meterCreateMagicChange(sub_meter_class*,f32)
ext 0259295C void meterCreateMagicTransNowInit(sub_meter_class*)
ext 025DB67C void meterCreateSetNowAlphaZero(fopMsgM_pane_class*)
ext 02526C9C u32 meterCreateArrowType()
ext 027EAA2C void meterCreateReadTexture(u32,u32,u32,u32,u32)
ext C00088B8 void meterCreateStoreTexture(u32,u32)
ext 025C61E4 void meterCreateStockTimer()
ext 025DB2BC void meterCreateSetStageLayer(sub_meter_class*)
ext 025B61C8 u32 meterCreateGetTimer(dSv_player_item_record_c*)
str 1005138C "d_meter.cpp"
str 10051398 "i_Meter->arrowTex[i] != (0)"
str 100513B4 "i_Meter->heap != (0)"
str 100513CC "sMainParts1 != (0)"
str 100513E0 "sMainParts2 != (0)"
str 100513F4 "sMainParts3 != (0)"
str 10051408 "sChoiceRoad != (0)"
str 1005141C "i_Meter->actionTex[i] != (0)"
str 1005143C "i_Meter->xyIconTex[i] != (0)"
str 1005145C "i_Meter->moveIconTex[i] != (0)"
str 1005134C "main_parts1.blo"
str 1005135C "main_parts2.blo"
str 1005136C "main_parts3.blo"
str 1005137C "choice_road.blo"
str 1005147C "\x83\x7D\x83\x62\x83\x76\x82\x51"
*/
// clang-format on

// HUD lifetime and HIO layouts.
// HD: HUD panes follow a four-byte larger message prefix. HIO objects omit the
// GameCube host-I/O number; aligned fields retain the measured HD padding.
#pragma once
#include "d/d_meter_02.h"
using meter_detail::sub_meter_class;
struct dMeter_HIO_c {
    /* 0x00 */ be<u32> mVtable;
    /* 0x004 */ be<s16> mCompassFadeFrames;  /* was field_0x6; was mHudFadeFrames (misnamed): WWHD offset 0x4 (GameCube 0x6): the fade length in frames of the compass alpha (sub_meter_class mCompassAlpha, which TWW dMeter_compassAlpha applies to the compass and map panes); the alpha step inlined into dMeter_Execute 02593B10 (meterUpdateAlphaControl) fades mCompassAlpha by valueIncrease(mCompassFadeFrames, mCompassFadeTimer) and sets the timer to it when fully shown; dMeter_Create 02595C6C starts the timer at it; constructor 025974DC default 10; same code TWW d_meter.cpp (dMeter_alphaControl, dMeter_Create) */
    /* 0x006 */ be<s16> mReserved6;
    /* 0x008 */ be<s16> mReserved8;
    u8 mPaddingA[0x2];
    /* 0x00C */ be<f32> mBossLifeRatioDefault;  /* was field_0xc: dMeter_enemyMove 0259ACD4: the boss life ratio shown when the boss actor's max_health is 0 (else health / max_health); only the constructor 025974DC writes it (1.0); same use TWW dMeter_bossEnemyMove */
    /* 0x010 */ be<f32> mHeartScale;  /* was field_0x10: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_setNowHeartScaleXY sizes the current heart to its original size * it (tww default 1.4) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x014 */ be<f32> field_0x14;
    /* 0x018 */ be<f32> field_0x18;
    /* 0x01C */ be<f32> field_0x1c;
    /* 0x020 */ be<f32> field_0x20;
    /* 0x024 */ be<f32> mShutterBladeAngle;  /* was field_0x24: WWHD writes it only in the constructor 025974DC; tww reads it: d_picture_box.cpp dJle_Pb_c::clickShutterMode / shutterChange turn each of the 12 shutter blades by its base angle + it * the shutter progress (tww default 150.0) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x028 */ be<f32> mScopeWipeMaxScale;
    /* 0x02C */ be<u8> mScopeWipeAlpha;
    /* 0x02D */ be<u8> field_0x2d;
    /* 0x02E */ be<s16> field_0x2e;
    /* 0x030 */ be<s16> field_0x30;
    /* 0x032 */ be<s16> field_0x32;
    /* 0x034 */ be<s16> field_0x34;
    /* 0x036 */ be<s16> field_0x36;
    /* 0x038 */ be<s16> field_0x38;
    u8 mPadding3A[0x2];
    /* 0x03C */ be<f32> mHeartLightScale;  /* was field_0x3c: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_heartLightMove scales the heart light by 1 + it * the pulse value (tww default 0.65) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x040 */ be<s16> mButtonScale;  /* was field_0x40: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_Execute scales the button group about mButtonCenter by it / 100 (dMeter_parentPaneTrans / dMeter_childPaneTrans for the weapon, action and item buttons; tww default 0x5A) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x042 */ be<s16> field_0x42;
    /* 0x044 */ be<s16> mButtonTextScale;  /* was field_0x44: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_Execute passes it / 100 as the second scale of dMeter_childPaneTransOnly for mWeaponText and mActionText (tww default 100) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x046 */ be<s16> mButtonIconScale;  /* was field_0x46: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_Execute passes it / 100 as the second scale for mWeaponIcon / mWeaponIconShadow and the X / Y / Z mItemIcon, digits and shadows (dMeter_childPaneTransOnly / ChildTrans; tww default 0x5A) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x048 */ be<f32> mBattleHeartScale;  /* was field_0x48: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_setHeartScaleXY2 / dMeter_setNowHeartScaleXY2, used by dMeter_BattleLifeMove, multiply the heart size by it (tww default 1.8) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x04C */ be<f32> mBattleLifePosX;  /* was field_0x4c: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_BattleLifeMove adds it to the hearts' original x (tww default 12.0) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x050 */ be<s16> mButtonOffsetX;  /* was field_0x50: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_parentPaneTrans and the dMeter_childPaneTrans* helpers add it to the button panes' x (tww default 7) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x052 */ be<s16> mButtonOffsetY;  /* was field_0x52: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_parentPaneTrans and the dMeter_childPaneTrans* helpers add it to the button panes' y (tww default -12) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x054 */ be<s16> field_0x54;
    /* 0x056 */ be<s16> field_0x56;
    /* 0x058 */ be<f32> mBattleLifeStepX;  /* was field_0x58: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_BattleLifeMove spaces the hearts by it * scale * index in x (tww default 2.3) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x05C */ be<f32> mBattleLifePosY;  /* was field_0x5c: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_BattleLifeMove adds it to the hearts' original y (tww default 12.0) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x060 */ be<u8> field_0x60;
    /* 0x061 */ be<u8> field_0x61;
    /* 0x062 */ be<u8> mIgnoreArrowNumCount;  /* was field_0x62: dMeter_xyMove 02599584: when set, the pending play mItemArrowNumCount is cleared before it is added to the saved arrow count (the change is dropped); only the constructor 025974DC writes it (0); same use TWW dMeter_xyItemCountUp */
    /* 0x063 */ be<u8> mIgnoreBombNumCount;  /* was field_0x63: dMeter_xyMove 02599584: when set, the pending play mItemBombNumCount is cleared before it is added to the saved bomb count (the change is dropped); only the constructor 025974DC writes it (0); same use TWW dMeter_xyItemCountUp */
    /* 0x064 */ be<s16> mMapWindArrowOffsetX;  /* was field_0x64: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_menuPlusMove places mMapWindArrow at the map corner x - 7 + it (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x066 */ be<s16> mMapWindArrowOffsetY;  /* was field_0x66: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_menuPlusMove places mMapWindArrow at the map corner y + 9 + it (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x068 */ be<s16> field_0x68;
    /* 0x06A */ be<s16> field_0x6a;
    /* 0x06C */ be<s16> mClockCloudPosX[3];  /* was field_0x6c: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_clockMove sets mFixedClockCloud[i].mPosCenter.x (the scroll of cloud layer i) to [i] (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x072 */ be<s16> mClockCloudPosY[3];  /* was field_0x72: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_clockMove sets mFixedClockCloud[i].mPosCenter.y (the scroll of cloud layer i) to [i] (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x078 */ be<s16> mClockCloudScale[3];  /* was field_0x78: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_clockMultiMove scales cloud layer clock[i] by 100 / [i] (percent) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x07E */ be<s16> mSwimLightSize;  /* was field_0x7e: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimLightAnime sets each mSwimTimerLight's width and height to it (tww default 0x14) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x080 */ be<s16> mReserved80;
    /* 0x082 */ be<s16> mReserved82;
    /* 0x084 */ be<s16> mReserved84;
    /* 0x086 */ be<s16> mReserved86;
    /* 0x088 */ be<s16> mKeyLightStep0;  /* was field_0x88: dMeter_keyMove 0259C7FC: mKeyLight2Frames[i+1] = previous frame + mKeyLightStep[i] + cM_rndFX(mKeyLightStepRange[i]), i = 0..2 (0x88/0x8A/0x8C) */
    /* 0x08A */ be<s16> mKeyLightStep1;  /* was field_0x8a: second mKeyLightStep (the d_meter_02.h view reads 0x88..0x8C as mKeyLightStep[3], dMeter_keyMove 0259C7FC) */
    /* 0x08C */ be<s16> mKeyLightStep2;  /* was field_0x8c: third mKeyLightStep (view mKeyLightStep[2], dMeter_keyMove 0259C7FC) */
    /* 0x08E */ be<s16> mKeyLightEndStep;  /* was field_0x8e: dMeter_keyMove 0259C7FC: mKeyLight2Frames[4] = mKeyLight2Frames[3] + mKeyLightEndStep + cM_rndFX(mKeyLightEndRange) */
    /* 0x090 */ be<s16> mKeyLightStart;  /* was field_0x90: dMeter_keyMove 0259C7FC: base of mKeyLight2Frames[0] (plus cM_rndFX(mKeyLightStartRange)) */
    /* 0x092 */ be<s16> mKeyLightStepRange0;  /* was field_0x92: dMeter_keyMove 0259C7FC: cM_rndFX ranges for the mKeyLightStep steps (0x92/0x94/0x96) */
    /* 0x094 */ be<s16> mKeyLightStepRange1;  /* was field_0x94: second mKeyLightStepRange (view mKeyLightStepRange[1], dMeter_keyMove 0259C7FC) */
    /* 0x096 */ be<s16> mKeyLightStepRange2;  /* was field_0x96: third mKeyLightStepRange (view mKeyLightStepRange[2], dMeter_keyMove 0259C7FC) */
    /* 0x098 */ be<s16> mKeyLightEndRange;  /* was field_0x98: dMeter_keyMove 0259C7FC: cM_rndFX range of the last mKeyLight2Frames step */
    /* 0x09A */ be<s16> mKeyLightStartRange;  /* was field_0x9a: dMeter_keyMove 0259C7FC: cM_rndFX range added to mKeyLightStart for mKeyLight2Frames[0] */
    /* 0x09C */ be<s16> mMapCrossKeyPosX;  /* was field_0x9c: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_menuPlusMove places mMapCrossKey (and the map cross arrows around it) at it, + 0x52 while the compass is open (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x09E */ be<s16> mMapCrossKeyPosY;  /* was field_0x9e: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_menuPlusMove places mMapCrossKey (and the map cross arrows around it) at it (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x0A0 */ be<s16> mButtonPos[8][2][4];  /* was field_0xa0: WWHD writes it only in the constructor 025974DC; tww reads it: 8 rows (one per HUD layout state) of 4 x then 4 y positions: dMeter_actionTrans reads column 0 (A, its second / exp position from column 2 in rows 1, 4, 6), dMeter_weponTrans column 1 (B, also column 3 in row 6), dMeter_xyTrans column 2 (X / Y / Z, rows 2, 5, 7), dMeter_rTrans column 3 (R, rows 1, 4, 5, 7), each by the layout's moveStatus (tww d_meter.cpp unless named; no WWHD read); 0xA0..0x11F: 8 rows of 0x10 bytes, each 4 x values then 4 y values (tww field_0xa0..field_0x11e) */
    /* 0x120 */ be<u8> mShutterLineAlpha;  /* was field_0x120: WWHD writes it only in the constructor 025974DC; tww reads it: d_picture_box.cpp dJle_Pb_c::draw passes it to shutterLineDraw as the alpha of the 12 shutter lines (tww default 0x8C) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x121 */ be<u8> field_0x121;
    /* 0x122 */ be<u8> field_0x122;
    /* 0x123 */ be<u8> field_0x123;
    /* 0x124 */ be<s16> mShutterFrames;  /* was field_0x124: WWHD writes it only in the constructor 025974DC; tww reads it: d_picture_box.cpp dJle_Pb_c::clickShutterMode / changePicture / shutterChange: the shutter closes over it frames and opens over the next it (tww default 5) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x126 */ be<s16> field_0x126;
    /* 0x128 */ be<s16> field_0x128;
    /* 0x12A */ be<s16> field_0x12a;
    /* 0x12C */ be<s16> field_0x12c;
    /* 0x12E */ be<s16> field_0x12e;
    /* 0x130 */ be<s16> mSwimBlinkPeriodStart;  /* was field_0x130: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimMove / dMeter_swimMainBlink set mSwimBlinkPeriod to it while at least half the time is left and ease it towards mSwimBlinkPeriodEnd below that (tww default 0x14) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x132 */ be<s16> mSwimSoulTurnFrames;  /* was field_0x132: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimMainRotate turns mSwimTimerSoul once around in it frames (tww default 0x27) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x134 */ be<s16> mSwimBlinkPeriodEnd;  /* was field_0x134: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimMainBlink eases mSwimBlinkPeriod towards it as the time runs out (tww default 3) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x136 */ be<s16> mSwimLightFadeFrames;  /* was field_0x136: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimLightAnime fades each mSwimTimerLight in over its first it frames (tww default 4) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x138 */ be<s16> field_0x138;
    /* 0x13A */ be<s16> mSwimTimerOffsetY;  /* was field_0x13a: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimInit sets the swim timer panes' y offset to it + 32 (dMeter_swimTransY; tww default 0) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x13C */ be<s16> mSwimLightStartY;  /* was field_0x13c: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimLightBirth starts a new mSwimTimerLight at mSwimTimerGauge's top + it (tww default 10) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x13E */ be<s16> field_0x13e;
    /* 0x140 */ be<s16> field_0x140;
    /* 0x142 */ be<u8> field_0x142;
    /* 0x143 */ be<u8> field_0x143;
    /* 0x144 */ be<u8> field_0x144;
    /* 0x145 */ be<u8> field_0x145;
    /* 0x146 */ be<u8> field_0x146;
    /* 0x147 */ be<u8> field_0x147;
    /* 0x148 */ be<u8> field_0x148;
    /* 0x149 */ be<u8> field_0x149;
    /* 0x14A */ be<u8> mSwimLowTimeR;  /* was field_0x14a: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimMainBlink: red of the blink colour below half the time (mSwimMainBlinkColor / mSwimFrameBlink*; tww default 0xFF) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x14B */ be<u8> mSwimLowTimeG;  /* was field_0x14b: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimMainBlink: green of the blink colour below half the time (tww default 0x32) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x14C */ be<u8> mSwimLowTimeB;  /* was field_0x14c: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimMainBlink: blue of the blink colour below half the time (tww default 0x32) (tww d_meter.cpp unless named; no WWHD read) */
    u8 mPadding14D[0x1];
    /* 0x14E */ be<s16> mSwimTekariSweepFrames;  /* was field_0x14e: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimTekariScroll: each of the three shine sweeps over mSwimTimerFrame lasts it frames (tww default 10) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x150 */ be<s16> mSwimTekariPause1;  /* was field_0x150: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimTekariScroll: the pause after the first shine sweep (tww default 0x14) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x152 */ be<s16> mSwimTekariPause2;  /* was field_0x152: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimTekariScroll: the pause after the second shine sweep (tww default 4) (tww d_meter.cpp unless named; no WWHD read) */
    /* 0x154 */ be<s16> mSwimTekariPauseEnd;  /* was field_0x154: WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimTekariScroll: the pause after the third sweep before the cycle restarts (tww default 0x32) (tww d_meter.cpp unless named; no WWHD read) */
    u8 mTail[2];
};
WWHD_SIZE(dMeter_HIO_c, 0x158);
WWHD_OFFSET(dMeter_HIO_c, mCompassFadeFrames, 0x4);
WWHD_OFFSET(dMeter_HIO_c, mReserved6, 0x6);
WWHD_OFFSET(dMeter_HIO_c, mReserved8, 0x8);
WWHD_OFFSET(dMeter_HIO_c, mBossLifeRatioDefault, 0xC);
WWHD_OFFSET(dMeter_HIO_c, mHeartScale, 0x10);
WWHD_OFFSET(dMeter_HIO_c, field_0x14, 0x14);
WWHD_OFFSET(dMeter_HIO_c, field_0x18, 0x18);
WWHD_OFFSET(dMeter_HIO_c, field_0x1c, 0x1C);
WWHD_OFFSET(dMeter_HIO_c, field_0x20, 0x20);
WWHD_OFFSET(dMeter_HIO_c, mShutterBladeAngle, 0x24);
WWHD_OFFSET(dMeter_HIO_c, mScopeWipeMaxScale, 0x28);
WWHD_OFFSET(dMeter_HIO_c, mScopeWipeAlpha, 0x2C);
WWHD_OFFSET(dMeter_HIO_c, field_0x2d, 0x2D);
WWHD_OFFSET(dMeter_HIO_c, field_0x2e, 0x2E);
WWHD_OFFSET(dMeter_HIO_c, field_0x30, 0x30);
WWHD_OFFSET(dMeter_HIO_c, field_0x32, 0x32);
WWHD_OFFSET(dMeter_HIO_c, field_0x34, 0x34);
WWHD_OFFSET(dMeter_HIO_c, field_0x36, 0x36);
WWHD_OFFSET(dMeter_HIO_c, field_0x38, 0x38);
WWHD_OFFSET(dMeter_HIO_c, mHeartLightScale, 0x3C);
WWHD_OFFSET(dMeter_HIO_c, mButtonScale, 0x40);
WWHD_OFFSET(dMeter_HIO_c, field_0x42, 0x42);
WWHD_OFFSET(dMeter_HIO_c, mButtonTextScale, 0x44);
WWHD_OFFSET(dMeter_HIO_c, mButtonIconScale, 0x46);
WWHD_OFFSET(dMeter_HIO_c, mBattleHeartScale, 0x48);
WWHD_OFFSET(dMeter_HIO_c, mBattleLifePosX, 0x4C);
WWHD_OFFSET(dMeter_HIO_c, mButtonOffsetX, 0x50);
WWHD_OFFSET(dMeter_HIO_c, mButtonOffsetY, 0x52);
WWHD_OFFSET(dMeter_HIO_c, field_0x54, 0x54);
WWHD_OFFSET(dMeter_HIO_c, field_0x56, 0x56);
WWHD_OFFSET(dMeter_HIO_c, mBattleLifeStepX, 0x58);
WWHD_OFFSET(dMeter_HIO_c, mBattleLifePosY, 0x5C);
WWHD_OFFSET(dMeter_HIO_c, field_0x60, 0x60);
WWHD_OFFSET(dMeter_HIO_c, field_0x61, 0x61);
WWHD_OFFSET(dMeter_HIO_c, mIgnoreArrowNumCount, 0x62);
WWHD_OFFSET(dMeter_HIO_c, mIgnoreBombNumCount, 0x63);
WWHD_OFFSET(dMeter_HIO_c, mMapWindArrowOffsetX, 0x64);
WWHD_OFFSET(dMeter_HIO_c, mMapWindArrowOffsetY, 0x66);
WWHD_OFFSET(dMeter_HIO_c, field_0x68, 0x68);
WWHD_OFFSET(dMeter_HIO_c, field_0x6a, 0x6A);
WWHD_OFFSET(dMeter_HIO_c, mClockCloudPosX, 0x6C);
WWHD_OFFSET(dMeter_HIO_c, mClockCloudPosY, 0x72);
WWHD_OFFSET(dMeter_HIO_c, mClockCloudScale, 0x78);
WWHD_OFFSET(dMeter_HIO_c, mSwimLightSize, 0x7E);
WWHD_OFFSET(dMeter_HIO_c, mReserved80, 0x80);
WWHD_OFFSET(dMeter_HIO_c, mReserved82, 0x82);
WWHD_OFFSET(dMeter_HIO_c, mReserved84, 0x84);
WWHD_OFFSET(dMeter_HIO_c, mReserved86, 0x86);
WWHD_OFFSET(dMeter_HIO_c, mKeyLightStep0, 0x88);
WWHD_OFFSET(dMeter_HIO_c, mKeyLightStep1, 0x8A);
WWHD_OFFSET(dMeter_HIO_c, mKeyLightStep2, 0x8C);
WWHD_OFFSET(dMeter_HIO_c, mKeyLightEndStep, 0x8E);
WWHD_OFFSET(dMeter_HIO_c, mKeyLightStart, 0x90);
WWHD_OFFSET(dMeter_HIO_c, mKeyLightStepRange0, 0x92);
WWHD_OFFSET(dMeter_HIO_c, mKeyLightStepRange1, 0x94);
WWHD_OFFSET(dMeter_HIO_c, mKeyLightStepRange2, 0x96);
WWHD_OFFSET(dMeter_HIO_c, mKeyLightEndRange, 0x98);
WWHD_OFFSET(dMeter_HIO_c, mKeyLightStartRange, 0x9A);
WWHD_OFFSET(dMeter_HIO_c, mMapCrossKeyPosX, 0x9C);
WWHD_OFFSET(dMeter_HIO_c, mMapCrossKeyPosY, 0x9E);
WWHD_OFFSET(dMeter_HIO_c, mButtonPos, 0xA0);
WWHD_OFFSET(dMeter_HIO_c, mShutterLineAlpha, 0x120);
WWHD_OFFSET(dMeter_HIO_c, field_0x121, 0x121);
WWHD_OFFSET(dMeter_HIO_c, field_0x122, 0x122);
WWHD_OFFSET(dMeter_HIO_c, field_0x123, 0x123);
WWHD_OFFSET(dMeter_HIO_c, mShutterFrames, 0x124);
WWHD_OFFSET(dMeter_HIO_c, field_0x126, 0x126);
WWHD_OFFSET(dMeter_HIO_c, field_0x128, 0x128);
WWHD_OFFSET(dMeter_HIO_c, field_0x12a, 0x12A);
WWHD_OFFSET(dMeter_HIO_c, field_0x12c, 0x12C);
WWHD_OFFSET(dMeter_HIO_c, field_0x12e, 0x12E);
WWHD_OFFSET(dMeter_HIO_c, mSwimBlinkPeriodStart, 0x130);
WWHD_OFFSET(dMeter_HIO_c, mSwimSoulTurnFrames, 0x132);
WWHD_OFFSET(dMeter_HIO_c, mSwimBlinkPeriodEnd, 0x134);
WWHD_OFFSET(dMeter_HIO_c, mSwimLightFadeFrames, 0x136);
WWHD_OFFSET(dMeter_HIO_c, field_0x138, 0x138);
WWHD_OFFSET(dMeter_HIO_c, mSwimTimerOffsetY, 0x13A);
WWHD_OFFSET(dMeter_HIO_c, mSwimLightStartY, 0x13C);
WWHD_OFFSET(dMeter_HIO_c, field_0x13e, 0x13E);
WWHD_OFFSET(dMeter_HIO_c, field_0x140, 0x140);
WWHD_OFFSET(dMeter_HIO_c, field_0x142, 0x142);
WWHD_OFFSET(dMeter_HIO_c, field_0x143, 0x143);
WWHD_OFFSET(dMeter_HIO_c, field_0x144, 0x144);
WWHD_OFFSET(dMeter_HIO_c, field_0x145, 0x145);
WWHD_OFFSET(dMeter_HIO_c, field_0x146, 0x146);
WWHD_OFFSET(dMeter_HIO_c, field_0x147, 0x147);
WWHD_OFFSET(dMeter_HIO_c, field_0x148, 0x148);
WWHD_OFFSET(dMeter_HIO_c, field_0x149, 0x149);
WWHD_OFFSET(dMeter_HIO_c, mSwimLowTimeR, 0x14A);
WWHD_OFFSET(dMeter_HIO_c, mSwimLowTimeG, 0x14B);
WWHD_OFFSET(dMeter_HIO_c, mSwimLowTimeB, 0x14C);
WWHD_OFFSET(dMeter_HIO_c, mSwimTekariSweepFrames, 0x14E);
WWHD_OFFSET(dMeter_HIO_c, mSwimTekariPause1, 0x150);
WWHD_OFFSET(dMeter_HIO_c, mSwimTekariPause2, 0x152);
WWHD_OFFSET(dMeter_HIO_c, mSwimTekariPauseEnd, 0x154);
struct dMeter_menuHIO_c {
    /* 0x00 */ be<u32> mVtable;
    /* 0x004 */ be<f32> mItemScale;  /* was field_0x8: WWHD writes it only in the constructor 02597B00; tww reads it: d_menu_collect.cpp dMenu_Collect_c::itemScale scales the collection screen's item panes by it, d_meter.cpp dMeter_moveItemMove sizes the moving item icon by it (no WWHD read) */
    /* 0x008 */ be<f32> field_0xc;
    /* 0x00C */ be<f32> field_0x10;
    /* 0x010 */ be<f32> mStarterScale;  /* was field_0x14: WWHD writes it only in the constructor 02597B00; tww reads it: d_minigame_starter.cpp anime2 scales the minigame start sign by its animation factor * it (no WWHD read) */
    /* 0x014 */ be<f32> field_0x18;
    /* 0x018 */ be<f32> field_0x1c;
    /* 0x01C */ be<f32> field_0x20;
    /* 0x020 */ be<f32> field_0x24;
    /* 0x024 */ be<s16> field_0x28;
    /* 0x026 */ be<s16> field_0x2a;
    /* 0x028 */ be<s16> field_0x2c;
    /* 0x02A */ be<s16> field_0x2e;
    /* 0x02C */ be<s16> field_0x30;
    /* 0x02E */ be<s16> field_0x32;
    /* 0x030 */ be<s16> field_0x34;
    /* 0x032 */ be<s16> field_0x36;
    /* 0x034 */ be<s16> field_0x38;
    /* 0x036 */ be<s16> field_0x3a;
    /* 0x038 */ be<f32> field_0x3c;
    /* 0x03C */ be<f32> field_0x40;
    /* 0x040 */ be<f32> field_0x44;
    /* 0x044 */ be<f32> field_0x48;
    /* 0x048 */ be<f32> field_0x4c;
    /* 0x04C */ be<f32> field_0x50;
    /* 0x050 */ be<f32> field_0x54;
    /* 0x054 */ be<f32> field_0x58;
    /* 0x058 */ be<f32> field_0x5c;
    /* 0x05C */ be<f32> field_0x60;
    /* 0x060 */ be<f32> field_0x64;
    /* 0x064 */ be<f32> field_0x68;
    /* 0x068 */ be<u8> field_0x6c;
    /* 0x069 */ be<u8> field_0x6d;
    /* 0x06A */ be<u8> field_0x6e;
    /* 0x06B */ be<u8> field_0x6f;
    /* 0x06C */ be<u8> field_0x70;
    /* 0x06D */ be<u8> field_0x71;
    /* 0x06E */ be<u8> field_0x72;
    /* 0x06F */ be<u8> field_0x73;
    /* 0x070 */ be<u8> field_0x74;
    /* 0x071 */ be<u8> field_0x75;
    /* 0x072 */ be<u8> field_0x76;
    /* 0x073 */ be<u8> field_0x77;
    /* 0x074 */ be<u8> field_0x78;
    /* 0x075 */ be<u8> field_0x79;
    /* 0x076 */ be<u8> field_0x7a;
    /* 0x077 */ be<u8> field_0x7b;
    /* 0x078 */ be<s16> field_0x7c;
    /* 0x07A */ be<s16> field_0x7e;
    /* 0x07C */ be<s16> field_0x80;
    /* 0x07E */ be<s16> field_0x82;
    /* 0x080 */ be<s16> field_0x84;
    /* 0x082 */ be<s16> mReserved82;
    /* 0x084 */ be<s16> field_0x88;
    /* 0x086 */ be<s16> field_0x8a;
    /* 0x088 */ be<s16> field_0x8c;
    /* 0x08A */ be<s16> field_0x8e;
    /* 0x08C */ be<s16> mMenuOpenDelay;  /* was field_0x90: WWHD writes it only in the constructor 02597B00; tww reads it: d_menu_window.cpp dMs_Execute opens the item / collection sub-menu once its timer passes it (no WWHD read) */
    /* 0x08E */ be<s16> mMenuOpenFrames;  /* was field_0x92: WWHD writes it only in the constructor 02597B00; tww reads it: d_menu_collect.cpp _open2 / _close2 (and the other menu screens) slide and fade the menu in and out over it frames; d_meter.cpp dMeter_menuLRMove uses it * 2 as the L / R arrow cycle (no WWHD read) */
    /* 0x090 */ be<f32> mTimerClockScale;  /* was field_0x94: WWHD writes it only in the constructor 02597B00; tww reads it: d_timer.cpp dTimer_c::anime scales the timer's clock icon and the panes parented to it (clock background, numbers, timer background and shadow) by it (no WWHD read) */
    /* 0x094 */ be<u8> field_0x98;
    /* 0x095 */ be<u8> field_0x99;
    /* 0x096 */ be<s16> mTimerRupeePosX;  /* was field_0x9a: WWHD writes it only in the constructor 02597B00; tww reads it: d_timer.cpp dTimer_c::anime / closeAnime place the timer's rupee pane at its x + it (no WWHD read) */
    /* 0x098 */ be<s16> mTimerRupeePosY;  /* was field_0x9c: WWHD writes it only in the constructor 02597B00; tww reads it: d_timer.cpp dTimer_c::anime / closeAnime place the timer's rupee pane at y it (no WWHD read) */
    /* 0x09A */ be<s16> field_0x9e;
    /* 0x09C */ be<f32> mTimerRupeeScale;  /* was field_0xa0: WWHD writes it only in the constructor 02597B00; tww reads it: d_timer.cpp dTimer_c::anime scales the timer's rupee pane and its shadow by it (no WWHD read) */
    /* 0x0A0 */ be<s16> field_0xa4;
    /* 0x0A2 */ be<s16> field_0xa6;
    /* 0x0A4 */ be<s16> field_0xa8;
    /* 0x0A6 */ be<s16> field_0xaa;
    /* 0x0A8 */ be<s16> field_0xac;
    /* 0x0AA */ be<s16> field_0xae;
    /* 0x0AC */ be<s16> field_0xb0;
    /* 0x0AE */ be<s16> field_0xb2;
    /* 0x0B0 */ be<s16> field_0xb4;
    /* 0x0B2 */ be<s16> mGameOverDelay;  /* was field_0xb6: WWHD writes it only in the constructor 02597B00; tww reads it: d_gameover.cpp dGameover_c::_create copies it into the countdown that _execute runs down before going on (no WWHD read) */
    /* 0x0B4 */ be<u8> field_0xb8;
    u8 mPaddingB5[0x1];
    /* 0x0B6 */ be<s16> field_0xba;
    /* 0x0B8 */ be<s16> field_0xbc;
    /* 0x0BA */ be<u8> field_0xbe;
    /* 0x0BB */ be<u8> field_0xbf;
    /* 0x0BC */ be<u8> field_0xc0;
    /* 0x0BD */ be<u8> field_0xc1;
    /* 0x0BE */ be<u8> field_0xc2;
    /* 0x0BF */ be<u8> field_0xc3;
    /* 0x0C0 */ be<u8> field_0xc4;
    /* 0x0C1 */ be<u8> field_0xc5;
    /* 0x0C2 */ be<u8> field_0xc6;
    /* 0x0C3 */ be<u8> field_0xc7;
    /* 0x0C4 */ be<u8> mItemMoveFrames;  /* was field_0xc8: WWHD writes it only in the constructor 02597B00; tww reads it: d_meter.cpp dMeter_moveItemMove / dMeter_moveItemDraw: the item-get icon move lasts it frames (the move flag drops once dMeter_itemTimer reaches it) (no WWHD read) */
    /* 0x0C5 */ be<u8> mItemMoveScaleMode;  /* was field_0xc9: WWHD writes it only in the constructor 02597B00; tww reads it: d_meter.cpp dMeter_moveItemMove switches on it to choose how the moving item icon is scaled (1: mItemScale, 2: shrinking from it) (no WWHD read) */
    /* 0x0C6 */ be<u8> field_0xca;
    u8 mTail[1];
};
WWHD_SIZE(dMeter_menuHIO_c, 0xC8);
WWHD_OFFSET(dMeter_menuHIO_c, mItemScale, 0x4);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc, 0x8);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x10, 0xC);
WWHD_OFFSET(dMeter_menuHIO_c, mStarterScale, 0x10);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x18, 0x14);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x1c, 0x18);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x20, 0x1C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x24, 0x20);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x28, 0x24);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x2a, 0x26);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x2c, 0x28);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x2e, 0x2A);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x30, 0x2C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x32, 0x2E);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x34, 0x30);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x36, 0x32);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x38, 0x34);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x3a, 0x36);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x3c, 0x38);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x40, 0x3C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x44, 0x40);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x48, 0x44);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x4c, 0x48);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x50, 0x4C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x54, 0x50);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x58, 0x54);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x5c, 0x58);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x60, 0x5C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x64, 0x60);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x68, 0x64);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x6c, 0x68);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x6d, 0x69);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x6e, 0x6A);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x6f, 0x6B);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x70, 0x6C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x71, 0x6D);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x72, 0x6E);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x73, 0x6F);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x74, 0x70);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x75, 0x71);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x76, 0x72);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x77, 0x73);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x78, 0x74);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x79, 0x75);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x7a, 0x76);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x7b, 0x77);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x7c, 0x78);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x7e, 0x7A);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x80, 0x7C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x82, 0x7E);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x84, 0x80);
WWHD_OFFSET(dMeter_menuHIO_c, mReserved82, 0x82);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x88, 0x84);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x8a, 0x86);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x8c, 0x88);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x8e, 0x8A);
WWHD_OFFSET(dMeter_menuHIO_c, mMenuOpenDelay, 0x8C);
WWHD_OFFSET(dMeter_menuHIO_c, mMenuOpenFrames, 0x8E);
WWHD_OFFSET(dMeter_menuHIO_c, mTimerClockScale, 0x90);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x98, 0x94);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x99, 0x95);
WWHD_OFFSET(dMeter_menuHIO_c, mTimerRupeePosX, 0x96);
WWHD_OFFSET(dMeter_menuHIO_c, mTimerRupeePosY, 0x98);
WWHD_OFFSET(dMeter_menuHIO_c, field_0x9e, 0x9A);
WWHD_OFFSET(dMeter_menuHIO_c, mTimerRupeeScale, 0x9C);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xa4, 0xA0);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xa6, 0xA2);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xa8, 0xA4);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xaa, 0xA6);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xac, 0xA8);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xae, 0xAA);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xb0, 0xAC);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xb2, 0xAE);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xb4, 0xB0);
WWHD_OFFSET(dMeter_menuHIO_c, mGameOverDelay, 0xB2);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xb8, 0xB4);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xba, 0xB6);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xbc, 0xB8);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xbe, 0xBA);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xbf, 0xBB);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc0, 0xBC);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc1, 0xBD);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc2, 0xBE);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc3, 0xBF);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc4, 0xC0);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc5, 0xC1);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc6, 0xC2);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xc7, 0xC3);
WWHD_OFFSET(dMeter_menuHIO_c, mItemMoveFrames, 0xC4);
WWHD_OFFSET(dMeter_menuHIO_c, mItemMoveScaleMode, 0xC5);
WWHD_OFFSET(dMeter_menuHIO_c, field_0xca, 0xC6);
struct dMeter_msg_HIO_c {
    /* 0x00 */ be<u32> mVtable;
    /* 0x004 */ GXColor mTalkWindowWhite;  /* was field_0x5: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_screenDataSetTalk / dMsg_Execute give the talk window pane m049C this white colour (no WWHD read) */
    /* 0x008 */ GXColor mTalkWindowBlack;  /* was field_0x9: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_screenDataSetTalk / dMsg_Execute give the talk window pane m049C this black colour (no WWHD read) */
    /* 0x00C */ be<u8> field_0xd;
    /* 0x00D */ be<u8> field_0xe;
    /* 0x00E */ be<u8> field_0xf;
    /* 0x00F */ be<u8> field_0x10;
    /* 0x010 */ be<u8> field_0x11;
    /* 0x011 */ be<u8> field_0x12;
    /* 0x012 */ be<u8> field_0x13;
    /* 0x013 */ be<u8> field_0x14;
    /* 0x014 */ be<u8> field_0x15;
    /* 0x015 */ be<u8> field_0x16;
    /* 0x016 */ be<u8> field_0x17;
    /* 0x017 */ be<u8> field_0x18;
    /* 0x018 */ be<u8> field_0x19;
    /* 0x019 */ be<u8> field_0x1a;
    /* 0x01A */ be<u8> field_0x1b;
    /* 0x01B */ be<u8> field_0x1c;
    /* 0x01C */ be<u8> field_0x1d;
    /* 0x01D */ be<u8> field_0x1e;
    /* 0x01E */ be<u8> field_0x1f;
    /* 0x01F */ be<u8> field_0x20;
    /* 0x020 */ be<u8> field_0x21;
    /* 0x021 */ be<u8> field_0x22;
    /* 0x022 */ be<u8> field_0x23;
    /* 0x023 */ be<u8> field_0x24;
    /* 0x024 */ be<u8> field_0x25;
    /* 0x025 */ be<u8> field_0x26;
    /* 0x026 */ be<u8> field_0x27;
    /* 0x027 */ be<u8> field_0x28;
    /* 0x028 */ be<u8> field_0x29;
    /* 0x029 */ be<u8> field_0x2a;
    /* 0x02A */ be<u8> field_0x2b;
    /* 0x02B */ be<u8> mMsgArrowAlphaA;  /* was field_0x2c: WWHD writes it only in the constructor 02597E34; tww reads it: f_op_msg_mng.cpp fopMsgM_arrowAnime blends the message arrow's alpha between it and mMsgArrowAlphaB and back (no WWHD read) */
    /* 0x02C */ be<u8> field_0x2d;
    /* 0x02D */ be<u8> field_0x2e;
    /* 0x02E */ be<u8> field_0x2f;
    /* 0x02F */ be<u8> mMsgArrowAlphaB;  /* was field_0x30: WWHD writes it only in the constructor 02597E34; tww reads it: f_op_msg_mng.cpp fopMsgM_arrowAnime blends the message arrow's alpha between mMsgArrowAlphaA and it and back (no WWHD read) */
    u8 mPadding30[0x4];
    /* 0x034 */ be<f32> field_0x38;
    /* 0x038 */ be<f32> field_0x3c;
    /* 0x03C */ be<f32> mSelectCloseScale;  /* was field_0x40: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextClose scales the choice box by 1 + (it - 1) * the close progress (no WWHD read) */
    /* 0x040 */ be<s16> mSelectOpenDelay;  /* was field_0x44: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextOpen / dMsg_selectProc: the choice box starts opening after it frames (no WWHD read) */
    /* 0x042 */ be<s16> mSelectOpenFrames;  /* was field_0x46: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextOpen opens the choice box over it frames after mSelectOpenDelay (no WWHD read) */
    /* 0x044 */ be<s16> mSelectCloseFrames;  /* was field_0x48: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextClose closes the choice box over it frames (no WWHD read) */
    /* 0x046 */ be<s16> field_0x4a;
    /* 0x048 */ be<s16> field_0x4c;
    u8 mPadding4A[0x2];
    /* 0x04C */ be<f32> field_0x50;
    /* 0x050 */ be<f32> field_0x54;
    /* 0x054 */ be<u8> mDemoFontSize;  /* was field_0x58: WWHD writes it only in the constructor 02597E34; tww reads it: the font size of the demo message (d_msg.cpp dMsg_screenDataSetDemo), the picture box (d_picture_box.cpp screenSet2) and the telescope text (d_scope.cpp dScp_ScreenDataSet) (no WWHD read) */
    u8 mPadding55[0x1];
    /* 0x056 */ be<s16> mCharSpace;  /* was field_0x5a: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_screenDataSetTalk / d_mesg.cpp setCommonData: the character spacing of the text panes ([0] and its shadow [2]) (no WWHD read) */
    /* 0x058 */ be<s16> mRubyCharSpace;  /* was field_0x5c: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_screenDataSetTalk / d_mesg.cpp setCommonData and ruby_character: the character spacing of the ruby panes ([1] and its shadow [3]) (no WWHD read) */
    /* 0x05A */ be<s16> mLineSpace;  /* was field_0x5e: WWHD writes it only in the constructor 02597E34; tww reads it: the line spacing of the message text (d_mesg.cpp initialize / do_tag / do_systemTagCode, the menus' itemnoteSet) (no WWHD read) */
    /* 0x05C */ be<s16> mRubyOffsetY;  /* was field_0x60: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_ScreenDataValueInitItem / InitTact / dMsg_mesgOutPos / dMsg_Execute place the ruby panes ([1], [3]) it above the text line (no WWHD read) */
    /* 0x05E */ be<s16> mTextOffsetY;  /* was field_0x62: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_ScreenDataValueInitItem / InitTact / dMsg_mesgOutPos / dMsg_Execute place the text panes ([0], [2]) it above the line position (no WWHD read) */
    /* 0x060 */ be<u8> field_0x64;
    u8 mPadding61[0x1];
    /* 0x062 */ be<s16> mTalkWindowOffsetY;  /* was field_0x66: WWHD writes it only in the constructor 02597E34; tww reads it: added to the y of the talk window pane, its text panes and the message arrow (d_msg.cpp dMsg_Execute / dMsg_arrowMove, d_mesg.cpp arwAnime) (no WWHD read) */
    /* 0x064 */ be<s16> mRubyFontSize;  /* was field_0x68: WWHD writes it only in the constructor 02597E34; tww reads it: the ruby font size (d_msg.cpp dMsg_screenDataSetItem / Demo, d_message.cpp dMsg2_screenDataInit, d_mesg.cpp ruby_character) (no WWHD read) */
    /* 0x066 */ be<s16> mOutFontOffsetY;  /* was field_0x6a: WWHD writes it only in the constructor 02597E34; tww reads it: f_op_msg_mng.cpp fopMsgM_outFontDraw draws the inline button icon and its shadow it lower (no WWHD read) */
    /* 0x068 */ be<s16> mSpaceTimer;  /* was field_0x6c: WWHD writes it only in the constructor 02597E34; tww reads it: the message space timer (setSpaceTimer in d_picture_box.cpp messageSet, d_scope.cpp dScp_talkBeforeProc; f_op_msg_mng.cpp stringSet) (no WWHD read) */
    /* 0x06A */ be<s16> mTalkWindowSplitY;  /* was field_0x6e: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_mesgOutPos puts the talk window at the top or the bottom by whether the speaker's screen y is below it (no WWHD read) */
    /* 0x06C */ be<s16> mFontSize;  /* was field_0x70: WWHD writes it only in the constructor 02597E34; tww reads it: the message font size (f_op_msg_mng.cpp, d_msg.cpp, d_mesg.cpp, the menu screens' itemnoteSet, d_picture_box.cpp, d_scope.cpp) (no WWHD read) */
    /* 0x06E */ be<u8> field_0x72;
    u8 mPadding6F[0x1];
    /* 0x070 */ be<s16> field_0x74;
    /* 0x072 */ be<s16> field_0x76;
    /* 0x074 */ be<s16> mSelectWidthAdd;  /* was field_0x78: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextSizeSet widens the choice box by it plus the choice text length (no WWHD read) */
    /* 0x076 */ be<s16> mSelectHeightAdd;  /* was field_0x7a: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextSizeSet adds it to the choice box height (no WWHD read) */
    /* 0x078 */ be<s16> mSelectOffsetY;  /* was field_0x7c: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextSizeSet places the choice box it from the talk window's edge (no WWHD read) */
    /* 0x07A */ be<u8> field_0x7e;
    /* 0x07B */ be<u8> mCornerColorFrames;  /* was field_0x7f: WWHD writes it only in the constructor 02597E34; tww reads it: d_mesg.cpp cornerMove turns the corner colours of the message cursor one step every it frames (a 4-step cycle) (no WWHD read) */
    /* 0x07C */ be<u8> mSelectWindowAlpha;  /* was field_0x80: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextOpen / dMsg_subTextClose fade the talk window pane m049C to it / 255 while the choice box is open (no WWHD read) */
    /* 0x07D */ be<u8> field_0x81;
    /* 0x07E */ be<u8> mSendSpeed;  /* was field_0x82: WWHD writes it only in the constructor 02597E34; tww reads it: the text speed: setSendSpeed in d_msg.cpp dMsg_ScreenDataValueInit* / dMsg_continueProc and d_picture_box.cpp messageSet (no WWHD read) */
    /* 0x07F */ be<u8> mSpaceFlag;  /* was field_0x83: WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_ScreenDataValueInit* / dMsg_continueProc and d_picture_box.cpp messageSet switch the message space flag on while it is not 0 (no WWHD read) */
    /* 0x080 */ be<u8> field_0x84;
    /* 0x081 */ be<u8> field_0x85;
    /* 0x082 */ be<u8> field_0x86;
    /* 0x083 */ be<u8> mStickBlendFrames;  /* was field_0x87: WWHD writes it only in the constructor 02597E34; tww reads it: f_op_msg_mng.cpp / d_2dnumber.cpp outFontStickAnime*: the frames of each blend between the two stick icon frames (no WWHD read) */
    /* 0x084 */ be<u8> mStickStartDelay;  /* was field_0x88: WWHD writes it only in the constructor 02597E34; tww reads it: f_op_msg_mng.cpp / d_2dnumber.cpp outFontStickAnime*: the frames before the stick icon animation starts (no WWHD read) */
    /* 0x085 */ be<u8> mStickHoldFrames;  /* was field_0x89: WWHD writes it only in the constructor 02597E34; tww reads it: f_op_msg_mng.cpp / d_2dnumber.cpp outFontStickAnime*: the frames the second stick icon frame is held (no WWHD read) */
    /* 0x086 */ be<u8> field_0x8a;
    u8 mTail[1];
};
WWHD_SIZE(dMeter_msg_HIO_c, 0x88);
WWHD_OFFSET(dMeter_msg_HIO_c, mTalkWindowWhite, 0x4);
WWHD_OFFSET(dMeter_msg_HIO_c, mTalkWindowBlack, 0x8);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0xd, 0xC);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0xe, 0xD);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0xf, 0xE);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x10, 0xF);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x11, 0x10);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x12, 0x11);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x13, 0x12);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x14, 0x13);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x15, 0x14);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x16, 0x15);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x17, 0x16);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x18, 0x17);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x19, 0x18);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x1a, 0x19);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x1b, 0x1A);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x1c, 0x1B);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x1d, 0x1C);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x1e, 0x1D);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x1f, 0x1E);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x20, 0x1F);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x21, 0x20);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x22, 0x21);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x23, 0x22);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x24, 0x23);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x25, 0x24);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x26, 0x25);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x27, 0x26);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x28, 0x27);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x29, 0x28);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x2a, 0x29);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x2b, 0x2A);
WWHD_OFFSET(dMeter_msg_HIO_c, mMsgArrowAlphaA, 0x2B);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x2d, 0x2C);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x2e, 0x2D);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x2f, 0x2E);
WWHD_OFFSET(dMeter_msg_HIO_c, mMsgArrowAlphaB, 0x2F);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x38, 0x34);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x3c, 0x38);
WWHD_OFFSET(dMeter_msg_HIO_c, mSelectCloseScale, 0x3C);
WWHD_OFFSET(dMeter_msg_HIO_c, mSelectOpenDelay, 0x40);
WWHD_OFFSET(dMeter_msg_HIO_c, mSelectOpenFrames, 0x42);
WWHD_OFFSET(dMeter_msg_HIO_c, mSelectCloseFrames, 0x44);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x4a, 0x46);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x4c, 0x48);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x50, 0x4C);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x54, 0x50);
WWHD_OFFSET(dMeter_msg_HIO_c, mDemoFontSize, 0x54);
WWHD_OFFSET(dMeter_msg_HIO_c, mCharSpace, 0x56);
WWHD_OFFSET(dMeter_msg_HIO_c, mRubyCharSpace, 0x58);
WWHD_OFFSET(dMeter_msg_HIO_c, mLineSpace, 0x5A);
WWHD_OFFSET(dMeter_msg_HIO_c, mRubyOffsetY, 0x5C);
WWHD_OFFSET(dMeter_msg_HIO_c, mTextOffsetY, 0x5E);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x64, 0x60);
WWHD_OFFSET(dMeter_msg_HIO_c, mTalkWindowOffsetY, 0x62);
WWHD_OFFSET(dMeter_msg_HIO_c, mRubyFontSize, 0x64);
WWHD_OFFSET(dMeter_msg_HIO_c, mOutFontOffsetY, 0x66);
WWHD_OFFSET(dMeter_msg_HIO_c, mSpaceTimer, 0x68);
WWHD_OFFSET(dMeter_msg_HIO_c, mTalkWindowSplitY, 0x6A);
WWHD_OFFSET(dMeter_msg_HIO_c, mFontSize, 0x6C);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x72, 0x6E);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x74, 0x70);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x76, 0x72);
WWHD_OFFSET(dMeter_msg_HIO_c, mSelectWidthAdd, 0x74);
WWHD_OFFSET(dMeter_msg_HIO_c, mSelectHeightAdd, 0x76);
WWHD_OFFSET(dMeter_msg_HIO_c, mSelectOffsetY, 0x78);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x7e, 0x7A);
WWHD_OFFSET(dMeter_msg_HIO_c, mCornerColorFrames, 0x7B);
WWHD_OFFSET(dMeter_msg_HIO_c, mSelectWindowAlpha, 0x7C);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x81, 0x7D);
WWHD_OFFSET(dMeter_msg_HIO_c, mSendSpeed, 0x7E);
WWHD_OFFSET(dMeter_msg_HIO_c, mSpaceFlag, 0x7F);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x84, 0x80);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x85, 0x81);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x86, 0x82);
WWHD_OFFSET(dMeter_msg_HIO_c, mStickBlendFrames, 0x83);
WWHD_OFFSET(dMeter_msg_HIO_c, mStickStartDelay, 0x84);
WWHD_OFFSET(dMeter_msg_HIO_c, mStickHoldFrames, 0x85);
WWHD_OFFSET(dMeter_msg_HIO_c, field_0x8a, 0x86);

struct mapCtrlDisp_c {
    be<u8> field_0x0, field_0x1, field_0x2;
    u8 mPadding;
    be<s16> field_0x4;
    void initMapCtrlDisp();
};
WWHD_SIZE(mapCtrlDisp_c, 6);
WWHD_OFFSET(mapCtrlDisp_c, field_0x4, 4);
struct MeterHIOFloats {
    be<f32> mValues[24];
};
extern MeterHIOFloats& l_hioValues;
extern be<f32>& l_zero;
extern be<f32>& l_one;
extern be<f32>& l_half;
extern be<f32>& l_mapRange;
inline u32 meter01_allocate(u32 size) {
    return gabi::call<u32>(0x0273AD10, size);
}
inline void meter01_clear(void* address, u32 size) {
    gabi::call(0x028F521C, address, size);
}

struct dMeter_map_HIO_c {
    be<s8> mNo;
    be<u8> field_0x5;
    be<u8> mIconFreeAlpha;  /* was field_0x6: WWHD writes it only in the constructor 02598254 (200); tww reads it: mapCtrlDisp_c::moveMapCtrlDisp sets the free icon's alpha to the fade value * it / 255 (dMap_c::setIconFreeAlpha) (no WWHD read) */
    be<u8> field_0x7;
    be<s16> mMapPosX;  /* was field_0x8: the map's left x when shown: mapCtrlDisp_c::initMapCtrlDisp 0259359C copies it into l_mapState mPosLeftUpX; dMeter_Create 02595C6C starts the slide offset at field_0xc - it; constructor 02598254 default 0x23 (tww the same; TWW mapCtrlDisp_c::moveMapCtrlDisp sets the map x to it + the slide offset) */
    be<s16> mMapPosY;  /* was field_0xa: the map's top y: mapCtrlDisp_c::initMapCtrlDisp 0259359C copies it into l_mapState mPosLeftUpY; constructor 02598254 default 0x152 (tww the same; TWW moveMapCtrlDisp sets the map y to it, raised when the scissor ends above 452) */
    be<s16> mMapHiddenPosX;  /* was field_0xc: the map's left x when hidden: dMeter_Create 02595C6C sets mapCtrlDisp field_0x4 (the slide offset) to it - mMapPosX, so the map starts hidden; constructor 02598254 default -0xB4 (tww the same; TWW moveMapCtrlDisp eases the offset to it - mMapPosX while the map is off and to 0 while it is on) */
    u8 mPaddingA[2];
    be<f32> mIconFreeScale;  /* was field_0x10: WWHD writes it only in the constructor 02598254 (l_mapIconScale); tww reads it: mapCtrlDisp_c::moveMapCtrlDisp sets the free icon's scale to 1.0 / 1.1 * it (dMap_c::setIconFreeScale; tww default 0.8) (no WWHD read) */
    be<s16> mIconFreePosX;  /* was field_0x14: mapCtrlDisp_c::initMapCtrlDisp 0259359C copies it into l_mapState mIconFreePosX; constructor 02598254 default 0x24E (tww initMapCtrlDisp dMap_setIconFreePosX the same) */
    be<s16> mIconFreePosY;  /* was field_0x16: mapCtrlDisp_c::initMapCtrlDisp 0259359C copies it into l_mapState mIconFreePosY; constructor 02598254 default 0x96 (tww initMapCtrlDisp dMap_setIconFreePosY the same) */
    u8 mPadding14[4];
    be<f32> mHDScale;
    be<u32> mVtable;
};
WWHD_SIZE(dMeter_map_HIO_c, 0x20);
WWHD_OFFSET(dMeter_map_HIO_c, mMapPosX, 4);
WWHD_OFFSET(dMeter_map_HIO_c, mIconFreePosX, 0x10);
struct MeterMapState {
    be<s16> mIconFreePosX;
    be<s16> mIconFreePosY;
    u8 mPadding4[2];
    be<s16> mPosLeftUpX;
    be<s16> mPosLeftUpY;
    u8 mPaddingA[6];
    be<u8> mSelfAlpha;
    u8 mPadding11[7];
    be<u8> mDisplayMode, mAlpha, mIconFreeAlpha, mIconDisplayMode;
};
WWHD_SIZE(MeterMapState, 0x1C);
WWHD_OFFSET(MeterMapState, mDisplayMode, 0x18);
extern dMeter_map_HIO_c& g_meter_mapHIO;
extern MeterMapState& l_mapState;
extern be<f32>& l_iconFreeScale;
extern be<f32>& l_iconSelfScale;
inline void meter01_setAlpha(fopMsgM_pane_class* pane) {
    gabi::call(0x025DB678, pane);
}
inline s32 meter01_isOverlapRequest() {
    return gabi::call<s32>(0x025DBE38);
}
inline void meter01_setDrawList(u32 object, u32 offset) {
    u32 list = gabi::call<u32>(0x025200D4) + 0x5D30;
    gabi::call(0x0252CDC0, list, list + offset, list + offset + 4, object);
}
struct MeterPrefixHD {
    u8 mMessage[0x100];
    be<u32> heap;
    fopMsgM_pane_class mMapCrossArrow[3];  /* was field_0x0100: panes 'yrig' / 'yup' / 'ylef' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C) (moved 2 / 2 / 1 units there), drawn by dMeter_Draw 02593630: the right, up and left arrows of the map cross-key guide; dMeter_menuInit and dMeter_Create store dComIfGs_isDungeonItemCompass in [0].mUserArea; TWW dMeter_menuPlusMove re-checks that flag, shows [0..2] by the map / compass state, places them around the map or the compass and fades them with mMapCrossKey (tww writes it, the same code) */
    fopMsgM_pane_class field_0x01a8;
    fopMsgM_pane_class field_0x01e0;
    fopMsgM_pane_class field_0x0218;
    fopMsgM_pane_class mMenuArrowR;  /* was field_0x0250: pane 'r' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_menuInit (in dMeter_Create) sets its mUserArea to 10; TWW dMeter_menuLRMove slides it right by up to 20 and fades it while the menu button is pushed, its mUserArea being that slide frame (tww writes it, the same code) */
    fopMsgM_pane_class mMenuArrowL;  /* was field_0x0288: pane 'l' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_menuInit (in dMeter_Create) clears its mUserArea; TWW dMeter_menuLRMove slides it left by up to 20 with mMenuArrowR; its mUserArea keeps both fully visible after a menu button change (tww writes it, the same code) */
    fopMsgM_pane_class mMapCrossKey;  /* was field_0x02c0: pane 'crs1' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_menuInit (in dMeter_Create) clears its mUserArea; TWW dMeter_menuPlusMove places it at HIO 0x9C / 0x9E, shows it by the map state and fades it (and the map) by its mUserArea, a 0..5 frame counter (tww writes it, the same code) */
    fopMsgM_pane_class mMapWindArrow;  /* was field_0x02f8: pane 'wiy1' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; TWW dMeter_menuPlusMove turns it to the wind direction (dKyw_get_wind_vec), places it at the map corner + HIO 0x64 / 0x66 and hides it unless the wind is in use (dKyw_gbwind_use_check) (tww writes it, the same code) */
    fopMsgM_pane_class field_0x0330;
    fopMsgM_pane_class mBossIconEye1;  /* was field_0x0368: pane 'bey1' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_enemyInit (in dMeter_Create) clears its mUserArea; TWW dMeter_bossEnemyMove / Hide fade it with the boss gauge, dMeter_bossAnime moves it with mBossIconTop and dMeter_bossEyeAnime blinks it, its mUserArea counting up to mBossIconEye2's (tww writes it, the same code) */
    fopMsgM_pane_class mBossIconEye2;  /* was field_0x03a0: pane 'bey2' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_enemyInit (in dMeter_Create) sets its mUserArea to (40 + rnd 18) * 2, the blink period; TWW dMeter_bossEyeAnime blinks it with mBossIconEye1 and rolls a new period after each blink (tww writes it, the same code) */
    fopMsgM_pane_class mBossIconTop;  /* was field_0x03d8: pane 'enem' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_enemyInit (in dMeter_Create) clears its mUserArea; TWW dMeter_bossAnime moves it up (and mBossIconBottom down) by a curve over its mUserArea frame, which restarts at mBossIconBottom's period; dMeter_bossEnemyMove / Hide fade it (tww writes it, the same code) */
    fopMsgM_pane_class mBossIconBottom;  /* was field_0x0410: pane 'enm2' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_enemyInit (in dMeter_Create) sets its mUserArea to 300 + rnd 200, the period of dMeter_bossAnime, which TWW re-rolls each cycle; TWW dMeter_bossAnime moves it down opposite mBossIconTop, dMeter_bossEnemyMove / Hide fade it (tww writes it, the same code) */
    fopMsgM_pane_class mBossLifeBar[4];  /* was field_0x0448: panes 'ben1'..'ben4' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_enemyInit (in dMeter_Create) clears [1].mUserArea; TWW dMeter_bossEnemyMove opens the boss life gauge over [0].mUserArea (0..30 frames), fades it in over [1].mUserArea (0..10, faded out by dMeter_bossEnemyHide) and scales [2] / [3] in x by the boss's health / max_health, right-aligned (tww writes it, the same code) */
    fopMsgM_pane_class mEnemyLifePart3;  /* was field_0x0528: pane 'ene3' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; TWW dMeter_zakoEnemyMove keeps it at its layout offset from mEnemyLifeBar beside the lock-on target and fades it with it (dMeter_zakoEnemyHide) (tww writes it, the same code) */
    fopMsgM_pane_class mEnemyLifeBar;  /* was field_0x0560: pane 'ene2' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_enemyInit (in dMeter_Create) clears its mUserArea; TWW dMeter_zakoEnemyMove places it 50 beside the lock-on target's projected attention position, sizes its width by the target's health / max_health and fades it in over its mUserArea (0..10, out in dMeter_zakoEnemyHide) (tww writes it, the same code) */
    fopMsgM_pane_class mEnemyLifePart1;  /* was field_0x0598: pane 'ene1' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; TWW dMeter_zakoEnemyMove keeps it at its layout offset from mEnemyLifeBar and fades it with it (dMeter_zakoEnemyHide) (tww writes it, the same code) */
    fopMsgM_pane_class mEnemyLifeShadow2;  /* was field_0x05d0: pane 'enk2' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C) (the 'k' tags are the shadow panes); not in dMeter_Draw's list; TWW dMeter_zakoEnemyMove only resets it to its layout position and size (tww writes it, the same code) */
    fopMsgM_pane_class mEnemyLifeShadow;  /* was field_0x0608: pane 'enek' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630 (the 'k' tags are the shadow panes); TWW dMeter_zakoEnemyMove keeps it at its layout offset from mEnemyLifeBar and fades it with it (dMeter_zakoEnemyHide) (tww writes it, the same code) */
};
WWHD_SIZE(MeterPrefixHD, 0x644);
WWHD_OFFSET(MeterPrefixHD, heap, 0x100);

struct MeterHeapVtable {
    u8 mPadding0[0x3C];
    be<u32> mFree;
    u8 mPadding40[4];
    be<u32> mRemoveResourceAll;
};
struct MeterHeap {
    u8 mPadding0[0xC];
    gptr<MeterHeapVtable> mVtable;
};
WWHD_OFFSET(MeterHeap, mVtable, 0xC);
WWHD_OFFSET(MeterHeapVtable, mFree, 0x3C);
WWHD_OFFSET(MeterHeapVtable, mRemoveResourceAll, 0x44);
struct MeterScreenVtable {
    u8 mPadding0[0xC];
    be<u32> mDestructor;
};
struct MeterScreen {
    u8 mPadding0[0xC8];
    gptr<MeterScreenVtable> mVtable;
};
WWHD_OFFSET(MeterScreen, mVtable, 0xC8);
WWHD_OFFSET(MeterScreenVtable, mDestructor, 0xC);
struct MeterHDState {
    be<f32> mPaneScales[8];
    be<u32> mStatusFlags;
    be<s16> mRoomNumber;
    be<u8> mStateBytes[0x26];
};
WWHD_OFFSET(MeterHDState, mStatusFlags, 0x20);
WWHD_OFFSET(MeterHDState, mStateBytes, 0x26);
WWHD_SIZE(MeterHDState, 0x4C);
extern MeterHDState& l_hdState;
extern be<u32>& sMainParts1;
extern be<u32>& sMainParts2;
extern be<u32>& sMainParts3;
extern be<u32>& sScrTimer1;
extern be<u32>& sScrTimer2;
extern be<u32>& sChoiceRoad;
extern be<u32>& moveItemPane;
extern be<u32>& dMeter_windID;
extern be<u8>& rupy_soundOnFlag;
extern be<u8>& rupy_soundSetFlag;
extern be<u8>& dMeter_windStatus;
inline u32 meter01_setCurrentHeap(u32 heap) {
    return gabi::call<u32>(0x025E3570, heap);
}
inline void meter01_soundStart(u32 sound) {
    gabi::call(0x025E1988, sound);
}
inline u32 meter01_searchMessage(u32 id) {
    return gabi::call<u32>(0x025DE50C, id);
}
inline void meter01_deleteMessage(u32 message) {
    gabi::call(0x025DF944, message);
}
inline void meter01_destroyExpHeap(u32 heap) {
    gabi::call(0x025DB8F4, heap);
}
inline void meter01_deleteHIOChild(s32 child) {
    gabi::call(0x025F0A18, child);
}
inline void meter01_freeTexture(sub_meter_class* meter, be<u32>& texture) {
    u32 heap = reinterpret_cast<MeterPrefixHD*>(meter)->heap;
    u32 target = gabi::at<MeterHeap>(heap)->mVtable->mFree;
    u32 resource = texture;
    gabi::call_ptr(target, heap, resource);
}
inline void meter01_destroyScreen(u32 screen) {
    if (screen) {
        u32 target = gabi::at<MeterScreen>(screen)->mVtable->mDestructor;
        gabi::call_ptr(target, screen, 3);
    }
}
inline void meter01_removeSwimResources() {
    u32 heap = gabi::load<u32>(gabi::call<u32>(0x025200D4) + 0x5AA0);
    u32 target = gabi::at<MeterHeap>(heap)->mVtable->mRemoveResourceAll;
    gabi::call_ptr(target, heap);
}
inline void meter01_clearHudState() {
    // HD: reset the expanded HUD state after releasing resources.
    l_hdState.mStatusFlags = 0;
    for (u32 offset : {0x62u, 0x63u, 0x3Eu, 0x3Fu, 0x40u, 0x41u, 0x42u, 0x43u, 0x44u, 0x5Au, 0x5Bu, 0x5Cu,
                       0x5Du, 0x5Eu, 0x5Fu})
        l_hdState.mStateBytes[offset - 0x3E] = 0;
    for (int i = 0; i < 8; i++)
        l_hdState.mPaneScales[i] = l_zero;
    l_hdState.mRoomNumber = 0;
    l_hdState.mStateBytes[0x60 - 0x3E] = 0;
    l_hdState.mStateBytes[0x61 - 0x3E] = 0;
    for (int i = 0; i < 7; i++) {
        l_hdState.mStateBytes[0x53 - 0x3E + i] = 0;
        l_hdState.mStateBytes[0x45 - 0x3E + i] = 0;
        l_hdState.mStateBytes[0x4C - 0x3E + i] = 0;
    }
}

inline u8 meter01_mapSetting() {
    return gabi::load<u8>(0x1047AD0C);
}
inline void meter01_restoreMapSetting(u8 setting) {
    // HD: this byte is separate from the shared play accessors.
    u32 game = gabi::call<u32>(0x025200D4);
    gabi::store<u8>(game + 0x5C23, setting == 0);
}

WWHD_SIZE(MeterHIOFloats, 0x60);
WWHD_SIZE(MeterHeapVtable, 0x48);
WWHD_SIZE(MeterHeap, 0x10);
WWHD_SIZE(MeterScreenVtable, 0x10);
WWHD_SIZE(MeterScreen, 0xCC);
WWHD_OFFSET(mapCtrlDisp_c, field_0x0, 0);
WWHD_OFFSET(mapCtrlDisp_c, field_0x1, 1);
WWHD_OFFSET(mapCtrlDisp_c, field_0x2, 2);
WWHD_OFFSET(MeterMapState, mIconFreePosX, 0);
WWHD_OFFSET(MeterMapState, mIconFreePosY, 2);
WWHD_OFFSET(MeterMapState, mPosLeftUpX, 6);
WWHD_OFFSET(MeterMapState, mPosLeftUpY, 8);
WWHD_OFFSET(MeterMapState, mSelfAlpha, 0x10);
WWHD_OFFSET(MeterMapState, mAlpha, 0x19);
WWHD_OFFSET(MeterMapState, mIconFreeAlpha, 0x1A);
WWHD_OFFSET(MeterMapState, mIconDisplayMode, 0x1B);

// HUD frame-update views. HD: preserve repeated play lookups and virtual-call order.
struct MeterUpdateCamera {
  u8 mPadding[0x510];
  be<u32> mFlags;
};
WWHD_OFFSET(MeterUpdateCamera, mFlags, 0x510);
WWHD_SIZE(MeterUpdateCamera, 0x514);
struct MeterUpdate2D {
  be<u8> mShow;
};
WWHD_SIZE(MeterUpdate2D, 1);
struct MeterUpdateMenu {
  be<u8> mMenuStatus;
  u8 mPadding1;
  be<u8> mWindowFlag;
  u8 mPadding3[2];
  be<u8> mAuctionFlag;
};
WWHD_OFFSET(MeterUpdateMenu, mWindowFlag, 2);
WWHD_OFFSET(MeterUpdateMenu, mAuctionFlag, 5);
WWHD_SIZE(MeterUpdateMenu, 6);
struct MeterUpdateCameraAttention {
  be<u32> mStatus;
};
WWHD_SIZE(MeterUpdateCameraAttention, 4);
struct MeterUpdatePlayerVtable {
  u8 mPadding[0xD4];
  be<u32> mCheckRopeTag;
};
WWHD_OFFSET(MeterUpdatePlayerVtable, mCheckRopeTag, 0xD4);
WWHD_SIZE(MeterUpdatePlayerVtable, 0xD8);
inline dComIfG_play_c *meterUpdatePlay() {
  return &gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play;
}
inline MeterUpdate2D *meterUpdate2DState() {
  return gabi::at<MeterUpdate2D>(dComIfGp_ea() + 0x5C22);
}
inline MeterUpdateMenu *meterUpdateMenu() {
  return gabi::at<MeterUpdateMenu>(0x101EA067);
}
inline s32 meterUpdateDemoMode() {
  return gabi::object_at<be<s32>>(0x101D6010);
}
inline u8 meterUpdateKamomeState() {
  return gabi::object_at<be<u8>>(0x101D5F45);
}
inline MeterUpdateCameraAttention *meterUpdateCameraAttention() {
  return gabi::at<MeterUpdateCameraAttention>(dComIfGp_ea() + 0x5B00);
}
inline BOOL meterUpdateCheckRopeTag(fopAc_ac_c *player) {
  u32 target = gabi::at<MeterUpdatePlayerVtable>(player->__vtbl)->mCheckRopeTag;
  return gabi::call_ptr<BOOL>(target, player);
}
inline meter_detail::StageInfo *meterUpdateStage() {
  return static_cast<meter_detail::StageInfo *>(dComIfGp_getStageStagInfo());
}
MeterUpdateCamera *meterUpdateCameraBody();
s32 meterUpdateStartCheckOld(dEvent_manager_c *, const char *);
u32 meterUpdateGetPictureRecord(u32);
u32 meterUpdatePictureActive(u32);

inline u32 meterUpdatePictureRecord() {
  u32 save = gabi::object_at<be<u32>>(0x101F84DC);
  return meterUpdateGetPictureRecord(save + 0x12C0);
}

f32 meterUpdateValueIncrease(s32, s32, u32);
inline dMeter_HIO_c *meterUpdateHIO() {
  return gabi::at<dMeter_HIO_c>(0x1047AD14);
}
inline s32 meterUpdateMenuTimer() {
  return gabi::object_at<be<s32>>(0x101EA02C);
}

struct MeterUpdateScratch {
  u8 mPadding0[8];
  be<u8> mButtonFlags;
  u8 mPadding9;
  be<u16> mSound;
  u8 mPaddingC[4];
  union {
    be<u32> mConvertedLife;
    struct {
      be<s16> mConversionHigh;
      be<s16> mLifeHalfword;
    };
  };
  u8 mPadding14[8];
};
WWHD_OFFSET(MeterUpdateScratch, mButtonFlags, 8);
WWHD_OFFSET(MeterUpdateScratch, mSound, 0xA);
WWHD_OFFSET(MeterUpdateScratch, mConvertedLife, 0x10);
WWHD_OFFSET(MeterUpdateScratch, mLifeHalfword, 0x12);
WWHD_SIZE(MeterUpdateScratch, 0x1C);
struct MeterUpdateSharedState {
  u8 mPadding0[0x18];
  be<f32> mHeartAlpha, mMagicAlpha, mRupyAlpha, mKeyAlpha;  /* mHeartAlpha was mActionAlpha (misnamed), mMagicAlpha was mSecondaryAlpha (misnamed): dMeter_Execute 02593B10 copies sub_meter_class mHeartAlpha / mMagicAlpha (the heart and magic gauge alpha ratios, dMeter_gaugeAlpha 0259C2BC) into them, as it copies mRupyAlpha / mKeyAlpha */
  u8 mPadding28[0x10];
  be<u32> mStatusFlags;
  be<u16> mMiniGameType;
  be<u8> mButtonFlags;
  u8 mPadding3F[2];
  be<u8> mButtonShowFlags;
  u8 mPadding42;
  be<u8> mBattleState, mBattleMode;
};
WWHD_OFFSET(MeterUpdateSharedState, mHeartAlpha, 0x18);
WWHD_OFFSET(MeterUpdateSharedState, mStatusFlags, 0x38);
WWHD_OFFSET(MeterUpdateSharedState, mButtonShowFlags, 0x41);
WWHD_OFFSET(MeterUpdateSharedState, mBattleState, 0x43);
WWHD_SIZE(MeterUpdateSharedState, 0x45);
struct MeterUpdatePlayer {
  u8 mPadding0[0x3BC];
  be<u32> mSoupPowerFlags;
};
WWHD_OFFSET(MeterUpdatePlayer, mSoupPowerFlags, 0x3BC);
WWHD_SIZE(MeterUpdatePlayer, 0x3C0);
inline MeterUpdateSharedState *meterUpdateSharedState() {
  return gabi::at<MeterUpdateSharedState>(0x1047B03C);
}
inline dSv_player_status_a_c *meterUpdatePlayerStatus() {
  return &dComIfGs_info()->mSavedata.mPlayer.mPlayerStatusA;
}
inline be<u8> &meterUpdateBattleFlag() {
  return gabi::object_at<be<u8>>(0x101EA078);
}
inline be<u8> &meterUpdateHeartSoundFlag() {
  return gabi::object_at<be<u8>>(0x101EA077);
}
inline u8 meterUpdateBattleState() {
  return gabi::object_at<be<u8>>(0x101D5F44);
}
void meterUpdateResetHearts(sub_meter_class *);

inline u8 meterUpdateCollectMode() {
  return gabi::object_at<be<u8>>(0x101EA073);
}
void meterUpdateHeartScaleCountdown(u32);
void meterUpdateMaxLifeCountdown(u32);
void meterUpdateActionAudio();

inline u8 meterUpdateMessageMode() {
  return gabi::object_at<be<u8>>(0x1047B09E);
}

inline be<s16> &meterUpdateActionMode() {
  return gabi::object_at<be<s16>>(0x101EA04E);
}
inline be<s16> &meterUpdateActionFrame() {
  return gabi::object_at<be<s16>>(0x101EA04C);
}
inline be<u8> &meterUpdateButtonFlags() {
  return gabi::object_at<be<u8>>(0x1047B07A);
}
inline u8 meterUpdateButtonIconMode() {
  return gabi::object_at<be<u8>>(0x1047B09F);
}
inline u8 meterUpdateMenuButton() {
  return gabi::object_at<be<u8>>(0x101EA06A);
}
inline u8 meterUpdateItemMode() { return gabi::object_at<be<u8>>(0x101EA072); }
BOOL meterUpdateIsBit(be<u8> *, u8);
void meterUpdateOnBit(be<u8> *, u8);
void meterUpdateOffBit(be<u8> *, u8);

struct MeterUpdateButtonState {
  u8 mPadding0[0x3039];
  be<u8> mMaskFrames;
};
WWHD_OFFSET(MeterUpdateButtonState, mMaskFrames, 0x3039);
WWHD_SIZE(MeterUpdateButtonState, 0x303A);
struct MeterUpdateSystem {
  u8 mPadding0[0x17C];
  be<u32> mHudContext;
};
WWHD_OFFSET(MeterUpdateSystem, mHudContext, 0x17C);
WWHD_SIZE(MeterUpdateSystem, 0x180);
inline MeterUpdateSystem *meterUpdateSystem() {
  return gabi::at<MeterUpdateSystem>(gabi::object_at<be<u32>>(0x101F8344));
}
inline be<s16> &meterUpdateActionAlphaFrame() {
  return gabi::object_at<be<s16>>(0x101EA050);
}
void meterUpdateWeaponMove(sub_meter_class *);
void meterUpdateXYMove(sub_meter_class *);
void meterUpdateRAlpha(sub_meter_class *);
void meterUpdateEnemyMove(sub_meter_class *);
void meterUpdateMagicGaugeMove(sub_meter_class *);
void meterUpdateMagicAlpha(sub_meter_class *);
void meterUpdateMetronomeVisibility(sub_meter_class *);
void meterUpdateRupyMove(sub_meter_class *);
void meterUpdateKeyMove(sub_meter_class *);
void meterUpdateCompassMove(sub_meter_class *);
void meterUpdateRecollectBossData();
void meterUpdateMetronomeMove(sub_meter_class *);
void meterUpdateGaugeAlpha(sub_meter_class *);
void meterUpdateWindMove(sub_meter_class *);
BOOL meterUpdateGetterB();
BOOL meterUpdateIsPause();
void meterUpdateDecTimer(dSv_player_item_record_c *);
BOOL meterUpdateHudContextActive(u32);
void meterUpdateSetAlpha(fopMsgM_pane_class *, f32);
void meterUpdateSetAlphaZero(fopMsgM_pane_class *);
void *meterUpdateActorNameJudge(void *, void *);
using MeterUpdateActorJudgeFn = void *(*)(void *, void *);
void *meterUpdateActorJudge(MeterUpdateActorJudgeFn, void *);

namespace MeterUpdate {
enum dActStts_e : u8 {
  dActStts_BLANK_e = 0x0,
  dActStts_RETURN_e = 0x7,
  dActStts_CHOOSE_e = 0x17,
  dActStts_NEXT_e = 0x19,
  dActStts_PARRY_e = 0x1A,
  dActStts_ba_nageru__dupe_2D = 0x2D,
  dActStts_ba_motu__dupe_2E = 0x2E,
  dActStts_ba_sake__dupe_31 = 0x31,
  dActStts_bow_01 = 0x3D,
  dActStts_HIDDEN_e = 0x3E,
  dActStts_HD_RETURN_e = 0x46,
};
} // namespace MeterUpdate

// Meter weapon range: HD omits the GameCube HIO number and retains alignment.
struct dMeter_message_HIO_c {
    /* 0x00 */ be<u32> mVtable;
    /* 0x04 */ GXColor mTextColor;  /* was field_0x5: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the font colour of the text panes (no WWHD read) */
    /* 0x08 */ GXColor mTextWhite;  /* was field_0x9: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the white colour of the text panes (no WWHD read) */
    /* 0x0C */ GXColor mTextBlack;  /* was field_0xd: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the black colour of the text panes (no WWHD read) */
    /* 0x10 */ GXColor mShadowColor;  /* was field_0x11: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the font colour of the text and ruby shadow panes (no WWHD read) */
    /* 0x14 */ GXColor mShadowWhite;  /* was field_0x15: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the white colour of the text and ruby shadow panes (no WWHD read) */
    /* 0x18 */ GXColor mShadowBlack;  /* was field_0x19: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the black colour of the text and ruby shadow panes (no WWHD read) */
    /* 0x1C */ GXColor mRubyColor;  /* was field_0x1d: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the font colour of the ruby panes (no WWHD read) */
    /* 0x20 */ GXColor mRubyWhite;  /* was field_0x21: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the white colour of the ruby panes (no WWHD read) */
    /* 0x24 */ GXColor mRubyBlack;  /* was field_0x25: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the black colour of the ruby panes (no WWHD read) */
    /* 0x28 */ be<u8> mAimBrightness;  /* was field_0x29: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_aimBrightness returns 0xFF up to it and fades above it (no WWHD read) */
    /* 0x29 */ be<u8> field_0x2a;
    /* 0x2A */ be<u8> field_0x2b;
    /* 0x2B */ be<u8> field_0x2c;
    /* 0x2C */ be<u8> field_0x2d;
    /* 0x2D */ be<u8> field_0x2e;
    /* 0x2E */ be<u8> field_0x2f;
    /* 0x2F */ be<u8> field_0x30;
    /* 0x30 */ be<u8> field_0x31;
    /* 0x31 */ be<u8> mMsg2FontSize;  /* was field_0x32: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_screenDataInit: the font size (no WWHD read) */
    /* 0x32 */ be<s16> field_0x34;
    /* 0x34 */ be<s16> field_0x36;
    /* 0x36 */ be<s16> mMsg2RubyOffsetY;  /* was field_0x38: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute places the ruby panes and their shadows it above the text; dMsg2_setCharAlpha uses the same offset (no WWHD read) */
    /* 0x38 */ be<u8> field_0x3a;
    /* 0x39 */ be<u8> mPopSpeed;  /* was field_0x3b: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Create copies it into dMsg2_popSpeed (no WWHD read) */
    /* 0x3A */ be<s16> mLineHeightAdd;  /* was field_0x3c: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_ScreenDataValueInit: the line height is the text pane's height + it (no WWHD read) */
    /* 0x3C */ be<f32> field_0x40;
    /* 0x40 */ be<u8> field_0x44;
    u8 mPadding41[1];
    /* 0x42 */ be<s16> field_0x46;
    /* 0x44 */ be<s16> field_0x48;
    /* 0x46 */ be<u8> field_0x4a;
    /* 0x47 */ be<u8> mShowRuby;  /* was field_0x4b: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_rubySet hides the ruby panes while it is 0 (no WWHD read) */
    /* 0x48 */ be<u8> field_0x4c;
    /* 0x49 */ be<u8> field_0x4d;
    /* 0x4A */ be<u8> field_0x4e;
    /* 0x4B */ be<u8> field_0x4f;
    /* 0x4C */ be<u8> field_0x50;
    /* 0x4D */ be<u8> field_0x51;
    /* 0x4E */ be<u8> field_0x52;
    /* 0x4F */ be<u8> field_0x53;
    /* 0x50 */ be<s16> field_0x54;
    /* 0x52 */ be<s16> field_0x56;
    /* 0x54 */ be<s16> mTextOffsetX;  /* was field_0x58: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute places the text, ruby and shadow panes at the text pane's x + it (no WWHD read) */
    /* 0x56 */ be<u8> field_0x5a;
    /* 0x57 */ be<u8> mMultiTexAlpha;  /* was field_0x5b: WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_multiTexDraw scales the overlay alpha by it / 255 (no WWHD read) */
    /* 0x58 */ be<u8> field_0x5c;
    /* 0x59 */ be<u8> field_0x5d;
    /* 0x5A */ be<u8> field_0x5e;
    u8 mPadding5B[1];
    /* 0x5C */ be<s16> field_0x60;
    /* 0x5E */ be<s16> field_0x62;
    /* 0x60 */ be<s16> field_0x64;
    /* 0x62 */ be<s16> field_0x66;
    /* 0x64 */ be<s16> field_0x68;
    /* 0x66 */ be<s16> field_0x6a;
    /* 0x68 */ be<s16> field_0x6c;
    /* 0x6A */ be<s16> field_0x6e;
    /* 0x6C */ be<s16> field_0x70;
    /* 0x6E */ be<s16> field_0x72;
    /* 0x70 */ be<s16> field_0x74;
    /* 0x72 */ be<s16> field_0x76;
    /* 0x74 */ be<u8> field_0x78;
    u8 mPadding75[1];
    /* 0x76 */ be<s16> field_0x7a;
    /* 0x78 */ be<s16> field_0x7c;
    /* 0x7A */ be<s16> field_0x7e;
    /* 0x7C */ be<s16> field_0x80;
    /* 0x7E */ be<s16> field_0x82;
};
WWHD_SIZE(dMeter_message_HIO_c, 0x80);
WWHD_OFFSET(dMeter_message_HIO_c, mVtable, 0x0);
WWHD_OFFSET(dMeter_message_HIO_c, mTextColor, 0x4);
WWHD_OFFSET(dMeter_message_HIO_c, mTextWhite, 0x8);
WWHD_OFFSET(dMeter_message_HIO_c, mTextBlack, 0xC);
WWHD_OFFSET(dMeter_message_HIO_c, mShadowColor, 0x10);
WWHD_OFFSET(dMeter_message_HIO_c, mShadowWhite, 0x14);
WWHD_OFFSET(dMeter_message_HIO_c, mShadowBlack, 0x18);
WWHD_OFFSET(dMeter_message_HIO_c, mRubyColor, 0x1C);
WWHD_OFFSET(dMeter_message_HIO_c, mRubyWhite, 0x20);
WWHD_OFFSET(dMeter_message_HIO_c, mRubyBlack, 0x24);
WWHD_OFFSET(dMeter_message_HIO_c, mAimBrightness, 0x28);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x2a, 0x29);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x2b, 0x2A);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x2c, 0x2B);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x2d, 0x2C);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x2e, 0x2D);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x2f, 0x2E);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x30, 0x2F);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x31, 0x30);
WWHD_OFFSET(dMeter_message_HIO_c, mMsg2FontSize, 0x31);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x34, 0x32);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x36, 0x34);
WWHD_OFFSET(dMeter_message_HIO_c, mMsg2RubyOffsetY, 0x36);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x3a, 0x38);
WWHD_OFFSET(dMeter_message_HIO_c, mPopSpeed, 0x39);
WWHD_OFFSET(dMeter_message_HIO_c, mLineHeightAdd, 0x3A);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x40, 0x3C);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x44, 0x40);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x46, 0x42);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x48, 0x44);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x4a, 0x46);
WWHD_OFFSET(dMeter_message_HIO_c, mShowRuby, 0x47);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x4c, 0x48);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x4d, 0x49);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x4e, 0x4A);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x4f, 0x4B);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x50, 0x4C);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x51, 0x4D);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x52, 0x4E);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x53, 0x4F);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x54, 0x50);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x56, 0x52);
WWHD_OFFSET(dMeter_message_HIO_c, mTextOffsetX, 0x54);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x5a, 0x56);
WWHD_OFFSET(dMeter_message_HIO_c, mMultiTexAlpha, 0x57);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x5c, 0x58);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x5d, 0x59);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x5e, 0x5A);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x60, 0x5C);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x62, 0x5E);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x64, 0x60);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x66, 0x62);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x68, 0x64);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x6a, 0x66);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x6c, 0x68);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x6e, 0x6A);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x70, 0x6C);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x72, 0x6E);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x74, 0x70);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x76, 0x72);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x78, 0x74);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x7a, 0x76);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x7c, 0x78);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x7e, 0x7A);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x80, 0x7C);
WWHD_OFFSET(dMeter_message_HIO_c, field_0x82, 0x7E);

struct MeterWeaponMapHD {
    u8 mBeforeDisplay[0x14];
    be<u8> mHDDisplay;
};
WWHD_OFFSET(MeterWeaponMapHD, mHDDisplay, 0x14);
WWHD_SIZE(MeterWeaponMapHD, 0x15);
struct MeterWeaponGrafVtable {
    u8 mReserved[0x24];
    be<u32> mSetPort;
};
struct MeterWeaponGraf {
    u8 mReserved[0xB8];
    gptr<MeterWeaponGrafVtable> mVtable;
    void setPort() { gabi::call_ptr(mVtable->mSetPort, this); }
};
WWHD_OFFSET(MeterWeaponGraf, mVtable, 0xB8);
WWHD_SIZE(MeterWeaponGraf, 0xBC);
WWHD_SIZE(MeterWeaponGrafVtable, 0x28);
struct MeterWeaponScreenHD {
    u8 mReserved[0xCC];
    be<u8> mScissor;
};
WWHD_OFFSET(MeterWeaponScreenHD, mScissor, 0xCC);
WWHD_SIZE(MeterWeaponScreenHD, 0xCD);
struct MeterWeaponClockArray {
    u8 mClock[3][0xC4];
};
WWHD_SIZE(MeterWeaponClockArray, 0x24C);

struct MeterWeaponDrawList;

struct MeterWeaponApi {
    // HD: this existing meter-TU vtable is owned by d_meter_02.
    enum : u32 { safeStringVtable = 0x1005107C };
    static void registerGlobal(void *);
    static void initializeHeaderByte(void *);
    static void initializeHeaderFlag(void *);
    static void constructDrawList(MeterWeaponDrawList *);
    static void constructArray(void *, u32, u32, u32);

    static u8 getAStatusForce() { return gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mAStatusForce; }
    static bool eventHind();
    static meter_detail::StageInfo *stageInfo() {
        auto data = reinterpret_cast<meter_detail::StageData *>(
            &gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mStageData);
        return gabi::at<meter_detail::StageInfo>(data->mVtable->getStag(data));
    }
    static void drawScreen(u32, MeterWeaponGraf *, f32, f32);
    static void destroyArray(void *, u32, u32, u32, u32, u32);
    enum : u32 { destroyClock = 0x0259CFB4, constructClock = 0x0259CF60 };
    static MeterWeaponGraf *getGrafPort() {
        return gabi::at<MeterWeaponGraf>(gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mCurrentGrafPort);
    }
};
extern be<f32> &l_messageScale;
extern be<f32> &l_mapIconScale;
extern be<u8> &l_itemMoveFlag;
extern be<u8> &l_menuPause;
extern MeterWeaponClockArray &l_meterClock;

// HD: scratch slots retain GHS SafeString and integer-conversion effects.
struct MeterWeaponScratch {
    u8 mReserved[8];
    be<u8> mMoveFlag;
    u8 mPadding9[3];
    meter_detail::SafeStringView mNames[4];
    u8 mPadding2C[4];
    union {
        struct {
            be<u32> mConversionHigh, mConversionLow;
        };
        be<u8> mConversionBytes[8];
    };
    meter_detail::SafeStringView mStages[4];
};
WWHD_OFFSET(MeterWeaponScratch, mNames, 0xC);
WWHD_OFFSET(MeterWeaponScratch, mConversionHigh, 0x30);
WWHD_OFFSET(MeterWeaponScratch, mStages, 0x38);
WWHD_SIZE(MeterWeaponScratch, 0x58);
struct MeterWeaponEventHD {
    u8 mBeforeHind[0x5292];
    be<u8> mEventFlag;
    u8 mBeforeMask[0x52A6 - 0x5293];
    be<u16> mHindMask;
};
WWHD_OFFSET(MeterWeaponEventHD, mEventFlag, 0x5292);
WWHD_OFFSET(MeterWeaponEventHD, mHindMask, 0x52A6);
WWHD_SIZE(MeterWeaponEventHD, 0x52A8);
extern be<f64> &l_weaponUnsignedBias;
extern be<f64> &l_weaponSignedBias;
extern be<s16> &l_weaponMoveFrame;
extern be<s16> &l_weaponMoveStatus;
extern be<s16> &l_weaponAlphaFrame;
extern be<u8> &l_weaponMenuButton;
extern be<u8> &l_weaponMenuStatus;
extern be<u8> &l_weaponItemMode;
inline bool MeterWeaponApi::eventHind() {
    if (gabi::at<MeterWeaponEventHD>(dComIfGp_ea())->mEventFlag == 0)
        return false;
    return (gabi::at<MeterWeaponEventHD>(dComIfGp_ea())->mHindMask & 4) != 0;
}

// HD: the common header initializer uses one-byte constructors and GHS registration nodes.
struct MeterWeaponInitializeWords {
    be<u32> mWords[4];
};
struct MeterWeaponRegistration {
    u8 mData[0xC];
};
struct MeterWeaponRegistrations {
    MeterWeaponRegistration mEntries[7];
};
struct MeterWeaponHeaderBytes {
    be<u8> mFirst, mSecond;
};
struct MeterWeaponTextureState {
    be<f32> mRateX, mRateY, mScaleX, mScaleY, mScaleMax, mScaleMin;
};
struct MeterWeaponDrawList {
    be<u32> mVtable;
};
WWHD_SIZE(MeterWeaponInitializeWords, 0x10);
WWHD_SIZE(MeterWeaponRegistration, 0xC);
WWHD_SIZE(MeterWeaponRegistrations, 0x54);
WWHD_SIZE(MeterWeaponHeaderBytes, 2);
WWHD_SIZE(MeterWeaponTextureState, 0x18);
WWHD_SIZE(MeterWeaponDrawList, 4);
extern MeterWeaponInitializeWords &l_meterInitializeWords;
extern MeterWeaponRegistrations &l_meterRegistrations;
extern MeterWeaponHeaderBytes &l_meterHeaderBytes;
extern MeterWeaponTextureState &l_meterTextureState;
extern MeterWeaponDrawList &l_meter1;
extern MeterWeaponDrawList &l_meter2;
extern MeterWeaponDrawList &l_meterTekari;
extern dMeter_HIO_c &g_meterHIO;
extern dMeter_menuHIO_c &g_menuHIO;
extern dMeter_msg_HIO_c &g_msgHIO;
extern dMeter_message_HIO_c &g_messageHIO;
extern be<f32> &l_meterRateX;
extern be<f32> &l_meterRateY;
extern be<f32> &l_meterScale;
extern be<f32> &l_meterScaleMax;

namespace meter_weapon {
enum ActionStatus {
    dActStts_BLANK_e = 0,
    dActStts_SPEAK_e = 2,
    dActStts_RETURN_e = 7,
    dActStts_BID_e = 37,
    dActStts_CANCEL_e = 39,
    dActStts_ba_nageru__dupe_2D = 45,
    dActStts_ba_motu__dupe_2E = 46,
    dActStts_ba_sake__dupe_31 = 49,
    dActStts_sword_01 = 53,
    dActStts_boko_stick = 55,
    dActStts_hatchet = 56,
    dActStts_iron_club = 57,
    dActStts_longsword = 58,
    dActStts_spear = 59,
    dActStts_spear__dupe_3C = 60,
    dActStts_HIDDEN_e = 62,
};
}

// HD: creation retains its original 0x130-byte frame, four screen bounds, and copied tag tables.
struct MeterCreateScratch {
    u8 mPadding0[8];
    union {
        be<u32> mInteger;
        struct { be<u16> mIntegerHigh; be<u16> mIntegerLow; };
    };
    be<u32> mConversionLow;
    J2DPane::Bounds mScreenBounds[4];
    be<u32> mHeartTags[20];
    be<u32> mHeartShadowTags[20];
    u8 mPaddingF0[0x40];
};
WWHD_OFFSET(MeterCreateScratch, mInteger, 8);
WWHD_OFFSET(MeterCreateScratch, mIntegerLow, 0xA);
WWHD_OFFSET(MeterCreateScratch, mScreenBounds, 0x10);
WWHD_OFFSET(MeterCreateScratch, mHeartTags, 0x50);
WWHD_OFFSET(MeterCreateScratch, mHeartShadowTags, 0xA0);
WWHD_SIZE(MeterCreateScratch, 0x130);
struct MeterCreateScreen {
    u8 mPadding0[0xC8];
    be<u32> mVtable;
    be<u8> mShow;
    be<u32> mColor;
    u8 mPaddingD1[3];
};
WWHD_OFFSET(MeterCreateScreen, mVtable, 0xC8);
WWHD_OFFSET(MeterCreateScreen, mShow, 0xCC);
WWHD_OFFSET(MeterCreateScreen, mColor, 0xCD);
WWHD_SIZE(MeterCreateScreen, 0xD4);
struct MeterCreateHeapVtable {
    u8 mPadding0[0x34];
    be<u32> mAlloc;
};
struct MeterCreateHeap {
    u8 mPadding0[0xC];
    gptr<MeterCreateHeapVtable> mVtable;
    u32 alloc(u32 size, s32 alignment) {
        return gabi::call_ptr<u32>(mVtable->mAlloc, this, size, alignment);
    }
};
WWHD_OFFSET(MeterCreateHeapVtable, mAlloc, 0x34);
WWHD_SIZE(MeterCreateHeapVtable, 0x38);
WWHD_OFFSET(MeterCreateHeap, mVtable, 0xC);
WWHD_SIZE(MeterCreateHeap, 0x10);
struct MeterCreateMenuState {
    be<s32> dMenu_frame_timer;
    be<f32> texScaleX, texScaleY, texRateX, texRateY;
    be<u32> dMeter_windID;
    be<s16> dMeter_itemTimer;
    u8 mPadding1A[0x20];
    be<u8> menu_status_old, menu_status, subWinFlag, dMenu_pause, dMenu_menuButton;
    be<u8> dMeter_btn_chk, dMeter_auctionFlag, dMeter_itemNum, dMeter_place_name;
    be<u8> dMeter_itemMoveFlag, dMenu_menuFlag, mapAlpha, dMeter_itemMode, dMeter_collectMode, dMeter_windStatus;
    be<u8> rupy_soundSetFlag, rupy_soundOnFlag;
};
WWHD_OFFSET(MeterCreateMenuState, menu_status_old, 0x3A);
WWHD_OFFSET(MeterCreateMenuState, dMeter_itemMoveFlag, 0x43);
WWHD_SIZE(MeterCreateMenuState, 0x4B);
inline MeterCreateMenuState *meterCreateMenuState() {
    return gabi::at<MeterCreateMenuState>(0x101EA02C);
}
inline be<u8> &meterCreateBaitNum(s32 index) { return gabi::at<be<u8>>(0x1047A98C)[index]; }
inline be<u32> &meterCreateArrowTextureName(u32 type) { return gabi::at<be<u32>>(0x101EA07C)[type]; }
inline mapCtrlDisp_c *meterCreateMapControl() { return gabi::at<mapCtrlDisp_c>(0x1047AD0C); }
inline u32 meterCreateMenuArchive() { return gabi::load<u32>(dComIfGp_ea() + 0x5A40); }
inline u32 meterCreateItemIconArchive() { return gabi::load<u32>(dComIfGp_ea() + 0x5A84); }
inline be<u8> &meterCreateMagicFlags() { return gabi::object_at<be<u8>>(dComIfGp_ea() + 0x5BEF); }
inline be<u8> &meterCreateWindCancelFlag() { return gabi::object_at<be<u8>>(dComIfGp_ea() + 0x5BD0); }
inline be<u8> &meterCreateItemOpenFlag() { return gabi::object_at<be<u8>>(dComIfGp_ea() + 0x5BEC); }
inline be<u8> &meterCreateTimerFlag() { return gabi::object_at<be<u8>>(dComIfGp_ea() + 0x5BE0); }
inline dSv_info_c *meterCreateSave() { return dComIfGs_info(); }
// HD: the bag selector is inlined and accepts all byte values. Keep its original bounded ranges.
inline u8 meterCreateSelectedItem(u8 slot) {
    auto *player = &meterCreateSave()->mSavedata.mPlayer;
    if (slot < 21) return player->mPlayerItem.mItems[slot];
    if (slot >= 24 && slot < 32) return player->mBagItem.mBeast[slot - 24];
    if (slot >= 36 && slot < 44) return player->mBagItem.mBait[slot - 36];
    if (static_cast<u32>(slot - 48) < 8) return player->mBagItem.mReserve[slot - 48];
    return 0xFF;
}

u32 meterCreateExpHeap(u32);
u32 meterCreateAllocate(u32, u32, s32);
u32 meterCreatePaneConstruct(u32, u32, u32, u32, J2DPane::Bounds *);
void meterCreateScreenSet(u32, const char *, u32);
u32 meterCreateAssert(const char *, s32, const char *);
u32 meterCreateHIOChild(const char *, dMeter_map_HIO_c *);
void meterCreateSetPaneData(fopMsgM_pane_class *, u32, u32);
void meterCreateWalletChange(sub_meter_class *);
u32 meterCreateIsDungeonItemCompass(u32, u32);
void meterCreateCompassInit(sub_meter_class *);
void meterCreateHeartInit(sub_meter_class *);
void meterCreateXYItemChange(sub_meter_class *, u32);
f32 meterCreateRndF(f32);
void meterCreateMagicLength(sub_meter_class *, f32);
void meterCreateMagicChange(sub_meter_class *, f32);
void meterCreateMagicTransNowInit(sub_meter_class *);
void meterCreateSetNowAlphaZero(fopMsgM_pane_class *);
u32 meterCreateArrowType();
void meterCreateReadTexture(u32, u32, u32, u32, u32);
void meterCreateStoreTexture(u32, u32);
void meterCreateStockTimer();
void meterCreateSetStageLayer(sub_meter_class *);
u32 meterCreateGetTimer(dSv_player_item_record_c *);

inline u32 meterCreateDungeonMemory() { return gabi::ea(&meterCreateSave()->mMemory.mMembit); }
inline u8 meterCreateKeyNum() { return meterCreateSave()->mMemory.mMembit.mKeyNum; }
enum MeterCreateVtables { l_createMyScreenVtable = 0x100511A4, l_createScreenVtable = 0x1016E084 };

// HD: HUD creation initializes this additional UI mode byte.
inline be<u8> &meterCreateHudMode() { return gabi::object_at<be<u8>>(dComIfGp_ea() + 0x5C2F); }

WWHD_OFFSET(MeterCreateMenuState, dMenu_frame_timer, 0x0);
WWHD_OFFSET(MeterCreateMenuState, texScaleX, 0x4);
WWHD_OFFSET(MeterCreateMenuState, texScaleY, 0x8);
WWHD_OFFSET(MeterCreateMenuState, texRateX, 0xC);
WWHD_OFFSET(MeterCreateMenuState, texRateY, 0x10);
WWHD_OFFSET(MeterCreateMenuState, dMeter_windID, 0x14);
WWHD_OFFSET(MeterCreateMenuState, dMeter_itemTimer, 0x18);
WWHD_OFFSET(MeterCreateMenuState, menu_status, 0x3B);
WWHD_OFFSET(MeterCreateMenuState, subWinFlag, 0x3C);
WWHD_OFFSET(MeterCreateMenuState, dMenu_pause, 0x3D);
WWHD_OFFSET(MeterCreateMenuState, dMenu_menuButton, 0x3E);
WWHD_OFFSET(MeterCreateMenuState, dMeter_btn_chk, 0x3F);
WWHD_OFFSET(MeterCreateMenuState, dMeter_auctionFlag, 0x40);
WWHD_OFFSET(MeterCreateMenuState, dMeter_itemNum, 0x41);
WWHD_OFFSET(MeterCreateMenuState, dMeter_place_name, 0x42);
WWHD_OFFSET(MeterCreateMenuState, dMenu_menuFlag, 0x44);
WWHD_OFFSET(MeterCreateMenuState, mapAlpha, 0x45);
WWHD_OFFSET(MeterCreateMenuState, dMeter_itemMode, 0x46);
WWHD_OFFSET(MeterCreateMenuState, dMeter_collectMode, 0x47);
WWHD_OFFSET(MeterCreateMenuState, dMeter_windStatus, 0x48);
WWHD_OFFSET(MeterCreateMenuState, rupy_soundSetFlag, 0x49);
WWHD_OFFSET(MeterCreateMenuState, rupy_soundOnFlag, 0x4A);
WWHD_OFFSET(MeterCreateScratch, mIntegerHigh, 8);
WWHD_OFFSET(MeterCreateScratch, mConversionLow, 0xC);

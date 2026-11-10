/**
 * d_camera.h (WWHD) - follow camera dCamera_c, WWHD layout.
 *
 * Layout measured from the WWHD code (constructor
 * 02501B8C allocates 0x8E0; GameCube 0x78C). GameCube names are kept; "GC" gives the GameCube
 * offset where it differs.
 *  - 0x000..0x14B: unchanged.
 *  - 0x14C..0x157: HD has three floats where GameCube has mSightFitRadius + 4 unknown bytes; from 0x158
 *    (GC 0x154) everything is shifted by +4 up to mWindowAspectRatio/mWallUpDist (HD 0x608/0x60C).
 *  - 0x610..0x73B: HD-only (a byte, a sead::LookAtCamera at 0x614 with its vtable at 0x644,
 *    a sead projection at 0x66C, 8 bytes at 0x730 and a byte at 0x738).
 *  - 0x73C: dCamSetup_c (HD 0x168, GC 0x144); 0x8A4: dCamParam_c; 0x8B0: camera type ids.
 * cSAngle / cSGlobe / cXyz operators are out of line in HD (c_angle / c_xyz): every use is a call.
 */
#pragma once
#include "bindings.h"

typedef be<s16> cSAngle_l;  /* cSAngle: one s16 */

struct cSGlobe_l {          /* cSGlobe: radius, V (pitch), U (yaw) */
    /* 0x0 */ be<f32> mRadius;
    /* 0x4 */ be<s16> mV;
    /* 0x6 */ be<s16> mU;
};
WWHD_SIZE(cSGlobe_l, 0x8);

struct camera_class;

struct camSphChkdata_l {
    /* 0x00 */ gptr<cXyz> field_0x0;
    /* 0x04 */ be<f32> field_0x4;
    /* 0x08 */ cXyz field_0x8;
    /* 0x14 */ cXyz field_0x14;
};
WWHD_SIZE(camSphChkdata_l, 0x20);

struct dCamForcusLine {
    /* 0x00 */ u8 mEffectLine[0x38];  /* dDlst_effectLine_c */
    /* 0x38 */ cXyz m38;
    /* 0x44 */ GXColor m44;
    /* 0x48 */ be<u8> m48;
    /* 0x49 */ be<u8> m49;
    /* 0x4A */ u8 _4A[2];
    /* 0x4C */ be<s32> m4C;
    /* 0x50 */ be<s32> m50;
    /* 0x54 */ be<s32> m54;
    /* 0x58 */ be<u16> m58;
    /* 0x5A */ be<u16> m5A;
    /* 0x5C */ be<u16> m5C;
    /* 0x5E */ be<u16> m5E;
    /* 0x60 */ be<f32> m60;
    /* 0x64 */ be<f32> m64;
    /* 0x68 */ be<f32> m68;
    /* 0x6C */ be<f32> m6C;
};
WWHD_SIZE(dCamForcusLine, 0x70);

struct dCamera_c {
    struct ViewCache {
        /* 0x00 */ cSGlobe_l mDirection;
        /* 0x08 */ cXyz mCenter;
        /* 0x14 */ cXyz mEye;
        /* 0x20 */ cSAngle_l mBank;
        /* 0x22 */ u8 _22[2];
        /* 0x24 */ be<f32> mFovy;
    };
    struct PosSet {
        /* 0x00 */ cXyz mCenter;
        /* 0x0C */ cXyz mEye;
        /* 0x18 */ be<f32> mFovY;
        /* 0x1C */ cSAngle_l mBank;
        /* 0x1E */ be<s16> m1E;
    };
    struct DMC {
        /* 0x0 */ be<u8> field_0x0;
        /* 0x1 */ be<u8> field_0x1;
        /* 0x2 */ cSAngle_l field_0x2;
        /* 0x4 */ cSAngle_l field_0x4;
    };
    struct Monitor {
        /* 0x00 */ cXyz mPos;
        /* 0x0C */ cXyz field_0x0C;
        /* 0x18 */ be<s32> field_0x18;
        /* 0x1C */ be<f32> field_0x1C;
    };
    struct BGChk {
        /* 0x00 */ be<u8> m00;
        /* 0x01 */ u8 _01[3];
        /* 0x04 */ dBgS_CamGndChk m04;
        /* 0x58 */ be<f32> m58;
    };
    /* GameCube struct BG (0xB8) */
    struct BG {
        /* 0x00 */ BGChk m00;
        /* 0x5C */ BGChk m5C;
    };
    struct EventParam {
        /* 0x00 */ char mName[16];
        /* 0x10 */ be<s32> mValue;
    };
    struct EventData {
        /* 0x000 */ be<u8> field_0x00;
        /* 0x001 */ u8 _001[3];
        /* 0x004 */ be<s32> mStaffIdx;
        /* 0x008 */ be<s32> field_0x08;
        /* 0x00C */ be<s32> field_0x0c;
        /* 0x010 */ be<u8> field_0x10;
        /* 0x011 */ u8 _011[3];
        /* 0x014 */ be<s32> field_0x14;
        /* 0x018 */ be<s32> field_0x18;
        /* 0x01C */ be<s32> field_0x1c;
        /* 0x020 */ be<s32> field_0x20;
        /* 0x024 */ be<s32> field_0x24;
        /* 0x028 */ be<s32> field_0x28;
        /* 0x02C */ EventParam mEventParams[8];
        /* 0x0CC */ be<u32> field_0xcc;     /* dStage_Event_dt_c* (GC comment 0xEC) */
        /* 0x0D0 */ u8 mSpline2DPath[0x44]; /* d2DBSplinePath */
    };

    /* 0x000 */ gptr<camera_class> mpCamera;
    /* 0x004 */ be<u8> mActive;
    /* 0x005 */ be<u8> mPause;
    /* 0x006 */ cSAngle_l m006;
    /* 0x008 */ cSGlobe_l mDirection;
    /* 0x010 */ cXyz mCenter;
    /* 0x01C */ cXyz mEye;
    /* 0x028 */ cXyz mUp;
    /* 0x034 */ cSAngle_l mBank;
    /* 0x036 */ u8 _036[2];
    /* 0x038 */ be<f32> mFovy;
    /* 0x03C */ ViewCache mViewCache;
    /* 0x064 */ be<f32> m064;
    /* 0x068 */ be<s32> mBumpCheckFlags;  /* was m068: Run 024FE3E8 sets 9/0x3F/0xF, ORs 0x40, passes it to bumpCheck; TP name */
    /* 0x06C */ cSAngle_l mAngleY;
    /* 0x06E */ u8 _06E[2];
    /* 0x070 */ cXyz m070;
    /* 0x07C */ be<u32> mFrameCounter;  /* was m07C: cleared by initialize 02500984, +1 per frame in Run 024FE3E8 and NotRun 024FF6C0; TP name */
    /* 0x080 */ be<u32> mTicks;  /* was m080: initialize 02500984 seeds it with cM_rndFX(32767), Run 024FE3E8 adds 1, event cameras test bit 0 (025314BC); TP name */
    /* 0x084 */ cXyz mSavedCenter;  /* was m084: pushPos 024FA3CC copies mViewCache.mCenter */
    /* 0x090 */ cXyz mSavedEye;  /* was m090: pushPos 024FA3CC copies mViewCache.mEye */
    /* 0x09C */ be<f32> mSavedFovy;  /* was m09C: pushPos 024FA3CC copies mViewCache.mFovy */
    /* 0x0A0 */ cSAngle_l mSavedBank;  /* was m0A0: pushPos 024FA3CC copies mViewCache.mBank */
    /* 0x0A2 */ u8 _0A2[2];
    /* 0x0A4 */ PosSet mSavedViewStack[2];  /* was m0A4: saveEvCamera 02538B54 / loadEvCamera 02538C7C store and load slot [index]; TP name */
    /* 0x0E4 */ be<s32> mStageMapToolCameraIdx;
    /* 0x0E8 */ be<s32> m0E8;
    /* 0x0EC */ cXyz mExtendedPos;
    /* 0x0F8 */ u8 _0F8[8];
    /* 0x100 */ be<u8> mStyleSettle0;  /* was m100: engines set m100..m102 together when the view reached its target (0250C77C); onModeChange 024FA79C clears them; Run 024FE3E8 sets ComStat 0x10 only when all three are set (TP: one flag, mStyleSettle.mFinished) */
    /* 0x101 */ be<u8> mStyleSettle1;  /* was m101: set and cleared together with mStyleSettle0 (0250C77C, 024FA79C); Run 024FE3E8 needs all three */
    /* 0x102 */ be<u8> mStyleSettle2;  /* was m102: set and cleared together with mStyleSettle0 (0250C77C, 024FA79C); Run 024FE3E8 needs all three */
    /* 0x103 */ u8 _103;
    /* 0x104 */ cSAngle_l m104;
    /* 0x106 */ cSAngle_l m106;
    /* 0x108 */ be<u32> mCurModeTimer;  /* was m108: onModeChange 024FA79C clears it, Run/NotRun (024FE3E8/024FF6C0) add 1; engines compare it with their transition length (02511200) */
    /* 0x10C */ be<s32> m10C;
    /* 0x110 */ be<u8> mModeChangeTransition;  /* was m110: onModeChange 024FA79C sets 1, clears it when mode 0<->1 keeps the same style; followCamera 025028B8 settles the view at once when it is 0 */
    /* 0x111 */ u8 _111[3];
    /* 0x114 */ be<s32> m114;
    /* 0x118 */ be<u32> mCurTypeTimer;  /* was m118: onTypeChange 024FA90C clears it, Run/NotRun add 1; eventCamera tests == 0 (024FF164) */
    /* 0x11C */ be<u32> mCurStyleTimer;  /* was m11C: onStyleChange 024FB334 clears it, Run/NotRun add 1; engines compare it with their transition frames (0250C77C); TP mCurCamStyleTimer */
    /* 0x120 */ be<s32> mCameraID;
    /* 0x124 */ be<s32> mPadId;
    /* 0x128 */ gptr<fopAc_ac_c> mpPlayerActor;
    /* 0x12C */ gptr<fopAc_ac_c> mpLockonTarget;
    /* 0x130 */ be<u32> mLockOnActorId;
    /* 0x134 */ gptr<fopAc_ac_c> mpLockonActor;
    /* 0x138 */ be<s32> mForceLockTimer;
    /* 0x13C */ be<s32> mCurMode;
    /* 0x140 */ be<s32> mNextMode;
    /* 0x144 */ be<s32> mManualInactive;  /* was m144: initialize 02500984 sets 1; nextMode 024FA9C8 clears it when the C stick passes the manual-start threshold and sets 1 to leave; onTypeChange 024FA90C sets 1 for fixed/event styles */
    /* 0x148 */ cSAngle_l mForwardTiltOffset;  /* was m148: initialize 02500984 sets cSAngle 0; followCamera 025028B8 / towerCamera 0250B30C ease it by 0.33; TP name at the same place (before mSightFitRadius) */
    /* 0x14A */ u8 _14A[2];
    /* 0x14C */ be<f32> mSightFitRadius;  /* was m14C: bumpCheck 024FD11C eases it toward the sight radius and adds it to the radius; onModeChange 024FA79C clears it; TP name */
    /* 0x150 */ be<f32> m150;
    /* 0x154 */ be<f32> m154;
    /* 0x158 */ be<f32> mStickMainPosXLast;   /* GC 0x154: +4 from here */
    /* 0x15C */ be<f32> mStickMainPosYLast;
    /* 0x160 */ be<f32> mStickMainValueLast;
    /* 0x164 */ be<f32> mStickMainPosXDelta;
    /* 0x168 */ be<f32> mStickMainPosYDelta;
    /* 0x16C */ be<f32> mStickMainValueDelta;
    /* 0x170 */ be<f32> mStickCPosXLast;
    /* 0x174 */ be<f32> mStickCPosYLast;
    /* 0x178 */ be<f32> mStickCValueLast;
    /* 0x17C */ be<f32> mStickCPosXDelta;
    /* 0x180 */ be<f32> mStickCPosYDelta;
    /* 0x184 */ be<f32> mStickCValueDelta;
    /* 0x188 */ be<s32> mCStickYState;  /* was m184: initPad 025006D8 clears it with m188/m18C (TP initPad clears the same three); nextMode 024FA9C8 sets 0/1 for the C stick Y direction; TP name */
    /* 0x18C */ be<u32> mCStickYHoldCount;  /* was m188: cleared by initPad 025006D8 with mCStickYState (TP initPad: the same three in this order); TP name */
    /* 0x190 */ be<u32> mCStickUpLatch;  /* was m18C: cleared by initPad 025006D8 with mCStickYState (TP initPad: the same three in this order); TP name */
    /* 0x194 */ be<f32> mTriggerLeftLast;
    /* 0x198 */ be<f32> mTriggerLeftDelta;
    /* 0x19C */ be<u8> mHoldLockL;
    /* 0x19D */ be<u8> mTrigLockL;
    /* 0x19E */ be<u8> mLockLActive;  /* was m19A: updatePad 024F9BF4: 1 while the L analog value is above ManualEndVal; TP name */
    /* 0x19F */ be<u8> mLockLJustActivated;  /* was m19B: updatePad 024F9BF4: m19A ^ 1 before m19A is set (first frame only); TP name */
    /* 0x1A0 */ be<f32> mTriggerRightLast;
    /* 0x1A4 */ be<f32> mTriggerRightDelta;
    /* 0x1A8 */ be<u8> mHoldLockR;
    /* 0x1A9 */ be<u8> mTrigLockR;
    /* 0x1AA */ be<u8> mLockRActive;  /* was m1A6: updatePad 024F9BF4: 1 while the R analog value is above ManualEndVal; TP name */
    /* 0x1AB */ be<u8> mLockRJustActivated;  /* was m1A7: updatePad 024F9BF4: m1A6 ^ 1 before m1A6 is set (first frame only); TP name */
    /* 0x1AC */ be<u8> mHoldX;
    /* 0x1AD */ be<u8> mTrigX;
    /* 0x1AE */ be<u8> mHoldY;
    /* 0x1AF */ be<u8> mTrigY;
    /* 0x1B0 */ be<u8> mHoldZ;
    /* 0x1B1 */ be<u8> mTrigZ;
    /* 0x1B2 */ be<u8> m1AE;
    /* 0x1B3 */ be<u8> m1AF;
    /* 0x1B4 */ dCamForcusLine mForcusLine;
    /* 0x224 */ DMC mDMCSystem;
    /* 0x22A */ u8 _22A[2];
    /* 0x22C */ Monitor mMonitor;
    /* 0x24C */ be<s32> mSeIds[3];  /* was m248: initialize 02500984 stores JA_SE_MAN_CAMERA_NG / JA_SE_CAMERA_TO_MANUAL / JA_SE_ATM_PRT_SHIP_CREAK; Run 024FE3E8 starts [i] when bit i of m254 is new */
    /* 0x258 */ be<s32> mSeRequest;  /* was m254: nextMode 024FA9C8 / onModeChange 024FA79C / Run (ship rock) set bits 1/2/4; Run 024FE3E8 starts mSeIds[i] for new bits, then clears it */
    /* 0x25C */ be<s32> mSeRequestLast;  /* was m258: Run 024FE3E8 keeps last frame's m254 here and plays only bits not set in it */
    /* 0x260 */ BG mBG;                     /* GameCube 0x25C */
    /* 0x318 */ be<u8> mInSeaArea;  /* was m314: checkGroundInfo 024F9044: 1 when daSea_ChkArea(player x, z), else 0 */
    /* 0x319 */ u8 _319[3];
    /* 0x31C */ be<f32> mSeaWaveHeight;  /* was m318: checkGroundInfo 024F9044: daSea_calcWave(player x, z), else -1e9; rideCamera 0250D4E8 compares it with m354 */
    /* 0x320 */ be<u8> mOnMoveBg;  /* was m31C: checkGroundInfo 024F9044: 1 while the ground under the player is a moving BG, 0 otherwise; Run 024FE3E8 tests it */
    /* 0x321 */ be<u8> mFollowMoveBg;  /* was m31D: checkGroundInfo 024F9044: 1 when the view is carried with the moving BG (no event camera, player on the floor) */
    /* 0x322 */ u8 _322[2];
    /* 0x324 */ cXyz mMoveBgPosDelta;  /* was m320: checkGroundInfo 024F9044: m32C - the moving BG actor's position (last minus current) */
    /* 0x330 */ cXyz mMoveBgLastPos;  /* was m32C: checkGroundInfo 024F9044: the moving BG actor's position, kept for the next frame */
    /* 0x33C */ cSAngle_l mMoveBgAngleDelta;  /* was m338: checkGroundInfo 024F9044: m33A - the moving BG actor's direction */
    /* 0x33E */ cSAngle_l mMoveBgLastAngle;  /* was m33A: checkGroundInfo 024F9044: the moving BG actor's direction, kept for the next frame */
    /* 0x340 */ gptr<fopAc_ac_c> mpMoveBgActor;  /* was m33C: checkGroundInfo 024F9044: GetActorPointer of the moving BG under the player, else NULL */
    /* 0x344 */ u8 m340[0x10];
    /* 0x354 */ be<s32> mCamMoveBg;  /* was m350: checkGroundInfo 024F9044: dBgS::GetCamMoveBG of the player's ground, else 0; nextType 024FA410 reads it */
    /* 0x358 */ be<f32> mGroundHeight;  /* was m354: checkGroundInfo 024F9044: ground cross height under the camera check point, raised to mSeaWaveHeight */
    /* 0x35C */ be<s32> mRoomNo;
    /* 0x360 */ be<s32> mRoomMapToolCameraIdx;
    /* 0x364 */ be<u8> mPlayerOnFloor;  /* was m360: checkGroundInfo 024F9044: 0 when the player's feet are more than FloorMargin above the ground, else 1 */
    /* 0x365 */ u8 _365[3];
    /* 0x368 */ be<s32> mBushHit;  /* was m364: checkGroundInfo 024F9044: lineCollisionCheckBush(...) & 5; bumpCheck 024FD11C tests it for CheckBush */
    /* 0x36C */ be<f32> mBushHeight;  /* was m368: checkGroundInfo 024F9044: mCamSetup mBushHigh / mBushLow by the bush hit bits, else 0; talk camera 02508A60 adds it to the eye limit */
    /* 0x370 */ cXyz mBushTopPos;  /* was m36C: checkGroundInfo 024F9044: GetMassCamTopPos output; bumpCheck 024FD11C uses it */
    /* 0x37C */ u8 mWork[0x80];          /* per-engine work union (GC 0x378); see the *_work_l overlays */
    /* 0x3FC */ EventData mEventData;
    /* 0x510 */ be<u32> mEventFlags;
    /* 0x514 */ be<s32> mCurStyle;
    /* 0x518 */ be<s32> m514;
    /* 0x51C */ be<s32> mCurType;
    /* 0x520 */ be<s32> mNextType;
    /* 0x524 */ be<s32> mMapToolType;
    /* 0x528 */ be<s32> mForceType;  /* was m524: SetType 02514EB8 stores the type when it is 0xFF; nextType 024FA410 uses and resets it to 0xFF */
    /* 0x52C */ gptr<fopAc_ac_c> mpForceTypeActor;  /* was m528: SetType 02514EB8 stores the actor with mForceType; nextType 024FA410 reads and clears it */
    /* 0x530 */ be<u8> m52C;
    /* 0x531 */ u8 _531[3];
    /* 0x534 */ be<s32> mShipRockType;  /* was m530: initialize 02500984: 1 Asoko/Abship/PShip, 2 Obshop, 3 A_umikz, else 0; Run 024FE3E8 rocks the view while it is not 0 */
    /* 0x538 */ be<s16> mShipRockAngle;  /* was m534: initialize 02500984 clears it; daObjPirateship::getShipOffsetY 025269A8 (d_com_static.cpp, parameter `angle`) adds mShipRockSpeed to it and returns scale * sin of it */
    /* 0x53A */ be<s16> mShipRockSpeed;  /* was m536: initialize 02500984 sets 0x180; daObjPirateship::getShipOffsetY 025269A8 (parameter `speed`) eases it toward 0x180 while |angle| <= 0x4000, else 0x280, and adds it to mShipRockAngle */
    /* 0x53C */ be<f32> mShipOffsetY;  /* was m538: initialize 02500984 / Run 024FE3E8: getShipOffsetY(...) * mShipOffsetScale, kept for the next frame */
    /* 0x540 */ be<f32> mShipOffsetYDelta;  /* was m53C: Run 024FE3E8: this frame's ship offset minus mShipOffsetY; ship creak sound when it turns upward */
    /* 0x544 */ be<f32> mShipOffsetScale;  /* was m540: initialize 02500984: 1.0 / 0.12 / 1.0 by mShipRockType, else 0; Run 024FE3E8 scales the ship offset by it */
    /* 0x548 */ u8 mShakePattern[4];  /* was m544: StartShake 02515298 copies the i_pattern bits (length mShakeLength) */
    /* 0x54C */ u8 mShakeCyclePattern[4];  /* was m548: StartShake 02515298 copies the pattern, the last byte wrapped with the first for length 0x20; shakeCamera 024FC108 reads it */
    /* 0x550 */ u8 m54C[4];
    /* 0x554 */ be<s32> mShakeLength;  /* was m550: StartShake 02515298 stores i_length (max 0x20); StopShake 025153D4 clears it; shakeCamera 024FC108 shakes while the frame is below it */
    /* 0x558 */ be<s32> mShakeFrame;  /* was m554: StartShake 02515298 / StopShake clear it; shakeCamera 024FC108 reads it as the pattern position and adds 1 */
    /* 0x55C */ u8 m558[4];
    /* 0x560 */ cXyz mShakeDirection;  /* was m55C: StartShake 02515298 stores the normalised i_pos; shakeCamera 024FC108 scales it */
    /* 0x56C */ cXyz mCenterShake;
    /* 0x578 */ cXyz mEyeShake;
    /* 0x584 */ be<f32> mFovYShake;
    /* 0x588 */ cSAngle_l mBankShake;
    /* 0x58A */ u8 _58A[2];
    /* 0x58C */ be<s32> mShakeFlags;  /* was m588: StartShake 02515298 stores i_flags, StopShake clears it; shakeCamera 024FC108 tests its bits */
    /* 0x590 */ be<s32> mBlureType;  /* was m58C: ResetBlure 0250D464 stores its argument; the blur part of shakeCamera 024FC108 branches on 0/1 */
    /* 0x594 */ be<s32> mBlureTimer;
    /* 0x598 */ csXyz mBlureRotation;
    /* 0x59E */ be<s16> m59A;
    /* 0x5A0 */ be<s32> mBlurePositionType;
    /* 0x5A4 */ cXyz mBlurePosition;
    /* 0x5B0 */ cXyz mBlureScale;
    /* 0x5BC */ be<f32> mBlureAlpha;
    /* 0x5C0 */ u8 _5C0[4];
    /* 0x5C4 */ u8 mCurRoomCamEntry[0x14];   /* stage_camera2_data_class */
    /* 0x5D8 */ u8 mCurRoomArrowEntry[0x14]; /* stage_arrow_data_class */
    /* 0x5EC */ be<s32> mCurArrowIdx;
    /* 0x5F0 */ be<f32> mWindowWidth;
    /* 0x5F4 */ be<f32> mWindowHeight;
    /* 0x5F8 */ be<f32> m5F4;
    /* 0x5FC */ be<f32> mTrimHeight;
    /* 0x600 */ be<s32> mTrimSize;
    /* 0x604 */ be<s32> mTrimTypeForce;
    /* 0x608 */ be<f32> mWindowAspectRatio;
    /* 0x60C */ be<f32> mWallUpDist;  /* was m608: initialize 02500984: mCamSetup.mBGChk.WallUpDistance(); TP name */
    /* 0x610 */ be<u8> mHD610;                /* HD-only */
    /* 0x611 */ u8 _611[3];
    /* 0x614 */ u8 mHDLookAtCamera[0x58];     /* HD-only sead::LookAtCamera (vtable at +0x30) */
    /* 0x66C */ u8 mHDProjection[0xC4];       /* HD-only sead projection */
    /* 0x730 */ u8 mHD730[8];
    /* 0x738 */ be<u8> mHD738;
    /* 0x739 */ u8 _739[3];
    /* 0x73C */ u8 mCamSetup[0x168];          /* dCamSetup_c (GC 0x60C, 0x144) */
    /* 0x8A4 */ u8 mCamParam[0xC];            /* dCamParam_c (GC 0x750) */
    /* 0x8B0 */ be<s32> mCamTypeField;
    /* 0x8B4 */ be<s32> mCamTypeEvent;
    /* 0x8B8 */ be<s32> mCamTypeWater;
    /* 0x8BC */ be<s32> m768;
    /* 0x8C0 */ be<s32> mCamTypeBoat;
    /* 0x8C4 */ be<s32> mCamTypeBoatBattle;
    /* 0x8C8 */ be<s32> mCamTypeSubject;
    /* 0x8CC */ be<s32> mCamTypeKeep;
    /* 0x8D0 */ be<s32> mCamTypeRestrict;
    /* 0x8D4 */ be<u8> mIsSeaMap;  /* was m780: initialize 02500984: is_current_map("sea"); followCamera 025028B8 reads it */
    /* 0x8D5 */ be<u8> mIsShipRoomMap;  /* was m781: initialize 02500984: is_current_map("Asoko"/"Abship"/"PShip"); selects mShipRockType 1 */
    /* 0x8D6 */ be<u8> mIsObshopMap;  /* was m782: initialize 02500984: is_current_map("Obshop"); selects mShipRockType 2 */
    /* 0x8D7 */ be<u8> mIsUmikzMap;  /* was m783: initialize 02500984: is_current_map("A_umikz"); selects mShipRockType 3 */
    /* 0x8D8 */ be<u8> mIsGanonKMap;  /* was m784: initialize 02500984: is_current_map("GanonK"); freeCamera 0250FDC8 tests it */
    /* 0x8D9 */ be<u8> mIsGTowerMap;  /* was m785: initialize 02500984: is_current_map("GTower") */
    /* 0x8DA */ be<u8> mInSeaCenter;  /* was m786: checkSpecialArea 024F8DB4: 1 when the player is within sSeaRadius of sSeaCenter on the sea map */
    /* 0x8DB */ be<u8> mInKazeCenter;  /* was m787: checkSpecialArea 024F8DB4: 1 when the player is within sKazeRadius of sKazeCenter on the kaze map; followCamera 025028B8 tests it */
    /* 0x8DC */ be<u8> mIsKazeMap;  /* was m788: initialize 02500984: is_current_map("kaze") */
    /* 0x8DD */ be<u8> mIsMDaiMap;  /* was m789: initialize 02500984: is_current_map("M_Dai") */
    /* 0x8DE */ be<u8> m78A;
    /* 0x8DF */ be<u8> mIsKazeBMap;  /* was m78B: initialize 02500984: is_current_map("kazeB"); bumpCheck 024FD11C tests it */

    u32 ea() const { return gabi::ea(this); }
    bool chkFlag(u32 flag) { return (mEventFlags & flag) ? true : false; }

    /* functions (WWHD addresses in d_camera*.cpp) */
    bool Reset();
    void Start();
    void Stop();
    void Stay();
    u8 Active();
    u8 Pause();
    bool SetTrimSize(s32 size);
    bool SetTrimTypeForce(s32 force);
    bool StopShake();
    void SetBlureAlpha(f32 alpha);
    void SetBlurePosition(f32 x, f32 y, f32 z);
    void SetBlurePositionType(s32 t);
    void SetBlureTimer(s32 t);
    bool SubjectLockOn(fopAc_ac_c* target);
    bool SubjectLockOff();
    fopAc_ac_c* GetForceLockOnActor();
    bool ForceLockOn(u32 id);
    bool ForceLockOff(u32 id);
    bool SetExtendedPosition(cXyz* pos);
    bool ScopeViewMsgModeOff();
    s16 U2();
    cXyz* positionOf(cXyz* ret, fopAc_ac_c* actor);
    cXyz* attentionPos(cXyz* ret, fopAc_ac_c* actor);
    cXyz* eyePos(cXyz* ret, fopAc_ac_c* actor);
    cSAngle_l* directionOf(cSAngle_l* ret, fopAc_ac_c* actor);
    f32 footHeightOf(fopAc_ac_c* actor);
    cXyz* positionPntOf(fopAc_ac_c* actor);
    cSAngle_l* getDMCAngle(cSAngle_l* ret, cSAngle_l* unused);
    bool demoCamera(s32);
    bool letCamera(s32);
    bool followCamera2(s32 p);
    bool followCamera(s32 p);
    f32 radiusActorInSight(fopAc_ac_c* a1, fopAc_ac_c* a2);
    f32 radiusActorInSight6(fopAc_ac_c* a1, fopAc_ac_c* a2, cXyz* center, cXyz* eye, f32 fovY, s16 bank); /* overload */
    s32 GetCameraTypeFromCameraName(const char* name);
    bool onTypeChange(s32 cur, s32 next);
    bool Chtyp(s32 next);
    void setView(f32 x, f32 y, f32 w, f32 h);
    void ResetBlure(s32 p);
    f32 heightOf(fopAc_ac_c* actor);
    void Att();
    s32 GetCameraTypeFromMapToolID(s32 id, s32 roomNo);
    void pushPos();
    bool onModeChange(s32 cur, s32 next);
    void setDMCAngle();
    bool onStyleChange(s32 style1, s32 style2);
    bool ChangeModeOK(s32 mode);
    void updateMonitor();
    void updatePad();
    bool checkForceLockTarget();
    bool Draw();
    void initMonitor();
    void initPad();
    u32 lineCollisionCheckBush(cXyz* start, cXyz* end);
    bool lineBGCheck(cXyz* start, cXyz* end, u8* linChk, u32 flags);
    bool lineBGCheckBoth(cXyz* start, cXyz* end, u8* linChk, u32 flags);
    s32 defaultTriming();
    void compWallMargin(cXyz* ret, cXyz* center, f32 radius);
    void relationalPos(cXyz* ret, fopAc_ac_c* actor, cXyz* offset);
    bool pointInSight(cXyz* point);
    void checkSpecialArea();
    void CalcTrimSize();
    bool NotRun();
    u32 Run();
    f32 groundHeight(cXyz* pos);
    cSAngle_l* calcPeepAngle(cSAngle_l* ret);
    void initialize(camera_class* camera, fopAc_ac_c* player, u32 infoIdx, u32 padId);
    f32 shakeCamera();
    void forwardCheckAngle(cSAngle_l* ret);
    bool eventCamera(s32);
    bool bumpCheck(u32 flags);
    bool nonOwnerCamera(s32 style);
    bool fixedFrameCamera(s32 style);
    bool fixedPositionCamera(s32 style);
    bool crawlCamera(s32 style);
    bool hookshotCamera(s32 style);
    bool lockonCamera(s32 style);
    bool subjectCamera(s32 style);
    bool talktoCamera(s32 style);
    bool towerCamera(s32 style);
    bool rideCamera(s32 style);
    bool hungCamera(s32 style);
    bool manualCamera(s32 style);
    bool tornadoCamera(s32 style);
    bool vomitCamera(s32 style);
    bool shieldCamera(s32 style);
    bool CalcSubjectAngle(s16* outV, s16* outU);
    f32 getWaterSurfaceHeight(cXyz* pos);
    s32 nextType(s32 cur);
    s32 nextMode(s32 cur);
    void checkGroundInfo();

    /* inlines */
    u16 camStyleFlags() { return gabi::load<u16>(gabi::load<u32>(ea() + 0x8A8) + 0x80); } /* mCamParam.CheckFlag */
    void clrComStat(u32 flag) {
        s32 id = mCameraID;
        u32 p = dComIfGp_ea() + id * 0x34 + 0x5B00;
        gabi::store<u32>(p, gabi::load<u32>(p) & ~flag);
    }
    void setComStat(u32 flag) {
        s32 id = mCameraID;
        u32 p = dComIfGp_ea() + id * 0x34 + 0x5B00;
        gabi::store<u32>(p, gabi::load<u32>(p) | flag);
    }
    bool getComStat(u32 flag) {
        s32 id = mCameraID;
        return (gabi::load<u32>(dComIfGp_ea() + id * 0x34 + 0x5B00) & flag) != 0;
    }
    void setComZoomScale(f32 v) {
        s32 id = mCameraID;
        gabi::store<f32>(dComIfGp_ea() + id * 0x34 + 0x5B04, v);
    }
};
WWHD_SIZE(dCamera_c, 0x8E0);
WWHD_OFFSET(dCamera_c, mViewCache, 0x3C);
WWHD_OFFSET(dCamera_c, mSavedViewStack, 0xA4);
WWHD_OFFSET(dCamera_c, mCameraID, 0x120);
WWHD_OFFSET(dCamera_c, mStickMainPosXLast, 0x158);
WWHD_OFFSET(dCamera_c, mForcusLine, 0x1B4);
WWHD_OFFSET(dCamera_c, mMonitor, 0x22C);
WWHD_OFFSET(dCamera_c, mBG, 0x260);
WWHD_OFFSET(dCamera_c, mSeaWaveHeight, 0x31C);
WWHD_OFFSET(dCamera_c, mWork, 0x37C);
WWHD_OFFSET(dCamera_c, mEventData, 0x3FC);
WWHD_OFFSET(dCamera_c, mEventData.mSpline2DPath, 0x4CC);
WWHD_OFFSET(dCamera_c, mEventFlags, 0x510);
WWHD_OFFSET(dCamera_c, mpForceTypeActor, 0x52C);
WWHD_OFFSET(dCamera_c, mCenterShake, 0x56C);
WWHD_OFFSET(dCamera_c, mBlureScale, 0x5B0);
WWHD_OFFSET(dCamera_c, mTrimSize, 0x600);
WWHD_OFFSET(dCamera_c, mCamSetup, 0x73C);
WWHD_OFFSET(dCamera_c, mCamParam, 0x8A4);
WWHD_OFFSET(dCamera_c, mCamTypeField, 0x8B0);
WWHD_OFFSET(dCamera_c, mIsSeaMap, 0x8D4);

/* dCamera_c::types[] (0x40 each: name[24], mStyles[20]) and type_num */
enum : u32 { CAM_TYPES = 0x10049350, CAM_TYPE_NUM = 0x10049348, CAM_STYLES = 0x1004487C };
inline s16 camTypeStyle(s32 type, s32 i) { return gabi::load<s16>(CAM_TYPES + type * 0x40 + 0x18 + i * 2); }
/* dCamParam_c style table (0x84 each): algorithm at +4 (mCamParam.Algorythmn(style)) */
inline u32 camStyleAlg(s32 style) { return gabi::load<u32>(CAM_STYLES + style * 0x84 + 4); }
/* mCamParam.Val(style, i): the style's parameters (f32[]) at +8 */
inline f32 camParamVal(s32 style, s32 i) { return gabi::load<f32>(CAM_STYLES + style * 0x84 + 8 + i * 4); }
/* pad accessors (HD: out of line, by pad id) */
inline s16 CPad_GET_STICK_ANGLE(s32 pad) { return gabi::call<s16>(0x02007A1C, pad); }

/* camera_process_class: the dCamera_c body is at +0x248 (dCam_getBody) */
enum { CAMERA_BODY = 0x248 };

/* ---- c_angle / c_xyz out-of-line helpers (HD) ---- */
inline void cSAngle_ct(cSAngle_l* a) { gabi::call(0x020065FC, a); }
inline void cSAngle_ct(cSAngle_l* a, s16 v) { gabi::call(0x0200658C, a, v); }
inline void cSAngle_ct(cSAngle_l* a, const cSAngle_l* b) { gabi::call(0x02006644, a, b); }
inline void cSAngle_Val(cSAngle_l* a, s16 v) { gabi::call(0x02006584, a, v); }
inline s16 cSAngle_Inv(const cSAngle_l* a) { return gabi::call<s16>(0x02006804, a); }
inline void cSGlobe_Val(cSGlobe_l* g, const cXyz* v) { gabi::call(0x020072E0, g, v); }

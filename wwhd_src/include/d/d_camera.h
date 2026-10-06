/**
 * d_camera.h (WWHD) - follow camera dCamera_c, WWHD layout.
 *
 * Layout measured from the WWHD code (constructor
 * 02501B8C allocates 0x8E0; GameCube 0x78C). GameCube names are kept; "GC" gives the GameCube
 * offset where it differs.
 *  - 0x000..0x14B: unchanged.
 *  - 0x14C..0x157: HD has three floats where GameCube has m14C + 4 unknown bytes; from 0x158
 *    (GC 0x154) everything is shifted by +4 up to mWindowAspectRatio/m608 (HD 0x608/0x60C).
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
        /* 0x04 */ u8 m04[0x54];   /* dBgS_CamGndChk */
        /* 0x58 */ be<f32> m58;
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
    /* 0x068 */ be<s32> m068;
    /* 0x06C */ cSAngle_l mAngleY;
    /* 0x06E */ u8 _06E[2];
    /* 0x070 */ cXyz m070;
    /* 0x07C */ be<u32> m07C;
    /* 0x080 */ be<u32> m080;
    /* 0x084 */ cXyz m084;
    /* 0x090 */ cXyz m090;
    /* 0x09C */ be<f32> m09C;
    /* 0x0A0 */ cSAngle_l m0A0;
    /* 0x0A2 */ u8 _0A2[2];
    /* 0x0A4 */ PosSet m0A4[2];
    /* 0x0E4 */ be<s32> mStageMapToolCameraIdx;
    /* 0x0E8 */ be<s32> m0E8;
    /* 0x0EC */ cXyz mExtendedPos;
    /* 0x0F8 */ u8 _0F8[8];
    /* 0x100 */ be<u8> m100;
    /* 0x101 */ be<u8> m101;
    /* 0x102 */ be<u8> m102;
    /* 0x103 */ u8 _103;
    /* 0x104 */ cSAngle_l m104;
    /* 0x106 */ cSAngle_l m106;
    /* 0x108 */ be<u32> m108;
    /* 0x10C */ be<s32> m10C;
    /* 0x110 */ be<u8> m110;
    /* 0x111 */ u8 _111[3];
    /* 0x114 */ be<s32> m114;
    /* 0x118 */ be<u32> m118;
    /* 0x11C */ be<u32> m11C;
    /* 0x120 */ be<s32> mCameraID;
    /* 0x124 */ be<s32> mPadId;
    /* 0x128 */ gptr<fopAc_ac_c> mpPlayerActor;
    /* 0x12C */ gptr<fopAc_ac_c> mpLockonTarget;
    /* 0x130 */ be<u32> mLockOnActorId;
    /* 0x134 */ gptr<fopAc_ac_c> mpLockonActor;
    /* 0x138 */ be<s32> mForceLockTimer;
    /* 0x13C */ be<s32> mCurMode;
    /* 0x140 */ be<s32> mNextMode;
    /* 0x144 */ be<s32> m144;
    /* 0x148 */ cSAngle_l m148;
    /* 0x14A */ u8 _14A[2];
    /* 0x14C */ be<f32> m14C;
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
    /* 0x188 */ be<s32> m184;
    /* 0x18C */ be<u32> m188;
    /* 0x190 */ be<u32> m18C;
    /* 0x194 */ be<f32> mTriggerLeftLast;
    /* 0x198 */ be<f32> mTriggerLeftDelta;
    /* 0x19C */ be<u8> mHoldLockL;
    /* 0x19D */ be<u8> mTrigLockL;
    /* 0x19E */ be<u8> m19A;
    /* 0x19F */ be<u8> m19B;
    /* 0x1A0 */ be<f32> mTriggerRightLast;
    /* 0x1A4 */ be<f32> mTriggerRightDelta;
    /* 0x1A8 */ be<u8> mHoldLockR;
    /* 0x1A9 */ be<u8> mTrigLockR;
    /* 0x1AA */ be<u8> m1A6;
    /* 0x1AB */ be<u8> m1A7;
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
    /* 0x24C */ be<s32> m248[3];
    /* 0x258 */ be<s32> m254;
    /* 0x25C */ be<s32> m258;
    /* 0x260 */ BGChk mBG_m00;
    /* 0x2BC */ BGChk mBG_m5C;
    /* 0x318 */ be<u8> m314;
    /* 0x319 */ u8 _319[3];
    /* 0x31C */ be<f32> m318;
    /* 0x320 */ be<u8> m31C;
    /* 0x321 */ be<u8> m31D;
    /* 0x322 */ u8 _322[2];
    /* 0x324 */ cXyz m320;
    /* 0x330 */ cXyz m32C;
    /* 0x33C */ cSAngle_l m338;
    /* 0x33E */ cSAngle_l m33A;
    /* 0x340 */ gptr<fopAc_ac_c> m33C;
    /* 0x344 */ u8 m340[0x10];
    /* 0x354 */ be<s32> m350;
    /* 0x358 */ be<f32> m354;
    /* 0x35C */ be<s32> mRoomNo;
    /* 0x360 */ be<s32> mRoomMapToolCameraIdx;
    /* 0x364 */ be<u8> m360;
    /* 0x365 */ u8 _365[3];
    /* 0x368 */ be<s32> m364;
    /* 0x36C */ be<f32> m368;
    /* 0x370 */ cXyz m36C;
    /* 0x37C */ u8 mWork[0x80];          /* per-engine work union (GC 0x378); see the *_work_l overlays */
    /* 0x3FC */ EventData mEventData;
    /* 0x510 */ be<u32> mEventFlags;
    /* 0x514 */ be<s32> mCurStyle;
    /* 0x518 */ be<s32> m514;
    /* 0x51C */ be<s32> mCurType;
    /* 0x520 */ be<s32> mNextType;
    /* 0x524 */ be<s32> mMapToolType;
    /* 0x528 */ be<s32> m524;
    /* 0x52C */ gptr<fopAc_ac_c> m528;
    /* 0x530 */ be<u8> m52C;
    /* 0x531 */ u8 _531[3];
    /* 0x534 */ be<s32> m530;
    /* 0x538 */ be<s16> m534;
    /* 0x53A */ be<s16> m536;
    /* 0x53C */ be<f32> m538;
    /* 0x540 */ be<f32> m53C;
    /* 0x544 */ be<f32> m540;
    /* 0x548 */ u8 m544[4];
    /* 0x54C */ u8 m548[4];
    /* 0x550 */ u8 m54C[4];
    /* 0x554 */ be<s32> m550;
    /* 0x558 */ be<s32> m554;
    /* 0x55C */ u8 m558[4];
    /* 0x560 */ cXyz m55C;
    /* 0x56C */ cXyz mCenterShake;
    /* 0x578 */ cXyz mEyeShake;
    /* 0x584 */ be<f32> mFovYShake;
    /* 0x588 */ cSAngle_l mBankShake;
    /* 0x58A */ u8 _58A[2];
    /* 0x58C */ be<s32> m588;
    /* 0x590 */ be<s32> m58C;
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
    /* 0x60C */ be<f32> m608;
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
    /* 0x8D4 */ be<u8> m780, m781, m782, m783, m784, m785, m786, m787, m788, m789, m78A, m78B;

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
WWHD_OFFSET(dCamera_c, m0A4, 0xA4);
WWHD_OFFSET(dCamera_c, mCameraID, 0x120);
WWHD_OFFSET(dCamera_c, mStickMainPosXLast, 0x158);
WWHD_OFFSET(dCamera_c, mForcusLine, 0x1B4);
WWHD_OFFSET(dCamera_c, mMonitor, 0x22C);
WWHD_OFFSET(dCamera_c, mBG_m00, 0x260);
WWHD_OFFSET(dCamera_c, m318, 0x31C);
WWHD_OFFSET(dCamera_c, mWork, 0x37C);
WWHD_OFFSET(dCamera_c, mEventData, 0x3FC);
WWHD_OFFSET(dCamera_c, mEventData.mSpline2DPath, 0x4CC);
WWHD_OFFSET(dCamera_c, mEventFlags, 0x510);
WWHD_OFFSET(dCamera_c, m528, 0x52C);
WWHD_OFFSET(dCamera_c, mCenterShake, 0x56C);
WWHD_OFFSET(dCamera_c, mBlureScale, 0x5B0);
WWHD_OFFSET(dCamera_c, mTrimSize, 0x600);
WWHD_OFFSET(dCamera_c, mCamSetup, 0x73C);
WWHD_OFFSET(dCamera_c, mCamParam, 0x8A4);
WWHD_OFFSET(dCamera_c, mCamTypeField, 0x8B0);
WWHD_OFFSET(dCamera_c, m780, 0x8D4);

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

/* Scheduled quake/sound actor. HD actor fields move by 0x11C; HIO vtable is last. */
#pragma once
#include "gc_api/gc_api.h"
#include "gc_api/SSystem/c_phase.h"
struct daObjQuake_c : fopAc_ac_c {
    enum Prm_e { PRM_SCH_W = 8, PRM_SCH_S = 0, PRM_TYPE_W = 3, PRM_TYPE_S = 8,
                 PRM_POWER_W = 3, PRM_POWER_S = 11 };
    enum Type { TYPE_QUAKE = 0, TYPE_SOUND = 1, TYPE_CAMERA = 2 };
    enum : u32 { VTABLE = 0x1002EBDC };
    /* 0x3AC */ be<f32> mDuration;
    /* 0x3B0 */ be<f32> mTimer;
    /* 0x3B4 */ be<f32> mStart;
    /* 0x3B8 */ be<f32> mEnd;
    /* 0x3BC */ be<u8> mType;
    /* 0x3BD */ be<u8> mActive;
    u8 mPadding[2];
    u8 getPrmType();
    s32 getPrmPower();
    u8 getPrmSch();
    cPhs_State _create();
    BOOL _execute();
    BOOL _is_delete();
};
WWHD_OFFSET(daObjQuake_c, mDuration, 0x3AC);
WWHD_OFFSET(daObjQuake_c, mTimer, 0x3B0);
WWHD_OFFSET(daObjQuake_c, mStart, 0x3B4);
WWHD_OFFSET(daObjQuake_c, mEnd, 0x3B8);
WWHD_OFFSET(daObjQuake_c, mType, 0x3BC);
WWHD_OFFSET(daObjQuake_c, mActive, 0x3BD);
WWHD_SIZE(daObjQuake_c, 0x3C0);
struct daObjQuake_HIO_c {
    /* 0x00 */ be<s8> mNo;
    u8 mPadding[3];
    /* 0x04 */ be<u32> mCount;
    /* 0x08 */ be<u8> mCameraOverride;
    /* 0x09 */ be<u8> mMotorOverride;
    /* 0x0A */ be<u8> mCameraPower;
    /* 0x0B */ be<u8> mMotorPower;
    /* 0x0C */ be<u8> mCamera;
    /* 0x0D */ be<u8> mSound;
    /* 0x0E */ be<u8> mMotor;
    /* 0x0F */ be<u8> mExtra;
    /* 0x10 */ be<u32> mVtable;
    enum : u32 { VTABLE = 0x1002EBCC };
};
WWHD_OFFSET(daObjQuake_HIO_c, mCount, 4);
WWHD_OFFSET(daObjQuake_HIO_c, mCameraOverride, 8);
WWHD_OFFSET(daObjQuake_HIO_c, mExtra, 0xF);
WWHD_OFFSET(daObjQuake_HIO_c, mVtable, 0x10);
WWHD_SIZE(daObjQuake_HIO_c, 0x14);
struct QuakeStageInfo { u8 mStageInfo[0xF]; be<u8> mSchSec; };
WWHD_OFFSET(QuakeStageInfo, mSchSec, 0xF);
WWHD_SIZE(QuakeStageInfo, 0x10);
s32 dKy_get_schbit_timer();
u32 dKy_get_schbit();
void dVibration_StopQuake(dVibration_c*, u32);
void dVibration_StartQuake(dVibration_c*, s32, u32, cXyz*);
void mDoAud_seStartQuake(u32);
void fopDwTg_DrawQTo(void*);
enum QuakeFlag : u32 {
    QuakeFlag_CAMERA_ONLY = 1, QuakeFlag_CAMERA = 2, QuakeFlag_SOUND = 4,
    QuakeFlag_MOTOR = 8, QuakeFlag_EXTRA = 0x10, QuakeFlag_BASE = 0x20, QuakeFlag_ALL = 0x3E,
};
enum { dSymbol_DIN_e = 1, JA_SE_ATM_JINARI = 0x105E, dEvtFlag_TALK_e = 4 };
struct QuakeHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 mPadding[2];
    be<u32> zero[4];
};
WWHD_OFFSET(QuakeHeaderStatics, zero, 0xC);
WWHD_SIZE(QuakeHeaderStatics, 0x1C);
void QuakeFirstHeaderObject_ct(u8*);
void QuakeSecondHeaderObject_ct(u8*);

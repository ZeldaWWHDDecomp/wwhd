/* Ganondorf execute-only HD views. The GameCube class leaves these fields unknown.
 * Kept separate from the shared Ganondorf header used by the other split sources. */
#pragma once
#include "d/actor/d_a_gnd.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/tu.h"

struct gndExecFields {
    u8 _pad000[0x3D4];
    /* 0x3D4 */ be<u8> mBgmParam;
    u8 _pad3D5[0x13];
    /* 0x3E8 */ be<s16> mCounter;
    /* 0x3EA */ be<s16> mAction;
    /* 0x3EC */ be<s16> mMode;
    u8 _pad3EE[0x30];
    /* 0x41E */ be<s16> mTimers[5];
    /* 0x428 */ be<s16> mDamageTimer;
    /* 0x42A */ be<s16> mHitTimer;
    u8 _pad42C[0x8];
    /* 0x434 */ be<s16> mAttackTimer;
    u8 _pad436[0x6];
    /* 0x43C */ dKy_tevstr_c mBattleTevStr;
    u8 _pad604[0x7];
    /* 0x60B */ be<s8> mInvulnerabilityTimer;
    /* 0x60C */ be<s8> mBattlePhase;
    u8 _pad60D[0x7];
    /* 0x614 */ be<f32> mModelYawOffset;
    u8 _pad618[0x10E8];
    /* 0x1700 */ cXyz mSwordPos[2];
    /* 0x1718 */ be<s8> mSwordAttack[2];
    u8 _pad171A[0x2];
    /* 0x171C */ be<u32> mSwordAttackType[2];
    /* 0x1724 */ be<f32> mSwordRadius[2];
    /* 0x172C */ be<u8> mSwordActive[2];
    u8 _pad172E[0x1];
    /* 0x172F */ be<u8> mGuardAttackType;
    /* 0x1730 */ be<s8> mGuardTimer;
    /* 0x1731 */ be<s8> mGuardTimerAlt;
    u8 _pad1732[0x2];
    /* 0x1734 */ be<u32> mAttackEmitters[6];
    u8 _pad174C[0x4];
    /* 0x1750 */ be<u32> mFinishEmitters[2];
    u8 _pad1758[0x34];
    /* 0x178C */ be<s16> mEffectTimer;
    u8 _pad178E[0x12E];
    /* 0x18BC */ be<s16> mDemoWaitTimer;
    /* 0x18BE */ be<s16> mCameraCut;
    /* 0x18C0 */ be<s16> mCameraTimer;
    /* 0x18C2 */ be<s8> mCameraWaypoint;
    u8 _pad18C3[0x1];
    /* 0x18C4 */ cXyz mCameraEye;
    /* 0x18D0 */ cXyz mCameraCenter;
    /* 0x18DC */ cXyz mCameraEyeStep;
    /* 0x18E8 */ cXyz mCameraCenterStep;
    u8 _pad18F4[0xC];
    /* 0x1900 */ be<f32> mCameraBlend;
    /* 0x1904 */ be<f32> mCameraFovy;
    /* 0x1908 */ be<f32> mCameraFovyStep;
    /* 0x190C */ be<f32> mCameraShake;
    /* 0x1910 */ be<u8> mDemoActive;
    u8 _pad1911[0x7];
    /* 0x1918 */ be<u32> mRippleEmitter;
    u8 _pad191C[0x8];
    /* 0x1924 */ be<f32> mRippleHeight;
    /* 0x1928 */ cXyz mSwordJointPos[2];
    /* 0x1940 */ cXyz mHeadPos;
};
WWHD_OFFSET(gndExecFields, mCounter, 0x3E8);
WWHD_OFFSET(gndExecFields, mAction, 0x3EA);
WWHD_OFFSET(gndExecFields, mMode, 0x3EC);
WWHD_OFFSET(gndExecFields, mTimers, 0x41E);
WWHD_OFFSET(gndExecFields, mDamageTimer, 0x428);
WWHD_OFFSET(gndExecFields, mHitTimer, 0x42A);
WWHD_OFFSET(gndExecFields, mAttackTimer, 0x434);
WWHD_OFFSET(gndExecFields, mBattleTevStr, 0x43C);
WWHD_OFFSET(gndExecFields, mInvulnerabilityTimer, 0x60B);
WWHD_OFFSET(gndExecFields, mBattlePhase, 0x60C);
WWHD_OFFSET(gndExecFields, mModelYawOffset, 0x614);
WWHD_OFFSET(gndExecFields, mSwordPos, 0x1700);
WWHD_OFFSET(gndExecFields, mSwordAttack, 0x1718);
WWHD_OFFSET(gndExecFields, mSwordAttackType, 0x171C);
WWHD_OFFSET(gndExecFields, mSwordRadius, 0x1724);
WWHD_OFFSET(gndExecFields, mSwordActive, 0x172C);
WWHD_OFFSET(gndExecFields, mGuardAttackType, 0x172F);
WWHD_OFFSET(gndExecFields, mGuardTimer, 0x1730);
WWHD_OFFSET(gndExecFields, mGuardTimerAlt, 0x1731);
WWHD_OFFSET(gndExecFields, mAttackEmitters, 0x1734);
WWHD_OFFSET(gndExecFields, mFinishEmitters, 0x1750);
WWHD_OFFSET(gndExecFields, mEffectTimer, 0x178C);
WWHD_OFFSET(gndExecFields, mDemoWaitTimer, 0x18BC);
WWHD_OFFSET(gndExecFields, mCameraCut, 0x18BE);
WWHD_OFFSET(gndExecFields, mCameraTimer, 0x18C0);
WWHD_OFFSET(gndExecFields, mCameraWaypoint, 0x18C2);
WWHD_OFFSET(gndExecFields, mCameraEye, 0x18C4);
WWHD_OFFSET(gndExecFields, mCameraCenter, 0x18D0);
WWHD_OFFSET(gndExecFields, mCameraEyeStep, 0x18DC);
WWHD_OFFSET(gndExecFields, mCameraCenterStep, 0x18E8);
WWHD_OFFSET(gndExecFields, mCameraBlend, 0x1900);
WWHD_OFFSET(gndExecFields, mCameraFovy, 0x1904);
WWHD_OFFSET(gndExecFields, mCameraFovyStep, 0x1908);
WWHD_OFFSET(gndExecFields, mCameraShake, 0x190C);
WWHD_OFFSET(gndExecFields, mDemoActive, 0x1910);
WWHD_OFFSET(gndExecFields, mRippleEmitter, 0x1918);
WWHD_OFFSET(gndExecFields, mRippleHeight, 0x1924);
WWHD_OFFSET(gndExecFields, mSwordJointPos, 0x1928);
WWHD_OFFSET(gndExecFields, mHeadPos, 0x1940);
WWHD_SIZE(gndExecFields, 0x194C);

inline gndExecFields* execFields(gnd_class* actor) {
    return gabi::at<gndExecFields>(gabi::ea(actor));
}

// HD: bindings used by the execute fragment.
void* gnd_dDemo_getActor(u32 obj, u8 id);
void* gnd_dDemo_getP_BtpData(void* ac, const char* arc);
void* gnd_dDemo_getP_BtkData(void* ac, const char* arc);
void* gnd_dDemo_getP_BrkData(void* ac, const char* arc);
BOOL gnd_dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n,
                           void* ids, u32 p6, s8 p7);
s32 gnd_GetAttributeCode(dBgS* bgs, void* poly);
void gnd_JPASetRMtxTVecfromMtx(Mtx34* m, u32 r, u32 t);
BOOL gnd_orderPotentialEvent(fopAc_ac_c* a, u16 type, u16 flag, u16 p);
void gnd_cam_Stop(u32 cam);
void gnd_cam_Start(u32 cam);
void gnd_cam_SetTrimSize(u32 cam, s32 s);
void gnd_cam_Set(u32 cam, cXyz* center, cXyz* eye, s16 bank, f32 fovy);
void gnd_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 lastSpeed,
                      u32 lastMode, s32 setPoint, s8 wipe);
void gnd_bgmStop(u32 frames);
void gnd_changeBgmStatus(s32 st);
void gnd_fadeOut(s8 rate);
void gnd_setFadeColor(u32 color);
void gnd_play5C22_on();
struct gndExecHIO {
    u8 _pad000[0x1];
    /* 0x001 */ be<u8> mFreeze;
    u8 _pad002[0x6];
    /* 0x008 */ be<f32> mScale;
    /* 0x00C */ be<u8> mPhaseOverride;
    u8 _pad00D[0x34];
    /* 0x041 */ be<u8> mSwordShield;
    u8 _pad042[0x22];
    /* 0x064 */ be<s16> mAttackWait;
    u8 _pad066[0x18];
    /* 0x07E */ be<s16> mSwordAttackType;
    /* 0x080 */ be<s16> mSpinAttackType;
    u8 _pad082[0x2];
    /* 0x084 */ be<u8> mLongIntro;
    /* 0x085 */ be<u8> mStartZeldaDemo;
    u8 _pad086[0x2];
};
WWHD_OFFSET(gndExecHIO, mFreeze, 0x1);
WWHD_OFFSET(gndExecHIO, mScale, 0x8);
WWHD_OFFSET(gndExecHIO, mPhaseOverride, 0xC);
WWHD_OFFSET(gndExecHIO, mSwordShield, 0x41);
WWHD_OFFSET(gndExecHIO, mAttackWait, 0x64);
WWHD_OFFSET(gndExecHIO, mSwordAttackType, 0x7E);
WWHD_OFFSET(gndExecHIO, mSpinAttackType, 0x80);
WWHD_OFFSET(gndExecHIO, mLongIntro, 0x84);
WWHD_OFFSET(gndExecHIO, mStartZeldaDemo, 0x85);
WWHD_SIZE(gndExecHIO, 0x88);
struct gndExecPlayer {
    u8 _pad000[0x3AC];
    /* 0x3AC */ be<u8> mAction;
    u8 _pad3AD[0x73];
    /* 0x420 */ be<u16> mDemoMode;
    u8 _pad422[0x6];
    /* 0x428 */ be<u32> mDemoTimer;
    u8 _pad42C[0x4];
    /* 0x430 */ be<u32> mDemoAction;
};
WWHD_OFFSET(gndExecPlayer, mAction, 0x3AC);
WWHD_OFFSET(gndExecPlayer, mDemoMode, 0x420);
WWHD_OFFSET(gndExecPlayer, mDemoTimer, 0x428);
WWHD_OFFSET(gndExecPlayer, mDemoAction, 0x430);
WWHD_SIZE(gndExecPlayer, 0x434);
struct gndExecZelda {
    u8 _pad000[0x857];
    /* 0x857 */ be<u8> mFinishDemo;
    /* 0x858 */ be<u8> mResumeBattle;
    u8 _pad859[0x283];
    /* 0xADC */ be<u32> mDemoAction;
};
WWHD_OFFSET(gndExecZelda, mFinishDemo, 0x857);
WWHD_OFFSET(gndExecZelda, mResumeBattle, 0x858);
WWHD_OFFSET(gndExecZelda, mDemoAction, 0xADC);
WWHD_SIZE(gndExecZelda, 0xAE0);
struct gndExecPlayerVtable {
    u8 _pad000[0xE4];
    /* 0x0E4 */ gfn mVoiceStart;
    u8 _pad0E8[0x2C];
    /* 0x114 */ gfn mSetPlayerPosAndAngle;
    void setPlayerPosAndAngle(fopAc_ac_c* player, cXyz* pos, s16 angle) {
        gabi::call_ptr(mSetPlayerPosAndAngle.get(), player, pos, angle);
    }
    void voiceStart(fopAc_ac_c* player) { gabi::call_ptr(mVoiceStart.get(), player); }
};
WWHD_OFFSET(gndExecPlayerVtable, mVoiceStart, 0xE4);
WWHD_OFFSET(gndExecPlayerVtable, mSetPlayerPosAndAngle, 0x114);
WWHD_SIZE(gndExecPlayerVtable, 0x118);
struct gndExecStringVtable {
    u8 _pad000[0x14];
    /* 0x014 */ gfn mAssureTermination;
    void assureTermination(SafeString* string) { gabi::call_ptr(mAssureTermination.get(), string); }
};
WWHD_OFFSET(gndExecStringVtable, mAssureTermination, 0x14);
WWHD_SIZE(gndExecStringVtable, 0x18);

struct gndExecCameraPoints {
    cXyz mPoints[5];
};
WWHD_SIZE(gndExecCameraPoints, 0x3C);
struct gndExecDemoName {
    char mName[0x100];
};
WWHD_SIZE(gndExecDemoName, 0x100);
struct gndExecPlayView {
    u8 _pad[0x5AF8];
    gptr<void> mpCamera;
    u8 _padCamera[0x34];
    be<s8> mCameraId;
    u8 _padEnd[3];
};
WWHD_OFFSET(gndExecPlayView, mpCamera, 0x5AF8);
WWHD_OFFSET(gndExecPlayView, mCameraId, 0x5B30);
WWHD_SIZE(gndExecPlayView, 0x5B34);
struct gndExecCamera {
    u8 _pad[0x248];
    u8 mController;
};
WWHD_OFFSET(gndExecCamera, mController, 0x248);
using gndExecSigned = be<s32>;
using gndExecWord = be<u32>;
void* z_s_sub(void*, void*);
void gndExec_assureTermination(SafeString*);
void gndExec_move(gnd_class*);
BOOL gndExec_startShock(u32, s32, s32, cXyz*);
void gndExec_monsSeStart(u32, cXyz*, u32, s32, s32);
void gndExec_reportCamera(s32, s32, const char*, s32);

using gndExecActorPtr = gptr<fopAc_ac_c>;

WWHD_OFFSET(gndExecFields, mBgmParam, 0x3D4);
struct gndExecArchiveName {
    char mName[4];
};
WWHD_SIZE(gndExecArchiveName, 4);
WWHD_SIZE(gndExecCamera, 0x249);
enum gndExecCameraCut {
    CAMERA_INTRO_START = 1,
    CAMERA_INTRO_ORBIT = 2,
    CAMERA_INTRO_ZELDA = 3,
    CAMERA_INTRO_LOOK_UP = 4,
    CAMERA_INTRO_LINK = 5,
    CAMERA_ZELDA_START = 10,
    CAMERA_ZELDA_FOLLOW = 11,
    CAMERA_HIT_START = 20,
    CAMERA_HIT_LINK = 21,
    CAMERA_HIT_GANONDORF = 22,
    CAMERA_FINISH_START = 100,
    CAMERA_FINISH_SETUP = 101,
    CAMERA_FINISH_FOLLOW = 102,
    CAMERA_END = 150,
};

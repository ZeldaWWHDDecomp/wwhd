// Earth/Wind sage song statues.
// GC -> HD: MoveBg actor prefix grows from 0x2C8 to 0x3E0; actor size is 0x620.
// HD: messages use a global manager; smoke callback stride remains 0x20.
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/d/d_item.h"
// HD: private views cover the model/joint records and callback layouts used by this unit.
template <class T> inline T *Mknjd_view(u32 address) {
    return gabi::at<T>(address);
}
struct MknjdSmokeCallback {
    /* 0x0 */ be<u32> mVtable;
    /* 0x4 */ be<u32> mEmitter;
    u8 mPadding8[0x9];
    /* 0x11 */ be<u8> mRateOff;
    u8 mPadding12[0xE];
};
WWHD_OFFSET(MknjdSmokeCallback, mVtable, 0x0);
WWHD_OFFSET(MknjdSmokeCallback, mEmitter, 0x4);
WWHD_OFFSET(MknjdSmokeCallback, mRateOff, 0x11);
WWHD_SIZE(MknjdSmokeCallback, 0x20);
struct MknjdModel {
    u8 mPadding0[0x2C];
    /* 0x2C */ be<u32> mMatrixBlock;
    u8 mPadding30[0x7C];
    /* 0xAC */ be<u32> mModelData;
    u8 mPaddingB0[0x8];
    /* 0xB8 */ be<u32> mUserArea;
    /* 0xBC */ cXyz mBaseScale;
};
WWHD_OFFSET(MknjdModel, mMatrixBlock, 0x2C);
WWHD_OFFSET(MknjdModel, mModelData, 0xAC);
WWHD_OFFSET(MknjdModel, mUserArea, 0xB8);
WWHD_OFFSET(MknjdModel, mBaseScale, 0xBC);
WWHD_SIZE(MknjdModel, 0xC8);
struct MknjdMatrixBlock {
    u8 mPadding0[0x4];
    /* 0x4 */ be<u16> mFlags;
    u8 mPadding6[0xA];
    /* 0x10 */ be<u32> mMatrices;
};
WWHD_OFFSET(MknjdMatrixBlock, mFlags, 0x4);
WWHD_OFFSET(MknjdMatrixBlock, mMatrices, 0x10);
WWHD_SIZE(MknjdMatrixBlock, 0x14);
struct MknjdModelData {
    u8 mPadding0[0x4];
    /* 0x4 */ be<u32> mJointCount;
    /* 0x8 */ be<u32> mJoints;
};
WWHD_OFFSET(MknjdModelData, mJointCount, 0x4);
WWHD_OFFSET(MknjdModelData, mJoints, 0x8);
WWHD_SIZE(MknjdModelData, 0xC);
struct MknjdJoint {
    /* 0x0 */ be<u32> mNameOffset;
    /* 0x4 */ be<u16> mJointNo;
    u8 mPadding6[0x2];
    /* 0x8 */ be<u32> mCallback;
    u8 mPaddingC[0x4];
    /* 0x10 */ be<u32> mMesh;
    u8 mPadding14[0x8];
};
WWHD_OFFSET(MknjdJoint, mNameOffset, 0x0);
WWHD_OFFSET(MknjdJoint, mJointNo, 0x4);
WWHD_OFFSET(MknjdJoint, mCallback, 0x8);
WWHD_OFFSET(MknjdJoint, mMesh, 0x10);
WWHD_SIZE(MknjdJoint, 0x1C);
struct MknjdNameHeader {
    u8 mPadding0[0x10];
    /* 0x10 */ be<u32> mNameOffset;
};
WWHD_OFFSET(MknjdNameHeader, mNameOffset, 0x10);
WWHD_SIZE(MknjdNameHeader, 0x14);
struct MknjdJointTree {
    u8 mPadding0[0x8];
    /* 0x8 */ be<u16> mJointCount;
};
WWHD_OFFSET(MknjdJointTree, mJointCount, 0x8);
WWHD_SIZE(MknjdJointTree, 0xA);
struct MknjdMaterial {
    u8 mPadding0[0x4];
    /* 0x4 */ be<u32> mNext;
    /* 0x8 */ be<u32> mShape;
};
WWHD_OFFSET(MknjdMaterial, mNext, 0x4);
WWHD_OFFSET(MknjdMaterial, mShape, 0x8);
WWHD_SIZE(MknjdMaterial, 0xC);
struct MknjdShape {
    u8 mPadding0[0x4];
    /* 0x4 */ be<u8> mVisible;
};
WWHD_OFFSET(MknjdShape, mVisible, 0x4);
WWHD_SIZE(MknjdShape, 0x5);
struct MknjdEmitter {
    u8 mPadding0[0x34];
    /* 0x34 */ be<f32> mRate;
    u8 mPadding38[0x24];
    /* 0x5C */ be<u32> mMaxFrame;
    /* 0x60 */ be<u16> mLifeTime;
    u8 mPadding62[0x1A];
    /* 0x7C */ be<f32> mVolumeSweep;
    u8 mPadding80[0x1A0];
    /* 0x220 */ cXyz mDynamicsScale;
    u8 mPadding22C[0xC];
    /* 0x238 */ cXyz mParticleScale;
    u8 mPadding244[0x10];
    /* 0x254 */ be<u32> mFlags;
};
WWHD_OFFSET(MknjdEmitter, mRate, 0x34);
WWHD_OFFSET(MknjdEmitter, mMaxFrame, 0x5C);
WWHD_OFFSET(MknjdEmitter, mLifeTime, 0x60);
WWHD_OFFSET(MknjdEmitter, mVolumeSweep, 0x7C);
WWHD_OFFSET(MknjdEmitter, mDynamicsScale, 0x220);
WWHD_OFFSET(MknjdEmitter, mParticleScale, 0x238);
WWHD_OFFSET(MknjdEmitter, mFlags, 0x254);
WWHD_SIZE(MknjdEmitter, 0x258);
struct MknjdMedli {
    u8 mPadding0[0x628];
    /* 0x628 */ be<u32> mEmitter;
};
WWHD_OFFSET(MknjdMedli, mEmitter, 0x628);
WWHD_SIZE(MknjdMedli, 0x62C);
struct MknjdEnvironment {
    u8 mPadding0[0x10A4];
    /* 0x10A4 */ be<u8> mDemoFlag;
};
WWHD_OFFSET(MknjdEnvironment, mDemoFlag, 0x10A4);
WWHD_SIZE(MknjdEnvironment, 0x10A5);
struct MknjdPlayer {
    u8 mPadding0[0x3B8];
    /* 0x3B8 */ be<u32> mNoResetFlg0;
};
WWHD_OFFSET(MknjdPlayer, mNoResetFlg0, 0x3B8);
WWHD_SIZE(MknjdPlayer, 0x3BC);
struct MknjdMessage {
    u8 mPadding0[0x921];
    /* 0x921 */ be<u8> mSend;
};
WWHD_OFFSET(MknjdMessage, mSend, 0x921);
WWHD_SIZE(MknjdMessage, 0x922);
struct MknjdSafeStringVtable {
    u8 mPadding0[0x14];
    /* 0x14 */ be<u32> mAssureTermination;
};
WWHD_OFFSET(MknjdSafeStringVtable, mAssureTermination, 0x14);
WWHD_SIZE(MknjdSafeStringVtable, 0x18);
struct MknjdSmokeVtable {
    u8 mPadding0[0x44];
    /* 0x44 */ be<u32> mRemove;
};
WWHD_OFFSET(MknjdSmokeVtable, mRemove, 0x44);
WWHD_SIZE(MknjdSmokeVtable, 0x48);
struct MknjdPlayerVtable {
    u8 mPadding0[0x84];
    /* 0x84 */ be<u32> mSetTactZev;
    u8 mPadding88[0x8C];
    /* 0x114 */ be<u32> mSetPlayerPosAndAngle;
};
WWHD_OFFSET(MknjdPlayerVtable, mSetTactZev, 0x84);
WWHD_OFFSET(MknjdPlayerVtable, mSetPlayerPosAndAngle, 0x114);
WWHD_SIZE(MknjdPlayerVtable, 0x118);
struct MknjdInteger {
    u8 mPadding0[0x2];
    /* 0x2 */ be<s16> mLowHalf;
};
WWHD_OFFSET(MknjdInteger, mLowHalf, 0x2);
WWHD_SIZE(MknjdInteger, 0x4);
namespace daObjMknjD {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e { PRM_SWITCH_W = 8, PRM_SWITCH_S = 0, PRM_TYPE_W = 1, PRM_TYPE_S = 0x10 };
    s32 prm_get_swSave();
    u8 prm_get_Type();
    s16 XyCheckCB(int);
    s16 XyEventCB(int);
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    BOOL Delete();
    void set_mtx();
    void init_mtx();
    void setGoal(int);
    void setPlayerAngle(int);
    u16 talk(int);
    void privateCut();
    bool daObjMknjD_break();
    BOOL Execute(Mtx34 **);
    BOOL Draw();
    u32 smokeCB(int i) { return gabi::ea(&mSmokeCBs[i]); }

    /* 0x3E0 */ cXyz mLeftHalfPos;
    /* 0x3EC */ cXyz mRightHalfPos;
    /* 0x3F8 */ cXyz mShardPositions[0x14];
    /* 0x4E8 */ be<f32> mShardHeights[0x14];
    /* 0x538 */ request_of_phase_process_class mPhs;
    /* 0x540 */ gptr<J3DModel> mMainMdl;
    /* 0x544 */ gptr<J3DModel> mBreakMdl;
    /* 0x548 */ be<u16> m0430;
    /* 0x54A */ be<u16> m0432;
    /* 0x54C */ be<u16> m0434;
    /* 0x54E */ u8 _54E[2];
    /* 0x550 */ be<s32> mBreakTimer;
    /* 0x554 */ be<u8> mMainMdlAlpha;
    /* 0x555 */ be<u8> m043D;
    /* 0x556 */ be<u8> m043E;
    /* 0x557 */ be<u8> m043F;
    /* 0x558 */ gptr<JPABaseEmitter> mEmitters[4];
    /* 0x568 */ MknjdSmokeCallback
        mSmokeCBs[4]; /* dPa_smokeEcallBack (emitter at +4, rate-off flag at +0x11) */
    /* 0x5E8 */ cXyz mBrokenPos;
    /* 0x5F4 */ be<s16> mCheckEventIdx;
    /* 0x5F6 */ be<s16> mDemoEventIdx;
    /* 0x5F8 */ be<s16> mErrorEventIdx;
    /* 0x5FA */ be<s16> mLessonEventIdx;
    /* 0x5FC */ be<s8> mActionIdx;
    /* 0x5FD */ be<u8> mMelodyNum;
    /* 0x5FE */ be<u8> mGiveItemNo;
    /* 0x5FF */ u8 _5FF;
    /* 0x600 */ cXyz mGoalPos;
    /* 0x60C */ be<u32> mMsgNo;
    /* 0x610 */ be<s32> mMsgPID;
    /* 0x614 */ be<u8> mMsgFound; /* HD: replaces msg_class* mMsgPtr (the message manager is global) */
    /* 0x615 */ u8 _615[3];
    /* 0x618 */ be<s32> m0500;
    /* 0x61C */ be<u8> m0504;
    /* 0x61D */ u8 _61D[3];
};
WWHD_OFFSET(Act_c, mShardHeights, 0x4E8);
WWHD_OFFSET(Act_c, mMainMdl, 0x540);
WWHD_OFFSET(Act_c, mSmokeCBs, 0x568);
WWHD_OFFSET(Act_c, mBrokenPos, 0x5E8);
WWHD_OFFSET(Act_c, mGoalPos, 0x600);
WWHD_OFFSET(Act_c, m0504, 0x61C);
WWHD_SIZE(Act_c, 0x620);
} // namespace daObjMknjD

void Mknjd_setEventName(void *, const char *);
void mDoAud_bgmStart(u32);
void mDoAud_bgmStop(u32);
void mDoAud_taktModeMuteOff();
void Mknjd_seStart(u32);
s16 Mknjd_drawPriority(fopAc_ac_c *);
void Mknjd_toDrawQueue(void *, s16);
u32 msgMng_getStatus(u32);
void msgMng_setStatus(u32, u32);
u32 msgMng_messageSet(u32, u32, cXyz *);
u32 Mknjd_getNameHeader(u32);
s32 JUTNameTab_getIndex(u32, const char *);
u32 Mknjd_getJointTree(u32);
u32 Mknjd_smokeCtor(void *, s32);
void Mknjd_constructArray(void *, s32, s32, u32);
void Mknjd_destroyArray(void *, s32, s32, u32, s32, s32);
void Mknjd_actorDtor(void *, s32);
void Mknjd_orderChangeEventId(fopAc_ac_c *, s16, u16, u16);

enum { fpcNm_NPC_CB1_e = 0x14E, fpcNm_NPC_MD_e = 0x16F };
enum {
    fopMsgStts_MSG_DISPLAYED_e = 0xE,
    fopMsgStts_MSG_ENDS_e = 0x10,
    fopMsgStts_BOX_CLOSED_e = 0x12,
    fopMsgStts_MSG_DESTROYED_e = 0x13,
    fopMsgStts_INPUT_e = 0x15
};
enum { TYPE_EARTH, TYPE_WIND };
enum { cPhs_STOP_e = 3 };

/* NPC base (f_op_npc / d_npc), WWHD layout. 
 *
 * fopNpc_npc_c: GameCube members +0x11C (fopAc_ac_c grew by 0x11C), size 0x7DC. Confirmed by the
 * verified ba1/ls1 (m_jnt 0x3AC, mEventCut 0x3E0, mpMorf 0x44C, mObjAcch 0x450, mAcchCir 0x614,
 * mStts 0x654, mCyl 0x690, mCurrMsgNo 0x7C0, mCurrMsgBsPcId 0x7C8). */
#pragma once
#include "d/d_bg_s.h"
#include "d/d_cc_d.h"
#include "f_op/f_op_actor.h"
#include "m_Do/m_Do_ext.h"
#include "wwhd.h"

struct dNpc_JntCtrl_c {
    /* 0x00 */ be<s16> mAngles[2][2];
    /* 0x08 */ be<s8> mHeadJntNum;
    /* 0x09 */ be<s8> mBackboneJntNum;
    /* 0x0A */ be<u8> mbTrn;
    /* 0x0B */ be<u8> mbHeadLock;
    /* 0x0C */ be<u8> mbBackBoneLock;
    /* 0x0D */ be<u8> field_0x0D;
    /* 0x0E */ be<s16> mMinAngles[2][2];
    /* 0x16 */ be<s16> mMaxAngles[2][2];
    /* 0x1E */ be<s16> mMaxTurnStep[2][2];
    /* 0x26 */ u8 pad_0x26[0x2C - 0x26];
    /* 0x2C */ be<s16> field_0x2C;
    /* 0x2E */ be<s16> field_0x2E;
    /* 0x30 */ be<s16> field_0x30;
    /* 0x32 */ be<s16> field_0x32;
    /* 0259E08C [g] */
    void setParam(s16 a, s16 b, s16 c, s16 d, s16 e, s16 f, s16 g, s16 h, s16 i) {
        gabi::call(0x0259E08C, this, a, b, c, d, e, f, g, h, i);
    }
    /* 0259DED0 lookAtTarget / 0259E2D4 lookAtTarget_2 (this, s16* outY, cXyz* target, cXyz* eyeCopy, s16 yrot, s16 vel, u8 headOnly) [ba1] */
};
WWHD_SIZE(dNpc_JntCtrl_c, 0x34);

struct fopNpc_npc_c;
struct dNpc_EventCut_c {
    /* 0x00 */ be<u32> mpEvtStaffName;
    /* 0x04 */ be<s32> mEvtStaffId;
    /* 0x08 */ gptr<fopAc_ac_c> mpActor;
    /* 0x0C */ gptr<fopNpc_npc_c> mpTalkActor;
    /* 0x10 */ be<s32> mCurActIdx;
    /* 0x14 */ be<u32> field_0x14;
    /* 0x18 */ be<s32> mTimer;
    /* 0x1C */ be<u32> mpActorName;
    /* 0x20 */ be<u32> field_0x20;
    /* 0x24 */ be<s32> mSetId;
    /* 0x28 */ cXyz mOffsetPos;
    /* 0x34 */ cXyz mTargetActorPos;
    /* 0x40 */ gptr<fopAc_ac_c> mpTargetActor;
    /* 0x44 */ be<s16> field_0x44;
    /* 0x46 */ u8 _46[2];
    /* 0x48 */ be<f32> mSpeed;
    /* 0x4C */ be<f32> pDelDistance;
    /* 0x50 */ be<s16> mAddAngle;
    /* 0x52 */ be<u8> field_0x52;
    /* 0x53 */ u8 _53;
    /* 0x54 */ cXyz mPos;
    /* 0x60 */ be<u8> mbAttention;
    /* 0x61 */ be<u8> mbNoTurn;
    /* 0x62 */ be<s16> mTurnSpeed;
    /* 0x64 */ be<u32> mTurnType;
    /* 0x68 */ gptr<dNpc_JntCtrl_c> mpJntCtrl;
    /* 0259F814 [g] */
    void setActorInfo2(const char* name, fopNpc_npc_c* actor) { gabi::call(0x0259F814, this, name, actor); }
    /* 0259F858 [g] */
    BOOL cutProc() { return gabi::call<BOOL>(0x0259F858, this); }
};
WWHD_SIZE(dNpc_EventCut_c, 0x6C);

struct fopNpc_npc_c : fopAc_ac_c {
    /* 0x3AC */ dNpc_JntCtrl_c m_jnt;
    /* 0x3E0 */ dNpc_EventCut_c mEventCut;
    /* 0x44C */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x450 */ dBgS_ObjAcch mObjAcch;
    /* 0x614 */ dBgS_AcchCir mAcchCir;
    /* 0x654 */ dCcD_Stts mStts;
    /* 0x690 */ dCcD_Cyl mCyl;            /* [measured] */
    /* 0x7C0 */ be<u32> mCurrMsgNo;
    /* 0x7C4 */ be<u32> mEndMsgNo;
    /* 0x7C8 */ be<u32> mCurrMsgBsPcId;
    /* 0x7CC */ be<u8> mbHasMsg;           /* HD: a flag (GameCube msg_class* mpCurrMsg); talk() stores 0/1 */
    /* 0x7CD */ u8 _7CD[3];
    /* 0x7D0 */ be<u32> mPartnerProcId;   /* [ob1/yw1/ko1] */
    /* 0x7D4 */ be<u16> mMsgStatus;
    /* 0x7D6 */ be<u16> field_0x7d6;
    /* 0x7D8 */ be<u8> mTalkState;        /* two-NPC talk: 1 invited, 2 ready/talking, 3 end */
    /* 0x7D9 */ u8 _7D9[3];
    /* the GameCube vtable at 0x6C0 is merged into the HD vtable at 0xB4 */
    /* 025A15AC [g] */
    void setCollision(f32 r, f32 h) { gabi::call(0x025A15AC, this, r, h); }
    /* 025A11EC [g] */
    u16 talk(s32 p) { return gabi::call<u16>(0x025A11EC, this, p); }
};
WWHD_OFFSET(fopNpc_npc_c, mCyl, 0x690);
WWHD_SIZE(fopNpc_npc_c, 0x7DC); /* constructor 025A1458 allocates 0x7DC [v ba1, ls1] */

/* d_npc free functions [g] */
/* dNpc_playerEyePos returns a cXyz through a hidden result pointer (r3) [ba1] */
inline void dNpc_playerEyePos(cXyz* result, f32 offs) { gabi::call(0x0259D54C, result, offs); }
inline BOOL dNpc_setAnmIDRes(mDoExt_McaMorf* morf, s32 loopMode, f32 morfF, f32 speed, s32 anmIdx, s32 soundIdx, const char* arc) {
    return gabi::call<BOOL>(0x0259D24C, morf, loopMode, morfF, speed, anmIdx, soundIdx, arc);
}

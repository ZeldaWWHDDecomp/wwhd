/* NPC base (f_op_npc / d_npc), WWHD layout. 
 *
 * fopNpc_npc_c: GameCube members +0x11C (fopAc_ac_c grew by 0x11C), size 0x7DC. Confirmed by the
 * verified ba1/ls1 (m_jnt 0x3AC, mEventCut 0x3E0, mpMorf 0x44C, mObjAcch 0x450, mAcchCir 0x614,
 * mStts 0x654, mCyl 0x690, mCurrMsgNo 0x7C0, mCurrMsgBsPcId 0x7C8).
 *
 * The d_npc classes (layouts from the verified d_npc unit, d/d_npc.cpp):
 *  - dNpc_JntCtrl_c 0x34, dNpc_EventCut_c 0x6C, dNpc_PathRun_c 0x8: unchanged from GameCube;
 *  - dNpc_HIO_c 0x28: the GameCube members -4, the vtable at the end (+0x24; constructor 0259DA18);
 *  - dNpc_HeadAnm_c 0x20: the swing pointer-to-member is a GHS PTMF (8 bytes, GameCube 12), so the
 *    members after it are -4 (as the p1/bms1/rsh1 unit headers measured them).
 * HD differences of the methods are noted where they are bound. */
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
    /* 0259E08C setParam(maxBackboneX, maxBackboneY, minBackboneX, minBackboneY, maxHeadX, maxHeadY,
     * minHeadX, minHeadY, maxTurnStep): the last two on the stack [v sarace, kg1, kg2] */
    void setParam(s16 a, s16 b, s16 c, s16 d, s16 e, s16 f, s16 g, s16 h, s16 i) {
        gabi::call(0x0259E08C, this, a, b, c, d, e, f, g, h, i);
    }
    /* out of line in HD (d_npc unit) [g] */
    void limitter(be<s16>* angle, s16 upper, s16 lower) { gabi::call(0x0259DB48, this, angle, upper, lower); }
    BOOL move(s16 requested, int axis) { return gabi::call<BOOL>(0x0259DB6C, this, requested, axis); }
    BOOL angCalcS(be<s16>* angle, s16 goal, s16 minimum, s16 maximum) {
        return gabi::call<BOOL>(0x0259DCEC, this, angle, goal, minimum, maximum);
    }
    BOOL follow(be<s16>* angle, s16 goal, s16 maxStep, int axis) {
        return gabi::call<BOOL>(0x0259DDA0, this, angle, goal, maxStep, axis);
    }
    s16 chkLim(s16 value, int joint, int axis) { return gabi::call<s16>(0x0259E0C8, this, value, joint, axis); }
    void turn_fromBackbone2Head(s16 requested, be<s16>* head, be<s16>* back, bool direction) {
        gabi::call(0x0259E104, this, requested, head, back, direction);
    }
    void turn_fromHead2Backbone(s16 requested, be<s16>* head, be<s16>* back) {
        gabi::call(0x0259E1DC, this, requested, head, back);
    }
    s16 follow_current(be<s16>* angle, s16 amount) { return gabi::call<s16>(0x0259E28C, this, angle, amount); }
    /* 0259E2D4 lookAtTarget_2: srcPos by value (address of a stack copy) */
    void lookAtTarget_2(be<s16>* outY, cXyz* dstPos, const cXyz& srcPos, s16 defaultY, s16 maxVel, bool p6) {
        gabi::Stack<cXyz> src(srcPos);
        gabi::call(0x0259E2D4, this, outY, dstPos, src.get(), defaultY, maxVel, p6);
    }
    /* 0259DED0 lookAtTarget / 0259E2D4 lookAtTarget_2 (this, s16* outY, cXyz* target, cXyz* eyeCopy, s16 yrot, s16 vel, u8 headOnly) [ba1] */

    /* ---- GameCube-named inlines (gc_api) [v nz] ---- */
    s8 getHeadJntNum() { return mHeadJntNum; }
    s8 getBackboneJntNum() { return mBackboneJntNum; }
    void setHeadJntNum(s8 j) { mHeadJntNum = j; }
    void setBackboneJntNum(s8 j) { mBackboneJntNum = j; }
    s16 getHead_x() { return mAngles[0][0]; }
    s16 getHead_y() { return mAngles[0][1]; }
    s16 getBackbone_x() { return mAngles[1][0]; }
    s16 getBackbone_y() { return mAngles[1][1]; }
    bool trnChk() { return mbTrn != 0; }
    void onHeadLock() { mbHeadLock = 1; }
    void onBackBoneLock() { mbBackBoneLock = 1; }
    void offHeadLock() { mbHeadLock = 0; }
    void offBackBoneLock() { mbBackBoneLock = 0; }
    /* srcPos is passed by value: GHS passes the address of a stack copy */
    void lookAtTarget(be<s16>* outY, cXyz* dstPos, const cXyz& srcPos, s16 defaultY, s16 maxVel, bool p6) {
        gabi::Stack<cXyz> src(srcPos);
        gabi::call(0x0259DED0, this, outY, dstPos, src.get(), defaultY, maxVel, p6);
    }
};
WWHD_SIZE(dNpc_JntCtrl_c, 0x34);

/* dNpc_HIO_c (HD 0x28: GameCube members -4, vtable at the end; constructor 0259DA18) */
struct dNpc_HIO_c {
    /* 0x00 */ be<f32> m04;              /* GameCube 0x04 */
    /* 0x04 */ be<s16> mMaxHeadX;
    /* 0x06 */ be<s16> mMaxBackboneX;
    /* 0x08 */ be<s16> mMaxHeadY;
    /* 0x0A */ be<s16> mMaxBackboneY;
    /* 0x0C */ be<s16> mMinHeadX;
    /* 0x0E */ be<s16> mMinBackboneX;
    /* 0x10 */ be<s16> mMinHeadY;
    /* 0x12 */ be<s16> mMinBackboneY;
    /* 0x14 */ be<s16> mMaxTurnStep;
    /* 0x16 */ be<s16> mMaxHeadTurnVel;
    /* 0x18 */ be<f32> mAttnYOffset;
    /* 0x1C */ be<s16> mMaxAttnAngleY;
    /* 0x1E */ be<u8> m22;
    /* 0x1F */ u8 _1F;
    /* 0x20 */ be<f32> mMaxAttnDistXZ;
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(dNpc_HIO_c, 0x28);

/* dNpc_HeadAnm_c (HD 0x20: the swing PTMF is 8 bytes, the members after it GameCube -4) [v d_npc; embedded in p1, bms1] */
struct dNpc_HeadAnm_c {
    /* 0x00 */ csXyz field_0x00;
    /* 0x06 */ u8 _06[2];
    /* 0x08 */ be<s16> mProcDelta;       /* mProc: GHS PTMF {this delta, vtable index (-1: direct), function} */
    /* 0x0A */ be<s16> mProcIndex;
    /* 0x0C */ be<u32> mProcFunc;
    /* 0x10 */ be<f32> field_0x14;       /* GameCube 0x14 */
    /* 0x14 */ be<f32> field_0x18;
    /* 0x18 */ be<s16> field_0x1C;
    /* 0x1A */ be<s16> field_0x1E;
    /* 0x1C */ be<s16> field_0x20;
    /* 0x1E */ u8 _1E[2];
    /* 0259F36C / 0259F514 [g] */
    void swing_vertical_init(s16 count, s16 step, s16 amplitude, int resume) { gabi::call(0x0259F36C, this, count, step, amplitude, resume); }
    void swing_horizone_init(s16 count, s16 step, s16 amplitude, int resume) { gabi::call(0x0259F514, this, count, step, amplitude, resume); }
    /* 0259F67C [g] */
    void move() { gabi::call(0x0259F67C, this); }
};
WWHD_OFFSET(dNpc_HeadAnm_c, field_0x14, 0x10);
WWHD_SIZE(dNpc_HeadAnm_c, 0x20);

/* dNpc_PathRun_c (0x8, unchanged from GameCube) [v d_npc] */
struct dNpc_PathRun_c {
    /* 0x00 */ gptr<dPath> mPath;
    /* 0x04 */ be<u8> field_0x04;
    /* 0x05 */ be<u8> mIdx;
    /* 0x06 */ be<u8> mbDir;
    /* 0x07 */ be<u8> field_0x07;
    /* out of line in HD (d_npc unit) [g] */
    BOOL setInf(u8 pathIdx, s8 roomNo, u8 forwards) { return gabi::call<BOOL>(0x0259E6D0, this, pathIdx, roomNo, forwards); }
    BOOL setInfDrct(dPath* path) { return gabi::call<BOOL>(0x0259E730, this, path); }
    dPath* nextPath(s8 roomNo) { return gabi::call<dPath*>(0x0259E744, this, roomNo); }
    /* getPoint returns a cXyz through a hidden result pointer */
    void getPoint(cXyz* result, u8 pointIdx) { gabi::call(0x0259E778, this, result, pointIdx); }
    BOOL chkPointPass(cXyz* pos, bool forward) { return gabi::call<BOOL>(0x0259E838, this, pos, forward); }
    BOOL incIdx() { return gabi::call<BOOL>(0x0259EB1C, this); }
    BOOL incIdxLoop() { return gabi::call<BOOL>(0x0259EB60, this); }
    BOOL incIdxAuto() { return gabi::call<BOOL>(0x0259EBA4, this); }
    BOOL decIdx() { return gabi::call<BOOL>(0x0259EC0C, this); }
    BOOL decIdxLoop() { return gabi::call<BOOL>(0x0259EC50, this); }
    BOOL decIdxAuto() { return gabi::call<BOOL>(0x0259EC94, this); }
    BOOL nextIdx() { return gabi::call<BOOL>(0x0259ECFC, this); }
    BOOL nextIdxAuto() { return gabi::call<BOOL>(0x0259ED58, this); }
    s32 absIdx(u8 a, u8 b) { return gabi::call<s32>(0x0259EDD0, this, a, b); }
    u8 maxPoint() { return gabi::call<u8>(0x0259EDB8, this); }
    u8 pointArg(u8 idx) { return gabi::call<u8>(0x0259EE4C, this, idx); }
    BOOL setNearPathIndx(cXyz* pos, f32 weight) { return gabi::call<BOOL>(0x0259EE7C, this, pos, weight); }
    f32 setNearPathIndxMk(cXyz* pos) { return gabi::call<f32>(0x0259EFE0, this, pos); }
    BOOL setNearPathIndxMk2(cXyz* pos, u8 cur, u8 maxDiff) { return gabi::call<BOOL>(0x0259F0F8, this, pos, cur, maxDiff); }
    BOOL chkInside(cXyz* pos) { return gabi::call<BOOL>(0x0259F220, this, pos); }
    u8 getIdx() { return mIdx; }
    void setIdx(u8 idx) { mIdx = idx; }
    dPath* getPath() { return mPath; }
};
WWHD_SIZE(dNpc_PathRun_c, 0x8);

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
    /* 0259F7D4 [g] */
    void setActorInfo(const char* name, fopAc_ac_c* actor) { gabi::call(0x0259F7D4, this, name, actor); }
    /* 0259F814 [g] */
    void setActorInfo2(const char* name, fopNpc_npc_c* actor) { gabi::call(0x0259F814, this, name, actor); }
    /* 0259F858 [g] */
    BOOL cutProc() { return gabi::call<BOOL>(0x0259F858, this); }
    const char* getActorName() { return gabi::at<const char>(mpEvtStaffName); }
    int getNowCut() { return mCurActIdx; }
    bool getAttnFlag() { return mbAttention != 0; }
    void setAttnFlag(bool flag) { mbAttention = flag; }
    bool getAttnNoTurnFlag() { return mbNoTurn != 0; }
    void setAttnNoTurnFlag(bool flag) { mbNoTurn = flag; }
    f32 getMoveSpeed() { return mSpeed; }
    s16 getTurnSpeed() { return mTurnSpeed; }
    void setJntCtrlPtr(dNpc_JntCtrl_c* ctrl) { mpJntCtrl = ctrl; }
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
    /* GameCube msg_class* mpCurrMsg; HD: the message manager behind the pointer at 0x101F4B5C
     * (layout: gc_api/f_op/f_op_msg.h) [v nz] */
    static gptr<msg_class>& mpCurrMsg;
};
inline gptr<msg_class>& fopNpc_npc_c::mpCurrMsg = gabi::object_at<gptr<msg_class>>(0x101F4B5C);
WWHD_OFFSET(fopNpc_npc_c, mCyl, 0x690);
WWHD_SIZE(fopNpc_npc_c, 0x7DC); /* constructor 025A1458 allocates 0x7DC [v ba1, ls1] */

/* d_npc free functions [g] */
/* dNpc_playerEyePos returns a cXyz through a hidden result pointer (r3) [ba1] */
inline void dNpc_playerEyePos(cXyz* result, f32 offs) { gabi::call(0x0259D54C, result, offs); }
/* GameCube form: `cXyz p = dNpc_playerEyePos(offs);` [v nz] */
inline auto dNpc_playerEyePos(f32 offs) {
    return gabi::into<cXyz>([offs](cXyz* r) { gabi::call(0x0259D54C, r, offs); });
}
inline BOOL dNpc_setAnmIDRes(mDoExt_McaMorf* morf, s32 loopMode, f32 morfF, f32 speed, s32 anmIdx, s32 soundIdx, const char* arc) {
    return gabi::call<BOOL>(0x0259D24C, morf, loopMode, morfF, speed, anmIdx, soundIdx, arc);
}
/* the other d_npc free functions (dNpc_setAnm, dNpc_calc_DisXZ_AngY, ...): gc_api/d/d_npc.h (raw units
 * define static copies of some of them) */

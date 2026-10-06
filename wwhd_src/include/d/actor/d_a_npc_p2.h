/* daNpc_P2_c (Zuko, Niko, Mako: Tetra's pirates), WWHD layout. 
 *
 * The GameCube decompilation has no layout for this class (all functions are "Nonmatching" stubs):
 * everything here is measured from the WWHD code of d_a_npc_p2 (profile size 0x980). The class
 * derives from fopAc_ac_c directly. Names follow the GameCube function names and the event
 * substance names ("Speed_y", "Gravity", "MsgNum", "Attention", "Pos", ...). */
#pragma once
#include "bindings.h"
#include "d/d_npc.h"
#include "d/actor/d_a_npc_ba1.h" /* ProcFunc_l (GHS pointer to member function, measured) */

/* dNpc_HIO_c (0x28, constructor 0259DA18), as in d_a_npc_bs1.h (SHARED-CANDIDATE) */
struct dNpc_HIO_p2 {
    /* 0x00 */ be<f32> m04;              /* playerEyePos offset */
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
WWHD_SIZE(dNpc_HIO_p2, 0x28);

/* daNpc_P2_childHIO_c (HD 0xD8, vtable at the end) */
struct daNpc_P2_childHIO_c {
    /* 0x00 */ be<u32> unk0;
    /* 0x04 */ dNpc_HIO_p2 mNpc;
    /* 0x2C */ be<u8> m2C;
    /* 0x2D */ u8 _2D[3];
    /* 0x30 */ cXyz mGoalTalkPos;        /* the talk position at the goal (P2b) */
    /* 0x3C */ cXyz mGoalPos2;           /* goal_wait_2 circle centre */
    /* 0x48 */ cXyz mDemoWaitPos;        /* demo_wait circle centre */
    /* 0x54 */ be<f32> mDemoWaitR;
    /* 0x58 */ be<f32> mDemoWaitH;
    /* 0x5C */ be<f32> mGoalTalkR;
    /* 0x60 */ be<f32> mGoal2R;
    /* 0x64 */ be<f32> mGoalTalkH;
    /* 0x68 */ be<f32> mGoal2H;
    /* 0x6C */ be<u8> m6C;
    /* 0x6D */ u8 _6D[3];
    /* 0x70 */ be<f32> mGoalDist;
    /* 0x74 */ be<f32> mMorf[24];        /* copied to the morf table l_morf (0x101C2D50) by setAnm */
    /* 0xD4 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_P2_childHIO_c, 0xD8);

/* daNpc_P2_HIO_c (HD 0x294, vtable first) */
struct daNpc_P2_HIO_c {
    /* 0x000 */ be<u32> __vtbl;
    /* 0x004 */ daNpc_P2_childHIO_c children[3];
    /* 0x28C */ be<f32> mRunSpeedScale;
    /* 0x290 */ be<f32> mMinRunSpeed;
};
WWHD_SIZE(daNpc_P2_HIO_c, 0x294);

struct daNpc_P2_c : fopAc_ac_c {
    enum { TYPE_P2A_e = 0, TYPE_P2B_e = 1, TYPE_P2C_e = 2 };

    BOOL initTexPatternAnm(u32);
    void playTexPatternAnm();
    void setAnm();
    u32 setTexAnm();
    void setAttention();
    BOOL chkAttention();
    void lookBack();
    void setMtx();
    void setCollision();
    void smoke_set();
    u32 next_msgStatus(u32*);
    u32 getMsg();
    void talkInit();
    void anmAtr(u16);
    u16 talk(bool);
    void eventOrder();
    void checkOrder();
    void demo_wait_2();
    void demo_intro_2();
    void goal_wait_2();
    void demo_goal_2();
    void demo_bomb_get();
    void demo_wait();
    void demo_intro();
    void demo_lift();
    void demo_jump();
    void goal_goalpos_to_talkpos();
    void goal_talkpos_to_goalpos();
    void goal_goalpos_wait();
    void goal_talkpos_wait();
    void demo_goal();
    void goal_talkpos_talk();
    void treasure_wait();
    void treasure_wait_talk();
    void demo_arrive();
    void wait01();
    void zukotelescope();
    void moccowait();
    void talk01();
    BOOL intro_action(void*);
    BOOL wait_action(void*);
    BOOL _execute();
    void draw_item(J3DModel*, s8);
    void drawDagger();
    void drawHead();
    void drawP2a();
    void drawP2b();
    void drawP2c();
    BOOL _draw();
    void getArg();
    BOOL _createHeap();
    void createInit();
    cPhs_State _create();
    BOOL _delete();
    void cutProc();
    void cutTalkStart(int);
    void cutTalkProc(int);
    void cutRideSwitchStart(int);
    void cutRideSwitchProc(int);
    void cutRunWaitStart(int);
    void cutRunWaitProc(int);
    void cutJumpToLiftStart(int);
    void cutJumpToLiftProc(int);
    void cutLiftToRopeStart(int);
    void cutLiftToRopeProc(int);
    void cutRopeTalkStart(int);
    void cutRopeTalkProc(int);
    void cutRopeToLiftStart(int);
    void cutRopeToLiftProc(int);
    void cutJumpToGoalStart(int);
    void cutJumpToGoalProc(int);
    void cutJumpStart(int);
    void cutJumpProc(int);
    void cutSetAnmStart(int);
    void cutSetAnmProc(int);
    void cutSwOnStart(int);
    void cutSwOnProc(int);
    void cutSwOffStart(int);
    void cutSwOffProc(int);
    void cutSurpriseStart(int);
    void cutSurpriseProc(int);
    u32 cutOmamoriInitStart(int);
    void cutOmamoriInitProc(int);
    u32 cutOmamoriEndStart(int);
    void cutOmamoriEndProc(int);

    /* 0x3AC */ be<u8> mType;            /* parameter bits 0..1 (3 -> 0) */
    /* 0x3AD */ be<u8> mSubType;         /* parameter bits 2..9 (0xFF -> 0) */
    /* 0x3AE */ be<u8> mSwitchNo;        /* parameter bits 10..17 */
    /* 0x3AF */ u8 _3AF;
    /* 0x3B0 */ request_of_phase_process_class mPhs;
    /* 0x3B8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3BC */ gptr<mDoExt_McaMorf> mpBookMorf;   /* P2c: the book */
    /* 0x3C0 */ u8 mBtp[0x74];           /* mDoExt_btpAnm (HD 0x74); its J3DAnmTexPattern* at +0x10 */
    /* 0x434 */ gptr<J3DModel> mpHeadModel;
    /* 0x438 */ gptr<J3DModel> mpDaggerModel;
    /* 0x43C */ gptr<J3DModel> mpDagger2Model;
    /* 0x440 */ gptr<J3DModel> mpTelescopeModel; /* P2a */
    /* 0x444 */ dNpc_JntCtrl_c m_jnt;
    /* 0x478 */ dNpc_EventCut_c mEventCut;
    /* 0x4E4 */ u8 _4E4[4];
    /* 0x4E8 */ be<u8> mMorfPlayEnd;
    /* 0x4E9 */ u8 _4E9[3];
    /* 0x4EC */ be<f32> mPrevFrame;
    /* 0x4F0 */ be<u8> mBlinkFrame;
    /* 0x4F1 */ u8 _4F1;
    /* 0x4F2 */ be<s16> mBlinkTimer;
    /* 0x4F4 */ dBgS_ObjAcch mObjAcch;
    /* 0x6B8 */ dBgS_AcchCir mAcchCir;
    /* 0x6F8 */ dCcD_Stts mStts;
    /* 0x734 */ dCcD_Cyl mCyl;
    /* 0x864 */ csXyz mHomeAngle;
    /* 0x86A */ be<s16> mLookBackY;
    /* 0x86C */ u8 _86C[4];
    /* 0x870 */ cXyz mEyePos;            /* nodeCallBack: head joint (20, 10, 0) */
    /* 0x87C */ cXyz mHeadPos;           /* nodeCallBack: head joint origin */
    /* 0x888 */ be<u8> mHeadCalcCount;
    /* 0x889 */ u8 _889[3];
    /* 0x88C */ cXyz mNearPos;           /* searchNearLift / searchNearRope */
    /* 0x898 */ gptr<fopAc_ac_c> mpNearActor;
    /* 0x89C */ be<u8> mbAttention;      /* chkAttention() */
    /* 0x89D */ be<u8> mbTalk;
    /* 0x89E */ u8 _89E[2];
    /* 0x8A0 */ be<u32> mMsgNo;
    /* 0x8A4 */ be<u8> mbEvtAttention;
    /* 0x8A5 */ u8 _8A5[3];
    /* 0x8A8 */ cXyz mLookPos;
    /* 0x8B4 */ cXyz mGoalLookPos;
    /* 0x8C0 */ be<s16> mShipAngleOffs;
    /* 0x8C2 */ be<u8> m8C2;
    /* 0x8C3 */ u8 _8C3;
    /* 0x8C4 */ be<s32> mMoccoTimer;
    /* 0x8C8 */ be<u8> mbNoAttention;
    /* 0x8C9 */ be<u8> m8C9;
    /* 0x8CA */ u8 _8CA[2];
    /* 0x8CC */ u8 mSmokeCB[0x20];       /* dPa_smokeEcallBack (constructor 025A5B18(this, 1)); emitter at +4 */
    /* 0x8EC */ cXyz mSmokePos;
    /* 0x8F8 */ csXyz mSmokeAngle;
    /* 0x8FE */ u8 _8FE[6];
    /* 0x904 */ cXyz mHandPos;           /* rope talk: the hand joint position */
    /* 0x910 */ be<f32> mRopeLen;
    /* 0x914 */ be<f32> mRopeOmega;
    /* 0x918 */ be<f32> mRopeSwingX;
    /* 0x91C */ be<f32> mRopeSwingZ;
    /* 0x920 */ be<s16> mRopeAmp;
    /* 0x922 */ be<s16> mRopeTilt;
    /* 0x924 */ be<u32> m924;
    /* 0x928 */ cXyz mRopePos;
    /* 0x934 */ gptr<fopAc_ac_c> mpRope;
    /* 0x938 */ be<u8> mbRopeHang;
    /* 0x939 */ be<u8> mbBigCyl;
    /* 0x93A */ u8 _93A[2];
    /* 0x93C */ ProcFunc_l mAction;
    /* 0x944 */ be<s8> mBtpNum;
    /* 0x945 */ be<s8> mTexAnm;
    /* 0x946 */ be<s8> mAnmRes;
    /* 0x947 */ be<s8> mAnm;             /* requested animation */
    /* 0x948 */ be<s8> mCurAnm;
    /* 0x949 */ be<s8> mOrder;           /* event order (1/2 speak, >= 3 other event) */
    /* 0x94A */ be<s8> mMode;            /* sub-action of the action function */
    /* 0x94B */ be<u8> mbInEvent;
    /* 0x94C */ u8 _94C[4];
    /* 0x950 */ cXyz mLookOffs;
    /* 0x95C */ cXyz mJumpPos;
    /* 0x968 */ be<s32> mTimer;
    /* 0x96C */ be<f32> mJumpSpeedY;
    /* 0x970 */ be<f32> mJumpSpeed;
    /* 0x974 */ be<f32> mJumpGravity;
    /* 0x978 */ be<u32> m978;
    /* 0x97C */ be<s8> mActionStatus;
    /* 0x97D */ be<s8> mTalkState;
    /* 0x97E */ be<s8> mbDagger;
    /* 0x97F */ u8 _97F;
};
WWHD_OFFSET(daNpc_P2_c, mPhs, 0x3B0);
WWHD_OFFSET(daNpc_P2_c, m_jnt, 0x444);
WWHD_OFFSET(daNpc_P2_c, mEventCut, 0x478);
WWHD_OFFSET(daNpc_P2_c, mObjAcch, 0x4F4);
WWHD_OFFSET(daNpc_P2_c, mStts, 0x6F8);
WWHD_OFFSET(daNpc_P2_c, mCyl, 0x734);
WWHD_OFFSET(daNpc_P2_c, mSmokeCB, 0x8CC);
WWHD_OFFSET(daNpc_P2_c, mAction, 0x93C);
WWHD_SIZE(daNpc_P2_c, 0x980);

/* ======================= helpers shared by the d_a_npc_p2 source files ======================= */
#define P2_SAFESTRING_VTBL 0x1001FB04u /* this TU's sead::SafeString vtable */
#define P2_VTBL 0x1001FB5Cu            /* daNpc_P2_c vtable (HD virtual destructor) */
#define P2_AAB_VTBL 0x1001FB1Cu        /* this TU's cM3dGAab vtable copy */
#define P2_ARC 0x100200A0u             /* "P2" */

/* file statics: l_HIO (0x10468054), l_msgId (0x10467FDC), l_rope_dist (0x10467FE0) */
static inline daNpc_P2_HIO_c* p2_HIO() { return gabi::at<daNpc_P2_HIO_c>(0x10468054); }
static inline daNpc_P2_childHIO_c* p2_child(u32 type) { return &p2_HIO()->children[type]; }
static inline be<u32>& p2_msgId() { return *gabi::at<be<u32>>(0x10467FDC); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 p2_msgMgr() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 p2_msgGetStatus(u32 m) { return gabi::call<u32>(0x025F795C, m); }
static inline void p2_msgSetStatus(u32 m, u32 st) { gabi::call(0x025F74D0, m, st); }
static inline u32 p2_msgSet(u32 m, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, m, msgNo, pos); }
static inline dSv_event_c* p2_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL p2_isEventBit(u16 f) { return dSv_event_isEventBit(p2_event(), f); }
static inline void p2_onEventBit(u16 f) { gabi::call(0x025B8B68, p2_event(), f); }
/* dComIfGp_event_reset(): play + 0x52B8 |= 8 */
static inline void p2_event_reset() {
    u32 a = dComIfGp_ea() + 0x52B8;
    gabi::store<u16>(a, (u16)(gabi::load<u16>(a) | 8));
}
/* dComIfGp_event_runCheck(): play + 0x5292 */
static inline u8 p2_event_getMode() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
/* 0254457C dEvent_manager_c::endCheckOld(const char*) */
static inline BOOL p2_endCheckOld(u32 name) { return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name); }
static inline void* p2_getRes(s32 idx) { return dComIfG_getObjectRes(STR(P2_ARC), idx, P2_SAFESTRING_VTBL); }
/* mDoExt_McaMorf::isStop(): frame control state bit 0 or rate 0 */
static inline bool p2_isStop(mDoExt_McaMorf* m) {
    u32 p = gabi::ea(m);
    return (gabi::load<u8>(p + 0xA7) & 1) || gabi::load<f32>(p + 0x98) == 0.0f;
}
/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb): fopAcM_monsSeStart(this, id, 0) */
static inline void p2_monsSeStart(fopAc_ac_c* a, u32 id) {
    s32 reverb = dComIfGp_getReverb(a->current.roomNo);
    gabi::call(0x025E1AA4, id, &a->eyePos, gabi::load<u32>(gabi::ea(a) + 4), 0, reverb);
}
static inline void p2_copy12(u32 dst, u32 src) {
    u32 a = gabi::load<u32>(src), b = gabi::load<u32>(src + 4), c = gabi::load<u32>(src + 8);
    gabi::store<u32>(dst, a);
    gabi::store<u32>(dst + 4, b);
    gabi::store<u32>(dst + 8, c);
}
/* matrix assignment through FPRs (lfs x12, stfs x12) */
static inline void p2_mtx_copy(u32 dst, u32 src) {
    f32 t[12];
    for (int i = 0; i < 12; i++) t[i] = gabi::load<f32>(src + 4 * i);
    for (int i = 0; i < 12; i++) gabi::store<f32>(dst + 4 * i, t[i]);
}
/* J3DModel::getAnmMtx (HD: joint matrix block at +0x2C, dirty flag 0x10 at +4, matrices at +0x10) */
static inline u32 p2_getAnmMtx(J3DModel* model, s32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::load<u32>(blk + 0x10) + jntNo * 0x30;
}
/* GHS pointer to member function call */
static inline void p2_pmf_call(void* self, ProcFunc_l* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}
/* setAction(&daNpc_P2_c::fn, NULL) (inline) */
static inline void p2_setAction(daNpc_P2_c* a, u32 fn) {
    ProcFunc_l* cur = &a->mAction;
    s16 i = cur->i;
    if (i == -1 && cur->d == 0 && cur->f == fn) return;
    if (i != 0) {
        a->mActionStatus = -1;
        p2_pmf_call(a, cur, nullptr);
    }
    cur->d = 0;
    cur->f = fn;
    a->mActionStatus = 0;
    cur->i = -1;
    gabi::call_ptr<BOOL>(cur->f, gabi::ea(a) + (s32)(s16)cur->d, (u32)0);
}
#define P2_intro_action 0x022BD7FCu
#define P2_wait_action 0x022BC544u

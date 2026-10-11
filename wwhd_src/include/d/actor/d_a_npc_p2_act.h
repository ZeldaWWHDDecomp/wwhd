/* Private action view of daNpc_P2_c (Zuko, Niko, Mako), WWHD layout.
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
    /* 0x8C2 */ be<u8> mbGoalTalkStarted;
    /* 0x8C3 */ u8 _8C3;
    /* 0x8C4 */ be<s32> mMoccoTimer;
    /* 0x8C8 */ be<u8> mbNoAttention;
    /* 0x8C9 */ be<u8> mbMessageGestureStarted;
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
    /* 0x924 */ u8 paddingRope[4];
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
    /* 0x978 */ be<u32> mLiftPhase;
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


#include "gc_api/d/d_com_inf_game.h"
// HD: private views for the player busy virtual and goal ship's stop flag.
struct P2ActionPlayerVtable { u8 padding[0x4C]; be<u32> isBusy; };
struct P2ActionPlayer { u8 padding[0xB4]; gptr<P2ActionPlayerVtable> vtable; };
struct P2ActionShip { u8 padding[0x3B4]; be<u8> mbStopped; };
WWHD_OFFSET(P2ActionPlayerVtable, isBusy, 0x4C);
WWHD_OFFSET(P2ActionPlayer, vtable, 0xB4);
WWHD_OFFSET(P2ActionShip, mbStopped, 0x3B4);
BOOL dLib_checkPlayerInCircle(cXyz*, f32, f32);
u32 dLib_setFirstMsg(u16, u32, u32);
s32 P2_calcTimer(be<s32>*);
u32 P2_msgGetStatus(u32);
void P2_msgSetStatus(u32, u32);
u32 P2_msgSet(u32, u32, cXyz*);
BOOL P2_endCheckOld(dEvent_manager_c*, const char*);
void dKy_instant_timechg(f32);
static daNpc_P2_childHIO_c* p2_child(u32 type) {
    return &gabi::at<daNpc_P2_HIO_c>(0x10468054)->children[type];
}
// HD: these globals belong to the base P2 source and the message subsystem.
static be<u32>& p2_msgId() { return *gabi::at<be<u32>>(0x10467FDC); }
static u32 p2_msgMgr() { return *gabi::at<be<u32>>(0x101F4B5C); }
static bool p2_isStop(mDoExt_McaMorf* morf) {
    return (morf->mFrameCtrl.mState & 1) || morf->mFrameCtrl.mRate == 0.0f;
}
static void p2_copyPosition(cXyz* dst, const cXyz* src) {
    // HD: all three words are loaded before any destination write.
    const be<u32>* from = reinterpret_cast<const be<u32>*>(src);
    be<u32>* to = reinterpret_cast<be<u32>*>(dst);
    u32 x = from[0], y = from[1], z = from[2];
    to[0] = x; to[1] = y; to[2] = z;
}

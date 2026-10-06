/* daNpc_Mk_c (Ivan, Outset hide-and-seek boy), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_mk: daNpc_Mk_c derives from
 * fopAc_ac_c directly, so its members are +0x11C up to mpMorf; mShadowId (GameCube 0x29C) is
 * gone (HD shadows), so m_head_tex_pattern/mBtpAnm are +0x118; mDoExt_btpAnm grew from 0x14 to
 * 0x74, so everything from mBlinkFrame on is +0x178 up to mCurrActionFunc; the pointer to member
 * function is 8 bytes (GHS) instead of 12, so the members after it are +0x174. Size 0x87C
 * (GameCube 0x708). */
#pragma once
#include "bindings.h"
#include "d/actor/d_a_npc_mk_static.h"

/* GHS pointer to member function: {s16 this delta, s16 vtable index (< 0: not virtual), u32
 * function (or, with an index, the vtable offset in its low half)} */
struct MkActionFunc_l {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
WWHD_SIZE(MkActionFunc_l, 8);

struct daNpc_Mk_c : fopAc_ac_c {
    enum TalkStates { TALK_FINISHED = -1, TALK_INIT = 0, TALK_MSG_CREATE = 1, TALK_ACTIVE = 2 };
    enum ActionStatus { ACTION_STARTING = 0, ACTION_ONGOING = 1, ACTION_ENDING = -1 };
    enum VisitModes {
        VISIT_START = 0,
        VISIT_TALK = 1,
        VISIT_WALK_PATH = 2,
        VISIT_RUN_LINK = 3,
        VISIT_WALK_AROUND_LINK = 4,
        VISIT_REACHED_LINK = 5,
        VISIT_NOTICE_LINK = 6,
        VISIT_LEFT_PATH = 7,
        VISIT_WALK_PATH_FAST = 8,
        VISIT_WALK_PATH_IGNORE_LINK = 9,
    };
    enum States {
        STATE_WAIT = 0,
        STATE_TALK01 = 1,
        STATE_TALK02 = 2,
        STATE_VISIT = 3,
        STATE_CLIMB01 = 4,
        STATE_DROP01 = 5,
        STATE_RUNAWAY = 6,
        STATE_JUMP = 7,
        STATE_JITANDA01 = 8,
        STATE_JITANDA02 = 9,
        STATE_DEMO01 = 10,
        STATE_DEMO02 = 11,
        STATE_DEMO03 = 12,
        STATE_13 = 13,
    };
    enum Types { TYPE_NONE = 0, TYPE_NORMAL = 1, TYPE_MINIGAME = 2 };

    u32 ChkOrder(u8 flag) { return (u8)mOrderFlags & flag; }
    void ClrOrder() { mOrderFlags = 0; }
    void SetOrder(u8 v) { mOrderFlags = (u8)(mOrderFlags | v); }
    bool chkFlag(u16 flag) { return (mFlags & flag) == flag; }
    void clrFlag(u16 flag) { mFlags = (u16)(mFlags & ~flag); }
    void setFlag(u16 flag) { mFlags = (u16)(mFlags | flag); }
    bool isMorf(); /* mDoExt_McaMorf::isMorf: mCurMorf (+0xB0) < 1.0 */
    void setAction(u32 actionFunc); /* inline setAction(&daNpc_Mk_c::xxx_action, NULL) */

    BOOL initTexPatternAnm(u32); /* bool */
    void playTexPatternAnm();
    void setAnm(s8);
    void setAnmStatus();
    bool chkAttentionLocal();
    void chkAttention();
    void eventOrder();
    void checkOrder();
    u32 next_msgStatus(be<u32>*); /* u16 */
    u32 getMsg();
    void setCollision();
    void msgAnm(u8);
    void talkInit();
    void msgPushButton();
    u16 talk();
    BOOL checkDemoStart();
    s32 getNowEventAction();
    BOOL init();
    u8 getType();
    u8 getPath();
    u8 nextVisitMode();
    void visitInit(u8);
    void visitProc();
    void runawayInit();
    void walkPath(u8);
    void runLink();
    void aroundLink();
    void remove_Um2();
    void demoInitCom();
    bool demoProc();
    void setAttention(bool);
    u8 getLookBackMode();
    void lookBack();
    bool wait01();
    bool talk01();
    bool talk02();
    u8 visitTalkInit();
    void visitSetEvent();
    bool visit01();
    bool climb01();
    bool drop01();
    bool runaway();
    bool jump();
    bool jitanda01();
    bool jitanda02();
    bool demo01();
    bool demo02();
    bool demo03();
    BOOL wait_action(void*);
    BOOL seek_action(void*);
    BOOL hind_action(void*);
    BOOL visit_action(void*);
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateHeap();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3B8 */ gptr<J3DAnmTexPattern> m_head_tex_pattern; /* HD: no mShadowId before it */
    /* 0x3BC */ u8 mBtpAnm[0x74];                           /* mDoExt_btpAnm (HD 0x74) */
    /* 0x430 */ be<u8> mBlinkFrame;
    /* 0x431 */ u8 _431;
    /* 0x432 */ be<s16> mBlinkTimer;
    /* 0x434 */ dBgS_ObjAcch mObjAcch;
    /* 0x5F8 */ dBgS_AcchCir mAcchCir;
    /* 0x638 */ dCcD_Stts mStts;
    /* 0x674 */ dCcD_Cyl mCyl;
    /* 0x7A4 */ dNpc_JntCtrl_c m_jnt;
    /* 0x7D8 */ cXyz mEyePos;
    /* 0x7E4 */ cXyz mAttnBasePos;
    /* 0x7F0 */ be<s16> mMaxHeadTurnVelocity;
    /* 0x7F2 */ be<s8> mAnmEnded;
    /* 0x7F3 */ be<u8> mAttnSetCount;
    /* 0x7F4 */ be<f32> mAnmTimer;
    /* 0x7F8 */ be<u32> mCurrMsgNo;
    /* 0x7FC */ be<u16> mFlags;
    /* 0x7FE */ be<u8> mAttentionTimer;
    /* 0x7FF */ be<u8> mMsgSelectNum;
    /* 0x800 */ MkPath_l field_0x688; /* dNpc_PathRun_c */
    /* 0x808 */ MkPath_l field_0x690;
    /* 0x810 */ MkPath_l field_0x698;
    /* 0x818 */ be<u8> mVisitMode;
    /* 0x819 */ be<u8> mWaitTimer;
    /* 0x81A */ be<u8> mRunAroundLinkTimer;
    /* 0x81B */ be<u8> mTimerToReachLink;
    /* 0x81C */ be<u16> field_0x6A4;
    /* 0x81E */ u8 _81E[2];
    /* 0x820 */ MkActionFunc_l mCurrActionFunc;
    /* 0x828 */ be<s8> mTexPatternIdx;
    /* 0x829 */ be<s8> mAnmIdx;
    /* 0x82A */ u8 _82A[2];
    /* 0x82C */ be<s32> mStaffIdx;
    /* 0x830 */ be<s32> mEventAction;
    /* 0x834 */ be<u8> mOrderFlags;
    /* 0x835 */ be<s8> mState;
    /* 0x836 */ be<s8> mPrevState;
    /* 0x837 */ be<s8> mType;
    /* 0x838 */ be<s8> mActionStatus;
    /* 0x839 */ be<s8> mTalkState;
    /* 0x83A */ be<u8> mMsgAnmIdx;
    /* 0x83B */ u8 _83B;
    /* 0x83C */ be<u32> mMtrlSndId;
    /* 0x840 */ be<s8> mReverb;
    /* 0x841 */ u8 _841[3];
    /* 0x844 */ cXyz field_0x6D0;
    /* 0x850 */ cXyz field_0x6DC;
    /* 0x85C */ be<s16> field_0x6E8;
    /* 0x85E */ be<s16> mEventIdx;
    /* 0x860 */ be<u32> mpName;
    /* 0x864 */ be<s16> field_0x6F0;
    /* 0x866 */ u8 _866[2];
    /* 0x868 */ be<u32> mMsgNo;
    /* 0x86C */ daNpc_Mk_Static_c mMkStatic;
};
WWHD_OFFSET(daNpc_Mk_c, mBtpAnm, 0x3BC);
WWHD_OFFSET(daNpc_Mk_c, mObjAcch, 0x434);
WWHD_OFFSET(daNpc_Mk_c, mAcchCir, 0x5F8);
WWHD_OFFSET(daNpc_Mk_c, mStts, 0x638);
WWHD_OFFSET(daNpc_Mk_c, mCyl, 0x674);
WWHD_OFFSET(daNpc_Mk_c, m_jnt, 0x7A4);
WWHD_OFFSET(daNpc_Mk_c, mFlags, 0x7FC);
WWHD_OFFSET(daNpc_Mk_c, field_0x688, 0x800);
WWHD_OFFSET(daNpc_Mk_c, mCurrActionFunc, 0x820);
WWHD_OFFSET(daNpc_Mk_c, mOrderFlags, 0x834);
WWHD_OFFSET(daNpc_Mk_c, mMtrlSndId, 0x83C);
WWHD_OFFSET(daNpc_Mk_c, mpName, 0x860);
WWHD_OFFSET(daNpc_Mk_c, mMkStatic, 0x86C);
WWHD_SIZE(daNpc_Mk_c, 0x87C);

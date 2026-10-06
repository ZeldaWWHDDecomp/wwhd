/* daNpc_Uk_c (Jin, Jan and Jun-Roberto, the Killer Bees of Windfall), WWHD layout.
 *
 * The GameCube decompilation of this TU has only "Nonmatching" placeholders and an empty class,
 * so the whole layout is measured from the WWHD functions of d_a_npc_uk. daNpc_Uk_c derives from
 * fopAc_ac_c directly (members from 0x3AC); its HD vtable (0x10023050) holds only the virtual
 * destructor. Size 0x874 (profile g_profile_NPC_UK at 0x101C69C8). Names follow the sister
 * actor daNpc_Tt_c and the GameCube header's accessors. */
#pragma once
#include "d/actor/d_a_npc_ba1.h"       /* ProcFunc_l (GHS pointer to member function, 8 bytes) */
#include "d/actor/d_a_npc_mk_static.h" /* daNpc_Mk_Static_c, MkPath_l (dNpc_PathRun_c) */

struct daNpc_Uk_c : fopAc_ac_c {
    enum ActionStatus { ACTION_ENDING = -1, ACTION_STARTING = 0, ACTION_ONGOING = 1 };

    u32 ChkOrder(u8 flag) { return (u8)mOrderFlags & flag; }
    void ClrOrder() { mOrderFlags = 0; }
    void SetOrder(u8 flag) { mOrderFlags = (u8)(mOrderFlags | flag); }
    bool chkFlag(u16 flag) { return (mFlags & flag) != 0; }
    void clrFlag(u16 flag) { mFlags = (u16)(mFlags & ~flag); }
    void setFlag(u16 flag) { mFlags = (u16)(mFlags | flag); }
    f32 curMorf() { return gabi::load<f32>(gabi::ea(mpMorf.get()) + 0xB0); } /* mDoExt_McaMorf::mCurMorf */
    BOOL isMorf() { return curMorf() < 1.0f; }

    u8 getType();
    u8 getPath();
    u32 getShapeType();
    u16 getCaughtFlag();
    u16 getFoundFlag();
    u16 getFirstTalk();
    BOOL chkGameStart();
    BOOL chkPositioning(f32, f32, f32, s16, s16);
    u8 nextVisitMode();
    void approachRun(fopAc_ac_c*);
    void aroundWalk(fopAc_ac_c*);
    void surrender();
    void runawayInit();
    BOOL initTexPatternAnm(u32); /* bool, passed on unnormalised */
    void playTexPatternAnm();
    void setAnm(s8, u8);
    BOOL chkAttentionLocal();
    void chkAttention();
    void setAnmStatus();
    void eventOrder();
    void checkOrder();
    u32 next_msgStatus(be<u32>*); /* u16 */
    u32 getMsg();
    void setCollision();
    void msgAnm(u32); /* u8, compared unnormalised */
    void talkInit();
    u16 talk();
    BOOL init();
    void setAttention(u32); /* bool, tested unnormalised */
    u32 getLookBackMode();
    void lookBack();
    u32 getStaffName();
    BOOL checkDemoStart();
    s32 getNowEventAction();
    void visitInit(u32); /* u8, compared unnormalised */
    void visitProc();
    BOOL wait01();
    BOOL talk01();
    BOOL talk02();
    u32 visitTalkInit();
    void visitSetEvent();
    BOOL visit01();
    BOOL warningB();
    BOOL warningC();
    BOOL warningD();
    BOOL found();
    BOOL runaway();
    BOOL jump();
    BOOL jitanda01();
    BOOL jitanda02();
    BOOL demo01();
    BOOL demo02();
    void demoInitCom();
    BOOL demoProc();
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
    /* 0x3B8 */ gptr<J3DModel> mpHeadModel;             /* head model, placed on the head joint */
    /* 0x3BC */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x3C0 */ u8 mBtpAnm[0x74];                       /* mDoExt_btpAnm (HD 0x74) */
    /* 0x434 */ be<u8> mBlinkFrame;
    /* 0x435 */ u8 _435;
    /* 0x436 */ be<s16> mBlinkTimer;
    /* 0x438 */ dBgS_ObjAcch mObjAcch;
    /* 0x5FC */ dBgS_AcchCir mAcchCir;
    /* 0x63C */ dCcD_Stts mStts;
    /* 0x678 */ dCcD_Cyl mCyl;
    /* 0x7A8 */ dNpc_JntCtrl_c m_jnt;
    /* 0x7DC */ cXyz mEyePos;
    /* 0x7E8 */ cXyz mAttnBasePos;
    /* 0x7F4 */ be<s16> mMaxHeadTurnVelocity;
    /* 0x7F6 */ be<s8> mAnmEnded;
    /* 0x7F7 */ be<u8> mAttnSetCount;
    /* 0x7F8 */ be<f32> mAnmTimer;
    /* 0x7FC */ be<u32> mCurrMsgNo;
    /* 0x800 */ be<u16> mFlags;
    /* 0x802 */ be<u8> mTalkWait;          /* frames without attention after a talk */
    /* 0x803 */ u8 _803;
    /* 0x804 */ be<u32> mPartnerId;        /* process id of the partner actor */
    /* 0x808 */ be<f32> m808;
    /* 0x80C */ be<f32> m80C;
    /* 0x810 */ MkPath_l mPath;            /* dNpc_PathRun_c */
    /* 0x818 */ ProcFunc_l mCurrActionFunc;
    /* 0x820 */ be<s8> mTexPatternIdx;
    /* 0x821 */ be<s8> mCurrAnmIdx;
    /* 0x822 */ u8 _822[2];
    /* 0x824 */ be<s32> mStaffIdx;
    /* 0x828 */ be<s32> mEventAction;
    /* 0x82C */ be<u8> mOrderFlags;
    /* 0x82D */ be<s8> mState;
    /* 0x82E */ be<s8> mReturnToState;
    /* 0x82F */ be<u8> mVisitMode;
    /* 0x830 */ be<u8> mVisitTimer;
    /* 0x831 */ be<u8> mVisitTimer2;
    /* 0x832 */ be<u8> m832;
    /* 0x833 */ be<s8> mType;
    /* 0x834 */ be<s8> mActionStatus;
    /* 0x835 */ be<s8> mTalkState;
    /* 0x836 */ be<u8> mMsgAnmIdx;
    /* 0x837 */ u8 _837;
    /* 0x838 */ be<u32> mMtrlSndId;
    /* 0x83C */ be<s8> mReverb;
    /* 0x83D */ u8 _83D[3];
    /* 0x840 */ cXyz m840;
    /* 0x84C */ cXyz m84C;
    /* 0x858 */ be<s16> m858;
    /* 0x85A */ be<s16> mEventIdx;
    /* 0x85C */ be<s16> m85C;
    /* 0x85E */ u8 _85E[2];
    /* 0x860 */ be<u32> mMsgNo;
    /* 0x864 */ daNpc_Mk_Static_c mMk;
};
WWHD_OFFSET(daNpc_Uk_c, mPhs, 0x3AC);
WWHD_OFFSET(daNpc_Uk_c, mBtpAnm, 0x3C0);
WWHD_OFFSET(daNpc_Uk_c, mObjAcch, 0x438);
WWHD_OFFSET(daNpc_Uk_c, mAcchCir, 0x5FC);
WWHD_OFFSET(daNpc_Uk_c, mStts, 0x63C);
WWHD_OFFSET(daNpc_Uk_c, mCyl, 0x678);
WWHD_OFFSET(daNpc_Uk_c, m_jnt, 0x7A8);
WWHD_OFFSET(daNpc_Uk_c, mFlags, 0x800);
WWHD_OFFSET(daNpc_Uk_c, mCurrActionFunc, 0x818);
WWHD_OFFSET(daNpc_Uk_c, mOrderFlags, 0x82C);
WWHD_OFFSET(daNpc_Uk_c, m840, 0x840);
WWHD_OFFSET(daNpc_Uk_c, mMsgNo, 0x860);
WWHD_OFFSET(daNpc_Uk_c, mMk, 0x864);
WWHD_SIZE(daNpc_Uk_c, 0x874);

/* daNpc_Tt_c (Tott, the dancing NPC on Windfall), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_tt: daNpc_Tt_c derives from fopAc_ac_c
 * directly, so mHeadAnm is +0x11C (0x3AC). dNpc_HeadAnm_c holds a GHS pointer to member function
 * (8 bytes instead of 12), so it is 0x20 (GameCube 0x24) and mEventIdx..mpMorf are +0x118;
 * mShadowId (GameCube 0x2C4) is gone (HD shadows), so m_head_tex_pattern/mBtpAnm are +0x114;
 * mDoExt_btpAnm grew from 0x14 to 0x74, so mBlinkFrame..mAttnSetCount..mNoticeLinkTimer are
 * +0x174; mCurrActionFunc is an 8-byte pointer to member (GameCube 12), so mDanceStep and
 * mLineKe are +0x170; mDoExt_3DlineMat0_c grew from 0x1C to 0x148, so the members after it
 * are +0x29C. Size 0x1120 (GameCube 0xE84). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* ProcFunc_l (GHS pointer to member function, measured) */

/* dNpc_HeadAnm_c (HD 0x20): csXyz at 0, swing proc (pointer to member) at 8 */
struct dNpc_HeadAnm_tt {
    /* 0x00 */ csXyz field_0x00;
    /* 0x06 */ u8 _06[2];
    /* 0x08 */ ProcFunc_l mProc;
    /* 0x10 */ be<f32> field_0x14;
    /* 0x14 */ be<f32> field_0x18;
    /* 0x18 */ be<s16> field_0x1C;
    /* 0x1A */ be<s16> field_0x1E;
    /* 0x1C */ be<s16> field_0x20;
    /* 0x1E */ u8 _1E[2];
    /* 0259F36C */
    void swing_vertical_init(s16 a, s16 b, s16 c, s32 d) { gabi::call(0x0259F36C, this, a, b, c, d); }
    /* 0259F67C */
    void move() { gabi::call(0x0259F67C, this); }
};
WWHD_SIZE(dNpc_HeadAnm_tt, 0x20);

/* mDoExt_3DlineMat0_c (HD 0x148): vtable at +0x130, lines (0x10 each, position array first) at +0x144 */
struct mDoExt_3DlineMat0_tt {
    /* 0x000 */ u8 _000[0x130];
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 _134[0x144 - 0x134];
    /* 0x144 */ be<u32> mpLines;
};
WWHD_SIZE(mDoExt_3DlineMat0_tt, 0x148);

struct tt_ke_s {
    /* 0x00 */ cXyz field_0x00[10];
    /* 0x78 */ cXyz field_0x78[10];
    void ke_control(f32);
    void ke_pos_set(cXyz*);
};
WWHD_SIZE(tt_ke_s, 0xF0);

struct daNpc_Tt_c : fopAc_ac_c {
    enum ActionStatus { ACTION_ENDING = -1, ACTION_STARTING = 0, ACTION_ONGOING = 1 };
    enum TalkStates { TALK_FINISHED = -1, TALK_INIT = 0, TALK_MSG_CREATE = 1, TALK_ACTIVE = 2 };

    u32 ChkOrder(u8 flag) { return (u8)mOrderFlags & flag; }
    void ClrOrder() { mOrderFlags = 0; }
    void SetOrder(u8 flag) { mOrderFlags = (u8)(mOrderFlags | flag); }
    bool chkEvFlag(u8 f) { return (mEvFlags & f) == f; }
    void setEvFlag(u8 f) { mEvFlags = (u8)(mEvFlags | f); }
    void clrEvFlag(u8 f) { mEvFlags = (u8)(mEvFlags & ~f); }
    bool chkFlag(u16 flag) { return (mFlags & flag) == flag; }
    void clrFlag(u16 flag) { mFlags = (u16)(mFlags & ~flag); }
    void setFlag(u16 flag) { mFlags = (u16)(mFlags | flag); }
    f32 curMorf(); /* mDoExt_McaMorf +0xB0 (isMorf: < 1.0) */

    s16 XyCheckCB(int);
    void setAnmStatus();
    void eventOrder();
    void checkOrder();
    u32 next_msgStatus(be<u32>*); /* u16 */
    u32 getMsg();
    void setCollision();
    void talkInit();
    void msgAnm();
    void msgPushButton();
    void msgContinue();
    u16 talk();
    void demoInitChange();
    bool demoProcTact1();
    void demoInitPatten();
    bool demoProcPatten();
    void demoInitSpeak();
    bool demoProcSpeak();
    void demoInitWait();
    bool demoProcWait();
    void demoInitCom();
    void demoProcCom();
    s32 getNowEventAction();
    bool demoProc();
    BOOL init();
    void setAttention(bool);
    void lookBack();
    bool wait01();
    bool talk01();
    bool tact00();
    BOOL wait_action(void*);
    void set_ke_root(int, int, int);
    void ke_execute();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateHeap();
    BOOL initTexPatternAnm(u32); /* bool, passed on unnormalised */
    void playTexPatternAnm();
    void setAnm(int);
    void danceInit(int);
    BOOL danceProc();
    BOOL danceNext();

    /* 0x3AC */ dNpc_HeadAnm_tt mHeadAnm;
    /* 0x3CC */ be<s16> mEventIdx;
    /* 0x3CE */ u8 _3CE[2];
    /* 0x3D0 */ request_of_phase_process_class mPhs;
    /* 0x3D8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3DC */ gptr<J3DAnmTexPattern> m_head_tex_pattern; /* HD: no mShadowId before it */
    /* 0x3E0 */ u8 mBtpAnm[0x74];                           /* mDoExt_btpAnm (HD 0x74) */
    /* 0x454 */ be<u8> mBlinkFrame;
    /* 0x455 */ u8 _455;
    /* 0x456 */ be<s16> mBlinkTimer;
    /* 0x458 */ dBgS_ObjAcch mObjAcch;
    /* 0x61C */ dBgS_AcchCir mAcchCir;
    /* 0x65C */ dCcD_Stts mStts;
    /* 0x698 */ dCcD_Cyl mCyl;
    /* 0x7C8 */ be<s8> m_head_jnt_num;
    /* 0x7C9 */ be<s8> m_backbone_jnt_num;
    /* 0x7CA */ u8 _7CA[2];
    /* 0x7CC */ dNpc_JntCtrl_c m_jnt;
    /* 0x800 */ cXyz mEyePos;
    /* 0x80C */ cXyz mAttnBasePos;
    /* 0x818 */ be<s16> mMaxHeadTurnVelocity;
    /* 0x81A */ u8 _81A[2];
    /* 0x81C */ be<s8> mAnmEnded;
    /* 0x81D */ be<u8> mAttnSetCount;
    /* 0x81E */ u8 _81E[2];
    /* 0x820 */ be<f32> mAnmTimer;
    /* 0x824 */ be<u32> mCurrMsgNo;
    /* 0x828 */ be<u16> mFlags;
    /* 0x82A */ be<s16> mNoticeLinkTimer;
    /* 0x82C */ ProcFunc_l mCurrActionFunc;
    /* 0x834 */ be<u8> mDanceStep;
    /* 0x835 */ be<u8> mDanceStepTimer;
    /* 0x836 */ u8 _836[2];
    /* 0x838 */ tt_ke_s mLineKe[8];
    /* 0xFB8 */ mDoExt_3DlineMat0_tt mLineMat;
    /* 0x1100 */ be<s8> mTexPatternIdx;
    /* 0x1101 */ be<s8> mCurrAnmIdx;
    /* 0x1102 */ be<s8> mAnmLoopCount;
    /* 0x1103 */ be<u8> mOrderFlags;
    /* 0x1104 */ be<s8> mState;
    /* 0x1105 */ be<s8> mReturnToState;
    /* 0x1106 */ be<u8> mType;
    /* 0x1107 */ be<s8> mActionStatus;
    /* 0x1108 */ be<s8> mTalkState;
    /* 0x1109 */ be<u8> mMsgAnmIdx;
    /* 0x110A */ u8 _110A[2];
    /* 0x110C */ be<u32> mMtrlSndId;
    /* 0x1110 */ be<s8> mReverb;
    /* 0x1111 */ be<u8> mEvFlags;
    /* 0x1112 */ be<s16> mSoundTimer;
    /* 0x1114 */ be<s16> mTimer;
    /* 0x1116 */ be<u8> field_0xE7A;
    /* 0x1117 */ u8 _1117;
    /* 0x1118 */ be<u32> mMsgNo;
    /* 0x111C */ be<s32> mStaffIdx;
};
WWHD_OFFSET(daNpc_Tt_c, mEventIdx, 0x3CC);
WWHD_OFFSET(daNpc_Tt_c, mBtpAnm, 0x3E0);
WWHD_OFFSET(daNpc_Tt_c, mObjAcch, 0x458);
WWHD_OFFSET(daNpc_Tt_c, mCyl, 0x698);
WWHD_OFFSET(daNpc_Tt_c, m_jnt, 0x7CC);
WWHD_OFFSET(daNpc_Tt_c, mCurrActionFunc, 0x82C);
WWHD_OFFSET(daNpc_Tt_c, mLineKe, 0x838);
WWHD_OFFSET(daNpc_Tt_c, mLineMat, 0xFB8);
WWHD_OFFSET(daNpc_Tt_c, mTexPatternIdx, 0x1100);
WWHD_OFFSET(daNpc_Tt_c, mStaffIdx, 0x111C);
WWHD_SIZE(daNpc_Tt_c, 0x1120);

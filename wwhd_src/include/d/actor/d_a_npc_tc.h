/* daNpc_Tc_c (Tingle, Ankle, David Jr., Knuckle), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the WWHD code of d_a_npc_tc:
 * - base fopNpc_npc_c 0x7DC (d/d_npc.h), so members are +0x118 up to mStatus (0x7E7);
 * - the pointer-to-member mCurrActionFunc is 8 bytes (GHS) instead of 12: mActionStatus 0x7F0;
 * - HD-only s32 at 0x7F4 (a frame counter in statusDemoRescue/help_action: light tev colour);
 * - mSmokePos .. mPhs: +0x118 again (mSmokePos 0x7F8, mSmokeCallBack 0x80C, follow 0x82C/0x840,
 *   mPhs 0x854);
 * - mShadowId (GameCube 0x744) is gone (HD shadows): m_head_tex_pattern 0x85C;
 * - mDoExt_btpAnm grew from 0x14 to 0x74 (constructor 025E7820): everything from mBlinkFrame on
 *   is +0x174.
 * Size 0x990 (profile g_profile_NPC_TC at 0x101C6874, method table 0x101C67EC; GameCube 0x81C).
 * The HD vtable at 0xB4 is 0x10022D20 (stored by _create). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* ProcFunc_l (GHS pointer to member, 8 bytes) */
#include "d/d_npc.h"

struct dPa_smokeEcallBack_tc { /* 0x00 */ be<u32> __vtbl; /* 0x04 */ gptr<JPABaseEmitter> mpEmitter; /* 0x08 */ u8 _08[0x18]; };
WWHD_SIZE(dPa_smokeEcallBack_tc, 0x20);

struct daNpc_Tc_c : fopNpc_npc_c {
    enum Status {
        STATUS_WAIT = 1,
        STATUS_SIT = 2,
        STATUS_TALK = 3,
        STATUS_JUMP = 4,
        STATUS_WALK_TO_JAIL = 5,
        STATUS_TALK_NEAR_JAIL = 6,
        STATUS_WAIT_NEAR_JAIL = 7,
        STATUS_WALK_TO_STOOL = 8,
        STATUS_SIT_TO_STOOL = 9,
        STATUS_DEMO_JUMP = 10,
        STATUS_DEMO_RESCUE = 11,
        STATUS_DEMO_TALK = 12,
        STATUS_PAY_RUPEE = 13,
        STATUS_DEMO_PAY_RUPEE = 14,
        STATUS_GET_RUPEE = 15,
        STATUS_DEMO_GET_RUPEE = 16,
        STATUS_MONUMENT_COMPLETE = 17,
        STATUS_DEMO_MONUMENT_COMPLETE = 18,
    };
    enum ActionStatus { ACTION_STARTING = 0, ACTION_ONGOING = 1, ACTION_ENDING = -1 };
    enum Type { TYPE_NORMAL2 = 0, TYPE_RED = 1, TYPE_WHITE = 2, TYPE_BLUE = 3, TYPE_NORMAL = 4 };

    /* ---- methods (types follow the GameCube header; adjust to what the WWHD code reads) ---- */
    /* main (d_a_npc_tc.cpp) */
    fopAc_ac_c* _searchTower(fopAc_ac_c*);
    BOOL initTexPatternAnm(u32); /* bool, passed on unnormalised */
    void playTexPatternAnm();
    void smoke_set(f32, f32, f32, f32, f32);
    void setAnm();
    u32 setTexAnm();
    u8 chkAttention(cXyz*, s16); /* bool (stored as is); cXyz by value: GHS passes a pointer to a copy */
    void eventOrder();
    void checkOrder();
    void anmAtr(u16);
    void startTower();
    void getArg();
    void createInit();
    static void* searchStoolPos(void*, void*);
    void setAttention();
    void calcMove();
    void lookBack();
    void statusWait();
    void statusSit();
    void statusJump();
    void statusWalkToJail();
    void statusTalkNearJail();
    void statusWaitNearJail();
    void statusWalkToStool();
    void statusSitToStool();
    void statusTalk();
    void statusPayRupee();
    void statusDemoPayRupee();
    void statusGetRupee();
    void statusDemoGetRupee();
    void statusMonumentComplete();
    void statusDemoMonumentComplete();
    void statusDemoJump();
    void statusDemoRescue();
    void statusDemoTalk();
    BOOL help_action(void*);
    BOOL wait_action(void*);
    void calc_sitpos();
    void set_mtx();
    BOOL _draw();
    void setTower();
    BOOL _execute();
    BOOL _delete();
    bool isCreate();
    cPhs_State _create();
    BOOL _createHeap();
    /* messages and cuts (d_a_npc_tc_msg.cpp / d_a_npc_tc_cut.cpp) */
    bool existTcMonument();
    void stopTower();
    u16 next_msgStatusBlue(be<u32>*);
    u16 next_msgStatusNormal(be<u32>*);
    u16 next_msgStatus(be<u32>*);
    u32 setFirstMsg(be<u8>*, u32, u32);
    u32 getMsgNormal();
    u32 getMsgBlue();
    u32 getMsg();
    u16 next_msgStatusNormal2(be<u32>*);
    u32 getMsgNormal2();
    u32 setRupeeSizeMsg();
    bool existUnknownCollectMap();
    int analysisCollectMap();
    bool existKnownCollectMap();
    u16 next_msgStatusWhite(be<u32>*);
    u32 getMsgWhite();
    u16 next_msgStatusRed(be<u32>*);
    u32 setFinishMsg();
    u32 getMsgRed();
    bool checkMonumentFee(u16, u16);
    bool checkAllMonumentFee();
    int checkAllMonumentPay();
    bool checkMonumentPay(u16, u16);
    void cutProc();
    void cutSitToJumpStart(int);
    void cutSitToJumpProc(int);
    void cutPresentStart(int);
    void cutPresentProc(int);
    void cutSetAnmStart(int);
    void cutSetAnmProc(int);
    void cutBackJumpStart(int);
    void cutBackJumpProc(int);
    void cutEffectStart(int);
    void cutEffectProc(int);
    void cutDoorOpenStart(int);
    void cutDoorOpenProc(int);
    void cutDoorCloseStart(int);
    void cutDoorCloseProc(int);
    void cutDoorClose2Start(int);
    void cutDoorClose2Proc(int);
    void cutPayStart(int);
    void cutPayProc(int);

    /* ---- layout ---- */
    /* 0x7DC */ u8 field_0x6C4[4];
    /* 0x7E0 */ be<s32> field_0x6C8;            /* BOOL (anmAtr) */
    /* 0x7E4 */ be<u8> mJtbtTimer;
    /* 0x7E5 */ be<u8> mTalk01Wait01Timer;
    /* 0x7E6 */ be<u8> mTalk01Talk02Timer;
    /* 0x7E7 */ be<s8> mStatus;
    /* 0x7E8 */ ProcFunc_l mCurrActionFunc;     /* HD: 8 bytes (GameCube 12) */
    /* 0x7F0 */ be<s8> mActionStatus;
    /* 0x7F1 */ u8 _7F1[3];
    /* 0x7F4 */ be<s32> field_0x7F4_hd;         /* HD-only counter (statusDemoRescue, help_action) */
    /* 0x7F8 */ cXyz mSmokePos;
    /* 0x804 */ csXyz mSmokeAngle;
    /* 0x80A */ u8 _80A[2];
    /* 0x80C */ dPa_smokeEcallBack_tc mSmokeCallBack;
    /* 0x82C */ dPa_followEcallBack field_0x714;
    /* 0x840 */ dPa_followEcallBack field_0x728;
    /* 0x854 */ request_of_phase_process_class mPhs;
    /* 0x85C */ gptr<J3DAnmTexPattern> m_head_tex_pattern; /* HD: no mShadowId before it */
    /* 0x860 */ u8 mBtpAnm[0x74];                           /* mDoExt_btpAnm (HD 0x74) */
    /* 0x8D4 */ be<u8> mBlinkFrame;
    /* 0x8D5 */ u8 _8D5;
    /* 0x8D6 */ be<s16> mBlinkTimer;
    /* 0x8D8 */ cXyz mEyePos;
    /* 0x8E4 */ cXyz mAttPos;
    /* 0x8F0 */ be<u8> mAttnSetCount;
    /* 0x8F1 */ u8 _8F1[3];
    /* 0x8F4 */ cXyz mStoolLookPos;
    /* 0x900 */ be<s16> mMaxHeadTurnVelocity;
    /* 0x902 */ u8 _902[2];
    /* 0x904 */ csXyz field_0x790;
    /* 0x90A */ u8 _90A[2];
    /* 0x90C */ be<s8> field_0x798;
    /* 0x90D */ u8 _90D[3];
    /* 0x910 */ be<f32> mTargetSpeed;
    /* 0x914 */ cXyz mStoolPos;
    /* 0x920 */ cXyz mSitPos;
    /* 0x92C */ cXyz mWalkToStoolPos;
    /* 0x938 */ cXyz mDeltaPos;
    /* 0x944 */ u8 field_0x7D0[0xC];
    /* 0x950 */ cXyz mParticlePos;
    /* 0x95C */ be<u8> mHasAttention;
    /* 0x95D */ be<u8> mTalkingNearJail;
    /* 0x95E */ u8 field_0x7EA[0x965 - 0x95E];
    /* 0x965 */ be<u8> mHasTalkedNearJail;
    /* 0x966 */ be<u8> mHasEnteredSitRadius;
    /* 0x967 */ be<s8> mJumpLandingTimer;
    /* 0x968 */ u8 field_0x7F4[2];
    /* 0x96A */ be<s8> mTexPatternNum;
    /* 0x96B */ be<s8> mTexPatternNumIdx;
    /* 0x96C */ be<s8> mBckIdx;
    /* 0x96D */ be<s8> mAnmPrmIdx;
    /* 0x96E */ be<s8> mOldAnmPrmIdx;
    /* 0x96F */ be<s8> mEventIdx;
    /* 0x970 */ be<u8> mType;
    /* 0x971 */ u8 field_0x7FD[0x978 - 0x971];
    /* 0x978 */ gptr<fopAc_ac_c> m_tower_actor;  /* daObjSmplbg::Act_c* */
    /* 0x97C */ be<u8> field_0x808;
    /* 0x97D */ be<u8> field_0x809;
    /* 0x97E */ u8 field_0x80A;
    /* 0x97F */ be<u8> field_0x80B;
    /* 0x980 */ be<u8> field_0x80C;
    /* 0x981 */ be<u8> field_0x80D;
    /* 0x982 */ be<u8> field_0x80E;
    /* 0x983 */ be<u8> field_0x80F;
    /* 0x984 */ be<u8> field_0x810;
    /* 0x985 */ be<u8> field_0x811;
    /* 0x986 */ be<u8> field_0x812;
    /* 0x987 */ be<u8> field_0x813;
    /* 0x988 */ be<u8> field_0x814;
    /* 0x989 */ u8 field_0x815[0x990 - 0x989];
};
WWHD_OFFSET(daNpc_Tc_c, mCurrActionFunc, 0x7E8);
WWHD_OFFSET(daNpc_Tc_c, mSmokeCallBack, 0x80C);
WWHD_OFFSET(daNpc_Tc_c, mPhs, 0x854);
WWHD_OFFSET(daNpc_Tc_c, mBlinkFrame, 0x8D4);
WWHD_OFFSET(daNpc_Tc_c, mParticlePos, 0x950);
WWHD_OFFSET(daNpc_Tc_c, mType, 0x970);
WWHD_OFFSET(daNpc_Tc_c, m_tower_actor, 0x978);
WWHD_SIZE(daNpc_Tc_c, 0x990);

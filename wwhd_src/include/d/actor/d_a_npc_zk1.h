/* daNpc_Zk1_c (Zuko, the Rito postman at the Dragon Roost aerie), WWHD layout.
 *
 * GameCube -> WWHD, measured from the verified functions of d_a_npc_zk1 (the GameCube source is
 * all "Nonmatching" stubs; only m72C/m750 are named there): fopNpc_npc_c is 0x7DC (see
 * d_a_npc_ba1.h / d/d_npc.h). The archive name is a member (mArcName, "Zk", copied by
 * decideType) and is used as the resource key everywhere. mDoExt_btpAnm is 0x74 (constructor
 * 025E7820) and the pointer to member function 8 bytes, so the members after it are +0x150
 * from GameCube (m72C -> mInitPos 0x87C). No dNpc_EventCut_c member: the fopNpc_npc_c one (0x3E0)
 * is used. Size 0x924 (profile g_profile_NPC_ZK1 at 0x101C7098; GameCube 0x7B4). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* fopNpc_npc_l, ProcFunc_l (shared NPC base, measured) */

struct daNpc_Zk1_c : fopNpc_npc_l {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;
        /* 0x01 */ be<s8> mBtpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    void _nodeCB_Head(J3DNode*, J3DModel*);
    void _nodeCB_BackBone(J3DNode*, J3DModel*);
    s32 btpResID(int);
    bool setBtp(s32, u32);           /* (s8, bool): r4 is tested as a word, r5 passed on */
    u32 init_texPttrnAnm(s32, u32);  /* tail call to setBtp: registers passed through */
    BOOL bodyCreateHeap();
    BOOL CreateHeap();
    bool decideType(int);
    BOOL set_action(ProcFunc_l*, void*);
    bool init_ZK1_0();
    void play_btp_anm();
    void play_animation();
    void setAttention(u32);          /* bool, tested as a word */
    void setMtx(u32);                /* bool, passed on */
    bool createInit();
    cPhs_State _create();
    BOOL _delete();
    void checkOrder();
    u8 demo();                       /* bool */
    s32 isEventEntry();
    void endEvent();
    void privateCut(int);
    void event_proc(int);
    fopAc_ac_c* searchByID(fpc_ProcID, be<s32>*);
    void lookBack();
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    s32 bckResID(int);
    void setAnm_anm(anm_prm_c*);
    void setAnm();
    void setAnm_ATR();
    void chngAnmAtr(u32);            /* u8, compared as a word */
    void anmAtr(u32);                /* u16, compared as a word */
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_ZK1_0();
    u32 getMsg();
    bool chk_talk();
    u8 chk_parts_notMov();           /* bool */
    u8 chkAttention();               /* bool */
    void setStt(u32);                /* s8, compared as a word */
    BOOL wait_1();
    u32 talk_1();                    /* returns chk_parts_notMov()'s register */
    BOOL wait_action1(void*);

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ char mArcName[3];                         /* HD: "Zk" (decideType) */
    /* 0x7E9 */ u8 _7E9[3];
    /* 0x7EC */ u8 mBtpAnm[0x74];                         /* mDoExt_btpAnm (HD 0x74); +0x10 the J3DAnmTexPattern* */
    /* 0x860 */ be<u8> mBlinkFrame;
    /* 0x861 */ u8 _861;
    /* 0x862 */ be<s16> mBlinkTimer;
    /* 0x864 */ ProcFunc_l mCurrProcFunc;
    /* 0x86C */ u8 _86C[4];
    /* 0x870 */ be<u32> mPartnerID;                       /* lookBack state 4 */
    /* 0x874 */ u8 _874[8];
    /* 0x87C */ cXyz mInitPos;                            /* GameCube m72C */
    /* 0x888 */ csXyz mInitAngle;
    /* 0x88E */ csXyz mAngle;                             /* model rotation (setMtx) */
    /* 0x894 */ u8 _894[8];
    /* 0x89C */ cXyz mEyePos;                             /* head joint * a_eye_pos_off */
    /* 0x8A8 */ cXyz mLookTarget;
    /* 0x8B4 */ u8 _8B4[0xC];
    /* 0x8C0 */ cXyz mHeadPos;                            /* head joint translation */
    /* 0x8CC */ u8 _8CC[0xC];
    /* 0x8D8 */ be<f32> mPrevMorfFrame;
    /* 0x8DC */ u8 _8DC[4];
    /* 0x8E0 */ be<s16> mActorAngleY;
    /* 0x8E2 */ be<s16> mJointHeadY;
    /* 0x8E4 */ be<s16> mJointBackboneY;
    /* 0x8E6 */ u8 _8E6[2];
    /* 0x8E8 */ be<s32> mbSetEyePos;                      /* wait_1 / talk_1 result */
    /* 0x8EC */ be<s16> mEventIdTable[1];
    /* 0x8EE */ be<s16> mEventIdx;
    /* 0x8F0 */ u8 _8F0[8];
    /* 0x8F8 */ be<s16> mTimer;
    /* 0x8FA */ u8 _8FA[8];
    /* 0x902 */ be<s16> mTargetYRot;
    /* 0x904 */ be<s8> mbMorfAnimStopped;
    /* 0x905 */ be<u8> m905;
    /* 0x906 */ be<u8> mItemNo;
    /* 0x907 */ be<u8> m907;                              /* event bit 0x1802 (init_ZK1_0) */
    /* 0x908 */ be<u8> m908;                              /* event bit 0x1C01 (init_ZK1_0) */
    /* 0x909 */ u8 _909;
    /* 0x90A */ be<u8> m90A;                              /* run event_proc */
    /* 0x90B */ be<u8> mbResLoaded;                       /* _create: resLoad == complete */
    /* 0x90C */ be<u8> m90C;                              /* no execute/draw (unless in a demo) */
    /* 0x90D */ be<u8> m90D;                              /* keep shape_angle */
    /* 0x90E */ be<u8> m90E;                              /* no draw */
    /* 0x90F */ be<u8> mbRanExecute;
    /* 0x910 */ be<u8> mbAttention;
    /* 0x911 */ be<u8> m911;                              /* talk ordered (checkOrder) */
    /* 0x912 */ be<u8> mHeadOnlyFollow;
    /* 0x913 */ be<u8> m913;                              /* turn back to mInitAngle */
    /* 0x914 */ be<u8> mbInDemo;
    /* 0x915 */ be<s8> mActIdx;
    /* 0x916 */ be<u8> mAnmAtr;
    /* 0x917 */ be<u8> mMesgAnimeTagInfo;
    /* 0x918 */ be<s8> mBtpNum;
    /* 0x919 */ be<s8> mAnmNum;
    /* 0x91A */ be<s8> mEvtCond;                          /* 1/2 talk, >= 3 event mEventIdTable[n - 3] */
    /* 0x91B */ be<s8> mStatus;
    /* 0x91C */ be<s8> mPrevStatus;
    /* 0x91D */ be<s8> mLookBackState;
    /* 0x91E */ be<s8> mType;
    /* 0x91F */ be<s8> mSpecificType;
    /* 0x920 */ be<s8> mActState;                         /* set_action: 0 init, 9 leaving */
    /* 0x921 */ be<s8> m921;
    /* 0x922 */ u8 _922[2];
};
WWHD_OFFSET(daNpc_Zk1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Zk1_c, mBtpAnm, 0x7EC);
WWHD_OFFSET(daNpc_Zk1_c, mCurrProcFunc, 0x864);
WWHD_OFFSET(daNpc_Zk1_c, mInitPos, 0x87C);
WWHD_OFFSET(daNpc_Zk1_c, mEyePos, 0x89C);
WWHD_OFFSET(daNpc_Zk1_c, mPrevMorfFrame, 0x8D8);
WWHD_OFFSET(daNpc_Zk1_c, mbSetEyePos, 0x8E8);
WWHD_OFFSET(daNpc_Zk1_c, mTargetYRot, 0x902);
WWHD_OFFSET(daNpc_Zk1_c, mbInDemo, 0x914);
WWHD_OFFSET(daNpc_Zk1_c, m921, 0x921);
WWHD_SIZE(daNpc_Zk1_c, 0x924);

/* daNpc_Gp1_c (Maggie's father, rich), WWHD layout. 
 *
 * The GameCube decompilation has no layout (all functions are "Nonmatching" stubs); the members
 * below are measured from the WWHD code of d_a_npc_gp1 and named after their use. The class
 * derives from fopNpc_npc_c (0x7DC, see d_a_npc_ba1.h). Size 0x988 (profile g_profile_NPC_GP1,
 * size word at 0x101BE6D0). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* fopNpc_npc_l, ProcFunc_l (shared NPC base, measured) */

/* dNpc_PathRun_c (8 bytes, as on GameCube) */
struct dNpc_PathRun_gp1 {
    /* 0x00 */ gptr<dPath> mPath;
    /* 0x04 */ be<u8> field_0x04;
    /* 0x05 */ be<u8> mIdx;
    /* 0x06 */ be<u8> mbDir;
    /* 0x07 */ be<u8> field_0x07;
};
WWHD_SIZE(dNpc_PathRun_gp1, 8);

struct daNpc_Gp1_c : fopNpc_npc_l {
    struct anm_prm_c {
        /* 0x00 */ be<s8> bckNum;
        /* 0x01 */ be<s8> btpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> morf;
        /* 0x08 */ be<f32> speed;
        /* 0x0C */ be<s32> loopMode;
    };

    void nodeGp1Control(J3DNode*, J3DModel*);
    J3DModelData* create_Anm();
    int btpNum_toResID(int);
    u32 setBtp(u32, int);
    u32 iniTexPttrnAnm(u32);
    BOOL CreateHeap();
    BOOL charDecide(int);
    BOOL set_action(ProcFunc_l*, void*);
    bool init_GP1_0();
    void plyTexPttrnAnm();
    u32 setAnm_tex(s8);
    int anmNum_toResID(int);
    BOOL setAnm_anm(anm_prm_c*);
    void setAnm_NUM(int, int);
    void ctrl_WAITanm();
    void setAttention(u32);
    void setMtx(u32);
    bool createInit();
    cPhs_State _create();
    BOOL _delete();
    fopAc_ac_c* searchByID(u32);
    u8 partner_srch_sub(u32);
    void partner_srch();
    void checkOrder();
    u8 demo();
    s32 isEventEntry();
    BOOL setAnm();
    void setStt(s8);
    void endEvent();
    void eInit_INI_KAERE_KAERE_();
    void eInit_END_KAERE_KAERE_();
    void event_actionInit(int);
    BOOL event_action();
    void privateCut(int);
    void lookBack();
    void event_proc(int);
    void gp_clcMovSpd();
    int gp_movPass();
    void gp_nMove();
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    void setAnm_ATR(int);
    void chg_anmAtr(u8);
    BOOL create_rupee();
    void control_anmAtr();
    void anmAtr(u16);
    u8 chk_talk();
    u8 chk_partsNotMove();
    u8 chk_forceTlkArea();
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_GP1_0();
    u32 getMsg();
    u8 chkAttention();
    BOOL wait_1();
    u8 talk_1();
    BOOL walk_1();
    BOOL hair_1();
    BOOL wait_2();
    BOOL wait_action1(void*);

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ be<s8> m_hnd_L_jnt_num;             /* rupees are thrown from the left hand */
    /* 0x7E7 */ u8 _7E7;
    /* 0x7E8 */ gptr<J3DAnmTexPattern> m_hed_tex_pttrn;
    /* 0x7EC */ u8 mBtpAnm[0x74];                   /* mDoExt_btpAnm (HD 0x74) */
    /* 0x860 */ be<u8> mBtpFrame;
    /* 0x861 */ u8 _861;
    /* 0x862 */ be<s16> mBlinkTimer;
    /* 0x864 */ ProcFunc_l mCurrProcFunc;
    /* 0x86C */ dNpc_PathRun_gp1 mPathRun;
    /* 0x874 */ dNpc_EventCut_c mEventCut;          /* hides fopNpc_npc_c::mEventCut */
    /* 0x8E0 */ be<u32> mPartnerID;                 /* the Bm (fpcNm 0x149) found by partner_srch */
    /* 0x8E4 */ cXyz mHomePos;
    /* 0x8F0 */ csXyz mHomeAngle;
    /* 0x8F6 */ csXyz mAngle;
    /* 0x8FC */ cXyz mEyePos;                       /* head joint * (24, -24, 0) */
    /* 0x908 */ cXyz mLookPos;
    /* 0x914 */ cXyz mPathPos;                      /* current path point */
    /* 0x920 */ u8 _920[0x92C - 0x920];
    /* 0x92C */ be<f32> mPrevMorfFrame;
    /* 0x930 */ be<f32> mEyeOffsetY;
    /* 0x934 */ u8 _934[4];
    /* 0x938 */ be<f32> mWalkSpd;
    /* 0x93C */ be<f32> mWalkAccel;
    /* 0x940 */ be<f32> mPassDist;
    /* 0x944 */ be<s16> mJointHeadY;
    /* 0x946 */ be<s16> mJointBackboneY;
    /* 0x948 */ be<s16> mActorAngleY;
    /* 0x94A */ be<s16> mEventIDTbl[3];
    /* 0x950 */ be<s16> mEventIndex;
    /* 0x952 */ u8 _952[2];
    /* 0x954 */ be<s16> mTalkTimer;
    /* 0x956 */ be<s16> mWaitTimer2;
    /* 0x958 */ be<s16> mHairTimer;
    /* 0x95A */ be<s16> mLookVel;
    /* 0x95C */ be<s16> mLookAngleY;
    /* 0x95E */ be<s16> mBuyNum;                    /* skull necklaces handed over */
    /* 0x960 */ be<s16> mRupeeNum;                  /* rupees to throw */
    /* 0x962 */ be<s16> mWaitTimer;
    /* 0x964 */ be<s8> mbMorfAnimStopped;
    /* 0x965 */ be<s8> mLoopCnt;
    /* 0x966 */ be<s8> mbAfterEvent;
    /* 0x967 */ be<u8> mItemNo;
    /* 0x968 */ be<u8> m968;
    /* 0x969 */ be<u8> mbNoShapeAngle;
    /* 0x96A */ be<u8> mbNoMove;
    /* 0x96B */ be<u8> mbNoDraw;
    /* 0x96C */ be<u8> mbPathEnd;
    /* 0x96D */ be<u8> mbPresent;
    /* 0x96E */ be<u8> mbHomeSet;
    /* 0x96F */ u8 _96F;
    /* 0x970 */ be<s32> mbSetEyePos;
    /* 0x974 */ be<u8> mbAttention;
    /* 0x975 */ be<u8> mbTalk;
    /* 0x976 */ be<u8> mbHeadOnly;
    /* 0x977 */ be<u8> mbDemo;
    /* 0x978 */ be<s8> mMoveState;
    /* 0x979 */ be<s8> mActionIndex;
    /* 0x97A */ be<s8> mActNo;
    /* 0x97B */ be<u8> mAnmAtr;
    /* 0x97C */ be<u8> mMesgAnimeTag;
    /* 0x97D */ be<s8> mBtpNum;
    /* 0x97E */ be<s8> mBckNum;
    /* 0x97F */ be<s8> mEvtState;
    /* 0x980 */ be<s8> mStt;
    /* 0x981 */ be<s8> mPrevStt;
    /* 0x982 */ be<s8> mLookMode;
    /* 0x983 */ be<s8> m983;
    /* 0x984 */ be<s8> mType;
    /* 0x985 */ be<s8> mActState;
    /* 0x986 */ be<s8> mAtrSet;
    /* 0x987 */ u8 _987;
};
WWHD_OFFSET(daNpc_Gp1_c, mBtpAnm, 0x7EC);
WWHD_OFFSET(daNpc_Gp1_c, mCurrProcFunc, 0x864);
WWHD_OFFSET(daNpc_Gp1_c, mEventCut, 0x874);
WWHD_OFFSET(daNpc_Gp1_c, mPartnerID, 0x8E0);
WWHD_OFFSET(daNpc_Gp1_c, mEyePos, 0x8FC);
WWHD_OFFSET(daNpc_Gp1_c, mPrevMorfFrame, 0x92C);
WWHD_OFFSET(daNpc_Gp1_c, mEventIDTbl, 0x94A);
WWHD_OFFSET(daNpc_Gp1_c, mbSetEyePos, 0x970);
WWHD_OFFSET(daNpc_Gp1_c, mAtrSet, 0x986);
WWHD_SIZE(daNpc_Gp1_c, 0x988);

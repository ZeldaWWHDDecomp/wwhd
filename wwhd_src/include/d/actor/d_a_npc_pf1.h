/* daNpc_Pf1_c (Maggie's father, poor), WWHD layout. 
 *
 * The GameCube decompilation has no layout (all functions are "Nonmatching" stubs); the members
 * below are measured from the WWHD code of d_a_npc_pf1 and named after their use (the same
 * family as d_a_npc_aj1). The class derives from fopNpc_npc_c (0x7DC, see d_a_npc_ba1.h).
 * Members are read up to 0x92E. */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* fopNpc_npc_l, ProcFunc_l (shared NPC base, measured) */

/* dNpc_PathRun_c (8 bytes, as on GameCube) */
struct dNpc_PathRun_pf1 {
    /* 0x00 */ gptr<dPath> mPath;
    /* 0x04 */ be<u8> field_0x04;
    /* 0x05 */ be<u8> mIdx;
    /* 0x06 */ be<u8> mbDir;
    /* 0x07 */ be<u8> field_0x07;
};
WWHD_SIZE(dNpc_PathRun_pf1, 8);

struct daNpc_Pf1_c : fopNpc_npc_l {
    struct anm_prm_c {
        /* 0x00 */ be<s8> bckNum;
        /* 0x01 */ be<s8> btpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> morf;
        /* 0x08 */ be<f32> speed;
        /* 0x0C */ be<s32> loopMode;
    };

    void _nodeCB_Head(J3DNode*, J3DModel*);
    void _nodeCB_BackBone(J3DNode*, J3DModel*);
    bool init_PF1_0();
    bool createInit();
    void play_animation();
    void setMtx(u32);
    int bckResID(int);
    int btpResID(int);
    u32 setBtp(s8, u32);
    u32 init_texPttrnAnm(s8, u32);
    void play_btp_anm();
    void setAnm_anm(anm_prm_c*);
    void setAnm_NUM(int, int);
    void setAnm();
    void chngAnmAtr(u8);
    void ctrlAnmAtr();
    void setAnm_ATR();
    void anmAtr(u16);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_PF1_0();
    u32 getMsg();
    void eventOrder();
    void checkOrder();
    BOOL chk_talk();
    u8 chk_parts_notMov();
    fopAc_ac_c* searchByID(fpc_ProcID, be<s32>*);
    void lookBack();
    u8 chkAttention();
    void setAttention(u32);
    u8 decideType(int);
    void privateCut(int);
    void endEvent();
    s32 isEventEntry();
    void event_proc(int);
    BOOL set_action(ProcFunc_l*, void*);
    void setStt(s8);
    void createTama(f32);
    BOOL chk_areaIN(f32, cXyz*);
    BOOL endEvent_check();
    BOOL startEvent_check();
    void set_pthPoint(u32); /* u8, passed on unextended */
    BOOL chk_attn();
    void setBikon(cXyz*);
    BOOL regret();
    BOOL attk_1();
    BOOL walk_1();
    BOOL wait_2();
    BOOL wait_3();
    BOOL talk_1();
    BOOL wait_action1(void*);
    u8 demo();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL bodyCreateHeap();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ char mArcName[6];                   /* "Pf" (decideType copies 3 bytes) */
    /* 0x7EC */ u8 mBtpAnm[0x74];                   /* mDoExt_btpAnm (HD 0x74); +0x10 the J3DAnmTexPattern */
    /* 0x860 */ be<u8> mBtpFrame;
    /* 0x861 */ u8 _861;
    /* 0x862 */ be<s16> mBlinkTimer;
    /* 0x864 */ ProcFunc_l mCurrProcFunc;
    /* 0x86C */ u8 _86C[4];
    /* 0x870 */ be<u32> mLookActorId;               /* lookBack mode 4: looks at this actor */
    /* 0x874 */ dNpc_PathRun_pf1 mPathRun;
    /* 0x87C */ cXyz mHomePos;
    /* 0x888 */ csXyz mHomeAngle;
    /* 0x88E */ csXyz mAngle;
    /* 0x894 */ u8 _894[8];
    /* 0x89C */ cXyz mEyePos;                       /* head joint * (20, -30, 0) */
    /* 0x8A8 */ cXyz mLookPos;
    /* 0x8B4 */ u8 _8B4[0xC];
    /* 0x8C0 */ cXyz mHeadPos;                      /* the attention arrow is above the head */
    /* 0x8CC */ u8 _8CC[0xC];
    /* 0x8D8 */ cXyz mGndNormal;
    /* 0x8E4 */ be<f32> mPrevMorfFrame;
    /* 0x8E8 */ u8 _8E8[4];
    /* 0x8EC */ be<s16> mActorAngleY;
    /* 0x8EE */ be<s16> mJointHeadY;
    /* 0x8F0 */ be<s16> mJointBackboneY;
    /* 0x8F2 */ u8 _8F2[2];
    /* 0x8F4 */ be<s32> mbSetEyePos;
    /* 0x8F8 */ be<s16> mEventIDTbl[1];
    /* 0x8FA */ be<s16> mEventIndex;
    /* 0x8FC */ u8 _8FC[0xA];
    /* 0x906 */ be<s16> mAttkTimer;
    /* 0x908 */ be<s16> mTimer;
    /* 0x90A */ u8 _90A[4];
    /* 0x90E */ be<s16> mLookAngleY;
    /* 0x910 */ be<s8> mbMorfAnimStopped;
    /* 0x911 */ be<s8> mAtrLoopCnt;
    /* 0x912 */ be<u8> mItemNo;
    /* 0x913 */ u8 _913;
    /* 0x914 */ be<u8> m914;                        /* set when message 0x1B60 ends */
    /* 0x915 */ be<u8> mbTalked;                    /* message 0x1B59 answered */
    /* 0x916 */ u8 _916;
    /* 0x917 */ be<u8> mbResLoaded;
    /* 0x918 */ be<u8> m918;                        /* no execute/draw while not in a demo */
    /* 0x919 */ u8 _919;
    /* 0x91A */ be<u8> mbNoShapeAngle;
    /* 0x91B */ be<u8> mbNoDraw;
    /* 0x91C */ be<u8> mbHomeSet;
    /* 0x91D */ be<u8> mbAttention;
    /* 0x91E */ be<u8> mbTalk;
    /* 0x91F */ be<u8> mbHeadOnly;
    /* 0x920 */ be<u8> mbFar;
    /* 0x921 */ be<u8> mbDemo;
    /* 0x922 */ be<s8> mActionIndex;
    /* 0x923 */ be<u8> mAnmAtr;
    /* 0x924 */ be<u8> mMesgAnimeTag;
    /* 0x925 */ be<s8> mBtpNum;
    /* 0x926 */ be<s8> mBckNum;
    /* 0x927 */ be<s8> mEvtState;
    /* 0x928 */ be<s8> mStt;
    /* 0x929 */ be<s8> mPrevStt;
    /* 0x92A */ be<s8> mLookMode;
    /* 0x92B */ be<s8> mType;
    /* 0x92C */ be<s8> mSubType;
    /* 0x92D */ be<s8> mActState;
    /* 0x92E */ be<s8> mAtrSet;
    /* 0x92F */ u8 _92F;
};
WWHD_OFFSET(daNpc_Pf1_c, mArcName, 0x7E6);
WWHD_OFFSET(daNpc_Pf1_c, mBtpAnm, 0x7EC);
WWHD_OFFSET(daNpc_Pf1_c, mCurrProcFunc, 0x864);
WWHD_OFFSET(daNpc_Pf1_c, mPathRun, 0x874);
WWHD_OFFSET(daNpc_Pf1_c, mEyePos, 0x89C);
WWHD_OFFSET(daNpc_Pf1_c, mHeadPos, 0x8C0);
WWHD_OFFSET(daNpc_Pf1_c, mPrevMorfFrame, 0x8E4);
WWHD_OFFSET(daNpc_Pf1_c, mbSetEyePos, 0x8F4);
WWHD_OFFSET(daNpc_Pf1_c, mTimer, 0x908);
WWHD_OFFSET(daNpc_Pf1_c, mbMorfAnimStopped, 0x910);
WWHD_OFFSET(daNpc_Pf1_c, mAtrSet, 0x92E);

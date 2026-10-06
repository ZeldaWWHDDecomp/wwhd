/* daNpc_Hi1_c (the King of Hyrule), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the verified functions of d_a_npc_hi1: fopNpc_npc_c is 0x7DC
 * (see d_a_npc_ba1.h). mShadowId (GameCube 0x6D4) is gone (HD shadows), so mBtkAnm starts at
 * +0x114; mDoExt_btkAnm and mDoExt_btpAnm grew from 0x14 to 0x74 each (constructors 025E7C6C /
 * 025E7820), so mBtpAnm is +0x174 and mBtpAnmFrame/mTimer1/mCurrActionFunc +0x1D4; the pointer
 * to member function is 8 bytes (GHS) instead of 12, so everything after it is +0x1D0.
 * Size 0x99C (profile g_profile_NPC_HI1 at 0x101BE7FC; GameCube 0x7CC). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* fopNpc_npc_l, ProcFunc_l (shared NPC base, measured) */

struct daNpc_Hi1_c : fopNpc_npc_l {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;
        /* 0x01 */ be<s8> mTexNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    void _nodeCB_Head(J3DNode*, J3DModel*);
    void _nodeCB_BackBone(J3DNode*, J3DModel*);
    s32 btpResID(int);
    s32 btkResID(int);
    bool setBtp(s32, u32);           /* (s8, bool) */
    bool setBtk(s32, u32);           /* (s8, bool) */
    bool init_texPttrnAnm(s32, u32); /* (s8, bool) */
    BOOL bodyCreateHeap();
    BOOL CreateHeap();
    bool decideType(int);
    BOOL set_action(ProcFunc_l*, void*);
    bool init_HI1_0();
    void play_btp_anm();
    void play_btk_anm();
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
    void setAnm_NUM(int, int);
    void setAnm();
    void setAnm_ATR();
    void chngAnmAtr(u32);            /* u8, compared as a word */
    void anmAtr(u32);                /* u16, compared as a word */
    u16 next_msgStatus(be<u32>*);
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
    /* 0x7E6 */ char mArcName[3];                         /* "Hi" (decideType) */
    /* 0x7E9 */ u8 _7E9[3];
    /* 0x7EC */ u8 mBtkAnm[0x74];                         /* mDoExt_btkAnm (HD 0x74); +0x68 the J3DAnmTextureSRTKey* */
    /* 0x860 */ be<u8> mBtkAnmFrame;
    /* 0x861 */ u8 _861[3];
    /* 0x864 */ u8 mBtpAnm[0x74];                         /* mDoExt_btpAnm (HD 0x74); +0x10 the J3DAnmTexPattern* */
    /* 0x8D8 */ be<u8> mBtpAnmFrame;
    /* 0x8D9 */ u8 _8D9;
    /* 0x8DA */ be<s16> mTimer1;                          /* blink timer */
    /* 0x8DC */ ProcFunc_l mCurrActionFunc;
    /* 0x8E4 */ u8 _8E4[4];
    /* 0x8E8 */ be<u32> mPId;                             /* lookBack state 4 */
    /* 0x8EC */ u8 _8EC[8];
    /* 0x8F4 */ cXyz mInitPos;                            /* GameCube field_0x724 */
    /* 0x900 */ csXyz mInitAngle;                         /* field_0x730 */
    /* 0x906 */ csXyz mAngle;                             /* field_0x736: model rotation (setMtx) */
    /* 0x90C */ u8 _90C[8];
    /* 0x914 */ cXyz mEyePos;                             /* field_0x744: head joint * a_eye_pos_off */
    /* 0x920 */ cXyz mLookTarget;                         /* field_0x750 */
    /* 0x92C */ u8 _92C[0xC];
    /* 0x938 */ cXyz mHeadPos;                            /* field_0x768: head joint translation */
    /* 0x944 */ u8 _944[0xC];
    /* 0x950 */ be<f32> mFrame;
    /* 0x954 */ u8 _954[4];
    /* 0x958 */ be<s16> mActorAngleY;                     /* field_0x788.x */
    /* 0x95A */ be<s16> mJointHeadY;                      /* field_0x788.y */
    /* 0x95C */ be<s16> mJointBackboneY;                  /* field_0x788.z */
    /* 0x95E */ u8 _95E[2];
    /* 0x960 */ be<s32> field_0x790;                      /* wait_1 / talk_1 result: set eyePos */
    /* 0x964 */ be<s16> mEventIdx[1];
    /* 0x966 */ be<s16> field_0x796;                      /* current event index */
    /* 0x968 */ u8 _968[8];
    /* 0x970 */ be<s16> mTimer2;
    /* 0x972 */ u8 _972[8];
    /* 0x97A */ be<s16> field_0x7AA;                      /* lookBack state 3 target */
    /* 0x97C */ be<s8> field_0x7AC;                       /* animation ended */
    /* 0x97D */ be<u8> field_0x7AD;
    /* 0x97E */ be<u8> mItemNo;
    /* 0x97F */ u8 _97F;
    /* 0x980 */ be<u8> field_0x7B0;                       /* run event_proc */
    /* 0x981 */ be<u8> mStateIsComplaete;
    /* 0x982 */ be<u8> field_0x7B2;                       /* no execute/draw (unless in a demo) */
    /* 0x983 */ be<u8> field_0x7B3;                       /* keep shape_angle */
    /* 0x984 */ be<u8> field_0x7B4;                       /* no draw */
    /* 0x985 */ be<u8> field_0x7B5;                       /* initial position saved */
    /* 0x986 */ be<u8> mHasAttention;
    /* 0x987 */ be<u8> field_0x7B7;                       /* talk ordered */
    /* 0x988 */ be<u8> field_0x7B8;                       /* lookAtTarget_2 head only */
    /* 0x989 */ be<u8> field_0x7B9;                       /* turn back to mInitAngle */
    /* 0x98A */ be<u8> field_0x7BA;                       /* in a demo */
    /* 0x98B */ be<s8> mActIdx;
    /* 0x98C */ be<u8> field_0x7BC;                       /* animation attribute */
    /* 0x98D */ be<u8> field_0x7BD;                       /* message animation tag */
    /* 0x98E */ be<s8> field_0x7BE;                       /* btp number */
    /* 0x98F */ be<s8> field_0x7BF;                       /* btk number */
    /* 0x990 */ be<s8> field_0x7C0;                       /* animation number */
    /* 0x991 */ be<s8> field_0x7C1;                       /* 1/2 talk, >= 3 event mEventIdx[n - 3] */
    /* 0x992 */ be<s8> field_0x7C2;                       /* status */
    /* 0x993 */ be<s8> field_0x7C3;                       /* previous status */
    /* 0x994 */ be<s8> field_0x7C4;                       /* lookBack state */
    /* 0x995 */ be<s8> field_0x7C5;                       /* type */
    /* 0x996 */ be<s8> field_0x7C6;                       /* specific type */
    /* 0x997 */ be<s8> field_0x7C7;                       /* set_action: 0 init, 9 leaving */
    /* 0x998 */ be<s8> field_0x7C8;
    /* 0x999 */ u8 _999[3];
};
WWHD_OFFSET(daNpc_Hi1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Hi1_c, mBtkAnm, 0x7EC);
WWHD_OFFSET(daNpc_Hi1_c, mBtpAnm, 0x864);
WWHD_OFFSET(daNpc_Hi1_c, mCurrActionFunc, 0x8DC);
WWHD_OFFSET(daNpc_Hi1_c, mInitPos, 0x8F4);
WWHD_OFFSET(daNpc_Hi1_c, mEyePos, 0x914);
WWHD_OFFSET(daNpc_Hi1_c, mFrame, 0x950);
WWHD_OFFSET(daNpc_Hi1_c, field_0x790, 0x960);
WWHD_OFFSET(daNpc_Hi1_c, mTimer2, 0x970);
WWHD_OFFSET(daNpc_Hi1_c, field_0x7B0, 0x980);
WWHD_OFFSET(daNpc_Hi1_c, field_0x7C8, 0x998);
WWHD_SIZE(daNpc_Hi1_c, 0x99C);

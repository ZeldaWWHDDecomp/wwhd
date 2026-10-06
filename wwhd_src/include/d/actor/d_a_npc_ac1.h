/* daNpc_Ac1_c (Rito postman with wings/arms; NPC "Ac1"), WWHD layout.
 *
 * GameCube -> WWHD, measured from the verified functions of d_a_npc_ac1 (all GameCube functions
 * are "Nonmatching" stubs, so the names below follow the GameCube header's offsets and the
 * usage in WWHD's code):
 * - fopNpc_npc_c is 0x7DC (shared d/d_npc.h), so the members start at 0x7DC (GameCube 0x6C4,
 *   +0x118).
 * - the GameCube word at 0x6D0 (mShadowID in the sibling NPCs) is gone: m_handR_jnt_num and the
 *   item model are at 0x7E8/0x7EC (GameCube 0x6D0/0x6D4 area), +0x114 from the tex pattern on.
 * - mDoExt_btpAnm grew from 0x14 to 0x74, so everything after it is +0x174; the pointer to
 *   member mCurrProcFunc is 8 bytes (GHS) instead of 12, so from the matrices on +0x170.
 * - size 0x9EC (profile; GameCube 0x87C + 0x170). */
#pragma once
#include "bindings.h"

/* GHS pointer to member function (8 bytes): this adjustment, virtual index (0: null, < 0: not
 * virtual), then the function address, or (virtual) the vtable pointer's offset at +6 */
struct ProcFunc_l {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
WWHD_SIZE(ProcFunc_l, 8);

/* dNpc_PathRun_c (8 bytes) */
struct dNpc_PathRun_l {
    /* 0x0 */ be<u32> mPath;
    /* 0x4 */ be<u8> mCurrPointIndex;
    /* 0x5 */ be<u8> field_0x5;
    /* 0x6 */ u8 _6[2];
};
WWHD_SIZE(dNpc_PathRun_l, 8);

struct daNpc_Ac1_c : fopNpc_npc_c {
    /* HD: 0x14 bytes (the GameCube struct is empty in the decompilation) */
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;
        /* 0x01 */ be<s8> mBtpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
        /* 0x10 */ be<s32> mbArm; /* 1: the arm animation set (arm morf), else the wing set */
    };

    void nodeWngControl(J3DNode*, J3DModel*);
    void nodeArmControl(J3DNode*, J3DModel*);
    void nodeAc1Control(J3DNode*, J3DModel*);
    J3DModelData* create_Anm();
    int btpNum_toResID(int);
    bool setBtp(u32, int);
    u32 iniTexPttrnAnm(u32);
    J3DModelData* create_wng_Anm();
    J3DModelData* create_arm_Anm();
    bool create_itm_Mdl();
    BOOL CreateHeap();
    bool charDecide(int);
    BOOL set_action(ProcFunc_l*, void*);
    bool init_AC1_0();
    void plyTexPttrnAnm();
    void setAttention(u32);
    void setMtx(u32);
    bool createInit();
    cPhs_State _create();
    BOOL _delete();
    void checkOrder();
    u8 demo();
    s32 isEventEntry();
    void event_actionInit(int);
    bool event_action();
    void privateCut(int);
    void lookBack();
    void event_proc(int);
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    int anmNum_toResID(int);
    int armAnmNum_toResID(int);
    u32 wingAnmNum_toResID(int);
    u32 setAnm_tex(s8);
    BOOL setAnm_anm(anm_prm_c*);
    bool setAnm();
    void setAnm_ATR(int);
    void chg_anmAtr(u8);
    void anmAtr(u16);
    bool chk_talk();
    u8 chk_partsNotMove();
    u16 next_msgStatus(be<u32>*);
    u32 getBitMask();
    u32 getMsg_AC1_0();
    u32 getMsg();
    u8 chkAttention();
    void endEvent();
    void setStt(s8);
    BOOL wait_1();
    BOOL talk_1();
    BOOL wait_action1(void*);

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ be<s8> m_armL_jnt_num;
    /* 0x7E7 */ be<s8> m_armR_jnt_num;
    /* 0x7E8 */ be<s8> m_handR_jnt_num;
    /* 0x7E9 */ u8 _7E9[3];
    /* 0x7EC */ gptr<J3DModel> mpItemModel;               /* GameCube m6D4 */
    /* 0x7F0 */ gptr<J3DAnmTexPattern> m_hed_tex_pttrn;   /* GameCube m6DC */
    /* 0x7F4 */ u8 mBtpAnm[0x74];                         /* mDoExt_btpAnm (HD 0x74) */
    /* 0x868 */ be<u8> mBlinkFrame;                       /* GameCube m6F4 */
    /* 0x869 */ u8 _869;
    /* 0x86A */ be<s16> mBlinkTimer;
    /* 0x86C */ u8 _86C[4];
    /* 0x870 */ gptr<mDoExt_McaMorf> mpWngMorf;           /* GameCube m6FC */
    /* 0x874 */ be<s8> m_wngL_jnt_num;                    /* "wingL_loc" */
    /* 0x875 */ be<s8> m_wngR_jnt_num;                    /* "wingR_loc" */
    /* 0x876 */ u8 _876[6];
    /* 0x87C */ gptr<mDoExt_McaMorf> mpArmMorf;           /* GameCube m708 */
    /* 0x880 */ be<s8> m_armL_loc_jnt_num;                /* "armL_loc" */
    /* 0x881 */ be<s8> m_armR_loc_jnt_num;                /* "armR_loc" */
    /* 0x882 */ u8 _882[2];
    /* 0x884 */ ProcFunc_l mCurrProcFunc;
    /* 0x88C */ Mtx34 mArmLMtx;                           /* the body's armL joint matrix */
    /* 0x8BC */ Mtx34 mArmRMtx;                           /* the body's armR joint matrix */
    /* 0x8EC */ dNpc_PathRun_l mPathRun;                  /* GameCube 0x77C */
    /* 0x8F4 */ dNpc_EventCut_c mEventCut;                /* hides fopNpc_npc_c::mEventCut (as on GameCube) */
    /* 0x960 */ u8 _960[4];
    /* 0x964 */ cXyz mInitialPos;                         /* GameCube m7F4 */
    /* 0x970 */ csXyz mInitialAngle;                      /* GameCube m800 */
    /* 0x976 */ u8 _976[2];
    /* 0x978 */ cXyz mTransformedEyePos;
    /* 0x984 */ cXyz m984;                                /* GameCube m814: the position at creation */
    /* 0x990 */ u8 _990[0x99C - 0x990];
    /* 0x99C */ be<f32> mPrevMorfFrame;                   /* GameCube m82C */
    /* 0x9A0 */ u8 _9A0[0x9B4 - 0x9A0];
    /* 0x9B4 */ be<s16> mJointHeadY;
    /* 0x9B6 */ be<s16> mJointBackboneY;
    /* 0x9B8 */ be<s16> mActorAngleY;
    /* 0x9BA */ u8 _9BA[2];
    /* 0x9BC */ be<s16> mEvTimer2;                        /* GameCube m84C */
    /* 0x9BE */ u8 _9BE[4];
    /* 0x9C2 */ be<s16> mLookAtMaxVel;                    /* GameCube m852 */
    /* 0x9C4 */ be<s16> mTargetYRot;                      /* GameCube m854 */
    /* 0x9C6 */ be<u16> mMsgStatus;                       /* HD: the message status seen by talk_1 */
    /* 0x9C8 */ be<s8> mbMorfAnimStopped;                 /* GameCube m858 */
    /* 0x9C9 */ be<u8> m9C9;
    /* 0x9CA */ u8 _9CA;
    /* 0x9CB */ be<u8> mItemNo;
    /* 0x9CC */ u8 _9CC;
    /* 0x9CD */ be<u8> m9CD;                              /* GameCube m85D: no execute/draw */
    /* 0x9CE */ be<u8> mbArmAnm;                          /* GameCube m85E: the arm animation set is active */
    /* 0x9CF */ be<u8> m9CF;                              /* GameCube m85F */
    /* 0x9D0 */ be<u8> m9D0;                              /* GameCube m860 */
    /* 0x9D1 */ be<u8> m9D1;                              /* GameCube m861 */
    /* 0x9D2 */ u8 _9D2;
    /* 0x9D3 */ be<u8> mbRanExecute;                      /* GameCube m863 */
    /* 0x9D4 */ be<s32> mbSetEyePos;
    /* 0x9D8 */ be<u8> mbAttention;
    /* 0x9D9 */ be<u8> mbTalkReq;                         /* GameCube m869 */
    /* 0x9DA */ be<u8> mHeadOnlyFollow;
    /* 0x9DB */ be<u8> mbInDemo;                          /* GameCube m86B */
    /* 0x9DC */ be<s8> mActionIndex;
    /* 0x9DD */ be<s8> mActNo;
    /* 0x9DE */ be<u8> mAnmAtr;                           /* GameCube m86E */
    /* 0x9DF */ be<u8> mMesgAnimeTagInfo;
    /* 0x9E0 */ be<s8> mBtpNum;                           /* GameCube m870 */
    /* 0x9E1 */ be<s8> mAnmNum;                           /* GameCube m871 */
    /* 0x9E2 */ u8 _9E2[2];
    /* 0x9E4 */ be<s8> mTalkState;                        /* GameCube m874 */
    /* 0x9E5 */ be<s8> mStatus;                           /* GameCube m875 */
    /* 0x9E6 */ be<s8> mPrevStatus;                       /* GameCube m876 */
    /* 0x9E7 */ be<s8> mLookBackState;                    /* GameCube m877 */
    /* 0x9E8 */ be<s8> mType;
    /* 0x9E9 */ be<s8> mSpecificType;                     /* GameCube m879 */
    /* 0x9EA */ be<s8> mActState;                         /* set_action: 9 in the old action, 0 in the new */
    /* 0x9EB */ be<s8> m9EB;
};
WWHD_OFFSET(daNpc_Ac1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Ac1_c, mBtpAnm, 0x7F4);
WWHD_OFFSET(daNpc_Ac1_c, mpWngMorf, 0x870);
WWHD_OFFSET(daNpc_Ac1_c, mCurrProcFunc, 0x884);
WWHD_OFFSET(daNpc_Ac1_c, mPathRun, 0x8EC);
WWHD_OFFSET(daNpc_Ac1_c, mEventCut, 0x8F4);
WWHD_OFFSET(daNpc_Ac1_c, mInitialPos, 0x964);
WWHD_OFFSET(daNpc_Ac1_c, mPrevMorfFrame, 0x99C);
WWHD_OFFSET(daNpc_Ac1_c, mEvTimer2, 0x9BC);
WWHD_OFFSET(daNpc_Ac1_c, mbSetEyePos, 0x9D4);
WWHD_OFFSET(daNpc_Ac1_c, m9EB, 0x9EB);
WWHD_SIZE(daNpc_Ac1_c, 0x9EC);

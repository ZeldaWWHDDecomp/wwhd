/* daNpc_Gk1_c (Mila's father, poor), WWHD layout. 
 *
 * The GameCube decompilation has no layout (all functions are "Nonmatching" stubs); the members
 * below are measured from the WWHD code of d_a_npc_gk1 and named after their use (the family of
 * d_a_npc_aj1). The class derives from fopNpc_npc_c (0x7DC, see d_a_npc_ba1.h). Members are read
 * up to 0x925. The archive name "Gk" is a member string (0x7F0) copied in decideType. */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* fopNpc_npc_l, ProcFunc_l (shared NPC base, measured) */

struct daNpc_Gk1_c : fopNpc_npc_l {
    struct anm_prm_c {
        /* 0x00 */ be<s8> bckNum;
        /* 0x01 */ be<s8> btpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> morf;
        /* 0x08 */ be<f32> speed;
        /* 0x0C */ be<s32> loopMode;
    };

    void _nodeCB_Head(J3DNode*, J3DModel*);
    void _nodeCB_Neck(J3DNode*, J3DModel*);
    void _nodeCB_BackBone(J3DNode*, J3DModel*);
    bool init_GK1_0();
    bool createInit();
    void play_animation();
    void setMtx(u32);
    int bckResID(int);
    int btpResID(int);
    u32 setBtp(s8, u32);
    u32 init_texPttrnAnm(s8, u32);
    void play_btp_anm();
    void setAnm_anm(anm_prm_c*);
    void setAnm();
    void chngAnmAtr(u8);
    void setAnm_ATR();
    void anmAtr(u16);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_GK1_0();
    u32 getMsg();
    void eventOrder();
    void checkOrder();
    u8 chk_talk();
    u8 chk_parts_notMov();
    fopAc_ac_c* searchByID(u32, be<s32>*);
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
    u8 chk_attn();
    BOOL wait_1();
    BOOL talk_1();
    BOOL wait_action1(void*);
    u8 demo();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL bodyCreateHeap();
    BOOL itemCreateHeap();
    BOOL hat_CreateHeap();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ be<s8> m_nck_jnt_num;
    /* 0x7E7 */ u8 _7E7;
    /* 0x7E8 */ gptr<J3DModel> mpItemModel;         /* "Gk" res 5, on the head joint */
    /* 0x7EC */ gptr<J3DModel> mpHatModel;          /* "Gk" res 6, on the head joint */
    /* 0x7F0 */ char mArcName[4];                   /* "Gk" (decideType) */
    /* 0x7F4 */ u8 mBtpAnm[0x74];                   /* mDoExt_btpAnm (HD 0x74); the J3DAnmTexPattern* at +0x10 */
    /* 0x868 */ be<u8> mBtpFrame;
    /* 0x869 */ u8 _869;
    /* 0x86A */ be<s16> mBlinkTimer;
    /* 0x86C */ ProcFunc_l mCurrProcFunc;
    /* 0x874 */ u8 _874[4];
    /* 0x878 */ be<u32> mPartnerProcId;             /* looked at in lookBack mode 4 */
    /* 0x87C */ u8 _87C[8];
    /* 0x884 */ cXyz mHomePos;
    /* 0x890 */ csXyz mHomeAngle;
    /* 0x896 */ csXyz mAngle;
    /* 0x89C */ u8 _89C[8];
    /* 0x8A4 */ cXyz mEyePos;                       /* head joint * (30, 30, 0) */
    /* 0x8B0 */ cXyz mLookPos;
    /* 0x8BC */ u8 _8BC[0xC];
    /* 0x8C8 */ cXyz mHeadPos;
    /* 0x8D4 */ u8 _8D4[0xC];
    /* 0x8E0 */ be<f32> mPrevMorfFrame;
    /* 0x8E4 */ u8 _8E4[4];
    /* 0x8E8 */ be<s16> mActorAngleY;
    /* 0x8EA */ be<s16> mJointHeadY;
    /* 0x8EC */ be<s16> mJointBackboneY;
    /* 0x8EE */ u8 _8EE[2];
    /* 0x8F0 */ be<s32> mbSetEyePos;
    /* 0x8F4 */ be<s16> mEventIDTbl[1];
    /* 0x8F6 */ be<s16> mEventIndex;
    /* 0x8F8 */ u8 _8F8[8];
    /* 0x900 */ be<s16> mTalkEndTimer;
    /* 0x902 */ u8 _902[8];
    /* 0x90A */ be<s16> mLookAngleY;
    /* 0x90C */ be<s8> mbMorfAnimStopped;
    /* 0x90D */ be<u8> m90D;
    /* 0x90E */ be<u8> mItemNo;
    /* 0x90F */ be<u8> mbResLoaded;                 /* _create: dComIfG_resLoad == complete */
    /* 0x910 */ be<u8> m910;                        /* no execute/draw outside demos */
    /* 0x911 */ be<u8> mbNoShapeAngle;
    /* 0x912 */ be<u8> mbNoDraw;
    /* 0x913 */ be<u8> mbHomeSet;
    /* 0x914 */ be<u8> mbAttention;
    /* 0x915 */ be<u8> mbTalk;
    /* 0x916 */ be<u8> mbHeadOnly;
    /* 0x917 */ be<u8> mbFar;                       /* wait_1: player farther than 300 */
    /* 0x918 */ be<u8> mbDemo;
    /* 0x919 */ be<s8> mActionIndex;
    /* 0x91A */ be<u8> mAnmAtr;
    /* 0x91B */ be<u8> mMesgAnimeTag;
    /* 0x91C */ be<s8> mBtpNum;
    /* 0x91D */ be<s8> mBckNum;
    /* 0x91E */ be<s8> mEvtState;
    /* 0x91F */ be<s8> mStt;
    /* 0x920 */ be<s8> mPrevStt;
    /* 0x921 */ be<s8> mLookMode;
    /* 0x922 */ be<s8> mTypeSet;                    /* decideType: > 0 when decided */
    /* 0x923 */ be<s8> mType;
    /* 0x924 */ be<s8> mActState;
    /* 0x925 */ be<s8> mAtrSet;
    /* 0x926 */ u8 _926[2];
};
WWHD_OFFSET(daNpc_Gk1_c, mArcName, 0x7F0);
WWHD_OFFSET(daNpc_Gk1_c, mBtpAnm, 0x7F4);
WWHD_OFFSET(daNpc_Gk1_c, mCurrProcFunc, 0x86C);
WWHD_OFFSET(daNpc_Gk1_c, mEyePos, 0x8A4);
WWHD_OFFSET(daNpc_Gk1_c, mPrevMorfFrame, 0x8E0);
WWHD_OFFSET(daNpc_Gk1_c, mbSetEyePos, 0x8F0);
WWHD_OFFSET(daNpc_Gk1_c, mLookAngleY, 0x90A);
WWHD_OFFSET(daNpc_Gk1_c, mAtrSet, 0x925);

/* daNpc_Aj1_c (Sturgeon, Outset), WWHD layout. 
 *
 * The GameCube decompilation has no layout (all functions are "Nonmatching" stubs); the members
 * below are measured from the WWHD code of d_a_npc_aj1 and named after their use. The class
 * derives from fopNpc_npc_c (0x7DC, see d_a_npc_ba1.h). Members are read up to 0x931 (the
 * profile's size word 0x8C8 is, as for ls1 (0x950 for 0xA28), not the HD object size). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* fopNpc_npc_l, ProcFunc_l (shared NPC base, measured) */

struct daNpc_Aj1_c : fopNpc_npc_l {
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
    u32 _XyCheckCB(int);
    bool init_AJ1_0();
    bool init_AJ1_1();
    bool init_AJ1_2();
    bool createInit();
    void play_animation();
    void ctrl_WAITanm();
    void ctrl_TIREanm();
    void setMtx(u32);
    int bckResID(int);
    int btpResID(int);
    u32 init_texPttrnAnm(s8, u32);
    void play_texPttrnAnm();
    void setAnm_anm(anm_prm_c*);
    void setAnm_NUM(int, int);
    void setAnm();
    void chngAnmAtr(u8);
    void ctrlAnmAtr();
    void setAnm_ATR();
    void anmAtr(u16);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_AJ1_0();
    u32 getMsg_AJ1_1();
    u32 getMsg_AJ1_2();
    u32 getMsg();
    void eventOrder();
    void checkOrder();
    u8 chk_talk();
    u8 chk_parts_notMov();
    void lookBack();
    u8 chkAttention();
    void setAttention(u32);
    u8 decideType(int);
    void cut_init_AJ1_TLK();
    u32 cut_move_AJ1_TLK();
    void cut_init_INI_ANGRY();
    u32 cut_move_VIVRATE();
    u32 cut_move_JMP();
    u32 cut_move_SPPRISE();
    u32 cut_move_LOK();
    u32 cut_move_DAN();
    void cut_init_INVIT();
    void privateCut(int);
    void endEvent();
    s32 isEventEntry();
    void event_proc(int);
    void set_pa_pun();
    void set_pa_aka();
    void set_pa_don();
    void set_pa_smk();
    void flw_pa_pun();
    void flw_pa_aka();
    void del_pa_aka();
    void del_pa(gptr<JPABaseEmitter>*);
    void setSmoke();
    BOOL set_action(ProcFunc_l*, void*);
    void setStt(s8);
    u8 chk_areaIN(f32, s16, cXyz*);
    BOOL FARwai();
    BOOL call_1();
    BOOL wait_1();
    BOOL talk_1();
    BOOL wait_action1(void*);
    BOOL wait_action2(void*);
    u8 demo();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL bodyCreateHeap();
    BOOL itemCreateHeap();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ be<s8> m_hnd_L_jnt_num;
    /* 0x7E7 */ be<s8> m_foot_L_jnt_num;
    /* 0x7E8 */ gptr<J3DModel> mpItemModel;         /* "Aj" res 9, carried on handL */
    /* 0x7EC */ u8 mBtpAnm[0x74];                   /* mDoExt_btpAnm (HD 0x74) */
    /* 0x860 */ be<u8> mBtpFrame;
    /* 0x861 */ u8 _861;
    /* 0x862 */ be<s16> mBlinkTimer;
    /* 0x864 */ ProcFunc_l mCurrProcFunc;
    /* 0x86C */ cXyz mHomePos;
    /* 0x878 */ csXyz mHomeAngle;
    /* 0x87E */ csXyz mAngle;
    /* 0x884 */ cXyz mEyePos;                       /* head joint * (24, -16, 0) */
    /* 0x890 */ cXyz mLookPos;
    /* 0x89C */ cXyz mHeadPos;
    /* 0x8A8 */ be<s16> mActorAngleY;
    /* 0x8AA */ be<s16> mJointHeadY;
    /* 0x8AC */ be<s16> mJointBackboneY;
    /* 0x8AE */ u8 _8AE[2];
    /* 0x8B0 */ be<f32> mPrevMorfFrame;
    /* 0x8B4 */ be<s16> mEventIDTbl[1];
    /* 0x8B6 */ be<s16> mEventIndex;
    /* 0x8B8 */ be<s16> mWaitTimer;
    /* 0x8BA */ be<s16> mPunTimer;
    /* 0x8BC */ be<s16> mCutTimer;
    /* 0x8BE */ be<s16> mTalkEndTimer;
    /* 0x8C0 */ be<s16> mFarTimer;
    /* 0x8C2 */ u8 _8C2[2];
    /* 0x8C4 */ be<s16> mLookVel;
    /* 0x8C6 */ be<s16> mLookAngleY;
    /* 0x8C8 */ be<s8> mbMorfAnimStopped;
    /* 0x8C9 */ be<u8> m8C9;
    /* 0x8CA */ be<u8> mItemNo;
    /* 0x8CB */ be<u8> mSwitchNo;
    /* 0x8CC */ be<u8> mTireTimer;
    /* 0x8CD */ u8 _8CD;
    /* 0x8CE */ be<u8> m8CE;
    /* 0x8CF */ be<u8> mbNoShapeAngle;
    /* 0x8D0 */ be<u8> mbNoDraw;
    /* 0x8D1 */ be<u8> mbHomeSet;
    /* 0x8D2 */ u8 _8D2[2];
    /* 0x8D4 */ be<s32> mbSetEyePos;
    /* 0x8D8 */ be<u8> mbAttention;
    /* 0x8D9 */ be<u8> mbTalk;
    /* 0x8DA */ be<u8> mbHeadOnly;
    /* 0x8DB */ be<u8> mbDemo;
    /* 0x8DC */ u8 mSmokeCB[0x20];                  /* dPa_smokeEcallBack (constructor 025A5B18(this, 1)) */
    /* 0x8FC */ gptr<JPABaseEmitter> mpSmokeEmitter;
    /* 0x900 */ gptr<JPABaseEmitter> mpPunEmitter;
    /* 0x904 */ gptr<JPABaseEmitter> mpAkaEmitter;
    /* 0x908 */ gptr<JPABaseEmitter> mpDonEmitter;
    /* 0x90C */ cXyz mSmokePos;
    /* 0x918 */ cXyz mDonPos;
    /* 0x924 */ be<u8> mbAkaStop;
    /* 0x925 */ be<s8> mActionIndex;
    /* 0x926 */ be<u8> mAnmAtr;
    /* 0x927 */ be<u8> mMesgAnimeTag;
    /* 0x928 */ be<s8> mBtpNum;
    /* 0x929 */ be<s8> mBckNum;
    /* 0x92A */ be<s8> mEvtState;
    /* 0x92B */ be<s8> mStt;
    /* 0x92C */ be<s8> mPrevStt;
    /* 0x92D */ be<s8> mLookMode;
    /* 0x92E */ be<s8> m92E;
    /* 0x92F */ be<s8> mType;
    /* 0x930 */ be<s8> mActState;
    /* 0x931 */ be<s8> mAtrSet;
    /* 0x932 */ u8 _932[2];
};
WWHD_OFFSET(daNpc_Aj1_c, mBtpAnm, 0x7EC);
WWHD_OFFSET(daNpc_Aj1_c, mCurrProcFunc, 0x864);
WWHD_OFFSET(daNpc_Aj1_c, mEyePos, 0x884);
WWHD_OFFSET(daNpc_Aj1_c, mPrevMorfFrame, 0x8B0);
WWHD_OFFSET(daNpc_Aj1_c, mbSetEyePos, 0x8D4);
WWHD_OFFSET(daNpc_Aj1_c, mpSmokeEmitter, 0x8FC);
WWHD_OFFSET(daNpc_Aj1_c, mbAkaStop, 0x924);
WWHD_OFFSET(daNpc_Aj1_c, mAtrSet, 0x931);

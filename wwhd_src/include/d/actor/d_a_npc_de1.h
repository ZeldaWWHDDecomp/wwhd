/* daNpc_De1_c (the Great Deku Tree), WWHD layout. 
 *
 * The GameCube header has no members (all functions are "Nonmatching" stubs), so the layout is
 * measured from the WWHD code (the inline constructor in _create and the field accesses of all
 * the unit's functions). Fields without a known meaning are named after their offset.
 * Base: fopNpc_npc_c (0x7DC in HD). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* ProcFunc_l */

struct daNpc_De1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;
        /* 0x01 */ u8 _01[3];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    BOOL CreateHeap();
    BOOL decideType(int);
    BOOL set_action(ProcFunc_l*, void*);
    void followPa_happa();
    void setDemoStartCenter();
    fopAc_ac_c* searchByID(u32);
    void cc_set();
    void setAttention();
    void setMtx();
    BOOL createInit();
    cPhs_State _create();
    void del_pa_happa();
    BOOL _delete();
    void checkOrder();
    u8 demo();
    void endEvent();
    BOOL anmResID(u32, be<s32>*, be<s32>*);
    void set_pa_happa();
    BOOL setAnm_anm(anm_prm_c*);
    BOOL setAnm();
    void setStt(u32); /* s8, compared unnormalised */
    BOOL setAnm_NUM(int);
    void event_actionInit(int);
    BOOL event_action();
    void privateCut();
    void event_proc();
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    BOOL setAnm_ATR();
    void chngAnmAtr(u32); /* u8, compared unnormalised */
    void ctrlAnmAtr();
    void anmAtr(u32);     /* u16, compared unnormalised */
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    u8 chkAttention();
    BOOL partner_srch();
    void ccCreate();
    BOOL wait01();
    BOOL wait02();
    BOOL wait03();
    BOOL wait04();
    BOOL wait05();
    BOOL talk01();
    BOOL talk02();
    BOOL wait_action1(void*);
    BOOL wait_action2(void*);

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ gptr<dBgW> mpBgW;           /* dBgWDeform (0xC4, HD vtable at +4) */
    /* 0x7E8 */ be<u32> mDeformRes;         /* skinned collision vertices ("de.cvtx"): resource */
    /* 0x7EC */ be<u32> mDeformVtx;         /*   and the vertex buffer the BG reads (+0x90) */
    /* 0x7F0 */ ProcFunc_l mAction;
    /* 0x7F8 */ dNpc_EventCut_c mEventCut2;  /* the actor's own dNpc_EventCut_c (constructor 0259F740) */
    /* 0x864 */ csXyz mInitAngle;           /* _execute: current.angle on the first frame */
    /* 0x86A */ u8 _86A[2];
    /* 0x86C */ cXyz mInitPos;              /* _execute: current.pos on the first frame */
    /* 0x878 */ cXyz m878;                  /* setAttention: copy of mAttPos */
    /* 0x884 */ cXyz mAttPos;
    /* 0x890 */ cXyz mHomePos;              /* createInit */
    /* 0x89C */ cXyz mDemoStartCenter;
    /* 0x8A8 */ be<f32> mAnmFrame;
    /* 0x8AC */ u8 _8AC[4];
    /* 0x8B0 */ be<s16> mSeTimer;
    /* 0x8B2 */ be<s16> mEvtTimer;
    /* 0x8B4 */ u8 _8B4[2];
    /* 0x8B6 */ be<s16> mEvtLiftNo;
    /* 0x8B8 */ u8 _8B8[4];
    /* 0x8BC */ be<s8> mAnmEnd;
    /* 0x8BD */ be<u8> m8BD;
    /* 0x8BE */ be<u8> m8BE;
    /* 0x8BF */ be<u8> m8BF;                /* getMsg: second talk */
    /* 0x8C0 */ be<u8> mInitDone;
    /* 0x8C1 */ u8 _8C1[3];
    /* 0x8C4 */ be<s32> m8C4;
    /* 0x8C8 */ u8 _8C8[4];
    /* 0x8CC */ be<u8> mAttention;          /* chkAttention result */
    /* 0x8CD */ be<u8> mTalkReq;
    /* 0x8CE */ u8 _8CE;
    /* 0x8CF */ be<u8> mInDemo;
    /* 0x8D0 */ be<u8> m8D0;
    /* 0x8D1 */ u8 _8D1[3];
    /* 0x8D4 */ dPa_followEcallBack mPaHappa; /* leaf particle (0x81BA) on joint branchL */
    /* 0x8E8 */ cXyz mPaHappaPos;
    /* 0x8F4 */ be<u32> mLiftId[10];        /* leaf lifts created by ccCreate */
    /* 0x91C */ be<s8> mLiftJnt[10];        /* joints the leaf lifts follow */
    /* 0x926 */ u8 _926[2];
    /* 0x928 */ be<u32> mPartnerId;         /* partner_srch: leaf lift (proc 0x78) */
    /* 0x92C */ be<s8> mActIdx;
    /* 0x92D */ be<s8> mEvtActNo;
    /* 0x92E */ be<u8> mAnmAtr;
    /* 0x92F */ be<u8> mMsgAnmAtr;
    /* 0x930 */ be<s8> mAnmNum;
    /* 0x931 */ be<s8> mOrder;              /* eventOrder: 1/2 talk, 3.. other events */
    /* 0x932 */ be<s8> mStt;
    /* 0x933 */ be<s8> mPrevStt;
    /* 0x934 */ be<s8> m934;
    /* 0x935 */ be<s8> mType;               /* decideType: Farore's... symbol 2 owned */
    /* 0x936 */ be<s8> mActPhase;
    /* 0x937 */ be<s8> mMsgAtrInit;
    /* 0x938 */ be<s8> mHeadJnt;
    /* 0x939 */ be<s8> mBranchJnt;
    /* 0x93A */ u8 _93A[2];
};
WWHD_OFFSET(daNpc_De1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_De1_c, mAction, 0x7F0);
WWHD_OFFSET(daNpc_De1_c, mEventCut2, 0x7F8);
WWHD_OFFSET(daNpc_De1_c, mInitAngle, 0x864);
WWHD_OFFSET(daNpc_De1_c, mPaHappa, 0x8D4);
WWHD_OFFSET(daNpc_De1_c, mLiftId, 0x8F4);
WWHD_OFFSET(daNpc_De1_c, mLiftJnt, 0x91C);
WWHD_OFFSET(daNpc_De1_c, mPartnerId, 0x928);
WWHD_OFFSET(daNpc_De1_c, mBranchJnt, 0x939);
WWHD_SIZE(daNpc_De1_c, 0x93C); /* profile g_profile_NPC_DE1 (0x101BE0D0): size 0x93C, proc name 0x74 */

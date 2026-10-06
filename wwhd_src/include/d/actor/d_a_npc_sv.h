/* daNpcSv_c (Salvage Corp. members), WWHD layout. 
 *
 * The GameCube TU is "Nonmatching" throughout and its header is empty: the members below are
 * measured from the WWHD code of d_a_npc_sv and named after their use. Size 0x860 (constructor
 * 022E50CC allocates 0x860). Base: fopNpc_npc_c, HD 0x7DC (fopNpc_npc_c_l in d_a_npc_ob1.h, which
 * also holds the NPC local bindings this unit uses). The actor's vtable is 100226A4 (destructor
 * +0x0C, next_msgStatus +0x14, getMsg +0x1C, anmAtr +0x24 = fopNpc_npc_c::anmAtr).
 *
 * The parent actor (parentActorID) is the salvage ship (d_a_obj_ikada); its fields read here
 * are named by offset. */
#pragma once
#include "d/actor/d_a_npc_ob1.h"

/* animation script entry (3 bytes) */
struct sSvAnmDat {
    /* 0x0 */ be<u8> mAnm;   /* 0xFF: end */
    /* 0x1 */ be<u8> mMorf;
    /* 0x2 */ be<s8> mLoop;  /* > 0: play n times, then the next entry */
};
WWHD_SIZE(sSvAnmDat, 3);

/* per-member parameters, l_npc_dat (.data 101C6668, 0x44 per npc number) */
struct sSvNpcDat_l {
    /* 0x00 */ be<s16> mMaxHeadX;
    /* 0x02 */ be<s16> mMaxHeadY;
    /* 0x04 */ be<s16> mMaxBackboneX;
    /* 0x06 */ be<s16> mMaxBackboneY;
    /* 0x08 */ be<s16> mMinHeadX;
    /* 0x0A */ be<s16> mMinHeadY;
    /* 0x0C */ be<s16> mMinBackboneX;
    /* 0x0E */ be<s16> mMinBackboneY;
    /* 0x10 */ be<s16> mMaxTurnStep;
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ cXyz mOffset;        /* model offset (setMtx) */
    /* 0x20 */ be<f32> mEyeOffset;  /* dNpc_playerEyePos */
    /* 0x24 */ be<f32> mAttnY;      /* attention position height */
    /* 0x28 */ be<f32> mEyeY;       /* eye position height */
    /* 0x2C */ be<f32> mTalkDist;   /* m824 */
    /* 0x30 */ be<f32> mLookDist;
    /* 0x34 */ be<s16> mLookAngle;  /* m83A */
    /* 0x36 */ be<s16> mLookSpeed;  /* m842 */
    /* 0x38 */ be<f32> mCylR;
    /* 0x3C */ be<f32> mCylH;
    /* 0x40 */ be<s16> mLookTimer;  /* m838 */
    /* 0x42 */ be<u8> m42;          /* m857 */
    /* 0x43 */ be<u8> m43;          /* m858 */
};
WWHD_SIZE(sSvNpcDat_l, 0x44);

struct daNpcSv_c : fopNpc_npc_c_l {
    /* 022E50CC */ static daNpcSv_c* ct(daNpcSv_c* p);
    cPhs_State _create();
    BOOL createHeap();
    s32 createInit();
    BOOL _delete();
    BOOL _draw();
    BOOL _execute();
    s32 executeCommon();
    void executeSetMode(int mode);
    s32 executeWaitInit();
    void executeWait();
    u8 executeTalkInit();
    void executeTalk();
    void checkOrder();
    void eventOrder();
    void eventMove();
    void privateCut();
    void eventMesSetInit(int staffId);
    BOOL eventMesSet();
    void eventGetItemInit(int staffId);
    void eventSetAngleInit();
    void eventAttentionInit(int staffId);
    BOOL eventAttention();
    void eventTurnOkInit();
    u16 talk2(int mode, fopAc_ac_c* actor);
    u16 next_msgStatus(be<u32>* pMsgNo);
    u32 getMsg();
    void setMessage(u32 msgNo);
    void setAnmFromMsgTag();
    u8 getPrmNpcNo();
    void setMtx();
    void chkAttention();
    void lookBack();
    void playAnm();
    void setAnm(u8 anm, int loopMode, f32 morf);
    BOOL setAnmTbl(sSvAnmDat* p);
    void setCollision(dCcD_Cyl* cyl, cXyz* pos, f32 r, f32 h); /* cXyz by value: pointer to a copy */
    u8 getTalkNo();
    BOOL isTalkOK();

    /* 0x7DC */ request_of_phase_process_class mPhs;      /* dComIfG_resLoad */
    /* 0x7E4 */ request_of_phase_process_class mPhsHandler; /* dComLbG_PhaseHandler */
    /* 0x7EC */ be<u32> m7EC;              /* chkAttention: != 0 keeps the look timer */
    /* 0x7F0 */ u8 _7F0[0x800 - 0x7F0];
    /* 0x800 */ cXyz mLookPos;             /* look target (mode 1) */
    /* 0x80C */ u8 _80C[0x818 - 0x80C];
    /* 0x818 */ gptr<sSvAnmDat> mpAnmTbl;  /* current animation script entry */
    /* 0x81C */ be<u32> mpMsgTbl;          /* u32* message script */
    /* 0x820 */ be<f32> mNextMorf;         /* < 0: none */
    /* 0x824 */ be<f32> mTalkDist;
    /* 0x828 */ be<s32> mItemNo;
    /* 0x82C */ be<s32> mFrame;            /* from the ship (+0x1438) */
    /* 0x830 */ be<u8> mHeadOnly;          /* lookAtTarget's last argument */
    /* 0x831 */ u8 _831;
    /* 0x832 */ be<s16> mEventIdx[3];      /* SV_TALK_P1_1ST, SV_TALK_P1_2ND, SV_TALK_P4_1ST */
    /* 0x838 */ be<s16> mLookTimer;
    /* 0x83A */ be<s16> mLookAngle;
    /* 0x83C */ be<s16> mHomeAngleY;
    /* 0x83E */ be<s16> mAttnTimer;        /* event ATTENTION "Timer" */
    /* 0x840 */ be<s16> mEvtAngleY;        /* event SET_ANGLE */
    /* 0x842 */ be<s16> mLookSpeed;
    /* 0x844 */ be<s16> mLookVel;
    /* 0x846 */ be<s16> mLookAngleY;       /* look mode 2 */
    /* 0x848 */ be<u16> mFlags;
    /* 0x84A */ be<u8> mTalk;
    /* 0x84B */ be<u8> mAttn;
    /* 0x84C */ be<u8> mOrder;             /* 1: can talk, 2: speak */
    /* 0x84D */ be<u8> mMode;              /* 0 wait, 1 talk */
    /* 0x84E */ be<u8> mCreated;
    /* 0x84F */ be<u8> mNpcNo;
    /* 0x850 */ be<u8> mEvtFlags;
    /* 0x851 */ be<u8> mAnm;
    /* 0x852 */ be<u8> mAnmFlags;
    /* 0x853 */ be<s8> mAnmLoop;
    /* 0x854 */ be<s8> mActIdx;
    /* 0x855 */ be<s8> mLookMode;          /* 0 none, 1 position, 2 angle */
    /* 0x856 */ be<u8> m856;
    /* 0x857 */ be<u8> m857;
    /* 0x858 */ be<u8> m858;
    /* 0x859 */ be<u8> m859;
    /* 0x85A */ be<u8> mTalkNo;
    /* 0x85B */ be<u8> mMsgAnm;
    /* 0x85C */ be<u8> m85C;
    /* 0x85D */ be<u8> m85D;
    /* 0x85E */ be<u8> m85E;
    /* 0x85F */ u8 _85F;
};
WWHD_OFFSET(daNpcSv_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpcSv_c, mLookPos, 0x800);
WWHD_OFFSET(daNpcSv_c, mpAnmTbl, 0x818);
WWHD_OFFSET(daNpcSv_c, mEventIdx, 0x832);
WWHD_OFFSET(daNpcSv_c, mFlags, 0x848);
WWHD_OFFSET(daNpcSv_c, mNpcNo, 0x84F);
WWHD_OFFSET(daNpcSv_c, m85E, 0x85E);
WWHD_SIZE(daNpcSv_c, 0x860);

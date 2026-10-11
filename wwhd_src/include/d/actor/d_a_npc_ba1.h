/* daNpc_Ba1_c (Link's grandmother), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the verified functions of d_a_npc_ba1:
 * - fopNpc_npc_c is 0x7DC (GameCube 0x6C4 + 0x11C - 4): its GameCube vtable pointer (0x6C0)
 *   is merged into the HD vtable at 0xB4 (fopNpc_npc_c::fopNpc_npc_c, 025A1458, allocates
 *   0x7DC). mpCurrMsg (GameCube 0x6B0, a msg_class*) is a u8 flag at 0x7CC in HD (the message
 *   is reached through the HD message manager, not a pointer).
 * - daNpc_Ba1_c members: +0x118 up to m_footL_jnt_num; mShadowID (GameCube 0x6D4) is gone (HD
 *   shadows), so mpClothModel/m_hed_tex_pttrn are +0x118/+0x114; mDoExt_btpAnm grew from 0x14
 *   to 0x74 (constructor 025E7820), and the pointer-to-member mCurrProcFunc is 8 bytes (GHS)
 *   instead of 12, so everything from mBlinkFrame on is +0x174 and from the second
 *   dNpc_EventCut_c (GameCube 0x708) on +0x170. Size 0x98C (GameCube 0x81C). */
#pragma once
#include "bindings.h"

/* Legacy NPC base retained for other actor headers. Ba1 uses shared fopNpc_npc_c. */
struct fopNpc_npc_l : fopAc_ac_c {
    /* 0x3AC */ dNpc_JntCtrl_c m_jnt;
    /* 0x3E0 */ dNpc_EventCut_c mEventCut;
    /* 0x44C */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x450 */ dBgS_ObjAcch mObjAcch;
    /* 0x614 */ dBgS_AcchCir mAcchCir;
    /* 0x654 */ dCcD_Stts mStts;
    /* 0x690 */ dCcD_Cyl mCyl;
    /* 0x7C0 */ be<u32> mCurrMsgNo;
    /* 0x7C4 */ be<u32> mEndMsgNo;
    /* 0x7C8 */ be<u32> mCurrMsgBsPcId;
    /* 0x7CC */ be<u8> mbHasMsg;          /* HD: GameCube msg_class* mpCurrMsg */
    /* 0x7CD */ u8 _7CD[3];
    /* 0x7D0 */ u8 field_0x7d0[6];
    /* 0x7D6 */ be<u16> field_0x7d6;
    /* 0x7D8 */ u8 field_0x7d8[4];
    /* 025A15AC */
    void setCollision(f32 r, f32 h) { gabi::call(0x025A15AC, this, r, h); }
    /* 025A11EC */
    u16 talk(s32 p) { return gabi::call<u16>(0x025A11EC, this, p); }
};
WWHD_OFFSET(fopNpc_npc_l, mStts, 0x654);
WWHD_OFFSET(fopNpc_npc_l, mCyl, 0x690);
WWHD_OFFSET(fopNpc_npc_l, mCurrMsgNo, 0x7C0);
WWHD_SIZE(fopNpc_npc_l, 0x7DC);

/* GHS pointer to member function (8 bytes): this adjustment, virtual index (0: null, < 0: not
 * virtual), then the function address, or (virtual) the vtable pointer's offset at +6 */
struct ProcFunc_l {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
WWHD_SIZE(ProcFunc_l, 8);

struct daNpc_Ba1_c;
using Ba1ProcFunc = gabi::ptmf<BOOL(daNpc_Ba1_c::*)(void*)>;
WWHD_SIZE(Ba1ProcFunc, 8);

struct daNpc_Ba1_c : fopNpc_npc_c {
    enum ProcState {
        PROC_START = 0,
        PROC_SEARCH = 1,
        PROC_PARTNER_FOUND = 2,
        PROC_ACTIVE = 3,
        PROC_END = 9,
    };
    enum LookBackState {
        LOOK_NONE = 0,
        LOOK_PLAYER = 1,
        LOOK_POSITION = 2,
        LOOK_ANGLE = 3,
    };
    enum Status {
        STATUS_NONE = 0,
        STATUS_WAIT_HOME = 1,
        STATUS_TALK = 2,
        STATUS_WAIT_OUTSIDE = 3,
        STATUS_SLEEP = 4,
        STATUS_TALK_SLEEP = 5,
        STATUS_WAIT_HEALED = 6,
        STATUS_WAIT_PLAYER = 7,
    };
    enum EventAction {
        ACT_WAKE = 0,
        ACT_SET_PLAYER_GOAL = 1,
        ACT_SET_PLAYER_POS = 2,
        ACT_FAIRY_END = 3,
        ACT_MOVE_POS = 4,
        ACT_TURN_PLAYER = 5,
        ACT_DRAW_CONTROL = 6,
        ACT_CHECK_FAIRY = 7,
        ACT_EYE_OFFSET = 8,
        ACT_EYE_RESET = 9,
        ACT_FAIRY_MOVEMENT = 10,
    };

    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;
        /* 0x01 */ be<s8> mBtpNum;
        /* 0x02 */ be<s16> field_0x02;
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    void holdEventOn() { mbHoldEvent = 1; }

    void nodeBa1Control(J3DNode*, J3DModel*);
    bool XyCheck_cB(int);
    s16 XyEvent_cB(int);
    bool init_BA1_0();
    bool init_BA1_1();
    bool init_BA1_2();
    bool init_BA1_3();
    bool init_BA1_4();
    bool createInit();
    void setMtx(u32); /* bool, passed on unnormalised */
    int anmNum_toResID(int);
    int btpNum_toResID(int);
    bool setBtp(u32, int);
    u32 iniTexPttrnAnm(u32);
    void plyTexPttrnAnm();
    u32 setAnm_tex(s8);
    BOOL setAnm_anm(anm_prm_c*);
    void setAnm_NUM(int, int);
    bool setAnm();
    void chg_anmAtr(u8);
    void control_anmAtr();
    void setAnm_ATR(int);
    void anmAtr(u16);
    void eventOrder();
    void checkOrder();
    bool chk_talk();
    bool chk_drct(f32);
    u8 chk_partsNotMove(); /* bool (talk_1 returns it as is) */
    void lookBack();
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_BA1_0();
    u32 getMsg_BA1_1();
    u32 getMsg_BA1_3();
    u32 getMsg_BA1_4();
    u32 getMsg();
    u8 chkAttention(); /* bool (stored as is) */
    void setAttention(u32); /* bool */
    fopAc_ac_c* searchByID(fpc_ProcID);
    bool partner_srch_sub(u32 /* fpcLyIt_JudgeFunc */);
    void partner_srch();
    bool check_useFairyArea();
    bool checkCommandTalk();
    bool charDecide(int);
    void eInit_SET_PLYER_GOL_();
    void eInit_PLYER_INI_POS_();
    void eInit_USE_FAIRY_END_();
    void eInit_MOV_POS_();
    void eInit_SET_PLYER_TRN_ANG_();
    void eInit_ACTOR_DRW_CONTROL_(be<s32>*, be<s32>*);
    void eInit_setEvTimer(be<s32>*);
    void eInit_CHK_FAIRY_(be<s32>*);
    f32 eInit_prmFloat(be<f32>*, f32);
    void eInit_SET_EYE_OFF_(be<f32>*);
    void eInit_EYE_OFF_ZRO_(be<f32>*);
    void eInit_CHK_FAIRY_MOV_1(be<s32>*);
    void event_actionInit(int);
    u32 eMove_MOV_POS_(); /* bool (typed u32: event_action passes the register on) */
    u32 eMove_CHK_FAIRY_(); /* bool (typed u32: event_action passes the register on) */
    u32 eMove_EYE_OFF_ZRO_(); /* bool (typed u32: event_action passes the register on) */
    u32 eMove_CHK_FAIRY_MOV_1(); /* bool (typed u32: event_action passes the register on) */
    u32 event_action(); /* bool; returns the eMove_* result register as is */
    void cut_init_START_TALE1(int);
    bool cut_move_START_TALE1();
    void privateCut(int);
    void endEvent();
    s32 isEventEntry();
    void event_proc(int);
    BOOL set_action(Ba1ProcFunc*, void*);
    void setStt(s8);
    BOOL wait_0();
    BOOL wait_1();
    BOOL talk_1();
    BOOL talk_2();
    BOOL wait_2();
    BOOL wait_3();
    BOOL ZZZwai();
    BOOL wait_action1(void*);
    BOOL wait_action2(void*);
    BOOL demo_action1(void*);
    BOOL wait_action3(void*);
    BOOL wait_action4(void*);
    u8 demo(); /* bool */
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    J3DModelData* create_Anm();
    bool create_itm_Mdl();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ be<s8> m_footL_jnt_num;
    /* 0x7E7 */ u8 _7E7;
    /* 0x7E8 */ gptr<J3DModel> mpClothModel;
    /* 0x7EC */ gptr<J3DAnmTexPattern> m_hed_tex_pttrn;   /* HD: no mShadowID before it */
    /* 0x7F0 */ mDoExt_btpAnm mHeadBtpAnm;                     /* mDoExt_btpAnm (HD 0x74) */
    /* 0x864 */ be<u8> mBlinkFrame;
    /* 0x865 */ u8 _865;
    /* 0x866 */ be<s16> mBlinkTimer;
    /* 0x868 */ Ba1ProcFunc mCurrProcFunc;
    /* 0x870 */ u8 field_0x870[8];
    /* 0x878 */ dNpc_EventCut_c mEventCut;                /* hides fopNpc_npc_c::mEventCut (as on GameCube) */
    /* 0x8E4 */ be<u32> mPartnerProcID;
    /* 0x8E8 */ cXyz mInitialPos;
    /* 0x8F4 */ csXyz mInitialAngle;
    /* 0x8FA */ csXyz m78A;
    /* 0x900 */ cXyz mTransformedEyePos;
    /* 0x90C */ cXyz m79C;
    /* 0x918 */ u8 field_0x918[0x930 - 0x918];
    /* 0x930 */ be<f32> mPrevMorfFrame;
    /* 0x934 */ be<f32> mEyeOffset;
    /* 0x938 */ be<f32> mEyeOffsetZero;
    /* 0x93C */ be<s16> mJointHeadY;
    /* 0x93E */ be<s16> mJointBackboneY;
    /* 0x940 */ be<s16> mActorAngleY;
    /* 0x942 */ be<s16> mEventIdTable[6];
    /* 0x94E */ be<s16> mEventIdx;
    /* 0x950 */ be<s16> mEvTimer;
    /* 0x952 */ be<s16> mEvTimer2;
    /* 0x954 */ u8 field_0x954[4];
    /* 0x958 */ be<s16> mLookAtMaxVel;
    /* 0x95A */ be<s16> mTargetYRot;
    /* 0x95C */ u8 field_0x95C[2];
    /* 0x95E */ be<s8> mbMorfAnimStopped;
    /* 0x95F */ be<u8> m7EF;
    /* 0x960 */ be<s8> m7F0;
    /* 0x961 */ be<u8> mItemNo;
    /* 0x962 */ be<u8> m7F2;
    /* 0x963 */ u8 field_0x963;
    /* 0x964 */ be<u8> mbInitGrandma0;
    /* 0x965 */ be<u8> m7F5;
    /* 0x966 */ be<u8> m7F6;
    /* 0x967 */ be<u8> m7F7;
    /* 0x968 */ be<u8> m7F8;
    /* 0x969 */ be<u8> mbHoldEvent;
    /* 0x96A */ be<u8> m7FA;
    /* 0x96B */ be<u8> m7FB;
    /* 0x96C */ be<u8> mFairyUsed;
    /* 0x96D */ be<u8> m7FD;
    /* 0x96E */ be<u8> m7FE;
    /* 0x96F */ be<u8> m7FF;
    /* 0x970 */ be<u8> mbRanExecute;
    /* 0x971 */ u8 _971[3];
    /* 0x974 */ be<s32> mbSetEyePos;
    /* 0x978 */ be<u8> mbAttention;
    /* 0x979 */ be<u8> m809;
    /* 0x97A */ be<u8> mHeadOnlyFollow;
    /* 0x97B */ be<u8> mbInDemo;
    /* 0x97C */ be<s8> mActionIndex;
    /* 0x97D */ be<s8> mActNo;
    /* 0x97E */ be<u8> m80E;
    /* 0x97F */ be<u8> mMesgAnimeTagInfo;
    /* 0x980 */ be<s8> mBtpNum;
    /* 0x981 */ be<s8> mAnmNum;
    /* 0x982 */ be<s8> m812;
    /* 0x983 */ be<s8> mStatus;
    /* 0x984 */ be<s8> mPrevStatus;
    /* 0x985 */ be<s8> mLookBackState;
    /* 0x986 */ be<s8> mType;
    /* 0x987 */ be<s8> mSpecificType;
    /* 0x988 */ be<s8> m818;
    /* 0x989 */ be<s8> m819;
    /* 0x98A */ u8 _98A[2];
};
WWHD_OFFSET(daNpc_Ba1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Ba1_c, mHeadBtpAnm, 0x7F0);
WWHD_OFFSET(daNpc_Ba1_c, mCurrProcFunc, 0x868);
WWHD_OFFSET(daNpc_Ba1_c, mEventCut, 0x878);
WWHD_OFFSET(daNpc_Ba1_c, mInitialPos, 0x8E8);
WWHD_OFFSET(daNpc_Ba1_c, mPrevMorfFrame, 0x930);
WWHD_OFFSET(daNpc_Ba1_c, mbRanExecute, 0x970);
WWHD_OFFSET(daNpc_Ba1_c, m819, 0x989);
WWHD_SIZE(daNpc_Ba1_c, 0x98C);

struct daNpc_Ba1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mMaxHeadX;
        /* 0x02 */ be<s16> mMaxHeadY;
        /* 0x04 */ be<s16> mMinHeadX;
        /* 0x06 */ be<s16> mMinHeadY;
        /* 0x08 */ be<s16> mMaxBackboneX;
        /* 0x0A */ be<s16> mMaxBackboneY;
        /* 0x0C */ be<s16> mMinBackboneX;
        /* 0x0E */ be<s16> mMinBackboneY;
        /* 0x10 */ be<s16> mMaxTurnStep;
        /* 0x12 */ be<s16> mCalcAngleTarget;
        /* 0x14 */ be<f32> mAttPosOffsetY;
        /* 0x18 */ be<u8> m18;
        /* 0x19 */ u8 pad19[3];
        /* 0x1C */ be<f32> m1C;
        /* 0x20 */ be<f32> m20;
    };
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ hio_prm_c mPrmTbl;
};
WWHD_SIZE(daNpc_Ba1_HIO_c, 0x30);

struct Ba1ModelDataView {
    /* 0x00 */ u8 pad[0x38];
    /* 0x38 */ be<u32> mpTexNoAnimator;
};
WWHD_OFFSET(Ba1ModelDataView, mpTexNoAnimator, 0x38);
WWHD_SIZE(Ba1ModelDataView, 0x3C);
struct Ba1TexPatternVtable {
    u8 pad[0x14];
    be<u32> mGetFrameMax;
};
struct Ba1TexPatternView {
    u8 pad[4];
    gptr<Ba1TexPatternVtable> mpVtable;
};
WWHD_OFFSET(Ba1TexPatternView, mpVtable, 4);
WWHD_SIZE(Ba1TexPatternView, 8);
WWHD_OFFSET(Ba1TexPatternVtable, mGetFrameMax, 0x14);
WWHD_SIZE(Ba1TexPatternVtable, 0x18);
struct Ba1PlayerVtable {
    u8 pad[0x114];
    be<u32> mSetPlayerPosAndAngle;
};
struct Ba1PlayerView {
    u8 pad[0xB4]; gptr<Ba1PlayerVtable> mpVtable;
    u8 padB8[0x3B8-0xB8]; be<u32> mNoResetFlg0;
    u8 pad3BC[0x422-0x3BC]; be<s16> mDemoMoveAngle;
};
WWHD_OFFSET(Ba1PlayerView, mpVtable, 0xB4);
WWHD_OFFSET(Ba1PlayerView, mNoResetFlg0, 0x3B8);
WWHD_OFFSET(Ba1PlayerView, mDemoMoveAngle, 0x422);
WWHD_SIZE(Ba1PlayerView, 0x424);
WWHD_OFFSET(Ba1PlayerVtable, mSetPlayerPosAndAngle, 0x114);
WWHD_SIZE(Ba1PlayerVtable, 0x118);
struct Ba1FairyView {
    u8 pad[0x8AC];
    be<u8> mMode;
};
WWHD_OFFSET(Ba1FairyView, mMode, 0x8AC);
WWHD_SIZE(Ba1FairyView, 0x8AD);

using Ba1ActionSignature = BOOL(daNpc_Ba1_c::*)(void*);
struct Ba1ActionDescriptor {
    gabi::ptmf<Ba1ActionSignature> action;
};
WWHD_SIZE(Ba1ActionDescriptor, 8);

struct Ba1SafeStringVtable {
    u8 pad[0x14];
    be<u32> mAssureTermination;
};
WWHD_OFFSET(Ba1SafeStringVtable, mAssureTermination, 0x14);
WWHD_SIZE(Ba1SafeStringVtable, 0x18);
struct Ba1MorfVtable {
    u8 pad[0xC];
    be<u32> mDeletingDestructor;
};
struct Ba1MorfView {
    gptr<Ba1MorfVtable> mpVtable;
};
WWHD_OFFSET(Ba1MorfVtable, mDeletingDestructor, 0xC);
WWHD_SIZE(Ba1MorfVtable, 0x10);
WWHD_OFFSET(Ba1MorfView, mpVtable, 0);
WWHD_SIZE(Ba1MorfView, 4);

enum dRes_ID_BA {
    /* BCKS */
    dRes_ID_BA_BCK_05_BA_CUT8_WAIT_e=0x0,
    dRes_ID_BA_BCK_HOLD_e=0x1,
    dRes_ID_BA_BCK_SLEEP_e=0x2,
    dRes_ID_BA_BCK_TALK01_e=0x3,
    dRes_ID_BA_BCK_TALK02_e=0x4,
    dRes_ID_BA_BCK_TALK03_e=0x5,
    dRes_ID_BA_BCK_TALK04_e=0x6,
    dRes_ID_BA_BCK_WAIT01_e=0x7,
    dRes_ID_BA_BCK_WAIT02_e=0x8,
    dRes_ID_BA_BCK_GETUP_e=0x17,
    /* BDL  */
    dRes_ID_BA_BDL_BA_CLOTH_e=0x9,
    /* BDLM */
    dRes_ID_BA_BDL_BA_e=0xA,
    /* BTP  */
    dRes_ID_BA_BTP_BA_T01_e=0xB,
    dRes_ID_BA_BTP_BA_T02_e=0xC,
    dRes_ID_BA_BTP_BA_T03_e=0xD,
    dRes_ID_BA_BTP_BA_T04_e=0xE,
    dRes_ID_BA_BTP_BA_T05_e=0xF,
    dRes_ID_BA_BTP_BA_T06_e=0x10,
    dRes_ID_BA_BTP_BA_T07_e=0x11,
    dRes_ID_BA_BTP_BA_T08_e=0x12,
    dRes_ID_BA_BTP_BA_T09_e=0x13,
    dRes_ID_BA_BTP_BA_T10_e=0x14,
    dRes_ID_BA_BTP_BA_WAKE_e=0x15,
    dRes_ID_BA_BTP_MABA01_e=0x16,
};

WWHD_OFFSET(daNpc_Ba1_HIO_c, mPrmTbl, 0xC);
WWHD_OFFSET(daNpc_Ba1_HIO_c::hio_prm_c, m18, 0x18);
WWHD_SIZE(daNpc_Ba1_HIO_c::hio_prm_c, 0x24);

WWHD_OFFSET(Ba1ActionDescriptor, action, 0);

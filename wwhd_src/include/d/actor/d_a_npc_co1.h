/* daNpc_Co1_c (Prince Komali, before Dragon Roost Cavern), WWHD layout.
 *
 * The GameCube header has no members (all functions are "Nonmatching" stubs), so the layout is
 * measured from the WWHD code (constructor 02227BBC allocates 0xA38; field accesses of all the
 * unit's functions). Unidentified bytes retain their measured layout. Known fields are named from their uses.
 * Base: shared fopNpc_npc_c, 0x7DC in HD. */
#pragma once
#include "gc_api/gc_api.h"

struct daNpc_Co1_c;
using Co1ProcFunc = gabi::ptmf<BOOL (daNpc_Co1_c::*)(void*)>;

struct daNpc_Co1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;
        /* 0x01 */ be<s8> mBtpNum;
        /* 0x02 */ be<s16> field_0x02;
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    /* methods (signatures: each part's owner fixes its own) */
    void nodeCo1Control(J3DNode*, J3DModel*);
    J3DModelData* create_Anm();
    u32 btpNum_toResID(int);        /* 022275E0 (called by setBtp) */
    BOOL setBtp(u32, int);
    u32 iniTexPttrnAnm(u32);        /* 022276E0 */
    J3DModelData* create_prl_Anm();
    BOOL setBtk(u32);
    bool create_itm_Mdl();
    BOOL CreateHeap();
    bool charDecide(int);           /* 02227C34 */
    BOOL set_action(Co1ProcFunc*, void*);
    bool init_CO1_0();
    void plyTexPttrnAnm();
    void setAttention(u32);
    void setMtx(u32);
    bool createInit();
    cPhs_State _create();
    BOOL _delete();
    void checkOrder();
    u8 demo();
    s32 isEventEntry();
    u32 setAnm_tex(s8);
    u32 anmNum_toResID(int);        /* 02228A5C (setAnm_anm: body) */
    u32 anmNum_toResID_prl(int);    /* 02228A70 (setAnm_anm: pearl) */
    BOOL setAnm_anm(anm_prm_c*);
    bool setAnm();
    void setStt(s8);
    void endEvent();
    void setAnm_NUM(int, int);
    void eInit_MDR_();
    void eInit_RED_LTR_();
    void event_actionInit(int);
    u32 eMove_MDR_();
    u32 eMove_RED_LTR_();
    u32 event_action();
    void privateCut(int);
    void lookBack();
    void event_proc(int);
    void eventOrder();              /* 02229378 */
    void setCollision_SP_();
    BOOL _execute();
    BOOL _draw();
    void set_target(int);
    void setAnm_ATR(int);
    void chg_anmAtr(u8);
    void control_anmAtr();
    void anmAtr(u16);
    bool chk_talk();
    u8 chk_partsNotMove();
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_CO1_0();
    u32 getMsg();
    u8 chkAttention();
    BOOL wait_1();
    BOOL wait_2();
    BOOL wakeup();
    BOOL talk_1();
    BOOL toru_1();
    BOOL read_1();
    BOOL modoru();
    BOOL wait_action1(void*);

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> mHeadJoint;               /* joint numbers (s8) */
    /* 0x7E5 */ be<s8> mBackboneJoint;
    /* 0x7E6 */ be<s8> mRightHandJoint;
    /* 0x7E7 */ u8 _7E7;
    /* 0x7E8 */ gptr<mDoExt_McaMorf> mPearlMorf;              /* pointer */
    /* 0x7EC */ gptr<J3DAnmTextureSRTKey> mPearlTextureAnm;              /* pointer */
    /* 0x7F0 */ be<s8> mPearlJoint;
    /* 0x7F1 */ u8 _7F1[3];
    /* 0x7F4 */ mDoExt_btkAnm mBtkAnm;          /* mDoExt_btkAnm (HD 0x74, constructor 025E7C6C) */
    /* 0x868 */ be<u8> mPearlBtkFrame;
    /* 0x869 */ u8 _869[3];
    /* 0x86C */ gptr<J3DModel> mLetterModel;              /* pointer */
    /* 0x870 */ gptr<J3DAnmTexPattern> mHeadTextureAnm;              /* pointer */
    /* 0x874 */ mDoExt_btpAnm mBtpAnm;          /* mDoExt_btpAnm (HD 0x74, constructor 025E7820) */
    /* 0x8E8 */ be<u8> mBlinkFrame;
    /* 0x8E9 */ u8 _8E9;
    /* 0x8EA */ be<s16> mBlinkTimer;
    /* 0x8EC */ Co1ProcFunc mCurrProcFunc;
    /* 0x8F4 */ u8 _8F4[8];
    /* 0x8FC */ dNpc_EventCut_c mEventCut;  /* hides fopNpc_npc_c::mEventCut (constructor 0259F740) */
    /* 0x968 */ u8 _968[4];
    /* 0x96C */ cXyz mSavedPosition;
    /* 0x978 */ be<s16> mSavedAngleX;
    /* 0x97A */ be<s16> mSavedAngleY;
    /* 0x97C */ be<s16> mSavedAngleZ;
    /* 0x97E */ u8 _97E[2];
    /* 0x980 */ cXyz mEyePosition;
    /* 0x98C */ cXyz mLookTarget;                 /* initial position (createInit copies current.pos) */
    /* 0x998 */ u8 _998[0xC];
    /* 0x9A4 */ be<f32> mPreviousAnmFrame;
    /* 0x9A8 */ u8 _9A8[0x14];
    /* 0x9BC */ be<s16> mPreviousHeadYaw;
    /* 0x9BE */ be<s16> mPreviousBackboneYaw;
    /* 0x9C0 */ be<s16> mPreviousActorYaw;
    /* 0x9C2 */ be<s16> mEventIdx[3];       /* createInit: dEvent_manager_c::getEventIdx of 3 names */
    /* 0x9C8 */ be<s16> mActiveEventIdx;
    /* 0x9CA */ u8 _9CA[2];
    /* 0x9CC */ be<s16> mLookTimer;
    /* 0x9CE */ be<s16> m9CE;
    /* 0x9D0 */ be<s16> m9D0;
    /* 0x9D2 */ be<s16> mLookTurnSpeed;
    /* 0x9D4 */ be<s16> mLookTargetYaw;
    /* 0x9D6 */ u8 _9D6[2];
    /* 0x9D8 */ be<s8> mAnimationStopped;
    /* 0x9D9 */ be<u8> mAnimationFlag;
    /* 0x9DA */ be<s8> mReceivedLetter;
    /* 0x9DB */ be<u8> mPresentItemNo;
    /* 0x9DC */ be<u8> m9DC;
    /* 0x9DD */ be<u8> mSkipExecute;
    /* 0x9DE */ be<u8> m9DE;
    /* 0x9DF */ be<u8> mKeepShapeAngle;
    /* 0x9E0 */ be<u8> mSkipPositionMove;
    /* 0x9E1 */ be<u8> mHideModel;
    /* 0x9E2 */ be<u8> m9E2;
    /* 0x9E3 */ be<u8> mShowLetterModel;
    /* 0x9E4 */ be<u8> m9E4;
    /* 0x9E5 */ be<u8> mSavedInitialTransform;
    /* 0x9E6 */ u8 _9E6[2];
    /* 0x9E8 */ be<s32> mEyePositionResult;
    /* 0x9EC */ be<u8> mHasPlayerAttention;
    /* 0x9ED */ be<u8> mTalkRequested;
    /* 0x9EE */ be<u8> mHeadOnlyFollow;
    /* 0x9EF */ be<u8> mInDemo;
    /* 0x9F0 */ LIGHT_INFLUENCE mPearlLight;
    /* 0xA14 */ be<f32> mPearlPositionX;
    /* 0xA18 */ be<f32> mPearlPositionY;
    /* 0xA1C */ be<f32> mPearlPositionZ;
    /* 0xA20 */ be<f32> mGlowPulseSin;
    /* 0xA24 */ be<s16> mGlowPulseAngle;
    /* 0xA26 */ u8 _A26[2];
    /* 0xA28 */ be<s8> mCutIndex;
    /* 0xA29 */ be<s8> mEventAction;
    /* 0xA2A */ be<u8> mAnimationAttribute;
    /* 0xA2B */ be<u8> mAnimationTag;
    /* 0xA2C */ be<s8> mBtpNumber;                /* btp number (iniTexPttrnAnm) */
    /* 0xA2D */ be<s8> mBckNumber;
    /* 0xA2E */ u8 _A2E[2];
    /* 0xA30 */ be<s8> mEventOrderKind;                /* eventOrder: order kind */
    /* 0xA31 */ be<s8> mState;
    /* 0xA32 */ be<s8> mNextState;
    /* 0xA33 */ be<s8> mLookMode;
    /* 0xA34 */ be<s8> mCharacterSubtype;
    /* 0xA35 */ be<s8> mCharacterType;                /* charDecide: type */
    /* 0xA36 */ be<s8> mActionPhase;
    /* 0xA37 */ be<s8> mMessageAnimationPhase;
};
WWHD_OFFSET(daNpc_Co1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Co1_c, mBtkAnm, 0x7F4);
WWHD_OFFSET(daNpc_Co1_c, mBtpAnm, 0x874);
WWHD_OFFSET(daNpc_Co1_c, mCurrProcFunc, 0x8EC);
WWHD_OFFSET(daNpc_Co1_c, mEventCut, 0x8FC);
WWHD_OFFSET(daNpc_Co1_c, mLookTarget, 0x98C);
WWHD_OFFSET(daNpc_Co1_c, mEventIdx, 0x9C2);
WWHD_OFFSET(daNpc_Co1_c, mPearlLight, 0x9F0);

WWHD_OFFSET(daNpc_Co1_c, mEventOrderKind, 0xA30);
WWHD_SIZE(daNpc_Co1_c, 0xA38);

// HD: private bindings retain the original argument and return widths.
void co1Pending_nodeCo1Control(daNpc_Co1_c* self, J3DNode* a0, J3DModel* a1);
J3DModelData* co1Pending_create_Anm(daNpc_Co1_c* self);
u32 co1Pending_btpNum_toResID(daNpc_Co1_c* self, int a0);
BOOL co1Pending_setBtp(daNpc_Co1_c* self, u32 a0, int a1);
u32 co1Pending_iniTexPttrnAnm(daNpc_Co1_c* self, u32 a0);
J3DModelData* co1Pending_create_prl_Anm(daNpc_Co1_c* self);
BOOL co1Pending_setBtk(daNpc_Co1_c* self, u32 a0);
bool co1Pending_create_itm_Mdl(daNpc_Co1_c* self);
BOOL co1Pending_CreateHeap(daNpc_Co1_c* self);
bool co1Pending_charDecide(daNpc_Co1_c* self, int a0);
BOOL co1Pending_set_action(daNpc_Co1_c* self, Co1ProcFunc* a0, void* a1);
bool co1Pending_init_CO1_0(daNpc_Co1_c* self);
void co1Pending_plyTexPttrnAnm(daNpc_Co1_c* self);
void co1Pending_setAttention(daNpc_Co1_c* self, u32 a0);
void co1Pending_setMtx(daNpc_Co1_c* self, u32 a0);
bool co1Pending_createInit(daNpc_Co1_c* self);
cPhs_State co1Pending__create(daNpc_Co1_c* self);
BOOL co1Pending__delete(daNpc_Co1_c* self);
void co1Pending_checkOrder(daNpc_Co1_c* self);
u8 co1Pending_demo(daNpc_Co1_c* self);
s32 co1Pending_isEventEntry(daNpc_Co1_c* self);
u32 co1Pending_setAnm_tex(daNpc_Co1_c* self, s8 a0);
u32 co1Pending_anmNum_toResID(daNpc_Co1_c* self, int a0);
u32 co1Pending_anmNum_toResID_prl(daNpc_Co1_c* self, int a0);
BOOL co1Pending_setAnm_anm(daNpc_Co1_c* self, daNpc_Co1_c::anm_prm_c* a0);
bool co1Pending_setAnm(daNpc_Co1_c* self);
void co1Pending_setStt(daNpc_Co1_c* self, s8 a0);
void co1Pending_endEvent(daNpc_Co1_c* self);
void co1Pending_setAnm_NUM(daNpc_Co1_c* self, int a0, int a1);
void co1Pending_eInit_MDR_(daNpc_Co1_c* self);
void co1Pending_eInit_RED_LTR_(daNpc_Co1_c* self);
void co1Pending_event_actionInit(daNpc_Co1_c* self, int a0);
u32 co1Pending_eMove_MDR_(daNpc_Co1_c* self);
u32 co1Pending_eMove_RED_LTR_(daNpc_Co1_c* self);
u32 co1Pending_event_action(daNpc_Co1_c* self);
void co1Pending_privateCut(daNpc_Co1_c* self, int a0);
void co1Pending_lookBack(daNpc_Co1_c* self);
void co1Pending_event_proc(daNpc_Co1_c* self, int a0);
void co1Pending_eventOrder(daNpc_Co1_c* self);
void co1Pending_setCollision_SP_(daNpc_Co1_c* self);
BOOL co1Pending__execute(daNpc_Co1_c* self);
BOOL co1Pending__draw(daNpc_Co1_c* self);
void co1Pending_set_target(daNpc_Co1_c* self, int a0);
void co1Pending_setAnm_ATR(daNpc_Co1_c* self, int a0);
void co1Pending_chg_anmAtr(daNpc_Co1_c* self, u8 a0);
void co1Pending_control_anmAtr(daNpc_Co1_c* self);
void co1Pending_anmAtr(daNpc_Co1_c* self, u16 a0);
bool co1Pending_chk_talk(daNpc_Co1_c* self);
u8 co1Pending_chk_partsNotMove(daNpc_Co1_c* self);
u16 co1Pending_next_msgStatus(daNpc_Co1_c* self, be<u32>* a0);
u32 co1Pending_getMsg_CO1_0(daNpc_Co1_c* self);
u32 co1Pending_getMsg(daNpc_Co1_c* self);
u8 co1Pending_chkAttention(daNpc_Co1_c* self);
BOOL co1Pending_wait_1(daNpc_Co1_c* self);
BOOL co1Pending_wait_2(daNpc_Co1_c* self);
BOOL co1Pending_wakeup(daNpc_Co1_c* self);
BOOL co1Pending_talk_1(daNpc_Co1_c* self);
BOOL co1Pending_toru_1(daNpc_Co1_c* self);
BOOL co1Pending_read_1(daNpc_Co1_c* self);
BOOL co1Pending_modoru(daNpc_Co1_c* self);
BOOL co1Pending_wait_action1(daNpc_Co1_c* self, void* a0);
void* co1_getIDResource(dRes_control_c*, SafeString*, s32);
s8 co1_findJointName(u32, const char*);
s32 co1_initBtp(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32);
s32 co1_initBtk(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32);
s16 co1_decrementTimer(be<s16>*);
s8 co1_getRoomId(dBgS*, void*);
u8 co1_getPolyColor(dBgS*, void*);
u32 co1_setBtpResult(daNpc_Co1_c*, u32, s32);
void co1_constructNpc(daNpc_Co1_c*);
void co1_constructBtk(void*);
void co1_constructBtp(void*);
void co1_constructEventCut(dNpc_EventCut_c*);
void* co1_getDemoActor(u32, u32);
u32 co1_getDemoBtp(void*, const char*);
BOOL co1_setDemoData(daNpc_Co1_c*, s32, mDoExt_McaMorf*, const char*, s32, s32, s32, s32);
s8 co1_getActionIndex(dEvent_manager_c*, s32, u32, s32, BOOL, s32);
u32 co1_getMessageStatus(u32);
bool co1_lockonTruth(dAttention_c*);
fopAc_ac_c* co1_lockonTarget(dAttention_c*, s32);
fopAc_ac_c* co1_actionTarget(dAttention_c*, s32);
void co1_playerEyePosition(cXyz*, f32);
void co1_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
void co1_destroyCircle(u32, s32);
void co1_destroyActor(daNpc_Co1_c*, s32);

struct daNpc_Co1_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ be<s16> mMaxHeadX;
    /* 0x0E */ be<s16> mMaxHeadY;
    /* 0x10 */ be<s16> mMinHeadX;
    /* 0x12 */ be<s16> mMinHeadY;
    /* 0x14 */ be<s16> mMaxBackboneX;
    /* 0x16 */ be<s16> mMaxBackboneY;
    /* 0x18 */ be<s16> mMinBackboneX;
    /* 0x1A */ be<s16> mMinBackboneY;
    /* 0x1C */ be<s16> mMaxTurnStep;
    /* 0x1E */ be<s16> mCalcAngleTarget;
    /* 0x20 */ be<f32> mAttentionOffsetY;
    /* 0x24 */ be<u8> mDebugDraw;     /* _draw: debug leftovers */
    /* 0x25 */ u8 _25;
    /* 0x26 */ be<s16> mGlowColorR;
    /* 0x28 */ be<s16> mGlowColorG;
    /* 0x2A */ be<s16> mGlowColorB;
    /* 0x2C */ be<f32> mGlowMaxPower;
    /* 0x30 */ be<f32> mGlowMinPower;
    /* 0x34 */ be<s16> mGlowAngleStep;
    /* 0x36 */ u8 _36[2];
};
WWHD_SIZE(daNpc_Co1_HIO_c, 0x38);
extern daNpc_Co1_HIO_c& l_HIO;
extern be<u32> (&l_btpResID)[7];
extern be<u32> (&l_bckResID)[11];
extern be<u32> (&l_pearlBckResID)[11];
extern be<u32> (&l_heapSize)[1];
extern daNpc_Co1_c::anm_prm_c (&l_stateAnmPrm)[8];
extern daNpc_Co1_c::anm_prm_c (&l_numberAnmPrm)[12];
extern daNpc_Co1_c::anm_prm_c (&l_attributeAnmPrm)[14];
extern cXyz& l_eyeOffset;
extern be<u32>& l_eyeOffsetGuard;
extern be<u32>& l_debugColorGuard;
extern be<u32>& l_debugPearlColorGuard;
extern be<u32>& l_debugMorfColorGuard;
extern be<u8> (&l_debugColor)[4];
extern be<u8> (&l_debugColorInitial)[4];
extern be<u8> (&l_debugPearlColor)[4];
extern be<u8> (&l_debugPearlColorInitial)[4];
extern be<u8> (&l_debugMorfColor)[4];
extern be<u8> (&l_debugMorfColorInitial)[4];
extern const gptr<char> (&l_eventNames)[3];
extern const gptr<char> (&l_cutNames)[1];

// Shared NPC collision template; this unit only reads the original object.
inline dCcD_SrcCyl& co1NpcCylinderSource() { return gabi::object_at<dCcD_SrcCyl>(0x101EA190); }

struct Co1HIOParameters {
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
    /* 0x14 */ be<f32> mAttentionOffsetY;
    /* 0x18 */ be<u8> mDebugDraw;     /* _draw: debug leftovers */
    /* 0x19 */ u8 mReserved25;
    /* 0x1A */ be<s16> mGlowColorR;
    /* 0x1C */ be<s16> mGlowColorG;
    /* 0x1E */ be<s16> mGlowColorB;
    /* 0x20 */ be<f32> mGlowMaxPower;
    /* 0x24 */ be<f32> mGlowMinPower;
    /* 0x28 */ be<s16> mGlowAngleStep;
    /* 0x2A */ u8 mReserved36[2];
};
WWHD_SIZE(Co1HIOParameters, 0x2C);
extern Co1HIOParameters& l_hioParameters;

// GameCube resource IDs are unchanged in HD.
enum dRes_ID_CO {
    /* BCKS */
    dRes_ID_CO_BCK_COPEAL_MIGAMAE_e=0x0,
    dRes_ID_CO_BCK_COPEAL_OKIRU_e=0x1,
    dRes_ID_CO_BCK_COPEAL_READ_e=0x2,
    dRes_ID_CO_BCK_COPEAL_SWAIT01_e=0x3,
    dRes_ID_CO_BCK_COPEAL_TALK01_e=0x4,
    dRes_ID_CO_BCK_COPEAL_TALK02_e=0x5,
    dRes_ID_CO_BCK_COPEAL_UKETORI_e=0x6,
    dRes_ID_CO_BCK_COPEAL_WAIT01_e=0x7,
    dRes_ID_CO_BCK_COPEAL_WAIT02_e=0x8,
    dRes_ID_CO_BCK_CO_MIGAMAE_e=0x9,
    dRes_ID_CO_BCK_CO_OKIRU_e=0xA,
    dRes_ID_CO_BCK_CO_READ_e=0xB,
    dRes_ID_CO_BCK_CO_SWAIT01_e=0xC,
    dRes_ID_CO_BCK_CO_TALK01_e=0xD,
    dRes_ID_CO_BCK_CO_TALK02_e=0xE,
    dRes_ID_CO_BCK_CO_UKETORI_e=0xF,
    dRes_ID_CO_BCK_CO_WAIT01_e=0x10,
    dRes_ID_CO_BCK_CO_WAIT02_e=0x11,
    dRes_ID_CO_BCK_CO_WAIT00_e=0x1D,
    /* BDL  */
    dRes_ID_CO_BDL_CO_LETTER_e=0x12,
    /* BDLM */
    dRes_ID_CO_BDL_CO_e=0x13,
    dRes_ID_CO_BDL_CO_PEAL_e=0x14,
    /* BTK  */
    dRes_ID_CO_BTK_CO_PEAL_e=0x15,
    /* BTP  */
    dRes_ID_CO_BTP_FUAN_e=0x16,
    dRes_ID_CO_BTP_KIRI_e=0x17,
    dRes_ID_CO_BTP_MIGAMAE_e=0x18,
    dRes_ID_CO_BTP_NAGAME_e=0x19,
    dRes_ID_CO_BTP_NIKO_e=0x1A,
    dRes_ID_CO_BTP_OKIRU_e=0x1B,
    dRes_ID_CO_BTP_OKORI_e=0x1C,
};

// HD: GHS member pointers are copied as two words, in this order.
struct Co1ActionWords { be<u32> mThisAndVirtualIndex; be<u32> mFunction; };
extern Co1ActionWords& l_waitAction;

// HD: the two animation attachments cleared after entryDL are outside the shared data prefix.
struct Co1ModelData : J3DModelData {
    u8 _14[0x38 - 0x14];
    be<u32> mTexturePattern;
    u8 _3C[8];
    be<u32> mTextureSRT;
};
WWHD_OFFSET(Co1ModelData, mTexturePattern, 0x38);
WWHD_OFFSET(Co1ModelData, mTextureSRT, 0x44);
// HD: the joint resource exposes its joint index directly at +4.
struct Co1JointResource {
    be<u32> mData;
    be<u16> mJointNo;
};

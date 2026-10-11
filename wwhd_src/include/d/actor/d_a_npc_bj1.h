/* daNpc_Bj1_c: Forest Haven Koroks.
 * HD: the donor has no field layout; measured from the verified accesses. The shared
 * NPC base is 0x7DC, GHS member pointers are eight bytes and the class is 0x9C8.
 * Word-backed position components retain the native integer-copy NaN semantics.
 * The model/animation/event state is shared by both source parts. */
#pragma once
#include "gc_api/gc_api.h"

struct daNpc_Bj1_c;
using Bj1Action = gabi::ptmf<BOOL (daNpc_Bj1_c::*)(void*)>;

#define BJ1_VTBL 0x10016EF0

struct daNpc_Bj1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;
        /* 0x01 */ u8 _01[3];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    void nodeBj1Control(J3DNode*, J3DModel*);
    void nodePrpControl(J3DNode*, J3DModel*);
    BOOL init_BJ4_0();
    BOOL init_BJ6_0();
    BOOL init_BJ7_0();
    BOOL init_BJX_0();
    BOOL init_BJX_1();
    BOOL createInit();
    void setMtx_anmProc();
    void setMtx(int);
    u32 anmNum_toResID(int);
    BOOL setAnm_anm(anm_prm_c*);
    void setAnm_NUM(int);
    BOOL setAnm();
    void setAnm_prp(s8);
    void chg_anmTag();
    void control_anmTag();
    void chg_anmAtr(u8);
    void control_anmAtr();
    void setAnm_ATR();
    void anmAtr(u16);
    void eventOrder();
    void checkOrder();
    BOOL chk_talk();
    BOOL chk_drct(f32);
    BOOL chk_partsNotMove();
    void lookBack();
    BOOL getMaskInf(u8*);
    BOOL chkReg(u16);
    void setReg(u16);
    u32 next_msgStatus(be<u32>*);
    u32 getMsg_BJ1_0();
    u32 getMsg_BJ2_0();
    u32 getMsg_BJ3_0();
    u32 getMsg_BJ4_0();
    u32 getMsg_BJ5_0();
    u32 getMsg_BJ6_0();
    u32 getMsg_BJ7_0();
    u32 getMsg_BJ8_0();
    u32 getMsg_BJ9_0();
    u32 getMsg_Corog();
    u32 getMsg();
    BOOL chkAttention();
    void setAttention(bool);
    fopAc_ac_c* searchByID(fpc_ProcID);
    BOOL partner_srch_sub(u32 cb);
    void partner_srch();
    void setCollision_SP_();
    void set_pthPoint(u8);
    void bj_clcFlySpd();
    u32 bj_movPass(int);
    BOOL bj_flyMove();
    void bj_clcMovSpd();
    void bj_nMove();
    void setPrtcl_drugPot_1();
    void setPrtcl_drugPot_2();
    void delPrtcl_drugPot();
    void setPrtcl_danceLR();
    void flwPrtcl_danceLR();
    void delPrtcl_danceLR();
    void setPrtcl_peraProOpen();
    BOOL createSeed();
    BOOL deleteSeed();
    BOOL charDecide(int);
    void eInit_setLocFlag(int*);
    void eInit_setShapeAngleY(int*, s16);
    void eInit_setEvTimer(int*);
    u32 eInit_calcRelativPos(cXyz*, cXyz*, void*);
    f32 eInit_prmFloat(f32*, f32);
    void eInit_ATTENTION_(int*, int*, int*, cXyz*, int*, int*, int*);
    void eInit_PLYER_MOV_1_();
    void eInit_MOV_(f32*, f32*, f32*, int*);
    void eInit_JMP_(f32*, f32*);
    void eInit_CHG_PTH_(int*, int*);
    void eInit_END_MOV_();
    void eInit_SET_TNE_();
    void eInit_DEL_TNE_();
    void eInit_SET_ANM_(int*, f32*);
    void event_actionInit(int);
    void eMove_ATTENTION_();
    u32 eMove_MOV_();
    u32 eMove_JMP_();
    BOOL eMove_SET_TNE_();
    BOOL eMove_PTH_MOV_();
    u32 event_action();
    void privateCut(int);
    void endEvent();
    s32 isEventEntry();
    void event_proc(int);
    BOOL set_action(Bj1Action*, void*);
    void setStt(s8);
    BOOL wait_1();
    BOOL wait_2();
    BOOL wait_3();
    BOOL wait_4();
    BOOL flyMov();
    BOOL fall01();
    BOOL talk_1();
    BOOL walk_1();
    BOOL wait_action1(void*);
    BOOL wait_action2(void*);
    BOOL wait_action3(void*);
    BOOL wait_action4(void*);
    BOOL demo();
    void shadowDraw();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    J3DModelData* create_Anm();
    J3DModelData* create_prp_Anm();
    BOOL create_itm_Mdl();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> mHeadJoint;               /* joint number */
    /* 0x7E5 */ be<s8> mBackboneJoint;           /* joint number */
    /* 0x7E6 */ be<s8> mLeftHandJoint;           /* joint number */
    /* 0x7E7 */ be<s8> mRightHandJoint;          /* joint number */
    /* 0x7E8 */ gptr<J3DModel> mpMaskModel;      /* item model (create_itm_Mdl) */
    /* 0x7EC */ gptr<J3DModel> mpLeftHandModel;  /* item model (create_itm_Mdl) */
    /* 0x7F0 */ gptr<J3DModel> mpRightHandModel; /* item model (create_itm_Mdl) */
    /* 0x7F4 */ be<s8> mLeftItemJoint;           /* joint number in an item model */
    /* 0x7F5 */ be<s8> mRightItemJoint;          /* joint number in an item model */
    /* 0x7F6 */ u8 _7F6[0x2];
    /* 0x7F8 */ gptr<mDoExt_McaMorf> mpPrpMorf; /* the propeller morf (create_prp_Anm) */
    /* 0x7FC */ be<s8> mPropellerJoint;         /* joint number */
    /* 0x7FD */ be<s8> mPropellerRootJoint;     /* joint number */
    /* 0x7FE */ u8 _7FE[0x6];
    /* 0x804 */ be<s16> mPropellerAngle;
    /* 0x806 */ be<s16> mPropellerSpeed;
    /* 0x808 */ u8 _808[0x4];
    /* 0x80C */ Bj1Action mAction;
    /* 0x814 */ Mtx34 mPropellerAttachMtx;
    /* 0x844 */ dNpc_PathRun_c mPath;
    /* 0x84C */ dNpc_EventCut_c
        mEventCut2; /* the actor's own dNpc_EventCut_c (constructor 0259F740) */
    /* 0x8B8 */ be<u32> mPartnerID;
    /* 0x8BC */ be<u32> mInitialPosXBits;
    /* 0x8C0 */ be<u32> mInitialPosYBits;
    /* 0x8C4 */ be<u32> mInitialPosZBits;
    /* 0x8C8 */ be<s16> mInitialAngleX;
    /* 0x8CA */ be<s16> mInitialAngleY;
    /* 0x8CC */ be<s16> mInitialAngleZ;
    /* 0x8CE */ u8 _8CE[0x2];
    /* 0x8D0 */ be<f32> mHeadPosX;
    /* 0x8D4 */ be<f32> mHeadPosY;
    /* 0x8D8 */ be<f32> mHeadPosZ;
    /* 0x8DC */ be<u32> mLookTargetXBits;
    /* 0x8E0 */ be<f32> mLookTargetY;
    /* 0x8E4 */ be<u32> mLookTargetZBits;
    /* 0x8E8 */ be<u32> mMoveTargetXBits;
    /* 0x8EC */ be<f32> mMoveTargetY;
    /* 0x8F0 */ be<u32> mMoveTargetZBits;
    /* 0x8F4 */ be<f32> mHeadOffsetX;
    /* 0x8F8 */ be<f32> mHeadOffsetY;
    /* 0x8FC */ be<f32> mHeadOffsetZ;
    /* 0x900 */ be<f32> mHeadScaleX;
    /* 0x904 */ be<f32> mHeadScaleY;
    /* 0x908 */ be<f32> mHeadScaleZ;
    /* 0x90C */ be<f32> mBackboneScaleX;
    /* 0x910 */ be<f32> mBackboneScaleY;
    /* 0x914 */ be<f32> mBackboneScaleZ;
    /* 0x918 */ be<f32> mPreviousAnmFrame;
    /* 0x91C */ be<f32> mTargetSpeed;
    /* 0x920 */ u8 _920[0x4];
    /* 0x924 */ be<f32> mSpeedStep;
    /* 0x928 */ u8 _928[0x4];
    /* 0x92C */ be<f32> mPointPassDistance;
    /* 0x930 */ be<s16> mPreviousHeadPitch;
    /* 0x932 */ be<s16> mPreviousHeadRoll;
    /* 0x934 */ be<s16> mPreviousHeading;
    /* 0x936 */ be<s16> mEventIDs[3];
    /* 0x93C */ be<s16> mEventIndex;
    /* 0x93E */ be<s16> mFlightTimer;
    /* 0x940 */ be<s16> mEventTimer;
    /* 0x942 */ be<s16> mWaitTimer;
    /* 0x944 */ be<s16> mFlightWaitTimer;
    /* 0x946 */ u8 _946[0x2];
    /* 0x948 */ be<s16> mLookLimit;
    /* 0x94A */ be<s16> mLookHeading;
    /* 0x94C */ be<u8> mAnmEnded;
    /* 0x94D */ be<u8> mAnmTagIndex;
    /* 0x94E */ be<u8> mTalkItem;
    /* 0x94F */ be<u8> mPathIndex;
    /* 0x950 */ be<u8> mCollectedFirstGift;
    /* 0x951 */ be<u8> mCollectedSecondGift;
    /* 0x952 */ be<u8> mFirstEventEnded;
    /* 0x953 */ be<u8> mSecondEventEnded;
    /* 0x954 */ be<u8> mHidden;
    /* 0x955 */ be<u8> mDrawDisabled;
    /* 0x956 */ be<u8> mKeepShapeAngle;
    /* 0x957 */ be<u8> mSkipCollisionMove;
    /* 0x958 */ be<u8> mPathRising;
    /* 0x959 */ be<u8> mFlightComplete;
    /* 0x95A */ be<u8> mWalkComplete;
    /* 0x95B */ be<u8> mPartnerTalkAlternate;
    /* 0x95C */ be<u8> mPropellerVisible;
    /* 0x95D */ be<u8> mPropellerOpening;
    /* 0x95E */ be<u8> mInitialTransformSaved;
    /* 0x95F */ u8 _95F[0x1];
    /* 0x960 */ be<u32> mAttentionFromHead;
    /* 0x964 */ be<u8> mAttentionSelected;
    /* 0x965 */ be<u8> mTalkRequested;
    /* 0x966 */ be<u8> mLookAtPlayer;
    /* 0x967 */ be<u8> mDemoActive;
    /* 0x968 */ be<s16> mPotAngleX;
    /* 0x96A */ be<s16> mPotAngleY;
    /* 0x96C */ be<s16> mPotAngleZ;
    /* 0x96E */ u8 _96E[0x2];
    /* 0x970 */ be<f32> mPotPosX;
    /* 0x974 */ be<f32> mPotPosY;
    /* 0x978 */ be<f32> mPotPosZ;
    /* 0x97C */ be<f32> mPropellerPosX;
    /* 0x980 */ be<f32> mPropellerPosY;
    /* 0x984 */ be<f32> mPropellerPosZ;
    /* 0x988 */ be<u32> mPropellerEmitter;
    /* 0x98C */ be<u32> mPotEmitterA;
    /* 0x990 */ be<u32> mPotEmitterB;
    /* 0x994 */ be<u32> mPotEmitterC;
    /* 0x998 */ be<u32> mPotPourEmitter;
    /* 0x99C */ be<u32> mLeftDanceEmitter;
    /* 0x9A0 */ be<u32> mRightDanceEmitter;
    /* 0x9A4 */ be<s16> mPotPourTimer;
    /* 0x9A6 */ u8 _9A6[2];
    /* 0x9A8 */ be<fpc_ProcID> mSeedIDs[4];
    /* 0x9B8 */ be<u8> mFlightState;
    /* 0x9B9 */ be<u8> mJumpState;
    /* 0x9BA */ be<u8> mWalkState;
    /* 0x9BB */ be<u8> mPrivateCut;
    /* 0x9BC */ be<u8> mEventAction;
    /* 0x9BD */ be<u8> mAnmAttribute;
    /* 0x9BE */ be<u8> mPendingAnmAttribute;
    /* 0x9BF */ be<u8> mAnimation;
    /* 0x9C0 */ be<u8> mEventOrder;
    /* 0x9C1 */ be<u8> mState;
    /* 0x9C2 */ be<u8> mPreviousState;
    /* 0x9C3 */ be<u8> mLookMode;
    /* 0x9C4 */ be<s8> mType; /* the Korok (charDecide) */
    /* 0x9C5 */ be<u8> mMessageType;
    /* 0x9C6 */ be<u8> mActionPhase;
    /* 0x9C7 */ be<u8> mDemoAnmPhase;
};
WWHD_OFFSET(daNpc_Bj1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Bj1_c, mEventCut2, 0x84C);
WWHD_SIZE(daNpc_Bj1_c, 0x9C8);

WWHD_OFFSET(daNpc_Bj1_c, mAction, 0x80C);
WWHD_OFFSET(daNpc_Bj1_c, mPropellerAttachMtx, 0x814);
WWHD_OFFSET(daNpc_Bj1_c, mPath, 0x844);
WWHD_OFFSET(daNpc_Bj1_c, mSeedIDs, 0x9A8);

// HD: private views describe only the fields used by the joint and search helpers.
struct Bj1JointData {
    be<u32> mResource;
    be<u16> mJointNo;
};
struct Bj1JointRecord {
    u8 _00[8];
    be<u32> mCallBack;
    u8 _0C[0x10];
};
struct Bj1Partner : fopAc_ac_c {
    u8 _3AC[0x28];
    be<u8> mActive;
};
struct Bj1SearchActors {
    gptr<fopAc_ac_c> mActors[20];
};
WWHD_OFFSET(Bj1JointData, mJointNo, 4);
WWHD_SIZE(Bj1JointRecord, 0x1C);
WWHD_OFFSET(Bj1Partner, mActive, 0x3D4);
WWHD_SIZE(Bj1SearchActors, 0x50);

enum { fpcNm_JB_e = 0xD5, fpcNm_NPC_BJ1_e = 0x150 };

// HD: word copies preserve the payloads of signaling NaNs in saved positions.
inline u32 Bj1_floatBits(const be<f32>& value) { return *reinterpret_cast<const be<u32>*>(&value); }
inline void Bj1_setFloatBits(be<f32>& value, u32 bits) {
    *reinterpret_cast<be<u32>*>(&value) = bits;
}

// HD: preserve the complete returned GPR and explicitly forwarded stale registers.
u32 Bj1_setLocFlagCarry(daNpc_Bj1_c* actor, int* value, u32 carry);
u32 Bj1_setShapeAngleCarry(void* actor, int* value, u32 angle);
void Bj1_setEventTimerCarry(void* actor, int* value);
BOOL Bj1_deleteSeedTail(daNpc_Bj1_c* actor);

s32 Bj1_targetAngleY(const cXyz* arg0, const cXyz* arg1);
f64 Bj1_rndFX(f32 arg0);
void Bj1_initAngles(csXyz* arg0, s32 arg1, s32 arg2, s32 arg3);
u32 Bj1_calcTimer(be<s16>* arg0);
u32 Bj1_getMaterialSound(dBgS* arg0, cBgS_PolyInfo* arg1);
u32 Bj1_getPolyColor(dBgS* arg0, cBgS_PolyInfo* arg1);
u32 Bj1_getRoomId(dBgS* arg0, cBgS_PolyInfo* arg1);
void Bj1_correctPosition(dBgS_ObjAcch* arg0, dBgS* arg1);
s32 Bj1_getMapTrans(s32 arg0, be<f32>* arg1, be<f32>* arg2, be<s16>* arg3);
u32 Bj1_getDemoActor(void* arg0, u32 arg1);
void Bj1_setDemoData(daNpc_Bj1_c* arg0, u32 arg1, mDoExt_McaMorf* arg2, const char* arg3, u32 arg4,
                     u32 arg5, u32 arg6, u32 arg7);
s32 Bj1_getMyStaffId(dEvent_manager_c* arg0, u32 arg1, u32 arg2, u32 arg3);
void Bj1_setGoal(dEvent_manager_c* arg0, cXyz* arg1);
u32 Bj1_getEventIndex(dEvent_manager_c* arg0, u32 arg1, u32 arg2);
BOOL Bj1_startCheck(dEvent_manager_c* arg0, s32 arg1);
u32 Bj1_getSubstance(dEvent_manager_c* arg0, s32 arg1, const char* arg2, s32 arg3);
void Bj1_setPathInfo(dNpc_PathRun_c* arg0, u32 arg1, s32 arg2, u32 arg3);
void Bj1_setPathDirect(dNpc_PathRun_c* arg0, dPath* arg1);
u32 Bj1_nextPath(dNpc_PathRun_c* arg0, s32 arg1);
void Bj1_getPathPoint(dNpc_PathRun_c* arg0, cXyz* arg1, u32 arg2);
u32 Bj1_maxPoint(dNpc_PathRun_c* arg0);
void Bj1_initEventCut(dNpc_EventCut_c* arg0);
void Bj1_setEventActor(dNpc_EventCut_c* arg0, u32 arg1, daNpc_Bj1_c* arg2);
u32 Bj1_nextRoomPath(dPath* arg0, s32 arg1);
u32 Bj1_isSymbol(dSv_player_collect_c* arg0, s32 arg1);
u32 Bj1_isEventBit(dSv_event_c* arg0, u32 arg1);
void Bj1_searchById(u32 arg0, be<u32>* arg1);
u32 Bj1_createItemForKP2(cXyz* arg0, u32 arg1, s32 arg2, csXyz* arg3, cXyz* arg4, s32 arg5,
                         f32 arg6, f32 arg7, f32 arg8);
void Bj1_searchActors(void* arg0, daNpc_Bj1_c* arg1);
void Bj1_startSound(u32 arg0, cXyz* arg1, u32 arg2, s32 arg3);
void Bj1_assert(const char* arg0, u32 arg1, const char* arg2);
u32 Bj1_allocate(u32 arg0);
void Bj1_setEmitterMatrix(Mtx34* arg0, Mtx34* arg1, cXyz* arg2);
// HD: setMtx forwards its integer argument without normalizing it to bool.
void Bj1_setAttentionRegister(daNpc_Bj1_c* actor, s32 force);

struct Bj1EventInteger {
    u8 _00[3];
    be<u8> mLowByte;
};
WWHD_SIZE(Bj1EventInteger, 4);

struct Bj1ResourceTable {
    be<s32> mIDs[9];
};
struct Bj1AnimationTable {
    daNpc_Bj1_c::anm_prm_c mRecords[11];
};
struct Bj1AnimationResources {
    be<u32> mIDs[11];
};
struct Bj1ScaleRecord {
    cXyz mHeadScale;
    cXyz mBackboneScale;
    cXyz mHeadOffset;
};
struct Bj1ScaleTable {
    Bj1ScaleRecord mRecords[9];
};
struct Bj1EventNames {
    be<u32> mNames[17];
};
struct Bj1StaffNames {
    be<u32> mNames[3];
};
struct Bj1HeapSizes {
    be<u32> mSizes[9];
};
struct Bj1ActionWords {
    be<u32> mWords[2];
};
WWHD_SIZE(Bj1ResourceTable, 0x24);
WWHD_SIZE(Bj1ScaleRecord, 0x24);
WWHD_SIZE(Bj1ActionWords, 8);

struct Bj1RelativeFrame {
    cXyz mInput;
    cXyz mOutput;
};
struct Bj1SeedFrame {
    csXyz mAngles;
    u8 _06[2];
    cXyz mScale;
    cXyz mPosition;
    be<s16> mChoices[4];
    gptr<fopAc_ac_c> mResults[4];
};
struct Bj1Emitter {
    u8 _00[0x5C];
    be<s32> mMaxFrame;
    u8 _60[0x190];
    Mtx34 mRotationMtx;
    u8 _220[0xC];
    cXyz mPosition;
    u8 _238[0x1C];
    be<u32> mStatus;
};
struct Bj1PotEmitters {
    be<u32> mEmitters[3];
};
WWHD_SIZE(Bj1RelativeFrame, 0x18);
WWHD_OFFSET(Bj1SeedFrame, mScale, 8);
WWHD_OFFSET(Bj1SeedFrame, mPosition, 0x14);
WWHD_OFFSET(Bj1SeedFrame, mResults, 0x28);
WWHD_SIZE(Bj1SeedFrame, 0x38);
WWHD_OFFSET(Bj1Emitter, mRotationMtx, 0x1F0);
WWHD_OFFSET(Bj1Emitter, mPosition, 0x22C);
WWHD_OFFSET(Bj1Emitter, mStatus, 0x254);
WWHD_SIZE(Bj1Emitter, 0x258);

enum { dItemNo_BOKOBABA_SEED_e = 0x46 };

void Bj1_deleteDrugPot(daNpc_Bj1_c* arg0);
void Bj1_initCollisionStatus(dCcD_Stts* arg0, u32 arg1, u32 arg2, daNpc_Bj1_c* arg3);
void Bj1_setCollisionCylinder(dCcD_Cyl* arg0, const void* arg1);
u32 Bj1_createParticle(dPa_control_c* arg0, u32 arg1, u32 arg2, const cXyz* arg3, const csXyz* arg4,
                       const cXyz* arg5, u32 arg6, void* arg7, s32 arg8, const GXColor* arg9,
                       const GXColor* arg10, const cXyz* arg11);
u32 Bj1_playMorf(mDoExt_McaMorf* arg0, cXyz* arg1, u32 arg2, s32 arg3);
struct Bj1PositionWords {
    be<u32> x, y, z;
};
struct Bj1HIORecord {
    be<u32> mVtable;
    /* 0x04 */ be<s16> mMaxHeadX;
    /* 0x06 */ be<s16> mMaxHeadY;
    /* 0x08 */ be<s16> mMinHeadX;
    /* 0x0A */ be<s16> mMinHeadY;
    /* 0x0C */ be<s16> mMaxBackboneX;
    /* 0x0E */ be<s16> mMaxBackboneY;
    /* 0x10 */ be<s16> mMinBackboneX;
    /* 0x12 */ be<s16> mMinBackboneY;
    /* 0x14 */ be<s16> mMaxTurnStep;
    /* 0x16 */ be<s16> mLookAngle;
    be<f32> mAttentionHeight;
    /* 0x1C */ be<u8> mDrawDebug;
    u8 _1D[7];
    /* 0x24 */ be<s16> mFlightDuration;
    /* 0x26 */ be<s16> mFlightTurnScale;
    /* 0x28 */ be<s16> mFlightTurnMax;
    u8 _2A[2];
    /* 0x2C */ be<f32> mFlightHeightThreshold;
    /* 0x30 */ be<f32> mFlightVerticalSpeed;
    /* 0x34 */ be<f32> mFlightVerticalStep;
    /* 0x38 */ be<f32> mFlightSpeed;
    /* 0x3C */ be<f32> mFlightSpeedStep;
    /* 0x40 */ be<f32> mLandingHeight;
    /* 0x44 */ be<s16> mPropellerSpeedMax;
    /* 0x46 */ be<s16> mPropellerSpeedStep;
    u8 _48[2];
    /* 0x4A */ be<s16> mFlightStartPropellerSpeed;
    /* 0x4C */ be<f32> mTalkDistance;
    /* 0x50 */ be<s16> mTalkAngle;
    /* 0x52 */ be<s16> mWalkTurnScale;
    /* 0x54 */ be<s16> mWalkTurnMax;
    u8 _56[2];
    /* 0x58 */ be<f32> mWalkAnimationScale;
    /* 0x5C */ be<f32> mWalkSpeed;
    /* 0x60 */ be<f32> mWalkSpeedStep;
    /* 0x64 */ be<f32> mWalkPassDistance;
    /* 0x68 */ be<u32> mIndex;
};
struct Bj1HIOCollection {
    Bj1HIORecord mRecords[9];
};
struct Bj1CylinderSource {
    u8 mBytes[0x44];
};
WWHD_SIZE(Bj1PositionWords, 0xC);
WWHD_OFFSET(Bj1HIORecord, mAttentionHeight, 0x18);
WWHD_SIZE(Bj1HIORecord, 0x6C);

void* Bj1_getResource(dRes_control_c* manager, SafeString* key, s32 id);
void mDoMtx_XYZrotM(Mtx34* matrix, s16 x, s16 y, s16 z);
void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* result);

struct Bj1MorfVtable {
    u8 _00[0xC];
    be<u32> mDelete;
};
struct Bj1VirtualMorf {
    gptr<Bj1MorfVtable> mVtable;
};
WWHD_OFFSET(Bj1MorfVtable, mDelete, 0xC);

struct Bj1InitActionWords {
    be<u32> first;
    be<u32> second;
};
struct Bj1AttentionFrame {
    Bj1PositionWords mPoint;
};
WWHD_SIZE(Bj1InitActionWords, 8);
WWHD_SIZE(Bj1AttentionFrame, 12);

struct Bj1ActionFrame {
    be<u32> linkage[2];
    be<u32> words[2];
};

struct Bj1ParticleFrame {
    be<u32> linkage[2];
    be<u32> args[4];
    be<s16> angle;
    be<u16> pad;
    be<f32> x;
    be<f32> z;
};

struct Bj1PlayerMoveFrame {
    be<u32> linkage[2];
    cXyz output;
    cXyz input;
};
struct Bj1MovePointFrame {
    be<u32> linkage[2];
    cXyz point;
};

WWHD_SIZE(Bj1ActionFrame, 0x10);
WWHD_SIZE(Bj1ParticleFrame, 0x24);
WWHD_SIZE(Bj1PlayerMoveFrame, 0x20);
WWHD_SIZE(Bj1MovePointFrame, 0x14);

// HD: state animation records precede the eleven animation-attribute records.
struct Bj1StateAnimations {
    daNpc_Bj1_c::anm_prm_c mRecords[9];
};
WWHD_SIZE(Bj1StateAnimations, 0x90);

// HD: full register widths are retained for event records.
u32 Bj1_getEventReg(dSv_event_c *events, u32 reg);
void Bj1_setEventReg(dSv_event_c *events, u32 reg, u32 value);
void Bj1_onEventBit(dSv_event_c *events, u32 bit);

void Bj1_addCalcAngle(be<s16> *angle, s32 target, s32 scale, s32 maximum, s32 minimum);
void Bj1_addCalcAngle2(be<s16> *angle, s32 target, s32 scale, s32 maximum);
void Bj1_chaseF(be<f32> *value, f32 target, f32 step);

struct Bj1ParameterCollection {
    be<u32> mVtable;
    be<u8> mHostIONode;
    u8 _05[3];
    be<s32> mSelected;
    Bj1HIORecord mRecords[9];
};
struct Bj1ParameterDefaults {
    u8 mBytes[0x64];
};
struct Bj1DefaultTable {
    Bj1ParameterDefaults mRecords[9];
};
WWHD_SIZE(Bj1ParameterCollection, 0x3D8);
WWHD_OFFSET(Bj1ParameterCollection, mRecords, 0xC);
using Bj1RecordConstructor = u32 (*)(void *);
void Bj1_constructArray(void *storage, u32 count, u32 size, Bj1RecordConstructor constructor);
#define BJ1_PARAMETER_VTBL 0x10016BBC
#define BJ1_COLLECTION_VTBL 0x10016BCC

// HD: stack records retain their measured guest layout.
struct Bj1LookFrame {
    cXyz mTarget;
    cXyz mOrigin;
    cXyz mScratch;
};

struct Bj1MaskFrame {
    be<u32> linkage[2];
    be<u8> mask;
};

struct Bj1PassFrame {
    cXyz mVector;
    cXyz mPosition;
    cXyz mHorizontal;
};

struct Bj1FlyMoveFrame {
    cXyz mPoint;
};

struct Bj1PointFrame {
    be<u32> linkage[2];
    cXyz output;
};

struct Bj1FlyFrame {
    cXyz mHorizontal;
    cXyz mDifference;
};

void Bj1_cutEnd(dEvent_manager_c *manager, s32 staff);
u32 Bj1_getIsAddvance(dEvent_manager_c *manager, s32 staff);
u32 Bj1_endCheck(dEvent_manager_c *manager, s32 event);

// HD: exact-width movement calls keep the hidden vector-result argument in r4.
void Bj1_subtractVector(const cXyz *left, cXyz *result, const cXyz *right);
f64 Bj1_squareMagnitude(const cXyz *vector);
f64 Bj1_squareRoot(f64 value);
s32 Bj1_windDirection();
void Bj1_playerEyePos(cXyz *result, f32 distance);
void Bj1_lookAtTarget(dNpc_JntCtrl_c *joints, be<s16> *angle, cXyz *target, cXyz *origin,
                      s32 heading, s32 limit, u32 flag);

struct Bj1CylinderDimensions {
    be<f32> mHeight;
    be<f32> mRadius;
};
struct Bj1CylinderTable {
    Bj1CylinderDimensions mRecords[9];
};
WWHD_SIZE(Bj1CylinderDimensions, 8);

struct Bj1PartnerState : fopAc_ac_c {
    u8 _3AC[0x2A];
    be<u8> mTalkAlternate;
};
WWHD_OFFSET(Bj1PartnerState, mTalkAlternate, 0x3D6);
struct Bj1MessageManager;
u32 Bj1_messageStatus(Bj1MessageManager *message);
void Bj1_talk(fopNpc_npc_c *actor, s32 mode);
u32 Bj1_checkEmptyBottle(dSv_player_item_c *items);
BOOL Bj1_isItem(dSv_player_get_item_c *items, s32 item, u32 slot);

u32 Bj1_lockonTruth(dAttention_c *attention);
fopAc_ac_c *Bj1_actionTarget(dAttention_c *attention, s32 index);
fopAc_ac_c *Bj1_lockonTarget(dAttention_c *attention, s32 index);
u32 Bj1_checkPresentEnd(dEvent_manager_c *manager);
void Bj1_registerFigure(u32 color, fopAc_ac_c *actor, f32 x, f32 y, f32 z);
void Bj1_moveActor(fopAc_ac_c *actor, dCcD_Stts *status);

// HD: compiler-owned debug colors, initialization words and registration nodes.
using Bj1GuestWord = be<u32>;
struct Bj1FigureColors {
    be<u8> mColors[9];
};
struct Bj1DebugDefaults {
    be<u32> mColors[4];
};
struct Bj1DebugColors {
    be<u32> mFourth, mThird;
    u8 _08[4];
    be<u32> mFirst, mSecond;
};
struct Bj1ConstructorState {
    be<u32> mWords[4];
};
struct Bj1InitialRange {
    be<f32> mLower, mUpper;
};
struct Bj1GlobalRegistration {
    be<u32> mWords[3];
};
struct Bj1GlobalRegistrations {
    Bj1GlobalRegistration mRecords[3];
};
struct Bj1RuntimeInitializer {
    be<u8> mState;
};
void Bj1_registerGlobal(Bj1GlobalRegistration *record);
void Bj1_initializeRuntimeA(Bj1RuntimeInitializer *state);
void Bj1_initializeRuntimeB(Bj1RuntimeInitializer *state);
u32 Bj1_deallocate(void *storage);
u32 Bj1_deallocateWithFlags(void *storage, u32 flags);
void Bj1_destroyAcch(dBgS_ObjAcch *collision, u32 flags);
u32 Bj1_destroyActor(fopAc_ac_c *actor, u32 flags);
#define BJ1_GROUP_PASS_VTBL 0x10016B9C
#define BJ1_POLY_PASS_VTBL 0x10016BAC

struct Bj1PrivateCutNames {
    gptr<const char> mNames[1];
};
u32 Bj1_getActIndex(dEvent_manager_c *manager, s32 staff, const gptr<const char> *names, s32 count,
                    u32 force, u32 prefix);

// HD: the common dPath is opaque; only its unchanged flag prefix is needed.
struct Bj1PathFlags {
    be<u16> m_num, m_nextID;
    be<u8> mArg0, m_closed;
};
WWHD_OFFSET(Bj1PathFlags, m_closed, 5);
enum { dSymbol_FARORE_e = 2 };

enum Bj1State {
    BJ1_STATE_WAIT_1 = 1,
    BJ1_STATE_TALK = 2,
    BJ1_STATE_FLY = 3,
    BJ1_STATE_FALL = 4,
    BJ1_STATE_WAIT_2 = 5,
    BJ1_STATE_WALK = 6,
    BJ1_STATE_WAIT_3 = 7,
    BJ1_STATE_WAIT_4 = 8,
};
enum Bj1LookMode {
    BJ1_LOOK_NONE = 0,
    BJ1_LOOK_PLAYER = 1,
    BJ1_LOOK_TARGET = 2,
    BJ1_LOOK_INITIAL_HEADING = 3,
};
WWHD_OFFSET(Bj1HIORecord, mIndex, 0x68);
WWHD_SIZE(Bj1DebugColors, 0x14);
WWHD_SIZE(Bj1ConstructorState, 0x10);
WWHD_SIZE(Bj1GlobalRegistration, 0xC);
WWHD_SIZE(Bj1LookFrame, 0x24);
WWHD_SIZE(Bj1PassFrame, 0x24);
WWHD_SIZE(Bj1FlyMoveFrame, 0xC);
WWHD_SIZE(Bj1FlyFrame, 0x18);

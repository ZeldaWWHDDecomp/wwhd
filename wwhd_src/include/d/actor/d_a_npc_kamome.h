/* daNpc_kam_c (Hyoi seagull, the seagull Link controls with a Hyoi Pear), WWHD layout.
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_kamome (profile: size 0xBCC,
 * GameCube 0xC28; the constructor is inlined in create ):
 * - daPy_npc_c base: HD 0x608 (shared daPy_npc_c layout).
 * - members from mPhs (GameCube 0x4EC) to mAcchCirs are +0x11C;
 * - HD: one dBgS_AcchCir instead of two (-0x40 from mLinChk on);
 * - HD: one dCcD_Sph instead of two (the GameCube mTgSph is gone; the hit checks use mAtSph):
 *   -0x50 net from mCps on (mCps 0x9EC, GameCube 0xA3C);
 * - mShadowId (GameCube 0xBC8) is gone (HD shadows) and the two pointers to member are 8 bytes
 *   (GHS) instead of 12: -0x5C from mHitFlags on;
 * - HD-only byte 0xBCA (GameCube padding 0xC26): set while the seagull waits hidden at its home
 *   (waitNpcAction), cleared by initialDescendEvent; draw and the movement part of execute skip
 *   while it is set.
 * Field names are the GameCube ones. */
#pragma once
#include "d/actor/d_a_player_npc.h"
#include "gc_api/d/actor/d_a_player.h"
#include "gc_api/d/d_item.h"
#include "gc_api/f_op/f_op_actor.h"

void kam_drawDamageFog(void*);
u32 kam_eventGetPId(u32,u32);
void kam_seStartSimple(u32);
s32 kam_getMyActIdx(dEvent_manager_c*,s32,u32,s32,s32,s32);
void kam_cutEnd(dEvent_manager_c*,s32);
BOOL kam_getIsAdvance(dEvent_manager_c*,s32);
void kam_report(s32,s32,const char*);
BOOL kam_checkPolySafe(dBgS*,u32);
u32 kam_getMaterialSound(dBgS*,u32);
BOOL kam_morfPlay(mDoExt_McaMorf*,cXyz*,u32,s32);
BOOL kam_padRorStart(s32);
void kam_massSet(u32,void*,s32);
void kam_setCapsuleStartEnd(u32,cXyz*,cXyz*);
void kam_setCapsuleSource(dCcD_Cps*,u32);
void kam_constructPlayer(void*);
void kam_constructSphere(dCcD_Sph*);
void kam_constructObjectInfo(void*);
void kam_constructCapsuleGeometry(u32);
void kam_destroyObjectInfo(void*,s32);
void kam_destroySphere(void*,s32);
void kam_destroyStatus(void*,s32);
void kam_destroyLineCheck(void*,s32);
void kam_destroyCircleGeometry(u32,s32);
void kam_destroyAcch(void*,s32);
void kam_destroyPlayer(void*,s32);
void kam_emptyBaitItem(u32);
s16 kam_cameraAngleY(u32);
BOOL kam_padTriggerA(s32);
void kam_setAnimation(mDoExt_McaMorf*,s32,f32,f32,s32,s32,const char*);
s32 kam_randomValue(s32,s32);
BOOL kam_checkPositionMove(fopAc_ac_c*,const char*);
void kam_destroyCheck(void*,s32);
s32 kam_getGroundRoom(dBgS*,u32);
u32 kam_getGroundColor(dBgS*,u32);
void kam_clearSphereCollision(void*);
void kam_changePlayer(fopAc_ac_c*,fopAc_ac_c*);
BOOL kam_startCheckOld(dEvent_manager_c*,const char*);
BOOL kam_endCheckOld(dEvent_manager_c*,const char*);
void kam_rotateZXY(Mtx34*,s16,s16,s16);
s32 kam_timer(be<s16>*);
s32 kam_timerByte(be<u8>*);
f32 kam_stickX(s32);
f32 kam_stickY(s32);
struct KamomeNameTable;
KamomeNameTable* kam_jointNames(J3DModelData*);
s32 kam_jointIndex(u32,const char*);


struct KamomeAudioInterface {
    u8 padding[0x72];
    be<u8> mHyoiKamome;
};
struct KamomeNameTable {
    u8 padding[0x10];
    be<u32> mNameOffset;
};
struct KamomeJointEntry {
    u8 padding[8];
    be<u32> mCallBack;
    u8 tail[0x1C - 12];
};
struct KamomeJointTable {
    u8 padding[4];
    be<u32> mCount;
    gptr<KamomeJointEntry> mpJoints;
};
struct KamomeSafeStringVtable {
    u8 padding[0x14];
    be<u32> assureTermination;
};
struct KamomeSafeString {
    gptr<be<u8>> mStringTop;
    gptr<KamomeSafeStringVtable> mpVtable;
};
WWHD_SIZE(KamomeSafeString, 8);
extern gptr<KamomeAudioInterface>& l_audio;


struct KamomeCapsuleGeometry { cM3dGLin mLine; be<f32> mRadius; };
struct KamomeCapsule : dCcD_GObjInf {
    u8 padding[0x18];
    be<u32> mGeometryVtable;
    be<u32> mShapeVtable;
    KamomeCapsuleGeometry mCps;
};
WWHD_SIZE(KamomeCapsule, 0x138);
WWHD_OFFSET(KamomeCapsule, mGeometryVtable, 0x110);
WWHD_OFFSET(KamomeCapsule, mShapeVtable, 0x114);
WWHD_OFFSET(KamomeCapsule, mCps, 0x118);
struct KamomeCapsuleSource {
    u8 padding[0x48];
    be<f32> mRadius;
};
struct KamomeEventTimer { u8 padding[2]; be<s16> mValue; };
struct KamomeEmitter {
    u8 padding[0x220];
    cXyz mGlobalScale;
    u8 betweenScale[0x238 - 0x22C];
    cXyz mGlobalScaleOut;
};
extern dCcD_SrcSph& l_sph_src;
extern KamomeCapsuleSource& l_kam_at_cps_src;
extern dPa_levelEcallBack& l_rippleCallback;


enum KamomeResource {
    dRes_INDEX_KAMOME_BAS_KA_WAIT1_e = 8,
    dRes_INDEX_KAMOME_BAS_KA_WAIT2_e = 9,
    dRes_INDEX_KAMOME_BCK_KA_SING1_e = 16,
    dRes_INDEX_KAMOME_BCK_KA_WAIT1_e = 18,
    dRes_INDEX_KAMOME_BCK_KA_WAIT2_e = 19,
    dRes_INDEX_KAMOME_BDL_KA_HYOI_e = 24,
};
enum KamomeSound : u32 {
    JA_SE_CTRL_NPC_TO_LINK = 0x886,
    JA_SE_HYOI_USE_DEMO = 0x8EF,
    JA_SE_CV_KAMOME = 0x4805,
};
enum KamomePlayerStatus {
    dActStts_RETURN_e = 7,
    dActStts_FLY_e = 0x23,
    dActStts_HIDDEN_e = 0x3E,
};


WWHD_SIZE(KamomeAudioInterface, 0x73);
WWHD_OFFSET(KamomeAudioInterface, mHyoiKamome, 0x72);
WWHD_SIZE(KamomeNameTable, 0x14);
WWHD_OFFSET(KamomeNameTable, mNameOffset, 0x10);
WWHD_SIZE(KamomeJointEntry, 0x1C);
WWHD_OFFSET(KamomeJointEntry, mCallBack, 8);
WWHD_SIZE(KamomeJointTable, 0xC);
WWHD_OFFSET(KamomeJointTable, mCount, 4);
WWHD_OFFSET(KamomeJointTable, mpJoints, 8);
WWHD_SIZE(KamomeSafeStringVtable, 0x18);
WWHD_OFFSET(KamomeSafeStringVtable, assureTermination, 0x14);
WWHD_SIZE(KamomeCapsuleGeometry, 0x20);
WWHD_SIZE(KamomeCapsuleSource, 0x4C);
WWHD_OFFSET(KamomeCapsuleSource, mRadius, 0x48);
WWHD_SIZE(KamomeEventTimer, 4);
WWHD_OFFSET(KamomeEventTimer, mValue, 2);
WWHD_SIZE(KamomeEmitter, 0x244);
WWHD_OFFSET(KamomeEmitter, mGlobalScale, 0x220);
WWHD_OFFSET(KamomeEmitter, mGlobalScaleOut, 0x238);

// HD: GHS actions retain a signed this-adjustment and virtual index in eight bytes.
struct KamomeAction {
    be<s16> d;
    be<s16> i;
    be<u32> f;
};
WWHD_SIZE(KamomeAction, 8);
struct KamomeJointNumber {
    u8 padding[4];
    be<u16> mJointNo;
};
WWHD_OFFSET(KamomeJointNumber, mJointNo, 4);
struct KamomePolyInfo {
    be<u16> mPolyIndex;
    be<u16> mBgIndex;
    be<u32> mpBgW;
    be<s32> mProcId;
    be<u32> __vtbl;
};
WWHD_SIZE(KamomePolyInfo, 0x10);
inline void kam_copyAction(KamomeAction* dst, const void* src) {
    auto* words = reinterpret_cast<const be<u32>*>(src);
    auto* output = reinterpret_cast<be<u32>*>(dst);
    output[0] = static_cast<u32>(words[0]);
    output[1] = static_cast<u32>(words[1]);
}
struct daNpc_kam_c;
template<class R = BOOL, class A>
inline R kam_callAction(void* self, KamomeAction* action, A arg) {
    auto* method = reinterpret_cast<gabi::ptmf<R (daNpc_kam_c::*)(A)>*>(action);
    return (static_cast<daNpc_kam_c*>(self)->* *method)(arg);
}

extern be<u8>& l_hyoi_kamome;
extern be<u32>& l_act;
extern gptr<char>& l_staff_name;
extern be<s16>& l_demo_start_chk_cnt;
extern be<s16>& l_demo_start_chk_flag;
extern be<u32>& offset_guard;
extern be<u32>& offsetAttPos_guard;
extern be<u32>& center_guard;
extern be<u32>& splash_guard;
extern be<u32>& ripple_guard;
extern cXyz& l_ms_at_local_vec;
extern cXyz& l_ms_at_local_start;
extern cXyz& l_ms_at_local_end;
extern cXyz& l_line_bg_local_end;
extern cXyz& l_call_local_kyori;
extern cXyz& l_offset;
extern cXyz& l_offsetAttPos;
extern cXyz& l_center;
extern cXyz& splash_scale;
extern cXyz& ripple_scale;

// HD: this unit's actor and SafeString vtables.
#define KAM_VTBL 0x1001B720           /* daNpc_kam_c vtable (create ) */
#define KAM_SAFESTRING_VTBL 0x1001B454 /* this TU's sead::SafeString vtable */

/* daNpc_kam_HIO1_c (0x2C; GHS: vtable pointer after the members) */
struct daNpc_kam_HIO1_c {
    /* 0x00 */ be<f32> mSpeedF;
    /* 0x04 */ be<f32> mUnused08;
    /* 0x08 */ be<f32> mFlappingSpeedF;
    /* 0x0C */ be<f32> mAccelF;
    /* 0x10 */ be<s16> mGlidingAngVelY;
    /* 0x12 */ be<s16> mGlidingAngVelX;
    /* 0x14 */ be<s16> mMaxAngleZ;
    /* 0x16 */ be<s16> mFlappingAngVelY;
    /* 0x18 */ be<s16> mFlappingAngVelX;
    /* 0x1A */ be<s16> mAngVelStepScale;
    /* 0x1C */ be<s16> mAngVelMaxStep;
    /* 0x1E */ be<s16> mAngVelMinStep;
    /* 0x20 */ be<s16> mFlapDuration;
    /* 0x22 */ be<s16> mFlapExhaustedDuration;
    /* 0x24 */ be<s16> mFlapEnergyDuration;
    /* 0x26 */ u8 _26[2];
    /* 0x28 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_kam_HIO1_c, 0x2C);


struct KamomeHioPrm {
    be<f32> m08;
    be<f32> m0C;
    be<f32> m10;
    be<f32> m14;
    be<f32> m18;
    be<s16> m1C;
    be<u8> m1E;
    u8 padding;
};
WWHD_SIZE(KamomeHioPrm, 0x18);

/* daNpc_kam_HIO_c (0x50; vtable pointer last) */
struct daNpc_kam_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ KamomeHioPrm prm;       /* hio_prm_c (GameCube 0x08..0x1F) */
    /* 0x1C */ be<u32> mpActor;
    /* 0x20 */ daNpc_kam_HIO1_c mHio1;
    /* 0x4C */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_kam_HIO_c, 0x50);
extern daNpc_kam_HIO_c& l_HIO;


struct daNpc_kam_c : daPy_npc_c {
    enum ActionStatus {
        ACTION_STARTING = 0,
        ACTION_ONGOING_1 = 1,
        ACTION_ONGOING_2 = 2,
        ACTION_ONGOING_3 = 3,
        ACTION_ENDING = -1,
    };
    enum Animation { ANM_WAIT1 = 0, ANM_WAIT2 = 1, ANM_SING = 2 };

    void offLineHit() { mHitFlags = mHitFlags & ~7u; }
    u32 isFrontLineHit() { return mHitFlags & 1; }
    u32 isLeftLineHit() { return mHitFlags & 2; }
    u32 isRightLineHit() { return mHitFlags & 4; }
    u32 isWaterHit() { return mHitFlags & 8; }
    u32 isNoBgCheck() { return mHitFlags & 0x10; }
    static void onHyoiKamome() { l_hyoi_kamome = 1; }
    static void offHyoiKamome() { l_hyoi_kamome = 0; }
    /* daPy_npc_c inlines (m4E4) */
    void returnLink() { m4E4 = m4E4 | 1; }
    void offReturnLink() { m4E4 = m4E4 & ~1u; }
    u32 isReturnLink() { return m4E4 & 1; }
    void onEventAccept() { m4E4 = m4E4 | 2; }
    void offEventAccept() { m4E4 = m4E4 & ~2u; }
    u32 isEventAccept() { return m4E4 & 2; }

    s16 XyCheckCB(int);
    BOOL callDemoStartCheck();
    s16 XyEventCB(int);
    void setAttention(bool); /* HD: the GameCube int parameter is not read */
    void setBaseMtx();
    BOOL createHeap();
    cPhs_State create();
    BOOL init();
    BOOL setAction(KamomeAction*, KamomeAction*, void*);
    void npcAction(void*);
    void setNpcAction(KamomeAction*, void*);
    void playerAction(void*);
    void setPlayerAction(KamomeAction*, void*);
    BOOL returnLinkCheck();
    BOOL changeAreaCheck();
    BOOL areaOutCheck();
    BOOL getStickAngY(be<s16>*, be<s16>*);
    s16 getAngleX();
    int wallHitCheck();
    BOOL waitNpcAction(void*);
    BOOL keyProc();
    BOOL waitPlayerAction(void*);
    BOOL damagePlayerAction(void*);
    void eventOrder();
    BOOL checkCommandTalk();
    void returnLinkPlayer();
    BOOL eventProc();
    void eventEnd();
    BOOL actionDefault(int);
    void initialWaitEvent(int);
    BOOL actionWaitEvent(int);
    void initialChangeEvent(int);
    void initialDescendEvent(int);
    BOOL actionDescendEvent(int);
    void initialAreaOutTurn(int);
    BOOL actionAreaOutTurn(int);
    void setAnm(int);
    void setCollision();
    void setLineBgCheck();
    void animationPlay();
    BOOL execute();
    BOOL draw();

    /* 0x608 */ request_of_phase_process_class mPhs;    /* GameCube 0x4EC */
    /* 0x610 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x614 */ dBgS_ObjAcch mAcch;                     /* GameCube 0x4F8 */
    /* 0x7D8 */ dBgS_AcchCir mAcchCir;                  /* HD: one (GameCube mAcchCirs[2], 0x6BC) */
    /* 0x818 */ dBgS_LinChk mLinChk;                       /* dBgS_LinChk, GameCube 0x73C */
    /* 0x884 */ dCcD_Stts mStts;                        /* GameCube 0x7A8 */
    /* 0x8C0 */ dCcD_Sph mAtSph;                        /* GameCube 0x7E4; HD: no mTgSph */
    /* 0x9EC */ dCcD_Cps mCps;                          /* GameCube 0xA3C */
    /* 0xB24 */ cXyz mDescendStartPos;                  /* GameCube 0xB74 */
    /* 0xB30 */ cXyz mDescendStartPosUnangled;
    /* 0xB3C */ csXyz mDescendStartAngle;
    /* 0xB42 */ u8 mB92[0xB50 - 0xB42];
    /* 0xB50 */ KamomePolyInfo mPolyInfo;              /* GameCube 0xBA0 */
    /* 0xB60 */ KamomeAction mCurrNpcActionFunc;          /* GameCube 0xBB0 (12 bytes) */
    /* 0xB68 */ KamomeAction mCurrPlayerActionFunc;       /* GameCube 0xBBC */
    /* HD: no mShadowId (GameCube 0xBC8) */
    /* 0xB70 */ be<u32> mHitFlags;                      /* GameCube 0xBCC */
    /* 0xB74 */ u8 mBD0[4];
    /* 0xB78 */ be<s32> mAnmIdx;
    /* 0xB7C */ be<f32> mMaxY;
    /* 0xB80 */ be<f32> mMinY;
    /* 0xB84 */ be<f32> mPrevMorfFrame;
    /* 0xB88 */ be<f32> mTargetSpeedF;
    /* 0xB8C */ be<f32> mAccelF;
    /* 0xB90 */ be<s16> mTargetAngleX;
    /* 0xB92 */ be<s16> mLockAngleXTimer;
    /* 0xB94 */ be<s16> mAngVelY;
    /* 0xB96 */ be<s16> mAngVelX;
    /* 0xB98 */ be<s16> mTargetAngVelY;
    /* 0xB9A */ be<s16> mTargetAngVelX;
    /* 0xB9C */ be<s16> m_jnt_body;
    /* 0xB9E */ be<s16> mFlapTimer;
    /* 0xBA0 */ be<s16> mFlapExhaustedTimer;
    /* 0xBA2 */ be<s16> mFlapEnergyTimer;
    /* 0xBA4 */ be<s8> mAnmTblIdx;
    /* 0xBA5 */ be<s8> mReachedAnimEnd;
    /* 0xBA6 */ be<s8> mEventState;
    /* 0xBA7 */ be<s8> mCurrEventIdxIdx;
    /* 0xBA8 */ u8 mC04[1];
    /* 0xBA9 */ be<s8> mActionStatus;
    /* 0xBAA */ be<u8> mUnusedC06;
    /* 0xBAB */ u8 mC07[1];
    /* 0xBAC */ be<s16> mC08;
    /* 0xBAE */ be<s16> mC0A;
    /* 0xBB0 */ be<s16> mC0C;
    /* 0xBB2 */ be<s16> mUnusedC0E;
    /* 0xBB4 */ u8 mC10[4];
    /* 0xBB8 */ be<f32> mUnusedC14;
    /* 0xBBC */ u8 mC18[4];
    /* 0xBC0 */ be<s16> mWaitTimer;                     /* GameCube 0xC1C */
    /* 0xBC2 */ be<s16> mAreaOutTimer;
    /* 0xBC4 */ be<s16> mEventIdxs[3];
    /* 0xBCA */ be<u8> mHidden;                         /* HD-only (GameCube padding 0xC26) */
    /* 0xBCB */ u8 _BCB[1];
};
WWHD_OFFSET(daNpc_kam_c, mPhs, 0x608);
WWHD_OFFSET(daNpc_kam_c, mAcch, 0x614);
WWHD_OFFSET(daNpc_kam_c, mLinChk, 0x818);
WWHD_OFFSET(daNpc_kam_c, mAtSph, 0x8C0);
WWHD_OFFSET(daNpc_kam_c, mCps, 0x9EC);
WWHD_OFFSET(daNpc_kam_c, mDescendStartPos, 0xB24);
WWHD_OFFSET(daNpc_kam_c, mPolyInfo, 0xB50);
WWHD_OFFSET(daNpc_kam_c, mHitFlags, 0xB70);
WWHD_OFFSET(daNpc_kam_c, mActionStatus, 0xBA9);
WWHD_OFFSET(daNpc_kam_c, mEventIdxs, 0xBC4);
WWHD_SIZE(daNpc_kam_c, 0xBCC);

/* ---- this TU's statics (HD addresses) ---- */


using KamomeActionSignature = BOOL (daNpc_kam_c::*)(void*);
struct KamomeActionEntry { gabi::ptmf<KamomeActionSignature> action; };
using KamomeEventInitSignature = void (daNpc_kam_c::*)(int);
using KamomeEventActionSignature = BOOL (daNpc_kam_c::*)(int);
struct KamomeEventInit { gabi::ptmf<KamomeEventInitSignature> action; };
struct KamomeEventAction { gabi::ptmf<KamomeEventActionSignature> action; };
WWHD_SIZE(KamomeActionEntry, 8);
WWHD_SIZE(KamomeEventInit, 8);
WWHD_SIZE(KamomeEventAction, 8);
struct KamomeAnmPrm {
    be<s8> mAnmTblIdx;
    u8 padding[3];
    be<s32> mLoopMode;
    be<f32> mMorf;
    be<f32> mPlaySpeed;
    be<s32> m10;
};
struct KamomeAnmTbl { be<s8> mBckFileIdx; be<s8> mSoundFileIdx; };
WWHD_SIZE(KamomeAnmPrm, 0x14);
WWHD_SIZE(KamomeAnmTbl, 2);

/* ---- unit-private HD bindings ---- */
static inline void kam_copyWords(void* dst, const void* src, int n) {
    auto* output = reinterpret_cast<be<u32>*>(dst);
    auto* input = reinterpret_cast<const be<u32>*>(src);
    for (int i = 0; i < n; i++) output[i] = static_cast<u32>(input[i]);
}
static inline void kam_setVec(cXyz* vector, f32 x, f32 y, f32 z) {
    vector->x = x;
    vector->y = y;
    vector->z = z;
}
/* eventInfo.mCommand (actor + 0xF8) */
/* mDoAud_zelAudio_c::getInterface()->field_0x0062 (HD: the interface pointer at , +0x72) */
static inline void kam_setAudioFlag(u8 v) { l_audio->mHyoiKamome = v; }
/*  daPy_py_c::changePlayer(fopAc_ac_c*) */
static inline void daPy_py_changePlayer(fopAc_ac_c* self, fopAc_ac_c* a) { kam_changePlayer(self, a); }
/*  /  dEvent_manager_c::startCheckOld / endCheckOld(const char*) */
static inline BOOL dEvmng_startCheckOld(const char* name) { return kam_startCheckOld(dComIfGp_getPEvtManager(), name); }
static inline BOOL dEvmng_endCheckOld(const char* name) { return kam_endCheckOld(dComIfGp_getPEvtManager(), name); }
static inline void mDoMtx_ZXYrotS_kam(Mtx34* m, s16 x, s16 y, s16 z) { kam_rotateZXY(m, x, y, z); }
/*  cLib_calcTimer<s16> (out of line; the whole r3 is tested) */
static inline s32 kam_calcTimer(be<s16>* t) { return kam_timer(t); }
/*  cLib_calcTimer<u8> */
static inline s32 kam_calcTimerU8(be<u8>* t) { return kam_timerByte(t); }
/*  cLib_getRndValue<int>(base, range) (another TU's copy) */
static inline s32 kam_getRndValue(s32 base, s32 range) { return kam_randomValue(base, range); }
/*  /  CPad_GET_STICK_POS_X / _Y(port) (HD: out of line) */
static inline f32 CPad_GET_STICK_POS_X_kam() { return kam_stickX(0); }
static inline f32 CPad_GET_STICK_POS_Y_kam() { return kam_stickY(0); }
/* HD J3D: a model's joint matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices);
 * getAnmMtx marks them dirty */
static inline Mtx34* kam_getAnmMtx(J3DModel* model, u32 jntNo) {
    return model->getAnmMtx(static_cast<s32>(jntNo));
}
static inline u32 kam_getJointName(J3DModelData* d) {
    KamomeNameTable* table = kam_jointNames(d);
    u32 off = table->mNameOffset;
    return off != 0 ? gabi::ea(&table->mNameOffset) + off : 0;
}
static inline s32 JUTNameTab_getIndex_kam(u32 tab, const char* name) { return kam_jointIndex(tab, name); }
/* HD: getJointNodePointer(idx) is bounds-checked (count +4, array +8, 0x1C each); setCallBack at +8 */
static inline void kam_setJointCallBack(J3DModelData* d, u16 idx, J3DJointCallBack cb) {
    auto* table = reinterpret_cast<KamomeJointTable*>(d);
    u32 count = table->mCount;
    KamomeJointEntry* entry = table->mpJoints;
    if (idx < count) entry += idx;
    entry->mCallBack = gabi::guest_fn(reinterpret_cast<const void*>(cb));
}
/* (a - b).absXZ() */
static inline f32 kam_absXZ_delta(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> d;
    cXyz_mi(a, d, b);
    gabi::Local<cXyz> xz;
    xz->x = d->x;
    xz->y = 0.0f;
    xz->z = d->z;
    return std_sqrtf(PSVECSquareMag(xz));
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0, HD: sead::SafeString operator== */
static inline bool kam_isStartStage(const char* lit) {
    gabi::Local<KamomeSafeString> sa;
    sa->mStringTop.v = gabi::guest_addr(lit);
    sa->mpVtable.v = KAM_SAFESTRING_VTBL;
    u32 stage = dComIfGp_ea() + 0x5134;
    gabi::Local<KamomeSafeString> sb;
    sb->mStringTop.v = stage;
    sb->mpVtable.v = KAM_SAFESTRING_VTBL;
    gabi::call_ptr(sa->mpVtable->assureTermination, sa.get());
    gabi::call_ptr(sa->mpVtable->assureTermination, sa.get());
    const be<u8>* pa = sa->mStringTop;
    gabi::call_ptr(sb->mpVtable->assureTermination, sb.get());
    const be<u8>* pb = sb->mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = pa[i];
        if (ca != pb[i]) return false;
        if (ca == 0) return true;
    }
    return false;
}

/* am_class (Armos), WWHD layout. 
 *
 * GameCube -> WWHD: every member is +0x11C (fopEn_enemy_c grew by 0x11C; the members keep their
 * GameCube sizes: dPa_smokeEcallBack 0x20, enemyice 0x3B8, enemyfire HD 0x22C).
 * Size 0x11BC (constructor 0204B72C). */
#pragma once
#include "d/d_cc_uty.h"
#include "gc_api/gc_api.h"
class JntHit_c;

#ifndef WWHD_ENEMYICE_FIRE_L
#define WWHD_ENEMYICE_FIRE_L
/* enemyice / enemyfire (c_damagereaction), WWHD layouts (SHARED-CANDIDATE, as in d_a_cc.h) */
struct enemyice {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFreezeDuration;
    /* 0x006 */ be<s8> mLightShrinkTimer;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<f32> mYOffset;
    /* 0x00C */ be<s8> m00C;
    /* 0x00D */ be<s8> mMode;
    /* 0x00E */ be<s16> mFreezeTimer;
    /* 0x010 */ be<s16> mMoveDelayTimer;
    /* 0x012 */ be<s16> mAngleY;
    /* 0x014 */ be<s16> mAngularVelY;
    /* 0x016 */ u8 _016[2];
    /* 0x018 */ cXyz mSpeed;
    /* 0x024 */ be<f32> mSpeedF;
    /* 0x028 */ be<f32> m028;
    /* 0x02C */ be<f32> m02C;
    /* 0x030 */ dCcD_Stts mStts;
    /* 0x06C */ dCcD_Cyl mCyl;
    /* 0x19C */ be<f32> mCylHeight;
    /* 0x1A0 */ be<f32> mWallRadius;
    /* 0x1A4 */ be<f32> mScaleXZ;
    /* 0x1A8 */ be<f32> mScaleY;
    /* 0x1AC */ be<f32> mParticleScale;
    /* 0x1B0 */ be<u8> m1B0;
    /* 0x1B1 */ be<u8> mDeathSwitch;
    /* 0x1B2 */ u8 _1B2[2];
    /* 0x1B4 */ dBgS_AcchCir mBgAcchCir;
    /* 0x1F4 */ dBgS_ObjAcch mBgAcch;
};
WWHD_SIZE(enemyice, 0x3B8);

struct enemyfire {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFireDuration;
    /* 0x006 */ be<s8> mMode;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<s16> mFireTimer;
    /* 0x00A */ u8 _00A[2];
    /* 0x00C */ gptr<mDoExt_McaMorf> mpMcaMorf;
    /* 0x010 */ be<s8> mFlameJntIdxs[10];
    /* 0x01A */ u8 _01A[2];
    /* 0x01C */ be<f32> mParticleScale[10];
    /* 0x044 */ be<s16> mFlameTimers[10];
    /* 0x058 */ be<u32> mpFlameEmitters[10];
    /* 0x080 */ cXyz mPrevPos;
    /* 0x08C */ cXyz mDirection;
    /* 0x098 */ be<f32> mFlameScaleY;
    /* 0x09C */ u8 _09C;
    /* 0x09D */ be<u8> mHitboxFlameIdx;
    /* 0x09E */ u8 _09E[2];
    /* 0x0A0 */ dCcD_Stts mStts;
    /* 0x0DC */ dCcD_Sph mSph;
    /* 0x208 */ u8 mLight[0x20]; /* LIGHT_INFLUENCE */
    /* 0x228 */ be<f32> m228;    /* HD: 1.0f in the constructor */
};
WWHD_SIZE(enemyfire, 0x22C);
#endif

struct am_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<JntHit_c> mEyeJntHit;
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ u8 _3D8[4];
    /* 0x3DC */ be<u8> mType;
    /* 0x3DD */ be<u8> mPrmAreaRadius;
    /* 0x3DE */ be<u8> mStartsInactive;
    /* 0x3DF */ be<u8> mSwitch;
    /* 0x3E0 */ be<u8> mAction;
    /* 0x3E1 */ be<u8> mMode;
    /* 0x3E2 */ be<u8> mHugeKnockback;
    /* 0x3E3 */ be<u8> mbIsBodyBeingHit;
    /* 0x3E4 */ be<s16> mCountDownTimers[4];
    /* 0x3EC */ be<s16> mCountUpTimers[5];
    /* 0x3F6 */ be<s16> mTargetAngleY;
    /* 0x3F8 */ be<s16> mSpawnRotY;
    /* 0x3FA */ u8 _3FA[2];
    /* 0x3FC */ be<s32> mCurrBckIdx;
    /* 0x400 */ be<u32> mSwallowedActorPID;
    /* 0x404 */ be<f32> mAreaRadius;
    /* 0x408 */ be<f32> mCorrectionOffsetY;
    /* 0x40C */ be<f32> mSpawnPosY;
    /* 0x410 */ cXyz mEyeballPos;
    /* 0x41C */ cXyz mMouthPos;
    /* 0x428 */ cXyz mWaistPos;
    /* 0x434 */ cXyz mJawPos;
    /* 0x440 */ cXyz mSpawnPos;
    /* 0x44C */ csXyz mEyeRot;
    /* 0x452 */ csXyz mTargetEyeRot;
    /* 0x458 */ gptr<JPABaseEmitter> m033C;
    /* 0x45C */ gptr<JPABaseEmitter> m0340;
    /* 0x460 */ dPa_smokeEcallBack mSmokeCbs[4];
    /* 0x4E0 */ dBgS_AcchCir mAcchCir;
    /* 0x520 */ dBgS_ObjAcch mAcch;
    /* 0x6E4 */ dCcD_Stts mStts;
    /* 0x720 */ dCcD_Sph mEyeSph;
    /* 0x84C */ dCcD_Sph mMouthSph;
    /* 0x978 */ dCcD_Cyl mBodyCyl;
    /* 0xAA8 */ dCcD_Cyl mNeedleCyl;
    /* 0xBD8 */ enemyice mEnemyIce;
    /* 0xF90 */ enemyfire mEnemyFire;
};
WWHD_OFFSET(am_class, mPhase, 0x3C8);
WWHD_OFFSET(am_class, mCurrBckIdx, 0x3FC);
WWHD_OFFSET(am_class, mEyeRot, 0x44C);
WWHD_OFFSET(am_class, mSmokeCbs, 0x460);
WWHD_OFFSET(am_class, mAcch, 0x520);
WWHD_OFFSET(am_class, mStts, 0x6E4);
WWHD_OFFSET(am_class, mNeedleCyl, 0xAA8);
WWHD_OFFSET(am_class, mEnemyIce, 0xBD8);
WWHD_OFFSET(am_class, mEnemyFire, 0xF90);
WWHD_SIZE(am_class, 0x11BC);

/* resource indices (GameCube res/Object/Am.h; the WWHD code uses the same numbers) */
enum {
    dRes_INDEX_AM_BCK_BOM_NOMI_e = 0x14,
    dRes_INDEX_AM_BCK_CLOSE_e = 0x15,
    dRes_INDEX_AM_BCK_CLOSE_LOOP_e = 0x16,
    dRes_INDEX_AM_BCK_DAMAGE_e = 0x17,
    dRes_INDEX_AM_BCK_DAMAGE_END_e = 0x18,
    dRes_INDEX_AM_BCK_DAMAGE_LOOP_e = 0x19,
    dRes_INDEX_AM_BCK_DEAD_e = 0x1A,
    dRes_INDEX_AM_BCK_OKIRU_e = 0x1B,
    dRes_INDEX_AM_BCK_OPEN_e = 0x1C,
    dRes_INDEX_AM_BCK_OPEN_LOOP_e = 0x1D,
    dRes_INDEX_AM_BCK_SLEEP_e = 0x1E,
    dRes_INDEX_AM_BCK_SLEEP_LOOP_e = 0x1F,
    dRes_INDEX_AM_BDL_AM_e = 0x22,
};
enum { AM_JNT_KOSI_e = 1, AM_JNT_AGO_e = 2, AM_JNT_EYE_e = 4 };

enum {
    ACTION_DOUSA = 0,
    ACTION_MODORU_MOVE = 1,
    ACTION_HANDOU_MOVE = 2,
    ACTION_ITAI_MOVE = 3,
};
enum {
    MODE_DOUSA_INIT = 0,
    MODE_DOUSA_OKIRU = 2,
    MODE_DOUSA_SLEEP_INIT = 9,
    MODE_DOUSA_SLEEP_MAIN = 10,
    MODE_MODORU_MOVE_INIT = 20,
    MODE_MODORU_MOVE_MAIN = 21,
    MODE_MODORU_MOVE_END = 22,
    MODE_HANDOU_MOVE_INIT = 30,
    MODE_HANDOU_MOVE_MAIN = 31,
    MODE_ITAI_MOVE_INIT = 40,
};

/* sound ids */
enum {
    JA_SE_LK_SW_HIT_S = 0x2803,
    JA_SE_LK_LAST_HIT = 0x2828,
    JA_SE_LK_W_WEP_HIT = 0x2833,
    JA_SE_LK_MS_WEP_HIT = 0x2834,
    JA_SE_LK_HAMMER_HIT = 0x2855,
    JA_SE_CV_AM_AWAKE = 0x4874,
    JA_SE_CV_AM_JUMP = 0x4875,
    JA_SE_CV_AM_OPEN_MOUTH = 0x4876,
    JA_SE_CV_AM_EYE_DAMAGE = 0x4877,
    JA_SE_CV_AM_EAT_BOMB = 0x4878,
    JA_SE_CV_AM_JITABATA = 0x4879,
    JA_SE_CM_AM_NEEDLE_OUT = 0x587D,
    JA_SE_CM_AM_NEEDLE_IN = 0x587E,
    JA_SE_CM_AM_JUMP = 0x587F,
    JA_SE_CM_AM_MOUTH_OPEN = 0x5880,
    JA_SE_CM_AM_EYE_DAMAGE = 0x5881,
    JA_SE_CM_AM_JUMP_S = 0x5882,
    JA_SE_CM_AM_MOUTH_CLOSE = 0x5883,
    JA_SE_CM_AM_EAT_BOMB = 0x5884,
    JA_SE_CM_AM_JUMP_L = 0x5885,
    JA_SE_CM_AM_BEF_EXPLODE = 0x5886,
    JA_SE_CM_AM_EXPLODE = 0x5887,
};
/* particle ids */
enum {
    ID_AK_JN_NG = 0x000C,
    ID_AK_JN_CRITICALHITFLASH = 0x0010,
    ID_IT_JN_PIYOHIT00 = 0x027B,
    ID_AK_SN_AMOTHFLASH00 = 0x8127,
    ID_AK_SN_AMOTHHAHEN00 = 0x8128,
    ID_AK_SN_AMOTHBOMBMOUTH = 0x8156,
    ID_AK_SN_AMOTHBOMBEYE = 0x8157,
    ID_AK_ST_AMOTHSMOKE00 = 0xA125,
    ID_AK_ST_AMOTHSMOKE01 = 0xA126,
    ID_AK_ST_AMOTHSMOKE02 = 0xA154,
    ID_AK_ST_AMOTHSMOKE03 = 0xA155,
};
/* AT types */
enum : u32 {
    AT_TYPE_SWORD = 0x2,
    AT_TYPE_BOOMERANG = 0x40,
    AT_TYPE_BOKO_STICK = 0x80,
    AT_TYPE_MACHETE = 0x400,
    AT_TYPE_UNK800 = 0x800,
    AT_TYPE_UNK2000 = 0x2000,
    AT_TYPE_NORMAL_ARROW = 0x4000,
    AT_TYPE_SKULL_HAMMER = 0x10000,
    AT_TYPE_FIRE_ARROW = 0x40000,
    AT_TYPE_ICE_ARROW = 0x80000,
    AT_TYPE_LIGHT_ARROW = 0x100000,
    AT_TYPE_STALFOS_MACE = 0x1000000,
    AT_TYPE_DARKNUT_SWORD = 0x4000000,
    AT_TYPE_GRAPPLING_HOOK = 0x8000000,
    AT_TYPE_MOBLIN_SPEAR = 0x10000000,
};
enum { fpcNm_Bomb2_e = 0x127 };

/* ---- this TU's statics / vtables ---- */
#define AM_VTBL 0x10006EA4
#define AM_AAB_VTBL 0x10006E04
extern const dBgS_LinChk_vt AM_LINCHK_VT;

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline BOOL enemy_ice(enemyice* ice) { return gabi::call<BOOL>(0x020402C8, ice); }
static inline void fopAcM_setGbaName(fopAc_ac_c* a, u8 itemNo, u8 n0, u8 n1) {
    gabi::call(0x025DA088, a, itemNo, n0, n1);
}
/* dStage_roomControl_c::mStayNo (s8 at 0x1047E6C8) */
/* fopAcM_monsSeStart (HD inline): 025E1AA4 mDoAud_monsSeStart(id, pos, actorId, param, reverb) */
static inline void am_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, param, reverb);
    }
}
/* attention_info: position +0x390, flags +0x39C */
static inline be<u32>& attention_flags(fopAc_ac_c* a) { return a->attention_info.flags; }
static inline cXyz* attention_pos(fopAc_ac_c* a) { return &a->attention_info.position; }
/* cCcD_ObjAt: OnAtSPrmBit(Set) / OnAtHitBit (HD: the hit word is set to 1) / OffAtSetBit */
static inline void needle_onAt(dCcD_Cyl* c) {
    c->mObjAt.mSPrm |= 1;
    c->mObjAt.mRPrm = 1;
}
static inline void needle_offAt(dCcD_Cyl* c) { c->mObjAt.mSPrm &= ~1u; }
/* mDoExt_McaMorf::isStop (HD: state byte +0xA7 bit 0, or rate +0x98 == 0) */
static inline bool morf_isStop(mDoExt_McaMorf* m) { return m->isStop(); }
// HD: the emitter has no vtable; these fields follow its shared prefix.
struct AmEmitterView : JPABaseEmitter {
    u8 _70[0x1F0 - 0x70];
    be<f32> mGlobalRotation[3][3];
    u8 _214[0x22C - 0x214];
    cXyz mGlobalTranslation;
    u8 _238[0x254 - 0x238];
    be<u32> mStatus;
};
WWHD_OFFSET(AmEmitterView, mGlobalRotation, 0x1F0);
WWHD_OFFSET(AmEmitterView, mGlobalTranslation, 0x22C);
WWHD_OFFSET(AmEmitterView, mStatus, 0x254);
inline void JPA_becomeInvalidEmitter(JPABaseEmitter* e) {
    e->setMaxFrame(-1);
    static_cast<AmEmitterView*>(e)->mStatus |= 1;
}
inline void JPA_setGlobalRTMatrix(JPABaseEmitter* e, Mtx34* m) {
    AmEmitterView* emitter = static_cast<AmEmitterView*>(e);
    gabi::call(0x028249B0, m, emitter->mGlobalRotation, &emitter->mGlobalTranslation);
}

// HD: J3DNode's accessor returns the resource joint data, not a second J3DNode.
struct AmJointData {
    be<u32> mFlags;
    be<u16> mJntNo;
};
WWHD_OFFSET(AmJointData, mJntNo, 4);
inline void am_constructSphere(dCcD_Sph* sph) { gabi::call(0x025166F0, sph); }
inline void am_destroySphere(dCcD_Sph* sph) { gabi::call(0x02515AE8, sph, 2); }

/* dComIfGp_particle_setToon (HD): dPa_control_c::set with group 2, setupInfo = roomNo */
static inline JPABaseEmitter* am_particle_setToon(u16 id, const cXyz* pos, const csXyz* angle,
                                                  u8 alpha, void* cb, s8 roomNo) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, angle, nullptr, alpha, (dPa_levelEcallBack*)cb, roomNo,
                           nullptr, nullptr, nullptr);
}
static inline JntHit_c* JntHit_create(J3DModel* model, const void* src, s16 num) {
    return gabi::call<JntHit_c*>(0x02552B60, model, src, num);
}
/* 027F3F94 (the matcher names it __nw): the model data's joint tree; joint count (u16) at +8 */
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return d->getJointNum(); }
static inline void setJointCallBack(J3DModelData* d, u16 i, u32 cb) {
    d->getJointNodePointer(i)->mCallBack = cb;
}
/* 025192A8 cc_at_check(fopAc_ac_c*, CcAtInfo*) */
static inline void cc_at_check(fopAc_ac_c* a, CcAtInfo* info) { gabi::call(0x025192A8, a, info); }
/* 025163BC dCcD_GObjInf::GetCoHitObj */
static inline void* GetCoHitObj(dCcD_GObjInf* o) { return gabi::call<void*>(0x025163BC, o); }
/* cCcD_Obj::GetAc (HD inline): the hit object's stts (+0x44) -> actor (+0xC) */
static inline fopAc_ac_c* cCcD_Obj_GetAc(void* obj) {
    dCcD_Stts* stts = static_cast<dCcD_GObjInf*>(obj)->mStts;
    return stts ? stts->mp_actor.get() : nullptr;
}
/* fpcM_GetName (HD inline, null-checked by the cast of the actor) */
static inline s16 am_GetName(void* p) { return static_cast<fopAc_ac_c*>(p)->mProcName; }
/* daBomb_c / daBomb2::Act_c */
static inline BOOL daBomb_getBombCheck_Flag(fopAc_ac_c* b) {
    return gabi::call<BOOL>(0x020CB678, b);
}
static inline s32 daBomb_getBombRestTime(fopAc_ac_c* b) { return gabi::call<s32>(0x020CB648, b); }
static inline void daBomb_setBombCheck_Flag(fopAc_ac_c* b) { gabi::call(0x020CB6A8, b); }
static inline void daBomb_change_state(fopAc_ac_c* b, s32 s) { gabi::call(0x020CB978, b, s); }
static inline void daBomb_setBombNoHit(fopAc_ac_c* b) { gabi::call(0x020CB710, b); }
static inline void daBomb_setBombNoEff(fopAc_ac_c* b) { gabi::call(0x020CB7D8, b); }
static inline void daBomb_setBombRestTime(fopAc_ac_c* b, s16 t) { gabi::call(0x020CB860, b, t); }
static inline BOOL daBomb2_chk_eat(fopAc_ac_c* b) { return gabi::call<BOOL>(0x020CBA70, b); }
static inline s32 daBomb2_get_time(fopAc_ac_c* b) { return gabi::call<s32>(0x020CBA68, b); }
static inline void daBomb2_set_eat(fopAc_ac_c* b) { gabi::call(0x020CBA78, b); }
static inline void daBomb2_set_no_hit(fopAc_ac_c* b) { gabi::call(0x020CBA84, b); }
static inline void daBomb2_remove_fuse_effect(fopAc_ac_c* b) { gabi::call(0x020CBA28, b); }
static inline void daBomb2_set_time(fopAc_ac_c* b, s32 t) { gabi::call(0x020CBA60, b, t); }
/* daPy_py_c (HD offsets): cut type u8 +0x3AC, damage wait timer s16 +0x3B0 */
enum { CUT_TYPE_HAMMER_SIDESWING = 0x11 };
// HD: this partial view uses guest scalar wrappers for the accessed player fields.
struct AmPlayerView {
    u8 _00[0x3AC];
    be<u8> mCutType;
    u8 _3AD[3];
    be<s16> mDamageWaitTimer;
};
WWHD_OFFSET(AmPlayerView, mCutType, 0x3AC);
WWHD_OFFSET(AmPlayerView, mDamageWaitTimer, 0x3B0);
static inline u8 daPy_getCutType(fopAc_ac_c* p) {
    return reinterpret_cast<AmPlayerView*>(p)->mCutType;
}
static inline s16 daPy_getDamageWaitTimer(fopAc_ac_c* p) {
    return reinterpret_cast<AmPlayerView*>(p)->mDamageWaitTimer;
}
static inline void mDoAud_onEnemyDamage() { gabi::call(0x025E1FD8); }
static inline void fopAcM_onActor(fopAc_ac_c* a) { dComIfGs_onActor(a->setID, a->home.roomNo); }
// HD: private view of the character-name table, not exposed by the shared play prefix.
struct AmCharacterTable {
    u8 _00;
    s32 GetNameIndex(const char* name, s32 n) { return gabi::call<s32>(0x0200E814, this, name, n); }
};
struct AmPlayView {
    u8 _00[0x50AC];
    AmCharacterTable mCharTbl;
};
WWHD_OFFSET(AmPlayView, mCharTbl, 0x50AC);
inline AmCharacterTable* am_charTbl() {
    return &reinterpret_cast<AmPlayView*>(dComIfGp_get())->mCharTbl;
}
/* fopAcM_SetMtx: the model's base matrix (null-preserving) */
static inline void __construct_array(void* p, u32 n, u32 size, u32 ctor) {
    gabi::call(0x028EFFD0, p, n, size, ctor);
}
static inline void __destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 f7, u32 f8) {
    gabi::call(0x028F0164, p, n, size, dtor, f7, f8);
}

/* functions of this unit called across files (guest calls by address) */
void draw_SUB(am_class* i_this);
void anm_init(am_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx);
void body_atari_check(am_class* i_this);
BOOL bomb_nomi_check(am_class* i_this);
void bomb_move_set(am_class* i_this, u8 alwaysMoveY);
void BG_check(am_class* i_this);
BOOL Line_check(am_class* i_this, cXyz* destPos);
void am_seStart(fopAc_ac_c* a, u32 id, u32 param);
void JPABaseEmitter_onStatus(JPABaseEmitter* e, u32 bit);

// HD: GHS float loads/stores retain signaling NaN payloads as word copies.
inline void am_copyFloat(be<f32>& dst, const be<f32>& src) {
    reinterpret_cast<be<u32>&>(dst) = reinterpret_cast<const be<u32>&>(src);
}

// HD: packed hit parameters retain the GameCube collision-source layout.
struct AmSphereSource {
    be<u32> mFlags;
    be<u32> mAtType;
    be<u32> mAtPower;
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    be<u32> mGObjAtInfo;
    be<u32> mGObjAtSPrm;
    be<u32> mGObjTgInfo;
    be<u32> mGObjTgSPrm;
    be<u32> mGObjCoSPrm;
    be<f32> mCenter[3];
    be<f32> mRadius;
};
struct AmCylinderSource {
    be<u32> mFlags;
    be<u32> mAtType;
    be<u32> mAtPower;
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    be<u32> mGObjAtInfo;
    be<u32> mGObjAtSPrm;
    be<u32> mGObjTgInfo;
    be<u32> mGObjTgSPrm;
    be<u32> mGObjCoSPrm;
    be<f32> mCenter[3];
    be<f32> mRadius;
    be<f32> mHeight;
};
struct AmJointHitSource {
    be<s16> mShapeType;
    be<s16> mJointIndex;
    be<f32> mRadius;
    gptr<const f32> mpOffsets;
};

enum { DSNAP_TYPE_AM = 0xB7, dItemNo_BOW_e = 0x27 };
void* am_constructSmoke(void*, u8);
WWHD_SIZE(AmSphereSource, 0x40);
WWHD_SIZE(AmCylinderSource, 0x44);
WWHD_SIZE(AmJointHitSource, 0xC);

WWHD_SIZE(AmEmitterView, 0x258);
WWHD_OFFSET(am_class, mEyeJntHit, 0x3D0);
WWHD_OFFSET(am_class, mEyeballPos, 0x410);

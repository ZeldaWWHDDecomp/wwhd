/* oq_class (Octorok), WWHD layout. 
 *
 * The GameCube decompilation has no layout for this actor (an empty class and "Nonmatching"
 * stubs): the members below are named by their WWHD offset (mXXX) or by their use, from the
 * WWHD code. enemyice at 0xBA4 and enemyfire at 0xF5C as in d_a_pt/d_a_ph.
 * Size 0x1194 (constructor 023C3D0C). */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/actor/d_a_player.h"

#ifndef D_A_DAMAGEREACTION_L
#define D_A_DAMAGEREACTION_L
/* c_damagereaction (SHARED-CANDIDATE: same as in d_a_pt.h) */
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
WWHD_OFFSET(enemyice, mCylHeight, 0x19C);
WWHD_SIZE(enemyice, 0x3B8);

/* enemyfire (c_damagereaction), HD 0x22C */
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
    /* 0x228 */ be<f32> m228;    /* HD */
};
WWHD_OFFSET(enemyfire, mSph, 0xDC);
WWHD_SIZE(enemyfire, 0x22C);

#endif

/* mDoExt_baseAnm (HD): J3DFrameCtrl first (frame at +4) */
struct mDoExt_baseAnm_oq {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
};
/* dPa_rippleEcallBack (HD 0x14): vtable, emitter, ..., rate at +0x10 */
struct dPa_rippleEcallBack_oq {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<f32> mRate;
};

struct oq_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D4 */ u8 _3D4[4];
    /* 0x3D8 */ gptr<mDoExt_baseAnm_oq> mpBrk;
    /* 0x3DC */ be<u8> mType;          /* param byte 0: 0, 1, 4, 5 octorok kinds, 2/3 spawners, 6 the rock (tama) */
    /* 0x3DD */ be<u8> m3DD;
    /* 0x3DE */ be<u8> mProjectileType;           /* param byte 1 */
    /* 0x3DF */ be<u8> mAction;
    /* 0x3E0 */ be<u8> mMode;
    /* 0x3E1 */ be<u8> mDamageType;
    /* 0x3E2 */ be<u8> m3E2;
    /* 0x3E3 */ be<u8> mSplashDone;           /* splash done */
    /* 0x3E4 */ be<u8> mWaveFull;           /* set by the spawner on its first child */
    /* 0x3E5 */ be<u8> mDeleted;           /* deleted this frame */
    /* 0x3E6 */ be<s16> mTimer[6];     /* counted down by Execute */
    /* 0x3F2 */ be<s16> mActionFlags[6];       /* [0]: rock bounced / jumping; [5] (0x3FC): player-near latch */
    /* 0x3FE */ be<s16> mTargetAngleY;          /* target angle */
    /* 0x400 */ be<s16> mBobPhase;          /* bobbing phase */
    /* 0x402 */ be<s16> mWaveCount;          /* spawner: children left in this wave */
    /* 0x404 */ be<s16> mKillCount;          /* spawner: octoroks killed */
    /* 0x406 */ u8 _406[2];
    /* 0x408 */ be<u32> mChildId[6];   /* spawner: children */
    /* 0x420 */ be<s32> mAnmIdx;
    /* 0x424 */ u8 _424[4];
    /* 0x428 */ be<f32> mCollisionRadius;
    /* 0x42C */ be<f32> mCollisionHeightOffset;          /* collision height offset */
    /* 0x430 */ be<f32> mRange;          /* range (param byte 2 * 1000) */
    /* 0x434 */ be<f32> mBrkFrame;          /* brk frame */
    /* 0x438 */ be<f32> mSplashScale;          /* splash scale */
    /* 0x43C */ be<f32> mDrawOffsetY;          /* draw height offset */
    /* 0x440 */ be<u32> mParentId;          /* parent process id */
    /* 0x444 */ cXyz mMouthPos;             /* mouth (joint 8, 20 forward) */
    /* 0x450 */ cXyz mMouthEffectPos;             /* mouth (joint 8, 40 forward) */
    /* 0x45C */ cXyz mHomePos;
    /* 0x468 */ csXyz mSpin;            /* rock spin / turn speed (y: m46A) */
    /* 0x46E */ u8 _46E[2];
    /* 0x470 */ dPa_rippleEcallBack_oq mRipple;
    /* 0x484 */ dPa_followEcallBack mFollow;
    /* 0x498 */ cXyz mWaterPos;             /* water surface position */
    /* 0x4A4 */ u8 _4A4[8];
    /* 0x4AC */ dBgS_AcchCir mAcchCir;
    /* 0x4EC */ dBgS_ObjAcch mAcch;
    /* 0x6B0 */ dCcD_Stts mStts;
    /* 0x6EC */ dCcD_Cyl mBodyCoCyl;
    /* 0x81C */ dCcD_Cyl mBodyAtCyl;
    /* 0x94C */ dCcD_Sph mTamaAtSph;
    /* 0xA78 */ dCcD_Sph mTamaTgSph;
    /* 0xBA4 */ enemyice mEnemyIce;
    /* 0xF5C */ enemyfire mEnemyFire;
    /* 0x1188 */ u8 mInvisibleModel[8]; /* mDoExt_invisibleModel */
    /* 0x1190 */ be<u32> mpJntHit;
};
WWHD_OFFSET(oq_class, mpMorf, 0x3D0);
WWHD_OFFSET(oq_class, mAnmIdx, 0x420);
WWHD_OFFSET(oq_class, mRipple, 0x470);
WWHD_OFFSET(oq_class, mAcch, 0x4EC);
WWHD_OFFSET(oq_class, mStts, 0x6B0);
WWHD_OFFSET(oq_class, mTamaTgSph, 0xA78);
WWHD_OFFSET(oq_class, mEnemyIce, 0xBA4);
WWHD_OFFSET(oq_class, mEnemyFire, 0xF5C);
WWHD_SIZE(oq_class, 0x1194);

#define SAFESTRING_VTBL_OQ 0x10033D58 /* this TU's sead::SafeString vtable */
#define OQ_VTBL 0x10033E10
static const dBgS_ObjAcch_vt OQ_OBJACCH_VT = {0x10033DA0, 0x10033DC0, 0x10033DB0};
#define OQ_AAB_VTBL 0x10033D70        /* this TU's cM3dGAab vtable */
enum { DSNAP_TYPE_OQ = 0xBA };
static inline f32 oq_wtr_height(oq_class* i_this) { return gabi::load<f32>(gabi::ea(&i_this->mAcch) + 0x174 + 0x48); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline BOOL enemy_ice(enemyice* ice) { return gabi::call<BOOL>(0x020402C8, ice); }
static inline void enemy_fire(enemyfire* fire) { gabi::call(0x02041570, fire); }
static inline void enemy_fire_remove(enemyfire* fire) { gabi::call(0x02041C30, fire); }
/* 0259138C mDoExt_invisibleModel::entryDL (the matcher names it dMat_ice_c::entryDL) */
static inline void mDoExt_invisibleModel_entryDL(mDoExt_McaMorf* morf, s32 p, void* inv) { gabi::call(0x0259138C, morf, p, inv); }
static inline u32 JntHit_create(J3DModel* model, u32 src, s16 num) { return gabi::call<u32>(0x02552B60, model, src, num); }
/* mDoExt_brkAnm (HD 0x78): constructor 025E80D0 (the matcher calls it init), init 025E8154 */
static inline void* mDoExt_brkAnm_ct(void* p) { return gabi::call<void*>(0x025E80D0, p); }
static inline BOOL mDoExt_brkAnm_init(void* a, J3DModelData* d, void* brk, s32 anmPlay, s32 mode, f32 rate, s16 start, s16 end,
                                      bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, brk, anmPlay, mode, rate, start, end, modify, entry);
}
/* 025192A8 cc_at_check(fopAc_ac_c*, CcAtInfo*) (SHARED-CANDIDATE, as in d_a_pt/d_a_ph/d_a_kb) */
struct CcAtInfo_oq {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ be<u32> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
static inline void cc_at_check(fopAc_ac_c* a, CcAtInfo_oq* info) { gabi::call(0x025192A8, a, info); }
static inline fopAc_ac_c* dCcD_GAtTgCoCommonBase_GetAc(void* p) { return gabi::call<fopAc_ac_c*>(0x02515BBC, p); }
/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb) */
static inline void mDoAud_monsSeStart(u32 id, cXyz* pos, u32 pid, u32 param, s32 reverb) {
    gabi::call(0x025E1AA4, id, pos, pid, param, reverb);
}
/* fopAcM_seStart(actor, id, param) / fopAcM_monsSeStart: the HD inlines test &eyePos */
static inline void oq_se(fopAc_ac_c* a, u32 id, u32 param) {
    cXyz* eye = &a->eyePos;
    if (gabi::ea(eye) != 0) mDoAud_seStart(id, eye, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline void oq_mons_se(fopAc_ac_c* a, u32 id) {
    cXyz* eye = &a->eyePos;
    if (gabi::ea(eye) != 0) {
        u32 pid = a != nullptr ? gabi::load<u32>(gabi::ea(a) + 4) : 0xFFFFFFFFu;
        mDoAud_monsSeStart(id, eye, pid, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
    }
}
static inline u32 fopAcM_createChild(s16 name, u32 parent, u32 param, cXyz* pos, s32 roomNo, csXyz* angle, cXyz* scale, s8 subtype,
                                     u32 createFunc) {
    return gabi::call<u32>(0x025D5A20, name, parent, param, pos, roomNo, angle, scale, subtype, createFunc);
}
enum { PROC_OQ = 0xE3 };
static inline be<u32>& attn_flags_oq(fopAc_ac_c* actor) { return actor->attention_info.flags; }
/* 0200E814 cDT_NamePTbl::GetIndex(name, 0) on play+0x50AC (dComIfGp_CharTbl) */
static inline s32 dComIfGp_CharTbl_GetIndex_oq(const char* name, s32 p) {
    return gabi::call<s32>(0x0200E814, gabi::at<u8>(dComIfGp_ea() + PLAY_NAMETBL), name, p);
}
/* the spawn delay of the next type-4 octorok (file static s16 at 0x101CE2B4) */
extern be<s16>& oqSpawnDelay;
/* sead::SafeString equality (HD inline of strcmp(dComIfGp_getStartStageName(), lit) == 0) */
static inline bool oq_isStartStage(u32 lit) {
    gabi::Local<SafeString> a;
    a->mStringTop = lit;
    a->__vtbl = SAFESTRING_VTBL_OQ;
    gabi::Local<SafeString> b;
    b->mStringTop = dComIfGp_ea() + 0x5134;
    b->__vtbl = SAFESTRING_VTBL_OQ;
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
    if (s1 == b->mStringTop) return true;
    u32 p = a->mStringTop, q = b->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = gabi::load<u8>(p + i);
        if (c != gabi::load<u8>(q + i)) return false;
        if (c == 0) return true;
    }
    return false;
}
static inline void fopAcM_setGbaName(fopAc_ac_c* a, u8 itemNo, u8 n0, u8 n1) { gabi::call(0x025DA088, a, itemNo, n0, n1); }
static inline void csXyz_add(csXyz* a, csXyz* b) { gabi::call(0x0201A554, a, b); } /* csXyz::operator+= */
/* 020CB8D8 daBomb_c::prm_make(State_e, bool, bool): static. HARNESS WORKAROUND: the harness takes the
 * GameCube signature as a member function (this in r3) and compares r6, which the original leaves
 * at 0x10030000 (the .rodata base of the constant load before the call): pass the same word. */
static inline u32 daBomb_prm_make(s32 state, bool a, bool b) { return gabi::call<u32>(0x020CB8D8, state, a, b, 0x10030000u); }
static inline fopAc_ac_c* fopAcM_fastCreate(s16 name, u32 param, cXyz* pos, s32 roomNo, csXyz* angle, cXyz* scale, s8 subtype,
                                            u32 createFunc, void* data) {
    return gabi::call<fopAc_ac_c*>(0x025D5928, name, param, pos, roomNo, angle, scale, subtype, createFunc, data);
}
static inline void JPASetRMtxTVecfromMtx(Mtx34* m, u32 r, u32 t) { gabi::call(0x028249B0, m, r, t); }

/* this TU's dBgS_LinChk vtables (constructor and destructor) */
static const dBgS_LinChk_vt OQ_LINCHK_VT = {0x10033DD0, 0x10033DE0, 0x10033E00, 0x10033DF0};
static inline void dBgS_LinChk_dt_oq(void* c) {
    u32 b = gabi::ea(c);
    gabi::store<u32>(b + 0x58, 0x10033E00);
    gabi::store<u32>(b + 0x64, 0x10033D90);
    gabi::store<u32>(b + 0x20, 0x10033D80);
    cBgS_LinChk_dt(c, 0);
}

void oqSphereCtor(dCcD_Sph* sphere);
void oqRippleCtor(dPa_rippleEcallBack_oq* ripple);
void oqInvisibleCtor(void* model);
void oqAngleInit(u32 address);
void oqFlagInit(u32 address);
void oqInvisibleDtor(void* model, s32 flags);
void oqSphereDtor(dCcD_Sph* sphere, s32 flags);
void oqAcchDtor(u32 acch, s32 flags);
void oqCircleDtor(u32 circle, s32 flags);
void oqActorDtor(oq_class* actor, s32 flags);
void oqExecAnimation(oq_class* actor, s32 bck, f32 morph, u8 loop, f32 speed, s32 bas);
BOOL oqExecDamage(oq_class* actor);
void oqExecSplash(oq_class* actor, cXyz* position, f32 scale);
BOOL oqExecWater(oq_class* actor);
void oqExecTurn(oq_class* actor, s16 step);
void oqExecMatrix(oq_class* actor);
void oqExecBackground(oq_class* actor);
void oqExecSpawner(oq_class* actor);

struct OqAttackView { u8 _00[0x10]; be<u32> mType; };
WWHD_OFFSET(OqAttackView, mType, 0x10);
WWHD_SIZE(OqAttackView, 0x14);
struct OqPositionWords { be<u32> x, y, z; };
WWHD_SIZE(OqPositionWords, 0xC);
inline OqPositionWords& oqPositionWords(cXyz& pos) { return reinterpret_cast<OqPositionWords&>(pos); }
extern csXyz (&oqSpins)[4];
enum { OQ_SHIP_RIDE = 0x10000 };

// HD: use the checked guest read when a clobbered player pointer is outside the mapped image.
inline u8 oqPlayerCutType(fopAc_ac_c* player) { return gabi::load<u8>(gabi::ea(player) + offsetof(daPy_py_c, mCutType)); }

// HD: identical assertion text has distinct addresses at each inlined call site.
enum : u32 {
    OQ_SURFACE_ASSERT_FILE = 0x10033E3C,
    OQ_SURFACE_ASSERT_CONDITION = 0x10033D56,
    OQ_SWIM_ASSERT_FILE = 0x10033E30,
    OQ_SWIM_ASSERT_CONDITION = 0x10033D54,
    OQ_BOMB_ASSERT_FILE = 0x10033E48,
    OQ_BOMB_ASSERT_CONDITION = 0x10033E20,
};
inline const char* oqAssertText(u32 address) { return gabi::at<const char>(address); }

enum : u32 { OQ_JOINT_HIT_SOURCE = 0x101CE2D8, OQ_BOSS_STAGE_NAME = 0x10033F50 };
struct OqJointView { u8 _00[4]; be<u16> mJntNo; };
WWHD_OFFSET(OqJointView, mJntNo, 4);
WWHD_SIZE(OqJointView, 6);
struct OqModelDataView { u8 _00[0x48]; be<u32> field_0x48; };
WWHD_OFFSET(OqModelDataView, field_0x48, 0x48);
WWHD_SIZE(OqModelDataView, 0x4C);
inline OqModelDataView& oqModelData(J3DModelData* data) { return *reinterpret_cast<OqModelDataView*>(data); }

enum dRes_INDEX_OQ {
    /* BCK  */
    dRes_INDEX_OQ_BCK_AATTACK1_e=0x6,
    dRes_INDEX_OQ_BCK_AATTACK2_FUKU_e=0x7,
    dRes_INDEX_OQ_BCK_AATTACK3_e=0x8,
    dRes_INDEX_OQ_BCK_DEAD1_e=0x9,
    dRes_INDEX_OQ_BCK_DEAD2_e=0xA,
    dRes_INDEX_OQ_BCK_DEAD3_e=0xB,
    dRes_INDEX_OQ_BCK_IKAKU_e=0xC,
    dRes_INDEX_OQ_BCK_JET_ATTACK_e=0xD,
    dRes_INDEX_OQ_BCK_JET_LOOP_e=0xE,
    dRes_INDEX_OQ_BCK_JUMP_e=0xF,
    dRes_INDEX_OQ_BCK_MOGURU_e=0x10,
    dRes_INDEX_OQ_BCK_NOM_WAIT_e=0x11,
    dRes_INDEX_OQ_BCK_SHIP_HIT_e=0x12,
    dRes_INDEX_OQ_BCK_START_e=0x13,
    dRes_INDEX_OQ_BCK_UMI_NEW_WAIT_e=0x14,
    /* BDLM */
    dRes_INDEX_OQ_BDL_ISI_e=0x17,
    /* BMDM */
    dRes_INDEX_OQ_BMD_OQ_e=0x1A,
    dRes_INDEX_OQ_BMD_RED_OQ_e=0x1B,
    /* BRK  */
    dRes_INDEX_OQ_BRK_RED_OQ_e=0x1E,
};
enum OQ_JNT {
    OQ_JNT_KOSI_e=0x0,
    OQ_JNT_ASI1_e=0x1,
    OQ_JNT_ASI2_e=0x2,
    OQ_JNT_FACE1_e=0x3,
    OQ_JNT_FACE2_e=0x4,
    OQ_JNT_HEAD_e=0x5,
    OQ_JNT_HEAD2_e=0x6,
    OQ_JNT_KUTI_e=0x7,
    OQ_JNT_KUTI2_e=0x8,
    OQ_JNT_JET_e=0x9,
    OQ_JNT_KATAL_e=0xA,
    OQ_JNT_UDEL_e=0xB,
    OQ_JNT_UDEL2_e=0xC,
    OQ_JNT_UDEL3_e=0xD,
    OQ_JNT_HANDL_e=0xE,
    OQ_JNT_KATAR_e=0xF,
    OQ_JNT_UDER_e=0x10,
    OQ_JNT_UDER2_e=0x11,
    OQ_JNT_UDER3_e=0x12,
    OQ_JNT_HANDR_e=0x13,
};

// HD: the inlined Acch destructor restores both pass-check vtables before its call.
inline void oqPrepareAcchDtor(dBgS_ObjAcch& acch) {
    acch.dBgS_GrpPassChk::__vtbl = OQ_OBJACCH_VT.v20;
    acch.dBgS_PolyPassChk::__vtbl = OQ_OBJACCH_VT.v14;
}

WWHD_OFFSET(oq_class, mPhase, 0x3C8);
WWHD_OFFSET(oq_class, mpBrk, 0x3D8);
WWHD_OFFSET(oq_class, mType, 0x3DC);
WWHD_OFFSET(oq_class, m3DD, 0x3DD);
WWHD_OFFSET(oq_class, mProjectileType, 0x3DE);
WWHD_OFFSET(oq_class, mAction, 0x3DF);
WWHD_OFFSET(oq_class, mMode, 0x3E0);
WWHD_OFFSET(oq_class, mDamageType, 0x3E1);
WWHD_OFFSET(oq_class, m3E2, 0x3E2);
WWHD_OFFSET(oq_class, mSplashDone, 0x3E3);
WWHD_OFFSET(oq_class, mWaveFull, 0x3E4);
WWHD_OFFSET(oq_class, mDeleted, 0x3E5);
WWHD_OFFSET(oq_class, mTimer, 0x3E6);
WWHD_OFFSET(oq_class, mActionFlags, 0x3F2);
WWHD_OFFSET(oq_class, mTargetAngleY, 0x3FE);
WWHD_OFFSET(oq_class, mBobPhase, 0x400);
WWHD_OFFSET(oq_class, mWaveCount, 0x402);
WWHD_OFFSET(oq_class, mKillCount, 0x404);
WWHD_OFFSET(oq_class, mChildId, 0x408);
WWHD_OFFSET(oq_class, mCollisionRadius, 0x428);
WWHD_OFFSET(oq_class, mCollisionHeightOffset, 0x42C);
WWHD_OFFSET(oq_class, mRange, 0x430);
WWHD_OFFSET(oq_class, mBrkFrame, 0x434);
WWHD_OFFSET(oq_class, mSplashScale, 0x438);
WWHD_OFFSET(oq_class, mDrawOffsetY, 0x43C);
WWHD_OFFSET(oq_class, mParentId, 0x440);
WWHD_OFFSET(oq_class, mMouthPos, 0x444);
WWHD_OFFSET(oq_class, mMouthEffectPos, 0x450);
WWHD_OFFSET(oq_class, mHomePos, 0x45C);
WWHD_OFFSET(oq_class, mSpin, 0x468);
WWHD_OFFSET(oq_class, mFollow, 0x484);
WWHD_OFFSET(oq_class, mWaterPos, 0x498);
WWHD_OFFSET(oq_class, mAcchCir, 0x4AC);
WWHD_OFFSET(oq_class, mBodyCoCyl, 0x6EC);
WWHD_OFFSET(oq_class, mBodyAtCyl, 0x81C);
WWHD_OFFSET(oq_class, mTamaAtSph, 0x94C);
WWHD_OFFSET(oq_class, mInvisibleModel, 0x1188);
WWHD_OFFSET(oq_class, mpJntHit, 0x1190);
WWHD_OFFSET(enemyice, mpActor, 0x000);
WWHD_OFFSET(enemyice, mFreezeDuration, 0x004);
WWHD_OFFSET(enemyice, mLightShrinkTimer, 0x006);
WWHD_OFFSET(enemyice, mYOffset, 0x008);
WWHD_OFFSET(enemyice, m00C, 0x00C);
WWHD_OFFSET(enemyice, mMode, 0x00D);
WWHD_OFFSET(enemyice, mFreezeTimer, 0x00E);
WWHD_OFFSET(enemyice, mMoveDelayTimer, 0x010);
WWHD_OFFSET(enemyice, mAngleY, 0x012);
WWHD_OFFSET(enemyice, mAngularVelY, 0x014);
WWHD_OFFSET(enemyice, mSpeed, 0x018);
WWHD_OFFSET(enemyice, mSpeedF, 0x024);
WWHD_OFFSET(enemyice, m028, 0x028);
WWHD_OFFSET(enemyice, m02C, 0x02C);
WWHD_OFFSET(enemyice, mStts, 0x030);
WWHD_OFFSET(enemyice, mCyl, 0x06C);
WWHD_OFFSET(enemyice, mWallRadius, 0x1A0);
WWHD_OFFSET(enemyice, mScaleXZ, 0x1A4);
WWHD_OFFSET(enemyice, mScaleY, 0x1A8);
WWHD_OFFSET(enemyice, mParticleScale, 0x1AC);
WWHD_OFFSET(enemyice, m1B0, 0x1B0);
WWHD_OFFSET(enemyice, mDeathSwitch, 0x1B1);
WWHD_OFFSET(enemyice, mBgAcchCir, 0x1B4);
WWHD_OFFSET(enemyice, mBgAcch, 0x1F4);
WWHD_OFFSET(enemyfire, mpActor, 0x000);
WWHD_OFFSET(enemyfire, mFireDuration, 0x004);
WWHD_OFFSET(enemyfire, mMode, 0x006);
WWHD_OFFSET(enemyfire, mFireTimer, 0x008);
WWHD_OFFSET(enemyfire, mpMcaMorf, 0x00C);
WWHD_OFFSET(enemyfire, mFlameJntIdxs, 0x010);
WWHD_OFFSET(enemyfire, mParticleScale, 0x01C);
WWHD_OFFSET(enemyfire, mFlameTimers, 0x044);
WWHD_OFFSET(enemyfire, mpFlameEmitters, 0x058);
WWHD_OFFSET(enemyfire, mPrevPos, 0x080);
WWHD_OFFSET(enemyfire, mDirection, 0x08C);
WWHD_OFFSET(enemyfire, mFlameScaleY, 0x098);
WWHD_OFFSET(enemyfire, mHitboxFlameIdx, 0x09D);
WWHD_OFFSET(enemyfire, mStts, 0x0A0);
WWHD_OFFSET(enemyfire, mSph, 0x0DC);
WWHD_OFFSET(enemyfire, mLight, 0x208);
WWHD_OFFSET(enemyfire, m228, 0x228);
WWHD_OFFSET(mDoExt_baseAnm_oq, mFrameCtrl, 0x00);
WWHD_OFFSET(dPa_rippleEcallBack_oq, mpEmitter, 0x04);
WWHD_OFFSET(dPa_rippleEcallBack_oq, mRate, 0x10);
WWHD_OFFSET(CcAtInfo_oq, mpObj, 0x00);
WWHD_OFFSET(CcAtInfo_oq, mpActor, 0x04);
WWHD_OFFSET(CcAtInfo_oq, mDamage, 0x08);
WWHD_OFFSET(CcAtInfo_oq, mbDead, 0x09);
WWHD_OFFSET(CcAtInfo_oq, mResultingAttackType, 0x0A);
WWHD_OFFSET(CcAtInfo_oq, m0C, 0x0C);
WWHD_OFFSET(CcAtInfo_oq, mPlCutBit, 0x12);
WWHD_OFFSET(CcAtInfo_oq, pParticlePos, 0x14);
WWHD_OFFSET(CcAtInfo_oq, mHitSoundId, 0x18);
WWHD_SIZE(mDoExt_baseAnm_oq, 0x10);
WWHD_SIZE(dPa_rippleEcallBack_oq, 0x14);
WWHD_SIZE(CcAtInfo_oq, 0x1C);

enum OqAction {
    OQ_ACTION_DOUSA = 0xA,
    OQ_ACTION_KOUGEKI = 0xB,
    OQ_ACTION_ITAI = 0x14,
    OQ_ACTION_TAMA_SHOOT = 0x1E,
    OQ_ACTION_WAKIDASI = 0x64,
};
enum OqType {
    OQ_SMALL = 0,
    OQ_LARGE = 1,
    OQ_PLACE = 2,
    OQ_SPAWNER = 3,
    OQ_DELAYED = 4,
    OQ_CHILD = 5,
    OQ_ROCK = 6,
};
// HD: JPA writes rotation at +0x1F0 and translation at +0x22C.
inline void oqFollowEmitterMatrix(Mtx34* matrix, JPABaseEmitter* emitter) {
    JPASetRMtxTVecfromMtx(matrix, gabi::ea(emitter) + 0x1F0, gabi::ea(emitter) + 0x22C);
}

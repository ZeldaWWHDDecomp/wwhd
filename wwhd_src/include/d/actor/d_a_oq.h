/* oq_class (Octorok), WWHD layout. 
 *
 * The GameCube decompilation has no layout for this actor (an empty class and "Nonmatching"
 * stubs): the members below are named by their WWHD offset (mXXX) or by their use, from the
 * WWHD code. enemyice at 0xBA4 and enemyfire at 0xF5C as in d_a_pt/d_a_ph.
 * Size 0x1194 (constructor 023C3D0C). */
#pragma once
#include "bindings.h"

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
    /* 0x3DE */ be<u8> m3DE;           /* param byte 1 */
    /* 0x3DF */ be<u8> m3DF;
    /* 0x3E0 */ be<u8> m3E0;
    /* 0x3E1 */ be<u8> m3E1;
    /* 0x3E2 */ be<u8> m3E2;
    /* 0x3E3 */ be<u8> m3E3;           /* splash done */
    /* 0x3E4 */ be<u8> m3E4;           /* set by the spawner on its first child */
    /* 0x3E5 */ be<u8> m3E5;           /* deleted this frame */
    /* 0x3E6 */ be<s16> mTimer[6];     /* counted down by Execute */
    /* 0x3F2 */ be<s16> m3F2[6];       /* [0]: rock bounced / jumping; [5] (0x3FC): player-near latch */
    /* 0x3FE */ be<s16> m3FE;          /* target angle */
    /* 0x400 */ be<s16> m400;          /* bobbing phase */
    /* 0x402 */ be<s16> m402;          /* spawner: children left in this wave */
    /* 0x404 */ be<s16> m404;          /* spawner: octoroks killed */
    /* 0x406 */ u8 _406[2];
    /* 0x408 */ be<u32> mChildId[6];   /* spawner: children */
    /* 0x420 */ be<s32> mAnmIdx;
    /* 0x424 */ u8 _424[4];
    /* 0x428 */ be<f32> m428;
    /* 0x42C */ be<f32> m42C;          /* collision height offset */
    /* 0x430 */ be<f32> m430;          /* range (param byte 2 * 1000) */
    /* 0x434 */ be<f32> m434;          /* brk frame */
    /* 0x438 */ be<f32> m438;          /* splash scale */
    /* 0x43C */ be<f32> m43C;          /* draw height offset */
    /* 0x440 */ be<u32> m440;          /* parent process id */
    /* 0x444 */ cXyz m444;             /* mouth (joint 8, 20 forward) */
    /* 0x450 */ cXyz m450;             /* mouth (joint 8, 40 forward) */
    /* 0x45C */ cXyz m45C;
    /* 0x468 */ csXyz m468;            /* rock spin / turn speed (y: m46A) */
    /* 0x46E */ u8 _46E[2];
    /* 0x470 */ dPa_rippleEcallBack_oq mRipple;
    /* 0x484 */ dPa_followEcallBack mFollow;
    /* 0x498 */ cXyz m498;             /* water surface position */
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
/* J3D (HD): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, joint block at model+0x2C */
struct J3DMtxBlock_oq {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_oq {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_oq> mpMtxBlock;
    /* 0x30 */ u8 _30[0xAC - 0x30];
    /* 0xAC */ gptr<J3DModelData> mModelData;
    /* 0xB0 */ u8 _B0[8];
    /* 0xB8 */ be<u32> mUserArea;
};
static inline Mtx34* getAnmMtx(J3DModel* m, s32 jnt) {
    J3DMtxBlock_oq* blk = ((J3DModel_oq*)m)->mpMtxBlock;
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30);
}
static inline J3DModel_oq* j3dSys_getModel() { return gabi::at<J3DModel_oq>(gabi::load<u32>(0x104B462C)); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
/* 0259138C mDoExt_invisibleModel::entryDL (the matcher names it dMat_ice_c::entryDL) */
static inline void mDoExt_invisibleModel_entryDL(mDoExt_McaMorf* morf, s32 p, void* inv) { gabi::call(0x0259138C, morf, p, inv); }
static inline BOOL mDoExt_invisibleModel_create(void* inv, J3DModel* model) { return gabi::call<BOOL>(0x025E8A48, inv, model); }
static inline u32 JntHit_create(J3DModel* model, u32 src, s16 num) { return gabi::call<u32>(0x02552B60, model, src, num); }
/* 027F3F94 (the matcher names it __nw): the model data's joint tree; joint count (u16) at +8 */
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::ea(gabi::call<void*>(0x027F3F94, d)) + 8); }
static inline void setJointCallBack(J3DModelData* d, u16 i, u32 cb) {
    u32 data = gabi::ea(d);
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
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
static inline BOOL dComIfGs_isStageBossEnemy(s32 no) { return gabi::call<BOOL>(0x02520A84, no); }
enum { PROC_OQ = 0xE3 };
static inline be<u32>& attn_flags_oq(fopAc_ac_c* a) { return *gabi::at<be<u32>>(gabi::ea(a) + 0x39C); }
/* 0200E814 cDT_NamePTbl::GetIndex(name, 0) on play+0x50AC (dComIfGp_CharTbl) */
static inline s32 dComIfGp_CharTbl_GetIndex_oq(const char* name, s32 p) {
    return gabi::call<s32>(0x0200E814, gabi::at<u8>(dComIfGp_ea() + PLAY_NAMETBL), name, p);
}
/* the spawn delay of the next type-4 octorok (file static s16 at 0x101CE2B4) */
static inline be<s16>& oq_spawn_delay() { return *gabi::at<be<s16>>(0x101CE2B4); }
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
static inline dSv_event_c* dComIfGs_getEvent_oq() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
/* this TU's dBgS_LinChk vtables (constructor and destructor) */
static const dBgS_LinChk_vt OQ_LINCHK_VT = {0x10033DD0, 0x10033DE0, 0x10033E00, 0x10033DF0};
static inline void dBgS_LinChk_dt_oq(void* c) {
    u32 b = gabi::ea(c);
    gabi::store<u32>(b + 0x58, 0x10033E00);
    gabi::store<u32>(b + 0x64, 0x10033D90);
    gabi::store<u32>(b + 0x20, 0x10033D80);
    cBgS_LinChk_dt(c, 0);
}

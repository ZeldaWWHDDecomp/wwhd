/* bwds_class (Molgera larva), WWHD layout. 
 *
 * GameCube -> WWHD: fopEn_enemy_c is +0x11C; the members up to mKnockbackDist (GC 0x320) stay +0x11C.
 * HD dropped GC m0326 (the knockback pitch): mKnockbackAngleY (GC 0x324) is the knockback yaw,
 * mHitShakeTimer (GC 0x328) follows it, so the members from there on are +0x11A and from mBodyPos
 * (GC 0x32C) on +0x118. Size 0x19E4 (constructor 02108B2C).
 * Offsets from the verified functions. */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"

/* dPa_smokeEcallBack (HD 0x20): vtable at +0, colour (GXColor) at +0x16 */
struct dPa_smokeEcallBack_bw {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 _04[0x16 - 0x04];
    /* 0x16 */ be<u8> mColor[4];
    /* 0x1A */ u8 _1A[0x20 - 0x1A];
};

struct bwds_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhs;  /* was m02AC: daBwds_Create 02108BF0: dComIfG_resLoad of "Bwds" into it; daBwds_Delete 02108750 deletes the resource with it */
    /* 0x3D0 */ be<u8> mType;  /* was m02B4: daBwds_Create 02108BF0: parameter byte 0; 0x23 starts in mode -10 (jumps out of the sand at once, at a random angle) */
    /* 0x3D1 */ u8 _3D1[3];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ gptr<J3DModel> mpBodyModels[0xE];  /* was mp02BC: useHeapInit 021087FC creates the 13 body segment models (array of 14); body_control (in daBwds_Execute 0210602C) places segment i - 1 at mBodyPos[i]; daBwds_Draw 02105F00 / daBwds_Execute 0210602C use the 13 - mBurstCount still alive */
    /* 0x410 */ be<s16> mFrameCounter;  /* was m02F4: daBwds_Execute 0210602C adds 1 per frame; ug_move (in daBwds_Execute 0210602C) and body_control use it as the phase of the weave and segment pulse sines, daBwds_Execute 0210602C as the phase of the hit shake */
    /* 0x412 */ be<s16> mAction;  /* was m02F6: move (in daBwds_Execute 0210602C) switches on it (ACTION_UG_MOVE / HOOK_ON / HOOK_CHANCE / FAIL); damage_check / hook_on / hook_chance set it; action */
    /* 0x414 */ be<s16> mMode;  /* was m02F8: ug_move / hook_on / hook_chance / fail (in daBwds_Execute 0210602C) step through it; daBwds_Create 02108BF0 sets -10 for mType 0x23; mode */
    /* 0x416 */ be<s8> mInSand;  /* was m02FA: move (in daBwds_Execute 0210602C): 1 while current.pos.y is at most mSandY + 20; daBwds_Execute 0210602C then raises eyePos by 90 and moves the head sphere up with a smaller radius */
    /* 0x417 */ be<s8> mVulnerable;  /* was m02FB: ug_move (jump) / hook_chance (in daBwds_Execute 0210602C) set 1 each frame; damage_check takes a head hit only while it is 1; move clears it every frame */
    /* 0x418 */ cXyz mTargetPos;  /* was m02FC: ug_move (in daBwds_Execute 0210602C): the player's position, a random point near the actor or a point 2000 ahead (set_jump_target); pos_move turns current.angle.y toward it */
    /* 0x424 */ u8 m0308[4];
    /* 0x428 */ be<f32> mTurnSpeed;  /* was m030C: ug_move (in daBwds_Execute 0210602C) sets 3000 / 1000 (+ reg); pos_move: angle step = it * mTurnSpeedRate */
    /* 0x42C */ be<f32> mTurnSpeedRate;  /* was m0310: pos_move (in daBwds_Execute 0210602C) eases it to 1 and multiplies mTurnSpeed by it */
    /* 0x430 */ be<s16> mTimers[5];  /* was m0314: daBwds_Execute 0210602C counts all 5 down each frame; ug_move / hook_chance / fail (in daBwds_Execute 0210602C) load them ([0] = 0x23, l_HIO.mHookChanceTime; [1] / [2] random waits) */
    /* 0x43A */ be<s16> mNoDamageTimer;  /* was m031E: damage_check (in daBwds_Execute 0210602C) runs only while it is 0 and sets 10 / 6 after a hit; hook_on / fail keep it at 3; daBwds_Execute 0210602C counts it down */
    /* 0x43C */ be<f32> mKnockbackDist;  /* was m0320: damage_check (in daBwds_Execute 0210602C) sets 40 (+ reg) on a hit; move moves the actor back by it along mKnockbackAngleY and eases it to 0; knockback speed */
    /* 0x440 */ be<s16> mKnockbackAngleY;  /* was m0324: damage_check (in daBwds_Execute 0210602C): fopAcM_searchPlayerAngleY; move (in daBwds_Execute 0210602C) YrotS by it for the knockback; knockback yaw (HD: no pitch m0326) */
    /* 0x442 */ be<s16> mHitShakeTimer;  /* was m0328: damage_check (in daBwds_Execute 0210602C) sets 15 (+ reg); daBwds_Execute 0210602C counts it down and shakes the model's Y / X rotation by it * 500 */
    /* 0x444 */ cXyz mBodyPos[0xF];  /* was m032C: body_control (in daBwds_Execute 0210602C): [0] head joint position, [1..14] each segment trailing the previous one; segment spheres m0670[i] centre on it; fail bursts at [12 - mBurstCount] */
    /* 0x4F8 */ csXyz mBodyAngle[0xF];  /* was m03E0: body_control (in daBwds_Execute 0210602C): [0] shape_angle, [i - 1] the atan2 angles toward segment i; the segment models are rotated by it */
    /* 0x552 */ u8 _552[2];
    /* 0x554 */ cXyz mBodyMoveVec[0xF];  /* was m043C: body_control (in daBwds_Execute 0210602C): last frame's segment position, then (new - old) * stiffness; added back to the next frame's trail direction (the body's swing) */
    /* 0x608 */ be<s32> mBurstCount;  /* was m04F0: fail (in daBwds_Execute 0210602C) adds 1 per segment burst (SIBOUBAKUEN particle) and moves on after 13; daBwds_Execute 0210602C / daBwds_Draw 02105F00 handle only the 13 - it segments left */
    /* 0x60C */ be<s8> mDeleteRequest;  /* was m04F4: ko_delete_sub 020F9AA0 (d_a_bwd, view bwd_detail::Larva) sets 1 for every larva; damage_check (in daBwds_Execute 0210602C) then deletes the actor */
    /* 0x60D */ be<s8> mBodyFollowMode;  /* was m04F5: hook_chance / fail (in daBwds_Execute 0210602C) set 1 (segments kept above the ground, inertia on), ug_move 2 (diving); body_control changes the trail direction and stiffness on it; daBwds_Execute 0210602C clears it every frame */
    /* 0x60E */ u8 _60E[0x614 - 0x60E];
    /* 0x614 */ be<s16> mTargetAngleX;  /* was m04FC: hook_chance (in daBwds_Execute 0210602C): 0 or 0x7F00 at random per bounce; current.angle.x is eased to it */
    /* 0x616 */ u8 _616[2];
    /* 0x618 */ be<s16> m0500;
    /* 0x61A */ be<s16> mBounceTurnY;  /* was m0502: hook_chance (in daBwds_Execute 0210602C): random +-2500 per bounce, added to current.angle.y every frame */
    /* 0x61C */ be<f32> mSandY;  /* was m0504: daBwds_Execute 0210602C: ground cross - 90 under the actor (sand surface level); ug_move / hook_chance / fail (in daBwds_Execute 0210602C) and move compare current.pos.y with it */
    /* 0x620 */ dCcD_Stts mStts;  /* was m0508: daBwds_Create 02108BF0: Init(0xFA, 0) and SetStts of every sphere; damage_check (in daBwds_Execute 0210602C) Move() */
    /* 0x65C */ dCcD_Sph mHsSph;  /* was m0544: daBwds_Create 02108BF0: Set(hs_sph_src) (the hookshot target sphere); daBwds_Execute 0210602C sets its centre on the head and radius l_HIO.mHsSphRadius * scale */
    /* 0x788 */ dCcD_Sph mBodySph[0xF];  /* was m0670: daBwds_Create 02108BF0: [0] Set(cc_sph_src) (head), [1..14] Set(body_sph_src); daBwds_Execute 0210602C / body_control place them on the head and mBodyPos; damage_check reads their Tg hits */
    /* 0x191C */ cXyz mSandSplashPos;  /* was m1804: daBwds_Execute 0210602C: y = ground cross under the actor, x / z the actor's position when it leaves or dives into the sand; the OUT / DIVE smoke and sand particles start there */
    /* 0x1928 */ cXyz mMoveSmokePos;  /* was m1810: daBwds_Execute 0210602C: the actor's x / z on the ground cross; the IDOU (moving) smoke and sand particles follow it */
    /* 0x1934 */ be<s8> mMoveSmokeState;  /* was m181C: ug_move (in daBwds_Execute 0210602C) sets 1 when the larva goes under the sand; daBwds_Execute 0210602C starts the IDOU smoke / sand at 1 (then 2) and ends them once the larva is above the sand */
    /* 0x1935 */ be<s8> mOutSmokeTimer;  /* was m181D: daBwds_Execute 0210602C sets 5 (+ reg) when the larva leaves the sand (OUT_SMOKE00), counts it down and ends that smoke at 0 */
    /* 0x1936 */ be<s8> mDiveSmokeTimer;  /* was m181E: daBwds_Execute 0210602C sets 5 (+ reg) when the larva dives into the sand (DIVE_SMOKE00), counts it down and ends that smoke at 0 */
    /* 0x1937 */ u8 _1937;
    /* 0x1938 */ dPa_smokeEcallBack_bw mSmokeCb[3];  /* was m1820: daBwds_Execute 0210602C: [0] IDOU_MOKO00 (moving), [1] OUT_SMOKE00, [2] DIVE_SMOKE00 smoke callbacks; daBwds_Delete 02108750 removes them */
    /* 0x1998 */ dPa_followEcallBack mMoveSandCb;  /* was m1880: daBwds_Execute 0210602C passes it to the IDOU_SUNA00 (moving sand) particle and ends it with the moving smoke; daBwds_Delete 02108750 removes it */
    /* 0x19AC */ be<s8> mSplashFrame[2];  /* was m1894: daBwds_Execute 0210602C: [0] leaving / [1] diving into the sand; set 1, + 1 per frame up to 60, then 0; it - 1 is the frame of the splash model's animations; daBwds_Draw 02105F00 draws the splash while it is set */
    /* 0x19AE */ u8 _19AE[2];
    /* 0x19B0 */ cXyz mSplashPos[2];  /* was m1898: daBwds_Execute 0210602C: the actor's position when the splash starts; the splash model is placed there */
    /* 0x19C8 */ gptr<mDoExt_McaMorf> mpSplashMorf[2];  /* was mp18B0: useHeapInit 021087FC creates the 2 sand splash models (Bwd BDL_GSP00 / GSP01, one for leaving and one for diving); daBwds_Execute 0210602C sets their frame and matrix, daBwds_Draw 02105F00 draws them */
    /* 0x19D0 */ gptr<mDoExt_btkAnm> mpSplashBtk[2];  /* was mp18B8: useHeapInit 021087FC: btk of each splash model; daBwds_Execute 0210602C sets its frame, daBwds_Draw 02105F00 enters it */
    /* 0x19D8 */ gptr<mDoExt_brkAnm> mpSplashBrk[2];  /* was mp18C0: useHeapInit 021087FC: brk of each splash model; daBwds_Execute 0210602C sets its frame, daBwds_Draw 02105F00 enters it */
    /* 0x19E0 */ be<u8> mHioOwner;  /* was m18C8: daBwds_Create 02108BF0 sets 1 when it created the l_HIO child; daBwds_Delete 02108750 deletes the HIO child only then */
    /* 0x19E1 */ u8 _19E1[3];
};
WWHD_OFFSET(bwds_class, mpMorf, 0x3D4);
WWHD_OFFSET(bwds_class, mTargetPos, 0x418);
WWHD_OFFSET(bwds_class, mHitShakeTimer, 0x442);
WWHD_OFFSET(bwds_class, mBodyMoveVec, 0x554);
WWHD_OFFSET(bwds_class, mStts, 0x620);
WWHD_OFFSET(bwds_class, mBodySph, 0x788);
WWHD_OFFSET(bwds_class, mSandSplashPos, 0x191C);
WWHD_OFFSET(bwds_class, mSmokeCb, 0x1938);
WWHD_OFFSET(bwds_class, mSplashFrame, 0x19AC);
WWHD_OFFSET(bwds_class, mHioOwner, 0x19E0);
WWHD_SIZE(bwds_class, 0x19E4);

/* l_HIO (0x10462C44): daBwds_HIO_c, HD layout {mNo, mNoMove, mBodyScale.., vtable last} */
struct daBwds_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> mNoMove;  /* was m005: daBwds_Execute 0210602C: the move / ground checks and the attack / push collision bits run only while it is 0 (tww the same); daBwds_HIO_c_ct sets the default, never written elsewhere in WWHD or tww (0) */
    /* 0x02 */ u8 _02[2];
    /* 0x04 */ be<f32> mBodyScale;  /* was m008: body_control / move (in daBwds_Execute 0210602C): scales the body model, the joint offsets and the body / hookshot sphere radii (tww the same); daBwds_HIO_c_ct sets the default, never written elsewhere in WWHD or tww (0.8) */
    /* 0x08 */ be<f32> mSplashScale;  /* was m00C: daBwds_Execute 0210602C: scale of the sand splash models (tww the same); daBwds_HIO_c_ct sets the default, never written elsewhere in WWHD or tww (0.7) */
    /* 0x0C */ be<f32> mMoveSpeed;  /* was m010: ug_move (in daBwds_Execute 0210602C): the speedF of each jump out of the sand (tww the same); daBwds_HIO_c_ct sets the default, never written elsewhere in WWHD or tww (7.0) */
    /* 0x10 */ be<s16> mHookChanceTime;  /* was m014: hook_chance (in daBwds_Execute 0210602C): mTimers[0] at the start; at 0 the larva goes back to ug_move (tww the same); daBwds_HIO_c_ct sets the default, never written elsewhere in WWHD or tww (220) */
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ be<f32> mSwayMax;  /* was m018: ug_move (in daBwds_Execute 0210602C): upper limit of the sway amplitude (distance * 7) (tww the same); daBwds_HIO_c_ct sets the default, never written elsewhere in WWHD or tww (2000) */
    /* 0x18 */ be<f32> mHsSphRadius;  /* was m01C: daBwds_Execute 0210602C: radius of the hookshot sphere mHsSph (times mBodyScale) (tww the same); daBwds_HIO_c_ct sets the default, never written elsewhere in WWHD or tww (300) */
    /* 0x1C */ be<u32> __vtbl;
};
WWHD_SIZE(daBwds_HIO_c, 0x20);
#ifndef BWDS_STORAGE_OWNER
// Autoglue exports the owner source's statics as references to guest storage.
extern daBwds_HIO_c& l_HIO;
extern be<u8>& hio_set;
extern GXColor& eff_col;
#endif

#define BWDS_VTBL 0x1000C2A4      /* bwds_class vtable (HD virtual destructor) */
#define BWDS_HIO_VTBL 0x1000C294  /* daBwds_HIO_c vtable */
#define SAFESTRING_VTBL 0x1000C22C

enum {
    dRes_INDEX_BWDS_BCK_KOBOSS_CLOSE_e = 4,
    dRes_INDEX_BWDS_BCK_KOBOSS_PAKUPAKU_e = 5,
    dRes_INDEX_BWDS_BDL_KOBOSS_HEAD_e = 9,

};
enum {
    JA_SE_LK_HS_SPIKE = 0x286F,
    JA_SE_CV_BWD_C_HS_DAMAGE = 0x48E3,
    JA_SE_CV_BWD_C_ATTACK = 0x48E4,
    JA_SE_CV_BWD_C_DAMAGE = 0x48E5,
    JA_SE_CV_BWD_C_DIE = 0x48E6,
    JA_SE_CM_BWD_C_MOVE_SAND = 0x61E4,
    JA_SE_CM_BWD_C_JUMP_OUT = 0x69E2,
    JA_SE_CM_BWD_C_IN_SAND = 0x69E3,
};
enum { ACTION_UG_MOVE = 0, ACTION_HOOK_ON = 1, ACTION_HOOK_CHANCE = 2, ACTION_FAIL = 5 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* debug registers (HD keeps them): REG_F(child, i) / REG_S(child, i) from bindings.h */
#define REG0_F(i) REG_F(0, i)
#define REG0_S(i) REG_S(0, i)
#define REG10_F(i) REG_F(10, i)
#define REG10_S(i) REG_S(10, i)
#define REG18_F(i) REG_F(18, i)
// HD: callback removal and the joint-matrix dirty flag use virtual/private views.
struct BwdsCallbackVtable { u8 padding[0x44]; be<u32> remove; };
struct BwdsCallback { gptr<BwdsCallbackVtable> vtable; };
struct BwdsJointMatrices { u8 padding0[4]; be<u16> mFlags; u8 padding6[10]; gptr<Mtx34> mMatrices; };
struct BwdsModel { u8 padding[0x2C]; gptr<BwdsJointMatrices> mJointMatrices; };
struct BwdsPlayerVtable { u8 padding[0x10C]; be<u32> setHookshotCarryOffset; };
WWHD_OFFSET(BwdsCallbackVtable, remove, 0x44);
WWHD_OFFSET(BwdsModel, mJointMatrices, 0x2C);
WWHD_OFFSET(BwdsJointMatrices, mMatrices, 0x10);
WWHD_OFFSET(BwdsPlayerVtable, setHookshotCarryOffset, 0x10C);
static inline void bw_vremove(void* cb) {
    gabi::call_ptr(reinterpret_cast<BwdsCallback*>(cb)->vtable->remove, cb);
}
static inline Mtx34* bw_getAnmMtx(J3DModel* model, s32 joint) {
    BwdsJointMatrices* matrices = reinterpret_cast<BwdsModel*>(model)->mJointMatrices;
    matrices->mFlags = matrices->mFlags | 0x10;
    return matrices->mMatrices.get() + joint;
}
/* 02518CC8 def_se_set(fopAc_ac_c*, cCcD_Obj*, u32) */
void bw_def_se_set(fopAc_ac_c*, void*, u32);
/* 025192A8 cc_at_check(fopAc_ac_c*, CcAtInfo*) (as in d_a_kb.h) */
struct CcAtInfo_bw {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ u8 _08[0x14 - 0x08];
    /* 0x14 */ be<u32> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(CcAtInfo_bw, 0x1C);
void bw_cc_at_check(fopAc_ac_c*, CcAtInfo_bw*);
/* bw_particle_setToon (HD): dPa_control_c::set with group 2, setupInfo = roomNo (as in d_a_mo2.h) */
static inline JPABaseEmitter* bw_particle_setToon(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha,
                                                        void* cb, s8 roomNo) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, angle, scale, alpha, (dPa_levelEcallBack*)cb, roomNo, nullptr, nullptr, nullptr);
}
/* dPa_smokeEcallBack::setColor (inline): 4-byte copy */
static inline void smoke_setColor(dPa_smokeEcallBack_bw* cb, const GXColor* col) {
    u8 r = col->r, g = col->g, b = col->b, a = col->a;
    cb->mColor[0] = r; cb->mColor[1] = g; cb->mColor[2] = b; cb->mColor[3] = a;
}
static inline dPa_smokeEcallBack* smoke(dPa_smokeEcallBack_bw* cb) { return (dPa_smokeEcallBack*)cb; }
/* dBgS_GndChk on the stack (this TU's vtables) */
static const dBgS_GndChk_vt BW_GNDCHK_VT = {0x1000C254, 0x1000C264, 0x1000C284, 0x1000C274};
void bw_checkDtor(void*, s32);
static inline void bw_GndChk_dt(dBgS_GndChk* check) {
    static_cast<cBgS_PolyInfo*>(check)->__vtbl = 0x1000C264;
    static_cast<dBgS_PolyPassChk*>(check)->__vtbl = 0x1000C284;
    static_cast<dBgS_GrpPassChk*>(check)->__vtbl = 0x1000C244;
    bw_checkDtor(check, 0);
}
// HD: constructor entries return the original pointer (including NULL).
void* bw_brkCtor(void*);
void* bw_btkCtor(void*);
void bw_constructArray(void*, u32, u32, u32);
void bw_destroyArray(void*, u32, u32, u32, u32, u32);
void bw_sphereCtor(void*);
void bw_sphereDtor(void*, s32);
void* bw_smokeCtor(void*, u8);
void bw_actorDtor(fopAc_ac_c*, s32);
void bw_anm_init(bwds_class*, int, f32, u8, f32, int);
void bw_seStart(fopAc_ac_c*, u32, u32);

// HD: external array element callbacks.
static constexpr u32 BWDS_SPHERE_CTOR = 0x025166F0;
static constexpr u32 BWDS_SPHERE_DTOR = 0x02515AE8;

static inline BwdsPlayerVtable* bw_playerVtable(fopAc_ac_c* player) {
    return gabi::at<BwdsPlayerVtable>(player->__vtbl);
}

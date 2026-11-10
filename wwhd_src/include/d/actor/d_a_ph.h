/* ph_class (Peahat / Seahat), WWHD layout. 
 *
 * GameCube -> WWHD: +0x11C up to the second enemyfire (the GameCube header's names are kept,
 * mGCOFFSET fields at GameCube offset + 0x11C); enemyfire is 0x22C in HD (GameCube 0x228), so
 * the invisible models and mpJntHit are +0x124. Size 0x12D8 (constructor 023CF44C).
 * Offsets from the verified functions. */
#pragma once
#include "bindings.h"

#include "c/c_damagereaction.h"
#include "d/d_cc_uty.h"
#include "d/d_jnt_hit.h"

// HD: ripple callbacks gain a rate field after the emitter and inherited state.
struct PhRippleCallbackHD {
    be<u32> __vtbl;
    gptr<JPABaseEmitter> mpEmitter;
    be<u32> mState[2];
    be<f32> mRate;
};
WWHD_OFFSET(PhRippleCallbackHD, mRate, 0x10);
WWHD_SIZE(PhRippleCallbackHD, 0x14);
struct PhInvisibleModelHD {
    gptr<J3DModel> mpModel;
    be<u32> mState;
};
WWHD_SIZE(PhInvisibleModelHD, 8);

// HD: the two joint-hit source records retain the verified native word layout.
struct PhJointHitSourceHD {
    __jnt_hit_data_c mHits[2];
};
WWHD_SIZE(PhJointHitSourceHD, 0x18);

struct ph_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ u8 _3D0[4];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpPropellerMorf;
    /* 0x3D8 */ gptr<mDoExt_McaMorf> mpBodyMorf;
    /* 0x3DC */ cXyz mBodyTopPos;  /* was m02C0: nodeCallBack_DW 023C97BC: world position of body joint 1; the propeller is placed there while attached (daPH_Execute 023CC070, daPH_Create 023CF600 copy it to mPropellerPos); eyePos and attention position follow it */
    /* 0x3E8 */ cXyz mBodyCenterPos;  /* was m02CC: nodeCallBack_DW 023C97BC: world position of body joint 0; daPH_Execute 023CC070 sets mBodySph's centre there (or below it) */
    /* 0x3F4 */ cXyz mPropellerCylPos;  /* was m02D8: nodeCallBack_UP 023C9678: propeller joint 0 position 80 below; daPH_Execute 023CC070 sets mAtCyl / mTgCyl's centre there */
    /* 0x400 */ cXyz mFloatOffset;  /* was m02E4: daPH_Execute 023CC070 eases it towards mFloatTarget; DW_draw_SUB 023CBF44 / daPH_Draw 023C9994 / ph_hani_check 023CB368 add it to current.pos (the drawn body position); body_atari_check 023CA5AC folds it into current.pos on ice / light arrows */
    /* 0x40C */ cXyz mFloatTarget;  /* was m02F0: fuwafuwa_set 023CA0BC: sine targets per axis (Peahat 30 / 200, Seahat 30 / 1000); daPH_Execute 023CC070 eases mFloatOffset to it; cleared when the drift stops */
    /* 0x418 */ cXyz mPropellerScale;  /* was m02FC: UP_draw_SUB 023CBD80 scales the propeller model by it; 0 means no propeller (BG_check 023CBBE4, body_atari_check 023CA5AC test x != 0); ph_fujyou_grow (in daPH_Execute 023CC070) grows it to mPropellerExtraScale + 1 */
    /* 0x424 */ cXyz mHopCenterPos;  /* was m0308: ph_bunri_move (in daPH_Execute 023CC070): current.pos when the propeller-less body lands; the hops turn back towards it when farther than 200 */
    /* 0x430 */ cXyz mPropellerPos;  /* was m0314: UP_draw_SUB 023CBD80 translates the propeller model there; daPH_Execute 023CC070 copies mBodyTopPos while attached, moves it away by mPropellerFlyDist once it is knocked off */
    /* 0x43C */ cXyz mSquashScale;  /* was m0320: ph_damage_dead_move (in daPH_Execute 023CC070): target of actor scale (mExtraScale + 1, 0.8/1.2 squash on the ground bounces); scale eases to it; target scale (damage) */
    /* 0x448 */ cXyz mHomePos;  /* was m032C: daPH_Create 023CF600 copies current.pos; ph_hani_check 023CB368 (useHome) and the Seahat moves (in daPH_Execute 023CC070) measure from it and return to it; sea_water_check 023CB4C0 starts mWaterSurfacePos.y from it */
    /* 0x454 */ u8 _454[2];
    /* 0x456 */ be<s16> mPropellerAngle;  /* was m033A: puropera_kaiten 023CA054 adds mPropellerSpeed; UP_draw_SUB 023CBD80 rotates the propeller by it; puropera_sound 023C9E58 plays a blade sound at three angles; propeller angle */
    /* 0x458 */ u8 _458[2];
    /* 0x45A */ be<u8> mType; /* 0 Peahat, 1 Seahat */
    /* 0x45B */ be<u8> mAction;  /* was m033F: daPH_Execute 023CC070 switches on it (PH_ACTION_FLY / BOUNCE / SEPARATED / RISE / DAMAGE / WIND / WATER); set by body_atari_check 023CA5AC, hajiki_check 023CAFB0, ph_wall_hit_check 023CB2E0 */
    /* 0x45C */ be<u8> mDamageType;  /* was m0340: body_atari_check 023CA5AC: reaction per attack type (2 wind, 3 critical sword, 4 boomerang knock-off, 6/7 skull hammer, 8 bomb, 9 hookshot, 1 arrows / fire); ph_damage_dead_move and ph_wind_move 023CFDF0 test it */
    /* 0x45D */ be<u8> mInWater;  /* was m0341: sea_water_check 023CB4C0 sets 1 on entering water (ripple, splash, water pillar), ph_leave_water (in sea_water_check) clears it; daPH_Execute 023CC070 tests it */
    /* 0x45E */ be<u8> mIceHit;  /* was m0342: body_atari_check 023CA5AC sets 1 for AT_TYPE_ICE_ARROW (mEnemyIce freeze 200); ph_damage_dead_move (in daPH_Execute 023CC070) then stops the body (mode 0x2E) */
    /* 0x45F */ be<u8> mStealHitCount;  /* was m0343: body_atari_check 023CA5AC: +1 per AT_TYPE_GRAPPLING_HOOK steal while items are left; never read; hit counter */
    /* 0x460 */ be<u8> mStealItemsLeft;  /* was m0344: daPH_Create 023CF600 sets 1; body_atari_check 023CA5AC copies it to stealItemLeft for an AT_TYPE_GRAPPLING_HOOK hit and decrements it after the steal */
    /* 0x461 */ u8 _461;
    /* 0x462 */ be<s16> mMode;  /* was m0346: step within mAction: every move function switches on it (ph_fly_move 0..4, ph_hane_move 10, ph_bunri_move 0x14.., ph_fujyou_move 0x1E.., ph_damage_dead_move 0x28.., ph_wind_move 023CFDF0 0x32.., ph_water_move 023D051C 0x3C..); UP_draw_SUB 023CBD80 tests 3/4/0x33 */
    /* 0x464 */ be<s16> mPropellerSpeed;  /* was m0348: puropera_kaiten 023CA054 adds it to mPropellerAngle and eases it to mPropellerTargetSpeed by mPropellerAccel; puropera_sound 023C9E58 (Seahat) sets the sound volume from it; propeller speed */
    /* 0x466 */ be<s16> mPropellerTargetSpeed;  /* was m034A: puropera_kaiten 023CA054: target of mPropellerSpeed; the moves set 0x1000 (hover) / 0x2000 (attack); target propeller speed */
    /* 0x468 */ be<s16> mPropellerAccel;  /* was m034C: puropera_kaiten 023CA054: step of mPropellerSpeed, itself eased to 0x100; the moves reset it to 0 with each new mPropellerTargetSpeed */
    /* 0x46A */ be<s16> mDamageSpinSpeed;  /* was m034E: ph_damage_dead_move (in daPH_Execute 023CC070): 0x2000 / 0x1000 / -0x8000 at a hit, added to shape_angle.y and eased to 0; spin speed (damage) */
    /* 0x46C */ be<s16> mSeparatedTimer;  /* was m0350: ph_bunri_move (in daPH_Execute 023CC070): 900 (Peahat) / 150 (Seahat) when the propeller comes off, counts down while hopping; at 0 the propeller regrows (PH_ACTION_RISE) */
    /* 0x46E */ be<s16> mTargetAngleY;  /* was m0352: ph_fly_move / ph_fly_sea_move (in daPH_Execute 023CC070): angle to the player or home; current.angle.y eases to it; target angle */
    /* 0x470 */ be<s16> mPropellerSeFlags;  /* was m0354: puropera_sound 023C9E58 (Peahat): bits 1/2/4 for the three blade angles already sounded, cleared when all three are set; propeller sound flags */
    union {
        struct {
            /* 0x472 */ be<s16> mFloatPhaseY;  /* was m0356: fuwafuwa_set 023CA0BC: +700 (Seahat 350) per frame, sine of it is mFloatTarget.y; ph_fujyou_rise +2000; daPH_Create 023CF600 seeds it from the process id */
            /* 0x474 */ be<s16> mFloatPhaseX;  /* was m0358: fuwafuwa_set 023CA0BC: +200 (Seahat 100) per frame, cosine of it is mFloatTarget.x; daPH_Create 023CF600 seeds it from the process id */
            /* 0x476 */ be<s16> mFloatPhaseZ;  /* was m035A: fuwafuwa_set 023CA0BC: +200 (Seahat 100) per frame, cosine of it is mFloatTarget.z; daPH_Create 023CF600 seeds it from the process id */
            /* 0x478 */ be<s16> mPitchWobblePhase;  /* was m035C: fly_angle_set 023CA258: +800 (Seahat 900) per frame, shape_angle.x eases to sin(it) * 4000; pitch wobble */
            /* 0x47A */ be<s16> mRollWobblePhase;  /* was m035E: fly_angle_set 023CA258 (mode 4) +3000, angle.z = sin(it) * 7000; ph_fujyou_rise and ph_wind_move 023CFDF0 roll by its sine; ph_damage_dead_move also counts ground bounces in it; roll wobble */
            /* 0x47C */ be<s16> mSpinSpeed;  /* was m0360: ph_wind_move 023CFDF0: +-10000 / +-1000 added to mRollWobblePhase, later to shape_angle.y; spin speed */
            /* 0x47E */ be<s16> mWindTurnSpeed;  /* was m0362: ph_wind_move 023CFDF0: +-0x1000 added to shape_angle.y while blown away, eased to 0 */
        };
        be<s16> mWobbleAngles[7];
    };
    union {
        struct {
            /* 0x480 */ be<s16> mActionTimer;  /* was m0364: mTimers[0] (counted down in daPH_Execute 023CC070): waits of the fly / hane / damage moves (ph_fly_move 20, 60..120, ph_hane_move 15, ph_damage_dead_move 0x3C) */
            /* 0x482 */ be<s16> mWaitTimer;  /* was m0366: mTimers[1]: ph_water_anm random wait in water (100..400), ph_wind_move 023CFDF0 50..100, ph_fly_sea_move 15-frame splash interval; timer */
            /* 0x484 */ be<s16> mHomeCheckTimer;  /* was m0368: mTimers[2]: ph_fly_sea_move / ph_water_move 023D051C (30 / 20); the distance-from-home check runs only at 0 */
            /* 0x486 */ be<s16> mHDTimer;
            /* 0x488 */ be<s16> mHitCooldown;  /* was m036C: mTimers[4]: body_atari_check 023CA5AC sets 8 after a hit and ignores hits while it is not 0; hit cooldown */
        };
        be<s16> mTimers[5];
    };
    /* 0x48A */ be<s16> m036E;
    /* 0x48C */ be<s16> mBobPhase;  /* was m0370: sea_water_check 023CB4C0: +1000 per frame while floating, pos.y bobs by its sine below the water height; cleared on entering */
    /* 0x48E */ be<s16> mAttackDelay;  /* was m0372: ph_fly_sea_move (in daPH_Execute 023CC070): Seahat waits it out (0x3C after an attack) before checking the player again; Seahat: attack delay */
    /* 0x490 */ be<s32> mPropellerAnmIdx;  /* was m0374: anm_init 023C9AD8 stores the propeller BCK index (part 0, Peahat); the moves skip re-starting 0x1C / 0x1E when it is already playing */
    /* 0x494 */ be<f32> mHeightAbovePlayer;  /* was m0378: fly_angle_set 023CA258: pitch aims at player y + it; the moves set 150 / 80 / 100; target height above the player */
    /* 0x498 */ be<f32> mPropellerFlyDist;  /* was m037C: daPH_Execute 023CC070: once the propeller is knocked off it eases to 10 / 30 and moves mPropellerPos by it; reset to 0 with mPropellerScale 0; 0 with mPropellerScale != 0 means attached (BG_check 023CBBE4, body_atari_check 023CA5AC) */
    /* 0x49C */ be<f32> mBgCheckOffsetY;  /* was m0380: ph_crr_pos (in BG_check 023CBBE4): pos is lowered by it for mAcch.CrrPos and raised back; daPH_Create 023CF600 30 (Seahat 100), ph_damage_dead_move 10 */
    /* 0x4A0 */ be<f32> mRiseSpeed;  /* was m0384: ph_fujyou_rise (in daPH_Execute 023CC070): speed.y, eased to the given target; also drives mPropellerAngle and the roll; rising speed */
    /* 0x4A4 */ be<f32> mRiseTargetY;  /* was m0388: ph_fujyou_move (in daPH_Execute 023CC070): pos.y + 200 at the take-off start; flying resumes above it; rising target height */
    /* 0x4A8 */ be<f32> mHomeRange;  /* was m038C: daPH_Create 023CF600: mNoticeRange + 800; ph_fly_sea_move / ph_water_move 023D051C return home when farther from mHomePos than it */
    /* 0x4AC */ be<f32> mNoticeRange;  /* was m0390: daPH_Create 023CF600: param bits 8-15 * 100 (default 1000, Seahat 12000); ph_hani_check 023CB368 horizontal range for starting an attack */
    /* 0x4B0 */ be<f32> mNoticeHeight;  /* was m0394: daPH_Create 023CF600: param bits 16-23 * 100 (default 500, Seahat 6000); ph_hani_check 023CB368 height range for starting an attack */
    /* 0x4B4 */ be<f32> mWaterFollowSpeed;  /* was m0398: sea_water_check 023CB4C0: step of pos.y towards the water height, eased to 100 / 30; ph_leave_water clears it */
    /* 0x4B8 */ be<f32> mExtraScale;  /* was m039C: daPH_Create 023CF600: 0, Seahat 8 (actor scale 9); added to particle / fire / ice scales and the damage squash (body_atari_check 023CA5AC, dead_item 023CBAC0) */
    /* 0x4BC */ be<f32> mPropellerExtraScale;  /* was m03A0: daPH_Create 023CF600: Seahat 5; the propeller scale is mPropellerExtraScale + 1 (ph_fujyou_grow, daPH_Execute 023CC070) */
    /* 0x4C0 */ dBgS_AcchCir mAcchCir;
    /* 0x500 */ dBgS_Acch mAcch;
    /* 0x6C4 */ PhRippleCallbackHD mParticleCallBack; /* dPa_rippleEcallBack */
    /* 0x6D8 */ cXyz mWaterSurfacePos;  /* was m05BC: sea_water_check 023CB4C0: current.pos x/z + mFloatOffset with y = the sea wave / water height; ripple and splash position; the moves fly 500 above its y */
    /* 0x6E4 */ u8 _6E4[8];
    /* 0x6EC */ dCcD_Stts mStts;
    /* 0x728 */ dCcD_Sph mBodySph;
    /* 0x854 */ dCcD_Cyl mAtCyl;
    /* 0x984 */ dCcD_Cyl mTgCyl;
    /* 0xAB4 */ enemyice mEnemyIce;
    /* 0xE6C */ enemyfire mBodyEnemyFire;
    /* 0x1098 */ enemyfire mPropellerEnemyFire;
    /* 0x12C4 */ PhInvisibleModelHD mPropellerInvisibleModel; /* mDoExt_invisibleModel */
    /* 0x12CC */ PhInvisibleModelHD mBodyInvisibleModel;
    /* 0x12D4 */ be<u32> mpJntHit;
};
WWHD_OFFSET(ph_class, mpPropellerMorf, 0x3D4);
WWHD_OFFSET(ph_class, mType, 0x45A);
WWHD_OFFSET(ph_class, mPropellerFlyDist, 0x498);
WWHD_OFFSET(ph_class, mAcch, 0x500);
WWHD_OFFSET(ph_class, mStts, 0x6EC);
WWHD_OFFSET(ph_class, mAtCyl, 0x854);
WWHD_OFFSET(ph_class, mEnemyIce, 0xAB4);
WWHD_OFFSET(ph_class, mBodyEnemyFire, 0xE6C);
WWHD_OFFSET(ph_class, mPropellerInvisibleModel, 0x12C4);
WWHD_SIZE(ph_class, 0x12D8);

// HD: J3DNode_toJoint returns joint data, whose index occupies the first halfword after the header.
struct PhJointDataHD {
    be<u32> mHeader;
    be<u16> mJointNo;
};
WWHD_OFFSET(PhJointDataHD, mJointNo, 4);
WWHD_SIZE(PhJointDataHD, 6);
inline PhJointDataHD *phJointData(J3DJoint *joint) {
    return gabi::at<PhJointDataHD>(gabi::ea(joint));
}

// HD: preserve the three integer loads before any store in GHS vector copies.
struct PhVectorWordsHD {
    be<u32> x, y, z;
};
WWHD_SIZE(PhVectorWordsHD, 12);
inline PhVectorWordsHD *phVectorWords(cXyz *vector) {
    return gabi::at<PhVectorWordsHD>(gabi::ea(vector));
}
inline const PhVectorWordsHD *phVectorWords(const cXyz *vector) {
    return gabi::at<PhVectorWordsHD>(gabi::ea(vector));
}

struct PhAttackWordsHD {
    be<u32> mWords[7];
};
WWHD_SIZE(PhAttackWordsHD, sizeof(CcAtInfo));
inline PhAttackWordsHD *phAttackWords(CcAtInfo *info) {
    return gabi::at<PhAttackWordsHD>(gabi::ea(info));
}

// HD: only the particle controller's native splash scale is needed by this actor.
struct PhParticleScaleHD {
    u8 mControllerState[0xF8];
    be<f32> mSplashScale;
};
WWHD_OFFSET(PhParticleScaleHD, mSplashScale, 0xF8);
WWHD_SIZE(PhParticleScaleHD, 0xFC);
inline PhParticleScaleHD *phParticleScale(dPa_control_c *controller) {
    return gabi::at<PhParticleScaleHD>(gabi::ea(controller));
}
// HD: the name table is opaque; preserve its own address as the native lookup's this argument.
struct PhNameTableHD {
    u8 mNativeState;
};
struct PhGameNameTableHD {
    u8 mGameState[PLAY_NAMETBL];
    PhNameTableHD mNameTable;
};
WWHD_OFFSET(PhGameNameTableHD, mNameTable, PLAY_NAMETBL);
inline PhNameTableHD *phNameTable() {
    return &gabi::at<PhGameNameTableHD>(dComIfGp_ea())->mNameTable;
}

struct PhJ3DSysModelHD {
    gptr<J3DModel> mpModel;
};
WWHD_SIZE(PhJ3DSysModelHD, 4);
WWHD_SIZE(PhNameTableHD, 1);
WWHD_SIZE(PhGameNameTableHD, PLAY_NAMETBL + 1);
WWHD_OFFSET(ph_class, mWobbleAngles, 0x472);
WWHD_OFFSET(ph_class, mTimers, 0x480);
WWHD_OFFSET(ph_class, mParticleCallBack, 0x6C4);
WWHD_OFFSET(ph_class, mBodyInvisibleModel, 0x12CC);

enum PhType { PH_TYPE_PEAHAT, PH_TYPE_SEAHAT };
enum PhAction {
    PH_ACTION_FLY,
    PH_ACTION_BOUNCE,
    PH_ACTION_SEPARATED,
    PH_ACTION_RISE,
    PH_ACTION_DAMAGE,
    PH_ACTION_WIND,
    PH_ACTION_WATER,
};

// HD: unit-local native bindings retain the verified register widths and overloads.
void phSphereConstruct(dCcD_Sph *sphere);
void phAcchCirConstruct(dBgS_AcchCir *cir);
void phAcchConstruct(dBgS_Acch *acch);
void phRippleConstruct(void *callback);
void phInvisibleConstruct(void *model);
void phInvisibleDestroy(void *model, s32 flags);
void phSphereDestroy(dCcD_Sph *sphere, s32 flags);
void phAcchDestroy(u32 acch, s32 flags);
void phCircleDestroy(u32 circle, s32 flags);
void phActorDestroy(fopAc_ac_c *actor, s32 flags);
void phCancelBoomerangLock();
BOOL phNative_enemy_ice(enemyice *ice);
void phNative_enemy_fire(enemyfire *fire);
void phNative_enemy_fire_remove(enemyfire *fire);
void phNative_PSVECSubtract(const cXyz *a, const cXyz *b, cXyz *out);
void phNative_dSnap_RegistFig(s32 type, fopAc_ac_c *a, cXyz *pos, s16 angleY, f32 x, f32 y, f32 z);
void phNative_mDoExt_invisibleModel_entryDL(mDoExt_McaMorf *morf, s32 p, void *inv);
BOOL phNative_mDoExt_invisibleModel_create(void *inv, J3DModel *model);
u32 phNative_JntHit_create(J3DModel *model, u32 src, s16 num);
s32 phNameTableGetIndex(void *table, const char *name, s32 p);
void phNative_fopAcM_setGbaName(fopAc_ac_c *a, u8 itemNo, u8 n0, u8 n1);
void phNative_cc_at_check(fopAc_ac_c *a, CcAtInfo *info);
void phNative_mDoAud_monsSeStart(u32 id, cXyz *pos, u32 pid, u32 param, s32 reverb);
u32 phNative_dBgS_GetMtrlSndId(dBgS *bgs, void *poly);

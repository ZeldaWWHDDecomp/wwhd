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
    /* 0x3DC */ cXyz m02C0;
    /* 0x3E8 */ cXyz m02CC;
    /* 0x3F4 */ cXyz m02D8;
    /* 0x400 */ cXyz m02E4;
    /* 0x40C */ cXyz m02F0;
    /* 0x418 */ cXyz m02FC;
    /* 0x424 */ cXyz m0308;
    /* 0x430 */ cXyz m0314;
    /* 0x43C */ cXyz m0320; /* target scale (damage) */
    /* 0x448 */ cXyz m032C;
    /* 0x454 */ u8 _454[2];
    /* 0x456 */ be<s16> m033A; /* propeller angle */
    /* 0x458 */ u8 _458[2];
    /* 0x45A */ be<u8> mType; /* 0 Peahat, 1 Seahat */
    /* 0x45B */ be<u8> m033F;
    /* 0x45C */ be<u8> m0340;
    /* 0x45D */ be<u8> m0341;
    /* 0x45E */ be<u8> m0342;
    /* 0x45F */ be<u8> m0343; /* hit counter */
    /* 0x460 */ be<u8> m0344;
    /* 0x461 */ u8 _461;
    /* 0x462 */ be<s16> m0346;
    /* 0x464 */ be<s16> m0348; /* propeller speed */
    /* 0x466 */ be<s16> m034A; /* target propeller speed */
    /* 0x468 */ be<s16> m034C;
    /* 0x46A */ be<s16> m034E; /* spin speed (damage) */
    /* 0x46C */ be<s16> m0350;
    /* 0x46E */ be<s16> m0352; /* target angle */
    /* 0x470 */ be<s16> m0354; /* propeller sound flags */
    union {
        struct {
            /* 0x472 */ be<s16> m0356;
            /* 0x474 */ be<s16> m0358;
            /* 0x476 */ be<s16> m035A;
            /* 0x478 */ be<s16> m035C; /* pitch wobble */
            /* 0x47A */ be<s16> m035E; /* roll wobble */
            /* 0x47C */ be<s16> m0360; /* spin speed */
            /* 0x47E */ be<s16> m0362;
        };
        be<s16> mWobbleAngles[7];
    };
    union {
        struct {
            /* 0x480 */ be<s16> m0364;
            /* 0x482 */ be<s16> m0366; /* timer */
            /* 0x484 */ be<s16> m0368;
            /* 0x486 */ be<s16> mHDTimer;
            /* 0x488 */ be<s16> m036C; /* hit cooldown */
        };
        be<s16> mTimers[5];
    };
    /* 0x48A */ be<s16> m036E;
    /* 0x48C */ be<s16> m0370;
    /* 0x48E */ be<s16> m0372; /* Seahat: attack delay */
    /* 0x490 */ be<s32> m0374;
    /* 0x494 */ be<f32> m0378; /* target height above the player */
    /* 0x498 */ be<f32> m037C;
    /* 0x49C */ be<f32> m0380;
    /* 0x4A0 */ be<f32> m0384; /* rising speed */
    /* 0x4A4 */ be<f32> m0388; /* rising target height */
    /* 0x4A8 */ be<f32> m038C;
    /* 0x4AC */ be<f32> m0390;
    /* 0x4B0 */ be<f32> m0394;
    /* 0x4B4 */ be<f32> m0398;
    /* 0x4B8 */ be<f32> m039C;
    /* 0x4BC */ be<f32> m03A0;
    /* 0x4C0 */ dBgS_AcchCir mAcchCir;
    /* 0x500 */ dBgS_Acch mAcch;
    /* 0x6C4 */ PhRippleCallbackHD mParticleCallBack; /* dPa_rippleEcallBack */
    /* 0x6D8 */ cXyz m05BC;
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
WWHD_OFFSET(ph_class, m037C, 0x498);
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

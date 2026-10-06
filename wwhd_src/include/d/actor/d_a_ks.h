/* ks_class (Morth), WWHD layout. 
 *
 * GameCube -> WWHD: fopEn_enemy_c +0x11C; HD inserts two mDoExt_J3DModelPacketS (0xB0 each,
 * constructor 02080404) after the anm pointers, so the members from m2C8 (GameCube 0x2C8) on are
 * +0x27C. enemyice keeps its GameCube layout (0x3B8), enemyfire is 0x22C. Size 0xF08 (constructor
 * 021AA364; GameCube 0xC8C). */
#pragma once
#include "bindings.h"

#ifndef WWHD_ENEMYICE_FIRE_L
#define WWHD_ENEMYICE_FIRE_L
/* enemyice / enemyfire (c_damagereaction), WWHD layouts (SHARED-CANDIDATE, same as d_a_cc.h) */
struct enemyice_l {
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
WWHD_SIZE(enemyice_l, 0x3B8);

struct enemyfire_l {
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
    /* 0x208 */ u8 mLight[0x20];  /* LIGHT_INFLUENCE */
    /* 0x228 */ be<f32> m228;     /* HD: 1.0f in the constructor */
};
WWHD_SIZE(enemyfire_l, 0x22C);
#endif

struct ks_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpBodyMorf;
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpEyeMorf;
    /* 0x3D8 */ gptr<mDoExt_btkAnm> mpEyeBtkAnm;
    /* 0x3DC */ gptr<mDoExt_brkAnm> mpBodyBrkAnm;
    /* 0x3E0 */ gptr<mDoExt_brkAnm> mpEyeBrkAnm;
    /* 0x3E4 */ u8 mPacket[2][0xB0];   /* HD: mDoExt_J3DModelPacketS x2 (set up in GanonK) */
    /* 0x544 */ be<u8> m2C8;
    /* 0x545 */ be<u8> m2C9;
    /* 0x546 */ be<u8> m2CA;
    /* 0x547 */ be<u8> mAction;
    /* 0x548 */ be<u8> mMode;
    /* 0x549 */ be<u8> m2CD;
    /* 0x54A */ be<u8> m2CE;
    /* 0x54B */ be<u8> m2CF;
    /* 0x54C */ be<u8> m2D0;
    /* 0x54D */ be<u8> m2D1;
    /* 0x54E */ be<u8> m2D2;
    /* 0x54F */ be<u8> m2D3;
    /* 0x550 */ be<u32> mGmID;
    /* 0x554 */ be<u32> mKsID;
    /* 0x558 */ cXyz m2DC;
    /* 0x564 */ be<s16> m2E8[4];
    /* 0x56C */ be<s16> m2F0[5];
    /* 0x576 */ be<s16> m2FA;
    /* 0x578 */ be<s16> m2FC;
    /* 0x57A */ be<s16> m2FE;
    /* 0x57C */ be<s16> m300;
    /* 0x57E */ be<s16> m302;
    /* 0x580 */ be<f32> m304;
    /* 0x584 */ be<f32> m308;
    /* 0x588 */ be<f32> m30C;
    /* 0x58C */ be<f32> m310;
    /* 0x590 */ be<f32> m314;
    /* 0x594 */ be<f32> m318;
    /* 0x598 */ be<f32> m31C;
    /* 0x59C */ be<f32> m320;
    /* 0x5A0 */ dBgS_AcchCir mAcchCir;
    /* 0x5E0 */ dBgS_ObjAcch mAcch;
    /* 0x7A4 */ be<u32> m528;           /* MtxP */
    /* 0x7A8 */ u8 m52C[0x14];          /* dPa_rippleEcallBack */
    /* 0x7BC */ dCcD_Stts mStts;
    /* 0x7F8 */ dCcD_Sph mSph;
    /* 0x924 */ enemyice_l mEnemyIce;
    /* 0xCDC */ enemyfire_l mEnemyFire;
};
WWHD_OFFSET(ks_class, m2C8, 0x544);
WWHD_OFFSET(ks_class, m320, 0x59C);
WWHD_OFFSET(ks_class, mAcch, 0x5E0);
WWHD_OFFSET(ks_class, mSph, 0x7F8);
WWHD_OFFSET(ks_class, mEnemyIce, 0x924);
WWHD_SIZE(ks_class, 0xF08);

/* ---- file statics (.data, d_a_ks.cpp) ---- */
static inline be<s32>& KS_ALL_COUNT() { return *gabi::at<be<s32>>(0x101B883C); }
static inline be<s32>& KUTTUKU_ALL_COUNT() { return *gabi::at<be<s32>>(0x101B8840); }
static inline be<s32>& HEAVY_IN() { return *gabi::at<be<s32>>(0x101B8844); }
static inline be<s32>& GORON_COUNT() { return *gabi::at<be<s32>>(0x101B8848); }

#define KS_SAFESTRING_VTBL 0x10013668 /* this TU's sead::SafeString vtable */
#define KS_VTBL 0x100136C0            /* ks_class vtable (HD virtual destructor) */

/* functions called across the source files of this unit */
void draw_SUB(ks_class* i_this);
BOOL tyaku_check(ks_class* i_this);
BOOL ks_kuttuki_check(ks_class* i_this);
BOOL body_atari_check(ks_class* i_this);
void speed_keisan(ks_class* i_this, s16 i_speed);
void dead_eff_set(ks_class* i_this, cXyz* i_pos);
void BG_check(ks_class* i_this);

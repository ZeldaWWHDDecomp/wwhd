// Execution part: exact shared Moblin layout, isolated from legacy accessor copies.
/* mo2_class (Moblin), WWHD layout.
 *
 * GameCube -> WWHD (offsets measured from the verified functions and the class constructor
 * 021CC91C, which allocates 0x343C; GameCube 0x3038):
 *  - +0x11C up to mpMorf (fopEn_enemy_c);
 *  - mDoExt_btpAnm is 0x74 (GameCube 0x14);
 *  - mShadowHandle (GameCube 0x2E4) is gone (HD shadows): +0x178 from m02E8;
 *  - mDoExt_3DlineMat0_c is 0x148 (GameCube 0x1C);
 *  - a new float after m05C0 (HD: |dy| to the target), so +0x2A8 from m05C4;
 *  - damagereaction is 0x7F0 (GameCube 0x7BC): its joint arrays have 21 entries, m3D0 15;
 *  - +0x2DC from m0DD0, +0x408 after the second 3DlineMat0, +0x404 from the ga_s array;
 *  - m2A50 (J3DMaterialTable*) is gone; enemyice at 0x2E54; enemyfire is 0x22C. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/JSystem/JPAEmitter.h"

WWHD_OPAQUE(JntHit_c);
/* J3DMaterial: JSystem/J3DGraph.h */
struct mDoExt_btpAnm_l {
    u8 _[0x74];
}; /* HD size (constructor 025E7820) */
struct mDoExt_3DlineMat0_l {
    u8 _[0x148];
}; /* HD size (constructor 025E9960) */
struct dPa_smokeEcallBack_l {
    u8 _[0x20];
};
struct LIGHT_INFLUENCE_l {
    u8 _[0x24];
}; /* HD: 0x24 */
struct JntHit_HIO_l {
    u8 _[0x2C];
};

struct ke_s {
    /* 0x000 */ cXyz m000[10];
    /* 0x078 */ cXyz m078[10];
    /* 0x0F0 */ u8 m0F0[0x124 - 0x0F0];
};
WWHD_SIZE(ke_s, 0x124);

struct ga_s {
    /* 0x00 */ gptr<J3DModel> mpModel;
    /* 0x04 */ cXyz mPos;
    /* 0x10 */ cXyz m10;
    /* 0x1C */ be<s16> m1C;
    /* 0x1E */ be<s16> m1E;
    /* 0x20 */ u8 m20[4];
    /* 0x24 */ be<f32> m24;
    /* 0x28 */ be<f32> m28;
    /* 0x2C */ be<s16> m2C;
    /* 0x2E */ be<u8> m2E;
    /* 0x2F */ be<u8> m2F;
};
WWHD_SIZE(ga_s, 0x30);

struct damagereaction_l {
    enum { TYPE_MOBLIN = 1 };
    /* 0x0000 */ gptr<fopEn_enemy_c> mpEnemy;
    /* 0x0004 */ be<s16> mMode;
    /* 0x0006 */ be<s16> mAction;
    /* 0x0008 */ be<s16> mEnemyType;
    /* 0x000A */ u8 _000A[0x2];
    /* 0x000C */ be<s32> mTimer;
    /* 0x0010 */ csXyz m010[21];
    /* 0x008E */ csXyz m088[21];
    /* 0x010C */ cXyz m100[21];
    /* 0x0208 */ cXyz m1F0[21];
    /* 0x0304 */ cXyz m2E0[21];
    /* 0x0400 */ be<s32> m3D0[15];
    /* 0x043C */ be<s32> m408;
    /* 0x0440 */ be<s32> m40C;
    /* 0x0444 */ be<s32> m410;
    /* 0x0448 */ be<s32> m414;
    /* 0x044C */ be<s32> m418;
    /* 0x0450 */ u8 _0450[0x4];
    /* 0x0454 */ be<s32> m420;
    /* 0x0458 */ be<s32> m424;
    /* 0x045C */ be<f32> m428;
    /* 0x0460 */ cXyz m42C;
    /* 0x046C */ be<s32> m438;
    /* 0x0470 */ u8 _0470[0x4];
    /* 0x0474 */ be<s16> m440;
    /* 0x0476 */ be<s16> m442;
    /* 0x0478 */ be<s16> m444;
    /* 0x047A */ be<s16> m446;
    /* 0x047C */ be<s16> m448;
    /* 0x047E */ u8 _047E[0x2];
    /* 0x0480 */ cXyz m44C;
    /* 0x048C */ cXyz m458;
    /* 0x0498 */ u8 _0498[0x4];
    /* 0x049C */ be<f32> m468;
    /* 0x04A0 */ be<f32> m46C;
    /* 0x04A4 */ be<f32> m470;
    /* 0x04A8 */ be<f32> m474;
    /* 0x04AC */ be<f32> m478;
    /* 0x04B0 */ be<s32> m47C;
    /* 0x04B4 */ be<s16> m480;
    /* 0x04B6 */ be<s16> m482;
    /* 0x04B8 */ be<s16> m484;
    /* 0x04BA */ be<s16> m486;
    /* 0x04BC */ be<s16> m488;
    /* 0x04BE */ be<s16> m48A;
    /* 0x04C0 */ csXyz m48C;
    /* 0x04C6 */ be<s16> m492;
    /* 0x04C8 */ be<s16> m494;
    /* 0x04CA */ be<s16> m496;
    /* 0x04CC */ be<s16> m498;
    /* 0x04CE */ be<s16> m49A;
    /* 0x04D0 */ be<s16> m49C;
    /* 0x04D2 */ be<s16> m49E;
    /* 0x04D4 */ be<s16> m4A0;
    /* 0x04D6 */ be<s16> m4A2;
    /* 0x04D8 */ be<s16> m4A4;
    /* 0x04DA */ be<s16> m4A6;
    /* 0x04DC */ be<s16> m4A8;
    /* 0x04DE */ be<s16> m4AA;
    /* 0x04E0 */ be<s16> m4AC;
    /* 0x04E2 */ be<s16> m4AE;
    /* 0x04E4 */ be<s16> m4B0;
    /* 0x04E6 */ be<s16> m4B2;
    /* 0x04E8 */ be<s16> m4B4;
    /* 0x04EA */ be<s16> m4B6;
    /* 0x04EC */ be<s16> m4B8;
    /* 0x04EE */ be<s16> m4BA;
    /* 0x04F0 */ be<s16> m4BC;
    /* 0x04F2 */ be<s16> m4BE;
    /* 0x04F4 */ be<s16> m4C0;
    /* 0x04F6 */ u8 _04F6[0x6];
    /* 0x04FC */ be<s16> m4C8[3];
    /* 0x0502 */ be<s16> mInvincibleTimer;
    /* 0x0504 */ be<s16> m4D0;
    /* 0x0506 */ u8 _0506[0x2];
    /* 0x0508 */ be<f32> m4D4;
    /* 0x050C */ u8 _050C[0x4];
    /* 0x0510 */ dBgS_AcchCir mAcchCir;
    /* 0x0550 */ dBgS_ObjAcch mAcch;
    /* 0x0714 */ be<s32> m6E0;
    /* 0x0718 */ be<f32> mSpawnY;
    /* 0x071C */ cXyz m6E8;
    /* 0x0728 */ cXyz m6F4[2];
    /* 0x0740 */ be<u8> m70C;
    /* 0x0741 */ be<u8> m70D;
    /* 0x0742 */ be<u8> m70E;
    /* 0x0743 */ u8 _0743[0x1];
    /* 0x0744 */ be<u8> m710;
    /* 0x0745 */ be<u8> m711;
    /* 0x0746 */ be<u8> m712;
    /* 0x0747 */ be<u8> m713;
    /* 0x0748 */ gptr<fopAc_ac_c> m714;
    /* 0x074C */ be<s16> m718;
    /* 0x074E */ be<s16> m71A;
    /* 0x0750 */ u8 _0750[0x2];
    /* 0x0752 */ be<s16> m71E;
    /* 0x0754 */ be<f32> mMaxFallDistance;
    /* 0x0758 */ dCcD_Stts mStts;
    /* 0x0794 */ cXyz mParticlePos;
    /* 0x07A0 */ csXyz mParticleAngle;
    /* 0x07A6 */ u8 _07A6[0x2];
    /* 0x07A8 */ dPa_smokeEcallBack_l mParticleCallBack;
    /* 0x07C8 */ be<s16> m794;
    /* 0x07CA */ u8 _07CA[0x2];
    /* 0x07CC */ be<f32> m798;
    /* 0x07D0 */ gptr<cXyz> m79C;
    /* 0x07D4 */ cXyz m7A0;
    /* 0x07E0 */ csXyz m7AC;
    /* 0x07E6 */ be<s16> m7B2;
    /* 0x07E8 */ be<s16> m7B4;
    /* 0x07EA */ be<u8> m7B6;
    /* 0x07EB */ u8 _07EB[0x1];
    /* 0x07EC */ be<u32> m7B8;
};
WWHD_SIZE(damagereaction_l, 0x7F0);
struct enemyice_l {
    /* 0x0000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x0004 */ be<s16> mFreezeDuration;
    /* 0x0006 */ be<s8> mLightShrinkTimer;
    /* 0x0007 */ u8 _0007[0x1];
    /* 0x0008 */ be<f32> mYOffset;
    /* 0x000C */ be<s8> m00C;
    /* 0x000D */ be<s8> mMode;
    /* 0x000E */ be<s16> mFreezeTimer;
    /* 0x0010 */ be<s16> mMoveDelayTimer;
    /* 0x0012 */ be<s16> mAngleY;
    /* 0x0014 */ be<s16> mAngularVelY;
    /* 0x0016 */ u8 _0016[0x2];
    /* 0x0018 */ cXyz mSpeed;
    /* 0x0024 */ be<f32> mSpeedF;
    /* 0x0028 */ be<f32> m028;
    /* 0x002C */ be<f32> m02C;
    /* 0x0030 */ dCcD_Stts mStts;
    /* 0x006C */ dCcD_Cyl mCyl;
    /* 0x019C */ be<f32> mCylHeight;
    /* 0x01A0 */ be<f32> mWallRadius;
    /* 0x01A4 */ be<f32> mScaleXZ;
    /* 0x01A8 */ be<f32> mScaleY;
    /* 0x01AC */ be<f32> mParticleScale;
    /* 0x01B0 */ be<u8> m1B0;
    /* 0x01B1 */ be<u8> mDeathSwitch;
    /* 0x01B2 */ u8 _01B2[0x2];
    /* 0x01B4 */ dBgS_AcchCir mBgAcchCir;
    /* 0x01F4 */ dBgS_ObjAcch mBgAcch;
};
WWHD_SIZE(enemyice_l, 0x3B8);
struct enemyfire_l {
    /* 0x0000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x0004 */ be<s16> mFireDuration;
    /* 0x0006 */ be<s8> mMode;
    /* 0x0007 */ u8 _0007[0x1];
    /* 0x0008 */ be<s16> mFireTimer;
    /* 0x000A */ u8 _000A[0x2];
    /* 0x000C */ gptr<mDoExt_McaMorf> mpMcaMorf;
    /* 0x0010 */ be<s8> mFlameJntIdxs[10];
    /* 0x001A */ u8 _001A[0x2];
    /* 0x001C */ be<f32> mParticleScale[10];
    /* 0x0044 */ be<s16> mFlameTimers[10];
    /* 0x0058 */ gptr<JPABaseEmitter> mpFlameEmitters[10];
    /* 0x0080 */ cXyz mPrevPos;
    /* 0x008C */ cXyz mDirection;
    /* 0x0098 */ be<f32> mFlameScaleY;
    /* 0x009C */ u8 _009C[0x1];
    /* 0x009D */ be<u8> mHitboxFlameIdx;
    /* 0x009E */ u8 _009E[0x2];
    /* 0x00A0 */ dCcD_Stts mStts;
    /* 0x00DC */ dCcD_Sph mSph;
    /* 0x0208 */ LIGHT_INFLUENCE_l mLight;
};
WWHD_SIZE(enemyfire_l, 0x22C);

struct mo2_class : fopEn_enemy_c {
    /* 0x03C8 */ request_of_phase_process_class mPhsMo2;
    /* 0x03D0 */ request_of_phase_process_class mPhsSpear;
    /* 0x03D8 */ be<u8> mMode;
    /* 0x03D9 */ be<u8> mFrozenInTimePose;
    /* 0x03DA */ be<u8> mPathIndex;
    /* 0x03DB */ be<u8> mEnableSpawnSwitch;
    /* 0x03DC */ be<u8> mDeathSwitch;
    /* 0x03DD */ be<u8> m02C1;
    /* 0x03DE */ u8 _03DE[0x2];
    /* 0x03E0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x03E4 */ mDoExt_btpAnm_l m02C8;
    /* 0x0458 */ be<u8> m02DC;
    /* 0x0459 */ be<u8> m02DD;
    /* 0x045A */ be<u8> m02DE;
    /* 0x045B */ u8 _045B[0x1];
    /* 0x045C */ be<s16> m02E0;
    /* 0x045E */ be<s16> m02E2;
    /* 0x0460 */ be<u32> m02E8;
    /* 0x0464 */ u8 _0464[0x284];
    /* 0x06E8 */ mDoExt_3DlineMat0_l m0570;
    /* 0x0830 */ gptr<J3DModel> m058C;
    /* 0x0834 */ be<f32> m0590;
    /* 0x0838 */ be<s32> m0594;
    /* 0x083C */ be<s32> m0598;
    /* 0x0840 */ be<s32> m059C;
    /* 0x0844 */ be<s32> m05A0;
    /* 0x0848 */ be<s16> m05A4[5];
    /* 0x0852 */ be<s16> m05AE;
    /* 0x0854 */ be<s16> m05B0;
    /* 0x0856 */ be<s16> m05B2;
    /* 0x0858 */ be<s16> m05B4;
    /* 0x085A */ be<s16> m05B6;
    /* 0x085C */ u8 _085C[0x2];
    /* 0x085E */ be<s16> m05BA;
    /* 0x0860 */ be<s16> m05BC;
    /* 0x0862 */ be<u8> m05BE;
    /* 0x0863 */ be<u8> m05BF;
    /* 0x0864 */ be<f32> m05C0;
    /* 0x0868 */ be<f32> m05C0_dy_hd; /* HD: |dy| to the target (Mo2_move) */
    /* 0x086C */ cXyz m05C4;
    /* 0x0878 */ u8 m05D0[4];
    /* 0x087C */ be<s16> m05D4;
    /* 0x087E */ be<s16> m05D6;
    /* 0x0880 */ be<s16> m05D8;
    /* 0x0882 */ be<s16> m05DA;
    /* 0x0884 */ cXyz m05DC;
    /* 0x0890 */ csXyz m05E8;
    /* 0x0896 */ be<s16> m05EE;
    /* 0x0898 */ be<s16> m05F0;
    /* 0x089A */ be<u8> m05F2;
    /* 0x089B */ be<u8> m05F3;
    /* 0x089C */ dPa_smokeEcallBack_l m05F4;
    /* 0x08BC */ damagereaction_l mDamageReaction;
    /* 0x10AC */ u8 _10AC[0x4];
    /* 0x10B0 */ be<s8> mbHasInnateWeapon;
    /* 0x10B1 */ be<s8> mSpawnWeaponActor;
    /* 0x10B2 */ be<s8> mSpawnWeaponActorMode;
    /* 0x10B3 */ be<s8> mbThrowWeapon;
    /* 0x10B4 */ ke_s m0DD8[16];
    /* 0x22F4 */ mDoExt_3DlineMat0_l m3Dline;
    /* 0x243C */ cXyz m2034;
    /* 0x2448 */ cXyz m2040[2];
    /* 0x2460 */ u8 _2460[0x8];
    /* 0x2468 */ be<s32> m2060;
    /* 0x246C */ be<s32> m2064;
    /* 0x2470 */ be<f32> m2068;
    /* 0x2474 */ be<f32> m206C;
    /* 0x2478 */ be<f32> m2070;
    /* 0x247C */ be<f32> m2074;
    /* 0x2480 */ be<f32> m2078;
    /* 0x2484 */ be<s16> m207C;
    /* 0x2486 */ be<s8> m207E;
    /* 0x2487 */ u8 _2487[0xD];
    /* 0x2494 */ dCcD_Cyl mCoCyl;
    /* 0x25C4 */ dCcD_Cyl mTgCyl;
    /* 0x26F4 */ dCcD_Sph mHeadSph;
    /* 0x2820 */ dCcD_Sph mDefenseSph;
    /* 0x294C */ dCcD_Sph mWeaponSph;
    /* 0x2A78 */ dCcD_Sph mWeapon2Sph;
    /* 0x2BA4 */ dCcD_Sph m279C;
    /* 0x2CD0 */ cXyz m28C8;
    /* 0x2CDC */ cXyz m28D4;
    /* 0x2CE8 */ cXyz m28E0;
    /* 0x2CF4 */ cXyz m28EC;
    /* 0x2D00 */ cXyz m28F8;
    /* 0x2D0C */ cXyz m2904;
    /* 0x2D18 */ cXyz m2910;
    /* 0x2D24 */ cXyz m291C;
    /* 0x2D30 */ cXyz m2928;
    /* 0x2D3C */ cXyz m2934;
    /* 0x2D48 */ be<u8> m2940;
    /* 0x2D49 */ be<u8> m2941;
    /* 0x2D4A */ be<u8> m2942;
    /* 0x2D4B */ be<u8> m2943;
    /* 0x2D4C */ be<s16> m2944;
    /* 0x2D4E */ be<s16> m2946;
    /* 0x2D50 */ be<s16> m2948;
    /* 0x2D52 */ be<s16> m294A;
    /* 0x2D54 */ be<s16> m294C;
    /* 0x2D56 */ be<s16> m294E;
    /* 0x2D58 */ be<u8> mParryOpeningType;
    /* 0x2D59 */ be<s8> m2951;
    /* 0x2D5A */ be<s16> m2952;
    /* 0x2D5C */ be<u32> m2954;
    /* 0x2D60 */ be<u32> mWeaponPcId;
    /* 0x2D64 */ gptr<fopAc_ac_c> mpBomb;
    /* 0x2D68 */ be<u16> m2960;
    /* 0x2D6A */ u8 _2D6A[0x2];
    /* 0x2D6C */ be<s32> m2964;
    /* 0x2D70 */ be<s8> m2968;
    /* 0x2D71 */ be<s8> m2969;
    /* 0x2D72 */ be<s8> mHasPath;
    /* 0x2D73 */ u8 _2D73[0x1];
    /* 0x2D74 */ gptr<dPath> ppd;
    /* 0x2D78 */ be<s8> m2970;
    /* 0x2D79 */ u8 _2D79[0x3];
    /* 0x2D7C */ ga_s m2978[3];
    /* 0x2E0C */ be<s8> m2A08;
    /* 0x2E0D */ be<u8> m2A09;
    /* 0x2E0E */ be<u8> m2A0A;
    /* 0x2E0F */ be<s8> m2A0B;
    /* 0x2E10 */ be<s16> m2A0C;
    /* 0x2E12 */ u8 _2E12[0x2];
    /* 0x2E14 */ cXyz m2A10;
    /* 0x2E20 */ be<s8> m2A1C;
    /* 0x2E21 */ be<s8> m2A1D;
    /* 0x2E22 */ be<s16> m2A1E;
    /* 0x2E24 */ cXyz m2A20;
    /* 0x2E30 */ cXyz m2A2C;
    /* 0x2E3C */ be<f32> m2A38;
    /* 0x2E40 */ be<f32> m2A3C;
    /* 0x2E44 */ be<f32> m2A40;
    /* 0x2E48 */ be<f32> m2A44;
    /* 0x2E4C */ be<s16> m2A48;
    /* 0x2E4E */ be<u8> m2A4A;
    /* 0x2E4F */ be<u8> m2A4B;
    /* 0x2E50 */ be<s16> m2A4C;
    /* 0x2E52 */ u8 _2E52[0x2];
    /* 0x2E54 */ enemyice_l mEnemyIce;
    /* 0x320C */ enemyfire_l mEnemyFire;
    /* 0x3438 */ gptr<JntHit_c> mpJntHit;
};
WWHD_SIZE(mo2_class, 0x343C);
WWHD_OFFSET(mo2_class, m02DC, 0x458);
WWHD_OFFSET(mo2_class, m05B0, 0x854);
WWHD_OFFSET(mo2_class, m05D4, 0x87C);
WWHD_OFFSET(mo2_class, m05C4, 0x86C);
WWHD_OFFSET(mo2_class, mDamageReaction, 0x8BC);
WWHD_OFFSET(mo2_class, mbHasInnateWeapon, 0x10B0);
WWHD_OFFSET(mo2_class, mCoCyl, 0x2494);
WWHD_OFFSET(mo2_class, m2952, 0x2D5A);
WWHD_OFFSET(mo2_class, m2A08, 0x2E0C);
WWHD_OFFSET(mo2_class, mEnemyIce, 0x2E54);
WWHD_OFFSET(mo2_class, mpJntHit, 0x3438);

/* mo2HIO_c: GameCube offsets - 4 (GHS: vtable pointer after the members) */
struct mo2HIO_c {
    /* 0x0000 */ be<s8> mNo;
    /* 0x0001 */ be<u8> m005;
    /* 0x0002 */ be<u8> m006;
    /* 0x0003 */ be<u8> m007;
    /* 0x0004 */ be<u8> m008;
    /* 0x0005 */ be<u8> m009;
    /* 0x0006 */ be<s16> m00A;
    /* 0x0008 */ be<s16> m00C;
    /* 0x000A */ u8 _000A[0x2];
    /* 0x000C */ be<f32> m010;
    /* 0x0010 */ be<f32> m014;
    /* 0x0014 */ be<f32> m018;
    /* 0x0018 */ be<f32> m01C;
    /* 0x001C */ be<s16> m020;
    /* 0x001E */ be<s16> m022;
    /* 0x0020 */ be<s16> m024;
    /* 0x0022 */ u8 _0022[0x2];
    /* 0x0024 */ be<f32> m028;
    /* 0x0028 */ be<f32> m02C;
    /* 0x002C */ be<f32> m030;
    /* 0x0030 */ be<f32> m034;
    /* 0x0034 */ be<s16> m038;
    /* 0x0036 */ u8 _0036[0x2];
    /* 0x0038 */ be<f32> m03C;
    /* 0x003C */ cXyz m040;
    /* 0x0048 */ be<f32> m04C;
    /* 0x004C */ be<f32> m050;
    /* 0x0050 */ be<f32> m054;
    /* 0x0054 */ be<f32> m058;
    /* 0x0058 */ be<f32> m05C;
    /* 0x005C */ be<f32> m060;
    /* 0x0060 */ be<f32> m064;
    /* 0x0064 */ be<f32> m068;
    /* 0x0068 */ be<f32> m06C;
    /* 0x006C */ be<f32> m070;
    /* 0x0070 */ be<s16> m074;
    /* 0x0072 */ u8 _0072[0x2];
    /* 0x0074 */ be<f32> m078;
    /* 0x0078 */ be<f32> m07C;
    /* 0x007C */ be<f32> m080;
    /* 0x0080 */ be<f32> m084;
    /* 0x0084 */ be<s16> m088;
    /* 0x0086 */ be<s16> m08A;
    /* 0x0088 */ be<f32> m08C;
    /* 0x008C */ be<f32> m090;
    /* 0x0090 */ be<f32> m094;
    /* 0x0094 */ be<f32> m098;
    /* 0x0098 */ be<f32> m09C;
    /* 0x009C */ be<f32> m0A0;
    /* 0x00A0 */ be<f32> m0A4;
    /* 0x00A4 */ be<f32> m0A8;
    /* 0x00A8 */ be<f32> m0AC;
    /* 0x00AC */ be<f32> m0B0;
    /* 0x00B0 */ be<f32> m0B4;
    /* 0x00B4 */ be<f32> m0B8;
    /* 0x00B8 */ be<f32> m0BC;
    /* 0x00BC */ be<f32> m0C0;
    /* 0x00C0 */ be<f32> m0C4;
    /* 0x00C4 */ be<f32> m0C8;
    /* 0x00C8 */ be<s16> m0CC;
    /* 0x00CA */ be<s16> m0CE;
    /* 0x00CC */ be<s16> m0D0;
    /* 0x00CE */ be<s16> m0D2;
    /* 0x00D0 */ be<s16> m0D4;
    /* 0x00D2 */ be<s16> m0D6;
    /* 0x00D4 */ be<s16> m0D8;
    /* 0x00D6 */ be<s16> m0DA;
    /* 0x00D8 */ be<s16> m0DC;
    /* 0x00DA */ be<s16> m0DE;
    /* 0x00DC */ be<f32> m0E0[10];
    /* 0x0104 */ be<s16> m108;
    /* 0x0106 */ be<s16> m10A;
    /* 0x0108 */ be<s16> m10C;
    /* 0x010A */ be<s16> m10E;
    /* 0x010C */ be<f32> m110[10];
    /* 0x0134 */ be<f32> m138;
    /* 0x0138 */ be<f32> m13C;
    /* 0x013C */ be<f32> m140;
    /* 0x0140 */ be<s16> m144;
    /* 0x0142 */ be<s16> m146;
    /* 0x0144 */ be<s16> m148;
    /* 0x0146 */ be<s16> m14A;
    /* 0x0148 */ be<s16> m14C;
    /* 0x014A */ be<s16> m14E;
    /* 0x014C */ be<s16> m150;
    /* 0x014E */ be<s16> m152;
    /* 0x0150 */ be<s16> m154;
    /* 0x0152 */ be<s16> m156;
    /* 0x0154 */ be<s16> m158;
    /* 0x0156 */ be<s16> m15A;
    /* 0x0158 */ JntHit_HIO_l mJntHitHIO;
    /* 0x0184 */ be<u32> __vtbl;
};
WWHD_SIZE(mo2HIO_c, 0x188);

/* resource indices and joints (GameCube res/Object/Mo2.h; the WWHD code uses the same numbers) */
enum dRes_INDEX_MO2 {
    dRes_INDEX_MO2_BAS_ADOTSUKI_e = 0x8,
    dRes_INDEX_MO2_BAS_AHAZUSHI01_e = 0x9,
    dRes_INDEX_MO2_BAS_AHAZUSHI02_e = 0xA,
    dRes_INDEX_MO2_BAS_AHAZUSHI03_e = 0xB,
    dRes_INDEX_MO2_BAS_AKAITEN01_e = 0xC,
    dRes_INDEX_MO2_BAS_AKAITEN02_e = 0xD,
    dRes_INDEX_MO2_BAS_AKAITEN03_e = 0xE,
    dRes_INDEX_MO2_BAS_ATATE01_e = 0xF,
    dRes_INDEX_MO2_BAS_ATATE02_e = 0x10,
    dRes_INDEX_MO2_BAS_ATATE03_e = 0x11,
    dRes_INDEX_MO2_BAS_ATSUKI01_e = 0x12,
    dRes_INDEX_MO2_BAS_ATSUKI02_e = 0x13,
    dRes_INDEX_MO2_BAS_ATSUKI03_e = 0x14,
    dRes_INDEX_MO2_BAS_BB_FLY_e = 0x15,
    dRes_INDEX_MO2_BAS_BKYORO_e = 0x16,
    dRes_INDEX_MO2_BAS_BWAIT_e = 0x17,
    dRes_INDEX_MO2_BAS_BWALKFB_e = 0x18,
    dRes_INDEX_MO2_BAS_BWALKLR_e = 0x19,
    dRes_INDEX_MO2_BAS_DASH_e = 0x1A,
    dRes_INDEX_MO2_BAS_DEFAULT_e = 0x1B,
    dRes_INDEX_MO2_BAS_HIPDMG01_e = 0x1C,
    dRes_INDEX_MO2_BAS_HIPDMG02_e = 0x1D,
    dRes_INDEX_MO2_BAS_JEND_e = 0x1E,
    dRes_INDEX_MO2_BAS_JSTART_e = 0x1F,
    dRes_INDEX_MO2_BAS_KKEIKAI_e = 0x20,
    dRes_INDEX_MO2_BAS_KNAGE_e = 0x21,
    dRes_INDEX_MO2_BAS_KOKERUB_e = 0x22,
    dRes_INDEX_MO2_BAS_KOKERUF_e = 0x23,
    dRes_INDEX_MO2_BAS_KWAIT_e = 0x24,
    dRes_INDEX_MO2_BAS_KWALK_e = 0x25,
    dRes_INDEX_MO2_BAS_NABIGPUNCH01_e = 0x26,
    dRes_INDEX_MO2_BAS_NABIGPUNCH02_e = 0x27,
    dRes_INDEX_MO2_BAS_NABIGPUNCH03_e = 0x28,
    dRes_INDEX_MO2_BAS_NAJAB_e = 0x29,
    dRes_INDEX_MO2_BAS_NBWAIT_e = 0x2A,
    dRes_INDEX_MO2_BAS_NBWALKFB_e = 0x2B,
    dRes_INDEX_MO2_BAS_NBWALKLR_e = 0x2C,
    dRes_INDEX_MO2_BAS_NWALK_e = 0x2D,
    dRes_INDEX_MO2_BAS_NYWALK_e = 0x2E,
    dRes_INDEX_MO2_BAS_OKIRUA_e = 0x2F,
    dRes_INDEX_MO2_BAS_OKIRUU_e = 0x30,
    dRes_INDEX_MO2_BAS_PAOMUKE_e = 0x31,
    dRes_INDEX_MO2_BAS_PUTSUBUSE_e = 0x32,
    dRes_INDEX_MO2_BAS_SCATCH_e = 0x33,
    dRes_INDEX_MO2_BAS_SHAKKEN_e = 0x34,
    dRes_INDEX_MO2_BAS_SKYORO_e = 0x35,
    dRes_INDEX_MO2_BAS_SWALK_e = 0x36,
    dRes_INDEX_MO2_BAS_TALK_e = 0x37,
    dRes_INDEX_MO2_BAS_WAIT_e = 0x38,
    dRes_INDEX_MO2_BAS_WALK_e = 0x39,
    dRes_INDEX_MO2_BAS_YOROKEB_e = 0x3A,
    dRes_INDEX_MO2_BAS_YOROKEF_e = 0x3B,
    dRes_INDEX_MO2_BCK_ADOTSUKI_e = 0x3E,
    dRes_INDEX_MO2_BCK_AHAZUSHI01_e = 0x3F,
    dRes_INDEX_MO2_BCK_AHAZUSHI02_e = 0x40,
    dRes_INDEX_MO2_BCK_AHAZUSHI03_e = 0x41,
    dRes_INDEX_MO2_BCK_AKAITEN01_e = 0x42,
    dRes_INDEX_MO2_BCK_AKAITEN02_e = 0x43,
    dRes_INDEX_MO2_BCK_AKAITEN03_e = 0x44,
    dRes_INDEX_MO2_BCK_ATATE01_e = 0x45,
    dRes_INDEX_MO2_BCK_ATATE02_e = 0x46,
    dRes_INDEX_MO2_BCK_ATATE03_e = 0x47,
    dRes_INDEX_MO2_BCK_ATSUKI01_e = 0x48,
    dRes_INDEX_MO2_BCK_ATSUKI02_e = 0x49,
    dRes_INDEX_MO2_BCK_ATSUKI03_e = 0x4A,
    dRes_INDEX_MO2_BCK_AWATEDEMO_e = 0x4B,
    dRes_INDEX_MO2_BCK_BB_FLY_e = 0x4C,
    dRes_INDEX_MO2_BCK_BKYORO_e = 0x4D,
    dRes_INDEX_MO2_BCK_BWAIT_e = 0x4E,
    dRes_INDEX_MO2_BCK_BWALKFB_e = 0x4F,
    dRes_INDEX_MO2_BCK_BWALKLR_e = 0x50,
    dRes_INDEX_MO2_BCK_DASH_e = 0x51,
    dRes_INDEX_MO2_BCK_DEFAULT_e = 0x52,
    dRes_INDEX_MO2_BCK_GAKEDEMO_e = 0x53,
    dRes_INDEX_MO2_BCK_HIPDMG01_e = 0x54,
    dRes_INDEX_MO2_BCK_HIPDMG02_e = 0x55,
    dRes_INDEX_MO2_BCK_JEND_e = 0x56,
    dRes_INDEX_MO2_BCK_JSTART_e = 0x57,
    dRes_INDEX_MO2_BCK_KKEIKAI_e = 0x58,
    dRes_INDEX_MO2_BCK_KNAGE_e = 0x59,
    dRes_INDEX_MO2_BCK_KOKERUB_e = 0x5A,
    dRes_INDEX_MO2_BCK_KOKERUF_e = 0x5B,
    dRes_INDEX_MO2_BCK_KWAIT_e = 0x5C,
    dRes_INDEX_MO2_BCK_KWALK_e = 0x5D,
    dRes_INDEX_MO2_BCK_NABIGPUNCH01_e = 0x5E,
    dRes_INDEX_MO2_BCK_NABIGPUNCH02_e = 0x5F,
    dRes_INDEX_MO2_BCK_NABIGPUNCH03_e = 0x60,
    dRes_INDEX_MO2_BCK_NAJAB_e = 0x61,
    dRes_INDEX_MO2_BCK_NBWAIT_e = 0x62,
    dRes_INDEX_MO2_BCK_NBWALKFB_e = 0x63,
    dRes_INDEX_MO2_BCK_NBWALKLR_e = 0x64,
    dRes_INDEX_MO2_BCK_NWALK_e = 0x65,
    dRes_INDEX_MO2_BCK_NYWALK_e = 0x66,
    dRes_INDEX_MO2_BCK_OKIRUA_e = 0x67,
    dRes_INDEX_MO2_BCK_OKIRUB_e = 0x68,
    dRes_INDEX_MO2_BCK_OKIRUU_e = 0x69,
    dRes_INDEX_MO2_BCK_PAOMUKE_e = 0x6A,
    dRes_INDEX_MO2_BCK_PUTSUBUSE_e = 0x6B,
    dRes_INDEX_MO2_BCK_SCATCH_e = 0x6C,
    dRes_INDEX_MO2_BCK_SHAKKEN_e = 0x6D,
    dRes_INDEX_MO2_BCK_SKYORO_e = 0x6E,
    dRes_INDEX_MO2_BCK_SLIPDEMO_e = 0x6F,
    dRes_INDEX_MO2_BCK_SWALK_e = 0x70,
    dRes_INDEX_MO2_BCK_TACKLEDEMO_e = 0x71,
    dRes_INDEX_MO2_BCK_UKYADEMO_e = 0x72,
    dRes_INDEX_MO2_BCK_WAIT_e = 0x73,
    dRes_INDEX_MO2_BCK_WAITDEMO_e = 0x74,
    dRes_INDEX_MO2_BCK_WALK_e = 0x75,
    dRes_INDEX_MO2_BDL_MO_e = 0x78,
    dRes_INDEX_MO2_BMD_KB_e = 0x7B,
    dRes_INDEX_MO2_BMT_MO_BLUR_e = 0x7E,
    dRes_INDEX_MO2_BMT_MO_GREEN_e = 0x7F,
    dRes_INDEX_MO2_BTP_TCLOSE_e = 0x82,
    dRes_INDEX_MO2_BTP_TDAMAGE_e = 0x83,
    dRes_INDEX_MO2_BTP_THAKKEN_e = 0x84,
    dRes_INDEX_MO2_BTP_TKOUGEKI_e = 0x85,
    dRes_INDEX_MO2_BTP_TMABATAKI_e = 0x86,
    dRes_INDEX_MO2_BTP_TMIHIRAKI_e = 0x87,
    dRes_INDEX_MO2_BTP_TOPEN_e = 0x88,
};
enum MO_JNT {
    MO_JNT_MO_BODY_e = 0x0,
    MO_JNT_CENTER_e = 0x1,
    MO_JNT_HARA_J_e = 0x2,
    MO_JNT_MUNE_J_e = 0x3,
    MO_JNT_ACCELA_J_e = 0x4,
    MO_JNT_ACCELB_J_e = 0x5,
    MO_JNT_DOKURO_J_e = 0x6,
    MO_JNT_ACCERA_J_e = 0x7,
    MO_JNT_ACCERB_J_e = 0x8,
    MO_JNT_KATAL_J_e = 0x9,
    MO_JNT_UDELA_J_e = 0xA,
    MO_JNT_UDELB_J_e = 0xB,
    MO_JNT_TEL_J_e = 0xC,
    MO_JNT_OYUBIL_J_e = 0xD,
    MO_JNT_YUBILA_J_e = 0xE,
    MO_JNT_YUBILB_J_e = 0xF,
    MO_JNT_KATAR_J_e = 0x10,
    MO_JNT_UDERA_J_e = 0x11,
    MO_JNT_UDERB_J_e = 0x12,
    MO_JNT_TER_J_e = 0x13,
    MO_JNT_OYUBIR_J_e = 0x14,
    MO_JNT_YARI_J_e = 0x15,
    MO_JNT_MO_YARI_e = 0x16,
    MO_JNT_YUBIRA_J_e = 0x17,
    MO_JNT_YUBIRB_J_e = 0x18,
    MO_JNT_KUBI_J_e = 0x19,
    MO_JNT_KAO_J_e = 0x1A,
    MO_JNT_AGOUE_J_e = 0x1B,
    MO_JNT_HANA_J_e = 0x1C,
    MO_JNT_JAWA_J_e = 0x1D,
    MO_JNT_JAWB_J_e = 0x1E,
    MO_JNT_JAWC_J_e = 0x1F,
    MO_JNT_TARAKO_J_e = 0x20,
    MO_JNT_MIMILA_J_e = 0x21,
    MO_JNT_MIMILB_J_e = 0x22,
    MO_JNT_MIMIRA_J_e = 0x23,
    MO_JNT_MIMIRB_J_e = 0x24,
    MO_JNT_KOSHI_J_e = 0x25,
    MO_JNT_KOKAL_J_e = 0x26,
    MO_JNT_ASHILA_J_e = 0x27,
    MO_JNT_ASHILB_J_e = 0x28,
    MO_JNT_ASHILC_J_e = 0x29,
    MO_JNT_HIZL_J_e = 0x2A,
    MO_JNT_KOKAR_J_e = 0x2B,
    MO_JNT_ASHIRA_J_e = 0x2C,
    MO_JNT_ASHIRB_J_e = 0x2D,
    MO_JNT_ASHIRC_J_e = 0x2E,
    MO_JNT_HIZR_J_e = 0x2F,
    MO_JNT_OA_J_e = 0x30,
    MO_JNT_OB_J_e = 0x31,
    MO_JNT_OC_J_e = 0x32,
};

enum Action {
    ACTION_JYUNKAI = 0,
    ACTION_FIGHT_RUN = 4,
    ACTION_FIGHT = 5,
    ACTION_NAGE = 6,
    ACTION_OSHI = 7,
    ACTION_P_LOST = 8,
    ACTION_B_NIGE = 9,
    ACTION_DEFENCE = 10,
    ACTION_HUKKI = 11,
    ACTION_WEPON_SEARCH = 12,
    ACTION_HIP_DAMAGE = 13,
    ACTION_AITE_MIRU = 14,
    ACTION_D_SIT = 17,
    ACTION_D_MAHI = 18,
    ACTION_FAIL = 20,
    ACTION_YOGAN_FAIL = 21,
    ACTION_D_DOZOU = 23,
    ACTION_CARRY = 30,
    ACTION_CARRY_DROP = 31,
    ACTION_E3_DEMO = 40,
};
enum { fpcNm_PLAYER_e = 0xA8, fpcNm_BOMB_e = 0x126, fpcNm_BOKO_e = 0x1CF };

struct Mo2AtInfo {
    /* 0x00 */ gptr<dCcD_GObjInf> mpObj;
    /* 0x04 */ gptr<fopAc_ac_c> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ gptr<cXyz> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(Mo2AtInfo, 0x1C);
void tex_anm_set(mo2_class*, u16);
void anm_init(mo2_class*, s32, f32, u8, f32, s32);
void smoke_set_s(mo2_class*, f32);
void wait_set(mo2_class*);
void path_check(mo2_class*);
void Mo2_move(mo2_class*);
BOOL enemy_ice(enemyice_l*);
void enemy_fire(enemyfire_l*);
s32 damage_reaction(damagereaction_l*);
void dComIfGp_setNextStage(const char*, s16, s8, s8, f32, u32, s32, s8);
void fopAcM_orderPotentialEvent(fopAc_ac_c*, u16, u16, u16);
s32 CPad_CHECK_TRIG_B(s32);
s32 CPad_CHECK_HOLD_Y(s32);
void def_se_set(fopAc_ac_c*, void*, u32);
void at_power_check(Mo2AtInfo*);
fopAc_ac_c* cc_at_check(fopAc_ac_c*, Mo2AtInfo*);
void dKy_Sound_set(cXyz*, s32, u32, s32);
void dCam_getCamera();
u8* dCam_getBody();
void dCamera_Set(u8*, cXyz*, cXyz*, f32, s16);
void dCamera_SetTypeForce(u8*, const char*, fopAc_ac_c*);
void dCamera_SetTrimTypeForce(u8*, s32);
void dCamera_SetTrimSize(u8*, s32);
void dCamera_Stop(u8*);
void dCamera_Start(u8*);
void dMeter_mtrShow();
void dVibration_StartShock(u8*, s32, s32, cXyz*);
void Mo2Exec_endSmoke(dPa_smokeEcallBack_l*);
void Mo2Exec_enemyFireRemove(enemyfire_l*);
void Mo2Exec_monsSeStart(u32, cXyz*, u32, u32, s32);
void mDoAud_seStop(u32, s32);
void Mo2Exec_massSet(void*, void*, u32);
void Mo2Exec_gndConstruct(void*);
void Mo2Exec_gndDestroy(void*, s32);
// HD: aliases to the main Moblin TU's storage; no additional definitions.
inline mo2HIO_c& l_mo2HIO() {
    return *gabi::at<mo2HIO_c>(0x104654E0);
}
inline be<u8>& rouya_mode() {
    return *gabi::at<be<u8>>(0x101BA692);
}
inline be<s8>& camera_mode() {
    return *gabi::at<be<s8>>(0x101BA693);
}
struct Mo2ExecPlayer {
    u8 padding[0x3AC];
    /* 0x3AC */ be<u8> mCutType;
    u8 gap[0x1F];
    /* 0x3CC */ be<f32> mGrabWear;
};
WWHD_OFFSET(Mo2ExecPlayer, mCutType, 0x3AC);
WWHD_OFFSET(Mo2ExecPlayer, mGrabWear, 0x3CC);
WWHD_SIZE(Mo2ExecPlayer, 0x3D0);
struct Mo2ExecBoko : fopAc_ac_c {
    enum { Type_MOBLIN_SPEAR_e = 4 };
    u8 padding[0x43A - sizeof(fopAc_ac_c)];
    /* 0x43A */ be<s16> mRotAngleSpeed;
};
WWHD_OFFSET(Mo2ExecBoko, mRotAngleSpeed, 0x43A);
WWHD_SIZE(Mo2ExecBoko, 0x43C);
struct Mo2ExecCamera {
    u8 padding[0xDC];
    struct {
        cXyz mEye, mCenter;
    } mLookat;
};
WWHD_OFFSET(Mo2ExecCamera, mLookat.mEye, 0xDC);
WWHD_OFFSET(Mo2ExecCamera, mLookat.mCenter, 0xE8);
WWHD_SIZE(Mo2ExecCamera, 0xF4);
struct Mo2ExecEmitter {
    u8 padding[0x34];
    /* 0x34 */ be<f32> mRate;
    u8 gap38[0x20];
    /* 0x58 */ be<f32> mSpread;
    /* 0x5C */ be<s32> mMaxFrame;
    u8 gap60[0x1C];
    /* 0x7C */ be<f32> mVolumeSweep;
    u8 gap80[0x1B8];
    /* 0x238 */ cXyz mGlobalParticleScale;
};
WWHD_OFFSET(Mo2ExecEmitter, mRate, 0x34);
WWHD_OFFSET(Mo2ExecEmitter, mSpread, 0x58);
WWHD_OFFSET(Mo2ExecEmitter, mMaxFrame, 0x5C);
WWHD_OFFSET(Mo2ExecEmitter, mVolumeSweep, 0x7C);
WWHD_OFFSET(Mo2ExecEmitter, mGlobalParticleScale, 0x238);
WWHD_SIZE(Mo2ExecEmitter, 0x244);
struct Mo2ExecGndChk {
    /* 0x00 */ gptr<be<u32>> mpPolyPassChk; /* -> +0x40 */
    /* 0x04 */ gptr<be<u32>> mpGrpPassChk;  /* -> +0x4C */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ u8 _30[0x40 - 0x30];
    /* 0x40 */ be<u32> __vtbl_40;
    /* 0x44 */ be<u8> mPass[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> __vtbl_4C;
    /* 0x50 */ be<u32> mGrp;
    void SetPos(cXyz* p) {
        m_pos.copy(*p);
    }
};
WWHD_SIZE(Mo2ExecGndChk, 0x54);
inline void dBgS_GndChk_ct(Mo2ExecGndChk* c) {
    Mo2Exec_gndConstruct(c); /* cBgS_GndChk::cBgS_GndChk */
    for (int i = 0; i < 7; i++)
        c->mPass[i] = 0;
    c->__vtbl_10 = 0x10014DDC;
    c->__vtbl_4C = 0x10014DFC;
    c->__vtbl_20 = 0x10014DEC;
    c->__vtbl_40 = 0x10014E0C;
    c->mpGrpPassChk = &c->__vtbl_4C;
    c->mpPolyPassChk = &c->__vtbl_40;
    c->mGrp = 1;
}
inline void dBgS_GndChk_dt(Mo2ExecGndChk* c) {
    c->__vtbl_20 = 0x10014DEC;
    c->__vtbl_40 = 0x10014E0C;
    c->__vtbl_4C = 0x10014DCC;
    Mo2Exec_gndDestroy(c, 0); /* cBgS_Chk::~cBgS_Chk */
}
struct Mo2ExecMatrixBlock {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct Mo2ExecModel {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<Mo2ExecMatrixBlock> mpMtxBlock;
    /* 0x30 */ u8 _30[0xB8 - 0x30];
    /* 0xB8 */ be<u32> mUserArea;
};
/* getAnmMtx: HD marks the joint matrices dirty */
inline Mtx34* model_getAnmMtx(Mo2ExecModel* model, s32 jntNo) {
    Mo2ExecMatrixBlock* blk = model->mpMtxBlock;
    blk->mFlags |= 0x10;
    return blk->mpMtx.get() + jntNo;
}

struct Mo2ExecMassManager {
    u8 padding[0x2854];
    u8 mMass;
};
WWHD_OFFSET(Mo2ExecMassManager, mMass, 0x2854);
WWHD_SIZE(Mo2ExecMassManager, 0x2855);
enum { JA_SE_MAJUTOU_ALERM = 0x834 };
WWHD_OFFSET(Mo2ExecMatrixBlock, mFlags, 0x4);
WWHD_OFFSET(Mo2ExecMatrixBlock, mpMtx, 0x10);
WWHD_SIZE(Mo2ExecMatrixBlock, 0x14);
WWHD_OFFSET(Mo2ExecModel, mpMtxBlock, 0x2C);
WWHD_OFFSET(Mo2ExecModel, mUserArea, 0xB8);
WWHD_SIZE(Mo2ExecModel, 0xBC);

enum {
    dEvtFlag_STAFF_ALL_e = 2,
    dEvtType_OTHER_e = 2,
    ID_AK_JN_ELEMENTKIKUZU00 = 0x2B,
    ID_AK_JN_TUBA00 = 0xE
};

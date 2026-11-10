/* Stone tentacle layout.
 * HD: removes m2F2 and m314; adds collider activity and wobble amplitude at the tail. */
#pragma once

#include "d/d_jnt_hit.h"
#include "gc_api/gc_api.h"

enum {
    dRes_INDEX_SSK_BDL_KTANA_00_e = 3,
    dRes_INDEX_SSK_BDL_TURU_02_e = 4,
};
enum {
    JA_SE_OBJ_JAMA_SHOKU_OUT = 0x694B,
    JA_SE_OBJ_JAMA_SHOKU_IN = 0x694D,
    JA_SE_OBJ_SHOKU_LIFT_MOVE = 0x3823,
    JA_SE_LK_SW_HIT_S = 0x2803,
    JA_SE_LK_W_WEP_HIT = 0x2833,
    JA_SE_LK_MS_WEP_HIT = 0x2834,
    JA_SE_LK_HAMMER_HIT = 0x2855,
    ID_IT_SN_TGSYOKU_ROCK00 = 0x816C,
    ID_IT_ST_TGSYOKU_SMOKE00 = 0xA16D,
};

enum SskMoveState {
    SSK_WAIT = 0,
    SSK_ROCK_APPEAR = 1,
    SSK_RISE = 2,
    SSK_RETRACT = 3,
    SSK_ACTIVE = 10,
};
enum {
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
    AT_TYPE_PGANON_SWORD = 0x20000000,
};

struct ssk_class : fopEn_enemy_c {
    enum : u32 { __vt = 0x1003D304, __aabVt = 0x1003D2C4 }; // HD: TU-specific vtables.
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ be<u8> mType;  /* was m2B4: daSsk_Create 0248E770: parameter bits 0-7; 0xFF starts the tentacle fully out (mMoveState SSK_ACTIVE, mKtanaScale 1); nomal_move (in daSsk_Execute 0248DC20): type 1 retracts once the player is farther than mRetractDist */
    /* 0x3D1 */ be<u8> mRadiusPrm;  /* was m2B5: daSsk_Create 0248E770: parameter bits 8-15 (0xFF read as 0); times 10 gives mAppearDist */
    /* 0x3D2 */ be<u8> mSwitchNo;  /* was m2B6: daSsk_Create 0248E770: parameter bits 24-31; daSsk_Create and nomal_move (in daSsk_Execute 0248DC20) test dComIfGs_isSwitch on it (not 0xFF): delete at once / retract, then delete */
    /* 0x3D3 */ be<u8> mMoveSePlayed;  /* was m2B7: nomal_move (in daSsk_Execute 0248DC20): set when JA_SE_OBJ_SHOKU_LIFT_MOVE starts as joint 1's wobble (mJointAngle[1].z) nears 0x1000, cleared when it leaves, so the sound plays once per swing */
    /* 0x3D4 */ be<s16> mAction;  /* was m2B8: daSsk_Execute 0248DC20: switch, case 0 runs nomal_move; daSsk_Create 0248E770: 0 */
    /* 0x3D6 */ be<s16> mMoveState;  /* was m2BA: nomal_move (in daSsk_Execute 0248DC20): the SskMoveState switch (wait, rock appear, rise, retract, active); daSsk_Create 0248E770: SSK_ACTIVE for type 0xFF */
    /* 0x3D8 */ be<s16> mHitTimer;  /* was m2BC: body_atari_check (in daSsk_Execute 0248DC20): 8 on a hit, counted down; no hit is taken while above 0 */
    /* 0x3DA */ u8 _3DA[2];
    /* 0x3DC */ be<f32> mAppearDist;  /* was m2C0: daSsk_Create 0248E770: mRadiusPrm * 10; nomal_move (in daSsk_Execute 0248DC20): the tentacle appears when the player comes closer (SSK_WAIT) */
    /* 0x3E0 */ be<f32> mRetractDist;  /* was m2C4: daSsk_Execute 0248DC20: mAppearDist + 100; nomal_move (in daSsk_Execute): type 1 retracts when the player is farther (SSK_ACTIVE) */
    /* 0x3E4 */ gptr<mDoExt_McaMorf> mpMorf1;
    /* 0x3E8 */ gptr<mDoExt_McaMorf> mpMorf2;
    /* 0x3EC */ dPa_smokeEcallBack mSmokeCb;  /* was m2D0: nomal_move (in daSsk_Execute 0248DC20): smoke callback of the ID_IT_ST_TGSYOKU_SMOKE00 particles; daSsk_Create 0248E770 constructs it, daSsk_Delete 0248E504 removes it; after TWW mSmokeCb (dPa_smokeEcallBack of other actors) */
    /* 0x40C */ be<s16> mWobbleTimer;  /* was m2F0: nomal_move (in daSsk_Execute 0248DC20): random at appear, +1 per frame from SSK_RISE on; phase of the joints' sine wobble (mJointAngle) */
    /* 0x40E */ csXyz mJointAngle[5];  /* was m2F4: nodeCallBack 0248D880: [jntNo] rotates joint jntNo (Y, X, Z) of the tentacle model (mpMorf1); nomal_move (in daSsk_Execute 0248DC20) sets [1..4].y/.z to the wobble; HD: m2F2 removed */
    /* 0x42C */ cXyz mKtanaScale;  /* was m350: draw_sub 0248D9B4: base scale of the ktana_00 model (mpMorf2, useHeapInit 0248E554), drawn by daSsk_Draw 0248DB4C only above 0.01; nomal_move (in daSsk_Execute 0248DC20) grows it to 1 (SSK_ROCK_APPEAR) and shrinks it to 0 (SSK_RETRACT); HD: m314[5] removed */
    /* 0x438 */ u8 m35C[0x470 - 0x438];
    /* 0x470 */ dBgS_AcchCir mAcchCir;
    /* 0x4B0 */ dBgS_ObjAcch mAcch;
    /* 0x674 */ dCcD_Stts mStts;
    /* 0x6B0 */ dCcD_Cyl mCyl;
    /* 0x7E0 */ be<u8> mColliderActive; /* HD: collider active */
    /* 0x7E1 */ u8 _7E1[3];
    /* 0x7E4 */ be<f32> mWobbleAmplitude; /* HD: wobble amplitude */
    /* 0x7E8 */ gptr<JntHit_c> mpJntHit;  /* was m708: useHeapInit 0248E554: sskCreateJntHit(mpMorf1 model, search_data, 3) result (also stored in the enemy's jntHit); after TWW mpJntHit (JntHit_c* of other actors) */
};
WWHD_OFFSET(ssk_class, mType, 0x3D0);
WWHD_OFFSET(ssk_class, mWobbleTimer, 0x40C);
WWHD_OFFSET(ssk_class, mKtanaScale, 0x42C);
WWHD_OFFSET(ssk_class, mSmokeCb, 0x3EC);
WWHD_OFFSET(ssk_class, mAcchCir, 0x470);
WWHD_OFFSET(ssk_class, mAcch, 0x4B0);
WWHD_OFFSET(ssk_class, mColliderActive, 0x7E0);
WWHD_OFFSET(ssk_class, mWobbleAmplitude, 0x7E4);
WWHD_OFFSET(ssk_class, mStts, 0x674);
WWHD_OFFSET(ssk_class, mCyl, 0x6B0);
WWHD_OFFSET(ssk_class, mpJntHit, 0x7E8);
WWHD_SIZE(ssk_class, 0x7EC);

namespace ssk_detail {
struct Joint {
    be<u32> mData;
    be<u16> mJointNo;
};
WWHD_OFFSET(Joint, mJointNo, 4);
WWHD_SIZE(Joint, 6);
// HD: this TU places the smoke-scale guard between the pi pair and header objects.
struct HeaderStatics {
    be<f32> mNegativePi;
    be<f32> mPositivePi;
    be<u32> mSmokeScaleInitialized;
    u8 mFirstObject;
    u8 mSecondObject;
    u8 mPadding[2];
    be<u32> mZero[4];
};
WWHD_OFFSET(HeaderStatics, mSmokeScaleInitialized, 8);
WWHD_OFFSET(HeaderStatics, mFirstObject, 0xC);
WWHD_OFFSET(HeaderStatics, mSecondObject, 0xD);
WWHD_OFFSET(HeaderStatics, mZero, 0x10);
WWHD_SIZE(HeaderStatics, 0x20);
struct RegistrationRecord {
    u8 mData[0xC];
};
WWHD_SIZE(RegistrationRecord, 0xC);
struct CylinderSource {
    u8 mData[0x44];
};
WWHD_SIZE(CylinderSource, 0x44);
} // namespace ssk_detail

#pragma once
#include "gc_api/gc_api.h"
struct SkJointData { u8 mPadding[4]; be<u16> mJntNo; };
struct SkHitObject { u8 mPadding[0x10]; be<u32> mAtType; u32 GetAtType() const { return mAtType; } };
SkJointData* Sk_jointData(J3DNode*);
u32 JntHit_create(J3DModel*, const void*, s16);
void Sk_constructArray(void*, u32, u32, u32);
void Sk_destroyArray(void*, u32, u32, u32, u32);
void fopAcM_SetMin(fopAc_ac_c*, f32, f32, f32);
void fopAcM_SetMax(fopAc_ac_c*, f32, f32, f32);
enum {
 AT_TYPE_SWORD=2, AT_TYPE_MACHETE=0x400, AT_TYPE_UNK800=0x800,
 AT_TYPE_DARKNUT_SWORD=0x4000000, AT_TYPE_MOBLIN_SPEAR=0x10000000, AT_TYPE_PGANON_SWORD=0x20000000,
 AT_TYPE_BOOMERANG=0x40, AT_TYPE_BOKO_STICK=0x80, AT_TYPE_UNK2000=0x2000,
 AT_TYPE_STALFOS_MACE=0x1000000, AT_TYPE_SKULL_HAMMER=0x10000,
 AT_TYPE_FIRE_ARROW=0x4000, AT_TYPE_ICE_ARROW=0x40000, AT_TYPE_LIGHT_ARROW=0x80000,
 AT_TYPE_NORMAL_ARROW=0x100000, AT_TYPE_GRAPPLING_HOOK=0x8000000
};
struct sk_class : fopEn_enemy_c {
    sk_class();
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ be<u8> m2B4;
    /* 0x3D1 */ be<u8> mSwitchNo;  /* was m2B5: daSk_Create 02488D58: parameter bits 24-31; when not 0xFF and the switch is on the actor is not created */
    /* 0x3D2 */ u8 _3D2[2];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ be<u8> mMoveSePlayed;  /* was m2BC: dousa_move (in daSk_Execute 024886D0): set when JA_SE_OBJ_SHOKU_LIFT_MOVE starts as mJointAngle[1].z nears 0x1000, cleared when it leaves (as ssk_class mMoveSePlayed) */
    /* 0x3D9 */ u8 _3D9;
    /* 0x3DA */ be<s16> mSwayTimer;  /* was m2BE: daSk_Create 02488D58: the process id's low byte << 12; daSk_Execute 024886D0: +1 per frame; dousa_move (in daSk_Execute): phase of the joints' sine sway (mJointAngle) */
    /* 0x3DC */ be<s16> mHitTimer;  /* was m2C0: body_atari_check (in daSk_Execute 024886D0): 8 on a hit, counted down; no hit is taken while above 0 (as ssk_class mHitTimer) */
    /* 0x3DE */ csXyz mJointAngle[4];  /* was m2C2: nodeCallBack 024884FC: [jntNo - 1] rotates joint jntNo (Y, X, Z); dousa_move (in daSk_Execute 024886D0) sets the sway into .x / .y; HD: indexed by joint - 1 */
    /* 0x3F6 */ u8 _3F6[2];
    /* 0x3F8 */ cXyz mJointPos[4];  /* was m2DC: nodeCallBack 024884FC: [jntNo - 1] = joint jntNo's point (joint_offsets) after its rotation; daSk_Execute 024886D0: centre of sphere mSph[i]; HD: indexed by joint - 1 */
    /* 0x428 */ dCcD_Stts mStts;
    /* 0x464 */ dCcD_Sph mSph[4];  /* was m348: daSk_Create 02488D58: body_co_sph_src, mStts; daSk_Execute 024886D0: [i] at mJointPos[i] with sphere_radii[i]; body_atari_check (in daSk_Execute) checks their hits; after TWW mSph (dCcD_Sph of other actors); HD: 4 spheres (GameCube 5) */
    /* 0x914 */ be<u32> mpJntHit;  /* was m924: useHeapInit 02488BB8: JntHit_create(mpMorf model, search_data, 3), also stored in the enemy's jntHit; after TWW mpJntHit (JntHit_c* of other actors); JntHit_c* */
};
WWHD_OFFSET(sk_class, mpMorf, 0x3D4);
WWHD_OFFSET(sk_class, mJointAngle, 0x3DE);
WWHD_OFFSET(sk_class, mJointPos, 0x3F8);
WWHD_OFFSET(sk_class, mStts, 0x428);
WWHD_OFFSET(sk_class, mpJntHit, 0x914);
WWHD_SIZE(sk_class, 0x918);


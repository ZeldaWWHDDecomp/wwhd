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
    /* 0x3D1 */ be<u8> m2B5;
    /* 0x3D2 */ u8 _3D2[2];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ be<u8> m2BC;
    /* 0x3D9 */ u8 _3D9;
    /* 0x3DA */ be<s16> m2BE;
    /* 0x3DC */ be<s16> m2C0;
    /* 0x3DE */ csXyz m2C2[4]; /* HD: indexed by joint - 1 */
    /* 0x3F6 */ u8 _3F6[2];
    /* 0x3F8 */ cXyz m2DC[4];  /* HD: indexed by joint - 1 */
    /* 0x428 */ dCcD_Stts mStts;
    /* 0x464 */ dCcD_Sph m348[4]; /* HD: 4 spheres (GameCube 5) */
    /* 0x914 */ be<u32> m924;   /* JntHit_c* */
};
WWHD_OFFSET(sk_class, mpMorf, 0x3D4);
WWHD_OFFSET(sk_class, m2C2, 0x3DE);
WWHD_OFFSET(sk_class, m2DC, 0x3F8);
WWHD_OFFSET(sk_class, mStts, 0x428);
WWHD_OFFSET(sk_class, m924, 0x914);
WWHD_SIZE(sk_class, 0x918);


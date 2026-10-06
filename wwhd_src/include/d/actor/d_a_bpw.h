/* d_a_bpw.h (WWHD): Jalhalla (big Poe boss) layout (see wwhd_src/README.md).
 *
 * HD constructor 020D60C8 allocates 0xE9C bytes (GameCube 0xD80). Every GameCube field is
 * shifted by the fopAc_ac_c delta (+0x11C); confirmed by this unit's loads/stores (embedded
 * effect callbacks, background check, colliders and the invisible model sit at the shifted
 * offsets). The unit's code addresses fields by offset; the names below are the GameCube ones. */
#pragma once
#include "wwhd.h"

struct bpw_class {
    u8 opaque_000[0x3C8];
    u8 mPhase[8];                       // 0x3C8
    be<u32> mpMorf;                     // 0x3D0
    u8 opaque_3D4[4];
    be<u32> mpLightFreezeBrkAnm;        // 0x3D8
    be<u32> mpLanternGlowBrkAnm;        // 0x3DC
    be<u32> mpLightStunBrkAnm;          // 0x3E0
    be<u32> mpCurseStartBrkAnm;         // 0x3E4
    be<u32> mpCurseEndBrkAnm;           // 0x3E8
    be<u32> mpDefaultBrkAnm;            // 0x3EC
    be<f32> m2D4[12];                   // 0x3F0
    be<f32> m304[9];                    // 0x420
    be<f32> m328[9];                    // 0x444
    be<f32> mBodyPos[3];                // 0x468
    be<f32> m358[3];                    // 0x474
    u8 opaque_480[0xC];
    be<f32> m370[3];                    // 0x48C
    be<f32> mChildActorPos[3];          // 0x498
    be<f32> m388;                       // 0x4A4
    be<f32> m38C;                       // 0x4A8
    be<f32> m390;                       // 0x4AC
    be<f32> m394[3];                    // 0x4B0
    be<f32> m3A0[3];                    // 0x4BC
    be<f32> m3AC[3];                    // 0x4C8
    be<f32> m3B8[3];                    // 0x4D4
    be<f32> m3C4[3];                    // 0x4E0
    be<s16> mKanteraDousaRot[3];        // 0x4EC
    be<s16> m3D6[3];                    // 0x4F2
    u8 mType;                           // 0x4F8
    u8 mUnknownParam2;                  // 0x4F9
    u8 mLightState;                     // 0x4FA
    u8 mHitType;                        // 0x4FB
    u8 m3E0;                            // 0x4FC
    u8 m3E1;                            // 0x4FD
    u8 m3E2;                            // 0x4FE
    u8 opaque_4FF;
    u8 m3E4, m3E5, m3E6, m3E7, m3E8, m3E9, m3EA; // 0x500..0x506
    u8 mKankyouHendouState;             // 0x507
    be<f32> m3EC;                       // 0x508
    be<f32> m3F0;                       // 0x50C
    be<s16> m3F4;                       // 0x510
    be<s16> m3F6;                       // 0x512
    be<s32> m3F8;                       // 0x514
    be<u32> m3FC;                       // 0x518
    be<u32> m400;                       // 0x51C
    be<u32> m404;                       // 0x520
    be<s16> m408;                       // 0x524
    u8 opaque_526[2];
    be<f32> m40C[3];                    // 0x528
    be<f32> m418[3];                    // 0x534
    be<f32> m424[3];                    // 0x540
    u8 opaque_54C[0xC];
    be<f32> m43C;                       // 0x558
    be<f32> m440;                       // 0x55C
    be<s16> mBodyAction;                // 0x560
    be<s16> mActionState;               // 0x562
    u8 opaque_564[4];
    be<s16> mAttWaitTimer;              // 0x568
    be<s16> mSomeCountdownTimers[10];   // 0x56A
    be<s16> m462;                       // 0x57E
    be<s16> m464;                       // 0x580
    u8 opaque_582[0xE];
    be<s16> m474, m476, m478, m47A, m47C, m47E, m480, m482, m484; // 0x590..0x5A0
    u8 opaque_5A2[2];
    be<f32> m488, m48C, m490, m494, m498, m49C, m4A0, m4A4, m4A8; // 0x5A4..0x5C4
    be<s32> mChildPoeIds[15];           // 0x5C8
    /* HD: no mShadowId (the ground shadow is gone); LIGHT_INFLUENCE starts at 0x604 and is
     * 0x24 bytes (the constructor stores 1.0f at its +0x20) */
    u8 mLightInfluence[0x24];           // 0x604
    u8 m50C[0x20];                      // 0x628 dPa_smokeEcallBack (HD 0x20)
    u8 m52C[0x14];                      // 0x648 dPa_followEcallBack
    u8 m540[0x14];                      // 0x65C
    u8 m554[0x14];                      // 0x670
    u8 mFire1Dousa_Pa_followEcallBack[0x14];  // 0x684
    u8 mFire1Dousa_Pa_followEcallBack2[0x14]; // 0x698
    u8 m590[0x14];                      // 0x6AC
    u8 mFireDousa2_Pa_followEcallBack[0x14];  // 0x6C0
    u8 mFireDousa2_Pa_followEcallBack2[0x14]; // 0x6D4
    u8 m5CC[0x14];                      // 0x6E8
    u8 m5E0[4][0x20];                   // 0x6FC
    be<f32> mFire1DousaPos[3];          // 0x77C
    be<f32> m66C[3];                    // 0x788
    be<s16> mFire1DousaRot[3];          // 0x794
    be<s16> m67E;                       // 0x79A
    be<s16> m680;                       // 0x79C
    u8 opaque_79E[2];
    be<s16> m684;                       // 0x7A0
    u8 opaque_7A2[2];
    u8 mAcchCir[0x40];                  // 0x7A4
    u8 mAcch[0x1C4];                    // 0x7E4
    u8 mStts[0x3C];                     // 0x9A8
    u8 mBodyCoSph[0x12C];               // 0x9E4
    u8 mBodyAtSph[0x12C];               // 0xB10
    u8 mKanteraCoSph[0x12C];            // 0xC3C
    u8 mDamageBallCoSph[0x12C];         // 0xD68
    u8 mInvisibleModel[8];              // 0xE94
};
WWHD_SIZE(bpw_class, 0xE9C);
WWHD_OFFSET(bpw_class, mpMorf, 0x3D0);
WWHD_OFFSET(bpw_class, m2D4, 0x3F0);
WWHD_OFFSET(bpw_class, m358, 0x474);
WWHD_OFFSET(bpw_class, mKanteraDousaRot, 0x4EC);
WWHD_OFFSET(bpw_class, mType, 0x4F8);
WWHD_OFFSET(bpw_class, m3E4, 0x500);
WWHD_OFFSET(bpw_class, m3EC, 0x508);
WWHD_OFFSET(bpw_class, m40C, 0x528);
WWHD_OFFSET(bpw_class, m43C, 0x558);
WWHD_OFFSET(bpw_class, mAttWaitTimer, 0x568);
WWHD_OFFSET(bpw_class, m474, 0x590);
WWHD_OFFSET(bpw_class, m488, 0x5A4);
WWHD_OFFSET(bpw_class, mChildPoeIds, 0x5C8);
WWHD_OFFSET(bpw_class, mLightInfluence, 0x604);
WWHD_OFFSET(bpw_class, m50C, 0x628);
WWHD_OFFSET(bpw_class, m5CC, 0x6E8);
WWHD_OFFSET(bpw_class, m5E0, 0x6FC);
WWHD_OFFSET(bpw_class, mFire1DousaPos, 0x77C);
WWHD_OFFSET(bpw_class, m684, 0x7A0);
WWHD_OFFSET(bpw_class, mAcchCir, 0x7A4);
WWHD_OFFSET(bpw_class, mAcch, 0x7E4);
WWHD_OFFSET(bpw_class, mStts, 0x9A8);
WWHD_OFFSET(bpw_class, mBodyCoSph, 0x9E4);
WWHD_OFFSET(bpw_class, mDamageBallCoSph, 0xD68);
WWHD_OFFSET(bpw_class, mInvisibleModel, 0xE94);

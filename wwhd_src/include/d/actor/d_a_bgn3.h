// Puppet Ganon, phase three: GameCube names with the verified WWHD layout.
// HD: model packets are 0xB0 bytes, line materials are enlarged, and each body part is 0x19D8.
#pragma once
#include "bindings.h"
#include "sead/ElementAt.h"
#include "d/actor/d_a_bgn_packet.h"

struct bgn3_ke_s {
    /* 0x00000 */ cXyz mPos[5];  /* was m00: ke_move 02090420: the five hair points, each the previous plus a segment toward mPull; copied into the line material's positions; setModels sets the root (tww the same) */
    /* 0x0003C */ cXyz m3C[5];
    /* 0x00078 */ cXyz mPull;  /* was m78: setModels (in move 02091730): (0, mHairPullY, 0) in the hair's frame minus the root; ke_move 02090420 bends each segment toward it (tww the same) */
};
WWHD_OFFSET(bgn3_ke_s, mPos, 0x0);
WWHD_OFFSET(bgn3_ke_s, m3C, 0x3C);
WWHD_OFFSET(bgn3_ke_s, mPull, 0x78);
WWHD_SIZE(bgn3_ke_s, 0x84);

struct Bgn3LineMat0 {
    /* 0x00000 */ u8 _00000[0x130];
    /* 0x00130 */ be<u32> __vtbl;
    /* 0x00134 */ u8 _00134[0x10];
    /* 0x00144 */ be<u32> mpLines;
};
WWHD_OFFSET(Bgn3LineMat0, __vtbl, 0x130);
WWHD_OFFSET(Bgn3LineMat0, mpLines, 0x144);
WWHD_SIZE(Bgn3LineMat0, 0x148);

struct Bgn3LineMat1 {
    /* 0x00000 */ u8 _00000[0x130];
    /* 0x00130 */ be<u32> __vtbl;
    /* 0x00134 */ u8 _00134[0x50];
    /* 0x00184 */ be<u32> mpLines;
};
WWHD_OFFSET(Bgn3LineMat1, __vtbl, 0x130);
WWHD_OFFSET(Bgn3LineMat1, mpLines, 0x184);
WWHD_SIZE(Bgn3LineMat1, 0x188);

struct part_s3 {
    /* 0x00000 */ gptr<J3DModel> mpPart3Model;
    /* 0x00004 */ mDoExt_J3DModelPacketS_l mPart3Packet;  /* was m0004: partCtor 02090D4C / partDtor 0209445C construct and destroy the mDoExt_J3DModelPacketS at +4; daBgn_Draw 0207B2CC (inlined daBgn3_Draw, view bgn_draw::part_s3) gives it the part model and updates it while l_HIO.m00C is set (as bgn part_s mPartPacket); type was u8[0xB0], now the packet type of the view bgn_draw::part_s3 and of bgn part_s mPartPacket */
    /* 0x000B4 */ dKy_tevstr_c mPart3TevStr;
    /* 0x0027C */ be<s16> mPart3FogColorAdd;  /* was m00C8: daBgn_Draw 0207B2CC (inlined daBgn3_Draw, view bgn_draw::part_s3) passes it to bgn_fog_flash while mPart3ArrowHitFlashTimer runs: eased and added to the fog colour of mPart3TevStr (same use as bgn part_s mPartFogColorAdd) */
    /* 0x0027E */ u8 _0027E[0x2];
    /* 0x00280 */ be<f32> mPart3FogStartAdd;  /* was m00CC: daBgn_Draw 0207B2CC (inlined daBgn3_Draw, view bgn_draw::part_s3) passes it to bgn_fog_flash while mPart3ArrowHitFlashTimer runs: eased and added to the fog start of mPart3TevStr (same use as bgn part_s mPartFogStartAdd) */
    /* 0x00284 */ be<s16> mPart3ArrowHitFlashTimer;
    /* 0x00286 */ u8 _00286[0x2];
    /* 0x00288 */ cXyz mPart3Pos;  /* was m00D4: part_control (in move 02091730) sets [0] to mPartRootPos and chains [1..9] behind it (not below the ground floor); setModels (in move) translates the part matrix to it; splash (in move) tests the ground there; daBgn_Draw 0207B2CC (inlined daBgn3_Draw) lights the part at it (as bgn part_s mPartPos) */
    /* 0x00294 */ be<s16> mPart3AngleX;  /* was m00E0: part_control (in move 02091730) sets it to minus the chain pitch toward the next part; setModels (in move) rotates the part matrix by it (X) */
    /* 0x00296 */ be<s16> mPart3AngleY;  /* was m00E2: part_control (in move 02091730) sets it to the chain heading (cM_atan2s) toward the next part; setModels (in move) rotates the part matrix by it (Y) */
    /* 0x00298 */ u8 _00298[0x4];
    /* 0x0029C */ cXyz mPart3Speed;  /* was m00E8: part_control (in move 02091730) adds it (and gravity) to the part position, then stores (new - old position) * mPartStiffness in it */
    /* 0x002A8 */ be<f32> mPart3Scale;  /* was m00F4: part_control (in move 02091730) sets it to size_d[i] * 1.7 and spaces the chain and the ground floor by it; setModels (in move) scales the part matrix, mPart3Sph, the hair lengths and the arrow-hit emitters by it (as bgn part_s mPartScale) */
    /* 0x002AC */ dCcD_Sph mPart3Sph;
    /* 0x003D8 */ u8 _003D8[0xC];
    /* 0x003E4 */ gptr<JPABaseEmitter> mpPart3ArrowHitEmitter1;
    /* 0x003E8 */ gptr<JPABaseEmitter> mpPart3ArrowHitEmitter2;
    /* 0x003EC */ be<s16> mPart3ArrowHitEffectTimer;
    /* 0x003EE */ u8 _003EE[0x2];
    /* 0x003F0 */ bgn3_ke_s mPart3Hairs[40];
    /* 0x01890 */ Bgn3LineMat0 mPart3LineMat;
};
WWHD_OFFSET(part_s3, mpPart3Model, 0x0);
WWHD_OFFSET(part_s3, mPart3Packet, 0x4);
WWHD_OFFSET(part_s3, mPart3TevStr, 0xB4);
WWHD_OFFSET(part_s3, mPart3FogColorAdd, 0x27C);
WWHD_OFFSET(part_s3, mPart3FogStartAdd, 0x280);
WWHD_OFFSET(part_s3, mPart3ArrowHitFlashTimer, 0x284);
WWHD_OFFSET(part_s3, mPart3Pos, 0x288);
WWHD_OFFSET(part_s3, mPart3AngleX, 0x294);
WWHD_OFFSET(part_s3, mPart3AngleY, 0x296);
WWHD_OFFSET(part_s3, mPart3Speed, 0x29C);
WWHD_OFFSET(part_s3, mPart3Scale, 0x2A8);
WWHD_OFFSET(part_s3, mPart3Sph, 0x2AC);
WWHD_OFFSET(part_s3, mpPart3ArrowHitEmitter1, 0x3E4);
WWHD_OFFSET(part_s3, mpPart3ArrowHitEmitter2, 0x3E8);
WWHD_OFFSET(part_s3, mPart3ArrowHitEffectTimer, 0x3EC);
WWHD_OFFSET(part_s3, mPart3Hairs, 0x3F0);
WWHD_OFFSET(part_s3, mPart3LineMat, 0x1890);
WWHD_SIZE(part_s3, 0x19D8);

struct bgn3_class : fopEn_enemy_c {
    /* 0x003C8 */ request_of_phase_process_class mPhase;
    /* 0x003D0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x003D4 */ u8 mHeadPacket[0xB0];  /* was m002B8: useHeapInit 02090984 sets the mDoExt_J3DModelPacketS up after mpMorf (BCK_BGN_HEAD1); daBgn_Draw 0207B2CC (inlined daBgn3_Draw) gives it the mpMorf model and updates it */
    /* 0x00484 */ gptr<J3DModel> mpDekuModel;  /* was m002CC: useHeapInit 02090984 creates it from dRes_INDEX_BGN_BDL_BGN_DEKU1_e; setModels (in move 02091730) places it (scale 2) and centres mDekuSph on it; daBgn_Draw 0207B2CC draws it */
    /* 0x00488 */ u8 mDekuPacket[0xB0];  /* was m002D0: useHeapInit 02090984 sets the mDoExt_J3DModelPacketS up after mpDekuModel; daBgn_Draw 0207B2CC (inlined daBgn3_Draw) gives it the mpDekuModel model and updates it */
    /* 0x00538 */ bgn3_ke_s mHairs[40];
    /* 0x019D8 */ Bgn3LineMat0 mLineMat;
    /* 0x01B20 */ u8 m017A0[0xB0];
    /* 0x01BD0 */ gptr<J3DModel> mpJyakutenCModel;
    /* 0x01BD4 */ gptr<J3DModel> mpJyakutenBModel;
    /* 0x01BD8 */ gptr<J3DModel> mpJyakutenAModel;
    /* 0x01BDC */ gptr<mDoExt_brkAnm> mJyakutenCBrkAnm;
    /* 0x01BE0 */ gptr<mDoExt_brkAnm> mJyakutenBBrkAnm;
    /* 0x01BE4 */ u8 _01BE4[0x4];
    /* 0x01BE8 */ part_s3 mParts[10];
    /* 0x11E58 */ cXyz mPartRootPos;  /* was m0FD7C: part_control (in move 02091730): current.pos + (0, 0, REG - 300) turned by shape_angle; copied to mParts[0] as the start of the part chain */
    /* 0x11E64 */ be<s16> mFrameCounter;  /* was m0FD88: daBgn3_Execute 02090680 adds 1 per frame; move0 / splash / setModels (in move 02091730) use it as the phase of the turn wobble, the ripples and the head / rope shake */
    /* 0x11E66 */ be<s16> mAction;  /* was m0FD8A: move 02091730 switch: 0 move0, 1 mahi, 2 damage, 3 end; damage_check (in move) sets 1 / 2 / 3, daBgn3_Execute 02090680 resets 10 to 0 */
    /* 0x11E68 */ be<s16> mMode;  /* was m0FD8C: the step inside mAction: mahi / damage / end (in move 02091730) switch on it; damage_check sets 0 */
    /* 0x11E6A */ be<s16> mTurnSpeed;  /* was m0FD8E: move0 (in move 02091730) loads it from l_HIO by health (random sign), adds it to current.angle.y or uses it as the turn step; end eases it to 5000 and adds it to shape_angle.y */
    /* 0x11E6C */ be<s16> mChaseTimer;  /* was m0FD90: move0 (in move 02091730): while it runs bgn3 turns toward the bait, the centre or the player instead of circling; set 1 with bait, from l_HIO after mWallHitCount wall hits, 0 on a wall hit; daBgn3_Execute 02090680 sets 180 */
    /* 0x11E6E */ be<s16> mWallHitCount;  /* was m0FD92: move0 (in move 02091730) counts the wall hits and starts mChaseTimer when it reaches the l_HIO threshold for the health, then clears it */
    /* 0x11E70 */ be<s16> mRippleSeTimer;  /* was m0FD94: splash (in move 02091730) plays the ripple sound (0x6A44 / 0x6A45) only at 0 and reloads it with cM_rndF(20) + 20; daBgn3_Execute 02090680 counts it down (same use as bgn_class mRippleSeTimer) */
    /* 0x11E72 */ be<s16> mFogColorAdd;  /* was m0FD96: daBgn_Draw 0207B2CC (inlined daBgn3_Draw) raises it while mArrowHitFlashTimer runs and adds it to the fog colour (same use as bgn_class mFogColorAdd) */
    /* 0x11E74 */ be<f32> mFogStartAdd;  /* was m0FD98: daBgn_Draw 0207B2CC (inlined daBgn3_Draw) moves the fog start by it while mArrowHitFlashTimer runs (same use as bgn_class mFogStartAdd) */
    /* 0x11E78 */ be<s16> mArrowHitFlashTimer;
    /* 0x11E7A */ be<s16> mCoreHitFrame;  /* was m0FD9E: damage_check (in move 02091730) sets 1 on a core hit; setModels counts it up to 100 and starts the arrow-hit effect of part i when it equals fl_check_d[i] (same use as bgn_class mCoreHitFrame) */
    /* 0x11E7C */ gptr<JPABaseEmitter> mpArrowHitEmitter1;
    /* 0x11E80 */ gptr<JPABaseEmitter> mpArrowHitEmitter2;
    /* 0x11E84 */ be<s16> mArrowHitEffectTimer;
    /* 0x11E86 */ be<s16> mTimers[2];  /* was m0FDAA: daBgn3_Execute 02090680 counts it down (through Bgn3TimerView, five halfwords from here); [0] mahi duration (damage_check), [1] wall-hit lockout 20 (move0) */
    /* 0x11E8A */ be<s16> mTurnTimer;  /* was m0FDAE: move0 (in move 02091730) picks a new mTurnSpeed at 0 and reloads it with cM_rndF(50) + 30; counted down as timer [2] of the Bgn3TimerView sweep in daBgn3_Execute 02090680 */
    /* 0x11E8C */ be<s16> mCenterTimer;  /* was m0FDB0: daBgn3_Execute 02090680 sets 180; while it runs move0 (in move 02091730) turns toward the arena centre (centerAngle); counted down as timer [3] of the Bgn3TimerView sweep */
    /* 0x11E8E */ u8 _11E8E[0x2];
    /* 0x11E90 */ be<s16> mDamageTimer;  /* was m0FDB4: damage_check (in move 02091730) returns while it runs and sets 20 (10000 on the last hit); daBgn3_Execute 02090680 counts it down */
    /* 0x11E92 */ be<s16> mHitSeTimer;  /* was m0FDB6: damage_check (in move 02091730) plays the hit sound only at 0 and sets 10; daBgn3_Execute 02090680 counts it down */
    /* 0x11E94 */ be<f32> mPartStiffness;  /* was m0FDB8: part_control (in move 02091730) multiplies the part velocities by it ("stiffness") and eases it to l_HIO.m38; move0 sets REG + 0.9 on a wall hit, mahi / damage / end 1.0 */
    /* 0x11E98 */ be<f32> mPartDistance;  /* was m0FDBC: part_control (in move 02091730) places the part chain at (0, 0, -it) behind the head and eases it to l_HIO.m34; move0 sets REG + 30 while chasing the bait, end eases it to its target */
    /* 0x11E9C */ dCcD_Stts mStts;
    /* 0x11ED8 */ dCcD_Sph mHeadSph;  /* was m0FDFC: setModels (in move 02091730) centres it on the mpMorf model (r REG + 150) and turns its At on while moving; daBgn3_Create 02091008 sets it up; damage_check reads its hit position */
    /* 0x12004 */ dCcD_Sph mDekuSph;  /* was m0FF28: setModels (in move 02091730) centres it on mpDekuModel (r REG + 200) and turns its At on while moving; daBgn3_Create 02091008 sets it up */
    /* 0x12130 */ cXyz mCorePos;  /* was m10054: setModels (in move 02091730): world position of the part that carries the jyakuten (weak point) model; daBgn3_Delete 02090910 deletes its sound object */
    /* 0x1213C */ be<f32> mFogFade;  /* was m10060: bgn hensin 02084D6C sets 1, end (in move 02091730) eases it to 1, daBgn3_Execute 02090680 to 0; daBgn_Draw 0207B2CC (inlined daBgn3_Draw) blacks the fog by it (same use as bgn_class mFogFade) */
    /* 0x12140 */ Bgn3LineMat1 mRedRopeMat;
    /* 0x122C8 */ be<f32> mRopeSway;  /* was m100A0: damage / end (in move 02091730) ease it up / set the height, setModels eases it to 0 and swings the 60 points of mRedRopeMat by sin * it (same use as bgn2_class mRopeSway) */
    /* 0x122CC */ u8 mHDModelPacket[0xB0];
    /* 0x1237C */ u8 m100A4[4];
    /* 0x12380 */ be<s16> mShakeTimer;  /* was m100A8: damage_check (in move 02091730) sets 30 / 25 / 20 / 15 / 10 / 7 by the hit; setModels counts it down and shakes the head by it * (REG + 500) */
    /* 0x12382 */ u8 _12382[2];
    /* 0x12384 */ be<s32> mKiSetCount;  /* was m100AC: move0 (in move 02091730) sets l_HIO.m3C on a wall hit; daBgn3_Execute 02090680 counts it down and spawns one fpcNm_KI_e per count while fewer than 20 exist (same use as bgn2_class mKiSetCount) */
    /* 0x12388 */ be<u8> mHioOwner;  /* was m100B0: daBgn3_Create 02091008 sets 1 when it created the l_HIO child; daBgn3_Delete 02090910 deletes the HIO child only then (same as bgn_class mHioOwner) */
    /* 0x12389 */ u8 _12389[0x3];
};
WWHD_OFFSET(bgn3_class, mPhase, 0x3C8);
WWHD_OFFSET(bgn3_class, mpMorf, 0x3D0);
WWHD_OFFSET(bgn3_class, mHeadPacket, 0x3D4);
WWHD_OFFSET(bgn3_class, mpDekuModel, 0x484);
WWHD_OFFSET(bgn3_class, mDekuPacket, 0x488);
WWHD_OFFSET(bgn3_class, mHairs, 0x538);
WWHD_OFFSET(bgn3_class, mLineMat, 0x19D8);
WWHD_OFFSET(bgn3_class, m017A0, 0x1B20);
WWHD_OFFSET(bgn3_class, mpJyakutenCModel, 0x1BD0);
WWHD_OFFSET(bgn3_class, mpJyakutenBModel, 0x1BD4);
WWHD_OFFSET(bgn3_class, mpJyakutenAModel, 0x1BD8);
WWHD_OFFSET(bgn3_class, mJyakutenCBrkAnm, 0x1BDC);
WWHD_OFFSET(bgn3_class, mJyakutenBBrkAnm, 0x1BE0);
WWHD_OFFSET(bgn3_class, mParts, 0x1BE8);
WWHD_OFFSET(bgn3_class, mPartRootPos, 0x11E58);
WWHD_OFFSET(bgn3_class, mFrameCounter, 0x11E64);
WWHD_OFFSET(bgn3_class, mAction, 0x11E66);
WWHD_OFFSET(bgn3_class, mMode, 0x11E68);
WWHD_OFFSET(bgn3_class, mTurnSpeed, 0x11E6A);
WWHD_OFFSET(bgn3_class, mChaseTimer, 0x11E6C);
WWHD_OFFSET(bgn3_class, mWallHitCount, 0x11E6E);
WWHD_OFFSET(bgn3_class, mRippleSeTimer, 0x11E70);
WWHD_OFFSET(bgn3_class, mFogColorAdd, 0x11E72);
WWHD_OFFSET(bgn3_class, mFogStartAdd, 0x11E74);
WWHD_OFFSET(bgn3_class, mArrowHitFlashTimer, 0x11E78);
WWHD_OFFSET(bgn3_class, mCoreHitFrame, 0x11E7A);
WWHD_OFFSET(bgn3_class, mpArrowHitEmitter1, 0x11E7C);
WWHD_OFFSET(bgn3_class, mpArrowHitEmitter2, 0x11E80);
WWHD_OFFSET(bgn3_class, mArrowHitEffectTimer, 0x11E84);
WWHD_OFFSET(bgn3_class, mTimers, 0x11E86);
WWHD_OFFSET(bgn3_class, mTurnTimer, 0x11E8A);
WWHD_OFFSET(bgn3_class, mCenterTimer, 0x11E8C);
WWHD_OFFSET(bgn3_class, mDamageTimer, 0x11E90);
WWHD_OFFSET(bgn3_class, mHitSeTimer, 0x11E92);
WWHD_OFFSET(bgn3_class, mPartStiffness, 0x11E94);
WWHD_OFFSET(bgn3_class, mPartDistance, 0x11E98);
WWHD_OFFSET(bgn3_class, mStts, 0x11E9C);
WWHD_OFFSET(bgn3_class, mHeadSph, 0x11ED8);
WWHD_OFFSET(bgn3_class, mDekuSph, 0x12004);
WWHD_OFFSET(bgn3_class, mCorePos, 0x12130);
WWHD_OFFSET(bgn3_class, mFogFade, 0x1213C);
WWHD_OFFSET(bgn3_class, mRedRopeMat, 0x12140);
WWHD_OFFSET(bgn3_class, mRopeSway, 0x122C8);
WWHD_OFFSET(bgn3_class, mHDModelPacket, 0x122CC);
WWHD_OFFSET(bgn3_class, mShakeTimer, 0x12380);
WWHD_OFFSET(bgn3_class, mKiSetCount, 0x12384);
WWHD_OFFSET(bgn3_class, mHioOwner, 0x12388);
WWHD_SIZE(bgn3_class, 0x1238C);

struct daBgn3_HIO_c {
    /* 0x00000 */ be<s8> mNo;
    /* 0x00001 */ be<u8> mNoMove;  /* was m05: move 02091730: the actions run only while it is 0 (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (0) */
    /* 0x00002 */ be<u8> mDebugHealth;  /* was m06: daBgn3_Execute 02090680: when not 0, health is set to it (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (0) */
    /* 0x00003 */ u8 _00003[0x1];
    /* 0x00004 */ be<f32> mSpeedHealth3;  /* was m08: move0 (in move 02091730): the speedF target at health 3 (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (50) */
    /* 0x00008 */ be<s16> mTurnSpeedHealth3;  /* was m0C: move0 (in move 02091730): mTurnSpeed at health 3 (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (800) */
    /* 0x0000A */ be<s16> mWallHitsHealth3;  /* was m0E: move0 (in move 02091730): at health 3 the chase (mChaseTimer) starts after this many wall hits (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (3) */
    /* 0x0000C */ be<f32> mSpeedHealth2;  /* was m10: move0 (in move 02091730): the speedF target at health 2 (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (60) */
    /* 0x00010 */ be<s16> mTurnSpeedHealth2;  /* was m14: move0 (in move 02091730): mTurnSpeed at health 2 (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (1000) */
    /* 0x00012 */ be<s16> mWallHitsHealth2;  /* was m16: move0 (in move 02091730): the wall hits before the chase at health 2 (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (3) */
    /* 0x00014 */ be<f32> mSpeedHealth1;  /* was m18: move0 (in move 02091730): the speedF target at health 1 (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (80) */
    /* 0x00018 */ be<s16> mTurnSpeedHealth1;  /* was m1C: move0 (in move 02091730): mTurnSpeed at health 1 (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (1200) */
    /* 0x0001A */ be<s16> mWallHitsHealth1;  /* was m1E: move0 (in move 02091730): the wall hits before the chase at health 1 (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (3) */
    /* 0x0001C */ be<s16> mChaseTimeHealth3;  /* was m20: move0 (in move 02091730): mChaseTimer after the wall hits at health 3 (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (60) */
    /* 0x0001E */ be<s16> mChaseTimeHealth2;  /* was m22: move0 (in move 02091730): mChaseTimer after the wall hits at health 2 (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (40) */
    /* 0x00020 */ be<s16> mChaseTimeHealth1;  /* was m24: move0 (in move 02091730): mChaseTimer after the wall hits at health 1 (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (30) */
    /* 0x00022 */ be<s16> mOtherHitTime;  /* was m26: damage_check (in move 02091730): mTimers[0] after a hit that is none of the cases below (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (10) */
    /* 0x00024 */ be<s16> mArrowHitTime;  /* was m28: damage_check (in move 02091730): mTimers[0] after an arrow hit (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (20) */
    /* 0x00026 */ be<s16> mSwordHitTime;  /* was m2A: damage_check (in move 02091730): mTimers[0] after a sword hit (attack type 1) whose AtSpl is not 1 (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (45) */
    /* 0x00028 */ be<s16> mSpl1HitTime;  /* was m2C: hioCtor 020914F4 sets 120; damage_check 02090384: the shake duration of a hit with mSpl 1 (next to mSwordHitTime / mArrowHitTime / mOtherHitTime) (tww the same) */
    /* 0x0002A */ be<s16> mBombHitTime;  /* was m2E: damage_check (in move 02091730): mTimers[0] after a bomb hit (attack type 2) (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (200) */
    /* 0x0002C */ be<s16> mHammerHitTime;  /* was m30: damage_check (in move 02091730): mTimers[0] after a Skull Hammer hit (attack type 9) (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (250) */
    /* 0x0002E */ u8 _0002E[0x2];
    /* 0x00030 */ be<f32> mPartDistanceTarget;  /* was m34: part_control (in move 02091730): mPartDistance eases to it (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (5.0) */
    /* 0x00034 */ be<f32> mPartStiffnessTarget;  /* was m38: part_control (in move 02091730): mPartStiffness eases to it (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (0.5) */
    /* 0x00038 */ be<s16> mWallKiCount;  /* was m3C: move0 (in move 02091730): a wall hit sets mKiSetCount to it; daBgn3_Execute 02090680 then creates one Ki (0xCD) per frame (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (3) */
    /* 0x0003A */ be<u8> mHairOn;  /* was m3E: daBgn3_Draw 0208FC64 / ke_move 02090420 / setModels (in move 02091730): the hair lines are moved and drawn only while it is set (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (1) */
    /* 0x0003B */ u8 _0003B[0x1];
    /* 0x0003C */ be<f32> mHairPullY;  /* was m40: setModels (in move 02091730): each hair's mPull is (0, it, 0) in the hair's frame (tww move the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (150) */
    /* 0x00040 */ be<f32> mHairLength;  /* was m44: setModels (in move 02091730): ke_move's segment length is 2 * it (the parts: their scale * it) (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (45) */
    /* 0x00044 */ be<f32> mHairWidth;  /* was m48: daBgn3_Draw 0208FC64: mDoExt_3DlineMat0_c::update's width for the hair lines (tww the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (8) */
    /* 0x00048 */ be<f32> mHairGravity;  /* was m4C: ke_move 02090420: added to each hair point's y (tww ke_control the same); hioCtor 020914F4 sets the default, never written elsewhere in WWHD or tww (-8) */
    /* 0x0004C */ be<s16> mHairColorR;  /* was m50: written in WWHD only by hioCtor 020914F4 (188); HD daBgn3_Draw uses the constant 188; tww daBgn3_Create reads it as ke_color.r */
    /* 0x0004E */ be<s16> mHairColorG;  /* was m52: written in WWHD only by hioCtor 020914F4 (152); HD daBgn3_Draw uses the constant 152; tww daBgn3_Create reads it as ke_color.g */
    /* 0x00050 */ be<s16> mHairColorB;  /* was m54: written in WWHD only by hioCtor 020914F4 (66); HD daBgn3_Draw uses the constant 66; tww daBgn3_Create reads it as ke_color.b */
    /* 0x00052 */ u8 _00052[0x2];
    /* 0x00054 */ be<u32> __vtbl;
};
WWHD_OFFSET(daBgn3_HIO_c, mNo, 0x0);
WWHD_OFFSET(daBgn3_HIO_c, mNoMove, 0x1);
WWHD_OFFSET(daBgn3_HIO_c, mDebugHealth, 0x2);
WWHD_OFFSET(daBgn3_HIO_c, mSpeedHealth3, 0x4);
WWHD_OFFSET(daBgn3_HIO_c, mTurnSpeedHealth3, 0x8);
WWHD_OFFSET(daBgn3_HIO_c, mWallHitsHealth3, 0xA);
WWHD_OFFSET(daBgn3_HIO_c, mSpeedHealth2, 0xC);
WWHD_OFFSET(daBgn3_HIO_c, mTurnSpeedHealth2, 0x10);
WWHD_OFFSET(daBgn3_HIO_c, mWallHitsHealth2, 0x12);
WWHD_OFFSET(daBgn3_HIO_c, mSpeedHealth1, 0x14);
WWHD_OFFSET(daBgn3_HIO_c, mTurnSpeedHealth1, 0x18);
WWHD_OFFSET(daBgn3_HIO_c, mWallHitsHealth1, 0x1A);
WWHD_OFFSET(daBgn3_HIO_c, mChaseTimeHealth3, 0x1C);
WWHD_OFFSET(daBgn3_HIO_c, mChaseTimeHealth2, 0x1E);
WWHD_OFFSET(daBgn3_HIO_c, mChaseTimeHealth1, 0x20);
WWHD_OFFSET(daBgn3_HIO_c, mOtherHitTime, 0x22);
WWHD_OFFSET(daBgn3_HIO_c, mArrowHitTime, 0x24);
WWHD_OFFSET(daBgn3_HIO_c, mSwordHitTime, 0x26);
WWHD_OFFSET(daBgn3_HIO_c, mSpl1HitTime, 0x28);
WWHD_OFFSET(daBgn3_HIO_c, mBombHitTime, 0x2A);
WWHD_OFFSET(daBgn3_HIO_c, mHammerHitTime, 0x2C);
WWHD_OFFSET(daBgn3_HIO_c, mPartDistanceTarget, 0x30);
WWHD_OFFSET(daBgn3_HIO_c, mPartStiffnessTarget, 0x34);
WWHD_OFFSET(daBgn3_HIO_c, mWallKiCount, 0x38);
WWHD_OFFSET(daBgn3_HIO_c, mHairOn, 0x3A);
WWHD_OFFSET(daBgn3_HIO_c, mHairPullY, 0x3C);
WWHD_OFFSET(daBgn3_HIO_c, mHairLength, 0x40);
WWHD_OFFSET(daBgn3_HIO_c, mHairWidth, 0x44);
WWHD_OFFSET(daBgn3_HIO_c, mHairGravity, 0x48);
WWHD_OFFSET(daBgn3_HIO_c, mHairColorR, 0x4C);
WWHD_OFFSET(daBgn3_HIO_c, mHairColorG, 0x4E);
WWHD_OFFSET(daBgn3_HIO_c, mHairColorB, 0x50);
WWHD_OFFSET(daBgn3_HIO_c, __vtbl, 0x54);
WWHD_SIZE(daBgn3_HIO_c, 0x58);

// HD process IDs corresponding to the GameCube searches in this translation unit.
enum { fpcNm_BGN_e = 0xF3, fpcNm_KS_e = 0xCD, fpcNm_ARROW_e = 0x1D8 };
// HD: shared pause byte written by damage reactions.
inline be<u8>& bgn3PauseTimer() { return *gabi::at<be<u8>>(0x101EACB7); }

template <class T> inline be<u32>& bgn3Word(T& field) {
    return *gabi::at<be<u32>>(gabi::ea(&field));
}

// HD: the inline line-check frame includes the pass-check subobjects (0x6C bytes).
struct Bgn3LinChk {
    be<u32> mpPolyPassChk;
    be<u32> mpGrpPassChk;
    u8 _08[8];
    be<u32> __vtbl_10;
    u8 mPolyInfo[0xC];
    be<u32> __vtbl_20;
    u8 _24[0xC];
    cXyz mCross;
    u8 _3C[0x1C];
    be<u32> __vtbl_58;
    be<u8> mPass[8];
    be<u32> __vtbl_64;
    be<u32> mGrp;
};
WWHD_SIZE(Bgn3LinChk, 0x6C);
WWHD_OFFSET(Bgn3LinChk, mCross, 0x30);
WWHD_OFFSET(Bgn3LinChk, mPass, 0x5C);
// HD: this inline attack-info frame is 0x14 bytes, not the general CcAtInfo layout.
struct Bgn3AtInfo {
    be<u32> mpObj;
    be<u32> mpActor;
    u8 _08[2];
    be<u8> mResultingAttackType;
    u8 _0B[9];
};
WWHD_SIZE(Bgn3AtInfo, 0x14);
WWHD_OFFSET(Bgn3AtInfo, mResultingAttackType, 0xA);
inline void cMtx_XrotS(Mtx34* matrix, s16 angle) { gabi::call(0x025F18EC, matrix, angle); }

inline part_s3* bgn3Part(u32 address) { return gabi::at<part_s3>(address); }
struct Bgn3SinCos {
    be<f32> mSin, mCos;
};
WWHD_SIZE(Bgn3SinCos, 8);

struct Bgn3LineRecord {
    gptr<cXyz> mpPositions;
    u8 _04[12];
};
WWHD_SIZE(Bgn3LineRecord, 0x10);
inline cXyz* bgn3LinePositions(Bgn3LineMat0* line, int index) {
    /* guest address arithmetic: lines + index * 0x10 wraps at 32 bits as on the console */
    return sead::elementAt<Bgn3LineRecord>(u32(line->mpLines), u32(index)).mpPositions;
}
enum Bgn3ResourceIndex {
    dRes_INDEX_BGN_BDL_BGN_HEAD1_e = 21,
    dRes_INDEX_BGN_BCK_BGN_HEAD1_e = 6,
    dRes_INDEX_BGN_BDL_BGN_DEKU1_e = 20,
    dRes_INDEX_BGN_BDL_BGN_JYAKUTENA3_e = 24,
    dRes_INDEX_BGN_BDL_BGN_JYAKUTENB3_e = 27,
    dRes_INDEX_BGN_BDL_BGN_JYAKUTENC3_e = 30,
    dRes_INDEX_BGN_BRK_BGN_JYAKUTENB3_e = 39,
    dRes_INDEX_BGN_BRK_BGN_JYAKUTENC3_e = 42,
    dRes_INDEX_BGN_BTI_NOT_CUT1_e = 46,
};

// HD emitter fields used by the worm's arrow-hit effects.
struct Bgn3EmitterView {
    u8 _000[0x5C];
    be<s32> mMaxFrame;
    u8 _060[0x220 - 0x60];
    cXyz mGlobalScale;
    cXyz mGlobalTranslation;
    cXyz mGlobalParticleScale;
    u8 _244[0x10];
    be<u32> mFlags;
    u8 _258[0xA];
    be<u8> mType;
    u8 _263;
};
WWHD_SIZE(Bgn3EmitterView, 0x264);
WWHD_OFFSET(Bgn3EmitterView, mGlobalScale, 0x220);
WWHD_OFFSET(Bgn3EmitterView, mGlobalTranslation, 0x22C);
WWHD_OFFSET(Bgn3EmitterView, mGlobalParticleScale, 0x238);
WWHD_OFFSET(Bgn3EmitterView, mFlags, 0x254);
WWHD_OFFSET(Bgn3EmitterView, mType, 0x262);

// HD entry bindings retain the verified full-width argument and return registers.
namespace Bgn3Bindings {
inline s32 cBgS_LineCross(u32 a0, u32 a1) { return gabi::call<s32>(0x02008860, a0, a1); }
inline void cBgS_LinChk_dt(u32 a0, u32 a1) { return gabi::call<void>(0x02008B4C, a0, a1); }
inline void cBgS_LinChk_ct(u32 a0) { return gabi::call<void>(0x02008FEC, a0); }
inline void cCcD_Stts_ct(u32 a0) { return gabi::call<void>(0x0200BD2C, a0); }
inline void cCcS_Set(u32 a0, u32 a1) { return gabi::call<void>(0x0200E240, a0, a1); }
inline u32 cM_rad2s(f32 a0) { return gabi::call<u32>(0x02019510, a0); }
inline void cXyz_add(u32 a0, u32 a1, u32 a2) { return gabi::call<void>(0x0201AD78, a0, a1, a2); }
inline void cXyz_subtract(u32 a0, u32 a1, u32 a2) {
    return gabi::call<void>(0x0201ADE0, a0, a1, a2);
}
inline void mDoExt_J3DModelPacketS_setMaterial(u32 a0, u32 a1) {
    return gabi::call<void>(0x0207FD38, a0, a1);
}
inline void mDoExt_J3DModelPacketS_ct(u32 a0) { return gabi::call<void>(0x02080404, a0); }
inline void mDoExt_J3DModelPacketS_dt(u32 a0, u32 a1) {
    return gabi::call<void>(0x02082DDC, a0, a1);
}
inline s32 dBgS_GetAttributeCode(u32 a0, u32 a1) { return gabi::call<s32>(0x024EF0F4, a0, a1); }
inline void dBgS_LinChk_Set(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x024F1AFC, a0, a1, a2, a3);
}
inline u32 dCcD_GetGObjInf(u32 a0) { return gabi::call<u32>(0x025157AC, a0); }
inline void dCcD_Stts_dt(u32 a0, u32 a1) { return gabi::call<void>(0x02515860, a0, a1); }
inline void dCcD_Sph_dt(u32 a0, u32 a1) { return gabi::call<void>(0x02515AE8, a0, a1); }
inline void dCcD_GStts_ct(u32 a0) { return gabi::call<void>(0x02515DA0, a0); }
inline void dCcD_Stts_Init(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x02515F14, a0, a1, a2, a3);
}
inline s32 dCcD_GObjInf_ChkTgHit(u32 a0) { return gabi::call<s32>(0x025162A4, a0); }
inline u32 dCcD_GObjInf_GetTgHitObj(u32 a0) { return gabi::call<u32>(0x02516300, a0); }
inline void dCcD_Sph_ct(u32 a0) { return gabi::call<void>(0x025166F0, a0); }
inline void dCcD_Sph_Set(u32 a0, u32 a1) { return gabi::call<void>(0x0251677C, a0, a1); }
inline void def_se_set(u32 a0, u32 a1, u32 a2) { return gabi::call<void>(0x02518CC8, a0, a1, a2); }
inline u32 at_power_check(u32 a0) { return gabi::call<u32>(0x02518DB0, a0); }
inline s32 dComIfG_resLoad(u32 a0, u32 a1) { return gabi::call<s32>(0x02520460, a0, a1); }
inline void dKy_SordFlush_set(u32 a0, u32 a1) { return gabi::call<void>(0x0255F554, a0, a1); }
inline u32 dPa_control_c_set(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8,
                             u32 a9, u32 a10, u32 a11) {
    return gabi::call<u32>(0x025A847C, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
}
inline void dPa_control_c_setSimple(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6) {
    return gabi::call<void>(0x025A8D40, a0, a1, a2, a3, a4, a5, a6);
}
inline void dVibration_c_StartShock(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x025CB374, a0, a1, a2, a3);
}
inline void fopAc_ac_c_ct(u32 a0) { return gabi::call<void>(0x025D4ED0, a0); }
inline void fopAc_ac_c_dt(u32 a0, u32 a1) { return gabi::call<void>(0x025D50BC, a0, a1); }
inline u32 fopAcM_create(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7) {
    return gabi::call<u32>(0x025D5834, a0, a1, a2, a3, a4, a5, a6, a7);
}
inline s32 fopAcM_entrySolidHeap(u32 a0, u32 a1, u32 a2) {
    return gabi::call<s32>(0x025D63E8, a0, a1, a2);
}
inline s16 fopAcM_searchActorAngleY(u32 a0, u32 a1) { return gabi::call<s16>(0x025D6894, a0, a1); }
inline u32 fpcM_Search(u32 a0, u32 a1) { return gabi::call<u32>(0x025DE508, a0, a1); }
inline void mDoAud_bgmStop(u32 a0) { return gabi::call<void>(0x025E1904, a0); }
inline void mDoAud_seStart(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x025E1A40, a0, a1, a2, a3);
}
inline void mDoAud_monsSeStart(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x025E1A7C, a0, a1, a2, a3);
}
inline void mDoAud_monsSeStartAtActor(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) {
    return gabi::call<void>(0x025E1AA4, a0, a1, a2, a3, a4);
}
inline u32 mDoExt_J3DModel_create(u32 a0, u32 a1, u32 a2) {
    return gabi::call<u32>(0x025E38E0, a0, a1, a2);
}
inline u32 mDoExt_McaMorf_ct(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8,
                             u32 a9, u32 a10, u32 a11, f32 a12) {
    return gabi::call<u32>(0x025E4F64, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
}
inline void mDoExt_McaMorf_play(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x025E535C, a0, a1, a2, a3);
}
inline void mDoExt_McaMorf_calc(u32 a0) { return gabi::call<void>(0x025E55A0, a0); }
inline void mDoExt_baseAnm_play(u32 a0) { return gabi::call<void>(0x025E742C, a0); }
inline u32 mDoExt_brkAnm_ct(u32 a0) { return gabi::call<u32>(0x025E80D0, a0); }
inline s32 mDoExt_brkAnm_init(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                              u32 a8, f32 a9) {
    return gabi::call<s32>(0x025E8154, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
}
inline void mDoExt_3DlineMat0_ct(u32 a0) { return gabi::call<void>(0x025E9960, a0); }
inline void mDoExt_3DlineMat0_dt(u32 a0, u32 a1) { return gabi::call<void>(0x025E99E0, a0, a1); }
inline s32 mDoExt_3DlineMat0_init(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<s32>(0x025E9B80, a0, a1, a2, a3);
}
inline void mDoExt_3DlineMat0_update(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, f32 a5) {
    return gabi::call<void>(0x025EA548, a0, a1, a2, a3, a4, a5);
}
inline void mDoExt_3DlineMat1_ct(u32 a0) { return gabi::call<void>(0x025EB82C, a0); }
inline void mDoExt_3DlineMat1_dt(u32 a0, u32 a1) { return gabi::call<void>(0x025EB8B8, a0, a1); }
inline s32 mDoExt_3DlineMat1_init(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) {
    return gabi::call<s32>(0x025EBA58, a0, a1, a2, a3, a4);
}
inline void mDoExt_3DlineMatSortPacket_setMat(u32 a0, u32 a1) {
    return gabi::call<void>(0x025EDD04, a0, a1);
}
inline u32 mDoHIO_createChild(u32 a0, u32 a1) { return gabi::call<u32>(0x025F0A10, a0, a1); }
inline u32 operator_new(u32 a0) { return gabi::call<u32>(0x0273AD10, a0); }
inline void operator_delete(u32 a0) { return gabi::call<void>(0x0273AF40, a0); }
inline void PSVECSubtract(u32 a0, u32 a1, u32 a2) {
    return gabi::call<void>(0x028E8DAC, a0, a1, a2);
}
inline void std_allocator_ct(u32 a0) { return gabi::call<void>(0x028EAB2C, a0); }
inline void std_char_traits_ct(u32 a0) { return gabi::call<void>(0x028ED6F8, a0); }
inline void construct_array(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x028EFFD0, a0, a1, a2, a3);
}
inline void destroy_array(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5) {
    return gabi::call<void>(0x028F0164, a0, a1, a2, a3, a4, a5);
}
inline void register_global_object(u32 a0) { return gabi::call<void>(0x028F026C, a0); }
inline f32 sqrtf(f32 a0) { return gabi::call<f32>(0x028F4384, a0); }
} // namespace Bgn3Bindings

// HD: the lighting constructor copies this 0x44-byte prefix to three light records.
struct Bgn3LightInit {
    be<f32> mPositionDirection[6];
    be<u8> mColorBytes[4];
    be<s16> mColorWords[4];
    be<f32> mParameters[8];
};
WWHD_SIZE(Bgn3LightInit, 0x44);
struct Bgn3TevCopyView {
    Bgn3LightInit mLight;
    u8 _044[0x40];
    cXyz mLightPosWorld;
    be<u16> mColorC0[4];
    be<u32> mColorK0;
    be<u32> mColorK1;
    be<u16> mFogColor[4];
    be<f32> mFogValues[3];
    be<u8> mEnvironment[9];
    u8 _0BD[3];
    Bgn3LightInit mLightCopy1;
    u8 _104[0x40];
    Bgn3LightInit mLightCopy2;
    u8 _188[0x40];
};
WWHD_SIZE(Bgn3TevCopyView, 0x1C8);
WWHD_OFFSET(Bgn3TevCopyView, mLightPosWorld, 0x84);
WWHD_OFFSET(Bgn3TevCopyView, mLightCopy1, 0xC0);
WWHD_OFFSET(Bgn3TevCopyView, mLightCopy2, 0x144);

struct Bgn3BossView {
    u8 _000[0x3D5];
    be<s8> mForm;
    u8 _3D6[0x151CE - 0x3D6];
    be<u8> mCSMode;
    u8 _151CF;
};
WWHD_SIZE(Bgn3BossView, 0x151D0);
WWHD_OFFSET(Bgn3BossView, mForm, 0x3D5);
WWHD_OFFSET(Bgn3BossView, mCSMode, 0x151CE);
struct Bgn3HeaderStatics {
    be<f32> mNegativePi, mPi;
    be<u8> mFlag, mByte;
    u8 _0A[2];
    be<u32> mZeroWords[4];
};
WWHD_SIZE(Bgn3HeaderStatics, 0x1C);
struct Bgn3PiConstants {
    be<f32> mNegativePi, mPi;
};
WWHD_SIZE(Bgn3PiConstants, 8);
inline dComIfG_inf_c* bgn3GameInfo(u32 address) { return gabi::at<dComIfG_inf_c>(address); }

struct Bgn3TimerView {
    be<s16> mTimers[5];
};
WWHD_SIZE(Bgn3TimerView, 10);
struct Bgn3RopeLineRecord {
    gptr<cXyz> mpPositions;
    gptr<be<u8>> mpSizes;
    u8 _08[0x10];
};
WWHD_SIZE(Bgn3RopeLineRecord, 0x18);
inline u32 bgn3LineMaterialID(Bgn3LineMat0* line) {
    u32 vtable = line->__vtbl;
    return gabi::call_ptr<u32>(gabi::load<u32>(vtable + 0x14), line);
}

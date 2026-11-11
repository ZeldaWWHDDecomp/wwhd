/* bdk_class (Helmaroc King, battle), WWHD layout. 
 *
 * GameCube -> WWHD: fopEn_enemy_c is +0x11C (mPhase .. mpMorf). HD has no mp2BC/m2C0 (the real
 * shadow model and its id): mFrameCounter .. mp8F0 are +0x114. HD has no bva (the mask's visibility
 * animation; the mask's materials are switched directly): mMaskBreakStage .. mMaskPieceOn are +0x110. The
 * kamen-break block shrinks by 0x10 (see below): mMaskPieceStts .. mMaskPieceSpinRate are +0x100. HD has no mF14 (the
 * neck's Y rotation): mTargetOffset .. mBgTevStr are +0xFC. dKy_tevstr_c grows by 0x118: mTogeOffsetY .. the end
 * are +0x214. Size 0x65E0 (constructor 02069964). */
#pragma once
#include "bindings.h"

struct bdk_tail_s {
    /* 0x000 */ gptr<J3DModel> mpModel[9];  /* was m000: useHeapInit 02069428 creates the 9 segment models of each tail; daBdk_Draw 02065AB4 (tail_draw) gives each its matrix and draws it */
    /* 0x024 */ cXyz mPos[10];  /* was m024: tail_control (in daBdk_Execute 02066A38) sets [0] to mRootPos[1] and chains [1..9] behind it (not below the ground + 5); daBdk_Draw 02065AB4 (tail_draw) translates the segment models there */
    /* 0x09C */ csXyz mAngle[10];  /* was m09C: tail_control (in daBdk_Execute 02066A38) sets [i - 1] .y / .x to the heading / minus the pitch toward segment i; daBdk_Draw 02065AB4 (tail_draw) rotates the segment models by it */
    /* 0x0D8 */ cXyz mSpeed[10];  /* was m0D8: tail_control (in daBdk_Execute 02066A38) adds it to the segment position, then stores (new - old position) * 0.77 in it */
    /* 0x150 */ cXyz mRootPos[2];  /* was m0150: nodeCallBack 02065794 sets [1] to the tail joint position and [0] to the point 10 behind it; tail_control (in daBdk_Execute 02066A38) aims the tail along [0] -> [1] and starts the chain at [1] */
    /* 0x168 */ csXyz mRootAngle;  /* was m0168: tail_control (in daBdk_Execute 02066A38) sets .y / .x to the heading / minus the pitch of mRootPos[0] -> [1] */
    /* 0x16E */ u8 _16E[2];
    /* 0x170 */ cXyz mRootStep;  /* was m0170: tail_control (in daBdk_Execute 02066A38) sets it to 30 units along mRootAngle and pushes segment i by it * (1 - 0.1 * (i - 1)) */
};
WWHD_SIZE(bdk_tail_s, 0x17C);

struct bdk_eff_s {
    /* 0x000 */ be<s8> mType;  /* was m000: eff_hane_set 02065278 sets 1 (a feather), kuti_attack (in move 0206D644, mEff[0x27]) 2 (the rock) and 0, damage_check (in daBdk_Execute 02066A38) 0; my_effect_move 02071A68 and daBdk_Draw 02065AB4 (my_effect_draw) handle 1 / 2 and skip 0; eff_hane_move (in my_effect_move) clears it when the vanished feather is small */
    /* 0x001 */ be<s8> mVanish;  /* was m001: eff_hane_set 02065278 clears it; eff_hane_move (in my_effect_move 02071A68) sets 1 below y 9700, when mLifeTimer ends or off the land area, then shrinks mScale and frees the slot */
    /* 0x002 */ u8 _002[2];
    /* 0x004 */ cXyz mPos;  /* was m004: eff_hane_set 02065278 sets it (scattered by +-500 for param_4 < 0); eff_hane_move (in my_effect_move 02071A68) moves it by mSpeedF / mSpeedY, keeps it on the ground, places the model and mSph there; kuti_attack (in move 0206D644) places the rock and its particles there */
    /* 0x010 */ cXyz mPosOld;  /* was m010: eff_hane_move (in my_effect_move 02071A68) copies mPos before moving; on a line hit x / z go back to it */
    /* 0x01C */ be<f32> mSpeedY;  /* was m01C: eff_hane_set 02065278 sets 5..10 (or -10..-5 for a thrown feather); eff_hane_move (in my_effect_move 02071A68) adds it to mPos.y and lowers it by 0.1 per frame to -5; a hit sets 10..15 */
    /* 0x020 */ be<f32> mSpeedF;  /* was m020: eff_hane_set 02065278 sets 5..30; eff_hane_move (in my_effect_move 02071A68) moves mPos along mAngle.y by it, clears it on a wall, eases it to 0 on the ground; a hit sets 10..15 */
    /* 0x024 */ be<f32> mPivotHeight;  /* was m024: eff_hane_set 02065278 sets 0; eff_hane_move (in my_effect_move 02071A68) eases it to -130 / -300 high up (above y 9930) and to 0 below, and tilts the model (mAngle.x / .z) around the point it below mPos */
    /* 0x028 */ be<f32> mFlutterAmplitude;  /* was m028: eff_hane_move (in my_effect_move 02071A68) eases it to 8000 (15000 while mBlowTimer runs) in the air and tilts the feather by sin / cos(mFrameCounter) * it */
    /* 0x02C */ be<f32> mScale;  /* was m02C: eff_hane_set 02065278 sets 1; eff_hane_move (in my_effect_move 02071A68) scales the model by it and shrinks it to 0 while mVanish is set */
    /* 0x030 */ csXyz mAngle;  /* was m030: eff_hane_set 02065278 sets .y (the flight heading); eff_hane_move (in my_effect_move 02071A68) moves along .y, turns .y by mSpin.x on the ground, sets .x / .z from mFlutterAmplitude in the air and eases them to 0 on the ground; a hit turns .y away from the hitter */
    /* 0x036 */ csXyz mSpin;  /* was m036: eff_hane_set 02065278 sets .x / .z to random spin speeds; eff_hane_move (in my_effect_move 02071A68) adds .z to .y (the model yaw), adds .x to mAngle.y on the ground and eases .x / .z to 0 there; a hit sets new .x / .z */
    /* 0x03C */ be<s16> mFrameCounter;  /* was m03C: eff_hane_set 02065278 sets a random start, kuti_attack (in move 0206D644) 0; my_effect_move 02071A68 adds 1 per frame; the phase of the flutter tilt; eff_Grock_move lowers the rock by 10 at 1 */
    /* 0x03E */ be<s16> mLifeTimer;  /* was m03E: eff_hane_set 02065278 sets 2000; eff_hane_move (in my_effect_move 02071A68) counts it down and sets mVanish at 0 */
    /* 0x040 */ be<s8> mBlowTimer;  /* was m040: eff_hane_set 02065278 sets 0 (20..40 for a thrown feather); a hit in eff_hane_move (in my_effect_move 02071A68) sets 30..60; counted down; while it runs the feather takes no hits and flutters harder (15000, pivot -300) */
    /* 0x041 */ u8 _041[3];
    /* 0x044 */ gptr<J3DModel> mpModel;  /* was m044: useHeapInit 02069428 creates the feather / rock model of each slot; eff_hane_move / eff_Grock_move (in my_effect_move 02071A68) set its matrix; daBdk_Draw 02065AB4 (my_effect_draw) draws it */
    /* 0x048 */ dCcD_Sph mSph;  /* was m048: bdk_eff_s_ct 0207260C / bdk_eff_s_dt 0207267C construct and destroy it; daBdk_Create 02069C8C sets it up with eff_sph_src and mEffStts; eff_hane_move (in my_effect_move 02071A68) places it at mPos while the feather lives and blows the feather away when it is hit */
};
WWHD_SIZE(bdk_eff_s, 0x174);

/* dPa_smokeEcallBack (HD 0x20): vtable at +0, emitter at +4 */
struct dPa_smokeEcallBack_bdk {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ u8 _08[0x20 - 0x08];
};
WWHD_SIZE(dPa_smokeEcallBack_bdk, 0x20);

struct bdk_class : fopEn_enemy_c {
    /* 0x03C8 */ request_of_phase_process_class mPhase;
    /* 0x03D0 */ be<u8> mTowerBattle;  /* was m2B4: daBdk_Create 02069C8C sets 1 and then starts ACTION_T_FLY with JA_BGM_MJ_TOWER_BATTLE; demo_camera 0206FEF0 clears it when the main battle starts (ACTION_FLY, JA_BGM_DK_BATTLE); while 1 daBdk_Execute 02066A38 scales the body 0.85, kankyo_cont 0206F9E0 makes the wind spheres 600 (else 300) and damage_check (in daBdk_Execute) sends a hammer hit to ACTION_T_DOWN (tww the same; tww's create also loads fopAcM_GetParam first) */
    /* 0x03D1 */ u8 _3D1[3];
    /* 0x03D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x03D8 */ be<s16> mFrameCounter;  /* was m2C4: daBdk_Execute 02066A38 adds 1 per frame; wind_attack 0206AE64 / kamen_draw (in daBdk_Draw 02065AB4) / head_swing use it as the phase of cM_ssin swings; t_fly 0206C3AC tests == 100 */
    /* 0x03DA */ be<s16> mAction;
    /* 0x03DC */ be<s16> mState;
    /* 0x03DE */ be<s16> mFlyAnm;  /* was m2CA: daBdk_Create 02069C8C sets -1; fly / wait (in move 0206D644) set 0..3 and start the matching animation (e.g. 1: SENKAI_L1) only when the value changes */
    /* 0x03E0 */ cXyz mTargetPos;  /* was m2CC: fly_attack 0206A610 sets it to the player position (+ mTargetOffset) or fixed points; wind_attack 0206AE64 copies current.pos; pos_move 02066398 flies toward it */
    /* 0x03EC */ be<s16> mTargetAngleY;  /* was m2D8: fly_attack 0206A610: angle.y + 0x8000, then turns current.angle.y toward it */
    /* 0x03EE */ u8 _3EE[2];
    /* 0x03F0 */ be<f32> mTurnSpeed;  /* was m2DC: fly_attack 0206A610 / fly, wait (in move 0206D644) set 100..1000; pos_move 02066398: angle step = it * mTurnSpeedRate */
    /* 0x03F4 */ be<f32> mTurnSpeedRate;  /* was m2E0: fly_attack 0206A610 clears it, wind_attack 0206AE64 / t_fly 0206C3AC ease it to 1; pos_move 02066398 multiplies mTurnSpeed by it */
    /* 0x03F8 */ be<f32> mTargetSpeed;  /* was m2E4: fly_attack 0206A610 / fly, wait, jump (in move 0206D644) set it (l_HIO speeds, 0, 30, 40); pos_move 02066398 eases speedF to it */
    /* 0x03FC */ be<f32> mSpeedStep;  /* was m2E8: fly_attack 0206A610 / move 0206D644 set 1/5/30; pos_move 02066398: the max step of speedF toward mTargetSpeed */
    /* 0x0400 */ be<s16> mTimers[5];  /* was m2EC: daBdk_Execute 02066A38 counts all 5 down each frame; actions load them (daBdk_Create 0x168, fly_attack 0206A610 0x1E/200) */
    /* 0x040A */ be<s16> mTailDrawDelay;  /* was m2F6: start 0206B67C sets 25; daBdk_Draw 02065AB4 (tail_draw) skips a tail and counts it down while it is not 0 */
    /* 0x040C */ be<s16> mDamageTimer;  /* was m2F8: damage_check (in daBdk_Execute 02066A38) takes no hit while it is set and sets 0xC / 0x1E / 3 / 6 / 0x14 after a hit; t_fly 0206C3AC sets 5, end 0206BAA8 / after_fight 0206D638 10; wind_attack 0206AE64 lands at once while it is set; daBdk_Execute counts it down (as bgn_class mDamageTimer) */
    /* 0x040E */ be<s16> mMaskShakeTimer;  /* was m2FA: damage_check (in daBdk_Execute 02066A38) sets 10, daBdk_Execute counts down; kamen_draw (in daBdk_Draw 02065AB4) shakes the mask by it * 150 */
    /* 0x0410 */ be<u8> mFlyPoint;  /* was m2FC: fly (in move 0206D644): which fly home point is used (0 = 6500 / 11000 / 2600, 1 = 9800 / 12000 / -1500, 2 = -1000 / 12000 / -2900); chosen at random, then alternates 1 / 2 (20 % back to 0); at 0 the arrival always lands, at 1 / 2 a fly_attack may follow */
    /* 0x0411 */ u8 _411[3];
    /* 0x0414 */ bdk_tail_s mTails[4];  /* was m300: useHeapInit 02069428 creates the bdk_tail_s models; tail_control (daBdk_Execute 02066A38) and nodeCallBack 02065794 / daBdk_Draw 02065AB4 per tail */
    /* 0x0A04 */ gptr<J3DModel> mp8F0;
    /* 0x0A08 */ be<u8> mMaskBreakStage;  /* was m8F8: damage_check (in daBdk_Execute 02066A38) adds 1 per mask hit up to 4; kamen_draw (in daBdk_Draw 02065AB4) shows kamen_pt.mJoint[it]; col_set and move 0206D644 branch on it */
    /* 0x0A09 */ u8 _A09[3];
    /* 0x0A0C */ gptr<J3DModel> mpMaskPieceModel[4];  /* was m8FC: useHeapInit 02069428 creates the 4 mask piece models; kamen_break_move (in daBdk_Execute 02066A38) places them, daBdk_Draw 02065AB4 draws them */
    /* 0x0A1C */ be<s8> mMaskPieceOn[4];  /* was m90C: kamen_demo 0206B22C sets [i] = 1, end 0206BAA8 clears; daBdk_Draw 02065AB4 draws and kamen_break_move moves only pieces with it set */
    /* 0x0A20 */ cXyz mMaskPiecePos[4];  /* was m910: kamen_demo 0206B22C: piece start position (MtxPosition + dist); kamen_break_move (in daBdk_Execute 02066A38) moves it; daBdk_Delete 020692E0 deletes its sound object */
    /* 0x0A50 */ cXyz mMaskPiecePosOld[4];  /* was m940: kamen_demo 0206B22C copies mMaskPiecePos; kamen_break_move (in daBdk_Execute 02066A38) keeps last frame's piece position in it */
    /* 0x0A80 */ cXyz mMaskPieceSpeed[4];  /* was m970: kamen_demo 0206B22C sets [i] to the break direction of mask piece i (kamen_break_sd_x / z); kamen_break_move (in daBdk_Execute 02066A38) adds it to mMaskPiecePos, lowers .y by 5 per frame (gravity), bounces it on the ground and sets .x / .z from mMaskPieceSlideSpeed / mMaskPieceSlideYaw or a hit */
    /* 0x0AB0 */ csXyz mMaskPieceAngle[4];  /* was m9A0: kamen_demo 0206B22C sets the piece angle (shape_angle.y, 0x3625); kamen_break_move (in daBdk_Execute 02066A38) spins it and rotates the piece model by it */
    /* 0x0AC8 */ u8 _AC8[0x20]; /* GameCube m9B8/m9D0 (csXyz[4] each): HD layout to be measured */
    /* 0x0AE8 */ be<s8> mMaskPieceBreakTime[4];  /* was m9E8: kamen_demo 0206B22C loads kamen_break_time[i]; kamen_break_move (in daBdk_Execute 02066A38) counts it down */
    /* 0x0AEC */ be<f32> mMaskPieceRockAmp[4];  /* was m9EC: kamen_break_move (in daBdk_Execute 02066A38): set 3000 when mask piece i bounces, then eased to 0; the piece's z angle rocks by sin * it */
    /* 0x0AFC */ be<f32> mMaskPieceSlideSpeed[4];  /* was m9FC: kamen_break_move (in daBdk_Execute 02066A38): a type-8 hit sets [i] to 7 + rnd(2); while on the ground mask piece i slides at it along mMaskPieceSlideYaw and spins its y angle by mMaskPieceSpinRate * it; eased to 0, cleared on another hit or a wall */
    /* 0x0B0C */ be<s16> mMaskPieceSlideYaw[4];  /* was mA0C: kamen_break_move (in daBdk_Execute 02066A38): set from the hitting actor's shape_angle.y + rnd(+-2000) on a type-8 hit; the direction of mask piece i's mMaskPieceSlideSpeed */
    /* 0x0B14 */ dCcD_Stts mMaskPieceStts;  /* was mA14: daBdk_Create 02069C8C: Init(0xFF, 0xFF), the Stts of the 4 mMaskPieceSph */
    /* 0x0B50 */ dCcD_Sph mMaskPieceSph[4];  /* was mA50: daBdk_Create 02069C8C: Set(kamen_sph_src) x4; kamen_break_move (in daBdk_Execute 02066A38) uses sphere [i] for piece [i] */
    /* 0x1000 */ be<f32> mMaskPieceSpinRate[4];  /* was mF00: kamen_break_move (in daBdk_Execute 02066A38): set rnd(+-200) on a type-8 hit; mask piece i's y angle turns by it * mMaskPieceSlideSpeed per frame */
    /* 0x1010 */ be<u8> mLookAtPlayer;  /* was mF10: fly_attack 0206A610, t_fly 0206C3AC and fly / landing / wait / jump / jida_attack (in move 0206D644) set 1 each frame; daBdk_Execute 02066A38 then eases mNeckAngleX toward the player's direction (clamped to +-7000), otherwise back to 0, and clears it */
    /* 0x1011 */ u8 _1011;
    /* 0x1012 */ be<s16> mNeckAngleX;  /* was mF12: daBdk_Execute 02066A38 eases it toward the neck angle; nodeCallBack 02065794 XrotM by it */
    /* 0x1014 */ be<f32> mTargetOffset;  /* was mF18: fly_attack 0206A610: cM_rndFX(100), added to the player's x and z for mTargetPos */
    /* 0x1018 */ dBgS_AcchCir mAcchCir;
    /* 0x1058 */ dBgS_ObjAcch mAcch;
    /* 0x121C */ be<s16> mTosakaYaw;  /* was m1120: head_swing (in daBdk_Execute 02066A38) eases it to the head-velocity angle + a sway + 4500; nodeCallBack 02065794 rotates the crest joints (TOSAKA_A1 .. B2) about y by it (A side negated); daBdk_Create 02069C8C sets -2000 */
    /* 0x121E */ be<s16> mTosakaRoll;  /* was m1122: head_swing (in daBdk_Execute 02066A38) eases it to 4; nodeCallBack 02065794 rotates the crest joints about z by it (A side negated) */
    /* 0x1220 */ be<s16> mTosakaWobbleYawB;  /* was m1124: head_swing (in daBdk_Execute 02066A38): sin(mTosakaPhase * 1500) * mTosakaWobbleAmp; nodeCallBack 02065794 adds it to the y rotation of the B crest joints */
    /* 0x1222 */ be<s16> mTosakaWobbleRollB;  /* was m1126: head_swing (in daBdk_Execute 02066A38): cos(mTosakaPhase * 1200) * mTosakaWobbleAmp; nodeCallBack 02065794 adds it to the z rotation of the B crest joints */
    /* 0x1224 */ be<s16> mTosakaWobbleYawA;  /* was m1128: head_swing (in daBdk_Execute 02066A38): sin(mTosakaPhase * 1500 + 0x4000) * mTosakaWobbleAmp; nodeCallBack 02065794 adds it to the (negated) y rotation of the A crest joints */
    /* 0x1226 */ be<s16> mTosakaWobbleRollA;  /* was m112A: head_swing (in daBdk_Execute 02066A38): cos(mTosakaPhase * 1200 + 0x4000) * mTosakaWobbleAmp; nodeCallBack 02065794 adds it to the (negated) z rotation of the A crest joints */
    /* 0x1228 */ be<s16> mTosakaHitRoll;  /* was m112C: head_swing (in daBdk_Execute 02066A38): while mTosakaHitTimer runs, sin(frame * 7000) * the x of the hit direction (mHitAngleY) * timer * 500; nodeCallBack 02065794 adds it to the z rotation of all crest joints */
    /* 0x122A */ be<s16> mTosakaHitYaw;  /* was m112E: head_swing (in daBdk_Execute 02066A38): while mTosakaHitTimer runs, sin(frame * 7000) * the z of the hit direction (mHitAngleY) * timer * 500; nodeCallBack 02065794 adds it to the y rotation of all crest joints */
    /* 0x122C */ be<s16> mTosakaPhase;  /* was m1130: head_swing (in daBdk_Execute 02066A38) adds 1 per frame (2 while mTosakaWobbleTarget is above 3000): the phase of the crest sway and wobble terms */
    /* 0x122E */ be<s16> mTosakaWobbleAmp;  /* was m1132: head_swing (in daBdk_Execute 02066A38) eases it to mTosakaWobbleTarget (step 500) and multiplies the four wobble terms by it; cleared while a hit shake runs or the swing is off */
    /* 0x1230 */ be<s16> mTosakaWobbleTarget;  /* was m1134: head_swing (in daBdk_Execute 02066A38) eases mTosakaWobbleAmp to it and resets it to 1500 each frame; kuti_attack (in move 0206D644) sets 2500 each frame of the attack, kamen_demo 0206B22C 500, end 0206BAA8 4000 */
    /* 0x1232 */ be<s16> mHitAngleY;  /* was m1136: damage_check (in daBdk_Execute 02066A38): atan2 from the attacker to the actor; head_swing YrotS by it; kuti_attack (in move 0206D644) clears it */
    /* 0x1234 */ be<s16> mTosakaHitTimer;  /* was m1138: damage_check (in daBdk_Execute 02066A38) sets 30 on a crest hit (0 on a mask hit), kuti_attack (in move 0206D644) 30 at the beak strike frame; daBdk_Execute counts it down; head_swing shakes the crest by it (mTosakaHitRoll / mTosakaHitYaw) */
    /* 0x1236 */ be<s8> mTosakaSwingOn;  /* was m113A: kamen_demo 0206B22C sets 1 once the mask is broken; head_swing (in daBdk_Execute 02066A38) moves the crest only while set and clears the crest terms otherwise */
    /* 0x1237 */ u8 _1237;
    /* 0x1238 */ be<f32> mTosakaTurnRate;  /* was m113C: head_swing (in daBdk_Execute 02066A38) eases it from 0 to 1; mTosakaYaw / mTosakaRoll move at most 4000 * it per frame */
    /* 0x123C */ be<f32> mTosakaSwayScale;  /* was m1140: head_swing (in daBdk_Execute 02066A38) eases it from 0 to 1 once mDemoTimer passes 0x41; the sway term of mTosakaYaw is sin * -2000 * it */
    /* 0x1240 */ cXyz mHeadPosOld;  /* was m1144: col_set (in daBdk_Execute 02066A38) copies mHeadPos into it before updating mHeadPos; head_swing takes mHeadPos - it as the head movement */
    /* 0x124C */ cXyz mHeadPos;  /* was m1150: col_set (in daBdk_Execute 02066A38): the head point (ATAMA1 joint + offset), centre of mHeadTgSph; damage_check tests .y < 10000; demo_camera 0206FEF0 centres the camera on it */
    /* 0x1258 */ cXyz mHeadAtPos;  /* was m115C: col_set (in daBdk_Execute 02066A38): MtxPosition of the head point (y -10000 when off), centre of mHeadAtSph */
    /* 0x1264 */ cXyz mBodyCenter;  /* was m1168: col_set (in daBdk_Execute 02066A38): current.pos + 380 in y, centre of mBodyCCSph; fly_attack 0206A610, wind_attack 0206AE64, t_down 0206D0FC, landing / kuti_attack (in move 0206D644) spawn feathers there (eff_hane_set) */
    /* 0x1270 */ cXyz mFootPos[2];  /* was m1174: col_set (in daBdk_Execute 02066A38): MtxPosition per foot, centres of mFootCCSph[i]; fly_attack 0206A610 copies them to foot_eff_pos */
    /* 0x1288 */ dCcD_Stts mStts;
    /* 0x12C4 */ dCcD_Sph mHeadAtSph;
    /* 0x13F0 */ dCcD_Sph mHeadTgSph;
    /* 0x151C */ dCcD_Sph mTosakaTgSph;
    /* 0x1648 */ dCcD_Sph mBodyCCSph;
    /* 0x1774 */ dCcD_Sph mFootCCSph[2];
    /* 0x19CC */ dCcD_Sph mWindAtSph[10];
    /* 0x2584 */ be<u8> mWindAge[10];  /* was m2488: kankyo_cont 0206F9E0: per wind sphere, 0 = off, +1 per frame, cleared when old */
    /* 0x258E */ u8 _258E[2];
    /* 0x2590 */ cXyz mWindPos[10];  /* was m2494: wind_set 02066798 sets the start; kankyo_cont 0206F9E0 adds mWindSpeed and moves mWindAtSph[i] to it */
    /* 0x2608 */ cXyz mWindSpeed[10];  /* was m250C: wind_set 02066798: MtxPosition of the wind direction; kankyo_cont 0206F9E0 adds it to mWindPos each frame */
    /* 0x2680 */ be<u8> mAttackPart;  /* was m2584: the attack sphere for this frame: t_fly 0206C3AC, t_lastattack 0206CAD8 and kuti_attack (in move 0206D644) set 1, col_set (in daBdk_Execute 02066A38) then places mHeadAtPos at the jaw (AGO); fly_attack 0206A610, t_fly and landing / jida_attack (in move) set 2, col_set then turns the foot spheres' At on; daBdk_Execute clears it every frame */
    /* 0x2681 */ be<u8> mFootAtStarted;  /* was m2585: col_set (in daBdk_Execute 02066A38): 0 -> StartCAt of the foot spheres and adds 1, otherwise MoveCAt; cleared while mAttackPart is not 2 */
    /* 0x2682 */ be<s8> mGroundTimer;  /* was m2586: daBdk_Execute 02066A38 sets 2 on a ground hit (HD; GameCube 3) and counts it down off the ground; wait (in move 0206D644) sets 2, jump clears it; landing (in move) stops on it, damage_check (in daBdk_Execute) plays DAMAGE3 and returns to ACTION_WAIT while it is set */
    /* 0x2683 */ u8 _2683;
    /* 0x2684 */ be<s32> mBaseBck;  /* was m2588: anm_init 02065620 stores the bck index when param7 is 0; move 0206D644 restarts it (anm_init) after a one-shot animation */
    /* 0x2688 */ be<s32> mBaseSound;  /* was m258C: anm_init 02065620 stores the sound index with mBaseBck; move 0206D644 passes it back to anm_init */
    /* 0x268C */ be<u8> mBaseLoopMode;  /* was m2590: anm_init 02065620 stores the loop mode with mBaseBck; move 0206D644 passes it back to anm_init */
    /* 0x268D */ be<u8> mReturnToBase;  /* was m2591: anm_init 02065620 sets 1 for a one-shot (param7 != 0), 0 otherwise; move 0206D644 restarts mBaseBck when the one-shot stops, then clears it */
    /* 0x268E */ be<s8> mWaitCount;  /* was m2592: wait (in move 0206D644) adds 1, leaves the wait when it reaches 2 (mask not broken); wait_to_up_fly clears it */
    /* 0x268F */ be<s8> mFlyCount;  /* was m2593: fly (in move 0206D644) adds 1 per pass, clears it; >= 4 ends the fly */
    /* 0x2690 */ be<s8> mHitCount;  /* was m2594: damage_check (in daBdk_Execute 02066A38) adds 1 per hit; wait (in move 0206D644) leaves at >= 5; wait_to_up_fly clears it */
    /* 0x2691 */ u8 _2691[3];
    /* 0x2694 */ gptr<fopAc_ac_c> mp2598;
    /* 0x2698 */ u8 m259C[2];
    /* 0x269A */ be<s16> mBlurRate;  /* was m259E: daBdk_Draw 02065AB4 sets l_blur.mRate to it while above 1 and clears it at 1; t_down 0206D0FC sets 0xB4 and eases it to 1; demo_camera 0206FEF0 clears it (as bgn_class mBlurRate) */
    /* 0x269C */ be<s16> mDemoMode;  /* was m25A0: the cutscene step: kamen_demo 0206B22C / start 0206B67C / t_down 0206D0FC / end 0206BAA8 set 1, 10, 0x32, 0x6E ...; demo_camera 0206FEF0 switches on it, runs the camera while it is not 0 and reports it as DEMO MODE (BdkDemo_reports().demoMode) */
    /* 0x269E */ be<s8> mFinalHitHigh;  /* was m25A2: damage_check (in daBdk_Execute 02066A38) (end_set) sets 1 when health runs out while current.pos.y is at least 9810 (end state 2); demo_camera 0206FEF0 then lets the eye height follow the boss instead of keeping 9950 */
    /* 0x269F */ u8 _269F;
    /* 0x26A0 */ be<s16> mDemoMainCount;  /* was m25A4: kamen_demo 0206B22C, start 0206B67C and demo_camera 0206FEF0 clear it when a cutscene starts; demo_camera adds 1 per frame, uses it as the phase of the camera shake and reports it as K MAIN COUNT (tww JUTReport "K MAIN COUNT  %d") */
    /* 0x26A2 */ be<s16> mDemoTimer;  /* was m25A6: kamen_demo 0206B22C clears it and compares it with frame marks (0x2D, 0x6E); demo_camera 0206FEF0 reads it */
    /* 0x26A4 */ cXyz mDemoCamEye;  /* was m25A8: demo_camera 0206FEF0: copied to camera->mEye and passed to dCamera_Set_bdk as the eye */
    /* 0x26B0 */ cXyz mDemoCamCenter;  /* was m25B4: demo_camera 0206FEF0: copied to camera->mCenter and passed to dCamera_Set_bdk as the centre */
    /* 0x26BC */ csXyz mDemoCamAngle;  /* was m25C0: demo_camera 0206FEF0 (t_down cutscene): the yaw (.y) and pitch (.x) from the boss to the camera eye, then YrotS / XrotM of the eye offset (mDemoCamDist) around the boss */
    /* 0x26C2 */ u8 _26C2[2];
    /* 0x26C4 */ be<f32> mDemoCamFovy;  /* was m25C8: kamen_demo 0206B22C sets 45; demo_camera 0206FEF0 sets / eases 25..60 and passes it to dCamera_Set_bdk as the fovy */
    /* 0x26C8 */ be<f32> mDemoCamSpeed;  /* was m25CC: demo_camera 0206FEF0: clears it and scales the cLib_addCalc2 steps of mDemoCamCenter / mDemoCamEye by it */
    /* 0x26CC */ be<f32> mDemoCamDist;  /* was m25D0: demo_camera 0206FEF0 (t_down cutscene) sets 1500 and uses it as the z of the eye offset rotated by mDemoCamAngle */
    /* 0x26D0 */ be<f32> mDemoShakeAmp;  /* was m25D4: t_down 0206D0FC sets 10, demo_camera 0206FEF0 sets 0.75 / 2.5 (HD; GameCube 1.5 / 5.0) while the shutters close; demo_camera scales the sin/cos camera shake by it above 0.1 and eases it to 0 */
    /* 0x26D4 */ be<s8> mDemoStepReq;  /* was m25D8: demo_camera 0206FEF0 sets 1 at given cutscene frames; start 0206B67C goes to its next state (fly up, then the S_DEMO2 animation) when it is set and clears it */
    /* 0x26D5 */ u8 _26D5[3];
    /* 0x26D8 */ cXyz m25DC;
    /* 0x26E4 */ cXyz m25E8;
    /* 0x26F0 */ cXyz m25F4;
    /* 0x26FC */ u8 m2600[8];
    /* 0x2704 */ be<f32> m2608;
    /* 0x2708 */ cXyz mTakeoffSmokePos;  /* was m260C: daBdk_Execute 02066A38: current.pos with y 9800, position of the TOBITATI_SMOKE00 (take-off smoke) particle */
    /* 0x2714 */ be<s8> mTakeoffSmokeTimer;  /* was m2618: wind_attack 0206AE64, up_fly / jump (in move 0206D644) set 0x19; daBdk_Execute 02066A38 counts it down and at 0 starts ID_IT_ST_DK_TOBITATI_SMOKE00 (the take-off smoke) at mTakeoffSmokePos with mTakeoffSmokeCb */
    /* 0x2715 */ be<s8> mClawSparkOn;  /* was m2619: fly_attack 0206A610 starts the ID_IT_SN_DK_TSUME_HIBANA claw-spark emitters (mp6214) at the feet on the first ground hit and sets it; daBdk_Execute 02066A38 plays JA_SE_CM_DK_NAIL and moves the foot effect points while it is set, and clears it (ending the emitters) when the feet leave the ground (y > 9810) or mAttackPart is 0 */
    /* 0x2716 */ be<s8> mEffNext;  /* was m261A: eff_hane_set 02065278: next slot of mEff to use */
    /* 0x2717 */ u8 _2717;
    /* 0x2718 */ bdk_eff_s mEff[40];  /* was m261C: daBdk_Draw 02065AB4 / my_effect_move 02071A68 loop over the 40 bdk_eff_s feather effects; eff_hane_set 02065278 fills them */
    /* 0x6138 */ dCcD_Stts mEffStts;  /* was m603C: daBdk_Create 02069C8C: Init(0xFF, 0xFF), the Stts of each mEff[i].mSph sphere */
    /* 0x6174 */ be<s16> mSmokeWaitTimer[4];  /* was m6078: daBdk_Execute 02066A38 counts all 4 down; [i] gates the toon smoke started with mSmokeCb[i]: [0] the landing smoke (landing in move 0206D644, t_lastattack 0206CAD8, 100), [1] the stamping smoke (jida_attack in move, 10), [2] the mouth-rock smoke (kuti_attack in move, 0x3C), [3] the wall-hit smoke (fly_attack 0206A610, 100) */
    /* 0x617C */ dPa_smokeEcallBack_bdk mSmokeCb[4];  /* was m6080: bdk_class_ct 02069964 / bdk_class_dt 020726D0 construct and destroy the 4 dPa_smokeEcallBack; daBdk_Create 02069C8C turns follow off, daBdk_Delete 020692E0 removes them; the ID_IT_ST_DK_TYAKUTI / JIDANDA / KUTI / BUTSUKARI smokes use [0] .. [3] (gated by mSmokeWaitTimer[i]) */
    /* 0x61FC */ gptr<JPABaseEmitter> mpDeadHaneEmitter[2];  /* was m6100: end 0206BAA8: particle_set(ID_IT_SN_DK_DEAD_HANE_A00) emitters, cleared when done */
    /* 0x6204 */ u8 _6204[8];
    /* 0x620C */ dPa_smokeEcallBack_bdk mTakeoffSmokeCb;  /* was m6110: bdk_class_ct 02069964 constructs it; daBdk_Execute 02066A38 starts ID_IT_ST_DK_TOBITATI_SMOKE00 with it when mTakeoffSmokeTimer ends; daBdk_Create 02069C8C turns follow off, daBdk_Delete 020692E0 removes it */
    /* 0x622C */ dPa_smokeEcallBack_bdk mLidSmokeCb[4];  /* was m6130: bdk_class_ct 02069964 / bdk_class_dt 020726D0 construct and destroy the 4 dPa_smokeEcallBack; demo_camera 0206FEF0 starts the ID_IT_ST_DK_FUTATOJI_SMOKE_A / B / C smokes (the tower lid closing) with them and ends [3]; daBdk_Create 02069C8C turns follow off, daBdk_Delete 020692E0 removes them */
    /* 0x62AC */ dPa_followEcallBack mDemoFeatherCb;  /* was m61B0: bdk_class_ct 02069964 constructs the dPa_followEcallBack; start 0206B67C starts ID_IT_SN_DK_DEMO_HANE00 (the demo feathers) with it and ends it; daBdk_Delete 020692E0 removes it */
    /* 0x62C0 */ dPa_followEcallBack m61C4[4];
    /* 0x6310 */ gptr<JPABaseEmitter> mp6214[4];
    /* 0x6320 */ dKy_tevstr_c mBgTevStr;  /* was m6224: bdk_class_ct 02069964 constructs it, daBdk_Create 02069C8C copies tevStr into it; daBdk_Draw 02065AB4 sets it up as TEV_TYPE_BG3 (obj_draw) and lights the arena models (mp62D8, mp6310) and the mouth rock (mEff type 2) with it; landing / jida_attack / kuti_attack (in move 0206D644) and t_lastattack 0206CAD8 tint the rock particles with its mColorK0 */
    /* 0x64E8 */ be<f32> mTogeOffsetY;  /* was m62D4: obj_move 0206FC6C: y offset of the S_TTOGE arena model (mp62D8) and its pm_bgw; eased to -500 once STAGE_BOSS_ENEMY (dungeon item 3) is set; daBdk_Create 02069C8C sets -500 after the fight */
    /* 0x64EC */ gptr<J3DModel> mp62D8;
    /* 0x64F0 */ Mtx34 mBgwMtx;  /* was m62DC: useHeapInit 02069428: cBgW_Set(pm_bgw, ..., &it); obj_move 0206FC6C copies the object matrix into it */
    /* 0x6520 */ gptr<dBgW> pm_bgw;
    /* 0x6524 */ gptr<J3DModel> mp6310[3];
    /* 0x6530 */ be<f32> mShutterRotY;  /* was m631C: obj_move 0206FC6C: MtxRotY of each S_TSHUTTER model (mp6310[3]) inside its 120-degree slot; demo_camera 0206FEF0 adds 0.003 per frame while the shutters close and resets it to 0 */
    /* 0x6534 */ be<f32> mShutterSlide;  /* was m6320: obj_move 0206FC6C: z offset (MtxTrans) of each S_TSHUTTER model (mp6310[3]) after mShutterRotY; daBdk_Create 02069C8C sets -630 (open), demo_camera 0206FEF0 eases it to 0, sets -100 and eases it to 0 again (closed) */
    /* 0x6538 */ be<f32> mShutterOffsetY;  /* was m6324: obj_move 0206FC6C: y offset (+ 9800) of the S_TSHUTTER group (mp6310[3]); daBdk_Create 02069C8C sets -70, demo_camera 0206FEF0 eases it to 0 once mShutterSlide is within 1 */
    /* 0x653C */ u8 m6328[4];
    /* 0x6540 */ Mtx34 mObjBgwMtx[3];  /* was m632C: useHeapInit 02069428: cBgW_Set(mp63BC[i], ..., &it[i]); obj_move 0206FC6C copies each object matrix into it */
    /* 0x65D0 */ gptr<dBgW> mp63BC[3];
    /* 0x65DC */ be<u32> mp63C8; /* JntHit_c* */
};
WWHD_OFFSET(bdk_class, mPhase, 0x3C8);
WWHD_OFFSET(bdk_class, mTails, 0x414);
WWHD_OFFSET(bdk_class, mMaskPieceStts, 0xB14);
WWHD_OFFSET(bdk_class, mAcch, 0x1058);
WWHD_OFFSET(bdk_class, mTosakaYaw, 0x121C);
WWHD_OFFSET(bdk_class, mWindAtSph, 0x19CC);
WWHD_OFFSET(bdk_class, mEff, 0x2718);
WWHD_OFFSET(bdk_class, mBgTevStr, 0x6320);
WWHD_OFFSET(bdk_class, mp63C8, 0x65DC);
WWHD_SIZE(bdk_class, 0x65E0);

/* daBdk_HIO_c (0x2C; HD: the JORReflexible vtable is at the end, +0x28) */
struct daBdk_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> mStopMove;  /* was m005: daBdk_Execute 02066A38 skips the animation play, move, ground check and damage_check while set; daBdk_HIO_c_ct 0206A43C sets 0 (tww the same) */
    /* 0x02 */ be<s16> m006;
    /* 0x04 */ be<f32> mBodyScale;  /* was m008: daBdk_Execute 02066A38 sets actor scale and model scale to it (0.85 * it in the tower battle, mTowerBattle); daBdk_HIO_c_ct 0206A43C sets 1.0 (tww the same) */
    /* 0x08 */ be<f32> mTailScale;  /* was m00C: daBdk_Draw 02065AB4 (tail_draw) scales each tail feather part by tial_scale * it; tail_control (in daBdk_Execute 02066A38) multiplies the part spacing by it; daBdk_HIO_c_ct 0206A43C sets 4.0 (tww the same) */
    /* 0x0C */ be<f32> mTailSpacing;  /* was m010: tail_control (in daBdk_Execute 02066A38): the feather part spacing is 2 * 20 * (0.25 + 0.03 * i) * mTailScale * it; daBdk_HIO_c_ct 0206A43C sets 1.5 (tww the same) */
    /* 0x10 */ be<u8> mForceAttack;  /* was m014: wait (in move 0206D644): when the wait ends, 1 always starts kuti_attack, 2 always wind_attack, 0 picks by distance; daBdk_HIO_c_ct 0206A43C sets 0 (tww the same) */
    /* 0x11 */ u8 _11[3];
    /* 0x14 */ be<f32> mWalkSpeed;  /* was m018: wait (in move 0206D644) state 2 (walking to the player) sets mTargetSpeed to it; daBdk_HIO_c_ct 0206A43C sets 20 (tww the same) */
    /* 0x18 */ be<f32> mWalkAnmSpeed;  /* was m01C: wait (in move 0206D644) starts the WALK1 animation with it as the play speed; daBdk_HIO_c_ct 0206A43C sets 2.0 (tww the same) */
    /* 0x1C */ be<f32> mFlySpeed;  /* was m020: fly (in move 0206D644) sets mTargetSpeed to it; daBdk_Create 02069C8C starts with speedF = it; daBdk_HIO_c_ct 0206A43C sets 60 (tww the same) */
    /* 0x20 */ be<f32> mFlyAttackSpeed;  /* was m024: fly_attack 0206A610 sets mTargetSpeed to it while it chases the player; daBdk_HIO_c_ct 0206A43C sets 100 (tww the same) */
    /* 0x24 */ be<s16> mTowerAttackWait;  /* was m028: t_fly 0206C3AC (tower battle) sets mTimers[0] to it after starting a foot attack (ATTACK_T2), the wait before the next one; daBdk_HIO_c_ct 0206A43C sets 0x2D0 (tww the same) */
    /* 0x26 */ u8 _26[2];
    /* 0x28 */ be<u32> __vtbl;
};
WWHD_SIZE(daBdk_HIO_c, 0x2C);
#define l_HIO (*gabi::at<daBdk_HIO_c>(0x1046191C))
#define DABDK_HIO_VTBL 0x10007F48
#define BDK_VTBL 0x10007F58          /* bdk_class vtable (HD virtual destructor) */
#define BDK_SAFESTRING_VTBL 0x10007E60 /* this TU's sead::SafeString vtable */

/* ---- local bindings and HD inlines ---- */
#define STR_BDK_ANM STR(0x10007FE8) /* "Bdk" (anm_init) */
#define REG8_F(i) REG_F(8, i)
#define REG6_F(i) REG_F(6, i)
#define REG12_F(i) REG_F(12, i)
#define REG13_F(i) REG_F(13, i)
#define REG14_F(i) REG_F(14, i)
#define REG0_F_(i) REG_F(0, i)
/* statics (.bss) */
#define center_pos (*gabi::at<cXyz>(0x104618F8))
#define center_pos2 (*gabi::at<cXyz>(0x10461904))
#define wind_se_pos (*gabi::at<cXyz>(0x10461910))
/* mDoExt_McaMorf (HD): model +0x90, frame +0x9C */
static inline J3DModel* morf_model(mDoExt_McaMorf* m) { return gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(m) + 0x90)); }
static inline f32 morf_frame(mDoExt_McaMorf* m) { return gabi::load<f32>(gabi::ea(m) + 0x9C); }
static inline void morf_setAnm(mDoExt_McaMorf* m, void* anm, s32 loopMode, f32 morf, f32 speed, f32 start, f32 end, void* sound) {
    gabi::call(0x025E4A98, m, anm, loopMode, morf, speed, start, end, sound);
}
/* camera_process_class* dComIfGp_getCamera(0): play+0x5AF8; view.mLookat.mEye at +0xDC */
static inline u32 bdk_camera0() { return gabi::load<u32>(dComIfGp_ea() + 0x5AF8); }
/* J3D (HD): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, joint block at model+0x2C */
struct J3DMtxBlock_bdk {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_bdk {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_bdk> mpMtxBlock;
    /* 0x30 */ u8 _30[0xAC - 0x30];
    /* 0xAC */ gptr<J3DModelData> mModelData;
    /* 0xB0 */ u8 _B0[8];
    /* 0xB8 */ be<u32> mUserArea;
};
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty) */
static inline Mtx34* bdk_getAnmMtx(J3DModel_bdk* m, s32 jnt) {
    J3DMtxBlock_bdk* blk = m->mpMtxBlock;
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30);
}
static inline J3DModel_bdk* bdk_j3dSys_getModel() { return gabi::at<J3DModel_bdk>(gabi::load<u32>(0x104B462C)); }
static inline Mtx34* bdk_J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
/* virtual remove() (vtable +0x44) of the particle callbacks */
template <class CB> static inline void bdk_vremove(CB* cb) {
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(cb)) + 0x44), cb);
}
/* dKy_tevstr_c (HD): three light blocks of 0x44 bytes at +0x00, +0xC0, +0x144 (floats 0..0x14,
 * bytes 0x18..0x1B, shorts 0x1C..0x22, floats 0x24..0x40) */
/* float members are copied bit-exactly (an lfs/stfs pair keeps a signalling NaN) */
static inline void bdk_fbits(u32 d, u32 s) { gabi::store<u32>(d, gabi::load<u32>(s)); }
static inline void bdk_tev_light_copy(u32 d, u32 s) {
    for (int k = 0; k < 0x18; k += 4) bdk_fbits(d + k, s + k);
    for (int k = 0x18; k < 0x1C; k++) gabi::store<u8>(d + k, gabi::load<u8>(s + k));
    for (int k = 0x1C; k < 0x24; k += 2) gabi::store<s16>(d + k, gabi::load<s16>(s + k));
    for (int k = 0x24; k < 0x44; k += 4) bdk_fbits(d + k, s + k);
}
/* dKy_tevstr_c::operator= (HD, inline): the three light blocks and the members at 0x84..0xBC */
static inline void bdk_tevstr_copy(dKy_tevstr_c* dst, const dKy_tevstr_c* src) {
    u32 d = gabi::ea(dst), s = gabi::ea(src);
    bdk_tev_light_copy(d, s);
    for (int k = 0x84; k < 0x90; k += 4) gabi::store<u32>(d + k, gabi::load<u32>(s + k));
    for (int k = 0x90; k < 0x98; k += 2) gabi::store<u16>(d + k, gabi::load<u16>(s + k));
    for (int k = 0x98; k < 0xA0; k++) gabi::store<u8>(d + k, gabi::load<u8>(s + k));
    for (int k = 0xA0; k < 0xA8; k += 2) gabi::store<u16>(d + k, gabi::load<u16>(s + k));
    for (int k = 0xA8; k < 0xB4; k += 4) bdk_fbits(d + k, s + k);
    for (int k = 0xB4; k < 0xBD; k++) gabi::store<u8>(d + k, gabi::load<u8>(s + k));
    bdk_tev_light_copy(d + 0xC0, s + 0xC0);
    bdk_tev_light_copy(d + 0x144, s + 0x144);
}
/* dComIfGs_isStageBossEnemy / onStageBossEnemy (HD: dungeon item bit 3 of the save's memory bits,
 * *(0x101F84DC) + 0x798) */
static inline u32 bdk_memBit() { return gabi::load<u32>(0x101F84DC) + 0x798; }
static inline BOOL dSv_memBit_isDungeonItem(u32 mb, s32 i) { return gabi::call<BOOL>(0x025B9100, mb, i); }
static inline void dSv_memBit_onDungeonItem(u32 mb, s32 i) { gabi::call(0x025B9098, mb, i); }
static inline void mDoAud_bgmStart(u32 id) { gabi::call(0x025E18EC, id); }
/* dPa_smokeEcallBack::setFollowOff: +0x12 = 1 */
static inline void smoke_setFollowOff(dPa_smokeEcallBack_bdk* cb) { gabi::store<u8>(gabi::ea(cb) + 0x12, 1); }
/* float copies through FPRs without arithmetic keep the bits (lfs/stfs) */
static inline void bdk_fcopy(be<f32>& d, const be<f32>& s) { gabi::store<u32>(gabi::ea(&d), gabi::load<u32>(gabi::ea(&s))); }
/* matrix copy: GHS loads all twelve values into FPRs, then stores them (float stores: the
 * recompiled original quiets some signalling NaNs on the way, the harness's NaN tolerance covers it) */
static inline void bdk_mtx_copy(Mtx34* dst, const Mtx34* src) {
    f32 t[12];
    for (int i = 0; i < 12; i++) t[i] = gabi::load<f32>(gabi::ea(src) + 4 * i);
    for (int i = 0; i < 12; i++) gabi::store<f32>(gabi::ea(dst) + 4 * i, t[i]);
}
static inline void bdk_setBaseTRMtx(J3DModel* m, const Mtx34* src) { bdk_mtx_copy(gabi::at<Mtx34>(gabi::ea(m) + 0xC8), src); }
/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void MtxRotY(f32 rad, u8 concat) { gabi::call(0x0200FBA4, rad, concat); }
static inline void dCcD_Sph_StartCAt(dCcD_Sph* s, cXyz* c) { gabi::call(0x025167C0, s, c); }
static inline void dCcD_Sph_MoveCAt(dCcD_Sph* s, cXyz* c) { gabi::call(0x025167E4, s, c); }
/* functions of the unit called across its source files (WWHD_FUNC turns the calls into guest calls) */
void eff_hane_set(bdk_class* i_this, cXyz* offset, int param_3, s8 param_4);
void anm_init(bdk_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx, u8 param7);
void pos_move(bdk_class* i_this);
void ground_move(bdk_class* i_this);
void wind_set(bdk_class* i_this, cXyz* param2);
void kankyo_cont(bdk_class* i_this);
void obj_move(bdk_class* i_this);
static inline void mDoAud_monsSeStart(u32 id, cXyz* pos, u32 pid, u32 param, s32 reverb) { gabi::call(0x025E1AA4, id, pos, pid, param, reverb); }
/* fopAcM_seStart (HD inline: tests &eyePos only) */
static inline void bdk_seStart(fopAc_ac_c* a, u32 id, u32 param) {
    cXyz* eye = &a->eyePos;
    if (gabi::ea(eye) != 0) mDoAud_seStart(id, eye, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* fopAcM_monsSeStart (HD inline: tests the actor and &eyePos) */
static inline void bdk_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s32 room = fopAcM_GetRoomNo(a);
        u32 pid = a != nullptr ? gabi::load<u32>(gabi::ea(a) + 4) : 0xFFFFFFFFu; /* fopAcM_GetID */
        mDoAud_monsSeStart(id, &a->eyePos, pid, param, dComIfGp_getReverb(room));
    }
}
/* JPABaseEmitter::setGlobalRTMatrix (inline) */
static inline void bdk_setGlobalRTMatrix(JPABaseEmitter* e, Mtx34* m) {
    gabi::call(0x028249B0, m, gabi::ea(e) + 0x1F0, gabi::ea(e) + 0x22C); /* JPASetRMtxTVecfromMtx */
}
static inline Mtx34* bdk_morfAnmMtx(bdk_class* i_this, s32 jnt) { return bdk_getAnmMtx((J3DModel_bdk*)morf_model(i_this->mpMorf), jnt); }
/* tevStr.mColorK0 (HD +0x98) */
static inline GXColor* bdk_colorK0(fopAc_ac_c* a) { return gabi::at<GXColor>(gabi::ea(a) + 0x1A8); }
static inline void dComIfGs_offSwitch_bdk(s32 no, s32 roomNo) { gabi::call(0x025B9F7C, gabi::load<u32>(0x101F84DC) + 0x20, no, roomNo); }
/* mDoExt_McaMorf frame control (HD +0x98): rate +0x98, frame +0x9C */
static inline void morf_setPlaySpeed(mDoExt_McaMorf* m, f32 v) { gabi::store<f32>(gabi::ea(m) + 0x98, v); }
static inline void morf_setFrame(mDoExt_McaMorf* m, f32 v) { gabi::store<f32>(gabi::ea(m) + 0x9C, v); }
/* fopAcM_monsSeStart (HD inline variant: tests &eyePos only) */
static inline void bdk_monsSeStart_e(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0) {
        s32 room = fopAcM_GetRoomNo(a);
        u32 pid = a != nullptr ? gabi::load<u32>(gabi::ea(a) + 4) : 0xFFFFFFFFu;
        mDoAud_monsSeStart(id, &a->eyePos, pid, param, dComIfGp_getReverb(room));
    }
}
/* fopAcM_seStart (HD inline variant: tests the actor and &eyePos) */
static inline void bdk_seStart_a(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline void dComIfGs_onSwitch_bdk(s32 no, s32 roomNo) { gabi::call(0x025B9E38, gabi::load<u32>(0x101F84DC) + 0x20, no, roomNo); }
/* dComIfGp_particle_setToon: dPa_control_c::set with group 2 */
static inline JPABaseEmitter* bdk_particle_setToon(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha, void* cb, s8 setup) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, angle, scale, alpha, (dPa_levelEcallBack*)cb, setup, nullptr, nullptr, nullptr);
}
/* dComIfGp_getVibration().StartShock(REG0_S(2) + add, -0x21, cXyz(0, 1, 0)): the register is read
 * after the play object */
static inline void bdk_StartShock(s32 add) {
    dVibration_c* vib = dComIfGp_getVibration();
    s32 strength = REG0_S(2) + add;
    gabi::Local<cXyz> v;
    v->x = 0.0f;
    v->y = 1.0f;
    v->z = 0.0f;
    gabi::call(0x025CB374, vib, strength, -0x21, v.get());
}
/* JPABaseEmitter::becomeInvalidEmitter (inline): +0x5C = -1, flags (+0x254) |= 1 */
static inline void bdk_becomeInvalidEmitter(JPABaseEmitter* e) {
    u32 a = gabi::ea(e);
    u32 f = gabi::load<u32>(a + 0x254);
    gabi::store<s32>(a + 0x5C, -1);
    gabi::store<u32>(a + 0x254, f | 1);
}

/* Gohma, WWHD layout.
 * HD: actor prefix adds 0x11C; the extra death-body animation triplet adds 0x0C.
 * LIGHT_INFLUENCE adds four bytes, and tev state adds 0x118 bytes. */
#pragma once
#include "bindings.h"
#include "gc_api/JSystem/J3DModelData.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"

struct dMagma_floor_c;
struct btd_hahen_s {
    /* 0x00 */ gptr<J3DModel> model;
    /* 0x04 */ be<u8> m04;
    /* 0x05 */ u8 m05[3];
    /* 0x08 */ cXyz m08;
    /* 0x14 */ cXyz m14;
    /* 0x20 */ be<s16> m20, m22;
    /* 0x24 */ u8 m24[4];
};
WWHD_SIZE(btd_hahen_s, 0x28);
struct btd_sibuki_s {
    /* 0x00 */ be<s8> m00;
    /* 0x01 */ u8 m01[3];
    /* 0x04 */ cXyz m04;
    /* 0x10 */ cXyz m10;
    /* 0x1C */ u8 m1C[8];
};
WWHD_SIZE(btd_sibuki_s, 0x24);
struct btd_class : fopEn_enemy_c {
    /* 0x03C8 */ request_of_phase_process_class mPhaseBtd;
    /* 0x03D0 */ request_of_phase_process_class mPhaseMagma;
    /* 0x03D8 */ gptr<mDoExt_McaMorf> mpPhase1Morf;
    /* 0x03DC */ gptr<mDoExt_btkAnm> btk;
    /* 0x03E0 */ gptr<mDoExt_brkAnm> brk;
    /* 0x03E4 */ gptr<mDoExt_McaMorf> mpPhase2Morf;
    /* 0x03E8 */ gptr<mDoExt_btkAnm> btkS;
    /* 0x03EC */ gptr<mDoExt_brkAnm> brkS;
    /* 0x03F0 */ gptr<mDoExt_McaMorf> mpDeadBodyMorf;
    /* 0x03F4 */ gptr<mDoExt_btkAnm> mpDeadBodyBtkAnm;
    /* 0x03F8 */ gptr<mDoExt_brkAnm> mpDeadBodyBrkAnm;
    /* 0x03FC */ gptr<mDoExt_McaMorf> mpDeadHeadMorf;
    /* 0x0400 */ gptr<mDoExt_btkAnm> mpDeadHeadBtkAnm;
    /* 0x0404 */ gptr<mDoExt_brkAnm> mpDeadHeadBrkAnm;
    /* 0x0408 */ be<u8> mBattlePhase;  /* was m02E0: get_anm 020F032C / get_btk / get_brk pick the phase-2 morf and animations when it is set; btdActionDamage (in move 020F6004) sets 1, end 020F52B8 sets 2; btd_effect 020F90C4 hides the fire chips at >= 2 */
    /* 0x0409 */ be<s8> m02E1;
    /* 0x040A */ be<s16> mFrameCounter;  /* was m02E2: daBtd_Execute 020F1494 adds 1 per frame; btdExecCamera (in daBtd_Execute 020F1494) uses it as the cos phase of the camera bank shake */
    /* 0x040C */ be<s16> mAction;  /* was m02E4: move 020F6004: switch (it) selects the action (BtdAction ACT_WAIT .. ACT_END); daBtd_Create 020F4308 sets ACT_START_DEMO, damage_check 020F4D80 sets 2 */
    /* 0x040E */ be<s16> mGohmaState;
    /* 0x0410 */ be<s8> mAttackType;
    /* 0x0411 */ be<s8> mAttackAnmIdx;  /* was m02E9: btdActionWait (in move 020F6004) sets 0 / 5; btdActionJab / btdActionPunch index jab_bck / punch_bck with it, btdActionNextAnimation adds 1 */
    /* 0x0412 */ be<s8> mAttackHand;  /* was m02EA: btdActionWait (in move 020F6004) sets 0 / 1; jab joint 10 : 8, hahen_set(side = it), btd_effect 020F90C4 picks the right-claw effects (effR_name) when it is set */
    /* 0x0413 */ be<u8> m02EB[1];
    /* 0x0414 */ be<s16> mTimers[2];  /* was m02EC: daBtd_Execute 020F1494 counts [0], [1] down with mBlinkTimer and mStunTimer; btdActionWait (in move 020F6004) loads them (150, 300, 40) */
    /* 0x0418 */ be<s16> mBlinkTimer;  /* was m02F0: daBtd_Execute 020F1494 counts it down; at 0 it reloads cM_rndF + 100 and sets mEyeCloseTimer to 20; startdemo 020F4924 sets 30 */
    /* 0x041A */ be<s16> mStunTimer;
    /* 0x041C */ be<s16> mActionState;
    /* 0x041E */ be<u8> mRockHitState;  /* was m02F6: iwa_move (in daDr2_Execute 0212B3D0) sets 1 when the falling rock lands on the btd (then waits until it is 0 again); damage_check 020F4D80 turns 1 into 2 and starts the damage action; btdActionDamage (in move 020F6004) clears it */
    /* 0x041F */ be<u8> m02F7[1];
    /* 0x0420 */ be<s16> mEyeCloseTimer;  /* was m02F8: daBtd_Execute 020F1494 sets 20 (blink, a sword cut or a weapon near the eye) and counts it down; while it is set the eyelid angle eases to 13000 */
    /* 0x0422 */ be<s16> mEyelidAngle;  /* was m02FA: daBtd_Execute 020F1494 eases it to 13000 / 0 (cLib_addCalcAngleS2); nodeCallBack 020F04EC rotates the eyelid joints by it (XrotM) */
    /* 0x0424 */ cXyz mJointPos[19];  /* was m02FC: nodeCallBack 020F04EC: MtxPosition of each joint_check role; daBtd_Execute 020F1494 sets mAtSphs[i] there, hahen_set2 020F0C68 / btdActionJab use the hand points [8] / [10] */
    /* 0x0508 */ cXyz mEyeJointPos;  /* was m03E0: nodeCallBack 020F04EC: MtxPosition of the eye joint (copied to eyePos); daBtd_Execute 020F1494 centres mEyeSph there; damage_check 020F4D80 sets the hit particles there */
    /* 0x0514 */ dCcD_Stts mStts;
    /* 0x0550 */ dCcD_Sph mAtSphs[19];
    /* 0x1B94 */ dCcD_Sph mEyeSph;
    /* 0x1CC0 */ dCcD_Sph mFireSph;
    /* 0x1DEC */ dCcD_Sph mSibukiSphs[50];
    /* 0x5884 */ dCcD_Cyl mHandCyls[6];
    /* 0x5FA4 */ be<u8> mHandCylMode;  /* was m5E7C: btdActionPunch (in move 020F6004) sets mAttackHand + 2, btdActionFire 1; daBtd_Execute 020F1494 places mHandCyls (1: all, 2: [0..2], 3: [3..5]) and clears it */
    /* 0x5FA5 */ be<u8> m5E7D[3];
    /* 0x5FA8 */ be<f32> mFireSize;  /* was m5E80: btdActionFire (in move 020F6004) grows it by 0.1 from 0; btdActionFireCollision: mFireSph radius = 200 * it */
    /* 0x5FAC */ be<u8> mAtMode;  /* was m5E84: btdActionJab (in move 020F6004) sets 1, btdActionPunch 2, btdActionFire 3, move clears it; daBtd_Execute 020F1494 turns the At of the hand spheres (1/2: Atp 1/2) or the mouth spheres (3) on by it */
    /* 0x5FAD */ be<s8> mTotalDamage;
    /* 0x5FAE */ be<u8> mLookMode;  /* was m5E86: btdActionWait / btdActionFire / btdActionHighFire (in move 020F6004) set 1..3; btdActionTrack turns the neck and head toward the player only while it is set, then clears it */
    /* 0x5FAF */ be<u8> m5E87[1];
    /* 0x5FB0 */ be<s16> mNeckAngleY;  /* was m5E88: btdActionTrack (in move 020F6004) eases it toward the player angle (cLib_addCalcAngleS2); nodeCallBack 020F04EC: YrotM / ZrotM of the neck joint by it */
    /* 0x5FB2 */ be<s16> mNeckAngleX;  /* was m5E8A: btdActionTrack (in move 020F6004) eases it to mNeckTargetAngleX; nodeCallBack 020F04EC: XrotM of the neck joint by it */
    /* 0x5FB4 */ be<s16> mNeckTargetAngleX;  /* was m5E8C: btdActionHighFire (in move 020F6004) sets -pitch - 2500; btdActionTrack eases mNeckAngleX to it, then clears it */
    /* 0x5FB6 */ be<s16> mHeadAngleY;  /* was m5E8E: btdActionTrack (in move 020F6004) eases it to half the eye-to-player angle; nodeCallBack 020F04EC: YrotM of the head (eye) joint by it */
    /* 0x5FB8 */ be<s16> mTurnTargetAngleY;  /* was m5E90: btdActionWait (in move 020F6004) sets facing + 0x4000; btdActionTrack turns current.angle.y to it (player angle when 0), then clears it */
    /* 0x5FBA */ be<s16> mTurnScale;  /* was m5E92: btdActionTurningPunch (in move 020F6004) sets 15 (+REG); btdActionTrack: scale argument of the cLib_addCalcAngleS2 turn, reset to 10 */
    /* 0x5FBC */ be<s16> mTurnMaxStep;  /* was m5E94: btdActionWait / btdActionHighFire / btdActionTurningPunch (in move 020F6004) set 0x100..0xC00; btdActionTrack turns only while it is set (max step of the turn), then clears it */
    /* 0x5FBE */ be<s16> m5E96;
    /* 0x5FC0 */ cXyz mHitSePos[25];  /* was m5E98: damage_check 020F4D80: [i] = mJointPos[i] and def_se_set_p at [i] / [i + 19]; daBtd_Execute 020F1494: [19 + i] = centre of mHandCyls[i]; daBtd_Delete 020F34A8 deletes their sound objects */
    /* 0x60EC */ be<s16> mSphHitTimer[19];  /* was m5FC4: damage_check 020F4D80: per mAtSphs[i] hit, 10 frames before the next hit sound, counted down */
    /* 0x6112 */ be<s16> mCylHitTimer[6];  /* was m5FEA: damage_check 020F4D80: per mHandCyls[i] hit, 10 frames before the next hit sound, counted down */
    /* 0x611E */ u8 _pad611E[0x2];
    /* 0x6120 */ gptr<JPABaseEmitter> mFireChipEmitter[5];  /* was m5FF8: btd_effect 020F90C4: eff_name emitters on the eff_index joints (TWW eff_name: ID_AK_SN_ARMFIRECHIP / BACKBONEFIRECHIP), moved to -10000 at mBattlePhase >= 2 */
    /* 0x6134 */ gptr<JPABaseEmitter> mAttackEmitter[7];  /* was m600C: btd_effect 020F90C4: effF_name / effR_name emitters (fire breath, claw rock and smoke) while mAttackEmitterTimer[i] runs; btdActionFire (in move 020F6004) clears [3] */
    /* 0x6150 */ be<s16> mAttackEmitterTimer[7];  /* was m6028: btdActionPunch / btdActionFire / btdActionHighFire (in move 020F6004) load it; btd_effect 020F90C4 counts [i] down and keeps mAttackEmitter[i] while it is set */
    /* 0x615E */ be<u8> m6036[2];
    /* 0x6160 */ gptr<JPABaseEmitter> mpDeadHeadEmitter;  /* was m6038: end 020F52B8: particle_set(0x80DE) at mDeadHeadPos (TWW: ID_AK_SN_BTDBREAKDOWNHEAD00), moved with the head, made invalid when the head lands */
    /* 0x6164 */ cXyz mHahenPos;  /* was m603C: hahen_set 020F0EB4: hand point - 100 in y; position of the debris (0x80A8 / 0x80A9) particles and smoke_set_s */
    /* 0x6170 */ csXyz mHahenAngle;  /* was m6048: hahen_set 020F0EB4: current.angle +- 0x4000 in y by side; angle of the debris particles and smoke_set_s */
    /* 0x6176 */ be<u8> m604E[2];
    /* 0x6178 */ dPa_smokeEcallBack mPa_SmokeEcallBack[3];
    /* 0x61D8 */ LIGHT_INFLUENCE mLightInfluence;
    /* 0x61FC */ be<f32> mKankyoBlend;  /* was m60D0: btd_kankyo 020F94DC: eased per mKankyoState (cLib_addCalc2 / addCalc0), passed to dKy_custom_colset as the blend */
    /* 0x6200 */ be<f32> mLightRate;  /* was m60D4: btd_kankyo 020F94DC: eased per mKankyoState; scales mLightInfluence colour and power */
    /* 0x6204 */ be<u8> mLightColorR;  /* was m60D8: btd_kankyo 020F94DC: l_HIO.m3C / m42; red of mLightInfluence (times mLightRate) */
    /* 0x6205 */ be<u8> mLightColorG;  /* was m60D9: btd_kankyo 020F94DC: l_HIO.m3E / m44; green of mLightInfluence (times mLightRate) */
    /* 0x6206 */ be<u8> mLightColorB;  /* was m60DA: btd_kankyo 020F94DC: l_HIO.m40 / m46; blue of mLightInfluence (times mLightRate) */
    /* 0x6207 */ be<u8> mKankyoState;
    /* 0x6208 */ be<s16> mKankyoTimer;
    /* 0x620A */ be<u8> m60DE[2];
    /* 0x620C */ dKy_tevstr_c mTevstr;
    /* 0x63D4 */ be<u8> mCrackStage;  /* was m6190: btdActionBreakShell (in move 020F6004): index into hibi_brk (crack animations), +1 per hit up to 3; btdActionDamage tests >= 3 */
    /* 0x63D5 */ be<u8> m6191[3];
    /* 0x63D8 */ btd_hahen_s hahen[35];
    /* 0x6950 */ btd_sibuki_s mBtdSibukiS[50];
    /* 0x7058 */ be<s8> mSplashPopCount;  /* was m6E14: sibuki_set 020F10B4 clears it; btdExecSplashMove (in daBtd_Execute 020F1494) adds 1 per splash pop near the player, up to 20 */
    /* 0x7059 */ be<u8> mBlurRate;  /* was m6E15: daBtd_Draw 020F0A74: mDoGph_setBlureRate(it) while > 1, off and cleared at 1; end 020F52B8 sets 180, daBtd_Execute 020F1494 50 */
    /* 0x705A */ be<s16> mDemoCamMode;  /* was m6E16: btdExecCamera (in daBtd_Execute 020F1494): switch (it) of the demo camera steps, +1 per step; 0 = no demo camera */
    /* 0x705C */ be<s16> mDemoMainCount;  /* was m6E18: btdExecCamera (in daBtd_Execute 020F1494): +1 per frame, reported as "K MAIN COUNT  %d"; the player demo modes start at its frame marks */
    /* 0x705E */ be<s16> mDemoSubCount;  /* was m6E1A: btdExecCamera (in daBtd_Execute 020F1494): +1 per frame, cleared per step, reported as "K SUB  COUNT  %d"; compared with each step's frame marks */
    /* 0x7060 */ cXyz mDemoCamEye;  /* was m6E1C: btdExecCamera (in daBtd_Execute 020F1494): set / eased per step; the eye passed to dCamera_c::Set (with shake) and Reset */
    /* 0x706C */ cXyz mDemoCamCenter;  /* was m6E28: btdExecCamera (in daBtd_Execute 020F1494): set / eased per step; the centre passed to dCamera_c::Set (with shake) and Reset */
    /* 0x7078 */ be<s16> mDemoCamOrbitAngleX;  /* was m6E34: btdExecCamera (in daBtd_Execute 020F1494): -11000, eased; XrotM before mDemoCamEye = MtxPosition(0, 0, mDemoCamOrbitDist) */
    /* 0x707A */ be<s16> mDemoCamOrbitAngleY;  /* was m6E36: btdExecCamera (in daBtd_Execute 020F1494): -6000, eased to 0x4000; YrotS before mDemoCamEye = MtxPosition(0, 0, mDemoCamOrbitDist) */
    /* 0x707C */ be<u8> m6E38[4];
    /* 0x7080 */ be<f32> mDemoCamFovy;  /* was m6E3C: btdExecCamera (in daBtd_Execute 020F1494): 50 (+REG) / 55 / 60, eased; passed to dCamera_c::Set as the fovy */
    /* 0x7084 */ be<f32> mDemoCamSpeed;  /* was m6E40: btdExecCamera (in daBtd_Execute 020F1494): cleared per step and eased up; scales the cLib_addCalc2 steps of mDemoCamEye / mDemoCamCenter */
    /* 0x7088 */ be<f32> mDemoCamOrbitDist;  /* was m6E44: btdExecCamera (in daBtd_Execute 020F1494): 2800, eased to 500; z of the orbit vector that gives mDemoCamEye */
    /* 0x708C */ be<f32> mDemoCamShake;  /* was m6E48: btdExecCamera (in daBtd_Execute 020F1494): sin / cos amplitude added to mDemoCamEye / mDemoCamCenter and the bank, decays (addCalc0); startdemo 020F4924 sets 40, end 020F52B8 30 */
    /* 0x7090 */ be<u8> mDeadHeadOn;  /* was m6E4C: end 020F52B8 sets 1 when the head comes off; daBtd_Draw 020F0A74 then draws the dead-head model at mDeadHeadPos, nodeCallBack 020F04EC stops moving eyePos */
    /* 0x7091 */ be<u8> m6E4D[3];
    /* 0x7094 */ cXyz mDeadHeadPos;  /* was m6E50: end 020F52B8: starts at eyePos, moves by mDeadHeadSpeed toward mDeadHeadTargetPos; kubi_calc 020F1288 places the dead-head model there; daBtd_Draw 020F0A74 settingTevStruct at it */
    /* 0x70A0 */ cXyz mDeadHeadTargetPos;  /* was m6E5C: btdExecCamera (in daBtd_Execute 020F1494): MtxPosition of the landing point; end 020F52B8 moves mDeadHeadPos toward it and lands at its y */
    /* 0x70AC */ cXyz mDeadHeadSpeed;  /* was m6E68: end 020F52B8: MtxPosition of the throw vector, added to mDeadHeadPos per frame, y minus gravity, bounced */
    /* 0x70B8 */ csXyz mDeadHeadAngle;  /* was m6E74: end 020F52B8: starts at current.angle, spins / eases; kubi_calc 020F1288: Y/X/ZrotM of the dead-head model by it */
    /* 0x70BE */ be<u8> m6E7A[2];
    /* 0x70C0 */ be<f32> mWaveRadius;  /* was m6E7C: wave_set 020F02D4 clears it, daBtd_Execute 020F1494 adds 30 (+REG) up to 2300; dMagma_ballBoss_c::calc 024F60E8 raises the balls at about this distance from the centre */
    /* 0x70C4 */ be<f32> mMagmaFloorY;  /* was m6E80: daBtd_Create 020F4308 sets -50; daBtd_Execute 020F1494 passes it to the magma floor (setYpos) */
    /* 0x70C8 */ be<f32> mMagmaBallOffsetY;  /* was m6E84: daBtd_Create 020F4308 sets 0 / -30, end 020F52B8 -200, btdExecCamera eases it to 0; dMagma_ballBoss_c::setup 024F637C adds it to the ball base height, calc 024F60E8 sinks the balls below -150 */
    /* 0x70CC */ be<u8> mMagmaRiseCenter;  /* was m6E88: daBtd_Execute 020F1494 sets 1, the actions (in move 020F6004) clear it; dMagma_ballBoss_c::setup 024F637C raises the balls near the centre only when it is 1 */
    /* 0x70CD */ be<u8> m6E89[3];
    /* 0x70D0 */ gptr<dMagma_floor_c> floor;
    /* 0x70D4 */ be<s8> mForceBossCamera;  /* was m6E90: daBtd_Create 020F4308 sets 1, btdExecCamera clears it; daBtd_Execute 020F1494 calls SetTypeForce("Boss01") while it is set */
    /* 0x70D5 */ be<u8> m6E91[3];
};
WWHD_OFFSET(btd_class, mPhaseBtd, 0x3C8);
WWHD_OFFSET(btd_class, mPhaseMagma, 0x3D0);
WWHD_OFFSET(btd_class, mpPhase1Morf, 0x3D8);
WWHD_OFFSET(btd_class, btk, 0x3DC);
WWHD_OFFSET(btd_class, brk, 0x3E0);
WWHD_OFFSET(btd_class, mpPhase2Morf, 0x3E4);
WWHD_OFFSET(btd_class, btkS, 0x3E8);
WWHD_OFFSET(btd_class, brkS, 0x3EC);
WWHD_OFFSET(btd_class, mpDeadBodyMorf, 0x3F0);
WWHD_OFFSET(btd_class, mpDeadBodyBtkAnm, 0x3F4);
WWHD_OFFSET(btd_class, mpDeadBodyBrkAnm, 0x3F8);
WWHD_OFFSET(btd_class, mpDeadHeadMorf, 0x3FC);
WWHD_OFFSET(btd_class, mpDeadHeadBtkAnm, 0x400);
WWHD_OFFSET(btd_class, mpDeadHeadBrkAnm, 0x404);
WWHD_OFFSET(btd_class, mBattlePhase, 0x408);
WWHD_OFFSET(btd_class, m02E1, 0x409);
WWHD_OFFSET(btd_class, mFrameCounter, 0x40A);
WWHD_OFFSET(btd_class, mAction, 0x40C);
WWHD_OFFSET(btd_class, mGohmaState, 0x40E);
WWHD_OFFSET(btd_class, mAttackType, 0x410);
WWHD_OFFSET(btd_class, mAttackAnmIdx, 0x411);
WWHD_OFFSET(btd_class, mAttackHand, 0x412);
WWHD_OFFSET(btd_class, m02EB, 0x413);
WWHD_OFFSET(btd_class, mTimers, 0x414);
WWHD_OFFSET(btd_class, mBlinkTimer, 0x418);
WWHD_OFFSET(btd_class, mStunTimer, 0x41A);
WWHD_OFFSET(btd_class, mActionState, 0x41C);
WWHD_OFFSET(btd_class, mRockHitState, 0x41E);
WWHD_OFFSET(btd_class, m02F7, 0x41F);
WWHD_OFFSET(btd_class, mEyeCloseTimer, 0x420);
WWHD_OFFSET(btd_class, mEyelidAngle, 0x422);
WWHD_OFFSET(btd_class, mJointPos, 0x424);
WWHD_OFFSET(btd_class, mEyeJointPos, 0x508);
WWHD_OFFSET(btd_class, mStts, 0x514);
WWHD_OFFSET(btd_class, mAtSphs, 0x550);
WWHD_OFFSET(btd_class, mEyeSph, 0x1B94);
WWHD_OFFSET(btd_class, mFireSph, 0x1CC0);
WWHD_OFFSET(btd_class, mSibukiSphs, 0x1DEC);
WWHD_OFFSET(btd_class, mHandCyls, 0x5884);
WWHD_OFFSET(btd_class, mHandCylMode, 0x5FA4);
WWHD_OFFSET(btd_class, m5E7D, 0x5FA5);
WWHD_OFFSET(btd_class, mFireSize, 0x5FA8);
WWHD_OFFSET(btd_class, mAtMode, 0x5FAC);
WWHD_OFFSET(btd_class, mTotalDamage, 0x5FAD);
WWHD_OFFSET(btd_class, mLookMode, 0x5FAE);
WWHD_OFFSET(btd_class, m5E87, 0x5FAF);
WWHD_OFFSET(btd_class, mNeckAngleY, 0x5FB0);
WWHD_OFFSET(btd_class, mNeckAngleX, 0x5FB2);
WWHD_OFFSET(btd_class, mNeckTargetAngleX, 0x5FB4);
WWHD_OFFSET(btd_class, mHeadAngleY, 0x5FB6);
WWHD_OFFSET(btd_class, mTurnTargetAngleY, 0x5FB8);
WWHD_OFFSET(btd_class, mTurnScale, 0x5FBA);
WWHD_OFFSET(btd_class, mTurnMaxStep, 0x5FBC);
WWHD_OFFSET(btd_class, m5E96, 0x5FBE);
WWHD_OFFSET(btd_class, mHitSePos, 0x5FC0);
WWHD_OFFSET(btd_class, mSphHitTimer, 0x60EC);
WWHD_OFFSET(btd_class, mCylHitTimer, 0x6112);
WWHD_OFFSET(btd_class, mFireChipEmitter, 0x6120);
WWHD_OFFSET(btd_class, mAttackEmitter, 0x6134);
WWHD_OFFSET(btd_class, mAttackEmitterTimer, 0x6150);
WWHD_OFFSET(btd_class, m6036, 0x615E);
WWHD_OFFSET(btd_class, mpDeadHeadEmitter, 0x6160);
WWHD_OFFSET(btd_class, mHahenPos, 0x6164);
WWHD_OFFSET(btd_class, mHahenAngle, 0x6170);
WWHD_OFFSET(btd_class, m604E, 0x6176);
WWHD_OFFSET(btd_class, mPa_SmokeEcallBack, 0x6178);
WWHD_OFFSET(btd_class, mLightInfluence, 0x61D8);
WWHD_OFFSET(btd_class, mKankyoBlend, 0x61FC);
WWHD_OFFSET(btd_class, mLightRate, 0x6200);
WWHD_OFFSET(btd_class, mLightColorR, 0x6204);
WWHD_OFFSET(btd_class, mLightColorG, 0x6205);
WWHD_OFFSET(btd_class, mLightColorB, 0x6206);
WWHD_OFFSET(btd_class, mKankyoState, 0x6207);
WWHD_OFFSET(btd_class, mKankyoTimer, 0x6208);
WWHD_OFFSET(btd_class, m60DE, 0x620A);
WWHD_OFFSET(btd_class, mTevstr, 0x620C);
WWHD_OFFSET(btd_class, mCrackStage, 0x63D4);
WWHD_OFFSET(btd_class, m6191, 0x63D5);
WWHD_OFFSET(btd_class, hahen, 0x63D8);
WWHD_OFFSET(btd_class, mBtdSibukiS, 0x6950);
WWHD_OFFSET(btd_class, mSplashPopCount, 0x7058);
WWHD_OFFSET(btd_class, mBlurRate, 0x7059);
WWHD_OFFSET(btd_class, mDemoCamMode, 0x705A);
WWHD_OFFSET(btd_class, mDemoMainCount, 0x705C);
WWHD_OFFSET(btd_class, mDemoSubCount, 0x705E);
WWHD_OFFSET(btd_class, mDemoCamEye, 0x7060);
WWHD_OFFSET(btd_class, mDemoCamCenter, 0x706C);
WWHD_OFFSET(btd_class, mDemoCamOrbitAngleX, 0x7078);
WWHD_OFFSET(btd_class, mDemoCamOrbitAngleY, 0x707A);
WWHD_OFFSET(btd_class, m6E38, 0x707C);
WWHD_OFFSET(btd_class, mDemoCamFovy, 0x7080);
WWHD_OFFSET(btd_class, mDemoCamSpeed, 0x7084);
WWHD_OFFSET(btd_class, mDemoCamOrbitDist, 0x7088);
WWHD_OFFSET(btd_class, mDemoCamShake, 0x708C);
WWHD_OFFSET(btd_class, mDeadHeadOn, 0x7090);
WWHD_OFFSET(btd_class, m6E4D, 0x7091);
WWHD_OFFSET(btd_class, mDeadHeadPos, 0x7094);
WWHD_OFFSET(btd_class, mDeadHeadTargetPos, 0x70A0);
WWHD_OFFSET(btd_class, mDeadHeadSpeed, 0x70AC);
WWHD_OFFSET(btd_class, mDeadHeadAngle, 0x70B8);
WWHD_OFFSET(btd_class, m6E7A, 0x70BE);
WWHD_OFFSET(btd_class, mWaveRadius, 0x70C0);
WWHD_OFFSET(btd_class, mMagmaFloorY, 0x70C4);
WWHD_OFFSET(btd_class, mMagmaBallOffsetY, 0x70C8);
WWHD_OFFSET(btd_class, mMagmaRiseCenter, 0x70CC);
WWHD_OFFSET(btd_class, m6E89, 0x70CD);
WWHD_OFFSET(btd_class, floor, 0x70D0);
WWHD_OFFSET(btd_class, mForceBossCamera, 0x70D4);
WWHD_OFFSET(btd_class, m6E91, 0x70D5);
WWHD_SIZE(btd_class, 0x70D8);

// HD: these GHS word copies preserve signalling-NaN payloads.
inline u32 btdWord(const be<f32> &value) {
    return gabi::load<u32>(gabi::ea(&value));
}
inline void btdSetWord(be<f32> &value, u32 bits) {
    gabi::store<u32>(gabi::ea(&value), bits);
}

// HD: translation-unit guest-call bindings, retaining the native register signatures.
void btdGuest_JAIZelBasic_seDeleteObject(u32 a0);
void btdGuest_JPASetRMtxTVecfromMtx(u32 a0, u32 a1, u32 a2);
void btdGuest_JUTAssertion_getSDevice(u32 a0, s32 a1, u32 a2);
void btdGuest_JUTAssertion_getSDevice(u32 a0, u32 a1, u32 a2);
void btdGuest_anm_init(u32 a0, s32 a1, f32 a2, s32 a3, f32 a4, s32 a5);
void btdGuest_anm_init(u32 a0, s32 a1, f32 a2, u8 a3, f32 a4, s32 a5);
void btdGuest_btd_effect(u32 a0);
void btdGuest_btd_kankyo(u32 a0);
void btdGuest_cCcS_Set(u32 a0, u32 a1);
void btdGuest_cLib_addCalc0(u32 a0, f32 a1, f32 a2);
void btdGuest_cLib_addCalc2(u32 a0, f32 a1, f32 a2, f32 a3);
void btdGuest_cLib_addCalcAngleS2(u32 a0, s16 a1, s32 a2, s16 a3);
void btdGuest_cLib_addCalcAngleS2(u32 a0, s32 a1, s32 a2, s16 a3);
void btdGuest_cLib_addCalcAngleS2(u32 a0, s32 a1, s32 a2, s32 a3);
void btdGuest_cLs_Addition(u32 a0, u32 a1);
void btdGuest_cM3dGCyl_SetC(u32 a0, u32 a1);
void btdGuest_cM3dGCyl_SetC(u32 a0, void *a1);
void btdGuest_cM3dGCyl_SetH(u32 a0, f32 a1);
void btdGuest_cM3dGCyl_SetR(u32 a0, f32 a1);
void btdGuest_cM3dGSph_SetC(u32 a0, u32 a1);
void btdGuest_cM3dGSph_SetC(u32 a0, void *a1);
void btdGuest_cM3dGSph_SetR(u32 a0, f32 a1);
s32 btdGuest_cM_atan2sFullResult(f32 a0, f32 a1);
void btdGuest_cXyz_normalize(void *a0, void *a1);
void btdGuest_cXyz_op_sub(u32 a0, void *a1, u32 a2);
void btdGuest_cXyz_op_sub(u32 a0, void *a1, void *a2);
void btdGuest_construct_array(u32 a0, s32 a1, s32 a2, u32 a3);
u32 btdGuest_dCam_getBody();
void btdGuest_dCamera_c_Reset_2(u32 a0, void *a1, void *a2);
void btdGuest_dCamera_c_SetTrimSize(u32 a0, s32 a1);
void btdGuest_dCamera_c_SetTypeForce_2(u32 a0, u32 a1, s32 a2);
void btdGuest_dCamera_c_Set_3(u32 a0, void *a1, void *a2, s16 a3, f32 a4);
void btdGuest_dCamera_c_Start(u32 a0);
void btdGuest_dCamera_c_Stop(u32 a0);
void btdGuest_dCcD_Cyl_Set(u32 a0, u32 a1);
s32 btdGuest_dCcD_GObjInf_ChkAtHit(u32 a0);
void btdGuest_dCcD_GObjInf_dCcD_GObjInf(u32 a0);
void btdGuest_dCcD_GStts_dCcD_GStts(u32 a0);
void btdGuest_dCcD_Sph_MoveCAt(u32 a0, u32 a1);
void btdGuest_dCcD_Sph_Set(u32 a0, u32 a1);
void btdGuest_dCcD_Sph_StartCAt(u32 a0, u32 a1);
void btdGuest_dCcD_Stts_Init(u32 a0, s32 a1, s32 a2, u32 a3);
s32 btdGuest_dComIfG_play_c_createMagma(u32 a0);
void btdGuest_dComIfG_resDelete(u32 a0, u32 a1);
s32 btdGuest_dComIfG_resLoad(u32 a0, u32 a1);
s32 btdGuest_dComIfGp_getReverb(s32 a0);
void btdGuest_dKy_custom_colset(u8 a0, u8 a1, f32 a2);
void btdGuest_dKy_custom_timeset(f32 a0);
void btdGuest_dKy_efplight_cut(u32 a0);
void btdGuest_dKy_efplight_set(u32 a0);
void btdGuest_dLib_setNextStageBySclsNum(s32 a0, s32 a1);
u32 btdGuest_dMagma_packet_c_newFloor(u32 a0, void *a1, u32 a2, s32 a3, s32 a4);
u32 btdGuest_dPa_J3DmodelEmitter_c_dPa_J3DmodelEmitter_c(s32 a0, u32 a1, u32 a2, s32 a3, u32 a4,
                                                         u32 a5, s32 a6, s32 a7);
u32 btdGuest_dPa_control_c_set(u32 a0, s32 a1, s32 a2, u32 a3, s32 a4, s32 a5, s32 a6, s32 a7,
                               s32 a8, s32 a9, s32 a10, s32 a11);
u32 btdGuest_dPa_control_c_set(u32 a0, s32 a1, s32 a2, u32 a3, u32 a4, s32 a5, s32 a6, s32 a7,
                               s32 a8, s32 a9, s32 a10, s32 a11);
u32 btdGuest_dPa_control_c_set(u32 a0, s32 a1, u16 a2, u32 a3, s32 a4, s32 a5, s32 a6, s32 a7,
                               s32 a8, s32 a9, s32 a10, s32 a11);
u32 btdGuest_dPa_control_c_set(u32 a0, s32 a1, u32 a2, u32 a3, s32 a4, s32 a5, s32 a6, u32 a7,
                               s32 a8, s32 a9, s32 a10, s32 a11);
u32 btdGuest_dPa_control_c_set(u32 a0, s32 a1, u32 a2, u32 a3, u32 a4, s32 a5, s32 a6, s32 a7,
                               s32 a8, s32 a9, s32 a10, s32 a11);
void btdGuest_dPa_control_c_setDiscardResult(u32 a0, s32 a1, s32 a2, u32 a3, s32 a4, s32 a5, s32 a6,
                                             s32 a7, s32 a8, s32 a9, s32 a10, s32 a11);
void btdGuest_dPa_control_c_setDiscardResult(u32 a0, s32 a1, s32 a2, void *a3, s32 a4, s32 a5,
                                             s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11);
void btdGuest_dPa_control_c_setDiscardResult(u32 a0, s32 a1, s32 a2, void *a3, u32 a4, s32 a5,
                                             s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11);
void btdGuest_dPa_control_c_setDiscardResult(u32 a0, s32 a1, s32 a2, void *a3, void *a4, s32 a5,
                                             s32 a6, u32 a7, s32 a8, s32 a9, s32 a10, s32 a11);
void btdGuest_dPa_control_c_setSimple(u32 a0, u32 a1, u32 a2, s32 a3, u32 a4, u32 a5, s32 a6);
void *btdGuest_dPa_smokeEcallBack_dPa_smokeEcallBack(void *a0, s32 a1);
u32 btdGuest_dRes_control_c_getRes(u32 a0, void *a1, s32 a2);
u32 btdGuest_getEnvironmentLight();
void btdGuest_dScnKy_env_light_c_settingTevStruct(u32 a0, s32 a1, u32 a2, u32 a3);
void btdGuest_dSnap_RegistFig(s32 a0, u32 a1, u32 a2, s16 a3, f32 a4, f32 a5, f32 a6);
void btdGuest_dSv_event_c_offEventBit(u32 a0, s32 a1);
void btdGuest_dSv_event_c_onEventBit(u32 a0, s32 a1);
s32 btdGuest_dSv_memBit_c_isDungeonItem(u32 a0, s32 a1);
void btdGuest_dSv_memBit_c_onDungeonItem(u32 a0, s32 a1);
void btdGuest_dVibration_c_StartQuake(u32 a0, s32 a1, s32 a2, void *a3);
void btdGuest_dVibration_c_StartShock(u32 a0, s32 a1, s32 a2, void *a3);
void btdGuest_dVibration_c_StopQuake(u32 a0, s32 a1);
void btdGuest_move(u32 a0);
void btdGuest_damage_check(u32 a0);
void btdGuest_destroy_arr(u32 a0, s32 a1, s32 a2, u32 a3, s32 a4, s32 a5);
void btdGuest_end(u32 a0);
void btdGuest_fopAcM_create(s32 a0, s32 a1, u32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7);
void btdGuest_fopAcM_createDisappear(u32 a0, u32 a1, s32 a2, s32 a3, s32 a4);
void btdGuest_fopAcM_createWarpFlower(u32 a0, s32 a1, s32 a2, s32 a3);
s32 btdGuest_fopAcM_entrySolidHeap(u32 a0, u32 a1, s32 a2);
void btdGuest_fopAcM_orderPotentialEvent(u32 a0, s32 a1, s32 a2, s32 a3);
s16 btdGuest_fopAcM_searchActorAngleY(u32 a0, u32 a1);
s32 btdGuest_fopAcM_searchActorAngleYFullResult(u32 a0, u32 a1);
s32 btdGuest_fopAc_IsActor(void *a0);
void btdGuest_fopAc_ac_c_dtor(u32 a0, s32 a1);
void btdGuest_fopAc_ac_c_fopAc_ac_c(u32 a0);
u32 btdGuest_fpcEx_Search(u32 a0, u32 a1);
u32 btdGuest_get_anm(u32 a0);
void btdGuest_hahen_set2(u32 a0);
void btdGuest_hahen_set(u32 a0, s32 a1);
void btdGuest_kubi_calc(u32 a0);
u32 btdGuest_mDoExt_J3DModel_create(u32 a0, s32 a1, u32 a2);
u32 btdGuest_mDoExt_McaMorf_mDoExt_McaMorf(s32 a0, u32 a1, s32 a2, s32 a3, s32 a4, s32 a5, f32 a6,
                                           s32 a7, s32 a8, s32 a9, s32 a10, s32 a11, u32 a12);
u32 btdGuest_mDoExt_McaMorf_mDoExt_McaMorf(s32 a0, u32 a1, s32 a2, s32 a3, u32 a4, s32 a5, f32 a6,
                                           s32 a7, s32 a8, s32 a9, s32 a10, s32 a11, u32 a12);
void btdGuest_mDoExt_McaMorf_setAnm(void *a0, u32 a1, u8 a2, f32 a3, f32 a4, f32 a5, f32 a6,
                                    u32 a7);
void btdGuest_mDoGph_gInf_c_onBlure();
s32 btdGuest_mDoHIO_subRoot_c_createChild(u32 a0, u32 a1);
void btdGuest_mDoHIO_subRoot_c_deleteChild(s32 a0);
void btdGuest_mDoMtx_XrotM(u32 a0, s16 a1);
void btdGuest_mDoMtx_YrotM(u32 a0, s16 a1);
void btdGuest_mDoMtx_YrotS(u32 a0, s16 a1);
void btdGuest_mDoMtx_YrotS(u32 a0, s32 a1);
void btdGuest_mDoMtx_ZrotM(u32 a0, s16 a1);
void btdGuest_op_delete(u32 a0);
void btdGuest_op_delete(void *a0);
void btdGuest_register_global_object(u32 a0);
void btdGuest_sibuki_set(u32 a0);
void btdGuest_smoke_set_s(u32 a0, u32 a1, u32 a2);
void btdGuest_startdemo(u32 a0);
void btdGuest_wave_set(u32 a0);

// HD: HIO stores its vtable after the data, rather than before it.
struct daBtd_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ be<u8> m06;
    /* 0x03 */ be<u8> m07;
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<s16> m0C;
    /* 0x0A */ be<s16> m0E;
    /* 0x0C */ be<u8> m10;
    /* 0x0D */ be<s8> m11;
    /* 0x0E */ be<s8> m12;
    /* 0x0F */ be<s8> m13;
    /* 0x10 */ be<f32> m14;
    /* 0x14 */ be<f32> m18;
    /* 0x18 */ be<f32> m1C;
    /* 0x1C */ be<f32> m20;
    /* 0x20 */ be<f32> m24;
    /* 0x24 */ be<f32> m28;
    /* 0x28 */ be<s16> m2C;
    /* 0x2A */ be<s16> m2E;
    /* 0x2C */ be<f32> m30;
    /* 0x30 */ be<f32> m34;
    /* 0x34 */ be<f32> m38;
    /* 0x38 */ be<s16> m3C;
    /* 0x3A */ be<s16> m3E;
    /* 0x3C */ be<s16> m40;
    /* 0x3E */ be<s16> m42;
    /* 0x40 */ be<s16> m44;
    /* 0x42 */ be<s16> m46;
    /* 0x44 */ be<f32> m48;
    /* 0x48 */ be<f32> m4C;
    /* 0x4C */ be<f32> m50;
    /* 0x50 */ be<u8> m54;
    u8 _pad51[3];
    be<u32> vtable;
};
WWHD_SIZE(daBtd_HIO_c, 0x58);

inline J3DModel *btdModel(u32 address) {
    return gabi::at<J3DModel>(address);
}

inline mDoExt_McaMorf *btdMorph(u32 address) {
    return gabi::at<mDoExt_McaMorf>(address);
}

inline J3DMtxBuffer_hd *btdMatrixBuffer(u32 address) {
    return gabi::at<J3DMtxBuffer_hd>(address);
}

inline btd_hahen_s *btdDebris(u32 address) {
    return gabi::at<btd_hahen_s>(address);
}

inline btd_sibuki_s *btdSplash(u32 address) {
    return gabi::at<btd_sibuki_s>(address);
}

inline dCcD_Sph *btdSphere(u32 address) {
    return gabi::at<dCcD_Sph>(address);
}

u32 btdGuest_allocate(s32 a0);
void btdGuest_bgmStart(u32 a0);
void btdGuest_bgmStop(s32 a0);
void btdGuest_brkInitNative(u32 a0, u32 a1, u32 a2, s32 a3, f32 a4, s32 a5, s32 a6, s32 a7, s32 a8,
                            s32 a9);
void btdGuest_brkInitNative(u32 a0, u32 a1, u32 a2, s32 a3, s32 a4, f32 a5, s32 a6, s32 a7, s32 a8,
                            s32 a9);
void btdGuest_btkInitNative(u32 a0, u32 a1, u32 a2, s32 a3, f32 a4, s32 a5, s32 a6, s32 a7, s32 a8,
                            s32 a9);
void btdGuest_cCcD_Stts_construct(u32 a0);
void btdGuest_cM3dGCyl_construct(u32 a0);
void btdGuest_changeBgmStatus(s32 a0);
void btdGuest_dCcD_Sph_construct(u32 a0);
void btdGuest_dCcD_Sph_destroy(u32 a0, s32 a1);
void btdGuest_dCcD_Stts_destroy(u32 a0, s32 a1);
u32 btdGuest_getBrkNative(u32 a0);
u32 btdGuest_getBtkNative(u32 a0);
u32 btdGuest_getPlay();
void btdGuest_modelUpdateDL(u32 a0);
u32 btdGuest_nodeToJoint(void *a0);
void btdGuest_oneByte_construct(u32 a0);
void btdGuest_screenReport(s32 a0, s32 a1, u32 a2, s32 a3);
void btdGuest_seStartMonster(u32 a0, u32 a1, s32 a2, s32 a3, s32 a4);
void btdGuest_seStartMonster(u32 a0, u32 a1, u32 a2, s32 a3, s32 a4);
void btdGuest_setLightTevColorType(u32 a0, u32 a1, u32 a2);
f32 btdGuest_sqrtf(f32 a0);
void btdGuest_subBgmStart(u32 a0);
void btdGuest_subBgmStop();
void btdGuest_zeroByte_construct(u32 a0);

// HD: REG0 stores float registers at +8 and short registers at +0x84.
struct BtdDebugRegisters {
    u8 prefix[8];
    be<f32> floats[31];
    be<s16> shorts[8];
};
WWHD_OFFSET(BtdDebugRegisters, floats, 8);
WWHD_OFFSET(BtdDebugRegisters, shorts, 0x84);
#include "d/actor/d_a_player.h"
inline daPy_py_c *btdPlayer(u32 address) {
    return gabi::at<daPy_py_c>(address);
}

u32 btdGuest_btkConstruct(u32 handle);
u32 btdGuest_brkConstruct(u32 handle);
s32 btdGuest_btkInit(u32 handle, u32 data, u32 animation, s32 enable, s32 loop, f32 speed,
                     s32 start, s32 end, s32 second);
s32 btdGuest_brkInit(u32 handle, u32 data, u32 animation, s32 enable, s32 loop, f32 speed,
                     s32 start, s32 end, s32 second);
void btdGuest_startFireAttack(u32 sphere, cXyz *position);
void btdGuest_moveFireAttack(u32 sphere, cXyz *position);

// HD: the constructor copies all three light blocks with their native field widths.
struct BtdLightObjectHD {
    cXyz position, direction;
    GXColor color;
    GXColorS10 ambientColor;
    be<f32> k0Intensity, c0Intensity;
    be<f32> cosineAttenuation[3], distanceAttenuation[3];
    u8 tail[0x40];
};
WWHD_SIZE(BtdLightObjectHD, 0x84);
struct BtdLightingHD {
    BtdLightObjectHD light;
    u8 tevFields[0x3C];
    BtdLightObjectHD materialLight, secondMaterialLight;
};
WWHD_SIZE(BtdLightingHD, 0x1C8);
inline BtdLightingHD *btdLighting(u32 actor) {
    return gabi::at<BtdLightingHD>(actor + offsetof(btd_class, mTevstr));
}

inline dComIfG_play_c *btdPlay() {
    return gabi::at<dComIfG_play_c>(dComIfGp_ea() + offsetof(dComIfG_inf_c, play));
}

using BtdActorPointer = gptr<fopAc_ac_c>;
using BtdFloat = be<f32>;
using BtdByte = be<u8>;

struct BtdConstructorWords {
    be<u32> values[4];
};

// HD: emitter position and status are used after guest callbacks, so reload each field at its use.
struct BtdEmitterHD {
    u8 prefix[0x5C];
    be<s32> maxFrame;
    u8 particleState[0x238 - 0x60];
    cXyz globalTranslation;
    u8 matrixState[0x10];
    be<u32> status;
};
WWHD_OFFSET(BtdEmitterHD, globalTranslation, 0x238);
WWHD_OFFSET(BtdEmitterHD, status, 0x254);
inline BtdEmitterHD *btdEmitter(u32 address) {
    return gabi::at<BtdEmitterHD>(address);
}
inline cXyz *btdVector(u32 address) {
    return gabi::at<cXyz>(address);
}
inline Mtx34 *btdMatrix(u32 address) {
    return gabi::at<Mtx34>(address);
}
inline fopAc_ac_c *btdBaseActor(u32 address) {
    return gabi::at<fopAc_ac_c>(address);
}

struct BtdShapeAttributesHD {
    cXyz minimum, maximum;
    be<u32> aabVtable, shapeVtable;
};
WWHD_SIZE(BtdShapeAttributesHD, 0x20);
struct BtdJointDataHD {
    be<u32> resource;
    be<u16> number;
    u8 padding[2];
    be<u32> callback;
};
struct BtdDragonHD : fopAc_ac_c {
    u8 prefix[0x558 - 0x3AC];
    be<u8> m43C;
    u8 padding[0x5C4 - 0x559];
    cXyz m4A8;
    u8 padding2[6];
    be<s16> m4BA;
    u8 padding3[0xE];
    be<s16> m4CA;
};
WWHD_OFFSET(BtdDragonHD, m43C, 0x558);
WWHD_OFFSET(BtdDragonHD, m4BA, 0x5D6);
WWHD_OFFSET(BtdDragonHD, m4CA, 0x5E6);
struct BtdMagmaFloorHD {
    u8 prefix[0xB38];
    cXyz position;
};
inline dCcD_Cyl *btdCylinder(u32 address) {
    return gabi::at<dCcD_Cyl>(address);
}
inline BtdShapeAttributesHD *btdCylinderShape(u32 address) {
    return gabi::at<BtdShapeAttributesHD>(address + offsetof(dCcD_Cyl, mShapeAttr));
}
inline BtdJointDataHD *btdJoint(u32 address) {
    return gabi::at<BtdJointDataHD>(address);
}
inline BtdDragonHD *btdDragon(u32 address) {
    return gabi::at<BtdDragonHD>(address);
}
inline J3DModelData *btdModelData(u32 address) {
    return gabi::at<J3DModelData>(address);
}
inline mDoExt_baseAnm *btdBaseAnimation(u32 address) {
    return gabi::at<mDoExt_baseAnm>(address);
}
inline BtdMagmaFloorHD *btdMagmaFloor(u32 address) {
    return gabi::at<BtdMagmaFloorHD>(address);
}

using BtdWord = be<u32>;
using BtdEmitterPointer = gptr<JPABaseEmitter>;
struct BtdCameraViewHD {
    u8 prefix[0xDC];
    struct {
        cXyz eye, center;
    } lookat;
};
struct BtdParticleControlHD {
    u8 prefix[0x130];
    be<u32> emitterList;
};
inline BtdCameraViewHD *btdCameraView(u32 address) {
    return gabi::at<BtdCameraViewHD>(address);
}
inline BtdParticleControlHD *btdParticleControl(u32 address) {
    return gabi::at<BtdParticleControlHD>(address);
}

enum BtdAction {
    ACT_WAIT = 0,
    ACT_ATTACK = 1,
    ACT_DAMAGE = 2,
    ACT_START_DEMO = 10,
    ACT_END = 11,
};
enum BtdAttack {
    ATTACK_JAB = 0,
    ATTACK_PUNCH = 1,
    ATTACK_FIRE = 2,
    ATTACK_HIGH_FIRE = 3,
    ATTACK_SIDE_FIRE = 4,
    ATTACK_TURNING_PUNCH = 5,
};

// Verified actor entry points and per-TU compiler helpers.
void wave_set(btd_class *i_this);
void *get_anm(btd_class *i_this);
void *get_btk(btd_class *i_this);
void *get_brk(btd_class *i_this);
void anm_init(btd_class *i_this, s32 animation, f32 blend, u8 loop, f32 speed, s32 sound);
void eff_off(btd_class *i_this);
void *k_a_d_sub(void *actor, void *context);
void *dr2_a_d_sub(void *actor, void *context);
void *wepon_s_sub(void *actor, void *context);
BOOL daBtd_IsDelete(btd_class *i_this);
void *smokeEcallBack_ct(void *self);
void SafeString_dt(void *self, s32 flags);
void smokeEcallBack_dt(void *self, s32 flags);
void SafeString_v14(void *self);
BOOL daBtd_Delete(btd_class *i_this);
void *btdCylConstructor(void *self);
void btd_class_dt(btd_class *i_this, s32 flags);
void *daBtd_HIO_ct(void *self);
void __sinit_d_a_btd_cpp();
BOOL daBtd_Draw(btd_class *i_this);
void smoke_set_s(btd_class *i_this, cXyz *position, csXyz *angles);
void kubi_calc(btd_class *i_this);
void sibuki_set(btd_class *i_this);
BOOL nodeCallBack(void *node, s32 timing);
void hahen_set2(btd_class *i_this);
void btd_kankyo(btd_class *i_this);
void btd_effect(btd_class *i_this);
btd_class *btd_class_ct(btd_class *self);
s32 daBtd_Create(btd_class *i_this);
void hahen_set(btd_class *i_this, s32 side);
void startdemo(btd_class *i_this);
BOOL useHeapInit(btd_class *i_this);
void move(btd_class *i_this);
void end(btd_class *i_this);
BOOL daBtd_Execute(btd_class *i_this);

inline mDoExt_btkAnm* btdTextureAnimation(u32 address) { return gabi::at<mDoExt_btkAnm>(address); }

inline mDoExt_brkAnm* btdColorAnimation(u32 address) { return gabi::at<mDoExt_brkAnm>(address); }

/* d_a_bpw.h (WWHD): Jalhalla (big Poe boss) layout (see wwhd_src/README.md).
 *
 * HD constructor 020D60C8 allocates 0xE9C bytes (GameCube 0xD80). Every GameCube field is
 * shifted by the fopAc_ac_c delta (+0x11C); confirmed by this unit's loads/stores (embedded
 * effect callbacks, background check, colliders and the invisible model sit at the shifted
 * offsets). Member names follow the GameCube actor (zeldaret/tww); fields it leaves offset-named are
 * named from their uses (`was mXXX` comments). */
#pragma once
#include "wwhd.h"
#include "f_op/f_op_actor.h"
#include "d/d_cc_d.h"
#include "d/d_particle.h"
#include "m_Do/m_Do_ext.h"
#include "d/actor/d_a_player.h"
#include "d/d_com_inf_game.h"

// HD: light influences append a final float after the GC position, color and power.
struct BpwLightInfluenceHD {
    cXyz mPos;
    GXColorS10 mColor;
    be<f32> mPower, mFluctuation;
    be<s32> mIdx;
    be<f32> mHD20;
};
WWHD_SIZE(BpwLightInfluenceHD, 0x24);
WWHD_OFFSET(BpwLightInfluenceHD, mPower, 0x14);
WWHD_OFFSET(BpwLightInfluenceHD, mHD20, 0x20);

struct bpw_class {
    /* 0x000 */ fopEn_enemy_c actor;
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ be<u32> mpMorf;
    /* 0x3D4 */ u8 opaque_3D4[4];
    /* 0x3D8 */ be<u32> mpLightFreezeBrkAnm;
    /* 0x3DC */ be<u32> mpLanternGlowBrkAnm;
    /* 0x3E0 */ be<u32> mpLightStunBrkAnm;
    /* 0x3E4 */ be<u32> mpCurseStartBrkAnm;
    /* 0x3E8 */ be<u32> mpCurseEndBrkAnm;
    /* 0x3EC */ be<u32> mpDefaultBrkAnm;
    /* 0x3F0 */ be<f32> mAlphaModelMtx[12];  /* was m2D4: kantera_draw / damage_ball_draw (in daBPW_Draw 020D1334) copy calc_mtx into it and pass it to dComIfGd_setAlphaModel */
    /* 0x420 */ cXyz mLinChkStart[3];  /* was m304: action_dousa (in body_execute 020DC704) and action_b_fire_1_dousa (in daBPW_Execute 020D3464) set [0] to current.pos (+400 y in action_dousa) and pass it as the start of linChk.Set; daBPW_Create 020D6268 seeds all 3 with current.pos */
    /* 0x444 */ cXyz mLinChkEnd[3];  /* was m328: action_dousa (in body_execute 020DC704) and action_b_fire_1_dousa (in daBPW_Execute 020D3464) set [0] to a point ahead of current.pos and pass it as the end of linChk.Set; daBPW_Create 020D6268 seeds all 3 with current.pos */
    /* 0x468 */ cXyz mBodyPos;
    /* 0x474 */ cXyz mCoSphCenter;  /* was m358: body_nodeCallBack 020D0C04 puts it mCoSphOffsetY above the root joint, kantera_nodeCallBack 020D0DF0 100 below the lantern joint; body_execute 020DC704 / kantera_execute (in daBPW_Execute 020D3464) centre mBodyCoSph / mKanteraCoSph on it; the lantern's torch particle and burning sound sit there */
    /* 0x480 */ u8 opaque_480[0xC];
    /* 0x48C */ cXyz mAtSphCenter;  /* was m370: body_nodeCallBack 020D0C04 puts it 50 below the root joint; body_execute 020DC704 centres mBodyAtSph on it */
    /* 0x498 */ cXyz mChildActorPos;
    /* 0x4A4 */ be<f32> mKanteraSwingX;  /* was m388: kantera_calc 020D30EC eases it to 2000 / 6000 and swings the lantern's mKanteraDousaRot.x by it * sin(mKanteraSwingPhase); action_dousa (in body_execute 020DC704) sets 0 */
    /* 0x4A8 */ be<f32> m38C;
    /* 0x4AC */ be<f32> mKanteraSwingZ;  /* was m390: kantera_calc 020D30EC eases it to 2000 / 6000 and swings the lantern's mKanteraDousaRot.z by it * cos(mKanteraSwingPhase); action_dousa (in body_execute 020DC704) sets 0 */
    /* 0x4B0 */ cXyz mWaistPos;  /* was m394: body_nodeCallBack 020D0C04 stores the J_BPW_KOSI1 (waist) joint position; action_kougeki (in body_execute 020DC704) emits the inhale / blow particles there */
    /* 0x4BC */ cXyz mFlameBallPos;  /* was m3A0: action_b_fire_1_dousa (in daBPW_Execute 020D3464) starts it at current.pos and eases it along the flame line (mLinChkStart / mLinChkEnd); damage_ball_execute (same) centres mDamageBallCoSph on it */
    /* 0x4C8 */ cXyz mModelOffset;  /* was m3AC: draw_SUB 020D0FEC translates the body model by current.pos + it, kantera_draw_SUB 020D1158 the lantern model by it; next_status_clear 020D257C, action_damage 020D72BC, action_bunri_dousa 020D8564 and action_start_demo 020DB380 set or ease its y */
    /* 0x4D4 */ cXyz mRightArmPos;  /* was m3B8: body_nodeCallBack 020D0C04 stores the J_BPW_UDE_R3 joint position; action_start_demo 020DB380 copies it to mFire1DousaPos for the opening smoke */
    /* 0x4E0 */ cXyz mJawPos;  /* was m3C4: body_nodeCallBack 020D0C04 stores the J_BPW_AGO1 (jaw) joint position; action_kougeki (in body_execute 020DC704) plays the breath-in / breath-out / fire-blast sounds there; daBPW_Delete 020D57A8 deletes its sound object */
    /* 0x4EC */ csXyz mKanteraDousaRot;
    /* 0x4F2 */ csXyz mRollRot;  /* was m3D6: draw_SUB 020D0FEC rotates the body by Y(y) X(x) Y(-y); action_damage 020D72BC sets y to the player's angle and adds 5000 to x per frame while thrown (debug REG8_S(3) path), 0 on landing */
    /* 0x4F8 */ be<u8> mType;
    /* 0x4F9 */ be<u8> mUnknownParam2;
    /* 0x4FA */ be<u8> mLightState;
    /* 0x4FB */ be<u8> mHitType;
    /* 0x4FC */ be<u8> mIsLightFrozen;  /* was m3E0: action_damage 020D72BC sets 1 when the light freezes the body; while it is set daBPW_Draw 020D1334 uses mpLightFreezeBrkAnm, body_execute 020DC704 plays it, body_atari_check 020D6D98 and alpha_anime 020D1C40 skip; next_status_clear 020D257C sets 0 */
    /* 0x4FD */ be<u8> mIsLightStunned;  /* was m3E1: action_damage 020D72BC sets 1 (light hit before the freeze) and 0; while it is set daBPW_Draw 020D1334 and body_execute 020DC704 use mpLightStunBrkAnm; next_status_clear 020D257C clears it */
    /* 0x4FE */ be<u8> mCurseBrkState;  /* was m3E2: noroi_brk_check 020D2350 sets 2 (curse start), 1 (curse end), 0; daBPW_Draw 020D1334 and body_execute 020DC704 pick mpCurseStartBrkAnm at 2, mpCurseEndBrkAnm at 1 */
    /* 0x4FF */ u8 opaque_4FF;
    /* 0x500 */ be<u8> mReturnToParent;  /* was m3E4: action_kantera_dousa (in daBPW_Execute 020D3464): while it is set the lantern goes to state 5 and slides toward its parent (mParentId), which clears it; no setter in HD or zeldaret/tww */
    /* 0x501 */ be<u8> mIsThrown;  /* was m3E5: action_damage 020D72BC sets 1 when the carried body is thrown and 0 when it hits a wall; body_execute 020DC704 turns toward mTargetAngleY only while it is 0; next_status_clear 020D257C clears it */
    /* 0x502 */ be<u8> mIsCurseDash;  /* was m3E6: action_dousa (in body_execute 020DC704) sets 1 when the curse dash starts; noroi_check 020D2058 drops the body on a nearby player only while it is 0; next_status_clear 020D257C clears it */
    /* 0x503 */ be<u8> mKanteraHidden;  /* was m3E7: daBPW_Create 020D6268 sets 1 for the lantern; while it is 0 daBPW_Draw 020D1334 draws the lantern glow and daBPW_Execute 020D3464 plays it and lights the torch particle; action_start_demo 020DB380 clears it on the lantern when it appears */
    /* 0x504 */ be<u8> mVibMode;  /* was m3E8: action_damage 020D72BC and action_karada_taore (in body_execute 020DC704) set 1 / 2; vib_mode_check (in daBPW_Execute 020D3464) starts the matching shock or quake and sets 0 */
    /* 0x505 */ be<u8> mIsReformed;  /* was m3E9: action_bunri_dousa 020D8564 sets 1 when the regrowing body reaches full scale; action_kougeki (in body_execute 020DC704) then goes to state 0x32 and clears it; Big_pow_gattai_check 0244A5A8 (d_a_pw) merges a Poe only while it is set */
    /* 0x506 */ be<u8> mStartDemoStep;  /* was m3EA: action_start_demo 020DB380 sets 1..6 as the opening goes on; the child Poes (d_a_pw, 024500C0) follow each step */
    /* 0x507 */ be<u8> mKankyouHendouState;
    /* 0x508 */ be<f32> mKankyouBlend;  /* was m3EC: kankyou_hendou 020D1DA8 eases it to 0 / 1 and uses it as the dKy_custom_colset blend; daBPW_Create 020D6268 sets 1, action_bunri_dousa 020D8564 0 */
    /* 0x50C */ be<f32> mDemoCamDist;  /* was m3F0: action_bunri_dousa 020D8564 sets it and eases it to 600; the demo camera eye is mDemoCamDist * sin / cos(mDemoCamAngleY) */
    /* 0x510 */ be<s16> mDemoCamAngleY;  /* was m3F4: action_bunri_dousa 020D8564 sets it and steps it toward 0; the demo camera eye is mDemoCamDist * sin / cos(it) */
    /* 0x512 */ be<s16> mBattlePhase;  /* was m3F6: action_bunri_dousa 020D8564 sets 1 / 2 as health drops to 10 / 5 (and 3 / 4 in the defeat demo) and ends each separation at the matching health; action_dousa (in body_execute 020DC704) takes the turn speed from it; light_on_off 020D271C and noroi_check 020D2058 read it; action_start_demo 020DB380 sets 0 */
    /* 0x514 */ be<s32> mBckFileIdx;  /* was m3F8: anm_init 020D1A64 stores its bckFileIdx argument; action_dousa / action_kougeki / action_karada_taore (in body_execute 020DC704), action_damage 020D72BC, action_bunri_dousa 020D8564 and action_start_demo 020DB380 compare it with BCK indices */
    /* 0x518 */ be<u32> mKanteraId;  /* was m3FC: daBPW_Create 020D6268 stores the id of the lantern child it creates; kantera_calc 020D30EC, kantera_pos_search 020D2ED8, action_bunri_dousa 020D8564 and action_start_demo 020DB380 search it */
    /* 0x51C */ be<u32> mFireId;  /* was m400: action_kougeki (in body_execute 020DC704) stores the id of the fire actor it creates at the lantern; fire_and_emitter_clear 020D22B8 deletes it and sets -1 */
    /* 0x520 */ be<u32> mParentId;  /* was m404: daBPW_Create 020D6268 (kantera_create_init) stores parentActorID; action_kantera_dousa (in daBPW_Execute 020D3464) turns the lantern toward that actor */
    /* 0x524 */ be<s16> mBlureRate;  /* was m408: daBPW_Draw 020D1334 passes it to the blur (setBlureRate) while above 1 and clears it at 1; action_damage 020D72BC (wall hit) sets 0xB5; daBPW_Execute 020D3464 sets 0x32 / steps it down by 2 */
    /* 0x526 */ u8 opaque_526[2];
    /* 0x528 */ cXyz mDemoCamCenter;  /* was m40C: action_bunri_dousa 020D8564 and action_start_demo 020DB380 set and ease it and pass it as the center to dCamera_c::Set / Reset */
    /* 0x534 */ cXyz mDemoCamEye;  /* was m418: action_bunri_dousa 020D8564 and action_start_demo 020DB380 set and ease it and pass it as the eye to dCamera_c::Set / Reset */
    /* 0x540 */ cXyz mChildPoePos;  /* was m424: action_bunri_dousa 020D8564 copies the watched child Poe's current.pos; the demo camera center and the player's placement follow it */
    /* 0x54C */ u8 opaque_54C[0xC];
    /* 0x558 */ be<f32> mDemoCamBlend;  /* was m43C: action_bunri_dousa 020D8564 and action_start_demo 020DB380 set 0 at each camera cut and ease it to 1; the demo camera steps (demo_calc) are scaled by it */
    /* 0x55C */ be<f32> mDemoCamFovy;  /* was m440: action_bunri_dousa 020D8564 and action_start_demo 020DB380 set and ease it and pass it as the fovy to dCamera_c::Set */
    /* 0x560 */ be<s16> mBodyAction;
    /* 0x562 */ be<s16> mActionState;
    /* 0x564 */ u8 opaque_564[4];
    /* 0x568 */ be<s16> mAttWaitTimer;
    /* 0x56A */ be<s16> mSomeCountdownTimers[10];
    /* 0x57E */ be<s16> mPoeCount;  /* was m462: body: action_big_demo 024500C0 (d_a_pw, view PwJalhallaView) adds 1 for each Poe that merges back; action_bunri_dousa 020D8564 ends the separation when it reaches health and reuses it as the mChildPoeIds index of the Poe the camera follows; action_dousa (in body_execute 020DC704) sets 1, action_bunri_dousa 0. Other roles of the same word: the lantern's landing step in action_kantera_dousa and the flame's sweep phase in action_b_fire_1_dousa (both in daBPW_Execute 020D3464) */
    /* 0x580 */ be<s16> mPoeMerged;  /* was m464: body: action_big_demo 024500C0 (d_a_pw, view PwJalhallaView) sets 1 when a Poe merges back; action_bunri_dousa 020D8564 and action_start_demo 020DB380 then play BCK_CORE_NIGE1 and clear it; action_bunri_dousa also keeps the child Poe's angle to the player there for the camera. Other roles of the same word: the lantern's spin in action_kantera_dousa and the flame's sweep direction in action_b_fire_1_dousa (both in daBPW_Execute 020D3464) */
    /* 0x582 */ u8 opaque_582[0xE];
    /* 0x590 */ be<s16> mDemoCamActive;  /* was m474: action_bunri_dousa 020D8564 sets 1 when a demo starts and sets the camera from mDemoCamCenter / mDemoCamEye / mDemoCamFovy each frame while it is set */
    /* 0x592 */ be<s16> mTargetAngleY;  /* was m476: action_dousa / action_kougeki / action_karada_taore (in body_execute 020DC704), action_damage 020D72BC, action_bunri_dousa 020D8564 and kantera_pos_search 020D2ED8 set it (mostly the angle to the player); body_execute turns current.angle.y toward it by mTurnStep */
    /* 0x594 */ be<s16> mAlphaWavePhase;  /* was m478: alpha_anime 020D1C40 adds 0x400 per frame and sets mAlpha to mBaseAlpha + sin(it) * 30; next_status_clear 020D257C and action_damage 020D72BC set 0 */
    /* 0x596 */ be<s16> mTurnStep;  /* was m47A: body_execute 020DC704 turns current.angle.y toward mTargetAngleY by it; action_dousa (same) takes it from {0x100, 0x300, 0x500}[mBattlePhase]; action_kougeki sets 0x500 / 0x300 / 140, action_damage 020D72BC 0 */
    /* 0x598 */ be<s16> mFloatPhase;  /* was m47C: fuwafuwa_calc 020D1D3C and torituki_execute (in daBPW_Execute 020D3464) add 700 per frame and bob current.pos.y by sin(it) * 30; action_bunri_dousa 020D8564 sets 0 and swings mModelOffset by it */
    /* 0x59A */ be<s16> mAlpha;  /* was m47E: daBPW_Draw 020D1334 writes it to the materials' K color 3 alpha and the alpha models; alpha_anime 020D1C40, action_dousa (in body_execute 020DC704), action_damage 020D72BC and action_bunri_dousa 020D8564 fade it; body_atari_check 020D6D98 takes no hit at 0 */
    /* 0x59C */ be<s16> mBaseAlpha;  /* was m480: alpha_anime 020D1C40 eases it to 0x96 and sets mAlpha to it + sin(mAlphaWavePhase) * 30; action_bunri_dousa 020D8564 eases mAlpha toward it; action_dousa / action_karada_taore (in body_execute 020DC704) set 0x96 / 200 */
    /* 0x59E */ be<s16> mKanteraSwingPhase;  /* was m482: kantera_calc 020D30EC adds mKanteraSwingSpeed per frame and swings the lantern by sin / cos(it); action_dousa (in body_execute 020DC704) sets 0 */
    /* 0x5A0 */ be<s16> mKanteraSwingSpeed;  /* was m484: kantera_calc 020D30EC eases it to 3000 / 6000 and adds it to mKanteraSwingPhase */
    /* 0x5A2 */ u8 opaque_5A2[2];
    /* 0x5A4 */ be<f32> mSpeedStep;  /* was m488: action_dousa (in body_execute 020DC704) sets 1 / 5 / 2 and eases speedF toward mTargetSpeed by it */
    /* 0x5A8 */ be<f32> mBgOffsetY;  /* was m48C: BG_check 020D1B90 lowers current / old pos.y by it before the ground check and raises them after; action_dousa (in body_execute 020DC704) eases it to 0, action_damage 020D72BC / action_bunri_dousa 020D8564 to -120, action_kantera_dousa (in daBPW_Execute 020D3464) sets 60 */
    /* 0x5AC */ be<f32> mTargetSpeed;  /* was m490: action_dousa (in body_execute 020DC704) eases speedF toward it by mSpeedStep and sets 10 / * 1.35 / 0; action_damage 020D72BC sets 120 / 20 / 5; next_status_clear 020D257C 0 */
    /* 0x5B0 */ be<f32> mDamageBallRadius;  /* was m494: action_b_fire_1_dousa / action_b_fire_2_dousa (in daBPW_Execute 020D3464) set 50 / 30 / 35; damage_ball_execute (same) passes it to mDamageBallCoSph.SetR */
    /* 0x5B4 */ be<f32> mWallHalfHeight;  /* was m498: BG_check 020D1B90 passes it as the half height to mAcchCir.SetWall; next_status_clear 020D257C sets 350, daBPW_Execute 020D3464 50 / 0, action_bunri_dousa 020D8564 0 */
    /* 0x5B8 */ be<f32> mWallRadius;  /* was m49C: BG_check 020D1B90 passes it as the radius to mAcchCir.SetWall; next_status_clear 020D257C sets 400, action_damage 020D72BC 300, action_bunri_dousa 020D8564 160, daBPW_Execute 020D3464 200 / 40 */
    /* 0x5BC */ be<f32> mCoSphOffsetY;  /* was m4A0: body_nodeCallBack 020D0C04 puts mCoSphCenter this far above the root joint; action_damage 020D72BC sets 50, action_dousa / action_kougeki (in body_execute 020DC704) 0 */
    /* 0x5C0 */ be<f32> mEyeHeight;  /* was m4A4: body_execute 020DC704 sets eyePos.y to current.pos.y + it and the attention point at + 2 * it + 100; action_karada_taore (same) sets 100, action_damage 020D72BC / next_status_clear 020D257C 300 */
    /* 0x5C4 */ be<f32> mBodyAtSphR;  /* was m4A8: body_execute 020DC704 passes it to mBodyAtSph.SetR; action_karada_taore (same) sets 500 / 220 / 400, next_status_clear 020D257C and daBPW_Create 020D6268 220 */
    /* 0x5C8 */ be<s32> mChildPoeIds[15];
    /* HD: no mShadowId (the ground shadow is gone); LIGHT_INFLUENCE starts at 0x604 and is
     * 0x24 bytes (the constructor stores 1.0f at its +0x20) */
    /* 0x604 */ BpwLightInfluenceHD mLightInfluence;
    /* 0x628 */ dPa_smokeEcallBack mBlowSmokeCb;  /* was m50C: action_kougeki (in body_execute 020DC704) passes it to the BPWBLOWSMOKE00 particle; fire_and_emitter_clear 020D22B8 ends it; HD 0x20 */
    /* 0x648 */ dPa_followEcallBack mBlowCb;  /* was m52C: action_kougeki (in body_execute 020DC704) passes it to the BPWBLOW00 particle; fire_and_emitter_clear 020D22B8 ends it */
    /* 0x65C */ dPa_followEcallBack mInhaleCb;  /* was m540: action_kougeki (in body_execute 020DC704) passes it to the BPWINHALE01 particle and ends it; fire_and_emitter_clear 020D22B8 ends it */
    /* 0x670 */ dPa_followEcallBack mOpeningSmokeCb;  /* was m554: action_start_demo 020DB380 passes it to the BPWOPENINGSMOKE00 particle (0x845A) and ends it */
    /* 0x684 */ dPa_followEcallBack mFire1Dousa_Pa_followEcallBack;
    /* 0x698 */ dPa_followEcallBack mFire1Dousa_Pa_followEcallBack2;
    /* 0x6AC */ dPa_followEcallBack mFlameHitCb;  /* was m590: action_b_fire_1_dousa (in daBPW_Execute 020D3464) passes it to the BPWFLAMETHROWER02 particle at mFlameHitPos and ends it when the line misses; fire_and_emitter_clear 020D22B8 ends it */
    /* 0x6C0 */ dPa_followEcallBack mFireDousa2_Pa_followEcallBack;
    /* 0x6D4 */ dPa_followEcallBack mFireDousa2_Pa_followEcallBack2;
    /* 0x6E8 */ dPa_followEcallBack mTorchCb;  /* was m5CC: daBPW_Execute 020D3464 passes it to the lantern's ID_AK_JN_TORCH particle at mCoSphCenter; daBPW_Delete 020D57A8 ends it */
    /* 0x6FC */ dPa_smokeEcallBack mSmokeCb[4];  /* was m5E0: action_damage 020D72BC uses [0] (BPWSIRIMOTISMOKE00), action_karada_taore (in body_execute 020DC704) [1] (BPWJUMPSMOKE00) and [2] / [3] (BPWPRESSSMOKE00 / 01); daBPW_Delete 020D57A8 ends all 4 */
    /* 0x77C */ cXyz mFire1DousaPos;
    /* 0x788 */ cXyz mFlameHitPos;  /* was m66C: action_b_fire_1_dousa (in daBPW_Execute 020D3464) stores the line-check cross point and emits BPWFLAMETHROWER02 there */
    /* 0x794 */ csXyz mFire1DousaRot;
    /* 0x79A */ be<s16> mFlameHitRotX;  /* was m67E: action_b_fire_1_dousa (in daBPW_Execute 020D3464) sets 0 on a floor, 0x4000 on a wall, and passes it as the start of the particle's csXyz angle */
    /* 0x79C */ be<s16> mFlameHitRotY;  /* was m680: action_b_fire_1_dousa (in daBPW_Execute 020D3464) sets the wall angle (atan2 from mFlameHitPos to the flame start); the y of the particle angle that starts at mFlameHitRotX */
    /* 0x79E */ u8 opaque_79E[2];
    /* 0x7A0 */ be<s16> mCarryAngleY;  /* was m684: action_damage 020D72BC stores shape_angle.y when the body can be picked up and, while carried, adds the player's turn and sets shape_angle.y from it */
    /* 0x7A2 */ u8 opaque_7A2[2];
    /* 0x7A4 */ dBgS_AcchCir mAcchCir;
    /* 0x7E4 */ dBgS_ObjAcch mAcch;
    /* 0x9A8 */ dCcD_Stts mStts;
    /* 0x9E4 */ dCcD_Sph mBodyCoSph;
    /* 0xB10 */ dCcD_Sph mBodyAtSph;
    /* 0xC3C */ dCcD_Sph mKanteraCoSph;
    /* 0xD68 */ dCcD_Sph mDamageBallCoSph;
    /* 0xE94 */ u8 mInvisibleModel[8];
};
WWHD_SIZE(bpw_class, 0xE9C);
WWHD_OFFSET(bpw_class, mpMorf, 0x3D0);
WWHD_OFFSET(bpw_class, mAlphaModelMtx, 0x3F0);
WWHD_OFFSET(bpw_class, mCoSphCenter, 0x474);
WWHD_OFFSET(bpw_class, mKanteraDousaRot, 0x4EC);
WWHD_OFFSET(bpw_class, mType, 0x4F8);
WWHD_OFFSET(bpw_class, mReturnToParent, 0x500);
WWHD_OFFSET(bpw_class, mKankyouBlend, 0x508);
WWHD_OFFSET(bpw_class, mDemoCamCenter, 0x528);
WWHD_OFFSET(bpw_class, mDemoCamBlend, 0x558);
WWHD_OFFSET(bpw_class, mAttWaitTimer, 0x568);
WWHD_OFFSET(bpw_class, mDemoCamActive, 0x590);
WWHD_OFFSET(bpw_class, mSpeedStep, 0x5A4);
WWHD_OFFSET(bpw_class, mChildPoeIds, 0x5C8);
WWHD_OFFSET(bpw_class, mLightInfluence, 0x604);
WWHD_OFFSET(bpw_class, mBlowSmokeCb, 0x628);
WWHD_OFFSET(bpw_class, mTorchCb, 0x6E8);
WWHD_OFFSET(bpw_class, mSmokeCb, 0x6FC);
WWHD_OFFSET(bpw_class, mFire1DousaPos, 0x77C);
WWHD_OFFSET(bpw_class, mCarryAngleY, 0x7A0);
WWHD_OFFSET(bpw_class, mAcchCir, 0x7A4);
WWHD_OFFSET(bpw_class, mAcch, 0x7E4);
WWHD_OFFSET(bpw_class, mStts, 0x9A8);
WWHD_OFFSET(bpw_class, mBodyCoSph, 0x9E4);
WWHD_OFFSET(bpw_class, mDamageBallCoSph, 0xD68);
WWHD_OFFSET(bpw_class, mInvisibleModel, 0xE94);

// HD: fixed ABI bindings preserve call-site register widths of this translation unit.
unsigned int BpwHD_dComIfGp_getPlay();
unsigned int BpwHD_dRes_control_c_getRes(unsigned int, void *, u32);
unsigned int BpwHD_dPa_smokeEcallBack_dPa_smokeEcallBack(u32, int);
void BpwHD___dl(u32);
int BpwHD_dComIfGp_getReverb(s32);
void BpwHD_mDoAud_seStart(u32, u32, u32, s32);
void BpwHD___register_global_object(unsigned int);
void BpwHD_firstHeaderObject_construct(unsigned int);
void BpwHD_secondHeaderObject_construct(unsigned int);
unsigned int BpwHD_operator_new(unsigned int);
void BpwHD_fopAc_ac_c_fopAc_ac_c(u32);
void BpwHD_dPa_smokeEcallBack_dPa_smokeEcallBack_2(u32, int);
void BpwHD_dPa_followEcallBack_dPa_followEcallBack(u32, int, int);
void BpwHD___construct_array(u32, int, int, unsigned int);
void BpwHD_dBgS_AcchCir_dBgS_AcchCir(u32);
void BpwHD_dBgS_Acch_dBgS_Acch(u32);
void BpwHD_dCcD_Stts_construct(u32);
void BpwHD_dCcD_GStts_dCcD_GStts(u32);
void BpwHD_dCcD_Sph_construct(u32);
void BpwHD_mDoExt_invisibleModel_construct(u32);
void BpwHD_mDoExt_invisibleModel_destroy(u32, int);
void BpwHD_dCcD_Sph_destroy(u32, int);
void BpwHD_dCcD_Stts_destroy(u32, int);
void BpwHD_dBgS_Acch_destroy(u32, int);
void BpwHD_cM3dGPla_destroy(u32, int);
void BpwHD___destroy_arr(u32, int, int, unsigned int, int, int);
void BpwHD_fopAc_ac_c_destroy_fopAc_ac_c(u32, int);
void BpwHD_JAIZelBasic_seDeleteObject(u32);
void BpwHD_dComIfG_resDelete(u32, unsigned int);
void BpwHD_dKy_plight_cut(u32);
void BpwHD_PSMTXTrans(u32, f32, f32, f32);
void BpwHD_mDoMtx_YrotM(u32, s32);
void BpwHD_mDoMtx_XrotM(u32, s32);
void BpwHD_mDoMtx_ZrotM(u32, s32);
void BpwHD_mDoExt_McaMorf_calc(unsigned int);
unsigned int BpwHD_dKy_getEnvlight();
void BpwHD_dScnKy_env_light_c_settingTevStruct(u32, int, u32, u32);
void BpwHD_MtxTrans(float, float, float, int);
void BpwHD_mDoMtx_YrotM_2(unsigned int, s32);
void BpwHD_mDoMtx_XrotM_2(unsigned int, s32);
void BpwHD_mDoMtx_ZrotM_2(unsigned int, s32);
void BpwHD_mDoExt_McaMorf_setAnm(unsigned int, u32, u32, f32, f32, float, float, u32);
void BpwHD_mDoExt_McaMorf_setAnm_2(unsigned int, u32, u32, f32, f32, float, float, unsigned int);
void BpwHD_dBgS_AcchCir_SetWall(u32, float, float);
void BpwHD_dBgS_Acch_CrrPos(u32, u32);
int BpwHD_dDetect_c_chk_light(u32, u32);
void BpwHD_cLib_addCalcAngleS2(u32, int, int, int);
void BpwHD_PSMTXCopy(unsigned int, unsigned int);
void BpwHD_PSMTXCopy_2(u32, unsigned int);
unsigned int BpwHD_J3DNode_toJoint(u32);
void BpwHD_MtxPosition(void *, u32);
void BpwHD_cLib_addCalc2(u32, f32, float, float);
void BpwHD_dKy_custom_colset(u32, u32, f32);
float BpwHD_fopAcM_searchActorDistance(u32, unsigned int);
int BpwHD_dCcD_GObjInf_ChkAtHit(u32);
unsigned int BpwHD_dCcD_GAtTgCoCommonBase_GetAc(u32);
void BpwHD_dCcD_GObjInf_ClrTgHit(u32);
void BpwHD_dPa_control_c_set(unsigned int, int, int, u32, int, int, int, int, int, int, int, int);
void BpwHD_fopAcM_create(int, int, u32, s32, void *, int, int, int);
unsigned int BpwHD_fopAcIt_Judge(unsigned int, gabi::be<unsigned int> *);
void BpwHD_fopAcM_delete(u32);
void BpwHD_dPa_followEcallBack_end(u32);
void BpwHD_dPa_smokeEcallBack_end(u32);
void BpwHD_mDoExt_brkAnm_initWithFlags(u32, u32, u32, int, int, float, int, int, int, int);
void BpwHD_dSv_info_c_onSwitch(u32, s32, s32);
void BpwHD_dSv_info_c_offSwitch(u32, s32, s32);
float BpwHD_cM_rndF(float);
float BpwHD_cM_rndF_2(f32);
int BpwHD_fopAc_IsActor(u32);
void BpwHD_fpcEx_Search(unsigned int, u32);
float BpwHD_fopAcM_searchActorDistance_2(u32, u32);
short BpwHD_fopAcM_searchActorAngleY(u32, u32);
short BpwHD_cLib_distanceAngleS(s32, s32);
float BpwHD_cM_rndFX(float);
void BpwHD_mDoMtx_YrotS(u32, s32);
void BpwHD_MtxPosition_2(void *, void *);
void BpwHD_PSVECAdd(void *, u32, void *);
void BpwHD_cLib_addCalc2_2(u32, f32, float, f32);
short BpwHD_cM_atan2s(f32, f32);
float BpwHD_sqrtf(f32);
void BpwHD_cLib_addCalcAngleS2_2(u32, s32, int, int);
void BpwHD_cLib_addCalc2_3(u32, float, float, float);
unsigned int BpwHD_mDoExt_McaMorf_mDoExt_McaMorf(unsigned int, u32, unsigned int, unsigned int, u32,
                                                 u32, float, unsigned int, int, int, unsigned int,
                                                 unsigned int, unsigned int);
unsigned int BpwHD_J3DModelData_getJointTree(unsigned int);
unsigned int BpwHD_mDoExt_brkAnm_construct(u32);
int BpwHD_mDoExt_brkAnm_initWithFlags_2(u32, u32, u32, int, u32, float, int, int, int, int);
int BpwHD_mDoExt_invisibleModel_create(u32, u32);
int BpwHD_mDoExt_brkAnm_initWithFlags_3(u32, u32, u32, int, int, float, int, int, int, int);
void BpwHD_dCcD_GStts_Move(u32);
int BpwHD_dCcD_GObjInf_ChkTgHit(u32);
unsigned int BpwHD_dCcD_GObjInf_GetTgHitObj(u32);
void BpwHD_dPa_control_c_set_2(unsigned int, int, int, void *, u32, void *, int, int, int, int, int,
                               int);
unsigned int BpwHD_J3DModelData_getMaterialTable(u32);
void BpwHD_GXColor_toFloat(void *, u32);
void BpwHD_GXColor_fromFloat(void *, void *, float);
unsigned int BpwHD_J3DMaterial_getConstantColor(u32, int);
void BpwHD_mDoExt_brkAnm_entry(u32, u32, float);
void BpwHD_mDoGph_gInf_c_onBlure();
void BpwHD_dKy_setLightTevColorType(u32, u32, u32);
void BpwHD_J3DModel_setVisibility(u32, int);
void BpwHD_mDoExt_McaMorf_entryDL(unsigned int);
void BpwHD_mDoExt_invisibleModel_entryMaskOff(u32);
void BpwHD_mDoExt_invisibleModel_entry(u32);
void BpwHD_dSnap_RegistFig(int, u32, float, float, float);
void BpwHD_mDoExt_McaMorf_entryDL_2(u32);
void BpwHD_PSMTXCopy_3(unsigned int, u32);
void BpwHD_MtxTrans_2(f32, f32, f32, int);
void BpwHD_MtxScale(float, float, float, int);
void BpwHD_mDoExt_McaMorf_updateDL(unsigned int);
void BpwHD_fopAcM_setCullSizeBox(u32, f32, f32, f32, f32, f32, f32);
void BpwHD_dBgS_Acch_Set(u32, u32, u32, u32, int, u32, u32, int, int);
int BpwHD_dSv_memBit_c_isDungeonItem(unsigned int, int);
int BpwHD_dComIfG_resLoad(u32, unsigned int);
int BpwHD_fopAcM_entrySolidHeap(u32, unsigned int, unsigned int);
void BpwHD_dCcD_Stts_Init(u32, int, int, u32);
void BpwHD_dCcD_Sph_Set(u32, unsigned int);
void BpwHD_mDoExt_McaMorf_play(unsigned int, int, int, int);
unsigned int BpwHD_fopAcM_createChild(int, u32, int, u32, s32, void *, int, int, int);
unsigned int BpwHD_fopAcM_createChild_2(int, u32, u32, void *, s32, void *, int, int, int);
float BpwHD_cM_rndFX_2(f32);
void BpwHD_mDoAud_bgmStart(u32);
void BpwHD_dKy_plight_set(u32);
void BpwHD_mDoAud_seStartLevel(u32, u32, u32, u32, s32);
void BpwHD_mDoExt_brkAnm_initWithFlags_4(u32, u32, u32, int, u32, float, int, int, int, int);
void BpwHD_fopAcM_cancelCarryNow(u32);
short BpwHD_fopAcM_searchActorAngleY_2(u32, unsigned int);
void BpwHD_cBgS_GetTriPnt(u32, u32, void *, void *, void *);
int BpwHD_dBgS_GetAttributeCode(u32, u32);
void BpwHD_dCamera_c_ForceLockOff(u32, u32);
unsigned int BpwHD_dPa_control_c_set_3(unsigned int, int, int, u32, u32, int, int, u32, s32, int,
                                       int, int);
void BpwHD_dKy_get_seacolor(gabi::be<unsigned int> *, gabi::be<unsigned int> *);
void BpwHD_cLib_addCalc2_4(u32, f32, f32, f32);
void BpwHD_fopAcM_orderPotentialEvent(u32, int, int, int);
void BpwHD_dCamera_c_Stop(u32);
void BpwHD_dCamera_c_SetTrimSize(u32, int);
void BpwHD_mDoAud_bgmStreamPlay();
void BpwHD_cLib_addCalc2_5(u32, float, float, f32);
void BpwHD_dPa_control_c_set_4(unsigned int, int, int, u32, u32, int, int, u32, int, int, int, int);
void BpwHD_dVibration_c_StopQuake(u32, int);
void BpwHD_dCamera_c_Reset(u32, void *, void *);
void BpwHD_dCamera_c_Start(u32);
void BpwHD_mDoAud_bgmStart_2(unsigned int);
void BpwHD_dSv_memBit_c_onDungeonItem(unsigned int, int);
int BpwHD_J3DFrameCtrl_checkPass(unsigned int, float);
void BpwHD_mDoAud_seStart_2(unsigned int, u32, unsigned int, s32);
void BpwHD_dCamera_c_Set(u32, void *, void *, float, int);
int BpwHD_J3DFrameCtrl_checkPass_2(unsigned int, f32);
void BpwHD_dPa_control_c_set_5(unsigned int, int, u32, u32, u32, u32, int, u32, int, int, int, int);
void BpwHD_cM3dGSph_SetC(u32, u32);
void BpwHD_cM3dGSph_SetR(u32, f32);
void BpwHD_cCcS_Set(u32, u32);
short BpwHD_cLib_distanceAngleS_2(s32, int);
void BpwHD_cLib_addCalc0(u32, float, float);
void BpwHD_mDoExt_baseAnm_play(unsigned int);
void BpwHD_cBgS_LinChk_ct(void *);
void BpwHD_mDoMtx_YrotS_2(unsigned int, s32);
void BpwHD_PSVECAdd_2(u32, u32, u32);
void BpwHD_dBgS_LinChk_Set(void *, u32, u32, u32);
int BpwHD_cBgS_LineCross(u32, void *);
unsigned int BpwHD_cBgS_GetTriPla(u32, u32, u32);
void BpwHD_cBgS_LinChk_destroy(void *, int);
void BpwHD_fopAcM_posMove(u32, unsigned int);
void BpwHD_fopAcM_posMove_2(u32, u32);
void BpwHD_cXyz___mi(u32, void *, u32);
void BpwHD_dVibration_c_StartShock(u32, int, int, void *);
void BpwHD_cBgS_LinChk_ct_2(u32);
void BpwHD_cBgS_LinChk_destroy_2(u32, int);
void BpwHD_JPASetRMtxSTVecfromMtx(u32, u32, u32, u32);
unsigned int BpwHD_dPa_control_c_set_6(unsigned int, int, u32, u32, u32, int, int, u32, s32, int,
                                       int, int);
float BpwHD_cM_rnd();
void BpwHD_dBgS_LinChk_Set_2(u32, u32, u32, u32);
int BpwHD_cBgS_LineCross_2(u32, u32);
unsigned int BpwHD_dPa_control_c_set_7(unsigned int, int, int, u32, u32, int, int, int, int, int,
                                       int, int);
void BpwHD_dPa_control_c_set_8(unsigned int, int, int, u32, u32, int, int, u32, s32, int, int, int);
unsigned int BpwHD_fopAcM_create_2(int, int, u32, s32, u32, int, int, int);
unsigned int BpwHD_fopAcM_create_3(int, int, void *, s32, u32, int, int, int);
int BpwHD_J3DFrameCtrl_checkPass_3(u32, float);
void BpwHD_cM3dGSph_SetC_2(u32, void *);
void BpwHD_cM3dGSph_SetR_2(u32, float);
void BpwHD_cLib_addCalcAngleS2_3(u32, s32, int, s32);
void BpwHD_mDoExt_baseAnm_play_2(u32);
void BpwHD_dSv_event_c_onEventBit(unsigned int, int);
void BpwHD_dLib_setNextStageBySclsNum(int, s32);
void BpwHD_mDoAud_seStart_3(unsigned int, unsigned int, unsigned int, s32);
void BpwHD_fopAcM_createDisappear(u32, u32, int, int, int);
void BpwHD_mDoAud_bgmStop(int);
void BpwHD_mDoAud_bgmStreamPrepare(unsigned int);
void BpwHD_fopAcM_createDisappear_2(u32, void *, int, int, int);
void BpwHD_fopAcM_createWarpFlower(u32, int, s32, int);

// HD: debug register banks retain their header and the GC float/short arrays.
struct BpwHioBank {
    be<s16> mHeader[4];
    be<f32> mFloat[30];
    be<s16> mShort[8];
};
WWHD_SIZE(BpwHioBank, 0x90);
WWHD_OFFSET(BpwHioBank, mFloat, 8);
WWHD_OFFSET(BpwHioBank, mShort, 0x80);

// HD: model rendering fields differ from the GameCube J3DModel layout.
struct BpwModelHD {
    u8 _00[0x2C];
    be<u32> mpMtxBuffer;
    u8 _30[0xAC - 0x30];
    be<u32> mpModelData;
    u8 _B0[8];
    be<u32> mUserArea;
    cXyz mBaseScale;
    be<f32> mBaseTransformMtx[12];
};
WWHD_SIZE(BpwModelHD, 0xF8);
WWHD_OFFSET(BpwModelHD, mpMtxBuffer, 0x2C);
WWHD_OFFSET(BpwModelHD, mpModelData, 0xAC);
WWHD_OFFSET(BpwModelHD, mUserArea, 0xB8);
WWHD_OFFSET(BpwModelHD, mBaseScale, 0xBC);
WWHD_OFFSET(BpwModelHD, mBaseTransformMtx, 0xC8);

// HD: keep the cut byte in guest memory even though the legacy player header uses u8.
struct BpwPlayerCutHD {
    u8 _00[0x3AC];
    be<u8> mCutType;
};
WWHD_SIZE(BpwPlayerCutHD, 0x3AD);
WWHD_OFFSET(BpwPlayerCutHD, mCutType, 0x3AC);

// HD: joint and material arrays are contiguous descriptors rather than GC pointer tables.
struct BpwModelDataHD {
    u8 _00[4];
    be<u32> mJointCount;
    be<u32> mpJoints;
    be<u32> mMaterialCount;
    be<u32> mpMaterials;
    u8 _14[0x48 - 0x14];
    be<u32> mpTevRegAnimator;
};
WWHD_SIZE(BpwModelDataHD, 0x4C);
WWHD_OFFSET(BpwModelDataHD, mpTevRegAnimator, 0x48);
struct BpwJointHD {
    u8 _00[4];
    be<u16> mJntNo;
    u8 _06[2];
    be<u32> mpCallBack;
    u8 _0C[0x10];
};
WWHD_SIZE(BpwJointHD, 0x1C);
WWHD_OFFSET(BpwJointHD, mpCallBack, 8);
struct BpwJointTreeHD {
    u8 _00[8];
    be<u16> mJointCount;
};
WWHD_SIZE(BpwJointTreeHD, 0xA);
struct BpwMaterialTableHD {
    u8 _00[0x24];
    be<u16> mMaterialCount;
};
WWHD_SIZE(BpwMaterialTableHD, 0x26);
struct BpwMaterialHD {
    u8 _00[0x18];
    be<u32> mpTevBlock;
    u8 _1C[0xA0 - 0x1C];
    be<u32> mDirtyFlags;
    u8 _A4[0x39C - 0xA4];
};
WWHD_SIZE(BpwMaterialHD, 0x39C);
WWHD_OFFSET(BpwMaterialHD, mpTevBlock, 0x18);
WWHD_OFFSET(BpwMaterialHD, mDirtyFlags, 0xA0);
struct BpwTevBlockHD {
    u8 _00[4];
    be<u32> mpVtable;
};
WWHD_SIZE(BpwTevBlockHD, 8);
struct BpwTevVtableHD {
    u8 _00[0x3C];
    be<u32> setTevKColor;
    u8 _40[0xC];
    be<u32> getTevKColor;
};
WWHD_SIZE(BpwTevVtableHD, 0x50);
struct BpwColorF32 {
    be<f32> r, g, b, a;
};
WWHD_SIZE(BpwColorF32, 0x10);
struct BpwMtxBufferHD {
    u8 _00[4];
    be<u16> mFlags;
    u8 _06[0xA];
    be<u32> mpAnmMtx;
};
WWHD_SIZE(BpwMtxBufferHD, 0x14);
WWHD_OFFSET(BpwMtxBufferHD, mpAnmMtx, 0x10);

struct BpwDrawListHD {
    u8 _00[0x48];
    be<u32> mpOpaList, mpXluList;
    u8 _50[4];
    be<u32> mpOpaListMaskOff, mpXluListMaskOff;
};
WWHD_SIZE(BpwDrawListHD, 0x5C);
WWHD_OFFSET(BpwDrawListHD, mpOpaList, 0x48);
WWHD_OFFSET(BpwDrawListHD, mpOpaListMaskOff, 0x54);

// HD: the ten-short reset starts after mSomeCountdownTimers and reaches mDemoCamActive.
struct BpwResetTimersHD {
    u8 _00[0x57E];
    be<s16> mResetTimers[10];
};
WWHD_SIZE(BpwResetTimersHD, 0x592);
WWHD_OFFSET(BpwResetTimersHD, mResetTimers, 0x57E);

struct BpwActorSearchTable {
    be<u32> mActors[100];
};
WWHD_SIZE(BpwActorSearchTable, 400);

// HD: the renderer keeps model/list pointers and the current matrix in J3DSys.
struct BpwJ3DSysHD {
    be<u32> mpModel;
    u8 _04[4];
    be<u32> mpOpaDrawBuffer, mpXluDrawBuffer;
    u8 _10[0x23C - 0x10];
    Mtx34 mCurrentMtx;
};
WWHD_SIZE(BpwJ3DSysHD, 0x26C);
WWHD_OFFSET(BpwJ3DSysHD, mCurrentMtx, 0x23C);

struct BpwSinCosHD {
    be<f32> mSin, mCos;
};
WWHD_SIZE(BpwSinCosHD, 8);

struct BpwPlaneHD {
    cXyz mNormal;
    be<f32> mD;
    be<u32> mVtable;
};
WWHD_SIZE(BpwPlaneHD, 0x14);

// HD: particle emitter direction, transform and color fields.
struct BpwEmitterHD {
    u8 _00[0x28];
    cXyz mLocalDirection;
    u8 _34[0x1F0 - 0x34];
    be<f32> mGlobalRotation[9];
    u8 _214[0xC];
    cXyz mGlobalScale;
    cXyz mGlobalTranslation;
    cXyz mGlobalParticleScale;
    GXColor mGlobalPrmColor, mGlobalEnvColor;
};
WWHD_SIZE(BpwEmitterHD, 0x24C);
WWHD_OFFSET(BpwEmitterHD, mGlobalRotation, 0x1F0);
WWHD_OFFSET(BpwEmitterHD, mGlobalPrmColor, 0x244);
struct BpwEnvLightHD {
    u8 _00[0xB70];
    GXColor mActorColor;
};
WWHD_SIZE(BpwEnvLightHD, 0xB74);
struct BpwPlayerVtableHD {
    u8 _00[0xEC];
    be<u32> setOutPower;
    u8 _F0[0x24];
    be<u32> setPlayerPosAndAngle;
};
WWHD_SIZE(BpwPlayerVtableHD, 0x118);
struct BpwCameraHD {
    u8 _00[0x236];
    be<u16> mAngleY;
    u8 _238[0x10];
    u8 mCamera;
};
WWHD_SIZE(BpwCameraHD, 0x249);
struct BpwChildPoeHD {
    u8 _00[0x460];
    be<u8> mKilledJalhalla;  /* pw_class view; was m344: Big_pow_down_check 0244A67C sets 1 when this Poe's hit takes Jalhalla's health to 0 */
};
WWHD_SIZE(BpwChildPoeHD, 0x461);

// Resource indices and joints follow the GameCube Bpw archive.
enum dRes_INDEX_BPW {
    /* BCK  */
    dRes_INDEX_BPW_BCK_ATTACK_KAN1_e = 0x5,
    dRes_INDEX_BPW_BCK_BOYON_L1_e = 0x6,
    dRes_INDEX_BPW_BCK_BOYON_S1_e = 0x7,
    dRes_INDEX_BPW_BCK_CORE1_e = 0x8,
    dRes_INDEX_BPW_BCK_CORE_NIGE1_e = 0x9,
    dRes_INDEX_BPW_BCK_DAMAGE1_e = 0xA,
    dRes_INDEX_BPW_BCK_DOSUN1_e = 0xB,
    dRes_INDEX_BPW_BCK_DOSUN_WAIT1_e = 0xC,
    dRes_INDEX_BPW_BCK_DOSUN_WAIT11_e = 0xD,
    dRes_INDEX_BPW_BCK_GATAGATA1_e = 0xE,
    dRes_INDEX_BPW_BCK_HIROU1_e = 0xF,
    dRes_INDEX_BPW_BCK_HIT1_e = 0x10,
    dRes_INDEX_BPW_BCK_HONOO1_e = 0x11,
    dRes_INDEX_BPW_BCK_HONOO_END1_e = 0x12,
    dRes_INDEX_BPW_BCK_HONOO_WAIT1_e = 0x13,
    dRes_INDEX_BPW_BCK_HUKKATU1_e = 0x14,
    dRes_INDEX_BPW_BCK_IKI1_e = 0x15,
    dRes_INDEX_BPW_BCK_IKI_END1_e = 0x16,
    dRes_INDEX_BPW_BCK_IKI_WAIT1_e = 0x17,
    dRes_INDEX_BPW_BCK_JITA1_e = 0x18,
    dRes_INDEX_BPW_BCK_JUMP1_e = 0x19,
    dRes_INDEX_BPW_BCK_KAN_DEFAULT1_e = 0x1A,
    dRes_INDEX_BPW_BCK_KYORO_GARN1_e = 0x1B,
    dRes_INDEX_BPW_BCK_MOVEB1_e = 0x1C,
    dRes_INDEX_BPW_BCK_MOVEF1_e = 0x1D,
    dRes_INDEX_BPW_BCK_MOVES1_e = 0x1E,
    dRes_INDEX_BPW_BCK_OPENING1_e = 0x1F,
    dRes_INDEX_BPW_BCK_OPENING2_e = 0x20,
    dRes_INDEX_BPW_BCK_OPENING_KAN1_e = 0x21,
    dRes_INDEX_BPW_BCK_OPENING_KAN2_e = 0x22,
    dRes_INDEX_BPW_BCK_PRESS1_e = 0x23,
    dRes_INDEX_BPW_BCK_SONOBA_PRESS1_e = 0x24,
    dRes_INDEX_BPW_BCK_SONOBA_PRESS2_e = 0x25,
    dRes_INDEX_BPW_BCK_SUIKOMU1_e = 0x26,
    dRes_INDEX_BPW_BCK_TAMA1_e = 0x27,
    dRes_INDEX_BPW_BCK_TAME1_e = 0x28,
    dRes_INDEX_BPW_BCK_TORITUKI1_e = 0x29,
    dRes_INDEX_BPW_BCK_WAIT1_e = 0x2A,
    dRes_INDEX_BPW_BCK_WARAU1_e = 0x2B,
    /* BDLM */
    dRes_INDEX_BPW_BDL_BPW_e = 0x2E,
    dRes_INDEX_BPW_BDL_BPW_KAN1_e = 0x2F,
    /* BRK  */
    dRes_INDEX_BPW_BRK_BPW_KAN1_e = 0x32,
    dRes_INDEX_BPW_BRK_DEFAULT_e = 0x33,
    dRes_INDEX_BPW_BRK_HIRARU1_e = 0x34,
    dRes_INDEX_BPW_BRK_HIT_e = 0x35,
    dRes_INDEX_BPW_BRK_NOROI_E1_e = 0x36,
    dRes_INDEX_BPW_BRK_NOROI_S1_e = 0x37,
};

enum BPW_JNT {
    BPW_JNT_BPW_ALLROOT_e = 0x0,
    BPW_JNT_BPW_SKLROOT_e = 0x1,
    BPW_JNT_J_BPW_KOSI1_e = 0x2,
    BPW_JNT_J_BPW_ASI_L1_e = 0x3,
    BPW_JNT_J_BPW_ASI_L2_e = 0x4,
    BPW_JNT_J_BPW_ASI_L3_e = 0x5,
    BPW_JNT_J_BPW_ASI_R1_e = 0x6,
    BPW_JNT_J_BPW_ASI_R2_e = 0x7,
    BPW_JNT_J_BPW_ASI_R3_e = 0x8,
    BPW_JNT_J_BPW_SIPPO1_e = 0x9,
    BPW_JNT_J_BPW_MUNE1_e = 0xA,
    BPW_JNT_J_BPW_MUNE2_e = 0xB,
    BPW_JNT_J_BPW_FUKU_BL_e = 0xC,
    BPW_JNT_J_BPW_FUKU_BR_e = 0xD,
    BPW_JNT_J_BPW_FUKU_FL_e = 0xE,
    BPW_JNT_J_BPW_FUKU_FR_e = 0xF,
    BPW_JNT_J_BPW_FUKU_SL_e = 0x10,
    BPW_JNT_J_BPW_FUKU_SR_e = 0x11,
    BPW_JNT_J_BPW_KATA_L1_e = 0x12,
    BPW_JNT_J_BPW_UDE_L1_e = 0x13,
    BPW_JNT_J_BPW_UDE_L2_e = 0x14,
    BPW_JNT_J_BPW_UDE_L3_e = 0x15,
    BPW_JNT_J_BPW_YUBI_L1_e = 0x16,
    BPW_JNT_J_BPW_YUBI_L2_e = 0x17,
    BPW_JNT_J_BPW_YUBI_L3_e = 0x18,
    BPW_JNT_J_BPW_YUBI_L4_e = 0x19,
    BPW_JNT_J_BPW_KATA_R1_e = 0x1A,
    BPW_JNT_J_BPW_UDE_R1_e = 0x1B,
    BPW_JNT_J_BPW_UDE_R2_e = 0x1C,
    BPW_JNT_J_BPW_UDE_R3_e = 0x1D,
    BPW_JNT_J_BPW_ITEM_e = 0x1E,
    BPW_JNT_J_BPW_YUBI_R1_e = 0x1F,
    BPW_JNT_J_BPW_YUBI_R2_e = 0x20,
    BPW_JNT_J_BPW_YUBI_R3_e = 0x21,
    BPW_JNT_J_BPW_YUBI_R4_e = 0x22,
    BPW_JNT_J_BPW_KUBI1_e = 0x23,
    BPW_JNT_J_BPW_ATAMA1_e = 0x24,
    BPW_JNT_J_BPW_AGO1_e = 0x25,
    BPW_JNT_J_BPW_BOUSI1_e = 0x26,
    BPW_JNT_J_BPW_KAMEN_SEIGYO1_e = 0x27,
    BPW_JNT_J_BPW_KAMEN1_e = 0x28,
    BPW_JNT_J_BPW_KAMEN2_e = 0x29,
};

enum BPW_KAN1_JNT {
    BPW_KAN1_JNT_TOTTE_e = 0x0,
    BPW_KAN1_JNT_KANTERA_e = 0x1,
    BPW_KAN1_JNT_FIRE_e = 0x2,
};

// HD: private guest layout used by this translation unit.
struct BpwHeaderStateHD {
    be<f32> mLow, mHigh;
    be<u8> mFirstObject, mSecondObject;
    u8 mPadding[2];
    be<u32> mZeroWords[4];
};
static_assert(sizeof(BpwHeaderStateHD) == 0x1C);

// HD: private guest layout used by this translation unit.
struct BpwBlurHD {
    be<u8> mEnabled, mRate;
};
WWHD_SIZE(BpwBlurHD, 2);

// HD: private guest layout used by this translation unit.
struct BpwVecHD {
    be<f32> x, y, z;
};
WWHD_SIZE(BpwVecHD, 0xC);

// HD: private guest layout used by this translation unit.
struct BpwResourceNameHD {
    be<u32> data;
    be<u32> vtable;
};
WWHD_SIZE(BpwResourceNameHD, 8);

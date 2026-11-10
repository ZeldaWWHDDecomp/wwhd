#pragma once
#include "bindings.h"

#include "m_Do/m_Do_ext.h"
#include "JSystem/J3DGraph.h"
#include "d/d_bg_s.h"
#include "d/d_cc_d.h"
#include "d/d_particle.h"

// HD: the emitter's global transform and flags lie beyond the shared JPABaseEmitter view (the HD
// emitter is larger than the GameCube one from 0x60 on).
struct BstEmitter : JPABaseEmitter {
    /* 0x070 */ u8 m070[0x1F0 - 0x70];
    /* 0x1F0 */ Mtx34 mGlobalRotation;    // GameCube 0x1A8
    /* 0x220 */ u8 m220[0xC];
    /* 0x22C */ cXyz mGlobalTranslation;  // GameCube 0x1E4
    /* 0x238 */ u8 m238[0x1C];
    /* 0x254 */ be<u32> mFlags;           // GameCube 0x20C
    /* GameCube inline: mMaxFrame = -1; stopCreateParticle() (HD loads the flags first) */
    void becomeInvalidEmitter() {
        u32 flags = mFlags;
        mMaxFrame = -1;
        mFlags = flags | 1;
    }
};
WWHD_OFFSET(BstEmitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(BstEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(BstEmitter, mFlags, 0x254);
WWHD_SIZE(BstEmitter, 0x258);
inline BstEmitter* bstEmitter(JPABaseEmitter* emitter) { return static_cast<BstEmitter*>(emitter); }

// Gohdan (head and both hands): the zeldaret/tww layout and member names at the HD offsets
// (one layout for all bst sources). HD: fopEn_enemy_c and dKy_tevstr_c are
// larger, so every member after mPhs sits 0x11C (0x234 after mEnvLight) above its tww offset;
// the descriptive meaning is noted where tww only has an offset name.
struct bst_class : fopEn_enemy_c {
    enum Part_Type_e {
        Type_HEAD_e = 0,
        Type_LEFT_HAND_e = 1,
        Type_RIGHT_HAND_e = 2,
    };

    enum ActionType {
        ACTION_STAY_e = 0,
        ACTION_FLY_e = 1,
        ACTION_DAMAGE_e = 5,
        ACTION_SLEEP_e = 6,
        ACTION_HEAD_DAMAGE_e = 7,
        ACTION_HEAD_HUKKI_e = 8,
        ACTION_DOWN_ATTACK_e = 10,
        ACTION_PAA_ATTACK_e = 11,
        ACTION_KUMI_ATTACK_e = 12,
        ACTION_HARAI_ATTACK_e = 13,
        ACTION_BEAM_ATTACK_e = 14,
        ACTION_HANA_DEMO_e = 20,
        ACTION_END_DEMO_e = 22,
    };

    /* 0x03C8 */ request_of_phase_process_class mPhs;
    /* 0x03D0 */ be<u8> mBstPartType;
    /* 0x03D1 */ u8 m02B5[3];
    /* 0x03D4 */ gptr<mDoExt_McaMorf> mpMorf;  /* was m02B8: useHeapInit 020E7278 creates the mDoExt_McaMorf of the head / hand body; daBst_Draw 020E4A64 enters it with mpTexMtxAnimator / mpTevRegAnimator; beam_attack 020E9F08, demo_camera 020EE200 and hana_demo 020EACE0 read its frame and joint matrices */
    /* 0x03D8 */ gptr<mDoExt_btkAnm> mpTexMtxAnimator;
    /* 0x03DC */ gptr<mDoExt_brkAnm> mpTevRegAnimator;
    /* 0x03E0 */ be<s8> mBodyAnmOn;  /* was m02C4: daBst_Execute 020E54A0 plays mpTevRegAnimator / mpTexMtxAnimator (the body animations) only while it is set; fly (in daBst_move 020EB4BC) and demo_camera 020EE200 set 1; animation state */
    /* 0x03E1 */ u8 m02C5[3];
    /* 0x03E4 */ gptr<J3DModel> mpPedestalModel;  /* was m02C8: useHeapInit 020E7278 creates it from set_za_bdl[mBstPartType]; daBst_Draw 020E4A64 draws it with mpBtkAnm / mpBrkAnm and mEnvLight at home.pos; bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) places it at home.pos / home.angle; pedestal model */
    /* 0x03E8 */ gptr<mDoExt_btkAnm> mpBtkAnm;
    /* 0x03EC */ gptr<mDoExt_brkAnm> mpBrkAnm;
    /* 0x03F0 */ be<s8> mPedestalAnmOn;  /* was m02D4: daBst_Execute 020E54A0 plays mpBrkAnm / mpBtkAnm (the mpPedestalModel animations) only while it is set; fly (in daBst_move 020EB4BC) and demo_camera 020EE200 set 1 */
    /* 0x03F1 */ u8 m02D5[3];
    /* 0x03F4 */ dKy_tevstr_c mEnvLight;
    /* 0x05BC */ gptr<J3DModel> mpShadowModel;  /* was m0388: useHeapInit 020E7278 creates a second model from the mpMorf model data; bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) gives it the body base matrix; zeldaret/tww daBst_Draw copies the body joint matrices into it and passes it to dComIfGd_setRealShadow */
    /* 0x05C0 */ be<u32> mShadowID;
    /* 0x05C4 */ gptr<J3DModel> mpBeamModel[10];  /* was m0390: useHeapInit 020E7278 creates the 10 BDL_TAMA models; beam_move 020EDC8C places each at mBeamPos / mBeamAngle, daBst_Draw 020E4A64 draws the ones whose mBeamState is set; beam models */
    /* 0x05EC */ cXyz mBeamPos[10];  /* was m03B8: beam_attack 020E9F08 sets it to the mouth joint (BST_JNT_BUKI2) when a beam fires; beam_move 020EDC8C adds mBeamSpeed, checks ground / walls there and moves mCcD_beams to it; daBst_Delete 020E7190 stops the sound there; beam positions */
    /* 0x0664 */ cXyz mBeamSpeed[10];  /* was m0430: beam_attack 020E9F08: (0, 0, REG0_F(12) + 160) turned by the head angle; beam_move 020EDC8C adds it to mBeamPos each frame; beam velocities */
    /* 0x06DC */ csXyz mBeamAngle[10];  /* was m04A8: beam_attack 020E9F08 copies shape_angle into it; beam_move 020EDC8C rotates mpBeamModel by its y / x and passes y to beam_eff_set on a wall hit; beam angles */
    /* 0x0718 */ be<s8> mBeamState[10];  /* was m04E4: beam_attack 020E9F08 takes a slot with 0 and sets 1; beam_move 020EDC8C starts mCcD_beams at 1 (then 2), moves it at 2, clears it on a hit or above 2000; daBst_Draw 020E4A64 draws mpBeamModel while it is set; beam states */
    /* 0x0722 */ u8 m04EE[2];
    /* 0x0724 */ dCcD_Sph mCcD_beams[10];
    /* 0x12DC */ be<f32> mBeamLength[10];  /* was m10A8: beam_attack 020E9F08 sets 1.0; beam_move 020EDC8C scales mpBeamModel along z by it (scaleM(1, 1, it)) and grows it toward REG0_F(4) + 5; beam lengths */
    /* 0x1304 */ be<f32> mBeamAimOffsetY;  /* was m10D0: beam_attack 020E9F08 adds it to the player height difference for the aim pitch and eases it to REG0_F(6); main_cont (in daBst_Execute 020E54A0) sets REG0_F(7) - 500 when the beam attack starts */
    /* 0x1308 */ be<s16> mUpdateLastFacingDirIfMultipleOf32;
    /* 0x130A */ be<s16> mActionType;
    /* 0x130C */ be<s16> mPrevAction;  /* was m10D8: fly (in daBst_move 020EB4BC) sets 1 (ACTION_FLY_e), sleep 6 (ACTION_SLEEP_e); fly refills a hand's health to 4 when it is still 6 (the hand comes back from sleep) */
    /* 0x130E */ be<s16> mDamage;
    /* 0x1310 */ cXyz mTargetPos;
    /* 0x131C */ be<s16> mLastCheckedPlayerFacingDir;
    /* 0x131E */ u8 m10EA[2];
    /* 0x1320 */ cXyz mMoveParam;  /* was m10EC: x: turn-step limit, y: its ramp, z: shake amplitude; pos_move 020E4CD0 turns current.angle by at most x * y and eases y to 1; the attack actions (kumi 020E90F0, harai 020E9AA0, paa 020E888C, daBst_move 020EB4BC) set x and clear y; bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) shakes the position by z and eases z to 0; turn scale */
    /* 0x132C */ be<f32> mSwayAngle;  /* was m10F8: head_hukki (in daBst_move 020EB4BC) eases it to REG0_F(4) + 4000 and back to 0 and sets shape_angle.x / z = it * sin; bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) eases it to 0 */
    /* 0x1330 */ be<s16> mTimers[5];  /* was m10FC: daBst_Execute 020E54A0 counts all 5 down; [0] action steps (beam_attack 020E9F08 45 / 50, harai 020E9AA0, hana_demo 020EACE0 40), [1] sleep (main_cont 100 / 3), [3] end_demo 020EB174; zeldaret/tww: 0 = mUpdateTargetPosTimer, 1 = mSleepTimer; timers: 0 = update target pos, 1 = sleep */
    /* 0x133A */ be<s16> mDamageTimer;
    /* 0x133C */ be<s16> mState;
    /* 0x133E */ be<s16> mStateTimer;
    /* 0x1340 */ u8 m110C[4];
    /* 0x1344 */ be<f32> mBodyOffsetX;  /* was m1110: bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) translates the body matrix by (it, mBodyOffsetY, mBodyOffsetZ) after the rotation and eases it to 0; kumi 020E90F0 / harai 020E9AA0 ease it up */
    /* 0x1348 */ be<f32> mBodyOffsetY;  /* was m1114: bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) translates the body matrix by it on y and eases it to 0; kumi 020E90F0 and down_attack (in daBst_move 020EB4BC) ease it to -200 */
    /* 0x134C */ be<f32> mBodyOffsetZ;  /* was m1118: bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) translates the body matrix by it on z and eases it to 0; paa 020E888C eases it to -40, kumi 020E90F0 to its target */
    /* 0x1350 */ be<s16> mAttackBaseYaw;  /* was m111C: paa 020E888C and kumi 020E90F0 store the player's shape_angle.y in it at the start and turn the hand around it (YrotS(it), shape_angle.y toward it + offset) */
    /* 0x1352 */ u8 m111E[2];
    /* 0x1354 */ be<f32> mHeadHurtSpeed;
    /* 0x1358 */ be<s16> mHurtRecoilAngle1;
    /* 0x135A */ be<s16> mHurtRecoilAngle2;
    /* 0x135C */ be<s16> mHeadHurtAngle;
    /* 0x135E */ be<s16> mImpactShakeTimer;  /* was m112A: down_attack / head_damage (in daBst_move 020EB4BC), paa 020E888C, kumi 020E90F0 and demo_camera 020EE200 set REG + 10 / 4 / 5 / 3 on an impact; daBst_Execute 020E54A0 counts it down; bstExecuteTransformAndCollision shakes the body yaw / pitch by it * (REG0_F(14) + 150) */
    /* 0x1360 */ be<s16> mPitchShakeTimer;  /* was m112C: daBst_Execute 020E54A0 counts it down; bstExecuteTransformAndCollision (in daBst_Execute) shakes the body pitch by it * (REG0_F(14) + 100); no writer in HD or zeldaret/tww */
    /* 0x1362 */ u8 m112E[2];
    /* 0x1364 */ dBgS_AcchCir mAcchCir;
    /* 0x13A4 */ dBgS_ObjAcch mAcch;
    /* 0x1568 */ dCcD_Stts mStts;
    /* 0x15A4 */ dCcD_Cyl mHeadHurtCyl;
    /* 0x16D4 */ dCcD_Cyl mHandHurtCyl;
    /* 0x1804 */ dCcD_Sph mFingerSphs[19];
    /* 0x2E48 */ dCcD_Sph mEyeSphs[2];
    /* 0x30A0 */ u8 m2E6C[2];
    /* 0x30A2 */ be<s16> mDownAttackYaw;  /* was m2E6E: down_attack (in daBst_move 020EB4BC) sets shape_angle.y +/- 0x4000 and turns shape_angle.y toward it */
    /* 0x30A4 */ be<s16> mFacingDir;
    /* 0x30A6 */ be<s8> mEyeHealth[2];
    /* 0x30A8 */ be<s16> mEyeCloseTimer[2];  /* was m2E74: beam_attack 020E9F08 and main_cont (in daBst_Execute 020E54A0) set 10; bstExecuteTransformAndCollision counts it down, keeps the eyelid (mEyelidAngle) closed and the eye target (mEyeAttIds) off while it is set; eye timers */
    /* 0x30AC */ be<s16> mEyelidAngle[2];  /* was m2E78: bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) eases it to 0 (open) or REG0_S(8) + 11000 (closed); nodeCallBackHead 020E48F8 rotates the eyelid joints by it; eyelid angles */
    /* 0x30B0 */ be<s8> mBattlePhase;  /* was m2E7C: main_cont (in daBst_Execute 020E54A0) switch: 0 start, 1 wait for the player (then 2 with mDemoCamMode 10 for the intro, or 10 when the intro was seen), 10 -> 11 hand attacks, 12 head (beam) phase; fly / daBst_move 020EB4BC read it, demo_camera 020EE200 sets 10 / 100; head phase */
    /* 0x30B1 */ u8 m2E7D[1];
    /* 0x30B2 */ be<s16> mBattleTimers[3];  /* was m2E7E: main_cont (in daBst_Execute 020E54A0) counts all 3 down; [0] next hand attack, [1] next beam attack (beam_attack 020E9F08 reloads it; zeldaret/tww: 1 = mHeadBeamAttackTimer), [2] head phase delay (50); head timers (1 = mHeadBeamAttackTimer) */
    /* 0x30B8 */ be<s16> mBombArrowDropTimer;
    /* 0x30BA */ u8 m2E86[2];
    /* 0x30BC */ be<u32> mBombId;
    /* 0x30C0 */ be<u32> mCreatedItemId; /* nose item */
    /* 0x30C4 */ be<u32> mEyeAttIds[2];  /* was m2E90: daBst_Create 020E7DC0 creates two fpcNm_ATT_e actors and keeps their ids; bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) puts them on the eye joints with mEyeHealth; daBst_Delete 020E7190 deletes them; eye attachment ids */
    /* 0x30CC */ be<s16> mBlurRate;  /* was m2E98: daBst_Draw 020E4A64 turns the motion blur on with this rate while it is > 1 and off at 1 (zeldaret/tww: mDoGph_gInf_c::setBlureRate); demo_camera 020EE200 sets 100 / 120 / 150 / 1; head light mode */
    /* 0x30CE */ be<s8> mDemoCamMode;  /* was m2E9A: demo_camera 020EE200 switch: CAMERA_IDLE, CAMERA_LEFT_HAND .. CAMERA_FOLLOW_ITEM; the original's report "K DEMO MODET %d" prints it; main_cont (in daBst_Execute 020E54A0) sets 10 and skips the battle while it is set (mDemoMode is the player member read in demo_camera); camera state */
    /* 0x30CF */ u8 m2E9B[1];
    /* 0x30D0 */ be<s16> msFrameCount; /* camera frame */
    /* 0x30D2 */ be<s8> mItemNoseSide;  /* was m2E9E: hana_demo 020EACE0 sets 0 (arrows from BST_JNT_HANAL) / 1 (bombs from BST_JNT_HANAR) and picks the smoke effect by it; demo_camera 020EE200 places the camera on that side; camera side */
    /* 0x30D3 */ u8 m2E9F[1];
    /* 0x30D4 */ cXyz mCamEye;  /* was m2EA0: demo_camera 020EE200 moves it and passes it as the eye of dCamera_c::Set (with the mCamShake offset); camera eye */
    /* 0x30E0 */ cXyz mCamCenter;  /* was m2EAC: demo_camera 020EE200 moves it and passes it as the center of dCamera_c::Set (with the mCamShake offset); camera center */
    /* 0x30EC */ u8 m2EB8[0xC];
    /* 0x30F8 */ be<f32> mCamWork;  /* was m2EC4: demo_camera 020EE200: a ramp 0 -> 1 that scales the eye chase steps (40 / 100 / 10 * it), and in CAMERA_NOSE an eye height offset eased to -650; camera chase speed */
    /* 0x30FC */ be<f32> mCamFovy;  /* was m2EC8: demo_camera 020EE200 sets 55 / 75 and eases it to 30 / 55; passes it as the fovy of dCamera_c::Set; camera fovy */
    /* 0x3100 */ be<f32> mCamShake;  /* was m2ECC: demo_camera 020EE200 sets REG + 10, shakes eye and center by it * sin / cos (bank 7.5 * it) and eases it to 0; stay (in daBst_move 020EB4BC) sets it on the head; camera shake */
    /* 0x3104 */ be<f32> mRoomColorBlend;  /* was m2ED0: bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) passes it to the custom color set by mRoomState and eases it to 1 / 0; daBst_Create 020E7DC0 and demo_camera 020EE200 set 1 / 0; room lighting blend */
    /* 0x3108 */ be<s8> mRoomState;
    /* 0x3109 */ u8 m2ED5[3];
    /* 0x310C */ gptr<JPABaseEmitter> mpChargeEmitter[2];  /* was m2ED8: beam_attack 020E9F08 sets the two charge_e_name emitters and keeps them on joint 0; it and daBst_Execute 020E54A0 retire them outside the beam attack; beam charge emitters */
    /* 0x3114 */ gptr<JPABaseEmitter> mpBombEmitter[5];  /* was m2EE0: head_damage (in daBst_move 020EB4BC) sets [0] = 0x81E3 (ID_AK_SN_BSTBOMBFLASH00) and [1..] = the bomb effects on the head joint; end_demo 020EB174 retires [1..]; HD: 5 words where tww has mpBombEmitter[2] + 12 gap bytes */
    /* 0x3128 */ gptr<JPABaseEmitter> mpHaloEmitter;  /* was m2EF4: end_demo 020EB174 sets particle 0x81E8 (ID_AK_SN_BSTHALO00) on the head joint */
    /* 0x312C */ gptr<JPABaseEmitter> mpStageEffEmitter;  /* was m2EF8: daBst_Create 020E7DC0 and demo_camera 020EE200 set particle 0x81E9 (ID_AK_SN_BSTSTAGEEFFECT00); demo_camera retires it */
    /* 0x3130 */ u8 m2EFC[4];
    /* 0x3134 */ dPa_smokeEcallBack mPa_smokeEcallBack;
    /* 0x3154 */ dKy_tevstr_c mBgTevStr;  /* was m2F20: daBst_Draw 020E4A64 sets it up as TEV type 1 (TEV_TYPE_BG0) at center_pos and lights mpHbyokoModel / mpHbsitaModel with it; bstConstructor 020E78C4 / daBst_Create 020E7DC0 initialise it */
    /* 0x331C */ gptr<J3DModel> mpHbsitaModel;  /* was m2FD0: useHeapInit 020E7278 creates it from dRes_INDEX_BST_BDL_HBSITA1_e; daBst_Draw 020E4A64 draws it with mpHbsitaBtkAnm / mpHbsitaBrkAnm; daBst_Execute 020E54A0 sets its base matrix; auxiliary model A */
    /* 0x3320 */ gptr<mDoExt_btkAnm> mpHbsitaBtkAnm;  /* was m2FD4: useHeapInit 020E7278 creates it with dRes_INDEX_BST_BTK_HBSITA1_e for mpHbsitaModel; daBst_Draw 020E4A64 enters it, daBst_Execute 020E54A0 plays it */
    /* 0x3324 */ gptr<mDoExt_brkAnm> mpHbsitaBrkAnm;  /* was m2FD8: useHeapInit 020E7278 creates it with dRes_INDEX_BST_BRK_HBSITA1_e for mpHbsitaModel; daBst_Execute 020E54A0 loops it while the player is below the limit height and plays it */
    /* 0x3328 */ gptr<J3DModel> mpHbyokoModel;  /* was m2FDC: useHeapInit 020E7278 creates it from dRes_INDEX_BST_BDL_HBYOKO1_e; daBst_Draw 020E4A64 draws it with mpHbyokoBrkAnm; daBst_Execute 020E54A0 sets its base matrix; auxiliary model B */
    /* 0x332C */ gptr<mDoExt_brkAnm> mpHbyokoBrkAnm;  /* was m2FE0: useHeapInit 020E7278 creates it with dRes_INDEX_BST_BRK_HBYOKO1_e for mpHbyokoModel; daBst_Execute 020E54A0 plays it; main_cont / demo_camera 020EE200 set its rate to 1 */
    /* 0x3330 */ be<s8> mBattleCamOn;  /* was m2FE4: daBst_Execute 020E54A0 sets the camera type "Test01" while it is set; main_cont (in daBst_Execute) sets 1 when the intro was seen (STAGE_BOSS_DEMO), demo_camera 020EE200 sets 0 at the intro start and 1 at its end; intro complete */
    /* 0x3331 */ be<u8> mHioSet;
    /* 0x3332 */ u8 m2FE6[2];
};
WWHD_OFFSET(bst_class, mPhs, 0x3C8);
WWHD_OFFSET(bst_class, mBstPartType, 0x3D0);
WWHD_OFFSET(bst_class, mpMorf, 0x3D4);
WWHD_OFFSET(bst_class, mpPedestalModel, 0x3E4);
WWHD_OFFSET(bst_class, mEnvLight, 0x3F4);
WWHD_OFFSET(bst_class, mpShadowModel, 0x5BC);
WWHD_OFFSET(bst_class, mpBeamModel, 0x5C4);
WWHD_OFFSET(bst_class, mBeamPos, 0x5EC);
WWHD_OFFSET(bst_class, mBeamSpeed, 0x664);
WWHD_OFFSET(bst_class, mBeamAngle, 0x6DC);
WWHD_OFFSET(bst_class, mBeamState, 0x718);
WWHD_OFFSET(bst_class, mCcD_beams, 0x724);
WWHD_OFFSET(bst_class, mBeamLength, 0x12DC);
WWHD_OFFSET(bst_class, mBeamAimOffsetY, 0x1304);
WWHD_OFFSET(bst_class, mActionType, 0x130A);
WWHD_OFFSET(bst_class, mDamage, 0x130E);
WWHD_OFFSET(bst_class, mTargetPos, 0x1310);
WWHD_OFFSET(bst_class, mMoveParam, 0x1320);
WWHD_OFFSET(bst_class, mTimers, 0x1330);
WWHD_OFFSET(bst_class, mState, 0x133C);
WWHD_OFFSET(bst_class, mBodyOffsetX, 0x1344);
WWHD_OFFSET(bst_class, mAttackBaseYaw, 0x1350);
WWHD_OFFSET(bst_class, mHeadHurtSpeed, 0x1354);
WWHD_OFFSET(bst_class, mHeadHurtAngle, 0x135C);
WWHD_OFFSET(bst_class, mImpactShakeTimer, 0x135E);
WWHD_OFFSET(bst_class, mPitchShakeTimer, 0x1360);
WWHD_OFFSET(bst_class, mAcchCir, 0x1364);
WWHD_OFFSET(bst_class, mAcch, 0x13A4);
WWHD_OFFSET(bst_class, mStts, 0x1568);
WWHD_OFFSET(bst_class, mHeadHurtCyl, 0x15A4);
WWHD_OFFSET(bst_class, mHandHurtCyl, 0x16D4);
WWHD_OFFSET(bst_class, mFingerSphs, 0x1804);
WWHD_OFFSET(bst_class, mEyeSphs, 0x2E48);
WWHD_OFFSET(bst_class, mFacingDir, 0x30A4);
WWHD_OFFSET(bst_class, mEyeHealth, 0x30A6);
WWHD_OFFSET(bst_class, mEyeCloseTimer, 0x30A8);
WWHD_OFFSET(bst_class, mEyelidAngle, 0x30AC);
WWHD_OFFSET(bst_class, mBattlePhase, 0x30B0);
WWHD_OFFSET(bst_class, mBattleTimers, 0x30B2);
WWHD_OFFSET(bst_class, mBombArrowDropTimer, 0x30B8);
WWHD_OFFSET(bst_class, mBombId, 0x30BC);
WWHD_OFFSET(bst_class, mCreatedItemId, 0x30C0);
WWHD_OFFSET(bst_class, mEyeAttIds, 0x30C4);
WWHD_OFFSET(bst_class, mBlurRate, 0x30CC);
WWHD_OFFSET(bst_class, mDemoCamMode, 0x30CE);
WWHD_OFFSET(bst_class, msFrameCount, 0x30D0);
WWHD_OFFSET(bst_class, mCamEye, 0x30D4);
WWHD_OFFSET(bst_class, mCamCenter, 0x30E0);
WWHD_OFFSET(bst_class, mCamWork, 0x30F8);
WWHD_OFFSET(bst_class, mCamShake, 0x3100);
WWHD_OFFSET(bst_class, mRoomColorBlend, 0x3104);
WWHD_OFFSET(bst_class, mRoomState, 0x3108);
WWHD_OFFSET(bst_class, mpChargeEmitter, 0x310C);
WWHD_OFFSET(bst_class, mpBombEmitter, 0x3114);
WWHD_OFFSET(bst_class, mpHaloEmitter, 0x3128);
WWHD_OFFSET(bst_class, mpStageEffEmitter, 0x312C);
WWHD_OFFSET(bst_class, mPa_smokeEcallBack, 0x3134);
WWHD_OFFSET(bst_class, mBgTevStr, 0x3154);
WWHD_OFFSET(bst_class, mpHbsitaModel, 0x331C);
WWHD_OFFSET(bst_class, mpHbyokoBrkAnm, 0x332C);
WWHD_OFFSET(bst_class, mBattleCamOn, 0x3330);
WWHD_OFFSET(bst_class, mHioSet, 0x3331);
WWHD_SIZE(bst_class, 0x3334);

// HD: the split translation unit defines this descriptor once in d_a_bst_execute.cpp.
struct gc_tu_data;
extern const gc_tu_data gc_tu;

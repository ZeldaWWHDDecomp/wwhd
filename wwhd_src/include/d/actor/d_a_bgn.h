/* bgn_class (Puppet Ganon, phase 1: the marionette), WWHD layout.
 *
 * GameCube -> WWHD, measured from the inline constructor 0208064C, the deleting destructor
 * 0208ACD8, daBgn_Delete and useHeapInit:
 * - fopEn_enemy_c is followed by an HD word (the constructor stores `this` at 0x3C8), so mPhase
 *   is at 0x3CC and the members up to the first model packet are +0x120;
 * - mDoExt_J3DModelPacketS grew from 0x14 to 0xB0 (constructor 02080404) and dKy_tevstr_c from
 *   0xB0 to 0x1C8, so part_s is 0x3F0 (GameCube 0x23C);
 * - mDoExt_3DlineMat1_c grew from 0x3C to 0x188;
 * - HD inserts three more model packets after mLastHimoSway (0x14C84, 0x14D34, 0x14DE4).
 * Size 0x15768 (GameCube 0xCC94). */
#pragma once
#include "bindings.h"
#include "d/actor/d_a_bgn_packet.h"

/* dKy_tevstr_c HD fields used here: the fog colour is GXColorS10-like (s16 r, g, b) */
struct dKy_tevstr_l {
    /* 0x000 */ u8 _000[0xA0];
    /* 0x0A0 */ be<s16> mFogColorR;
    /* 0x0A2 */ be<s16> mFogColorG;
    /* 0x0A4 */ be<s16> mFogColorB;
    /* 0x0A6 */ u8 _0A6[2];
    /* 0x0A8 */ be<f32> mFogStartZ;
    /* 0x0AC */ u8 _0AC[0x1C8 - 0xAC];
};
WWHD_SIZE(dKy_tevstr_l, 0x1C8);

struct part_s {
    /* 0x000 */ gptr<J3DModel> mpPartModel;
    /* 0x004 */ mDoExt_J3DModelPacketS_l mPartPacket;  /* was m004: part_init 0207FDA4 / useHeapInit 0207FE14 set it up (the part's HD reflection packet); part_draw 0207ACD0 gives it the part model and updates it while l_HIO.mReflectionOn is set; part_s_ct 02080498 / part_s_dt 0208AC78 construct and destroy it (as bgn_class mChestPacket) */
    /* 0x0B4 */ dKy_tevstr_l mPartTevStr;
    /* 0x27C */ be<s16> mPartFogColorAdd;  /* was m0C8: part_draw 0207ACD0 eases it to 0x118 / 0 while mPartArrowHitFlashTimer runs (above / below 40) and adds it to the fog colour of mPartTevStr (same use as bgn_class mFogColorAdd) */
    /* 0x27E */ u8 _27E[2];
    /* 0x280 */ be<f32> mPartFogStartAdd;  /* was m0CC: part_draw 0207ACD0 eases it to -50000 / 0 while mPartArrowHitFlashTimer runs and adds it to mFogStartZ of mPartTevStr (same use as bgn_class mFogStartAdd) */
    /* 0x284 */ be<s16> mPartArrowHitFlashTimer;
    /* 0x286 */ be<s8> mPartCoreHitFrame;  /* was m0D2: useHeapInit 0207FE14 sets 16 (head), 10 (pelvis), i * 3 + 13 (arms), i * 3 + 10 (legs), 11 - i (tail); part_mtx_set 0207DE64 starts the part's arrow-hit flash (mPartArrowHitFlashTimer 50, mPartArrowHitEffectTimer 100) when bgn_class mCoreHitFrame equals it */
    /* 0x287 */ u8 _287[1];
    /* 0x288 */ cXyz mPartPos;  /* was m0D4: move 0208857C (MtxPosition) and part_control_0 0207D858 / part_control_2 0207DC84 chain the parts by it; part_mtx_set 0207DE64 translates the part matrix to it (not below mGroundY) and puts the arrow-hit emitters there; part_draw 0207ACD0 lights the part at it; punch_LR (in action_main 02086130) / hensin 02084D6C read the hand / head position from it */
    /* 0x294 */ csXyz mPartAngle;  /* was m0E0: part_control_0 0207D858 and move 0208857C (part_control_0Z) set .x / .y from the chain direction; part_mtx_set 0207DE64 eases .z toward mBodyAngle.y and rotates the part matrix by it; move rotates the leg roots by the pelvis part's */
    /* 0x29A */ u8 _29A[0x2A8 - 0x29A];
    /* 0x2A8 */ be<f32> mPartScale;  /* was m0F4: move 0208857C (size_set) sets it per limb from l_HIO.mHeadScale .. mTailRootScale; part_mtx_set 0207DE64 scales the part matrix and mPartSph by it * l_HIO.mScale; part_control_0 0207D858 / part_control_2 0207DC84 space the chain by it; part_draw 0207ACD0 compares the camera distance with l_HIO.mPartDrawDist * it */
    /* 0x2AC */ dCcD_Sph mPartSph;
    /* 0x3D8 */ cXyz mPartCenter;  /* was m224: part_mtx_set 0207DE64 stores the transformed part origin (also the mPartSph centre) in it; part_draw 0207ACD0 measures its distance to the camera eye and draws the part only beyond l_HIO.mPartDrawDist * mPartScale */
    /* 0x3E4 */ gptr<JPABaseEmitter> mpPartArrowHitEmitter1;
    /* 0x3E8 */ gptr<JPABaseEmitter> mpPartArrowHitEmitter2;
    /* 0x3EC */ be<s16> mPartArrowHitEffectTimer;
    /* 0x3EE */ u8 _3EE[2];
};
WWHD_OFFSET(part_s, mPartTevStr, 0xB4);
WWHD_OFFSET(part_s, mPartPos, 0x288);
WWHD_OFFSET(part_s, mPartSph, 0x2AC);
WWHD_SIZE(part_s, 0x3F0);

struct move_s {
    /* 0x000 */ cXyz mHimo[60];
    /* 0x2D0 */ be<u8> mCut;  /* was m2D0: daAtt_Execute 020587B4 (d_a_att, view bgn_att_move_s) sets 1 when the string's mHitPoints reach 0; part_control_0 0207D858 / part_mtx_set 0207DE64 let the hanging part drop while it is set; head_recover 02084B90 clears it; action_s 02085630 clears it in HIO pose mode */
    /* 0x2D1 */ u8 _2D1[3];
    /* 0x2D4 */ cXyz mPose;  /* was m2D4: action_s 02085630 eases it toward mPoseTarget by mPoseSpeed; dance_0 (in action_main 02086130) and the part code add it to the string's limb position */
    /* 0x2E0 */ csXyz mPoseTarget;  /* was m2E0: start 020844F4 / dance_0 and tail_attack 0208459C (in action_main 02086130) load the pose angles (bgnPoses, dance_pause tables); action_s 02085630 eases mPose toward it */
    /* 0x2E6 */ u8 _2E6[2];
    /* 0x2E8 */ be<f32> m2E8;
    /* 0x2EC */ be<f32> mSwayAmplitude;  /* was m2EC: damage 02084804 / body_attack (in action_main 02086130) ease it to 250; action_s_himo 020859AC eases it to 0 and swings the rope points by sin * it; daAtt_Execute 020587B4 (d_a_att, view bgn_att_move_s) targets the string only below 1 */
    /* 0x2F0 */ u8 _2F0[4];
    /* 0x2F4 */ be<f32> mLimbShakeAmplitude;  /* was m2F4: action_s 02085630 eases it to 300 + mLimbShakeTimer * 20; damage 02084804 / hensin 02084D6C / punch_LR (in action_main 02086130) set 8000 / 6000 / 5000; part_control_0 0207D858 and move 0208857C shake the limb by sin / cos * it */
    /* 0x2F8 */ be<s16> mLimbShakeTimer;  /* was m2F8: damage 02084804 / hensin 02084D6C / dance_0 (in action_main 02086130) set it (0x46, 60, ...); action_s 02085630 counts it down and raises the mLimbShakeAmplitude target by it */
    /* 0x2FA */ be<s16> mPhaseSin;  /* was m2FA: daBgn_Create 02080BA4 sets a random start; action_s 02085630 adds 2600 + 3 * index per frame; the phase of the sin terms of mLimbShakeAmplitude (part_control_0 0207D858, move 0208857C) and mCutSwayAmplitude (action_s_himo 020859AC) */
    /* 0x2FC */ be<s16> mPhaseCos;  /* was m2FC: daBgn_Create 02080BA4 sets a random start; action_s 02085630 adds 2300 + 3 * index per frame; the phase of the cos terms of mLimbShakeAmplitude and mCutSwayAmplitude */
    /* 0x2FE */ be<s16> m2FE;
    /* 0x300 */ be<s16> mPluckTimer;  /* was m300: daAtt_Execute 020587B4 (d_a_att, view bgn_att_move_s) sets 15 when a hit does not cut the string; action_s_himo 020859AC counts it down and vibrates the rope by it * sin; part_mtx_set 0207DE64 sums [0] + [1] */
    /* 0x302 */ u8 _302[2];
    /* 0x304 */ be<f32> mCutSwayAmplitude;  /* was m304: daAtt_Execute 020587B4 (d_a_att, view bgn_att_move_s) sets 6000 when the string is cut; action_s_himo 020859AC swings the rope by sin / cos (mPhaseSin / mPhaseCos) * it while above 0.01 and eases it to 0 */
    /* 0x308 */ be<s8> mHitPoints;  /* was m308: daAtt_Execute 020587B4 (d_a_att, view bgn_att_move_s) subtracts 1 per hit on the string and cuts it (mCut) at 0; head_recover 02084B90 sets 2 on the restored strings, daBgn_Create 02080BA4 3 */
    /* 0x309 */ u8 _309[3];
};
WWHD_SIZE(move_s, 0x30C);

/* mDoExt_3DlineMat1_c (HD 0x188): line array pointer at +0x184 (line i: +0 positions, +4 sizes,
 * 0x?? per line) [as in d_a_bgn2.h] */
struct mDoExt_3DlineMat1_bgn {
    /* 0x000 */ u8 _000[0x130];
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 _134[0x184 - 0x134];
    /* 0x184 */ be<u32> mpLines;
};
WWHD_SIZE(mDoExt_3DlineMat1_bgn, 0x188);

struct bgn_class : fopEn_enemy_c {
    /* 0x3C8 */ be<u32> mSelf;  /* was m3C8: bgn_class_ct 0208064C stores gabi::ea(this) (HD); daBgn_Draw 0207B2CC sets mHdPacket[i].mA4 from it + 0x110; HD: `this` (stored by the constructor) */
    /* 0x3CC */ request_of_phase_process_class mPhase;
    /* 0x3D4 */ be<u8> mParam;  /* was m02B4: daBgn_Create 02080BA4: (u8)fopAcM_GetParam; 0xFF makes daBgn_Execute 0207E7B4 return at once and daBgn_Create skip the setup */
    /* 0x3D5 */ be<s8> mForm;  /* was m02B5: daBgn_Draw 0207B2CC draws the bgn (0), bgn2 (1) or bgn3 (2) form by it; hensin 02084D6C sets 1, daBgn_Execute 0207E7B4 2 / 3; move 0208857C acts only at 0 */
    /* 0x3D6 */ u8 _3D6[2];
    /* 0x3D8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3DC */ gptr<J3DModel> mpChestModel;
    /* 0x3E0 */ mDoExt_J3DModelPacketS_l mChestPacket;  /* was m02C0: daBgn_Draw 0207B2CC: model packet of mpChestModel (setModel + update); useHeapInit 0207FE14 sets it up */
    /* 0x490 */ gptr<JPABaseEmitter> mpArrowHitEmitter1;
    /* 0x494 */ gptr<JPABaseEmitter> mpArrowHitEmitter2;
    /* 0x498 */ be<s16> mArrowHitEffectTimer;
    /* 0x49A */ u8 _49A[2];
    /* 0x49C */ gptr<J3DModel> mpJyakutenCModel;
    /* 0x4A0 */ gptr<J3DModel> mpJyakutenBModel;
    /* 0x4A4 */ gptr<J3DModel> mpJyakutenAModel;
    /* 0x4A8 */ gptr<mDoExt_brkAnm> mJyakutenCBrkAnm;
    /* 0x4AC */ gptr<mDoExt_brkAnm> mJyakutenBBrkAnm;
    /* 0x4B0 */ u8 _4B0[4];
    /* 0x4B4 */ be<s16> mFogColorAdd;  /* was m02F8: daBgn_Draw 0207B2CC (bgn_fog_flash): eased to 0x118 / 0, added to the fog colour of tevStr */
    /* 0x4B6 */ u8 _4B6[2];
    /* 0x4B8 */ be<f32> mFogStartAdd;  /* was m02FC: daBgn_Draw 0207B2CC (bgn_fog_flash): eased to -50000 / 0, added to mFogStartZ of tevStr */
    /* 0x4BC */ be<s16> mArrowHitFlashTimer;
    /* 0x4BE */ be<s16> mCoreHitFrame;  /* was m0302: move 0208857C (damage_check) sets 1 on a core hit; daBgn_Execute 0207E7B4 counts it up to 100, then clears it; at 0xD move starts the arrow hit flash; part_mtx_set 0207DE64 compares it with part mPartCoreHitFrame */
    /* 0x4C0 */ be<s16> mRippleSeTimer;  /* was m0304: action_s_himo 020859AC: cM_rndF(20) + 20 frames until the next ripple sound; daBgn_Execute 0207E7B4 counts it down */
    /* 0x4C2 */ u8 _4C2[2];
    /* 0x4C4 */ cXyz mRipplePos;  /* was m0308: action_s_himo 020859AC / move 0208857C: position of the JA_SE_CM_BGN_STRING_RIPPLE / BODY_RIPPLE sound; daBgn_Delete 0207FC64 deletes its sound object */
    /* 0x4D0 */ part_s mHeadParts[2];
    /* 0xCB0 */ part_s mPelvisParts[2];
    /* 0x1490 */ part_s mLeftArmParts[21];
    /* 0x6740 */ part_s mRightArmParts[21];
    /* 0xB9F0 */ part_s mLeftLegParts[4];
    /* 0xC9B0 */ part_s mRightLegParts[4];
    /* 0xD970 */ part_s mTailParts[21];
    /* 0x12C20 */ move_s mMove[8];  /* was mAAA8: the 8 move_s (strings and their limb pose: mCut cut, mPoseTarget pose target); part_control_0 0207D858 / part_mtx_set 0207DE64 / start 020844F4 / action_main 02086130 per string */
    /* 0x14480 */ cXyz mBodyPos;  /* was mC308: move 0208857C places the chest model there (+ l_HIO.mBodyDrawOffsetY, at least mGroundY + 100); hensin 02084D6C hands it to the bgn2 form as current.pos; body_attack (in action_main 02086130) reads it */
    /* 0x1448C */ csXyz mBodyAngle;  /* was mC314: move 0208857C rotates the chest model by it (y = mBaseAngleY + mDanceAngleY); hensin 02084D6C hands it to the bgn2 form as current.angle / shape_angle */
    /* 0x14492 */ be<s16> mBaseAngleY;  /* was mC31A: move 0208857C eases it (cLib_addCalcAngleS2) and sets mBodyAngle.y = it + mDanceAngleY */
    /* 0x14494 */ u8 _14494[8];
    /* 0x1449C */ be<f32> mArmLength[2];  /* was mC324: move 0208857C eases [i] toward l_HIO.mArmPartSpacing + stretch and passes it (* l_HIO.mScale) to part_control_0 for the arms; part_mtx_set 0207DE64 scales the arm parts by it */
    /* 0x144A4 */ be<f32> mArmStretch[2];  /* was mC32C: punch_LR (in action_main 02086130) eases [i] to 1, move 0208857C back to 0; size_set (in move) pulls the arm part lengths toward the middle by it */
    /* 0x144AC */ be<f32> mPoseSpeed;  /* was mC334: action_s 02085630: step of cLib_addCalc2 of each move_s pose (m2D4 toward m2E0); action_main 02086130 eases it to mPoseSpeedTarget */
    /* 0x144B0 */ be<f32> mPoseSpeedTarget;  /* was mC338: punch_LR (in action_main 02086130) sets 50 / 130 / 200, action_main l_HIO.mPoseSpeedBase; mPoseSpeed is eased to it */
    /* 0x144B4 */ cXyz mHimoTopPos[8];  /* was mC33C: part_mtx_set 0207DE64: MtxPosition of each string's attach point; action_s_himo 020859AC starts string [i] from it */
    /* 0x14514 */ mDoExt_3DlineMat1_bgn mBlueRopeMat;
    /* 0x1469C */ mDoExt_3DlineMat1_bgn mRedRopeMat;
    /* 0x14824 */ mDoExt_3DlineMat1_bgn mDefeatCSRopeMat;
    /* 0x149AC */ cXyz mLastHimoPos[60];  /* was mC450: last_himo_control (in daBgn_Execute 0207E7B4): the 60 points of the last string ([0] fixed at (-15, 4441.46, 375.17)); drawn while mLastHimoState is set */
    /* 0x14C7C */ be<s8> mLastHimoState;  /* was mC720: daBgn_Execute 0207E7B4 sets 1 at the final explosion; last_himo_control turns 1 into 2 (start); daBgn_Draw 0207B2CC draws the last string while it is set */
    /* 0x14C7D */ u8 _14C7D[3];
    /* 0x14C80 */ be<f32> mLastHimoSway;  /* was mC724: last_himo_control (in daBgn_Execute 0207E7B4): set 5000 (+REG) on start, decays (addCalc0); sin / cos amplitude of the string points */
    /* 0x14C84 */ mDoExt_J3DModelPacketS_l mHdPacket[3]; /* HD */
    /* 0x14E94 */ cXyz mBodyOffset;  /* was mC728: tail_attack 0208459C / damage 02084804 / head_recover 02084B90 / dance_0: height offset of the body (y eased, + speed.y, clamped); move_se_set 0207D04C sounds by its change */
    /* 0x14EA0 */ cXyz mBodyOffsetOld;  /* was mC734: move 0208857C copies mBodyOffset; move_se_set 0207D04C: mBodyOffset - it (last frame's offset) */
    /* 0x14EAC */ u8 _14EAC[4];
    /* 0x14EB0 */ be<s16> mHeadCutYaw;  /* was mC744: move 0208857C eases it (step 0x400) to +8000 while head string mMove[0] is cut, -8000 while mMove[1] is cut (0 for both or neither); part_mtx_set 0207DE64 adds it to the head part's y rotation (with l_HIO.mHeadRotY); the head hangs to the side of the cut string (tww the same) */
    /* 0x14EB2 */ be<s16> mFrameCounter;  /* was mC746: daBgn_Execute 0207E7B4 adds 1 per frame; sin / cos phase of the dance, the body shake, the ripples and the camera bank */
    /* 0x14EB4 */ be<s16> mAction;  /* was mC748: action_main 02086130: switch (it) of the action (0 .. 7, 10); move 0208857C sets 5 on a core hit, daBgn_Create 02080BA4 7, dance_0 1 .. 4 */
    /* 0x14EB6 */ be<s16> mMode;  /* was mC74A: tail_attack 0208459C / hensin 02084D6C / dance_0: switch (it) inside each action, +1 per step; damage 02084804 sets 2 */
    /* 0x14EB8 */ be<s16> mDanceType;  /* was mC74C: dance_0 (in action_main 02086130) sets 0 / 1 and runs dance_A (0) or dance_B (1); start 020844F4 clears it */
    /* 0x14EBA */ be<s16> mDanceStep;  /* was mC74E: dance_A (in action_main 02086130): switch (it) of the dance_pause_1 / dance_pause_2 poses, +1 per step */
    /* 0x14EBC */ be<s16> mDanceAngleY;  /* was mC750: dance_A adds mDanceAngleSpeed, dance_B eases it to +-0x1555 (in action_main 02086130); move 0208857C: mBodyAngle.y = mBaseAngleY + it; hensin 02084D6C clears it */
    /* 0x14EBE */ be<s16> mDanceAngleSpeed;  /* was mC752: dance_A (in action_main 02086130): cleared on start, added to mDanceAngleY per frame, eased toward the turn target */
    /* 0x14EC0 */ be<s16> mDanceCount;  /* was mC754: start 020844F4 sets 1; dance_0 (in action_main 02086130) adds 1 per dance, wraps after 5, and picks the attack by it */
    /* 0x14EC2 */ u8 _14EC2[2];
    /* 0x14EC4 */ cXyz mTargetPos;  /* was mC758: dance_0 (in action_main 02086130) sets a random x / z (cM_rndFX(1500)), tail_attack 0208459C the player position; current.pos is eased toward it by mMoveSpeed */
    /* 0x14ED0 */ be<s16> mC764;
    /* 0x14ED2 */ u8 _14ED2[6];
    /* 0x14ED8 */ be<f32> mMoveSpeed;  /* was mC76C: tail_attack 0208459C / dance_0 (in action_main 02086130): step of cLib_addCalc2 of current.pos toward mTargetPos, eased up from 0 */
    /* 0x14EDC */ be<s16> mShakeTimer;  /* was mC770: damage 02084804 sets 0x32, hensin 02084D6C 0x14 (+REG); move 0208857C counts it down and shakes the chest (X / Z rotation) by it * 300 */
    /* 0x14EDE */ u8 _14EDE[2];
    /* 0x14EE0 */ be<f32> mRedRopeSway;  /* was mC774: action_main 02086130 swings the 60 points of mRedRopeMat by sin * it * (59 - i) / 59 and eases it to 0; damage 02084804, hensin 02084D6C and body_attack (in action_main) ease it to 250 (tww the same) */
    /* 0x14EE4 */ be<s8> mLeftPunchOn;  /* was mC778: punch_LR (in action_main 02086130) sets 1 while the left punch flies, 0 after; move 0208857C moves the left arm (i == 0) to mLeftPunchPos while it is set */
    /* 0x14EE5 */ be<s8> mRightPunchOn;  /* was mC779: punch_LR (in action_main 02086130) sets 1 while the right punch flies, 0 after; move 0208857C moves the right arm (i == 1) to mRightPunchPos while it is set */
    /* 0x14EE6 */ u8 _14EE6[2];
    /* 0x14EE8 */ cXyz mLeftPunchPos;  /* was mC77C: punch_LR (in action_main 02086130): copied from the left hand part (mPartPos), + mLeftPunchSpeed per frame; daBgn_Delete 0207FC64 deletes its sound object */
    /* 0x14EF4 */ cXyz mRightPunchPos;  /* was mC788: punch_LR (in action_main 02086130): copied from the right hand part (mPartPos), + mRightPunchSpeed per frame; daBgn_Delete 0207FC64 deletes its sound object */
    /* 0x14F00 */ cXyz mLeftPunchSpeed;  /* was mC794: punch_LR (in action_main 02086130): bgn_punch_aim velocity, added to mLeftPunchPos */
    /* 0x14F0C */ cXyz mRightPunchSpeed;  /* was mC7A0: punch_LR (in action_main 02086130): bgn_punch_aim velocity, added to mRightPunchPos */
    /* 0x14F18 */ be<s16> mTimers[5];  /* was mC7AC: daBgn_Execute 0207E7B4 counts all 5 down each frame; the actions load them (tail_attack 0208459C [0] = 100, dance_A [3] = l_HIO.mDanceAStepTime) */
    /* 0x14F22 */ be<s16> mDamageTimer;  /* was mC7B6: move 0208857C (damage_check) sets 100 on a core hit and takes no core hit while it is set; daBgn_Execute 0207E7B4 counts it down */
    /* 0x14F24 */ be<s16> mPartHitSeTimer;  /* was mC7B8: move 0208857C (damage_check) sets 10 when a part sphere is hit (def_se_set), no new part hit sound while it is set; daBgn_Execute 0207E7B4 counts it down */
    /* 0x14F26 */ u8 _14F26[2];
    /* 0x14F28 */ be<f32> mGroundY;  /* was mC7BC: part_control_0 0207D858 / part_mtx_set 0207DE64 keep the part positions at or above it; punch_LR (in action_main 02086130) tests the hand y against it + 30 */
    /* 0x14F2C */ dCcD_Stts mStts;
    /* 0x14F68 */ dCcD_Sph mBodyAtSph;  /* was mC7FC: daBgn_Create 02080BA4: Set(cc_sph_src); body_attack (in action_main 02086130) turns its At on (OnAtSetBit) */
    /* 0x15094 */ dCcD_Sph mCoreSph;
    /* 0x151C0 */ cXyz mCorePos;  /* was mCA54: part_mtx_set 0207DE64: centre of mCoreSph (the weak point); move 0208857C (damage_check) sets the weak point hit / break particles there; daBgn_Delete 0207FC64 deletes its sound object */
    /* 0x151CC */ be<s16> mBlurRate;  /* was mCA60: daBgn_Draw 0207B2CC: mDoGph_setBlureRate(it) while > 1, off and cleared at 1; bgn_demo_end (in daBgn_Execute 0207E7B4) sets 1, hensin 02084D6C eases it to 0xB4 */
    /* 0x151CE */ be<s8> mCSMode;
    /* 0x151CF */ u8 _151CF[1];
    /* 0x151D0 */ be<s16> mKSubCount;
    /* 0x151D2 */ u8 _151D2[2];
    /* 0x151D4 */ cXyz mCSCamEye;
    /* 0x151E0 */ cXyz mCSCamEyeOffset;  /* was mCA74: daBgn_Execute 0207E7B4 (demo camera): set to (200, 2100, 2500), eased; rotated by the bgn2 shape angle into mCSCamEye */
    /* 0x151EC */ cXyz mCSCamCenter;
    /* 0x151F8 */ u8 _151F8[0xC];
    /* 0x15204 */ be<f32> mCalcSpeed;  /* was mCA98: head_recover 02084B90 / the demo camera (in daBgn_Execute 0207E7B4) scale their cLib_addCalc2 steps by it and ease it up; dance_0 (in action_main 02086130) clears it */
    /* 0x15208 */ be<f32> mCSCamShake;  /* was mCA9C: hensin 02084D6C sets 20 / 15 / 10, daBgn_Execute 0207E7B4 30 (+REG): sin / cos amplitude of the demo camera eye / centre and bank */
    /* 0x1520C */ be<f32> mCSFovY;
    /* 0x15210 */ gptr<J3DModel> mpWater0Model;
    /* 0x15214 */ gptr<J3DModel> mpWater1Model;
    /* 0x15218 */ dKy_tevstr_l mWaterTevStr;
    /* 0x153E0 */ gptr<J3DModel> mpRoomReflectionModel;
    /* 0x153E4 */ mDoExt_J3DModelPacketS_l mRoomReflectionPacket;  /* was mCB60: daBgn_Draw 0207B2CC: model packet of mpRoomReflectionModel (setModel + update); useHeapInit 0207FE14 sets it up */
    /* 0x15494 */ dKy_tevstr_l mRoomTevStr;
    /* 0x1565C */ mDoExt_J3DModelPacketS_l mObjPacket;  /* was mCC24: ten_a_d_sub 0207B040: model packet of the searched object's model (setModel + update); useHeapInit 0207FE14 sets it up */
    /* 0x1570C */ be<s8> mCC38;
    /* 0x1570D */ u8 _1570D[3];
    /* 0x15710 */ u8 mPunchSmokeCb[2][0x20];      /* dPa_smokeEcallBack (HD 0x20) */
    /* 0x15750 */ be<s8> mKeeseSpawnNum;
    /* 0x15751 */ u8 _15751[3];
    /* 0x15754 */ be<f32> mPartLengthScale;  /* was mCC80: action_main 02086130 sets 1, hensin 02084D6C eases it to 0; part_control_0 0207D858 / action_s_himo 020859AC scale the part and string lengths by it */
    /* 0x15758 */ be<f32> mChestSquash;  /* was mCC84: hensin 02084D6C eases it to 0.22; move 0208857C scales the chest by (1 + it, 1 - it, 1 + it) while it is > 0 */
    /* 0x1575C */ be<f32> mFogFade;  /* was mCC88: hensin 02084D6C eases it to 1; part_draw 0207ACD0 / daBgn_Draw 0207B2CC move the fog start by -50000 * it */
    /* 0x15760 */ gptr<JPABaseEmitter> mpChestEmitter;  /* was mCC8C: hensin 02084D6C: particle_set (TWW: ID_AK_SN_KGTT2CHESTPOTA00) on the chest joint, made invalid when done */
    /* 0x15764 */ be<s8> mFormRequest;  /* was mCC90: daBgn_Execute 0207E7B4: 1 switches to the bgn3 form (mForm = 2), 2 starts the next stage (GanonK); the demo camera / bgn2 set it */
    /* 0x15765 */ be<u8> mHioOwner;  /* was mCC91: daBgn_Create 02080BA4 sets 1 when it created the l_HIO child; daBgn_Delete 0207FC64 deletes the HIO child only then */
    /* 0x15766 */ u8 _15766[2];
};
WWHD_OFFSET(bgn_class, mPhase, 0x3CC);
WWHD_OFFSET(bgn_class, mRipplePos, 0x4C4);
WWHD_OFFSET(bgn_class, mHeadParts, 0x4D0);
WWHD_OFFSET(bgn_class, mPelvisParts, 0xCB0);
WWHD_OFFSET(bgn_class, mLeftArmParts, 0x1490);
WWHD_OFFSET(bgn_class, mRightArmParts, 0x6740);
WWHD_OFFSET(bgn_class, mLeftLegParts, 0xB9F0);
WWHD_OFFSET(bgn_class, mRightLegParts, 0xC9B0);
WWHD_OFFSET(bgn_class, mTailParts, 0xD970);
WWHD_OFFSET(bgn_class, mMove, 0x12C20);
WWHD_OFFSET(bgn_class, mBodyPos, 0x14480);
WWHD_OFFSET(bgn_class, mBlueRopeMat, 0x14514);
WWHD_OFFSET(bgn_class, mLastHimoPos, 0x149AC);
WWHD_OFFSET(bgn_class, mHdPacket, 0x14C84);
WWHD_OFFSET(bgn_class, mBodyOffset, 0x14E94);
WWHD_OFFSET(bgn_class, mLeftPunchPos, 0x14EE8);
WWHD_OFFSET(bgn_class, mStts, 0x14F2C);
WWHD_OFFSET(bgn_class, mCoreSph, 0x15094);
WWHD_OFFSET(bgn_class, mCorePos, 0x151C0);
WWHD_OFFSET(bgn_class, mBlurRate, 0x151CC);
WWHD_OFFSET(bgn_class, mWaterTevStr, 0x15218);
WWHD_OFFSET(bgn_class, mRoomReflectionPacket, 0x153E4);
WWHD_OFFSET(bgn_class, mObjPacket, 0x1565C);
WWHD_OFFSET(bgn_class, mPunchSmokeCb, 0x15710);
WWHD_OFFSET(bgn_class, mPartLengthScale, 0x15754);
WWHD_OFFSET(bgn_class, mHioOwner, 0x15765);
WWHD_SIZE(bgn_class, 0x15768);

/* daBgn_HIO_c (HD: GameCube offsets - 4, the vtable pointer is the last word 0x178); size 0x17C */
struct daBgn_HIO_c {
    /* 0x000 */ be<s16> mChestRotX;  /* was m004: move 0208857C (shape_calc) rotates the chest model about x by it after the body rotation and scale; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x002 */ be<s8> mNo;
    /* 0x003 */ u8 _003;
    /* 0x004 */ be<f32> mPartDrawDist;  /* was m008: part_draw 0207ACD0 draws a part's model only while its camera distance is above it * mPartScale; daBgn_HIO_c_ct 02083558 sets 200 (tww the same) */
    /* 0x008 */ be<u8> mReflectionOn;  /* was m00C: part_draw 0207ACD0 / daBgn_Draw 0207B2CC update the model packets (mPartPacket, mChestPacket) and run obj_disp only while set; daBgn_HIO_c_ct 02083558 sets 1 (tww the same, also for bgn2 / bgn3) */
    /* 0x009 */ be<u8> mRoomReflectionOn;  /* was m00D: daBgn_Draw 0207B2CC runs room_disp (mpRoomReflectionModel into mRoomReflectionPacket) only while set; water0_disp (in daBgn_Draw) keeps mWaterAlphaPct then instead of the bgn2-height value; daBgn_HIO_c_ct 02083558 sets 1 (tww the same) */
    /* 0x00A */ u8 _00A[2];
    /* 0x00C */ be<f32> mWaterAlphaPct;  /* was m010: water0_disp (in daBgn_Draw 0207B2CC): the water konst alpha is it * 2.559 (percent; 50..90 from the bgn2 height in form 1 while mRoomReflectionOn is 0); daBgn_HIO_c_ct 02083558 sets 50 (tww the same) */
    /* 0x010 */ be<s16> m014;
    /* 0x012 */ be<s16> m016;
    /* 0x014 */ be<s16> m018;
    /* 0x016 */ u8 _016[2];
    /* 0x018 */ be<f32> m01C;
    /* 0x01C */ be<f32> m020;
    /* 0x020 */ be<u8> m024;
    /* 0x021 */ be<u8> mPoseEditMode;  /* was m025: while set action_main 02086130 runs no action, action_s 02085630 clears mCut and loads every string's mPoseTarget from mPoseEdit*, and move 0208857C takes mPoseEditAngleY as the body yaw; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x022 */ be<u8> m026;
    /* 0x023 */ be<u8> mDebugHealth;  /* was m027: daBgn_Execute 0207E7B4 sets health to it while it is not 0; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x024 */ be<u8> mAttackOn;  /* was m028: dance_0 (in action_main 02086130) leaves the dance for an attack (mTimers[0] 0, no keese left) only while set; daBgn_HIO_c_ct 02083558 sets 1 (tww the same) */
    /* 0x025 */ be<u8> mForceCutString0;  /* was m029: move 0208857C sets mMove[0].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x026 */ be<u8> mForceCutString1;  /* was m02A: move 0208857C sets mMove[1].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x027 */ be<u8> mForceCutString2;  /* was m02B: move 0208857C sets mMove[2].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x028 */ be<u8> mForceCutString3;  /* was m02C: move 0208857C sets mMove[3].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x029 */ be<u8> mForceCutString4;  /* was m02D: move 0208857C sets mMove[4].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x02A */ be<u8> mForceCutString5;  /* was m02E: move 0208857C sets mMove[5].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x02B */ be<u8> mForceCutString6;  /* was m02F: move 0208857C sets mMove[6].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x02C */ be<u8> mForceCutString7;  /* was m030: move 0208857C sets mMove[7].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x02D */ u8 _02D[3];
    /* 0x030 */ be<f32> mHeadOffsetY;  /* was m034: part_mtx_set 0207DE64 (head part) translates by (0, it, mHeadOffsetZ) before the mHeadRotX rotation; daBgn_HIO_c_ct 02083558 sets -20 (tww the same) */
    /* 0x034 */ be<f32> mHeadOffsetZ;  /* was m038: part_mtx_set 0207DE64 (head part) translates by (0, mHeadOffsetY, it) before the mHeadRotX rotation; daBgn_HIO_c_ct 02083558 sets -30 (tww the same) */
    /* 0x038 */ be<s16> mHeadRotY;  /* was m03C: part_mtx_set 0207DE64 (head part) rotates about y by mHeadCutYaw + it + the pluck shake; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x03A */ be<s16> mHeadRotX;  /* was m03E: part_mtx_set 0207DE64 (head part) rotates about x by it after the mHeadOffsetY / Z translation; daBgn_HIO_c_ct 02083558 sets 22000 (tww the same) */
    /* 0x03C */ be<s16> mHeadRotZ;  /* was m040: part_mtx_set 0207DE64 (head part) rotates about z by it + the pluck shake; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x03E */ be<s16> mPoseEditAngleY;  /* was m042: move 0208857C takes it as the body yaw target (instead of shape_angle.y) while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x040 */ be<s16> m044;
    /* 0x042 */ u8 _042[2];
    /* 0x044 */ be<f32> m048;
    /* 0x048 */ be<f32> mBodyHeight;  /* was m04C: move 0208857C adds it to mBodyPos.y (current.pos + mBodyOffset; at least mGroundY + 100); daBgn_HIO_c_ct 02083558 sets 950 (tww the same) */
    /* 0x04C */ be<f32> m050;
    /* 0x050 */ be<f32> mHeadEndPosX;  /* was m054: x of the cXyz move 0208857C passes to bgn_limb_end for the head string: rotated by the body yaw, + mMove[0].mPose, + mBodyPos gives mHeadParts[1].mPartPos; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x054 */ be<f32> mHeadEndPosY;  /* was m058: y of the head-string end cXyz at mHeadEndPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets 2000 (tww the same) */
    /* 0x058 */ be<f32> mHeadEndPosZ;  /* was m05C: z of the head-string end cXyz at mHeadEndPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets 800 (tww the same) */
    /* 0x05C */ be<f32> mPelvisEndPosX;  /* was m060: x of the cXyz move 0208857C passes to bgn_limb_end for the pelvis string (mMove[2]): gives mPelvisParts[1].mPartPos; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x060 */ be<f32> mPelvisEndPosY;  /* was m064: y of the pelvis-string end cXyz at mPelvisEndPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets -800 (tww the same) */
    /* 0x064 */ be<f32> mPelvisEndPosZ;  /* was m068: z of the pelvis-string end cXyz at mPelvisEndPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets -800 (tww the same) */
    /* 0x068 */ be<f32> mLeftHandPosX;  /* was m06C: move 0208857C: x of the left hand rest position (rotated by the body yaw, + mMove[3].mPose, + mBodyPos) for mLeftArmParts[BGN_HAND_MAX] while not punching; daBgn_HIO_c_ct 02083558 sets 700 (tww the same) */
    /* 0x06C */ be<f32> mLeftHandPosY;  /* was m070: move 0208857C: y of the left hand rest position (see mLeftHandPosX); daBgn_HIO_c_ct 02083558 sets -900 (tww the same) */
    /* 0x070 */ be<f32> mLeftHandPosZ;  /* was m074: move 0208857C: z of the left hand rest position (see mLeftHandPosX); daBgn_HIO_c_ct 02083558 sets 300 (tww the same) */
    /* 0x074 */ be<f32> mRightHandPosX;  /* was m078: move 0208857C: x of the right hand rest position (rotated by the body yaw, + mMove[4].mPose, + mBodyPos) for mRightArmParts[BGN_HAND_MAX] while not punching; daBgn_HIO_c_ct 02083558 sets -700 (tww the same) */
    /* 0x078 */ be<f32> mRightHandPosY;  /* was m07C: move 0208857C: y of the right hand rest position (see mRightHandPosX); daBgn_HIO_c_ct 02083558 sets -900 (tww the same) */
    /* 0x07C */ be<f32> mRightHandPosZ;  /* was m080: move 0208857C: z of the right hand rest position (see mRightHandPosX); daBgn_HIO_c_ct 02083558 sets 300 (tww the same) */
    /* 0x080 */ be<f32> mLeftFootPosX;  /* was m084: x of the cXyz move 0208857C passes to bgn_limb_end for the left leg string (mMove[5]): gives mLeftLegParts[3].mPartPos; daBgn_HIO_c_ct 02083558 sets 500 (tww the same) */
    /* 0x084 */ be<f32> mLeftFootPosY;  /* was m088: y of the left-foot cXyz at mLeftFootPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets -1300 (tww the same) */
    /* 0x088 */ be<f32> mLeftFootPosZ;  /* was m08C: z of the left-foot cXyz at mLeftFootPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets -100 (tww the same) */
    /* 0x08C */ be<f32> mRightFootPosX;  /* was m090: x of the cXyz move 0208857C passes to bgn_limb_end for the right leg string (mMove[6]): gives mRightLegParts[3].mPartPos; daBgn_HIO_c_ct 02083558 sets -500 (tww the same) */
    /* 0x090 */ be<f32> mRightFootPosY;  /* was m094: y of the right-foot cXyz at mRightFootPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets -1300 (tww the same) */
    /* 0x094 */ be<f32> mRightFootPosZ;  /* was m098: z of the right-foot cXyz at mRightFootPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets -100 (tww the same) */
    /* 0x098 */ be<f32> mTailEndPosX;  /* was m09C: move 0208857C: x of the tail end position (rotated by the body yaw, + mMove[7].mPose, + mBodyPos, not below mGroundY) for mTailParts[BGN_TAIL_MAX]; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x09C */ be<f32> mTailEndPosY;  /* was m0A0: move 0208857C: y of the tail end position (see mTailEndPosX); daBgn_HIO_c_ct 02083558 sets -700 (tww the same) */
    /* 0x0A0 */ be<f32> mTailEndPosZ;  /* was m0A4: move 0208857C: z of the tail end position (see mTailEndPosX); daBgn_HIO_c_ct 02083558 sets -1200 (tww the same) */
    /* 0x0A4 */ be<s16> mBodyDrawOffsetY;  /* was m0A8: move 0208857C (shape_calc) starts the body matrix (chest and all part roots) at mBodyPos.y + it (at least mGroundY + 100); daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0A6 */ be<s16> mPoseEdit0X;  /* was m0AA: action_s 02085630 copies it to mMove[0].mPoseTarget.x while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0A8 */ be<s16> mPoseEdit0Y;  /* was m0AC: action_s 02085630 copies it to mMove[0].mPoseTarget.y while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0AA */ be<s16> mPoseEdit0Z;  /* was m0AE: action_s 02085630 copies it to mMove[0].mPoseTarget.z while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0AC */ be<s16> mPoseEdit2X;  /* was m0B0: action_s 02085630 copies it to mMove[2].mPoseTarget.x while mPoseEditMode is set (string 1 has no entry); daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0AE */ be<s16> mPoseEdit2Y;  /* was m0B2: action_s 02085630 copies it to mMove[2].mPoseTarget.y while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0B0 */ be<s16> mPoseEdit2Z;  /* was m0B4: action_s 02085630 copies it to mMove[2].mPoseTarget.z while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0B2 */ be<s16> mPoseEdit3X;  /* was m0B6: action_s 02085630 copies it to mMove[3].mPoseTarget.x while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0B4 */ be<s16> mPoseEdit3Y;  /* was m0B8: action_s 02085630 copies it to mMove[3].mPoseTarget.y while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0B6 */ be<s16> mPoseEdit3Z;  /* was m0BA: action_s 02085630 copies it to mMove[3].mPoseTarget.z while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0B8 */ be<s16> mPoseEdit4X;  /* was m0BC: action_s 02085630 copies it to mMove[4].mPoseTarget.x while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0BA */ be<s16> mPoseEdit4Y;  /* was m0BE: action_s 02085630 copies it to mMove[4].mPoseTarget.y while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0BC */ be<s16> mPoseEdit4Z;  /* was m0C0: action_s 02085630 copies it to mMove[4].mPoseTarget.z while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0BE */ be<s16> mPoseEdit5X;  /* was m0C2: action_s 02085630 copies it to mMove[5].mPoseTarget.x while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0C0 */ be<s16> mPoseEdit5Y;  /* was m0C4: action_s 02085630 copies it to mMove[5].mPoseTarget.y while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0C2 */ be<s16> mPoseEdit5Z;  /* was m0C6: action_s 02085630 copies it to mMove[5].mPoseTarget.z while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0C4 */ be<s16> mPoseEdit6X;  /* was m0C8: action_s 02085630 copies it to mMove[6].mPoseTarget.x while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0C6 */ be<s16> mPoseEdit6Y;  /* was m0CA: action_s 02085630 copies it to mMove[6].mPoseTarget.y while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0C8 */ be<s16> mPoseEdit6Z;  /* was m0CC: action_s 02085630 copies it to mMove[6].mPoseTarget.z while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0CA */ be<s16> mPoseEdit7X;  /* was m0CE: action_s 02085630 copies it to mMove[7].mPoseTarget.x while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0CC */ be<s16> mPoseEdit7Y;  /* was m0D0: action_s 02085630 copies it to mMove[7].mPoseTarget.y while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0CE */ be<s16> mPoseEdit7Z;  /* was m0D2: action_s 02085630 copies it to mMove[7].mPoseTarget.z while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x0D0 */ be<f32> mPoseSpeedBase;  /* was m0D4: action_main 02086130 resets mPoseSpeedTarget to it every frame (punch_LR raises it for its frame); daBgn_HIO_c_ct 02083558 sets 40 (tww the same) */
    /* 0x0D4 */ be<s16> mKeeseDanceTime;  /* was m0D8: dance_0 (in action_main 02086130) sets mTimers[0] to it for the first dance (mDanceCount 0, the one that calls the keese); daBgn_HIO_c_ct 02083558 sets 120 (tww the same) */
    /* 0x0D6 */ be<s16> mDanceAStepTime;  /* was m0DA: dance_A (in action_main 02086130) sets mTimers[3] to it at each switch between dance_pause_1 and dance_pause_2; daBgn_HIO_c_ct 02083558 sets 20 (tww the same) */
    /* 0x0D8 */ be<s16> mDanceASpin3HP;  /* was m0DC: dance_A (in action_main 02086130) eases mDanceAngleSpeed to it at 3 health; daBgn_HIO_c_ct 02083558 sets 1200 (tww the same) */
    /* 0x0DA */ be<s16> mDanceASpin2HP;  /* was m0DE: dance_A (in action_main 02086130) eases mDanceAngleSpeed to it at 2 health; daBgn_HIO_c_ct 02083558 sets 1500 (tww the same) */
    /* 0x0DC */ be<s16> mDanceASpin1HP;  /* was m0E0: dance_A (in action_main 02086130) eases mDanceAngleSpeed to it at 1 health; daBgn_HIO_c_ct 02083558 sets 2000 (tww the same) */
    /* 0x0DE */ be<s16> mDanceTime3HP;  /* was m0E2: dance_0 (in action_main 02086130) sets mTimers[0] to it for the later dances (mDanceCount not 0) at 3 health; daBgn_HIO_c_ct 02083558 sets 300 (tww DEMO_SELECT(150, 300)) */
    /* 0x0E0 */ be<s16> mDanceTime2HP;  /* was m0E4: dance_0 (in action_main 02086130) sets mTimers[0] to it for the later dances at 2 health; daBgn_HIO_c_ct 02083558 sets 200 (tww DEMO_SELECT(100, 200)) */
    /* 0x0E2 */ be<s16> mDanceTime1HP;  /* was m0E6: dance_0 (in action_main 02086130) sets mTimers[0] to it for the later dances at 1 health; daBgn_HIO_c_ct 02083558 sets 150 (tww DEMO_SELECT(80, 150)) */
    /* 0x0E4 */ be<s16> mDanceBStepTime3HP;  /* was m0E8: dance_B (in action_main 02086130) sets mTimers[3] to it at each switch between dance_pause_3 and dance_pause_4 at 3 health; daBgn_HIO_c_ct 02083558 sets 15 (tww DEMO_SELECT(20, 15)) */
    /* 0x0E6 */ be<s16> mDanceBStepTime2HP;  /* was m0EA: dance_B (in action_main 02086130) sets mTimers[3] to it at each pose switch at 2 health; daBgn_HIO_c_ct 02083558 sets 15 (tww the same) */
    /* 0x0E8 */ be<s16> mDanceBStepTime1HP;  /* was m0EC: dance_B (in action_main 02086130) sets mTimers[3] to it at each pose switch at 1 health; daBgn_HIO_c_ct 02083558 sets 15 (tww DEMO_SELECT(10, 15)) */
    /* 0x0EA */ be<s16> mDanceBTurnStep;  /* was m0EE: dance_B (in action_main 02086130): the max step of mDanceAngleY toward +-0x1555; daBgn_HIO_c_ct 02083558 sets 500 (tww the same) */
    /* 0x0EC */ be<s16> mHandMax;  /* was m0F0: daBgn_Create 02080BA4 and move 0208857C copy it to BGN_HAND_MAX (the arm part count); daBgn_HIO_c_ct 02083558 sets 5 (tww the same) */
    /* 0x0EE */ be<s16> mTailMax;  /* was m0F2: daBgn_Create 02080BA4 and move 0208857C copy it to BGN_TAIL_MAX (the tail part count); daBgn_HIO_c_ct 02083558 sets 10 (tww the same) */
    /* 0x0F0 */ be<f32> mScale;  /* was m0F4: part_mtx_set 0207DE64 scales every part by mPartScale * it; move 0208857C scales the chest (mChestScale), pelvis (mPelvisScale) and the part spacings by it; daBgn_HIO_c_ct 02083558 sets 1.3 (tww the same) */
    /* 0x0F4 */ be<f32> mHeadScale;  /* was m0F8: move 0208857C (size_set) sets mHeadParts[0].mPartScale to it; daBgn_HIO_c_ct 02083558 sets 1.6 (tww the same) */
    /* 0x0F8 */ be<f32> mChestScale;  /* was m0FC: move 0208857C (shape_calc) scales the chest matrix and mBodyAtSph by it * mScale; daBgn_HIO_c_ct 02083558 sets 3.25 (tww the same) */
    /* 0x0FC */ be<f32> mPelvisScale;  /* was m100: move 0208857C (size_set) sets mPelvisParts[0] / [1].mPartScale to it and scales the leg / tail root matrix by it * mScale; daBgn_HIO_c_ct 02083558 sets 1.8 (tww the same) */
    /* 0x100 */ be<f32> mArmRootScale;  /* was m104: move 0208857C (size_set): the arm parts' mPartScale runs from it at the shoulder down to mArmTipScale; daBgn_HIO_c_ct 02083558 sets 1.2 (tww the same) */
    /* 0x104 */ be<f32> mArmTipScale;  /* was m108: move 0208857C (size_set): mPartScale of the hand part (mLeft / mRightArmParts[BGN_HAND_MAX]), the end of the mArmRootScale run; daBgn_HIO_c_ct 02083558 sets 0.3 (tww the same) */
    /* 0x108 */ be<f32> mLegRootScale;  /* was m10C: move 0208857C (size_set): the leg parts' mPartScale runs from it at the hip down to mLegTipScale; daBgn_HIO_c_ct 02083558 sets 1.2 (tww the same) */
    /* 0x10C */ be<f32> mLegTipScale;  /* was m110: move 0208857C (size_set): mPartScale of the foot part (mLeft / mRightLegParts[3]); daBgn_HIO_c_ct 02083558 sets 0.4 (tww the same) */
    /* 0x110 */ be<f32> mTailRootScale;  /* was m114: move 0208857C (size_set): the tail parts' mPartScale runs from it down to mTailTipScale (the last part 0.7 * it); daBgn_HIO_c_ct 02083558 sets 1.2 (tww the same) */
    /* 0x114 */ be<f32> mTailTipScale;  /* was m118: move 0208857C (size_set): the end of the mTailRootScale run; daBgn_HIO_c_ct 02083558 sets 0.1 (tww the same) */
    /* 0x118 */ be<f32> mBodyPartSpacing;  /* was m11C: move 0208857C passes it * mScale to part_control_0 0207D858 / part_control_2 0207DC84 as the part spacing of the head and pelvis chains; daBgn_HIO_c_ct 02083558 sets 185 (tww the same) */
    /* 0x11C */ be<f32> mLegPartSpacing;  /* was m120: move 0208857C passes it * mScale to part_control_0 / part_control_2 as the part spacing of both leg chains; daBgn_HIO_c_ct 02083558 sets 185 (tww the same) */
    /* 0x120 */ be<f32> mArmPartSpacing;  /* was m124: move 0208857C eases mArmLength (the arm chains' spacing) to it + the punch stretch; part_mtx_set 0207DE64 stretches the arm parts by mArmLength - it; daBgn_HIO_c_ct 02083558 sets 185 (tww the same) */
    /* 0x124 */ be<f32> mTailPartSpacing;  /* was m128: move 0208857C passes it * mScale to part_control_0 / part_control_2 as the part spacing of the tail chain; daBgn_HIO_c_ct 02083558 sets 185 (tww the same) */
    /* 0x128 */ be<f32> mHeadRootPosX;  /* was m12C: move 0208857C: x of the head root in the body matrix (MtxPosition into mHeadParts[0].mPartPos); daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x12C */ be<f32> mHeadRootPosY;  /* was m130: move 0208857C: y of the head root in the body matrix; daBgn_HIO_c_ct 02083558 sets -10 (tww the same) */
    /* 0x130 */ be<f32> mHeadRootPosZ;  /* was m134: move 0208857C: z of the head root in the body matrix; daBgn_HIO_c_ct 02083558 sets 120 (tww the same) */
    /* 0x134 */ be<f32> mShoulderPosX;  /* was m138: move 0208857C: x of the left shoulder in the body matrix (* mPartLengthScale; the right one takes -it) for mLeft / mRightArmParts[0]; daBgn_HIO_c_ct 02083558 sets 100 (tww the same) */
    /* 0x138 */ be<f32> mShoulderPosY;  /* was m13C: move 0208857C: y of both shoulders in the body matrix (* mPartLengthScale); daBgn_HIO_c_ct 02083558 sets 25 (tww the same) */
    /* 0x13C */ be<f32> mShoulderPosZ;  /* was m140: move 0208857C: z of both shoulders in the body matrix (* mPartLengthScale); daBgn_HIO_c_ct 02083558 sets 20 (tww the same) */
    /* 0x140 */ be<f32> mPelvisRootPosX;  /* was m144: move 0208857C: x of the pelvis root in the body matrix (* mPartLengthScale) for mPelvisParts[0]; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x144 */ be<f32> mPelvisRootPosY;  /* was m148: move 0208857C: y of the pelvis root in the body matrix ((it + 20) * mPartLengthScale); daBgn_HIO_c_ct 02083558 sets -115 (tww the same) */
    /* 0x148 */ be<f32> mPelvisRootPosZ;  /* was m14C: move 0208857C: z of the pelvis root in the body matrix (* mPartLengthScale); daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x14C */ be<f32> mHipPosX;  /* was m150: move 0208857C: x of the left hip in the pelvis matrix (* mPartLengthScale; the right one takes -it), + the pelvis position, for mLeft / mRightLegParts[0]; daBgn_HIO_c_ct 02083558 sets 80 (tww the same) */
    /* 0x150 */ be<f32> mHipPosY;  /* was m154: move 0208857C: y of both hips in the pelvis matrix (* mPartLengthScale); daBgn_HIO_c_ct 02083558 sets -20 (tww the same) */
    /* 0x154 */ be<f32> mHipPosZ;  /* was m158: move 0208857C: z of both hips in the pelvis matrix (* mPartLengthScale); daBgn_HIO_c_ct 02083558 sets 100 (tww the same) */
    /* 0x158 */ be<f32> mTailRootPosX;  /* was m15C: move 0208857C: x of the tail root in the pelvis matrix (* mPartLengthScale), + the pelvis position, for mTailParts[0]; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) */
    /* 0x15C */ be<f32> mTailRootPosY;  /* was m160: move 0208857C: y of the tail root in the pelvis matrix (* mPartLengthScale); daBgn_HIO_c_ct 02083558 sets -60 (tww the same) */
    /* 0x160 */ be<f32> mTailRootPosZ;  /* was m164: move 0208857C: z of the tail root in the pelvis matrix (* mPartLengthScale); daBgn_HIO_c_ct 02083558 sets 150 (tww the same) */
    /* 0x164 */ be<f32> mPartOffsetZ;  /* was m168: part_mtx_set 0207DE64 translates every part matrix by (0, 0, it) after the scale and places the string tops of the non-head parts there; daBgn_HIO_c_ct 02083558 sets 100 (tww the same) */
    /* 0x168 */ be<f32> mPartDropHung;  /* was m16C: part_control_0 0207D858 adds it to each part's y (not below the ground) while the part's string is intact; daBgn_HIO_c_ct 02083558 sets -50 (tww the same) */
    /* 0x16C */ be<f32> mPartDropCut;  /* was m170: part_control_0 0207D858 and part_control_0Z (in move 0208857C) add it to each part's y while the string is cut (-40 in a cut scene); daBgn_HIO_c_ct 02083558 sets -100 (tww the same) */
    /* 0x170 */ be<s16> mKeeseNum3HP;
    /* 0x172 */ be<s16> mKeeseNum2HP;
    /* 0x174 */ be<s16> mKeeseNum1HP;
    /* 0x176 */ be<s16> mKeeseNumMax;
    /* 0x178 */ be<u32> __vtbl;
};
WWHD_OFFSET(daBgn_HIO_c, mPoseSpeedBase, 0xD0);
WWHD_OFFSET(daBgn_HIO_c, mScale, 0xF0);
WWHD_OFFSET(daBgn_HIO_c, mKeeseNum3HP, 0x170);
WWHD_SIZE(daBgn_HIO_c, 0x17C);

/* ---- this TU's statics ---- */
inline daBgn_HIO_c& l_HIO() { return *gabi::at<daBgn_HIO_c>(0x10461CDC); }
inline be<u32>& bgn_g() { return *gabi::at<be<u32>>(0x10461A34); }       /* static bgn_class* bgn */
inline be<s32>& ki_all_count() { return *gabi::at<be<s32>>(0x10461A48); }
inline be<u8>& hio_set() { return *gabi::at<be<u8>>(0x10190F40); }
inline be<u32>& bgn2_g() { return *gabi::at<be<u32>>(0x10461A38); }      /* static bgn2_class* bgn2 */
inline be<u32>& bgn3_g() { return *gabi::at<be<u32>>(0x10461A3C); }      /* static bgn3_class* bgn3 */
inline be<s32>& BGN_HAND_MAX() { return *gabi::at<be<s32>>(0x10461A40); }
inline be<s32>& BGN_TAIL_MAX() { return *gabi::at<be<s32>>(0x10461A44); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* J3DDrawBuffer, J3DPacket: JSystem/J3DGraph.h */
/* j3dSys (HD 0x104B45C0): view matrix at +0x38 */
inline Mtx34* j3dSys_viewMtx() { return gabi::at<Mtx34>(0x104B45F8); }
/* 028E9108 PSMTXConcat(a, b, ab) */
inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
/* dComIfGd_getOpaListSky, dComIfGd_setListSky: bindings.h */
/* 027F0E04 J3DDrawBuffer::entryImm(J3DPacket*, u16) */
inline s32 J3DDrawBuffer_entryImm(J3DDrawBuffer* b, void* pkt, u16 idx) { return gabi::call<s32>(0x027F0E04, b, pkt, idx); }
/* dPa_smokeEcallBack::remove(): virtual (vtable at +0, slot +0x44) */
inline void smoke_remove(void* cb) { gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(cb)) + 0x44), cb); }

/* cM_rad2s 02019510 (cM_fsin inline = cM_ssin(cM_rad2s(x))) [as in d_a_bgn2] */
inline s16 bgn_cM_rad2s(f32 x) { return gabi::call<s16>(0x02019510, x); }
/* 025F18EC mDoMtx_XrotS */
inline void cMtx_XrotS(Mtx34* m, s16 x) { gabi::call(0x025F18EC, m, x); }
/* 028E8DAC PSVECSubtract(a, b, a-b) */
inline void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* out) { gabi::call(0x028E8DAC, a, b, out); }
/* 024EF0F4 dBgS::GetAttributeCode(cBgS_PolyInfo&) */
inline s32 dBgS_GetAttributeCode(dBgS* bgs, void* polyInfo) { return gabi::call<s32>(0x024EF0F4, bgs, polyInfo); }
/* f32 -> u32 (GHS: values not below 2^31, and NaN, are converted with the top bit added back) [as in d_a_bgn2] */
inline u32 f2u(f32 f) {
    if (f < 2147483648.0f)
        return (u32)gabi::ftoi(f);
    return (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000u;
}
/* dComIfGp_particle_setToon: dPa_control_c::set with group 2 */
inline JPABaseEmitter* dComIfGp_particle_setToon(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha,
                                                 void* cb, s8 setup) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, angle, scale, alpha, (dPa_levelEcallBack*)cb, setup, nullptr, nullptr, nullptr);
}
/* bg_tevstr (static dKy_tevstr_c, 0x10461E58): colour C0 (s16 r, g, b at +0x90), K0 (u8 at +0x98) */
inline u32 bg_tevstr() { return 0x10461E58; }
/* dBgS_LinChk on the stack (HD layout as in d_a_bgn2; this TU's vtables) */
struct dBgS_LinChk_bgn {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x58 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x64 */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ u8 _24[0x30 - 0x24];
    /* 0x30 */ cXyz mCross;
    /* 0x3C */ u8 _3C[0x58 - 0x3C];
    /* 0x58 */ be<u32> __vtbl_58;     /* dBgS_PolyPassChk */
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64;     /* dBgS_GrpPassChk */
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_LinChk_bgn, 0x6C);

/* dCcD_GObjInf::OnAtSetBit / OffAtSetBit (inline): bit 0 of the At SPrm word */
inline void OnAtSetBit(dCcD_GObjInf* o) { o->mObjAt.mSPrm |= 1u; }
inline void OffAtSetBit(dCcD_GObjInf* o) { o->mObjAt.mSPrm &= ~1u; }
/* HD J3DMaterial: material i of a model's data (+0xAC) is *(*(data+0x10) + 4i); its pixel-engine block
 * is self-relative at material+0x20 (0: none) */
inline u32 bgn_material0(J3DModel* model) {
    u32 md = gabi::load<u32>(gabi::ea(model) + 0xAC);
    return gabi::load<u32>(gabi::load<u32>(md + 0x10));
}
inline u32 bgn_material_pe(u32 mat) {
    s32 off = gabi::load<s32>(mat + 0x20);
    return off != 0 ? mat + 0x20 + off : 0;
}

/* 025CB374 dVibration_c::StartShock(int, int, cXyz) (cXyz by value) */
inline BOOL StartShock(dVibration_c* vib, s32 strength, s32 flags, cXyz* pos) { return gabi::call<BOOL>(0x025CB374, vib, strength, flags, pos); }
/* 025E18EC mDoAud_bgmStart(id) */
inline void mDoAud_bgmStart(u32 id) { gabi::call(0x025E18EC, id); }
/* HD J3D: a model's joint matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices);
 * getAnmMtx marks them dirty [as in d_a_bgn2] */
inline Mtx34* model_getAnmMtx(J3DModel* model, s32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30);
}
/* 028249B0 JPASetRMtxTVecfromMtx(mtx, rot, trans): JPABaseEmitter::setGlobalRTMatrix (HD inline) */
inline void JPABaseEmitter_setGlobalRTMatrix(JPABaseEmitter* e, Mtx34* m) {
    u32 b = gabi::ea(e);
    gabi::call(0x028249B0, m, gabi::at<u8>(b + 0x1F0), gabi::at<u8>(b + 0x22C));
}
/* JPABaseEmitter::becomeInvalidEmitter (HD inline): mMaxFrame (+0x5C) = -1, flags (+0x254) |= 1 */
inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* e) {
    u32 b = gabi::ea(e);
    u32 f = gabi::load<u32>(b + 0x254);
    gabi::store<s32>(b + 0x5C, -1);
    gabi::store<u32>(b + 0x254, f | 1);
}
/* JPABaseEmitter::setGlobalScale (HD inline): +0x220 and +0x238 [as in d_a_bgn2] */
inline void JPABaseEmitter_setGlobalScale(JPABaseEmitter* e, f32 x, f32 y, f32 z) {
    u32 b = gabi::ea(e);
    gabi::store<f32>(b + 0x220, x);
    gabi::store<f32>(b + 0x224, y);
    gabi::store<f32>(b + 0x228, z);
    gabi::store<f32>(b + 0x238, x);
    gabi::store<f32>(b + 0x23C, y);
    gabi::store<f32>(b + 0x240, z);
}
/* JPABaseEmitter::setGlobalTranslation (HD inline): +0x22C; y negated for version (+0x262) >= 7 */
inline void JPABaseEmitter_setGlobalTranslation(JPABaseEmitter* e, f32 x, f32 y, f32 z) {
    u32 b = gabi::ea(e);
    if (gabi::load<u8>(b + 0x262) >= 7)
        y = -y;
    gabi::store<f32>(b + 0x22C, x);
    gabi::store<f32>(b + 0x230, y);
    gabi::store<f32>(b + 0x234, z);
}

#define BGN_SAFESTRING_VTBL 0x10008A1C /* this TU's sead::SafeString vtable */
#define BGN_ARC STR(0x10008C4C)         /* "Bgn" (useHeapInit) */
/* mDoExt_brkAnm (HD 0x78): constructor 025E80D0, init 025E8154 [as in d_a_bgn2] */
inline mDoExt_brkAnm* mDoExt_brkAnm_ct(mDoExt_brkAnm* p) { return gabi::call<mDoExt_brkAnm*>(0x025E80D0, p); }
inline BOOL mDoExt_brkAnm_init(mDoExt_brkAnm* a, J3DModelData* d, void* k, s32 play, s32 mode, f32 speed, s16 start, s16 end,
                               s32 modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, k, play, mode, speed, start, end, modify, entry);
}
/* 025EBA58 mDoExt_3DlineMat1_c::init(u16 numLines, u16 numSegments, ResTIMG*, int) */
inline bool mDoExt_3DlineMat1_init(void* m, u16 lines, u16 segs, void* img, s32 p) { return gabi::call<bool>(0x025EBA58, m, lines, segs, img, p); }

/* __construct_array / __destroy_arr (GHS runtime) */
inline void __construct_array(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
inline void __destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 f7, u32 f8) { gabi::call(0x028F0164, p, n, size, dtor, f7, f8); }
/* mDoExt_3DlineMat1_c: constructor 025EB82C, destructor 025EB8B8 [as in d_a_bridge] */
inline void mDoExt_3DlineMat1_ct(void* l) { gabi::call(0x025EB82C, l); }
inline void mDoExt_3DlineMat1_dt(void* l, s32 flags) { gabi::call(0x025EB8B8, l, flags); }
/* dCcD_Sph: constructor 025166F0, destructor 02515AE8 */
inline void dCcD_Sph_ct(dCcD_Sph* s) { gabi::call(0x025166F0, s); }
inline void dCcD_Sph_dt(dCcD_Sph* s, s32 flags) { gabi::call(0x02515AE8, s, flags); }
/* dKy_tevstr_c inline constructor (HD): three 0x44-byte light blocks (+0x0, +0xC0, +0x144) start as
 * copies of the template at 0x1016E414 (floats +0x0..+0x14 and +0x24..+0x40, bytes +0x18..+0x1B,
 * halves +0x1C..+0x22) */
struct tevstr_tmpl {
    f32 f0[6];
    u8 b[4];
    s16 h[4];
    f32 f1[8];
    void load() {
        u32 s = 0x1016E414;
        for (int i = 0; i < 6; i++) f0[i] = gabi::load<f32>(s + 4 * i);
        for (int i = 0; i < 4; i++) b[i] = gabi::load<u8>(s + 0x18 + i);
        for (int i = 0; i < 4; i++) h[i] = gabi::load<s16>(s + 0x1C + 2 * i);
        for (int i = 0; i < 8; i++) f1[i] = gabi::load<f32>(s + 0x24 + 4 * i);
    }
    void store(u32 t) const {
        for (u32 k : {0x0u, 0xC0u, 0x144u}) {
            for (int i = 0; i < 6; i++) gabi::store<f32>(t + k + 4 * i, f0[i]);
            for (int i = 0; i < 4; i++) gabi::store<u8>(t + k + 0x18 + i, b[i]);
            for (int i = 0; i < 4; i++) gabi::store<s16>(t + k + 0x1C + 2 * i, h[i]);
            for (int i = 0; i < 8; i++) gabi::store<f32>(t + k + 0x24 + 4 * i, f1[i]);
        }
    }
};

/* ---- functions of the unit (natural calls between parts) ---- */
void bgn_mtx_copy(Mtx34* dst, Mtx34* src);
void bgn_colorS10_to_f(be<f32>* dst, be<s16>* src);
void bgn_color_to_f(be<f32>* dst, be<u8>* src);
void mDoExt_J3DModelPacketS_update(mDoExt_J3DModelPacketS_l* p);
void mDoExt_J3DModelPacketS_setup(mDoExt_J3DModelPacketS_l* p, u32 heap);
mDoExt_J3DModelPacketS_l* mDoExt_J3DModelPacketS_ct(mDoExt_J3DModelPacketS_l* p);
void part_draw(bgn_class* i_this, part_s* param_2);
s32 ki_check(bgn_class* i_this);
void SafeString_assureTermination(SafeString* s);
void mDoExt_J3DModelPacketS_setMaterial(mDoExt_J3DModelPacketS_l* p, u32 st);
void mDoExt_J3DModelPacketS_dt(mDoExt_J3DModelPacketS_l* p, s32 flags);
s32 part_init(part_s* param_1, J3DModelData* param_2);
s32 gr_check(bgn_class* i_this, cXyz* param_2);
void move_se_set(bgn_class* i_this);
void attack_eff_set(bgn_class* i_this, cXyz* param_2, int param_3);
void part_control_0(bgn_class* i_this, int param_2, part_s* param_3, move_s* param_4, f32 param_5);
void part_control_2(bgn_class* i_this, int param_2, part_s* param_3, f32 param_4);
void water1_disp(bgn_class* i_this);

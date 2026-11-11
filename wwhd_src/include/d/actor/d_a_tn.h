/**
 * d_a_tn.h (WWHD)
 * Enemy - Darknut: tn_class in the WWHD layout. 
 */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"
#include "c/c_damagereaction.h"
#include "d/d_path.h"
#include "gc_api/d/d_com_inf_game.h"

enum Action {
    ACTION_JYUNKAI = 0,
    ACTION_STAND = 1,
    ACTION_FIGHT_RUN = 4,
    ACTION_FIGHT = 5,
    ACTION_P_LOST = 8,
    ACTION_B_NIGE = 9,
    ACTION_DEFENCE = 10,
    ACTION_HUKKI = 11,
    ACTION_WEPON_SEARCH = 12,
    ACTION_AITE_MIRU = 14,
    ACTION_D_SIT = 17,
    ACTION_D_MAHI = 18,
    ACTION_FAIL = 20,
    ACTION_YOGAN_FAIL = 21,
    ACTION_D_DOZOU = 23,
    ACTION_S_DEMO = 25,
};

enum BehaviorType {
    BEHAVIOR_ROAMS_AROUND = 0,
    BEHAVIOR_GUARDS_AREA = 4,
    BEHAVIOR_UNK_12 = 12,
    BEHAVIOR_MINIBOSS = 13,
    BEHAVIOR_MSWORD_CHAMBER_AMBUSH = 14,
    BEHAVIOR_FROZEN_IN_TIME = 15,
};

// Embedded enemyfire constructor has this layout in the Darknut TU.
struct tn_enemyfire {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFireDuration;
    /* 0x006 */ be<s8> mMode;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<s16> mFireTimer;
    /* 0x00A */ u8 _00A[2];
    /* 0x00C */ gptr<mDoExt_McaMorf> mpMcaMorf;
    /* 0x010 */ be<s8> mFlameJntIdxs[10];
    /* 0x01A */ u8 _01A[2];
    /* 0x01C */ be<f32> mParticleScale[10];
    /* 0x044 */ be<s16> mFlameTimers[10];
    /* 0x058 */ be<u32> mpFlameEmitters[10];
    /* 0x080 */ cXyz mPrevPos;
    /* 0x08C */ cXyz mDirection;
    /* 0x098 */ be<f32> mFlameScaleY;
    /* 0x09C */ u8 _09C;
    /* 0x09D */ be<u8> mHitboxFlameIdx;
    /* 0x09E */ u8 _09E[2];
    /* 0x0A0 */ dCcD_Stts mStts;
    /* 0x0DC */ dCcD_Sph mSph;
    /* 0x208 */ u8 mLight[0x20];
    /* 0x228 */ be<f32> mLightExpansion;
};
WWHD_OFFSET(tn_enemyfire, mStts, 0xA0);
WWHD_OFFSET(tn_enemyfire, mSph, 0xDC);
WWHD_OFFSET(tn_enemyfire, mLightExpansion, 0x228);
WWHD_SIZE(tn_enemyfire, 0x22C);

struct tn_p {
    /* 0x00 */ gptr<J3DModel> mpPartModel;
    /* 0x04 */ gptr<mDoExt_brkAnm> mpPartBrkAnm;
    /* 0x08 */ be<s8> mPartState;  /* was m08: part_move 024BC3EC: 0 follows the body joint, 1 coming loose (set when the actor's part-break bits ask for it), 5 flying, -1 broken (yoroi_break); part_draw 024BA0F0 skips the part below 0 (tww the same) */
    /* 0x09 */ be<s8> mbDetached;  /* was m09: part_move 024BC3EC: 0 takes the body joint's matrix, 1 (set when the part comes off) builds the matrix from mPos and mAngle (tww the same) */
    /* 0x0A */ u8 _0A[2];
    /* 0x0C */ cXyz mPos;  /* was m0C: part_move 024BC3EC: the part's position, taken from the body joint while attached; tn_moveFlyingPart (inline of part_move) adds mSpeed each frame; yoroi_break's effect position (tww the same) */
    /* 0x18 */ cXyz mOldPos;  /* was m18: tn_moveFlyingPart (inline of part_move 024BC3EC): mPos before this frame's step, restored on a wall hit and used for the motion vector (tww the same) */
    /* 0x24 */ cXyz mSpeed;  /* was m24: tn_moveFlyingPart (inline of part_move 024BC3EC): added to mPos each frame, y minus 5 per frame (gravity), bounced and damped on the ground (tww the same) */
    /* 0x30 */ csXyz mAngle;  /* was m30: part_move 024BC3EC: the flying part's rotation (Y, X, Z rotM) and its start angle; tn_moveFlyingPart adds mAngleSpeed (tww the same) */
    /* 0x36 */ csXyz mAngleSpeed;  /* was m36: tn_moveFlyingPart (inline of part_move 024BC3EC): added to mAngle each frame; random on launch and bounce, 0 when the part settles (tww the same) */
    /* 0x3C */ u8 _3C[6];
    /* 0x42 */ be<s16> m42;
    /* 0x44 */ be<s8> mTimer;  /* was m44: part_move 024BC3EC counts it down each frame: the delay before a loose part flies, then 30 / 20 frames until it breaks at 1 (tww the same) */
    /* 0x45 */ u8 _45[7];
};
WWHD_OFFSET(tn_p, mPos, 0x0C);
WWHD_OFFSET(tn_p, mOldPos, 0x18);
WWHD_OFFSET(tn_p, mSpeed, 0x24);
WWHD_OFFSET(tn_p, mAngle, 0x30);
WWHD_OFFSET(tn_p, mAngleSpeed, 0x36);
WWHD_OFFSET(tn_p, m42, 0x42);
WWHD_OFFSET(tn_p, mTimer, 0x44);
WWHD_SIZE(tn_p, 0x4C);

using tn_enemyice = enemyice;
WWHD_SIZE(tn_enemyice, 0x3B8);

// Complete stack object used by the two Darknut visibility line checks.
struct tn_linecheck {
    /* 0x00 */ be<u32> mPolyPassChk;
    /* 0x04 */ be<u32> mGrpPassChk;
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> mCheckVtable;
    /* 0x14 */ u8 _14[0x0C];
    /* 0x20 */ be<u32> mLineVtable;
    /* 0x24 */ u8 _24[0x0C];
    /* 0x30 */ cXyz mIntersection;
    /* 0x3C */ u8 _3C[0x1C];
    /* 0x58 */ be<u32> mPolyPassVtable;
    /* 0x5C */ be<u8> mPassFlags[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> mGroupPassVtable;
    /* 0x68 */ be<u32> mGroupMask;
};
WWHD_OFFSET(tn_linecheck, mPolyPassVtable, 0x58);
WWHD_OFFSET(tn_linecheck, mIntersection, 0x30);
WWHD_OFFSET(tn_linecheck, mGroupPassVtable, 0x64);
WWHD_SIZE(tn_linecheck, 0x6C);

using tn_path = path_layout::dPath;

struct tn_groundcheck {
    /* 0x00 */ be<u32> mPolyPassChk;
    /* 0x04 */ be<u32> mGrpPassChk;
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> mCheckVtable;
    /* 0x14 */ u8 _14[0x0C];
    /* 0x20 */ be<u32> mGroundVtable;
    /* 0x24 */ cXyz mPosition;
    /* 0x30 */ u8 _30[0x10];
    /* 0x40 */ be<u32> mPolyPassVtable;
    /* 0x44 */ be<u8> mPassFlags[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> mGroupPassVtable;
    /* 0x50 */ be<u32> mGroupMask;
};
WWHD_OFFSET(tn_groundcheck, mPosition, 0x24);
WWHD_SIZE(tn_groundcheck, 0x54);

// HD: callback count and point array accompany its emitted vtable.
struct TnCutTurnCallback {
    be<u32> __vtbl;
    u8 _04[2];
    be<s16> mCount;
    gptr<cXyz> mpPoints;
    u8 _0C[4];
};
WWHD_OFFSET(TnCutTurnCallback, mCount, 6);
WWHD_OFFSET(TnCutTurnCallback, mpPoints, 8);
WWHD_SIZE(TnCutTurnCallback, 0x10);

// Darknut: native allocation/profile size 0x1C5C. Unnamed intervals stay
// opaque until their exact HD layout has been established from actor bodies.
struct tn_class : fopEn_enemy_c {
    /* 0x03C8 */ request_of_phase_process_class mPhaseTn;
    /* 0x03D0 */ request_of_phase_process_class mPhaseTkwn;
    /* 0x03D8 */ be<u8> mBehaviorType;
    /* 0x03D9 */ be<u8> mRangeOrFrozenAnim;
    /* 0x03DA */ be<u8> mPathIndex;
    /* 0x03DB */ be<u8> mEnableSpawnSwitch;
    /* 0x03DC */ be<u8> mDisableSpawnOnDeathSwitch;
    /* 0x03DD */ be<u8> mWaitSpawnSwitch;  /* was m02C1: daTn_Create 024C0B0C: mEnableSpawnSwitch + 1 (0 = none); executePrefix (in daTn_Execute 024BD170) waits until that switch is on, then clears it; daTn_Draw 024BA238 draws nothing while it is set */
    /* 0x03DE */ u8 _03DE[2];
    /* 0x03E0 */ gptr<mDoExt_McaMorf> mpBodyMorf;
    /* 0x03E4 */ gptr<mDoExt_McaMorf> mpShieldMorf;
    /* 0x03E8 */ gptr<mDoExt_McaMorf> mpArmorMorf;
    /* 0x03EC */ be<s8> mRemainingEquipmentPieces;
    /* 0x03ED */ u8 _03ED[3];
    /* 0x03F0 */ gptr<mDoExt_brkAnm> mpBrkAnm;
    /* 0x03F4 */ be<u8> mPartsAttached;  /* was m02DC: daTn_Create 024C0B0C sets 3; part_move 024BC3EC subtracts 1 each time an armour part (helmet, armour, shield) falls off */
    /* 0x03F5 */ be<u8> mPartDetachFlags;  /* was m02DD: applyArmoredHit (in daTn_Execute 024BD170) ORs 1 / 2 for the part that was hit off; part_move 024BC3EC detaches the part whose bit is set; executePrefix clears it */
    /* 0x03F6 */ u8 _03F6[2];
    /* 0x03F8 */ tn_p mParts[3];
    /* 0x04DC */ be<u8> mArmorColorIndex;
    /* 0x04DD */ be<u8> mEquipmentType;
    /* 0x04DE */ u8 _04DE[0x04F0 - 0x04DE];
    /* 0x04F0 */ be<u32> mFrameCount;  /* was m03D8: executePrefix (in daTn_Execute 024BD170) adds 1 every frame; tn_combatDefensiveReaction (in Tn_move 024C3E64) and yogan_fail 024C2BEC act only every 4th frame (& 3) */
    /* 0x04F4 */ u8 _04F4[4];
    /* 0x04F8 */ be<s16> mCountDownTimers[5];
    /* 0x0502 */ be<s16> mWeponRunTimer;  /* was m03EA: wepon_search 024C1A3C sets l_tnHIO.mWeponRunTime when the Darknut starts running to a dropped weapon and gives up (mWeponSearchGaveUp) when it reaches 0; counted down by executePrefix (in daTn_Execute 024BD170) */
    /* 0x0504 */ be<s16> mAnmLock;  /* was m03EC: anm_init 024B91E4, tate_anm_init 024B931C and yoroi_anm_init 024B93E8 return at once while it is set, and tn_moveCombat (in Tn_move 024C3E64) skips the turn toward mTargetAngle; nothing sets it (same in tww d217e2b) */
    /* 0x0506 */ be<s16> mWeponTurnWait;  /* was m03EE: wepon_search 024C1A3C sets 5 and turns toward the weapon only once it is 0; counted down in updateDemoCameraAndSmoke (in daTn_Execute 024BD170) */
    /* 0x0508 */ be<s16> mNoDamageTimer;  /* was m03F0: prepareDamage (in daTn_Execute 024BD170) returns at once while it runs and sets REG0_S(7) + 5 after a hit; applyArmoredHit sets 20; updateCollisionShapes moves the head sphere away while it runs; counted down by executePrefix */
    /* 0x050A */ be<s16> mNoGuardTimer;  /* was m03F2: applyOrdinaryDamage (in daTn_Execute 024BD170) sets 25; tn_combatDefensiveReaction (in Tn_move 024C3E64) does not guard while it runs; counted down by executePrefix */
    /* 0x050C */ u8 _050C[8];
    /* 0x0514 */ be<s16> mWeponSearchWait;  /* was m03FC: wepon_search 024C1A3C sets 70 when it runs into a wall; search_wepon 024BAFAC finds no weapon while it runs; counted down by executePrefix (in daTn_Execute 024BD170) */
    /* 0x0516 */ be<u8> mObstacleSide;  /* was m03FE: tn_combatObstacles (in Tn_move 024C3E64) stores the ground_4_check result (2 / 4 / 8 = side blocked); tn_combatLocomotion picks the sidestep from it */
    /* 0x0517 */ u8 _0517;
    /* 0x0518 */ be<f32> mPlayerDistance;
    // HD: an extra height-distance float precedes the path target.
    /* 0x051C */ be<f32> mPlayerHeightDistance;
    /* 0x0520 */ cXyz mPathXyz;
    /* 0x052C */ u8 _052C[4];
    /* 0x0530 */ be<s16> mHeadAngleY;  /* was m0414: nodeCallBack 024B97A0: atan2 from the eye toward a point ahead of the head joint; search_wepon 024BAFAC compares weapon directions with it (view cone) */
    /* 0x0532 */ be<s16> mTargetAngle;
    /* 0x0534 */ be<s16> mSideStepAngle;  /* was m0418: tn_combatLocomotion / tn_attackCircle (in Tn_move 024C3E64) set +-0x4000; tn_finishMovement adds it to the move heading; Tn_move 024C3E64 clears it */
    /* 0x0536 */ be<s16> mTubaEffectTimer;  /* was m041A: updateDamageAnimation / updateKnockback (in daTn_Execute 024BD170) set 5 / 3 / 1; updateCollisionShapes counts it down and at 0 sets particle 0x0E at mHeadPos with mEffectAngle (tww d217e2b calls it dPa_name::ID_AK_JN_TUBA00) */
    /* 0x0538 */ cXyz mSmokePos;  /* was m041C: updateDemoCameraAndSmoke (in daTn_Execute 024BD170) puts it on the smoke ring around the body (MtxPosition) or 25000 above mSpawnY when idle; smoke_set_s 024B94C0 drops it to the ground and emits the smoke there */
    /* 0x0544 */ csXyz mEffectAngle;  /* was m0428: updateDemoCameraAndSmoke (in daTn_Execute 024BD170) sets y = mSmokeAngle for the ground smoke; updateCollisionShapes sets y = angle.y for particle 0x0E at mHeadPos; updateDamageAnimation / updateKnockback set x (-0x4000 / 0) */
    /* 0x054A */ be<s16> mSmokeAngle;  /* was m042E: tn_attackAnimation (in Tn_move 024C3E64) seeds it from the heading; updateDemoCameraAndSmoke (in daTn_Execute 024BD170) rotates mSmokePos by it and adds a step each frame */
    /* 0x054C */ be<s16> mGroundSmokeTimer;  /* was m0430: tn_attackAnimation (in Tn_move 024C3E64) and updateDamageAnimation (in daTn_Execute 024BD170) set it (l_tnHIO.mGroundSmokeEnd + 16 ...); updateDemoCameraAndSmoke emits ground smoke while it counts down; applyIceOrShatter clears it */
    /* 0x054E */ be<u8> mGroundSmokeType;  /* was m0432: tn_attackAnimation (in Tn_move 024C3E64) sets 0 (attack ring, radius 200), updateDamageAnimation (in daTn_Execute 024BD170) 1 (fall, radius 55); updateDemoCameraAndSmoke picks the smoke ring by it */
    /* 0x054F */ be<u8> mSmokeCount;  /* was m0433: smoke_set_s 024B94C0 counts the smoke puffs (the first one differs); updateDemoCameraAndSmoke (in daTn_Execute 024BD170) clears it when the ground smoke ends */
    /* 0x0550 */ dPa_smokeEcallBack mSmokeCb1;
    /* 0x0570 */ dPa_smokeEcallBack mSmokeCb2;
    // HD: the reaction has 21 joint entries and a larger tail.
    /* 0x0590 */ damagereaction mDamageReaction;
    /* 0x0D80 */ u8 _0D80[4];
    /* 0x0D84 */ be<s32> mHasWepon;  /* was m0C34: tn_moveReturnHome / tn_movePatrol / path_check use it as "holding a weapon" (armed patrol, BCK 0x39 vs 0x3D); updateHeldWeapon (in daTn_Execute 024BD170) clears it when the weapon drops (tww d217e2b yari_off_check) */
    /* 0x0D88 */ be<u32> mWeponDropMode;  /* was m0C38: updateDamageAnimation (in daTn_Execute 024BD170) and yogan_fail 024C2BEC set 1 (throw the weapon), applyIceOrShatter 2 (drop it in place); updateHeldWeapon releases the weapon by it and clears it with mHasWepon */
    /* 0x0D8C */ cXyz mRightItemPos;  /* was m0C3C: nodeCallBack_P 024B9C1C: MtxPosition of reaction joint 20 (model joint 23, J_TN_ITEM_R1: joint_check at 101D2558 and tww d217e2b joint_check / Tn.h), the hand that holds the weapon; never read */
    /* 0x0D98 */ be<s32> mAttackType;  /* was m0C48: tn_chooseAttack (in Tn_move 024C3E64) sets 0..3 (and 4); tn_attackAnimation and wepon_hit_check 024BBB3C pick the swing animation and the hit frame window by it */
    /* 0x0D9C */ u8 _0D9C[0x0DB6 - 0x0D9C];
    /* 0x0DB6 */ be<s8> mAttackState;  /* was m0C66: tn_chooseAttack (in Tn_move 024C3E64) sets 1 at an attack start; tn_attackAnimation sets -1 when Link's guard bounces it; wepon_hit_check 024BBB3C does no hit while it is < 0 */
    /* 0x0DB7 */ be<s8> mKickLunge;  /* was m0C67: tn_moveAttack (in Tn_move 024C3E64) sets 1 for the kick; tn_attackAnimation moves forward (speed 80) during it; tn_chooseAttack clears it */
    /* 0x0DB8 */ u8 _0DB8[0x0DC4 - 0x0DB8];
    /* 0x0DC4 */ dCcD_Cyl mCoCyl;
    /* 0x0EF4 */ dCcD_Cyl mTgCyl;
    /* 0x1024 */ dCcD_Sph mHeadSph;
    /* 0x1150 */ dCcD_Sph mDefenceSph;
    /* 0x127C */ dCcD_Sph mWeponSph;
    /* 0x13A8 */ dCcD_Sph mWepon2Sph;
    /* 0x14D4 */ cXyz mHeadPos;  /* was m1384: nodeCallBack 024B97A0: MtxPosition of the head joint; updateCollisionShapes (in daTn_Execute 024BD170) sets particle 0x0E there; yogan_fail 024C2BEC emits particle 0x0E there */
    /* 0x14E0 */ cXyz mRightItemFrontPos;  /* was m1390: nodeCallBack_P 024B9C1C: MtxPosition of offset x = 165 on reaction joint 20 (J_TN_ITEM_R1, see mRightItemPos); never read */
    /* 0x14EC */ cXyz mRightItemBackPos;  /* was m139C: nodeCallBack_P 024B9C1C: MtxPosition of offset x = -50 on reaction joint 20 (J_TN_ITEM_R1, see mRightItemPos); never read */
    /* 0x14F8 */ cXyz mLeftArmPos;  /* was m13A8: nodeCallBack_P 024B9C1C: MtxPosition of offset (17.5, -8.75, 0) on reaction joint 17 (model joint 14, J_TN_UDE_L3: joint_check at 101D2558 and tww d217e2b joint_check / Tn.h); never read */
    /* 0x1504 */ cXyz mRightArmPos;  /* was m13B4: nodeCallBack_P 024B9C1C: MtxPosition of reaction joint 16 (model joint 22, J_TN_UDE_R3: joint_check at 101D2558 and tww d217e2b joint_check / Tn.h); never read */
    /* 0x1510 */ cXyz mWeponSphMove;
    /* 0x151C */ cXyz mWepon2SphMove;
    /* 0x1528 */ cXyz mPrevWeponSphMove;  /* was m13D8: wepon_hit_check 024BBB3C copies mWeponSphMove here before it computes the new one */
    /* 0x1534 */ cXyz mDefenceSphPos;  /* was m13E4: prepareDamage (in daTn_Execute 024BD170): MtxPosition of the body joint; the centre of mDefenceSph */
    /* 0x1540 */ u8 _1540[0x154C - 0x1540];
    /* 0x154C */ be<u8> m13FC;
    /* 0x154D */ be<u8> mWeponAtStarted;  /* was m13FD: wepon_hit_check 024BBB3C starts the weapon spheres (StartCAt) while 0 and sets 1, then only moves them; tn_chooseAttack (in Tn_move 024C3E64) clears it at a new attack */
    /* 0x154E */ u8 _154E;
    /* 0x154F */ be<u8> mWeponSearchGaveUp;  /* was m13FF: tn_movePatrol (in Tn_move 024C3E64) sets 1 when mWeponRunTimer ran out without the weapon; the patrol then follows the path as if armed (mHasWepon || it); wepon_search 024C1A3C clears it */
    /* 0x1550 */ be<s8> mItemStolen;  /* was m1400: applyOrdinaryDamage (in daTn_Execute 024BD170) sets 1 after a steal hit (kind 14) and passes it as stealItemBitNo; daTn_Draw 024BA238 hides shape 0 while it is set */
    /* 0x1551 */ u8 _1551;
    /* 0x1552 */ be<s16> mArmorShakeTimer;  /* was m1402: applyArmoredHit (in daTn_Execute 024BD170) sets 15; part_move 024BC3EC wobbles the helmet part by it; counted down by executePrefix */
    /* 0x1554 */ be<s16> m1404;
    /* 0x1556 */ u8 _1556[2];
    /* 0x1558 */ be<u32> mLookTargetId;  /* was m1408: aite_miru 024C29CC looks at the actor with this process id while it waits and resets it to -1 at the end; updateDamageAnimation (in daTn_Execute 024BD170) sets -1 */
    /* 0x155C */ be<s8> mWeponCarryPending;  /* was m140C: daTn_Create 024C0B0C sets 1; executePrefix (in daTn_Execute 024BD170) clears it once the weapon actor exists (and puts it in the hand); updateHeldWeapon keeps mHasWepon only while it is 0 or the weapon is held */
    /* 0x155D */ u8 _155D[3];
    /* 0x1560 */ be<u32> mBokoPcId;
    /* 0x1564 */ u8 _1564[4];
    /* 0x1568 */ gptr<fopAc_ac_c> mpBomb;  /* was m1418: b_nige 024C1ED4 stores search_bomb's bomb; the run-away move flees from it */
    /* 0x156C */ be<u16> mHitTypeFlags;  /* was m141C: applyOrdinaryDamage (in daTn_Execute 024BD170) ORs the flags of every hit in; search_bomb 024BAB8C looks for bombs only once bit 0x200 is set */
    /* 0x156E */ u8 _156E[2];
    /* 0x1570 */ be<s32> m1420;
    /* 0x1574 */ be<u8> mPathNo;  /* was m1424: daTn_Create 024C0B0C and path_check 024BB800: mPathIndex + 1 (0 = no path); tn_advancePatrolTarget / tn_patrolLocomotion (in Tn_move 024C3E64) follow the path only when it is set */
    /* 0x1575 */ be<s8> mPathPoint;  /* was m1425: path_check 024BB800 picks the nearest point; tn_advancePatrolTarget (in Tn_move 024C3E64) steps it by mPathDir and wraps; tn_currentPatrolPoint reads it */
    /* 0x1576 */ be<u8> mPathDir;  /* was m1426: daTn_Create 024C0B0C sets 1; tn_advancePatrolTarget (in Tn_move 024C3E64) flips it to -1 (0xFF) / 1 at the path ends; path_check 024BB800 offsets the start point by it */
    /* 0x1577 */ u8 _1577;
    /* 0x1578 */ gptr<tn_path> ppd;
    /* 0x157C */ u8 _157C[6];
    /* 0x1582 */ be<u8> mHioSet;  /* was m1433: daTn_Create 024C0B0C sets 1 when it created the l_tnHIO child; daTn_Delete 024C0194 deletes the child only then */
    /* 0x1583 */ be<s8> mMahiDelay;  /* was m1434: applyOrdinaryDamage (in daTn_Execute 024BD170) sets REG0_S(3) + 8 at a kind-10 hit, else 0; tn_updateMovementTarget (in Tn_move 024C3E64) counts it down and starts ACTION_D_MAHI at 0 */
    /* 0x1584 */ be<s16> mMoveTargetTimer;  /* was m1436: tn_updateMovementTarget (in Tn_move 024C3E64) counts it down and moves toward mMoveTargetPos instead of the player while it runs; nothing sets it (same in tww d217e2b) */
    /* 0x1586 */ u8 _1586[2];
    /* 0x1588 */ cXyz mMoveTargetPos;  /* was m1438: tn_updateMovementTarget (in Tn_move 024C3E64) uses it as the move target while mMoveTargetTimer runs; nothing sets it (same in tww d217e2b) */
    /* 0x1594 */ be<s8> mDefenceSphOn;  /* was m1444: defence 024C2430 sets 1; prepareDamage (in daTn_Execute 024BD170) then places mDefenceSph at mDefenceSphPos and counts it down */
    /* 0x1595 */ u8 _1595[3];
    /* 0x1598 */ be<u32> mMantPcId;
    /* 0x159C */ TnCutTurnCallback mCutTurnEcallBack[3];  /* was m144C: tn_class_construct 024C0924 constructs the 3 callbacks; finishNormalEffects (in daTn_Execute 024BD170) drives the spin-attack trails with them; daTn_Delete 024C0194 ends them */
    /* 0x15CC */ cXyz mCutTurnPoints[10];  /* was m147C: finishNormalEffects (in daTn_Execute 024BD170) fills the 10 trail points around the sword and hands them to mCutTurnEcallBack (mpPoints) */
    /* 0x1644 */ be<s16> mCutTurnStep;  /* was m14F4: tn_moveAttack (in Tn_move 024C3E64) sets 1 at a spin attack; finishNormalEffects (in daTn_Execute 024BD170) draws the trails while it is set, steps it, clears it when mCutTurnTimer ends */
    /* 0x1646 */ be<s16> mCutTurnTimer;  /* was m14F6: tn_moveAttack (in Tn_move 024C3E64) sets 7 / 3; finishNormalEffects (in daTn_Execute 024BD170) counts it down, picks the trail colours by it (>= 5) and ends the trails at 0 */
    /* 0x1648 */ be<s8> mDemoMode;  /* was m14F8: updateKnockback (in daTn_Execute 024BD170) sets 50 (death demo request, stops the sub BGM); updateDemoCameraAndSmoke sets 51 when the event runs and drives the demo camera; executePrefix suppresses attention while set; fail 024C2AAC checks it */
    /* 0x1649 */ u8 _1649;
    /* 0x164A */ be<s16> mDemoTimer;  /* was m14FA: updateDemoCameraAndSmoke (in daTn_Execute 024BD170): cleared when the death demo starts, +1 per demo frame */
    /* 0x164C */ cXyz mCameraEyePos;
    /* 0x1658 */ cXyz mCenterPos;
    /* 0x1664 */ be<f32> mDemoCamFovy;  /* was m1514: updateDemoCameraAndSmoke (in daTn_Execute 024BD170): 55 at the demo start, eased to 30; passed as the fovy of the demo camera set */
    /* 0x1668 */ be<s8> mNoDraw;  /* was m1520: fail 024C2AAC sets 1; daTn_Draw 024BA238 draws nothing while it is set; updateDemoCameraAndSmoke (in daTn_Execute 024BD170) stops moving the camera centre (GameCube offset 0x1518: tww d217e2b names it m1518) */
    /* 0x1669 */ be<s8> mDemoSide;  /* was m1519: s_demo 024C3348 sets 1 / 0 for the two start positions; picks demo mode 13 / 12 from it */
    /* 0x166A */ u8 _166A[2];
    /* 0x166C */ be<f32> mReturnAnmMorf;  /* was m151C: tn_moveReturnHome (in Tn_move 024C3E64) uses it as the morf of the return animations; daTn_Create 024C0B0C sets 1 for the 3 create-time executes, then 10 */
    /* 0x1670 */ be<s8> mStatusTimer; // HD: delayed actor-status release.
    /* 0x1671 */ u8 _1671[3];
    /* 0x1674 */ tn_enemyice mEnemyIce;
    /* 0x1A2C */ tn_enemyfire mEnemyFire;
    /* 0x1C58 */ gptr<void> mpJntHit;
};
WWHD_OFFSET(tn_class, mPhaseTn, 0x3C8);
WWHD_OFFSET(tn_class, mBehaviorType, 0x3D8);
WWHD_OFFSET(tn_class, mpBodyMorf, 0x3E0);
WWHD_OFFSET(tn_class, mRemainingEquipmentPieces, 0x3EC);
WWHD_OFFSET(tn_class, mpBrkAnm, 0x3F0);
WWHD_OFFSET(tn_class, mParts, 0x3F8);
WWHD_OFFSET(tn_class, mEquipmentType, 0x4DD);
WWHD_OFFSET(tn_class, mArmorColorIndex, 0x4DC);
WWHD_OFFSET(tn_class, mAnmLock, 0x504);
WWHD_OFFSET(tn_class, mCountDownTimers, 0x4F8);
WWHD_OFFSET(tn_class, mHasWepon, 0xD84);
WWHD_OFFSET(tn_class, mAttackType, 0xD98);
WWHD_OFFSET(tn_class, mAttackState, 0xDB6);
static_assert(offsetof(tn_class, mDamageReaction) + offsetof(damagereaction, mMode) == 0x594);
static_assert(offsetof(tn_class, mDamageReaction) + offsetof(damagereaction, mAction) == 0x596);
static_assert(offsetof(tn_class, mDamageReaction) + offsetof(damagereaction, mJntPos) == 0x69C);
static_assert(offsetof(tn_class, mDamageReaction) + offsetof(damagereaction, mJntAngle) == 0x61E);
WWHD_OFFSET(tn_class, mSmokeCb1, 0x550);
WWHD_OFFSET(tn_class, mSmokePos, 0x538);
WWHD_OFFSET(tn_class, mSmokeCount, 0x54F);
static_assert(offsetof(tn_class, mDamageReaction) + offsetof(damagereaction, mpEnemy) == 0x590);
WWHD_OFFSET(tn_class, mCoCyl, 0xDC4);
WWHD_OFFSET(tn_class, mTgCyl, 0xEF4);
WWHD_OFFSET(tn_class, mHeadSph, 0x1024);
WWHD_OFFSET(tn_class, mDefenceSph, 0x1150);
WWHD_OFFSET(tn_class, mWeponSph, 0x127C);
WWHD_OFFSET(tn_class, mWepon2Sph, 0x13A8);
WWHD_OFFSET(tn_class, mpBomb, 0x1568);
WWHD_OFFSET(tn_class, mHeadPos, 0x14D4);
WWHD_OFFSET(tn_class, mWeponSphMove, 0x1510);
WWHD_OFFSET(tn_class, mWeponAtStarted, 0x154D);
WWHD_OFFSET(tn_class, mWeponSearchGaveUp, 0x154F);
WWHD_OFFSET(tn_class, mBokoPcId, 0x1560);
WWHD_OFFSET(tn_class, mPathNo, 0x1574);
WWHD_OFFSET(tn_class, ppd, 0x1578);
WWHD_OFFSET(tn_class, mEnemyFire, 0x1A2C);
WWHD_OFFSET(tn_class, mHioSet, 0x1582);
WWHD_OFFSET(tn_class, mMantPcId, 0x1598);
WWHD_OFFSET(tn_class, mCutTurnEcallBack, 0x159C);
WWHD_OFFSET(tn_class, mNoDraw, 0x1668);
WWHD_OFFSET(tn_class, mEnemyIce, 0x1674);
WWHD_OFFSET(tn_class, mpJntHit, 0x1C58);
WWHD_SIZE(tn_class, 0x1C5C);

void Tn_move(tn_class* actor);
BOOL daTn_Execute(tn_class* actor);

struct tnHIO_c {
    be<u32> __vtbl;
    /* 0x004 */ be<s8> mNo;
    /* 0x005 */ be<u8> m005;
    /* 0x006 */ be<u8> mPadStep;  /* was m006: written only by the HIO constructor tn_staticInit 024C161C (0); read only in tww daTn_Execute: while it is not 0 the Darknut's frame (timers and move) runs only on a pad B trigger or Y hold (frame-step debugging) */
    /* 0x007 */ be<u8> mDebugHealth;  /* was m007: written only by the HIO constructor tn_staticInit 024C161C (0); read only in tww damage_check: after a hit 1 sets health 10, 2 sets health 0 */
    /* 0x008 */ be<u8> m008;
    /* 0x009 */ u8 m009[0x00A - 0x009];
    /* 0x00A */ be<s16> m00A;
    /* 0x00C */ be<f32> m00C;
    /* 0x010 */ u8 m010[0x014 - 0x010];
    /* 0x014 */ be<f32> m014;
    /* 0x018 */ be<f32> mDropOffsetY;  /* was m018: fail 024C2AAC: the fopAcM_createDisappear drop point is current.pos.y + it + 100; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x01C */ be<s16> m01C;
    /* 0x01E */ be<s16> mSmokeAlpha;  /* was m01E: smoke_set_s 024B94C0: alpha of the ground-smoke particle 0x2022; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x020 */ be<s16> mGroundSmokeEnd;  /* was m020: tn_attackAnimation (in Tn_move 024C3E64) / updateDamageAnimation (in daTn_Execute 024BD170) load mGroundSmokeTimer with it + 14 / + 16; updateDemoCameraAndSmoke (in daTn_Execute) sets the smoke only while the timer is still >= it; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x022 */ u8 m022[0x024 - 0x022];
    /* 0x024 */ be<f32> mAttentionOffsetY;  /* was m024: nodeCallBack 024B97A0: attention_info.position.y = eyePos.y + it; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x028 */ be<f32> mNoticeRange;  /* was m028: tn_movePatrol (in Tn_move 024C3E64): the patrol checks the player view only while mPlayerDistance < it; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x02C */ be<f32> mFightRange;  /* was m02C: tn_combatLocomotion / tn_combatRangeTransition / tn_finishAttack (in Tn_move 024C3E64), hukki 024C25F8: within it the fight run goes to the fight stance (mode 2), beyond it + 75 the Darknut runs again; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x030 */ be<f32> mAttackRange;  /* was m030: tn_combatLocomotion / tn_combatRangeTransition / tn_attackCircle (in Tn_move 024C3E64): an attack starts at mPlayerDistance < it (and within it +- 62.5 / 25); set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x034 */ be<s16> mViewAngle;  /* was m034: get_view_H 024BAA68: the view half-angle for daTn_player_view_check / search_wepon without the helmet; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x036 */ be<s16> mViewAngleHelmet;  /* was m036: get_view_H 024BAA68: the view half-angle while the helmet is on (EQUIPMENT_PIECE_HELMET); set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x038 */ be<f32> mViewHeight;  /* was m038: daTn_player_view_check 024BB404, search_wepon 024BAFAC: no sight when the height difference to the target is more than it; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x03C */ be<f32> mViewBoxX;  /* was m03C: daTn_player_view_check 024BB404, search_wepon 024BAFAC: sight box, |local x| < it; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x040 */ be<f32> mViewBoxY;  /* was m040: daTn_player_view_check 024BB404, search_wepon 024BAFAC: sight box, |local y| < it; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x044 */ be<f32> mViewBoxFront;  /* was m044: daTn_player_view_check 024BB404, search_wepon 024BAFAC, search_bomb 024BAB8C: sight box, local z < it; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x048 */ be<f32> mViewBoxBack;  /* was m048: daTn_player_view_check 024BB404, search_wepon 024BAFAC, search_bomb 024BAB8C: sight box, local z > it (-125); set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x04C */ be<f32> mPathWalkSpeed;  /* was m04C: tn_patrolLocomotion (in Tn_move 024C3E64): patrol speed along the path; tn_combatLocomotion: * 1.5 is the fight-run speed with the body armor; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x050 */ be<f32> mWalkSpeed;  /* was m050: tn_patrolLocomotion (in Tn_move 024C3E64): patrol speed without a path; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x054 */ be<f32> mRunSpeed;  /* was m054: tn_combatLocomotion (in Tn_move 024C3E64): fight-run speed without the body armor; tn_moveReturnHome: run speed back home; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x058 */ be<f32> mWeponRunSpeed;  /* was m058: wepon_search 024C1A3C: speed while running to the dropped weapon; b_nige 024C1ED4: speed while fleeing a bomb; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x05C */ be<f32> mFightStepSpeed;  /* was m05C: tn_combatLocomotion / tn_attackCircle (in Tn_move 024C3E64): speedF target (+ / -) of the fight-stance steps forward / back; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x060 */ be<f32> mSideStepSpeed;  /* was m060: tn_combatLocomotion / tn_attackCircle (in Tn_move 024C3E64): speedF target of the side steps (mSideStepAngle 0x4000 / 0xC000); set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x064 */ be<f32> m064;
    /* 0x068 */ be<f32> m068;
    /* 0x06C */ be<s16> mAttackCheckDelay;  /* was m06C: tn_combatRangeTransition (in Tn_move 024C3E64): mCountDownTimers[3] reload after each attack roll; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x06E */ u8 m06E[0x070 - 0x06E];
    /* 0x070 */ be<f32> mAttackChance;  /* was m070: tn_combatRangeTransition (in Tn_move 024C3E64): the attack starts only when cM_rndF(100) < it; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x074 */ be<f32> m074;
    /* 0x078 */ be<f32> mVerticalCutChance;  /* was m078: tn_chooseAttack (in Tn_move 024C3E64): with the weapon, cM_rndF(100) < it picks the vertical cut (YAT_TATE1, mAttackType 0), else the side cut (YAT_YOKO1, 1); set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x07C */ be<f32> mPalmStrikeChance;  /* was m07C: tn_chooseAttack (in Tn_move 024C3E64): without the weapon, cM_rndF(100) < it (or the body armor on) picks the palm strike BAT_SYOUTEI_S1 (mAttackType 2); set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x080 */ be<s16> mDownRecoverDelay;  /* was m080: updateDamageAnimation (in daTn_Execute 024BD170): mDamageReaction.mDownTimers[2] (the recovery delay) for the reactions 20 / 21; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x082 */ be<s16> mWeponRunTime;  /* was m082: wepon_search 024C1A3C: mWeponRunTimer at the start of the run to the dropped weapon; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x084 */ be<f32> m084;
    /* 0x088 */ be<f32> m088;
    /* 0x08C */ be<f32> m08C;
    /* 0x090 */ be<f32> m090;
    /* 0x094 */ be<f32> m094;
    /* 0x098 */ be<f32> m098;
    /* 0x09C */ be<f32> m09C;
    /* 0x0A0 */ be<f32> m0A0;
    /* 0x0A4 */ be<f32> m0A4;
    /* 0x0A8 */ be<f32> m0A8;
    /* 0x0AC */ be<f32> m0AC;
    /* 0x0B0 */ be<f32> m0B0;
    /* 0x0B4 */ be<f32> m0B4;
    /* 0x0B8 */ be<f32> m0B8;
    /* 0x0BC */ be<f32> m0BC;
    /* 0x0C0 */ be<f32> m0C0;
    /* 0x0C4 */ be<s16> m0C4;
    /* 0x0C6 */ be<s16> m0C6;
    /* 0x0C8 */ be<s16> m0C8;
    /* 0x0CA */ be<s16> m0CA;
    /* 0x0CC */ be<s16> m0CC;
    /* 0x0CE */ be<s16> m0CE;
    /* 0x0D0 */ be<f32> mWeakHitPushback;  /* was m0D0: updateKnockback (in daTn_Execute 024BD170): mDamageReaction.mPushback = -it for a hit with mKnockbackPower < 25; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    /* 0x0D4 */ be<f32> mWeponHitMaxDis;  /* was m0D4: tn_attackAnimation (in Tn_move 024C3E64): copied to mBtMaxDis for every attack; set only by the HIO constructor (tn_staticInit 024C161C), never written in WWHD or tww */
    u8 mJntHitHIO[0x2C];
};
WWHD_SIZE(tnHIO_c, 0x104);
WWHD_OFFSET(tnHIO_c, mAttentionOffsetY, 0x24);

// HD: these entry points keep the exact result/register widths used by this TU.
inline f32 tn_sqrtf(f32 value) {
    return gabi::call<f32>(0x028F4384, value);
}
inline void tn_MtxRotY(s32 concatenate, f32 angle) {
    gabi::call(0x0200FBA4, concatenate, angle);
}
inline void tn_VECAdd(cXyz* a, cXyz* b, cXyz* output) {
    gabi::call(0x028E8D88, a, b, output);
}
inline BOOL tn_IsActor(fopAc_ac_c* actor) {
    return gabi::call<BOOL>(0x025D4604, actor);
}
inline BOOL tn_bombState(fopAc_ac_c* actor, s32 state) {
    return gabi::call<BOOL>(0x020CB92C, actor, state);
}
inline void tn_Search(u32 callback, tn_class* context) {
    gabi::call(0x025DE508, callback, context);
}
inline dCcD_GObjInf* tn_GetAtHitObj(dCcD_Sph* sphere) {
    return gabi::call<dCcD_GObjInf*>(0x02516178, sphere);
}
inline void tn_StartCAt(dCcD_Sph* sphere, cXyz* pos) {
    gabi::call(0x025167C0, sphere, pos);
}
inline void tn_MoveCAt(dCcD_Sph* sphere, cXyz* pos) {
    gabi::call(0x025167E4, sphere, pos);
}
inline void tn_SetCollision(void* cc, dCcD_Sph* sphere) {
    gabi::call(0x0200E240, cc, sphere);
}
inline void tn_iceEntryDL(mDoExt_McaMorf* morph, s32 index, void* tev) {
    gabi::call(0x0259138C, morph, index, tev);
}
inline void tn_iceUpdateDL(J3DModel* model, s32 index, void* tev) {
    gabi::call(0x02591200, model, index, tev);
}
inline void* tn_envLight() {
    return gabi::call<void*>(0x02555D0C);
}
inline void tn_setLight(void* light, J3DModel* model, dKy_tevstr_c* tev) {
    gabi::call(0x02562F5C, light, model, tev);
}
inline void tn_RegistFig(s32 figure, tn_class* actor, f32 x, f32 y, f32 z) {
    gabi::call(0x025BED80, figure, actor, x, y, z);
}
inline void tn_modelUpdateDL(J3DModel* model, s32 flag) {
    gabi::call(0x025E2DE0, model, flag);
}
inline s32 tn_getReverb(s8 room) {
    return gabi::call<s32>(0x02520540, room);
}
inline void tn_seStart(s32 sound, cXyz* pos, s32 flag, s32 reverb) {
    gabi::call(0x025E1A40, sound, pos, flag, reverb);
}
inline s32 tn_materialIndex(void* table, const char* name) {
    return gabi::call<s32>(0x027DF9B0, table, name);
}
inline u32 tn_GetAttributeCode(dBgS* bg, void* poly) {
    return gabi::call<u32>(0x024EF0F4, bg, poly);
}
inline u32 tn_SearchByID(u32 callback, be<u32>* id) {
    return gabi::call<u32>(0x025D5218, callback, id);
}

fopAc_ac_c* search_bomb(tn_class*, int);
BOOL daTn_player_bg_check(tn_class*, cXyz*);

struct TnTargets {
    gptr<fopAc_ac_c> m[10];
};
struct TnJointChecks {
    be<s8> m[33];
};
struct TnPathChecks {
    be<u8> m[255];
};
struct TnFloat4 {
    be<f32> m[4];
};
struct TnByte4 {
    be<u8> m[4];
};
struct TnPower6 {
    be<u32> m[6];
};
struct TnColor6 {
    GXColor m[6];
};
WWHD_SIZE(TnTargets, 40);
WWHD_SIZE(TnJointChecks, 33);
WWHD_SIZE(TnPathChecks, 255);
WWHD_SIZE(TnFloat4, 16);
WWHD_SIZE(TnPower6, 24);

// HD: header-local joint-number view avoids a second toJoint call.
struct TnJoint : J3DNode {
    be<u16> mJntNo;
};
WWHD_OFFSET(TnJoint, mJntNo, 4);
struct TnResource {
    u8 _00[0x18];
    be<u32> mMaterialNameOffset;
};
WWHD_OFFSET(TnResource, mMaterialNameOffset, 0x18);

// HD: preserve wrapping guest indexing when the diagnostic asserts and continues.
inline DamageAngles* tn_jointAngles(tn_class* actor, s8 index) {
    return gabi::at<DamageAngles>(gabi::ea(&actor->mDamageReaction.mJntAngle[0]) +
                                  static_cast<u32>(static_cast<s32>(index)) * 6u);
}
inline cXyz* tn_jointPosition(tn_class* actor, s8 index) {
    return gabi::at<cXyz>(gabi::ea(&actor->mDamageReaction.mJntPos[0]) +
                          static_cast<u32>(static_cast<s32>(index)) * 12u);
}

enum dRes_INDEX_TN {
    /* BAS  */
    dRes_INDEX_TN_BAS_AAT_KAITEN2_e = 0x6,
    dRes_INDEX_TN_BAS_AAT_KAITEN3_e = 0x7,
    dRes_INDEX_TN_BAS_AAT_KAITEN4_e = 0x8,
    dRes_INDEX_TN_BAS_AAT_KAITEN_FB1_e = 0x9,
    dRes_INDEX_TN_BAS_AAT_KAITEN_LR1_e = 0xA,
    dRes_INDEX_TN_BAS_AAT_TATE1_e = 0xB,
    dRes_INDEX_TN_BAS_AAT_YOKO1_e = 0xC,
    dRes_INDEX_TN_BAS_AHAJIKI1_e = 0xD,
    dRes_INDEX_TN_BAS_AHAZURE1_e = 0xE,
    dRes_INDEX_TN_BAS_ASURI_FB1_e = 0xF,
    dRes_INDEX_TN_BAS_ASURI_LR1_e = 0x10,
    dRes_INDEX_TN_BAS_AWALK1_e = 0x11,
    dRes_INDEX_TN_BAS_BAT_JUMP1_e = 0x12,
    dRes_INDEX_TN_BAS_BAT_KAITEN2_e = 0x13,
    dRes_INDEX_TN_BAS_BAT_KAITEN3_e = 0x14,
    dRes_INDEX_TN_BAS_BAT_KAITEN4_e = 0x15,
    dRes_INDEX_TN_BAS_BAT_KERI1_e = 0x16,
    dRes_INDEX_TN_BAS_BAT_SHOUTEI_L1_e = 0x17,
    dRes_INDEX_TN_BAS_BAT_SYOUTEI_S1_e = 0x18,
    dRes_INDEX_TN_BAS_BAT_TATE1_e = 0x19,
    dRes_INDEX_TN_BAS_BAT_YOKO1_e = 0x1A,
    dRes_INDEX_TN_BAS_BBACK1_e = 0x1B,
    dRes_INDEX_TN_BAS_BBACK_S1_e = 0x1C,
    dRes_INDEX_TN_BAS_BHIROU_K1_e = 0x1D,
    dRes_INDEX_TN_BAS_BOKIRU_A1_e = 0x1E,
    dRes_INDEX_TN_BAS_BOKIRU_U1_e = 0x1F,
    dRes_INDEX_TN_BAS_BRUN1_e = 0x20,
    dRes_INDEX_TN_BAS_BRUN2_e = 0x21,
    dRes_INDEX_TN_BAS_BSURI_SFB1_e = 0x22,
    dRes_INDEX_TN_BAS_BSURI_SLR1_e = 0x23,
    dRes_INDEX_TN_BAS_S_DEMO_M1_e = 0x24,
    dRes_INDEX_TN_BAS_S_DEMO_M2_e = 0x25,
    /* BCK  */
    dRes_INDEX_TN_BCK_AAT_KAITEN1_e = 0x28,
    dRes_INDEX_TN_BCK_AAT_KAITEN2_e = 0x29,
    dRes_INDEX_TN_BCK_AAT_KAITEN3_e = 0x2A,
    dRes_INDEX_TN_BCK_AAT_KAITEN4_e = 0x2B,
    dRes_INDEX_TN_BCK_AAT_KAITEN_FB1_e = 0x2C,
    dRes_INDEX_TN_BCK_AAT_KAITEN_LR1_e = 0x2D,
    dRes_INDEX_TN_BCK_AAT_TATE1_e = 0x2E,
    dRes_INDEX_TN_BCK_AAT_YOKO1_e = 0x2F,
    dRes_INDEX_TN_BCK_AGUARD1_e = 0x30,
    dRes_INDEX_TN_BCK_AGUARD2_e = 0x31,
    dRes_INDEX_TN_BCK_AHAJIKI1_e = 0x32,
    dRes_INDEX_TN_BCK_AHAKAI1_e = 0x33,
    dRes_INDEX_TN_BCK_AHAZURE1_e = 0x34,
    dRes_INDEX_TN_BCK_AKAMAE1_e = 0x35,
    dRes_INDEX_TN_BCK_AKIDUKU1_e = 0x36,
    dRes_INDEX_TN_BCK_AKYORO1_e = 0x37,
    dRes_INDEX_TN_BCK_AMAHI1_e = 0x38,
    dRes_INDEX_TN_BCK_ANIOU1_e = 0x39,
    dRes_INDEX_TN_BCK_ASURI_FB1_e = 0x3A,
    dRes_INDEX_TN_BCK_ASURI_LR1_e = 0x3B,
    dRes_INDEX_TN_BCK_ATATE_ON1_e = 0x3C,
    dRes_INDEX_TN_BCK_AWAIT1_e = 0x3D,
    dRes_INDEX_TN_BCK_AWALK1_e = 0x3E,
    dRes_INDEX_TN_BCK_BAT_JUMP1_e = 0x3F,
    dRes_INDEX_TN_BCK_BAT_KAITEN2_e = 0x40,
    dRes_INDEX_TN_BCK_BAT_KAITEN3_e = 0x41,
    dRes_INDEX_TN_BCK_BAT_KAITEN4_e = 0x42,
    dRes_INDEX_TN_BCK_BAT_KERI1_e = 0x43,
    dRes_INDEX_TN_BCK_BAT_SYOUTEI_L1_e = 0x44,
    dRes_INDEX_TN_BCK_BAT_SYOUTEI_S1_e = 0x45,
    dRes_INDEX_TN_BCK_BAT_TATE1_e = 0x46,
    dRes_INDEX_TN_BCK_BAT_YOKO1_e = 0x47,
    dRes_INDEX_TN_BCK_BBACK1_e = 0x48,
    dRes_INDEX_TN_BCK_BBACK_S1_e = 0x49,
    dRes_INDEX_TN_BCK_BDOWN_A1_e = 0x4A,
    dRes_INDEX_TN_BCK_BDOWN_U1_e = 0x4B,
    dRes_INDEX_TN_BCK_BHIROU_K1_e = 0x4C,
    dRes_INDEX_TN_BCK_BKAMAE_S1_e = 0x4D,
    dRes_INDEX_TN_BCK_BOKIRU_A1_e = 0x4E,
    dRes_INDEX_TN_BCK_BOKIRU_U1_e = 0x4F,
    dRes_INDEX_TN_BCK_BRUN1_e = 0x50,
    dRes_INDEX_TN_BCK_BRUN2_e = 0x51,
    dRes_INDEX_TN_BCK_BSURI_SFB1_e = 0x52,
    dRes_INDEX_TN_BCK_BSURI_SLR1_e = 0x53,
    dRes_INDEX_TN_BCK_S_DEMO_M1_e = 0x54,
    dRes_INDEX_TN_BCK_S_DEMO_M2_e = 0x55,
    dRes_INDEX_TN_BCK_S_DEMO_Y1_e = 0x56,
    dRes_INDEX_TN_BCK_S_DEMO_Y2_e = 0x57,
    dRes_INDEX_TN_BCK_YAT_KAITEN1_e = 0x58,
    dRes_INDEX_TN_BCK_YAT_KAITEN2_e = 0x59,
    dRes_INDEX_TN_BCK_YAT_KAITEN3_e = 0x5A,
    dRes_INDEX_TN_BCK_YAT_KAITEN4_e = 0x5B,
    dRes_INDEX_TN_BCK_YAT_TATE1_e = 0x5C,
    dRes_INDEX_TN_BCK_YAT_YOKO1_e = 0x5D,
    dRes_INDEX_TN_BCK_YGUARD1_e = 0x5E,
    dRes_INDEX_TN_BCK_YHAJIKI1_e = 0x5F,
    dRes_INDEX_TN_BCK_YHAKAI1_e = 0x60,
    dRes_INDEX_TN_BCK_YHAZURE1_e = 0x61,
    dRes_INDEX_TN_BCK_YKAMAE1_e = 0x62,
    dRes_INDEX_TN_BCK_YKIDUKU1_e = 0x63,
    dRes_INDEX_TN_BCK_YKIHON1_e = 0x64,
    dRes_INDEX_TN_BCK_YKYORO1_e = 0x65,
    dRes_INDEX_TN_BCK_YMAHI1_e = 0x66,
    dRes_INDEX_TN_BCK_YWALK1_e = 0x67,
    /* BMDM */
    dRes_INDEX_TN_BMD_TN_KABUTO1_e = 0x6A,
    dRes_INDEX_TN_BMD_TN_KABUTO2_e = 0x6B,
    dRes_INDEX_TN_BMD_TN_MAIN_e = 0x6C,
    dRes_INDEX_TN_BMD_TN_TATE1_e = 0x6D,
    dRes_INDEX_TN_BMD_TN_YOROI1_e = 0x6E,
    /* BRK  */
    dRes_INDEX_TN_BRK_TN_KABUTO1_e = 0x71,
    dRes_INDEX_TN_BRK_TN_KABUTO2_e = 0x72,
    dRes_INDEX_TN_BRK_TN_MAIN_e = 0x73,
    dRes_INDEX_TN_BRK_TN_TATE1_e = 0x74,
    dRes_INDEX_TN_BRK_TN_YOROI1_e = 0x75,
};

enum RemainingEquipmentPieces {
    EQUIPMENT_PIECE_BODY_ARMOR = 1,
    EQUIPMENT_PIECE_HELMET = 2,
    EQUIPMENT_PIECE_SHIELD = 4,
    EQUIPMENT_PIECE_UNKNOWN = 8,
};
enum EquipmentType {
    EQUIPMENT_NORMAL = 0,
    EQUIPMENT_HELMET = 1,
    EQUIPMENT_SHIELD = 2,
    EQUIPMENT_HELMET_AND_SHIELD = 3,
    EQUIPMENT_SHIELD_AND_CAPE = 4,
    EQUIPMENT_HELMET_SHIELD_AND_CAPE = 5,
};
namespace tn_proc {
// HD: process names retain their GameCube spelling with WWHD IDs.
enum { fpcNm_BOKO_e = 0x1CF, fpcNm_BOMB_e = 0x126, fpcNm_HIMO2_e = 0x1BE };
} // namespace tn_proc
enum TnFigure {
    DSNAP_TYPE_TN_NO_SHIELD = 0xBF,
    DSNAP_TYPE_TN_SHIELD = 0xC0,
    DSNAP_TYPE_TN_MIGHTY = 0xC1,
};
// HD: colour bytes lie in the emitter tail, after both scale vectors.
struct TnEmitterColors : JPABaseEmitter {
    u8 _070[0x238 - 0x70];
    be<f32> mGlobalParticleWidthScale;
    u8 _23C[8];
    GXColor mGlobalPrmColor;
    GXColor mGlobalEnvColor;
};
WWHD_OFFSET(TnEmitterColors, mGlobalPrmColor, 0x244);
WWHD_OFFSET(TnEmitterColors, mGlobalEnvColor, 0x248);
WWHD_SIZE(TnEmitterColors, 0x24C);
// HD: retain guest wrapping for colour indices outside the six normal presets.
inline u8 tn_colorChannel(const TnColor6* table, u8 index, u32 channel) {
    return gabi::load<u8>(gabi::ea(table) + static_cast<u32>(index) * sizeof(GXColor) + channel);
}
void* s_w_sub(fopAc_ac_c*, void*);
void* s_b_sub(fopAc_ac_c*, void*);

// HD: deleting destructors are emitted independently of their owning classes.
static constexpr u32 TN_CUTTURN_VTABLE = 0x10052228;

// HD: this color conversion keeps the single-precision result of powf.
inline f32 tn_powf(f32 base, f32 exponent) {
    return gabi::call<f32>(0x028F4560, base, exponent);
}

WWHD_OFFSET(tn_class, mRangeOrFrozenAnim, 0x03D9);
WWHD_OFFSET(tn_class, mPathIndex, 0x03DA);
WWHD_OFFSET(tn_class, mEnableSpawnSwitch, 0x03DB);
WWHD_OFFSET(tn_class, mDisableSpawnOnDeathSwitch, 0x03DC);
WWHD_OFFSET(tn_class, mWaitSpawnSwitch, 0x03DD);
WWHD_OFFSET(tn_class, mPartsAttached, 0x03F4);
WWHD_OFFSET(tn_class, mPartDetachFlags, 0x03F5);
WWHD_OFFSET(tn_class, mFrameCount, 0x04F0);
WWHD_OFFSET(tn_class, mWeponRunTimer, 0x0502);
WWHD_OFFSET(tn_class, mWeponTurnWait, 0x0506);
WWHD_OFFSET(tn_class, mNoDamageTimer, 0x0508);
WWHD_OFFSET(tn_class, mNoGuardTimer, 0x050A);
WWHD_OFFSET(tn_class, mWeponSearchWait, 0x0514);
WWHD_OFFSET(tn_class, mObstacleSide, 0x0516);
WWHD_OFFSET(tn_class, mPlayerDistance, 0x0518);
WWHD_OFFSET(tn_class, mPlayerHeightDistance, 0x051C);
WWHD_OFFSET(tn_class, mPathXyz, 0x0520);
WWHD_OFFSET(tn_class, mHeadAngleY, 0x0530);
WWHD_OFFSET(tn_class, mTargetAngle, 0x0532);
WWHD_OFFSET(tn_class, mSideStepAngle, 0x0534);
WWHD_OFFSET(tn_class, mTubaEffectTimer, 0x0536);
WWHD_OFFSET(tn_class, mEffectAngle, 0x0544);
WWHD_OFFSET(tn_class, mSmokeAngle, 0x054A);
WWHD_OFFSET(tn_class, mGroundSmokeTimer, 0x054C);
WWHD_OFFSET(tn_class, mGroundSmokeType, 0x054E);
WWHD_OFFSET(tn_class, mWeponDropMode, 0x0D88);
WWHD_OFFSET(tn_class, mRightItemPos, 0x0D8C);
WWHD_OFFSET(tn_class, mKickLunge, 0x0DB7);
WWHD_OFFSET(tn_class, mRightItemFrontPos, 0x14E0);
WWHD_OFFSET(tn_class, mRightItemBackPos, 0x14EC);
WWHD_OFFSET(tn_class, mLeftArmPos, 0x14F8);
WWHD_OFFSET(tn_class, mRightArmPos, 0x1504);
WWHD_OFFSET(tn_class, mWepon2SphMove, 0x151C);
WWHD_OFFSET(tn_class, mPrevWeponSphMove, 0x1528);
WWHD_OFFSET(tn_class, mDefenceSphPos, 0x1534);
WWHD_OFFSET(tn_class, m13FC, 0x154C);
WWHD_OFFSET(tn_class, mItemStolen, 0x1550);
WWHD_OFFSET(tn_class, mArmorShakeTimer, 0x1552);
WWHD_OFFSET(tn_class, m1404, 0x1554);
WWHD_OFFSET(tn_class, mLookTargetId, 0x1558);
WWHD_OFFSET(tn_class, mWeponCarryPending, 0x155C);
WWHD_OFFSET(tn_class, mHitTypeFlags, 0x156C);
WWHD_OFFSET(tn_class, m1420, 0x1570);
WWHD_OFFSET(tn_class, mPathPoint, 0x1575);
WWHD_OFFSET(tn_class, mPathDir, 0x1576);
WWHD_OFFSET(tn_class, mMahiDelay, 0x1583);
WWHD_OFFSET(tn_class, mMoveTargetTimer, 0x1584);
WWHD_OFFSET(tn_class, mMoveTargetPos, 0x1588);
WWHD_OFFSET(tn_class, mDefenceSphOn, 0x1594);
WWHD_OFFSET(tn_class, mCutTurnPoints, 0x15CC);
WWHD_OFFSET(tn_class, mCutTurnStep, 0x1644);
WWHD_OFFSET(tn_class, mCutTurnTimer, 0x1646);
WWHD_OFFSET(tn_class, mDemoMode, 0x1648);
WWHD_OFFSET(tn_class, mDemoTimer, 0x164A);
WWHD_OFFSET(tn_class, mCameraEyePos, 0x164C);
WWHD_OFFSET(tn_class, mCenterPos, 0x1658);
WWHD_OFFSET(tn_class, mDemoCamFovy, 0x1664);
WWHD_OFFSET(tn_class, mDemoSide, 0x1669);
WWHD_OFFSET(tn_class, mReturnAnmMorf, 0x166C);
WWHD_OFFSET(tn_class, mStatusTimer, 0x1670);

void path_check(tn_class* i_this);

// HD: unit-private call boundaries retain the native register widths.
namespace tn_abi {
inline void enemy_piyo_set(fopAc_ac_c* actor) { gabi::call(0x02041CAC, actor); }
inline void cCcD_Stts_construct(void* object) { gabi::call(0x0200BD2C, object); }
inline void dCcD_GStts_construct(void* object) { gabi::call(0x02515DA0, object); }
inline void dCcD_Sph_construct(dCcD_Sph* object) { gabi::call(0x025166F0, object); }
inline void dCcD_Sph_destroy(void* object,s32 flags) { gabi::call(0x02515AE8, object, flags); }
inline void dCcD_Stts_destroy(void* object,s32 flags) { gabi::call(0x02515860, object, flags); }
inline void dBgS_Acch_destroy(void* object,s32 flags) { gabi::call(0x024EFD9C, object, flags); }
inline void cM3dGCir_destroy(void* object,s32 flags) { gabi::call(0x02018034, object, flags); }
inline void dCcD_Cyl_destroy(void* object,s32 flags) { gabi::call(0x02515A70, object, flags); }
inline void destroyArray(void* array,s32 count,s32 stride,void* destructor,s32 flags,s32 reserved) { gabi::call(0x028F0164, array, count, stride, destructor, flags, reserved); }
inline void fopAc_ac_c_destroy(fopAc_ac_c* object,s32 flags) { gabi::call(0x025D50BC, object, flags); }
inline void mDoHIO_deleteChild(s8 child) { gabi::call(0x025F0A18, child); }
inline void cutTurnEnd(void* callback) { gabi::call(0x025A9D64, callback); }
inline void mDoAud_subBgmStop() { gabi::call(0x025E1928); }
inline void enemy_fire_remove(void* fire) { gabi::call(0x02041C30, fire); }
inline void dCcD_GObjInf_construct(dCcD_GObjInf* object) { gabi::call(0x02515FB8, object); }
inline void cM3dGCyl_construct(void* object) { gabi::call(0x02018590, object); }
inline void smokeConstruct(void* callback,s32 follow) { gabi::call(0x025A5B18, callback, follow); }
inline void dBgS_AcchCir_construct(void* object) { gabi::call(0x024EFE94, object); }
inline void dBgS_Acch_construct(void* object) { gabi::call(0x024F0474, object); }
inline void constructArray(void* array,s32 count,s32 stride,void* constructor) { gabi::call(0x028EFFD0, array, count, stride, constructor); }
inline void dCcD_Stts_Init(dCcD_Stts* object,u32 weight,u32 flags,fopAc_ac_c* actor) { gabi::call(0x02515F14, object, weight, flags, actor); }
inline void saveOnSwitch(void* save,u32 switchNo,s32 room) { gabi::call(0x025B9E38, save, switchNo, room); }
inline void saveOnActor(void* save,u32 setId,s32 room) { gabi::call(0x025BA5D4, save, setId, room); }
inline BOOL saveIsSwitch(void* save,u32 switchNo,s32 room) { return gabi::call<BOOL>(0x025BA0C0, save, switchNo, room); }
inline BOOL saveIsEventBit(void* event,u32 flag) { return gabi::call<BOOL>(0x025B8B94, event, flag); }
inline void cBgS_GndChk_construct(void* check) { gabi::call(0x02008E0C, check); }
inline void cBgS_Chk_destroy(void* check,s32 flags) { gabi::call(0x02008DAC, check, flags); }
inline void registerGlobalObject(void* registration) { gabi::call(0x028F026C, registration); }
inline void initializeMathState(void* state) { gabi::call(0x028ED6F8, state); }
inline void initializeRandomState(void* state) { gabi::call(0x028EAB2C, state); }
inline void JntHit_HIO_construct(void* hio) { gabi::call(0x02552BE8, hio); }
inline void cMtx_YrotS(void* matrix,s32 angle) { gabi::call(0x025F1884, matrix, angle); }
inline void fopAcM_setCarryNow(void* actor,s32 carry) { gabi::call(0x025D9D0C, actor, carry); }
inline mDoExt_brkAnm* brkConstruct(mDoExt_brkAnm* object) { return gabi::call<mDoExt_brkAnm*>(0x025E80D0, object); }
inline BOOL brkInit(u32 animation,J3DModelData* model,void* resource,u32 enable,u32 loop,f32 speed,s32 start,s32 end,s32 material,s32 remap) { return gabi::call<BOOL>(0x025E8154, animation, model, resource, enable, loop, speed, start, end, material, remap); }
inline u32 JntHit_create(J3DModel* model,void* searchData,s32 count) { return gabi::call<u32>(0x02552B60, model, searchData, count); }
inline void MTXCopy(void* source,void* destination) { gabi::call(0x028E90D4, source, destination); }
inline void addAngles(csXyz* target,csXyz* addition) { gabi::call(0x0201A554, target, addition); }
inline void subtractPosition(cXyz* left,cXyz* result,cXyz* right) { gabi::call(0x0201ADE0, left, result, right); }
inline f32 VECSquareMag(cXyz* value) { return gabi::call<f32>(0x028E8DD0, value); }
inline void dBgS_LinChk_Set(tn_linecheck* check,cXyz* start,cXyz* end,fopAc_ac_c* actor) { gabi::call(0x024F1AFC, check, start, end, actor); }
inline BOOL LineCross(void* background,tn_linecheck* check) { return gabi::call<BOOL>(0x02008860, background, check); }
inline f32 GroundCross(void* background,tn_groundcheck* check) { return gabi::call<f32>(0x02008974, background, check); }
inline void cMtx_YrotM(void* matrix,s32 angle) { gabi::call(0x025F1C28, matrix, angle); }
inline void cMtx_XrotM(void* matrix,s32 angle) { gabi::call(0x025F1BF4, matrix, angle); }
inline void cMtx_ZrotM(void* matrix,s32 angle) { gabi::call(0x025F1C5C, matrix, angle); }
inline void MtxRotX(s32 concatenate,f32 angle) { gabi::call(0x0200FB3C, concatenate, angle); }
inline void cylinderSet(dCcD_Cyl* cylinder,void* source) { gabi::call(0x02516518, cylinder, source); }
inline void sphereSet(dCcD_Sph* sphere,void* source) { gabi::call(0x0251677C, sphere, source); }
inline u32 characterNameIndex(void* table,const char* name,s32 variant) { return gabi::call<u32>(0x0200E814, table, name, variant); }
inline BOOL entrySolidHeap(fopAc_ac_c* actor,u32 callback,u32 size) { return gabi::call<BOOL>(0x025D63E8, actor, callback, size); }
inline u8 mDoHIO_createChild(const char* name,void* hio) { return gabi::call<u8>(0x025F0A10, name, hio); }
inline void fopAcM_SetMin(fopAc_ac_c* actor,f32 x,f32 y,f32 z) { gabi::call(0x025D672C, actor, x, y, z); }
inline void fopAcM_SetMax(fopAc_ac_c* actor,f32 x,f32 y,f32 z) { gabi::call(0x025D673C, actor, x, y, z); }
inline tn_path* dPath_GetRoomPath(u32 index,s32 room) { return gabi::call<tn_path*>(0x025AAF88, index, room); }
inline void dBgS_Acch_Set(void* acch,cXyz* position,cXyz* oldPosition,fopAc_ac_c* actor,s32 mode,void* circle,cXyz* speed,s32 rotation,s32 shape) { gabi::call(0x024F06B4, acch, position, oldPosition, actor, mode, circle, speed, rotation, shape); }
inline void dBgS_AcchCir_SetWall(void* circle,f32 height,f32 radius) { gabi::call(0x024EFF44, circle, height, radius); }
inline s32 eventIndex(void* manager,const char* name,u32 mapTool) { return gabi::call<s32>(0x02543F10, manager, name, mapTool); }
inline BOOL eventExists(void* manager,s32 index) { return gabi::call<BOOL>(0x02544044, manager, index); }
inline s32 eventStaffId(void* manager,const char* name,s32 actor,s32 flags) { return gabi::call<s32>(0x02542D88, manager, name, actor, flags); }
inline u32 eventCutName(void* manager,s32 staff) { return gabi::call<u32>(0x02544830, manager, staff); }
inline void mDoAud_bgmAllMute(s32 duration) { gabi::call(0x025E1960, duration); }
inline void mDoAud_subBgmStart(u32 music) { gabi::call(0x025E1918, music); }
inline void vibrationStartShock(void* vibration,s32 power,s32 flags,cXyz* direction) { gabi::call(0x025CB374, vibration, power, flags, direction); }
inline void sphereSetCenter(void* sphere,cXyz* center) { gabi::call(0x02018D40, sphere, center); }
inline void sphereSetRadius(void* sphere,f32 radius) { gabi::call(0x02018C8C, sphere, radius); }
inline void setCollision(void* manager,void* object) { gabi::call(0x0200E240, manager, object); }
inline void setCollisionMass(void* manager,void* object,s32 mass) { gabi::call(0x02516C14, manager, object, mass); }
inline BOOL checkAttackHit(dCcD_Sph* sphere) { return gabi::call<BOOL>(0x025160DC, sphere); }
inline void cylinderSetRadius(void* cylinder,f32 radius) { gabi::call(0x020184DC, cylinder, radius); }
inline u32 fopAcM_otoCheck(fopAc_ac_c* actor,f32 distance) { return gabi::call<u32>(0x025D9DA0, actor, distance); }
inline u32 groundCheck(tn_class* actor,s32 count,s16 heading,f32 distance) { return gabi::call<u32>(0x024BA6D8, actor, count, heading, distance); }
inline u32 searchActors(u32 callback,tn_class* context) { return gabi::call<u32>(0x025DE508, callback, context); }
inline BOOL attentionLockon(void* attention) { return gabi::call<BOOL>(0x024EDFCC, attention); }
inline u32 attentionActionTarget(void* attention,s32 index) { return gabi::call<u32>(0x024EC8D0, attention, index); }
inline f32 padStickX(s32 pad) { return gabi::call<f32>(0x0200796C, pad); }
inline void morfPlay(mDoExt_McaMorf* morf,cXyz* position,u32 sound,u32 reverb) { gabi::call(0x025E535C, morf, position, sound, reverb); }
inline BOOL enemy_ice(tn_enemyice* ice) { return gabi::call<BOOL>(0x020402C8, ice); }
inline void morfCalc(void* morf) { gabi::call(0x025E55A0, morf); }
inline u32 dCam_getCamera(void* play) { return gabi::call<u32>(0x024F8020, play); }
inline u32 dCam_getBody(void* camera) { return gabi::call<u32>(0x024F8044, camera); }
inline void orderPotentialEvent(fopAc_ac_c* actor,u32 flags,u32 event,u32 priority) { gabi::call(0x025D7B24, actor, flags, event, priority); }
inline void cameraStop(void* camera) { gabi::call(0x02514F2C, camera); }
inline void cameraTrimSize(void* camera,s32 trim) { gabi::call(0x02515280, camera, trim); }
inline void cameraStart(void* camera) { gabi::call(0x02514F38, camera); }
inline void dMeter_mtrShow() { gabi::call(0x0259169C); }
inline void cameraSet(void* camera,cXyz* center,cXyz* eye,s32 roll,f32 fovy) { gabi::call(0x02514F88, camera, center, eye, roll, fovy); }
inline void smokeEnd(void* callback) { gabi::call(0x025A5F88, callback); }
inline void cylinderSetCenter(void* cylinder,cXyz* center) { gabi::call(0x020182E0, cylinder, center); }
inline void cancelCarry(void* actor) { gabi::call(0x025D9D24, actor); }
inline u32 environmentLight() { return gabi::call<u32>(0x02555D0C); }
inline void settingTevStruct(void* light,s32 type,cXyz* position,void* tev) { gabi::call(0x025626A4, light, type, position, tev); }
inline u32 damage_reaction(void* reaction) { return gabi::call<u32>(0x02041F94, reaction); }
inline void enemy_fire(void* fire) { gabi::call(0x02041570, fire); }
inline void collisionStatusMove(void* status) { gabi::call(0x02515E50, status); }
inline s32 checkTargetHit(void* object) { return gabi::call<s32>(0x025162A4, object); }
inline u32 getTargetHitObject(void* object) { return gabi::call<u32>(0x02516300, object); }
inline void def_se_set(fopAc_ac_c* actor,void* object,u32 sound) { gabi::call(0x02518CC8, actor, object, sound); }
inline void at_power_check(void* info) { gabi::call(0x02518DB0, info); }
inline u32 cc_at_check(fopAc_ac_c* actor,void* info) { return gabi::call<u32>(0x025192A8, actor, info); }
inline void dKy_Sound_set(cXyz* position,s32 volume,u32 actorId,s32 kind) { gabi::call(0x0255F458, position, volume, actorId, kind); }
inline void mDoAud_onEnemyDamage() { gabi::call(0x025E1FD8); }
inline void mDoAud_bgmHitSound(void* sound) { gabi::call(0x025E1D30, sound); }
inline s16 searchPlayerAngleY(fopAc_ac_c* actor,void* player) { return gabi::call<s16>(0x025D6894, actor, player); }
} // namespace tn_abi

static constexpr u32 TN_ACTOR_ID_JUDGE = 0x025E1234;
namespace tn_abi {
inline void monsterSound(u32 sound, cXyz* position, u32 actorId, u32 flags, s32 reverb) {
    gabi::call(0x025E1AA4, sound, position, actorId, flags, reverb);
}
inline void assertCondition(const char* file, s32 line, const char* expression) {
    gabi::call(0x0273AA24, file, line, expression);
}
inline s32 atan2s(f32 y, f32 x) { return gabi::call<s32>(0x020195B0, y, x); }
// HD: the unused actor receiver remains in r3 at this inlined lookup site.
inline void playLookupWithReceiver(tn_class* actor) { gabi::call(0x025200D4, actor); }
inline u32 particleSet(void* particle, u32 type, u32 effect, cXyz* position, csXyz* rotation,
                       cXyz* scale, u32 alpha, void* callback, s32 room, void* primary,
                       void* environment, void* extra) {
    return gabi::call<u32>(0x025A847C, particle, type, effect, position, rotation, scale,
                           alpha, callback, room, primary, environment, extra);
}
inline fopAc_ac_c* actorJudge(void* callback, be<u32>* id) {
    return gabi::call<fopAc_ac_c*>(0x025D5218, callback, id);
}
inline u32 actorJudgeAddress(u32 callback, be<u32>* id) {
    return gabi::call<u32>(0x025D5218, callback, id);
}
} // namespace tn_abi

// HD: these emitted virtual slots retain their 32-bit guest return values.
struct TnPlayerMethods {
    u8 _00[0x3C];
    be<u32> checkPlayerGuard;
    u8 _40[0x6C - 0x40];
    be<u32> checkCutCharge;
};
WWHD_OFFSET(TnPlayerMethods, checkPlayerGuard, 0x3C);
WWHD_OFFSET(TnPlayerMethods, checkCutCharge, 0x6C);
inline s32 tn_checkPlayerGuard(fopAc_ac_c* player) {
    const auto* methods = gabi::at<TnPlayerMethods>(player->__vtbl);
    return gabi::call<s32>(methods->checkPlayerGuard, player);
}
inline s32 tn_checkCutCharge(fopAc_ac_c* player) {
    const auto* methods = gabi::at<TnPlayerMethods>(player->__vtbl);
    return gabi::call<s32>(methods->checkCutCharge, player);
}
struct TnParticleMethods {
    u8 _00[0x44];
    be<u32> remove;
};
struct TnParticleCallback {
    gptr<TnParticleMethods> __vtbl;
    void remove() { gabi::call(__vtbl->remove, this); }
};
WWHD_OFFSET(TnParticleMethods, remove, 0x44);
struct TnSafeStringMethods {
    u8 _00[0x14];
    be<u32> assureTermination;
};
inline void tn_assureTermination(SafeString* string) {
    const auto* methods = gabi::at<TnSafeStringMethods>(string->__vtbl);
    gabi::call(methods->assureTermination, string);
}

// Only the player fields used by Darknut are exposed by this private view.
struct TnPlayer : fopAc_ac_c {
    /* 0x3AC */ be<u8> mCutType;
    u8 _3AD[0x3CC - 0x3AD];
    /* 0x3CC */ be<f32> mGrabItemOffsetY;  /* was field_0x2b0: view of daPy_py_c::mGrabItemOffsetY (d_a_player.h) */
};
WWHD_OFFSET(TnPlayer, mCutType, 0x3AC);
WWHD_OFFSET(TnPlayer, mGrabItemOffsetY, 0x3CC);
WWHD_SIZE(TnPlayer, 0x3D0);

// Embedded shape attribute vtables inside dCcD_Cyl::mShapeAttr.
struct TnShapeAttr {
    u8 _00[0x18];
    be<u32> mAabVtable;
    be<u32> mShapeVtable;
};
WWHD_OFFSET(TnShapeAttr, mAabVtable, 0x18);
WWHD_OFFSET(TnShapeAttr, mShapeVtable, 0x1C);
WWHD_SIZE(TnShapeAttr, 0x20);

// Private partial actor views avoid importing the mantle renderer's TU types.
struct TnMantle : fopAc_ac_c {
    u8 _3AC[0x362C - 0x3AC];
    /* 0x362C */ cXyz mShoulderPosR;
    /* 0x3638 */ cXyz mShoulderPosL;
    /* 0x3644 */ be<f32> mSegmentLen;
    /* 0x3648 */ be<f32> mSpeedRate;
    u8 _364C[0x365A - 0x364C];
    /* 0x365A */ be<s16> mWindTimer;
    u8 _365C[0x4280 - 0x365C];
    /* 0x4280 */ be<s8> mMeshHitOn;
    u8 _4281;
    /* 0x4282 */ be<s16> mHitWait;
    u8 _4284[4];
};
WWHD_SIZE(TnMantle, 0x4288);
struct TnBoko : fopAc_ac_c {
    u8 _3AC[8];
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    u8 _3B8[0x43A - 0x3B8];
    /* 0x43A */ be<s16> mRotAngleSpeed;  /* was m2C2: view of daBoko_c 0x43A (d_a_boko.h mRotAngleSpeed): updateHeldWeapon (in daTn_Execute 024BD170) sets the random spin cM_rndFX(2000) when the Darknut drops its weapon */
    u8 _43C[0xA34 - 0x43C];
};
WWHD_SIZE(TnBoko, 0xA34);
WWHD_OFFSET(TnMantle, mShoulderPosR, 0x362C);
WWHD_OFFSET(TnMantle, mShoulderPosL, 0x3638);
WWHD_OFFSET(TnMantle, mSegmentLen, 0x3644);
WWHD_OFFSET(TnMantle, mSpeedRate, 0x3648);
WWHD_OFFSET(TnMantle, mWindTimer, 0x365A);
WWHD_OFFSET(TnMantle, mMeshHitOn, 0x4280);
WWHD_OFFSET(TnMantle, mHitWait, 0x4282);
WWHD_OFFSET(TnBoko, mpModel, 0x3B4);
WWHD_OFFSET(TnBoko, mRotAngleSpeed, 0x43A);
WWHD_OFFSET(TnEmitterColors, mGlobalParticleWidthScale, 0x238);

struct TnSpinColor { be<u8> r, g, b, a; };
struct TnSpinColors { TnSpinColor m[6]; };
struct TnEffects3 { be<u16> m[3]; };
struct TnStaticStates { be<u32> m[3]; };
struct TnAngleRange { be<f32> min, max; };
struct TnGlobalObject { u8 data[12]; };
WWHD_SIZE(TnSpinColor, 4);
WWHD_SIZE(TnSpinColors, 24);
WWHD_SIZE(TnEffects3, 6);
WWHD_SIZE(TnStaticStates, 12);
WWHD_SIZE(TnAngleRange, 8);
WWHD_SIZE(TnGlobalObject, 12);

struct TnCameraView {
    u8 _00[0xDC];
    cXyz mEye;
    cXyz mCenter;
};
WWHD_OFFSET(TnCameraView, mEye, 0xDC);
WWHD_OFFSET(TnCameraView, mCenter, 0xE8);
WWHD_SIZE(TnCameraView, 0xF4);
struct TnAttention {
    u8 _00[0x20];
    be<u32> mFlags;
};
WWHD_OFFSET(TnAttention, mFlags, 0x20);

struct TnResourceIndices3 { be<u16> m[3]; };
struct TnJointIndices3 { be<u32> m[3]; };
struct TnFlameJoints { be<u8> m[10]; };
struct TnFlameScales { be<f32> m[10]; };
WWHD_SIZE(TnResourceIndices3, 6);
WWHD_SIZE(TnJointIndices3, 12);
WWHD_SIZE(TnFlameJoints, 10);
WWHD_SIZE(TnFlameScales, 40);

// Native entry declarations also serve named callback bindings.
void anm_init(tn_class* i_this, int bckFileIdx, f32 morf, s32 loopMode, f32 speed, int soundFileIdx);
void tate_anm_init(tn_class* i_this, int bckFileIdx, f32 morf, s32 loopMode, f32 speed);
void yoroi_anm_init(tn_class* i_this, int bckFileIdx, f32 morf, s32 loopMode, f32 speed);
BOOL daTn_IsDelete(tn_class* actor);
BOOL daTn_checkNearbyBomb(tn_class* i_this);
void tnSafeString_noop(void* string);
s16 get_view_H(tn_class* i_this);
void* s_w_sub(fopAc_ac_c* i_this, void* context);
void* s_b_sub(fopAc_ac_c* i_this, void* context);
void* shot_s_sub(fopAc_ac_c* i_this, void* context);
void wait_set(tn_class* i_this);
void fight_run_set(tn_class* i_this);
void d_mahi(tn_class* actor);
void tnSafeString_destroy(void* string, int flags);
void* tnCutTurn_construct(void* callback);
void tnCutTurn_destroy(void* callback, int flags);
f32 daTn_colorPower(u32 channel);
tn_enemyfire* tnEnemyFire_construct(tn_enemyfire* fire);
void tn_class_destroy(tn_class* actor, int flags);
BOOL daTn_Delete(tn_class* actor);
tn_class* tn_class_construct(tn_class* actor);
void part_draw(tn_class* i_this, int frozen);
BOOL daTn_player_way_check(tn_class* i_this);
BOOL daTn_Draw(tn_class* i_this);
void fail(tn_class* actor);
void aite_miru(tn_class* actor);
void defence(tn_class* actor);
void d_sit(tn_class* actor);
void d_dozou(tn_class* actor);
BOOL daTn_player_view_check(tn_class* i_this, cXyz* target, s16 heading, s16 angleLimit);
void p_lost(tn_class* actor);
BOOL nodeCallBack_kata(J3DNode* node, int calcTiming);
BOOL nodeCallBack_mimi(J3DNode* node, int calcTiming);
BOOL daTn_other_bg_check(tn_class* i_this, fopAc_ac_c* target);
BOOL daTn_player_bg_check(tn_class* i_this, cXyz* target);
void path_check(tn_class* i_this);
BOOL search_wepon(tn_class* i_this);
fopAc_ac_c* search_bomb(tn_class* i_this, int requireView);
void yogan_fail(tn_class* actor);
void smoke_set_s(tn_class* i_this, f32 scale);
BOOL nodeCallBack_P(J3DNode* node, int calcTiming);
BOOL nodeCallBack(J3DNode* node, int calcTiming);
u32 ground_4_check(tn_class* i_this, int count, s16 heading, f32 radius);
void way_pos_check(tn_class* i_this, cXyz* result);
void tn_staticInit();
void yoroi_break(tn_class* i_this, cXyz* position, int variant);
fopAc_ac_c* wepon_hit_check(tn_class* i_this);
void hukki(tn_class* actor);
void wepon_search(tn_class* actor);
void b_nige(tn_class* actor);
int useHeapInit(tn_class* actor);
void part_move(tn_class* actor, int index);
int daTn_Create(tn_class* actor);
void s_demo(tn_class* actor);
void Tn_move(tn_class* actor);
BOOL daTn_Execute(tn_class* actor);

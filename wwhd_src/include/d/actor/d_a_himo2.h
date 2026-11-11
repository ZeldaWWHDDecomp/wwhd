/* himo2_class (Grappling Hook rope), WWHD layout. 
 *
 * GameCube -> WWHD: fopAc_ac_c +0x11C up to mRopeLine (GameCube 0x1F30). mDoExt_3DlineMat1_c
 * grew from 0x3C to 0x188 (constructor 025EB82C), so mRopePointNum..mHandLine (GameCube
 * 0x1F6C..0x1FD8) shift and dCcD_Stts mStts (GameCube 0x2014) is at 0x2514. HD keeps only 100
 * entries of mCandidates (GameCube 0x218C, 200 entries). After mpHookModel HD inserts four
 * mDoExt_J3DModelPacketS (0xB0 each, constructor 02080404), so the members from mWrapOffsetZ
 * (GameCube 0x24B4) on are +0x630. Size 0x2D68.
 * Offsets measured from daHimo2_Create, CallbackCreateHeap, s_a_d_sub and himo2_bg_check. */
#pragma once
#include "gc_api/gc_api.h"

struct himo2_xyzWords_c {
    be<u32> x, y, z;
};
WWHD_SIZE(himo2_xyzWords_c, 0xC);
struct himo2_safeStringVtable_c {
    u8 _00[0x14];
    be<u32> assureTermination;
};
WWHD_OFFSET(himo2_safeStringVtable_c, assureTermination, 0x14);
WWHD_SIZE(himo2_safeStringVtable_c, 0x18);
struct himo2_playerVtable_c {
    u8 _00[0x14];
    be<u32> getLeftHandMatrix;
    u8 _18[4];
    be<u32> getRightHandMatrix;
    u8 _20[0x1C];
    be<u32> checkPlayerGuard;
    u8 _40[0x0C];
    be<u32> checkPlayerFly;
    u8 _50[0x7C];
    be<u32> checkPlayerNoDraw;
    u8 _D0[0x0C];
    be<u32> checkRopeReadyAnime;
};
WWHD_OFFSET(himo2_playerVtable_c, getLeftHandMatrix, 0x14);
WWHD_OFFSET(himo2_playerVtable_c, getRightHandMatrix, 0x1C);
WWHD_OFFSET(himo2_playerVtable_c, checkPlayerGuard, 0x3C);
WWHD_OFFSET(himo2_playerVtable_c, checkPlayerFly, 0x4C);
WWHD_OFFSET(himo2_playerVtable_c, checkPlayerNoDraw, 0xCC);
WWHD_OFFSET(himo2_playerVtable_c, checkRopeReadyAnime, 0xDC);
WWHD_SIZE(himo2_playerVtable_c, 0xE0);
inline himo2_playerVtable_c* himo2_playerVtable(fopAc_ac_c* player) {
    return gabi::at<himo2_playerVtable_c>(player->__vtbl);
}
struct himo2_attention_c {
    u8 _00[0x20];
    be<u32> mFlags;
};
WWHD_OFFSET(himo2_attention_c, mFlags, 0x20);
WWHD_SIZE(himo2_attention_c, 0x24);
// HD: the guide particle's draw-status word is later than in the GameCube emitter.
struct himo2_guideEmitter_c {
    u8 _00[0x254];
    be<u32> mStatus;
};
WWHD_OFFSET(himo2_guideEmitter_c, mStatus, 0x254);
WWHD_SIZE(himo2_guideEmitter_c, 0x258);
extern gptr<himo2_guideEmitter_c>& l_guideEmitter;
struct himo2_bk_c : fopAc_ac_c {
    u8 _3AC[0x4A0 - 0x3AC];
    struct {
        be<s16> mMode, mAction;
    } dr;
};
WWHD_OFFSET(himo2_bk_c, dr, 0x4A0);
WWHD_SIZE(himo2_bk_c, 0x4A4);

// HD: the rope impact effect also sets the emitter's volume sweep.
struct himo2_particle_c {
    u8 _00[0x34];
    /* 0x34 */ be<f32> mRate;
    u8 _38[0x20];
    /* 0x58 */ be<f32> mSpread;
    /* 0x5C */ be<s32> mMaxFrame;
    u8 _60[0x0C];
    /* 0x6C */ be<f32> mInitialVelAxis;
    u8 _70[0x0C];
    /* 0x7C */ be<f32> mVolumeSweep;
};
WWHD_OFFSET(himo2_particle_c, mRate, 0x34);
WWHD_OFFSET(himo2_particle_c, mSpread, 0x58);
WWHD_OFFSET(himo2_particle_c, mMaxFrame, 0x5C);
WWHD_OFFSET(himo2_particle_c, mInitialVelAxis, 0x6C);
WWHD_OFFSET(himo2_particle_c, mVolumeSweep, 0x7C);
WWHD_SIZE(himo2_particle_c, 0x80);

// HD: camera-process view used by rope targeting and the boss sequences.
struct himo2_camera_c {
    u8 _00[0xDC];
    /* 0xDC */ cXyz eye;
    u8 _E8[0x248 - 0xE8];
    /* 0x248 */ u8 mCamera[1];
};
WWHD_OFFSET(himo2_camera_c, eye, 0xDC);
WWHD_OFFSET(himo2_camera_c, mCamera, 0x248);
WWHD_SIZE(himo2_camera_c, 0x249);

// Only the tww boss members used by the rope sequences are exposed here.
struct himo2_dr_c : fopAc_ac_c {
    u8 _3AC[0x526 - 0x3AC];
    /* 0x526 */ be<u8> mTailState; /* was unk_40A: dr2_class mTailState (0x526); new_himo2_move 0216F7FC sets 3 (rock shake) / 0 in the boss-fall demo */
    u8 _527[9];
    /* 0x530 */ be<f32> mShakeAmplitude; /* was unk_414: dr2_class mShakeAmplitude (0x530); new_himo2_move 0216F7FC eases it to 2000 / 100 in the boss-fall demo */
    u8 _534[0x5D6 - 0x534];
    /* 0x5D6 */ be<s16> mRockState; /* was unk_4BA: dr2_class mRockState (0x5D6); new_himo2_move 0216F7FC tests >= 10 (rock broken) */
    u8 _5D8[0x0E];
    /* 0x5E6 */ be<s16> mRockFallTimer; /* was unk_4CA: dr2_class mRockFallTimer (0x5E6); new_himo2_move 0216F7FC sets 0x5A in the rope jump demo */
    u8 _5E8[0x40];
    /* 0x628 */ be<u8> mShowDragon; /* was unk_50C: dr2_class mShowDragon (0x628); new_himo2_move 0216F7FC sets 1 / 0 in the rope demo */
    u8 _629[3];
};
WWHD_OFFSET(himo2_dr_c, mTailState, 0x526);
WWHD_OFFSET(himo2_dr_c, mShakeAmplitude, 0x530);
WWHD_OFFSET(himo2_dr_c, mRockState, 0x5D6);
WWHD_OFFSET(himo2_dr_c, mRockFallTimer, 0x5E6);
WWHD_OFFSET(himo2_dr_c, mShowDragon, 0x628);
WWHD_SIZE(himo2_dr_c, 0x62C);
struct himo2_btd_c : fopAc_ac_c {
    u8 _3AC[0x63D4 - 0x3AC];
    /* 0x63D4 */ be<u8> mCrackStage;
    u8 _63D5[0x7059 - 0x63D5];
    /* 0x7059 */ be<u8> mBlurRate;
    u8 _705A[2];
};
WWHD_OFFSET(himo2_btd_c, mCrackStage, 0x63D4);
WWHD_OFFSET(himo2_btd_c, mBlurRate, 0x7059);
WWHD_SIZE(himo2_btd_c, 0x705C);

enum himo2_demo_state_e {
    HIMO2_DEMO_REQUEST = -1,
    HIMO2_DEMO_INACTIVE = 0,
    HIMO2_DEMO_COIL = 2,
    HIMO2_DEMO_SET_ROCK = 3,
    HIMO2_DEMO_DROP_ROCK = 4,
    HIMO2_DEMO_CONTROL = 5,
    HIMO2_DEMO_JUMP = 6,
    HIMO2_DEMO_RESET = 7,
    HIMO2_DEMO_BOSS_FALL = 8,
    HIMO2_DEMO_BOSS_END = 9,
};

enum himo2_state_e {
    HIMO2_WAIT = 0,
    HIMO2_SPIN = 1,
    HIMO2_LAUNCH_FREE = 2,
    HIMO2_LAUNCH_TARGET = 3,
    HIMO2_HOOK_BODY = 4,
    HIMO2_PULL_BACK = 5,
    HIMO2_MAX_LENGTH = 8,
    HIMO2_REBOUND = 9,
    HIMO2_HOOKED = 10,
    HIMO2_HANG = 11,
};

struct himo2_s {
    /* 0x00 */ u8 m00[0x0C];
    /* 0x0C */ be<f32> m0C;
    /* 0x10 */ cXyz mPos;  /* was m10: himo2_control / himo2_control2 (inlined in daHimo2_Execute 0216E194) move each rope segment from its neighbour's position; daHimo2_Execute 0216E194 takes the ground height below it; himo2_draw (in daHimo2_Draw 0216CA5C) puts the segment model there (tww the same) */
    /* 0x1C */ be<s16> mAngleX;  /* was m1C: himo2_control2 (in daHimo2_Execute 0216E194) stores the segment's pitch toward the next one; himo2_draw (in daHimo2_Draw 0216CA5C) turns the segment model by it about x (tww the same) */
    /* 0x1E */ be<s16> mAngleY;  /* was m1E: himo2_control2 (in daHimo2_Execute 0216E194) stores the segment's yaw toward the next one; himo2_draw (in daHimo2_Draw 0216CA5C) turns the segment model by it about y (tww the same) */
    /* 0x20 */ u8 m20[4];
};
WWHD_SIZE(himo2_s, 0x24);

// HD: rope line entries have a 0x10 stride; the shared line API uses 0x18.
struct himo2_line_c {
    gptr<cXyz> mpPositions;
    u8 _04[0x0C];
};
WWHD_SIZE(himo2_line_c, 0x10);
struct himo2_lineMat_c {
    u8 _00[0x130];
    /* 0x130 */ be<u32> __vtbl;
    u8 _134[0x50];
    /* 0x184 */ gptr<himo2_line_c> mpLines;
};
WWHD_OFFSET(himo2_lineMat_c, __vtbl, 0x130);
WWHD_OFFSET(himo2_lineMat_c, mpLines, 0x184);
WWHD_SIZE(himo2_lineMat_c, 0x188);

// HD: the added packet's lighting pointer is at 0xA4.
struct himo2_modelPacket_c {
    u8 _00[0xA4];
    /* 0xA4 */ gptr<dKy_tevstr_c> mpTevStr;
    u8 _A8[8];
};
WWHD_OFFSET(himo2_modelPacket_c, mpTevStr, 0xA4);
WWHD_SIZE(himo2_modelPacket_c, 0xB0);

struct himo2_class : fopAc_ac_c {
    BOOL setTargetPos(cXyz*, be<f32>*, be<f32>*);
    /* 0x03AC */ u8 _3AC[0x3B8 - 0x3AC];
    /* 0x03B8 */ be<s16> mDemoTimer;  /* was m029C: new_himo2_move 0216F7FC: counted down every frame; set 120/50/210/220/10 by the demo states (mDemoState SET_ROCK..BOSS_END), which act on frame numbers of it (30, 200, 170, 0) */
    /* 0x03BA */ be<s16> mHideTimer;  /* was m029E: daHimo2_Draw 0216CA5C draws nothing while it is not 0; new_himo2_move 0216F7FC: REG + 0x14 when the hook is back at the hand (states 8/9), 0 on lock-on and on throw; counted down */
    /* 0x03BC */ be<s16> mCancelWait;  /* was m02A0: new_himo2_move 0216F7FC: REG + 10 on throw, counted down; X/Y/Z cancels the flight (states 2/3) only while it is 0 */
    /* 0x03BE */ be<s16> mHitWait;  /* was m02A2: new_himo2_move 0216F7FC: 3 on throw, counted down; while it is not 0 himo2_bg_check 0216DE7C returns at once and At hits (ChkAtHit) are ignored */
    /* 0x03C0 */ be<s16> mControlEndWait;  /* was m02A4: new_himo2_move 0216F7FC: 20 when the swing-control demo (HIMO2_DEMO_CONTROL) starts; the demo ends on !checkPlayerFly() only once it is 0 */
    /* 0x03C2 */ u8 _3C2[0x3D0 - 0x3C2];
    /* 0x03D0 */ cXyz mRootPos;  /* was m02B4: daHimo2_Execute 0216E194: copy of mHandPos[0]; segments 0..mRootSegment of mSegments are set to it; himo2_draw (in daHimo2_Draw 0216CA5C) starts the rope line there */
    /* 0x03DC */ u8 _3DC[0x3E8 - 0x3DC];
    /* 0x03E8 */ be<s32> mRootSegment;  /* was m02CC: daHimo2_Execute 0216E194: segments 0..it of mSegments sit at mRootPos, himo2_control/himo2_control2 simulate from it + 1; new_himo2_move 0216F7FC sets 0x61 - distance * k (more rope out = lower), at least 2 */
    /* 0x03EC */ u8 _3EC[0x3F4 - 0x3EC];
    /* 0x03F4 */ be<u32> mFrameCounter;  /* was m02D8: daHimo2_Execute 0216E194: + 1 every frame; new_himo2_move 0216F7FC / himo2_control2 use it as the phase of the spin, sway and camera shake (it * 0x2000 etc.) */
    /* 0x03F8 */ be<s32> mState;  /* was m02DC: new_himo2_move 0216F7FC: switch over the rope states (himo2_state_e HIMO2_WAIT .. HIMO2_HANG); himo2_bg_check 0216DE7C sets HIMO2_REBOUND; daPy_lk_c::checkNextActionRopeReady 023EC084 reads it (view player02::ropeMode_c) */
    /* 0x03FC */ be<s8> mRockfallSeStep;  /* was m02E0: new_himo2_move 0216F7FC (HIMO2_DEMO_CONTROL): 0/1/2+ selects JA_SE_CV_DRG_ROCKFALL_1 / _1_2 / _1_3 every 50 frames, + 1 each time; 0 when the rock is dropped */
    /* 0x03FD */ u8 _3FD[3];
    /* 0x0400 */ be<f32> mSagY;  /* was m02E4: himo2_control (in daHimo2_Execute 0216E194) adds it to each segment's height difference (the rope's droop); new_himo2_move 0216F7FC: 0 on throw, -5 when wrapped round a body, -> -6.25 while hooked */
    /* 0x0404 */ u8 _404[4];
    /* 0x0408 */ cXyz mHandPos[2];  /* was m02EC: daHimo2_Execute 0216E194: [0]/[1] = MtxPosition(4, 0, 0) of the left/right hand matrices, [0] the hand holding the rope (getRopeGrabRightHand); new_himo2_move 0216F7FC pulls the hook back to [0]; daHimo2_Draw 0216CA5C draws the coils at [1] */
    /* 0x0420 */ u8 _420[4];
    /* 0x0424 */ be<s16> mStateTimer;  /* was m0308: new_himo2_move 0216F7FC: counted down; set on each state change (throw, rebound 30/40, max length 50); at 0 the spin throws (state 1), the flight stops at max length (2) or pulls back (3); himo2_bg_check 0216DE7C sets 30 */
    /* 0x0426 */ u8 _426[0x42C - 0x426];
    /* 0x042C */ himo2_s mSegments[100];  /* was m0310: daHimo2_Execute 0216E194: the 100 rope segments (himo2_s), positions set from mRootPos, simulated by himo2_control/himo2_control2, [99] = the hook position; himo2_draw (in daHimo2_Draw 0216CA5C) draws them */
    /* 0x123C */ himo2_s m1120[100];
    /* 0x204C */ himo2_lineMat_c mRopeLine;  /* was m1F30: CallbackCreateHeap 0216E9E0: mDoExt_3DlineMat1_c init(1, 200, rope texture); daHimo2_Draw 0216CA5C fills it with the rope points (spin_draw, himo2_draw, himo_hang_draw) and updates it */
    /* 0x21D4 */ be<s32> mRopePointNum;  /* was m1F6C: daHimo2_Draw 0216CA5C: 0 at the start, + 1 per point written into mRopeLine (spin_draw / himo2_draw / himo_hang_draw); the line is updated with it as the point count (nothing above 200) */
    /* 0x21D8 */ be<f32> mCoilRadiusRate[5];  /* was m1F70: daHimo2_Draw 0216CA5C: per coil i (5), -> 0 while mHandDist * (REG + 3) + REG - 120 is above (4 - i) * 300, else -> 1; the coil radius is it * 15, and [0] scales the line between the hands */
    /* 0x21EC */ cXyz mOldCoilHandPos;  /* was m1F84: daHimo2_Draw 0216CA5C: mHandPos[1] of the previous frame; the movement mHandPos[1] - it sets the coil swing angles */
    /* 0x21F8 */ be<s16> mCoilAngleX;  /* was m1F90: daHimo2_Draw 0216CA5C: follows the hand movement (z) into the coils' X rotation (cLib_addCalcAngleS2, then mDoMtx_XrotM) */
    /* 0x21FA */ be<s16> mCoilFlipAngleZ;  /* was m1F92: daHimo2_Draw 0216CA5C: -> -0x8000 when the hand moves down (below REG - 20), else -> 0; added to the coils' Z rotation */
    /* 0x21FC */ be<s16> mCoilAngleZ;  /* was m1F94: daHimo2_Draw 0216CA5C: follows the hand movement (x) and is the per-coil step of the coils' Z rotation ((it - 2000) * i) */
    /* 0x21FE */ u8 _21FE[2];
    /* 0x2200 */ himo2_lineMat_c mCoilLine;  /* was m1F98: CallbackCreateHeap 0216E9E0: mDoExt_3DlineMat1_c init(5, 0x20, rope texture); daHimo2_Draw 0216CA5C fills 5 circles of 32 points round mHandPos[1] (the coiled rope in the hand) */
    /* 0x2388 */ be<f32> mHandLineSag;  /* was m1FD4: daHimo2_Draw 0216CA5C: -> REG + 15 or 0 by the bits of the rope distance; lowers/widens the sine curve of mHandLine */
    /* 0x238C */ himo2_lineMat_c mHandLine;  /* was m1FD8: CallbackCreateHeap 0216E9E0: mDoExt_3DlineMat1_c init(1, 0x10, rope texture); daHimo2_Draw 0216CA5C fills 16 points from mHandPos[1] to mHandPos[0] (the rope between the hands) */
    /* 0x2514 */ dCcD_Stts mStts;  /* was m2014: daHimo2_Create 0216EC18: dCcD_Stts, Init(0xFF, 0xFF, this), the status of mSph */
    /* 0x2550 */ dCcD_Sph mSph;  /* was m2050: daHimo2_Create 0216EC18: dCcD_Sph set from cc_sph_src; daHimo2_Execute 0216E194 places it at the hook (R 20 in flight, else -100); new_himo2_move 0216F7FC tests ChkAtHit */
    /* 0x267C */ gptr<fopAc_ac_c> mpTarget;  /* was m217C: himo2_class::setTargetPos 0216EEB8 stores the target found by search_target; new_himo2_move 0216F7FC: the lock-on target on throw, the hooked actor (kui / body) whose health it sets */
    /* 0x2680 */ be<u32> mTargetId;  /* was m2180: new_himo2_move 0216F7FC: fopAcM_GetID(the lock-on target) on throw (else -1); states 3/4 look the actor up with fopAcM_SearchByID(it) */
    /* 0x2684 */ be<f32> mWrapRadius;  /* was m2184: new_himo2_move 0216F7FC (state 4, hooked on a body): the radius of the hook circling the target's eyePos (rotated by mFrameCounter << 13), -> 0; below 0.1 the hook is done */
    /* 0x2688 */ be<f32> mHandDist;  /* was m2188: new_himo2_move 0216F7FC: |current.pos - mHandPos[0]| every frame; sets mRootSegment and the segment length in himo2_control/himo2_control2 (in daHimo2_Execute 0216E194) */
    /* 0x268C */ gptr<fopAc_ac_c> mCandidates[100];  /* was m218C: s_a_d_sub 0216DD3C appends each hook target found by fpcM_Search; search_target (in setTargetPos 0216EEB8) clears and walks them; HD: 100 (GameCube 200) */
    /* 0x281C */ be<u8> mCandidateNum;  /* was m24AC: s_a_d_sub 0216DD3C: index of the next free mCandidates entry (assert < 100), + 1 per candidate; search_target (in setTargetPos 0216EEB8) clears it */
    /* 0x281D */ u8 _281D[3];
    /* 0x2820 */ gptr<J3DModel> mpHookModel;
    /* 0x2824 */ himo2_modelPacket_c mPackets[4]; /* HD: mDoExt_J3DModelPacketS x4 */
    /* 0x2AE4 */ be<f32> mWrapOffsetZ;  /* was m24B4: new_himo2_move 0216F7FC: REG - 400 when the wrap demo starts, -> -144 (HIMO2_DEMO_COIL); spin_draw (in daHimo2_Draw 0216CA5C): z of the wrap's start point relative to the target */
    /* 0x2AE8 */ be<f32> mWrapOffsetX;  /* was m24B8: new_himo2_move 0216F7FC: REG - 90 (body) or 0 when the wrap demo starts, -> 0 (HIMO2_DEMO_COIL); spin_draw (in daHimo2_Draw 0216CA5C): x of the wrap's start point relative to the target */
    /* 0x2AEC */ be<s32> mWrapCount;  /* was m24BC: new_himo2_move 0216F7FC (HIMO2_DEMO_COIL): wrap progress, + (mWrapSpeed / size) + 1 per frame up to 400, coil sounds at 0/13/26/39, hook sound past 49, wrap done past 0x82; spin_draw (in daHimo2_Draw 0216CA5C) bends segments up to it + 49 */
    /* 0x2AF0 */ be<s32> mOldWrapCount;  /* was m24C0: new_himo2_move 0216F7FC (HIMO2_DEMO_COIL): mWrapCount before this frame's step */
    /* 0x2AF4 */ be<f32> mWrapSpeed;  /* was m24C4: new_himo2_move 0216F7FC (HIMO2_DEMO_COIL): 0 at the start, + REG + 2 per frame; mWrapCount advances by it / (target size) + 1 */
    /* 0x2AF8 */ be<s16> mWrapAngleStep;  /* was m24C8: new_himo2_move 0216F7FC (HIMO2_DEMO_COIL): -> 4000 - 2000 * (target scale.y - 1); spin_draw (in daHimo2_Draw 0216CA5C) adds it per wrapped segment to the bend angle */
    /* 0x2AFA */ be<s16> mHookAngleX;  /* was m24CA: new_himo2_move 0216F7FC: 0 when the wrap starts, 9000 (with the hook sound) then 0x238C; spin_draw (in daHimo2_Draw 0216CA5C): X rotation of the hook model at the rope end */
    /* 0x2AFC */ cXyz mHookTipPos;  /* was m24CC: spin_draw / himo2_draw (in daHimo2_Draw 0216CA5C): MtxPosition((0, 0, 15)) in the hook model's matrix, the hook's tip */
    /* 0x2B08 */ be<s8> mWrapDrawMode;  /* was m24D8: new_himo2_move 0216F7FC: 1 when hooked, 2 when the wrap is done, 0 on release; daHimo2_Draw 0216CA5C draws the wrap (spin_draw) when not 0, below 2 also its first 50 points */
    /* 0x2B09 */ be<s8> mDemoState;  /* was m24D9: new_himo2_move 0216F7FC: switch over the hook demos (himo2_demo_state_e HIMO2_DEMO_REQUEST .. BOSS_END); daHimo2_Draw 0216CA5C draws nothing while below HIMO2_DEMO_INACTIVE */
    /* 0x2B0A */ u8 _2B0A[2];
    /* 0x2B0C */ cXyz mCamEye;  /* was m24DC: new_himo2_move 0216F7FC: the demo camera's eye (cameraSet / cameraReset e argument, with the shake), moved by the demo states */
    /* 0x2B18 */ cXyz mCamCenter;  /* was m24E8: new_himo2_move 0216F7FC: the demo camera's center (cameraSet / cameraReset c argument, with the shake): the target, the player or the boss */
    /* 0x2B24 */ be<f32> mCamFovy;  /* was m24F4: new_himo2_move 0216F7FC: the f32 (fovy) argument of cameraSet 02514FE8 in the demos; 65 at the control demo, -> mCamFovyTarget */
    /* 0x2B28 */ be<f32> mCamFovyTarget;  /* was m24F8: new_himo2_move 0216F7FC: 65 at the control demo, moved by the sub stick Y (REG + 20 .. REG + 70); mCamFovy follows it */
    /* 0x2B2C */ be<f32> mCamEyeDist;  /* was m24FC: new_himo2_move 0216F7FC (HIMO2_DEMO_COIL): scale of the eye offset added to mCamEye each frame; REG + 3.5 (body) / REG + 3 at the start, -> REG + 1.8 / REG + 1.2 */
    /* 0x2B30 */ be<f32> mReturnSpeedRate;  /* was m2500: new_himo2_move 0216F7FC: scales the speed the hook returns to the hand (200 * it in HIMO2_WAIT, (REG + 400) * it in mv_pull_back), -> 1; 0 on throw, 0.1/0.02 on a hit / time out; daHimo2_Create 0216EC18 sets 1 */
    /* 0x2B34 */ cXyz mHangPos;  /* was m2504: spin_draw (in daHimo2_Draw 0216CA5C): the wrap point REG + 48 (where the rope leaves the target); new_himo2_move 0216F7FC puts the hook there in HIMO2_HOOKED/HANG; himo_hang_draw draws the rope from it to mHandPos[0] */
    /* 0x2B40 */ be<s16> mCamAngleY;  /* was m2510: new_himo2_move 0216F7FC: target angle +-0x4000 at the wrap demo, player angle + 0x8000 at the control demo, -> mCamAngleYTarget; YrotS for the camera eye offset; spin_draw (in daHimo2_Draw 0216CA5C) uses it as the wrap's Y angle */
    /* 0x2B42 */ be<s16> mCamAngleYTarget;  /* was m2512: new_himo2_move 0216F7FC (control demo): set with mCamAngleY, + sub stick X * (REG + 1000) per frame; mCamAngleY follows it */
    /* 0x2B44 */ be<f32> mCamEyeSignX;  /* was m2514: new_himo2_move 0216F7FC: -1 at the wrap demo; multiplies the x of the eye offset (HIMO2_DEMO_COIL) */
    /* 0x2B48 */ be<f32> mCamEyeSignZ;  /* was m2518: new_himo2_move 0216F7FC: REG + 1 (body) or -1 at the wrap demo; multiplies the z of the eye offset (HIMO2_DEMO_COIL) */
    /* 0x2B4C */ be<s8> mWrapMirror;  /* was m251C: search_target (in setTargetPos 0216EEB8): 1 when the player is on the negative-angle side of a plain kui, else 0; new_himo2_move 0216F7FC: mCamAngleY = target angle - 0x4000 when set, + 0x4000 when not */
    /* 0x2B4D */ u8 _2B4D[3];
    /* 0x2B50 */ be<f32> mCamShake;  /* was m2520: new_himo2_move 0216F7FC: REG + 50 at the boss-fall demo, -> 0; sin/cos(mFrameCounter) * it shakes mCamEye/mCamCenter, cos * it * 7.5 is the camera bank */
    /* 0x2B54 */ cXyz mTargetPos;  /* was m2524: himo2_class::setTargetPos 0216EEB8 stores its cXyz argument; new_himo2_move 0216F7FC: the lock-on target's eyePos, limited to REG * 100 + 1000 from the camera eye; the free throw flies to it */
    /* 0x2B60 */ be<f32> mSwayAmp;  /* was m2530: new_himo2_move 0216F7FC: REG + 4 on throw; himo2_control2 (in daHimo2_Execute 0216E194): sway amplitude it * (99 - i) * 0.01 of the flying rope (states 1-3) */
    /* 0x2B64 */ dBgS_AcchCir mAcchCir;  /* was m2534: daHimo2_Create 0216EC18: dBgS_AcchCir, SetWall(30, 30), the circle of mAcch */
    /* 0x2BA4 */ dBgS_ObjAcch mAcch;  /* was m2574: daHimo2_Create 0216EC18: dBgS_ObjAcch set on current/old pos with mAcchCir; himo2_bg_check 0216DE7C CrrPos and the wall/roof/ground hit */
};
WWHD_OFFSET(himo2_class, mRootSegment, 0x3E8);
WWHD_OFFSET(himo2_class, mSegments, 0x42C);
WWHD_OFFSET(himo2_class, mRopeLine, 0x204C);
WWHD_OFFSET(himo2_class, mCoilLine, 0x2200);
WWHD_OFFSET(himo2_class, mHandLine, 0x238C);
WWHD_OFFSET(himo2_class, mStts, 0x2514);
WWHD_OFFSET(himo2_class, mSph, 0x2550);
WWHD_OFFSET(himo2_class, mCandidates, 0x268C);
WWHD_OFFSET(himo2_class, mCandidateNum, 0x281C);
WWHD_OFFSET(himo2_class, mpHookModel, 0x2820);
WWHD_OFFSET(himo2_class, mWrapOffsetZ, 0x2AE4);
WWHD_OFFSET(himo2_class, mReturnSpeedRate, 0x2B30);
WWHD_OFFSET(himo2_class, mAcchCir, 0x2B64);
WWHD_OFFSET(himo2_class, mAcch, 0x2BA4);
WWHD_SIZE(himo2_class, 0x2D68);

/* himo2HIO_c (HD: vtable after the members), l_himo2HIO at 0x1046469C */
struct himo2HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01;
    /* 0x02 */ be<s16> mSwingDirMode;  /* was m06: read only by tww: daHimo2_Execute's switch picks the swing directions (0 random, 1..4 the four fixed sign pairs); WWHD writes it only in __sinit_d_a_himo2_cpp 0216F6D8 (0) and new_himo2_move always takes (-1, -1) (2026-11-11 rule: tww's one role) */
    /* 0x04 */ be<s16> mTargetAngleRange;  /* was m08: setTargetPos 0216EEB8 (inline search_target): a target counts only while its angle to Link lies within 0x4000 +- it (plus a debug register); HIO 15000 (tww the same) */
    /* 0x06 */ be<s16> m0A;
    /* 0x08 */ be<s16> mPitchTolBelow;  /* was m0C: setTargetPos 0216EEB8: a normal target is taken only while its pitch from the camera eye exceeds the aim point's by less than it (2000 for flagged targets); HIO 600 (tww the same) */
    /* 0x0A */ be<s16> mPitchTolAbove;  /* was m0E: setTargetPos 0216EEB8: a target is taken only while the aim point's pitch exceeds the target's by less than it; HIO 400 (tww the same) */
    /* 0x0C */ be<f32> mTargetRadius;  /* was m10: setTargetPos 0216EEB8: the yaw tolerance is cM_atan2s(target scale.z * it, distance from the camera eye), the target's radius per unit of scale; HIO 60 (tww the same) */
    /* 0x10 */ be<f32> mTargetMaxDY;  /* was m14: setTargetPos 0216EEB8: a normal target is taken only while its height offset in Link's frame is below it (plus a stage tolerance on ADMumi); HIO 1500 (tww the same) */
    /* 0x14 */ be<f32> mSearchDistMax;  /* was m18: setTargetPos 0216EEB8: the search distance grows by 100 per pass and stops above it (2000 for flagged targets); HIO 1000 (tww the same) */
    /* 0x18 */ be<f32> mRopeScaleLockOn;  /* was m1C: daHimo2_Execute 0216E194 and new_himo2_move 0216F7FC (while Lockon) store it in rope_scale, the rope lines' width for lineMat_update; HIO 2.3 (tww the same) */
    /* 0x1C */ be<f32> mRopeScale;  /* was m20: new_himo2_move 0216F7FC stores it in rope_scale while nothing is locked on; HIO 1.9 (tww the same) */
    /* 0x20 */ be<u32> __vtbl;
};
WWHD_SIZE(himo2HIO_c, 0x24);
extern himo2HIO_c& l_himo2HIO;
extern be<f32>& rope_scale;
extern gptr<himo2_btd_c>& himo2_btd;
extern gptr<himo2_dr_c>& himo2_dr;
// HD: the debug-register root's children start at 4; each child keeps the tww layout.
struct himo2_reg_childHIO_c {
    be<u32> __vtbl;
    be<f32> mFloatRegs[30];
    be<s16> mShortRegs[10];
};
WWHD_OFFSET(himo2_reg_childHIO_c, mFloatRegs, 4);
WWHD_OFFSET(himo2_reg_childHIO_c, mShortRegs, 0x7C);
WWHD_SIZE(himo2_reg_childHIO_c, 0x90);
struct himo2_reg_HIO_c {
    u8 _00[4];
    himo2_reg_childHIO_c mChild[22];
};
WWHD_OFFSET(himo2_reg_HIO_c, mChild, 4);
WWHD_SIZE(himo2_reg_HIO_c, 0xC64);
extern himo2_reg_HIO_c& l_himo2Regs;

#define HIMO2_VTBL 0x10010EF4
#define HIMO2_SAFESTRING_VTBL 0x10010DEC

// HD: unit-local ABI bridges preserve full-word collision results and stack-copy arguments.
u32 himo2_GetOnePolyInfo(dBgS_Acch*, cBgS_PolyInfo*);
s32 himo2_GetAttributeCode(dBgS*, cBgS_PolyInfo*);
void himo2_SoundSet(cXyz*, s32, u32, s32);
void himo2_constructSphere(dCcD_Sph*);
void himo2_setupPacket(void*, s32);
void himo2_moveExternal(himo2_class*);
void himo2_destroyGroundCheck(dBgS_GndChk*, s32);
void himo2_setLinePacket(daPy_py_c*, himo2_modelPacket_c*, himo2_lineMat_c*);
void himo2_setHookPacket(daPy_py_c*, himo2_modelPacket_c*, J3DModel*);

u32 himo2_padTrigX(u32);
u32 himo2_padTrigY(u32);
u32 himo2_padTrigZ(u32);
f32 himo2_subStickY(u32);
f32 himo2_subStickX(u32);
void himo2_cameraStop(void*);
void himo2_cameraStart(void*);
void himo2_cameraTrim(void*, s32);
void himo2_cameraReset(void*, cXyz*, cXyz*);
void himo2_cameraSet(void*, cXyz*, cXyz*, s16, f32);
void himo2_customColorSet(u32, u32, f32);
void himo2_orderPotentialEvent(fopAc_ac_c*, u16, u16, u32);
void himo2_playerPosExternal(himo2_class*);
BOOL himo2_bgCheckExternal(himo2_class*);
u32 himo2_LockonTruth(u32);
u32 himo2_LockonTarget(u32, s32);
void himo2_destroyLineCheck(dBgS_LinChk*, s32);

void himo2_constructPacket(himo2_modelPacket_c*);
void himo2_destroyPacket(himo2_modelPacket_c*, s32);
void himo2_destroySphere(dCcD_Sph*, s32);

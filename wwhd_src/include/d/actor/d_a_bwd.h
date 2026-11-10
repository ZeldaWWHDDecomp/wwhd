/* bwd_class (Molgera), WWHD layout. 
 *
 * GameCube -> WWHD: fopEn_enemy_c is +0x11C; the members up to mBodyAnmSpeed stay +0x11C. HD adds an
 * array of 20 floats after mBodyAnmSpeed (m03C4b, per body segment, written by end()), so mBodyAnmStartNo..mBgmStartTimer
 * are +0x16C. The tevstr mPlatformTevStr grows by 0x118 (dKy_tevstr_c 0x1C8), so mSandWaveMode..mSandCrossWait are +0x284.
 * HD has no m18F0 (the cXyz between mItemPos and mSandCrossWait): mSandCrossWait and everything after it is +0x278.
 * Size 0x3ECC (constructor 020FDE40). Offsets from the verified functions. */
#pragma once
#include "bindings.h"

/* dPa_smokeEcallBack (HD 0x20): vtable at +0, colour (GXColor) at +0x16 */
struct dPa_smokeEcallBack_bd {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 _04[0x16 - 0x04];
    /* 0x16 */ be<u8> mColor[4];
    /* 0x1A */ u8 _1A[0x20 - 0x1A];
};

struct sita_s {
    /* 0x00 */ gptr<J3DModel> m00;
    /* 0x04 */ cXyz m04;
    /* 0x10 */ u8 m10[4];
};

struct bwd_class : fopEn_enemy_c {
    /* 0x03C8 */ request_of_phase_process_class mPhaseBwd;
    /* 0x03D0 */ request_of_phase_process_class mPhaseBwds;
    /* 0x03D8 */ be<u8> m02BC;
    /* 0x03D9 */ u8 _3D9[3];
    /* 0x03DC */ gptr<mDoExt_McaMorf> mpHeadMorf;
    /* 0x03E0 */ gptr<mDoExt_brkAnm> mpHeadBrkAnm;
    /* 0x03E4 */ be<f32> mHeadBrkSpeed;  /* was m02C8: end 020FEA30 sets 1.0; bwd_execute 020FA614 passes it to mpHeadBrkAnm->setPlaySpeed */
    /* 0x03E8 */ gptr<J3DModel> m02CC;
    /* 0x03EC */ u8 _3EC[4];
    /* 0x03F0 */ gptr<J3DModel> mpBodyModel[20];
    /* 0x0440 */ gptr<J3DModel> m0324[20];
    /* 0x0490 */ gptr<mDoExt_brkAnm> mpBodyMorf[20];
    /* 0x04E0 */ be<f32> mBodyAnmSpeed[20];  /* was m03C4: bwd_execute 020FA614 passes [i] to mpBodyMorf[i]->setPlaySpeed (and skips even segments below 0.1); end 020FEA30 sets them to 1 one by one (mBodyAnmStartNo) and tests >= 0.5 */
    /* 0x0530 */ be<f32> m03C4b[20]; /* HD */
    /* 0x0580 */ be<s32> mBodyAnmStartNo;  /* was m0414: end 020FEA30: every 4th frame (while mTimers[0] is 0) sets mBodyAnmSpeed[it] = 1 and adds 1, up to 20 */
    /* 0x0584 */ cXyz mBodyPos[20];  /* was m0418: bwd_execute 020FA614: MtxPosition per body segment (sand particle 0x8241 there); end 020FEA30 bursts segment [mBodyExplodeNo] (SIBOUBAKUEN / SIBOUSUNA, MONS_EXPLODE); demo_camera 02101F30 eases its centre to it; daBwd_Delete 020FD578 deletes their sound objects */
    /* 0x0674 */ cXyz mPosHistory[256];  /* was m0508: historyPosition() (d_a_bwd_execute.h) indexes it as a 256-entry ring; end 020FEA30 fills it with current.pos */
    /* 0x1274 */ csXyz mAngleHistory[256];  /* was m1108: historyAngles() (d_a_bwd_execute.h) indexes it as a 256-entry ring; bwd_execute 020FA614 stores current.angle at [mHistoryIndex] */
    /* 0x1874 */ be<s32> mHistoryIndex;  /* was m1708: bwd_execute 020FA614: ring index for historyAngles / historyPosition; the body segments read entry mHistoryIndex - (i + 1) * mHistoryStep */
    /* 0x1878 */ be<s32> mDrawBody;  /* was m170C: end 020FEA30 sets 1; daBwd_Draw 020F9C1C draws the body segment models only while it is set; bwd_execute 020FA614 clears it every frame */
    /* 0x187C */ be<s8> mBgmStartTimer;  /* was m1710: sita_hit (in move 020FF80C) sets 10 when it is 0; bwd_execute 020FA614 counts it down to 1 and, once no larva is left (ko_count 0), starts the battle BGM and clears it */
    /* 0x187D */ u8 _187D[3];
    /* 0x1880 */ dKy_tevstr_c mPlatformTevStr;  /* was m1714: daBwd_Draw 020F9C1C: settingTevStruct(TEV_TYPE_BG0, ...) into it, then setLightTevColorType for mpTriforcePlatformModel and mpSandfallModel[i] */
    /* 0x1A48 */ be<u8> mSandWaveMode;  /* was m17C4: daBwd_Execute 020FA614 eases the sand-wave terms by it: 0xFF rise, 200 calm, 0xC9 / 0xCA sink, any other value counts down to 0 (then they ease to 0); start / wait / sita_hit / eat_attack / fly / s_fly (in move 020FF80C), demo_camera 02101F30 and fly_pos_move 020F9F80 set it */
    /* 0x1A49 */ u8 _1A49[3];
    /* 0x1A4C */ cXyz mSandDentCenter;  /* was m17C8: wait / eat_attack (in move 020FF80C) copy current.pos, fly / s_fly copy mSandSurfacePos; demo_camera 02101F30 sets x / z 0; wave_cont 02103FA4 (bwdg) measures every vertex's distance from it; daBwd_Execute 020FA614 measures the player's */
    /* 0x1A58 */ be<f32> mSandDentDepth;  /* was m17D4: daBwd_Execute 020FA614 eases it per mSandWaveMode (0.35, -0.3, -0.7, -0.75; else 0); wave_cont 02103FA4 (bwdg) adds ((3000 - distance) * 0.01)^2 * it to the vertex heights around mSandDentCenter */
    /* 0x1A5C */ be<s16> mWaveSpeed;  /* was m17D8: daBwd_Execute 020FA614 eases it per mSandWaveMode (-0x5DC, 0, -500, 1000; else 0); start (in move 020FF80C) sets 500; wave_cont 02103FA4 (bwdg) adds it to mWavePhase each frame */
    /* 0x1A5E */ u8 _1A5E[2];
    /* 0x1A60 */ be<f32> mWaveStepRate;  /* was m17DC: daBwd_Execute 020FA614 eases it to 1 (to 0 when mSandWaveMode is 0); wave_cont 02103FA4 (bwdg) multiplies the cLib_addCalc2 step of every vertex height by it */
    /* 0x1A64 */ be<f32> mSandBaseY;  /* was m17E0: wave_cont 02103FA4 (bwdg) sets every vertex y = mWaveHeight + it; start (in move 020FF80C) / demo_camera 02101F30 set or ease it to 200 / 400 / 900; daBwd_Execute 020FA614 raises the platforms to it + 50 */
    /* 0x1A68 */ be<s8> mSandfallState[2];  /* was m17E4: daBwd_Create 020FE0B4 / demo_camera 02101F30 start it (1 / + 1); bwd_execute 020FA614 steps each sandfall: 2 starting (SUNATAKI smoke at 0x32), 3 TAKI_WAIT loop, 4 / 5 TAKI_END, 0 off; daBwd_Draw 020F9C1C draws the sandfall while it is not 0 */
    /* 0x1A6A */ be<s16> mSandfallTimer[2];  /* was m17E6: bwd_execute 020FA614: cleared when a sandfall state starts, + 1 per frame; compared with 0x32 / 0xB4 / 0x3C for the state steps and the SANDFALL sound */
    /* 0x1A6E */ u8 _1A6E[2];
    /* 0x1A70 */ gptr<J3DModel> mpSandfallModel[2];  /* was m17EC: useHeapInit 020FD6F8 creates the 2 sandfall models; bwd_execute 020FA614 places them (YrotM mSandfallAngle) and moves their dBgW; daBwd_Draw 020F9C1C draws them with mPlatformTevStr */
    /* 0x1A78 */ gptr<mDoExt_btkAnm> mpSandfallBtk[2];  /* was m17F4: useHeapInit 020FD6F8: btk of each sandfall model; bwd_execute 020FA614 re-inits it with TAKI_WAIT / TAKI_END and plays it; daBwd_Draw 020F9C1C enters it */
    /* 0x1A80 */ Mtx34 mBgwMtx1[2];
    /* 0x1AE0 */ gptr<dBgW> mpBgW1[2];
    /* 0x1AE8 */ be<s8> mPlatformShakeTimer;  /* was m1864: demo_camera 02101F30 sets 10; bwd_execute 020FA614 counts it down and sets a random mPlatformShakeOffset.x while it runs */
    /* 0x1AE9 */ be<s8> mPlatformHidden;  /* was m1865: daBwd_Create 020FE0B4 sets 1, demo_camera 02101F30 clears it; daBwd_Draw 020F9C1C draws the triforce platform only at 0, bwd_execute 020FA614 moves it to y -20000 while it is set */
    /* 0x1AEA */ u8 _1AEA[2];
    /* 0x1AEC */ gptr<J3DModel> mpTriforcePlatformModel;
    /* 0x1AF0 */ Mtx34 mBgwMtx2;
    /* 0x1B20 */ gptr<dBgW> mpBgW2;
    /* 0x1B24 */ cXyz mPlatformShakeOffset;  /* was m18A0: bwd_execute 020FA614: x random while mPlatformShakeTimer runs; the triforce platform model is placed at it (+ l_HIO heights) */
    /* 0x1B30 */ be<s16> mFrameCounter;  /* was m18AC: bwd_execute 020FA614 adds 1 per frame (move 020FF80C clears it); control1 / end 020FEA30 use it as a sin phase and test its low bits */
    /* 0x1B32 */ be<s16> mAction;  /* was m18AE: move 020FF80C: switch (it) selects the action function (0: start); damage_check (in bwd_execute 020FA614) sets 3, daBwd_Create 020FE0B4 0/1/0xD */
    /* 0x1B34 */ be<s16> mMode;  /* was m18B0: move 020FF80C / end 020FEA30: switch (it) inside each action; damage_check (in bwd_execute 020FA614) clears it with each new action */
    /* 0x1B36 */ u8 _1B36[2];
    /* 0x1B38 */ cXyz mTargetPos;  /* was m18B4: fly_pos_move 020F9F80 flies toward it (x/y/z minus current.pos) */
    /* 0x1B44 */ u8 _1B44[4];
    /* 0x1B48 */ be<f32> mTurnSpeed;  /* was m18C4: end 020FEA30 sets 1000 (+REG); fly_pos_move 020F9F80: angle step = it * mTurnSpeedRate */
    /* 0x1B4C */ be<f32> mTurnSpeedRate;  /* was m18C8: fly_pos_move 020F9F80 multiplies mTurnSpeed by it and eases it to 1 */
    /* 0x1B50 */ be<s16> mTimers[2];  /* was m18CC: bwd_execute 020FA614 counts [0] / [1] down each frame (one sweep of five halfwords with mActionTimer and the two padding words, as zeldaret/tww daBwd_Execute mTimers[i] for i < 5; view Timers in d_a_bwd_execute.h); wait / reset / sita_hit / eat_attack / fly / s_fly (in move 020FF80C) and end 020FEA30 load them and act at 0 or at a given count (as bgn_class / bwds_class mTimers) */
    /* 0x1B54 */ be<s16> mActionTimer;  /* was m18D0: wait / eat_attack / fly / s_fly (in move 020FF80C) set 0xDE / 100 / 0xB4 / 0x12E and act at 0 or at 0x50 */
    /* 0x1B56 */ u8 _1B56[4];
    /* 0x1B5A */ be<s16> mTongueNoHitTimer;  /* was m18D6: damage_check (in bwd_execute 020FA614) checks the tongue for hits only while it is 0 and sets 6 after a hit; sita_move 5, fly (in move 020FF80C) 10; bwd_execute 020FA614 counts it down */
    /* 0x1B5C */ be<u8> mBwdgCreated;  /* was m18D8: bwd_execute 020FA614 creates the BWDG (sand surface) actor once and sets 1 */
    /* 0x1B5D */ u8 _1B5D[3];
    /* 0x1B60 */ be<f32> m18DC;
    /* 0x1B64 */ be<u32> mItemId;  /* was m18E0: demo_camera 02101F30: ID of the item created at mItemPos (bwdCameraCreateItem), looked up with fopAcM_SearchByID */
    /* 0x1B68 */ cXyz mItemPos;  /* was m18E4: end 020FEA30 copies current.pos; demo_camera 02101F30 creates the item there */
    /* 0x1B74 */ be<s16> mSandCrossWait;  /* was m18FC: fly_pos_move 020F9F80 checks the head crossing the sand surface (dive / out splash) only while it is 0 and sets 30 after one; bwd_execute 020FA614 counts it down; HD: no m18F0 */
    /* 0x1B76 */ be<s16> mWobblePhaseSin;  /* was m18FE: end 020FEA30 adds 0x834 per frame; its sin * 7000 is the sideways offset passed to fly_pos_move, and the body segments wobble by sin(it - i * 0x3A98) until they harden */
    /* 0x1B78 */ be<s16> mWobblePhaseCos;  /* was m1900: end 020FEA30 adds 0x9C4 per frame; its cos * 7000 is the second offset passed to fly_pos_move */
    /* 0x1B7A */ be<s16> mRollPhase;  /* was m1902: bwd_execute 020FA614: + 1000 per frame; the roll added to the history angle is sin(it) * 4000 */
    /* 0x1B7C */ be<s32> mHistoryStep;  /* was m1904: end 020FEA30 sets 3, fly (in move 020FF80C) 5 (+REG); bwd_execute 020FA614: history entries between body segments */
    /* 0x1B80 */ sita_s mTongueSegments[32];
    /* 0x1E00 */ cXyz mTongueTipPos;  /* was m1B88: control2 (in bwd_execute 020FA614) puts the last tongue segment (mTongueSegments[29]) at it; sita_move (in bwd_execute) eases it toward the hookshot owner / mTonguePulledPos / the mouth and keeps it on the ground; the tongue loses attention while it is below mSandSurfacePos.y; daBwd_Create 020FE0B4 sets y -5000 / -20000 / current.pos.y, end 020FEA30 -20000 */
    /* 0x1E0C */ cXyz mTonguePulledPos;  /* was m1B94: sita_move (in bwd_execute 020FA614) copies mTongueTipPos into it while mTongueHooked is set, then (while mTonguePulledTimer runs) pulls it to within 1200 of the body and eases the tip to it (shaken by mTongueHitShakeTimer) */
    /* 0x1E18 */ be<f32> mTongueSegLength;  /* was m1BA0: sita_move (in bwd_execute 020FA614) eases it to 0.035 * the mouth-to-tip distance (at least 20 while mTongueRetract is set) and ends the hookshot pull above 50; control1 / control2 (in bwd_execute) place each tongue segment this far from the previous one */
    /* 0x1E1C */ be<f32> m1BA4;
    /* 0x1E20 */ be<f32> mTongueWaveAmplitude;  /* was m1BA8: sita_move (in bwd_execute 020FA614) sets 10 each frame; control1 (in bwd_execute) swings the free tongue segments by sin / cos * it (mTongueHitShakeTimer * 4.5 instead while that runs) */
    /* 0x1E24 */ be<f32> mTongueTipStepScale;  /* was m1BAC: sita_move (in bwd_execute 020FA614) moves the tip by at most 100 * it per frame, clears it while mTongueHooked is set and eases it back to 1; sita_hit (in move 020FF80C) sets 1 */
    /* 0x1E28 */ be<s8> mTongueHooked;  /* was m1BB0: damage_check (in bwd_execute 020FA614) sets 1 when the hookshot (AtType 0x8000) hits the tongue, with mHitOwnerId and mAction 3; sita_move (in bwd_execute) pulls the tip to the owner while it is set and clears it when the hookshot returns or mTongueSegLength exceeds 50; control1 (in bwd_execute) stops the tongue wave while it is set */
    /* 0x1E29 */ be<s8> mTonguePulledTimer;  /* was m1BB1: sita_move (in bwd_execute 020FA614) sets l_HIO.m14 while mTongueHooked is set and counts it down after; damage_check (in bwd_execute) makes the tongue hittable by all attacks (TgType 0xFF1DFEFF) and counts mTongueHitCount only while it runs; sita_hit (in move 020FF80C) ends at 0 and clears it after a damage round; control1 lowers the tongue (y -5) while it runs */
    /* 0x1E2A */ be<s8> mTongueThrashTimer;  /* was m1BB2: sita_hit (in move 020FF80C) sets 0x32 after a damage round and for the last hit; sita_move (in bwd_execute 020FA614) counts it down and swings the tip around the mouth by 400 * sin */
    /* 0x1E2B */ be<s8> mTongueHitShakeTimer;  /* was m1BB3: damage_check (in bwd_execute 020FA614) sets 20 on a counted tongue hit; sita_move counts it down and shakes the tip by it * 3; control1 (in bwd_execute) shakes the segments by it * 4.5 */
    /* 0x1E2C */ be<s8> mTongueHitCount;  /* was m1BB4: damage_check (in bwd_execute 020FA614) adds 1 per tongue hit; sita_hit (in move 020FF80C) acts below 3, then clears it */
    /* 0x1E2D */ be<s8> mTongueDamageCount;  /* was m1BB5: sita_hit (in move 020FF80C) adds 1 per finished damage round (mTongueHitCount reaching 3); at 2 it switches to the flying form (mAction 10), at 4 (or l_HIO.m06) to the death (mAction 0xC) */
    /* 0x1E2E */ be<s8> mTongueRetract;  /* was m1BB6: bwd_execute 020FA614 clears it every frame; start / sita_hit / eat_attack (in move 020FF80C) and end 020FEA30 set 1, fly / s_fly (in move) and end 2; sita_move (in bwd_execute) pulls the tip into the mouth while it is set and drops attention unless it is 2; daBwd_Draw 020F9C1C draws the tongue only below 2 */
    /* 0x1E2F */ u8 _1E2F;
    /* 0x1E30 */ be<f32> mPlayerDistXZ;  /* was m1BB8: bwd_execute 020FA614: XZ distance to the player; wait (in move 020FF80C) compares it with 80 and l_HIO.m18 */
    /* 0x1E34 */ be<u32> mHitOwnerId;  /* was m1BBC: damage_check (in bwd_execute 020FA614): ID of the Stts owner of the hit (else 0xFFFFFFFF); sita_move looks it up */
    /* 0x1E38 */ be<u32> mAttId;  /* was m1BC0: daBwd_Create 020FE0B4: fopAcM_create(fpcNm_ATT_e, 0x65, ...); end 020FEA30 / damage_check look it up */
    /* 0x1E3C */ dCcD_Stts mStts;
    /* 0x1E78 */ dCcD_Sph mBodySph[19];
    /* 0x34BC */ dCcD_Sph mTongueSph;
    /* 0x35E8 */ dCcD_Sph mTongueCoSph[5];
    /* 0x3BC4 */ be<s8> mPushPlayer;  /* was m394C: start (in move 020FF80C) sets 1; bwd_execute 020FA614 pushes the player away from mSandDentCenter once (setOutPower) when below y 900 and clears it */
    /* 0x3BC5 */ u8 _3BC5[7];
    /* 0x3BCC */ cXyz mSandSurfacePos;  /* was m3954: bwd_execute 020FA614 sets y to the ground below the body; fly_pos_move 020F9F80, the actions (in move 020FF80C) and end 020FEA30 set x / z to the body and start the dive / out / hit sand effects and sounds there; mSandDentCenter and mSplashPos copy it; daBwd_Delete 020FD578 deletes its sound object */
    /* 0x3BD8 */ be<s8> mFaceSandState;  /* was m3960: g_eff_on 020F99F8 sets 1, g_eff_off 020F9A10 -1; bwd_execute 020FA614 starts the KAODASHI_SUNA (sand around the head) particle with mFaceSandCb at 1 (then 2, playing SAND_MOVE), ends it at -1 and clears it */
    /* 0x3BD9 */ u8 _3BD9[3];
    /* 0x3BDC */ gptr<JPABaseEmitter> mpHitEmitter;  /* was m3964: sita_hit (in move 020FF80C): particle_set(ID_IT_SN_BWO_HIT_TAIEKI00), made invalid when done */
    /* 0x3BE0 */ gptr<JPABaseEmitter> mpLastHitEmitter;  /* was m3968: end 020FEA30: particle_set(ID_IT_SN_BWO_LASTHIT_TAIEKI00), made invalid when done */
    /* 0x3BE4 */ u8 _3BE4[0xC];
    /* 0x3BF0 */ dPa_smokeEcallBack_bd mSmokeCb[10];  /* was m3978: the 10 sand smoke callbacks: fly_pos_move 020F9F80 [0] / [1] (dive / out), move 020FF80C [2..5], end 020FEA30 [6]; bwd_execute 020FA614 ends [i] when its smoke timer runs out; daBwd_Delete 020FD578 removes them */
    /* 0x3D30 */ dPa_followEcallBack mSandFollowCb[2];  /* was m3AB8: fly_pos_move 020F9F80 passes [0] / [1] to the dive / out sand particles; bwd_execute 020FA614 ends them with mSmokeCb[0] / [1]; daBwd_Delete 020FD578 removes them */
    /* 0x3D58 */ be<s16> mSmokeTimer[2];  /* was m3AE0: fly_pos_move 020F9F80 sets [0] / [1] to 0x6E with the dive / out smoke; bwd_execute 020FA614 counts them down (with mSmokeTimer2 .. 6, ten words, view EffectTimers::mEffect) and ends mSmokeCb[0] / [1] at 0 */
    /* 0x3D5C */ be<s16> mSmokeTimer2;  /* was m3AE4: sita_hit (in move 020FF80C) sets 0x4D when it is 0 (smoke mSmokeCb[2]); bwd_execute 020FA614 counts it down and ends that smoke at 0 */
    /* 0x3D5E */ be<s16> mSmokeTimer3;  /* was m3AE6: eat_attack (in move 020FF80C) sets 10 (+ reg) at head frame 0x1B (smoke mSmokeCb[3]); bwd_execute 020FA614 counts it down and ends that smoke at 0 */
    /* 0x3D60 */ be<s16> mSmokeTimer4;  /* was m3AE8: eat_attack (in move 020FF80C) sets 0x3C when it is 0 (smoke mSmokeCb[4]); bwd_execute 020FA614 counts it down and ends that smoke at 0 */
    /* 0x3D62 */ be<s16> mSmokeTimer5;  /* was m3AEA: eat_attack (in move 020FF80C) sets 0x14 when it is 0 (smoke mSmokeCb[5]); bwd_execute 020FA614 counts it down and ends that smoke at 0 */
    /* 0x3D64 */ be<s16> mSmokeTimer6;  /* was m3AEC: end 020FEA30 sets 0x28 when it is 0 (smoke mSmokeCb[6]); bwd_execute 020FA614 counts it down and ends that smoke at 0 */
    /* 0x3D66 */ u8 _3D66[6];
    /* 0x3D6C */ dPa_followEcallBack mFaceSandCb;  /* was m3AF4: bwd_execute 020FA614 passes it to the KAODASHI_SUNA particle and ends it (mFaceSandState); daBwd_Delete 020FD578 removes it */
    /* 0x3D80 */ be<s8> mSplashFrame[2];  /* was m3B08: fly_pos_move 020F9F80 sets [0] (out) / [1] (dive) to 1; bwd_execute 020FA614: + 1 per frame up to 0x3C, then 0; it - 1 is the frame of the GSP splash model; daBwd_Draw 020F9C1C draws the splash while it is set */
    /* 0x3D82 */ u8 _3D82[2];
    /* 0x3D84 */ cXyz mSplashPos[2];  /* was m3B0C: fly_pos_move 020F9F80 copies the sand crossing point (mSandSurfacePos); bwd_execute 020FA614 places the splash model there */
    /* 0x3D9C */ gptr<mDoExt_McaMorf> mpGspMorf[2];
    /* 0x3DA4 */ gptr<mDoExt_btkAnm> mpGspBtkAnm[2];
    /* 0x3DAC */ gptr<mDoExt_brkAnm> mpGspBrkAnm[2];
    /* 0x3DB4 */ be<f32> m3B3C;
    /* 0x3DB8 */ be<f32> m3B40;
    /* 0x3DBC */ be<f32> m3B44;
    /* 0x3DC0 */ csXyz mSandfallAngle[2];  /* was m3B48: bwd_execute 020FA614: y = demo_ang[i] (+ reg) per sandfall; YrotM of the sandfall model by it */
    /* 0x3DCC */ dPa_smokeEcallBack_bd mSandfallSmokeCb[6];  /* was m3B54: bwd_execute 020FA614 passes [i * 3 + j] to the 3 SUNATAKI_SMOKEA particles of sandfall i and removes them when it ends; daBwd_Delete 020FD578 removes them */
    /* 0x3E8C */ be<s8> mKankyoTimer;  /* was m3C14: fly_pos_move 020F9F80 sets 0x5A; bwd_kankyo (in bwd_execute 020FA614) counts it down and leaves mKankyoState 2 at 0 */
    /* 0x3E8D */ be<s8> mKankyoState;  /* was m3C15: daBwd_Create 020FE0B4 sets 5 / 1, fly_pos_move 020F9F80 2, demo_camera 02101F30 1 / 5; bwd_kankyo (in bwd_execute 020FA614) picks the dKy_custom_colset pair by it (0..5) */
    /* 0x3E8E */ u8 _3E8E[2];
    /* 0x3E90 */ be<f32> mEnvColorRate;  /* was m3C18: bwd_kankyo (in bwd_execute 020FA614): setEnvironmentColor(..., it), eased to 1 */
    /* 0x3E94 */ be<s16> mBlurRate;  /* was m3C1C: daBwd_Draw 020F9C1C: mDoGph_setBlureRate(it) while it is >= 1 */
    /* 0x3E96 */ be<s16> mDemoMode;  /* was m3C1E: demo_camera 02101F30: switch (it) of the demo camera steps, +1 per step; fly_pos_move 020F9F80 tests it */
    /* 0x3E98 */ be<s16> mDemoTimer;  /* was m3C20: demo_camera 02101F30: cleared per step and compared with frame marks (0x1E, 0x3C, ...) */
    /* 0x3E9A */ be<s16> mDemoMainTimer;  /* was m3C22: demo_camera 02101F30: cleared with mDemoTimer when a demo starts but not per step, + 1 per frame ("K MAIN COUNT"); compared with 0xAA, 0x280, 0x2EE, 0x2F8, 0x5B4 */
    /* 0x3E9C */ be<s8> mBodyExplodeNo;  /* was m3C24: end 020FEA30: cleared, + 1 every 4th frame up to 20; the segment of mBodyPos that bursts; demo_camera 02101F30 eases its centre to mBodyPos[it] */
    /* 0x3E9D */ u8 _3E9D[3];
    /* 0x3EA0 */ cXyz mDemoCamEye;  /* was m3C28: demo_camera 02101F30: copied from the camera's mLookat.mEye, eased toward the next eye; the eye passed to bwdCameraSet is built from it */
    /* 0x3EAC */ cXyz mDemoCamCenter;  /* was m3C34: demo_camera 02101F30: copied from the camera's mLookat.mCenter, eased toward the next centre; the centre passed to bwdCameraSet is built from it */
    /* 0x3EB8 */ u8 _3EB8[4];
    /* 0x3EBC */ be<f32> mDemoCamSpeed;  /* was m3C44: demo_camera 02101F30 clears it at the demo starts; DEMO_DEATH_SEGMENT eases it to 1 and moves mDemoCamCenter by up to 200 * it per frame; DEMO_WARP eases it to 40 and back to 0 and turns current.angle.y (which places mDemoCamEye) by it; end 020FEA30 clears it */
    /* 0x3EC0 */ be<f32> mDemoCamFovy;  /* was m3C48: demo_camera 02101F30: 45/55 (eased), passed to bwdCameraSet as the fovy */
    /* 0x3EC4 */ be<f32> mDemoCamShake;  /* was m3C4C: fly_pos_move 020F9F80 sets 50, demo_camera 02101F30 5 / 1.5 / 2 (+ reg); demo_camera shakes the camera by it and eases it to 0 */
    /* 0x3EC8 */ be<s8> mCamTypeState;  /* was m3C50: daBwd_Create 020FE0B4 sets 1, fly (in move 020FF80C) 2, demo_camera 02101F30 0; bwd_execute 020FA614 forces camera type "Boss03" at 2 (then 1) and "WindBoss" at 1 */
    /* 0x3EC9 */ be<u8> mHioOwner;  /* was m3C51: daBwd_Create 020FE0B4 sets 1 when it created the l_HIO child; daBwd_Delete 020FD578 deletes the HIO child only then */
    /* 0x3ECA */ u8 _3ECA[2];
};
WWHD_OFFSET(bwd_class, mpHeadMorf, 0x3DC);
WWHD_OFFSET(bwd_class, mBodyAnmStartNo, 0x580);
WWHD_OFFSET(bwd_class, mPlatformTevStr, 0x1880);
WWHD_OFFSET(bwd_class, mSandWaveMode, 0x1A48);
WWHD_OFFSET(bwd_class, mpBgW2, 0x1B20);
WWHD_OFFSET(bwd_class, mSandCrossWait, 0x1B74);
WWHD_OFFSET(bwd_class, mTongueSegments, 0x1B80);
WWHD_OFFSET(bwd_class, mTongueTipPos, 0x1E00);
WWHD_OFFSET(bwd_class, mStts, 0x1E3C);
WWHD_OFFSET(bwd_class, mTongueSph, 0x34BC);
WWHD_OFFSET(bwd_class, mPushPlayer, 0x3BC4);
WWHD_OFFSET(bwd_class, mSmokeCb, 0x3BF0);
WWHD_OFFSET(bwd_class, mFaceSandCb, 0x3D6C);
WWHD_OFFSET(bwd_class, mpGspMorf, 0x3D9C);
WWHD_OFFSET(bwd_class, mSandfallSmokeCb, 0x3DCC);
WWHD_OFFSET(bwd_class, mBlurRate, 0x3E94);
WWHD_OFFSET(bwd_class, mHioOwner, 0x3EC9);
WWHD_SIZE(bwd_class, 0x3ECC);

/* l_HIO (0x10462A68): daBwd_HIO_c, HD layout {mNo, m05.., floats.., vtable last}, 0x40 bytes */
struct daBwd_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ be<u8> m06;
    /* 0x03 */ be<u8> m07;
    /* 0x04 */ be<u8> m08;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<s16> m14;
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ be<f32> m18;
    /* 0x18 */ be<f32> m1C;
    /* 0x1C */ be<f32> m20;
    /* 0x20 */ be<s16> m24;
    /* 0x22 */ be<s16> m26;
    /* 0x24 */ be<f32> m28;
    /* 0x28 */ be<f32> m2C;
    /* 0x2C */ be<f32> m30;
    /* 0x30 */ be<f32> m34;
    /* 0x34 */ be<s16> m38;
    /* 0x36 */ be<s16> m3A;
    /* 0x38 */ be<s16> m3C;
    /* 0x3A */ be<s16> m3E;
    /* 0x3C */ be<u32> __vtbl;
};
WWHD_SIZE(daBwd_HIO_c, 0x40);
#define l_HIO (*gabi::at<daBwd_HIO_c>(0x10462A68))
#define hio_set (*gabi::at<be<u8>>(0x10192F24))
#define eff_col 0x10462A54u /* GXColor */
#define ko_count (*gabi::at<be<s32>>(0x10462A58))
static inline be<u32>* ko_ac() { return gabi::at<be<u32>>(0x10462B3C); }
static inline cXyz* suna_gr_pos() { return gabi::at<cXyz>(0x10462AF4); }
static inline csXyz* suna_gr_ang() { return gabi::at<csXyz>(0x10462AB8); }
#define center_pos (gabi::at<cXyz>(0x10462ADC))

#define BWD_VTBL 0x1000BE6C     /* bwd_class vtable (HD virtual destructor) */
#define BWD_HIO_VTBL 0x1000BE5C /* daBwd_HIO_c vtable */
#define SAFESTRING_VTBL 0x1000BDA4

/* ---- local bindings (SHARED-CANDIDATE) ---- */
#define REG0_F(i) REG_F(0, i)
#define REG0_S(i) REG_S(0, i)
/* virtual remove() (vtable +0x44) of a particle callback */
static inline void bd_vremove(void* cb) {
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(cb)) + 0x44), cb);
}
/* attention_info.flags (+0x39C) */
static inline be<u32>& attn_flags(fopAc_ac_c* a) { return *gabi::at<be<u32>>(gabi::ea(a) + 0x39C); }
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices
 * +0x10) */
static inline Mtx34* bd_getAnmMtx(J3DModel* m, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(m) + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jnt * 0x30);
}
/* 02518CC8 def_se_set(fopAc_ac_c*, cCcD_Obj*, u32) */
static inline void def_se_set(fopAc_ac_c* a, void* obj, u32 mtrl) {
    gabi::call(0x02518CC8, a, obj, mtrl);
}
struct CcAtInfo_bd {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ u8 _08[0x14 - 0x08];
    /* 0x14 */ be<u32> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(CcAtInfo_bd, 0x1C);
static inline void cc_at_check(fopAc_ac_c* a, CcAtInfo_bd* info) {
    gabi::call(0x025192A8, a, info);
}
/* dComIfGp_particle_setToon (HD): dPa_control_c::set with group 2, setupInfo = roomNo */
static inline JPABaseEmitter* dComIfGp_particle_setToon(u16 id, const cXyz* pos, const csXyz* angle,
                                                        const cXyz* scale, u8 alpha, void* cb,
                                                        s8 roomNo) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, angle, scale, alpha, (dPa_levelEcallBack*)cb, roomNo,
                           nullptr, nullptr, nullptr);
}
/* dPa_smokeEcallBack::setColor (inline): 4-byte copy */
static inline void smoke_setColor(dPa_smokeEcallBack_bd* cb, u32 col) {
    u8 c[4];
    for (int k = 0; k < 4; k++)
        c[k] = gabi::load<u8>(col + k);
    for (int k = 0; k < 4; k++)
        cb->mColor[k] = c[k];
}
/* mDoExt_brkAnm (HD 0x78): constructor 025E80D0 (the matcher calls it init), init 025E8154 */
static inline void* mDoExt_brkAnm_ct(void* p) { return gabi::call<void*>(0x025E80D0, p); }
static inline BOOL mDoExt_brkAnm_init(void* a, J3DModelData* d, void* brk, s32 anmPlay, s32 mode,
                                      f32 rate, s16 start, s16 end, bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, brk, anmPlay, mode, rate, start, end, modify, entry);
}
/* mDoExt_btkAnm (HD 0x74): constructor 025E7C6C (the matcher calls it init) */
static inline void* mDoExt_btkAnm_ct(void* p) { return gabi::call<void*>(0x025E7C6C, p); }
/* GHS array helpers */
static inline void __construct_array(void* p, u32 n, u32 size, u32 ctor) {
    gabi::call(0x028EFFD0, p, n, size, ctor);
}
static inline void __destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 f7, u32 f8) {
    gabi::call(0x028F0164, p, n, size, dtor, f7, f8);
}
/* 02515AE8 dCcD_Sph::~dCcD_Sph, 025166F0 dCcD_Sph::dCcD_Sph */
static inline void dCcD_Sph_dt(void* s, s32 flags) { gabi::call(0x02515AE8, s, flags); }
static inline void dCcD_Sph_ct(void* s) { gabi::call(0x025166F0, s); }
/* dBgS_LinChk on the stack (this TU's vtables; layout as in d_a_mt.h) */
struct dBgS_LinChk_bd {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x58 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x64 */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ u8 _24[0x58 - 0x24];
    /* 0x58 */ be<u32> __vtbl_58;
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64;
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_LinChk_bd, 0x6C);
static inline void bd_LinChk_ct(dBgS_LinChk_bd* c) {
    cBgS_LinChk_ct(c);
    for (int i = 0; i < 7; i++)
        c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(c) + 0x64;
    c->mpPolyPassChk = gabi::ea(c) + 0x58;
    c->__vtbl_10 = 0x1000BE1C;
    c->__vtbl_64 = 0x1000BE3C;
    c->__vtbl_58 = 0x1000BE4C;
    c->__vtbl_20 = 0x1000BE2C;
}
static inline void bd_LinChk_dt(dBgS_LinChk_bd* c) {
    c->__vtbl_58 = 0x1000BE4C;
    c->__vtbl_64 = 0x1000BDCC;
    c->__vtbl_20 = 0x1000BDBC;
    cBgS_LinChk_dt(c, 0);
}

/* calls between this unit's source files (by address) */
static inline void bwd_anm_init(bwd_class* i_this, int bck, f32 morf, u8 loopMode, f32 speed,
                                int snd) {
    gabi::call(0x020F9AF4, i_this, bck, morf, loopMode, speed, snd);
}
static inline void bwd_fly_pos_move(bwd_class* i_this, s16 p2, s16 p3) {
    gabi::call(0x020F9F80, i_this, p2, p3);
}
static inline void bwd_g_eff_on(bwd_class* i_this) { gabi::call(0x020F99F8, i_this); }
static inline void bwd_g_eff_off(bwd_class* i_this) { gabi::call(0x020F9A10, i_this); }
#define REG10_F(i) REG_F(10, i)
/* dBgS_GndChk on the stack (this TU's vtables) */
static const dBgS_GndChk_vt BD_GNDCHK_VT = {0x1000BDDC, 0x1000BDEC, 0x1000BE0C, 0x1000BDFC};
static inline void bd_GndChk_dt(void* c) {
    u32 b = gabi::ea(c);
    gabi::store<u32>(b + 0x20, 0x1000BDEC);
    gabi::store<u32>(b + 0x40, 0x1000BE0C);
    gabi::store<u32>(b + 0x4C, 0x1000BDCC);
    gabi::call(0x02008DAC, c, 0); /* cBgS_Chk::~cBgS_Chk */
}
/* fopAcM_seStart as inlined in this unit (HD: only the eyePos address is checked) */
static inline void bd_seStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0)
        mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline u8 dComIfGp_getStartStageName0() { return gabi::load<u8>(dComIfGp_ea() + 0x5134); }
/* JAIZelBasic (HD): bgmStreamPrepare 025E1934, bgmStreamPlay 025E1944, bgmStart 025E18EC */
static inline void mDoAud_bgmStreamPrepare(u32 id) { gabi::call(0x025E1934, id); }
static inline void mDoAud_bgmStreamPlay() { gabi::call(0x025E1944); }
/* JPABaseEmitter (HD inline): becomeInvalidEmitter (+0x5C = -1, flags +0x254 |= 1);
 * setGlobalRTMatrix */
static inline void JPA_becomeInvalidEmitter(JPABaseEmitter* e) {
    u32 b = gabi::ea(e);
    u32 f = gabi::load<u32>(b + 0x254);
    gabi::store<s32>(b + 0x5C, -1);
    gabi::store<u32>(b + 0x254, f | 1);
}
static inline void JPA_setGlobalRTMatrix(JPABaseEmitter* e, Mtx34* m) {
    gabi::call(0x028249B0, m, gabi::ea(e) + 0x1F0, gabi::ea(e) + 0x22C); /* JPASetRMtxTVecfromMtx */
}
#define REG10_S(i) REG_S(10, i)

// HD: private views for the exact GHS light-copy operations in this translation unit.
namespace bwd_detail {
struct LightBlock {
    be<f32> mPosition[3];
    be<f32> mDirection[3];
    be<u8> mColor[4];
    be<s16> mParameters[4];
    be<f32> mFactors[8];
};
WWHD_OFFSET(LightBlock, mColor, 0x18);
WWHD_OFFSET(LightBlock, mParameters, 0x1C);
WWHD_OFFSET(LightBlock, mFactors, 0x24);
WWHD_SIZE(LightBlock, 0x44);
struct TevCopyView {
    LightBlock mLight;
    u8 mLightPadding[0x84 - 0x44];
    be<u32> mLightPositionWords[3];
    be<u16> mColorC0[4];
    be<u8> mColorsK[8];
    be<u16> mFogColor[4];
    be<f32> mFogAndBlend[3];
    be<u8> mEnvironment[9];
    u8 mPadding[3];
    LightBlock mHdLight;
    u8 mHdLightPadding[0x144 - 0x104];
    LightBlock mHdLight2;
    u8 mTail[0x1C8 - 0x188];
};
WWHD_OFFSET(TevCopyView, mLightPositionWords, 0x84);
WWHD_OFFSET(TevCopyView, mColorC0, 0x90);
WWHD_OFFSET(TevCopyView, mColorsK, 0x98);
WWHD_OFFSET(TevCopyView, mFogColor, 0xA0);
WWHD_OFFSET(TevCopyView, mFogAndBlend, 0xA8);
WWHD_OFFSET(TevCopyView, mEnvironment, 0xB4);
WWHD_OFFSET(TevCopyView, mHdLight, 0xC0);
WWHD_OFFSET(TevCopyView, mHdLight2, 0x144);
WWHD_SIZE(TevCopyView, 0x1C8);
} // namespace bwd_detail

namespace bwd_detail {
struct SphereSource {
    u8 mData[0x40];
};
WWHD_SIZE(SphereSource, 0x40);
struct Larva {
    u8 mUnaccessed[0x60C];
    be<s8> mDeleteRequest; /* was m04F4: bwds_class mDeleteRequest (0x60C); ko_delete_sub 020F9AA0 sets 1 for every larva, whose damage_check (in daBwds_Execute 0210602C) then deletes it */
};
WWHD_OFFSET(Larva, mDeleteRequest, 0x60C);
WWHD_SIZE(Larva, 0x60D);
} // namespace bwd_detail

namespace bwd_detail {
struct BlurState {
    be<u8> mEnabled;
    be<u8> mRate;
};
WWHD_OFFSET(BlurState, mRate, 1);
WWHD_SIZE(BlurState, 2);
struct ModelFlags {
    u8 mUnaccessed[0x74];
    be<u32> mFlags;
};
WWHD_OFFSET(ModelFlags, mFlags, 0x74);
WWHD_SIZE(ModelFlags, 0x78);
struct ModelDataAnimation {
    u8 mUnaccessed[0x44];
    be<u32> mTextureMatrixAnimation;
};
WWHD_OFFSET(ModelDataAnimation, mTextureMatrixAnimation, 0x44);
WWHD_SIZE(ModelDataAnimation, 0x48);
struct Background {
    u8 mUnaccessed[0xA8];
    be<u32> mCorrectionCallback;
};
WWHD_OFFSET(Background, mCorrectionCallback, 0xA8);
WWHD_SIZE(Background, 0xAC);
} // namespace bwd_detail

namespace bwd_detail {
enum {
    fpcNm_BWDS_e = 0xDA,
    DSNAP_TYPE_BWD = 0xCC,
    ID_IT_ST_BWO_DIVE_SMOKE00 = 0xA25A,
    ID_IT_SN_BWO_DIVE_SUNA00 = 0x8259,
    JA_SE_CM_BWD_IN_SAND_1 = 0x5912,
    ID_IT_ST_BWO_OUT_SMOKE00 = 0xA258,
    ID_IT_SN_BWO_OUT_SUNA00 = 0x8257,
    JA_SE_CM_BWD_OUT_SAND_1 = 0x590F,
    fpcNm_ATT_e = 0x1C0,
    JA_BGM_PAST_RANE = 0x8000004C,
    JA_BGM_RANE_BATTLE = 0x80000029,
};
}

namespace bwd_detail {
// HD: callbacks implemented outside this translation unit retain their guest ABI addresses.
constexpr u32 bwdConstructSphereCallback = 0x025166F0;
constexpr u32 bwdDestroySphereCallback = 0x02515AE8;
constexpr u32 bwdTypicalCorrectionCallback = 0x024EE658;
} // namespace bwd_detail

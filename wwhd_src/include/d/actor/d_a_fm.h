/* daFm_c (Floormaster), WWHD layout. 
 *
 * GameCube -> WWHD (size 0xEA0 -> 0x101C, constructor 02141074):
 * - +0x11C up to mBtkAnm (fopEn_enemy_c);
 * - mDoExt_btkAnm grew by 0x60 (HD 0x74): +0x17C from mHoleScale on.
 * Offsets from the constructor and the verified functions. */
#pragma once
#include "bindings.h"

#ifndef WWHD_ENEMYICE_FIRE_L
#define WWHD_ENEMYICE_FIRE_L
/* enemyice / enemyfire (c_damagereaction), WWHD layouts (SHARED-CANDIDATE, as in d_a_cc.h) */
struct enemyice_l {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFreezeDuration;
    /* 0x006 */ be<s8> mLightShrinkTimer;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<f32> mYOffset;
    /* 0x00C */ be<s8> mFreezeType;  /* was m00C: view of enemyice::mFreezeType (c/c_damagereaction.h) */
    /* 0x00D */ be<s8> mMode;
    /* 0x00E */ be<s16> mFreezeTimer;
    /* 0x010 */ be<s16> mMoveDelayTimer;
    /* 0x012 */ be<s16> mAngleY;
    /* 0x014 */ be<s16> mAngularVelY;
    /* 0x016 */ u8 _016[2];
    /* 0x018 */ cXyz mSpeed;
    /* 0x024 */ be<f32> mSpeedF;
    /* 0x028 */ be<f32> mBodyLiftY;  /* was m028: view of enemyice::mBodyLiftY (c/c_damagereaction.h) */
    /* 0x02C */ be<f32> mIceOffsetY;  /* was m02C: view of enemyice::mIceOffsetY (c/c_damagereaction.h) */
    /* 0x030 */ dCcD_Stts mStts;
    /* 0x06C */ dCcD_Cyl mCyl;
    /* 0x19C */ be<f32> mCylHeight;
    /* 0x1A0 */ be<f32> mWallRadius;
    /* 0x1A4 */ be<f32> mScaleXZ;
    /* 0x1A8 */ be<f32> mScaleY;
    /* 0x1AC */ be<f32> mParticleScale;
    /* 0x1B0 */ be<u8> m1B0;
    /* 0x1B1 */ be<u8> mDeathSwitch;
    /* 0x1B2 */ u8 _1B2[2];
    /* 0x1B4 */ dBgS_AcchCir mBgAcchCir;
    /* 0x1F4 */ dBgS_ObjAcch mBgAcch;
};
WWHD_SIZE(enemyice_l, 0x3B8);

struct enemyfire_l {
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
    /* 0x208 */ u8 mLight[0x20];  /* LIGHT_INFLUENCE */
    /* 0x228 */ be<f32> mHD20;  /* HD: 1.0f in the constructor; was m228: mLight (0x208, a LIGHT_INFLUENCE of HD size 0x24, declared here as its first 0x20 bytes) + 0x20 = LIGHT_INFLUENCE::mHD20 (bindings.h / d/d_kankyo.h), the light's HD tail; the enemyfire constructors (pt 02446C68, ki 0219D368, ks 021AA2D8, cc 02110B28, oq 023C3C80, am 0204B6A0, rd 0245A984, nz 0230F104) set it to 1.0 as the LIGHT_INFLUENCE constructor does; no reader in WWHD, so it keeps the class name */
};
WWHD_SIZE(enemyfire_l, 0x22C);
#endif

WWHD_OPAQUE(JntHit_c);

struct Quaternion_fm { be<f32> x, y, z, w; };

struct daFm_c : fopEn_enemy_c {
    enum Proc_e { PROC_INIT_e = 0, PROC_EXEC_e = 1 };

    bool isBodyAppear() {
        int m = mMode;
        return m != 1 && m != 0x12 && m != 0 && (m != 2 && (m != 4) && (m != 3 && (m != 0x11)));
    }
    void modeProcInit(int newMode) { modeProc(PROC_INIT_e, newMode); }

    void _nodeControl(J3DNode*, J3DModel*);
    BOOL _createHeap();
    bool holeCreateHeap();
    bool bodyCreateHeap();
    bool jntHitCreateHeap();
    BOOL _pathMove(cXyz*, cXyz*, cXyz*);
    fopAc_ac_c* searchNearOtherActor(fopAc_ac_c*);
    void* searchNearFm(fopAc_ac_c*);
    void moveRndBack();
    void moveRndEscape();
    void spAttackVJump();
    void spAttackJump();
    void spAttackNone();
    void iceProc();
    void bodySetMtx();
    void holeSetMtx();
    void setCollision();
    void setAttention();
    bool checkTgHit();
    void setGrabPos();
    void getOffsetPos(cXyz* out);  /* returns cXyz by hidden pointer */
    u8 checkPlayerGrabBomb();
    u8 checkPlayerGrabNpc();
    u8 checkPlayerGrabTarget();
    bool isGrabPos();
    bool isGrab();
    bool isGrabFoot();
    void modeSwWaitInit();
    void modeSwWait();
    void modeHideInit();
    void modeHide();
    void modeUnderFootInit();
    void modeUnderFoot();
    void modePathMoveInit();
    void modePathMove();
    void modeGoalKeeperInit();
    void modeGoalKeeper();
    void modeAppearInit();
    void modeAppear();
    void modeDisappearInit();
    void modeDisappear();
    void modeWaitInit();
    void modeWait();
    void modeAttackInit();
    void modeAttack();
    void modeThrowInit();
    void modeThrow();
    void modeGrabFootDemoInit();
    void modeGrabFootDemo();
    void modeParalysisInit();
    void modeParalysis();
    void modeDamageInit();
    void modeDamage();
    void modeGrabInit();
    void modeGrab();
    void modeGrabDemoInit();
    void modeGrabDemo();
    void modeDeathInit();
    void modeDeath();
    void modePrepareItemInit();
    void modePrepareItem();
    void modeGrabNpcDemoInit();
    void modeGrabNpcDemo();
    void modePlayerStartDemoInit();
    void modePlayerStartDemo();
    void modeDeleteInit();
    void modeDelete();
    void modeBikubikuInit();
    void modeBikubiku();
    void modeProc(Proc_e, int);
    void setAnm(s8, bool);
    void cancelGrab();
    void calcInvKine(fopAc_ac_c*);
    void resetInvKine();
    void grabBomb();
    void grabTsubo();
    void grabPlayer();
    void grabNPC();
    void searchTarget();
    void setBaseTarget();
    void turnToBaseTarget();
    bool isNpc(fopAc_ac_c*);
    bool checkHeight(fopAc_ac_c*);
    bool isLink(fopAc_ac_c*);
    bool isLinkControl();
    bool areaCheck();
    bool lineCheck(cXyz*, cXyz*);
    int setRnd(int, int);
    void setHoleEffect();
    void holeExecute();
    bool setHoleScale(f32, f32, f32);
    bool _execute();
    void MtxToRot(Mtx34*, csXyz*);
    void debugDraw();
    void holeDraw();
    void bodyDraw();
    bool _draw();
    void getArg();
    void createInit();
    cPhs_State _create();
    bool _delete();

    /* 0x3C8 */ be<s32> mMode;
    /* 0x3CC */ dPa_followEcallBack mpFollowEcallBack;
    /* 0x3E0 */ be<s8> mBckIdx;
    /* 0x3E1 */ be<s8> mAnmPrmIdx;
    /* 0x3E2 */ be<s8> mOldAnmPrmIdx;
    /* 0x3E3 */ be<u8> mExitSclsNo;  /* was field_0x2C7: getArg 021413E0: param bits 0-7; modeGrabDemo 021482F0 calls dLib_setNextStageBySclsNum(it) when the grab demo with Link ends (0xFF: back to the start stage) (tww the same) */
    /* 0x3E4 */ be<s16> mNpcRestartPoint;  /* was field_0x2C8: getArg 021413E0: home.angle.x bits 8-15; modeGrabDemo 021482F0 passes it to daPy_npc_setPointRestart for Makar / Medli when DEFAULT_FM_SUIKOMI_NPC ends (0xFF: none) (tww the same) */
    /* 0x3E6 */ u8 _3E6[2];
    /* 0x3E8 */ be<s32> m_path_no;
    /* 0x3EC */ be<s32> mMoveType;  /* was field_0x2D0: getArg 021413E0: param bits 8-9 (3 -> 0); createInit 02141930 starts MODE_UNDERFOOT / MODE_PATH_MOVE / MODE_GOAL_KEEPER for 0 / 1 / 2 and picks the Fmaster / Fmastr1 / Fmastr2 item table; areaCheck 0214264C, modeDisappear 02146310, modeGrab 02147BE0 branch on it (tww the same) */
    /* 0x3F0 */ be<s32> mAppearSwitchNo;  /* was field_0x2D4: getArg 021413E0: param bits 24-31; createInit 02141930 starts MODE_SW_WAIT while it is not set; modeSwWait 021442A8 waits for dComIfGs_isSwitch(it) (tww the same) */
    /* 0x3F4 */ be<s32> mNoCreateSwitchNo;  /* was field_0x2D8: getArg 021413E0: home.angle.x bits 0-7; _create 02141E40 returns cPhs_ERROR_e while that switch is set (tww the same) */
    /* 0x3F8 */ be<s32> mBaseTargetType;  /* was field_0x2DC: getArg 021413E0: param bits 10-11 (3 -> 0); setBaseTarget 02141714: 0 Medli or Link, 1 Link, 2 Makar or Medli; searchTarget 02145A60 / modeGrab 02147BE0 branch on it (tww the same) */
    /* 0x3FC */ be<f32> mAreaRadius;  /* was field_0x2E0: getArg 021413E0: home.angle.z bits 0-7 times 100 (0xFF or 0: 3000); areaCheck 0214264C drops area points farther than it from mHomePos; modeGoalKeeper 02144B94 / isGrabPos 02145F90 / modeWait 021464B4 compare XZ distances to mHomePos with it (tww the same) */
    /* 0x400 */ be<u8> mSwAppearDemo;  /* was field_0x2E4: getArg 021413E0 sets 1 when home.angle.y != 0 and mAppearSwitchNo != 0xFF; with it createInit 02141930 turns off the wall check, modeSwWait 021442A8 orders DEFAULT_FM_SW_APEEAR, modeGrab 02147BE0 goes to MODE_GRAB_NPC_DEMO, isGrab 021467D8 doubles mGrabReach, modeDisappear 02146310 deletes (tww the same) */
    /* 0x401 */ be<u8> mSwAppearInEvent;  /* was field_0x2E5: modeSwWait 021442A8 sets 1 when the switch opens while DEFAULT_FM_SW_APEEAR runs; modeDisappear 02146310 deletes the actor while set; modeGoalKeeper 02144B94 waits for DEFAULT_FM_SUIKOMI_NPC to end while set (tww the same) */
    /* 0x402 */ u8 _402[2];
    /* 0x404 */ cXyz mJointPos[6];  /* was field_0x2E8: _nodeControl 021404AC stores the world position of joints 0-5 (stack matrix translation); calcInvKine 021468E8 reads them as the IK chain (tww the same) */
    /* 0x44C */ Quaternion_fm mJointQuat[6];  /* was field_0x330: the IK rotation per joint: createInit 02141930 sets ZeroQuat; calcInvKine 021468E8 / resetInvKine 021441D0 slerp it toward the solved / zero rotation; _nodeControl 021404AC applies it (mDoMtx_stack_quatM) (tww the same) */
    /* 0x4AC */ be<s32> mIkStartJoint;  /* was field_0x390: calcInvKine 021468E8 stores the first IK joint index; resetInvKine 021441D0 clears it; _nodeControl 021404AC applies mJointQuat only from it on (tww the same) */
    /* 0x4B0 */ be<f32> mSpeedRate;  /* was field_0x394: createInit 02141930 / _pathMove 02140AC8: cM_rndF(9) + 1; modePathMove 02144A18 / modeGoalKeeper 02144B94 / modeAttack 02146A34 / moveRndEscape 02147914 scale the fm_addCalcPosXZ2 step by it (tww the same) */
    /* 0x4B4 */ cXyz field_0x398;
    /* 0x4C0 */ cXyz field_0x3A4;
    /* 0x4CC */ cXyz mPathPos;  /* was field_0x3B0: modePathMove 02144A18: dLib_pathMove moves it along mpPath and the actor follows it with fm_addCalcPosXZ2 (tww the same) */
    /* 0x4D8 */ be<s8> mPathPntNo;  /* was field_0x3BC: modePathMove 02144A18: dLib_pathMove's point index argument for mpPath (tww the same) */
    /* 0x4D9 */ u8 _4D9[3];
    /* 0x4DC */ gptr<dPath> mpPath;
    /* 0x4E0 */ u8 _4E0[4];
    /* 0x4E4 */ gptr<J3DModel> mpModel;
    /* 0x4E8 */ mDoExt_btkAnm mBtkAnm;
    /* 0x55C */ be<f32> mHoleScale;  /* was field_0x3E0: the hole scale: setHoleScale 02142450 chases it (cLib_addCalc2); holeSetMtx 0214162C sets the hole model base scale to it; setHoleEffect 02142158 the emitter scale; setCollision 021432B8 mCyl.SetR(it); holeExecute 02142200 / _draw 021440D8 test it > 0.015 (tww the same) */
    /* 0x560 */ cXyz mNearFmPos;  /* was field_0x3E4: searchNearFm 02140F14 stores the position of the nearest other Floormaster; modeGrab 02147BE0 (goal keeper with an NPC) turns toward it when it is non-zero (tww the same) */
    /* 0x56C */ request_of_phase_process_class mPhs;
    /* 0x574 */ u8 _574[4];
    /* 0x578 */ be<u8> mGrabDemoLink;  /* was field_0x3FC: modeGrabDemoInit 021482AC stores fm_isLink(mpActorTarget); modeGrabDemo 021482F0 runs Link's grab demo (DEFAULT_FM_GRAB, next stage) while set, the NPC branch otherwise (tww the same) */
    /* 0x579 */ u8 _579[3];
    /* 0x57C */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x580 */ dBgS_ObjAcch mObjAcch;
    /* 0x744 */ dBgS_AcchCir mAcchCir;
    /* 0x784 */ u8 mInvisibleModel[8];  /* mDoExt_invisibleModel */
    /* 0x78C */ cXyz mBodyOffset;  /* was field_0x610: bodySetMtx 0214153C translates the body model by it; createInit 02141930 copies HIO mBodyOffsetInit into it; modeGrabDemo 021482F0 chases it to 0; modeGrabFootDemoInit 0214879C clears it (tww the same) */
    /* 0x798 */ cXyz mHandPos;  /* was field_0x61C: _nodeControl 021404AC: mHandOffset through the TE joint matrix; setAttention 021422A4 attention / eye position, setCollision 021432B8 mSph center, _draw 021440D8 snap position, isGrab 021467D8 / calcInvKine 021468E8 distance to mGrabPos, modeGrab 02147BE0 copies it into mGrabPos for held items (tww the same) */
    /* 0x7A4 */ u8 _7A4[8];
    /* 0x7AC */ cXyz mFootGrabLinkPos;  /* was field_0x630: modeGrabFootDemoInit 0214879C copies Link's position; modeGrabFootDemo 021487F8 chases it to the actor (fm_addCalcPosXZ2) and sets Link there (daPy_setPlayerPosAndAngle) (tww the same) */
    /* 0x7B8 */ cXyz mPullPos;  /* was field_0x63C: _nodeControl 021404AC: HIO mPullOffset through the TE joint matrix; modeAttack 02146A34 (fm_pushLink) / modeGrab 02147BE0 pull Link toward it by distance and angle from mGrabPos (tww the same) */
    /* 0x7C4 */ be<s32> mMoveTimer;  /* was field_0x648: moveRndBack 02144DE4 / moveRndEscape 02147914 pick a new random mMoveTargetPos when it runs out (reload mRndMoveTime); modeUnderFoot 02144838 retargets the base target when it runs out (tww the same) */
    /* 0x7C8 */ be<s32> mHandShakeTimer;  /* was field_0x64C: checkTgHit 02145070 / modeBikubiku 02149818 set 0x28; _nodeControl 021404AC shakes the TE joint while fm_calcTimer counts it down; modeDamage 02147764 / modeBikubiku wait for 0 (tww the same) */
    /* 0x7CC */ be<s32> mModeTimer;  /* was field_0x650: the per-mode timer: loaded by modeHideInit 02144568, modeUnderFootInit 0214468C, modeAppearInit 02144D88, modeWaitInit 02146434, modeAttackInit 02146764, modeGrabInit 02147810, modeDamageInit 02147724, modeParalysisInit 02149688, modeBikubikuInit 021497C0; the modes act when fm_calcTimer reaches 0 (tww the same) */
    /* 0x7D0 */ be<s32> mSinkTimer;
    /* 0x7D4 */ be<s32> mFarTimer;  /* was field_0x658: modeWaitInit 02146434 / modeWait 021464B4 load HIO mFarTime; modeWait counts it down while the base target is beyond mNoticeRange and disappears at 0 (tww the same) */
    /* 0x7D8 */ be<s32> mGrabDelayTimer;  /* was field_0x65C: modeWaitInit 02146434 loads HIO mGrabDelayTime; modeWait 021464B4 attacks a target Link carries (or an NPC) only after it ran out (tww the same) */
    /* 0x7DC */ cXyz mMoveTargetPos;  /* was field_0x660: the XZ move target: moveRndBack 02144DE4 / moveRndEscape 02147914 (random point), modeUnderFoot 02144838 (base target), modeAttack 02146A34 (mGrabPos), modeGrabInit 02147810 (mGoalPos); the actor follows it with fm_addCalcPosXZ2; _execute 02143418 keeps its y at current.pos.y (tww the same) */
    /* 0x7E8 */ gptr<fopAc_ac_c> mpActorTarget;
    /* 0x7EC */ be<u32> mProcId;
    /* 0x7F0 */ cXyz mGrabPos;
    /* 0x7FC */ be<s16> mMoveAngle;  /* was field_0x680: moveRndBack 02144DE4 / moveRndEscape 02147914: the random direction of the next mMoveTargetPos (turned by 0x5000 plus a random 4096 step) (tww the same) */
    /* 0x7FE */ u8 _7FE[2];
    /* 0x800 */ be<s32> mGrabKind;  /* was field_0x684: what the hand holds: modeGrab 02147BE0 1 (Link), modeAttack 02146A34 2 (bomb) / 3 (pot) / 4 (NPC); _execute 02143418 calls grabPlayer / grabBomb / grabTsubo / grabNPC by it and clears it when the target is gone; cancelGrab 02142028, modeThrow 02147408 test it; createInit 02141930 clears it (tww the same) */
    /* 0x804 */ be<u8> field_0x688;
    /* 0x805 */ u8 _805;
    /* 0x806 */ be<s16> mHandShakePhase;  /* was field_0x68A: _nodeControl 021404AC: advanced by 0x1830 per frame while mHandShakeTimer runs, the sine phase of the TE joint shake (tww the same) */
    /* 0x808 */ be<s16> mHandShakeAmp;  /* was field_0x68C: checkTgHit 02145070 sets 0x2B00; _nodeControl 021404AC decays it to 0 (cLib_addCalcAngleS2) and rotates the TE joint by it * sin(mHandShakePhase) (tww the same) */
    /* 0x80A */ u8 _80A[2];
    /* 0x80C */ cXyz mGoalPos;  /* was field_0x690: createInit 02141930 / modeUnderFoot 02144838 / modePathMove 02144A18 / modeDisappear 02146310 store the position; modeGoalKeeper 02144B94 sets it to mHomePos and moves there; modeGrabInit 02147810 / moveRndEscape 02147914 start the escape from it (tww the same) */
    /* 0x818 */ cXyz mHomePos;  /* was field_0x69C: createInit 02141930 copies the start position; areaCheck 0214264C measures the area points from it; modeHide 021445B8 / modeUnderFoot 02144838 compare the base target height with its y; modeGoalKeeper 02144B94 returns to it (tww the same) */
    /* 0x824 */ be<u32> mProcId2;
    /* 0x828 */ u8 _828[8];
    /* 0x830 */ be<s32> field_0x6B4;
    /* 0x834 */ be<u8> field_0x6B8;
    /* 0x835 */ u8 _835[3];
    /* 0x838 */ Mtx34 mGrabMtx;  /* was field_0x6BC: grabPlayer 021429BC / grabNPC 02142C4C / grabTsubo 02142EF4 copy the hand matrix (mDoMtx_stack_c::get()) into it; debugDraw 021439A0 draws its axis under HIO mDebugGrabAxis (tww the same) */
    /* 0x868 */ gptr<JntHit_c> mpJntHit;
    /* 0x86C */ dCcD_Stts mStts;
    /* 0x8A8 */ dCcD_Stts mStts2;
    /* 0x8E4 */ dCcD_Sph mSph;
    /* 0xA10 */ dCcD_Cyl mCyl;
    /* 0xB40 */ be<s32> mTgHitTimer;  /* was field_0x9C4: checkTgHit 02145070 ignores hits while fm_calcTimer counts it down and reloads it (HIO mTgHitTime, or 0xC) after a hit (tww the same) */
    /* 0xB44 */ be<u8> mHitType;
    /* 0xB45 */ u8 _B45[3];
    /* 0xB48 */ gptr<fopAc_ac_c> mBaseTarget;
    /* 0xB4C */ be<s16> mBaseTargetAngleY;  /* was field_0x9D0: setBaseTarget 02141714: fopAcM_searchActorAngleY(this, mBaseTarget) (0 without one); turnToBaseTarget 02144A04, modeAppearInit 02144D88, modeWait 021464B4 turn to it (tww the same) */
    /* 0xB4E */ u8 _B4E[2];
    /* 0xB50 */ be<f32> mBaseTargetDist;  /* was field_0x9D4: setBaseTarget 02141714: fopAcM_searchActorDistanceXZ(this, mBaseTarget) (0 without one); _execute 02143418, modeUnderFoot 02144838, modePathMove 02144A18, modeGoalKeeper 02144B94, moveRndBack 02144DE4, modeWait 021464B4 compare it with the HIO ranges (tww the same) */
    /* 0xB54 */ dBgS_LinChk mLinChk;  /* dBgS_ObjLinChk */
    /* 0xBC0 */ u8 _BC0[4];
    /* 0xBC4 */ cXyz mAreaPoints[12];  /* was field_0xA48: areaCheck 0214264C: the 12 points at HIO mNearRange around the actor (every 0x1555), each checked against mAreaRadius and by a BG line (tww the same; tww debugDraw draws them) */
    /* 0xC54 */ be<u8> mAreaPointOk[12];  /* was field_0xAD8: areaCheck 0214264C: per point of mAreaPoints 1 while it is inside mAreaRadius and has ground under it (lineCheck hit), else 0 (tww the same) */
    /* 0xC60 */ be<u8> mInArea;  /* was field_0xAE4: _execute 02143418 stores areaCheck's result (all 12 mAreaPoints ok); modeHide 021445B8, modeUnderFoot 02144838, moveRndBack 02144DE4, modeAttack 02146A34, moveRndEscape 02147914 leave or stop moving without it (tww the same) */
    /* 0xC61 */ be<u8> mBaseTargetBlocked;  /* was field_0xAE5: _execute 02143418: 1 when the BG line from the actor to mBaseTarget (100 up) hits within mNoticeRange, 0 with mSwAppearDemo; modeUnderFoot 02144838 / modePathMove 02144A18 / modeGoalKeeper 02144B94 act only while it is 0 (tww the same) */
    /* 0xC62 */ u8 _C62[2];
    /* 0xC64 */ enemyice_l mEnemyIce;
};
WWHD_OFFSET(daFm_c, mMode, 0x3C8);
WWHD_OFFSET(daFm_c, mBtkAnm, 0x4E8);
WWHD_OFFSET(daFm_c, mHoleScale, 0x55C);
WWHD_OFFSET(daFm_c, mObjAcch, 0x580);
WWHD_OFFSET(daFm_c, mAcchCir, 0x744);
WWHD_OFFSET(daFm_c, mInvisibleModel, 0x784);
WWHD_OFFSET(daFm_c, mpJntHit, 0x868);
WWHD_OFFSET(daFm_c, mStts, 0x86C);
WWHD_OFFSET(daFm_c, mSph, 0x8E4);
WWHD_OFFSET(daFm_c, mCyl, 0xA10);
WWHD_OFFSET(daFm_c, mLinChk, 0xB54);
WWHD_OFFSET(daFm_c, mAreaPoints, 0xBC4);
WWHD_OFFSET(daFm_c, mEnemyIce, 0xC64);
WWHD_SIZE(daFm_c, 0x101C);

/* fganon_class (Phantom Ganon), WWHD layout. */
#ifndef WWHD_D_A_FGANON_H
#define WWHD_D_A_FGANON_H
#include "bindings.h"
#include "gc_api/tu.h"

// HD: tww actor members are shifted by +0x11C through mKenTevStr, then +0x234.
// HD adds the previous-position and node-rotation fields at the end.
struct fganon_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhs1;
    /* 0x3D0 */ request_of_phase_process_class mPhs2;
    /* 0x3D8 */ be<u8> mType;  /* was m2BC: daFganon_Create 02137F9C: parameter bits 0-3 (zeldaret/tww names the HIO child Phantom Ganon 1 / 2 / 3 for 0 / 1 / 2; 3 = a spin-attack clone, which reads mCloneNo); down 02139680 / standby, shot, move (in move 0213B3AC) branch on it (camera type, light-arrow checks) */
    /* 0x3D9 */ be<u8> mStartDist;  /* was m2BD: daFganon_Create 02137F9C: parameter byte 1; start 02139AA0 and standby (in move 0213B3AC) begin when the player's XZ distance to home is below it * 10 */
    /* 0x3DA */ be<u8> mSwitchNo;
    /* 0x3DB */ be<u8> mBattleSwitchNo;  /* was m2BF: daFganon_Create 02137F9C: parameter byte 3; demo_camera (in daFganon_Execute 02134B28) turns this switch on when the opening demo hands over to the battle and off at the end of the defeat demo (0xFF = none) */
    /* 0x3DC */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3E0 */ gptr<mDoExt_baseAnm> mpBrkAnm1;
    /* 0x3E4 */ gptr<J3DModel> mpKenModel;
    /* 0x3E8 */ gptr<mDoExt_baseAnm> mpBrkAnm2;
    /* 0x3EC */ be<s8> mKenState;  /* was m2D0: last_end 0213A228 sets 1 (sword no longer follows the hand) and 2 (sword handed to the Boko actor); execute_model (in daFganon_Execute 02134B28) places mpKenModel only at 0, daFganon_Draw 02134428 draws it unless 2 */
    /* 0x3ED */ be<u8> m2D1[0x2D4 - 0x2D1];
    /* 0x3F0 */ dKy_tevstr_c mKenTevStr;
    /* 0x5B8 */ be<s16> mFrameCounter;  /* was m384: execute_model (in daFganon_Execute 02134B28) adds 1 per frame and uses it as the phase of the sway and hit-shake sines; fly_common (in move 0213B3AC) samples every 16th frame */
    /* 0x5BA */ be<s16> mAction;
    /* 0x5BC */ be<s16> mMode;
    /* 0x5BE */ be<s16> m38A;
    /* 0x5C0 */ cXyz mTargetPos;  /* was m38C: fly_common (in move 0213B3AC): player position + offset; spinattack2 02139194: player position; pos_move 02134840 turns current.angle toward it */
    /* 0x5CC */ be<s16> mTargetAngleY;  /* was m398: fly_common (in move 0213B3AC): the player's shape_angle.y, re-sampled at random every 16 frames; YrotS by it to place mTargetPos around the player */
    /* 0x5CE */ be<s16> m39A;
    /* 0x5D0 */ be<f32> mTurnSpeed;  /* was m39C: fly_common (in move 0213B3AC) sets 1500 (+ reg); pos_move 02134840: angle step = it * mTurnSpeedRate */
    /* 0x5D4 */ be<f32> mTurnSpeedRate;  /* was m3A0: fly_common (in move 0213B3AC) clears it, pos_move 02134840 eases it to 1 and multiplies mTurnSpeed by it */
    /* 0x5D8 */ be<s16> mTimers[5];  /* was m3A4: daFganon_Execute 02134B28 counts all 5 down each frame; the actions load them (start 02139AA0 [0] = 0x50, down 02139680 [0] = l_HIO.m48, spinattack2 02139194 [0] = 40) */
    /* 0x5E2 */ be<s16> mNoDamageTimer;  /* was m3AE: damage_check 0213A6FC returns while it is not 0 and sets 6 / 2 / 1000 after a hit; the demo and vanish actions (start 02139AA0, down 02139680, deru in move 0213B3AC) keep it at 3 / 5; execute_model (in daFganon_Execute 02134B28) counts it down */
    /* 0x5E4 */ be<f32> mKnockbackDist;  /* was m3B0: knockDown (in damage_check 0213A6FC) sets 80 (+ reg); move 0213B3AC moves the actor by it along mKnockbackAngleY / X and eases it to 0 */
    /* 0x5E8 */ be<s16> mKnockbackAngleY;  /* was m3B4: knockDown (in damage_check 0213A6FC): atan2 from the player to the actor; move 0213B3AC YrotS by it for the knockback */
    /* 0x5EA */ be<s16> mKnockbackAngleX;  /* was m3B6: knockDown (in damage_check 0213A6FC): -atan2 of the height difference over the XZ distance; move 0213B3AC XrotM by it for the knockback */
    /* 0x5EC */ be<s16> mHitShakeTimer;  /* was m3B8: knockDown (in damage_check 0213A6FC) sets 7 (+ reg); execute_model (in daFganon_Execute 02134B28) counts it down and shakes the model's Y / X rotation by it * 500 */
    /* 0x5EE */ be<u8> m3BA[0x3BC - 0x3BA];
    /* 0x5F0 */ be<f32> mSwayAmplitude;  /* was m3BC: fly_common (in move 0213B3AC) eases it to 50; execute_model (in daFganon_Execute 02134B28) offsets the model by it * sin / cos of mFrameCounter and eases it to 0 */
    /* 0x5F4 */ be<f32> mFadeFogEndZ;  /* was m3C0: last_end 0213A228: sets 10000 / 5100, eases it to 100 and copies it to tevStr.mFogEndZ (the white fade-out) */
    /* 0x5F8 */ gptr<void> mEmitters1[2];
    /* 0x600 */ gptr<void> mEmitters2[2];
    /* 0x608 */ gptr<void> mEmitters3[2];
    /* 0x610 */ be<u32> m3DC;
    /* 0x614 */ cXyz mBallPos;  /* was m3E0: energy_ball_move (in daFganon_Execute 02134B28): the energy ball's position (hand joint, then + mBallVelocity); ball_bg_check line-checks from it; shot (in move 0213B3AC) plays the ball sound there */
    /* 0x620 */ cXyz mBallPosOld;  /* was m3EC: energy_ball_move (in daFganon_Execute 02134B28) copies mBallPos before moving it; ball_bg_check checks the line from it */
    /* 0x62C */ cXyz mBallVelocity;  /* was m3F8: aimEnergyBall (in daFganon_Execute 02134B28): MtxPosition of (0, 0, mBallSpeed) toward the target; energy_ball_move adds it to mBallPos and takes the effect angle from it */
    /* 0x638 */ be<f32> mBallSpeed;  /* was m404: aimEnergyBall (in daFganon_Execute 02134B28): l_HIO.m1C / m20 when thrown, kept on each return; energy_ball_move adds l_HIO.m24 / m28 per return; shot (in move 0213B3AC) scales the swing distance by it */
    /* 0x63C */ be<s8> mBallState;  /* was m408: energy_ball_move (in daFganon_Execute 02134B28): 0 none, 1 create the ball effects, 2 held in the hand, 3 thrown, 4 flying at the player, 5 sent back at Phantom Ganon, 35 remove; shot (in move 0213B3AC) / start 02139AA0 set 1, damage_check 0213A6FC / kieru set 35 */
    /* 0x63D */ be<s8> mBallThrow;  /* was m409: shot (in move 0213B3AC) sets 1 at the throw frame; energy_ball_move (in daFganon_Execute 02134B28) then aims the ball, sets mBallState 3 and clears it */
    /* 0x63E */ be<s8> mBallNoHitTimer;  /* was m40A: energy_ball_move (in daFganon_Execute 02134B28) sets 5 after each return (50 when the third form vanishes) and counts it down; while it runs the ball's colliders are moved away and hits are ignored */
    /* 0x63F */ be<s8> m40B;
    /* 0x640 */ dCcD_Sph mBallTgSph;
    /* 0x76C */ dCcD_Sph mBallAtSph;
    /* 0x898 */ cXyz mDarkBallPos;  /* was m664: tama_set (in shot2 02138670): the dark ball's position in front of the actor (mDarkBallDist); shot2 creates the 8 fgmahou balls there */
    /* 0x8A4 */ be<s8> mMahouHitCount;  /* was m670: shot2 02138670 clears it; damage_check 0213A6FC adds 1 per hit by an fgmahou ball and knocks Phantom Ganon down at 5 */
    /* 0x8A5 */ be<s8> mDarkBallState;  /* was m671: shot2 02138670 sets 1 / 3; tama_set (in shot2) 1: create the effects, 3: burst, 4: fade (mDarkBallAlpha), 5..9 then 0; daFganon_Draw 02134428 draws the dark ball model while it is not 0 */
    /* 0x8A6 */ be<u8> mDarkBallAlpha;  /* was m672: tama_set (in shot2 02138670): 250 when the ball appears, -50 per frame after the burst; the emitters' global alpha; 0 removes them */
    /* 0x8A7 */ be<s8> m673;
    /* 0x8A8 */ gptr<J3DModel> mpEnergySphereModel;
    /* 0x8AC */ gptr<mDoExt_baseAnm> mpBtkAnm;
    /* 0x8B0 */ gptr<mDoExt_baseAnm> mpBrkAnm3;
    /* 0x8B4 */ be<f32> mDarkBallDist;  /* was m680: shot2 02138670 clears it and eases it to 250 at the throw; tama_set (in shot2) places mDarkBallPos it forward and it down */
    /* 0x8B8 */ be<s8> mWeponAtStarted;  /* was m684: execute_collision (in daFganon_Execute 02134B28): set 1 after StartCAt of mWeponSph, MoveCAt while set; cleared when the sword attack is off */
    /* 0x8B9 */ be<s8> mWeponAtOn;  /* was m685: spinattack2 02139194 / spinattack (in move 0213B3AC) set 1 each attack frame; execute_collision (in daFganon_Execute 02134B28) places mWeponSph on the sword while set and clears it */
    /* 0x8BA */ be<s8> mBallSwing;  /* was m686: shot (in move 0213B3AC) sets 1 on swing frames 5..15; energy_ball_move (in daFganon_Execute 02134B28) sends a near ball back at the player while it is set; execute_collision clears it each frame */
    /* 0x8BB */ be<s8> mSwingCount;  /* was m687: shot (in move 0213B3AC): cleared at the throw, +1 per swing at the ball; from 4 swings it may stop returning (mStopReturn) */
    /* 0x8BC */ be<s8> mRallyCount;  /* was m688: shot (in move 0213B3AC) clears it; energy_ball_move (in daFganon_Execute 02134B28) adds 1 per return by the player and passes it + 2 (max 7) to mDoAud_changeSubBgmStatus; shot stops returning at 7 */
    /* 0x8BD */ be<s8> mStopReturn;  /* was m689: shot (in move 0213B3AC): cleared at the throw, set when mRallyCount reaches 7 or at random after 4 swings; while set the ball is no longer swung back */
    /* 0x8BE */ be<s8> mLaughTimer;  /* was m68A: execute_model (in daFganon_Execute 02134B28) sets 60 with the WARAU1 laugh and counts it down; shot2 02138670 / fly_common (in move 0213B3AC) return to WAIT1 at 1 */
    /* 0x8BF */ be<s8> mLaughRequest;  /* was m68B: energy_ball_move (in daFganon_Execute 02134B28) sets 1 when the ball hits the player; d_a_fgmahou (view FgmahouBossHD) sets 1 when a dark ball hits; execute_model starts the laugh and clears it */
    /* 0x8C0 */ be<s8> mBallHitSelf;  /* was m68C: energy_ball_move (in daFganon_Execute 02134B28) sets 1 when a returned ball reaches Phantom Ganon; damage_check 0213A6FC takes it as a hit; move 0213B3AC clears it */
    /* 0x8C1 */ be<s8> mbIsMaterialized;
    /* 0x8C2 */ be<s8> mCapeOff;  /* was m68E: last_end 0213A228 sets 1; execute_collision (in daFganon_Execute 02134B28) eases the cape's scale.y to 0 while it is set */
    /* 0x8C3 */ be<u8> mCloneNo;  /* was m68F: daFganon_Create 02137F9C: parameter bits 4-7 when mType is 3 (spinattack2 02139194 creates clones 1..4 with (i * 16) | 3); 0 = the real Phantom Ganon; spinattack2 / damage_check 0213A6FC / kieru branch on it */
    /* 0x8C4 */ be<s8> mDownHealth;  /* was m690: damage_check 0213A6FC: sword damage is taken off it while Phantom Ganon is down; at 0 it is reset to 10 and he vanishes */
    /* 0x8C5 */ be<u8> m691[0x694 - 0x691];
    /* 0x8C8 */ be<f32> mSwordFallAngle;  /* was m694: fail (in move 0213B3AC): + mSwordFallSpeed per frame up to 15884 (sword lying flat); execute_model (in daFganon_Execute 02134B28) XrotM of the sword by it */
    /* 0x8CC */ be<f32> mSwordFallSpeed;  /* was m698: fail (in move 0213B3AC): + mSwordFallAccel per frame; on reaching the ground it bounces back (* -0.4) while above 700 */
    /* 0x8D0 */ be<f32> mSwordFallAccel;  /* was m69C: fail (in move 0213B3AC): + 3 per frame up to a limit; added to mSwordFallSpeed and to the tilt step */
    /* 0x8D4 */ be<s16> mSwordFallDir;  /* was m6A0: fail (in move 0213B3AC): angle from the sword to the wall line cross (+ 0x7058); execute_model (in daFganon_Execute 02134B28) ZrotM by it and its negative around the fall */
    /* 0x8D6 */ be<s16> mSwordTilt;  /* was m6A2: fail (in move 0213B3AC) eases it toward mSwordFallDir by mSwordFallAccel + mSwordTiltStep; execute_model (in daFganon_Execute 02134B28) ZrotM of the sword by it */
    /* 0x8D8 */ be<s16> mSwordTiltStep;  /* was m6A4: fail (in move 0213B3AC): 0x80, 0x800 once the sword lies flat; added to the cLib_addCalcAngleS2 step of mSwordTilt */
    /* 0x8DA */ be<s16> mSwordShakeTimer;  /* was m6A6: fail (in move 0213B3AC) sets 30 (+ reg) on a bounce; execute_model (in daFganon_Execute 02134B28) shakes the sword by it * sin and counts it down */
    /* 0x8DC */ be<u32> mSwordFallSeVolume;  /* was m6A8: fail (in move 0213B3AC): 100, passed to the sword-fall sound 0x6998 on each bounce, -20 per bounce down to 30 */
    /* 0x8E0 */ be<s8> mBtAttackTimer;  /* was m6AC: spinattack2 02139194 / spinattack (in move 0213B3AC) set l_HIO.m40 - m3E; execute_collision (in daFganon_Execute 02134B28) counts it down and sets the bt attack data (frames 0..10) while it runs */
    /* 0x8E1 */ be<u8> m6AD[0x6B0 - 0x6AD];
    /* 0x8E4 */ be<u32> mBokoID;
    /* 0x8E8 */ be<u32> mCapeID;
    /* 0x8EC */ dBgS_AcchCir mAcchCir;
    /* 0x92C */ dBgS_ObjAcch mAcch;
    /* 0xAF0 */ dCcD_Stts mStts;
    /* 0xB2C */ dCcD_Cyl mCyl;
    /* 0xC5C */ dCcD_Sph mWeponSph;
    /* 0xD88 */ be<s16> mDemoCamMode;  /* was mB54: start 02139AA0 sets 1, end 02139E80 50, last_end 0213A228 100 (+1 per step); demo_camera (in daFganon_Execute 02134B28) switches on it; cameraReset clears it */
    /* 0xD8A */ be<s16> mDemoTimer;  /* was mB56: demo_camera (in daFganon_Execute 02134B28) clears it per step, adds 1 per frame and compares it with frame marks (2, 30, 45, 110, 280); last_end 0213A228 clears it */
    /* 0xD8C */ be<u8> mB58[0xB5C - 0xB58];
    /* 0xD90 */ cXyz mDemoCamEye;  /* was mB5C: demo_camera (in daFganon_Execute 02134B28): set to fixed points / eased; passed to dCamera_c::Set as the eye */
    /* 0xD9C */ cXyz mDemoCamCenter;  /* was mB68: demo_camera (in daFganon_Execute 02134B28): set to fixed points, the actor's position or eyePos; passed to dCamera_c::Set as the centre */
    /* 0xDA8 */ be<s16> mB74;
    /* 0xDAA */ be<s16> mDemoCamAngleY;  /* was mB76: demo_camera (in daFganon_Execute 02134B28): + 30 per frame, added to the player's shape_angle.y to orbit mDemoCamEye around the player */
    /* 0xDAC */ be<u8> mB78[0xB80 - 0xB78];
    /* 0xDB4 */ be<f32> mDemoCamSpeed;  /* was mB80: demo_camera (in daFganon_Execute 02134B28): cleared per step, eased to 0.1 / 1; scales the cLib_addCalc2 steps of mDemoCamEye / mDemoCamCenter */
    /* 0xDB8 */ be<f32> mDemoCamFovy;  /* was mB84: demo_camera (in daFganon_Execute 02134B28): 55 or 60; passed to dCamera_c::Set as the fovy */
    /* 0xDBC */ be<u8> mHioOwner;  /* was mB88: daFganon_Create 02137F9C sets 1 when it created the l_HIO child; daFganon_Delete 02137810 deletes the HIO child only then */
    /* 0xDBD */ be<s8> mBgmStartTimer;  /* was mB89: standby (in move 0213B3AC) sets 22; execute_model (in daFganon_Execute 02134B28) counts it down and starts sub BGM 0x80000047 at 1; daFganon_Delete 02137810 stops the sub BGM when it is not 0 */
    /* 0xDBE */ be<u8> mResLoadState;  /* was mB8A: daFganon_Create 02137F9C: 1 after requesting "Pgsw", 2 after requesting "Fganon"; daFganon_Delete 02137810 releases Pgsw from 1 and Fganon too from 2 */
    /* 0xDBF */ be<u8> mB8B;
    // HD: previous rendered position drives the added joint rotation callback.
    /* 0xDC0 */ cXyz mHdPreviousPos;
    /* 0xDCC */ be<s16> mHdNodeY;
    /* 0xDCE */ be<s16> mHdNodeZ;
};
WWHD_SIZE(fganon_class, 0xDD0);
WWHD_OFFSET(fganon_class, mpMorf, 0x3DC);
WWHD_OFFSET(fganon_class, mMode, 0x5BC);
WWHD_OFFSET(fganon_class, mWeponSph, 0xC5C);

// HD: HIO data starts before its vtable; GC member names retain their original suffixes.
struct daFganon_HIO_c {
    be<s8> mNo;
    be<u8> m05;
    be<u8> m06;
    be<u8> m07;
    be<s16> m08;
    be<s16> m0A;
    be<f32> m0C;
    be<f32> m10;
    be<f32> m14;
    be<f32> m18;
    be<f32> m1C;
    be<f32> m20;
    be<f32> m24;
    be<f32> m28;
    be<f32> m2C;
    be<f32> m30;
    be<f32> m34;
    be<f32> m38;
    be<s16> m3C;
    be<s16> m3E;
    be<s16> m40;
    be<s16> m42;
    be<f32> m44;
    be<s16> m48;
    u8 _pad46[2];
    be<u32> __vtbl;
};
WWHD_SIZE(daFganon_HIO_c, 0x4C);
struct FganonDebugRegisters {
    u8 _pad[8];
    be<f32> f[30];
    be<s16> s[8];
};
WWHD_SIZE(FganonDebugRegisters, 0x90);

template <class T> inline T *fgView(u32 address) {
    return gabi::at<T>(address);
}
struct FganonModelFlags {
    u8 _pad[0x74];
    be<u32> mFlags;
};
struct FganonJointInfo {
    u8 _pad[4];
    be<u16> mJointNo;
};
struct FganonEmitter {
    u8 _pad0[0x5C];
    be<s32> mMaxFrame;
    u8 _pad60[0x1F0 - 0x60];
    be<f32> mGlobalRotation[9];
    u8 _pad214[0x22C - 0x214];
    cXyz mGlobalTranslation;
    u8 _pad238[0x247 - 0x238];
    be<u8> mGlobalAlpha;
    u8 _pad248[0x254 - 0x248];
    be<u32> mStatus;
    u8 _pad258[0x262 - 0x258];
    be<u8> mParticleCount;
};
struct FganonAttackInfo {
    be<u32> mpObj;
    be<u32> mpActor;
    be<u8> mDamage;
    be<u8> mbDead;
    be<u8> mResultingAttackType;
    u8 _padB[9];
    be<u32> pParticlePos;
    be<u32> mHdResult;
};
WWHD_SIZE(FganonAttackInfo, 0x1C);
struct FganonPlayerDemo {
    u8 _pad[0x420];
    be<u16> mDemoType;
    u8 _pad422[6];
    be<u32> mDemoParam;
    u8 _pad42C[4];
    be<u32> mDemoMode;
};
struct FganonPlayerCut {
    u8 _pad[0x69E8];
    be<u8> mCutType;
};
struct FganonBoko {
    u8 _pad[0x3B4];
    be<u32> mpModel;
    u8 _pad3B8[0x434 - 0x3B8];
    be<u8> mFloorFlg;
};
struct FganonCape {
    u8 _pad[0x362C];
    cXyz mShoulderPosR;
    cXyz mShoulderPosL;
};

struct FganonCameraView {
    u8 _pad[0xDC];
    cXyz mEye;
    cXyz mCenter;
    u8 _padF4[0x248 - 0xF4];
    be<u8> mBody; // HD: camera body follows the view storage.
};
struct FganonSaveView {
    u8 _hdHeader[0x20];
    dSv_info_c mSave;
};
struct FganonSaveEquipment {
    u8 _pad[0x2E];
    be<u8> mSelectEquip;
};
struct FganonJointTree {
    u8 _pad[8];
    be<u16> mJointCount;
};
struct FganonLightDefaults {
    be<f32> mPositionDirection[6];
    be<u8> mColor[4];
    be<s16> mAngles[4];
    be<f32> mFactors[8];
};
struct FganonHeaderZeros {
    be<u32> values[4];
};
struct FganonHeaderPi {
    be<f32> values[2];
};
struct FganonPlayerVirtuals {
    u8 _pad[0x64];
    be<u32> checkBottleSwing;
    u8 _pad68[0x114 - 0x68];
    be<u32> setPlayerPosAndAngle;
};
inline be<u32> &fgVectorWord(cXyz *v, u32 i) {
    return *fgView<be<u32>>(gabi::ea(v) + i * 4);
}
inline be<u16> &fgAngleWord(csXyz *v, u32 i) {
    return *fgView<be<u16>>(gabi::ea(v) + i * 2);
}
inline be<f32> &fgVectorFloat(cXyz *v, u32 i) {
    return *fgView<be<f32>>(gabi::ea(v) + i * 4);
}
inline be<f32> &fgMatrixFloat(u32 m, u32 i) {
    return fgView<Mtx34>(m)->m[i / 4][i % 4];
}
inline gptr<void> &fgEmitterAt(fganon_class *a, u32 i) {
    if (i < 2)
        return a->mEmitters1[i];
    if (i < 4)
        return a->mEmitters2[i - 2];
    return a->mEmitters3[i - 4];
}

struct FganonCylinderShapeVtables {
    u8 _pad[0x110];
    be<u32> mVtable;
    be<u32> mShapeVtable;
    u8 _pad118[0x130 - 0x118];
};
struct FganonCylinderGeometryVtable {
    u8 _pad[0x12C];
    be<u32> mVtable;
};


// HD views used where the complete external class is unnecessary.
WWHD_OFFSET(FganonModelFlags, mFlags, 0x74);
WWHD_OFFSET(FganonJointInfo, mJointNo, 0x4);
WWHD_OFFSET(FganonEmitter, mMaxFrame, 0x5C);
WWHD_OFFSET(FganonEmitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(FganonEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(FganonEmitter, mGlobalAlpha, 0x247);
WWHD_OFFSET(FganonEmitter, mStatus, 0x254);
WWHD_OFFSET(FganonEmitter, mParticleCount, 0x262);
WWHD_OFFSET(FganonAttackInfo, pParticlePos, 0x14);
WWHD_OFFSET(FganonAttackInfo, mHdResult, 0x18);
WWHD_OFFSET(FganonPlayerDemo, mDemoType, 0x420);
WWHD_OFFSET(FganonPlayerDemo, mDemoParam, 0x428);
WWHD_OFFSET(FganonPlayerDemo, mDemoMode, 0x430);
WWHD_OFFSET(FganonPlayerCut, mCutType, 0x69E8);
WWHD_OFFSET(FganonBoko, mpModel, 0x3B4);
WWHD_OFFSET(FganonBoko, mFloorFlg, 0x434);
WWHD_OFFSET(FganonCape, mShoulderPosR, 0x362C);
WWHD_OFFSET(FganonCape, mShoulderPosL, 0x3638);
WWHD_OFFSET(FganonCameraView, mEye, 0xDC);
WWHD_OFFSET(FganonCameraView, mCenter, 0xE8);
WWHD_OFFSET(FganonCameraView, mBody, 0x248);
WWHD_OFFSET(FganonSaveView, mSave, 0x20);
WWHD_OFFSET(FganonSaveEquipment, mSelectEquip, 0x2E);
WWHD_OFFSET(FganonJointTree, mJointCount, 0x8);
WWHD_OFFSET(FganonPlayerVirtuals, checkBottleSwing, 0x64);
WWHD_OFFSET(FganonPlayerVirtuals, setPlayerPosAndAngle, 0x114);
WWHD_OFFSET(FganonCylinderShapeVtables, mVtable, 0x110);
WWHD_OFFSET(FganonCylinderShapeVtables, mShapeVtable, 0x114);
WWHD_OFFSET(FganonCylinderGeometryVtable, mVtable, 0x12C);
WWHD_SIZE(FganonLightDefaults, 0x44);
WWHD_SIZE(FganonHeaderZeros, 0x10);
WWHD_SIZE(FganonHeaderPi, 8);

// HD guest ABI bindings used by this unit.
u32 fg_cBgS_LineCross(u32 a0, u32 a1);
f32 fg_cBgS_GroundCross(u32 a0, u32 a1);
void fg_cBgS_LinChk_dtor(u32 a0, u32 a1);
void fg_cBgS_Chk_cBgS_Chk(u32 a0, u32 a1);
void fg_cCcD_Stts_ctor(u32 a0);
void fg_cCcS_Set(u32 a0, u32 a1);
void fg_cLib_addCalc2(u32 a0, f32 a1, f32 a2, f32 a3);
void fg_cLib_addCalc0(u32 a0, f32 a1, f32 a2);
void fg_cLib_addCalcAngleS2(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_MtxTrans(u32 a0, f32 a1, f32 a2, f32 a3);
void fg_MtxPosition(u32 a0, u32 a1);
void fg_cM3dGCir_dtor(u32 a0, u32 a1);
void fg_cM3dGCyl_SetC(u32 a0, u32 a1);
void fg_cM3dGCyl_SetH(u32 a0, f32 a1);
void fg_cM3dGCyl_SetR(u32 a0, f32 a1);
void fg_cM3dGCyl_ctor(u32 a0);
void fg_cM3dGSph_SetR(u32 a0, f32 a1);
void fg_cM3dGSph_SetC(u32 a0, u32 a1);
u32 fg_cM_atan2s(f32 a0, f32 a1);
f32 fg_cM_rndF(f32 a0);
f32 fg_cM_rndFX(f32 a0);
void fg_csXyz_csXyz(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_cXyz___pl(u32 a0, u32 a1, u32 a2);
void fg_cXyz___mi(u32 a0, u32 a1, u32 a2);
void fg_cXyz___ml(u32 a0, u32 a1, f32 a2);
void fg_daFganon_Execute_inlines_demo_camera(u32 a0);
u32 fg_fopAcM_SearchByID(u32 a0);
void fg_JPABaseEmitter_setFlags(u32 a0, u32 a1);
u32 fg_emptyVirtual(u32 a0);
void fg_dBgS_Acch_dtor(u32 a0, u32 a1);
void fg_dBgS_AcchCir_dBgS_AcchCir(u32 a0);
void fg_dBgS_AcchCir_SetWall(u32 a0, f32 a1, f32 a2);
void fg_dBgS_Acch_dBgS_Acch(u32 a0);
void fg_dBgS_Acch_Set(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8);
void fg_dBgS_Acch_CrrPos(u32 a0, u32 a1);
void fg_dBgS_LinChk_Set(u32 a0, u32 a1, u32 a2, u32 a3);
u32 fg_dCam_getBody();
void fg_dCamera_c_SetTypeForce(u32 a0, u32 a1, u32 a2);
void fg_dCamera_c_Stop(u32 a0);
void fg_dCamera_c_Start(u32 a0);
void fg_dCamera_c_Set(u32 a0, u32 a1, u32 a2, f32 a3, u32 a4);
void fg_dCamera_c_SetTrimSize(u32 a0, u32 a1);
void fg_dCcD_Stts_dtor(u32 a0, u32 a1);
void fg_dCcD_Cyl_dtor(u32 a0, u32 a1);
void fg_dCcD_Sph_dtor(u32 a0, u32 a1);
void fg_dCcD_GStts_dCcD_GStts(u32 a0);
void fg_dCcD_GStts_Move(u32 a0);
void fg_dCcD_Stts_Init(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_dCcD_GObjInf_dCcD_GObjInf(u32 a0);
void fg_dCcD_GObjInf_ClrAtHit(u32 a0);
u32 fg_dCcD_GObjInf_ChkAtHit(u32 a0);
u32 fg_dCcD_GObjInf_GetAtHitObj(u32 a0);
u32 fg_dCcD_GObjInf_ChkTgHit(u32 a0);
u32 fg_dCcD_GObjInf_GetTgHitObj(u32 a0);
void fg_dCcD_Cyl_Set(u32 a0, u32 a1);
void fg_dCcD_Sph_ctor(u32 a0);
void fg_dCcD_Sph_Set(u32 a0, u32 a1);
void fg_dCcD_Sph_StartCAt(u32 a0, u32 a1);
void fg_dCcD_Sph_MoveCAt(u32 a0, u32 a1);
u32 fg_at_power_check1(u32 a0);
void fg_at_power_check2(u32 a0);
u32 fg_cc_at_check(u32 a0, u32 a1);
u32 fg_dComIfGp_getGameInfo();
void fg_dComIfGp_getGameInfoDiscard();
u32 fg_dComIfG_resLoad(u32 a0, u32 a1);
void fg_dComIfG_resDelete(u32 a0, u32 a1);
u32 fg_dComIfGp_getReverb(u32 a0);
u32 fg_dComIfGs_checkGetItem(u32 a0);
u32 fg_dComIfGp_getEnvLight();
void fg_dKy_SordFlush_set(u32 a0, u32 a1);
void fg_dScnKy_env_light_c_settingTevStruct(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_setLightTevColorType(u32 a0, u32 a1, u32 a2);
void fg_dMeter_mtrShow();
u32 fg_dPa_control_c_set1(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8,
                          u32 a9, u32 a10, u32 a11);
void fg_dPa_control_c_set2(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8,
                           u32 a9, u32 a10, u32 a11);
void fg_dSv_event_c_onEventBit(u32 a0, u32 a1);
u32 fg_dSv_event_c_isEventBit(u32 a0, u32 a1);
u32 fg_dSv_info_c_isSwitch(u32 a0, u32 a1, u32 a2);
void fg_dSnap_RegistFig(u32 a0, u32 a1, f32 a2, f32 a3, f32 a4);
u32 fg_fopAc_IsActor(u32 a0);
void fg_fopAc_ac_c_fopAc_ac_c(u32 a0);
void fg_fopAc_ac_c_dtor(u32 a0, u32 a1);
u32 fg_fopAcIt_Judge(u32 a0, u32 a1);
void fg_fopAcM_delete(u32 a0);
void fg_fopAcM_create1(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7);
u32 fg_fopAcM_create2(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7);
u32 fg_fopAcM_entrySolidHeap(u32 a0, u32 a1, u32 a2);
u32 fg_fopAcM_searchActorAngleY(u32 a0, u32 a1);
void fg_fopAcM_orderPotentialEvent(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_fopAcM_setCarryNow(u32 a0, u32 a1);
void fg_fopAcM_cancelCarryNow(u32 a0);
void fg_fpcEx_Search(u32 a0, u32 a1);
void fg_mDoAud_subBgmStart(u32 a0);
void fg_mDoAud_subBgmStop();
void fg_mDoAud_bgmAllMute(u32 a0);
void fg_mDoAud_seStart(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_mDoAud_monsSeStartPos(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_mDoAud_monsSeStart(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4);
void fg_JAIZelBasic_seDeleteObject(u32 a0);
void fg_mDoAud_changeSubBgmStatus(u32 a0);
void fg_mDoExt_modelUpdateDL(u32 a0, u32 a1);
u32 fg_mDoExt_J3DModel__create(u32 a0, u32 a1, u32 a2);
void fg_mDoExt_McaMorf_setAnm(u32 a0, u32 a1, u32 a2, f32 a3, f32 a4, f32 a5, f32 a6, u32 a7);
u32 fg_mDoExt_McaMorf_mDoExt_McaMorf(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, f32 a6, u32 a7,
                                     u32 a8, u32 a9, u32 a10, u32 a11, u32 a12);
void fg_mDoExt_McaMorf_play(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_mDoExt_McaMorf_entryDL(u32 a0);
void fg_mDoExt_McaMorf_calc(u32 a0);
void fg_mDoExt_baseAnm_play(u32 a0);
void fg_mDoExt_brkAnm_initDiscard(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, f32 a5, u32 a6, u32 a7,
                                  u32 a8, u32 a9);
u32 fg_mDoExt_brkAnm_initResult(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, f32 a5, u32 a6, u32 a7,
                                u32 a8, u32 a9);
u32 fg_mDoHIO_subRoot_c_createChild(u32 a0, u32 a1);
void fg_mDoHIO_subRoot_c_deleteChild(u32 a0);
void fg_mDoMtx_YrotS(u32 a0, u32 a1);
void fg_mDoMtx_XrotM(u32 a0, u32 a1);
void fg_mDoMtx_YrotM(u32 a0, u32 a1);
void fg_mDoMtx_ZrotM(u32 a0, u32 a1);
u32 fg_operatorNew(u32 a0);
void fg_dl(u32 a0);
void fg_JUTReport(u32 a0, u32 a1, u32 a2, u32 a3);
u32 fg_J3DModelData_getJointTree(u32 a0);
void fg_J3DModel_setFlags(u32 a0, u32 a1);
u32 fg_J3DJoint_getJointInfo(u32 a0);
void fg_JPASetRMtxTVecfromMtx(u32 a0, u32 a1, u32 a2);
void fg_PSVECAdd(u32 a0, u32 a1, u32 a2);
f32 fg_PSVECSquareMag(u32 a0);
void fg_PSMTXCopy(u32 a0, u32 a1);
void fg_PSMTXTrans(u32 a0, f32 a1, f32 a2, f32 a3);
void fg_cSGlobe_ctor(u32 a0);
void fg_cSAngle_ctor(u32 a0);
void fg_register_global_object(u32 a0);
f32 fg_sqrtf(f32 a0);
void fg_mDoExt_brkAnm_entry(u32 animation, u32 data, f32 frame);
void fg_mDoExt_btkAnm_entry(u32 animation, u32 data, f32 frame);
u32 fg_mDoExt_brkAnm_ctor(u32 self);
u32 fg_mDoExt_btkAnm_ctor(u32 self);
BOOL fg_mDoExt_brkAnm_init(u32 animation, u32 data, u32 resource, u32 enable, s32 loop, f32 speed,
                           s32 start, s32 end, u32 modify, u32 allocator);
BOOL fg_mDoExt_btkAnm_init(u32 animation, u32 data, u32 resource, u32 enable, s32 loop, f32 speed,
                           s32 start, s32 end, u32 modify, u32 allocator);
void fg_dSv_info_c_onSwitch(u32 self, u32 index, s32 room);
void fg_dSv_info_c_offSwitch(u32 self, u32 index, s32 room);

// Resource indices shared with the GameCube archives.
enum dRes_INDEX_FGANON {
    /* BCK  */
    dRes_INDEX_FGANON_BCK_DAMAGE1_e = 0x6,
    dRes_INDEX_FGANON_BCK_DAMAGE2_e = 0x7,
    dRes_INDEX_FGANON_BCK_DOWN1_e = 0x8,
    dRes_INDEX_FGANON_BCK_KAITEN1_e = 0x9,
    dRes_INDEX_FGANON_BCK_KAITEN_S1_e = 0xA,
    dRes_INDEX_FGANON_BCK_LAST_DAMAGE1_e = 0xB,
    dRes_INDEX_FGANON_BCK_MOGAKU1_e = 0xC,
    dRes_INDEX_FGANON_BCK_NAGERU1_e = 0xD,
    dRes_INDEX_FGANON_BCK_NAGERU_S1_e = 0xE,
    dRes_INDEX_FGANON_BCK_SARU1_e = 0xF,
    dRes_INDEX_FGANON_BCK_TAME1_e = 0x10,
    dRes_INDEX_FGANON_BCK_TAME_S1_e = 0x11,
    dRes_INDEX_FGANON_BCK_TAME_S2_e = 0x12,
    dRes_INDEX_FGANON_BCK_TENNISLR1_e = 0x13,
    dRes_INDEX_FGANON_BCK_TENNISRL1_e = 0x14,
    dRes_INDEX_FGANON_BCK_TYAKUTI1_e = 0x15,
    dRes_INDEX_FGANON_BCK_WAIT1_e = 0x16,
    dRes_INDEX_FGANON_BCK_WARAU1_e = 0x17,
    /* BDLM */
    dRes_INDEX_FGANON_BDL_BPG_e = 0x1A,
    dRes_INDEX_FGANON_BDL_YDKBL00_e = 0x1B,
    dRes_INDEX_FGANON_BDL_YDKSP00_e = 0x1C,
    /* BRK  */
    dRes_INDEX_FGANON_BRK_DERU_KEN1_e = 0x1F,
    dRes_INDEX_FGANON_BRK_DERU_KEN2_e = 0x20,
    dRes_INDEX_FGANON_BRK_DERU_MAIN1_e = 0x21,
    dRes_INDEX_FGANON_BRK_DERU_MAIN2_e = 0x22,
    dRes_INDEX_FGANON_BRK_KIERU_KEN2_e = 0x23,
    dRes_INDEX_FGANON_BRK_KIERU_MAIN1_e = 0x24,
    dRes_INDEX_FGANON_BRK_KIERU_MAIN2_e = 0x25,
    dRes_INDEX_FGANON_BRK_YDKBL00_e = 0x26,
    dRes_INDEX_FGANON_BRK_YDKSP00_e = 0x27,
    /* BTK  */
    dRes_INDEX_FGANON_BTK_YDKBL00_e = 0x2A,
    dRes_INDEX_FGANON_BTK_YDKSP00_e = 0x2B,
};
enum dRes_INDEX_PGSW {
    /* BDLM */
    dRes_INDEX_PGSW_BDL_BPG_KEN1_e = 0x4,
    /* BRK  */
    dRes_INDEX_PGSW_BRK_KIERU_KEN1_e = 0x7,
};

enum FganonAction {
    ACT_STANDBY = 0,
    ACT_DERU = 1,
    ACT_KIERU = 2,
    ACT_FAIL = 3,
    ACT_FLY = 5,
    ACT_SHOT = 6,
    ACT_SPIN = 7,
    ACT_DOWN = 8,
    ACT_FLY2 = 9,
    ACT_SPIN2 = 10,
    ACT_SHOT2 = 11,
    ACT_START = 20,
    ACT_END = 21,
    ACT_LAST_END = 22,
};

#endif

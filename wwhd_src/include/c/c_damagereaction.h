// Damage reactions: GameCube names with the verified WWHD layout.
// HD: 21 joint entries and reaction-tail offsets shifted by 0x34; fire light has a four-byte HD
// tail.
#pragma once
#include "bindings.h"
using gabi::load;
using gabi::store;
static void line_init(u32 p);
static void line_destroy(u32 p);

struct enemyice {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFreezeDuration;
    /* 0x006 */ be<s8> mLightShrinkTimer;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<f32> mYOffset;
    /* 0x00C */ be<s8> m00C;
    /* 0x00D */ be<s8> mMode;
    /* 0x00E */ be<s16> mFreezeTimer;
    /* 0x010 */ be<s16> mMoveDelayTimer;
    /* 0x012 */ be<s16> mAngleY;
    /* 0x014 */ be<s16> mAngularVelY;
    /* 0x016 */ u8 _016[2];
    /* 0x018 */ cXyz mSpeed;
    /* 0x024 */ be<f32> mSpeedF;
    /* 0x028 */ be<f32> m028;
    /* 0x02C */ be<f32> m02C;
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
WWHD_OFFSET(enemyice, mCylHeight, 0x19C);
WWHD_SIZE(enemyice, 0x3B8);

/* enemyfire (c_damagereaction), HD 0x22C */
struct enemyfire {
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
    /* 0x208 */ LIGHT_INFLUENCE mLight;
};
WWHD_OFFSET(enemyfire, mSph, 0xDC);
WWHD_SIZE(enemyfire, 0x22C);

struct DamageAngle : be<s16> {
    template <class T> DamageAngle& operator=(const T& x) {
        if constexpr (std::is_floating_point_v<T>)
            set((s16)gabi::ftoi(x));
        else
            set((s16)x);
        return *this;
    }
    DamageAngle& operator=(const DamageAngle& x) {
        set(x.get());
        return *this;
    }
    template <class T> DamageAngle& operator+=(const T& x) { return *this = get() + x; }
    template <class T> DamageAngle& operator-=(const T& x) { return *this = get() - x; }
};
struct DamageAngles {
    DamageAngle x, y, z;
};
static_assert(sizeof(DamageAngles) == 6);
struct DamageSmoke {
    u8 data[0x20];
    void end() { gabi::call(0x025A5F88, this); }
};
struct DamageStts {
    u8 data[0x3C];
    cXyz* GetCCMoveP() { return gabi::at<cXyz>(gabi::ea(this)); }
};
struct DamageLine : dBgS_LinChk {
    void Init() { line_init(gabi::ea(this)); }
    void End() { line_destroy(gabi::ea(this)); }
    void Set(cXyz* a, cXyz* b, fopAc_ac_c* actor) { gabi::call(0x024F1AFC, this, a, b, actor); }
    cXyz& GetCross() { return mLin.mEnd; }
};
struct DamageGround : dBgS_GndChk {
    void Init(bool spl) {
        u32 a = gabi::ea(this);
        gabi::call(0x02008E0C, this);
        mbObjThrough = 1;
        mbCamThrough = 0;
        mbLinkThrough = 0;
        mbArrowThrough = 0;
        mbBombThrough = 0;
        mbBoomerangThrough = 0;
        mbRopeThrough = 0;
        dBgS_GrpPassChk::mGrp = spl ? 14 : 4;
        cBgS_Chk::mPolyPassChk = a + 0x40;
        cBgS_Chk::mGrpPassChk = a + 0x4C;
        cBgS_Chk::__vtbl = spl ? 0x100069D4 : 0x10006994;
        cBgS_PolyInfo::__vtbl = spl ? 0x100069E4 : 0x100069A4;
        dBgS_PolyPassChk::__vtbl = spl ? 0x10006A04 : 0x100069C4;
        dBgS_GrpPassChk::__vtbl = spl ? 0x100069F4 : 0x100069B4;
    }
    void SetPos(const cXyz* v) { m_pos.set(v->x, v->y, v->z); }
    void End() {
        cBgS_PolyInfo::__vtbl = 0x10006924;
        dBgS_PolyPassChk::__vtbl = 0x10006944;
        dBgS_GrpPassChk::__vtbl = 0x10006904;
        gabi::call(0x02008DAC, this, 0);
    }
};
static_assert(sizeof(DamageLine) == 0x6C && sizeof(DamageGround) == 0x54);
struct damagereaction {
    enum { TYPE_MOBLIN = 1, TYPE_BOKOBLIN = 2, TYPE_DARKNUT = 3 };
    /* 0x000 */ gptr<fopAc_ac_c> mpEnemy;
    /* 0x004 */ be<s16> mMode;
    /* 0x006 */ be<s16> mAction;
    /* 0x008 */ be<s16> mEnemyType;
    /* 0x00A */ u8 padA[2];
    /* 0x00C */ be<s32> mTimer;
    /* 0x010 */ DamageAngles mJntTargetAngle[21];  /* was m010: dr_damage_anime (in damage_reaction 02041F94) clears it each frame and builds the per-joint pose angles in it (knock-down pose, limb offsets, head look, shake); mJntAngle eases toward it */
    /* 0x08E */ DamageAngles mJntAngle[21];  /* was m088: dr_damage_anime (in damage_reaction 02041F94) eases it toward mJntTargetAngle (cLib_addCalcAngleS2); nodeCallBack (bk 020991CC, mo2 021C6854) applies it as the joint's Y/X/Z rotation */
    /* 0x10C */ cXyz mJntPos[21];  /* was m100: nodeCallBack / nodeCallBack_P (bk 020991CC / 02099584, mo2 021C6854 / 021C6BE0) store each joint's world position (MtxPosition); dr_joint_bg_check (in dr_damage_set 02043F34) line-checks between them */
    /* 0x208 */ cXyz mJntGroundPos[21];  /* was m1F0: dr_joint_bg_check (in dr_damage_set 02043F34): mJntPos raised by 200, then y = cBgS_GroundCross (1e9 when no ground); compared with mJntPos.y */
    /* 0x304 */ cXyz m2E0[21];
    /* 0x400 */ be<s32> mJntGroundState[14];  /* was m3D0: dr_joint_bg_check (in dr_damage_set 02043F34) sets it for joints 0..3: 2 no ground below, 0 more than 200 above the ground, 1 near the ground; dr_damage_anime (in damage_reaction 02041F94) bends joints 0..3 only while it is 0 */
    /* 0x438 */ be<s32> mJnt10LineBlocked;  /* was m408: dr_joint_bg_check (in dr_damage_set 02043F34): 2 when the line from joint 10 toward joint 14 hits background at pitch 0 (mJnt10Pitch then from the free pitch), else 0; dr_damage_anime (in damage_reaction 02041F94) applies mJnt10Pitch only while it and mJntGroundState[2] are 0 */
    /* 0x43C */ be<s32> mJnt11LineBlocked;  /* was m40C: as mJnt10LineBlocked for the line joint 11 -> joint 15 (dr_joint_bg_check, in dr_damage_set 02043F34); dr_damage_anime (in damage_reaction 02041F94) checks it with mJntGroundState[3] */
    /* 0x440 */ be<s32> mJnt8LineBlocked;  /* was m410: as mJnt10LineBlocked for the line joint 8 -> joint 16 (dr_joint_bg_check, in dr_damage_set 02043F34): 2 blocked, 0 free */
    /* 0x444 */ be<s32> mJnt9LineBlocked;  /* was m414: as mJnt10LineBlocked for the line joint 9 -> joint 17 (dr_joint_bg_check, in dr_damage_set 02043F34): 2 blocked, 0 free */
    /* 0x448 */ be<s32> mJnt12LineBlocked;  /* was m418: as mJnt10LineBlocked for the line joint 12 -> joint 18 (dr_joint_bg_check, in dr_damage_set 02043F34); dr_damage_anime (in damage_reaction 02041F94) tests it == 2 */
    /* 0x44C */ u8 pad44C[0x8];
    /* 0x454 */ be<s32> mJntBgCheckTimer;  /* was m420: dr_damage_set 02043F34 sets it on landing (REG0_S(3) + 1) and to 1 when the slide stops; dr_joint_bg_check (inlined there) runs the joint ground / line checks only while it is non-zero and counts it down */
    /* 0x458 */ be<s32> mHitFlags;  /* was m424: damage_check (in daBk_Execute 0209BD1C / daMo2_Execute 021C9B1C) ORs hit bits 0x10 / 0x20 / 0x40 into it; dr_damage_set 02043F34: 0x40 launches backward and up, 0x10 a flinch, non-zero with mKnockbackPower > 25 a knock-down; then cleared */
    /* 0x45C */ be<f32> mKnockbackPower;  /* was m428: damage_check (in daBk_Execute 0209BD1C / daMo2_Execute 021C9B1C) sets 23 / 26 or a random 90+ / 70+; dr_damage_set 02043F34 knocks the enemy down when it is > 25 and copies it into mKnockbackSpeed */
    /* 0x460 */ cXyz mHitVec;  /* was m42C: damage_check (in daBk_Execute / daMo2_Execute) stores the hit direction (MtxPosition); dr_damage_set 02043F34 takes atan2(-x, -z) as the push-back direction, the knock-back yaw and the wall-smoke yaw */
    /* 0x46C */ be<s32> m438;
    /* 0x470 */ u8 pad470[0x4];
    /* 0x474 */ be<s16> mJnt10Pitch;  /* was m440: dr_joint_bg_check (in dr_damage_set 02043F34): pitch of joint 10 toward joint 14 (free line pitch + 0x400, or the ground slope), clamped to -10000..7000; dr_damage_anime (in damage_reaction 02041F94) bends mJntTargetAngle[6].z by it */
    /* 0x476 */ be<s16> mJnt11Pitch;  /* was m442: as mJnt10Pitch for joint 11 -> joint 15 (dr_joint_bg_check, in dr_damage_set 02043F34); bends mJntTargetAngle[7].z (dr_damage_anime, in damage_reaction 02041F94) */
    /* 0x478 */ be<s16> mJnt8Pitch;  /* was m444: as mJnt10Pitch for joint 8 -> joint 16 (dr_joint_bg_check, in dr_damage_set 02043F34); bends mJntTargetAngle[4].z (dr_damage_anime, in damage_reaction 02041F94) */
    /* 0x47A */ be<s16> mJnt9Pitch;  /* was m446: as mJnt10Pitch for joint 9 -> joint 17 (dr_joint_bg_check, in dr_damage_set 02043F34); bends mJntTargetAngle[5].z (dr_damage_anime, in damage_reaction 02041F94) */
    /* 0x47C */ be<s16> mJnt12Pitch;  /* was m448: as mJnt10Pitch for joint 12 -> joint 18, clamped to +-13000 (dr_joint_bg_check, in dr_damage_set 02043F34); half of it bends joints 12, 13, 18, 19 (dr_damage_anime, in damage_reaction 02041F94) */
    /* 0x47E */ u8 pad47E[0x2];
    /* 0x480 */ cXyz mPivot;  /* was m44C: dr_matrix_set (in damage_reaction 02041F94) rotates the body about it (MtxTrans -it .. +it); dr_damage_set 02043F34 sets y = -125 on knock-down; dr_body_bg_check lowers the position by y for the ground check; hukki (bk 020A3048, mo2 021CE6E8) moves the actor by y and clears it */
    /* 0x48C */ cXyz mBodyOffset;  /* was m458: dr_matrix_set (in damage_reaction 02041F94) adds it to current.pos for the model translation; hukki (mo2 021CE6E8) fills it and eases x / z to 0, yogan_fail (mo2 021CEEB0) clears y */
    /* 0x498 */ u8 pad498[0x4];
    /* 0x49C */ be<f32> mCarryOffsetY;  /* was m468: dr_matrix_set (in damage_reaction 02041F94): last translation MtxTrans(0, it, mCarryOffsetZ); daBb_Execute 0205EA58 sets it while a Kargaroc carries the enemy, carry_drop (bk 020A47B4, mo2 021D0294) eases it to 0 */
    /* 0x4A0 */ be<f32> mCarryOffsetZ;  /* was m46C: dr_matrix_set (in damage_reaction 02041F94): z of the same MtxTrans(0, mCarryOffsetY, it); daBb_Execute 0205EA58 sets it while carrying, carry_drop (bk 020A47B4, mo2 021D0294) eases it to 0 */
    /* 0x4A4 */ be<f32> mFlinchBend;  /* was m470: dr_damage_set 02043F34 sets 4000 for a flinch (hit bit 0x10); while > 200 dr_damage_anime (in damage_reaction 02041F94) bends the upper joints away from the hit direction by it and eases it to 0; p_lost (mo2 021CDE6C) waits for < 200 */
    /* 0x4A8 */ be<f32> mShakeAmplitude;  /* was m474: dr_damage_anime (in damage_reaction 02041F94): amplitude of the sin/cos joint shake (mTimer * 5000 / 4000) on the joints selected by mShakeJntMask; dr_damage_set 02043F34 sets 2000..9000 on hits and landings and eases it to 0 */
    /* 0x4AC */ be<f32> mKnockbackSpeed;  /* was m478: dr_damage_set 02043F34: the knock-back speed along mKnockbackAngleY (from mKnockbackPower, -7 / -20 for hit bit 0x40); speed.y is derived from it; a wall hit while it is non-zero bounces; cleared on wall or ground contact */
    /* 0x4B0 */ be<s32> mLaunched;  /* was m47C: dr_damage_set 02043F34 sets 1 at the knock-down launch, 0 on wall / ground contact (and in dr_body_bg_check); dr_damage_anime (in damage_reaction 02041F94) poses the flying body while it is set */
    /* 0x4B4 */ be<s16> mFallPitch;  /* was m480: dr_damage_set 02043F34 eases it toward mFallPitchTarget; dr_matrix_set (in damage_reaction 02041F94) and kado_check 02041D10 rotate the body by it about X (after mKnockbackAngleY); hukki (bk 020A3048, mo2 021CE6E8) clears it */
    /* 0x4B6 */ be<s16> mKnockbackAngleY;  /* was m482: dr_damage_set 02043F34: yaw of the knock-back (atan2 of mHitVec, +0x8000 for hit bit 0x40, or shape_angle.y); the speed is rotated by it and dr_matrix_set (in damage_reaction 02041F94) pitches the body about it */
    /* 0x4B8 */ be<s16> mSpinAngleY;  /* was m484: dr_damage_set 02043F34 steps it by +-0x1000 while mSpinLeft runs and adds it to current / shape angle.y at recovery; dr_matrix_set (in damage_reaction 02041F94) turns the body by it */
    /* 0x4BA */ be<s16> mFallPitchTarget;  /* was m486: dr_damage_set 02043F34 sets 0x4000 (falls over) / -0x4000 / 0; mFallPitch eases toward it */
    /* 0x4BC */ be<s16> mKnockedDown;  /* was m488: dr_damage_set 02043F34: 0 upright, set to 1 by a knock-down, cleared at recovery; hukki (bk 020A3048, mo2 021CE6E8) clears it; dr_damage_anime / dr_body_bg_check test == 1 */
    /* 0x4BE */ be<s16> mHitStunTimer;  /* was m48A: damage_check (in daBk_Execute / daMo2_Execute) sets 10 on a hit; daBk_Execute 0209BD1C / daMo2_Execute 021C9B1C count it down; Bk_move 020A5EF4 / Mo2_move 021D0A14 stagger while it runs and the enemy is not knocked down */
    /* 0x4C0 */ csXyz mSlopeTilt;  /* was m48C: daBk_Execute 0209BD1C / daMo2_Execute 021C9B1C ease x / z toward the ground slope angles; dr_matrix_set (in damage_reaction 02041F94) tilts the body by x / z */
    /* 0x4C6 */ be<s16> mJnt12SlopeY;  /* was m492: dr_damage_anime (in damage_reaction 02041F94): eased toward mSlopeTilt rotated into the enemy's frame (0 while knocked down) and subtracted from mJntTargetAngle[12].y */
    /* 0x4C8 */ be<s16> mJnt12SlopeZ;  /* was m494: dr_damage_anime (in damage_reaction 02041F94): z partner of mJnt12SlopeY, subtracted from mJntTargetAngle[12].z */
    /* 0x4CA */ be<s16> mJnt18AddY;  /* was m496: dr_damage_anime (in damage_reaction 02041F94) eases it toward mJnt18AddYTarget and adds it to mJntTargetAngle[18].y */
    /* 0x4CC */ be<s16> mJnt13AddY;  /* was m498: dr_damage_anime (in damage_reaction 02041F94) eases it toward mJnt13AddYTarget and adds it to mJntTargetAngle[13].y */
    /* 0x4CE */ be<s16> mJnt18AddYTarget;  /* was m49A: dr_damage_set 02043F34 sets a random +-18000 on landing, 0 at recovery; mJnt18AddY eases toward it */
    /* 0x4D0 */ be<s16> mJnt13AddYTarget;  /* was m49C: dr_damage_set 02043F34 sets a random +-10000 on landing, 0 at recovery; mJnt13AddY eases toward it */
    /* 0x4D2 */ be<s16> mParalysisTimer;  /* was m49E: d_mahi (bk 020A4B44, mo2 021CFBBC) sets 15; dr_damage_anime (in damage_reaction 02041F94) counts it down and shakes the head by a sine of it */
    /* 0x4D4 */ DamageAngle mJnt0AddZ;  /* was m4A0: dr_damage_anime (in damage_reaction 02041F94) eases it toward mJnt0AddZTarget and adds it to mJntTargetAngle[0].z */
    /* 0x4D6 */ DamageAngle mJnt1AddZ;  /* was m4A2: dr_damage_anime (in damage_reaction 02041F94) eases it toward mJnt1AddZTarget and adds it to mJntTargetAngle[1].z */
    /* 0x4D8 */ DamageAngle mJnt2AddZ;  /* was m4A4: dr_damage_anime (in damage_reaction 02041F94) eases it toward mJnt2AddZTarget, adds it to mJntTargetAngle[2].z and half of it subtracts from [6].z */
    /* 0x4DA */ DamageAngle mJnt3AddZ;  /* was m4A6: dr_damage_anime (in damage_reaction 02041F94) eases it toward mJnt3AddZTarget, adds it to mJntTargetAngle[3].z and half of it subtracts from [7].z */
    /* 0x4DC */ DamageAngle mJnt0AddZTarget;  /* was m4A8: dr_damage_set 02043F34 sets a random -20000..0 at the launch / bounce, 0 on contact; mJnt0AddZ eases toward it */
    /* 0x4DE */ DamageAngle mJnt1AddZTarget;  /* was m4AA: dr_damage_set 02043F34 sets a random -20000..0 at the launch / bounce, 0 on contact; mJnt1AddZ eases toward it */
    /* 0x4E0 */ DamageAngle mJnt2AddZTarget;  /* was m4AC: dr_damage_set 02043F34 sets a random 0..20000 at the launch / bounce, 0 on contact; mJnt2AddZ eases toward it */
    /* 0x4E2 */ DamageAngle mJnt3AddZTarget;  /* was m4AE: dr_damage_set 02043F34 sets a random 0..20000 at the launch / bounce, 0 on contact; mJnt3AddZ eases toward it */
    /* 0x4E4 */ DamageAngle mJnt2LandZ;  /* was m4B0: dr_damage_set 02043F34 sets a random 0..20000 on landing (0 at recovery, in dr_joint_bg_check when face down); dr_damage_anime (in damage_reaction 02041F94) adds it to mJntTargetAngle[2].z */
    /* 0x4E6 */ DamageAngle mJnt3LandZ;  /* was m4B2: dr_damage_set 02043F34 sets a random 0..20000 on landing (0 at recovery, in dr_joint_bg_check when face down); dr_damage_anime (in damage_reaction 02041F94) adds it to mJntTargetAngle[3].z */
    /* 0x4E8 */ DamageAngle mJnt6LandZ;  /* was m4B4: dr_damage_set 02043F34 sets mJnt2LandZ / 2 on landing (0 at recovery / face down); dr_damage_anime (in damage_reaction 02041F94) subtracts it from mJntTargetAngle[6].z */
    /* 0x4EA */ DamageAngle mJnt7LandZ;  /* was m4B6: dr_damage_set 02043F34 sets mJnt3LandZ / 2 on landing (0 at recovery / face down); dr_damage_anime (in damage_reaction 02041F94) subtracts it from mJntTargetAngle[7].z */
    /* 0x4EC */ DamageAngle mJnt6AddY;  /* was m4B8: dr_damage_set 02043F34 sets a random 1000..7000 at the knock-down (not Darknut), 0 at recovery; dr_damage_anime (in damage_reaction 02041F94) adds it to mJntTargetAngle[6].y */
    /* 0x4EE */ DamageAngle mJnt7AddY;  /* was m4BA: dr_damage_set 02043F34 sets a random 1000..7000 at the knock-down (not Darknut), 0 at recovery; dr_damage_anime (in damage_reaction 02041F94) subtracts it from mJntTargetAngle[7].y */
    /* 0x4F0 */ DamageAngle mJnt4AddY;  /* was m4BC: dr_damage_set 02043F34 sets a random value at the knock-down, 0 at recovery; dr_damage_anime (in damage_reaction 02041F94) adds it to mJntTargetAngle[4].y (and uses it as [8].z) */
    /* 0x4F2 */ DamageAngle mJnt5AddY;  /* was m4BE: dr_damage_set 02043F34 sets a random value at the knock-down, 0 at recovery; dr_damage_anime (in damage_reaction 02041F94) subtracts it from mJntTargetAngle[5].y (and uses it as [9].z) */
    /* 0x4F4 */ be<s16> mLandAngleY;  /* was m4C0: dr_damage_set 02043F34 sets it on landing to mKnockbackAngleY or its reverse (whichever is nearer current.angle.y); while knocked down current.angle.y eases toward it */
    /* 0x4F6 */ u8 pad4F6[0x6];
    /* 0x4FC */ be<s16> mDownTimers[3];  /* was m4C8: damage_reaction 02041F94 counts all three down; dr_damage_set 02043F34: [0] = 10 holds the body after a high wall hit, [2] delays the recovery (set from the enemies' HIO by daBk_Execute 0209BD1C / daMo2_Execute 021C9B1C, 10000 at death) */
    /* 0x502 */ be<s16> mInvincibleTimer;
    /* 0x504 */ be<s16> mTurnTargetY;  /* was m4D0: bk / mo2 action code (fight_run 020A0A98, b_nige 020A2A54, wepon_search 020A3CC8, hukki 021CE6E8, ...) sets the yaw the enemy should face (from mTargetAngleY or atan2) and eases current.angle.y toward it */
    /* 0x506 */ u8 pad506[0x2];
    /* 0x508 */ be<f32> mPushback;  /* was m4D4: damage_check (in daBk_Execute / daMo2_Execute) sets -20 / -HIO value; dr_damage_set 02043F34 moves the enemy by it along the hit direction (atan2 of -mHitVec) each frame and eases it to 0 */
    /* 0x50C */ u8 pad50C[4];
    /* 0x510 */ dBgS_AcchCir mAcchCir;
    /* 0x550 */ dBgS_ObjAcch mAcch;
    /* 0x714 */ be<s32> mSpinLeft;  /* was m6E0: dr_damage_set 02043F34 adds +-0x8000 / +-0x10000 after a corner (kado_check 02041D10) hit, then steps it to 0 by 0x1000 while adding the same steps to mSpinAngleY */
    /* 0x718 */ be<f32> mSpawnY;
    /* 0x71C */ cXyz mPrevGroundPos;  /* was m6E8: dr_body_bg_check (in dr_damage_set 02043F34): old.pos with y = the ground height before this check; dr_damage_set 02043F34 uses it as the ledge position (mHangPos) when the fall height exceeds 300 */
    /* 0x728 */ cXyz m6F4[2];
    /* 0x740 */ be<u8> mBounceCount;  /* was m70C: dr_damage_set 02043F34 increments it at each ground landing while knocked down and clears it at the launch; daBk_Create 0209F918 / daMo2_Create 021CCB00 set 1; damage_check (in daBk_Execute) clears it */
    /* 0x741 */ be<u8> mFaceDown;  /* was m70D: dr_matrix_set (in damage_reaction 02041F94): 1 when a point 10000 ahead in the body frame lies below current.pos; dr_damage_set 02043F34 picks recovery mode 10 / 12 by it */
    /* 0x742 */ be<u8> mShakeJntMask;  /* was m70E: dr_damage_set 02043F34 sets 0xFF / 7; dr_damage_anime (in damage_reaction 02041F94) shakes joints 2, 3, 6, 7 (bit 8), 0, 1, 4, 5 (bit 4), 19 (bit 2), 13 (bit 1) */
    /* 0x743 */ u8 pad743[0x1];
    /* 0x744 */ be<u8> mLookMode;  /* was m710: set by the bk / mo2 action code (hukki 020A3048, fight_run 020A0A98, p_lost 020A27F8, Bk_move 020A5EF4, ...: 0, 1, 3, 4, 6); dr_damage_anime (in damage_reaction 02041F94): 1 turns the head toward mpLookTarget's eyePos, 5 uses mLookPitch / mLookYaw, 3 / 4 / 6 fixed poses */
    /* 0x745 */ be<u8> mDefencePose;  /* was m711: defence (mo2 021CE46C) sets 1, Bk_move 020A5EF4 / Mo2_move 021D0A14 clear it; dr_damage_anime (in damage_reaction 02041F94) sets mJntTargetAngle[8].x = 8000 while it is set */
    /* 0x746 */ be<u8> m712;
    /* 0x747 */ be<u8> mPlayerInView;  /* was m713: daBk_player_view_check 0209B05C / daMo2_player_view_check 021C8C14 set 1 / 2 when the player is in the view box, 0 otherwise; daBk_player_bg_check 0209AE7C / daMo2_player_bg_check 021C8A38 test and clear it; dr_damage_set 02043F34 clears it at the knock-down */
    /* 0x748 */ gptr<fopAc_ac_c> mpLookTarget;  /* was m714: Bk_move 020A5EF4 / Mo2_move 021D0A14 set the player, aite_miru (bk 020A3624, mo2 021CEC44) another actor; the view / bg checks use its current.pos; dr_damage_anime (in damage_reaction 02041F94) turns the head toward its eyePos */
    /* 0x74C */ be<s16> mLookPitch;  /* was m718: set by bk_move code (-10000 / random); dr_damage_anime (in damage_reaction 02041F94) uses it as the head pitch for mLookMode 5 and adds it to mJntTargetAngle[18].z */
    /* 0x74E */ be<s16> mLookYaw;  /* was m71A: set by bk_move code (10000 / random); dr_damage_anime (in damage_reaction 02041F94) uses it as the head yaw for mLookMode 5 and adds it to mJntTargetAngle[18].y */
    /* 0x750 */ u8 pad750[0x2];
    /* 0x752 */ be<s16> mBodyBgCheckDelay;  /* was m71E: dr_body_bg_check (in dr_damage_set 02043F34) skips the body ground check and counts it down while it is non-zero; dr_damage_set sets 2 / 10, water_fail 020A3B18 / b_hang 020A5674 5, damage_check (in daBk_Execute) 10 */
    /* 0x754 */ be<f32> mMaxFallDistance;
    /* 0x758 */ DamageStts mStts;
    /* 0x794 */ cXyz mParticlePos;
    /* 0x7A0 */ csXyz mParticleAngle;
    /* 0x7A6 */ u8 pad7A6[2];
    /* 0x7A8 */ DamageSmoke mParticleCallBack;
    /* 0x7C8 */ be<s16> mWallSmokeTimer;  /* was m794: dr_damage_set 02043F34 sets 6 when it starts the wall-hit smoke; damage_reaction 02041F94 counts it down and ends mParticleCallBack at 0 */
    /* 0x7CA */ u8 pad7CA[0x2];
    /* 0x7CC */ be<f32> mHangMoveSpeed;  /* was m798: b_hang 020A5674 clears it and eases it to 100; the step with which current.pos follows the hang position */
    /* 0x7D0 */ gptr<cXyz> mpHangPos;  /* was m79C: ride_call_back (d_a_bridge 020DF908) points it at the bridge's side position; b_hang 020A5674 makes current.pos follow it while mHangBridgeId is set */
    /* 0x7D4 */ cXyz mHangPos;  /* was m7A0: dr_damage_set 02043F34 copies mPrevGroundPos into it before the hang; hang_ang_get (inlined there) measures from it; b_hang 020A5674 makes current.pos follow it (ledge hang) */
    /* 0x7E0 */ csXyz mHangAngle;  /* was m7AC: dr_damage_set 02043F34 sets y from hang_ang_get (0xDCF = no hang: dies); ride_call_back (d_a_bridge 020DF908) sets the bridge rotation; b_hang 020A5674 turns current.angle.y toward y */
    /* 0x7E6 */ be<s16> mBridgeRideTimer;  /* was m7B2: ride_call_back (d_a_bridge 020DF908) sets 8 each frame a Bokoblin rides the bridge; damage_reaction 02041F94 counts it down; dr_damage_set 02043F34 starts the hang action 0x13 while it is non-zero */
    /* 0x7E8 */ be<s16> mBridgeRideRoll;  /* was m7B4: ride_call_back (d_a_bridge 020DF908) sets -0x2000; b_hang 020A5674 passes it to the bridge as mRideRollTarget */
    /* 0x7EA */ be<u8> mSkipMoveOnce;  /* was m7B6: dr_damage_set 02043F34 sets 1 at recovery and skips one frame of its movement / ground checks while it is set (clearing it); hukki (bk 020A3048) sets 1 */
    /* 0x7EB */ u8 pad7EB[0x1];
    /* 0x7EC */ be<u32> mHangBridgeId;  /* was m7B8: ride_call_back (d_a_bridge 020DF908) stores the bridge's process ID; dr_damage_set 02043F34 sets -1 for a ledge hang; b_hang 020A5674 searches the bridge by it */
};
static_assert(offsetof(damagereaction, mLaunched) == 0x4B0);
static_assert(offsetof(damagereaction, mJntGroundState) == 0x400);
static_assert(offsetof(damagereaction, mAcch) == 0x550);
static_assert(offsetof(damagereaction, mSpawnY) == 0x718);

WWHD_OFFSET(damagereaction, mpEnemy, 0x0);
WWHD_OFFSET(damagereaction, mMode, 0x4);
WWHD_OFFSET(damagereaction, mAction, 0x6);
WWHD_OFFSET(damagereaction, mEnemyType, 0x8);
WWHD_OFFSET(damagereaction, mTimer, 0xC);
WWHD_OFFSET(damagereaction, mJntTargetAngle, 0x10);
WWHD_OFFSET(damagereaction, mJntAngle, 0x8E);
WWHD_OFFSET(damagereaction, mJntPos, 0x10C);
WWHD_OFFSET(damagereaction, mJntGroundPos, 0x208);
WWHD_OFFSET(damagereaction, m2E0, 0x304);
WWHD_OFFSET(damagereaction, mAcchCir, 0x510);
WWHD_OFFSET(damagereaction, mAcch, 0x550);
WWHD_OFFSET(damagereaction, mStts, 0x758);
WWHD_OFFSET(damagereaction, mParticleCallBack, 0x7A8);
WWHD_OFFSET(damagereaction, mJntGroundState, 0x400);
WWHD_OFFSET(damagereaction, mJnt10LineBlocked, 0x438);
WWHD_OFFSET(damagereaction, mJnt11LineBlocked, 0x43C);
WWHD_OFFSET(damagereaction, mJnt8LineBlocked, 0x440);
WWHD_OFFSET(damagereaction, mJnt9LineBlocked, 0x444);
WWHD_OFFSET(damagereaction, mJnt12LineBlocked, 0x448);
WWHD_OFFSET(damagereaction, mJntBgCheckTimer, 0x454);
WWHD_OFFSET(damagereaction, mHitFlags, 0x458);
WWHD_OFFSET(damagereaction, mKnockbackPower, 0x45C);
WWHD_OFFSET(damagereaction, mHitVec, 0x460);
WWHD_OFFSET(damagereaction, m438, 0x46C);
WWHD_OFFSET(damagereaction, mJnt10Pitch, 0x474);
WWHD_OFFSET(damagereaction, mJnt11Pitch, 0x476);
WWHD_OFFSET(damagereaction, mJnt8Pitch, 0x478);
WWHD_OFFSET(damagereaction, mJnt9Pitch, 0x47A);
WWHD_OFFSET(damagereaction, mJnt12Pitch, 0x47C);
WWHD_OFFSET(damagereaction, mPivot, 0x480);
WWHD_OFFSET(damagereaction, mBodyOffset, 0x48C);
WWHD_OFFSET(damagereaction, mCarryOffsetY, 0x49C);
WWHD_OFFSET(damagereaction, mCarryOffsetZ, 0x4A0);
WWHD_OFFSET(damagereaction, mFlinchBend, 0x4A4);
WWHD_OFFSET(damagereaction, mShakeAmplitude, 0x4A8);
WWHD_OFFSET(damagereaction, mKnockbackSpeed, 0x4AC);
WWHD_OFFSET(damagereaction, mLaunched, 0x4B0);
WWHD_OFFSET(damagereaction, mFallPitch, 0x4B4);
WWHD_OFFSET(damagereaction, mKnockbackAngleY, 0x4B6);
WWHD_OFFSET(damagereaction, mSpinAngleY, 0x4B8);
WWHD_OFFSET(damagereaction, mFallPitchTarget, 0x4BA);
WWHD_OFFSET(damagereaction, mKnockedDown, 0x4BC);
WWHD_OFFSET(damagereaction, mHitStunTimer, 0x4BE);
WWHD_OFFSET(damagereaction, mSlopeTilt, 0x4C0);
WWHD_OFFSET(damagereaction, mJnt12SlopeY, 0x4C6);
WWHD_OFFSET(damagereaction, mJnt12SlopeZ, 0x4C8);
WWHD_OFFSET(damagereaction, mJnt18AddY, 0x4CA);
WWHD_OFFSET(damagereaction, mJnt13AddY, 0x4CC);
WWHD_OFFSET(damagereaction, mJnt18AddYTarget, 0x4CE);
WWHD_OFFSET(damagereaction, mJnt13AddYTarget, 0x4D0);
WWHD_OFFSET(damagereaction, mParalysisTimer, 0x4D2);
WWHD_OFFSET(damagereaction, mJnt0AddZ, 0x4D4);
WWHD_OFFSET(damagereaction, mJnt1AddZ, 0x4D6);
WWHD_OFFSET(damagereaction, mJnt2AddZ, 0x4D8);
WWHD_OFFSET(damagereaction, mJnt3AddZ, 0x4DA);
WWHD_OFFSET(damagereaction, mJnt0AddZTarget, 0x4DC);
WWHD_OFFSET(damagereaction, mJnt1AddZTarget, 0x4DE);
WWHD_OFFSET(damagereaction, mJnt2AddZTarget, 0x4E0);
WWHD_OFFSET(damagereaction, mJnt3AddZTarget, 0x4E2);
WWHD_OFFSET(damagereaction, mJnt2LandZ, 0x4E4);
WWHD_OFFSET(damagereaction, mJnt3LandZ, 0x4E6);
WWHD_OFFSET(damagereaction, mJnt6LandZ, 0x4E8);
WWHD_OFFSET(damagereaction, mJnt7LandZ, 0x4EA);
WWHD_OFFSET(damagereaction, mJnt6AddY, 0x4EC);
WWHD_OFFSET(damagereaction, mJnt7AddY, 0x4EE);
WWHD_OFFSET(damagereaction, mJnt4AddY, 0x4F0);
WWHD_OFFSET(damagereaction, mJnt5AddY, 0x4F2);
WWHD_OFFSET(damagereaction, mLandAngleY, 0x4F4);
WWHD_OFFSET(damagereaction, mDownTimers, 0x4FC);
WWHD_OFFSET(damagereaction, mInvincibleTimer, 0x502);
WWHD_OFFSET(damagereaction, mTurnTargetY, 0x504);
WWHD_OFFSET(damagereaction, mPushback, 0x508);
WWHD_OFFSET(damagereaction, mSpinLeft, 0x714);
WWHD_OFFSET(damagereaction, mSpawnY, 0x718);
WWHD_OFFSET(damagereaction, mPrevGroundPos, 0x71C);
WWHD_OFFSET(damagereaction, m6F4, 0x728);
WWHD_OFFSET(damagereaction, mBounceCount, 0x740);
WWHD_OFFSET(damagereaction, mFaceDown, 0x741);
WWHD_OFFSET(damagereaction, mShakeJntMask, 0x742);
WWHD_OFFSET(damagereaction, mLookMode, 0x744);
WWHD_OFFSET(damagereaction, mDefencePose, 0x745);
WWHD_OFFSET(damagereaction, m712, 0x746);
WWHD_OFFSET(damagereaction, mPlayerInView, 0x747);
WWHD_OFFSET(damagereaction, mpLookTarget, 0x748);
WWHD_OFFSET(damagereaction, mLookPitch, 0x74C);
WWHD_OFFSET(damagereaction, mLookYaw, 0x74E);
WWHD_OFFSET(damagereaction, mBodyBgCheckDelay, 0x752);
WWHD_OFFSET(damagereaction, mMaxFallDistance, 0x754);
WWHD_OFFSET(damagereaction, mParticlePos, 0x794);
WWHD_OFFSET(damagereaction, mParticleAngle, 0x7A0);
WWHD_OFFSET(damagereaction, mWallSmokeTimer, 0x7C8);
WWHD_OFFSET(damagereaction, mHangMoveSpeed, 0x7CC);
WWHD_OFFSET(damagereaction, mpHangPos, 0x7D0);
WWHD_OFFSET(damagereaction, mHangPos, 0x7D4);
WWHD_OFFSET(damagereaction, mHangAngle, 0x7E0);
WWHD_OFFSET(damagereaction, mBridgeRideTimer, 0x7E6);
WWHD_OFFSET(damagereaction, mBridgeRideRoll, 0x7E8);
WWHD_OFFSET(damagereaction, mSkipMoveOnce, 0x7EA);
WWHD_OFFSET(damagereaction, mHangBridgeId, 0x7EC);
WWHD_SIZE(damagereaction, 0x7F0);

// HD: word-copy views preserve the original lfs/stfs bit transport.
template <class T> inline T* damageView(u32 address) { return gabi::at<T>(address); }
template <class T> inline be<u32>& damageWord(T& field) {
    return *gabi::at<be<u32>>(gabi::ea(&field));
}

template <class T, class U> inline T* damageView(U* address) {
    return reinterpret_cast<T*>(address);
}

// HD: cCcS is opaque in the shared layout; this is its verified Set call.
inline void damageSetCollision(cCcS* cc, dCcD_GObjInf* object) {
    gabi::call(0x0200E240, cc, object);
}

// HD: emitter globals follow the expanded particle lists; group ID is at 0x262.
struct DamageEmitter : JPABaseEmitter {
    /* 0x070 */ u8 _070[0x220 - 0x70];
    /* 0x220 */ cXyz mGlobalDynamicsScale;
    /* 0x22C */ cXyz mGlobalTranslation;
    /* 0x238 */ cXyz mGlobalParticleScale;
    /* 0x244 */ u8 _244[0x254 - 0x244];
    /* 0x254 */ be<u32> mFlags;
    /* 0x258 */ u8 _258[0x262 - 0x258];
    /* 0x262 */ be<u8> mGroupID;
    /* 0x263 */ u8 _263;
};
WWHD_OFFSET(DamageEmitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(DamageEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(DamageEmitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(DamageEmitter, mFlags, 0x254);
WWHD_OFFSET(DamageEmitter, mGroupID, 0x262);
WWHD_SIZE(DamageEmitter, 0x264);

// HD: per-translation-unit header statics emitted by the compiler.
struct DamageHeaderStatics {
    be<f32> mMinAngle, mMaxAngle;
    be<u8> mMathState, mRandomState;
    u8 _00A[2];
    be<u32> mZero[4];
};
WWHD_OFFSET(DamageHeaderStatics, mZero, 0xC);
WWHD_SIZE(DamageHeaderStatics, 0x1C);

// Narrow actor views used by the shared damage-reaction code.
struct DamageBokoblin : fopAc_ac_c {
    u8 _3AC[0x3D4 - 0x3AC];
    be<u8> m02B8;
};
struct DamageDarknut : fopAc_ac_c {
    u8 _3AC[0x3EC - 0x3AC];
    be<s8> mRemainingEquipmentPieces;
};
WWHD_OFFSET(DamageBokoblin, m02B8, 0x3D4);
WWHD_SIZE(DamageBokoblin, 0x3D5);
WWHD_OFFSET(DamageDarknut, mRemainingEquipmentPieces, 0x3EC);
WWHD_SIZE(DamageDarknut, 0x3ED);

// HD: these call sites retain double results or full-width integer registers.
inline f64 damageRndF(f32 limit) { return gabi::call<f64>(0x020198D8, limit); }
inline f64 damageRndFX(f32 limit) { return gabi::call<f64>(0x02019918, limit); }
inline f64 damageSqrt(f32 value) { return gabi::call<f64>(0x028F4384, value); }
inline void damageYrotS(Mtx34* matrix, s32 angle) { gabi::call(0x025F1884, matrix, angle); }
inline s32 damageAtan2s(f32 y, f32 x) { return gabi::call<s32>(0x020195B0, y, x); }

// HD: the compiler emits a draw-list object with nine eight-byte entries.
struct DamageDrawList {
    be<u32> __vtbl;
    be<u32> mRandomState[3];
    u8 mEntries[9][8];
    be<f32> mValue;
};
WWHD_OFFSET(DamageDrawList, mEntries, 0x10);
WWHD_OFFSET(DamageDrawList, mValue, 0x58);
WWHD_SIZE(DamageDrawList, 0x5C);

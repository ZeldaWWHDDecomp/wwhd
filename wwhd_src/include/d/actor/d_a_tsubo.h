/* daTsubo::Act_c (small carriable objects: pots, skulls, barrels, ...), WWHD layout.
 *
 * GameCube -> WWHD: every member +0x11C (fopAc_ac_c); size 0x93C (constructor 024CA1DC
 * allocates 0x93C; GameCube 0x820). The HD vtable pointer is at +0xB4. The data tables keep
 * their GameCube layouts (Data_c 0xCC, M_data at 0x10040FAC). */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"

namespace daTsubo {

/* dBgS_ObjGndChk_Yogan (0x54, the lava ground check: mGrp 4) */
struct dBgS_GndChk_l {
    u8 _[0x54];
};

/* dPa_followEcallBack (0x14), see bindings.h; remove() is the virtual at vtable +0x44 */

struct Act_c : fopAc_ac_c {
    bool pos_init(); /* GameCube inline (below) [v nz, ji1] */
    enum Prm_e {
        PRM_ITEMNO_W = 6, PRM_ITEMNO_S = 0,
        PRM_SPEC_W = 6, PRM_SPEC_S = 8,
        PRM_MOVEBG_W = 2, PRM_MOVEBG_S = 14,
        PRM_ITEMSAVE_W = 7, PRM_ITEMSAVE_S = 16,
        PRM_TYPE_W = 4, PRM_TYPE_S = 24,
        PRM_CULL_W = 3, PRM_CULL_S = 28,
        PRM_STICK_W = 1, PRM_STICK_S = 31,
    };
    enum DataFlag_e {
        DATA_FLAG_1_e = 0x1, DATA_FLAG_2_e = 0x2, DATA_FLAG_4_e = 0x4,
        DATA_FLAG_8_e = 0x8, DATA_FLAG_10_e = 0x10, DATA_FLAG_20_e = 0x20,
    };

    struct Data_c {
        /* 0x00 */ be<f32> mGravity;
        /* 0x04 */ be<f32> mCenterHeight;  /* was m04: set_mtx 024CB280 lifts the model by it and turns the pose about that height (transM back by -it); crr_pos_water 024CD5E0 / crr_pos_lava 024CD70C / chk_sink_lava 024CCB4C test the water and lava surface at pos.y + it (tww d_a_tsubo.cpp the same) */
        /* 0x08 */ be<f32> mAttnY;
        /* 0x0C */ be<f32> mModelScale;
        /* 0x10 */ be<u8> mWeight;  /* was m10: create_init_cc 024CA804 / mode_wait_init 024CAC44 / mode_drop_init 024CF5AC: mStts.Init's weight (tww the same) */
        /* 0x11 */ u8 _11[3];
        /* 0x14 */ be<f32> mDropSpeedY;  /* was m14: mode_drop_init 024CF5AC: speed.y when the pot is let go (tww: unless m810 is set) */
        /* 0x18 */ be<f32> mDropSpeedF;  /* was m18: mode_drop_init 024CF5AC: speedF = it + the player's speedF * mDropSpeedFPlayerRate (tww the same) */
        /* 0x1C */ be<f32> mDropSpeedFPlayerRate;  /* was m1C: mode_drop_init 024CF5AC: the share of the player's speedF added to mDropSpeedF (tww the same) */
        /* 0x20 */ be<f32> mAirResistLinear;  /* was m20: calc_drop_param 024CED64 / calc_afl_param 024CFB20: posMoveF_stream's linear resistance above water (tww posMoveF_resist_acc param_4); mode_afl 024CFD3C scales the wind vector by it (tww the same) */
        /* 0x24 */ be<f32> mAirResistQuad;  /* was m24: calc_drop_param 024CED64 / calc_afl_param 024CFB20: posMoveF_stream's quadratic resistance above water (tww posMoveF_resist_acc param_5) */
        /* 0x28 */ be<s16> mThrowRotAxisRollMax;  /* was m28: init_rot_throw 024CF4E0: mRotAxisRoll = it * cM_rnd() (tww the same) */
        /* 0x2A */ be<s16> mThrowRotAxisPitchSpeed;  /* was m2A: init_rot_throw 024CF4E0: the start value of mRotAxisPitchSpeed (tww the same) */
        /* 0x2C */ be<s16> mThrowRotAngleSpeed;  /* was m2C: init_rot_throw 024CF4E0: the start value of mRotAngleSpeed (tww the same) */
        /* 0x2E */ u8 _2E[2];
        /* 0x30 */ be<f32> mDropRotResistLinear;  /* was m30: set_tensor_drop 024D0074: tensor_damp's linear damping of mRotAxisPitchSpeed / mRotAngleSpeed in the air (tww: speed -= rad2s(v * it + v * |v| * mDropRotResistQuad)) */
        /* 0x34 */ be<f32> mDropRotResistQuad;  /* was m34: set_tensor_drop 024D0074: tensor_damp's quadratic damping of mRotAxisPitchSpeed / mRotAngleSpeed in the air (tww the same) */
        /* 0x38 */ be<f32> mBreakFallHeight;  /* was m38: damage_bg_proc_directly 024CDAB4: the pot breaks on landing when mFallStartY - current.pos.y is above it (tww the same) */
        /* 0x3C */ be<f32> mBuoyancy;  /* was m3C: mode_sink_init 024CCE64: gravity = mGravity + it; damage_bg_proc 024CD1E8 tests mGravity + it < 0; calc_drop_param 024CED64 / calc_afl_param 024CFB20 add immersion * it to mGravity (tww the same) */
        /* 0x40 */ be<f32> mWaterResistLinear;  /* was m40: mode_sink 024CFAB8: posMoveF_stream's linear resistance under water; calc_drop_param 024CED64 / calc_afl_param 024CFB20 its immersion share (tww the same) */
        /* 0x44 */ be<f32> mWaterResistQuad;  /* was m44: mode_sink 024CFAB8: posMoveF_stream's quadratic resistance under water; calc_drop_param 024CED64 / calc_afl_param 024CFB20 its immersion share (tww the same) */
        /* 0x48 */ be<f32> mWaterRotResistLinear;  /* was m48: set_tensor_sink 024D0184 / set_tensor_afl 024D0294: tensor_damp's linear damping of mRotAxisPitchSpeed / mRotAngleSpeed in water (tww the same) */
        /* 0x4C */ be<f32> mWaterRotResistQuad;  /* was m4C: set_tensor_sink 024D0184 / set_tensor_afl 024D0294: tensor_damp's quadratic damping of mRotAxisPitchSpeed / mRotAngleSpeed in water (tww the same) */
        /* 0x50 */ be<f32> mWaterEntrySpeedMax;  /* was m50: limit_speed (inlined in mode_sink_init 024CCE64 / mode_afl_init 024CCD44): the speed (|speed.y, speedF|) is scaled down to it on entering water (tww mode_sink_init / mode_afl_init the same) */
        /* 0x54 */ be<s16> m54;
        /* 0x56 */ be<s16> mNoGradeAngle;  /* was m56: mode_walk 024CF094: posMoveF_grade's no_grade_cos is cos(it - 0.5 degrees) (tww the same) */
        /* 0x58 */ be<f32> m58;
        /* 0x5C */ be<f32> mGradeFriction;  /* was m5C: mode_walk 024CF094: posMoveF_grade's friction (tww the same) */
        /* 0x60 */ be<u8> mBreakNoiseLevel;  /* was m60: se_break 024CBE2C: set_senv's level (dKy_Sound_set's mNoiseLevel) of the break noise (tww the same) */
        /* 0x61 */ be<u8> mBreakNoiseTimer;  /* was m61: se_break 024CBE2C: set_senv's timer (dKy_Sound_set's mNoiseTimer) of the break noise (tww the same) */
        /* 0x62 */ be<u8> mHitNoiseLevel;  /* was m62: damage_cc_proc 024CC7EC: set_senv's level (dKy_Sound_set's mNoiseLevel) of the hit noise (tww the same) */
        /* 0x63 */ be<u8> mHitNoiseTimer;  /* was m63: damage_cc_proc 024CC7EC: set_senv's timer (dKy_Sound_set's mNoiseTimer) of the hit noise (tww the same) */
        /* 0x64 */ be<u8> mFallInNoiseLevel;  /* was m64: se_fall_water 024CCBA0 / se_fall_lava 024CCF84: set_senv's level (dKy_Sound_set's mNoiseLevel) of the splash noise (tww the same) */
        /* 0x65 */ be<u8> mFallInNoiseTimer;  /* was m65: se_fall_water 024CCBA0 / se_fall_lava 024CCF84: set_senv's timer (dKy_Sound_set's mNoiseTimer) of the splash noise (tww the same) */
        /* 0x66 */ u8 _66[2];
        /* 0x68 */ be<u32> mFlag;
        /* 0x6C */ be<u16> mModelIdx;  /* was m6C: create_heap 024CA3D0: the model's resource index in M_arcname(mType) (tww the same) */
        /* 0x6E */ be<u8> mShadowSize;  /* was m6E: never read in WWHD (the HD _draw draws no simple shadow); tww _draw passes scale.x * it as dComIfGd_setSimpleShadow2's size */
        /* 0x6F */ be<u8> mAcchCirRad;
        /* 0x70 */ be<u8> mCylHeight;  /* was m70: create_init_cc 024CA804: mCyl.SetH (tww the same; mAcchCirRad is its radius) */
        /* 0x71 */ be<u8> mAcchRoofHeight;
        /* 0x72 */ be<u8> mAttnDist;
        /* 0x73 */ u8 _73;
        /* 0x74 */ cXyz mBreakParticleScale;  /* was m74: eff_break_tsubo 024D0550: the scale of the TUBOHAHEN / TUBOKONAGONA break particles (tww get_particle_scale) */
        /* 0x80 */ be<u32> mBreakTexIdx;  /* was m80: eff_break_tsubo 024D0550: the texture-pattern index of the shards and the tsubo select-texture callback index (tww getTsuboSelectTexEcallBack(it)) */
        /* 0x84 */ be<s32> mSoundID_Break;
        /* 0x88 */ be<s32> mSoundID_Land;  /* was m88: damage_bg_proc_directly 024CDAB4: fopAcM_seStart of a landing that does not break the pot (material sound, 0x2D for the bomb flower) (tww the same) */
        /* 0x8C */ be<s32> mSoundID_FallLava;
        /* 0x90 */ be<s32> mSoundID_FallWater;
        /* 0x94 */ be<s32> mSoundID_Hit;
        /* 0x98 */ be<s16> mCullSphX_Move;
        /* 0x9A */ be<s16> mCullSphY_Move;
        /* 0x9C */ be<s16> mCullSphZ_Move;
        /* 0x9E */ be<s16> mCullSphR_Move;
        /* 0xA0 */ u8 mA0[4];
        /* 0xA4 */ be<s16> mCullSphX_Draw;
        /* 0xA6 */ be<s16> mCullSphY_Draw;
        /* 0xA8 */ be<s16> mCullSphZ_Draw;
        /* 0xAA */ be<s16> mCullSphR_Draw;
        /* 0xAC */ u8 mAC[4];
        /* 0xB0 */ be<f32> mCullSizeFar;  /* was mB0: create_init_cull 024CA690: on the sea stage cullSizeFar = it / the stage's cull point (tww fopAcM_setCullSizeFar the same) */
        /* 0xB4 */ u8 mB4[4];
        /* 0xB8 */ be<u32> mHeapSize;
        /* 0xBC */ be<f32> mWpillarScaleXZ;  /* was mBC: eff_hit_water_splash 024CCCEC: fopKyM_createWpillar's scaleXZ (tww the same) */
        /* 0xC0 */ be<f32> mWpillarScaleY;  /* was mC0: eff_hit_water_splash 024CCCEC: fopKyM_createWpillar's scaleY (tww the same) */
        /* 0xC4 */ be<f32> mMpillarSize;  /* was mC4: eff_hit_lava_splash 024CD0B8: fopKyM_createMpillar's size (tww the same) */
        /* 0xC8 */ be<f32> mLandEffectScale;  /* was mC8: eff_land_smoke 024CDA98: daObj::make_land_effect's scale (tww the same) */
    };

    struct AttrSpine_c {
        /* 0x00 */ be<f32> mSwellDampXZ;  /* was m00: the bomb flower's regrow swell: tww mode_appear (spec 4) damps the x/z scale speed m808 by m808 * it each frame; WWHD mode_appear 024CEB50 folds it into the constant 0.55 (no WWHD reader of the table) */
        /* 0x04 */ be<f32> mSwellDampY;  /* was m04: tww mode_appear (spec 4) damps the y scale speed m80C by m80C * it each frame; WWHD mode_appear 024CEB50 folds it into the constant 0.55 */
        /* 0x08 */ be<f32> mSwellSpringXZ;  /* was m08: tww mode_appear (spec 4): m808 -= (scale.x - 1) * it, the spring back to scale 1; WWHD mode_appear 024CEB50 folds it into the constant 0.7 */
        /* 0x0C */ be<f32> mSwellSpringY;  /* was m0C: tww mode_appear (spec 4): m80C -= (scale.y - 1) * it; WWHD mode_appear 024CEB50 folds it into the constant 0.7 */
        /* 0x10 */ be<f32> mSwellSpeedXZInit;  /* was m10: tww mode_appear_init (spec 4) sets m808 to it; WWHD mode_appear_init 024CAB30 folds it into the constant 0.2 (commented M_attrSpine.mSwellSpeedXZInit) */
        /* 0x14 */ be<f32> mSwellSpeedYInit;  /* was m14: tww mode_appear_init (spec 4) sets m80C to it; WWHD mode_appear_init 024CAB30 folds it into the constant 0.2 (commented M_attrSpine.mSwellSpeedYInit) */
        /* 0x18 */ be<s16> mRegrowTime;  /* was m18: tww mode_appear_init (spec 4) sets the appear timer m802 to it; WWHD mode_appear_init 024CAB30 folds it into 0x50 (commented M_attrSpine.mRegrowTime) */
        /* 0x1A */ be<s16> mRegrowGrowEnd;  /* was m1A: tww mode_appear (spec 4): while the timer m802 is at least it the scale eases to mRegrowScale, below it the swell spring runs; WWHD mode_appear 024CEB50 folds it into 0xE (commented) */
        /* 0x1C */ be<s16> mRecoverSeTime;  /* was m1C: tww mode_appear (spec 4): JA_SE_OBJ_BOKKURI_RECOVER when the timer m802 reaches it; WWHD mode_appear 024CEB50 folds it into 0x46 (commented) */
        /* 0x1E */ be<s16> mSwellSeTime;  /* was m1E: tww mode_appear (spec 4): JA_SE_OBJ_BOKKURI_SWELL when the timer m802 reaches it; WWHD mode_appear 024CEB50 folds it into 0xD (commented) */
        /* 0x20 */ be<f32> mRegrowScale;  /* was m20: tww mode_appear (spec 4): cLib_addCalc's target of scale.x in the grow phase; WWHD mode_appear 024CEB50 folds it into 0.5 */
        /* 0x24 */ be<f32> mRegrowScaleRate;  /* was m24: tww mode_appear (spec 4): cLib_addCalc's scale (fraction) in the grow phase; WWHD mode_appear 024CEB50 folds it into 0.1 */
        /* 0x28 */ be<f32> mRegrowStepMax;  /* was m28: tww mode_appear (spec 4): cLib_addCalc's max step in the grow phase; WWHD mode_appear 024CEB50 folds it into 0.02 */
        /* 0x2C */ be<f32> mRegrowStepMin;  /* was m2C: tww mode_appear (spec 4): cLib_addCalc's min step in the grow phase; WWHD mode_appear 024CEB50 folds it into 0.001 */
        /* 0x30 */ be<s16> mFuseTime;  /* was m30: the bomb flower's fuse: tww _create / mode_carry_init set m800 to it, _draw shows the brk frame (1 - m800 / it); WWHD folds it into 0x4B0 / 1200 (commented M_attrSpine.mFuseTime in _create 024CB5EC, mode_carry_init 024CD4C8, damage_cc_proc 024CC7EC and _draw 024CE994); damage_kill_proc 024CD3CC explodes the pot when m800 runs out */
        /* 0x32 */ u8 _32[2];
    };

    struct SpecBokoData {
        /* 0x00 */ be<s16> mYaw;  /* was m00: spec_make_boko 024CAD38: the stuck boko stick's yaw around the pot (copied into M7A0 mStickYaw) (tww the same) */
        /* 0x02 */ be<s16> mTiltX;  /* was m02: spec_mtx 024CB118: the stick's X rotation (XrotM) (tww the same) */
        /* 0x04 */ be<f32> mRestHeight;  /* was m04: spec_make_boko 024CAD38 / spec_mode_put_init 024CF77C: the stick's start height (M7A0 mHeight); spec_mode_carry 024CF320 bounces it back above it (tww the same) */
        /* 0x08 */ be<f32> mOffsetZ;  /* was m08: spec_mtx 024CB118: the stick's z offset (transM(0, M7A0 mHeight, it)) (tww the same) */
    };

    struct M7A0 {
        /* 0x00 */ be<u32> mBokoId;  /* was m00: spec_make_boko 024CAD38: fopAcM_create's process id of the stuck boko stick; spec_init 024CAF78 sets fpcM_ERROR_PROCESS_ID_e (tww the same); fpc_ProcID */
        /* 0x04 */ be<u8> mbCarried;  /* was m04: spec_carry_spec 024CAE78 sets 1 after tsuboSetCarryNow on the stick; spec_remove 024CBB58 / spec_mode_carry 024CF320 / spec_mode_put_init 024CF77C act only when it is set (tww the same) */
        /* 0x05 */ u8 _05;
        /* 0x06 */ be<s16> mStickYaw;  /* was m06: spec_make_boko 024CAD38 copies SpecBokoData mYaw; spec_mtx 024CB118 turns the stick by -it, spec_remove 024CBB58 throws it off at shape_angle.y - it - 0x8000 (tww the same) */
        /* 0x08 */ be<s16> m08;
        /* 0x0A */ u8 _0A[2];
        /* 0x0C */ be<f32> m0C;
        /* 0x10 */ be<f32> mHeight;  /* was m10: spec_make_boko 024CAD38 / spec_mode_put_init 024CF77C set SpecBokoData mRestHeight; spec_mode_carry 024CF320 moves it by mHeightSpeed; spec_mtx 024CB118: transM(0, it, mOffsetZ) (tww the same) */
        /* 0x14 */ be<f32> mHeightSpeed;  /* was m14: spec_mode_carry 024CF320: the per-frame step of mHeight (random kick, -0.8 per frame, halved and reversed at the bottom) (tww the same) */
        /* 0x18 */ be<s8> mBounceCount;  /* was m18: spec_mode_carry 024CF320: set to 2 at a kick, decremented at each bounce at the bottom; at 0 the stick rests (tww the same) */
        /* 0x19 */ u8 _19[3];
    };

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<mDoExt_brkAnm> mpBrk;
    /* 0x3BC */ dBgS_ObjAcch mAcch;
    /* 0x580 */ dBgS_AcchCir mAcchCir;
    /* 0x5C0 */ dBgS_GndChk_l mGndChkYogan;
    /* 0x614 */ be<f32> mLavaY;  /* was m4F8: crr_pos_lava 024CD70C: cBgS_GroundCross of the lava (Yogan) ground check, else -inf; damaged_lava 024CD108 tests it */
    /* 0x618 */ be<f32> mWaterY;  /* was m4FC: crr_pos_water 024CD5E0: mAcch's water height, else -inf; chk_sink_water / eff_hit_water_splash use it */
    /* 0x61C */ be<f32> mWaterYOld;  /* was m500: crr_pos_water 024CD5E0 keeps last frame's mWaterY here; water_tention 024CFA48 uses the difference */
    /* 0x620 */ be<u8> m504;
    /* 0x621 */ be<u8> m505;
    /* 0x622 */ u8 _622[2];
    /* 0x624 */ dCcD_Cyl mCyl;
    /* 0x754 */ dCcD_Stts mStts;
    /* 0x790 */ be<s32> mType;
    /* 0x794 */ be<s32> mMode;  /* was m678: mode_hide_init 024CAAD0 sets 0, mode_appear_init 024CAB30 1, mode_carry_init 024CD4C8 4, ...; set_mtx 024CB280 tests == 4 */
    /* 0x798 */ be<u16> mPrmZ;
    /* 0x79A */ be<u8> m67E;
    /* 0x79B */ be<u8> m67F;
    /* 0x79C */ be<u8> m680;
    /* 0x79D */ be<s8> m681;
    /* 0x79E */ be<u8> m682;
    /* 0x79F */ be<u8> m683;
    /* 0x7A0 */ be<u8> m684;
    /* 0x7A1 */ be<u8> m685;
    /* 0x7A2 */ be<u8> m686;
    /* 0x7A3 */ u8 _7A3;
    /* 0x7A4 */ be<s16> mRotAxisRoll;  /* was m688: the roll of the pose rotation axis: the set_tensor_* tail (set_tensor_wait 024CFEC8) turns BaseX by YrotS(mRotAxisYaw) XrotM(mRotAxisPitch) ZrotM(it) and set_tensor 024CB088 rotates the pose about that axis by mRotAngle; init_rot_throw 024CF4E0 sets data mThrowRotAxisRollMax * cM_rnd(), init_rot_clean 024CAA54 / pos_init 0 (tww d_a_tsubo.cpp the same); cSAngle */
    /* 0x7A6 */ be<s16> mRotAxisPitch;  /* was m68A: cSAngle; the pitch of the pose rotation axis (XrotM in the set_tensor_* tail, set_tensor_wait 024CFEC8); init_rot_throw 024CF4E0 sets cM_rndFX(0x8000), set_tensor_drop 024D0074 / sink 024D0184 / afl 024D0294 add mRotAxisPitchSpeed to it each frame; init_rot_clean 024CAA54 / pos_init 0 (tww the same) */
    /* 0x7A8 */ be<s16> mRotAxisPitchSpeed;  /* was m68C: cSAngle; the per-frame step of mRotAxisPitch: init_rot_throw 024CF4E0 sets data mThrowRotAxisPitchSpeed, set_tensor_drop 024D0074 (data mDropRotResistLinear/Quad) and sink 024D0184 / afl 024D0294 (data mWaterRotResistLinear/Quad) damp it and add it to mRotAxisPitch; init_rot_clean 024CAA54 / pos_init 0 (tww the same) */
    /* 0x7AA */ be<s16> mRotAngle;  /* was m68E: cSAngle; the angle set_tensor 024CB088 rotates the pose matrix by about the axis (rotAxisRad, cM_s2rad); set_tensor_walk 024CFF48 / drop 024D0074 / sink 024D0184 / afl 024D0294 add mRotAngleSpeed to it each frame; init_rot_throw 024CF4E0 / init_rot_clean 024CAA54 / pos_init 0 (tww the same) */
    /* 0x7AC */ be<s16> mRotAngleSpeed;  /* was m690: cSAngle; the per-frame step of mRotAngle: set_tensor_walk 024CFF48 eases it towards twice the distance moved over data mAcchRoofHeight (the pot rolls as it moves), set_tensor_drop 024D0074 / sink 024D0184 damp it, set_tensor_afl 024D0294 also pushes it back by mRotAngle and a sine sway; init_rot_throw 024CF4E0 sets data mThrowRotAngleSpeed, init_rot_clean 024CAA54 / pos_init 0 (tww the same) */
    /* 0x7AE */ be<s16> mRotAxisYaw;  /* was m692: cSAngle; the yaw of the pose rotation axis (YrotS in the set_tensor_* tail, set_tensor_wait 024CFEC8); init_rot_throw 024CF4E0 / init_rot_clean 024CAA54 / pos_init set it to current.angle.y and set_tensor_walk 024CFF48 copies current.angle.y each frame (tww the same) */
    /* 0x7B0 */ be<f32> m694[4];       /* Quaternion x, y, z, w */
    /* 0x7C0 */ u8 m6A4[0xC];
    /* 0x7CC */ cXyz m6B0;
    /* 0x7D8 */ Mtx34 mPoseMtx;
    /* 0x808 */ be<f32> mOffsetY;  /* was m6EC: set_mtx 024CB280 adds it to the model's y translation; _create 024CB5EC clears it; moment_proc_call 024CE204 sets it */
    /* 0x80C */ cXyz m6F0;
    /* 0x818 */ be<f32> mFallStartY;  /* was m6FC: _create 024CB5EC sets current.pos.y; damage_bg_proc_directly 024CDAB4 breaks the pot when it minus current.pos.y exceeds the attribute's fall height (mBreakFallHeight) */
    /* 0x81C */ cXyz mWaterDropPos;  /* was m700: set_mtx 024CB280: the model matrix times offsetPos; eff_drop_water 024CD95C sets dPa_ID_IT_SN_WPOT_PITYA there */
    /* 0x828 */ dPa_followEcallBack m70C[3];
    /* 0x864 */ dPa_followEcallBack m748[3];
    /* 0x8A0 */ dPa_followEcallBack mKutaniCb;  /* was m784: the constructor 024CA1DC constructs it (dPa_followEcallBack); eff_kutani_set 024CE3D0 uses it; _delete 024CBC98 removes it */
    /* 0x8B4 */ be<u8> mKutaniTimer;  /* was m798: eff_kutani_init 024CACEC sets 1 + rnd(30); eff_kutani_set 024CE3D0 acts while it is not 0 */
    /* 0x8B5 */ u8 m799[7];
    /* 0x8BC */ M7A0 m7A0[3];
    /* 0x910 */ cXyz mWindVec;  /* was m7F4: _create 024CB5EC clears it; set_wind_vec 024CC244 sets the wind vector (PSVECSquareMag); mode_walk / mode_drop / mode_afl use it */
    /* 0x91C */ be<s16> m800;
    /* 0x91E */ be<s16> m802;
    /* 0x920 */ be<s16> m804;
    /* 0x922 */ be<s16> m806;
    /* 0x924 */ be<f32> m808;
    /* 0x928 */ be<f32> m80C;
    /* 0x92C */ be<u8> m810;
    /* 0x92D */ be<u8> m811;
    /* 0x92E */ u8 _92E[2];
    /* 0x930 */ cXyz m814;
};
WWHD_OFFSET(Act_c, mAcch, 0x3BC);
WWHD_OFFSET(Act_c, mAcchCir, 0x580);
WWHD_OFFSET(Act_c, mCyl, 0x624);
WWHD_OFFSET(Act_c, mStts, 0x754);
WWHD_OFFSET(Act_c, mType, 0x790);
WWHD_OFFSET(Act_c, mRotAxisRoll, 0x7A4);
WWHD_OFFSET(Act_c, mPoseMtx, 0x7D8);
WWHD_OFFSET(Act_c, m70C, 0x828);
WWHD_OFFSET(Act_c, m7A0, 0x8BC);
WWHD_OFFSET(Act_c, m814, 0x930);
WWHD_SIZE(Act_c::Data_c, 0xCC);
WWHD_SIZE(Act_c::AttrSpine_c, 0x34);
WWHD_SIZE(Act_c::M7A0, 0x1C);
WWHD_SIZE(Act_c, 0x93C);

/* ---- this TU's addresses ---- */
#define TSUBO_SAFESTRING_VTBL 0x10040AD4 /* sead::SafeString vtable (per TU) */
#define TSUBO_ACT_VTBL 0x10040BFC        /* Act_c vtable (HD virtual destructor) */
#define TSUBO_CYL_AAB_VTBL 0x10040AEC    /* cM3dGAab vtable (per TU) */
static const dBgS_ObjAcch_vt TSUBO_OBJACCH_VT = {0x10040BCC, 0x10040BEC, 0x10040BDC};
/* dBgS_ObjGndChk_Yogan vtables at +0x10/+0x20/+0x40/+0x4C */
static const dBgS_GndChk_vt TSUBO_YOGAN_VT = {0x10040B8C, 0x10040B9C, 0x10040BBC, 0x10040BAC};

/* const tables (.rodata): GHS folds the constant members of M_attrSpine / M_data_spec_boko into
 * immediates where it can; where the code reads them, these accessors give the addresses */
inline Act_c::Data_c* M_data(s32 type) { return gabi::at<Act_c::Data_c>(0x10040FAC + 0xCC * type); }
inline const char* M_arcname(s32 type) { return gabi::at<const char>(gabi::load<u32>(0x10040F00 + 4 * type)); }
inline Act_c::SpecBokoData* M_data_spec_boko(s32 i) { return gabi::at<Act_c::SpecBokoData>(0x10040F40 + 0xC * i); }
#define M_cyl_src gabi::at<dCcD_SrcCyl>(0x10040F68)
/* static fopAc_ac_c* Act_c::M_spec_act[3] */
inline gptr<fopAc_ac_c>* M_spec_act() { return gabi::at<gptr<fopAc_ac_c>>(0x1046EA94); }
/* the data of this object's type: mType is reloaded at each use */
inline Act_c::Data_c* data(Act_c* a) { return M_data(a->mType); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02048038 daObj::PrmAbstract(actor, width, shift): (mParameters >> shift) & ((1 << width) - 1)
 * (one shared out-of-line copy in HD) */
inline u32 daObj_PrmAbstract(const fopAc_ac_c* a, s32 w, s32 s) { return gabi::call<u32>(0x02048038, a, w, s); }
inline s32 prm_get_type(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_TYPE_W, Act_c::PRM_TYPE_S); }
inline s32 prm_get_spec(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_SPEC_W, Act_c::PRM_SPEC_S); }
inline s32 prm_get_cull(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_CULL_W, Act_c::PRM_CULL_S); }
inline s32 prm_get_itemNo(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_ITEMNO_W, Act_c::PRM_ITEMNO_S); }
inline s32 prm_get_itemSave(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_ITEMSAVE_W, Act_c::PRM_ITEMSAVE_S); }
inline bool prm_get_moveBg(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_MOVEBG_W, Act_c::PRM_MOVEBG_S) == 1u; }
inline u32 prm_get_stick(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_STICK_W, Act_c::PRM_STICK_S); }
inline void prm_set_cull_non(Act_c* a) { a->mParameters = a->mParameters & ~0x70000000u; }
inline void prm_off_moveBg(Act_c* a) { a->mParameters = a->mParameters | 0xC000u; }
inline void prm_off_stick(Act_c* a) { a->mParameters = a->mParameters & ~0x80000000u; }
inline bool is_switch(Act_c* a) { return fopAcM_isSwitch(a, a->mPrmZ & 0xFF); }
inline bool spec_is_boko(s32 spec) { return spec == 1 || spec == 2 || spec == 3; }

/* cSAngle (c_angle, out of line in HD): 020065FC cSAngle() (allocates when this == NULL),
 * 02006638 Val(const cSAngle&), 02006584 Val(s16), 020068CC operator+=(const cSAngle&),
 * 0200691C operator+=(s16), 0200692C operator-=(s16), 020069A0 operator*=(f32) */
inline void cSAngle_ct(be<s16>* a) { gabi::call(0x020065FC, a); }
inline void cSAngle_Val(be<s16>* a, const be<s16>* b) { gabi::call(0x02006638, a, b); }
inline void cSAngle_Val(be<s16>* a, s16 v) { gabi::call(0x02006584, a, v); }
inline void cSAngle_addeq(be<s16>* a, const be<s16>* b) { gabi::call(0x020068CC, a, b); }
inline void cSAngle_addeq(be<s16>* a, s16 v) { gabi::call(0x0200691C, a, v); }
inline void cSAngle_subeq(be<s16>* a, s16 v) { gabi::call(0x0200692C, a, v); }
inline void cSAngle_muleq(be<s16>* a, f32 f) { gabi::call(0x020069A0, a, f); }
#define cSAngle__0 gabi::at<be<s16>>(0x101FF354) /* cSAngle::_0 */

/* dBgS_Acch inline flag setters missing from d_bg_s.h */
inline void Acch_ClrRoofNone(dBgS_Acch* a) { a->m_flags &= ~(u32)dBgS_Acch::ROOF_NONE; }
inline void Acch_ClrWallNone(dBgS_Acch* a) { a->m_flags &= ~(u32)dBgS_Acch::WALL_NONE; }
inline void Acch_ClrWaterNone(dBgS_Acch* a) { a->m_flags &= ~(u32)dBgS_Acch::WATER_NONE; }
inline void Acch_ClrGroundLanding(dBgS_Acch* a) { a->m_flags &= ~(u32)dBgS_Acch::GROUND_LANDING; }
inline u32 Acch_MaskWaterIn(dBgS_Acch* a) { return a->m_flags & dBgS_Acch::WATER_IN; }
inline cBgS_PolyInfo* Acch_gnd(dBgS_Acch* a) { return gabi::at<cBgS_PolyInfo>(gabi::ea(a) + 0xD4 + 0x14); }

/* attention_info.flags (+0x39C), distances (+0x388..), position (+0x390) */
inline void attn_onBit(fopAc_ac_c* a, u32 b) { u32 p = gabi::ea(a) + 0x39C; gabi::store<u32>(p, gabi::load<u32>(p) | b); }
inline void attn_offBit(fopAc_ac_c* a, u32 b) { u32 p = gabi::ea(a) + 0x39C; gabi::store<u32>(p, gabi::load<u32>(p) & ~b); }
/* fopAc_Attn_ACTION_CARRY_e: gc_api/f_op/f_op_actor.h */

/* HD fopAcM_seStart inline where the actor is known non-null (only &eyePos is checked) */
inline void fopAcM_seStart_nn(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0)
        mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}

/* HD: strcmp(s, lit) == 0 as a sead::SafeString comparison (see d_a_ki_execute.cpp): `lit` is the
 * literal, `str` the other string (computed after the literal's SafeString is built) */
template <class F> inline bool sead_streq(u32 lit, F str_fn) {
    gabi::Local<SafeString> a;
    a->mTextPtr = lit;
    a->__vtbl = TSUBO_SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    {
        u32 s = str_fn();
        b->__vtbl = TSUBO_SAFESTRING_VTBL;
        b->mTextPtr = s;
    }
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 bv = b->__vtbl;
    u32 s1 = a->mTextPtr;
    gabi::call_ptr(gabi::load<u32>(bv + 0x14), b.get());
    u32 s2 = b->mTextPtr;
    if (s1 == s2) return true;
    for (u32 n = 0x40001; n != 0; n--) {
        u8 c1 = gabi::load<u8>(s1);
        u8 c2 = gabi::load<u8>(s2);
        if (c1 != c2) return false;
        if (c1 == 0) return true;
        s1++;
        s2++;
    }
    return false;
}

/* GameCube inline Act_c::pos_init (cSAngle::Val out of line, mDoMtx_identity = PSMTXIdentity) [v nz] */
inline bool Act_c::pos_init() {
    if (mMode == 2) {
        current.pos.set(home.pos);
        current.angle = home.angle;
        shape_angle = home.angle;
        cSAngle_Val(&mRotAxisRoll, cSAngle__0);
        cSAngle_Val(&mRotAxisPitch, cSAngle__0);
        cSAngle_Val(&mRotAxisPitchSpeed, cSAngle__0);
        cSAngle_Val(&mRotAngle, cSAngle__0);
        cSAngle_Val(&mRotAngleSpeed, cSAngle__0);
        cSAngle_Val(&mRotAxisYaw, (s16)current.angle.y);
        gabi::call(0x028E9098, &mPoseMtx);
        return true;
    }
    return false;
}
} // namespace daTsubo

# Field names: objects

Offset-named fields of the object classes (`Act_c` of the object actors, then the other `daObj*_c` classes), named in
the field naming pass (2026-11-10; rules in docs/decomp-notes.md, "Field names (offset-named fields)").
A name comes from how the original code uses the field (the address of the use and what it shows), from names already
in this decomp, or from the CC0 zeldaret/tww and zeldaret/tp decomps ("TWW": an inline accessor of the same field in
zeldaret/tww d217e2b). Renames only: offsets, types and code are unchanged (objcmp: object-identical). Each header
declaration keeps the old name (`was mXXX`) with the evidence. Fields without enough evidence keep their offset name
and are listed as open.

## daTsubo::Act_c (d_a_tsubo.h; copy in d_a_tsubo_main.h; view Ji1EventTsuboHD in d_a_npc_ji1_event.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`). The partial copies daTsuboDamage::Act_c (d_a_tsubo_dmg.h) and daTsuboMode::Act_c (d_a_tsubo_mode.h) declare none of these fields.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x614 | `m4F8` | `mLavaY` | crr_pos_lava 024CD70C: cBgS_GroundCross of the lava (Yogan) ground check, else -inf; damaged_lava 024CD108 tests it |
| 0x618 | `m4FC` | `mWaterY` | crr_pos_water 024CD5E0: mAcch's water height, else -inf; chk_sink_water / eff_hit_water_splash use it |
| 0x61C | `m500` | `mWaterYOld` | crr_pos_water 024CD5E0 keeps last frame's mWaterY here; water_tention 024CFA48 uses the difference |
| 0x794 | `m678` | `mMode` | mode_hide_init 024CAAD0 sets 0, mode_appear_init 024CAB30 1, mode_carry_init 024CD4C8 4, ...; set_mtx 024CB280 tests == 4 |
| 0x7A4 | `m688` | `mRotAxisRoll` | the roll of the pose rotation axis: the set_tensor_* tail (set_tensor_wait 024CFEC8) turns BaseX by YrotS(mRotAxisYaw) XrotM(mRotAxisPitch) ZrotM(it) and set_tensor 024CB088 rotates the pose about that axis by mRotAngle; init_rot_throw 024CF4E0 sets data mThrowRotAxisRollMax * cM_rnd(), init_rot_clean 024CAA54 / pos_init 0 (tww d_a_tsubo.cpp the same) |
| 0x7A6 | `m68A` | `mRotAxisPitch` | cSAngle; the pitch of the pose rotation axis (XrotM in the set_tensor_* tail, set_tensor_wait 024CFEC8); init_rot_throw 024CF4E0 sets cM_rndFX(0x8000), set_tensor_drop 024D0074 / sink 024D0184 / afl 024D0294 add mRotAxisPitchSpeed to it each frame; init_rot_clean 024CAA54 / pos_init 0 (tww the same) |
| 0x7A8 | `m68C` | `mRotAxisPitchSpeed` | cSAngle; the per-frame step of mRotAxisPitch: init_rot_throw 024CF4E0 sets data mThrowRotAxisPitchSpeed, set_tensor_drop 024D0074 (data mDropRotResistLinear/Quad) and sink 024D0184 / afl 024D0294 (data mWaterRotResistLinear/Quad) damp it and add it to mRotAxisPitch; init_rot_clean 024CAA54 / pos_init 0 (tww the same) |
| 0x7AA | `m68E` | `mRotAngle` | cSAngle; the angle set_tensor 024CB088 rotates the pose matrix by about the axis (rotAxisRad, cM_s2rad); set_tensor_walk 024CFF48 / drop 024D0074 / sink 024D0184 / afl 024D0294 add mRotAngleSpeed to it each frame; init_rot_throw 024CF4E0 / init_rot_clean 024CAA54 / pos_init 0 (tww the same) |
| 0x7AC | `m690` | `mRotAngleSpeed` | cSAngle; the per-frame step of mRotAngle: set_tensor_walk 024CFF48 eases it towards twice the distance moved over data mAcchRoofHeight (the pot rolls as it moves), set_tensor_drop 024D0074 / sink 024D0184 damp it, set_tensor_afl 024D0294 also pushes it back by mRotAngle and a sine sway; init_rot_throw 024CF4E0 sets data mThrowRotAngleSpeed, init_rot_clean 024CAA54 / pos_init 0 (tww the same) |
| 0x7AE | `m692` | `mRotAxisYaw` | cSAngle; the yaw of the pose rotation axis (YrotS in the set_tensor_* tail, set_tensor_wait 024CFEC8); init_rot_throw 024CF4E0 / init_rot_clean 024CAA54 / pos_init set it to current.angle.y and set_tensor_walk 024CFF48 copies current.angle.y each frame (tww the same) |
| 0x808 | `m6EC` | `mOffsetY` | set_mtx 024CB280 adds it to the model's y translation; _create 024CB5EC clears it; moment_proc_call 024CE204 sets it |
| 0x818 | `m6FC` | `mFallStartY` | _create 024CB5EC sets current.pos.y; damage_bg_proc_directly 024CDAB4 breaks the pot when it minus current.pos.y exceeds the attribute's fall height (mBreakFallHeight) |
| 0x81C | `m700` | `mWaterDropPos` | set_mtx 024CB280: the model matrix times offsetPos; eff_drop_water 024CD95C sets dPa_ID_IT_SN_WPOT_PITYA there |
| 0x8A0 | `m784` | `mKutaniCb` | the constructor 024CA1DC constructs it (dPa_followEcallBack); eff_kutani_set 024CE3D0 uses it; _delete 024CBC98 removes it |
| 0x8B4 | `m798` | `mKutaniTimer` | eff_kutani_init 024CACEC sets 1 + rnd(30); eff_kutani_set 024CE3D0 acts while it is not 0 |
| 0x910 | `m7F4` | `mWindVec` | _create 024CB5EC clears it; set_wind_vec 024CC244 sets the wind vector (PSVECSquareMag); mode_walk / mode_drop / mode_afl use it |

Open: `m504`, `m505`, `m67E`..`m686`, `m694` (quaternion, set to zero by init_mtx only), `m6A4`, `m6B0`, `m6F0`, the effect callbacks `m70C`, `m748`, `m799`, `m7A0`, `m800`..`m80C`, `m810`, `m811`, `m814`.


## daTsubo::Act_c::Data_c (M_data, the per-type table; d_a_tsubo.h, copy in d_a_tsubo_main.h)

Round 10. The table keeps its GameCube layout (0xCC per type, M_data at 0x10040FAC), so the offsets are tww's. Read through data(this) / d in d_a_tsubo*.cpp; tww reads it as data().mXX in d_a_tsubo.cpp (which keeps the offset names).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `m04` | `mCenterHeight` | set_mtx 024CB280 lifts the model by it and turns the pose about that height (transM back by -it); crr_pos_water 024CD5E0 / crr_pos_lava 024CD70C / chk_sink_lava 024CCB4C test the water and lava surface at pos.y + it (tww d_a_tsubo.cpp the same) |
| 0x10 | `m10` | `mWeight` | create_init_cc 024CA804 / mode_wait_init 024CAC44 / mode_drop_init 024CF5AC: mStts.Init's weight (tww the same) |
| 0x14 | `m14` | `mDropSpeedY` | mode_drop_init 024CF5AC: speed.y when the pot is let go (tww: unless m810 is set) |
| 0x18 | `m18` | `mDropSpeedF` | mode_drop_init 024CF5AC: speedF = it + the player's speedF * mDropSpeedFPlayerRate (tww the same) |
| 0x1C | `m1C` | `mDropSpeedFPlayerRate` | mode_drop_init 024CF5AC: the share of the player's speedF added to mDropSpeedF (tww the same) |
| 0x20 | `m20` | `mAirResistLinear` | calc_drop_param 024CED64 / calc_afl_param 024CFB20: posMoveF_stream's linear resistance above water (tww posMoveF_resist_acc param_4); mode_afl 024CFD3C scales the wind vector by it (tww the same) |
| 0x24 | `m24` | `mAirResistQuad` | calc_drop_param 024CED64 / calc_afl_param 024CFB20: posMoveF_stream's quadratic resistance above water (tww posMoveF_resist_acc param_5) |
| 0x28 | `m28` | `mThrowRotAxisRollMax` | init_rot_throw 024CF4E0: mRotAxisRoll = it * cM_rnd() (tww the same) |
| 0x2A | `m2A` | `mThrowRotAxisPitchSpeed` | init_rot_throw 024CF4E0: the start value of mRotAxisPitchSpeed (tww the same) |
| 0x2C | `m2C` | `mThrowRotAngleSpeed` | init_rot_throw 024CF4E0: the start value of mRotAngleSpeed (tww the same) |
| 0x30 | `m30` | `mDropRotResistLinear` | set_tensor_drop 024D0074: tensor_damp's linear damping of mRotAxisPitchSpeed / mRotAngleSpeed in the air (tww: speed -= rad2s(v * it + v * |v| * mDropRotResistQuad)) |
| 0x34 | `m34` | `mDropRotResistQuad` | set_tensor_drop 024D0074: tensor_damp's quadratic damping of mRotAxisPitchSpeed / mRotAngleSpeed in the air (tww the same) |
| 0x38 | `m38` | `mBreakFallHeight` | damage_bg_proc_directly 024CDAB4: the pot breaks on landing when mFallStartY - current.pos.y is above it (tww the same) |
| 0x3C | `m3C` | `mBuoyancy` | mode_sink_init 024CCE64: gravity = mGravity + it; damage_bg_proc 024CD1E8 tests mGravity + it < 0; calc_drop_param 024CED64 / calc_afl_param 024CFB20 add immersion * it to mGravity (tww the same) |
| 0x40 | `m40` | `mWaterResistLinear` | mode_sink 024CFAB8: posMoveF_stream's linear resistance under water; calc_drop_param 024CED64 / calc_afl_param 024CFB20 its immersion share (tww the same) |
| 0x44 | `m44` | `mWaterResistQuad` | mode_sink 024CFAB8: posMoveF_stream's quadratic resistance under water; calc_drop_param 024CED64 / calc_afl_param 024CFB20 its immersion share (tww the same) |
| 0x48 | `m48` | `mWaterRotResistLinear` | set_tensor_sink 024D0184 / set_tensor_afl 024D0294: tensor_damp's linear damping of mRotAxisPitchSpeed / mRotAngleSpeed in water (tww the same) |
| 0x4C | `m4C` | `mWaterRotResistQuad` | set_tensor_sink 024D0184 / set_tensor_afl 024D0294: tensor_damp's quadratic damping of mRotAxisPitchSpeed / mRotAngleSpeed in water (tww the same) |
| 0x50 | `m50` | `mWaterEntrySpeedMax` | limit_speed (inlined in mode_sink_init 024CCE64 / mode_afl_init 024CCD44): the speed (|speed.y, speedF|) is scaled down to it on entering water (tww mode_sink_init / mode_afl_init the same) |
| 0x56 | `m56` | `mNoGradeAngle` | mode_walk 024CF094: posMoveF_grade's no_grade_cos is cos(it - 0.5 degrees) (tww the same) |
| 0x5C | `m5C` | `mGradeFriction` | mode_walk 024CF094: posMoveF_grade's friction (tww the same) |
| 0x60 | `m60` | `mBreakNoiseLevel` | se_break 024CBE2C: set_senv's level (dKy_Sound_set's mNoiseLevel) of the break noise (tww the same) |
| 0x61 | `m61` | `mBreakNoiseTimer` | se_break 024CBE2C: set_senv's timer (dKy_Sound_set's mNoiseTimer) of the break noise (tww the same) |
| 0x62 | `m62` | `mHitNoiseLevel` | damage_cc_proc 024CC7EC: set_senv's level (dKy_Sound_set's mNoiseLevel) of the hit noise (tww the same) |
| 0x63 | `m63` | `mHitNoiseTimer` | damage_cc_proc 024CC7EC: set_senv's timer (dKy_Sound_set's mNoiseTimer) of the hit noise (tww the same) |
| 0x64 | `m64` | `mFallInNoiseLevel` | se_fall_water 024CCBA0 / se_fall_lava 024CCF84: set_senv's level (dKy_Sound_set's mNoiseLevel) of the splash noise (tww the same) |
| 0x65 | `m65` | `mFallInNoiseTimer` | se_fall_water 024CCBA0 / se_fall_lava 024CCF84: set_senv's timer (dKy_Sound_set's mNoiseTimer) of the splash noise (tww the same) |
| 0x6C | `m6C` | `mModelIdx` | create_heap 024CA3D0: the model's resource index in M_arcname(mType) (tww the same) |
| 0x6E | `m6E` | `mShadowSize` | never read in WWHD (the HD _draw draws no simple shadow); tww _draw passes scale.x * it as dComIfGd_setSimpleShadow2's size |
| 0x70 | `m70` | `mCylHeight` | create_init_cc 024CA804: mCyl.SetH (tww the same; mAcchCirRad is its radius) |
| 0x74 | `m74` | `mBreakParticleScale` | eff_break_tsubo 024D0550: the scale of the TUBOHAHEN / TUBOKONAGONA break particles (tww get_particle_scale) |
| 0x80 | `m80` | `mBreakTexIdx` | eff_break_tsubo 024D0550: the texture-pattern index of the shards and the tsubo select-texture callback index (tww getTsuboSelectTexEcallBack(it)) |
| 0x88 | `m88` | `mSoundID_Land` | damage_bg_proc_directly 024CDAB4: fopAcM_seStart of a landing that does not break the pot (material sound, 0x2D for the bomb flower) (tww the same) |
| 0xB0 | `mB0` | `mCullSizeFar` | create_init_cull 024CA690: on the sea stage cullSizeFar = it / the stage's cull point (tww fopAcM_setCullSizeFar the same) |
| 0xBC | `mBC` | `mWpillarScaleXZ` | eff_hit_water_splash 024CCCEC: fopKyM_createWpillar's scaleXZ (tww the same) |
| 0xC0 | `mC0` | `mWpillarScaleY` | eff_hit_water_splash 024CCCEC: fopKyM_createWpillar's scaleY (tww the same) |
| 0xC4 | `mC4` | `mMpillarSize` | eff_hit_lava_splash 024CD0B8: fopKyM_createMpillar's size (tww the same) |
| 0xC8 | `mC8` | `mLandEffectScale` | eff_land_smoke 024CDA98: daObj::make_land_effect's scale (tww the same) |

Open (no name yet): `m54` and `m58` (read nowhere in WWHD or tww; only the table values); padding `mA0`, `mAC`, `mB4`.


## daTsubo::Act_c::AttrSpine_c (M_attrSpine, the bomb flower's regrow and fuse constants; d_a_tsubo.h, copy in d_a_tsubo_main.h)

Round 10. GHS folds this const table into immediates (the code comments name M_attrSpine.<field>), so no WWHD code reads the struct; named under the 2026-11-11 rule from tww's reads (tww d_a_tsubo.cpp mode_appear_init / mode_appear (spec 4) / _create / _draw read M_attrSpine.mXX with one role each) and the matching folded constants in WWHD.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mSwellDampXZ` | the bomb flower's regrow swell: tww mode_appear (spec 4) damps the x/z scale speed m808 by m808 * it each frame; WWHD mode_appear 024CEB50 folds it into the constant 0.55 (no WWHD reader of the table) |
| 0x04 | `m04` | `mSwellDampY` | tww mode_appear (spec 4) damps the y scale speed m80C by m80C * it each frame; WWHD mode_appear 024CEB50 folds it into the constant 0.55 |
| 0x08 | `m08` | `mSwellSpringXZ` | tww mode_appear (spec 4): m808 -= (scale.x - 1) * it, the spring back to scale 1; WWHD mode_appear 024CEB50 folds it into the constant 0.7 |
| 0x0C | `m0C` | `mSwellSpringY` | tww mode_appear (spec 4): m80C -= (scale.y - 1) * it; WWHD mode_appear 024CEB50 folds it into the constant 0.7 |
| 0x10 | `m10` | `mSwellSpeedXZInit` | tww mode_appear_init (spec 4) sets m808 to it; WWHD mode_appear_init 024CAB30 folds it into the constant 0.2 (commented M_attrSpine.mSwellSpeedXZInit) |
| 0x14 | `m14` | `mSwellSpeedYInit` | tww mode_appear_init (spec 4) sets m80C to it; WWHD mode_appear_init 024CAB30 folds it into the constant 0.2 (commented M_attrSpine.mSwellSpeedYInit) |
| 0x18 | `m18` | `mRegrowTime` | tww mode_appear_init (spec 4) sets the appear timer m802 to it; WWHD mode_appear_init 024CAB30 folds it into 0x50 (commented M_attrSpine.mRegrowTime) |
| 0x1A | `m1A` | `mRegrowGrowEnd` | tww mode_appear (spec 4): while the timer m802 is at least it the scale eases to mRegrowScale, below it the swell spring runs; WWHD mode_appear 024CEB50 folds it into 0xE (commented) |
| 0x1C | `m1C` | `mRecoverSeTime` | tww mode_appear (spec 4): JA_SE_OBJ_BOKKURI_RECOVER when the timer m802 reaches it; WWHD mode_appear 024CEB50 folds it into 0x46 (commented) |
| 0x1E | `m1E` | `mSwellSeTime` | tww mode_appear (spec 4): JA_SE_OBJ_BOKKURI_SWELL when the timer m802 reaches it; WWHD mode_appear 024CEB50 folds it into 0xD (commented) |
| 0x20 | `m20` | `mRegrowScale` | tww mode_appear (spec 4): cLib_addCalc's target of scale.x in the grow phase; WWHD mode_appear 024CEB50 folds it into 0.5 |
| 0x24 | `m24` | `mRegrowScaleRate` | tww mode_appear (spec 4): cLib_addCalc's scale (fraction) in the grow phase; WWHD mode_appear 024CEB50 folds it into 0.1 |
| 0x28 | `m28` | `mRegrowStepMax` | tww mode_appear (spec 4): cLib_addCalc's max step in the grow phase; WWHD mode_appear 024CEB50 folds it into 0.02 |
| 0x2C | `m2C` | `mRegrowStepMin` | tww mode_appear (spec 4): cLib_addCalc's min step in the grow phase; WWHD mode_appear 024CEB50 folds it into 0.001 |
| 0x30 | `m30` | `mFuseTime` | the bomb flower's fuse: tww _create / mode_carry_init set m800 to it, _draw shows the brk frame (1 - m800 / it); WWHD folds it into 0x4B0 / 1200 (commented M_attrSpine.mFuseTime in _create 024CB5EC, mode_carry_init 024CD4C8, damage_cc_proc 024CC7EC and _draw 024CE994); damage_kill_proc 024CD3CC explodes the pot when m800 runs out |


## daTsubo::Act_c::SpecBokoData (M_data_spec_boko, the boko sticks stuck in a pot; d_a_tsubo.h, copy in d_a_tsubo_main.h)

Round 10. One entry per stick (spec 1..3); WWHD reads it through M_data_spec_boko(i), tww through data_spec_boko(i).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mYaw` | spec_make_boko 024CAD38: the stuck boko stick's yaw around the pot (copied into M7A0 mStickYaw) (tww the same) |
| 0x02 | `m02` | `mTiltX` | spec_mtx 024CB118: the stick's X rotation (XrotM) (tww the same) |
| 0x04 | `m04` | `mRestHeight` | spec_make_boko 024CAD38 / spec_mode_put_init 024CF77C: the stick's start height (M7A0 mHeight); spec_mode_carry 024CF320 bounces it back above it (tww the same) |
| 0x08 | `m08` | `mOffsetZ` | spec_mtx 024CB118: the stick's z offset (transM(0, M7A0 mHeight, it)) (tww the same) |


## daTsubo::Act_c::M7A0 (m7A0[3], the state of the stuck boko sticks; d_a_tsubo.h, copy in d_a_tsubo_main.h)

Round 10. One entry per stick, used by the spec_* functions (WWHD and tww).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mBokoId` | spec_make_boko 024CAD38: fopAcM_create's process id of the stuck boko stick; spec_init 024CAF78 sets fpcM_ERROR_PROCESS_ID_e (tww the same) |
| 0x04 | `m04` | `mbCarried` | spec_carry_spec 024CAE78 sets 1 after tsuboSetCarryNow on the stick; spec_remove 024CBB58 / spec_mode_carry 024CF320 / spec_mode_put_init 024CF77C act only when it is set (tww the same) |
| 0x06 | `m06` | `mStickYaw` | spec_make_boko 024CAD38 copies SpecBokoData mYaw; spec_mtx 024CB118 turns the stick by -it, spec_remove 024CBB58 throws it off at shape_angle.y - it - 0x8000 (tww the same) |
| 0x10 | `m10` | `mHeight` | spec_make_boko 024CAD38 / spec_mode_put_init 024CF77C set SpecBokoData mRestHeight; spec_mode_carry 024CF320 moves it by mHeightSpeed; spec_mtx 024CB118: transM(0, it, mOffsetZ) (tww the same) |
| 0x14 | `m14` | `mHeightSpeed` | spec_mode_carry 024CF320: the per-frame step of mHeight (random kick, -0.8 per frame, halved and reversed at the bottom) (tww the same) |
| 0x18 | `m18` | `mBounceCount` | spec_mode_carry 024CF320: set to 2 at a kick, decremented at each bounce at the bottom; at 0 the stick rests (tww the same) |

Open (no name yet): `m08` (written only: spec_make_boko copies m06 into it, spec_init clears it) and `m0C` (written only: spec_init / spec_mode_put_init clear it); read nowhere in WWHD or tww.

## daObjBarrel2::Act_c (d_a_obj_barrel2.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B4 | `m298` | `mpModel` | create_heap 0231E670 creates the barrel's J3DModel; set_mtx 0231EEF4 / _draw 023210E0 use it |
| 0x3B8 | `m29C` | `mpBrkAnm` | create_heap 0231E670 stores the brk animation; execute_sub 02320AD4 plays it in MODE_AFL; _draw 023210E0 enters it |
| 0x528 | `m40C` | `mMode` | mode_afl_init 0231F344 sets MODE_AFL, mode_exit_v_init 02320918 ...; demo_mode_chk 0231FD50 / execute_sub 02320AD4 test it |
| 0x52C | `m410` | `mType` | the attribute index: attr() returns M_attr[it] (checked < 4); _create 0231F350 sets it; TWW attr() { return &M_attr[m410]; } |
| 0x538 | `m41C` | `mWaterY` | set_water_pos 0231EA58: the mean of three water heights; set_pos_y 0231ED94 sets y = it + mPosYOffset |
| 0x538 | `m450` | `mPosYOffset` | set_pos_y 0231ED94 adds it to mWaterY; mode_afl 02321564 moves it with mPosYOffsetSpeed (bobbing); _create 0231F350 sets the sunk start |
| 0x53C | `m420` | `mWaterNormal` | set_water_pos 0231EA58: the normalized cross product of the water surface; afl_sway 0232115C tilts the barrel by it |
| 0x56C | `m454` | `mPosYOffsetSpeed` | mode_afl 02321564 integrates mPosYOffset with it; co_hitCB 0231E84C sets -60 (pushed under); the exit modes set it |
| 0x578 | `m45C` | `mItemDistance` | _create 0231F350 / set_item_position 023213F8 set barrelItemDistanceMax; item_drop_init 02320294 / item_drop 02321CEC use it |
| 0x57C | `m460` | `mChildPid` | _create 0231F350: process ID of the child actor 0x1D1 it creates, else fpcM_ERROR_PROCESS_ID_e; set_mtx 0231EEF4 / buoy_jump 023201B0 use it |
| 0x580 | `m464` | `mModeTimer` | mode_exit_v_init 02320918 sets 4, mode_explode_init 023203B0 30; the modes count it down |
| 0x586 | `m46A` | `mStagePoint` | _create 0231F350: the current stage's start point (mCurStage.mPoint); _execute 0232101C tests != 1 |
| 0x589 | `m46D` | `mCoHitTimer` | co_hitCB 0231E84C sets 20 when it is 0; execute_sub 02320AD4 counts it down |
| 0x590 | `m474` | `mExitReq` | execute_sub 02320AD4 starts the exit in MODE_AFL when set; TWW exit_req() { m474 = 1; } |
| 0x592 | `m476` | `mItemGive` | item_give 023208BC sets 1; TWW chk_item_give() { return m476; } |

Round 2:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x530 | `m414` | `mDriftVelX` | mode_afl 02321564: X speed = (it + (x - home.x) * -0.002) * 0.9, added to current.pos.x with the collision push (the pull back home) (tww the same) |
| 0x534 | `m418` | `mDriftVelZ` | mode_afl 02321564: Z speed = (it + (z - home.z) * -0.002) * 0.9, added to current.pos.z with the collision push (see mDriftVelX) (tww the same) |
| 0x548 | `m42C` | `mWobbleX` | set_water_pos 0231EA58: off the sea, a random spring value (velocity mWobbleVelX) used as mWaterNormal.x before normalizing; _create 0231F350 clears (tww the same) |
| 0x54C | `m430` | `mWobbleZ` | set_water_pos 0231EA58: off the sea, a random spring value (velocity mWobbleVelZ) used as mWaterNormal.z before normalizing; _create 0231F350 clears (tww the same) |
| 0x550 | `m434` | `mWobbleVelX` | set_water_pos 0231EA58: (it + mWobbleX * -0.2 + rnd) * 0.99, added to mWobbleX; _create 0231F350 clears (tww the same) |
| 0x554 | `m438` | `mWobbleVelZ` | set_water_pos 0231EA58: (it + mWobbleZ * -0.2 + rnd) * 0.99, added to mWobbleZ; _create 0231F350 clears (tww the same) |
| 0x558 | `m43C` | `mBobAngle` | mode_afl 02321564 / mode_explode 02321D54 advance it by attr m58 * (rnd + 1); set_pos_y 0231ED94 adds sin(it) times the sway height to the water y (tww the same) |
| 0x55C | `m440` | `mTiltX` | set_mtx 0231EEF4: the X of the up axis (it, 1, mTiltZ) the model is tilted to; afl_sway 0232115C springs it toward the water normal; _create 0231F350 random for a coming mine, else 0 (tww the same) |
| 0x560 | `m444` | `mTiltZ` | set_mtx 0231EEF4: the Z of the up axis (mTiltX, 1, it); afl_sway 0232115C springs it toward the water normal; _create 0231F350 random for a coming mine, else 0 (tww the same) |
| 0x564 | `m448` | `mTiltVelX` | afl_sway 0232115C: the spring speed of mTiltX (damped by attr m64), added to it each frame; _create 0231F350 clears (tww the same) |
| 0x568 | `m44C` | `mTiltVelZ` | afl_sway 0232115C: the spring speed of mTiltZ (damped by attr m64), added to it each frame; _create 0231F350 clears (tww the same) |
| 0x584 | `m468` | `mMineFlash` | execute_sub 02320AD4: a mine (mType 2) sets 1 once mine_chk_range_flash finds the ship; in MODE_AFL it then plays mpBrkAnm and explodes when it ends, and blocks the mine exit; _create 0231F350 clears (tww the same) |
| 0x588 | `m46C` | `mEventRun` | _execute 0232101C: 1 while an event runs (unless mStagePoint is 1), and then runs execute_sub off screen too; set_item_position 023213F8 sets the item by matrix while set (tww the same) |
| 0x58C | `m470` | `mHitType` | tg_hitCB 0231E800 3 (attack type 0x20), co_hitCB 0231E84C 1 (ship rams it) / 2 (ship touches); execute_sub 02320AD4 takes and clears it: 1 break demo, 2 push under, 3 break and drop (tww the same) |
| 0x591 | `m475` | `mDeleteReq` | execute_sub 02320AD4 deletes the item and the barrel while set; _execute 0232101C runs execute_sub off screen while set (tww the same); never set in WWHD or tww |
| 0x594 | `m478` | `mBaseMtx` | set_mtx 0231EEF4 copies the model's base TR matrix (before the buoy flag offset) into it (tww cMtx_copy(mDoMtx_stack_c::get(), it)); never read in WWHD or tww |

Open: Attr_c `m00`..`m70` (the M_attr tuning table) is not part of this pass.

### daObjBarrel2::Attr_c (M_attr; d_a_obj_barrel2.h)

Round 3: the per-type tuning table M_attr[mType] (Attr_c is 0x74 in WWHD as on the GameCube, so the offsets are the old names). M_attr is constant data: no field is written in WWHD or tww; the reads are in our d_a_obj_barrel2.cpp and match tww's (zeldaret/tww d217e2b, src/d/actor/d_a_obj_barrel2.cpp).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mModelResIdx` | create_heap 0231E670: dComIfG_getObjectRes("Ktaru_00", it) is the model data (tww the same); M_attr is read only, never written in WWHD or tww |
| 0x02 | `m02` | `mBrkResIdx` | create_heap 0231E670: dComIfG_getObjectRes("Ktaru_00", it) is the brk_data (tww the same) |
| 0x04 | `m04` | `mHeapSize` | _create 0231F350: the size passed to fopAcM_entrySolidHeap (tww the same) |
| 0x08 | `m08` | `mCylRadius` | _create 0231F350: mCyl's radius (tww mCyl.SetR) |
| 0x0C | `m0C` | `mCylHeight` | _create 0231F350: mCyl's height (tww mCyl.SetH) |
| 0x10 | `m10` | `mMoveCullRadius` | cull_set_move 0231FBA8: the cull sphere radius while moving (tww the same) |
| 0x14 | `m14` | `mBarrelHeight` | set_pos_y 0231ED94 (float depth mFloatDepthRate * it), set_mtx 0231EEF4 / _create 0231F350 (the top, (it - 5) * scale), mode_afl 02321564 / mode_explode 02321D54 (mPosYOffset capped at 0.6 / 0.2 of it) (tww the same) |
| 0x18 | `m18` | `mCenterHeight` | _create 0231F350: attention / eye position and cull sphere centre at it * scale above pos, the coming start depth -(it * scale) - 50; mode_exit_v 0232183C / mode_exit_h 023219D8 the exit depth from it (tww the same) |
| 0x34 | `m34` | `mItemGiveDist` | execute_sub 02320AD4: item_give once mItemDistance < it squared (tww the same) |
| 0x38 | `m38` | `mShipHitSpeed` | co_hitCB 0231E84C: a ship hit counts only above this speedF (tww the same) |
| 0x3C | `m3C` | `mShipHitDot` | co_hitCB 0231E84C, mine_chk_range_damage 02320418: when dot(barrel - ship, ship speed) < it squared the ship gets flag 4 (tww the same) |
| 0x40 | `m40` | `mFlashRange` | mine_chk_range_flash 023209E4: ship distance squared below it squared (tww the same) |
| 0x44 | `m44` | `mExplodeRange` | mine_chk_range_explode 0231FD84: ship distance squared below it squared (tww the same) |
| 0x48 | `m48` | `mDamageRange` | mine_chk_range_damage 02320418: ship distance squared below it squared (tww the same) |
| 0x4C | `m4C` | `mDamageHeight` | mine_chk_range_damage 02320418: the ship counts only below pos.y + it (tww the same) |
| 0x50 | `m50` | `mFloatDepthRate` | set_pos_y 0231ED94: the floating depth is it * mBarrelHeight; effect_position (in eff_break / eff_explode) puts effects at that depth (tww the same) |
| 0x54 | `m54` | `mBobAmplitude` | set_pos_y 0231ED94: the bob it * sin(mBobAngle) (tww the same) |
| 0x58 | `m58` | `mBobAngleStep` | mode_afl 02321564 / mode_explode 02321D54: mBobAngle += it * (cM_rnd() + 1) (tww the same) |
| 0x5C | `m5C` | `mTiltTargetMax` | afl_sway 0232115C: the water-normal tilt target is clamped to this length (tww the same) |
| 0x60 | `m60` | `mTiltSpring` | afl_sway 0232115C: the tilt acceleration -(tilt - target) * it (tww the same) |
| 0x64 | `m64` | `mTiltDamping` | afl_sway 0232115C: the tilt velocity damping -velocity * it (tww the same) |
| 0x68 | `m68` | `mWaveTiltRate` | afl_sway 0232115C: the tilt target is mWaterNormal.x / .z * it (tww the same) |
| 0x6C | `m6C` | `mPillarScaleXZ` | eff_explode 0231FE60: fopKyM_createWpillar's scaleXZ (tww the same) |
| 0x70 | `m70` | `mPillarScaleY` | eff_explode 0231FE60: fopKyM_createWpillar's scaleY (tww the same) |

Open (no name yet): `m1C` (read nowhere in WWHD or tww); `m20` and `m24` (only ever read as their product, the model scale: set_mtx 0231EEF4, set_pos_y 0231ED94, cull_set_draw 02320EDC, effect_position; no use tells them apart); `m28` and `m2C` (only ever read as their product, the race item's scale: _create 0231F350, item_drop_init 02320294); `m30` (two roles: the exit depth below the centre in mode_exit_v 0232183C / mode_exit_h 023219D8, and the item's height above pos in set_item_position 023213F8); the padding `_5a`.



## daPirate_Flag_packet_c (d_a_pirate_flag.h): index buffer fields

Not offset names: the two words at 0x1698 / 0x169C had their names swapped (same pattern as d_tree / d_magma, see
docs/field-names/draw-systems.md). The packet embeds an agl::IndexBufferLayout at 0x1698 (constructed by 027B5430 at
023D2B9C, setStorage 027B54E0 at 023D337C with format 4 and count 0x28). GX2 argument order:
`GX2DrawIndexedEx(mode r3, count r4, indexType r5, indices r6, baseVertex r7, numInstances r8)`.
pirate_render_material's locals follow (`format`, `mode`; were `topology`, `type`).

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x1698 | `mPrimitiveType` (misnamed) | `mIndexFormat` | GX2DrawIndexedEx indexType (r5) at 023D39D4 / 023D3A64 (`lwz r5,0x1698(r28)`); agl::IndexBufferLayout::mFormat |
| 0x169C | `mIndexType` (misnamed) | `mPrimitiveMode` | GX2DrawIndexedEx mode (r3) at 023D39D4 / 023D3A64 (`lwz r3,0x169C(r28)`); 023D2BC0 stores 6 (triangle strip); agl::IndexBufferLayout::mPrimitiveMode |

0x16A0 `mpIndices` (indices r6) and 0x16A8 `mIndexStride` (bytes per index, scales the strip offset) were right and
stay.

## daBomb2::Act_c (d_a_bomb2.h; the BombStatic::daBomb2::Act_c view in d_a_bomb_static.h derives from it)

Old names are the GameCube offsets (zeldaret/tww `field_0xXXX`). The view in d_a_bomb_static.h declares no fields of
its own; its accessors chk_eat / set_eat in d_a_bomb_static.cpp follow. The tilt spring (vib_*/tensor_*) works on two
components; the field pairs are named by the axis they take from speed / the ground normal (vib_proc's locals dx/dz
are the other way round and keep their names).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x714 | `field_0x51C` | `mLavaY` | crr_init 020C8B58 sets -inf; bgCrrPos_lava 020C8C78: cBgS_GroundCross of the lava (Yogan) ground check mGndChk; chk_lava_in 020CA790 tests it |
| 0x718 | `field_0x520` | `mWaterY` | crr_init 020C8B58 sets -inf; bgCrrPos_water 020C8CE4 stores mAcch's water height or the daSea wave height (-inf when out of water); eff_water_splash 020CA9F8 puts the splash there |
| 0x71E | `field_0x526` | `mWaterIsSea` | bgCrrPos_water 020C8CE4 sets 1 when mWaterY comes from the sea wave (daSea_calcWave), 0 when from mAcch's water polygon or no water; se_fall_water 020CA91C then passes no water polygon |
| 0x720 | `field_0x528` | `mSinkWaterY` | crr_init 020C8B58 sets -inf; mode_sink_init 020CAFB4: fopAcM_getWaterY at the bomb; mode_sink 020CB218 moves the bomb by the water height change and keeps the new height |
| 0x890 | `field_0x698` | `mSinkTimer` | mode_sink_init 020CAFB4 sets 4; mode_sink 020CB218 counts it down and sets mDeleteReq when it runs out |
| 0x8B8 | `field_0x6C0` | `mFusePos` | eff_fuse_start 020CA4F0: current.pos + 60 in y, the position of the sparks and smoke emitters (dPa_control_c::set); eff_fuse_move 020C8FB4: l_currentMtx times the fuse offset (0, 60, 5) |
| 0x8C4 | `field_0x6CC` | `mFuseOldPos` | eff_fuse_move 020C8FB4 copies last frame's mFusePos here; eff_fuse_start 020CA4F0 passes it as the first FuseSmokeCB_c::setOldPosP argument |
| 0x8D0 | `field_0x6D8` | `mFuseOldOldPos` | eff_fuse_move 020C8FB4 copies last frame's mFuseOldPos here; second FuseSmokeCB_c::setOldPosP argument in eff_fuse_start 020CA4F0 |
| 0x938 | `field_0x73C` | `mExplodeCcTimer` | mode_explode_init 020CA3A4 sets 4; cc_set 020C9D04 counts it down while exploding and sets the attack sphere only while it stays above 0; mode_explode 020CAE14 waits for 0 (and mEnv.is_end) before mDeleteReq |
| 0x93C | `field_0x740` | `mDeleteReq` | create_init 020C9218 clears it; set by mode_explode 020CAE14 and mode_sink 020CB218; _execute 020C9E78 deletes the actor (fopAcM_delete) when set; mode_wait 020CB044 bounces only while clear |
| 0x93D | `field_0x741` | `mThrown` | mode_carry 020CAE6C sets 1 when released with speedF > 0; bound 020CA7C0 clears it once the bounce is below 19.5; chk_exp_cc_nut 020CAA6C / chk_exp_bg_nut 020CAB88 light the fuse on any contact while it is set |
| 0x93E | `field_0x742` | `mExplodeEffDone` | eff_explode 020CA2A4 returns if set, else sets it and starts the explosion effects/sound once; is_draw 020C9F18 hides the bomb when set |
| 0x93F | `field_0x743` | `mWaterExplodeEffDone` | eff_explode_water 020CA0F8 returns if set, else creates the water pillar and sets it; is_draw 020C9F18 hides the bomb when set |
| 0x940 | `field_0x744` | `mFuseLit` | eff_fuse_start 020CA4F0 sets it (returns early if set), eff_fuse_init 020C8F2C / eff_fuse_end 020C95F4 clear it; eff_fuse_move 020C8FB4 plays the ignition sound while set; carry_fuse_start 020CAD9C returns if set |
| 0x941 | `field_0x745` | `mEaten` | our decomp's accessors daBomb2::Act_c::chk_eat 020CBA70 (returns it) and set_eat 020CBA78 (sets 1) in d_a_bomb_static.cpp; mode_wait 020CB044 skips posMoveF while set |
| 0x950 | `field_0x754` | `mTensorMtx` | tensor_init 020C8EF4 sets the identity; set_vib_tensor 020C9BB8 builds it (PSMTXQuat); set_mtx 020C9074 concatenates it into the model matrix |
| 0x980 | `field_0x784` | `mVibTargetZ` | vib_proc 020C9B18: the spring pulls mVibZ towards it; tensor_wait_ground 020CB2BC sets the ground normal's z * 1.5, the other tensor_* modes 0 |
| 0x984 | `field_0x788` | `mVibTargetX` | vib_proc 020C9B18: the spring pulls mVibX towards it; tensor_wait_ground 020CB2BC sets the ground normal's x * 1.5, the other tensor_* modes 0 |
| 0x988 | `field_0x78C` | `mVibZ` | vib_proc 020C9B18 adds mVibVelZ; set_vib_tensor 020C9BB8 uses it as the z of the tilt direction (quat_rotBaseY2) |
| 0x98C | `field_0x790` | `mVibX` | vib_proc 020C9B18 adds mVibVelX; set_vib_tensor 020C9BB8 uses it as the x of the tilt direction (quat_rotBaseY2) |
| 0x990 | `field_0x794` | `mVibVelZ` | vib_proc 020C9B18: velocity of mVibZ (old + 2 * (impulse - 0.03 * (mVibZ - target)), times 1 - mVibDamping) |
| 0x994 | `field_0x798` | `mVibVelX` | vib_proc 020C9B18: velocity of mVibX, same update as mVibVelZ |
| 0x998 | `field_0x79C` | `mVibImpulseZ` | tensor_wait_drop 020CB34C sets speed.z * -0.005 while mVibImpulseTimer > 0; vib_proc 020C9B18 adds it to the velocity once and clears it |
| 0x99C | `field_0x7A0` | `mVibImpulseX` | tensor_wait_drop 020CB34C sets speed.x * -0.005 while mVibImpulseTimer > 0; vib_proc 020C9B18 adds it once and clears it |
| 0x9A0 | `field_0x7A4` | `mVibDamping` | vib_proc 020C9B18 scales the velocity by 1 - it; tensor_wait_drop 020CB34C sets 0.1, tensor_wait_ground 020CB2BC / tensor_carry 020CB3AC 0.4 |
| 0x9A4 | `field_0x7A8` | `mVibImpulseTimer` | set_wind_vec 020C9674 (wind push) and mode_carry 020CAE6C (throw) set 2; vib_proc 020C9B18 counts it down; tensor_wait_drop 020CB34C sets the impulse only while it is above 0 |

Open: 0x71C `field_0x524` (crr_init 020C8B58 clears it; no other use in this source).

### daBomb2::Env_c and daBomb2::FuseSmokeCB_c (d_a_bomb2.h)

Old names are the zeldaret/tww `field_0xXX` (GameCube offsets inside the helper class).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x50 | `field_0x4C` | `mStep` | Env_c::proc 020C8308: 0 raises mIntensity to 1, 1 lowers it to 0, each step advances when done; Env_c::is_end 020C82F4 returns it > 1; Env_c::set 020C81A4 clears it |
| 0x54 | `field_0x50` | `mIntensity` | Env_c::proc 020C8308: point wind strength, point light power (1500 * it) and the ambient/bg colour add factor; eased 0 -> 1 -> 0 by mStep; Env_c::set 020C81A4 clears it |

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x4 | `field_0x04` | `mPrevLife` | FuseSmokeCB_c::executeAfter 020C8564: last frame's particle life time (stored at the end), interpolated towards the new one over the in-between particles; setOldPosP 020C8518 sets 20 |
| 0xC | `field_0x0C` | `mpOldPos` | FuseSmokeCB_c::setOldPosP 020C8518 first argument (mFuseOldPos); executeAfter 020C8564 interpolates the smoke from it to mpPos |
| 0x10 | `field_0x10` | `mpOldOldPos` | FuseSmokeCB_c::setOldPosP 020C8518 second argument (mFuseOldOldPos); executeAfter 020C8564 uses mpOldPos - it as the start tangent of the curve |

Open: Bomb2WindInfluence `field_0x20` (a copy of WIND_INFLUENCE, d_kankyo.h; Env_c::set 020C81A4 only clears it): left to
the owner of WIND_INFLUENCE.

## daObjSwlight::Act_c (d_a_obj_swlight.h)

Old names are the zeldaret/tww GameCube offset names (`mXXX`).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B4 | `m298` | `mpModel` | create_heap 02395298 creates the J3DModel; _create 023958E8 takes its base matrix as the actor matrix; set_mtx 02395564 / _draw 02395ED4 use it |
| 0x3B8 | `m29C` | `mBtkAnm` | create_heap 02395298 inits the btk animation; _execute 02395CA4 plays it; _draw 02395ED4 enters it |
| 0x42C | `m2B0` | `mBckAnm` | create_heap 02395298 inits the bck animation; _execute 02395CA4 plays it; _draw 02395ED4 enters it |
| 0x4B8 | `m2C0` | `mpBgW` | create_heap 02395298 creates the dBgW; _create 023958E8 registers it (dComIfG_Bgsp()->Regist); _delete 02395C40 releases it |
| 0x4BC | `m2C4` | `mTri` | init_cc 02395758 sets the 8 dCcD_Tri from M_tri_src; set_cc_pos 02395648 places them; set_cc 0239620C registers them; chk_light 0239625C tests ChkTgHit |
| 0xF3C | `mD44` | `mStts` | init_cc 02395758: Init(0xFF, 0xFF, this) and m2C4[i].SetStts(&mD44[i]) |
| 0x111C | `mF24` | `mType` | _create 023958E8: prm PRM_TYPE; 0 starts the norm (sun/moon) modes, else the active modes that also need switch 2 |
| 0x1120 | `mF28` | `mMode` | mode_norm_sun_init 02395830 / mode_norm_moon_init 0239583C / mode_active_sun_init 023958C8 / mode_active_moon_init 023958D4 set MODE_*; _execute 02395CA4 calls mode_proc[mF28] |
| 0x1124 | `mF2C` | `mLightTimer` | mode_norm_moon_init 0239583C / mode_active_moon_init 023958D4 set 20; mode_norm_moon 02396370 / mode_active_moon 0239657C count it down while chk_light, reset 20 otherwise; 0 turns the switch on |
| 0x1128 | `mF30` | `mPower` | power_up 023962F8 / power_down 02396334 chase it to 1 / 0 by 1/30; _create 023958E8 sets 1 or 0 by the switch; _draw 02395ED4 alpha = 255 * it; mode_active_sun 0239666C sound while > 0.5 |
| 0x1130 | `mF38` | `mBgMtx` | _create 023958E8 copies the position/angle/scale matrix here; create_heap 02395298 passes it to the dBgW set (swlightSetBackground) |

## daObjPirateship::Act_c (d_a_obj_pirateship.h; views: daObjPirateship::Act_c in d_a_kaji.h, PirateShipView in d_a_npc_p1.h)

Old names are the zeldaret/tww GameCube offset names (`mXXX`). The view in d_a_kaji.h follows for `mAppeared` (was
`m2CC`, read by daKaji_c::_draw, inlined into daKajiDraw 021853D8); PirateShipView already had `mKajiId`.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3E4 | `m2CC` | `mAppeared` | executeShip (inlined into _execute 02385B70 / _create 023852E0): while 0 the ship waits for the demo and event 0x0310, then creates the wake, registers the dBgW and sets 1; Draw 02384FB4 draws only when set; sound_proc 02384A78 runs only when 1 |
| 0x3E5 | `m2CD` | `mPiratesCreated` | pirateCreate 023844DC sets 1; executeShip (_execute 02385B70) calls pirateCreate(create_idx_tbl_1stIsland_demo) once event 0x0001 is set and it is still 0 |
| 0x474 | `m2E4` | `mpSail` | executeShip (_execute 02385B70): searchPart(m_sail_id, it), the actor of the Psail child |
| 0x47C | `m2EC` | `mpFlag` | executeShip (_execute 02385B70): searchPart(mFlagId, it), the pirate flag child |
| 0x480 | `m2F0` | `mFlagId` | partsCreate 0238418C: fopAcM_createChild(fpcNm_PIRATE_FLAG_e, ...) |
| 0x488 | `m2F8` | `mTousekikiId` | partsCreate 0238418C: fopAcM_createChild(fpcNm_Obj_Tousekiki_e, ...) (the catapult) |
| 0x48C | `m2FC` | `mpKaji` | executeShip (_execute 02385B70): searchPart(mKajiId, it), the steering wheel (Kaji) child |
| 0x490 | `m300` | `mKajiId` | partsCreate 0238418C: fopAcM_createChild(fpcNm_Kaji_e, ...); TWW accessor getKajiID() { return m300; } (zeldaret/tww d_a_obj_pirateship.h); view PirateShipView::mKajiId (d_a_npc_p1.h) |
| 0x494 | `m304` | `mpDoor` | partsCreate 0238418C clears it; set_mtx 02384690 searches mDoorId and then places the door (pirateshipDoorSetPos) 400 up / 475 ahead; TWW type daKnob00_c* |
| 0x498 | `m308` | `mDoorId` | partsCreate 0238418C: fopAcM_createChild(fpcNm_KNOB00_e, ...); set_mtx 02384690 searches it |
| 0x49C | `m30C` | `mCruiseSePos` | sound_proc 02384A78: (0, 800, 2000) in ship space, position of JA_SE_OBJ_PIRATE_CRUISE; _delete 02385AEC mDoAud_seDeleteObject |
| 0x4A8 | `m318` | `mSailSePos` | sound_proc 02384A78: (0, 1500, 300) in ship space, position of JA_SE_OBJ_PIRATE_SAIL; _delete 02385AEC mDoAud_seDeleteObject |
| 0x4BE | `m32E` | `mSwayAngle` | executeShip (_execute 02385B70): adds mSwaySpeed each frame, pos.y = home.y + amplitude * sin(it); after the angle parameter of daObjPirateship::getShipOffsetY 025269A8 (d_com_static.cpp) |
| 0x4C0 | `m330` | `mSwaySpeed` | executeShip (_execute 02385B70): cLib_addCalcAngleS2 towards the HIO speed by the sway half; after the speed parameter of daObjPirateship::getShipOffsetY 025269A8 |
| 0x4C8 | `m338` | `mStts` | _create 023852E0 constructs the dCcD_Stts; pirateship_Act_c_dt 02386148 destroys it |
| 0x634 | `m4A4` | `mPathPntNo` | executeShip (_execute 02385B70): dLib_pathMove's point number (s8*) argument; _create 023852E0 clears it |
| 0x638 | `m4A8` | `mpPath` | _create 023852E0: dPath_GetRoomPath; executeShip (_execute 02385B70) moves along it with dLib_pathMove |
| 0x63C | `m4AC` | `mWakeCb` | CreateWave 02384570: the dPa_trackEcallBack of the PIRATESHIP_TAIL wake emitter; SetWave 02384C2C sets its water height/speed; DeleteWave 02384A48 removes it |
| 0x68C | `m4FC` | `mpSplashEmitter` | CreateWave 02384570: the PIRATESHIP_SPLASH emitter; SetWave 02384C2C sets its colour/translation/rotation; DeleteWave 02384A48 stops it |
| 0x690 | `m500` | `mSplashColor` | CreateWave 02384570 / SetWave 02384C2C: dKy_get_seacolor, the splash emitter's prm colour |
| 0x694 | `m504` | `mAcch` | _create 023852E0 constructs the dBgS_ObjAcch; SetWave 02384C2C takes its water height (m_wtr) when in water |
| 0x858 | `m6C8` | `mWakePos` | CreateWave 02384570: current.pos, the wake emitter's position; SetWave 02384C2C: ship matrix times HIO offset, its y is the wake water height |
| 0x3E6 | `m2CE` | `mSailSeOn` | _create 023852E0 sets (mParameters & 15) != 0; sound_proc 02384A78 starts JA_SE_OBJ_PIRATE_SAIL only when set |

Open: 0x3E2 `m2CA` (executeShip sets it from the HIO every frame outside demos; nothing in this source reads it).

### daObjPirateShipHIO_c (l_HIO; d_a_obj_pirateship.h)

Round 3: the pirate ship's HIO values. In WWHD the vtable sits at the end, so each field is 4 below its GameCube name. Each is set only by pirateship_sinit 02385FA0 (the static constructor; default in parentheses) and never written elsewhere in WWHD or tww; the reads are in our d_a_obj_pirateship.cpp (executeShip is the body of daObjPirateship::Act_c::_execute 02385B70) and match zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00001 | `m05` | `mNoPathMove` | executeShip (in daObjPirateship::Act_c::_execute 02385B70): the path move and the sway run only while it is 0 (tww _execute the same) (0) |
| 0x00004 | `m08` | `mPathSpeed` | path_move_call_back 02384078: the speed toward the next path point is (it + 5) * \|cos\| of the turn left (tww the same) (2) |
| 0x00008 | `m0C` | `mSwayHeight` | executeShip (in daObjPirateship::Act_c::_execute 02385B70): pos.y = home.pos.y + (it + 10) * sin(mSwayAngle) (tww _execute the same) (100) |
| 0x0000C | `m10` | `mSwaySpeedRising` | executeShip (in daObjPirateship::Act_c::_execute 02385B70): mSwaySpeed's target while mSwayAngle is within +-0x4000 (the ship rising) (tww _execute the same) (250) |
| 0x0000E | `m12` | `mSwaySpeedFalling` | executeShip (in daObjPirateship::Act_c::_execute 02385B70): mSwaySpeed's target while mSwayAngle is outside +-0x4000 (the ship sinking) (tww _execute the same) (500) |
| 0x00014 | `m18` | `mWakeOffset` | daObjPirateship::Act_c::SetWave 02384C2C: the wake emitter's position in ship space (mWakePos is the ship matrix times it) (tww the same) (0, 0, 0) |
| 0x00020 | `m24` | `mWakeScale` | daObjPirateship::Act_c::SetWave 02384C2C: the wake emitter's particle and dynamics scale (tww the same) (4) |
| 0x00024 | `m28` | `mWakeIndirectOffset` | daObjPirateship::Act_c::SetWave 02384C2C: mWakeCb.mIndirectOffset (tww setIndirectTexData's first argument) (tww the same) (-0.02) |
| 0x00028 | `m2C` | `mWakeIndirectScale` | daObjPirateship::Act_c::SetWave 02384C2C: mWakeCb.mIndirectScale (tww setIndirectTexData's second argument) (tww the same) (5) |
| 0x0002C | `m30` | `mWakeSpeed` | daObjPirateship::Act_c::SetWave 02384C2C: mWakeCb.mSpeed (tww setSpeed) (tww the same) (300) |
| 0x00030 | `m34` | `mWakeLimitSpeed` | daObjPirateship::Act_c::SetWave 02384C2C: mWakeCb.mLimitSpeed (tww setLimitSpeed) (tww the same) (3) |
| 0x00034 | `m38` | `mWakeDirectionalSpeed` | daObjPirateship::Act_c::SetWave 02384C2C: the wake emitter's mInitialVelDir (tww setDirectionalSpeed) (tww the same) (17) |

Open (no name yet): `m06` (executeShip copies it into Act_c `m2CA` every frame, and m2CA is read nowhere in WWHD or tww, so no role), `m14` (set by the constructor only, read nowhere); padding `reserved11`.

## daObjTide::Act_c (d_a_obj_tide.h)

Old names are the zeldaret/tww GameCube offset names (`mXXX`). The sound flags follow the existing `M_id_gopo` / `M_id_out` / `M_id_up` sound ids they start and stop.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x404 | `m2EC` | `mMode` | mode_norm_init 023A0DC0 / mode_gmtw_pre_init 023A0D58 / mode_gmtw_demo_init 023A1644 / mode_gmtw_init 023A16AC set MODE_*; Execute 023A0F08 calls tideModeProc[it] |
| 0x408 | `m2F0` | `mMoveSpeed` | move_to_aim 023A1258: the step of the height chase (cLib_addCalc), grows by attr m24 per frame between attr m2C and m28, 0 once at the aim; Create 023A0DCC clears it |
| 0x40C | `m2F4` | `mTimer` | mode_norm 023A14A8: counts down from attr m42 and restarts the second brk when it reaches 0; mode_gmtw_demo 023A1D68: set 90 when the rise starts, counted down to step 2 (water particle and dBgW at 59) |
| 0x40E | `m2F6` | `mDemoStep` | mode_gmtw_demo 023A1D68: 0 waits for gmtw_demo_up_start, 1 counts mTimer, 2 rises; mode_gmtw_init 023A16AC sets 2; se_set_gmtw 023A19AC plays the out/up sounds in step 2 (step 1 once mBgRegistered) |
| 0x40F | `m2F7` | `mBgRegistered` | Mthd_Create 023A0484 sets 1 after the move dBgW is created; mode_gmtw_pre 023A1798 releases the dBgW and clears it; mode_gmtw_demo 023A1D68 registers it again and sets 1 |
| 0x410 | `m2F8` | `mSeGopoOn` | se_set_gmtw 023A19AC: on while the water is not finished; starts/stops the gurgle sound (se_start_gopo / se_stop_gopo, M_id_gopo) on a change; se_init_gmtw 023A0418 clears it |
| 0x411 | `m2F9` | `mSeOutOn` | se_set_gmtw 023A19AC: on in demo step 2 (or 1 with mBgRegistered); starts/stops se_start_out / se_stop_out (M_id_out) on a change; se_init_gmtw 023A0418 clears it |
| 0x412 | `m2FA` | `mSeUpOn` | se_set_gmtw 023A19AC: on with mSeOutOn; se_start_up on the rising edge, se_pos_up while on, se_stop_up on the falling edge (M_id_up); se_init_gmtw 023A0418 clears it |
| 0x413 | `m2FB` | `mSePachaOn` | se_set_gmtw 023A19AC: set when the water is finished (boss beaten or at home.y + 2923.5); then starts JA_SE_ATM_MJ_WATER_PACHA at eyePos; se_init_gmtw 023A0418 clears it |
| 0x420 | `m308` | `mSwLatched` | mode_gmtw 023A1B78: set once the PRM_SWSAVE switch is seen on; from then the aim is home.y + 2923.5 and the height snaps there while the switch is off; mode_gmtw_init 023A16AC clears it |
| 0x421 | `m309` | `mUseRes` | Mthd_Create 023A0484 sets 1 (create fails with cPhs_ERROR_e while 0); Mthd_Delete 023A0658 deletes the move dBgW and the resource only when set |
| 0x422 | `m30A` | `mDeleteReq` | mode_gmtw 023A1B78 sets it when mRescueTimer runs out after RESCUE_COMPLETE; Execute 023A0F08 deletes the actor when set; Create 023A0DCC clears it |
| 0x423 | `m30B` | `mBossDead` | se_init_gmtw 023A0418: dungeon membit STAGE_BOSS_ENEMY; se_set_gmtw 023A19AC treats the water as finished when set |
| 0x424 | `m30C` | `mRescueTimer` | mode_gmtw_init 023A16AC sets 300; mode_gmtw 023A1B78 counts it down while the RESCUE_COMPLETE tmp bit is set (300 otherwise), then sets mDeleteReq |

## daObjMknjD::Act_c (d_a_obj_mknjd.h)

Old names are the zeldaret/tww GameCube offset names (`m0XXX`).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x548 | `m0430` | `mDemoEventBit` | Create 02372DA8 sets 0x2910 (TYPE_WIND) or 0x2920; Execute 02373EA8 dComIfGs_onEventBit(it) when the statue's demo starts |
| 0x54A | `m0432` | `mBgmTimer` | Execute 02373EA8: set 0x2B (TYPE_WIND) or 0x0E when the demo starts, counted down; at 1 starts JA_BGM_TAKT_MAKORE / JA_BGM_TAKT_MEDRI |
| 0x54C | `m0434` | `mFriendDrawTimer` | Execute 02373EA8: set 5 when the demo starts, counted down; at 1 calls manage_friend_draw(1) |
| 0x555 | `m043D` | `mBreakMdlVisible` | Create 02372DA8 clears it; daObjMknjD_break 023734B4 sets it at mBreakTimer 60; Draw 023747BC draws mBreakMdl only when set |
| 0x556 | `m043E` | `mType` | Mthd_Create 0237274C: prm_get_Type() (TWW accessor); CreateHeap 023728DC / Create 02372DA8 / privateCut 02373A30 / Execute 02373EA8 test TYPE_WIND |
| 0x557 | `m043F` | `mState` | Create 02372DA8 sets STATE_WAIT_LESSON or STATE_WAIT; Execute 02373EA8 switches on it (STATE_*) |
| 0x618 | `m0500` | `mNormalTalk` | Execute 02373EA8 sets 1 for a plain talk, 0 when the lesson starts (talk XY); talk 02373360 / TWW getMsg(): 0 gives message 0x5B3 with the melody, else 0x1901; privateCut 02373A30 clears it |
| 0x61C | `m0504` | `mMsgCanceled` | talk 02373360 sets it when the message cancel button is pressed; privateCut 02373A30 (ACT_TACT) ends the cut only while it is clear, and clears it; Create 02372DA8 clears it |

## daObjMmrr::Act_c (d_a_obj_mmrr.h)

Old names are the zeldaret/tww `field_0xXXX`. The attribute block MmrrAttributes (L_attr) keeps its offset names for now.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x4A4 | `field_0x2C8` | `mTri` | init_cc 023751B0 sets the 5 dCcD_Tri from M_tri_src; set_cc_rec_pos 02374E98 places them; _execute 02375644 registers them; chk_light 02375504 tests their hits |
| 0xB34 | `field_0x958` | `mTriStts` | init_cc 023751B0: Init(0xff, 0xff, this) and mTri[i].SetStts(&it[i]) |
| 0xC60 | `field_0xA84` | `mBeamCps` | init_cc 023751B0 sets the dCcD_Cps from M_cps_src; set_cc_trans_pos 02374FC0 sets it along the reflected beam; _execute 02375644 registers it while mReflecting |
| 0xD98 | `field_0xBBC` | `mBeamStts` | init_cc 023751B0: Init(0xff, 0xff, this) and mBeamCps.SetStts(&it) |
| 0xDD4 | `field_0xBF8` | `mPosChanged` | TWW inline setup(pos) sets it with current.pos; _execute 02375644 re-places mTri (set_cc_rec_pos) and the beam while set, then clears it; _create 02375300 clears it |
| 0xDD5 | `field_0xBF9` | `mReflecting` | _execute 02375644: mLightRate > 0.999; on the rising edge JA_SE_OBJ_MIRROR_REFLECT and eff_start, on the falling edge eff_stop; set_cull 02375264 / _draw 02375860 include the beam while set |
| 0xDD8 | `field_0xBFC` | `mLightRate` | _execute 02375644: cLib_chaseF to 1 while chk_light, else to 0, by 0.2; _create 02375300 clears it |
| 0xDDC | `field_0xC00` | `mBeamLenRate` | set_cc_trans_pos 02374FC0: beam length up to the light line check hit over attr 0xD0; set_mtx 02374D2C scales the beam model's z by it |
| 0xDE0 | `field_0xC04` | `mEff` | _create 02375300 constructs the Eff_c (dPa_followEcallBack); eff_start 023755D4 / eff_stop 0237563C / eff_remove 023754BC use it |

## daObjHlift::Act_c (d_a_obj_hlift.h)

Old names are the zeldaret/tww `field_0xXXX`. Open: 0x40F `field_0x2F7` and 0x412 `field_0x2FA` (no use); the size table row (`field_0x00`..`field_0x02`) is left for a later pass.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3F0 | `field_0x2D8` | `mDistIdx` | Mthd_Create 02356090: prm PRM_DIST; distance() (inlined into mode_l_u 02356BD0 / mode_u_l 02356EE0) reads M_up_dist[it], the lift's travel height |
| 0x3F4 | `field_0x2DC` | `mSizeIdx` | Mthd_Create 02356090: prm PRM_SIZE; CreateHeap 023561E4 picks the model resource from M_data_size[it] |
| 0x3F8 | `field_0x2E0` | `mMode` | mode_upper_init 023562EC / mode_lower_init 02356344 / mode_l_u_init 02356B2C / mode_u_l_init 02356E40 / mode_demoreq_init 02356A14 set Mode_*; Execute 02356828 calls mode_proc[it] |
| 0x3FC | `field_0x2E4` | `mNextMode` | mode_demoreq_init 02356A14 stores the mode to start after the demo request; mode_demoreq 02357040 starts mode_u_l_init when it is Mode_U_L, else mode_l_u_init; Create 023564F4 sets Mode_UNK |
| 0x400 | `field_0x2E8` | `mVibTimer` | vib_set 02356A00 sets 10; vib_proc 02356728 counts it down while the lift shakes |
| 0x402 | `field_0x2EA` | `mVibAngle` | vib_set 02356A00 sets 0x4000; vib_proc 02356728 adds 0x4000 per frame, the phase of the shake sine |
| 0x404 | `field_0x2EC` | `mVibOffsetY` | vib_proc 02356728: sine * 2.5 * factor, 0 at the end; set_mtx 02356358 adds it to the model's height; Create 023564F4 clears it |
| 0x408 | `field_0x2F0` | `mMoveSpeed` | mode_l_u 02356BD0 / mode_u_l 02356EE0: speed of the cLib_chaseF of current.pos.y (eased to 10, then 2 near the end); mode_l_u_init 02356B2C / mode_u_l_init 02356E40 clear it |
| 0x40C | `field_0x2F4` | `mTimeLag` | lagSet (inlined into mode_l_u_init 02356B2C / mode_u_l_init 02356E40): L_time_lag[L_time_lag_num]; mode_l_u 02356BD0 / mode_u_l 02356EE0 wait while it counts down |
| 0x40E | `field_0x2F6` | `mDemoRunning` | mode_demoreq 02357040 sets 1 when the demo event is accepted; chk_demo_end 02356664 clears it at the event's end and returns !it; Create 023564F4 / mode_demoreq_init 02356A14 clear it |

## daObjVyasi::Act_c (d_a_obj_vyasi.h)

Old names are the GameCube offset names (`m0XXX`). "TWW name": the same field has this name in zeldaret/tww d217e2b (include/d/actor/d_a_obj_vyasi.h).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3AC | `m0294` | `mAnimWave` | TWW name (zeldaret/tww d_a_obj_vyasi.h 0x0294 mAnimWave); quaternion_main 023B8388 advances the per-joint wave phase |
| 0x3C8 | `m02B0` | `mAnimDir` | TWW name (0x02B0 mAnimDir); quaternion_main 023B8388 eases it (cLib_addCalcAngleS2) and uses it as the wind angle |
| 0x4C4 | `m03AC` | `mJointAngle` | TWW name (0x03AC mJointAngle); calc_dif_angle 023B86B8 computes it; JointNodeCallBack 023B775C adds it to the joint rotation |
| 0x518 | `m0400` | `mJointPos` | TWW name (0x0400 mJointPos); JointNodeCallBack 023B775C stores the joint position; set_collision 023B7FDC uses joint i / i + 1 as the capsule ends |
| 0x5C0 | `m04A8` | `mLeafScaleX` | after TWW mLeafScale (cXyz at 0x04A8; three floats here): x; JointNodeCallBack 023B775C scaleM(x, y, z); leaf_scale_main 023B8DC8 eases it; _create 023B7B3C sets 1 |
| 0x5C4 | `m04AC` | `mLeafScaleY` | after TWW mLeafScale: y (see mLeafScaleX) |
| 0x5C8 | `m04B0` | `mLeafScaleZ` | after TWW mLeafScale: z (see mLeafScaleX) |
| 0x5D4 | `m04BC` | `mMtx` | TWW name (0x04BC mMtx); set_mtx 023B7668 copies the actor matrix here |
| 0x61C | `m0504` | `mSagRatio` | TWW name (0x0504 mSagRatio); process_sagWind_init 023B9160 sets it; calc_dif_angle 023B86B8 / process_toNormal_main 023B940C scale the wave by it |
| 0x63C | `m0524` | `mWaveSpeed` | TWW name (0x0524 mWaveSpeed[14]; 15 entries here); process_sagWind_init 023B9160 sets it; calc_dif_angle 023B86B8 adds it to the wave phase |
| 0x1ADC | `m19C4` | `mAnmPlaying` | TWW name (0x19C4 mAnmPlaying); _execute 023B8EB0 = PlayStopJointAnimation(); SetStopJointAnimation 023B8FDC sets 1 |
| 0x1AE4 | `m19CC` | `mToNormalAmp` | process_toNormal_main 023B940C: set -1792 * mSagRatio, then * 0.85 per frame until about 0; calc_dif_angle 023B86B8: amplitude of the joint z swing |
| 0x1AE8 | `m19D0` | `mToNormalPhase` | process_toNormal_main 023B940C: cleared, then + 0x3000 per frame; calc_dif_angle 023B86B8: the phase of the joint z swing (sin) |
| 0x1AEC | `m19D4` | `mWindScale` | TWW name (0x19D4 mWindScale); set_first_process 023B75E0 sets 1; process_normal_main 023B9568 eases it to 1; quaternion_main 023B8388 scales the wind by it |

## boomerangIndexBuffer_c (d_a_boomerang.h): index buffer fields

Not offset names: all four words of d_a_boomerang's view of agl::IndexBufferLayout (027B5430 construct, 027B54E0
setStorage with format 4 and count 4 in daBoomerang_sightPacket_c::setup 020CC5EC / daBoomerang_blur_c::setup 020CCA50)
were named against their GX2 argument positions (same pattern as daPirate_Flag_packet_c above, mantHD::IndexBuffer
in enemies.md). GX2 argument order: `GX2DrawIndexedEx(mode r3, count r4, indexType r5, indices r6, baseVertex r7,
numInstances r8)`; both draw sites (daBoomerang_sightPacket_c::draw 020CDBFC, daBoomerang_blur_c::draw 020CDF78 through
boomerangDrawBlurRing) pass word 4, word 0xC, word 0, word 8. The draw sites' locals follow (`mode`, `indexCount`,
`indexFormat`, `indices`). The view is used by daBoomerang_sightPacket_c, daBoomerang_blur_c and boomerangSightBuffers_c.

| View offset | Old | New | Evidence |
|---|---|---|---|
| 0x0 | `mpIndices` (misnamed) | `mIndexFormat` | GX2DrawIndexedEx indexType (r5) in 020CDBFC / 020CDF78; agl::IndexBufferLayout::mFormat |
| 0x4 | `mCount` (misnamed) | `mPrimitiveMode` | GX2DrawIndexedEx mode (r3) in 020CDBFC / 020CDF78; 020CC5EC / 020CCA50 store 6 (triangle strip); agl::IndexBufferLayout::mPrimitiveMode |
| 0x8 | `mOffset` (misnamed) | `mpIndices` | GX2DrawIndexedEx indices (r6) in 020CDBFC / 020CDF78 |
| 0xC | `mPrimitive` (misnamed) | `mCount` | GX2DrawIndexedEx count (r4) in 020CDBFC / 020CDF78, no draw when 0; setStorage count 4 |

Types: the rename commit kept the old declarations (word 0 `gptr<void>`, word 8 `be<u32>`, the reverse of what they
hold); a separate type-fix commit made word 0 `be<u32>` (index format) and word 8 `gptr<void>` (index pointer).

## daObjFtree::Act_c (d_a_obj_ftree.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_obj_ftree.h) keeps GameCube offset names for these and its d_a_obj_ftree.cpp is
stubs only, so every name below comes from our decomp's use (address given). The header was first split into one member per
line with `/* 0xOFF */` prefixes (layout-neutral change). Collision: tww's `M_cyl_srcW` (Tg type AT_TYPE_WATER)
feeds 0x510, `M_cyl_src` feeds 0x680.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3AC | `m294` | `mMediumJointAngleX` | ftree_nodeMediumCallback 02345C68: csXyz(it, mMediumJointAngleY, 0) -> mDoMtx_ZXYrotM on the medium tree's joints; _execute 02347CF0 cLib_addCalcAngleS2 toward mMediumJointAngleXTarget |
| 0x3AE | `m296` | `mMediumJointAngleY` | ftree_nodeMediumCallback 02345C68: the csXyz y angle of the medium tree's joint rotation; _execute 02347CF0 cLib_addCalcAngleS2 toward mMediumJointAngleYTarget |
| 0x3B0 | `m298` | `mLargeJointAngleY` | ftree_nodeLargeCallback 02345DA0: csXyz(0, it, mLargeJointAngleZ) -> the large tree's joint rotation; _execute 02347CF0 cLib_addCalcAngleS2 toward mLargeJointAngleYTarget |
| 0x3B2 | `m29A` | `mLargeJointAngleZ` | ftree_nodeLargeCallback 02345DA0: the csXyz z angle of the large tree's joint rotation; _execute 02347CF0 cLib_addCalcAngleS2 toward mLargeJointAngleZTarget |
| 0x3C0 | `m2A8` | `mMtx` | set_mtx 023467DC: PSMTXCopy of the work matrix (PSMTXTrans current.pos, mDoMtx_ZXYrotM shape_angle) after each model's base transform is set |
| 0x4C4 | `m34C` | `mGroundY` | _create 02346D34: cBgS::GroundCross(mGndChk), the ground height under the tree |
| 0x4CE | `m356` | `mXyEventIdx` | XyEvent_set 02346A10 stores the event index (XyEvent_init 023469BC: -1 = none); XyEvent_exe 023477AC passes it to dEvent_manager_c::endCheck |
| 0x4D0 | `m358` | `mXyEventState` | XyEvent_init 023469BC 0; XyEvent_set 02346A10 1 (refused unless 0); XyEvent_exe 023477AC 1 -> 2 once eventInfo.mCommand is 1, then waits for endCheck |
| 0x4D4 | `m35C` | `mWaterStts` | _create 02346D34: dCcD_Stts::Init, the status of mWaterCyl (mWaterCyl.mStts = &it) |
| 0x510 | `m398` | `mWaterCyl` | _create 02346D34: dCcD_Cyl::Set(M_cyl_srcW), tww's source with Tg type AT_TYPE_WATER; set_collision 023474F4 registers it only while the small tree is shown |
| 0x640 | `m4C8` | `mCylRadius` | set_collision 023474F4: the radius given to mCyl (61, or 132 * largeScale); estimate_water 02346A40 / talk_main 023472BC / _execute 02347CF0 (fopAcM_rollPlayerCrash) read it |
| 0x644 | `m4CC` | `mStts` | _create 02346D34: dCcD_Stts::Init, the status of mCyl (mCyl.mStts = &it) |
| 0x680 | `m508` | `mCyl` | _create 02346D34: dCcD_Cyl::Set(M_cyl_src); set_collision 023474F4 ChkTgHit / SetR / cCcS::Set; action_waitS_main 02348718 / action_waitM_main 02348A38 ChkCoHit |
| 0x7C6 | `m654` | `mSmallBaseColor` | create_heap 02346064: get_tev_material0_color of the small model (material color 2); copied into mSmallColor |
| 0x7D6 | `m664` | `mLargeBaseColor` | create_heap 02346064: get_tev_material0_color of the large model mpModel (material color 2); copied into mLargeColor; action_changeSL2_main 02349238 / action_changeLS_main 023495C0 blend end value |
| 0x7DE | `m66C` | `mSmallColor` | _draw 023480C4: set_tev_color of the small model in MODE_CHANGE_SL2 / MODE_CHANGE_LS; action_changeSL2_main 02349238 / action_changeLS_main 023495C0 blend it |
| 0x7E6 | `m674` | `mLargeColor` | _draw 023480C4: set_tev_color of the large model mpModel; action_changeSL2_main 02349238 / action_changeLS_main 023495C0 blend it |
| 0x80A | `m696` | `mMediumJointAngleXTarget` | _execute 02347CF0: cLib_addCalcAngleS2 target of mMediumJointAngleX; action_pikupikuM_main 02349A20 sets it, process_init 023463E4 clears it |
| 0x80C | `m698` | `mMediumJointAngleYTarget` | _execute 02347CF0: cLib_addCalcAngleS2 target of mMediumJointAngleY; action_pikupikuM_main 02349A20 sets it, process_init 023463E4 clears it |
| 0x814 | `m6A0` | `mLargeJointAngleYTarget` | _execute 02347CF0: cLib_addCalcAngleS2 target of mLargeJointAngleY; action_pikupikuL_main 02349B50 / action_changeSL2_main 02349238 set it (30 * sin), process_init 023463E4 clears it |
| 0x816 | `m6A2` | `mLargeJointAngleZTarget` | _execute 02347CF0: cLib_addCalcAngleS2 target of mLargeJointAngleZ; action_pikupikuL_main 02349B50 / action_changeSL2_main 02349238 set it (36 * sin), process_init 023463E4 clears it |

Open: none of the offset-named fields; `_4D2`, `_7EE`, `_7FA`, `_81C` are padding (no use).

## daStone2::Act_c (d_a_stone2.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_stone2.cpp, which keeps the GameCube offset names) and our decomp's
use (addresses given). The header was first split into one member per line with `/* 0xOFF */` prefixes (layout-neutral
change). The assertion strings in d_a_stone2.cpp still read `m644 >= 0 && m644 < Type_Max` (string data, left
unchanged for object identity); the original binary's text is `M_type >= 0 && M_type < Type_Max`.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x75C | `m644` | `mType` | Create 0249E8BC: prm type (tww Create: m644 = prm_get_type()); the original assertion at d_a_stone2.cpp line 0x247 reads "M_type >= 0 && M_type < Type_Max" (CreateHeap 0249E5B8, Create 0249E8BC); attr() indexes M_attr by it |
| 0x760 | `m648` | `mMode` | mode_wait_init 0249E6A0 / mode_carry_init 0249F9B0 / mode_drop_init 0249FD5C / mode_fine_init 0249F19C store Mode_*; mode_proc_call 0249F3D0 calls mode_proc[it] |
| 0x764 | `m64C` | `mDemo` | demo_non_init 0249E6D0 / demo_req_init 0249F238 / demo_run_init 024A0054 store Demo_*; demo_proc_call 0249F2A4 calls demo_proc[it] |
| 0x768 | `m650` | `mEventIdx` | Create 0249E8BC: dComIfGp_evmng_getEventIdx(prmZ_get_evId()); demo_req_init 0249F238 / demo_req 024A0060 order it (fopAcM_orderOtherEventId), demo_run 024A0128 |
| 0x76C | `m654` | `mFineTimer` | mode_fine_init 0249F19C sets 1; mode_fine 0249FFE0 counts it down and at 0 (type 3, no event) starts the demo |
| 0x76E | `m656` | `mPrmZInit` | prmZ_init 0249E2D0: once-guard, set after mPrmZ is taken from home.angle.z (after TWW daObjSwpush::Act_c::mPrmZInit) |
| 0x76F | `m657` | `mForceExecute` | Create 0249E8BC and cull_set_move 0249EC74 (no cull sphere) set 1; Execute 0249F6CC runs the move step when it is set or the mode is not Mode_Wait, then clears it |
| 0x770 | `m658` | `mGrabbedUp` | Create 0249E8BC 0; mode_carry 0249FF48 sets 1 on daPy getGrabUpStart(); tww draw_shadow then uses the ground shadow |
| 0x771 | `m659` | `mAppear` | Mthd_Create 0249E3A0: chk_appear(); Mthd_Delete 0249E528 tests it |
| 0x772 | `m65A` | `mBgLockTimer` | Create 0249E8BC 2; mode_wait 0249FA28 counts it down and at 0 calls mpBgW->SetLock() (tww mode_wait) |
| 0x774 | `m65C` | `mBreakSmokeCb` | eff_m_break 024A018C: smoke callback of the break particle 0x2027 (ID_AK_JT_ELEMENTSMOKE01); Delete 0249F968 remove(); mode_proc_call 0249F3D0 waits for its end |
| 0x794 | `m67C` | `mBreakSmokePos` | eff_m_break 024A018C: mDoMtx_stack_c::multVec(offset) into it, the position of the break particle 0x2027 |
| 0x7A0 | `m688` | `mLiftSmokeCb` | eff_lift_smoke_start 0249FA68: smoke callback of particle 0x23F2 (ID_AK_JT_LIFTROCKSMOKE00); eff_lift_smoke_end 0249F17C end(); eff_lift_smoke_remove 0249F958 remove() |
| 0x7C0 | `m6A8` | `mLiftSmokeUsed` | Create 0249E8BC 0; eff_lift_smoke_start 0249FA68 1, never cleared; mode_proc_call 0249F3D0 waits for mLiftSmokeCb's end only when set |
| 0x7C1 | `m6A9` | `mLiftSmokeActive` | eff_lift_smoke_start 0249FA68 starts only when 0 and sets 1; eff_lift_smoke_end 0249F17C clears it and ends mLiftSmokeCb |

Open: none of the offset-named Act_c fields. `Attr_c` keeps its two-digit tww offset names (m04, m0E..m5C), out of the
lint pattern; not done here.

### daStone2::Attr_c (M_attr; d_a_stone2.h)

Round 3: the per-type tuning table M_attr[mType] (Attr_c is 0x60 in WWHD as on the GameCube, so the offsets are the old names). Constant data: no field is written in WWHD or tww; the reads are in our d_a_stone2.cpp and match tww's (zeldaret/tww d217e2b). The comma-list declarations were split one per line first (own commit). mBreakSmokeCb.m10 / mLiftSmokeCb.m10 belong to the smoke callback class (dPa_followEcallBack, round 3's copies pass), not to Attr_c.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `m04` | `mModelResIdx` | CreateHeap 0249E5B8: dComIfG_getObjectRes(resName, it) is the model data (tww the same); M_attr is read only, never written in WWHD or tww |
| 0x0C | `m0C` | `mCarryAttnDist` | Create 0249E8BC: attention_info.distances[fopAc_Attn_TYPE_CARRY_e] (tww the same) |
| 0x0E | `m0E` | `mShadowSize` | never read in WWHD (HD draws no simple shadow for the rock); tww Act_c::draw_shadow passes it as dComIfGd_setSimpleShadow / setSimpleShadow2's size |
| 0x10 | `m10` | `mRoofCrrHeight` | Create 0249E8BC: mAcch.SetRoofCrrHeight(it) (tww the same) |
| 0x12 | `m12` | `mRadius` | Create 0249E8BC: mCyl's radius and mAcchCir's wall radius (tww mCyl.SetR / mAcchCir.SetWall) |
| 0x14 | `m14` | `mHeight` | Create 0249E8BC: mCyl's height (tww mCyl.SetH) |
| 0x18 | `m18` | `mDamageSE` | damaged 0249EEAC: the sound started on a hit (tww fopAcM_seStart(this, it, 0)) |
| 0x20 | `m20` | `mMoveCullX` | cull_set_move 0249EC74: the cull sphere's x (tww fopAcM_setCullSizeSphere) |
| 0x22 | `m22` | `mMoveCullY` | cull_set_move 0249EC74: the cull sphere's y (tww fopAcM_setCullSizeSphere) |
| 0x24 | `m24` | `mMoveCullZ` | cull_set_move 0249EC74: the cull sphere's z (tww fopAcM_setCullSizeSphere) |
| 0x26 | `m26` | `mMoveCullRadius` | cull_set_move 0249EC74: the cull sphere's radius; below 0 the rock is always executed (mForceExecute) (tww the same) |
| 0x28 | `m28` | `mDrawCullX` | cull_set_draw 0249F57C: the cull sphere's x (tww fopAcM_setCullSizeSphere) |
| 0x2A | `m2A` | `mDrawCullY` | cull_set_draw 0249F57C: the cull sphere's y (tww fopAcM_setCullSizeSphere) |
| 0x2C | `m2C` | `mDrawCullZ` | cull_set_draw 0249F57C: the cull sphere's z (tww fopAcM_setCullSizeSphere) |
| 0x2E | `m2E` | `mDrawCullRadius` | cull_set_draw 0249F57C: the cull sphere's radius (tww fopAcM_setCullSizeSphere) |
| 0x30 | `m30` | `mGravity` | Create 0249E8BC / mode_drop_init 0249FD5C set gravity to it (tww the same) |
| 0x34 | `m34` | `mPivotHeight` | set_mtx 0249E6DC: while dropping the model rotates about the point it above pos (translate up, rotate, translate back) (tww the same) |
| 0x38 | `m38` | `mAttnHeight` | Create 0249E8BC / Execute 0249F6CC: attention_info.position.y = pos.y + it (tww the same) |
| 0x3C | `m3C` | `mDropSpeedY` | mode_drop_init 0249FD5C: speed.y (tww the same) |
| 0x40 | `m40` | `mDropSpeedF` | mode_drop_init 0249FD5C: speedF (tww the same) |
| 0x44 | `m44` | `mResistLinear` | mode_drop 0249FE90: posMoveF_stream's linear resistance (tww posMoveF_resist_acc param_4) |
| 0x48 | `m48` | `mResistQuad` | mode_drop 0249FE90: posMoveF_stream's quadratic resistance (tww posMoveF_resist_acc param_5) |
| 0x50 | `m50` | `mShockStrength` | damaged 0249EEAC: dComIfGp_getVibration().StartShock(it, ...) when above 0 (tww the same) |
| 0x54 | `m54` | `mMBreakEffScale` | eff_m_break 024A018C: the break particles' scale (tww the same) |
| 0x58 | `m58` | `mBBreakEffScale` | eff_b_break 024A02F0: the break particles' scale (tww the same) |
| 0x5C | `m5C` | `mLiftSmokeScale` | eff_lift_smoke_start 0249FA68: the smoke particles' scale (tww the same) |

Open (no name yet): `m1C` and `m4C` (read nowhere in WWHD or tww); padding `pad0D`, `pad16`, `pad52`.

## daComing3::Act_c (d_a_coming3.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_coming3.cpp, which keeps the `unk_4xx` names) and our decomp's use.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x518 | `unk_400` | `mBarrelId` | coming_start_main 02115788 stores the process ID of the barrel it makes (tww: daObjBarrel2::Act_c::make_coming), fpcM_ERROR_PROCESS_ID_e (0xFFFFFFFF) = none (coming_start_init 02115668); coming_game_main 02115978 / coming_wait_main 02115B50 fopAcM_SearchByID it |
| 0x51C | `unk_404` | `mGameState` | coming_game_main 02115978 switches on it (0 wait for the barrel, 1 follow it, 2 barrel told to exit, 3 done); coming_game_init 02115958 0, coming_start_init 02115668 / coming_wait_init 02115B40 3; _draw 02114F94 draws only in 1 / 2 |
| 0x528 | `unk_410` | `mMtx` | set_mtx 02114C60: copy of mDoMtx_stack_c (tww MTXCopy) after the model's base TR matrix is set; collision_main 02115004 / eff_break_tsubo 021153B4 transform local points with it |
| 0x558 | `unk_440` | `mpModel` | create_heap 02114A78: the J3DModel (tww mDoExt_J3DModel__create); set_mtx 02114C60 / _draw 02114F94 use it |
| 0x55C | `unk_444` | `mProcessIdx` | coming_process_init 02114BA8 stores the index after init_table[idx] succeeds; coming_process_main 02114F04 calls main_table[it] |
| 0x590 | `unk_478` | `mPlayerDist` | coming_start_main 02115788 / coming_game_main 02115978: sqrt of the player's square distance, compared with get_limit_dist(); coming_start_init 02115668 0 |
| 0x594 | `unk_47C` | `mSpawnTimer` | coming_start_init 02115668 30; coming_start_main 02115788 counts it down while the player is beyond get_limit_dist() and makes the barrel at 0, else resets 30 |
| 0x596 | `unk_47E` | `mChallengeNo` | _create 02114D90: shape_angle.z + 1; get_challenge_id 02115694 returns (it - 1) & 1 |

Open: `unk446[0x32]` (no use in tww or here).

## daObjMsdanSub::Act_c (d_a_obj_msdan_sub.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_msdan_sub.cpp, which keeps the GameCube offset names) and our
decomp's use.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3EC | `m2D4` | `mMinObjNo` | Create 0237BA50: the start value of mCurObjNo (0, or 16 for the other size); Execute 0237BCB4 lowers mCurObjNo only while it is above it (tww Execute: mCurObjNo > m2D4) |
| 0x3F4 | `m2DC` | `mMoveY` | Create 0237BA50 home.pos.y; Execute 0237BCB4 adds mSpeedY to it each frame and copies it into current.pos.y for the step whose number is mCurObjNo |
| 0x3F8 | `m2E0` | `mSpeedY` | Create 0237BA50 0; Execute 0237BCB4 steps it by -10 (lowering) / +15 (raising) and adds it to mMoveY; reset to 0 when a step lands |
| 0x3FC | `m2E4` | `mShockDone` | Create 0237BA50 false; Execute 0237BCB4 sets it after the one-time vibration StartShock(1, -0x21) (tww), clears it when the step goes back up (JA_SE_OBJ_SW_STAIR_OFF_1) |

## daObjMovebox::Act_c and EffSmokeCB (d_a_obj_movebox.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_movebox.cpp, which keeps the GameCube offset names; tp c8fa8c9e has
only `field_0x...` names here) and our decomp's use. The header was first split into one member per line with `/* 0xOFF */`
prefixes (layout-neutral change). RideCallBack (02377308) and PPCallBack (023775DC) carry no address marker in
d_a_obj_movebox.cpp; their addresses come from MOVEBOX_RIDE_CALLBACK / MOVEBOX_PUSH_PULL_CALLBACK.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x71C | `m604` | `mAflBobAngle` | mode_afl 0237A0A8 adds attr m38 * (1 + cM_rnd()) to it each frame; the gravity gets attr m34 * cM_ssin(it) (tww mode_afl); Create 02377C1C 0 |
| 0x720 | `m608` | `mRideSinkAccel` | RideCallBack 02377308 adds the rider's push-down (attr m2C player / m30 other, scaled by distance), clamped; mode_afl 0237A0A8 adds it to the gravity and clears it |
| 0x724 | `m60C` | `mSwayTargetX` | RideCallBack 02377308 adds the rider's x offset times attr m74 * m44/m48; afl_sway 02379D00 clamps (mSwayTargetX, mSwayTargetZ) to attr m4C, springs mSwayX toward it, then clears it |
| 0x728 | `m610` | `mSwayTargetZ` | RideCallBack 02377308 adds the rider's z offset times attr m74 * m44/m48; afl_sway 02379D00 springs mSwayZ toward it, then clears it |
| 0x72C | `m614` | `mSwayX` | afl_sway 02379D00 += mSwaySpeedX, zeroed on a left/right wall touch; set_mtx 02377764 tilts the floating box by the up vector (mSwayX, 1, mSwayZ) (tww set_mtx) |
| 0x730 | `m618` | `mSwayZ` | afl_sway 02379D00 += mSwaySpeedZ, zeroed on a front/back wall touch; set_mtx 02377764 uses it as the up vector's z |
| 0x734 | `m61C` | `mSwaySpeedX` | afl_sway 02379D00: spring speed of mSwayX (attr m50 pull toward mSwayTargetX, attr m54 damping) |
| 0x738 | `m620` | `mSwaySpeedZ` | afl_sway 02379D00: spring speed of mSwayZ (attr m50 pull toward mSwayTargetZ, attr m54 damping) |
| 0x73C | `m624` | `mSinkDepth` | mode_afl 0237A0A8: mBgc.mWaterY - current.pos.y clamped to [0, attr m68]; set_mtx 02377764 rotates the floating box about that height (transM(0, it, 0) ... transM(0, -it, 0)); Create 02377C1C attr m68 * 0.5 |
| 0x740 | `m628` | `mGridX` | mode_wait 02379308 / mode_walk 02379834: x position in pushes (transM(it * attr m0C, 0, mGridZ * attr m0C)); mode_walk steps it by +-1 per finished push (directions 1 / 3) |
| 0x744 | `m62C` | `mGridZ` | mode_wait 02379308 / mode_walk 02379834: z position in pushes; mode_walk steps it by +-1 per finished push (directions 0 / 2) |
| 0x748 | `m630` | `mWalkAngleStep` | mode_wait 02379308: 0x8000 / the push length (attr m0A or m04); mode_walk 02379834 eases with cM_scos(mWalkTimer * it) |
| 0x74C | `m634` | `mWalkDir` | mode_wait 02379308: the direction from check_to_walk (index into M_dir_base, -1 none; mode_wait_init 02377B84 -1); mode_walk 02379834 / eff_set_slip_smoke_pos 02379010 use it |
| 0x75C | `m644` | `mWalkTimer` | mode_wait 02379308: the push length in frames (attr m0A or m04); mode_walk 02379834 counts it down and ends the push at 0 |
| 0x75E | `m646` | `mAflCooldown` | Execute 02378638 sets 20 when an afloat box lands (mode_wait_init); mode_wait 02379308 counts it down; a waiting box goes afloat again only at 0 (tww Execute) |
| 0x760 | `m648` | `mLandFxDelay` | Create 02377C1C 0x14; Execute 02378638 counts it down and plays the landing sound / vibration / smoke only once it is 0 |
| 0x762 | `m64A` | `mbPushPullHit` | PPCallBack 023775DC sets it after counting mMomentCnt; check_to_walk 02378D18 starts a walk only when set (and on the ground), then clears it |
| 0x767 | `m64F` | `mbPathSaved` | Create 02377C1C true; mode_walk_init 023792E8 false; mode_wait 02379308 sets it on landing and calls path_save() |

EffSmokeCB (the slip smoke callbacks mSmokeCbs[2]):

| Offset | Old | New | Evidence |
|---|---|---|---|
| 0x20 | `field_0x20` | `mPos` | eff_set_slip_smoke_pos 02379010: PSMTXMultVec of the smoke base position into it (tww multVec(&base_pos[i])); eff_smoke_slip_start 0237918C passes it as the position of particle 0x2022 (ID_AK_JT_ELEMENTSMOKE00) |
| 0x2C | `field_0x2C` | `mAngle` | eff_set_slip_smoke_pos 02379010: (0, push angle, 0) (tww set(0, angle, 0)); eff_smoke_slip_start 0237918C passes it as the angle of particle 0x2022 |

### daObjMovebox::Attr_c (M_attr; d_a_obj_movebox.h)

Round 3: the per-type tuning table M_attr[mType] of the 13 box types (Attr_c is 0x9C in WWHD as on the GameCube, so the offsets are the old names). Constant data: only m0C is filled by the static initializer; no field is written anywhere else in WWHD or tww. The reads are in our d_a_obj_movebox.cpp and match tww's (zeldaret/tww d217e2b). The comma-list declarations were split one per line first (own commit).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x02 | `m02` | `mPushMomentCnt` | check_to_walk 02378D18: a push (mPPLabel without bit 4) starts the walk once every mMomentCnt[i] reaches it (tww the same); M_attr is constant data (only m0C is filled by the static initializer), never written elsewhere in WWHD or tww |
| 0x04 | `m04` | `mPushWalkTime` | mode_wait 02379308: a push walk sets mWalkTimer to it and mWalkAngleStep to 0x8000 / it (tww the same) |
| 0x08 | `m08` | `mPullMomentCnt` | check_to_walk 02378D18: a pull (mPPLabel bit 2, without bit 4) starts the walk once every mMomentCnt[i] reaches it (tww the same) |
| 0x0A | `m0A` | `mPullWalkTime` | mode_wait 02379308: a pull walk (mPPLabel bit 2) sets mWalkTimer to it and mWalkAngleStep to 0x8000 / it (tww the same) |
| 0x0C | `m0C` | `mGridSize` | mode_wait 02379308 / mode_walk 02379834: the position is home + (mGridX, mGridZ) * it (tww the same); the static initializer (d_a_obj_movebox_static.cpp) sets it to 75 for all 13 types |
| 0x10 | `m10` | `mShadowSize` | never read in WWHD (HD draws no simple shadow for the box); tww Act_c::Draw passes it as dComIfGd_setSimpleShadow's size |
| 0x14 | `m14` | `mGravity` | mode_wait_init 02377B84 / Create 02377C1C set gravity to it; mode_afl 0237A0A8 adds it to the buoyancy; Execute 02378638 floats up only when mBuoyancy + it > 0 (tww the same) |
| 0x18 | `m18` | `mAirResistLinear` | mode_wait 02379308 / mode_walk 02379834: posMoveF_stream's linear resistance (tww posMoveF_resist_acc param_4); mode_afl 0237A0A8 the above-water share of it |
| 0x1C | `m1C` | `mAirResistQuad` | mode_wait 02379308 / mode_walk 02379834: posMoveF_stream's quadratic resistance (tww posMoveF_resist_acc param_5); mode_afl 0237A0A8 the above-water share of it |
| 0x20 | `m20` | `mLeaveGroundSpeedY` | Bgc_c::proc_vertical 02376914: speed.y when the box loses the ground (tww the same) |
| 0x28 | `m28` | `mBuoyancy` | mode_afl 0237A0A8: gravity = mGravity + immersion * it + mBobAmplitude * sin(mAflBobAngle); Execute 02378638 floats up only when it + mGravity > 0 (tww the same) |
| 0x2C | `m2C` | `mPlayerRideSink` | RideCallBack 02377308: the sink acceleration of the player riding it (tww fpcNm_PLAYER_e); with mOtherRideSink the lower limit (tww the same) |
| 0x30 | `m30` | `mOtherRideSink` | RideCallBack 02377308: the sink acceleration of a rider that is not the player; with mPlayerRideSink the lower limit (tww the same) |
| 0x34 | `m34` | `mBobAmplitude` | mode_afl 0237A0A8: the gravity term it * sin(mAflBobAngle) (tww the same) |
| 0x38 | `m38` | `mBobAngleStep` | mode_afl 0237A0A8: mAflBobAngle += it * (cM_rnd() + 1) (tww the same) |
| 0x3C | `m3C` | `mWaterResistLinear` | mode_afl 0237A0A8: posMoveF_stream's linear resistance under water (immersion share; mAirResistLinear the rest) (tww the same) |
| 0x40 | `m40` | `mWaterResistQuad` | mode_afl 0237A0A8: posMoveF_stream's quadratic resistance under water (immersion share; mAirResistQuad the rest) (tww the same) |
| 0x44 | `m44` | `mPlayerRideTilt` | RideCallBack 02377308: the player's sway moment is mInvRideRadius * it (tww the same) |
| 0x48 | `m48` | `mOtherRideTilt` | RideCallBack 02377308: another rider's sway moment is mInvRideRadius * it (tww the same) |
| 0x4C | `m4C` | `mSwayTargetMax` | afl_sway 02379D00: the sway target is clamped to this length (tww the same) |
| 0x50 | `m50` | `mSwaySpring` | afl_sway 02379D00: the sway acceleration -(sway - target) * it (tww the same) |
| 0x54 | `m54` | `mSwayDamping` | afl_sway 02379D00: the sway speed damping -speed * it (tww the same) |
| 0x68 | `m68` | `mBoxHeight` | set_mtx 02377764 / Create 02377C1C: the buoy sits at it - 5 above pos; mode_afl 0237A0A8: fully immersed below -it, mSinkDepth capped at it (tww the same) |
| 0x6C | `m6C` | `mImmersionRate` | mode_afl 0237A0A8: immersion = depth * it (1 once deeper than mBoxHeight) (tww the same) |
| 0x74 | `m74` | `mInvRideRadius` | RideCallBack 02377308: the rider's weight falls off as 1 - distance * it, and the sway moments are it * mPlayerRideTilt / mOtherRideTilt (tww the same) |
| 0x9A | `m9A` | `mbHeavy` | PPCallBack 023775DC: a push / pull counts only with PPLABEL_HEAVY (power bracelets) when set; Bgc_c::proc_vertical 02376914 / check_to_walk 02378D18 / afl_sway 02379D00 use the 21-line M_lin20 instead of M_lin5 (tww the same) |

Open (no name yet): `m00` and `m06` (check_to_walk 02378D18: the push / pull moment counts when mPPLabel bit 4 is set; that bit is tww's PPLABEL_UNK4, set by Link's setPushPullKeepData from an unnamed proc variable, so the case has no name yet); padding `_3A`, `_9B`.

### daObjMovebox::BgcSrc_c (Bgc_c::M_lin5 / M_lin20; d_a_obj_movebox.h)

Round 3: the ground / wall check line sources. tww's header comments mark m00 / m08 as z and m04 / m0C as x.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mScaledZ` | Bgc_c::gnd_pos 02376314: the check point's z is it * mScaleXZ + mOffsetZ; wall_pos 023765D0 / chk_wall_touch 02376B54: the z scaled by the box size (tww comment "z"); M_lin20 is filled by its static initializer (d_a_obj_movebox_static.cpp), never written elsewhere |
| 0x04 | `m04` | `mScaledX` | Bgc_c::gnd_pos 02376314: the check point's x is it * mScaleXZ + mOffsetX; wall_pos 023765D0 / chk_wall_touch 02376B54: the x scaled by the box size (tww comment "x") |
| 0x08 | `m08` | `mOffsetZ` | Bgc_c::gnd_pos 02376314: the unscaled z added to mScaledZ * mScaleXZ; wall_pos 023765D0 / chk_wall_touch 02376B54: the line's z offset (tww comment "z") |
| 0x0C | `m0C` | `mOffsetX` | Bgc_c::gnd_pos 02376314: the unscaled x added to mScaledX * mScaleXZ; wall_pos 023765D0 / chk_wall_touch 02376B54: the line's x offset (tww comment "x") |

## daObjBarrel::Act_c (d_a_obj_barrel.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_barrel.cpp, which keeps the GameCube offset names) and our decomp's use. The view `Player01BarrelState` (d_a_player_main_01.h, used by daPy_lk_c::freeGrabItem) is renamed with the class.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x72C | `m610` | `mTiltAngle` | set_mtx 0231C380: XrotM(it) between YrotM(mTiltDir) and YrotM(-mTiltDir) (tww set_mtx); mode_vib0_init 0231D344 3500 (tww attr().m2A), mode_vib2 0231E154 chases it to 0, set_walk_rot 0231DB3C sets it from the walk speed (tww vibFactor * l_max_vib_angl) |
| 0x72E | `m612` | `mTiltDir` | set_mtx 0231C380: the Y angle about which mTiltAngle is applied (YrotM(it) ... YrotM(-it)); set_walk_rot 0231DB3C / vib_pos_ang 0231DED0 advance it, mode_jump_init 0231CC9C / mode_vib0_init 0231D344 / mode_carry 0231DE48 clear it |
| 0x74C | `m630` | `mRollAngle` | set_mtx 0231C380: a further YrotM(it) after the tilt; set_walk_rot 0231DB3C advances it by the distance rolled over the barrel's circumference (tww mag / ((cos * 5 + l_s_radius) * 6.28) * 0xFFFF); mode_jump_init 0231CC9C / mode_vib0_init 0231D344 clear it |
| 0x738 | `m61C` | `mSlantAngle` | tww accessors get_slant_angle() { return m61C; } / set_slant_angle(s16); mode_carry 0231DE48 chases shape_angle.z toward it (tww cLib_chaseAngleS(&shape_angle.z, m61C, 0x1000)); daPy_lk_c::freeGrabItem 023DCF8C clears it through the view Player01BarrelState |

## daObjSwheavy::Act_c (d_a_obj_swheavy.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_swheavy.cpp, which keeps the GameCube offset names) and our decomp's use.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x444 | `m328` | `mBottomPos` | calc_top_pos 02394730: (1 - mCurHFrac) * -35.5 limited to [-36.5, 1] (tww calc_top_pos), the lower bound of mTopPos; set_mtx 02393F08 adds it to the model's Y; _execute 023948D4 eyePos; _create 02394114 0 / -35.5 (after mTopPos) |
| 0x448 | `m32C` | `mTopAimHFrac` | calc_top_pos 02394730: takes mTopAimReqHFrac when mTopAimTimer runs out and is the top's height fraction outside mode 0; _create 02394114 1 / 0 |
| 0x44C | `m330` | `mTopAimTimer` | top_bg_aim_req 02394ADC sets it (tww param_2); calc_top_pos 02394730 counts it down and copies mTopAimReqHFrac into mTopAimHFrac at 0; _create 02394114 0 |
| 0x450 | `m334` | `mTopAimReqHFrac` | top_bg_aim_req 02394ADC stores the requested height fraction (tww param_1); calc_top_pos 02394730 copies it into mTopAimHFrac when mTopAimTimer reaches 0; _create 02394114 0 |
| 0x454 | `m338` | `mTopHFrac` | calc_top_pos 02394730: mCurHFrac in mode 0, else mTopAimHFrac; mTopPos = (1 - it) * -35.5 (tww calc_top_pos); _create 02394114 1 / 0 |

## daObjLadder::Act_c (d_a_obj_ladder.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_ladder.cpp, which keeps the GameCube offset names) and our decomp's use.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3F6 | `unk2DE` | `mBounceCount` | mode_drop_init 023698FC sets it to attr field_0x10 (3); mode_drop 023699A0 plays JA_SE_OBJ_LADDER_FALL_1 + shock on the first landing (it == attr field_0x10), FALL_2 with a volume per bounce otherwise, and counts it down per bounce (tww mode_drop) |
| 0x450 | `unk338` | `mVibXAngle` | mode_vib_init 02369810 0; mode_vib 02369934 adds attr field_0x16 (20000) and sets mVibXOffset = cM_scos(it) * attr field_0x1C |
| 0x452 | `unk33A` | `mVibYAngle` | mode_vib_init 02369810 0; mode_vib 02369934 adds attr field_0x18 (0x3CC3) and sets mVibYOffset = cM_scos(it) * attr field_0x20 |
| 0x45E | `unk346` | `mDemoAccepted` | mode_demoreq 02369830 sets 1 when eventInfo.checkCommandDemoAccrpt(); demo_end_reset 023695C8 waits for dComIfGp_evmng_endCheck(mEventIdx) only while set, then clears it; Create 023693E0 / mode_demoreq_init 0236979C 0 |

## daObjSwpush::Act_c (d_a_obj_swpush.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_swpush.cpp, which keeps the GameCube offset names) and our decomp's use.
The top-height fields follow the same scheme as daObjSwheavy::Act_c (same names, same calc_top_pos / top_bg_aim_req code).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3F0 | `m2D4` | `mOpVtxBaseY` | Mthd_Create 023975B0: the Y of the moving top vertex in mpBgW's vertex table (tww GetVtxTbl()[M_op_vtx[0]].y); Mthd_Execute 02398014 sets those vertices to it + mTopPos |
| 0x498 | `m31C` | `mBottomPos` | calc_top_pos 02397E70: (1 - mCurHFrac) * -35.5 limited to [-36.5, 1] (tww calc_top_pos), the lower bound of mTopPos; jnodeCB 02396BC8 translates the joint by it; Mthd_Execute 02398014 eyePos; Mthd_Create 023975B0 0 / -35.5 (as daObjSwheavy::Act_c::mBottomPos) |
| 0x49C | `m320` | `mTopAimHFrac` | calc_top_pos 02397E70: takes mTopAimReqHFrac when mTopAimTimer runs out and is the top's height fraction outside MODE_UPPER; Mthd_Create 023975B0 1 / 0 |
| 0x4A0 | `m324` | `mTopAimTimer` | top_bg_aim_req 0239833C sets it (tww param_2); calc_top_pos 02397E70 counts it down and copies mTopAimReqHFrac into mTopAimHFrac at 0; Mthd_Create 023975B0 0 |
| 0x4A4 | `m328` | `mTopAimReqHFrac` | top_bg_aim_req 0239833C stores the requested height fraction (tww param_1); calc_top_pos 02397E70 copies it into mTopAimHFrac when mTopAimTimer reaches 0; Mthd_Create 023975B0 0 |
| 0x4A8 | `m32C` | `mTopHFrac` | calc_top_pos 02397E70: mCurHFrac in MODE_UPPER, else mTopAimHFrac; mTopPos = (1 - it) * -35.5 (tww calc_top_pos); Mthd_Create 023975B0 1 / 0 |

## daTagLight::Act_c (d_a_tag_light.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_tag_light.cpp, which keeps the GameCube offset names) and our decomp's use.
The header was first split into one member per line with `/* 0xOFF */` prefixes (layout-neutral change).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3BC | `m2A0` | `mType` | _create 024ADB04: prm type (tww prm_get_type(); Type_Box / Type_Cone / Type_Detector); _execute 024AE0E8 / _draw 024AE430 / _delete 024AE044 branch on it |
| 0x3C0 | `m2A4` | `mMtx` | _create 024ADB04: copy of mDoMtx_stack_c (tww cMtx_copy), given to fopAcM_SetMtx in tww; set_spot_mtx 024AD894 / get_projection_info 024AD198 transform local points with it |
| 0x3F0 | `m2D4` | `mInvMtx` | _create 024ADB04: MTXInverse(mMtx, it) |
| 0x420 | `m304` | `mLightOn` | _create 024ADB04 / _execute 024AE0E8: chk_sw() && chk_sch() (tww), false for Type_Detector; picks the fade target of mAlpha |
| 0x421 | `m305` | `mProjMtxValid` | make_projection_mtx 024AD594: 0 when the cone angle's cosine is about 0 (no projection), else 1; tww _draw draws the light model only when set |
| 0x422 | `m306` | `mLitCount` | _execute 024AE0E8 (Type_Detector): +1 while chk_light(), else -1, limited to [0, 20]; above 10 it turns the switch on (tww); _create 024ADB04 0 |
| 0x424 | `m308` | `mAlpha` | _execute 024AE0E8: cLib_chaseF toward mLightOn ? 1 : 0 at M_alpha_spd[fadeType]; _draw 024AE430 draws the cone only above 0, alpha it * 255.5; _create 024ADB04 1 / 0 |
| 0x590 | `m474` | `mProjMtx` | make_projection_mtx 024AD594: copy of mDoMtx_stack_c (the light's projection matrix); tww _draw passes it to dComIfGd_setLightModel |
| 0x5C0 | `m4A4` | `mSize` | _create 024ADB04: the scale-derived size (tww: scale with x/y/z doubled per type); make_projection_mtx 024AD594 / get_projection_info 024AD198 use its x and z as the light's width and depth |

## daObjFlame::Act_c (d_a_obj_flame.h)

zeldaret/tww's d_a_obj_flame.cpp is mostly stubs, so these names come from our decomp's use (addresses given). The header was
first split into one member per line with `/* 0xOFF */` prefixes (layout-neutral change). KI = Keese (fpcNm_KI_e).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x550 | `m434` | `mbCpsActive` | em_position 02343D7C: set when the flame is tall enough (1500 * mHeight - 300 > 0) and only then places mCpsP1; mode_proc_call 02344DF4 registers mCps only while set; _create 0234521C 0 |
| 0x573 | `m457` | `mbEmPosUpdate` | em_position 02343D7C returns at once unless set; create_mode_init 0234365C / mode_wait 02344A8C / mode_l_u 02344CA4 / mode_u_l 02344D28 set it, mode_l_before 02344C50 clears it |
| 0x574 | `m458` | `mReverb` | _create 0234521C: dComIfGp_getReverb(home.roomNo); mode_l_before 02344C50 / mode_proc_call 02344DF4 pass it to mDoAud_seStart (as daObjMovebox::Act_c::mReverb) |
| 0x575 | `m459` | `mbLiftingObj` | liftup_magmarock 023443A0 / liftup_mflft 023446B0 set it when they lift an object this frame (cleared before the search, 023455E0); em_simple_set 02344014 / em_manual_set 023432B4 / em_manual_inv 02344230 hold the emitter back while set; _create 0234521C 0 |
| 0x576 | `m45A` | `mbKiMake` | ki_init 02344A30 sets it when the prm Keese count is not 32; ki_make 02345068 runs only while set and clears it when the count is reached or a delay starts |
| 0x578 | `m45C` | `mKiDelay` | _create 0234521C: random delay from prm(2, 13) (at least 1 outside MODE_WAIT); ki_make 02345068 counts it down before spawning and reloads it from prm(2, 13) after the last Keese |
| 0x57C | `m460` | `mKiTimer` | ki_init 02344A30 0; ki_make 02345068 counts it down and spawns a Keese (fpcNm_KI_e) every 5 frames |
| 0x580 | `m464` | `mKiCount` | ki_init 02344A30 0; ki_make 02345068: Keese spawned so far, compared with the prm count (prm(5, 8) + 1) |
| 0x588 | `m46C` | `mHeightRatio` | em_position 02343D7C / liftup_magmarock 023443A0 / liftup_mflft 023446B0: factor on the flame height (1500 * mHeight * attr scale * it); liftup_mflft eases it toward 1; _create 0234521C 1 |

Open: padding with no use: `m435[3]`, `m45B`, `m46A[2]`, `m480[0x80]`.

## daObjHami3::Act_c (d_a_obj_hami3.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_hami3.cpp, which keeps `field_0x...` names) and our decomp's use.
The header was first split into one member per line (layout-neutral change).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3E0 | `field_0x2C8` | `mOpenAngle` | Create 0234F0C8: OPEN_ANGLE (0x4000) when the switch is on, else 0; daObjHami3_open_demo 0234F318 / daObjHami3_close_demo 0234F4AC step it by ROTATION_STEP (0x100); nodeCallBack 0234EC78 rotates the gate joints by it (tww cMtx_YrotM); Execute 0234F564 adds it to home.angle.x |
| 0x3E4 | `field_0x2CC` | `mPhs` | Mthd_Create 0234ED9C / Mthd_Delete 0234EE70: the request_of_phase_process_class of dComIfG_resLoad / dComIfG_resDeleteDemo (the usual mPhs of the other Act_c classes) |
| 0x3EC | `field_0x2D4` | `mpModel` | CreateHeap 0234EEBC: the J3DModel (tww mDoExt_J3DModel__create); set_mtx 0234EFD4 / init_mtx 0234F0A8 / Draw 0234F724 use it |
| 0x3F0 | `field_0x2D8` | `mMode` | Create 0234F0C8 OPEN_STOP / CLOSE_STOP; the mode functions 0234F210..0234F4AC store Act_c::Mode values; Execute 0234F564 switches on it |
| 0x3F4 | `field_0x2DC` | `mEventIdx` | Create 0234F0C8: dComIfGp_evmng_getEventIdx("AMI3") (tww); the open / close waits pass it to fopAcM_orderOtherEventId |

Open: `field_0x2CA`, `field_0x2DE` (no use in tww or here).

## daObjFerris::Act_c (d_a_obj_ferris.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_ferris.cpp, which keeps `field_0x...` names) and our decomp's use:
the three per-gondola collision pairs (arrays of 5).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3F0 | `field_0x02d4` | `mGondolaStts` | _create 0233DD60: dCcD_Stts::Init of each gondola's first cylinder (tww: mGondolaCyl[i].SetStts(&it[i])) |
| 0x51C | `field_0x0400` | `mGondolaCyl` | set_collision 0233E9C4: per gondola, R 50 / H 195 at gondola-local (0, -100, -135) while the gondola is below 1820 (tww set_collision, M_cyl_src); _create 0233DD60 constructs / Sets it |
| 0xB0C | `field_0x09f0` | `mGondolaStts2` | _create 0233DD60: dCcD_Stts::Init of each gondola's second cylinder (tww: mGondolaCyl2[i].SetStts(&it[i])) |
| 0xC38 | `field_0x0b1c` | `mGondolaCyl2` | set_collision 0233E9C4: per gondola, R 50 / H 148 at gondola-local (0, -56, 127) (tww set_collision, M_cyl_src) |
| 0x1228 | `field_0x110c` | `mHangStts` | _create 0233DD60: dCcD_Stts::Init of each hanging-point sphere (tww: mHangSph[i].SetStts(&it[i])) |
| 0x1354 | `field_0x1238` | `mHangSph` | set_collision 0233E9C4: per gondola, R 30 sphere at the gondola's hanging point on the turning wheel (the five rim offsets rotated by mRotAngle) while below 1820 (tww set_collision, M_sph_src) |

Open: `field_0x1976` (no use in tww or here).

## daComing2::Act_c and its info structs (d_a_coming2.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_coming2.cpp, which keeps the `unk_...` names) and our decomp's use.
The header was first split into one member per line (layout-neutral change).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3AC | `unk_294` | `mGameCount` | coming_game_init 02114604 counts it up once per game start (tww unk_294++) |
| 0x3B0 | `unk_298` | `mStartRoomNo` | coming_setF_init 021140E0 stores the player's room; coming_game_init 02114604 copies it back into mPlayerRoomNo |
| 0x3B1 | `unk_299` | `mPlayerRoomNo` | the player's last room: _create 021133BC -1; coming_wait_main 02113FD4 / coming_checkS_main 021144C4 / coming_clear_init 02113F18 store current.roomNo; coming_wait_main starts the course when the player has left it (tww) |
| 0x3B2 | `unk_29A` | `mMakeReq` | barrel_execute 02113754 / coming_game_init 02114604 set 1; coming_game_main 02114630 makes the next barrel while it is 1 and clears it once made; init_coming_info 02113288 0 |
| 0x3B4 | `unk_29C` | `mScopeSide` | init_coming_info 02113288: cM_rndF(2) & 1; renew_scope_info 021134F8 picks the turn sign ((mBarrelIdx + it) & 1 ? -0x546 : 0x546) |
| 0x3B8 | `unk_2A0` | `mSpeed` | renew_scope_info 021134F8: the ship's speed (tww get_speed(this)); coming_wait_main 02113FD4 needs it above 35; coming_setF_main 02114298 places the start ahead by it * 120 |
| 0x3BC | `unk_2A4` | `mBarrelIdx` | the next barrel's index in mBarrelInfo (barrels made so far): init_coming_info 02113288 / coming_wait_init 02113FBC 0; coming_game_main 02114630 advances it; barrel_execute 02113754 ends at 15; make_coming_param 02113E50 / renew_scope_info 021134F8 index by it |
| 0x3BE | `unk_2A6` | `mWaitTimer` | init_coming_info 02113288 / coming_wait_init 02113FBC 600; coming_wait_main 02113FD4 counts it down and starts the course only at 0 |
| 0x3C0 | `unk_2A8` | `mMakeTimer` | coming_game_main 02114630: frames since the last barrel attempt, retried at 60 (tww unk_2A8++ >= 60); init_coming_info 02113288 / coming_game_init 02114604 0 |
| 0x558 | `unk_440` | `mStartPos` | _create 021133BC / coming_wait_main 02113FD4: current.pos saved together with mStartStat (the start angle) when the course starts (tww) |
| 0x584 | `unk_46C` | `mCheckTimer` | coming_checkS_init 021144B4 0; coming_checkS_main 021144C4 counts it up to 180 |
| 0x586 | `unk_46E` | `mProcessIdx` | coming_process_init 02113304 stores the process index; coming_process_main 02113680 calls main_table[it] (as daComing3::Act_c::mProcessIdx) |
| 0x5C0 | `unk_4A8` | `mScopePos` | renew_scope_info 021134F8: the point ahead of the ship (matrix times the forward vector, tww cMtx_multVec(.., &unk_4A8)); tww make_coming_param places the next barrel from it |

| Struct offset | Old | New | Evidence |
|---|---|---|---|
| _barrel_info_ 0x00 | `unk_00` | `mBarrelId` | the barrel's process ID; init_barrelN_info 0211321C clears it, coming_game_main 02114630 stores it, barrel_execute 02113754 searches it |
| _barrel_info_ 0x04 | `unk_04` | `mExitTimer` | coming_game_main 02114630 sets 0x93 - mBarrelIdx; barrel_execute 02113754 counts it down, request_barrel_exitN at 0 |
| _barrel_info_ 0x06 | `unk_06` | `mItemGive` | barrel_execute 02113754: the barrel's item-give flag (tww chk_item_give()) |
| _barrel_info_ 0x08 | `unk_08` | `mPos` | coming_game_main 02114630 stores the barrel position; renew_scope_info 021134F8 measures from the previous one |
| _start_buoy_ 0x00 | `unk_00` | `mBuoyId` | the start buoy's process ID; init_flag_info 021132F4 clears, coming_setF_main 02114298 stores |
| _start_buoy_ 0x04 | `unk_04` | `mPos` | coming_setF_main 02114298 stores it; coming_checkS_main 021144C4 checks the line between the two buoys (tww checkLineCrossXZ) |
| _start_stat_ 0x00 | `unk_00` | `mAngleY` | coming_wait_main 02113FD4 / _create 021133BC store shape_angle.y with mStartPos; the scope / buoy / barrel rotations start from it |

Open: `unk_2AA`, `unk_476` (no use in tww or here).

## daObjJump::Act_c (d_a_obj_jump.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_jump.cpp, which keeps `field_0x...` names) and our decomp's use.
The header was first split into one member per line (layout-neutral change). "Pulse" flags are 1 for the one frame
their counter reaches its attr threshold (set_push_flag).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3FC | `field_0x338` | `mScaleTarget` | calc_vib_pos 02366668: the spring target of mScale (tww calc_vib_pos); Create 023662E4 / mode_wait_init 023662C8 1, mode_w_l_init 0236685C attr field_0x038, mode_l_u_init 023669C0 attr field_0x03C; mode_w_l 023669AC / mode_l_u compare mScale with it |
| 0x400 | `field_0x33C` | `mScale` | calc_vib_pos 02366668 adds mScaleSpeed to it; jnodeCB_lower 02365DD8 scales the joint by (it, 1, 1) and back (tww scaleM(i_this->field_0x33C, 1, 1)); set_mtx 023660A4 offsets by it; Create 023662E4 1 |
| 0x404 | `field_0x340` | `mScaleSpeed` | calc_vib_pos 02366668: spring speed (pull attr field_0x02C toward mScaleTarget, damping attr field_0x028); mode_wait 0236687C / mode_lower 023669E0 add the push impulse attr field_0x030, Execute 023666C8 attr field_0x034; mode_l_u 02366AF4 0 |
| 0x409 | `field_0x345` | `mbRidePulse` | set_push_flag 0236642C: 1 for the frame mRideCount reaches attr field_0x043 (tww set_push_flag); mode_wait 0236687C pushes on it |
| 0x40A | `field_0x346` | `mbUnridePulse` | set_push_flag 0236642C: 1 for the frame mUnrideCount reaches attr field_0x044; mode_wait 0236687C pushes on it |
| 0x40B | `field_0x347` | `mRideCount` | set_push_flag 0236642C: frames with something riding (mIsRide), capped at 0xFF, 0 otherwise |
| 0x40C | `field_0x348` | `mUnrideCount` | set_push_flag 0236642C: frames with nothing riding, capped at 0xFF, 0 while ridden |
| 0x40E | `field_0x34a` | `mbHangPulse` | set_push_flag 0236642C: 1 for the frame mHangCount reaches attr field_0x045; mode_wait 0236687C pushes on it |
| 0x40F | `field_0x34b` | `mHangCount` | set_push_flag 0236642C: frames the player rides it while hanging (tww mIsPlayerRide && daPyStts0_HANG_e), capped at 0xFF |
| 0x411 | `field_0x34d` | `mbHeavyRidePulse` | set_push_flag 0236642C: 1 for the frame mHeavyRideCount reaches attr field_0x046; mode_wait 0236687C pushes on it |
| 0x412 | `field_0x34e` | `mbHeavyUnridePulse` | set_push_flag 0236642C: 1 for the frame mHeavyUnrideCount reaches attr field_0x047; mode_lower 023669E0 pushes on it |
| 0x413 | `field_0x34f` | `mHeavyRideCount` | set_push_flag 0236642C: frames ridden with the Iron Boots (mIsHeavyRide), capped at 0xFF; mode_wait 0236687C lowers above attr field_0x040 |
| 0x414 | `field_0x350` | `mHeavyUnrideCount` | set_push_flag 0236642C: frames without the Iron Boots on it, capped at 0xFF; mode_lower 023669E0 rises above attr field_0x041 |
| 0x416 | `field_0x352` | `mPushTimer` | mode_wait 0236687C / mode_lower 023669E0: after a push it is set to attr field_0x048 and counts down, no new push before 0; mode_wait_init 023662C8 / mode_lower_init 02366998 0 |
| 0x418 | `field_0x354` | `mbFastRide` | rideCB 02366268 sets it when the riding player's speedF exceeds attr field_0x04C (tww rideCB); clear_push_flag 023666B0 clears it; Execute 023666C8 adds attr field_0x034 to mScaleSpeed in Mode_WAIT_e / Mode_LOWER_e while set |

daObjJump::Attr_c (the M_attr table):

| Offset | Old | New | Evidence |
|---|---|---|---|
| 0x024 | `field_0x024` | `mScalePivotY` | set_mtx 023660A4: the Y about which the body is scaled (tww transM(0, it, 0), scaleM, transM(0, -it, 0)) |
| 0x028 | `field_0x028` | `mSpringDamping` | calc_vib_pos 02366668: mScaleSpeed -= mScaleSpeed * it (tww) |
| 0x02C | `field_0x02C` | `mSpringStiffness` | calc_vib_pos 02366668: mScaleSpeed -= (mScale - mScaleTarget) * it (tww) |
| 0x030 | `field_0x030` | `mPushImpulse` | mode_wait 0236687C / mode_lower 023669E0: added to mScaleSpeed on a push |
| 0x034 | `field_0x034` | `mFastRideImpulse` | Execute 023666C8: added to mScaleSpeed while mbFastRide |
| 0x038 | `field_0x038` | `mLowerScale` | mode_w_l_init 0236685C: mScaleTarget when lowering |
| 0x03C | `field_0x03C` | `mUpperScale` | mode_l_u_init 023669C0: mScaleTarget when rising |
| 0x040 | `field_0x040` | `mHeavyRideFrames` | mode_wait 0236687C: lowers (JA_SE_OBJ_JUMP_SPR_CLOSE) once mHeavyRideCount exceeds it (tww) |
| 0x041 | `field_0x041` | `mHeavyUnrideFrames` | mode_lower 023669E0: rises (JA_SE_OBJ_JUMP_SPR_OPEN) once mHeavyUnrideCount exceeds it (tww) |
| 0x042 | `field_0x042` | `mUpperTime` | mode_upper_init 02366AD4: mTimer = it (tww mode_upper_init) |
| 0x043 | `field_0x043` | `mRidePulseFrames` | set_push_flag 0236642C: mbRidePulse when mRideCount reaches it |
| 0x044 | `field_0x044` | `mUnridePulseFrames` | set_push_flag 0236642C: mbUnridePulse when mUnrideCount reaches it |
| 0x045 | `field_0x045` | `mHangPulseFrames` | set_push_flag 0236642C: mbHangPulse when mHangCount reaches it |
| 0x046 | `field_0x046` | `mHeavyRidePulseFrames` | set_push_flag 0236642C: mbHeavyRidePulse when mHeavyRideCount reaches it |
| 0x047 | `field_0x047` | `mHeavyUnridePulseFrames` | set_push_flag 0236642C: mbHeavyUnridePulse when mHeavyUnrideCount reaches it |
| 0x048 | `field_0x048` | `mPushInterval` | mode_wait 0236687C / mode_lower 023669E0: mPushTimer after a push |
| 0x04C | `field_0x04C` | `mFastRideSpeed` | rideCB 02366268: mbFastRide when the riding player's speedF exceeds it |

Open: Attr_c `field_0x018`, `field_0x01C`, `field_0x020` (heights used by set_mtx / jnodeCB_lower: the joint offset grows by (0x1C - 0x18) * (mScale - 1), the body scale is (0x20 - 0x24 + (0x1C - 0x18) * (mScale - 1)) / (0x20 - 0x24); their single meanings are not proven).

## daObj_Stair_c (d_a_obj_stair.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_stair.cpp, which keeps `field_0x...` names) and our decomp's use.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3E0 | `field_0x2C8` | `mHitOffset` | ride_call_back 023905F4 / _execute 02390AC4 (bomb hit): the rider's or bomb's position minus the step's (tww); _execute uses its direction and length for the break tilt |
| 0x3EC | `field_0x2D4` | `mBreakDir` | ride_call_back 023905F4 (0, -1, 0) for the player; _execute 02390AC4 the bomb-to-step vector clamped to the step (tww); the break speed is l_HIO.mBreakSpeed * mBreakPower * it, the tilt axis is mHitOffset x it |
| 0x3F8 | `field_0x2E0` | `mBreakTimer` | _execute 02390AC4: frames since the break started; the break happens at l_HIO.m18, the fall after it, fopAcM_delete at l_HIO.m18 + 0xF0 (tww); ride_call_back 023905F4 only triggers while it is below l_HIO.m18; _create 0239069C 0 |
| 0x3FC | `field_0x2E4` | `mBreaking` | ride_call_back 023905F4 / _execute 02390AC4 (bomb hit) set 1; _execute runs the break and Execute 02391328 shakes the debris position while it is 1 (tww) |
| 0x400 | `field_0x2E8` | `mShakeY` | _execute 02390AC4: 1.5 * sin offset; Execute 02391328 adds it to current.pos.y (tww transS(.., current.pos.y + it, ..)); _create 0239069C 0 |
| 0x404 | `field_0x2EC` | `mBreakPower` | ride_call_back 023905F4 1; _execute 02390AC4 scales the break speed and the tilt angle by it (tww); _create 0239069C 0 |
| 0x414 | `field_0x2FC` | `mMtx` | Execute 02391328: copy of mDoMtx_stack_c, handed out as the BG matrix (tww MTXCopy(.., it); *mtx = &it) |
| 0x5B8 | `field_0x4A0` | `mRot` | _create 0239069C: the start rotation; Execute 02391328 multiplies it by mRotSpeed each frame (tww mDoMtx_quatMultiply(&it, &mRotSpeed, &quat); it = quat) and applies it (quatM) |
| 0x5C8 | `field_0x4B0` | `mRotSpeed` | _execute 02390AC4: the per-frame break rotation (axis mHitOffset x mBreakDir, angle from l_HIO.m10 and mBreakPower; identity when the axis is 0) (tww); _create 0239069C identity |

Open: `field_0x4C0` (Execute 02391328 stores a random point on the step into it while breaking; no reader in tww or here).

## daObjIce_c (d_a_obj_ice.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_ice.cpp, which keeps the GameCube offset names) and our decomp's use.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x528 | `m40C` | `mMtx` | set_mtx 0235D664: copy of mDoMtx_stack_c (tww cMtx_copy); create_heap 0235D588 gives it to the background (tww dBgW_NewSet(.., &it)) |
| 0x558 | `m43C` | `mActProc` | _execute 0235DFF8 calls it (pointer to member); tww sets &daObjIce_c::wait_act_proc / &daObjIce_c::fade_out_retire_act_proc |
| 0x560 | `m448` | `mAppear` | _create 0235DB5C: chk_appear() (tww); _delete 0235DF70 tests it |
| 0x564 | `m44C` | `mScaleXZ` | set_mtx 0235D664: scaleM(it, mScaleY, it); tg_hitCallback 0235D774 shrinks it on non-fire hits (tww -= 0.0009) and scales the hit ellipse; fade_out_retire_act_proc 0235E820 shrinks it; _create 0235DB5C 1 |
| 0x568 | `m450` | `mScaleY` | set_mtx 0235D664: the Y scale; tg_hitCallback 0235D774 / fade_out_retire_act_proc 0235E820 set it to 1 - cos(mScaleXZ * 0x4000); _execute 0235DFF8 cylinder height; _create 0235DB5C 1 |
| 0x56C | `m454` | `mAlpha` | _create 0235DB5C 255; fade_out_retire_act_proc 0235E820 fades it (tww -= 2.8333); _draw 0235E0D0 alpha |
| 0x570 | `m458` | `mRetireTimer` | wait_act_proc 0235E6C4 sets 90 (tww 0x5A) when the melt starts; fade_out_retire_act_proc 0235E820 counts it down and retires at 0 |
| 0x574 | `m45C` | `mMeltReq` | tg_hitCallback 0235D774: 0, 2 on a fire hit (AT_TYPE_FIRE / FIRE_ARROW / UNK20000), 1 once mScaleXZ drops below 0.1 (tww); wait_act_proc 0235E6C4 starts the melt (sound for 2) when it is not 0 |

## daObjDoguu_c (d_a_obj_doguu.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_doguu.cpp, which keeps the GameCube offset names) and our decomp's use (its local names main / head / body / crystal / variant).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x7E4 | `field_0x6CC` | `mMainModel` | CreateHeap 02334BE8: the VGSMA model (local main) with the vgsm material table; _draw 02335CA4 draws it with the eye-flash mBrk while the head/body pair is hidden (tww: the model mBrk.init / mBrk.entry use); CreateInit 02335548 gives its base matrix to cullMtx |
| 0x7E8 | `field_0x6D0` | `mHeadModel` | CreateHeap 02334BE8: the bdl_vgsh model (local head) that mBckHead animates (tww bck_head); set_mtx 0233528C puts it on the body's head1 joint (tww setBaseTRMtx(field_0x6D4->getAnmMtx(mHead1JntNo))); _draw 02335CA4 entries mBckHead on it |
| 0x7EC | `field_0x6D4` | `mBodyModel` | CreateHeap 02334BE8: the VGSBA model (local body) that mBckBody animates (tww bck_body) and whose joint "head1" gives mHead1JntNo; _draw 02335CA4 entries mBckBody on it |
| 0x7F0 | `field_0x6D8` | `mCrystalModel` | CreateHeap 02334BE8: the bdl_vgsp model (local crystal) that mBckCrystal animates (tww bck_crystal); _draw 02335CA4 draws it while mDrawCrystal; privateCut 02336408 / _execute 023368F0 read the light position from its joint mCrystalJntNo |
| 0xB80 | `field_0x894` | `mVariant` | _create 02335B6C: argument - 1 or the low parameter byte (tww); selects the Din / Farore / Nayru pearl (CreateInit 02335548 mItemNo), the resource indices (CreateHeap 02334BE8), the light colour (setPointLight 0233516C), the finish event (setFinishMyEvent 023368B0) and the particle (privateCut 02336408); our code's local name is variant |
| 0xB88 | `field_0x89C` | `mCrystalJntNo` | CreateHeap 02334BE8 1 (tww); privateCut 02336408 / _execute 023368F0: the joint of mCrystalModel whose animation matrix gives mPos (tww getAnmMtx(it)) |
| 0xB8C | `field_0x8A0` | `mDrawCrystal` | _draw 02335CA4 draws mCrystalModel only while it is set (tww if (it == true) mBckCrystal.entry(..)); privateCut 02336408 sets it on ACT_SETBALL_e; CreateInit 02335548 sets it when the pearl is already placed |
| 0xB8D | `field_0x8A1` | `mDrawHeadBody` | _draw 02335CA4: set draws mHeadModel + mBodyModel, clear draws mMainModel with mBrk (tww); _execute 023368F0 sets it from the demo shape (mShape != 0) and plays mBrk only while it is clear; CreateInit 02335548 1 once all three pearls are placed |
| 0xB8E | `field_0x8A2` | `mEyeFlash` | privateCut 02336408 sets it on ACT_EYEFLASH_e; _execute 023368F0 plays mBrk (sound 0x6A31 at its start) while it is set and mDrawHeadBody is not (tww); CreateInit 02335548 sets it when the pearl is already placed |
| 0xB8F | `field_0x8A3` | `mLightState` | _execute 023368F0 moves mPos to the crystal joint and drives mLightPower while it is not 0 (1: dim target 0.15, 2: bright target 0.8); privateCut 02336408 1 on ACT_SETBALL_e, 2 at SETLIGHT frame 80; CreateInit 02335548 2 when the pearl is shown, else 0 (tww a bool) |
| 0xB98 | `field_0x8AC` | `mState` | CreateInit 02335548 / _execute 023368F0: the State enum the _execute switch runs (STATE_WAIT_PEARL_e .. STATE_DONE_e) (tww switch (it) with 0..14) |
| 0xBA0 | `field_0x8B4` | `mGoalPos` | setGoal 02336194: the goal position handed to the event manager (tww it = pos; dComIfGp_evmng_setGoal(&it)) |
| 0xBB0 | `field_0x8C4` | `mLightTimer` | privateCut 02336408: 0 on ACT_SETLIGHT_e, counted to 200 in that cut; mAllColRatio dims below 160, the light particle starts at 40, mLightState 2 at 80 (tww 125) |
| 0xBEC | `field_0x8FC` | `mLightPower` | setPointLight 0233516C scales the light colour and mPower (300 * it) by it (tww); _execute 023368F0 eases it to a jittered target (cLib_addCalc2); CreateInit 02335548 0 |

## daObj_Canon_c and daObj_Canon_HIO_c (d_a_obj_canon.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_canon.cpp, which keeps the GameCube offset names) and our decomp's use (its locals muzzle / attention / phase / recoil / rise / prediction). Open: daObj_Canon_c field_0x2A0, field_0x2B4, field_0x448 (gaps, never used); daObj_Canon_HIO_c field_0x3A (0x2800) and field_0x48 (0), set by the constructor and never read.

daObj_Canon_c:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B0 | `field_0x294` | `mPathNo` | getArg 02330100: parameter bits 16-23 (tww fopAcM_GetParamBit(param, 16, 0xFF)); createInit 02330304 gets the room path with it (tww dPath_GetRoomPath(it, current.roomNo)); _create 02330524 fails when it is 0xFF |
| 0x3B1 | `field_0x295` | `mDeleteSwNo` | getArg 02330100: parameter bits 24-31; modeDeleteInit 02331558 turns the switch on when it is not 0xFF; createInit 02330304 starts in MODE_DELETE when that switch is already on (tww dComIfGs_onSwitch / isSwitch(it, ..)) |
| 0x3B2 | `field_0x296` | `mAppearSwNo` | getArg 02330100: parameter bits 8-15; createInit 02330304 starts in MODE_SW_WAIT while that switch is off; modeSwWait 02331614 goes to MODE_APPEAR once it is on (tww dComIfGs_isSwitch(it, ..)) |
| 0x3B3 | `field_0x297` | `mScaleArg` | getArg 02330100: parameter bits 0-7; when not 0xFF the actor scale is 1 + it * 0.1 (tww scale.set(1.0 + it * 0.1, ..)) |
| 0x3B4 | `field_0x298` | `mPathRun` | createInit 02330304 gives it the room path (tww setInfDrct(mpPath)); modeWait 02330D3C only aims while the player and the target point are inside it (tww chkInside) |
| 0x3CC | `field_0x2B0` | `mpPath` | createInit 02330304: the room path from mPathNo (tww it = dPath_GetRoomPath(..)), handed to mPathRun |
| 0x3E0 | `field_0x2C4` | `mTargetYaw` | lockonCannon 0233113C: target Y angle to mTargetPos minus shape_angle.y, limited to l_HIO.mYawMin..mYawMax (tww); modeWait 02330D3C 0 outside the path; _execute 02330760 eases mYaw to it |
| 0x3E2 | `field_0x2C6` | `mYaw` | _execute 02330760 eases it to mTargetYaw (tww cLib_addCalcAngleS2(&it, mTargetYaw, 6, 0x300)); _nodeControl 0232FDF0 turns the barrel by -it (Z); attackCannon 02331298 adds it to the shot's Y angle |
| 0x3E4 | `field_0x2C8` | `mTargetPitch` | lockonCannon 0233113C: target X angle to mTargetPos plus a distance lift, limited to l_HIO.mPitchMin..mPitchMax (tww); modeWait 02330D3C 0 outside the path; _execute 02330760 eases mPitch to it |
| 0x3E6 | `field_0x2CA` | `mPitch` | _execute 02330760 eases it to mTargetPitch (tww cLib_addCalcAngleS2(&it, mTargetPitch, 6, 0x300)); _nodeControl 0232FDF0 turns the barrel by -it (X); attackCannon 02331298 subtracts it from the shot's X angle |
| 0x3E8 | `field_0x2CC` | `mTargetPos` | modeWait 02330D3C: the player position moved back along the player angle by the lead distance (tww it = player pos; it.x -= temp * sin ..); lockonCannon 0233113C aims at it |
| 0x55C | `field_0x440` | `mHitTimer` | checkTgHit 02330A98: hits count only while cLib_calcTimer(&it) is 0; 5 after each hit (tww) |
| 0x560 | `field_0x444` | `mModeTimer` | modeWaitInit 02330A88 l_HIO.mWaitTime, modeAttackInit 0233112C l_HIO.mAttackTime; modeWait 02330D3C / modeAttack 023313B4 act when cLib_calcTimer(&it) reaches 0 (tww) |
| 0x56C | `field_0x450` | `mMuzzlePos` | _execute 02330760: the shot joint matrix times the l_HIO muzzle offset (tww cMtx_multVec(.., &it)); attackCannon 02331298 creates the bomb there and starts the fire sound on it; _delete 02330640 deletes that sound |
| 0x578 | `field_0x45C` | `mAttentionPos` | _execute 02330760: the shot joint matrix times (0, 60 * scale.x, 0) (our local attention); setAttention 0233069C puts attention_info.position and eyePos there (tww); setCollision 023306E4 sphere centre |
| 0x584 | `field_0x468` | `mRecoilPhase` | _nodeControl 0232FDF0: advanced by 0x1830 each call; the barrel X shake is mRecoil * 5 * sin(it) (tww; our locals phase / recoil) |
| 0x586 | `field_0x46A` | `mRecoil` | _nodeControl 0232FDF0: the shake amplitude (our local recoil); attackCannon 02331298 0xC8, checkTgHit 02330A98 0x190 on a boomerang hit, modeAppearInit 02331678 0x258, modeDeleteInit 02331558 0; _execute 02330760 eases it to 0 (tww) |
| 0x588 | `field_0x46C` | `mSinkY` | _nodeControl 0232FDF0 translates the barrel by -it (our local rise; tww temp2.y = -it); modeSwWaitInit 023315F8 / modeAppearInit 02331678 120; modeAppear 023316A0 eases it to 0 and ends the appearance below 2 |
| 0x58C | `field_0x470` | `mSmokeCallBack` | setEffect 02331418: the follow callback of the ID_IT_JN_MJTAIHOU_SMOKE01 particle (tww dComIfGp_particle_set(.., &it, ..)); _execute 02330760 removes it when mSmokeTimer runs out; _delete 02330640 removes it |
| 0x5A0 | `field_0x484` | `mSmokeTimer` | setEffect 02331418 0xE6 with the smoke particle; _execute 02330760 removes mSmokeCallBack when cLib_calcTimer(&it) reaches 0 (tww) |
| 0x5A4 | `field_0x488` | `mShipShotCount` | modeWait 02330D3C: +1 each frame the ship's cannon shoots (tww ship->checkShootCannon()), 0 when the player status says swimming / UNK1000000; below 6 it adds 200 * (6 - it) to the lead distance (tww) |

daObj_Canon_HIO_c (l_HIO):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x05 | `field_0x05` | `mForceBreak` | checkTgHit 02330A98: while set, health = 0 and MODE_DELETE (tww if (l_HIO.it) { health = 0; modeProcInit(2); }); daObj_Canon_HIO_c_ct 0233172C 0 |
| 0x08 | `field_0x08` | `mLeadDist` | modeWait 02330D3C: the base distance the aim point mTargetPos is put behind the player (our local prediction; tww s16 temp = l_HIO.it); daObj_Canon_HIO_c_ct 0233172C 150 |
| 0x0C | `field_0x0C` | `mAttentionY` | setAttention 0233069C: attention_info.position.y = mAttentionPos.y + it (tww); daObj_Canon_HIO_c_ct 0233172C sets nothing (0 from the zeroed static) |
| 0x10 | `field_0x10` | `mEyeY` | setAttention 0233069C: eyePos.y = mAttentionPos.y + it (tww); daObj_Canon_HIO_c_ct 0233172C sets nothing |
| 0x14 | `field_0x14` | `mMuzzleX` | _execute 02330760: x of the muzzle offset on the shot joint (our local muzzle; tww temp.x = l_HIO.it); daObj_Canon_HIO_c_ct 0233172C 0 |
| 0x18 | `field_0x18` | `mMuzzleY` | _execute 02330760: y of the muzzle offset, times scale.x (tww temp.y = l_HIO.it * scale.x); daObj_Canon_HIO_c_ct 0233172C 180 |
| 0x1C | `field_0x1C` | `mMuzzleZ` | _execute 02330760: z of the muzzle offset (tww temp.z = l_HIO.it); daObj_Canon_HIO_c_ct 0233172C 0 |
| 0x20 | `field_0x20` | `mShotSpeed` | attackCannon 02331298: the bomb's speedF = it * cos(pitch) and speed.y = -(it * sin(pitch)) (tww); daObj_Canon_HIO_c_ct 0233172C 90 |
| 0x24 | `field_0x24` | `mShotGravity` | attackCannon 02331298: the bomb's gravity (tww bomb->gravity = l_HIO.it); daObj_Canon_HIO_c_ct 0233172C -1.1 |
| 0x28 | `field_0x28` | `mShotNoGravityTime` | attackCannon 02331298: bomb->setNoGravityTime(it) (tww); daObj_Canon_HIO_c_ct 0233172C 0x16 |
| 0x2C | `field_0x2C` | `mPitchLiftDist` | lockonCannon 0233113C: beyond this XZ distance to mTargetPos the excess is added to the pitch, up to mPitchLiftMax (tww if (temp2 > l_HIO.it) temp2 -= l_HIO.it); daObj_Canon_HIO_c_ct 0233172C 5000 |
| 0x30 | `field_0x30` | `mPitchLiftMax` | lockonCannon 0233113C: the cap of that distance lift (tww if (temp3 > l_HIO.it) temp3 = l_HIO.it); daObj_Canon_HIO_c_ct 0233172C 0x3000 |
| 0x32 | `field_0x32` | `mPitchMax` | lockonCannon 0233113C: upper limit of mTargetPitch (tww cLib_minMaxLimit(.., l_HIO.field_0x34, l_HIO.it)); daObj_Canon_HIO_c_ct 0233172C 0x2328 |
| 0x34 | `field_0x34` | `mPitchMin` | lockonCannon 0233113C: lower limit of mTargetPitch (tww cLib_minMaxLimit(.., l_HIO.it, ..)); daObj_Canon_HIO_c_ct 0233172C -0x2328 |
| 0x36 | `field_0x36` | `mYawMax` | lockonCannon 0233113C: upper limit of mTargetYaw (tww cLib_minMaxLimit(.., l_HIO.field_0x38, l_HIO.it)); daObj_Canon_HIO_c_ct 0233172C 0x2328 |
| 0x38 | `field_0x38` | `mYawMin` | lockonCannon 0233113C: lower limit of mTargetYaw (tww cLib_minMaxLimit(.., l_HIO.it, ..)); daObj_Canon_HIO_c_ct 0233172C -0x2328 |
| 0x3C | `field_0x3C` | `mAttackTime` | modeAttackInit 0233112C: mModeTimer = it; modeAttack 023313B4 fires when it runs out (tww); daObj_Canon_HIO_c_ct 0233172C 30 |
| 0x3E | `field_0x3E` | `mWaitTime` | modeWaitInit 02330A88 / modeWait 02330D3C: mModeTimer = it (ten times it while the player status has UNK1000000) (tww); daObj_Canon_HIO_c_ct 0233172C 30 |
| 0x40 | `field_0x40` | `mLeadFarDist` | modeWait 02330D3C: beyond this distance to mTargetPos the lead distance grows by (distance - it) * mLeadFarRate, and the ship-shot bonus applies (tww abs > l_HIO.it); daObj_Canon_HIO_c_ct 0233172C 2000 |
| 0x44 | `field_0x44` | `mLeadFarRate` | modeWait 02330D3C: the factor of that growth (tww (abs - l_HIO.field_0x40) * l_HIO.it); daObj_Canon_HIO_c_ct 0233172C 0.2 |

## daObjFigure_c (d_a_obj_figure.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_figure.cpp, which keeps the GameCube offset names) and our decomp's use. The two message fields are named after tww fopNpc_npc_c (same role: fopMsgM_messageSet id and message number). Open: m732 (only ever set to 0), m740 and the gaps m2C8, m398, m743 (never used).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x988 | `m694` | `mCurrMsgBsPcId` | talk 02340128: the message process id (Figure_messageSetHD result), 0xFFFFFFFF while no message runs; setMessage 0233FF44 0xFFFFFFFF; eventMesSet 0234025C waits for it (tww m694 = fopMsgM_messageSet(..), fpcM_ERROR_PROCESS_ID_e); after tww fopNpc_npc_c::mCurrMsgBsPcId |
| 0x990 | `m69C` | `mCurrMsgNo` | setMessage 0233FF44 sets it; talk 02340128 shows it, getMsg() for param 1, and steps it with next_msgStatus(&it) (tww); after tww fopNpc_npc_c::mCurrMsgNo |
| 0xA00 | `m70C` | `mCamCenter` | eventMesSet 0234025C: (0, mCamHeight + 30, 0) plus current.pos, the camera centre handed to the camera Set (tww dCam_getBody()->Set(it, m718, ..)) |
| 0xA0C | `m718` | `mCamEye` | eventMesSet 0234025C: (0, mCamHeight + 30, 200) turned by current.angle.y + mCamAngleY plus current.pos, the camera eye (tww Set(m70C, it, ..)) |
| 0xA18 | `m724` | `mCamFovy` | eventMesSetInit 0233FF68 50; eventMesSet 0234025C: minus 2 * sub stick Y, limited to 20..60, the camera Set's fovy argument (tww Set(.., .., it, 0)) |
| 0xA1C | `m728` | `mCamHeight` | eventMesSetInit 0233FF68 118; eventMesSet 0234025C: plus 5 * stick Y, limited to 80..150; the camera centre and eye heights are it + 30 (tww) |
| 0xA20 | `m72C` | `mpMsgList` | eventMesSetInit 0233FF68 / getMsg 0233FF54 null; next_msgStatus 023400E0 steps it and returns its next message number, null at the 0 end (tww m72C++; *m72C) |
| 0xA28 | `m734` | `mCamAngleY` | eventMesSet 0234025C: plus 512 * stick X; the camera eye turns by current.angle.y + it; 0 on B (tww); daObjFigure_constructHD 0233F6DC 0 |
| 0xA2A | `m736` | `mFigureAngleY` | setMtx 0233FA44 turns the figure by it (tww mDoMtx_stack_c::YrotM(it)); daObjFigure_constructHD 0233F6DC / eventMesSet 0234025C (B) current.angle.y; tww also turns it while idle (+= l_figure_check_tbl[0].field_0x28), HD does not |
| 0xA2C | `m738` | `mSpinWait` | eventMesSetInit 0233FF68 0; eventMesSet 0234025C: 0 on stick input, counted down while idle (tww: table field_0x32 on input; at 0 tww turns mFigureAngleY) |
| 0xA2E | `m73A` | `mInputDelay` | eventMesSetInit 0233FF68 10; eventMesSet 0234025C counts it down and reads no input (and allows no A/B) until it is 0 (tww) |
| 0xA30 | `m73C` | `mMoveProcIdx` | daObjFigure_constructHD 0233F6DC 0; tww _execute calls (this->*moveProc[it])() (moveProc = { executeNormal }) |
| 0xA33 | `m73F` | `mMesState` | eventMesSetInit 0233FF68 0; eventMesSet 0234025C: 0 looking (A sets 1), 1 starting the talk (2 once mCurrMsgBsPcId is set), 2 talking (0 when the box closes, status 0x12) (tww switch (it)) |
| 0xA35 | `m741` | `mActIdx` | privateCut 02340664: the event action index (tww it = dComIfGp_evmng_getMyActIdx(..), -1 ends the cut, switch (it)); same name as daObjDoguu_c::mActIdx and other actors' cut indices |

## daObj_Ikada_c (d_a_obj_ikada.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_ikada.cpp and include/d/actor/d_a_obj_ikada.h, which names mCranePos and mFrame and has the accessors setStart / setStop / getSvDirection) and our decomp's use. The SvShip view in d_a_npc_sv.h is aligned in its own commit. Open: m0294 / m0298 / m029C / m02A0 (parameter fields getArg stores and nothing reads) and m02CC (written only by daTag_Island_c::demoProcCom_ikada, through the IslandRaft view in d_a_tag_island.h, which keeps the name).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x590 | `m0474` | `mCranePos` | TWW name (tww d_a_obj_ikada.h 0x0474 mCranePos); createInit 023614C8 / setRopePos 023620F0: the translation of the crane joint matrix (tww mDoMtx_stack_c::multVecZero(&mCranePos)) |
| 0x1438 | `m1154` | `mFrame` | TWW name (tww d_a_obj_ikada.h 0x1154 mFrame, accessor getFrame() read by daNpc_Sv_c); HandleRight 02363F30 adds and HandleLeft 02364098 subtracts the handle speed, wrapping at the bck start / end; _execute 02362C54 sets the bck frame from it |
| 0x410 | `m02F4` | `mPrevMode` | _execute 02362C54: mCurMode saved before the switch to MODE_STOP_e (tww it = mCurMode; modeProcInit(9)); modeStop 02364668 returns to it (MODE_CRANE_DOWN_e for 7, MODE_PATH_MOVE_e for 9) and keeps the path move going while it is 8 (tww) |
| 0x55C | `m0440` | `mWpillarTimer` | setRopePos 023620F0: a water pillar (fopKyM_createWpillar at mRopeWaterPos) only while it is 0, then 19 (tww 0x14); counted down each call (tww) |
| 0x560 | `m0444` | `mRopeWaterPos` | setRopePos 023620F0: the rope point at the water height, where the ripple particle (mRippleCallBack) and the water pillar go (tww it.x = ptr->x; it.y = water; ..) |
| 0x5C4 | `m04A8` | `mBombSmokeTimer` | _execute 02362C54 removes mBombSmokeEasterEgg when cLib_calcTimer(&it) reaches 0 (tww); tww checkTgHit sets 230 with that smoke particle (our checkTgHit 02364854 has no such write) |
| 0x624 | `m0508` | `mLightAlpha` | epProc 02361E40 eases it to mLightAlphaTarget (tww cLib_addCalc2(&it, m050C, 1.0, ..)); tww _draw uses it as the alpha of the light sphere (setAlphaModel(TYPE_SPHERE, mLightMtx, it)) |
| 0x628 | `m050C` | `mLightAlphaTarget` | epProc 02361E40: a random 8 .. 12 target, renewed when mEpTimer0 runs out (tww); mLightAlpha eases to it |
| 0xABC | `m07D8` | `mRopeCnt` | incRopeCnt 02363D28 adds the increment (tww it = (s16)(it + arg)); setRopePos 023620F0 / _draw 02363B78 the number of rope points (tww mRopeLine.update(it, ..)); createInit 023614C8 15; the crane modes compare it with l_HIO limits |
| 0xAC0 | `m07DC` | `mRopeVelocity` | setRopePos 023620F0: per rope point velocity (our local velocity) added to and damped on the mRopeLine points (tww pcVar7 = &it[m07D8 - 1]); incRopeCnt 02363D28 / createInit 023614C8 zero the new entries |
| 0x14B4 | `m1158` | `mCraneAngleTarget` | modeCraneTurn 02364274: 0x4000 or -0x4000 from mSvDirection, modeCraneReset 02364354 0; mCraneAngle eases to it and the mode ends within 0x100 (tww cLib_distanceAngleS(m115A, it)) |
| 0x14B6 | `m115A` | `mCraneAngle` | modeCraneTurn 02364274 / modeCraneReset 02364354 ease it to mCraneAngleTarget (tww cLib_addCalcAngleS2(&it, m1158, 10, 0x100)); _nodeControl 0235FE84 turns the crane joint by it plus the shake (tww XrotM(it + m115E * .. * sin(m115C))) |
| 0x14B8 | `m115C` | `mCraneShakePhase` | modeCraneDown 02364148 / modeCraneTurn 02364274 / modeCraneReset 02364354 advance it by 0x1830 (tww); _nodeControl 0235FE84: the sine phase of the crane shake (our local phase) |
| 0x14BA | `m115E` | `mCraneShakeAmpl` | modeCraneDownInit 0236408C / modeCraneTurnInit 0236425C / modeCraneResetInit 0236433C 150; the modes ease it to 0 and act below 20 (tww); _nodeControl 0235FE84 the shake amplitude (tww it * (REG12_S(5) + 5) * sin(m115C)) |
| 0x14C2 | `m1166` | `mPlayerNearby` | tww accessors setStart() { it = 1; } / setStop() { it = 0; }, which daNpc_Sv_c calls while its mbNearPlayer is set / clear (tww d_a_npc_sv.cpp); our SvShip view of this class (d_a_npc_sv.h) already names it mPlayerNearby; _execute 02362C54 switches to MODE_STOP_e while it is set (tww); modeStop 02364668 pulls the rope in while it is set and lets it out / resumes mPrevMode while it is 0 (tww) |
| 0x14C4 | `m1168` | `mSvDirection` | tww accessor getSvDirection() { return it; } (read by daNpc_Sv_c for its look angle); modePathMoveInit 02364460: random 0 or 1 (tww); modeCraneTurn 02364274 turns to 0x4000 when it is 1, else -0x4000 (tww) |
| 0x1640 | `m12E4` | `mHitTimer` | checkTgHit 02364854: hits count only once cLib_calcTimer(&it) is 0, then it is set (60 in ours; tww 5) |

### daObj_Ikada_HIO_c (l_HIO; d_a_obj_ikada.h)

Round 3: the remaining offset-named HIO values (vtable first here, so the WWHD offsets equal the GameCube names). Each is set only by hioConstructor 02364E6C (default in parentheses) and never written elsewhere in WWHD or tww; the reads are in our d_a_obj_ikada.cpp and match zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00005 | `m05` | `mDebugHandleLeft` | daObj_Ikada_c::modePathMove 02364578 / modeStop 02364668: HandleLeft turns the handle animation back each frame while it is set (tww the same) (0) |
| 0x00006 | `m06` | `mDebugHandleRight` | daObj_Ikada_c::modePathMove 02364578 / modeStop 02364668: HandleRight turns the handle animation on each frame while it is set (tww the same) (0) |
| 0x00008 | `m08` | `mDebugNoPathMove` | daObj_Ikada_c::modePathMove 02364578: the path move is skipped while it is set (as without a path) (tww the same) (0) |
| 0x0001C | `m1C` | `mDebugRopeCnt` | daObj_Ikada_c::modeWait 02363E98 (type 4): when not 0, mRopeCnt steps toward it by incRopeCnt (tww the same) (0) |
| 0x0001E | `m1E` | `mCraneDownRopeCnt` | daObj_Ikada_c::modeCraneDown 02364148: the rope is let out (incRopeCnt) until mRopeCnt reaches it, then crane wait (tww the same) (100) |
| 0x00020 | `m20` | `mHandleSpeed` | daObj_Ikada_c::HandleRight 02363F30 / HandleLeft 02364098: the handle animation's frame step (tww the same) (1.5) |
| 0x00024 | `m24` | `mCraneWaitTime` | daObj_Ikada_c::modeCraneUpWaitInit 02364034 / modeCraneDownWaitInit 02364204: mTimer for the crane waits (tww the same) (60) |
| 0x00028 | `m28` | `mBombSmokeHeight` | daObj_Ikada_c::_execute 02362C54: mBombSmokePos.y (tww the same) (100) |
| 0x00094 | `m94` | `mTerry1StopDistanceShip` | daObj_Ikada_c::modePathMoveTerry 023649B8 / modeStopTerry 02364C6C: type 1 stops while the player is closer than it and rides a ship (player status 0x10000, tww daPyStts0_SHIP_RIDE_e) (tww the same) (3000) |
| 0x00098 | `m98` | `mTerry3StopDistance` | daObj_Ikada_c::modePathMoveTerry 023649B8 / modeStopTerry 02364C6C: type 3 stops while the player is closer than it (tww the same) (1000) |
| 0x0009A | `m9A` | `mTerry1StopDistance` | daObj_Ikada_c::modePathMoveTerry 023649B8 / modeStopTerry 02364C6C: type 1 stops while the player is closer than it and not on a ship (tww the same) (1000) |
| 0x0009C | `m9C` | `mSphRadius` | daObj_Ikada_c::setCollision 023647D0: the collision sphere's radius (times scale.x) (tww the same) (300) |
| 0x000A8 | `mA8` | `mVelocityTargetTerry3Near` | daObj_Ikada_c::modePathMoveTerry 023649B8: type 3's mVelocityFwdTarget while the player is closer than mPlayerStopDistance (tww the same) (30) |

Open (no name yet): padding `m09`, `m26`.

## daObj_Warpt_c (d_a_obj_warpt.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_warpt.cpp, which keeps the GameCube offset names; accessor isHuta) and our decomp's use. Open: m298 and m2C5 (stored by getArg, never read), m2BC (only compared with 0xFF to play the riddle sound) and m2C0 (only compared with 0xFF to set the warp bit at creation).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3AC | `m290` | `mCurMode` | modeProc 023B9CB0: stores the new mode and calls its init / run from mode_tbl (tww mode_tbl[it].init / .run); _ride 023B9D5C / _draw 023BB074 test it (MODE_EVENT_WARP, the open mode 2); after tww daObj_Canon_c / daObj_Ikada_c mCurMode |
| 0x3B8 | `m29C` | `mWarpSclsNo` | getArg 023B9F74: parameter bits 12-19 (tww); normalWarp 023BB7AC warps to it once mWarpSwNo is on (tww warp(it), warp sets the next stage by scls number) |
| 0x3BC | `m2A0` | `mSpSclsNo1` | getArg 023B9F74: parameter bits 8-15 for the special pots (tww); spWarp 023BB6A4 warps to it when warp bit 1 is set (tww isWarpBit(1) -> warp(it)) |
| 0x3C0 | `m2A4` | `mSpSclsNo2` | getArg 023B9F74: parameter bits 16-23 for the special pots (tww); spWarp 023BB6A4 warps to it when warp bit 2 is set (tww isWarpBit(2) -> warp(it)) |
| 0x3C4 | `m2A8` | `mSpSclsNo4` | getArg 023B9F74: parameter bits 24-31 for the special pots (tww); spWarp 023BB6A4 warps to it when warp bit 4 is set (tww isWarpBit(4) -> warp(it)) |
| 0x3C8 | `m2AC` | `mLidSwNo` | getArg 023B9F74: switch number (tww uVar6 bits 0-7); openHuta 023BB400 turns it on, mLidClosed is set while it is off (tww dComIfGs_onSwitch / isSwitch(it, ..)); isOtherHuta 023B9884 / modeClose 023BB488 test it |
| 0x3CC | `m2B0` | `mWarpSwNo` | getArg 023B9F74: switch number (tww uVar6 bits 8-15); normalWarp 023BB7AC only warps while it is on (tww dComIfGs_isSwitch(it, ..)) |
| 0x3D0 | `m2B4` | `mPotType` | getArg 023B9F74: parameter bits 0-3 (tww); isSp 023B9864: 2 .. 4 are the special pots (tww it == 2 || 3 || 4); spWarp 023BB6A4 switches on it; createInit 023BA4E0 picks the smoke colours by it (tww m_def_smoke_prm_color[it]) |
| 0x3D4 | `m2B8` | `mEventRegNo` | getArg 023B9F74: parameter bits 4-7 for the special pots (tww); onWarpBit 023B9E94 / isWarpBit 023B9F1C use the event register m_event_reg[it] (tww) |
| 0x3E0 | `m2C4` | `mWarpBit` | getArg 023B9F74: 1, 2 or 4 by mPotType (tww); openHuta 023BB400 sets it in the event register (tww onWarpBit(it)); getArg sets mLidClosed from isWarpBit(it) (tww) |
| 0x3E2 | `m2C6` | `mLidClosed` | tww accessor isHuta() { return it; } (our isHuta 023B985C); getArg 023B9F74: set while the lid switch / warp bit is off (tww); openHuta 023BB400 clears it |
| 0x3F8 | `m2DC` | `mBodyBgW1Mtx` | createBodyHeap 023B9AE8: the matrix mpBodyBgW1 is set up with (tww mpBodyBgW1->Set(.., &it)); setMtx 023BA250 copies the pot matrix into it (tww cMtx_copy(.., it)) |
| 0x42C | `m310` | `mBodyBgW2Mtx` | createBodyHeap 023B9AE8: the matrix mpBodyBgW2 is set up with (tww mpBodyBgW2->Set(.., &it)); setMtx 023BA250 copies the pot matrix into it (tww) |
| 0x460 | `m344` | `mLidBgWMtx` | createHutaHeap 023B9944: the matrix mpLidBgW is set up with (tww mpLidBgW->Set(.., &it)); setMtx 023BA250 copies the lid matrix into it (tww) |
| 0x9AC | `m830` | `mLidPos` | setMtx 023BA250: current.pos raised by l_HIO.mLidOffsetY (tww it = current.pos; it.y += l_HIO.m08), where the lid models are put; setCollision 023BA1BC mCyl2 centre; breakHuta 023BAA28 particles; _draw 023BB074 tev position (tww) |
| 0x9B8 | `m83C` | `mBreakFireTimer` | modeBreakFireInit 023BB57C 60; modeBreakFire 023BB5CC acts when cLib_calcTimer(&it) reaches 0 (tww) |
| 0x9BC | `m840` | `mBgW2RegistTimer` | createInit 023BA4E0 30 or 5, modeEventWarp 023BB820 30 (tww); _execute 023BAD74 counts it down (tww cLib_calcTimer(&it)); setMtx 023BA250 registers mpBodyBgW2 when it is 1 (tww if (it == 1) .. Regist(mpBodyBgW2, this)) |
| 0x9C0 | `m844` | `mOpenSmokeCallBack` | modeOpenInit 023BB2A4: the follow callback of the open smoke particle 0x8161 (tww ID_AK_SN_WARPPOTOPEN00, &it); _execute 023BAD74 recolours its emitter; _delete 023BA8C8 removes it |
| 0x9D4 | `m858` | `mCloseSmokeCallBack` | modeCloseInit 023BB380: the follow callback of the close smoke particle 0x8162 (tww ID_AK_SN_WARPPOTCLOSE00, &it); modeOpenInit 023BB2A4 removes it; _delete 023BA8C8 removes it |
| 0x9E8 | `m86C` | `mSmokePrmColor` | createInit 023BA4E0: m_def_smoke_prm_color[mPotType] (tww); modeOpenInit 023BB2A4 / modeCloseInit 023BB380 pass it as the smoke particle's prim colour (tww particle_set(.., &it, &m870)) |
| 0x9EC | `m870` | `mSmokeEnvColor` | createInit 023BA4E0: m_def_smoke_env_color[mPotType] (tww); modeOpenInit 023BB2A4 / modeCloseInit 023BB380 pass it as the smoke particle's env colour (tww particle_set(.., &m86C, &it)) |

## daObj_Pfall_c and daObj_PfallHIO_c (d_a_obj_pfall.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_pfall.cpp, which keeps the GameCube offset names) and our decomp's use. Open: field_0x290, field_0x294, field_0x30C, field_0x384, field_0x39C (never used) and field_0x444 (the 3D line material, only constructed and destroyed).

daObj_Pfall_c:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x4C4 | `field_0x3A8` | `mCurMode` | mode_wait_init 023830CC MODE_WAIT, mode_event_init 02383D18 MODE_EVENT; mode_proc_call 02383748 calls mode_proc[it] (tww (this->*mode_proc[it])()); after tww daObj_Canon_c / daObj_Ikada_c mCurMode |
| 0x4CC | `field_0x3B0` | `mOpenStep` | cutOpenStart 02383310 0; cutOpenProc 02383404 steps it through the seven target angles of mFloorAngle and ends the cut at 7 (tww) |
| 0x4D0 | `field_0x3B4` | `mFloorAngle` | set_mtx 0238299C turns both floor models by it (tww mDoMtx_stack_c::ZrotM(it)); cutOpenStart 02383310 0 (with the jail-floor-break sound); cutOpenProc 02383404 eases it to the table angles (tww cLib_addCalcAngleS(&it, temp[mOpenStep], ..)) |
| 0x4D4 | `field_0x3B8` | `mWaitTimer` | mode_wait_init 023830CC l_HIO.mWaitTime; mode_wait 02383D24 starts the event once the player stands still and cLib_calcTimer(&it) reaches 0 (tww) |
| 0x4E0 | `field_0x3C4` | `mpModel` | CreateHeap 02382E64: the first floor model (tww it = mDoExt_J3DModel__create(..)); set_mtx 0238299C gives it the mBgMtx placement; _draw 02383C24 draws it; pairs with mpBgW |
| 0x4E8 | `field_0x3CC` | `mBgMtx` | set_mtx 0238299C: copy of the first floor model's matrix (tww cMtx_copy(.., it)); CreateHeap 02382E64 sets mpBgW up with it (tww mpBgW->Set(.., &it)) |
| 0x518 | `field_0x3FC` | `mpModel2` | CreateHeap 02382E64: the second floor model from the same model data (tww); set_mtx 0238299C gives it the mBgMtx2 placement; _draw 02383C24 draws it; pairs with mpBgW2 |
| 0x520 | `field_0x404` | `mBgMtx2` | set_mtx 0238299C: copy of the second floor model's matrix (tww); CreateHeap 02382E64 sets mpBgW2 up with it (tww mpBgW2->Set(.., &it)) |
| 0x554 | `field_0x438` | `mHimoY` | set_mtx 0238299C: the rope model mpHimoModel is moved up by it + l_HIO.mHimoOffsetY (tww transM(0, it + l_HIO.field_0x08, 0)); _execute 023837E4 raises it during the pull animation and lowers it again, 0 otherwise (tww) |
| 0x6E5 | `field_0x481` | `mAnmPrmIdx` | setAnm 02383794 passes it as dLib_bcks_setAnm's pPrmIdx, next to mBckIdx / mOldAnmPrmIdx (tww); CreateInit 023830E4 ANM_WAIT, cutHikuStart 0238336C ANM_HIKU, cutHikuProc 02383510 back to ANM_WAIT (tww) |
| 0x6E7 | `field_0x483` | `mLaughSePlayed` | cutWaitProc 02383378: when the cut timer runs out and it is 0, set 1 and start the rat laugh once (tww fopAcM_monsSeStart(this, JA_SE_CV_NZ_LAUGH, 0)) |

daObj_PfallHIO_c (l_HIO; HD offsets, data first):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x001 | `field_0x05` | `mDebugInit` | _draw 02383C24 runs the one-time debug setup (pfallDebugInit) while it is set; pfallHioConstruct 02383EB0 0 (tww false) |
| 0x002 | `field_0x06` | `mNoWarp` | mode_event 02383DC4: when the NZFALL event ends, set goes back to mode_wait_init, clear warps to the sea stage (tww if (l_HIO.it != 0)); pfallHioConstruct 02383EB0 0 |
| 0x004 | `field_0x08` | `mHimoOffsetY` | set_mtx 0238299C adds it to mHimoY for the rope model height (tww transM(0, field_0x438 + l_HIO.it, 0)); pfallHioConstruct 02383EB0 3 (tww 3.0f) |
| 0x008 | `field_0x0C` | `mWaitTime` | mode_wait_init 023830CC: mWaitTimer = it (tww field_0x3B8 = l_HIO.it); pfallHioConstruct 02383EB0 0 |

## daObjIceisland_c (d_a_obj_iceisland.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_iceisland.cpp, which keeps the GameCube offset names) and our decomp's use. Open: field_0x394 (padding, never used).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x6EA | `field_0x396` | `mWindSeVolume` | _execute 0235F8F0: counted down to 0 while the switch is on and up to 100 while it is off, then passed as the iceberg wind sound's volume (tww fopAcM_seStartCurrent(this, JA_SE_ATM_ICEBERG_WIND, it)); CreateInit 0235F0A0 0 when melted, 100 when frozen (tww) |
| 0x6F0 | `field_0x39C` | `mState` | the State enum (Frozen .. FailureDemo): CreateInit 0235F0A0 and the demo steps 0235F420 / 0235F498 / 0235F5E0 / 0235F658 / 0235F740 set it; _execute 0235F8F0 switches on it (tww switch (it) with 0 .. 7) |

## daObjYLzou_c and ylzou::EffSmokeCB (d_a_obj_YLzou.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_YLzou.cpp and include/d/actor/d_a_obj_YLzou.h, which keep the GameCube offset names) and our decomp's use (its locals triforce_complete / angle / amplitude / requested). mPhs is named after the other actors' request_of_phase_process_class mPhs.

daObjYLzou_c:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3AC | `field_0x290` | `mPhs` | _create 023BE004 / _delete 023BE198: the phase of the "YLzou" resource load / delete (tww dComIfG_resLoad(&it, l_arcname)); after tww daObjDoguu_c::mPhs and the other actors' request_of_phase_process_class mPhs |
| 0x3B4 | `field_0x298` | `mpModel` | create_heap 023BDD20: the statue model from bdl_table[mTriforceComplete] (tww it = model); set_mtx 023BDEE4 / init_mtx 023BDFE4 place it; _create 023BE004 hands its base matrix to the actor; _draw 023BE6B8 draws it (tww) |
| 0x3B8 | `field_0x29C` | `mpBgW` | create_heap 023BDD20: the dBgW from dzb_table[mTriforceComplete] on mpModel's base matrix (tww it = dBgW_NewSet(..)); _create 023BE004 registers it, _execute 023BE5F8 moves it, _delete 023BE198 releases it (tww) |
| 0x3BC | `field_0x2A0` | `mMtx` | set_mtx 023BDEE4: copy of the matrix stack after placing the statue (tww cMtx_copy(mDoMtx_stack_c::get(), it)) |
| 0x3F8 | `field_0x2E0` | `mSwSaveNo` | _create 023BE004: param_get_swSave() (tww; the PARAM_SWSAVE bits); set_start_type 023BDADC / move_ylzou_demo_start_wait_act_proc test that switch when it is not 0xFF (tww fopAcM_isSwitch(this, it)) |
| 0x3FC | `field_0x2E4` | `mEventIdx` | demo_regist_wait_act_proc 023BEA90: the event index of l_demo_name[mDemoIdx] (tww it = dComIfGp_evmng_getEventIdx(..)); the demo act procs order it and test existence / end (tww); -1 when none |
| 0x400 | `field_0x2E8` | `mTriforceComplete` | set_start_type 023BDADC: our local triforce_complete (tww it = triforce_complete); create_heap 023BDD20 / _create 023BE004 index bdl_table / dzb_table / solidHeapSize_table with it (tww) |
| 0x470 | `field_0x358` | `mVibAngle` | vib_proc 023BE548 adds 0x4000 each frame (tww it += l_HIO.m14 / 0x4000); set_mtx 023BDEE4 lifts the statue by mVibAmplitude * sin(it) (tww current.pos.y + field_0x35C * cM_ssin(it)); our local angle |
| 0x474 | `field_0x35C` | `mVibAmplitude` | vib_proc 023BE548: grows by 0.05 to 0.8 in vibration state 1, shrinks to 0.4 / 0 in the others (tww); set_mtx 023BDEE4 the shake height factor (our locals amplitude / height) |
| 0x478 | `field_0x360` | `mSmokeState` | eff_smoke_proc 023BE4B4: the smoke state in effect; when mSmokeReq differs the slip smoke starts (1) or ends, and it takes mSmokeReq; while 1 the smoke positions follow (tww) |
| 0x479 | `field_0x361` | `mSmokeReq` | the requested slip smoke state: the demo act init procs (023BE864, 023BE880, ..) set 1 / 0 (tww); eff_smoke_proc 023BE4B4 applies it (our local requested) |
| 0x47C | `field_0x364` | `mVibState` | vib_proc 023BE548: the vibration state in effect, taken from mVibReq when they differ; selects the mVibAmplitude rule (tww switch (it)) |
| 0x480 | `field_0x368` | `mVibReq` | the requested vibration state: move_ylzou_demo_vib_act_init_proc 023BE800 1, move_ylzou_demo_move_act_init_proc 023BE864 2, later 0 (tww); vib_proc 023BE548 applies it (our local requested) |

ylzou::EffSmokeCB (the slip-smoke callbacks mSmokeCbs[2]):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x20 | `field_0x20` | `mPos` | eff_set_slip_smoke_pos 023BE21C: the smoke position from the matrix stack (tww multVecZero(&mSmokeCbs[i].it)); eff_smoke_slip_start 023BE33C passes it as the ID_AK_JT_ELEMENTSMOKE00 particle position (tww) |
| 0x2C | `field_0x2C` | `mAngle` | eff_set_slip_smoke_pos 023BE21C: (0, current.angle.y, 0) (tww mSmokeCbs[i].it.set(0, current.angle.y, 0)); eff_smoke_slip_start 023BE33C passes it as the particle angle (tww) |

## daBomb_c and daBomb_fuseSmokeEcallBack (d_a_bomb.h)

Old names are the GameCube offsets (zeldaret/tww d_a_bomb.h `field_0x...`, same code in tww d_a_bomb.cpp). The declaration
lists were split one per line first (layout-neutral). mBombCheckFlag and mNoHit take tww's accessor names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x8D4 | `field_0x554` | `mLavaY` | bgCrrPos_lava 020C5980 stores the lava ground height (GroundCross with the lava filter); chk_lava_hit 020C7AF4 is true below it, chk_dead_zone 020C7EB0 tests it with mWaterY / ground; create_init 020C6408 sets -1e9 (tww the same) |
| 0x8D8 | `field_0x558` | `mWaterY` | bgCrrPos_water 020C59EC stores the water surface height (the water BG or the sea wave, -1e9 without); chk_water_sink 020C7A98 (30 below it), water_tention 020C6E44 and eff_water_splash 020C7B24 use it; create_init 020C6408 sets -1e9 (tww the same) |
| 0x8DC | `field_0x55C` | `mWaterYOld` | bgCrrPos_water 020C59EC copies mWaterY into it before the update; water_tention 020C6E44 reads it as the old surface height; create_init 020C6408 sets -1e9 (tww the same) |
| 0x8E0 | `field_0x560` | `mWaterLand` | bgCrrPos_water 020C59EC sets it on the frame the bomb enters water (in water and mbWaterIn still 0); chk_water_land 020C7AEC returns it; create_init 020C6408 clears it (tww the same) |
| 0x8E2 | `field_0x562` | `mWaterIsSea` | bgCrrPos_water 020C59EC sets 1 when mWaterY is the sea wave (daSea_calcWave), 0 for the water BG; eff_water_splash 020C7B24 then passes no water polygon to the splash; create_init 020C6408 clears it (tww the same) |
| 0xA70 | `field_0x6F0` | `mInPlayerBombCnt` | create_init 020C6408 sets 1 for a bomb in STATE_3 (Link's); procExplode_init 020C5EF8 and bombDelete 020C50BC take 1 off Link's mBombCnt while it is set (explode then clears it) (tww the same) |
| 0xA71 | `field_0x6F1` | `mBombCheckFlag` | getBombCheck_Flag 020CB678 returns it, setBombCheck_Flag 020CB6A8 sets 1 (tww accessor names; d_a_am / d_a_nz skip a bomb whose flag is set) |
| 0xA73 | `field_0x6F3` | `mNoHit` | setBombNoHit 020CB710 sets 1 (tww accessor name); posMoveF 020C6EB4 skips water_tention and the ground-slope handling while it is 1 |
| 0xA74 | `field_0x6F4` | `mFuseLit` | waitState_bomtyu 020C713C sets 1 when the bomb flower bomb is lit (mBombFire); execute 020C4D84 plays the fuse sound 0x6100 every frame while it is 1 (or ATTR_STATE_10); create_init 020C6408 clears it (tww the same) |
| 0xA7E | `field_0x6FE` | `mExplodeCcTimer` | procExplode_init 020C5EF8 sets 2; procExplode 020C74A0 registers the explosion sphere (mSph at current.pos) while it counts down, then waits for mExplodeLightStep (tww the same) |
| 0xAF8 | `field_0x774` | `mExplodeLightStep` | procExplode_init 020C5EF8 clears it; procExplode 020C74A0: 0 brightens mExplodeLight to 1, 1 fades it to 0, then the bomb deletes itself (mDeleteReq) once it is above 1 (tww the same) |
| 0xAFC | `field_0x778` | `mExplodeLight` | procExplode 020C74A0 eases it per mExplodeLightStep and scales the explosion's actor / BG ambient and diffuse add colours (dKy_*addcol*_set) by it; procExplode_init 020C5EF8 clears it (tww the same) |
| 0xB00 | `field_0x77C` | `mFireEffectSet` | execute 020C4D84 makes the fire effect (makeFireEffect) once in STATE_4 and sets it; create_init 020C6408 clears it (tww the same) |
| 0xB01 | `field_0x77D` | `mWaterExplodeEffDone` | makeWaterEffect 020C7CC0 runs once (water pillar, sound, the water explosion sphere) and sets it; draw 020C4660 hides the bomb in STATE_4 once it is set; create_init 020C6408 clears it (tww the same) |
| 0xB02 | `field_0x77E` | `mFuseEffectSet` | setFuseEffect 020C584C starts the fuse sparks / smoke once and sets it; create_init 020C6408 clears it (tww the same) |
| 0xB03 | `field_0x77F` | `mCannonFlySeOn` | se_cannon_fly_set 020C70DC sets it; se_cannon_fly_stop 020C5078 stops the flying sound only while it is set and clears it; create_init 020C6408 clears it (tww the same) |
| 0xB04 | `field_0x780` | `mThrown` | procCarry 020C76E0 sets it when the bomb leaves the hand with speedF > 0; bound 020C71FC clears it on a ground bounce slower than 19.5; while it is set a bomb nut explodes on a CO hit (checkExplodeCc_nut 020C789C) or a wall / ground hit (checkExplodeBg_nut 020C7DC4) (tww the same) |
| 0xB05 | `field_0x781` | `mDeleteReq` | checkExplodeBg_norm 020C7C34 / _nut 020C7DC4 / _cannon 020C7EEC (sinking, the cannon bomb also in the dead zone) and procExplode 020C74A0 (end) set it; execute 020C4D84 deletes the actor while it is set; procWait 020C7358 returns before bound then (tww the same) |
| 0xB06 | `field_0x782` | `mExplodeCcSet` | set when the explosion sphere is first registered (procExplode_init 020C5EF8, procExplode 020C74A0, makeWaterEffect 020C7CC0); execute 020C4D84 counts mExplodeFrame up while it is set (tww the same) |
| 0xB08 | `field_0x784` | `mExplodeFrame` | execute 020C4D84 adds 1 per frame while mExplodeCcSet; get_explode_instant 020CB98C is true at 1; create_init 020C6408 clears it (tww the same) |
| 0xB4C | `field_0x7C8` | `mDrawDelay` | create_init 020C6408 sets 2 for a bomb in STATE_3, else 0; draw 020C4660 draws nothing while it is above 0; set_real_shadow_flag 020C506C counts it down and keeps the real shadow at 1 (tww the same) |

Fuse smoke callback (offsets within daBomb_fuseSmokeEcallBack):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x4 | `field_0x04` | `mPrevLife` | daBomb_fuseSmokeEcallBack::executeAfter 020C6860 stores this frame's particle life in it; the next frame steps the in-between particles' life from it to the new one; setFuseEffect 020C584C sets 20 (tww set() the same) |
| 0xC | `field_0x0C` | `mpOldPos` | setFuseEffect 020C584C points it at daBomb_c::mFusePos2 (last frame's fuse point); executeAfter 020C6860 fills the smoke trail from mpPos back to it (distance, spline end) (tww set() the same) |
| 0x10 | `field_0x10` | `mpOldOldPos` | setFuseEffect 020C584C points it at daBomb_c::mFusePos3 (the fuse point two frames back); executeAfter 020C6860 takes the spline tangent at mpOldPos from it (tww set() the same) |

Open: `field_0x6F5` / `6F6` / `6F7`, `field_0x702`, `field_0x794[0x30]` (no use in WWHD or tww); BombWindInfluence
`field_0x20` (view of the shared WIND_INFLUENCE, unnamed there too; procExplode_init only clears it).

## daTitle_proc_c, daTitle_Attr_c and daTitle_c (d_a_title.h)

Old names are the GameCube names (zeldaret/tww d_a_title.h `mXXX`; tww's L_attr Attr_c is `field_0x00` .. `field_0x34`,
its values are quoted in the evidence; HD keeps them in a 0x3C attribute object with a vtable, so the HD offset is +4).
HD has no kirari animation: the kirari timer / frame are named from tww's calc_2d_alpha, the pane pointers from tww's
proc_init3D.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x4 | `field_0x00` | `mShipPosX` | set_mtx 024B83F4 places the ship model at x = it + mShipOffsetX (tww L_attr 0.0) |
| 0x08 | `field_0x04` | `mShipPosY` | set_mtx 024B83F4 places the ship model at y = it (tww L_attr 0.0) |
| 0x0C | `field_0x08` | `mShipScale` | set_mtx 024B83F4 sets the ship model's base scale to it (tww L_attr 0.9) |
| 0x10 | `field_0x0C` | `mShipMoveRate` | calc_2d_alpha 024B7CFC: mShipOffsetX = mShipMoveFrame^2 * it (sign by direction); daTitle_proc_ct 024B89D8 starts at 2500 * it (tww L_attr 0.1) |
| 0x14 | `field_0x10` | `mSubtitlePosX` | set_mtx 024B83F4 places the subtitle and kirari models at x = it (tww L_attr -57) |
| 0x18 | `field_0x14` | `mSubtitlePosY` | set_mtx 024B83F4 places the subtitle and kirari models at y = it (tww L_attr -19 JPN / -3) |
| 0x1C | `field_0x18` | `mSubtitleScaleX` | set_mtx 024B83F4 sets the subtitle and kirari models' x scale to it (tww L_attr 1.0) |
| 0x20 | `field_0x1C` | `mSubtitleScaleY` | set_mtx 024B83F4 sets the subtitle and kirari models' y scale to it (tww L_attr 1.0) |
| 0x24 | `field_0x20` | `mKirariIntervalRnd` | daTitle_proc_ct 024B89D8 sets mKirariTimer to rnd(it) + mKirariIntervalBase + 130; tww calc_2d_alpha reloads mKirariTimer the same way after each kirari (HD has no kirari animation) (tww L_attr 120) |
| 0x28 | `field_0x24` | `mKirariIntervalBase` | daTitle_proc_ct 024B89D8: base of mKirariTimer (rnd(mKirariIntervalRnd) + it + 130); tww calc_2d_alpha the same (tww L_attr 10) |
| 0x2C | `field_0x28` | `mWindIntervalRnd` | calc_2d_alpha 024B7CFC and daTitle_proc_ct 024B89D8 set mWindTimer to rnd(it) + mWindIntervalBase (the layout's wind interval wins when set) (tww L_attr 120) |
| 0x30 | `field_0x2C` | `mWindIntervalBase` | calc_2d_alpha 024B7CFC and daTitle_proc_ct 024B89D8: base of mWindTimer (rnd(mWindIntervalRnd) + it) (tww L_attr 10) |
| 0x34 | `field_0x30` | `mSmokeOffsetX` | calc_2d_alpha 024B7CFC adds it to the x of the TITLESMOKE00 emitter (the layout smoke position +- mShipOffsetX) (tww L_attr 85 / 78 JPN) |
| 0x38 | `field_0x34` | `mSmokeOffsetY` | calc_2d_alpha 024B7CFC adds it to the y of the TITLESMOKE00 emitter (tww L_attr 5 / 8 JPN) |
| 0x048 | `m00C` | `mKiraPos` | calc_2d_alpha 024B7CFC sets it to the layout kira position at the kira start frame, starts the TITLEKIRAKIRA00 emitter mpEmitter2 there and moves it right by 320 / (end - start) per frame until the kira end (tww the same) |
| 0x054 | `m018` | `mFrameCount` | calc_2d_alpha 024B7CFC adds 1 per frame; the intro ends at 200 (ENTER_WAIT), the layout intro starts at 30, the kira effect runs between the layout's kira start and end frames; daTitle_proc_ct 024B89D8 clears it (tww the same) |
| 0x058 | `m01C` | `mStartDelay` | daTitle_proc_ct 024B89D8 sets 120; proc_execute 024B8670 counts it down (JA_SE_TITLE_WIND at 0) and runs calc_2d_alpha only once it is 0; a button press in the intro clears it (tww the same) |
| 0x05C | `m020` | `mWindTimer` | calc_2d_alpha 024B7CFC counts it down and at 0 starts a TITLEWIND00 particle and reloads it (rnd(mWindIntervalRnd) + mWindIntervalBase or the layout's interval); daTitle_proc_ct 024B89D8 sets the first value (tww the same) |
| 0x060 | `m024` | `mKirariTimer` | daTitle_proc_ct 024B89D8 sets rnd(mKirariIntervalRnd) + mKirariIntervalBase + 130; tww calc_2d_alpha counts it down and at 0 plays the 20-frame kirari animation (mKirariFrame), then reloads it (HD has no kirari animation, nothing else uses it) |
| 0x068 | `m02C` | `mKirariFrame` | daTitle_proc_ct 024B89D8 clears it; tww calc_2d_alpha steps it 1..20 as the mBtkKirari frame while mKirariTimer is 0, then clears it (HD has no kirari animation) |
| 0x274 | `m094` | `mShipOffsetX` | calc_2d_alpha 024B7CFC sets it to mShipMoveFrame^2 * mShipMoveRate (negated once the ship leaves); set_mtx 024B83F4 adds it to the ship x, calc_2d_alpha moves the smoke emitter with it; daTitle_proc_ct 024B89D8 starts at 2500 * mShipMoveRate (tww the same, sign flipped) |
| 0x278 | `m098` | `mShipMoveFrame` | daTitle_proc_ct 024B89D8 sets -50; calc_2d_alpha 024B7CFC counts it up to 0 in the intro (the ship sails in), proc_execute 024B8670 sets 0 on a button press and counts it up while leaving; it drives mShipOffsetX and the ship bpk frame (100 +- 2 * it) (tww the same) |
| 0x280 | `m0A0` | `mScreenPanes` | tww proc_init3D: the J2DPane pointers m_Screen->search('zeld' / 'zelj' / 'pres' / 'nint' / 'eft1' / 'eft2'), passed to fopMsgM_setPaneData for pane[i] (HD: the 2D logo is a separate layout; no use) |
| 0x3B8 | `m29C` | `mSceneChangeReq` | daTitle_Execute 024B87A4 requests the scene change (to the name scene in ENTER mode 3, else back to the title scene) only while it is 0 and then sets it; daTitle_Create 024B8F44 clears it (tww the same) |

Open: `m028` (calc_2d_alpha counts it 0..100, nothing reads it in WWHD or tww), `m090` (constructor only), `m034`
(padding).

## daTag_MdCb_c (Medli / Makar event tag; d_a_tag_md_cb.h)

Old names are the GameCube offsets (zeldaret/tww d_a_tag_md_cb.h `mXXX`, same code). The member lists were split one
per line first (layout-neutral).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3AC | `m290` | `mActionFn` | setAction 024AEFA0 stores the action (member-function pointer with delta / index) and calls it; action 024AF0E0 calls it each frame unless its index is 0 (tww the same) |
| 0x3B4 | `m29C` | `mParamMsgNo` | init 024AF468 sets it to the low 16 parameter bits; initialMsgSetEvent 024AF844 takes it as the message number when the event gives none (tww the same) |
| 0x3B8 | `m2A0` | `mSwitchStart` | init 024AF468 sets it to parameter bits 16..23; checkCondition 024AEA28 tests the switches mSwitchStart .. + mSwitchCount - 1 (tww the same) |
| 0x3BC | `m2A4` | `mSwitchCount` | init 024AF468 sets it to parameter bits 24..31; checkCondition 024AEA28 loops over that many switches from mSwitchStart (tww the same) |
| 0x3C0 | `m2A8` | `mMsgNo` | initialMsgSetEvent 024AF844 sets the message number (event substance or mParamMsgNo) and picks the sound by it; talk_init 024AF974 / talk 024AFB68 set it as the message, next_msgStatus 024AF9E4 steps it (tww the same) |
| 0x3C8 | `m2B0` | `mSwitchNo` | init 024AF468 sets it to the low byte of current.angle.x; checkCondition 024AEA28 tests (and clears) that switch per tag kind (tww the same) |
| 0x3CA | `m2B2` | `mTimer` | checkTimer 024AE9FC counts it down (cLib_calcTimer) and is true at 0; init 024AF468 sets 0 / 60 / l_HIO m0A by kind, next_msgStatus 024AF9E4 l_HIO m08 (tww the same) |
| 0x3CC | `m2B4` | `mOrderEventIdx` | messageAction 024AFD80 sets the index (0 .. 6) of the event to order by the partner (Medli / Makar) and its carry state; eventOrder 024AF2CC orders mEventIdxTbl[it] while it is not -1; eventProc 024AE748 / init 024AF468 set -1 (tww the same) |
| 0x3CD | `m2B5` | `mEventIdx` | eventOrder 024AF2CC copies mOrderEventIdx into it when it orders; eventProc 024AE748 ends the event mEventIdxTbl[it], talk 024AFB68 picks the partner animation by it, eventEnd 024AE6FC sets -1 (tww the same) |
| 0x3CE | `m2B6` | `mActionPhase` | setAction 024AEFA0 sets -1 for the old action's last call and 0 for the new one; warpAction 024AFD10 / messageAction 024AFD80 turn 0 into 1 on their first call and run only after that (tww the same) |
| 0x3D1 | `m2B9` | `mAreaIn` | checkAreaIn 024AE968 sets 1 while the partner is inside the tag's cylinder; execute 024AF360 copies it to m2B8 and clears it each frame (tww the same) |
| 0x3D2 | `m2BA` | `mSwitchSeen` | checkCondition 024AEA28 sets 1 once a switch of the mSwitchStart range is on; then entering the area starts mTimer (checkTimer); init 024AF468 clears it (tww the same) |
| 0x3D4 | `m2BC` | `mEventIdxTbl` | init 024AF468 and execute 024AF360 fill [i] with getEventIdx(event_name_tbl[i]) for the 7 events; eventOrder 024AF2CC orders [mOrderEventIdx], eventProc 024AE748 checks [mEventIdx] for the end (tww the same) |

Open: `m2AC` (eventProc sets bit 0, eventEnd clears it, nothing reads it), `m2B7` (init clears it only), `m2B8`
(execute copies mAreaIn into it, never read), `m2CA` / `m2CC` / `m2CE` / `m2D0` / `m2D8` (setAction clears them only;
tww the same), `m2BB` / `m2D2` (padding).

### HioPrm (l_HIO.mPrm; d_a_tag_md_cb.h)

Round 3: the tag's HIO values (tww daTag_MdCb_HIO_c::hio_prm_c). daTag_MdCb_HIO_ctor 024AFF08 fills them by a word copy from init_data (default in parentheses); no field is written elsewhere in WWHD or tww. The reads are in our d_a_tag_md_cb.cpp and match zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mAreaHeight` | daTag_MdCb_c::execute 024AF360: scale.y for kinds 0 / 1, the height of the tag area (the area check counts Link below scale.y) (tww execute the same) (200) |
| 0x04 | `m04` | `mAreaRadius` | daTag_MdCb_c::execute 024AF360: scale.x for kinds 0 / 1, the radius of the tag area (tww execute the same) (200) |
| 0x08 | `m08` | `mNextMsgTime` | daTag_MdCb_c::next_msgStatus 024AF9E4: mTimer after messages 0x19F0 / 0x19F1 / 0x19F2 / 0x19F4 (tww the same) (900) |
| 0x0A | `m0A` | `mInitTime` | daTag_MdCb_c::init 024AF468: mTimer at the start for every kind but 3 (tww the same) (150) |

Open (no name yet): `m0C`..`m12` (filled from init_data only, read nowhere in WWHD or tww); `padding`.

## daMachine_c (d_a_machine.h)

Old names are the GameCube offsets (zeldaret/tww d_a_machine.h `field_0x...`, same code in tww d_a_machine.cpp).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x520 | `field_0x404` | `mBodySph` | CreateInit 021B7600 sets the three spheres up (l_sph_src_col); set_body 021B7F84 places them along the body (l_body0 / 1 / 2 in front of current.pos, turned by current.angle) and registers them; [0] also gets At and l_cullRadius (tww the same) |
| 0x9D0 | `field_0x84b` | `mAcch` | CreateInit 021B7600 sets it up (Set with current.pos / old.pos / mAcchCir / speed); constructed in _create 021B7844, destroyed in the destructor 021B8268 (tww the same) |
| 0xB94 | `field_0xa78` | `mAcchCir` | CreateInit 021B7600 sets its wall radius (l_wallRadius) and passes it to mAcch (tww the same) |
| 0xBD4 | `field_0xab8` | `mSearchCps` | set_cube 021B7D8C sets the capsule from l_searchNear to l_cullFront in front of the machine (radius l_searchRadius); attack 021B7E48 starts the attack when mPlayerSph crosses it (tww the same) |
| 0xD0C | `field_0xbf0` | `mPlayerSph` | attack 021B7E48 sets it at the player (+ l_playerHeight, radius l_playerRadius) and tests it against mSearchCps (tww the same) |
| 0xD20 | `field_0xc04` | `mBck` | CreateHeap 021B7364 sets up the machine's bck; attack 021B7E48 restarts and plays it (state 1 / 2); set_at 021B8080 sets the attack sphere only between two of its frames; _draw 021B7A70 enters it (tww the same) |
| 0xDAC | `field_0xc14` | `mPathId` | CreateInit 021B7600 sets the low parameter byte (255 = no path, also when the room has no such path); path_move 021B7D14 and set_next_pnt 021B7C24 do nothing at 255 (tww the same) |
| 0xDAD | `field_0xc15` | `mPathPntNo` | set_next_pnt 021B7C24 steps it by mPathDir and turns at the path ends; the next target mNextPos is that point; CreateInit 021B7600 sets 1 (tww the same) |
| 0xDAE | `field_0xc16` | `mPathDir` | set_next_pnt 021B7C24: +1 / -1, reversed at the path ends; CreateInit 021B7600 sets 1 (tww the same) |
| 0xDB0 | `field_0xc18` | `mpPath` | CreateInit 021B7600 stores the room path mPathId; set_next_pnt 021B7C24 reads the point count and points from it (tww the same) |
| 0xDB4 | `field_0xc1c` | `mNextPos` | CreateInit 021B7600 / set_next_pnt 021B7C24 set it to path point mPathPntNo; path_move 021B7D14 moves current.pos toward it at speedF (tww the same) |
| 0xDD4 | `field_0xc3c` | `mJoint3Mtx` | nodeCallBack 021B7494 copies the animated "joint3" matrix into it; set_at 021B8080 puts the attack sphere mSph at its translation (tww the same) |
| 0xE04 | `field_0xc6c` | `mAtPos` | set_at 021B8080 copies the translation of mJoint3Mtx into it and centres the attack sphere mSph on it (tww the same) |
| 0xE10 | `field_0xc78` | `mAttackState` | attack 021B7E48: 0 waits for the player in mSearchCps, 1 starts the attack animation (JAMA_MECHA_OUT), 2 plays it and returns to 0 at its end (tww the same) |

Open: `field_0x788` (a sphere only set up, constructed and destroyed), `field_0xc28` (set_next_pnt copies mNextPos into
it, nothing reads it in WWHD or tww), `field_0xc34` / `field_0xC79` (padding).

## daStandItem_c (shop stand item; d_a_dai_item.h)

Old names are the GameCube offsets (zeldaret/tww d_a_dai_item.h `mXXX`, same code). The lists holding offset-named fields
were split one per line first (layout-neutral).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x7AC | `m690` | `mpWaitEmitter` | actionFobj09 02118390 (Fountain Idol): before the animation has played (mbBckDidPlay 0) and while the player is within 500 it keeps particle 0x82B3 on the tuboko_head joint matrix (m660), else invalidates it; _delete 021177AC invalidates it (tww the same) |
| 0x7B0 | `m694` | `mpHeadEmitter` | actionFobj09 02118390 (Fountain Idol): once the animation has played it starts particle 0x82B4 and keeps it on the tuboko_head joint matrix (m660); _delete 021177AC invalidates it (tww the same) |
| 0x7B4 | `m698` | `mpBaseEmitter` | actionFobj09 02118390 (Fountain Idol): once the animation has played it starts particle 0x82B5 (immortal) and keeps it on the tuboko_base joint matrix (m630); _delete 021177AC invalidates it (tww the same) |
| 0x7BE | `m6A2` | `mClothAngleY` | CreateInit 02117168 sets it to the wind angle (atan2 of mClothWind); set_mtx 02116EA8 eases it toward the angle of mClothWind and turns the cloth matrix by it - current.angle.y (tww the same) |
| 0x7C0 | `m6A4` | `mClothWind` | CreateInit 02117168 copies the wind vector; set_mtx 02116EA8 takes a stronger wind at once and eases toward a weaker one (cLib_addCalcPos2); its angle is mClothAngleY's target (tww the same) |
| 0x7CD | `m6B1` | `mbBckDidPlayOld` | actionFobj09 02118390 copies mbBckDidPlay into it at the end; on the frame the animation has played and it is still 0 the head / base emitters are started (tww the same) |
| 0x7CE | `m6B2` | `mSpinSpeed` | actionFobj06 02117F20 (pinwheel) sets it to 1536 * mSpinPower and adds it to the spin angle m6B4 (tww the same) |
| 0x7E0 | `m6C4` | `mSpinPower` | actionFobj06 02117F20 (pinwheel) adds the horizontal wind strength (at most 4), eases it to 0 by the wind power and sets mSpinSpeed from it (tww the same) |

Named in round 3 with the view ComStatic::StandItem (d_com_static.h): `m630` / `m660` / `m6B4` ->
daiItemNodeCallBack 02526580 copies the tuboko_base / tuboko_head joint matrices into `m630` / `m660` (the Fountain Idol
emitters' matrices) and turns the pinwheel's top joint by `m6B4` (actionFobj06 adds mSpinSpeed to it); so
`mBaseJointMtx` / `mHeadJointMtx` / `mSpinAngle` in both. Open: `m6BC` (CreateInit stores the wind power, nothing reads
it in WWHD or tww), padding.

## daObjTpost_c, daObjTpost_HIO_c and the letter data (mailbox; d_a_obj_toripost.h)

Old names are the GameCube offsets (zeldaret/tww d_a_obj_toripost.h `field_0x...`, same code). The member lists were split one per line first. HIO and letter-data offsets are within their struct.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x9F4 | `field_0x8DC` | `mXYEndTimer` | modeTalkXYInit 023A4E34 loads l_HIO mXYEndWaitTime; modeTalkXY 023A4E8C ends the item-show talk (back to MODE_WAIT) only once it has counted down and the animation has stopped (tww the same) |
| 0x9F8 | `field_0x8E0` | `mPresentTimer` | modeTalkXYInit 023A4E34 loads l_HIO mPresentWaitTime; modeTalkXY 023A4E8C cancels the present and plays animation 2 once the present has ended and it has counted down (tww the same) |
| 0x9FC | `field_0x8E4` | `mPutOutDemoTimer` | modeTalkXY 023A4E8C sets 10 when the put-out animation starts (mPutOutStart) and at 0 puts the player into demo mode 24, then -1 (tww the same) |
| 0xA01 | `field_0x8E9` | `mReceiveReq` | next_msgStatus 023A55BC sets 1 when the letter is to be received (no postage due, or paid); modeTalk 023A4D9C then goes to MODE_RECEIVE instead of MODE_WAIT and clears it (tww the same) |
| 0xA02 | `field_0x8EA` | `mPutOutStart` | getMsgXY 023A52B8 and next_msgStatus 023A55BC set it with the POST_PUTOUT animation; modeTalkXY 023A4E8C clears it and starts mPutOutDemoTimer (tww the same) |
| 0xA03 | `field_0x8EB` | `mNextLetterMsg` | modeReceiveDemo 023A50CC sets it when more readable letters wait; getMsgNormal 023A5500 then opens with message 0xCF7 for the next letter (count mLetterOrdinal) and clears it (tww the same) |
| 0xA08 | `field_0x8F0` | `mLetterOrdinal` | the ordinal of the letter being handed out (its low half is mLetterOrdinalLow, the message count number): createInit 023A418C / modeWaitInit 023A4C68 set 1, modeReceiveDemo 023A50CC and next_msgStatus 023A55BC add 1 per letter (tww dComIfGp_setMessageCountNumber(it)) |
| 0xA0C | `field_0x8F4` | `mTalkReq` | checkOrder 023A4548 sets it when the normal talk event starts; modeWait 023A4CD8 goes to MODE_TALK while it is set; modeTalk 023A4D9C clears it at the end (tww the same) |
| 0xA0D | `field_0x8F5` | `mTalkXYReq` | checkOrder 023A4548 sets it when the item-show (XY) talk event starts; modeWait 023A4CD8 goes to MODE_TALK_XY; modeTalkXY 023A4E8C clears it at the end (tww the same) |
| 0x7 | `field_0x07` | `mForcePay` | next_msgStatus 023A55BC asks for postage (message 0xCF3) for every letter while it is set, else only for a letter with mPayOnReceive; createHIO 023A5AB4 sets 0 (tww the same) |
| 0x14 | `field_0x14` | `mXYEndWaitTime` | modeTalkXYInit 023A4E34 loads it into mXYEndTimer; createHIO 023A5AB4 sets 30 (tww the same) |
| 0x16 | `field_0x16` | `mPresentWaitTime` | modeTalkXYInit 023A4E34 loads it into mPresentTimer; createHIO 023A5AB4 sets 30 (tww the same) |
| 0x0 | `field_0x00` | `mPayOnReceive` | next_msgStatus 023A55BC: a letter with it set asks for postage on delivery (message 0xCF3 -> 0xCF4 / 0xCF8, 10 or 201 rupees) before it is received (tww m_letter[i].field_0x00 the same) |


Open: HIO `field_0x06` (createHIO only; no read in WWHD or tww).

## daAleaf_c (acorn leaf; d_a_acorn_leaf.h)

Old names are tww's `unk_XXX` (zeldaret/tww d_a_acorn_leaf.h, same code).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3AC | `unk_290` | `mPhs` | _create 02047948 loads the m_arcname resource into it (dComIfG_resLoad); daAleaf_Delete 02047AD4 releases it (tww the same) |
| 0x3B4 | `unk_298` | `mpModel` | CreateHeap 020475B4 creates the leaf model; set_mtx 020476D0 sets its scale / matrix, _draw 02047B04 draws it (tww the same) |
| 0x3B8 | `unk_29C` | `mStts` | CreateInit 02047878 inits it (255, 255) and gives it to mCyl; constructed in the constructor, destroyed in daAleaf_dtor 02047FBC (tww the same) |
| 0x3F4 | `unk_2D8` | `mCyl` | CreateInit 02047878 sets it from l_cyl_src with mStts; constructed / destroyed with the actor (tww the same) |
| 0x524 | `unk_408` | `mBck` | CreateHeap 020475B4 sets up the leaf's bck (EMode_RESET); _execute 02047C00 restarts and plays it while mBckPlay, _draw 02047B04 enters it (tww the same) |
| 0x5B0 | `unk_418` | `mBckPlay` | _execute 02047C00 sets it (and the bck rate 1) when the leaf is disturbed (acorn appears / falls off, the player steps close, a dropped object lands near) and clears it when the bck play ends (tww the same) |
| 0x5B1 | `unk_419` | `mAcornStuckOld` | _execute 02047C00 stores whether the acorn (Tsubo child mAcornId) still sticks (prm_get_stick); when it was stuck last frame and is not now, the leaf sways (tww the same) |
| 0x5B4 | `unk_41C` | `mAcornWaitCount` | _execute 02047C00 counts it up while the acorn exists and at 69 plays the sway once (then 70); create_acorn_sub 020477B0 clears it (tww the same) |
| 0x5B8 | `unk_420` | `mRespawnTimer` | create_acorn 02047B8C creates a new acorn only while it is 0 and the old one is gone, then sets 60 and counts it down (tww the same) |
| 0x5BC | `unk_424` | `mAcornId` | create_acorn_sub 020477B0 stores the process id of the acorn (a Tsubo child); create_acorn 02047B8C / _execute 02047C00 search it; CreateInit 02047878 sets the error id (tww the same) |
| 0x5C0 | `unk_428` | `mGrabIdOld` | _execute 02047C00 stores the player's grab actor id each frame; when the player has just let go (no grab now) an object dropped within 70 sways the leaf; CreateInit 02047878 sets the error id (tww the same) |
| 0x5C4 | `unk_42C` | `mPlayerDistOld` | _execute 02047C00 stores the XZ distance to the player; a step closer by more than 3 within 50 (and at the same height) sways the leaf (tww the same) |


Open: `unk_41B` (create_acorn_sub clears it only; no read in WWHD or tww), `unk_41A` (padding).

## daSTBox_c and daSTBox_shadowEcallBack_c (salvage treasure box; d_a_salvage_tbox.h)

Old names are the GameCube offsets (zeldaret/tww d_a_salvage_tbox.h, same code). The lists holding offset-named fields were split one per line first.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B8 | `field_0x29C` | `mpDropEmitter` | CreateInit 02467F24 starts the water-drop emitters (STBOX_WATER_DROP; [2] only for box types 1 / 2), setupEmitter places them, _execute 02468550 updates them and clearEmitters invalidates and clears them (HD emitter pointers; tww the same three) |
| 0x440 | `field_0x324` | `mShadowPos` | CreateInit 02467F24 sets it to the ship's crane top at the water height (or the crane point) and starts the STBOX_SALVAGE_SHADOW particle there, copying it to mShadowCallback.mPos (tww the same) |
| 0x44C | `field_0x330` | `mItemNo` | CreateInit 02467F24 sets the low parameter byte (1 .. 4 become 5); actWait 0246933C creates that item from the box (stboxCreateItem) for box types 1 / 2 (tww the same) |
| 0x44D | `field_0x331` | `mBoxType` | _create 024682CC sets parameter bits 8..11; CreateHeap 02467D9C picks the model by it, actWait 0246933C / actWait02 02469554 the crane offset (crane_offset[it]), types 1 / 2 carry an item, _delete 024683D8 tests 2 (tww the same) |
| 0x450 | `field_0x334` | `mSplashDone` | actDrop 024695C4 plays the splash sound and water pillar once when the box falls below the water and sets it; CreateInit 02467F24 clears it (tww the same) |
| 0x451 | `field_0x335` | `mRippleSet` | actWait 0246933C starts the STBOX_RIPPLE particle once the box rises to the water surface and sets it (tww the same) |
| 0x452 | `field_0x336` | `mBgmStarted` | actWait 0246933C starts the salvage sub BGM once a type 1 / 2 box rises above the water and sets it; CreateInit 02467F24 clears it (tww the same) |
| 0x14 | `field_0x14` | `mPrevRibbonPos` | daSTBox_shadowEcallBack_c::draw 02468C20 keeps the first three particle positions of the shadow ribbon in it (each step writes them as the previous vertex position and stores the new ones); _create 024682CC constructs the array (tww draw the same) |


Open: `field_0x332` (initWait02 sets 20, nothing reads it in WWHD or tww), the shadow callback's `field_0x4` (only cleared in setup; execute stops the emitter while it is set, never set in WWHD or tww) and `field_0x50` (never written; execute turns the shadow by 0x8000 while it is negative).

## daArrow_Iceeff_c (ice arrow effect; d_a_arrow_iceeff.h)

Old names are the GameCube offsets (zeldaret/tww d_a_arrow_iceeff.h, same code).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B4 | `field_0x298` | `mpIceModel` | CreateHeap 02055B84 creates the 30 ice chunk models (only for the arrow version, mWaterType 0); set_mtx 02055D38 places them, _draw lights and draws the first min(mFrameCount, 30) (tww the same) |
| 0x430 | `field_0x314` | `mIceMtx` | CreateInit 02055F04 builds each ice chunk's random rotation and offset matrix (in the arrow's frame); set_mtx 02055D38 concatenates it to place chunk i (tww the same) |
| 0x9D0 | `field_0x8B4` | `mIceScale` | CreateInit 02055F04 sets chunk i's random scale (6.3 .. 10.8); set_mtx 02055D38 gives it to the chunk model (tww the same) |
| 0xB38 | `field_0xA1C` | `mpIceBtk` | CreateInit 02055F04 points it at Link's ice particle btk (mWaterType 0) or ice-water particle btk (1); _draw sets its frame to mBtkFrame, advance_frame stops at its last frame (tww the same) |
| 0xBC8 | `field_0xA30` | `mFrameCount` | _execute 020564D0 counts it up: the arrow version draws min(it, 30) chunks, advances the btk from 30 and breaks at 60; the water version starts the hit effect at 0, the ripples at 0x23 / 0x28 and ends at 300; CreateInit 02055F04 clears it (tww the same) |
| 0xBCC | `field_0xA34` | `mBtkFrame` | advance_frame (in _execute 020564D0) adds 1 up to the btk's last frame; _draw sets mpIceBtk's frame to it; CreateInit 02055F04 clears it (tww the same) |
| 0xBD0 | `field_0xA38` | `mWaterType` | _create sets 1 when the arrow's parameter is 4 (the ice arrow hit the water surface), else 0; CreateHeap / CreateInit / set_mtx / _draw / _execute 020564D0 switch between the 30 ice chunks (0) and the single water model (1) (tww the same) |
| 0xBD4 | `field_0xA3C` | `mVisible` | CreateInit 02055F04 sets 1; _draw draws the models only while it is set; _execute 020564D0 clears it at frame 0x28 of the water version (the break) (tww the same) |


Open: `field_0x290` / `field_0x294` (no use in WWHD or tww); the view ArrowIceParentHD takes daArrow_c's name `mImpactPos` (round 3).

## daDoor10_c (d_a_door10.h)

Old names are the GameCube offsets (zeldaret/tww d_a_door10.h, same members and inline accessors).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3EC | `m2D0` | `mSmoke` | demoProc 021268C8 runs the door smoke (smokeInit / smokeProc / smokeEnd), daDoor10_Delete 02127900 ends it, daDoor10_Create 02127C80 constructs it (tww dDoor_smoke_c m2D0 the same) |
| 0x4EC | `m354` | `mAction` | setAction() stores it (tww accessor name setAction); daDoor10_Execute 021277D0 runs l_action[it] |
| 0x4EE | `m356` | `mFlag` | checkFlag() / onFlag() / offFlag() test, set and clear its bits (tww accessor names) |
| 0x4F0 | `m358` | `mOpenHeight` | openInit 021262D8 / CreateInit 02127998 clear it, openProc 0212662C chases it to 300 and closeProc 021267A8 to 0 at speedF, openEnd 0212673C sets 300; calcMtx 02125FA4 raises the door by it, the hkyo bars follow it (tww the same) |
| 0x4FC | `m364` | `mDoorBsBdlIdx` | create 02127B20 sets 4 / 5 for door arguments 13 / 14 (else 0) and then loads the DoorBs archive; CreateHeap 02126074 takes model DoorBs[it] and collision DoorBs[8 or 9] instead of the stage's own door; daDoor10_Delete 02127900 releases DoorBs while it is set (tww the same) |


## daObjBuoyrace::Act_c (race buoy; d_a_obj_buoyrace.h)

Old names are the GameCube offsets (zeldaret/tww, same code). The sway spring takes daObjMovebox::Act_c's names for the same spring.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B0 | `m294` | `mWaterNrm` | set_water_pos 0232EF34 stores the normalised cross product of the water surface around the buoy; afl_calc_sway 0232F554 pulls the tilt toward its x / z (* 0.35, at most 0.3) (tww the same) |
| 0x3BC | `m2A0` | `mBobPhase` | afl_calc 0232F650 adds 1000 .. 2000 at random each frame; the buoy height follows mMeanSeaHeight + 2 * (0.5 * sin(it) - 120) + mBobOffsetY; _create 0232F3D8 uses it for the start height (tww the same) |
| 0x3C8 | `m2AC` | `mSwayX` | afl_calc_sway 0232F554 adds mSwaySpeedX each frame; set_mtx 0232F170 tilts the buoy by the up vector (mSwayX, 1, mSwayZ) (tww the same; as daObjMovebox::Act_c mSwayX) |
| 0x3CC | `m2B0` | `mSwayZ` | afl_calc_sway 0232F554 adds mSwaySpeedZ each frame; set_mtx 0232F170 uses it as the up vector's z (tww the same) |
| 0x3D0 | `m2B4` | `mSwaySpeedX` | afl_calc_sway 0232F554: spring speed of mSwayX (pull 0.02 toward the water normal's x, damping 0.04) (tww the same) |
| 0x3D4 | `m2B8` | `mSwaySpeedZ` | afl_calc_sway 0232F554: spring speed of mSwayZ (pull 0.02 toward the water normal's z, damping 0.04) (tww the same) |
| 0x3D8 | `m2BC` | `mBobOffsetY` | afl_calc 0232F650 adds mBobSpeedY (at most 30) and adds it to the buoy's target height (tww the same) |
| 0x3DC | `m2C0` | `mBobSpeedY` | afl_calc 0232F650: spring speed of mBobOffsetY (pull -0.01 * it, * 0.94 damping) (tww the same) |


Open: `m2A2[10]` (padding).

## daTag_Mk_c (d_a_tag_mk.h)

Old names are the GameCube offsets (zeldaret/tww d_a_tag_mk.h, same members).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3AC | `m290` | `mMsgNo` | talk 024B08E8 sets it from getMsg, shows it (setMessage) and steps it with next_msgStatus (tww the same) |
| 0x3B0 | `m294` | `mTalkState` | talkInit 024B065C sets 0; talk 024B08E8 turns it into 1 (message chosen), 2 (message shown) and -1 (talk over) (tww the same) |
| 0x3B2 | `m296` | `mDaichiStep` | actionDaichi 024B0BD4: its step (0 / 10 wait for tmp bit 0x440, 1 the first switch, 2 / 11 the cut timer and sub BGM, 3 / 12 the second switch); daTag_Mk_Create 024B0F4C sets 0, 10 or 20 by the switches already set (tww the same) |
| 0x3B8 | `m29C` | `mFacingDir` | daTag_Mk_Create 024B0F4C sets (sin, 0, cos) of current.angle.y for the villa tag; checkArea 024B0668 projects the player offset on it for the area depth / width test (tww the same) |
| 0x3D0 | `m2B4` | `mItemId` | demoInitMake 024B01C4 stores the created item's process id; demoInitSetgoal 024B031C and demoInitDelete 024B02D0 search the item by it (tww the same) |
| 0x3D4 | `m2B8` | `mStaffName` | daTag_Mk_Create 024B0F4C points it at the guest string "TagMk"; actionReady 024B0538 passes it to the event manager's getMyStaffId (tww the same) |


## daObjMshokki_c (dish; d_a_obj_mshokki.h)

Old names are the GameCube offsets (zeldaret/tww d_a_obj_mshokki.h, same members). `be<u32> m614, m618;` was split first.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x728 | `m60C` | `mType` | _create 0237D260 sets the dish type from the parameter (0 when out of range); create_heap 0237CE88 picks the bdl, _create the cylinder radius / height, break_proc 0237D6DC the particle, set_se 0237D574 the sound and _execute 0237D89C the attention height by it (tww the same) |
| 0x72E | `m612` | `mGroundTouched` | _execute 0237D89C sets 1 once the dish rests on the ground (ground hit without a landing); a later landing (after being knocked up) then breaks it (break_proc) (tww the same) |
| 0x730 | `m614` | `mFrameCount` | _execute 0237D89C adds 1 per frame; co_hitCallback 0237CF48 compares it with mLaunchFrame (tww the same) |
| 0x734 | `m618` | `mLaunchFrame` | co_hitCallback 0237CF48 stores mFrameCount when the rolling player knocks the dish up, and knocks it up again only more than 2 frames later (tww the same) |
| 0x738 | `m61C` | `mPushAngleY` | co_hitCallback 0237CF48 sets the pusher's facing + 0x2000 when it pushes the dish over; set_se 0237D574 plays the rattle only while shape_angle.y is within 4096 of it (tww the same) |


Open: `m610` (padding).


## MshData (l_data, the dish table; d_a_obj_mshokki.h)

Round 10. One entry per dish type (l_data.values[mType]); radius / height were named earlier. tww's l_data entries (d_a_obj_mshokki.cpp) keep m00 / m0C and read them the same way.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mAttentionOffsetY` | _execute 0237D89C: attention_info.position.y = current.pos.y + it (tww the same) |
| 0x0C | `m0C` | `mMaxTilt` | co_hitCallback 0237CF48: the push tilt shape_angle.x is strength * it; set_se 0237D574 scales the sound by the tilt / it (tww the same) |

## daItem_c and daItemBase_c_m_data (field item; d_a_item.h)

Old names are the GameCube offsets (zeldaret/tww d_a_item.h / d_a_itembase.h, same code). m_data offsets are within the struct.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x764 | `field_0x650` | `mLastSpeedY` | the last non-zero speed.y: CreateInit 021800F0 and the item actions (itemActionForRupee 02180DE4, ForKey 02181338, ForEmono 021814C0, ForArrow 021815E8, ForThrownItem 02182460) store speed.y while airborne and bounce with it * mGroundReflect on landing; set_bound_se 0218086C picks the bound sound by its size (tww the same) |
| 0x77A | `field_0x660` | `mSwordRockAngle` | itemActionForSword 0218188C: the lying sword's rocking angle, 0x2000 at the first ground touch, * -0.8 at each further touch; current.angle.x is eased to it + 0x4000 until it is 0 (tww the same) |
| 0x780 | `field_0x666` | `mSwordRockCount` | itemActionForSword 0218188C counts the ground touches of the rocking sword (1 starts the rock at 0x2000) and clears it when the rock ends (tww the same) |
| 0x2C | `field_0x2C` | `mAct4SpeedY` | initAction 0217F93C: daItemAct_4_e launches the item with speed.y = it + rnd(5) (tww daItemBase_c_m_data the same) |
| 0x30 | `field_0x30` | `mAct4SpeedF` | initAction 0217F93C: daItemAct_4_e gives the item a random speedF up to it (tww the same) |
| 0x44 | `field_0x44` | `mAct8SpeedY` | initAction 0217F93C: daItemAct_8_e launches the item with speed.y = it + rnd(+-5) and speedF mVelocityScale (tww the same) |


Open: daItemBase_c_m_data `field_0x34` (no use in WWHD or tww).

## daRaceItem_c (d_a_race_item.h)

Old names are the GameCube offsets (zeldaret/tww d_a_race_item.cpp, same code).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x750 | `field_0x63C` | `mGetTimer` | execute 02459230 sets 13 when the get starts (the item hops above Link's head) and counts it down in RaceItem_Get; the item is deleted below 0 (tww the same) |
| 0x754 | `field_0x640` | `mState` | the race item state (RaceItem_Wait / GetStart / Get / DropStart / Drop): execute 02459230 switches on it; CreateInit 024597B8, normalItemGet 02459EA0, raceItemForceGet 02459F6C, startOffsetPos 02459F90 and endOffsetPos 02459FA0 set it (tww the same) |
| 0x758 | `field_0x644` | `mType` | CreateInit 024597B8 sets parameter bits 15..18; checkGet 0245910C and execute 02459230 take a touch or a boomerang hit only for RaceItem_Normal; raceItemForceGet 02459F6C reads it (tww the same) |
| 0x759 | `field_0x645` | `mFlags` | bit RaceItem_BoomerangCarry: execute 02459230 sets it on a boomerang hit and clears it when the boomerang lets go or the get starts; checkOffsetPos 0245A000 and execute keep the item on its offset only while it is clear (tww cLib_offBit(field_0x645, 1)) |


## Small tags and objects (round 2 batch 1)

The small classes of this round, one or a few fields each:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x01 | `field_0x05` | `mForceClear` | modeClearWait 024A878C goes to the clear event while it is set, as when a DEFAULT_TREASURE event ends (debug switch); daTag_Gship_HIO_ct 024A8A70 sets 0 (tww the same) |
| 0x18 | `field_0x018` | `mStrokeLowY` | set_mtx 023660A4 and the spring joint callback 02365DD8 move the spring by (mStrokeHighY - it) * (mScale - 1): the low end of the spring stroke (tww L_attr 40 / 160) |
| 0x1C | `field_0x01C` | `mStrokeHighY` | set_mtx 023660A4 and the spring joint callback 02365DD8 move the spring by (it - mStrokeLowY) * (mScale - 1): the high end of the spring stroke (tww L_attr 125 / 270) |
| 0x20 | `field_0x020` | `mBodyTopY` | set_mtx 023660A4 scales the body height between mScalePivotY and it (span = it - mScalePivotY) so that its top follows the stroke (tww L_attr 150 / 300; tww set_mtx the same) |
| 0x3E0 | `field_0x2C4` | `mSplash` | clear_splash 024C9F44 clears the 30 entries, renew_splash 024C9F6C fills them with the particles of mpEmitters[1] and their y; check_water_h 024C9E8C looks a particle up in it to see whether it was above the water (-230) (tww the same) |
| 0x3E0 | `m2C8` | `mOpenX` | set_mtx 0239FD7C moves the two shutter halves by -it / +it in x; Execute 023A0060 raises it by 1 per frame while opening up to 100 (the save switch 2 goes on at 80); Create 0239FF5C sets 200 when already open, else 0 (tww the same) |
| 0x3E4 | `m2CC` | `mMode` | Execute 023A0060 switches on it (MODE_WAIT_e -> START -> OPENING -> OPEN); Create 0239FF5C sets MODE_OPEN_e or MODE_WAIT_e (tww the same) |
| 0xC | `field_0x0C` | `mOffset` | daShopItem_c::set_mtx 02483508 translates the item model by mData[m_itemNo]'s it after the actor position / angle (tww daShopItem_c::set_mtx the same) |
| 0x18 | `field_0x18` | `mRotation` | daShopItem_c::set_mtx 02483508 rotates the item model by mData[m_itemNo]'s it (ZXY) after mOffset (tww set_mtx the same) |
| 0x3BE | `unk_2A2` | `mOpenAngle` | calcMtx 02057F50 turns the jail door by home.angle.y - it; daAtdoor_actionOpen 0205828C raises it by 0x400 per frame up to 0x4000 (open), daAtdoor_actionClose 020581D0 lowers it to 0; CreateInit 0205842C sets 0x4000 when the switch is already on (tww the same) |
| 0x0 | `field_0x00` | `mBdlResIdx` | CreateHeap 023561E4 loads the platform model Hlift[it] for the size index mSizeIdx (tww CreateHeap the same) |
| 0x1 | `field_0x01` | `mDzbResIdx` | Mthd_Create 02356090 passes it to MoveBGCreate as the collision (dzb) resource of the size (tww Create the same) |
| 0x2 | `field_0x02` | `mHeapSize` | Mthd_Create 02356090 passes it to MoveBGCreate as the heap size of the size (tww Create the same) |
| 0x20 | `field_0x20` | `mLifeTimer` | BarrelSmokeCB::BarrelSmokeCB 0233A258 sets 60; execute 0233A2C4 counts it down, fades the smoke alpha below 50 (* 3.6) and ends the smoke at 0 (tww the same) |


Rows: daTag_Gship_HIO_c (HIO offset), daObjJump::Attr_c, daTpota_c, daObjTenmado::Act_c, daShopItem_c_m_data (offsets within mData's entry), daAtdoor_c, daObjHlift::Act_c::size_data (offsets within the entry), daObjEff::BarrelSmokeCB (the same `mLifeTimer` in StoolSmokeCB, SkullSmokeCB, PineconeSmokeCB and WoodBoxSmokeCB, each set to 60 by its constructor and counted down by its execute). daTag_Kk1_HIO_c::hio_prm_c `field_0x10` -> `mDebugDraw` (_draw 024ACC98 sets up the debug colour while it is set). Open: daTag_Gship_c `field_0x294` / `field_0x298` (getArg stores them, nothing reads them), daObjHlift `field_0x2F7` / `field_0x2FA` (padding), daFallRockTag_c `field_0x298` (execute clears it only), daObj_Stair_c `field_0x4C0` (written with a random offset while breaking, nothing reads it), daObjIceisland_c `field_0x394` (padding), kytag01_wave_chan_c `field_0x2f` (cleared only), daNpc_Zk1_c `m72C` (_execute copies current.pos into it once, nothing reads it) and HIO `field_0x8` (constructor -1 only), daTpota_c `field_0x3B4` (padding).
## daObj_Demo_Barrel_c (d_a_obj_demo_barrel.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x3EC | `m2D0` | `mSplashPos` | setParticleSibuki 02334098: current.pos with y from dBgS_ObjGndChk_Wtr_Func (the water surface), the position of the HAMON / SUIMEN / MIZUBASHIRA / SHIBUKI splash particles (tww the same) |
| 0x3F8 | `m2DC` | `mUnbroken` | daObj_Demo_BarrelCreate 02333EB4 sets 1; daObj_Demo_BarrelExecute 02334294 clears it at demo prm id 1 (onCollect, setParticleHahen: the barrel breaks); daObj_Demo_BarrelDraw 02334494 draws only while set (tww the same) |
| 0x3FA | `m2DE` | `mSplashDone` | daObj_Demo_BarrelCreate 02333EB4 clears it; daObj_Demo_BarrelExecute 02334294 calls setParticleSibuki once at demo prm id 2 and sets it (tww the same) |

Open (offset names kept):

- `m2A0[...]`: the unaccessed block up to 0x3EC (padding).


## daObj_Warpt_c (d_a_obj_warpt.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x3AC | `m290` | `mCurMode` | modeProc 023B9CB0: stores the new mode and calls its init / run from mode_tbl (tww mode_tbl[it].init / .run); _ride 023B9D5C / _draw 023BB074 test it (MODE_EVENT_WARP, the open mode 2); after tww daObj_Canon_c / daObj_Ikada_c mCurMode |
| 0x3B8 | `m29C` | `mWarpSclsNo` | getArg 023B9F74: parameter bits 12-19 (tww); normalWarp 023BB7AC warps to it once mWarpSwNo is on (tww warp(it), warp sets the next stage by scls number) |
| 0x3BC | `m2A0` | `mSpSclsNo1` | getArg 023B9F74: parameter bits 8-15 for the special pots (tww); spWarp 023BB6A4 warps to it when warp bit 1 is set (tww isWarpBit(1) -> warp(it)) |
| 0x3C0 | `m2A4` | `mSpSclsNo2` | getArg 023B9F74: parameter bits 16-23 for the special pots (tww); spWarp 023BB6A4 warps to it when warp bit 2 is set (tww isWarpBit(2) -> warp(it)) |
| 0x3C4 | `m2A8` | `mSpSclsNo4` | getArg 023B9F74: parameter bits 24-31 for the special pots (tww); spWarp 023BB6A4 warps to it when warp bit 4 is set (tww isWarpBit(4) -> warp(it)) |
| 0x3C8 | `m2AC` | `mLidSwNo` | getArg 023B9F74: switch number (tww uVar6 bits 0-7); openHuta 023BB400 turns it on, mLidClosed is set while it is off (tww dComIfGs_onSwitch / isSwitch(it, ..)); isOtherHuta 023B9884 / modeClose 023BB488 test it |
| 0x3CC | `m2B0` | `mWarpSwNo` | getArg 023B9F74: switch number (tww uVar6 bits 8-15); normalWarp 023BB7AC only warps while it is on (tww dComIfGs_isSwitch(it, ..)) |
| 0x3D0 | `m2B4` | `mPotType` | getArg 023B9F74: parameter bits 0-3 (tww); isSp 023B9864: 2 .. 4 are the special pots (tww it == 2 // 3 // 4); spWarp 023BB6A4 switches on it; createInit 023BA4E0 picks the smoke colours by it (tww m_def_smoke_prm_color[it]) |
| 0x3D4 | `m2B8` | `mEventRegNo` | getArg 023B9F74: parameter bits 4-7 for the special pots (tww); onWarpBit 023B9E94 / isWarpBit 023B9F1C use the event register m_event_reg[it] (tww) |
| 0x3DC | `m2C0` | `mOpenPrm` | getArg 023B9F74 (sp pots): home.angle.z bits 8-15; when not 0xFF getArg sets the pot's warp bit at once, so the lid starts open (mLidClosed = !isWarpBit) (tww the same) |
| 0x3E0 | `m2C4` | `mWarpBit` | getArg 023B9F74: 1, 2 or 4 by mPotType (tww); openHuta 023BB400 sets it in the event register (tww onWarpBit(it)); getArg sets mLidClosed from isWarpBit(it) (tww) |
| 0x3E2 | `m2C6` | `mLidClosed` | tww accessor isHuta() { return it; } (our isHuta 023B985C); getArg 023B9F74: set while the lid switch / warp bit is off (tww); openHuta 023BB400 clears it |
| 0x3F8 | `m2DC` | `mBodyBgW1Mtx` | createBodyHeap 023B9AE8: the matrix mpBodyBgW1 is set up with (tww mpBodyBgW1->Set(.., &it)); setMtx 023BA250 copies the pot matrix into it (tww cMtx_copy(.., it)) |
| 0x42C | `m310` | `mBodyBgW2Mtx` | createBodyHeap 023B9AE8: the matrix mpBodyBgW2 is set up with (tww mpBodyBgW2->Set(.., &it)); setMtx 023BA250 copies the pot matrix into it (tww) |
| 0x460 | `m344` | `mLidBgWMtx` | createHutaHeap 023B9944: the matrix mpLidBgW is set up with (tww mpLidBgW->Set(.., &it)); setMtx 023BA250 copies the lid matrix into it (tww) |
| 0x9AC | `m830` | `mLidPos` | setMtx 023BA250: current.pos raised by l_HIO.mLidOffsetY (tww it = current.pos; it.y += l_HIO.m08), where the lid models are put; setCollision 023BA1BC mCyl2 centre; breakHuta 023BAA28 particles; _draw 023BB074 tev position (tww) |
| 0x9B8 | `m83C` | `mBreakFireTimer` | modeBreakFireInit 023BB57C 60; modeBreakFire 023BB5CC acts when cLib_calcTimer(&it) reaches 0 (tww) |
| 0x9BC | `m840` | `mBgW2RegistTimer` | createInit 023BA4E0 30 or 5, modeEventWarp 023BB820 30 (tww); _execute 023BAD74 counts it down (tww cLib_calcTimer(&it)); setMtx 023BA250 registers mpBodyBgW2 when it is 1 (tww if (it == 1) .. Regist(mpBodyBgW2, this)) |
| 0x9C0 | `m844` | `mOpenSmokeCallBack` | modeOpenInit 023BB2A4: the follow callback of the open smoke particle 0x8161 (tww ID_AK_SN_WARPPOTOPEN00, &it); _execute 023BAD74 recolours its emitter; _delete 023BA8C8 removes it |
| 0x9D4 | `m858` | `mCloseSmokeCallBack` | modeCloseInit 023BB380: the follow callback of the close smoke particle 0x8162 (tww ID_AK_SN_WARPPOTCLOSE00, &it); modeOpenInit 023BB2A4 removes it; _delete 023BA8C8 removes it |
| 0x9E8 | `m86C` | `mSmokePrmColor` | createInit 023BA4E0: m_def_smoke_prm_color[mPotType] (tww); modeOpenInit 023BB2A4 / modeCloseInit 023BB380 pass it as the smoke particle's prim colour (tww particle_set(.., &it, &m870)) |
| 0x9EC | `m870` | `mSmokeEnvColor` | createInit 023BA4E0: m_def_smoke_env_color[mPotType] (tww); modeOpenInit 023BB2A4 / modeCloseInit 023BB380 pass it as the smoke particle's env colour (tww particle_set(.., &m86C, &it)) |

Open (offset names kept):

- `m298`: getArg 023B9F74 stores param bits 4-11 (normal pots); never read in WWHD or tww.
- `m2BC`: getArg 023B9F74 stores home.angle.z bits 0-7 (sp pots); its only read is openHuta 023BB400 playing JA_SE_READ_RIDDLE_1 while it is 0xFF, no clear role.
- `m2C5`: getArg 023B9F74 sets 0 / 1 / 2 for pot types 2 / 3 / 4; never read in WWHD or tww.


## daShopItem_c (d_a_shop_item.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x758 | `field_0x644` | `mpCloth` | clothCreate 02483C10 stores the dCloth_packet_c made for the flag items (nullptr for the others); set_mtx 02483508 sets its scale and matrix; _draw 02483B28 draws it (dCloth_packet_cloth_draw) (tww the same) |
| 0x75C | `field_0x648` | `mClothType` | clothCreate 02483C10 stores the cloth kind of the item (0-3, by item number), the index into clothRes / clothFunc; set_mtx 02483508 reads it again (tww the same) |
| 0x760 | `field_0x64C` | `mMtx` | set_mtx 02483508 copies the model matrix (mDoMtx_stack_c::get()) into it; CreateInit 024836F4 sets cullMtx to it (fopAcM_SetMtx) (tww the same) |


## daSpcItem01_c (d_a_spc_item01.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x750 | `field_0x63C` | `mLastSpeedY` | move 02489FA4 stores speed.y whenever it is non-zero; on ground landing the bounce speed is -it * 0.62 (stop when below gravity - 0.5) (tww the same) |
| 0x754 | `field_0x640` | `mTargetAngleX` | rotate_item 0248A08C: the angle.x target of cLib_chaseAngleS for rupees (angle.x + 0x2000 before the first bounce, else 0) (tww the same) |
| 0x756 | `field_0x642` | `mBounceCount` | move 02489FA4 adds 1 per ground landing; set_effect 02489E3C makes the appear effect only while it is 0; rotate_item 0248A08C spins rupees only while it is 0 (tww the same) |
| 0x758 | `field_0x644` | `mFlag` | CreateInit 0248A2A4 stores daSpcItem01_prm::getFlag(this); set_effect 02489E3C makes the appear effect only with bit 0 set (tww the same) |

Open (offset names kept):

- `field_0x643`, `field_0x646`, `field_0x647`: no use in WWHD or tww.


## daBranch_c (d_a_branch.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x3D4 | `m02B8` | `mShapeId` | the demo shape id: daBranch_Execute 020DF140 (execute inlined) stores the demo actor's getShapeId() and plays mAnims[0] for 6 / mAnims[1] for 5; daBranch_Draw 020DEFB0 draws model 1 for 5, else model 0; daBranch_Create 020DF534 sets 6 (tww the same) |

Open (offset names kept):

- `m02BC`, `m02BD`: only daBranch_Create 020DF534 clears them; never read in WWHD or tww.


## Act_c (d_a_obj_ekskz.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x6D4 | `m480` | `mBroken` | Create 0233BCBC clears it; Execute 0233BEC8 sets 1 when the object breaks (dComIfGs_onSwitch(prm_get_swSave()), particle 0x82EC) and handles the pieces while set; Draw 0233C0C8 draws the whole object only while it is 0; broken |

Open (offset names kept):

- `m504[2]`, `m510`: only the constructor Act_c_ct 0233B4A8 clears them; no use in tww.


## daKnob00_c (d_a_knob00.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `m2D0` | `mDoorMsg` | the dDoor_msg_c door message: demoProc 021A3FB8 init(home.angle.z) at DEMO_MESSAGE and proc() until it ends; actionPassward2 021A4808 init(0x1B1A + event reg 0xBA0F) and proc() for the password door (tww: dDoor_msg_c mDoorMsg) |


## daObjRcloud_c (d_a_obj_rcloud.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x434 | `m2BC` | `mAction` | setup_action 02387428 stores the act_proc index next to mCurrentActProc; setTexMtx 0238767C scrolls the texture's y by mCloudAnimProgress only while it is 2 (clouds_lift_act_proc) |

Open (offset names kept):

- `m2C5[3]`: no use in WWHD or tww.


## Act_c (d_a_obj_dragonhead.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x528 | `field_0x40c` | `mBgRegistered` | CreateInit 02337A28 clears it and sets 1 when dComIfG_Bgsp()->Regist(mpBgW) succeeds (switch off); Execute 02337FA0 releases mpBgW while set when the switch turns on (clears it on success, JA_SE_OBJ_ICEVERG_MELT); Delete 02337CAC releases it while set (tww: background registered flag); tww: background registered flag |


## daTag_Hint_c (d_a_tag_hint.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x3AC | `m290` | `mTalkMsgId` | talk 024A9E68 stores getMsg() at the first step and drives the message with it (hintMessageSet / next_msgStatus) (tww the same) |
| 0x3B0 | `m294` | `mTalkState` | talkInit 024A9CF8 clears it; talk 024A9E68 steps it 0 -> 1 (message created) -> 2 (shown) and -1 at the end (tww the same) |
| 0x3B8 | `m29C` | `mStaffId` | startProc 024A97E4 stores getMyStaffId("TAGHINT"); actionEvent 024A9FAC reads the event actions and ends the cuts with it (tww the same) |
| 0x3BC | `m2A0` | `mHintDelay` | arrivalTerms 024A9284 sets 600 / 3600 / 1800 / 900 / 0 by hint type; otherCheck 024A996C counts it down (types 4 / 8 / 9 / 12 before the range check, the others while no message is open) and allows the hint only at 0; daTag_Hint_Create 024AA334 clears it (tww the same) |
| 0x3BF | `m2A3` | `mGrabHint` | makeEventId 024A94A0 sets 1 when Link holds an actor (event DEFAULT_HINT2), else 0; setPlayerAngle 024A95C4 then uses search_angle_table_grab (tww the same) |
| 0x3C0 | `m2A4` | `mHintEventIdx` | makeEventId 024A94A0 stores the event index of HIRL_HINT / DEFAULT_HINT2 / DEFAULT_HINT; actionHunt 024A9B78 / actionArrival 024A9C74 order it (eventInfo.setEventId) (tww the same) |
| 0x3C2 | `m2A6` | `mFlags` | actionEvent 024A9FAC sets bit 0 when the event advances (action 2 ends the cut only with it); tww chkFlag / setFlag on it |
| 0x3C4 | `m2A8` | `mRangeXZSq` | daTag_Hint_Create 024AA334: (scale.x * 100) squared; rangeCheck_local 024A8BC0 needs the squared XZ distance below it (tww the same) |
| 0x3C8 | `m2AC` | `mRangeY` | daTag_Hint_Create 024AA334: scale.y * 100 (2410 for type 14); rangeCheck_local 024A8BC0 needs the height difference within it (tww the same) |
| 0x3CC | `m2B0` | `mDarkRatio` | startProc 024A97E4 sets 1; actionEvent 024A9FAC lowers it by 0.05 to 0.15 during the hint talk, actionLight 024A8F08 raises it back to 1; darkProc 024A8D2C passes it to dKy_set_vrboxcol_ratio / dKy_set_fogcol_ratio (tww the same) |


## daBeam_c (d_a_beam.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x658 | `m53C` | `mAtHit` | daBeam_AtHitCallback 02074C98 / daBeam_checkHitCallback 02075078 set 1 and store the hit position in mAtHitPos; _execute 02075988 then ends the beam there (no hot floor) and clears it (tww the same) |
| 0x65C | `m540` | `mAtHitPos` | daBeam_AtHitCallback 02074C98 / daBeam_checkHitCallback 02075078 copy the AT hit position (mGObjAt.mHitPos); _execute 02075988 shortens the beam to it and copies it into mBeamEndPos (tww the same) |
| 0x668 | `m54C` | `mSidePush` | CreateInit 02075384: param bits 24-27 (0 for the searching type); while non-zero the hit on Link (hitDirection, in the AT hit callbacks) sets the AT / TG vectors to the beam's side direction (tww the same) |
| 0x66C | `m550` | `mSearchProc` | _execute 02075988 calls it every frame; CreateInit 02075384 sets it; tww setSearchProc(func) stores into it |
| 0x674 | `m55C` | `mDefaultProc` | _execute 02075988 restores mSearchProc from it when the switch is off; CreateInit 02075384 sets it; tww setDefaultProc(func) stores into it |
| 0x718 | `m588` | `mBeamFrame` | the beam's grow frame 0..5: move_search 02076260 counts it up while on target, fade (in move_search / wait_proc 02076598) and _execute 02075988 use it; set_mtx 02075124 width (it * (5 - mFadeFrame)) / 50 + 0.5; the BG line length is it * 20 * scale.z (tww the same) |
| 0x798 | `m5A8` | `mFadeFrame` | fade (in move_search 02076260 / wait_proc 02076598) counts it up to 4 while mBeamOn and then ends the beam; set_mtx 02075124 narrows the beam by it (tww the same) |
| 0x814 | `m5C4` | `mHotFloorMtx` | set_mtx 02075124 builds the hot floor matrix (at mBeamEndPos, mHotFloorQuat, Y rotation, z scale mHotFloorScaleZ) into it and gives it to mpHotFloor (mtx_p) (tww the same) |
| 0x844 | `m5F4` | `mBeamOn` | move_search 02076260 sets 1 once mBeamFrame reached 5 on target; fade (in move_search / wait_proc 02076598) fades the beam out and clears it; _execute 02075988 sets the AT capsule only while it is 1 (tww the same) |
| 0x846 | `m5F6` | `mCycleTimer` | no use in WWHD (CreateInit 02075384 runs move_search for kind 2); tww CreateInit stores a parameter in it for kind 2 and timer_change counts it down: beam on while it is above 0, off below, reloaded with 60 at -30 |
| 0x848 | `m5F8` | `mHFloorParticle` | CreateInit 02075384: 1 - param bit 28; _execute 02075988 passes !it as bit 0 of the Hot_Floor parameters; tww checkHFloorParticle / onHFloorParticle / offHFloorParticle use it |
| 0x849 | `m5F9` | `mSmokeParticle` | CreateInit 02075384: 1 - param bit 29; _execute 02075988 passes !it as bit 1 of the Hot_Floor parameters; tww checkSmokeParticle / onSmokeParticle / offSmokeParticle use it |
| 0x8B8 | `m668` | `mBeamEndPos` | _execute 02075988: the beam's end (mAtHitPos, the BG line cross or the line end), where the Hot_Floor is created and JA_SE_OBJ_STATUE_BEAM plays; set_mtx 02075124 places the hot floor there; daBeamDelete 02075924 deletes its sound (tww the same) |
| 0x8C4 | `m674` | `mHotFloorQuat` | _execute 02075988: Quaternion_setRotate from the hit polygon's normal; set_mtx 02075124 rotates the hot floor by it; CreateInit 02075384 sets ZeroQuat (tww the same) |
| 0x8D4 | `m684` | `mHotFloorId` | _execute 02075988 stores the fopAcM_create id of the Hot_Floor (-1 without one) and looks the actor up by it (tww the same) |
| 0x8D8 | `m688` | `mHotFloorScaleZ` | _execute 02075988 sets 1; set_mtx 02075124 scales the hot floor matrix's z by it (tww the same) |
| 0x8DC | `m68C` | `mpHotFloor` | _execute 02075988 finds the daHot_Floor_c by mHotFloorId (nullptr while none); set_mtx 02075124 gives it mHotFloorMtx and mbSpawnParticle (tww the same) |
| 0x8E0 | `m690` | `mpBeamEmitter` | the ID_AK_SN_LASERROOT00 emitter: move_search 02076260 creates it, move_search / wait_proc 02076598 end it; set_mtx 02075124 places and scales it; tww beamCheck() tests it |
| 0x8E4 | `m694` | `mEmitterOffsetZ` | CreateInit 02075384: 0.001 times a global float for the searching type, else 0; set_mtx 02075124 moves the emitter matrix along z by it |
| 0x8E8 | `m698` | `mSwitchNo` | CreateInit 02075384: param bits 16-23 (searching type); CreateInit / _execute 02075988 switch to wait_proc while that switch is set (0xFF: none) (tww the same) |


## daArrow_c (d_a_arrow.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x71C | `field_0x600` | `mDeleteRequest` | procStop_BG 02054FE0 / procStop_Actor 020553C0 delete the stuck arrow once it is set; set when the stuck time (mStopTimer) ends, when the hit actor is gone, by mbSetByZelda, and by _execute 02052DB8 when l_count comes round to mShotNo again (tww the same) |
| 0x71E | `field_0x602` | `mShotNo` | arrowShooting 02053814 stores l_count (0-4) for the arrow; _execute 02052DB8 sets mDeleteRequest when l_count is back at it; createInit 02052738 sets -1 (tww the same) |
| 0x720 | `field_0x604` | `mHitShakeTimer` | procMove 02054040 sets 0x28 when the arrow sticks; setStopActorMatrix 02053DDC / procStop_BG 02054FE0 count it down and shake the arrow's x angle by (1024 * (t / 40)^2) * sin(t * 0x52FB) (tww the same) |
| 0x734 | `field_0x618` | `mHitJointOffset` | procMove 02054040 stores the hit position local to the hit joint (ArrowSearchJointHit); setStopActorMatrix 02053DDC translates the joint matrix by it (tww the same) |
| 0x79E | `field_0x682` | `mLightEffType` | setLightEffect 02052C40: the arrow type the light effect child was made for; a different mArrowType deletes the child (tww the same) |
| 0x7B0 | `field_0x698` | `mDraw` | _draw 02053188 draws only while set; createInit 02052738 sets it; procStop_BG 02054FE0 blinks it during the last 60 frames of mStopTimer; cleared when the arrow ends in water or hits (tww the same) |
| 0x7B1 | `field_0x699` | `mWaterEffect` | check_water_in 020532B8 / procReturn 02055450: the state byte passed to and returned by Link's setItemWaterEffect; createInit 02052738 clears it (tww the same) |
| 0x7B2 | `field_0x69a` | `mBounceGround` | procReturn 02055450 sets it when the falling arrow bounces off a polygon with normal.y >= 0.5 and deletes the arrow once it falls again; createInit 02052738 clears it (tww the same) |
| 0x7B4 | `field_0x69c` | `mSpinSpeedX` | rebound (in procMove 02054040) sets 0x2C00; procReturn 02055450 adds it to shape_angle.x each frame and halves and reverses it on a bounce; createInit 02052738 clears it (tww the same) |
| 0x7B8 | `field_0x6a0` | `mStopTimer` | createInit 02052738 sets 300; procStop_BG 02054FE0 counts it down for normal arrows (blink below 60, mDeleteRequest at 0) (tww the same) |
| 0x7C0 | `field_0x6a8` | `mImpactPos` | arrowShooting 02053814 sets the shot position; procMove 02054040 stores the hit position (BG cross or AT hit) and plays the hit particles there; setLightEffect 02052C40 creates the light effect at it; procStop_BG 02054FE0 keeps the arrow tip there (moving BG); procMove sets mFlewFar beyond 20000 of it (tww the same) |
| 0x7CC | `field_0x6b4` | `mModelMtx` | procWait 02053D24 / procMove 02054040 copy the model (or hit stack) matrix into it each frame; the light effect child daArrow_Lighteff_c reads it (setTopPos 020570BC / set_mtx 02056EB8) (tww the same) |
| 0x7FC | `field_0x6e4` | `mFlewFar` | procMove 02054040 sets it once the arrow is farther than 20000 from mImpactPos (the shot position); check_water_in 020532B8 / procMove then skip dKy_arrowcol_chg_on; createInit 02052738 clears it (tww the same) |
| 0x7FE | `field_0x6e6` | `mHitAngle` | procWait 02053D24 / procMove 02054040 store shape_angle each frame, and the angle local to the hit joint on an actor hit; setStopActorMatrix 02053DDC rotates the stuck arrow by it, procStop_BG 02054FE0 uses it as the base of the shake (tww the same) |

Open (offset names kept):

- `field_0x624[...]`: unaccessed block.


## ArrowLightArrow (d_a_arrow_lighteff.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `field_0x6b4` | `mModelMtx` | as daArrow_c::mModelMtx; setTopPos 020570BC / set_mtx 02056EB8 copy it |
|  | `field_0x6e4` | `mFlewFar` | as daArrow_c::mFlewFar; _execute 02057930 deletes the effect while it is set |
|  | `field_0x6e6` | `mHitAngle` | as daArrow_c::mHitAngle; _execute 02057930 copies it into current.angle |


## daArrow_Lighteff_c (d_a_arrow_lighteff.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x3B4 | `field_0x298` | `mpModel` | CreateHeap 02056B54 creates the effect model (mDoExt_J3DModel__create); CreateInit 0205715C sets cullMtx to its matrix; set_mtx 02056EB8 sets its base scale and matrix (tww the same) |
| 0x3B8 | `field_0x29C` | `mTopPos` | setTopPos 020570BC: 62 along the parent arrow's mModelMtx; CreateInit 0205715C makes the type particles there; setPointLight 0205777C puts the point light there (tww the same); top position |
| 0x3C4 | `field_0x2A8` | `mScale` | CreateInit 0205715C sets 1 (or the particle manager's global scale); _execute 02057930 stretches z up to 20 and shrinks x / y to 0.5 while the arrow flies; set_mtx 02056EB8 sets it as the model's base scale (tww the same); model scale |
| 0x4BC | `field_0x2E0` | `mBrkFrame` | brk_play (in _execute 02057930) counts it toward mBrk's end frame (toward 5 while the camera attention status 0x20 is set) and sets mBrk's frame to it; CreateHeap 02056B54 clears it (tww the same); brk frame |
| 0x4C0 | `field_0x2E4` | `mEndTimer` | CreateInit 0205715C clears it; _execute 02057930 counts it up while the arrow is stuck (param 2 / 4) and deletes the effect at 0x3C (tww the same) |
| 0x4C4 | `field_0x2E8` | `mArrowType` | the arrow type: CreateHeap 02056B54 stores the parameter; CreateInit 0205715C picks the particles, setPointLight 0205777C the light colour, delete_particle 02057510 the alpha fade by it (tww the same); type |
| 0x4C5 | `field_0x2E9` | `mDraw` | CreateInit 0205715C sets 1; _execute 02057930 clears it once the arrow is stuck; _draw 0205763C draws only while set (tww the same); draw |
| 0x4C6 | `field_0x2EA` | `mUseArrowEffect` | _execute 02057930 sets 1 when it calls Link's onUseArrowEffect (the arrow is shot); _delete 020575A4 calls offUseArrowEffect while set; _create 02057478 clears it (tww the same) |
| 0x4C8 | `field_0x2EC` | `mArrowParam` | _execute 02057930 stores the parent arrow's parameter (its state: 1 flying, 3 returned, other stuck) each frame; setPointLight 0205777C lights only while it is 0 or 1; CreateInit 0205715C clears it (tww the same) |
| 0x4CC | `field_0x2F0` | `mPrevArrowParam` | _execute 02057930 copies mArrowParam into it before reading the new one; the first frame of param 1 (it != 1) starts the arrow effect; CreateInit 0205715C clears it (tww the same) |
| 0x4D0 | `field_0x2F4` | `mParticleCb0` | the follow callback of the first type particle (HINOKO00 / KIRAKIRA00 / TSUBU00) made by CreateInit 0205715C; delete_particle 02057510 fades and removes it (tww the same) |
| 0x4E4 | `field_0x308` | `mParticleCb1` | the follow callback of the second type particle (REIKI00 / FLASH00) made by CreateInit 0205715C; delete_particle 02057510 fades and removes it (tww the same) |
| 0x4F8 | `field_0x31C` | `mLight` | setPointLight 0205777C fills this LIGHT_INFLUENCE (position mTopPos, the type colour, power mLightPower * 150, fluctuation 250) (tww the same) |
| 0x51C | `field_0x33C` | `mLightPower` | setPointLight 0205777C eases it toward 1 + rnd(0.2) while the arrow flies with a non-normal type, else 0; the light power is it * 150 (tww the same) |

Open (offset names kept):

- `field_0x290`, `field_0x294`: no use in WWHD or tww.


## daTag_Etc_c (d_a_tag_etc.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x3B6 | `field_0x29A` | `mCutEndDelay` | demoInitProc 024A5FA4 sets 15 (type2 0); demoProc 024A5E0C counts it down while Medli is not flying and ends the TAG_ETC_D cut at 0 (tww the same) |

Open (offset names kept):

- `field_0x291`, `field_0x292`, `field_0x293`: no use in WWHD or tww.


## TagEtcMedli (d_a_tag_etc.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `m30F0` | `mStatus` | as daNpc_Md_c::mStatus (d_a_npc_md.h, same offset 0x420C); checkStatusFly / onBitCamTagIn / offBitCamTagIn test and set its bits |


## daGhostship_c (d_a_ghostship.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `field_0x6C4` | `mPathRadiusTarget` | _execute 0214A9A4: per circle path the target of cLib_addCalc2 on mPaths[i].mRadius, 1500 + 100 * i while the radius is below 900, 800 once it passed 1400 + 100 * i (tww the same) |

Open (offset names kept):

- `field_0x2B0`, `field_0x2C0`, `field_0x2C4`, `field_0x2C8`, `field_0x700`: no read in WWHD or tww. Hio `field_0x0C`: only the HIO constructor sets 5.0. Cloth `field_0xF0` (no use), `field_0xF2` (only cleared).


## Hio (d_a_ghostship.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `field_0x04` | `mDebugDraw` | _draw 0214B4A8 sets up the debug axis / fan / vector colours only while it is set; the HIO constructor clears it (tww the same) |


## daObj_hsh_c (d_a_obj_hsehi1.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `field_0x514` | `mOrderEvent` | the event to order: waitAction 0235C144 sets 5 (argument 0) or 3 while Link is in attention; eventOrder 0235ABE0 makes Link able to talk for 3 / 4 / 5 (orders the speak event for 4) or orders mEventId[it] for 0 / 1; checkOrder 0235AAA0 / checkCommandTalk 0235A628 / eventProc 0235A6F8 reset it to -1; init 0235AEF8 sets -1 |

Open (offset names kept):

- `field_0x4d8[12]`, `field_0x516`, `field_0x51a[2]`, `field_0x524[4]`, `field_0x52c[4]`: no use. `field_0x51c`, `field_0x51e`, `field_0x520`, `field_0x522`, `field_0x528`: only init 0235AEF8 clears them. HIO prm `field_0x16`: no use.


## hio_prm_c (d_a_obj_hsehi1.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `field_0x0c` | `mAttnOffsetY` | setAttention 0235BF68 adds it to the attention_info.position height (tww the same) |
|  | `field_0x10` | `mEyeOffsetY` | setAttention 0235BF68 adds it to the eyePos height (tww the same) |


## daObjGnnbtaki_c (d_a_obj_gnnbtltaki.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `field_0x2B0` | `mAnmStarted` | _create 0234AFF8 sets 0 for type 1 (btk stopped) and 1 otherwise; _execute 0234B184 starts the btk animation (rate 1) once for type 1 when the g2before event passes demo frame 0x126F in the tower stage and sets it |
|  | `field_0x2B1` | `mType` | _create 0234AFF8: param bits 0-3 (0xF -> 0); type 1 waits for the g2before event (_execute 0234B184) |


## MmrrAttributes (d_a_obj_mmrr.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `field_0x00` | `mTriVertices` | set_cc_rec_pos 02374E98 transforms vertex(i, j) of these 5 x 3 points into the five mTri collision triangles (L_attr, filled by the static initialiser; tww set_cc_rec_pos the same) |
|  | `field_0xB4` | `mTriOffsetY` | set_cc_rec_pos 02374E98 translates the triangle frame by (0, it, mTriOffsetZ) before rotating it by mMirrorAngleX (tww the same) |
|  | `field_0xB8` | `mTriOffsetZ` | set_cc_rec_pos 02374E98: the z of that translation (tww the same) |
|  | `field_0xBC` | `mMirrorAngleX` | set_cc_rec_pos 02374E98 rotates the triangle frame about x by it; set_cull 02375264 turns the reflected beam's cull box by it (tww the same) |
|  | `field_0xC0` | `mBeamStart` | set_cc_trans_pos 02374FC0 translates to it for the start of the reflected beam capsule (tww the same) |
|  | `field_0xD0` | `mBeamLength` | set_cc_trans_pos 02374FC0: the beam end lies at z = it; mBeamLenRate is the line-check distance over it; set_cull 02375264 sizes the cull box by it (tww the same) |

## daBemos_c (Beamos: blue, red and yellow types; d_a_obj_bemos.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`, unnamed there). Named from the uses in d_a_obj_bemos.cpp.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3AC | `m290` | `mEyeProc` | tww's type EyeProc (pointer to member function); blue / red / yellow eye *_init (blue_eye_wait_init 02324428, blue_eye_charge_init 0232666C, red_eye_wait_init 02324830, yellow_eye_wait_init 02324C5C, ...) store the eye state function, _execute 02326020 calls it |
| 0x3B4 | `m29C` | `mHeadAngle` | set_mtx 023252C8: rotation of the head model (y added to current.angle.y); blue_eye_charge 023268D0 / red_eye_wait 0232728C turn it toward the target or by mRotSpeed; CreateInit3 02324DC8 aims it at the next path point |
| 0x3BC | `m2A4` | `mHeadPos` | CreateInit1 02324628 / CreateInit2 02324A2C: current.pos raised by 260 (+REG); set_mtx 023252C8 places the head model there; the beam child (fpcNm_Beam_e) and the BMOSBALL00 particle are created at it; mSph centres on it |
| 0x3D0 | `m2B8` | `mpBodyModel` | CreateHeap1 0232409C / CreateHeap2 0232419C / CreateHeap3 0232435C: model BDL_HMOS1 / HMOS2 / HMOS3 (by mType); set_mtx 023252C8 scales and places it at current.pos; _draw 023261A0 draws it; CreateInit 02325558 takes its matrix as cullMtx |
| 0x3D4 | `m2BC` | `mpHeadModel` | CreateHeap1 0232409C / CreateHeap2 0232419C: model BDL_HMOSUP1 / HMOSUP2 (blue and red types); set_mtx 023252C8 scales it by mHeadScale and places it at mHeadPos turned by mHeadAngle; _draw 023261A0 draws it |
| 0x3D8 | `m2C0` | `mpHmos2tModel` | CreateHeap2 0232419C: model BDL_HMOS2T with the brk mBrkAnm2 (red type only); set_mtx 023252C8 turns it with the head yaw at current.pos; _draw 023261A0 draws it with mBrkAnm2 |
| 0x4D4 | `m2FC` | `mpWaitEmitter` | blue_eye_wait_init 02324428 / red_eye_wait_init 02324830 / yellow_eye_wait_init 02324C5C: particle ID_AK_SN_BMOSWAIT00 (wait glow); blue_eye_charge_init 0232666C and event_move 02325B44 (switch on) stop it |
| 0x4D8 | `m300` | `mpEyeEmitter` | blue_eye_wait_init 02324428 / red_eye_wait_init 02324830: particle ID_AK_SN_BMOSEYE00; blue_eye_wait 023267CC / red_eye_wait 0232728C move it to mBeamRootPos and set its alpha to mEyeAlpha |
| 0x4DC | `m304` | `mpChargeEmitter` | blue_eye_charge_init 0232666C / red_eye_charge_init 0232712C / eye_break_init 0232599C: particle ID_AK_SN_BMOSBALL00 at mHeadPos; blue_eye_wait_init 02324428 / yellow_eye_wait_init 02324C5C stop it |
| 0x4E4 | `m30C` | `mpLaserRootEmitter` | blue_eye_charge 023268D0: particle ID_AK_SN_LASERROOT00 at mBeamRootPos once mChargeCount passes mChargeTime, scaled by mLaserRootScale; stopped when the laser starts or the charge ends |
| 0x4E8 | `m310` | `mEyeAlpha` | CreateInit1 02324628 / CreateInit2 02324A2C set 255; blue_eye_charge 023268D0 fades it to 0, blue_eye_wait 023267CC / red_eye_wait 0232728C back to 255 (then attention off); the alpha of mpEyeEmitter |
| 0x4EC | `m314` | `mLaserRootScale` | blue_eye_charge 023268D0: (6.25 - (mLaserRootTimer - 2.5)^2) * 1.6 in x, y, z, the scale of mpLaserRootEmitter; CreateInit1 / CreateInit2 clear it |
| 0x4F8 | `m320` | `mSearchRange` | CreateInit1 02324628 / CreateInit2 02324A2C: param bits 0-7 * 100 + 500; CreateInit3 02324DC8: 1000 or the distance to the target point; blue_range_check 02326274 / red_range_check 02326460: the player is found closer than it |
| 0x52C | `m354` | `mBeamRootPos` | set_mtx 023252C8: the point 60 in front of the head (mHeadPos, mHeadAngle); mpEyeEmitter / mpLaserRootEmitter follow it; blue_eye_search 02326E4C / red_eye_search 02327968 put the beam actor there |
| 0x538 | `m360` | `mHeadScale` | set_mtx 023252C8: base scale of mpHeadModel; CreateInit1 / blue_eye_wait_init 02324428 set (0, 0.75, 1); blue_eye_charge 023268D0 grows x to 1 (HIO m14) and shrinks it back (HIO m18), the laser starts once x is 1 |
| 0x544 | `m36C` | `mLastPlayerPos` | red_eye_search 02327968 stores the player position when the fire time (mFireTimer) ends; red_range_check 02326460 re-arms (mArmed) only once the player is more than 5 away from it or out of the angle |
| 0x878 | `m6A0` | `mGroundY` | CreateInit1 02324628 / CreateInit2 02324A2C: cBgS_GroundCross of mGndChk (the ground under the Beamos); not read in WWHD; tww's daBemos_c::_draw passes it to dComIfGd_setSimpleShadow2 as the shadow ground height |
| 0x87C | `m6A4` | `mInRange` | blue_range_check 02326274 / red_range_check 02326460 / yellow_range_check 0232665C store the last result (1 = player in range); the eye states pass it back as the active flag, which widens the range (HIO m30 / m34 / m2C) |
| 0x880 | `m6A8` | `mSwitchNo` | CreateInit3 02324DC8: param bits 0-7 (passed to the child); yellow_eye_wait 02327D98 turns the beam off and event_move 02325B44 stops mpWaitEmitter while dSv_info_c::isSwitch(it) (255: none) |
| 0x884 | `m6AC` | `mLaserRootTimer` | blue_eye_charge 023268D0 counts it up to 5 once fully charged (then blue_eye_search_init) and down again when the player leaves; drives mLaserRootScale; blue_eye_charge_init 0232666C / blue_eye_search_init 023268A8 clear it |
| 0x886 | `m6AE` | `mChargeCount` | blue_eye_charge_init 0232666C clears it or sets HIO m1E; blue_eye_charge 023268D0 counts it up to mChargeTime while the player is in range and down otherwise; red_eye_charge 023273F0 up to HIO m28 |
| 0x888 | `m6B0` | `mChargeTime` | blue_eye_charge_init 0232666C: HIO m1C (from the wait state) or m1E; blue_eye_charge 023268D0: the laser root starts once mChargeCount passes it |
| 0x88A | `m6B2` | `mRotSpeed` | CreateInit2 02324A2C: param bits 8-15 (signed) * 250 + 750 / -500; red_eye_wait 0232728C adds it to mHeadAngle.y every frame (the red head turns) |
| 0x88E | `m6B6` | `mFireTimer` | red_eye_search_init 023273C0 sets HIO m42; red_eye_search 02327968 counts it down while firing and at 0 stores mLastPlayerPos and clears mArmed |
| 0x890 | `m6B8` | `mType` | _create 02325698: param bits 28-31; 0 = blue (CreateHeap1 / CreateInit1), 1 = red (2), 2 = yellow (3); set_mtx 023252C8, eye_break_init 0232599C, event_move 02325B44 and guard_proc 02325DB8 branch on it |
| 0x891 | `m6B9` | `mSubType` | CreateInit3 02324DC8: param bits 8-15 (255 -> 0), passed to the child; 2: other beam flags and no guard cylinder; yellow_eye_wait 02327D98: 1 turns the beam off while the player is in another room; _execute 02326020 tests 2 |
| 0x892 | `m6BA` | `mSwitchOn` | CreateInit3 02324DC8 / event_move 02325B44: the last dSv_info_c::isSwitch(mSwitchNo); event_move starts the hmos3cam event when it turns on |
| 0x893 | `m6BB` | `mEventStep` | event_move 02325B44: 1 orders the hmos3cam event, 2 while it runs (cutEnd until it ends, then 0); CreateInit3 02324DC8 clears it |
| 0x894 | `m6BC` | `mArmed` | CreateInit 02325558 sets 1; red_eye_search 02327968 clears it when mFireTimer ends; red_range_check 02326460 does not look for the player while it is 0 and sets it again once the player moved more than 5 from mLastPlayerPos or out of the angle |
| 0x895 | `m6BD` | `mHasPath` | CreateInit3 02324DC8: 1 when the yellow Beamos takes its position from a path (and creates the second one at the next point); event_move 02325B44 (the switch event) and guard_proc 02325DB8 (guard cylinder) run only for it |
| 0x897 | `m6BF` | `mDropItemNo` | CreateInit1 02324628 / CreateInit2 02324A2C: shape_angle.z bits 0-5; eye_break 02328120 passes it to fopAcM_createItemFromTable as the item number |
| 0x898 | `m6C0` | `mDropItemBitNo` | CreateInit1 02324628 / CreateInit2 02324A2C: shape_angle.z bits 6-12; eye_break 02328120 passes it to fopAcM_createItemFromTable as the item bit |

Open: `m308` (an emitter slot that only CreateInit 02325558 / eye_break_init 0232599C clear and _delete 023258A8 stops; nothing sets it in WWHD or tww), `m6BE` (no use; tww has none either).

### daBemos_HIO_c (l_HIO; d_a_obj_bemos.h)

Round 3: the Beamos HIO values. In WWHD the vtable sits at the end, so each field is 4 below its GameCube name. Each is set only by daBemos_HIO_c__ct 02328718 (default in parentheses) and never written elsewhere in WWHD or tww; the reads are in our d_a_obj_bemos.cpp (through bemos_hio()) and match zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00004 | `m08` | `mBlueSearchWidth` | daBemos_c::blue_range_check 02326274: the half width of the search box in front (the player's side distance must be below it) (tww the same) (400) |
| 0x00010 | `m14` | `mBlueHeadOpenStep` | daBemos_c::blue_eye_charge 023268D0: the cLib_chaseF step of mHeadScale.x toward 1 (y a quarter of it) while the player is in range (tww the same) (0.3) |
| 0x00014 | `m18` | `mBlueHeadCloseStep` | daBemos_c::blue_eye_charge 023268D0: the cLib_chaseF step of mHeadScale.x toward 0 (y a quarter of it) once the player is out of range (tww the same) (0.3) |
| 0x00018 | `m1C` | `mBlueChargeTime` | daBemos_c::blue_eye_charge_init 0232666C: mChargeTime (mChargeCount 0) when the charge starts from blue_eye_wait; blue_eye_charge fires once mChargeCount passes it (tww the same) (3) |
| 0x0001A | `m1E` | `mBlueChargeResume` | daBemos_c::blue_eye_charge_init 0232666C: mChargeCount and mChargeTime when the charge starts from any other state (tww the same) (7) |
| 0x0001C | `m20` | `mRedHeadOpenStep` | daBemos_c::red_eye_charge 023273F0: the cLib_chaseF step of mHeadScale.x toward 1 (y a quarter of it) while the player is in range (tww the same) (0.1) |
| 0x00020 | `m24` | `mRedHeadCloseStep` | daBemos_c::red_eye_charge 023273F0: the cLib_chaseF step of mHeadScale.x toward 0 (y a quarter of it) once the player is out of range (tww the same) (0.07) |
| 0x00024 | `m28` | `mRedChargeTime` | daBemos_c::red_eye_charge_init 0232712C: mChargeTime (mChargeCount 0) when the charge starts from red_eye_wait; red_eye_charge fires once mChargeCount passes it (tww the same) (0) |
| 0x00026 | `m2A` | `mRedChargeResume` | daBemos_c::red_eye_charge_init 0232712C: mChargeCount and mChargeTime when the charge starts from any other state (tww the same) (0) |
| 0x00028 | `m2C` | `mRedRangeInRangeAdd` | daBemos_c::red_range_check 02326460: added to the search range while the player is already in range (mInRange) (tww the same) (0) |
| 0x0002C | `m30` | `mBlueWidthInRangeAdd` | daBemos_c::blue_range_check 02326274: added to mBlueSearchWidth while the player is already in range (mInRange) (tww the same) (0) |
| 0x00030 | `m34` | `mBlueRangeInRangeAdd` | daBemos_c::blue_range_check 02326274: added to mSearchRange while the player is already in range (mInRange) (tww the same) (0) |
| 0x0003C | `m40` | `mRedTurnStep` | daBemos_c::red_eye_search 02327968: the largest step of the head's yaw toward the player (cLib_addCalcAngleS2) (tww the same) (400) |
| 0x0003E | `m42` | `mRedFireTime` | daBemos_c::red_eye_search_init 023273C0: mFireTimer at the start of the search (tww the same) (30) |
| 0x00040 | `m44` | `mGuardRadius` | daBemos_c::guard_proc 02325DB8: the guard cylinder's radius; the cylinder sits it - 20 to the player's side (tww the same) (200) |
| 0x00044 | `m48` | `mGuardHeight` | daBemos_c::guard_proc 02325DB8: the guard cylinder's height (100 while the player crouches) and half of it is taken off its y (tww the same) (100) |

Open (no name yet): `m0C` (900), `m10`, `m38`, `m3C`, `m4C` (each set by the constructor only and read nowhere in WWHD or tww); padding `_pad1`, `_pad49`.

## daFire_c (fire / torch flame object; d_a_fire.h)

Old names are the GameCube offsets (zeldaret/tww `field_0x...`, unnamed there).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3E8 | `field_0x2CC` | `mTgCyls` | CreateInit 0213E930: Set(l_cyl_src) (all three for shape 1 with radius 50, else [0]); ctrlEffect 0213F078 reads [0]'s Tg hit (wind / water / ice); checkCol 0213FA58 places and sets [0] (shape != 1); execStopFire / execPlayFire toggle [0]'s At (tww Set(l_cyl_src)) |
| 0x778 | `field_0x65C` | `mCoCyl` | CreateInit 0213E930: Set(l_co_cyl_src); checkCol 0213FA58 places it at current.pos and sets it; execStopFire 0213E8C0 / execPlayFire 0213EF18 toggle its Co (tww Set(l_co_cyl_src)) |
| 0x8A8 | `field_0x78C` | `mAtCyl` | CreateInit 0213E930: Set(at_cyl_src); checkCol 0213FA58 places it at current.pos and sets it; execStopFire 0213E8C0 / execPlayFire 0213EF18 toggle its At (tww Set(at_cyl_src)) |
| 0x9D8 | `field_0x8BC` | `mpEmitter0` | _create 0213F3D0: particle 0x461 at current.pos; emitter(0): execStopFire / execPlayFire / setDirParticle 0213EF88 / CreateInit (shape 2 scale) drive it; demo_proc 0213F654 checks its live particles |
| 0x9DC | `field_0x8C0` | `mpEmitter1` | _create 0213F3D0: particle 0x462 at current.pos; emitter(1), driven like mpEmitter0 |
| 0x9E0 | `field_0x8C4` | `mpEmitter2` | _create 0213F3D0: particle 0x445B (group 4) at current.pos; emitter(2), driven like mpEmitter0 |
| 0x9EE | `field_0x8D2` | `mStopTimer` | ctrlEffect 0213F078 sets 80 on a water / ice hit (At type 0x80100) and keeps the fire stopped while it counts down (tww the same) |
| 0x9F0 | `field_0x8D4` | `mParticleDir` | CreateInit 0213E930 sets (0, 1, 0); setDirParticle 0213EF88 chases it toward the given direction and copies it to each emitter's mDirection |
| 0x9FC | `field_0x8E0` | `mWindDir` | CreateInit 0213E930 sets (0, 1, 0); ctrlEffect 0213F078 sets it from a wind hit's vector (normalised, y from the XZ length) and eases it back to (0, 1, 0), then passes it to setDirParticle |
| 0xA08 | `field_0x8EC` | `mWasStopped` | CreateInit 0213E930: the initial stopped state (switch or treasure bit); ctrlEffect 0213F078 plays the on / off sound when the new stopped state differs, then stores it |
| 0xA09 | `field_0x8ED` | `mOffState` | eventOrder 0213F8C4: the last off state (treasure bit, switch, or no enemies left); a change starts the on (1) / off (2) event in mEventState |
| 0xA0C | `field_0x8F0` | `mWindId` | search_wind 0213EE34: process ID of actor 0x187 (or -1); ctrlEffect 0213F078 looks it up to ignore / stop on that actor's wind hit (tww the same) |
| 0xA10 | `field_0x8F4` | `mEventState` | eventOrder 0213F8C4: 1 / 2 order mOnEventIdx / mOffEventIdx; checkOrder 0213F7A4 clears it once the event started |
| 0xA14 | `field_0x8F8` | `mStaffId` | checkOrder 0213F7A4: dComIfGp_evmng_getMyStaffId by type; demo_proc 0213F654 uses it for getMyActIdx / cutEnd |
| 0xA18 | `field_0x8FC` | `mOnEventIdx` | CreateInit 0213E930: event index of "fire_on_cam" (type 0, else -1); eventOrder 0213F8C4 orders it for mEventState 1; checkOrder 0213F7A4 checks its start / end |
| 0xA1A | `field_0x8FE` | `mOffEventIdx` | CreateInit 0213E930: event index of "fire_off_cam(_ws)" (type 0) / "zenfire_cam(_ws)" (type 1), else -1; eventOrder 0213F8C4 orders it for mEventState 2; checkOrder 0213F7A4 checks its start / end |
| 0xA1D | `field_0x901` | `mEventDelay` | CreateInit 0213E930 sets 30; _execute 0213FAF4 counts it down to -1; eventOrder 0213F8C4 starts on / off events only once it is negative |
| 0xA1E | `field_0x902` | `mFireOn` | CreateInit 0213E930: 1 unless the switch is on (then execStopFire); ctrlEffect 0213F078 runs only while set; demo_proc 0213F654 sets 1 / 0 for the on / off cuts (tww the same) |
| 0xA1F | `field_0x903` | `mNoEnemyTimer` | eventOrder 0213F8C4, type 1: reset to 5 while an enemy is in the room; when it runs out the fire turns off |

Open: `field_0x8D1` (ctrlEffect clears it and tests it with mStopTimer; nothing sets it non-zero in WWHD or tww), the padding `field_0x8EE`.

## daMbdoor_c (barred double door; d_a_mbdoor.h)

Old names are tww's (`field_0x...`, the GameCube offsets, unnamed there; the header carried short role comments, now in the evidence). Named from the uses in d_a_mbdoor.cpp.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3C8 | `field_0x2ac` | `mBgRegistered` | CreateInit 021C0AFC sets 1 once mpBgW is registered; daMbdoor_actionCloseWait 021C07FC sets 0 when it releases mpBgW for the door event (tww the same); not read in WWHD; tww's demo-version daMbdoor_Delete releases mpBgW only while it is set (the retail one, like WWHD 021C0AA0, tests the heap) |
| 0x3C9 | `field_0x2ad` | `mOpenSide` | checkArea 021C06D8: 1 when the player stands on the +lateral side, else 0; calcMtx 021BF854 swings the door to that side (mDoorAngle or its negative) and places mPlayerGoalPos there; demoProc 021C00C4 uses it for the goal; execute 021C0984 sets 0 for a demo actor |
| 0x3CC | `field_0x2b0` | `mBarAngle` | calcMtx 021BF854: Z rotation of the bar (lock) model; CreateInit 021C0AFC: 0 when locked, -0x3F65 when open; demoProc 021C00C4 ACT_STOP_OPEN raises it to -0x3F65 by mAngleSpeed; execute 021C0984 takes a demo actor's mRotate.z |
| 0x3CE | `field_0x2b2` | `mDoorAngle` | calcMtx 021BF854: Y rotation of the door wings; demoProc 021C00C4 ACT_OPEN opens it to -0x1C71 by mAngleSpeed; CreateInit 021C0AFC sets 0; execute 021C0984 takes a demo actor's mRotate.y |
| 0x3D0 | `field_0x2b4` | `mAngleSpeed` | demoProc 021C00C4: cleared by ACT_OPEN / ACT_STOP_OPEN, then grows each frame as the step of mDoorAngle (ACT_OPEN) or mBarAngle (ACT_STOP_OPEN) |
| 0x3D2 | `field_0x2b6` | `mBarOpening` | demoProc 021C00C4: ACT_STOP_OPEN sets 1 and moves mBarAngle only while it is set; cleared when the bar reaches -0x3F65 |
| 0x3D8 | `field_0x2bc` | `mUnlockDelay` | CreateInit 021C0AFC sets 30; checkUnlock 021BFF14 (type 1): counts it down after the room's enemies are gone and turns the switch on only at 0 |
| 0x3E0 | `field_0x2c4` | `mPlayerGoalPos` | calcMtx 021BF854: the point beside the opening door wing (on mOpenSide); demoProc 021C00C4 makes it the player's goal (setPlayerGoal / ACT_SET_GOAL) and checks the player's distance to it |

Open: none besides the padding.

## daSalvage_c (salvage point manager; d_a_salvage.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`, unnamed there). Named from the uses in d_a_salvage.cpp.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x404 | `m2E8` | `mPrevRoomNo` | CreateInit 02465914 stores the start room; _create 02465A8C, on a room change, stores the old mNowRoomNo (tww getSRoomNo()); never read in WWHD or tww |
| 0x405 | `m2E9` | `mOrderType` | proc_wait 02467364: 1 / 2 get item (crane left / right), 3 / 4 nothing found (left / right); eventOrder 024662F0 orders the event it selects; checkOrder 02465EC4 starts the salvage for the matching event and clears it |
| 0x406 | `m2EA` | `mInitDone` | CreateInit 02465914 sets 1 (tww true); _delete 02465C68 stops mEmitter and cleans up only while it is set (tww the same) |
| 0x408 | `m2EC` | `mTboxId` | checkOrder 02465EC4: process ID of the fpcNm_SALVAGE_TBOX_e chest it creates at the crane top; checks it exists after mSearchWait and deletes it at the end; proc_wait_init 02465E9C sets -1 |
| 0x418 | `m2FC` | `mShowDelay` | onSalvageForOship 024679F4 sets 90 when it moves a point under the ship; calcAlpha 02466578 counts it down outside events and keeps kind 2 points hidden while it is not 0 (tww the same) |
| 0x41D | `m301` | `mHasSbox` | checkOrder 02465EC4: 1 when the fpcNm_SBOX_e box was created with the chest (get item events), 0 for the nothing-found events and after the cleanup; the existence check and cleanup include mSboxId only while set |
| 0x41E | `m302` | `mSearchWait` | checkOrder 02465EC4 sets 99 when the salvage starts and counts it down; at 0 it checks that mTboxId / mSboxId still exist and resets the event otherwise |
| 0x420 | `m304` | `mSboxId` | checkOrder 02465EC4: process ID of the fpcNm_SBOX_e box created at the crane top (or -1); checked and deleted with mTboxId |

Open: none besides the padding.

## daObjLeaves_c (leaf pile; d_a_obj_leaves.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`, unnamed there). Named from the uses in d_a_obj_leaves.cpp (the delays and steps are copied from the l_effect entry, whose member names this decomp already had).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x524 | `m408` | `mAlpha` | _create 0236A554 sets 255; item_set_wait_proc 0236ACF8 / retire_wait_proc 0236ADFC add mAlphaSpeed each frame (item at <= 200, delete at 0); _draw 0236A884 uses it as the model opacity (translucent below 255) |
| 0x528 | `m40C` | `mAlphaSpeed` | birthEffect 02369D08: the l_effect entry's mAlphaStep; item_set_wait_proc 0236ACF8 / retire_wait_proc 0236ADFC add it to mAlpha |
| 0x54C | `m430` | `mFadeWait` | birthEffect 02369D08: the l_effect entry's mFadeDelay; _execute 0236A7E8 counts it down; alpha_calc_start_wait_proc 0236ACCC starts the fade (LeavesItemWait) at 0 |
| 0x550 | `m434` | `mRetireWait` | birthEffect 02369D08: the l_effect entry's mRetireDelay; _execute 0236A7E8 counts it down; retire_wait_proc 0236ADFC deletes the actor only once it is 0 |
| 0x554 | `m438` | `mEffectKind` | birthEffect 02369D08: the l_effect entry's mKind (0 particle, 1 smoke callback); not read in WWHD; tww's demo-version _delete removes the smoke callback only for kind 1 |
| 0x558 | `m43C` | `mAppear` | _create 0236A554: chk_appear's result (kept after the first phase); resources are loaded only while set; _delete 0236A74C deletes the resources, smoke callback and background only while set (tww the same) |
| 0x564 | `m44C` | `mBlown` | tg_hitCallback 0236A18C sets 1 on a wind, bomb or fire hit (with the blow / burn effects); wait_proc 0236ABD0 sets the switch, releases the background and starts the fade (LeavesAlphaWait) once it is set |

Open: none besides the padding.

## daMdoor_c (cage / stop door; d_a_mdoor.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`, unnamed there). Named from the uses in d_a_mdoor.cpp.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3AC | `m290` | `mSmokeCallback` | smokeInit 021C10C4: the dPa_smokeEcallBack of the ID_AK_JT_ELEMENTSMOKE00 landing smoke; create 021C1E8C constructs it, smokeEnd 021C19FC ends it (tww dPa_smokeEcallBack) |
| 0x3DC | `m2C0` | `mStaffId` | daMdoor_actionDemoWait 021C1418 ("Mori1") / daMdoor_actionReady 021C17A4 ("SHUTTER_DOOR"): dComIfGp_evmng_getMyStaffId; demoProc 021C1174 reads the action and advance of that staff |
| 0x3E0 | `m2C4` | `mDelayTimer` | CreateInit 021C1AFC sets 1 (type 1); daMdoor_actionGenocide 021C1514 counts it down after the room's enemies are gone before ordering the event; daMdoor_actionReadyOpen 021C188C sets 20 and daMdoor_actionOpen 021C15B0 counts it down before the door rises |
| 0x3E2 | `m2C6` | `mEventIdx` | CreateInit 021C1AFC: event index of "MORI1_EVENT" (type 0) or "DEFAULT_STOP_OPEN" (types 1 / 2); order and the demo / open actions start and end-check it |
| 0x3E4 | `m2C8` | `mToolId` | CreateInit 021C1AFC: 0xFF, or getToolId() for type 2; order passes it to fopAcM_orderOtherEventId; it selects ACT_SWITCH8_e / ACT_SWITCH12_e |
| 0x3E6 | `m2CA` | `mDemoFlags` | demoProc 021C1174: bit 0 set when the close action starts, cleared when the door lands (then the debris and smokeInit) |
| 0x3E8 | `m2CC` | `mHeight` | calcMtx 021C0EAC: the model's height above current.pos; CreateInit 021C1AFC: 0 (closed) or 280 (open); demoProc 021C1174 / daMdoor_actionOpen 021C15B0 chase it to 0 / 280 by speedF |

Open: none besides the padding.

## daShutter_c (sliding shutter; d_a_shutter.h)

Old names are tww's (`field_0x...`, the GameCube offsets, unnamed there). Named from the uses in d_a_shutter.cpp.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x43C | `field_0x320` | `mEventOrder` | demo 02484B10: 1 (open) / 2 (close) when the switch changes after mFrameTimer; orders mOpenEventIdx / mCloseEventIdx while set and clears it once that event starts (tww the same) |
| 0x455 | `field_0x339` | `mActionStarted` | shutter_move 0248478C: set on the first frame of ACT_OPEN (with the TOTG open sound) and ACT_CLOSE, cleared by ACT_WAIT02 when its timer ends (tww the same) |
| 0x456 | `field_0x33A` | `mOpenFrame` | shutter_move 0248478C: ACT_WAIT clears it, ACT_OPEN counts it up; at 75 the TOTG shutter plays the stop sound and a shock (tww the same) |

Open: `field_0x328` (CreateInit sets 3 / 2 by the switch; never read in WWHD or tww), `field_0x334`, `field_0x33B` (no use; tww has none either).

## daObjDrift::Act_c (drifting raft; d_a_obj_drift.h)

Old names are tww's (`field_0x...`, the GameCube offsets, unnamed there).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x5E0 | `field_0x4C8` | `mLastPrevY` | set_mtx 023384F0 stores mPrevPos.y before mPrevPos takes the new matrix position (the previous mLastPrevY moves to mOlderPrevY); Create 023388B0 sets current.pos.y; never read in WWHD or tww (tww the same writes) |
| 0x5E4 | `field_0x4CC` | `mOlderPrevY` | set_mtx 023384F0 takes mLastPrevY before it is updated (the y one step further back); Create 023388B0 sets current.pos.y; never read in WWHD or tww (tww the same writes) |

Open: `field_0x49F` (no use), `field_0x4B0` (Create sets (5, 5, 5); never read), `field_0x4D0` (set to 1 when mRotTimer passes 10; never read), `field_0x4D4` (Create clears it; never read) (tww the same).

## daDai_c (figurine / item pedestal; d_a_dai.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`, unnamed there). The header declares several fields per line, so each renamed field has its own comment line above the declaration.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x95C | `m844` | `mCanTalk` | proc 02116648: 1 while the player is within 100 (XZ, |dy| < 10) and faces the stand; eventOrder 02116750 offers the talk / item talk only then (tww the same) |
| 0x962 | `m84A` | `mSelectItem` | XyCheckCB 02115E7C stores the X / Y item when it can go on the stand; checkOrder 02116470 refuses (talk) unless it is a daiza item; next_msgStatus 02116848 clears it when the item is taken back |
| 0x963 | `m84B` | `mPlacedItemNo` | checkOrder 02116470: the mItemNo of the daStandItem_c the player puts down (mStandItemId), saved into the stand's event register when the DAI_ITEM event ends |
| 0x964 | `m84C` | `mTalkPending` | checkOrder 02116470 sets 1 for a plain talk (or an item it cannot take); proc 02116648 runs talk(1) while it is set and clears it (and resets the event) when the box closes |
| 0x968 | `m850` | `mStandItemId` | CreateInit 02116054: process ID of the fpcNm_STANDITEM_e created for the saved item (or -1); checkOrder 02116470 takes the item the player holds when DAI_ITEM starts and moves it onto the stand at the end; next_msgStatus 02116848 deletes it when taken back |

Open: the padding `m84E`.

## daWbird_c (Wind Waker wind-direction bird; d_a_wbird.h)

Old names are tww's (`field_0x...`, the GameCube offsets, unnamed there). Named from the uses in d_a_wbird.cpp.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B9 | `field_0x29D` | `mStartWindSe` | actionSelect 024DE55C sets it when the wind direction is decided; actionMove 024DE768 then plays JA_SE_TAKT_WIND_DEMO once and clears it (tww the same) |
| 0x3BA | `field_0x29E` | `mTimer` | CreateInit 024DE9F0 clears it; actionSelect 024DE55C counts it up (the wind window opens at 10); setStartPos 024DE334 sets 90 and actionMove 024DE768 counts it down: the bird moves while it is above 30, the tact wind starts at 20, the move ends at 0 |
| 0x3BC | `field_0x2A0` | `mAccelY` | setStartPos 024DE334 sets 1.0; actionMove 024DE768 adds it to speed.y every moving frame (tww the same) |

Open: the padding `field_0x290`.

## daObjTry::Act_c: offset-name union aliases removed (d_a_obj_try.h)

As in the alias batch (2026-11-10, docs/field-names/libraries.md): each of these unions paired tww's GameCube offset name with a member of exactly the same type that the port had already named; the alias goes, its uses (d_a_obj_try.cpp) move to the named member, and the union becomes that member.

| WWHD offset | Removed alias | Named member | Uses moved |
|---|---|---|---|
| 0x7B8 | `field_0x63C` | `correctPosition` | 6 |
| 0x7C4 | `field_0x648` | `correctYaw` | 1 |
| 0x7C8 | `field_0x64C` | `correctEnabled` | 3 |
| 0x7C9 | `field_0x64D` | `correctImmediate` | 5 |

## daYkgr_c (d_a_ykgr.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`, unnamed there).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3E8 | `m2CC` | `mAimRate` | daYkgrExecute 024EAAE8 eases it toward the global m_aim_rate and blends it half and half with mPosRate into the particle callback parameter (between l_HIO m14 and m18); _create 024EA6A0 sets 0.5 (tww the same) |
| 0x3EC | `m2D0` | `mPosRate` | daYkgrExecute 024EAAE8 eases it toward getPosRate() and blends it with mAimRate into the particle callback parameter; _create 024EA6A0 sets 0 (tww the same) |

Open: the padding `m2C8`.

## daObj_Otble::Act_c (table; d_a_obj_otble.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`, unnamed there). The header has no offset prefixes, so each renamed field has its own comment line above the declaration.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B0 | `m294` | `mVariant` | _create 023811A0: param bits 0-7 clamped to 0 / 1; _createHeap 02380D28 picks the bdl (OTABLE / OTABLEL) and dzb by it, _create the heap size, CreateInit 02380F8C the cull box (tww the same) |
| 0x3C0 | `m2A4` | `mBgMtx` | _createHeap 02380D28: the model matrix that mBgW is Set with (dBgW::MOVE_BG_e); set_mtx 02380E74 copies the new matrix into it each frame before mBgW->Move() (tww the same) |

## daMmusic::Act_c (music note particles; d_a_mmusic.h)

Old names are tww's (`field_0x...`, the GameCube offsets, unnamed there).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B4 | `field_0x298` | `mMusicPlaying` | _create 021C5EE4 and _execute 021C612C store Macore_is_playing(); _execute creates the ID_AK_SN_MACOREMUSIC00 note emitter only once it is 1, then clears it (tww the same) |
| 0x3B8 | `field_0x29c` | `mEmitting` | _execute 021C612C / manage_se 021C6008: 1 while the note emitter creates particles; stopCreateParticle sets 0 (culled, or no practice song), playCreateParticle sets 1 (tww the same) |

## daTagWaterlevel::Act_c (water-level schedule tag; d_a_tag_waterlevel.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3AC | `field_0x290` | `mLevelStep` | _execute 024B3AC0: the step of the water level ease (cLib_addCalc of M_now): grows by attr 0x00 each frame, clamped to attr 0x0C .. 0x08, reset to 0 once the level is reached; _create 024B398C sets 0 (tww the same) |

Open: the cpp-local attr fields (`field_0x00` .. `field_0x0c`, not in this header).

## daMozo_c (Mozo statue; d_a_mozo.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x54E | `field_0x376` | `mType` | CreateInit 021D4B14: parameter byte 3 clamped to 0..2 (0xFF -> 0); wait_proc 021D53D8 / towait_proc 021D5E08 start the beam (MOZO_BEAM / MOZO_BEAM_ALT) or fire (MOZO_FIRE) search by it; checkRange 021D5030 picks the l_HIO child block by it; daMozo_nodeCallBack 021D3DF0 moves the joints only up to MOZO_FIRE (tww the same switch) |

## daObjHbrf1::Act_c (rising lift; d_a_obj_hbrf1.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3FC | `m2E4` | `mEventOrdered` | daObjHbrf1_down_stop 02351278 / daObjHbrf1_up_stop 02351580 set 1 when they order the lift demo event (first time or by the event parameter); daObjHbrf1_up_demo 0235146C / daObjHbrf1_down_demo 02351798 reset the event at the end of the move only while it is set, then clear it; Create 0235106C clears it (tww the same) |

## daObjBuoyflag::Act_c: offset-name union aliases removed (d_a_obj_buoyflag.h)

As in the alias batch (2026-11-10): the unions paired tww's GameCube offset names with already named members of the same type; the aliases go and their uses (d_a_obj_buoyflag.cpp) move to the named members.

| WWHD offset | Removed alias | Named member | Uses moved |
|---|---|---|---|
| 0x3218 | `m1124` | `mMode` | 2 |
| 0x321C | `m1128` | `mbInit` | 3 |

## daOship_c (pirate / enemy ship; d_a_oship.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3F0 | `m2D4` | `mPathPntNo` | pathMove 023C7084 passes it to dLib_pathMove (OshipPathMoveHD) as the path point index the ship moves toward, which the call advances (tww dLib_pathMove(&mOrigPos, &it, mpPath, ...) the same) |

Open: daOship_HIO_c `field_0x8E`, `field_0x90` (only the HIO constructor sets them; no read in WWHD or tww).
## daPds_c (tower pedestal; d_a_pedestal.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_pedestal.cpp, which keeps the GameCube offset names) and our decomp's use; the brk fields follow daNpc_Os_c (npcs.md), whose initBrkAnm / playBrkAnm have the same shape.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x480 | `unk308` | `mBrkFrame` | initBrkAnm 023C85F8 stores mBrk's frame after the init (end frame for a reversed table entry); playBrkAnm 023C8D18 stores the frame after each play; nothing reads it (tww the same) |
| 0x486 | `unk30E` | `mBrkAnmIdx` | initBrkAnm 023C85F8 stores the brkAnmTbl index it initialised; nothing reads it (tww the same); after daNpc_Os_c::mBrkAnmIdx (same initBrkAnm code) |
| 0x487 | `unk30F` | `mBrkAnmEnd` | playBrkAnm 023C8D18 stores mBrk's play() result (animation end); nothing reads it (tww the same); after daNpc_Os_c::mBrkAnmEnd (same playBrkAnm code) |

Open: `unk310`, `unk312`, `unk314`, `unk316` and `unk31C` (0x488..0x48E, 0x494: only cleared by setAction 023C8FBC, tww the same, no other use).

## daIball_c and IbRipple (d_a_ib.h)

Open only: `m670`, `m678`, `m682` (0x928, 0x930, 0x93A) are unused byte gaps (no use in WWHD or tww). IbRipple (the HD-only 0x90 object at 0x894, mRippleCb) `field_0x48`, `field_0x6c`, `field_0x70`, `field_0x74`, `field_0x78` are only zeroed by ibConstructor 0217B1F8; tww's mRippleCb is a plain dPa_rippleEcallBack, so there is no counterpart.

## daLlift_c (Forbidden Woods leaf lift; d_a_leaflift.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_leaflift.cpp, which keeps the GameCube offset names) and our decomp's use.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x558 | `m43C` | `mbPlayerRiding` | rideCallBack 021B3028 sets 1 when the rider is the player (fopAcM_GetName 0xA8); _execute 021B3D04 resets the tilt and lets the lift sink only while it is 0, then clears it every frame (tww the same) |
| 0x559 | `m43D` | `mbRiseWobble` | MoveUpLift 021B4090 sets 1 while the lift is still below home.y + 560; rideCallBack 021B3028 then uses a random sine wobble (mActorLifetimeFrameCount << 11) as the tilt factor instead of 2, and clears it after each call (tww the same) |
| 0x55A | `m43E` | `mbDescendReady` | set to 1 by _execute 021B3D04 when mStartDelay runs out and by rideCallBack 021B3028 when the player rides; while set and not mbPlayerRiding, _execute resets the tilt and starts the descent (mbIsDescending) once the player is 270 away (tww the same) |
| 0x55B | `m43F` | `mStartDelay` | CreateInit 021B35F4 sets 30; _execute 021B3D04 counts it down and sets mbDescendReady when it reaches 0 (tww the same) |

Open: `m442`, `m446` and `m46A` (unused byte gaps, no use in WWHD or tww).

## daObjRflw_c (Rito flower; d_a_obj_rflw.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_rflw.cpp, which keeps the GameCube offset names) and our decomp's use; mHitTimer is the name the unit spec (tools/verify/units/d_a_obj_rflw.txt) already used.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x524 | `m408` | `mSwingAngle` | _execute (in daObjRflw_Execute 02388064): sin(mSwingPhase) * (0x100 - mHitTimer) * 4 (first 8 frames) or * 2, 0 at the end; nodeCallBack 02387A74 rotates each joint by it about Y (tww the same); CreateInit 02387DA8 clears it |
| 0x526 | `m40A` | `mSwingPhase` | _execute (in daObjRflw_Execute 02388064): 0 on a hit, + 0x1000 per frame for the first 8 frames then + 0x800; the sine argument of mSwingAngle (tww the same) |
| 0x528 | `m40C` | `mHitTimer` | _execute (in daObjRflw_Execute 02388064): 0 on a hit, + 1 per frame while mIsSwinging, ends the swing above 0x100; a new hit counts only above 0x38; damps mSwingAngle by (0x100 - it) (tww the same); the unit spec d_a_obj_rflw.txt already calls 0x528 mHitTimer |
| 0x52A | `m40E` | `mSwingDirAngle` | _execute (in daObjRflw_Execute 02388064): the angle from the colliding actor to the flower (cM_atan2s) - 0x4000 on a hit; nodeCallBack 02387A74 rotates X by it, Y by mSwingAngle, X back by -it, so the flower swings away from the hitter (tww the same) |
| 0x52C | `m410` | `mIsSwinging` | _execute (in daObjRflw_Execute 02388064): 1 on a cylinder co-hit (with the JA_SE_OBJ_TREE_SWING_S sound), 0 when mHitTimer passes 0x100; the swing runs while set; _create (in daObjRflw_Create 02387E5C) clears it (tww the same); the unit spec calls 0x52C "swinging" |

Open: `m411` (unused byte gap).

## daBFlower_c (bomb flower; d_a_bflower.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_bflower.cpp, which keeps the GameCube offset names) and our decomp's use (d_a_bflower.cpp). TP has no such actor.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x754 | `m55C` | `mBck1AnmIdx` | init_bck_anm 02077530 plays the mBck1 resource index only while it is -1 and stores it; animPlay 02077280 sets -1 when mBck1 ends; CreateInit 02076D94 -1 (tww the same, tww comment "Current bck1 anim?") |
| 0x860 | `m58C` | `mHasBomb` | set while the bomb fruit sits on the flower: _draw 0207717C draws mpModel2 (the bomb) only then; actLive 020775F8 clears it when it creates the bomb (hit or carry), sets 1 when it regrows; CreateInit 02076D94 1 (tww the same) |
| 0x861 | `m58D` | `mBombReady` | actLive 020775F8: 1 when the bomb grow animation mBck2 ends, 0 while mGrabbable is 1 or the last bomb still exists; sets/clears the carry attention flag and lets a carry create the bomb (tww the same) |
| 0x88C | `m5B8` | `mBombGrowing` | actLive 020775F8: set to mHasBomb && mBombScale still growing at the end; at the start, when set and the growth has just ended, init_bck_anm(VBAKH) sways the flower (tww the same); CreateInit 02076D94 0 |

Open: `m58E` (no use in WWHD or tww), `m594` (actDead 02077AE8 only writes 70 when the flower revives; never read).

## daAmiProp_c (rotating mesh propeller; d_a_amiprop.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_amiprop.cpp, which keeps the `unk_...` names) and our decomp's use (d_a_amiprop.cpp).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x528 | `unk_40C` | `mBgMtx` | CreateHeap 02050800 registers mpBgW (MOVE_BG) with this matrix; set_mtx 02050918 / setMoveBGMtx 02050788 copy the model (with scale) matrix into it (tww the same) |
| 0x558 | `unk_43C` | `mSwitchNo` | CreateInit 02050A04: daAmiProp_prm::getSwitchNo; _execute 02050D14 tests fopAcM_isSwitch(this, it) (tww the same) |
| 0x55C | `unk_440` | `mPrevSwitch` | _execute 02050D14: last frame's switch state; a change from it starts the turn, stored at the end (tww the same) |
| 0x560 | `unk_444` | `mTurning` | _execute 02050D14: set when the switch state changes; while set, angle x or y (by type) eases to mTargetAngle; cleared when the turn is done (tww the same) |
| 0x562 | `unk_446` | `mTargetAngle` | CreateInit 02050A04: angle.x + 0x4000; _execute 02050D14 eases angle x / y to it (cLib_addCalcAngleS) and adds 0x4000 when reached (tww the same) |

## dDoor_info_c (door base; d_door_info.h; copies in d_door_key.h, d_door_checks.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_door10.cpp / d_a_door12.cpp / d_a_kddoor.cpp / d_a_knob00.cpp, which keep the GameCube offset names) and our decomp's use. The copies in d_door_key.h and d_door_checks.h keep the old names for now.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3BD | `m2A1` | `mStopOpenTimer` | door10 / door12 / kddoor chkStopOpen (door10 02126D8C): 65 while enemies remain, counted down once the room is clear, the stop bars open only at 0; chkStopClose sets 65; dDoor_info_c__ct 0252A244 0 (tww the same) |
| 0x3E2 | `m2C6` | `mEventKind` | index into mEventIdx / mToolId (event table DEFAULT_STOP_OPEN .. SHUTTER_DROP_CARRY): door10 setEventPrm 02127080 2/3 (+2, 6), knob00 setEventPrm 021A4318 7/8/9; initOpenDemo 0252A2DC tests != 9 (tww the same) |

Open: `m2C7` (no use in WWHD or tww). The copies in d_door_checks.h / d_door_key.h take the class names in round 3.

Still open in objects (unchanged): daObj_Canon_c `field_0x2A0`, `field_0x2B4`, `field_0x448`; daObj_Canon_HIO_c `field_0x3A`, `field_0x48`; daComing2::Act_c `unk_2AA`, `unk_476`; daObjAjav::Act_c `m884`, `mC25`, `mC29` (padding); daObjWood::Act_c / daObjMsdan2::Act_c `field_0x290`; daIkari_c `m29E`; daTag_So_c `m299` (padding); Ji1EventTsuboHD `m688`..`m692` (view of daTsubo::Act_c, open there).

## daObj_Roten_c and daObj_Roten_HIO_c (Zunari's shop stalls; d_a_obj_roten.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_roten.cpp) and our decomp's use.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3ED | `field_0x2D1` | `mCreateFailed` | daObj_RotenCreate 02388B50 sets 1 and fails when the bag holds too few items for the stall type, else 0; daObj_RotenDelete 02388CC4 releases mpBgW and the resource only while 0 (tww the same) |

Open: daObj_Roten_c `field_0x29C` (gap, no use); daObj_Roten_HIO_c `field_0x08`, `field_0x0C` (set only by the HIO constructor 02388DB0, never read in WWHD or tww).

## daObj_Tousekiki_c (catapult on the pirate ship; d_a_obj_tousekiki.h; view daObjPirateship_ActView)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3E4 | `m2CC` | `mAppeared` | view of daObjPirateship::Act_c; same name as daObjPirateship::Act_c::mAppeared (d_a_obj_pirateship.h, HD 0x3E4); _draw (in daObj_TousekikiDraw 023A6304) draws only once it is set (tww the same) |

Open: daObj_Tousekiki_c `m2A0` (written only: the model matrix copy, never read in WWHD or tww), `m2D0` (only set 0xFFFF by CreateHeap 023A5CCC).

## daObjGnntakis_c (Tower of the Gods waterfall demo; d_a_obj_gnndemotakis.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x42C | `m2B0` | `mAnmStarted` | _create 0234BA3C FALSE; _execute 0234BB78 (g2before running): TRUE when the demo frame reaches 0x11D9 and the btk starts (rate 1); once set, deletes the actor at 0x126F (tww the same) |

## daLbridge_c (Tower of the Gods light bridge; d_a_lbridge.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x558 | `unk31C` | `mDemoState` | holds DemoState: appear_bridge 021B2880 APPEAR_DEMO, disappear_bridge 021B2A38 DISAPPEAR_DEMO; demo 021B2C8C orders that event and back to NO_DEMO when it starts (tww the same) |

## daDekuItem_c (Deku Leaf item; d_a_deku_item.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x844 | `unk630` | `mGetEventOrder` | mode_getdemo_init 0211F0CC sets 1; eventOrder 0211EE80 orders the get-item event while 1; checkOrder 0211ED7C clears it when the event starts; mode_getdemo_wait 0211F134 creates the item once 0 (tww the same) |

## daWindMill_c (windmill; d_a_windmill.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x1360 | `m1244` | `mCpsShape` | set_at 024E179C: per blade the capsule start / end (model matrix) and radius 170 (large) / 70 (small), handed to mCps[i] (tww cM3dGCpsS m1244, mCps[i].Set(m1244[i])) |

WindTagView `field_0x49c` takes daWindTag_c's name `mHeightScale` (round 3, view follows its class).

## daObjMkiek::Act_c (light-sensitive wall; d_a_obj_mkiek.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_mkiek.cpp, which keeps the GameCube offset names) and our decomp's use.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x5D0 | `m458` | `mVanish` | demo_wait 02371CBC sets 1 when the wall breaks (vanish particles, switch on); Draw 0237205C then draws mpModelV with the brk animation instead of mpModel; Create 02371B3C 0 (tww the same) |
| 0x5D8 | `m460` | `mFrameCount` | check 02371C14 counts frames of light / hit, the die event at 20, else 0; demo_wait 02371CBC counts on to 20 before breaking; Create 02371B3C 0 (tww the same) |

## daObjComing::Act_c (floating barrel spawner; d_a_obj_coming.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_obj_coming.cpp, which keeps the GameCube offset names) and our decomp's use. `mType` would clash with PrmBarrel2_c::mType in the same files, so m290 is `mPrmType`.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3AC | `m290` | `mPrmType` | _create 02331B98: parameter bits 0-3 (PRM_TYPE, tww prm_get_type()); _execute 02331C94 picks mode_proc[it][mode] (tww the same) |
| 0x3B0 | `m294` | `mCtrl` | the five Ctrl_c barrel controllers: _create 02331B98 inits them, _execute 02331C94 counts their timers and runs their mode (tww the same) |

## daTagKbItem_c (pig-dig item tag; d_com_static.h; second declaration in d_a_tag_kb_item.h)

Named from zeldaret/tww's code (d217e2b, src/d/d_com_static.cpp, field_0x298 / field_0x299) and our decomp's use (d_com_static.cpp). The copy in d_a_tag_kb_item.h takes the same names in round 3; it kept the old names before (same names suggested there).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B4 | `field_0x298` | `mDigDone` | daTagKbItem_c::dig_main 02526330: returns at once while set; set 1 once the item was dropped or the enemy kind handed to the pig (tww the same) |
| 0x3B5 | `field_0x299` | `mDigRequest` | kb_dig 02526560 sets 1 (with the pig mpActor) and calls dig_main; dig_main 02526330 does nothing unless it is set and clears it at the end (tww the same) |

ComStatic::StandItem `m630`, `m660`, `m6B4`: named with daStandItem_c in round 3 (mBaseJointMtx / mHeadJointMtx / mSpinAngle).

## daWarpf_c (boss warp flower; d_a_warpf.h)

Old names are the GameCube offsets (zeldaret/tww d217e2b, include/d/actor/d_a_warpf.h `mXXX`); WWHD runs the same code as tww src/d/actor/d_a_warpf.cpp.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3AC | `m290` | `mpBstWarpEmitter` | set_effect 024D6940: on the TOTG stage the emitter of ID_AK_SN_BSTWARP00 + 3, created once; _delete 024D717C invalidates it (tww the same) |
| 0x3B0 | `m294` | `mpKirakiraEmitter` | set_effect 024D6940: on the boss stages the emitter of ID_AK_SN_BOSSWARPKIRAKIRA00, created once; initWait 024D7BCC / _delete 024D717C invalidate it (tww the same) |
| 0x3B4 | `m298` | `mpLightEmitter` | set_effect 024D6940: on ET / WT before the end demo the emitter of ID_IT_SN_SHINDENTF_LIGHT00; initWait 024D7BCC / _delete 024D717C invalidate it (tww the same) |
| 0x3B8 | `m29C` | `mpStarEmitter` | set_effect 024D6940: on ET / WT the emitter of ID_IT_SN_SHINDENTF_STAR00 (or ID_IT_SN_DEMO29_STAR00 after the end demo); initWait 024D7BCC / _delete 024D717C invalidate it (tww the same) |
| 0x3C4 | `m2A8` | `mpModel` | CreateHeap 024D6114 creates the main model (YSBWP00 / GTFGLOW00 / GDEMO29_A00); set_mtx 024D7258 places it, _draw 024D73B4 draws it with the brk / bck / btk (tww the same) |
| 0x3C8 | `m2AC` | `mpBrkAnm` | CreateHeap 024D6114: the main model's brk (YSBWP00 / GTFGLOW00 / GDEMO29_A01); anim_play 024D7A54 plays it, setEndAnim 024D6D94 sets its end frame, _draw 024D73B4 enters it (tww the same) |
| 0x3CC | `m2B0` | `mpBckAnm` | CreateHeap 024D6114: the TOTG model's bck (YSBWP00); anim_play 024D7A54 plays it, setEndAnim 024D6D94 sets its end frame, _draw 024D73B4 enters it (tww the same) |
| 0x3D0 | `m2B4` | `mpBtkAnm` | CreateHeap 024D6114: the TOTG model's btk (YSBWP00); anim_play 024D7A54 plays it, setEndAnim 024D6D94 sets its end frame, _draw 024D73B4 enters it (tww the same) |
| 0x3D4 | `m2B8` | `mpBrkAnm1` | CreateHeap 024D6114: the second brk (GTFGLOW01, loop, rate 0); set_effect 024D6940 / anim_play 024D7A54 (from frame 120 of mFrameCount) start it, _draw 024D73B4 enters it while its rate > 0 (tww the same) |
| 0x3D8 | `m2BC` | `mpBrkAnm2` | CreateHeap 024D6114: the third brk (GTFGLOW02, rate 0), NULL after the end demo; never played or entered in WWHD or tww |
| 0x3DC | `m2C0` | `mpEndModel` | CreateHeap 024D6114: on ET / WT after the end demo the second model GDEMO29_B00, else NULL; set_mtx 024D7258 places it, _draw 024D73B4 draws it (tww the same) |
| 0x3EC | `m2D0` | `mOrderRequest` | checkOrder 024D7628 sets 1 when check_warp_event finds the player in warp range and clears it once the event starts; eventOrder 024D79F4 orders the event mEventIdx while it is 1 (tww the same) |
| 0x3F2 | `m2D6` | `mEventIdx` | CreateInit 024D6EEC: event index of WARP_WIND (WARP_WIND_AFTER after the end demo); eventOrder 024D79F4 orders it, checkOrder 024D7628 startCheck / endCheck it (tww the same) |
| 0x3F4 | `m2D8` | `mSeState` | set_se 024D78D0: 0 plays the appear SE (JA_SE_CM_BST_WARP_BEAM / JA_SE_OBJ_BOSS_WARP_APPEAR) once and sets 1 (tww the same) |
| 0x3FC | `m2E0` | `mSeTimer` | set_se 024D78D0 counts it up to 20000; on TOTG the level SE mpLevelSe starts only from 45 (tww the same) |
| 0x400 | `m2E4` | `mFrameCount` | _execute 024D7B0C adds 1 each frame; actWait 024D7C98 / set_effect 024D6940 spawn wind every N frames of it, anim_play 024D7A54 starts mpBrkAnm1 from 120 (tww the same) |
| 0x404 | `m2E8` | `mpLevelSe` | _create 024D6FB8: the dLevelSe_c made by fopKyM_fastCreate(fpcNm_LEVEL_SE_e, ..), reverb set on it; set_se 024D78D0 dLevelSe_seStart it (tww the same) |
| 0x408 | `m2EC` | `mPlayerPos` | initWarpStart 024D7CE4 copies the player's current.pos; actWarpMode_1 024D7D38 / _2 024D7E64 / _3 024D7F68 put the wind effect at its y + 100 / 120 / 140 (tww the same) |

Open: `m2CC`, `m2D4` (padding arrays, no use in WWHD or tww).

## daHmlif_c (Tower of the Gods glowing lift; d_a_hmlif.h)

Old names are the GameCube offsets (zeldaret/tww d217e2b, include/d/actor/d_a_hmlif.h `mXXX`); names after the tww daHmlif_prm accessors where tww reads the field from one (getPathId, getSwitchNo, getSwitchNo2, getSmooth, getType). The header declares several fields per line (`m444, m445`; `m446, m447, m448`; `m488, m489`); each gets its own name.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x55C | `m444` | `mPathId` | Create 021762C8: params >> 8 (tww daHmlif_prm::getPathId()); set_next_pnt 02175908 / setNextPath 021757CC fetch the home path with it |
| 0x55D | `m445` | `mCurPathId` | Create 021762C8: mPathId, 255 when the room has no such path; move 02175DE4 / set_next_pnt 02175908 stop while 255; setNextPath 021757CC sets mPathId again on return (tww the same) |
| 0x55E | `m446` | `mPathDir` | Create 021762C8: +1 / -1 from the direction parameter (random for 2); set_next_pnt 02175908 adds it to mPointNo and reverses it at the ends of an open path; setNextPath 021757CC sets it (tww the same) |
| 0x55F | `m447` | `mSavedPathDir` | Create 021762C8 copies mPathDir; setNextPath 021757CC keeps mPathDir in it when switching to the next path, sets -1 / 1 at the path ends and restores mPathDir from it on return (tww the same) |
| 0x560 | `m448` | `mPointNo` | Create 021762C8: the start point (params >> 22 & 31); set_next_pnt 02175908 steps it by mPathDir (wraps / bounces) and loads mTargetPos from that point; setNextPath 021757CC sets 1 / mArg0 + dir (tww the same) |
| 0x568 | `m450` | `mTargetPos` | Create 021762C8: the start point; set_next_pnt 02175908: the position of point mPointNo; lift_normal_move 02175C34 / lift_smooth_move 02175AD0 move the lift toward it (tww the same) |
| 0x574 | `m45C` | `mPrevTargetPos` | Create 021762C8: the start position; set_next_pnt 02175908 keeps the old mTargetPos in it; lift_smooth_move 02175AD0 turns toward the angle from it to mTargetPos (tww the same) |
| 0x580 | `m468` | `mJunctionPos` | Create 021762C8: while mSwitchNo != 255, the position of path point mArg0 (the point where setNextPath 021757CC switches paths) (tww the same); never read in WWHD or tww |
| 0x58C | `m474` | `mSwitchNo` | Create 021762C8: params & 0xFF (tww daHmlif_prm::getSwitchNo()); chkSwitchPathType 021757B8: != 255; setNextPath 021757CC takes the next room path while the switch is on |
| 0x590 | `m478` | `mSwitchNo2` | CreateHeap 02175520: home.angle.z & 0xFF (tww daHmlif_prm::getSwitchNo2()); check_col 02175F48 turns this switch on when hit; anim_play 02175E60 / CreateHeap set mBtpFrame by it |
| 0x596 | `m47E` | `mSmoothMove` | Create 021762C8: params >> 31 (tww daHmlif_prm::getSmooth()); move 02175DE4 runs lift_smooth_move while set, else lift_normal_move |
| 0x598 | `m480` | `mMaxSpeed` | Create 021762C8: the speed table entry of params >> 16 & 15 (capped at 8), also speedF; lift_smooth_move 02175AD0 / lift_normal_move 02175C34 ease speedF to it (tww the same) |
| 0x59C | `m484` | `mMoveCount` | lift_normal_move 02175C34: 0 in MOVE_INIT / MOVE_CRUISE, +1 per frame while accelerating or decelerating (tww the same); never otherwise read in WWHD or tww |
| 0x5A0 | `m488` | `mMoveState` | lift_normal_move 02175C34: the MoveState (MOVE_INIT, MOVE_ACCELERATE, MOVE_CRUISE, MOVE_DECELERATE) it switches on and advances (tww the same) |
| 0x5A1 | `m489` | `mType` | daHmlifCreate 021753A8: params >> 27 & 15, 0 when >= 3 (tww daHmlif_prm::getType()); CreateHeap 02175520 / Create 021762C8 / check_col 02175F48 pick the arc, model, cull box and sphere by it |
| 0x5A2 | `m48A` | `mBtpFrame` | CreateHeap 02175520: 0, or 3 when mSwitchNo2 is on; anim_play 02175E60 steps it toward 3 (switch on) or 0 (off); Draw 021761F8 enters the btp at it (tww the same) |

Open: `m449`, `m47C`, `m47F` (padding, no use in WWHD or tww).
## daBoko_c (enemy weapons: Bokoblin stick, machete, mace, sword, spear; d_a_boko.h; view TnBoko in d_a_tn.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_boko.h) keeps the GameCube offset names for these fields; mRotAngleSpeed and mThrowAngle follow its setRotAngleSpeed(s16) / setThrow(s16), the rest come from the WWHD use (d_a_boko.cpp); the tww code is the same for every use cited. The multi-name declarations were split into one declaration per member first (separate commit). The view TnBoko (d_a_tn.h) takes mRotAngleSpeed for 0x43A: updateHeldWeapon (in daTn_Execute 024BD170) gives the dropped weapon a random spin.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x431 | `m2B9` | `mNoDraw` | checkNoDraw 020C0BFC: the stick is not drawn while it is set (as in Mode_4_e); never written in WWHD or tww |
| 0x433 | `m2BB` | `mGroundSnapCount` | procMove_init 020C2624: bokoCarryHideTimer + 5; procMove 020C2C90: while it is not 0 each ground contact pulls current.pos.y down by the smaller root / top gap and counts it down (by 2 when the pitch has settled) |
| 0x438 | `m2C0` | `mPitchSpeed` | procMove 020C2C90: the per-bounce pitch step toward the ground slope, added to shape_angle.x (with mRotAngleSpeed, the larger one); procWait_init 020C25A8 / setThrowReverse 020C26AC clear it (tww the same) |
| 0x43A | `m2C2` | `mRotAngleSpeed` | tww setRotAngleSpeed(s16) stores it; setThrowReverse 020C26AC: +-0x7FF, then the 1500 / 2000 spin; procMove 020C2C90 adds it to shape_angle.x and damps it at each bounce; procWait_init 020C25A8 clears it |
| 0x440 | `m2C8` | `mThrowGravityDelay` | procCarry 020C3480: 2 when the throw starts (gravity 0); procThrow 020C2978 counts it down and sets gravity -3 at 0 (tww the same) |
| 0x442 | `m2CA` | `mThrowAngle` | tww setThrow(s16) stores it; procCarry 020C3480: the throw's shape_angle.x, speedF = 70 cos / speed.y = -70 sin of it |
| 0x44C | `m2D0` | `mTopPos` | setTopRootPos 020C0E08: m_top_offset[type] through the stick matrix; procMove 020C2C90 ground-checks it; setFlameEffect 020C0EF8 puts the flame there (tww the same) |
| 0x458 | `m2DC` | `mRootPos` | setTopRootPos 020C0E08: m_root_offset[type] through the stick matrix; procMove 020C2C90 ground-checks it against mTopPos (tww the same) |
| 0x4A0 | `m320` | `mOldTopPos` | setFlameEffect 020C0EF8: copy of mTopPos after the flame emitter update; the emitter direction is (mTopPos - it) * rate (tww the same) |
| 0x964 | `m7E4` | `mFlameScale` | setFlameEffect 020C0EF8: eased to 1 + random 0.2, scales the four flame models; execute 020C183C sets 0 when the flame goes out and scales mLight.mPower by it |

### ke_c_s (one line-drawn hair strand) and LineKe (its 16 strands)

| Offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x000 | `m000` | `mNodePos` | keCalc1 020C1250: the 10 node positions of one strand, each pulled from the previous node (tww the same) |
| 0x078 | `m078` | `mNodeSpeed` | keCalc1 020C1250: the 10 node velocities added to the strand step (tww the same) |
| 0x148 | `m001C` | `mKe` | keCalc 020C1684 / keCalc1 020C1250 / keDraw: the 16 hair strands (ke_c_s) drawn with the line material (tww the same) |

Open (no name yet): `m2F4` (create 020C2188 copies current.pos there; never read) and the padding.

## daWarpdm20_c (warp to Hyrule, the ring in the sea; d_a_warpdm20.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_warpdm20.h) keeps the GameCube offset names for these fields; the names come from the WWHD use (d_a_warpdm20.cpp) and the tww code is the same for every use cited. `m2E0, m2EC` were split into one declaration each first (separate commit).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3E4 | `m2C8` | `mWarpOrder` | CreateInit 024D4D10 clears it; normal_execute 024D5354 sets 1 when check_warp sees the ship within 500; eventOrder 024D5708 orders TO_HYRULE_WARP while it is 1; checkOrder 024D5468 clears it when the event starts and runs normal_execute only while it is 0 (tww the same) |
| 0x3E8 | `m2CC` | `mVisible` | CreateInit (event bit 0x2D08) / normal_execute 024D5354 / demo_execute 024D57E4 set 1; actDead 024D5B84 clears it when the bck ends; _draw 024D507C draws nothing while it is 0; _execute 024D58FC plays the warp sound while it is set (tww the same) |
| 0x3F4 | `m2D8` | `mDemoShapeId` | demo_execute 024D57E4: the demo actor's mShapeId; 0 hides the model, 1 starts the 0x82BD effect (tww switches on it) |
| 0x3FC | `m2E0` | `mEffectPos` | CreateInit 024D4D10 / _execute: current.pos at the sea height; set_effect 024D513C places the ripple particle there (tww the same) |
| 0x408 | `m2EC` | `mEffectScale` | CreateInit 024D4D10 sets (1, 1, 1); set_effect 024D513C passes it as the particle scale; actDead 024D5B84 chases it to 0 and copies it into the emitter's particle and dynamics scales (tww the same) |
| 0x414 | `m2F8` | `mSenkoSet` | set_wpsenko 024D5768: the 0x82BC particle is set only once; set to 1 after it (tww the same) |

Open (no name yet): `m2D6` (CreateInit stores the parameter type; never read in WWHD or tww) and the padding.

## daWarphr_c (warp out of Hyrule; d_a_warphr.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_warphr.h) keeps the GameCube offset names for these fields; the names come from the WWHD use (d_a_warphr.cpp) and the tww code is the same for every use cited; mWarpOrder / mEventIdx / mStaffIdx / mDemoShapeId as in daWarpdm20_c. `m2AC, m2B0` were split into one declaration each first (separate commit).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3C8 | `m2AC` | `mpWarpEmitter` | initWarp 024DB718 / initWarpArrive 024DB808 store the ID_IT_SN_HRWARPB00 emitter; initWarpArriveEnd 024DB930 / _delete 024DA99C end it and clear it |
| 0x3CC | `m2B0` | `mpDemoEmitter` | _execute 024DB310 stores the ID_IT_SP_DEMO20_INDWP00 emitter in the demo; _draw 024DAA1C sets its rotation / translation from mDemoEmitterMtx; _delete 024DA99C ends it |
| 0x3D0 | `m2B4` | `mDemoEmitterMtx` | _draw 024DAA1C: translation to the demo camera eye and its yaw; decomposed into mpDemoEmitter's global rotation / translation (tww the same) |
| 0x400 | `m2E4` | `mDemoEffectSet` | _execute 024DB310: set once the demo frame reaches 0x225 (the monotone effect starts only while it is 0) (tww the same) |
| 0x40C | `m2F0` | `mWarpOrder` | normal_execute 024DAEE4 sets 2 when check_warp sees the player; eventOrder 024DB1EC orders the warp event while it is 2; checkOrder 024DAF54 clears it when the event starts; _execute 024DB310 clears it in the demo (tww the same) |
| 0x414 | `m2F8` | `mEventIdx` | CreateInit 024DA6DC: the event index of TO_SEA_WARP_2 / TO_SEA_WARP_1; eventOrder 024DB1EC / checkOrder 024DAF54 order and check it (tww the same); as daWarpdm20_c::mEventIdx |
| 0x41C | `m300` | `mDemoShapeId` | demo_execute 024DB24C: the demo actor's mShapeId; 0 plays animation 0 (tww switches on it); as daWarpdm20_c |
| 0x420 | `m304` | `mStaffIdx` | demo_proc 024DB028: dComIfGp_evmng_getMyStaffId("Ghrwp"); the warp cuts read it (tww the same); as daWarpdm20_c::mStaffIdx |

Open (no name yet): `m2FA` (CreateInit stores the parameter type; never read in WWHD or tww) and the padding.

## daWarpgn_c (warp to Ganon's Tower; d_a_warpgn.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_warpgn.h) keeps the GameCube offset names for these fields; the names come from the WWHD use (d_a_warpgn.cpp) and the tww code is the same for every use cited.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3E0 | `m2C4` | `mWarpOrder` | normal_execute 024D9880 sets WarpgnOrder_APPEAR / WarpgnOrder_WARP; eventOrder 024D9CA4 orders the appear / to-majyuu warp event by it; checkOrder 024D999C and _execute 024D9E00 (in a demo) reset it to WarpgnOrder_NONE (tww the same); as daWarpdm20_c / daWarphr_c::mWarpOrder |
| 0x5BC | `m388` | `mAppeared` | actAppear 024DA23C sets 1; _execute 024D9E00 runs the warp effects and the 100-frame timer while it is set or event bit 0x3D02 is on (tww the same) |

## daWindTag::daWindTag_c (wind column tag; d_a_wind_tag.h)

zeldaret/tww (d217e2b, src/d/actor/d_a_wind_tag.cpp) keeps the GameCube offset names; the names come from the WWHD use (d_a_wind_tag.cpp), the same code as tww.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x6F4 | `field_0x49c` | `mHeightScale` | CreateInit 024DF548: copy of scale.y; _execute 024E065C / CreateInit scale the type's height (mData[type * 2 + 1]) by it for mOffsY (tww the same) |
| 0x8E7 | `field_0x577` | `mSwitchLatch` | _execute 024E065C: set when the switch changes (and the switch type allows it), cleared when the schedule bits turn off again (mPrevSchActive); while it is set the wind blows regardless of the switch / schedule (tww the same) |
| 0x8EC | `field_0x57c` | `mPrevSwitch` | CreateInit 024DF548 / _execute 024E065C: fopAcM_isSwitch(mSwNo) of the last frame; _execute 024E065C compares it with the current state to latch mSwitchLatch (tww the same) |
| 0x8ED | `field_0x57d` | `mPrevSchActive` | _execute 024E065C: the last frame's getSch & dKy_get_schbit (schedule active); clears mSwitchLatch when it falls to 0 (tww the same) |
| 0x8EF | `field_0x57f` | `mSplash` | CreateInit 024DF548: daWindTag_prm::getSplashFlag == 1; CreateInit sets up the splash effect colour and emitter only while it is set (tww the same) |

## daFallRock_c (falling rock; d_a_fallrock.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_fallrock.h) keeps the GameCube offset names; the names come from the WWHD use (d_a_fallrock.cpp), the same code as tww.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B8 | `field_0x29C` | `mAcch` | create 0213185C sets it up with mAcchCir and speed; daFallRock_Execute 02131520 runs CrrPos and checks ChkGroundLanding / ChkWaterIn; daFallRock_c_dt 02131B58 destroys it (dBgS_ObjAcch; tww the same) |
| 0x57C | `field_0x460` | `mAcchCir` | create 0213185C: SetWall(30, 30), passed to mAcch.Set; daFallRock_c_dt 02131B58 destroys it (dBgS_AcchCir; tww the same) |
| 0x728 | `field_0x60C` | `mParticleColor` | setParticle 02131208: tevStr.mColorC0 copied in, the prim colour of the ID_IT_JN_M_STS_HAHEN fragments (tww the same) |
| 0x72C | `field_0x610` | `mFallDist` | create 0213185C clears it; daFallRock_Execute 02131520 adds /speed.y/ each frame and deletes the rock once it exceeds m_falllen (tww the same) |

Open (no name yet): `field_0x614` (daFallRock_Execute counts it up; never read).

## daObjPlant_c (swaying plant; d_a_obj_plant.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names; the names come from the WWHD use, the same code as tww.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x524 | `field_0x408` | `mSwayAngle` | _execute (in daObjPlant_Execute 023867F4): sin(mSwayPhase) * (0x100 - mHitTimer) * 32 / 16 while mSwaying, 0 at the end; nodeCallBack 023861E0 rotates the joint by it about the axis set by mSwayDir (tww the same) |
| 0x526 | `field_0x40A` | `mSwayPhase` | _execute (in daObjPlant_Execute 023867F4): 0 at a hit, + 0x1000 (first 8 frames) / + 0x800 per frame; the sine phase of mSwayAngle (tww the same) |
| 0x52A | `field_0x40E` | `mSwayDir` | _execute (in daObjPlant_Execute 023867F4): the angle from the hitting actor - 0x4000; nodeCallBack 023861E0 rotates X by it before and by -it after the sway rotation (tww the same) |
| 0x52C | `field_0x410` | `mSwaying` | CreateInit 02386514 clears it; _execute (in daObjPlant_Execute 023867F4) sets 1 at a hit, sways while it is set and clears it after 0x100 frames; a new hit counts only while it is 0 or after frame 0x38 (tww the same) |

## daFloor_c (floor that breaks under the Iron Boots; d_a_floor.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names; the names come from the WWHD use, the same code as tww.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3F0 | `field_0x2d8` | `mBreakStarted` | rideCallBack 0213FD18: set once (with mBreakTimer 6) when the player in iron boots lands on the floor after mPlayerStanding was set; only then (tww the same) |
| 0x3F1 | `field_0x2d9` | `mPlayerStanding` | rideCallBack 0213FD18 sets 1 while the player rides the floor without the iron boots; Execute 0214000C clears it whenever the player moves in XZ; the break needs it (tww the same) |
| 0x3F2 | `field_0x2da` | `mBreakTimer` | rideCallBack 0213FD18: 6 when the break starts; Execute 0214000C counts it down and at 0 sets the effect, the break sound, the switch and deletes the floor (tww the same) |

Open (no name yet): `field_0x2db` (no use).

## daToge_c (Wind Temple floor spikes; d_a_toge.h)

zeldaret/tww (d217e2b) names these unk470 / unk474 / unk485 / unk486; the names come from the WWHD use (d_a_toge.cpp), the same code as tww. The header comment that said the unk names follow the retail decompilation is updated.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x58C | `unk470` | `mSpikeY` | nodeCallBack 024C6E78: transM(0, it, 0) on the spike model; Create 024C6FB0 -150 (m_y_min); toge_move 024C7520 eases it to -150 (RETRACT) or to mSpikeTargetY (EXTEND); set_collision 024C7774 puts the collision at it - 10 (tww the same) |
| 0x590 | `unk474` | `mSpikeTargetY` | toge_move 024C7520: the EXTEND target of mSpikeY: 0 once the spikes are up, then -60 / 0 for the bob (tww the same) |
| 0x5A1 | `unk485` | `mRetractRequest` | _execute 024C77F0 sets 1 when the switch is on or the wind tag's wind stops (unless already waiting to retract); toge_move 024C7520 starts RETRACT_WAIT with mRetractWait at the extended position and clears it (tww the same) |
| 0x5A2 | `unk486` | `mRetractWait` | toge_move 024C7520: 10 when RETRACT_WAIT starts; counted down there, the spikes retract at 0 (tww the same) |

## daHys_c (arrow switch; d_a_hys.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names; the names come from the WWHD use, the same code as tww.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x5C8 | `field_0x450` | `mMode` | mode_proc_call 0217A8B4 runs mode_proc[it] (0 mode_wait, 1 mode_sw_on); Create 0217AB18 sets it from the switch; mode_sw_on_init 0217AD8C / mode_wait_init 0217AD04 set 1 / 0 (tww the same) |
| 0x5D0 | `field_0x458` | `mBtpFrame` | Draw 0217AA6C enters mBtpAnm at it; mode_sw_on 0217AD10 counts it up to 3, mode_wait 0217ADF8 down to 0; Create 0217AB18 3 / 0 by the switch (tww the same) |

## daObj_MjDoor_c and daObj_MjDoorHIO_c (Forsaken Fortress breakable door; d_a_obj_majyuu_door.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names; the names come from the WWHD use (d_a_obj_majyuu_door.cpp), the same code as tww. The HIO flag is set only by the HIO constructor.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3E8 | `field_0x2CC` | `mSwitchNo` | getArg 0236F3BC: param & 0xFF; _create 0236F580 creates nothing when that switch is on; modeDeleteInit 0236FAAC sets it (tww the same) |
| HIO 0x04 | `field_0x4` | `mForceBreak` | modeWait 0236FCD8 breaks the door (modeDeleteInit) when it is set as when health < 0; MjDoor_hioCtor 0236FDB8 sets 0, never written elsewhere in WWHD or tww |

## daSwProp_c (wind propeller switch; d_a_swpropeller.h)

zeldaret/tww (d217e2b) keeps the GameCube offset name; the name comes from the WWHD use (d_a_swpropeller.cpp), the same code as tww.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x764 | `m648` | `mSwitchNo` | CreateInit 024A2E10: param & 0xFF; _execute 024A31E4 toggles that switch (dComIfGs_revSwitch) (tww the same) |

## daObjOspbox::Act_c (Ospbox; d_a_obj_ospbox.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names; the names come from the WWHD use (d_a_obj_ospbox.cpp), the same code as tww.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x5AC | `m494` | `mGroundY` | set_ground 02380610: the GroundCross result 100 above the box (tww tests it against -G_CM3D_F_INF) |
| 0x5B0 | `m498` | `mGroundTries` | init_ground 023806CC sets 5; set_ground 02380610 checks the ground only while it is > 0, sets 0 once found, else counts down (tww the same) |

Open (no name yet): `m49A` (Create sets 2, Execute counts it down and then sets bit 0x80 of the dBgW flags; that flag has no name, so the role is not settled).

## daDemo_Kmm_c (demo actor Kmm; d_a_demo_kmm.h)

Named under the 2026-11-11 rule (written by WWHD only at init, read only by zeldaret/tww d217e2b).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B8 | `unk_29C` | `mAction` | written only by CreateInit 02124324 through setAction(0) (d_a_demo_kmm.h); read only in tww daDemo_Kmm_c::execute, which calls l_action[it] (the one entry daDemo_Kmm_actionWait) |

Open (no name yet): `unk_29E` (CreateInit clears it; no read in WWHD or tww).

## daObj_Warpt_HIO_c (l_HIO; d_a_obj_warpt.h)

Round 5 (two-digit offset names). The warp pot's HIO: a one-shot lid-break request, the ride switch, the lid height, the warp circle radius and the smoke colours. Same meanings in zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `m04` | `mBreakHutaReq` | _execute 023BAD74: while set it is cleared, breakHuta(0) runs and the mode goes to 2 (MODE_BREAK_FIRE), a one-shot request; warptHioConstruct 023BBA6C 0 (tww the same) |
| 0x06 | `m06` | `mNoRide` | _ride 023B9D5C returns before the warp check while it is set (tww the warp check runs only while it is 0); warptHioConstruct 023BBA6C 0 (tww the same) |
| 0x08 | `m08` | `mLidOffsetY` | setMtx 023BA250: mLidPos.y = current.pos.y + it, the height where the lid models are put; warptHioConstruct 023BBA6C 150 (tww m830.y += it) |
| 0x0C | `m0C` | `mWarpRadius` | _ride 023B9D5C: Link inside a circle of this radius (height 20) around mLidPos starts mode 3, MODE_EVENT_WARP; warptHioConstruct 023BBA6C 30 (tww dLib_checkPlayerInCircle(m830, it, 20)) |
| 0x14 | `m14` | `mSmokePrmColorOverride` | _execute 023BAD74: when its r/g/b is not all 0 it becomes the open / close smoke emitters' prim colour (tww setGlobalPrmColor); warptHioConstruct 023BBA6C the default colour (tww {0, 0, 0, 128}) |
| 0x18 | `m18` | `mSmokeEnvColorOverride` | _execute 023BAD74: when its r/g/b is not all 0 it becomes the open / close smoke emitters' env colour (tww setGlobalEnvColor); warptHioConstruct 023BBA6C the default colour (tww {0, 0, 0, 128}) |

Open (no name yet): `m05` and `m10` (set by warptHioConstruct 023BBA6C to 0 / -2.5, read nowhere in WWHD or tww), padding `mPadding7`.


## daYkgr_HIO_c and dPa_YkgrPcallBack (l_HIO, YkgrCB; d_a_ykgr.h)

Round 5 (two-digit offset names). The Dragon Roost heat haze: the HIO's alpha fade step, the strength range blended by the aim / position rates, the aim target and the path distance range; the particle callback's 2x3 indirect texture matrix and its scale exponent, which YkgrPcallBack_draw 024EAD4C hands to JPASetIndTexMtx. Same meanings in zeldaret/tww d217e2b.

daYkgr_HIO_c (l_HIO):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0C | `m0C` | `mAlphaStep` | daYkgrExecute 024EAAE8 (_execute inlined): m_alpha moves by it per frame toward 0 (stopped) or 0xFF (started); __sinit_d_a_ykgr_cpp 024EAD68 3 (tww the same) |
| 0x14 | `m14` | `mStrengthLow` | daYkgrExecute 024EAAE8: setParam(rate * mStrengthHigh + (1 - rate) * it), the haze strength (the ind-matrix scale exponent and mantissa) at rate 0; __sinit_d_a_ykgr_cpp 024EAD68 -5 (tww the same) |
| 0x18 | `m18` | `mStrengthHigh` | daYkgrExecute 024EAAE8: the haze strength passed to setParam at rate 1 (rate = half mAimRate + half mPosRate); __sinit_d_a_ykgr_cpp 024EAD68 -3 (tww the same) |
| 0x1C | `m1C` | `mAimRateTarget` | daYkgrExecute 024EAAE8: m_aim_rate eases toward it, and mAimRate toward m_aim_rate; __sinit_d_a_ykgr_cpp 024EAD68 0 (tww the same) |
| 0x20 | `m20` | `mPathDistMax` | getPosRate 024EA984: the player's xz distance to the nearest path point is clamped to [mPathDistMin, it]; the rate is 1 - (dist - min) / (it - min); __sinit_d_a_ykgr_cpp 024EAD68 1500 (tww the same) |
| 0x24 | `m24` | `mPathDistMin` | getPosRate 024EA984: the lower clamp of the path distance, where the position rate is 1; __sinit_d_a_ykgr_cpp 024EAD68 500 (tww the same) |

dPa_YkgrPcallBack (YkgrCB):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `m04` | `mIndMtx00` | YkgrPcallBack_draw 024EAD4C passes &it to JPASetIndTexMtx as the 2x3 indirect texture matrix (tww GXSetIndTexMtx(GX_ITM_0, &m04, m1C)); row 0 column 0, the mantissa setParam 024EA58C computes |
| 0x08 | `m08` | `mIndMtx01` | YkgrPcallBack_draw 024EAD4C: row 0 column 1 of the indirect texture matrix starting at mIndMtx00; setParam 024EA58C and __sinit_d_a_ykgr_cpp 024EAD68 0 (tww the same) |
| 0x0C | `m0C` | `mIndMtx02` | YkgrPcallBack_draw 024EAD4C: row 0 column 2 of the indirect texture matrix; setParam 024EA58C and __sinit_d_a_ykgr_cpp 024EAD68 0 (tww the same) |
| 0x10 | `m10` | `mIndMtx10` | YkgrPcallBack_draw 024EAD4C: row 1 column 0 of the indirect texture matrix; setParam 024EA58C and __sinit_d_a_ykgr_cpp 024EAD68 0 (tww the same) |
| 0x14 | `m14` | `mIndMtx11` | YkgrPcallBack_draw 024EAD4C: row 1 column 1 of the indirect texture matrix; setParam 024EA58C stores the same mantissa as mIndMtx00 (tww m04 = m14 = ..) |
| 0x18 | `m18` | `mIndMtx12` | YkgrPcallBack_draw 024EAD4C: row 1 column 2 of the indirect texture matrix; setParam 024EA58C and __sinit_d_a_ykgr_cpp 024EAD68 0 (tww the same) |
| 0x1C | `m1C` | `mIndScaleExp` | YkgrPcallBack_draw 024EAD4C passes it to JPASetIndTexMtx as the matrix's scale exponent (tww GXSetIndTexMtx scale_exp); setParam 024EA58C stores its parameter rounded down; __sinit_d_a_ykgr_cpp 024EAD68 1 (tww the same) |

Open (no name yet): daYkgr_HIO_c `m08` and `m10` (set by __sinit_d_a_ykgr_cpp 024EAD68 to 0xFF / -16, read nowhere in WWHD or tww), padding `_05`.


## daObjTide::Attr_c (attr(); d_a_obj_tide.h)

Round 6 (two-digit offset names). The tide object's attributes: the height it moves with the water level, the speed curve of its chase to the aim height, and the delay and rate of its wet (nure) brk. Same meanings in zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x20 | `m20` | `mLevelHeight` | Mthd_Create 023A0484 (with sch_flag) and mode_norm 023A14A8: current.pos.y = home.pos.y + (1 - the water level) * it, the height the object moves with the tide (tww the same) |
| 0x24 | `m24` | `mMoveAccel` | move_to_aim 023A1258: mMoveSpeed grows by it every frame (tww m2F0 += it) |
| 0x28 | `m28` | `mMoveSpeedMax` | move_to_aim 023A1258: the upper clamp of mMoveSpeed (tww the same) |
| 0x2C | `m2C` | `mMoveSpeedMin` | move_to_aim 023A1258: mMoveSpeed is kept above it (it + 0.001) and it is cLib_addCalc's minimum step (tww the same) |
| 0x30 | `m30` | `mMoveScale` | move_to_aim 023A1258: cLib_addCalc's scale for the height chase toward the aim (tww the same) |
| 0x42 | `m42` | `mNureBrkDelay` | mode_norm 023A14A8: mTimer = it when the water state flags it (STATE_2 without STATE_1); when mTimer runs out the nure (wet) brk restarts from frame 0 (tww m2F4 = it) |
| 0x44 | `m44` | `mNureBrkRate` | CreateHeap 023A06C0 inits the nure (wet) brk mBrk2 with it as play speed; mode_norm 023A14A8 sets that rate again on restart (tww setPlaySpeed(it)) |

Open (no name yet): padding `padding0E`, `padding22`, `padding41`.


## daobj_stairHIO_c (l_HIO; d_a_obj_stair.h)

Round 6 (two-digit offset names). The crumbling stairs (崩れる階段): the delay before a hit stair breaks loose, its break speed and spin, and its fall. Same meanings in zeldaret/tww d217e2b; in WWHD the vtable is last, so the offsets are 4 lower than the old names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `m08` | `mFallAccel` | _execute 02390AC4: after mBreakDelay, speed.y eases toward mFallSpeedMax with cLib_addCalc2 step it; daobj_stairHIO_c_ct 02391570 2.5 (tww the same) |
| 0x08 | `m0C` | `mFallSpeedMax` | _execute 02390AC4: the target of speed.y while the broken stair falls; daobj_stairHIO_c_ct 02391570 -175 (tww the same) |
| 0x0C | `m10` | `mBreakRotSpeed` | _execute 02390AC4: when mBreakTimer reaches mBreakDelay the spin per frame is it * 0.5 * the hit offset length * 0.01 * mBreakPower degrees (mRotSpeed); daobj_stairHIO_c_ct 02391570 5 (tww the same) |
| 0x10 | `m14` | `mBreakSpeed` | _execute 02390AC4: when mBreakTimer reaches mBreakDelay the speed is it * mBreakPower along mBreakDir (times the spin cosine); daobj_stairHIO_c_ct 02391570 2.5 (tww the same) |
| 0x14 | `m18` | `mBreakDelay` | ride_call_back 023905F4 and the hit code in _execute 02390AC4 start mBreakTimer below it (it - 2, or less for a far hit); at it the stair breaks loose (speed, spin, JA_SE_OBJ_BREAK_STEPS), it shakes in the 10 frames before, falls after and is deleted 240 frames later; daobj_stairHIO_c_ct 02391570 6 (tww the same) |

Open (no name yet): padding `_01`, `_16`. The local `m18` in _execute (a copy of the HIO value) keeps its name.


## daMozo_childHIO_c (l_HIO.mBeamChild / mFireChild; d_a_mozo.h)

Round 8 (two-digit offset names). The per-type search and attack ranges of the Mozo statue (beam and fire child HIOs, picked by mType in checkRange). Same meanings in zeldaret/tww; in WWHD the vtable is last, so the offsets are 4 lower than the names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m04` | `mSearchDist` | daMozo_c::checkRange 021D5030 with 0 (wait_proc 021D53D8, before the attack starts): Link must be closer than it horizontally; daMozo_HIO_c__ct 021D6070 1000 for both children (tww the same) |
| 0x04 | `m08` | `mAttackDist` | daMozo_c::checkRange 021D5030 with 1 (the attack procs; a failed check returns to towait_proc_init): Link must stay closer than it horizontally; daMozo_HIO_c__ct 021D6070 1200 for both children (tww the same) |
| 0x08 | `m0C` | `mSearchAngle` | daMozo_c::checkRange 021D5030 with 0: the angle between the actor's facing and Link must be below it; daMozo_HIO_c__ct 021D6070 0x2000 (tww the same) |
| 0x0A | `m0E` | `mAttackAngle` | daMozo_c::checkRange 021D5030 with 1: the facing angle limit while it attacks; daMozo_HIO_c__ct 021D6070 0x2800 (tww the same) |

Open (no name yet): daMozo_childHIO_c `m10` (set to 0 by the constructor, read nowhere in WWHD or tww); daMozo_HIO_c `m08` (set to -1, read nowhere), `m3C`, `m40`, `m44` (constructor only, read nowhere in WWHD or tww).

## daOship_HIO_c (l_HIO; d_a_oship.h)

Round 8 (two-digit offset names). The enemy ship's debug switches: instant kill, no shot, debug-register bomb flight and perfect aim. Same fields (bool, unnamed) in zeldaret/tww; the vtable is first, so the offsets equal the old names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x05 | `m05` | `mDebugKill` | daOship_c::checkTgHit 023C714C: while it is set the ship's health becomes 0 and it enters MODE_DELETE at once, before any hit check; daOship_HIO_c__ct 023C8090 0 (tww the same, bool) |
| 0x06 | `m06` | `mNoAttack` | daOship_c::modeAttack 023C760C: when the aim timer runs out, a set flag skips the shot the way a blocked lineCheck does (changeModeByRange, no bomb); daOship_HIO_c__ct 023C8090 0 (tww the same, bool) |
| 0x07 | `m07` | `mBombRegFlight` | daOship_c::_execute 023C68B8: only while it is set do the fired bombs get the debug-register flight (speedF eased to l_debugRegisters mFloat[3]/[4], speed.y set from mFloat[2] once below mFloat[1]); daOship_HIO_c__ct 023C8090 0 (tww the same with REG12_F) |
| 0x08 | `m08` | `mPerfectAim` | daOship_c::modeAttackInit 023C6DB4: a set flag forces the random aim error to 0 on both branches (Link in the special status and normal); daOship_HIO_c__ct 023C8090 0 (tww the same, bool) |

Open (no name yet): daOship_HIO_c `m04` (set to 0 by the constructor, read nowhere in WWHD or tww), padding `padding09`.

## daObj_Hole_HIO_c (l_HIO; d_a_obj_hole.h)

Round 8 (two-digit offset names). The pitfall hole's HIO: debug draw, the base radius, a size override and the model's height offset. Same meanings in zeldaret/tww; in WWHD the vtable is last, so the offsets are 4 lower than the names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x01 | `m05` | `mDebugDraw` | daObj_Hole_c::_draw 0235792C calls debugDraw while it is set; daObj_Hole_HIO_c_ct 02357CE0 0 (tww the same) |
| 0x04 | `m08` | `mBaseRadius` | daObj_Hole_c::modeWait 02357AC8: the pit's radius is it * scale.x + size * 10, and Link inside that circle (dLib_checkPlayerInCircle) starts the pitfall event; setMtx 023573BC divides the same radius by it for the model scale; daObj_Hole_HIO_c_ct 02357CE0 65 (tww the same) |
| 0x08 | `m0C` | `mSizeOverride` | daObj_Hole_c::modeWait 02357AC8 and setMtx 023573BC: when non-zero it replaces the actor's own size step mScaleLocal in the radius (it * 10); daObj_Hole_HIO_c_ct 02357CE0 0 (tww the same) |
| 0x0C | `m10` | `mModelOffsetY` | daObj_Hole_c::setMtx 023573BC: added to current.pos.y for the model's position; daObj_Hole_HIO_c_ct 02357CE0 2 (tww the same) |

Open (no name yet): padding `_02`, `_0A`.

## daTbox_HIO_c (l_HIO; d_a_tbox.h)

Round 8 (two-digit offset names). The treasure chest's HIO: the timing of the appear demo after the Wind Waker song (show frame, length, dimming and brightening of the scene colours). Same meanings in zeldaret/tww; in WWHD the vtable is last, so the offsets are 4 lower than the names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x02 | `m06` | `mAppearShowFrame` | daTbox_c::demoProcAppear_Tact 024B68B0: when mAppearTimer reaches mAppearTime - it (that many frames into the demo) the chest model is made visible and the appear brk is played to its end; daTbox_HIO_ct 024B7888 130 (tww the same) |
| 0x04 | `m08` | `mAppearTime` | the length of the chest's Wind Waker appear demo: daTbox_c::CreateInit 024B5CFC starts mAppearTimer at it, demoProcAppear_Tact 024B68B0 counts it down and measures the show frame and the dimming from it; daTbox_HIO_ct 024B7888 180 (tww the same) |
| 0x06 | `m0A` | `mAppearDimTime` | daTbox_c::demoProcAppear_Tact 024B68B0: over the first it frames the scene colour ratio falls from 1 to 0.4 (tboxSetAllColorRatio); daTbox_HIO_ct 024B7888 48 (tww dKy_set_allcol_ratio, the same) |
| 0x08 | `m0C` | `mAppearBrightenTime` | daTbox_c::demoProcAppear_Tact 024B68B0: over the last it frames the colour ratio rises from 0.4 back to 1; daTbox_HIO_ct 024B7888 30 (tww the same) |

Nothing else is open; `mPadding` and `mPaddingA` are padding.

## daObjComing::Ctrl_c, Act_c::MakeTmp_c and PrmBarrel2_c (d_a_obj_coming.h)

Round 8 (two-digit offset names). The floating barrel spawner: each of its five Ctrl_c controllers (mode, timer, barrel id), the position check record MakeTmp_c and the barrel parameter record's buoy flag. Same meanings in zeldaret/tww; no vtable, so the offsets equal the old names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mMode` | the controller's Ctrl_c::Mode: Ctrl_c::init 02331A08 MODE_WAIT; mode_barrel2_wait 02332728 sets MODE_APPEAR after creating a barrel, mode_barrel2_appear 02332868 MODE_WAIT again; _execute 02331C94 dispatches on it (tww the same) |
| 0x04 | `m04` | `mTimer` | Ctrl_c::init_timer 0233194C / set_wait_timer 02331A44 / set_retry_timer 02331B00 / set_appear_timer 02331B4C set it; _execute 02331C94 counts it down; mode_barrel2_wait / appear act only once it is 0 (tww the same) |
| 0x08 | `m08` | `mBarrelId` | process id of the controller's barrel: mode_barrel2_wait 02332728 stores Coming_createBarrel's result, mode_barrel2_appear 02332868 / leave 02332960 look it up and clear it to fpcM_ERROR_PROCESS_ID_e; Ctrl_c::init 02331A08 clears it (tww the same) |

Act_c::MakeTmp_c:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mShipPos` | Act_c::barrel2_get_param 023325C4: the ship's position; chk_make_pos_wall 023322EC: start of the line check to mMakePos (tww the same) |
| 0x0C | `m0C` | `mMakePos` | Act_c::barrel2_get_param 023325C4: the candidate barrel position from get_make_pos; chk_make_pos_gnd 0233211C, chk_make_pos_wall 023322EC and chk_make_pos_act 0233233C test it (tww the same) |
| 0x18 | `m18` | `mCheckRadius` | barrel2_get_param 023325C4 200; chk_make_pos_gnd 0233211C and chk_make_pos_act 0233233C use it + 100 as the radius around mMakePos (tww the same) |
| 0x1C | `m1C` | `mCheckHeight` | barrel2_get_param 023325C4 400; chk_make_pos_act 0233233C: an actor counts as in the way within it + 100 of mMakePos.y (tww the same) |

PrmBarrel2_c:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x14 | `m14` | `mHasBuoy` | Act_c::barrel2_get_param 023325C4 sets it from barrel2_get_buoy (tww daObjComing::Act_c::barrel2_get_buoy, a per-pattern bool table); mode_barrel2_wait 02332728 puts its inverse in bit 8 of the barrel's parameters |

Open (no name yet): `Attr_c::m00` in d_a_obj_coming.cpp (the s16 table L_Attr: entries 0-3 are the barrel counts per PRM_MAKENUM, entries 4-11 the base and random range of the wait timer per PRM_WAIT_TIME; no single role for one name), padding `PrmBarrel2_c::padding`, `Act_c::padding`.

## daObjSwpush::Attr_c (M_attr; d_a_obj_swpush.h)

Round 9 (two-digit offset names). The floor switch's per-type attributes: the speed when a rider lands, the delay of the downward move, and the ride-area scales before and while riding. Same fields, unnamed, in zeldaret/tww (tww reads them the same way: attr().m34 / m38 / m40 / m44); no vtable, so the offsets equal the old names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x34 | `m34` | `mLandSpeed` | daObjSwpush::Act_c::mode_upper 02398608: when a rider newly lands on a switch that does not need a heavy rider, mSpeed is set to it (with a 30-frame debounce and a pause); per-type attribute (tww the same field, unnamed) |
| 0x38 | `m38` | `mLowerDelay` | daObjSwpush::Act_c::mode_u_l_init 02398348: the timer passed to top_bg_aim_req with target 0 when the switch starts going down (upper to lower); next to mMiniPushDelay1 / mPushDelay / mMiniPushDelay2 (tww the same field, unnamed) |
| 0x40 | `m40` | `mRideAreaScale` | rideCB 0239705C: scale of the no-push offset vectors that widen the top's ride-check triangles while the rider was not on it the previous frame (tww the same field, unnamed) |
| 0x44 | `m44` | `mRideAreaScaleHeld` | rideCB 0239705C: the same scale while the rider was already on it (mPrevRiding), so the area differs once riding (tww the same field, unnamed) |

Nothing else open apart from the padding `padding4A`.

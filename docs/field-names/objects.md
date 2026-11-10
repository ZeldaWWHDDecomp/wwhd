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
| 0x808 | `m6EC` | `mOffsetY` | set_mtx 024CB280 adds it to the model's y translation; _create 024CB5EC clears it; moment_proc_call 024CE204 sets it |
| 0x818 | `m6FC` | `mFallStartY` | _create 024CB5EC sets current.pos.y; damage_bg_proc_directly 024CDAB4 breaks the pot when it minus current.pos.y exceeds the attribute's fall height (m38) |
| 0x81C | `m700` | `mWaterDropPos` | set_mtx 024CB280: the model matrix times offsetPos; eff_drop_water 024CD95C sets dPa_ID_IT_SN_WPOT_PITYA there |
| 0x8A0 | `m784` | `mKutaniCb` | the constructor 024CA1DC constructs it (dPa_followEcallBack); eff_kutani_set 024CE3D0 uses it; _delete 024CBC98 removes it |
| 0x8B4 | `m798` | `mKutaniTimer` | eff_kutani_init 024CACEC sets 1 + rnd(30); eff_kutani_set 024CE3D0 acts while it is not 0 |
| 0x910 | `m7F4` | `mWindVec` | _create 024CB5EC clears it; set_wind_vec 024CC244 sets the wind vector (PSVECSquareMag); mode_walk / mode_drop / mode_afl use it |

Open: `m504`, `m505`, `m67E`..`m686`, the rotation angles `m688`..`m692`, `m694` (quaternion, set to zero by init_mtx only), `m6A4`, `m6B0`, `m6F0`, the effect callbacks `m70C`, `m748`, `m799`, `m7A0`, `m800`..`m80C`, `m810`, `m811`, `m814`.

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

Open: `m414`, `m418`, `m42C`..`m438`, `m43C`, `m440`..`m44C` (sway / drift state), `m468`, `m46C`, `m470`, `m475`, `m478`.

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
| 0x3EC | `field_0x2D4` | `mBreakDir` | ride_call_back 023905F4 (0, -1, 0) for the player; _execute 02390AC4 the bomb-to-step vector clamped to the step (tww); the break speed is l_HIO.m14 * mBreakPower * it, the tilt axis is mHitOffset x it |
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
| 0x9AC | `m830` | `mLidPos` | setMtx 023BA250: current.pos raised by l_HIO.m08 (tww it = current.pos; it.y += l_HIO.m08), where the lid models are put; setCollision 023BA1BC mCyl2 centre; breakHuta 023BAA28 particles; _draw 023BB074 tev position (tww) |
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

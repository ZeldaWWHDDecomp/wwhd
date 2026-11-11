# Field names: bosses

Offset-named fields of the boss actor classes (`*_class`) that got names in the field naming pass (2026-11-10); rules as in docs/decomp-notes.md, "Field names (offset-named fields)": a name comes from
how the original code uses the field (address of the use and what it shows), from names already in this decomp,
or from the CC0 zeldaret/tww and zeldaret/tp decomps. Renames only (objcmp: object-identical). The header line
keeps the old name (`was mXXX`) with the evidence. Inlined GameCube functions are cited as `name (in Caller
ADDR)`. Fields without enough evidence keep their offset name and are listed as open.

## bdk_class (Helmaroc King, battle; d_a_bdk.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03D8 | `m2C4` | `mFrameCounter` | daBdk_Execute 02066A38 adds 1 per frame; wind_attack 0206AE64 / kamen_draw (in daBdk_Draw 02065AB4) / head_swing use it as the phase of cM_ssin swings; t_fly 0206C3AC tests == 100 |
| 0x03DE | `m2CA` | `mFlyAnm` | daBdk_Create 02069C8C sets -1; fly / wait (in move 0206D644) set 0..3 and start the matching animation (e.g. 1: SENKAI_L1) only when the value changes |
| 0x03E0 | `m2CC` | `mTargetPos` | fly_attack 0206A610 sets it to the player position (+ mTargetOffset) or fixed points; wind_attack 0206AE64 copies current.pos; pos_move 02066398 flies toward it |
| 0x03EC | `m2D8` | `mTargetAngleY` | fly_attack 0206A610: angle.y + 0x8000, then turns current.angle.y toward it |
| 0x03F0 | `m2DC` | `mTurnSpeed` | fly_attack 0206A610 / fly, wait (in move 0206D644) set 100..1000; pos_move 02066398: angle step = it * mTurnSpeedRate |
| 0x03F4 | `m2E0` | `mTurnSpeedRate` | fly_attack 0206A610 clears it, wind_attack 0206AE64 / t_fly 0206C3AC ease it to 1; pos_move 02066398 multiplies mTurnSpeed by it |
| 0x03F8 | `m2E4` | `mTargetSpeed` | fly_attack 0206A610 / fly, wait, jump (in move 0206D644) set it (l_HIO speeds, 0, 30, 40); pos_move 02066398 eases speedF to it |
| 0x03FC | `m2E8` | `mSpeedStep` | fly_attack 0206A610 / move 0206D644 set 1/5/30; pos_move 02066398: the max step of speedF toward mTargetSpeed |
| 0x0400 | `m2EC` | `mTimers` | daBdk_Execute 02066A38 counts all 5 down each frame; actions load them (daBdk_Create 0x168, fly_attack 0206A610 0x1E/200) |
| 0x040A | `m2F6` | `mTailDrawDelay` | start 0206B67C sets 25; daBdk_Draw 02065AB4 (tail_draw) skips a tail and counts it down while it is not 0 |
| 0x040E | `m2FA` | `mMaskShakeTimer` | damage_check (in daBdk_Execute 02066A38) sets 10, daBdk_Execute counts down; kamen_draw (in daBdk_Draw 02065AB4) shakes the mask by it * 150 |
| 0x0414 | `m300` | `mTails` | useHeapInit 02069428 creates the bdk_tail_s models; tail_control (daBdk_Execute 02066A38) and nodeCallBack 02065794 / daBdk_Draw 02065AB4 per tail |
| 0x0A08 | `m8F8` | `mMaskBreakStage` | damage_check (in daBdk_Execute 02066A38) adds 1 per mask hit up to 4; kamen_draw (in daBdk_Draw 02065AB4) shows kamen_pt.mJoint[it]; col_set and move 0206D644 branch on it |
| 0x0A0C | `m8FC` | `mpMaskPieceModel` | useHeapInit 02069428 creates the 4 mask piece models; kamen_break_move (in daBdk_Execute 02066A38) places them, daBdk_Draw 02065AB4 draws them |
| 0x0A1C | `m90C` | `mMaskPieceOn` | kamen_demo 0206B22C sets [i] = 1, end 0206BAA8 clears; daBdk_Draw 02065AB4 draws and kamen_break_move moves only pieces with it set |
| 0x0A20 | `m910` | `mMaskPiecePos` | kamen_demo 0206B22C: piece start position (MtxPosition + dist); kamen_break_move (in daBdk_Execute 02066A38) moves it; daBdk_Delete 020692E0 deletes its sound object |
| 0x0A50 | `m940` | `mMaskPiecePosOld` | kamen_demo 0206B22C copies mMaskPiecePos; kamen_break_move (in daBdk_Execute 02066A38) keeps last frame's piece position in it |
| 0x0AB0 | `m9A0` | `mMaskPieceAngle` | kamen_demo 0206B22C sets the piece angle (shape_angle.y, 0x3625); kamen_break_move (in daBdk_Execute 02066A38) spins it and rotates the piece model by it |
| 0x0AE8 | `m9E8` | `mMaskPieceBreakTime` | kamen_demo 0206B22C loads kamen_break_time[i]; kamen_break_move (in daBdk_Execute 02066A38) counts it down |
| 0x0B14 | `mA14` | `mMaskPieceStts` | daBdk_Create 02069C8C: Init(0xFF, 0xFF), the Stts of the 4 mMaskPieceSph |
| 0x0B50 | `mA50` | `mMaskPieceSph` | daBdk_Create 02069C8C: Set(kamen_sph_src) x4; kamen_break_move (in daBdk_Execute 02066A38) uses sphere [i] for piece [i] |
| 0x1010 | `mF10` | `mLookAtPlayer` | fly_attack 0206A610, t_fly 0206C3AC and fly / landing / wait / jump / jida_attack (in move 0206D644) set 1 each frame; daBdk_Execute 02066A38 then eases mNeckAngleX toward the player's direction (clamped to +-7000), otherwise back to 0, and clears it |
| 0x1012 | `mF12` | `mNeckAngleX` | daBdk_Execute 02066A38 eases it toward the neck angle; nodeCallBack 02065794 XrotM by it |
| 0x1014 | `mF18` | `mTargetOffset` | fly_attack 0206A610: cM_rndFX(100), added to the player's x and z for mTargetPos |
| 0x1232 | `m1136` | `mHitAngleY` | damage_check (in daBdk_Execute 02066A38): atan2 from the attacker to the actor; head_swing YrotS by it; kuti_attack (in move 0206D644) clears it |
| 0x1258 | `m115C` | `mHeadAtPos` | col_set (in daBdk_Execute 02066A38): MtxPosition of the head point (y -10000 when off), centre of mHeadAtSph |
| 0x1270 | `m1174` | `mFootPos` | col_set (in daBdk_Execute 02066A38): MtxPosition per foot, centres of mFootCCSph[i]; fly_attack 0206A610 copies them to foot_eff_pos |
| 0x2584 | `m2488` | `mWindAge` | kankyo_cont 0206F9E0: per wind sphere, 0 = off, +1 per frame, cleared when old |
| 0x2590 | `m2494` | `mWindPos` | wind_set 02066798 sets the start; kankyo_cont 0206F9E0 adds mWindSpeed and moves mWindAtSph[i] to it |
| 0x2608 | `m250C` | `mWindSpeed` | wind_set 02066798: MtxPosition of the wind direction; kankyo_cont 0206F9E0 adds it to mWindPos each frame |
| 0x2680 | `m2584` | `mAttackPart` | the attack sphere for this frame: t_fly 0206C3AC, t_lastattack 0206CAD8 and kuti_attack (in move 0206D644) set 1, col_set (in daBdk_Execute 02066A38) then places mHeadAtPos at the jaw (AGO); fly_attack 0206A610, t_fly and landing / jida_attack (in move) set 2, col_set then turns the foot spheres' At on; daBdk_Execute clears it every frame |
| 0x2681 | `m2585` | `mFootAtStarted` | col_set (in daBdk_Execute 02066A38): 0 -> StartCAt of the foot spheres and adds 1, otherwise MoveCAt; cleared while mAttackPart is not 2 |
| 0x2682 | `m2586` | `mGroundTimer` | daBdk_Execute 02066A38 sets 2 on a ground hit (HD; GameCube 3) and counts it down off the ground; wait (in move 0206D644) sets 2, jump clears it; landing (in move) stops on it, damage_check (in daBdk_Execute) plays DAMAGE3 and returns to ACTION_WAIT while it is set |
| 0x2684 | `m2588` | `mBaseBck` | anm_init 02065620 stores the bck index when param7 is 0; move 0206D644 restarts it (anm_init) after a one-shot animation |
| 0x2688 | `m258C` | `mBaseSound` | anm_init 02065620 stores the sound index with mBaseBck; move 0206D644 passes it back to anm_init |
| 0x268C | `m2590` | `mBaseLoopMode` | anm_init 02065620 stores the loop mode with mBaseBck; move 0206D644 passes it back to anm_init |
| 0x268D | `m2591` | `mReturnToBase` | anm_init 02065620 sets 1 for a one-shot (param7 != 0), 0 otherwise; move 0206D644 restarts mBaseBck when the one-shot stops, then clears it |
| 0x268E | `m2592` | `mWaitCount` | wait (in move 0206D644) adds 1, leaves the wait when it reaches 2 (mask not broken); wait_to_up_fly clears it |
| 0x268F | `m2593` | `mFlyCount` | fly (in move 0206D644) adds 1 per pass, clears it; >= 4 ends the fly |
| 0x2690 | `m2594` | `mHitCount` | damage_check (in daBdk_Execute 02066A38) adds 1 per hit; wait (in move 0206D644) leaves at >= 5; wait_to_up_fly clears it |
| 0x269C | `m25A0` | `mDemoMode` | the cutscene step: kamen_demo 0206B22C / start 0206B67C / t_down 0206D0FC / end 0206BAA8 set 1, 10, 0x32, 0x6E ...; demo_camera 0206FEF0 switches on it, runs the camera while it is not 0 and reports it as DEMO MODE (BdkDemo_reports().demoMode) |
| 0x26A0 | `m25A4` | `mDemoMainCount` | kamen_demo 0206B22C, start 0206B67C and demo_camera 0206FEF0 clear it when a cutscene starts; demo_camera adds 1 per frame, uses it as the phase of the camera shake and reports it as K MAIN COUNT (tww JUTReport "K MAIN COUNT  %d") |
| 0x26A2 | `m25A6` | `mDemoTimer` | kamen_demo 0206B22C clears it and compares it with frame marks (0x2D, 0x6E); demo_camera 0206FEF0 reads it |
| 0x26A4 | `m25A8` | `mDemoCamEye` | demo_camera 0206FEF0: copied to camera->mEye and passed to dCamera_Set_bdk as the eye |
| 0x26B0 | `m25B4` | `mDemoCamCenter` | demo_camera 0206FEF0: copied to camera->mCenter and passed to dCamera_Set_bdk as the centre |
| 0x26BC | `m25C0` | `mDemoCamAngle` | demo_camera 0206FEF0 (t_down cutscene): the yaw (.y) and pitch (.x) from the boss to the camera eye, then YrotS / XrotM of the eye offset (mDemoCamDist) around the boss |
| 0x26C4 | `m25C8` | `mDemoCamFovy` | kamen_demo 0206B22C sets 45; demo_camera 0206FEF0 sets / eases 25..60 and passes it to dCamera_Set_bdk as the fovy |
| 0x26C8 | `m25CC` | `mDemoCamSpeed` | demo_camera 0206FEF0: clears it and scales the cLib_addCalc2 steps of mDemoCamCenter / mDemoCamEye by it |
| 0x26CC | `m25D0` | `mDemoCamDist` | demo_camera 0206FEF0 (t_down cutscene) sets 1500 and uses it as the z of the eye offset rotated by mDemoCamAngle |
| 0x26D0 | `m25D4` | `mDemoShakeAmp` | t_down 0206D0FC sets 10, demo_camera 0206FEF0 sets 0.75 / 2.5 (HD; GameCube 1.5 / 5.0) while the shutters close; demo_camera scales the sin/cos camera shake by it above 0.1 and eases it to 0 |
| 0x2708 | `m260C` | `mTakeoffSmokePos` | daBdk_Execute 02066A38: current.pos with y 9800, position of the TOBITATI_SMOKE00 (take-off smoke) particle |
| 0x2714 | `m2618` | `mTakeoffSmokeTimer` | wind_attack 0206AE64, up_fly / jump (in move 0206D644) set 0x19; daBdk_Execute 02066A38 counts it down and at 0 starts ID_IT_ST_DK_TOBITATI_SMOKE00 (the take-off smoke) at mTakeoffSmokePos with mTakeoffSmokeCb |
| 0x2715 | `m2619` | `mClawSparkOn` | fly_attack 0206A610 starts the ID_IT_SN_DK_TSUME_HIBANA claw-spark emitters (mp6214) at the feet on the first ground hit and sets it; daBdk_Execute 02066A38 plays JA_SE_CM_DK_NAIL and moves the foot effect points while it is set, and clears it (ending the emitters) when the feet leave the ground (y > 9810) or mAttackPart is 0 |
| 0x2716 | `m261A` | `mEffNext` | eff_hane_set 02065278: next slot of mEff to use |
| 0x2718 | `m261C` | `mEff` | daBdk_Draw 02065AB4 / my_effect_move 02071A68 loop over the 40 bdk_eff_s feather effects; eff_hane_set 02065278 fills them |
| 0x6138 | `m603C` | `mEffStts` | daBdk_Create 02069C8C: Init(0xFF, 0xFF), the Stts of each mEff[i].mSph sphere |
| 0x6174 | `m6078` | `mSmokeWaitTimer` | daBdk_Execute 02066A38 counts all 4 down; [i] gates the toon smoke started with mSmokeCb[i]: [0] the landing smoke (landing in move 0206D644, t_lastattack 0206CAD8, 100), [1] the stamping smoke (jida_attack in move, 10), [2] the mouth-rock smoke (kuti_attack in move, 0x3C), [3] the wall-hit smoke (fly_attack 0206A610, 100) |
| 0x617C | `m6080` | `mSmokeCb` | bdk_class_ct 02069964 / bdk_class_dt 020726D0 construct and destroy the 4 dPa_smokeEcallBack; daBdk_Create 02069C8C turns follow off, daBdk_Delete 020692E0 removes them; the ID_IT_ST_DK_TYAKUTI / JIDANDA / KUTI / BUTSUKARI smokes use [0] .. [3] (gated by mSmokeWaitTimer[i]) |
| 0x61FC | `m6100` | `mpDeadHaneEmitter` | end 0206BAA8: particle_set(ID_IT_SN_DK_DEAD_HANE_A00) emitters, cleared when done |
| 0x620C | `m6110` | `mTakeoffSmokeCb` | bdk_class_ct 02069964 constructs it; daBdk_Execute 02066A38 starts ID_IT_ST_DK_TOBITATI_SMOKE00 with it when mTakeoffSmokeTimer ends; daBdk_Create 02069C8C turns follow off, daBdk_Delete 020692E0 removes it |
| 0x622C | `m6130` | `mLidSmokeCb` | bdk_class_ct 02069964 / bdk_class_dt 020726D0 construct and destroy the 4 dPa_smokeEcallBack; demo_camera 0206FEF0 starts the ID_IT_ST_DK_FUTATOJI_SMOKE_A / B / C smokes (the tower lid closing) with them and ends [3]; daBdk_Create 02069C8C turns follow off, daBdk_Delete 020692E0 removes them |
| 0x62AC | `m61B0` | `mDemoFeatherCb` | bdk_class_ct 02069964 constructs the dPa_followEcallBack; start 0206B67C starts ID_IT_SN_DK_DEMO_HANE00 (the demo feathers) with it and ends it; daBdk_Delete 020692E0 removes it |
| 0x6320 | `m6224` | `mBgTevStr` | bdk_class_ct 02069964 constructs it, daBdk_Create 02069C8C copies tevStr into it; daBdk_Draw 02065AB4 sets it up as TEV_TYPE_BG3 (obj_draw) and lights the arena models (mp62D8, mp6310) and the mouth rock (mEff type 2) with it; landing / jida_attack / kuti_attack (in move 0206D644) and t_lastattack 0206CAD8 tint the rock particles with its mColorK0 |
| 0x64E8 | `m62D4` | `mTogeOffsetY` | obj_move 0206FC6C: y offset of the S_TTOGE arena model (mp62D8) and its pm_bgw; eased to -500 once STAGE_BOSS_ENEMY (dungeon item 3) is set; daBdk_Create 02069C8C sets -500 after the fight |
| 0x64F0 | `m62DC` | `mBgwMtx` | useHeapInit 02069428: cBgW_Set(pm_bgw, ..., &it); obj_move 0206FC6C copies the object matrix into it |
| 0x6530 | `m631C` | `mShutterRotY` | obj_move 0206FC6C: MtxRotY of each S_TSHUTTER model (mp6310[3]) inside its 120-degree slot; demo_camera 0206FEF0 adds 0.003 per frame while the shutters close and resets it to 0 |
| 0x6534 | `m6320` | `mShutterSlide` | obj_move 0206FC6C: z offset (MtxTrans) of each S_TSHUTTER model (mp6310[3]) after mShutterRotY; daBdk_Create 02069C8C sets -630 (open), demo_camera 0206FEF0 eases it to 0, sets -100 and eases it to 0 again (closed) |
| 0x6538 | `m6324` | `mShutterOffsetY` | obj_move 0206FC6C: y offset (+ 9800) of the S_TSHUTTER group (mp6310[3]); daBdk_Create 02069C8C sets -70, demo_camera 0206FEF0 eases it to 0 once mShutterSlide is within 1 |
| 0x6540 | `m632C` | `mObjBgwMtx` | useHeapInit 02069428: cBgW_Set(mp63BC[i], ..., &it[i]); obj_move 0206FC6C copies each object matrix into it |

Named in round 2 (the crest joints are DK_JNT_J_DK_TOSAKA_A1 .. B2, rotated in nodeCallBack 02065794):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03D0 | `m2B4` | `mTowerBattle` | daBdk_Create 02069C8C sets 1 and then starts ACTION_T_FLY with JA_BGM_MJ_TOWER_BATTLE; demo_camera 0206FEF0 clears it when the main battle starts (ACTION_FLY, JA_BGM_DK_BATTLE); while 1 daBdk_Execute 02066A38 scales the body 0.85, kankyo_cont 0206F9E0 makes the wind spheres 600 (else 300) and damage_check (in daBdk_Execute) sends a hammer hit to ACTION_T_DOWN (tww the same; tww's create also loads fopAcM_GetParam first) |
| 0x040C | `m2F8` | `mDamageTimer` | damage_check (in daBdk_Execute 02066A38) takes no hit while it is set and sets 0xC / 0x1E / 3 / 6 / 0x14 after a hit; t_fly 0206C3AC sets 5, end 0206BAA8 / after_fight 0206D638 10; wind_attack 0206AE64 lands at once while it is set; daBdk_Execute counts it down (as bgn_class mDamageTimer) |
| 0x0410 | `m2FC` | `mFlyPoint` | fly (in move 0206D644): which fly home point is used (0 = 6500 / 11000 / 2600, 1 = 9800 / 12000 / -1500, 2 = -1000 / 12000 / -2900); chosen at random, then alternates 1 / 2 (20 % back to 0); at 0 the arrival always lands, at 1 / 2 a fly_attack may follow |
| 0x0A80 | `m970` | `mMaskPieceSpeed` | kamen_demo 0206B22C sets [i] to the break direction of mask piece i (kamen_break_sd_x / z); kamen_break_move (in daBdk_Execute 02066A38) adds it to mMaskPiecePos, lowers .y by 5 per frame (gravity), bounces it on the ground and sets .x / .z from mMaskPieceSlideSpeed / mMaskPieceSlideYaw or a hit |
| 0x0AEC | `m9EC` | `mMaskPieceRockAmp` | kamen_break_move (in daBdk_Execute 02066A38): set 3000 when mask piece i bounces, then eased to 0; the piece's z angle rocks by sin * it |
| 0x0AFC | `m9FC` | `mMaskPieceSlideSpeed` | kamen_break_move (in daBdk_Execute 02066A38): a type-8 hit sets [i] to 7 + rnd(2); while on the ground mask piece i slides at it along mMaskPieceSlideYaw and spins its y angle by mMaskPieceSpinRate * it; eased to 0, cleared on another hit or a wall |
| 0x0B0C | `mA0C` | `mMaskPieceSlideYaw` | kamen_break_move (in daBdk_Execute 02066A38): set from the hitting actor's shape_angle.y + rnd(+-2000) on a type-8 hit; the direction of mask piece i's mMaskPieceSlideSpeed |
| 0x1000 | `mF00` | `mMaskPieceSpinRate` | kamen_break_move (in daBdk_Execute 02066A38): set rnd(+-200) on a type-8 hit; mask piece i's y angle turns by it * mMaskPieceSlideSpeed per frame |
| 0x121C | `m1120` | `mTosakaYaw` | head_swing (in daBdk_Execute 02066A38) eases it to the head-velocity angle + a sway + 4500; nodeCallBack 02065794 rotates the crest joints (TOSAKA_A1 .. B2) about y by it (A side negated); daBdk_Create 02069C8C sets -2000 |
| 0x121E | `m1122` | `mTosakaRoll` | head_swing (in daBdk_Execute 02066A38) eases it to 4; nodeCallBack 02065794 rotates the crest joints about z by it (A side negated) |
| 0x1220 | `m1124` | `mTosakaWobbleYawB` | head_swing (in daBdk_Execute 02066A38): sin(mTosakaPhase * 1500) * mTosakaWobbleAmp; nodeCallBack 02065794 adds it to the y rotation of the B crest joints |
| 0x1222 | `m1126` | `mTosakaWobbleRollB` | head_swing (in daBdk_Execute 02066A38): cos(mTosakaPhase * 1200) * mTosakaWobbleAmp; nodeCallBack 02065794 adds it to the z rotation of the B crest joints |
| 0x1224 | `m1128` | `mTosakaWobbleYawA` | head_swing (in daBdk_Execute 02066A38): sin(mTosakaPhase * 1500 + 0x4000) * mTosakaWobbleAmp; nodeCallBack 02065794 adds it to the (negated) y rotation of the A crest joints |
| 0x1226 | `m112A` | `mTosakaWobbleRollA` | head_swing (in daBdk_Execute 02066A38): cos(mTosakaPhase * 1200 + 0x4000) * mTosakaWobbleAmp; nodeCallBack 02065794 adds it to the (negated) z rotation of the A crest joints |
| 0x1228 | `m112C` | `mTosakaHitRoll` | head_swing (in daBdk_Execute 02066A38): while mTosakaHitTimer runs, sin(frame * 7000) * the x of the hit direction (mHitAngleY) * timer * 500; nodeCallBack 02065794 adds it to the z rotation of all crest joints |
| 0x122A | `m112E` | `mTosakaHitYaw` | head_swing (in daBdk_Execute 02066A38): while mTosakaHitTimer runs, sin(frame * 7000) * the z of the hit direction (mHitAngleY) * timer * 500; nodeCallBack 02065794 adds it to the y rotation of all crest joints |
| 0x122C | `m1130` | `mTosakaPhase` | head_swing (in daBdk_Execute 02066A38) adds 1 per frame (2 while mTosakaWobbleTarget is above 3000): the phase of the crest sway and wobble terms |
| 0x122E | `m1132` | `mTosakaWobbleAmp` | head_swing (in daBdk_Execute 02066A38) eases it to mTosakaWobbleTarget (step 500) and multiplies the four wobble terms by it; cleared while a hit shake runs or the swing is off |
| 0x1230 | `m1134` | `mTosakaWobbleTarget` | head_swing (in daBdk_Execute 02066A38) eases mTosakaWobbleAmp to it and resets it to 1500 each frame; kuti_attack (in move 0206D644) sets 2500 each frame of the attack, kamen_demo 0206B22C 500, end 0206BAA8 4000 |
| 0x1234 | `m1138` | `mTosakaHitTimer` | damage_check (in daBdk_Execute 02066A38) sets 30 on a crest hit (0 on a mask hit), kuti_attack (in move 0206D644) 30 at the beak strike frame; daBdk_Execute counts it down; head_swing shakes the crest by it (mTosakaHitRoll / mTosakaHitYaw) |
| 0x1236 | `m113A` | `mTosakaSwingOn` | kamen_demo 0206B22C sets 1 once the mask is broken; head_swing (in daBdk_Execute 02066A38) moves the crest only while set and clears the crest terms otherwise |
| 0x1238 | `m113C` | `mTosakaTurnRate` | head_swing (in daBdk_Execute 02066A38) eases it from 0 to 1; mTosakaYaw / mTosakaRoll move at most 4000 * it per frame |
| 0x123C | `m1140` | `mTosakaSwayScale` | head_swing (in daBdk_Execute 02066A38) eases it from 0 to 1 once mDemoTimer passes 0x41; the sway term of mTosakaYaw is sin * -2000 * it |
| 0x1240 | `m1144` | `mHeadPosOld` | col_set (in daBdk_Execute 02066A38) copies mHeadPos into it before updating mHeadPos; head_swing takes mHeadPos - it as the head movement |
| 0x124C | `m1150` | `mHeadPos` | col_set (in daBdk_Execute 02066A38): the head point (ATAMA1 joint + offset), centre of mHeadTgSph; damage_check tests .y < 10000; demo_camera 0206FEF0 centres the camera on it |
| 0x1264 | `m1168` | `mBodyCenter` | col_set (in daBdk_Execute 02066A38): current.pos + 380 in y, centre of mBodyCCSph; fly_attack 0206A610, wind_attack 0206AE64, t_down 0206D0FC, landing / kuti_attack (in move 0206D644) spawn feathers there (eff_hane_set) |
| 0x269A | `m259E` | `mBlurRate` | daBdk_Draw 02065AB4 sets l_blur.mRate to it while above 1 and clears it at 1; t_down 0206D0FC sets 0xB4 and eases it to 1; demo_camera 0206FEF0 clears it (as bgn_class mBlurRate) |
| 0x269E | `m25A2` | `mFinalHitHigh` | damage_check (in daBdk_Execute 02066A38) (end_set) sets 1 when health runs out while current.pos.y is at least 9810 (end state 2); demo_camera 0206FEF0 then lets the eye height follow the boss instead of keeping 9950 |
| 0x26D4 | `m25D8` | `mDemoStepReq` | demo_camera 0206FEF0 sets 1 at given cutscene frames; start 0206B67C goes to its next state (fly up, then the S_DEMO2 animation) when it is set and clears it |

HIO block daBdk_HIO_c (offsets within the struct; constructor daBdk_HIO_c_ct 0206A43C, defaults as tww):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x01 | `m005` | `mStopMove` | daBdk_Execute 02066A38 skips the animation play, move, ground check and damage_check while set; daBdk_HIO_c_ct 0206A43C sets 0 (tww the same) |
| 0x04 | `m008` | `mBodyScale` | daBdk_Execute 02066A38 sets actor scale and model scale to it (0.85 * it in the tower battle, mTowerBattle); daBdk_HIO_c_ct 0206A43C sets 1.0 (tww the same) |
| 0x08 | `m00C` | `mTailScale` | daBdk_Draw 02065AB4 (tail_draw) scales each tail feather part by tial_scale * it; tail_control (in daBdk_Execute 02066A38) multiplies the part spacing by it; daBdk_HIO_c_ct 0206A43C sets 4.0 (tww the same) |
| 0x0C | `m010` | `mTailSpacing` | tail_control (in daBdk_Execute 02066A38): the feather part spacing is 2 * 20 * (0.25 + 0.03 * i) * mTailScale * it; daBdk_HIO_c_ct 0206A43C sets 1.5 (tww the same) |
| 0x10 | `m014` | `mForceAttack` | wait (in move 0206D644): when the wait ends, 1 always starts kuti_attack, 2 always wind_attack, 0 picks by distance; daBdk_HIO_c_ct 0206A43C sets 0 (tww the same) |
| 0x14 | `m018` | `mWalkSpeed` | wait (in move 0206D644) state 2 (walking to the player) sets mTargetSpeed to it; daBdk_HIO_c_ct 0206A43C sets 20 (tww the same) |
| 0x18 | `m01C` | `mWalkAnmSpeed` | wait (in move 0206D644) starts the WALK1 animation with it as the play speed; daBdk_HIO_c_ct 0206A43C sets 2.0 (tww the same) |
| 0x1C | `m020` | `mFlySpeed` | fly (in move 0206D644) sets mTargetSpeed to it; daBdk_Create 02069C8C starts with speedF = it; daBdk_HIO_c_ct 0206A43C sets 60 (tww the same) |
| 0x20 | `m024` | `mFlyAttackSpeed` | fly_attack 0206A610 sets mTargetSpeed to it while it chases the player; daBdk_HIO_c_ct 0206A43C sets 100 (tww the same) |
| 0x24 | `m028` | `mTowerAttackWait` | t_fly 0206C3AC (tower battle) sets mTimers[0] to it after starting a foot attack (ATTACK_T2), the wait before the next one; daBdk_HIO_c_ct 0206A43C sets 0x2D0 (tww the same) |

Open (no name yet): `m25DC` / `m25E8` / `m25F4` / `m2608` (kankyo_cont writes the position, facing direction and wind
term every frame, wind_attack raises `m2608`; nothing in WWHD or tww reads the vectors), `m61C4` (follow callbacks, only
constructed and destroyed), the `mp`-prefixed pointers (`mp8F0`, `mp2598`, `mp6214`, `mp62D8`, `mp6310`, `mp63BC`, `mp63C8`;
not in the census pattern), unused `m259C` / `m2600` / `m6328`, HIO `m006` (constructor only).

Tail helper struct bdk_tail_s (offsets within the struct; bdk_class mTails[4], one per tail feather chain):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0000 | `m000` | `mpModel` | useHeapInit 02069428 creates the 9 segment models of each tail; daBdk_Draw 02065AB4 (tail_draw) gives each its matrix and draws it |
| 0x0024 | `m024` | `mPos` | tail_control (in daBdk_Execute 02066A38) sets [0] to mRootPos[1] and chains [1..9] behind it (not below the ground + 5); daBdk_Draw 02065AB4 (tail_draw) translates the segment models there |
| 0x009C | `m09C` | `mAngle` | tail_control (in daBdk_Execute 02066A38) sets [i - 1] .y / .x to the heading / minus the pitch toward segment i; daBdk_Draw 02065AB4 (tail_draw) rotates the segment models by it |
| 0x00D8 | `m0D8` | `mSpeed` | tail_control (in daBdk_Execute 02066A38) adds it to the segment position, then stores (new - old position) * 0.77 in it |
| 0x0150 | `m0150` | `mRootPos` | nodeCallBack 02065794 sets [1] to the tail joint position and [0] to the point 10 behind it; tail_control (in daBdk_Execute 02066A38) aims the tail along [0] -> [1] and starts the chain at [1] |
| 0x0168 | `m0168` | `mRootAngle` | tail_control (in daBdk_Execute 02066A38) sets .y / .x to the heading / minus the pitch of mRootPos[0] -> [1] |
| 0x0170 | `m0170` | `mRootStep` | tail_control (in daBdk_Execute 02066A38) sets it to 30 units along mRootAngle and pushes segment i by it * (1 - 0.1 * (i - 1)) |

Effect helper struct bdk_eff_s (offsets within the struct; bdk_class mEff[40]: the feathers, and in mEff[0x27] the
rock of the mouth attack). mPosOld, mSpeedF, mSpeedY, mScale, mAngle and mFrameCounter follow the same uses in
bdkobj_class bdo_eff_s:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0000 | `m000` | `mType` | eff_hane_set 02065278 sets 1 (a feather), kuti_attack (in move 0206D644, mEff[0x27]) 2 (the rock) and 0, damage_check (in daBdk_Execute 02066A38) 0; my_effect_move 02071A68 and daBdk_Draw 02065AB4 (my_effect_draw) handle 1 / 2 and skip 0; eff_hane_move (in my_effect_move) clears it when the vanished feather is small |
| 0x0001 | `m001` | `mVanish` | eff_hane_set 02065278 clears it; eff_hane_move (in my_effect_move 02071A68) sets 1 below y 9700, when mLifeTimer ends or off the land area, then shrinks mScale and frees the slot |
| 0x0004 | `m004` | `mPos` | eff_hane_set 02065278 sets it (scattered by +-500 for param_4 < 0); eff_hane_move (in my_effect_move 02071A68) moves it by mSpeedF / mSpeedY, keeps it on the ground, places the model and mSph there; kuti_attack (in move 0206D644) places the rock and its particles there |
| 0x0010 | `m010` | `mPosOld` | eff_hane_move (in my_effect_move 02071A68) copies mPos before moving; on a line hit x / z go back to it |
| 0x001C | `m01C` | `mSpeedY` | eff_hane_set 02065278 sets 5..10 (or -10..-5 for a thrown feather); eff_hane_move (in my_effect_move 02071A68) adds it to mPos.y and lowers it by 0.1 per frame to -5; a hit sets 10..15 |
| 0x0020 | `m020` | `mSpeedF` | eff_hane_set 02065278 sets 5..30; eff_hane_move (in my_effect_move 02071A68) moves mPos along mAngle.y by it, clears it on a wall, eases it to 0 on the ground; a hit sets 10..15 |
| 0x0024 | `m024` | `mPivotHeight` | eff_hane_set 02065278 sets 0; eff_hane_move (in my_effect_move 02071A68) eases it to -130 / -300 high up (above y 9930) and to 0 below, and tilts the model (mAngle.x / .z) around the point it below mPos |
| 0x0028 | `m028` | `mFlutterAmplitude` | eff_hane_move (in my_effect_move 02071A68) eases it to 8000 (15000 while mBlowTimer runs) in the air and tilts the feather by sin / cos(mFrameCounter) * it |
| 0x002C | `m02C` | `mScale` | eff_hane_set 02065278 sets 1; eff_hane_move (in my_effect_move 02071A68) scales the model by it and shrinks it to 0 while mVanish is set |
| 0x0030 | `m030` | `mAngle` | eff_hane_set 02065278 sets .y (the flight heading); eff_hane_move (in my_effect_move 02071A68) moves along .y, turns .y by mSpin.x on the ground, sets .x / .z from mFlutterAmplitude in the air and eases them to 0 on the ground; a hit turns .y away from the hitter |
| 0x0036 | `m036` | `mSpin` | eff_hane_set 02065278 sets .x / .z to random spin speeds; eff_hane_move (in my_effect_move 02071A68) adds .z to .y (the model yaw), adds .x to mAngle.y on the ground and eases .x / .z to 0 there; a hit sets new .x / .z |
| 0x003C | `m03C` | `mFrameCounter` | eff_hane_set 02065278 sets a random start, kuti_attack (in move 0206D644) 0; my_effect_move 02071A68 adds 1 per frame; the phase of the flutter tilt; eff_Grock_move lowers the rock by 10 at 1 |
| 0x003E | `m03E` | `mLifeTimer` | eff_hane_set 02065278 sets 2000; eff_hane_move (in my_effect_move 02071A68) counts it down and sets mVanish at 0 |
| 0x0040 | `m040` | `mBlowTimer` | eff_hane_set 02065278 sets 0 (20..40 for a thrown feather); a hit in eff_hane_move (in my_effect_move 02071A68) sets 30..60; counted down; while it runs the feather takes no hits and flutters harder (15000, pivot -300) |
| 0x0044 | `m044` | `mpModel` | useHeapInit 02069428 creates the feather / rock model of each slot; eff_hane_move / eff_Grock_move (in my_effect_move 02071A68) set its matrix; daBdk_Draw 02065AB4 (my_effect_draw) draws it |
| 0x0048 | `m048` | `mSph` | bdk_eff_s_ct 0207260C / bdk_eff_s_dt 0207267C construct and destroy it; daBdk_Create 02069C8C sets it up with eff_sph_src and mEffStts; eff_hane_move (in my_effect_move 02071A68) places it at mPos while the feather lives and blows the feather away when it is hit |

Open in bdk_tail_s / bdk_eff_s: the padding only. The view `BdkBaseFragment` of bdkobj_class's bdo_eff_s
(d_a_bdk_base.h, used by obj_draw in d_a_bdk.cpp) now uses the bdo_eff_s names (`mState`, `mPos`,
`mHorizontalSpeedX` / `Z`, `mAngleSpeed`). zeldaret/tww (CC0) has offset names only for these fields.

## bwd_class (Molgera; d_a_bwd.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03E4 | `m02C8` | `mHeadBrkSpeed` | end 020FEA30 sets 1.0; bwd_execute 020FA614 passes it to mpHeadBrkAnm->setPlaySpeed |
| 0x04E0 | `m03C4` | `mBodyAnmSpeed` | bwd_execute 020FA614 passes [i] to mpBodyMorf[i]->setPlaySpeed (and skips even segments below 0.1); end 020FEA30 sets them to 1 one by one (mBodyAnmStartNo) and tests >= 0.5 |
| 0x0580 | `m0414` | `mBodyAnmStartNo` | end 020FEA30: every 4th frame (while mTimers[0] is 0) sets mBodyAnmSpeed[it] = 1 and adds 1, up to 20 |
| 0x0584 | `m0418` | `mBodyPos` | bwd_execute 020FA614: MtxPosition per body segment (sand particle 0x8241 there); end 020FEA30 bursts segment [mBodyExplodeNo] (SIBOUBAKUEN / SIBOUSUNA, MONS_EXPLODE); demo_camera 02101F30 eases its centre to it; daBwd_Delete 020FD578 deletes their sound objects |
| 0x0674 | `m0508` | `mPosHistory` | historyPosition() (d_a_bwd_execute.h) indexes it as a 256-entry ring; end 020FEA30 fills it with current.pos |
| 0x1274 | `m1108` | `mAngleHistory` | historyAngles() (d_a_bwd_execute.h) indexes it as a 256-entry ring; bwd_execute 020FA614 stores current.angle at [mHistoryIndex] |
| 0x1874 | `m1708` | `mHistoryIndex` | bwd_execute 020FA614: ring index for historyAngles / historyPosition; the body segments read entry mHistoryIndex - (i + 1) * mHistoryStep |
| 0x1878 | `m170C` | `mDrawBody` | end 020FEA30 sets 1; daBwd_Draw 020F9C1C draws the body segment models only while it is set; bwd_execute 020FA614 clears it every frame |
| 0x187C | `m1710` | `mBgmStartTimer` | sita_hit (in move 020FF80C) sets 10 when it is 0; bwd_execute 020FA614 counts it down to 1 and, once no larva is left (ko_count 0), starts the battle BGM and clears it |
| 0x1880 | `m1714` | `mPlatformTevStr` | daBwd_Draw 020F9C1C: settingTevStruct(TEV_TYPE_BG0, ...) into it, then setLightTevColorType for mpTriforcePlatformModel and mpSandfallModel[i] |
| 0x1A48 | `m17C4` | `mSandWaveMode` | daBwd_Execute 020FA614 eases the sand-wave terms by it: 0xFF rise, 200 calm, 0xC9 / 0xCA sink, any other value counts down to 0 (then they ease to 0); start / wait / sita_hit / eat_attack / fly / s_fly (in move 020FF80C), demo_camera 02101F30 and fly_pos_move 020F9F80 set it |
| 0x1A4C | `m17C8` | `mSandDentCenter` | wait / eat_attack (in move 020FF80C) copy current.pos, fly / s_fly copy mSandSurfacePos; demo_camera 02101F30 sets x / z 0; wave_cont 02103FA4 (bwdg) measures every vertex's distance from it; daBwd_Execute 020FA614 measures the player's |
| 0x1A58 | `m17D4` | `mSandDentDepth` | daBwd_Execute 020FA614 eases it per mSandWaveMode (0.35, -0.3, -0.7, -0.75; else 0); wave_cont 02103FA4 (bwdg) adds ((3000 - distance) * 0.01)^2 * it to the vertex heights around mSandDentCenter |
| 0x1A5C | `m17D8` | `mWaveSpeed` | daBwd_Execute 020FA614 eases it per mSandWaveMode (-0x5DC, 0, -500, 1000; else 0); start (in move 020FF80C) sets 500; wave_cont 02103FA4 (bwdg) adds it to mWavePhase each frame |
| 0x1A60 | `m17DC` | `mWaveStepRate` | daBwd_Execute 020FA614 eases it to 1 (to 0 when mSandWaveMode is 0); wave_cont 02103FA4 (bwdg) multiplies the cLib_addCalc2 step of every vertex height by it |
| 0x1A64 | `m17E0` | `mSandBaseY` | wave_cont 02103FA4 (bwdg) sets every vertex y = mWaveHeight + it; start (in move 020FF80C) / demo_camera 02101F30 set or ease it to 200 / 400 / 900; daBwd_Execute 020FA614 raises the platforms to it + 50 |
| 0x1A68 | `m17E4` | `mSandfallState` | daBwd_Create 020FE0B4 / demo_camera 02101F30 start it (1 / + 1); bwd_execute 020FA614 steps each sandfall: 2 starting (SUNATAKI smoke at 0x32), 3 TAKI_WAIT loop, 4 / 5 TAKI_END, 0 off; daBwd_Draw 020F9C1C draws the sandfall while it is not 0 |
| 0x1A6A | `m17E6` | `mSandfallTimer` | bwd_execute 020FA614: cleared when a sandfall state starts, + 1 per frame; compared with 0x32 / 0xB4 / 0x3C for the state steps and the SANDFALL sound |
| 0x1A70 | `m17EC` | `mpSandfallModel` | useHeapInit 020FD6F8 creates the 2 sandfall models; bwd_execute 020FA614 places them (YrotM mSandfallAngle) and moves their dBgW; daBwd_Draw 020F9C1C draws them with mPlatformTevStr |
| 0x1A78 | `m17F4` | `mpSandfallBtk` | useHeapInit 020FD6F8: btk of each sandfall model; bwd_execute 020FA614 re-inits it with TAKI_WAIT / TAKI_END and plays it; daBwd_Draw 020F9C1C enters it |
| 0x1AE8 | `m1864` | `mPlatformShakeTimer` | demo_camera 02101F30 sets 10; bwd_execute 020FA614 counts it down and sets a random mPlatformShakeOffset.x while it runs |
| 0x1AE9 | `m1865` | `mPlatformHidden` | daBwd_Create 020FE0B4 sets 1, demo_camera 02101F30 clears it; daBwd_Draw 020F9C1C draws the triforce platform only at 0, bwd_execute 020FA614 moves it to y -20000 while it is set |
| 0x1B24 | `m18A0` | `mPlatformShakeOffset` | bwd_execute 020FA614: x random while mPlatformShakeTimer runs; the triforce platform model is placed at it (+ l_HIO heights) |
| 0x1B30 | `m18AC` | `mFrameCounter` | bwd_execute 020FA614 adds 1 per frame (move 020FF80C clears it); control1 / end 020FEA30 use it as a sin phase and test its low bits |
| 0x1B32 | `m18AE` | `mAction` | move 020FF80C: switch (it) selects the action function (0: start); damage_check (in bwd_execute 020FA614) sets 3, daBwd_Create 020FE0B4 0/1/0xD |
| 0x1B34 | `m18B0` | `mMode` | move 020FF80C / end 020FEA30: switch (it) inside each action; damage_check (in bwd_execute 020FA614) clears it with each new action |
| 0x1B38 | `m18B4` | `mTargetPos` | fly_pos_move 020F9F80 flies toward it (x/y/z minus current.pos) |
| 0x1B48 | `m18C4` | `mTurnSpeed` | end 020FEA30 sets 1000 (+REG); fly_pos_move 020F9F80: angle step = it * mTurnSpeedRate |
| 0x1B4C | `m18C8` | `mTurnSpeedRate` | fly_pos_move 020F9F80 multiplies mTurnSpeed by it and eases it to 1 |
| 0x1B50 | `m18CC` | `mTimers` | bwd_execute 020FA614 counts all 5 down each frame (zeldaret/tww daBwd_Execute: m18CC[i] for i < 5; view Timers::mTimers); wait / reset / sita_hit / eat_attack / fly / s_fly (in move 020FF80C) and end 020FEA30 load [0] / [1] and act at 0 or at a given count (as bgn_class / bwds_class mTimers). Declared [5]: [2] is the former mActionTimer (m18D0), [3] / [4] the former padding (never loaded) |
| 0x1B54 | `m18D0` | `mTimers[2]` | wait / eat_attack / fly / s_fly (in move 020FF80C) set 0xDE / 100 / 0xB4 / 0x12E and act at 0 or at 0x50 (was declared `mActionTimer`; folded into mTimers[5]) |
| 0x1B5A | `m18D6` | `mTongueNoHitTimer` | damage_check (in bwd_execute 020FA614) checks the tongue for hits only while it is 0 and sets 6 after a hit; sita_move 5, fly (in move 020FF80C) 10; bwd_execute 020FA614 counts it down |
| 0x1B5C | `m18D8` | `mBwdgCreated` | bwd_execute 020FA614 creates the BWDG (sand surface) actor once and sets 1 |
| 0x1B64 | `m18E0` | `mItemId` | demo_camera 02101F30: ID of the item created at mItemPos (bwdCameraCreateItem), looked up with fopAcM_SearchByID |
| 0x1B68 | `m18E4` | `mItemPos` | end 020FEA30 copies current.pos; demo_camera 02101F30 creates the item there |
| 0x1B74 | `m18FC` | `mSandCrossWait` | fly_pos_move 020F9F80 checks the head crossing the sand surface (dive / out splash) only while it is 0 and sets 30 after one; bwd_execute 020FA614 counts it down |
| 0x1B76 | `m18FE` | `mWobblePhaseSin` | end 020FEA30 adds 0x834 per frame; its sin * 7000 is the sideways offset passed to fly_pos_move, and the body segments wobble by sin(it - i * 0x3A98) until they harden |
| 0x1B78 | `m1900` | `mWobblePhaseCos` | end 020FEA30 adds 0x9C4 per frame; its cos * 7000 is the second offset passed to fly_pos_move |
| 0x1B7A | `m1902` | `mRollPhase` | bwd_execute 020FA614: + 1000 per frame; the roll added to the history angle is sin(it) * 4000 |
| 0x1B7C | `m1904` | `mHistoryStep` | end 020FEA30 sets 3, fly (in move 020FF80C) 5 (+REG); bwd_execute 020FA614: history entries between body segments |
| 0x1E00 | `m1B88` | `mTongueTipPos` | control2 (in bwd_execute 020FA614) puts the last tongue segment (mTongueSegments[29]) at it; sita_move (in bwd_execute) eases it toward the hookshot owner / mTonguePulledPos / the mouth and keeps it on the ground; the tongue loses attention while it is below mSandSurfacePos.y; daBwd_Create 020FE0B4 sets y -5000 / -20000 / current.pos.y, end 020FEA30 -20000 |
| 0x1E0C | `m1B94` | `mTonguePulledPos` | sita_move (in bwd_execute 020FA614) copies mTongueTipPos into it while mTongueHooked is set, then (while mTonguePulledTimer runs) pulls it to within 1200 of the body and eases the tip to it (shaken by mTongueHitShakeTimer) |
| 0x1E18 | `m1BA0` | `mTongueSegLength` | sita_move (in bwd_execute 020FA614) eases it to 0.035 * the mouth-to-tip distance (at least 20 while mTongueRetract is set) and ends the hookshot pull above 50; control1 / control2 (in bwd_execute) place each tongue segment this far from the previous one |
| 0x1E20 | `m1BA8` | `mTongueWaveAmplitude` | sita_move (in bwd_execute 020FA614) sets 10 each frame; control1 (in bwd_execute) swings the free tongue segments by sin / cos * it (mTongueHitShakeTimer * 4.5 instead while that runs) |
| 0x1E24 | `m1BAC` | `mTongueTipStepScale` | sita_move (in bwd_execute 020FA614) moves the tip by at most 100 * it per frame, clears it while mTongueHooked is set and eases it back to 1; sita_hit (in move 020FF80C) sets 1 |
| 0x1E28 | `m1BB0` | `mTongueHooked` | damage_check (in bwd_execute 020FA614) sets 1 when the hookshot (AtType 0x8000) hits the tongue, with mHitOwnerId and mAction 3; sita_move (in bwd_execute) pulls the tip to the owner while it is set and clears it when the hookshot returns or mTongueSegLength exceeds 50; control1 (in bwd_execute) stops the tongue wave while it is set |
| 0x1E29 | `m1BB1` | `mTonguePulledTimer` | sita_move (in bwd_execute 020FA614) sets l_HIO.m14 while mTongueHooked is set and counts it down after; damage_check (in bwd_execute) makes the tongue hittable by all attacks (TgType 0xFF1DFEFF) and counts mTongueHitCount only while it runs; sita_hit (in move 020FF80C) ends at 0 and clears it after a damage round; control1 lowers the tongue (y -5) while it runs |
| 0x1E2A | `m1BB2` | `mTongueThrashTimer` | sita_hit (in move 020FF80C) sets 0x32 after a damage round and for the last hit; sita_move (in bwd_execute 020FA614) counts it down and swings the tip around the mouth by 400 * sin |
| 0x1E2B | `m1BB3` | `mTongueHitShakeTimer` | damage_check (in bwd_execute 020FA614) sets 20 on a counted tongue hit; sita_move counts it down and shakes the tip by it * 3; control1 (in bwd_execute) shakes the segments by it * 4.5 |
| 0x1E2C | `m1BB4` | `mTongueHitCount` | damage_check (in bwd_execute 020FA614) adds 1 per tongue hit; sita_hit (in move 020FF80C) acts below 3, then clears it |
| 0x1E2D | `m1BB5` | `mTongueDamageCount` | sita_hit (in move 020FF80C) adds 1 per finished damage round (mTongueHitCount reaching 3); at 2 it switches to the flying form (mAction 10), at 4 (or l_HIO.m06) to the death (mAction 0xC) |
| 0x1E2E | `m1BB6` | `mTongueRetract` | bwd_execute 020FA614 clears it every frame; start / sita_hit / eat_attack (in move 020FF80C) and end 020FEA30 set 1, fly / s_fly (in move) and end 2; sita_move (in bwd_execute) pulls the tip into the mouth while it is set and drops attention unless it is 2; daBwd_Draw 020F9C1C draws the tongue only below 2 |
| 0x1E30 | `m1BB8` | `mPlayerDistXZ` | bwd_execute 020FA614: XZ distance to the player; wait (in move 020FF80C) compares it with 80 and l_HIO.m18 |
| 0x1E34 | `m1BBC` | `mHitOwnerId` | damage_check (in bwd_execute 020FA614): ID of the Stts owner of the hit (else 0xFFFFFFFF); sita_move looks it up |
| 0x1E38 | `m1BC0` | `mAttId` | daBwd_Create 020FE0B4: fopAcM_create(fpcNm_ATT_e, 0x65, ...); end 020FEA30 / damage_check look it up |
| 0x3BC4 | `m394C` | `mPushPlayer` | start (in move 020FF80C) sets 1; bwd_execute 020FA614 pushes the player away from mSandDentCenter once (setOutPower) when below y 900 and clears it |
| 0x3BCC | `m3954` | `mSandSurfacePos` | bwd_execute 020FA614 sets y to the ground below the body; fly_pos_move 020F9F80, the actions (in move 020FF80C) and end 020FEA30 set x / z to the body and start the dive / out / hit sand effects and sounds there; mSandDentCenter and mSplashPos copy it; daBwd_Delete 020FD578 deletes its sound object |
| 0x3BD8 | `m3960` | `mFaceSandState` | g_eff_on 020F99F8 sets 1, g_eff_off 020F9A10 -1; bwd_execute 020FA614 starts the KAODASHI_SUNA (sand around the head) particle with mFaceSandCb at 1 (then 2, playing SAND_MOVE), ends it at -1 and clears it |
| 0x3BDC | `m3964` | `mpHitEmitter` | sita_hit (in move 020FF80C): particle_set(ID_IT_SN_BWO_HIT_TAIEKI00), made invalid when done |
| 0x3BE0 | `m3968` | `mpLastHitEmitter` | end 020FEA30: particle_set(ID_IT_SN_BWO_LASTHIT_TAIEKI00), made invalid when done |
| 0x3BF0 | `m3978` | `mSmokeCb` | the 10 sand smoke callbacks: fly_pos_move 020F9F80 [0] / [1] (dive / out), move 020FF80C [2..5], end 020FEA30 [6]; bwd_execute 020FA614 ends [i] when its smoke timer runs out; daBwd_Delete 020FD578 removes them |
| 0x3D30 | `m3AB8` | `mSandFollowCb` | fly_pos_move 020F9F80 passes [0] / [1] to the dive / out sand particles; bwd_execute 020FA614 ends them with mSmokeCb[0] / [1]; daBwd_Delete 020FD578 removes them |
| 0x3D58 | `m3AE0` | `mSmokeTimer` | fly_pos_move 020F9F80 sets [0] / [1] to 0x6E with the dive / out smoke; bwd_execute 020FA614 counts them down (with mSmokeTimer2 .. 6, ten words, view EffectTimers::mEffect) and ends mSmokeCb[0] / [1] at 0 |
| 0x3D5C | `m3AE4` | `mSmokeTimer2` | sita_hit (in move 020FF80C) sets 0x4D when it is 0 (smoke mSmokeCb[2]); bwd_execute 020FA614 counts it down and ends that smoke at 0 |
| 0x3D5E | `m3AE6` | `mSmokeTimer3` | eat_attack (in move 020FF80C) sets 10 (+ reg) at head frame 0x1B (smoke mSmokeCb[3]); bwd_execute 020FA614 counts it down and ends that smoke at 0 |
| 0x3D60 | `m3AE8` | `mSmokeTimer4` | eat_attack (in move 020FF80C) sets 0x3C when it is 0 (smoke mSmokeCb[4]); bwd_execute 020FA614 counts it down and ends that smoke at 0 |
| 0x3D62 | `m3AEA` | `mSmokeTimer5` | eat_attack (in move 020FF80C) sets 0x14 when it is 0 (smoke mSmokeCb[5]); bwd_execute 020FA614 counts it down and ends that smoke at 0 |
| 0x3D64 | `m3AEC` | `mSmokeTimer6` | end 020FEA30 sets 0x28 when it is 0 (smoke mSmokeCb[6]); bwd_execute 020FA614 counts it down and ends that smoke at 0 |
| 0x3D6C | `m3AF4` | `mFaceSandCb` | bwd_execute 020FA614 passes it to the KAODASHI_SUNA particle and ends it (mFaceSandState); daBwd_Delete 020FD578 removes it |
| 0x3D80 | `m3B08` | `mSplashFrame` | fly_pos_move 020F9F80 sets [0] (out) / [1] (dive) to 1; bwd_execute 020FA614: + 1 per frame up to 0x3C, then 0; it - 1 is the frame of the GSP splash model; daBwd_Draw 020F9C1C draws the splash while it is set |
| 0x3D84 | `m3B0C` | `mSplashPos` | fly_pos_move 020F9F80 copies the sand crossing point (mSandSurfacePos); bwd_execute 020FA614 places the splash model there |
| 0x3DC0 | `m3B48` | `mSandfallAngle` | bwd_execute 020FA614: y = demo_ang[i] (+ reg) per sandfall; YrotM of the sandfall model by it |
| 0x3DCC | `m3B54` | `mSandfallSmokeCb` | bwd_execute 020FA614 passes [i * 3 + j] to the 3 SUNATAKI_SMOKEA particles of sandfall i and removes them when it ends; daBwd_Delete 020FD578 removes them |
| 0x3E8C | `m3C14` | `mKankyoTimer` | fly_pos_move 020F9F80 sets 0x5A; bwd_kankyo (in bwd_execute 020FA614) counts it down and leaves mKankyoState 2 at 0 |
| 0x3E8D | `m3C15` | `mKankyoState` | daBwd_Create 020FE0B4 sets 5 / 1, fly_pos_move 020F9F80 2, demo_camera 02101F30 1 / 5; bwd_kankyo (in bwd_execute 020FA614) picks the dKy_custom_colset pair by it (0..5) |
| 0x3E90 | `m3C18` | `mEnvColorRate` | bwd_kankyo (in bwd_execute 020FA614): setEnvironmentColor(..., it), eased to 1 |
| 0x3E94 | `m3C1C` | `mBlurRate` | daBwd_Draw 020F9C1C: mDoGph_setBlureRate(it) while it is >= 1 |
| 0x3E96 | `m3C1E` | `mDemoMode` | demo_camera 02101F30: switch (it) of the demo camera steps, +1 per step; fly_pos_move 020F9F80 tests it |
| 0x3E98 | `m3C20` | `mDemoTimer` | demo_camera 02101F30: cleared per step and compared with frame marks (0x1E, 0x3C, ...) |
| 0x3E9A | `m3C22` | `mDemoMainTimer` | demo_camera 02101F30: cleared with mDemoTimer when a demo starts but not per step, + 1 per frame ("K MAIN COUNT"); compared with 0xAA, 0x280, 0x2EE, 0x2F8, 0x5B4 |
| 0x3E9C | `m3C24` | `mBodyExplodeNo` | end 020FEA30: cleared, + 1 every 4th frame up to 20; the segment of mBodyPos that bursts; demo_camera 02101F30 eases its centre to mBodyPos[it] |
| 0x3EA0 | `m3C28` | `mDemoCamEye` | demo_camera 02101F30: copied from the camera's mLookat.mEye, eased toward the next eye; the eye passed to bwdCameraSet is built from it |
| 0x3EAC | `m3C34` | `mDemoCamCenter` | demo_camera 02101F30: copied from the camera's mLookat.mCenter, eased toward the next centre; the centre passed to bwdCameraSet is built from it |
| 0x3EBC | `m3C44` | `mDemoCamSpeed` | demo_camera 02101F30 clears it at the demo starts; DEMO_DEATH_SEGMENT eases it to 1 and moves mDemoCamCenter by up to 200 * it per frame; DEMO_WARP eases it to 40 and back to 0 and turns current.angle.y (which places mDemoCamEye) by it; end 020FEA30 clears it |
| 0x3EC0 | `m3C48` | `mDemoCamFovy` | demo_camera 02101F30: 45/55 (eased), passed to bwdCameraSet as the fovy |
| 0x3EC4 | `m3C4C` | `mDemoCamShake` | fly_pos_move 020F9F80 sets 50, demo_camera 02101F30 5 / 1.5 / 2 (+ reg); demo_camera shakes the camera by it and eases it to 0 |
| 0x3EC8 | `m3C50` | `mCamTypeState` | daBwd_Create 020FE0B4 sets 1, fly (in move 020FF80C) 2, demo_camera 02101F30 0; bwd_execute 020FA614 forces camera type "Boss03" at 2 (then 1) and "WindBoss" at 1 |
| 0x3EC9 | `m3C51` | `mHioOwner` | daBwd_Create 020FE0B4 sets 1 when it created the l_HIO child; daBwd_Delete 020FD578 deletes the HIO child only then |
| 0x1E1C | `m1BA4` | `mTongueSegStep` | control1 (in bwd_execute 020FA614): the forward length of the step added to every tongue segment along the body's angle (with the previous segment's offset); never written in WWHD or tww (always 0) |

Open (no name yet): `m02BC` (param byte, only stored), `m02CC` / `m0324` (models created by useHeapInit, not
used), `m03C4b`, `m18DC` (only stored), `m3B3C` / `m3B40` / `m3B44`
(only stored) and the padding. The tongue terms `mTongueTipPos` .. `mTongueRetract` were named from sita_move,
control1 / control2 and damage_check (in bwd_execute 020FA614) and sita_hit (in move 020FF80C); `mWobblePhaseSin` /
`mWobblePhaseCos` name the phases (as bgn move_s mPhaseSin / mPhaseCos), not what fly_pos_move does with the terms;
`mDemoCamSpeed` covers both demo_camera uses (the step scale of the camera chase, as bmd_class / btd_class
mDemoCamSpeed, and the turn speed of the orbit that places mDemoCamEye). (`struct Timers` in d_a_bwd_execute.h is a
separate view of the five-halfword timer sweep; its `mAction` was corrected to `mTimers` (misnamed); `EffectTimers::mEffect` in the same header is the ten-word view of
`mSmokeTimer` .. `mSmokeTimer6`.)

### daBwd_HIO_c (l_HIO; d_a_bwd.h)

Round 3: Molgera's HIO values. In WWHD the vtable sits at the end, so each field is 4 below its GameCube name. Each is set only by daBwd_HIO_c_ct 020FE7BC (default in parentheses) and never written elsewhere in WWHD or tww; the reads are in our d_a_bwd*.cpp (wait, reset, sita_hit, eat_attack, fly and s_fly are inlined into move 020FF80C; sita_move, control1 and control2 into bwd_execute 020FA614) and match zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00001 | `m05` | `mNoMove` | bwd_execute 020FA614: move and the head animation run only while it is 0 (tww daBwd_Execute the same) (0) |
| 0x00002 | `m06` | `mDebugLastHit` | sita_hit (in move 020FF80C): a tongue hit becomes the last hit (end action 12) when it is set, as at 4 tongue damage (tww the same) (0) |
| 0x00003 | `m07` | `mDebugFlyAfterHit` | sita_hit (in move 020FF80C): after the tongue hit the action becomes fly (10) when it is set, as at 2 tongue damage (tww the same) (0) |
| 0x00008 | `m0C` | `mWaitHeight` | wait (in move 020FF80C): current.pos.y eases to mBaseHeight + it (tww the same) (-400) |
| 0x0000C | `m10` | `mPushPower` | bwd_execute 020FA614: the setOutPower strength (scaled down with the distance) that pushes the player away from mSandDentCenter (tww daBwd_Execute the same) (6) |
| 0x00010 | `m14` | `mTonguePulledTime` | sita_move (in bwd_execute 020FA614): mTonguePulledTimer when the tongue is pulled (tww the same) (120) |
| 0x00014 | `m18` | `mEatAttackDistance` | wait (in move 020FF80C): the action becomes eat_attack (5) when the player's XZ distance (mPlayerDistXZ) is below it (tww the same) (700) |
| 0x00018 | `m1C` | `mResetDistance` | wait (in move 020FF80C): the action becomes reset (2) when mPlayerDistXZ is above it (tww the same) (3000) |
| 0x0001C | `m20` | `mBodyScale` | bwd_execute 020FA614: the head model's scale and the factor on the body spheres' radii, except while flying / ending (tww daBwd_Execute the same) (1.3) |
| 0x00020 | `m24` | `mHitLarvaCount` | sita_hit (in move 020FF80C): a larva (BWDS) is created on each of frames 0x30.. 0x30 + it - 1 of the dive while ko_count is below mLarvaMax (tww the same) (3) |
| 0x00022 | `m26` | `mLarvaMax` | ko_s_sub 020F9A28 / bwd_execute 020FA614 / sita_hit (in move 020FF80C): the number of larvae (BWDS) gathered into ko_ac and the cap on creating more (tww the same) (5) |
| 0x00024 | `m28` | `mBaseHeight` | start, wait, reset, sita_hit, eat_attack, fly, s_fly (in move 020FF80C), bwd_execute 020FA614, demo_camera 02101F30, end 020FEA30: the base height the boss, camera, larvae and platform positions are offset from (tww the same) (800) |
| 0x00028 | `m2C` | `mPlatformHeight` | bwd_execute 020FA614: the Triforce platform model's y offset from mBaseHeight (tww daBwd_Execute the same) (-500) |
| 0x0002C | `m30` | `mSplashScaleXZ` | bwd_execute 020FA614: the sand splash (Gsp) model's x and z scale (tww daBwd_Execute the same) (3.5) |
| 0x00030 | `m34` | `mSplashScaleY` | bwd_execute 020FA614: the sand splash (Gsp) model's y scale (tww daBwd_Execute the same) (2) |

Open (no name yet): `m08`, `m38`, `m3A`, `m3C`, `m3E` (set by the constructor only, read nowhere in WWHD or tww); padding `_05`, `_12`.

### sita_s (the tongue segments mTongueSegments[32]; d_a_bwd.h)

Round 3.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00000 | `m00` | `mpModel` | useHeapInit 020FD6F8 creates the segment's model (BERO, the tip BERO_SAKI) into it; daBwd_Draw 020F9C1C draws it and sita_move (in bwd_execute 020FA614) sets its base matrix (tww the same) |
| 0x00004 | `m04` | `mPos` | the tongue segment's position: daBwd_Create 020FE0B4 sets it, control1 / control2 (in bwd_execute 020FA614) chain each segment to its neighbour, sita_move places the model, the eye position and the tongue spheres on it (tww the same) |

Open (no name yet): `m10` (u8[4], used nowhere).

## btd_class (Gohma; d_a_btd.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0408 | `m02E0` | `mBattlePhase` | get_anm 020F032C / get_btk / get_brk pick the phase-2 morf and animations when it is set; btdActionDamage (in move 020F6004) sets 1, end 020F52B8 sets 2; btd_effect 020F90C4 hides the fire chips at >= 2 |
| 0x040A | `m02E2` | `mFrameCounter` | daBtd_Execute 020F1494 adds 1 per frame; btdExecCamera (in daBtd_Execute 020F1494) uses it as the cos phase of the camera bank shake |
| 0x040C | `m02E4` | `mAction` | move 020F6004: switch (it) selects the action (BtdAction ACT_WAIT .. ACT_END); daBtd_Create 020F4308 sets ACT_START_DEMO, damage_check 020F4D80 sets 2 |
| 0x0411 | `m02E9` | `mAttackAnmIdx` | btdActionWait (in move 020F6004) sets 0 / 5; btdActionJab / btdActionPunch index jab_bck / punch_bck with it, btdActionNextAnimation adds 1 |
| 0x0412 | `m02EA` | `mAttackHand` | btdActionWait (in move 020F6004) sets 0 / 1; jab joint 10 : 8, hahen_set(side = it), btd_effect 020F90C4 picks the right-claw effects (effR_name) when it is set |
| 0x0414 | `m02EC` | `mTimers` | daBtd_Execute 020F1494 counts [0], [1] down with m02F0 and mStunTimer; btdActionWait (in move 020F6004) loads them (150, 300, 40) |
| 0x0418 | `m02F0` | `mBlinkTimer` | daBtd_Execute 020F1494 counts it down; at 0 it reloads cM_rndF + 100 and sets mEyeCloseTimer to 20; startdemo 020F4924 sets 30 |
| 0x041E | `m02F6` | `mRockHitState` | iwa_move (in daDr2_Execute 0212B3D0) sets 1 when the falling rock lands on the btd (then waits until it is 0 again); damage_check 020F4D80 turns 1 into 2 and starts the damage action; btdActionDamage (in move 020F6004) clears it |
| 0x0420 | `m02F8` | `mEyeCloseTimer` | daBtd_Execute 020F1494 sets 20 (blink, a sword cut or a weapon near the eye) and counts it down; while it is set the eyelid angle eases to 13000 |
| 0x0422 | `m02FA` | `mEyelidAngle` | daBtd_Execute 020F1494 eases it to 13000 / 0 (cLib_addCalcAngleS2); nodeCallBack 020F04EC rotates the eyelid joints by it (XrotM) |
| 0x0424 | `m02FC` | `mJointPos` | nodeCallBack 020F04EC: MtxPosition of each joint_check role; daBtd_Execute 020F1494 sets mAtSphs[i] there, hahen_set2 020F0C68 / btdActionJab use the hand points [8] / [10] |
| 0x0508 | `m03E0` | `mEyeJointPos` | nodeCallBack 020F04EC: MtxPosition of the eye joint (copied to eyePos); daBtd_Execute 020F1494 centres mEyeSph there; damage_check 020F4D80 sets the hit particles there |
| 0x5FA4 | `m5E7C` | `mHandCylMode` | btdActionPunch (in move 020F6004) sets mAttackHand + 2, btdActionFire 1; daBtd_Execute 020F1494 places mHandCyls (1: all, 2: [0..2], 3: [3..5]) and clears it |
| 0x5FA8 | `m5E80` | `mFireSize` | btdActionFire (in move 020F6004) grows it by 0.1 from 0; btdActionFireCollision: mFireSph radius = 200 * it |
| 0x5FAC | `m5E84` | `mAtMode` | btdActionJab (in move 020F6004) sets 1, btdActionPunch 2, btdActionFire 3, move clears it; daBtd_Execute 020F1494 turns the At of the hand spheres (1/2: Atp 1/2) or the mouth spheres (3) on by it |
| 0x5FAE | `m5E86` | `mLookMode` | btdActionWait / btdActionFire / btdActionHighFire (in move 020F6004) set 1..3; btdActionTrack turns the neck and head toward the player only while it is set, then clears it |
| 0x5FB0 | `m5E88` | `mNeckAngleY` | btdActionTrack (in move 020F6004) eases it toward the player angle (cLib_addCalcAngleS2); nodeCallBack 020F04EC: YrotM / ZrotM of the neck joint by it |
| 0x5FB2 | `m5E8A` | `mNeckAngleX` | btdActionTrack (in move 020F6004) eases it to mNeckTargetAngleX; nodeCallBack 020F04EC: XrotM of the neck joint by it |
| 0x5FB4 | `m5E8C` | `mNeckTargetAngleX` | btdActionHighFire (in move 020F6004) sets -pitch - 2500; btdActionTrack eases mNeckAngleX to it, then clears it |
| 0x5FB6 | `m5E8E` | `mHeadAngleY` | btdActionTrack (in move 020F6004) eases it to half the eye-to-player angle; nodeCallBack 020F04EC: YrotM of the head (eye) joint by it |
| 0x5FB8 | `m5E90` | `mTurnTargetAngleY` | btdActionWait (in move 020F6004) sets facing + 0x4000; btdActionTrack turns current.angle.y to it (player angle when 0), then clears it |
| 0x5FBA | `m5E92` | `mTurnScale` | btdActionTurningPunch (in move 020F6004) sets 15 (+REG); btdActionTrack: scale argument of the cLib_addCalcAngleS2 turn, reset to 10 |
| 0x5FBC | `m5E94` | `mTurnMaxStep` | btdActionWait / btdActionHighFire / btdActionTurningPunch (in move 020F6004) set 0x100..0xC00; btdActionTrack turns only while it is set (max step of the turn), then clears it |
| 0x5FC0 | `m5E98` | `mHitSePos` | damage_check 020F4D80: [i] = mJointPos[i] and def_se_set_p at [i] / [i + 19]; daBtd_Execute 020F1494: [19 + i] = centre of mHandCyls[i]; daBtd_Delete 020F34A8 deletes their sound objects |
| 0x60EC | `m5FC4` | `mSphHitTimer` | damage_check 020F4D80: per mAtSphs[i] hit, 10 frames before the next hit sound, counted down |
| 0x6112 | `m5FEA` | `mCylHitTimer` | damage_check 020F4D80: per mHandCyls[i] hit, 10 frames before the next hit sound, counted down |
| 0x6120 | `m5FF8` | `mFireChipEmitter` | btd_effect 020F90C4: eff_name emitters on the eff_index joints (TWW eff_name: ID_AK_SN_ARMFIRECHIP / BACKBONEFIRECHIP), moved to -10000 at mBattlePhase >= 2 |
| 0x6134 | `m600C` | `mAttackEmitter` | btd_effect 020F90C4: effF_name / effR_name emitters (fire breath, claw rock and smoke) while mAttackEmitterTimer[i] runs; btdActionFire (in move 020F6004) clears [3] |
| 0x6150 | `m6028` | `mAttackEmitterTimer` | btdActionPunch / btdActionFire / btdActionHighFire (in move 020F6004) load it; btd_effect 020F90C4 counts [i] down and keeps mAttackEmitter[i] while it is set |
| 0x6160 | `m6038` | `mpDeadHeadEmitter` | end 020F52B8: particle_set(0x80DE) at mDeadHeadPos (TWW: ID_AK_SN_BTDBREAKDOWNHEAD00), moved with the head, made invalid when the head lands |
| 0x6164 | `m603C` | `mHahenPos` | hahen_set 020F0EB4: hand point - 100 in y; position of the debris (0x80A8 / 0x80A9) particles and smoke_set_s |
| 0x6170 | `m6048` | `mHahenAngle` | hahen_set 020F0EB4: current.angle +- 0x4000 in y by side; angle of the debris particles and smoke_set_s |
| 0x61FC | `m60D0` | `mKankyoBlend` | btd_kankyo 020F94DC: eased per mKankyoState (cLib_addCalc2 / addCalc0), passed to dKy_custom_colset as the blend |
| 0x6200 | `m60D4` | `mLightRate` | btd_kankyo 020F94DC: eased per mKankyoState; scales mLightInfluence colour and power |
| 0x6204 | `m60D8` | `mLightColorR` | btd_kankyo 020F94DC: l_HIO.m3C / m42; red of mLightInfluence (times mLightRate) |
| 0x6205 | `m60D9` | `mLightColorG` | btd_kankyo 020F94DC: l_HIO.m3E / m44; green of mLightInfluence (times mLightRate) |
| 0x6206 | `m60DA` | `mLightColorB` | btd_kankyo 020F94DC: l_HIO.m40 / m46; blue of mLightInfluence (times mLightRate) |
| 0x63D4 | `m6190` | `mCrackStage` | btdActionBreakShell (in move 020F6004): index into hibi_brk (crack animations), +1 per hit up to 3; btdActionDamage tests >= 3 |
| 0x7058 | `m6E14` | `mSplashPopCount` | sibuki_set 020F10B4 clears it; btdExecSplashMove (in daBtd_Execute 020F1494) adds 1 per splash pop near the player, up to 20 |
| 0x7059 | `m6E15` | `mBlurRate` | daBtd_Draw 020F0A74: mDoGph_setBlureRate(it) while > 1, off and cleared at 1; end 020F52B8 sets 180, daBtd_Execute 020F1494 50 |
| 0x705A | `m6E16` | `mDemoCamMode` | btdExecCamera (in daBtd_Execute 020F1494): switch (it) of the demo camera steps, +1 per step; 0 = no demo camera |
| 0x705C | `m6E18` | `mDemoMainCount` | btdExecCamera (in daBtd_Execute 020F1494): +1 per frame, reported as "K MAIN COUNT  %d"; the player demo modes start at its frame marks |
| 0x705E | `m6E1A` | `mDemoSubCount` | btdExecCamera (in daBtd_Execute 020F1494): +1 per frame, cleared per step, reported as "K SUB  COUNT  %d"; compared with each step's frame marks |
| 0x7060 | `m6E1C` | `mDemoCamEye` | btdExecCamera (in daBtd_Execute 020F1494): set / eased per step; the eye passed to dCamera_c::Set (with shake) and Reset |
| 0x706C | `m6E28` | `mDemoCamCenter` | btdExecCamera (in daBtd_Execute 020F1494): set / eased per step; the centre passed to dCamera_c::Set (with shake) and Reset |
| 0x7078 | `m6E34` | `mDemoCamOrbitAngleX` | btdExecCamera (in daBtd_Execute 020F1494): -11000, eased; XrotM before mDemoCamEye = MtxPosition(0, 0, mDemoCamOrbitDist) |
| 0x707A | `m6E36` | `mDemoCamOrbitAngleY` | btdExecCamera (in daBtd_Execute 020F1494): -6000, eased to 0x4000; YrotS before mDemoCamEye = MtxPosition(0, 0, mDemoCamOrbitDist) |
| 0x7080 | `m6E3C` | `mDemoCamFovy` | btdExecCamera (in daBtd_Execute 020F1494): 50 (+REG) / 55 / 60, eased; passed to dCamera_c::Set as the fovy |
| 0x7084 | `m6E40` | `mDemoCamSpeed` | btdExecCamera (in daBtd_Execute 020F1494): cleared per step and eased up; scales the cLib_addCalc2 steps of mDemoCamEye / mDemoCamCenter |
| 0x7088 | `m6E44` | `mDemoCamOrbitDist` | btdExecCamera (in daBtd_Execute 020F1494): 2800, eased to 500; z of the orbit vector that gives mDemoCamEye |
| 0x708C | `m6E48` | `mDemoCamShake` | btdExecCamera (in daBtd_Execute 020F1494): sin / cos amplitude added to mDemoCamEye / mDemoCamCenter and the bank, decays (addCalc0); startdemo 020F4924 sets 40, end 020F52B8 30 |
| 0x7090 | `m6E4C` | `mDeadHeadOn` | end 020F52B8 sets 1 when the head comes off; daBtd_Draw 020F0A74 then draws the dead-head model at mDeadHeadPos, nodeCallBack 020F04EC stops moving eyePos |
| 0x7094 | `m6E50` | `mDeadHeadPos` | end 020F52B8: starts at eyePos, moves by mDeadHeadSpeed toward mDeadHeadTargetPos; kubi_calc 020F1288 places the dead-head model there; daBtd_Draw 020F0A74 settingTevStruct at it |
| 0x70A0 | `m6E5C` | `mDeadHeadTargetPos` | btdExecCamera (in daBtd_Execute 020F1494): MtxPosition of the landing point; end 020F52B8 moves mDeadHeadPos toward it and lands at its y |
| 0x70AC | `m6E68` | `mDeadHeadSpeed` | end 020F52B8: MtxPosition of the throw vector, added to mDeadHeadPos per frame, y minus gravity, bounced |
| 0x70B8 | `m6E74` | `mDeadHeadAngle` | end 020F52B8: starts at current.angle, spins / eases; kubi_calc 020F1288: Y/X/ZrotM of the dead-head model by it |
| 0x70C0 | `m6E7C` | `mWaveRadius` | wave_set 020F02D4 clears it, daBtd_Execute 020F1494 adds 30 (+REG) up to 2300; dMagma_ballBoss_c::calc 024F60E8 raises the balls at about this distance from the centre |
| 0x70C4 | `m6E80` | `mMagmaFloorY` | daBtd_Create 020F4308 sets -50; daBtd_Execute 020F1494 passes it to the magma floor (setYpos) |
| 0x70C8 | `m6E84` | `mMagmaBallOffsetY` | daBtd_Create 020F4308 sets 0 / -30, end 020F52B8 -200, btdExecCamera eases it to 0; dMagma_ballBoss_c::setup 024F637C adds it to the ball base height, calc 024F60E8 sinks the balls below -150 |
| 0x70CC | `m6E88` | `mMagmaRiseCenter` | daBtd_Execute 020F1494 sets 1, the actions (in move 020F6004) clear it; dMagma_ballBoss_c::setup 024F637C raises the balls near the centre only when it is 1 |
| 0x70D4 | `m6E90` | `mForceBossCamera` | daBtd_Create 020F4308 sets 1, btdExecCamera clears it; daBtd_Execute 020F1494 calls SetTypeForce("Boss01") while it is set |

Open (no name yet): `m02E1`, `m5E96` (no uses), and the padding bytes `m02EB`, `m02F7`, `m5E7D`, `m5E87`, `m6036`,
`m604E`, `m60DE`, `m6191`, `m6E38`, `m6E4D`, `m6E7A`, `m6E89`, `m6E91`. The private views of btd_class in
d_a_btd_damage.h, d_a_dr2.h (`Dr2Btd`), d_a_bita.h (`BitaBossHD`), d_boss_magma.h, d_a_kui.h (`KuiBtdView`) and
d_a_himo2.h (`himo2_btd_c`) use the same names. The zeldaret/tww d_a_btd.cpp (CC0) was read alongside our decomp to
follow the uses; it has no names for these fields, and the "K MAIN COUNT" / "K SUB  COUNT" report strings are the
original's.

The view `BtdDragonHD` (d_a_btd.h) of dr2_class takes dr2_class's names (d_a_dr2.h): `m43C` -> `mFloorBroken` (end 020F52B8 sets 1 when the demo timer reaches 2), `m4A8` -> `mRockPos` (daBtd_Delete 020F34A8 deletes the sound object at it), `m4BA` -> `mRockState` (btdActionBurst (in move 020F6004) sets 10; btdActionTrack turns Gohma only while it is 0 or >= 10), `m4CA` -> `mRockFallTimer` (btdActionTrack: only while it is 0 or >= 98).

### daBtd_HIO_c (l_HIO; d_a_btd.h; view BtdHioDamageHD in d_a_btd_damage.h)

Round 3: Gohma's HIO values. In WWHD the vtable sits at the end, so each field is 4 below its GameCube name. Each is set only by daBtd_HIO_ct 020F4744 (default in parentheses) and never written elsewhere in WWHD or tww; the reads are in our d_a_btd.cpp / d_a_btd_damage.cpp (the btdAction* and btdExec* helpers are inlined into daBtd_Execute 020F1494) and match zeldaret/tww d217e2b. The view BtdHioDamageHD takes the class's names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x01 | `m05` | `mNoAnmPlay` | daBtd_Execute 020F1494: the body morf plays only while it is 0 (tww the same) (0) |
| 0x02 | `m06` | `mDebugBreak` | btdActionBreakShell (inlined into daBtd_Execute 020F1494): when set the crack stage goes straight to 3; damage_check 020F4D80 (d_a_btd_damage.cpp): when set any hit counts as the last one (tww the same) (0) |
| 0x03 | `m07` | `mDebugNoBreak` | btdActionBreakShell (inlined into daBtd_Execute 020F1494): when set a crack stage of 3 or more is held at 2 (tww damage the same) (0) |
| 0x04 | `m08` | `mScale` | daBtd_Execute 020F1494: scale (x, y, z) and the model base scale (tww the same) (1.0) |
| 0x08 | `m0C` | `mStunTime` | btdActionDamage (inlined into daBtd_Execute 020F1494): the stun timer of the damage action (tww damage mStunTimer) (150) |
| 0x0A | `m0E` | `mDamageLimit` | damage_check 020F4D80 (d_a_btd_damage.cpp): in action 2, once mTotalDamage reaches it, it is cleared and mGohmaState becomes 10 (tww the same) (5) |
| 0x0C | `m10` | `mDebugAttackType` | btdActionWait / btdActionTurningPunch (inlined into daBtd_Execute 020F1494): 0 picks the attack at random, 1..3 force attack type it - 1, 4 the turning punch (tww the same) (0) |
| 0x10 | `m14` | `mWaitTime` | btdActionReset / btdActionPunch (inlined into daBtd_Execute 020F1494): ACT_WAIT's mTimers[0] is cM_rndF(50) + it (tww the same) (100) |
| 0x14 | `m18` | `mAttack0Rate` | btdActionWait (inlined into daBtd_Execute 020F1494): in battle phase 0 attack type 0 is chosen when cM_rndF(1) < it (tww wait the same) (0.5) |
| 0x18 | `m1C` | `mAttack1Rate` | btdActionWait (inlined into daBtd_Execute 020F1494): in battle phase 0 otherwise attack type 1 when cM_rndF(1) < it, else 2 (tww wait the same) (0.5) |
| 0x1C | `m20` | `mAttack0RatePhase1` | btdActionWait (inlined into daBtd_Execute 020F1494): mAttack0Rate in battle phase 1 (tww wait the same) (0.2) |
| 0x20 | `m24` | `mAttack1RatePhase1` | btdActionWait (inlined into daBtd_Execute 020F1494): mAttack1Rate in battle phase 1 (tww wait the same) (0.2) |
| 0x24 | `m28` | `mPunchWaitTime` | btdActionPunch (inlined into daBtd_Execute 020F1494): mTimers[0] after the punch is cM_rndF(100) + it (tww punch_attack the same) (150) |
| 0x28 | `m2C` | `mFireTime` | btdActionFire / btdActionHighFire (inlined into daBtd_Execute 020F1494): the fire timers (it * 9, it * 4, it) (tww fire_attack the same) (10) |
| 0x2C | `m30` | `mCustomTime` | btd_kankyo 020F94DC / daBtd_Create 020F4308: dKy_custom_timeset(it) every frame (tww the same) (4.0) |
| 0x30 | `m34` | `mCustomTimeState2` | btd_kankyo 020F94DC: dKy_custom_timeset(it) in kankyo state 2 (tww the same) (20.0) |
| 0x34 | `m38` | `mCustomTimeState7` | btd_kankyo 020F94DC: dKy_custom_timeset(it) in kankyo state 7 (tww the same) (6.0) |
| 0x38 | `m3C` | `mLightRState1` | btd_kankyo 020F94DC: mLightColorR in kankyo state 1 (tww the same) (182) |
| 0x3A | `m3E` | `mLightGState1` | btd_kankyo 020F94DC: mLightColorG in kankyo state 1 (tww the same) (156) |
| 0x3C | `m40` | `mLightBState1` | btd_kankyo 020F94DC: mLightColorB in kankyo state 1 (tww the same) (86) |
| 0x3E | `m42` | `mLightRState2` | btd_kankyo 020F94DC: mLightColorR in kankyo state 2 (tww the same) (255) |
| 0x40 | `m44` | `mLightGState2` | btd_kankyo 020F94DC: mLightColorG in kankyo state 2 (tww the same) (142) |
| 0x42 | `m46` | `mLightBState2` | btd_kankyo 020F94DC: mLightColorB in kankyo state 2 (tww the same) (36) |
| 0x44 | `m48` | `mSplashSpeedF` | sibuki_set 020F10B4: the splash's forward speed it + cM_rndF(it / 2) (tww the same) (12.0) |
| 0x48 | `m4C` | `mSplashSpeedY` | sibuki_set 020F10B4: the splash's upward speed it + cM_rndF(it / 2) (tww the same) (35.0) |
| 0x4C | `m50` | `mSplashGravity` | btdExecSplashMove (inlined into daBtd_Execute 020F1494): subtracted from the splash's speed.y each frame (tww sibuki_move the same) (1.0) |
| 0x50 | `m54` | `mSplashFollow` | btdExecSplashMove (inlined into daBtd_Execute 020F1494): when set the first splash follows the player (tww sibuki_move the same) (0) |

Open (no name yet): `m11`, `m12`, `m13`, `m2E` (written and read nowhere in WWHD or tww); padding `_pad51`.

## bgn_class (Puppet Ganon, first form; d_a_bgn.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3C8 | `m3C8` | `mSelf` | bgn_class_ct 0208064C stores gabi::ea(this) (HD); daBgn_Draw 0207B2CC sets mHdPacket[i].mA4 from it + 0x110 |
| 0x3D4 | `m02B4` | `mParam` | daBgn_Create 02080BA4: (u8)fopAcM_GetParam; 0xFF makes daBgn_Execute 0207E7B4 return at once and daBgn_Create skip the setup |
| 0x3D5 | `m02B5` | `mForm` | daBgn_Draw 0207B2CC draws the bgn (0), bgn2 (1) or bgn3 (2) form by it; hensin 02084D6C sets 1, daBgn_Execute 0207E7B4 2 / 3; move 0208857C acts only at 0 |
| 0x3E0 | `m02C0` | `mChestPacket` | daBgn_Draw 0207B2CC: model packet of mpChestModel (setModel + update); useHeapInit 0207FE14 sets it up |
| 0x4B4 | `m02F8` | `mFogColorAdd` | daBgn_Draw 0207B2CC (bgn_fog_flash): eased to 0x118 / 0, added to the fog colour of tevStr |
| 0x4B8 | `m02FC` | `mFogStartAdd` | daBgn_Draw 0207B2CC (bgn_fog_flash): eased to -50000 / 0, added to mFogStartZ of tevStr |
| 0x4BE | `m0302` | `mCoreHitFrame` | move 0208857C (damage_check) sets 1 on a core hit; daBgn_Execute 0207E7B4 counts it up to 100, then clears it; at 0xD move starts the arrow hit flash; part_mtx_set 0207DE64 compares it with part mPartCoreHitFrame |
| 0x4C0 | `m0304` | `mRippleSeTimer` | action_s_himo 020859AC: cM_rndF(20) + 20 frames until the next ripple sound; daBgn_Execute 0207E7B4 counts it down |
| 0x4C4 | `m0308` | `mRipplePos` | action_s_himo 020859AC / move 0208857C: position of the JA_SE_CM_BGN_STRING_RIPPLE / BODY_RIPPLE sound; daBgn_Delete 0207FC64 deletes its sound object |
| 0x12C20 | `mAAA8` | `mMove` | the 8 move_s (strings and their limb pose: mCut cut, mPoseTarget pose target); part_control_0 0207D858 / part_mtx_set 0207DE64 / start 020844F4 / action_main 02086130 per string |
| 0x14480 | `mC308` | `mBodyPos` | move 0208857C places the chest model there (+ l_HIO.m0A8, at least mGroundY + 100); hensin 02084D6C hands it to the bgn2 form as current.pos; body_attack (in action_main 02086130) reads it |
| 0x1448C | `mC314` | `mBodyAngle` | move 0208857C rotates the chest model by it (y = mBaseAngleY + mDanceAngleY); hensin 02084D6C hands it to the bgn2 form as current.angle / shape_angle |
| 0x14492 | `mC31A` | `mBaseAngleY` | move 0208857C eases it (cLib_addCalcAngleS2) and sets mBodyAngle.y = it + mDanceAngleY |
| 0x1449C | `mC324` | `mArmLength` | move 0208857C eases [i] toward l_HIO.m124 + stretch and passes it (* l_HIO.m0F4) to part_control_0 for the arms; part_mtx_set 0207DE64 scales the arm parts by it |
| 0x144A4 | `mC32C` | `mArmStretch` | punch_LR (in action_main 02086130) eases [i] to 1, move 0208857C back to 0; size_set (in move) pulls the arm part lengths toward the middle by it |
| 0x144AC | `mC334` | `mPoseSpeed` | action_s 02085630: step of cLib_addCalc2 of each move_s pose (mPose toward mPoseTarget); action_main 02086130 eases it to mPoseSpeedTarget |
| 0x144B0 | `mC338` | `mPoseSpeedTarget` | punch_LR (in action_main 02086130) sets 50 / 130 / 200, action_main l_HIO.m0D4; mPoseSpeed is eased to it |
| 0x144B4 | `mC33C` | `mHimoTopPos` | part_mtx_set 0207DE64: MtxPosition of each string's attach point; action_s_himo 020859AC starts string [i] from it |
| 0x149AC | `mC450` | `mLastHimoPos` | last_himo_control (in daBgn_Execute 0207E7B4): the 60 points of the last string ([0] fixed at (-15, 4441.46, 375.17)); drawn while mLastHimoState is set |
| 0x14C7C | `mC720` | `mLastHimoState` | daBgn_Execute 0207E7B4 sets 1 at the final explosion; last_himo_control turns 1 into 2 (start); daBgn_Draw 0207B2CC draws the last string while it is set |
| 0x14C80 | `mC724` | `mLastHimoSway` | last_himo_control (in daBgn_Execute 0207E7B4): set 5000 (+REG) on start, decays (addCalc0); sin / cos amplitude of the string points |
| 0x14E94 | `mC728` | `mBodyOffset` | tail_attack 0208459C / damage 02084804 / head_recover 02084B90 / dance_0: height offset of the body (y eased, + speed.y, clamped); move_se_set 0207D04C sounds by its change |
| 0x14EA0 | `mC734` | `mBodyOffsetOld` | move 0208857C copies mBodyOffset; move_se_set 0207D04C: mBodyOffset - it (last frame's offset) |
| 0x14EB2 | `mC746` | `mFrameCounter` | daBgn_Execute 0207E7B4 adds 1 per frame; sin / cos phase of the dance, the body shake, the ripples and the camera bank |
| 0x14EB4 | `mC748` | `mAction` | action_main 02086130: switch (it) of the action (0 .. 7, 10); move 0208857C sets 5 on a core hit, daBgn_Create 02080BA4 7, dance_0 1 .. 4 |
| 0x14EB6 | `mC74A` | `mMode` | tail_attack 0208459C / hensin 02084D6C / dance_0: switch (it) inside each action, +1 per step; damage 02084804 sets 2 |
| 0x14EB8 | `mC74C` | `mDanceType` | dance_0 (in action_main 02086130) sets 0 / 1 and runs dance_A (0) or dance_B (1); start 020844F4 clears it |
| 0x14EBA | `mC74E` | `mDanceStep` | dance_A (in action_main 02086130): switch (it) of the dance_pause_1 / dance_pause_2 poses, +1 per step |
| 0x14EBC | `mC750` | `mDanceAngleY` | dance_A adds mDanceAngleSpeed, dance_B eases it to +-0x1555 (in action_main 02086130); move 0208857C: mBodyAngle.y = mBaseAngleY + it; hensin 02084D6C clears it |
| 0x14EBE | `mC752` | `mDanceAngleSpeed` | dance_A (in action_main 02086130): cleared on start, added to mDanceAngleY per frame, eased toward the turn target |
| 0x14EC0 | `mC754` | `mDanceCount` | start 020844F4 sets 1; dance_0 (in action_main 02086130) adds 1 per dance, wraps after 5, and picks the attack by it |
| 0x14EC4 | `mC758` | `mTargetPos` | dance_0 (in action_main 02086130) sets a random x / z (cM_rndFX(1500)), tail_attack 0208459C the player position; current.pos is eased toward it by mMoveSpeed |
| 0x14ED8 | `mC76C` | `mMoveSpeed` | tail_attack 0208459C / dance_0 (in action_main 02086130): step of cLib_addCalc2 of current.pos toward mTargetPos, eased up from 0 |
| 0x14EDC | `mC770` | `mShakeTimer` | damage 02084804 sets 0x32, hensin 02084D6C 0x14 (+REG); move 0208857C counts it down and shakes the chest (X / Z rotation) by it * 300 |
| 0x14EE4 | `mC778` | `mLeftPunchOn` | punch_LR (in action_main 02086130) sets 1 while the left punch flies, 0 after; move 0208857C moves the left arm (i == 0) to mLeftPunchPos while it is set |
| 0x14EE5 | `mC779` | `mRightPunchOn` | punch_LR (in action_main 02086130) sets 1 while the right punch flies, 0 after; move 0208857C moves the right arm (i == 1) to mRightPunchPos while it is set |
| 0x14EE8 | `mC77C` | `mLeftPunchPos` | punch_LR (in action_main 02086130): copied from the left hand part (mPartPos), + mLeftPunchSpeed per frame; daBgn_Delete 0207FC64 deletes its sound object |
| 0x14EF4 | `mC788` | `mRightPunchPos` | punch_LR (in action_main 02086130): copied from the right hand part (mPartPos), + mRightPunchSpeed per frame; daBgn_Delete 0207FC64 deletes its sound object |
| 0x14F00 | `mC794` | `mLeftPunchSpeed` | punch_LR (in action_main 02086130): bgn_punch_aim velocity, added to mLeftPunchPos |
| 0x14F0C | `mC7A0` | `mRightPunchSpeed` | punch_LR (in action_main 02086130): bgn_punch_aim velocity, added to mRightPunchPos |
| 0x14F18 | `mC7AC` | `mTimers` | daBgn_Execute 0207E7B4 counts all 5 down each frame; the actions load them (tail_attack 0208459C [0] = 100, dance_A [3] = l_HIO.m0DA) |
| 0x14F22 | `mC7B6` | `mDamageTimer` | move 0208857C (damage_check) sets 100 on a core hit and takes no core hit while it is set; daBgn_Execute 0207E7B4 counts it down |
| 0x14F24 | `mC7B8` | `mPartHitSeTimer` | move 0208857C (damage_check) sets 10 when a part sphere is hit (def_se_set), no new part hit sound while it is set; daBgn_Execute 0207E7B4 counts it down |
| 0x14F28 | `mC7BC` | `mGroundY` | part_control_0 0207D858 / part_mtx_set 0207DE64 keep the part positions at or above it; punch_LR (in action_main 02086130) tests the hand y against it + 30 |
| 0x14F68 | `mC7FC` | `mBodyAtSph` | daBgn_Create 02080BA4: Set(cc_sph_src); body_attack (in action_main 02086130) turns its At on (OnAtSetBit) |
| 0x151C0 | `mCA54` | `mCorePos` | part_mtx_set 0207DE64: centre of mCoreSph (the weak point); move 0208857C (damage_check) sets the weak point hit / break particles there; daBgn_Delete 0207FC64 deletes its sound object |
| 0x151CC | `mCA60` | `mBlurRate` | daBgn_Draw 0207B2CC: mDoGph_setBlureRate(it) while > 1, off and cleared at 1; bgn_demo_end (in daBgn_Execute 0207E7B4) sets 1, hensin 02084D6C eases it to 0xB4 |
| 0x151E0 | `mCA74` | `mCSCamEyeOffset` | daBgn_Execute 0207E7B4 (demo camera): set to (200, 2100, 2500), eased; rotated by the bgn2 shape angle into mCSCamEye |
| 0x15204 | `mCA98` | `mCalcSpeed` | head_recover 02084B90 / the demo camera (in daBgn_Execute 0207E7B4) scale their cLib_addCalc2 steps by it and ease it up; dance_0 (in action_main 02086130) clears it |
| 0x15208 | `mCA9C` | `mCSCamShake` | hensin 02084D6C sets 20 / 15 / 10, daBgn_Execute 0207E7B4 30 (+REG): sin / cos amplitude of the demo camera eye / centre and bank |
| 0x153E4 | `mCB60` | `mRoomReflectionPacket` | daBgn_Draw 0207B2CC: model packet of mpRoomReflectionModel (setModel + update); useHeapInit 0207FE14 sets it up |
| 0x1565C | `mCC24` | `mObjPacket` | ten_a_d_sub 0207B040: model packet of the searched object's model (setModel + update); useHeapInit 0207FE14 sets it up |
| 0x15754 | `mCC80` | `mPartLengthScale` | action_main 02086130 sets 1, hensin 02084D6C eases it to 0; part_control_0 0207D858 / action_s_himo 020859AC scale the part and string lengths by it |
| 0x15758 | `mCC84` | `mChestSquash` | hensin 02084D6C eases it to 0.22; move 0208857C scales the chest by (1 + it, 1 - it, 1 + it) while it is > 0 |
| 0x1575C | `mCC88` | `mFogFade` | hensin 02084D6C eases it to 1; part_draw 0207ACD0 / daBgn_Draw 0207B2CC move the fog start by -50000 * it |
| 0x15760 | `mCC8C` | `mpChestEmitter` | hensin 02084D6C: particle_set (TWW: ID_AK_SN_KGTT2CHESTPOTA00) on the chest joint, made invalid when done |
| 0x15764 | `mCC90` | `mFormRequest` | daBgn_Execute 0207E7B4: 1 switches to the bgn3 form (mForm = 2), 2 starts the next stage (GanonK); the demo camera / bgn2 set it |
| 0x15765 | `mCC91` | `mHioOwner` | daBgn_Create 02080BA4 sets 1 when it created the l_HIO child; daBgn_Delete 0207FC64 deletes the HIO child only then |

Named in round 2:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x14EB0 | `mC744` | `mHeadCutYaw` | move 0208857C eases it (step 0x400) to +8000 while head string mMove[0] is cut, -8000 while mMove[1] is cut (0 for both or neither); part_mtx_set 0207DE64 adds it to the head part's y rotation (with l_HIO.mHeadRotY); the head hangs to the side of the cut string (tww the same) |
| 0x14EE0 | `mC774` | `mRedRopeSway` | action_main 02086130 swings the 60 points of mRedRopeMat by sin * it * (59 - i) / 59 and eases it to 0; damage 02084804, hensin 02084D6C and body_attack (in action_main) ease it to 250 (tww the same) |

Open (no name yet): `mC764` (two meanings in tail_attack: target angle and spin speed), `mCC38` (no uses), and the
padding bytes. The private views of bgn_class in d_a_bgn2.h
(`bgn2BgnView`), d_a_bgn3.h (`Bgn3BossView`) and d_a_att.h (`bgn_att_view_c`) use the same names. The zeldaret/tww
d_a_bgn.cpp (CC0) was read alongside our decomp to follow the uses; names it already has (`mCSMode`, `mKSubCount`,
`mCSCamEye`, ...) were kept. The keese view `BgnKeeseView` (d_a_bgn_main.h, used by ki_a_d_sub 0207B0D8) takes ki_class's
name `mModelPacket` for 0x3D4 (was `m2B8`).

HIO block daBgn_HIO_c (HD offsets = GameCube - 4; constructor daBgn_HIO_c_ct 02083558, defaults as tww d_a_bgn.cpp
except the DEMO_SELECT values noted):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x000 | `m004` | `mChestRotX` | move 0208857C (shape_calc) rotates the chest model about x by it after the body rotation and scale; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x004 | `m008` | `mPartDrawDist` | part_draw 0207ACD0 draws a part's model only while its camera distance is above it * mPartScale; daBgn_HIO_c_ct 02083558 sets 200 (tww the same) |
| 0x008 | `m00C` | `mReflectionOn` | part_draw 0207ACD0 / daBgn_Draw 0207B2CC update the model packets (mPartPacket, mChestPacket) and run obj_disp only while set; daBgn_HIO_c_ct 02083558 sets 1 (tww the same, also for bgn2 / bgn3) |
| 0x009 | `m00D` | `mRoomReflectionOn` | daBgn_Draw 0207B2CC runs room_disp (mpRoomReflectionModel into mRoomReflectionPacket) only while set; water0_disp (in daBgn_Draw) keeps mWaterAlphaPct then instead of the bgn2-height value; daBgn_HIO_c_ct 02083558 sets 1 (tww the same) |
| 0x00C | `m010` | `mWaterAlphaPct` | water0_disp (in daBgn_Draw 0207B2CC): the water konst alpha is it * 2.559 (percent; 50..90 from the bgn2 height in form 1 while mRoomReflectionOn is 0); daBgn_HIO_c_ct 02083558 sets 50 (tww the same) |
| 0x021 | `m025` | `mPoseEditMode` | while set action_main 02086130 runs no action, action_s 02085630 clears mCut and loads every string's mPoseTarget from mPoseEdit*, and move 0208857C takes mPoseEditAngleY as the body yaw; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x023 | `m027` | `mDebugHealth` | daBgn_Execute 0207E7B4 sets health to it while it is not 0; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x024 | `m028` | `mAttackOn` | dance_0 (in action_main 02086130) leaves the dance for an attack (mTimers[0] 0, no keese left) only while set; daBgn_HIO_c_ct 02083558 sets 1 (tww the same) |
| 0x025 | `m029` | `mForceCutString0` | move 0208857C sets mMove[0].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x026 | `m02A` | `mForceCutString1` | move 0208857C sets mMove[1].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x027 | `m02B` | `mForceCutString2` | move 0208857C sets mMove[2].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x028 | `m02C` | `mForceCutString3` | move 0208857C sets mMove[3].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x029 | `m02D` | `mForceCutString4` | move 0208857C sets mMove[4].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x02A | `m02E` | `mForceCutString5` | move 0208857C sets mMove[5].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x02B | `m02F` | `mForceCutString6` | move 0208857C sets mMove[6].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x02C | `m030` | `mForceCutString7` | move 0208857C sets mMove[7].mCut while set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x030 | `m034` | `mHeadOffsetY` | part_mtx_set 0207DE64 (head part) translates by (0, it, mHeadOffsetZ) before the mHeadRotX rotation; daBgn_HIO_c_ct 02083558 sets -20 (tww the same) |
| 0x034 | `m038` | `mHeadOffsetZ` | part_mtx_set 0207DE64 (head part) translates by (0, mHeadOffsetY, it) before the mHeadRotX rotation; daBgn_HIO_c_ct 02083558 sets -30 (tww the same) |
| 0x038 | `m03C` | `mHeadRotY` | part_mtx_set 0207DE64 (head part) rotates about y by mHeadCutYaw + it + the pluck shake; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x03A | `m03E` | `mHeadRotX` | part_mtx_set 0207DE64 (head part) rotates about x by it after the mHeadOffsetY / Z translation; daBgn_HIO_c_ct 02083558 sets 22000 (tww the same) |
| 0x03C | `m040` | `mHeadRotZ` | part_mtx_set 0207DE64 (head part) rotates about z by it + the pluck shake; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x03E | `m042` | `mPoseEditAngleY` | move 0208857C takes it as the body yaw target (instead of shape_angle.y) while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x048 | `m04C` | `mBodyHeight` | move 0208857C adds it to mBodyPos.y (current.pos + mBodyOffset; at least mGroundY + 100); daBgn_HIO_c_ct 02083558 sets 950 (tww the same) |
| 0x050 | `m054` | `mHeadEndPosX` | x of the cXyz move 0208857C passes to bgn_limb_end for the head string: rotated by the body yaw, + mMove[0].mPose, + mBodyPos gives mHeadParts[1].mPartPos; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x054 | `m058` | `mHeadEndPosY` | y of the head-string end cXyz at mHeadEndPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets 2000 (tww the same) |
| 0x058 | `m05C` | `mHeadEndPosZ` | z of the head-string end cXyz at mHeadEndPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets 800 (tww the same) |
| 0x05C | `m060` | `mPelvisEndPosX` | x of the cXyz move 0208857C passes to bgn_limb_end for the pelvis string (mMove[2]): gives mPelvisParts[1].mPartPos; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x060 | `m064` | `mPelvisEndPosY` | y of the pelvis-string end cXyz at mPelvisEndPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets -800 (tww the same) |
| 0x064 | `m068` | `mPelvisEndPosZ` | z of the pelvis-string end cXyz at mPelvisEndPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets -800 (tww the same) |
| 0x068 | `m06C` | `mLeftHandPosX` | move 0208857C: x of the left hand rest position (rotated by the body yaw, + mMove[3].mPose, + mBodyPos) for mLeftArmParts[BGN_HAND_MAX] while not punching; daBgn_HIO_c_ct 02083558 sets 700 (tww the same) |
| 0x06C | `m070` | `mLeftHandPosY` | move 0208857C: y of the left hand rest position (see mLeftHandPosX); daBgn_HIO_c_ct 02083558 sets -900 (tww the same) |
| 0x070 | `m074` | `mLeftHandPosZ` | move 0208857C: z of the left hand rest position (see mLeftHandPosX); daBgn_HIO_c_ct 02083558 sets 300 (tww the same) |
| 0x074 | `m078` | `mRightHandPosX` | move 0208857C: x of the right hand rest position (rotated by the body yaw, + mMove[4].mPose, + mBodyPos) for mRightArmParts[BGN_HAND_MAX] while not punching; daBgn_HIO_c_ct 02083558 sets -700 (tww the same) |
| 0x078 | `m07C` | `mRightHandPosY` | move 0208857C: y of the right hand rest position (see mRightHandPosX); daBgn_HIO_c_ct 02083558 sets -900 (tww the same) |
| 0x07C | `m080` | `mRightHandPosZ` | move 0208857C: z of the right hand rest position (see mRightHandPosX); daBgn_HIO_c_ct 02083558 sets 300 (tww the same) |
| 0x080 | `m084` | `mLeftFootPosX` | x of the cXyz move 0208857C passes to bgn_limb_end for the left leg string (mMove[5]): gives mLeftLegParts[3].mPartPos; daBgn_HIO_c_ct 02083558 sets 500 (tww the same) |
| 0x084 | `m088` | `mLeftFootPosY` | y of the left-foot cXyz at mLeftFootPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets -1300 (tww the same) |
| 0x088 | `m08C` | `mLeftFootPosZ` | z of the left-foot cXyz at mLeftFootPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets -100 (tww the same) |
| 0x08C | `m090` | `mRightFootPosX` | x of the cXyz move 0208857C passes to bgn_limb_end for the right leg string (mMove[6]): gives mRightLegParts[3].mPartPos; daBgn_HIO_c_ct 02083558 sets -500 (tww the same) |
| 0x090 | `m094` | `mRightFootPosY` | y of the right-foot cXyz at mRightFootPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets -1300 (tww the same) |
| 0x094 | `m098` | `mRightFootPosZ` | z of the right-foot cXyz at mRightFootPosX (bgn_limb_end in move 0208857C); daBgn_HIO_c_ct 02083558 sets -100 (tww the same) |
| 0x098 | `m09C` | `mTailEndPosX` | move 0208857C: x of the tail end position (rotated by the body yaw, + mMove[7].mPose, + mBodyPos, not below mGroundY) for mTailParts[BGN_TAIL_MAX]; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x09C | `m0A0` | `mTailEndPosY` | move 0208857C: y of the tail end position (see mTailEndPosX); daBgn_HIO_c_ct 02083558 sets -700 (tww the same) |
| 0x0A0 | `m0A4` | `mTailEndPosZ` | move 0208857C: z of the tail end position (see mTailEndPosX); daBgn_HIO_c_ct 02083558 sets -1200 (tww the same) |
| 0x0A4 | `m0A8` | `mBodyDrawOffsetY` | move 0208857C (shape_calc) starts the body matrix (chest and all part roots) at mBodyPos.y + it (at least mGroundY + 100); daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0A6 | `m0AA` | `mPoseEdit0X` | action_s 02085630 copies it to mMove[0].mPoseTarget.x while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0A8 | `m0AC` | `mPoseEdit0Y` | action_s 02085630 copies it to mMove[0].mPoseTarget.y while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0AA | `m0AE` | `mPoseEdit0Z` | action_s 02085630 copies it to mMove[0].mPoseTarget.z while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0AC | `m0B0` | `mPoseEdit2X` | action_s 02085630 copies it to mMove[2].mPoseTarget.x while mPoseEditMode is set (string 1 has no entry); daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0AE | `m0B2` | `mPoseEdit2Y` | action_s 02085630 copies it to mMove[2].mPoseTarget.y while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0B0 | `m0B4` | `mPoseEdit2Z` | action_s 02085630 copies it to mMove[2].mPoseTarget.z while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0B2 | `m0B6` | `mPoseEdit3X` | action_s 02085630 copies it to mMove[3].mPoseTarget.x while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0B4 | `m0B8` | `mPoseEdit3Y` | action_s 02085630 copies it to mMove[3].mPoseTarget.y while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0B6 | `m0BA` | `mPoseEdit3Z` | action_s 02085630 copies it to mMove[3].mPoseTarget.z while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0B8 | `m0BC` | `mPoseEdit4X` | action_s 02085630 copies it to mMove[4].mPoseTarget.x while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0BA | `m0BE` | `mPoseEdit4Y` | action_s 02085630 copies it to mMove[4].mPoseTarget.y while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0BC | `m0C0` | `mPoseEdit4Z` | action_s 02085630 copies it to mMove[4].mPoseTarget.z while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0BE | `m0C2` | `mPoseEdit5X` | action_s 02085630 copies it to mMove[5].mPoseTarget.x while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0C0 | `m0C4` | `mPoseEdit5Y` | action_s 02085630 copies it to mMove[5].mPoseTarget.y while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0C2 | `m0C6` | `mPoseEdit5Z` | action_s 02085630 copies it to mMove[5].mPoseTarget.z while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0C4 | `m0C8` | `mPoseEdit6X` | action_s 02085630 copies it to mMove[6].mPoseTarget.x while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0C6 | `m0CA` | `mPoseEdit6Y` | action_s 02085630 copies it to mMove[6].mPoseTarget.y while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0C8 | `m0CC` | `mPoseEdit6Z` | action_s 02085630 copies it to mMove[6].mPoseTarget.z while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0CA | `m0CE` | `mPoseEdit7X` | action_s 02085630 copies it to mMove[7].mPoseTarget.x while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0CC | `m0D0` | `mPoseEdit7Y` | action_s 02085630 copies it to mMove[7].mPoseTarget.y while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0CE | `m0D2` | `mPoseEdit7Z` | action_s 02085630 copies it to mMove[7].mPoseTarget.z while mPoseEditMode is set; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x0D0 | `m0D4` | `mPoseSpeedBase` | action_main 02086130 resets mPoseSpeedTarget to it every frame (punch_LR raises it for its frame); daBgn_HIO_c_ct 02083558 sets 40 (tww the same) |
| 0x0D4 | `m0D8` | `mKeeseDanceTime` | dance_0 (in action_main 02086130) sets mTimers[0] to it for the first dance (mDanceCount 0, the one that calls the keese); daBgn_HIO_c_ct 02083558 sets 120 (tww the same) |
| 0x0D6 | `m0DA` | `mDanceAStepTime` | dance_A (in action_main 02086130) sets mTimers[3] to it at each switch between dance_pause_1 and dance_pause_2; daBgn_HIO_c_ct 02083558 sets 20 (tww the same) |
| 0x0D8 | `m0DC` | `mDanceASpin3HP` | dance_A (in action_main 02086130) eases mDanceAngleSpeed to it at 3 health; daBgn_HIO_c_ct 02083558 sets 1200 (tww the same) |
| 0x0DA | `m0DE` | `mDanceASpin2HP` | dance_A (in action_main 02086130) eases mDanceAngleSpeed to it at 2 health; daBgn_HIO_c_ct 02083558 sets 1500 (tww the same) |
| 0x0DC | `m0E0` | `mDanceASpin1HP` | dance_A (in action_main 02086130) eases mDanceAngleSpeed to it at 1 health; daBgn_HIO_c_ct 02083558 sets 2000 (tww the same) |
| 0x0DE | `m0E2` | `mDanceTime3HP` | dance_0 (in action_main 02086130) sets mTimers[0] to it for the later dances (mDanceCount not 0) at 3 health; daBgn_HIO_c_ct 02083558 sets 300 (tww DEMO_SELECT(150, 300)) |
| 0x0E0 | `m0E4` | `mDanceTime2HP` | dance_0 (in action_main 02086130) sets mTimers[0] to it for the later dances at 2 health; daBgn_HIO_c_ct 02083558 sets 200 (tww DEMO_SELECT(100, 200)) |
| 0x0E2 | `m0E6` | `mDanceTime1HP` | dance_0 (in action_main 02086130) sets mTimers[0] to it for the later dances at 1 health; daBgn_HIO_c_ct 02083558 sets 150 (tww DEMO_SELECT(80, 150)) |
| 0x0E4 | `m0E8` | `mDanceBStepTime3HP` | dance_B (in action_main 02086130) sets mTimers[3] to it at each switch between dance_pause_3 and dance_pause_4 at 3 health; daBgn_HIO_c_ct 02083558 sets 15 (tww DEMO_SELECT(20, 15)) |
| 0x0E6 | `m0EA` | `mDanceBStepTime2HP` | dance_B (in action_main 02086130) sets mTimers[3] to it at each pose switch at 2 health; daBgn_HIO_c_ct 02083558 sets 15 (tww the same) |
| 0x0E8 | `m0EC` | `mDanceBStepTime1HP` | dance_B (in action_main 02086130) sets mTimers[3] to it at each pose switch at 1 health; daBgn_HIO_c_ct 02083558 sets 15 (tww DEMO_SELECT(10, 15)) |
| 0x0EA | `m0EE` | `mDanceBTurnStep` | dance_B (in action_main 02086130): the max step of mDanceAngleY toward +-0x1555; daBgn_HIO_c_ct 02083558 sets 500 (tww the same) |
| 0x0EC | `m0F0` | `mHandMax` | daBgn_Create 02080BA4 and move 0208857C copy it to BGN_HAND_MAX (the arm part count); daBgn_HIO_c_ct 02083558 sets 5 (tww the same) |
| 0x0EE | `m0F2` | `mTailMax` | daBgn_Create 02080BA4 and move 0208857C copy it to BGN_TAIL_MAX (the tail part count); daBgn_HIO_c_ct 02083558 sets 10 (tww the same) |
| 0x0F0 | `m0F4` | `mScale` | part_mtx_set 0207DE64 scales every part by mPartScale * it; move 0208857C scales the chest (mChestScale), pelvis (mPelvisScale) and the part spacings by it; daBgn_HIO_c_ct 02083558 sets 1.3 (tww the same) |
| 0x0F4 | `m0F8` | `mHeadScale` | move 0208857C (size_set) sets mHeadParts[0].mPartScale to it; daBgn_HIO_c_ct 02083558 sets 1.6 (tww the same) |
| 0x0F8 | `m0FC` | `mChestScale` | move 0208857C (shape_calc) scales the chest matrix and mBodyAtSph by it * mScale; daBgn_HIO_c_ct 02083558 sets 3.25 (tww the same) |
| 0x0FC | `m100` | `mPelvisScale` | move 0208857C (size_set) sets mPelvisParts[0] / [1].mPartScale to it and scales the leg / tail root matrix by it * mScale; daBgn_HIO_c_ct 02083558 sets 1.8 (tww the same) |
| 0x100 | `m104` | `mArmRootScale` | move 0208857C (size_set): the arm parts' mPartScale runs from it at the shoulder down to mArmTipScale; daBgn_HIO_c_ct 02083558 sets 1.2 (tww the same) |
| 0x104 | `m108` | `mArmTipScale` | move 0208857C (size_set): mPartScale of the hand part (mLeft / mRightArmParts[BGN_HAND_MAX]), the end of the mArmRootScale run; daBgn_HIO_c_ct 02083558 sets 0.3 (tww the same) |
| 0x108 | `m10C` | `mLegRootScale` | move 0208857C (size_set): the leg parts' mPartScale runs from it at the hip down to mLegTipScale; daBgn_HIO_c_ct 02083558 sets 1.2 (tww the same) |
| 0x10C | `m110` | `mLegTipScale` | move 0208857C (size_set): mPartScale of the foot part (mLeft / mRightLegParts[3]); daBgn_HIO_c_ct 02083558 sets 0.4 (tww the same) |
| 0x110 | `m114` | `mTailRootScale` | move 0208857C (size_set): the tail parts' mPartScale runs from it down to mTailTipScale (the last part 0.7 * it); daBgn_HIO_c_ct 02083558 sets 1.2 (tww the same) |
| 0x114 | `m118` | `mTailTipScale` | move 0208857C (size_set): the end of the mTailRootScale run; daBgn_HIO_c_ct 02083558 sets 0.1 (tww the same) |
| 0x118 | `m11C` | `mBodyPartSpacing` | move 0208857C passes it * mScale to part_control_0 0207D858 / part_control_2 0207DC84 as the part spacing of the head and pelvis chains; daBgn_HIO_c_ct 02083558 sets 185 (tww the same) |
| 0x11C | `m120` | `mLegPartSpacing` | move 0208857C passes it * mScale to part_control_0 / part_control_2 as the part spacing of both leg chains; daBgn_HIO_c_ct 02083558 sets 185 (tww the same) |
| 0x120 | `m124` | `mArmPartSpacing` | move 0208857C eases mArmLength (the arm chains' spacing) to it + the punch stretch; part_mtx_set 0207DE64 stretches the arm parts by mArmLength - it; daBgn_HIO_c_ct 02083558 sets 185 (tww the same) |
| 0x124 | `m128` | `mTailPartSpacing` | move 0208857C passes it * mScale to part_control_0 / part_control_2 as the part spacing of the tail chain; daBgn_HIO_c_ct 02083558 sets 185 (tww the same) |
| 0x128 | `m12C` | `mHeadRootPosX` | move 0208857C: x of the head root in the body matrix (MtxPosition into mHeadParts[0].mPartPos); daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x12C | `m130` | `mHeadRootPosY` | move 0208857C: y of the head root in the body matrix; daBgn_HIO_c_ct 02083558 sets -10 (tww the same) |
| 0x130 | `m134` | `mHeadRootPosZ` | move 0208857C: z of the head root in the body matrix; daBgn_HIO_c_ct 02083558 sets 120 (tww the same) |
| 0x134 | `m138` | `mShoulderPosX` | move 0208857C: x of the left shoulder in the body matrix (* mPartLengthScale; the right one takes -it) for mLeft / mRightArmParts[0]; daBgn_HIO_c_ct 02083558 sets 100 (tww the same) |
| 0x138 | `m13C` | `mShoulderPosY` | move 0208857C: y of both shoulders in the body matrix (* mPartLengthScale); daBgn_HIO_c_ct 02083558 sets 25 (tww the same) |
| 0x13C | `m140` | `mShoulderPosZ` | move 0208857C: z of both shoulders in the body matrix (* mPartLengthScale); daBgn_HIO_c_ct 02083558 sets 20 (tww the same) |
| 0x140 | `m144` | `mPelvisRootPosX` | move 0208857C: x of the pelvis root in the body matrix (* mPartLengthScale) for mPelvisParts[0]; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x144 | `m148` | `mPelvisRootPosY` | move 0208857C: y of the pelvis root in the body matrix ((it + 20) * mPartLengthScale); daBgn_HIO_c_ct 02083558 sets -115 (tww the same) |
| 0x148 | `m14C` | `mPelvisRootPosZ` | move 0208857C: z of the pelvis root in the body matrix (* mPartLengthScale); daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x14C | `m150` | `mHipPosX` | move 0208857C: x of the left hip in the pelvis matrix (* mPartLengthScale; the right one takes -it), + the pelvis position, for mLeft / mRightLegParts[0]; daBgn_HIO_c_ct 02083558 sets 80 (tww the same) |
| 0x150 | `m154` | `mHipPosY` | move 0208857C: y of both hips in the pelvis matrix (* mPartLengthScale); daBgn_HIO_c_ct 02083558 sets -20 (tww the same) |
| 0x154 | `m158` | `mHipPosZ` | move 0208857C: z of both hips in the pelvis matrix (* mPartLengthScale); daBgn_HIO_c_ct 02083558 sets 100 (tww the same) |
| 0x158 | `m15C` | `mTailRootPosX` | move 0208857C: x of the tail root in the pelvis matrix (* mPartLengthScale), + the pelvis position, for mTailParts[0]; daBgn_HIO_c_ct 02083558 sets 0 (tww the same) |
| 0x15C | `m160` | `mTailRootPosY` | move 0208857C: y of the tail root in the pelvis matrix (* mPartLengthScale); daBgn_HIO_c_ct 02083558 sets -60 (tww the same) |
| 0x160 | `m164` | `mTailRootPosZ` | move 0208857C: z of the tail root in the pelvis matrix (* mPartLengthScale); daBgn_HIO_c_ct 02083558 sets 150 (tww the same) |
| 0x164 | `m168` | `mPartOffsetZ` | part_mtx_set 0207DE64 translates every part matrix by (0, 0, it) after the scale and places the string tops of the non-head parts there; daBgn_HIO_c_ct 02083558 sets 100 (tww the same) |
| 0x168 | `m16C` | `mPartDropHung` | part_control_0 0207D858 adds it to each part's y (not below the ground) while the part's string is intact; daBgn_HIO_c_ct 02083558 sets -50 (tww the same) |
| 0x16C | `m170` | `mPartDropCut` | part_control_0 0207D858 and part_control_0Z (in move 0208857C) add it to each part's y while the string is cut (-40 in a cut scene); daBgn_HIO_c_ct 02083558 sets -100 (tww the same) |

Open in daBgn_HIO_c: `m014`, `m016`, `m018`, `m01C`, `m020`, `m026`, `m044`, `m048`, `m050` (constructor only, no read in
WWHD or tww), `m024` (tww's form change counts it up, nothing reads it; HD sets mForm directly).

String helper struct move_s (offsets within the struct; bgn_class mMove[8]):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x02D0 | `m2D0` | `mCut` | daAtt_Execute 020587B4 (d_a_att, view bgn_att_move_s) sets 1 when the string's mHitPoints reach 0; part_control_0 0207D858 / part_mtx_set 0207DE64 let the hanging part drop while it is set; head_recover 02084B90 clears it; action_s 02085630 clears it in HIO pose mode |
| 0x02D4 | `m2D4` | `mPose` | action_s 02085630 eases it toward mPoseTarget by mPoseSpeed; dance_0 (in action_main 02086130) and the part code add it to the string's limb position |
| 0x02E0 | `m2E0` | `mPoseTarget` | start 020844F4 / dance_0 and tail_attack 0208459C (in action_main 02086130) load the pose angles (bgnPoses, dance_pause tables); action_s 02085630 eases mPose toward it |
| 0x02EC | `m2EC` | `mSwayAmplitude` | damage 02084804 / body_attack (in action_main 02086130) ease it to 250; action_s_himo 020859AC eases it to 0 and swings the rope points by sin * it; daAtt_Execute 020587B4 (d_a_att, view bgn_att_move_s) targets the string only below 1 |
| 0x02F4 | `m2F4` | `mLimbShakeAmplitude` | action_s 02085630 eases it to 300 + mLimbShakeTimer * 20; damage 02084804 / hensin 02084D6C / punch_LR (in action_main 02086130) set 8000 / 6000 / 5000; part_control_0 0207D858 and move 0208857C shake the limb by sin / cos * it |
| 0x02F8 | `m2F8` | `mLimbShakeTimer` | damage 02084804 / hensin 02084D6C / dance_0 (in action_main 02086130) set it (0x46, 60, ...); action_s 02085630 counts it down and raises the mLimbShakeAmplitude target by it |
| 0x02FA | `m2FA` | `mPhaseSin` | daBgn_Create 02080BA4 sets a random start; action_s 02085630 adds 2600 + 3 * index per frame; the phase of the sin terms of mLimbShakeAmplitude (part_control_0 0207D858, move 0208857C) and mCutSwayAmplitude (action_s_himo 020859AC) |
| 0x02FC | `m2FC` | `mPhaseCos` | daBgn_Create 02080BA4 sets a random start; action_s 02085630 adds 2300 + 3 * index per frame; the phase of the cos terms of mLimbShakeAmplitude and mCutSwayAmplitude |
| 0x0300 | `m300` | `mPluckTimer` | daAtt_Execute 020587B4 (d_a_att, view bgn_att_move_s) sets 15 when a hit does not cut the string; action_s_himo 020859AC counts it down and vibrates the rope by it * sin; part_mtx_set 0207DE64 sums [0] + [1] |
| 0x0304 | `m304` | `mCutSwayAmplitude` | daAtt_Execute 020587B4 (d_a_att, view bgn_att_move_s) sets 6000 when the string is cut; action_s_himo 020859AC swings the rope by sin / cos (mPhaseSin / mPhaseCos) * it while above 0.01 and eases it to 0 |
| 0x0308 | `m308` | `mHitPoints` | daAtt_Execute 020587B4 (d_a_att, view bgn_att_move_s) subtracts 1 per hit on the string and cuts it (mCut) at 0; head_recover 02084B90 sets 2 on the restored strings, daBgn_Create 02080BA4 3 |

Open in move_s: `m2E8` (punch_LR sets 100, nothing reads it), `m2FE` (only counted down) and the padding. The
string targets' view `bgn_att_move_s` (d_a_att.h) uses the same names.

Body part helper struct part_s (offsets within the struct; bgn_class mHeadParts, mPelvisParts, mLeftArmParts,
mRightArmParts, mLeftLegParts, mRightLegParts, mTailParts). The `mPart` prefix follows the names part_s already has
(`mpPartModel`, `mPartTevStr`, `mPartSph`, `mPartArrowHit*`):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0004 | `m004` | `mPartPacket` | part_init 0207FDA4 / useHeapInit 0207FE14 set it up (the part's HD reflection packet); part_draw 0207ACD0 gives it the part model and updates it while l_HIO.m00C is set; part_s_ct 02080498 / part_s_dt 0208AC78 construct and destroy it (as bgn_class mChestPacket) |
| 0x027C | `m0C8` | `mPartFogColorAdd` | part_draw 0207ACD0 eases it to 0x118 / 0 while mPartArrowHitFlashTimer runs (above / below 40) and adds it to the fog colour of mPartTevStr (same use as bgn_class mFogColorAdd) |
| 0x0280 | `m0CC` | `mPartFogStartAdd` | part_draw 0207ACD0 eases it to -50000 / 0 while mPartArrowHitFlashTimer runs and adds it to mFogStartZ of mPartTevStr (same use as bgn_class mFogStartAdd) |
| 0x0286 | `m0D2` | `mPartCoreHitFrame` | useHeapInit 0207FE14 sets 16 (head), 10 (pelvis), i * 3 + 13 (arms), i * 3 + 10 (legs), 11 - i (tail); part_mtx_set 0207DE64 starts the part's arrow-hit flash (mPartArrowHitFlashTimer 50, mPartArrowHitEffectTimer 100) when bgn_class mCoreHitFrame equals it |
| 0x0288 | `m0D4` | `mPartPos` | move 0208857C (MtxPosition) and part_control_0 0207D858 / part_control_2 0207DC84 chain the parts by it; part_mtx_set 0207DE64 translates the part matrix to it (not below mGroundY) and puts the arrow-hit emitters there; part_draw 0207ACD0 lights the part at it; punch_LR (in action_main 02086130) / hensin 02084D6C read the hand / head position from it |
| 0x0294 | `m0E0` | `mPartAngle` | part_control_0 0207D858 and move 0208857C (part_control_0Z) set .x / .y from the chain direction; part_mtx_set 0207DE64 eases .z toward mBodyAngle.y and rotates the part matrix by it; move rotates the leg roots by the pelvis part's |
| 0x02A8 | `m0F4` | `mPartScale` | move 0208857C (size_set) sets it per limb from l_HIO.m0F8 .. m114; part_mtx_set 0207DE64 scales the part matrix and mPartSph by it * l_HIO.m0F4; part_control_0 0207D858 / part_control_2 0207DC84 space the chain by it; part_draw 0207ACD0 compares the camera distance with l_HIO.m008 * it |
| 0x03D8 | `m224` | `mPartCenter` | part_mtx_set 0207DE64 stores the transformed part origin (also the mPartSph centre) in it; part_draw 0207ACD0 measures its distance to the camera eye and draws the part only beyond l_HIO.m008 * mPartScale |

Open in part_s: the padding only. No other header has a view of part_s (bgn3's `part_s3` in d_a_bgn3.h and its
draw view in d_a_bgn_draw.h are a different struct).

## bst_class (Gohdan: head and both hands; d_a_bst.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03D4 | `m02B8` | `mpMorf` | useHeapInit 020E7278 creates the mDoExt_McaMorf of the head / hand body; daBst_Draw 020E4A64 enters it with mpTexMtxAnimator / mpTevRegAnimator; beam_attack 020E9F08, demo_camera 020EE200 and hana_demo 020EACE0 read its frame and joint matrices |
| 0x03E0 | `m02C4` | `mBodyAnmOn` | daBst_Execute 020E54A0 plays mpTevRegAnimator / mpTexMtxAnimator (the body animations) only while it is set; fly (in daBst_move 020EB4BC) and demo_camera 020EE200 set 1 |
| 0x03E4 | `m02C8` | `mpPedestalModel` | useHeapInit 020E7278 creates it from set_za_bdl[mBstPartType]; daBst_Draw 020E4A64 draws it with mpBtkAnm / mpBrkAnm and mEnvLight at home.pos; bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) places it at home.pos / home.angle |
| 0x03F0 | `m02D4` | `mPedestalAnmOn` | daBst_Execute 020E54A0 plays mpBrkAnm / mpBtkAnm (the mpPedestalModel animations) only while it is set; fly (in daBst_move 020EB4BC) and demo_camera 020EE200 set 1 |
| 0x05BC | `m0388` | `mpShadowModel` | useHeapInit 020E7278 creates a second model from the mpMorf model data; bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) gives it the body base matrix; zeldaret/tww daBst_Draw copies the body joint matrices into it and passes it to dComIfGd_setRealShadow |
| 0x05C4 | `m0390` | `mpBeamModel` | useHeapInit 020E7278 creates the 10 BDL_TAMA models; beam_move 020EDC8C places each at mBeamPos / mBeamAngle, daBst_Draw 020E4A64 draws the ones whose mBeamState is set |
| 0x05EC | `m03B8` | `mBeamPos` | beam_attack 020E9F08 sets it to the mouth joint (BST_JNT_BUKI2) when a beam fires; beam_move 020EDC8C adds mBeamSpeed, checks ground / walls there and moves mCcD_beams to it; daBst_Delete 020E7190 stops the sound there |
| 0x0664 | `m0430` | `mBeamSpeed` | beam_attack 020E9F08: (0, 0, REG0_F(12) + 160) turned by the head angle; beam_move 020EDC8C adds it to mBeamPos each frame |
| 0x06DC | `m04A8` | `mBeamAngle` | beam_attack 020E9F08 copies shape_angle into it; beam_move 020EDC8C rotates mpBeamModel by its y / x and passes y to beam_eff_set on a wall hit |
| 0x0718 | `m04E4` | `mBeamState` | beam_attack 020E9F08 takes a slot with 0 and sets 1; beam_move 020EDC8C starts mCcD_beams at 1 (then 2), moves it at 2, clears it on a hit or above 2000; daBst_Draw 020E4A64 draws mpBeamModel while it is set |
| 0x12DC | `m10A8` | `mBeamLength` | beam_attack 020E9F08 sets 1.0; beam_move 020EDC8C scales mpBeamModel along z by it (scaleM(1, 1, it)) and grows it toward REG0_F(4) + 5 |
| 0x1304 | `m10D0` | `mBeamAimOffsetY` | beam_attack 020E9F08 adds it to the player height difference for the aim pitch and eases it to REG0_F(6); main_cont (in daBst_Execute 020E54A0) sets REG0_F(7) - 500 when the beam attack starts |
| 0x130C | `m10D8` | `mPrevAction` | fly (in daBst_move 020EB4BC) sets 1 (ACTION_FLY_e), sleep 6 (ACTION_SLEEP_e); fly refills a hand's health to 4 when it is still 6 (the hand comes back from sleep) |
| 0x1320 | `m10EC` | `mMoveParam` | x: turn-step limit, y: its ramp, z: shake amplitude; pos_move 020E4CD0 turns current.angle by at most x * y and eases y to 1; the attack actions (kumi 020E90F0, harai 020E9AA0, paa 020E888C, daBst_move 020EB4BC) set x and clear y; bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) shakes the position by z and eases z to 0 |
| 0x132C | `m10F8` | `mSwayAngle` | head_hukki (in daBst_move 020EB4BC) eases it to REG0_F(4) + 4000 and back to 0 and sets shape_angle.x / z = it * sin; bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) eases it to 0 |
| 0x1330 | `m10FC` | `mTimers` | daBst_Execute 020E54A0 counts all 5 down; [0] action steps (beam_attack 020E9F08 45 / 50, harai 020E9AA0, hana_demo 020EACE0 40), [1] sleep (main_cont 100 / 3), [3] end_demo 020EB174; zeldaret/tww: 0 = mUpdateTargetPosTimer, 1 = mSleepTimer |
| 0x1344 | `m1110` | `mBodyOffsetX` | bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) translates the body matrix by (it, mBodyOffsetY, mBodyOffsetZ) after the rotation and eases it to 0; kumi 020E90F0 / harai 020E9AA0 ease it up |
| 0x1348 | `m1114` | `mBodyOffsetY` | bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) translates the body matrix by it on y and eases it to 0; kumi 020E90F0 and down_attack (in daBst_move 020EB4BC) ease it to -200 |
| 0x134C | `m1118` | `mBodyOffsetZ` | bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) translates the body matrix by it on z and eases it to 0; paa 020E888C eases it to -40, kumi 020E90F0 to its target |
| 0x1350 | `m111C` | `mAttackBaseYaw` | paa 020E888C and kumi 020E90F0 store the player's shape_angle.y in it at the start and turn the hand around it (YrotS(it), shape_angle.y toward it + offset) |
| 0x135E | `m112A` | `mImpactShakeTimer` | down_attack / head_damage (in daBst_move 020EB4BC), paa 020E888C, kumi 020E90F0 and demo_camera 020EE200 set REG + 10 / 4 / 5 / 3 on an impact; daBst_Execute 020E54A0 counts it down; bstExecuteTransformAndCollision shakes the body yaw / pitch by it * (REG0_F(14) + 150) |
| 0x1360 | `m112C` | `mPitchShakeTimer` | daBst_Execute 020E54A0 counts it down; bstExecuteTransformAndCollision (in daBst_Execute) shakes the body pitch by it * (REG0_F(14) + 100); no writer in HD or zeldaret/tww |
| 0x30A2 | `m2E6E` | `mDownAttackYaw` | down_attack (in daBst_move 020EB4BC) sets shape_angle.y +/- 0x4000 and turns shape_angle.y toward it |
| 0x30A8 | `m2E74` | `mEyeCloseTimer` | beam_attack 020E9F08 and main_cont (in daBst_Execute 020E54A0) set 10; bstExecuteTransformAndCollision counts it down, keeps the eyelid (mEyelidAngle) closed and the eye target (mEyeAttIds) off while it is set |
| 0x30AC | `m2E78` | `mEyelidAngle` | bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) eases it to 0 (open) or REG0_S(8) + 11000 (closed); nodeCallBackHead 020E48F8 rotates the eyelid joints by it |
| 0x30B0 | `m2E7C` | `mBattlePhase` | main_cont (in daBst_Execute 020E54A0) switch: 0 start, 1 wait for the player (then 2 with mDemoCamMode 10 for the intro, or 10 when the intro was seen), 10 -> 11 hand attacks, 12 head (beam) phase; fly / daBst_move 020EB4BC read it, demo_camera 020EE200 sets 10 / 100 |
| 0x30B2 | `m2E7E` | `mBattleTimers` | main_cont (in daBst_Execute 020E54A0) counts all 3 down; [0] next hand attack, [1] next beam attack (beam_attack 020E9F08 reloads it; zeldaret/tww: 1 = mHeadBeamAttackTimer), [2] head phase delay (50) |
| 0x30C4 | `m2E90` | `mEyeAttIds` | daBst_Create 020E7DC0 creates two fpcNm_ATT_e actors and keeps their ids; bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) puts them on the eye joints with mEyeHealth; daBst_Delete 020E7190 deletes them |
| 0x30CC | `m2E98` | `mBlurRate` | daBst_Draw 020E4A64 turns the motion blur on with this rate while it is > 1 and off at 1 (zeldaret/tww: mDoGph_gInf_c::setBlureRate); demo_camera 020EE200 sets 100 / 120 / 150 / 1 |
| 0x30CE | `m2E9A` | `mDemoCamMode` | demo_camera 020EE200 switch: CAMERA_IDLE, CAMERA_LEFT_HAND .. CAMERA_FOLLOW_ITEM; the original's report "K DEMO MODET %d" prints it; main_cont (in daBst_Execute 020E54A0) sets 10 and skips the battle while it is set (mDemoMode is the player member read in demo_camera) |
| 0x30D2 | `m2E9E` | `mItemNoseSide` | hana_demo 020EACE0 sets 0 (arrows from BST_JNT_HANAL) / 1 (bombs from BST_JNT_HANAR) and picks the smoke effect by it; demo_camera 020EE200 places the camera on that side |
| 0x30D4 | `m2EA0` | `mCamEye` | demo_camera 020EE200 moves it and passes it as the eye of dCamera_c::Set (with the mCamShake offset) |
| 0x30E0 | `m2EAC` | `mCamCenter` | demo_camera 020EE200 moves it and passes it as the center of dCamera_c::Set (with the mCamShake offset) |
| 0x30F8 | `m2EC4` | `mCamWork` | demo_camera 020EE200: a ramp 0 -> 1 that scales the eye chase steps (40 / 100 / 10 * it), and in CAMERA_NOSE an eye height offset eased to -650 |
| 0x30FC | `m2EC8` | `mCamFovy` | demo_camera 020EE200 sets 55 / 75 and eases it to 30 / 55; passes it as the fovy of dCamera_c::Set |
| 0x3100 | `m2ECC` | `mCamShake` | demo_camera 020EE200 sets REG + 10, shakes eye and center by it * sin / cos (bank 7.5 * it) and eases it to 0; stay (in daBst_move 020EB4BC) sets it on the head |
| 0x3104 | `m2ED0` | `mRoomColorBlend` | bstExecuteTransformAndCollision (in daBst_Execute 020E54A0) passes it to the custom color set by mRoomState and eases it to 1 / 0; daBst_Create 020E7DC0 and demo_camera 020EE200 set 1 / 0 |
| 0x310C | `m2ED8` | `mpChargeEmitter` | beam_attack 020E9F08 sets the two charge_e_name emitters and keeps them on joint 0; it and daBst_Execute 020E54A0 retire them outside the beam attack |
| 0x3114 | `m2EE0` | `mpBombEmitter` | head_damage (in daBst_move 020EB4BC) sets [0] = 0x81E3 (ID_AK_SN_BSTBOMBFLASH00) and [1..] = the bomb effects on the head joint; end_demo 020EB174 retires [1..] |
| 0x3128 | `m2EF4` | `mpHaloEmitter` | end_demo 020EB174 sets particle 0x81E8 (ID_AK_SN_BSTHALO00) on the head joint |
| 0x312C | `m2EF8` | `mpStageEffEmitter` | daBst_Create 020E7DC0 and demo_camera 020EE200 set particle 0x81E9 (ID_AK_SN_BSTSTAGEEFFECT00); demo_camera retires it |
| 0x3154 | `m2F20` | `mBgTevStr` | daBst_Draw 020E4A64 sets it up as TEV type 1 (TEV_TYPE_BG0) at center_pos and lights mpHbyokoModel / mpHbsitaModel with it; bstConstructor 020E78C4 / daBst_Create 020E7DC0 initialise it |
| 0x331C | `m2FD0` | `mpHbsitaModel` | useHeapInit 020E7278 creates it from dRes_INDEX_BST_BDL_HBSITA1_e; daBst_Draw 020E4A64 draws it with mpHbsitaBtkAnm / mpHbsitaBrkAnm; daBst_Execute 020E54A0 sets its base matrix |
| 0x3320 | `m2FD4` | `mpHbsitaBtkAnm` | useHeapInit 020E7278 creates it with dRes_INDEX_BST_BTK_HBSITA1_e for mpHbsitaModel; daBst_Draw 020E4A64 enters it, daBst_Execute 020E54A0 plays it |
| 0x3324 | `m2FD8` | `mpHbsitaBrkAnm` | useHeapInit 020E7278 creates it with dRes_INDEX_BST_BRK_HBSITA1_e for mpHbsitaModel; daBst_Execute 020E54A0 loops it while the player is below the limit height and plays it |
| 0x3328 | `m2FDC` | `mpHbyokoModel` | useHeapInit 020E7278 creates it from dRes_INDEX_BST_BDL_HBYOKO1_e; daBst_Draw 020E4A64 draws it with mpHbyokoBrkAnm; daBst_Execute 020E54A0 sets its base matrix |
| 0x332C | `m2FE0` | `mpHbyokoBrkAnm` | useHeapInit 020E7278 creates it with dRes_INDEX_BST_BRK_HBYOKO1_e for mpHbyokoModel; daBst_Execute 020E54A0 plays it; main_cont / demo_camera 020EE200 set its rate to 1 |
| 0x3330 | `m2FE4` | `mBattleCamOn` | daBst_Execute 020E54A0 sets the camera type "Test01" while it is set; main_cont (in daBst_Execute) sets 1 when the intro was seen (STAGE_BOSS_DEMO), demo_camera 020EE200 sets 0 at the intro start and 1 at its end |

Open (no name yet): the padding bytes only. zeldaret/tww (CC0) d_a_bst.h already had names for 36 bst_class
fields (`mDamage`, `mTargetPos`, `mState`, `mStateTimer`, `mEyeHealth`, `mBombId`, ...); the header already
used them, and its old line comments ("beam positions", "camera eye", ...) are kept after the evidence. Its
d_a_bst.cpp was read alongside our decomp to follow the uses. `mDemoCamMode` (not mDemoMode) because
demo_camera also reads the player's `mDemoMode`; `mCamEye` / `mCamCenter` are named after TP d_a_b_yo.h
mCamEye / mCamCenter (a different class with the same use; the evidence is the WWHD use as the eye and center
arguments of dCamera_c::Set).

## bgn2_class (Puppet Ganon, second form; d_a_bgn2.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D4 | `m02B8` | `mHeadPacket` | useHeapInit 0208EEA4 sets the mDoExt_J3DModelPacketS up; daBgn_Draw 0207B2CC (inlined daBgn2_Draw) gives it the mpHeadMorf model and updates it |
| 0x488 | `m02D0` | `mBodyPacket` | useHeapInit 0208EEA4 sets the mDoExt_J3DModelPacketS up; daBgn_Draw 0207B2CC (inlined daBgn2_Draw) gives it the mpBodyMorf model and updates it |
| 0x538 | `m02E4` | `mJyakutenPacket` | useHeapInit 0208EEA4 sets the mDoExt_J3DModelPacketS up; daBgn_Draw 0207B2CC (inlined daBgn2_Draw) gives it the mpJyakutenModel shown for the current health and updates it |
| 0x600 | `m0310` | `mFrameCounter` | daBgn2_Execute 0208B6A4 adds 1 per frame and uses it as the cos / sin phase of the head shake; asi_hamon_set 0208B268 / asi_eff_set (in daBgn2_Execute) and the rope sway in move use it as a phase |
| 0x602 | `m0312` | `mAction` | move (in daBgn2_Execute 0208B6A4) switch: 0 start, 1 plesattack, 2 jumpattack, 4 mahi, 5 damage, 6 hensin, 10 none; damage_check sets 4 / 5 / 6 |
| 0x604 | `m0314` | `mMode` | the step inside mAction: start / plesattack / jumpattack / damage / hensin (in daBgn2_Execute 0208B6A4) switch on it; damage_check sets 0 |
| 0x608 | `m0318` | `mTargetPos` | plesattack (in daBgn2_Execute 0208B6A4) copies the player position and turns toward it (cM_atan2s) |
| 0x620 | `m0330` | `mTimers` | daBgn2_Execute 0208B6A4 counts all 5 down; plesattack / mahi / hensin load [0] from l_HIO or constants and wait on it |
| 0x62A | `m033A` | `mDamageTimer` | damage_check (in daBgn2_Execute 0208B6A4) takes a core hit only at 0 and sets 20; daBgn2_Execute counts it down |
| 0x62C | `m033C` | `mHitSeTimer` | damage_check (in daBgn2_Execute 0208B6A4) plays def_se_set for a head / body / leg hit only at 0 and sets 10; daBgn2_Execute counts it down |
| 0x630 | `m0340` | `mMovePos` | pos_move 0208B530 adds speed to it (back to mMovePosOld on a wall); daBgn2_Create 0208F218 sets current.pos; move (in daBgn2_Execute 0208B6A4) eases current.pos x / z to it, hensin eases it to 0 |
| 0x63C | `m034C` | `mMovePosOld` | pos_move 0208B530 keeps mMovePos in it before the step and restores mMovePos from it |
| 0x648 | `m0358` | `mShakeTimer` | damage_check (in daBgn2_Execute 0208B6A4) sets REG0_S(5) + 20; daBgn2_Execute counts it down and shakes the head by it * (REG0_F(14) + 500) |
| 0x68C | `m039C` | `mHeadSph` | daBgn2_Create 0208F218 sets it up (cc_sph_src); daBgn2_Execute 0208B6A4 centres it on mHeadPos (r 150); damage_check reads its hit (critical-hit flash) |
| 0x7B8 | `m04C8` | `mBodySph` | daBgn2_Create 0208F218 sets the two up (cc_sph_src); daBgn2_Execute 0208B6A4 centres them on mBodyPos (body_scale radii) and turns their At on with mBodyAtOn |
| 0xA10 | `m0720` | `mLegSph` | daBgn2_Create 0208F218 sets the 30 up; daBgn2_Execute 0208B6A4 centres them on mLegPos and turns their At on with mLegAtOn; damage_check reads their hits |
| 0x2D38 | `m2A48` | `mCoreSph` | daBgn2_Create 0208F218 sets it up from core_sph_src; daBgn2_Execute 0208B6A4 centres it on mCorePos (r 210); damage_check: its hit damages bgn2 |
| 0x2E64 | `m2B74` | `mHeadPos` | daBgn2_Execute 0208B6A4: MtxPosition of the head joint; mHeadSph is centred there and eyePos copies it |
| 0x2E70 | `m2B80` | `mBodyPos` | daBgn2_Execute 0208B6A4: MtxPosition of body_d[i]; mBodySph[i] is centred there and attack_eff_set uses it |
| 0x2E88 | `m2B98` | `mLegPos` | daBgn2_Execute 0208B6A4: MtxPosition of each leg joint; mLegSph[i] is centred there; gr_check 0208B0C0 / asi_hamon_set 0208B268 / asi_eff_set use it for the ground ripples |
| 0x3050 | `m2D60` | `mFogColorAdd` | daBgn_Draw 0207B2CC (inlined daBgn2_Draw) raises it while mArrowHitFlashTimer runs and adds it to the fog colour (same use as bgn_class mFogColorAdd) |
| 0x3054 | `m2D64` | `mFogStartAdd` | daBgn_Draw 0207B2CC (inlined daBgn2_Draw) moves the fog start by it while mArrowHitFlashTimer runs (same use as bgn_class mFogStartAdd) |
| 0x305A | `m2D6A` | `mCoreHitFrame` | damage_check (in daBgn2_Execute 0208B6A4) sets 1 on a core hit; daBgn2_Execute counts it up to 100 and starts mArrowHitEffectTimer[i] when it equals fl_check_d[i] (same use as bgn_class mCoreHitFrame) |
| 0x315C | `m2E6C` | `mCorePos` | daBgn2_Execute 0208B6A4: MtxPosition of BGN_KUMO1_JNT_JYAKUTEN; mCoreSph is centred there; damage_check sets the weak-point break / hit particles there |
| 0x3168 | `m2E78` | `mBodyAtOn` | plesattack (in daBgn2_Execute 0208B6A4) sets 1; daBgn2_Execute turns the At of mBodySph on while it is set and clears it |
| 0x3169 | `m2E79` | `mLegAtOn` | plesattack (in daBgn2_Execute 0208B6A4) sets 1; daBgn2_Execute turns the At of mLegSph on while it is set and clears it |
| 0x316C | `m2E7C` | `mFogFade` | hensin (in daBgn2_Execute 0208B6A4) eases it to 1, daBgn2_Execute back to 0, bgn hensin 02084D6C sets 1; daBgn_Draw 0207B2CC (inlined daBgn2_Draw) blacks the fog and moves its start by -50000 * it (same use as bgn_class mFogFade) |
| 0x3170 | `m2E80` | `mSpinSpeed` | move (in daBgn2_Execute 0208B6A4) adds it to shape_angle.y and eases it to mSpinSpeedTarget; damage sets 0x800, hensin 0 |
| 0x3172 | `m2E82` | `mSpinSpeedTarget` | jumpattack (in daBgn2_Execute 0208B6A4) sets l_HIO speeds (and negates it), damage 0x800, plesattack 0; move eases mSpinSpeed to it |
| 0x3300 | `m2EC4` | `mRopeSway` | plesattack / damage (in daBgn2_Execute 0208B6A4) ease it to 250; move eases it to 0 and swings the 60 points of mRedRopeMat by sin * it |
| 0x3304 | `m3304` | `mHdPacket` | useHeapInit 0208EEA4 sets the fourth mDoExt_J3DModelPacketS up (HD); daBgn_Draw 0207B2CC (inlined daBgn2_Draw) points it at mRedRopeMat and updates it; the bgn_draw::bgn2_class view already calls it mHdPacket |
| 0x33B4 | `m2EC8` | `mpPotaEmitter` | jumpattack (in daBgn2_Execute 0208B6A4) sets [0] = ID_AK_SN_KGTT2CHESTPOTA00 and [1] = ID_AK_SN_KGTT2STOMACHPOTA00 on the KARADA1 / KARADA2 joints and retires them |
| 0x33BC | `m2ED0` | `mKiSetCount` | ki_set (in daBgn2_Execute 0208B6A4) sets l_HIO.m2E; daBgn2_Execute counts it down and spawns one fpcNm_KI_e (while ki_check < l_HIO.m30) per count; daBgn_Execute 0207E7B4 clears it |
| 0x33BE | `m2ED2` | `mKiSetTimer` | ki_set (in daBgn2_Execute 0208B6A4) returns while it runs and reloads it from l_HIO.m2C; daBgn2_Execute counts it down |
| 0x35C4 | `m30D8` | `mHioOwner` | daBgn2_Create 0208F218 sets 1 when it created the l_HIO child; daBgn2_Delete 0208EE40 deletes the HIO child only then (same as bgn_class mHioOwner) |

Open (no name yet): the padding bytes (`m2D40` has no uses). zeldaret/tww (CC0) names kept: `mpHeadMorf`,
`mpBodyMorf`, `mpJyakutenModel`, `mArrowHitFlashTimer`, `mRedRopeMat`, ... The private views use the same names:
`bgn_draw::bgn2_class` (d_a_bgn_draw.h), `bgn_exec::Phase2` (d_a_bgn_exec.h), `BgnPhaseTwoView` (d_a_bgn_main.h);
the d_a_bgn2 unit spec comment follows. Names shared with bgn_class (`mFogColorAdd`, `mFogStartAdd`, `mFogFade`,
`mCoreHitFrame`, `mHioOwner`, ...) have the same use there.

### daBgn2_HIO_c (l_HIO; d_a_bgn2.h)

Round 3: the second form's HIO values. In WWHD the vtable sits at the end, so each field is 4 below its GameCube name. Each is set only by daBgn2_HIO_c_ct 0208F7C8 (default in parentheses) and never written elsewhere in WWHD or tww; the reads are in our d_a_bgn2.cpp / d_a_bgn2_exec.cpp (plesattack, jumpattack, mahi, hensin and ki_set are inlined into daBgn2_Execute 0208B6A4) and match zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00001 | `m05` | `mNoMove` | daBgn2_Execute 0208B6A4: move and the animations run only while it is 0 (tww the same) (0) |
| 0x00002 | `m06` | `mDebugHealth` | daBgn2_Execute 0208B6A4: when not 0, health is set to it (tww the same) (0) |
| 0x00004 | `m08` | `mGroundHeight` | checkGround 0208B340: the floor height; current.pos.y is clamped to it and speed.y cleared there (tww the same) (280) |
| 0x00008 | `m0C` | `mPressHeight` | plesattack (in daBgn2_Execute 0208B6A4): the height it rises to before the drop (cLib_addCalc2 target) (tww the same) (3000) |
| 0x0000C | `m10` | `mPressRiseStep` | plesattack (in daBgn2_Execute 0208B6A4): the largest cLib_addCalc2 step of the rise to mPressHeight (tww the same) (15) |
| 0x00010 | `m14` | `mGravity` | plesattack (in daBgn2_Execute 0208B6A4) / hensin (in daBgn2_Execute 0208B6A4): added to speed.y each frame (taken off again while mTimers[0] runs before the drop) (tww the same) (-2) |
| 0x00014 | `m18` | `mSpinSpeedHealth3` | jumpattack (in daBgn2_Execute 0208B6A4): mSpinSpeedTarget at health 3 (random sign) (tww the same) (0x5DC) |
| 0x00016 | `m1A` | `mSpinSpeedHealth2` | jumpattack (in daBgn2_Execute 0208B6A4): mSpinSpeedTarget at health 2 (tww the same) (0xA28) |
| 0x00018 | `m1C` | `mSpinSpeedHealth1` | jumpattack (in daBgn2_Execute 0208B6A4): mSpinSpeedTarget at health 1 (tww the same) (0xED8) |
| 0x0001A | `m1E` | `mPressHangTimeHealth3` | plesattack (in daBgn2_Execute 0208B6A4): mTimers[0] at the top before the drop at health 3 (tww the same) (60) |
| 0x0001C | `m20` | `mPressHangTimeHealth2` | plesattack (in daBgn2_Execute 0208B6A4): mTimers[0] at the top before the drop at health 2 (tww the same) (50) |
| 0x0001E | `m22` | `mPressHangTimeHealth1` | plesattack (in daBgn2_Execute 0208B6A4): mTimers[0] at the top before the drop at health 1 (tww the same) (40) |
| 0x00020 | `m24` | `mPressLandTimeHealth3` | plesattack (in daBgn2_Execute 0208B6A4): mTimers[0] after landing at health 3; at 0 the action becomes jumpattack (2) (tww the same) (100) |
| 0x00022 | `m26` | `mPressLandTimeHealth2` | plesattack (in daBgn2_Execute 0208B6A4): mTimers[0] after landing at health 2 (tww the same) (100) |
| 0x00024 | `m28` | `mPressLandTimeHealth1` | plesattack (in daBgn2_Execute 0208B6A4): mTimers[0] after landing at health 1 (tww the same) (100) |
| 0x00026 | `m2A` | `mMahiTime` | mahi (in daBgn2_Execute 0208B6A4): mTimers[0] for the stunned (mahi) state; at 0 the action becomes jumpattack (2) (tww the same) (600) |
| 0x00028 | `m2C` | `mKiSetInterval` | ki_set (in daBgn2_Execute 0208B6A4) / daBgn2_Execute: mKiSetTimer, the wait before the next Ki wave (tww the same) (100) |
| 0x0002A | `m2E` | `mKiPerLanding` | ki_set (in daBgn2_Execute 0208B6A4): mKiSetCount, the Ki created per landing (tww the same) (3) |
| 0x0002C | `m30` | `mKiMax` | daBgn2_Execute 0208B6A4: a Ki is created only while ki_check counts fewer than it (tww the same) (10) |

Open (no name yet): padding `_03`, `_2E`. `CcAtInfo_l::m0C` (the CcAtInfo copy) became `mHitAngle` with the CcAtInfo copies as a group (other.md, round 12).

## bgn3_class (Puppet Ganon, third form; d_a_bgn3.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x003D4 | `m002B8` | `mHeadPacket` | useHeapInit 02090984 sets the mDoExt_J3DModelPacketS up after mpMorf (BCK_BGN_HEAD1); daBgn_Draw 0207B2CC (inlined daBgn3_Draw) gives it the mpMorf model and updates it |
| 0x00484 | `m002CC` | `mpDekuModel` | useHeapInit 02090984 creates it from dRes_INDEX_BGN_BDL_BGN_DEKU1_e; setModels (in move 02091730) places it (scale 2) and centres mDekuSph on it; daBgn_Draw 0207B2CC draws it |
| 0x00488 | `m002D0` | `mDekuPacket` | useHeapInit 02090984 sets the mDoExt_J3DModelPacketS up after mpDekuModel; daBgn_Draw 0207B2CC (inlined daBgn3_Draw) gives it the mpDekuModel model and updates it |
| 0x11E58 | `m0FD7C` | `mPartRootPos` | part_control (in move 02091730): current.pos + (0, 0, REG - 300) turned by shape_angle; copied to mParts[0] as the start of the part chain |
| 0x11E64 | `m0FD88` | `mFrameCounter` | daBgn3_Execute 02090680 adds 1 per frame; move0 / splash / setModels (in move 02091730) use it as the phase of the turn wobble, the ripples and the head / rope shake |
| 0x11E66 | `m0FD8A` | `mAction` | move 02091730 switch: 0 move0, 1 mahi, 2 damage, 3 end; damage_check (in move) sets 1 / 2 / 3, daBgn3_Execute 02090680 resets 10 to 0 |
| 0x11E68 | `m0FD8C` | `mMode` | the step inside mAction: mahi / damage / end (in move 02091730) switch on it; damage_check sets 0 |
| 0x11E6A | `m0FD8E` | `mTurnSpeed` | move0 (in move 02091730) loads it from l_HIO by health (random sign), adds it to current.angle.y or uses it as the turn step; end eases it to 5000 and adds it to shape_angle.y |
| 0x11E6C | `m0FD90` | `mChaseTimer` | move0 (in move 02091730): while it runs bgn3 turns toward the bait, the centre or the player instead of circling; set 1 with bait, from l_HIO after mWallHitCount wall hits, 0 on a wall hit; daBgn3_Execute 02090680 sets 180 |
| 0x11E6E | `m0FD92` | `mWallHitCount` | move0 (in move 02091730) counts the wall hits and starts mChaseTimer when it reaches the l_HIO threshold for the health, then clears it |
| 0x11E70 | `m0FD94` | `mRippleSeTimer` | splash (in move 02091730) plays the ripple sound (0x6A44 / 0x6A45) only at 0 and reloads it with cM_rndF(20) + 20; daBgn3_Execute 02090680 counts it down (same use as bgn_class mRippleSeTimer) |
| 0x11E72 | `m0FD96` | `mFogColorAdd` | daBgn_Draw 0207B2CC (inlined daBgn3_Draw) raises it while mArrowHitFlashTimer runs and adds it to the fog colour (same use as bgn_class mFogColorAdd) |
| 0x11E74 | `m0FD98` | `mFogStartAdd` | daBgn_Draw 0207B2CC (inlined daBgn3_Draw) moves the fog start by it while mArrowHitFlashTimer runs (same use as bgn_class mFogStartAdd) |
| 0x11E7A | `m0FD9E` | `mCoreHitFrame` | damage_check (in move 02091730) sets 1 on a core hit; setModels counts it up to 100 and starts the arrow-hit effect of part i when it equals fl_check_d[i] (same use as bgn_class mCoreHitFrame) |
| 0x11E86 | `m0FDAA` | `mTimers` | daBgn3_Execute 02090680 counts it down (through Bgn3TimerView, five halfwords from here); [0] mahi duration (damage_check), [1] wall-hit lockout 20 (move0) |
| 0x11E8A | `m0FDAE` | `mTurnTimer` | move0 (in move 02091730) picks a new mTurnSpeed at 0 and reloads it with cM_rndF(50) + 30; counted down as timer [2] of the Bgn3TimerView sweep in daBgn3_Execute 02090680 |
| 0x11E8C | `m0FDB0` | `mCenterTimer` | daBgn3_Execute 02090680 sets 180; while it runs move0 (in move 02091730) turns toward the arena centre (centerAngle); counted down as timer [3] of the Bgn3TimerView sweep |
| 0x11E90 | `m0FDB4` | `mDamageTimer` | damage_check (in move 02091730) returns while it runs and sets 20 (10000 on the last hit); daBgn3_Execute 02090680 counts it down |
| 0x11E92 | `m0FDB6` | `mHitSeTimer` | damage_check (in move 02091730) plays the hit sound only at 0 and sets 10; daBgn3_Execute 02090680 counts it down |
| 0x11E94 | `m0FDB8` | `mPartStiffness` | part_control (in move 02091730) multiplies the part velocities by it ("stiffness") and eases it to l_HIO.m38; move0 sets REG + 0.9 on a wall hit, mahi / damage / end 1.0 |
| 0x11E98 | `m0FDBC` | `mPartDistance` | part_control (in move 02091730) places the part chain at (0, 0, -it) behind the head and eases it to l_HIO.m34; move0 sets REG + 30 while chasing the bait, end eases it to its target |
| 0x11ED8 | `m0FDFC` | `mHeadSph` | setModels (in move 02091730) centres it on the mpMorf model (r REG + 150) and turns its At on while moving; daBgn3_Create 02091008 sets it up; damage_check reads its hit position |
| 0x12004 | `m0FF28` | `mDekuSph` | setModels (in move 02091730) centres it on mpDekuModel (r REG + 200) and turns its At on while moving; daBgn3_Create 02091008 sets it up |
| 0x12130 | `m10054` | `mCorePos` | setModels (in move 02091730): world position of the part that carries the jyakuten (weak point) model; daBgn3_Delete 02090910 deletes its sound object |
| 0x1213C | `m10060` | `mFogFade` | bgn hensin 02084D6C sets 1, end (in move 02091730) eases it to 1, daBgn3_Execute 02090680 to 0; daBgn_Draw 0207B2CC (inlined daBgn3_Draw) blacks the fog by it (same use as bgn_class mFogFade) |
| 0x122C8 | `m100A0` | `mRopeSway` | damage / end (in move 02091730) ease it up / set the height, setModels eases it to 0 and swings the 60 points of mRedRopeMat by sin * it (same use as bgn2_class mRopeSway) |
| 0x12380 | `m100A8` | `mShakeTimer` | damage_check (in move 02091730) sets 30 / 25 / 20 / 15 / 10 / 7 by the hit; setModels counts it down and shakes the head by it * (REG + 500) |
| 0x12384 | `m100AC` | `mKiSetCount` | move0 (in move 02091730) sets l_HIO.m3C on a wall hit; daBgn3_Execute 02090680 counts it down and spawns one fpcNm_KI_e per count while fewer than 20 exist (same use as bgn2_class mKiSetCount) |
| 0x12388 | `m100B0` | `mHioOwner` | daBgn3_Create 02091008 sets 1 when it created the l_HIO child; daBgn3_Delete 02090910 deletes the HIO child only then (same as bgn_class mHioOwner) |

Open (no name yet): `m017A0` (a model packet without uses), `m100A4` (no uses) and the padding bytes. The
private views use the same names: `bgn_draw::bgn3_class` (d_a_bgn_draw.h) and `bgn2Bgn3View` (d_a_bgn2.h).
`mTimers` / `mTurnTimer` / `mCenterTimer` stay separate fields although daBgn3_Execute counts them down as one
five-halfword sweep (Bgn3TimerView), as in zeldaret/tww's layout.

Body part helper struct part_s3 (offsets within the struct; bgn3_class mParts[10]). The `mPart3` prefix follows the
names zeldaret/tww (CC0) already has for it (`mpPart3Model`, `mPart3TevStr`, `mPart3Sph`, `mPart3ArrowHit*`,
`mPart3Hairs`, `mPart3LineMat`); the uses match bgn part_s:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0004 | `m0004` | `mPart3Packet` | partCtor 02090D4C / partDtor 0209445C construct and destroy the mDoExt_J3DModelPacketS at +4; daBgn_Draw 0207B2CC (inlined daBgn3_Draw, view bgn_draw::part_s3) gives it the part model and updates it while l_HIO.m00C is set (as bgn part_s mPartPacket) |
| 0x027C | `m00C8` | `mPart3FogColorAdd` | daBgn_Draw 0207B2CC (inlined daBgn3_Draw, view bgn_draw::part_s3) passes it to bgn_fog_flash while mPart3ArrowHitFlashTimer runs: eased and added to the fog colour of mPart3TevStr (same use as bgn part_s mPartFogColorAdd) |
| 0x0280 | `m00CC` | `mPart3FogStartAdd` | daBgn_Draw 0207B2CC (inlined daBgn3_Draw, view bgn_draw::part_s3) passes it to bgn_fog_flash while mPart3ArrowHitFlashTimer runs: eased and added to the fog start of mPart3TevStr (same use as bgn part_s mPartFogStartAdd) |
| 0x0288 | `m00D4` | `mPart3Pos` | part_control (in move 02091730) sets [0] to mPartRootPos and chains [1..9] behind it (not below the ground floor); setModels (in move) translates the part matrix to it; splash (in move) tests the ground there; daBgn_Draw 0207B2CC (inlined daBgn3_Draw) lights the part at it (as bgn part_s mPartPos) |
| 0x0294 | `m00E0` | `mPart3AngleX` | part_control (in move 02091730) sets it to minus the chain pitch toward the next part; setModels (in move) rotates the part matrix by it (X) |
| 0x0296 | `m00E2` | `mPart3AngleY` | part_control (in move 02091730) sets it to the chain heading (cM_atan2s) toward the next part; setModels (in move) rotates the part matrix by it (Y) |
| 0x029C | `m00E8` | `mPart3Speed` | part_control (in move 02091730) adds it (and gravity) to the part position, then stores (new - old position) * mPartStiffness in it |
| 0x02A8 | `m00F4` | `mPart3Scale` | part_control (in move 02091730) sets it to size_d[i] * 1.7 and spaces the chain and the ground floor by it; setModels (in move) scales the part matrix, mPart3Sph, the hair lengths and the arrow-hit emitters by it (as bgn part_s mPartScale) |

Open in part_s3: the padding only. The draw view `bgn_draw::part_s3` (d_a_bgn_draw.h) uses the same names. Its
`mPart3Packet` is declared `mDoExt_J3DModelPacketS_l` there but `u8[0xB0]` in d_a_bgn3.h (type left as it is).

### daBgn3_HIO_c (l_HIO; d_a_bgn3.h)

Round 3: the third form's HIO values. In WWHD the vtable sits at the end, so each field is 4 below its GameCube name. Each is set only by hioCtor 020914F4 (default in parentheses) and never written elsewhere in WWHD or tww; the reads are in our d_a_bgn3.cpp (move0, damage_check, part_control and setModels are inlined into move 02091730) and match zeldaret/tww d217e2b. m50 / m52 / m54 are named from tww's reads under the 2026-11-11 rule (WWHD writes them only in the constructor; HD's draw uses the same values as constants).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00001 | `m05` | `mNoMove` | move 02091730: the actions run only while it is 0 (tww the same) (0) |
| 0x00002 | `m06` | `mDebugHealth` | daBgn3_Execute 02090680: when not 0, health is set to it (tww the same) (0) |
| 0x00004 | `m08` | `mSpeedHealth3` | move0 (in move 02091730): the speedF target at health 3 (tww the same) (50) |
| 0x00008 | `m0C` | `mTurnSpeedHealth3` | move0 (in move 02091730): mTurnSpeed at health 3 (tww the same) (800) |
| 0x0000A | `m0E` | `mWallHitsHealth3` | move0 (in move 02091730): at health 3 the chase (mChaseTimer) starts after this many wall hits (tww the same) (3) |
| 0x0000C | `m10` | `mSpeedHealth2` | move0 (in move 02091730): the speedF target at health 2 (tww the same) (60) |
| 0x00010 | `m14` | `mTurnSpeedHealth2` | move0 (in move 02091730): mTurnSpeed at health 2 (tww the same) (1000) |
| 0x00012 | `m16` | `mWallHitsHealth2` | move0 (in move 02091730): the wall hits before the chase at health 2 (tww the same) (3) |
| 0x00014 | `m18` | `mSpeedHealth1` | move0 (in move 02091730): the speedF target at health 1 (tww the same) (80) |
| 0x00018 | `m1C` | `mTurnSpeedHealth1` | move0 (in move 02091730): mTurnSpeed at health 1 (tww the same) (1200) |
| 0x0001A | `m1E` | `mWallHitsHealth1` | move0 (in move 02091730): the wall hits before the chase at health 1 (tww the same) (3) |
| 0x0001C | `m20` | `mChaseTimeHealth3` | move0 (in move 02091730): mChaseTimer after the wall hits at health 3 (tww the same) (60) |
| 0x0001E | `m22` | `mChaseTimeHealth2` | move0 (in move 02091730): mChaseTimer after the wall hits at health 2 (tww the same) (40) |
| 0x00020 | `m24` | `mChaseTimeHealth1` | move0 (in move 02091730): mChaseTimer after the wall hits at health 1 (tww the same) (30) |
| 0x00022 | `m26` | `mOtherHitTime` | damage_check (in move 02091730): mTimers[0] after a hit that is none of the cases below (tww the same) (10) |
| 0x00024 | `m28` | `mArrowHitTime` | damage_check (in move 02091730): mTimers[0] after an arrow hit (tww the same) (20) |
| 0x00026 | `m2A` | `mSwordHitTime` | damage_check (in move 02091730): mTimers[0] after a sword hit (attack type 1) whose AtSpl is not 1 (tww the same) (45) |
| 0x0002A | `m2E` | `mBombHitTime` | damage_check (in move 02091730): mTimers[0] after a bomb hit (attack type 2) (tww the same) (200) |
| 0x0002C | `m30` | `mHammerHitTime` | damage_check (in move 02091730): mTimers[0] after a Skull Hammer hit (attack type 9) (tww the same) (250) |
| 0x00030 | `m34` | `mPartDistanceTarget` | part_control (in move 02091730): mPartDistance eases to it (tww the same) (5.0) |
| 0x00034 | `m38` | `mPartStiffnessTarget` | part_control (in move 02091730): mPartStiffness eases to it (tww the same) (0.5) |
| 0x00038 | `m3C` | `mWallKiCount` | move0 (in move 02091730): a wall hit sets mKiSetCount to it; daBgn3_Execute 02090680 then creates one Ki (0xCD) per frame (tww the same) (3) |
| 0x0003A | `m3E` | `mHairOn` | daBgn3_Draw 0208FC64 / ke_move 02090420 / setModels (in move 02091730): the hair lines are moved and drawn only while it is set (tww the same) (1) |
| 0x00078 | `m40` | `mHairPullY` | setModels (in move 02091730): each hair's mPull is (0, it, 0) in the hair's frame (tww move the same) (150) |
| 0x00040 | `m44` | `mHairLength` | setModels (in move 02091730): ke_move's segment length is 2 * it (the parts: their scale * it) (tww the same) (45) |
| 0x00044 | `m48` | `mHairWidth` | daBgn3_Draw 0208FC64: mDoExt_3DlineMat0_c::update's width for the hair lines (tww the same) (8) |
| 0x00048 | `m4C` | `mHairGravity` | ke_move 02090420: added to each hair point's y (tww ke_control the same) (-8) |
| 0x0004C | `m50` | `mHairColorR` | written in WWHD only by hioCtor 020914F4 (188); HD daBgn3_Draw uses the constant 188; tww daBgn3_Create reads it as ke_color.r |
| 0x0004E | `m52` | `mHairColorG` | written in WWHD only by hioCtor 020914F4 (152); HD daBgn3_Draw uses the constant 152; tww daBgn3_Create reads it as ke_color.g |
| 0x00050 | `m54` | `mHairColorB` | written in WWHD only by hioCtor 020914F4 (66); HD daBgn3_Draw uses the constant 66; tww daBgn3_Create reads it as ke_color.b |

Open (no name yet): `m2C` (damage_check: mTimers[0] after a sword hit whose AtSpl is 1; the meaning of that AtSpl value is not known, tww dCcG_At_Spl_UNK1); padding `_00003`, `_0002E`, `_0003B`, `_00052`.

### bgn3_ke_s (the hair strands mHairs[40] and the parts' hair; d_a_bgn3.h)

Round 3.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00000 | `m00` | `mPos` | ke_move 02090420: the five hair points, each the previous plus a segment toward mPull; copied into the line material's positions; setModels sets the root (tww the same) |
| 0x00000 | `m78` | `mPull` | setModels (in move 02091730): (0, mHairPullY, 0) in the hair's frame minus the root; ke_move 02090420 bends each segment toward it (tww the same) |

Open (no name yet): `m3C` (cXyz[5], read and written nowhere in WWHD or tww).

## bwdg_class (Molgera sand surface; d_a_bwdg.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03D4 | `m002B4` | `mFrameCounter` | daBwdg_Execute 021043EC adds 1 per frame (its only use, in HD and zeldaret/tww) |
| 0x03D8 | `m002B8` | `mWavePhase` | wave_cont 02103FA4 advances it by the bwd_class wave speed (mWaveSpeed) each frame and uses it + the distance term as the cM_ssin phase of every vertex |
| 0x03E0 | `m002C0` | `mWaveHeight` | wave_cont 02103FA4 eases [i] toward the wave / dent height of vertex i (cLib_addCalc2) and sets the vertex y = it + base height |

Open (no name yet): `m00290` and `m002B6` / `m002BA` (padding). The wave terms come from bwd_class
(`mSandWaveMode` .. `mSandBaseY`, named from these uses).

BwdgIndexBuffer (the sand packet's index buffer, BwdgRenderState::mIndices at 0x8E8): two names were swapped.
daBwdg_packet_c::draw 0210508C calls GX2DrawIndexedEx (slot C0006178) with word 4 as mode (r3),
word C as count (r4), word 0 as indexType (r5) and word 8 as indices (r6); bwdg_packet_ct 021045E8 stores 4
(triangles) in word 4 (`mPrimitive`, correct).
The declared types follow since the type fix: word 0 `be<u32>`, word 8 `gptr<void>` (the glue
signature still declares r5 `void*` / r6 `u32`, kept for object identity).

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x0000 | `mpIndices` (misnamed) | `mIndexType` | GX2DrawIndexedEx indexType (r5) in daBwdg_packet_c::draw 0210508C |
| 0x0008 | `mIndexType` (misnamed) | `mpIndices` | GX2DrawIndexedEx indices (r6) in daBwdg_packet_c::draw 0210508C |

## bmd_class (Kalle Demos; d_a_bmd.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03F4 | `m2D8` | `mR00EfBrkFrame` | bmd_kankyo (in daBmd_Execute 020AEBCC) sets 11 * (1 - mColBlend) or 11 * mColBlend; daBmd_Execute sets it as the frame of mpR00_EFBrk |
| 0x03F8 | `m2DC` | `mMakarMode` | mk_move (in daBmd_Execute 020AEBCC) switch: 1 / 2 call loops, 3 / 4 call once, 5 drop, 6 saved, 10 idle; daBmd_Draw 020AE30C draws Makar only while it is not 0; start (in move 020B1DE4) sets 1, demo_camera 020B3BF8 3 / 4 / 5 / 0 |
| 0x03FC | `m2E0` | `mMakarPos` | start_posMk (in move 020B1DE4) puts it 90 in front of the core; mk_move (in daBmd_Execute 020AEBCC) places the Makar model there; mk_voice_set 020AEA40 plays Makar's voice there; demo_camera 020B3BF8 creates fpcNm_NPC_CB1_e there |
| 0x0416 | `m2FA` | `mMakarAngleY` | start (in move 020B1DE4) copies mCoreAngleY; mk_move (in daBmd_Execute 020AEBCC) turns the Makar model by it; daBmd_Draw 020AE30C passes it to dSnap_RegistFig |
| 0x0420 | `m2FE` | `mFrameCounter` | daBmd_Execute 020AEBCC adds 1 per frame; move1 020AE8E8 uses it as the sin / cos phase of the bob and tilt, demo_camera 020B3BF8 of the camera roll |
| 0x0424 | `m302` | `mSubMode` | the step inside mMode: wait / attack_1 / attack_2 / damage / eat / start / end (in move 020B1DE4) switch on it; core_damage_check (in daBmd_Execute 020AEBCC) sets 0 |
| 0x0426 | `m304` | `mFlowerState` | damage (in move 020B1DE4) sets 1 when the flower falls open, 2 when it closes, 3 from closing frame 20 (ride_call_back 020AEA94 then pulls the rider in); wait / start set 0; core_damage_check (in daBmd_Execute 020AEBCC) takes no core hit at 0; daBmd_Execute lifts mBodySph out of reach while it is not 0 |
| 0x0428 | `m306` | `mCoreMode` | core_move (in daBmd_Execute 020AEBCC) switch: 0 wait, 1-2 damage, 100-111 death; core_damage_check sets 1 / 100; daBmd_Draw 020AE30C draws mpHeadMorf below 100 and mpHeadDeadMorf below 110 |
| 0x042A | `m308` | `mTimers` | daBmd_Execute 020AEBCC counts all 4 down; wait / damage / eat (in move 020B1DE4) load [0] and wait on it, [1] / [2] time the turns and attacks, mk_move [3] the Makar calls |
| 0x0432 | `m310` | `mDamageTimer` | core_damage_check (in daBmd_Execute 020AEBCC) takes a core hit only at 0 and sets 8 (4 while the flower is closed, 20000 at death); daBmd_Execute counts it down |
| 0x0434 | `m312` | `mVineCutTimer` | hand_move (in daBmdhand_Execute 020B8D9C) sets 3 when a cut tentacle lowers mVineCount; daBmd_Execute 020AEBCC counts it down; move 020B1DE4 starts damage when it is 1 and mVineCount is 0 |
| 0x0436 | `m314` | `mBgOffTimer` | daBmd_Execute 020AEBCC counts it down and scales the petal / body background matrices to 0 while it runs; ride_call_back 020AEA94 sets 20, damage / eat / end (in move 020B1DE4) 30 / 100 / 30000 |
| 0x0438 | `m318` | `mTargetPos` | wait (in move 020B1DE4) picks a random point near home.pos (kept within 1300 of the player) and walks toward it |
| 0x0444 | `m324` | `mBobHeight` | move1 020AE8E8 eases it to sin(mFrameCounter) * 20; move 020B1DE4 adds it to home.pos.y for the height, 0 in damage |
| 0x0448 | `m328` | `mGroundY` | daBmd_Create 020B1918 sets the start y; damage (in move 020B1DE4) lands on it; core_move (in daBmd_Execute 020AEBCC), cut_control 020B88C4 and ug_move 020B5904 / daBmdfoot_Execute 020B5DC4 keep positions above it |
| 0x0450 | `m330` | `mIsResting` | wait (in move 020B1DE4) sets 1 in step 2 (standing still); daBmd_Execute 020AEBCC clears it each frame; no reader in HD or zeldaret/tww |
| 0x0451 | `m331` | `mVineCount` | hand_calc / hand_move (in daBmdhand_Execute 020B8D9C) add 1 per grown tentacle (max 20) and subtract 1 per cut one; wait (in move 020B1DE4) walks at 0.15 * it; move 020B1DE4 floats 250 + 2.5 * it high and starts damage at 0 |
| 0x0452 | `m332` | `mVineCommand` | move 020B1DE4 clears it each frame; attack_1 / attack_2 / damage / start / end set 1 / 2 / 3, 4 / 5, 6 / 8, 9, demo_camera 020B3BF8 7; hand_move (in daBmdhand_Execute 020B8D9C) and wait / start / move (in daBmdfoot_Execute 020B5DC4) act on the value |
| 0x0454 | `m334` | `mAttackTimer` | attack_2 (in move 020B1DE4) sets 10 and ends the attack at 0; attack_2 (in daBmdfoot_Execute 020B5DC4) sets 10 while the floor tentacle attacks; daBmd_Execute 020AEBCC counts it down |
| 0x0456 | `m336` | `mTurnTargetAngleY` | wait (in move 020B1DE4) sets a random angle (or shape_angle.y) and turns shape_angle.y toward it |
| 0x0458 | `m338` | `mTurnSpeed` | wait (in move 020B1DE4) sets 0 and eases it to 64; the turn step toward mTurnTargetAngleY |
| 0x0A24 | `m904` | `mPushTimer` | damage / eat (in move 020B1DE4) set 20; daBmd_Execute 020AEBCC counts it down and keeps mCoCyl at current.pos while it runs |
| 0x0A28 | `m908` | `mPushRadius` | daBmd_Execute 020AEBCC eases it to 800 while mPushTimer runs (else 0) and sets it as the radius of mCoCyl |
| 0x0A2C | `m90C` | `mCoreSway` | core_move (in daBmd_Execute 020AEBCC) sets [0] / [1] x / z from the sway; core_nodeCallBack 020AE138 rotates the core joints by [0] (below TOSAKA1) or [1]; core_damage_check sets [0].y to the hit direction |
| 0x0A38 | `m918` | `mCoreSwayPhase` | core_move (in daBmd_Execute 020AEBCC) sets 0 on a hit, adds 6000 per frame and uses it as the sin phase of mCoreSway |
| 0x0A3C | `m91C` | `mCoreSwayAmp` | core_move (in daBmd_Execute 020AEBCC) sets 6000 on a hit and eases it to 0; the amplitude of mCoreSway[0]; ends the damage step below 200 |
| 0x0A40 | `m920` | `mCoreSwayAmp2` | core_move (in daBmd_Execute 020AEBCC) sets 9000 on a hit and eases it to 0; the amplitude of mCoreSway[1] |
| 0x0A44 | `m924` | `mCoreDeadPos` | core_move (in daBmd_Execute 020AEBCC) copies current.pos at death, adds mCoreDeadSpeed each frame and places mpHeadDeadMorf there (above mGroundY - 10); demo_camera 020B3BF8 aims at it |
| 0x0A50 | `m930` | `mCoreDeadSpeed` | core_move (in daBmd_Execute 020AEBCC) sets the jump speed at death, applies gravity, then 10 / 0; core_dead_move reverses x / z every 16 frames |
| 0x0A5C | `m93C` | `mCoreAngleY` | core_move (in daBmd_Execute 020AEBCC) turns the head models by it; damage / start_posMk (in move 020B1DE4) set it to the player angle; core_dead_move eases it to 0 |
| 0x0A5E | `m93E` | `mCoreAngleX` | core_move (in daBmd_Execute 020AEBCC) adds 0x500 per frame while the dead core flies and rotates mpHeadDeadMorf by it; core_dead_move eases it to 0 |
| 0x0A60 | `m940` | `mPetalAngle` | daBmd_Execute 020AEBCC rotates the 5 petal background matrices about z by it (above 0 they are switched off); wait / start set -0x4000, damage eases it to 0 and back |
| 0x0A62 | `m942` | `mRideCaught` | ride_call_back 020AEA94 sets 5 while the player rides the closing flower (mFlowerState 3); damage (in move 020B1DE4) clears it before frame 20 and changes to eat while it is not 0 |
| 0x0A64 | `m944` | `mPetalBgMtx` | useHeapInit 020B11E4 gives [i] to pm_bgw[i]; daBmd_Execute 020AEBCC sets each from the body matrix turned by i * 72 degrees and mPetalAngle |
| 0x0B54 | `mA34` | `mBodyBgMtx` | useHeapInit 020B11E4 gives it to pm_bgw[5]; daBmd_Execute 020AEBCC copies the body matrix to it |
| 0x0B9C | `mA7C` | `mCoreEmitters` | eff_cont (in daBmd_Execute 020AEBCC) creates [i] from eff_name (core wait, damage, blood) and moves it with the core joint eff_joint[i] |
| 0x0BA8 | `mA88` + `mA8C` | `mCoreEffTimers` (now `[3]`) | eff_cont (in daBmd_Execute 020AEBCC) loops i < 3 (one per mCoreEmitters entry) and keeps mCoreEmitters[i] alive while [i] runs and counts it down; damage (in move 020B1DE4) sets [0] = 3, core_damage_check [1] = 10, core_move [2] = 3 / 10 while the core dies (blood emitter). GameCube declares [2] apart as `mA8C` (zeldaret/tww d_a_bmd.h:76-77); it is now folded into the array |
| 0x0C90 | `mB70` | `mBodyTexAnmOn` | daBmd_Draw 020AE30C enters mpBrkAnm / mpBtkAnm and daBmd_Execute 020AEBCC plays them only while it is set; demo_camera 020B3BF8 sets 1 |
| 0x0C91 | `mB71` | `mCamTypeMode` | daBmd_Execute 020AEBCC forces camera type Boss02 / Dungeon while it is set, Field once at 2; daBmd_Create 020B1918 sets 1, damage (in move 020B1DE4) 2, end 0 |
| 0x0C92 | `mB72` | `mBlurRate` | daBmd_Draw 020AE30C sets mBlureRate from it (above 1) or clears the blur flag and it (at 1); demo_camera 020B3BF8 / eat / daBmd_Execute set 150 / 50 / 1 |
| 0x0C94 | `mB74` | `mDemoMode` | demo_camera 020B3BF8 switch on it and prints it as "K DEMO MODE"; core_damage_check / eat / start (in daBmd_Execute 020AEBCC, move 020B1DE4) set 100 / 1 / 5 |
| 0x0C96 | `mB76` | `mDemoMainCount` | demo_camera 020B3BF8 counts it up while a demo runs and prints it as "K MAIN COUNT"; start / end / eat (in move 020B1DE4) and end (in daBmdfoot_Execute 020B5DC4) time their steps by it |
| 0x0C98 | `mB78` | `mDemoSubCount` | demo_camera 020B3BF8 counts it up while a demo runs, resets it per demo step and prints it as "K SUB COUNT"; eat (in move 020B1DE4) times the spit by it |
| 0x0C9C | `mB7C` | `mDemoCamEye` | demo_camera 020B3BF8 copies the camera eye into it, moves it per demo step and passes it to cam_Set as the eye |
| 0x0CA8 | `mB88` | `mDemoCamCenter` | demo_camera 020B3BF8 copies the camera center into it, moves it per demo step and passes it to cam_Set as the center |
| 0x0CB4 | `mB94` | `mDemoCamAngleX` | demo_camera 020B3BF8 sets -8000, eases it to -5000 and turns the orbiting demo camera about x by it |
| 0x0CB6 | `mB96` | `mDemoCamAngleY` | demo_camera 020B3BF8 sets it and turns the demo camera (and player) about y by it; eat (in move 020B1DE4) throws the player along it |
| 0x0CBC | `mB9C` | `mDemoCamFovy` | demo_camera 020B3BF8 sets 55 / 60 and eases it per step; passed to cam_Set as the field of view |
| 0x0CC0 | `mBA0` | `mDemoCamSpeed` | demo_camera 020B3BF8 sets 0, eases it up and scales the camera easing steps by it |
| 0x0CC4 | `mBA4` | `mDemoCamDist` | demo_camera 020B3BF8 sets 1500, eases it to 500; the distance of the orbiting demo camera |
| 0x0CC8 | `mBA8` | `mDemoCamShake` | demo_camera 020B3BF8 shakes eye / center and rolls the camera by it and eases it to 0 |
| 0x0CF8 | `mBD8` | `mWindRadiusDelta` | daBmd_Execute 020AEBCC eases it to 0 and sets mWindInfluence radius = 2000 - it (strength on while it is above 1); damage / start / end (in move 020B1DE4) set 2000 |
| 0x0CFC | `mBDC` | `mColBlend` | bmd_kankyo (in daBmd_Execute 020AEBCC) passes it to dKy_custom_colset and eases it to 0; daBmd_Create 020B1918, core_damage_check, damage, start, end set 1 |
| 0x0D00 | `mBE0` | `mKankyoMode` | bmd_kankyo (in daBmd_Execute 020AEBCC) switch: 0 / 1 / 2 pick the dKy_custom_colset palettes; daBmd_Create 020B1918 sets 1, core_damage_check 2, demo_camera 020B3BF8 0 |

Open (no name yet): the padding bytes only. zeldaret/tww (CC0) has no names for these fields; its d_a_bmd source
was read alongside our decomp. The views take the same names: BmdhandBossView (d_a_bmdhand.h; its `m308_0`, the
first of the four timers, is `mTimers0`) and BmdfootBoss (d_a_bmdfoot.h). `mCoreEffTimers` was declared `[2]` plus a
separate `mCoreBloodEffTimer` (GameCube `mA8C`), but eff_cont indexes it 0..2; it is now declared `[3]`.

## bmdhand_class (Kalle Demos ceiling tentacle; d_a_bmdhand.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03D4 | `m2B8` | `mFrameCounter` | daBmdhand_Execute 020B8D9C adds 1 per frame (daBmdhand_Create 020BAD4C starts it random); the sway phase of control1 / cut_control 020B88C4; hand_calc checks the ceiling every 16 frames; hand_move picks the mChainSph segment by it |
| 0x03D6 | `m2BA` | `mAction` | hand_move (in daBmdhand_Execute 020B8D9C) switch on the Bmdhand_* actions (Attached, Cut, BossDown, Intro, Disappear, RegrowWait); daBmdhand_Create 020BAD4C sets Intro |
| 0x03D8 | `m2BC` | `mHandStep` | hand_calc (in daBmdhand_Execute 020B8D9C) switch: 0 reach the ceiling, 1 hold, 2 move the hand to a new place; hand_move lets the tentacle be cut only above 0 |
| 0x03DA | `m2BE` | `mIntroStep` | hand_move (in daBmdhand_Execute 020B8D9C), Intro action: 0 wait for mVineCommand 5, 1 random delay, 2 grow to the ceiling (adds to mVineCount), 3 grown |
| 0x03DC | `m2C0` | `mTimers` | daBmdhand_Execute 020B8D9C counts all 4 down; hand_calc / hand_move load [0] (hand moves, regrow delay l_HIO.mRegrowTime) and [1] (boss-down delay) and wait on them |
| 0x03E4 | `m2C8` | `mCcOffTimer` | hand_move (in daBmdhand_Execute 020B8D9C) sets 20 / 10 / 70 and moves both spheres out of reach (no lock-on) while it runs; daBmdhand_Execute counts it down |
| 0x03E6 | `m2CA` | `mBloodTimer` | hand_move (in daBmdhand_Execute 020B8D9C) sets 2 when the tentacle is cut; cut_control 020B88C4 emits ID_AK_SN_O_BKMTENTACLEBLOOD00 at the cut end while it runs; daBmdhand_Execute counts it down |
| 0x03E8 | `m2CC` | `mCeilingPos` | hand_ceiling_check (in daBmdhand_Execute 020B8D9C): the ceiling hit of a line 2500 up from mHandPos, + l_HIO.mCeilingOffsetY; hand_calc holds the hand at its height; daBmdhand_Execute opens / closes the hand model when it crosses it |
| 0x03F4 | `m2D8` | `mHandPos` | hand_calc (in daBmdhand_Execute 020B8D9C) moves it toward mHandTargetPos; control2 pulls the last chain point to it; the vine-set sound plays there; daBmdhand_Delete 020BABAC deletes its sound object |
| 0x0400 | `m2E4` | `mHandTargetPos` | hand_calc / start_hand_calc (in daBmdhand_Execute 020B8D9C) set it 250 / 350 in front of the root at the ceiling height and move mHandPos toward it |
| 0x040C | `m2F0` | `mHandModelPos` | control_tail (in daBmdhand_Execute 020B8D9C) copies the last chain point; hand_mtx_set 020B8724 places the hand model there; cut_control3 020B8C74 eases it to the ceiling |
| 0x0418 | `m2FC` | `mHandModelPosYOld` | daBmdhand_Execute 020B8D9C keeps mHandModelPos.y of the previous frame to see it cross the ceiling height |
| 0x041C | `m300` | `mHandAngleX` | control_tail (in daBmdhand_Execute 020B8D9C) sets it from the last chain segment; hand_mtx_set 020B8724 turns the hand model about x by it |
| 0x041E | `m302` | `mHandAngleY` | control_tail (in daBmdhand_Execute 020B8D9C) sets it from the last chain segment; hand_mtx_set 020B8724 turns the hand model about y by it |
| 0x0420 | `m304` | `mCloseTimer` | daBmdhand_Execute 020B8D9C sets 3 when the hand rises past the ceiling height, counts it down and closes the hand (FOOK_TOJIRU) at 0 |
| 0x0424 | `m308` | `mHandMoveRate` | hand_calc / start_hand_calc (in daBmdhand_Execute 020B8D9C) set 0 and ease it to 1; the step of mHandPos toward mHandTargetPos is 30 / 50 * it |
| 0x0428 | `m30C` | `mAttachRate` | control2 (in daBmdhand_Execute 020B8D9C) pulls the last chain point to mHandPos at 50 * it and eases it to 1; hand_calc starts holding above 0.9; hand_move sets 0 / 1 / random 0.2 |
| 0x042C | `m310` | `mSegmentLength` | control1 / control2 / cut_control 020B88C4 place the chain points it apart; hand_move (in daBmdhand_Execute 020B8D9C) eases it to 30 / 20, 0 when it disappears; daBmdhand_Draw 020B8650 draws only above 0.1 |
| 0x0430 | `m314` | `mDriftY` | control1 / cut_control 020B88C4 / start_control1 (in daBmdhand_Execute 020B8D9C) add it to every chain segment as y drift and ease it to -10 / -3 / 0; hand_move sets 3 on a cut |
| 0x0434 | `m318` | `mDriftZ` | control1 / cut_control 020B88C4 / start_control1 (in daBmdhand_Execute 020B8D9C) add it to every chain segment as forward drift and ease it to 50 / 30 / 5; hand_move sets 40 on a cut |
| 0x0438 | `m31C` | `mSwayAmp` | control1 / cut_control 020B88C4 multiply the chain sway (sin / cos of mFrameCounter) by it; hand_move (in daBmdhand_Execute 020B8D9C) sets it and eases it to 70 / 200 |
| 0x043C | `m320` | `mHandScale` | hand_mtx_set 020B8724 scales the hand model by it; daBmdhand_Draw 020B8650 draws the hand only above 0.01; hand_move (in daBmdhand_Execute 020B8D9C) eases it to 1 or 0 |
| 0x0440 | `m324` | `mChain` | control1 / control2 / cut_control 020B88C4 (in daBmdhand_Execute 020B8D9C) move the 20 chain points from the root to the hand; hand_move copies them to mLineMat; control3 020B882C sets their widths |
| 0x0834 | `m5CC` | `mEyeSph` | daBmdhand_Create 020BAD4C sets it up (r 110 / 90); hand_move (in daBmdhand_Execute 020B8D9C) centres it on chain point 6 (= eyePos); a hit cuts the tentacle |
| 0x0960 | `m6F8` | `mChainSph` | daBmdhand_Create 020BAD4C sets it up; hand_move (in daBmdhand_Execute 020B8D9C) centres it on chain point 3 + 4 * (mFrameCounter & 3); a hit cuts the tentacle |
| 0x0A8C | `m824` | `mHioOwner` | daBmdhand_Create 020BAD4C sets 1 when it created the l_HIO child; daBmdhand_Delete 020BABAC deletes the HIO child only then (same use as bgn_class mHioOwner) |

Open (no name yet): the padding bytes. The members of `hand_s` are named in round 8 (section "daBmdhand_HIO_c and hand_s" below).

## bmdfoot_class (Kalle Demos floor tentacle; d_a_bmdfoot.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03D4 | `m2B8` | `mFrameCounter` | daBmdfoot_Execute 020B5DC4 adds 1 per frame (daBmdfoot_Create 020B7FC8 starts it random); ug_move 020B5904 starts it random and wobbles the crawl direction by sin(it); attack_1 (in daBmdfoot_Execute) picks the attack sphere joints by it & 7 |
| 0x03D6 | `m2BA` | `mAction` | move (in daBmdfoot_Execute 020B5DC4) switch on the Bmdfoot_* actions (Wait, Attack1, Attack2, Damage, Start, End); daBmdfoot_Create 020B7FC8 sets Start; daBmdfoot_Execute tilts the vine by l_HIO.mActivePitch below 3 |
| 0x03D8 | `m2BC` | `mMode` | the step inside mAction: wait / attack_1 / attack_2 / damage / start (in daBmdfoot_Execute 020B5DC4) switch on it |
| 0x03DA | `m2BE` | `mAimAngle` | attack_1 (in daBmdfoot_Execute 020B5DC4) turns it toward the player (within l_HIO.mAttackAngle) at mAimSpeed + 128; daBmdfoot_Execute 020B5DC4 adds it to the model's y and twice to its z rotation and eases it to 0 |
| 0x03DC | `m2C0` | `mTimers` | daBmdfoot_Execute 020B5DC4 counts all 4 down; wait / attack_1 / attack_2 / damage / start load [0] and [2] and wait on them |
| 0x03E8 | `m2CC` | `mJointRot` | nodeCallBack 020B5580 rotates joint i about x by [i].x and about z by [i].z; daBmdfoot_Execute 020B5DC4 eases [i].z to the sway and [i].x to 0; wait / damage bend [i].x |
| 0x04C0 | `m3A4` | `mJointSwayAmp` | daBmdfoot_Execute 020B5DC4 eases mJointRot[i].z to sin(mSwayPhase + i * 9000) * [i]; wait / damage (in daBmdfoot_Execute) set random amplitudes, attack_1 / start 0 |
| 0x0508 | `m3EC` | `mSwayPhase` | wait / damage (in daBmdfoot_Execute 020B5DC4) set it random; wait adds 0x200 per frame; the phase of the joint sway in daBmdfoot_Execute |
| 0x0510 | `m3F4` | `mWaitEffSet` | wait (in daBmdfoot_Execute 020B5DC4) creates the three ID_AK_SN_BKMASIWAIT00 emitters only while it is 0, then sets 1 |
| 0x0514 | `m3F8` | `mWaitEffPos` | wait (in daBmdfoot_Execute 020B5DC4) sets [i] to the eff_id joint i each frame; the position of mAsiWaitFollowCB[i]'s emitter |
| 0x05B4 | `m498` | `mLAttackSmokePos` | attack_2 (in daBmdfoot_Execute 020B5DC4) sets [0] / [1] and creates the ID_AK_ST_BKMLATTACKSMOKE00 emitters of mLAttackSmoke00CB there |
| 0x0C04 | `mAE8` | `mTipPos` | daBmdfoot_Execute 020B5DC4: position of joint 16 (ASI17), kept above the boss mGroundY; the attack / move sounds play there; daBmdfoot_Delete 020B79C4 deletes its sound object |
| 0x0DDC | `mBA8` | `mUgMode` | ug_move 020B5904 switch: 1 start crawling under the floor, 2 crawl toward the player, 10 surfaced; attack_2 (in daBmdfoot_Execute 020B5DC4) sets 1 / 0; daBmdfoot_Draw 020B56CC draws mpFloorVineMorf from 10 |
| 0x0DE0 | `mBAC` | `mUgPos` | ug_move 020B5904 starts it at current.pos on the boss mGroundY, moves it 20 per frame along mUgAngle and places mpFloorVineMorf there; daBmdfoot_Draw 020B56CC lights mTevstr there; attack_2 sounds play there |
| 0x0DEC | `mBB8` | `mUgAngle` | ug_move 020B5904 sets .y to current.angle.y, turns it toward the player (with a wobble) and moves / turns the floor vine by it |
| 0x0DF4 | `mBC0` | `mUgDistance` | ug_move 020B5904 sets 0 and adds the 20 per frame; the crawl ends past 3000 (or within 300 of the player) |
| 0x0DF8 | `mBC4` | `mAimSpeed` | attack_1 (in daBmdfoot_Execute 020B5DC4) sets 0, eases it to 10000; mAimAngle turns at it + 128 |
| 0x0DFC | `mBC8` | `mDrawOn` | daBmdfoot_Draw 020B56CC draws only while it is set; start (in daBmdfoot_Execute 020B5DC4) and daBmdfoot_Create 020B7FC8 set 1 (the create one when the start demo is skipped) |
| 0x0E04 | `mBD0` | `mBtkTimer` | attack_1 (in daBmdfoot_Execute 020B5DC4) sets 30; daBmdfoot_Execute 020B5DC4 plays ATKVINE_MOVE_2 at 30, counts it down and sets the btk frame to 29 - it |
| 0x0E06 | `mBD2` | `mHioOwner` | daBmdfoot_Create 020B7FC8 sets 1 when it created the l_HIO child; daBmdfoot_Delete 020B79C4 deletes the HIO child only then (same use as bgn_class mHioOwner) |

Open (no name yet): `m2C8` (daBmdfoot_Execute 020B5DC4 counts [0] down; nothing sets it, in HD or zeldaret/tww)
and the padding bytes.

## bpw_class (Jalhalla: body, lantern, fire and curse children; d_a_bpw.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03F0 | `m2D4` | `mAlphaModelMtx` | kantera_draw / damage_ball_draw (in daBPW_Draw 020D1334) copy calc_mtx into it and pass it to dComIfGd_setAlphaModel |
| 0x0420 | `m304` | `mLinChkStart` | action_dousa (in body_execute 020DC704) and action_b_fire_1_dousa (in daBPW_Execute 020D3464) set [0] to current.pos (+400 y in action_dousa) and pass it as the start of linChk.Set; daBPW_Create 020D6268 seeds all 3 with current.pos |
| 0x0444 | `m328` | `mLinChkEnd` | action_dousa (in body_execute 020DC704) and action_b_fire_1_dousa (in daBPW_Execute 020D3464) set [0] to a point ahead of current.pos and pass it as the end of linChk.Set; daBPW_Create 020D6268 seeds all 3 with current.pos |
| 0x0474 | `m358` | `mCoSphCenter` | body_nodeCallBack 020D0C04 puts it m4A0 above the root joint, kantera_nodeCallBack 020D0DF0 100 below the lantern joint; body_execute 020DC704 / kantera_execute (in daBPW_Execute 020D3464) centre mBodyCoSph / mKanteraCoSph on it; the lantern's torch particle and burning sound sit there |
| 0x048C | `m370` | `mAtSphCenter` | body_nodeCallBack 020D0C04 puts it 50 below the root joint; body_execute 020DC704 centres mBodyAtSph on it |
| 0x04A4 | `m388` | `mKanteraSwingX` | kantera_calc 020D30EC eases it to 2000 / 6000 and swings the lantern's mKanteraDousaRot.x by it * sin(m482); action_dousa (in body_execute 020DC704) sets 0 |
| 0x04AC | `m390` | `mKanteraSwingZ` | kantera_calc 020D30EC eases it to 2000 / 6000 and swings the lantern's mKanteraDousaRot.z by it * cos(m482); action_dousa (in body_execute 020DC704) sets 0 |
| 0x04B0 | `m394` | `mWaistPos` | body_nodeCallBack 020D0C04 stores the J_BPW_KOSI1 (waist) joint position; action_kougeki (in body_execute 020DC704) emits the inhale / blow particles there |
| 0x04BC | `m3A0` | `mFlameBallPos` | action_b_fire_1_dousa (in daBPW_Execute 020D3464) starts it at current.pos and eases it along the flame line (mLinChkStart / mLinChkEnd); damage_ball_execute (same) centres mDamageBallCoSph on it |
| 0x04C8 | `m3AC` | `mModelOffset` | draw_SUB 020D0FEC translates the body model by current.pos + it, kantera_draw_SUB 020D1158 the lantern model by it; next_status_clear 020D257C, action_damage 020D72BC, action_bunri_dousa 020D8564 and action_start_demo 020DB380 set or ease its y |
| 0x04D4 | `m3B8` | `mRightArmPos` | body_nodeCallBack 020D0C04 stores the J_BPW_UDE_R3 joint position; action_start_demo 020DB380 copies it to mFire1DousaPos for the opening smoke |
| 0x04E0 | `m3C4` | `mJawPos` | body_nodeCallBack 020D0C04 stores the J_BPW_AGO1 (jaw) joint position; action_kougeki (in body_execute 020DC704) plays the breath-in / breath-out / fire-blast sounds there; daBPW_Delete 020D57A8 deletes its sound object |
| 0x04F2 | `m3D6` | `mRollRot` | draw_SUB 020D0FEC rotates the body by Y(y) X(x) Y(-y); action_damage 020D72BC sets y to the player's angle and adds 5000 to x per frame while thrown (debug REG8_S(3) path), 0 on landing |
| 0x04FC | `m3E0` | `mIsLightFrozen` | action_damage 020D72BC sets 1 when the light freezes the body; while it is set daBPW_Draw 020D1334 uses mpLightFreezeBrkAnm, body_execute 020DC704 plays it, body_atari_check 020D6D98 and alpha_anime 020D1C40 skip; next_status_clear 020D257C sets 0 |
| 0x04FD | `m3E1` | `mIsLightStunned` | action_damage 020D72BC sets 1 (light hit before the freeze) and 0; while it is set daBPW_Draw 020D1334 and body_execute 020DC704 use mpLightStunBrkAnm; next_status_clear 020D257C clears it |
| 0x04FE | `m3E2` | `mCurseBrkState` | noroi_brk_check 020D2350 sets 2 (curse start), 1 (curse end), 0; daBPW_Draw 020D1334 and body_execute 020DC704 pick mpCurseStartBrkAnm at 2, mpCurseEndBrkAnm at 1 |
| 0x0500 | `m3E4` | `mReturnToParent` | action_kantera_dousa (in daBPW_Execute 020D3464): while it is set the lantern goes to state 5 and slides toward its parent (mParentId), which clears it; no setter in HD or zeldaret/tww |
| 0x0501 | `m3E5` | `mIsThrown` | action_damage 020D72BC sets 1 when the carried body is thrown and 0 when it hits a wall; body_execute 020DC704 turns toward mTargetAngleY only while it is 0; next_status_clear 020D257C clears it |
| 0x0502 | `m3E6` | `mIsCurseDash` | action_dousa (in body_execute 020DC704) sets 1 when the curse dash starts; noroi_check 020D2058 drops the body on a nearby player only while it is 0; next_status_clear 020D257C clears it |
| 0x0503 | `m3E7` | `mKanteraHidden` | daBPW_Create 020D6268 sets 1 for the lantern; while it is 0 daBPW_Draw 020D1334 draws the lantern glow and daBPW_Execute 020D3464 plays it and lights the torch particle; action_start_demo 020DB380 clears it on the lantern when it appears |
| 0x0504 | `m3E8` | `mVibMode` | action_damage 020D72BC and action_karada_taore (in body_execute 020DC704) set 1 / 2; vib_mode_check (in daBPW_Execute 020D3464) starts the matching shock or quake and sets 0 |
| 0x0505 | `m3E9` | `mIsReformed` | action_bunri_dousa 020D8564 sets 1 when the regrowing body reaches full scale; action_kougeki (in body_execute 020DC704) then goes to state 0x32 and clears it; Big_pow_gattai_check 0244A5A8 (d_a_pw) merges a Poe only while it is set |
| 0x0506 | `m3EA` | `mStartDemoStep` | action_start_demo 020DB380 sets 1..6 as the opening goes on; the child Poes (d_a_pw, 024500C0) follow each step |
| 0x0508 | `m3EC` | `mKankyouBlend` | kankyou_hendou 020D1DA8 eases it to 0 / 1 and uses it as the dKy_custom_colset blend; daBPW_Create 020D6268 sets 1, action_bunri_dousa 020D8564 0 |
| 0x050C | `m3F0` | `mDemoCamDist` | action_bunri_dousa 020D8564 sets it and eases it to 600; the demo camera eye is mDemoCamDist * sin / cos(m3F4) |
| 0x0510 | `m3F4` | `mDemoCamAngleY` | action_bunri_dousa 020D8564 sets it and steps it toward 0; the demo camera eye is m3F0 * sin / cos(it) |
| 0x0512 | `m3F6` | `mBattlePhase` | action_bunri_dousa 020D8564 sets 1 / 2 as health drops to 10 / 5 (and 3 / 4 in the defeat demo) and ends each separation at the matching health; action_dousa (in body_execute 020DC704) takes the turn speed from it; light_on_off 020D271C and noroi_check 020D2058 read it; action_start_demo 020DB380 sets 0 |
| 0x0514 | `m3F8` | `mBckFileIdx` | anm_init 020D1A64 stores its bckFileIdx argument; action_dousa / action_kougeki / action_karada_taore (in body_execute 020DC704), action_damage 020D72BC, action_bunri_dousa 020D8564 and action_start_demo 020DB380 compare it with BCK indices |
| 0x0518 | `m3FC` | `mKanteraId` | daBPW_Create 020D6268 stores the id of the lantern child it creates; kantera_calc 020D30EC, kantera_pos_search 020D2ED8, action_bunri_dousa 020D8564 and action_start_demo 020DB380 search it |
| 0x051C | `m400` | `mFireId` | action_kougeki (in body_execute 020DC704) stores the id of the fire actor it creates at the lantern; fire_and_emitter_clear 020D22B8 deletes it and sets -1 |
| 0x0520 | `m404` | `mParentId` | daBPW_Create 020D6268 (kantera_create_init) stores parentActorID; action_kantera_dousa (in daBPW_Execute 020D3464) turns the lantern toward that actor |
| 0x0524 | `m408` | `mBlureRate` | daBPW_Draw 020D1334 passes it to the blur (setBlureRate) while above 1 and clears it at 1; action_damage 020D72BC (wall hit) sets 0xB5; daBPW_Execute 020D3464 sets 0x32 / steps it down by 2 |
| 0x0528 | `m40C` | `mDemoCamCenter` | action_bunri_dousa 020D8564 and action_start_demo 020DB380 set and ease it and pass it as the center to dCamera_c::Set / Reset |
| 0x0534 | `m418` | `mDemoCamEye` | action_bunri_dousa 020D8564 and action_start_demo 020DB380 set and ease it and pass it as the eye to dCamera_c::Set / Reset |
| 0x0540 | `m424` | `mChildPoePos` | action_bunri_dousa 020D8564 copies the watched child Poe's current.pos; the demo camera center and the player's placement follow it |
| 0x0558 | `m43C` | `mDemoCamBlend` | action_bunri_dousa 020D8564 and action_start_demo 020DB380 set 0 at each camera cut and ease it to 1; the demo camera steps (demo_calc) are scaled by it |
| 0x055C | `m440` | `mDemoCamFovy` | action_bunri_dousa 020D8564 and action_start_demo 020DB380 set and ease it and pass it as the fovy to dCamera_c::Set |
| 0x057E | `m462` | `mPoeCount` | body: action_big_demo 024500C0 (d_a_pw, view PwJalhallaView) adds 1 for each Poe that merges back; action_bunri_dousa 020D8564 ends the separation when it reaches health and reuses it as the mChildPoeIds index of the Poe the camera follows; action_dousa (in body_execute 020DC704) sets 1, action_bunri_dousa 0. Other roles of the same word: the lantern's landing step in action_kantera_dousa and the flame's sweep phase in action_b_fire_1_dousa (both in daBPW_Execute 020D3464) |
| 0x0580 | `m464` | `mPoeMerged` | body: action_big_demo 024500C0 (d_a_pw, view PwJalhallaView) sets 1 when a Poe merges back; action_bunri_dousa 020D8564 and action_start_demo 020DB380 then play BCK_CORE_NIGE1 and clear it; action_bunri_dousa also keeps the child Poe's angle to the player there for the camera. Other roles of the same word: the lantern's spin in action_kantera_dousa and the flame's sweep direction in action_b_fire_1_dousa (both in daBPW_Execute 020D3464) |
| 0x0590 | `m474` | `mDemoCamActive` | action_bunri_dousa 020D8564 sets 1 when a demo starts and sets the camera from mDemoCamCenter / mDemoCamEye / mDemoCamFovy each frame while it is set |
| 0x0592 | `m476` | `mTargetAngleY` | action_dousa / action_kougeki / action_karada_taore (in body_execute 020DC704), action_damage 020D72BC, action_bunri_dousa 020D8564 and kantera_pos_search 020D2ED8 set it (mostly the angle to the player); body_execute turns current.angle.y toward it by mTurnStep |
| 0x0594 | `m478` | `mAlphaWavePhase` | alpha_anime 020D1C40 adds 0x400 per frame and sets mAlpha to mBaseAlpha + sin(it) * 30; next_status_clear 020D257C and action_damage 020D72BC set 0 |
| 0x0596 | `m47A` | `mTurnStep` | body_execute 020DC704 turns current.angle.y toward mTargetAngleY by it; action_dousa (same) takes it from {0x100, 0x300, 0x500}[mBattlePhase]; action_kougeki sets 0x500 / 0x300 / 140, action_damage 020D72BC 0 |
| 0x0598 | `m47C` | `mFloatPhase` | fuwafuwa_calc 020D1D3C and torituki_execute (in daBPW_Execute 020D3464) add 700 per frame and bob current.pos.y by sin(it) * 30; action_bunri_dousa 020D8564 sets 0 and swings mModelOffset by it |
| 0x059A | `m47E` | `mAlpha` | daBPW_Draw 020D1334 writes it to the materials' K color 3 alpha and the alpha models; alpha_anime 020D1C40, action_dousa (in body_execute 020DC704), action_damage 020D72BC and action_bunri_dousa 020D8564 fade it; body_atari_check 020D6D98 takes no hit at 0 |
| 0x059C | `m480` | `mBaseAlpha` | alpha_anime 020D1C40 eases it to 0x96 and sets mAlpha to it + sin(mAlphaWavePhase) * 30; action_bunri_dousa 020D8564 eases mAlpha toward it; action_dousa / action_karada_taore (in body_execute 020DC704) set 0x96 / 200 |
| 0x059E | `m482` | `mKanteraSwingPhase` | kantera_calc 020D30EC adds mKanteraSwingSpeed per frame and swings the lantern by sin / cos(it); action_dousa (in body_execute 020DC704) sets 0 |
| 0x05A0 | `m484` | `mKanteraSwingSpeed` | kantera_calc 020D30EC eases it to 3000 / 6000 and adds it to mKanteraSwingPhase |
| 0x05A4 | `m488` | `mSpeedStep` | action_dousa (in body_execute 020DC704) sets 1 / 5 / 2 and eases speedF toward mTargetSpeed by it |
| 0x05A8 | `m48C` | `mBgOffsetY` | BG_check 020D1B90 lowers current / old pos.y by it before the ground check and raises them after; action_dousa (in body_execute 020DC704) eases it to 0, action_damage 020D72BC / action_bunri_dousa 020D8564 to -120, action_kantera_dousa (in daBPW_Execute 020D3464) sets 60 |
| 0x05AC | `m490` | `mTargetSpeed` | action_dousa (in body_execute 020DC704) eases speedF toward it by mSpeedStep and sets 10 / * 1.35 / 0; action_damage 020D72BC sets 120 / 20 / 5; next_status_clear 020D257C 0 |
| 0x05B0 | `m494` | `mDamageBallRadius` | action_b_fire_1_dousa / action_b_fire_2_dousa (in daBPW_Execute 020D3464) set 50 / 30 / 35; damage_ball_execute (same) passes it to mDamageBallCoSph.SetR |
| 0x05B4 | `m498` | `mWallHalfHeight` | BG_check 020D1B90 passes it as the half height to mAcchCir.SetWall; next_status_clear 020D257C sets 350, daBPW_Execute 020D3464 50 / 0, action_bunri_dousa 020D8564 0 |
| 0x05B8 | `m49C` | `mWallRadius` | BG_check 020D1B90 passes it as the radius to mAcchCir.SetWall; next_status_clear 020D257C sets 400, action_damage 020D72BC 300, action_bunri_dousa 020D8564 160, daBPW_Execute 020D3464 200 / 40 |
| 0x05BC | `m4A0` | `mCoSphOffsetY` | body_nodeCallBack 020D0C04 puts mCoSphCenter this far above the root joint; action_damage 020D72BC sets 50, action_dousa / action_kougeki (in body_execute 020DC704) 0 |
| 0x05C0 | `m4A4` | `mEyeHeight` | body_execute 020DC704 sets eyePos.y to current.pos.y + it and the attention point at + 2 * it + 100; action_karada_taore (same) sets 100, action_damage 020D72BC / next_status_clear 020D257C 300 |
| 0x05C4 | `m4A8` | `mBodyAtSphR` | body_execute 020DC704 passes it to mBodyAtSph.SetR; action_karada_taore (same) sets 500 / 220 / 400, next_status_clear 020D257C and daBPW_Create 020D6268 220 |
| 0x0628 | `m50C` | `mBlowSmokeCb` | action_kougeki (in body_execute 020DC704) passes it to the BPWBLOWSMOKE00 particle; fire_and_emitter_clear 020D22B8 ends it |
| 0x0648 | `m52C` | `mBlowCb` | action_kougeki (in body_execute 020DC704) passes it to the BPWBLOW00 particle; fire_and_emitter_clear 020D22B8 ends it |
| 0x065C | `m540` | `mInhaleCb` | action_kougeki (in body_execute 020DC704) passes it to the BPWINHALE01 particle and ends it; fire_and_emitter_clear 020D22B8 ends it |
| 0x0670 | `m554` | `mOpeningSmokeCb` | action_start_demo 020DB380 passes it to the BPWOPENINGSMOKE00 particle (0x845A) and ends it |
| 0x06AC | `m590` | `mFlameHitCb` | action_b_fire_1_dousa (in daBPW_Execute 020D3464) passes it to the BPWFLAMETHROWER02 particle at mFlameHitPos and ends it when the line misses; fire_and_emitter_clear 020D22B8 ends it |
| 0x06E8 | `m5CC` | `mTorchCb` | daBPW_Execute 020D3464 passes it to the lantern's ID_AK_JN_TORCH particle at mCoSphCenter; daBPW_Delete 020D57A8 ends it |
| 0x06FC | `m5E0` | `mSmokeCb` | action_damage 020D72BC uses [0] (BPWSIRIMOTISMOKE00), action_karada_taore (in body_execute 020DC704) [1] (BPWJUMPSMOKE00) and [2] / [3] (BPWPRESSSMOKE00 / 01); daBPW_Delete 020D57A8 ends all 4 |
| 0x0788 | `m66C` | `mFlameHitPos` | action_b_fire_1_dousa (in daBPW_Execute 020D3464) stores the line-check cross point and emits BPWFLAMETHROWER02 there |
| 0x079A | `m67E` | `mFlameHitRotX` | action_b_fire_1_dousa (in daBPW_Execute 020D3464) sets 0 on a floor, 0x4000 on a wall, and passes it as the start of the particle's csXyz angle |
| 0x079C | `m680` | `mFlameHitRotY` | action_b_fire_1_dousa (in daBPW_Execute 020D3464) sets the wall angle (atan2 from mFlameHitPos to the flame start); the y of the particle angle that starts at mFlameHitRotX |
| 0x07A0 | `m684` | `mCarryAngleY` | action_damage 020D72BC stores shape_angle.y when the body can be picked up and, while carried, adds the player's turn and sets shape_angle.y from it |

The class body was converted to `/* 0xNNN */ type name;` with one declarator per line (the `m3E4..m3EA`,
`m474..m484` and `m488..m4A8` lines were split) before the renames.

Open (no name yet): `m38C` (action_dousa (in body_execute 020DC704) sets 0; nothing reads it, in HD or
zeldaret/tww) and the padding bytes.

The Poes' view of this class, PwJalhallaView (d_a_pw.h), uses the same names: `mIsReformed`,
`mStartDemoStep`, `mBattlePhase`, `mActionState`, `mSomeCountdownTimers0` (element [0]), `mPoeCount`,
`mPoeMerged`. `mPoeCount` / `mPoeMerged` were the view's names and go to the class (the pw uses are the
strongest evidence); the view's `mMergeState`, `mAction` and `mMode` follow the class, and `mKnockdown` /
`mMergeAllowed` were misnamed (0x6F is ACTION_STATE_SEPARATE_BUNRI_DOUSA_EXECUTE in zeldaret/tww; 0x56A is
mSomeCountdownTimers[0]).

## fganon_class (Phantom Ganon; d_a_fganon.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03D8 | `m2BC` | `mType` | daFganon_Create 02137F9C: parameter bits 0-3 (zeldaret/tww names the HIO child Phantom Ganon 1 / 2 / 3 for 0 / 1 / 2; 3 = a spin-attack clone, which reads mCloneNo); down 02139680 / standby, shot, move (in move 0213B3AC) branch on it (camera type, light-arrow checks) |
| 0x03D9 | `m2BD` | `mStartDist` | daFganon_Create 02137F9C: parameter byte 1; start 02139AA0 and standby (in move 0213B3AC) begin when the player's XZ distance to home is below it * 10 |
| 0x03DB | `m2BF` | `mBattleSwitchNo` | daFganon_Create 02137F9C: parameter byte 3; demo_camera (in daFganon_Execute 02134B28) turns this switch on when the opening demo hands over to the battle and off at the end of the defeat demo (0xFF = none) |
| 0x03EC | `m2D0` | `mKenState` | last_end 0213A228 sets 1 (sword no longer follows the hand) and 2 (sword handed to the Boko actor); execute_model (in daFganon_Execute 02134B28) places mpKenModel only at 0, daFganon_Draw 02134428 draws it unless 2 |
| 0x05B8 | `m384` | `mFrameCounter` | execute_model (in daFganon_Execute 02134B28) adds 1 per frame and uses it as the phase of the sway and hit-shake sines; fly_common (in move 0213B3AC) samples every 16th frame |
| 0x05C0 | `m38C` | `mTargetPos` | fly_common (in move 0213B3AC): player position + offset; spinattack2 02139194: player position; pos_move 02134840 turns current.angle toward it |
| 0x05CC | `m398` | `mTargetAngleY` | fly_common (in move 0213B3AC): the player's shape_angle.y, re-sampled at random every 16 frames; YrotS by it to place mTargetPos around the player |
| 0x05D0 | `m39C` | `mTurnSpeed` | fly_common (in move 0213B3AC) sets 1500 (+ reg); pos_move 02134840: angle step = it * mTurnSpeedRate |
| 0x05D4 | `m3A0` | `mTurnSpeedRate` | fly_common (in move 0213B3AC) clears it, pos_move 02134840 eases it to 1 and multiplies mTurnSpeed by it |
| 0x05D8 | `m3A4` | `mTimers` | daFganon_Execute 02134B28 counts all 5 down each frame; the actions load them (start 02139AA0 [0] = 0x50, down 02139680 [0] = l_HIO.m48, spinattack2 02139194 [0] = 40) |
| 0x05E2 | `m3AE` | `mNoDamageTimer` | damage_check 0213A6FC returns while it is not 0 and sets 6 / 2 / 1000 after a hit; the demo and vanish actions (start 02139AA0, down 02139680, deru in move 0213B3AC) keep it at 3 / 5; execute_model (in daFganon_Execute 02134B28) counts it down |
| 0x05E4 | `m3B0` | `mKnockbackDist` | knockDown (in damage_check 0213A6FC) sets 80 (+ reg); move 0213B3AC moves the actor by it along mKnockbackAngleY / X and eases it to 0 |
| 0x05E8 | `m3B4` | `mKnockbackAngleY` | knockDown (in damage_check 0213A6FC): atan2 from the player to the actor; move 0213B3AC YrotS by it for the knockback |
| 0x05EA | `m3B6` | `mKnockbackAngleX` | knockDown (in damage_check 0213A6FC): -atan2 of the height difference over the XZ distance; move 0213B3AC XrotM by it for the knockback |
| 0x05EC | `m3B8` | `mHitShakeTimer` | knockDown (in damage_check 0213A6FC) sets 7 (+ reg); execute_model (in daFganon_Execute 02134B28) counts it down and shakes the model's Y / X rotation by it * 500 |
| 0x05F0 | `m3BC` | `mSwayAmplitude` | fly_common (in move 0213B3AC) eases it to 50; execute_model (in daFganon_Execute 02134B28) offsets the model by it * sin / cos of mFrameCounter and eases it to 0 |
| 0x05F4 | `m3C0` | `mFadeFogEndZ` | last_end 0213A228: sets 10000 / 5100, eases it to 100 and copies it to tevStr.mFogEndZ (the white fade-out) |
| 0x0614 | `m3E0` | `mBallPos` | energy_ball_move (in daFganon_Execute 02134B28): the energy ball's position (hand joint, then + mBallVelocity); ball_bg_check line-checks from it; shot (in move 0213B3AC) plays the ball sound there |
| 0x0620 | `m3EC` | `mBallPosOld` | energy_ball_move (in daFganon_Execute 02134B28) copies mBallPos before moving it; ball_bg_check checks the line from it |
| 0x062C | `m3F8` | `mBallVelocity` | aimEnergyBall (in daFganon_Execute 02134B28): MtxPosition of (0, 0, mBallSpeed) toward the target; energy_ball_move adds it to mBallPos and takes the effect angle from it |
| 0x0638 | `m404` | `mBallSpeed` | aimEnergyBall (in daFganon_Execute 02134B28): l_HIO.m1C / m20 when thrown, kept on each return; energy_ball_move adds l_HIO.m24 / m28 per return; shot (in move 0213B3AC) scales the swing distance by it |
| 0x063C | `m408` | `mBallState` | energy_ball_move (in daFganon_Execute 02134B28): 0 none, 1 create the ball effects, 2 held in the hand, 3 thrown, 4 flying at the player, 5 sent back at Phantom Ganon, 35 remove; shot (in move 0213B3AC) / start 02139AA0 set 1, damage_check 0213A6FC / kieru set 35 |
| 0x063D | `m409` | `mBallThrow` | shot (in move 0213B3AC) sets 1 at the throw frame; energy_ball_move (in daFganon_Execute 02134B28) then aims the ball, sets mBallState 3 and clears it |
| 0x063E | `m40A` | `mBallNoHitTimer` | energy_ball_move (in daFganon_Execute 02134B28) sets 5 after each return (50 when the third form vanishes) and counts it down; while it runs the ball's colliders are moved away and hits are ignored |
| 0x0898 | `m664` | `mDarkBallPos` | tama_set (in shot2 02138670): the dark ball's position in front of the actor (mDarkBallDist); shot2 creates the 8 fgmahou balls there |
| 0x08A4 | `m670` | `mMahouHitCount` | shot2 02138670 clears it; damage_check 0213A6FC adds 1 per hit by an fgmahou ball and knocks Phantom Ganon down at 5 |
| 0x08A5 | `m671` | `mDarkBallState` | shot2 02138670 sets 1 / 3; tama_set (in shot2) 1: create the effects, 3: burst, 4: fade (mDarkBallAlpha), 5..9 then 0; daFganon_Draw 02134428 draws the dark ball model while it is not 0 |
| 0x08A6 | `m672` | `mDarkBallAlpha` | tama_set (in shot2 02138670): 250 when the ball appears, -50 per frame after the burst; the emitters' global alpha; 0 removes them |
| 0x08B4 | `m680` | `mDarkBallDist` | shot2 02138670 clears it and eases it to 250 at the throw; tama_set (in shot2) places mDarkBallPos it forward and it down |
| 0x08B8 | `m684` | `mWeponAtStarted` | execute_collision (in daFganon_Execute 02134B28): set 1 after StartCAt of mWeponSph, MoveCAt while set; cleared when the sword attack is off |
| 0x08B9 | `m685` | `mWeponAtOn` | spinattack2 02139194 / spinattack (in move 0213B3AC) set 1 each attack frame; execute_collision (in daFganon_Execute 02134B28) places mWeponSph on the sword while set and clears it |
| 0x08BA | `m686` | `mBallSwing` | shot (in move 0213B3AC) sets 1 on swing frames 5..15; energy_ball_move (in daFganon_Execute 02134B28) sends a near ball back at the player while it is set; execute_collision clears it each frame |
| 0x08BB | `m687` | `mSwingCount` | shot (in move 0213B3AC): cleared at the throw, +1 per swing at the ball; from 4 swings it may stop returning (mStopReturn) |
| 0x08BC | `m688` | `mRallyCount` | shot (in move 0213B3AC) clears it; energy_ball_move (in daFganon_Execute 02134B28) adds 1 per return by the player and passes it + 2 (max 7) to mDoAud_changeSubBgmStatus; shot stops returning at 7 |
| 0x08BD | `m689` | `mStopReturn` | shot (in move 0213B3AC): cleared at the throw, set when mRallyCount reaches 7 or at random after 4 swings; while set the ball is no longer swung back |
| 0x08BE | `m68A` | `mLaughTimer` | execute_model (in daFganon_Execute 02134B28) sets 60 with the WARAU1 laugh and counts it down; shot2 02138670 / fly_common (in move 0213B3AC) return to WAIT1 at 1 |
| 0x08BF | `m68B` | `mLaughRequest` | energy_ball_move (in daFganon_Execute 02134B28) sets 1 when the ball hits the player; d_a_fgmahou (view FgmahouBossHD) sets 1 when a dark ball hits; execute_model starts the laugh and clears it |
| 0x08C0 | `m68C` | `mBallHitSelf` | energy_ball_move (in daFganon_Execute 02134B28) sets 1 when a returned ball reaches Phantom Ganon; damage_check 0213A6FC takes it as a hit; move 0213B3AC clears it |
| 0x08C2 | `m68E` | `mCapeOff` | last_end 0213A228 sets 1; execute_collision (in daFganon_Execute 02134B28) eases the cape's scale.y to 0 while it is set |
| 0x08C3 | `m68F` | `mCloneNo` | daFganon_Create 02137F9C: parameter bits 4-7 when mType is 3 (spinattack2 02139194 creates clones 1..4 with (i * 16) | 3); 0 = the real Phantom Ganon; spinattack2 / damage_check 0213A6FC / kieru branch on it |
| 0x08C4 | `m690` | `mDownHealth` | damage_check 0213A6FC: sword damage is taken off it while Phantom Ganon is down; at 0 it is reset to 10 and he vanishes |
| 0x08C8 | `m694` | `mSwordFallAngle` | fail (in move 0213B3AC): + mSwordFallSpeed per frame up to 15884 (sword lying flat); execute_model (in daFganon_Execute 02134B28) XrotM of the sword by it |
| 0x08CC | `m698` | `mSwordFallSpeed` | fail (in move 0213B3AC): + mSwordFallAccel per frame; on reaching the ground it bounces back (* -0.4) while above 700 |
| 0x08D0 | `m69C` | `mSwordFallAccel` | fail (in move 0213B3AC): + 3 per frame up to a limit; added to mSwordFallSpeed and to the tilt step |
| 0x08D4 | `m6A0` | `mSwordFallDir` | fail (in move 0213B3AC): angle from the sword to the wall line cross (+ 0x7058); execute_model (in daFganon_Execute 02134B28) ZrotM by it and its negative around the fall |
| 0x08D6 | `m6A2` | `mSwordTilt` | fail (in move 0213B3AC) eases it toward mSwordFallDir by mSwordFallAccel + mSwordTiltStep; execute_model (in daFganon_Execute 02134B28) ZrotM of the sword by it |
| 0x08D8 | `m6A4` | `mSwordTiltStep` | fail (in move 0213B3AC): 0x80, 0x800 once the sword lies flat; added to the cLib_addCalcAngleS2 step of mSwordTilt |
| 0x08DA | `m6A6` | `mSwordShakeTimer` | fail (in move 0213B3AC) sets 30 (+ reg) on a bounce; execute_model (in daFganon_Execute 02134B28) shakes the sword by it * sin and counts it down |
| 0x08DC | `m6A8` | `mSwordFallSeVolume` | fail (in move 0213B3AC): 100, passed to the sword-fall sound 0x6998 on each bounce, -20 per bounce down to 30 |
| 0x08E0 | `m6AC` | `mBtAttackTimer` | spinattack2 02139194 / spinattack (in move 0213B3AC) set l_HIO.m40 - m3E; execute_collision (in daFganon_Execute 02134B28) counts it down and sets the bt attack data (frames 0..10) while it runs |
| 0x0D88 | `mB54` | `mDemoCamMode` | start 02139AA0 sets 1, end 02139E80 50, last_end 0213A228 100 (+1 per step); demo_camera (in daFganon_Execute 02134B28) switches on it; cameraReset clears it |
| 0x0D8A | `mB56` | `mDemoTimer` | demo_camera (in daFganon_Execute 02134B28) clears it per step, adds 1 per frame and compares it with frame marks (2, 30, 45, 110, 280); last_end 0213A228 clears it |
| 0x0D90 | `mB5C` | `mDemoCamEye` | demo_camera (in daFganon_Execute 02134B28): set to fixed points / eased; passed to dCamera_c::Set as the eye |
| 0x0D9C | `mB68` | `mDemoCamCenter` | demo_camera (in daFganon_Execute 02134B28): set to fixed points, the actor's position or eyePos; passed to dCamera_c::Set as the centre |
| 0x0DAA | `mB76` | `mDemoCamAngleY` | demo_camera (in daFganon_Execute 02134B28): + 30 per frame, added to the player's shape_angle.y to orbit mDemoCamEye around the player |
| 0x0DB4 | `mB80` | `mDemoCamSpeed` | demo_camera (in daFganon_Execute 02134B28): cleared per step, eased to 0.1 / 1; scales the cLib_addCalc2 steps of mDemoCamEye / mDemoCamCenter |
| 0x0DB8 | `mB84` | `mDemoCamFovy` | demo_camera (in daFganon_Execute 02134B28): 55 or 60; passed to dCamera_c::Set as the fovy |
| 0x0DBC | `mB88` | `mHioOwner` | daFganon_Create 02137F9C sets 1 when it created the l_HIO child; daFganon_Delete 02137810 deletes the HIO child only then |
| 0x0DBD | `mB89` | `mBgmStartTimer` | standby (in move 0213B3AC) sets 22; execute_model (in daFganon_Execute 02134B28) counts it down and starts sub BGM 0x80000047 at 1; daFganon_Delete 02137810 stops the sub BGM when it is not 0 |
| 0x0DBE | `mB8A` | `mResLoadState` | daFganon_Create 02137F9C: 1 after requesting "Pgsw", 2 after requesting "Fganon"; daFganon_Delete 02137810 releases Pgsw from 1 and Fganon too from 2 |

Open (no name yet): `m38A`, `m39A`, `m3DC`, `m40B`, `m673`, `mB74`, `mB8B` (no uses in HD or zeldaret/tww) and
the padding bytes `m2D1`, `m3BA`, `m691`, `m6AD`, `mB58`, `mB78`. zeldaret/tww (CC0) has offset names only for
these fields; its d_a_fganon.cpp was read alongside our decomp to follow the uses. The dark ball's view of this
class, `FgmahouBossHD` (d_a_fgmahou.h), uses the same name (`mLaughRequest`). The cape position view
`FganonCape` is a view of mant_class and keeps mant's names. The HIO block `daFganon_HIO_c` is named in round 3 (below).

### daFganon_HIO_c (l_HIO; d_a_fganon.h)

Round 3: Phantom Ganon's HIO values. In WWHD the vtable sits at the end, so each field is 4 below its GameCube name. Each is set only by hioCtor 0213846C (default in parentheses) and never written elsewhere in WWHD or tww; the reads are in our d_a_fganon.cpp (deru, standby, fly_common, spinattack and kabe_check are inlined into move 0213B3AC; execute_model, aimEnergyBall, energy_ball_move and execute_collision into daFganon_Execute 02134B28) and match zeldaret/tww d217e2b (fly_common is tww's fly and fly2).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00001 | `m05` | `mNoMove` | execute_model (in daFganon_Execute 02134B28): move, the background check and the animations run only while it is 0 (tww daFganon_Execute the same) (0) |
| 0x00002 | `m06` | `mDebugAppearAction` | deru (in move 0213B3AC): when not 0, picks the action after appearing instead of the random choice: 1 fly, 2 spinattack, 3 fly2, 4 spinattack2 (tww the same) (0) |
| 0x00004 | `m08` | `mStandbyTime` | standby (in move 0213B3AC): mTimers[0] when standby starts or the player comes within mStartDist; at 0 the action becomes deru (tww the same) (60) |
| 0x00006 | `m0A` | `mFlyTime` | deru (in move 0213B3AC): mTimers[2] when fly / fly2 start; fly_common (in move 0213B3AC) changes to kieru when it reaches 0 (tww the same) (500) |
| 0x00008 | `m0C` | `mFlyDistance` | fly_common (in move 0213B3AC), fly: the target position's z offset from the player in the turned frame (tww fly the same) (1500) |
| 0x0000C | `m10` | `mFlyHeight` | fly_common (in move 0213B3AC), fly: the height above the player on appearing and the target's y offset (tww fly the same) (400) |
| 0x00010 | `m14` | `mFly2Distance` | fly_common (in move 0213B3AC), fly2: the target position's z offset from the player (tww fly2 the same) (1500) |
| 0x00014 | `m18` | `mFly2Height` | fly_common (in move 0213B3AC), fly2: the height above the player on appearing and the target's y offset (tww fly2 the same) (400) |
| 0x00018 | `m1C` | `mBallSpeedType0` | aimEnergyBall (in daFganon_Execute 02134B28): the energy ball's first speed (mBallSpeed) when mType is 0 (tww energy_ball_move the same) (45) |
| 0x0001C | `m20` | `mBallSpeedOtherType` | aimEnergyBall (in daFganon_Execute 02134B28): the energy ball's first speed (mBallSpeed) when mType is not 0 (tww energy_ball_move the same) (45) |
| 0x00020 | `m24` | `mBallSpeedUpType0` | energy_ball_move (in daFganon_Execute 02134B28): added to mBallSpeed on each reflect when mType is 0 (tww the same) (2) |
| 0x00024 | `m28` | `mBallSpeedUpOtherType` | energy_ball_move (in daFganon_Execute 02134B28): added to mBallSpeed on each reflect when mType is not 0 (tww the same) (2) |
| 0x00028 | `m2C` | `mSpinHeight` | spinattack (in move 0213B3AC) / spinattack2 02139194: the y offset from the player where the spin attack starts (spinattack adds 100) (tww the same) (30) |
| 0x0002C | `m30` | `mSpinDistance` | spinattack (in move 0213B3AC): the z offset from the player where the spin attack starts (tww the same) (-270) |
| 0x00030 | `m34` | `mSpin2Distance` | spinattack2 02139194: the z offset from the player where the second spin attack starts; kabe_check (in move 0213B3AC) tests lines of it + 100 for walls before choosing it (tww the same) (-600) |
| 0x00034 | `m38` | `mSpin2SwingDistance` | spinattack2 02139194: the swing starts once the distance to the stored player position is below it (tww the same) (270) |
| 0x00038 | `m3C` | `mShot2ChargeTime` | shot2 02138670: mTimers[0] after the charge animation; the throw starts at 0 (tww the same) (50) |
| 0x0003A | `m3E` | `mSwingHitFrame` | spinattack (in move 0213B3AC) / spinattack2 02139194: at this swing frame mBtAttackTimer is started (tww the same) (1) |
| 0x0003C | `m40` | `mSwingHitEndFrame` | spinattack (in move 0213B3AC) / spinattack2 02139194: mBtAttackTimer = it - mSwingHitFrame (tww the same) (32) |
| 0x00040 | `m44` | `mSwingBtMaxDis` | execute_collision (in daFganon_Execute 02134B28): mBtMaxDis while mBtAttackTimer runs (tww setBtMaxDis the same) (600) |
| 0x00044 | `m48` | `mDownTime` | down 02139680: mTimers[0] while falling; after landing the action becomes kieru when it reaches 0 (tww the same) (150) |

Open (no name yet): `m07`, `m42` (read nowhere in WWHD or tww; tww never writes them either); padding `_pad46`.

## bwds_class (Molgera larva; d_a_bwds.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03C8 | `m02AC` | `mPhs` | daBwds_Create 02108BF0: dComIfG_resLoad of "Bwds" into it; daBwds_Delete 02108750 deletes the resource with it |
| 0x03D0 | `m02B4` | `mType` | daBwds_Create 02108BF0: parameter byte 0; 0x23 starts in mode -10 (jumps out of the sand at once, at a random angle) |
| 0x03D8 | `mp02BC` | `mpBodyModels` | useHeapInit 021087FC creates the 13 body segment models (array of 14); body_control (in daBwds_Execute 0210602C) places segment i - 1 at mBodyPos[i]; daBwds_Draw 02105F00 / daBwds_Execute 0210602C use the 13 - mBurstCount still alive |
| 0x0410 | `m02F4` | `mFrameCounter` | daBwds_Execute 0210602C adds 1 per frame; ug_move (in daBwds_Execute 0210602C) and body_control use it as the phase of the weave and segment pulse sines, daBwds_Execute 0210602C as the phase of the hit shake |
| 0x0412 | `m02F6` | `mAction` | move (in daBwds_Execute 0210602C) switches on it (ACTION_UG_MOVE / HOOK_ON / HOOK_CHANCE / FAIL); damage_check / hook_on / hook_chance set it |
| 0x0414 | `m02F8` | `mMode` | ug_move / hook_on / hook_chance / fail (in daBwds_Execute 0210602C) step through it; daBwds_Create 02108BF0 sets -10 for mType 0x23 |
| 0x0416 | `m02FA` | `mInSand` | move (in daBwds_Execute 0210602C): 1 while current.pos.y is at most mSandY + 20; daBwds_Execute 0210602C then raises eyePos by 90 and moves the head sphere up with a smaller radius |
| 0x0417 | `m02FB` | `mVulnerable` | ug_move (jump) / hook_chance (in daBwds_Execute 0210602C) set 1 each frame; damage_check takes a head hit only while it is 1; move clears it every frame |
| 0x0418 | `m02FC` | `mTargetPos` | ug_move (in daBwds_Execute 0210602C): the player's position, a random point near the actor or a point 2000 ahead (set_jump_target); pos_move turns current.angle.y toward it |
| 0x0428 | `m030C` | `mTurnSpeed` | ug_move (in daBwds_Execute 0210602C) sets 3000 / 1000 (+ reg); pos_move: angle step = it * mTurnSpeedRate |
| 0x042C | `m0310` | `mTurnSpeedRate` | pos_move (in daBwds_Execute 0210602C) eases it to 1 and multiplies mTurnSpeed by it |
| 0x0430 | `m0314` | `mTimers` | daBwds_Execute 0210602C counts all 5 down each frame; ug_move / hook_chance / fail (in daBwds_Execute 0210602C) load them ([0] = 0x23, l_HIO.m014; [1] / [2] random waits) |
| 0x043A | `m031E` | `mNoDamageTimer` | damage_check (in daBwds_Execute 0210602C) runs only while it is 0 and sets 10 / 6 after a hit; hook_on / fail keep it at 3; daBwds_Execute 0210602C counts it down |
| 0x043C | `m0320` | `mKnockbackDist` | damage_check (in daBwds_Execute 0210602C) sets 40 (+ reg) on a hit; move moves the actor back by it along mKnockbackAngleY and eases it to 0 |
| 0x0440 | `m0324` | `mKnockbackAngleY` | damage_check (in daBwds_Execute 0210602C): fopAcM_searchPlayerAngleY; move (in daBwds_Execute 0210602C) YrotS by it for the knockback |
| 0x0442 | `m0328` | `mHitShakeTimer` | damage_check (in daBwds_Execute 0210602C) sets 15 (+ reg); daBwds_Execute 0210602C counts it down and shakes the model's Y / X rotation by it * 500 |
| 0x0444 | `m032C` | `mBodyPos` | body_control (in daBwds_Execute 0210602C): [0] head joint position, [1..14] each segment trailing the previous one; segment spheres m0670[i] centre on it; fail bursts at [12 - mBurstCount] |
| 0x04F8 | `m03E0` | `mBodyAngle` | body_control (in daBwds_Execute 0210602C): [0] shape_angle, [i - 1] the atan2 angles toward segment i; the segment models are rotated by it |
| 0x0554 | `m043C` | `mBodyMoveVec` | body_control (in daBwds_Execute 0210602C): last frame's segment position, then (new - old) * stiffness; added back to the next frame's trail direction (the body's swing) |
| 0x0608 | `m04F0` | `mBurstCount` | fail (in daBwds_Execute 0210602C) adds 1 per segment burst (SIBOUBAKUEN particle) and moves on after 13; daBwds_Execute 0210602C / daBwds_Draw 02105F00 handle only the 13 - it segments left |
| 0x060C | `m04F4` | `mDeleteRequest` | ko_delete_sub 020F9AA0 (d_a_bwd, view bwd_detail::Larva) sets 1 for every larva; damage_check (in daBwds_Execute 0210602C) then deletes the actor |
| 0x060D | `m04F5` | `mBodyFollowMode` | hook_chance / fail (in daBwds_Execute 0210602C) set 1 (segments kept above the ground, inertia on), ug_move 2 (diving); body_control changes the trail direction and stiffness on it; daBwds_Execute 0210602C clears it every frame |
| 0x0614 | `m04FC` | `mTargetAngleX` | hook_chance (in daBwds_Execute 0210602C): 0 or 0x7F00 at random per bounce; current.angle.x is eased to it |
| 0x061A | `m0502` | `mBounceTurnY` | hook_chance (in daBwds_Execute 0210602C): random +-2500 per bounce, added to current.angle.y every frame |
| 0x061C | `m0504` | `mSandY` | daBwds_Execute 0210602C: ground cross - 90 under the actor (sand surface level); ug_move / hook_chance / fail (in daBwds_Execute 0210602C) and move compare current.pos.y with it |
| 0x0620 | `m0508` | `mStts` | daBwds_Create 02108BF0: Init(0xFA, 0) and SetStts of every sphere; damage_check (in daBwds_Execute 0210602C) Move() |
| 0x065C | `m0544` | `mHsSph` | daBwds_Create 02108BF0: Set(hs_sph_src) (the hookshot target sphere); daBwds_Execute 0210602C sets its centre on the head and radius l_HIO.m01C * scale |
| 0x0788 | `m0670` | `mBodySph` | daBwds_Create 02108BF0: [0] Set(cc_sph_src) (head), [1..14] Set(body_sph_src); daBwds_Execute 0210602C / body_control place them on the head and mBodyPos; damage_check reads their Tg hits |
| 0x191C | `m1804` | `mSandSplashPos` | daBwds_Execute 0210602C: y = ground cross under the actor, x / z the actor's position when it leaves or dives into the sand; the OUT / DIVE smoke and sand particles start there |
| 0x1928 | `m1810` | `mMoveSmokePos` | daBwds_Execute 0210602C: the actor's x / z on the ground cross; the IDOU (moving) smoke and sand particles follow it |
| 0x1934 | `m181C` | `mMoveSmokeState` | ug_move (in daBwds_Execute 0210602C) sets 1 when the larva goes under the sand; daBwds_Execute 0210602C starts the IDOU smoke / sand at 1 (then 2) and ends them once the larva is above the sand |
| 0x1935 | `m181D` | `mOutSmokeTimer` | daBwds_Execute 0210602C sets 5 (+ reg) when the larva leaves the sand (OUT_SMOKE00), counts it down and ends that smoke at 0 |
| 0x1936 | `m181E` | `mDiveSmokeTimer` | daBwds_Execute 0210602C sets 5 (+ reg) when the larva dives into the sand (DIVE_SMOKE00), counts it down and ends that smoke at 0 |
| 0x1938 | `m1820` | `mSmokeCb` | daBwds_Execute 0210602C: [0] IDOU_MOKO00 (moving), [1] OUT_SMOKE00, [2] DIVE_SMOKE00 smoke callbacks; daBwds_Delete 02108750 removes them |
| 0x1998 | `m1880` | `mMoveSandCb` | daBwds_Execute 0210602C passes it to the IDOU_SUNA00 (moving sand) particle and ends it with the moving smoke; daBwds_Delete 02108750 removes it |
| 0x19AC | `m1894` | `mSplashFrame` | daBwds_Execute 0210602C: [0] leaving / [1] diving into the sand; set 1, + 1 per frame up to 60, then 0; it - 1 is the frame of the splash model's animations; daBwds_Draw 02105F00 draws the splash while it is set |
| 0x19B0 | `m1898` | `mSplashPos` | daBwds_Execute 0210602C: the actor's position when the splash starts; the splash model is placed there |
| 0x19C8 | `mp18B0` | `mpSplashMorf` | useHeapInit 021087FC creates the 2 sand splash models (Bwd BDL_GSP00 / GSP01, one for leaving and one for diving); daBwds_Execute 0210602C sets their frame and matrix, daBwds_Draw 02105F00 draws them |
| 0x19D0 | `mp18B8` | `mpSplashBtk` | useHeapInit 021087FC: btk of each splash model; daBwds_Execute 0210602C sets its frame, daBwds_Draw 02105F00 enters it |
| 0x19D8 | `mp18C0` | `mpSplashBrk` | useHeapInit 021087FC: brk of each splash model; daBwds_Execute 0210602C sets its frame, daBwds_Draw 02105F00 enters it |
| 0x19E0 | `m18C8` | `mHioOwner` | daBwds_Create 02108BF0 sets 1 when it created the l_HIO child; daBwds_Delete 02108750 deletes the HIO child only then |

Open (no name yet): `m0500` (two meanings: the spin speed added to shape_angle.z while ug_move falls, and
the target roll that hook_chance eases shape_angle.z to) and the padding bytes `m0308`. The HIO block
zeldaret/tww (CC0) has offset names only for these fields; its d_a_bwds.cpp
was read alongside our decomp. Molgera's view of this class, `bwd_detail::Larva` (d_a_bwd.h), uses the same
name (`mDeleteRequest`).

### daBwds_HIO_c (l_HIO; d_a_bwds.h)

The read HIO values (round 2). Each is set only by daBwds_HIO_c_ct (default in parentheses) and never written elsewhere in WWHD or tww; the reads are all inlined into daBwds_Execute 0210602C (d_a_bwds_exec.cpp).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x01 | `m005` | `mNoMove` | daBwds_Execute 0210602C: the move / ground checks and the attack / push collision bits run only while it is 0 (tww the same) (0) |
| 0x04 | `m008` | `mBodyScale` | body_control / move (in daBwds_Execute 0210602C): scales the body model, the joint offsets and the body / hookshot sphere radii (tww the same) (0.8) |
| 0x08 | `m00C` | `mSplashScale` | daBwds_Execute 0210602C: scale of the sand splash models (tww the same) (0.7) |
| 0x0C | `m010` | `mMoveSpeed` | ug_move (in daBwds_Execute 0210602C): the speedF of each jump out of the sand (tww the same) (7.0) |
| 0x10 | `m014` | `mHookChanceTime` | hook_chance (in daBwds_Execute 0210602C): mTimers[0] at the start; at 0 the larva goes back to ug_move (tww the same) (220) |
| 0x14 | `m018` | `mSwayMax` | ug_move (in daBwds_Execute 0210602C): upper limit of the sway amplitude (distance * 7) (tww the same) (2000) |
| 0x18 | `m01C` | `mHsSphRadius` | daBwds_Execute 0210602C: radius of the hookshot sphere mHsSph (times mBodyScale) (tww the same) (300) |

## bdkobj_class (Helmaroc King's breakable tower parts; d_a_bdkobj.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03B4 | `m298` | `mType` | daBdkobj_Create 020745AC: parameter byte 0 (0xFF -> 0); 0 = block, 1 = pole (BLOCK / POLE break particles in daBdkobj_Execute 020729A8), 2 = tower stairs (dBgW, tower_kaidan_move); useHeapInit 02074330 picks the models by it |
| 0x03B5 | `m299` | `mBreakTimer` | tower_kaidan_move (in daBdkobj_Execute 020729A8) sets 100 when the stairs break; daBdkobj_Execute 020729A8 counts it down; at 90 or less the bridge smoke fades |
| 0x0A4C | `m918` | `mSmokeCb` | daBdkobj_Execute 020729A8 / tower_kaidan_move (in daBdkobj_Execute 020729A8) pass it to the BLOCK / POLE / BRIDGE smoke particles (toon group) and end it first; daBdkobj_Create 020745AC sets follow off; daBdkobj_Delete 020742C4 removes it |
| 0x0A6C | `m938` | `mSmokeAlpha` | tower_kaidan_move (in daBdkobj_Execute 020729A8): 0xB4 when the bridge smoke starts, -2 per frame once mBreakTimer is 90 or less; the smoke emitter's global alpha |

Fragment helper struct bdo_eff_s (offsets within the struct):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0000 | `m000` | `mState` | daBdkobj_Execute 020729A8 sets 1 for the three block / pole fragments, tower_kaidan_move (in daBdkobj_Execute 020729A8) 2 for the two stair fragments; hahen_move (same) runs top_hahen_move at 1 and kaidan_hahen_move at 2; 0 = off (daBdkobj_Draw 020728A8 draws only the others; a type-9 hit, y < 8500 or no ground clears it) |
| 0x0008 | `m008` | `mPos` | top_hahen_move / kaidan_hahen_move (in daBdkobj_Execute 020729A8): the fragment's position (moved by mSpeedF / mSpeedY, ground and wall checks); eff_set_mtx places the model there, mSph is centred on it |
| 0x0014 | `m014` | `mPosOld` | top_hahen_move / kaidan_hahen_move (in daBdkobj_Execute 020729A8) copy mPos before moving; on a wall hit x / z go back to it |
| 0x0020 | `m020` | `mSpeedF` | daBdkobj_Execute 020729A8 sets 20..35 (+ reg) for a block / pole fragment; top_hahen_move (same) moves it forward along mAngle.y, eases it to 0 on the ground and clears it at a wall |
| 0x002C | `m024` | `mSpeedY` | daBdkobj_Execute 020729A8 sets 60..80 (+ reg); top_hahen_move (in daBdkobj_Execute 020729A8) -5 and kaidan_hahen_move -3 per frame, bounced (* -0.3) or cleared on the ground |
| 0x0030 | `m028` | `mScale` | daBdkobj_Execute 020729A8 / tower_kaidan_move (in daBdkobj_Execute 020729A8) set 0.7 +- 0.25; eff_set_mtx scales the model by it, top_hahen_move sizes the sphere, ground offset and push by it |
| 0x0034 | `m02C` | `mSpinRate` | top_hahen_move (in daBdkobj_Execute 020729A8): random +-100 on each bounce; on the ground mAngleSpeed.y = mSpeedF * it |
| 0x0038 | `m030` | `mAngle` | daBdkobj_Execute 020729A8 / tower_kaidan_move (in daBdkobj_Execute 020729A8) set a random angle; top_hahen_move / kaidan_hahen_move add mAngleSpeed each frame; eff_set_mtx rotates the model by it |
| 0x003E | `m036` | `mAngleSpeed` | daBdkobj_Execute 020729A8 / tower_kaidan_move (in daBdkobj_Execute 020729A8) set random +-6000 / +-800; added to mAngle each frame; top_hahen_move / kaidan_hahen_move ease or set it on the ground |
| 0x01AC | `m1A4` | `mPushSpeed` | top_hahen_move (in daBdkobj_Execute 020729A8): set on a type-8 hit (15 + rnd) * (1 - mScale), moves the fragment along mPushAngleY while > 0.1 and eases to 0; a new push only below 1 |
| 0x01B0 | `m1A8` | `mPushAngleY` | top_hahen_move (in daBdkobj_Execute 020729A8): the hitting actor's shape_angle.y +- 2000; YrotS by it for the push |
| 0x01B2 | `m1AA` | `mPushSpinY` | top_hahen_move (in daBdkobj_Execute 020729A8): random +-400 on a push, added to mAngle.y while the push runs |
| 0x01B4 | `m1AC` | `mFrameCounter` | hahen_move (in daBdkobj_Execute 020729A8) adds 1 per frame; tower_kaidan_move sets a random start; kaidan_hahen_move uses it as the phase of the rocking sines |

Open (no name yet): the padding bytes, `m0C` of `BdkObjAttackInfo` (the CcAtInfo copy) became `mHitAngle` with the CcAtInfo copies (other.md, round 12).
bdo_eff_s keeps its GameCube offset names in the evidence (HD inserted `mHorizontalSpeedX` / `Z` after
`m020`, so the old names after it no longer match the WWHD offsets).

## att_class (Puppet Ganon's string attention targets; d_a_att.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03D1 | `m2B5` | `mStringNo` | daAtt_Create 02058B38: parameter byte 0; below 10 it is the index of Puppet Ganon's string (daAtt_Execute 020587B4 follows bgn mHimoTopPos[it] and hits mMove[it]); 100 = a plain attention point, 101 = the tongue sphere (sita_sph_src) |
| 0x066C | `m550` | `mHitWaitTimer` | daAtt_Execute 020587B4 sets 10 after a hit on the string and counts it down; no new hit is taken until it reaches 0 |

Open (no name yet): the padding bytes `m298`, `m2B6`. The view `bgn_att_move_s` (bgn's move_s) uses
move_s's names (bgn_class section).

## fgmahou_class (Phantom Ganon's dark ball; d_a_fgmahou.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03F0 | `field_0x2D4` | `mWeaveAngleY` | move (in daFgmahou_Execute 0213D594): starts at mOrbNumber * reg, + 0xDAC per frame; shape_angle.y = home.angle.y + sin(it) * the distance term (the ball's weave) |
| 0x03F2 | `field_0x2D6` | `mWeaveAngleX` | move (in daFgmahou_Execute 0213D594): random start, + 0xCE4 per frame; shape_angle.x = home.angle.x + sin(it) * 0.75 * the distance term |
| 0x03FC | `field_0x2E0` | `mReflectFrame` | move (in daFgmahou_Execute 0213D594): set 1 when the ball is sent back at Phantom Ganon (state 5), + 1 per frame and cleared after 8 |
| 0x089C | `field_0x780` | `mDeleteTimer` | move (in daFgmahou_Execute 0213D594) sets 50 when the ball hits or is lost; daFgmahou_Execute then stops moving it, ends its effect at 50 and deletes the actor at 0 |

Open (no name yet): `field_0x2D8` (no uses), `field_0x2DE` (only counted down, never set), `field_0x2E2`
(only set to 0x8000 / -0x8000, never read) and the padding `field_0x290`. The view of fganon_class in this
header, `FgmahouBossHD`, is covered in the fganon_class section.

## gnd_class (Ganondorf; d_a_gnd.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03C8 | `m03C8` | `mpSelf` | gnd_class_ct 0215788C stores the actor's own address (name from the decomp view gndMainFields::mpSelf, d_a_gnd_main.h) |
| 0x03D4 | `m03D4` | `mBgmParam` | daGnd_Create 02157B30: parameter bits 0-3; starts BGM 0x8000004D when not 0 (name from the decomp view gndExecFields::mBgmParam, d_a_gnd_exec.h) |

Both fields take the names of the decomp's own views of gnd_class (gndMainFields, gndExecFields).
`m0C` of `CcAtInfo_gnd` (the CcAtInfo copy) became `mHitAngle` with the CcAtInfo copies (other.md, round 12).

## dr2_class (Valoo's tail and the rock in Gohma's room; d_a_dr2.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03D8 | `unk_2BC` | `mFrameCounter` | daDr2_Execute 0212B3D0 adds 1 per frame; move (in daDr2_Execute 0212B3D0) uses it as the phase of the rock-shake sines of the tail joints |
| 0x04CC | `unk_3B0` | `mHookTargetPos` | nodeCallBack 0212AACC: world position of joint HANE_L2 (+ 215, 0, -5); daKui_Execute 021ACB0C (d_a_kui, view KuiDragonTailView) puts the hookshot target there |
| 0x04D8 | `unk_3BC` | `mJointRotY` | move (in daDr2_Execute 0212B3D0): [2..11] sin(mFrameCounter) * mShakeAmplitude per joint while the rock shakes, [0] the grabbed tail's angle; nodeCallBack 0212AACC YrotM of the joint by it |
| 0x04F0 | `unk_3D4` | `mJointRotZ` | move (in daDr2_Execute 0212B3D0): [2..11] cos(mFrameCounter) * mShakeAmplitude per joint while the rock shakes, [0] eased to 0 while grabbed; nodeCallBack 0212AACC ZrotM of the joint by it |
| 0x0508 | `unk_3EC` | `mSwayRotY` | move (in daDr2_Execute 0212B3D0): sin(mSwayPhaseY) * mSwayAmplitude, eased to -0.175 while pulled; nodeCallBack 0212AACC rotates the tail joints about Y by it (radians) |
| 0x050C | `unk_3F0` | `mSwayRotZ` | move (in daDr2_Execute 0212B3D0): sin(mSwayPhaseZ) * mSwayAmplitude, eased to 0 while pulled; nodeCallBack 0212AACC rotates the tail joints about Z by it (radians) |
| 0x0510 | `unk_3F4` | `mSwayPhaseY` | move (in daDr2_Execute 0212B3D0): + 0.04 per frame; the phase of mSwayRotY |
| 0x0514 | `unk_3F8` | `mSwayPhaseZ` | move (in daDr2_Execute 0212B3D0): + 0.05 per frame; the phase of mSwayRotZ |
| 0x0518 | `unk_3FC` | `mKuiId` | daDr2_Create 0212D394: the process id of the KUI actor (hookshot target, parameter 0x511) it requests; move (in daDr2_Execute 0212B3D0) looks it up and reads its health (1..3) for the tail state |
| 0x051C | `unk_400` | `mTimers` | daDr2_Execute 0212B3D0 counts all 5 down each frame; iwa_move (in daDr2_Execute 0212B3D0) sets [0] = 0x32 when the rock lands on Gohma |
| 0x0526 | `unk_40A` | `mTailState` | move (in daDr2_Execute 0212B3D0) / iwa_move set it (Dr2TailState: idle, pulled, grabbed, rock shake); nodeCallBack 0212AACC bends the tail joints by it; new_himo2_move 0216F7FC (d_a_himo2, view himo2_dr_c) sets 3 / 0 in the boss-fall demo |
| 0x0528 | `unk_40C` | `mPullScaleX` | move (in daDr2_Execute 0212B3D0): 0.98 at rest, eased to 1.04 while pulled; nodeCallBack 0212AACC scales the tail joints' x by it while pulled |
| 0x052C | `unk_410` | `mSwayAmplitude` | move (in daDr2_Execute 0212B3D0): eased to 0.048 (0 once the KUI's health is 1); scales mSwayRotY / mSwayRotZ; iwa_move clears it when the tail is far from home |
| 0x0530 | `unk_414` | `mShakeAmplitude` | iwa_move (in daDr2_Execute 0212B3D0) eases it to 2000 while the rock rides Gohma; new_himo2_move 0216F7FC (view himo2_dr_c) eases it to 2000 / 100; move scales the joint shake mJointRotY / Z by it |
| 0x0534 | `unk_418` | `mpRockModel` | useHeapInit 0212CDAC: model IWA00; daDr2_Execute 0212B3D0 places it at mRockPos / mRockAngle; iwa_draw (in daDr2_Draw 0212AEEC) draws it while mRockState is below 10 |
| 0x0538 | `unk_41C` | `mpMagmaModel` | useHeapInit 0212CDAC: model GAN_MAGMA (lava on the rock); daDr2_Execute 0212B3D0 places it with the rock; iwa_draw (in daDr2_Draw 0212AEEC) draws it while mMagmaFrame runs |
| 0x053C | `unk_420` | `mpMagmaBtk` | useHeapInit 0212CDAC: btk GAN_MAGMA of mpMagmaModel; iwa_move (in daDr2_Execute 0212B3D0) sets its frame from mMagmaFrame, iwa_draw (in daDr2_Draw 0212AEEC) enters it |
| 0x0540 | `unk_424` | `mMagmaFrame` | iwa_move (in daDr2_Execute 0212B3D0) sets 1 when the rock leaves Gohma dripping lava, + 1 per frame up to 180, then 0; the frame of mpMagmaBtk; iwa_draw (in daDr2_Draw 0212AEEC) draws the lava while it is not 0 |
| 0x0544 | `unk_428` | `mpFloorModel` | useHeapInit 0212CDAC: model MBYO1 (the floor before the boss is beaten); yuka_move (in daDr2_Execute 0212B3D0) places it, yuka_draw (in daDr2_Draw 0212AEEC) draws it unless mFloorBroken |
| 0x0548 | `unk_42C` | `mpBrokenFloorModel` | useHeapInit 0212CDAC: model MBYO2; yuka_move (in daDr2_Execute 0212B3D0) places it, yuka_draw (in daDr2_Draw 0212AEEC) draws it (with the view-projected lava texture) when mFloorBroken |
| 0x054C | `unk_430` | `mpFloorBtk` | useHeapInit 0212CDAC: btk MBYO1 (loop) of mpFloorModel; yuka_move (in daDr2_Execute 0212B3D0) plays it, yuka_draw (in daDr2_Draw 0212AEEC) enters it |
| 0x0550 | `unk_434` | `mpBrokenFloorBtk` | useHeapInit 0212CDAC: btk MBYO2 of mpBrokenFloorModel; yuka_move (in daDr2_Execute 0212B3D0) plays it or sets frame 299, yuka_draw (in daDr2_Draw 0212AEEC) enters it |
| 0x0554 | `unk_438` | `mpBrokenFloorBrk` | useHeapInit 0212CDAC: brk MBYO2 of mpBrokenFloorModel; yuka_move (in daDr2_Execute 0212B3D0) plays it or sets frame 299, yuka_draw (in daDr2_Draw 0212AEEC) enters it |
| 0x0558 | `unk_43C` | `mFloorBroken` | yuka_move (in daDr2_Execute 0212B3D0) sets 1 when the stage boss is beaten (and the start stage is not an X stage); then mpBrokenFloorModel / mpBgW2 are used and the other floor is moved to y -10000 |
| 0x055C | `unk_440` | `mFloorMtx` | yuka_move (in daDr2_Execute 0212B3D0): the matrix of mpFloorModel (scaled), given to mpBgW1 by cBgW_Set in useHeapInit 0212CDAC |
| 0x058C | `unk_470` | `mBrokenFloorMtx` | yuka_move (in daDr2_Execute 0212B3D0): the matrix of mpBrokenFloorModel (scaled), given to mpBgW2 by cBgW_Set in useHeapInit 0212CDAC |
| 0x05C4 | `unk_4A8` | `mRockPos` | daDr2_Create 0212D394: home + (0, -50, 0); iwa_move (in daDr2_Execute 0212B3D0) moves the rock (shake, fall onto Gohma, rise back); daDr2_Execute 0212B3D0 places mpRockModel there; the rock particles and sounds start there |
| 0x05D0 | `unk_4B4` | `mRockAngle` | iwa_move (in daDr2_Execute 0212B3D0): x / y follow Gohma while riding, z the shake; daDr2_Execute 0212B3D0 rotates mpRockModel by it; hahen fragments start with it |
| 0x05D6 | `unk_4BA` | `mRockState` | iwa_move (in daDr2_Execute 0212B3D0) steps through it (Dr2RockState: return, attached, fall, ride Gohma, rise, break, gone); iwa_draw (in daDr2_Draw 0212AEEC) draws the rock below 10; new_himo2_move 0216F7FC (view himo2_dr_c) and daKui_Execute 021ACB0C (view KuiDragonTailView) read it |
| 0x05DC | `unk_4C0` | `mRockRideOffsetY` | iwa_move (in daDr2_Execute 0212B3D0): while riding Gohma, (100 - Gohma's joint 3 height) * 0.3 when that joint is low; raises the rock's tilt target; cleared on return |
| 0x05E0 | `unk_4C4` | `mDragonAnmState` | dr_move (in daDr2_Execute 0212B3D0): 0, then 1 after starting DR_BOSS_DEMO1 on mpMorf2 once mShowDragon is set; back to 0 when it is cleared |
| 0x05E4 | `unk_4C8` | `mRockReturnWait` | iwa_move (in daDr2_Execute 0212B3D0) sets 0x82 when the rock is back at the top and counts it down before easing the actor back to home |
| 0x05E6 | `unk_4CA` | `mRockFallTimer` | new_himo2_move 0216F7FC (d_a_himo2, view himo2_dr_c) sets 0x5A in the rope jump demo; iwa_move (in daDr2_Execute 0212B3D0) shakes the rock while it runs, counts it down and drops the rock at 1 |
| 0x05E8 | `unk_4CC` | `mShakeStepTimer` | iwa_move (in daDr2_Execute 0212B3D0): every 3 (+ reg) frames a new shake target (mRockShakeAngleZ / mRockShakeOffsetY); move sets 20 once the KUI's health is 1 |
| 0x05EA | `unk_4CE` | `mRockShakeAngleZ` | iwa_move (in daDr2_Execute 0212B3D0): random +-300 per shake step; mRockAngle.z is eased to it; 0 when not shaking |
| 0x05EC | `unk_4D0` | `mRockShakeOffsetY` | iwa_move (in daDr2_Execute 0212B3D0): random +-20 per shake step; mRockPos.y is eased to home - 50 + it; 0 when not shaking |
| 0x05F0 | `unk_4D4` | `mpBrokenRockEmitter` | iwa_move (in daDr2_Execute 0212B3D0): the BTDBROKENROCK00 emitter (rate mBrokenRockRate); its particles spawn BTDBROKENROCKTAIL00; invalidated when mBrokenRockTimer runs out |
| 0x05F4 | `unk_4D8` | `mDripCb` | iwa_move (in daDr2_Execute 0212B3D0) passes it to the BTDDRIPMAGMA00 (dripping lava) particle and ends it when the rock is back; daDr2_Delete 0212CD10 removes it |
| 0x0608 | `unk_4EC` | `mSmokeCb` | iwa_move (in daDr2_Execute 0212B3D0) passes it to the BTDSMOKE02 particle (toon); daDr2_Create 0212D394 sets follow off; daDr2_Delete 0212CD10 removes it |
| 0x0628 | `unk_50C` | `mShowDragon` | new_himo2_move 0216F7FC (d_a_himo2, view himo2_dr_c) sets 1 / 0 in the rope demo; dr_draw (in daDr2_Draw 0212AEEC) draws and dr_move (in daDr2_Execute 0212B3D0) animates the dragon (mpMorf2) while it is set |
| 0x0629 | `unk_50D` | `mBrokenRockRate` | iwa_move (in daDr2_Execute 0212B3D0): 1 / 30 / 10 (+ reg); the rate given to mpBrokenRockEmitter |
| 0x062A | `unk_50E` | `mBrokenRockState` | iwa_move (in daDr2_Execute 0212B3D0): 1 = create mpBrokenRockEmitter, 2 = running (spawning tail particles), 0 = none |
| 0x062C | `unk_510` | `mBrokenRockTimer` | iwa_move (in daDr2_Execute 0212B3D0): 20 / 1 / 2 with a new mpBrokenRockEmitter, counted down while it runs; at 0 the emitter is invalidated |
| 0x0630 | `unk_514` | `mpBtd` | iwa_move (in daDr2_Execute 0212B3D0): Gohma (btd_class), found by s_a_d_sub 0212B380 and kept; the rock rides its joints |

Rock fragment helper struct iwa_hahen_s (offsets within the struct):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0004 | `unk_04` | `mActive` | iwa_move (in daDr2_Execute 0212B3D0) sets it for the 6 fragments when the rock breaks; hahen_move (same) clears it below y -100; hahen_draw (in daDr2_Draw 0212AEEC) draws only active fragments |
| 0x0008 | `unk_08` | `mPos` | iwa_move (in daDr2_Execute 0212B3D0): starts at mRockPos; hahen_move (same) adds mSpeed and places the fragment model there |
| 0x0014 | `unk_14` | `mSpeed` | iwa_move (in daDr2_Execute 0212B3D0): MtxPosition of (hsx[i], 0, hsz[i]) * 3 along mRockAngle; hahen_move (same) adds it to mPos and -0.5 to its y per frame |
| 0x0020 | `unk_20` | `mAngle` | iwa_move (in daDr2_Execute 0212B3D0): starts at mRockAngle; hahen_move (same) spins x / z each frame and rotates the fragment model by it |

Open (no name yet): `unk_4C6` (dr_move clears it and counts it down; nothing reads it) and the padding
bytes. The HIO block `daDr2_HIO_c` is not renamed. zeldaret/tww (CC0) has `unk_` names only for these
fields; its d_a_dr2.cpp was read alongside our decomp. The private views of this class use the same names:
`himo2_dr_c` (d_a_himo2.h: `mTailState`, `mShakeAmplitude`, `mRockState`, `mRockFallTimer`, `mShowDragon`)
and `KuiDragonTailView` (d_a_kui.h: `mHookTargetPos`, `mRockState`).

## dk_class (Helmaroc King, Forsaken Fortress flight demo; d_a_dk.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03D0 | `field_0x2B4` | `mType` | daDk_Create 02125AA0: parameter byte 0; 1 = the Forsaken Fortress flight (event checks, not created once event bits 0x0310 and 0x0001 are set); below 2 the actor and its tails use ground collision (mAcch), from 2 not |
| 0x03D4 | `field_0x2B8` | `mpMorf` | useHeapInit 02125858 creates the body model (McaMorf, joint callbacks); daDk_Execute 02124F00 plays / calcs it and hands it to dDemo_setDemoData; daDk_Draw 02124C44 draws it |
| 0x03D8 | `field_0x2BC` | `mFrameCounter` | daDk_Execute 02124F00 adds 1 per frame (not read in HD or zeldaret/tww) |
| 0x03DC | `field_0x2C0` | `mTimers` | daDk_Execute 02124F00 counts all 5 down each frame (never loaded in HD or zeldaret/tww) |
| 0x09DC | `field_0x8C0` | `mAcchCir` | daDk_Create 02125AA0 constructs it, SetWall(0, 300), and passes it to mAcch.Set |
| 0x0A1C | `field_0x900` | `mAcch` | daDk_Create 02125AA0: Set with mAcchCir when mType is below 2; daDk_Execute 02124F00 CrrPos, tail_control (in daDk_Execute 02124F00) keeps the tails above its ground height |
| 0x0BE1 | `field_0xAC5` | `mEventState` | daDk_Create 02125AA0 sets 0 (mType 1) or 10; daDk_Execute 02124F00: 0 waits for event bit 0x0310, 2 for the "zelda_fly" event, 3 for event bit 0x0001 and then deletes the actor (-1); daDk_Draw 02124C44 draws only above 2 |
| 0x0BE8 | `field_0xACC` | `mSmokeCb` | daDk_Create 02125AA0 constructs it (dPa_smokeEcallBack(1, 1, 0, 0)); daDk_Delete 02125808 removes it |
| 0x0C0A | `field_0xAEE` | `mNotInDemo` | daDk_Execute 02124F00: 0 while dDemo_setDemoData drives the model, 1 otherwise (then it plays the morf itself); daDk_Draw 02124C44 draws nothing while it is 1 |

Tail helper struct tail_s (offsets within the struct):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0024 | `field_0x024` | `mJointPos` | tail_control (in daDk_Execute 02124F00): [0] the tail root (mRootPos[1]), [1..9] each piece trailing the previous one; tail_draw (in daDk_Draw 02124C44) places the 9 tail models there |
| 0x009C | `field_0x09C` | `mJointAngle` | tail_control (in daDk_Execute 02124F00): the atan2 angles from piece i - 1 to piece i; tail_draw (in daDk_Draw 02124C44) rotates the tail models by them |
| 0x00D8 | `field_0x0D8` | `mJointMoveVec` | tail_control (in daDk_Execute 02124F00): last frame's piece position, then (new - old) * 0.77; added back into the next frame's trail direction (the tail's swing) |
| 0x0150 | `field_0x150` | `mRootPos` | nodeCallBack 02124B6C: [1] the tail joint's position, [0] the point 10 behind it; tail_control (in daDk_Execute 02124F00) aims the tail from [0] to [1] and starts it at [1] |
| 0x0168 | `field_0x168` | `mRootAngle` | tail_control (in daDk_Execute 02124F00): the atan2 angles from mRootPos[0] to mRootPos[1] |
| 0x0170 | `field_0x170` | `mRootDir` | tail_control (in daDk_Execute 02124F00): (0, 0, 30) rotated by mRootAngle; added (fading per piece) to every piece's trail direction |

Open (no name yet): `field_0xAC8` (only cleared in daDk_Create) and the padding bytes `field_0x290`,
`field_0x2BE`, `field_0xAC4`, `field_0xAEC`. zeldaret/tww (CC0) names kept: `tails`, `mpModelKamen`, `mPhs`.

## demo_dk_class (Helmaroc King idling, demo; d_a_demo_dk.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03D2 | `unk_2B6` | `mMode` | daDEMO_DK_Execute 02122AA8 switches on it (Mode_e: MODE_WAIT_e, MODE_AKUBI_e); mode_wait (same) sets MODE_AKUBI_e when it starts the yawn, mode_akubi MODE_WAIT_e |
| 0x03D8 | `unk_2BC` | `mTimers` | daDEMO_DK_Execute 02122AA8 counts both down each frame (never loaded in HD or zeldaret/tww) |
| 0x03DC | `unk_2C0` | `mYawnWait` | mode_akubi (in daDEMO_DK_Execute 02122AA8) sets 3 after a yawn; mode_wait counts it down per finished WAIT1 loop and allows the next yawn (10 percent) only at 0 |
| 0x03E0 | `unk_2C4` | `mBasePos` | daDEMO_DK_Create 02122E78 copies current.pos; daDEMO_DK_Execute 02122AA8 sets current.pos.y from it each frame |

Open (no name yet): the padding bytes `unk290`, `unk2B4`.

bita_class (`field_0x2a7`: mode_dead sets a random 0..3, nothing reads it) and swtdoor_class (`field_0x29c`:
daSwtdoor_Create stores parameter byte 0, nothing reads it) have no field with enough evidence; both stay open.

## daDaiocta_Eye_c and its HIO view (Big Octo eye; d_a_daiocta_eye.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`, unnamed there). The three fields are the minimum step of the eye's random look target, the counterparts of tww's mEyeRndRotMaxX / Y / Z; the HIO view `octoEyeHD::Hio` holds the values the eye copies (mRndRotMinX / Y / Z: a different spelling, so that each file keeps one old name per new name).

| Struct / WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| daDaiocta_Eye_c 0x4C8 | `m2EC` | `mEyeRndRotMinX` | setRotEye 0211A1E8 (d_a_daiocta.cpp) passes it to oct_rndRotEyeAxis as the minimum: a random target closer than it to the current angle is pushed away by it (tww rndRotEye the same); _execute 0211DCAC copies the HIO value each frame |
| daDaiocta_Eye_c 0x4CA | `m2EE` | `mEyeRndRotMinY` | the same for the Y axis |
| daDaiocta_Eye_c 0x4CC | `m2F0` | `mEyeRndRotMinZ` | the same for the Z axis |
| Hio 0x24 | `m24` | `mRndRotMinX` | daDaiocta_Eye_HIO_construct 0211E4F4 sets 0x4000; _execute 0211DCAC copies it into the eye |
| Hio 0x26 | `m26` | `mRndRotMinY` | the constructor sets 0x1000; copied into the eye |
| Hio 0x28 | `m28` | `mRndRotMinZ` | the constructor sets 0x1000; copied into the eye |

Open: daDaiocta_Eye_c `m295` (set to 1 only; tww the same), `m498` (set from shape_angle.y only; tww the same); Hio `m04`, `m08`, `m0C` and the padding.

## btd_hahen_s and btd_sibuki_s (btd_class::hahen[35], mBtdSibukiS[50]; d_a_btd.h)

Round 5 (two-digit offset names). Gohma's shell debris pieces (btd_hahen_s) and lava splashes (btd_sibuki_s): their active flag / state, position, speed and spin. Same meanings in zeldaret/tww d217e2b.

btd_hahen_s:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `m04` | `mActive` | btdActionBurst (in btdActionDamage, move 020F6004; tww damage) sets 1 on a free piece; btdExecDebrisMove (in daBtd_Execute 020F1494; tww hahen_move) moves only active pieces and clears it below y -100; daBtd_Draw 020F0A74 draws only active pieces (tww the same) |
| 0x08 | `m08` | `mPos` | btdActionBurst: a joint position plus a random offset; btdExecDebrisMove adds mSpeed every frame and puts the piece's model there (tww the same) |
| 0x14 | `m14` | `mSpeed` | btdActionBurst: 20..25 up and out, turned toward the piece's direction; btdExecDebrisMove adds it to mPos and lowers its y by 3 per frame (tww the same) |
| 0x20 | `m20` | `mAngleX` | btdActionBurst random; btdExecDebrisMove adds 0x900 per frame and turns the piece's model by it about x (tww the same) |
| 0x22 | `m22` | `mAngleY` | btdActionBurst random; btdExecDebrisMove adds 0x600 per frame and turns the piece's model by it about y (tww the same) |

btd_sibuki_s:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mState` | sibuki_set 020F10B4 sets 1 on a free splash (0); btdExecSplashMove (in daBtd_Execute 020F1494; tww sibuki_move) flies states 1 / 2 (1 -> 2 on the first collision update), counts states above 2 up to 30 and back to 0, and clears it when the splash lands or hits (tww the same) |
| 0x04 | `m04` | `mPos` | sibuki_set 020F10B4: a random point 300..500 around the boss; btdExecSplashMove adds mSpeed, follows Link for splash 0 with HIO mSplashFollow, and puts the attack sphere and the 0x8063 / 0x8064 effects there; daBtd_Delete 020F34A8 deletes the sound object at it (tww the same) |
| 0x10 | `m10` | `mSpeed` | sibuki_set 020F10B4: HIO mSplashSpeedF / mSplashSpeedY plus random, turned to a random direction; btdExecSplashMove lowers its y by HIO mSplashGravity and adds it to mPos (tww the same) |

Open (no name yet): padding `btd_hahen_s::m05`, `m24`, `btd_sibuki_s::m01`, `m1C`.


## daBmd_HIO_c (l_HIO(); d_a_bmd.h)

Round 8 (two-digit offset names). Kalle Demos' HIO: the chance to start a move, the custom environment time and how long the flower stays open. Same meanings in zeldaret/tww; in WWHD the vtable is last, so the offsets are 4 lower than the names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `m08` | `mMoveChance` | wait (d_a_bmd_move.cpp, inlined into daBmd_Execute 020AEBCC): every 30-80 frames Kalle Demos switches to mode 1 (Link within 1300) or 2 (farther) when cM_rndF(1) is below it; daBmd_HIO_c_ct 020B1CB4 0.8 (tww the same) |
| 0x08 | `m0C` | `mCustomTime` | bmd_kankyo (inlined into daBmd_Execute 020AEBCC): passed to dKy_custom_timeset every frame; daBmd_HIO_c_ct 020B1CB4 1.0 (tww the same) |
| 0x10 | `m14` | `mOpenTime` | damage (d_a_bmd_move.cpp, inlined into daBmd_Execute 020AEBCC): when the flower's open animation (HIRAKU) stops, mTimers[0] is set to it; sub mode 3 closes the flower (HANA_TOJIRU) when it runs out; daBmd_HIO_c_ct 020B1CB4 100 (tww m308[0] the same) |

Open (no name yet): `m05` (0) and `m10` (0.5), set by daBmd_HIO_c_ct 020B1CB4 and read nowhere in WWHD or tww; padding `_02`, `_12`.

## daBmdfoot_HIO_c (l_HIO; d_a_bmdfoot.h)

Round 8 (two-digit offset names). The Kalle Demos floor tentacle's HIO: the attack angle range and the raised pitch. Same meanings in zeldaret/tww; in WWHD the vtable is last, so the offsets are 4 lower than the names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x02 | `m06` | `mAttackAngle` | wait (inlined into daBmdfoot_Execute 020B5DC4): the tentacle attacks (Bmdfoot_Attack1) only when the angle to Link is below it; attack_1 (the same): the aim angle toward Link is clamped to +-it; daBmdfoot_HIO_c_ct 020B8458 3500 (tww the same) |
| 0x04 | `m08` | `mActivePitch` | daBmdfoot_Execute 020B5DC4: current.angle.x is eased to it while mAction is Wait / Attack1 / Attack2, to 0 otherwise; daBmdfoot_HIO_c_ct 020B8458 1000 (tww the same) |

Open (no name yet): `m05` (not even set by daBmdfoot_HIO_c_ct 020B8458; read nowhere in WWHD or tww), padding `_06`.

## daBmdhand_HIO_c and hand_s (l_HIO, bmdhand_class::mChain; d_a_bmdhand.h)

Round 8 (two-digit offset names). The Kalle Demos ceiling tentacle's HIO (ceiling offset, regrow time; in WWHD the vtable is last, so the offsets are 4 lower than the names) and its 20 chain segments (hand_s: position, velocity, line width). Same meanings in zeldaret/tww.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `m08` | `mCeilingOffsetY` | hand_ceiling_check (inlined into daBmdhand_Execute 020B8D9C through hand_calc / start_hand_calc): added to the y of the ceiling the upward line check found, giving mCeilingPos; daBmdhand_HIO_c_ct 020BAF50 -20 (tww the same) |
| 0x08 | `m0C` | `mRegrowTime` | hand_move (inlined into daBmdhand_Execute 020B8D9C), Bmdhand_Attached: a cut tentacle sets mTimers[0] to it; Bmdhand_Cut shrinks the hand below 100 and re-attaches it at 0; daBmdhand_HIO_c_ct 020BAF50 1000 (tww the same) |

hand_s:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mPos` | the tentacle segment's position: control1 / control2 (inlined into daBmdhand_Execute 020B8D9C) and cut_control 020B88C4 chain each segment to the previous one, control2 pulls segment 0x13 to mHandPos, the blood effect is set at it (tww the same) |
| 0x0C | `m0C` | `mSpeed` | cut_control 020B88C4: first a copy of the old position, then (new - old) * factor, added in the next step's position; the segment's velocity (tww the same) |
| 0x18 | `m18` | `mLineSize` | control3 020B882C and cut_control3 020B8C74 set it per segment (10.5, then tapering); hand_move (inlined into daBmdhand_Execute 020B8D9C) passes it to Bmdhand_setLineSize, the tentacle line's width (tww the same) |

Open (no name yet): daBmdhand_HIO_c padding `_01`, `_0A`.

# Field names: other actors

Offset-named fields of the `*_class` actors that are neither enemies nor bosses (objects, items, environment), named
in the field naming pass (2026-11-10; rules in docs/decomp-notes.md, "Field names (offset-named
fields)"). A name comes from how the original code uses the field (the address of the use and what it shows), from
names already in this decomp, or from the CC0 zeldaret/tww and zeldaret/tp decomps. Renames only: offsets, types and
code are unchanged (objcmp: object-identical). Each header declaration keeps the old name (`was mXXXX`) with the
evidence. Inlined GameCube functions are cited as `name (in Caller ADDR)`. Fields without enough evidence keep their
offset name and are listed as open.

## himo2_class (Grappling Hook rope; d_a_himo2.h; view player02::ropeMode_c in d_a_player_main_02.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names for every field of this class and TP has no grappling hook, so
every name comes from the WWHD uses (d_a_himo2.cpp, d_a_himo2_move.cpp). The state enums (`himo2_state_e`,
`himo2_demo_state_e`) were already named in this decomp. `REG` is a debug register term (l_himo2Regs). The view
`player02::ropeMode_c` (Link's checkNextActionRopeReady 023EC084 reads the rope state) takes the name `mState`.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03B8 | `m029C` | `mDemoTimer` | new_himo2_move 0216F7FC: counted down every frame; set 120/50/210/220/10 by the demo states (m24D9 SET_ROCK..BOSS_END), which act on frame numbers of it (30, 200, 170, 0) |
| 0x03BA | `m029E` | `mHideTimer` | daHimo2_Draw 0216CA5C draws nothing while it is not 0; new_himo2_move 0216F7FC: REG + 0x14 when the hook is back at the hand (states 8/9), 0 on lock-on and on throw; counted down |
| 0x03BC | `m02A0` | `mCancelWait` | new_himo2_move 0216F7FC: REG + 10 on throw, counted down; X/Y/Z cancels the flight (states 2/3) only while it is 0 |
| 0x03BE | `m02A2` | `mHitWait` | new_himo2_move 0216F7FC: 3 on throw, counted down; while it is not 0 himo2_bg_check 0216DE7C returns at once and At hits (ChkAtHit) are ignored |
| 0x03C0 | `m02A4` | `mControlEndWait` | new_himo2_move 0216F7FC: 20 when the swing-control demo (HIMO2_DEMO_CONTROL) starts; the demo ends on !checkPlayerFly() only once it is 0 |
| 0x03D0 | `m02B4` | `mRootPos` | daHimo2_Execute 0216E194: copy of mHandPos[0]; segments 0..mRootSegment of mSegments are set to it; himo2_draw (in daHimo2_Draw 0216CA5C) starts the rope line there |
| 0x03E8 | `m02CC` | `mRootSegment` | daHimo2_Execute 0216E194: segments 0..it of mSegments sit at mRootPos, himo2_control/himo2_control2 simulate from it + 1; new_himo2_move 0216F7FC sets 0x61 - distance * k (more rope out = lower), at least 2 |
| 0x03F4 | `m02D8` | `mFrameCounter` | daHimo2_Execute 0216E194: + 1 every frame; new_himo2_move 0216F7FC / himo2_control2 use it as the phase of the spin, sway and camera shake (it * 0x2000 etc.) |
| 0x03F8 | `m02DC` | `mState` | new_himo2_move 0216F7FC: switch over the rope states (himo2_state_e HIMO2_WAIT .. HIMO2_HANG); himo2_bg_check 0216DE7C sets HIMO2_REBOUND; daPy_lk_c::checkNextActionRopeReady 023EC084 reads it (view player02::ropeMode_c) |
| 0x03FC | `m02E0` | `mRockfallSeStep` | new_himo2_move 0216F7FC (HIMO2_DEMO_CONTROL): 0/1/2+ selects JA_SE_CV_DRG_ROCKFALL_1 / _1_2 / _1_3 every 50 frames, + 1 each time; 0 when the rock is dropped |
| 0x0400 | `m02E4` | `mSagY` | himo2_control (in daHimo2_Execute 0216E194) adds it to each segment's height difference (the rope's droop); new_himo2_move 0216F7FC: 0 on throw, -5 when wrapped round a body, -> -6.25 while hooked |
| 0x0408 | `m02EC` | `mHandPos` | daHimo2_Execute 0216E194: [0]/[1] = MtxPosition(4, 0, 0) of the left/right hand matrices, [0] the hand holding the rope (getRopeGrabRightHand); new_himo2_move 0216F7FC pulls the hook back to [0]; daHimo2_Draw 0216CA5C draws the coils at [1] |
| 0x0424 | `m0308` | `mStateTimer` | new_himo2_move 0216F7FC: counted down; set on each state change (throw, rebound 30/40, max length 50); at 0 the spin throws (state 1), the flight stops at max length (2) or pulls back (3); himo2_bg_check 0216DE7C sets 30 |
| 0x042C | `m0310` | `mSegments` | daHimo2_Execute 0216E194: the 100 rope segments (himo2_s), positions set from mRootPos, simulated by himo2_control/himo2_control2, [99] = the hook position; himo2_draw (in daHimo2_Draw 0216CA5C) draws them |
| 0x204C | `m1F30` | `mRopeLine` | CallbackCreateHeap 0216E9E0: mDoExt_3DlineMat1_c init(1, 200, rope texture); daHimo2_Draw 0216CA5C fills it with the rope points (spin_draw, himo2_draw, himo_hang_draw) and updates it |
| 0x21D4 | `m1F6C` | `mRopePointNum` | daHimo2_Draw 0216CA5C: 0 at the start, + 1 per point written into mRopeLine (spin_draw / himo2_draw / himo_hang_draw); the line is updated with it as the point count (nothing above 200) |
| 0x21D8 | `m1F70` | `mCoilRadiusRate` | daHimo2_Draw 0216CA5C: per coil i (5), -> 0 while mHandDist * (REG + 3) + REG - 120 is above (4 - i) * 300, else -> 1; the coil radius is it * 15, and [0] scales the line between the hands |
| 0x21EC | `m1F84` | `mOldCoilHandPos` | daHimo2_Draw 0216CA5C: mHandPos[1] of the previous frame; the movement mHandPos[1] - it sets the coil swing angles |
| 0x21F8 | `m1F90` | `mCoilAngleX` | daHimo2_Draw 0216CA5C: follows the hand movement (z) into the coils' X rotation (cLib_addCalcAngleS2, then mDoMtx_XrotM) |
| 0x21FA | `m1F92` | `mCoilFlipAngleZ` | daHimo2_Draw 0216CA5C: -> -0x8000 when the hand moves down (below REG - 20), else -> 0; added to the coils' Z rotation |
| 0x21FC | `m1F94` | `mCoilAngleZ` | daHimo2_Draw 0216CA5C: follows the hand movement (x) and is the per-coil step of the coils' Z rotation ((it - 2000) * i) |
| 0x2200 | `m1F98` | `mCoilLine` | CallbackCreateHeap 0216E9E0: mDoExt_3DlineMat1_c init(5, 0x20, rope texture); daHimo2_Draw 0216CA5C fills 5 circles of 32 points round mHandPos[1] (the coiled rope in the hand) |
| 0x2388 | `m1FD4` | `mHandLineSag` | daHimo2_Draw 0216CA5C: -> REG + 15 or 0 by the bits of the rope distance; lowers/widens the sine curve of mHandLine |
| 0x238C | `m1FD8` | `mHandLine` | CallbackCreateHeap 0216E9E0: mDoExt_3DlineMat1_c init(1, 0x10, rope texture); daHimo2_Draw 0216CA5C fills 16 points from mHandPos[1] to mHandPos[0] (the rope between the hands) |
| 0x2514 | `m2014` | `mStts` | daHimo2_Create 0216EC18: dCcD_Stts, Init(0xFF, 0xFF, this), the status of mSph |
| 0x2550 | `m2050` | `mSph` | daHimo2_Create 0216EC18: dCcD_Sph set from cc_sph_src; daHimo2_Execute 0216E194 places it at the hook (R 20 in flight, else -100); new_himo2_move 0216F7FC tests ChkAtHit |
| 0x267C | `m217C` | `mpTarget` | himo2_class::setTargetPos 0216EEB8 stores the target found by search_target; new_himo2_move 0216F7FC: the lock-on target on throw, the hooked actor (kui / body) whose health it sets |
| 0x2680 | `m2180` | `mTargetId` | new_himo2_move 0216F7FC: fopAcM_GetID(the lock-on target) on throw (else -1); states 3/4 look the actor up with fopAcM_SearchByID(it) |
| 0x2684 | `m2184` | `mWrapRadius` | new_himo2_move 0216F7FC (state 4, hooked on a body): the radius of the hook circling the target's eyePos (rotated by mFrameCounter << 13), -> 0; below 0.1 the hook is done |
| 0x2688 | `m2188` | `mHandDist` | new_himo2_move 0216F7FC: \|current.pos - mHandPos[0]\| every frame; sets mRootSegment and the segment length in himo2_control/himo2_control2 (in daHimo2_Execute 0216E194) |
| 0x268C | `m218C` | `mCandidates` | s_a_d_sub 0216DD3C appends each hook target found by fpcM_Search; search_target (in setTargetPos 0216EEB8) clears and walks them |
| 0x281C | `m24AC` | `mCandidateNum` | s_a_d_sub 0216DD3C: index of the next free mCandidates entry (assert < 100), + 1 per candidate; search_target (in setTargetPos 0216EEB8) clears it |
| 0x2AE4 | `m24B4` | `mWrapOffsetZ` | new_himo2_move 0216F7FC: REG - 400 when the wrap demo starts, -> -144 (HIMO2_DEMO_COIL); spin_draw (in daHimo2_Draw 0216CA5C): z of the wrap's start point relative to the target |
| 0x2AE8 | `m24B8` | `mWrapOffsetX` | new_himo2_move 0216F7FC: REG - 90 (body) or 0 when the wrap demo starts, -> 0 (HIMO2_DEMO_COIL); spin_draw (in daHimo2_Draw 0216CA5C): x of the wrap's start point relative to the target |
| 0x2AEC | `m24BC` | `mWrapCount` | new_himo2_move 0216F7FC (HIMO2_DEMO_COIL): wrap progress, + (mWrapSpeed / size) + 1 per frame up to 400, coil sounds at 0/13/26/39, hook sound past 49, wrap done past 0x82; spin_draw (in daHimo2_Draw 0216CA5C) bends segments up to it + 49 |
| 0x2AF0 | `m24C0` | `mOldWrapCount` | new_himo2_move 0216F7FC (HIMO2_DEMO_COIL): mWrapCount before this frame's step |
| 0x2AF4 | `m24C4` | `mWrapSpeed` | new_himo2_move 0216F7FC (HIMO2_DEMO_COIL): 0 at the start, + REG + 2 per frame; mWrapCount advances by it / (target size) + 1 |
| 0x2AF8 | `m24C8` | `mWrapAngleStep` | new_himo2_move 0216F7FC (HIMO2_DEMO_COIL): -> 4000 - 2000 * (target scale.y - 1); spin_draw (in daHimo2_Draw 0216CA5C) adds it per wrapped segment to the bend angle |
| 0x2AFA | `m24CA` | `mHookAngleX` | new_himo2_move 0216F7FC: 0 when the wrap starts, 9000 (with the hook sound) then 0x238C; spin_draw (in daHimo2_Draw 0216CA5C): X rotation of the hook model at the rope end |
| 0x2AFC | `m24CC` | `mHookTipPos` | spin_draw / himo2_draw (in daHimo2_Draw 0216CA5C): MtxPosition((0, 0, 15)) in the hook model's matrix, the hook's tip |
| 0x2B08 | `m24D8` | `mWrapDrawMode` | new_himo2_move 0216F7FC: 1 when hooked, 2 when the wrap is done, 0 on release; daHimo2_Draw 0216CA5C draws the wrap (spin_draw) when not 0, below 2 also its first 50 points |
| 0x2B09 | `m24D9` | `mDemoState` | new_himo2_move 0216F7FC: switch over the hook demos (himo2_demo_state_e HIMO2_DEMO_REQUEST .. BOSS_END); daHimo2_Draw 0216CA5C draws nothing while below HIMO2_DEMO_INACTIVE |
| 0x2B0C | `m24DC` | `mCamEye` | new_himo2_move 0216F7FC: the demo camera's eye (cameraSet / cameraReset e argument, with the shake), moved by the demo states |
| 0x2B18 | `m24E8` | `mCamCenter` | new_himo2_move 0216F7FC: the demo camera's center (cameraSet / cameraReset c argument, with the shake): the target, the player or the boss |
| 0x2B24 | `m24F4` | `mCamFovy` | new_himo2_move 0216F7FC: the f32 (fovy) argument of cameraSet 02514FE8 in the demos; 65 at the control demo, -> mCamFovyTarget |
| 0x2B28 | `m24F8` | `mCamFovyTarget` | new_himo2_move 0216F7FC: 65 at the control demo, moved by the sub stick Y (REG + 20 .. REG + 70); mCamFovy follows it |
| 0x2B2C | `m24FC` | `mCamEyeDist` | new_himo2_move 0216F7FC (HIMO2_DEMO_COIL): scale of the eye offset added to mCamEye each frame; REG + 3.5 (body) / REG + 3 at the start, -> REG + 1.8 / REG + 1.2 |
| 0x2B30 | `m2500` | `mReturnSpeedRate` | new_himo2_move 0216F7FC: scales the speed the hook returns to the hand (200 * it in HIMO2_WAIT, (REG + 400) * it in mv_pull_back), -> 1; 0 on throw, 0.1/0.02 on a hit / time out; daHimo2_Create 0216EC18 sets 1 |
| 0x2B34 | `m2504` | `mHangPos` | spin_draw (in daHimo2_Draw 0216CA5C): the wrap point REG + 48 (where the rope leaves the target); new_himo2_move 0216F7FC puts the hook there in HIMO2_HOOKED/HANG; himo_hang_draw draws the rope from it to mHandPos[0] |
| 0x2B40 | `m2510` | `mCamAngleY` | new_himo2_move 0216F7FC: target angle +-0x4000 at the wrap demo, player angle + 0x8000 at the control demo, -> mCamAngleYTarget; YrotS for the camera eye offset; spin_draw (in daHimo2_Draw 0216CA5C) uses it as the wrap's Y angle |
| 0x2B42 | `m2512` | `mCamAngleYTarget` | new_himo2_move 0216F7FC (control demo): set with mCamAngleY, + sub stick X * (REG + 1000) per frame; mCamAngleY follows it |
| 0x2B44 | `m2514` | `mCamEyeSignX` | new_himo2_move 0216F7FC: -1 at the wrap demo; multiplies the x of the eye offset (HIMO2_DEMO_COIL) |
| 0x2B48 | `m2518` | `mCamEyeSignZ` | new_himo2_move 0216F7FC: REG + 1 (body) or -1 at the wrap demo; multiplies the z of the eye offset (HIMO2_DEMO_COIL) |
| 0x2B4C | `m251C` | `mWrapMirror` | search_target (in setTargetPos 0216EEB8): 1 when the player is on the negative-angle side of a plain kui, else 0; new_himo2_move 0216F7FC: mCamAngleY = target angle - 0x4000 when set, + 0x4000 when not |
| 0x2B50 | `m2520` | `mCamShake` | new_himo2_move 0216F7FC: REG + 50 at the boss-fall demo, -> 0; sin/cos(mFrameCounter) * it shakes mCamEye/mCamCenter, cos * it * 7.5 is the camera bank |
| 0x2B54 | `m2524` | `mTargetPos` | himo2_class::setTargetPos 0216EEB8 stores its cXyz argument; new_himo2_move 0216F7FC: the lock-on target's eyePos, limited to REG * 100 + 1000 from the camera eye; the free throw flies to it |
| 0x2B60 | `m2530` | `mSwayAmp` | new_himo2_move 0216F7FC: REG + 4 on throw; himo2_control2 (in daHimo2_Execute 0216E194): sway amplitude it * (99 - i) * 0.01 of the flying rope (states 1-3) |
| 0x2B64 | `m2534` | `mAcchCir` | daHimo2_Create 0216EC18: dBgS_AcchCir, SetWall(30, 30), the circle of mAcch |
| 0x2BA4 | `m2574` | `mAcch` | daHimo2_Create 0216EC18: dBgS_ObjAcch set on current/old pos with mAcchCir; himo2_bg_check 0216DE7C CrrPos and the wall/roof/ground hit |

Open: `m1120` (0x123C, himo2_s[100]): its only use is the ground check in daHimo2_Execute 0216E194, which reads each
entry's position and writes the ground height + 5 into its m0C; nothing else reads or writes the array (the rope
segments are `mSegments`), so its purpose is not shown.

## kb_class (Pig; d_a_kb.h; views Ob1Pig in d_a_npc_ob1.h, PeopleMainPig / PeoplePigView / PeopleEventPig in d_a_npc_people_main.h / _exec.h / _event.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names for these fields and TP has no pig. `mTauraPos` and
`mTauraPosSet` are named after TWW's accessor `taura_pos_set(cXyz)` (d_a_kb.h:17), which sets m450 and m408; the other
names come from the WWHD uses (d_a_kb.cpp, d_a_kb_exec.cpp, d_a_kb_move.cpp). `mPigBit` (m405) takes the name the view
Ob1Pig already had for it. The daNpcPeople pig views take `mTauraPosSet` / `mTauraPos`.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x749 | `m401` | `mMoveRangePrm` | daKb_Create 02193E48: param bits 4-7 (0xFF -> 0; 3 for the Windfall path pigs); mMoveRange = it * 100 (0 -> 300) |
| 0x74B | `m403` | `mTgHitLatch` | pl_attack_hit_check (in daKb_Execute 02190CE0): 1 on the first frame of a Tg hit (the hit reaction runs only while it is 0), 0 once the hit is gone |
| 0x74C | `m404` | `mOnWaterGround` | daKb_Execute 02190CE0: 1 when the ground polygon's attribute is water (0x13), else 0 (not while carried); carry_move (in daKb_Execute) / swim_move set mTauraPos to the landing position only while it is 0; carry_check 0218EF4C clears it; swim_mode_change_check 0218F380 tests it |
| 0x74D | `m405` | `mPigBit` | daKb_Execute 02190CE0: 1 << (mShapeType & 3) while on ground of special code 4 on Outset (also ORed into event reg 0xBFFF), else 0; normal_move (in daKb_Execute) does not run at the player while it is set; the view Ob1Pig (d_a_npc_ob1.h) already calls it mPigBit |
| 0x74E | `m406` | `mDemoOwner` | esa_demo_check 02190708: 1 when this pig starts the bait demo (sets l_kbStatics.DEMO_START); daKb_Delete 02193778 and the demo end (esa_demo_move 0219509C) clear DEMO_START again only when it is set |
| 0x74F | `m407` | `mItemDropped` | money_drop 021907F4: 1 when a rupee/item was created; esa_demo_move 0219509C: 1 after digging up a daTagKbItem; picks the player's smile / other reaction (DEMO_SMILE) |
| 0x750 | `m408` | `mTauraPosSet` | TWW taura_pos_set(cXyz) (d_a_kb.h:17) sets it to 1 with mTauraPos; set/cleared by daNpcPeople (checkPig 022C3E78 tests it == 0 before tying the pig); no WWHD pig code reads it (normal_move notes the GameCube test became mbCanBeBigPig != 4) |
| 0x751 | `m409` | `mDigFound` | esa_demo_move 0219509C: 1 when the pig ends on ground of special code 5 (a dig spot, big pig) and digs; then it looks for the item tag (daTagKbItem::kb_dig) |
| 0x752 | `m40A` | `mDigSeDone` | esa_demo_move 0219509C: 0 in step 0x2C, 1 once the dig sound (JA_SE_CM_PG_DIG / _L_DIG) has played in step 0x2D |
| 0x753 | `m40B` | `mGrabMissed` | speed_pos_set 02194654: 1 while the player's grab-miss actor is this pig (animation stopped, no move), 0 after; normal_move (in daKb_Execute 02190CE0) does not turn while it is set |
| 0x765 | `m41D` | `mPathNo` | daKb_Create 02193E48: param bits 16-23, dPath_GetRoomPath(it) when not 0xFF (mpPath); the Windfall pigs start at a random point of that path |
| 0x766 | `m41E` | `mMode` | daKb_Execute 02190CE0: switch 0 normal_move, 1 carry_move, 2 swim_move, 3 attack_move, 4 esa_demo_move; carry_check 0218EF4C sets 1, esa_demo_check 02190708 sets 4 |
| 0x768 | `m420` | `mAction` | daKb_Execute 02190CE0: the step inside mMode (normal 0-6, carry 0xA-0xD, swim 0x14-0x19, attack 0x21-0x24, bait demo 0x28-0x31), switched on by normal_move/carry_move/swim_move, attack_move 021947AC and esa_demo_move 0219509C |
| 0x76A | `m422` | `mTargetAngleY` | normal_move / swim_move (in daKb_Execute 02190CE0): current.angle.y turns to it (cLib_addCalcAngleS2 with mTurnStep); the angle to mTargetPos, target_set 02190400 (case 2) |
| 0x76C | `m424` | `mTurnStep` | normal_move (in daKb_Execute 02190CE0): the step of the turn to mTargetAngleY, 0x800 walking, 0x2000 running away |
| 0x76E | `m426` | `mTimers` | daKb_Execute 02190CE0: 8 timers counted down every frame ([0] the action timer, [1] no-flee wait, [2] voice/struggle wait, [3] keep-direction wait (target_set 02190400), [4] bait-demo cool-down (esa_demo_check 02190708), [5] anger flash, [6] swim voice, [7] first ground check, 5 at daKb_Create 02193E48) |
| 0x77E | `m436` | `mFlipState` | carry_move (in daKb_Execute 02190CE0): 0 while thrown (shape_angle.x spins), 1 after the first bounce; swim_move: + 0x800 per frame, the phase of the bobbing (sin) |
| 0x780 | `m438` | `mFallFrame` | carry_move (in daKb_Execute 02190CE0): + 1 per frame the carried pig is not at mCarryStartY; at 4 the squeal particle (particle_tuba); carry_check 0218EF4C / swim_move clear it |
| 0x782 | `m43A` | `mSwimTurnWait` | swim_move (in daKb_Execute 02190CE0): 0 -> target_set(2) then 1; after a wall a random 5-25 turn; counted down while facing mTargetAngleY |
| 0x788 | `m440` | `mInWater` | sibuki_set 0218F1FC sets 1 (splash on entering water); carry_move (in daKb_Execute 02190CE0) lands in water (swim, mMode 2) when set; swim_move clears it when out; swim_mode_change_check 0218F380 tests it |
| 0x78A | `m442` | `mAttackAngleY` | carry_move (in daKb_Execute 02190CE0): the player angle when the angry run starts; attack_move 021947AC turns shape_angle.y to it and picks a new one (+- rnd 4096) when reached |
| 0x78C | `m444` | `mLandSquash` | carry_move (in daKb_Execute 02190CE0): 6 on landing (0x1E for mbCanBeBigPig 4); daKb_Execute counts it down and squashes mScale.y (+0.5625 above 3, +1.125 below); no move while above 3 |
| 0x790 | `m448` | `mStallFrames` | daKb_Execute 02190CE0: in attack (mMode 3, action >= 0x24) + 1 per frame the pig moves less than 1; above 10 the anger ends (ALL_ANGER 0, back to mMode 0); carry_move clears it |
| 0x792 | `m44A` | `mRepeatCount` | carry_move (in daKb_Execute 02190CE0): 3 struggle animations (JITA2) while carried, - 1 each; esa_demo_move 0219509C: the eat animation count (1) |
| 0x794 | `m44C` | `mAngerFlashPhase` | daKb_Execute 02190CE0: + REG + 1000 per frame while angry (mTimers[5] or actions 0x21-0x24); \|sin\| of it drives mAngerColorRate |
| 0x798 | `m450` | `mTauraPos` | TWW taura_pos_set(cXyz) (d_a_kb.h:17) sets it with mTauraPosSet (daNpcPeople 022C3E78); daKb_Execute 02190CE0 sets it to the first ground position; normal_move (in daKb_Execute) heads back when farther than mMoveRange / 2; target_set 02190400 (case 2) aims at it |
| 0x7A4 | `m45C` | `mTargetPos` | target_set 02190400: 1000 ahead in the chosen direction or the bait's position; normal_move (in daKb_Execute 02190CE0) walks towards it (mTargetAngleY) |
| 0x7EC | `m4A4` | `mTailPos` | nodeCallBack 0218F98C: MtxPosition of the J_PG_TAIL joint; he_set 0218F4EC starts the pig gas (ID_AK_SN_PIGGAS00) there |
| 0x7F8 | `m4B0` | `mAngerColorRate` | daKb_Execute 02190CE0: -> the anger flash value (from mAngerFlashPhase), else 0; daKb_Draw 0218FC3C blends the C0/K0 colours towards red by it |
| 0x7FC | `m4B4` | `mTargetSpeed` | normal_move (in daKb_Execute 02190CE0): 0 wait, 3 walk, 12 (6 big) run; speedF follows it with step mSpeedStep |
| 0x800 | `m4B8` | `mSpeedStep` | normal_move (in daKb_Execute 02190CE0): 2.5 / 2.0, the cLib_addCalc2 step of speedF towards mTargetSpeed |
| 0x804 | `m4BC` | `mCarryStartY` | carry_check 0218EF4C: current.pos.y when the pig is picked up; carry_move (in daKb_Execute 02190CE0) starts the fall reaction once the height differs |
| 0x808 | `m4C0` | `mDrawOffsetY` | draw_SUB 0218FABC: the model's y translation, -> mDrawOffsetYTarget (not in mMode 4); daKb_Create 02193E48 30; esa_demo_move 0219509C moves it (-33 dig / 30) |
| 0x80C | `m4C4` | `mDrawOffsetYTarget` | draw_SUB 0218FABC: mDrawOffsetY follows it; 30 normally, 20 while carried and falling (carry_move in daKb_Execute 02190CE0), 30 at sibuki_set 0218F1FC |
| 0x810 | `m4C8` | `mStruggleAnmSpeed` | carry_move (in daKb_Execute 02190CE0): play speed of the struggle animation BCK_JITA2, 1.8 at the start, - 0.6 per repeat (not below 0) |
| 0x814 | `m4CC` | `mMoveRange` | daKb_Create 02193E48: mMoveRangePrm * 100 (0 -> 300); normal_move (in daKb_Execute 02190CE0) returns to mTauraPos when farther than it / 2; 0 (no limit) after fleeing or being carried |
| 0x81C | `m4D4` | `mSizeRate` | daKb_Create 02193E48: 2 for the big pig (mShapeType + 8), else 0; mScale = it + 1, and it scales the collider, eye/attention height, ripples and smoke (daKb_Execute 02190CE0, sibuki_set 0218F1FC, smoke_set 0218F600) |
| 0x820 | `m4D8` | `mEsaId` | esa_demo_check 02190708: fopAcM_GetID(the bait (esa) actor); target_set 02190400 / esa_demo_move 0219509C look it up; carry_check 0218EF4C releases the bait and sets fpcM_ERROR_PROCESS_ID_e |
| 0x824 | `m4DC` | `mHitPos` | pl_attack_hit_check (in daKb_Execute 02190CE0): mSph.GetTgHitPosP(); attack_move 021947AC runs from it |
| 0x830 | `m4E8` | `mScale` | daKb_Create 02193E48: mSizeRate + 1 (x = y = z); draw_SUB 0218FABC MtxScale(x, y, x); daKb_Execute 02190CE0 squashes y on landing (mLandSquash) |
| 0x83C | `m4F4` | `mGroundAngle` | daKb_Execute 02190CE0: fopAcM_getGroundAngle; draw_SUB 0218FABC tilts the model by x/z; carry_move returns it to 0 |
| 0x842 | `m4FA` | `mAttackTilt` | attack_move 021947AC: target x/z of shape_angle while running angry (from the ground slope), shape_angle follows it |
| 0x8BC | `m50C` | `mBckNo` | anm_init 0218EB64: the started BCK resource index; normal_move / pl_attack_hit_check (in daKb_Execute 02190CE0) restart WAIT1/WALK1/RUN1 only when it differs |
| 0x8C0 | `m514` | `mCamEye` | esa_demo_move 0219509C: the bait demo camera's eye (copied into dCamera_Set's eye) |
| 0x8CC | `m520` | `mCamCenter` | set_cam_center (in esa_demo_move 0219509C): current.pos + (REG, 60 + REG, REG), the bait demo camera's center |
| 0x8D8 | `m53C` | `mCamFovy` | esa_demo_move 0219509C: 50, the fovy argument of dCamera_Set |
| 0x8DC | `m540` | `mRippleCb` | hamon_set 0218F150: the ripple (ID_AK_JN_HAMON00) emitter callback; mRate set from mRippleScale; carry_check 0218EF4C / swim_move end it |
| 0x8F0 | `m554` | `mSmokeCb` | smoke_set 0218F600 / smoke_set2 0218F71C: the smoke (ELEMENTSMOKE00) emitter callback; ended when the anger ends (daKb_Execute 02190CE0) |
| 0x910 | `m574` | `mDigSmokeCb` | smoke_set3 0218F868: the dig smoke emitter callback (onWindOff, scaled); esa_demo_move 0219509C ends it after digging |
| 0x930 | `m594` | `mGasCb` | he_set 0218F4EC: the pig gas (ID_AK_SN_PIGGAS00) follow callback at mTailPos; carry_move (in daKb_Execute 02190CE0) ends it |
| 0x944 | `m5A8` | `mShakeWaterCb` | swim_move (in daKb_Execute 02190CE0): the two shake-off-water (ID_AK_SN_PIGDASSUI) follow callbacks, frames 10/11 of BCK_DASSUI; pl_attack_hit_check ends them |
| 0x96C | `m5D0` | `mRippleScale` | hamon_set 0218F150: scale of the ripple; sibuki_set 0218F1FC / swim_mode_change_check 0218F380 set it from mSizeRate, swim_move 0.5 |
| 0x970 | `m5D4` | `mParticlePos` | he_set 0218F4EC / smoke_set 0218F600 / smoke_set2 0218F71C / smoke_set3 0218F868: positions the particles start at ([0] body/tail, [1] the dig spot) |
| 0x988 | `m5EC` | `mParticleAngle` | he_set 0218F4EC / smoke_set 0218F600 / attack_move 021947AC: angles for the particles at mParticlePos |

Open: `m340` (0x574, dKy_tevstr_c: only constructed in kb_class_ct 02193AC4), `m446` (0x78E: a random start value
counted up every frame in daKb_Execute, never read), `m968` (0xD00, dCcD_Sph: only constructed and destroyed),
`m44E` and `field_0x468` / `field_0x480` (no uses); `m410`, `m43C`, `m4D0` are unused byte gaps.

## kanban_class (cuttable sign; d_a_kanban.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names for these fields (only `mPhase` is named) and TP has no
such sign, so every name comes from the WWHD uses (d_a_kanban.cpp, d_a_kanban_exec.cpp). The mode values
(`KANBAN_MOTHER` .. `KANBAN_PARTS_SWIM`) were already named in this decomp. A "piece" is a kanban actor created
for a cut-off part (`mIsPart`); the "mother" is the sign it was cut from.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3AC | `m290` | `mIsPart` | daKanban_Create 0218CA20: 1 for a cut-off piece (param parts mask not 0, linked to its mother sign by parentActorID), 0 for the whole sign; useHeapInit 0218C874 then creates only the masked models |
| 0x3B0 | `m294` | `mPartsMask` | daKanban_Create 0218CA20: param >> 20 (0 -> 1, the whole sign); daKanban_Draw 0218A548 / useHeapInit 0218C874 draw / create model i for bit i; cut_point_check 0218AD44 removes the cut parts (0x7FE all pieces) |
| 0x3B4 | `m298` | `mWobbleStep` | mother_move / parts_move (in daKanban_Execute 0218B234): (it + 1) & 3 per frame of the landing wobble, the wobble ends at 0; cut_point_check 0218AD44 sets 0 or 1 |
| 0x3B6 | `m29A` | `mHammerHit` | mother_move (in daKanban_Execute 0218B234): 2 on a skull hammer hit, 1 when the sign is hammered into the ground (not CUT_TYPE_HAMMER_SIDESWING); parts_move flies farther when its mother's is 2 |
| 0x3B7 | `m29B` | `mRestored` | mother_return_move (in daKanban_Execute 0218B234): 1 when the sign is whole again after the Song of Passing return; chield_parts_move then deletes the pieces; 0 when cut (cut_point_check 0218AD44) or hammered |
| 0x3B8 | `m29C` | `mWaterEntered` | sea_water_check 0218A6F8: 1 after the first entry into water (ripple and splash only once) |
| 0x3BC | `m2A0` | `mWaterSinkY` | sea_water_check 0218A6F8: 7 in water for cut type 1, else 0; ret_keisan_move 0218B00C -> 0; daKanban_Execute 0218B234 lowers the position by it around fopAcM_posMove |
| 0x3C0 | `m2A4` | `mCylHeight` | daKanban_Create 0218CA20 105 (whole sign); cut_point_check 0218AD44 0 / 25; daKanban_Execute 0218B234 m5B0.SetH(it), and no cc move while it is 0 |
| 0x3C4 | `m2A8` | `mCylRadius` | daKanban_Create 0218CA20 50 (whole sign); cut_point_check 0218AD44 0 / 10; daKanban_Execute 0218B234 m5B0.SetR(it) |
| 0x3C8 | `m2AC` | `mGroundAngle` | mother_move (in daKanban_Execute 0218B234): fopAcM_getGroundAngle on landing; daKanban_Draw 0218A548 tilts the models by x/z |
| 0x3CE | `m2B2` | `mTimers` | daKanban_Execute 0218B234: 5 timers counted down every frame ([0] the return delay, rnd 30, return_move; [1] keeps the pivot-y shift off while hammered in; [4] shock_damage_check 0218AB94 cool-down 100) |
| 0x3D8 | `m2BC` | `mBobPhase` | sea_water_check 0218A6F8: + REG + 1000 per frame in water, sin(it) bobs the height; 0 on water entry |
| 0x3DA | `m2BE` | `mMode` | daKanban_Execute 0218B234: switch KANBAN_MOTHER / _RETURN / _SWIM, KANBAN_PARTS / _RETURN / _SWIM; set by daKanban_Create 0218CA20 and tact_return |
| 0x3DC | `m2C0` | `mAction` | the step inside mMode: mother_move 10-14 / 0x14-0x15, mother_return_move 30-33, parts_move 0x6E-0x78, chield_parts_move 0x82-0x85, swim 0x28 / 0x8C (in daKanban_Execute 0218B234); cut_point_check 0218AD44 sets 0xB |
| 0x3DE | `m2C2` | `mCutFlags` | cut_point_check 0218AD44: bit 1 << mCutDir for each cut direction already done (a direction cuts once); mother_move (in daKanban_Execute 0218B234) reads 8 / 0x10 for the hammer depth; 0 when restored |
| 0x3E0 | `m2C4` | `mCutDir` | mother_move (in daKanban_Execute 0218B234): the cut direction from the player's cut type (pl_cut_no_dt), 4 for a non-sword hit; cut_point_check 0218AD44 picks the cut parts by it (cut_parts_arg_data[it * 8]); daKanban_Create 0218CA20: a piece's current.angle.z |
| 0x3E2 | `m2C6` | `mWobbleAngle` | mother_move / parts_move (in daKanban_Execute 0218B234): 0x800 on landing, shape_angle.x -> it + 0x4000 and it ^ 0xFF00 per frame (the wobble), 0x400 / 0 as mWobbleStep advances |
| 0x3E4 | `m2C8` | `mMotherId` | daKanban_Create 0218CA20: the piece's parentActorID (fpc_ProcID of the mother sign); parts_move / chield_parts_move (in daKanban_Execute 0218B234) look the mother up by it |
| 0x3E8 | `m2CC` | `mModelOffset` | daKanban_Draw 0218A548: translation of the models after the rotation (minus mPivotX/Y/Z); y 70 for a piece (daKanban_Create 0218CA20, cut_point_check 0218AD44); return_move (in daKanban_Execute 0218B234) clears it |
| 0x3F4 | `m2D8` | `mCreatePos` | daKanban_Create 0218CA20: current.pos at creation; not read |
| 0x400 | `m2E4` | `mPivotX` | daKanban_Draw 0218A548: x of the rotation pivot (added before, subtracted after the rotations); return_move (in daKanban_Execute 0218B234) clears it |
| 0x404 | `m2E8` | `mPivotY` | daKanban_Draw 0218A548: y of the rotation pivot; 140 for a piece (cut_point_check 0218AD44, daKanban_Create 0218CA20); daKanban_Execute 0218B234 shifts the position by it around fopAcM_posMove |
| 0x408 | `m2EC` | `mPivotZ` | daKanban_Draw 0218A548: z of the rotation pivot; return_move (in daKanban_Execute 0218B234) clears it |
| 0x40C | `m2F0` | `mHomeAngle` | daKanban_Create 0218CA20: current.angle (a piece copies its mother's); return_angle / mother_return_move (in daKanban_Execute 0218B234) turn back to it |
| 0x414 | `m2F8` | `mHomePos` | daKanban_Create 0218CA20: current.pos (a piece copies its mother's); ret_keisan_move 0218B00C moves back to it; mother_move (in daKanban_Execute 0218B234) sinks to it.y - 90/65/20 when hammered |
| 0x420 | `m304` | `mCreateModelOffset` | daKanban_Create 0218CA20: a piece's mModelOffset at creation; not read |
| 0x42C | `m310` | `mAcchCir` | daKanban_Create 0218CA20: dBgS_AcchCir, SetWall(20, 30), the circle of mAcch; daKanban_Execute 0218B234 SetWall(0, 40) |
| 0x46C | `m350` | `mAcch` | daKanban_Create 0218CA20: dBgS_ObjAcch with mAcchCir, water check offset 300; wall/ground/water checks in mother_move/parts_move (in daKanban_Execute 0218B234) and sea_water_check 0218A6F8 |
| 0x630 | `m514` | `mRippleCb` | sea_water_check 0218A6F8: the ripple (ID_AK_JN_HAMON00) emitter callback at mRipplePos; daKanban_Delete 0218C824 / the return end it |
| 0x644 | `m528` | `mRipplePos` | sea_water_check 0218A6F8: current.pos with y = the water surface, where the ripple is set |
| 0x660 | `m544` | `mpModels` | useHeapInit 0218C874: the 11 part models (kanban_bdl[i]); daKanban_Draw 0218A548 draws those in mPartsMask |
| 0x68C | `m574` | `mStts` | daKanban_Create 0218CA20: dCcD_Stts, Init(0xFF, 0, this), the status of mCyl; daKanban_Execute 0218B234 moves by its m_cc_move |
| 0x6C8 | `m5B0` | `mCyl` | daKanban_Create 0218CA20: dCcD_Cyl from cyl_src; mother_move (in daKanban_Execute 0218B234) cuts on its Tg hits; SetC/SetH(mCylHeight)/SetR(mCylRadius) |

`m534` (0x650) is an unused byte gap and stays.

## himo3_class (hanging rope; d_a_himo3.h; views Link05RopeHD in d_a_player_main_05.h, Link07::Rope3 in d_a_player_main_07.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names for these fields and TP has no such rope. `mPlayerMoveLength`
(m15FC) is named after TWW's accessor `getPlayerMoveLength()` (d_a_himo3.h:30), which returns it; `mActorHangPos` /
`mActorHangAngle` (m21F4 / m2200) after `setActorHang(cXyz, s16)` (d_a_himo3.h:32), which sets them; `mGa` (m2114)
after its type `h3_ga_s` and TWW's `ga_draw` / `ga_move`. The other names come from the WWHD uses (d_a_himo3.cpp).
`mGlowMtx` and `mGlowSize` have no reader in HD (the GameCube draw passed them to the alpha model, which HD dropped).
Link's two rope views take `mPlayerMoveLength` (procRopeSwing_init 0241D040, procRopeDown_init 02436780).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03B4 | `m0298` | `mType` | daHimo3_Create 02174730: param & 0xFF (0xFF -> 0); 0 = torch (fire, light, ga, model hook_bmd[0]), 1 = Link's hook model (hook_bmd[1] from Link), 2..4 Himo3 models, 0xF = the Bgn rope (no end model, Bgn texture, cylinder, 200 segments); daHimo3_Draw 021727D4 / daHimo3_Execute 02172A00 branch on it |
| 0x03B5 | `m0299` | `mPathSpeed` | daHimo3_Create 02174730: param >> 16; path_move (in daHimo3_Execute 02172A00) eases speedF to it when the path point's arg3 is 0 or 0xFF |
| 0x03B6 | `m029A` | `mPathNo` | daHimo3_Create 02174730: param >> 24 (0xFF for type 0xF); dPath_GetRoomPath(it, room) when not 0xFF |
| 0x03B7 | `m029B` | `mPathOn` | daHimo3_Create 02174730: mPathNo + 1 when a path was found; daHimo3_Execute 02172A00 runs path_move only while it is not 0 |
| 0x03B8 | `m029C` | `mPathPntNo` | path_move (in daHimo3_Execute 02172A00): the current path point, + mPathDir per point, wraps (closed path) or turns at the ends; mPathTargetPos and the point's arg3 speed come from it |
| 0x03B9 | `m029D` | `mPathDir` | path_move (in daHimo3_Execute 02172A00): step of mPathPntNo, 1 / -1 (turned at the ends of an open path); daHimo3_Create 02174730 sets 1 |
| 0x03C0 | `m02A4` | `mPathMode` | path_move (in daHimo3_Execute 02172A00): switch: 0 takes the next point (sets mPathTargetPos, then 1), 1 moves toward it (back to 0 when nearer than 2 * speedF) |
| 0x03C4 | `m02A8` | `mPathTargetPos` | path_move (in daHimo3_Execute 02172A00): position of path point mPathPntNo; the actor moves toward it |
| 0x03D0 | `m02B4` | `mTimers` | daHimo3_Execute 02172A00 (type 0): 3 timers counted down every frame; [0] reloads 5..15 and picks a new mGlowSizeTarget, [1] reloads 3..9 |
| 0x03D6 | `m02BA` | `mFrameCounter` | daHimo3_Execute 02172A00: + 1 every frame; himo3_control (in daHimo3_Execute 02172A00) uses it as the phase of the wind sway (it * 500) and to space the 5 collision spheres along the rope |
| 0x03D8 | `m02BC` | `mSphOffTimer` | himo3_control (in daHimo3_Execute 02172A00): 0x1E on release (param 3), counted down by daHimo3_Execute 02172A00; the rope spheres get radius 30 only while nothing hangs and it is 0, else -200 |
| 0x03DA | `m02BE` | `mHangType` | himo3_control (in daHimo3_Execute 02172A00): 0 free, 1 the player hangs (param 1: hand position, shape_angle), 2 an actor hangs (param 2: mActorHangPos / mActorHangAngle); 0 on param 3; path_move stops while it is not 0 |
| 0x03DC | `m02C0` | `mSegments` | himo3_control (in daHimo3_Execute 02172A00): the 200 rope segments (himo3_s), [0] = current.pos, simulated up to mSegmentNum and copied into the line material; daHimo3_Create 02174730 clears them |
| 0x1828 | `m15C0` | `mSegmentNum` | daHimo3_Create 02174730: (param >> 8) & 0xFF, 200 for type 0xF (error above 200); himo3_control (in daHimo3_Execute 02172A00) loops over it; daHimo3_Draw 021727D4 updates the line with it as the point count |
| 0x18DC | `m15C4` | `mOldHangPos` | himo3_control (in daHimo3_Execute 02172A00): the hang point of the previous frame (-23535 y while nothing hangs, then the first hang point); hang point - it drives mSwingX / mSwingZ |
| 0x18E8 | `m15D0` | `mSwingZ` | himo3_control (in daHimo3_Execute 02172A00): while hanging eased to -10 * the hang point's local z movement, else a damped spring with mSwingVelZ; local z of the rope's bend offset (spA0.z) |
| 0x18EC | `m15D4` | `mSwingVelZ` | himo3_control (in daHimo3_Execute 02172A00): while free, added to mSwingZ every frame and reduced by mSwingZ * 0.003 (spring) |
| 0x18F0 | `m15D8` | `mSwingX` | himo3_control (in daHimo3_Execute 02172A00): as mSwingZ for the local x movement; local x of the rope's bend offset (spA0.x) |
| 0x18F4 | `m15DC` | `mSwingVelX` | himo3_control (in daHimo3_Execute 02172A00): as mSwingVelZ for mSwingX |
| 0x18F8 | `m15E0` | `mLineFollowRate` | himo3_control (in daHimo3_Execute 02172A00): the line points follow the segments at 100 * it per frame (copied when >= 0.999), eased to 1; 0 on release, 1 when a hang starts; daHimo3_Create 02174730 sets 1 |
| 0x18FC | `m15E4` | `mSwingStepRate` | himo3_control (in daHimo3_Execute 02172A00): step of the mSwingX / mSwingZ ease (10 * it); -> 1 while the hang point moves more than 1, else -> 0.1 |
| 0x1900 | `m15E8` | `mWindSway` | himo3_control (in daHimo3_Execute 02172A00): eased to the wind power (half while hanging); amplitude of the sway along the wind direction (sin(mFrameCounter * 500) * 100 * it) |
| 0x1908 | `m15F0` | `mGustSway` | himo3_control (in daHimo3_Execute 02172A00): eased to mGustSwayTarget; amplitude of the sway along mGustAngleY (sin(mGustPhase * 500) * 100 * it) |
| 0x190C | `m15F4` | `mGustSwayTarget` | daHimo3_Execute 02172A00: 1 when mSph is hit by an AT_TYPE_WIND attack; himo3_control (in daHimo3_Execute 02172A00) eases it back to 0 and mGustSway to it |
| 0x1910 | `m15F8` | `mGustAngleY` | daHimo3_Execute 02172A00: the player's shape_angle.y at the wind hit; himo3_control (in daHimo3_Execute 02172A00) sways the rope in that direction |
| 0x1912 | `m15FA` | `mGustPhase` | himo3_control (in daHimo3_Execute 02172A00): + 1 per frame while mGustSway > 0.01, else 0; phase of the gust sway (it * 500) |
| 0x1914 | `m15FC` | `mPlayerMoveLength` | tww accessor himo3_class::getPlayerMoveLength() returns it; daHimo3_Create 02174730: mSegmentNum * scale - 50 (- 50 more for type 0); Link's procRopeSwing_init 0241D040 / procRopeDown_init 02436780 limit the climb with it |
| 0x1918 | `m1600` | `mFireLight` | daHimo3_Execute 02172A00 (type 0): LIGHT_INFLUENCE at mEndPos, colour 600/400/120, power 150 * mFireLightRate; daHimo3_Create 02174730 dKy_plight_set, daHimo3_Delete 021743F0 dKy_plight_cut |
| 0x193C | `m1620` | `mFireLightRate` | daHimo3_Execute 02172A00: 1 when the torch fire emitter is created, eased to 1..1.2 (flicker) while it exists, 0 without it; mFireLight power = 150 * it |
| 0x1940 | `m1624` | `mEndPos` | daHimo3_Execute 02172A00: MtxPosition of the end model's matrix (offset, scale); centre of mSph, the torch fire and heat haze, mFireLight; mAcch position (daHimo3_Create 02174730); ga_move (in daHimo3_Execute 02172A00) flies around it |
| 0x194C | `m1630` | `mOldEndPos` | daHimo3_Execute 02172A00: mEndPos of the previous frame (copied at the end); the fire direction is (mEndPos - it) * -0.03; daHimo3_Create 02174730 mAcch old position |
| 0x1B5C | `m1840` | `mGlowMtx` | daHimo3_Execute 02172A00: end model matrix moved to the flame (10, -140, -15 for type 0) and scaled by mGlowScale; HD has no reader (the GameCube daHimo3_Draw passes it to dComIfGd_setAlphaModel / setSpotModel, tww d_a_himo3.cpp) |
| 0x1B8C | `m1870` | `mGlowScale` | daHimo3_Execute 02172A00: himo3HIO.m18 every frame; scales mGlowMtx |
| 0x1B94 | `m1878` | `mGlowSize` | daHimo3_Execute 02172A00: eased to mGlowSizeTarget; HD has no reader (the GameCube daHimo3_Draw passes it to dComIfGd_setAlphaModel as the size, tww d_a_himo3.cpp) |
| 0x1B98 | `m187C` | `mGlowSizeTarget` | daHimo3_Execute 02172A00: random 4..20 each time mTimers[0] runs out; mGlowSize eases to it |
| 0x2414 | `m20FC` | `mFireEcallBack` | daHimo3_Execute 02172A00: follow callback of the torch fire (dPa_name ID_AK_JN_TORCH at mEndPos); its emitter gets the direction and scale; himo3_class_ct 0217447C constructs it, daHimo3_Delete 021743F0 removes it |
| 0x2428 | `m2110` | `mHioOwner` | daHimo3_Create 02174730: written (0) when this actor created the l_HIO child; daHimo3_Delete 021743F0 deletes the HIO child only while it is not 0 |
| 0x242C | `m2114` | `mGa` | daHimo3_Draw 021727D4 / ga_move (in daHimo3_Execute 02172A00): the h3_ga_s (model Himo3 H3_GA, type 0 only, created in useHeapInit (in daHimo3_Create 02174730)) that flies around mEndPos; named after its type h3_ga_s and tww ga_draw / ga_move |
| 0x2624 | `m21F4` | `mActorHangPos` | setActorHang 02174F44 (tww himo3_class::setActorHang(cXyz, s16)) stores the position; himo3_control (in daHimo3_Execute 02172A00) uses it as the hang point for mHangType 2 |
| 0x2630 | `m2200` | `mActorHangAngle` | setActorHang 02174F44 stores the angle; himo3_control (in daHimo3_Execute 02172A00) turns the rope (current.angle.y) to it for mHangType 2 |

Open: `m2111` (0x2429: no use in HD; in the GameCube demo build it held the HIO flag, tww DEMO_SELECT) and `m2144`
(0x245C, dKy_tevstr_c: only constructed in himo3_class_ct 0217447C; the draw uses the actor's tevStr).

## mflft_class (rope-hung lava lift; d_a_mflft.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names for these fields (only `mPhase`, `mpModel`, `mStts`, `mCyls`,
`pm_bgw` and `mLineMat` are named) and TP has no such lift, so every name comes from the WWHD uses (d_a_mflft.cpp). The
lift hangs on three ropes (indices 0..2 of the rope arrays); cutting all three drops it into the lava.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B4 | `m298` | `mFrameCounter` | mflft_move (in daMflft_Execute 021C2748): + 1 every frame; phase of the wobble and sway (cM_ssin(it * 0x5DC) etc.); himo_move (in daMflft_Execute 021C2748) phase of the rope shake |
| 0x3B6 | `m29A` | `mMode` | mflft_move (in daMflft_Execute 021C2748): switch: 0 hangs on its 3 ropes, 1 falls (set when all three are cut) and lands on mFallGroundY; mflft_class::setLiftUp 021C3ECC carries the lift only in mode 1 or within 200 of the given height |
| 0x3BC | `m2A0` | `mSizeType` | daMflft_Create 021C3AC0: param & 0xFF (0xFF -> 0); 1 / 2 / 3 scale the lift (x, z) to 0.9 / 0.8 / 0.7, else 1.0 |
| 0x3BD | `m2A1` | `mSwitchNo` | daMflft_Create 021C3AC0: param >> 24; when that switch is on the ropes start cut and the lift on the ground; mflft_move (in daMflft_Execute 021C2748) turns it on when all three ropes are cut (0xFF none) |
| 0x3C0 | `m2A4` | `mSwayOffset` | mflft_move (in daMflft_Execute 021C2748): (mSwayX * sin, 0, mSwayZ * sin); current.pos = home.pos + it while hanging |
| 0x3CC | `m2B0` | `mSwayX` | ride_call_back 021C2064: raised toward the rider's x movement * 5 (above 10); mflft_move (in daMflft_Execute 021C2748): amplitude of mSwayOffset.x, eased back to 0 |
| 0x3D4 | `m2B8` | `mSwayZ` | ride_call_back 021C2064: raised toward the rider's z movement * 5 (above 10); mflft_move (in daMflft_Execute 021C2748): amplitude of mSwayOffset.z, eased back to 0 |
| 0x3D8 | `m2BC` | `mWobbleAmpX` | mflft_move (in daMflft_Execute 021C2748): amplitude of mWobbleAngle.x, eased to 100 / 2000 (lifted up) / 400 / 2500 (fallen); 2000 on landing and on a rope cut (himo_move); ride_call_back 021C2064 raises it with the rider's z movement * 50 |
| 0x3E0 | `m2C4` | `mWobbleAmpZ` | mflft_move (in daMflft_Execute 021C2748): amplitude of mWobbleAngle.z, as mWobbleAmpX; ride_call_back 021C2064 raises it with the rider's x movement * 50 |
| 0x3E4 | `m2C8` | `mWobbleAngle` | mflft_move (in daMflft_Execute 021C2748): x / z = mWobbleAmpX / mWobbleAmpZ * sin(mFrameCounter * k); shape_angle = current.angle + it |
| 0x3EA | `m2CE` | `mLiftUpTimer` | mflft_class::setLiftUp 021C3ECC: 5 (a shock when it was 0); daMflft_Execute 021C2748 counts it down; while not 0 the bg matrix is scaled 5 in y, the wobble goes to 2000 / 2500 and the magma effect starts |
| 0x3EC | `m2D0` | `mLiftUpRate` | mflft_class::setLiftUp 021C3ECC: current.pos.y follows the lift height with step 1000 * it, it eased to 1; daMflft_Execute 021C2748 sets 0 while mLiftUpTimer is 0 |
| 0x3F0 | `m2D4` | `mRopeCut` | himo_move (in daMflft_Execute 021C2748): [i] = 1 when rope i's hit points run out (then himo_cut_control lets it hang); mflft_move (in daMflft_Execute 021C2748) collects the cut ropes (himo_off_check, 7 = all); daMflft_Create 021C3AC0 sets all with mSwitchNo |
| 0x3F4 | `m2D8` | `mRopeLiftPos` | daMflft_Execute 021C2748: rope i's end on the lift (MtxPosition of (xd[i], 15, zd[i]) * scale in the lift frame); himo_move (in daMflft_Execute 021C2748) draws the rope from it and puts mCyls[i] there |
| 0x418 | `m2FC` | `mRopeTopPos` | daMflft_Execute 021C2748: rope i's top end, mRopeLiftPos with y = home.pos.y + 2000 (set while mRopeTopInitTimer runs); himo_move (in daMflft_Execute 021C2748) draws the rope up to it |
| 0x808 | `m6EC` | `mRopeHitTimer` | himo_move (in daMflft_Execute 021C2748): [i] = 15 when rope i is hit (only while below 10), amplitude of its shake; daMflft_Execute 021C2748 counts it down |
| 0x80E | `m6F2` | `mRopeHp` | daMflft_Create 021C3AC0: 3 per rope; himo_move (in daMflft_Execute 021C2748) subtracts the hit's damage (4 above 1), the rope is cut at 0 or below |
| 0x811 | `m6F5` | `mCutCountTimer` | himo_move (in daMflft_Execute 021C2748): 0x14 on a rope cut; mflft_move (in daMflft_Execute 021C2748) counts it down and at 0 (some rope cut) adds 1 to event reg 0xA507 (up to 6) |
| 0x812 | `m6F6` | `mRopeTopInitTimer` | daMflft_Create 021C3AC0: 10; daMflft_Execute 021C2748 counts it down and sets mRopeTopPos while it is not 0 |
| 0x814 | `m6F8` | `mTiltAngleY` | mflft_move (in daMflft_Execute 021C2748): eased to himo_off_ya[cut ropes]; daMflft_Execute 021C2748 tilts the lift by mTiltX about this direction; himo_cut_control 021C23E0 hangs the cut ropes in it |
| 0x816 | `m6FA` | `mTiltX` | mflft_move (in daMflft_Execute 021C2748): eased to -0x3000 while ropes are cut (0 when himo_off_ya is 1), back to 0 when falling / landed; daMflft_Execute 021C2748 XrotM by it |
| 0x81C | `m700` | `mBgMtx` | daMflft_Execute 021C2748: the lift matrix (y scaled 5 while mLiftUpTimer runs); CallbackCreateHeap 021C395C: cBgW_Set(pm_bgw, dzb, MOVE_BG, it) |
| 0x850 | `m734` | `mFallGroundY` | mflft_move (in daMflft_Execute 021C2748): ground (lava check) height under the lift when all ropes are cut; the falling lift stops at it + 80 |
| 0x9DC | `m774` | `mpMagmaEmitter` | eff_cont (in daMflft_Execute 021C2748): the ID_AK_SN_MAGMAISLAND01 emitter (scale 1.8) under the lift, moved with it in state 2, invalidated in state 3 |
| 0x9E0 | `m778` | `mMagmaEffState` | eff_cont (in daMflft_Execute 021C2748): 0 none, 1 create mpMagmaEmitter (-> 2, or 0 on failure), 2 follow, 3 end (-> 0); mflft_move (in daMflft_Execute 021C2748) sets 1 when mLiftUpTimer starts and 3 when it ends |

`m2A2`, `m2B4`, `m2C0`, `m2D7`, `m6FC` and `m779` are unused byte gaps and stay.

## kn_class (crab; d_a_kn.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names for these fields (only `mPhase`, `mpMorf`, `mAcchCir` and `mAcch`
are named) and TP has no crab, so every name comes from the WWHD uses (d_a_kn.cpp). The mode values `Mode_10_e` ..
`Mode_19_e` were already named in this decomp.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D9 | `m2BD` | `mMode` | kn_move (in daKN_Execute 021A23B8): switch over Mode_10_e .. Mode_19_e (wait, walk, turn home, burrow, emerge, flee); daKN_Create 021A30C0 starts in Mode_10_e; daKN_Execute 021A23B8 checks it for the water ground |
| 0x3DA | `m2BE` | `mOnWater` | daKN_Execute 021A23B8: 1 when the ground under the walking crab (Mode_13_e / Mode_19_e) is a water polygon; kn_move (in daKN_Execute 021A23B8) then splashes (shibuki_set) and burrows; 0 when a walk or flight starts |
| 0x3DB | `m2BF` | `mSandBurrow` | kn_move (in daKN_Execute 021A23B8): 1 when the fleeing crab burrows with smoke (wall hit or too far from mHomePos), 0 when it burrows in water; picks smoke_set or shibuki_set on emerging and ends mSmokeEcallBack |
| 0x3DC | `m2C0` | `mBurrowed` | kn_move (in daKN_Execute 021A23B8): 0 when burrowing starts (Mode_15_e), 1 once mBurrowY reached the depth (-7, -11 for the big crab); emerging waits for it |
| 0x3DE | `m2C2` | `mTimers` | daKN_Execute 021A23B8: 4 timers counted down every frame; kn_move (in daKN_Execute 021A23B8): [0] walk time (20..60) / delay before sinking (20), [1] walk-back steps (20 / 8), [2] 40 after emerging (no flight, walk home), [3] minimum time burrowed (30..60) |
| 0x3E6 | `m2CA` | `mWaitLoops` | kn_move (in daKN_Execute 021A23B8): 1..3 at the start of the wait (Mode_10_e), - 1 per wait animation loop (frame 43), the walk starts at 0 |
| 0x3E8 | `m2CC` | `mBurrowY` | kn_move (in daKN_Execute 021A23B8): eased to -7 (-11 for scale above 1.5) while burrowing, back to 0 on emerging; daKN_Execute 021A23B8 lowers the model by it, BG_check (in daKN_Execute 021A23B8) checks the ground without it; daKN_Draw 021A1FA0 draws nothing while it is below 0 |
| 0x3EC | `m2D0` | `mTargetAngleY` | kn_move (in daKN_Execute 021A23B8): random near current.angle.y, the angle home (mHomePos) or away from the player; current.angle.y turns to it with step mTurnStep |
| 0x3EE | `m2D2` | `mTurnStep` | kn_move (in daKN_Execute 021A23B8): 0x800 for wait / walk, 0x100 for the flight; step of the turn to mTargetAngleY |
| 0x3F8 | `m2DC` | `mAnmSpeed` | kn_move (in daKN_Execute 021A23B8): eased to mAnmSpeedTarget; the morf's play speed |
| 0x3FC | `m2E0` | `mAnmSpeedTarget` | kn_move (in daKN_Execute 021A23B8): 1, or 2 while the turn to mTargetAngleY is above 0x100 (walk modes) |
| 0x400 | `m2E4` | `mTargetSpeed` | kn_move (in daKN_Execute 021A23B8): mWalkSpeed, its negative (walking back) or 0; speedF is eased to it |
| 0x404 | `m2E8` | `mWalkSpeed` | kn_move (in daKN_Execute 021A23B8): 3 or -3 (sideways either way, 3 when heading home), 40 for the flight |
| 0x408 | `m2EC` | `mHomePos` | daKN_Create 021A30C0: current.pos at creation; kn_move (in daKN_Execute 021A23B8) walks back toward it when farther than 200 and burrows when the flight takes it farther |
| 0x414 | `m2F8` | `mGroundAngle` | daKN_Execute 021A23B8: fopAcM_getGroundAngle on ground hits; the model is rotated by its x / z |
| 0x41C | `m300` | `mSmokePos` | kn_move (in daKN_Execute 021A23B8): current.pos (+ 20 in y on emerging) before smoke_set; smoke_set 021A2160 emits ID_AK_JT_ELEMENTSMOKE01 there |
| 0x430 | `m314` | `mSmokeEcallBack` | smoke_set 021A2160: the smoke's dPa_smokeEcallBack (ended before a new smoke); daKN_Create 021A30C0 constructs it, daKN_Delete 021A2F80 removes it |
| 0x450 | `m334` | `mpSmokeEmitter` | smoke_set 021A2160: the ID_AK_JT_ELEMENTSMOKE01 emitter (max frame 0x19, rate 3, lifetime 25 set on it) |

Round 2:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D0 | `m2B4` | `mType` | daKN_Create 021A30C0 stores param bits 0-7; nothing in WWHD reads it; tww daKN_Create / daKN_Execute (demo build only): 1 = crab spawner (oya_kn_move, no model, returns before the move code), 2 = spawned crab (starts buried at -25, tilted) |
| 0x3D1 | `m2B5` | `mSpawnCountPrm` | daKN_Create 021A30C0 stores param bits 8-15; nothing in WWHD reads it; tww daKN_Create (demo build only): the spawner's crab count, copied to mSpawnLeft (0 / 0xFF -> 100) |
| 0x3D8 | `m2BC` | `mIsSpawner` | no use in WWHD (retail); tww demo build: daKN_Create sets 1 for type 1, else 0; daKN_Execute switches on it: 0 kn_move, 1 oya_kn_move |
| 0x3F0 | `m2D4` | `mSpawnLeft` | no use in WWHD (retail); tww demo build: daKN_Create sets it from mSpawnCountPrm (0 / 0xFF -> 100); oya_kn_move subtracts 1 per crab created (fopAcM_Create fpcNm_KN_e, param 2, every 10 frames) and deletes the spawner at 0 |

Open: `m2B6`, `m2C1`, `m2D6`, `m2FE` and `m30C` are unused byte gaps and stay.
mType, mSpawnCountPrm, mIsSpawner and mSpawnLeft are named from zeldaret/tww's `VERSION_DEMO` code (crab spawner); the
retail code in WWHD only stores the two param bytes.

## kokiie_class (Forbidden Woods hanging flower house; d_a_kokiie.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names for these fields (only `mPhase`, `mpModel` and `pm_bgw` are
named) and TP has no such house, so every name comes from the WWHD uses (d_a_kokiie.cpp, and d_a_shand.cpp for what
the rope actors write into `mRopeState`). The house hangs on five SHAND rope actors; the wobble, sway, size and tilt
fields take the names of the same fields of mflft_class (same code shape). `mDemoFovy` is the fovY argument of the
camera Set, as tww's call `camera->mCamera.Set(sp38, sp44, iVar1, i_this->m3A0)` against `Set(cXyz center, cXyz eye,
s16 bank, f32 fovY)` (d_camera.h) shows. The view `KokiieShand` in d_a_kokiie.h is a view of shand_class and keeps
its names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B4 | `m298` | `mFrameCounter` | kokiie_move (in daKokiie_Execute 021A57A0): + 1 every frame; phase of the wobble and sway (cM_ssin(it * 900) etc.); demo_camera (in daKokiie_Execute 021A57A0) phase of the camera bank shake |
| 0x3B6 | `m29A` | `mMode` | kokiie_move (in daKokiie_Execute 021A57A0): 0 hangs on its 5 ropes, 1 falls (set when all five are cut and the Boomerang is owned) and lands on mFallGroundY; daKokiie_Create 021A652C sets 1 when mSwitchNo is on |
| 0x3BC | `m2A0` | `mSizeType` | daKokiie_Create 021A652C: param & 0xFF (0xFF -> 0); 1 / 2 / 3 scale the house (x, z) to 0.9 / 0.8 / 0.7, else 1.0 |
| 0x3BE | `m2A2` | `mSwitchNo` | daKokiie_Create 021A652C: param >> 24; when that switch is on no ropes are created and the house starts fallen; kokiie_move (in daKokiie_Execute 021A57A0) turns it on at the crash (0xFF none) |
| 0x3C0 | `m2A4` | `mSwayOffset` | kokiie_move (in daKokiie_Execute 021A57A0): (mSwayX * sin, y, mSwayZ * sin); current.pos = home.pos + it + mSagY while hanging; y eased to 0 |
| 0x3CC | `m2B0` | `mSwayX` | kokiie_move (in daKokiie_Execute 021A57A0): amplitude of mSwayOffset.x, eased to 0 |
| 0x3D4 | `m2B8` | `mSwayZ` | kokiie_move (in daKokiie_Execute 021A57A0): amplitude of mSwayOffset.z, eased to 0 |
| 0x3D8 | `m2BC` | `mWobbleAmpX` | kokiie_move (in daKokiie_Execute 021A57A0): amplitude of mWobbleAngle.x (sin(mFrameCounter * 900)), eased to 300 |
| 0x3E0 | `m2C4` | `mWobbleAmpZ` | kokiie_move (in daKokiie_Execute 021A57A0): amplitude of mWobbleAngle.z (sin(mFrameCounter * 700)), eased to 300 |
| 0x3E4 | `m2C8` | `mWobbleAngle` | kokiie_move (in daKokiie_Execute 021A57A0): x / z = sin * mWobbleAmpX / mWobbleAmpZ; shape_angle = current.angle + it |
| 0x3EC | `m2D0` | `mSagY` | kokiie_move (in daKokiie_Execute 021A57A0): eased to -himo_off_yp[cut ropes] (step 50 * mTiltStepRate); added to current.pos.y while hanging |
| 0x3F0 | `m2D4` | `mShandIds` | himo_create (in daKokiie_Create 021A652C): process ids of the 5 SHAND rope actors it creates; fopAcM_SearchByID(it) to link them |
| 0x404 | `m2E8` | `mShandCreateStep` | himo_create (in daKokiie_Create 021A652C): per rope 0 = create the SHAND actor, 1 = link it (when found), 2 = done |
| 0x40C | `m2F0` | `mRopeEndPos` | daKokiie_Execute 021A57A0: rope i's end on the house (MtxPosition of (0, -170, 400) turned by i * 0x3333); himo_create (in daKokiie_Create 021A652C) gives the SHAND a pointer to it |
| 0x448 | `m32C` | `mRopeState` | himo_create (in daKokiie_Create 021A652C) gives the SHAND a pointer to it; the shand writes 1 when cut (hand_move in daShand_Execute 0246FB78) and 2 when back (normal, same); kokiie_move (in daKokiie_Execute 021A57A0): 1 -> 5 (cut, counted), 2 -> 0 |
| 0x44E | `m332` | `mTiltAngleY` | kokiie_move (in daKokiie_Execute 021A57A0): eased to himo_off_ya[cut ropes]; daKokiie_Execute 021A57A0 tilts the house by mTiltX about this direction |
| 0x450 | `m334` | `mTiltX` | kokiie_move (in daKokiie_Execute 021A57A0): eased to himo_off_xa[cut ropes], back to 0 when falling / landed; daKokiie_Execute 021A57A0 XrotM by it |
| 0x454 | `m338` | `mTiltStepRate` | kokiie_move (in daKokiie_Execute 021A57A0): 1 on a rope cut, 0 when a rope is back, eased to 1; step scale of the mSagY (50 * it) and tilt (10000 * it) eases |
| 0x45C | `m340` | `mBgMtx` | daKokiie_Execute 021A57A0: the house matrix; CallbackCreateHeap 021A6418: cBgW_Set(pm_bgw, dzb, MOVE_BG, it) |
| 0x490 | `m374` | `mFallGroundY` | kokiie_move (in daKokiie_Execute 021A57A0): 1380 (REG0_F(9) + 1380), the falling house stops there; daKokiie_Create 021A652C places a fallen house at it + 770 |
| 0x494 | `m378` | `mDemoMode` | kokiie_move (in daKokiie_Execute 021A57A0): 1 when the house falls; demo_camera (in daKokiie_Execute 021A57A0): 1 waits for the event, 2 runs the fall camera, 0 at its end (mDemoTimer 0x96) |
| 0x496 | `m37A` | `mDemoTimer` | demo_camera (in daKokiie_Execute 021A57A0): 0 at the start, + 1 per frame while mDemoMode is not 0; kokiie_move crashes at 0x1B, splashes at 0x40, the riddle SE at 100, the demo ends at 0x96 |
| 0x498 | `m37C` | `mDemoEye` | demo_camera (in daKokiie_Execute 021A57A0): camera eye of the fall demo (house + (500, 0, -1000) turned, y 1730); passed (with the shake) as the eye to the camera Set and Reset |
| 0x4A4 | `m388` | `mDemoCenter` | demo_camera (in daKokiie_Execute 021A57A0): camera centre of the fall demo (the house, y eased to pos.y - 450); passed (with the shake) as the centre to the camera Set and Reset |
| 0x4B8 | `m39C` | `mShakeAmp` | kokiie_move (in daKokiie_Execute 021A57A0): 90 on the landing; demo_camera (in daKokiie_Execute 021A57A0) shakes centre / eye by sin / cos * it and the bank by it * 8, eased to 0 |
| 0x4BC | `m3A0` | `mDemoFovy` | demo_camera (in daKokiie_Execute 021A57A0): 65 at the start, eased to 80 after frame 0x32; the fovY argument of the camera Set (tww: mCamera.Set(center, eye, bank, fovY)) |

## bridge_class (rope bridge; d_a_bridge.h)

zeldaret/tww (d217e2b) names `mMoveProcMode`, `mTypeBits`, `mPathId`, `mPathIdP`, `mBrCount`, `mpBgW`, `mEndPos`,
`mpAite` and `mBr` (kept) and keeps the GameCube offset names for the fields below; TP has no such bridge, so every
name comes from the WWHD uses (d_a_bridge.cpp, d_a_bridge_exec.cpp, d_a_bridge_draw.cpp; daBridge_Execute inlines
bridge_move, control1/2/3 and the rope checks). The per-plank struct `br_s` keeps its offset names here (next batch).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0541 | `m02D9` | `mRopePhaseOfs` | daBridge_Create 020E4098: (param >> 8) & 0xFF; bridge_move (in daBridge_Execute 020E11E8) sets 0 in the first move mode; daBridge_Execute 020E11E8 adds it * 0x5DC to the phase of the side ropes' wind sway (mFrameCounter * 0x578) |
| 0x0545 | `m02DD` | `mActiveBrCount` | daBridge_Create 020E4098: mBrCount, or 15 / 11 / 7 (by mBrCount) for a bridge with a partner (mTypeBits & 2); daBridge_Execute 020E11E8 hides the planks from it on (scale 0, no ropes) and builds the bg vertices of the first it planks |
| 0x0548 | `m02E0` | `mWavePower` | ride_call_back 020DF908: raised to the rider's movement * 0.3 (at most 20), 20 on a breaking hit; bridge_move_normal (in daBridge_Execute 020E11E8) copies it to mWaveAmpH / mWaveAmpV and eases it to 2 (wind above 0.1) or 0 |
| 0x054C | `m02E4` | `mSwingPower` | ride_call_back 020DF908: + 0.5 while below the rider's sideways movement (at most 50); bridge_move_normal (in daBridge_Execute 020E11E8) copies it to mSwingAmp and eases it to 0.6 (wind) or 0 |
| 0x0554 | `m02EC` | `mWavePhaseH` | control1 (in daBridge_Execute 020E11E8): + mWaveStepH per frame; phase of the sideways plank wave (sin(it + i * k) * mWaveAmpH) and of the whole bridge's swing (cos(it) * mSwingAmp, also in bridge_move_normal) |
| 0x0556 | `m02EE` | `mWavePhaseV` | control1 (in daBridge_Execute 020E11E8): + mWaveStepV per frame; phase of the vertical plank wave (sin(it + i * (k + 1000)) * mWaveAmpV) |
| 0x0558 | `m02F0` | `mWaveStepH` | bridge_move_normal (in daBridge_Execute 020E11E8): 0x578 every frame; control1 adds it to mWavePhaseH |
| 0x055A | `m02F2` | `mWaveStepV` | bridge_move_normal (in daBridge_Execute 020E11E8): 3000 every frame; control1 adds it to mWavePhaseV |
| 0x055C | `m02F4` | `mWaveAmpH` | bridge_move_normal (in daBridge_Execute 020E11E8): mWavePower every frame; control1 (in daBridge_Execute 020E11E8): amplitude of the sideways plank wave |
| 0x0560 | `m02F8` | `mSwingAmp` | bridge_move_normal (in daBridge_Execute 020E11E8): mSwingPower every frame; amplitude of the whole bridge's sideways swing (cos(mWavePhaseH) * it, the first plank -2 * it); control1 uses it too |
| 0x0564 | `m02FC` | `mWaveAmpV` | bridge_move_normal (in daBridge_Execute 020E11E8): mWavePower every frame; control1 (in daBridge_Execute 020E11E8): amplitude of the vertical plank wave; ride_call_back 020DF908 adds sin(mAnmPhase) * it * 0.03 to the rider's plank sag |
| 0x0568 | `m0300` | `mAnmPhase` | bridge_move_normal (in daBridge_Execute 020E11E8): + 3000 per frame (+ 4000 broken, bridge_move); phase of the ride bounce (ride_call_back 020DF908), the cut-rope shake (* 4), the side rope sway (* 6) and in daBridge_Draw 020DFF60 the plank ropes (* 5), chain and the draw rotation |
| 0x056A | `m0302` | `mFrameCounter` | daBridge_Execute 020E11E8: + 1 every frame; the side ropes' wind sway phase is it * 0x578 + mRopePhaseOfs * 0x5DC |
| 0x056C | `m0304` | `mBreakBrNo` | bridge_move_normal (in daBridge_Execute 020E11E8): the plank where the bridge breaks (both ropes cut and overloaded, or sag below -200), then mMoveProcMode 4; cut_control1 / cut_control2 hang the two halves from it; daBridge_Execute 020E11E8 moves the planks either side of it apart |
| 0x0570 | `m0308` | `mBreakCount` | bridge_move_normal (in daBridge_Execute 020E11E8): + 2 (+ 8 more for the heavy player) per frame while something rides near a plank whose ropes are both cut, - 1 per frame, 0 during an event; above 100 the bridge breaks |
| 0x0574 | `m030C` | `mSideRopeNum` | daBridge_Execute 020E11E8: 0 at the start, + 1 per plank whose side ropes it writes (segment index it + 1); daBridge_Draw 020DFF60 updates the side rope lines with it (+ the end) segments |
| 0x057A | `m0312` | `mBreakTimer` | bridge_move (in daBridge_Execute 020E11E8): 50 when the bridge breaks (mMoveProcMode 4), counted down in mode 5 |
| 0x0588 | `m0320` | `mEndRopePos1` | daBridge_Execute 020E11E8: copy of m11C[0] of the last active plank (mActiveBrCount - 1) of a partnered bridge; the partner's daBridge_Draw 020DFF60 (side_rope_end, via mpAite) ends its side rope 1 there |
| 0x0594 | `m032C` | `mEndRopePos0` | daBridge_Execute 020E11E8: copy of m0F8[0] of the last active plank of a partnered bridge; the partner's daBridge_Draw 020DFF60 (side_rope_end, via mpAite) ends its side rope 0 there |
| 0x05A4 | `m033C` | `mRideTimer` | ride_call_back 020DF908: 5 when the player stands on the bridge; daBridge_Execute 020E11E8 counts it down and does nothing during an event while it is not 0; bridge_move_normal shakes the controller on a break only while it is not 0 |

## br_s (rope bridge plank; d_a_bridge.h; view bkActionBridgeSegment in d_a_bk_action.h)

zeldaret/tww (d217e2b) names `mpModel`, `mTevStr`, `mpModelRope0/1`, `mLineMat1`, `mCyl`, `mPosition`, `mRotation`,
`mRotationYExtra` and `mScale` (kept) and keeps the GameCube offset names for the fields below; TP has no such bridge,
so every name comes from the WWHD uses (d_a_bridge.cpp, d_a_bridge_exec.cpp, d_a_bridge_draw.cpp). Side 0 / rope 0 is
the plank's +x side (`mSide0Pos`, `mRope0Hp`, ...), side 1 its -x side. br_s m406 is `mPlankRideTimer`, not
`mRideTimer`, because bridge_class already has a `mRideTimer` (m033C). The Bokoblin's view `bkActionBridgeSegment`
(b_hang 020A5674 hangs a Bokoblin on a plank) takes `mRideSag`, `mRideRollTarget` and `mPlankRideTimer`.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x35C | `m0F8` | `mSide1Pos` | daBridge_Execute 020E11E8: the plank's side 1 points: [1] the edge at -99 * scale.x, [2] 30 below it, [0] the rope top above [1] (while mRopeTopTimer runs); rope 1 / chain 1 hang from it (rope_fire, side_rope_segment, daBridge_Draw 020DFF60 draw_plank_rope / draw_chain); ride_call_back 020DF908 hands [1] to a Bokoblin |
| 0x380 | `m11C` | `mSide0Pos` | daBridge_Execute 020E11E8: the plank's side 0 points: [1] the edge at +99 * scale.x, [2] 30 below it, [0] the rope top above [1] (while mRopeTopTimer runs); rope 0 / chain 0 hang from it (rope_fire, side_rope_segment, daBridge_Draw 020DFF60); ride_call_back 020DF908 hands [1] to a Bokoblin |
| 0x604 | `m3A0` | `mRopeHitTimer` | rope_hit / chain_hit (in daBridge_Execute 020E11E8): [side] = 20 (rope) / 15 (chain) on a hit, only while below 10; daBridge_Execute 020E11E8 counts both down; amplitude of the rope's shake (daBridge_Draw 020DFF60, side_rope_segment) and with both, the plank's (bridge_move_normal) |
| 0x608 | `m3A4` | `mRope0Hp` | bridge_move (in daBridge_Execute 020E11E8): 3; rope_hit (in daBridge_Execute 020E11E8) subtracts the hit's damage (4 above 1) for side 0, the rope is cut at 0 or below (mRopeFlags bit 1 off); daBridge_Draw 020DFF60 thins the rope at 2 and 1 |
| 0x609 | `m3A5` | `mRope1Hp` | bridge_move (in daBridge_Execute 020E11E8): 3; rope_hit (in daBridge_Execute 020E11E8): as mRope0Hp for side 1 (mRopeFlags bit 2) |
| 0x60C | `m3A8` | `mRopeMidPos` | daBridge_Draw 020DFF60 (draw_plank_rope): [line] = the middle point of the plank's rope (segment 2 of 5); rope_hit (in daBridge_Execute 020E11E8) puts the hit and splinter effects there |
| 0x624 | `m3C0` | `mRope0FireTimer` | rope_fire (in daBridge_Execute 020E11E8): 30 when the player's Bokoblin torch flame is within 50 of rope 0's top; counted down, rope 0 burns off at 0 (mRopeFlags bit 1 off) |
| 0x626 | `m3C2` | `mRope1FireTimer` | rope_fire (in daBridge_Execute 020E11E8): as mRope0FireTimer for rope 1 (mRopeFlags bit 2) |
| 0x628 | `m3C4` | `mpRope0FireEmitter` | rope_fire (in daBridge_Execute 020E11E8): the ID_AK_SN_BRIDGEROPEFIRE00 emitter of rope 0, moved to the rope top while mRope0FireTimer runs, invalidated at 0 |
| 0x62C | `m3C8` | `mpRope1FireEmitter` | rope_fire (in daBridge_Execute 020E11E8): as mpRope0FireEmitter for rope 1 |
| 0x630 | `m3CC` | `mCtrlPos` | control1 / control2 / cut_control1 / cut_control2 (in daBridge_Execute 020E11E8): the plank's simulated chain position (75 apart, from home.pos and mEndPos); bridge_move_normal copies it (blended) into mPosition; daBridge_IsDelete 020E37C4 / sounds use it |
| 0x650 | `m3EC` | `mGroundY` | bridge_move (in daBridge_Execute 020E11E8): -10000 while broken; cut_control1 / cut_control2 keep the hanging plank at least 30 above it |
| 0x654 | `m3F0` | `mSwayRate` | bridge_move (in daBridge_Execute 020E11E8): \|sin(pi * i / (count - 1))\| (1 for chain bridges); control1 / control2 scale the plank's wave, droop and sideways swing by it, bridge_move_normal its ride tilt |
| 0x658 | `m3F4` | `mRideSag` | ride_call_back 020DF908: -31 (player, -40 heavy), -40 (MO2), -25 (Bokoblin), -10 (others), -300 for a breaking hit, plus the bounce; bridge_move_normal (in daBridge_Execute 020E11E8) eases mSagY of the planks around to it, breaks below -200; b_hang 020A5674 (bk) sets -25 |
| 0x65C | `m3F8` | `mSagY` | bridge_move_normal (in daBridge_Execute 020E11E8): eased to mRideSag (or -80 / -30 at cut ropes) * the plank influence, then back to 0; control1 / control2 add half of it to the plank height |
| 0x660 | `m3FC` | `mDroopY` | bridge_move_normal (in daBridge_Execute 020E11E8): eased to -15; control1 / control2 add half of it (* mSwayRate) to the plank height; bridge_move 0 when broken, then eased to -50 (cut_control1 adds it) |
| 0x664 | `m400` | `mRideRollTarget` | ride_call_back 020DF908: -(rider's local x) * k; bridge_move_normal (in daBridge_Execute 020E11E8) eases mRideRoll of the planks around to it, then 0; b_hang 020A5674 (bk) sets it (+-0x2000) |
| 0x666 | `m402` | `mRideRoll` | bridge_move_normal (in daBridge_Execute 020E11E8): eased to mRideRollTarget * influence * mSwayRate, then back to 0; mRotation.z = it + mCutRoll |
| 0x668 | `m404` | `mCutRoll` | bridge_move_normal (in daBridge_Execute 020E11E8): eased to +-7000 (one rope cut) * influence, plus the rope shake; mRotation.z = mRideRoll + it |
| 0x66A | `m406` | `mPlankRideTimer` | ride_call_back 020DF908: 2 when something rides the plank (b_hang 020A5674 sets 1); bridge_move_normal (in daBridge_Execute 020E11E8) applies mRideSag / mRideRollTarget while it is not 0 and counts it down |
| 0x66B | `m407` | `mGroundHitCount` | cut_control1 / cut_control2 (in daBridge_Execute 020E11E8): + 1 each time the hanging plank is held at mGroundY + 30; nothing reads it |
| 0x66C | `m408` | `mRopeFlags` | CallbackCreateHeap 020E3890: 7 on every 4th plank; bit 4 = the plank carries ropes / chains, bit 1 / 2 = rope 0 / 1 intact (cleared when cut or burnt, rope_hit / rope_fire); bridge_move_normal and daBridge_Draw 020DFF60 act on them |
| 0x67C | `m418` | `mRopeTopTimer` | CallbackCreateHeap 020E3890: 0x32 (chain bridge) or -1 on a rope plank; daBridge_Execute 020E11E8 places the rope tops ([0] of mSide0Pos / mSide1Pos) while it is not 0 and counts a positive value down |

### BridgeHD::BokoReaction (d_a_bridge.h, the bridge's view of bk_class)

ride_call_back (d_a_bridge.cpp) writes a Bokoblin that steps on a plank through this view. Each field takes the
bk_class name (d_a_bk.h); the dr (damagereaction_l) fields m79C, m7AC, m7B2, m7B4 and m7B8 are still offset-named
there, so the view keeps them.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0xC8C | `m0B2C` | `mpBridge` | bk_class mpBridge (d_a_bk.h) |

## ssk_class (stone tentacle; d_a_ssk.h)

zeldaret/tww (d217e2b) names `mPhase`, `mpMorf1`, `mpMorf2`, `mAcchCir`, `mAcch`, `mStts` and `mCyl` (kept) and keeps
the GameCube offset names for the fields below; TP has no such actor, so the names come from the WWHD uses
(d_a_ssk.cpp; daSsk_Execute inlines nomal_move and body_atari_check). `mSmokeCb` and `mpJntHit` follow the names
TWW gives the same member types in other actors. m35C (0x438, 0x38 bytes, no uses) stays open.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D0 | `m2B4` | `mType` | daSsk_Create 0248E770: parameter bits 0-7; 0xFF starts the tentacle fully out (mMoveState SSK_ACTIVE, mKtanaScale 1); nomal_move (in daSsk_Execute 0248DC20): type 1 retracts once the player is farther than mRetractDist |
| 0x3D1 | `m2B5` | `mRadiusPrm` | daSsk_Create 0248E770: parameter bits 8-15 (0xFF read as 0); times 10 gives mAppearDist |
| 0x3D2 | `m2B6` | `mSwitchNo` | daSsk_Create 0248E770: parameter bits 24-31; daSsk_Create and nomal_move (in daSsk_Execute 0248DC20) test dComIfGs_isSwitch on it (not 0xFF): delete at once / retract, then delete |
| 0x3D3 | `m2B7` | `mMoveSePlayed` | nomal_move (in daSsk_Execute 0248DC20): set when JA_SE_OBJ_SHOKU_LIFT_MOVE starts as joint 1's wobble (mJointAngle[1].z) nears 0x1000, cleared when it leaves, so the sound plays once per swing |
| 0x3D4 | `m2B8` | `mAction` | daSsk_Execute 0248DC20: switch, case 0 runs nomal_move; daSsk_Create 0248E770: 0 |
| 0x3D6 | `m2BA` | `mMoveState` | nomal_move (in daSsk_Execute 0248DC20): the SskMoveState switch (wait, rock appear, rise, retract, active); daSsk_Create 0248E770: SSK_ACTIVE for type 0xFF |
| 0x3D8 | `m2BC` | `mHitTimer` | body_atari_check (in daSsk_Execute 0248DC20): 8 on a hit, counted down; no hit is taken while above 0 |
| 0x3DC | `m2C0` | `mAppearDist` | daSsk_Create 0248E770: mRadiusPrm * 10; nomal_move (in daSsk_Execute 0248DC20): the tentacle appears when the player comes closer (SSK_WAIT) |
| 0x3E0 | `m2C4` | `mRetractDist` | daSsk_Execute 0248DC20: mAppearDist + 100; nomal_move (in daSsk_Execute): type 1 retracts when the player is farther (SSK_ACTIVE) |
| 0x3EC | `m2D0` | `mSmokeCb` | nomal_move (in daSsk_Execute 0248DC20): smoke callback of the ID_IT_ST_TGSYOKU_SMOKE00 particles; daSsk_Create 0248E770 constructs it, daSsk_Delete 0248E504 removes it; after TWW mSmokeCb (dPa_smokeEcallBack of other actors) |
| 0x40C | `m2F0` | `mWobbleTimer` | nomal_move (in daSsk_Execute 0248DC20): random at appear, +1 per frame from SSK_RISE on; phase of the joints' sine wobble (mJointAngle) |
| 0x40E | `m2F4` | `mJointAngle` | nodeCallBack 0248D880: [jntNo] rotates joint jntNo (Y, X, Z) of the tentacle model (mpMorf1); nomal_move (in daSsk_Execute 0248DC20) sets [1..4].y/.z to the wobble |
| 0x42C | `m350` | `mKtanaScale` | draw_sub 0248D9B4: base scale of the ktana_00 model (mpMorf2, useHeapInit 0248E554), drawn by daSsk_Draw 0248DB4C only above 0.01; nomal_move (in daSsk_Execute 0248DC20) grows it to 1 (SSK_ROCK_APPEAR) and shrinks it to 0 (SSK_RETRACT) |
| 0x7E8 | `m708` | `mpJntHit` | useHeapInit 0248E554: sskCreateJntHit(mpMorf1 model, search_data, 3) result (also stored in the enemy's jntHit); after TWW mpJntHit (JntHit_c* of other actors) |

## ep_class (torch / flame; d_a_ep.h; per-moth struct ep_ga_s)

zeldaret/tww (d217e2b) names `mLightPower`, `mAlphaModel*`, `mTimers`, `mbNoEp`, `mbShouldDrawModel`, `mType`,
`mOnSwitchNo`, `mpEmitter`, `mLight`, `mPosTop`, `mbHasObm`, `mbHasGa`, `mEpGa` and `mGroundCheckTimer` (kept) and keeps
the GameCube offset names for the fields below; TP has no such actor, so the names come from the WWHD uses (d_a_ep.cpp;
daEp_Execute inlines ep_move, ga_move and ep_light). `mStaffId` follows the name TWW gives the same event value in
other actors. The padding arrays m4D4, m4FC, m4FF, m509, m7D6, m7DF, m7E5 (and ep_ga_s m2C) have no uses and stay open.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x5EC | `m4D0` | `mState` | ep_move (in daEp_Execute 0212DBFC): the EpState switch (EP_OFF, EP_IGNITE, EP_BURN, EP_EXTINGUISH); daEp_Execute sets EP_EXTINGUISH when water stands above the flame; daEp_CreateInit (in daEp_Create 0212FAF4): EP_OFF; ga_move (in daEp_Execute) sends the moths up while EP_OFF |
| 0x5FC | `m4E0` | `mLightPowerTarget` | ep_move (in daEp_Execute 0212DBFC): scale.x on ignition; EP_BURN eases mLightPower to it |
| 0x625 | `m505` | `mBurnTime` | daEp_Create 0212FAF4: parameter bits 8-15; ep_move (in daEp_Execute 0212DBFC): a hit that lights the torch sets mTimers[EP_EXTINGUISH_TIMER] to it * 10 + 1 (not 0xFF); at 1 the torch's switch goes off |
| 0x627 | `m507` | `mKeepSwitchNo` | daEp_Create 0212FAF4: parameter bits 16-23; ep_move (in daEp_Execute 0212DBFC): while this switch is on (not 0xFF) the burn time running out does not turn mOnSwitchNo off |
| 0x628 | `m508` | `mIgniteSwitchNo` | ep_move (in daEp_Execute 0212DBFC): mOnSwitchNo + 1 when the torch goes out (0 = none); while set, EP_OFF waits: a hit turns switch m508 - 1 on, and that switch being on lights the torch (then 0); daEp_CreateInit (in daEp_Create 0212FAF4): 0 |
| 0x8F8 | `m7D4` | `mWindTimer` | ep_move (in daEp_Execute 0212DBFC): 40 when the burning flame's sphere is hit by a 0x600000 attack (tww AT_TYPE_WIND \| AT_TYPE_UNK400000), counted down; starts the ID_AK_JN_TORCH emitter and bends it while running, no fire particle while above 6; ga_move (in daEp_Execute) blows the moths meanwhile |
| 0x8FC | `m7D8` | `mWindPower` | ep_move (in daEp_Execute 0212DBFC): eased to 4 while mWindTimer is above 10, else to 0; z of the torch emitter's direction; ga_move (in daEp_Execute) pushes the moths by it * 5 |
| 0x900 | `m7DC` | `mWindAngle` | ep_move (in daEp_Execute 0212DBFC): shape_angle.y of the attacker (fpcNm_BDK_e) or else the player on the wind hit; Y rotation of the emitter direction and the moths' push (ga_move, in daEp_Execute) |
| 0x904 | `m7E0` | `mStaffId` | ep_move (in daEp_Execute 0212DBFC): dComIfGp_evmng_getMyStaffId(l_staffName) of the SHOKUDAI_SWITCH event (-1 when done); ep_switch_event_move 0212DB44 passes it to dComIfGp_evmng_getIsAddvance / getMyActIdx / cutEnd; after TWW mStaffId (other actors' staff ids) |
| 0x984 | `m860` | `mLightInfo` | ep_class_ct 0212F878: the first of the three J3DLightInfo blocks (with mHdLightInfo1 / mHdLightInfo2) filled from j3dDefaultLightInfo |

ep_ga_s (one moth around the torch, mEpGa[2]):

| Offset in ep_ga_s | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x10 | `m10` | `mOldPos` | ga_move (in daEp_Execute 0212DBFC): mPos before the frame's move; start of the line check that stops the moth at walls |
| 0x1C | `m1C` | `mTargetPos` | ga_move (in daEp_Execute 0212DBFC): random point within 150/100/150 of mPosTop (near the Bokoblin torch flame for moth 0), 1000 higher while the torch is out; the moth turns toward it |
| 0x38 | `m38` | `mFlapAngle` | ga_move (in daEp_Execute 0212DBFC): + 0x3E00 per frame, scaleY = cM_ssin of it; random start (daEp_CreateHeap 0212F5CC) |
| 0x3B | `m3B` | `mTargetTimer` | ga_move (in daEp_Execute 0212DBFC): counted down; at 0 a new mTargetPos is picked and it is set to cM_rndF(10) |

## kui_class (hook post / bell; d_a_kui.h)

zeldaret/tww (d217e2b) names `mPhs`, `mpModel2`, `mpModel`, `type` and `mSwitchNo` (kept) and keeps the GameCube offset
names (field_0x...) for the fields below; TP has no such actor, so the names come from the WWHD uses (d_a_kui.cpp;
daKui_Execute inlines kui_bell and demo_camera). The views of dr2_class / btd_class in d_a_kui.h belong to those classes.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3BD | `field_0x2A1` | `mScaleType` | daKui_Create 021ADD6C: parameter bits 8-15 (4 for type 3); its switch sets scale (0: 0.5 x/y, 1: 2 x/y, 2: 2 z, 3, 4: 2, 5: 4/2/4 ...) |
| 0x3BE | `field_0x2A2` | `mOnDragonTail` | daKui_Create 021ADD6C: parameter bits 4-7; daKui_Execute 021ACB0C: when set the post follows the dragon tail (dr2_class unk_3B0, found with s_a_i_sub / b_a_i_sub) and its collision matrix is scaled by 4 |
| 0x3C4 | `field_0x2A8` | `mBgMtx` | daKui_CreateHeap 021ADB48: the matrix cBgW_Set gives mpBgW; daKui_Execute 021ACB0C and kui_bell (in daKui_Execute) fill it before dBgW_Move |
| 0x3F4 | `field_0x2D8` | `mpBgW` | daKui_CreateHeap 021ADB48: the created dBgW (cBgW_Set, dBgW_SetCrrFunc); daKui_Create 021ADD6C registers it (dBgS_Regist), daKui_Delete 021ADAFC releases it, daKui_Execute 021ACB0C moves it (dBgW_Move) |
| 0x3F8 | `field_0x2DC` | `mBellTimers` | kui_bell (in daKui_Execute 021ACB0C): [1] / [2] = 0x50 when the hooked bell hits its limit on that side (no new ring while set), [0] = 40 starts the ring: its countdown drives the shake and turns mSwitchNo on at 0; all counted down |
| 0x3FE | `field_0x2E2` | `mSwingAngle` | kui_bell (in daKui_Execute 021ACB0C): eased to mSwingTarget at mSwingStep; added to the bell model's X rotation |
| 0x400 | `field_0x2E4` | `mSwingTarget` | kui_bell (in daKui_Execute 021ACB0C): bure_xa_d[side] (+-0x1000) on a ring, eased back to 0; target of mSwingAngle |
| 0x402 | `field_0x2E6` | `mSwingStep` | kui_bell (in daKui_Execute 021ACB0C): 0x400 on a ring, eased to 0x100; step of mSwingAngle |
| 0x404 | `field_0x2E8` | `mDemoMode` | demo_camera (in daKui_Execute 021ACB0C): its switch (0 none, 1 order the event, 2 run the camera); daKui_Execute sets 1 when mSwitchTimer reaches 970 (type 2); 0 at the end (mDemoTimer 70) |
| 0x406 | `field_0x2EA` | `mDemoTimer` | demo_camera (in daKui_Execute 021ACB0C): 0 at the demo start, +1 per frame (K SUB COUNT report); drives the post's sink (mDemoOffsetY) and ends the demo at 70 |
| 0x408 | `field_0x2EC` | `mDemoCamEye` | demo_camera (in daKui_Execute 021ACB0C): home.pos + the rotated (800, 0, 100); the eye passed to the camera Set / Reset (KuiCameraControl Set(center, eye)) |
| 0x414 | `field_0x2F8` | `mDemoCamCenter` | demo_camera (in daKui_Execute 021ACB0C): home.pos 200 higher; the center passed to the camera Set / Reset (KuiCameraControl Set(center, eye)) |
| 0x424 | `field_0x308` | `mSwitchTimer` | daKui_Execute 021ACB0C (types 2 and 4): 1000 when health is 3 and the switch is off, counted down; at 970 type 2 starts the demo (mDemoMode 1), type 4 turns mSwitchNo on |
| 0x428 | `field_0x30C` | `mDemoOffsetY` | demo_camera (in daKui_Execute 021ACB0C): current.pos.y = home.pos.y + it; a small sine shake, then eased to -70 (the post sinks), then a shake around -70 |

## msw_class (chain-hung swinging platform; d_a_msw.h)

zeldaret/tww (d217e2b) names `mPhs`, `mpModel`, `mpChainModels`, `mStts`, `mChainCyls`, `mMtx` and `mpBgW` (kept) and
keeps the GameCube offset names for the fields below; TP has no such actor, so the names come from the WWHD uses
(d_a_msw.cpp; daMsw_Execute inlines msw_move, daMsw_Draw chain_Draw). m29A (no uses) and the padding m846 stay open.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B4 | `m298` | `mSwayTimer` | msw_move (in daMsw_Execute 021D6880): +1 per frame; phase of the sine sway (mSwayOffset) and tilt (mTiltAngle); chain_Draw (in daMsw_Draw 021D6518): phase of a hit chain's shake |
| 0x3BC | `m2A0` | `mScaleType` | daMsw_Create 021D6F48: parameter bits 0-7 (0xFF read as 0); its switch sets scale.x/z (1: 1.5, 2: 2, 3: 3, else 1) |
| 0x3C0 | `m2A4` | `mSwayOffset` | msw_move (in daMsw_Execute 021D6880): x / z = sine of mSwayTimer times mSwayAmp; current.pos = home.pos + it |
| 0x3CC | `m2B0` | `mSwayAmp` | ride_call_back 021D6270: raised toward the rider's step (old to new position, x / z) above 10; msw_move (in daMsw_Execute 021D6880): amplitude of mSwayOffset, eased back to 0 |
| 0x3D8 | `m2BC` | `mTiltAmp` | ride_call_back 021D6270: raised to the rider's step (z into x, x into z); msw_move (in daMsw_Execute 021D6880): amplitude of mTiltAngle, eased to REG0_F(9) |
| 0x3E4 | `m2C8` | `mTiltAngle` | msw_move (in daMsw_Execute 021D6880): x / z = sine of mSwayTimer times mTiltAmp; shape_angle = current.angle + it |
| 0x3FC | `m2E0` | `mChainLowerPos` | daMsw_Execute 021D6880: [i] = the platform corner (xd / zd times 200) through the platform matrix; centre of chain i's cylinder and its hit sound; chain_Draw (in daMsw_Draw 021D6518) draws chain i from it to mChainUpperPos[i]; daMsw_IsDelete 021D6D50 deletes its sound object |
| 0x42C | `m310` | `mChainUpperPos` | daMsw_Execute 021D6880: [i] = mChainLowerPos[i] 1000 higher while mChainInitTimer runs; chain_Draw (in daMsw_Draw 021D6518): the chain's other end |
| 0x958 | `m83C` | `mChainHitTimer` | daMsw_Execute 021D6880: [i] = 15 when chain i's cylinder is hit while below 10, counted down; chain_Draw (in daMsw_Draw 021D6518) shakes chain i by it |
| 0x960 | `m844` | `mChainInitTimer` | daMsw_Create 021D6F48: 10; daMsw_Execute 021D6880: counted down in the chain loop, and while not 0 the chains' upper ends are placed (mChainUpperPos) |

## syan_class (hanging torch chandelier; d_a_syan.h)

zeldaret/tww (d217e2b) names `mPhs`, `morf`, `frameCount`, `state`, `timer`, the `part*` arrays, `emtrCallBack` and
`emtrEnabled` (kept) and keeps the GameCube offset names (field_0x...) for the fields below; TP has no such actor, so the
names come from the WWHD uses (d_a_syan.cpp). field_0x2d8 (no uses) stays open.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3C0 | `field_0x2a4` | `mSwingAngle` | nodeCallBack 024A4298: [0] rotates joint 5 (SYAN_JNT_JOINT5_e: Y, -X, Z), [1].x is added to joint 8's X; daSyan_Execute 024A45E0: [0].x / [1].x = sine of mSwingPhase0 / 1 times mSwingAmp0 / 1 |
| 0x3CC | `field_0x2b0` | `mSwayAngle` | nodeCallBack 024A4298: rotates joint 8 (SYAN_JNT_JOINT8_e: Y, -(X + mSwingAngle[1].x), Z); daSyan_Execute 024A45E0: y / z = sine of frameCount times mSwayAmp, x eased to 2500 in states 1 and 2 |
| 0x3D4 | `field_0x2b8` | `mSwayAmp` | daSyan_Create 024A5224: 200; daSyan_Execute 024A45E0: amplitude of mSwayAngle.y / z, eased to 2000 in state 1 and to mSwingAmpScale * 500 in state 2 |
| 0x3D8 | `field_0x2bc` | `mSwingAmp0` | daSyan_Execute 024A45E0: amplitude of mSwingAngle[0].x; eased to 1500 in state 1, to mSwingAmpScale * 300 in state 2 |
| 0x3DC | `field_0x2c0` | `mSwingAmp1` | daSyan_Execute 024A45E0: amplitude of mSwingAngle[1].x; eased to 2500 in state 1, to mSwingAmpScale * 300 in state 2 |
| 0x3E0 | `field_0x2c4` | `mSwingPhase0` | daSyan_Execute 024A45E0: + mSwingSpeed0 per frame; sine phase of mSwingAngle[0].x |
| 0x3E4 | `field_0x2c8` | `mSwingPhase1` | daSyan_Execute 024A45E0: + mSwingSpeed1 per frame; sine phase of mSwingAngle[1].x |
| 0x3E8 | `field_0x2cc` | `mSwingSpeed0` | daSyan_Execute 024A45E0: added to mSwingPhase0 each frame; eased to 500 in state 2 |
| 0x3EC | `field_0x2d0` | `mSwingSpeed1` | daSyan_Execute 024A45E0: added to mSwingPhase1 each frame; eased to 1000 in state 2 |
| 0x648 | `field_0x52c` | `mPartAlphaTarget` | daSyan_Execute 024A45E0: [i] = 4 + cM_rndF(4) whenever mPartAlphaTimer[i] runs out; partAlpha[i] is eased to it |
| 0x678 | `field_0x55c` | `mPartScale1Target` | daSyan_Execute 024A45E0: [i] = 0.75 + cM_rndF(0.075) whenever mPartScale1Timer[i] runs out; partScale1[i] is eased to it |
| 0x6A8 | `field_0x58c` | `mPartAlphaTimer` | daSyan_Execute 024A45E0: [i] = 5 + cM_rndF(10) at 0 (with a new mPartAlphaTarget[i]), counted down |
| 0x6B4 | `field_0x598` | `mPartScale1Timer` | daSyan_Execute 024A45E0: [i] = 3 + cM_rndF(6) at 0 (with a new mPartScale1Target[i]), counted down |
| 0x6C0 | `field_0x5a4` | `mFireStartTimer` | daSyan_Create 024A5224: [i] = fire_time[i]; daSyan_Execute 024A45E0: counted down; at 0 part i's ID_AK_JN_TORCH emitter starts (emtrEnabled) and partScale2[i] grows |

Round 2:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3F8 | `field_0x2dc` | `mSwingAmpScale` | daSyan_Execute 024A45E0 state 2: scales the targets of mSwingAmp0 / mSwingAmp1 (300) and mSwayAmp (500) (tww the same); never written in WWHD or tww |

## sk2_class (swaying lily-pad lift; d_a_sk2.h)

zeldaret/tww (d217e2b) names `mPhase`, `mpMorf`, `mpBgW`, `mAcchCir` and `mAcch` (kept) and keeps the GameCube offset
names for the fields below; TP has no such actor, so the names come from the WWHD uses (d_a_sk2.cpp). The padding m318
stays open.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D0 | `m2B4` | `mType` | daSk2_Create 02489A00: parameter bits 0-7 (0xFF read as 0); useHeapInit 02489770 picks the bck and dzb by it (sk2_bck / sk2_dzb); dousa_move 02489290: type 0 sways joints 1-3 in x / z, others in y |
| 0x3D1 | `m2B5` | `mStartFramePrm` | daSk2_Create 02489A00: parameter bits 8-15; unless 0xFF it becomes mStartFrame |
| 0x3D2 | `m2B6` | `mSwitchNo` | daSk2_Create 02489A00: parameter bits 24-31; when not 0xFF and the switch is on the actor is not created (cPhs_ERROR_e) |
| 0x3D6 | `m2BA` | `mMoveState` | dousa_move 02489290: its switch (0 start at mStartFrame, 1 play to frame 0 or 18, 2 slow down, 3 rest mWaitTimer, then 1 with JA_SE_OBJ_SHOKU_LIFT_MOVE) |
| 0x3D8 | `m2BC` | `mWaitTimer` | dousa_move 02489290: 100 when the animation stops (state 2), counted down in state 3; at 0 the animation restarts |
| 0x3DC | `m2C0` | `mPlaySpeed` | dousa_move 02489290: 0.5 on (re)start, eased to 0 in state 2; passed to mpMorf->setPlaySpeed |
| 0x3E0 | `m2C4` | `mStartFrame` | daSk2_Create 02489A00: mStartFramePrm (0 above 35); dousa_move 02489290: mpMorf->setFrame(it) in state 0 |
| 0x3E8 | `m2CC` | `mSwayTimer` | dousa_move 02489290: +1 per frame; phase of the joints' sine sway (mJointAngle) |
| 0x3EA | `m2CE` | `mJointAngle` | nodeCallBack 02489098: [jntNo] rotates joint jntNo (Y, X, Z, up to KSYLF_00_JNT_B1_e); dousa_move 02489290 sets [1..3] to the sway (x / z for type 0, y otherwise) |
| 0x404 | `m2E8` | `mJointPos` | nodeCallBack 02489098: [jntNo] = the origin of joint jntNo after its rotation (MtxPosition of 0, 0, 0) |

## sail_class and daSail_packet_c (ship's sail; d_a_sail.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names for all fields below; there they sit in daSail_packet_c
(0x1C34..0x1C52), WWHD moves m1C40..m1C52 behind the packet into sail_class. TP has no such actor, so the names come
from the WWHD uses (d_a_sail.cpp; sail_move inlines sail_pos_move and demo_move). Left open: daSail_packet_c m1C3B (set to 1 in the constructor, never read).

sail_class:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3280 | `m1C40` | `mWavePhase` | sail_move 02460678: + 4000 * (1 - 0.95 * mFurl) * the wind's z share each frame; subtracted from / added to the per-vertex phase of the flutter sine / cosine (tww sail_move: mSailPacket.m1C40 +=) |
| 0x3284 | `m1C44` | `mFurl` | sail_move 02460678: eased to 0.6 (l_HIO m06 0) or to 0 (m06 1) at mFurlSpeed, kept in 0..1; a demo actor's animation frame sets it (0.6 - 0.006 * frame); it shortens the sail rows (the 'furl bulge' comment: 700 / 150) and lowers the flutter |
| 0x3288 | `m1C48` | `mFurlSpeed` | sail_move 02460678: step of cLib_addCalc on mFurl: 0.015 then -0.001 per frame toward 0.6, +0.0075 per frame toward 0, 0 when settled |
| 0x3290 | `m1C50` | `mWaveColStep` | sail_move 02460678: 7500 * (1 + 1.5 * the wind's x share); flutter phase step per sail column (times j - 3) |
| 0x3292 | `m1C52` | `mWaveRowStep` | sail_move 02460678: 7200 * (0.05 + 0.95 * the wind's z share); flutter phase step per sail row (times i - 6) |

daSail_packet_c:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x2EB8 | `m1C34` | `mNrmAngle` | daSail_packet_c::setCorrectNrmAngle 0245FFC8: 300 * sin(mNrmWobblePhase) + mNrmCorrectAngle; daSail_packet_c::setNrmMtx 024601F0: the Y rotation of the normal matrix; daSail_packet_ct 024616AC: 0 |
| 0x2EBA | `m1C36` | `mNrmCorrectAngle` | daSail_packet_c::setCorrectNrmAngle 0245FFC8: eased to +-l_HIO m10 degrees when the wind comes from near the front or back, else to 0; added to mNrmAngle; daSail_packet_ct 024616AC: 0 |
| 0x2EBC | `m1C38` | `mNrmWobblePhase` | daSail_packet_c::setCorrectNrmAngle 0245FFC8: + 900 + cM_rndF(200) per call; sine phase of mNrmAngle; daSail_packet_ct 024616AC: 0 |
| 0x2EBE | `m1C3A` | `mCurArr` | daSail_packet_c::update 0245F638, setBackNrm 024605DC, sail_move 02460678: index of the mPos / mNrm / mBackNrm vertex array set in use (getPos / getNrm / getBackNrm); daSail_packet_ct 024616AC: 0 |

Round 2:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x328C | `m1C4C` | `mFurlFactor` | sail_move 02460678: 15 * (mFurl - 0.6) each frame; damps the wind z share and the light factor by 1 - 0.5 * mFurl * it (tww the same) |

## sk_class (small swaying stone tentacle; d_a_sk.h)

zeldaret/tww (d217e2b) names `mPhase`, `mpMorf` and `mStts` (kept) and keeps the GameCube offset names for the fields
below; TP has no such actor, so the names come from the WWHD uses (d_a_sk.cpp; daSk_Execute inlines dousa_move and
body_atari_check). mMoveSePlayed and mHitTimer match the same uses in ssk_class; mSph and mpJntHit follow the names
TWW gives the same member types in other actors. m2B4 (parameter bits 0-7, stored but never used) stays open.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D1 | `m2B5` | `mSwitchNo` | daSk_Create 02488D58: parameter bits 24-31; when not 0xFF and the switch is on the actor is not created |
| 0x3D8 | `m2BC` | `mMoveSePlayed` | dousa_move (in daSk_Execute 024886D0): set when JA_SE_OBJ_SHOKU_LIFT_MOVE starts as mJointAngle[1].z nears 0x1000, cleared when it leaves (as ssk_class mMoveSePlayed) |
| 0x3DA | `m2BE` | `mSwayTimer` | daSk_Create 02488D58: the process id's low byte << 12; daSk_Execute 024886D0: +1 per frame; dousa_move (in daSk_Execute): phase of the joints' sine sway (mJointAngle) |
| 0x3DC | `m2C0` | `mHitTimer` | body_atari_check (in daSk_Execute 024886D0): 8 on a hit, counted down; no hit is taken while above 0 (as ssk_class mHitTimer) |
| 0x3DE | `m2C2` | `mJointAngle` | nodeCallBack 024884FC: [jntNo - 1] rotates joint jntNo (Y, X, Z); dousa_move (in daSk_Execute 024886D0) sets the sway into .x / .y |
| 0x3F8 | `m2DC` | `mJointPos` | nodeCallBack 024884FC: [jntNo - 1] = joint jntNo's point (joint_offsets) after its rotation; daSk_Execute 024886D0: centre of sphere m348[i] |
| 0x464 | `m348` | `mSph` | daSk_Create 02488D58: body_co_sph_src, mStts; daSk_Execute 024886D0: [i] at mJointPos[i] with sphere_radii[i]; body_atari_check (in daSk_Execute) checks their hits; after TWW mSph (dCcD_Sph of other actors) |
| 0x914 | `m924` | `mpJntHit` | useHeapInit 02488BB8: JntHit_create(mpMorf model, search_data, 3), also stored in the enemy's jntHit; after TWW mpJntHit (JntHit_c* of other actors) |

## kamome_class (seagull; d_a_kamome.h)

zeldaret/tww (d217e2b) names the other kamome_class fields (kept) and keeps the GameCube offset names for the fields
below; TP has no such actor, so the names come from the WWHD uses (d_a_kamome.cpp, d_a_kamome_exec.cpp,
d_a_kamome_auto.cpp). mHeisouTimer / mHeisouHeight take the name of tww's kamome_heisou_move (the gull flying beside
the boat), where they are used. The esa_class copy in d_a_kamome.h follows with esa_class.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3C3 | `m2AB` | `mLookAround` | kamome_auto_move 02189270 and the move states of daKamome_Execute 021861D4 (path wait, ko wait, kamome_imouto2_move) set 1 for the frame; daKamome_Execute: while set the head targets (mJointRotYTarget / ZTarget) get random values every 10-30 frames (mTimers[5]), else 0; cleared each frame |
| 0x3CC | `m2B4` | `mHeisouTimer` | daKamome_Create 02188D84: cM_rndF(10000) for mType 7; kamome_heisou_move (in daKamome_Execute 021861D4): +1 per frame; sine phase of the flying-beside-the-boat offset |
| 0x3D0 | `m2B8` | `mHeisouHeight` | kamome_heisou_move (in daKamome_Execute 021861D4): eased to home.pos.y (or 0 when the boat is fast and a master gull leads); the escort target's height above the player (+ 600) |
| 0x414 | `m2FC` | `mLandAngleY` | kamome_auto_move 02189270 and the landing in daKamome_Execute 021861D4: current.angle.y + cM_rndFX(15000) on touchdown; current.angle.y is eased to it while landing |
| 0x420 | `m308` | `mPathStart` | kamome_auto_move 02189270: 1 when the gull takes off onto its path (mPathIdx set); kamome_path_move (in daKamome_Execute 021861D4): the first point then uses speed 25 / 2, then 0 |
| 0x79C | `m688` | `mHioSet` | daKamome_Create 02188D84: 1 when this gull created the l_kamomeHIO child (hio_set); daKamome_Delete 02188C0C deletes the HIO child only if set |

## ff_class (firefly; d_a_ff.h)

zeldaret/tww (d217e2b) names the other ff_class fields (kept) and keeps the GameCube offset names for the fields
below; TP has no such actor, so the name comes from the WWHD use (d_a_ff.cpp). Left open: m2EC (a copy of mHomePos
made at create, read once to place the HD point light) and the padding m2A8, m2AA, m315.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3DC | `m2C0` | `mVelAccel` | daFf_Execute 02133070 (mMode 4, until mTimers[4] runs out): added to mVelImpulse each frame (y capped at 10); x / z get a random cM_rndFX(1) one frame in ten, y is 2 |

## esa_class (bait; d_a_esa.h; copies in gc_api/d/actor/d_a_esa.h, d_a_bb.h, d_a_kamome.h; views KbFoodActorHD in d_a_kb.h, SoEsa in d_a_npc_so.h)

zeldaret/tww (d217e2b) names `mPhase`, `mGroundHeight`, `mActionState`, `mState`, `mpModel` and `mTimer` (kept) and
keeps the GameCube offset names for the fields below; TP has no such actor, so the names come from the WWHD uses in
d_a_esa.cpp and in the actors that use the bait (kamome, bb, npc_nz, npc_so). All copies and views take the same
names; the pig's accessors become esa_mTaken / esa_set_mTaken. field_0x2BA (parameter bits 8-15, it only gates the
extra baits) stays open.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B4 | `field_0x298` | `mTaken` | s_a_d_sub 0218566C (kamome) / s_a_d_sub 0205DB74 (bb) pick only baits at 0, search_esa 02185808 (kamome) / search_esa 0205DCFC (bb) set 1 on the chosen bait, bb_auto_move 020625E8 sets 0 again; daNpc_Nz_c::_searchEsa 022A2EEC chases only baits at 0; daNpc_So_c::cutEatesaProc 022E2754 / cutEatesaFirstProc 022E2830 set 0x23 (HD; GameCube 1), and daEsa_Execute 021300C4 then deletes the bait (a splash half of the time) |
| 0x3C0 | `field_0x2A4` | `mRippleCb` | esa_setRipple (in daEsa_Execute 021300C4): the callback of the ripple particle (rate 0.3), its emitter checked; daEsa_Delete 02130D6C ends it (dPa_rippleEcallBack_end); daEsa_Create 02130E30 constructs it; after TWW mRippleCallBack / mRipple (dPa_rippleEcallBack members of other actors) |
| 0x3D4 | `field_0x2B9` | `mAppendNum` | daEsa_Create 02130E30: parameter bits 0-7; while field_0x2BA (bits 8-15) is 0 that many more baits are created at Link's left hand (esaCreateAppend / esaCreate) |

## kantera_class (Bokoblin lantern; d_a_kantera.h; moth struct mo_ga_s)

zeldaret/tww (d217e2b) names the other kantera_class fields (kept) and keeps the GameCube offset names for the fields
below; TP has no such actor, so the names come from the WWHD uses (d_a_kantera.cpp; daKantera_Execute inlines ga_move
and the move code). The moth struct mo_ga_s takes the names of ep_class's ep_ga_s for the same uses. Left open: m2B0 /
m2B4 (set to cM_rndF(100) at create, never read), m2CA (no uses) and the padding.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x400 | `m2E4` | `mBonPosOld` | daKantera_Execute 0218D3F8: mBonPos copied in before mBonPos is moved; the step mBonPos - it tilts the lantern (x / z, capped at 0x4000) and ga_move (in daKantera_Execute) carries the moths along by 0.3 of it |
| 0x47C | `m35C` | `mBreakTimer` | daKantera_Execute 0218D3F8: 0x82 when the lantern hits the ground (state 10), counted down: the flame sphere grows while above 40 and attacks while above 110, the particles end at 30, the light fades below 30, the actor is deleted at 0 |

mo_ga_s (one moth around the lantern, mGa[2]):

| Offset in mo_ga_s | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x10 | `m10` | `mTargetPos` | ga_move (in daKantera_Execute 0218D3F8): random point within 150 / 30 / 150 of mBonPos, renewed when mTargetTimer runs out; the moth turns toward it (as ep_ga_s mTargetPos) |
| 0x2C | `m2C` | `mFlapAngle` | ga_move (in daKantera_Execute 0218D3F8): + 0x3E00 per frame, mScaleY = cM_ssin of it; daKantera_CreateHeap 0218E39C: random start (as ep_ga_s mFlapAngle) |
| 0x2E | `m2E` | `mbEnabled` | daKantera_CreateHeap 0218E39C: 1 for each moth with chance 0.5; ga_move (in daKantera_Execute 0218D3F8) skips a moth at 0; after TWW ep_ga_s mbEnabled (the same moth struct of the torch) |
| 0x2F | `m2F` | `mTargetTimer` | ga_move (in daKantera_Execute 0218D3F8): counted down; at 0 set to cM_rndF(5) with a new mTargetPos (as ep_ga_s mTargetTimer) |

## Small classes: kytag06_class, jbo_class, dr_class (and what stays open in kt, kytag00, kytag03, grass, vrbox, vrbox2)

zeldaret/tww (d217e2b) keeps the GameCube offset names for the fields below and TP has no such actors, so the names
come from the WWHD uses. Left open for lack of uses: kt_class field_0x2e4 / field_0x31d / field_0x31f (no uses),
kytag00_class field_0x290 (none) / field_0x296 (only zeroed at create), kytag03_class field_0x2a0 / field_0x2a8 (only
zeroed at create), kytag06_class field_0x290 (none), jbo_class m2BA (only set to 1, never read), dr_class m2B9 /
m2CA (no uses) and the padding m290, grass_class field_0x00 / field_0x04 (none), vrbox_class m29C (only zeroed at
create), vrbox2_class m290..m2A8 (padding, no uses). The himo2_dr_c view in d_a_himo2.h is dr2_class's and was renamed
with it.

kytag06_class (d_a_kytag06.h):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3B0 | `field_0x294` | `mRainLevel` | daKytag06_Create 021B0E78: 0; daKytag06_Execute 021B0D44: eased to 1 (cLib_addCalc 0.1 / 0.001); dKyw_rain_set(it * 250) whenever that is above g_env_light.mRainCount |

jbo_class (d_a_jbo.h):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D7 | `m2BB` | `mAction` | daJBO_Execute 021846B8: switch, case 0 runs jbo_move (as ssk_class mAction) |

dr_class (d_a_dr.h):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3E4 | `m2C8` | `mBikuRequest` | move (in daDr_Execute 0212A00C): when not 0 (or idle with l_HIO mDebugBiku / a quake) it is cleared and mMode 10 starts the flinch (BCK DR_BIKU1); nothing in d_a_dr sets it |
| 0x3E5 | `m2C9` | `mBikuDone` | move (in daDr_Execute 0212A00C): 0 when the flinch (mMode 10, DR_BIKU1) starts, 1 once its animation stopped; while 0, frame 15 sets mCountDownTimers[1] = 5 |

## pirate_flag_class and daPirate_Flag_packet_c (pirate ship flag; d_a_pirate_flag.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names for the fields below; TP has no such actor, so the names come
from the WWHD uses (d_a_pirate_flag.cpp). The packet's normal-angle fields and the array index have the same uses as
daSail_packet_c's and take its names. Left open: packet m87F (set to 1 in pirate_packet_construct, never read). The
GX2 draw members are not part of this pass.

pirate_flag_class:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3C4 | `m2A8` | `mWindPhase` | pirate_flag_move 023D1EA4: + l_HIO m08 per frame; the wind strength along the flag is l_HIO m10 .. m0C by 0.5 + 0.5 * sin of it |

daPirate_Flag_packet_c:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x7D4 | `m74C` | `mVtxVel` | pirate_flag_move 023D1EA4: [index] = the velocity of cloth vertex index: the spring / wind force is added, it is damped by 0.875 and added to the vertex position |
| 0x900 | `m878` | `mNrmAngle` | daPirate_Flag_packet_c::setCorrectNrmAngle 023D1804: 300 * sin(mNrmWobblePhase) + mNrmCorrectAngle; pirate_flag_move 023D1EA4: Y rotation of the normals; pirate_packet_construct 023D3008: 0 (as daSail_packet_c mNrmAngle) |
| 0x902 | `m87A` | `mNrmCorrectAngle` | daPirate_Flag_packet_c::setCorrectNrmAngle 023D1804: eased toward the wind-facing correction (step 0xC0), added to mNrmAngle; daPirate_Flag_Draw 023D116C adds it to the facing; pirate_packet_construct 023D3008: 0 (as daSail_packet_c mNrmCorrectAngle) |
| 0x904 | `m87C` | `mNrmWobblePhase` | daPirate_Flag_packet_c::setCorrectNrmAngle 023D1804: + 900 + a random step per call; sine phase of mNrmAngle; pirate_packet_construct 023D3008: 0 (as daSail_packet_c mNrmWobblePhase) |
| 0x906 | `m87E` | `mCurArr` | pirate_flag_move 023D1EA4: flipped (^ 1) each frame; index of the mPos / mNrm / mBackNrm array set in use (setNrmVtx 023D1A2C, setBackNrm 023D1E08, draw 023D0E68, daPirate_Flag_Create 023D33D4); pirate_packet_construct 023D3008: 0 (as daSail_packet_c mCurArr) |

## daHookshot_c (d_a_hookshot.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0xD4FC | `m2A0` | `mKeepMtx` | procWait 021789B4 sets it before the BG line check of the tip; execute 02176D98 skips rebuilding mMtx from the position and angle once while it is set and clears it |
| 0xD4FD | `m2A1` | `mWaterEffect` | execute 02176D98: the state byte passed to and returned by Link's setItemWaterEffect each frame (outside Mode_Wait) |
| 0xD4FF | `m2A3` | `mRebound` | procShot 02178DDC sets it when the tip hits a surface it cannot stick to (and stores mReboundEffectId / mReboundEffectAngle / mMtrlSndId); on the next check it plays the rebound particle, JA_SE_LK_HS_REBOUND and the shock vibration and returns; procWait 021789B4 clears it |
| 0xD500 | `m2A4` | `mCarryAngleY` | procShot 02178DDC stores the hit actor's shape_angle.y; procPlayerPull 02179888 rotates mCarryOffset by the carried actor's change of shape_angle.y and stores the new one |
| 0xD502 | `m2A6` | `mReboundEffectId` | procShot 02178DDC: the rebound particle, ID_IT_SN_LK_FKSHOT_SUNA00 on sand in kazeB, else ID_AK_JN_NG; played by procShot on mRebound |
| 0xD516 | `m2BA` | `mReboundEffectAngle` | procShot 02178DDC: cM3d_CalcVecZAngle of the hit polygon's plane, the rotation of the rebound particle (not for the sand particle) |


## esa_class (d_a_esa.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x3B4 | `field_0x298` | `mTaken` | s_a_d_sub 0218566C (kamome) / s_a_d_sub 0205DB74 (bb) pick only baits at 0, search_esa 02185808 (kamome) / search_esa 0205DCFC (bb) set 1 on the chosen bait, bb_auto_move 020625E8 sets 0 again; daNpc_Nz_c::_searchEsa 022A2EEC chases only baits at 0; daNpc_So_c::cutEatesaProc 022E2754 / cutEatesaFirstProc 022E2830 set 0x23 (HD; GameCube 1), and daEsa_Execute 021300C4 then deletes the bait (a splash half of the time) |
| 0x3C0 | `field_0x2A4` | `mRippleCb` | esa_setRipple (in daEsa_Execute 021300C4): the callback of the ripple particle (rate 0.3), its emitter checked; daEsa_Delete 02130D6C ends it (dPa_rippleEcallBack_end); daEsa_Create 02130E30 constructs it; after TWW mRippleCallBack / mRipple (dPa_rippleEcallBack members of other actors) |
| 0x3D4 | `field_0x2B9` | `mAppendNum` | daEsa_Create 02130E30: parameter bits 0-7; while mNoAppend (bits 8-15) is 0 that many more baits are created at Link's left hand (esaCreateAppend / esaCreate) |
| 0x3D5 | `field_0x2BA` | `mNoAppend` | daEsa_Create 02130E30: parameter bits 8-15; only while it is 0 the bait creates mAppendNum more baits at Link's left hand (tww the same; tww's REG0 debug override sets 1) |


## dScnKy_env_light_c (d_kankyo.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x101C | `field_0xc34` | `mColChgRatio` | the colour-change blend ratio: drawKankyo 0255A924 (the arrow colour changes, flags 0x10-0x60) and dKy_Itemgetcol_chg_move 0255A568 ease it toward 1 / 0 and pass it to dKy_actor_addcol_amb_set / dKy_bg*_addcol_amb_set and the dKy_set_*col_ratio calls, clearing mColChgFlag when it fades out; dScnKy_env_light_c ctor 025554CC / dKy_Create 0255DAC0 zero it (tww dKy_Itemgetcol_chg_move the same) |
| 0x10A0 | `field_0xc98` | `mColChgActive` | dKy_Itemgetcol_chg_move 0255A568 clears it and sets 1 while mColChgFlag & 7 (the item-get colour change runs); the thunder flash in d_kankyo_rain_movement_qa (02577138) skips the actor flash colours while it is set; ctor 025554CC / dKy_Create 0255DAC0 zero it (tww the same, read in d_kankyo_rain) |
| 0x10B8 | `field_0x10b8` | `mWaveSecondaryColorScale` | HD-only: dKankyo_wave_Packet::setColors 02582518 scales the wave texture's secondary colour (colorB) by it; dKy_Create 0255DAC0 initialises it (colorFogFlickerDivisor) |
| 0x10BC | `field_0x10bc` | `mWavePrimaryColorScale` | HD-only: dKankyo_wave_Packet::setColors 02582518 scales the wave texture's primary colour (colorA) by it; dKy_Create 0255DAC0 initialises it (colorFogFlickerDivisor) |

Open (offset names kept):

- Only the constructor 025554CC / dKy_Create 0255DAC0 write them, no read in WWHD or tww: `field_0x760`, `field_0xc2c`, `field_0xc30`, `field_0x102c`..`field_0x1048` (except 0x103c), `field_0xc44`, `field_0xc48`, `field_0xc4c`, `field_0xc54`, `field_0xc58`, `field_0xc5c`, `field_0xc68`, `field_0xc74`, `field_0xc78`, `field_0xc9a`, `field_0xc9b`, `field_0x10a4`..`field_0x10b4`, `field_0x10c0`..`field_0x10e4`; `field_0xa1c` (the wave weather code clears it only).
- `field_0xc8e`: only dKy_pship_existense_set 02560C34 / dKy_pship_existense_cut write it; no reader in WWHD or tww.
- `field_0x10e8` -> `mMonotoneBlend` (round 3): read through monotoneBlend() (d_kankyo_4.h): 0255C85C (HD monotone option, Hyroom) chases it and passes it as the blend factor of the monotone light presets; the constructor 025554CC and dKy_Create 0255DAC0 clear it.
- `field_0x103c` (skyBrightness(), d_kankyo_4.h: only set and eased toward its target, never read), `field_0x10c0`, `field_0x10d4` (shaderLightRatios() / shaderLightFactors(), d_kankyo_5.h: only set to 1): stay open.

### SND_INFLUENCE (d_kankyo.h)

`field_0x0` / `field_0xc` / `field_0x10` / `field_0x14` -> `mNoisePos` / `mNoiseLevel` / `mNoiseTimer` / `mNoiseActorId`: dKy_Sound_set 0255F458, Sndpos 0255B9AC, dKy_Sound_init 0255B930; tww fopAcM_otoCheck (evidence in the header).


## dSnap_Obj (d_snap.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x19 | `field_0x19` | `mCylSegments` | dSnap_Obj_ctor 025BD71C sets 4; CalcArea2D 025BDC78 projects that many cylinder side quads (cylvtx[i], cylvtx[i + 1]); SetInf 025BEB90 stores its fourth argument (tww: GXBegin(..., mCylSegments * 8) and the same loops) |
| 0x1B | `field_0x1b` | `mGeoFlags` | bit 0 = sphere geometry: SetGeoSph 025BEF3C sets it, SetGeo 025BEB5C clears it; CalcArea2D 025BDC78 / draw 025BF01C pick sphere or cylinder by it; ChkCamCull 025BE268 skips the angle cull for spheres; dSnap_Obj_ctor 025BD71C clears it |


## dSnap_RegistObjElm (d_snap.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x34 | `field_0x34` | `mPlayerDistSq` | dSnap_packet::Regist 025BE99C stores the squared distance from Link (+0x314) to the registered object; when the table is full it replaces the farthest entry with mPhotoNo >= 72; dSnap_packet::Execute 025BE870 resets it to 1e9 |


## dSnap_packet (d_snap.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x9C | `field_0x14` | `mRegistNum` | the number of used m_tbl entries: dSnap_packet::Regist 025BE99C appends at it and adds 1 (full at 63); dSnap_packet::Execute 025BE870 / dSnap_packet_ctor clear it; the judge and draw loops run up to it |


## dSeaFightGame_ship_data (d_seafightgame.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x8 | `field_0x8` | `mLength` | put_ship 025BB198 stores the ship size (2-4); daMgBoard_c's MgBoardShipHD view (d_a_mgameboard.cpp) picks the ship model by it (mLength there) (tww the same) |
| 0x9 | `field_0x9` | `mLife` | put_ship 025BB198 sets the ship size; attack 025BB470 subtracts 1 per hit and counts the ship as sunk (mAliveShipNum - 1, mDeadShipNum + 1) at 0 (tww the same) |
| 0xB | `field_0xb` | `mX` | put_ship 025BB198: the ship's first cell x; the board draw (d_a_mgameboard.cpp MgBoardShipHD::mX) places the ship model there (tww the same) |
| 0xC | `field_0xc` | `mY` | put_ship 025BB198: the ship's first cell y; the board draw (MgBoardShipHD::mY) places the ship model there (tww the same) |
| 0xD | `field_0xd` | `mLengthX` | put_ship 025BB198: the ship size when it lies along x (direction 1), else 0 (tww the same) |
| 0xE | `field_0xe` | `mLengthY` | put_ship 025BB198: the ship size when it lies along y (direction 0), else 0; the board draw (MgBoardShipHD::mVertical) turns the model by -0x8000 while it is non-zero, else 0x4000 (tww the same) |

Open (offset names kept):

- `field_0xa`: no use in WWHD or tww. dSeaFightGame_info_c `field_0x84[31]`: no use in WWHD or tww. The view MgBoardShipHD (d_a_mgameboard.h) took the class names mLife / mLengthX / mLengthY in a later '(misnamed)' commit (it had mHitCount / mDamage / mVertical).


## dOvlpFd2_c (d_ovlp_fade2.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `field_0x110` | `mTurnAngle` | execFadeOut 025A1FB4 adds mTurnSpeed to it each frame and moves on to the next snap when it crosses the next quarter turn (+-0x4000) with mTimer at 0, then sets -0x4000; execNextSnap 025A2150 adds mTurnSpeed once more (tww the same) |
|  | `field_0x112` | `mTurnSpeed` | execFadeOut 025A1FB4 chases it toward 2000 (step 100) and adds it to mTurnAngle; execNextSnap 025A2150 adds it once more (tww the same) |
|  | `field_0x11e` | `mSceneStarted` | execFadeIn 025A21F0: once the scale is below 0.001 the first frame calls fopOvlpM_SceneIsStart and sets it, the next ends the overlap (fopOvlpM_Done) (tww the same) |


## dStage_Event_dt_c (d_event_manager.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `field_0x0` | `mNextEventNo` | dEvent_manager_c::getToolId 02544708 follows the event entries through it (0xFF ends the chain) (tww the same); the event camera's next entry |
|  | `field_0x10` | `mMapToolCamId` | the event camera map tool id: maptoolIdEvCamera 02535830 (d_ev_camera_3.cpp, through the StageEvent view) picks the camera type by it |
|  | `field_0x11` | `mDuration` | the event camera duration in units of 10 frames: maptoolIdEvCamera 02535830 (through the StageEvent view) |


## StageEvent (d_ev_camera_3.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `field_0x10` | `mMapToolCamId` | maptoolIdEvCamera 02535830 passes it (with mRoomNo) to getCameraTypeFromMapToolID to pick the event camera type; same byte as dStage_Event_dt_c::mMapToolCamId |
|  | `field_0x11` | `mDuration` | maptoolIdEvCamera 02535830: the event camera keeps its style until mCurStyleTimer exceeds it * 10 (0xFF: until the event changes), then moves to the next event entry; same byte as dStage_Event_dt_c::mDuration |
|  | `field_0x14` | `mRoomNo` | maptoolIdEvCamera 02535830 passes it as the room to getCameraTypeFromMapToolID |


## dCcMassS_MngNative (d_cc_mass_s.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x128 | `field_0x128` | `mChkFlags` | the checks Chk 025170D8 runs, massFlag bits: ATTACK (1) the AT check against mCylAttr, COLLISION (2), AREA (8) the area objects, MOVE (16); Clear 025169A8 / Ct 02516A78 clear it; tww SetAttr(radius, height, it, camBit) sets it |


## RAIN_EFF (d_kankyo_rain.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `field_0x30` | `mGroundY` | rain_bg_chk 0256691C stores the camera centre y plus RainFloorOffset (the floor of the drop); tww rain_bg_chk the same, and tww dKyr_rain_move respawns the drop above the camera once it falls below it + 20 |

Open (offset names kept):

- `WAVECHAN` (view) `field_0x0`, `field_0x4`, `field_0x8`, `field_0x2f`: only cleared / initialised (dKyr_wave init); no read in WWHD or tww. `WINDEFF_SET` `field_0x759`: only cleared; no read. `RAIN_EFF` `field_0x1c`, `field_0x20`, `field_0x24`, `field_0x2c`: dKyr_rain_move 02566B88 (through the d_kankyo_rain_kazanbai_move.h copy) sets 0x1c / 0x20 to random angles and 0x24 at spawn and eases 0x24, but nothing reads them; 0x2c no use (round 3 correction: the copies' 0x30 is mGroundY, read by dKyr_rain_move). `dKankyo_rain_Packet` `field_0x3700`: no use. `POISON_EFF` `field_0x2c`: no use in WWHD.


## POISON_EFF (d_kankyo_rain.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `field_0x2e` | `mWaitTimer` | dKyr_poison_light_colision 0256C8FC sets 60 when a light arrow hits the poison particle; tww poison_move sets 180 / 120 / 60, counts it down and moves / damages only while it is 0 |


## dKankyo_poison_Packet (d_kankyo_rain.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `field_0xbb9c` | `mNearestPos` | poison_init 0256DC04 clears it; tww poison_move stores the particle position nearest to the camera, and tww dKyw_drawPoison passes it to dKyw_setDrawPacketListXluZPos as the packet's sort position |


## Small classes left open after a tww check (field naming round 2)

Each field below was checked against WWHD and zeldaret/tww d217e2b (the 2026-11-11 rule: a field WWHD writes only in a constructor / init may take tww's read role); none has a read that shows a role, so all keep their offset names.

| Header | Field | WWHD | tww |
|---|---|---|---|
| d_a_tama.h | TamaPartner `field_0x6ba` (WWHD 0x7D6) | daTama_c sets 1 on the partner when the ball hits the player | the same store; no reader of the partner's field in WWHD (fopNpc_npc_c 0x7D6 is only cleared by d_npc.cpp) |
| d_a_grass.h | `field_0x00`, `field_0x04` | no use | no use |
| d_a_sea.h | `m13A`, `m13B` | m13A set true once, m13B no use | m13A set once, no read |
| d_a_steam_tag.h | `m29A`, `m29B` | m29A no use; m29B stores the schbit parameter | m29B stored only |
| d_a_tori_flag.h | `field_0xF0`, `field_0xF2` | no use | no use |
| d_a_obj_homensmoke.h | `m294[...]` | unaccessed block | - |
| d_a_tag_ba1.h | hio_prm_c `field_0x0` | no use | - |
| d_a_obj_shmrgrd.h | `m85C` | cleared once | cleared once |
| d_salvage.h | `field_0x32` | no use | no use |
| d_a_am2.h | `m304` | copies of the position stored, never read | stored, never read |
| d_a_jbo.h | `m2BA` | set 1 once | set 1 once |
| d_a_pirate_flag.h | `m87F` | the cloth packet's constructor sets 1 | constructor only |
| d_a_bmdfoot.h | `m2C8[2]` | [0] counted down, never set non-zero in WWHD | counted down only |
| d_cloth_packet.h | `field_0x64`, `field_0xF0`, `field_0xF2` | 0x64 cleared; 0xF0 advanced by 0xF2, which is always 0, and never read | the same ("field_0xF2 is always set to 0") |
| d_jnt_hit.h | JntHit_c `field_0x1C`, JntHit_HIO_c `field_0x24` | cleared in the constructors only | no use |
| d_a_obj_gryw00.h, d_a_lstair.h | GrywBckState / the lstair copy `field_0x58`, `field_0x7C`..`field_0x88` | cleared by the inline SDK constructor only | no use |
| hd_snd_0202BD3C.h, hd_snd_020302E4.h | `field_0x8`, `field_0x10`, `field_0x0` | HD-only sound records, no read in the decomp | - |

The earlier open lists (people_event, os_c, btsw, ko1, bs1, ep, st, dk, cb1_action, ff, ac1, jbo, msw, att, rsh1, jb1) were checked again against that rule and tww: only daObjMmrr's attribute table (MmrrAttributes, read in WWHD and tww) and st_ke_s (the ke chain, read in WWHD) had evidence; the rest keep their recorded reasons (btsw's HIO timer / setWaitParm arguments have no parameter names in tww; msw m29A is only a switch with an empty case in tww; jb1 field_0x90C is read but never set non-zero).
## kita_class (Forbidden Woods hanging flower platform; d_a_kita.h)

Old names are tww's (`field_XXX`, the GameCube offsets, unnamed there). Named from the uses in d_a_kita.cpp.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x3B6 | `field_29A` | `mMode` | daKita_Execute 0219DF90 switches on it: 0 kita_move_wait (hanging on the vines), 1 kita_move_fall; kita_move_wait sets 1 once all four vines are cut, daKita_Create 0219F05C when mStartFloating; ride_call_back 0219DBFC tilts the platform only in mode 0 or on water |
| 0x3BC | `field_2A0` | `mSizeType` | daKita_Create 0219F05C: param bits 0-7 (0xFF -> 0); 1 / 2 scale the platform by 1.25 / 1.5 in x and z |
| 0x3BD | `field_2A1` | `mStartFloating` | daKita_Create 0219F05C: param bits 8-15; 1 starts the platform already fallen and floating (mMode 1, mOnWater 1, mFloorY = pos.y + 70) without creating vines |
| 0x3CC | `field_2B0` | `mSwayAmp` | kita_move_wait (in daKita_Execute 0219DF90): mPosRel.x / .z = sin * it, eased to 0; ride_call_back 0219DBFC raises it from the rider's movement |
| 0x3D8 | `field_2BC` | `mTiltAmp` | kita_move_wait / kita_move_fall (in daKita_Execute 0219DF90): amplitude of the mTilt wobble, eased toward 300 / 400 or 0; ride_call_back 0219DBFC raises it from the rider's movement; a hard landing sets 2000 |
| 0x3E4 | `field_2C8` | `mTilt` | kita_move_wait / kita_move_fall (in daKita_Execute 0219DF90): x / z = sin(mMoveCounter * n) * mTiltAmp; shape_angle = current.angle + it |
| 0x3F0 | `field_2D4` | `mShandIds` | himo_create (in daKita_Create 0219F05C): process IDs of the four fpcNm_SHAND_e vines it creates, looked up to link them |
| 0x400 | `field_2E4` | `mShandCreateStep` | himo_create (in daKita_Create 0219F05C): per vine 0 create, 1 link (pointers into mRopeRootPos / mRopeState), 2 done |
| 0x404 | `field_2E8` | `mRopeRootPos` | daKita_Execute 0219DF90: the four vine attachment points on the platform (xd / zd * scale, y -15) through the model matrix; himo_create hands each vine a pointer (KitaShandView::mpRootPos) |
| 0x434 | `field_318` | `mRopeState` | himo_create (in daKita_Create 0219F05C) hands each vine a pointer (KitaShandView::mpParentState); kita_move_wait (in daKita_Execute 0219DF90) reads it: non-zero = that vine is cut (mask of himo_off_check), 1 -> 5 and 2 -> 0 restart mCutEase |
| 0x43C | `field_320` | `mCutEase` | kita_move_wait (in daKita_Execute 0219DF90): cleared when a vine's state changes, eased to 1; scales the mHeight step (250 * it) and the mRotX / mRotY turn speed (10000 * it) |
| 0x478 | `field_35C` | `mFloorY` | kita_move_wait (in daKita_Execute 0219DF90): the highest ground (solid ground cross or water + 40) under the platform once it falls; kita_move_fall lands on it; daKita_Create 0219F05C sets pos.y + 70 for mStartFloating |
| 0x47C | `field_360` | `mOnWater` | kita_move_wait (in daKita_Execute 0219DF90) sets 1 when water is below; kita_move_fall sets 2 when it lands there (splash, FFHAMON00 ripple, the push sphere and drift run while 2); ride_call_back 0219DBFC tilts while set |
| 0x480 | `field_364` | `mDriftSpeed` | kita_move_fall (in daKita_Execute 0219DF90): eased toward mDriftSpeedTarget while mPushTimer runs, else to 0; speed.x / .z chase 2 * it along mPlayerAngle |
| 0x488 | `field_36C` | `mDriftSpeedTarget` | kita_move_fall (in daKita_Execute 0219DF90): set on a sphere hit from the player's facing (|cos| * -6 or 6) |
| 0x48C | `field_370` | `mAngleYSpeedTarget` | kita_move_fall (in daKita_Execute 0219DF90): set on a sphere hit (sin * 200 * distance factor, or random 100); mAngleYSpeed eases toward it while mPushTimer runs |
| 0x490 | `field_374` | `mPushTimer` | kita_move_fall (in daKita_Execute 0219DF90): 20 on a sphere hit, counted down; the drift and turn chase their targets while it runs |

Open: `unused_324` (no use; tww names it unused), the padding `_3BE`, `_3EA`, `_47D`.
## DEMO_PLIGHT (demo point light; d_kankyo_demo.h)

Named from zeldaret/tww's code (d217e2b, src/d/d_kankyo_demo.cpp, field_0x00 / field_0x04) and our decomp's use.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `field_0x04` | `mType` | dKydm_demo_plight_entry 02563ABC stores type + group * 100 (picks plight / efplight set), then the type; dKydm_demo_plight_delete 02563BF0 cuts the plight below 100, else the efplight (tww the same) |

Open: `field_0x00` (only cleared by dKydm_demo_plight_entry 02563ABC, never read in WWHD or tww).

## dMagma_ballBoss_c (Gohma's magma balls; d_boss_magma.h)

zeldaret/tww (d217e2b, include/d/d_magma.h) has field_0x7C with no code; the name comes from the WWHD use.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0xDD0 | `field_0x7C` | `mWaveSpeed` | dMagma_ballBoss_c::calc 024F60E8 adds it to mWave each frame (the sine bob of mPos.y), sets 1000 near the boss wave ring; setup 024F637C 400 + cM_rndF(100) |

## Still open after round 2

Re-checked in round 2, no name (unchanged): kytag00_class `field_0x290`, `field_0x296`; vrbox_class `m290`, `m298`, `m29C`; vrbox2_class `m290`, `m298`, `m2A0`, `m2A8`; dr_class `m290`, `m2B9`, `m2CA`; ssk_class `m35C` (padding); daMajuu_Flag_packet_c `m796`, `m798`, `m79B` (set only by the packet constructor 021B98B4); SieFlagClothHD `field_0xF0` / `field_0xF2` (view of dCloth_packet_c, unnamed there); PointWind::WIND_INFLUENCE `field_0x20` (view of WIND_INFLUENCE, d_kankyo.h, unnamed there).
## dOvlpFd4_c (scene change overlap, fade type 4; d_ovlp_fade4.h)

zeldaret/tww (d217e2b, include/d/d_ovlp_fade4.h) keeps the GameCube offset names; the name comes from the WWHD use (d_ovlp_fade4.cpp), the same code as tww's execFadeIn.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x368 | `field_0x2cc` | `mSceneStarted` | dOvlpFd4_construct 025A2D24 clears it; execFadeIn 025A3224: when the timer and blur are done, the first time calls fopOvlpM_SceneIsStart and sets it, the next time ends the overlap (fopOvlpM_Done) (tww the same) |

Open (no name yet): `field_0x2b8`, `field_0x2bc`, `field_0x2c2`, `field_0x2c4` (dOvlpFd4_construct only clears them in WWHD).

## dScnName_c (name entry scene; d_s_name.h)

zeldaret/tww (d217e2b, include/d/d_s_name.h) keeps the GameCube offset name (GameCube offset 0x55F, WWHD 0x534); the name comes from the WWHD use (d_s_name.cpp) and tww's changeGameScene, which reads it the same way.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x534 | `field_0x55f` | `mToOpenScene` | NameInMain 025ADD1C copies the name controller's choice when the name is accepted; changeGameScene 025ADC60 requests OPEN_SCENE while it is set, else PLAY_SCENE (tww dScnName_c sets 1 after the name input, 0 on its other path) |

Open (no name yet): the private views `Controller::field_0x145` (the HD name controller; only copied into mToOpenScene), `PlayView::field_0x62F1` (three other views call this play byte mBrightness / mBackAlpha / mWindowAlpha; no class declaration to follow). Round 3: `SaveView::field_0x7BC` -> `mDanStageNo` (dSv_info_c::mDan.mStageNo; changeGameScene 025ADC60 sets 255 where tww calls dComIfGs_resetDan()).

## daTagPhoto_c (Lenzo's photo talk tag; d_a_tag_photo.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_tag_photo.h) keeps the GameCube offset names; the names come from the WWHD use (d_a_tag_photo.cpp), the same code as tww.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x42C | `field_0x310` | `mpMsgSeq` | eventMesSetInit 024B1F30 / getMsg 024B1FD4 clear it; next_msgStatus 024B1FF0 steps it through a 0-terminated message number list while it is set and clears it at the end (tww the same) |
| 0x438 | `field_0x31C` | `mTalkEventSet` | daTagPhoto_construct 024B1D20 clears it; executeWait 024B24A4: the first time the player comes within 160 it is set and the photo talk event is ordered unless Po is on a date (tww the same) |

The view `PhotoLenzo::field_0x9C1` takes daNpcPhoto_c's name `mPlaceState` (round 3).

## ShopCursor_c (shop selection cursor; d_shop.h; views Bs1ShopCursor, Bms1ShopCursor, RshShopCursor in d_a_npc_bs1.h / bms1.h / rsh1.h)

Round 4 (two-digit offset names). Four cursor pieces that pulse between a far and a near distance around the selected item; the shop NPCs (bs1, ds1, bms1, bmsw, btsw, rsh1) set the scale values from their HIO through setScale and show / hide it. Same meanings in zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x98 | `m38` | `mPieceDistFar` | ShopCursor_c::anm_play 025BD290: mPieceDist switches between it and mPieceDistNear; the ctor ShopCursor_c_ct sets 30, the shop NPCs' setScale sets their HIO value (tww the same) |
| 0x9C | `m3C` | `mPieceDistNear` | ShopCursor_c::anm_play 025BD290: mPieceDist switches between mPieceDistFar and it; the ctor sets 25, the shop NPCs' setScale their HIO value (tww the same) |
| 0xA0 | `m40` | `mPieceDist` | ShopCursor_c::draw 025BD338: each of the four cursor pieces is moved it * sqrt(2) out from mPos; anm_play 025BD290 sets it to the far or near distance (tww the same) |
| 0xA4 | `m44` | `mDistSwitchTimer` | ShopCursor_c::anm_play 025BD290: counts down; at 0 mPieceDist switches and it is reloaded with 15 + random (tww the same) |
| 0xA8 | `m48` | `mPieceScale` | ShopCursor_c::draw 025BD338: the pieces' scale (y times mPieceScaleYFar / mPieceScaleYNear); ShopCursor_create's scale argument, setScale's first (tww the same) |
| 0xAC | `m4C` | `mPieceScaleYFar` | ShopCursor_c::draw 025BD338: the pieces' y scale factor while mPieceDist is the far distance (tww the same) |
| 0xB0 | `m50` | `mPieceScaleYNear` | ShopCursor_c::draw 025BD338: the pieces' y scale factor while mPieceDist is the near distance (tww the same) |
| 0xB4 | `m54` | `mbShow` | ShopCursor_c::draw 025BD338 draws only while it is set; the shop NPCs' show() / hide() set 1 / 0 and test it (tww the same) |

Open (no name yet): padding `_0A6`, `_0B5`.

## ShopCam_action_c (shop camera; d_shop.h; views Bs1ShopCam, Bms1ShopCam, RshShopCamera, daNpc_Ds1_c::mShopCam)

Round 4 (two-digit offset names). The camera the shop actions drive: the center, eye and fovy passed to the camera's Set, and the selected item index that picks shop_cam_data_tbl's zoomed row. Same meanings in zeldaret/tww d217e2b; tww's mOrigCenter / mOrigEye / mOrigFovy (Save / Reset) gave the pattern.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x10 | `m18` | `mCenter` | shop_cam_action 025BBF4C, rsh_talk_cam_action 025BC38C, ds_normal_cam_action 025BC4DC pass it to the camera's Set as the center (shop_cam_action eases it towards shop_cam_data_tbl's center); the NPCs' inline getItemZoomPos aims from mEye towards it (tww the same) |
| 0x1C | `m24` | `mEye` | the same three actions pass it to the camera's Set as the eye; the NPCs' inline getItemZoomPos returns mEye + normalize(mCenter - mEye) * distance (tww the same) |
| 0x28 | `m30` | `mFovy` | the same three actions pass it to the camera's Set as the fovy; shop_cam_action 025BBF4C eases it towards the table's fovy; ShopCam_action_c_ct 025BBDF0 sets 60 (tww the same) |
| 0x4C | `m54` | `mSelectIdx` | shop_cam_action 025BBF4C uses the table's second (zoomed) row while it is >= 0; the NPCs set it to mShopItems.mSelectedItemIdx, the init actions and ShopCam_action_c_ct to -1 (tww the same) |

Open (no name yet): `m50`, `m52` (ShopCam_action_c_ct 025BBDF0 clears them; read nowhere in WWHD or tww).

## himo2HIO_c (l_himo2HIO; d_a_himo2.h)

Round 4 (two-digit offset names). The Grappling Hook's target search tolerances (setTargetPos, the inline search_target), the rope width while locked on / free and the swing direction mode. Same meanings in zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x02 | `m06` | `mSwingDirMode` | read only by tww: daHimo2_Execute's switch picks the swing directions (0 random, 1..4 the four fixed sign pairs); WWHD writes it only in __sinit_d_a_himo2_cpp 0216F6D8 (0) and new_himo2_move always takes (-1, -1) (2026-11-11 rule: tww's one role) |
| 0x04 | `m08` | `mTargetAngleRange` | setTargetPos 0216EEB8 (inline search_target): a target counts only while its angle to Link lies within 0x4000 +- it (plus a debug register); HIO 15000 (tww the same) |
| 0x08 | `m0C` | `mPitchTolBelow` | setTargetPos 0216EEB8: a normal target is taken only while its pitch from the camera eye exceeds the aim point's by less than it (2000 for flagged targets); HIO 600 (tww the same) |
| 0x0A | `m0E` | `mPitchTolAbove` | setTargetPos 0216EEB8: a target is taken only while the aim point's pitch exceeds the target's by less than it; HIO 400 (tww the same) |
| 0x0C | `m10` | `mTargetRadius` | setTargetPos 0216EEB8: the yaw tolerance is cM_atan2s(target scale.z * it, distance from the camera eye), the target's radius per unit of scale; HIO 60 (tww the same) |
| 0x10 | `m14` | `mTargetMaxDY` | setTargetPos 0216EEB8: a normal target is taken only while its height offset in Link's frame is below it (plus a stage tolerance on ADMumi); HIO 1500 (tww the same) |
| 0x14 | `m18` | `mSearchDistMax` | setTargetPos 0216EEB8: the search distance grows by 100 per pass and stops above it (2000 for flagged targets); HIO 1000 (tww the same) |
| 0x18 | `m1C` | `mRopeScaleLockOn` | daHimo2_Execute 0216E194 and new_himo2_move 0216F7FC (while Lockon) store it in rope_scale, the rope lines' width for lineMat_update; HIO 2.3 (tww the same) |
| 0x1C | `m20` | `mRopeScale` | new_himo2_move 0216F7FC stores it in rope_scale while nothing is locked on; HIO 1.9 (tww the same) |

Open (no name yet): `m0A` (set to 1000 by __sinit_d_a_himo2_cpp, read nowhere in WWHD or tww), padding `_01`.

## daPirate_Flag_HIO_c (l_HIO; d_a_pirate_flag.h)

Round 4 (two-digit offset names). The pirate ship flag's cloth simulation values (wind wave, gravity, spring) and two debug switches. Same meanings in zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x001 | `m05` | `mDrawDebugLines` | daPirate_Flag_Draw 023D116C: while it is set, lines for the first light's direction, the flag's facing and its escape-normal angle are drawn around the flag; __sinit 0 (tww the same) |
| 0x003 | `m07` | `mStopMove` | daPirate_Flag_Execute 023D2610 returns at once while it is set (the cloth stops moving); __sinit 0 (tww the same) |
| 0x004 | `m08` | `mWindPhaseStep` | pirate_flag_move 023D1EA4 adds it to mWindPhase every frame; the sine of that phase blends the wind strength; __sinit 0x40 (tww the same) |
| 0x008 | `m0C` | `mWindStrengthMax` | pirate_flag_move 023D1EA4: the wind strength is it * f + mWindStrengthMin * (1 - f), f = 0.5 + 0.5 sin(mWindPhase); __sinit 13 (tww the same) |
| 0x00C | `m10` | `mWindStrengthMin` | pirate_flag_move 023D1EA4: the wind strength's value at the low point of the wind wave (see mWindStrengthMax); __sinit 7 (tww the same) |
| 0x010 | `m14` | `mGravity` | pirate_flag_move 023D1EA4 adds it to each cloth point's y force; __sinit -3.5 (tww the same) |
| 0x014 | `m18` | `mSpringRate` | get_cloth_anim_sub_factor 023D176C: the force between neighbouring cloth points is (length - rest length) * it; __sinit 0.45 (tww the same) |
| 0x018 | `m1C` | `mEscapeNrmAngle` | daPirate_Flag_packet_c::setCorrectNrmAngle 023D1804: the escape angle in degrees (scaled by 1 - wind strength / 2) the normals turn to when the wind blows along the flag; __sinit 0 (tww the same) |

Open (no name yet): `m06` (set to 0 by __sinit_d_a_pirate_flag_cpp 023D3A90, read nowhere in WWHD or tww).

## himo3HIO_c and h3_ga_s (himo3HIO, himo3_class::mGa[0]; d_a_himo3.h)

Round 5 (two-digit offset names). The hanging rope (ぶら下がりロープ): the HIO's end model scales, glow scale and tww's alpha-model type and colour; the moth (ga) that flies around the rope end of type 0. Same meanings in zeldaret/tww d217e2b. In WWHD the HIO has the vtable last, so its offsets are 4 lower than the old names.

himo3HIO_c:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x02 | `m06` | `mAlphaModelType` | read only by tww: daHimo3_Draw picks the glow's alpha model, 0 one sphere, 1 two spheres (both in the mAlphaColorR / G / B colour), 2 the spot model; WWHD writes it only in __sinit_d_a_himo3_cpp 02174F64 (0) and has no alpha model (2026-11-11 rule: tww's one role) |
| 0x04 | `m08` | `mEndScaleType1` | himo3_control (inlined in daHimo3_Execute 02172A00): the end model's scale for type 1; __sinit_d_a_himo3_cpp 02174F64 0.75 (tww the same) |
| 0x08 | `m0C` | `mEndScale` | himo3_control (inlined in daHimo3_Execute 02172A00): the end model's scale for the other types, which also turn it by -0x4000 about x; __sinit_d_a_himo3_cpp 02174F64 0.5 (tww the same) |
| 0x0C | `m10` | `mAlphaColorR` | read only by tww: daHimo3_Draw's alpha model colour r (dComIfGd_setAlphaModelColor); WWHD writes it only in __sinit_d_a_himo3_cpp 02174F64 (0xEB) (2026-11-11 rule: tww's one role) |
| 0x0E | `m12` | `mAlphaColorG` | read only by tww: daHimo3_Draw's alpha model colour g; WWHD writes it only in __sinit_d_a_himo3_cpp 02174F64 (0x7D) (2026-11-11 rule: tww's one role) |
| 0x10 | `m14` | `mAlphaColorB` | read only by tww: daHimo3_Draw's alpha model colour b; WWHD writes it only in __sinit_d_a_himo3_cpp 02174F64 (0) (2026-11-11 rule: tww's one role) |
| 0x14 | `m18` | `mGlowMtxScale` | daHimo3_Execute 02172A00 copies it into the actor's mGlowScale every frame (the glow matrix scale); __sinit_d_a_himo3_cpp 02174F64 5.0 (tww m1870 = it) |

h3_ga_s:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x10 | `m10` | `mTargetPos` | ga_move (inlined in daHimo3_Execute 02172A00): a new random point within 150 / 100 / 150 of the rope end (mEndPos) whenever mRetargetTimer runs out; the moth turns toward it (tww the same) |
| 0x1C | `m1C` | `mAngle` | ga_move: yaw / pitch eased toward mTargetPos by 0x1000 steps, the direction the moth flies; daHimo3_Draw 021727D4 (ga_draw inlined) turns its model by it (tww the same) |
| 0x24 | `m24` | `mScale` | daHimo3_Create 02174730 (useHeapInit inlined): random 0.3..0.6; daHimo3_Draw (ga_draw) scales the moth model by it (tww the same) |
| 0x28 | `m28` | `mFlapScale` | ga_move: sin(mFlapPhase); daHimo3_Draw (ga_draw) multiplies the model's y scale by it, the wing flap (tww the same) |
| 0x2C | `m2C` | `mFlapPhase` | ga_move adds 0x3E00 per frame and takes mFlapScale = sin(it); daHimo3_Create (useHeapInit) random start (tww the same) |
| 0x2E | `m2E` | `mActive` | daHimo3_Create (useHeapInit) sets it for type 0; ga_move moves and daHimo3_Draw (ga_draw) draws the moth only while it is set (tww the same) |
| 0x2F | `m2F` | `mRetargetTimer` | ga_move counts it down and, at 0, sets a random 0..10 and picks a new mTargetPos (tww the same) |

Open (no name yet): padding `himo3HIO_c::_01`, `_12`, `h3_ga_s::_22`.


## himo2_s (himo2_class::mSegments[100] / m1120[100]; d_a_himo2.h)

Round 6 (two-digit offset names). One Grappling Hook rope segment: its position and the yaw / pitch its model is drawn with. Same meanings in zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x10 | `m10` | `mPos` | himo2_control / himo2_control2 (inlined in daHimo2_Execute 0216E194) move each rope segment from its neighbour's position; daHimo2_Execute 0216E194 takes the ground height below it; himo2_draw (in daHimo2_Draw 0216CA5C) puts the segment model there (tww the same) |
| 0x1C | `m1C` | `mAngleX` | himo2_control2 (in daHimo2_Execute 0216E194) stores the segment's pitch toward the next one; himo2_draw (in daHimo2_Draw 0216CA5C) turns the segment model by it about x (tww the same) |
| 0x1E | `m1E` | `mAngleY` | himo2_control2 (in daHimo2_Execute 0216E194) stores the segment's yaw toward the next one; himo2_draw (in daHimo2_Draw 0216CA5C) turns the segment model by it about y (tww the same) |

Open (no name yet): `m0C` (daHimo2_Execute 0216E194 stores the ground height below the segment + 5, or -100000 while launched free; read nowhere in WWHD or tww), padding `m00`, `m20`.


## kamomeHIO_c (l_kamomeHIO; d_a_kamome.h)

Round 6 (two-digit offset names). The seagull's HIO: a stop switch, the forward-step frame window, the take-off distance and the player distance of the seagulls with Aryll. Same meanings in zeldaret/tww d217e2b; in WWHD the vtable is last, so the offsets are 4 lower than the old names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x01 | `m05` | `mStopMove` | daKamome_Execute 021861D4 runs the timers, the move functions and the animation only while it is 0; __sinit_d_a_kamome_cpp 02189180 0 (tww the same) |
| 0x02 | `m06` | `mMoveStartFrame` | kamome_path_move (inlined), kamome_imouto_move (inlined) and kamome_auto_move 02189270: from this animation frame to mMoveEndFrame the seagull moves forward (speedF scale * 5), outside it stands; __sinit_d_a_kamome_cpp 02189180 1 (tww the same) |
| 0x04 | `m08` | `mMoveEndFrame` | the same three move functions: the last animation frame of the forward step window; __sinit_d_a_kamome_cpp 02189180 8 (tww the same) |
| 0x08 | `m0C` | `mTakeOffDist` | kamome_auto_move 02189270: a seagull on the ground takes off (state 0, the bait released) when Link comes within it; __sinit_d_a_kamome_cpp 02189180 400 (tww the same) |
| 0x0C | `m10` | `mImoutoPlayerDist` | kamome_imouto_move (inlined in daKamome_Execute 021861D4), the seagulls with Aryll: Link within it of Aryll or of the target makes the seagull drop it, and a landed one takes off; __sinit_d_a_kamome_cpp 02189180 300 (tww the same) |

Open (no name yet): padding `_06`.


## daKanban_HIO_c (l_HIO; d_a_kanban.cpp)

Round 9 (two-digit offset names). The cuttable sign's HIO; only the model scale is read. Same in zeldaret/tww; the vtable is first, so the offsets equal the old names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0C | `m0C` | `mModelScale` | daKanban_Draw 0218A548 sets the signboard's scale (x, y, z) to it before drawing the parts; daKanban_HIO_c_ct 0218CE48 0.8 (tww daKanban_Draw the same) |

Open (no name yet): `m04` (0), `m08` (0), `m0A` (0), set by daKanban_HIO_c_ct 0218CE48 and read nowhere in WWHD or tww.

## Round 11 small classes

Small classes named in round 11; each table is its own commit.

### daNpc_Ba1_HIO_c::hio_prm_c (l_HIO.mPrmTbl; d_a_npc_ba1.h)

Round 11. tww d_a_npc_ba1.cpp keeps the offset names and reads them the same way.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x18 | `m18` | `mDebugDraw` | _draw 021F128C: the debug-colour block runs only while it is set (tww _draw the same: tww d_a_npc_ba1.cpp _draw guards only an unused debug position with it, the same shape as the block before d_a_npc_bm1.cpp's stripped "// Debug:" dDbVw_drawSphereXlu / dDbVw_drawCircleOpa calls) |
| 0x1C | `m1C` | `mUseFairyRadius` | check_useFairyArea 021F1DA8: the player counts as in the fairy area within it (XZ, and within 100 in y) (tww the same) |
| 0x20 | `m20` | `mFairyEventRadius` | wait_0 021F1F0C: within it (XZ, 100 in y) tmp bit 0x0310 is set and event 6 is ordered (tww the same) |

### daTag_So_HIO_c (l_HIO; d_a_tag_so.h)

Round 11. WWHD keeps the vtable last (offset 1 = GameCube 5). The constructor clears it.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x01 | `m05` | `mDebugDraw` | _draw 024B2C3C calls debugDraw while it is set (tww the same) |

### daDr_HIO_c (l_HIO; d_a_dr.h)

Round 11. WWHD keeps the vtable last (offsets 4 lower). The constructor sets false / 300.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x08 | `m0C` | `mDebugBiku` | move (in daDr_Execute 0212A00C): when set while idle the flinch (mode 10) starts and it is cleared (debug trigger, like a quake) (tww the same) |
| 0x0A | `m0E` | `mBikuTime` | move (in daDr_Execute 0212A00C) mode 10: mCountDownTimers[0] of the flinch (tww the same) |

### daDr2_HIO_c (l_HIO; d_a_dr2.h)

Round 11. WWHD keeps the vtable last (offset 4 = GameCube 8). The constructor sets 1.0.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `m08` | `mScale` | daDr2_Execute 0212B3D0: scale.x/y/z are set to it every frame (tww the same) |

### daBgn3_HIO_c (l_HIO; d_a_bgn3.h)

Round 11. The other HIO fields were named earlier (bosses.md).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x28 | `m2C` | `mSpl1HitTime` | hioCtor 020914F4 sets 120; damage_check 02090384: the shake duration of a hit with mSpl 1 (next to mSwordHitTime / mArrowHitTime / mOtherHitTime) (tww the same) |

### dDoor_hkyo_c (d_door_hkyo.h)

Round 11. tww's dDoor_hkyo_c (d_door.h) keeps m11 and reads it through chkUse / onUse / offUse.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x11 | `m11` | `mUseMode` | dDoor_hkyo_construct 0252C0BC clears it; resLoad / proc 0252C574 and the other methods act only while it is not 0, proc picks the animation by it (1, 4, ...) (tww chkUse / onUse(arg) / offUse) |

### dDoor_key2_c (d_door_key.h)

Round 11. The copies dDoor_key2_c in d_door_smoke.h and Door10KeyHD in d_a_door10.h declare m20 in a comma list and follow in a later round. tww dDoor_key2_c (d_door.h) keeps m20.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x9C | `m20` | `mbUnlocking` | keyInit 0252B4A4 sets 1 when the key door is opened (switch on, key used, lock sound), else 0; keyProc 0252B5B4 plays the unlock bck only while it is set and clears it at the end (tww the same) |

## Round 12 small classes

Classes named in round 12; each table is its own commit.

### CcAtInfo and its copies, the hit angle

Round 12. CcAtInfo (d_cc_uty.h) and the 13 per-unit copies that still had m0C; the copies in d_a_mo2_exec.h, d_a_mt_exec.h and d_a_pt.h were already named mHitAngle. tww keeps m0C.

| Struct | WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|---|
| CcAtInfo (d_cc_uty.h) | 0x0C | `m0C` | `mHitAngle` | cc_at_check 025192A8 sets .y = cc_atan2s(-x, -z) of the hit direction (the attacker's speed, or the attacker-to-target offset when it is slow); damage_check of bk (in daBk_Execute 0209BD1C) rotates calc_mtx by .y (cMtx_YrotS) for the knock-back, bb's damage_check 0206432C copies .y into mKnockbackAngleY (tww the same) |
| AttackInfo (d_a_st.h) | 0x0C | `m0C` | `mHitAngle` | copy of CcAtInfo; see the row above |
| bkExecAtInfo (d_a_bk_exec.h) | 0x0C | `m0C` | `mHitAngle` | copy of CcAtInfo; see the row above |
| CcAtInfo_gnd (d_a_gnd.h) | 0x0C | `m0C` | `mHitAngle` | copy of CcAtInfo; see the row above |
| CcAtInfo_ss (d_a_ss.h) | 0x0C | `m0C` | `mHitAngle` | copy of CcAtInfo; see the row above |
| CcAtInfo_bdk (d_a_bdk_exe.h) | 0x0C | `m0C` | `mHitAngle` | copy of CcAtInfo; see the row above |
| CcAtInfo (d_a_bridge.h) | 0x0C | `m0C` | `mHitAngle` | copy of CcAtInfo; see the row above |
| CcAtInfo_l (d_a_rd.h) | 0x0C | `m0C` | `mHitAngle` | copy of CcAtInfo; see the row above |
| CcActionAtInfo (d_a_cc_act.h) | 0x0C | `m0C` | `mHitAngle` | copy of CcAtInfo; see the row above |
| CcAtInfo_l (d_a_bgn2.h) | 0x0C | `m0C` | `mHitAngle` | copy of CcAtInfo; see the row above |
| CcAtInfo_oq (d_a_oq.h) | 0x0C | `m0C` | `mHitAngle` | copy of CcAtInfo; see the row above |
| BdkObjAttackInfo (d_a_bdkobj.h) | 0x0C | `m0C` | `mHitAngle` | copy of CcAtInfo; see the row above |
| CcAtInfo_l (d_a_fm_local.h) | 0x0C | `m0C` | `mHitAngle` | copy of CcAtInfo; see the row above |
| KiAttackInfo (d_a_ki.h) | 0x0C | `m0C` | `mHitAngle` | copy of CcAtInfo; see the row above |

### dDoor_smoke_c (d_door_smoke.h) and its view KddoorDoorSmoke (d_a_kddoor.h)

Round 12. The door smoke's wobble counter and started flag. KddoorDoorSmoke is d_a_kddoor's view of the same struct and takes the same names. tww dDoor_smoke_c (d_door.h) keeps m34 / m35.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x34 | `m34` | `mWaveStep` | smokeProc 0252B2D8: step counter of the smoke wobble, moves mPos sideways by step * 20 with alternating sign and counts up each frame; dDoor_smoke_ct 0252B190 / smokeInit 0252B1E8 clear it (tww the same) |
| 0x35 | `m35` | `mbStarted` | smokeProc 0252B2D8: 0 on the first call, which only sets it to 1; the wobble runs once it is set; dDoor_smoke_ct 0252B190 / smokeInit 0252B1E8 clear it (tww the same) |

### dDoor_key2_c copies (d_door_smoke.h, d_a_door10.h)

Round 12. The two comma-list copies of dDoor_key2_c named in round 11 (d_door_key.h, `mbUnlocking`), split in the commit before, follow its name.

| Struct | WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|---|
| dDoor_key2_c (d_door_smoke.h) | 0x9C | `m20` | `mbUnlocking` | the same struct as d_door_key.h's; dDoor_key2_ct 0252B3D0 clears it |
| Door10KeyHD (d_a_door10.h) | 0x9C | `m20` | `mbUnlocking` | view of dDoor_key2_c; no uses in d_a_door10 |

### ShopItems_c__shop_cam_data (d_shop.h)

Round 12. One row of shop_cam_data_tbl (normal and zoomed row per shop): the camera centre, eye and fovy that ShopCam_action_c's shop actions copy or ease towards. The names are ShopCam_action_c's (round 4) for the same values; tww keeps m00 / m0C / m18.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mCenter` | shop_cam_action_init 025BBE98 copies it into ShopCam_action_c::mCenter, shop_cam_action 025BBF4C eases mCenter towards it; the camera Set center (tww the same; one row per shop and zoom state in shop_cam_data_tbl) |
| 0x0C | `m0C` | `mEye` | shop_cam_action_init 025BBE98 copies it into ShopCam_action_c::mEye, shop_cam_action 025BBF4C eases mEye towards it; the camera Set eye (tww the same) |
| 0x18 | `m18` | `mFovy` | shop_cam_action_init 025BBE98 copies it into ShopCam_action_c::mFovy, shop_cam_action 025BBF4C eases mFovy towards it; the camera Set fovy (tww the same) |

### __shop_items_set_data (d_shop.h)

Round 12. The shop item set entry's sold-out message, next to mShowMsgNo / mBuyMsgNo. tww keeps m0C.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0C | `m0C` | `mSoldOutMsgNo` | dShop_now_triggercheck 025BB698 shows it instead of mShowMsgNo when the selected item is sold out (tww the same: *pMsgNo = itemSetList[nextIdx]->m0C when isSoldOutItem) |

### ShopItems_c (d_shop.h)

Round 12. The zoom-up target of the selected shop item and its flag. tww keeps m30 / m3C.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x30 | `m30` | `mZoomPos` | Item_ZoomUp 025BCC7C stores the position it is given; Item_Select 025BCA2C moves the selected item towards it (minus the item centre) while mbZoomUp is 1 (tww the same) |
| 0x3C | `m3C` | `mbZoomUp` | Item_ZoomUp 025BCC7C sets 1, Item_Move 025BCCA4 clears it after each pass, ShopItems_c_ct 025BC760 clears it; Item_Select 025BCA2C moves the item to mZoomPos while it is 1, else back home (tww the same) |

### st_nun_s (d_a_st.h)

Round 12. The Stalfos' pulled-off arm swung as a nunchaku (st_class::mNun): three chain points with position, angle and velocity, like st_ke_s. tww keeps m00 / m24 / m38.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mPos` | nun_control 0249196C: the positions of the 3 nunchaku chain points; point 1 is placed at the hand joint, point 2 moves from it by the swung offset and is drawn there (tww nun_control the same) |
| 0x24 | `m24` | `mAngle` | nun_control 0249196C stores the pitch / yaw of point 2 as seen from point 1 in [1] and turns the nunchaku model by them (tww the same) |
| 0x38 | `m38` | `mSpeed` | nun_control 0249196C: per-point velocity, the damped difference of the new and old position of point 2, added to the next step (tww the same; as st_ke_s::mSpeed) |

### himo3_s (d_a_himo3.h)

Round 12. One of himo3_class's 200 rope segments (mSegments): position and velocity, as himo2_s::mPos. tww keeps m00 / m0C.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mPos` | himo3_control (inlined in daHimo3_Execute 02172A00) moves each rope segment from the previous one by the gravity / pull offset and feeds the collision spheres and line from it; [0] = current.pos (tww the same; as himo2_s::mPos) |
| 0x0C | `m0C` | `mSpeed` | himo3_control (in daHimo3_Execute 02172A00) adds it to the next step and stores (new - old) * a factor (0 in WWHD, a REG value in tww) (tww the same) |

### OsMainBrkEntry (d_a_npc_os_create.h)

Round 12. An entry of the Cannon statue NPC's brk table (osMain_brkAnmTbl, tww initBrkAnm's local brkAnmTbl). tww keeps unk_4.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0C | `unk_4` | `mStartAtEnd` | initBrkAnm 022A9F4C: when it is negative the new brk starts at its end frame, else at its current frame; static osMain_brkAnmTbl entries are 0 or -1, never written (tww initBrkAnm brkAnmTbl the same) |

### BrkTableEntry (d_a_npc_os_b.h)

Round 12. An entry of setAnm_brkAnm's anmBrkTbl: the animation and brk indices set together. tww keeps unk_1 / unk_2.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `unk_1` | `mAnmPrmIdx` | setAnm_brkAnm 022ADA3C passes it to setAnm (the l_anmPrm index); static anmBrkTbl, never written (tww setAnm_brkAnm the same) |
| 0x01 | `unk_2` | `mBrkAnmIdx` | setAnm_brkAnm 022ADA3C passes it to initBrkAnm (the brkAnmTbl index); static anmBrkTbl, never written (tww setAnm_brkAnm the same) |

### OsMainAnmPrm (d_a_npc_os_create.h)

Round 12. An entry of the statue NPC's animation parameter table (tww setAnm's l_anmPrm); the same start-at-end switch as OsMainBrkEntry. tww keeps m10.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x10 | `m10` | `mStartAtEnd` | setAnm 022AA88C: when it is negative the new bck starts at its end frame (setFrame(getEndFrame())); static osMain_anmPrm entries are 0 or -1, never written (tww setAnm l_anmPrm the same) |

### KamomeAnmPrm (d_a_npc_kamome.h)

Round 12. An entry of the seagull's animation parameter table l_anmPrm; the same start-at-end switch as OsMainAnmPrm. tww keeps m10.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x10 | `m10` | `mStartAtEnd` | setAnm 022602DC: when it is negative the new bck starts at its end frame (setFrame(getEndFrame())); static l_anmPrm (all three entries 0), never written (tww setAnm the same; as OsMainAnmPrm::mStartAtEnd) |

### AnmTableEntry (d_a_pedestal.cpp)

Round 12. An entry of the pedestal's brk table brkAnmTbl; the same start-at-end switch as OsMainBrkEntry. tww keeps unk_4.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x08 | `unk_4` | `mStartAtEnd` | initBrkAnm 023C85F8: when it is negative the new brk starts at its end frame (setFrame(getEndFrame())); static brkAnmTbl (0, 0, 0, -1), never written (tww daPds_c::initBrkAnm the same; as OsMainBrkEntry::mStartAtEnd) |

### rd_anm_prm_struct (d_a_rd.h)

Round 12. An entry of the ReDead's btk parameter table btk_anm_prm_tbl (setBtkAnm). tww keeps m00 / m01 (setBtkAnm's local anm_prm_struct).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mBtkIdx` | setBtkAnm 0245AC14: index into btk_anm_idx_tbl of the btk to load (-1 none); static btk_anm_prm_tbl, never written (tww setBtkAnm a_anm_prm_tbl the same) |
| 0x01 | `m01` | `mNextPrmIdx` | setBtkAnm 0245AC14: once a one-shot (EMode_NONE) btk has stopped, mBtkAnmPrmIdx moves on to this entry (-1 none; the open and close entries go on to a looping one); static btk_anm_prm_tbl, never written (tww setBtkAnm the same) |

### AnmTbl (d_a_npc_md_exec.h)

Round 12. An entry of Medli's bck name tables l_anmTbl / armAnmTbl / wingAnmTbl (setAnm). tww keeps m00 / m20 (setAnm's local anm_tbl).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mBckName` | setAnm 02286D7C passes it to dNpc_Md_setAnm as the bck file of the body / arm morf (the only one of the wing morf); static l_anmTbl / armAnmTbl / wingAnmTbl, never written (tww setAnm anm_tbl the same) |
| 0x20 | `m20` | `mBlendBckName` | setAnm 02286D7C passes it to dNpc_Md_setAnm as the second bck, which the McaMorf2 blends in by mRunRate when it is not "" (md_walk / md_run); static tables, never written (tww the same) |

### BtpEntry (d_a_npc_md_heap.h)

Round 12. An entry of Medli's eye btp table btpAnmTbl (initTexPatternAnm); a separate table from AnmTbl. tww keeps m00 / m20 (btp_anm_tbl).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mFileName` | initTexPatternAnm 022846EC loads the eye btp of that file name; static btpAnmTbl, never written (tww initTexPatternAnm btp_anm_tbl the same) |
| 0x20 | `m20` | `mbSyncBody` | initTexPatternAnm 022846EC copies it into daNpc_Md_c::mBtpSyncBody (0 / 1; while it is 1 the btp frame follows the body bck instead of blinking); static btpAnmTbl, never written (tww the same: m3136, playTexPatternAnm) |

### dNpc_HIO_c and its copies, the player eye offset

Round 12. dNpc_HIO_c (d_npc.h), the NPC HIO base, and its copies dNpc_HIO_l (d_a_npc_md.h; also used by d_a_npc_os and d_a_npc_cb1), dNpc_HIO_p2 (d_a_npc_p2.h, d_a_npc_p2_act.h) and the field inlined into daNpc_Bms1_HIO_c (d_a_npc_bms1.h). The census had put dNpc_HIO_c `m04` under "constructor only, no tww reader"; tww's d_a_npc_so / btsw / btsw2 / bmsw read `l_HIO.mNpc.m04`, so it is named here. tww keeps m04.

| Struct | WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|---|
| dNpc_HIO_c (d_npc.h) | 0x00 | `m04` | `mPlayerEyeOffsetY` | the player's eye height offset the NPCs pass to dNpc_playerEyePos for their look target (WWHD: so, btsw, btsw2, bmsw, bs1, ds1, rsh1, sarace, kg1, tc, cb1, md, os, p2, bms1; tww: so, btsw, btsw2, bmsw); the dNpc_HIO_c constructor 0259DA18 sets 0, each NPC's HIO constructor then its own value (-20 to -80) (tww the same); same name as the NpcDatStruct copies of other NPCs |
| dNpc_HIO_l (d_a_npc_md.h) | 0x00 | `m04` | `mPlayerEyeOffsetY` | copy of dNpc_HIO_c |
| dNpc_HIO_p2 (d_a_npc_p2.h, d_a_npc_p2_act.h) | 0x00 | `m04` | `mPlayerEyeOffsetY` | copy of dNpc_HIO_c |
| daNpc_Bms1_HIO_c (d_a_npc_bms1.h) | 0x0C | `m04` | `mPlayerEyeOffsetY` | dNpc_HIO_c's field inlined into the bms1 HIO; lookBack passes it to dNpc_playerEyePos_l |

### daObjSwheavy::Attr_c (d_a_obj_swheavy.h)

Round 12. The heavy floor switch's attribute table (one entry per type). tww keeps m14.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x14 | `m14` | `mTopLowerDelay` | mode_u_l_init 02394AE8 passes it to top_bg_aim_req as the timer: calc_top_pos 02394730 moves the top collision aim to height 0 that many frames after the switch starts going down (0: the request never takes effect); static attribute table (0, 0, 4, 4), never written (tww mode_u_l_init the same) |

### dCamera_c::BGChk (d_camera.h)

Round 12. One ground record of dCamera_c::BG (tww's anonymous struct inside BG). tww keeps m00 / m04 / m58.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mbFound` | checkGroundInfo 024F9044 sets it when the ground check found ground (mGroundY != -1e9); the camera reads it before using the poly / room camera ids and the move-BG of the record (tww the same) |
| 0x04 | `m04` | `mGndChk` | checkGroundInfo 024F9044: the dBgS_CamGndChk of the record; its position is set and GroundCross runs on it, its poly / bg index give the camera ids and the move-BG (tww the same) |
| 0x58 | `m58` | `mGroundY` | checkGroundInfo 024F9044 stores GroundCross of mGndChk (-1e9 when none); the camera modes compare heights with it (tww the same) |

### dCamera_c::BG (d_camera.h)

Round 12. dCamera_c::mBG, the two ground records (BGChk) checkGroundInfo fills each frame; also embedded as CameraProcessHD::BG::mChecks in d_camera_proc.h. tww keeps m00 / m5C.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mPlayerGround` | checkGroundInfo 024F9044: the ground under the player's foot position (default check flags); its ground y feeds m354 and the swim camera id (tww the same) |
| 0x5C | `m5C` | `mCameraGround` | checkGroundInfo 024F9044: the camera ground under the player's head (SetCam / ClrObj, raised by the plain check when that is higher); its move-BG, room and poly camera ids and ground y drive the camera modes (tww the same) |

### dCstick_c (d_cam_param.h) and the CameraSetupView view (d_camera_mode.h)

Round 12. The camera setup's C-stick thresholds (dCamSetup_c::mCstick). mUpPressY is named under the 2026-11-11 rule (WWHD constructor only, tww nextMode reads it). m08 (6) stays open: set by the constructors only, read nowhere in WWHD or tww. tww keeps m00 / m04 / m08.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `m00` | `mUpReleaseY` | nextMode 024FA9C8: while the C-stick-up state (mCStickYState) is set it is cleared when the C-stick y falls below it; dCstick_c__ct 024F6DEC sets 0.2 (tww dCamera_c::nextMode the same) |
| 0x08 | `m04` | `mUpPressY` | written only by dCstick_c__ct 024F6DEC (0.95) in WWHD, where nextMode 024FA9C8 enters the C-stick-up state on a pad option flag (0x40) instead; tww dCamera_c::nextMode enters it (dCamAttnStts_00001000_e, m184 = 1) when the C-stick y rises above it |

### dCamera__Style (d_cam_param.h)

Round 12. An entry of the camera style table (dCamParam_c::styles). tww keeps m00.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `m00` | `mStyleName` | SearchStyle 024F732C returns the index of the style whose four-character name it is (callers pass e.g. 'TT01'); the camera style table, never written by code (tww dCamParam_c::SearchStyle the same) |

### cM_rnd_c and dDlst_effectLine_c's embedded copy (d_drawlist.h), the three seeds (misnamed)

Round 12. cM_rnd_c is the Wichmann-Hill generator: get() steps three seeds (x171 mod 30269, x172 mod 30307, x170 mod 30323) and sums their fractions. The second and third were named as if it were a linear congruential generator; all three now carry the seed number. dDlst_effectLine_c flattens its cM_rnd_c member (tww mRnd) at 0x04..0x0C and takes the same names. tww keeps m0 / m4 / m8.

| Struct | WWHD offset | Old | New | Evidence |
|---|---|---|---|---|
| cM_rnd_c | 0x00 | `mSeed` | `mSeed0` | the first seed (x171 mod 30269); init 0252CD10 stores its first argument; renamed only to number the series |
| cM_rnd_c | 0x04 | `mMultiplier` | `mSeed1` | (misnamed) the second seed (x172 mod 30307), not a multiplier; init's second argument |
| cM_rnd_c | 0x08 | `mIncrement` | `mSeed2` | (misnamed) the third seed (x170 mod 30323), not an increment; init's third argument |
| dDlst_effectLine_c | 0x04 / 0x08 / 0x0C | `mSeed` / `mMultiplier` / `mIncrement` | `mSeed0` / `mSeed1` / `mSeed2` | the embedded cM_rnd_c; the same seeds |

### dDlst_effectLine_c (d_drawlist.h), the line parameters (misnamed)

Round 12. update 0252CDF0 stores its four u16 and four f32 arguments here; tww's dDlst_effectLine_c::draw (field_0x20..0x34) shows each one is a min / random-range pair for cM_rnd_c::getValue: the line count, the line width, the start radius from mPos and the line length. The old names (mLineCount, mLineLength, mLineSpeed, mLineMode, mWidth, mHeight, mDepth, mScale) were our own and do not match that use; the fields now carry the names dCamForcusLine's matching update arguments took in round 11. tww keeps field_0x20..field_0x34.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x20 | `mLineCount` | `mLineNumMin` | update's 3rd argument; tww draw: the line count is mRnd.getValue(it, mLineNumRange) |
| 0x22 | `mLineLength` | `mLineNumRange` | update's 4th argument; tww draw: the random range added to mLineNumMin |
| 0x24 | `mLineSpeed` | `mLineWidthMin` | update's 5th argument; tww draw: GXSetLineWidth(mRnd.getValue(it, mLineWidthRange)) per line |
| 0x26 | `mLineMode` | `mLineWidthRange` | update's 6th argument; tww draw: the random range added to mLineWidthMin |
| 0x28 | `mWidth` | `mStartRadiusMin` | update's 1st float; tww draw: each line starts mRnd.getValue(it, mStartRadiusRange) from mPos at a random angle |
| 0x2C | `mHeight` | `mStartRadiusRange` | update's 2nd float; tww draw: the random range added to mStartRadiusMin |
| 0x30 | `mDepth` | `mLineLengthMin` | update's 3rd float; tww draw: each line is mRnd.getValue(it, mLineLengthRange) long |
| 0x34 | `mScale` | `mLineLengthRange` | update's 4th float; tww draw: the random range added to mLineLengthMin |

### dNpc_HIO_c and its copies, the debug draw switch

Round 12. The same struct and copies as the player eye offset above. The census had put `m22` under "constructor only, no tww reader" for all four; WWHD's btsw / bmsw / tc _draw and so's _draw (through d_a_npc_so_mode.h's view `debugDraw`) read it, and so do tww's so / btsw / bmsw. tww keeps m22.

| Struct | WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|---|
| dNpc_HIO_c (d_npc.h) | 0x1E | `m22` | `mDebugDraw` | _draw of btsw 02219840, bmsw 0220F06C, tc 022EA464 and so 022DFEDC draw the debug shapes while it is set (tww so / btsw / bmsw the same); the dNpc_HIO_c constructor 0259DA18 and the NPCs' HIO constructors clear it; same name as daRd_HIO_c's and daTag_So_HIO_c's own debug switches |
| dNpc_HIO_l (d_a_npc_md.h) | 0x1E | `m22` | `mDebugDraw` | copy of dNpc_HIO_c |
| dNpc_HIO_p2 (d_a_npc_p2.h, d_a_npc_p2_act.h) | 0x1E | `m22` | `mDebugDraw` | copy of dNpc_HIO_c |
| daNpc_Bms1_HIO_c (d_a_npc_bms1.h) | 0x2A | `m22` | `mDebugDraw` | dNpc_HIO_c's field inlined into the bms1 HIO; its constructor clears it |

## Two-digit offset names left open (round 3 census)

Round 12. The round-3 census counted 440 two-digit offset names; rounds 10 to 12 named 205 of them. The 235 below stay open, one line per class or group, each with its reason: read nowhere, written only, padding, constructor-only with no tww reader, product of one value, or several roles.

- `hdScreenMsgOnly2_c` (d/hd_screen_msg_only2.h): `m74`, `mA8`. Constructor-only, no tww reader: HD screen: construct clears it, read nowhere.
- `dEvDtStaff_c` (d/d_event_data.h): `m28`, `m34`, `m44`, `m45`. Written only, read nowhere: written only (init / advanceCut clear m44/m45); m28 read nowhere; m34 padding.
- `ShopCam_action_c` (d/d_shop.h): `m50`, `m52`. Constructor-only, no tww reader: constructor-only in WWHD (ShopCam_action_c_ct); the tww uses counted are ShopCursor_c's m50.
- `dSnap_CharaData` (d/d_snap.h): `m11`. Read nowhere: declared after figRoom, read nowhere in WWHD; tww d_snap.cpp declares it only.
- `dCamera_c::PosSet` (d/d_camera.h): `m1E`. Written only, read nowhere: written only (eventCamera clears it); tww only writes it too.
- `CameraSavedViewHD` (d/d_ev_camera_5.h): `m1E`. Written only, read nowhere: set to 1/2 by saveEvCamera / possessedEvCamera only; no reader found (HD-only view).
- `JntHit_HIO_c` (d/d_jnt_hit.h): `m06`, `m08`, `m0C`, `m10`, `m1C`, `m20`, `m24`. Constructor-only, no tww reader: constructor-only (JntHit_HIO_c__ct); tww d_jnt_hit.cpp has no reader either.
- `dCstick_c` (d/d_cam_param.h): `m08`. Constructor-only, no tww reader: set by the constructors only (6), read nowhere in WWHD or tww.
- `dCamBGChk_c` (d/d_cam_param.h): `m2C`, `m48`, `m4C`, `m50`, `m54`, `m58`, `m5C`. Constructor-only, no tww reader: constructor-only (dCamBGChk_c__ct); tww reads none of these offsets either.
- `HIO_c` (d/actor/d_a_st.h): `m14`. Constructor-only, no tww reader: static_initialize only; read nowhere in d_a_st (the tww hits are d_a_stone).
- `st_p` (d/actor/d_a_st.h): `m48`. Read nowhere: dPa_smokeEcallBack constructed in part_construct, used nowhere else.
- `daSie_Flag_HIO_c` (d/actor/d_a_sie_flag.h): `m08`. Constructor-only, no tww reader: constructor-only.
- `daFganon_HIO_c` (d/actor/d_a_fganon.h): `m07`, `m42`. Read nowhere: never used.
- `daBmdfoot_HIO_c` (d/actor/d_a_bmdfoot.h): `m05`. Read nowhere: never used.
- `sita_s` (d/actor/d_a_bwd.h): `m10`. Padding: padding (u8 view; the tww f32 m10 is the HIO).
- `daBwd_HIO_c` (d/actor/d_a_bwd.h): `m08`, `m38`, `m3A`, `m3C`, `m3E`. Constructor-only, no tww reader: constructor-only; tww constructor only.
- `daGFlag_HIO_c` (d/actor/d_a_goal_flag.h): `m04`, `m06`, `m07`, `m08`, `m09`. Constructor-only, no tww reader: constructor/destructor only (round 8).
- `daPirate_Flag_HIO_c` (d/actor/d_a_pirate_flag.h): `m06`. Constructor-only, no tww reader: constructor-only.
- `ga_s` (d/actor/d_a_mo2_main.h): `m10`, `m1C`, `m1E`, `m20`, `m24`, `m28`, `m2C`, `m2E`, `m2F`. Read nowhere: d_a_mo2 effect struct: never used in WWHD or tww (m20 padding).
- `daNpc_Bs1_childHIO_c` (d/actor/d_a_npc_bs1.h): `unk0`. Read nowhere: never used.
- `daBeam_HIO_c` (d/actor/d_a_beam.h): `m08`, `m0C`. Constructor-only, no tww reader: constructor-only; tww constructor only.
- `daNpc_P1_childHIO_c` (d/actor/d_a_npc_p1.h): `unk4`. Read nowhere: never used.
- `mo_ga_s` (d/actor/d_a_kantera.h): `m20`. Padding: padding.
- `SoModeHIO` (d/actor/d_a_npc_so_mode.h): `m2D`. Constructor-only, no tww reader: constructor-only (copy of daNpc_So_HIO_c::m2D).
- `BckConstructView` (d/actor/d_a_npc_jb1.h): `m58`, `m7C`, `m80`, `m84`, `m88`. Constructor-only, no tww reader: mDoExt_bckAnm view, zeroed in the constructor only (round 8).
- `ep_ga_s` (d/actor/d_a_ep.h): `m2C`. Padding: padding.
- `RippleCallback` (d/actor/d_a_warpdm20.h): `m08`. Read nowhere: never used.
- `BckConstructView` (d/actor/d_a_warpdm20.h): `m58`, `m7C`, `m80`, `m84`, `m88`. Constructor-only, no tww reader: mDoExt_bckAnm view, zeroed in the constructor only (round 8).
- `daBemos_HIO_c` (d/actor/d_a_obj_bemos.h): `m0C`, `m10`, `m38`, `m3C`, `m4C`. Constructor-only, no tww reader: constructor-only; tww constructor only.
- `daObj_Warpt_HIO_c` (d/actor/d_a_obj_warpt.h): `m05`, `m10`. Constructor-only, no tww reader: constructor-only; tww constructor only.
- `tn_p` (d/actor/d_a_tn.h): `m42`. Written only, read nowhere: only ever incremented (part_move ++); no other reader in WWHD or tww.
- `daTori_Flag_HIO_c` (d/actor/d_a_tori_flag.h): `m08`, `m0C`. Constructor-only, no tww reader: constructor-only (tww).
- `daMozo_childHIO_c` (d/actor/d_a_mozo.h): `m10`. Constructor-only, no tww reader: constructor-only.
- `daMozo_HIO_c` (d/actor/d_a_mozo.h): `m08`, `m3C`, `m40`, `m44`. Constructor-only, no tww reader: constructor-only; tww constructor only.
- `Act_c::Data_c` (d/actor/d_a_tsubo.h): `m54`, `m58`. Read nowhere: read nowhere (WWHD or tww d_a_tsubo.cpp); only the table value.
- `Act_c::Data_c` (d/actor/d_a_tsubo.h): `mA0`. Padding: padding (u8[4], tww mA0[0xA4-0xA0]).
- `Act_c::Data_c` (d/actor/d_a_tsubo.h): `mAC`. Padding: padding (u8[4], tww mAC[0xB0-0xAC]).
- `Act_c::Data_c` (d/actor/d_a_tsubo.h): `mB4`. Padding: padding (u8[4], tww mB4[0xB8-0xB4]).
- `Act_c::M7A0` (d/actor/d_a_tsubo.h): `m08`. Written only, read nowhere: written only (spec_make_boko copies m06 into it, spec_init clears it); read nowhere in WWHD or tww.
- `Act_c::M7A0` (d/actor/d_a_tsubo.h): `m0C`. Written only, read nowhere: written only (spec_init / spec_mode_put_init clear it); read nowhere in WWHD or tww.
- `bgn3_ke_s` (d/actor/d_a_bgn3.h): `m3C`. Read nowhere: never used in WWHD (the tww hit is daBgn3_HIO_c::m3C).
- `daObjPirateShipHIO_c` (d/actor/d_a_obj_pirateship.h): `m06`. Written only, read nowhere: only copied into the ship's own unnamed m2CA; no role of its own.
- `daObjPirateShipHIO_c` (d/actor/d_a_obj_pirateship.h): `m14`. Constructor-only, no tww reader: constructor only.
- `PirateshipOffset` (d/actor/d_a_obj_pirateship.h): `m1A`. Read nowhere: never used.
- `Attr_c` (d/actor/d_a_stone2.h): `m1C`, `m4C`. Read nowhere: read nowhere (WWHD or tww d_a_stone2.cpp).
- `Part_c` (d/actor/d_a_obj_ajav.h): `mB8`. Padding: padding.
- `daNpc_P2_childHIO_c` (d/actor/d_a_npc_p2.h): `unk0`, `m2C`, `m6C`. Constructor-only, no tww reader: constructor-only (unk0 unused).
- `Attr_c` (d/actor/d_a_obj_swheavy.h): `m00`. Padding: padding.
- `Attr_c` (d/actor/d_a_obj_movebox.h): `m00`, `m06`. Read nowhere: already listed open in objects.md (daObjMovebox::Attr_c): check_to_walk's push / pull counts for mPPLabel bit 4, tww's PPLABEL_UNK4 set from an unnamed proc variable; no role name.
- `HIO` (d/actor/d_a_majuu_flag.h): `m05`, `m06`, `m07`, `m08`. Constructor-only, no tww reader: majuu flag HIO: constructor only.
- `mDoExt_J3DModelPacketS_l` (d/actor/d_a_bgn.h): `m90`, `m9C`, `mA0`, `mA4`, `mAC`, `mAD`, `mAE`, `mAF`. Written only, read nowhere: HD packet view: mA0/mA4 written only, m90/m9C/mAD-mAF unused; mAC read once as a flag of the player packet with no role of its own here.
- `daWarpfout_c` (d/actor/d_a_warpfout.h): `unk0`. Read nowhere: never used.
- `daTag_So_HIO_c` (d/actor/d_a_tag_so.h): `m06`. Padding: padding.
- `Hio` (d/actor/d_a_daiocta_eye.h): `m04`, `m08`, `m0C`, `m30`. Constructor-only, no tww reader: daiocta eye HIO: constructor only (m30 a JntHit_HIO_c byte view).
- `daRd_HIO_c` (d/actor/d_a_rd.h): `m7C`. Read nowhere: JntHit_HIO_c member kept as a byte view (constructJntHitHIO only).
- `KamomeHioPrm` (d/actor/d_a_npc_kamome.h): `m08`, `m0C`, `m14`, `m18`, `m1C`, `m1E`. Read nowhere: never used in WWHD or tww (declared only).
- `KamomeHioPrm` (d/actor/d_a_npc_kamome.h): `m10`. Read nowhere: never used as KamomeHioPrm (the m10 read counted is KamomeAnmPrm::m10).
- `Attr_c` (d/actor/d_a_obj_barrel2.h): `m1C`, `m20`, `m24`, `m28`, `m2C`, `m30`. Several roles (round 9): m20/m24 and m28/m2C only in products of one scale (WWHD and tww); m30 two uses; m1C unread.
- `daSea_WaterHeightInfo_Mng` (d/actor/d_a_sea.h): `m51`. Padding: padding.
- `daSea_WaveInfo` (d/actor/d_a_sea.h): `m04`. Constructor-only, no tww reader: constructor-only (array cleared).
- `daObj_Ikada_HIO_c` (d/actor/d_a_obj_ikada.h): `m09`, `m26`. Padding: padding in tww (u8 arrays), unused in WWHD.
- `daOship_HIO_c` (d/actor/d_a_oship.h): `m04`. Constructor-only, no tww reader: constructor-only.
- `daYkgr_HIO_c` (d/actor/d_a_ykgr.h): `m08`, `m10`. Constructor-only, no tww reader: constructor-only (tww).
- `daNpc_Ym1_HIO_c` (d/actor/d_a_npc_ym1_b.h): `m08`. Constructor-only, no tww reader: constructor-only.
- `Bm1BehaviorChildHio` (d/actor/d_a_npc_bm1_b.h): `m50`. Written only, read nowhere: written only (child index in the constructor).
- `daBmd_HIO_c` (d/actor/d_a_bmd.h): `m05`, `m10`. Constructor-only, no tww reader: constructor-only.
- `daNpc_Ls1_matAnm_c` (d/actor/d_a_npc_ls1.h): `m74`, `m7D`. Padding: padding.
- `daNpc_Ls1_HIO_c` (d/actor/d_a_npc_ls1.h): `m04`, `m08`. Constructor-only, no tww reader: constructor-only.
- `himo2_s` (d/actor/d_a_himo2.h): `m00`, `m0C`, `m20`. Written only, read nowhere: m00/m20 padding; m0C written only (ground height + 5) with no reader found.
- `himo2HIO_c` (d/actor/d_a_himo2.h): `m0A`. Constructor-only, no tww reader: constructor only.
- `daMt_HIO_c` (d/actor/d_a_mt.h): `m05`, `m06`, `m07`, `m08`, `m0C`, `m10`, `m14`, `m18`, `m20`, `m24`, `m28`, `m2C`, `m30`, `m32`, `m34`, `m38`, `m3A`, `m3C`, `m3E`, `m40`, `m44`, `m48`, `m4C`, `m52`, `m54`, `m58`, `m5C`. Constructor-only, no tww reader: constructor-only in WWHD (the tww d_a_mt.cpp HIO is never read).
- `daSail_HIO_c` (d/actor/d_a_sail.h): `m07`. Constructor-only, no tww reader: constructor-only.
- `daNpc_Rsh1_HIO_c` (d/actor/d_a_npc_rsh1.h): `m5C`, `m68`, `m69`, `m6A`, `m6B`. Constructor-only, no tww reader: constructor-only; tww d_a_npc_rsh1.cpp only clears them in the constructor too.
- `daPt_HIO_c` (d/actor/d_a_pt.h): `m02`. Constructor-only, no tww reader: constructor-only.
- `Bm1SetupChildHio` (d/actor/d_a_npc_bm1.h): `m50`. Written only, read nowhere: written only (child index in the constructor).
- `daNpc_So_HIO_c` (d/actor/d_a_npc_so.h): `m2D`, `m40`, `m58`, `m60`, `m6C`, `m70`, `m74`, `m78`. Constructor-only, no tww reader: constructor-only; tww constructor only too.
- `btd_hahen_s` (d/actor/d_a_btd.h): `m05`, `m24`. Padding: padding (u8 views).
- `btd_sibuki_s` (d/actor/d_a_btd.h): `m01`, `m1C`. Padding: padding (u8 views).
- `daBtd_HIO_c` (d/actor/d_a_btd.h): `m11`, `m12`, `m13`, `m2E`. Read nowhere: never used (tww declares only).
- `HioPrm::<anon>::<anon>` (d/actor/d_a_tag_md_cb.h): `m0C`, `m0D`, `m0E`, `m0F`, `m10`, `m11`, `m12`. Read nowhere: never used in WWHD or tww.
- `dPa_rippleEcallBack_l` (d/actor/d_a_npc_ko1.h): `m04`, `m08`, `m0C`, `m10`. Read nowhere: ripple callback view: m08 cleared once, the rest unused.
- `daNpc_Md_HIO3_l` (d/actor/d_a_npc_md.h): `m24`. Constructor-only, no tww reader: tww's l_HIO.m008.m24 reads are daNpc_Md_HIO2_c's field (already mTurnScale); HIO3's m24 is set by its constructor only and read nowhere in WWHD or tww.
- `kiHIO_c` (d/actor/d_a_ki.h): `m10`. Constructor-only, no tww reader: constructor-only (__sinit 1.0f); read nowhere in WWHD or tww.
- `MachineBck` (wwhd_src/d/actor/d_a_machine.cpp): `m7C`, `m80`, `m84`, `m88`. Constructor-only, no tww reader: mDoExt_bckAnm view, zeroed only (round 8).
- `IbData` (wwhd_src/d/actor/d_a_ib.cpp): `m00`, `m01`, `m02`, `m03`. Read nowhere: declared, never used (round 9).
- `Attr_c` (wwhd_src/d/actor/d_a_obj_coming.cpp): `m00`. Several roles: s16[16] array that packs several tables (make counts by index, wait-time base/range pairs at 4 + 2n / 5 + 2n): more than one role; would need a type split first.
- `lk_emitterParam_l` (wwhd_src/d/actor/d_a_player_main_04.cpp): `m08`, `m18`. Written only, read nowhere: Link emitter param: written only (m08 also address-taken once for the emitter, no field role).
- `daKanban_HIO_c` (wwhd_src/d/actor/d_a_kanban.cpp): `m04`, `m08`, `m0A`. Constructor-only, no tww reader: constructor-only (round 9).

### Bm1BehaviorHioPrm (d_a_npc_bm1_b.h; copy Bm1SetupHioParam in d_a_npc_bm1.h)

Round 12, resubmitted from round 11 with the tww debug-drawing evidence. tww's hio_prm (d_a_npc_bm1.cpp) keeps m18. npcs.md's open line no longer lists it.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x18 | `m18` | `mDebugDraw` | _draw 02200F90: the debug leftovers (the debug GXColor statics) run only while it is set; tww d_a_npc_bm1.cpp _draw tests it right before its stripped "// Debug:" block, the same three debug GXColors followed by the removed dDbVw_drawSphereXlu / dDbVw_drawCircleOpa calls, so it switches the debug drawing; tww's hio_prm tables hold 0 and no code writes it |

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

Open: `m2B4` / `m2B5` (0x3D0 / 0x3D1: daKN_Create 021A30C0 stores param bytes 0 and 1, nothing reads them) and `m2BC`
(0x3D8, no use). `m2B6`, `m2C1`, `m2D6`, `m2FE` and `m30C` are unused byte gaps and stay.

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
names come from the WWHD uses (d_a_syan.cpp). field_0x2d8 (no uses) and field_0x2dc (only read: it scales the state 2
targets, nothing in this source writes it) stay open.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3C0 | `field_0x2a4` | `mSwingAngle` | nodeCallBack 024A4298: [0] rotates joint 5 (SYAN_JNT_JOINT5_e: Y, -X, Z), [1].x is added to joint 8's X; daSyan_Execute 024A45E0: [0].x / [1].x = sine of mSwingPhase0 / 1 times mSwingAmp0 / 1 |
| 0x3CC | `field_0x2b0` | `mSwayAngle` | nodeCallBack 024A4298: rotates joint 8 (SYAN_JNT_JOINT8_e: Y, -(X + mSwingAngle[1].x), Z); daSyan_Execute 024A45E0: y / z = sine of frameCount times mSwayAmp, x eased to 2500 in states 1 and 2 |
| 0x3D4 | `field_0x2b8` | `mSwayAmp` | daSyan_Create 024A5224: 200; daSyan_Execute 024A45E0: amplitude of mSwayAngle.y / z, eased to 2000 in state 1 and to field_0x2dc * 500 in state 2 |
| 0x3D8 | `field_0x2bc` | `mSwingAmp0` | daSyan_Execute 024A45E0: amplitude of mSwingAngle[0].x; eased to 1500 in state 1, to field_0x2dc * 300 in state 2 |
| 0x3DC | `field_0x2c0` | `mSwingAmp1` | daSyan_Execute 024A45E0: amplitude of mSwingAngle[1].x; eased to 2500 in state 1, to field_0x2dc * 300 in state 2 |
| 0x3E0 | `field_0x2c4` | `mSwingPhase0` | daSyan_Execute 024A45E0: + mSwingSpeed0 per frame; sine phase of mSwingAngle[0].x |
| 0x3E4 | `field_0x2c8` | `mSwingPhase1` | daSyan_Execute 024A45E0: + mSwingSpeed1 per frame; sine phase of mSwingAngle[1].x |
| 0x3E8 | `field_0x2cc` | `mSwingSpeed0` | daSyan_Execute 024A45E0: added to mSwingPhase0 each frame; eased to 500 in state 2 |
| 0x3EC | `field_0x2d0` | `mSwingSpeed1` | daSyan_Execute 024A45E0: added to mSwingPhase1 each frame; eased to 1000 in state 2 |
| 0x648 | `field_0x52c` | `mPartAlphaTarget` | daSyan_Execute 024A45E0: [i] = 4 + cM_rndF(4) whenever mPartAlphaTimer[i] runs out; partAlpha[i] is eased to it |
| 0x678 | `field_0x55c` | `mPartScale1Target` | daSyan_Execute 024A45E0: [i] = 0.75 + cM_rndF(0.075) whenever mPartScale1Timer[i] runs out; partScale1[i] is eased to it |
| 0x6A8 | `field_0x58c` | `mPartAlphaTimer` | daSyan_Execute 024A45E0: [i] = 5 + cM_rndF(10) at 0 (with a new mPartAlphaTarget[i]), counted down |
| 0x6B4 | `field_0x598` | `mPartScale1Timer` | daSyan_Execute 024A45E0: [i] = 3 + cM_rndF(6) at 0 (with a new mPartScale1Target[i]), counted down |
| 0x6C0 | `field_0x5a4` | `mFireStartTimer` | daSyan_Create 024A5224: [i] = fire_time[i]; daSyan_Execute 024A45E0: counted down; at 0 part i's ID_AK_JN_TORCH emitter starts (emtrEnabled) and partScale2[i] grows |

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
from the WWHD uses (d_a_sail.cpp; sail_move inlines sail_pos_move and demo_move). Left open: sail_class m1C4C
(= 15 * (mFurl - 0.6), only damps the flutter) and daSail_packet_c m1C3B (set to 1 in the constructor, never read).

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
| 0x3E4 | `m2C8` | `mBikuRequest` | move (in daDr_Execute 0212A00C): when not 0 (or idle with l_HIO m0C / a quake) it is cleared and mMode 10 starts the flinch (BCK DR_BIKU1); nothing in d_a_dr sets it |
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

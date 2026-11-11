# Field names: regular enemies

Offset-named fields of the regular enemy actor classes (`*_class`) that got names in the field naming pass
(2026-11-10); rules as in docs/decomp-notes.md, "Field names (offset-named fields)": a name comes
from how the original code uses the field (address of the use and what it shows), from names already in this
decomp, or from the CC0 zeldaret/tww and zeldaret/tp decomps. Renames only (objcmp: object-identical). The
header line keeps the old name (`was mXXXX`) with the evidence. Inlined GameCube functions are cited as
`name (in Caller ADDR)`. Fields without enough evidence keep their offset name and are listed as open.

## bk_class (Bokoblin; d_a_bk.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D1 | `m02B5` | `mSightRangePrm` | daBk_Create 0209F918: param bits 8-15; stand_sight (in Bk_move 020A5EF4) and daBk_Execute 0209BD1C use it * 10 as the player sight distance (0xFF: default 500 / 300) |
| 0x3D2 | `m02B6` | `mPathNo` | daBk_Create 0209F918: param bits 16-23, dPath_GetRoomPath(it, room) when not 0xFF; path_check 0209B354 stores it + 1 in mPathOn |
| 0x3D3 | `m02B7` | `mSwitchNo` | daBk_Create 0209F918: param bits 24-31; mAppearWait = it + 1 (wait for the switch); d_dozou 020A4594 / daBk_Draw 0209971C test dComIfGs_isSwitch(it) |
| 0x3D4 | `m02B8` | `mDeathSwitch` | daBk_Create 0209F918: angle.z (0xFF -> 0), no spawn when the switch is set, copied to mEnemyIce.mDeathSwitch; fail 020A3704 / water_fail 020A3B18 dComIfGs_onSwitch(it) |
| 0x3D5 | `m02B9` | `mDeleteOnSwitch` | daBk_Create 0209F918: param & 0x10; when set the actor is not created if mSwitchNo is on, and mSwitchNo becomes 0xFF |
| 0x3D6 | `m02BA` | `mAppearWait` | daBk_Create 0209F918: 0xFF (types 2/3, wait for the player), mSwitchNo + 1 (wait for that switch); daBk_Execute 0209BD1C clears it when the player comes within range / the switch is on; daBk_Draw 0209971C draws nothing while it is set |
| 0x3E0 | `m02C4` | `mpEyeBtp` | useHeapInit 0209F220: mDoExt_btpAnm with BTP_TMABATAKI (blink); bk_eye_tex_anm (in daBk_Execute 0209BD1C) sets frame 6 / 0 |
| 0x3E4 | `m02C8` | `mBlinkWait` | bk_eye_tex_anm (in daBk_Execute 0209BD1C): counts down, then reloads rnd(100)+20 and starts a blink (mBlinkTimer) |
| 0x3E6 | `m02CA` | `mBlinkTimer` | bk_eye_tex_anm (in daBk_Execute 0209BD1C): rnd(3)+3 per blink; eye frame 6 (closed) while it counts down |
| 0x3E8 | `m02CC` | `mWeaponViewWait` | daBk_Create 0209F918 sets 5, wepon_search 020A3CC8 70, daBk_Execute 0209BD1C counts down; daBk_wepon_view_check 0209AB64 does not look for a weapon while it is not 0 |
| 0x3EA | `m02CE` | `mWeaponJumpCount` | wepon_search 020A3CC8: cleared when the run starts, +1 per jump over a wall (JUMP1) up to 2, then gives up |
| 0x3EC | `m02D0` | `mpShieldModel` | useHeapInit 0209F220: model BMD_BK_TATE (shield) when mHasShield; tate_mtx_set 0209BAD8 places it, daBk_Draw 0209971C draws it |
| 0x3F0 | `m02D4` | `mHasShield` | daBk_Create 0209F918: param & 0x20 (health 7 instead of 5); useHeapInit 0209F220: green body and the TATE (shield) model; fight_run 020A0A98 guards (action 10) only when set |
| 0x3F1 | `m02D5` | `mHasMachete` | daBk_Create 0209F918: param & 0xC0, 0x40 creates boko type 1 (machete), then only 0x40 kept; useHeapInit 0209F220 selects the KB_KEN blur model; attack_set 0209B760 At type 0x800 when set |
| 0x3F4 | `m02D8` | `mpTelescopeModel` | useHeapInit 0209F220: model BDL_BOUEN (telescope) when mHasTelescope; bou_mtx_set 0209BBD4 places it at the BUKI joint, daBk_Draw 0209971C draws it |
| 0x3F8 | `m02DC` | `mHasTelescope` | daBk_Create 0209F918: set for type 0xB (no weapon created); useHeapInit 0209F220: pink body and the BOUEN (telescope) model; stand (in Bk_move 020A5EF4) plays NOZOKU (peek) |
| 0x3F9 | `m02DD` | `mGroundCheck` | fight_run 020A0A98: ground_4_check(...) result bits; 4/8/2 select the side-step direction (mSideStepDir) |
| 0x3FA | `m02DE` | `mDefeated` | fail 020A3704: set to 1 for type 10 after the drop item instead of deleting; daBk_Draw 0209971C draws nothing and Bk_move 020A5EF4 turns Co off while it is set |
| 0x3FB | `m02DF` | `mSpawnTimer` | HD: daBk_Create 0209F918 sets 5 with status bit 0x4000 (0 for type 10); daBk_Execute 0209BD1C counts down and clears bit 0x4000 at 0; HD |
| 0x400 | `m02E8` | `mpBlurModel` | useHeapInit 0209F220: swing blur model KB_KEN / KB_BOKO; br_draw (in daBk_Draw 0209971C) shows its BLURA or BLURB shape |
| 0x404 | `m02EC` | `mBlurScaleX` | fight_attack (in fight 020A1AAC): l_bkHIO m0A8[] / m0D8[] by attack frame; br_draw (in daBk_Draw 0209971C) scales the blur model x by it |
| 0x408 | `m02F0` | `mBlurOn` | daBk_Execute 0209BD1C clears it each frame; fight_attack (in fight 020A1AAC) sets 1 inside the swing frames; br_draw (in daBk_Draw 0209971C) draws the blur only when set |
| 0x40C | `m02F4` | `mBlurUseB` | daBk_Execute 0209BD1C clears it; fight_attack (in fight 020A1AAC) sets 1 for the middle swing frames; br_draw shows BLURB instead of BLURA when set |
| 0x410 | `m02F8` | `mFrameCounter` | daBk_Execute 0209BD1C adds 1 per frame; ground_smoke_set 020A070C / yogan_fail 020A38D4 / fight_run 020A0A98 test its low bits |
| 0x414 | `m02FC` | `mOshiCount` | fight_run 020A0A98 counts it up while the Bokoblin holds its weapon close to the player and sees it; at 25 (+REG) it may start BK_ACTION_OSHI and clears it, else it is cleared; damage_check (in daBk_Execute 0209BD1C) clears it on a hit |
| 0x418 | `m0300` | `mTimers` | daBk_Execute 0209BD1C counts all 5 down each frame; actions load [0]..[4] (e.g. hukki 020A3048, water_fail 020A3B18) |
| 0x422 | `m030A` | `mWeaponRunTimer` | wepon_search 020A3CC8 sets l_bkHIO m08E when it starts running to a dropped weapon and gives up (bare-handed fight run) when it is 0; daBk_Execute 0209BD1C counts it down; jyunkai (in Bk_move 020A5EF4) starts no new weapon search while it runs (as tn_class mWeponRunTimer) |
| 0x424 | `m030C` | `mAnmStopTimer` | wepon_search 020A3CC8 sets 5, daBk_Execute 0209BD1C counts down and plays mpMorf only when it is 0 |
| 0x426 | `m030E` | `mNoHitTimer` | daBk_Execute 0209BD1C counts down and moves the Tg spheres 20000 down while it is not 0; damage_check (in daBk_Execute) takes a hit only when it is 0 and reloads it |
| 0x428 | `m0310` | `mRecentHitTimer` | damage_check (in daBk_Execute 0209BD1C) sets 25 on a hit (weak hits are ignored while it runs in BK_ACTION_TUBO_WAIT), daBk_Execute 0209BD1C 20 when the hiding Bokoblin comes out, then counts it down; fight_run 020A0A98 dodges shots only when it is 0 |
| 0x42C | `m0314` | `mSideStepTimer` | fight_run 020A0A98: 0x10/0x20 when a side step starts; side steps while it is not 0; daBk_Execute 0209BD1C counts down |
| 0x42E | `m0316` | `mSideStepWait` | fight_run 020A0A98: rnd(100)+30 when it reaches 0, then picks the next side step; daBk_Execute 0209BD1C counts down |
| 0x430 | `m0318` | `mSideStepDir` | fight_run 020A0A98: 0/1/2 by mGroundCheck 4/8/2 when a side step starts; selects dr.m710 3/4/... |
| 0x448 | `m0330` | `mHeadAngleY` | nodeCallBack 020991CC: atan2 of the head direction (or angle.y); search_check_draw (in daBk_Draw) and daBk_player_view_check 0209B05C measure the sight cone from it |
| 0x44A | `m0332` | `mTargetAngleY` | Bk_move 020A5EF4: cM_atan2s to dr.m714 (the target); fight_run 020A0A98 / carry_drop 020A47B4 turn to it |
| 0x44C | `m0334` | `mMoveAngleOffset` | fight_run 020A0A98 sets +-0x4000 for side steps; Bk_move 020A5EF4 clears it and moves along angle.y + it |
| 0x44E | `m0336` | `mSpitTimer` | damage reaction cases in daBk_Execute 0209BD1C set 1/3/5; daBk_Execute counts down and at 0 sets particle ID_AK_JN_TUBA00 (spit) at mSpitPos |
| 0x450 | `m0338` | `mSmokePos` | ground_smoke_set 020A070C computes it; smoke_set_s 02098EEC line-checks below it and sets the smoke / grass particles there |
| 0x45C | `m0344` | `mEffectAngle` | ground_smoke_set 020A070C / smoke_set_s 02098EEC: smoke particle angle; daBk_Execute 0209BD1C / yogan_fail 020A38D4: TUBA00 particle angle |
| 0x462 | `m034A` | `mSmokeAngleY` | ground_smoke_set 020A070C: rotates the smoke offset by it and adds 2000 / 0x1FA0 / 0x2000 per call |
| 0x464 | `m034C` | `mSmokeTimer` | ground_smoke_set 020A070C: counts down, sets smoke while >= l_bkHIO m00C; hukki 020A3048 / wepon_search 020A3CC8 / daBk_Execute load l_bkHIO m00C + n |
| 0x466 | `m034E` | `mSmokeType` | ground_smoke_set 020A070C: switch 0..4 selects where the smoke comes from; hukki 020A3048 sets 3, wepon_search 020A3CC8 4, daBk_Execute 0209BD1C 1 |
| 0x467 | `m034F` | `mSmokeCount` | ground_smoke_set 020A070C clears it; smoke_set_s 02098EEC adds 1 per smoke set and tests it with the ground attribute |
| 0x468 | `m0350` | `mSmokeCallBack` | bk_class_ct 0209F74C: dPa_smokeEcallBack_ct; smoke_set_s 02098EEC passes it to the smoke emitter; daBk_Delete 0209F164 removes it |
| 0xC8C | `m0B2C` | `mpBridge` | b_hang 020A5674 reads it as the bridge segment (br_s) the actor hangs on |
| 0xC90 | `m0B30` | `mHoldWeapon` | daBk_Execute 0209BD1C sets 1 when it takes the created weapon (mWeaponId) and moves that weapon with the BUKI joint while set; yari_off_check (in daBk_Execute) clears it on dropping |
| 0xC94 | `m0B34` | `mDropWeapon` | daBk_Execute 0209BD1C damage reactions set 1, damage_check 2; yari_off_check (in daBk_Execute) throws the weapon (1) or drops it (2), then clears it |
| 0xCBC | `m0B5C` | `mAttackKind` | attack_set 0209B760 sets 0/1/2; indexes bk_at_kind / bk_attack_AP / attack SE tables (yari_hit_check, fight_attack in fight 020A1AAC) |
| 0xCC0 | `m0B60` | `mAttackStep` | attack_set 0209B760 clears it; fight_attack (in fight 020A1AAC) steps +1/-1 through bkFight_attackInfo(mAttackKind)[it] animations |
| 0xCC4 | `m0B64` | `mAttackFrame` | attack_set 0209B760 clears it; fight_attack (in fight 020A1AAC) adds the step speed; compared with mAtStartFrame/mAtEndFrame and the l_bkHIO blur frames |
| 0xCC8 | `m0B68` | `mAtStartFrame` | attack_set 0209B760 sets 23/3/0 (+REG); yari_hit_check (in fight_run 020A0A98): no At before it, attack SE at it |
| 0xCCC | `m0B6C` | `mAtEndFrame` | attack_set 0209B760 sets 26/9/20 (+REG); yari_hit_check (in fight_run 020A0A98): no At after it |
| 0xCD0 | `m0B70` | `mLookStartFrame` | attack_set 0209B760 sets 45, wepon_search 020A3CC8 63; fight_attack (in fight 020A1AAC) sets dr.mLookMode = 1 (head toward mpLookTarget) once mAttackFrame is past it and the target is in view (as mo2_class mLookStartFrame) |
| 0xCD4 | `m0B74` | `mTurnEndFrame` | attack_set 0209B760 / wepon_search 020A3CC8 set it; fight_attack (in fight 020A1AAC) keeps turning to mTargetAngleY while mAttackFrame is below it |
| 0xCD8 | `m0B78` | `mAtHitWait` | yari_hit_check (in fight_run 020A0A98) sets 5 on an At hit and has no At while it is not 0; daBk_Execute 0209BD1C counts down |
| 0xCDA | `m0B7A` | `mAttackDir` | attack_set / wepon_search 020A3CC8 set 1; fight_attack (in fight 020A1AAC) sets -1 when the player guards (plays back); yari_hit_check has no At while < 0 |
| 0xCDB | `m0B7B` | `mWeaponAtHome` | stand2 (in Bk_move 020A5EF4) sets 1; daBk_Execute 0209BD1C clears it and, while set, places the held weapon at home.pos instead of the BUKI joint |
| 0xCE8 | `m0B88` | `mCoCyl` | daBk_Create 0209F918: Set(co_cyl_src); daBk_Execute 0209BD1C places it 35 ahead and sets it in the Co check |
| 0xE18 | `m0CB8` | `mTgCyl` | daBk_Create 0209F918: Set(tg_cyl_src); damage_check (in daBk_Execute 0209BD1C) reads its Tg hit |
| 0xF48 | `m0DE8` | `mHeadSph` | daBk_Create 0209F918: Set(head_sph_src); daBk_Execute 0209BD1C centres it on mSpitPos (head); damage_check reads its Tg hit |
| 0x1074 | `m0F14` | `mDefenceSph` | daBk_Create 0209F918: Set(defence_sph_src); damage_check (in daBk_Execute 0209BD1C) centres it on mDefenceSphPos and reads its Tg hit |
| 0x11A0 | `m1040` | `mWeaponSph` | daBk_Create 0209F918: Set(wepon_sph_src); yari_hit_check (in fight_run 020A0A98) moves it along the swing and reads its At hit |
| 0x12CC | `m116C` | `mSpitPos` | nodeCallBack 020991CC: head joint point (20.75, 18.5); daBk_Execute 0209BD1C / yogan_fail 020A38D4 set the TUBA00 (spit) particle there |
| 0x12D8 | `m1178` | `mWeaponTipPos` | daBk_Execute 0209BD1C: point on the held weapon's matrix; yari_hit_check (in fight_run 020A0A98) copies it to mAtPos |
| 0x12F0 | `m1190` | `mNeckPos` | nodeCallBack_P 02099584: point on joint BK_JNT_KUBI (neck) |
| 0x12FC | `m119C` | `mChestPos` | nodeCallBack_P 02099584: point on joint BK_JNT_MUNE (chest) |
| 0x1308 | `m11A8` | `mAtPos` | yari_hit_check (in fight_run 020A0A98): mWeaponTipPos (or dr.m100[14] for kind 2), the At sphere moves to it; ground_smoke_set 020A070C |
| 0x1320 | `m11C0` | `mAtPosOld` | yari_hit_check (in fight_run 020A0A98): mAtPos of the previous frame |
| 0x132C | `m11CC` | `mShieldPos` | tate_mtx_set 0209BAD8: point on the shield (TATE joint); damage_check (in daBk_Execute 0209BD1C) sets the KIKUZU particle there; defence 020A2E04 copies it to mDefenceSphPos |
| 0x1338 | `m11D8` | `mDefenceSphPos` | defence 020A2E04 copies mShieldPos; damage_check (in daBk_Execute 0209BD1C) centres mDefenceSph on it and moves it to y -10000 when not guarding |
| 0x1351 | `m11F1` | `mAtStarted` | wepon_search 020A3CC8 / attack_set 0209B760 clear it; yari_hit_check (in fight_run 020A0A98) starts the At sphere when it is 0, then sets it |
| 0x1352 | `m11F2` | `mTurnAlways` | wepon_search 020A3CC8 sets 1, attack_set 0209B760 clears it; fight_attack (in fight 020A1AAC) keeps turning to the target while it is set |
| 0x1353 | `m11F3` | `mBareHanded` | daBk_Create 0209F918 sets 1 when no weapon is created; wepon_search 020A3CC8 clears it; walk_set/jyunkai (in Bk_move 020A5EF4) branch on it |
| 0x1354 | `m11F4` | `mHeadAngleZ` | nodeCallBack 020991CC: ZrotM by it; aite_miru 020A3624 / fight_run 020A0A98 ease it to 0x2EE0/12000, Bk_move 020A5EF4 back to 0 |
| 0x1358 | `m11F8` | `mpBomb` | daBk_bomb_view_check 0209AE40 / b_nige 020A2A54 store search_bomb() |
| 0x135C | `m11FC` | `mHitActorId` | fight_attack (in fight 020A1AAC) stores the hit actor's ID; aite_miru 020A3624 looks it up and clears it to fpcM_ERROR_PROCESS_ID_e |
| 0x1360 | `m1200` | `mWeaponId` | daBk_Create 0209F918: fopAcM_create(fpcNm_BOKO_e, ...); daBk_wepon_view_check 0209AB64 stores the found weapon; daBk_Execute 0209BD1C moves it with the hand |
| 0x1364 | `m1204` | `mHideActorId` | daBk_Execute 0209BD1C: ID of the actor the waiting (mAppearWait 0xFF) Bokoblin hit, followed until the player comes near |
| 0x1368 | `m1208` | `mCutBits` | damage_check (in daBk_Execute 0209BD1C) ORs atInfo->mPlCutBit; fight_run 020A0A98 guards against those cuts (learn_check) |
| 0x136C | `m120C` | `mJumpMove` | fight_run 020A0A98 and path_run (in Bk_move 020A5EF4) set 1 when the Bokoblin moves by jumps (BK_JUMP1 / JUMP2, steps in mJumpStep) instead of running, and clear it with the BK_RUN animation; path_run turns faster (0x2000) while it is set; jyunkai clears it |
| 0x1370 | `m1210` | `mJumpStep` | fight_run 020A0A98: switch 0/1/2 of the JUMP2 jump while holding a weapon |
| 0x1374 | `m1214` | `mWeaponPending` | daBk_Create 0209F918 sets 1 after creating the weapon; daBk_Execute 0209BD1C takes it into the hand once it exists and clears it |
| 0x1375 | `m1215` | `mPathOn` | path_check 0209B354: 0 or mPathNo + 1; jyunkai (in Bk_move 020A5EF4) follows the path while it is not 0 |
| 0x1376 | `m1216` | `mPathPoint` | path_check 0209B354 / jyunkai (in Bk_move 020A5EF4): index of the current path point, stepped by mPathDir and kept in 0..num-1 |
| 0x1377 | `m1217` | `mPathDir` | daBk_Create 0209F918 sets 1; path_run / jyunkai (in Bk_move 020A5EF4) add it to mPathPoint and set -1/1 at the path ends |
| 0x137C | `m121C` | `mPathEnd` | path_run (in Bk_move 020A5EF4) sets 1 at either path end; daBk_Draw 0209971C draws nothing and daBk_Execute 0209BD1C deletes the weapon while set |
| 0x137D | `m121D` | `mHioOwner` | daBk_Create 0209F918 sets 1 when it created the l_bkHIO child; daBk_Delete 0209F164 removes the HIO child only then |
| 0x137E | `m121E` | `mStandWaitCount` | stand (in Bk_move 020A5EF4) adds 1 per wait cycle; after the NOBI stretch it goes to mode 2 below 3 cycles, else to mode 3 and clears it |
| 0x137F | `m121F` | `mWeaponLost` | yari_off_check (in daBk_Execute 0209BD1C) sets 1 when the weapon is dropped; walk_set (in Bk_move 020A5EF4) reacts once and clears it |
| 0x1384 | `m1224` | `mRotAngle` | stand2 (in Bk_move 020A5EF4) plays BK_MIGIROT / BK_HIDARIROT (turn right / left) when its change against mPrevRotAngle changes sign; nothing sets it (same in tww d217e2b) |
| 0x1388 | `m1228` | `mPrevRotAngle` | stand2 (in Bk_move 020A5EF4) stores mRotAngle here each frame |
| 0x138A | `m122A` | `mRotStep` | stand2 (in Bk_move 020A5EF4) stores mRotAngle - mPrevRotAngle each frame and picks BK_MIGIROT / BK_HIDARIROT when the new step has the other sign |
| 0x138C | `m122C` | `mParalyzeDelay` | damage_check (in daBk_Execute 0209BD1C) sets 8 for attack type 10; Bk_move 020A5EF4 counts down and starts BK_ACTION_D_MAHI (paralysis) at 0 |
| 0x138D | `m1234` | `mDemoCamMode` | demo_camera (in daBk_Execute 0209BD1C): switch of the demo camera steps (0, 1..5, 11, 50, 51); damage_check sets 50, stand (in Bk_move) 1; HD: packed after mParalyzeDelay (fail, demo_camera) |
| 0x138E | `m1236` | `mDemoCamTimer` | demo_camera (in daBk_Execute 0209BD1C): cleared per step, compared with 5..150 frame marks |
| 0x1390 | `m1238` | `mDemoCamEye` | demo_camera (in daBk_Execute 0209BD1C): camera eye (copies the camera's eye), passed to dCam_Set as the eye; demo_camera stores |
| 0x139C | `m1244` | `mDemoCamCenter` | demo_camera (in daBk_Execute 0209BD1C): camera centre (player / ken / actor position + height), passed to dCam_Set as the centre |
| 0x13B8 | `m1260` | `mDemoCamFovy` | demo_camera (in daBk_Execute 0209BD1C): 45/35/55 eased; passed to dCam_Set as the fovy |

Open (no name yet): `m0320` (patrol target in jyunkai / stand2, random offset to the path point in path_run: two meanings), `m11E4` (particle ID_AK_JN_NG position in fight_attack; never written) and `m11F0` (yari_hit_check clears and shifts it, so it is always 0; fight_attack branches on it), the same pair as mo2_class m2934 / m2940, `m1212` (child spawn count in waki_set, random counter in fight_run, look-around delay in stand2: three meanings), the unused and padding members (`m02BB`, `m02C0`, `m02CF`, `m02D6`, `m02E0`, `m0312`, `m0319`, `m032C`, `m0B38`, `m0B44`, `m0B7C`, `m1184`, `m11B4`, `m11F6`, `m120A`, `m1211`, `m1226`, `m1250`), and the shared helper structs in d_a_bk.h (`damagereaction_l`, `enemyice_l`, `enemyfire_l`: local copies of the c_damagereaction.h types, named with those; bkHIO_c below).

View (views are renamed with the class): d_a_andsw0.h declares a private `bk_class` view with the one field hajimarinomori_check 02051174 sets; its `m121C` (0x137C) becomes `mPathEnd`, the name bk_class has for that offset.

### bkHIO_c (l_bkHIO, the Bokoblin tuning block; d_a_bk.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`, unnamed there). HD offsets are the GameCube ones - 4 (the vtable pointer
follows the members). __sinit_d_a_bk_cpp 020A0364 (bkHIO_c::bkHIO_c) sets every field; nothing else writes them. Named from the reads.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x001 | `m005` | `mDrawSearchRange` | search_check_draw (in daBk_Draw 0209971C) draws the sight circles and cone only while it is set; __sinit_d_a_bk_cpp 020A0364 sets the default 0 |
| 0x002 | `m006` | `mDebugFrameStep` | daBk_Execute 0209BD1C runs the per-frame update (mFrameCounter, mTimers, actions) only while it is 0 or on a B trigger / Y hold, so a set value steps frame by frame; __sinit_d_a_bk_cpp 020A0364 sets the default 0 |
| 0x003 | `m007` | `mResetHealthOnHit` | damage_check (in daBk_Execute 0209BD1C) sets health back to 10 on every hit while it is set; __sinit_d_a_bk_cpp 020A0364 sets the default 0 |
| 0x004 | `m008` | `mAlwaysGuard` | fight_run 020A0A98: a shield Bokoblin (mHasShield) guards a cut every time while it is set, else with a 50 % chance; __sinit_d_a_bk_cpp 020A0364 sets the default 1 |
| 0x005 | `m009` | `mHeadAngleIsBody` | nodeCallBack 020991CC: while set mHeadAngleY takes angle.y instead of the head joint direction; __sinit_d_a_bk_cpp 020A0364 sets the default 0 |
| 0x006 | `m00A` | `mNoPickupAttack` | wepon_search 020A3CC8: once the weapon is picked up, attacks (action 5) a player inside mAttackDist only while it is 0; __sinit_d_a_bk_cpp 020A0364 sets the default 0 |
| 0x008 | `m00C` | `mSmokeTimerBase` | ground_smoke_set 020A070C sets smoke while mSmokeTimer >= it; hukki 020A3048, wepon_search 020A3CC8, carry_drop 020A47B4, daBk_Execute 0209BD1C and fight_run 020A0A98 load mSmokeTimer with it + 2..16; __sinit_d_a_bk_cpp 020A0364 sets the default 4 |
| 0x00C | `m010` | `mWeakHitPushback` | damage_check (in daBk_Execute 0209BD1C): dr.mPushback = -it for a hit with knockback power below 25; __sinit_d_a_bk_cpp 020A0364 sets the default 0.0 |
| 0x010 | `m014` | `mAttackBtMaxDis` | fight_attack (in fight 020A1AAC) copies it to the fopEn_enemy_c mBtMaxDis of the swing; __sinit_d_a_bk_cpp 020A0364 sets the default 400 |
| 0x014 | `m018` | `mBlurScale` | br_draw (in daBk_Draw 0209971C): MtxScale(it * mBlurScaleX, it, it) of the swing blur model; __sinit_d_a_bk_cpp 020A0364 sets the default 0.7 |
| 0x01C | `m020` | `mDropItemOffsetY` | fail 020A3704: the drop item (fopAcM_createDisappear) appears at current.pos.y + 100 + it; __sinit_d_a_bk_cpp 020A0364 sets the default 25 |
| 0x020 | `m024` | `mAttnOffsetY` | nodeCallBack 020991CC: attention_info.position = eyePos + it in y; __sinit_d_a_bk_cpp 020A0364 sets the default 25 |
| 0x024 | `m028` | `mNoticeDist` | jyunkai / path_run (in Bk_move 020A5EF4) notice the player closer than it (with the sight check); search_check_draw (in daBk_Draw 0209971C) draws a circle of this radius; __sinit_d_a_bk_cpp 020A0364 sets the default 1000 |
| 0x028 | `m02C` | `mAttackDist` | fight_attack (in fight 020A1AAC), hukki 020A3048 and wepon_search 020A3CC8 attack a player closer than it; fight_run 020A0A98 gives up the fight beyond it + 75; search_check_draw draws a circle of this radius; __sinit_d_a_bk_cpp 020A0364 sets the default 400 |
| 0x02C | `m030` | `mFightDist` | fight_run 020A0A98: walks in (mode 3) farther than it, backs off (mode 4) closer, attack chances inside it +- 62.5, the push (oshi) check below it - 62.5; __sinit_d_a_bk_cpp 020A0364 sets the default 240 |
| 0x030 | `m034` | `mViewAngle` | daBk_player_view_check 0209B05C (called by fight_run 020A0A98, p_lost 020A27F8, jyunkai in Bk_move 020A5EF4, fight_attack) and search_bomb 0209A728: half angle of the sight cone around mHeadAngleY; search_check_draw draws the cone with it; __sinit_d_a_bk_cpp 020A0364 sets the default 0x6590 |
| 0x034 | `m038` | `mViewHeight` | daBk_player_view_check 0209B05C, search_bomb 0209A728 and search_wepon_inline (in daBk_wepon_view_check 0209AB64): no sight when the height difference to eyePos is above it; search_check_draw draws the cone at +- it; __sinit_d_a_bk_cpp 020A0364 sets the default 300 |
| 0x038 | `m03C` | `mViewBoxHalfX` | daBk_player_view_check 0209B05C, search_bomb 0209A728, search_wepon_inline (in daBk_wepon_view_check 0209AB64): the near sight box is |x| < it in the actor's frame; __sinit_d_a_bk_cpp 020A0364 sets the default 500 |
| 0x03C | `m040` | `mViewBoxHalfY` | daBk_player_view_check 0209B05C, search_bomb 0209A728, search_wepon_inline (in daBk_wepon_view_check 0209AB64): the near sight box is |y| < it; __sinit_d_a_bk_cpp 020A0364 sets the default 300 |
| 0x040 | `m044` | `mViewBoxFront` | daBk_player_view_check 0209B05C, search_bomb 0209A728, search_wepon_inline (in daBk_wepon_view_check 0209AB64): the near sight box ends at z < it (in front); __sinit_d_a_bk_cpp 020A0364 sets the default 500 |
| 0x044 | `m048` | `mViewBoxBack` | daBk_player_view_check 0209B05C, search_bomb 0209A728, search_wepon_inline (in daBk_wepon_view_check 0209AB64): the near sight box starts at z > it (behind); __sinit_d_a_bk_cpp 020A0364 sets the default -125 |
| 0x048 | `m04C` | `mPatrolSpeed` | jyunkai (in Bk_move 020A5EF4): patrol speedF target with a weapon (or bare-handed), arrival radius half of it; __sinit_d_a_bk_cpp 020A0364 sets the default 12 |
| 0x04C | `m050` | `mPatrolSpeedNoWeapon` | jyunkai (in Bk_move 020A5EF4): patrol speedF target without a weapon, arrival radius half of it; __sinit_d_a_bk_cpp 020A0364 sets the default 60 |
| 0x050 | `m054` | `mRunSpeed` | stand / path_run (in Bk_move 020A5EF4) speedF (x 0.5 / x 1.2 in some modes, home arrival radius it x 1.25); fight_run 020A0A98 run speed with a weapon; __sinit_d_a_bk_cpp 020A0364 sets the default 45 |
| 0x054 | `m058` | `mRunSpeedNoWeapon` | fight_run 020A0A98: run speed (mode 1) while it holds no weapon and is not bare-handed; __sinit_d_a_bk_cpp 020A0364 sets the default 70 |
| 0x058 | `m05C` | `mDashSpeed` | wepon_search 020A3CC8 (run to the weapon, x 0.5 while turning) and b_nige 020A2A54 (flee from a bomb): speedF; __sinit_d_a_bk_cpp 020A0364 sets the default 90 |
| 0x05C | `m060` | `mFightWalkSpeed` | fight_run 020A0A98: speedF target walking in (mode 3), minus it backing off (mode 4); __sinit_d_a_bk_cpp 020A0364 sets the default 20 |
| 0x060 | `m064` | `mSideStepSpeed` | fight_run 020A0A98: speedF target of the side step (mMoveAngleOffset +-0x4000); __sinit_d_a_bk_cpp 020A0364 sets the default 20 |
| 0x064 | `m068` | `mJumpRunSpeed` | fight_run 020A0A98: speedF during the JUMP2 jumps while holding a weapon (mJumpStep); __sinit_d_a_bk_cpp 020A0364 sets the default 90 |
| 0x06C | `m070` | `mRunAnmSpeed` | fight_run 020A0A98 (fight_run_set) and path_run (in Bk_move 020A5EF4): speed of the BK_RUN animation; __sinit_d_a_bk_cpp 020A0364 sets the default 1.0 |
| 0x070 | `m074` | `mDashAnmSpeed` | wepon_search 020A3CC8: speed of the BK_RUN animation while running to the weapon; __sinit_d_a_bk_cpp 020A0364 sets the default 1.5 |
| 0x074 | `m078` | `mAttackChanceWait` | fight_run 020A0A98: reloads mTimers[4] with it; the attack chance (mAttackChance) is rolled only when that timer is 0; __sinit_d_a_bk_cpp 020A0364 sets the default 0x23 |
| 0x078 | `m07C` | `mAttackChance` | fight_run 020A0A98: attacks (action 5) when cM_rndF(100) < it inside mFightDist +- 62.5; __sinit_d_a_bk_cpp 020A0364 sets the default 50 |
| 0x088 | `m08C` | `mDownTime` | daBk_Execute 0209BD1C: damage reaction modes 20 / 21 load dr.mDownTimers[2] with it; __sinit_d_a_bk_cpp 020A0364 sets the default 0x1E |
| 0x08A | `m08E` | `mWeaponRunTime` | wepon_search 020A3CC8 loads mWeaponRunTimer with it when the run to the weapon starts; __sinit_d_a_bk_cpp 020A0364 sets the default 0x12C |
| 0x098 | `m09C` | `mAttackTurnEndFrame` | attack_set 0209B760 (kind 0) and wepon_search 020A3CC8 copy it to mTurnEndFrame; __sinit_d_a_bk_cpp 020A0364 sets the default 10 |
| 0x09C | `m0A0` | `mBlur0Start` | fight_attack (in fight 020A1AAC), attack kind 0: the swing blur is on from this mAttackFrame; mBlur0ScaleX is indexed from it; __sinit_d_a_bk_cpp 020A0364 sets the default 0x17 |
| 0x09E | `m0A2` | `mBlur0ALen` | fight_attack (in fight 020A1AAC), attack kind 0: frames of the BLURA shape after mBlur0Start before mBlurUseB; __sinit_d_a_bk_cpp 020A0364 sets the default 1 |
| 0x0A0 | `m0A4` | `mBlur0BLen` | fight_attack (in fight 020A1AAC), attack kind 0: frames with mBlurUseB (BLURB shape); __sinit_d_a_bk_cpp 020A0364 sets the default 1 |
| 0x0A2 | `m0A6` | `mBlur0TailLen` | fight_attack (in fight 020A1AAC), attack kind 0: the last part of the blur window (start + A + B + it); __sinit_d_a_bk_cpp 020A0364 sets the default 0 |
| 0x0A4 | `m0A8` | `mBlur0ScaleX` | fight_attack (in fight 020A1AAC), attack kind 0: mBlurScaleX = it[mAttackFrame - mBlur0Start] (10 entries); __sinit_d_a_bk_cpp 020A0364 sets the default table bkHioDefaultsA8 |
| 0x0CC | `m0D0` | `mBlur1Start` | fight_attack (in fight 020A1AAC), attack kind 1: the swing blur is on from this mAttackFrame; mBlur1ScaleX is indexed from it; __sinit_d_a_bk_cpp 020A0364 sets the default 5 |
| 0x0CE | `m0D2` | `mBlur1ALen` | fight_attack (in fight 020A1AAC), attack kind 1: frames of the BLURA shape after mBlur1Start before mBlurUseB; __sinit_d_a_bk_cpp 020A0364 sets the default 1 |
| 0x0D0 | `m0D4` | `mBlur1BLen` | fight_attack (in fight 020A1AAC), attack kind 1: frames with mBlurUseB (BLURB shape); __sinit_d_a_bk_cpp 020A0364 sets the default 1 |
| 0x0D2 | `m0D6` | `mBlur1TailLen` | fight_attack (in fight 020A1AAC), attack kind 1: the last part of the blur window (start + A + B + it); __sinit_d_a_bk_cpp 020A0364 sets the default 1 |
| 0x0D4 | `m0D8` | `mBlur1ScaleX` | fight_attack (in fight 020A1AAC), attack kind 1: mBlurScaleX = it[mAttackFrame - mBlur1Start] (10 entries); __sinit_d_a_bk_cpp 020A0364 sets the default table bkHioDefaultsD8 |
| 0x0FC | `m100` | `mBlurOffsetX` | br_draw (in daBk_Draw 0209971C): the blur model sits 150 + it along the BUKI joint's x; __sinit_d_a_bk_cpp 020A0364 sets the default -60 |
| 0x100 | `m104` | `mWaitTimeMin` | stand (in Bk_move 020A5EF4): mTimers[1] = random in [it, mWaitTimeMax) for the wait; __sinit_d_a_bk_cpp 020A0364 sets the default 0xC8 |
| 0x102 | `m106` | `mWaitTimeMax` | stand (in Bk_move 020A5EF4): upper end of the random wait time (mWaitTimeMin); __sinit_d_a_bk_cpp 020A0364 sets the default 0x1F4 |
| 0x104 | `m108` | `mSleepTimeMin` | stand (in Bk_move 020A5EF4): mTimers[1] = random in [it, mSleepTimeMax) once the BK_SLEEP animation starts; __sinit_d_a_bk_cpp 020A0364 sets the default 0x12C |
| 0x106 | `m10A` | `mSleepTimeMax` | stand (in Bk_move 020A5EF4): upper end of the random sleep time (mSleepTimeMin); __sinit_d_a_bk_cpp 020A0364 sets the default 0x258 |

Open: `m01C`, `m06C`, `m080`, `m084`, `m088`, `m090`, `m094`, `m098` (only the constructor sets them, nothing reads them; tww the same), the padding `m00B`, `m00E`, `m036`, `m07A`.

## cc_class (ChuChu; d_a_cc.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_cc.h) keeps these GameCube offset names; every name below is from the use.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D0 | `m2B4` | `mpMorf` | useHeapInit 02110528: McaMorf of BMD_CC (the body, nodeCallBack on its joints); anm_init 0210C5C8 sets its animation; daCC_Draw 0210BFEC draws it unless split / flattened |
| 0x3D4 | `m2B8` | `mpColorBrk` | useHeapInit 02110528: BRK_CC on the body model; daCC_Draw 0210BFEC sets its frame to mColorType |
| 0x3D8 | `m2BC` | `mpFlatMorf` | useHeapInit 02110528: McaMorf of BDL_CC_BETA; daCC_Draw 0210BFEC draws it instead of the body in mode CC_MODE_FLATTENED; action_dead_move (in daCC_Execute 0210DD3C) plays BCK_CC_BETA on it |
| 0x3DC | `m2C0` | `mpFlatColorBrk` | useHeapInit 02110528: BRK_CC_BETA on the flat model; daCC_Draw 0210BFEC sets its frame to mColorType |
| 0x3E0 | `m2C4` | `mpStoneMorf` | useHeapInit 02110528: McaMorf of BDL_CC_IWA (rock); draw_SUB 0210BB58 / daCC_Draw 0210BFEC place and draw it while mStoneState != 0 |
| 0x3E4 | `m2C8` | `mpStoneBrk` | useHeapInit 02110528: BRK_CC_IWA01 on the rock model; daCC_Draw 0210BFEC uses it in mStoneState 1 at frame mStoneAnmFrame; action_up_check 02112414 rewinds it when the ChuChu turns to stone |
| 0x3E8 | `m2CC` | `mpStoneRecoverBrk` | useHeapInit 02110528: BRK_CC_IWA02 on the rock model; daCC_Draw 0210BFEC uses it in mStoneState 2; action_up_check 02112414 rewinds it when the recovery starts |
| 0x3EC | `m2D0` | `mpStoneBtk` | useHeapInit 02110528: BTK_CC_IWA01 on the rock model; daCC_Draw 0210BFEC uses it in mStoneState 1; action_up_check 02112414 rewinds it |
| 0x3F0 | `m2D4` | `mpStoneRecoverBtk` | useHeapInit 02110528: BTK_CC_IWA02 on the rock model; daCC_Draw 0210BFEC uses it in mStoneState 2; action_up_check 02112414 rewinds it |
| 0x3F4 | `m2D8` | `mpSplitMorf` | useHeapInit 02110528: McaMorf of BDL_CC_PTCL with BCK_CC_PTCL; drawn and played instead of the body while mIsSplit (action_damage_move mode 0x2D, in daCC_Execute 0210DD3C) |
| 0x3FD | `m2F5` | `mMode` | step within mCurrAction: every action function switches on it (action_nomal_move .. action_tubo_search, daCC_Execute 0210DD3C); values are the CC_MODE_* constants (daCC_Create 02110D4C, action_tomaru 02112DA0) |
| 0x3FE | `m2F6` | `mDamageType` | body_atari_check 0210CE9C: reaction per attack type (0 none, 1 critical, 3 wind, 4 boomerang, 6 hammer, 7 hookshot, 8 fire/ice/light, 9 grappling hook); damage_mode_move 0210C6F4 2; action_damage_move (in daCC_Execute 0210DD3C) switches on it |
| 0x3FF | `m2F7` | `mElectricState` | denki_start 0210CBC0 sets 1 (blue / yellow only, At spark on), denki_end 0210CC00 clears it; daCC_Execute 0210DD3C: 1 starts the CCTHUNDER00/01 emitters and goes to 2, 2 keeps them on the BODY03 joint |
| 0x400 | `m2F8` | `mOnWaterSurface` | action_oyogu (in daCC_Execute 0210DD3C) sets 1 once the body is at the water height - 20; BG_check 0210DC8C then holds pos.y there; cleared on leaving the water (naraku_check 0210C828) or on a hit while swimming |
| 0x401 | `m2F9` | `mNoBgCheck` | BG_check 0210DC8C does nothing while it is set; daCC_Create 02110D4C sets 1, the actions clear it when the ChuChu starts to move (action_nomal_move, action_tubo_search 02112E74) |
| 0x402 | `m2FA` | `mAbyssFrames` | naraku_check 0210C828: +1 per frame on ground code 4; deletes the actor after 50 frames (or below y -500) |
| 0x403 | `m2FB` | `mDekuState` | behavior DEKU_TREE: draw_SUB 0210BB58 / daCC_Execute 0210DD3C keep the body on mDekuMtx while it is not 3; action_noboru 021116B8 clears it, waits for it to become non-zero (mode 0x3D), sets 3 when the ChuChu drops off and 4 when it is back |
| 0x404 | `m2FC` | `mComeDemoStep` | deku_come_demo (in action_noboru 021116B8): step of the CyuCyu_STR event (1..5, 0 when done); action_noboru sets 1 |
| 0x405 | `m2FD` | `mRetDemoStep` | deku_ret_demo (in action_noboru 021116B8): step of the CyuCyu_RET event (1..6, 0 when done); action_noboru sets 1, 2 |
| 0x406 | `m2FE` | `mStoneState` | action_up_check 02112414: 1 when the ChuChu turns to stone, 2 while it turns back, 0 when done; daCC_Draw 0210BFEC draws the rock model with the IWA01 / IWA02 animations by it; naraku_check 0210C828 sinks deeper and deletes it in water |
| 0x407 | `m2FF` | `mStoneAnmFrame` | daCC_Execute 0210DD3C: +1 per frame up to 25 while mStoneState is set; daCC_Draw 0210BFEC uses it as the frame of the rock brk/btk; action_up_check 02112414 resets it and ends the recovery at 25 |
| 0x408 | `m300` | `mInWater` | naraku_check 0210C828: set when the actor first enters water (water pillar, FALL_WATER sound, HAMON ripple), cleared with the ripple when it is out of water |
| 0x409 | `m301` | `mIsSplit` | action_damage_move (in daCC_Execute 0210DD3C) sets 1 in mode 0x2D (CCSPLIT particle, split model plays BCK_CC_PTCL) and clears it at the end; draw_SUB 0210BB58 / daCC_Draw 0210BFEC draw mpSplitMorf instead of the body while it is set |
| 0x40A | `m302` | `mBombShockDone` | shock_damage_check 0210CC58: set when a bomb within 1000 caused the damage (once per bomb), cleared when no bomb is near |
| 0x40C | `m304` | `mOtherHitState` | daCC_Create 02110D4C: 1 for colour param 15 (red, Tg also hit by other actors); body_atari_check 0210CE9C: 2 (health 0) when hit by an actor other than the player; dead_drop (in daCC_Execute 0210DD3C): 0 drops the IBALL item, 1 a heart / magic / arrow, 2 nothing |
| 0x40E | `m306` | `mAttackFrame` | action_attack_move (in daCC_Execute 0210DD3C): cleared at the attack start, +1 per frame in modes 0x1F..0x22; mBtNowFrame is 5 (parry window) while it is between REG_F(8,7) and 25 |
| 0x410 | `m308` | `mEventIdx` | deku_come_demo / action_noboru 021116B8: dComIfGp_evmng_getEventIdx("CyuCyu_STR" / "CyuCyu_RET"); fopAcM_orderOtherEventId and evmng_endCheck on it |
| 0x414 | `m30C` | `mPotId` | tsubo_search 0210DBE0: process id of the TSUBO (pot) at the actor's position; action_tubo_search 02112E74 follows that pot (fopAcM_SearchByID), deletes the actor when there is none |
| 0x418 | `m310` | `mSwingAngle` | nodeCallBack 0210B874 rotates body joints 0..BODY03 by it * jointNo / 2; damage_swing (in daCC_Execute 0210DD3C) sets x/z from the swing; cleared after a hit |
| 0x41E | `m316` | `mGroundAngle` | BG_check 0210DC8C: fopAcM_getGroundAngle; draw_SUB 0210BB58 tilts the model by its x/z; daCC_Execute 0210DD3C clears it except on the deku tree |
| 0x424 | `m31C` | `mWobbleAngle` | draw_SUB 0210BB58: +1000 per frame; the 1.1/1.0/0.9 squash scale is applied rotated by it (0 and uniform mScale when climbing, flattened, split, stone or stunned) |
| 0x428 | `m320` | `mAnmResIdx` | anm_init 0210C5C8 stores the BCK resource index it plays; action_noboru / action_oyogu / daCC_Execute 0210DD3C test it (TACHI_WALK, TACHI2HUSE, HUSE_WALK: lower Cyl height) |
| 0x42C | `m324` | `mWaterRiseSpeed` | action_oyogu (in daCC_Execute 0210DD3C): max step of pos.y towards the water height, eased up to 6; body_atari_check 0210CE9C clears it |
| 0x430 | `m328` | `mSwimBobY` | action_oyogu (in daCC_Execute 0210DD3C): eased to sin(mWavePhase)*3 - 15; draw_SUB 0210BB58 lowers the model by it |
| 0x434 | `m32C` | `mSwingAmplitude` | action_damage_move (in daCC_Execute 0210DD3C): 7000 / 16000 at a hit; damage_swing scales the sine swing by it and eases it to 0 by mSwingDecay; the damage ends below 1 |
| 0x438 | `m330` | `mSwingDecay` | action_damage_move (in daCC_Execute 0210DD3C): 250 / 500; damage_swing passes it as the step of cLib_addCalc0(mSwingAmplitude) |
| 0x43C | `m334` | `mScale` | draw_SUB 0210BB58: uniform model scale when not squashing; daCC_Create 02110D4C 1.0; action_noboru 021116B8 shrinks it to 0.1 on the way back into the deku tree |
| 0x440 | `m338` | `mCarryStartY` | action_up_check 02112414: pos.y when the stone is picked up; the shape turns towards angle.y only once the actor is 10 above it |
| 0x448 | `m340` | `mHomeRange` | daCC_Create 02110D4C: angle.z * 10 (at least 100); search_angle_set 0210DA18 (behavior FIXED) heads back to mHomePos when farther than it |
| 0x44C | `m344` | `mVoiceFrame` | action_noboru 021116B8: cM_rndF(43) per walk start; CV_CC_PARASITE plays when the body animation reaches that frame |
| 0x450 | `m348` | `mSwingPhase` | damage_swing (in daCC_Execute 0210DD3C): sine phase of the damage swing, +0x3000 per frame (not for wind); action_damage_move sets 0 / 10000 |
| 0x452 | `m34A` | `mSwingDirY` | action_damage_move (in daCC_Execute 0210DD3C): player angle + 0x8000 - shape_angle.y; damage_swing rotates the swing by it |
| 0x454 | `m34C` | `mChaseAngleOffset` | daCC_Create 02110D4C: random offset from the process id; search_angle_set 0210DA18 adds it to the angle towards the player |
| 0x456 | `m34E` | `mTimers` | daCC_Execute 0210DD3C counts all 7 down each frame; [0] move/attack waits, [2] stone shake, [3] spark sound, [4] stun / stone time, [5] deku demo, [6] switch delay (action_tomaru 02112DA0) |
| 0x464 | `m35C` | `mWavePhase` | only [0] is used: action_oyogu (in daCC_Execute 0210DD3C) +0x800 per frame for mSwimBobY, action_up_check 02112414 +20000 for the stone shake; all 5 cleared together |
| 0x470 | `m368` | `mThunderEcallBack0` | cc_class_ct 02110BB4 constructs it; daCC_Execute 0210DD3C attaches ID_AK_JN_CCTHUNDER00 to it and moves its emitter with the BODY03 joint; denki_end 0210CC00 ends it |
| 0x484 | `m37C` | `mThunderEcallBack1` | cc_class_ct 02110BB4 constructs it; daCC_Execute 0210DD3C attaches ID_AK_JN_CCTHUNDER01 to it and moves its emitter with the BODY03 joint; denki_end 0210CC00 ends it |
| 0x498 | `m390` | `mRippleEcallBack` | naraku_check 0210C828: the water ripple (HAMON) callback (dPa_rippleEcallBack, rate 0); cc_class_ct 02110BB4 constructs it; action_noboru 021116B8 ends it |
| 0x4AC | `m3A4` | `mThunderPos` | daCC_Execute 0210DD3C: translation of the BODY03 joint, the position of the CCTHUNDER00/01 particles |
| 0x4C2 | `m3BA` | `mCenterRotZ` | nodeCallBack 0210B874 rotates the CENTER joint by it about Z; action_noboru 021116B8 -0x4000 on the deku tree, 0 when it drops off |
| 0x4C4 | `m3BC` | `mHomePos` | daCC_Create 02110D4C copies current.pos; search_angle_set 0210DA18 (behavior FIXED) heads to it; naraku_check 0210C828 deletes 20000 below it; daCC_Execute 0210DD3C sets it from mDekuMtx on the deku tree |
| 0x578 | `m470` | `mBody03Pos` | nodeCallBack 0210B874: world position of the BODY03 joint; cc_eff_set 0210C364 uses it as the particle position |
| 0x8F4 | `m7EC` | `mDekuMtx` | daCC_Create 02110D4C / daCC_Execute 0210DD3C: base matrix of the body (and source of mHomePos) for behavior DEKU_TREE while mDekuState != 3; read only in this actor |
| 0xF08 | `mDFC` | `mInvisModel` | mDoExt_invisibleModel (cc_class_ct 02110BB4, useHeapInit 02110528 create on the body model, cc_class_dt 02113110); daCC_Draw 0210BFEC draws the frozen body through it (Cc_iceUpdateDL) |

Open (no name yet): `m3E0[6]` and `m428[6]` (no use in the ChuChu code).

## ph_class (Peahat and Seahat; d_a_ph.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_ph.h) keeps these GameCube offset names; every name below is from the use. The members in the two unions are also reachable as mWobbleAngles[7] (m0356..m0362) and mTimers[5] (m0364..m036C).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3DC | `m02C0` | `mBodyTopPos` | nodeCallBack_DW 023C97BC: world position of body joint 1; the propeller is placed there while attached (daPH_Execute 023CC070, daPH_Create 023CF600 copy it to mPropellerPos); eyePos and attention position follow it |
| 0x3E8 | `m02CC` | `mBodyCenterPos` | nodeCallBack_DW 023C97BC: world position of body joint 0; daPH_Execute 023CC070 sets mBodySph's centre there (or below it) |
| 0x3F4 | `m02D8` | `mPropellerCylPos` | nodeCallBack_UP 023C9678: propeller joint 0 position 80 below; daPH_Execute 023CC070 sets mAtCyl / mTgCyl's centre there |
| 0x400 | `m02E4` | `mFloatOffset` | daPH_Execute 023CC070 eases it towards mFloatTarget; DW_draw_SUB 023CBF44 / daPH_Draw 023C9994 / ph_hani_check 023CB368 add it to current.pos (the drawn body position); body_atari_check 023CA5AC folds it into current.pos on ice / light arrows |
| 0x40C | `m02F0` | `mFloatTarget` | fuwafuwa_set 023CA0BC: sine targets per axis (Peahat 30 / 200, Seahat 30 / 1000); daPH_Execute 023CC070 eases mFloatOffset to it; cleared when the drift stops |
| 0x418 | `m02FC` | `mPropellerScale` | UP_draw_SUB 023CBD80 scales the propeller model by it; 0 means no propeller (BG_check 023CBBE4, body_atari_check 023CA5AC test x != 0); ph_fujyou_grow (in daPH_Execute 023CC070) grows it to mPropellerExtraScale + 1 |
| 0x424 | `m0308` | `mHopCenterPos` | ph_bunri_move (in daPH_Execute 023CC070): current.pos when the propeller-less body lands; the hops turn back towards it when farther than 200 |
| 0x430 | `m0314` | `mPropellerPos` | UP_draw_SUB 023CBD80 translates the propeller model there; daPH_Execute 023CC070 copies mBodyTopPos while attached, moves it away by mPropellerFlyDist once it is knocked off |
| 0x43C | `m0320` | `mSquashScale` | ph_damage_dead_move (in daPH_Execute 023CC070): target of actor scale (mExtraScale + 1, 0.8/1.2 squash on the ground bounces); scale eases to it |
| 0x448 | `m032C` | `mHomePos` | daPH_Create 023CF600 copies current.pos; ph_hani_check 023CB368 (useHome) and the Seahat moves (in daPH_Execute 023CC070) measure from it and return to it; sea_water_check 023CB4C0 starts mWaterSurfacePos.y from it |
| 0x456 | `m033A` | `mPropellerAngle` | puropera_kaiten 023CA054 adds mPropellerSpeed; UP_draw_SUB 023CBD80 rotates the propeller by it; puropera_sound 023C9E58 plays a blade sound at three angles |
| 0x45B | `m033F` | `mAction` | daPH_Execute 023CC070 switches on it (PH_ACTION_FLY / BOUNCE / SEPARATED / RISE / DAMAGE / WIND / WATER); set by body_atari_check 023CA5AC, hajiki_check 023CAFB0, ph_wall_hit_check 023CB2E0 |
| 0x45C | `m0340` | `mDamageType` | body_atari_check 023CA5AC: reaction per attack type (2 wind, 3 critical sword, 4 boomerang knock-off, 6/7 skull hammer, 8 bomb, 9 hookshot, 1 arrows / fire); ph_damage_dead_move and ph_wind_move 023CFDF0 test it |
| 0x45D | `m0341` | `mInWater` | sea_water_check 023CB4C0 sets 1 on entering water (ripple, splash, water pillar), ph_leave_water (in sea_water_check) clears it; daPH_Execute 023CC070 tests it |
| 0x45E | `m0342` | `mIceHit` | body_atari_check 023CA5AC sets 1 for AT_TYPE_ICE_ARROW (mEnemyIce freeze 200); ph_damage_dead_move (in daPH_Execute 023CC070) then stops the body (mode 0x2E) |
| 0x45F | `m0343` | `mStealHitCount` | body_atari_check 023CA5AC: +1 per AT_TYPE_GRAPPLING_HOOK steal while items are left; never read |
| 0x460 | `m0344` | `mStealItemsLeft` | daPH_Create 023CF600 sets 1; body_atari_check 023CA5AC copies it to stealItemLeft for an AT_TYPE_GRAPPLING_HOOK hit and decrements it after the steal |
| 0x462 | `m0346` | `mMode` | step within mAction: every move function switches on it (ph_fly_move 0..4, ph_hane_move 10, ph_bunri_move 0x14.., ph_fujyou_move 0x1E.., ph_damage_dead_move 0x28.., ph_wind_move 023CFDF0 0x32.., ph_water_move 023D051C 0x3C..); UP_draw_SUB 023CBD80 tests 3/4/0x33 |
| 0x464 | `m0348` | `mPropellerSpeed` | puropera_kaiten 023CA054 adds it to mPropellerAngle and eases it to mPropellerTargetSpeed by mPropellerAccel; puropera_sound 023C9E58 (Seahat) sets the sound volume from it |
| 0x466 | `m034A` | `mPropellerTargetSpeed` | puropera_kaiten 023CA054: target of mPropellerSpeed; the moves set 0x1000 (hover) / 0x2000 (attack) |
| 0x468 | `m034C` | `mPropellerAccel` | puropera_kaiten 023CA054: step of mPropellerSpeed, itself eased to 0x100; the moves reset it to 0 with each new mPropellerTargetSpeed |
| 0x46A | `m034E` | `mDamageSpinSpeed` | ph_damage_dead_move (in daPH_Execute 023CC070): 0x2000 / 0x1000 / -0x8000 at a hit, added to shape_angle.y and eased to 0 |
| 0x46C | `m0350` | `mSeparatedTimer` | ph_bunri_move (in daPH_Execute 023CC070): 900 (Peahat) / 150 (Seahat) when the propeller comes off, counts down while hopping; at 0 the propeller regrows (PH_ACTION_RISE) |
| 0x46E | `m0352` | `mTargetAngleY` | ph_fly_move / ph_fly_sea_move (in daPH_Execute 023CC070): angle to the player or home; current.angle.y eases to it |
| 0x470 | `m0354` | `mPropellerSeFlags` | puropera_sound 023C9E58 (Peahat): bits 1/2/4 for the three blade angles already sounded, cleared when all three are set |
| 0x472 | `m0356` | `mFloatPhaseY` | fuwafuwa_set 023CA0BC: +700 (Seahat 350) per frame, sine of it is mFloatTarget.y; ph_fujyou_rise +2000; daPH_Create 023CF600 seeds it from the process id |
| 0x474 | `m0358` | `mFloatPhaseX` | fuwafuwa_set 023CA0BC: +200 (Seahat 100) per frame, cosine of it is mFloatTarget.x; daPH_Create 023CF600 seeds it from the process id |
| 0x476 | `m035A` | `mFloatPhaseZ` | fuwafuwa_set 023CA0BC: +200 (Seahat 100) per frame, cosine of it is mFloatTarget.z; daPH_Create 023CF600 seeds it from the process id |
| 0x478 | `m035C` | `mPitchWobblePhase` | fly_angle_set 023CA258: +800 (Seahat 900) per frame, shape_angle.x eases to sin(it) * 4000 |
| 0x47A | `m035E` | `mRollWobblePhase` | fly_angle_set 023CA258 (mode 4) +3000, angle.z = sin(it) * 7000; ph_fujyou_rise and ph_wind_move 023CFDF0 roll by its sine; ph_damage_dead_move also counts ground bounces in it |
| 0x47C | `m0360` | `mSpinSpeed` | ph_wind_move 023CFDF0: +-10000 / +-1000 added to mRollWobblePhase, later to shape_angle.y |
| 0x47E | `m0362` | `mWindTurnSpeed` | ph_wind_move 023CFDF0: +-0x1000 added to shape_angle.y while blown away, eased to 0 |
| 0x480 | `m0364` | `mActionTimer` | mTimers[0] (counted down in daPH_Execute 023CC070): waits of the fly / hane / damage moves (ph_fly_move 20, 60..120, ph_hane_move 15, ph_damage_dead_move 0x3C) |
| 0x482 | `m0366` | `mWaitTimer` | mTimers[1]: ph_water_anm random wait in water (100..400), ph_wind_move 023CFDF0 50..100, ph_fly_sea_move 15-frame splash interval |
| 0x484 | `m0368` | `mHomeCheckTimer` | mTimers[2]: ph_fly_sea_move / ph_water_move 023D051C (30 / 20); the distance-from-home check runs only at 0 |
| 0x488 | `m036C` | `mHitCooldown` | mTimers[4]: body_atari_check 023CA5AC sets 8 after a hit and ignores hits while it is not 0 |
| 0x48C | `m0370` | `mBobPhase` | sea_water_check 023CB4C0: +1000 per frame while floating, pos.y bobs by its sine below the water height; cleared on entering |
| 0x48E | `m0372` | `mAttackDelay` | ph_fly_sea_move (in daPH_Execute 023CC070): Seahat waits it out (0x3C after an attack) before checking the player again |
| 0x490 | `m0374` | `mPropellerAnmIdx` | anm_init 023C9AD8 stores the propeller BCK index (part 0, Peahat); the moves skip re-starting 0x1C / 0x1E when it is already playing |
| 0x494 | `m0378` | `mHeightAbovePlayer` | fly_angle_set 023CA258: pitch aims at player y + it; the moves set 150 / 80 / 100 |
| 0x498 | `m037C` | `mPropellerFlyDist` | daPH_Execute 023CC070: once the propeller is knocked off it eases to 10 / 30 and moves mPropellerPos by it; reset to 0 with mPropellerScale 0; 0 with mPropellerScale != 0 means attached (BG_check 023CBBE4, body_atari_check 023CA5AC) |
| 0x49C | `m0380` | `mBgCheckOffsetY` | ph_crr_pos (in BG_check 023CBBE4): pos is lowered by it for mAcch.CrrPos and raised back; daPH_Create 023CF600 30 (Seahat 100), ph_damage_dead_move 10 |
| 0x4A0 | `m0384` | `mRiseSpeed` | ph_fujyou_rise (in daPH_Execute 023CC070): speed.y, eased to the given target; also drives mPropellerAngle and the roll |
| 0x4A4 | `m0388` | `mRiseTargetY` | ph_fujyou_move (in daPH_Execute 023CC070): pos.y + 200 at the take-off start; flying resumes above it |
| 0x4A8 | `m038C` | `mHomeRange` | daPH_Create 023CF600: mNoticeRange + 800; ph_fly_sea_move / ph_water_move 023D051C return home when farther from mHomePos than it |
| 0x4AC | `m0390` | `mNoticeRange` | daPH_Create 023CF600: param bits 8-15 * 100 (default 1000, Seahat 12000); ph_hani_check 023CB368 horizontal range for starting an attack |
| 0x4B0 | `m0394` | `mNoticeHeight` | daPH_Create 023CF600: param bits 16-23 * 100 (default 500, Seahat 6000); ph_hani_check 023CB368 height range for starting an attack |
| 0x4B4 | `m0398` | `mWaterFollowSpeed` | sea_water_check 023CB4C0: step of pos.y towards the water height, eased to 100 / 30; ph_leave_water clears it |
| 0x4B8 | `m039C` | `mExtraScale` | daPH_Create 023CF600: 0, Seahat 8 (actor scale 9); added to particle / fire / ice scales and the damage squash (body_atari_check 023CA5AC, dead_item 023CBAC0) |
| 0x4BC | `m03A0` | `mPropellerExtraScale` | daPH_Create 023CF600: Seahat 5; the propeller scale is mPropellerExtraScale + 1 (ph_fujyou_grow, daPH_Execute 023CC070) |
| 0x6D8 | `m05BC` | `mWaterSurfacePos` | sea_water_check 023CB4C0: current.pos x/z + mFloatOffset with y = the sea wave / water height; ripple and splash position; the moves fly 500 above its y |

Open (no name yet): `m036E` (ph_hane_move only clears it; never read).

## bb_class (Kargaroc; d_a_bb.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_bb.h) keeps these GameCube offset names (`unk_XXX`); every name below is from the use.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x448 | `unk_2CC` | `mEyeFrame` | tex_anm_set 0205D964 clears it; bb_eye_tex_anm (in daBb_Execute 0205EA58) advances it up to mEyeMaxFrame; daBb_Draw 0205E420 enters mBtpAnm at this frame |
| 0x449 | `unk_2CD` | `mEyeAnmOn` | tex_anm_set 0205D964 sets 1 when an eye BTP starts; bb_eye_tex_anm (in daBb_Execute 0205EA58) clears it at mEyeMaxFrame and starts a blink only while it is 0 |
| 0x44A | `unk_2CE` | `mEyeMaxFrame` | tex_anm_set 0205D964: bb_tex_max_frame[idx], the last frame of the eye BTP; bb_eye_tex_anm (in daBb_Execute 0205EA58) stops there |
| 0x44C | `unk_2D0` | `mBlinkWait` | bb_eye_tex_anm (in daBb_Execute 0205EA58): counts down, then reloads rnd(50)+10 and starts the blink BTP (tex_anm_set 4); bb_atack_move sets 2 |
| 0x44E | `unk_2D8` | `mPrmMode` | daBb_Create 02061DEC: param bits 0-7 (BB_MODE_*), copied to mBehavior; bb_atack_move (in daBb_Execute 0205EA58) returns to the wait modes it names |
| 0x44F | `unk_2D9` | `mSightRangePrm` | daBb_Create 02061DEC: param bits 8-15; bb_player_view_check 0205DFC0 sees the player within it * 100 (0xFF: l_bbHIO ranges) |
| 0x450 | `unk_2DA` | `mPathNo` | daBb_Create 02061DEC: param bits 16-23, dPath_GetRoomPath(it, room) when not 0xFF; path_check 0205E14C / bb_atack_move store it + 1 in mPathOn |
| 0x451 | `unk_2DB` | `mAppearSwitchNo` | daBb_Create 02061DEC: param bits 24-31; when not 0xFF mAppearWait = it + 1 |
| 0x452 | `unk_2DC` | `mDeleteSwitchNo` | daBb_Create 02061DEC: home.angle.z (0xFF -> 0); the actor is not created when event bit 0x1101 and this switch are set |
| 0x453 | `unk_2DD` | `mBehavior` | daBb_Create 02061DEC: from mPrmMode; daBb_Execute 0205EA58 picks bb_atack_move / bb_wait_move / bb_su_wait_move / bb_auto_move by it (BB_MODE_*); damage_check 0206432C and bb_player_view_check 0205DFC0 switch it to BB_MODE_ATTACK |
| 0x455 | `unk_2DF` | `mCarryState` | daBb_Create 02061DEC 1 for BB_MODE_CARRY_MOBLIN / _BOKOBLIN; daBb_Execute 0205EA58: 1 FLYC animation, 2 waits for the carried actor, 3 holds it between the feet, 0 when dropped; anm_init 0205DA3C changes animations only below 3 |
| 0x456 | `unk_2E0` | `mDropCarried` | bb_path_move (in daBb_Execute 0205EA58) sets 1; daBb_Execute then lets the carried Moblin / Bokoblin go (damage reaction action 31, speedF 40) and ends mCarryState |
| 0x458 | `unk_2E4` | `mpCarriedActor` | daBb_Execute 0205EA58: the carried actor once fpcSCtRq created it (mCarryState 3 places it between mFootPos) |
| 0x45C | `unk_2E8` | `mCarriedRequestId` | daBb_Create 02061DEC: fpcSCtRq_Request id of the carried MO2 / BK; daBb_Execute 0205EA58 looks the actor up by it |
| 0x460 | `unk_2EC` | `mCarriedProcName` | daBb_Create 02061DEC: fpcNm_MO2_e or fpcNm_BK_e; daBb_Execute 0205EA58 picks the carried actor's damage reaction layout by it |
| 0x464 | `unk_2F0` | `mFlyAnmState` | bb_path_move / bb_kamome_attack (in daBb_Execute 0205EA58): 0 flapping (FLY01), 1 gliding (FLY02) while above mTargetPos, 10 landing (LAND01, bb_auto_move 020625E8 / bb_wait_move) |
| 0x465 | `unk_2F1` | `mMode` | step within the current move: bb_path_move, bb_kamome_attack, bb_atack_move (in daBb_Execute 0205EA58), bb_auto_move 020625E8, bb_wait_move 0206348C switch on it; damage_check 0206432C clears it |
| 0x466 | `unk_2F2` | `mAppearWait` | daBb_Create 02061DEC: mAppearSwitchNo + 1; daBb_Execute 0205EA58 does nothing and daBb_Draw 0205E420 draws nothing until switch (it - 1) is set, then 0 |
| 0x468 | `unk_2F4` | `mTargetPos` | bb_pos_move 0205E638 / bb_ground_pos_move 0205E8AC fly to it; path point position (bb_path_move), circle centre (bb_atack_move); bb_water_check 02063404 tests the sea there |
| 0x474 | `unk_300` | `mTargetSpeed` | bb_pos_move 0205E638 / bb_ground_pos_move 0205E8AC: speedF eases to it by mSpeedStep; the moves set 0..30 |
| 0x478 | `unk_304` | `mSpeedStep` | bb_pos_move 0205E638 / bb_ground_pos_move 0205E8AC: step of speedF towards mTargetSpeed |
| 0x47C | `unk_308` | `mTurnScale` | bb_pos_move 0205E638 / bb_ground_pos_move 0205E8AC: turn step = mTurnSpeed * it, eased back to 1; the moves set 0 to stop turning; bb_atack_move also scales the circle approach by it |
| 0x480 | `unk_30C` | `mAttackTurnScale` | bb_atack_move (in daBb_Execute 0205EA58): turn steps of angle y/x/z scaled by it, cleared at a new attack and eased to 1 |
| 0x484 | `unk_310` | `mTurnSpeed` | bb_pos_move 0205E638 / bb_ground_pos_move 0205E8AC: maximum turn step (times mTurnScale); the moves set 400..5000 |
| 0x48C | `unk_318` | `mTimers` | daBb_Execute 0205EA58 counts all 6 down each frame; [0] move waits, [1] attack time, [2] path point wait, [3] bait search delay (daBb_Create 300..500), [5] next swoop |
| 0x498 | `unk_324` | `mLiftTimer` | bb_pos_move 0205E638: while it counts down pos.y rises 5 per frame; bb_path_move / bb_auto_move 020625E8 set REG0_S(4) + 10 at take-off |
| 0x49A | `unk_326` | `mGuardedTimer` | bb_atack_move (in daBb_Execute 0205EA58) sets 10 when Link's guard stops the head attack (mHeadAtSph hit, LK_ROPE_HOOK_METAL sound); daBb_Execute counts it down; never read (same in tww d217e2b) |
| 0x4A0 | `unk_32C` | `mKeyTurnSpeed` | no use in WWHD; tww d217e2b bb_key_move (a stick-steered flight mode that WWHD does not have) eases it toward stick X * -400 and adds it to current.angle.y |
| 0x4A4 | `unk_330` | `mEsaId` | bb_path_move (in daBb_Execute 0205EA58): process id of the bait (ESA) found by search_esa 0205DCFC; bb_auto_move 020625E8 flies to it (fopAcM_SearchByID) |
| 0x4AA | `unk_336` | `mPlayerAngleY` | daBb_Execute 0205EA58: cM_atan2s towards the player; bb_atack_move / wait_move_body turn to it; bb_player_view_check 0205DFC0 tests the view cone with it |
| 0x4AC | `unk_338` | `mPlayerAngleX` | daBb_Execute 0205EA58: pitch towards the player's y + 100; bb_atack_move turns angle.x to it |
| 0x4B0 | `unk_33C` | `mPlayerDistXZ` | daBb_Execute 0205EA58: horizontal distance to the player; bb_player_view_check 0205DFC0 and bb_atack_move compare it |
| 0x4B4 | `unk_340` | `mHitCooldown` | damage_check 0206432C ignores hits while it is not 0, sets 5 (50 for fire); daBb_Execute 0205EA58 counts it down |
| 0x4B6 | `unk_342` | `mKnockbackAngleY` | damage_check 0206432C: mPlayerAngleY or the attack's direction; daBb_Execute 0205EA58 pushes the body back along it by mKnockbackDist |
| 0x4B8 | `unk_344` | `mKnockbackAngleX` | damage_check 0206432C: mPlayerAngleX or 0; daBb_Execute 0205EA58 pitch of the knock-back push |
| 0x4BC | `unk_348` | `mKnockbackDist` | damage_check 0206432C: l_bbHIO knock-back distance (dead / alive); daBb_Execute 0205EA58 moves the body by it and eases it to 0 |
| 0x4C0 | `unk_34C` | `mBodyShakeAmp` | damage_check 0206432C 5000 / 3000; daBb_Execute 0205EA58 shakes the 11 mJointRot joints by sines of it (at most 4000) and eases it to 0 |
| 0x4C4 | `unk_350` | `mKnockbackSpinSpeed` | damage_check 0206432C: +-(5000..8000) on a kill, 0 otherwise; daBb_Execute 0205EA58 adds it to angle.y and eases it to 0 |
| 0x4C6 | `unk_352` | `mFrameCounter` | daBb_Execute 0205EA58: +1 per frame (daBb_Create starts it at rnd(10000)); phase of the joint shake, tail wave (tail_control 0205D4BC) and the random head turns / calls |
| 0x4C8 | `unk_354` | `mMoveCounter` | bb_atack_move (in daBb_Execute 0205EA58): +1 per frame, phase of the circling sines; bb_auto_move 020625E8 / bb_wait_move: frames since landing (compared with l_bbHIO) |
| 0x4CC | `unk_358` | `mPrevEyeY` | head_water_check (in daBb_Execute 0205EA58): eyePos.y of the last frame; a splash when the eye crosses the wave height |
| 0x4D1 | `unk_35D` | `mPathOn` | path_check 0205E14C / daBb_Create 02061DEC: mPathNo + 1 while the path is followed, 0 otherwise; daBb_Execute 0205EA58 runs bb_path_move while it is set |
| 0x4D2 | `unk_35E` | `mPathPoint` | path_check 0205E14C / bb_path_move (in daBb_Execute 0205EA58): index of the current path point (ppd->getPoint(it)) |
| 0x4D3 | `unk_35F` | `mPathDir` | bb_path_move (in daBb_Execute 0205EA58): +1 / -1 step through the path points (reverses at the end of an open path); daBb_Create 02061DEC 1 |
| 0x4D8 | `unk_364` | `mPathReentry` | bb_auto_move 020625E8 sets 1 when it goes back to the path; bb_path_move (in daBb_Execute 0205EA58) then starts at speed 25 / step 2 and clears it |
| 0x4DA | `unk_366` | `mBodyPitchOffset` | daBb_Execute 0205EA58 adds it to current.angle.x in both body matrix rotations (XrotM); nothing in WWHD sets it; tww d217e2b bb_key_move eases it toward stick Y * 5000 |
| 0x4DC | `unk_368` | `mBodyYawOffset` | daBb_Execute 0205EA58 adds it to current.angle.y in both body matrix rotations (YrotM); nothing in WWHD sets it; tww d217e2b bb_key_move eases it toward stick X * -5000 |
| 0x6F0 | `unk_57C` | `mNoBgCheck` | bb_wait_move 0206348C sets 1 while perched / landing; daBb_Execute 0205EA58 skips mAcch.CrrPos that frame and clears it |
| 0xBE0 | `unk_A6C` | `mFootPos` | nodeCallBack 0205D078: positions of the FOOTL / FOOTR joints; daBb_Execute 0205EA58 holds the carried actor at their midpoint |
| 0xBF8 | `unk_A84` | `mpTailModels` | useHeapInit 020619D0: 9 tail feather models; daBb_Draw 0205E420 places them along mTailPos / mTailAngle |
| 0xC1C | `unk_AA8` | `mTailPos` | tail_control 0205D4BC: positions of the 10 tail segments ([0] = the OB joint, copied in daBb_Execute 0205EA58); daBb_Draw 0205E420 draws the feathers there |
| 0xC94 | `unk_B20` | `mTailAngle` | tail_control 0205D4BC: angle of each tail segment towards the next; daBb_Draw 0205E420 rotates the feathers by it |
| 0xCD0 | `unk_B5C` | `mTailSpeed` | tail_control 0205D4BC: per segment movement of the last frame (times REG0_F(2) + 0.8), added back as inertia |
| 0xD48 | `unk_BD4` | `mTailBasePos` | nodeCallBack 0205D078: positions of the OA and OB (tail base) joints; tail_control 0205D4BC takes the tail direction from them |
| 0xD60 | `unk_BEC` | `mTailBaseAngleX` | tail_control 0205D4BC: pitch from mTailBasePos[0] to [1] |
| 0xD62 | `unk_BEE` | `mTailBaseAngleY` | tail_control 0205D4BC: yaw from mTailBasePos[0] to [1] |
| 0xD68 | `unk_BF4` | `mTailBaseDir` | tail_control 0205D4BC: a 5-unit step along mTailBaseAngleX/Y, added (scaled down per segment) to every tail segment |
| 0xD74 | `unk_C00` | `mJointRot` | nodeCallBack 0205D078 rotates the joints listed in callback_check_index by it [0..10]; daBb_Execute 0205EA58 clears it per frame and adds the shake / head turn / beak angles |
| 0xDC2 | `unk_C4E` | `mBeakOpenAngle` | daBb_Execute 0205EA58 eases it to 0x2000 while mBeakOpenTimer runs; nodeCallBack 0205D078 opens the KUCHIA / KUCHIB joints by it |
| 0xDC4 | `unk_C50` | `mBeakOpenTimer` | kuti_open 0205DE54 sets it (when 0); daBb_Execute 0205EA58 counts it down and keeps the beak open |
| 0xDC6 | `unk_C52` | `mBeakSeFrame` | kuti_open 0205DE54: open time - 3; daBb_Execute 0205EA58 plays mBeakSeId when mBeakOpenTimer reaches it |
| 0xDC8 | `unk_C54` | `mBeakSeId` | kuti_open 0205DE54: the voice sound; daBb_Execute 0205EA58 fopAcM_monsSeStart(it) |
| 0xDCC | `unk_C58` | `mHeadShakeAmp` | damage_check 0206432C 10000 for a hit that is not mbDead; daBb_Execute 0205EA58 turns the neck / head joints by it, shakes mHeadTargetAngleY, eases it to 0 |
| 0xDD0 | `unk_C5C` | `mHeadAngleY` | daBb_Execute 0205EA58: eased to mHeadTargetAngleY, subtracted from the head joints' x; bb_player_view_check 0205DFC0 adds it to the view direction |
| 0xDD2 | `unk_C5E` | `mHeadTargetAngleY` | daBb_Execute 0205EA58: by mHeadLookMode, towards the player (+-15000), random, or the damage shake |
| 0xDD4 | `unk_C60` | `mHeadLookMode` | the moves set it per frame (1 look at the player, 2 look around and call, 0 none); daBb_Execute 0205EA58 sets 3 while mHeadShakeAmp > 1, uses it and clears it |
| 0xDF0 | `unk_C7C` | `mWasHit` | damage_check 0206432C sets 1; bb_atack_move (in daBb_Execute 0205EA58) clears it, restarts mTimers[5] and flies off (mode 4, FLYB01) |
| 0xDF2 | `unk_C7E` | `mDying` | damage_check 0206432C sets 1 when health is <= 0 after the hit (FAINTED voice); daBb_Execute 0205EA58 then runs bb_fail_move instead of the other moves |

Open (no name yet): `unk_328` (read by bb_path_move as the case-23 dash speed factor, never written; no name settles from that), the unused members `unk_2DE`, `unk_C42`, and the padding arrays `unk314`, `unk334`, `unk35C`, `unk36A`, `unkBF0`, `unkC61`, `unkC7D`.

### bbHIO_c (l_bbHIO, the Kargaroc tuning block; d_a_bb.h)

Old names are tww's (`unk_XX`, unnamed there); the HD layout is the GameCube one. __sinit_d_a_bb_cpp 02062394 (bbHIO_c::bbHIO_c)
sets every field; nothing else writes them. Named from the reads.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x04 | `unk_04` | `mResetHealthOnHit` | damage_check 0206432C sets health back to 10 on every hit while it is set; __sinit_d_a_bb_cpp 02062394 sets the default 0 |
| 0x05 | `unk_05` | `mDebugPause` | daBb_Execute 0205EA58 counts the timers down, applies the knock-back and runs the move / animation update only while it is 0; __sinit_d_a_bb_cpp 02062394 sets the default 0 |
| 0x08 | `unk_08` | `mSeagullAttackSpeed` | bb_kamome_attack (in daBb_Execute 0205EA58): mTargetSpeed while chasing the seagull; __sinit_d_a_bb_cpp 02062394 sets the default 20 |
| 0x0C | `unk_0C` | `mWanderRange` | bb_auto_move 020625E8: the next flight target is home.pos + cM_rndFX(it) in x and z; __sinit_d_a_bb_cpp 02062394 sets the default 2500 |
| 0x12 | `unk_12` | `mAttackTimeMin` | bb_atack_move (in daBb_Execute 0205EA58), mode 4 entry from the free-flying modes: mTimers[1] (time circling the player before giving up) = random in [it, mAttackTimeMax); __sinit_d_a_bb_cpp 02062394 sets the default 0x258 |
| 0x14 | `unk_14` | `mAttackTimeMax` | bb_atack_move (in daBb_Execute 0205EA58): upper end of that random attack time (mAttackTimeMin); __sinit_d_a_bb_cpp 02062394 sets the default 0x4B0 |
| 0x16 | `unk_16` | `mWaitAttackTimeMin` | bb_atack_move (in daBb_Execute 0205EA58), mode 4 entry from BB_MODE_WAIT / BB_MODE_NEST_WAIT, and damage_check 0206432C when a hit starts BB_MODE_ATTACK: mTimers[1] = random in [it, mWaitAttackTimeMax); __sinit_d_a_bb_cpp 02062394 sets the default 0x1C2 |
| 0x18 | `unk_18` | `mWaitAttackTimeMax` | bb_atack_move (in daBb_Execute 0205EA58) and damage_check 0206432C: upper end of that random attack time (mWaitAttackTimeMin); __sinit_d_a_bb_cpp 02062394 sets the default 0x2EE |
| 0x1C | `unk_1C` | `mTurnScaleMax` | bb_atack_move (in daBb_Execute 0205EA58), modes 4 / 5: mTurnScale eases toward it (step 0.1 * it); __sinit_d_a_bb_cpp 02062394 sets the default 0.35 |
| 0x20 | `unk_20` | `mAttackWaitScale` | bb_atack_move (in daBb_Execute 0205EA58): mTimers[5] (wait before the next dive, mode 5) = (rnd(30) + 20) * it after a hit, a dive or a crash; __sinit_d_a_bb_cpp 02062394 sets the default 0.5 |
| 0x24 | `unk_24` | `mFlyAnmSpeed` | bb_path_move / bb_atack_move (in daBb_Execute 0205EA58), bb_auto_move 020625E8, wait_move_body (in bb_wait_move 0206348C, x 1.5): speed of the FLY01 animation; __sinit_d_a_bb_cpp 02062394 sets the default 1.5 |
| 0x28 | `unk_28` | `mHoverAnmSpeed` | bb_atack_move (in daBb_Execute 0205EA58): speed of the FLYB01 animation while circling the player (mode 4); __sinit_d_a_bb_cpp 02062394 sets the default 1.0 |
| 0x2C | `unk_2C` | `mDiveAnmSpeed` | bb_atack_move (in daBb_Execute 0205EA58): speed of the ATACK01 animation of the dive (mode 6); __sinit_d_a_bb_cpp 02062394 sets the default 0.85 |
| 0x30 | `unk_30` | `mCrashTime` | bb_atack_move (in daBb_Execute 0205EA58): mTimers[0] after the dive hits a wall (modes 10 GUSYA / 11 FURA) before circling again; __sinit_d_a_bb_cpp 02062394 sets the default 0x2D |
| 0x32 | `unk_32` | `mDiveWindUpTime` | bb_atack_move (in daBb_Execute 0205EA58): mTimers[0] of mode 5 (wind-up before the dive, mode 6); __sinit_d_a_bb_cpp 02062394 sets the default 20 |
| 0x34 | `unk_34` | `mDiveWindUpAnmSpeed` | bb_atack_move (in daBb_Execute 0205EA58): speed of the FLYB01 animation in mode 5 (wind-up before the dive); __sinit_d_a_bb_cpp 02062394 sets the default 2.5 |
| 0x38 | `unk_38` | `mCrashAnmSpeed` | bb_atack_move (in daBb_Execute 0205EA58): speed of the GUSYA01 (crash) animation, mode 10; __sinit_d_a_bb_cpp 02062394 sets the default 0.85 |
| 0x3C | `unk_3C` | `mDizzyAnmSpeed` | bb_atack_move (in daBb_Execute 0205EA58): speed of the FURA01 (wobble) animation after the crash, mode 11; __sinit_d_a_bb_cpp 02062394 sets the default 1.0 |
| 0x40 | `unk_40` | `mLandDelay` | wait_move_body (in bb_wait_move 0206348C): gravity and the LAND02 landing start only once mMoveCounter is above it; __sinit_d_a_bb_cpp 02062394 sets the default 30 |
| 0x44 | `unk_44` | `mLand1AnmSpeed` | wait_move_body (in bb_wait_move 0206348C): speed of the LAND01 animation; __sinit_d_a_bb_cpp 02062394 sets the default 1.0 |
| 0x48 | `unk_48` | `mLand2AnmSpeed` | wait_move_body (in bb_wait_move 0206348C): speed of the LAND02 animation; __sinit_d_a_bb_cpp 02062394 sets the default 1.0 |
| 0x4C | `unk_4C` | `mCarryFlyAnmSpeed` | daBb_Execute 0205EA58, carry state 1: speed of the FLYC (carrying) animation; __sinit_d_a_bb_cpp 02062394 sets the default 1.5 |
| 0x50 | `unk_50` | `mSwoopDist` | bb_atack_move (in daBb_Execute 0205EA58) mode 1, bb_auto_move 020625E8 mode 10 and wait_move_body (in bb_wait_move 0206348C) mode 11: closer than it to mTargetPos it switches to the FLY03 swoop animation and the next mode; __sinit_d_a_bb_cpp 02062394 sets the default 0x44C |
| 0x54 | `unk_54` | `mEsaWalkSpeed` | bb_auto_move 020625E8, mode 23: mTargetSpeed while walking to the bait (esa) on the ground; __sinit_d_a_bb_cpp 02062394 sets the default 3.5 |
| 0x58 | `unk_58` | `mKnockbackDistDead` | damage_check 0206432C: mKnockbackDist for a killing hit; __sinit_d_a_bb_cpp 02062394 sets the default 45 |
| 0x5C | `unk_5C` | `mKnockbackDistAlive` | damage_check 0206432C: mKnockbackDist for a hit it survives; __sinit_d_a_bb_cpp 02062394 sets the default 30 |
| 0x68 | `unk_68` | `mSightDist` | bb_player_view_check 0205DFC0: the player is out of sight farther than it (XZ), except in the wait modes and with mSightRangePrm; __sinit_d_a_bb_cpp 02062394 sets the default 8000 |
| 0x6C | `unk_6C` | `mWaitSightDist` | bb_player_view_check 0205DFC0: the same in BB_MODE_WAIT / BB_MODE_NEST_WAIT; __sinit_d_a_bb_cpp 02062394 sets the default 1500 |
| 0x70 | `unk_70` | `mViewAngle` | bb_player_view_check 0205DFC0: angle window toward the player outside the wait modes; __sinit_d_a_bb_cpp 02062394 sets the default 0x6D60 |
| 0x72 | `unk_72` | `mWaitViewAngle` | bb_player_view_check 0205DFC0: angle window in BB_MODE_WAIT / BB_MODE_NEST_WAIT; __sinit_d_a_bb_cpp 02062394 sets the default 0x59D8 |
| 0x74 | `unk_74` | `mViewHeight` | bb_player_view_check 0205DFC0: the player is in sight only while the height difference to eyePos is below it (outside the wait modes); __sinit_d_a_bb_cpp 02062394 sets the default 3000 |
| 0x78 | `unk_78` | `mWaitViewHeight` | bb_player_view_check 0205DFC0: the same height limit in BB_MODE_WAIT / BB_MODE_NEST_WAIT; __sinit_d_a_bb_cpp 02062394 sets the default 2000 |
| 0x7C | `unk_7C` | `mGiveUpDist` | bb_atack_move (in daBb_Execute 0205EA58): farther than it from mTargetPos (or the target blocked) the attack ends and mBehavior returns to mPrmMode; __sinit_d_a_bb_cpp 02062394 sets the default 10000 |

Open: `unk_06` (two effects: bb_player_view_check 0205DFC0 always sees the player while it is set, and bb_path_move / bb_auto_move 020625E8 search for bait regardless of the carry timer; no single role), `unk_10` (nothing sets or reads it in WWHD; tww's constructor sets 0x3228 and only its demo version reads it, as a switch for the seagull attack), `unk_60`, `unk_62`, `unk_64` (only the constructor sets them; tww the same), the padding `_07`, `_1A`, `_42`, `_52`, `_66`.

## pw_class (Poe; d_a_pw.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_pw.h) keeps these GameCube offset names (`mXXX`) and TP has no pw_class, so every name below is from the use. `m344` is also read by Jalhalla through the view `BpwChildPoeHD` (d_a_bpw.h), renamed with it.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D8 | `m2BC` | `mpColorBtp` | useHeapInit 0244DE14 creates the BTP_IROGAE1 (colour change) animation into it; daPW_Draw 02449F80 sets its frame from mColorIndex |
| 0x3DC | `m2C0` | `mpMaterializeBrk` | useHeapInit 0244DE14: BRK_JITTAIKA1; daPW_Draw 02449F80 enters it while mMaterialized; action_itai sets it up again |
| 0x3E0 | `m2C4` | `mpHitBrk` | useHeapInit 0244DE14: BRK_HIT; action_itai (in daPW_Execute 0244B3E4) starts it with mHitBrkOn; daPW_Draw 02449F80 enters it while mHitBrkOn |
| 0x3E4 | `m2C8` | `mpDefaultBrk` | useHeapInit 0244DE14: BRK_DEFAULT; daPW_Draw 02449F80 enters it when neither mMaterialized nor mHitBrkOn |
| 0x3E8 | `m2CC` | `mLanternHandPos` | nodeCallBack 02449C60: world position of joint 0x17 (the hand); kantera_calc 0244F730 puts the lantern (proc 0xC1) there; daPW_Create 0244E398 seeds it |
| 0x3F4 | `m2D8` | `mLanternSphPos` | kantera_calc 0244F730 copies the lantern's mBonPos; daPW_Execute 0244B3E4 makes it the centre of the 40-unit lantern sphere mSph |
| 0x400 | `m2E4` | `mPrevLanternHandPos` | nodeCallBack 02449C60 copies mLanternHandPos here before updating it; no other use |
| 0x40C | `m2F0` | `mHomePos` | daPW_Create 0244E398 copies the spawn position; hani_check 0244B058 keeps a pathless Poe within 1000 of it; fuwafuwa_calc 0244AA98 hovers at its height; next_dousa_check 0244B1B8 moves x/z to the current position |
| 0x418 | `m2FC` | `mPathAreaCenter` | action_dousa (in daPW_Execute 0244B3E4) mode 20 copies the position every frame while following the path; hani_check 0244B058 keeps a path Poe within 500 of it |
| 0x424 | `m308` | `mLineStart` | Line_check 0244A7F0: actor position + 80 in y, start of the dBgS_LinChk |
| 0x430 | `m314` | `mLineEnd` | Line_check 0244A7F0: target position (+ 80 in y), end of the dBgS_LinChk |
| 0x43C | `m320` | `mCircleCenter` | action_kougeki (in daPW_Execute 0244B3E4) copies the position after an attack; fuwafuwa_calc 0244AA98 circles around it in mode 0x26 |
| 0x45A | `m33E` | `mMaterialized` | set 1 by action_torituku 0244FE24 mode 0x50, daPW_Create 0244E398 (type 3) and action_itai (light hit); cleared by action_big_demo 024500C0, action_dousa and action_itai mode 60; daPW_Draw 02449F80 draws with the JITTAIKA brk when set; alpha_anime 0244A9E4 runs only while 0 |
| 0x45B | `m33F` | `mHitBrkOn` | action_itai (in daPW_Execute 0244B3E4) mode 53 sets 1 and starts BRK_HIT; mode 60 / action_dousa clear it and go back to BRK_DEFAULT; daPW_Draw 02449F80 picks mpHitBrk while set |
| 0x45C | `m340` | `mNoTurn` | daPW_Execute 0244B3E4 turns current/shape angle toward mTargetAngleY only while 0; set 1 for the appear spin (action_dousa mode 1), the jump attack (action_kougeki mode 110), action_demo; Big_pow_gattai_check 0244A5A8 clears |
| 0x45D | `m341` | `mLanternSwingMode` | kantera_calc 0244F730: 0 slow swing (speed 3000, amplitudes 5000/-1000), 1 fast swing (4000, 10000/-10000), 2 no swing; action_dousa sets 0 / 2, action_kougeki and action_itai 1 |
| 0x45E | `m342` | `mLanternHitLatch` | kantera_atari_check 0244F28C: set at a lantern hit and cleared when there is none, so one contact takes one lantern health |
| 0x45F | `m343` | `mPossessing` | action_kougeki (in daPW_Execute 0244B3E4) sets 1 together with global TORITUKI_ON = 1 (the Poe possesses Link); action_torituku 0244FE24 and daPW_Delete 0244DD6C clear both |
| 0x460 | `m344` | `mKilledJalhalla` | Big_pow_down_check 0244A67C sets 1 when this Poe's hit takes Jalhalla's health to 0; daBPW checks it on its child Poes (view BpwChildPoeHD in d_a_bpw.h) |
| 0x461 | `m345` | `mJalhallaHitDone` | Big_pow_down_check 0244A67C sets 1 after this dead Poe took one health from Jalhalla and returns early while it is set |
| 0x462 | `m346` | `mLanternState` | daPW_Create 0244E398 sets -1 (0 for the other types); kantera_break 0244A4A8 and action_dousa mode 0 set 0 (no lantern); action_dousa sets 1 once the lantern is held; daPW_Execute 0244B3E4 runs kantera_calc / kantera_atari_check only at 1, kantera_calc 0244F730 returns at <= 0 |
| 0x494 | `m378` | `mActionTimer` | daPW_Execute 0244B3E4 counts it down (first of 6 timers); the wait times of action_dousa (rnd 60/120), action_torituku (300), action_demo (30), action_itai (stun mStunTime + 75) |
| 0x496 | `m37A` | `mObstacleCheckTimer` | action_dousa / action_big_demo 024500C0: Line_check and hani_check are skipped while it runs; reloaded 10 / rnd(20)+20 after hitting an obstacle; counted down by daPW_Execute 0244B3E4 |
| 0x498 | `m37C` | `mAttackWaitTimer` | next_dousa_check 0244B1B8 and action_kougeki set rnd(70)+70; action_dousa (in daPW_Execute 0244B3E4) checks for an attack (modes 14/16/20) only when it is 0 |
| 0x49A | `m37E` | `mWanderTurnTimer` | action_dousa / action_big_demo 024500C0: when 0, adds a random yaw to mTargetAngleY and reloads rnd(15)+15; action_itai sets 3 and runs alpha_anime only when it is 0 |
| 0x49C | `m380` | `mAnmHoldTimer` | action_dousa mode 8 sets 2 every frame, action_itai clears; daPW_Execute 0244B3E4 plays the body animation only while it is 0 |
| 0x49E | `m382` | `mLanternSettleTimer` | action_dousa sets 5 / 3 after creating the lantern (fopAcM_create 0xC1); daPW_Execute 0244B3E4 runs kantera_atari_check and kantera_calc 0244F730 only while 0, body_atari_check 0244E960 ignores light while set |
| 0x4A6 | `m38A` | `mSilentVanish` | action_torituku 0244FE24 sets 1 when the possession ends (light, button or hit); action_itai mode 60/61 then skips the death sound/voice and fopAcM_createDisappear |
| 0x4A8 | `m38C` | `mTargetAngleY` | daPW_Execute 0244B3E4 turns current.angle.y toward it; set by hani_check 0244B058 (atan2 home), Line_check, Big_pow_gattai_check 0244A5A8, action_dousa wander |
| 0x4AA | `m38E` | `mAlphaPhase` | alpha_anime 0244A9E4: +0x400 per frame, the sine of it pulses mAlpha around mAlphaBase |
| 0x4AC | `m390` | `mBobPhase` | fuwafuwa_calc 0244AA98: +700 per frame, the sine of it bobs the height; daPW_Create 0244E398 seeds it with cM_rndFX(0x4000) |
| 0x4AE | `m392` | `mCirclePhase` | fuwafuwa_calc 0244AA98 mode 0x26: +1000 per frame, the sine of it circles around mCircleCenter |
| 0x4B4 | `m398` | `mLanternHealth` | daPW_Create 0244E398 sets 4; kantera_atari_check 0244F28C subtracts per hit; action_itai breaks the lantern (kantera_break) at 0 |
| 0x4B6 | `m39A` | `mAlpha` | alpha_anime 0244A9E4 writes it; daPW_Draw 02449F80 uses it as the konst alpha; action_itai raises it to 255 (fully visible) under light |
| 0x4B8 | `m39C` | `mAlphaBase` | alpha_anime 0244A9E4 eases it to 100 and pulses mAlpha around it; action_itai sets 200 / 0 |
| 0x4BA | `m39E` | `mBigDemoTurnStep` | daPW_Create 0244E398 sets 0x1000; action_big_demo 024500C0 eases/sets it; daPW_Execute 0244B3E4 uses it as the turn step in PW_ACTION_BIG_DEMO |
| 0x4C0 | `m3A4` | `mDrawOffsetY` | draw_SUB 02449DA8 adds it to the model's y; action_dousa mode 1 sets -80 and eases it to 0 (the appear) |
| 0x4C8 | `m3AC` | `mGiveUpRange` | daPW_Create 0244E398: mNoticeRange + 5000; action_dousa mode 8 waits until Link is within it |
| 0x4CC | `m3B0` | `mLanternSwingAmpX` | kantera_calc 0244F730: eased to 5000 / 10000 by mLanternSwingMode; sine(mLanternSwingPhase) * it is the lantern joint's x rotation |
| 0x6E0 | `m5C4` | `mAttackEcallBack` | action_kougeki (in daPW_Execute 0244B3E4) mode 110 starts particle 0x8445 into it at the jump attack; mode 111 ends it on landing; daPW_Delete 0244DD6C ends it |

Open (no name yet): `m32C[2]` (no use), `m384[3]` (tww d217e2b's `m384[4]` block, whose fourth element is our `mSilentVanish`; [0] has two meanings: the sway phase while possessing Link in action_torituku 0244FE24, a wait timer in action_big_demo 024500C0; [1] / [2] are only cleared, with all four, through the helper `pwM384At`, was `pwDamageTimer` (misnamed)), and the padding arrays `_3D4`, `_459`, `_468`, `_476`, `_486`, `_4D0`.

Corrected existing names (our own names, not tww/TP: tww d217e2b covers these bytes with its padding arrays `m392` and `m3B4`; separate commit):

| WWHD offset | Old (misnamed) | New | Evidence |
|---|---|---|---|
| 0x4B0 | `mFloatPhase` | `mLanternSwingPhase` | kantera_calc 0244F730 adds mLanternSwingSpeed per frame; its sine/cosine set the lantern joint's x/z rotation (the lantern swing), not the float |
| 0x4B2 | `mFloatPhaseStep` | `mLanternSwingSpeed` | kantera_calc 0244F730 eases it to 3000 / 4000 by mLanternSwingMode and adds it to mLanternSwingPhase each frame |
| 0x4D4 | `mLightRadius` | `mLanternSwingAmpZ` | kantera_calc 0244F730 eases it to -1000 / -10000 by mLanternSwingMode; cos(mLanternSwingPhase) * it is the lantern joint's z rotation, not a light radius |

## tn_class (Darknut; d_a_tn.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_tn.h) keeps these GameCube offset names and TP has no tn_class, so every name below is from the WWHD use; where the tww code is cited it is the same field of the same class. `m1520` (WWHD 0x1668) is GameCube offset 0x1518, which tww names `m1518`; our old name was off by 8.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03DD | `m02C1` | `mWaitSpawnSwitch` | daTn_Create 024C0B0C: mEnableSpawnSwitch + 1 (0 = none); executePrefix (in daTn_Execute 024BD170) waits until that switch is on, then clears it; daTn_Draw 024BA238 draws nothing while it is set |
| 0x03F4 | `m02DC` | `mPartsAttached` | daTn_Create 024C0B0C sets 3; part_move 024BC3EC subtracts 1 each time an armour part (helmet, armour, shield) falls off |
| 0x03F5 | `m02DD` | `mPartDetachFlags` | applyArmoredHit (in daTn_Execute 024BD170) ORs 1 / 2 for the part that was hit off; part_move 024BC3EC detaches the part whose bit is set; executePrefix clears it |
| 0x04F0 | `m03D8` | `mFrameCount` | executePrefix (in daTn_Execute 024BD170) adds 1 every frame; tn_combatDefensiveReaction (in Tn_move 024C3E64) and yogan_fail 024C2BEC act only every 4th frame (& 3) |
| 0x0502 | `m03EA` | `mWeponRunTimer` | wepon_search 024C1A3C sets l_tnHIO.mWeponRunTime when the Darknut starts running to a dropped weapon and gives up (mWeponSearchGaveUp) when it reaches 0; counted down by executePrefix (in daTn_Execute 024BD170) |
| 0x0504 | `m03EC` | `mAnmLock` | anm_init 024B91E4, tate_anm_init 024B931C and yoroi_anm_init 024B93E8 return at once while it is set, and tn_moveCombat (in Tn_move 024C3E64) skips the turn toward mTargetAngle; nothing sets it (same in tww d217e2b) |
| 0x0506 | `m03EE` | `mWeponTurnWait` | wepon_search 024C1A3C sets 5 and turns toward the weapon only once it is 0; counted down in updateDemoCameraAndSmoke (in daTn_Execute 024BD170) |
| 0x0508 | `m03F0` | `mNoDamageTimer` | prepareDamage (in daTn_Execute 024BD170) returns at once while it runs and sets REG0_S(7) + 5 after a hit; applyArmoredHit sets 20; updateCollisionShapes moves the head sphere away while it runs; counted down by executePrefix |
| 0x050A | `m03F2` | `mNoGuardTimer` | applyOrdinaryDamage (in daTn_Execute 024BD170) sets 25; tn_combatDefensiveReaction (in Tn_move 024C3E64) does not guard while it runs; counted down by executePrefix |
| 0x0514 | `m03FC` | `mWeponSearchWait` | wepon_search 024C1A3C sets 70 when it runs into a wall; search_wepon 024BAFAC finds no weapon while it runs; counted down by executePrefix (in daTn_Execute 024BD170) |
| 0x0516 | `m03FE` | `mObstacleSide` | tn_combatObstacles (in Tn_move 024C3E64) stores the ground_4_check result (2 / 4 / 8 = side blocked); tn_combatLocomotion picks the sidestep from it |
| 0x0530 | `m0414` | `mHeadAngleY` | nodeCallBack 024B97A0: atan2 from the eye toward a point ahead of the head joint; search_wepon 024BAFAC compares weapon directions with it (view cone) |
| 0x0534 | `m0418` | `mSideStepAngle` | tn_combatLocomotion / tn_attackCircle (in Tn_move 024C3E64) set +-0x4000; tn_finishMovement adds it to the move heading; Tn_move 024C3E64 clears it |
| 0x0536 | `m041A` | `mTubaEffectTimer` | updateDamageAnimation / updateKnockback (in daTn_Execute 024BD170) set 5 / 3 / 1; updateCollisionShapes counts it down and at 0 sets particle 0x0E at mHeadPos with mEffectAngle (tww d217e2b calls it dPa_name::ID_AK_JN_TUBA00) |
| 0x0538 | `m041C` | `mSmokePos` | updateDemoCameraAndSmoke (in daTn_Execute 024BD170) puts it on the smoke ring around the body (MtxPosition) or 25000 above mSpawnY when idle; smoke_set_s 024B94C0 drops it to the ground and emits the smoke there |
| 0x0544 | `m0428` | `mEffectAngle` | updateDemoCameraAndSmoke (in daTn_Execute 024BD170) sets y = mSmokeAngle for the ground smoke; updateCollisionShapes sets y = angle.y for particle 0x0E at mHeadPos; updateDamageAnimation / updateKnockback set x (-0x4000 / 0) |
| 0x054A | `m042E` | `mSmokeAngle` | tn_attackAnimation (in Tn_move 024C3E64) seeds it from the heading; updateDemoCameraAndSmoke (in daTn_Execute 024BD170) rotates mSmokePos by it and adds a step each frame |
| 0x054C | `m0430` | `mGroundSmokeTimer` | tn_attackAnimation (in Tn_move 024C3E64) and updateDamageAnimation (in daTn_Execute 024BD170) set it (l_tnHIO.mGroundSmokeEnd + 16 ...); updateDemoCameraAndSmoke emits ground smoke while it counts down; applyIceOrShatter clears it |
| 0x054E | `m0432` | `mGroundSmokeType` | tn_attackAnimation (in Tn_move 024C3E64) sets 0 (attack ring, radius 200), updateDamageAnimation (in daTn_Execute 024BD170) 1 (fall, radius 55); updateDemoCameraAndSmoke picks the smoke ring by it |
| 0x054F | `m0433` | `mSmokeCount` | smoke_set_s 024B94C0 counts the smoke puffs (the first one differs); updateDemoCameraAndSmoke (in daTn_Execute 024BD170) clears it when the ground smoke ends |
| 0x0D84 | `m0C34` | `mHasWepon` | tn_moveReturnHome / tn_movePatrol / path_check use it as "holding a weapon" (armed patrol, BCK 0x39 vs 0x3D); updateHeldWeapon (in daTn_Execute 024BD170) clears it when the weapon drops (tww d217e2b yari_off_check) |
| 0x0D88 | `m0C38` | `mWeponDropMode` | updateDamageAnimation (in daTn_Execute 024BD170) and yogan_fail 024C2BEC set 1 (throw the weapon), applyIceOrShatter 2 (drop it in place); updateHeldWeapon releases the weapon by it and clears it with mHasWepon |
| 0x0D8C | `m0C3C` | `mRightItemPos` | nodeCallBack_P 024B9C1C: MtxPosition of reaction joint 20 (model joint 23, J_TN_ITEM_R1: joint_check at 101D2558 and tww d217e2b joint_check / Tn.h), the hand that holds the weapon; never read |
| 0x0D98 | `m0C48` | `mAttackType` | tn_chooseAttack (in Tn_move 024C3E64) sets 0..3 (and 4); tn_attackAnimation and wepon_hit_check 024BBB3C pick the swing animation and the hit frame window by it |
| 0x0DB6 | `m0C66` | `mAttackState` | tn_chooseAttack (in Tn_move 024C3E64) sets 1 at an attack start; tn_attackAnimation sets -1 when Link's guard bounces it; wepon_hit_check 024BBB3C does no hit while it is < 0 |
| 0x0DB7 | `m0C67` | `mKickLunge` | tn_moveAttack (in Tn_move 024C3E64) sets 1 for the kick; tn_attackAnimation moves forward (speed 80) during it; tn_chooseAttack clears it |
| 0x14D4 | `m1384` | `mHeadPos` | nodeCallBack 024B97A0: MtxPosition of the head joint; updateCollisionShapes (in daTn_Execute 024BD170) sets particle 0x0E there; yogan_fail 024C2BEC emits particle 0x0E there |
| 0x14E0 | `m1390` | `mRightItemFrontPos` | nodeCallBack_P 024B9C1C: MtxPosition of offset x = 165 on reaction joint 20 (J_TN_ITEM_R1, see mRightItemPos); never read |
| 0x14EC | `m139C` | `mRightItemBackPos` | nodeCallBack_P 024B9C1C: MtxPosition of offset x = -50 on reaction joint 20 (J_TN_ITEM_R1, see mRightItemPos); never read |
| 0x14F8 | `m13A8` | `mLeftArmPos` | nodeCallBack_P 024B9C1C: MtxPosition of offset (17.5, -8.75, 0) on reaction joint 17 (model joint 14, J_TN_UDE_L3: joint_check at 101D2558 and tww d217e2b joint_check / Tn.h); never read |
| 0x1504 | `m13B4` | `mRightArmPos` | nodeCallBack_P 024B9C1C: MtxPosition of reaction joint 16 (model joint 22, J_TN_UDE_R3: joint_check at 101D2558 and tww d217e2b joint_check / Tn.h); never read |
| 0x1528 | `m13D8` | `mPrevWeponSphMove` | wepon_hit_check 024BBB3C copies mWeponSphMove here before it computes the new one |
| 0x1534 | `m13E4` | `mDefenceSphPos` | prepareDamage (in daTn_Execute 024BD170): MtxPosition of the body joint; the centre of mDefenceSph |
| 0x154D | `m13FD` | `mWeponAtStarted` | wepon_hit_check 024BBB3C starts the weapon spheres (StartCAt) while 0 and sets 1, then only moves them; tn_chooseAttack (in Tn_move 024C3E64) clears it at a new attack |
| 0x154F | `m13FF` | `mWeponSearchGaveUp` | tn_movePatrol (in Tn_move 024C3E64) sets 1 when mWeponRunTimer ran out without the weapon; the patrol then follows the path as if armed (mHasWepon \|\| it); wepon_search 024C1A3C clears it |
| 0x1550 | `m1400` | `mItemStolen` | applyOrdinaryDamage (in daTn_Execute 024BD170) sets 1 after a steal hit (kind 14) and passes it as stealItemBitNo; daTn_Draw 024BA238 hides shape 0 while it is set |
| 0x1552 | `m1402` | `mArmorShakeTimer` | applyArmoredHit (in daTn_Execute 024BD170) sets 15; part_move 024BC3EC wobbles the helmet part by it; counted down by executePrefix |
| 0x1558 | `m1408` | `mLookTargetId` | aite_miru 024C29CC looks at the actor with this process id while it waits and resets it to -1 at the end; updateDamageAnimation (in daTn_Execute 024BD170) sets -1 |
| 0x155C | `m140C` | `mWeponCarryPending` | daTn_Create 024C0B0C sets 1; executePrefix (in daTn_Execute 024BD170) clears it once the weapon actor exists (and puts it in the hand); updateHeldWeapon keeps mHasWepon only while it is 0 or the weapon is held |
| 0x1568 | `m1418` | `mpBomb` | b_nige 024C1ED4 stores search_bomb's bomb; the run-away move flees from it |
| 0x156C | `m141C` | `mHitTypeFlags` | applyOrdinaryDamage (in daTn_Execute 024BD170) ORs the flags of every hit in; search_bomb 024BAB8C looks for bombs only once bit 0x200 is set |
| 0x1574 | `m1424` | `mPathNo` | daTn_Create 024C0B0C and path_check 024BB800: mPathIndex + 1 (0 = no path); tn_advancePatrolTarget / tn_patrolLocomotion (in Tn_move 024C3E64) follow the path only when it is set |
| 0x1575 | `m1425` | `mPathPoint` | path_check 024BB800 picks the nearest point; tn_advancePatrolTarget (in Tn_move 024C3E64) steps it by mPathDir and wraps; tn_currentPatrolPoint reads it |
| 0x1576 | `m1426` | `mPathDir` | daTn_Create 024C0B0C sets 1; tn_advancePatrolTarget (in Tn_move 024C3E64) flips it to -1 (0xFF) / 1 at the path ends; path_check 024BB800 offsets the start point by it |
| 0x1582 | `m1433` | `mHioSet` | daTn_Create 024C0B0C sets 1 when it created the l_tnHIO child; daTn_Delete 024C0194 deletes the child only then |
| 0x1583 | `m1434` | `mMahiDelay` | applyOrdinaryDamage (in daTn_Execute 024BD170) sets REG0_S(3) + 8 at a kind-10 hit, else 0; tn_updateMovementTarget (in Tn_move 024C3E64) counts it down and starts ACTION_D_MAHI at 0 |
| 0x1584 | `m1436` | `mMoveTargetTimer` | tn_updateMovementTarget (in Tn_move 024C3E64) counts it down and moves toward mMoveTargetPos instead of the player while it runs; nothing sets it (same in tww d217e2b) |
| 0x1588 | `m1438` | `mMoveTargetPos` | tn_updateMovementTarget (in Tn_move 024C3E64) uses it as the move target while mMoveTargetTimer runs; nothing sets it (same in tww d217e2b) |
| 0x1594 | `m1444` | `mDefenceSphOn` | defence 024C2430 sets 1; prepareDamage (in daTn_Execute 024BD170) then places mDefenceSph at mDefenceSphPos and counts it down |
| 0x159C | `m144C` | `mCutTurnEcallBack` | tn_class_construct 024C0924 constructs the 3 callbacks; finishNormalEffects (in daTn_Execute 024BD170) drives the spin-attack trails with them; daTn_Delete 024C0194 ends them |
| 0x15CC | `m147C` | `mCutTurnPoints` | finishNormalEffects (in daTn_Execute 024BD170) fills the 10 trail points around the sword and hands them to mCutTurnEcallBack (mpPoints) |
| 0x1644 | `m14F4` | `mCutTurnStep` | tn_moveAttack (in Tn_move 024C3E64) sets 1 at a spin attack; finishNormalEffects (in daTn_Execute 024BD170) draws the trails while it is set, steps it, clears it when mCutTurnTimer ends |
| 0x1646 | `m14F6` | `mCutTurnTimer` | tn_moveAttack (in Tn_move 024C3E64) sets 7 / 3; finishNormalEffects (in daTn_Execute 024BD170) counts it down, picks the trail colours by it (>= 5) and ends the trails at 0 |
| 0x1648 | `m14F8` | `mDemoMode` | updateKnockback (in daTn_Execute 024BD170) sets 50 (death demo request, stops the sub BGM); updateDemoCameraAndSmoke sets 51 when the event runs and drives the demo camera; executePrefix suppresses attention while set; fail 024C2AAC checks it |
| 0x164A | `m14FA` | `mDemoTimer` | updateDemoCameraAndSmoke (in daTn_Execute 024BD170): cleared when the death demo starts, +1 per demo frame |
| 0x1664 | `m1514` | `mDemoCamFovy` | updateDemoCameraAndSmoke (in daTn_Execute 024BD170): 55 at the demo start, eased to 30; passed as the fovy of the demo camera set |
| 0x1668 | `m1520` | `mNoDraw` | fail 024C2AAC sets 1; daTn_Draw 024BA238 draws nothing while it is set; updateDemoCameraAndSmoke (in daTn_Execute 024BD170) stops moving the camera centre (GameCube offset 0x1518: tww d217e2b names it m1518) |
| 0x1669 | `m1519` | `mDemoSide` | s_demo 024C3348 sets 1 / 0 for the two start positions; picks demo mode 13 / 12 from it |
| 0x166C | `m151C` | `mReturnAnmMorf` | tn_moveReturnHome (in Tn_move 024C3E64) uses it as the morf of the return animations; daTn_Create 024C0B0C sets 1 for the 3 create-time executes, then 10 |

Open (no name yet): `m13FC` (wepon_hit_check 024BBB3C only clears it), `m1404` (aite_miru 024C29CC eases it to 12000, Tn_move 024C3E64 back to 0; never read, so the meaning of the angle is not known), `m1420` (tn_combatLocomotion in Tn_move 024C3E64 only clears it).

### tnHIO_c (the Darknut's HIO tuning values, l_tnHIO; d_a_tn.h)

zeldaret/tww (d217e2b, src/d/actor/d_a_tn.cpp) keeps the GameCube offset names. Every named field is set only by the HIO constructor (WWHD tn_staticInit 024C161C, tww tnHIO_c::tnHIO_c) and read in the places given; nothing else writes it in WWHD or tww.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x006 | `m006` | `mPadStep` | written only by the HIO constructor tn_staticInit 024C161C (0); read only in tww daTn_Execute: while it is not 0 the Darknut's frame (timers and move) runs only on a pad B trigger or Y hold (frame-step debugging) |
| 0x007 | `m007` | `mDebugHealth` | written only by the HIO constructor tn_staticInit 024C161C (0); read only in tww damage_check: after a hit 1 sets health 10, 2 sets health 0 |
| 0x018 | `m018` | `mDropOffsetY` | fail 024C2AAC: the fopAcM_createDisappear drop point is current.pos.y + it + 100 |
| 0x01E | `m01E` | `mSmokeAlpha` | smoke_set_s 024B94C0: alpha of the ground-smoke particle 0x2022 |
| 0x020 | `m020` | `mGroundSmokeEnd` | tn_attackAnimation (in Tn_move 024C3E64) / updateDamageAnimation (in daTn_Execute 024BD170) load mGroundSmokeTimer with it + 14 / + 16; updateDemoCameraAndSmoke (in daTn_Execute) sets the smoke only while the timer is still >= it |
| 0x024 | `m024` | `mAttentionOffsetY` | nodeCallBack 024B97A0: attention_info.position.y = eyePos.y + it |
| 0x028 | `m028` | `mNoticeRange` | tn_movePatrol (in Tn_move 024C3E64): the patrol checks the player view only while mPlayerDistance < it |
| 0x02C | `m02C` | `mFightRange` | tn_combatLocomotion / tn_combatRangeTransition / tn_finishAttack (in Tn_move 024C3E64), hukki 024C25F8: within it the fight run goes to the fight stance (mode 2), beyond it + 75 the Darknut runs again |
| 0x030 | `m030` | `mAttackRange` | tn_combatLocomotion / tn_combatRangeTransition / tn_attackCircle (in Tn_move 024C3E64): an attack starts at mPlayerDistance < it (and within it +- 62.5 / 25) |
| 0x034 | `m034` | `mViewAngle` | get_view_H 024BAA68: the view half-angle for daTn_player_view_check / search_wepon without the helmet |
| 0x036 | `m036` | `mViewAngleHelmet` | get_view_H 024BAA68: the view half-angle while the helmet is on (EQUIPMENT_PIECE_HELMET) |
| 0x038 | `m038` | `mViewHeight` | daTn_player_view_check 024BB404, search_wepon 024BAFAC: no sight when the height difference to the target is more than it |
| 0x03C | `m03C` | `mViewBoxX` | daTn_player_view_check 024BB404, search_wepon 024BAFAC: sight box, /local x/ < it |
| 0x040 | `m040` | `mViewBoxY` | daTn_player_view_check 024BB404, search_wepon 024BAFAC: sight box, /local y/ < it |
| 0x044 | `m044` | `mViewBoxFront` | daTn_player_view_check 024BB404, search_wepon 024BAFAC, search_bomb 024BAB8C: sight box, local z < it |
| 0x048 | `m048` | `mViewBoxBack` | daTn_player_view_check 024BB404, search_wepon 024BAFAC, search_bomb 024BAB8C: sight box, local z > it (-125) |
| 0x04C | `m04C` | `mPathWalkSpeed` | tn_patrolLocomotion (in Tn_move 024C3E64): patrol speed along the path; tn_combatLocomotion: * 1.5 is the fight-run speed with the body armor |
| 0x050 | `m050` | `mWalkSpeed` | tn_patrolLocomotion (in Tn_move 024C3E64): patrol speed without a path |
| 0x054 | `m054` | `mRunSpeed` | tn_combatLocomotion (in Tn_move 024C3E64): fight-run speed without the body armor; tn_moveReturnHome: run speed back home |
| 0x058 | `m058` | `mWeponRunSpeed` | wepon_search 024C1A3C: speed while running to the dropped weapon; b_nige 024C1ED4: speed while fleeing a bomb |
| 0x05C | `m05C` | `mFightStepSpeed` | tn_combatLocomotion / tn_attackCircle (in Tn_move 024C3E64): speedF target (+ / -) of the fight-stance steps forward / back |
| 0x060 | `m060` | `mSideStepSpeed` | tn_combatLocomotion / tn_attackCircle (in Tn_move 024C3E64): speedF target of the side steps (mSideStepAngle 0x4000 / 0xC000) |
| 0x06C | `m06C` | `mAttackCheckDelay` | tn_combatRangeTransition (in Tn_move 024C3E64): mCountDownTimers[3] reload after each attack roll |
| 0x070 | `m070` | `mAttackChance` | tn_combatRangeTransition (in Tn_move 024C3E64): the attack starts only when cM_rndF(100) < it |
| 0x078 | `m078` | `mVerticalCutChance` | tn_chooseAttack (in Tn_move 024C3E64): with the weapon, cM_rndF(100) < it picks the vertical cut (YAT_TATE1, mAttackType 0), else the side cut (YAT_YOKO1, 1) |
| 0x07C | `m07C` | `mPalmStrikeChance` | tn_chooseAttack (in Tn_move 024C3E64): without the weapon, cM_rndF(100) < it (or the body armor on) picks the palm strike BAT_SYOUTEI_S1 (mAttackType 2) |
| 0x080 | `m080` | `mDownRecoverDelay` | updateDamageAnimation (in daTn_Execute 024BD170): mDamageReaction.mDownTimers[2] (the recovery delay) for the reactions 20 / 21 |
| 0x082 | `m082` | `mWeponRunTime` | wepon_search 024C1A3C: mWeponRunTimer at the start of the run to the dropped weapon |
| 0x0D0 | `m0D0` | `mWeakHitPushback` | updateKnockback (in daTn_Execute 024BD170): mDamageReaction.mPushback = -it for a hit with mKnockbackPower < 25 |
| 0x0D4 | `m0D4` | `mWeponHitMaxDis` | tn_attackAnimation (in Tn_move 024C3E64): copied to mBtMaxDis for every attack |

Open (no name yet): the constructor-only fields `m005`, `m008`, `m00A`, `m00C`, `m014`, `m01C`, `m064`, `m068`, `m074`, `m084`..`m0C0` (f32), `m0C4`..`m0CE` (s16) (no read in WWHD or tww), and the padding `m009`, `m010`, `m022`, `m06E`. `TnPlayer::field_0x2b0` is a view of the player class (on hold) and `TnBoko::m2C2` takes daBoko_c's `mRotAngleSpeed` (docs/field-names/objects.md).

## mo2_class (Moblin; d_a_mo2.h, d_a_mo2_main.h, d_a_mo2_exec.h, d_a_mo2_move.h)

The class is declared in four headers (one per source split); all four copies are renamed together and carry the same comments. zeldaret/tww (d217e2b, include/d/actor/d_a_mo2.h) keeps these GameCube offset names and TP has no mo2_class, so every name below is from the WWHD use; tww d217e2b's d_a_mo2.cpp (same class) is cited only for helper names (ke_control / ke_disp). `m05C0_dy_hd` is an HD-only member next to `m05C0` and is renamed with it.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03DD | `m02C1` | `mAppearWait` | daMo2_Create 021CCB00: mEnableSpawnSwitch + 1 (0 = none; cleared for the statue mode 0xF); daMo2_Execute 021C9B1C waits until that switch is on, then clears it and sets the lock-on flag; daMo2_Draw 021C7344 draws nothing while it is set |
| 0x03E4 | `m02C8` | `mEyeBtpAnm` | tex_anm_set 021C6368: mDoExt_btpAnm_init with the BTP mo2_tex_anm_idx[idx]; createHeap 021CC524 inits it with BTP 3; daMo2_Draw 021C7344 enters it at mEyeBtpFrame; stepped by mo2_eye_tex_anm (in daMo2_Execute 021C9B1C) |
| 0x0458 | `m02DC` | `mEyeBtpFrame` | tex_anm_set 021C6368 clears it; mo2_eye_tex_anm (in daMo2_Execute 021C9B1C) adds 1 up to mEyeBtpMaxFrame; daMo2_Draw 021C7344 enters mEyeBtpAnm at this frame |
| 0x0459 | `m02DD` | `mEyeBtpPlaying` | tex_anm_set 021C6368 sets 1; mo2_eye_tex_anm (in daMo2_Execute 021C9B1C) clears it at mEyeBtpMaxFrame and starts BTP 3 only while it is 0 |
| 0x045A | `m02DE` | `mEyeBtpMaxFrame` | tex_anm_set 021C6368: mo2_tex_max_frame[idx]; mo2_eye_tex_anm (in daMo2_Execute 021C9B1C) stops the eye BTP there |
| 0x045C | `m02E0` | `mEyeAnmWait` | mo2_eye_tex_anm (in daMo2_Execute 021C9B1C) counts it down, then reloads rnd(50)+10 and starts BTP 3 unless one is playing; nage 021CD958 / p_lost 021CDE6C / defence 021CE46C / Mo2_move 021D0A14 hold it at 2 |
| 0x045E | `m02E2` | `mWeaponViewWait` | wepon_search 021CF0F4 sets 70 when it gives up the run to the weapon; daMo2_Execute 021C9B1C counts it down; daMo2_wepon_view_check 021C872C finds no weapon while it is not 0 |
| 0x0460 | `m02E8` | `mLanternId` | kantera_get_init (in daMo2_Create 021CCB00): fpcSCtRq_Request(fpcNm_KANTERA_e) result, then fopAcM_SearchByID(it) to give the lantern this actor's id |
| 0x0830 | `m058C` | `mpBlurModel` | createHeap 021CC524: mDoExt_J3DModel__create of the spear-trail BMD (HD index 0x7E); br_draw (in daMo2_Draw 021C7344) places it on joint MO_JNT_MO_YARI_e and draws it |
| 0x0834 | `m0590` | `mBlurScaleX` | fight (in Mo2_move 021D0A14): moveHIO mAttack1BlurScaleX[] / mAttack2BlurScaleX[] by the attack frame; br_draw (in daMo2_Draw 021C7344) scales the blur model x by l_mo2HIO.mBlurScale * it |
| 0x0838 | `m0594` | `mBlurOn` | daMo2_Execute 021C9B1C clears it each frame; fight (in Mo2_move 021D0A14) sets 1 inside the swing frames of attack kinds 1 / 2; br_draw (in daMo2_Draw 021C7344) draws only while it is set |
| 0x083C | `m0598` | `mBlurUseB` | daMo2_Execute 021C9B1C clears it each frame; fight (in Mo2_move 021D0A14) sets 1 for the middle swing frames; br_draw (in daMo2_Draw 021C7344) shows the blur model's shape 2 instead of 1 when set |
| 0x0840 | `m059C` | `mFrameCounter` | daMo2_Execute 021C9B1C adds 1 per frame; p_lost 021CDE6C / yogan_fail 021CEEB0 / hip_damage 021CF900 / ground_smoke_set (in daMo2_Execute) / fight_run (in Mo2_move 021D0A14) test its low bits |
| 0x0844 | `m05A0` | `mOshiCount` | fight_run (in Mo2_move 021D0A14) adds 1 per frame while the armed Moblin is close and sees the target, at 15 starts ACTION_OSHI (push) with 50 %, else clears it; damage_check (in daMo2_Execute 021C9B1C) clears it on a hit |
| 0x0848 | `m05A4` | `mTimers` | daMo2_Execute 021C9B1C counts all 5 down each frame; the actions load [0]..[4] (e.g. wait_set 021C8E30 [1], nage 021CD958 [0]/[1]/[2], attack_set 021C9310 [4]) |
| 0x0852 | `m05AE` | `mWeaponRunTimer` | wepon_search 021CF0F4 sets l_mo2HIO.mWeaponRunTime when the run to a weapon starts and gives up at 0; yari_off_check (in daMo2_Execute 021C9B1C) sets it when it spawns the dropped weapon actor; daMo2_Execute counts it down; jyunkai (in Mo2_move 021D0A14) sets mWeaponSearchGaveUp at 0 |
| 0x0854 | `m05B0` | `mDashStopTimer` | fight_run (in Mo2_move 021D0A14) sets rnd(10)+15 when a dash longer than 20 frames stops (BCK GAKEDEMO) and waits for it; anm_init 021C6440 sets no new animation while it is not 0; daMo2_Execute 021C9B1C counts it down and keeps smoke type 4 going |
| 0x0856 | `m05B2` | `mAnmStopTimer` | wepon_search 021CF0F4 sets 5 when it sees a weapon and turns only once it is 0; daMo2_Execute 021C9B1C counts it down and plays mpMorf only while it is 0 |
| 0x0858 | `m05B4` | `mNoHitTimer` | damage_check (in daMo2_Execute 021C9B1C) takes a Tg hit only while it is 0 and reloads it (4, 10, 0x32, 200); fail 021CED78 / yogan_fail 021CEEB0 / d_sit 021CFFA4 / d_dozou 021D0064 / e3_demo 021D0570 / Mo2_move 021D0A14 hold it at 5; daMo2_Execute moves the head sphere 20000 down while it is not 0 |
| 0x085A | `m05B6` | `mNoGuardTimer` | damage_check (in daMo2_Execute 021C9B1C) sets 25 after a hit; daMo2_Execute counts it down; fight_run (in Mo2_move 021D0A14) guards (ACTION_DEFENCE) only while it is 0 |
| 0x085E | `m05BA` | `mSideStepTimer` | fight_run (in Mo2_move 021D0A14) sets 0x10 / 0x20 when a side step starts (and fight on a bounce); side-step reaction (dr.m710 3/4) while it is not 0; daMo2_Execute 021C9B1C counts it down |
| 0x0860 | `m05BC` | `mSideStepWait` | fight_run (in Mo2_move 021D0A14) reloads rnd(100)+30 when it is 0 and then picks the next side step; daMo2_Execute 021C9B1C counts it down |
| 0x0862 | `m05BE` | `mSideStepDir` | fight_run (in Mo2_move 021D0A14): 0 / 1 / 2 by mGroundCheck 4 / 8 / 2 when a side step starts; selects dr.m710 3 / 4 while mSideStepTimer runs |
| 0x0863 | `m05BF` | `mGroundCheck` | fight_run (in Mo2_move 021D0A14): ground_4_check 021C7EDC result (4 directions); 2 / 4 / 8 select the side step and the move-angle offset |
| 0x0864 | `m05C0` | `mTargetDist` | Mo2_move 021D0A14: xz distance to the target (dr.m714 or mSoundTargetPos); nage 021CD958 / hukki 021CE6E8 / fight_run (in Mo2_move) compare it with the l_mo2HIO attack ranges |
| 0x0868 | `m05C0_dy_hd` | `mTargetDistY` | Mo2_move 021D0A14: \|dy\| to the target; fight_run (in Mo2_move) also needs it below REG_F(10,3) + 250 before an attack |
| 0x086C | `m05C4` | `mPathTargetPos` | jyunkai (in Mo2_move 021D0A14) copies the current path point there (way_pos_check 021C7C30 adjusts it) and walks toward it; path_check2 (in Mo2_move) line-checks to it |
| 0x087C | `m05D4` | `mHeadAngleY` | nodeCallBack 021C6854: atan2 of the face joint (MO_JNT_KAO_J_e) direction (or angle.y); search_bomb 021C8378 / daMo2_player_view_check 021C8C14 measure the sight cone from it |
| 0x087E | `m05D6` | `mTargetAngleY` | Mo2_move 021D0A14: cM_atan2s to the target; nage 021CD958 / fight_run (in Mo2_move) / carry_drop turn to it |
| 0x0880 | `m05D8` | `mMoveAngleOffset` | fight_run (in Mo2_move 021D0A14) sets +-0x4000 for side steps; Mo2_move 021D0A14 clears it and moves along angle.y + it |
| 0x0882 | `m05DA` | `mSpitTimer` | damage_check / daMo2_Execute 021C9B1C damage cases set 1 / 3 / 5 / 10; daMo2_Execute counts it down and at 0 sets particle ID_AK_JN_TUBA00 at mSpitPos |
| 0x0884 | `m05DC` | `mSmokePos` | ground_smoke_set (in daMo2_Execute 021C9B1C) computes it (MtxPosition); smoke_set_s 021C6578 drops it to the ground and sets the ELEMENTSMOKE particle there |
| 0x0890 | `m05E8` | `mEffectAngle` | smoke_set_s 021C6578 / ground_smoke_set (in daMo2_Execute 021C9B1C): smoke particle angle; daMo2_Execute / yogan_fail 021CEEB0: TUBA00 particle angle |
| 0x0896 | `m05EE` | `mSmokeAngleY` | ground_smoke_set (in daMo2_Execute 021C9B1C) rotates the smoke offset by it and adds 2000 / 0x1FA0 / 0x2000 per call; fight (in Mo2_move 021D0A14) seeds it from angle.y |
| 0x0898 | `m05F0` | `mSmokeTimer` | ground_smoke_set (in daMo2_Execute 021C9B1C) counts it down and sets smoke while it is >= l_mo2HIO.mSmokeTimerBase; b_nige 021CE138 / hukki 021CE6E8 / wepon_search 021CF0F4 / fight load mSmokeTimerBase + n |
| 0x089A | `m05F2` | `mSmokeType` | ground_smoke_set (in daMo2_Execute 021C9B1C): switch 0..4 selects where the smoke comes from; b_nige 021CE138 sets 4, hukki 021CE6E8 3, daMo2_Execute 1 |
| 0x089B | `m05F3` | `mSmokeCount` | ground_smoke_set (in daMo2_Execute 021C9B1C) clears it; smoke_set_s 021C6578 adds 1 per smoke set and tests it with the ground attribute |
| 0x089C | `m05F4` | `mSmokeCallBack` | mo2_class_ct 021CC91C: dPa_smokeEcallBack_ct; smoke_set_s 021C6578 passes it to the smoke emitter; daMo2_Delete 021CC44C removes it |
| 0x10B4 | `m0DD8` | `mSpearHair` | nodeCallBack_P 021C6BE0 places the 16 strand roots on joint MO_JNT_MO_YARI_e (spear); ke_control 021C6F14 / ke_disp (in daMo2_Draw 021C7344) move and draw the strands (ke_s, tww d217e2b ke_control / ke_disp) |
| 0x243C | `m2034` | `mSpearJointPos` | nodeCallBack_P 021C6BE0: origin of joint MO_JNT_MO_YARI_e (joint_check 0x14, the spear joint, as for mSpearShaftPos); never read |
| 0x2448 | `m2040` | `mSpearEndPos` | nodeCallBack_P 021C6BE0: [0] point x 165, [1] point x -130 on joint MO_JNT_MO_YARI_e (the spear's two ends, beside mSpearShaftPos at x -50); never read |
| 0x2468 | `m2060` | `mAttackKind` | attack_set 021C9310 sets 0..5; indexes attack_info / mo2_attack_ready_SE and the move attack tables (fight, yari_hit_check in Mo2_move 021D0A14) |
| 0x246C | `m2064` | `mAttackStep` | attack_set 021C9310 clears it; fight (in Mo2_move 021D0A14) steps +1 / -1 through attack_info[mAttackKind][it] animations; hukki 021CE6E8 sets 1 |
| 0x2470 | `m2068` | `mAttackFrame` | attack_set 021C9310 clears it; fight (in Mo2_move 021D0A14) adds the step speed; compared with mAtStartFrame / mAtEndFrame, the blur frames and mTurnEndFrame |
| 0x2474 | `m206C` | `mAtStartFrame` | attack_set 021C9310 sets 3..36 per kind; yari_hit_check (in Mo2_move 021D0A14): no At before it, attack SE at it |
| 0x2478 | `m2070` | `mAtEndFrame` | attack_set 021C9310 sets 8..52 per kind; yari_hit_check (in Mo2_move 021D0A14): no At after it |
| 0x247C | `m2074` | `mLookStartFrame` | attack_set 021C9310 sets 0 / 10 / 59 / 75 per attack kind (hukki 021CE6E8 0, wepon_search 021CF0F4 75); fight (in Mo2_move 021D0A14) sets mDamageReaction.mLookMode = 1 (head toward mpLookTarget) once mAttackFrame is past it and the target is in view |
| 0x2480 | `m2078` | `mTurnEndFrame` | attack_set 021C9310: l_mo2HIO mAttack0TurnEndFrame..mAttack5TurnEndFrame per kind; fight (in Mo2_move 021D0A14) keeps turning to mTargetAngleY while mAttackFrame is below it |
| 0x2484 | `m207C` | `mAtWaitTimer` | yari_hit_check (in Mo2_move 021D0A14) counts it down and does no spear hit check while it runs; nothing sets it (same in tww d217e2b) |
| 0x2486 | `m207E` | `mAttackDir` | attack_set 021C9310 sets 1; fight (in Mo2_move 021D0A14) sets -1 when the player guards (plays back) and steps by it; yari_hit_check has no At while < 0 |
| 0x2CD0 | `m28C8` | `mSpitPos` | nodeCallBack 021C6854: point (20.75, 18.5) on the face joint MO_JNT_KAO_J_e; daMo2_Execute 021C9B1C / yogan_fail 021CEEB0 set particle ID_AK_JN_TUBA00 there |
| 0x2CDC | `m28D4` | `mSpearHeadPos` | nodeCallBack_P 021C6BE0 copies the first mSpearHair root (x 98 on MO_JNT_MO_YARI_e); yari_hit_check (in Mo2_move 021D0A14) uses it as mAtPos for spear kinds 0..2; fight plays LANCE_HIT_FLOOR there |
| 0x2CE8 | `m28E0` | `mSpearShaftPos` | nodeCallBack_P 021C6BE0: point x -50 on joint MO_JNT_MO_YARI_e; yari_hit_check (in Mo2_move 021D0A14) copies it to mAt2Pos, defence 021CE46C to mDefenseSphPos |
| 0x2CF4 | `m28EC` | `mLeftHandPos` | nodeCallBack_P 021C6BE0: point (17.5, -8.75) on joint MO_JNT_TEL_J_e (left hand); yari_hit_check (in Mo2_move 021D0A14) uses it as mAtPos for kinds 3 / 4 |
| 0x2D00 | `m28F8` | `mRightHandPos` | nodeCallBack_P 021C6BE0: origin of joint MO_JNT_TER_J_e (right hand); yari_hit_check (in Mo2_move 021D0A14) uses it as mAtPos for kind 5 |
| 0x2D0C | `m2904` | `mAtPos` | yari_hit_check (in Mo2_move 021D0A14): mRightHandPos / mLeftHandPos / mSpearHeadPos by mAttackKind; mWeaponSph starts / moves there; ground_smoke_set (in daMo2_Execute 021C9B1C) |
| 0x2D18 | `m2910` | `mAt2Pos` | yari_hit_check (in Mo2_move 021D0A14): mSpearShaftPos for the spear kinds; mWeapon2Sph starts / moves there |
| 0x2D24 | `m291C` | `mPrevAtPos` | yari_hit_check (in Mo2_move 021D0A14) copies mAtPos here before it picks the new one; never read |
| 0x2D30 | `m2928` | `mDefenseSphPos` | defence 021CE46C copies mSpearShaftPos; damage_check (in daMo2_Execute 021C9B1C) centres mDefenseSph on it, sets the KIKUZU particle there and moves it to y -10000 |
| 0x2D49 | `m2941` | `mAtStarted` | attack_set 021C9310 clears it; yari_hit_check (in Mo2_move 021D0A14) starts the At spheres while it is 0 and sets it, then only moves them |
| 0x2D4A | `m2942` | `mTurnAlways` | hukki 021CE6E8 sets 1 when it attacks out of getting up, attack_set 021C9310 clears it; fight (in Mo2_move 021D0A14) keeps turning to the target while it is set |
| 0x2D4B | `m2943` | `mWeaponSearchGaveUp` | wepon_search 021CF0F4 sets 1 when the run to a weapon ends at a cliff / wall / timeout and clears it at the start; jyunkai (in Mo2_move 021D0A14) sets it when mWeaponRunTimer ran out; has_weapon / wait_set 021C8E30 / path_check 021C9004 treat it as armed |
| 0x2D4C | `m2944` | `mBtCounter` | attack_set 021C9310 clears it for attack kinds 1 / 2 / 5; daMo2_Execute 021C9B1C adds 1 per frame up to 200 (then -1 = off) and opens the parry window at mBtWindow1Start / mBtWindow2Start; damage_check sets -1 |
| 0x2D4E | `m2946` | `mBtWindow1Start` | attack_set 021C9310: l_mo2HIO mAttack1BtWindow1Start / mAttack2BtWindow1Start / mAttack5BtWindow1Start; daMo2_Execute 021C9B1C loads mBtWindowTimer with mBtWindow1Len when mBtCounter reaches it |
| 0x2D50 | `m2948` | `mBtWindow1Len` | attack_set 021C9310: l_mo2HIO mAttack1BtWindow1Len / mAttack2BtWindow1Len / mAttack5BtWindow1Len; daMo2_Execute 021C9B1C copies it to mBtWindowTimer at mBtWindow1Start |
| 0x2D52 | `m294A` | `mBtWindow2Start` | attack_set 021C9310: l_mo2HIO mAttack1BtWindow2Start / mAttack2BtWindow2Start / mAttack5BtWindow2Start; daMo2_Execute 021C9B1C loads mBtWindowTimer with mBtWindow2Len when mBtCounter reaches it |
| 0x2D54 | `m294C` | `mBtWindow2Len` | attack_set 021C9310: l_mo2HIO mAttack1BtWindow2Len / mAttack2BtWindow2Len / mAttack5BtWindow2Len; daMo2_Execute 021C9B1C copies it to mBtWindowTimer at mBtWindow2Start |
| 0x2D56 | `m294E` | `mBtWindowTimer` | daMo2_Execute 021C9B1C counts it down and sets mBtNowFrame 5 (inside the parry window) while it is not 0, else 100; damage_check clears it |
| 0x2D59 | `m2951` | `mItemStolen` | damage_check (in daMo2_Execute 021C9B1C) sets 1 at the first steal hit (type 0xE) and starts ACTION_P_LOST; daMo2_Draw 021C7344 hides the mo_e1 shape while it is set |
| 0x2D5A | `m2952` | `mJawAngleZ` | nodeCallBack 021C6854: ZrotM of joint MO_JNT_JAWA_J_e by it; aite_miru 021CEC44 / fight_run (in Mo2_move 021D0A14) ease it to 12000, Mo2_move 021D0A14 back to 0 |
| 0x2D5C | `m2954` | `mHitActorId` | attack_set 021C9310 clears it; fight (in Mo2_move 021D0A14) stores the id of a Moblin its attack hit; aite_miru 021CEC44 looks at that actor and clears it |
| 0x2D68 | `m2960` | `mCutBits` | damage_check (in daMo2_Execute 021C9B1C) ORs atInfo->mPlCutBit in; fight_run (in Mo2_move 021D0A14) guards against those cuts; search_bomb 021C8378 looks for bombs only once bit 0x200 is set |
| 0x2D6C | `m2964` | `mDashFrames` | fight_run (in Mo2_move 021D0A14) sets 1 when the armed Moblin starts the DASH animation, adds 1 per frame and stops the dash after 20 frames; 0 = not dashing |
| 0x2D70 | `m2968` | `mPathOn` | path_check 021C9004: 0 or mPathIndex + 1; jyunkai (in Mo2_move 021D0A14) follows the path while it is not 0 |
| 0x2D71 | `m2969` | `mPathPoint` | path_check 021C9004 picks the start point; jyunkai (in Mo2_move 021D0A14) steps it by mPathDir and indexes ppd->m_points with it; nage 021CD958 steps it back |
| 0x2E0C | `m2A08` | `mLanternStep` | kantera_get_init (in daMo2_Create 021CCB00): 0 creates the lantern, then counts the frames until it exists (deletes the Moblin after 20), 100 = lantern linked; daMo2_Draw 021C7344 draws nothing while it is 1..99 |
| 0x2E0E | `m2A0A` | `mHioOwner` | daMo2_Create 021CCB00 sets 1 when it created the l_mo2HIO child; daMo2_Delete 021CC44C deletes the HIO child only then |
| 0x2E0F | `m2A0B` | `mParalyzeDelay` | damage_check (in daMo2_Execute 021C9B1C) sets REG_S(13,3) + 8 for attack type 10; Mo2_move 021D0A14 counts it down and starts ACTION_D_MAHI at 0 |
| 0x2E10 | `m2A0C` | `mSoundTargetTimer` | jyunkai (in Mo2_move 021D0A14) sets 30 when it hears a sound (dKy_Sound_get); Mo2_move 021D0A14 counts it down and targets mSoundTargetPos instead of the player while it runs |
| 0x2E14 | `m2A10` | `mSoundTargetPos` | jyunkai (in Mo2_move 021D0A14) copies the heard sound's position (dKy_Sound_get); Mo2_move 021D0A14 measures mTargetDist / mTargetAngleY to it while mSoundTargetTimer runs |
| 0x2E20 | `m2A1C` | `mDefeated` | fail 021CED78 sets 1 for a boss-status Moblin instead of deleting it; daMo2_Draw 021C7344 draws nothing, Mo2_move 021D0A14 turns Co off and mo2_demo_camera (in daMo2_Execute 021C9B1C) stops following while it is set |
| 0x2E21 | `m2A1D` | `mDemoCamMode` | mo2_demo_camera (in daMo2_Execute 021C9B1C): switch of the demo camera steps; nage 021CD958 sets 2 / 10, damage_check 0x32, daMo2_Execute 1 |
| 0x2E22 | `m2A1E` | `mDemoCamTimer` | mo2_demo_camera (in daMo2_Execute 021C9B1C): cleared per step, +1 per frame, compared with 150 |
| 0x2E24 | `m2A20` | `mDemoCamEye` | mo2_demo_camera (in daMo2_Execute 021C9B1C): copies the camera's eye and passes it to dCamera_Set as the eye |
| 0x2E30 | `m2A2C` | `mDemoCamCenter` | mo2_demo_camera (in daMo2_Execute 021C9B1C): actor position / camera centre eased toward the actor (+230); passed to dCamera_Set as the centre |
| 0x2E48 | `m2A44` | `mDemoCamFovy` | mo2_demo_camera (in daMo2_Execute 021C9B1C): 55 eased to 30; passed to dCamera_Set as the fovy |
| 0x2E4C | `m2A48` | `mCatchDemoDelay` | AtHitCallback 021C9AA0 sets 1 when the spear hits a hooded player (prison mode); daMo2_Execute 021C9B1C counts it down and at 0 starts ACTION_E3_DEMO |
| 0x2E4E | `m2A4A` | `mCatchEventPending` | AtHitCallback 021C9AA0 sets 1 with mCatchDemoDelay; daMo2_Execute 021C9B1C orders a potential event each frame until it is accepted, then clears it |
| 0x2E4F | `m2A4B` | `mSendToPrison` | e3_demo 021D0570 sets 1 at the end of the catch demo; daMo2_Execute 021C9B1C then sets the next stage "majroom" |
| 0x2E50 | `m2A4C` | `mCatchTimer` | AtHitCallback 021C9AA0 sets 200; daMo2_Execute 021C9B1C counts it down, stops JA_SE_MAJUTOU_ALERM at 30 and sets the next stage "majroom" at 1 |


Correction (a tww name that the tww code itself shows to be wrong may be corrected, own commit). `mHasPath` is the tww d217e2b name of this field, but the tww code (src/d/actor/d_a_mo2.cpp lines 1325, 1330 / 1340, 1386) and ours use it as the step direction along the path, not as a flag:

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x2D72 | `mHasPath` | `mPathDir` | daMo2_Create 021CCB00 sets 1 with a path; jyunkai (in Mo2_move 021D0A14) adds it to mPathPoint, sets -1 / 1 at the path ends and negates it; path_check 021C9004 / nage 021CD958 step mPathPoint back by it |

Round 2:

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x2D3C | `m2934` | `mClangEffectPos` | fight (in Mo2_move 021D0A14) sets particle ID_AK_JN_NG and starts JA_SE_LK_ROPE_HOOK_METAL at it on the spear-bounce branch (m2940 != 0); daMo2_Delete 021CC44C deletes its sound object; never written (tww the same) |

Open (no name yet): `m0570` (mDoExt_3DlineMat0, only constructed / destroyed), `m05D0[4]` and `m2978[3]` (no use; ga_s is otherwise unused), `m279C` (dCcD_Sph, only constructed / destroyed), `m2940` (yari_hit_check clears and shifts it, so it is always 0; fight branches on it; what 1 / 2 would mean is not settled), `m2970` (no use in WWHD; tww reads it but never writes it), `m2A09` (damage_check / nage / e3_demo set 1 / 2; never read), `m2A38` / `m2A3C` (mo2_demo_camera sets them; never read), `m2A40` (nage sets it, e3_demo eases it to 0; never read). The helper struct ke_s keeps `m0F0` (tail gap) and ga_s keeps all its members (`m10` .. `m2F`, no use).

## ks_class (Morth; d_a_ks.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_ks.h) keeps these GameCube offset names and TP has no ks_class, so every name below is from the WWHD use; for `m528` / `m52C` the tww types (MtxP, dPa_rippleEcallBack) of the same fields are cited.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x544 | `m2C8` | `mType` | daKS_Create 021AA4C8: param bits 0-7 (0xFF -> 0); types 0 / 1 create mBirthNum Morths (param 3 / 2), 4 / 5 are linked to Mothula (fpcNm_GM_e; 5 is thrown out), 2 starts in KS_MOVE, 6 in KS_POT_SEARCH, 7 in KS_BIRTH; daKS_Draw 021A71EC draws nothing for type 6 and daKS_Execute 021A7F94 returns after its action |
| 0x545 | `m2C9` | `mBirthNum` | daKS_Create 021AA4C8: param bits 8-15 (1 when 0 or > 0x15); types 0 / 1 create that many Morths; action_tubo_search (in daKS_Execute 021A7F94) creates that many from the pot |
| 0x546 | `m2CA` | `mSearchRangePrm` | daKS_Create 021AA4C8: param bits 16-23 (0xFF -> 0); mSearchRange = it * 10 |
| 0x549 | `m2CD` | `mDrawSubDone` | daKS_Execute 021A7F94 sets 1 after it calls draw_SUB 021A6F64 itself (deleted after falling out of the map); daKS_Draw 021A71EC skips draw_SUB while it is set and clears it |
| 0x54A | `m2CE` | `mFirstStuck` | action_omoi (in daKS_Execute 021A7F94) sets 1 for the Morth that sticks to Link while KUTTUKU_ALL_COUNT is 0; only that one checks Link's front roll and resets HEAVY_IN / KUTTUKU_ALL_COUNT when they come off; body_atari_check 021A74E4 tests it in mode 43 |
| 0x54B | `m2CF` | `mRippleOn` | daKS_Execute 021A7F94 sets 1 and starts the water ripple (mRipple) when the Morth is in water, clears it and ends the ripple on ground / 100 above the water; ks_kuttuki_check 021A73E4 ends it when the Morth sticks to Link |
| 0x54C | `m2D0` | `mFrozen` | body_atari_check 021A74E4 sets 1 at an ice arrow / light arrow hit; daKS_Execute 021A7F94 steps mBrkFrame to 7 while it is set; daKS_Draw 021A71EC skips draw_SUB and draw_SUB 021A6F64 lifts the model by mBgOffsetY |
| 0x54E | `m2D2` | `mStickJointDir` | action_omoi (in daKS_Execute 021A7F94): 0 = mStickJointNo counts up to 0x13, 1 = counts down to 0 (the Morth crawls over Link's joints) |
| 0x54F | `m2D3` | `mNoGroundCount` | daKS_Execute 021A7F94 (naraku_check): +1 per frame without ground, deletes the actor after 50 frames; cleared when there is ground |
| 0x564 | `m2E8` | `mTimers` | daKS_Execute 021A7F94 counts all 4 down each frame; [0] blink wait (reloads rnd 50 +-25 and starts mEyeBtkFrame), [1] / [2] action timers (action_kaze_move, action_kougeki_move, action_omoi) |
| 0x576 | `m2FA` | `mSpinAngle` | draw_SUB 021A6F64: z rotation of the body model (+ before, - after the scale) and adds mSpinSpeed per draw |
| 0x578 | `m2FC` | `mMoveAngleOffset` | action_dousa_move (in daKS_Execute 021A7F94): random offset ((process id & 0xF) * cM_rndFX(512)) added to the player angle as the move angle; cleared while Link is slow or swimming |
| 0x57A | `m2FE` | `mSpinSpeed` | daKS_Create 021AA4C8: (process id & 7) * 50 + 1000; draw_SUB 021A6F64 adds it to mSpinAngle; action_omoi (in daKS_Execute 021A7F94) turns shape_angle.y by it / 4 |
| 0x57C | `m300` | `mStickJointNo` | action_omoi (in daKS_Execute 021A7F94): index 0..0x13 into pl_harituki_joint_dt (Link joint the Morth sticks to), stepped by mStickJointDir |
| 0x57E | `m302` | `mEyeBtkFrame` | daKS_Execute 021A7F94 sets 1 when mTimers[0] runs out and steps it to 7, then 0 (blink); daKS_Draw 021A71EC sets the eye BTK frame to it |
| 0x580 | `m304` | `mBgOffsetY` | daKS_Create 021AA4C8 sets 25; BG_check 021A7F24 lowers the position by it for the ground check and raises it back; draw_SUB 021A6F64 lifts the models by it while mFrozen |
| 0x584 | `m308` | `mWindRiseStep` | action_kaze_move (in daKS_Execute 021A7F94): cleared at the start, eased to 5; step of speed.y toward mWindRiseSpeed |
| 0x588 | `m30C` | `mSwayAmp` | action_dousa_move (in daKS_Execute 021A7F94) sets 0 / 10, action_kaze_move 2; speed_keisan 021A7A88 adds sin(m2F0[0]) * it to the move angle; action_kougeki_move / action_kaze_move clear it |
| 0x58C | `m310` | `mWindStartY` | action_kaze_move (in daKS_Execute 021A7F94) copies pos.y at the start; the wind lift ends 200 above it (or when mTimers[1] runs out) |
| 0x590 | `m314` | `mWindRiseSpeed` | action_kaze_move (in daKS_Execute 021A7F94): rnd(15) + 10; speed.y is eased to it |
| 0x594 | `m318` | `mSearchRange` | daKS_Create 021AA4C8: mSearchRangePrm * 10; action_tubo_search (in daKS_Execute 021A7F94) waits in the pot until the player is within it (0 = no wait) |
| 0x598 | `m31C` | `mSphRadius` | daKS_Create 021AA4C8 sets 20; action_kougeki_move 20 / 60, action_tubo_search 100; daKS_Execute 021A7F94 sets it as the mSph radius each frame |
| 0x59C | `m320` | `mBrkFrame` | daKS_Execute 021A7F94 adds 1 up to 7 while mFrozen; daKS_Draw 021A71EC sets the body and eye BRK frames to it |
| 0x7A4 | `m528` | `mpStickJointMtx` | action_omoi (in daKS_Execute 021A7F94): daPy_getModelJointMtx(Link, pl_harituki_joint_dt[mStickJointNo]) (tww d217e2b type MtxP) of the Link joint the Morth sticks to; when the Morth comes off (mode 42 / later) it is cleared together with status 0x4000 |
| 0x7A8 | `m52C` | `mRipple` | ks_class_ct 021AA364: dPa_rippleEcallBack ctor (tww d217e2b type dPa_rippleEcallBack); daKS_Execute 021A7F94 starts particle ID_AK_JN_HAMON00 into it in water; ks_kuttuki_check 021A73E4 / daKS_Execute end it |

Open (no name yet): `m2D1` (no use), `m2DC` (daKS_Create 021AA4C8 sets it to (1, 1, 1); never read), `m2F0[5]` (clear_m2F0 clears all 5; each action uses the elements differently: [0] sway phase in action_dousa_move / action_kaze_move and step counter in action_dead_move, [1] a flag in action_kougeki_move and the spin step in action_dead_move, ...).

## sss_class (Dexivine; d_a_sss.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_sss.h) keeps these GameCube offset names (field_0x...) and TP has no sss_class, so every name below is from the WWHD use. The helper struct sss_s is named in round 2, below the table.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D4 | `field_0x2B8` | `mType` | daSss_Create 0248F140: SSS_WATER_e (param byte 0 == 1) or SSS_LAND_e; daSss_Draw 0248EC3C draws and daSss_Execute 0248EEEC moves only while it is set; hand_move 0248F484 picks SVINE_OUT_WATER / SVINE_OUT by it |
| 0x3D5 | `field_0x2B9` | `mOpenRangePrm` | daSss_Create 0248F140: param bits 8-15; hand_move 0248F484 opens the hand (SSS_WAIT_e -> SSS_OPEN_e) when the player is within it * 10 (0xFF: 1000) and closes again 100 beyond |
| 0x3D6 | `field_0x2BA` | `mSwitchNo` | daSss_Create 0248F140: param switch (0xFF -> 0); hand_move 0248F484 (SSS_WAIT_e) opens the hand when that switch is on instead of by distance |
| 0x3D8 | `field_0x2BC` | `mFrameCounter` | daSss_Create 0248F140: rnd(10000); daSss_Execute 0248EEEC adds 1 per frame; hand_move 0248F484 / control1 / cut_control1 (in hand_move) use it as the sway phase and its low bits for the magic drain and the sphere points |
| 0x3DC | `field_0x2C0` | `mAction` | hand_move 0248F484: switch over SSS_WAIT_e / SSS_OPEN_e / SSS_REACH_e / SSS_CATCH_e / SSS_CUT_e / SSS_SINK_e |
| 0x3DE | `field_0x2C2` | `mTimers` | daSss_Execute 0248EEEC counts the 3 down; indexed by SSS_ACTION_TIMER_e / SSS_EMITTER_TIMER_e / SSS_HIT_TIMER_e in hand_move 0248F484 and cut_control1 (in hand_move) |
| 0x3E4 | `field_0x2C8` | `mHandPos` | hand_move 0248F484: the hand position, eased toward the player / the open point and falling after the cut; control2 (in hand_move) makes it the last vine point |
| 0x3F0 | `field_0x2D4` | `mHandMtxPos` | control2 / cut_control2 (in hand_move 0248F484) copy the last vine point; hand_mtx_set 0248EDD0 places the hand model there |
| 0x3FC | `field_0x2E0` | `mHandAngleX` | control2 / cut_control2 (in hand_move 0248F484): -atan2 of the last vine segment; hand_mtx_set 0248EDD0 rotates the hand model x by it |
| 0x3FE | `field_0x2E2` | `mHandAngleY` | control2 / cut_control2 (in hand_move 0248F484): atan2 of the last vine segment; hand_mtx_set 0248EDD0 rotates the hand model y by it |
| 0x404 | `field_0x2E8` | `mSegmentLen` | hand_move 0248F484 eases it to 0 / 25 / 30 by action; control1 / control2 / cut_control2 (in hand_move) use it as the length of each vine segment |
| 0x40C | `field_0x2F0` | `mReachPull` | hand_move 0248F484 eases it to 10 in SSS_REACH_e, else 0; control1 (in hand_move) adds it along the vine direction to every segment |
| 0x410 | `field_0x2F4` | `mSwayAmp` | hand_move 0248F484: 10 (catch) / 50 (cut), eased to 5 / 50 / 0; control1 / cut_control2 (in hand_move) scale the segment sway by it |
| 0x414 | `field_0x2F8` | `mCutGroundY` | hand_move 0248F484 (SSS_CUT_e): ground height under the falling hand; cut_control2 (in hand_move) keeps the vine points above it + 5 |
| 0x418 | `field_0x2FC` | `mCutSegmentLen` | hand_move 0248F484 sets 1.5 * the 5th segment length at the cut and eases it to 0; cut_control1 (in hand_move) uses it as the segment length of the cut-off piece; daSss_Draw 0248EC3C draws that piece while it is > 0.1 |
| 0x41C | `field_0x300` | `mVineLine` | useHeapInit 0248EFEC: init(1, 10, BTI); hand_move 0248F484 copies the 10 vine points into it; daSss_Draw 0248EC3C updates and draws it (l_color) |
| 0x5A4 | `field_0x33C` | `mVine` | control1 / control2 (in hand_move 0248F484): the 10 vine points from the root (current.pos) to the hand; size_d[] widths; copied into mVineLine |
| 0x6BC | `field_0x454` | `mCutLine` | useHeapInit 0248EFEC: init(1, 5, BTI); hand_move 0248F484 copies the 5 cut-piece points into it; daSss_Draw 0248EC3C draws it (l_cutColor) while mCutSegmentLen > 0.1 |
| 0x844 | `field_0x490` | `mCutVine` | hand_move 0248F484 copies the first 5 vine points at the cut; cut_control1 (in hand_move) moves them and sets the TSURU_TAIEKI00 particle at the end |
| 0x90C | `field_0x558` | `mVineSph` | daSss_Create 0248F140: Set(tg_sph_src) x3; hand_move 0248F484 places them on vine points (by mFrameCounter) and cuts the vine on a Tg hit |
| 0xC90 | `field_0x8DC` | `mHandSph` | daSss_Create 0248F140: Set(bm_sph_src); hand_move 0248F484 centres it on eyePos; a Tg hit cuts the vine, attack type 8 pushes the hand away instead |
| 0xDBC | `field_0xA08` | `mPushDist` | hand_move 0248F484 sets 300 when mHandSph is hit by attack type 8 and eases it to 0; the hand moves to 100 up / it away from the root while it is > 1 |
| 0xDC0 | `field_0xA0C` | `mPushAngleY` | hand_move 0248F484: player angle + 0x8000 at that hit; direction of the push (mPushDist) |
| 0xDCC | `field_0xA18` | `mCutEffectOn` | hand_move 0248F484 sets 1 at the cut; cut_control1 / cut_control2 (in hand_move) start the TSURU_TAIEKI00 emitters only while it is set; cut_control2 clears it when its emitter ends |

Open (no name yet): none of sss_class's offset-named fields.


### sss_s (one vine point; d_a_sss.h)

| Offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `field_0x00` | `mPos` | the vine point: hand_move 0248F484 (control1 / control2) pulls each point toward its neighbour and pins mVine[0] to current.pos / mVine[9] to mHandPos; lines_copy (in hand_move) copies it into the line segments (tww the same) |
| 0x18 | `field_0x18` | `mSize` | hand_move 0248F484 (control3) sets size_d[i] * a pulsing factor; lines_copy (in hand_move) converts it to the line width byte; the cut vine copies it (tww the same) |

Open: the padding `_0C`.

## nzg_class (rat hole; d_a_nzg.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_nzg.h) keeps these GameCube offset names and TP has no nzg_class, so every name below is from the WWHD use.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D4 | `m2B8` | `mSpawnRangePrm` | daNZG_Create 0231175C: param bits 0-7; mSpawnRange = it * 10 |
| 0x3D5 | `m2B9` | `mMaxRatsPrm` | daNZG_Create 0231175C: param bits 8-15; mMaxRats = it (0 -> 1) |
| 0x3D6 | `m2BA` | `mSpawnMode` | daNZG_Create 0231175C: param bits 16-23 (> 2 -> 0); mode 0 also creates an NpcNz child; nzg_00_move (in daNZG_Execute 02311240) gives the rat param 0x100 (no player-range wait) always for 1, by 50 % for 2 |
| 0x3D7 | `m2BB` | `mBlocked` | nzg_00_move (in daNZG_Execute 02311240) sets 1 when another actor (not a rat, bomb, item or bait) touches the hole cylinder; daNZG_Execute switches on it; nzg_01_move clears it once that actor is 80 away |
| 0x3DA | `m2BE` | `mRatCount` | nzg_00_move (in daNZG_Execute 02311240) adds 1 per rat created (fopAcM_createChild fpcNm_NZ_e) and creates no more once it reaches mMaxRats |
| 0x3DC | `m2C0` | `mMaxRats` | daNZG_Create 0231175C: mMaxRatsPrm or 1; nzg_00_move (in daNZG_Execute 02311240) stops creating rats at it |
| 0x3DE | `m2C2` | `mTimers` | daNZG_Execute 02311240 counts all 5 down; nzg_00_move (in daNZG_Execute) sets [0] to (child id & 3) * 10 + 20 after a rat and waits for it |
| 0x3E8 | `m2CC` | `mSpawnRange` | daNZG_Create 0231175C: mSpawnRangePrm * 10; nzg_00_move (in daNZG_Execute 02311240) creates rats without param 0x100 only while the player is within it * 0.5 |
| 0x3EC | `m2D0` | `mNoSpawnRange` | daNZG_Execute 02311240 sets 400 each frame; nzg_00_move (in daNZG_Execute) creates a rat only while the player is farther than it * 0.5 |
| 0x3F0 | `m2D4` | `mBlockerId` | nzg_00_move (in daNZG_Execute 02311240) stores the id of the actor on the hole; nzg_01_move (in daNZG_Execute) checks its distance and resets it to fpcM_ERROR_PROCESS_ID_e |
| 0x405 | `m2E9` | `mPathNo` | daNZG_Create 0231175C: param bits 24-31; dPath_GetRoomPath(it, room) into mpPath when it is not 0xFF |

Open (no name yet): only the padding arrays `m2BC[2]` and `m2DC[]` (no use).

## mant_class (Darknut / Phantom Ganon cape; d_a_mant.h)

Correction (same pattern as d_tree / d_magma, see docs/field-names/draw-systems.md): the view `mantHD::IndexBuffer` of agl::IndexBufferLayout (027B5430 construct, 027B54E0 setStorage) had its first two words named the wrong way round. GX2DrawIndexedEx(mode r3, count r4, indexType r5, indices r6, ...) in daMant_packet_c::setupUniforms 021BE340 takes word 0 as the index type and word 4 as the mode. The code was right; only the names were wrong. setupUniforms' locals follow (primitive -> indexFormat, format -> mode).

| Offset in the view | Old | New | Evidence |
|---|---|---|---|
| 0x0 | `mPrimitive` | `mIndexFormat` | GX2DrawIndexedEx indexType (r5) in daMant_packet_c::setupUniforms 021BE340; agl::IndexBufferLayout::mFormat |
| 0x4 | `mFormat` | `mPrimitiveMode` | GX2DrawIndexedEx mode (r3) in daMant_packet_c::setupUniforms 021BE340; set to 4 (triangles) by daMant_packet_c_ct 021BD4A0; agl::IndexBufferLayout::mPrimitiveMode |

Offset-named mant_class fields (zeldaret/tww d217e2b include/d/actor/d_a_mant.h keeps these GameCube offset names; TP has no mant_class; every name is from the WWHD use). The view TnMantle in d_a_tn.h is renamed with the class.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3644 | `m1BF8` | `mSegmentLen` | daMant_Create 021BE134 sets 30 (Darknut) / 40 (Phantom Ganon); daMant_Execute 021BC3C4 uses it (+ a sine per row, * scale.y) as the distance between the cape nodes and eases it back to 30 |
| 0x3648 | `m1BFC` | `mSpeedRate` | daMant_Create 021BE134 sets 0.7 / 0.8; daMant_Execute 021BC3C4 keeps (new - old position) * it as each node's speed and eases it back to 0.7 |
| 0x365A | `m1C0E` | `mWindTimer` | daMant_Execute 021BC3C4: 2 / 6 at a mesh hit, the wind duration + 5 at a mWindSph hit; counted down, mWindAmplitude rises while it runs and mWindGust only while it is > 4 |
| 0x4280 | `m2834` | `mMeshHitOn` | daMant_Create 021BE134 sets 10 for the Darknut cape; daMant_Execute 021BC3C4 gives the mMeshSph spheres radius 30 only while it is not 0 (else -200, hidden) and clears it when a sword hit tears the cape (mTexNo 2 / 4) |
| 0x4282 | `m2836` | `mHitWait` | daMant_Execute 021BC3C4 counts it down, checks the mesh spheres for hits only while it is 0 and sets 10 after a hit |

Second batch (the views TnMantle in d_a_tn.h and FganonCape in d_a_fganon.h are renamed with the class). The two cape points are the owner's right / left shoulder: zeldaret/tww d_a_tn.cpp and d_a_fganon.cpp write m1BE0 from the joints TN_YOROI1_JNT_J_YOROI_KATA_R1_e (0x6) / BPG_JNT_J_BPG_KATA_R1_e (0x14) and m1BEC from TN_YOROI1_JNT_J_YOROI_KATA_L1_e (0x4) / BPG_JNT_J_BPG_KATA_L1_e (0xB) (the generated joint enums in tww assets/GZLE01/res/Object/Tn.h, Fganon.h); WWHD uses the same joints 6 / 4 and 20 / 11.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x362C | `m1BE0` | `mShoulderPosR` | the cape's right shoulder point: part_move 024BC3EC (Darknut) writes it from armor joint 6 (zeldaret/tww d_a_tn.cpp: TN_YOROI1_JNT_J_YOROI_KATA_R1_e) + (2, y, -43), execute_collision (inlined in daFganon_Execute 02134B28) from joint 0x14 (zeldaret/tww d_a_fganon.cpp: BPG_JNT_J_BPG_KATA_R1_e) + (35, 0, -30); daMant_Execute 021BC3C4 spreads the top cape row from mShoulderPosL (row 0) to it (row 8) |
| 0x3638 | `m1BEC` | `mShoulderPosL` | the cape's left shoulder point: part_move 024BC3EC writes it from armor joint 4 (zeldaret/tww d_a_tn.cpp: TN_YOROI1_JNT_J_YOROI_KATA_L1_e) + (2, y, 43), execute_collision (inlined in daFganon_Execute 02134B28) from joint 0xB (zeldaret/tww d_a_fganon.cpp: BPG_JNT_J_BPG_KATA_L1_e) + (35, 0, 30); daMant_Execute 021BC3C4 starts the top cape row (row 0) here; daTn_Execute 024BD170 parks both points at (-10000, 20000, -21000) |

## st_class (Stalfos; d_a_st.h)

Offset-named st_class fields (zeldaret/tww d217e2b include/d/actor/d_a_st.h keeps these GameCube offset names; TP has no st_class; every name is from the WWHD use, the knockback pair also from tww's VERSION_DEMO damage_check). The header's several-fields-per-line declarations were first split one per line (layout-neutral). Callers: jyunkai, fight (with buki_smoke_set), fight2, ground_wait and kan_wait (with ambush_trigger) are inlined into St_move 024972A4; damage_check, cc_set, eff_move and spin_smoke_move (with smoke_set_s) into daSt_Execute 02493FB0.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03D5 | `m02B9` | `mAmbushTrigger` | daSt_Create 02495C70 sets mAmbushSwitch + 1 (0xFF: by sight range) for the ambush behaviours 1-3; ambush_trigger (in ground_wait / kan_wait, St_move 024972A4) checks that switch (value - 1) or mAmbushSightRange; cleared when the Stalfos appears; daSt_Draw 02491F34 draws nothing while it is set and part_move 024925B8 moves the held weapon away |
| 0x03E0 | `m02C4` | `mMode` | step within mActionState: every action function of St_move 024972A4 (jyunkai, fight, fight2, ground_wait, kan_wait) and st_break_wait 024963A4, head_damage 02496718, ue_move 02496AA0, sita_move 02496F34 switch on it; damage_check (in daSt_Execute 02493FB0) sets 0 / 20 / 35 by bodyForm |
| 0x03E6 | `m02CA` | `mSpinSeCounter` | fight (in St_move 024972A4): adds (5000 * abs(mSpinSpeed)) while spinning, plays SE 0x5895 each time it passes 100 (s16 wrap), 0 when not spinning |
| 0x03E8 | `m02CC` | `mLeftStance` | fight (in St_move 024972A4) sets 1 / 0 after the right / left attack ends; wait_set 0249210C and jyunkai pick dRes_INDEX_ST_BCK_LWAIT_e / LWALK_e (0x21 / 0x22) instead of WAIT / WALK and the left attack chains (mFightBehavior 5 / 15) while it is set |
| 0x03F8 | `m02DC` | `mTargetSpeed` | speed_pos_calc 024961B8 eases speedF to it (step mSpeedStep); the actions of St_move 024972A4, ue_move 02496AA0 and sita_move 02496F34 set it (0, l_HIO walk speed, 10, 20, -2) |
| 0x03FC | `m02E0` | `mSpeedStep` | speed_pos_calc 024961B8: step of cLib_addCalc2(speedF, mTargetSpeed, 1, it), reset to 1 every frame; jyunkai (in St_move 024972A4) sets 5 |
| 0x0400 | `m02E4` | `mTurnStep` | speed_pos_calc 024961B8: while on the ground (mAcch flag 0x20) and not 0, turns current.angle.y toward mPlayerPos by at most it, then clears it; jyunkai / fight (in St_move 024972A4), ue_move 02496AA0, sita_move 02496F34 set 0x200..0x2000 |
| 0x0404 | `m02E8` | `mFrameCounter` | daSt_Execute 02493FB0 increments it every frame; phase of the shake / wobble sines (part_move 024925B8, fight in St_move 024972A4, nun_control 0249196C), cc_set's (in daSt_Execute) odd / even weapon sphere and nun_control's every-8th-frame check |
| 0x0412 | `m02F6` | `mNoHitTimer` | damage_check (in daSt_Execute 02493FB0) checks hits only while it is 0 and sets 5 / 2 after a hit; daSt_Execute counts it down; daSt_Create 02495C70 sets 15, the wait / unarmed actions keep it at 10 |
| 0x0414 | `m02F8` | `mDisappearTimer` | st_break_wait 024963A4 sets 499 when the scattered parts start to reassemble, counts it down and at 0 removes the Stalfos (fopAcM_createDisappear, fopAcM_delete, death switch); part_move 024925B8 sets 500 when it picks the dropped weapon back up, damage_check (in daSt_Execute 02493FB0) / sita_move 02496F34 clear it |
| 0x0416 | `m02FA` | `mDeleteRequest` | daSt_Execute 02493FB0 deletes the actor while it is set (with the 4000-unit fall check); daSt_Draw 02491F34 draws nothing; never set in d_a_st |
| 0x0417 | `m02FB` | `mHioOwner` | daSt_Create 02495C70 sets 1 when this actor created the l_HIO child (hio_set); daSt_Delete 024957B4 deletes the HIO child only then |
| 0x0418 | `m02FC` | `mHeadAngleOffset` | part_move 024925B8: added to shape_angle (csXyz::operator+) for the attached head part 15's rotation; never written in d_a_st |
| 0x041E | `m0302` | `mMoveAngleOffset` | speed_pos_calc 024961B8 moves along current.angle.y + it; daSt_Execute 02493FB0 turns shape_angle.y toward the same and clears it; fight (in St_move 024972A4) sets a sine wobble of mFrameCounter |
| 0x0420 | `m0304` | `mSpinAngleY` | fight (in St_move 024972A4) adds 5000 * mSpinSpeed each frame; daSt_Execute 02493FB0 rotates the body model by shape_angle.y + it and eases it back to 0; the spin-end checks wait for it to be within +-0x400 |
| 0x0424 | `m0308` | `mSpinSpeed` | fight (in St_move 024972A4) eases it to +-1 during the spin attacks (KAITEN*), 0 otherwise; drives mSpinAngleY, mSpinSeCounter, the spin smoke (spin_smoke_move in daSt_Execute 02493FB0) and cc_set's weapon sphere size / spl 7 above 0.5 |
| 0x0668 | `m054C` | `mBodySph` | daSt_Create 02495C70 sets [0] from head_sph_src and [1..6] from body_sph_src; cc_set (in daSt_Execute 02493FB0) places them on the cc_j_no joints (radius 45 head / 25); damage_check checks their TG hits ([0] = head) |
| 0x0E9C | `m0D80` | `mWeaponSph` | daSt_Create 02495C70 sets it from wepon_sph_src; cc_set (in daSt_Execute 02493FB0) places it on the weapon (ST_JNT_BUKI_e), the nunchaku arm or the foot (ST_JNT_ASIR_e) by mAtMode |
| 0x0FC8 | `m0EAC` | `mWeaponTipPos` | St_move 024972A4: ST_JNT_BUKI_e joint + (160, 0, 0) every frame; the weapon drag smoke 0xA17D (mDragSmokeCallBack) is emitted there |
| 0x0FD4 | `m0EB8` | `mWeaponLinePos` | fight (in St_move 024972A4): ST_JNT_BUKI_e joint + (80, 0, 0); a wall line check from mWeaponLinePosOld to it ends the fast spin attack |
| 0x0FE0 | `m0EC4` | `mWeaponLinePosOld` | fight (in St_move 024972A4): the previous frame's mWeaponLinePos (copied before the update), start of the spin attack's wall line check |
| 0x0FEC | `m0ED0` | `mTgMode` | cc_set (in daSt_Execute 02493FB0): 0 all body spheres (mBodySph), 1 the head sphere only, 2 none; also keeps mWeaponSph off while not 0; reset to 0 every frame; fight2 / ground_wait / kan_wait (in St_move 024972A4) and st_break_wait 024963A4 set it |
| 0x0FED | `m0ED1` | `mAtMode` | cc_set (in daSt_Execute 02493FB0): 0 no weapon attack, 1 weapon, 2 spin (sphere scaled by mSpinSpeed), 3 kick (ST_JNT_ASIR_e); reset every frame; fight / fight2 (in St_move 024972A4) and sita_move 02496F34 set it |
| 0x0FEE | `m0ED2` | `mPartsAttached` | part_move 024925B8 counts down for each part that falls off and up for each part that gets back in place (not the weapon part 11); daSt_Create 02495C70 sets 25; st_break_wait 024963A4 ends the break once it is >= 25 |
| 0x0FEF | `m0ED3` | `mPartCommand` | part_move 024925B8: 1 scatters all parts, 10 drops the upper body parts (< 19), 20 the lower body parts, 2 / 11 sends the parts back; set by damage_check (in daSt_Execute 02493FB0), st_break_wait 024963A4, sita_move 02496F34, daSt_Create 02495C70; daSt_Execute clears it unless 2 |
| 0x2010 | `m1DC8` | `mNunMode` | the pulled-off arm used as a nunchaku (mNun): fight2 (in St_move 024972A4) sets 1 after NUKU (pull off), 3 for HONE_ATTACK, 0 after TUKERU (put back); wait_set 0249210C sets 2 (HYUNHYUN); daSt_Execute 02493FB0 runs nun_control 0249196C and skips parts 3 / 4 while it is set; daSt_Draw 02491F34 hides parts 5-7 |
| 0x2018 | `m1DD0` | `mWeaponState` | part_move 024925B8: 1 (daSt_Create 02495C70) picks the weapon up (fopAcM_setCarryNow) and becomes 10 (held); 2 while the weapon part 11 is carried back with the reassembling parts, 0 when it lies on the ground (or the Stalfos is hit by light); jyunkai (in St_move 024972A4) fights armed at 10, unarmed (fight2) at 0 |
| 0x2020 | `m1DD8` | `mHeadState` | part_move 024925B8: 1 when the head part 15 is knocked off, 2 while it is drawn / moved with mpMorf2, 0 when it is back; daSt_Draw 02491F34, ke_control 02491458, damage_check / cc_set (in daSt_Execute 02493FB0) treat the loose head separately |
| 0x2022 | `m1DDA` | `mHeadTargetAngleY` | part_move 024925B8: target of the loose head part's rotation y (cLib_addCalcAngleS2), set to its angle when it falls off and when it turns |
| 0x2024 | `m1DDC` | `mReassembleWait` | st_break_wait 024963A4 sets l_HIO.m1A / 100; daSt_Execute 02493FB0 counts it down; part_move 024925B8 moves the loose parts back only when it is 0 |
| 0x2026 | `m1DDE` | `mKnockbackAngleY` | speed_pos_calc 024961B8: direction of the extra speed mKnockbackSpeed added to speed; never written in WWHD's d_a_st (zeldaret/tww d_a_st.cpp: only the VERSION_DEMO damage_check sets it to the hit angle atInfo.m0C.y) |
| 0x2028 | `m1DE0` | `mKnockbackSpeed` | speed_pos_calc 024961B8: while > 0.1 adds this speed in direction mKnockbackAngleY to speed, then eases it to 0 (cLib_addCalc0, step 2.5); never set in WWHD (zeldaret/tww d_a_st.cpp: the VERSION_DEMO damage_check sets 40 on a hit) |
| 0x202C | `m1DE4` | `mShakeTimer` | damage_check (in daSt_Execute 02493FB0) sets 5 / 8 / 10 / 20 on a hit and counts it down; part_move 024925B8 shakes the attached parts by it * 2 |
| 0x20CA | `m1E82` | `mHeadShakeTimer` | damage_check (in daSt_Execute 02493FB0) sets 20 when the loose head is hit; part_move 024925B8 counts it down and shakes the head part 15 by it |
| 0x20CC | `m1E84` | `mBreakHealth` | daSt_Create 02495C70 sets 16 (5 for the upper body); damage_check (in daSt_Execute 02493FB0) subtracts the damage only for hammer (0xA8) / type-2 hits and breaks the Stalfos apart at <= 0; st_break_wait 024963A4 resets 16 after it reassembles |
| 0x20CE | `m1E86` | `mUpperSeparate` | sita_move 02496F34 sets 1 while the separate upper-body actor (mUpperBodyEntityId) exists and 0 when it deletes it; part_move 024925B8 moves eyePos / attention to the head part only while it is 0 |
| 0x20CF | `m1E87` | `mAtStarted` | cc_set (in daSt_Execute 02493FB0): 0 -> dCcD_Sph::StartCAt and counts up, then MoveCAt; cleared when the weapon attack is off |
| 0x20D0 | `m1E88` | `mSmokeCallBack` | smoke callbacks: [0] the appear smoke 0xA17C (ground_wait in St_move 024972A4), [1] the weapon ground smoke 0x2027 (buki_smoke_set in fight), [2] the spin smoke 0x2022 (smoke_set_s in daSt_Execute 02493FB0); actor_construct 02495AD0 / actor_destroy 0249A124 build / destroy the 3 |
| 0x2130 | `m1EE8` | `mBreakEmitter` | eff_move (in daSt_Execute 02493FB0): up to 20 emitters 0x817B made every 15 frames while the Stalfos lies broken (mBreakEffMode 1), each stopped after mBreakEmitterTimer; daSt_Delete 024957B4 stops the rest |
| 0x2180 | `m1F38` | `mBreakEmitterTimer` | eff_move (in daSt_Execute 02493FB0): 200 per mBreakEmitter slot, counted down; at 0 the emitter is stopped and the slot freed |
| 0x21A8 | `m1F60` | `mBreakEffWait` | eff_move (in daSt_Execute 02493FB0): counted down; a new mBreakEmitter is made when it is 0, then 15 again |
| 0x21AC | `m1F64` | `mBreakEffScale` | eff_move (in daSt_Execute 02493FB0): global particle scale of every mBreakEmitter; 1 while mBreakEffMode is 1, eased to 0 at 2 |
| 0x21B0 | `m1F68` | `mBreakEffMode` | st_break_wait 024963A4 sets 1 while the parts lie scattered and 2 when they are back; eff_move (in daSt_Execute 02493FB0) makes the 0x817B emitters at 1 and fades them at 2 |
| 0x21B4 | `m1F6C` | `mSpinSmokePos` | spin_smoke_move (in daSt_Execute 02493FB0): body position + 175 along mSpinSmokeAngleY; smoke_set_s puts it on the ground (line check) and emits the smoke 0x2022 / 0x24 there |
| 0x21C0 | `m1F78` | `mSpinSmokeAngle` | spin_smoke_move (in daSt_Execute 02493FB0): (0, mSpinSmokeAngleY, 0); angle of the spin smoke (smoke_set_s) |
| 0x21C6 | `m1F7E` | `mSpinSmokeAngleY` | fight (in St_move 024972A4) sets it from current.angle.y when a swing / spin starts; spin_smoke_move (in daSt_Execute 02493FB0) turns it by +-2000 or 5000 * mSpinSpeed each frame |
| 0x21C8 | `m1F80` | `mSpinSmokeTimer` | fight (in St_move 024972A4) sets 23 / 15; spin_smoke_move (in daSt_Execute 02493FB0) counts it down, makes smoke while it is >= 10 and ends mSmokeCallBack[2] at 0 |
| 0x21CA | `m1F82` | `mSpinSmokeDir` | fight (in St_move 024972A4): 0 right swing, 1 left swing, 2 spin; spin_smoke_move (in daSt_Execute 02493FB0) turns mSpinSmokeAngleY by +2000 / -2000 / 5000 * mSpinSpeed |
| 0x21CB | `m1F83` | `mSpinSmokeCount` | smoke_set_s (in daSt_Execute 02493FB0) emits the ground smoke only while it is 0 (or on attribute 4) and counts it up; spin_smoke_move clears it when the smoke ends |
| 0x21CC | `m1F84` | `mDragSmokeMode` | jyunkai (in St_move 024972A4) sets 1 when the walk starts and 2 while walking; St_move clears it every frame and, while the weapon is held (mWeaponState 10), emits the drag smoke 0xA17D at mWeaponTipPos at 1 / keeps it, else ends it |
| 0x21D0 | `m1F88` | `mDragSmokeCallBack` | St_move 024972A4: callback of the weapon drag smoke 0xA17D (emitted at mWeaponTipPos, ended when mDragSmokeMode is 0); actor_construct 02495AD0 builds it |

Open (no name yet): `m1DE5` (no use in WWHD; tww's VERSION_DEMO damage_check sets -1), `m1DE6[26]` (no use in WWHD; tww's nodeCallBack adds them to the joint matrices as per-joint rotations, but WWHD never reads them), `m2364` (daSt_Create 02495C70 sets 5 together with actor_status bit 0x4000, which tww calls fopAcStts_UNK4000_e; daSt_Execute 02493FB0 clears that bit when it counts down to 0; the bit's meaning is not known, so no name).

## ki_class (Keese, also Fire Keese; d_a_ki.h)

Offset-named ki_class fields (zeldaret/tww d217e2b include/d/actor/d_a_ki.h keeps these GameCube offset names; TP has no ki_class; every name is from the WWHD use). ki_wait_move, ki_fly_move, ki_fire_set_move, ki_damage_move, ki_path_move and ki_eye_tex_anm are inlined into daKi_Execute 0219AAE8 (d_a_ki_execute.cpp); ki_atack_move 02199C48 (d_a_ki_exec.cpp) and ki_pos_move 02199A64 are separate functions.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D4 | `m2B8` | `mModelPacket` | ki_class_ct 0219D3F4 / ki_class_dt 0219DAD8 construct / destroy it as an mDoExt_J3DModelPacketS (HD 0xB0 bytes); useHeapInit 0219CF38 sets it up (ki_packet_init) |
| 0x485 | `m2CD` | `mAttackRangePrm` | daKi_Create 0219D568: param bits 8-14; picks mMaxAttackMoveDist300 = 300 / 800 / 1500 / 3000 (0 / 1 / 2 / 3 and above) |
| 0x487 | `m2CF` | `mAppearSwitchPrm` | daKi_Create 0219D568: param bits 24-31; when not 0xFF, mAppearSwitch = it + 1 |
| 0x48C | `m2D4` | `mAppearSwitch` | daKi_Create 0219D568 sets mAppearSwitchPrm + 1; while it is not 0 daKi_Draw 02199748 draws nothing and daKi_Execute 0219AAE8 waits (no attention) until switch (it - 1) is on, then clears it and flies off |
| 0x48E | `m2D6` | `mPathPoint` | ki_path_move (in daKi_Execute 0219AAE8): index of the current point of ppd (ki_path_point), stepped by mPathDir and turned round at either end |
| 0x48F | `m2D7` | `mPathDir` | daKi_Create 0219D568 sets 1; ki_path_move (in daKi_Execute 0219AAE8) adds it to mPathPoint and sets -1 at the path's end |
| 0x494 | `m2DC` | `mVanish` | ki_fire_set_move (in daKi_Execute 0219AAE8) sets 1 after the flight timer runs out or when far from the player behind a wall; daKi_Execute then shrinks scale to 0 and deletes the actor |
| 0x4B8 | `m304` | `mTurnScale` | ki_pos_move 02199A64 turns current.angle y / x by at most it * mPosMoveDist; ki_atack_move 02199C48 sets 10000, ki_fly_move / ki_path_move (in daKi_Execute 0219AAE8) 3000 / 4000 |
| 0x4C8 | `m314` | `mNoHitTimer` | daKi_Execute 0219AAE8 counts it down and handles a mDamageSphere hit only while it is 0, then sets 10 (0x32 after fire); ki_damage_move keeps it at 3 |
| 0x4CA | `m316` | `mKnockbackAngleY` | daKi_Execute 0219AAE8 sets the player angle (fopAcM_searchPlayerAngleY) at a hit; ki_damage_move (in daKi_Execute) turns shape_angle.y to it and moves along it by mKnockbackSpeed |
| 0x4D0 | `m31C` | `mKnockbackSpeed` | daKi_Execute 0219AAE8 sets 100 (wind) / 150 / l_kiHIO.mHitKnockback / mHitKnockbackSpl at a hit; ki_damage_move (in daKi_Execute) moves back by it while > 0.1 and eases it to 0 (l_kiHIO.mKnockbackDecay) |
| 0x4D4 | `m320` | `mFrameCounter` | daKi_Create 0219D568 starts it at random(10000); daKi_Execute 0219AAE8 increments it every frame; ki_atack_move 02199C48 uses it as the phase of its sine / cosine circling offsets |
| 0x4D6 | `m322` | `mHeadTargetAngleY` | ki_wait_move (in daKi_Execute 0219AAE8): the player's direction relative to current.angle.y, clamped to +-10000; daKi_Execute eases mHeadAngleY to it and clears it |
| 0x4D8 | `m324` | `mHeadTargetAngleX` | ki_wait_move (in daKi_Execute 0219AAE8): the player's elevation (-atan2(y, xz)), clamped to -0x38E..0x1FF6; daKi_Execute eases mHeadAngleX to it and clears it |
| 0x4DA | `m326` | `mHeadAngleY` | daKi_Execute 0219AAE8 eases it to mHeadTargetAngleY; nodeCallBack 021991AC (on KI_JNT_J_KI_HEAD_e) applies it as the joint's Z rotation |
| 0x4DC | `m328` | `mHeadAngleX` | daKi_Execute 0219AAE8 eases it to mHeadTargetAngleX; nodeCallBack 021991AC (on KI_JNT_J_KI_HEAD_e) applies it as the joint's Y rotation |
| 0x4E0 | `m32C` | `mBtpPattern` | tex_anm_set 021994B0 stores the J3DAnmTexPattern it picked and resets its frame; daKi_Draw 02199748 sets its frame to mEyeFrame and (re)initialises mBtpAnm with it |
| 0x4E4 | `m330` | `mBtpAnm` | useHeapInit 0219CF38 creates the mDoExt_btpAnm; daKi_Draw 02199748 initialises it with mBtpPattern and enters it at mEyeFrame |
| 0x4E8 | `m334` | `mEyeFrame` | tex_anm_set 021994B0 resets it to 0; ki_eye_tex_anm (in daKi_Execute 0219AAE8) counts it up to mEyeMaxFrame; daKi_Draw 02199748 shows the texture pattern frame it names |
| 0x4E9 | `m335` | `mEyeAnmOn` | tex_anm_set 021994B0 sets 1; ki_eye_tex_anm (in daKi_Execute 0219AAE8) advances mEyeFrame only while it is set and clears it at the end of a non-looping pattern |
| 0x4EA | `m336` | `mEyeMaxFrame` | tex_anm_set 021994B0: ki_tex_max_frame[pattern]; ki_eye_tex_anm (in daKi_Execute 0219AAE8) advances mEyeFrame until it |
| 0x4EB | `m337` | `mEyeLoop` | tex_anm_set 021994B0: ki_tex_loop[pattern]; ki_eye_tex_anm (in daKi_Execute 0219AAE8) restarts mEyeFrame at 0 while it is set, else stops |
| 0x4EC | `m338` | `mAnmPause` | ki_wait_move (in daKi_Execute 0219AAE8) sets 1 (state 3); daKi_Execute skips mpMorf->play for that frame and clears it |
| 0x4ED | `m339` | `mHioOwner` | daKi_Create 0219D568 sets 1 when this actor created the l_kiHIO child (hio_set); daKi_Delete 0219CE98 deletes the HIO child only then |
| 0x6F4 | `m540` | `mBgCheckOn` | daKi_Execute 0219AAE8 clears it each frame and runs mAcch.CrrPos (with the position lowered by 30) only while it is set; ki_atack_move 02199C48, ki_fly_move, ki_damage_move and ki_fire_set_move (in daKi_Execute) set it |
| 0x734 | `m580` | `mAtSph` | daKi_Create 0219D568 sets it from at_sph_src (attack type fire); ki_atack_move 02199C48 gives it radius 30 / attack power 1-2 and checks its AT hit; daKi_Execute 0219AAE8 resets the radius to 0 every frame |
| 0x860 | `m6AC` | `mCoSph` | daKi_Create 0219D568 sets it from co_sph_src; daKi_Execute 0219AAE8 gives it radius l_kiHIO.mCoSphRadius at current.pos every frame, ki_atack_move 02199C48 0 |
| 0xAB8 | `m904` | `mFallSpinDir` | ki_damage_move (in daKi_Execute 0219AAE8): random 0 / 1 when it hits a wall; then turns shape_angle.z by -0x250 (0) or +0x250 (1) per frame while falling |
| 0xABC | `m908` | `mFireCallBack` | ki_class_ct 0219D3F4 constructs the dPa_followEcallBack; ki_fire_set_move (in daKi_Execute 0219AAE8) passes it to the fire particle IT_SN_FIREK_FIRE_A and removes it with that emitter; daKi_Delete 0219CE98 removes it |
| 0xAD0 | `m91C` | `mFireEmitter` | ki_fire_set_move (in daKi_Execute 0219AAE8): the emitter of IT_SN_FIREK_FIRE_A (with mFireCallBack) while the fire Keese is flung out, force-deleted when it lands |
| 0xAD4 | `m920` | `mBtkAnm` | useHeapInit 0219CF38 creates and initialises the mDoExt_btkAnm; daKi_Execute 0219AAE8 plays it, daKi_Draw 02199748 enters it |

Round 2:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| enemyfire_l 0x228 | `m228` | `mHD20` | (round 3, misnamed: was mParticleScaleMultiplier) mLight + 0x20, the HD tail LIGHT_INFLUENCE::mHD20 (LIGHT_INFLUENCE is 0x24 in HD); enemyfire_ct 0219D368 sets 1.0 as the LIGHT_INFLUENCE constructor does; no reader |


## kiHIO_c (l_kiHIO, the keese tuning; d_a_ki.h)

Round 10. WWHD keeps the vtable at the end, so the offsets are 4 lower than the GameCube names. __sinit_d_a_ki_cpp 0219D920 sets every field; tww's kiHIO_c (d_a_ki.cpp) keeps the offset names and reads them the same way.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x01 | `m05` | `mDebugNoDeath` | daKi_Execute 0219AAE8: when set, a hit leaves the keese at health 10 (debug switch; __sinit 0219D920 clears it) (tww the same) |
| 0x02 | `m06` | `mDebugPause` | daKi_Execute 0219AAE8: the timers, animation and move procs only run while it is 0 (debug switch; __sinit 0219D920 clears it) (tww the same) |
| 0x03 | `m07` | `mDebugReport` | daKi_Execute 0219AAE8: when set, ki_check runs and JUTReport prints the ALL / READY / FIGHT counts (debug switch) (tww the same) |
| 0x04 | `m08` | `mFly1AnmSpeed` | ki_atack_move 02199C48 / ki_fly_move: anm_init's speed of the FLY1 animation (tww the same) |
| 0x08 | `m0C` | `mFlyWanderRange` | ki_fly_move (in daKi_Execute): cM_rndFX range of the next wander target around the flight position (tww the same) |
| 0x10 | `m14` | `mFly2AnmSpeed` | ki_atack_move 02199C48: anm_init's speed of the FLY2 animation (tww the same) |
| 0x14 | `m18` | `mAttack1AnmSpeed` | ki_atack_move 02199C48: anm_init's speed of the ATTACK1 animation (tww the same) |
| 0x18 | `m1C` | `mCircleHeight` | ki_atack_move 02199C48: the circling target point is rnd(50) + it above the player (tww the same) |
| 0x1C | `m20` | `mCircleDist` | ki_atack_move 02199C48: the circling (and reserve) target point's distance from the player (tww the same) |
| 0x20 | `m24` | `mCircleAngleRange` | ki_atack_move 02199C48: the circling target point's random angle (degrees, cM_rndFX) around the player's facing (tww the same) |
| 0x24 | `m28` | `mFlySpeed` | ki_atack_move 02199C48 / ki_fly_move / ki_path_move: mPosMoveTarget while flying; daKi_Execute 0219AAE8: speedF when the appear switch turns on (tww the same) |
| 0x28 | `m2C` | `mCircleMoveRate` | ki_atack_move 02199C48: the circling move's cLib_addCalc2 step factor (times mPosMoveDist) (tww the same) |
| 0x2C | `m30` | `mReserveMoveSpeed` | ki_atack_move 02199C48 behaviour 10 (waiting while enough keese fight): mPosMoveTarget and the cLib_addCalc2 step factor of the hover around the player (tww the same) |
| 0x30 | `m34` | `mHitKnockback` | daKi_Execute 0219AAE8: mKnockbackSpeed after a normal hit (tww the same) |
| 0x34 | `m38` | `mHitKnockbackSpl` | daKi_Execute 0219AAE8: mKnockbackSpeed after a hit with mAtSpl 1 (tww the same) |
| 0x38 | `m3C` | `mWallStunSpeed` | ki_damage_move (in daKi_Execute): hitting a wall while mKnockbackSpeed is at least it stuns the keese (BITA1) (tww the same) |
| 0x3C | `m40` | `mKnockbackDecay` | ki_damage_move (in daKi_Execute): cLib_addCalc0's step of mKnockbackSpeed (tww the same) |
| 0x40 | `m44` | `mAttackSpeed` | ki_atack_move 02199C48: mPosMoveTarget when the attack dive starts (tww the same) |
| 0x44 | `m48` | `mAttackChance` | ki_atack_move 02199C48: the chance (halved when mCrowdFightCount keese fight) to start an attack at each check (tww the same) |
| 0x48 | `m4C` | `mAttackCheckTime` | ki_atack_move 02199C48: mTimers[2] between attack checks (doubled when crowded) (tww the same) |
| 0x4A | `m4E` | `mAttackModeTimeMax` | ki_atack_move 02199C48: mTimers[0] (the attack mode's duration) = rnd(|it - mAttackModeTimeMin|) + mAttackModeTimeMin (tww the same) |
| 0x4C | `m50` | `mAttackModeTimeMin` | ki_atack_move 02199C48: the lower bound of mTimers[0], the attack mode's duration (tww the same) |
| 0x4E | `m52` | `mJoinFightCount` | ki_atack_move 02199C48 behaviour 10: a waiting keese joins the circling while fewer than it fight (tww the same) |
| 0x50 | `m54` | `mCrowdFightCount` | ki_atack_move 02199C48: from it fighting keese on, the attack chance is halved and the check time doubled (tww the same) |
| 0x54 | `m58` | `mCoSphRadius` | daKi_Execute 0219AAE8: mCoSph.SetR every frame (tww the same) |

Open (no name yet): `m10` (__sinit sets 1.0; read nowhere in WWHD or tww).

## pt_class (Miniblin; d_a_pt.h)

Offset-named pt_class fields (zeldaret/tww d217e2b include/d/actor/d_a_pt.h keeps these GameCube offset names, its d_a_pt.cpp has only stubs for the action functions; TP has no pt_class; every name is from the WWHD use). pt_wait, next_pos_set / jump_target_set, pt_move, pt_attack, pt_koke, pt_ples, pt_bat and damage_check are inlined into action 02447300 (d_a_pt_action.cpp).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D6 | `m2BA` | `mRespawn` | daPt_Execute 02446374 (fell below home - 3000) and action 02447300 (killed) set 1 for behaviour 0; daPt_Delete 024468E4 then creates a new Miniblin at the home position (if the spawn switches allow) |
| 0x3DC | `m2C0` | `mpMorf` | useHeapInit 02446A4C creates the mDoExt_McaMorf; anm_init 02445DBC sets its animations, daPt_Execute 02446374 plays / calcs it, daPt_Draw 02445EE4 draws its model; daPt_Create 02446E54 gives it to mEnemyFire |
| 0x3E0 | `m2C4` | `mpBtpAnm` | useHeapInit 02446A4C initialises it as the btp (pt_btpInit); daPt_Execute 02446374 sets its frame from mBlinkTimer (blink), daPt_Draw 02445EE4 enters it (mDoExt_btpAnm_entry) |
| 0x3E4 | `m2C8` | `mpBrkAnm` | useHeapInit 02446A4C initialises it as the brk (pt_brkInit); daPt_Execute 02446374 sets its frame to mColorNo, daPt_Draw 02445EE4 enters it (mDoExt_brkAnm_entry) |
| 0x3E8 | `m2CC` | `mColorNo` | useHeapInit 02446A4C: random 0..3; daPt_Execute 02446374 shows that frame of mpBrkAnm (body colour); pt_attack (in action 02447300) waits it * 3 + 10 frames |
| 0x3EC | `m2D0` | `mFrameCounter` | daPt_Execute 02446374 increments it every frame and checks the fall-out-of-the-world condition every 32nd frame |
| 0x3EE | `m2D2` | `mAction` | action 02447300 switches on it (PT_WAIT, PT_MOVE, PT_ATTACK, PT_KOKE, PT_PLES, PT_BAT); daPt_Create 02446E54 sets PT_WAIT, the action functions and damage_check (in action) switch it |
| 0x3F2 | `m2D6` | `mBlinkTimer` | daPt_Execute 02446374: counted down, its last 8 frames are the btp (blink) frames; restarts at random 20..50 |
| 0x410 | `m2F8` | `mpTargetPos` | action 02447300: the position of the bait (esa) actor if there is one, else the player's; pt_move / pt_attack (in action) jump toward it |
| 0x414 | `m2FC` | `mTargetAngleY` | action 02447300: angle to the bait (cM_atan2s) or the player (fopAcM_searchPlayerAngleY); next_pos_set / pt_attack (in action) aim with it |
| 0x418 | `m300` | `mTargetDist` | action 02447300: distance to the bait or the player (fopAcM_searchPlayerDistance); pt_attack (in action) goes back to PT_MOVE when it lands at > 700 |
| 0x41C | `m304` | `mTargetDistXZ` | action 02447300: XZ distance to the bait or the player (fopAcM_searchPlayerDistanceXZ); pt_move (in action) attacks at < 500 |
| 0x426 | `m30E` | `mNoHitTimer` | action 02447300 runs damage_check only while it is 0; daPt_Execute 02446374 counts it down; pt_wait / pt_ples / pt_bat / damage_check (in action) set 5..100 |
| 0x42C | `m314` | `mFallSpeedY` | daPt_Execute 02446374 keeps speed.y before mAcch.CrrPos; pt_attack (in action 02447300) switches to PT_KOKE (falls over) when it lands at < -100 |
| 0x430 | `m318` | `mHopY` | action 02447300: height of the hop drawn on top of the movement (adds mHopSpeed, floor 0); daPt_Execute 02446374 translates the model up by it |
| 0x434 | `m31C` | `mHopSpeed` | jump_target_set (in action 02447300) sets the jump distance * 0.1 (max 50); action adds it to mHopY and lowers it by 5 per frame |
| 0x438 | `m320` | `mKnockbackSpeed` | damage_check (in action 02447300) sets 40 / 80 / 90..120 / 110 at a hit; action moves back along mKnockbackAngleY by it, eases it to 0 (step 7) and kills a dead Miniblin when it ends or hits a wall; pt_koke waits for < 0.05 |
| 0x43C | `m324` | `mKnockbackAngleY` | damage_check (in action 02447300) sets the player angle, pt_bat a random flight angle; action moves back by mKnockbackSpeed along it |
| 0x440 | `m328` | `mAcchCir` | pt_class_ct 02446CF4 / pt_class_dt 02449B54 construct / destroy the dBgS_AcchCir; daPt_Create 02446E54 sets its wall (50, 50) and passes it to mAcch.Set |
| 0x480 | `m368` | `mAcch` | pt_class_ct 02446CF4 constructs the dBgS_ObjAcch; daPt_Create 02446E54 sets it (with mAcchCir), daPt_Execute 02446374 runs CrrPos; pt_attack (in action 02447300) reads its ground / wall flags |
| 0xEE0 | `mDC4` | `mHioOwner` | daPt_Create 02446E54 sets 1 when this actor created the l_HIO child (hio_set); daPt_Delete 024468E4 deletes the HIO child only then |

## wz_class (Wizzrobe; d_a_wz.h)

Offset-named wz_class fields (zeldaret/tww d217e2b include/d/actor/d_a_wz.h keeps the GameCube offset names m2B4, m314, m351, m7D8; TP has no wz_class). mHasActiveSummon, mDemoSteps and mpNextRoomPath are the names our decomp's view wz::ActionFields (d_a_wz.h) already uses for the same offsets, checked against the uses; the other names are from the use.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D0 | `m2B4` | `mpMorf` | useHeapInit 024E6420 creates the body mDoExt_McaMorf (user area = the actor); anm_init 024E2A90 sets its animations, draw_SUB 024E2148 calcs it and copies its joints to mpMorf2 / mpRodMorf, daWZ_Draw 024E2468 draws it |
| 0x430 | `m314` | `mBallHitPos` | next_tama_move 024E3950 (from wz_tamaNext with mBallSph's AT / TG hit position, in action_tama_dousa in daWZ_Execute 024E398C) stores the hit position and sets mode 0x68; action_tama_dousa then emits the burst 0x823A / 0x8282 there |
| 0x46D | `m351` | `mType3` | daWZ_Create 024E6BEC sets 1 for param type 3 (which then runs as behaviour 0); daWZ_Draw 024E2468 shows brk frame 3 instead of mBehaviorType + mIsMiniBoss |
| 0x470 | `m470` | `mHasActiveSummon` | weapon_shoot 024E3624 sets 1 when its summon child is created; the child's daWZ_Delete 024E61E0 / Create / action paths clear it on the parent; wz::ActionFields (d_a_wz.h, our decomp's view) already names it mHasActiveSummon |
| 0x4FA | `m4FA` | `mDemoSteps` | demo steps: action_demo 024E7904 switches on and counts up [0]; action_summon_dousa 024E9384 clears all 4 when it opens the door (mode 200); wz::ActionFields (our decomp's view) already names it mDemoSteps |
| 0x540 | `m540` | `mpNextRoomPath` | the next-room path that daWZ_Create 024E6BEC looks up from mpPath (wz_getNextRoomPath); weapon_shoot 024E3624 summons nothing while it is 0; wz::ActionFields (our decomp's view) already names it mpNextRoomPath |
| 0x8F0 | `m7D8` | `mBallSph` | daWZ_Create 024E6BEC sets it up as the magic ball's sphere (AT type 0x400); action_tama_dousa (in daWZ_Execute 024E398C) checks its AT / TG hits and takes the hit position (mBallHitPos) |

Extent fix (own commit): `m4F0` + the padding `_4F2[8]` -> `mTimers[5]` (0x4F0..0x4F9), the extent and name that wz::ActionFields already has: daWZ_Execute 024E398C counts all 5 down, the actions use them through the view (daWZ_Create 024E6BEC sets [2] = 300). Layout unchanged; d_a_wz object-identical.

## gm_class (Mothula and its larva; d_a_gm.h)

Offset-named gm_class fields. zeldaret/tww d217e2b include/d/actor/d_a_gm.h has only m2CD and m31E (GameCube offset names, behaviour stubs); TP has no gm_class. m3DC / m3E0 and m43E / m442 are the y / z and x / z members of the union struct views of mLineEnd and mGroundAngle (named after their peers mLineEndX / mGroundYaw); every name is from the use or that structure.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x3E9 | `m2CD` | `mLarvaType` | daGM_Create 021507C4: param byte 1 (0xFF -> 0); 0 is Mothula itself, not 0 a larva: nodeCallBack 0214BB1C, draw_SUB 0214BDFC, daGM_Draw 0214BF60, body_atari_check 0214C2F4, daGM_Execute 0214D128 and useHeapInit 02150124 branch on it; action_hane_rakka (in daGM_Execute) treats type 2 apart |
| 0x43A | `m31E` | `mKsCount` | ks_set_rtn 0214CFA4: number of Morths (process 0xCD, fpcNm_KS_e) this Mothula has created; no more once it reaches 10 (mType 1) / 20 |
| 0x55A | `m43E` | `mGroundPitch` | x of mGroundAngle (the union's struct view, next to mGroundYaw): draw_SUB 0214BDFC, Line_check 0214CDB4 and gm_groundProbe (in action_ground_attack 02151B98) rotate by it in X; action_ground_attack clears it |
| 0x55E | `m442` | `mGroundRoll` | z of mGroundAngle (the union's struct view, next to mGroundYaw): draw_SUB 0214BDFC, Line_check 0214CDB4 and gm_groundProbe (in action_ground_attack 02151B98) rotate by it in Z; action_ground_attack clears it |
| 0x4F8 | `m3DC` | `mLineEndY` | y of mLineEnd (the union's struct view, after mLineEndX); no use by this name |
| 0x4FC | `m3E0` | `mLineEndZ` | z of mLineEnd (the union's struct view, after mLineEndX); no use by this name |

Misnamed-member correction (own commit): the mLineStart union's struct view named its y member (0x4EC) `z`; it is now `mLineStartY` (between mLineStartX and mLineStartZ). No code used the name; d_a_gm object-identical.

## am_class (Armos; d_a_am.h)

Offset-named am_class fields (zeldaret/tww d217e2b include/d/actor/d_a_am.h keeps the GameCube offset names m033C / m0340; TP has no am_class; the names are from the use).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x458 | `m033C` | `mpBombEyeEmitter` | action_itai_move (in daAM_Execute 0204938C) creates the particle ID_AK_SN_AMOTHBOMBEYE at mJawPos and attaches it to the jaw joint (AM_JNT_AGO_e); daAM_Execute stops it (max frame -1, status 1) at the explosion (JA_SE_CM_AM_BEF_EXPLODE), daAM_Delete 0204B430 invalidates it |
| 0x45C | `m0340` | `mpBombMouthEmitter` | action_itai_move (in daAM_Execute 0204938C) creates the particle ID_AK_SN_AMOTHBOMBMOUTH at mJawPos and attaches it to the jaw joint (AM_JNT_AGO_e); daAM_Execute stops it (max frame -1, status 1) at the explosion (JA_SE_CM_AM_BEF_EXPLODE), daAM_Delete 0204B430 invalidates it |

## shand_class (Kokiie's hand tentacle; d_a_shand.h)

zeldaret/tww d217e2b include/d/actor/d_a_shand.h names these fields after their GameCube offsets (field_2BA .. field_31C); TP has no shand_class. The names are from the use in WWHD (normal, control1 / control2, set_thickness, cut_control and hand_move are inlined into daShand_Execute 0246FB78), with tww's d_a_shand.cpp as the matching reading. The view KokiieShand in d_a_kokiie.h (and its uses in himo_create, Kokiie 021A6418) is renamed with the class. unused_2B6 / unused_2E0 keep their names (no use).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D6 | `field_2BA` | `mTipMode` | normal (in daShand_Execute 0246FB78): 0 while the tip (field_31C[19]) moves to mTipTargetPos, 1 once it is within 10 (then it tells the parent *mpParentState = 2) and keeps the target at mTipGroundPos; daShand_Create 02470E24 sets 1, cut_control (regrow) and hand_move (parent dead / cut) set 0 |
| 0x3D8 | `field_2BC` | `mTimers` | daShand_Execute 0246FB78 counts all 4 down; hand_move (in daShand_Execute) sets [0] (regrow wait, l_HIO) and [1] (no new cut) after a cut and checks them |
| 0x3E0 | `field_2C4` | `mNoHitTimer` | hand_move (in daShand_Execute 0246FB78) checks the mCylArr / mSph TG hits only while it is 0 and sets 10 after a hit; daShand_Execute counts it down; daShand_Create 02470E24 sets 30 |
| 0x3E4 | `field_2C8` | `mTipGroundPos` | normal (in daShand_Execute 0246FB78): the ground point under the tip found by the line check while mGroundSetTimer runs; the tip goes back there (mTipTargetPos = it) and its height below home sets mLinkLen |
| 0x3F0 | `field_2D4` | `mTipTargetPos` | control1 (in daShand_Execute 0246FB78) moves the tip link (field_31C[19]) toward it with step mTipSpeedRate * 50; normal sets it to mTipGroundPos |
| 0x40C | `field_2F0` | `mTipSpeedRate` | control1 (in daShand_Execute 0246FB78): rate of the tip's move to mTipTargetPos (step * 50), eased up to 1; daShand_Create 02470E24 sets 1, cut_control resets it to 0 when the hand regrows |
| 0x410 | `field_2F4` | `mLinkLen` | control1 / control2 (in daShand_Execute 0246FB78): z of the per-link offset (the link length); normal eases it to the tip's depth below home * 0.05, cut_control to 20 |
| 0x414 | `field_2F8` | `mBendY` | control1 / control2 (in daShand_Execute 0246FB78): y of the per-link bend offset, eased to a REG value (0 / -5); a cut sets 3 |
| 0x418 | `field_2FC` | `mBendZ` | control1 / control2 (in daShand_Execute 0246FB78): z of the per-link bend offset, eased to a REG value (0 / 20); a cut sets 40 |
| 0x41C | `field_300` | `mWobbleAmp` | control1 / control2 (in daShand_Execute 0246FB78): amplitude of the sine / cosine wobble of the links (phase mExecuteCount); normal eases it to 10, control2 to 0, a cut sets 30..50 |
| 0x420 | `field_304` | `mThickness` | set_thickness (in daShand_Execute 0246FB78): base thickness of each link (shand_s field_18), thinner toward the end; daShand_Create 02470E24 sets 15.75 / 10.5 by type |
| 0x424 | `field_308` | `mParentId` | the parent Kokiie's process id: himo_create (in Kokiie 021A6418) sets it; hand_move (in daShand_Execute 0246FB78) looks the parent up by it (mpParent) |
| 0x428 | `field_30C` | `mpParent` | hand_move (in daShand_Execute 0246FB78): the parent actor found by mParentId; the hand follows its position / shape angle and is cut when its health is 0 |
| 0x42C | `field_310` | `mpRootPos` | himo_create (in Kokiie 021A6418) points it at the parent's anchor position (kokiie mRopeEndPos[i]); hand_move (in daShand_Execute 0246FB78) puts the hand's root there |
| 0x430 | `field_314` | `mpParentState` | himo_create (in Kokiie 021A6418) points it at the parent's per-hand state byte (kokiie mRopeState[i]); the hand writes 1 when it is cut and 2 when its tip is back in place |
| 0x434 | `field_318` | `mGroundSetTimer` | daShand_Create 02470E24 sets 10; normal (in daShand_Execute 0246FB78) line-checks the ground under the tip and sets mTipGroundPos / mTipTargetPos / the tip position each frame while it counts down |
| 0x438 | `field_31C` | `mLinks` | the 20 shand_s links of the hand (position, thickness field_18): control1 / control2 / set_thickness (in daShand_Execute 0246FB78) move them, [0] is put at the root, [19] is the tip |

### shand_s (one link of the hand; d_a_shand.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x18 | `field_18` | `mThickness` | set_thickness (in daShand_Execute 0246FB78) sets it to shand_class mThickness for links 0..11 and thinner toward the tip for 12..19; hand_move (in daShand_Execute 0246FB78) writes it (ftoi) as the link's line width into mLineMat. tww d217e2b src/d/actor/d_a_shand.cpp keeps the offset name field_18 (same uses, lines 113 / 116 / 285) |


## daShand_HIO_c (l_HIO, the hand tentacle tuning; d_a_shand.h)

Round 10. WWHD keeps the vtable last (field_6 / field_8 are tww's names at the GameCube offsets 6 / 8). tww's daShand_HIO_c (d_a_shand.cpp) reads them the same way.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x02 | `field_6` | `mEyeSegment` | daShand_HIO_c_ct sets -3; hand_move 0246FAEC: eyePos is the hand line segment it + 10 (tww the same) |
| 0x04 | `field_8` | `mHitHoldTime` | daShand_HIO_c_ct sets 50; hand_move 0246FAEC: mTimers[0] after the hand hits while it has a parent, mTimers[1] it + 90 (tww field_2BC[0] the same) |

## mt_class (Magtail; d_a_mt.h, with the views MtExec::Actor and mtBodyActorView)

zeldaret/tww d217e2b names mt_class's fields after their GameCube offsets; TP has no mt_class. The names are those of the two views of the same object (MtExec::Actor in d_a_mt_exec.h, mtBodyActorView in d_a_mt_body.h), checked against the uses in d_a_mt.cpp / d_a_mt_exec.cpp (mt_move, mt_fight, damage_check, mt_action, body_control1 are inlined into daMt_Execute 021D87A0) / d_a_mt_body.cpp. Where the views disagreed, the name with the stronger use evidence was taken and the losing view name was corrected in a separate commit. Left open: m454 (no name in any view; four uses).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D0 | `m3D0` | `mType` | MtExec::Actor mType (d_a_mt_exec.h, same offset); uses: daMt_Create 021DCE84; mt_move (in daMt_Execute 021D87A0); daMt_Execute 021D87A0 |
| 0x3D1 | `m3D1` | `mSnapTurn` | MtExec::Actor mSnapTurn (d_a_mt_exec.h, same offset); uses: daMt_Create 021DCE84; mt_move (in daMt_Execute 021D87A0) |
| 0x3D2 | `m3D2` | `mUseAngleParam` | MtExec::Actor mUseAngleParam (d_a_mt_exec.h, same offset); uses: bakuha 021D8530; daMt_Delete 021DC634; daMt_Create 021DCE84 |
| 0x3D3 | `m3D3` | `mPathIndex` | MtExec::Actor mPathIndex (d_a_mt_exec.h, same offset); uses: daMt_Create 021DCE84 |
| 0x3D4 | `m3D4` | `mSwitch` | MtExec::Actor mSwitch (d_a_mt_exec.h, same offset); uses: daMt_Create 021DCE84 |
| 0x3D5 | `m3D5` | `mDeathSwitch` | MtExec::Actor mDeathSwitch (d_a_mt_exec.h, same offset); uses: bakuha 021D8530; daMt_Create 021DCE84; daMt_Execute 021D87A0 |
| 0x3D6 | `m3D6` | `mAngleParam` | MtExec::Actor mAngleParam (d_a_mt_exec.h, same offset); uses: daMt_Delete 021DC634; daMt_Create 021DCE84 |
| 0x3D7 | `m3D7` | `mAppearSwitch` | MtExec::Actor mAppearSwitch (d_a_mt_exec.h, same offset); uses: daMt_Draw 021D7FFC; daMt_Create 021DCE84; daMt_Execute 021D87A0 |
| 0x3D8 | `m3D8` | `mHasPath` | MtExec::Actor mHasPath (d_a_mt_exec.h, same offset); uses: daMt_Create 021DCE84; mt_move (in daMt_Execute 021D87A0) |
| 0x3D9 | `m3D9` | `mPathPoint` | MtExec::Actor mPathPoint (d_a_mt_exec.h, same offset); uses: mt_move (in daMt_Execute 021D87A0) |
| 0x3DA | `m3DA` | `mPathDirection` | MtExec::Actor mPathDirection (d_a_mt_exec.h, same offset); uses: daMt_Create 021DCE84; mt_move (in daMt_Execute 021D87A0) |
| 0x400 | `m400` | `mMaterialMode` | MtExec::Actor mMaterialMode (d_a_mt_exec.h, same offset): selects the brk / btk frames in daMt_Execute (0 per segment, else shared); mtBodyActorView called it mTextureFrameMode; uses: daMt_Draw 021D7FFC; damage_check (in daMt_Execute 021D87A0); mt_action (in daMt_Execute 021D87A0); daMt_Execute 021D87A0; mt_move_maru 021DF238 |
| 0x404 | `m404` | `mBrkFrame` | MtExec::Actor mBrkFrame (d_a_mt_exec.h, same offset); uses: daMt_Draw 021D7FFC; daMt_Execute 021D87A0 |
| 0x408 | `m408` | `mBtkFrame` | MtExec::Actor mBtkFrame (d_a_mt_exec.h, same offset); uses: daMt_Draw 021D7FFC; daMt_Execute 021D87A0 |
| 0x44C | `m44C` | `mBodyWaveAmplitude` | MtExec::Actor mBodyWaveAmplitude; mtBodyActorView mBodyWaveAmplitude (d_a_mt_exec.h / d_a_mt_body.h, same offset); uses: mt_fight (in daMt_Execute 021D87A0); body_control3 021DE1E0; mt_move_maru 021DF238 |
| 0x450 | `m450` | `mJointWaveStep` | MtExec::Actor mJointWaveStep (d_a_mt_exec.h, same offset); uses: daMt_Execute 021D87A0 |
| 0x464 | `m464` | `mWaterSteamState` | mtBodyActorView mWaterSteamState (d_a_mt_body.h, same offset): water_damage_se_set 021D86F8 sets 1, mt_move_maru starts the steam emitter at 1 and sets 2, ends at 0 (MtExec::Actor called it mWaterSoundPlayed); uses: water_damage_se_set 021D86F8; mt_move_maru 021DF238 |
| 0x466 | `m466` | `mWaterSteamTimer` | mtBodyActorView mWaterSteamTimer (d_a_mt_body.h, same offset): mt_move_maru sets 80 and fades the steam by it, daMt_Execute counts it down (MtExec::Actor: mSteamTimer); uses: daMt_Execute 021D87A0; mt_move_maru 021DF238 |
| 0x468 | `m468` | `mFollowParticlesStarted` | MtExec::Actor mFollowParticlesStarted (d_a_mt_exec.h, same offset); uses: daMt_Execute 021D87A0 |
| 0x46C | `m46C` | `mParticlePos` | MtExec::Actor mParticlePos (d_a_mt_exec.h, same offset); uses: daMt_Execute 021D87A0 |
| 0x570 | `m570` | `mAction` | MtExec::Actor mAction; mtBodyActorView mAction (d_a_mt_exec.h / d_a_mt_body.h, same offset); uses: mt_a_d_sub 021D7480; bakuha 021D8530; mt_move (in daMt_Execute 021D87A0); mt_fight (in daMt_Execute 021D87A0); damage_check (in daMt_Execute 021D87A0); mt_action (in daMt_Execute 021D87A0); daMt_Execute 021D87A0; body_control2 021DD634 |
| 0x571 | `m571` | `mMode` | MtExec::Actor mMode (d_a_mt_exec.h, same offset): the sub-mode of the current action: switched on in mt_move / mt_fight / damage_check / mt_action and mt_move_maru (mtBodyActorView: mRollMode); uses: mt_move (in daMt_Execute 021D87A0); mt_fight (in daMt_Execute 021D87A0); damage_check (in daMt_Execute 021D87A0); mt_action (in daMt_Execute 021D87A0); body_control2 021DD634; mt_move_maru 021DF238 |
| 0x572 | `m572` | `mActionTimer` | MtExec::Actor mActionTimer (d_a_mt_exec.h, same offset): first of the five timers daMt_Execute counts down: the per-action timer of mt_move / mt_fight (from l_HIO), mt_move_maru's collision wait (mtBodyActorView: mRollCollisionTimer); uses: mt_move (in daMt_Execute 021D87A0); mt_fight (in daMt_Execute 021D87A0); damage_check (in daMt_Execute 021D87A0); mt_action (in daMt_Execute 021D87A0); daMt_Execute 021D87A0; mt_move_maru 021DF238 |
| 0x574 | `m574` | `mPauseTimer` | MtExec::Actor mPauseTimer (d_a_mt_exec.h, same offset): second counted-down timer: mt_move restarts it at 150..250 and starts a wave pause (mWavePauseTimer); body_control2 sets 100 (mtBodyActorView: mActionTimer); uses: mt_move (in daMt_Execute 021D87A0); body_control2 021DD634 |
| 0x576 | `m576` | `mDeleteTimer` | MtExec::Actor mDeleteTimer (d_a_mt_exec.h, same offset); uses: bakuha 021D8530; mt_action (in daMt_Execute 021D87A0) |
| 0x578 | `m578` | `mVoiceTimer` | MtExec::Actor mVoiceTimer (d_a_mt_exec.h, same offset); uses: daMt_Execute 021D87A0 |
| 0x57A | `m57A` | `mMoveSoundTimer` | MtExec::Actor mMoveSoundTimer (d_a_mt_exec.h, same offset); uses: daMt_Execute 021D87A0 |
| 0x57C | `m57C` | `mInvulnerableTimer` | MtExec::Actor mInvulnerableTimer (d_a_mt_exec.h, same offset): damage_check ignores hits while it runs and sets 5 after a hit; body_control2 shrinks the segment colliders while it runs (mtBodyActorView: mShrinkColliderTimer); uses: body_control1 (in daMt_Execute 021D87A0); mt_fall (in mt_fight, in daMt_Execute 021D87A0); damage_check (in daMt_Execute 021D87A0); mt_action (in daMt_Execute 021D87A0); daMt_Execute 021D87A0; body_control2 021DD634 |
| 0x57E | `m57E` | `mShakeTimer` | MtExec::Actor mShakeTimer (d_a_mt_exec.h, same offset); uses: mt_fight (in daMt_Execute 021D87A0); damage_check (in daMt_Execute 021D87A0); daMt_Execute 021D87A0 |
| 0x580 | `m580` | `mBlinkTimer` | MtExec::Actor mBlinkTimer (d_a_mt_exec.h, same offset): mt_eye_tex_anm counts it down and blinks at 0 (reload 50..150); a hit and mt_move_maru set 3 (mtBodyActorView: mRollingStateTimer); uses: mt_eye_tex_anm (in daMt_Execute 021D87A0); mt_action (in daMt_Execute 021D87A0); mt_move_maru 021DF238 |
| 0x582 | `m582` | `mRollTimer` | MtExec::Actor mRollTimer (d_a_mt_exec.h, same offset): mt_fight sets the roll duration (l_HIO), daMt_Execute counts it down, mt_move_maru / body_control3 use its value as the roll's progress (mtBodyActorView: mCrawlPhase; body_control3 runs in the roll); uses: mt_fight (in daMt_Execute 021D87A0); mt_action (in daMt_Execute 021D87A0); daMt_Execute 021D87A0; body_control3 021DE1E0; mt_move_maru 021DF238 |
| 0x584 | `m584` | `mHeadWobbleAngle` | MtExec::Actor mHeadWobbleAngle; mtBodyActorView mHeadWobbleAngle (d_a_mt_exec.h / d_a_mt_body.h, same offset); uses: body_control1 (in daMt_Execute 021D87A0); mt_move (in daMt_Execute 021D87A0); mt_fight (in daMt_Execute 021D87A0); body_control2 021DD634; body_control5 021DEBC8; mt_move_maru 021DF238 |
| 0x586 | `m586` | `mFrameCounter` | MtExec::Actor mFrameCounter (d_a_mt_exec.h, same offset): daMt_Execute increments it every frame; the phase of the shake and of the body waves (mtBodyActorView: mBodyWavePhase); uses: daMt_Create 021DCE84; mt_move (in daMt_Execute 021D87A0); mt_fight (in daMt_Execute 021D87A0); daMt_Execute 021D87A0; body_control2 021DD634; body_control5 021DEBC8 |
| 0x588 | `m588` | `mWallLostTimer` | MtExec::Actor mWallLostTimer (d_a_mt_exec.h, same offset); uses: mt_move (in daMt_Execute 021D87A0) |
| 0x58A | `m58A` | `mJointWavePhase` | MtExec::Actor mJointWavePhase (d_a_mt_exec.h, same offset); uses: mt_action (in daMt_Execute 021D87A0); daMt_Execute 021D87A0 |
| 0x58C | `m58C` | `mModelOffsetZ` | mtBodyActorView mModelOffsetZ (d_a_mt_body.h, same offset): transM(0, 0, it) of the segment models in body_control1/2/3/5, eased to 20 / -10 (MtExec::Actor: mBodyOffset); uses: body_control1 (in daMt_Execute 021D87A0); mt_move (in daMt_Execute 021D87A0); body_control2 021DD634; body_control3 021DE1E0; body_control5 021DEBC8 |
| 0x590 | `m590` | `mBodyWaveScale` | mtBodyActorView mBodyWaveScale (d_a_mt_body.h, same offset): scales the sine waves of the body in body_control3 / 5; damage_check sets 0.2 / 1.5, mt_move_maru eases it (MtExec::Actor: mWobbleAmplitude); uses: mt_action (in daMt_Execute 021D87A0); body_control3 021DE1E0; body_control5 021DEBC8; mt_move_maru 021DF238 |
| 0x594 | `m594` | `mRollBodyOffset` | MtExec::Actor mRollBodyOffset (d_a_mt_exec.h, same offset): body_control3 (the roll's body control) spaces the segments by it, mt_move_maru eases it to 1000, mt_fight clears it at the roll start (mtBodyActorView: mCrawlSegmentOffset); uses: mt_fight (in daMt_Execute 021D87A0); body_control3 021DE1E0; mt_move_maru 021DF238 |
| 0x598 | `m598` | `mTargetPos` | MtExec::Actor mTargetPos (d_a_mt_exec.h, same offset); uses: daMt_Create 021DCE84; mt_set_back_pos (in daMt_Execute 021D87A0); mt_move (in daMt_Execute 021D87A0); mt_fight (in daMt_Execute 021D87A0) |
| 0x5A4 | `m5A4` | `mTurnSpeed` | MtExec::Actor mTurnSpeed (d_a_mt_exec.h, same offset); uses: daMt_Create 021DCE84; mt_move (in daMt_Execute 021D87A0) |
| 0x5A6 | `m5A6` | `mRollPitchStep` | MtExec::Actor mRollPitchStep (d_a_mt_exec.h, same offset): body_control3 subtracts it per segment from the pitch, mt_move_maru eases it, mt_fight clears it at the roll start (mtBodyActorView: mSegmentPitchStep); uses: mt_fight (in daMt_Execute 021D87A0); body_control3 021DE1E0; mt_move_maru 021DF238 |
| 0x5A8 | `m5A8` | `mWallAngle` | MtExec::Actor mWallAngle (d_a_mt_exec.h, same offset); uses: mt_move (in daMt_Execute 021D87A0) |
| 0x5AA | `m5AA` | `mWavePauseTimer` | MtExec::Actor mWavePauseTimer (d_a_mt_exec.h, same offset): mt_move sets 50..100 (a pause in the sideways wave), daMt_Execute counts it down and grows mWaveExtent while it runs; body_control2 waves only while it is 0 (mtBodyActorView: mSuppressBodyWave); uses: body_control1 (in daMt_Execute 021D87A0); mt_move (in daMt_Execute 021D87A0); mt_fight (in daMt_Execute 021D87A0); mt_action (in daMt_Execute 021D87A0); daMt_Execute 021D87A0; body_control2 021DD634 |
| 0x5AC | `m5AC` | `mWaveExtent` | MtExec::Actor mWaveExtent (d_a_mt_exec.h, same offset); uses: daMt_Execute 021D87A0 |
| 0x5AE | `m5AE` | `mClimbTimer` | MtExec::Actor mClimbTimer (d_a_mt_exec.h, same offset); uses: mt_move (in daMt_Execute 021D87A0) |
| 0x5B0 | `m5B0` | `mPlayerPitch` | MtExec::Actor mPlayerPitch (d_a_mt_exec.h, same offset); uses: mt_fight (in daMt_Execute 021D87A0) |
| 0x5B2 | `m5B2` | `mPlayerYaw` | MtExec::Actor mPlayerYaw (d_a_mt_exec.h, same offset); uses: mt_fight (in daMt_Execute 021D87A0) |
| 0x5B6 | `m5B6` | `mShakeAngle` | MtExec::Actor mShakeAngle (d_a_mt_exec.h, same offset); uses: mt_fight (in daMt_Execute 021D87A0) |
| 0x5BC | `m5BC` | `mSegmentPos` | MtExec::Actor mSegmentPos (d_a_mt_exec.h, same offset); uses: body_wall_check 021D7E80; body_control1 (in daMt_Execute 021D87A0); body_control2 021DD634; body_control3 021DE1E0; body_control4 021DE888; body_control5 021DEBC8 |
| 0x61C | `m61C` | `mOldSegmentPos` | MtExec::Actor mOldSegmentPos (d_a_mt_exec.h, same offset); uses: body_wall_check 021D7E80; body_control2 021DD634; body_control3 021DE1E0 |
| 0x67C | `m67C` | `mSegmentAngle` | MtExec::Actor mSegmentAngle (d_a_mt_exec.h, same offset); uses: body_control1 (in daMt_Execute 021D87A0); body_control2 021DD634; body_control3 021DE1E0; body_control4 021DE888; body_control5 021DEBC8 |
| 0x6AC | `m6AC` | `mSegmentSpeeds` | mtBodyActorView mSegmentSpeeds (d_a_mt_body.h, same offset): per-segment speeds: bakuha sets random scatter speeds, body_control2 / 4 move the segments by them (MtExec::Actor: mScatterSpeed); uses: bakuha 021D8530; body_control2 021DD634; body_control4 021DE888 |
| 0x70C | `m70C` | `mScatterDelay` | mtBodyActorView mScatterDelay (d_a_mt_body.h, same offset): bakuha sets a per-segment delay, body_control4 counts it down before the segment flies (MtExec::Actor: mSegmentPhase); uses: bakuha 021D8530; body_control4 021DE888 |
| 0x71C | `m71C` | `mSegmentScale` | MtExec::Actor mSegmentScale (d_a_mt_exec.h, same offset); uses: CallbackCreateHeap 021DC780; body_control1 (in daMt_Execute 021D87A0); body_control2 021DD634; body_control3 021DE1E0; body_control4 021DE888; body_control5 021DEBC8 |
| 0x73C | `m73C` | `mSegmentScaleY` | MtExec::Actor mSegmentScaleY; mtBodyActorView mSegmentScaleY (d_a_mt_exec.h / d_a_mt_body.h, same offset); uses: body_control1 (in daMt_Execute 021D87A0); daMt_Execute 021D87A0; body_control2 021DD634; body_control3 021DE1E0; body_control5 021DEBC8 |
| 0x75C | `m75C` | `mJointAnglesLeft` | MtExec::Actor mJointAnglesLeft (d_a_mt_exec.h, same offset); uses: nodeCallBack_head 021D7668; nodeCallBack_body 021D78F4; nodeCallBack_tail 021D7CC4; daMt_Execute 021D87A0 |
| 0x7B6 | `m7B6` | `mJointAnglesRight` | MtExec::Actor mJointAnglesRight (d_a_mt_exec.h, same offset); uses: nodeCallBack_head 021D7668; nodeCallBack_body 021D78F4; nodeCallBack_tail 021D7CC4 |
| 0x810 | `m810` | `mTrailPos` | MtExec::Actor mTrailPos (d_a_mt_exec.h, same offset); uses: daMt_Create 021DCE84; body_control1 (in daMt_Execute 021D87A0); mt_move (in daMt_Execute 021D87A0); body_control2 021DD634 |
| 0xB10 | `mB10` | `mTrailAngle` | MtExec::Actor mTrailAngle (d_a_mt_exec.h, same offset); uses: daMt_Create 021DCE84; body_control1 (in daMt_Execute 021D87A0); mt_move (in daMt_Execute 021D87A0); body_control2 021DD634 |
| 0xC90 | `mC90` | `mTrailHeadAngle` | MtExec::Actor mTrailHeadAngle (d_a_mt_exec.h, same offset); uses: body_control1 (in daMt_Execute 021D87A0) |
| 0xD10 | `mD10` | `mTrailIndex` | MtExec::Actor mTrailIndex (d_a_mt_exec.h, same offset): ring index (& 0x3F) into mTrailPos / mTrailAngle / mTrailHeadAngle, advanced by mt_move (mtBodyActorView: mTrailState); uses: body_control1 (in daMt_Execute 021D87A0); body_control2 021DD634 |
| 0xD1C | `mD1C` | `mFastTrail` | MtExec::Actor mFastTrail (d_a_mt_exec.h, same offset): picks the fast trail offset table (mt_trail_ofs_fast) in body_control1 / body_control2 (mtBodyActorView: mTrailTableSelect); uses: body_control1 (in daMt_Execute 021D87A0); mt_move (in daMt_Execute 021D87A0); body_control2 021DD634 |
| 0xD1D | `mD1D` | `mStoreTrail` | MtExec::Actor mStoreTrail (d_a_mt_exec.h, same offset); uses: body_control1 (in daMt_Execute 021D87A0); mt_move (in daMt_Execute 021D87A0); mt_fight (in daMt_Execute 021D87A0); body_control2 021DD634 |
| 0xD1E | `mD1E` | `mAttackFrame` | MtExec::Actor mAttackFrame (d_a_mt_exec.h, same offset); uses: mt_fight (in daMt_Execute 021D87A0) |
| 0xD20 | `mD20` | `mHeadColliderMode` | MtExec::Actor mHeadColliderMode; mtBodyActorView mHeadColliderMode (d_a_mt_exec.h / d_a_mt_body.h, same offset); uses: mt_fight (in daMt_Execute 021D87A0); damage_check (in daMt_Execute 021D87A0); mt_action (in daMt_Execute 021D87A0); body_control2 021DD634 |
| 0x19F0 | `m19F0` | `mRubbleFrame` | MtExec::Actor mRubbleFrame (d_a_mt_exec.h, same offset); uses: br_draw (in daMt_Draw 021D7FFC); daMt_Draw 021D7FFC; mt_fight (in daMt_Execute 021D87A0); damage_check (in daMt_Execute 021D87A0); daMt_Execute 021D87A0 |
| 0x1A10 | `m1A10` | `mSegmentSpeedScale` | mtBodyActorView mSegmentSpeedScale (d_a_mt_body.h, same offset): body_control2 scales the segment speeds by it and eases it to 0; mt_fight eases it to 0.9 (MtExec::Actor: mKnockbackRate); uses: mt_fight (in daMt_Execute 021D87A0); body_control2 021DD634; body_control3 021DE1E0 |
| 0x1A14 | `m1A14` | `mDamageMode` | MtExec::Actor mDamageMode (d_a_mt_exec.h, same offset): damage_check sets 1 / 2 / 3 by the hit, daMt_Execute / mt_fight act on >= 2, mt_move_maru sets 2 (mtBodyActorView: mImpactState); uses: bakuha 021D8530; mt_fight (in daMt_Execute 021D87A0); damage_check (in daMt_Execute 021D87A0); daMt_Execute 021D87A0; mt_move_maru 021DF238 |
| 0x1A15 | `m1A15` | `mPlayerOccluded` | MtExec::Actor mPlayerOccluded (d_a_mt_exec.h, same offset); uses: mt_move (in daMt_Execute 021D87A0); mt_fight (in daMt_Execute 021D87A0); mt_action (in daMt_Execute 021D87A0) |
| 0x1A16 | `m1A16` | `mInLava` | MtExec::Actor mInLava (d_a_mt_exec.h, same offset): daMt_Execute sets 1 while the lava ground check finds lava under the actor, 0 otherwise (mtBodyActorView: mSuppressUnrollHop); uses: mt_action (in daMt_Execute 021D87A0); daMt_Execute 021D87A0; mt_move_maru 021DF238 |
| 0x1A17 | `m1A17` | `mHeadHealth` | MtExec::Actor mHeadHealth (d_a_mt_exec.h, same offset); uses: daMt_Create 021DCE84; damage_check (in daMt_Execute 021D87A0); mt_action (in daMt_Execute 021D87A0) |
| 0x1A18 | `m1A18` | `mHitEffectTimer` | MtExec::Actor mHitEffectTimer (d_a_mt_exec.h, same offset): damage_check sets 12, daMt_Execute counts it down; body_control3 flickers the pitch by (it & 2) * 500 (mtBodyActorView: mCrawlPitchFlags); uses: damage_check (in daMt_Execute 021D87A0); daMt_Execute 021D87A0; body_control3 021DE1E0 |
| 0x1DD4 | `m1DD4` | `mDeleteRequested` | MtExec::Actor mDeleteRequested (d_a_mt_exec.h, same offset); uses: mt_bg_check 021D74F8; daMt_Delete 021DC634; damage_check (in daMt_Execute 021D87A0); mt_action (in daMt_Execute 021D87A0) |


## daMt_HIO_c (l_HIO, the Magtail tuning; d_a_mt.h)

Round 10. Only these two of the 29 offset-named fields are read; the tww daMt_HIO_c (d_a_mt.cpp) is never read there.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x1C | `m1C` | `mRubbleScale` | br_draw 021D7E80: the rubble (br) model scale is it * 2; the constructor sets 0.8 |
| 0x50 | `m50` | `mSegmentFrameStep` | daMt_Draw 021D7FFC: segment i plays the brk / btk at mBrkFrame / mBtkFrame + i * it; the constructor sets -5 |

Open (no name yet): the other 27 offset-named fields (`m05`-`m18`, `m20`-`m4C`, `m52`-`m5C`): the constructor sets them, nothing reads them in WWHD or tww.

## damagereaction (shared enemy damage reaction; c_damagereaction.h, with its copies / views)

zeldaret/tww d217e2b include/c/c_damagereaction.h names these fields after their GameCube offsets (m010 .. m7B8); TP has no damagereaction. The names are from the use in WWHD: the shared code (dr_body_bg_check, dr_joint_bg_check and hang_ang_get are inlined into dr_damage_set 02043F34; dr_damage_anime and dr_matrix_set into damage_reaction 02041F94) and the Bokoblin / Moblin / Darknut / Kargaroc / bridge code that fills it. "Joint n" is the index into the per-joint arrays (the enemies map their model joints to it). The same names are used in the copies and views: damagereaction_l in d_a_bk.h, d_a_mo2_main.h, d_a_mo2.h, d_a_mo2_exec.h, damagereaction in d_a_mo2_move.h, BridgeHD::BokoReaction in d_a_bridge.h and BbDamageReaction in d_a_bb.h. Left open: m2E0 and m6F4 (no use), m438 (only cleared by daBk_Execute / daMo2_Execute), m712 (read only: dr_damage_set 02043F34 and ground_smoke_set 020A070C; nothing sets it).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x010 | `m010` | `mJntTargetAngle` | dr_damage_anime (in damage_reaction 02041F94) clears it each frame and builds the per-joint pose angles in it (knock-down pose, limb offsets, head look, shake); mJntAngle eases toward it |
| 0x08E | `m088` | `mJntAngle` | dr_damage_anime (in damage_reaction 02041F94) eases it toward mJntTargetAngle (cLib_addCalcAngleS2); nodeCallBack (bk 020991CC, mo2 021C6854) applies it as the joint's Y/X/Z rotation |
| 0x10C | `m100` | `mJntPos` | nodeCallBack / nodeCallBack_P (bk 020991CC / 02099584, mo2 021C6854 / 021C6BE0) store each joint's world position (MtxPosition); dr_joint_bg_check (in dr_damage_set 02043F34) line-checks between them |
| 0x208 | `m1F0` | `mJntGroundPos` | dr_joint_bg_check (in dr_damage_set 02043F34): mJntPos raised by 200, then y = cBgS_GroundCross (1e9 when no ground); compared with mJntPos.y |
| 0x400 | `m3D0` | `mJntGroundState` | dr_joint_bg_check (in dr_damage_set 02043F34) sets it for joints 0..3: 2 no ground below, 0 more than 200 above the ground, 1 near the ground; dr_damage_anime (in damage_reaction 02041F94) bends joints 0..3 only while it is 0 |
| 0x438 | `m408` | `mJnt10LineBlocked` | dr_joint_bg_check (in dr_damage_set 02043F34): 2 when the line from joint 10 toward joint 14 hits background at pitch 0 (mJnt10Pitch then from the free pitch), else 0; dr_damage_anime (in damage_reaction 02041F94) applies mJnt10Pitch only while it and mJntGroundState[2] are 0 |
| 0x43C | `m40C` | `mJnt11LineBlocked` | as mJnt10LineBlocked for the line joint 11 -> joint 15 (dr_joint_bg_check, in dr_damage_set 02043F34); dr_damage_anime (in damage_reaction 02041F94) checks it with mJntGroundState[3] |
| 0x440 | `m410` | `mJnt8LineBlocked` | as mJnt10LineBlocked for the line joint 8 -> joint 16 (dr_joint_bg_check, in dr_damage_set 02043F34): 2 blocked, 0 free |
| 0x444 | `m414` | `mJnt9LineBlocked` | as mJnt10LineBlocked for the line joint 9 -> joint 17 (dr_joint_bg_check, in dr_damage_set 02043F34): 2 blocked, 0 free |
| 0x448 | `m418` | `mJnt12LineBlocked` | as mJnt10LineBlocked for the line joint 12 -> joint 18 (dr_joint_bg_check, in dr_damage_set 02043F34); dr_damage_anime (in damage_reaction 02041F94) tests it == 2 |
| 0x454 | `m420` | `mJntBgCheckTimer` | dr_damage_set 02043F34 sets it on landing (REG0_S(3) + 1) and to 1 when the slide stops; dr_joint_bg_check (inlined there) runs the joint ground / line checks only while it is non-zero and counts it down |
| 0x458 | `m424` | `mHitFlags` | damage_check (in daBk_Execute 0209BD1C / daMo2_Execute 021C9B1C) ORs hit bits 0x10 / 0x20 / 0x40 into it; dr_damage_set 02043F34: 0x40 launches backward and up, 0x10 a flinch, non-zero with mKnockbackPower > 25 a knock-down; then cleared |
| 0x45C | `m428` | `mKnockbackPower` | damage_check (in daBk_Execute 0209BD1C / daMo2_Execute 021C9B1C) sets 23 / 26 or a random 90+ / 70+; dr_damage_set 02043F34 knocks the enemy down when it is > 25 and copies it into mKnockbackSpeed |
| 0x460 | `m42C` | `mHitVec` | damage_check (in daBk_Execute / daMo2_Execute) stores the hit direction (MtxPosition); dr_damage_set 02043F34 takes atan2(-x, -z) as the push-back direction, the knock-back yaw and the wall-smoke yaw |
| 0x474 | `m440` | `mJnt10Pitch` | dr_joint_bg_check (in dr_damage_set 02043F34): pitch of joint 10 toward joint 14 (free line pitch + 0x400, or the ground slope), clamped to -10000..7000; dr_damage_anime (in damage_reaction 02041F94) bends mJntTargetAngle[6].z by it |
| 0x476 | `m442` | `mJnt11Pitch` | as mJnt10Pitch for joint 11 -> joint 15 (dr_joint_bg_check, in dr_damage_set 02043F34); bends mJntTargetAngle[7].z (dr_damage_anime, in damage_reaction 02041F94) |
| 0x478 | `m444` | `mJnt8Pitch` | as mJnt10Pitch for joint 8 -> joint 16 (dr_joint_bg_check, in dr_damage_set 02043F34); bends mJntTargetAngle[4].z (dr_damage_anime, in damage_reaction 02041F94) |
| 0x47A | `m446` | `mJnt9Pitch` | as mJnt10Pitch for joint 9 -> joint 17 (dr_joint_bg_check, in dr_damage_set 02043F34); bends mJntTargetAngle[5].z (dr_damage_anime, in damage_reaction 02041F94) |
| 0x47C | `m448` | `mJnt12Pitch` | as mJnt10Pitch for joint 12 -> joint 18, clamped to +-13000 (dr_joint_bg_check, in dr_damage_set 02043F34); half of it bends joints 12, 13, 18, 19 (dr_damage_anime, in damage_reaction 02041F94) |
| 0x480 | `m44C` | `mPivot` | dr_matrix_set (in damage_reaction 02041F94) rotates the body about it (MtxTrans -it .. +it); dr_damage_set 02043F34 sets y = -125 on knock-down; dr_body_bg_check lowers the position by y for the ground check; hukki (bk 020A3048, mo2 021CE6E8) moves the actor by y and clears it |
| 0x48C | `m458` | `mBodyOffset` | dr_matrix_set (in damage_reaction 02041F94) adds it to current.pos for the model translation; hukki (mo2 021CE6E8) fills it and eases x / z to 0, yogan_fail (mo2 021CEEB0) clears y |
| 0x49C | `m468` | `mCarryOffsetY` | dr_matrix_set (in damage_reaction 02041F94): last translation MtxTrans(0, it, mCarryOffsetZ); daBb_Execute 0205EA58 sets it while a Kargaroc carries the enemy, carry_drop (bk 020A47B4, mo2 021D0294) eases it to 0 |
| 0x4A0 | `m46C` | `mCarryOffsetZ` | dr_matrix_set (in damage_reaction 02041F94): z of the same MtxTrans(0, mCarryOffsetY, it); daBb_Execute 0205EA58 sets it while carrying, carry_drop (bk 020A47B4, mo2 021D0294) eases it to 0 |
| 0x4A4 | `m470` | `mFlinchBend` | dr_damage_set 02043F34 sets 4000 for a flinch (hit bit 0x10); while > 200 dr_damage_anime (in damage_reaction 02041F94) bends the upper joints away from the hit direction by it and eases it to 0; p_lost (mo2 021CDE6C) waits for < 200 |
| 0x4A8 | `m474` | `mShakeAmplitude` | dr_damage_anime (in damage_reaction 02041F94): amplitude of the sin/cos joint shake (mTimer * 5000 / 4000) on the joints selected by mShakeJntMask; dr_damage_set 02043F34 sets 2000..9000 on hits and landings and eases it to 0 |
| 0x4AC | `m478` | `mKnockbackSpeed` | dr_damage_set 02043F34: the knock-back speed along mKnockbackAngleY (from mKnockbackPower, -7 / -20 for hit bit 0x40); speed.y is derived from it; a wall hit while it is non-zero bounces; cleared on wall or ground contact |
| 0x4B0 | `m47C` | `mLaunched` | dr_damage_set 02043F34 sets 1 at the knock-down launch, 0 on wall / ground contact (and in dr_body_bg_check); dr_damage_anime (in damage_reaction 02041F94) poses the flying body while it is set |
| 0x4B4 | `m480` | `mFallPitch` | dr_damage_set 02043F34 eases it toward mFallPitchTarget; dr_matrix_set (in damage_reaction 02041F94) and kado_check 02041D10 rotate the body by it about X (after mKnockbackAngleY); hukki (bk 020A3048, mo2 021CE6E8) clears it |
| 0x4B6 | `m482` | `mKnockbackAngleY` | dr_damage_set 02043F34: yaw of the knock-back (atan2 of mHitVec, +0x8000 for hit bit 0x40, or shape_angle.y); the speed is rotated by it and dr_matrix_set (in damage_reaction 02041F94) pitches the body about it |
| 0x4B8 | `m484` | `mSpinAngleY` | dr_damage_set 02043F34 steps it by +-0x1000 while mSpinLeft runs and adds it to current / shape angle.y at recovery; dr_matrix_set (in damage_reaction 02041F94) turns the body by it |
| 0x4BA | `m486` | `mFallPitchTarget` | dr_damage_set 02043F34 sets 0x4000 (falls over) / -0x4000 / 0; mFallPitch eases toward it |
| 0x4BC | `m488` | `mKnockedDown` | dr_damage_set 02043F34: 0 upright, set to 1 by a knock-down, cleared at recovery; hukki (bk 020A3048, mo2 021CE6E8) clears it; dr_damage_anime / dr_body_bg_check test == 1 |
| 0x4BE | `m48A` | `mHitStunTimer` | damage_check (in daBk_Execute / daMo2_Execute) sets 10 on a hit; daBk_Execute 0209BD1C / daMo2_Execute 021C9B1C count it down; Bk_move 020A5EF4 / Mo2_move 021D0A14 stagger while it runs and the enemy is not knocked down |
| 0x4C0 | `m48C` | `mSlopeTilt` | daBk_Execute 0209BD1C / daMo2_Execute 021C9B1C ease x / z toward the ground slope angles; dr_matrix_set (in damage_reaction 02041F94) tilts the body by x / z |
| 0x4C6 | `m492` | `mJnt12SlopeY` | dr_damage_anime (in damage_reaction 02041F94): eased toward mSlopeTilt rotated into the enemy's frame (0 while knocked down) and subtracted from mJntTargetAngle[12].y |
| 0x4C8 | `m494` | `mJnt12SlopeZ` | dr_damage_anime (in damage_reaction 02041F94): z partner of mJnt12SlopeY, subtracted from mJntTargetAngle[12].z |
| 0x4CA | `m496` | `mJnt18AddY` | dr_damage_anime (in damage_reaction 02041F94) eases it toward mJnt18AddYTarget and adds it to mJntTargetAngle[18].y |
| 0x4CC | `m498` | `mJnt13AddY` | dr_damage_anime (in damage_reaction 02041F94) eases it toward mJnt13AddYTarget and adds it to mJntTargetAngle[13].y |
| 0x4CE | `m49A` | `mJnt18AddYTarget` | dr_damage_set 02043F34 sets a random +-18000 on landing, 0 at recovery; mJnt18AddY eases toward it |
| 0x4D0 | `m49C` | `mJnt13AddYTarget` | dr_damage_set 02043F34 sets a random +-10000 on landing, 0 at recovery; mJnt13AddY eases toward it |
| 0x4D2 | `m49E` | `mParalysisTimer` | d_mahi (bk 020A4B44, mo2 021CFBBC) sets 15; dr_damage_anime (in damage_reaction 02041F94) counts it down and shakes the head by a sine of it |
| 0x4D4 | `m4A0` | `mJnt0AddZ` | dr_damage_anime (in damage_reaction 02041F94) eases it toward mJnt0AddZTarget and adds it to mJntTargetAngle[0].z |
| 0x4D6 | `m4A2` | `mJnt1AddZ` | dr_damage_anime (in damage_reaction 02041F94) eases it toward mJnt1AddZTarget and adds it to mJntTargetAngle[1].z |
| 0x4D8 | `m4A4` | `mJnt2AddZ` | dr_damage_anime (in damage_reaction 02041F94) eases it toward mJnt2AddZTarget, adds it to mJntTargetAngle[2].z and half of it subtracts from [6].z |
| 0x4DA | `m4A6` | `mJnt3AddZ` | dr_damage_anime (in damage_reaction 02041F94) eases it toward mJnt3AddZTarget, adds it to mJntTargetAngle[3].z and half of it subtracts from [7].z |
| 0x4DC | `m4A8` | `mJnt0AddZTarget` | dr_damage_set 02043F34 sets a random -20000..0 at the launch / bounce, 0 on contact; mJnt0AddZ eases toward it |
| 0x4DE | `m4AA` | `mJnt1AddZTarget` | dr_damage_set 02043F34 sets a random -20000..0 at the launch / bounce, 0 on contact; mJnt1AddZ eases toward it |
| 0x4E0 | `m4AC` | `mJnt2AddZTarget` | dr_damage_set 02043F34 sets a random 0..20000 at the launch / bounce, 0 on contact; mJnt2AddZ eases toward it |
| 0x4E2 | `m4AE` | `mJnt3AddZTarget` | dr_damage_set 02043F34 sets a random 0..20000 at the launch / bounce, 0 on contact; mJnt3AddZ eases toward it |
| 0x4E4 | `m4B0` | `mJnt2LandZ` | dr_damage_set 02043F34 sets a random 0..20000 on landing (0 at recovery, in dr_joint_bg_check when face down); dr_damage_anime (in damage_reaction 02041F94) adds it to mJntTargetAngle[2].z |
| 0x4E6 | `m4B2` | `mJnt3LandZ` | dr_damage_set 02043F34 sets a random 0..20000 on landing (0 at recovery, in dr_joint_bg_check when face down); dr_damage_anime (in damage_reaction 02041F94) adds it to mJntTargetAngle[3].z |
| 0x4E8 | `m4B4` | `mJnt6LandZ` | dr_damage_set 02043F34 sets mJnt2LandZ / 2 on landing (0 at recovery / face down); dr_damage_anime (in damage_reaction 02041F94) subtracts it from mJntTargetAngle[6].z |
| 0x4EA | `m4B6` | `mJnt7LandZ` | dr_damage_set 02043F34 sets mJnt3LandZ / 2 on landing (0 at recovery / face down); dr_damage_anime (in damage_reaction 02041F94) subtracts it from mJntTargetAngle[7].z |
| 0x4EC | `m4B8` | `mJnt6AddY` | dr_damage_set 02043F34 sets a random 1000..7000 at the knock-down (not Darknut), 0 at recovery; dr_damage_anime (in damage_reaction 02041F94) adds it to mJntTargetAngle[6].y |
| 0x4EE | `m4BA` | `mJnt7AddY` | dr_damage_set 02043F34 sets a random 1000..7000 at the knock-down (not Darknut), 0 at recovery; dr_damage_anime (in damage_reaction 02041F94) subtracts it from mJntTargetAngle[7].y |
| 0x4F0 | `m4BC` | `mJnt4AddY` | dr_damage_set 02043F34 sets a random value at the knock-down, 0 at recovery; dr_damage_anime (in damage_reaction 02041F94) adds it to mJntTargetAngle[4].y (and uses it as [8].z) |
| 0x4F2 | `m4BE` | `mJnt5AddY` | dr_damage_set 02043F34 sets a random value at the knock-down, 0 at recovery; dr_damage_anime (in damage_reaction 02041F94) subtracts it from mJntTargetAngle[5].y (and uses it as [9].z) |
| 0x4F4 | `m4C0` | `mLandAngleY` | dr_damage_set 02043F34 sets it on landing to mKnockbackAngleY or its reverse (whichever is nearer current.angle.y); while knocked down current.angle.y eases toward it |
| 0x4FC | `m4C8` | `mDownTimers` | damage_reaction 02041F94 counts all three down; dr_damage_set 02043F34: [0] = 10 holds the body after a high wall hit, [2] delays the recovery (set from the enemies' HIO by daBk_Execute 0209BD1C / daMo2_Execute 021C9B1C, 10000 at death) |
| 0x504 | `m4D0` | `mTurnTargetY` | bk / mo2 action code (fight_run 020A0A98, b_nige 020A2A54, wepon_search 020A3CC8, hukki 021CE6E8, ...) sets the yaw the enemy should face (from mTargetAngleY or atan2) and eases current.angle.y toward it |
| 0x508 | `m4D4` | `mPushback` | damage_check (in daBk_Execute / daMo2_Execute) sets -20 / -HIO value; dr_damage_set 02043F34 moves the enemy by it along the hit direction (atan2 of -mHitVec) each frame and eases it to 0 |
| 0x714 | `m6E0` | `mSpinLeft` | dr_damage_set 02043F34 adds +-0x8000 / +-0x10000 after a corner (kado_check 02041D10) hit, then steps it to 0 by 0x1000 while adding the same steps to mSpinAngleY |
| 0x71C | `m6E8` | `mPrevGroundPos` | dr_body_bg_check (in dr_damage_set 02043F34): old.pos with y = the ground height before this check; dr_damage_set 02043F34 uses it as the ledge position (mHangPos) when the fall height exceeds 300 |
| 0x740 | `m70C` | `mBounceCount` | dr_damage_set 02043F34 increments it at each ground landing while knocked down and clears it at the launch; daBk_Create 0209F918 / daMo2_Create 021CCB00 set 1; damage_check (in daBk_Execute) clears it |
| 0x741 | `m70D` | `mFaceDown` | dr_matrix_set (in damage_reaction 02041F94): 1 when a point 10000 ahead in the body frame lies below current.pos; dr_damage_set 02043F34 picks recovery mode 10 / 12 by it |
| 0x742 | `m70E` | `mShakeJntMask` | dr_damage_set 02043F34 sets 0xFF / 7; dr_damage_anime (in damage_reaction 02041F94) shakes joints 2, 3, 6, 7 (bit 8), 0, 1, 4, 5 (bit 4), 19 (bit 2), 13 (bit 1) |
| 0x744 | `m710` | `mLookMode` | set by the bk / mo2 action code (hukki 020A3048, fight_run 020A0A98, p_lost 020A27F8, Bk_move 020A5EF4, ...: 0, 1, 3, 4, 6); dr_damage_anime (in damage_reaction 02041F94): 1 turns the head toward mpLookTarget's eyePos, 5 uses mLookPitch / mLookYaw, 3 / 4 / 6 fixed poses |
| 0x745 | `m711` | `mDefencePose` | defence (mo2 021CE46C) sets 1, Bk_move 020A5EF4 / Mo2_move 021D0A14 clear it; dr_damage_anime (in damage_reaction 02041F94) sets mJntTargetAngle[8].x = 8000 while it is set |
| 0x747 | `m713` | `mPlayerInView` | daBk_player_view_check 0209B05C / daMo2_player_view_check 021C8C14 set 1 / 2 when the player is in the view box, 0 otherwise; daBk_player_bg_check 0209AE7C / daMo2_player_bg_check 021C8A38 test and clear it; dr_damage_set 02043F34 clears it at the knock-down |
| 0x748 | `m714` | `mpLookTarget` | Bk_move 020A5EF4 / Mo2_move 021D0A14 set the player, aite_miru (bk 020A3624, mo2 021CEC44) another actor; the view / bg checks use its current.pos; dr_damage_anime (in damage_reaction 02041F94) turns the head toward its eyePos |
| 0x74C | `m718` | `mLookPitch` | set by bk_move code (-10000 / random); dr_damage_anime (in damage_reaction 02041F94) uses it as the head pitch for mLookMode 5 and adds it to mJntTargetAngle[18].z |
| 0x74E | `m71A` | `mLookYaw` | set by bk_move code (10000 / random); dr_damage_anime (in damage_reaction 02041F94) uses it as the head yaw for mLookMode 5 and adds it to mJntTargetAngle[18].y |
| 0x752 | `m71E` | `mBodyBgCheckDelay` | dr_body_bg_check (in dr_damage_set 02043F34) skips the body ground check and counts it down while it is non-zero; dr_damage_set sets 2 / 10, water_fail 020A3B18 / b_hang 020A5674 5, damage_check (in daBk_Execute) 10 |
| 0x7C8 | `m794` | `mWallSmokeTimer` | dr_damage_set 02043F34 sets 6 when it starts the wall-hit smoke; damage_reaction 02041F94 counts it down and ends mParticleCallBack at 0 |
| 0x7CC | `m798` | `mHangMoveSpeed` | b_hang 020A5674 clears it and eases it to 100; the step with which current.pos follows the hang position |
| 0x7D0 | `m79C` | `mpHangPos` | ride_call_back (d_a_bridge 020DF908) points it at the bridge's side position; b_hang 020A5674 makes current.pos follow it while mHangBridgeId is set |
| 0x7D4 | `m7A0` | `mHangPos` | dr_damage_set 02043F34 copies mPrevGroundPos into it before the hang; hang_ang_get (inlined there) measures from it; b_hang 020A5674 makes current.pos follow it (ledge hang) |
| 0x7E0 | `m7AC` | `mHangAngle` | dr_damage_set 02043F34 sets y from hang_ang_get (0xDCF = no hang: dies); ride_call_back (d_a_bridge 020DF908) sets the bridge rotation; b_hang 020A5674 turns current.angle.y toward y |
| 0x7E6 | `m7B2` | `mBridgeRideTimer` | ride_call_back (d_a_bridge 020DF908) sets 8 each frame a Bokoblin rides the bridge; damage_reaction 02041F94 counts it down; dr_damage_set 02043F34 starts the hang action 0x13 while it is non-zero |
| 0x7E8 | `m7B4` | `mBridgeRideRoll` | ride_call_back (d_a_bridge 020DF908) sets -0x2000; b_hang 020A5674 passes it to the bridge as mRideRollTarget |
| 0x7EA | `m7B6` | `mSkipMoveOnce` | dr_damage_set 02043F34 sets 1 at recovery and skips one frame of its movement / ground checks while it is set (clearing it); hukki (bk 020A3048) sets 1 |
| 0x7EC | `m7B8` | `mHangBridgeId` | ride_call_back (d_a_bridge 020DF908) stores the bridge's process ID; dr_damage_set 02043F34 sets -1 for a ledge hang; b_hang 020A5674 searches the bridge by it |

## fm_main::HIO (Floormaster HIO; d_a_fm_main.h, with the views fm_a::HIO in d_a_fm_a.h and fm_c::HIO in d_a_fm_c.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x004 | `field_0x004` | `mDebugTargets` | debugDraw 021439A0: while set (and the body is out) marks mpActorTarget / mBaseTarget; tww debugDraw the same; ctor 0214996C clears it |
| 0x005 | `field_0x005` | `mDebugHeadTop` | debugDraw 021439A0 tests it; tww debugDraw reads Link's getHeadTopPos() under it; ctor 0214996C clears it |
| 0x006 | `field_0x006` | `mDebugGrabAxis` | debugDraw 021439A0 tests it; tww debugDraw draws the axis of mGrabMtx (dLib_debugDrawAxis) under it; ctor 0214996C clears it |
| 0x007 | `field_0x007` | `mDebugHandAxis` | debugDraw 021439A0 tests it; tww debugDraw draws the axis of the FM_JNT_TE_e joint matrix under it; ctor 0214996C clears it |
| 0x009 | `field_0x009` | `mDebugAngles` | debugDraw 021439A0 tests it; tww debugDraw draws the mGrabAngleMax / mTurnAngleMin fans (radius mReachDist) and the mHeightRange mark under it; ctor 0214996C clears it |
| 0x00A | `field_0x00A` | `mDebugGoalLine` | debugDraw 021439A0 tests it; tww debugDraw draws current.pos to mGoalPos under it; ctor 0214996C clears it |
| 0x00B | `field_0x00B` | `mDebugJoints` | debugDraw 021439A0 tests it (loop over the six joints, TE skipped); ctor 0214996C clears it |
| 0x00C | `field_0x00C` | `mDebugArea` | debugDraw 021439A0 tests it; tww debugDraw draws the 12 mAreaPoints, the line to mBaseTarget and mHomePos under it; ctor 0214996C clears it |
| 0x00D | `field_0x00D` | `mNoGrabDemo` | modeGrab 02147BE0 / modeGrabFootDemo 021487F8: while set the grab demo is not started (sink timer and item-throw timer end without MODE_GRAB_DEMO / MODE_THROW); ctor 0214996C clears it |
| 0x00F | `field_0x00F` | `mIkJointOn` | _nodeControl 021404AC applies the IK quaternion of joint i only while [i] == 1; calcInvKine 021468E8 solves only those joints; ctor 0214996C sets 1 / 0 per joint |
| 0x020 | `field_0x020` | `mHoleOffset` | holeSetMtx 0214162C translates the hole model by it after the Y rotation; ctor 0214996C (-10, 5, -10) |
| 0x02C | `field_0x02C` | `mBodyOffsetInit` | createInit 02141930 copies it into daFm_c::mBodyOffset (the body model translation in bodySetMtx); ctor 0214996C (0, 0, -30) |
| 0x038 | `field_0x038` | `mHandOffset` | _nodeControl 021404AC transforms it by the TE joint matrix into mHandPos; grabPlayer 021429BC / grabBomb 02142E0C / grabTsubo 02142EF4 add it to the carried position; ctor 0214996C (40, -20, 0) |
| 0x066 | `field_0x066` | `mUnderFootTime` | modeUnderFootInit 0214468C loads mModeTimer with it (the wait before appear / hide / foot grab); ctor 0214996C 0x28 |
| 0x068 | `field_0x068` | `mBikubikuTime` | modeBikubikuInit 021497C0 loads mModeTimer with it; ctor 0214996C 0x1E |
| 0x084 | `field_0x084` | `mHideTimeMin` | modeHideInit 02144568: mModeTimer = setRnd(it, mHideTimeMax); ctor 0214996C 0x5A |
| 0x086 | `field_0x086` | `mHideTimeMax` | modeHideInit 02144568: mModeTimer = setRnd(mHideTimeMin, it); ctor 0214996C 0xF0 |
| 0x088 | `field_0x088` | `mFarTime` | modeWaitInit 02146434 / modeWait 021464B4 load mFarTimer with it; modeWait disappears when the base target stays beyond mNoticeRange until it runs out; ctor 0214996C 0x3C |
| 0x08C | `field_0x08C` | `mWaitTimeMin` | modeWaitInit 02146434: mModeTimer = setRnd(it, mWaitTimeMax); ctor 0214996C 0x3C |
| 0x08E | `field_0x08E` | `mWaitTimeMax` | modeWaitInit 02146434: mModeTimer = setRnd(mWaitTimeMin, it); ctor 0214996C 0x78 |
| 0x090 | `field_0x090` | `mGrabLinkSinkTime` | modeGrabInit 02147810 loads mSinkTimer with it when the target is Link (mGrabNpcSinkTime otherwise); modeGrab 02147BE0 starts the grab demo when it runs out; ctor 0214996C 10 |
| 0x092 | `field_0x092` | `mItemThrowTime` | modeGrabInit 02147810 loads mModeTimer with it; modeGrab 02147BE0 reloads it while Link guards and throws the held bomb / pot (MODE_THROW) when it runs out; ctor 0214996C 0x11 |
| 0x094 | `field_0x094` | `mAttackTime` | modeAttackInit 02146764 loads mModeTimer with it; modeAttack 02146A34 ends the attack when the animation stops and it ran out; ctor 0214996C 0 |
| 0x096 | `field_0x096` | `mTurnStep` | modeAttack 02146A34 / modeGrab 02147BE0: the max step of cLib_addCalcAngleS2 turning shape_angle.y to the target; ctor 0214996C 500 |
| 0x098 | `field_0x098` | `mGrabAngleMax` | isGrabPos 02145F90 refuses while the angle to the grab position differs from shape_angle.y by more; ctor 0214996C 18000 |
| 0x09A | `field_0x09A` | `mTurnAngleMin` | modeWait 021464B4 / modeAttack 02146A34 / modeGrab 02147BE0 turn only while the angle difference is above it; ctor 0214996C 7000 |
| 0x09C | `field_0x09C` | `mTgHitTime` | checkTgHit 02145070 loads mTgHitTimer with it after a hit; ctor 0214996C 5 |
| 0x09E | `field_0x09E` | `mGrabNpcSinkTime` | modeGrabInit 02147810 loads mSinkTimer with it when the target is not Link (mGrabLinkSinkTime otherwise); ctor 0214996C 0x96 |
| 0x0A0 | `field_0x0A0` | `mRndMoveTime` | moveRndBack 02144DE4 / moveRndEscape 02147914 reload mMoveTimer with it when they pick a new random point; ctor 0214996C 0x14 |
| 0x0A6 | `field_0x0A6` | `mDamageTime` | modeDamageInit 02147724 loads mModeTimer with it; ctor 0214996C 10 |
| 0x0A8 | `field_0x0A8` | `mHeightRange` | the vertical range: checkHeight 02140CC0 (half of it), modeHide 021445B8 / modeUnderFoot 02144838 (base target against mHomePos.y), modePathMove 02144A18 / modeGoalKeeper 02144B94 / isGrabPos 02145F90 / isGrab 021467D8 / modeAttack 02146A34 compare a height difference with it; ctor 0214996C 600 |
| 0x0AC | `field_0x0AC` | `mRndBackDist` | moveRndBack 02144DE4: the new mMoveTargetPos is it along mMoveAngle; ctor 0214996C 300 |
| 0x0B0 | `field_0x0B0` | `mRndMoveStep` | moveRndBack 02144DE4 / moveRndEscape 02147914: the max step of fm_addCalcPosXZ2 toward mMoveTargetPos; ctor 0214996C 30 |
| 0x0B4 | `field_0x0B4` | `mEscapeRange` | moveRndEscape 02147914 / modeGrab 02147BE0 compare the player distance with it (escape with an NPC, goal-keeper throw check); ctor 0214996C 1000 |
| 0x0B8 | `field_0x0B8` | `mRndEscapeDist` | moveRndEscape 02147914: the new mMoveTargetPos is it along mMoveAngle from mGoalPos; ctor 0214996C 1000 |
| 0x0BC | `field_0x0BC` | `mNpcDemoPlayerDist` | modeAttack 02146A34: after grabbing an NPC, MODE_GRAB_NPC_DEMO starts while the player is farther than it (or out of mHeightRange); ctor 0214996C 750 |
| 0x0C0 | `field_0x0C0` | `mUnderFootStep` | modeUnderFoot 02144838: the max step of fm_addCalcPosXZ2 toward the base target; ctor 0214996C 15 |
| 0x0C4 | `field_0x0C4` | `mReachDist` | modeAttack 02146A34 does not chase a grab position between mNearRange and it; modeGrab 02147BE0 throws the held NPC at the player inside it; tww debugDraw uses it as the radius of the angle fans; ctor 0214996C 290 |
| 0x0C8 | `field_0x0C8` | `mSphRadius` | setCollision 021432B8: mSph.SetR(it); ctor 0214996C 50 |
| 0x0CC | `field_0x0CC` | `mChaseSpeed` | modeGoalKeeper 02144B94 / modeAttack 02146A34 / moveRndEscape 02147914: step rate (it + 1) * 0.005 * mSpeedRate of fm_addCalcPosXZ2; ctor 0214996C 7 |
| 0x0D0 | `field_0x0D0` | `mPathSpeed` | modePathMove 02144A18: step rate (it + 1) * 0.005 * mSpeedRate toward mPathPos; ctor 0214996C 2 |
| 0x0D4 | `field_0x0D4` | `mPathReachDist` | _pathMove 02140AC8: a path point counts as reached within it + 1; ctor 0214996C 5 |
| 0x0D8 | `field_0x0D8` | `mPathRndOffset` | _pathMove 02140AC8: the reached point moves along the segment by it times a random 1-10; ctor 0214996C 80 |
| 0x0DC | `field_0x0DC` | `mFootGrabDist` | isGrabFoot 0214472C: Link must be within it (XZ); calcInvKine 021468E8 solves the IK only for a target within it; ctor 0214996C 100 |
| 0x0E0 | `field_0x0E0` | `mNearRange` | areaCheck 0214264C: the radius of the 12 area points; isGrabPos 02145F90 / modeWait 021464B4 / modeAttack 02146A34: the inner distance limit; moveRndBack 02144DE4; ctor 0214996C 150 |
| 0x0E4 | `field_0x0E4` | `mAttackRange` | searchNearOtherActor 02140D04 / searchTarget 02145A60 take targets within it; modeUnderFoot 02144838 / modePathMove 02144A18 appear when the base target is within it; isGrabPos 02145F90 outer limit for non-Link targets; ctor 0214996C 500 |
| 0x0E8 | `field_0x0E8` | `mNoticeRange` | setBaseTarget 02141714 picks Medli / Link within it; _execute 02143418 line-checks the base target within it; modeGoalKeeper 02144B94 / modeWait 021464B4 (turn between mAttackRange and it, mFarTimer beyond it); ctor 0214996C 800 |
| 0x0EC | `field_0x0EC` | `mGrabReach` | isGrab 021467D8: the hand (mHandPos) must be within it of mGrabPos (doubled with mSwAppearDemo); setAttention 021422A4 moves eyePos while Link's head top is within it of the hand; ctor 0214996C 30 |
| 0x0F0 | `field_0x0F0` | `mThrowSpeedF` | modeThrow 02147408: the thrown target's speedF; ctor 0214996C 25 |
| 0x0F4 | `field_0x0F4` | `mThrowSpeedY` | modeThrow 02147408: the thrown target's speed.y; ctor 0214996C 9 |
| 0x0F8 | `field_0x0F8` | `mThrowGravity` | modeThrow 02147408: the thrown target's gravity; ctor 0214996C -2.5 |
| 0x0FC | `field_0x0FC` | `mThrowFrame` | modeThrow 02147408 releases the target at this animation frame; ctor 0214996C 16 |
| 0x100 | `field_0x100` | `mGrabDelayTime` | modeWaitInit 02146434 loads mGrabDelayTimer with it (modeWait attacks a held target only after it); ctor 0214996C 0x1E |
| 0x104 | `field_0x104` | `mPullOffset` | _nodeControl 021404AC transforms it by the TE joint matrix into mPullPos; ctor 0214996C (0, -20, 0) |
| 0x110 | `field_0x110` | `mPullDistMin` | modeAttack 02146A34 (fm_pushLink) / modeGrab 02147BE0 pull Link only while mGrabPos is farther than it from mPullPos; ctor 0214996C 20 |
| 0x114 | `field_0x114` | `mPullDistMax` | modeAttack 02146A34 (fm_pushLink) / modeGrab 02147BE0 pull Link only while mGrabPos is nearer than it to mPullPos; ctor 0214996C 50 |
| 0x118 | `field_0x118` | `mPullPower` | fm_pushLink (in modeAttack 02146A34) / modeGrab 02147BE0: daPy_setOutPower power = it * the distance ratio; ctor 0214996C -20 |
| 0x11C | `field_0x11C` | `mParalysisTime` | modeParalysisInit 02149688 loads mModeTimer with it; modeParalysis 021496C8 shakes between it - 0x14 and it - 0x3C; ctor 0214996C 100 |
| 0x120 | `field_0x120` | `mHoleScaleClosed` | the closed hole scale: modeSwWaitInit 02144190 / modePlayerStartDemoInit 021491B4 set mHoleScale to it; iceProc 021424D0 / modeHide 021445B8 / modeDeath 02148CC0 / modeDelete 021495F8 shrink toward it; ctor 0214996C 0.01 |
| 0x124 | `field_0x124` | `mHoleScaleOpen` | the open hole scale: modePathMoveInit 021449C4 sets mHoleScale to it; modeSwWait 021442A8 / modeUnderFoot 02144838 / modePlayerStartDemo 02149208 grow toward it; ctor 0214996C 1 |
| 0x128 | `field_0x128` | `mHoleScaleStep` | the max step of setHoleScale toward mHoleScaleClosed / mHoleScaleOpen (iceProc 021424D0, modeHide 021445B8, modeSwWait 021442A8, modeDeath 02148CC0, modePlayerStartDemo 02149208, modeDelete 021495F8); ctor 0214996C 0.1 |
| 0x12C | `field_0x12C` | `mHoleScaleStepUnderFoot` | modeUnderFoot 02144838: the max step of setHoleScale toward mHoleScaleOpen; ctor 0214996C 0.02 |

Open (offset names kept):

- `field_0x008`, `field_0x00E`, `field_0x01F`, `field_0x044`, `field_0x050`, `field_0x05C`, `field_0x0A2`: only the constructor 0214996C writes them; no read in WWHD or tww.
- `field_0x06A`, `field_0x0A4`, `field_0x11E`: no use in WWHD or tww.
- `field_0x015`: unused padding.
- `field_0x08A`: two unrelated reads (modeUnderFoot 02144838 reloads mMoveTimer with it; modeWaitInit 02146434 loads mSinkTimer with it), no single role.
- The private view in d_a_fm_b.h (`daFm_HIO_c`, another batch) keeps its offset names; its fields are the same HIO and should take these names.


## daFm_c (Floormaster; d_a_fm.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x3E3 | `field_0x2C7` | `mExitSclsNo` | getArg 021413E0: param bits 0-7; modeGrabDemo 021482F0 calls dLib_setNextStageBySclsNum(it) when the grab demo with Link ends (0xFF: back to the start stage) (tww the same) |
| 0x3E4 | `field_0x2C8` | `mNpcRestartPoint` | getArg 021413E0: home.angle.x bits 8-15; modeGrabDemo 021482F0 passes it to daPy_npc_setPointRestart for Makar / Medli when DEFAULT_FM_SUIKOMI_NPC ends (0xFF: none) (tww the same) |
| 0x3EC | `field_0x2D0` | `mMoveType` | getArg 021413E0: param bits 8-9 (3 -> 0); createInit 02141930 starts MODE_UNDERFOOT / MODE_PATH_MOVE / MODE_GOAL_KEEPER for 0 / 1 / 2 and picks the Fmaster / Fmastr1 / Fmastr2 item table; areaCheck 0214264C, modeDisappear 02146310, modeGrab 02147BE0 branch on it (tww the same) |
| 0x3F0 | `field_0x2D4` | `mAppearSwitchNo` | getArg 021413E0: param bits 24-31; createInit 02141930 starts MODE_SW_WAIT while it is not set; modeSwWait 021442A8 waits for dComIfGs_isSwitch(it) (tww the same) |
| 0x3F4 | `field_0x2D8` | `mNoCreateSwitchNo` | getArg 021413E0: home.angle.x bits 0-7; _create 02141E40 returns cPhs_ERROR_e while that switch is set (tww the same) |
| 0x3F8 | `field_0x2DC` | `mBaseTargetType` | getArg 021413E0: param bits 10-11 (3 -> 0); setBaseTarget 02141714: 0 Medli or Link, 1 Link, 2 Makar or Medli; searchTarget 02145A60 / modeGrab 02147BE0 branch on it (tww the same) |
| 0x3FC | `field_0x2E0` | `mAreaRadius` | getArg 021413E0: home.angle.z bits 0-7 times 100 (0xFF or 0: 3000); areaCheck 0214264C drops area points farther than it from mHomePos; modeGoalKeeper 02144B94 / isGrabPos 02145F90 / modeWait 021464B4 compare XZ distances to mHomePos with it (tww the same) |
| 0x400 | `field_0x2E4` | `mSwAppearDemo` | getArg 021413E0 sets 1 when home.angle.y != 0 and mAppearSwitchNo != 0xFF; with it createInit 02141930 turns off the wall check, modeSwWait 021442A8 orders DEFAULT_FM_SW_APEEAR, modeGrab 02147BE0 goes to MODE_GRAB_NPC_DEMO, isGrab 021467D8 doubles mGrabReach, modeDisappear 02146310 deletes (tww the same) |
| 0x401 | `field_0x2E5` | `mSwAppearInEvent` | modeSwWait 021442A8 sets 1 when the switch opens while DEFAULT_FM_SW_APEEAR runs; modeDisappear 02146310 deletes the actor while set; modeGoalKeeper 02144B94 waits for DEFAULT_FM_SUIKOMI_NPC to end while set (tww the same) |
| 0x404 | `field_0x2E8` | `mJointPos` | _nodeControl 021404AC stores the world position of joints 0-5 (stack matrix translation); calcInvKine 021468E8 reads them as the IK chain (tww the same) |
| 0x44C | `field_0x330` | `mJointQuat` | the IK rotation per joint: createInit 02141930 sets ZeroQuat; calcInvKine 021468E8 / resetInvKine 021441D0 slerp it toward the solved / zero rotation; _nodeControl 021404AC applies it (mDoMtx_stack_quatM) (tww the same) |
| 0x4AC | `field_0x390` | `mIkStartJoint` | calcInvKine 021468E8 stores the first IK joint index; resetInvKine 021441D0 clears it; _nodeControl 021404AC applies mJointQuat only from it on (tww the same) |
| 0x4B0 | `field_0x394` | `mSpeedRate` | createInit 02141930 / _pathMove 02140AC8: cM_rndF(9) + 1; modePathMove 02144A18 / modeGoalKeeper 02144B94 / modeAttack 02146A34 / moveRndEscape 02147914 scale the fm_addCalcPosXZ2 step by it (tww the same) |
| 0x4CC | `field_0x3B0` | `mPathPos` | modePathMove 02144A18: dLib_pathMove moves it along mpPath and the actor follows it with fm_addCalcPosXZ2 (tww the same) |
| 0x4D8 | `field_0x3BC` | `mPathPntNo` | modePathMove 02144A18: dLib_pathMove's point index argument for mpPath (tww the same) |
| 0x55C | `field_0x3E0` | `mHoleScale` | the hole scale: setHoleScale 02142450 chases it (cLib_addCalc2); holeSetMtx 0214162C sets the hole model base scale to it; setHoleEffect 02142158 the emitter scale; setCollision 021432B8 mCyl.SetR(it); holeExecute 02142200 / _draw 021440D8 test it > 0.015 (tww the same) |
| 0x560 | `field_0x3E4` | `mNearFmPos` | searchNearFm 02140F14 stores the position of the nearest other Floormaster; modeGrab 02147BE0 (goal keeper with an NPC) turns toward it when it is non-zero (tww the same) |
| 0x578 | `field_0x3FC` | `mGrabDemoLink` | modeGrabDemoInit 021482AC stores fm_isLink(mpActorTarget); modeGrabDemo 021482F0 runs Link's grab demo (DEFAULT_FM_GRAB, next stage) while set, the NPC branch otherwise (tww the same) |
| 0x78C | `field_0x610` | `mBodyOffset` | bodySetMtx 0214153C translates the body model by it; createInit 02141930 copies HIO mBodyOffsetInit into it; modeGrabDemo 021482F0 chases it to 0; modeGrabFootDemoInit 0214879C clears it (tww the same) |
| 0x798 | `field_0x61C` | `mHandPos` | _nodeControl 021404AC: mHandOffset through the TE joint matrix; setAttention 021422A4 attention / eye position, setCollision 021432B8 mSph center, _draw 021440D8 snap position, isGrab 021467D8 / calcInvKine 021468E8 distance to mGrabPos, modeGrab 02147BE0 copies it into mGrabPos for held items (tww the same) |
| 0x7AC | `field_0x630` | `mFootGrabLinkPos` | modeGrabFootDemoInit 0214879C copies Link's position; modeGrabFootDemo 021487F8 chases it to the actor (fm_addCalcPosXZ2) and sets Link there (daPy_setPlayerPosAndAngle) (tww the same) |
| 0x7B8 | `field_0x63C` | `mPullPos` | _nodeControl 021404AC: HIO mPullOffset through the TE joint matrix; modeAttack 02146A34 (fm_pushLink) / modeGrab 02147BE0 pull Link toward it by distance and angle from mGrabPos (tww the same) |
| 0x7C4 | `field_0x648` | `mMoveTimer` | moveRndBack 02144DE4 / moveRndEscape 02147914 pick a new random mMoveTargetPos when it runs out (reload mRndMoveTime); modeUnderFoot 02144838 retargets the base target when it runs out (tww the same) |
| 0x7C8 | `field_0x64C` | `mHandShakeTimer` | checkTgHit 02145070 / modeBikubiku 02149818 set 0x28; _nodeControl 021404AC shakes the TE joint while fm_calcTimer counts it down; modeDamage 02147764 / modeBikubiku wait for 0 (tww the same) |
| 0x7CC | `field_0x650` | `mModeTimer` | the per-mode timer: loaded by modeHideInit 02144568, modeUnderFootInit 0214468C, modeAppearInit 02144D88, modeWaitInit 02146434, modeAttackInit 02146764, modeGrabInit 02147810, modeDamageInit 02147724, modeParalysisInit 02149688, modeBikubikuInit 021497C0; the modes act when fm_calcTimer reaches 0 (tww the same) |
| 0x7D4 | `field_0x658` | `mFarTimer` | modeWaitInit 02146434 / modeWait 021464B4 load HIO mFarTime; modeWait counts it down while the base target is beyond mNoticeRange and disappears at 0 (tww the same) |
| 0x7D8 | `field_0x65C` | `mGrabDelayTimer` | modeWaitInit 02146434 loads HIO mGrabDelayTime; modeWait 021464B4 attacks a target Link carries (or an NPC) only after it ran out (tww the same) |
| 0x7DC | `field_0x660` | `mMoveTargetPos` | the XZ move target: moveRndBack 02144DE4 / moveRndEscape 02147914 (random point), modeUnderFoot 02144838 (base target), modeAttack 02146A34 (mGrabPos), modeGrabInit 02147810 (mGoalPos); the actor follows it with fm_addCalcPosXZ2; _execute 02143418 keeps its y at current.pos.y (tww the same) |
| 0x7FC | `field_0x680` | `mMoveAngle` | moveRndBack 02144DE4 / moveRndEscape 02147914: the random direction of the next mMoveTargetPos (turned by 0x5000 plus a random 4096 step) (tww the same) |
| 0x800 | `field_0x684` | `mGrabKind` | what the hand holds: modeGrab 02147BE0 1 (Link), modeAttack 02146A34 2 (bomb) / 3 (pot) / 4 (NPC); _execute 02143418 calls grabPlayer / grabBomb / grabTsubo / grabNPC by it and clears it when the target is gone; cancelGrab 02142028, modeThrow 02147408 test it; createInit 02141930 clears it (tww the same) |
| 0x806 | `field_0x68A` | `mHandShakePhase` | _nodeControl 021404AC: advanced by 0x1830 per frame while mHandShakeTimer runs, the sine phase of the TE joint shake (tww the same) |
| 0x808 | `field_0x68C` | `mHandShakeAmp` | checkTgHit 02145070 sets 0x2B00; _nodeControl 021404AC decays it to 0 (cLib_addCalcAngleS2) and rotates the TE joint by it * sin(mHandShakePhase) (tww the same) |
| 0x80C | `field_0x690` | `mGoalPos` | createInit 02141930 / modeUnderFoot 02144838 / modePathMove 02144A18 / modeDisappear 02146310 store the position; modeGoalKeeper 02144B94 sets it to mHomePos and moves there; modeGrabInit 02147810 / moveRndEscape 02147914 start the escape from it (tww the same) |
| 0x818 | `field_0x69C` | `mHomePos` | createInit 02141930 copies the start position; areaCheck 0214264C measures the area points from it; modeHide 021445B8 / modeUnderFoot 02144838 compare the base target height with its y; modeGoalKeeper 02144B94 returns to it (tww the same) |
| 0x838 | `field_0x6BC` | `mGrabMtx` | grabPlayer 021429BC / grabNPC 02142C4C / grabTsubo 02142EF4 copy the hand matrix (mDoMtx_stack_c::get()) into it; debugDraw 021439A0 draws its axis under HIO mDebugGrabAxis (tww the same) |
| 0xB40 | `field_0x9C4` | `mTgHitTimer` | checkTgHit 02145070 ignores hits while fm_calcTimer counts it down and reloads it (HIO mTgHitTime, or 0xC) after a hit (tww the same) |
| 0xB4C | `field_0x9D0` | `mBaseTargetAngleY` | setBaseTarget 02141714: fopAcM_searchActorAngleY(this, mBaseTarget) (0 without one); turnToBaseTarget 02144A04, modeAppearInit 02144D88, modeWait 021464B4 turn to it (tww the same) |
| 0xB50 | `field_0x9D4` | `mBaseTargetDist` | setBaseTarget 02141714: fopAcM_searchActorDistanceXZ(this, mBaseTarget) (0 without one); _execute 02143418, modeUnderFoot 02144838, modePathMove 02144A18, modeGoalKeeper 02144B94, moveRndBack 02144DE4, modeWait 021464B4 compare it with the HIO ranges (tww the same) |
| 0xBC4 | `field_0xA48` | `mAreaPoints` | areaCheck 0214264C: the 12 points at HIO mNearRange around the actor (every 0x1555), each checked against mAreaRadius and by a BG line (tww the same; tww debugDraw draws them) |
| 0xC54 | `field_0xAD8` | `mAreaPointOk` | areaCheck 0214264C: per point of mAreaPoints 1 while it is inside mAreaRadius and has ground under it (lineCheck hit), else 0 (tww the same) |
| 0xC60 | `field_0xAE4` | `mInArea` | _execute 02143418 stores areaCheck's result (all 12 mAreaPoints ok); modeHide 021445B8, modeUnderFoot 02144838, moveRndBack 02144DE4, modeAttack 02146A34, moveRndEscape 02147914 leave or stop moving without it (tww the same) |
| 0xC61 | `field_0xAE5` | `mBaseTargetBlocked` | _execute 02143418: 1 when the BG line from the actor to mBaseTarget (100 up) hits within mNoticeRange, 0 with mSwAppearDemo; modeUnderFoot 02144838 / modePathMove 02144A18 / modeGoalKeeper 02144B94 act only while it is 0 (tww the same) |

Open (offset names kept):

- `field_0x398`, `field_0x3A4`: only written (_pathMove 02140AC8 copies its segment end points), never read in WWHD or tww.
- `field_0x688`: only written (modeGrabDemo 021482F0 sets 1 when Medli is caught), never read in WWHD or tww.
- `field_0x6B4`: only written (modeWaitInit 02146434 clears it), never read in WWHD or tww.
- `field_0x6B8`: only written (modeAttack 02146A34 sets 1 on an NPC grab), never read in WWHD or tww.


## st_ke_s (d_a_st.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
|  | `m000` | `mPos` | ke_control 02491458: the positions of the 10 points of the ke chain; part_move 024925B8 puts point 0 at the part joint; ke_control moves points 1-9 after the previous one with mSpeed and keeps them above the ground (tww ke_control the same) |
|  | `m078` | `mSpeed` | ke_control 02491458: the per-point velocity of the ke chain, the damped difference of the new and old positions (tww ke_control the same) |
|  | `m0F0` | `mRootAngle` | part_move 024925B8 copies the part rotation into it and spreads the three chains by 0x800 in y (chain 1 + 0x500 in x); ke_control 02491458 turns the chain direction by it (tww the same) |

## mo2HIO_c (Moblin tuning values, l_mo2HIO / moveHIO; d_a_mo2.h, d_a_mo2_main.h, d_a_mo2_exec.h, d_a_mo2_move.h)

The HIO block is declared in all four Moblin headers (d_a_mo2_move.cpp reads it as moveHIO, d_a_mo2_exec.cpp through l_mo2HIO()); all four copies are renamed together. The old names are the GameCube offsets (WWHD offset + 4: GHS puts the vtable pointer after the members). zeldaret/tww d217e2b (include/d/actor/d_a_mo2.h) keeps these offset names and its genMessage is empty, so nothing writes them except the constructor (__sinit_d_a_mo2_cpp 021CD5B4) and the few daMo2_Execute / daMo2_Create writes named below; the names are from the WWHD reads (tww reads them the same way). "Attack kind" is mo2_class mAttackKind (0..5).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0001 | `m005` | `mDrawSearchArea` | search_check_draw (in daMo2_Draw 021C7344) returns at once while it is 0, else builds the search circles / view box geometry from the ranges below (HD draws nothing); __sinit_d_a_mo2_cpp 021CD5B4 0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0002 | `m006` | `mFrameStep` | daMo2_Execute 021C9B1C runs the timers, damage_check and Mo2_move only while it is 0 or on a B trigger / Y hold (frame stepping); __sinit_d_a_mo2_cpp 021CD5B4 0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0003 | `m007` | `mKeepHealth` | __sinit_d_a_mo2_cpp 021CD5B4 0; no read in WWHD (daMo2_Execute: "HD: health debug reset removed"); tww d217e2b damage_check sets health = 10 after a hit while it is set (d_a_mo2.cpp line 3127); never written in WWHD or tww (tww genMessage registers nothing) |
| 0x0004 | `m008` | `mHeadFollowsBody` | nodeCallBack 021C6854: 0 = mHeadAngleY from the face joint direction (atan2), else mHeadAngleY = current.angle.y; __sinit_d_a_mo2_cpp 021CD5B4 0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0005 | `m009` | `mNoAttack` | hukki 021CE6E8 (mode 14) and wepon_search 021CF0F4 start an attack only while it is 0; __sinit_d_a_mo2_cpp 021CD5B4 0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0006 | `m00A` | `mAlertCheckTime` | nage 021CD958: mTimers[1] = it + mAlertCheckDelay (length of the KKEIKAI alert look), so the alert runs this long after the view check opens; __sinit_d_a_mo2_cpp 021CD5B4 20; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0008 | `m00C` | `mAlertCheckDelay` | nage 021CD958: mTimers[2] = it at the alert start; the player view re-check waits until mTimers[2] is 0; __sinit_d_a_mo2_cpp 021CD5B4 40; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x000C | `m010` | `mModelScale` | daMo2_Execute 021C9B1C: MtxScale(it, it, it) for the model base matrix; __sinit_d_a_mo2_cpp 021CD5B4 1.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0010 | `m014` | `mBlurScale` | br_draw (in daMo2_Draw 021C7344): MtxScale(it * mBlurScaleX, it, it) for the spear-trail blur model; __sinit_d_a_mo2_cpp 021CD5B4 1.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0018 | `m01C` | `mDisappearOffsetY` | fail 021CED78: fopAcM_createDisappear at current.pos with y + it + 100; __sinit_d_a_mo2_cpp 021CD5B4 25.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x001C | `m020` | `mHairMode` | ke_control 021C6F14: 0 = ground check under each spear-hair point, else the floor is dr.mSpawnY + 2.5; daMo2_Draw 021C7344 draws the hair (ke_disp) only while it is <= 1; daMo2_Create 021CCB00 and __sinit_d_a_mo2_cpp 021CD5B4 set 0 (tww the same) |
| 0x001E | `m022` | `mSmokeAlpha` | smoke_set_s 021C6578: alpha of the ID_AK_JT_ELEMENTSMOKE00 toon particle (dComIfGp_particle_setToon); __sinit_d_a_mo2_cpp 021CD5B4 185; never written in WWHD or tww (tww genMessage registers nothing) |
| 0x0020 | `m024` | `mSmokeTimerBase` | ground_smoke_set (in daMo2_Execute 021C9B1C) sets smoke only while mSmokeTimer >= it; b_nige / hukki / wepon_search / fight (in Mo2_move 021D0A14) load mSmokeTimer with it + 3..16; __sinit_d_a_mo2_cpp 021CD5B4 12; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0024 | `m028` | `mAttentionOffsetY` | nodeCallBack 021C6854: attention_info.position = eyePos with y + it; __sinit_d_a_mo2_cpp 021CD5B4 25.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0028 | `m02C` | `mSearchDist` | jyunkai (in Mo2_move 021D0A14): an armed Moblin notices the player within it (and in view); daMo2_Execute 021C9B1C sets 800 / 1200 while the player has the grab wear in patrol; search_check_draw circle radius; __sinit_d_a_mo2_cpp 021CD5B4 1250.0 |
| 0x002C | `m030` | `mFightDist` | fight_run (in Mo2_move 021D0A14) stops the run and strafes within it, goes back to patrol beyond it + 75; nage 021CD958 / e3_demo 021D0570 walk until within it; hukki / wepon_search attack within it; __sinit_d_a_mo2_cpp 021CD5B4 575.0; never written in WWHD or tww (tww genMessage registers nothing) |
| 0x0030 | `m034` | `mAttackDist` | fight_run (in Mo2_move 021D0A14): attacks within it (random chance at it +-62.5), walks back below it, pushes (mOshiCount) below it - 62.5; __sinit_d_a_mo2_cpp 021CD5B4 375.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0034 | `m038` | `mViewAngle` | half angle of the view cone: jyunkai / fight_run / fight (in Mo2_move 021D0A14), nage 021CD958, p_lost 021CDE6C pass it to daMo2_player_view_check 021C8C14; search_bomb 021C8378 the same; __sinit_d_a_mo2_cpp 021CD5B4 12740; daMo2_Execute 021C9B1C 0x1932 with the grab wear |
| 0x0038 | `m03C` | `mViewHeight` | daMo2_player_view_check 021C8C14 / daMo2_wepon_view_check 021C872C / search_bomb 021C8378 reject a target whose y + 50 differs from eyePos.y by more than it; __sinit_d_a_mo2_cpp 021CD5B4 400.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x003C | `m040` | `mViewBox` | daMo2_player_view_check 021C8C14 / daMo2_wepon_view_check 021C872C / search_bomb 021C8378: target in the body frame within |x| < it.x, |y| < it.y, z < it.z is seen; __sinit_d_a_mo2_cpp 021CD5B4 (500, 130, 500); daMo2_Execute 021C9B1C x / z 340 with the grab wear |
| 0x0048 | `m04C` | `mViewBoxBackZ` | the same view checks (daMo2_player_view_check 021C8C14, daMo2_wepon_view_check 021C872C, search_bomb 021C8378): rear z limit of the view box (z > it); __sinit_d_a_mo2_cpp 021CD5B4 -125.0; daMo2_Execute 021C9B1C -200 with the grab wear |
| 0x004C | `m050` | `mPatrolSpeedArmed` | jyunkai (in Mo2_move 021D0A14): speedF target (and path-point reach distance) while the Moblin has its weapon; __sinit_d_a_mo2_cpp 021CD5B4 12.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0050 | `m054` | `mPatrolSpeedUnarmed` | jyunkai (in Mo2_move 021D0A14): speedF target without a weapon (turn speed 0x1000); __sinit_d_a_mo2_cpp 021CD5B4 60.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0054 | `m058` | `mRunSpeedArmed` | fight_run (in Mo2_move 021D0A14): speedF target with the weapon (no dash); nage 021CD958 / e3_demo 021D0570 walk toward the target at it; __sinit_d_a_mo2_cpp 021CD5B4 45.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0058 | `m05C` | `mRunSpeedUnarmed` | fight_run (in Mo2_move 021D0A14): speedF target without a weapon; __sinit_d_a_mo2_cpp 021CD5B4 70.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x005C | `m060` | `mSprintSpeed` | b_nige 021CE138 (run from a bomb) and wepon_search 021CF0F4 (run to a weapon) set speedF to it; __sinit_d_a_mo2_cpp 021CD5B4 90.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0060 | `m064` | `mStrafeSpeed` | fight_run (in Mo2_move 021D0A14) modes 3 / 4: speedF eased to +it (forward) / -it (back); __sinit_d_a_mo2_cpp 021CD5B4 20.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0064 | `m068` | `mSideStepSpeed` | fight_run (in Mo2_move 021D0A14) modes 5 / 6 (move angle +-0x4000): speedF eased to it; __sinit_d_a_mo2_cpp 021CD5B4 20.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0068 | `m06C` | `mDashSpeed` | fight_run (in Mo2_move 021D0A14): speedF target while mDashFrames runs (DASH animation, smoke type 4); __sinit_d_a_mo2_cpp 021CD5B4 70.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x006C | `m070` | `mDashAnmSpeed` | fight_run (in Mo2_move 021D0A14): play speed of the looping BCK DASH animation (anm_init); __sinit_d_a_mo2_cpp 021CD5B4 1.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0070 | `m074` | `mAttackCheckWait` | fight_run (in Mo2_move 021D0A14): mTimers[4] = it each time it rolls the attack chance at mAttackDist +-62.5; __sinit_d_a_mo2_cpp 021CD5B4 15; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0074 | `m078` | `mAttackChance` | fight_run (in Mo2_move 021D0A14): attacks when cM_rndF(100) < it at that roll; __sinit_d_a_mo2_cpp 021CD5B4 80.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0078 | `m07C` | `mAttack0Chance` | attack_set 021C9310 (armed, on the player way): rnd(100) < it picks attack kind 0; __sinit_d_a_mo2_cpp 021CD5B4 40.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x007C | `m080` | `mAttack1Chance` | attack_set 021C9310: the next it percent (after mAttack0Chance) pick attack kind 1, the rest kind 2; __sinit_d_a_mo2_cpp 021CD5B4 30.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0080 | `m084` | `mAttack4Chance` | attack_set 021C9310 (unarmed): rnd(100) < it picks attack kind 4, else kind 5; __sinit_d_a_mo2_cpp 021CD5B4 50.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0084 | `m088` | `mDownRecoverDelay` | daMo2_Execute 021C9B1C damage results 0x14 / 0x15 set dr.mDownTimers[2] (recovery delay) to it; __sinit_d_a_mo2_cpp 021CD5B4 30; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0086 | `m08A` | `mWeaponRunTime` | wepon_search 021CF0F4 / yari_off_check (in daMo2_Execute 021C9B1C) set mWeaponRunTimer to it; __sinit_d_a_mo2_cpp 021CD5B4 300; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x00C8 | `m0CC` | `mAttack0TurnEndFrame` | attack_set 021C9310 sets mTurnEndFrame to it for attack kind 0; __sinit_d_a_mo2_cpp 021CD5B4 14; never written in WWHD or tww (tww genMessage registers nothing) |
| 0x00CA | `m0CE` | `mAttack1TurnEndFrame` | attack_set 021C9310 sets mTurnEndFrame to it for attack kind 1; __sinit_d_a_mo2_cpp 021CD5B4 10; never written in WWHD or tww (tww genMessage registers nothing) |
| 0x00CC | `m0D0` | `mAttack2TurnEndFrame` | attack_set 021C9310 sets mTurnEndFrame to it for attack kind 2; wepon_search 021CF0F4 uses it for its kind-2 attack; __sinit_d_a_mo2_cpp 021CD5B4 10; never written in WWHD or tww (tww genMessage registers nothing) |
| 0x00CE | `m0D2` | `mAttack3TurnEndFrame` | attack_set 021C9310 sets mTurnEndFrame to it for attack kind 3; __sinit_d_a_mo2_cpp 021CD5B4 5; never written in WWHD or tww (tww genMessage registers nothing) |
| 0x00D0 | `m0D4` | `mAttack4TurnEndFrame` | attack_set 021C9310 sets mTurnEndFrame to it for attack kind 4; __sinit_d_a_mo2_cpp 021CD5B4 100; never written in WWHD or tww (tww genMessage registers nothing) |
| 0x00D2 | `m0D6` | `mAttack5TurnEndFrame` | attack_set 021C9310 sets mTurnEndFrame to it for attack kind 5; hukki 021CE6E8 uses it for its kind-5 attack; __sinit_d_a_mo2_cpp 021CD5B4 10; never written in WWHD or tww (tww genMessage registers nothing) |
| 0x00D4 | `m0D8` | `mAttack1BlurStart` | fight (in Mo2_move 021D0A14), attack kind 1: the spear blur (mBlurOn) starts at this attack frame; mAttack1BlurScaleX is indexed from it; __sinit_d_a_mo2_cpp 021CD5B4 29; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x00D6 | `m0DA` | `mAttack1BlurPreLen` | fight (in Mo2_move 021D0A14), attack kind 1: blur frames before the mBlurUseB part (start + it .. start + it + mAttack1BlurBLen); __sinit_d_a_mo2_cpp 021CD5B4 1; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x00D8 | `m0DC` | `mAttack1BlurBLen` | fight (in Mo2_move 021D0A14), attack kind 1: length of the part with mBlurUseB = 1; __sinit_d_a_mo2_cpp 021CD5B4 2; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x00DA | `m0DE` | `mAttack1BlurPostLen` | fight (in Mo2_move 021D0A14), attack kind 1: blur frames after the mBlurUseB part (blur ends at start + pre + B + it); __sinit_d_a_mo2_cpp 021CD5B4 1; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x00DC | `m0E0` | `mAttack1BlurScaleX` | fight (in Mo2_move 021D0A14), attack kind 1: mBlurScaleX = it[attack frame - mAttack1BlurStart] (10 entries); __sinit_d_a_mo2_cpp 021CD5B4 1.0 / 1.25 / 1.25 / 1.0 / 0.5 ...; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0104 | `m108` | `mAttack2BlurStart` | fight (in Mo2_move 021D0A14), attack kind 2: the spear blur (mBlurOn) starts at this attack frame; mAttack2BlurScaleX is indexed from it; __sinit_d_a_mo2_cpp 021CD5B4 40; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0106 | `m10A` | `mAttack2BlurPreLen` | fight (in Mo2_move 021D0A14), attack kind 2: blur frames before the mBlurUseB part (start + it .. start + it + mAttack2BlurBLen); __sinit_d_a_mo2_cpp 021CD5B4 1; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0108 | `m10C` | `mAttack2BlurBLen` | fight (in Mo2_move 021D0A14), attack kind 2: length of the part with mBlurUseB = 1; __sinit_d_a_mo2_cpp 021CD5B4 3; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x010A | `m10E` | `mAttack2BlurPostLen` | fight (in Mo2_move 021D0A14), attack kind 2: blur frames after the mBlurUseB part (blur ends at start + pre + B + it); __sinit_d_a_mo2_cpp 021CD5B4 1; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x010C | `m110` | `mAttack2BlurScaleX` | fight (in Mo2_move 021D0A14), attack kind 2: mBlurScaleX = it[attack frame - mAttack2BlurStart] (10 entries); __sinit_d_a_mo2_cpp 021CD5B4 1.0 / 1.1 / 1.15 / 1.25 / 1.1 / 1.0 ...; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0134 | `m138` | `mBlurOffsetX` | br_draw (in daMo2_Draw 021C7344): the blur model sits at x = it + 150 on the spear joint MO_JNT_MO_YARI_e; __sinit_d_a_mo2_cpp 021CD5B4 -22.25; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0138 | `m13C` | `mHitPushback` | damage_check (in daMo2_Execute 021C9B1C): dr.mPushback = -it for a hit with mKnockbackPower < 25; __sinit_d_a_mo2_cpp 021CD5B4 30.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x013C | `m140` | `mBtMaxDistance` | daMo2_Execute 021C9B1C: mBtMaxDis (parry max distance) = it each frame; __sinit_d_a_mo2_cpp 021CD5B4 650.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0140 | `m144` | `mAttack1BtWindow1Start` | attack_set 021C9310 copies it to mo2_class mBtWindow1Start for attack kind 1 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 8; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0142 | `m146` | `mAttack1BtWindow1Len` | attack_set 021C9310 copies it to mo2_class mBtWindow1Len for attack kind 1 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 12; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0144 | `m148` | `mAttack1BtWindow2Start` | attack_set 021C9310 copies it to mo2_class mBtWindow2Start for attack kind 1 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 34; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0146 | `m14A` | `mAttack1BtWindow2Len` | attack_set 021C9310 copies it to mo2_class mBtWindow2Len for attack kind 1 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 25; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0148 | `m14C` | `mAttack2BtWindow1Start` | attack_set 021C9310 copies it to mo2_class mBtWindow1Start for attack kind 2 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 16; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x014A | `m14E` | `mAttack2BtWindow1Len` | attack_set 021C9310 copies it to mo2_class mBtWindow1Len for attack kind 2 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 12; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x014C | `m150` | `mAttack2BtWindow2Start` | attack_set 021C9310 copies it to mo2_class mBtWindow2Start for attack kind 2 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 45; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x014E | `m152` | `mAttack2BtWindow2Len` | attack_set 021C9310 copies it to mo2_class mBtWindow2Len for attack kind 2 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 30; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0150 | `m154` | `mAttack5BtWindow1Start` | attack_set 021C9310 copies it to mo2_class mBtWindow1Start for attack kind 5 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 8; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0152 | `m156` | `mAttack5BtWindow1Len` | attack_set 021C9310 copies it to mo2_class mBtWindow1Len for attack kind 5 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 12; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0154 | `m158` | `mAttack5BtWindow2Start` | attack_set 021C9310 copies it to mo2_class mBtWindow2Start for attack kind 5 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 40; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |
| 0x0156 | `m15A` | `mAttack5BtWindow2Len` | attack_set 021C9310 copies it to mo2_class mBtWindow2Len for attack kind 5 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 25; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) |

Open (no name yet): `m018` (1.0, never read) and the 16 floats `m08C` .. `m0C8` (0.9 / 1.0 / 1.1 / 0.5 / 1.2; the constructor sets them between the per-attack turn-end frames, but nothing reads them in WWHD or tww).

## ke_s and Mo2AtInfo (Moblin helper structs; ke_s in d_a_mo2.h, d_a_mo2_main.h, d_a_mo2_exec.h, d_a_mo2_move.h; Mo2AtInfo in d_a_mo2_exec.h)

ke_s is one spear-hair strand (mo2_class mSpearHair[16]); Mo2AtInfo is d_a_mo2_exec.h's copy of CcAtInfo; mo2Move::Sound is d_a_mo2_move.h's view of SND_INFLUENCE (d_kankyo.h).

| Struct +offset | Old | New | Evidence |
|---|---|---|---|
| ke_s +0x000 | `m000` | `mPointPos` | ke_control 021C6F14: the 10 spear-hair points, each placed 21.875 from the previous one (floor-clamped); nodeCallBack_P 021C6BE0 sets [0] on joint MO_JNT_MO_YARI_e; ke_disp (in daMo2_Draw 021C7344) copies them into the 3D line (tww the same; after our decomp's tt_ke_s mPos) |
| ke_s +0x078 | `m078` | `mPointVelocity` | ke_control 021C6F14: holds the old x / z, then (new - old) * damping, added to the next step's point (tww the same; after our decomp's tt_ke_s mVelocity) |
| Mo2AtInfo +0x00C | `m0C` | `mHitAngle` | cc_at_check 025192A8 sets .y = cc_atan2s(-x, -z) of the hit direction; damage_check (in daMo2_Execute 021C9B1C) rotates calc_mtx by .y (cMtx_YrotS) for the paralysis / knock-down hit reactions (tww the same; as the CcAtInfo copies in d_a_mt_exec.h / d_a_pt.h) |

Open: ke_s `m0F0` (tail gap 0x0F0..0x124, no use in WWHD or tww); mo2Move::Sound `field_0x0` (view of SND_INFLUENCE in d_kankyo.h, which another batch names; dKy_Sound_set copies the sound position there); the enemyice copy `m1B0` (gm, ks, ph set 1; read nowhere in WWHD or tww); `m00C` / `m028` / `m02C` are named in round 3 with the class (mFreezeType / mBodyLiftY / mIceOffsetY, see shared-types.md).
## daDaiocta_c (Big Octo; d_a_daiocta.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_daiocta.h) names part of this class and keeps the GameCube offset names for the rest; TP has no such actor. Every name below is from the WWHD use (d_a_daiocta.cpp); where tww is cited it is the same code. The joint names are tww's generated enum (assets/GZLE01/res/Object/Daiocta.h, DO_MAIN1_JNT). The HIO fields are set only by the HIO constructor (WWHD daDaiocta_HIO_ct 0211CD80, tww daDaiocta_HIO_c::daDaiocta_HIO_c) and never written elsewhere in WWHD or tww.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0691 | `m575` | `mExitNo` | getArg 021192CC: param bits 16-23; modeDemo 0211C59C passes it as the scls (exit) number of dComIfGp_setNextStage after the suction demo |
| 0x0698 | `m057C` | `mElfType` | getArg 021192CC: from home.angle.z; modeDelete 0211BFFC: at 1 orders DAIOCTA_DEAD_ELF instead of DAIOCTA_DEAD and plays no riddle jingle |
| 0x2320 | `m21A0` | `mTentacleLTipPos` | _nodeControl 021189D4: point 450 along joint 30 (J_DO_TE_L6); setCollision 0211A4BC: end of capsule 11 |
| 0x232C | `m21AC` | `mTentacleRTipPos` | _nodeControl 021189D4: point 450 along joint 36 (J_DO_TE_R6); setCollision 0211A4BC: end of capsule 10 |
| 0x2338 | `m21B8` | `mBodyTipPos` | _nodeControl 021189D4: point 450 along joint 6 (J_DO_BODY5); setCollision 0211A4BC: end of capsule 12 |
| 0x2344 | `m21C4` | `mFinL1TipPos` | _nodeControl 021189D4: point (300, -200, 0) on joint 9 (J_DO_HIRE_L1); setCollision 0211A4BC: end of capsule 15 |
| 0x2350 | `m21D0` | `mFinL2TipPos` | _nodeControl 021189D4: point 300 along joint 7 (J_DO_HIRE_L2); setCollision 0211A4BC: end of capsule 16 |
| 0x235C | `m21DC` | `mFinR1TipPos` | _nodeControl 021189D4: point (300, 200, 0) on joint 10 (J_DO_HIRE_R1); setCollision 0211A4BC: end of capsule 13 |
| 0x2368 | `m21E8` | `mFinR2TipPos` | _nodeControl 021189D4: point 300 along joint 8 (J_DO_HIRE_R2); setCollision 0211A4BC: end of capsule 14 |
| 0x2374 | `m21F4` | `mShipPos` | modeDemo 0211C59C: copy of the ship's current.pos, pulled to mMouthPos at mShipPullSpeed and put back into the ship (oct_initStartPos) |
| 0x2380 | `m2200` | `mShipAngle` | modeDemo 0211C59C: copy of the ship's current.angle; its y is passed to oct_initStartPos with mShipPos |
| 0x2388 | `m2208` | `mShipPullSpeed` | modeDemo 0211C59C: eased to l_HIO.mShipPullSpeedTarget; the cLib_chaseF / cLib_chasePosXZ step pulling mShipPos to the mouth |
| 0x238C | `m220C` | `mCreatePos` | createInit 02119A0C: copy of current.pos; modeDemo 0211C59C puts the body back there |
| 0x25B4 | `m2434` | `mMouthPos` | _nodeControl 021189D4: point mMouthOffsetX along joint 20 (J_DO_KUTI2); modeDemo 0211C59C: the suck / spit sounds and the ship pull go to it; _delete deletes its sound object |
| 0x2840 | `m26C0` | `mEnterSePlayed` | modeAppear 0211B7D8: set once the body is within 1900 of the surface target, with the JA_SE_CM_DO_ENTER sound and the battle BGM, which play only while it is 0 |
| 0x2841 | `m26C1` | `mSinkStarted` | modeDelete 0211BFFC: set when the body is 600 below the water, with initAwa, JA_SE_CM_DO_SINK_IN_SEA and the BGM stop, which run only while it is 0 |
| 0x590C | `m31C4` | `mAwaRepeat` | modeDelete 0211BFFC sets 1 when the sinking starts and 0 at the end; execAwa 02119F84 restarts the finished bubble animations while it (or l_HIO.mAwaRepeatAlways) is set |

### daDaiocta_HIO_c (l_HIO)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x004 | `m004` | `mDrawDebug` | _draw 0211B420 calls drawDebug while it is not 0 (tww the same) |
| 0x005 | `m005` | `mHideBody` | _draw 0211B420 skips mpMorf->entryDL (the body) while it is not 0 (tww the same) |
| 0x006 | `m006` | `mForceDelete` | oct_checkTransitions (in modeWait 0211BB9C): goes to MODE_DELETE when it is not 0 as when isDead (tww modeWait the same) |
| 0x007 | `m007` | `mForceAnm` | _execute 0211AE68: while it is not 0, mPrmIdx = mForceAnmIdx and setAnm again (tww the same) |
| 0x008 | `m008` | `mAwaRepeatAlways` | execAwa 02119F84: restarts the finished bubble animations while it or mAwaRepeat is set (tww the same) |
| 0x00A | `m00A` | `mForceAnmIdx` | _execute 0211AE68: the mPrmIdx used while mForceAnm is set (tww the same) |
| 0x010 | `m010` | `mAppearTurnStep` | modeAppear 0211B7D8: max step of cLib_addCalcAngleS2 turning shape_angle.y to the player |
| 0x012 | `m012` | `mSuckAngle` | _execute 0211AE68: the suction demo starts when the player is within mSuckRange and the angle to the player off shape_angle.y + mSuckAngleOffset is below it; modeDemo 0211C59C: the ship is pulled sideways only within it |
| 0x014 | `m014` | `mSuckAngleOffset` | _execute 0211AE68, modeDemo 0211C59C: added to shape_angle.y before cLib_distanceAngleS to the player / ship (the mouth direction) |
| 0x016 | `m016` | `mAwaNum` | setAwaMtx 0211970C, initAwa 02119514, execAwa 02119F84, drawAwa: number of bubbles used (loop bound over the 30 mpAwaModels) |
| 0x018 | `m018` | `mEffectColorAdd` | setEffect 0211A9A0 and _execute: added to the sea ambient r / g / b (capped at 0xFF) for the particle colour |
| 0x05C | `m05C` | `mTentacleRTipRadius` | setCollision 0211A4BC: radius of capsule 10 (joint 36 J_DO_TE_R6 to mTentacleRTipPos) |
| 0x060 | `m060` | `mTentacleLTipRadius` | setCollision 0211A4BC: radius of capsule 11 (joint 30 J_DO_TE_L6 to mTentacleLTipPos) |
| 0x064 | `m064` | `mBodyTipRadius` | setCollision 0211A4BC: radius of capsule 12 (joint 6 J_DO_BODY5 to mBodyTipPos) |
| 0x068 | `m068` | `mFinR1Radius` | setCollision 0211A4BC: radius of capsule 13 (joint 10 J_DO_HIRE_R1 to mFinR1TipPos) |
| 0x06C | `m06C` | `mFinR2Radius` | setCollision 0211A4BC: radius of capsule 14 (joint 8 J_DO_HIRE_R2 to mFinR2TipPos) |
| 0x070 | `m070` | `mFinL1Radius` | setCollision 0211A4BC: radius of capsule 15 (joint 9 J_DO_HIRE_L1 to mFinL1TipPos) |
| 0x074 | `m074` | `mFinL2Radius` | setCollision 0211A4BC: radius of capsule 16 (joint 7 J_DO_HIRE_L2 to mFinL2TipPos) |
| 0x080 | `m080` | `mAppearRiseStep` | modeAppear 0211B7D8: max step of cLib_addCalcPos2 moving the body up to the water surface |
| 0x084 | `m084` | `mSuckRange` | _execute 0211AE68: the suction demo (MODE_DEMO) starts when the player is nearer than it (and within mSuckAngle) |
| 0x088 | `m088` | `mShipPullSpeedTarget` | modeDemo 0211C59C: target of cLib_addCalc2 on mShipPullSpeed |
| 0x08C | `m08C` | `mShipPullSpeedStep` | modeDemo 0211C59C: max step of cLib_addCalc2 on mShipPullSpeed |
| 0x090 | `m090` | `mShipPullSpeedScale` | modeDemo 0211C59C: scale of cLib_addCalc2 on mShipPullSpeed |
| 0x094 | `m094` | `mMouthOffsetX` | _nodeControl 021189D4: x offset on joint 20 (J_DO_KUTI2) for mMouthPos |
| 0x098 | `m098` | `mAnmPlaySpeed` | setAnm 0211AB80: copied into the play speed of the nine animation parameter entries (tww anm_prms[i].mPlaySpeed) |
| 0x0BC | `m0BC` | `mAnmMorf` | setAnm 0211AB80: copied into the morf of the nine animation parameter entries (tww anm_prms[i].mMorf) |
| 0x0E0 | `m0E0` | `mSinkStep` | modeDelete 0211BFFC: cLib_chasePos step of the sinking body; within 3 times it of the target the demo ends |
| 0x0E4 | `m0E4` | `mAwaDistMin` | setAwaRandom 021193F8: a bubble is placed at a random distance from mAwaDistMin to mAwaDistMax around current.pos |
| 0x0E8 | `m0E8` | `mAwaDistMax` | setAwaRandom 021193F8: upper end of the random bubble distance |
| 0x0EC | `m0EC` | `mAwaScaleMin` | setAwaRandom 021193F8: a bubble's scale is random from mAwaScaleMin to mAwaScaleMax |
| 0x0F0 | `m0F0` | `mAwaScaleMax` | setAwaRandom 021193F8: upper end of the random bubble scale |
| 0x0F4 | `m0F4` | `mAwaTimerMin` | initAwa 02119514 / execAwa 02119F84: mAwaTimers[i] = random from mAwaTimerMin to mAwaTimerMax |
| 0x0F8 | `m0F8` | `mAwaTimerMax` | initAwa 02119514 / execAwa 02119F84: upper end of the random bubble timer |
| 0x0FC | `m0FC` | `mSuikomiOffset` | setSuikomiMtx 02119630: mDoMtx_stack_transM offset of the suction (suikomi) model |

Open (no name yet): the padding / unused members of daDaiocta_c (`m0568`, `m062C`, `m0692`, `m0699`, `m2386`, `m25FC`, `m2842`, `m590D`) and of daDaiocta_HIO_c (`m009`, `m01A`).

## daRd_c (ReDead; d_a_rd.h)

zeldaret/tww (d217e2b, include/d/actor/d_a_rd.h) names part of this class and keeps the GameCube offset names for the rest. Every name below is from the WWHD use (d_a_rd.cpp and the inline rd_breakFree in d_a_rd.h); where tww is cited it is the same code.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3E0 | `m2C4` | `mTgHitWait` | checkTgHit 0245C770: a hit is taken only while it has counted down to 0; reloaded with l_HIO m48 at every hit, 3 after a strong hit (resulting attack type 1) (tww the same) |
| 0x8A8 | `m6D4` | `mFireDeathTimer` | _execute 0245BF50: reloaded with l_HIO m4E while mEnemyFire is off; while burning it counts down and at 0 the ReDead dies (MODE_DEATH) unless already attacking / crying / dead |
| 0x8AF | `m6DB` | `mBtkAnmPrmIdx` | setBtkAnm 0245AC14: the btk_anm_prm_tbl entry to play, follows the entry's m01 at a non-looping end (as mAnmPrmIdx for the bck) |
| 0x8B0 | `m6DC` | `mOldBtkAnmPrmIdx` | setBtkAnm 0245AC14: the entry last started; the btk is re-initialised only when mBtkAnmPrmIdx differs (as mOldAnmPrmIdx) |
| 0xEB8 | `mCE0` | `mHitReactTimer` | checkTgHit 0245C770: 40 for a non-damaging wind / boomerang / stick / far arrow / grappling hit; _execute 0245BF50 tilts the head (mHeadRotY to 0x2000) while it counts down and starts mLookPlayerTimer when it reaches 1 |
| 0xEBC | `mCE4` | `mLookPlayerTimer` | _execute 0245BF50: 4 * 30 when mHitReactTimer ends; lookBack 0245B9A8 turns the head to the player's head while it counts down (not while attacking / crying / dead) |
| 0xEC0 | `mCE8` | `mHeadMtx` | _nodeHeadControl 0245A4DC: copy of the head joint matrix; debugDraw 0245C49C draws its axes (tww the same) |
| 0xEF2 | `mD1A` | `mHeadRotX` | _nodeControl 0245A21C: extra X rotation of the head joint after the m_jnt angles; never written in WWHD or tww (always 0) |
| 0xEF4 | `mD1C` | `mHeadRotZ` | _nodeControl 0245A21C: extra Z rotation of the head joint after mHeadRotX; never written in WWHD or tww (always 0) |
| 0xEF6 | `mD1E` | `mHeadRotY` | _nodeControl 0245A21C: extra Y rotation of the head joint; _execute 0245BF50 eases it to 0x2000 while mHitReactTimer runs, else back to 0 (tww the same) |
| 0xF10 | `mD38` | `mHeadOffsetX` | _nodeHeadControl 0245A4DC: transM(it, 0, 0) on the head joint; _execute 0245BF50 eases it to l_HIO m60 while isIkari (angry), else to 0 (tww the same) |
| 0xF14 | `mD3C` | `mStickDirX` | createInit 0245B234 sets 1; rd_breakFree (d_a_rd.h, in modeCry 0245DD80 / modeAttack 0245E30C) flips it to -1 / 1 when the stick x changes side and counts mBreakFreeCounter down (tww the same) |
| 0xF18 | `mD40` | `mStickDirY` | createInit 0245B234 sets 1; rd_breakFree (d_a_rd.h, in modeCry 0245DD80 / modeAttack 0245E30C) flips it to -1 / 1 when the stick y changes side and counts mBreakFreeCounter down (tww the same) |

Open (no name yet): `mD2E` (_execute 0245BF50 eases it to 0x500 / 0 with mHeadRotY, never read; tww the same), the padding (`m2B0`, `m2C6`, `m30E`, `m6DD`, `mCDE`, `mD20`, `mD30`, `mD35`), and the local copies' `enemyice_l` `m1B0` (no read) / `enemyfire_l` `m228` (the LIGHT_INFLUENCE HD tail mHD20 of enemyfire::mLight, see shared-types.md) in d_a_rd.h; `m00C` / `m028` / `m02C` follow the class in round 3.


## daRd_HIO_c (l_HIO, the ReDead tuning; d_a_rd.h)

Round 10. WWHD keeps the vtable first here (offsets equal the GameCube names). daRd_HIO_c_ct 0245EFE0 sets every field; tww's daRd_HIO_c (d_a_rd.cpp, offset names kept) reads them the same way. mCryRadius, mAttackRadius, mCrySpreadAngle, mAttackSpreadAngle, mReturnWalkSpeed and mParalysisDuration were named earlier.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x2C | `m2C` | `mDebugDraw` | _draw 0245C620 calls debugDraw while it is set (debug switch; daRd_HIO_c_ct 0245EFE0 clears it) (tww the same) |
| 0x30 | `m30` | `mAreaRadiusAdd` | getArg 0245AB98: mAreaRadius = it + the parameter's area radius (tww the same) |
| 0x34 | `m34` | `mNoticeRadius` | the ReDead's notice range: rd_noticePlayer (checkInFan radius, with mNoticeSpreadAngle), _searchNearDeadRd 0245A194 (a dead ReDead nearer than it) and debugDraw 0245C49C's fan (tww the same) |
| 0x40 | `m40` | `mNoticeSpreadAngle` | rd_noticePlayer: checkInFan's spread angle (with mNoticeRadius); debugDraw 0245C49C's fan angle (tww the same) |
| 0x46 | `m46` | `mMaxHealth` | createInit 0245B234: max_health (tww the same) |
| 0x48 | `m48` | `mHitWaitTime` | checkTgHit 0245C770: mTgHitWait after a hit (tww the same) |
| 0x4A | `m4A` | `mFreezeTime` | checkTgHit 0245C770: mEnemyIce.mFreezeDuration on an ice hit (tww the same) |
| 0x4C | `m4C` | `mFireTime` | checkTgHit 0245C770: mEnemyFire.mFireDuration on a fire hit (tww the same) |
| 0x4E | `m4E` | `mFireDeathTime` | _execute 0245BF50: mFireDeathTimer (tww the same) |
| 0x50 | `m50` | `mAttackBreakFreeCount` | modeAttackInit 0245E298: mBreakFreeCounter, the stick turns needed to break free of the grab (tww the same) |
| 0x52 | `m52` | `mAttackTime` | modeAttackInit 0245E298: mTimer1 of the grab attack (tww the same) |
| 0x54 | `m54` | `mCryTime` | modeCryInit 0245DC9C: mTimer1 of the paralysing cry (tww the same) |
| 0x58 | `m58` | `mAttentionOffsetY` | setAttention 0245B854: added to the attention position's y (tww the same) |
| 0x5C | `m5C` | `mEyeOffsetY` | setAttention 0245B854: added to eyePos.y (tww the same) |
| 0x60 | `m60` | `mIkariHeadOffsetX` | _execute 0245BF50: mHeadOffsetX eases to it while isIkari (tww the same) |
| 0x64 | `m64` | `mHeadOffsetStep` | _execute 0245BF50: cLib_addCalc2's step of mHeadOffsetX (tww the same) |
| 0x68 | `m68` | `mWalkSpeed` | rd_walkToPlayer (modeMove / modeCry, d_a_rd.h): speedF eases to it * the distance factor, doubled while burning (tww modeMove / modeCry the same) |
| 0x70 | `m70` | `mWalkAnmSpeedRate` | _execute 0245BF50: the walk animation's play speed is speedF * it (at least mWalkAnmSpeedMin) (tww the same) |
| 0x74 | `m74` | `mWalkAnmSpeedMin` | _execute 0245BF50: the lower bound of the walk animation's play speed (tww the same) |
| 0x78 | `m78` | `mCryBreakFreeCount` | modeCryInit 0245DC9C: mBreakFreeCounter, the stick turns needed to break free of the cry's paralysis (tww the same) |

Open (no name yet): `m7C` (the JntHit_HIO_c member, kept as a byte view; constructJntHitHIO only).


## daFm_HIO_c view in d_a_fm_b.h (Floormaster part B)

d_a_fm_b.h's private view daFm_HIO_c of the Floormaster HIO (fm_main::HIO, d_a_fm_main.h) takes the names the class got in round 2 (views follow their class): `field_0x00F` -> `mIkJointOn`, `field_0x088` -> `mFarTime`, `field_0x08C` -> `mWaitTimeMin`, `field_0x08E` -> `mWaitTimeMax`, `field_0x094` -> `mAttackTime`, `field_0x096` -> `mTurnStep`, `field_0x098` -> `mGrabAngleMax`, `field_0x09A` -> `mTurnAngleMin`, `field_0x09C` -> `mTgHitTime`, `field_0x0A8` -> `mHeightRange`, `field_0x0BC` -> `mNpcDemoPlayerDist`, `field_0x0C4` -> `mReachDist`, `field_0x0CC` -> `mChaseSpeed`, `field_0x0DC` -> `mFootGrabDist`, `field_0x0E0` -> `mNearRange`, `field_0x0E4` -> `mAttackRange`, `field_0x0E8` -> `mNoticeRange`, `field_0x0EC` -> `mGrabReach`, `field_0x100` -> `mGrabDelayTime`, `field_0x110` -> `mPullDistMin`, `field_0x114` -> `mPullDistMax`, `field_0x118` -> `mPullPower`. `field_0x08A` stays open as in the class.

## tn_p (Darknut armour part, tn_class::mParts[3]; d_a_tn.h)

Round 4 (two-digit offset names). The state, motion and timer of each armour part that comes off and flies away (part_move, its inline tn_moveFlyingPart, part_draw). Same meanings in zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x08 | `m08` | `mPartState` | part_move 024BC3EC: 0 follows the body joint, 1 coming loose (set when the actor's part-break bits ask for it), 5 flying, -1 broken (yoroi_break); part_draw 024BA0F0 skips the part below 0 (tww the same) |
| 0x09 | `m09` | `mbDetached` | part_move 024BC3EC: 0 takes the body joint's matrix, 1 (set when the part comes off) builds the matrix from mPos and mAngle (tww the same) |
| 0x0C | `m0C` | `mPos` | part_move 024BC3EC: the part's position, taken from the body joint while attached; tn_moveFlyingPart (inline of part_move) adds mSpeed each frame; yoroi_break's effect position (tww the same) |
| 0x18 | `m18` | `mOldPos` | tn_moveFlyingPart (inline of part_move 024BC3EC): mPos before this frame's step, restored on a wall hit and used for the motion vector (tww the same) |
| 0x24 | `m24` | `mSpeed` | tn_moveFlyingPart (inline of part_move 024BC3EC): added to mPos each frame, y minus 5 per frame (gravity), bounced and damped on the ground (tww the same) |
| 0x30 | `m30` | `mAngle` | part_move 024BC3EC: the flying part's rotation (Y, X, Z rotM) and its start angle; tn_moveFlyingPart adds mAngleSpeed (tww the same) |
| 0x36 | `m36` | `mAngleSpeed` | tn_moveFlyingPart (inline of part_move 024BC3EC): added to mAngle each frame; random on launch and bounce, 0 when the part settles (tww the same) |
| 0x44 | `m44` | `mTimer` | part_move 024BC3EC counts it down each frame: the delay before a loose part flies, then 30 / 20 frames until it breaks at 1 (tww the same) |

Open (no name yet): `m42` (part_move only increments it; read nowhere else in WWHD or tww), padding `_0A`, `_3C`, `_45`.

## Stalfos::HIO_c (l_HIO; d_a_st.h)

Round 5 (two-digit offset names). The Stalfos HIO: the two step windows of the patrol walk and the patrol speed, the coffin wait and the reassemble time. Same meanings in zeldaret/tww d217e2b (where the vtable comes first, so tww's offsets are 4 higher).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x02 | `m06` | `mStepStartFrame1` | jyunkai (inlined in St_move 024972A4): from this walk frame to mStepEndFrame1 the Stalfos moves at mPatrolSpeed and turns, and on it the step sound 0x3832 plays; static_initialize 024960B4 9 (tww the same) |
| 0x04 | `m08` | `mStepStartFrame2` | jyunkai (inlined in St_move 024972A4): the second step window, from this walk frame to mStepEndFrame2, the step sound plays on it; static_initialize 024960B4 0x2C (tww the same) |
| 0x06 | `m0A` | `mStepEndFrame1` | jyunkai (inlined in St_move 024972A4): the last walk frame of the first step window; static_initialize 024960B4 0x15 (tww the same) |
| 0x08 | `m0C` | `mStepEndFrame2` | jyunkai (inlined in St_move 024972A4): the last walk frame of the second step window; static_initialize 024960B4 0x35 (tww the same) |
| 0x0C | `m10` | `mPatrolSpeed` | jyunkai (inlined in St_move 024972A4): mTargetSpeed inside the step windows (0 outside); static_initialize 024960B4 6.0 (tww m02DC = it) |
| 0x14 | `m18` | `mCoffinWaitTime` | kan_wait (inlined in St_move 024972A4): after the ambush trigger mTimers[0] = it (mTimers[2] = it + 20); the coffin animation waits until it runs out; static_initialize 024960B4 0x46 (tww the same) |
| 0x16 | `m1A` | `mReassembleTime` | st_break_wait 024963A4: on entering the broken wait mTimers[0] and mReassembleWait = it; when the timer runs out the parts are called back (mPartCommand 2); static_initialize 024960B4 200 (tww the same) |

Open (no name yet): `m14` (set to 7.0 by static_initialize 024960B4, read nowhere in WWHD or tww), padding `mPad01`, `mPad0A`.

## st_p (st_class::mParts[]; d_a_st.h)

Round 5 (two-digit offset names). One Stalfos body part: whether it is loose, and the angles, wobble counter and speed of its flight back to its joint (part_move state 1). Same meanings in zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x05 | `m05` | `mDetached` | part_move 024925B8: 0 while the part follows its joint matrix, 1 while it moves on its own (switch at the end); scatter_part (inlined) and the break code set 1, the return to the joint sets 0 (tww the same) |
| 0x38 | `m38` | `mFlyAngleX` | part_move 024925B8 state 1: the part's pitch toward its joint, eased by cLib_addCalcAngleS2 (step 0x2000) and used for the flight matrix; the head (joint 15) takes it as mPartRot.x (tww the same) |
| 0x3A | `m3A` | `mFlyAngleY` | part_move 024925B8 state 1: the part's yaw toward its joint, eased by cLib_addCalcAngleS2 and used for the flight matrix; the head takes it as mPartRot.y (tww the same) |
| 0x3C | `m3C` | `mFlyAngleZ` | part_move 024925B8 state 1: the head (joint 15) takes it as mPartRot.z with mFlyAngleX / mFlyAngleY; never written in WWHD or tww (tww the same read) |
| 0x3E | `m3E` | `mWobbleCount` | part_move 024925B8: counts up every frame (random start when a part breaks off); in state 1 the flight back wobbles by sin(it * 1000) / cos(it * 1100) / cos(it * 1200) (tww the same) |
| 0x44 | `m44` | `mFlySpeed` | part_move 024925B8 state 1: the flight step is it * 20 along the flight direction, eased toward 1 by 0.01; 0 or 0.5 when the part breaks off (tww the same) |

Open (no name yet): `m48` (the dPa_smokeEcallBack, constructed by part_construct 0249A0B0 and used nowhere else in WWHD or tww), padding `pad06`, `pad41`, `pad68`.


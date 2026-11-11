# Field names: camera

Fields the decomp still called by offset (`mXXXX`, `field_0x…`, `unk…`; zeldaret/tww keeps many of these GameCube offset
names) get names unit by unit (field naming pass, 2026-11-10). Rules: a name comes from how the original
code uses the field (address of the use and what it shows), from names already in this decomp, or from the CC0
zeldaret/tww and zeldaret/tp decomps ("TP name": the same field in TP's `dCamera_c`, matched by position and use).
Renames only: offsets, types and code are unchanged (objcmp: object-identical). The header line keeps the old name
(`was mXXX`) with the evidence. Fields without enough evidence keep their offset name and are listed as open.

## dCamera_c (d_camera.h; copies in d_ev_camera_5.h, d_ev_camera_6.h, d_camera_accessors.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x068 | `m068` | `mBumpCheckFlags` | Run 024FE3E8 sets 9/0x3F/0xF, ORs 0x40, passes it to bumpCheck; TP name |
| 0x070 | `m070` | `mCornerPos` | bumpCheck 024FD11C, corner hit (type 3): the point on the two walls' line near the middle of the two hits (cM3d_2PlaneLinePosNearPos), moved 2 units out along the summed plane normals; the camera direction is eased toward it from mCenter and the eye line check runs from it to the new eye; no other WWHD use (tww d_camera.cpp bumpCheck the same; TP keeps the hit point as mLastHitPos) |
| 0x07C | `m07C` | `mFrameCounter` | cleared by initialize 02500984, +1 per frame in Run 024FE3E8 and NotRun 024FF6C0; TP name |
| 0x080 | `m080` | `mTicks` | initialize 02500984 seeds it with cM_rndFX(32767), Run 024FE3E8 adds 1, event cameras test bit 0 (025314BC); TP name |
| 0x084 | `m084` | `mSavedCenter` | pushPos 024FA3CC copies mViewCache.mCenter |
| 0x090 | `m090` | `mSavedEye` | pushPos 024FA3CC copies mViewCache.mEye |
| 0x09C | `m09C` | `mSavedFovy` | pushPos 024FA3CC copies mViewCache.mFovy |
| 0x0A0 | `m0A0` | `mSavedBank` | pushPos 024FA3CC copies mViewCache.mBank |
| 0x0A4 | `m0A4` | `mSavedViewStack` | saveEvCamera 02538B54 / loadEvCamera 02538C7C store and load slot [index]; TP name |
| 0x0E8 | `m0E8` | `mEndEventIdCheck` | EndEventCamera 0253E860 (800300E8 lwz +0xE8): while it is not -1, an end request only ends the event camera when its id is mEventData field_0x14 (StartEventCamera 0253E70C's id); initialize 02500984 sets -1 and the constructor 02501B8C clears it with mStageMapToolCameraIdx; never set otherwise in WWHD or tww (tww initialize the same), so the id check is off |
| 0x100 | `m100` | `mStyleSettle0` | engines set m100..m102 together when the view reached its target (0250C77C); onModeChange 024FA79C clears them; Run 024FE3E8 sets ComStat 0x10 only when all three are set (TP: one flag, mStyleSettle.mFinished) |
| 0x101 | `m101` | `mStyleSettle1` | set and cleared together with mStyleSettle0 (0250C77C, 024FA79C); Run 024FE3E8 needs all three |
| 0x102 | `m102` | `mStyleSettle2` | set and cleared together with mStyleSettle0 (0250C77C, 024FA79C); Run 024FE3E8 needs all three |
| 0x108 | `m108` | `mCurModeTimer` | onModeChange 024FA79C clears it, Run/NotRun (024FE3E8/024FF6C0) add 1; engines compare it with their transition length (02511200) |
| 0x110 | `m110` | `mModeChangeTransition` | onModeChange 024FA79C sets 1, clears it when mode 0<->1 keeps the same style; followCamera 025028B8 settles the view at once when it is 0 |
| 0x118 | `m118` | `mCurTypeTimer` | onTypeChange 024FA90C clears it, Run/NotRun add 1; eventCamera tests == 0 (024FF164) |
| 0x11C | `m11C` | `mCurStyleTimer` | onStyleChange 024FB334 clears it, Run/NotRun add 1; engines compare it with their transition frames (0250C77C); TP mCurCamStyleTimer |
| 0x144 | `m144` | `mManualInactive` | initialize 02500984 sets 1; nextMode 024FA9C8 clears it when the C stick passes the manual-start threshold and sets 1 to leave; onTypeChange 024FA90C sets 1 for fixed/event styles |
| 0x148 | `m148` | `mForwardTiltOffset` | initialize 02500984 sets cSAngle 0; followCamera 025028B8 / towerCamera 0250B30C ease it by 0.33; TP name at the same place (before mSightFitRadius) |
| 0x14C | `m14C` | `mSightFitRadius` | bumpCheck 024FD11C eases it toward the sight radius and adds it to the radius; onModeChange 024FA79C clears it; TP name |
| 0x154 | `m154` | `mSmoothedCStickX` | HD-only float (GC 0x154 is mStickMainPosXLast, +4 from here): manualCamera 0250FDC8 chases it toward the curved C stick x turn (cLib_chaseF, 0.25 while the stick is centred, 0.1, or 0.18 when the sign flips) and turns the camera by it * 0.92 (negated by the save option camera x); Run 024FE3E8 clears it on the change to a subject style; the constructor 02501B8C clears it |
| 0x188 | `m184` | `mCStickYState` | initPad 025006D8 clears it with m188/m18C (TP initPad clears the same three); nextMode 024FA9C8 sets 0/1 for the C stick Y direction; TP name |
| 0x18C | `m188` | `mCStickYHoldCount` | cleared by initPad 025006D8 with mCStickYState (TP initPad: the same three in this order); TP name |
| 0x190 | `m18C` | `mCStickUpLatch` | cleared by initPad 025006D8 with mCStickYState (TP initPad: the same three in this order); TP name |
| 0x19E | `m19A` | `mLockLActive` | updatePad 024F9BF4: 1 while the L analog value is above ManualEndVal; TP name |
| 0x19F | `m19B` | `mLockLJustActivated` | updatePad 024F9BF4: m19A ^ 1 before m19A is set (first frame only); TP name |
| 0x1AA | `m1A6` | `mLockRActive` | updatePad 024F9BF4: 1 while the R analog value is above ManualEndVal; TP name |
| 0x1AB | `m1A7` | `mLockRJustActivated` | updatePad 024F9BF4: m1A6 ^ 1 before m1A6 is set (first frame only); TP name |
| 0x24C | `m248` | `mSeIds` | initialize 02500984 stores JA_SE_MAN_CAMERA_NG / JA_SE_CAMERA_TO_MANUAL / JA_SE_ATM_PRT_SHIP_CREAK; Run 024FE3E8 starts [i] when bit i of m254 is new |
| 0x258 | `m254` | `mSeRequest` | nextMode 024FA9C8 / onModeChange 024FA79C / Run (ship rock) set bits 1/2/4; Run 024FE3E8 starts mSeIds[i] for new bits, then clears it |
| 0x25C | `m258` | `mSeRequestLast` | Run 024FE3E8 keeps last frame's m254 here and plays only bits not set in it |
| 0x318 | `m314` | `mInSeaArea` | checkGroundInfo 024F9044: 1 when daSea_ChkArea(player x, z), else 0 |
| 0x31C | `m318` | `mSeaWaveHeight` | checkGroundInfo 024F9044: daSea_calcWave(player x, z), else -1e9; rideCamera 0250D4E8 compares it with m354 |
| 0x320 | `m31C` | `mOnMoveBg` | checkGroundInfo 024F9044: 1 while the ground under the player is a moving BG, 0 otherwise; Run 024FE3E8 tests it |
| 0x321 | `m31D` | `mFollowMoveBg` | checkGroundInfo 024F9044: 1 when the view is carried with the moving BG (no event camera, player on the floor) |
| 0x324 | `m320` | `mMoveBgPosDelta` | checkGroundInfo 024F9044: m32C - the moving BG actor's position (last minus current) |
| 0x330 | `m32C` | `mMoveBgLastPos` | checkGroundInfo 024F9044: the moving BG actor's position, kept for the next frame |
| 0x33C | `m338` | `mMoveBgAngleDelta` | checkGroundInfo 024F9044: m33A - the moving BG actor's direction |
| 0x33E | `m33A` | `mMoveBgLastAngle` | checkGroundInfo 024F9044: the moving BG actor's direction, kept for the next frame |
| 0x340 | `m33C` | `mpMoveBgActor` | checkGroundInfo 024F9044: GetActorPointer of the moving BG under the player, else NULL |
| 0x354 | `m350` | `mCamMoveBg` | checkGroundInfo 024F9044: dBgS::GetCamMoveBG of the player's ground, else 0; nextType 024FA410 reads it |
| 0x358 | `m354` | `mGroundHeight` | checkGroundInfo 024F9044: ground cross height under the camera check point, raised to mSeaWaveHeight |
| 0x364 | `m360` | `mPlayerOnFloor` | checkGroundInfo 024F9044: 0 when the player's feet are more than FloorMargin above the ground, else 1 |
| 0x368 | `m364` | `mBushHit` | checkGroundInfo 024F9044: lineCollisionCheckBush(...) & 5; bumpCheck 024FD11C tests it for CheckBush |
| 0x36C | `m368` | `mBushHeight` | checkGroundInfo 024F9044: mCamSetup mBushHigh / mBushLow by the bush hit bits, else 0; talk camera 02508A60 adds it to the eye limit |
| 0x370 | `m36C` | `mBushTopPos` | checkGroundInfo 024F9044: GetMassCamTopPos output; bumpCheck 024FD11C uses it |
| 0x518 | `m514` | `mHoldType` | nextType 024FA410 (891E0518 lbz, a byte in WWHD) keeps the current camera type while it is not 0 (the event type still applies first; no map-tool, Seagal or player-state type change); Run 024FE3E8, initialize 02500984 and the constructor 02501B8C only store 0; never set non-zero in WWHD or tww (tww nextType reads it as != 1, tww writes only 0) |
| 0x528 | `m524` | `mForceType` | SetType 02514EB8 stores the type when it is 0xFF; nextType 024FA410 uses and resets it to 0xFF |
| 0x52C | `m528` | `mpForceTypeActor` | SetType 02514EB8 stores the actor with mForceType; nextType 024FA410 reads and clears it |
| 0x534 | `m530` | `mShipRockType` | initialize 02500984: 1 Asoko/Abship/PShip, 2 Obshop, 3 A_umikz, else 0; Run 024FE3E8 rocks the view while it is not 0 |
| 0x538 | `m534` | `mShipRockAngle` | initialize 02500984 clears it; daObjPirateship::getShipOffsetY 025269A8 (our d_com_static.cpp, parameter `angle`) adds mShipRockSpeed to it and returns scale * sin of it |
| 0x53A | `m536` | `mShipRockSpeed` | initialize 02500984 sets 0x180; getShipOffsetY 025269A8 (parameter `speed`) eases it toward 0x180 while |angle| <= 0x4000, else 0x280, and adds it to mShipRockAngle |
| 0x53C | `m538` | `mShipOffsetY` | initialize 02500984 / Run 024FE3E8: getShipOffsetY(...) * mShipOffsetScale, kept for the next frame |
| 0x540 | `m53C` | `mShipOffsetYDelta` | Run 024FE3E8: this frame's ship offset minus mShipOffsetY; ship creak sound when it turns upward |
| 0x544 | `m540` | `mShipOffsetScale` | initialize 02500984: 1.0 / 0.12 / 1.0 by mShipRockType, else 0; Run 024FE3E8 scales the ship offset by it |
| 0x548 | `m544` | `mShakePattern` | StartShake 02515298 copies the i_pattern bits (length mShakeLength) |
| 0x54C | `m548` | `mShakeCyclePattern` | StartShake 02515298 copies the pattern, the last byte wrapped with the first for length 0x20; shakeCamera 024FC108 reads it |
| 0x554 | `m550` | `mShakeLength` | StartShake 02515298 stores i_length (max 0x20); StopShake 025153D4 clears it; shakeCamera 024FC108 shakes while the frame is below it |
| 0x558 | `m554` | `mShakeFrame` | StartShake 02515298 / StopShake clear it; shakeCamera 024FC108 reads it as the pattern position and adds 1 |
| 0x560 | `m55C` | `mShakeDirection` | StartShake 02515298 stores the normalised i_pos; shakeCamera 024FC108 scales it |
| 0x58C | `m588` | `mShakeFlags` | StartShake 02515298 stores i_flags, StopShake clears it; shakeCamera 024FC108 tests its bits |
| 0x590 | `m58C` | `mBlureType` | ResetBlure 0250D464 stores its argument; the blur part of shakeCamera 024FC108 branches on 0/1 |
| 0x60C | `m608` | `mWallUpDist` | initialize 02500984: mCamSetup.mBGChk.WallUpDistance(); TP name |
| 0x8D4 | `m780` | `mIsSeaMap` | initialize 02500984: is_current_map("sea"); followCamera 025028B8 reads it |
| 0x8D5 | `m781` | `mIsShipRoomMap` | initialize 02500984: is_current_map("Asoko"/"Abship"/"PShip"); selects mShipRockType 1 |
| 0x8D6 | `m782` | `mIsObshopMap` | initialize 02500984: is_current_map("Obshop"); selects mShipRockType 2 |
| 0x8D7 | `m783` | `mIsUmikzMap` | initialize 02500984: is_current_map("A_umikz"); selects mShipRockType 3 |
| 0x8D8 | `m784` | `mIsGanonKMap` | initialize 02500984: is_current_map("GanonK"); freeCamera 0250FDC8 tests it |
| 0x8D9 | `m785` | `mIsGTowerMap` | initialize 02500984: is_current_map("GTower") |
| 0x8DA | `m786` | `mInSeaCenter` | checkSpecialArea 024F8DB4: 1 when the player is within sSeaRadius of sSeaCenter on the sea map |
| 0x8DB | `m787` | `mInKazeCenter` | checkSpecialArea 024F8DB4: 1 when the player is within sKazeRadius of sKazeCenter on the kaze map; followCamera 025028B8 tests it |
| 0x8DC | `m788` | `mIsKazeMap` | initialize 02500984: is_current_map("kaze") |
| 0x8DD | `m789` | `mIsMDaiMap` | initialize 02500984: is_current_map("M_Dai") |
| 0x8DF | `m78B` | `mIsKazeBMap` | initialize 02500984: is_current_map("kazeB"); bumpCheck 024FD11C tests it |

Open (no name yet; checked again against every WWHD reader and writer, tww d217e2b and TP):

- `m006`: no use in WWHD or tww.
- `m064`: only set to 1.0 by initialize 02500984 (tww the same; TP's field at this place, field_0x84, is also only set to 1.0).
- `m104` / `m106`: only constructed (constructor 02501B8C); no use in tww (TP's mStyleSettle angles there are also unused).
- `m10C`: only cleared (onModeChange); tww the same; TP's field_0x164 at this place is also only cleared.
- `m114`: only cleared (onTypeChange, the constructor 02501B8C); tww the same; TP's field_0x16c is also only cleared.
- `m150`: HD padding (GC 0x150 is u8[4]); only cleared by the constructor 02501B8C.
- `m1AE`: never set non-zero in WWHD or tww; it is cleared in four places and read twice (crawlCamera: a 6- instead of 10-frame start while crawling, and setComStat 0x80), two effects with no setter to tell what it requests.
- `m1AF`, `m340`, `m54C`, `m558`, `m59A`, `m768`, `m78A`: no use in WWHD or tww.
- `m52C`: only cleared by the constructor 02501B8C; tww has padding there.
- `m5F4`: only cleared (the constructor 02501B8C, initialize 02500984); tww the same.
- The nested helper structs (`dCamForcusLine`, `DMC`, `Monitor`, `BGChk`, `EventData`) are not part of this pass.


## dCamForcusLine (dCamera_c::mForcusLine, the focus-line effect; d_camera.h, view camera_util::dCamForcusLine in d_camera_util.h)

Round 11. Init sets every field; Draw hands them to the embedded dDlst_effectLine_c (initRnd / update). tww keeps the offset names here; its dDlst_effectLine_c::draw (d_drawlist.cpp) shows what each update argument does (cM_rnd_c::getValue(min, range) = min + rnd * range). dDlst_effectLine_c's own fields (d_drawlist.h) were renamed to the same names in round 12 (other.md, misnamed).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x38 | `m38` | `mCenterPos` | Init 02500880 sets (320, 240, 0); Draw 024FFB94 passes it as the effect line's centre (tww dDlst_effectLine_c::update mPos, the point the lines radiate from) |
| 0x44 | `m44` | `mColor` | Init 02500880 sets white, alpha 0x60; Draw 024FFB94 passes it as the effect line's colour (tww update mColor) |
| 0x48 | `m48` | `mbNoReseed` | Init 02500880 sets 1; Draw 024FFB94 re-seeds the effect line's random generator with mRndSeed0..2 each frame only while it is 0 (tww the same) |
| 0x49 | `m49` | `mbOn` | Draw 024FFB94 draws only while it is set; Init 02500880 / dCamForcusLine_Off 024F8D4C clear it (tww Off the same) |
| 0x4C | `m4C` | `mRndSeed0` | Init 02500880 sets 100; Draw 024FFB94: the first initRnd seed (tww the same) |
| 0x50 | `m50` | `mRndSeed1` | Init 02500880 sets 100; Draw 024FFB94: the second initRnd seed (tww the same) |
| 0x54 | `m54` | `mRndSeed2` | Init 02500880 sets 100; Draw 024FFB94: the third initRnd seed (tww the same) |
| 0x58 | `m58` | `mLineNumMin` | Draw 024FFB94 -> update: the line count is cM_rnd_c::getValue(it, mLineNumRange) (tww dDlst_effectLine_c::draw field_0x20) |
| 0x5A | `m5A` | `mLineNumRange` | Draw 024FFB94 -> update: the random range added to mLineNumMin (tww draw field_0x22) |
| 0x5C | `m5C` | `mLineWidthMin` | Draw 024FFB94 -> update: GXSetLineWidth's width is getValue(it, mLineWidthRange) (tww draw field_0x24) |
| 0x5E | `m5E` | `mLineWidthRange` | Draw 024FFB94 -> update: the random range added to mLineWidthMin (tww draw field_0x26) |
| 0x60 | `m60` | `mStartRadiusMin` | Draw 024FFB94 -> update: each line starts getValue(it, mStartRadiusRange) from the centre (tww draw field_0x28) |
| 0x64 | `m64` | `mStartRadiusRange` | Draw 024FFB94 -> update: the random range added to mStartRadiusMin (tww draw field_0x2C) |
| 0x68 | `m68` | `mLineLengthMin` | Draw 024FFB94 -> update: each line is getValue(it, mLineLengthRange) long (tww draw field_0x30) |
| 0x6C | `m6C` | `mLineLengthRange` | Draw 024FFB94 -> update: the random range added to mLineLengthMin (tww draw field_0x34) |

## MedliShieldViewHD (d_camera_engine2.h)

d_camera_engine2.cpp's view of daNpc_Md_c for the shield direction (shieldAngles); the names follow daNpc_Md_c
(d_a_npc_md.h).

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x4230 | `m3114` | `mWaistRotX` | daNpc_Md_c::mWaistRotX at the same offset (lookBackWaist 0228DFF8 eases it; waistNodeCallBack 022837D0 rotates the waist by it); TWW getWaistRotX() |
| 0x4232 | `m3116` | `mWaistRotY` | daNpc_Md_c::mWaistRotY at the same offset; TWW getWaistRotY() |

## dCamSetup_c (camera setup parameters; d_cam_param.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`, unnamed there; zeldaret/tp names two of them). dCamSetup_c__ct 024F75E4 sets every field. Most reads are in dCamera_c code through private views of mCamSetup (LockonSetupHD, ManualSetupViewHD, SetupView, CameraSetupView, CameraBackgroundHD::Setup in the d_camera_*.h headers); the views keep their own member names until their owners rename them (two of them already used the names taken here: mFanFovySwing / mLeafFovySwing, mMovingShipScale, mBushHigh / mBushLow).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x02C | `m028` | `mLockonCushion` | lockonCamera 025052E8 (through LockonSetupHD::m028): the ease ratio of the view longitude without a lock-on target, of the view latitude, and of the fovy toward LockonFovy; dCamSetup_c__ct 024F75E4 sets it |
| 0x060 | `m05C` | `mLatitudeMin` | dCamSetup_c::CheckLatitudeRange 024F7878: the lower latitude limit in degrees (clamps the angle); dCamSetup_c__ct 024F75E4 sets it (tww -60) |
| 0x064 | `m060` | `mLatitudeMax` | dCamSetup_c::CheckLatitudeRange 024F7878: the upper latitude limit in degrees (clamps the angle); dCamSetup_c__ct 024F75E4 sets it (tww 80) |
| 0x080 | `m078` | `mFanFovySwing` | the fovy wobble in strong wind: manualCamera 0250FDC8 (ManualSetupViewHD::mFanFovySwing, cM_rndFX(it * wind)) and followCamera 025028B8 (SetupView::m078, cM_rndFX(it)); dCamSetup_c__ct 024F75E4 sets it |
| 0x084 | `m07C` | `mLeafFovySwing` | the fovy sine sway in wind: manualCamera 0250FDC8 (ManualSetupViewHD::mLeafFovySwing * sin) and followCamera 025028B8 (SetupView::m07C * sin + mFovy); dCamSetup_c__ct 024F75E4 sets it |
| 0x088 | `m080` | `mFanBankBase` | dCamSetup_c::FanBank 024F78F0: the base of the camera bank in wind (base + cM_rndFX(mFanBankWidth) * wind power); dCamSetup_c__ct 024F75E4 sets it |
| 0x08C | `m084` | `mFanBankWidth` | dCamSetup_c::FanBank 024F78F0: the random range of the camera bank, scaled by the wind power; dCamSetup_c__ct 024F75E4 sets it |
| 0x0B8 | `m0B0` | `mLockonChangeTimer` | lockonCamera 025052E8 (LockonSetupHD::m0B0): for the first it frames of the lock-on style the lock-on longitude is scaled by mCurModeTimer / it and the cushion eases from mLockonChangeCushion; TP's dCamSetup_c mLockonChangeTimer at the same place (after mChargeBRatio); dCamSetup_c__ct 024F75E4 sets 60 |
| 0x0BC | `m0B4` | `mLockonChangeCushion` | lockonCamera 025052E8 (LockonSetupHD::m0B4): the starting cushion that eases over mLockonChangeTimer frames; TP's dCamSetup_c mLockonChangeCushion at the same place; dCamSetup_c__ct 024F75E4 sets it |
| 0x0C0 | `m0B8` | `mMovingShipScale` | checkGroundInfo 024F9044 (CameraBackgroundHD::Setup::mMovingShipScale) scales the moving background (ship) position delta by it; Run 024FE3E8 (CameraSetupView::m0B8) reads it too; dCamSetup_c__ct 024F75E4 sets it |
| 0x0C8 | `m0C0` | `mBushHigh` | checkGroundInfo 024F9044 (CameraBackgroundHD::Setup::mBushHigh): dCamera_c::mBushHeight for the high bush hit bit; dCamSetup_c__ct 024F75E4 sets it |
| 0x0CC | `m0C4` | `mBushLow` | checkGroundInfo 024F9044 (CameraBackgroundHD::Setup::mBushLow): dCamera_c::mBushHeight for the low bush hit bit; dCamSetup_c__ct 024F75E4 sets it |

Open: `m00C`, `m024`, `m02C`, `m034`, `m044`, `m050`, `m054`, `m058`, `m06C`, `m090`, `m094`, `m0BC`, `m0C8` (only the constructor sets them; no read found in the WWHD camera code or its views), `m030` (CalcSubjectAngle reads it through SubjectSetupHD::m030 as a slow-down factor of the subject angle step outside the subject (first-person) status; one use, no clear role), `m098` (nextMode compares the camera radius with it while the C stick is idle; one use, no clear role).

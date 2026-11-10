# Field names: daShip_c (King of Red Lions)

Field naming pass (2026-11-10). Names come from how the original code uses a field (address of the use),
from names already in this decomp, or from the CC0 zeldaret/tww and zeldaret/tp decomps ("TWW getX()": the TWW
accessor that returns the field; "after TWW/TP X": the same use under that name in another class). Renames only:
offsets, types and code are unchanged. The header keeps the old name (`was mXXXX`) with the evidence.

## daShip_c (d_a_ship.h; views in d_a_obj_auzu.h AuzuShip, d_a_salvage.h, d_a_player_main_07.h Ship)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x03BC | `m02A0` | `mpEffectTexMtx` | create 0247C358 stores texGen->getTexMtx(1) of material "m_fn_main_hashi"; draw 0247255C writes the effect matrix (setEffectMtx, HD inline) into it; TWW type J3DTexMtx* (same uses at TWW d_a_ship.cpp draw/create) |
| 0x03C0 | `m02A4` | `mpScrollTexMtx` | create 0247C358 stores the same getTexMtx(1); draw 0247255C only sets its mSRT.mTranslationX = m03D4 (texture scroll); TWW type J3DTexMtx* |
| 0x03C4 | `m02A8` | `mOrthoMtx` | create 0247C358 fills it with an orthographic projection (C_MTXOrtho 50,-50,-50,50,0.1,100000 in TWW); draw 0247255C revConcats it into the effect matrix |
| 0x0633 | `m034A` | `mReverb` | setRoomInfo 024752F4 stores dComIfGp_getReverb(roomNo); seStart 02473080 passes it as the reverb argument of mDoAud_seStart; after TWW daPy_lk_c mReverb (src/d/actor/d_a_player_main.cpp, same getReverb store) |
| 0x0634 | `m034B` | `mPathId` | create 0247C358 stores the low parameter byte; checkOutRange 02474A98 tests 0xFF and passes it to dPath_GetRoomPath; after TWW daOship_c mPathId (include/d/actor/d_a_oship.h) |
| 0x0638 | `m034F` | `mCraneLiftUpTimer` | setRopePos 024763B0 sets 20 when the hook lifts out of the water (JA_SE_LK_SHIP_CRANE_LIFTUP), counts it down, and skips the drop splash/JA_SE_LK_SHIP_CRANE_DROP while non-zero |
| 0x0639 | `m0350` | `mDemoPart` | procZevDemo_init 02474FEC sets 4 (none); procZevDemo 024800A8 stores the demo's part substance and copies it to mPart / setPartOnAnime(m0350) |
| 0x063A | `m0351` | `mDemoMode` | execute 02477A24 stores the event cut number (or 0xFF); compared with the DemoMode_e values throughout procZevDemo 024800A8; TWW checkSalvageDemo() returns m0351 == DEMO_SALVAGE_e (enum DemoMode_e); player_main_07 reads it as checkSalvageDemo |
| 0x063B | `m0352` | `mWhirlCircleFlg` | TWW onWhirlFlg() sets 1, onWhirlFlgDirect() sets 0 (WWHD obj_auzu: 1 inside the inner radius); execute 02477A24: when set the ship circles the whirl at m0400/m040C, else it is pulled directly |
| 0x063C | `m0353` | `mCraneShockFlg` | procCrane_init 0247F57C / procCrane 0247F7CC set it to cM_rnd()<0.5; procCrane starts a controller shock (ship_StartShock) when set and mRopeCnt changed |
| 0x064E | `m0366` | `mTillerAngle` | execute 02477A24: mTillerAngleRate = m0366 / 0x2000; setSelfMove 0247E2B4 / procSteerMove 0247E854 ease it toward the stick-driven tiller target and pass it to setMoveAngle; bodyJointCallBack 02471298 rotates the tiller joint by it |
| 0x0654 | `m036C` | `mControllAngle` | setControllAngle 024751E0 springs it toward the given angle (m036C += m036E); execute 02477A24 shape_angle.z = m036C + m0372 |
| 0x0656 | `m036E` | `mControllAngleSpeed` | setControllAngle 024751E0 velocity of the spring (m036E += step; m036C += m036E; eased to 0); cleared in procSteerMove 0247E854 / procZevDemo 024800A8 |
| 0x0658 | `m0370` | `mWaveAngleX` | setWaveAngle 024759CC springs it toward the wave-slope pitch (m0370 += m0374); execute 02477A24 shape_angle.x = m0370 + m0384 and pitches speed by it; setYPos 024754D8 / setCrashData 0247D880 add sway/crash |
| 0x065A | `m0372` | `mWaveAngleZ` | setWaveAngle 024759CC springs it toward the wave-slope roll (m0372 += m0376); execute 02477A24 shape_angle.z = m036C + m0372 |
| 0x065C | `m0374` | `mWaveAngleSpeedX` | setWaveAngle 024759CC velocity of m0370 (fmadds diff*0.045, eased to 0); setCrashData 0247D880 kicks it |
| 0x065E | `m0376` | `mWaveAngleSpeedZ` | setWaveAngle 024759CC velocity of m0372 (fmadds diff*0.045, eased to 0); setCrashData 0247D880 kicks it |
| 0x0662 | `m037A` | `mProcVar0` | per-proc scratch: countdown in procTurn 02481874 / procCannon 0247F0D0, joint index in procTornadoUp 02481C94, phase in procTactWarp 02482438 / procStartModeWarp 02481FAC / procStartModeThrow 02482970; after TP daAlink_c mProcVar0 |
| 0x0664 | `m037C` | `mOldShapeAngleX` | execute 02477A24 copies shape_angle.x after setWaveAngle and reuses it as the wave pitch while flying; draw 0247255C uses it for the effect look-at |
| 0x0666 | `m037E` | `mOldShapeAngleZ` | execute 02477A24 copies shape_angle.z after setWaveAngle and reuses it as the wave roll while flying; draw 0247255C passes it as the mDoMtx_lookAt roll |
| 0x0668 | `m0380` | `mTargetSailAngle` | setSailAngle 0247D398 picks +-0x1555 from the wind side and eases mSailAngle toward it (cLib_addCalcAngleS(&mSailAngle, m0380,...)) |
| 0x0670 | `m0388` | `mLandTimer` | setYPos 024754D8 sets 30 when the ship lands (offStateFlg(daSFLG_FLY_e)); execute 02477A24 counts down; setMoveAngle 0247D4B0 limits the turn rate while >= 15 |
| 0x0672 | `m038A` | `mAnmTransformFileIdx` | procTalk_init 024749DC / procTalk 02481630: mAnmTransform = ship_getRes(m038A) (BCK_FN_TALK_A/B); TWW comment "file idx" |
| 0x0674 | `m038C` | `mTurnTargetAngle` | checkOutRange 02474A98 stores the path direction +0x4000; procTurn 02481874 / execute 02477A24 ease shape_angle.y toward it (cLib_addCalcAngleS2) |
| 0x0676 | `m038E` | `mTurnAngleSpeed` | procTurn 02481874 / execute 02477A24: max step of cLib_addCalcAngleS2(&shape_angle.y, m038C, 8, m038E), itself eased up to 0x800/0x1800; reset in procTurn_init 024814F8 |
| 0x067A | `m0392` | `mBodyAnmFileIdx` | createHeap 0247B508 loads mpBodyAnm's anim from ship_getResIdx("Ship", m0392) (BCK_FN_MAST_ON2/OFF2); procZevDemo 024800A8 / setPartOnAnime 0247BB24 / setPartOffAnime 0247BA38 update it with each mpBodyAnm->setAnm; TWW comment "file idx" |
| 0x067C | `m0394` | `mCannonAngleY` | TWW getCannonAngleY() returns shape_angle.y + m0394; procCannon 0247F0D0 aims it; cannonJointCallBack 024718C8 rotates by it |
| 0x067E | `m0396` | `mCannonAngleX` | TWW getCannonAngleX() returns shape_angle.x + m0396 - 0x4000; procCannon 0247F0D0 clamps 0x1556..0x4000; cannonJointCallBack 024718C8 |
| 0x0680 | `m0398` | `mCraneAngle` | TWW getCraneAngle() returns m0398; procCrane 0247F7CC eases it to mCraneBaseAngle; craneJointCallBack 02471AEC; d_a_salvage reads its sign |
| 0x0684 | `m039C` | `mCraneTurnAngle` | procCrane 0247F7CC / procCraneUp 0247FBC4 chase it toward the local craneTurn; craneJointCallBack 02471AEC rotates by m0398 + m039C |
| 0x0688 | `m03A0` | `mHeadAngleX` | headJointCallBack1 02471C88 mDoMtx_ZXYrotM(m03A0, m03A2, 0); execute 02477A24 eases it toward the look target X angle |
| 0x068A | `m03A2` | `mHeadAngleY` | headJointCallBack1 02471C88 Y argument of ZXYrotM and per-joint shape_angle.y offset; execute 02477A24 eases it toward the look target Y angle |
| 0x068E | `m03A6` | `mProcVar1` | per-proc scratch: swing target in procCraneUp 0247FBC4, crane speed in procCrane 0247F7CC, flag in procTornadoUp 02481C94, spin speed in procStartModeWarp 02481FAC / procTactWarp 02482438; after TP daAlink_c mProcVar1 |
| 0x0694 | `m03AC` | `mSwayPhaseZ` | setYPos 024754D8 advances it by random 600..1200 * k and adds sin(m03AC)*rate*115 to m0372 (roll) |
| 0x0696 | `m03AE` | `mSwayPhaseX` | setYPos 024754D8 advances it by random 800..1600 * k and adds sin(m03AE)*rate*100 to m0370 (pitch) |
| 0x0698 | `m03B0` | `mSwayPhaseY` | setYPos 024754D8 advances it by random 1000..2000 + rate*500 and adds sin(m03B0)*rate*0.25 to current.pos.y |
| 0x069A | `m03B2` | `mWhirlLinkID` | TWW onWhirlFlg(u32 whirlID, s16 linkID)/onWhirlFlgDirect store linkID here; WWHD obj_auzu stores PRM_LINK_ID; procWhirlDown 024827D4 passes it as the next-stage point |
| 0x069C | `m03B4` | `mHeadAnmFileIdx` | setHeadAnm 02475DA8 compares/stores the head BCK index and loads ship_getRes(m03B4); createHeap 0247B508 / procTalkReady_init 0247309C / procZevDemo 024800A8 set FN_LOOK_L/R/LOSE1; TWW comment "file idx" |
| 0x069E | `m03B6` | `mCrashTimer` | setCrashData 0247D880 returns early while non-zero and sets 30 after a crash; execute 02477A24 counts it down |
| 0x06A2 | `m03BC` | `mWarpSplashBackAngle` | procTactWarp_init 0247DBF8 / procStartModeWarp 02481FAC / procTactWarp 02482438 set it to (0, shape_angle.y+0x8000, 0); only used as the angle of the second ID_AK_JN_SHIPWARPSPLASH00 emitter (m1998) |
| 0x06A8 | `m03C4` | `mGroundCode` | setRoomInfo 024752F4 stores dBgS GetGroundCode of the ground poly (0 when no ground); no reader in the ship files |
| 0x06B0 | `m03CC` | `mAcchCirNum` | create 0247C358 sets 3 or 4 and passes it as the circle count to mAcch.Set(..., m03CC, mAcchCir, ...); setSelfMove 0247E2B4 loops i < m03CC |
| 0x06B4 | `m03D0` | `mFloatSpeedY` | setYPos 024754D8: vertical spring speed toward the water (d = (m03F4-y)*0.05 + m03D0, clamped +-20, added to pos.y); set to speed.y*0.5 on landing |
| 0x06B8 | `m03D4` | `mTexScrollX` | execute 02477A24 advances it by speedF (wrap 0..1); draw 0247255C writes it to the texture SRT mTranslationX of m02A4 and material m_fn_main_hashi |
| 0x06C4 | `m03E0` | `mTargetSpeed` | TWW name (include/d/actor/d_a_ship.h 0x03E0 mTargetSpeed); procSteerMove 0247E854 sets it from the wind power; setSelfMove 0247E2B4 decelerates toward it |
| 0x06CC | `m03E8` | `mSailScale` | bodyJointCallBack 02471298 PSMTXScale(m03E8); setPartOnAnime 0247BB24 sets 1.0 for PART_STEER (sail out) and 0.001 for cannon/crane; procWait/procGetOff/procPaddleMove reset 1.0 |
| 0x06D8 | `m03F4` | `mWaterY` | setYPos 024754D8 / create 0247C358 store getWaterY(); current.pos.y is set to/clamped around it; execute 02477A24 passes it to mTrack.setWaterY |
| 0x06DC | `m03F8` | `mWaterFlatY` | getWaterY 024753BC stores mAcch.m_wtr height (or -1e9); getMaxWaterY 0247204C; execute 02477A24 passes it to mTrack.setWaterFlatY (dPa_trackEcallBack setter name) |
| 0x06E4 | `m0400` | `mForceMoveDist` | setTornadoActor 02472C04 / setWhirlActor 02472F14 store the XZ distance to the tornado/whirl; execute 02477A24 shrinks it and places the ship at center + m0400*sin/cos(m040C); TWW checkForceMove() = tornado or whirl |
| 0x06E8 | `m0404` | `mForceMoveRate` | setTornadoActor 02472C04 / setWhirlActor 02472F14 store (range - m0400)*k clamped >= 0; execute 02477A24 scales the speed cap by it; procCannon 0247F0D0 scales aim speed |
| 0x06EC | `m0408` | `mForceMoveAngleSpeed` | execute 02477A24 stores the per-frame angle step speedF/m0400 added to m040C; procWhirlDown 024827D4 adds it (rad->s16) to shape_angle.y |
| 0x06F0 | `m040C` | `mForceMoveAngle` | setTornadoActor 02472C04 / setWhirlActor 02472F14 store cM_atan2f of the offset from the center; execute 02477A24 advances it and positions the ship on the circle |
| 0x0710 | `m0428` | `mpEvtAtnPos` | procZevDemo 024800A8 points it at the event pt1 eyePos or the "atn_pos" substance; execute 02477A24 aims eyePos/attention at it (after mAtnPos) |
| 0x0718 | `m0430` | `mCurrMsgBsPcId` | procTalk 02481630 stores fopMsgM_messageSet(...) and tests fpcM_ERROR_PROCESS_ID_e; procTalk_init 024749DC / procTurn_init 024814F8 reset it; after TWW dNpc mCurrMsgBsPcId (include/d/d_npc.h, same use) |
| 0x072C | `m0444` | `mSailPos` | execute 02477A24 copies the J_FN_SAIL1 joint translation and puts the grid actor there; SE origin for sail/part sounds in setSailAngle 0247D398 / setPartOn/OffAnime / procSteerMove; shipDelete 0247AE84 deletes its sound object |
| 0x0738 | `m0450` | `mTurnEyePos` | procTurn_init 024814F8 stores camera Center + dir*1600; procTurn 02481874 moves the camera eye toward it (camera_Set) |
| 0x0744 | `m045C` | `mPrevPos` | execute 02477A24 copies current.pos each frame before moving; procTurn 02481874 shifts the camera center by current.pos - m045C |
| 0x1314 | `m102C` | `mCranePos` | execute 02477A24 copies the VFNCR_JNT_V_CRANE_ROTATION joint translation; SE origin for crane sounds in incRopeCnt 02476204 / procCrane 0247F7CC; shipDelete 0247AE84 |
| 0x1320 | `m1038` | `mCannonPos` | execute 02477A24 copies the VFNCN_JNT_CANON2 joint translation; SE origin for JA_SE_LK_SHIP_CANNON_MOVE/FIRE (procCannon 0247F0D0, execute); shipDelete 0247AE84 |
| 0x132C | `m1044` | `mBeltSpeed` | TWW getBeltSpeed() returns m1044.absXZ(); execute 02477A24 eases it and adds it to current.pos; player_main_07 reads it as the belt speed |
| 0x1350 | `m1068` | `mAtnPos` | TWW setAtnPos() sets daSFLG_UNK1000000_e and m1068 = *pPos; execute 02477A24 aims eyePos at it while that flag is set |
| 0x135C | `m1074` | `mRopeRipplePos` | setRopePos 024763B0 sets it to rope pos 0 XZ +-20 at water height m03F4 and uses it as the ripple emitter (m19C0) position; same role as mCraneRipplePos for mRipple |
| 0xD140 | `m1984` | `mWarpSplash` | procStartModeWarp 02481FAC / procTactWarp_init 0247DBF8 attach it to ID_AK_JN_SHIPWARPSPLASH00 at shape_angle; ended in procTactWarp 02482438 / shipDelete 0247AE84 / daShip_initStartPos 024832E0 |
| 0xD154 | `m1998` | `mWarpSplashBack` | same procs attach it to the second ID_AK_JN_SHIPWARPSPLASH00 emitter at m03BC (shape_angle.y + 0x8000); ended alongside m1984 |
| 0xD168 | `m19AC` | `mRopeDrip` | setRopePos 024763B0 attaches it to ID_IT_JN_LK_NURE_POTA00 (wet drip) at rope pos 0 when the hook lifts out; execute 02477A24 / shipDelete 0247AE84 / daShip_initStartPos end it |
| 0xD17C | `m19C0` | `mRopeRipple` | setRopePos 024763B0 ripple emitter (5,0x33) at m1074, ended with m19AC; create 0247C358 constructs it as a ripple callback (025A9084); TWW type dPa_rippleEcallBack; neighbour mRipple is the crane ripple |

Open (no name yet):

- `m02D8`: u8[0x10] gap, no use in d_a_ship*.cpp or TWW (padding up to mAnmTransform)
- `m0368`: u8[4] gap, no use in WWHD or TWW
- `m0378`: only written: setSailAngle 0247D398 stores mSailAngle >> 1; never read in WWHD or TWW
- `m0382`: no use in WWHD or TWW
- `m0384`: evidence too weak: execute 02477A24: shape_angle.x = m0370 + m0384; eased to -0x800 while daSFLG_FLY_e is set, sprung back to 0 (via m0386) on the water; zeroed in procZevDemo 024800A8
- `m0386`: evidence too weak: execute 02477A24 velocity of m0384's spring (m0386 += step; m0384 += m0386; eased to 0); cleared in procSteerMove 0247E854 / procZevDemo 024800A8
- `m0390`: no use in WWHD or TWW
- `m03A8`: only cleared: procCannon_init 0247EFE0 (TWW too)
- `m03AA`: only read: setSailAngle 0247D398 uses m03AA+0x4000 as the wind direction while a tornado is active; never written in WWHD or TWW
- `m03B8`: evidence too weak: execute 02477A24 chases it to 0x4E8 while daSFLG_UNK2000000_e is set (else 0) and adds it to current.angle.y and shape_angle.y every frame
- `m03D8`: evidence too weak: execute 02477A24: peak-held m03F4 - pos.y + 5 (chaseF down 3/frame); draw 0247255C adds it to the effect projection origin height
- `m03F0`: u8[4] gap, no use in WWHD or TWW
- `m1050`: no use in WWHD or TWW

Type fixes (objcmp identical): `mpEffectTexMtx` / `mpScrollTexMtx` were `be<u32>` and are now
`gptr<ShipTexMtx>` (J3DTexMtx pointers, TWW type; ShipTexMtx is the header's 0x94 J3DTexMtx layout view, the decomp
has no J3DTexMtx declaration). `mRipple` and `mRopeRipple` were raw `u8[0x14]` and are now `ShipRipple`, the header's
dPa_rippleEcallBack layout view (TWW type dPa_rippleEcallBack; the global dPa_rippleEcallBack in d_particle.h is a
1-byte opaque stand-in). Corrected pin: the ripple constructor 025A9084 runs in create 0247C358, not createHeap.

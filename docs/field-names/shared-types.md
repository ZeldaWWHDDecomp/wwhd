# Field names: shared types

Fields the decomp still called by offset (`mXXXX`, `field_0x…`, `unk…`; zeldaret/tww keeps many of these GameCube offset
names) get names unit by unit (field naming pass, 2026-11-10). Rules: a name comes from how the original
code uses the field (address of the use and what it shows), from names already in this decomp, or from the CC0
zeldaret/tww and zeldaret/tp decomps ("TP name": the same field in TP's `dCamera_c`, matched by position and use).
Renames only: offsets, types and code are unchanged (objcmp: object-identical). The header line keeps the old name
(`was mXXX`) with the evidence. Fields without enough evidence keep their offset name and are listed as open.

## dNpc_HeadAnm_c, dNpc_EventCut_c (d_npc.h), dSv_player_info_c (d_save.h)

| Class | WWHD offset | Old | New | Evidence |
|---|---|---|---|---|
| dNpc_HeadAnm_c | 0x00 | `field_0x00` | `mSwingAngle` | swing_vertical 0259F448 / swing_horizone 0259F5A4 ease .x/.y toward the swing target, move 0259F67C eases both to 0 without a procedure |
| dNpc_HeadAnm_c | 0x10 | `field_0x14` | `mVerticalAmplitude` | swing_vertical_init 0259F36C stores the amplitude argument; swing_vertical 0259F448 multiplies cM_ssin(phase) by it for angle.x |
| dNpc_HeadAnm_c | 0x14 | `field_0x18` | `mHorizontalAmplitude` | swing_horizone_init 0259F514 stores the amplitude argument; swing_horizone 0259F5A4 multiplies cM_ssin(phase) by it for angle.y |
| dNpc_HeadAnm_c | 0x18 | `field_0x1C` | `mSwingPhase` | both inits clear it; swing_vertical 0259F448 / swing_horizone 0259F5A4 add mSwingPhaseStep and take cM_ssin of it |
| dNpc_HeadAnm_c | 0x1A | `field_0x1E` | `mSwingPhaseStep` | swing_vertical_init 0259F36C / swing_horizone_init 0259F514 store the step argument; added to mSwingPhase each frame |
| dNpc_HeadAnm_c | 0x1C | `field_0x20` | `mSwingCount` | both inits store the count argument; the swing procedures subtract 1 per completed cycle (phase wraps to >= 0) and stop at 0 |
| dNpc_EventCut_c | 0x44 | `field_0x44` | `mLastTurnDiff` | cutTurnToPosProc 025A0B3C / cutTurnToActorProc 0259FE88 store |target yaw - actor yaw| each frame and count the timer down while it is unchanged; the Start procs (025A0984, 0259FAB0) clear it |
| dNpc_EventCut_c | 0x52 | `field_0x52` | `mTalkActive` | cutTalkMsgStart 025A1090 sets 1 after setting the message numbers; cutTalkMsgProc 025A1388 ends the cut when it is 0 and clears it when the talk returns 18 |
| dSv_player_info_c | 0x10 | `field_0x10` | `mSaveCount` | saveMgrSessionCounter 02721A68 adds 1 while it is below MaxSaveCount; called by SaveMgr::saveCard 02721C44 on each save; init 025B8630 clears it |

Open: dNpc_EventCut_c `field_0x14`, `field_0x20` (only cleared by the constructor 0259F740); dSv_player_info_c `field_0x0`,
`field_0x25`, `field_0x36`, `field_0x5a` (no use in the decomp); the other d_save.h types are not done yet.

## stage_map_info_class and roomRead_data_class (d_stage.h)

Stage data read from the stage file; old names are tww's d_stage.h offsets. roomRead_data_class's list was split one
per line first (layout-neutral). The map rectangles have no use in this decomp; their names and evidence come from
zeldaret/tww d_map.cpp (getMapInfo_map1_* accessors, getRoomImage) and d_a_agb.cpp.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `field_0x00` | `mSubMapX0` | stage map data (map info, read from the stage file); tww dMap_RoomInfo_c::getRoomImage uses the floor's s%d.bti image only while mSubMapX1 differs from it and mSubMapZ1 from mSubMapZ0 (the same test it makes for the m%d.bti image on mMap1X0 .. mMap1Z1); no use in WWHD |
| 0x04 | `field_0x04` | `mSubMapZ0` | stage map data; tww getRoomImage compares it with mSubMapZ1 for the s%d.bti image (see mSubMapX0); no use in WWHD |
| 0x08 | `field_0x08` | `mSubMapX1` | stage map data; tww getRoomImage compares it with mSubMapX0 for the s%d.bti image (see mSubMapX0); no use in WWHD |
| 0x0C | `field_0x0c` | `mSubMapZ1` | stage map data; tww getRoomImage compares it with mSubMapZ0 for the s%d.bti image (see mSubMapX0); no use in WWHD |
| 0x18 | `field_0x18` | `mMap1X0` | stage map data; tww d_map.cpp getMapInfo_map1_X0 returns it (getRoomImage tests it against mMap1X1 for the m%d.bti image; d_a_agb clamps home.pos.x to mMap1X0 .. mMap1X1); no use in WWHD |
| 0x1C | `field_0x1c` | `mMap1Z0` | stage map data; tww d_map.cpp getMapInfo_map1_Z0 returns it (d_a_agb clamps home.pos.z to mMap1Z0 .. mMap1Z1); no use in WWHD |
| 0x20 | `field_0x20` | `mMap1X1` | stage map data; tww d_map.cpp getMapInfo_map1_X1 returns it; no use in WWHD |
| 0x24 | `field_0x24` | `mMap1Z1` | stage map data; tww d_map.cpp getMapInfo_map1_Z1 returns it; no use in WWHD |
| 0x28 | `field_0x28` | `mMap1XC` | stage map data; tww d_map.cpp getMapInfo_map1_XC returns it; no use in WWHD |
| 0x2C | `field_0x2c` | `mMap1ZC` | stage map data; tww d_map.cpp getMapInfo_map1_ZC returns it; no use in WWHD |
| 0x35 | `field_0x35` | `mFloorNo` | dStage_roomDt_c::getMapInfo2 025C40E0 returns the map info entry whose it equals the asked number; tww getRoomImage asks with the floor number (s%d / m%d.bti) |
| 0x1 | `field_0x1` | `mReverb` | dStage_roomRead_dt_c_GetReverbStage 025C2150 returns it & 0x7F as the room's reverb (tww inline dStage_roomRead_dt_c_GetReverb) |
| 0x2 | `field_0x2` | `mTimePass` | dStage_RoomCheck 025C35E8 reads it & 3 as the room's time-pass mode (tww inline dStage_roomRead_dt_c_GetTimePass) |

Open: stage_map_info_class `field_0x10` / `0x14` / `0x30` / `0x34` (no use in WWHD or tww), stage_stag_info_class
`field_0x14` / `0x18` / `0x1c` and stage_scls_info_class `field_0xb` (no use).

## material_hd::State, ExtendedState and Resource (d_material.h)

The HD ice-material state words had only offset names. State is constructed by 027BE6B8 (agl::TextureData::create)
and copied into ExtendedState by 027BDEB4 (agl::TextureSampler::copyTexture); ExtendedState is 0x198 bytes and starts
with that TextureData. State is therefore a view of agl::TextureData (agl/TextureData.h: `format`, then the
sead::GraphicsSurface) and ExtendedState of agl::TextureSampler (agl/TextureSampler.h; nativeTexture at 0xB0 is a
GX2Texture), and the words take those names (views follow their class). Offsets within each struct.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0 | `field_0x0` | `format` | view of agl::TextureData (agl/TextureData.h) 0x0 format: State is the TextureData that initState 027BE6B8 (TextureData::create) constructs and copyState 027BDEB4 (TextureSampler::copyTexture) copies |
| 0x4 | `field_0x4` | `mDimension` | view of agl::TextureData 0x4 = surface (sead::GraphicsSurface) 0x0 mDimension; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update |
| 0x8 | `field_0x8` | `mWidth` | view of agl::TextureData 0x8 = surface (sead::GraphicsSurface) 0x4 mWidth; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update |
| 0xC | `field_0xc` | `mHeight` | view of agl::TextureData 0xC = surface (sead::GraphicsSurface) 0x8 mHeight; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update |
| 0x10 | `field_0x10` | `mDepth` | view of agl::TextureData 0x10 = surface (sead::GraphicsSurface) 0xC mDepth; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update |
| 0x14 | `field_0x14` | `mMipCount` | view of agl::TextureData 0x14 = surface (sead::GraphicsSurface) 0x10 mMipCount; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update |
| 0x18 | `field_0x18` | `mFormat` | view of agl::TextureData 0x18 = surface (sead::GraphicsSurface) 0x14 mFormat; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update |
| 0x1C | `field_0x1c` | `mSamples` | view of agl::TextureData 0x1C = surface (sead::GraphicsSurface) 0x18 mSamples; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update |
| 0x20 | `field_0x20` | `mUsage` | view of agl::TextureData 0x20 = surface (sead::GraphicsSurface) 0x1C mUsage |
| 0x24 | `field_0x24` | `mImageBytes` | view of agl::TextureData 0x24 = surface (sead::GraphicsSurface) 0x20 mImageBytes |
| 0x28 | `field_0x28` | `mImage` | view of agl::TextureData 0x28 = surface (sead::GraphicsSurface) 0x24 mImage; transfer (in dMat_setup 02590FB8) copies it into the sampler's native texture (ExtendedState mNativeImage) when nothing else changed |
| 0x2C | `field_0x2c` | `mMipBytes` | view of agl::TextureData 0x2C = surface (sead::GraphicsSurface) 0x28 mMipBytes |
| 0x30 | `field_0x30` | `mMipImage` | view of agl::TextureData 0x30 = surface (sead::GraphicsSurface) 0x2C mMipImage; transfer (in dMat_setup 02590FB8) copies it into the sampler's native texture (ExtendedState mNativeMipImage) when nothing else changed |
| 0x34 | `field_0x34` | `mTileMode` | view of agl::TextureData 0x34 = surface (sead::GraphicsSurface) 0x30 mTileMode; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update |
| 0x38 | `field_0x38` | `mSwizzle` | view of agl::TextureData 0x38 = surface (sead::GraphicsSurface) 0x34 mSwizzle; transfer (in dMat_setup 02590FB8) compares it with the sampler's copy to decide between copyTexture and the image-pointer update |
| 0xD4 | `field_0xd4` | `mNativeImage` | view of agl::TextureSampler (agl/TextureSampler.h): nativeTexture (0xB0, a GX2Texture) + 0x24, the surface image pointer; transfer (in dMat_setup 02590FB8) sets it to the source TextureData mImage when only the image pointers changed |
| 0xDC | `field_0xdc` | `mNativeMipImage` | view of agl::TextureSampler: nativeTexture (0xB0) + 0x2C, the surface mip image pointer; transfer (in dMat_setup 02590FB8) sets it to the source TextureData mMipImage when only the image pointers changed |
| 0x20 | `field_0x20` | `mpTextureRes` | dMat_setup 02590FB8 passes it (from dRes_control_c::getRes 026066C4, resources 0x92 / 0x93) to initTexture 02773798, which loads the State texture from it |

Open: ExtendedState `field_0xd8` (the native surface's mip size word, no use).

## dWaterMark_c (d_water_mark.h)

Old names are the GameCube offsets (zeldaret/tww d_water_mark.cpp, same code).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x1F0 | `field_0x12c` | `mOnMoveBg` | setMatrix 025CC2B4 stores whether the ground polygon belongs to a moving BG (Water_ChkMoveBG); execute 025CC4A4 recomputes the matrix every frame while it is 1 (tww the same) |
| 0x1F2 | `field_0x12e` | `mAngleY` | create 025CC5D0 sets the upper parameter half; setMatrix 025CC2B4 turns the mark by it and tilts it by the ground normal relative to it (tww the same) |
| 0x1F4 | `field_0x130` | `mFadeStartId` | create 025CC5D0 (footprint, param 2): this footprint's id + 20 (mod 40); execute 025CC4A4 starts the brk fade once m_player_foot_now_id is in [it, mFadeEndId) (tww the same) |
| 0x1F6 | `field_0x132` | `mFadeEndId` | create 025CC5D0 (footprint, param 2): mFadeStartId + 20 (mod 40), the end of the id window that starts the fade (tww the same) |
| 0x1F8 | `field_0x134` | `mFootId` | create 025CC5D0 (footprint, param 2) stores m_player_foot_now_id (then advances the ring); -1 for other marks and once execute 025CC4A4 has started the fade (tww the same) |


## dOvlpFd3_c (snapshot fade; d_ovlp_fade3.h)

Old names are the GameCube offsets (zeldaret/tww d_ovlp_fade3.cpp). The two packets are only constructed and destroyed in HD (the native fade renderer draws); their names come from tww's constructor.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0xDC | `field_0xdc` | `mWipePacket` | tww dOvlpFd3_c: the 2D packet of the wipe_00.bti texture (init in the constructor), shrunk to 0 by execFadeOut and drawn last; HD dOvlpFd3_construct 025A25F0 constructs it and dOvlpFd3_Delete 025A2558 destroys it only, the native fade renderer replaces the draw |
| 0x14C | `field_0x120` | `mFrameBufferPacket` | tww dOvlpFd3_c: the 2D packet of the frame-buffer image (mDoGph_gInf_c::getFrameBufferTimg) drawn under mWipePacket; HD dOvlpFd3_construct 025A25F0 / dOvlpFd3_Delete 025A2558 construct and destroy it only |
| 0x1BC | `field_0x164` | `mSnapTaken` | dOvlpFd3_nativeDraw 025A28B0 captures the snapshot while it is 0 and sets it; dOvlpFd3_execFirstSnap 025A26D8 starts the fade-out only once it is set; dOvlpFd3_execNextSnap 025A27E4 clears it for the next snapshot (tww draw / execFirstSnap the same) |


## dStage_Ship_dt_c (d_stage.h), its view GameInfoHD::Ship (d_com_inf_game_unit.h), and cc_mtView (d_cc_uty.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0xE | `field_0xe` | `mShipId` | hdGameInfo_dComIfGp_getShip 02520584 returns the room's ship entry whose it equals the asked id (tww dComIfGp_getShip the same) |
| 0xE | `field_0xe` | `mShipId` | view of dStage_Ship_dt_c (d_stage.h) 0xE mShipId: hdGameInfo_dComIfGp_getShip 02520584 returns the entry whose it equals the asked id (tww dComIfGp_getShip the same) |
| 0x570 | `m454` | `mAction` | view of mt_class (d_a_mt.h) 0x570 mAction (GameCube m454 + 0x11C; tww cc_uty reads mt_class::m454): cc_at_check 025192A8 plays JA_SE_OBJ_MG_BALL_DMG when a Magtail target's action is 2 |

Open: dStage_Ship_dt_c `field_0xf` (no use); GameInfoHD::PlaySupplement `field_0x4965` / `field_0x497a` (the play constructor clears them only; tww the same).

## daNh_HIO_c::hio_prm_c (firefly; d_a_nh.h)

Offsets within hio_prm_c. checkBinCatch passes these four in the order of tww's dAttention_c::CatchRequest(actor, itemNo, horizontalDist, upDist, downDist, angle, ...).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x8 | `field_0x08` | `mCatchDist` | checkBinCatch 021E0BC0 passes it to the attention catch request as the horizontal catch distance (tww dAttention_c::CatchRequest horizontalDist; l_HIO default 200) |
| 0xC | `field_0x0c` | `mCatchUpDist` | checkBinCatch 021E0BC0 passes it to the attention catch request as the upward catch distance (tww CatchRequest upDist; default 100) |
| 0x10 | `field_0x10` | `mCatchDownDist` | checkBinCatch 021E0BC0 passes it to the attention catch request as the downward catch distance (tww CatchRequest downDist; default -50) |
| 0x3C | `field_0x3c` | `mCatchAngle` | checkBinCatch 021E0BC0 passes it to the attention catch request as the catch angle (tww CatchRequest angle; default 0x2000) |

Open: hio_prm_c `field_0x34` (default 600, no read in WWHD or tww); daNh_c `unk68C` (no use), `unk696` / `unk698` / `unk69A` / `unk6A0` (setAction clears them only; tww the same).

## Left open in round 2 batch 1 (no new evidence or shared with another batch)

- dMesg_core_c (d_mesg_core.h) `field_0x968` .. `field_0x9a1`: only the constructor 025F7500 sets them; the users are untranslated.
- daNpc_Tc_c / daNpc_Tc_HIO_c / TcAnmPrm: the open fields listed in npcs.md stay open (no new use); TcAnmPrm `field_0x02` is dLib_anm_prm_c's unnamed 0x02 (no use).
- EventStageEntry (d_event_readable.h) `field_0x0` / `field_0x10` / `field_0x11` / `field_0x12` is a view of dStage_Event_dt_c in d_event_manager.h (another batch); uses: d_event.cpp copies `field_0x10` into dEvt_control_c field_0xde, d_event_manager's search compares `field_0x12` with the asked index and follows `field_0x0` as the next entry (tww d_event_manager.cpp the same). Round 3: the view takes the class names `mNextEventNo` / `mMapToolCamId` / `mDuration`; `field_0x12` stays open in both (getToolId compares it with the asked index, the event camera tests it as MAP_* flags: two roles).
- mt_class / MtExec::Actor `m454` (f32 at HD 0x454; d_a_mt.h, d_a_mt_exec.h) is also declared in mtBodyActorView (d_a_mt_body.h, another batch): daMt_Execute 021D87A0 sets mSegmentScaleY[k] = 1 + sin * it and eases it to 0.1, mt_move_maru 021DF238 eases it to 0.3 and sets 0.5 at the roll start (a segment pulse amplitude, e.g. mSegmentPulseAmp). Round 3: `mSegmentPulseAmp` in the class and both views.
- daStandItem_c `m630` / `m660` / `m6B4`: named with ComStatic::StandItem in round 3, see objects.md.
- The fields listed open in earlier rounds for c_damagereaction, mflft, tag_island, npc_km1 / so / co1 / fa1 / cb1 / btsw2 / cb1 HIO, bpw, himo3, kt, sk, fgmahou, obj_flame, obj_hami3, the sound / message library structs (libraries.md) and MyPicture keep their recorded reasons; no new use was found.
## dNpc_JntCtrl_c (d_npc.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x2E | `field_0x2E` | `mHeadTargetY` | lookAtTarget_2 0259E2D4 stores the head part of the Y turn (from turn_fromHead2Backbone / turn_fromBackbone2Head) before easing mAngles[0][1] toward it, and compares the turn with it + mBackboneTargetY when not turning the body; the constructor clears it (tww the same) |
| 0x32 | `field_0x32` | `mBackboneTargetY` | lookAtTarget_2 0259E2D4 stores the backbone part of the Y turn and eases mAngles[1][1] toward it; next frame it picks the split order by its sign; turn_fromHead2Backbone 0259E1DC gives the head the rest after it, turn_fromBackbone2Head 0259E104 drops a backbone turn against its direction (tww the same) |

Open (offset names kept):

- `field_0x2C`, `field_0x30`: lookAtTarget_2 0259E2D4 stores the head / backbone parts of the X (pitch) turn, never read in WWHD or tww. `field_0x0D`: no use. dNpc_PathRun_c `field_0x04`, `field_0x07`, fopNpc_npc_c `field_0x7d6`: no use / only cleared.


## dBgS_Acch (d_bg_s.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x078 | `field_0x78` | `mpNowBgW` | the dBgW of the current check (tww SetNowActorInfo(bg_index, bgw, apid) stores it); dBgW::positionWallCorrect 024F2514 passes it with m_bg_index / m_ap_id to the hit circle (mpBgW); the constructor 024F0474 clears it (tww SetWallPolyIndex the same) |
| 0x0B0 | `field_0xb0` | `mPrevGroundHit` | GroundCheckInit 024F034C stores the ground-hit bit of the last check (flags bit 5) before clearing it; GroundCheck 024EFF50 sets GROUND_LANDING on a hit when it was 0 and GROUND_AWAY when it was set and there is no hit now (tww the same) |
| 0x0B8 | `field_0xb8` | `mGroundPosY` | GroundCheck 024EFF50: the ground cross height plus m_ground_up_h; the actor is put on it when it is above field_0xb4; GroundRoofProc 024F03A0 compares it with field_0xC4 (tww the same) |

Open (offset names kept):

- `cBgS_LinChk` `field_0x40`: tww Set copies the end point into it; never read in WWHD or tww. `dBgS_RoofChk` `field_0x44`, `dBgS_AcchCir` `field_0x2c`: only initialised. `dBgS_Acch` `field_0xb4`, `field_0xC4`: only the constructor writes 0 (WWHD and tww); GroundCheck / GroundRoofProc compare with them, so they are always 0 there, no role shown.


## dBgWHf (height-field background; d_bg_w_hf.h)

Round 10. tww's dBgWHf (include/d/d_bg_w_hf.h, src/d/d_bg_w_hf.cpp) keeps the offset names and uses them the same way.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0xC4 | `mC4` | `mNormalY` | Set 024F5870 stores its normal argument; CalcPlane 024F4B0C uses it as the y of every grid plane normal (tww the same) |
| 0xC8 | `mC8` | `mpTriMap` | Set 024F5870 stores its map argument; CalcPlane 024F4B0C looks up each grid cell's triangle index in it (tww the same) |

## dScnPly_msg_HIO_c (d_s_play.h; g_msgDHIO, the play-scene message test HIO)

Old names are the GameCube offsets (zeldaret/tww `field_0x..`, unnamed there).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x05 | `field_0x06` | `mShowMessage` | dScnPly_msg_HIO_messageProc 025AF738 runs the HIO test message only while it is set and clears it when the message ends (status 0x12); dScnPly_msg_HIO_padCheck 025AF444 sets it on a B trigger or a request (mShowRequest); dScnPly_Delete 025B0630 clears it; dScnPly_msg_HIO_c_ct 025B3438 clears it |
| 0x06 | `field_0x07` | `mShowRequest` | dScnPly_msg_HIO_setUpdate 025AF43C stores it; dScnPly_msg_HIO_padCheck 025AF444 turns it into mShowMessage and clears it; dScnPly_msg_HIO_c_ct 025B3438 clears it |
| 0x10 | `field_0x10` | `mMsgId` | dScnPly_msg_HIO_messageProc 025AF738: the fopMsgM_messageSet result (-1 while no message), reset to -1 when the message ends; dScnPly_msg_HIO_c_ct 025B3438 and dScnPly_Delete 025B0630 set -1 |

Named under the 2026-11-11 rule (WWHD writes them only in the constructor, tww reads them):

| Struct / WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| dScnPly_msg_HIO_c 0x07 | `field_0x08` | `mLanguage` | WWHD: dScnPly_msg_HIO_c_ct 025B3438 sets 0; tww: the font / text language switch (0 Japanese, else English) read by the mDoExt font init, d_message, d_mesg, f_op_msg_mng |
| dScnPly_env_otherHIO_c 0x40 | `field_0x40` | `mFocusDepthOffset` | WWHD: dScnPly_env_otherHIO_c_ct 025B32FC sets 0; tww: m_Do_graphic subtracts it from the focus depth for the depth-of-field TEV alpha |
| fapGm_HIO_c (f_ap_game.h) 0x05 | `field_0x09` | `mDrawMap` | WWHD: fapGm_HIO_c__ct 025D4384 sets 1; tww: dMap_c::draw draws the map and icons only while it is set |

Open in d_s_play.h (only the constructors set them; no read found in this decomp or tww): dScnPly_msg_HIO_c `field_0x09`; dScnPly_dark_HIO_c `field_0x05`, `field_0x06`; dScnPly_env_otherHIO_c `field_0x05`..`field_0x12`, `field_0x15`, `field_0x16`, `field_0x18`, `field_0x44`; dScnPly_env_debugHIO_c `field_0x08`..`field_0x2c`.

## dGameover_c (game-over screen; d_gameover.h)

Old names are the GameCube offsets (zeldaret/tww `field_0x...`, unnamed there).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x10C | `field_0x114` | `mAnimeTimer` | _create 02549B28 sets 90; _execute 025499A0 starts the game-over graphics at 90 and steps it down (tww dGameover_c::_execute counts it down to 0 after animeOpen, then valueInit / animeClose) |
| 0x10E | `field_0x116` | `mCloseDelay` | WWHD: _create 02549B28 sets l_gameoverDelay, not read here; tww: _create sets g_menuHIO field_0xb6 and _execute counts it down after animeClose before mState 3 (the select window) (2026-11-11 rule: name from tww's read) |

Open in f_ap_game.h (fapGm_HIO_c): every other field_0x.. (only fapGm_HIO_c__ct 025D4384 sets them; no read in WWHD or tww).

## dEnvSe_soundEntry and dEnvSe_c (environment sounds; d_envse.h)

Old names are tww's (stage_sound_data / dEnvSe_c `field_0x..`, unnamed there; stage_sound_data has no other declaration in this decomp).

| Struct / WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| dEnvSe_soundEntry 0x14 | `field_0x14` | `mSeParam` | dEnvSe_c::execute 0252FEB8 passes it to mDoAud_seaEnvSePlay / mDoAud_riverSePlay / mDoAud_waterfallSePlay as the sound parameter of the entry (tww stage_sound_data field_0x14 the same) |
| dEnvSe_soundEntry 0x17 | `field_0x17` | `mType` | dEnvSe_c::execute 0252FEB8 switches on it (dEnvSe_type: sea, shore, river, window, waterfall) (tww the same) |
| dEnvSe_soundEntry 0x18 | `field_0x18` | `mPathNo` | dEnvSe_c::execute 0252FEB8: the room path number of the sound (dEnvSe_getRoomPath(it, room)), walked for the nearest position (tww the same) |
| dEnvSe_c 0xFC | `field_0xf8` | `mWaveCounter` | dEnvSe_c::execute 0252FEB8 (shore): counts 0..99 and wraps; its low half goes to the play mWaveFrame; the shore sound restarts (mDoAud_startSeaShoreSE) when it is 0 (tww the same) |
| dEnvSe_c 0x100 | `field_0xfc` | `mShoreSeArg` | dEnvSe_c::execute 0252FEB8 (shore): the nearest shore path's mArg0 when the shore sound starts, passed to mDoAud_continueSeaShoreSE afterwards (tww the same) |

## dScnOpen_c (opening scene; d_s_open.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x1D8 | `field_0x1d4` | `mSkipRequested` | execute 025AED3C sets 1 on a skip or a button press (and stops the BGM), then changes to the game scene once the stream buffer heap is free while it is set; dScnOpen__ct 025AF024 clears it (tww the same) |

## menu_of_scene_class (debug stage menu; d_s_menu.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x1E5 | `field_0x1e1` | `mStageRepeatTimer` | dScnMenu_Execute 025AC798: the key-repeat timer of up / down (stage cursor): REPEAT_DELAY on the press, REPEAT_INTERVAL each time it runs out (tww the same) |
| 0x1E6 | `field_0x1e2` | `mRoomRepeatTimer` | dScnMenu_Execute 025AC798: the key-repeat timer of A / B (room selection): REPEAT_DELAY on the press, REPEAT_INTERVAL each time it runs out (tww the same) |

Open: room_inf `field_0x20`, stage_inf `field_0x20`, `field_0x22`, `field_0x23` (no use).
## dAttention_c and its request parts (d_attention.h, namespace AttentionHD: Attention, Catch, Look, Param)

zeldaret/tww (d217e2b) keeps the GameCube offset names for these fields and TP has no names for them either, so the
names come from the WWHD uses (d_attention.cpp, d_attention_2.cpp, d_attention_3.cpp).

| Class | WWHD offset | Old | New | Evidence |
|---|---|---|---|---|
| Catch (dAttCatch_c) | 0x04 | `field_0x4` | `mRequestPriority` | dAttCatch_c_request 024EBB70 refuses a request whose priority argument is above it and stores the argument when it is lower (or the actor is nearer); init 024EBB24 / proc 024EBB44 reset it to 3 (tww the same) |
| Look (dAttLook_c) | 0x04 | `field_0x4` | `mRequestPriority` | dAttLook_c_request 024EBD80 / requestF 024EBF40 refuse a request whose priority argument is above it and store the argument when it is lower (or the actor is nearer); init 024EBD4C / proc 024EBD64 reset it to 3 (tww the same) |
| Attention (dAttention_c) | 0x019 | `field_0x019` | `mLockOnStateCopy` | dAttention_c_Run 024EDB80 copies mLockOnState into it every frame after the judgement; the constructors 024ED630 / 024ED73C clear it; never read in WWHD or tww (tww the same) |
| Attention (dAttention_c) | 0x01A | `field_0x01a` | `mLockButtonState` | judgementButton 024ECCFC: 0 released, 1 on the frame L is pressed, 2 while held, 0 when released or the player is busy; judgementStatusSw 024ECED4 / Hd 024ED0A4 lock on, switch or release on 1 / 0 (tww the same) |
| Attention (dAttention_c) | 0x028 | `field_0x028` | `mCursorDrawState` | Draw 024EDDF8 sets 0 while it draws the cursor on a target and, with no target, keeps drawing it at the last target while > 0; runDrawProc 024ED2A0 sets 1 when the release / yj_delete animation starts, -1 when it ends (tww the same) |

Open: Catch `field_0xd`, `field_0xe`, `field_0xf` (no use in WWHD or tww); Param `field_0x04`, `field_0x08`,
`field_0x0c` (only set by the dAttParam_c constructors 024EB904 / 024EB984 to 0 / 45, 30, 90; never read in WWHD or tww);
Attention `field_0x014`, `field_0x01b`, `field_0x01c` (only cleared or set by the constructors, field_0x01b also by Run
024EDB80 during events; never read in WWHD or tww).

## dComIfG_play_c, dComIfG_camera_info_class (d_com_inf_game.h)

Open: every remaining offset field. dComIfG_play_c `field_0x48e2`, `field_0x48e6`, `field_0x492c`, `field_0x493f`,
`field_0x4940`, `field_0x494a`, `field_0x4953`, `field_0x4954` are only cleared by itemInit 025248BC (tww's itemInit
clears them too; no other use in WWHD or tww); `field_0x3EAE`, `field_0x48F8` and dComIfG_camera_info_class
`field_0x07` have no use in WWHD or tww.

## dTimer_c (minigame timer; d_timer.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x110 | `field_0x150` | `mPrevRestTimeMs` | _execute 025C5A8C stores getRestTimeMs() each frame while not stopped; SetSE 025C5960 plays JA_SE_MINIGAME_TIMER_EACH when the rest time's second differs from it; RestTimeCheck 025C591C: threshold crossed since it (tww the same) |
| 0x118 | `field_0x158` | `mEndDelay` | end 025C58A8 stores its delay argument (unless -1); _execute 025C5A8C counts it down in ENDING, then FINISHED (or deleteRequest for VOLCANO); _create 025C5EB8 sets 120 (tww the same) |
| 0x11C | `field_0x15c` | `mShowDelay` | _create 025C5EB8 sets 6; _execute 025C5A8C counts it down and calls timerShow() at 0 (HD; tww counts its draw-in frames up to 5 here) |
| 0x120 | `field_0x160` | `mStartDelay` | startDelay 025C6250 / stock_startDelay 025C5E90 store the delay; _execute 025C5A8C counts it down in START_DELAY / STOCK_DELAY and starts the timer at <= 0 (tww the same) |
| 0x122 | `field_0x162` | `mIsStopped` | stop 025C5794 sets 1, restart 025C57D4 0; _execute 025C5A8C sets 1 under the HD freeze flag and neither counts nor delays while 1; SetSE 025C5960 beeps only while 0 (tww the same) |
| 0x123 | `field_0x163` | `mStopReason` | stop 025C5794 stores its reason argument; restart 025C57D4 restarts only for the same reason and clears it; _create 025C5EB8 clears it (tww the same) |

## dDemo_prm_c, dDemo_prm_data (demo actor parameters; d_demo.h)

| Class | WWHD offset | Old | New | Evidence |
|---|---|---|---|---|
| dDemo_prm_c | 0x8 | `field_0x54` | `mDataSize` | dDemo_actor_c::JSGSetData 0252767C stores its third argument (the data size) next to mId / mData; getPrm_Morf 02526E7C reads a data byte only when it is above that byte's offset; dDemo_prm_ct 02527468 clears it (HD; tww has no such word) |
| dDemo_prm_data | 0x4 | `field_0x4` | `mMorfPrm1` | getPrm_Morf 02526E7C returns it as the morph value of data with id ID_UNK_1 (when mDataSize >= 4) (tww the same) |
| dDemo_prm_data | 0x6 | `field_0x6` | `mMorfPrm4` | getPrm_Morf 02526E7C returns it for id ID_UNK_4 (mDataSize >= 6) (tww the same) |
| dDemo_prm_data | 0x7 | `field_0x7` | `mMorfPrm2` | getPrm_Morf 02526E7C returns it for id ID_UNK_2 (mDataSize >= 7) (tww the same) |
| dDemo_prm_data | 0xB | `field_0xb` | `mMorfPrm5` | getPrm_Morf 02526E7C returns it for id ID_UNK_5 (mDataSize >= 0xB) (tww the same) |
| dDemo_prm_data | 0xF | `field_0xf` | `mMorfPrm6` | getPrm_Morf 02526E7C returns it for id ID_UNK_6 (mDataSize >= 0xF) (tww the same) |

## Particle callbacks and model emitter (particle/d_particle_local.h: dPa_followEcallBack, dPa_smokeEcallBack, dPa_cutTurnEcallBack_c, dPa_J3DmodelEmitter_c)

The same follow / smoke classes are also declared in d/d_particle.h (`m12`, `m13`, `field_0x14`); that copy is not
renamed here.

| Class | WWHD offset | Old | New | Evidence |
|---|---|---|---|---|
| dPa_followEcallBack | 0x12 | `field_0x12` | `mFollowOff` | tww setFollowOff() sets it 1 (WWHD actors write 1 the same way); execute 025A590C moves the emitter to mPos / mAngle only while 0, setup 025A5A04 takes pos/angle only while 0; dPa_followCtor 025A5894 stores the ctor argument (tww the same) |
| dPa_followEcallBack | 0x13 | `field_0x13` | `mTemporary` | setup 025A5A04 keeps the emitter (mpEmitter, Immortal flag) only while 0; dPa_smokeCtorFlags 025A5C04 stores its 'temporary' argument, dPa_followCtor 025A5894 clears it (tww the same) |
| dPa_smokeEcallBack | 0x14 | `field_0x14` | `mSetupInfo` | setup 025A5DD4 stores its setup-info argument (after tww dPa_control_c::set setupInfo); execute 025A5D90 passes it to the lighting call; the ctors 025A5B18 / 025A5C04 / 025A5CEC set -1 (tww the same) |
| dPa_cutTurnEcallBack_c | 0x5 | `field_0x5` | `mEndRequest` | end 025A9D64 sets 1; executeAfter 025A9C3C then stops the emitter (StopEmit, callback cleared) and returns; setup 025A9C20 clears it (tww the same) |
| dPa_cutTurnEcallBack_c | 0x6 | `field_0x6` | `mPosNum` | tww setPosArray(pos_array, n) stores the number of mPosArray entries; executeAfter 025A9C3C makes a particle at each of the first n-1 positions, moves the emitter to the last, then clears it (tww the same) |
| dPa_J3DmodelEmitter_c | 0x20 | `field_0x1c` | `mpAnmTexPattern` | ctor 025A3BDC stores the J3DAnmTexPattern (btp) argument and builds the btp animation from it; dPa_modelEmitterDraw 025A4204 sets its frame to mBtpFrame and enters the animation (tww the same) |
| dPa_J3DmodelEmitter_c | 0x24 | `field_0x20` | `mpBtpAnm` | ctor 025A3BDC: an mDoExt_btpAnm made in mpHeap from mpAnmTexPattern (0 without a pattern); dPa_modelEmitterDraw 025A4204 enters it on modelData (HD; tww: a J3DTexNoAnm array) |
| dPa_J3DmodelEmitter_c | 0x28 | `field_0x24` | `mBtpFrame` | ctor 025A3BDC stores the frame argument once the btp animation exists; dPa_modelEmitterDraw 025A4204 sets mpAnmTexPattern's frame to it (tww the same) |
| dPa_J3DmodelEmitter_c | 0x2A | `field_0x26` | `mRotAxis` | ctor 025A3BDC stores the last argument; dPa_modelEmitterDraw 025A4204 rotates each particle model about y (0), x (1), z (2) or a fixed axis (3) (tww 0-2 the same) |

## STControl and dLib_anm_prm_c (d_lib.h; dLib_anm_prm_c also in gc_api/d/d_lib.h)

STControl is the stick repeat-trigger helper of d_lib (shop cursors, letter sorting, menus). Old names are the GameCube offsets of zeldaret/tww (d217e2b, include/d/d_lib.h), which HD keeps although the vtable moved to the end (all members -4). "TP name": the same member of zeldaret/tp (c8fa8c9e) include/d/d_lib.h STControl, whose members up to 0x26 have the same layout and the same uses (TP adds three more after them). Parameter names ('first', 'minimum', ...) are those of this decomp's STControl_setWaitParm 025885C4.

| Class | WWHD offset | Old | New | Evidence |
|---|---|---|---|---|
| STControl | 0x00 | `field_0x04` | `mThreshold` | TP name (zeldaret/tp include/d/d_lib.h STControl::mThreshold); setWaitParm 025885C4 'high'; checkTrigger 0258873C takes the new direction bits while the stick value is >= it (tww the same) |
| STControl | 0x04 | `field_0x08` | `mReleaseThreshold` | setWaitParm 025885C4 'low'; checkTrigger 0258873C clears the direction bits while the stick value is below it, between it and mThreshold only drops bits (tww the same; TP leaves it unnamed) |
| STControl | 0x08 | `field_0x0c` | `mDirectionTrig` | TP name (zeldaret/tp include/d/d_lib.h STControl::mDirectionTrig); checkTrigger 0258873C sets the LEFT / RIGHT / UP / DOWN bits and returns them; the check*Trigger 02588A0C.. test them; Xinit / Yinit clear their half (tww the same) |
| STControl | 0x09 | `field_0x0d` | `mPrevDirectionTrig` | checkTrigger 0258873C copies mDirectionTrig into it first, then compares (a change resets mAngleShift) and counts the repeat timers only for bits held in both (tww the same); init 025885E8 clears |
| STControl | 0x0A | `field_0x0e` | `mXrepeatTimer` | checkLeftTrigger 02588A0C / checkRightTrigger 02588A7C fire only while it is 0 and reload it with mXwaitTimer; checkTrigger 0258873C counts it down while left / right is held; Xinit 025886EC clears (tww the same) |
| STControl | 0x0C | `field_0x10` | `mYrepeatTimer` | checkUpTrigger 02588AEC / checkDownTrigger 02588B5C fire only while it is 0 and reload it with mYwaitTimer; checkTrigger 0258873C counts it down while up / down is held; Yinit 02588714 clears (tww the same) |
| STControl | 0x0E | `field_0x12` | `mRepeatDelayY` | TP name (zeldaret/tp include/d/d_lib.h STControl::mRepeatDelayY); setWaitParm 025885C4 'first'; init 025885E8 / Xinit 025886EC / Yinit 02588714 reset mXwaitTimer and mYwaitTimer to it (tww the same) |
| STControl | 0x10 | `field_0x14` | `mRepeatDelayX` | TP name (zeldaret/tp include/d/d_lib.h STControl::mRepeatDelayX); setWaitParm 025885C4 'minimum'; the check*Trigger 02588A0C.. never let mXwaitTimer / mYwaitTimer drop below it (tww the same) |
| STControl | 0x12 | `field_0x16` | `mRepeatDelayStep` | setWaitParm 025885C4 'step'; the check*Trigger 02588A0C.. subtract it from mXwaitTimer / mYwaitTimer per repeat once the step delay count is 0 (tww the same; TP leaves it unnamed) |
| STControl | 0x14 | `field_0x18` | `mXwaitTimer` | TP name (zeldaret/tp include/d/d_lib.h STControl::mXwaitTimer); checkLeftTrigger 02588A0C / checkRightTrigger 02588A7C load mXrepeatTimer with it and shrink it by mRepeatDelayStep; init / Xinit 025886EC reset it to mRepeatDelayY (tww the same) |
| STControl | 0x16 | `field_0x1a` | `mYwaitTimer` | TP name (zeldaret/tp include/d/d_lib.h STControl::mYwaitTimer); checkUpTrigger 02588AEC / checkDownTrigger 02588B5C load mYrepeatTimer with it and shrink it by mRepeatDelayStep; init / Yinit 02588714 reset it to mRepeatDelayY (tww the same) |
| STControl | 0x18 | `field_0x1c` | `mStepDelayCount` | setWaitParm 025885C4 'delay'; init 025885E8 / Xinit 025886EC / Yinit 02588714 copy it into mXstepDelayCount / mYstepDelayCount (tww the same; TP leaves it unnamed) |
| STControl | 0x1A | `field_0x1e` | `mXstepDelayCount` | checkLeftTrigger 02588A0C / checkRightTrigger 02588A7C count it down per repeat and only shrink mXwaitTimer once it is 0; init / Xinit 025886EC load mStepDelayCount (tww the same) |
| STControl | 0x1C | `field_0x20` | `mYstepDelayCount` | checkUpTrigger 02588AEC / checkDownTrigger 02588B5C count it down per repeat and only shrink mYwaitTimer once it is 0; init / Yinit 02588714 load mStepDelayCount (tww the same) |
| STControl | 0x1E | `field_0x22` | `mAngleShift` | checkTrigger 0258873C adds it to every direction sector bound; when the direction bits change it becomes +mAngleMargin or -mAngleMargin (by the half of the sector the stick is in), 0 when released; init 025885E8 clears (tww the same) |
| STControl | 0x20 | `field_0x24` | `mAngleMargin` | setWaitParm 025885C4 'margin'; checkTrigger 0258873C sets mAngleShift to +/- it when the direction changes (tww the same; TP leaves it unnamed) |
| STControl | 0x22 | `field_0x26` | `mDiagonalWidth` | setWaitParm 025885C4 'sector'; checkTrigger 0258873C: the bounds move by (0x2000 - it) >> 1, so each diagonal sector is it wide (0x2000 default) (tww the same; TP leaves it unnamed) |

Open: dLib_anm_prm_c `field_0x02` (both headers; dLib_setAnm 025877F8 never reads it and every tww table has 0 there; the same field stays open in the views TcAnmPrm (d_a_npc_tc.h) and Kg1AnimationParameter (d_a_npc_kg1_anim.h)).

## Round 3 alignment: copies and views follow their class

Copies / views that round 2 left with offset names because their class sat in another batch. One name per field: the
view takes the class name; the evidence stays with the class (named in each declaration's comment).

| View / copy (header) | Old | New | Class (header) |
|---|---|---|---|
| dDoor_info_c (d_door_checks.h, d_door_key.h) | `m2A1`, `m2C6` | `mStopOpenTimer`, `mEventKind` | dDoor_info_c (d_door_info.h) |
| dPa_followEcallBack (d_particle.h) | `m08`, `m0C`, `m10`, `m11`, `m12`, `m13` | `mPos`, `mAngle`, `mFlag`, `mRateOff`, `mFollowOff`, `mTemporary` | pa::dPa_followEcallBack (particle/d_particle_local.h) |
| dPa_smokeEcallBack (d_particle.h) | `field_0x14` | `mSetupInfo` | pa::dPa_smokeEcallBack (particle/d_particle_local.h) |
| BgnCreateSmoke (d_a_bgn_create.h) | `field_0x12` | `mFollowOff` | pa::dPa_followEcallBack |
| fopMsg_prm_class (f_op_msg_mng.h) | `field_0x14` | `mStartMsgNo` | fopMsgLifecycle::fopMsg_prm_class (f_op_msg.h); writers fopMsgM_create 025DB374, fopMsgM_Timer_create 025DB468 |
| daTagKbItem_c (d_a_tag_kb_item.h) | `field_0x298`, `field_0x299` | `mDigDone`, `mDigRequest` | daTagKbItem_c (d_com_static.h) |
| PhotoLenzo (d_a_tag_photo.h) | `field_0x9C1` | `mPlaceState` | daNpcPhoto_c (d_a_npc_photo.h) |
| ArrowIceParentHD (d_a_arrow_iceeff.h) | `field_0x6a8` | `mImpactPos` | daArrow_c (d_a_arrow.h) |
| WindTagView (d_a_windmill.h) | `field_0x49c` | `mHeightScale` | daWindTag_c (d_a_wind_tag.h) |
| LinkPoseLook (d_a_player_main_04.h) | `field_0x4` | `mRequestPriority` | AttentionHD::Look (d_attention.h) |
| EventStageEntry (d_event_readable.h) | `field_0x0`, `field_0x10`, `field_0x11` | `mNextEventNo`, `mMapToolCamId`, `mDuration` | dStage_Event_dt_c (d_event_manager.h) |
| daNpc_P2_c (d_a_npc_p2.h, the class) | `m8C2`, `m8C9` | `mbGoalTalkStarted`, `mbMessageGestureStarted` | names of its copy in d_a_npc_p2_act.h (evidence in npcs.md) |
| dScnKy_env_light_c (d_kankyo.h, the class) | `field_0x10e8` | `mMonotoneBlend` | read through monotoneBlend() of d_kankyo_4.h (other.md) |

### Round 3, part 2: classes named together with their copies / views

| Class (header) and copies | Old | New | Evidence |
|---|---|---|---|
| daNpcPeople_c (six d_a_npc_people*.h copies) | `m7A0` | `mPartnerFacePlayer` | setMessageUb 022C1168 stores the Ub message's mFacePlayer; eventUb2Talk 022C2380: the partner Ub copies it into its mBodyTurn and mLookAtPlayer and opens its attention to 1000 / 0x7FFF when 1 (tww the same) |
| sUbMsgDat (six copies) | `field_0x06` | `mFacePlayer` | setMessageUb 022C1168 copies it into mBodyTurn and mLookAtPlayer, widens the attention ranges when 1, keeps it in mPartnerFacePlayer (tww the same) |
| mt_class (d_a_mt.h), MtExec::Actor, mtBodyActorView | `m454` | `mSegmentPulseAmp` | daMt_Execute 021D87A0: mSegmentScaleY[k] = 1 + sin * it, eased to 0.1; mt_move_maru 021DF238 eases it to 0.3, sets 0.5 at the roll start |
| daStandItem_c (d_a_dai_item.h), ComStatic::StandItem | `m630`, `m660`, `m6B4` | `mBaseJointMtx`, `mHeadJointMtx`, `mSpinAngle` | daiItemNodeCallBack 02526580 copies the Fountain Idol's joint 0 / 1 matrices, turns the pinwheel by mSpinAngle; actionFobj09 02118390 / actionFobj06 02117F20 (tww the same) |
| daNpc_Km1_c (d_a_npc_km1.h) | `field_0x7BC` | `mbInitEyePos` | createInit 02271E78 1; setAttention 02271CBC (same code as Kp1 / Pm1) |
| enemyice (c/c_damagereaction.h) and its 15 enemyice / enemyice_l copies | `m00C`, `m028`, `m02C` | `mFreezeType`, `mBodyLiftY`, `mIceOffsetY` | enemy_ice 020402C8 / ice_render: hop only at 0, no carry at 1, no gravity at 2 (fm, tn, rd, bo, pz set 1); the body lift eased toward the ice block's offset; daBb_Execute 0205EA58 sets 50 and lifts its body by mBodyLiftY (tww the same) |
| RAIN_EFF copies (d_kankyo_rain_kazanbai_move.h, d_kankyo_rain_cloudShadowMove.h) | `field_0x30` | `mGroundY` | d_kankyo_rain.h's name; dKyr_rain_move 02566B88 respawns the drop below it |
| RAIN_EFF (d_kankyo_wether.h) | `mSplashHeight` (misnamed) | `mGroundY` | the same field; no reader through this header |
| NameScene::SaveView (d_s_name.h) | `field_0x7BC` | `mDanStageNo` | dSv_info_c::mDan.mStageNo (the view starts 0x20 before dSv_info_c); changeGameScene 025ADC60 sets 255 (tww dComIfGs_resetDan) |

Still open after this pass, with the reason: enemyice `m1B0` (no read); PlayView `field_0x62F1` (views disagree);
dStage_Event_dt_c / EventStageEntry `field_0x12` (two roles); daNpc_P2_c `m924`, `m978`; dCloth_packet_c and
SieFlagClothHD `field_0xF0` / `field_0xF2`, WIND_INFLUENCE `field_0x20` and its views, cb1 HIO, JAIZelBasic, MyPicture, mo2,
Zl1, dNpc_PathRun copies (the class fields are open for no-use reasons, not because of sharing); Ym1StateView `m8A0`
(class mA10: one read, no writer found).

### Round 3, part 3: two misnamed copies

- enemyfire / enemyfire_l 0x228 in d_a_pt, ki, am, oq, cc, fm, ks, rd: the copies declare mLight (0x208) as its first 0x20
  bytes, and LIGHT_INFLUENCE is 0x24 in HD, so 0x228 is LIGHT_INFLUENCE::mHD20 (the HD tail its constructor sets to 1.0;
  c_damagereaction.h: enemyfire::mLight.mHD20). The enemyfire constructors set it to 1.0, nothing reads it: every copy is now
  `mHD20` (d_a_pt.h / d_a_ki.h had `mParticleScaleMultiplier`, misnamed; the others `m228`). d_a_pt.h's union alias m228 is gone.
- RAIN_EFF in d_kankyo_wether.h: `mSpeed` / `mWindScale` / `mGravity` / `mSplashRadius` had no evidence; back to the open names
  `field_0x1c` / `field_0x20` / `field_0x24` / `field_0x2c`, as in the other copies (dKyr_rain_move 02566B88 sets 0x1c / 0x20 /
  0x24 at spawn, nothing reads them; 0x2c no use).

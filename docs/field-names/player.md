# Field names: Link

Offset-named fields of Link (`daPy_lk_c`, `daPy_py_c`, `daPy_npc_c`) and of the Link classes and views in the
d_a_player* headers, named in the field naming pass (2026-11-10; rules in docs/decomp-notes.md,
"Field names (offset-named fields)"). A name comes from how the original code uses the field (the address of the use
and what it shows), from names already in this decomp, or from the CC0 zeldaret/tww decomp. Renames only: offsets,
types and code are unchanged (objcmp: object-identical). Each header declaration keeps the old name (`was ...`) with
the evidence. Fields without enough evidence keep their offset name and are listed as open. The `m3xxx` names of
daPy_lk_c are the GameCube offsets (zeldaret/tww); the WWHD offset is the one in d_a_player_main.h.

## daPy_anmHeap_c (d_a_player_main_01.h)

Views with the same names: anmHeap_c (d_a_player_main_02.h), LinkPoseAnmHeap (_04.h), player04b::AnmHeap (_04b.h), Link05AnmHeapHD (_05.h), Link08AnmHeap (_08.h). LinkActionAnmHeap (_03.h) takes them too (round 2).

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x2 | `field_0x2` | `mPriIdx` | setPriTextureAnime 023E7B24 (d_a_player_main_02) stores the btp / btk index of the priority texture animation; resetPriTextureAnime 023DC58C clears it (0xFFFF) and reloads mIdx unless mDemoIdx is set; setTextureAnime 023DD768 loads mIdx only while it and mDemoIdx are 0xFFFF; getAnimeResource 023DE71C 0xFFFF for bck heaps (tww the same) |
| 0x4 | `field_0x4` | `mDemoIdx` | the event animation index, which wins over mPriIdx and mIdx: setDemoTextureAnime 0241FCB4 / changeTextureAnime 02443DA0 / dProcTool 0241FD7C (bck from the demo archive) set it; resetDemoTextureAnime 023DDD48 clears it (0xFFFF) and reloads mPriIdx or mIdx (tww the same) |
| 0x6 | `field_0x6` | `mDemoAnmNo` | the demo animation number paired with mDemoIdx: setDemoTextureAnime 0241FCB4 / dProcTool 0241FD7C store theirs (tww getDemoMode() / DEMO_NEW_ANM0_e - 1), changeTextureAnime 02443DA0 0xFFFE; they reload only when it or mDemoIdx changes; setSeAnime 023E07E4 keeps it with mSeAnmIdx; resetDemoTextureAnime 023DDD48 0xFFFF (tww the same) |

## daPy_lk_c: effect callbacks and the waterfall ripple timer (d_a_player_main.h)

The views take the same names: LinkCollisionLayout (d_a_player_main_04_collision.h), LinkItemLayout (_04_item.h), LinkAuraLayout (_04_aura.h), LinkEffectLayout (_04_effect.h); LinkWaterRain (_04_water.h) and lk05DemoFollow (_05.h) use the members. The callbacks still used by d_a_player_main_03 / _07 (m3280, m32E4, m32F0, m33A8, m33E8) follow in that round.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x6730 | `m331C` | `mCutTurnEcallBack0` | dPa_cutTurnEcallBack_c of the spin-attack trail ID_AK_JN_ROUNDATTACK00: setCollision 024076CC starts it (with 01 / 02 below), feeds it the sword-blur positions while its alpha is 0xFF and removes it; procCutTurn 02441750 fades the alpha 0x80 -> 0; playerDelete 0240EBF0 removes it (tww the same) |
| 0x6740 | `m332C` | `mCutTurnEcallBack1` | the same for ID_AK_JN_ROUNDATTACK01: setCollision 024076CC, procCutTurn 02441750, playerDelete 0240EBF0 (tww the same) |
| 0x6750 | `m333C` | `mCutTurnEcallBack2` | the same for ID_AK_JN_ROUNDATTACK02: setCollision 024076CC, procCutTurn 02441750, playerDelete 0240EBF0 (tww the same) |
| 0x6760 | `m334C` | `mNeckWaterDropEcallBack` | daPy_waterDropEcallBack_c at the neck joint: setWaterDrop 0240CBE0 puts it at CL_JNT_NECK_JNT and runs setSwimWaterDrop on it; offBodyEffect 023D83D8 / onBodyEffect 023D8FF8 stop / play its particles; the constructor 0240FE40 sets its vtable (tww the same) |
| 0x6780 | `m336C` | `mBodyWaterDropEcallBack` | the same at the body joint CL_JNT_BODY_CHN: setWaterDrop 0240CBE0, offBodyEffect 023D83D8 / onBodyEffect 023D8FF8; setFootMark 02405BC0 / setCutWaterSplash 02406DE0 test its emitter (tww the same) |
| 0x67A0 | `m338C` | `mTalismanFlashEcallBack` | daPy_followEcallBack_c of ID_IT_JN_OMAMORI_FLASH00: dProcTalismanWait_init 02422950 starts it at the hand position; commonProcInit 023DFDD8 / playerDelete 0240EBF0 end it (tww the same, plus dProcTalismanWait) |
| 0x6814 | `m3400` | `mWaterfallSplashEcallBack` | daPy_mtxPosFollowEcallBack_c of the waterfall splash: setWaterRipple 0240AB88 makes ID_AK_JN_ELEMENTSHIBUKI00 on the head joint while Link stands or swims in a waterfall (or hangs on attribute 0x17), ends it otherwise and tints it with the sea colour (tww the same) |
| 0x6824 | `m3410` | `mWadeSplashEcallBack` | daPy_followEcallBack_c of the wading splash: setFootEffect 02406480 places ID_AK_JN_ELEMENTSHIBUKI00 45 in front of Link at the water surface, facing back, while he walks in water deeper than 25, and ends it otherwise (tww the same) |
| 0x6840 | `m342C` | `mMirrorShieldEcallBack` | daPy_mtxFollowEcallBack_c of ID_AK_SN_MIRRORSHIELD00 on the shield model: checkLightHit 0240584C makes it while the mirror shield reflects light and ends it (alpha 0) otherwise (tww the same) |
| 0x684C | `m3438` | `mShieldLightHitEcallBack` | daPy_followEcallBack_c of ID_AK_SN_HITSHIELDLIGHT00: checkLightHit 0240584C puts it where the reflected beam (mFanLightCps) hits, larger on Jalhalla (fpcNm_BPW_e), and ends it when nothing is hit (tww the same) |
| 0x6868 | `m3454` | `mLightSaverEcallBack` | daPy_mtxFollowEcallBack_c of ID_AK_JN_LIGHTSAVER00 on the sword joint: setLightSaver 0240C32C makes it in chance mode or with the soup power-up and ends it; deleteEquipItem 023DC7AC ends it (tww the same) |
| 0x6874 | `m3460` | `mRainDropEcallBack` | two daPy_mtxPosFollowEcallBack_c of ID_AK_JN_LK_NURE_POTA01 on the neck / body joints: setWaterDrop 0240CBE0 makes them while nothing is overhead (dKyr_player_overhead_bg_chk, no roof, not a MISC stage) and ends them otherwise; offBodyEffect 023D83D8 / onBodyEffect 023D8FF8 (tww the same) |
| 0x6912 | `m34CD` | `mWaterfallRippleTimer` | setWaterRipple 0240AB88: when 0 it sets the ID_IT_JN_WP_HAMON03 ripple at the root joint, starts a quake and sets 15; otherwise counts down and stops the quake at 5 (tww the same) |

## daPy_waterDropEcallBack_c and daPy_swimTailEcallBack_c (d_a_player_main_04b.h)

Views with the same names: LinkWaterCallback (d_a_player_main_04_water.h) and SwimTailCallback (d_a_player_main_07.h).

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x1C | `field_0x1C` | `mReady` | TWW checkReady() / onReady() / offReady(): setSwimWaterDrop 0240CAAC (d_a_player_main_04) sets 1 when the drop point goes under water (not in daPyProc_VOMIT_WAIT) and, once out of the water with no emitter, starts the drip emitter while it is set and clears it |
| 0x05 | `field_0x05` | `mRightFlg` | TWW onRightFlg(): setup 02415138 clears it; setSwimTail 02412D54 sets 1 on mSwimTailEcallBack[0] (onRightFlg); execute 02414E8C then points the emitter along l_right_dir (tww the same) |
| 0x20 | `field_0x20` | `mpAngle` | the csXyz pointer passed to setup 02415138 (TWW onEnd() clears it); execute 02414E8C rotates the emitter by its y (setGlobalRotation(0, it->y, 0)) while it is set |

## daPy_footData_c (d_a_player_main_01.h)

The leg and foot ground-fit data of mFootData[2] (0 right, 1 left). Views with the same names: LinkPoseFootData (d_a_player_main_04.h), PlayerMoveFoot (_06.h, mToePos only); LinkMotionFootData (_03.h) takes them too (round 2; its field_0x004 stays open as in the class). Open: field_0x004 (s16), eased by footBgCheck 0240005C toward the ground angle across the foot (shape_angle.y - 0x4000) but read nowhere in WWHD or tww.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x000 | `field_0x000` | `mGroundHit` | footBgCheck 0240005C (d_a_player_main_03): 1 when the ground cross under the foot is within 60.2 of the probe, else 0; the foot is tilted by the ground angle only while it is set (tww the same) |
| 0x001 | `field_0x001` | `mStopTimer` | footBgCheck 0240005C: set to 5 whenever the foot centre moves 10 or more (XZ); while it counts down the centre follows the foot, at 0 the stored mFootPos is kept (tww the same) |
| 0x002 | `field_0x002` | `mFootAngle` | jointCB1 02403150: ZrotM of the foot joint; footBgCheck 0240005C sets -(m34E0 + mLegBAngle) plus the ground angle along shape_angle.y (tww the same) |
| 0x006 | `field_0x006` | `mLegBAngle` | footBgCheck 0240005C eases it (cLib_addCalcAngleS) toward the lower-leg angle from setLegAngle 023FFBE0; mLegBJointAngle = it - mLegAAngle, mFootAngle starts from -(m34E0 + it) (tww the same) |
| 0x008 | `field_0x008` | `mLegAAngle` | jointCB1 02403150: ZrotS of the upper leg joint (LLEGA / RLEGA); footBgCheck 0240005C eases it toward the upper-leg angle from setLegAngle 023FFBE0 (tww the same) |
| 0x00A | `field_0x00A` | `mLegBJointAngle` | jointCB1 02403150: ZrotM of the lower leg joint (LLEGB / RLEGB); footBgCheck 0240005C sets mLegBAngle - mLegAAngle (tww the same) |
| 0x00C | `field_0x00C` | `mHeelPos` | posMoveFromFootPos 023FCB9C: the heel position (default rheel_pos_offset (-10.85, 0, -6.52), x mirrored for the left foot); footBgCheck 0240005C probes at the midpoint of it and mToePos (tww the same) |
| 0x018 | `field_0x018` | `mToePos` | posMoveFromFootPos 023FCB9C: the toe position (default rtoe_pos_offset (-14.05, 0, 5.02), x mirrored for the left foot); footBgCheck 0240005C probes at the midpoint of it and mHeelPos; changeHangEndProc 02428230 compares mHangGroundH with its y (tww the same) |
| 0x024 | `field_0x024` | `mFootPos` | footBgCheck 0240005C: the foot centre (midpoint of mToePos and mHeelPos) of the last frame; kept instead of the new one once mStopTimer is 0 and the foot moved less than 10 (tww the same) |
| 0x30 | `field_0x030` | `mClotchShiftX` | footBgCheck 0240005C: 0.3 x the height difference for the higher foot, 0 otherwise; jointBeforeCB 023D6B30 subtracts it from the translate x of the CLOTCH joint (tww jointCB0 the same) |
| 0x34 | `field_0x034` | `mGndChk` | footBgCheck 0240005C: SetPos / GroundCross of the ground check under the foot, then getGroundAngle on it (tww the same) |
| 0x88 | `field_0x088` | `mLegMtx` | jointAfterCB 023D7564 copies the LEGA, LEGB and FOOT joint matrices into [0] / [1] / [2]; setLegAngle 023FFBE0 reads them (tww the same) |

## daPy_swBlur_c (d_a_player_main_04_item.h)

The sword trail (HD offsets; the old names are the GameCube offsets). The view SwordBlur (d_a_player_main_07.h) takes mSegmentNum / mTopPos (round 2).

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x9C | `field_0x014` | `mSegmentNum` | the number of trail segments: initSwBlur 02407070 clears it, copySwBlur 02407310 adds the new samples (at most 58); setCollision 024076CC clears it after the spin attack and fades it by 10 per frame; procHammerFrontSwing 0243CA98 walks the first it mTopPos (tww the same, draw loops over it) |
| 0x0A0 | `field_0x018` | `mBufferIdx` | the frame index into mpPosBuffer (two positions per frame): initSwBlur 02407070 stores idx, copySwBlur 02407310 samples from the old to the new one and stores it; setCollision 024076CC uses its change as the cut-turn position count (tww the same) |
| 0x0B0 | `field_0x028` | `mPrevBasePos` | initSwBlur 02407070 / copySwBlur 02407310: the translation of the model matrix of the last sample; copySwBlur spreads the move since then over the new samples (tww the same) |
| 0xBC | `field_0x034` | `mTopPos` | initSwBlur 02407070 / copySwBlur 02407310: the outer trail edge, mpPosBuffer[2i] moved out from mRootPos by mBlurTopRate, newest first; procHammerFrontSwing 0243CA98 tests its heights (tww the same, draw's first vertex row) |
| 0x38C | `field_0x304` | `mRootPos` | initSwBlur 02407070 / copySwBlur 02407310: the inner trail edge, mpPosBuffer[2i + 1], newest first; setCollision 024076CC hands it to the three mCutTurnEcallBack (setPosArray) (tww the same) |

## daPy_npc_c (d_a_player_npc.h) and the npc_md view daPy_npc_l

The view daPy_npc_l (d_a_npc_md.h) takes the same names (its base daPy_py_l already has mWindVelocity from round 2). daPy_py_c field_0x2b0 (and its views daPy_py_l / TnPlayer) follows in the d_a_player_main_03 / _07 round; daPy_py_c field_0x2BA[2] is padding.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x600 | `m4E4` | `mNpcFlags` | the player-NPC flag word of the TWW accessors returnLink() / isReturnLink() (bit 1) and onEventAccept() / isEventAccept() (bit 2): checkNowPosMove 02445950 tests both; Medli / Makar / the stone heads (daNpc_Md_c::eventProc 02287EFC, daNpc_Cb1_c::eventProc 02220288, daNpc_Os_c::eventProc 022AB6DC) set and clear them |
| 0x604 | `m4E8` | `mFallTimer` | frames without ground: daNpc_Md_c::execute 02288B08, cb1_execFallCheck and daNpc_Os_c::execute 022AC1DC count it up to 30 while the NPC has no ground (or falls) and clear it otherwise; check_moveStop 024452A0 puts the NPC back home at 30 and clears it (tww check_moveStop the same) |

## daPy_lk_c: hat and tunic sway (d_a_player_main.h)

setHatAngle 024022D0 and jointBeforeCB 023D6B30: the three hat joints HATA / HATB / HATC, each with a Z and a Y angle, a per-frame speed and (Z only) a wind swing, plus the two MOMI joints.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x6946 | `m34F6` | `mHatAAngleZ` | jointBeforeCB 023D6B30: the Z angle of the HATA joint (with mHeadFrontAngleX - 0x8000 and mHatASwingZ); setHatAngle 024022D0 eases it toward the wind / motion direction (cLib_addCalcAngleS2), adds mHatASpeedZ and the head pitch change and clamps it (tww the same) |
| 0x6948 | `m34F8` | `mHatAAngleY` | jointBeforeCB 023D6B30: the Y angle of the HATA joint; setHatAngle 024022D0 eases it toward the sideways direction, adds mHatASpeedY minus the head yaw change, clamps it to +-0x3800 (tww the same) |
| 0x694A | `m34FA` | `mHatBAngleZ` | jointBeforeCB 023D6B30: the Z angle of the HATB joint (with mHatBSwingZ); setHatAngle 024022D0 takes off mHatAAngleZ's change, eases it to 0, adds mHatBSpeedZ, clamps it to -0x800..0x1000 (tww the same) |
| 0x694C | `m34FC` | `mHatBAngleY` | jointBeforeCB 023D6B30: the Y angle of the HATB joint; setHatAngle 024022D0 takes off mHatAAngleY's change, eases it to 0, adds mHatBSpeedY, keeps it + mHatAAngleY within +-0x3800 (tww the same) |
| 0x694E | `m34FE` | `mHatCAngleZ` | jointBeforeCB 023D6B30: the Z angle of the HATC joint (with mHatCSwingZ); setHatAngle 024022D0 takes off mHatBAngleZ's change, eases it to 0, adds mHatCSpeedZ, clamps it to -0x800..0x1000 (tww the same) |
| 0x6950 | `m3500` | `mHatCAngleY` | jointBeforeCB 023D6B30: the Y angle of the HATC joint; setHatAngle 024022D0 takes off mHatBAngleY's change, eases it to 0, adds mHatCSpeedY, keeps the sum of the three Y angles within +-0x3800 (tww the same) |
| 0x6952 | `m3502` | `mHatASpeedZ` | setHatAngle 024022D0: 0.2 x the frame's change of mHatAAngleZ, added to it on the next frame (tww the same) |
| 0x6954 | `m3504` | `mHatASpeedY` | setHatAngle 024022D0: 0.2 x the frame's change of mHatAAngleY, added to it on the next frame (tww the same) |
| 0x6956 | `m3506` | `mHatBSpeedZ` | setHatAngle 024022D0: 0.2 x the frame's change of mHatBAngleZ, added to it on the next frame (tww the same) |
| 0x6958 | `m3508` | `mHatBSpeedY` | setHatAngle 024022D0: 0.2 x the frame's change of mHatBAngleY, added to it on the next frame (tww the same) |
| 0x695A | `m350A` | `mHatCSpeedZ` | setHatAngle 024022D0: 0.2 x the frame's change of mHatCAngleZ, added to it on the next frame (tww the same) |
| 0x695C | `m350C` | `mHatCSpeedY` | setHatAngle 024022D0: 0.2 x the frame's change of mHatCAngleY, added to it on the next frame (tww the same) |
| 0x695E | `m350E` | `mHatASwingZ` | setHatAngle 024022D0: the wind swing of HATA, 2280 x strength x cos(mHatSwingPhase); jointBeforeCB 023D6B30 adds it to the HATA Z angle (tww the same) |
| 0x6960 | `m3510` | `mHatBSwingZ` | setHatAngle 024022D0: the wind swing of HATB, 3908 x strength x cos(mHatSwingPhase - 3 steps); jointBeforeCB 023D6B30 adds it to the HATB Z angle (tww the same) |
| 0x6962 | `m3512` | `mHatCSwingZ` | setHatAngle 024022D0: the wind swing of HATC, 7568 x strength x cos(mHatSwingPhase - 6 steps); jointBeforeCB 023D6B30 adds it to the HATC Z angle (tww the same) |
| 0x6964 | `m3514` | `mHatSwingPhase` | setHatAngle 024022D0: advanced by 1500 + 4060 x the wind strength each frame; the phase of mHatASwingZ / mHatBSwingZ / mHatCSwingZ (tww the same) |
| 0x6966 | `m3516` | `mLMomiAngleY` | setHatAngle 024022D0: -8192 x the sideways wind x strength, at most 0x1000, 0 while swimming; jointBeforeCB 023D6B30: the Y rotation of the LMOMI joint (tww the same) |
| 0x6968 | `m3518` | `mRMomiAngleY` | setHatAngle 024022D0: the same value as mLMomiAngleY, at least -0x1000, 0 while swimming; jointBeforeCB 023D6B30: the Y rotation of the RMOMI joint (tww the same) |
| 0x696A | `m351A` | `mMomiAngleZ` | setHatAngle 024022D0: -8192 x the forward wind x strength, 0 while swimming; jointBeforeCB 023D6B30: the Z rotation of both MOMI joints (tww the same) |
| 0x6978 | `m3528` | `mHeadFrontAngleX` | setHatAngle 024022D0: the pitch of the head's front vector (l_neck_front), whose change (halved, clamped to +-0x200) moves the hat Z angles; jointBeforeCB 023D6B30 adds it to the HATA Z angle (tww the same) |
| 0x697A | `m352A` | `mHeadFrontAngleY` | setHatAngle 024022D0: the yaw of the head's front vector (kept while it points steeply up or down), whose change (halved, clamped to +-0x800) moves the hat Y angles (tww the same) |
| 0x7320 | `m3718` | `mHatPrevPos` | setHatAngle 024022D0: the HATA joint position of the last frame (set to the new one at the end); the hat leans away from its movement since then (tww the same) |

## daPy_lk_c: sword combo, get-item event, attention angle, slide / start / wall timers, ship-ride follow speeds, Deku Leaf joints (d_a_player_main.h)

Open in this group: m351E (procCutF / R / A / L_init and procWeaponNormalSwing_init copy m34DC into it; read nowhere in WWHD or tww) and m3548 (changeDragonShield 023E9D50 sets 60; nothing in WWHD reads it; only tww's setAuraEffect counted it down, which HD's setAuraEffect 0240B7F4 does not).

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x68E0 | `m34C4` | `mCutComboCount` | the sword combo count: changeCutProc 023E73FC adds 1 per cut and picks the cut by it (4 the finishing EA / EB cut, mCutCount from it); execute 0240CDD0 adds 2 on a hit at 2 / 3 and clears it when m3522 runs out; procCutTurn_init 023E6380 / procCutExMJ_init 023E6DC4 test 6; the finishing cuts clear it (tww the same) |
| 0x6913 | `m34CE` | `mGetDemoFlg` | setGetDemo 023DBDD0: bit 1 the DEFAULT_GETITEM event is run here (in the air or off the ground), bit 2 the get-item sound was started, bit 4 the item message closed (cut end); cleared when no get-item event runs (tww the same) |
| 0x692E | `m34E6` | `mAtnShapeAngleY` | the shape angle kept while moving with attention: setAtnList 023DCD30, procWait 024177A8 and procCutTurnMove 02442470 store it, setSpeedAndAngleAtn 02417538 / setSpeedAndAngleAtnBack 0241699C put shape_angle.y back to it; procCutTurnMove takes the stick direction relative to it (tww the same) |
| 0x6976 | `m3526` | `mSlideStartTimer` | changeSlideProc 023E4CD0: 8 off slide polygons; on one it counts down while the normal speed is eased to 0 and the slide starts only when it is 0 and the speed is 5 or less; procSlideFront_init 023E4B38 / procSlideBack_init 023E4C04 / playerInit 02410BE8 set 8 (tww the same) |
| 0x697E | `m352E` | `mBgWaitTimer` | makeBgWait 024131D0: 900 from the player init, counted down each call; while set the start waits for a treasure-chest (TBOX) ground or, with parameter bit 0x80, for a raft (OBJ_IKADA) ground (then cleared) (tww the same) |
| 0x6990 | `m3540` | `mShipRideAngleXSpeed` | setShipRidePos 023E287C: the follow speed of m353C toward the ship's shape_angle.x (adds 0.75 x the difference, eased to 0, added to m353C each frame); procShipReady_init 023ECA94 clears it (tww the same) |
| 0x6992 | `m3542` | `mShipRideAngleZSpeed` | setShipRidePos 023E287C: the follow speed of m353E toward the ship's shape_angle.z (adds half the difference, eased to 0, added to m353E each frame); procShipReady_init 023ECA94 clears it (tww the same) |
| 0x6994 | `m3544` | `mFrontWallTimer` | changeFrontWallTypeProc 02418E00: frames Link keeps pushing against the front wall; above the HIO small-jump / wall-catch frame counts it starts the small jump, hang, or vertical jump, otherwise cleared; commonProcInit 023DFDD8 clears it (tww the same) |
| 0x69A8 | `m3558` | `mFanSmaAngleZ` | fanJointCB 023D641C: ZrotS of the FANSMA joint of the small Deku Leaf model; setShapeFanLeaf 023E771C sets 0x600 x sin(m355C) (tww the same) |
| 0x69AA | `m355A` | `mFanSmbAngleZ` | fanJointCB 023D641C: ZrotS of the FANSMB joint; setShapeFanLeaf 023E771C sets 0x800 x sin(m355C - 0xE00) (tww the same) |

## daPy_lk_c: neck angle, per-joint state, sword root / old positions, grab bounce, ladder top, roll crash sound (d_a_player_main.h)

m3658 is the whole per-joint block since the extent fix (m3658 + m3668 + m6AE0 -> m3658[0x7E0]). Open in this area: m3668 / m6AE0 no longer exist; m69E8 / m8260 below.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x69B4 | `m3564` | `mNeckAngle` | jointBeforeCB 023D6B30 rotates the head joint by it (y, z, x); setNeckAngle 02400C4C eases it toward the look target (limited to the ship angle while riding); procDamage 0241E150 sets it from the hit direction; procWait 024177A8 tests it for 0 (tww the same) |
| 0x69D8 | `m3588` | `mHangAttributeCode` | changeHangEndProc 02428230: the attribute code of the ground found when hanging (dBgS_Attr_UNK1B when none); setWaterRipple 0240AB88 starts the waterfall splash while hanging on code 0x17 (tww the same) |
| 0x6A24 | `m35CC` | `mGrabItemBounceY` | setGrabItemPos 0240A41C: -0.2 x speed.y per frame (at most 40), chased back to 0 and added to the held actor's y; freeGrabItem 023DCF8C clears it (tww the same) |
| 0x6A50 | `m35F8` | `mLadderTopY` | setFrontWallType 023E3850: the top y of the ladder triangle in front (wall codes 4 / 5; m3724 gets the bottom middle); changeFrontWallTypeProc 02418E00 keeps current.pos.y at most 150 below it (tww the same) |
| 0x6A54 | `m35FC` | `mSwordAtScale` | the reach of the sword attack capsules: setNormalCutAtParam 023E5B0C / setFinishCutAtParam 023E5678 / setExtraCutAtParam 023E6D00 / setExtraFinishCutAtParam 023E6A58 / setJumpCutAtParam 023ED380 set it from the HIO cut tables; setSwordAtCollision 02406E74 scales the root-to-top vectors by it (tww the same) |
| 0x6A78 | `m3620` | `mFrollCrashMtrlSndId` | TWW onFrollCrashFlg(): the material sound id of what Link rolled into; procFrontRoll 0241BAB8 sets it from mAcchCir[0], onFrollCrashFlg (in 02444F0C) from the caller; procFrontRollCrash_init 0241B904 plays JA_SE_LK_BODYATTACK with it |
| 0x6AA0 | `m3648` | `mRootJointQuat` | jointBeforeCB 023D6B30: during the guard / grab / ready-throw upper animations it keeps the root joint's quaternion (the root gets the identity) and concatenates it on the waist chain; playerInit 02410BE8 sets w 1 (tww the same root save) |
| 0x6AB0 | `m3658` | `mJointState` | the per-joint saved state PlayerJointState (d_a_player_main_01.h: 42 quaternions, 42 J3DTransformInfo): jointBeforeCB 023D6B30 saves a joint's quaternion before turning it, lk_saveTransInfo its transform before moving it; jointAfterCB 023D7564 restores them as mJointFlag says (tww m3658 / m3668 for one joint) |
| 0x68E2 | `m68E2` | `mJointFlag` | per joint (42): bit 1 quaternion saved, bit 2 transform saved in mJointState (jointBeforeCB 023D6B30 / lk_saveTransInfo set them); jointAfterCB 023D7564 restores and clears them (the HD per-joint form of tww's m34C6) |
| 0x72CC | `m36C4` | `mSwordRootPos` | execute 0240CDD0: the root of the held weapon (sword, Skull Hammer, Boko stick, Deku Leaf, else the CL_PODA joint), next to mSwordTopPos; setSwordAtCollision 02406E74 builds the attack capsules from it; setCollision 024076CC / setHammerWaterSplash 0240C0A8 (tww the same) |
| 0x72D8 | `m36D0` | `mOldSwordTopPos` | execute 0240CDD0 copies mSwordTopPos here before updating it; setSwordAtCollision 02406E74 sweeps from it (tww the same) |
| 0x72E4 | `m36DC` | `mOldSwordRootPos` | execute 0240CDD0 copies mSwordRootPos here before updating it; setSwordAtCollision 02406E74 (tww the same) |
| 0x7350 | `m3748` | `mExecStartPos` | execute 0240CDD0 stores current.pos before the proc runs; procGrabReady_init 023ED0F0 / procGrabUp_init 0242D8DC measure the held actor from it (tww the same) |

## daPy_lk_c: HD texture lists and texture animation controllers (d_a_player_main.h)

HD-only members in place of the GameCube mOtherLinktex and the texture animation pointers.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x458 | `m458` | `mLinktexList` | the list of Link's own texture headers that mpCurrLinktex points at (player04b::TextureOriginals: count, capacity, header pointers): the constructor 0240FE40 sets it up for 10 (lk_ct_listInit), playerInit 02410BE8 sets mpCurrLinktex to it and pushes the linktexS3TC headers; HD's form of what tww's mpCurrLinktex points at |
| 0x048C | `m48C` | `mOtherLinktex` | after TWW mOtherLinktex: the headers of the other outfit (HD keeps 10 of 0x24): playerInit 02410BE8 copies the originals here and swaps them with mLinktexList for the casual clothes (player04bCopyTextureList); playerDelete 0240EBF0 swaps them back (swapTexture) while the casual clothes are on |
| 0x0644 | `m644` | `mTexAnmCtrl` | the three HD texture animation controllers (0x58 each): draw 023D9820 attaches [0] / [1] (texture patterns) and [2] (texture scroll) to the model data; procSelfieHD 02415BA4 (d_a_player_main_04b) evaluates them at m3530 / m3532; frameBlend (d_a_player_main_07.h) indexes them |

## LinkPoseOldFrame (d_a_player_main_04.h), view of mDoExt_MtxCalcOldFrame

Open as in the class: field_0x1. The d_a_player_main_04.h view LinkPoseLook field_0x4 takes dAttLook_c's name mRequestPriority (round 3; d_attention.h Look names it now).

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x8 | `field_0x8` | `mOldFrameMorfStep` | view of mDoExt_MtxCalcOldFrame (m_Do/m_Do_ext_animation.h), whose names these are |
| 0x10 | `field_0x10` | `mOldFrameMorfRemain` | the same |
| 0x14 | `field_0x14` | `mOldFrameMorfRemainOld` | the same |

## daPy_lk_c: the HD members at 0x69E8 / 0x69EE (d_a_player_main.h)

After the split (0x69E9..0x69ED unused). The class takes the names of the player's own views (jumpCutState_c in d_a_player_main_02.h, LinkUpdateHeavyState in _04_update.h); the other views follow in the next commit (cc_playerView / StPlayerView mJumpCutExtraDamage, RdLinkState mStrongHitTimerActive, KsPlayerView mHeavyCount).

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x69E8 | `m69E8` | `mStrongJumpCut` | the name of the d_a_player_main_02.h view jumpCutState_c: procJumpCut_init 023ED380 / procJumpCut 02442C1C set it for a sword / Boko jump cut with /m3578/ > 0xF800 (a spun-up jump cut, not cursed); procJumpCutLand then goes on to the spin cut (procCutTurn_init) while it is set and clears it; the damage code (d_cc_uty 025192A8) applies a spin attack's damage twice and the Stalfos / ReDead hit timers (d_a_st damage_check, daRd_c::checkTgHit 0245C770) use 2 / 3 while it is set; 023ED5A4 clears it |
| 0x69EE | `m69EE` | `mHeavyStateCounter` | the name of the d_a_player_main_04_update.h view LinkUpdateHeavyState: each stuck morth (action_omoi in daKS_Execute) adds 1 per frame; execute 0240CDD0 turns daPyFlg0_HEAVY_STATE on at 5 or more, off otherwise, and clears it |

## daPy_lk_c: ripple, proc effect and fan wind callbacks; the TWW-named Wind Waker and equip members (d_a_player_main.h)

m3494 / m34CC / m3562 are offsets tww names (mTactZevEventName, mTactZevMelodyNum, mAnimeEquipItem); the WWHD uses agree. The view LinkAuraLayout (d_a_player_main_04_aura.h) follows for m3280.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x6694 | `m3280` | `mRippleEcallBack` | dPa_rippleEcallBack of the ripple ring ID_AK_JN_HAMON00 around Link: setWaterRipple 0240AB88 starts it (setShipTail) in shallow water, sets its rate and removes it; procFanGlide 02438E78 tests its emitter; playerDelete 0240EBF0 removes it (tww the same) |
| 0x66F8 | `m32E4` | `mProcEffectEcallBack0` | daPy_mtxFollowEcallBack_c the procs use for their own effect and commonProcInit 023DFDD8 ends on every proc change: procCutTurn_init 023E6380 (ID_AK_JN_ROUNDATTACKTOE), procCutTurnMove_init 02442190 (CHARGEPOWER00), dProcLavaDamage_init 023FE6B8 / dProcVorcanoFail_init 02424348 (HIDARUMAFIRE), dProcTalismanWait_init 02422950, procBottleOpen 0243DBFC, procFanGlide, the Wind Waker procs; offBodyEffect / onBodyEffect stop / play it (tww the same) |
| 0x6704 | `m32F0` | `mProcEffectEcallBack1` | the second proc effect slot, also ended by commonProcInit 023DFDD8: setBottleModel 023DF0FC / procBottleGet_init 0243E534 (ID_AK_JN_FORESTWATER00), procCutTurn_init 023E6380 (ROUNDATTACKSHIBUKI), procCutTurnMove_init 02442190 (CHARGEPOWER01); procCutTurn 02441750 / playerDelete end it (tww the same) |
| 0x67BC | `m33A8` | `mProcEffectEcallBack2` | the third proc effect slot (daPy_mtxPosFollowEcallBack_c), faded and ended by commonProcInit 023DFDD8: procBottleDrink 0243D838, procCutRollEnd_init 02441C38 (ID_IT_JN_PIYOPIYO00 on the head), procVomitJump_init 023E47D4 (tww the same) |
| 0x67FC | `m33E8` | `mFanWindEcallBack` | daPy_mtxFollowEcallBack_c of the Deku Leaf wind ID_AK_JN_UCHIWAWIND00: procFanSwing 024386C4 makes it on the YUCHW00 LWIND joint; setCollision 024076CC / playerDelete 0240EBF0 end it (tww the same) |
| 0x68AC | `m3494` | `mTactZevEventName` | TWW mTactZevEventName: setTactZev 02443768 stores the event name; procTactPlay 0243AC64 tests it, procTactPlayEnd_init 02412948 orders that event with the partner |
| 0x6911 | `m34CC` | `mTactZevMelodyNum` | TWW mTactZevMelodyNum: setTactZev 02443768 stores the melody number; procTactPlay_init 02439D54 / procTactPlay 0243AC64 compare the played song with it |
| 0x69B2 | `m3562` | `mAnimeEquipItem` | TWW mAnimeEquipItem: the item of the equip animation: setAnimeEquipItem 023EF750 / setAnimeEquipSword 023E9F60 / setAnimeUnequip 023ECEF0 set it; checkItemAction 023FB230 copies it into mEquipItem |

## daPy_py_c (d_a_player.h)

Views with the same name: TnPlayer (d_a_tn.h), daPy_py_l (d_a_npc_md.h). field_0x2BA[2] is padding.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x3CC | `field_0x2b0` | `mGrabItemOffsetY` | the held actor's height offset from the hand midpoint: daPy_lk_c::setGrabItemPos 0240A41C eases it (with the bounce) to 0 / -115 / -85 for a worn barrel and puts the held actor at the hands + it; freeGrabItem 023DCF8C clears it; TWW checkGrabWear() is it < 0, read by the Link procs (setStickData 023F32E4, changeDamageProc 023FA578, setCollision 024076CC ...) and by d_a_obj_search / d_a_tag_hint / d_a_tn (tww the same) |

## daPy_lk_c: stick input and spin input, ground code, fan wind, ship-ride angles, foot move, swim and whirlpool (d_a_player_main.h)

Open nearby: m3550 (procLargeDamage_init 023F54E8 reads it into current.angle.y; written nowhere in WWHD or tww), m34BA (setWindAtPower 023FC7F0: 1 when the wind hitting Link has At_Spl 1; the meaning of that wind kind is not shown).

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x6924 | `m34DC` | `mStickAngle` | setStickData 023F32E4: the control stick angle (CPad stick angle, + 0x8000 in the camera modes that need it); the rope, ship-attention, vomit and Wind Waker procs take directions from it; the cut inits copy it (tww the same) |
| 0x6932 | `m34EA` | `mOldStickAngle` | execute 0240CDD0 copies mStickAngle here; setStickData 023F32E4 turns the difference into the spin input (mStickSpinAngle); setSpeedAndAngleNormal 0241650C / checkNextMode test a backward flip (tww the same) |
| 0x6930 | `m34E8` | `mMoveTargetAngleY` | setStickData 023F32E4: mStickAngle plus the camera angle (the demo move angle in demos, 0 without input); the move / swim / attention speed functions turn current.angle.y and shape_angle.y toward it, getDirectionFromShapeAngle 023E4E18 / getDirectionFromCurrentAngle 023E3398 compare with it (tww the same) |
| 0x69C8 | `m3578` | `mStickSpinAngle` | setStickData 023F32E4: the summed stick rotation while it keeps turning one way (reset when it turns back or mStickSpinTimer runs out); more than 0xF800 either way is a spin input: checkNextActionFromButton 023EFF20, procBackJumpLand 0241C140, procJumpCut 02442C1C / setJumpCutAtParam 023ED380; changeCutProc 023E73FC / procCutTurn_init 023E6380 / procCutRoll_init 02441A44 clear it (tww the same) |
| 0x6974 | `m3524` | `mStickSpinTimer` | setStickData 023F32E4: set to the HIO mCutTurnR frame count whenever mStickSpinAngle grows, counted down otherwise; at 0 mStickSpinAngle is cleared (tww the same) |
| 0x68D8 | `m34BC` | `mBaseFootIdx` | posMoveFromFootPos 023FCB9C: the foot that stands lower (0 right, 1 left; 2 without old-frame data); Link is moved by that foot's movement (tww the same) |
| 0x690F | `m34CA` | `mOldItemButton` | setStickData 023F32E4 keeps the last mItemButton here before reading the buttons and puts it back in DEMO_KEEP (tww the same) |
| 0x6984 | `m3534` | `mFanWindTimer` | procFanSwing 024386C4 sets 10; setCollision 024076CC scales the fan wind capsule by it / 10 and counts it down (tww the same) |
| 0x6986 | `m3536` | `mFanWindAngleY` | procFanSwing 024386C4: shape_angle.y at the swing; setCollision 024076CC aims the fan wind capsule by it (tww the same) |
| 0x6988 | `m3538` | `mFanWindAngleX` | procFanSwing 024386C4: the downward angle of the swing (0 or toward the ground); setCollision 024076CC updates it from the ground under the wind and aims the capsule by it (tww the same) |
| 0x698A | `m353A` | `mFanWindSphTimer` | procFanSwing 024386C4 sets 3, procFanGlide 02438E78 5; setCollision 024076CC sets the fan wind sphere at Link while it counts down (tww the same) |
| 0x698C | `m353C` | `mShipRideAngleX` | setShipRidePos 023E287C: follows the ship's shape_angle.x by mShipRideAngleXSpeed; setWorldMatrix 023FF6A0 rotates Link by it on the ship; procShipReady_init 023ECA94 / dProcBossWarp_init 024248C0 clear it (tww the same) |
| 0x698E | `m353E` | `mShipRideAngleZ` | setShipRidePos 023E287C: follows the ship's shape_angle.z by mShipRideAngleZSpeed; setWorldMatrix 023FF6A0 rotates Link by it; procShipReady_init 023ECA94 / dProcBossWarp_init 024248C0 clear it (tww the same) |
| 0x69A4 | `m3554` | `mForestWaterUseTimer` | procBottleOpen 0243DBFC sets 3 when the Forest Water is used; setDemoData 023F22FC counts it down outside events and then clears daPyFlg1_FOREST_WATER_USE (tww the same) |
| 0x69D0 | `m3580` | `mGroundCode` | execute 0240CDD0 / makeBgWait 024131D0: GetGroundCode of the ground (-1 in the air); checkFallCode 023FEA6C, setNeckAngle 02400C4C, setBlendMoveAnime 023E14A0, posMoveFromFootPos 023FCB9C ... test codes 4 / 5 / 8 (tww the same) |
| 0x69CC | `m357C` | `mOldGroundCode` | execute 0240CDD0 / makeBgWait 024131D0 copy mGroundCode here before updating it; changeAutoJumpProc 023F81A4 tests 3 (tww the same) |
| 0x69F4 | `m359C` | `mFootMoveDist` | posMoveFromFootPos 023FCB9C: the XZ move of the base foot, smoothed (0.3 new + 0.7 old) while the stick is steady (tww the same) |
| 0x6A0C | `m35B4` | `mOldStickDistance` | execute 0240CDD0 copies mStickDistance here; posMoveFromFootPos 023FCB9C smooths mFootMoveDist only while the stick changed by less than 0.2 (tww the same) |
| 0x6A10 | `m35B8` | `mFootBgOffsetY` | footBgCheck 0240005C: eased (cLib_addCalc) to the lower foot's ground height below current.pos.y; added to the model base matrix y and taken off m37B4 (tww the same) |
| 0x6A60 | `m3608` | `mSwimTiredOffsetY` | setSwimTimerStartStop 023F8924 eases it toward a sink depth as the swim timer runs low (0 otherwise); setWorldMatrix 023FF6A0 adds it to the model y; commonProcInit 023DFDD8 clears it (tww the same) |
| 0x6A68 | `m3610` | `mWhirlPullSpeed` | posMove 023FDA70: chased toward 40 while a whirlpool (mWhirlId) pulls Link, who moves by it around the whirlpool; 0 otherwise (tww the same) |

## daPy_lk_c: animation root motion (d_a_player_main.h)

posMove 023FDA70 and jointBeforeCB 023D6B30.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x68DE | `m34C2` | `mRootMoveMode` | how the animation's root translation moves Link: the procs set a mode (0 .. 11) when they start their animation; posMove 023FDA70 switches on it (1 / 5 move current.pos by the root's change, 5 also in y; 2 / 6 keep the root; 3 / 7 and 4 restart into 1 / 5; 8 / 9 the other way round; 10 / 11 move the root by the position change); jointBeforeCB 023D6B30 holds the root in x / z (1) or at mRootTransY (5); commonProcInit 023DFDD8 turns 1 into 2 (tww the same) |
| 0x7308 | `m3700` | `mPrevRootTrans` | posMove 023FDA70: the root translation of the last frame, whose change moves Link in mRootMoveMode 1 / 5 / 8 / 9; the cut, crawl, auto-jump and door procs reset it when they start (tww the same) |
| 0x6A38 | `m35E0` | `mRootTransY` | jointBeforeCB 023D6B30 sets the root translation y to it in mRootMoveMode 5; posMove 023FDA70 takes it from the animation when mode 4 becomes 5; changeFrontWallTypeProc 02418E00 / procClimbDownStart_init set 43.67353 (tww the same) |

## daPy_lk_c: texture animation frames, outside push, inverse world matrix (d_a_player_main.h)

Open nearby: m3624 (a message number in the get-item / power-up / not-use procs, a counter in the Wind Waker procs: two roles), m3630 (the bait actor of procFoodThrow and an orderTalk id: not told apart), m35C4 (several y offsets: swim, steps, vomit).

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x6980 | `m3530` | `mTexAnmFrame` | the frame of the texture pattern (eyes / mouth) animation: playTextureAnime 023FBCEC advances it (blinks, demo frame); draw 023D9820 sets the pattern data frame from it; the talk / surprise / smile demo procs, procSelfieHD 02415BA4 and procShipScope 024327A4 set it (tww the same) |
| 0x6982 | `m3532` | `mTexScrollFrame` | the frame of the texture scroll animation: playTextureAnime 023FBCEC advances it; draw 023D9820 sets the scroll data frame from it; setTextureScrollResource 023DC43C and the surprise demo procs set it (tww the same) |
| 0x6A98 | `m3640` | `mOutPowerAngle` | the direction of the outside push: setOutPower 024438D4 sets it or combines a new push with the old one (atan2 of the summed vectors); posMove 023FDA70 moves Link by mOutPower along it (tww the same) |
| 0x6A9C | `m3644` | `mOutPower` | the strength of the outside push (TWW setOutPower()): setOutPower 024438D4 sets or sums it; posMove 023FDA70 moves current.pos by it along mOutPowerAngle and clears it; setNeckAngle 02400C4C / setBlendMoveAnime 023E14A0 test it (tww the same) |
| 0x73BC | `m37B4` | `mInvWorldMtx` | setWorldMatrix 023FF6A0: the inverse of Link's world matrix; footBgCheck 0240005C, setLegAngle 023FFBE0 and posMoveFromFootPos 023FCB9C bring joint matrices into Link's local space with it (tww the same) |

## daPy_lk_c: combo window, waist and ground angles, run lean, conveyor and wind (d_a_player_main.h)

mWindSpeed moves Link, mWindAnmSpeed (same wind, not cleared by the heavy state) only drives the animation and neck.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x6972 | `m3522` | `mCutComboTimer` | the window for the next combo cut: the cut inits (procCutA_init 023E5F88, procCutF_init 023E5BD0, ...) set it from the HIO cut tables, the finishing cuts clear it; execute 0240CDD0 counts it down and clears mCutComboCount at 0 (tww the same) |
| 0x6928 | `m34E0` | `mWaistAngleZ` | jointBeforeCB 023D6B30: the Z rotation of the waist joint; procFanGlide 02438E78 leans it (clamped to +-0x2000), setWaistAngle 023FFB08 sets it; footBgCheck 0240005C / posMoveFromFootPos 023FCB9C correct the feet by it; commonProcInit 023DFDD8 clears it (tww the same) |
| 0x692C | `m34E4` | `mWaistAngleY` | jointBeforeCB 023D6B30: the Y rotation of the waist joint; procFanGlide 02438E78 turns it (clamped to +-0x2000); commonProcInit 023DFDD8 clears it (tww the same) |
| 0x692A | `m34E2` | `mGroundAngle` | execute 0240CDD0: the slope of the ground along shape_angle.y (getGroundAngle, 0 in the air or on some grounds); setShapeAngleOnGround tilts shape_angle.x by it, setBlendMoveAnime 023E14A0 / checkNormalDamage 023F9D84 / setNeckAngle 02400C4C / procCrouch 0241AC94 read it (tww the same) |
| 0x696C | `m351C` | `mMoveSlantAngle` | setMoveSlantAngle 02400990: the sideways lean while running (eased, shape_angle.z = half of it); setGrabItemPos 0240A41C tilts the held actor by it, setNeckAngle 02400C4C reads it (tww the same) |
| 0x72C0 | `m36B8` | `mBeltConveyerSpeed` | setBeltConveyerPower 023FC568: eased toward the conveyor's push (0 off a conveyor); posMove 023FDA70 adds it to current.pos; changeAutoJumpProc 023F81A4 / setBlendMoveAnime / setNeckAngle read it (tww the same) |
| 0x7338 | `m3730` | `mWindSpeed` | setWindAtPower 023FC7F0 eases it toward the wind push (0 in the heavy state, events and demos); posMove 023FDA70 adds its x / z to current.pos; procFanGlide 02438E78 drives the glide with it (procFanGlide_init 0241A3C4 clears it) (tww the same) |
| 0x7344 | `m373C` | `mWindAnmSpeed` | setWindAtPower 023FC7F0 eases it toward the wind push like mWindSpeed but also in the heavy state (0 in events and demos); it does not move Link: setBlendMoveAnime 023E14A0 and setNeckAngle 02400C4C react to it (tww the same) |

## daPy_lk_c: fall start and slip inertia (d_a_player_main.h)

Open nearby: m35F4 (set with mFallStartY but only at proc starts, read by checkFallCode: the two are not told apart well enough), m355C / m355E / m3600 / m3604 (bow, hookshot, bottle, fan-glide and rope procs give each several roles).

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x7290 | `m3688` | `mProcStartPos` | commonProcInit 023DFDD8: current.pos when a proc starts (procHookshotFly 02437F00 / procFanGlide 02438E78 / procAutoJump_init 023F7958 / procRopeReady_init 023EB864 renew it); procSideStep 0241A604 / procBackJump 0241C094 / procFall 0241DD5C / changeLandProc 0241C898 measure how far Link dropped below its y (tww the same) |
| 0x6A48 | `m35F0` | `mFallStartY` | the height a fall is measured from: posMoveFromFootPos 023FCB9C keeps it at current.pos.y on the ground, commonProcInit 023DFDD8 / procHookshotFly / procFanGlide / procAutoJump_init set mProcStartPos.y; changeLandProc 0241C898, setFallVoice 0241D6CC, procJumpCutLand_init 02442990 and procBtVerticalJumpLand_init 02430858 use it - current.pos.y (tww the same) |
| 0x72A8 | `m36A0` | `mSlipSpeed` | posMoveFromFootPos 023FCB9C: the sliding inertia, built from the speed lost each frame (0.75 x (old - new speed)) and eased back to 0 (cleared off slippery ground); posMove 023FDA70 adds it to current.pos; checkIceSlipFall 024197A0 / procIceSlipFall_init 02419514 and the blend-move functions read it; procFanSwing 024386C4 pushes it back (tww the same) |
| 0x72B4 | `m36AC` | `mOldSlipSpeed` | posMoveFromFootPos 023FCB9C copies mSlipSpeed here before updating it; procWait 024177A8 and procIceSlipAlmostFall_init 024173F4 use it (tww the same) |

## daPy_lk_c: sound animation key (d_a_player_main.h)

Pairs with daPy_anmHeap_c::mDemoAnmNo.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x6940 | `m34F0` | `mSeAnmDemoAnmNo` | setSeAnime 023E07E4 copies the animation heap's mDemoAnmNo here next to mSeAnmIdx and keeps the loaded sound animation only while both match; resetSeAnime 023E07C0 / playerInit 02410BE8 set 0xFFFF (tww the same) |

# Field names: NPCs

Offset-named fields of the NPC actor classes (`daNpc_*`), named in the field naming pass (2026-11-10;
rules in docs/decomp-notes.md, "Field names (offset-named fields)"). A name comes from how the original code uses the
field (the address of the use and what it shows), from names already in this decomp, or from the CC0 zeldaret/tww and
zeldaret/tp decomps. Renames only: offsets, types and code are unchanged (objcmp: object-identical). Each header
declaration keeps the old name (`was field_0x...`) with the evidence. Fields without enough evidence keep their offset
name and are listed as open.

## daNpc_Ji1_c (d_a_npc_ji1.h; view KmonOrcaHD in d_a_kmon.h)

Old names are the GameCube offsets (zeldaret/tww). Functions are named after daNpc_Ji1_c members; setHairAngle is
inlined in _execute.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x3D4 | `field_0x2BC` | `mPrevAction` | setAction (inline, e.g. in normalAction 02250A1C) copies mAction here before switching; talkAction 0225268C / speakAction 02252FD0 / speakBadAction 02254A64 switch back to it |
| 0x3DC | `field_0x2C8` | `mEventReturnAction` | normalSubActionGuard 022504E4 (guard event) copies mAction here before eventAction; eventAction 022596CC switches back to it; evn_talk_init 022583A4 tests it |
| 0x42C | `field_0x320` | `mSmokePos` | battleSubActionTateAttack 0225B85C copies the spear tip (y 0) / YokoAttack 0225BBA4 sets it 200 around the body; position of the ID_AK_JT_ELEMENTSMOKE00 emitter (0225B720) |
| 0x438 | `field_0x32C` | `mSmokeAngle` | battleSubActionYokoAttack 0225BBA4: steps -0x1400 per frame, mSmokePos = pos + 200 * (sin, cos) of it |
| 0x658 | `field_0x430` | `mpTearEmitter` | setAnm 0224DFF0 sets dPa_ID_AK_SN_JITEAR00 at current.pos when NULL; setAnimFromMsgNo 02251A8C ends it (becomeInvalidEmitter) |
| 0x860 | `field_0x638` | `mStts` | CreateInit 0224C650: Init(0xFF, 0xFF, this), status of mCyl, mAtCyl and mAtCps; _execute 0224B268 moves by its m_cc_move (fopAcM_posMoveF) |
| 0x860 | `field_0x6B0` | `mCyl` | CreateInit 0224C650: Set(l_cyl_src), mStts; _execute 0224B268 SetC(current.pos) every frame |
| 0x860 | `field_0x910` | `mAtCyl` | CreateInit 0224C650: Set(l_cylAt_src); MoveToPlayer 0224F628 SetC(current.pos) and sets it at the attack's last frames |
| 0x860 | `field_0xA40` | `mAtCps` | CreateInit 0224C650: Set(l_cpsAt_src) from mSpearButtPos to mSpearTipPos; battleAtSet 0225B4AC sets radius and start/end (mSpearTipOldPos -> mSpearTipPos) |
| 0x89C | `field_0x674` | `mGuardStts` | CreateInit 0224C650: Init(0xFF, 0xFF, this), status of mGuardCyl |
| 0x89C | `field_0x7E0` | `mGuardCyl` | CreateInit 0224C650: Set(l_cyl2_src); battleSubActionNockBack 0225C208 OnTgShield/ChkTgShieldHit; setGuardParticle 02248D3C spark at its TG hit position |
| 0xC68 | `field_0xB78` | `mSpearTipPos` | set_mtx 0224AEF0: the spear joint matrix times (0, 0, 100) (70 for the side attack) |
| 0xC68 | `field_0xB84` | `mSpearButtPos` | set_mtx 0224AEF0: the spear joint matrix times (0, 0, -100); _delete 0224C550 deletes its sound object |
| 0xC68 | `field_0xB90` | `mSpearTipOldPos` | set_mtx 0224AEF0 copies mSpearTipPos here before updating it; battleAtSet 0225B4AC uses it as the capsule start |
| 0xDCA | `field_0xBA2` | `mArmShakeAmp` | battleSubActionNockBack 0225C208 sets t * 0x100; nodeCallBack3 022494C8 X-rotates both arm joints by it * sin(mArmShakePhase) |
| 0xDCA | `field_0xBA4` | `mArmShakePhase` | battleSubActionNockBack 0225C208 sets t * 0x2800; nodeCallBack3 022494C8: sin phase of the arm shake |
| 0xDCE | `field_0xBA6` | `mHeadPitch` | setHairAngle (inlined in _execute 0224B268): atan2 of the head front vector's y against its xz length |
| 0xDD0 | `field_0xBA8` | `mHeadYaw` | setHairAngle (inlined in _execute 0224B268): atan2(x, z) of the head front vector |
| 0xDD2 | `field_0xBAA` | `mHair1AngleZ` | setHairAngle (in _execute 0224B268) eases and clamps it (HIO limits); nodeCallBack2 02249224 Z-rotates the hair1 joint by it |
| 0xDD4 | `field_0xBAC` | `mHair1AngleY` | setHairAngle (in _execute 0224B268) eases and clamps it; nodeCallBack2 02249224 Y-rotates the hair1 joint by it |
| 0xDD6 | `field_0xBAE` | `mHair2AngleZ` | setHairAngle (in _execute 0224B268); nodeCallBack2 02249224 Z-rotates the hair2 joint by it |
| 0xDD8 | `field_0xBB0` | `mHair2AngleY` | setHairAngle (in _execute 0224B268); nodeCallBack2 02249224 Y-rotates the hair2 joint by it |
| 0xDDA | `field_0xBB2` | `mHair3AngleZ` | setHairAngle (in _execute 0224B268); nodeCallBack2 02249224 Z-rotates the hair3 joint by it |
| 0xDDC | `field_0xBB4` | `mHair3AngleY` | setHairAngle (in _execute 0224B268); nodeCallBack2 02249224 Y-rotates the hair3 joint by it |
| 0xDDE | `field_0xBB6` | `mHair1SpeedZ` | setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair1AngleZ, added next frame |
| 0xDE0 | `field_0xBB8` | `mHair1SpeedY` | setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair1AngleY, added next frame |
| 0xDE2 | `field_0xBBA` | `mHair2SpeedZ` | setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair2AngleZ, added next frame |
| 0xDE4 | `field_0xBBC` | `mHair2SpeedY` | setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair2AngleY, added next frame |
| 0xDE6 | `field_0xBBE` | `mHair3SpeedZ` | setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair3AngleZ, added next frame |
| 0xDE8 | `field_0xBC0` | `mHair3SpeedY` | setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair3AngleY, added next frame |
| 0xDEC | `field_0xBC4` | `mHair1LastPos` | setHairAngle (in _execute 0224B268): the hair1 joint position, kept for the next frame (its motion drives the hair) |
| 0xDF8 | `field_0xBD0` | `mHairWindPhase` | setHairAngle (in _execute 0224B268): advanced by 1500 + 4600 * wind each frame; cos phase of the wind sway |
| 0xDFA | `field_0xBD2` | `mHair1WindAngle` | setHairAngle (in _execute 0224B268): wind * 2280 * cos(mHairWindPhase); nodeCallBack2 02249224 adds it to the hair1 Z angle |
| 0xDFC | `field_0xBD4` | `mHair2WindAngle` | setHairAngle (in _execute 0224B268): wind * 3908 * cos(...); nodeCallBack2 02249224 adds it to the hair2 Z angle |
| 0xDFE | `field_0xBD6` | `mHair3WindAngle` | setHairAngle (in _execute 0224B268): wind * 7568 * cos(...); nodeCallBack2 02249224 adds it to the hair3 Z angle |
| 0xE00 | `field_0xBD8` | `mBackbonePos` | nodeCallBack1 02249084: the backbone joint matrix times l_hio's three offsets (HIO 0x0C0) |
| 0xE48 | `field_0xC24` | `mHitCutType` | daJi1_TgHitCallback 02248CF0 stores the player's cut type; teachAction 02255908 / teachSPRollCutAction 02256F1C / battleSubActionGuard 0225C7EC switch on it |
| 0xE4C | `field_0xC28` | `mCoHit` | daJi1_CoHitCallback 02248CA4 sets 1; _execute 0224B268 clears it every frame |
| 0xE50 | `field_0xC2C` | `mAttackEndWait` | MoveToPlayer 0224F628 sets 0x14 at the attack animation's end, then counts it down returning TRUE while > 0 |
| 0xE54 | `field_0xC30` | `mBattleTimer` | battleGameSetTimer 02259A6C sets a random time; battleSubActionWait 0225B344 counts it down and acts at < 0; battleAction 0225AEC8 sets 30 + rnd(150) |
| 0xE58 | `field_0xC34` | `mNockBackTimer` | the attack inits (0225B2B8, 0225B22C, 0225B1A0) clear it; battleSubActionNockBack 0225C208 counts it and scales the arm shake by it |
| 0xE60 | `field_0xC3C` | `mAtHitCount` | daJi1_AtHitCallback 02248EA0 adds 1; battleAction 0225AEC8 clears it and shows 3 - it as life; battleGuardCheck 0225A534 ends the game when > 2 |
| 0xE68 | `field_0xC40` | `mBgCorrectVec` | _execute 0224B268 copies the BG correction difference; calcBgCorrectValue 0224F59C dots it with the facing direction |
| 0xE74 | `field_0xC4C` | `mCoCorrectSum` | calcCoCorrectValue 0224F528 adds mStts' m_cc_move along the facing direction each call; cleared by CreateInit 0224C650 / MoveToPlayer 0224F628 |
| 0xE78 | `field_0xC50` | `mBgCorrectSum` | calcBgCorrectValue 0224F59C adds mBgCorrectVec along the facing direction each call; cleared by CreateInit 0224C650 / MoveToPlayer 0224F628 |
| 0xEA0 | `field_0xC78` | `mActionPhase` | setAction (inline) sets -1 before calling the old action, 0 for the new one; every action (e.g. battleAction 0225AEC8, eventAction 022596CC) inits at 0 and adds 1 |
| 0xEAC | `field_0xC84` | `mEventNo` | eventAction 022596CC orders mEventIdx[it], then sets 0x12 (none); _create 0224D280 sets 0x12; the guard event (022504E4) / kaitenwaitAction 02250D54 set the event to play |
| 0xEB0 | `field_0xC88` | `mLookMaxVel` | lookBack 0224ABA4 eases it to HIO 0x18 while m_jnt turns, else clears it; passed as lookAtTarget's maxVel (TWW d_npc.h parameter name) |
| 0xEB4 | `field_0xC8C` | `mTeachCutType` | teachAction 02255908 / teachSPRollCutAction 02256F1C copy mHitCutType here and test it (CUT_TYPE_CUT_ROLL) |
| 0xEC0 | `field_0xC98` | `mTeachHitsLeft` | CreateInit 0224C650 sets HIO 0x54[mTeachStep]; teachAction 02255908 counts it down per good cut, the lesson ends at 0 |
| 0xEC0 | `field_0xD70` | `mTeachStep` | CreateInit 0224C650 clears it and picks HIO 0x54[it]; teachAction 02255908 tests == 4; teachSPRollCutAction sets the message count number to it |
| 0xEC4 | `field_0xC9C` | `mBackSlideDist` | BackSlideInit 0224DE14 and every battle subaction init clear it; BackSlide 022525B4 eases it to the slide distance behind mBackSlideStartPos |
| 0xEC4 | `field_0xD38` | `mBackSlideStartPos` | BackSlideInit 0224DE14 and the subaction inits copy current.pos; BackSlide 022525B4 slides back from it |
| 0xEC8 | `field_0xCA0` | `mHarpoonMtx` | harpoonRelease 0224DE3C takes the held harpoon's matrix; harpoonMove 0224A2B8 concatenates it |
| 0xEF8 | `field_0xCD0` | `mHarpoonPos` | harpoonRelease 0224DE3C: translation of mHarpoonMtx; harpoonMove 0224A2B8 adds mHarpoonSpeed each frame |
| 0xEF8 | `field_0xCDC` | `mHarpoonSpeed` | harpoonRelease 0224DE3C copies the release velocity; harpoonMove 0224A2B8 adds it to mHarpoonPos and gravity (HIO) to y |
| 0xF1C | `field_0xCF4` | `mHarpoonRot` | harpoonRelease 0224DE3C sets the identity quaternion; harpoonMove 0224A2B8 multiplies it by mHarpoonRotStep and normalizes |
| 0xF1C | `field_0xD04` | `mHarpoonRotStep` | harpoonRelease 0224DE3C builds it from mHarpoonSwing and the facing; harpoonMove 0224A2B8 multiplies mHarpoonRot by it |
| 0xF2C | `field_0xD14` | `mHarpoonSwing` | harpoonRelease 0224DE3C sets 0x400 and builds mHarpoonRotStep from sin of it |
| 0xF3E | `field_0xD16` | `mRollAtAngle` | evn_RollAtControl_init 02258A14 sets the start angle + 0x2000; evn_RollAtControl 02258E6C adds 0x540 per frame, mRollAtPos = pos + mRollAtRadius * (sin, cos) |
| 0xF3E | `field_0xD18` | `mRollAtRadius` | evn_RollAtControl_init 02258A14 sets 400; evn_RollAtControl 02258E6C chases it to 0 |
| 0xF3E | `field_0xD1C` | `mRollAtPos` | evn_RollAtControl 02258E6C: position on the circle of mRollAtAngle / mRollAtRadius |
| 0xF50 | `field_0xD28` | `mInitPos` | CreateInit 0224C650 / kaitenAction 02251ED0 copy home.pos; initPos 02254390 puts current.pos and old.pos there |
| 0xF78 | `field_0xD50` | `mHeldItemOffset` | CreateInit 0224C650 sets (-13, -4, 10); _execute 0224B268 eases it to the item/normal aim offset; set_mtx 0224AEF0 translates by it |
| 0xF84 | `field_0xD5C` | `mHeldItemAngle` | CreateInit 0224C650 sets x 0x4000; _execute 0224B268 sets it to the aim angle; set_mtx 0224AEF0 rotates by it (XYZ) |
| 0xF94 | `field_0xD6C` | `mSubActionPhase` | every battle subaction (0225C208, 0225CA04, 0225C660, 0225C7EC, ...) starts at 0 and steps it; normalSubActionGuard 022504E4 sets 1 on a guard hit |
| 0xF9C | `field_0xD74` | `mGuardCount` | normalSubActionHarpoonGuard 0224FCC4 adds 1 per guarded hit; normalSubActionGuard 022504E4 plays the guard event at 3, 5 and > 5 (then 0) |
| 0xFA0 | `field_0xD78` | `mFirstMsgDone` | getMsg1stType 0224D8E0: returns 0x978 once and sets it, 0x97B after |
| 0xFAC | `field_0xD84` | `mHarpoonState` | _draw 02249638 / set_mtx 0224AEF0: Ji1Draw_HARPOON_HELD (1) / _RELEASED; harpoonRelease 0224DE3C and evn_harpoon_proc_init 0225896C set it |

Open (evidence not enough yet): `field_0xBC2` (no use), `field_0xC38` (set from event bits in teachSPRollCutAction,
value 2 picks other messages in evn_talk_init), `field_0xC94` (a lesson flag of
teachAction / speakAction; `field_0xC90` / `field_0xD34` are named in the round-2 section below), `field_0xD68` (a counter in evn_RollAtControl and the guard subactions), `field_0xD79`,
`field_0xD7A`, `field_0xD7B`, `field_0xD7C`, `field_0xD7E` (single-use flags of the message / item / look-back code).

### daNpc_Ji1_HIO_c (d_a_npc_ji1.h; Orca's tuning block l_HIO)

Old names are the tww offsets (field_0x..; tww has the same reads under those names). The HD block has no vtable at the start, so the WWHD offsets are 4 lower. Names from the WWHD reads, with the daNpc_Ji1_HIO_ct 0225CC44 values as support; the look-back limits follow the dNpc_HIO_c names they are passed as (m_jnt.setParam). Animation files from the setAnm 0224DFF0 resource cases.

| HIO offset | Old (tww name) | New | Evidence |
|---|---|---|---|
| 0x008 | `field_0x0C` | `mMaxHeadX` | lookBack 0224ABA4 passes it to m_jnt.setParam as the head's max X angle (and its negative as the min); daNpc_Ji1_HIO_ct 0225CC44 7000; same role as dNpc_HIO_c::mMaxHeadX |
| 0x00A | `field_0x0E` | `mMaxBackboneX` | lookBack 0224ABA4 passes it to m_jnt.setParam as the backbone's max X angle (and its negative as the min); daNpc_Ji1_HIO_ct 0225CC44 4000; same role as dNpc_HIO_c::mMaxBackboneX |
| 0x00C | `field_0x10` | `mMaxHeadY` | lookBack 0224ABA4 passes it to m_jnt.setParam as the head's max Y angle (its negative as the head and backbone min; 0x3E80 instead during BTKAMASI / BTKAMAE); chkAttention 0224AA24 adds it to mMaxBackboneY as the attention angle; daNpc_Ji1_HIO_ct 0225CC44 9000; same role as dNpc_HIO_c::mMaxHeadY |
| 0x00E | `field_0x12` | `mMaxBackboneY` | lookBack 0224ABA4 passes it to m_jnt.setParam as the backbone's max Y angle (0x1F40 instead during BTKAMASI / BTKAMAE); chkAttention 0224AA24 adds it to mMaxHeadY as the attention angle; daNpc_Ji1_HIO_ct 0225CC44 7000; same role as dNpc_HIO_c::mMaxBackboneY |
| 0x010 | `field_0x14` | `mMaxTurnStep` | lookBack 0224ABA4 passes it to m_jnt.setParam as the turn step; daNpc_Ji1_HIO_ct 0225CC44 5000; same role as dNpc_HIO_c::mMaxTurnStep |
| 0x012 | `field_0x16` | `mHeadTurnVelScale` | lookBack 0224ABA4: the scale of cLib_addCalcAngleS2 that eases mLookMaxVel toward mMaxHeadTurnVel while turning; daNpc_Ji1_HIO_ct 0225CC44 1 |
| 0x014 | `field_0x18` | `mMaxHeadTurnVel` | lookBack 0224ABA4: cLib_addCalcAngleS2 eases mLookMaxVel toward it while turning (else 0); daNpc_Ji1_HIO_ct 0225CC44 2000; same role as dNpc_HIO_c::mMaxHeadTurnVel |
| 0x028 | `field_0x2C` | `mTalkDistXZ` | normalAction 02250A1C / kaitenwaitAction 02250D54: Orca can be talked to (dEvtCnd_CANTALK_e) only while Link is within this XZ distance; daNpc_Ji1_HIO_ct 0225CC44 200.0 |
| 0x02C | `field_0x30` | `mbRepeatKaiten` | CreateInit 0224C650 starts kaitenExpAction when it is set even with event bit 0x0640; kaitenwaitAction 02250D54 reacts to Link's front-roll crash when it is set even with event bit 0x0501; daNpc_Ji1_HIO_ct 0225CC44 0 |
| 0x030 | `field_0x34` | `mJpGuardSlideDist` | battleSubActionJpGuard 0225C660 / teachAction 02255908 (animation 0xA, twice it on the last hit) / teachSPRollCutAction 02256F1C (twice it): cLib_addCalc2 target of mBackSlideDist, the jump-guard slide back; daNpc_Ji1_HIO_ct 0225CC44 150.0 |
| 0x034 | `field_0x38` | `mJpGuardSlideStep` | battleSubActionJpGuard 0225C660 / teachAction 02255908 / teachSPRollCutAction 02256F1C: cLib_addCalc2 max step of mBackSlideDist toward mJpGuardSlideDist; daNpc_Ji1_HIO_ct 0225CC44 20.0 |
| 0x038 | `field_0x3C` | `mGuardSlideDist` | teachAction 02255908: cLib_addCalc2 target of mBackSlideDist for the other guard animations; daNpc_Ji1_HIO_ct 0225CC44 90.0 |
| 0x03C | `field_0x40` | `mGuardSlideStep` | teachAction 02255908 / battleSubActionGuard 0225C7EC: cLib_addCalc2 max step of mBackSlideDist on a guard; daNpc_Ji1_HIO_ct 0225CC44 10.0 |
| 0x040 | `field_0x44` | `mInasiAnmSpeed` | setAnm 0224DFF0: the play speed of animation 7 (ji_inasi.bck); daNpc_Ji1_HIO_ct 0225CC44 2.0 |
| 0x044 | `field_0x48` | `mKrotAnmSpeed` | setAnm 0224DFF0: the play speed of animation 6 (ji_krot.bck); battleMove 02259EC4 / teachMove 0224EF14 / teachSpRollCutMove 0224F368 (scaled) and MoveToPlayer 0224F628 set the body's play speed from it; daNpc_Ji1_HIO_ct 0225CC44 2.0 |
| 0x048 | `field_0x4C` | `mWaraiAnmSpeed` | setAnm 0224DFF0: the play speed of animation 13 (ji_warai.bck); daNpc_Ji1_HIO_ct 0225CC44 1.25 |
| 0x050 | `field_0x54` | `mTeachHitCount` | CreateInit 0224C650 / speakAction 02252FD0 / startspeakAction 02253880 load mTeachHitsLeft with [mTeachStep] (or [0]); teachAction 02255908 counts the hits against it; daNpc_Ji1_HIO_ct 0225CC44 {9, 9, 9, 2, 2, 2} |
| 0x5C | `field_0x60` | `mLevelHits` | the battle-game hit thresholds: isClearRecord 0224D5BC / setClearRecord 0224D654 compare the hits with [level]; battleGameSetTimer 02259A6C picks the timer segment by them; createItem 02258574 / battleGuardCheck 0225A534 end the game at [3]; daNpc_Ji1_HIO_ct 0225CC44 {100, 300, 500, 1000} |
| 0x068 | `field_0x6C` | `mAttackAnmSpeed` | battleSubActionAttackInit 0225B2B8 / battleSubActionAttack 0225BF44: the attack animation's play speed up to frame 10; daNpc_Ji1_HIO_ct 0225CC44 0.7 (tww the same) |
| 0x06C | `field_0x70` | `mAttackSwingAnmSpeed` | battleSubActionAttack 0225BF44: the attack animation's play speed after frame 10; daNpc_Ji1_HIO_ct 0225CC44 1.5 (tww the same) |
| 0x070 | `field_0x74` | `mTateYokoSwingAnmSpeed` | battleSubActionTateAttack 0225B85C (after frame 26) / battleSubActionYokoAttack 0225BBA4 (after frame 25): the swing's play speed; daNpc_Ji1_HIO_ct 0225CC44 1.4 (tww the same) |
| 0x074 | `field_0x78` | `mTateAnmSpeed` | battleSubActionTateAttackInit 0225B22C sets it; battleSubActionTateAttack 0225B85C / battleSubActionYokoAttack 0225BBA4: the play speed before the swing; daNpc_Ji1_HIO_ct 0225CC44 1.4 (tww the same) |
| 0x07C | `field_0x80` | `mYokoAnmSpeed` | battleSubActionYokoAttackInit 0225B1A0: the side attack's starting play speed; daNpc_Ji1_HIO_ct 0225CC44 1.4 (tww the same) |
| 0x080 | `field_0x84` | `mNockBackAnmSpeed` | battleSubActionAttack 0225BF44 / TateAttack 0225B85C / YokoAttack 0225BBA4 / NockBack 0225C208: the body's play speed while mNockBackTimer runs; daNpc_Ji1_HIO_ct 0225CC44 0.1 |
| 0x084 | `field_0x88` | `mNockBackTime` | the attack sub-actions load mNockBackTimer with it when Link's shield blocks the attack (ChkTgShieldHit); daNpc_Ji1_HIO_ct 0225CC44 11 |
| 0x086 | `field_0x8A` | `mBattleTimerBase0` | battleGameSetTimer 02259A6C: mBattleTimer base at 0 hits (segment 0's start); daNpc_Ji1_HIO_ct 0225CC44 50 (tww the same) |
| 0x088 | `field_0x8C` | `mBattleTimerBase1` | battleGameSetTimer 02259A6C: mBattleTimer base at mLevelHits[0] (segment 1's start, also segment 3's); daNpc_Ji1_HIO_ct 0225CC44 40 (tww the same) |
| 0x08A | `field_0x8E` | `mBattleTimerBase2` | battleGameSetTimer 02259A6C: mBattleTimer base, segment 0's end and segment 2's start; daNpc_Ji1_HIO_ct 0225CC44 30 (tww the same) |
| 0x08C | `field_0x90` | `mBattleTimerBase3` | battleGameSetTimer 02259A6C: mBattleTimer base, segment 1's end; daNpc_Ji1_HIO_ct 0225CC44 20 (tww the same) |
| 0x08E | `field_0x92` | `mBattleTimerBase4` | battleGameSetTimer 02259A6C: mBattleTimer base, segment 2's end; daNpc_Ji1_HIO_ct 0225CC44 15 (tww the same) |
| 0x090 | `field_0x94` | `mBattleTimerBase5` | battleGameSetTimer 02259A6C: mBattleTimer base, segment 3's end and past mLevelHits[3]; daNpc_Ji1_HIO_ct 0225CC44 10 (tww the same) |
| 0x092 | `field_0x96` | `mBattleTimerRange0` | battleGameSetTimer 02259A6C: the random part (cM_rndF) of mBattleTimer at 0 hits (segment 0's start); daNpc_Ji1_HIO_ct 0225CC44 180 (tww the same) |
| 0x094 | `field_0x98` | `mBattleTimerRange1` | battleGameSetTimer 02259A6C: the random part of mBattleTimer, segment 1's start (also segment 3's); daNpc_Ji1_HIO_ct 0225CC44 140 (tww the same) |
| 0x096 | `field_0x9A` | `mBattleTimerRange2` | battleGameSetTimer 02259A6C: the random part of mBattleTimer, segment 0's end and segment 2's start; daNpc_Ji1_HIO_ct 0225CC44 100 (tww the same) |
| 0x098 | `field_0x9C` | `mBattleTimerRange3` | battleGameSetTimer 02259A6C: the random part of mBattleTimer, segment 1's end; daNpc_Ji1_HIO_ct 0225CC44 80 (tww the same) |
| 0x09A | `field_0x9E` | `mBattleTimerRange4` | battleGameSetTimer 02259A6C: the random part of mBattleTimer, segment 2's end; daNpc_Ji1_HIO_ct 0225CC44 60 (tww the same) |
| 0x09C | `field_0xA0` | `mBattleTimerRange5` | battleGameSetTimer 02259A6C: the random part of mBattleTimer, segment 3's end and past mLevelHits[3]; daNpc_Ji1_HIO_ct 0225CC44 50 (tww the same) |
| 0x0A0 | `field_0xA4` | `mBibiriAnmMorf` | setAnimFromMsgNo 02251A8C: the morf of animation 0x16 (ji_bibiri.bck); daNpc_Ji1_HIO_ct 0225CC44 6.0 |
| 0x0A4 | `field_0xA8` | `mOdorokuAnmMorf` | setAnimFromMsgNo 02251A8C: the morf of animation 0x15 (ji_odoroku.bck); daNpc_Ji1_HIO_ct 0225CC44 16.0 |
| 0x0A8 | `field_0xAC` | `mBibiriSlideDist` | talkAction 0225268C: BackSlide's target distance once the bibiri animation passes mBibiriSlideFrame; daNpc_Ji1_HIO_ct 0225CC44 70.0 |
| 0x0AC | `field_0xB0` | `mBibiriSlideStep` | talkAction 0225268C: BackSlide's max step during the bibiri animation; daNpc_Ji1_HIO_ct 0225CC44 10.0 |
| 0x0B0 | `field_0xB4` | `mBibiriSlideFrame` | talkAction 0225268C: the bibiri animation slides Orca back from this frame on; daNpc_Ji1_HIO_ct 0225CC44 1.0 |
| 0x0B8 | `field_0xBC` | `mHarpoonBounce` | harpoonMove 0224A2B8: the dropped harpoon's speed.y is multiplied by it when it hits the ground; daNpc_Ji1_HIO_ct 0225CC44 0.25 |
| 0x0BC | `field_0xC0` | `mHarpoonGravity` | harpoonMove 0224A2B8: added to the dropped harpoon's speed.y each frame; daNpc_Ji1_HIO_ct 0225CC44 -5.0 |
| 0xC0 | `field_0xC4` | `mBackboneOffsets` | nodeCallBack1 02249084: the backbone joint matrix times these three points gives mBackbonePos[0..2]; daNpc_Ji1_HIO_ct 0225CC44 (90, 5, 30), (90, 5, -30), (-20, -20, 0) |
| 0x0E4 | `field_0xE8` | `mHair1MinY` | _execute 0224B268 (daNpc_Ji1_setHairAngle): the lower limit of mHair1AngleY; daNpc_Ji1_HIO_ct 0225CC44 -0x3800 |
| 0x0E6 | `field_0xEA` | `mHair1MaxY` | _execute 0224B268 (daNpc_Ji1_setHairAngle): the upper limit of mHair1AngleY; daNpc_Ji1_HIO_ct 0225CC44 0x3800 |
| 0x0E8 | `field_0xEC` | `mHair2MinY` | _execute 0224B268 (daNpc_Ji1_setHairAngle): the lower limit of mHair1AngleY + mHair2AngleY; daNpc_Ji1_HIO_ct 0225CC44 -0x3A00 |
| 0x0EA | `field_0xEE` | `mHair2MaxY` | _execute 0224B268 (daNpc_Ji1_setHairAngle): the upper limit of mHair1AngleY + mHair2AngleY; daNpc_Ji1_HIO_ct 0225CC44 0x3A00 |
| 0x0EC | `field_0xF0` | `mHair3MinY` | _execute 0224B268 (daNpc_Ji1_setHairAngle): the lower limit of the three hair joints' Y angles together; daNpc_Ji1_HIO_ct 0225CC44 -0x3A00 |
| 0x0EE | `field_0xF2` | `mHair3MaxY` | _execute 0224B268 (daNpc_Ji1_setHairAngle): the upper limit of the three hair joints' Y angles together; daNpc_Ji1_HIO_ct 0225CC44 0x3A00 |
| 0x0F0 | `field_0xF4` | `mHair1MinZ` | _execute 0224B268 (daNpc_Ji1_setHairAngle): the lower limit of mHair1AngleZ; daNpc_Ji1_HIO_ct 0225CC44 -0x500 |
| 0x0F2 | `field_0xF6` | `mHair1MaxZ` | _execute 0224B268 (daNpc_Ji1_setHairAngle): the upper limit of mHair1AngleZ; daNpc_Ji1_HIO_ct 0225CC44 0x3800 |
| 0x0F4 | `field_0xF8` | `mHair2MinZ` | _execute 0224B268 (daNpc_Ji1_setHairAngle): the lower limit of mHair1AngleZ + mHair2AngleZ; daNpc_Ji1_HIO_ct 0225CC44 -0xA00 |
| 0x0F6 | `field_0xFA` | `mHair2MaxZ` | _execute 0224B268 (daNpc_Ji1_setHairAngle): the upper limit of mHair1AngleZ + mHair2AngleZ; daNpc_Ji1_HIO_ct 0225CC44 0x3A00 |
| 0x0F8 | `field_0xFC` | `mHair3MinZ` | _execute 0224B268 (daNpc_Ji1_setHairAngle): the lower limit of the three hair joints' Z angles together; daNpc_Ji1_HIO_ct 0225CC44 -0xF00 |
| 0x0FA | `field_0xFE` | `mHair3MaxZ` | _execute 0224B268 (daNpc_Ji1_setHairAngle): the upper limit of the three hair joints' Z angles together; daNpc_Ji1_HIO_ct 0225CC44 0x3A00 |

Open: the constructor sets them (or nothing does), but they are never read in WWHD or tww: `field_0x08`, `field_0x1A`, `field_0x1C`, `field_0x20`, `field_0x24`, `field_0x28`, `field_0x50`, `field_0x68`, `field_0x7C`, `field_0xA2`, `field_0xB8`.

## daNpcPeople_c (d_a_npc_people.h; copies in d_a_npc_people_event.h, _exec.h, _main.h, _msg.h, _talk.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x890 | `m71C` | `mUgTargetPos` | executeUgWalk 022C5434 / executeUgTurnInit 022C5AC4 set it (home.pos or current.pos + a direction times a distance, or a 50-unit side step); executeUgWalk walks toward it, executeUgTurnInit / executeUgTurn 022C5D4C turn to its angle; eventCameraStopInit 022C16A4 takes it as the camera's position unless UM3 looks at a star (tww the same) |
| 0x89C | `m728` | `mpAnmTbl` | setAnmTbl 022BED70 stores the table (NULL to stop); playAnm 022C2D6C steps to the next entry and plays its animation |
| 0x8A4 | `m730` | `mpTalkPartner` | eventUb1TalkInit 022C1228 sets this / NULL; eventUb1TalkXyInit 022C1384 sets peopleSearchFromName("Ub2"); the Ub talk events talk through it |
| 0x8A8 | `m734` | `mpMsgList` | getMsg3 022C1F20 points it at a message number list (l_msg_look_full_moon, ...); eventMesSetInit 022C0BF8 / next_msgStatus 022C6108 step through it |
| 0x8AC | `m738` | `mpUbMsg` | eventUb1TalkInit 022C1228 sets *mpUbMsgList and passes it to setMessageUb; next_msgStatus 022C6108 steps it |
| 0x8AC | `m73C` | `mpUbMsgList` | eventUb1TalkInit 022C1228 / eventUb1TalkXyInit 022C1384 select a l_msg_ub1_* / l_msg_xy_ub1_* list of sUbMsgDat pointers |
| 0x8B4 | `m740` | `mTargetSpeed` | _execute 022C305C chases speedF to it (cLib_chaseF); the walk / warp / event code sets it (0 to stop) |
| 0x8B8 | `m744` | `mMorf` | setAnm 022BEB7C uses it as the morf frames when not negative; setAnmTbl 022BED70 sets 16 / 13 |
| 0x8BC | `m748` | `mMaxAttnDistXZ` | chkAttention 022C0084: the player is in attention while closer than it (+40 while mInAttention); createInit 022BF510 / chkEndEvent 022C0840 / executeLook 022C511C copy npc dat 0x28, setMessageUb 022C1168 / eventAreaMaxInit 022C167C / eventUb2Talk 022C2380 set 1000 (tww the same); same role as daNpcRoten_c NpcDatStruct::mMaxAttnDistXZ |
| 0x8C0 | `m74C` | `mCylR` | createInit 022BF510 sets npc dat 0x3C and passes it to setCollision as the radius; eventCoCylRInit 022C17FC sets the event's radius |
| 0x8C4 | `m750` | `mAnmSpeed` | setAnmTbl 022BED70 sets 1 / -1; playAnm 022C2D6C passes it as setAnm's play speed |
| 0x8D0 | `m75C` | `mGetItemNo` | next_msgStatus 022C6108 sets l_item_id_sa3[...] or 4; eventGetItemInit 022C0FC8 gives that item |
| 0x8D4 | `m760` | `mAnmPlayMode` | setAnm 022BEB7C stores the play mode it passes to mpMorf->setAnm (setAnmTbl 022BED70: 0 for counted entries, 2 for looping ones); setAnmTbl restarts an unchanged animation only while it is 0 (tww the same); same code as daNpcRoten_c::mAnmPlayMode |
| 0x8D8 | `m764` | `mLookHeadOnly` | lookBack 022C2E50 passes it to lookAtTarget as the head-only flag (d_npc.h lookAtTarget headOnly) unless talking with mBodyTurn; chkAttention 022C0084 sets it (0 = turn the body), the walk / turn / furue / Ug modes set or clear it (tww the same); same code as daNpcRoten_c::mLookHeadOnly |
| 0x8DA | `m766` | `mEventIdx` | createInit 022BF510 fills it with people_getEventIdx("UO1_GET_ITEM", ...) or -1; XyEventCB 022BF220 / eventOrder 022C2A04 order [i] |
| 0x8E2 | `m76E` | `mWaitTimer` | executeWaitInit 022C3D88 sets npc dat 0x50 + random; executeWait 022C4074 counts it down before walking the path |
| 0x8E4 | `m770` | `mHomeTurnDelay` | chkAttention 022C0084 sets npc dat 0x58 when the player leaves the attention range and, without a path, counts it down before turning back to mTargetAngleY (tww the same); same code as daNpcRoten_c::mHomeTurnDelay |
| 0x8E6 | `m772` | `mReactTimer` | executeBikkuriInit 022C4B30 sets 0x15, executeFurueInit 022C4BF4 0x96; executeBikkuri / executeFurue count it down |
| 0x8E8 | `m774` | `mFurueTimer` | executeFurueInit 022C4BF4 sets 10 + rnd(20); executeFurue 022C4CA8 counts it down |
| 0x8EA | `m776` | `mMaxAttnAngleY` | chkAttention 022C0084: the angle of the outer check, which only sets mCanTalk (+0x71C while mInAttention); createInit 022BF510 / chkEndEvent 022C0840 copy npc dat 0x34, the Ub talk events 0x7FFF; checkOrder 022C045C copies it into mMaxLookAngleY when the talk starts (tww the same); same role as daNpcRoten_c NpcDatStruct::mMaxAttnAngleY |
| 0x8EC | `m778` | `mMaxLookAngleY` | chkAttention 022C0084: the player is looked at (mInAttention, mLookMode 1) while the angle to them is below it (+0x71C while mInAttention); createInit 022BF510 / chkEndEvent 022C0840 / eventOrder 022C2A04 copy npc dat 0x36, checkOrder 022C045C mMaxAttnAngleY at the talk start, the Ub talk events 0x7FFF (tww the same); daNpcRoten_c uses 0x4000 / 0x7FFF while talking in its place |
| 0x8EE | `m77A` | `mTargetAngleY` | eventTurnToPlayerInit 022C1094 sets the XZ angle to the player; eventTurnToPlayer 022C21D8 ends when current.angle.y equals it |
| 0x8F0 | `m77C` | `mMsgStatus` | talk3 022C1FD8 keeps the message status; talk2 022C1BBC resets 0xFFFF and tests MSG_CONTINUES / BOX_OPENING |
| 0x8F2 | `m77E` | `mEventFlags` | bit field: next_msgStatus 022C6108 sets 0x1..0x40 for message-list codes 1-5 / 7 / 9-0xA; when the box closes executeTalk 022C4404 orders the matching event (mOrderEventNum 3 / 7 / 8 / 9 / 0xC / 0xD / 0xF) and clears the bit, else sets 0x8000; chkEndEvent 022C0840 sets 0x8000 when the NPC's event ends; eventOrder 022C2A04 clears 0x8000 and resets the npc-data look state and the event (tww the same) |
| 0x8F4 | `m780` | `mBlinkTimer` | initTexPatternAnm 022BE1E8 clears it; playTexPatternAnm 022C2CC0 counts it down and restarts the head texture pattern with 0x78 |
| 0x8F6 | `m782` | `mLookMaxVelTarget` | chkAttention 022C0084 / executeWalk 022C46D0 set npc dat 0x38 / 0x3A; lookBack 022C2E50 eases mLookMaxVel toward it (the turn speed limit) |
| 0x8F6 | `m784` | `mLookMaxVel` | lookBack 022C2E50 eases it toward mLookMaxVelTarget (cLib_addCalcAngleS2 4, 0x800), else clears it, and passes it to lookAtTarget as maxVel; createInit 022BF510 clears it |
| 0x8FA | `m786` | `mLookAngleY` | lookBack 022C2E50: the target angle in look mode 2; chkAttention 022C0084 sets mTargetAngleY, executeWalk 022C46D0 / executeTurn 022C4A04 / executeUgWalk 022C5434 / executeUgTurn 022C5D4C the path angle, executeFurue 022C4CA8 +-0x4000 (tww the same); same code as daNpcRoten_c::mLookAngleY |
| 0x8FD | `m789` | `mInAttention` | chkAttention 022C0084: 1 once the player is in the look range (distance +40 and angles +0x71C while set), 0 when they leave; executeWait 022C4074 / executeWalk 022C46D0 / executeKyoro 022C4E98 / checkPig 022C3E78 / setWaitAnm 022BEEDC test it (tww the same); same code as daNpcRoten_c::mInAttention |
| 0x8FE | `m78A` | `mInLookDist` | chkAttention 022C0084: cleared each frame, 1 while the player is outside the attention range but closer than npc dat mMaxLookDistXZ (he still looks at them); executeWait 022C4074 / executeWalk 022C46D0 / executeKyoro 022C4E98 / checkPig 022C3E78 treat it like mInAttention (tww the same) |
| 0x8FF | `m78B` | `mCanTalk` | chkAttention 022C0084: 1 while the player is in the talk range (or the event attention is on), else 0; executeCommon 022C3AC4 sets mOrderEventNum 1 (can talk) while it is set (tww the same); same code as daNpcRoten_c::mCanTalk |
| 0x902 | `m78E` | `mBtpFrame` | initTexPatternAnm 022BE1E8 clears it; playTexPatternAnm 022C2CC0 advances and wraps it; _draw 022C36D4 passes it to mDoExt_btpAnm_entry |
| 0x903 | `m78F` | `mExecuteMode` | _execute 022C305C calls moveProc[it]; executeCommon 022C3AC4 / chkSurprise 022C39F0 test it |
| 0x906 | `m792` | `mSa3ItemIdx` | next_msgStatus 022C6108: message-list code 6 stores the index that follows; it picks the event bit (l_item_chk_sa3[it]) and the item (l_item_id_sa3[it]) given (tww the same) |
| 0x907 | `m793` | `mAnmIdx` | setAnm 022BEB7C stores the animation index; createHeap 022BE334 loads the bck by it; setAnmTbl 022BED70 / setWaitAnm 022BEEDC / _execute 022C305C / getMsg 022C6C68 test it (tww the same); same code as daNpcRoten_c::mAnmIdx |
| 0x908 | `m794` | `mLastRandNo` | _execute 022C305C draws a random number again when it equals this one, then keeps it here |
| 0x90A | `m796` | `mAnmLoopCount` | setAnmTbl 022BED70 sets the table entry's count (sPeopleAnmDat +0x02); playAnm 022C2D6C counts it down before the next entry |
| 0x90B | `m797` | `mCutNo` | privateCut 022C25E8 stores the event cut index and switches on it |
| 0x90D | `m799` | `mLookMode` | lookBack 022C2E50 switches on it: 1 looks at mLookAtPos, 2 turns to mLookAngleY, 0 neither; chkAttention 022C0084 and the walk / turn / look / Ug modes set it (tww the same); same code as daNpcRoten_c::mLookMode |
| 0x90E | `m79A` | `mSurpriseFlags` | bit 0x1: executeBikkuriInit 022C4B30 sets it, executeFurue 022C4CA8 clears it when the shake ends, getMsg 022C6C68 clears it and picks l_msg_uo2_surprise for UO2; bit 0x2: executeFurue toggles it at random times and turns the look +0x4000 / -0x4000 by it (tww the same) |
| 0x910 | `m79C` | `mModeInTalk` | _execute 022C305C runs the mode (not eventMove) during a talk only while set; executeCommon 022C3AC4 switches to the talk mode and returns 1 while talking with it set; executeTalkInit 022C43A8 tests it; XyEventCB 022BF220 / checkOrder 022C045C / executeCommon / executeTalk 022C4404 / createInit 022BF510 set or clear it, the constructor 022BE7CC sets 1 (tww the same); same role as daNpcPhoto_c::mModeInTalk |
| 0x911 | `m79D` | `mBodyTurn` | checkOrder 022C045C (talk start) copies npc dat 0x5B, createInit 022BF510 / eventOrder 022C2A04 0x5A, the Ug look modes set 1; while set, chkAttention 022C0084 and lookBack 022C2E50 (when talking) turn the body (mLookHeadOnly 0) (tww the same); same code as daNpcRoten_c::mBodyTurn |
| 0x912 | `m79E` | `mLookAtPlayer` | createInit 022BF510 / eventOrder 022C2A04 copy npc dat 0x5C, checkOrder 022C045C sets 1 at the talk start; chkAttention 022C0084: while 0 the player in range only turns him to mTargetAngleY instead of being looked at (tww the same); same role as daNpcPhoto_c::mLookAtPlayer |
| 0x915 | `m7A1` | `mCanMove` | createInit 022BF510 sets 1 for the path walkers and UG1; _execute 022C305C chases the walk speed and moves the actor (posMoveF, CrrPos) only while set; the constructor 022BE7CC clears it (tww the same) |
| 0x916 | `m7A2` | `mPartnerAnmTag` | setMessageUb 022C1168 stores the Ub message's partner animation tag (sUbMsgDat +0x05); eventUb2Talk 022C2380: the partner Ub plays it (setAnmFromMsgTagUb) once, then sets 0xFF (tww the same) |
| 0x917 | `m7A3` | `mHandRJntNo` | createHeap 022BE334: the "handR" joint index of the body model; _draw 022C36D4 puts mpEtcModel on that joint |
| 0x91A | `m7A6` | `mPsoNo` | _draw 022C36D4: l_pso_tbl()[it] unless 0xFF; _execute 022C305C sets 0xFF, executeFurue / executeLetter / executeLook set it |
| 0x91B | `m7A7` | `mSkyLookNo` | executeWait 022C4074: the sky object UM3's telescope look found, 0 the moon, 1 Orion, 2 the Big Dipper (dKy_moon / orion / hokuto_look_chk); getMsg3 022C1F20 picks the message list and eventCameraStopInit 022C16A4 the camera position by it (tww the same) |
| 0x91C | `m7A8` | `mMsgChecked` | talk2 022C1BBC: 1 once chkMsg has run for the shown message (typing after a continue or box open, or close wait); MSG_DISPLAYED and CLOSE_WAIT call chkMsg only while 0; a new message and MSG_DISPLAYED reset it (tww the same) |

Open: `m704`, `m754`, `m765`, `m798`, `m7A4`, `m7A5` (no use); `m79F` (npc dat copies, never read), `m79F` aside, `m7A0` is named in round 3: `mPartnerFacePlayer` (setMessageUb 022C1168 keeps the Ub message's mFacePlayer there; eventUb2Talk 022C2380 copies it into the partner's mBodyTurn and mLookAtPlayer), in all six copies.

### daNpcPeople_c__l_npc_dat (the npc data block, mpNpcDat; declared in the six people headers)

Old names are the GameCube offsets (zeldaret/tww `field_0x...`, unnamed there).

| Offset | Old | New | Evidence |
|---|---|---|---|
| 0x00 | `field_0x00` | `mMaxHeadX` | createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's maxHeadX (5th argument; d_npc.h setParam order) (tww the same) |
| 0x02 | `field_0x02` | `mMaxHeadY` | createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's maxHeadY (6th argument) (tww the same) |
| 0x04 | `field_0x04` | `mMaxBackboneX` | createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's maxBackboneX (1st argument) (tww the same) |
| 0x06 | `field_0x06` | `mMaxBackboneY` | createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's maxBackboneY (2nd argument) (tww the same) |
| 0x08 | `field_0x08` | `mMinHeadX` | createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's minHeadX (7th argument) (tww the same) |
| 0x0A | `field_0x0A` | `mMinHeadY` | createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's minHeadY (8th argument) (tww the same) |
| 0x0C | `field_0x0C` | `mMinBackboneX` | createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's minBackboneX (3rd argument) (tww the same) |
| 0x0E | `field_0x0E` | `mMinBackboneY` | createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's minBackboneY (4th argument) (tww the same) |
| 0x10 | `field_0x10` | `mMaxTurnStep` | createInit 022BF510 / executeWaitInit 022C3D88: m_jnt.setParam's maxTurnStep (9th argument) (tww the same) |
| 0x14 | `field_0x14` | `mPlayerEyeOffsetY` | chkAttention 022C0084: passed to dNpc_playerEyePos, whose look target is the player's head top + (0, it, 0) (tww the same); same role as daNpcRoten_c NpcDatStruct::mPlayerEyeOffsetY |
| 0x18 | `field_0x18` | `mAttnOffsetX` | _execute 022C305C: attention_info.position = current.pos + the vector (it, mAttnOffsetY, mAttnOffsetZ) Y-rotated by current.angle.y (tww the same) |
| 0x1C | `field_0x1C` | `mAttnOffsetY` | _execute 022C305C: the Y of the attention position offset (see mAttnOffsetX) (tww the same) |
| 0x20 | `field_0x20` | `mAttnOffsetZ` | _execute 022C305C: the Z of the attention position offset (see mAttnOffsetX) (tww the same) |
| 0x24 | `field_0x24` | `mEyeYOffset` | _execute 022C305C: eyePos = current.pos + (0, it, 0) (tww the same); same role as daNpcRoten_c NpcDatStruct::mEyeYOffset |
| 0x28 | `field_0x28` | `mMaxAttnDistXZ` | createInit 022BF510 / chkEndEvent 022C0840 / setMessageUb 022C1168 / eventUb2Talk 022C2380 / executeLook 022C511C / executeLook2 022C5268 / initUgSearchArea 022C077C copy it into daNpcPeople_c::mMaxAttnDistXZ; executeWait 022C4074 reads it (tww the same) |
| 0x2C | `field_0x2C` | `mMaxLookDistXZ` | chkAttention 022C0084: outside the attention range but closer than it, the player is still looked at (mLookMode 1, mInLookDist set) (tww the same) |
| 0x30 | `field_0x30` | `mCullFarDist` | createInit 022BF510: fopAcM_setCullSizeFar(it / the clipper's far) (tww the same) |
| 0x34 | `field_0x34` | `mMaxAttnAngleY` | createInit 022BF510 / chkEndEvent 022C0840 / setMessageUb 022C1168 / eventUb2Talk 022C2380 / initUgSearchArea 022C077C copy it into daNpcPeople_c::mMaxAttnAngleY (tww the same) |
| 0x36 | `field_0x36` | `mMaxLookAngleY` | createInit 022BF510 / chkEndEvent 022C0840 / setMessageUb 022C1168 / eventUb2Talk 022C2380 / eventOrder 022C2A04 / initUgSearchArea 022C077C copy it into daNpcPeople_c::mMaxLookAngleY (tww the same) |
| 0x38 | `field_0x38` | `mMaxHeadTurnVel` | chkAttention 022C0084: mLookMaxVelTarget, the speed lookBack eases the look toward (tww the same); same role as daNpcRoten_c NpcDatStruct::mMaxHeadTurnVel |
| 0x3A | `field_0x3A` | `mWalkHeadTurnVel` | executeWalk 022C46D0: mLookMaxVelTarget while walking the path; executeUgWalk 022C5434 scales it by mTargetSpeed / mWalkSpeed (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkHeadTurnVel |
| 0x3C | `field_0x3C` | `mCylRadius` | createInit 022BF510 / eventCoCylRInit 022C17FC copy it into mCylR, the body cylinder's radius (tww the same); same role as daNpcRoten_c NpcDatStruct::mCylRadius |
| 0x40 | `field_0x40` | `mCylHeight` | createInit 022BF510 / _execute 022C305C: setCollision(&mCyl, .., mCylR, it), the body cylinder's height (tww the same) |
| 0x44 | `field_0x44` | `mWalkAnmRate` | _execute 022C305C: while walking (mAnmIdx 2) the morf's play speed is speedF * it, at least 0.5 (tww the same) |
| 0x48 | `field_0x48` | `mWalkSpeed` | executeWalk 022C46D0: mTargetSpeed while walking the path; executeUgWalk 022C5434 divides by it (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkSpeed |
| 0x50 | `field_0x50` | `mWaitTimeMin` | executeWaitInit 022C3D88: mWaitTimer = it + rnd(mWaitTimeMax - it) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWaitTimeMin |
| 0x52 | `field_0x52` | `mWaitTimeMax` | executeWaitInit 022C3D88: mWaitTimer = mWaitTimeMin + rnd(it - mWaitTimeMin) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWaitTimeMax |
| 0x54 | `field_0x54` | `mWalkTimeMin` | executeWalkInit 022C4600 / executeTurnInit 022C48B8: mWaitTimer = it + rnd(mWalkTimeMax - it) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkTimeMin |
| 0x56 | `field_0x56` | `mWalkTimeMax` | executeWalkInit 022C4600 / executeTurnInit 022C48B8: mWaitTimer = mWalkTimeMin + rnd(it - mWalkTimeMin) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkTimeMax |
| 0x58 | `field_0x58` | `mHomeTurnDelay` | chkAttention 022C0084 copies it into daNpcPeople_c::mHomeTurnDelay when the player leaves the attention range (tww the same) |
| 0x5A | `field_0x5A` | `mBodyTurn` | createInit 022BF510 / eventOrder 022C2A04 copy it into daNpcPeople_c::mBodyTurn (tww the same) |
| 0x5B | `field_0x5B` | `mTalkBodyTurn` | checkOrder 022C045C copies it into daNpcPeople_c::mBodyTurn when the talk starts (tww the same) |
| 0x5C | `field_0x5C` | `mLookAtPlayer` | createInit 022BF510 / eventOrder 022C2A04 copy it into daNpcPeople_c::mLookAtPlayer (eventOrder also into m79F) (tww the same) |

Open: `field_0x12`, `field_0x4C`, `field_0x4E`, `field_0x5E`, `field_0x5F` (no use), `field_0x5D` (createInit 022BF510 copies it into m79F, which is never read).

### sPeopleAnmDat, sUbMsgDat, PsoData_l (people tables; the first two declared in the six people headers, PsoData_l in d_a_npc_people_main.h)

Old names are the GameCube offsets (zeldaret/tww `field_0x...`, unnamed there).

| Struct | Offset | Old | New | Evidence |
|---|---|---|---|---|
| sPeopleAnmDat | 0x00 | `field_0x00` | `mAnmIdx` | setAnmTbl 022BED70: the entry's animation index for setAnm (0xFF ends the table), compared with daNpcPeople_c::mAnmIdx; playAnm 022C2D6C replays it (tww the same); same code as sRotenAnmDat::mAnmIdx |
| sPeopleAnmDat | 0x01 | `field_0x01` | `mMorf` | setAnmTbl 022BED70: the entry's morf frames, passed to setAnm (tww the same); same code as sRotenAnmDat::mMorf |
| sPeopleAnmDat | 0x02 | `field_0x02` | `mLoopCount` | setAnmTbl 022BED70: the entry's count, copied into mAnmLoopCount; 0 loops, negative plays backwards (tww the same); same code as sRotenAnmDat::mLoopCount |
| sUbMsgDat | 0x00 | `field_0x00` | `mMsgNo` | setMessageUb 022C1168 passes it to setMessage; next_msgStatus 022C6108 steps mpUbMsg to the next entry and ends the list at 0 (tww the same) |
| sUbMsgDat | 0x04 | `field_0x04` | `mAnmTag` | setMessageUb 022C1168: the speaker's animation tag, passed to setAnmFromMsgTagUb (tww the same) |
| sUbMsgDat | 0x05 | `field_0x05` | `mPartnerAnmTag` | setMessageUb 022C1168 copies it into daNpcPeople_c::mPartnerAnmTag, which the partner Ub plays in eventUb2Talk 022C2380 (tww the same) |
| PsoData_l | 0x00..0x08 | `field_0x00` / `04` / `08` | `mOffsetX` / `mOffsetY` / `mOffsetZ` | _draw 022C36D4: the snap object's center = current.pos + (them) (tww the same) |
| PsoData_l | 0x0C | `field_0x0C` | `mRadius` | _draw 022C36D4: dSnap_Obj_SetGeo's radius (tww d_snap.h SetGeo(center, radius, height, ..)) |
| PsoData_l | 0x10 | `field_0x10` | `mHeight` | _draw 022C36D4: dSnap_Obj_SetGeo's height |
| PsoData_l | 0x14 | `field_0x14` | `mAngleY` | _draw 022C36D4: + current.angle.y, dSnap_Obj_SetGeo's angle (tww the same) |

Round 3: sUbMsgDat `field_0x06` -> `mFacePlayer` (setMessageUb 022C1168 copies it into mBodyTurn and mLookAtPlayer, widens the attention ranges when 1, keeps it in mPartnerFacePlayer). Open: sUbMsgDat `field_0x07` (no use); PsoData_l `field_0x16`, `field_0x17` (dSnap_Obj_SetInf's two unnamed u8 arguments; tww d_snap.h SetInf(u8, const fopAc_ac_c*, u8, u8, s16) leaves them unnamed).

## daNpc_Zl1_c (d_a_npc_zl1.h; copies in d_a_npc_zl1_heap.h, _main.h, and the partial copy in _action.h)

Old names are the GameCube offsets (zeldaret/tww `field_0x...`).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x7EC | `field_0x6D4` | `mpEyeMatAnm` | bodyCreateHeap sets the two eye material animations; setEyeCtrl 023041F8 / clrEyeCtrl 02304230 / eye_ctrl 02304310 drive [i] |
| 0x970 | `field_0x72C` | `mInitialPos` | _execute 02307D18 copies current.pos on the first run (while mbRanExecute is 0); event_proc 023071C4 copies it again (tww the same); written only, but the same code as daNpc_Hi1_c::mInitialPos (with mInitialAngle) |
| 0x97C | `field_0x738` | `mInitialAngle` | _execute 02307D18 copies current.angle on the first run, with mInitialPos, then mbRanExecute = 1; event_proc 023071C4 copies it again; wait_1 02309394 turns current.angle.y back to its y (type 4) (tww the same); same code as daNpc_Hi1_c::mInitialAngle |
| 0x982 | `field_0x73E` | `mMtxAngle` | setMtx 02304C98 rotates the model matrix by it (mDoMtx_ZXYrotM) when she is not on the branch or ship; _execute 02307D18 copies current.angle into it each frame; createInit 0230503C sets it and shape_angle from current.angle (tww the same) |
| 0x988 | `field_0x744` | `mEffAngle` | move_jmp 02306DB8 copies current.angle; setWaterSplash 023048F4 / set_simpleLand 0230498C pass it to createLandEmitter as the angle |
| 0x990 | `field_0x74C` | `mEyePos` | _nodeCB_Head 023020F8: the head matrix times a_eye_pos_off; setAttention 02304C44 copies it to eyePos |
| 0x99C | `field_0x758` | `mLookPos` | lookBack 023075B0 / kyorokyoro 023074E4 look at it; cut_init_MAJYU_START 02306624 and chngAnmAtr 0230883C set it |
| 0x9B4 | `field_0x770` | `mHeadPos` | _nodeCB_Head 023020F8: translation of the head joint matrix |
| 0x9C0 | `field_0x77C` | `mEffPos` | move_jmp 02306DB8 copies current.pos; setWaterSplash 023048F4 / set_simpleLand 0230498C pass it to createLandEmitter as the position |
| 0x9D8 | `field_0x794` | `mPartsAngle` | lookBack 023075B0 stores x = current.angle.y, y = the head Y angle, z = the backbone Y angle; chk_parts_notMov 02308DFC compares them with the current ones (tww the same); same code as daNpc_Hi1_c::mPartsAngle |
| 0x9E0 | `field_0x79C` | `mbSetEyePos` | wait_action1 02309CEC / demo_action1 02309E10 / demo_action2 02309EB8 / optn_action1 02309FC8 store the step's result (wait_1(), talk_1(), demo_1() ..); checkOrder 023055A4 sets 1; setAttention 02304C44 tests it with its param (if (it == 0 && !param_1)) (tww the same); same code as daNpc_Hi1_c::mbSetEyePos |
| 0x9EC | `field_0x7A8` | `mEventNo` | init_ZL1_5 02303E98 / eventOrder 02307988 set it and order mEventIdx[it] (fopAcM_orderOtherEventId) |
| 0x9F4 | `field_0x7B0` | `mKyoroTimer` | optn_1 02309794 sets HIO prm 0x2C / 0x2E; kyorokyoro 023074E4 counts it down, then picks the next look position |
| 0x9F6 | `field_0x7B2` | `mKyoroPosNo` | kyorokyoro 023074E4 passes it to kyoroPos and sets cLib_getRndValue(1, 10); optn_1 02309794 clears it |
| 0x9FA | `field_0x7B6` | `mWaitTimer` | wait_1 02309394 / talk_1 02309478 set a random 15..30 and count it down |
| 0xA07 | `field_0x7C3` | `mbMorfAnimStopped` | play_animation 023045E0: the morf's play() result, or 1 when the frame wrapped; setAnm_anm 0230607C / cut_init_OKIRU 023066B4 clear it; cut_move_OKIRU 02306D34 returns it, cut_move_OKIRU_2 02306D48 / optn_1 02309794 test it (tww the same); same code as daNpc_Hi1_c::mbMorfAnimStopped (tww daNpc_Ls1_c mbMorfAnimStopped) |
| 0xA0A | `field_0x7C6` | `mJmpLandEff` | move_jmp 02306DB8 sets it when the jump lands in water or on ground, after storing mEffPos / mEffAngle; setEff 02304A6C keeps those while set, plays set_simpleLand(it) and clears it (tww the same) |
| 0xA0B | `field_0x7C7` | `mbGroundHit` | _execute 02307D18 stores mObjAcch.ChkGroundHit() (the flag of the previous CrrPos) each frame; move_jmp 02306DB8 lands when it was 0 and the acch now hits the ground; cut_init_JMP_OFF 0230684C sets 1 (tww it = mObjAcch.ChkGroundHit()) |
| 0xA0C | `field_0x7C8` | `mbWaterIn` | _execute 02307D18 stores mObjAcch.ChkWaterIn() each frame; move_jmp 02306DB8, setWaterRipple 02304824 and setEff 02304A6C test it (HD in place of ChkWaterIn()); cut_init_JMP_OFF 0230684C clears it, setMtx 02304C98 sets it in Demo37 (tww it = mObjAcch.ChkWaterIn()) |
| 0xA0D | `field_0x7C9` | `mForceEventProc` | checkOrder 023055A4 sets it when event 2 starts; event_proc 023071C4 clears it at that event's end; _execute 02307D18 runs event_proc while set even without a staff index (tww the same) |
| 0xA0E | `field_0x7CA` | `mSwoonOnShip` | setStt 02306184 (state 5) sets it, cut_init_JMP_OFF 0230684C clears it; setMtx 02304C98 places her by swoon_OnShip while set; setWaterRipple 02304824 skips and _execute 02307D18 skips the collision while set (tww the same) |
| 0xA0F | `field_0x7CB` | `mJumpingOff` | cut_init_JMP_OFF 0230684C sets it, event_proc 023071C4 clears it when event 3 ends; _execute 02307D18 skips the collision while set (tww the same) |
| 0xA10 | `field_0x7CC` | `mJumping` | optn_2 023099F4 sets it when she jumps a wall or a drop and, while set, only runs move_jmp and keeps its result (tww the same; tww's comment there suggests mIsMoveJumped) |
| 0xA12 | `field_0x7CE` | `mOnTreeBranch` | init_ZL1_1 02303A7C sets it; setMtx 02304C98 puts the model on the "wood2" branch of the mProcId1 actor while set; _draw 02308090 registers the snap figure at that matrix; demo 02305778 clears it once the demo moves her (tww the same) |
| 0xA14 | `field_0x7D0` | `mEyeFollow` | lookBack 023075B0 sets it in look modes 1 / 4 / 5 (a target position), clears it in 3 and the rest; play_btk_anm 02304548 runs eye_ctrl instead of the btk while set, play_animation 023045E0 turns the eye control on with btk 1; chngAnmAtr 0230883C / cut_init_MAJYU_START 02306624 set it (tww the same) |
| 0xA15 | `field_0x7D1` | `mEyeCtrl` | setEyeCtrl 023041F8 sets 1, clrEyeCtrl 02304230 sets 0; eye_ctrl 02304310 moves the eyes only while set |
| 0xA16 | `field_0x7D2` | `mHideUntilDemo` | init_ZL1_0 023039E4 sets it; _execute 02307D18 returns early while it is set and no demo runs, then clears it in a demo; _draw 02308090 draws nothing while set (tww the same) |
| 0xA17 | `field_0x7D3` | `mKeepShapeAngle` | _execute 02307D18 copies current.angle into shape_angle only while it is 0; checkOrder 023055A4 (event 0) / setStt 02306184 (state 5) set it, cut_init_JMP_OFF 0230684C / event_proc 023071C4 / demo 02305778 clear it (tww the same) |
| 0xA18 | `field_0x7D4` | `mNoDraw` | cut_init_DRW_ONOFF 0230674C sets it when the event's OnOff is 0; demo 02305778 sets it for frame ranges; _draw 02308090 skips drawing while set |
| 0xA19 | `field_0x7D5` | `mbRanExecute` | _execute 02307D18: 0 on the first run, when mInitialPos and mInitialAngle are taken, then 1 (tww if (!it) { ..; it = true; }); same code as daNpc_Hi1_c::mbRanExecute |
| 0xA1B | `field_0x7D7` | `mTalking` | checkOrder 023055A4 sets 1 on the talk command (dEvtCmd_INTALK); wait_1 02309394 starts the talk (setStt(2)) while it is set; talk_1 02309478 talks while it is set and clears it when the message ends; optn_1 02309794 tests it (tww the same); same code as daNpc_Hi1_c::mTalking |
| 0xA1C | `field_0x7D8` | `mLookHeadOnly` | lookBack 023075B0 passes it to dNpc_JntCtrl_c::lookAtTarget_2 as headOnly (d_npc.h; tww lookAtTarget_2 turns the body only if (mbTrn && !it)); wait_1 02309394 / optn_1 02309794 / checkOrder 023055A4 / cut_init_LOK_PLYER 023062C4 / cut_init_LOK_PARTNER 023063A0 set it, wait_1 / optn_2 023099F4 / cut_init_JMP_OFF 0230684C clear it (tww the same); same role as daNpcPhoto_c::mLookHeadOnly |
| 0xA1D | `field_0x7D9` | `mInDemo` | demo 02305778: 1 while demoActorID is set (resets the joint angles when it turns on), 0 otherwise, and returns it; _execute 02307D18 skips the collision while set, setMtx 02304C98 the ripple and effects (tww the same); same code as daNpc_Hi1_c::mInDemo |
| 0xA88 | `field_0x83C` | `mHeadAngleY` | lookBack 023075B0 stores the head Y angle; _nodeCB_Head 023020F8 Y-rotates the head by -it |
| 0xA8A | `field_0x83E` | `mHeadAngleZ` | lookBack 023075B0 stores the head X angle; _nodeCB_Head 023020F8 Z-rotates the head by -it |
| 0xA8C | `field_0x840` | `mEyeAngleY` | lookBack 023075B0 stores the head Y angle; eye_ctrl 02304310 moves the eye texture by it / div * 0.1 |
| 0xA8E | `field_0x842` | `mEyeAngleX` | lookBack 023075B0 stores the head X angle; eye_ctrl 02304310 moves the eye texture by it / div * 0.1 |
| 0xA91 | `field_0x845` | `mAnmAtrNo` | setAnm_ATR 02306458 plays a_anm_prm_tbl[it]; checkOrder 023055A4 / endEvent 02306280 / setStt 02306184 reset it to 0xFF |
| 0xA92 | `field_0x846` | `mMesgAnimeTag` | anmAtr 02308AC0: the message's animation tag (dComIfGp_getMesgAnimeTagInfo) when it changes; checkOrder 023055A4 / endEvent 02306280 / setStt 02306184 0xFF (tww the same); same code as daNpc_Hi1_c::mMesgAnimeTag (tww daNpc_Ls1_c mMesgAnimeTag) |
| 0xA93 | `field_0x847` | `mBtpNum` | setBtp 023024F4 stores the btp number; play_btp_anm 02304268 blinks only while it is not 1; play_animation 023045E0 switches on it; demo 02305778 0x11 (tww the same); same code as daNpc_Hi1_c::mBtpNum (tww daNpc_Ls1_c mBtpNum) |
| 0xA94 | `field_0x848` | `mBtkNum` | setBtk 02302984 stores the btk number; play_animation 023045E0 switches on it (eye control while 1); demo 02305778 10 (tww the same); same code as daNpc_Hi1_c::mBtkNum (tww daNpc_Ls1_c mBtkNum) |
| 0xA95 | `field_0x849` | `mAnmNo` | setAnm_anm 0230607C: skips when the entry's animation equals it, else stores the entry's animation number; createInit 0230503C sets 0x11 |
| 0xA96 | `field_0x84A` | `mOrderEvent` | eventOrder 02307988: 1 / 2 the talk (1 orders the speak event), 3+ orders mEventIdx[it - 3] (mEventNo); checkOrder 023055A4 clears it once that event or the talk starts; init_ZL1_5 02303E98, wait_1 02309394, demo_2..4, optn_1 / optn_3, setStt 02306184 set it (tww the same); same code as daNpc_Hi1_c::mOrderEvent |
| 0xA97 | `field_0x84B` | `mStt` | setStt 02306184 stores the new state (and switches on it); setAnm 02306114 plays a_anm_prm_tbl[it] |
| 0xA98 | `field_0x84C` | `mPrevStt` | setStt 02306184 keeps the previous mStt here; talk_1 02309478 returns to it (setStt(it)) |
| 0xA99 | `field_0x84D` | `mLookMode` | lookBack 023075B0 switches on it: 1 the player's eyes, 2 mLookPos, 3 the angle at 0xA04, 4 the partner, 5 kyorokyoro (0 while mAnmNo is 0xD / 0xE); chngAnmAtr 0230883C, wait_1 02309394, optn_1 / optn_2, cut_init_LOK_PLYER 023062C4 / LOK_PARTNER 023063A0 / MAJYU_START 02306624 set it (tww the same); same code as daNpc_Hi1_c::mLookMode |
| 0xA9A | `field_0x84E` | `mHeapSizeIdx` (was `mType`, misnamed) | decideType 023037F4 returns while it is > 0, then sets it to 1 (the type argument goes to 0xA9B); _create 023052B4 creates the solid heap with a_siz_tbl[it] (tww decideType / _create the same); same code as daNpc_Hi1_c::mHeapSizeIdx |
| 0xA9B | `field_0x84F` | `mType` | decideType 023037F4 stores the type argument (0-7, else -1) and picks the archive by it (Zl2 for 0 / 6 / 7, else Zl); createInit 0230503C / partner_search 02305508 / getMsg 02308D08 switch on it, itemCreateHeap 02303400 / wait_1 02309394 / demo_1 023095E4 / wait_action1 02309CEC / demo_action1 02309E10 / demo 02305778 / _draw 02308090 test it (tww decideType the same); after daNpc_Hi1_c::mType (same code) |
| 0xA9C | `field_0x850` | `mActionStatus` | set_action 023038B8 sets 9 for the leaving action and 0 for the new one; wait_action1 02309CEC / demo_action1 02309E10 / demo_action2 02309EB8 / optn_action1 02309FC8 switch on it and step it (it + 1); partner_search 02305508 tests 1 and steps it (tww the same); same code as daNpc_Hi1_c::mActionStatus |
| 0xA9D | `field_0x851` | `mAnmAtrSetCnt` | anmAtr 02308AC0: while the message types, applies the message's animation attribute (chngAnmAtr) only when it is 0, then adds 1; MSG_DISPLAYED (0xE) and setStt 02306184 reset it (tww the same); same code as daNpc_Hi1_c::mAnmAtrSetCnt |
| 0xAAC | `field_0x860` | `mpEyeMat` | J3DMaterial* [6]: bodyCreateHeap 02302B08 stores the materials eyeLdamB, eyeLdamA, eyeL, eyeRdamB, eyeRdamA, eyeR (HD: found by name; tww walks the zl_eye joint's meshes); the GameCube material animation sharing in _draw is absent in HD |
| 0xAC4 | `field_0x878` | `mpMayuMat` | J3DMaterial* [6]: bodyCreateHeap 02302B08 stores the eyebrow materials mayuLdamB, mayuLdamA, mayuL, mayuRdamB, mayuRdamA, mayuR (HD: found by name; tww walks the zl_mayu joint's meshes) |
| 0xADC | `field_0x890` | `mpEyeShape` | J3DShape* [6]: bodyCreateHeap 02302B08 stores the shapes of mpEyeMat[i]; _draw 02308090 shows / hides them (shape_vis) around the zl_eye joint's two draw passes (tww the same) |
| 0xAF4 | `field_0x8A8` | `mpMayuShape` | J3DShape* [6]: bodyCreateHeap 02302B08 stores the shapes of mpMayuMat[i]; _draw 02308090 shows / hides them (shape_vis) around the zl_mayu joint's two draw passes (tww the same) |
| 0xB0C | `mB0C` | `mHeadMaterialIndex` | HD only: bodyCreateHeap 02302B08 stores the index of the material "zelda_zhead"; mesh_vis (_draw 02308090) sets the visibility of its shape against the other meshes of the world_root joint; the name d_a_npc_zl1_heap.h already gives it (one name per field) |
| 0xB10 | `mB10` | `mHeadOverlayMaterialIndex` | HD only: bodyCreateHeap 02302B08 stores the index of the material "zelda_zhead_2_"; mesh_vis (_draw 02308090) treats it like mHeadMaterialIndex; the name d_a_npc_zl1_heap.h already gives it (one name per field) |
| 0x00 | `field_0x0` | `bckNum` | setAnm_anm 0230607C: the entry's bck animation number (bckResID), stored to mAnmNo (tww); after tww daNpc_Ls1_c::anm_prm_c bckNum and our daNpc_Hi1_c |
| 0x01 | `field_0x1` | `btpNum` | setAnm_NUM 023066D0 / setAnm / setAnm_ATR pass it to init_texPttrnAnm (tww); after tww daNpc_Ls1_c::anm_prm_c btpNum and our daNpc_Hi1_c |
| 0x9A8 | `field_0x764` | `mDemoAreaPos` | init_ZL1_3 02303BBC: a point in front of the start position (y 400), event_proc 023071C4 / demo_action2 02309EB8 set its y; demo_2 02309640 orders event 4 when the player is in the chk_areaIN area around it, demo_3 023096CC event 5 when the player is nearer than mPrmTbl.field_28 (tww the same) |
| 0x9F2 | `field_0x7AE` | `mLookPlayerTimer` | optn_1 02309794 / kyorokyoro 023074E4 load it from mPrmTbl.field_30 / field_32 with mKyoroTimer; optn_2 023099F4 keeps look mode 1 (the player) while it counts down, then look mode 5 (look around) (tww the same) |
| 0x9FC | `field_0x7B8` | `mIdleAnmTimer` | setStt 02306184 / optn_1 02309794: random 90..180; optn_1 counts it down and switches to the second idle animation (setAnm_NUM 0xC) at 0 (tww the same) |
| 0x9FE | `field_0x7BA` | `mIdleAnmLoops` | optn_1 02309794: 1 or 2 when animation 0xC starts, counted down at each animation end, back to animation 8 at 0; setStt 02306184 clears it (tww the same) |
| 0xA00 | `field_0x7BC` | `mTurnStepMode` | _execute 02307D18: the m_jnt turn step is mPrmTbl.mMaxTurnStep while it is < 0, else mPrmTbl.field_5C; createInit 0230503C -1, chngAnmAtr 0230883C 10 for message 0x15E3 and -1 for 0x15E4 (tww the same) |
| 0xA04 | `field_0x7C0` | `mLookAngleY` | lookBack 023075B0: the target angle of look mode 3; never written in WWHD or tww |
| 0xA06 | `field_0x7C2` | `mEnvRatioStep` | cut_init_OMAMORI_ONOFF 02306BD8: -1 for OnOff 0 (darken, with the light at mLightPos) / 1 otherwise (brighten) / 0; darkProc 02307A58 runs decEnvironment / incEnvironment on mRatio for -1 / 1; incEnvironment 023079F8 clears it at ratio 1 (tww the same) |
| 0xA7C | `field_0x830` | `mLightPos` | cut_init_OMAMORI_ONOFF 02306BD8: set_LightPos, copied into mLightInfluence1 / 2 .mPos (tww the same); chngAnmAtr 0230883C: message 0x15E7 looks at a point beside it |
| 0x7C | `field_0x7C` | `mMoveFlag` | daNpc_Zl1_matAnm_c_ct 0230242C clears it; daNpc_Zl1_matAnm_c_calc 023086FC writes mOffset into the texture SRT only while it is set; demo 02305778 clears it (tww clrMoveFlag / setMoveFlag); as d_a_pz.h's matAnm mMoveFlag |
| 0x08 | `field_0x8` | `mRefCount` | daNpc_Zl1_HIO_c_ct 0230A0EC -1; never read in WWHD or tww; after our decomp's daNpc_Hi1_HIO_c / daNpc_Ba1_HIO_c mRefCount: the same HIO layout (mNo, then the s32 instance counter) |

Open: the padding arrays (no use), `field_0x7C4` (setAnm_anm 0230607C only clears it; tww the same), daNpc_Zl1_matAnm_c `field_0x74` (no use in WWHD or tww). The nested anm_prm_c rows (`bckNum`, `btpNum`) are at its offsets 0x00 / 0x01, daNpc_Zl1_matAnm_c `mMoveFlag` at 0x7C and daNpc_Zl1_HIO_c `mRefCount` at 0x08.

## daNpc_Md_c (d_a_npc_md.h)

Old names are the GameCube offsets (zeldaret/tww `mXXXX`). "TWW accessor": the CC0 zeldaret/tww header's inline accessor of the same field (same offset and use), e.g. `getEyePos()`.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0624 | `m0508` | `mEmitters` | setWingEmitter 0228C1B4 / setHane02Emitter 0228E2D4 set ID_IT_SN_MD_HANE* emitters in it; demoFlyNpcAction 022921B4 / flyPlayerAction 02294160 delete them (emitterDelete) |
| 0x063C | `m0520` | `mBtpAnm` | initTexPatternAnm 022846EC / playTexPatternAnm 0228729C init and play it as the eye texture pattern; draw 02289E2C enters it (btpAnm_entry) |
| 0x0D48 | `m0B70` | `mMirrorPacket` | draw 02289E2C updates it (mirrorPacket_update) and enters it in the XLU list; daNpc_Md_dt 0228A254 destroys it |
| 0x4170 | `m304C` | `mMirrorShieldCb` | lightHitCheck 02287754 makes its ID_AK_SN_MIRRORSHIELD00 emitter on the harp light model's matrix; daNpc_Md_dt 0228A254 ends it |
| 0x4198 | `m3074` | `mRippleCb` | execute 02288B08 sets the water ripple emitter with it when it has none; daNpc_Md_dt 0228A254 ends it |
| 0x41AC | `m3088` | `mEyePos` | nodeCallBack 02283614: the head matrix times l_offsetEyePos; setAttention 02285984 copies it to eyePos; TWW getEyePos() |
| 0x41B8 | `m3094` | `mAttentionBasePos` | nodeCallBack 02283614: the head matrix times l_offsetAttPos; init 02285580 copies current.pos; TWW getAttentionBasePos() |
| 0x41C4 | `m30A0` | `mThrowPos` | throwNpcAction 02291094 copies current.pos; glidingNpcAction 02291304 subtracts it from current.pos |
| 0x41DC | `m30B8` | `mTurnTargetPos` | initialTurnEvent 0228D150 sets the event's position; actionTurnEvent 0228D32C turns to it (lookBack) |
| 0x41F4 | `m30D0` | `mBtpFrame` | setAnm 02286D7C / initialSetAnmEvent 0228D564 copy the body morf frame; playTexPatternAnm 0228729C sets the eye texture pattern frame to it |
| 0x420C | `m30F0` | `mStatus` | init 02285580 sets 0 / daMdStts_UNK8; setBaseMtx 02285A6C tests daMdStts bits; TWW setStatus/checkStatus |
| 0x421C | `m3100` | `mSwitchNo` | init 02285580: (param >> 16) & 0xFF; waitNpcAction 0228EB88 tests dComIfGs_isSwitch(it) |
| 0x4228 | `m310C` | `mStickValue` | mkamaePlayerAction 02294984 sets md_stickValue(0); waitPlayerAction 02293840 clears it; lookBackWaist 0228DFF8 reads it |
| 0x422C | `m3110` | `mLookMaxVel` | demoFlyNpcAction 022921B4 eases it to HIO mMaxHeadTurnVel, else clears it; passed to dNpc_JntCtrl lookAtTarget as maxVel |
| 0x422E | `m3112` | `mBtpFrameMax` | initTexPatternAnm 022846EC: J3DAnm_getFrameMax of the eye texture pattern; playTexPatternAnm 0228729C restarts it with this length |
| 0x4230 | `m3114` | `mWaistRotX` | lookBackWaist 0228DFF8 eases it; waistNodeCallBack 022837D0 rotates the waist by it; TWW getWaistRotX() |
| 0x4232 | `m3116` | `mWaistRotY` | lookBackWaist 0228DFF8 eases it toward HIO m22 * stick; waistNodeCallBack 022837D0 rotates the waist by it; TWW getWaistRotY() |
| 0x4234 | `m3118` | `mEventTimer` | initialFlyEvent 0228C1C4 sets 60 * 30, initialWalkEvent 0228C4A4 20 * 30; actionFlyEvent 0228C234 ends the event when it runs out |
| 0x4236 | `m311A` | `mStickAngY` | mkamaePlayerAction 02294984 sets getStickAngY(FALSE); waitPlayerAction 02293840 clears it |
| 0x4247 | `m312B` | `mAttnSetCount` | setAttention 02285984 / nodeCallBack 02283614 add 1 up to 0xFF; execute 02288B08 clears it; TWW incAttnSetCount() |
| 0x4249 | `m312D` | `mAnmTblIdx` | setAnm 02286D7C stores the animation table index (and skips when it and the speed are unchanged); getMsg 0228DDEC tests == 0x10 |
| 0x424F | `m3133` | `mBlinkTimer` | initTexPatternAnm 022846EC clears it; playTexPatternAnm 0228729C counts it down and restarts the eye pattern with 30 + rnd(60) |
| 0x4250 | `m3134` | `mHairMode` | hairNodeCallBack 02283A78 tests and sets bits; TWW checkBitHairMode / setBitHairMode |
| 0x4251 | `m3135` | `mEffectStatus` | flyPlayerAction 02294160 / landPlayerAction 02294838 set bit 1; execute 02288B08 tests it; TWW setBitEffectStatus / checkBitEffectStatus |
| 0x4253 | `m3137` | `mBtpAnmTblIdx` | initTexPatternAnm 022846EC stores the texture pattern table index; setAnm 02286D7C re-inits the pattern when it differs |
| 0x4258 | `m313C` | `mPiyo2TalkCnt` | getMsg 0228DDEC reads and clears it; init 02285580 clears it; TWW getPiyo2TalkCNT / setPiyo2TalkCNT |
| 0x4290 | `m3174` | `mHairPos` | hairTopNodeCallBack 022839F0 / hairNodeCallBack 02283A78: the hair joint positions; TWW getPHairPos(i) |
| 0x42F0 | `m31D4` | `mHairVec` | hairNodeCallBack 02283A78: per hair joint vector; TWW getPHairVec(i) |
| 0x4350 | `m3234` | `mHairWall` | waistNodeCallBack 022837D0: four waist points (waistVecDat) the hair collides with (hairCross); TWW getPHairWall() |
| 0x4380 | `m3264` | `mHairDist` | hairNodeCallBack 02283A78: per hair joint distance; TWW getPHairDist(i) |

Round 2:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x417C | `m3058` | `mHitShieldLightCb` | lightHitCheck 02287754 sets the ID_AK_SN_HITSHIELDLIGHT00 (0x8232) emitter with it while the harp light hits, places it at the light's hit point on mCps (mPos) and the light direction (mAngle), else end(); create 02286084 sets its vtable; daNpc_Md_dt 0228A254 ends it (tww the same) |
| 0x41D0 | `m30AC` | `mLookDownOffset` | init 02285580 sets (0, 0, 50); actionLookDown 0228D848 sets y = -50, rotates it by shape_angle.y and adds it to (pos.x, eyePos.y, pos.z) as the look-at target (tww the same) |
| 0x41E8 | `m30C4` | `mMoveBlockSpeed` | execute 02288B08: chkMoveBlock's out vector (daPy_npc_c::chkMoveBlock: block current.pos - old.pos); escapeNpcAction 02293500 turns by cM_atan2s(it.x, it.z) +/- 0x4000 (tww the same); same role as daNpc_Cb1_c::mMoveBlockSpeed |
| 0x4210 | `m30F4` | `mHarpSe` | animationPlay 02287364: the SEA type's harp note SE (random from l_harp_play_se) played on the harp frames, kept for mHarpSeCount repeats; init 02285580 JA_SE_CM_MD_HARP_CN4 (tww the same) |
| 0x4214 | `m30F8` | `mCylH` | setCollision 02287A74 passes it to mCyl1 / mCyl2 / mCyl3 SetH; init 02285580 and the stand actions set 120, the squat / carry / land actions 75 (tww the same) |
| 0x4220 | `m3104` | `mAnmPrmIdx` | setAnm 02286D7C stores its argument and indexes l_anmPrm with it (mAnmTblIdx is prm->anmTblIdx); waitNpcAction 0228EB88, harpWaitNpcAction 0228F424, animationPlay 02287364 and 9 more test it for animation numbers (tww the same); after daNpc_Cb1_c::mAnmPrmIdx |
| 0x4224 | `m3108` | `mNormalSpeed` | setNormalSpeedF 0228E37C eases it toward the target speed (cLib_addCalc / step); walkProc 0228E5CC sets speedF = it and the run rate |it / mMaxNormalSpeed| (tww the same) |
| 0x4246 | `m312A` | `mAnmEnd` | animationPlay 02287364 = mpMorf play() result, also 1 when the frame wraps; setAnm 02286D7C clears it; about 25 actions / events step on when it is set (tww the same); unit specs call it the animation end flag |
| 0x4248 | `m312C` | `mHasAttention` | waitNpcAction 0228EB88 / talkNpcAction 0228F610 / shipNpcAction 0228FD20 and 4 more store chkAttention's result; chkAttention 0228DAF4 widens the check by +40 distance / +0x71C angle while set (tww the same); same role as daNpc_Cb1_c::mHasAttention |
| 0x424B | `m312F` | `mSearchSide` | searchNpcAction 02292C48 flips it (^= 1) when mSearchTimer runs out (tww the same); no read in WWHD or tww; same code as daNpc_Cb1_c::mSearchSide |
| 0x424C | `m3130` | `mSearchTimer` | searchNpcAction 02292C48 counts it down (cLib_calcTimer) and reloads it with cLib_getRndValue(8, 20) at 0, flipping mSearchSide (tww the same); same role as daNpc_Cb1_c::mSearchTimer |
| 0x424D | `m3131` | `mGroundHit` | execute 02288B08: mAcch.ChkGroundHit(); while set the moving-BG correction applies, the landing effect plays only when it was 0; routeCheck 022928A8 / initialMovePosEvent 0228B68C 1, carryNpcAction 022904DC 0 (tww the same); same role as daNpc_Cb1_c::mGroundHit |
| 0x4252 | `m3136` | `mBtpSyncBody` | initTexPatternAnm 022846EC stores the btp table's flag (tww btpAnmTbl m20); playTexPatternAnm 0228729C: while 1 the eye pattern frame is set to mBtpFrame (body frame) instead of blinking (tww the same) |
| 0x425A | `m313E` | `mHarpSeCount` | animationPlay 02287364: 0 picks a new random mHarpSe and loads 2, else counts down and replays mHarpSe; 0 outside the harp frames; init 02285580 0 (tww the same) |
| 0x425B | `m313F` | `mMoveBlockSide` | execute 02288B08: chkMoveBlock(&mMoveBlockSpeed) (-1 / 1 for the side of a pushed block, else 0) starts escapeNpcAction; escapeNpcAction 02293500 turns by +/-0x4000 by its sign (tww the same); same role as daNpc_Cb1_c::mMoveBlockSide |
| 0x4262 | `m3146` | `mActionTimer` | a countdown in the actions: flyPlayerAction 02294160 loads HIO mFlapTime (maxFallSpeed 10 while it runs), harpWaitNpcAction 0228F424 0xC / 0x78, escapeNpcAction 02293500 0x1E; setAction 02285384 clears it (tww the same) |
| 0x4264 | `m3148` | `mCrossWindHit` | glidingNpcAction 02291304 sets 1 when windProc reports a cross wind (anm 0xA, glide ends after mGlideAnmCount); windProc 02290CF8 uses the throw gravity and the tail-wind lift only while 0; throwNpcAction 02291094 / setAction 02285384 0 (tww the same) |
| 0x4266 | `m314A` | `mGlideAnmCount` | glidingNpcAction 02291304 loads 2 and subtracts 1 per animation end once mCrossWindHit is set, then starts fall02NpcAction; setAction 02285384 0 (tww the same) |
| 0x4268 | `m314C` | `mWindProcRet` | glidingNpcAction 02291304 stores windProc's result (1 tail or no wind, 0 cross wind) and switches the animation when it changes; throwNpcAction 02291094 1, then 0 when windProc returns 0 (tww the same) |
| 0x426A | `m314E` | `mThrowWindTimer` | throwNpcAction 02291094 loads HIO mThrowWindTime; windProc 02290CF8 counts it down and lifts by a tail wind only between 0 and mThrowWindTime - mWindLiftDelay (tww the same) |
| 0x426C | `m3150` | `mBasePosY` | jumpPlayerAction 02293FA8 / fall02NpcAction 02291764 store current.pos.y and shake on landing after dropping HIO mShockFallHeight below it; flyPlayerAction 02294160 sets pos.y + 1500 and caps the flight height at it; setAction 02285384 0 (tww the same) |
| 0x4270 | `m3154` | `mCrossWindPush` | windProc 02290CF8: in a cross wind it grows by HIO mCrossWindStep up to the wind power times mCrossWindRate and is the applied power; throwNpcAction 02291094 0 (tww the same) |
| 0x43B4 | `m3298` | `mCarryOffset` | carryNpcAction 022904DC clears y / z and chases it to HIO mCarryFlyOffsetX / Y / Z while Medli flies Link; setBaseMtx 02285A6C translates the model by it (tww the same); same role as daNpc_Cb1_c::mCarryOffset |
| 0x43C0 | `m32A4` | `mGroundNormal` | execute 02288B08 (md_setGroundInfo): the ground triangle's normal; waitGroundCheck 0228E9C0 turns to cM_atan2s(it.x, it.z); routeAngCheck 022926D0 crosses it with the wall normal (tww the same); same role as daNpc_Cb1_c::mGroundNormal |
| daPy_py_l 0x414 | `field_0x2f8` | `mWindVelocity` | the daPy_py_c name at the same offset (d_a_player.h, GameCube field_0x2f8); d_a_npc_md.h's local view of the player |

Open: `m3144` (per-action work slot: timer, angle or flag by action), `m313A` (talk state whose 1 is never set from outside its own branch), `m313D` (written by create only), `m313B`, `m3141`, `m3287` (padding / no use). Views: `MedliView` (d_a_tag_md_cb.h) follows (m30F0 = mStatus); `MedliShieldViewHD` (d_camera_engine2.cpp, camera area) holds m3114 = mWaistRotX. daPy_py_l `field_0x2b0` and daPy_npc_l `m4E4`, `m4E8` stay open (daPy_py_c / daPy_npc_c still offset-named; player classes on hold).


### daNpc_Md_HIO_c (d_a_npc_md.h, declared as daNpc_Md_HIO_l)

Medli's tuning block (l_HIO). zeldaret/tww has offset names only (m0B8 etc.) and the same reads; the names come from the WWHD reads, with the constructor values (daNpc_Md_HIO_c_ct 02294F64) as support. The five sub-HIOs are named for their role; their own m04.. members stay offset-named for now. Animation file names are from tww l_anmTbl.

| HIO offset | Old (tww name) | New | Evidence |
|---|---|---|---|
| 0x008 | `m008` | `mWalk` | daNpc_Md_HIO2_c: the walk and turn tuning: walkProc 0228E5CC / walkPlayerAction 02293B5C take its m08 as mMaxNormalSpeed; setSpeedAndAngleNormal 0228E4C0 / setNormalSpeedF 0228E37C its speed factors and m20 / m22 / m24 turn steps; searchNpcAction 02292C48 / escapeNpcAction 02293500 turn the body with m20 / m22 / m24; waitPlayerAction 02293840 turns toward the stick with m26 / m28 / m2A; waitGroundCheck 0228E9C0 m08; daNpc_Md_HIO_c_ct 02294F64 constructs it |
| 0x034 | `m034` | `mMirror` | daNpc_Md_HIO3_c: the harp-mirror tuning: lookBackWaist 0228DFF8 (mkamaePlayerAction 02294984 / mwaitNpcAction 0228FF5C) turns the waist by the stick with its m04 / m08 factors and m1E / m20 / m22 limits; setCollision 02287A74 / draw 02289E2C place the reflected light at the harp model by its m0C / m10 / m14 offset and m18 / m1A / m1C rotation; daNpc_Md_HIO_c_ct 02294F64 constructs it |
| 0x05C | `m05C` | `mCarried` | daNpc_Md_HIO4_c: setBaseMtx 02285A6C lowers the carried model by its m4 (86.0) while Link carries Medli in the no-carry-action pose; daNpc_Md_HIO_c_ct 02294F64 constructs it |
| 0x068 | `m068` | `mKyoroAnm` | daNpc_Md_HIO5_c: setAnm 02286D7C takes the morf (m4) and speed (m8) of animation 0x28 from it (tww l_anmTbl: md_kyoro2.bck), which waitNpcAction 0228EB88 plays for the M_DAI / EDAICHI types; daNpc_Md_HIO_c_ct 02294F64 constructs it |
| 0x074 | `m074` | `mShipRide` | daNpc_Md_HIO6_c: used while daMdStts_SHIP_RIDE is set: chkAttention 0228DAF4 takes its m04 as the attention distance; execute 02288B08 passes its m08..m16 to mJntCtrl.setParam as the head / backbone limits; daNpc_Md_HIO_c_ct 02294F64 constructs it |
| 0x0B8 | `m0B8` | `mDra09MaxAttnDistXZ` | chkAttention 0228DAF4: the attention XZ distance for the M_DRA09 type once event bit 1101 is set (instead of mNpc.mMaxAttnDistXZ); daNpc_Md_HIO_c_ct 02294F64 770.0 |
| 0x0BC | `m0BC` | `mMaxAttnY` | chkAttention 0228DAF4: Link counts for attention only below this height above Medli; waitNpcAction 0228EB88 the same bound for the SEA type's invitation; daNpc_Md_HIO_c_ct 02294F64 250.0 |
| 0x0C0 | `m0C0` | `mMinAttnY` | chkAttention 0228DAF4: Link counts for attention only above this height relative to Medli; waitNpcAction 0228EB88 the same bound for the SEA type's invitation; daNpc_Md_HIO_c_ct 02294F64 -450.0 |
| 0x0C4 | `m0C4` | `mFollowStopDist` | searchNpcAction 02292C48: Medli stops walking after Link inside this XZ distance (squared; walk speed rises beyond it); NpcCall 0228E8D8 / mwaitNpcAction 0228FF5C: a call ends (searchNpcAction) once Link is beyond twice it; daNpc_Md_HIO_c_ct 02294F64 90.0 |
| 0x0C8 | `m0C8` | `mCallDist` | NpcCall 0228E8D8 / mwaitNpcAction 0228FF5C: Link's call command reaches Medli within this distance (squared); daNpc_Md_HIO_c_ct 02294F64 400.0 |
| 0x0CC | `m0CC` | `mSeaEventDistXZ` | waitNpcAction 0228EB88: the SEA type starts its event (mode 0xB) while Link is within this XZ distance (squared) and between mMinAttnY / mMaxAttnY; daNpc_Md_HIO_c_ct 02294F64 600.0 |
| 0x0D4 | `m0D4` | `mFlyMaxSpeed` | flyPlayerAction 02294160: the stick speed target is capped at it; daNpc_Md_HIO_c_ct 02294F64 17.0 |
| 0x0D8 | `m0D8` | `mFlyMinSpeed` | flyPlayerAction 02294160: a positive stick speed target below it is raised to it; daNpc_Md_HIO_c_ct 02294F64 0.0 |
| 0x0DC | `m0DC` | `mFlyAccel` | flyPlayerAction 02294160: cLib_chaseF step of speedF toward the stick speed; daNpc_Md_HIO_c_ct 02294F64 0.5 |
| 0x0E8 | `m0E8` | `mThrowSpeedF` | throwNpcAction 02291094: the forward speed Medli is thrown with; daNpc_Md_HIO_c_ct 02294F64 15.0 |
| 0x0EC | `m0EC` | `mThrowSpeedY` | throwNpcAction 02291094: the upward speed Medli is thrown with; daNpc_Md_HIO_c_ct 02294F64 22.0 |
| 0x0F0 | `m0F0` | `mThrowFlapSpeedY` | throwNpcAction 02291094: at wing frame 23 speed.y is raised to at least it (the flap); daNpc_Md_HIO_c_ct 02294F64 9.0 |
| 0x0F4 | `m0F4` | `mGravity` | gravity of the ground, jump and land actions: init 02285580, waitNpcAction 0228EB88, mwaitNpcAction 0228FF5C, jumpNpcAction 022933A0, jumpPlayerAction 02293FA8, landPlayerAction 02294838, wallHitNpcAction 022919D4 and 11 more; daNpc_Md_HIO_c_ct 02294F64 4.0 |
| 0x0F8 | `m0F8` | `mCarryFlyOffsetX` | carryNpcAction 022904DC: cLib_chaseF moves the carried model offset's x (mCarryOffset, setBaseMtx 02285A6C) toward it while Medli flies Link; daNpc_Md_HIO_c_ct 02294F64 -1.51367 |
| 0x0FC | `m0FC` | `mCarryFlyOffsetY` | carryNpcAction 022904DC: cLib_chaseF moves the carried model offset's y (mCarryOffset, setBaseMtx 02285A6C) toward it while Medli flies Link; daNpc_Md_HIO_c_ct 02294F64 -1.09863 |
| 0x100 | `m100` | `mCarryFlyOffsetZ` | carryNpcAction 022904DC: cLib_chaseF moves the carried model offset's z (mCarryOffset, setBaseMtx 02285A6C) toward it while Medli flies Link; daNpc_Md_HIO_c_ct 02294F64 -0.56152 |
| 0x104 | `m104` | `mStickTurnMin` | waitPlayerAction 02293840 / flyPlayerAction 02294160 / mkamaePlayerAction 02294984: Medli turns (and flies) by the stick only from this stick value on (or while locked on); daNpc_Md_HIO_c_ct 02294F64 0.1 |
| 0x108 | `m108` | `mStickWalkMin` | waitPlayerAction 02293840: from this stick value on the body follows the stick and Medli starts walkPlayerAction; daNpc_Md_HIO_c_ct 02294F64 0.2 |
| 0x10C | `m10C` | `mFlyStickSpeed` | flyPlayerAction 02294160: the stick value times it is the flying speed target; daNpc_Md_HIO_c_ct 02294F64 10.0 |
| 0x110 | `m110` | `mThrowMaxFallSpeed` | throwNpcAction 02291094 / windProc 02290CF8 / fall02NpcAction 02291764: maxFallSpeed while thrown or gliding down; daNpc_Md_HIO_c_ct 02294F64 -50.0 |
| 0x114 | `m114` | `mThrowGravity` | throwNpcAction 02291094 / windProc 02290CF8 / fall02NpcAction 02291764: gravity while thrown or gliding down; daNpc_Md_HIO_c_ct 02294F64 1.3 |
| 0x118 | `m118` | `mFiringAnmSpeed` | setAnm 02286D7C: the play speed of animation 6 (tww l_anmTbl: md_firing.bck), which throwNpcAction 02291094 plays; daNpc_Md_HIO_c_ct 02294F64 1.0 |
| 0x11C | `m11C` | `mFly02AnmSpeed` | setAnm 02286D7C: the play speed of animations 9 and 10 (tww l_anmTbl: md_fly02.bck), which carryNpcAction 022904DC, glidingNpcAction 02291304 and the fly event play; daNpc_Md_HIO_c_ct 02294F64 1.5 |
| 0x120 | `m120` | `mCarryTiredAnmSpeed` | carryNpcAction 022904DC: the body and wing animation speed once the flying timer is below 30; daNpc_Md_HIO_c_ct 02294F64 2.0 |
| 0x124 | `m124` | `mTiredFlyAnmSpeed` | setAnm 02286D7C: the play speed of animation 0x2E (tww l_anmTbl: md_fly02.bck), which flyPlayerAction 02294160 plays when the flying timer is below mTiredFlyTime; daNpc_Md_HIO_c_ct 02294F64 1.6 |
| 0x130 | `m130` | `mCrossWindRate` | windProc 02290CF8: the wind power factor when the wind does not blow along Medli's facing (dot <= 0.7071); daNpc_Md_HIO_c_ct 02294F64 3.0 |
| 0x134 | `m134` | `mTailWindRate` | windProc 02290CF8: the wind power factor when the wind blows along Medli's facing; daNpc_Md_HIO_c_ct 02294F64 1.25 |
| 0x138 | `m138` | `mCrossWindStep` | windProc 02290CF8: in a cross wind the wind push (mCrossWindPush) grows by it each frame up to the power times mCrossWindRate; daNpc_Md_HIO_c_ct 02294F64 0.2 |
| 0x13C | `m13C` | `mWindMaxSpeed` | windProc 02290CF8: speedF after adding the wind is capped at it; daNpc_Md_HIO_c_ct 02294F64 100.0 |
| 0x140 | `m140` | `mWindLiftMaxFallSpeed` | windProc 02290CF8: maxFallSpeed while a tail wind lifts the thrown Medli (lift window of the mThrowWindTimer timer); daNpc_Md_HIO_c_ct 02294F64 50.0 |
| 0x144 | `m144` | `mWindLiftGravity` | windProc 02290CF8: gravity while a tail wind lifts the thrown Medli (lift window of the mThrowWindTimer timer); daNpc_Md_HIO_c_ct 02294F64 9.0 |
| 0x148 | `m148` | `mWindAtkPower` | windProc 02290CF8: the wind power when a wind attack hits mCyl2 (with mbWindAtkHit set); daNpc_Md_HIO_c_ct 02294F64 0.9 |
| 0x14C | `m14C` | `mHairGravity` | hairNodeCallBack 02283A78: the length of the downward pull vector on each hair joint; daNpc_Md_HIO_c_ct 02294F64 6.0 |
| 0x150 | `m150` | `mWallHitSpeedF` | wallHitNpcAction 022919D4: the speedF Medli bounces off the wall with; daNpc_Md_HIO_c_ct 02294F64 10.0 |
| 0x154 | `m154` | `mThrowWindFrame` | throwNpcAction 02291094: windProc runs only once the wing animation frame is past it; daNpc_Md_HIO_c_ct 02294F64 5.0 |
| 0x188 | `m188` | `mFlyGravity` | flyPlayerAction 02294160 / initialFlyEvent 0228C1C4: gravity while flying; daNpc_Md_HIO_c_ct 02294F64 1.0 |
| 0x18C | `m18C` | `mFlyMaxFallSpeed` | flyPlayerAction 02294160: maxFallSpeed while flying (10.0 instead during mFlapTime); daNpc_Md_HIO_c_ct 02294F64 -10.0 |
| 0x1A8 | `m1A8` | `mFly01AnmMorf` | setAnm 02286D7C: the morf of animations 7 and 8 (tww l_anmTbl: md_fly01.bck), which flyPlayerAction 02294160 plays; daNpc_Md_HIO_c_ct 02294F64 3.0 |
| 0x1AC | `m1AC` | `mHighFallHeight` | carryNpcAction 022904DC: put down at least this high above the ground, Medli falls with fall02NpcAction instead of fallNpcAction; daNpc_Md_HIO_c_ct 02294F64 1000.0 |
| 0x1B0 | `m1B0` | `mShockFallHeight` | jumpPlayerAction 02293FA8 / fall02NpcAction 02291764: a landing after a drop of at least this height (from mBasePosY) shakes the camera (the M_DAI type in fall02); daNpc_Md_HIO_c_ct 02294F64 1000.0 |
| 0x1B4 | `m1B4` | `mThrowWindTime` | throwNpcAction 02291094 loads the mThrowWindTimer timer with it; windProc 02290CF8 lifts by a tail wind only while the timer is between 0 and it minus mWindLiftDelay; daNpc_Md_HIO_c_ct 02294F64 23 |
| 0x1B6 | `m1B6` | `mWindLiftDelay` | windProc 02290CF8: the tail-wind lift starts only this many frames after the throw (timer below mThrowWindTime minus it); daNpc_Md_HIO_c_ct 02294F64 20 |
| 0x1BA | `m1BA` | `mPiyoTime` | piyo2NpcAction 02291F50: the dizzy time (m3144), halved for the EDAICHI / M_DAI types; daNpc_Md_HIO_c_ct 02294F64 120 |
| 0x1BC | `m1BC` | `mTurnLookMaxVel` | lookBack 0228A524: mLookMaxVel eases toward it instead of mNpc.mMaxHeadTurnVel when actionTurnEvent 0228D32C turns Medli; daNpc_Md_HIO_c_ct 02294F64 500 |
| 0x1BE | `m1BE` | `mFlyKeepTime` | flyPlayerAction 02294160: each A press (and the take-off) reloads m3144 with it; the flight ends when m3144 runs out; daNpc_Md_HIO_c_ct 02294F64 70 |
| 0x1C0 | `m1C0` | `mFlapTime` | flyPlayerAction 02294160: each A press (and the take-off) reloads mActionTimer with it; while mActionTimer runs, maxFallSpeed is 10.0 instead of mFlyMaxFallSpeed; daNpc_Md_HIO_c_ct 02294F64 5 |
| 0x1C4 | `m1C4` | `mTiredFlyTime` | flyPlayerAction 02294160: below this flying timer Medli switches to animation 0x2E and the hane03 emitter; daNpc_Md_HIO_c_ct 02294F64 150 |
| 0x1C6 | `m1C6` | `mbWindAtkHit` | windProc 02290CF8: while set, a wind attack hitting mCyl2 pushes Medli (power mWindAtkPower); daNpc_Md_HIO_c_ct 02294F64 1 |
| 0x1C7 | `m1C7` | `mbWindLiftAlways` | windProc 02290CF8: while set, the tail-wind lift applies even when mCrossWindHit is set; daNpc_Md_HIO_c_ct 02294F64 0 |

Open: `m0D0`, `m0E0`, `m0E4`, `m128`, `m12C`, `m158`..`m184`, `m190`..`m1A4`, `m1B8`, `m1C2`, `m1C8`: set only by daNpc_Md_HIO_c_ct 02294F64, never read in WWHD or tww; the sub-HIO members stay for a later batch; dNpc_HIO_l `m04` / `m22` became `mPlayerEyeOffsetY` / `mDebugDraw` with dNpc_HIO_c (other.md, round 12).

### daNpc_Md_HIO2_l (l_HIO.mWalk; d_a_npc_md.h)

Round 3: Medli's walk and turn tuning (tww daNpc_Md_HIO2_c; vtable first, so the WWHD offsets equal the GameCube names). Each is set only by daNpc_Md_HIO2_c_ct 02294EA0 (default in parentheses) and never written elsewhere in WWHD or tww; the reads are in our d_a_npc_md.cpp / d_a_npc_md_event.cpp and match zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `m04` | `mWalkAccel` | setSpeedAndAngleNormal 0228E4C0: the speed added per frame (it * stick * cos of the turn left) that setNormalSpeedF 0228E37C adds to mNormalSpeed (tww the same) (5) |
| 0x08 | `m08` | `mWalkMaxSpeed` | walkProc 0228E5CC / walkPlayerAction 02293B5C: mMaxNormalSpeed; waitGroundCheck 0228E9C0 caps the slide speed on slope ground at it (tww the same) (17) |
| 0x0C | `m0C` | `mWalkMaxStep` | setSpeedAndAngleNormal 0228E4C0: setNormalSpeedF 0228E37C's largest step of mNormalSpeed (tww the same) (7) |
| 0x10 | `m10` | `mWalkMinStep` | setSpeedAndAngleNormal 0228E4C0: setNormalSpeedF 0228E37C's smallest step of mNormalSpeed (tww the same) (6) |
| 0x14 | `m14` | `mWalkScale` | setSpeedAndAngleNormal 0228E4C0: setNormalSpeedF 0228E37C's cLib_addCalc scale for mNormalSpeed (tww the same) (0.7) |
| 0x18 | `m18` | `mWalkAnmSpeed` | walkProc 0228E5CC: the body and arm animations play at \|mNormalSpeed / mMaxNormalSpeed\| * it (tww the same) (2.6) |
| 0x1C | `m1C` | `mWallSlowRate` | setNormalSpeedF 0228E37C: facing a wall, the target speed is scaled by 1 - it * cos of the wall angle (tww the same) (0.6) |
| 0x20 | `m20` | `mTurnMaxStep` | setSpeedAndAngleNormal 0228E4C0 / searchNpcAction 02292C48 / escapeNpcAction 02293500 / walkPlayerAction 02293B5C: the largest cLib_addCalcAngleS step of the turn (the body turns twice it) (tww the same) (3300) |
| 0x22 | `m22` | `mTurnMinStep` | setSpeedAndAngleNormal 0228E4C0 / searchNpcAction 02292C48 / escapeNpcAction 02293500 / walkPlayerAction 02293B5C: the smallest cLib_addCalcAngleS step of the turn (the body turns twice it) (tww the same) (2900) |
| 0x24 | `m24` | `mTurnScale` | setSpeedAndAngleNormal 0228E4C0 / searchNpcAction 02292C48 / escapeNpcAction 02293500 / walkPlayerAction 02293B5C: the cLib_addCalcAngleS scale of the turn (tww the same) (5) |
| 0x26 | `m26` | `mStickTurnScale` | waitPlayerAction 02293840: the cLib_addCalcAngleS scale of the turn toward the stick (tww the same) (25) |
| 0x28 | `m28` | `mStickTurnMaxStep` | waitPlayerAction 02293840: the largest step of the turn toward the stick (tww the same) (0x7FFF) |
| 0x2A | `m2A` | `mStickTurnMinStep` | waitPlayerAction 02293840: the smallest step of the turn toward the stick (tww the same) (1) |

### daNpc_Md_HIO3_l (l_HIO.mMirror; d_a_npc_md.h)

Round 3: the waist turn while Medli holds the harp mirror and the reflected light's place at the harp model (tww daNpc_Md_HIO3_c; vtable first, so the WWHD offsets equal the GameCube names). Each is set only by daNpc_Md_HIO3_c_ct 02294DFC (default in parentheses) and never written elsewhere in WWHD or tww; the reads are in our d_a_npc_md_event.cpp / d_a_npc_md_exec.cpp and match zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `m04` | `mWaistRateX` | lookBackWaist 0228DFF8: the waist's X turn is it * the stick's forward part (tww the same) (0.8) |
| 0x08 | `m08` | `mWaistRateY` | lookBackWaist 0228DFF8: the waist's Y turn is it * the stick's side part (tww the same) (0.2) |
| 0x0C | `m0C` | `mLightOffsetX` | setCollision 02287A74 / draw 02289E2C, while daMdStts_LIGHT_HIT is set: the reflected light's x offset from the harp model (tww the same) (-5) |
| 0x10 | `m10` | `mLightOffsetY` | setCollision 02287A74 / draw 02289E2C, while daMdStts_LIGHT_HIT is set: the reflected light's y offset from the harp model (tww the same) (-5) |
| 0x14 | `m14` | `mLightOffsetZ` | setCollision 02287A74 / draw 02289E2C, while daMdStts_LIGHT_HIT is set: the reflected light's z offset from the harp model (tww the same) (0) |
| 0x18 | `m18` | `mLightRotX` | setCollision 02287A74: the reflected light's X rotation at the harp model (tww the same) (0) |
| 0x1A | `m1A` | `mLightRotY` | setCollision 02287A74: the reflected light's Y rotation at the harp model (tww the same) (0) |
| 0x1C | `m1C` | `mLightRotZ` | setCollision 02287A74: the reflected light's Z rotation at the harp model (tww the same) (0) |
| 0x1E | `m1E` | `mWaistMaxRotXPos` | lookBackWaist 0228DFF8: mWaistRotX's target is it * the X turn when that is positive (tww the same) (3800) |
| 0x20 | `m20` | `mWaistMaxRotXNeg` | lookBackWaist 0228DFF8: mWaistRotX's target is it * the X turn when that is negative (tww the same) (5800) |
| 0x22 | `m22` | `mWaistMaxRotY` | lookBackWaist 0228DFF8: mWaistRotY's target is it * the Y turn (tww the same) (0x7FFF) |

Open (no name yet): `m24` (set by the constructor only, read nowhere in WWHD or tww); padding `_26`.

### daNpc_Md_HIO6_l (l_HIO.mShipRide; d_a_npc_md.h)

Round 3: Medli's attention distance and head / backbone limits while she rides the ship (tww daNpc_Md_HIO6_c; vtable first, so the WWHD offsets equal the GameCube names). Each is set only by daNpc_Md_HIO6_c_ct 02294CD0 (default in parentheses) and never written elsewhere in WWHD or tww; the reads are in our d_a_npc_md_event.cpp / d_a_npc_md_exec.cpp and match zeldaret/tww d217e2b. The head / backbone names follow mJntCtrl.setParam's parameters (as dNpc_HIO_c).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `m04` | `mShipMaxAttnDistXZ` | chkAttention 0228DAF4: the attention XZ distance while daMdStts_SHIP_RIDE is set (instead of mNpc.mMaxAttnDistXZ) (tww the same) (500) |
| 0x08 | `m08` | `mMaxHeadX` | execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's max_head_x (tww the same) (430) |
| 0x0A | `m0A` | `mMaxHeadY` | execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's max_head_y (tww the same) (9000) |
| 0x0C | `m0C` | `mMinHeadX` | execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's min_head_x (tww the same) (-5000) |
| 0x0E | `m0E` | `mMinHeadY` | execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's min_head_y (tww the same) (-9000) |
| 0x10 | `m10` | `mMaxBackboneX` | execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's max_backbone_x (tww the same) (0) |
| 0x12 | `m12` | `mMaxBackboneY` | execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's max_backbone_y (tww the same) (0) |
| 0x14 | `m14` | `mMinBackboneX` | execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's min_backbone_x (tww the same) (0) |
| 0x16 | `m16` | `mMinBackboneY` | execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's min_backbone_y (tww the same) (0) |


## daNpc_So_c (d_a_npc_so.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`; tww d217e2b decompiles src/d/actor/d_a_npc_so.cpp, whose code the evidence cites as "tww the same"; the cut code is WWHD only).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x7E4 | `m6CC` | `mMode` | modeProc 022DE7D8 stores the mode index and runs mode_data[it]; setScale 022DF060 / _execute 022DF6EC / _draw 022DFEDC test it (MODE_EVENT_TRIFORCE_e, MODE_HIDE_e) |
| 0x7E8 | `m6D0` | `mParamMsgNo` | getArg 022DE414: home.angle.x (1 when 0); getMsg 022E1CA8 returns it while mMapopenDone is set; next_msgStatus 022E179C: 0x32D4 goes on to it, and it goes on to 0x32CE / 0x32D2 / 0x32D6 (tww the same) |
| 0x7F0 | `m6D8` | `mTgHitTimer` | checkTgHit 022E160C counts it down and, on a hit, sets HIO m7C; cutMiniGameProc 022E3484 (case 7, inlined so_miniGameJump) sets the hit sphere only while it is 0 |
| 0x9E0 | `m868` | `mBlinkTimer` | setAnmSwimSpeed 022DF60C: counted down; at 0 mBtpFrame steps, and after the btp animation's end frame it restarts at 100 + rnd(100) with mBtpFrame 0 (tww _execute the same) |
| 0x9E4 | `m86C` | `mBtpFrame` | setAnmSwimSpeed 022DF60C advances it; _draw 022DFEDC enters the btp animation at this frame |
| 0xBF1 | `mA78` | `mEquip` | cutEquipProc 022E26A4 sets 1, cutUnequipProc 022E26E4 sets 0; _draw 022DFEDC draws the equipment while set |
| 0xBF2 | `mA79` | `mTagRndNum` | createInit 022DE884: cM_rndF(4.9f); _searchTagSo 022DDF20 picks the tag whose mRndNum equals it |
| 0xBF4 | `mA7C` | `mJumpRange` | _searchTagSo 022DDF20 copies the tag's getJumpRange(); modeHideInit 022E0084 clears it; _execute 022DF6EC / modeJump 022E01D8 use it |
| 0xBF8 | `mA80` | `mTagPos` | _searchTagSo 022DDF20 copies the found tag's position; modeHide 022E00B8 puts current.pos there |
| 0xC08 | `mA90` | `mSwimTimer` | modeSwimInit 022E0324 sets 30 + rnd(90); modeSwim 022E03A4 counts it down; modeEventFirstWaitInit 022E09B0 sets 150 |
| 0xC14 | `mA9C` | `mCutTimer` | cutJumpStart 022E1F90 sets 0xF, cutMiniGameProc 022E3484 30, cutTurnStart 022E3CF0; the cut procs count it down |
| 0xC24 | `mAAC` | `mInitialPos` | createInit 022DE884: current.pos after the 500 drop (tww mAAC = current.pos); modeEventFirstWait 022E09CC waits while the ship is within HIO m54 (XZ) of it (tww the same) |
| 0xC60 | `mAE8` | `mRippleCb` | _create 022DED84 constructs it as dPa_rippleEcallBack; _delete 022DEFEC ends it |
| 0xC74 | `mAFC` | `mTargetSpeedF` | _execute 022DF6EC eases speedF toward it (cLib_addCalc2 0.3, 4); the cut procs (cutSwimProc 022E1E74, ...) set it |
| 0xC78 | `mB00` | `mJumpSpeedY` | modeJumpInit 022E00D4 / cutJumpMapopenProc 022E2C5C: the jump's start speed.y; _execute 022DF6EC picks the pitch target (HIO m64-m6A) from speed.y against a quarter / half of it (tww the same) |
| 0xC7C | `mB04` | `mPathBlend` | modeSwim 022E03A4 / modeNearSwim 022E06A4: eased 0 -> 1 (cLib_addCalc2 0.01, 0.05) near the circle path, current.pos += (path pos - current.pos) * it; 0 when far; cutEatesaFirstProc 022E2830 clears it (tww the same) |
| 0xC80 | `mB08` | `mTargetScale` | setScale 022DF060: from the player's XZ distance (HIO m44 / m48 / m4C), 1 in MODE_EVENT_TRIFORCE; scale eases toward it (cLib_addCalcPos2); modeJumpInit 022E00D4 / cutMiniGameProc 022E3484 scale the jump speeds by it (tww the same) |
| 0xC84 | `mB0C` | `mBowEventDone` | modeEventBow 022E100C sets it when SO_BOW ends; modeDisappear 022E12B4 clears it; getMsg 022E1CA8 gives the minigame result messages (0x32DD..0x32E2) while set (tww the same) |
| 0xC88 | `mB10` | `mCirclePath` | the cut and mode code read it as SoCutCirclePath / SoModeCirclePath |
| 0xCAC | `mB34` | `mOffsetY` | setMtx 022DE434 translates the model by (0, it, 0); _execute 022DF6EC eases it with the mOffsetYCalc parameters |
| 0xCAC | `mB38` | `mOffsetYCalc` | _execute 022DF6EC: cLib_addCalc2(&mOffsetY, x, z, y) (target, scale, max step); offsetZero 022DE6AC sets (-1, 20, ...); the appear / dive cuts set it |
| 0xCBC | `mB44` | `mLookPos` | lookBack 022DF1CC copies the event cut's attention position; setAttention 022DF148 passes it to lookAtTarget as the target |
| 0xCC8 | `mB50` | `mLookMaxVel` | lookBack 022DF1CC eases it (cLib_addCalcAngleS2 4, 0x800) or clears it; setAttention 022DF148 passes it to lookAtTarget as maxVel |
| 0xCCC | `mB54` | `mHeadEyePos` | _nodeControl 022DDCB0: the head joint matrix (head turned) times (24, -16, 0); setAttention 022DF148 copies it into eyePos and looks from it + (0, 200, 0); cutEatesaFirstProc 022E2830 turns from it (tww the same) |
| 0xCD8 | `mB60` | `mHeadAttnPos` | _nodeControl 022DDCB0: the head joint's origin; setAttention 022DF148: attention_info.position = it + HIO mAttnYOffset, at least the water height (tww the same) |
| 0xCE4 | `mB6C` | `mStaffId` | the cut procs (cutSwimProc 022E1E74, cutJumpStart 022E1F90, ...) read the event data with it (getMyP(it, "Speed_y")) and end the cut with dComIfGp_evmng_cutEnd(it) |
| 0xCE8 | `mB70` | `m_order_event_num` | eventOrder 022DF46C orders by it: 1 / 2 the talk (1 orders the speak event), 3+ a_demo_name_tbl[it - 3] (assert 0x73E names it "m_order_event_num"); checkOrder 022DF3F0 clears it once the event starts; the modes set it (tww the same) |
| 0xCEC | `mB74` | `mMiniGameStep` | cutMiniGameProc 022E3484 switches on it (0 start, 2 / 4 pick the attention point, 6 jump, 7 the jump in the air) and steps it by 1; the landing (case 7, inlined so_miniGameJump) sets 0 again; cutMiniGameStart 022E2F8C clears it |
| 0xCF0 | `mB78` | `mMiniGameShotCount` | cutMiniGameStart 022E2F8C clears it; cutMiniGameProc 022E3484 adds 1, ends at 10; TWW getMiniGameRestArrow() = 10 - it |
| 0xCF4 | `mB7C` | `mMiniGameHitCount` | cutMiniGameProc 022E3484 (case 7, inlined so_miniGameJump) adds 1 (at most 10) on a target hit; next_msgStatus 022E179C pays it * 10 rupees; getMsg 022E1CA8 picks the result message by it (tww getMsg the same); cutMiniGameStart 022E2F8C clears it |
| 0xCF8 | `mB80` | `mMiniGameJumpCount` | cutMiniGameProc 022E3484 (case 7, inlined so_miniGameJump) adds 1 per landing and ends the minigame at 10; cutMiniGameStart 022E2F8C / modeDisappear 022E12B4 clear it |
| 0xD00 | `mB88` | `mMiniGameAttnOfsX` | cutMiniGameProc 022E3484: case 2 sets -500 + rnd(1000), case 4 moves it by -150 + rnd(300); the attention point is (it, 0, mMiniGameAttnOfsZ) turned by the ship's angle plus the ship's position |
| 0xD04 | `mB8C` | `mMiniGameAttnOfsZ` | cutMiniGameProc 022E3484: case 2 sets 1000 + rnd(400); the z of the attention point (mMiniGameAttnOfsX, 0, it), turned by the ship's angle plus the ship's position |
| 0xD08 | `mB90` | `mMiniGameTagPos` | _searchMinigameTagSo 022DDF9C copies the minigame tag's position; cutMiniGameWarpStart 022E3E08 puts the ship and the player there |
| 0xD14 | `mB9C` | `mMiniGameTagAngle` | _searchMinigameTagSo 022DDF9C copies the tag's shape_angle.y; cutMiniGameWarpStart 022E3E08 turns the ship and the player by it |
| 0xD18 | `mBA0` | `mShipReturnPos` | cutMiniGameWarpStart 022E3E08 keeps the ship's position; cutMiniGameReturnStart 022E40B8 puts the ship and the player back there |
| 0xD24 | `mBAC` | `mShipReturnAngle` | cutMiniGameWarpStart 022E3E08 keeps the ship's shape_angle.y; cutMiniGameReturnStart 022E40B8 restores it |
| 0xD26 | `mBAE` | `mMiniGameTagFound` | _searchMinigameTagSo 022DDF9C sets 1 when the minigame tag is found; next_msgStatus 022E179C: 0x32D6 offers the minigame (0x32D8 / 0x32DC) only while it (or HIO m30) is set (tww the same) |
| 0xD28 | `mBB0` | `mMapopenGoalPos` | cutJumpMapopenProc 022E2C5C: the ship's position moved sideways by -HIO m88 (step 1) / -m8C (step 2); current.pos chases it (cLib_chasePosXZ, HIO m90) |
| 0xD34 | `mBBC` | `mMapopenStep` | cutJumpMapopenStart 022E2C18 sets 0; cutJumpMapopenProc 022E2C5C runs steps 0 / 1 / 2 by it and steps it by 1 |
| 0xD38 | `mBC0` | `mCamEye` | so_camAroundLink (in cutMiniGameStart 022E2F8C) sets it from Link's position; cutMiniGameStart passes it to the camera reset as the eye |
| 0xD44 | `mBCC` | `mCamCenter` | so_camAroundLink (in cutMiniGameStart 022E2F8C) sets it 100 in front of Link, +50 up; cutMiniGameStart passes it to the camera reset as the center |
| 0xD50 | `mBD8` | `mMapopenDone` | modeEventMapopen 022E0F10 sets it when SO_MAPOPEN ends; modeDisappear 022E12B4 clears it; getMsg 022E1CA8 / next_msgStatus 022E179C give mParamMsgNo and its follow-ups while set (tww the same) |
| 0xD51 | `mBD9` | `mGetRupeeDone` | modeGetRupee 022E140C sets it when SO_GET_RUPEE ends; modeDisappear 022E12B4 clears it; getMsg 022E1CA8: 0x32E2 instead of 0x32DD while set (tww the same) |
| 0xD52 | `mBDA` | `mLookHeadOnly` | setAttention 022DF148 passes it to lookAtTarget as the head-only flag (d_npc.h lookAtTarget headOnly; tww the same); createInit 022DE884 0; cutEatesaFirstStart 022E27C4 / cutJumpMapopenStart 022E2C18 set 1, their procs clear it |
| 0xD53 | `mBDB` | `mNoPosMove` | _execute 022DF6EC runs fopAcM_posMoveF / CrrPos only while it is 0 (tww the same); modeHideInit 022E0084, modeDisappear 022E12B4, cutMiniGameWarpStart 022E3E08, cutMiniGameReturnStart 022E40B8 set 1; modeSwim 022E03A4, modeEventBow 022E100C, cutMiniGameStart 022E2F8C clear it |
| 0xD54 | `mBDC` | `mEsaXyEventIdx` | XyEventCB 022DE088 sets dComIfGp_evmng_getEventIdx("SO_ESA_XY") and returns it; modeEventEsa 022E0E60 waits for its end |
| 0xD56 | `mBDE` | `mCamInitDone` | initCam 022E3158: 0 stops the camera and places it around Link once, then 1; cutMiniGameStart 022E2F8C, cutMiniGameEndStart 022E4540, cutMiniGamePlTurnStart 022E45B4 clear it |
| 0xD58 | `mBE0` | `mPosMoveDelay` | createInit 022DE884 sets 30; _execute 022DF6EC counts it down and moves the actor only once it is 0 (tww the same) |

Open: `m6C4`, `mA8C`, `mA94`, `mAA0`, `mAB8` (padding, no use); `mBF0` (HD only: _draw 022DFEDC draws the second morf only while it is 0, never written in WWHD; tww has no such field), `mB84` (cutMiniGameProc 022E3484 / modeDisappear 022E12B4 write 0 and the hit check reads == 0, never set nonzero; tww modeDisappear only clears it).

### daNpc_So_HIO_c (l_HIO; d_a_npc_so.h; views SoModeHIO in d_a_npc_so_mode.h, SoCutHIO in d_a_npc_so_cut.h)

Round 3: the Fishman's HIO values (the layout is the GameCube one, so the offsets are the old names). Each is set only by daNpc_So_HIO_ct 022E48B4 (default in parentheses) and never written elsewhere in WWHD or tww; the reads are in our d_a_npc_so.cpp / _mode.cpp / _cut.cpp, and zeldaret/tww d217e2b reads the same fields where it has the function (the mapopen jump cut has no tww reader). The two views take the class's names. The comma-list declarations were split one per line first (own commit). The locals m48 / m4C in setScale 022DF060 keep their names (copies of mScaleDist / mMaxScale).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x2C | `m2C` | `mForceHudeDraw` | _draw 022DFEDC: hudeDraw runs when mEquip is set or it is (tww the same) (0) |
| 0x2E | `m2E` | `mForceMiniGameClear` | getMsg 022E1CA8: the mini-game counts as cleared when it is set or mMiniGameHitCount >= 10 (tww the same) (0) |
| 0x2F | `m2F` | `mForceGridArrived` | next_msgStatus 022E179C: message 0x32D0 takes the arrived-grid branch when the room's grid is saved as arrived or it is set (tww the same) (0) |
| 0x30 | `m30` | `mForceMiniGameTag` | next_msgStatus 022E179C: message 0x32D6 goes on as if a mini-game tag was found when it is set (tww the same) (0) |
| 0x31 | `m31` | `mNoDraw` | _draw 022DFEDC: the model (and fins) are drawn only while it is 0 (tww the same) (0) |
| 0x34 | `m34` | `mSwimAnmSpeed` | setAnmSwimSpeed 022DF60C: the swim animation speed is min(1, speed / 10) * it (tww the same) (3.0) |
| 0x38 | `m38` | `mSwimAnmSpeedMin` | setAnmSwimSpeed 022DF60C: the lower limit of that speed (tww cLib_minLimit) (0.9) |
| 0x3C | `m3C` | `mMiniGameSphRadius` | so_miniGameJump (in cutMiniGameProc 022E3484): mSph's radius while it jumps in the mini-game (tww SetR(l_HIO.m3C)) (100) |
| 0x44 | `m44` | `mNearScale` | setScale 022DF060: mTargetScale while the player is within mScaleDist (tww the same) (0.7) |
| 0x48 | `m48` | `mScaleDist` | setScale 022DF060: beyond this player distance mTargetScale grows with the distance (tww the same) (2000) |
| 0x4C | `m4C` | `mMaxScale` | setScale 022DF060: the growth divisor and the upper limit of mTargetScale (tww the same) (2.0) |
| 0x50 | `m50` | `mJumpSpeedYMax` | modeJumpInit 022E00D4 / cutMiniGameProc 022E3484: the jump's speed.y is capped at it (tww modeJumpInit the same) (150) |
| 0x54 | `m54` | `mFirstWaitShipDist` | modeEventFirstWait 022E09CC: it waits until the ship is this far (XZ) from mInitialPos (tww the same) (20000) |
| 0x5C | `m5C` | `mWaitAnmSwapRate` | setAnm 022DE6D4: at the end of a loop it swaps animation 3 / 5 when cM_rndF(100) < it (tww the same) (30) |
| 0x64 | `m64` | `mFallPitchFast` | _execute 022DF6EC: the pitch target while speed.y < -mJumpSpeedY / 2 (tww the same) (0x1F40) |
| 0x66 | `m66` | `mFallPitchSlow` | _execute 022DF6EC: the pitch target while speed.y < -mJumpSpeedY / 4 (tww the same) (0xFA0) |
| 0x68 | `m68` | `mRisePitchFast` | _execute 022DF6EC: the pitch target while speed.y > mJumpSpeedY / 2; modeJumpInit 022E00D4 / cutJumpStart 022E1F90 / cutJumpMapopenProc 022E2C5C / cutMiniGameProc 022E3484 start the jump with shape_angle.x = it (tww the same) (-0x1F40) |
| 0x6A | `m6A` | `mRisePitchSlow` | _execute 022DF6EC: the pitch target while speed.y > mJumpSpeedY / 4 (tww the same) (-0xFA0) |
| 0x7C | `m7C` | `mTgHitTime` | checkTgHit 022E160C: mTgHitTimer is set to it on a hit (tww the same) (5) |
| 0x80 | `m80` | `mMapopenGravity` | cutJumpMapopenProc 022E2C5C: gravity of the jump onto the ship (-1.4) |
| 0x84 | `m84` | `mMapopenJumpSpeedY` | cutJumpMapopenProc 022E2C5C: speed.y (and mJumpSpeedY) of that jump (18) |
| 0x88 | `m88` | `mMapopenNearDist` | cutJumpMapopenProc 022E2C5C: step 1's goal is this far to the ship's side (120) |
| 0x8C | `m8C` | `mMapopenFarDist` | cutJumpMapopenProc 022E2C5C: step 2's goal (where it dives) is this far to the ship's side (300) |
| 0x90 | `m90` | `mMapopenMoveStep` | cutJumpMapopenProc 022E2C5C: the XZ step of cLib_chasePosXZ toward both goals (15) |
| 0x94 | `m94` | `mNoFmapFish` | cutJumpMapopenProc 022E2C5C: the fish mark on the map (dComIfGp_fmapOpenFishOn) is set only while it is 0 (0) |

Open (no name yet): `m2D`, `m40`, `m58`, `m60`, `m6C`, `m70`, `m74`, `m78` (set only by the constructor, read nowhere in WWHD or tww); padding `_32`, `_7E`, `_95`.

## daNpcPhoto_c (d_a_npc_photo.h)

Old names are the GameCube offsets (zeldaret/tww `field_0x...`).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x86C | `field_0x6F8` | `mCounterCyl` | _execute 022CF17C sets the two cylinders at l_counter_pos (radius 110, height 150) while field_0x9C1 is 2; executeWait 022CF6C4 sets mCounterHit when either touches the player; createInit 022CC8D0 Set(l_cyl_src2) (tww the same) |
| 0xACC | `field_0x958` | `mCylOffset` | _execute 022CF17C / createInit 022CC8D0: the collision cylinder center is current.pos + it; executeWait 022CF6C4 sets (0, 0, npc dat 0x34) or clears it |
| 0xAF4 | `field_0x980` | `mpMsgList` | eventMesSetInit 022CD580 points it at a message number list (l_msg_1st_talk_photo, ...) and sets *it; next_msgStatus 022CFE34 / getMsg 022D031C step through it |
| 0xAF8 | `field_0x984` | `mTargetSpeed` | _execute 022CF17C chases speedF to it (cLib_chaseF 0.1 / 0.5); executeWalk 022CFB4C sets npc dat 0x3C (* 0x40), executeSetMode 022CD190 clears it |
| 0xAFC | `field_0x988` | `mCylR` | the constructor 022CC418 sets 60; createInit 022CC8D0 passes it to setCollision as the radius |
| 0xB00 | `field_0x98C` | `mMorf` | setAnm 022CCFAC uses it as the morf frames once when not negative, then sets -1; the constructor 022CC418 sets -1 |
| 0xB08 | `field_0x994` | `mLookHeadOnly` | lookBack 022CEF70 passes it to lookAtTarget as the head-only flag (d_npc.h lookAtTarget headOnly) unless talking with mBodyTurn; chkAttention 022CCD4C sets it (0 = turn the body), eventLookUb 022CE468 clears it (tww the same); same role as daNpcRoten_c::mLookHeadOnly |
| 0xB1A | `field_0x9A6` | `mCounterTalkEventIdx` | createInit 022CC8D0 copies mPhotoCounterTalk0EventIdx; eventMove 022CE97C waits for that event's end and then resets the talk (tww the same) |
| 0xB1C | `field_0x9A8` | `mWaitTimer` | executeSetMode 022CD190 sets npc dat 0x4A / 0x4E + random; executeWait 022CF6C4 counts it down before executeSetMode(3); createInit 022CC8D0 sets 1 (tww the same); same role as daNpcRoten_c::mWaitTimer |
| 0xB20 | `field_0x9AC` | `mHomeTurnDelay` | chkAttention 022CCD4C sets 30 when the player leaves the attention range and counts it down before turning back to mTargetAngleY (tww the same); same role as daNpcRoten_c::mHomeTurnDelay |
| 0xB22 | `field_0x9AE` | `mTargetAngleY` | the constructor 022CC418 sets home.angle.y; eventPosSetInit 022CD764 / eventTurnToPlayerInit 022CDCDC set it; eventTurnToPlayer 022CE434 ends when current.angle.y equals it |
| 0xB26 | `field_0x9B2` | `mMaxAttnAngleY` | createInit 022CC8D0 copies npc dat 0x28; chkAttention 022CCD4C: the player is in attention while the angle to them is below it (+0x71C while mInAttention) (tww the same) |
| 0xB2A | `field_0x9B6` | `mLookMaxVelTarget` | chkAttention 022CCD4C / executeWalk 022CFB4C set npc dat 0x16 / 0x18; lookBack 022CEF70 eases mLookMaxVel toward it |
| 0xB2A | `field_0x9B8` | `mLookMaxVel` | lookBack 022CEF70 eases it toward mLookMaxVelTarget (cLib_addCalcAngleS2 4, 0x800), else clears it, and passes it to lookAtTarget as maxVel |
| 0xB2E | `field_0x9BA` | `mLookAngleY` | lookBack 022CEF70: the target angle in look mode 2; chkAttention 022CCD4C sets mTargetAngleY, executeWalk 022CFB4C / executeTurn 022CFD7C the path angle (tww the same); same role as daNpcRoten_c::mLookAngleY |
| 0xB30 | `field_0x9BC` | `mTalking` | checkOrder 022CD3F8 sets it when the talk starts; executeCommon 022CF670 switches to the talk mode while 1 and returns it; executeTalk 022CFA00 / eventMove 022CE97C / eventOrder 022CEB58 clear it (tww the same); same role as daNpcRoten_c::mTalking |
| 0xB31 | `field_0x9BD` | `mInAttention` | chkAttention 022CCD4C: 1 once the player is in the attention range (distance +40 and angle +0x71C while set), 0 when they leave (tww the same); executeWait 022CF6C4 tests it; same pattern as daNpcRoten_c::mInAttention |
| 0xB32 | `field_0x9BE` | `mOrderEvent` | eventOrder 022CEB58 orders by it; checkOrder 022CD3F8 clears 3-6 once PHOTO_LINK_BACK / GET_ITEM / GET_ITEM2 / GET_PHOTO starts; executeWait 022CF6C4 / eventMove 022CE97C set it (tww the same); after daNpc_Cb1_c::mOrderEvent |
| 0xB34 | `field_0x9C0` | `mExecuteMode` | executeSetMode 022CD190 stores the mode; _execute 022CF17C calls moveProc[it]; chkAttention 022CCD4C tests 0 (tww the same); same role as daNpcRoten_c::mExecuteMode |
| 0xB3B | `field_0x9C7` | `mModeInTalk` | _execute 022CF17C runs the mode (not eventMove) during a talk only while set; the constructor 022CC418, executeWait 022CF6C4, eventMove 022CE97C, eventOrder 022CEB58 set it, XyEventCB 022CC690 clears it (tww the same) |
| 0xB3C | `field_0x9C8` | `mAnmIdx` | setAnm 022CCFAC stores the animation index; createHeap 022CC160 loads the bck by it; setAnmTbl 022CD0B8 / _execute 022CF17C test it (tww the same); same role as daNpcRoten_c::mAnmIdx |
| 0xB3E | `field_0x9CA` | `mAnmLoopCount` | setAnmTbl 022CD0B8 sets the table entry's count (+0x02); playAnm 022CEE30 counts it down before the next entry |
| 0xB41 | `field_0x9CD` | `mCounterHit` | executeWait 022CF6C4 clears it, then sets it when an mCounterCyl touches the player; chkAttention 022CCD4C treats the player as in attention while set (tww the same) |
| 0xB44 | `field_0x9D0` | `mpMsgCameraList` | eventMesSetInit 022CD580 points it at the message's camera index list (or 0) and steps it; setMsgCamera 022CDE3C reads the camera from it (tww the same) |
| 0xB4A | `field_0x9D6` | `mLookMode` | lookBack 022CEF70 switches on it: 1 looks at mLookAtPos, 2 turns to mLookAngleY, 0 neither; chkAttention 022CCD4C / eventLookUb 022CE468 set it (tww the same); same role as daNpcRoten_c::mLookMode |
| 0xB4B | `field_0x9D7` | `mBodyTurn` | createInit 022CC8D0 copies npc dat 0x52, eventMove 022CE97C sets it after PHOTO_DATE_UB4; while set chkAttention 022CCD4C and lookBack 022CEF70 (when talking) turn the body (mLookHeadOnly 0) (tww the same); same role as daNpcRoten_c::mBodyTurn |
| 0xB4C | `field_0x9D8` | `mLookAtPlayer` | createInit 022CC8D0 copies npc dat 0x53, eventMove 022CE97C sets it after PHOTO_DATE_UB4; chkAttention 022CCD4C: while 0 the player in range only turns him to mTargetAngleY instead of being looked at (tww the same) |

The second photo batch names the attention / look / order state after the same roles in daNpcRoten_c (the same NPC template; tww d217e2b decompiles both with the same code).

Third photo batch: the event and animation bit fields and talk2's message camera flags.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0xB3A | `field_0x9C6` | `mEventFlags` | bit field: next_msgStatus 022CFE34 sets 0x1 / 0x2 / 0x20 and getMsg 022D031C 0x4; when the box closes executeTalk 022CFA00 turns 0x1 / 0x2 into the item event (mOrderEvent 4) and 0x4 into the spit event (3), else sets 0x40, and talk2 022CDFAC turns 0x20 into 0x40; eventMove 022CE97C sets 0x40 / 0x50; eventOrder 022CEB58 clears 0x40 and resets the event; executeWait 022CF6C4 tests 0x10 (tww the same); same role as daNpcRoten_c::mEventFlags / daNpcPeople_c::mEventFlags |
| 0xB3D | `field_0x9C9` | `mAnmFlags` | bit field: 0x1 playAnm 022CEE30 sets it when the animation table ends (cleared each frame); 0x4 setAnmTbl 022CD0B8 keeps the table while set, executeTalk 022CFA00 sets it with the spit table, playAnm clears it when the animation ends; 0x80 setAnmFromMsgTag 022CE7B0 sets it and playTexPatternAnm 022CED84 stops the blink while set, eventClrHanmeInit 022CDD50 / eventOrder 022CEB58 clear it (tww the same) |
| 0xB48 | `field_0x9D4` | `mMsgCameraSet` | setMsgCamera 022CDE3C: 1 when it set the message camera (dCamera_Set), 0 when it restarted the normal camera; talk2 022CDFAC restarts the camera at the box close while set (tww the same) |
| 0xB49 | `field_0x9D5` | `mMsgCameraPending` | talk2 022CDFAC sets 1 when a message is set and on every status but typing; on MSG_TYPING it clears it and calls setMsgCamera once (tww the same) |

Round 2:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0xB24 | `field_0x9B0` | `mCutCounter` | the counter of two event cuts: eventSetEyeInit 022CDC8C clears it, eventSetEye 022CE33C steps it by 0x400 as the sine phase of the eye sweep; eventGetPhotoInit 022CDD90 sets 10, eventGetPhoto 022CE44C counts it down and ends the cut at 0 (tww the same) |
| 0xB35 | `field_0x9C1` | `mPlaceState` | Lenzo's situation: the constructor 022CC418 0; phase_1 022CC4F8 4 (date, tww isDate() == 4: _draw 022CF534 registers the snap object only then); eventPosSetInit 022CD764 1; executeWait 022CF6C4 3 (player went down); executeWalk 022CFB4C 2 (at the counter, mCounterCyl on); getMsg 022D031C picks messages by it (tww the same); also d_a_tag_photo.h PhotoLenzo view |
| 0xB40 | `field_0x9CC` | `mPhotoAccepted` | next_msgStatus 022CFE34 sets it at message-list marker 6, where the shown picture is erased (dPb_erasePicture) and mEventFlags 0x1 orders the item event; never read in WWHD (HD has no l_msg_3rd_order_end); tww getMsg gives l_msg_3rd_order_end instead of l_msg_not_color while it is set |

Open: `field_0x9C2` (the constructor false, phase_1 022CC4F8 true, never read in WWHD or tww), `field_0x9C3` (the constructor 0 only), `field_0x9C4` (no use in WWHD or tww).

### NpcDatStruct, sPhotoAnmDat, PsoData (d_a_npc_photo.h; Lenzo's npc data, animation table entry and photo object)

Old names are the GameCube offsets (zeldaret/tww `field_0x...`, unnamed there).

| Struct | Old | New | Evidence |
|---|---|---|---|
| NpcDatStruct | `field_0x00` | `mPlayerEyeOffsetY` | chkAttention 022CCD4C: passed to dNpc_playerEyePos, whose look target is the player's head top + (0, it, 0) (tww the same); same role as daNpcRoten_c NpcDatStruct::mPlayerEyeOffsetY |
| NpcDatStruct | `field_0x04` | `mMaxHeadX` | _execute 022CF17C: m_jnt.setParam's maxHeadX (5th argument; d_npc.h setParam order) (tww the same) |
| NpcDatStruct | `field_0x06` | `mMaxHeadY` | _execute 022CF17C: m_jnt.setParam's maxHeadY (6th argument) (tww the same) |
| NpcDatStruct | `field_0x08` | `mMaxBackboneX` | _execute 022CF17C: m_jnt.setParam's maxBackboneX (1st argument) (tww the same) |
| NpcDatStruct | `field_0x0A` | `mMaxBackboneY` | _execute 022CF17C: m_jnt.setParam's maxBackboneY (2nd argument) (tww the same) |
| NpcDatStruct | `field_0x0C` | `mMinHeadX` | _execute 022CF17C: m_jnt.setParam's minHeadX (7th argument) (tww the same) |
| NpcDatStruct | `field_0x0E` | `mMinHeadY` | _execute 022CF17C: m_jnt.setParam's minHeadY (8th argument) (tww the same) |
| NpcDatStruct | `field_0x10` | `mMinBackboneX` | _execute 022CF17C: m_jnt.setParam's minBackboneX (3rd argument) (tww the same) |
| NpcDatStruct | `field_0x12` | `mMinBackboneY` | _execute 022CF17C: m_jnt.setParam's minBackboneY (4th argument) (tww the same) |
| NpcDatStruct | `field_0x14` | `mMaxTurnStep` | _execute 022CF17C: m_jnt.setParam's maxTurnStep (9th argument) (tww the same) |
| NpcDatStruct | `field_0x16` | `mMaxHeadTurnVel` | chkAttention 022CCD4C: mLookMaxVelTarget, the speed lookBack eases the look toward (tww the same); same role as daNpcRoten_c NpcDatStruct::mMaxHeadTurnVel |
| NpcDatStruct | `field_0x18` | `mWalkHeadTurnVel` | executeWalk 022CFB4C: mLookMaxVelTarget while walking the path (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkHeadTurnVel |
| NpcDatStruct | `field_0x1C` | `mAttnOffsetX` | _execute 022CF17C: attention_info.position = current.pos + the vector (it, mAttnOffsetY, mAttnOffsetZ) Y-rotated by current.angle.y (tww the same) |
| NpcDatStruct | `field_0x20` | `mAttnOffsetY` | _execute 022CF17C: the Y of the attention position offset (see mAttnOffsetX) (tww the same) |
| NpcDatStruct | `field_0x24` | `mAttnOffsetZ` | _execute 022CF17C: the Z of the attention position offset (see mAttnOffsetX) (tww the same) |
| NpcDatStruct | `field_0x28` | `mMaxAttnAngleY` | createInit 022CC8D0 copies it into daNpcPhoto_c::mMaxAttnAngleY (tww the same) |
| NpcDatStruct | `field_0x2C` | `mMaxAttnDistXZ` | chkAttention 022CCD4C: the player is in attention while closer than it (+40 while mInAttention) (tww the same); same role as daNpcRoten_c NpcDatStruct::mMaxAttnDistXZ |
| NpcDatStruct | `field_0x30` | `mEyeYOffset` | _execute 022CF17C: mEyePos = current.pos + (0, it, 0), then eyePos (tww the same); same role as daNpcRoten_c NpcDatStruct::mEyeYOffset |
| NpcDatStruct | `field_0x34` | `mCylOffsetZ` | executeWait 022CF6C4: mCylOffset.z (with the 150 radius) until event bit 0x1701 is set (tww the same) |
| NpcDatStruct | `field_0x38` | `mWalkAnmRate` | _execute 022CF17C: while walking (mAnmIdx 3) the morf's play speed is speedF * it, at least 0.5 (tww the same); same role as daNpcPeople_c__l_npc_dat::mWalkAnmRate |
| NpcDatStruct | `field_0x3C` | `mWalkSpeed` | executeWalk 022CFB4C: mTargetSpeed while walking the path (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkSpeed |
| NpcDatStruct | `field_0x40` | `mClimbSpeedRate` | executeWalk 022CFB4C: mTargetSpeed = mWalkSpeed * it while the next path point is more than 400 higher (tww the same) |
| NpcDatStruct | `field_0x4A` | `mWaitTimeMin` | executeSetMode 022CD190 (mode 0): mWaitTimer = it + rnd(mWaitTimeMax - it) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWaitTimeMin |
| NpcDatStruct | `field_0x4C` | `mWaitTimeMax` | executeSetMode 022CD190 (mode 0): mWaitTimer = mWaitTimeMin + rnd(it - mWaitTimeMin) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWaitTimeMax |
| NpcDatStruct | `field_0x4E` | `mWalkTimeMin` | executeSetMode 022CD190 (modes 2 / 3): mWaitTimer = it + rnd(mWalkTimeMax - it) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkTimeMin |
| NpcDatStruct | `field_0x50` | `mWalkTimeMax` | executeSetMode 022CD190 (modes 2 / 3): mWaitTimer = mWalkTimeMin + rnd(it - mWalkTimeMin) (tww the same); same role as daNpcRoten_c NpcDatStruct::mWalkTimeMax |
| NpcDatStruct | `field_0x52` | `mBodyTurn` | createInit 022CC8D0 copies it into daNpcPhoto_c::mBodyTurn (tww the same) |
| NpcDatStruct | `field_0x53` | `mLookAtPlayer` | createInit 022CC8D0 copies it into daNpcPhoto_c::mLookAtPlayer (tww the same) |
| sPhotoAnmDat | `field_0x00` | `mAnmIdx` | setAnmTbl 022CD0B8: the entry's animation index for setAnm (0xFF ends the table), compared with daNpcPhoto_c::mAnmIdx; playAnm 022CEE30 replays it (tww the same); same code as sRotenAnmDat::mAnmIdx |
| sPhotoAnmDat | `field_0x01` | `mMorf` | setAnmTbl 022CD0B8: the entry's morf frames, passed to setAnm (tww the same); same code as sRotenAnmDat::mMorf |
| sPhotoAnmDat | `field_0x02` | `mLoopCount` | setAnmTbl 022CD0B8: the entry's count, copied into mAnmLoopCount; 0 or less loops (tww the same); same code as sRotenAnmDat::mLoopCount |
| PsoData | `field_0x00` | `mOffsetX` | _draw 022CF534: the snap object's center = current.pos + (mOffsetX, mOffsetY, mOffsetZ) (tww the same); same role as PsoData_l::mOffsetX (people) |
| PsoData | `field_0x04` | `mOffsetY` | _draw 022CF534: the Y of the snap object's offset (see mOffsetX) (tww the same) |
| PsoData | `field_0x08` | `mOffsetZ` | _draw 022CF534: the Z of the snap object's offset (see mOffsetX) (tww the same) |
| PsoData | `field_0x0C` | `mRadius` | _draw 022CF534: dSnap_Obj_SetGeo's radius (tww d_snap.h SetGeo(center, radius, height, ..)) |
| PsoData | `field_0x10` | `mHeight` | _draw 022CF534: dSnap_Obj_SetGeo's height (tww d_snap.h SetGeo) |
| PsoData | `field_0x14` | `mAngleY` | _draw 022CF534: + current.angle.y, dSnap_Obj_SetGeo's angle (tww the same) |

Round 2:

| Struct | Old | New | Evidence |
|---|---|---|---|
| PsoData | `field_0x17` | `mCylDivNum` | _draw 022CF534 passes it to dSnap_Obj_SetInf as the second u8; tww dSnap_Obj::SetInf stores it in field_0x19, the cylinder segment count CalcArea2D / draw loop over (for i < it, cylvtx[i]) |

Open: NpcDatStruct `field_0x44`, `field_0x46`, `field_0x48` (no use in WWHD or tww); PsoData `field_0x16` (dSnap_Obj_SetInf's first u8; tww stores it in dSnap_Obj field_0x1a, which nothing reads).


## daNpcRoten_c (d_a_npc_roten.h)

Old names are the GameCube offsets (zeldaret/tww `field_0x...`; tww d217e2b decompiles src/d/actor/d_a_npc_roten.cpp with the same code, cited as "tww the same").

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x7EC | `field_0x6D4` | `mpHeadModel` | createHeap 022D2CCC: the head morf's model; _draw 022D5790 sets its light / TEV colors |
| 0x7F0 | `field_0x6D8` | `mpHeadMorf` | createHeap 022D2CCC creates it (mDoExt_McaMorf) from the head model data and head animation; executeWindInit 022D5FC8 / execute play it |
| 0x86C | `field_0x6F8` | `mItemPid` | eventSetItemInit 022D4330 stores the created item's process ID; privateCut 022D4828 / eventClrItem 022D45B4 find the item with it (SearchByID) |
| 0x87C | `field_0x708` | `mLookPos` | chkAttention 022D3B9C copies the event cut's attention position; lookBack 022D5374 passes it to lookAtTarget as the target |
| 0x888 | `field_0x714` | `mHandItemPos` | privateCut 022D4828: during ANM_WATASU (frame 60+) the left-hand joint position + (20, -30, -30), where the present item is shown; during ANM_ITEMWAIT copied into mLookPos |
| 0x894 | `field_0x720` | `mWindVec` | no use in this decomp; TWW getWindVec() { return field_0x720; } (CC0 zeldaret/tww d217e2b) |
| 0xAFC | `field_0x988` | `mpAnmTbl` | setAnmTbl 022D44D0 stores the table; playAnm 022D5174 steps to the next entry |
| 0xB00 | `field_0x98C` | `mpMsgList` | eventMesSetInit 022D40A4 points it at a message number list; next_msgStatus 022D60DC / getMsg 022D6304 step through it |
| 0xB04 | `field_0x990` | `mSpeedF` | _execute 022D5580 sets speedF to it; executeWalk 022D5C54 sets the walk speed, executeSetMode 022D3FDC clears it |
| 0xB08 | `field_0x994` | `mMorf` | setAnm 022D43B0 uses it as the morf frames when not negative; the constructor 022D30F4 sets -1 |
| 0xB0C | `field_0x998` | `mAnmPlayMode` | setAnm 022D43B0 stores the play mode it passes to mpMorf->setAnm (setAnmTbl 022D44D0: 0 for counted entries, 2 for looping ones); setAnmTbl restarts an unchanged animation only while it is 0 (tww the same) |
| 0xB10 | `field_0x99C` | `mLookHeadOnly` | lookBack 022D5374 passes it to lookAtTarget as the head-only flag (d_npc.h lookAtTarget headOnly) unless talking with mBodyTurn; chkAttention 022D3B9C sets 1 / 0 (0 = turn the body), executeWalk 022D5C54 / executeTurn 022D5F10 clear it; the constructor 022D30F4 sets 1 (tww the same) |
| 0xB12 | `field_0x99E` | `mExchange1stEventIdx` | createInit 022D3710: dComIfGp_evmng_getEventIdx("ROTEN_EXCHANGE_1ST"); XyEventCB 022D3388 returns it |
| 0xB14 | `field_0x9A0` | `mExchange2ndEventIdx` | createInit 022D3710: dComIfGp_evmng_getEventIdx("ROTEN_EXCHANGE_2ND"); checkOrder 022D3EA0 / eventMove 022D4C40 test it |
| 0xB16 | `field_0x9A2` | `mChangeItemEventIdx` | createInit 022D3710: dComIfGp_evmng_getEventIdx("ROTEN_CHANGE_ITEM"); checkOrder 022D3EA0 / eventMove 022D4C40 test it |
| 0xB18 | `field_0x9A4` | `mGetMapEventIdx` | createInit 022D3710: dComIfGp_evmng_getEventIdx("ROTEN_GET_MAP"); XyEventCB 022D3388 returns it |
| 0xB1A | `field_0x9A6` | `mWaitTimer` | executeWaitInit 022D5970 sets npc dat 0x4C + random; executeWait 022D5A48 counts it down before walking the path |
| 0xB1C | `field_0x9A8` | `mHomeTurnDelay` | chkAttention 022D3B9C sets 30 when the player leaves the attention range and, without a path, counts it down before turning back to home.angle.y (tww the same) |
| 0xB1E | `field_0x9AA` | `mBlinkTimer` | initTexPatternAnm 022D2BB8 clears it; playTexPatternAnm 022D50D4 counts it down and restarts the head texture pattern |
| 0xB20 | `field_0x9AC` | `mLookMaxVelTarget` | lookBack 022D5374 sets npc dat 0x16 (executeWalk 022D5C54: 0x18) and eases mLookMaxVel toward it |
| 0xB20 | `field_0x9AE` | `mLookMaxVel` | lookBack 022D5374 eases it toward mLookMaxVelTarget (cLib_addCalcAngleS2 4, 0x800), else clears it, and passes it to lookAtTarget as maxVel |
| 0xB24 | `field_0x9B0` | `mLookAngleY` | lookBack 022D5374: the target angle in look mode 2; executeWalk 022D5C54 / executeTurn 022D5F10 set the path angle, chkAttention 022D3B9C home.angle.y (tww the same) |
| 0xB26 | `field_0x9B2` | `mEventFlags` | bit field: eventOrder 022D4EF0 sets 0x4000 when it orders an event and eventMove 022D4C40 waits for its end while set; XyEventCB 022D3388 sets 0xC000 (during a talk, _execute 022D5580 runs eventMove instead of the mode while 0x8000 is set); next_msgStatus 022D60DC sets 8 / 0x10 / 0x20 / 0x40, which getMsg 022D6304 and eventMove turn into the follow-up messages and events (tww the same) |
| 0xB28 | `field_0x9B4` | `mTalking` | checkOrder 022D3EA0 sets 1 when the talk starts; executeCommon 022D58F4 switches to MODE_TALK while 1 and returns it; executeTalk 022D5ABC / eventMove 022D4C40 clear it; chkAttention 022D3B9C widens the angle while set (tww the same) |
| 0xB29 | `field_0x9B5` | `mInAttention` | chkAttention 022D3B9C: 1 once the player is in the look range (distance +40 and angle +0x71C while set), 0 when they leave; executeWalk 022D5C54 stops and executeWait 022D5A48 does not start walking while set (tww the same); same pattern as daNpcMt_c::mInAttention |
| 0xB2A | `field_0x9B6` | `mCanTalk` | chkAttention 022D3B9C: 1 while the player is in the talk range (or the event attention is on), else 0; executeCommon 022D58F4 sets mOrderEvent 1 (can talk) while it is set outside MODE_WIND (tww the same) |
| 0xB2B | `field_0x9B7` | `mOrderEvent` | eventOrder 022D4EF0 orders by it: 1 / 2 the talk (2 orders the speak event), 3-6 the ROTEN_EXCHANGE_1ST / _2ND / CHANGE_ITEM / GET_MAP events; checkOrder 022D3EA0 clears 3-6 once the event starts; executeCommon 022D58F4 / eventMove 022D4C40 set it (tww the same); after daNpc_Cb1_c::mOrderEvent |
| 0xB2C | `field_0x9B8` | `mBtpFrame` | initTexPatternAnm 022D2BB8 clears it; playTexPatternAnm 022D50D4 advances it; _draw 022D5790 enters the btp animation at it |
| 0xB2F | `field_0x9BB` | `mExecuteMode` | _execute 022D5580 calls moveProc[it]; executeSetMode 022D3FDC sets it; executeCommon 022D58F4 tests MODE_WIND |
| 0xB30 | `field_0x9BC` | `mResFlag` | phase_1 022D3234 sets 1 once the resources are loaded (GameCube setResFlag(1)) |
| 0xB32 | `field_0x9BE` | `mTradeItemIdx` | getMsg 022D6304 / XyEventCB 022D3388: the shown item's number minus dItemNo_TOWN_FLOWER; eventMesSetInit 022D40A4 / eventSetItemInit 022D4330 / eventGetItemInit 022D4620 / next_msgStatus 022D60DC index the exchange messages and items by it (tww the same) |
| 0xB34 | `field_0x9C0` | `mAnmIdx` | setAnm 022D43B0 stores the animation index (l_bck_ix_tbl); createHeap 022D2CCC loads the bck by it; setAnmTbl 022D44D0 / privateCut 022D4828 (ANM_WATASU, ANM_ITEMWAIT) test it (tww the same) |
| 0xB35 | `field_0x9C1` | `mAnmTblFlags` | bit 1: playAnm 022D5174 sets it when the animation table reaches its end; setAnmTbl 022D44D0 clears it; executeWind 022D6074 goes back to MODE_WAIT once it is set (tww the same) |
| 0xB36 | `field_0x9C2` | `mAnmLoopCount` | setAnmTbl 022D44D0 sets the table entry's count; playAnm 022D5174 counts it down before the next entry |
| 0xB37 | `field_0x9C3` | `mCutNo` | privateCut 022D4828 stores the event cut index and switches on it |
| 0xB38 | `field_0x9C4` | `mLookMode` | lookBack 022D5374 switches on it: 1 looks at mLookPos, 2 turns to mLookAngleY, 0 neither; chkAttention 022D3B9C, executeWalk 022D5C54, executeTurn 022D5F10, privateCut 022D4828 set it (tww the same) |
| 0xB3A | `field_0x9C6` | `mBodyTurn` | checkOrder 022D3EA0 (talk start) and eventClrItemInit 022D45B4 set 1; executeTalk 022D5ABC / eventMove 022D4C40 / privateCut 022D4828 clear it; while set, chkAttention 022D3B9C and lookBack 022D5374 (when talking) turn the body (mLookHeadOnly 0, setTrn) (tww the same) |
| 0xB3B | `field_0x9C7` | `mNoBackboneTurn` | _execute 022D5580 passes 0 for the four backbone limits of m_jnt.setParam while set; privateCut 022D4828 sets 1 (ANM_ITEMWAIT), eventClrItemInit 022D45B4 clears it (tww the same) |
| 0xB3C | `field_0x9C8` | `mDrawHead` | _draw 022D5790 puts the head model on the head joint and draws it only while set; the constructor 022D30F4 sets 1, nothing clears it (tww the same) |
| 0xB3E | `field_0x9CA` | `mWindHit` | setCollisionH 022D5238 sets it (and MODE_WIND) on the first TG hit of the head sphere; chkAttention 022D3B9C ignores the player while set; executeWind 022D6074 clears it when the wind animation table ends (tww the same) |

Open: `field_0x6FC` (the constructor 022D30F4 sets fpcM_ERROR_PROCESS_ID, never read; tww the same), `field_0x9C9` (the constructor writes 0, never read), `field_0x9BF` (no use; tww has none either).

The data structs (own commit). NpcDatStruct (l_npc_dat, one per merchant) starts like dNpc_HIO_c (the m_jnt limits in setParam order), so those fields take dNpc_HIO_c's / setParam's names. Open: NpcDatStruct `field_0x1A`, `field_0x40`, `field_0x48`, `field_0x4A` (no use; tww has none either); SaveDatStruct `field_0x04` (no use; tww has none either).

| Struct offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| NpcDatStruct 0x00 | `field_0x00` | `mPlayerEyeOffsetY` | chkAttention 022D3B9C: passed to dNpc_playerEyePos, whose look target is the player's head top + (0, it, 0) (tww dNpc_playerEyePos(f32 offsetY)) |
| NpcDatStruct 0x04 | `field_0x04` | `mMaxHeadX` | _execute 022D5580: m_jnt.setParam's maxHeadX (d_npc.h setParam order; the same layout as dNpc_HIO_c mMaxHeadX) |
| NpcDatStruct 0x06 | `field_0x06` | `mMaxBackboneX` | _execute 022D5580: m_jnt.setParam's maxBackboneX (0 while mNoBackboneTurn) |
| NpcDatStruct 0x08 | `field_0x08` | `mMaxHeadY` | _execute 022D5580: m_jnt.setParam's maxHeadY |
| NpcDatStruct 0x0A | `field_0x0A` | `mMaxBackboneY` | _execute 022D5580: m_jnt.setParam's maxBackboneY (0 while mNoBackboneTurn) |
| NpcDatStruct 0x0C | `field_0x0C` | `mMinHeadX` | _execute 022D5580: m_jnt.setParam's minHeadX |
| NpcDatStruct 0x0E | `field_0x0E` | `mMinBackboneX` | _execute 022D5580: m_jnt.setParam's minBackboneX (0 while mNoBackboneTurn) |
| NpcDatStruct 0x10 | `field_0x10` | `mMinHeadY` | _execute 022D5580: m_jnt.setParam's minHeadY |
| NpcDatStruct 0x12 | `field_0x12` | `mMinBackboneY` | _execute 022D5580: m_jnt.setParam's minBackboneY (0 while mNoBackboneTurn) |
| NpcDatStruct 0x14 | `field_0x14` | `mMaxTurnStep` | _execute 022D5580: m_jnt.setParam's maxTurnStep |
| NpcDatStruct 0x16 | `field_0x16` | `mMaxHeadTurnVel` | chkAttention 022D3B9C: mLookMaxVelTarget, the speed lookBack eases the look toward (dNpc_HIO_c mMaxHeadTurnVel at the same offset) |
| NpcDatStruct 0x18 | `field_0x18` | `mWalkHeadTurnVel` | executeWalk 022D5C54: mLookMaxVelTarget while walking the path |
| NpcDatStruct 0x1C | `field_0x1C` | `mAttnYOffset` | _execute 022D5580: attention_info.position = current.pos + (0, it, 0) (tww the same) |
| NpcDatStruct 0x20 | `field_0x20` | `mMaxAttnAngleY` | chkAttention 022D3B9C: the player is in attention while the angle to them is below it (+0x71C while mInAttention) |
| NpcDatStruct 0x24 | `field_0x24` | `mMaxAttnDistXZ` | chkAttention 022D3B9C: the player is in attention while closer than it (+40 while mInAttention) |
| NpcDatStruct 0x28 | `field_0x28` | `mEyeYOffset` | _execute 022D5580: eyePos = current.pos + (0, it, 0) (tww the same) |
| NpcDatStruct 0x2C | `field_0x2C` | `mCylRadius` | createInit 022D3710 / _execute 022D5580: setCollision(it, 200), the body cylinder's radius (tww the same) |
| NpcDatStruct 0x30 | `field_0x30` | `mCyl2Radius` | setCollisionB 022D35C0: radius of mCyl2 |
| NpcDatStruct 0x34 | `field_0x34` | `mCyl2Dist` | setCollisionB 022D35C0: mCyl2 sits at the joint position pushed it along the angle from the actor |
| NpcDatStruct 0x38 | `field_0x38` | `mSphRadius` | setCollisionH 022D5238: radius of the head sphere mSph |
| NpcDatStruct 0x3C | `field_0x3C` | `mSphOffsetX` | setCollisionH 022D5238: mSph sits at the head joint matrix times (it, 0, 0) |
| NpcDatStruct 0x44 | `field_0x44` | `mWalkSpeed` | executeWalk 022D5C54: mSpeedF while walking the path |
| NpcDatStruct 0x4C | `field_0x4C` | `mWaitTimeMin` | executeWaitInit 022D5970: mWaitTimer = it + rnd(mWaitTimeMax - it) |
| NpcDatStruct 0x4E | `field_0x4E` | `mWaitTimeMax` | executeWaitInit 022D5970: mWaitTimer = mWaitTimeMin + rnd(it - mWaitTimeMin) |
| NpcDatStruct 0x50 | `field_0x50` | `mWalkTimeMin` | executeWalkInit 022D5B7C / executeTurnInit 022D5DC0: mWaitTimer = it + rnd(mWalkTimeMax - it) |
| NpcDatStruct 0x52 | `field_0x52` | `mWalkTimeMax` | executeWalkInit 022D5B7C / executeTurnInit 022D5DC0: mWaitTimer = mWalkTimeMin + rnd(it - mWalkTimeMin) |
| sRotenAnmDat 0x00 | `field_0x00` | `mAnmIdx` | setAnmTbl 022D44D0: the entry's animation index for setAnm (0xFF ends the table); playAnm 022D5174 replays it (tww the same) |
| sRotenAnmDat 0x01 | `field_0x01` | `mMorf` | setAnmTbl 022D44D0: the entry's morf frames, passed to setAnm (tww the same) |
| sRotenAnmDat 0x02 | `field_0x02` | `mLoopCount` | setAnmTbl 022D44D0: the entry's count, copied into mAnmLoopCount; 0 or less loops (tww the same) |
| SaveDatStruct 0x00 | `field_0x00` | `mFirstTalkBit` | getMsg 022D6304: the event bit set with the first talk (l_msg_1st_talk); eventOrder 022D4EF0 allows the item talk (CANTALKITEM) once it is set (tww the same) |
| SaveDatStruct 0x02 | `field_0x02` | `mExchangeCountReg` | the event register counting exchanges: next_msgStatus 022D60DC adds 1; getMsg 022D6304 / XyEventCB 022D3388 switch to the exchange-done messages at 3 (tww the same) |
| SaveDatStruct 0x06 | `field_0x06` | `mItemShownBit` | getMsg 022D6304 / eventMesSetInit 022D40A4 set this event bit when a trade item is shown; getMsg then gives l_msg_exchange (tww the same) |

## daNpc_Os_c (d_a_npc_os.h)

Old names are the GameCube offsets (zeldaret/tww `field_0x...`; tww d217e2b decompiles src/d/actor/d_a_npc_os.cpp with the same code, cited as "tww the same").

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x8B0 | `field_0x738` | `mBeamCallBack0` | makeBeam 022ABF7C: emitter callback of ID_AK_SN_OTOMOBEAM00 (made while its emitter is NULL); endBeam 022AB28C ends it; execute 022AC1DC tests its emitter; init 022AA9A4 sets the vtable (tww the same) |
| 0x8B8 | `field_0x740` | `mBeamCallBack1` | makeBeam 022ABF7C: emitter callback of ID_AK_SN_OTOMOBEAM01 (made while its emitter is NULL); endBeam 022AB28C ends it; execute 022AC1DC tests its emitter; init 022AA9A4 sets the vtable (tww the same) |
| 0x8C0 | `field_0x748` | `mEyePos` | nodeCallBack 022A9AB8: the head matrix times the eye offset (osMain_offsetEyePos); setAttention 022AD6CC copies it to eyePos; TWW getEyePos() |
| 0x8CC | `field_0x754` | `mAttentionBasePos` | nodeCallBack 022A9AB8: the head matrix times the attention offset (osMain_offsetAttPos); init 022AA9A4 copies current.pos; TWW getAttentionBasePos() |
| 0x8DC | `field_0x764` | `mBrkLastFrame` | initBrkAnm 022A9F4C sets the brk animation's start frame; playBrkAnm 022AB3C4 compares the new frame with it (passed the end?) and keeps the frame here |
| 0x8F0 | `field_0x780` | `mMsgNo` | initialMsgSetEvent 022ADB60 stores the event's message number; talk_init 022ADBCC opens that message |
| 0x8F4 | `field_0x784` | `mStatus` | init 022AA9A4 clears it (GameCube clearStatus()) and sets os_b::STATUS_GRAVITY; animationPlay / execute test the STATUS bits; TWW clearStatus() / isGravity() |
| 0x8F8 | `field_0x788` | `mCylH` | init 022AA9A4 sets 120; setCollision 022AB2BC sets it as the cylinder height (SetH) |
| 0x8FC | `field_0x78C` | `mAnmPrmIdx` | setAnm 022AA88C stores its argument, the l_anmPrm index; animationPlay 022AB488 (1: smoke at frame 17), execute 022AC1DC / initialFinishEvent 022ADAA4 (2 / 4), waitNpcAction 022AE504 test it; init 022AA9A4 sets 5 (tww the same) |
| 0x904 | `field_0x794` | `mSwitchNo` | init 022AA9A4: (param >> 16) & 0xFF; eventOrderCheck 022AC030 tests dComIfGs_isSwitch(it) |
| 0x908 | `field_0x798` | `mLookMaxVel` | lookBack 022AE1D4 eases it to HIO mMaxHeadTurnVel (cLib_addCalcAngleS2 4, 0x800) or clears it; passed to lookAtTarget as maxVel |
| 0x910 | `field_0x7A0` | `mAnmTblIdx` | setAnm 022AA88C stores the animation table index (and skips when it and the play speed are unchanged) |
| 0x911 | `field_0x7A1` | `mBrkAnmEnd` | playBrkAnm 022AB3C4 stores mBrkAnm.play()'s result; actionWakeupEvent 022AD614 / actionFinishEvent 022ADB4C / finish01NpcAction 022AE834 wait for it; setAnm_brkAnm 022ADA3C and initialWakeupEvent 022AD598 clear it (tww the same) |
| 0x912 | `field_0x7A2` | `mBrkAnmIdx` | initBrkAnm 022A9F4C stores the brk animation index and re-inits only when it changes |
| 0x913 | `field_0x7A3` | `mAttnSetCount` | nodeCallBack 022A9AB8 adds 1 up to 0xFF (GameCube incAttnSetCount); execute 022AC1DC clears it |
| 0x914 | `field_0x7A4` | `mInAttention` | waitNpcAction 022AE504 / talkNpcAction 022AEA8C store chkAttention's result for the current facing; lookBack (in talkNpcAction / waitNpcAction) looks at the player while set; osCheckAttention 022AE014 tests it; searchNpcAction 022AF8D4 sets 1 (tww the same) |
| 0x915 | `field_0x7A5` | `mOrderEventNum` | the event number to order: waitNpcAction 022AE504 / checkGoalRoom 022ABE84 set getWakeupOrderEventNum / getFinishOrderEventNum, eventOrderCheck 022AC030 7-9, initialNextEvent 022ADDE0 2 / 4 / 6; eventOrder 022AC160 copies it into mEventNo (below 0x10) or orders the talk (0x11 / 0x12); eventProc 022AB6DC / checkOrder 022ABCB0 reset it to -1 (tww the same) |
| 0x918 | `field_0x7A8` | `mGroundHit` | execute 022AC1DC stores mAcch's ground-hit bit (flags bit 5) and, while set, keeps the actor on a moving background through mGroundPolyInfo; routeCheck 022AF4C8 sets 1 (tww the same) |
| 0x919 | `field_0x7A9` | `mActionPhase` | setAction 022AA630 sets -1 before calling the old action, 0 for the new one; the actions (d_a_npc_os_c.cpp) init at 0 and add 1 |
| 0x91A | `field_0x7AA` | `mEventNo` | init 022AA9A4 sets -1; eventOrder 022AC160 orders mEventIdx[it]; eventProc 022AB6DC runs it |
| 0x91A | `field_0x7C4` | `mEventIdx` | init 022AA9A4: os_getEventIdx of event_name_tbl[i]; eventOrder 022AC160 / eventProc 022AB6DC use [mEventNo] |
| 0x91C | `field_0x7AC` | `mCarryAngleY` | carryNpcAction 022AECD4: the angle to the player minus the player's shape_angle.y (fopAcM_toPlayerShapeAngleY) while carried; setAction 022AA630 clears it (tww the same) |
| 0x930 | `field_0x7C0` | `mEventTimer` | initialWaitEvent 022AD36C sets the event's Timer value (or -1); actionWaitEvent 022AD4F0 counts it down |
| 0x954 | `field_0x7E4` | `mCarryOffset` | carryNpcAction 022AECD4 clears it and chases it toward HIO (0x90, 0x94, 0x98); setBaseMtx 022AA464 translates the model by it (tww the same) |
| 0x960 | `field_0x7F0` | `mGroundNormal` | execute 022AC1DC copies the ground plane's normal; routeAngCheck 022AF2F0 crosses it with the route direction |
| 0x96C | `field_0x7FC` | `mGroundPolyInfo` | execute 022AC1DC copies mAcch.m_gnd's poly info into it and, while mGroundHit, corrects the position by the moving background under it; init 022AA9A4 clears its mpBgW (tww the same) |

Open: `field_0x790`, `field_0x79A`, `field_0x7AB`, `field_0x7B4`, `field_0x7B6`, `field_0x7BC`, `field_0x7BE`, `field_0x7C2` (no use; tww has none either); `field_0x7AE`, `field_0x7B0`, `field_0x7B2`, `field_0x7B8` (setAction 022AA630 clears them, never read; tww the same); `field_0x7A6` / `field_0x7A7` (searchNpcAction 022AF8D4 flips 0x7A6 whenever the 8-20 frame timer 0x7A7 runs out; 0x7A6 is never read; tww the same).

### daNpc_Os_HIO_c, daNpc_Os_HIO2_c (d_a_npc_os_b.h; the view OsActionHioHD / OsActionHioChildHD in d_a_npc_os_c.h takes the same names)

Old names are the GameCube offsets (zeldaret/tww `field_0x...`, unnamed there). The constructors daNpc_Os_HIO2_c_ct 022B01A4 / daNpc_Os_HIO_c_ct 022B024C set every field.

| Struct | Old | New | Evidence |
|---|---|---|---|
| daNpc_Os_HIO2_c (mOs2) | `field_0x08` | `mWalkSpeed` | walkProc 022AD668: speedF = it * the rate while the morf frame is inside (mWalkFrameStart, mWalkFrameEnd); walkPlayerAction 022AFF94 the same with the stick value (tww the same) |
| daNpc_Os_HIO2_c (mOs2) | `field_0x20` | `mPlayerWalkRate` | walkPlayerAction 022AFF94: speedF = mWalkSpeed * the stick value * it (tww the same) |
| daNpc_Os_HIO2_c (mOs2) | `field_0x24` | `mTurnMaxStep` | walkProc 022AD668 / walkPlayerAction 022AFF94: cLib_addCalcAngleS's maxStep (doubled for shape_angle in actionMoveEvent 022AD720, searchNpcAction 022AF8D4, walkPlayerAction) (tww the same; c_lib.h argument order) |
| daNpc_Os_HIO2_c (mOs2) | `field_0x26` | `mTurnMinStep` | walkProc 022AD668 / walkPlayerAction 022AFF94: cLib_addCalcAngleS's minStep (doubled for shape_angle, as mTurnMaxStep) (tww the same) |
| daNpc_Os_HIO2_c (mOs2) | `field_0x28` | `mTurnScale` | walkProc 022AD668 / walkPlayerAction 022AFF94 / actionMoveEvent 022AD720 / searchNpcAction 022AF8D4: cLib_addCalcAngleS's scale (tww the same) |
| daNpc_Os_HIO_c | `field_0x60` | `mFollowStopDist` | waitNpcAction 022AE504: once called, the statue starts searching while the player is farther than it; searchNpcAction 022AF8D4 walks at rate 0 inside it, else mFollowWalkRate (tww the same) |
| daNpc_Os_HIO_c | `field_0x64` | `mCallDist` | waitNpcAction 022AE504: while the player is closer than it, they may call the statue (onNpcCall) (tww the same) |
| daNpc_Os_HIO_c | `field_0x80` | `mThrowSpeedF` | throwNpcAction 022AEF70: speedF when thrown (tww the same) |
| daNpc_Os_HIO_c | `field_0x84` | `mThrowSpeedY` | throwNpcAction 022AEF70: speed.y when thrown (tww the same) |
| daNpc_Os_HIO_c | `field_0x8C` | `mGravity` | init 022AA9A4, waitNpcAction 022AE504, finish01NpcAction 022AE834, finish02NpcAction 022AE980, throwNpcAction 022AEF70, jumpNpcAction 022AFBF4, initialWaitEvent 022AD36C, initialWakeupEvent 022AD598: gravity = it (tww the same) |
| daNpc_Os_HIO_c | `field_0x90` | `mCarryOffsetX` | carryNpcAction 022AECD4 chases daNpc_Os_c::mCarryOffset.x toward it (tww the same) |
| daNpc_Os_HIO_c | `field_0x94` | `mCarryOffsetY` | carryNpcAction 022AECD4 chases daNpc_Os_c::mCarryOffset.y toward it (tww the same) |
| daNpc_Os_HIO_c | `field_0x98` | `mCarryOffsetZ` | carryNpcAction 022AECD4 chases daNpc_Os_c::mCarryOffset.z toward it (tww the same) |
| daNpc_Os_HIO_c | `field_0x9C` | `mStickTurnMin` | waitPlayerAction 022AFCE0: turns toward the stick while the stick value is at least it (or locked on) (tww the same) |
| daNpc_Os_HIO_c | `field_0xA0` | `mStickWalkMin` | waitPlayerAction 022AFCE0: switches to walkPlayerAction once the stick value is at least it (below it the statue only looks) (tww the same) |
| daNpc_Os_HIO_c | `field_0xA8` | `mFollowWalkRate` | searchNpcAction 022AF8D4: the walkProc rate while the player is farther than mFollowStopDist (tww the same) |
| daNpc_Os_HIO_c | `field_0xAC` | `mWalkFrameEnd` | walkProc 022AD668 / walkPlayerAction 022AFF94: the statue moves only while the morf frame is below it (and above mWalkFrameStart) (tww the same) |
| daNpc_Os_HIO_c | `field_0xB0` | `mWalkFrameStart` | walkProc 022AD668 / walkPlayerAction 022AFF94: the statue moves only while the morf frame is above it (and below mWalkFrameEnd) (tww the same) |

Open: daNpc_Os_HIO2_c `field_0x04`, `field_0x0C`..`field_0x1C`; daNpc_Os_HIO_c `field_0x68`..`field_0x7C`, `field_0x88`, `field_0xA4` (set by the constructor only, never read in WWHD or tww); `field_0x5C` (create 022AAEE0 stores the actor that registered the HIO child, never read; tww the same).

## daNpcMt_c (d_a_npc_mt.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_npc_mt.cpp, which keeps the GameCube offset names) and our decomp's use. Open (evidence not enough yet): `m6F0` (read only, never written: the turn-back runs only while it is 0), `m749` (message flag bits, never set in WWHD or tww); `m718`, `m72C`, `m736`, `m752` (written only); `m729`, `m74E` (no use); the padding `m6F4`, `m738`, `m754`.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x890 | `m71C` | `mNextMorf` | setAnm 022A01A4: when it is >= 0 it replaces the blend (morf) of the next animation and is reset to -1 (tww if (it >= 0.0f) { morf = it; it = -1.0f; }); daNpcMt_construct 0229FD7C -1 |
| 0x89C | `m728` | `mHeadOnlyFollow` | lookBack 022A1708 passes it as the head-only flag of the joint look-at (tww bool headOnlyFollow = it; m_jnt.lookAtTarget(.., headOnlyFollow)); chkAttention 022A0B20 / faceHome set 0 or 1 (tww); daNpcMt_construct 0229FD7C 1 |
| 0x8A6 | `m732` | `mHomeAngleY` | daNpcMt_construct 0229FD7C: home.angle.y (tww it = home.angle.y); faceHome / chkAttention 022A0B20 turn back to it (tww m740 = it) |
| 0x8A8 | `m734` | `mMsgStatus` | talk2 022A10BC: the message status, 0xFFFF on a new message, then the status that next_msgStatus returned (tww it = -1; if (it == fopMsgStts_MSG_CONTINUES_e) ..; it = status); after daNpcPeople_c::mMsgStatus (npcs.md) |
| 0x8B0 | `m73C` | `mTurnSpeedTarget` | chkAttention 022A0B20: l_npc_dat mMaxHeadTurnVel (tww); lookBack 022A1708 eases mTurnSpeed to it unless the event cut gives a turn speed (tww short target = it; cLib_addCalcAngleS2(&m73E, target, 4, 0x800)) |
| 0x8B2 | `m73E` | `mTurnSpeed` | lookBack 022A1708: eased to mTurnSpeedTarget while turning, else 0, and passed as the look-at's maxVel (tww m_jnt.lookAtTarget(.., desiredYRot, it, ..); dNpc_JntCtrl_c::lookAtTarget(.., s16 defaultY, s16 maxVel, ..)); createInit 022A0618 0 |
| 0x8B4 | `m740` | `mDesiredYRot` | lookBack 022A1708: the look-at's default Y angle in look mode 2 (tww desiredYRot = it); faceHome / chkAttention 022A0B20 set it to mHomeAngleY (tww it = m732) |
| 0x8B6 | `m742` | `mTalking` | checkOrder 022A0E84 sets 1 on the talk command and enters the talk mode; executeTalk 022A1D7C clears it when the box closes; executeCommon 022A1C4C returns it and enters the talk mode while it is 1 (tww); lookBack 022A1708 turns the whole body while it and mBodyTurn are set (tww); createInit 022A0618 0 |
| 0x8B7 | `m743` | `mInAttention` | chkAttention 022A0B20: 1 once the player comes close, 0 when they leave (tww); executeCommon 022A1C4C sets mOrderState 1 while it is set, else 0 (tww); createInit 022A0618 0 |
| 0x8B8 | `m744` | `mOrderState` | executeCommon 022A1C4C 1 / 0 from mInAttention; eventMesSet 022A1270 3 when a message flag asks for the item event; checkOrder 022A0E84 starts the talk for 1 / 2 and clears 3 once the get-item event (mMtGetItemEventIdx) starts; eventOrder 022A1500 orders by it (tww) |
| 0x8BA | `m746` | `mMoveProcIdx` | executeSetMode 022A0DFC: the value of l_execute_init[mode] (tww it = (this->*l_execute_init[param_1])()); _execute 022A1914 calls (this->*moveProc[it])() (tww); executeCommon 022A1C4C tests it; daNpcMt_construct 0229FD7C 0; same name as daObjFigure_c::mMoveProcIdx |
| 0x8BE | `m74A` | `mAnmIdx` | setAnm 022A01A4 stores the animation number; createHeap 0229FABC loads l_bck_ix_tbl[it] (tww); executeWait 022A1D2C goes back to the wait animation while it is not 0; setAnmTbl 022A02B0 compares with it (tww it != mpAnmDat->field_0x00) |
| 0x8C0 | `m74C` | `mAnmTimer` | setAnmTbl 022A02B0: mpAnmDat->mLoopCount (tww it = mpAnmDat->field_0x02); playAnm 022A1638 counts it down and acts at 0 (tww if (it > 0) { it--; if (it == 0) .. }) |
| 0x8C1 | `m74D` | `mActIdx` | privateCut 022A12FC: the event action index (tww it = dComIfGp_evmng_getMyActIdx(..), -1 ends, switch (it)); same name as the other actors' cut indices |
| 0x8C3 | `m74F` | `mLookMode` | lookBack 022A1708: 1 looks at mLookAtPos, 2 faces mDesiredYRot, 0 neither (tww switch (it)); chkAttention 022A0B20 / faceHome set it; daNpcMt_construct 0229FD7C 0 |
| 0x894 | `m720` | `mAttnDistXZ` | createInit 022A0618: l_npc_dat mMaxAttnDistXZ; chkAttention 022A0B20: the player counts as in attention while closer than it (+40 once in attention) (tww the same); same code as daNpcAh_c::mAttnDistXZ |
| 0x8A2 | `m72E` | `mTurnBackTimer` | chkAttention 022A0B20: l_npc_dat mTurnBackTime when the player leaves the attention, counted down while no message is set, and at 0 faceHome turns the NPC back home (tww the same); same code as daNpcAh_c::mTurnBackTimer |
| 0x8A4 | `m730` | `mAttnAngle` | createInit 022A0618: l_npc_dat mMaxAttnAngleY; chkAttention 022A0B20: the player counts as in attention while the angle to them is below it (+0x71C once in attention) (tww the same); same code as daNpcAh_c::mAttnAngle |

Round 2:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x8BB | `m747` | `mResFlag` | tww accessor setResFlag(u8 flag) { m747 = flag; }: daNpcMt_construct 0229FD7C 0, phase_1 022A0018 1 once the resources are loaded (tww setResFlag(1)); tww demo _delete deletes the resource only while it is set; same name as daNpcAh_c / daNpcRoten_c::mResFlag |
| 0x8C4 | `m750` | `mBodyTurn` | createInit 022A0618 copies l_npc_dat mBodyTurn; while set chkAttention 022A0B20 (event look) / its playerGaze turn the body (mHeadOnlyFollow 0, mbTrn 1) and lookBack 022A1708 turns the body while mTalking (tww the same); same role as daNpcPhoto_c / daNpcPeople_c::mBodyTurn |
| 0x8C5 | `m751` | `mLookAtPlayer` | createInit 022A0618 copies l_npc_dat mLookAtPlayer; playerGaze (chkAttention 022A0B20): while 0 the NPC turns back home (faceHome) right after looking at the player (tww if (m751 == 0) ..); same role as daNpcPhoto_c / daNpcPeople_c::mLookAtPlayer |
| 0x8C7 | `m753` | `mAttnEnabled` | after tww attnOn() / attnOff() (m753 = true / false); chkAttention 022A0B20: while set the player is looked at outside the attention range too; createInit 022A0618 0; daKnob00_c::actionFigure 021A43F8 sets 1, actionTalk 021A4F70 0 (tww the same); same name as the KnobMt view (d_a_knob00.h) |

### MtNpcDat and sMtAnmDat (d_a_npc_mt.h; the npc data l_npc_dat and the animation table entry)

Old names are the GameCube offsets (zeldaret/tww `field_0x..`; tww names only the m_jnt limits, kept). Where the role matches, the names are those of the photo / roten NpcDatStruct and sPhotoAnmDat.

| Struct | Old | New | Evidence |
|---|---|---|---|
| sMtAnmDat | `field_0x00` | `mAnmIdx` | setAnmTbl 022A02B0: the entry's animation number for setAnm (0xFF ends the table); playAnm 022A1638 replays it (tww the same); same code as sPhotoAnmDat::mAnmIdx |
| sMtAnmDat | `field_0x01` | `mMorf` | setAnmTbl 022A02B0: the entry's morf frames, passed to setAnm (tww the same); same code as sPhotoAnmDat::mMorf |
| sMtAnmDat | `field_0x02` | `mLoopCount` | setAnmTbl 022A02B0: the entry's count, copied into mAnmTimer; above 0 plays it that often, else loops (tww the same); same code as sPhotoAnmDat::mLoopCount |
| MtNpcDat | `field_0x14` | `mPlayerEyeOffsetY` | chkAttention 022A0B20: passed to dNpc_playerEyePos, the look target above the player's head (tww the same); same role as NpcDatStruct::mPlayerEyeOffsetY (photo, roten) |
| MtNpcDat | `field_0x18` | `mAttnYOffset` | _execute 022A1914: attention_info.position = current.pos + (0, it, 0) (tww the same); same role as daNpcRoten_c NpcDatStruct::mAttnYOffset |
| MtNpcDat | `field_0x1C` | `mEyeYOffset` | _execute 022A1914: eyePos = current.pos + (0, it, 0) (tww the same); same role as NpcDatStruct::mEyeYOffset (photo, roten) |
| MtNpcDat | `field_0x20` | `mMaxAttnDistXZ` | createInit 022A0618 copies it into mAttnDistXZ, chkAttention 022A0B20's attention distance (tww the same); same role as NpcDatStruct::mMaxAttnDistXZ (photo, roten) |
| MtNpcDat | `field_0x24` | `mLookDistXZ` | chkAttention 022A0B20: outside the attention, the player is still looked at while closer than it (sets m752) (tww the same) |
| MtNpcDat | `field_0x28` | `mMaxAttnAngleY` | createInit 022A0618 copies it into mAttnAngle, chkAttention 022A0B20's attention angle (tww the same); same role as NpcDatStruct::mMaxAttnAngleY (photo, roten) |
| MtNpcDat | `field_0x2A` | `mMaxHeadTurnVel` | chkAttention 022A0B20: mTurnSpeedTarget, the speed lookBack 022A1708 eases the look toward (tww the same); same role as NpcDatStruct::mMaxHeadTurnVel (photo, roten) |
| MtNpcDat | `field_0x30` | `mCylRadius` | createInit 022A0618 / _execute 022A1914: setCollision(it, 150), the body cylinder's radius (tww the same); same role as daNpcRoten_c NpcDatStruct::mCylRadius |
| MtNpcDat | `field_0x48` | `mTurnBackTime` | chkAttention 022A0B20 loads mTurnBackTimer with it when the player leaves the attention (tww the same) |
| MtNpcDat | `field_0x4A` | `mBodyTurn` | createInit 022A0618 copies it into daNpcMt_c::mBodyTurn; chkAttention 022A0B20 / its playerGaze turn the body while it is set (mHeadOnlyFollow 0), the head only while 0 (tww the same); same role as daNpcPhoto_c NpcDatStruct::mBodyTurn |
| MtNpcDat | `field_0x4B` | `mLookAtPlayer` | createInit 022A0618 copies it into daNpcMt_c::mLookAtPlayer; playerGaze (chkAttention 022A0B20) turns back home (faceHome) after the look while 0 (tww the same); same role as daNpcPhoto_c NpcDatStruct::mLookAtPlayer |

Open (never read in WWHD or tww): MtNpcDat `field_0x12`, `field_0x2C`, `field_0x2E`, `field_0x34`, `field_0x38`, `field_0x3C`, `field_0x3E`, `field_0x40`, `field_0x42`, `field_0x44`, `field_0x46`.

## daNpc_P1_c (d_a_npc_p1.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`; tww's code uses them the same way). All of the class's offset-named fields are named.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x7CE | `m65A` | `mExplainStep` | minigameExplainCut 022B1C84: 0 starts message 0xFAD and steps to 1, 1 steps to 2, -1 holds; _execute 022B32B0 resets it to 0 outside events |
| 0x7E0 | `m66C` | `mAnmLoopCnt` | setAnimFromMsg 022B22B8: 4/6 with anims 0xC/8, minus 1 per anim end, the follow-up anim starts at 0; kaji_anm 022B2830: 1-2 or 4 loops of anims 0xF/0xE; confuseAction 022B4764: counts loops to 3 before switching anim |
| 0x7E4 | `m670` | `mbDemoActive` | demo_move 022B12FC: 1 while the demo actor exists (head/backbone locked), demo_end_init 022B1128 when it is gone resets it to 0; _execute 022B32B0 moves the actor itself only at 0 |
| 0x7E5 | `m671` | `mbAtKaji` | _create 022B36A8: 1 only for type P1a with param 2 (backbone locked, mKajiId = getKajiID()); kaji_anm 022B2830, lookBack 022B2E70, normalAction 022B43EC: the helm (kaji) anims and look |

## daNpc_Tt_c and tt_ke_s (d_a_npc_tt.h)

zeldaret/tww (d217e2b) keeps offset names for these fields, so the names come from the WWHD uses.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x1116 | `field_0xE7A` | `mDanceCount` | demoInitPatten 022EFAD8: the event's "Cnt" integer (default 2); demoProcPatten 022EFF38: minus 1 per finished danceProc, the cut ends at 0 |
| tt_ke_s +0x00 | `field_0x00` | `mPos` | ke_control 022EE0D8: the strand's 10 joint positions, each pulled to 7.875 from the previous one; ke_pos_set 022EE2FC copies them into the line; set_ke_root 022EDF64 sets the root (after our decomp's gnd_ke_s mPos) |
| tt_ke_s +0x78 | `field_0x78` | `mVelocity` | ke_control 022EE0D8: holds the old position, then (new - old) * damping, added to the next step's position (after our decomp's gnd_ke_s mVelocity) |

## daNpc_Nz_c (d_a_npc_nz.h)

Open: `field_0xA16`, `field_0xA22`, `field_0xBD4` (no use in WWHD or tww); daNpc_Nz_HIO_c `field_0x1A`, `field_0x1C` (set by the constructor 022A5DEC only, never read).

## daNpc_Pm1_c and daNpc_Pm1_HIO_c (d_a_npc_pm1.h)

zeldaret/tww (d217e2b) has no code for this class, so the names come from the WWHD uses. The class follows the same NPC template as daNpc_Pf1_c (same members in the same order, same code in setMtx, setAnm_anm, anmAtr, set_action, wait/talk), so a field with the same use takes daNpc_Pf1_c's name in this decomp.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x862 | `field_0x6F2` | `mBlinkTimer` | init_btp 022D0F4C 0; playTexPatternAnm 022D14A4: counted down by cLib_calcTimer while btp 0 plays, then reset to 30 + rnd(60) (after our decomp's daNpc_Pf1_c mBlinkTimer, same code) |
| 0x8D8 | `field_0x76C` | `mHomeAngle` | _execute 022D1F70: current.angle on the first frame (mbHomeSet); wait01 022D269C turns back to its y (after our decomp's daNpc_Pf1_c mHomeAngle, same code) |
| 0x8E0 | `field_0x774` | `mHomePos` | _execute 022D1F70: current.pos on the first frame (mbHomeSet) (after our decomp's daNpc_Pf1_c mHomePos, same code) |
| 0x904 | `field_0x798` | `mLookPos` | createInit 022D174C: current.pos; lookBack 022D1CF4: the look-at target in LOOK_POSITION (after our decomp's daNpc_Pf1_c mLookPos) |
| 0x910 | `field_0x7A4` | `mPrevMorfFrame` | setMtx 022D15F0: the morf frame of the last frame; a smaller frame now means the animation looped; setAnm_anm 022D2324 0 (after our decomp's daNpc_Pf1_c mPrevMorfFrame, same code) |
| 0x91C | `field_0x7B0` | `mTurnSpeed` | lookBack 022D1CF4: eased to HIO field_12 while m_jnt turns, else 0, and passed as the look-at's maxVel (after our decomp's daNpcMt_c mTurnSpeed, the same lookAtTarget argument) |
| 0x91E | `field_0x7B2` | `mLookAngleY` | wait01 022D269C: mHomeAngle.y with LOOK_ANGLE; lookBack 022D1CF4: the target angle in LOOK_ANGLE (after our decomp's daNpc_Pf1_c mLookAngleY) |
| 0x920 | `field_0x7B4` | `mbMorfAnimStopped` | setMtx 022D15F0: mpMorf->play()'s result, or 1 when the frame went back; setAnm_anm 022D2324 0 (after our decomp's daNpc_Pf1_c mbMorfAnimStopped, same code) |
| 0x923 | `field_0x7B7` | `mItemNo` | wait01 022D269C: 0xFF, or dComIfGp_event_getPreItemNo() on a talk with an item; talk01 022D275C 0xFF (after our decomp's daNpc_Pf1_c mItemNo, same code) |
| 0x924 | `field_0x7B8` | `mbHomeSet` | _execute 022D1F70: 0 stores mHomePos / mHomeAngle once, then 1 (after our decomp's daNpc_Pf1_c mbHomeSet, same code) |
| 0x928 | `field_0x7BC` | `mbInitEyePos` | createInit 022D174C 1; setAttention 022D1590: copies mEyePos to eyePos while it or mbSetEyePos is set, then clears it (one eye position right after create) |
| 0x92C | `field_0x7C0` | `mbSetEyePos` | wait_action1 022D27DC: the result of talk01 / wait01 (0 for other states); setAttention 022D1590 copies mEyePos to eyePos while set (after our decomp's daNpc_Pf1_c mbSetEyePos, same code) |
| 0x930 | `field_0x7C4` | `mbAttention` | wait_action1 022D27DC: chkAttention(); wait01 022D269C looks at the player while set, else turns home (after our decomp's daNpc_Pf1_c mbAttention, same code) |
| 0x931 | `field_0x7C5` | `mbTalk` | checkOrder 022D1A5C: 1 on the talk command while mEvtState is 1/2; wait01 022D269C enters STATE_TALK; talk01 022D275C clears it when the message closes (after our decomp's daNpc_Pf1_c mbTalk) |
| 0x933 | `field_0x7C7` | `mbDemo` | demo 022D1A9C: 1 while demoActorID is set (returned), else 0; setMtx 022D15F0 skips animation and ground while set (after our decomp's daNpc_Pf1_c mbDemo, same code) |
| 0x935 | `field_0x7C9` | `mActionIndex` | privateCut 022D1C04: dComIfGp_evmng_getMyActIdx(staff, cut_name_tbl, ..); -1 ends the cut, 0 runs event_action (after our decomp's daNpc_Pf1_c mActionIndex, same code) |
| 0x937 | `field_0x7CB` | `mAnmAtr` | chngAnmAtr 022D24B0: the message animation attribute; setAnm_ATR 022D244C plays a_atr_prm_tbl[it]; setStt 022D2594 0xFF (after our decomp's daNpc_Pf1_c mAnmAtr) |
| 0x938 | `field_0x7CC` | `mMesgAnimeTag` | anmAtr 022D24D4: the last dComIfGp_getMesgAnimeTagInfo() taken (after our decomp's daNpc_Pf1_c mMesgAnimeTag, same code) |
| 0x939 | `field_0x7CD` | `mBtpNum` | setAnm_tex 022D2300: the btp (texture pattern) number; initTexPatternAnm 022D1060 / playTexPatternAnm 022D14A4 use it; CreateHeap 022D10A4 a_tex_pattern_num_tbl[0] (after our decomp's daNpc_Pf1_c mBtpNum) |
| 0x93A | `field_0x7CE` | `mBckNum` | setAnm_anm 022D2324: the bck number of the animation set (after our decomp's daNpc_Pf1_c mBckNum, same code) |
| 0x93B | `field_0x7CF` | `mEvtState` | wait01 022D269C 2; checkOrder 022D1A5C starts the talk at 1/2 and clears it; eventOrder 022D1F38 orders the speak event at 1, can-talk at 1/2 (after our decomp's daNpc_Pf1_c mEvtState) |
| 0x93C | `field_0x7D0` | `mStt` | setStt 022D2594: the state; wait_action1 022D27DC runs talk01 / wait01 by it; setAnm 022D23E0 plays a_anm_prm_tbl[it] (after our decomp's daNpc_Pf1_c mStt, same code) |
| 0x93D | `field_0x7D1` | `mPrevStt` | setStt 022D2594: the state before; talk01 022D275C goes back to it (after our decomp's daNpc_Pf1_c mPrevStt) |
| 0x93E | `field_0x7D2` | `mLookMode` | lookBack 022D1CF4: LOOK_PLAYER / LOOK_POSITION / LOOK_ANGLE; wait01 022D269C / setStt 022D2594 set it (after our decomp's daNpc_Pf1_c mLookMode) |
| 0x941 | `field_0x7D5` | `mActState` | set_action 022D1378: -1 when the old action ends, 0 for the new one; wait_action1 022D27DC sets up at 0 and steps to 1 (after our decomp's daNpc_Pf1_c mActState, same code; pf1 ends with 9) |
| 0x942 | `field_0x7D6` | `mAtrSet` | anmAtr 022D24D4: 0 takes the message's animation attribute once and steps to 1, MSG_DISPLAYED resets it (after our decomp's daNpc_Pf1_c mAtrSet, same code) |
| HIO +0x04 | `field_0x8` | `mRefCount` | daNpc_Pm1_HIO_c_ct 022D28A0 -1; _create 022D1858 creates the HIO child at < 0 and adds 1; _delete 022D19E0 subtracts 1 and deletes the child below 0 (after our decomp's daTag_Ba1_HIO_c mRefCount, same code) |
| hio_prm_c +0x00 | `field_0` | `mMaxHeadX` | _execute 022D1F70 passes it to m_jnt.setParam as max_head_x (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) |
| hio_prm_c +0x02 | `field_2` | `mMaxHeadY` | _execute 022D1F70 passes it to m_jnt.setParam as max_head_y (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) |
| hio_prm_c +0x04 | `field_4` | `mMinHeadX` | _execute 022D1F70 passes it to m_jnt.setParam as min_head_x (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) |
| hio_prm_c +0x06 | `field_6` | `mMinHeadY` | _execute 022D1F70 passes it to m_jnt.setParam as min_head_y (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) |
| hio_prm_c +0x08 | `field_8` | `mMaxBackboneX` | _execute 022D1F70 passes it to m_jnt.setParam as max_backbone_x (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) |
| hio_prm_c +0x0A | `field_A` | `mMaxBackboneY` | _execute 022D1F70 passes it to m_jnt.setParam as max_backbone_y (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) |
| hio_prm_c +0x0C | `field_C` | `mMinBackboneX` | _execute 022D1F70 passes it to m_jnt.setParam as min_backbone_x (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) |
| hio_prm_c +0x0E | `field_E` | `mMinBackboneY` | _execute 022D1F70 passes it to m_jnt.setParam as min_backbone_y (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) |
| hio_prm_c +0x10 | `field_10` | `mMaxTurnStep` | _execute 022D1F70 passes it to m_jnt.setParam as max_turn_step (dNpc_JntCtrl_c::setParam's parameter, tww d_npc.h; same names as daNpc_Nz_HIO_c) |
| hio_prm_c +0x12 | `field_12` | `mTurnSpeedTarget` | lookBack 022D1CF4: mTurnSpeed eases to it while m_jnt turns (after our decomp's daNpcMt_c mTurnSpeedTarget, the same role) |
| 0x936 | `field_0x7CA` | `mEventActionNo` | event_actionInit 022D1B74 stores the cut's "ActNo" integer (dComIfGp_evmng_getMyIntegerP); event_action 022D1BFC is empty, so it is never read; same store as daNpc_Kp1_c::mEventActionNo |
| hio_prm_c +0x18 | `field_18` | `mDebugDraw` | HD byte flag: _draw 022D2128 initialises the debug colour statics (a_red / a_blue) while it is set; never written in WWHD (tww has no Pm1 code); after our decomp's daNpc_Pf1_HIO_c mDebugDraw (same _draw code) |

Open: `field_0x6F1`, `field_0x7AC`, `field_0x7B6`, `field_0x7D7` (no use in WWHD; tww has no Pm1 code); `field_0x7B5` (setAnm_anm 022D2324 clears it, never read: Pm1's ctrlAnmAtr is empty); `field_0x7C8` (talk01 022D275C clears it, never read); `field_0x7D3`, `field_0x7D4` (written only, in decideType 022D1350).

## daNpc_Rsh1_c (d_a_npc_rsh1.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`); tww's code (src/d/actor/d_a_npc_rsh1.cpp) uses them the same way. Open: `m76F` (nodeCallBack_Rsh 022D691C counts it up to 0xFF, nothing else reads it), `m962` (CreateInit 022D75E8 writes 0, never read).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x878 | `m704` | `mPathPntIdx` | pathGet 022D71D4: the index of the path point with arg3 1 (the start); pathMove 022D9F74: steps it by 1 toward mShopPathPntIdx[aim] and stores it once the point is reached (tww same code) |
| 0x8B8 | `m744` | `mShopPathPntIdx` | pathGet 022D71D4: [arg3] = the index of the path point with that arg3 (the shop positions, as mPathPointPos[arg3]); pathMove 022D9F74 walks to [aim] (tww same code) |
| 0x8BD | `m749` | `mbItemMsgSet` | shop_talk 022D9ACC: 1 when the item selection (now_triggercheck) set a new message in mCurrMsgNo; the next displayed/continue status is then taken without advancing and clears it; CreateInit 022D75E8 0 (tww same code) |
| 0x8E5 | `m771` | `mTalking` | checkOrder 022D7D4C: 1 on the talk command; wait01 022DA1C4 saves mLookMode and looks at the player while set; talk01 022DA4C4 clears it when the box closes (after our decomp's daNpcMt_c mTalking, the same role; tww same code) |
| 0x8EC | `m778` | `mCurrMsgNo` | talk 022D9C18: getMsg() on a new talk, then the message manager's mMsgNo; normal_talk 022D96E4 advances it by next_msgStatus; evn_talk_init 022DAF30 the event's "MsgNo"; talk01 022DA4C4 picks the item demo by it (tww same code) |
| 0x8F0 | `m77C` | `mShopMsgNo` | shop_talk 022D9ACC: a copy of mCurrMsgNo, advanced by next_msgStatus(&it, &mCurrMsgNo) and set as the next message; talk 022D9C18 0 (tww same code) |
| 0x8F4 | `m780` | `mNextMsgNo` | getMsg 022D94B8: returned once (then 0) before the default messages; checkOrder 022D7D4C / wait01 022DA1C4 0x2883, getdemo_action 022DA818 0x2857/0x288F/0x2860 (tww same code) |
| 0x8F8 | `m784` | `mEndMsgNo` | evn_talk_init 022DAF30: the event's "EndMsgNo"; evn_talk 022DB12C ends the talk when mCurrMsgNo reaches it, then clears it (tww same code) |
| 0x900 | `m78C` | `mShopNum` | createShopList 022D6FDC: the number of shop item sets (at least 1); pl_shop_out_action 022DAA78 walks to that shop position (tww same code) |
| 0x907 | `m793` | `mbShopOut` | wait01 022DA1C4: 1 when the player bumps into the actor (with mNextMsgNo 0x2883); talk01 022DA4C4 starts pl_shop_out_action after message 0x2883 while set; pl_shop_out_action 022DAA78 / CreateInit 022D75E8 0 (tww same code) |
| 0x908 | `m794` | `mCollisionPos` | setCollision 022D8738: mCyl.SetC(&it); pl_shop_out_action 022DAA78 puts it 140 behind the actor (setShopOutPos) and eases it back to current.pos (tww same code) |
| 0x914 | `m7A0` | `mModelOffset` | set_mtx 022D729C: the model is drawn at current.pos + it; pl_shop_out_action 022DAA78 eases it out to 100 ahead and back to zero; CreateInit 022D75E8 zero (tww same code) |
| 0xAC0 | `m958` | `mTexAnmIdx` | setTexAnm 022D8380: the texture pattern index; initTexPatternAnm 022D6BB0 loads l_btp_ix_tbl[it]; CreateHeap 022D6CC4 0 (tww same code) |
| 0xAC1 | `m959` | `mAnmIdx` | setAnm 022D8B20: the animation index; setAnmFromMsgTag 022D8B7C counts the loops of animation 4 (tww same code) |
| 0xAC2 | `m95A` | `mAnmLoopCnt` | setAnmFromMsgTag 022D8B7C: 2 with animation 4, minus 1 per loop end, back to animation 0 at 0 (tww same code) |
| 0xAC3 | `m95B` | `mOrderState` | eventOrder 022D808C: 5 shop-out event, 4 RSH_GET_DEMO, 1-3 can talk (1/2 order the speak event); checkOrder 022D7D4C starts them and clears it; wait01 022DA1C4 / talk01 022DA4C4 set it (after our decomp's daNpcMt_c mOrderState, the same role; tww same code) |
| 0xAC4 | `m95C` | `mLookMode` | lookBack 022D83A8: switch (it) picks the look target; wait01 022DA1C4 2 while talking, wait_action 022DA748 1 (tww same code) |
| 0xAC5 | `m95D` | `mPrevLookMode` | wait01 022DA1C4: mLookMode saved when the talk starts; event_action 022DB4A4 restores it (tww same code) |
| 0xAC6 | `m95E` | `mType` | _create 022D7998: (param >> 20) & 0xF, every value maps to 0; CreateInit 022D75E8 sets up type 0 (tww switch (it)) |
| 0xAC9 | `m961` | `mTalkStep` | talk 022D9C18: 0 starts (getMsg), 1 waits for the message, 2 normal_talk, 3 shop_talk, -1 stops; talkInit 022D7D40 0 (tww same code) |
## daNpc_Cb1_c (d_a_npc_cb1.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_npc_cb1.cpp, which keeps the GameCube offset names) and our decomp's use (d_a_npc_cb1.cpp, _act.cpp, _exec.cpp). mSetGoalWaitCount (m8F3) is WWHD-only: tww does not use it. Open (evidence not enough yet): `m8F2` (flyAction sets it once mFlyHeight passes HIO mFlyDriftHeight and then adds a sideways drift; role unclear).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0xA9C | `m88C` | `mSlideSpeed` | execute 0222091C adds its x/z to current.pos each frame and eases it (cLib_addCalcPosXZ) toward the ground normal's x/z * 40 on slide-code ground, else toward zero (tww current.pos.x += it.x; cLib_addCalcPosXZ(&it, temp2, ..)) |
| 0xAC0 | `m8B0` | `mPrevAnmFrame` | playAnm 02220024: a morf frame below it means the animation wrapped; it then takes the current frame (tww if (r3 \|\| mpMorf->getFrame() < it) ..; it = mpMorf->getFrame()); initAnm 0221FEFC 0 |
| 0xAD8 | `m8D0` | `mLookAtMaxVel` | lookBack 02221EF8 eases it to HIO mMaxHeadTurnVel while turning, else 0, and passes it as the look-at's maxVel (tww mJntCtrl.lookAtTarget(.., desiredY, it, 0)); after tww daNpc_Bm1_c / daNpc_Ah_c mLookAtMaxVel |
| 0xADF | `m8D7` | `mAnmLooped` | playAnm 02220024 sets 1 when the animation wraps; initAnm 0221FEFC 0; flyAction 022222D0 / evActSow 022265AC act once it is set (tww) |
| 0xAE0 | `m8D8` | `mAnmLoopCount` | playAnm 02220024 adds 1 per wrap and switches the animation once it reaches the table's loop count (tww it++; if (it >= data.field_0x04)); initAnm 0221FEFC 0 |
| 0xAE3 | `m8DB` | `mAnmPrmIdx` | setAnm 02221978 stores the ANM_ number and indexes anmPrmData with it, skipping a repeat (tww if (it == param_1) ..; it = param_1; &anmPrmData[it]); create 0221F700 0xFF (tww constructor) |
| 0xAE4 | `m8DC` | `mAnmTblIdx` | initAnm 0221FEFC stores the animation table index; playAnm 02220024 reads anmTblData[it] (tww); flyAction 022222D0 / draw 022213BC / waitNpcAction 02223048 test it; same name as daNpc_Md_c::mAnmTblIdx (npcs.md) |
| 0xAE5 | `m8DD` | `mOrderEvent` | eventOrder 02220884: 0-4 order mEventIdx[it], 5/6 the talk, 7 the talk-with-item (tww); eventProc 02220288 / checkOrder 02220738 / checkCommandTalk 02220194 reset it to -1 once accepted; init 0221F3C0 -1; the actions set it |
| 0xAE6 | `m8DE` | `mSearchSide` | searchNpcAction 022244A0 flips it (^= 1) and turns by -/+ HIO mSearchTurnAngle by it (tww it ^= 1; temp += it ? -l_HIO.field_0xE6 : ..) |
| 0xAE7 | `m8DF` | `mSearchTimer` | searchNpcAction 022244A0: counted down (cLib_calcTimer) and reloaded with cLib_getRndValue(HIO mSearchTimeBase, mSearchTimeRange) at 0 (tww) |
| 0xAE8 | `m8E0` | `mGroundHit` | execute 0222091C: mAcch.ChkGroundHit() taken before CrrPos; while set, the moving-ground correction and the slope fall speed apply (tww); routeCheck 0222403C 1; flyAction 022222D0 / evActWalk 02225CB0 test it (cb1_execRoom, inlined into execute) |
| 0xAE9 | `m8E1` | `mMoveBlockSide` | execute 0222091C: chkMoveBlock(&mMoveBlockSpeed) (tww; daPy_npc_c::chkMoveBlock returns -1 / 1 for the side of a pushed block coming at it, else 0); searchNpcAction 022244A0 / waitNpcAction 02223048 step aside by its sign |
| 0xAEA | `m8E2` | `mDoStatus` | execute 0222091C: dComIfGp_getDoStatus() (tww it = dComIfGp_getDoStatus()), else 0; sowCheck 02222EF4 tests != 0x2D |
| 0xAEB | `m8E3` | `mOrderedEvent` | eventOrder 02220884 copies mOrderEvent and orders mEventIdx[it] (tww); eventProc 02220288 ends the event when dComIfGp_evmng_endCheck(mEventIdx[it]) (tww); evInitOffsetLink 02225B24 / evInitSetGoal 022266DC test 0; init 0221F3C0 -1 |
| 0xAF6 | `m8EE` | `mEvtTimer` | evInitWait 02225798 / evInitSow 022264E4 load the cut's Timer; evActWait 02225820 / evActSow 022265AC count it to 0 (tww cLib_chaseS(&it, 0, 1), cLib_calcTimer(&it)); evInitWarp 02226884 / evActWarp 022268E0 (tww) |
| 0xAF8 | `m8F0` | `mActionStatus` | setAction 0221F140 sets -1 for the leaving action and 0 for the new one; the actions step it (tww it++, if (it == 0) ..); after tww daNpc_Bs1_c / daNpc_Bmsw_c mActionStatus |
| 0xAF9 | `m8F1` | `mFlyLiftCycle` | flyNpcAction 02223A88: flyAction gets lift (param_1 TRUE) only when it is 0, then it = (it + 1) % HIO mFlyLiftPeriod (tww); setAction 0221F140 0 |
| 0xAFC | `m8F4` | `mCarryAngle` | carryNpcAction 02223694: the angle to the carrying player's shape angle, chased to 0 (tww it = fopAcM_toPlayerShapeAngleY(this); cLib_chaseAngleS(&it, 0, 0x800); shape_angle.y = player->shape_angle.y + it); setAction 0221F140 0 |
| 0xAFE | `m8F6` | `mLandIdleCount` | flyAction 022222D0: counts frames on the ground with the propeller at or below HIO mPropellerIdleSpeed and ends the flight past HIO mLandIdleMax (tww ++it > l_HIO.field_0xCC); 0 while lifting; setAction 0221F140 0 |
| 0xB00 | `m8F8` | `mPropellerSpeed` | flyAction 022222D0: chased up to HIO mPropellerSpeedMax while lifting, down otherwise, added to mPropellerAngle and scales JA_SE_CM_PRAPELLO_ROLLING (tww cLib_chaseS(&it, ..); m8FA += it; fopAcM_seStart(.., it * (100.0f / l_HIO.field_0xD0))); flyNpcAction 02223A88 20000; setAction 0221F140 0 |
| 0xB02 | `m8FA` | `mPropellerAngle` | flyAction 022222D0 adds mPropellerSpeed each frame (tww it += m8F8); ppNodeCallBack 0221E5BC turns the propeller joint by it (TWW getWork3()); setAction 0221F140 0 |
| 0xB04 | `m8FC` | `mFlyHeight` | flyAction 022222D0 adds the frame's rise (tww it += current.pos.y - old.pos.y) and tests it against HIO mFlyAnmHeight / mFlyDriftHeight; flyNpcAction 02223A88 HIO mFlyAnmHeight + 1; execute 0222091C (cb1_execRoom) > -400; setAction 0221F140 0 |
| 0xB08 | `m900` | `mFallDist` | execute 0222091C adds the frame's drop (tww it += temp4 - current.pos.y); checkLanding 0221FCE4 tests > 200; flyAction / jumpNpcAction 02224AC0 / jumpPlayerAction 02225464 / flyNpcAction 02223A88 reset it to 0 |
| 0xB0C | `m904` | `mCarryOffset` | carryNpcAction 02223694 chases it to HIO mCarryOffsetX / Y / Z (tww cLib_chaseF(&it.x, ..)); setBaseMtx 0221ED08 translates the model by it (tww mDoMtx_stack_c::transM(it.x, it.y, it.z)) |
| 0xB18 | `m910` | `mGroundNormal` | execute 0222091C (cb1_execRoom): the ground triangle's normal (tww it = pla->mNormal); sets the slope fall speed and the slide speed (tww); routeAngCheck 02223E64 crosses it with the route direction |
| 0xB24 | `m91C` | `mMoveBlockSpeed` | execute 0222091C: chkMoveBlock's out vector (tww chkMoveBlock(&it); daPy_npc_c::chkMoveBlock(cXyz* outBlockVel) = block current.pos - old.pos); searchNpcAction turns by its atan2 (tww cM_atan2s(it.x, it.z)) |
| 0xAFB | `m8F3` | `mSetGoalWaitCount` | WWHD only (no tww use): evActSetGoal 02226784 adds 1 while Link is more than 50 from the goal and puts Link there once it reaches 150 |

### daNpc_Cb1_HIO_c and AnmData (copies in d_a_npc_cb1_action.h, _create.h, _execute.h)

The three private HIO views keep the GameCube offsets and were all offset-named the same way, so each copy takes the same names (no copy used another name). zeldaret/tww's daNpc_Cb1_HIO_c keeps `field_0x` names for these; its code uses them as WWHD does. `AnmData` is in d_a_npc_cb1_execute.h only. Open: `field_0x94` (u8[4]), `field_0xB4`, `field_0xC8`, `field_0xEF` (only the constructor 02226B9C sets them; no use in WWHD or tww).

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| HIO 0x30 | `field_0x30` | `mEyePosOffsetY` | setBaseMtx 0221ED08 puts eyePos this far above current.pos (cLib_offsetPos), next to attention_info.position at mNpc.mAttnYOffset; daNpc_Cb1_HIO_c ctor 02226B9C 50.0 (tww execute the same) |
| HIO 0x64 | `field_0x64` | `mFlyLandSpeedY` | flyAction 022222D0: speed.y on touching down from the flight (not on ground special code 1), before checkLanding (tww the same); ctor 8.0 |
| HIO 0x70 | `field_0x70` | `mGravity` | init 0221F3C0 and execute 0222091C (cb1_execRoom, the walking position move) gravity (tww the same); ctor -4.0 |
| HIO 0x74 | `field_0x74` | `mCarryOffsetX` | carryNpcAction 02223694 chases mCarryOffset.x to it (tww the same); ctor -1.5 |
| HIO 0x78 | `field_0x78` | `mCarryOffsetY` | carryNpcAction 02223694 chases mCarryOffset.y to it (tww the same); ctor -6.0 |
| HIO 0x7C | `field_0x7C` | `mCarryOffsetZ` | carryNpcAction 02223694 chases mCarryOffset.z to it (tww the same); ctor -7.6 |
| HIO 0x80 | `field_0x80` | `mStickTurnMin` | waitPlayerAction 02224E38 turns Makar only with the stick at or above it (or a lock-on); flyAction 022222D0 ends a grounded flight at it (tww CPad_GET_STICK_VALUE(0) >= it); ctor 0.1 |
| HIO 0x84 | `field_0x84` | `mStickWalkMin` | waitPlayerAction 02224E38 switches to walkPlayerAction with the stick at or above it (tww the same); ctor 0.2 |
| HIO 0x8C | `field_0x8C` | `mPropellerLiftScale` | flyAction 022222D0: the lift is mPropellerSpeed times it (tww temp3 = m8F8 * it); ctor 0.0002 |
| HIO 0x90 | `field_0x90` | `mFlyMaxFallSpeed` | flyAction 022222D0 maxFallSpeed while the propeller turns (tww the same); ctor -15.0 |
| HIO 0x98 | `field_0x98` | `mFlyMaxSpeedY` | flyAction 022222D0 caps speed.y at it in the air (tww ySpeedLimit = it, raised by the wind); ctor 10.0 |
| HIO 0x9C | `field_0x9C` | `mFlyMaxSpeed` | flyAction 022222D0 (cb1_flyAir): the airborne speed limit the speed is chased down to (tww temp5 = it, raised by the wind); ctor 10.0 |
| HIO 0xA0 | `field_0xA0` | `mFlyGravity` | flyAction 022222D0: added to the propeller lift as the vertical acceleration (gravity on the ground, speed.y in the air) (tww the same); ctor -2.5 |
| HIO 0xA4 | `field_0xA4` | `mFlyDriftHeight` | flyAction 022222D0 sets m8F2 (the sideways drift) once mFlyHeight passes it (tww the same); ctor 100.0 |
| HIO 0xA8 | `field_0xA8` | `mFlyTiltScale` | flyAction 022222D0 (cb1_flyAir): the body tilt is the horizontal push times it, at most 14000 (tww the same); ctor 6000.0 |
| HIO 0xB0 | `field_0xB0` | `mFlyOpenSpeedY` | flyAction 022222D0: added to speed.y when the propeller opens in the air (tww the same); ctor 34.0 |
| HIO 0xB8 | `field_0xB8` | `mFlyWindScale` | flyAction 022222D0 (cb1_flyAir) scales the wind power by it (tww power *= it); ctor 2.0 |
| HIO 0xC0 | `field_0xC0` | `mNpcCallDist` | waitNpcAction 02223048: with event bit 0x1610 set and Link nearer than it, Link's NPC call turns on (tww daPy onNpcCall()); ctor 400.0 |
| HIO 0xC4 | `field_0xC4` | `mDamageSpeedF` | execute 0222091C (cb1_execDamage): speedF away from the hit before the hit action (tww the same); ctor 20.0 |
| HIO 0xCC | `field_0xCC` | `mLandIdleMax` | flyAction 022222D0 ends a grounded flight once mLandIdleCount passes it (tww ++m8F6 > it); ctor 100 |
| HIO 0xCE | `field_0xCE` | `mFlyEndWarnTime` | flyAction 022222D0: Makar as the player plays the fly-end animation (ANM_15) while the flying timer is below it (tww getFlyingTimer() < it); ctor 15 |
| HIO 0xD0 | `field_0xD0` | `mPropellerSpeedMax` | flyAction 022222D0 chases mPropellerSpeed up to it while lifting and scales the rolling sound by 100 / it (tww the same); ctor 20000 |
| HIO 0xD2 | `field_0xD2` | `mPropellerIdleSpeed` | flyAction 022222D0 chases mPropellerSpeed to it on the ground (0 in the air) when not lifting, and a grounded flight may end at or below it (tww the same); ctor 400 |
| HIO 0xD4 | `field_0xD4` | `mPropellerAccel` | flyAction 022222D0: the chase step toward mPropellerSpeedMax (tww cLib_chaseS(&m8F8, field_0xD0, it)); ctor 2500 |
| HIO 0xD6 | `field_0xD6` | `mPropellerDecel` | flyAction 022222D0: the chase step toward mPropellerIdleSpeed / 0 (tww the same); ctor 200 |
| HIO 0xD8 | `field_0xD8` | `mFlyAnmHeight` | flyAction 022222D0 plays the flying animation (ANM_07) once mFlyHeight passes it; flyNpcAction 02223A88 starts mFlyHeight at it + 1 (tww the same); ctor 1 |
| HIO 0xDA | `field_0xDA` | `mPropellerOpenSpeed` | flyAction 022222D0: mPropellerSpeed when the propeller opens in the air (tww the same); ctor 0x2968 |
| HIO 0xDE | `field_0xDE` | `mFlyLiftPeriod` | flyNpcAction 02223A88: mFlyLiftCycle = (mFlyLiftCycle + 1) % it, lift only at 0 (tww the same); ctor 13 |
| HIO 0xE0 | `field_0xE0` | `mFlyTiltStep` | flyAction 022222D0: cLib_chaseAngleS step of shape_angle.x / z toward the tilt (or 0 on the ground) (tww the same); ctor 200 |
| HIO 0xE2 | `field_0xE2` | `mSearchTimeBase` | searchNpcAction 022244A0 reloads mSearchTimer with cLib_getRndValue(it, mSearchTimeRange) (tww the same); ctor 5 |
| HIO 0xE4 | `field_0xE4` | `mSearchTimeRange` | searchNpcAction 022244A0: the cLib_getRndValue range for mSearchTimer (tww the same); ctor 20 |
| HIO 0xE6 | `field_0xE6` | `mSearchTurnAngle` | searchNpcAction 022244A0 turns by -/+ it by mSearchSide (tww the same); ctor 3000 |
| HIO 0xE8 | `field_0xE8` | `mWalkTurnMaxStep` | walkPlayerAction 022250AC: cLib_addCalcAngleS maxStep toward the stick angle (tww the same); ctor 0x8FC |
| HIO 0xEA | `field_0xEA` | `mWalkTurnMinStep` | walkPlayerAction 022250AC: cLib_addCalcAngleS minStep (tww the same); ctor 800 |
| HIO 0xEC | `field_0xEC` | `mWalkTurnScale` | walkPlayerAction 022250AC: cLib_addCalcAngleS scale (tww the same); ctor 3 |
| AnmData 0x02 | `field_0x02` | `mMorf` | initAnm 0221FEFC passes it to dNpc_setAnm as the morf frames when morphing (tww data.field_0x02 the same) |
| AnmData 0x04 | `field_0x04` | `mLoopCount` | playAnm 02220024 switches the animation once mAnmLoopCount reaches it (tww the same) |
| AnmData 0x05 | `field_0x05` | `mNextAnmTblIdx` | playAnm 02220024 then starts table entry it (initAnm(it, 1)); 0x13 / 0x11 also call musicStop / musicPlay (tww the same) |

## daNpc_Hi1_c (d_a_npc_hi1.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_npc_hi1.cpp, which keeps the GameCube `field_0x` names) and our decomp's use. Most fields share their code with tww's daNpc_Ls1_c / daNpc_Bm1_c (named there) and our daNpc_Zl1_c. Open (evidence not enough yet): `field_0x7AD` (cleared only). Round 2 named the read-only template fields after daNpc_Zl1_c's roles (mLookAngleY, mForceEventProc, mHideUntilDemo, mKeepShapeAngle) and mNoDraw.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x8F4 | `field_0x724` | `mInitialPos` | _execute 0223D6EC copies current.pos on the first run (tww if (!field_0x7B5) { it = current.pos; ..}); written only, but the same code as tww daNpc_Bm1_c's _execute, which names this field mInitialPos (after tww daNpc_Bm1_c mInitialPos) |
| 0x900 | `field_0x730` | `mInitialAngle` | _execute 0223D6EC copies current.angle on the first run, with mInitialPos = current.pos, then mbRanExecute = 1 (tww); wait_1 0223DFB4 turns current.angle.y back to its y (tww cLib_addCalcAngleS(&current.angle.y, it.y, ..)); same code as daNpc_Bm1_c::mInitialAngle (after tww daNpc_Bm1_c mInitialPos) |
| 0x906 | `field_0x736` | `mAngle` | createInit 0223CB30 sets current.angle and copies it to shape_angle; setMtx 0223CA28 rotates the model by it (tww mDoMtx_stack_c::ZXYrotM(it)); _execute 0223D6EC current.angle each frame (tww); after tww daNpc_Ls1_c mAngle (same layout slot) |
| 0x914 | `field_0x744` | `mEyePos` | _nodeCB_Head 0223BD84: the head matrix times a_eye_pos_off (tww multVec(&a_eye_pos_off, &it)); setAttention 0223C9D4 copies it to eyePos (tww); same name as daNpc_Zl1_c::mEyePos (npcs.md) |
| 0x920 | `field_0x750` | `mLookPos` | lookBack 0223D3E8: the look-at target, the player's eye position in look mode 1, the partner's position at eye height in mode 4 (tww); same name as daNpc_Zl1_c::mLookPos (npcs.md) |
| 0x938 | `field_0x768` | `mHeadPos` | _nodeCB_Head 0223BD84: the head joint matrix's translation (tww multVecZero(&it)); same name as daNpc_Zl1_c::mHeadPos (npcs.md) |
| 0x958 | `field_0x788` | `mPartsAngle` | lookBack 0223D3E8 stores x = current.angle.y, y = the head Y angle, z = the backbone Y angle (tww); chk_parts_notMov 0223DEA0 compares them with the current ones (tww); after tww daNpc_Ls1_c mActorAngleY / mJointHeadY / mJointBackboneY (three fields there) |
| 0x960 | `field_0x790` | `mbSetEyePos` | wait_action1 0223E204 stores wait_1() / talk_1() (tww); setAttention 0223C9D4 tests it with its param (tww if (it == 0 && !param_1)); after tww daNpc_Ls1_c mbSetEyePos (same code) |
| 0x966 | `field_0x796` | `mEventIndex` | eventOrder 0223D67C sets mOrderEvent - 3 and orders mEventIdx[it] (tww); checkOrder 0223CE7C / event_proc 0223D2F0 check mEventIdx[it] (tww); after tww daNpc_Ls1_c mEventIndex (same code) |
| 0x97C | `field_0x7AC` | `mbMorfAnimStopped` | play_animation 0223C928: the morf's play() result, or 1 (tww); setAnm_anm 0223DB2C 0; after tww daNpc_Ls1_c mbMorfAnimStopped (same code) |
| 0x985 | `field_0x7B5` | `mbRanExecute` | _execute 0223D6EC: 0 on the first run, when mInitialPos and mInitialAngle are taken, then 1 (tww if (!it) { ..; it = 1; }); after tww daNpc_Bm1_c mbRanExecute (same code) |
| 0x987 | `field_0x7B7` | `mTalking` | checkOrder 0223CE7C sets 1 on the talk command (tww); wait_1 0223DFB4 starts the talk (setStt(2)) while it is set (tww); talk_1 0223E12C clears it (tww); after daNpcMt_c::mTalking (npcs.md) |
| 0x988 | `field_0x7B8` | `mLookHeadOnly` | lookBack 0223D3E8 passes it to lookAtTarget_2 as the head-only flag (d_npc.h lookAtTarget headOnly; tww bool temp4 = it); wait_1 0223DFB4 sets 1 while waiting and 0 when the talk starts (tww the same); same name as daNpc_So_c::mLookHeadOnly and daNpc_Zl1_c::mLookHeadOnly (same evidence) |
| 0x989 | `field_0x7B9` | `mTurnToInitialAngle` | wait_1 0223DFB4 turns back to mInitialAngle.y while it is set; cleared when the talk starts, set when the attention check fails (tww); wait_action1 0223E204 1 (tww) |
| 0x98A | `field_0x7BA` | `mInDemo` | demo 0223CF54: 1 while demoActorID is set (resets the joint angles when it turns on), 0 otherwise, and returns it (tww); _execute 0223D6EC skips the collision while set (tww) |
| 0x98C | `field_0x7BC` | `mAnmAtrNo` | chngAnmAtr 0223DD0C stores the attribute (tww it = param_1); setAnm_ATR 0223DCA4 plays a_anm_prm_tbl[it] (tww); checkOrder 0223CE7C / endEvent 0223D214 / setStt 0223DF68 0xFF; same name as daNpc_Zl1_c::mAnmAtrNo (npcs.md) |
| 0x98D | `field_0x7BD` | `mMesgAnimeTag` | anmAtr 0223DD28: the message's animation tag (dComIfGp_getMesgAnimeTagInfo) when it changes (tww it = tagInfo; chngAnmTag()); checkOrder / endEvent / setStt 0xFF (tww); after tww daNpc_Ls1_c mMesgAnimeTag (same code) |
| 0x98E | `field_0x7BE` | `mBtpNum` | setBtp 0223C0BC stores the btp number (tww it = param_1); play_btp_anm 0223C82C blinks only while it is 0 (tww); demo 0223CF54 1; after tww daNpc_Ls1_c mBtpNum (same code) |
| 0x98F | `field_0x7BF` | `mBtkNum` | setBtk 0223C1B4 stores the btk number (tww it = param_1); demo 0223CF54 1; after tww daNpc_Ls1_c mBtkNum (same code) |
| 0x990 | `field_0x7C0` | `mBckNum` | setAnm_anm 0223DB2C: skips an entry whose bckNum is < 0 or equal to it, else stores it (tww); createInit 0223CB30 2; after tww daNpc_Ls1_c mBckNum (same code) |
| 0x991 | `field_0x7C1` | `mOrderEvent` | eventOrder 0223D67C: 1 / 2 the talk (1 orders the speak event), 3+ orders mEventIdx[it - 3] (tww); checkOrder 0223CE7C clears it once that event starts (tww); setStt 0223DF68 / wait_1 0223DFB4 set it; after daNpc_Cb1_c::mOrderEvent |
| 0x992 | `field_0x7C2` | `mStt` | setStt 0223DF68 stores the new state and switches on it (tww); setAnm 0223DC34 plays a_anm_prm_tbl[it] (tww); wait_action1 0223E204 switches on it; same name as daNpc_Zl1_c::mStt (npcs.md) |
| 0x993 | `field_0x7C3` | `mPrevStt` | setStt 0223DF68 keeps the previous mStt here (tww); talk_1 0223E12C returns to it (tww setStt(it)); same name as daNpc_Zl1_c::mPrevStt (npcs.md) |
| 0x994 | `field_0x7C4` | `mLookMode` | lookBack 0223D3E8 switches on it: 1 the player's eyes, 2 mLookPos, 3 field_0x7AA, 4 the partner (tww); wait_1 0223DFB4 sets 0 / 1 (tww); after daNpcMt_c::mLookMode (npcs.md) |
| 0x995 | `field_0x7C5` | `mHeapSizeIdx` | decideType 0223C65C sets it (tww); _create 0223CCC4 creates the solid heap with a_heap_size_tbl[it] (tww a_siz_tbl[it]); the slot of tww daNpc_Ls1_c m854 (unnamed there) |
| 0x996 | `field_0x7C6` | `mType` | decideType 0223C65C sets it (tww); createInit 0223CB30 / getMsg_HI1_0 switch on it (tww); after tww daNpc_Ls1_c mType (same code) |
| 0x997 | `field_0x7C7` | `mActionStatus` | set_action 0223C6C0 sets 9 for the leaving action and 0 for the new one (tww); wait_action1 0223E204 switches on it and steps it (tww it++); after tww daNpc_Bs1_c mActionStatus |
| 0x998 | `field_0x7C8` | `mAnmAtrSetCnt` | anmAtr 0223DD28: while the message types, applies the message's animation attribute only when it is 0, then adds 1; MSG_DISPLAYED and setStt 0223DF68 reset it (tww) |

anm_prm_c (the a_anm_prm_tbl entries):

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `field_0x0` | `bckNum` | setAnm_anm 0223DB2C: the entry's bck animation number, compared with and stored to mBckNum (tww); after tww daNpc_Ls1_c::anm_prm_c bckNum |
| 0x01 | `field_0x1` | `btpNum` | setAnm_NUM 0223DBC4 / setAnm 0223DC34 / setAnm_ATR 0223DCA4 pass it to init_texPttrnAnm (tww); after tww daNpc_Ls1_c::anm_prm_c btpNum |
| 0x04 | `field_0x4` | `morf` | setAnm_anm 0223DB2C: the morf frames passed to dNpc_setAnm (tww); after tww daNpc_Ls1_c::anm_prm_c morf |
| 0x08 | `field_0x8` | `speed` | setAnm_anm 0223DB2C: the play speed passed to dNpc_setAnm (tww); after tww daNpc_Ls1_c::anm_prm_c speed |
| 0xC | `field_0xC` | `loopMode` | setAnm_anm 0223DB2C: the loop mode passed to dNpc_setAnm (tww); after tww daNpc_Ls1_c::anm_prm_c loopMode |

Later batch:

| Struct / offset | Old | New | Evidence |
|---|---|---|---|
| HIO 0x08 | `field_0x8` | `mRefCount` | daNpc_Hi1_HIO_c_ct 0223E2BC -1; never read in WWHD or tww; after our decomp's daNpc_Pm1_HIO_c / daNpc_Ba1_HIO_c mRefCount (the same HIO layout) |
| hio_prm_c 0x12 | `field_12` | `mMaxHeadTurnVel` | lookBack 0223D3E8 passes it to the look-at as the max turn speed (tww the same); same role as dNpc_HIO_c::mMaxHeadTurnVel |
| 0x97A | `field_0x7AA` | `mLookAngleY` | lookBack 0223D3E8: the target angle of look mode 3; never written in WWHD or tww (always 0); same role as daNpc_Zl1_c::mLookAngleY (the shared template) |
| 0x980 | `field_0x7B0` | `mForceEventProc` | _execute 0223D6EC runs event_proc while it is set even without a staff index; never written in WWHD or tww (always 0); same role as daNpc_Zl1_c::mForceEventProc (the shared template) |
| 0x982 | `field_0x7B2` | `mHideUntilDemo` | _execute 0223D6EC returns early while it is set and no demo runs and clears it in a demo; _draw 0223D9C0 draws nothing while it is set; never set in WWHD or tww (only cleared); same role as daNpc_Zl1_c::mHideUntilDemo (the shared template) |
| 0x983 | `field_0x7B3` | `mKeepShapeAngle` | hi1_setAngle (in _execute 0223D6EC) copies current.angle into shape_angle only while it is 0; demo 0223CF54 clears it; never set in WWHD or tww; same role as daNpc_Zl1_c::mKeepShapeAngle (the shared template) |
| 0x984 | `field_0x7B4` | `mNoDraw` | _draw 0223D9C0 draws nothing while it (or mHideUntilDemo) is set; never written in WWHD or tww (always 0) |

## daNpcAh_c (d_a_npc_ah.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_npc_ah.cpp, which keeps the GameCube `field_0x` names) and our decomp's use. daNpcAh_c runs the same code as daNpcMt_c for its message, order, look and animation state, so those fields take daNpcMt_c's names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x890 | `field_0x71C` | `mNextMorf` | setAnm 021E55D8: when it is >= 0 it replaces the blend (morf) of the next animation and is reset to -1 (tww f32 tempMorf = it; it = -1); Ah_actorCtor 021E5398 -1; same code as daNpcMt_c::mNextMorf (npcs.md) |
| 0x894 | `field_0x720` | `mAttnDistXZ` | createInit 021E5920: l_npc_dat mMaxAttnDistXZ (tww); chkAttention 021E5BE0: the player counts as in attention while closer than it (+40 once in attention) (tww f32 temp720 = it; if (temp720 > distXZ && ..)) |
| 0x8A2 | `field_0x72E` | `mTurnBackTimer` | chkAttention 021E5BE0: l_npc_dat mTurnBackTime when the player leaves the attention, counted down, and at 0 the NPC turns back to mHomeAngleY (tww it = l_npc_dat.field_0x50; if (it != 0) it--; else { mTargetYRot = m732; .. }) |
| 0x8A4 | `field_0x730` | `mAttnAngle` | createInit 021E5920: l_npc_dat mMaxAttnAngleY (tww); chkAttention 021E5BE0: the player counts as in attention while the angle to them is below it (+0x71C once in attention) (tww s32 temp730 = it; .. temp730 > abs(angle)) |
| 0x8A6 | `field_0x732` | `mHomeAngleY` | Ah_actorCtor 021E5398: home.angle.y (tww it = home.angle.y); chkAttention 021E5BE0 turns back to it (tww mTargetYRot = it); same code as daNpcMt_c::mHomeAngleY (npcs.md) |
| 0x8A8 | `field_0x734` | `mMsgStatus` | talk2 021E6088: the message status, 0xFFFF on a new message, then the status that next_msgStatus returned (tww it = -1; if (it == fopMsgStts_MSG_CONTINUES_e) ..; it = msg_status); same code as daNpcMt_c::mMsgStatus (npcs.md) |
| 0x8B6 | `field_0x742` | `mTalking` | checkOrder 021E5F38 sets 1 on the talk command (tww); executeTalk 021E6CAC clears it; executeCommon 021E6BC0 returns it and enters the talk mode while it is 1 (tww); lookBack 021E6680 tests it (tww); same code as daNpcMt_c::mTalking (npcs.md) |
| 0x8B7 | `field_0x743` | `mInAttention` | chkAttention 021E5BE0: 1 once the player comes into the attention range, 0 when they leave (tww); executeCommon 021E6BC0 sets mOrderState 1 while it is set, else 0 (tww); createInit 021E5920 0; same code as daNpcMt_c::mInAttention (npcs.md) |
| 0x8B8 | `field_0x744` | `mOrderState` | executeCommon 021E6BC0 1 / 0 from mInAttention; eventMesSet 021E623C 3 (tww); checkOrder 021E5F38 starts the talk for 1 / 2 and eventOrder 021E64E0 orders by it (tww); same code as daNpcMt_c::mOrderState (npcs.md) |
| 0x8BB | `field_0x747` | `mResFlag` | TWW accessor setResFlag(u8 flag) { field_0x747 = flag; }: Ah_actorCtor 021E5398 setResFlag(0), phase_1 021E549C setResFlag(1); _delete 021E5B78 deletes the resource only while it is set (tww) |
| 0x8BF | `field_0x74B` | `mAnmTimer` | setAnmTbl 021E56E4: mpAnmDat->mLoopCount (tww it = mpAnmDat->field_0x02); playAnm 021E65B0 counts it down and acts at 0 (tww if (it > 0) { it += -1; if (it == 0) .. }); same code as daNpcMt_c::mAnmTimer (npcs.md) |
| 0x8C2 | `field_0x74E` | `mLookMode` | chkAttention 021E5BE0: 1 looks at mEyePos, 2 faces mTargetYRot, 0 neither (tww); lookBack 021E6680 switches on it (tww); Ah_actorCtor 021E5398 0; same code as daNpcMt_c::mLookMode (npcs.md) |

Round 2:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x8BC | `field_0x748` | `mItemFlags` | eventMesSet 021E623C: when the box closes, bit 0x1 gives item 7 and bit 0x2 item 5 (mItemNo, mOrderState 3), each bit cleared once used (tww the same); never set in WWHD or tww |
| 0x8BE | `field_0x74A` | `mAnmFlags` | playAnm 021E65B0: clears bit 0x1 each call and sets it when the animation table moves on to its next entry (setAnmTbl returns true) (tww the same); never read in WWHD or tww |
| 0x8C3 | `field_0x74F` | `mBodyTurn` | createInit 021E5920 copies l_npc_dat mBodyTurn; chkAttention 021E5BE0 turns the body (mHeadOnlyFollow 0, m_jnt.mbTrn 1) while set, else the head only; lookBack 021E6680 turns the body while talking with it set (tww the same); same role as daNpcPeople_c::mBodyTurn |
| 0x8C4 | `field_0x750` | `mLookAtPlayer` | createInit 021E5920 copies l_npc_dat mLookAtPlayer; chkAttention 021E5BE0: while 0 the player in range only turns him to mHomeAngleY (mLookMode 2) instead of being looked at (tww the same); same role as daNpcPeople_c::mLookAtPlayer |

Open (evidence not enough yet): `field_0x6F4`, `field_0x72A`, `field_0x738`, `field_0x74D` (no use); `field_0x718`, `field_0x72C`, `field_0x736` (only cleared); `field_0x751` (written only: set while the player is looked at from the look distance outside the attention; daNpcMt_c's counterpart m752 is open too).

### NpcDatStruct and sAhAnmDat (d_a_npc_ah.h; the npc data l_npc_dat and the animation table entry)

Old names are the GameCube offsets (zeldaret/tww `field_0x..`; tww names only the m_jnt limits, kept). Where the role matches, the names are those of the photo / roten NpcDatStruct and sPhotoAnmDat.

| Struct | Old | New | Evidence |
|---|---|---|---|
| sAhAnmDat | `field_0x02` | `mLoopCount` | setAnmTbl 021E56E4: the entry's count, copied into mAnmTimer; above 0 plays it that often, else loops (tww the same); same code as sPhotoAnmDat::mLoopCount |
| NpcDatStruct | `field_0x14` | `mPlayerEyeOffsetY` | chkAttention 021E5BE0: passed to dNpc_playerEyePos, the look target above the player's head (tww the same); same role as NpcDatStruct::mPlayerEyeOffsetY (photo, roten) |
| NpcDatStruct | `field_0x18` | `mAttnOffsetX` | _execute 021E688C: attention_info.position = current.pos + the vector (it, mAttnOffsetY, mAttnOffsetZ) Y-rotated by current.angle.y (tww the same); same role as daNpcPhoto_c NpcDatStruct::mAttnOffsetX |
| NpcDatStruct | `field_0x1C` | `mAttnOffsetY` | _execute 021E688C: the Y of the attention position offset (see mAttnOffsetX) (tww the same); same role as daNpcPhoto_c NpcDatStruct::mAttnOffsetY |
| NpcDatStruct | `field_0x20` | `mAttnOffsetZ` | _execute 021E688C: the Z of the attention position offset (see mAttnOffsetX) (tww the same); same role as daNpcPhoto_c NpcDatStruct::mAttnOffsetZ |
| NpcDatStruct | `field_0x24` | `mEyeYOffset` | _execute 021E688C: eyePos = current.pos + (0, it, 0) (tww the same); same role as NpcDatStruct::mEyeYOffset (photo, roten) |
| NpcDatStruct | `field_0x28` | `mMaxAttnDistXZ` | createInit 021E5920 copies it into mAttnDistXZ, chkAttention 021E5BE0's attention distance (tww the same); same role as NpcDatStruct::mMaxAttnDistXZ (photo, roten) |
| NpcDatStruct | `field_0x2C` | `mLookDistXZ` | chkAttention 021E5BE0: outside the attention, the player is still looked at while closer than it (tww the same); same role as MtNpcDat::mLookDistXZ |
| NpcDatStruct | `field_0x30` | `mMaxAttnAngleY` | createInit 021E5920 copies it into mAttnAngle, chkAttention 021E5BE0's attention angle (tww the same); same role as NpcDatStruct::mMaxAttnAngleY (photo, roten) |
| NpcDatStruct | `field_0x32` | `mMaxHeadTurnVel` | chkAttention 021E5BE0: mTargetAngle, the speed lookBack 021E6680 eases the look toward (tww the same); same role as NpcDatStruct::mMaxHeadTurnVel (photo, roten) |
| NpcDatStruct | `field_0x38` | `mCylRadius` | createInit 021E5920 / _execute 021E688C: setCollision(it, 150), the body cylinder's radius (tww the same); same role as daNpcRoten_c NpcDatStruct::mCylRadius |
| NpcDatStruct | `field_0x50` | `mTurnBackTime` | chkAttention 021E5BE0 loads mTurnBackTimer with it when the player leaves the attention (tww the same); same role as MtNpcDat::mTurnBackTime |
| NpcDatStruct | `field_0x52` | `mBodyTurn` | createInit 021E5920 copies it into daNpcAh_c::mBodyTurn; chkAttention 021E5BE0 turns the body while it is set (mHeadOnlyFollow 0), the head only while 0; lookBack 021E6680 turns the body while talking with it set (tww the same); same role as daNpcPhoto_c NpcDatStruct::mBodyTurn |
| NpcDatStruct | `field_0x53` | `mLookAtPlayer` | createInit 021E5920 copies it into daNpcAh_c::mLookAtPlayer; chkAttention 021E5BE0 turns back home after the look while 0 (tww the same); same role as daNpcPhoto_c NpcDatStruct::mLookAtPlayer |

Open (never read in WWHD or tww): NpcDatStruct `field_0x12`, `field_0x34`, `field_0x36`, `field_0x3C`, `field_0x40`, `field_0x44`, `field_0x46`, `field_0x48`, `field_0x4A`, `field_0x4C`, `field_0x4E`.

## daNpc_Bm1_c (d_a_npc_bm1.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_npc_bm1.cpp; tww names part of this class and keeps the GameCube offset names `mXXX` for the rest) and our decomp's use (d_a_npc_bm1.cpp, _b.cpp, _pending.cpp). The look, order and animation-attribute fields share their code with tww's daNpc_Ls1_c and our daNpc_Hi1_c. Open (evidence not enough yet): `m880` (written only), `m881`, `m882` (read only, never written in tww), `m88D`..`m88F`, `m8C8`, `m8CC` (no use) and the padding arrays.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x988 | `m818` | `mInitialAngle` | _execute 02200D58 copies current.angle on the first run, with mInitialPos (tww if (!mbRanExecute) { mInitialPos = current.pos; ..; }); wait_3 02203508 / wait_4 02203124 turn current.angle.y back to its y and h_wait 02203348 / wait_7 02203414 face it (tww); wait_action2 02203A9C turns it with the Kogoli start; after tww daNpc_Bm1_c mInitialPos (the position half) |
| 0x99C | `m82C` | `mLookPos` | lookBack 021FFFFC looks at it (tww dstPos = it); createInit 021FDB44 current.pos, chg_anmAtr 0220134C the partner's eyePos, eInit_ATTENTION_ 021FF344 the cut's relative position (tww); same name as daNpc_Zl1_c / daNpc_Hi1_c::mLookPos (npcs.md) |
| 0x9C8 | `m858` | `mPassDist` | bm_movPass 0220057C: the path point counts as reached once the XZ distance is <= it (tww if (dist_xz <= it)); eInit_FLY_ 021FEECC / eInit_WLK_ 021FF5D0 / setStt 02202838 set the HIO fly / walk value (tww hio_prm.m30 / m44) |
| 0x9CC | `m85C` | `mJointHeadY` | lookBack 021FFFFC stores the head Y angle (tww it = m_jnt.getHead_y()); chk_partsNotMove 022016BC compares it (tww); after tww daNpc_Ls1_c mJointHeadY (same code) |
| 0x9CE | `m85E` | `mJointBackboneY` | lookBack 021FFFFC stores the backbone Y angle (tww it = m_jnt.getBackbone_y()); chk_partsNotMove 022016BC compares it (tww); after tww daNpc_Ls1_c mJointBackboneY (same code) |
| 0x9D0 | `m860` | `mActorAngleY` | lookBack 021FFFFC stores current.angle.y (tww); chk_partsNotMove 022016BC compares it (tww); after tww daNpc_Ls1_c mActorAngleY (same code) |
| 0x9DC | `m86C` | `mEvTimer` | eInit_setEvTimer 021FF328 sets the cut's timer (tww it = *i_param_1, else 0); eMove_ATTENTION_ 021FFCA8 / eMove_WLK_ 021FFCFC count it down (tww cLib_calcTimer(&it)); named after eInit_setEvTimer |
| 0x9DE | `m86E` | `mLookTimer` | wait_1 02202C2C / wait_4 02203124 / h_wait 02203348 / wait_7 02203414 / wait_3 02203508 reset it to 0x28 while the player is in attention and look at the player (mLookBackState 1) while it counts down (tww); talk_1 02202CF0 0x28; setStt 02202838 0 |
| 0x9E0 | `m870` | `mSttTimer` | setStt 02202838 sets it per state (0x50, 0x5A, 180 + rnd(180), 0x3C) (tww); flyawy 02203200 deletes the actor, wait_3 02203508 / CHKwai 02203830 change the state when it runs out (tww cLib_calcTimer(&it) == 0) |
| 0x9E6 | `m876` | `mDesiredYRot` | lookBack 021FFFFC: the look-at's default Y angle (tww desiredYrot = it); h_wait 02203348 / wait_7 02203414 set mInitialAngle.y (tww); after daNpcMt_c::mDesiredYRot (npcs.md) |
| 0x9EB | `m87B` | `mAnmLoopCount` | setAnm_anm 021FEAB4 0 (tww); control_anmAtr 0220141C counts the morf stops in attribute 8 and switches to attribute 6 at 2 (tww if (++it >= 2)); after daNpc_Cb1_c::mAnmLoopCount |
| 0x9ED | `m87D` | `mPreItemNo` | chk_talk 0220163C: dComIfGp_event_getPreItemNo(), else 0xFF (tww); getMsg_BMB_2 022020EC picks the message by it (tww if (it == 0x47)); talk_1 02202CF0 0xFF |
| 0x9EE | `m87E` | `mPointArg` | walk_1 02203734: mPathRun.pointArg of the current point (tww it = mPathRun.pointArg(mPathRun.getIdx())) and tests it (tww) |
| 0x9EF | `m87F` | `mbResLoadIsComplete` | _create 021FDF08: the phase result == cPhs_COMPLEATE_e (tww it = state == cPhs_COMPLEATE_e); _delete tests it before deleting the resources (tww); after tww daNpc_Ls1_c mbResLoadIsComplete |
| 0x9F7 | `m887` | `mSkipPosMove` | _execute 02200D58 clears it and runs fopAcM_posMoveF only while it stays 0 (tww if (!it) fopAcM_posMoveF(..)); bm_flyMove 02200714 / bm_nMove 02200AF8 set 1 once their move is done (tww) |
| 0x9F8 | `m888` | `mNoDraw` | init_PST_4 021FCA68 1 (tww); eInit_INI_EVN_1_ 021FE768 0 (tww); _draw 02200F90 draws nothing while it is set (tww if (mbInitPostman0 \|\| it) return TRUE); after daNpc_Zl1_c::mNoDraw (npcs.md) |
| 0x9F9 | `m889` | `mFlyDescend` | bm_movPass 0220057C: 1 when the next point is not above the current one (tww it = cmpCoords.y <= idxCoords.y); bm_setFlyAnm 021FEEB0 / bm_clcFlySpd 02200420 pick the descending animation / speed by it; eInit_FLY_ 021FEECC / setStt 02202838 1 (tww) |
| 0x9FA | `m88A` | `mFlyArrived` | bm_flyMove 02200714: set when the flight sequence ends (the landing states) and then makes bm_flyMove return done at once (tww if (it != 0) { m887 = 1; return true; }); eInit_FLY_ 021FEECC / eMove_FLY_ 021FFC84 / eMove_KMA_FLY_ 021FFCA0 / flyawy 02203200 clear or test it (tww) |
| 0x9FB | `m88B` | `mWalkArrived` | bm_nMove 02200AF8: set when bm_movPass reports the last point, and then makes bm_nMove skip the move (tww if (it) { m887 = 1; return; } .. it = 1); walk_1 02203734 tests and clears it (tww) |
| 0xA05 | `m895` | `mTalking` | checkOrder 021FE408 sets 1 on the talk command (tww); the waits (wait_1 02202C2C ..) start the talk while it is set; talk_1 02202CF0 clears it (tww); after daNpcMt_c / daNpc_Hi1_c::mTalking (npcs.md) |
| 0xA06 | `m896` | `mHeadOnlyFollow` | lookBack 021FFFFC passes it as the head-only flag of the joint look-at (tww bool headOnlyFollow = it); eInit_setLocFlag 021FF2B8 / the waits set 0 / 1 (tww); after daNpcMt_c::mHeadOnlyFollow (npcs.md) |
| 0xA68 | `m8F4` | `mFlyState` | bm_flyMove 02200714 / bm_clcFlySpd 02200420 switch on it (0 not flying, 1..5 the take-off, flight and landing steps) (tww); eInit_FLY_ 021FEECC / eMove_FLY_ 021FFC84 / setStt 02202838 set and test it (tww return it == 0) |
| 0xA6B | `m8F7` | `mAnmAtrNo` | chg_anmAtr 0220134C stores the attribute and switches on it (tww it = i_param_1); control_anmAtr 0220141C / setAnm_ATR 022012DC / anmAtr 02201460 / endEvent 021FE6CC / setStt 02202838 use it; same name as daNpc_Zl1_c / daNpc_Hi1_c::mAnmAtrNo (npcs.md) |
| 0xA6C | `m8F8` | `mMesgAnimeTag` | anmAtr 02201460: the message's animation tag when it changes (tww if (uVar1 != 0xFF && it != uVar1) { it = uVar1; .. }); after tww daNpc_Ls1_c mMesgAnimeTag (same code) |
| 0xA71 | `m8FD` | `mOrderEvent` | eventOrder 02200CD0 orders by it, checkOrder 021FE408 clears it once the talk / event starts, the waits set 2 (tww); event_proc 0220026C 1 (tww); after daNpc_Cb1_c / daNpc_Hi1_c::mOrderEvent |
| 0xA73 | `m8FF` | `mPrevStatus` | setStt 02202838 keeps the previous mStatus here (tww it = previous_status); talk_1 02202CF0 / talk_2 02202FF4 / manzai 02203084 return to it (tww setStt(it)); after tww daNpc_Bm1_c mStatus |
| 0xA78 | `m904` | `mActionStatus` | set_action 021FC758 sets 9 for the leaving action and 0 for the new one (tww); the wait_action / demo_action functions switch on it and step it; partner_srch 021FE2F8 (tww); after tww daNpc_Bs1_c mActionStatus |
| 0xA79 | `m905` | `mAnmAtrSetCnt` | anmAtr 02201460: while the message types, applies the message's animation attribute only when it is 0, then adds 1; MSG_DISPLAYED resets it (tww); same code as daNpc_Hi1_c::mAnmAtrSetCnt |
| 0x9E2 | `m872` | `mWaitAnmLoops` | setStt 02202838: 1..4 ((l_counter & 3) + 1) when state 0x10 starts; wait_8 02203648 counts it down at each animation end and returns to state 8 at 0 (tww the same) |
| 0x9EC | `m87C` | `mPostEventMsg` | event_proc 0220026C sets 1 when event 0 ends; getMsg_BMB_2 022020EC / getMsg_BMC_2 022022B4 then answer 0x199C / 0x1EF7; talk_1 02202CF0 clears it after message 0x1EF7 (tww the same) |

### Bm1SetupHioParam / Bm1BehaviorHioPrm (the HIO child's hio_prm; d_a_npc_bm1.h / d_a_npc_bm1_b.h)

Round 3: two views of tww's daNpc_Bm1_childHIO_c::hio_prm_c (the per-type Rito parameters), named alike. The HIO constructor (Bm1BehaviorHio_ct 02204158) fills each child by memcpy from a_prm_tbl; no field is written elsewhere in WWHD or tww. The reads go through hio_prm(mType) in our d_a_npc_bm1.cpp / d_a_npc_bm1_b.cpp and match zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x20 | `m20` | `mFlySpeed` | daNpc_Bm1_c::eInit_FLY_ 021FEECC: mTargetFlySpeed when the event gives no speed (tww the same) |
| 0x24 | `m24` | `mFlyAccel` | daNpc_Bm1_c::eInit_FLY_ 021FEECC: mTargetFlyStep (speedF's chase step) when the event gives no accel (tww the same) |
| 0x28 | `m28` | `mFlyUpSpeed` | daNpc_Bm1_c::eInit_FLY_ 021FEECC / setStt 02202838: mFlySpeedY (the event's spd_y default) (tww the same) |
| 0x2C | `m2C` | `mFlyUpAccel` | daNpc_Bm1_c::eInit_FLY_ 021FEECC / setStt 02202838: mFlyAccelY (the event's acc_y default) (tww the same) |
| 0x30 | `m30` | `mFlyPassDist` | daNpc_Bm1_c::eInit_FLY_ 021FEECC / setStt 02202838: mPassDist while flying (tww m858 the same) |
| 0x34 | `m34` | `mTurnScale` | daNpc_Bm1_c::bm_clcMovSpd 02200A40: cLib_addCalcAngleS's scale for turning to mTargetPos (tww the same) |
| 0x36 | `m36` | `mTurnMaxStep` | daNpc_Bm1_c::bm_clcMovSpd 02200A40: cLib_addCalcAngleS's largest step for turning to mTargetPos (tww the same) |
| 0x38 | `m38` | `mWalkAnmSpeed` | daNpc_Bm1_c::bm_nMove 02200AF8: the walk animations (0xE / 0x14) play at speedF * it (at least 0.5) (tww the same) |
| 0x3C | `m3C` | `mWalkSpeed` | daNpc_Bm1_c::eInit_WLK_ 021FF5D0 / setStt 02202838: mTargetFlySpeed for walking (the event's speed default) (tww the same) |
| 0x40 | `m40` | `mWalkAccel` | daNpc_Bm1_c::eInit_WLK_ 021FF5D0 / setStt 02202838: mTargetFlyStep for walking (the event's accel default) (tww the same) |
| 0x44 | `m44` | `mWalkPassDist` | daNpc_Bm1_c::eInit_WLK_ 021FF5D0 / setStt 02202838: mPassDist while walking (tww m858 the same) |
| 0x48 | `m48` | `mEventOrderDist` | daNpc_Bm1_c::wait_5 0220325C: event 4 is ordered (mOrderEvent) when the player's XZ distance is below it and the height difference within -40..300 (tww the same) |

Open (no name yet): the child's `m50` (Bm1SetupChildHio / Bm1BehaviorChildHio: set to the child's index by the constructor, read nowhere); padding `_19`, `_1E`.

## daNpc_Ym1_c (d_a_npc_ym1.h; views ym1_b::State in d_a_npc_ym1_b.h, Ym1StateView and Ym1AngleView in d_a_npc_ym1_a2.h, Ym1PartAState in d_a_npc_ym1_readable.h)

The class header had WWHD offset names (`mA0C`); the views kept GameCube names (`m89C` = WWHD 0xA0C) or already had readable names. zeldaret/tww has no code for this class. Fields take the readable view name where one existed, otherwise a name from the WWHD uses; Mesa and Abe follow the same NPC template as daNpc_Pf1_c, so fields with the same code take daNpc_Pf1_c's names. The GameCube-named view fields follow the class (`was m89C: view of daNpc_Ym1_c ...`). Ym1PartAState called 0xA1B `mAnimationAttribute`; it is the animation number (setAnm_anm 022F9170 stores anm_prm_c::mAnmNum there; the message animation attribute is 0xA18), corrected to `mAnmNum` in its own commit (`was mAnimationAttribute (misnamed)`). One name per field: Ym1PartAState `mMorfAnimationStopped` and `mActionState` became the class / State names `mAnimationStopped` and `mActionPhase` (`was ... (misnamed)`, own commit). Open: `m9A4`, `m9A8`, `m9AC`, `m9B0`, `m9B4` (_execute 022F8C24 stores the home position words and angles once, never read), `m9FC`, `mA02`, `mA04` (no use), `mA10` / views `m8A0` (read by _draw 022F8F30, never written in WWHD; tww has no code).

| WWHD offset | Old name | New | Evidence |
|---|---|---|---|
| 0x7E8 | `m7E8` | `mItemModel` | itemCreateHeap 022F7114: mDoExt_J3DModel__create(a_mdl_dat, ..) (nullptr without one); setMtx 022F7B28 places it on the hand; _draw 022F8F30 draws it while set; view Ym1PartAState mItemModel (d_a_npc_ym1_readable.h); GameCube m6D0 |
| 0x9B6 | `m9B6` | `mModelAngle` | _execute 022F8C24 / createInit 022F7D68: current.angle every frame; setMtx 022F7B28: mDoMtx_ZXYrotM by it; view Ym1PartAState mModelAngle; GameCube m846 |
| 0x9BC | `m9BC` | `mEyePosition` | _nodeCB_Head 022F68C4: the head matrix times a_eye_pos_off; setAttention 022F7A60 copies it to eyePos; view Ym1PartAState mEyePosition |
| 0x9C8 | `m9C8` | `mLookPos` | lookBack 022F8670: the player's eye position in look mode 1, the look-at target in mode 2 (after our decomp's daNpc_Pf1_c mLookPos, same code) |
| 0x9D4 | `m9D4` | `mNbtAttentionPosition` | setAttention 022F7A60: the attention position for animations 5-7 (chk_nbt_attn); views ym1_b::State and Ym1PartAState mNbtAttentionPosition |
| 0x9E0 | `m9E0`, `m9E4`, `m9E8` | `mHeadPosition` (cXyz) | _nodeCB_Head 022F68C4: the head joint's position; setAttention 022F7A60: y - 210 for animation 6; view Ym1PartAState mHeadPosition. First named as three f32 `mHeadPositionX`..`Z`, then made one cXyz in its own type-fix commit (objcmp identical) |
| 0x9EC | `m9EC` | `mPreviousMorfFrame` | play_animation 022F79D0: the morf frame of the last frame (a smaller one means the animation looped); setAnm_anm 022F9170 0; views State / Ym1PartAState mPreviousMorfFrame |
| 0x9F0 | `m9F0` | `mPreviousAngles` | lookBack 022F8670: x = the target angle, y / z = the head / backbone angle; chk_parts_notMov 022F9B64 compares them with the current ones; view State mPreviousAngles |
| 0x9F8 | `m9F8` | `mEyePositionEnabled` | setAttention 022F7A60 copies mEyePosition to eyePos while set; the wait/talk actions store their result; views State / Ym1PartAState mEyePositionEnabled |
| 0x9FE | `m9FE` | `mAttentionTimer` | wait_1 022F9EF4 / wait_2 022FA0A0: cLib_getRndValue(15, 30) and counted down before turning; talk_1 022FA288 resets it; view State mAttentionTimer |
| 0xA06 | `mA06` | `mLookAngleY` | lookBack 022F8670: the target angle in look mode 3 (after our decomp's daNpc_Pf1_c mLookAngleY, same code) |
| 0xA08 | `mA08` | `mAnimationStopped` | play_animation 022F79D0: mpMorf->play()'s result, or 1 when the frame went back; setAnm_anm 022F9170 0; NBTwai 022FA5B8 tests it; view State mAnimationStopped (Ym1PartAState: mMorfAnimationStopped) |
| 0xA09 | `mA09` | `mAnimationWrapped` | setAnm_anm 022F9170: 0 with a new animation; view State mAnimationWrapped |
| 0xA0A | `mA0A` | `mPresentItem` | chk_talk 022F9ACC: dComIfGp_event_getPreItemNo() on a talk with an item, else 0xFF; talk_1 022FA288 0xFF; view State mPresentItem |
| 0xA0C | `mA0C` | `mPrevKariFlag` | _draw 022F8F30: mKariFlag of this frame (mKariFlag is then cleared); kari_1 022F9E00 goes back to waiting at 0, turn_1 022FA4E4 starts kari again while set (GameCube m89C) |
| 0xA0D | `mA0D` | `mbResLoaded` | _create 022F8108: dComIfG_resLoad(..) == cPhs_COMPLEATE_e, create waits while 0 (after our decomp's daNpc_Pf1_c mbResLoaded, same code; GameCube m89D) |
| 0xA0E | `mA0E` | `mbWaitForDemo` | _execute 022F8C24: no execution while set and no demo actor, cleared while a demo runs; _draw 022F8F30 draws nothing while set (after our decomp's daNpc_Pf1_c mbWaitForDemo, same code; GameCube m89E) |
| 0xA0F | `mA0F` | `mbNoShapeAngle` | _execute 022F8C24: shape_angle follows current.angle only while 0; demo 022F8344 clears it (after our decomp's daNpc_Pf1_c mbNoShapeAngle, same code; GameCube m89F) |
| 0xA11 | `mA11` | `mbHomeSet` | _execute 022F8C24: 0 stores the home position words, angles and mRotYTarget once, then 1 (after our decomp's daNpc_Pf1_c mbHomeSet, same code) |
| 0xA12 | `mA12` | `mPlayerAttending` | wait_1 022F9EF4 / wait_2 022FA0A0 / SITwai 022FA70C: the attention result, resets mAttentionTimer while set; view State mPlayerAttending |
| 0xA13 | `mA13` | `mTalkRequested` | checkOrder 022F8304: 1 on the talk command while mActionSubstate is 1/2; the wait actions start the talk while set, talk_1 022FA288 clears it; set_collision_sp 022F89CC skips while set; view State mTalkRequested |
| 0xA14 | `mA14` | `mbHeadOnly` | lookBack 022F8670: the head-only flag of the look-at; kari_1 022F9E00, wait_1 and the other actions set 1 / 0 with the look mode (after our decomp's daNpc_Pf1_c mbHeadOnly, same code; GameCube m8A4) |
| 0xA15 | `mA15` | `mTurnToPlayer` | wait_1 022F9EF4 / wait_2 022FA0A0: turn current.angle.y to mRotYTarget while set; NBTwai 022FA5B8 sets it; view State mTurnToPlayer |
| 0xA16 | `mA16` | `mbDemo` | demo 022F8344: 1 while demoActorID is set (returned), else 0; _execute 022F8C24 / set_cutGrass 022F88C8 skip while set (after our decomp's daNpc_Pf1_c mbDemo, same code) |
| 0xA17 | `mA17` | `mActionIndex` | privateCut 022F8588: dComIfGp_evmng_getMyActIdx(staff, a_cut_tbl, ..), -1 ends (after our decomp's daNpc_Pf1_c mActionIndex, same code) |
| 0xA18 | `mA18` | `mAnimationAttribute` | chngAnmAtr 022F9378: the message animation attribute (< 0xD); setAnm_ATR 022F9310 plays its table entry; endEvent 022F8544 / setStt 022F9C2C -1; view State mAnimationAttribute |
| 0xA19 | `mA19` | `mAnimationTag` | anmAtr 022F9394: the last message animation tag taken; endEvent 022F8544 / setStt 022F9C2C -1; view State mAnimationTag |
| 0xA1A | `mA1A` | `mBtpNum` | init_texPttrnAnm 022F6F38: the btp number; play_texPttrnAnm 022F7918 blinks only at 0; demo 022F8344 1 with the demo's btp; view Ym1PartAState mBtpNum |
| 0xA1B | `mA1B` | `mAnmNum` | setAnm_anm 022F9170: anm_prm_c::mAnmNum of the animation set (no change when equal); createInit 022F7D68 0xD; set_cutGrass 022F88C8 / set_collision_sp 022F89CC switch on it; chk_nbt_attn 022F7A3C tests 5-7 (GameCube m8AB) |
| 0xA1C | `mA1C` | `mActionSubstate` | the wait actions set 2; checkOrder 022F8304 starts the talk at 1/2 and clears it; eventOrder 022F8890 orders the speak event at 1, can-talk at 1/2; view State mActionSubstate |
| 0xA1D | `mA1D` | `mStatus` | setStt 022F9C2C: the state; wait_action1 022FA828 .. run by it; setAnm 022F92A0 plays its table entry; view State mStatus |
| 0xA1E | `mA1E` | `mReturnStatus` | setStt 022F9C2C: the state before a talk; talk_1 022FA288 goes back to it; view State mReturnStatus |
| 0xA1F | `mA1F` | `mLookMode` | lookBack 022F8670: 1 player's eyes, 2 mLookPos, 3 mLookAngleY; the actions set it (after our decomp's daNpc_Pf1_c mLookMode, same code; GameCube m8AF) |
| 0xA22 | `mA22` | `mActionPhase` | set_action 022F7388: 9 when the old action ends, 0 for the new; wait_action1 022FA828 .. set up at 0 and step to 1; view State mActionPhase (Ym1PartAState: mActionState) |
| 0xA23 | `mA23` | `mMessageAnimationState` | anmAtr 022F9394: 0 takes the message's animation attribute once and steps to 1, reset when the message is displayed; view State mMessageAnimationState |

## daNpc_Ls1_c (d_a_npc_ls1.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_npc_ls1.cpp; tww names part of this class and keeps the GameCube offset names `mXXX` for the rest) and our decomp's use. The remaining fields run the same code as daNpc_Hi1_c / daNpc_Bm1_c and take their names. Open (evidence not enough yet): `m82E` (read only: the look mode 3 angle), `m831` (cleared only), `m835`, `m836` (event-to-message flags), `m83A`, `m83C`, `m820`, `m821` (HD telescope bytes) and the padding arrays.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x960 | `m790` | `mPId` | lookBack 02280C40: the partner looked at in look mode 4 (tww searchByID(it, &param)); read only, never written in WWHD or tww; after tww daNpc_Hi1_c mPId (same lookBack code) |
| 0x964 | `m794` | `mInitialPos` | _execute 02281074 copies current.pos on the first run (tww if (!m83D) { it = current.pos; ..}); written only; after tww daNpc_Bm1_c mInitialPos (same code) |
| 0x970 | `m7A0` | `mInitialAngle` | _execute 02281074 copies current.angle on the first run (tww); wait_2 02282B54 turns current.angle.y back to its y, wait_3 02282D94 rotates by it (tww); wait_action1 02283038 sets its y (tww); same code as daNpc_Bm1_c / daNpc_Hi1_c::mInitialAngle |
| 0x99C | `m7CC` | `mPlayerEvnPos` | wait_action1 02283038: get_playerEvnPos(0) / (1) (tww it[0] = get_playerEvnPos(0)); checkOrder 0227FFEC turns to [0], cut_init_PLYER_MOV 022807DC sets the goal [1], telescope_proc 022825A0 puts the player at [1] (tww) |
| 0x9B4 | `m7E4` | `mHeadPos` | _nodeCB_Head 0227D7DC: the head joint matrix's translation (tww it.x = mDoMtx_stack_c::get()[0][3] ..); same name as daNpc_Zl1_c / daNpc_Hi1_c::mHeadPos (npcs.md) |
| 0x9E0 | `m810` | `mTelescopeMsgNo` | telescope_proc 022825A0: the message number of the telescope talk (tww it = 0xBC1; temp_msg_no = it; it = -1 / temp) |
| 0xA03 | `m833` | `mTelescopeStep` | telescope_proc 022825A0 switches on it and steps it (0 .. 4) (tww switch (it)); setStt 022805B8 0 (tww) |
| 0xA04 | `m834` | `mRunEventProc` | checkOrder 0227FFEC sets 1 when the telescope event starts (tww); _execute 02281074 runs event_proc while it is set (tww if (staff_id >= 0 \|\| it) event_proc(..)); event_proc 02280A1C clears it at the end (tww) |
| 0xA0D | `m83D` | `mbRanExecute` | _execute 02281074: 0 on the first run, when mInitialPos and mInitialAngle are taken, then 1 (tww); after tww daNpc_Bm1_c mbRanExecute (same code) |
| 0xA0F | `m83F` | `mTalking` | checkOrder 0227FFEC sets 1 on the talk command (tww); wait_2 02282B54 / wait_3 02282D94 / wait_4 02282EB0 start the talk while it is set; talk_1 02282F6C clears it (tww); after daNpc_Hi1_c / daNpcMt_c::mTalking |
| 0xA11 | `m841` | `mInDemo` | demo 022801BC: 1 while demoActorID is set, 0 otherwise (tww); setMtx 0227F6F0 / _execute 02281074 test it (tww); same code as daNpc_Hi1_c::mInDemo |
| 0xA16 | `m846` | `mEyeAngleY` | lookBack 02280C40: -mHalfHeadAngleY (tww it = -mHalfHeadAngleY); eye_ctrl 0227F2DC moves the eye texture by it / HIO m30 (tww); after daNpc_Zl1_c::mEyeAngleY (npcs.md) |
| 0xA18 | `m848` | `mEyeAngleX` | lookBack 02280C40: mHalfHeadAngleX (tww it = mHalfHeadAngleX); eye_ctrl 0227F2DC moves the eye texture by it / HIO m30 (tww); after daNpc_Zl1_c::mEyeAngleX (npcs.md) |
| 0xA1B | `m84B` | `mAnmAtrNo` | chngAnmAtr 02281758 stores the attribute (tww it = param_1); setAnm_ATR 022816E4 / ctrlAnmAtr 022818EC use it; setStt 022805B8 / endEvent 022806DC / checkOrder 0227FFEC 0xFF; same code as daNpc_Hi1_c::mAnmAtrNo |
| 0xA20 | `m850` | `mOrderEvent` | eventOrder 02281004: 1 / 2 the talk, 3+ orders mEventIDTbl[it - 3] (tww mEventIndex = it - 3); checkOrder 0227FFEC clears it; the waits set it (tww); same code as daNpc_Hi1_c::mOrderEvent |
| 0xA21 | `m851` | `mStt` | setStt 022805B8 stores the new state (tww u8 prev_m851 = it; it = param_1); setAnm 02280548 plays a_anm_prm_tbl[it] (tww); same code as daNpc_Hi1_c::mStt |
| 0xA22 | `m852` | `mPrevStt` | setStt 022805B8 keeps the previous mStt (tww it = prev_m851); talk_1 02282F6C returns to it (tww setStt(it)); same code as daNpc_Hi1_c::mPrevStt |
| 0xA23 | `m853` | `mLookMode` | lookBack 02280C40 switches on it (tww switch (it)); wait_1 02282A60 .. wait_4 / cut_init_LOK_PLYER 02280720 set it (tww); same code as daNpc_Hi1_c::mLookMode |
| 0xA24 | `m854` | `mHeapSizeIdx` | decideType 0227EE98 sets it (tww); _create 0227FBF0 creates the solid heap with a_siz_tbl[it] (tww); same code as daNpc_Hi1_c::mHeapSizeIdx |
| 0xA26 | `m856` | `mActionStatus` | set_action 0227EFF4 sets 9 for the leaving action and 0 for the new one (tww); wait_action1 02283038 / demo_action1 02283228 / partner_search 0227FE48 switch on it and step it (tww); after tww daNpc_Bs1_c mActionStatus |
| 0xA27 | `m857` | `mAnmAtrSetCnt` | anmAtr 02281C14: while the message types, applies the message's animation attribute only when it is 0, then adds 1; case 14 and setStt 022805B8 reset it (tww); same code as daNpc_Hi1_c::mAnmAtrSetCnt |

Later batch:

| Struct / offset | Old | New | Evidence |
|---|---|---|---|
| 0xA0B | `m83B` | `mbSetShapeAngle` | checkOrder 0227FFEC sets it at event index 1; demo 022801BC / event_proc 02280A1C clear it; _execute 02281074 copies current.angle into shape_angle only while it is clear (tww the same); after tww daNpc_Bm1_c mbSetShapeAngle (same code) |

Round 2:

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0xA08 | `m838` | `mEyeFollow` | lookBack 02280C40 sets it in the look modes with a target position and clears it otherwise; play_btk_anm 0227F514 runs eye_ctrl instead of the btk while set; play_animation 0227F5AC turns the eye control on (setEyeCtrl) with btk 0 while set, else clrEyeCtrl; wait_2 02282B54 / wait_3 02282D94 set it (tww the same); same role as the mEyeFollow of the NPC with field_0x7D0 in docs/field-names/npcs.md |
| 0xA10 | `m840` | `mLookHeadOnly` | lookBack 02280C40 passes it to lookAtTarget_2 as the head-only flag (d_npc.h lookAtTarget headOnly; tww bool temp_m840 = it); cut_init_LOK_PLYER 02280720 and wait_2 02282B54 / wait_3 02282D94 / wait_4 02282EB0 set 1 while waiting and 0 when the talk starts (tww the same); same name as daNpc_Hi1_c::mLookHeadOnly |

## daNpc_Kp1_c (d_a_npc_kp1.h)

zeldaret/tww has no code for this class (its d_a_npc_kp1.cpp is a stub), so the names come from our decomp's use. daNpc_Kp1_c runs the same NPC code as daNpc_Hi1_c / daNpc_Bm1_c / tww daNpc_Ls1_c and takes their names.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x86E | `field_0x6F2` | `mBlinkTimer` | playTexPatternAnm 0227B854 counts it down while the blink pattern runs and restarts it with 30 + rnd(60); init_btp 0227B1D0 0; after daNpcPeople_c::mBlinkTimer (npcs.md) |
| 0x8E4 | `field_0x76C` | `mInitialAngle` | _execute 0227C774 copies current.angle on the first run (with mInitialPos, then mbRanExecute = 1); wait01 0227D3C8 faces its y (mDesiredYRot = it.y); same code as daNpc_Bm1_c / daNpc_Hi1_c::mInitialAngle |
| 0x8EC | `field_0x774` | `mInitialPos` | _execute 0227C774 copies current.pos on the first run; written only; after tww daNpc_Bm1_c mInitialPos (same code) |
| 0x910 | `field_0x798` | `mLookPos` | createInit 0227BC14 current.pos; lookBack 0227C41C looks at it; same code as daNpc_Bm1_c::mLookPos (createInit current.pos, lookBack dstPos) |
| 0x91C | `field_0x7A4` | `mPrevMorfFrame` | setMtx 0227B9AC: a morf frame below it means the animation wrapped, then it takes the frame; setAnm_anm 0227CB88 0; after tww daNpc_Ls1_c mPrevMorfFrame |
| 0x928 | `field_0x7B0` | `mLookAtMaxVel` | lookBack 0227C41C eases it to HIO mMaxHeadTurnVel while turning, else 0, and passes it as the look-at's maxVel; after tww daNpc_Bm1_c mHeadLookAtMaxVel / daNpc_Ah_c mLookAtMaxVel |
| 0x92A | `field_0x7B2` | `mDesiredYRot` | lookBack 0227C41C: the look-at's default Y angle (targetY = it); wait01 0227D3C8 sets mInitialAngle.y; same code as daNpc_Bm1_c::mDesiredYRot |
| 0x92C | `field_0x7B4` | `mbMorfAnimStopped` | setMtx 0227B9AC: the morf's play() result, or 1; setAnm_anm 0227CB88 0; ctrlAnmAtr 0227CDE4 tests it; after tww daNpc_Ls1_c mbMorfAnimStopped (same code) |
| 0x92D | `field_0x7B5` | `mAnmLoopCount` | setAnm_anm 0227CB88 0; ctrlAnmAtr 0227CDE4 counts the morf stops and changes the attribute at 2; same code as daNpc_Bm1_c::mAnmLoopCount |
| 0x92E | `field_0x7B6` | `mPreItemNo` | chk_talk 0227D348: dComIfGp_event_getPreItemNo(), else 0xFF; getMsg 0227D104 picks the message by it; createInit 0227BC14 / talk01 0227D480 0xFF; same code as daNpc_Bm1_c::mPreItemNo |
| 0x933 | `field_0x7B8` | `mbRanExecute` | _execute 0227C774: 0 on the first run, when mInitialPos and mInitialAngle are taken, then 1; after tww daNpc_Bm1_c mbRanExecute (same code) |
| 0x938 | `field_0x7C0` | `mbSetEyePos` | wait_action1 0227D558 stores talk01() / wait01(); setAttention 0227B94C tests it; after tww daNpc_Ls1_c mbSetEyePos (same code) |
| 0x93C | `field_0x7C4` | `mHasAttention` | wait_action1 0227D558: chkAttention(); wait01 0227D3C8 looks at the player while it is set; after tww daNpc_Hi1_c mHasAttention (mHasAttention = chkAttention()) |
| 0x93D | `field_0x7C5` | `mTalking` | checkOrder 0227BF00 sets 1 on the talk command; wait01 0227D3C8 starts the talk while it is set; talk01 0227D480 clears it; same code as daNpc_Hi1_c::mTalking |
| 0x93F | `field_0x7C7` | `mInDemo` | demo 0227BFE0: 1 while demoActorID is set, 0 otherwise, and returns it; setMtx 0227B9AC / _execute 0227C774 test it; same code as daNpc_Hi1_c::mInDemo |
| 0x941 | `field_0x7C9` | `mActIdx` | privateCut 0227C32C: dComIfGp_evmng_getMyActIdx(..), -1 ends; after tww daNpc_Hi1_c mActIdx (same code) |
| 0x942 | `field_0x7CA` | `mEventActionNo` | event_actionInit 0227C1E0 stores the cut's action number (*actionNumber); event_action 0227C268 switches on it |
| 0x943 | `field_0x7CB` | `mAnmAtrNo` | chngAnmAtr 0227CD8C stores the attribute; setAnm_ATR 0227CD1C plays a_anm_prm_tbl[it]; ctrlAnmAtr 0227CDE4 / setStt 0227CF10 / endEvent 0227C1A0 use it; same code as daNpc_Hi1_c::mAnmAtrNo |
| 0x944 | `field_0x7CC` | `mMesgAnimeTag` | anmAtr 0227CE54: the message's animation tag when it changes (if (uVar1 != 0xFF && uVar1 != it)); after tww daNpc_Ls1_c mMesgAnimeTag (same code) |
| 0x945 | `field_0x7CD` | `mBtpNum` | setAnm_tex 0227CB64 stores the btp number when it changes; init_btp 0227B1D0 / initTexPatternAnm 0227B2FC / playTexPatternAnm 0227B854 / CreateHeap 0227B340 use it; after tww daNpc_Ls1_c mBtpNum |
| 0x946 | `field_0x7CE` | `mBckNum` | setAnm_anm 0227CB88: skips an entry whose bckNum is < 0 or equal to it, else stores it; after tww daNpc_Ls1_c mBckNum (same code) |
| 0x947 | `field_0x7CF` | `mOrderEvent` | eventOrder 0227C714 orders by it (1 / 2 the talk, 3 / 4 the events); checkOrder 0227BF00 clears it once they start; next_msgStatus 0227CF50 3 / 4; event_proc 0227C600 / wait01 0227D3C8 set it; same code as daNpc_Hi1_c::mOrderEvent |
| 0x948 | `field_0x7D0` | `mStt` | setStt 0227CF10 stores the new state; setAnm 0227CCB0 plays a_anm_prm_tbl[it]; wait_action1 0227D558 switches on it; same code as daNpc_Hi1_c::mStt |
| 0x949 | `field_0x7D1` | `mPrevStt` | setStt 0227CF10 keeps the previous mStt; talk01 0227D480 returns to it (setStt(it)); same code as daNpc_Hi1_c::mPrevStt |
| 0x94A | `field_0x7D2` | `mLookMode` | lookBack 0227C41C switches on it; setStt 0227CF10 / wait01 0227D3C8 set 1 / 3; same code as daNpc_Hi1_c::mLookMode |
| 0x94D | `field_0x7D5` | `mActionStatus` | set_action 0227B728 sets -1 for the leaving action and 0 for the new one; wait_action1 0227D558 switches on it; after tww daNpc_Bs1_c mActionStatus |
| 0x94E | `field_0x7D6` | `mAnmAtrSetCnt` | anmAtr 0227CE54: applies the message's animation attribute only when it is 0, then adds 1; same code as daNpc_Hi1_c::mAnmAtrSetCnt |

Later batch:

| Struct / offset | Old | New | Evidence |
|---|---|---|---|
| HIO 0x08 | `field_0x8` | `mRefCount` | daNpc_Kp1_HIO_c_ct 0227D61C -1; never read in WWHD (tww has a stub only); after our decomp's daNpc_Pm1_HIO_c / daNpc_Ba1_HIO_c mRefCount (the same HIO layout) |
| hio_prm_c 0x10 | `field_10` | `mMaxTurnStep` | _execute 0227C774: m_jnt.setParam's turn step (9th argument); same role as dNpc_HIO_c::mMaxTurnStep |
| hio_prm_c 0x12 | `field_12` | `mMaxHeadTurnVel` | lookBack 0227C41C: cLib_addCalcAngleS2 eases mLookAtMaxVel toward it while turning; same role as dNpc_HIO_c::mMaxHeadTurnVel |
| hio_prm_c 0x18 | `field_18` | `mDebug` | HD byte flag: _draw 0227C930 initialises the debug colour statics while it is set; same role as daNpc_Hi1_HIO_c::hio_prm_c::mDebug |
| 0x934 | `field_0x7BC` | `mbInitEyePos` | createInit 0227BC14 1; setAttention 0227B94C copies mEyePos to eyePos while it or mbSetEyePos is set, then clears it (one eye position right after create); same code as daNpc_Pm1_c::mbInitEyePos |
| hio_prm_c 0x00 | `field_0` | `mMaxHeadX` | _execute 0227C774 passes it to m_jnt.setParam as max_head_x (dNpc_JntCtrl_c::setParam, tww d_npc.h); same names as daNpc_Pm1_HIO_c::hio_prm_c |
| hio_prm_c 0x02 | `field_2` | `mMaxHeadY` | _execute 0227C774 passes it to m_jnt.setParam as max_head_y (dNpc_JntCtrl_c::setParam, tww d_npc.h); same names as daNpc_Pm1_HIO_c::hio_prm_c |
| hio_prm_c 0x04 | `field_4` | `mMinHeadX` | _execute 0227C774 passes it to m_jnt.setParam as min_head_x (dNpc_JntCtrl_c::setParam, tww d_npc.h); same names as daNpc_Pm1_HIO_c::hio_prm_c |
| hio_prm_c 0x06 | `field_6` | `mMinHeadY` | _execute 0227C774 passes it to m_jnt.setParam as min_head_y (dNpc_JntCtrl_c::setParam, tww d_npc.h); same names as daNpc_Pm1_HIO_c::hio_prm_c |
| hio_prm_c 0x08 | `field_8` | `mMaxBackboneX` | _execute 0227C774 passes it to m_jnt.setParam as max_backbone_x (dNpc_JntCtrl_c::setParam, tww d_npc.h); same names as daNpc_Pm1_HIO_c::hio_prm_c |
| hio_prm_c 0x0A | `field_A` | `mMaxBackboneY` | _execute 0227C774 passes it to m_jnt.setParam as max_backbone_y (dNpc_JntCtrl_c::setParam, tww d_npc.h); same names as daNpc_Pm1_HIO_c::hio_prm_c |
| hio_prm_c 0x0C | `field_C` | `mMinBackboneX` | _execute 0227C774 passes it to m_jnt.setParam as min_backbone_x (dNpc_JntCtrl_c::setParam, tww d_npc.h); same names as daNpc_Pm1_HIO_c::hio_prm_c |
| hio_prm_c 0x0E | `field_E` | `mMinBackboneY` | _execute 0227C774 passes it to m_jnt.setParam as min_backbone_y (dNpc_JntCtrl_c::setParam, tww d_npc.h); same names as daNpc_Pm1_HIO_c::hio_prm_c |

Open (evidence not enough yet): `field_0x6F1`, `field_0x7AC`, `field_0x7D7` (no use), `field_0x7C8` (talk01 0227D480 clears it, never read), `field_0x7D3`, `field_0x7D4` (written only, in decideType 0227B700).

## daNpc_Km1_c (d_a_npc_km1.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_npc_km1.cpp, which keeps the GameCube `field_0x` names and uses each of these fields the same way) and our decomp's use. daNpc_Km1_c has daNpc_Kp1_c's layout and code and takes its names (round 3 adds `field_0x7BC` -> `mbInitEyePos`: createInit 02271E78 1, setAttention 02271CBC). Open (evidence not enough yet): `field_0x6F1`, `field_0x7AC`, `field_0x7B6`, `field_0x7D7` (no use), `field_0x7B5`, `field_0x7C8` (cleared only), `field_0x7D3`, `field_0x7D4` (written only, in decideType).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x862 | `field_0x6F2` | `mBlinkTimer` | playTexPatternAnm 02271BD0 counts it down while the blink pattern runs and restarts it with 30 + rnd(60); init_btp 02271678 0 (tww km1 the same); after daNpcPeople_c::mBlinkTimer (npcs.md) |
| 0x8D8 | `field_0x76C` | `mInitialAngle` | _execute 022726A0 copies current.angle on the first run (with mInitialPos, then mbRanExecute = 1); wait01 02272DAC faces its y (mDesiredYRot = it.y) (tww km1 the same); same code as daNpc_Bm1_c / daNpc_Hi1_c / daNpc_Kp1_c::mInitialAngle |
| 0x8E0 | `field_0x774` | `mInitialPos` | _execute 022726A0 copies current.pos on the first run; written only (tww km1 the same); after tww daNpc_Bm1_c mInitialPos (same code) |
| 0x904 | `field_0x798` | `mLookPos` | createInit 02271E78 current.pos; lookBack 02272424 looks at it (tww km1 the same); same code as daNpc_Bm1_c::mLookPos (createInit current.pos, lookBack dstPos) |
| 0x910 | `field_0x7A4` | `mPrevMorfFrame` | setMtx 02271D1C: a morf frame below it means the animation wrapped, then it takes the frame; setAnm_anm 02272A34 0 (tww km1 the same); after tww daNpc_Ls1_c mPrevMorfFrame |
| 0x91C | `field_0x7B0` | `mLookAtMaxVel` | lookBack 02272424 eases it to HIO mMaxHeadTurnVel while turning, else 0, and passes it as the look-at's maxVel (tww km1 the same); after tww daNpc_Bm1_c mHeadLookAtMaxVel / daNpc_Ah_c mLookAtMaxVel |
| 0x91E | `field_0x7B2` | `mDesiredYRot` | lookBack 02272424: the look-at's default Y angle (targetY = it); wait01 02272DAC sets mInitialAngle.y (tww km1 the same); same code as daNpc_Bm1_c::mDesiredYRot |
| 0x920 | `field_0x7B4` | `mbMorfAnimStopped` | setMtx 02271D1C: the morf's play() result, or 1; setAnm_anm 02272A34 0 (tww km1 the same); after tww daNpc_Ls1_c mbMorfAnimStopped (same code) |
| 0x924 | `field_0x7B8` | `mbRanExecute` | _execute 022726A0: 0 on the first run, when mInitialPos and mInitialAngle are taken, then 1 (tww km1 the same); after tww daNpc_Bm1_c mbRanExecute (same code) |
| 0x92C | `field_0x7C0` | `mbSetEyePos` | wait_action1 02272EEC stores talk01() / wait01(); setAttention 02271CBC tests it (tww km1 the same); after tww daNpc_Ls1_c mbSetEyePos (same code) |
| 0x930 | `field_0x7C4` | `mHasAttention` | wait_action1 02272EEC: chkAttention(); wait01 02272DAC looks at the player while it is set (tww km1 the same); after tww daNpc_Hi1_c mHasAttention (mHasAttention = chkAttention()) |
| 0x931 | `field_0x7C5` | `mTalking` | checkOrder 0227218C sets 1 on the talk command; wait01 02272DAC starts the talk while it is set; talk01 02272E6C clears it (tww km1 the same); same code as daNpc_Hi1_c / daNpc_Kp1_c::mTalking |
| 0x933 | `field_0x7C7` | `mInDemo` | demo 022721CC: 1 while demoActorID is set, 0 otherwise, and returns it; setMtx 02271D1C / _execute 022726A0 test it (tww km1 the same); same code as daNpc_Hi1_c / daNpc_Kp1_c::mInDemo |
| 0x935 | `field_0x7C9` | `mActIdx` | privateCut 02272334: dComIfGp_evmng_getMyActIdx(..), -1 ends (tww km1 the same); after tww daNpc_Hi1_c mActIdx (same code) |
| 0x936 | `field_0x7CA` | `mEventActionNo` | event_actionInit 022722A4 stores the cut's action number (tww field_0x7CA = *puVar1); event_action (inlined in WWHD) switches on it (tww switch (field_0x7CA)); same code as daNpc_Kp1_c::mEventActionNo |
| 0x937 | `field_0x7CB` | `mAnmAtrNo` | chngAnmAtr 02272BC0 stores the attribute; setAnm_ATR 02272B5C plays a_anm_prm_tbl[it]; setStt 02272CA4 / endEvent 02272D6C use it (tww km1 the same); same code as daNpc_Hi1_c / daNpc_Kp1_c::mAnmAtrNo |
| 0x938 | `field_0x7CC` | `mMesgAnimeTag` | anmAtr 02272BE4: the message's animation tag when it changes (if (uVar1 != 0xFF && uVar1 != it)) (tww km1 the same); after tww daNpc_Ls1_c mMesgAnimeTag (same code) |
| 0x939 | `field_0x7CD` | `mBtpNum` | setAnm_tex 02272A10 stores the btp number when it changes; initTexPatternAnm 0227178C / playTexPatternAnm 02271BD0 / CreateHeap 022717D0 use it (tww km1 the same); after tww daNpc_Ls1_c mBtpNum |
| 0x93A | `field_0x7CE` | `mBckNum` | setAnm_anm 02272A34: skips an entry whose bckNum is < 0 or equal to it, else stores it (tww km1 the same); after tww daNpc_Ls1_c mBckNum (same code) |
| 0x93B | `field_0x7CF` | `mOrderEvent` | eventOrder 02272668 orders by it (1 / 2 the talk) (tww); checkOrder 0227218C clears it once the talk starts; wait01 02272DAC sets it (tww); same code as daNpc_Hi1_c / daNpc_Kp1_c::mOrderEvent |
| 0x93C | `field_0x7D0` | `mStt` | setStt 02272CA4 stores the new state; setAnm 02272AF0 plays a_anm_prm_tbl[it]; wait_action1 02272EEC switches on it (tww km1 the same); same code as daNpc_Hi1_c / daNpc_Kp1_c::mStt |
| 0x93D | `field_0x7D1` | `mPrevStt` | setStt 02272CA4 keeps the previous mStt; talk01 02272E6C returns to it (setStt(it)) (tww km1 the same); same code as daNpc_Hi1_c / daNpc_Kp1_c::mPrevStt |
| 0x93E | `field_0x7D2` | `mLookMode` | lookBack 02272424 switches on it; setStt 02272CA4 / wait01 02272DAC set 1 / 3 (tww km1 the same); same code as daNpc_Hi1_c / daNpc_Kp1_c::mLookMode |
| 0x941 | `field_0x7D5` | `mActionStatus` | set_action 02271AA4 sets -1 for the leaving action and 0 for the new one; wait_action1 02272EEC switches on it (tww km1 the same); after tww daNpc_Bs1_c mActionStatus |
| 0x942 | `field_0x7D6` | `mAnmAtrSetCnt` | anmAtr 02272BE4: applies the message's animation attribute only when it is 0, then adds 1 (tww km1 the same); same code as daNpc_Hi1_c / daNpc_Kp1_c::mAnmAtrSetCnt |

Later batch:

| Struct / offset | Old | New | Evidence |
|---|---|---|---|
| 0x923 | `field_0x7B7` | `mItemNo` | wait01 02272DAC: 0xFF, or dComIfGp_event_getPreItemNo() on a talk with an item; talk01 02272E6C 0xFF (tww the same); same code as daNpc_Pm1_c::mItemNo |
| HIO 0x04 | `field_0x8` | `mRefCount` | daNpc_Km1_HIO_c_ct 02272FB0 -1; _create 02271F84 creates the HIO child at < 0 and adds 1; _delete 02272110 subtracts 1 and deletes the child below 0 (tww the same); same code as daNpc_Pm1_HIO_c::mRefCount |
| hio_prm_c 0x10 | `field_10` | `mMaxTurnStep` | _execute 022726A0: m_jnt.setParam's turn step (9th argument) (tww the same); same role as dNpc_HIO_c::mMaxTurnStep |
| hio_prm_c 0x12 | `field_12` | `mMaxHeadTurnVel` | lookBack 02272424: cLib_addCalcAngleS2 eases mLookAtMaxVel toward it while turning (tww the same); same role as dNpc_HIO_c::mMaxHeadTurnVel |
| hio_prm_c 0x18 | `field_18` | `mDebug` | HD byte flag: _draw 02272858 initialises the debug colour statics while it is set (tww an unused f32); same role as daNpc_Hi1_HIO_c::hio_prm_c::mDebug |

## daNpc_Tc_c and daNpc_Tc_HIO_c (d_a_npc_tc.h; view daNpc_Tc_HIO_cut in d_a_npc_tc_cut.h)

Old names are the GameCube offsets (zeldaret/tww d217e2b, src/d/actor/d_a_npc_tc.cpp, which decompiles the class but keeps the `field_0x` names). The message flags are used only by the WWHD message code (d_a_npc_tc_msg.cpp; tww has no code for them except field_0x812 / 0x813 and setFirstMsg), so their names say which message they gate. daNpc_Tc_HIO_cut is a view of the same object (l_cutHIO and l_HIO are both 0x104687F4) and takes the HIO's names. Open: `field_0x798` (_execute 022EA140 stores the morf play result, never read), `field_0x80C` (next_msgStatusNormal2 022EB35C writes 1 twice, never read); the unused gaps `field_0x6C4`, `field_0x7D0`, `field_0x7EA`, `field_0x7F4`, `field_0x7FD`, `field_0x80A`, `field_0x815`; HIO `field_0x34` (read only by _draw 022EA464, where it sets up debug colours; tww calls mEventCut.getAttnPos() there; role not clear enough), `field_0x7C`..`field_0x93` (tww's constructor mirrors event bits, collect maps and stage boxes there for the debug view; WWHD never writes or reads them).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x7E0 | `field_0x6C8` | `mAtrSet` | anmAtr 022EB970: 0 takes the message's animation attribute once (MSG_TYPING) and steps to 1, MSG_DISPLAYED resets it (tww the same); same role as our decomp's daNpc_Pf1_c mAtrSet |
| 0x82C | `field_0x714` | `mBlurRCallBack` | setAnm 022E7E70: follow callback of the ID_IT_SN_TG_KURURINP_BLUR_R00 emitter at mParticlePos, ended at frame 178; _create 022E8CAC constructs it, _delete 022E8DD4 removes it (tww the same) |
| 0x840 | `field_0x728` | `mBlurGCallBack` | setAnm 022E7E70: follow callback of the ID_IT_SN_TG_KURURINP_BLUR_G00 emitter at mParticlePos, ended at frame 178; _create 022E8CAC constructs it, _delete 022E8DD4 removes it (tww the same) |
| 0x904 | `field_0x790` | `mHomeAngle` | createInit 022E87DC: current.angle at creation; statusSitToStool 022EC984 turns current.angle.y back to its y and goes on once within 16 (tww the same) |
| 0x97C | `field_0x808` | `mGreetingDone` | getMsgNormal2 022EB24C: 0 gives the greeting 0xDB2 / 0xDB3 / 0xDFB (event bits 0x1A20 / 0x1708) once and sets 1 |
| 0x97D | `field_0x809` | `mMapSaleIntroDone` | getMsgNormal2 022EB24C: with an unknown collect map left, 0 gives 0xDBB once (next_msgStatusNormal2 022EB35C: 0xDBB-0xDBD lead to the 0xDBE pay choice) and sets 1; after that 0xDBE directly |
| 0x97F | `field_0x80B` | `mShortOfRupeesOnReoffer` | next_msgStatusNormal2 022EB35C: set when the 0xDC3 offer is accepted with fewer than 398 rupees (-> 0xDC0); 0xDC0 then goes to 0xDC4 instead of 0xDC1; 0xDC4 clears it |
| 0x981 | `field_0x80D` | `mMapBought` | next_msgStatusNormal2 022EB35C: set when 0xDBE / 0xDC3 is accepted with 398+ rupees (STATUS_PAY_RUPEE), cleared by 0xDC4; getMsgNormal2 022EB24C: while set, 0xDC3 (and clear) if an unknown collect map is left, else 0xDC4 |
| 0x982 | `field_0x80E` | `mWhiteIntroAfterB80Done` | next_msgStatusWhite 022EAB1C: with event bit 0xB80, 0xDC8 goes on to 0xDD7 once and sets it; getMsgWhite 022EB7E8: with 0xB80, 0 starts at 0xDC8, 1 at 0xDD6 |
| 0x983 | `field_0x80F` | `mWhiteIntroDone` | next_msgStatusWhite 022EAB1C: without event bit 0xB80, 0xDC8 goes on to 0xDC9 once and sets it; getMsgWhite 022EB7E8: without 0xB80, 0 starts at 0xDC8, 1 at 0xDC5 |
| 0x984 | `field_0x810` | `mRedIntroAfterB80Done` | getMsgRed 022EB5F4: with event bit 0xB80, 0 gives 0xDEE (one of the five stages has treasure box 0xF open while its event bit is clear) or 0xDE6 once and sets 1; after that 0xDEB / 0xDEA |
| 0x985 | `field_0x811` | `mRedFirstMsgDone` | getMsgRed 022EB5F4: the setFirstMsg 022EB5D0 flag for 0xDB7 / 0xDE1 (first time, sets it) or 0xDE5 (after) (tww setFirstMsg(bool*, first, then)) |
| 0x986 | `field_0x812` | `mGetRupeeDemoDone` | statusDemoGetRupee 022ECD58 sets it when the TC_GET_RUPEE event ends (tww the same); getMsgRed 022EB5F4 clears it and answers 0xDF1 (all monument fees paid) or 0xDF0 |
| 0x987 | `field_0x813` | `mMonumentCompleteDemoDone` | statusDemoMonumentComplete 022ECEDC sets it when the TC_GET_RUPEE event ends (tww the same); getMsgRed 022EB5F4 clears it and answers 0xDF3 |
| 0x988 | `field_0x814` | `mMsgDE6Shown` | next_msgStatusRed 022EA7CC: 0xDE6 sets it; 0xDEB goes on to 0xDEC only while it is set (and event bit 0x1A20), else finishes; 0xDC7 clears it |
| HIO 0x2C | `field_0x2C` | `mSitJumpDist` | statusSit 022EC174: the jump down starts (STATUS_DEMO_JUMP the first time, else ANM_PRM_IDX_JAMP_A) once the player is within it; chkAttention 022EA648: the attention distance in STATUS_WALK_TO_JAIL / JUMP / SIT (tww the same); the constructor 022ED4A4 sets 900 |
| HIO 0x30 | `field_0x30` | `mWalkToJailDist` | statusWalkToStool 022EC7E8: STATUS_WALK_TO_JAIL while the player is within it; statusWalkToJail 022EC384: back to STATUS_WALK_TO_STOOL on a wall hit once the player is outside it (tww the same); the constructor 022ED4A4 sets 400 |
| HIO 0x38 | `field_0x38` | `mJumpSpeedF` | statusSit 022EC174: speedF of the jump (ANM_PRM_IDX_JAMP_B), with mJumpSpeedY / mJumpGravity (tww the same); cutSitToJumpProc 022E93D4 the same through the view daNpc_Tc_HIO_cut; the constructor 022ED4A4 sets 20 |
| HIO 0x3C | `field_0x3C` | `mJumpGravity` | statusSit 022EC174: gravity of the jump (ANM_PRM_IDX_JAMP_B) (tww the same); cutSitToJumpProc 022E93D4 the same through the view daNpc_Tc_HIO_cut; the constructor 022ED4A4 sets -2.5 |
| HIO 0x40 | `field_0x40` | `mJumpSpeedY` | statusSit 022EC174: speed.y of the jump (ANM_PRM_IDX_JAMP_B) (tww the same); cutSitToJumpProc 022E93D4 the same through the view daNpc_Tc_HIO_cut; the constructor 022ED4A4 sets 14 |
| HIO 0x44 | `field_0x44` | `mAlwaysTalkNearJail` | statusWalkToJail 022EC384 / statusWaitNearJail 022EC62C: nonzero starts the near-jail talk (STATUS_DEMO_TALK) on a wall hit even after mHasTalkedNearJail (tww the same); never written in WWHD or tww (HIO switch) |
| HIO 0x48 | `field_0x48` | `mBackJumpSpeedF` | cutBackJumpStart 022E919C: speedF of the back jump, through the view daNpc_Tc_HIO_cut, with mBackJumpSpeedY / mBackJumpGravity; the constructor 022ED4A4 sets -25 (tww: the constructor only) |
| HIO 0x4C | `field_0x4C` | `mBackJumpGravity` | cutBackJumpStart 022E919C: gravity of the back jump, through the view daNpc_Tc_HIO_cut; the constructor 022ED4A4 sets -2.5 (tww: the constructor only) |
| HIO 0x50 | `field_0x50` | `mBackJumpSpeedY` | cutBackJumpStart 022E919C: speed.y of the back jump, through the view daNpc_Tc_HIO_cut; the constructor 022ED4A4 sets 8 (tww: the constructor only) |
| HIO 0x54 | `field_0x54` | `mWalkAnmRate` | calcMove 022EBB90: the walk animation speed is speedF (in cuts 2 / 4 the cut speed) times it, at least 0.5 (tww the same); the constructor 022ED4A4 sets 0.35 |
| HIO 0x58 | `field_0x58` | `mWalkSpeed` | statusWalkToJail 022EC384 / statusWaitNearJail 022EC62C / statusWalkToStool 022EC7E8: mTargetSpeed while walking (ANM_PRM_IDX_WALK01) (tww the same); the constructor 022ED4A4 sets 4 |
| HIO 0x5C | `field_0x5C` | `mWalkStartSpeed` | statusWalkToJail 022EC384 / statusWaitNearJail 022EC62C: speedF when the walk starts from standing (tww the same); the constructor 022ED4A4 sets 8 |
| HIO 0x60 | `field_0x60` | `mWhiteTowerPos` | setTower 022E8E64: for TYPE_WHITE, current.pos = the tower's matrix times it (tww the same); the constructor 022ED4A4 sets (-100, 1700, -50) |
| HIO 0x6C | `field_0x6C` | `mRedTowerPos` | setTower 022E8E64: for TYPE_RED, current.pos = the tower's matrix times it (tww the same); the constructor 022ED4A4 sets (100, 1700, 50) |
| HIO 0x78 | `field_0x78` | `mRedTowerAngleY` | setTower 022E8E64 / startTower 022EBFB8: for TYPE_RED, the facing is the tower's shape_angle.y plus it (tww the same); the constructor 022ED4A4 sets -0x8000 |
| HIO 0x7A | `field_0x7A` | `mWhiteTowerAngleY` | setTower 022E8E64 / startTower 022EBFB8: for TYPE_WHITE, the facing is the tower's shape_angle.y plus it (tww the same); the constructor 022ED4A4 sets 0 |
| HIO 0x94 | `field_0x94` | `mAnmMorf` | setAnm 022E7E70 copies it[i] into a_anm_prm_tbl[i].mMorf for the 24 animation entries (tww the same); the constructor 022ED4A4 sets the 24 morf frame counts |
## daNpc_Ko1_c (d_a_npc_ko1.h; copies in d_a_npc_ko1_c.h, _d1.h, _d2.h and the view Ko1MovementFields in d_a_npc_ko1_b.h)

The class header had WWHD offset names (`m7E4`); zeldaret/tww has no code for this class (nonmatching stubs, no members). The part headers d_a_npc_ko1_c.h, _d1.h, _d2.h (copies of daNpc_Ko1_c) and the view Ko1MovementFields (d_a_npc_ko1_b.h) already named most fields, partly differently. One name per field (as for mt_class in enemies.md): where the copies agree, the class takes their name; where they disagree, the name with the strongest evidence wins: the original assert texts for the joint indices (`m_hed_jnt_num` ..), otherwise daNpc_Bm1_c's name when Ko1 runs the same code (lookBack, chg_anmAtr / anmAtr, setStt, setAttention, set_action follow tww's daNpc_Bm1_c line for line), otherwise the name the most uses support. The losing copy names are corrected in their own commit (`was X (misnamed)`). The copies' partner aggregates (`mPartnerIds[3]` in _c.h, `mPartners` in _d2.h, both over 0x924..0x92F) keep their shape; the class names the elements `mPartnerId`, `mOtherPartnerId`, `mPartnerCount`. Open: `m970`, `m9A8`, `m9BC`, `m9D2`, `m9D6`, `mA10` (no use).

daNpc_Ko1_childHIO_c (the HIO view in d_a_npc_ko1_d1.h): `mWalkTurnRate`, `mRunTurnRate`, `mSwimTurnRate` (0x30, 0x40, 0x44) are only copied into `mPassDist` by setStt 02278134, which ko_movPass 022761AC compares with the XZ distance to the point; corrected to `mWalkPassDist`, `mRunPassDist`, `mSwimPassDist` in their own commit (`was X (misnamed)`).

Ko1MovementParameters (the view of the same HIO table at 0x104677E8 in d_a_npc_ko1_b.h) called 0x28 / 0x38 `mWalkMaxSpeed` / `mRunMaxSpeed`, daNpc_Ko1_childHIO_c `mWalkSpeed` / `mRunSpeed`. By the uses they are the walk / run speeds: setStt 02278134 copies them (with `mWalkAcceleration` / `mRunAcceleration` at 0x2C / 0x3C) into `mMoveSpeed`, which ko_clcMovSpd 02276028 chases speedF to; ko_nMove 022763A8 only caps the play rate at speed * animation rate. The view takes `mWalkSpeed` / `mRunSpeed` in its own commit (`was X (misnamed)`).

Corrected copy names (own commit, each declaration says `was X (misnamed)` with the reason): _c.h: `mBalloonJoint` -> `m_bln_jnt_num`, `mAnimationFrame` -> `mPrevMorfFrame`, `mHeadAngleY` -> `mJointHeadY`, `mBodyAngleY` -> `mJointBackboneY`, `mbMoving` -> `mHeadOnlyFollow`, `mAnimationSelector` -> `mAnmAtrNo`, `mAnimationTag` -> `mMesgAnimeTag`, `mAnimationNum` -> `mAnmNum`, `mLookTarget` -> `mLookBackState`; _d1.h: `mTurnRate` -> `mPassDist`, `mWaitTimer` -> `mSttTimer`, `mTurnAngle` -> `mDesiredYRot`, `mbJumping` -> `mPreviousGroundHit`, `mTalkFlag` -> `mTalking`, `mbMoving` -> `mHeadOnlyFollow`, `mAnimationSelector` -> `mAnmAtrNo`, `mHeadAnimationSelector` -> `mMesgAnimeTag`, `mAnimationNum` -> `mAnmNum`, `mLookMode` -> `mOrderEvent`, `mLookTarget` -> `mLookBackState`; _d2.h: `mSleepTimer` -> `mSttTimer`, `mTurnAngle` -> `mDesiredYRot`, `mStatusResult` -> `mbSetEyePos`, `mTalkFlag` -> `mTalking`, `mAnimationSelector` -> `mAnmAtrNo`, `mAnimationMode` -> `mLookBackState`, `mActionPhase` -> `mActionStatus`; _b.h (Ko1MovementFields): `mBodyJoint` -> `m_hed_jnt_num`, `mItemJoint` -> `m_armR2_jnt_num`, `mLookTarget` -> `mLookPos`, `mMoveTarget` -> `mTargetPos`, `mSpawnPos` -> `mAreaCenter`, `mPreviousFrame` -> `mPrevMorfFrame`, `mTargetSpeed` -> `mMoveSpeed`, `mSwimSpeed` -> `mVerticalSpeed`, `mAcceleration` -> `mMoveAcceleration`, `mArrivalRadius` -> `mPassDist`, `mPreviousHeadY` -> `mJointHeadY`, `mPreviousBackboneY` -> `mJointBackboneY`, `mPreviousBodyY` -> `mActorAngleY`, `mLookAngle` -> `mDesiredYRot`, `mBalloonState` -> `mSleepState`, `mPointReached` -> `mbMoveFinished`, `mTalkRequested` -> `mTalking`, `mHeadOnly` -> `mHeadOnlyFollow`, `mAnimationAttribute` -> `mAnmAtrNo`, `mAnimationTag` -> `mMesgAnimeTag`, `mAnimationNo` -> `mAnmNum`, `mEventState` -> `mOrderEvent`, `mActionState` -> `mStatus`, `mLookMode` -> `mLookBackState`, `mActionStep` -> `mActionStatus`.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x7E4 | `m7E4` | `m_hed_jnt_num` | create_Anm 022736FC: the "head" joint's index, asserted with the original text "m_hed_jnt_num >= 0" (line 4317); nodeKo1Control 0227351C turns it by the joint control's head angles and takes the eye position there; setMtx 02274A7C puts the head model on it; tww Bm1 / Ls1 m_hed_jnt_num (same code) |
| 0x7E5 | `m7E5` | `m_bbone_jnt_num` | create_Anm 022736FC: the "backbone" joint's index, asserted with the original text "m_bbone_jnt_num >= 0"; nodeKo1Control 0227351C turns it by the joint control's backbone angles; tww Bm1 / Ls1 m_bbone_jnt_num (same code) |
| 0x7E6 | `m7E6` | `m_armR2_jnt_num` | create_Anm 022736FC: the "armR2" joint's index, asserted with the original text "m_armR2_jnt_num >= 0"; setMtx 02274A7C puts the item model (mpItemModel) on it |
| 0x7E7 | `m7E7` | `m_hed_2_jnt_num` | create_hed_Anm 02273944: the head model's "head2" joint index, asserted with the original text "m_hed_2_jnt_num >= 0"; nodeHedControl 022732F0 keeps that joint's matrix in mHed2Mtx |
| 0x7E8 | `m7E8` | `m_bln_loc_jnt_num` | create_bln_Anm 02273C94: the balloon model's "balloon_loc" joint index, asserted with the original text "m_bln_loc_jnt_num >= 0"; nodeBlnControl 022733C4 gives that joint mHed2Mtx |
| 0x7E9 | `m7E9` | `m_bln_jnt_num` | create_bln_Anm 02273C94: the balloon model's "ko_balloon" joint index, asserted with the original text "m_bln_jnt_num >= 0" |
| 0x7EC | `m7EC` | `mHed2Mtx` | nodeHedControl 022732F0 copies the head model's head2 joint matrix (m_hed_2_jnt_num) here; nodeBlnControl 022733C4 makes it the balloon_loc joint's matrix (the balloon follows the head) |
| 0x81C | `mpMorf81C` | `mpBalloonMorf` | create_bln_Anm 02273C94 creates the balloon morf from the balloon model; set_balloonAnm_anm 02276C70 sets its animation, setMtx 02274A7C syncs its frame and calcs it; same name as the views in d_a_npc_ko1_c.h / _b.h |
| 0x820 | `mpModel820` | `mpItemModel` | create_itm_Mdl 02273E7C creates the hand item's model; setMtx 02274A7C puts it on the armR2 joint and calcs it; same name as the view in d_a_npc_ko1_b.h |
| 0x824 | `mpMorf824` | `mpHeadMorf` | create_hed_Anm 02273944 creates the head morf; setAnm_anm 02276B9C sets its animation (headAnmNum_toResID), setMtx 02274A7C puts it on the head joint; same name as the views in d_a_npc_ko1_c.h / _b.h |
| 0x8B4 | `m8B4` | `mSavedPath` | setStt 02278134 keeps the current path here before the attack / swim states and restores it (setInfDrct) when walk_3 starts; same name as the view in d_a_npc_ko1_d1.h |
| 0x924 | `m924` | `mPartnerId` | partner_srch 022753EC: the first partner's process id (partner_srch_sub); setStt 02278134 / wait_7 02278EC0 / attk_3 02279A58 search it; same name as the views in d_a_npc_ko1_d1.h / _b.h |
| 0x928 | `m928` | `mOtherPartnerId` | partner_srch 022753EC: the second partner's process id (searchActor_Ob) for the types with two partners; chk_manzai_1 02277124 / talk_2 02279EF8 read it as element 1 of the partner list; same name as the view in d_a_npc_ko1_b.h |
| 0x92C | `m92C` | `mPartnerCount` | partner_srch 022753EC sets 1 or 2 with the partner ids; talk_2 02279EF8 loops over that many partners; same name as the view in d_a_npc_ko1_b.h |
| 0x930 | `m930` | `mAttackActorId` | attk_1 022797FC / attk_2 0227992C / attk_3 02279A58 store the actor id they act on; same name as the view in d_a_npc_ko1_d1.h |
| 0x946 | `m946` | `mModelAngle` | createInit 02274E28 copies current.angle; setMtx 02274A7C turns the model by its y; same name as the view in d_a_npc_ko1_b.h |
| 0x94C | `m94C` | `mEyePos` | nodeKo1Control 0227351C: the eye position from the head joint's matrix (a_eye_pos_off); setAttention 02274A1C copies it to eyePos while mbSetEyePos or the argument is set; same code as daNpc_Bm1_c::mEyePos (tww Bm1 eyePos.set(mEyePos.x, ..)) |
| 0x958 | `m958` | `mLookPos` | setStt 02278134 / wait_7 02278EC0 set the partner's position at its eye height; createInit 02274E28 current.pos; lookBack 022758B8 looks at it in look state 2; same code as daNpc_Bm1_c::mLookPos (tww Bm1 dstPos = m82C); same name as the view in d_a_npc_ko1_d1.h |
| 0x964 | `m964` | `mTargetPos` | setStt 02278134 sets the path point or the player's position; ko_movPass 022761AC / ko_clcMovSpd 02276028 / ko_clcSwmSpd 022762AC move towards it; same name as daNpc_Bm1_c::mTargetPos and the view in d_a_npc_ko1_d1.h |
| 0x97C | `m97C` | `mAreaCenter` | createInit 02274E28 current.pos; wait_4 02278B84 / wait_5 02278CB4 / wait_6 02278D7C / wait_9 022790E4 / walk_1 02279300 check the player against an area around it (chk_areaIn); _execute 022765B0 deletes the actor once it is more than 3000 away (XZ); same name as the view in d_a_npc_ko1_d1.h |
| 0x988 | `m988` | `mGroundNormal` | _execute 022765B0 stores the ground polygon's plane normal; chk_routeAngle 02275D90 turns the route along it; same name as the view in d_a_npc_ko1_b.h |
| 0x994 | `m994` | `mPrevMorfFrame` | setMtx 02274A7C: a morf frame below it means the animation wrapped (mAnimationEnded = 1), then it takes the frame; setAnm_anm 02276B9C 0; same code as daNpc_Km1_c / daNpc_Ls1_c::mPrevMorfFrame (after tww Ls1) |
| 0x998 | `m998` | `mBalloonAnimationFrame` | set_balloonAnm_anm 02276C70 0 when the balloon animation changes; written only; same name as the view in d_a_npc_ko1_c.h |
| 0x99C | `m99C` | `mMoveSpeed` | setStt 02278134 sets the HIO walk / run speed (0 for the waits), clrSpd 0227810C 0; ko_clcMovSpd 02276028 chases speedF to it; same name as the views in d_a_npc_ko1_c.h / _d1.h |
| 0x9A0 | `m9A0` | `mVerticalSpeed` | setStt 02278134 / swim_1 02279578 / swim_2 022796B4 set -4 or 4; ko_clcSwmSpd 022762AC chases speed.y to it; same name as the view in d_a_npc_ko1_d1.h |
| 0x9A4 | `m9A4` | `mMoveAcceleration` | setStt 02278134 sets the HIO walk / run acceleration, clrSpd 0227810C 0; ko_clcMovSpd 02276028 / ko_nMove 022763A8 chase speedF with it as the step; same name as the views in d_a_npc_ko1_c.h / _d1.h |
| 0x9AC | `m9AC` | `mPassDist` | ko_movPass 022761AC: the point counts as reached once the XZ distance is <= it; setStt 02278134 sets the HIO walk / run / swim value; same code as daNpc_Bm1_c::mPassDist (tww Bm1 bm_movPass if (dist_xz <= m858)) |
| 0x9B0 | `m9B0` | `mJointHeadY` | lookBack 022758B8 stores the joint control's head Y angle; chk_partsNotMove 0227732C compares it; same code as daNpc_Bm1_c::mJointHeadY (after tww Ls1) |
| 0x9B2 | `m9B2` | `mJointBackboneY` | lookBack 022758B8 stores the joint control's backbone Y angle; chk_partsNotMove 0227732C compares it; same code as daNpc_Bm1_c::mJointBackboneY (after tww Ls1) |
| 0x9B4 | `m9B4` | `mActorAngleY` | lookBack 022758B8 stores current.angle.y; chk_partsNotMove 0227732C compares it; same code as daNpc_Bm1_c::mActorAngleY (after tww Ls1); same name as the view in d_a_npc_ko1_c.h |
| 0x9B8 | `m9B8` | `mPartnerMsgNo` | manzai 02279FC0 copies the partner's current message number, talk_1 02279BC0 0xFFFFFFFF; chg_anmAtr 02276EB8 turns to the player on some of them; same name as the views in d_a_npc_ko1_c.h / _d2.h |
| 0x9BE | `m9BE` | `mTalkWaitTimer` | wait_1 0227896C / wait_a 022791F4 / talk_1 02279BC0 set 60 and count it down; setStt 02278134 0; same name as the views in d_a_npc_ko1_d1.h / _d2.h |
| 0x9C0 | `m9C0` | `mZigzagTimer` | ko_clcMovSpd 02276028: in the chase states the run direction flips when it runs out, then it restarts with 8 + rnd(20); same name as the view in d_a_npc_ko1_b.h |
| 0x9C2 | `m9C2` | `mSttTimer` | setStt 02278134 sets it per state (90 + rnd(90) for wait_1, 180 + rnd(180) for walk_1); wait_1 0227896C / walk_1 02279300 / neru_1 0227A110 / neru_2 0227A1C8 change the state or the sleep step when it runs out; same code as daNpc_Bm1_c::mSttTimer (tww Bm1 setStt m870 = ..) |
| 0x9C4 | `m9C4` | `mBalloonTimer` | setStt 02278134 / neru_2 0227A1C8 set it (counter & 3 + 1, 3 + rnd(10)); wait_1 0227896C / wait_2 02278AE0 / neru_2 count it down; same name as the views in d_a_npc_ko1_d1.h / _d2.h |
| 0x9C6 | `m9C6` | `mTurnVelocity` | lookBack 022758B8 eases it towards the HIO maximum and passes it as the look-at's turn velocity (0 while m_jnt.mbTrn is clear); same name as the view in d_a_npc_ko1_b.h |
| 0x9C8 | `m9C8` | `mDesiredYRot` | lookBack 022758B8: the look-at's Y angle in look state 3; setStt 02278134 / wait_1 0227896C / wait_a 022791F4 / manzai 02279FC0 set mInitialAngle.y; same code as daNpc_Bm1_c::mDesiredYRot (tww Bm1 desiredYrot = m876) |
| 0x9CA | `m9CA` | `mTargetOffsetAngle` | set_tgtPos 02277C44 offsets the target around the player by it and steps it by 0x400; same name as the view in d_a_npc_ko1_c.h |
| 0x9CE | `m9CE` | `mBalloonAnimationState` | set_balloonAnm_anm 02276C70 0 when the balloon animation changes; written only; same name as the view in d_a_npc_ko1_c.h |
| 0x9CF | `m9CF` | `mbBalloonAnimationStopped` | set_balloonAnm_anm 02276C70 0 when the balloon animation changes; written only; same name as the view in d_a_npc_ko1_c.h |
| 0x9D0 | `m9D0` | `mAnimationEnded` | setMtx 02274A7C: the morf's play() result, also set when the frame wraps; setAnm_anm 02276B9C 0; control_anmTag 02276E90 / control_anmAtr 02276F30 and the states test it; same name as the views in d_a_npc_ko1_c.h / _d1.h / _d2.h / _b.h |
| 0x9D1 | `m9D1` | `mAnimationState` | setAnm_anm 02276B9C 0 when the animation changes; written only; same name as the view in d_a_npc_ko1_c.h |
| 0x9D3 | `m9D3` | `mMessageSelector` | chk_talk 02277070 stores the item shown (dComIfGp_event_getPreItemNo) or 0xFF; getMsg picks the message by it; talk_1 02279BC0 0xFF; same name as the views in d_a_npc_ko1_c.h / _d2.h |
| 0x9D4 | `m9D4` | `mZigzagDirection` | ko_clcMovSpd 02276028 flips it when mZigzagTimer runs out and turns the run angle by -/+0x2000 by it; same name as the view in d_a_npc_ko1_b.h |
| 0x9D5 | `m9D5` | `mSleepState` | neru_1 0227A110 toggles it, neru_2 0227A1C8 switches on it and steps it; setStt 02278134 0; setMtx 02274A7C syncs the balloon frame while it is below 2; same name as the views in d_a_npc_ko1_d1.h / _d2.h |
| 0x9D7 | `m9D7` | `mHidden` | _draw 0227684C draws nothing while it is set; _execute 022765B0 clears it outside a demo; same name as the view in d_a_npc_ko1_b.h |
| 0x9D8 | `m9D8` | `mKeepShape` | _execute 022765B0 copies current.angle to shape_angle and mModelAngle only while it is clear; same name as the view in d_a_npc_ko1_b.h |
| 0x9D9 | `m9D9` | `mSkipMovement` | ko_nMove 022763A8 sets it when the move step is done; _execute 022765B0 skips ko_nMove and the position move while it is set; same name as the view in d_a_npc_ko1_b.h |
| 0x9DA | `m9DA` | `mDrawHidden` | _draw 0227684C draws nothing while it is set; same name as the view in d_a_npc_ko1_b.h |
| 0x9DB | `m9DB` | `mbMoveFinished` | ko_nMove 022763A8 sets it when a point or the path end is reached or a jump lands; setStt 02278134 clears it; walk_2 022793C8 / walk_3 0227949C / down_1 02279B90 change the state on it; same name as the view in d_a_npc_ko1_d1.h |
| 0x9DC | `m9DC` | `mPreviousGroundHit` | setMtx 02274A7C stores the ground-hit flag before the position correction; ko_nMove 022763A8 ends a jump when it was clear and the ground is hit now; setStt 02278134 sets 1 when the down / jump states start; same name as the view in d_a_npc_ko1_b.h |
| 0x9DD | `m9DD` | `mbTalkTriggered` | talk_1 02279BC0 sets 1; getMsg_HNA_3 02277784 picks the second message once it is set; same name as the views in d_a_npc_ko1_c.h / _d2.h |
| 0x9E0 | `m9E0` | `mbSetEyePos` | hana_action1 0227A374 .. store the state function's result; setAttention 02274A1C copies mEyePos to eyePos only while it or the argument is set; same code as daNpc_Bm1_c::mbSetEyePos (tww Bm1 mbSetEyePos = wait_1(); if (!mbSetEyePos && !i_setEyePos) return;) |
| 0x9E4 | `m9E4` | `mAttention` | hana_action1 0227A374 .. store chkAttention(); wait_1 0227896C / wait_2 02278AE0 / wait_a 022791F4 react while it is set; same name as the views in d_a_npc_ko1_d1.h / _d2.h |
| 0x9E5 | `m9E5` | `mTalking` | checkOrder 02275504 sets 1 on the talk command; the waits (wait_1 0227896C ..) start the talk while it is set; talk_1 02279BC0 clears it; same code as daNpc_Bm1_c::mTalking (tww Bm1 checkOrder m895 = true) |
| 0x9E6 | `m9E6` | `mHeadOnlyFollow` | lookBack 022758B8 passes it as the head-only flag of the joint look-at; setStt 02278134 sets 1 for the moving states and 0 for the others, chg_anmAtr 02276EB8 0; same code as daNpc_Bm1_c::mHeadOnlyFollow (tww Bm1 bool headOnlyFollow = m896) |
| 0x9E7 | `m9E7` | `mInDemo` | demo 02275544 sets and returns it; setMtx 02274A7C skips the animation and ground work while it is set; same name as the view in d_a_npc_ko1_b.h |
| 0x9FC | `m9FC` | `mRippleEmitter` | setPrtcl_Hamon 02277D48 keeps the ripple emitter, chk_start_swim 02277DEC clears it; same name as the view in d_a_npc_ko1_c.h |
| 0xA00 | `mA00` | `mBalloonEmitter` | setPrtcl_HanaPachi 02277FE8 keeps the balloon-pop emitter; same name as the view in d_a_npc_ko1_c.h |
| 0xA04 | `mA04` | `mBalloonLightEmitter` | setPrtcl_HanaPachi 02277FE8 keeps the second balloon-pop emitter; same name as the view in d_a_npc_ko1_c.h |
| 0xA08 | `mA08` | `mMoveMode` | setStt 02278134 sets 0 wait, 1 walk, 2 run, 3 swim, 4 jump; ko_nMove 022763A8 switches on it; same name as the views in d_a_npc_ko1_d1.h / _b.h |
| 0xA09 | `mA09` | `mPreviousMoveMode` | setStt 02278134 keeps mMoveMode here when a jump starts; ko_nMove 022763A8 returns to it on landing; same name as the views in d_a_npc_ko1_d1.h / _b.h |
| 0xA0A | `mA0A` | `mCutIndex` | privateCut 022757E4: the cut index from dComIfGp_evmng_getMyActIdx; same name as the view in d_a_npc_ko1_b.h |
| 0xA0B | `mA0B` | `mCutAction` | event_actionInit 0227577C: the cut's "ActNo" integer; same name as the view in d_a_npc_ko1_b.h |
| 0xA0C | `mA0C` | `mAnmAtrNo` | chg_anmAtr 02276EB8 stores the message animation attribute (below 0xD, when it changes); setAnm_ATR 02276E20 picks the attribute animation by it, control_anmAtr 02276F30 returns from 0xB; setStt 02278134 / endEvent 02275738 / anmAtr 02276F58 0xFF; same code as daNpc_Bm1_c::mAnmAtrNo (tww Bm1 m8F7) |
| 0xA0D | `mA0D` | `mMesgAnimeTag` | anmAtr 02276F58: the message's animation tag when it changes (if (tag != 0xFF && it != tag) { it = tag; chg_anmTag(); }); chg_anmTag 02276E08 / control_anmTag 02276E90 act on tag 10; setStt 02278134 / endEvent 02275738 0xFF; same code as daNpc_Bm1_c::mMesgAnimeTag (tww Bm1 m8F8) |
| 0x00 | `mA0F` | `mAnmNum` | setAnm_anm 02276B9C: the body animation number (returns at once when it is unchanged; also picks the head animation); setMtx 02274A7C plays a sound in animation 4; createInit 02274E28 0xE; same code as daNpc_Bm1_c::mAnmNum (tww Bm1 setAnm_anm mAnmNum = anmNum) |
| 0xA11 | `mA11` | `mBalloonAnimationNum` | set_balloonAnm_anm 02276C70: the balloon animation number (returns at once when it is unchanged); same name as the view in d_a_npc_ko1_c.h |
| 0xA12 | `mA12` | `mOrderEvent` | eventOrder 02276578 orders the talk while it is 1 or 2 (speak event at 1); checkOrder 02275504 clears it on the talk command; the waits set 2, setStt 02278134 0; same code as daNpc_Bm1_c::mOrderEvent (tww Bm1 m8FD) |
| 0xA13 | `mA13` | `mStatus` | setStt 02278134 stores the state; setAnm 02276D84 picks the state animation by it; hana_action1 0227A374 .. switch on it; same name as daNpc_Bm1_c::mStatus (tww) and the views in d_a_npc_ko1_c.h / _d1.h / _d2.h |
| 0xA14 | `mA14` | `mResumeStatus` | setStt 02278134 keeps the previous mStatus when the talk starts; talk_1 02279BC0 / talk_2 02279EF8 / manzai 02279FC0 return to it; same name as the views in d_a_npc_ko1_d1.h / _d2.h |
| 0xA15 | `mA15` | `mLookBackState` | lookBack 022758B8 switches on it (1 the player, 2 mLookPos, 3 mDesiredYRot); chg_anmAtr 02276EB8 sets 1 with m_jnt.mbTrn; setStt 02278134 / manzai 02279FC0 set it per state; same code as daNpc_Bm1_c::mLookBackState (tww Bm1 switch (mLookBackState)) |
| 0xA18 | `mA18` | `mActionStatus` | set_action 022742C0 sets 9 for the leaving action and 0 for the new one; hana_action1 0227A374 .. step it from 0; partner_srch 022753EC runs at 1; same code as daNpc_Bm1_c::mActionStatus (after tww daNpc_Bs1_c mActionStatus) |
| 0xA19 | `mA19` | `mbMessageTagsStarted` | anmAtr 02276F58: while the message types, applies the message's animation attribute only when it is 0, then adds 1; MSG_DISPLAYED (0xE) resets it; same name as the view in d_a_npc_ko1_c.h |

## daNpc_Kk1_c (d_a_npc_kk1.h; views Kk1CreationFields in d_a_npc_kk1_a.h, Kk1EventState in _b.h, Kk1ActionFields in _c.h)

The class header had WWHD offset names (`m86C`); zeldaret/tww has only nonmatching stubs for this class. The part views Kk1CreationFields (_a.h), Kk1EventState (_b.h) and Kk1ActionFields (_c.h) already named most fields, partly differently. One name per field (as for mt_class in enemies.md): where the views agree, the class takes their name; where they disagree, daNpc_Bm1_c's name wins when Kk1 runs the same code (setAttention / the state function result, lookBack's head-only flag, eventOrder / checkOrder, setAnm_anm), otherwise the name the uses support best. The losing view names are corrected in their own commit (`was X (misnamed)`). Open: `m861` (no use), and four fields the event cuts (_b.cpp) and the comment actions (_c.cpp) use for different values, so no one name fits yet: `m902` (kyorokyoro's look delay / init_CMT_TRN's turn ratio), `m904` (kyorokyoro's look timer / init_CMT_TRN's maximum turn step), `m92A` (event_proc's bye-done flag / getMsg_KK1_0's night-message flag).

Corrected view names (own commit, each declaration says `was X (misnamed)` with the reason): Kk1CreationFields (_a.h): `mActionResult` -> `mbSetEyePos`, `mEventAction` -> `mEventIdx`, `mDemoAnimation` -> `mPreserveShapeAngle`, `mLookState` -> `mMoveMode`, `mAnimationNo` -> `mAnmNum`, `mEventState` -> `mOrderEvent`; Kk1EventState (_b.h): `mLastAngle` -> `mModelAngle`, `mLookAngle` -> `mPreviousLookAngles`, `mMessageHandle` -> `mbSetEyePos`, `mEventIndex` -> `mEventIdx`, `mAnmEnded` -> `mAnimationEnded`, `mFollowPath` -> `mOnPath`, `mHideEffect` -> `mEffectHidden`, `mHeadOnly` -> `mHeadOnlyFollow`, `mDisableCollision` -> `mInDemo`, `mEffectAnmFrame` -> `mEffectFrame`, `mMoveAnm` -> `mMoveMode`, `mPathArg` -> `mCommentAction`, `mAnmState` -> `mAnmNum`, `mEventOrder` -> `mOrderEvent`; Kk1ActionFields (_c.h): `mLookTarget` -> `mLookPos`, `mActionResult` -> `mbSetEyePos`, `mEventAction` -> `mEventIdx`, `mTurnTimer` -> `mCutTimer`, `mUseLookTarget` -> `mHeadOnlyFollow`, `mAnimationNo` -> `mAnmNum`, `mEventState` -> `mOrderEvent`. The views keep their own names for the open fields 0x902, 0x904 and 0x92A.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x86C | `m86C` | `mPartnerId` | partner_search_sub 0226C4C4 stores the partner's process id (-1 when none); chngAnmAtr 0226EFB4 / wait_2 022703B4 / cut_init_RUN_START 0226CC80 search it; same name as the views Kk1CreationFields / Kk1EventState / Kk1ActionFields |
| 0x870 | `m870` | `mLookActorId` | lookBack 0226E604 looks at the actor with this id; same name as the view Kk1EventState |
| 0x87C | `m87C` | `mHomePos` | _execute 0226E99C copies current.pos on the first run (mHomeInitialized); same name as the view Kk1EventState |
| 0x888 | `m888` | `mHomeAngle` | _execute 0226E99C copies current.angle on the first run; wait_1 0226FE78 / wait_2 022703B4 turn back to its y; same name as the views Kk1EventState / Kk1ActionFields |
| 0x88E | `m88E` | `mModelAngle` | createInit 0226BFD0 sets it, setMtx 0226BDCC rotates the model by it; _execute 0226E99C copies current.angle into it while mPreserveShapeAngle is clear; same name as the view Kk1CreationFields; same code as daNpc_Ko1_c::mModelAngle |
| 0x8A8 | `m8A8` | `mLookPos` | lookBack 0226E604 sets it to the player's eye or the look actor's position and looks at it; chngAnmAtr 0226EFB4 sets the partner's position; same code as daNpc_Bm1_c / daNpc_Ko1_c::mLookPos; same name as the view Kk1EventState |
| 0x8D8 | `m8D8` | `mPreviousFrame` | play_animation 0226BB5C: a morf frame below it means the animation wrapped (mAnimationEnded = 1), then it takes the frame; same name as the view Kk1CreationFields |
| 0x8E0 | `m8E0` | `mPreviousLookAngles` | lookBack 0226E604 stores the look yaw (x), the joint control's head Y (y) and backbone Y (z); chk_parts_notMov 0226F6EC compares them; same name as the view Kk1ActionFields |
| 0x8E8 | `m8E8` | `mbSetEyePos` | wait_action1 02271084 stores the state function's result; setAttention 0226BD78 copies the eye position to eyePos only while it or the argument is set; cut_move_CATCH_START 0226DBB0 0; same code as daNpc_Bm1_c / daNpc_Ko1_c::mbSetEyePos (tww Bm1 mbSetEyePos = wait_1()) |
| 0x8EC | `m8EC` | `mEventIds` | eventOrder 0226E92C orders the event mEventIds[idx]; event_proc 0226E21C reads the current one; same name as the view Kk1EventState |
| 0x8F0 | `m8F0` | `mPickEventId` | init_CMT_PCK 022708B8 reads the event id it orders; same name as the view Kk1ActionFields |
| 0x8FC | `m8FC` | `mEventIdx` | eventOrder 0226E92C stores the index of the ordered event; checkOrder 0226C7DC / event_proc 0226E21C pick mEventIds by it; init_CMT_PCK 022708B8 sets 2; same code as daNpc_Bm1_c::mEventIdx (tww Bm1 mEventIdx = condition - 3; fopAcM_orderOtherEventId(this, mEventIdTable[mEventIdx])) |
| 0x908 | `m908` | `mCutTimer` | cut_init_RUN 0226CD04 .. set the cut's timer (-1 without one) and cut_move_RUN 0226DB64 .. end the cut when it runs out; init_CMT_TRN 0227066C sets the HIO turn time and move_CMT_TRN 022706E4 ends that comment action when it runs out: the countdown of the current cut or comment action; same name as the view Kk1EventState |
| 0x90A | `m90A` | `mTurnTimer` | cut_init_BYE 0226D0E8 sets the cut's delay (-1 without one); same name as the view Kk1EventState |
| 0x90E | `m90E` | `mAfterTalkTimer` | talk_1 02270F28 sets 15 + rnd(30) when the talk ends; same name as the view Kk1ActionFields |
| 0x912 | `m912` | `mTalkLookTimer` | chngAnmAtr 0226EFB4 sets the HIO time, talk_1 02270F28 / setStt 0226CB44 0; same name as the views Kk1CreationFields / Kk1ActionFields |
| 0x914 | `m914` | `mActionTimer` | wait_1 0226FE78 .. set 60 + rnd(30) and count it down; checkCommandTalk 0226C7A0 tests it; setStt 0226CB44 0; same name as the views Kk1CreationFields / Kk1ActionFields |
| 0x91A | `m91A` | `mTurnStartAngle` | init_CMT_TRN 0227066C keeps current.angle.y; move_CMT_TRN 022706E4 turns to its back; same name as the view Kk1ActionFields |
| 0x91C | `m91C` | `mLookYaw` | lookBack 0226E604 uses it as the look-at's Y angle; same name as the view Kk1EventState |
| 0x91E | `m91E` | `mRefusedAnswerCount` | next_msgStatus 0226F230 adds 1 per refused answer and picks the message by it; same name as the view Kk1ActionFields |
| 0x920 | `m920` | `mKeepMoving` | cut_init_BYE 0226D0E8 sets the cut's integer (0 without one); same name as the view Kk1EventState |
| 0x922 | `m922` | `mAnimationEnded` | play_animation 0226BB5C: the morf's play() result, also set when the frame wraps; setAnm_anm 0226C660 0; ctrlAnmAtr 0226F088 / wait_1 / the cuts test it; same name as the views Kk1CreationFields / Kk1ActionFields |
| 0x923 | `m923` | `mAnimationStarted` | setAnm_anm 0226C660 0 when the animation changes; written only; same name as the view Kk1CreationFields |
| 0x924 | `m924` | `mPresentedItem` | chk_talk 0226F654 stores the item shown (dComIfGp_event_getPreItemNo) or 0xFF; same name as the view Kk1ActionFields |
| 0x925 | `m925` | `mSwitchNo` | createInit 0226BFD0 takes it from the parameters ((prm >> 8) & 0xFF); searchActor_SWC00 0226B4A8 / wait_2 022703B4 test the switch; same name as the views Kk1CreationFields / Kk1ActionFields |
| 0x926 | `m926` | `mOnPath` | setStt 0226CB44 1 for the comment state; walk_1 022700E0 / wait_1 0226FE78 test and set it; cut_init_RUN 0226CD04 / event_move 0226D83C follow the path while it is set; same name as the views Kk1CreationFields / Kk1ActionFields |
| 0x927 | `m927` | `mWaitingAtPathEnd` | wait_1 0226FE78 .. set 1 at the path end and 0 when moving on; same name as the view Kk1ActionFields |
| 0x928 | `m928` | `mPartnerEventEnabled` | next_msgStatus 0226F230 sets 1; wait_2 022703B4 starts the partner event while it is set; same name as the view Kk1ActionFields |
| 0x92B | `m92B` | `mHideBody` | cut_init_BYE_START 0226D0A4 sets 1; _execute 0226E99C skips the collision while it is set; same name as the view Kk1EventState |
| 0x92C | `m92C` | `mResourceLoaded` | _create 0226C28C: the phase result == cPhs_COMPLEATE_e; same name as the view Kk1CreationFields |
| 0x92D | `m92D` | `mDemoSuspend` | _draw 0226ECA8 draws nothing while it is set; _execute 0226E99C clears it outside a demo; same name as the view Kk1EventState |
| 0x92E | `m92E` | `mEffectHidden` | setAnm_anm 0226C660 sets 1 / 0 by the animation; _draw 0226ECA8 draws the effect model only while it is clear; same name as the view Kk1CreationFields |
| 0x92F | `m92F` | `mPreserveShapeAngle` | cut_init_RUNAWAY_START 0226D454 sets 1; _execute 0226E99C copies current.angle to the shape and mModelAngle only while it is clear; demo 0226C8D8 0; same name as the view Kk1EventState |
| 0x930 | `m930` | `mHide` | cut_init_BYE_END 0226D1F0 / cut_init_RUNAWAY_END 0226D60C set 1; _draw 0226ECA8 draws nothing while it is set; same name as the view Kk1EventState |
| 0x931 | `m931` | `mHomeInitialized` | _execute 0226E99C stores mHomePos / mHomeAngle once and sets it; same name as the view Kk1EventState |
| 0x932 | `m932` | `mAttentionSelected` | wait_action1 02271084 stores chkAttention(); same name as the view Kk1ActionFields |
| 0x933 | `m933` | `mTalkRequested` | checkOrder 0226C7DC sets 1 on the talk command; wait_1 0226FE78 / walk_1 022700E0 start the talk on it; same name as the views Kk1CreationFields / Kk1ActionFields |
| 0x934 | `m934` | `mHeadOnlyFollow` | lookBack 0226E604 passes it as the head-only flag of the joint look-at; the cuts (cut_init_TRN 0226CE6C ..) and the waits (wait_1 0226FE78 ..) set 1 / 0; same code as daNpc_Bm1_c / daNpc_Ko1_c::mHeadOnlyFollow (tww Bm1 bool headOnlyFollow = m896) |
| 0x935 | `m935` | `mPlayerFar` | wait_3 02270C7C stores whether the player is far; wait_2 022703B4 0; same name as the view Kk1ActionFields |
| 0x936 | `m936` | `mInDemo` | demo 0226C8D8 sets and clears it and returns by it; _execute 0226E99C skips the collision while it is set; same name as the view Kk1CreationFields; same name as daNpc_Ko1_c::mInDemo (demo sets and returns it) |
| 0xAAC | `mAAC` | `mEffectFrame` | play_eff_anm 0226BA48 steps the effect brk (x) / btk (y) / bck (z) frames; setMtx 0226BDCC / _draw 0226ECA8 set the effect animations to them; same name as the view Kk1CreationFields |
| 0xAB4 | `mAB4` | `mEffectModel` | effcCreateHeap 0226B1E0 creates the effect model; _draw 0226ECA8 draws it with the brk / btk animations; same name as the views Kk1CreationFields / Kk1EventState |
| 0xABC | `mABC` | `mSweatEmitter` | setAse 0226C5F0 keeps the sweat (0x819E) emitter; flwAse 0226BCD4 moves it; same name as the view Kk1CreationFields |
| 0xAC0 | `mAC0` | `mCutIndex` | privateCut 0226DEF0: the cut index from dComIfGp_evmng_getMyActIdx, switched on; same name as the view Kk1EventState |
| 0xAC1 | `mAC1` | `mMoveMode` | event_move 0226D83C: 2 walks (walk rate / step / speed), else runs; takes the path point's argument 2 or 3; cut_init_RUN 0226CD04 / cut_init_BYE 0226D0E8 / setStt 0226CB44 (comment state) set 2 |
| 0xAC2 | `mAC2` | `mCommentAction` | cmmt_1 02270AE0 switches on it; move_CMT_WAI 02270570 / move_CMT_TRN 022706E4 / move_CMT_PCK 02270918 / setStt 0226CB44 set 0; checkCommandTalk 0226C7A0 tests 5; event_move 0226D83C clears it; same name as the views Kk1CreationFields / Kk1ActionFields |
| 0xAC3 | `mAC3` | `mTalkAnimation` | setAnm_ATR 0226EF4C picks the message animation by it; checkOrder 0226C7DC / endEvent 0226CBE8 0xFF; same name as the views Kk1CreationFields / Kk1ActionFields |
| 0xAC4 | `mAC4` | `mMessageTag` | anmAtr 0226F170: the message's animation tag when it changes; checkOrder 0226C7DC / endEvent 0226CBE8 0xFF; same name as the views Kk1CreationFields / Kk1ActionFields |
| 0xAC5 | `mAC5` | `mTextureAnimation` | setBtp 0226AE30 stores the texture animation number; play_btp_anm 0226B8E4 blinks only in number 0; same name as the view Kk1CreationFields |
| 0xAC6 | `mAC6` | `mAnmNum` | setAnm_anm 0226C660: the animation number (returns at once when it is unchanged); play_animation 0226BB5C switches on it; createInit 0226BFD0 0xC; same code as daNpc_Bm1_c / daNpc_Ko1_c::mAnmNum (tww Bm1 setAnm_anm mAnmNum) |
| 0xAC7 | `mAC7` | `mOrderEvent` | eventOrder 0226E92C orders by it (talk at 1 / 2, other events from 3); checkOrder 0226C7DC clears it once the talk / event starts; the waits set 2; same code as daNpc_Bm1_c / daNpc_Ko1_c::mOrderEvent (tww Bm1 m8FD) |
| 0xAC8 | `mAC8` | `mActionState` | setStt 0226CB44 stores the state; setAnm 0226CAD4 picks the state animation by it; wait_action1 02271084 switches on it; same name as the views Kk1CreationFields / Kk1ActionFields |
| 0xAC9 | `mAC9` | `mPreviousState` | setStt 0226CB44 keeps the previous state; talk_1 02270F28 returns to it; same name as the views Kk1CreationFields / Kk1ActionFields |
| 0xACA | `mACA` | `mLookMode` | lookBack 0226E604 switches on it; chngAnmAtr 0226EFB4 / the cuts / the waits set it; chk_attn 0226FC1C widens the angle in mode 1; same name as the views Kk1EventState / Kk1ActionFields |
| 0xACC | `mACC` | `mMessageType` | createInit 0226BFD0 / partner_search 0226C570 / getMsg 0226F61C branch on it; same name as the views Kk1CreationFields / Kk1ActionFields |
| 0xACD | `mACD` | `mActionStep` | partner_search 0226C570 runs at 1 and steps it; wait_action1 02271084 steps it from 0; same name as the views Kk1CreationFields / Kk1ActionFields |
| 0xACE | `mACE` | `mMessageAnimationStarted` | anmAtr 0226F170: applies the message's animation attribute only while it is 0, then adds 1; setStt 0226CB44 0; same name as the views Kk1CreationFields / Kk1ActionFields |

Later batch:

| Struct / offset | Old | New | Evidence |
|---|---|---|---|
| HIO 0x08 | `field_0x8` | `mRefCount` | daNpc_Kk1_HIO_c_ct 022711F4 -1; never read in WWHD or tww (stubs); after our decomp's daNpc_Pm1_HIO_c / daNpc_Ba1_HIO_c mRefCount (the same HIO layout) |

Open: `m906` (declared only: no use in WWHD; tww has stubs only).

## daNpc_Bs1_c (d_a_npc_bs1.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`); tww's code (src/d/actor/d_a_npc_bs1.cpp) uses them the same way. Beedle runs the shop code of tww's daNpc_Rsh1_c, so the shared fields take its names (tww header names, or our decomp's daNpc_Rsh1_c names above). Open (no use in WWHD or tww): `m2C5`, `m72C`, `m732`, `m733`, `m76F`, `m839`.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x43E | `m2C6` | `mBlinkTimer` | initTexPatternAnm 02212704 clears it; playTexPatternAnm 0221391C counts it down (cLib_calcTimer) and restarts the head texture pattern with 30 + rnd(100) (tww the same; tww daNpc_Rsh1_c calls it mTimer); same role as daNpcPeople_c::mBlinkTimer (npcs.md) |
| 0x7B6 | `m63E` | `mEvnWaitTimer` | evn_wait_init 02216C18 stores the cut's "Timer" (0 without one); evn_wait 02216C80 counts it down and ends the cut at 0 (tww the same) |
| 0x880 | `m708` | `mbItemMsgSet` | shop_talk 02215F2C: 1 when the item selection (now_triggercheck) set a new message in mCurrMsgNo; the next displayed/continue status is then taken without advancing and clears it (tww the same); after our decomp's daNpc_Rsh1_c mbItemMsgSet (same code) |
| 0x890 | `m718` | `mAttnBasePos` | nodeCallBack_Bs 0221225C stores the head position (tww setAttentionBasePos(pos) { m718 = value; }); setAttention 02216368 puts attention at it plus the HIO Y offset (tww) |
| 0x89C | `m724` | `mLookAtMaxVel` | lookBack 022163B0 eases it to HIO mMaxHeadTurnVel while turning, else 0, and passes it as the look-at's maxVel (tww the same); after tww daNpc_Rsh1_c mLookAtMaxVel (same code) |
| 0x89E | `m726` | `mActorAngle` | CreateInit 02213060 copies current.angle; setCollision 02213D5C rotates the collision offset by its y (tww the same); after tww daNpc_Rsh1_c mActorAngle (same CreateInit code) |
| 0x8A6 | `m72E` | `mMorfIsStop` | _execute 02213E18 stores mpMorf->play()'s result (1 without a morf) (tww the same); after tww daNpc_Rsh1_c mMorfIsStop (same code) |
| 0x8A7 | `m72F` | `mAttnSetCount` | nodeCallBack_Bs 0221225C adds 1 up to 0xFF (tww incAttnSetCount()); setAttention 02216368 needs it >= 2 (tww); same name as npcs.md mAttnSetCount |
| 0x8A8 | `m730` | `mbAttention` | wait_action 02217258 stores chkAttention()'s result; lookBack 022163B0 / wait01 02216748 look at the player while it is set (tww the same); after tww daNpc_Rsh1_c mbAttention (same code) |
| 0x8A9 | `m731` | `mTalking` | checkOrder 022139EC: 1 on the talk command; wait01 02216748 saves mLookMode and looks at the player while set; talk01 022167F0 / wait_action 02217258 clear it (tww the same); after our decomp's daNpc_Rsh1_c mTalking (same code) |
| 0x8AC | `m734` | `mMorfPrevFrame` | _execute 02213E18: a morf frame below it means the animation wrapped; it then takes the frame (tww the same); after tww daNpc_Rsh1_c mMorfPrevFrame (same code) |
| 0x8B0 | `m738` | `mCurrMsgNo` | evn_talk_init 022168EC the event's "MsgNo"; normal_talk 02215DA0 advances it by next_msgStatus; shop_talk 02215F2C passes it to now_triggercheck; talk01 022167F0 checks it (tww the same); after our decomp's daNpc_Rsh1_c mCurrMsgNo (same code) |
| 0x8B4 | `m73C` | `mShopMsgNo` | shop_talk 02215F2C: a copy of mCurrMsgNo, advanced by next_msgStatus(&it, &mCurrMsgNo) and set as the next message; talk 02216154 0 (tww the same); after our decomp's daNpc_Rsh1_c mShopMsgNo (same code) |
| 0x8B8 | `m740` | `mNextMsgNo` | getMsg 022157C8: returned once (then 0) before the default messages; getdemo_action 02217368 0xF4C / 0xF4E / 0x2F53 (tww the same); after our decomp's daNpc_Rsh1_c mNextMsgNo (same code) |
| 0x8BC | `m744` | `mEndMsgNo` | evn_talk_init 022168EC / evn_continue_talk_init 02216994: the event's "EndMsgNo"; evn_talk 022169F8 ends the talk when mCurrMsgNo reaches it, then clears it (tww the same); after our decomp's daNpc_Rsh1_c mEndMsgNo (same code) |
| 0x8E4 | `m76C` | `mItemSoldOut` | createShopList 02212BA0: 1 for the item sets marked sold out (bombs without the bomb bag, arrows without the bow); next_msgStatus 02214858 1 for the bought item; _draw 02214104 draws mpSoldSignModels[i] and _execute 02213E18 places it while [i] is set (tww the same) |
| 0x994 | `m828` | `mTexAnmIdx` | setTexAnm 02214360: the texture pattern index; initTexPatternAnm 02212704 loads l_btp_ix_tbl[it]; CreateHeap 02212818 1 (tww the same); after our decomp's daNpc_Rsh1_c mTexAnmIdx (same code) |
| 0x995 | `m829` | `mAnmIdx` | setAnm 022142F8: the animation index; setAnmFromMsgTag 02214388 goes back to the wait animation when animation 2/4/6/8/9 ends (tww the same); after our decomp's daNpc_Rsh1_c mAnmIdx (same code) |
| 0x996 | `m82A` | `mOrderState` | eventOrder 02213D00: 3 orders BS1/BS2_GETDEMO, 1/2 the talk; checkOrder 022139EC starts them (and 4) and clears it; next_msgStatus 02214858 3 after a purchase (tww the same); after our decomp's daNpc_Rsh1_c mOrderState (same code) |
| 0x997 | `m82B` | `mXyEventIdx` | XyEventCB 02212008: 0 for the Complimentary ID (PUT_PRAICE_TICKET), 1 for the Fill-Up Coupon (PUT_FULL_TICKET); event_action 0221709C waits for mEventIdxs[it] to end and sets 2; CreateInit 02213060 2 (tww the same) |
| 0x99C | `m830` | `mLookMode` | lookBack 022163B0: switch (it) picks the look target; wait01 02216748 2 while talking, wait_action 02217258 1 (tww the same); after our decomp's daNpc_Rsh1_c mLookMode (same code) |
| 0x99D | `m831` | `mPrevLookMode` | wait01 02216748: mLookMode saved when the talk starts; talk01 022167F0 / getdemo_action 02217368 restore it (tww the same); after our decomp's daNpc_Rsh1_c mPrevLookMode (same code) |
| 0x9A1 | `m835` | `mTalkStep` | talk 02216154: 0 starts (getMsg), 1 waits for the message, 2 normal_talk, 3 shop_talk, -1 stops; talkInit 022139E0 0; shop_talk 02215F2C 2 on message 0xF3F (tww the same); after our decomp's daNpc_Rsh1_c mTalkStep (same code) |
| 0x9A2 | `m836` | `mbTalkedBefore` | getMsg 022157C8 sets it with the first greeting (0xF3D / 0xF5A / 0xF5C, type 1: 0x2F45..0x2F5E) and gives the repeat greeting (0xF41 / 0xF5B / 0xF5D / 0x2F46 ..) while it is set (tww the same) |
| 0x9A3 | `m837` | `mbBombMsgDone` | getMsg 022157C8 sets it with the bomb-sale message (0xF55 / 0x2F64, event bit 0x1F20) and gives 0xF58 / 0x2F67 while it is set; CreateInit 02213060 0 (tww the same) |
| 0x9A4 | `m838` | `mbMsg2F5FDone` | getMsg 022157C8 (type 1) sets 1 with message 0x2F5F (event bit 0x2040 first set) and then gives 0x2F60 instead of 0x2F61 (tww the same) |
| 0x9A6 | `m83A` | `mGetDemoEventIdx` | CreateInit 02213060: dComIfGp_evmng_getEventIdx("BS1_GETDEMO" / "BS2_GETDEMO"); eventOrder 02213D00 orders it; getdemo_action 02217368 waits for its end (tww the same) |
| 0x9A8 | `m83C` | `mMessageRupee` | next_msgStatus 02214858 stores dComIfGp_getMessageRupee() for the sell questions (0xF78 ..) and pays it per item (daNpc_Bs1_setPayRupee(it, getBuyItem())) on the answer (tww the same) |
| 0x9AC | `m840` | `mSellItemNo` | getMsg 022157C8 stores the spoils item (isEmono) the player shows; next_msgStatus 02214858 counts it (daNpc_Bs1_getBuyItemMax(mMessageRupee, it)) (tww the same) |

Open: `m70C[0xF]` (padding: no use in WWHD or tww; kept as declared).

## daNpc_Jb1_c, JbAnimationParameters and jb1::Hio (d_a_npc_jb1.h)

Named from zeldaret/tww's code (d217e2b, src/d/actor/d_a_npc_jb1.cpp, which keeps the GameCube `field_0x` names) and our decomp's use. The header declares several fields per line without offset prefixes, so a line with several renamed fields carries one `/* NEW (0xOFF) was OLD: ... */` line per field above it. The look, order and animation-attribute fields run the same code as tww daNpc_Bm1_c / daNpc_Ls1_c and our daNpc_Km1_c / daNpc_Hi1_c and take their names. Open: `field_0x6DC` (the dKy_tevstr_c: only the constructor 0224769C fills it; tww: not used), `field_0x909` (setAnm_anm only clears it), `field_0x90C` (read only: _execute / _draw stop while it is set and no demo runs; open in daNpc_Hi1_c / daNpc_Ls1_c too), `field_0x90E` (read only in _draw, never set), `field_0x916`, `field_0x91F` (padding in tww), JbAnimationParameters `field_0x1` (no use), HioParameters `field_0x04` (only the HD debug branch reads its byte).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x7E8 | `field_0x6D0` | `mLightPos` | setMtx 02247B7C: an offset through the m_cse_jnt_num joint matrix, copied to mPointLight.mPos (tww mDoMtx_stack_c::multVec(&temp, &it); field_0x7F8.mPos.set(it)) |
| 0xB64 | `field_0x7F8` | `mPointLight` | createInit 02247E84 registers it (tww dKy_plight_set(&it)); setMtx 02247B7C sets its position from mLightPos and its colour, power and fluctuation from the HIO (tww); _delete removes it (tww dKy_plight_cut) |
| 0xC04 | `field_0x834` | `mLmpBrkFrame` | setMtx 02247B7C plays m_lmp_brk with it (tww playBrkAnm(m_lmp_brk, &it)); _draw 022485C4 enters mBrkAnm at it (tww mBrkAnm.entry(modelData, it)); create_Anm 02247260 0 |
| 0xC88 | `field_0x858` | `mLgtBrkFrame` | setMtx 02247B7C plays m_lgt_brk with it (tww playBrkAnm(m_lgt_brk, &it)); _draw 022485C4 enters mBrkAnm2 at it on m_lgt_mdl (tww); create_lgt 022474D8 0 |
| 0xD20 | `field_0x8F4` | `mAttPos` | setAttention 02247AC4: current.pos plus a fixed offset turned by current.angle.y; attention_info.position is it plus HIO mAttPosOffsetY and eyePos is it (tww the same) |
| 0xD34 | `field_0x908` | `mbMorfAnimStopped` | setMtx 02247B7C: the morf's play() result, or 1; setAnm_anm 02248780 0 (tww the same); after tww daNpc_Ls1_c mbMorfAnimStopped (same code) |
| 0xD39 | `field_0x90D` | `mbNoShapeAngle` | _execute 02248444: shape_angle follows current.angle only while it is 0 (tww if (!it) shape_angle = current.angle); read only, never written in WWHD or tww; after our decomp's daNpc_Pf1_c mbNoShapeAngle (same code) |
| 0xD3B | `field_0x90F` | `mbRanExecute` | _execute 02248444: 0 on the first run, when mCurrentPos and mCurrentRot are taken, then 1 (tww the same); after tww daNpc_Bm1_c mbRanExecute (same code) |
| 0xD3C | `field_0x910` | `mbSetEyePos` | wait_action1 02248A0C stores 1, the inlined wait_1() result (tww it = wait_1()); written only in WWHD and tww; after tww daNpc_Ls1_c mbSetEyePos (same wait_action1 code) |
| 0xD41 | `field_0x915` | `mTalking` | checkOrder 02248128 sets 1 on the talk command (tww the same); written only in WWHD and tww; same checkOrder code as daNpc_Km1_c / daNpc_Hi1_c::mTalking |
| 0xD43 | `field_0x917` | `mInDemo` | demo 02248168: 1 while demoActorID is set, 0 otherwise, and returns it; setMtx 02247B7C plays the animation only while it is 0 (tww the same); same code as daNpc_Km1_c / daNpc_Hi1_c::mInDemo |
| 0xD46 | `field_0x91A` | `mAnmAtrNo` | chg_anmAtr 02248874 stores the attribute; setAnm_ATR 0224885C plays l_atrPrm[it]; anmAtr 02248894 0xFF (tww the same, control_anmAtr switches on it); same code as daNpc_Km1_c / daNpc_Hi1_c::mAnmAtrNo |
| 0xD47 | `field_0x91B` | `mMesgAnimeTag` | anmAtr 02248894: the message's animation tag when it changes (if (tag != 0xFF && it != tag)) (tww the same); after tww daNpc_Ls1_c mMesgAnimeTag (same code) |
| 0xD48 | `field_0x91C` | `mAnmNum` | setAnm_anm 02248780 skips an entry whose anmNum equals it, else stores it and loads anmNum_toResID(it) (tww the same); createInit 02247E84 2; after tww daNpc_Bm1_c mAnmNum (same setAnm_anm code) |
| 0xD49 | `field_0x91D` | `mOrderEvent` | eventOrder 0224840C orders the talk at 1 / 2; checkOrder 02248128 starts it and clears it (tww the same); same code as daNpc_Km1_c / daNpc_Hi1_c::mOrderEvent |
| 0xD4A | `field_0x91E` | `mStt` | setStt 02248A04 stores the state; setAnm 02248814 plays l_anmPrm[it]; wait_action1 02248A0C switches on it (tww the same); same code as daNpc_Km1_c / daNpc_Hi1_c::mStt |
| 0xD4C | `field_0x920` | `mHeapSizeIdx` | charDecide 022478D8 0 (tww the same); tww _create creates the solid heap with a_size_tbl[it]; same code as daNpc_Ls1_c / daNpc_Hi1_c::mHeapSizeIdx |
| 0xD4D | `field_0x921` | `mType` | charDecide 022478D8 0 (tww -1 or 0); createInit 02247E84 switches on it (tww createInit / getMsg switch (it)); after tww daNpc_Ls1_c mType (same charDecide code) |
| 0xD4E | `field_0x922` | `mActionStatus` | set_action 022478EC sets 9 for the leaving action and 0 for the new one; wait_action1 02248A0C switches on it and steps it (tww the same); after tww daNpc_Bs1_c mActionStatus |
| 0xD4F | `field_0x923` | `mAnmAtrSetCnt` | anmAtr 02248894: applies the message's animation attribute only when it is 0, then adds 1; resets it (tww the same); same code as daNpc_Km1_c / daNpc_Hi1_c::mAnmAtrSetCnt |
| JbAnimationParameters +0x00 | `field_0x0` | `anmNum` | setAnm_anm 02248780 compares it with mAnmNum and loads it; setAnm 02248814 plays only entries with it >= 0 (tww the same); after tww daNpc_Bm1_c::anm_prm_c anmNum (same setAnm_anm code) |
| HioParameters +0x00 | `field_0x00` | `mAttPosOffsetY` | setAttention 02247AC4 puts attention_info.position this far above mAttPos (tww the same); same role as tww daNpc_Ls1_c::hio_prm_c mAttPosOffsetY |
| HioParameters +0x06 | `field_0x06` | `mLightColorR` | setMtx 02247B7C: mPointLight.mColorR (tww field_0x7F8.mColor.r = it) |
| HioParameters +0x08 | `field_0x08` | `mLightColorG` | setMtx 02247B7C: mPointLight.mColorG (tww field_0x7F8.mColor.g = it) |
| HioParameters +0x0A | `field_0x0A` | `mLightColorB` | setMtx 02247B7C: mPointLight.mColorB (tww field_0x7F8.mColor.b = it) |
| HioParameters +0x0C | `field_0x0C` | `mLightPower` | setMtx 02247B7C: mPointLight.mPower (tww field_0x7F8.mPower = it) |
| HioParameters +0x10 | `field_0x10` | `mLightFluctuation` | setMtx 02247B7C: mPointLight.mFluctuation (tww field_0x7F8.mFluctuation = it) |
| jb1::Hio +0x04 | `field_0x8` | `mRefCount` | the HIO constructor 02248A98 -1; _create 02247F64 adds 1 (creating the HIO child at < 0); _delete 02248098 subtracts 1 (tww the same); after our decomp's daTag_Ba1_HIO_c / daNpc_Pm1_HIO_c mRefCount (same code) |

## daNpc_Ac1_c (d_a_npc_ac1.h)

zeldaret/tww has only stubs for this class (its header keeps the GameCube offset names `mXXX`), so the names come from our decomp's use. Seven joint and texture fields take the original names from the JUT_ASSERT_fail texts in d_a_npc_ac1.cpp; the rest run the same code as tww daNpc_Bm1_c and our daNpc_Km1_c / daNpc_Hi1_c and take their names. Open: `m85D` (read only: _execute / _draw stop while it is set and no demo runs; open in daNpc_Hi1_c / daNpc_Ls1_c / daNpc_Jb1_c too), `m861` (read only in _draw, never set), `m878` (0x9E8: only charDecide writes it (0), never read; _create uses the fixed a_heap_size, so it is no heap index).

Misnamed (corrected in their own commit): `m_handR_jnt_num` -> `m_hnd_R_jnt_num`, `m_armL_loc_jnt_num` -> `m_armL1_jnt_num`, `m_armR_loc_jnt_num` -> `m_armR1_jnt_num`. create_arm_Anm 021E2A50 asserts each index with the original text ("m_hnd_R_jnt_num >= 0", lines 0x791 / 0x794 / 0x797); tww daNpc_Bm1_c uses the same names. `mType` (0x9E8) -> `m878` (tww's name, left open): only charDecide 021E3060 writes it (0), it is never read, and _create 021E3794 uses the fixed a_heap_size, so it is not the type or a heap index.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x7E4 | `m6CC` | `m_hed_jnt_num` | create_Anm 021E24C4: the "head" joint's index, asserted with the original text "m_hed_jnt_num >= 0" (line 0x71F); nodeAc1Control 021E22A4 turns it by the joint control; same name as tww daNpc_Bm1_c / daNpc_Ls1_c m_hed_jnt_num |
| 0x7E5 | `m6CD` | `m_bbone_jnt_num` | create_Anm 021E24C4: the "backbone" joint's index, asserted with the original text "m_bbone_jnt_num >= 0" (line 0x722); nodeAc1Control 021E22A4 turns it; same name as tww daNpc_Bm1_c m_bbone_jnt_num |
| 0x7E6 | `m6CE` | `m_arm_L_jnt_num` | create_Anm 021E24C4: the "armL" joint's index, asserted with the original text "m_arm_L_jnt_num >= 0" (line 0x725); nodeAc1Control 021E22A4 / CreateHeap 021E2D68 use it; same name as tww daNpc_Bm1_c m_arm_L_jnt_num |
| 0x7E7 | `m6CF` | `m_arm_R_jnt_num` | create_Anm 021E24C4: the "armR" joint's index, asserted with the original text "m_arm_R_jnt_num >= 0" (line 0x728); nodeAc1Control 021E22A4 / CreateHeap 021E2D68 use it; same name as tww daNpc_Bm1_c m_arm_R_jnt_num |
| 0x7EC | `m6D4` | `mpItemModel` | create_itm_Mdl 021E2C88 creates it (or nullptr); setMtx 021E3358 puts it on the arm morf's handR joint and _draw 021E417C draws it while mbHasArms is set; same name as daNpc_Ko1_c::mpItemModel (npcs.md) |
| 0x7F0 | `m6DC` | `m_hed_tex_pttrn` | setBtp 021E2760 loads the head texture pattern, asserted with the original text "m_hed_tex_pttrn != (0)" (line 0x23C); plyTexPttrnAnm 021E3230 / demo 021E3990 read its frame max; same name as tww daNpc_Bm1_c m_hed_tex_pttrn |
| 0x868 | `m6F4` | `mBlinkFrame` | setBtp 021E2760 0; plyTexPttrnAnm 021E3230 adds 1 and wraps it at m_hed_tex_pttrn's frame max; demo 021E3990 the same; after tww daNpc_Bm1_c mBlinkFrame (same plyTexPttrnAnm code) |
| 0x870 | `m6FC` | `mpWingMorf` | setAnm_anm 021E43A4 sets the wing animation (wingAnmNum_toResID); setMtx 021E3358 plays and calcs it, _draw 021E417C draws it, _delete 021E38D4 stops it; after tww daNpc_Bm1_c mpWingMorf (same code) |
| 0x874 | `m700` | `m_wngL1_jnt_num` | create_wng_Anm 021E2858: the "wingL_loc" joint's index, asserted with the original text "m_wngL1_jnt_num >= 0" (line 0x760); nodeWngControl 021E1EFC uses it; same name as tww daNpc_Bm1_c m_wngL1_jnt_num |
| 0x875 | `m701` | `m_wngR1_jnt_num` | create_wng_Anm 021E2858: the "wingR_loc" joint's index, asserted with the original text "m_wngR1_jnt_num >= 0" (line 0x763); nodeWngControl 021E1EFC uses it; same name as tww daNpc_Bm1_c m_wngR1_jnt_num |
| 0x87C | `m708` | `mpArmMorf` | setAnm_anm 021E43A4 sets the arm animation (armAnmNum_toResID); setMtx 021E3358 plays and calcs it and puts mpItemModel on its handR joint; _draw 021E417C draws it; after tww daNpc_Bm1_c mpArmMorf (same code) |
| 0x964 | `m7F4` | `mInitialPos` | _execute 021E3F90 copies current.pos on the first run (with mInitialAngle, then mbRanExecute = 1); written only; after tww daNpc_Bm1_c mInitialPos (same code) |
| 0x970 | `m800` | `mInitialAngle` | _execute 021E3F90 copies current.angle on the first run; wait_1 021E4A74 sets mDesiredYRot to its y; same code as daNpc_Bm1_c / daNpc_Km1_c::mInitialAngle |
| 0x984 | `m814` | `mLookPos` | createInit 021E35E4 current.pos; lookBack 021E3CC0 looks at it (dstPos); same code as daNpc_Bm1_c / daNpc_Km1_c::mLookPos |
| 0x99C | `m82C` | `mPrevMorfFrame` | setMtx 021E3358: a morf frame below it means the animation wrapped, then it takes the frame; setAnm_anm 021E43A4 0; same code as daNpc_Km1_c::mPrevMorfFrame (after tww daNpc_Ls1_c) |
| 0x9BC | `m84C` | `mSttTimer` | wait_1 021E4A74 sets 60 and counts it down (cLib_calcTimer) before the next look; talk_1 021E4B38 60, setStt 021E4A38 0; same role as daNpc_Bm1_c::mSttTimer |
| 0x9C2 | `m852` | `mLookAtMaxVel` | lookBack 021E3CC0 eases it to HIO mCalcAngleTarget while turning, else 0, and passes it as the look-at's maxVel; same code as daNpc_Km1_c::mLookAtMaxVel |
| 0x9C4 | `m854` | `mDesiredYRot` | lookBack 021E3CC0: the look-at's default Y angle; wait_1 021E4A74 sets mInitialAngle.y; same code as daNpc_Km1_c / daNpc_Bm1_c::mDesiredYRot |
| 0x9C8 | `m858` | `mbMorfAnimStopped` | setMtx 021E3358: the morf's play() result, or 1; setAnm_anm 021E43A4 0; same code as daNpc_Km1_c::mbMorfAnimStopped (after tww daNpc_Ls1_c) |
| 0x9CE | `m85E` | `mbHasArms` | setAnm_anm 021E43A4: the animation entry's arm flag (mbArm == 1); createInit 021E35E4 / setMtx 021E3358 / _draw 021E417C run the arm morf and mpItemModel only while it is set; after tww daNpc_Bm1_c mbHasArms (same createInit / setMtx code) |
| 0x9CF | `m85F` | `mbNoShapeAngle` | _execute 021E3F90: shape_angle follows current.angle only while it is 0; read only, never written in WWHD (tww has no code for this class); after our decomp's daNpc_Pf1_c mbNoShapeAngle (same code) |
| 0x9D0 | `m860` | `mSkipPosMove` | _execute 021E3F90 clears it each frame and moves the actor (fopAcM_posMoveF) only while it is 0; same code as daNpc_Bm1_c::mSkipPosMove |
| 0x9D3 | `m863` | `mbRanExecute` | _execute 021E3F90: 0 on the first run, when mInitialPos and mInitialAngle are taken, then 1; after tww daNpc_Bm1_c mbRanExecute (same code) |
| 0x9D9 | `m869` | `mTalking` | checkOrder 021E3950 sets 1 on the talk command; wait_1 021E4A74 starts the talk while it is set; talk_1 021E4B38 clears it; same code as daNpc_Km1_c / daNpc_Hi1_c::mTalking |
| 0x9DB | `m86B` | `mInDemo` | demo 021E3990: 1 while demoActorID is set, 0 otherwise, and returns it; setMtx 021E3358 / _execute 021E3F90 test it; same code as daNpc_Km1_c / daNpc_Hi1_c::mInDemo |
| 0x9DE | `m86E` | `mAnmAtrNo` | chg_anmAtr 021E45D0 stores the attribute; setAnm_ATR 021E4560 plays a_anm_prm_tbl[it]; anmAtr 021E45F4 / endEvent 021E49F8 / setStt 021E4A38 0xFF; same code as daNpc_Km1_c / daNpc_Hi1_c::mAnmAtrNo |
| 0x9E0 | `m870` | `mBtpNum` | setAnm_tex 021E4384 stores the btp number when it changes; iniTexPttrnAnm 021E284C / plyTexPttrnAnm 021E3230 use it; CreateHeap 021E2D68 0, demo 021E3990 3; same code as daNpc_Km1_c::mBtpNum (after tww daNpc_Ls1_c) |
| 0x9E1 | `m871` | `mAnmNum` | setAnm_anm 021E43A4 skips an entry whose mAnmNum equals it, else stores it and loads anmNum_toResID / armAnmNum_toResID / wingAnmNum_toResID(it); createInit 021E35E4 4; after tww daNpc_Bm1_c mAnmNum (same setAnm_anm code) |
| 0x9E4 | `m874` | `mOrderEvent` | eventOrder 021E3F58 orders the talk at 1 / 2; checkOrder 021E3950 starts it and clears it; wait_1 021E4A74 sets 2; same code as daNpc_Km1_c / daNpc_Hi1_c::mOrderEvent |
| 0x9E5 | `m875` | `mStt` | setStt 021E4A38 stores the new state; setAnm 021E44DC plays a_anm_prm_tbl[it]; wait_action1 021E4C10 switches on it; same code as daNpc_Km1_c / daNpc_Hi1_c::mStt |
| 0x9E6 | `m876` | `mPrevStt` | setStt 021E4A38 keeps the previous mStt; talk_1 021E4B38 returns to it (setStt(it)); same code as daNpc_Km1_c / daNpc_Hi1_c::mPrevStt |
| 0x9E7 | `m877` | `mLookMode` | lookBack 021E3CC0 switches on it; setStt 021E4A38 / wait_1 021E4A74 set 1 / 3; same code as daNpc_Km1_c / daNpc_Hi1_c::mLookMode |
| 0x9E9 | `m879` | `mType` | charDecide 021E3060 sets 0; createInit 021E35E4 stops (returns false) unless it is 0; getBitMask 021E47EC (mask 0x10) and getMsg 021E4938 (getMsg_AC1_0) pick the type-0 code only while it is 0; after daNpc_Jb1_c / daNpc_Hi1_c mType (the same charDecide / createInit / getMsg role) |

Later batch:

| Struct / offset | Old | New | Evidence |
|---|---|---|---|
| HIO 0x08 | `field_0x8` | `mRefCount` | daNpc_Ac1_HIO_c_ct 021E4CCC -1; never read in WWHD or tww (stubs); after our decomp's daNpc_Pm1_HIO_c / daNpc_Ba1_HIO_c mRefCount (the same HIO layout) |

## daNpc_Ba1_c (d_a_npc_ba1.h)

zeldaret/tww (d217e2b, src/d/actor/d_a_npc_ba1.cpp) names part of this class and keeps the GameCube offset names `mXXX` for the rest; tww's code uses these fields the same way. The first batch names the fields that run the same code as daNpc_Km1_c / daNpc_Hi1_c / daNpc_Bm1_c / daNpc_Ac1_c; the second batch (rows from `m7F0` on, and the HIO row) names the rest from their uses. Still open (evidence not enough): `m7EF` (setAnm_anm 021EFD10 only clears it, never read in WWHD or tww), `m7F8` (no use in WWHD or tww).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x8FA | `m78A` | `mAngle` | createInit 021EF4BC sets current.angle and copies it to shape_angle; setMtx 021EF2A0 rotates the model by its y; eMove_MOV_POS_ 021F04DC turns it to the player; _execute 021F1064 copies it to shape_angle (tww the same); same name as daNpc_Hi1_c::mAngle (after tww daNpc_Ls1_c mAngle) |
| 0x90C | `m79C` | `mLookPos` | createInit 021EF4BC current.pos; lookBack 021F0B18 looks at it (dstPos) (tww the same); same code as daNpc_Bm1_c / daNpc_Km1_c::mLookPos |
| 0x965 | `m7F5` | `mbNoShapeAngle` | _execute 021F1064: shape_angle takes mAngle only while it is 0; eInit_MOV_POS_ 021EFF78 / eMove_MOV_POS_ 021F04DC set it during the move and clear it at the end (tww the same); same role as daNpc_Ac1_c / our decomp's daNpc_Pf1_c mbNoShapeAngle |
| 0x966 | `m7F6` | `mSkipPosMove` | _execute 021F1064 clears it each frame and moves the actor (fopAcM_posMoveF) only while it is 0 (tww the same); same code as daNpc_Bm1_c / daNpc_Ac1_c::mSkipPosMove |
| 0x967 | `m7F7` | `mNoDraw` | eInit_ACTOR_DRW_CONTROL_ 021EFFDC sets and clears it by the cut, eInit_MOV_POS_ 021EFF78 sets it; _draw 021F128C draws nothing while it (or mbInitGrandma0) is set (tww the same); same role as daNpc_Bm1_c::mNoDraw |
| 0x979 | `m809` | `mTalking` | checkOrder 021EF8B4 sets 1 on the talk command; wait_1 021F2040 / wait_2 021F24B8 / ZZZwai 021F26D4 start the talk while it is set; talk_1 021F217C clears it (tww the same); same code as daNpc_Km1_c / daNpc_Hi1_c::mTalking |
| 0x97E | `m80E` | `mAnmAtrNo` | chg_anmAtr 021F1560 stores the attribute; setAnm_ATR 021F14F0 plays a_anm_prm_tbl[it]; control_anmAtr 021F1584 switches on it; anmAtr 021F15B8 / endEvent 021EFC9C / setStt 021F1E78 0xFF (tww the same); same code as daNpc_Km1_c / daNpc_Hi1_c::mAnmAtrNo |
| 0x982 | `m812` | `mOrderEvent` | eventOrder 021F0F78 orders by it (1 / 2 the talk); checkOrder 021EF8B4 starts it and clears it; event_proc 021F0D58 1 / 5, wait_0 021F1F0C set it (tww the same); same code as daNpc_Km1_c / daNpc_Hi1_c::mOrderEvent |
| 0x988 | `m818` | `mActionStatus` | set_action 021EECEC sets PROC_END (9) for the leaving action and PROC_START (0) for the new one; wait_action1 021F2818 / partner_srch 021EF848 switch on it and step it (tww the same); after tww daNpc_Bs1_c mActionStatus |
| 0x989 | `m819` | `mAnmAtrSetCnt` | anmAtr 021F15B8: applies the message's animation attribute only when it is 0, then adds 1; resets it (tww the same); same code as daNpc_Km1_c / daNpc_Hi1_c::mAnmAtrSetCnt |
| 0x960 | `m7F0` | `mbSoupGiven` | event_proc 021F0D58 sets 1 when the soup event (mEventIdx 1, the bottle gets dItemNo_SOUP_BOTTLE_e) ends after GRANDMA_HEALED; getMsg_BA1_3 021F1B70 then gives 0x801 (0x7f6 before GRANDMA_HEALED); talk_1 021F217C clears it when message 0x801 closes (tww the same) |
| 0x962 | `m7F2` | `mHealedDays` | createInit 021EF4BC copies event register 0xA60F, which dKankyo_DayProc 025542BC raises by 1 each day after GRANDMA_HEALED (at most 3); getMsg_BA1_3 021F1B70 gives 0x7fc / 0x7fd at 3 or more (0x7fb / 0x7fa below) and sets the register back to 0 (tww the same) |
| 0x96A | `m7FA` | `mbTalkedBefore` | getMsg_BA1_3 021F1B70 sets it with the first message after healing (0x7fc / 0x7fb) and gives the repeat message (0x7fd / 0x7fa) while it is set; wait_action1 021F2818 loads it from tmp bit 0x0310 (set by wait_0 021F1F0C's greeting when the player comes near) and clears that bit; getMsg_BA1_0 021F19AC gives 0x7d8 while it is set (tww the same); same pattern as daNpc_Bs1_c::mbTalkedBefore |
| 0x96B | `m7FB` | `mbMsg7E4Done` | talk_1 021F217C sets it when message 0x7e4 closes; getMsg_BA1_0 021F19AC then gives 0x7e5 instead of 0x7e4 (tww the same) |
| 0x96D | `m7FD` | `mbSmallCyl` | _execute 021F1064: setCollision with radius 30 instead of 50 while it is set; never written in WWHD or tww |
| 0x96E | `m7FE` | `mbNoCollision` | eInit_MOV_POS_ 021EFF78 sets it; event_proc 021F0D58 clears it when the healing event (mEventIdx 2) ends; _execute 021F1064 skips setCollision while it (or mbInDemo) is set (tww the same) |
| 0x96F | `m7FF` | `mbGoLinkRM` | wait_2 021F24B8 sets 1 after moving to (-290, 0, 110) (event bit 0x3202, not collect 1) when the player is on her floor, and orders event 1; while it and mTalking are set wait_2 calls dComIfGp_setNextStage("LinkRM", 201) (tww the same) |
| HIO 0x08 | `field_0x8` | `mRefCount` | daNpc_Ba1_HIO_c_ct 021F2BB8 -1; never read in WWHD or tww d_a_npc_ba1 (no _create / _delete count); after tww daTag_Ba1_HIO_c mRefCount: the same HIO layout (mNo at 0x04, s32 at 0x08, then the parameter block at 0x0C) and the same constructor value -1 |

Open: anm_prm_c `field_0x02` (never read in WWHD or tww; 0 in every tww a_anm_prm_tbl entry); the unused padding arrays `field_0x870[8]`, `field_0x918[0x18]`, `field_0x954[4]`, `field_0x95C[2]` and `field_0x963` (no use in WWHD or tww; kept as declared).

## daNpc_Mk_c (d_a_npc_mk.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names `field_0xNNN` for these fields; its code uses them the same way, so the names come from the uses (event data names "Pos", "Timer", "Angle" for the demo fields).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x800 | `field_0x688` | `mPathRun` | init 02295C30 sets it to the actor's path (parameter byte 1) and fails without it; visitInit 02297D78 / walkPath 02297F40 / runaway 02299A80 / seek_action 0229A3C8 walk it (setNearPathIndx, walkPath, runAwayProc, setRndPathPos) (tww the same); same role as daNpc_Mn_c::mPathRun |
| 0x808 | `field_0x690` | `mChaseAreaPath` | init 02295C30: the path after mPathRun (dNpc_PathRun_nextPath), used as an area: nextVisitMode 02297990 keeps VISIT_RUN_LINK / VISIT_REACHED_LINK only while the player is inside it (chkInside), else VISIT_LEFT_PATH (tww the same) |
| 0x810 | `field_0x698` | `mNoticeAreaPath` | init 02295C30: the path after mChaseAreaPath, used as an area: nextVisitMode 02297990 goes to VISIT_NOTICE_LINK / VISIT_RUN_LINK when the player is within 400 and inside it (chkInside) (tww the same) |
| 0x81C | `field_0x6A4` | `mIgnoreLinkTimer` | next_msgStatus 02297210 sets 1 on a non-zero answer to message 0x26CA; visitInit 02297D78 sets 100 (VISIT_WALK_PATH_FAST) / 600 (VISIT_WALK_PATH_IGNORE_LINK) and nextVisitMode 02297990 counts it down before the next mode; while it is non-zero getLookBackMode 022976A0 gives 2 and visit01 022995C4 drops the talk order (tww the same) |
| 0x844 | `field_0x6D0` | `mMoveStartPos` | demoProc 022987B0 (cuts 3 / 4): current.pos when the move starts; chkPointPass(it, mMoveGoalPos, current.pos) ends the cut (tww the same) |
| 0x850 | `field_0x6DC` | `mMoveGoalPos` | demoProc 022987B0 (cuts 3 / 4): the event's "Pos" data; Mk runs to it, the cut ends at it (cut 4 snaps x / z to it) (tww the same) |
| 0x85C | `field_0x6E8` | `mMoveTimer` | demoProc 022987B0 (cuts 3 / 4): the event's "Timer" data (-1 without it); counted down while running, the cut ends at 0 (tww the same) |
| 0x864 | `field_0x6F0` | `mTargetAngleY` | demoProc 022987B0: the event's "Angle" data (cut 16), turned to by cLib_addCalcAngleS; runaway 02299A80: runAwayProc writes the flee angle; runawayInit 0229814C jumps along it (tww the same) |

## daNpc_Btsw2_c and daNpc_Btsw2_HIO_c (d_a_npc_btsw2.h)

zeldaret/tww (d217e2b) keeps the GameCube offset names `mXXX` for these fields; its code uses them the same way, so the names come from the uses. `m_handL_jnt_num` / `m_handR_jnt_num` stay: tww names them so, CreateHeap loads them from the joints "handL" / "handR", and d_a_npc_btsw2.cpp has no assert text for them (unlike daNpc_Ac1_c). Open: `m714` (CreateInit copies current.angle, never read in WWHD or tww), `m724`, `m735` (only set to 0 in WWHD and tww).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x892 | `m71E` | `mTalking` | checkOrder 0221CFBC sets 1 on the talk command (and clears mOrderEvent); wait01 0221DC90 switches mMode to the talk (2) while it is set; talk01 0221DCB4 clears it when the box closes (tww the same); same code as daNpc_Ba1_c / daNpc_Km1_c::mTalking |
| 0x8A6 | `m736` | `mbTurnToPath` | talk01 0221DCB4 sets 1 when the talk ends while the actor walks (mWaitTimer 0); pathMove 0221D72C then stops (speedF 0) and turns to the next path point, clearing it and starting the walk animation (7) once within 0x10; CreateInit 0221CAEC 0 (tww the same) |
| 0x8AE | `m73E` | `mStopPointCount` | CreateInit 0221CAEC / pathMove 0221D72C (when a stop ends) set 1-3 (1 + cM_rndF(3)); pathMove counts it down at each reached path point and stops at the point where it is 0 (tww the same) |
| 0x8B2 | `m742` | `mWaitTimer` | CreateInit 0221CAEC 90-390; pathMove 0221D72C: while it is above 0 the actor stands (counts it down, walk animation at 0), set to HIO mWaitTimeBase + cM_rndF(mWaitTimeRange) at each stop; talk01 0221DCB4 tests it for 0 (tww the same) |
| 0x8B4 | `m744` | `mBtpIdx` | CreateHeap 0221C790 sets 0; tww initTexPatternAnm loads l_btp_ix_tbl[it] (WWHD's initTexPatternAnm 0221C68C always takes entry 0); after tww's use |
| 0x8B5 | `m745` | `mAnmIdx` | setAnm 0221D410 returns when the new animation equals it (or it is -1), stores it and loads l_bck_ix_tbl[it]; m_jnt.mbHeadLock while it is 7 / 8; anmAtr 0221D5FC / pathMove 0221D72C test it (tww the same); same role as daNpc_Mk_c::mAnmIdx |
| 0x8B6 | `m746` | `mOrderEvent` | wait01 0221DC90 sets 2, eventOrder 0221CFFC orders the speak event at 1 and sets dEvtCmd_INTALK_e at 1 / 2; checkOrder 0221CFBC clears it on the talk command; wait_action 0221DD2C clears it each frame (tww the same); same code as daNpc_Ba1_c / daNpc_Km1_c::mOrderEvent |
| 0x8B7 | `m747` | `mMode` | wait_action 0221DD2C sets 1 at the start and runs wait01 + pathMove at 1, talk01 at 2; wait01 0221DC90 sets 2 (talk), talk01 0221DCB4 1; setAnm 0221D410 picks the morf / speed / loop mode from a_*_tbl[it]; lookBack 0221DAD8 looks only at 1 / 2 (turns at 2) (tww the same) |
| HIO 0x2C | `m30` | `mPathTurnMaxStep` | pathMove 0221D72C: cLib_addCalcAngleS2's maxStep while turning to the next path point; daNpc_Btsw2_HIO_construct 0221DE30 600 (tww the same, 0x258) |
| HIO 0x2E | `m32` | `mPathTurnScale` | pathMove 0221D72C: cLib_addCalcAngleS2's scale while turning to the next path point; daNpc_Btsw2_HIO_construct 0221DE30 5 (tww the same) |
| HIO 0x30 | `m34` | `mWalkSpeed` | pathMove 0221D72C: speedF starts at it each frame; daNpc_Btsw2_HIO_construct 0221DE30 (tww 0.8f) (tww the same) |
| HIO 0x34 | `m38` | `mWalkDirSpeedRate` | pathMove 0221D72C: speedF += it * (direction to the point . facing); daNpc_Btsw2_HIO_construct 0221DE30 (tww 0.95f) (tww the same) |
| HIO 0x38 | `m3C` | `mWaitTimeBase` | pathMove 0221D72C: mWaitTimer = it + cM_rndF(mWaitTimeRange) at each stop; daNpc_Btsw2_HIO_construct 0221DE30 90 (tww the same) |
| HIO 0x3A | `m3E` | `mWaitTimeRange` | pathMove 0221D72C: the cM_rndF range added to mWaitTimeBase for mWaitTimer; daNpc_Btsw2_HIO_construct 0221DE30 90 (tww the same) |

## kg1_core::daNpc_Kg1_c (d_a_npc_kg1_core.h; views daNpc_Kg1_c in d_a_npc_kg1.h, Kg1Actor in d_a_npc_kg1_anim.h)

zeldaret/tww (d217e2b) has only stubs for this class; its header keeps the GameCube offset names `mXXX` (and calls 0x6C8, here 0x7E0, `mBtpAnm`: the prop model's pattern). The names come from our decomp's use. Open: `m70C` (only constructed), `m778` (wait_action 02266D60 writes 0 in both result branches; never read in WWHD, tww stub: no role).

Misnamed in the views (corrected in their own commit, each declaration says `was X (misnamed)`): d_a_npc_kg1.h `mpTexPattern` -> `m_eye_tex_pattern` (the core's assert text), `mBtpAnm` (0x86C) -> `mEyeBtpAnm` (the core's `mBtpAnm` is the prop pattern at 0x7E0, tww's 0x6C8), `mTalkActive` -> `mTalking`, `mEventOrder` -> `mOrderEvent`; d_a_npc_kg1_anim.h Kg1Actor `mTalkActive` -> `mTalking`, `mEventOrder` -> `mOrderEvent`. Kg1Actor `mTalkAnimationWeight` (0x7E4) was the prop pattern's frame `mBtpAnm.mFrameCtrl.mFrame` (setAnm 02265E24 sets 1.0 when animation 9's morf reaches frame 31, wait_action 02266D60 0.0 when the talk starts): the view now declares the core's `mDoExt_btpAnm mBtpAnm` at 0x7E0 and the two sites call `mBtpAnm.setFrame()` (own commit, objcmp identical).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x7DC | `m6C4` | `mpPropModel` | CreateHeap 02265978 creates it from object resource 6; daNpc_Kg1_nodeCallBack 0226559C puts it on joint 8; daNpc_Kg1Draw 02266B24 draws it with mBtpAnm while mSpecialAnimation is set; same name as the views daNpc_Kg1_c (d_a_npc_kg1.h) / Kg1Actor (d_a_npc_kg1_anim.h) |
| 0x86C | `m6F8` | `mEyeBtpAnm` | initTexPatternAnm 0226586C sets it up with m_eye_tex_pattern (the assert text "m_eye_tex_pattern != (0)" names that pattern); daNpc_Kg1Draw 02266B24 enters it at mBtpFrame |
| 0x954 | `m720` | `mBtpFrame` | initTexPatternAnm 0226586C 0; playTexPatternAnm 022665C8 steps it through m_eye_tex_pattern's frames (wraps and starts mBlinkTimer at the end); daNpc_Kg1Draw 02266B24 enters mEyeBtpAnm at it; same role as daNpc_Btsw2_c::mBtpFrame |
| 0x956 | `m72C` | `mBlinkTimer` | initTexPatternAnm 0226586C 0; playTexPatternAnm 022665C8 holds the eye frame while it counts down and sets 120 when the pattern wraps; same role as daNpc_Ba1_c::mBlinkTimer |
| 0x958 | `m72E` | `mLookAtMaxVel` | lookBack 0226676C: cLib_addCalcAngleS2 moves it toward mEventCut.mTurnSpeed (or the HIO mMaxHeadTurnVel) and passes it as the look-at speed; 0 for an immediate turn; CreateInit 022660C4 0; same code as daNpc_Ba1_c::mLookAtMaxVel |
| 0x95A | `m730` | `mTalking` | checkOrder 022663A4 sets 1 on the talk command while mOrderEvent is 1 / 2; CreateInit 022660C4 0; d_a_npc_kg1_dialogue.cpp tests 1 and clears it (through the view's mTalkActive); same code as daNpc_Ba1_c / daNpc_Btsw2_c::mTalking |
| 0x95B | `m731` | `mbAttention` | lookBack 0226676C stores chkAttention()'s result (0 out of range, 1 while the event cut looks); CreateInit 022660C4 0; same name as the view Kg1Actor (d_a_npc_kg1_anim.h) and daNpc_Ba1_c::mbAttention |
| 0x95C | `m732` | `mOrderEvent` | eventOrder 022664A4: 1 / 2 the talk (2 orders the speak event), 3-5 order mEventIds[it - 3]; checkOrder 022663A4 clears it when those events start / end; same code as daNpc_Ba1_c::mOrderEvent |
| 0x979 | `m74D` | `mRequestedAnimation` | CreateInit 022660C4 2; anmAtr 02266C34 sets it from the message animation table; next_msgStatus 022675BC 1 (game paid); lookBack 0226676C looks at the camera while it is 8; same name as the views daNpc_Kg1_c (d_a_npc_kg1.h) / Kg1Actor, whose setAnm code plays it |
| 0x97C | `m750` | `mbPlayMorf` | CreateInit 022660C4 sets 1; daNpc_Kg1Execute 02266990 plays mpMorf only while it is set; never cleared in WWHD |
| 0x97D | `m751` | `mbTalkCamera` | next_msgStatus 022675BC sets 1 when the player pays for the game; kg1_talk_camera 02266C90 drives the talk camera only while it is set; never cleared in WWHD |
| 0x9A5 | `m779` | `mbForceNextStatus` | wait_action 02266D60 sets 1 once the minigame result is decided (score / failure taken, mEvtCtrl.mHindFlag bit 0x2 cleared) and 0 when the reward message is ready; daNpc_Kg1Draw 02266B24 forces the A-button status to dActStts_NEXT_e (25, "Next") through mDoStatusForce while it is set; same name in the view Kg1Actor |

Open: Kg1AnimationParameter `field_0x02` (d_a_npc_kg1_anim.h; tww dLib_anm_prm_c field_0x02, never read in WWHD or tww).

## daNpc_Fa1_c and daNpc_Fa1_HIO_c / _HIO2_c / _HIO3_c (d_a_npc_fa1.h)

zeldaret/tww (d217e2b) has the full code of the recovery fairy and keeps the GameCube offset names `mXXX` (class) and `mNN` (HIO `hio_prm_c`, offsets relative to the GameCube HIO object); WWHD runs the same code, so the names come from the uses. HD HIO offsets are relative to the HD object (`mNo` at 0, the vtable moved to the end: GameCube - 4). The WWHD statics call HIO2 `l_areaHio*`, but only the bottle-baba modes read it. Open: `m6C4` and `m795` (padding, never used in WWHD or tww).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x880 | `m764` | `mpTargetActor` | init_bottle_baba_move 022347F4 searches Grandma (fpcNm_NPC_BA1_e) into it, init_bottle_baba_move2 02234778 the player; bottle_baba_move 02234894 / bottle_baba_move2 02234A60 head the fairy at it and turn back above it, baba_down 02234CA0 turns up below it; createInit 0223350C null (tww the same) |
| 0x884 | `m768` | `mPlayerOffset` | init_up1 02233154 sets it to current.pos minus the player's position; get_player_move 02234204 and bottle_appear_move 022344EC turn it round the player (radius mPlayerR), raise it and put current.pos at the player's position plus it; down 02235238 tests its y (tww the same) |
| 0x890 | `m774` | `mGroundNormal` | BGCheck 02233CD8 copies the ground polygon's normal into it (0, 1, 0 without ground); tww _draw passes it to dComIfGd_setSimpleShadow, a submission the WWHD _draw 02233B58 no longer makes |
| 0x8A6 | `m78A` | `mOrbitSpeed` | get_player_move 02234204 and bottle_appear_move 022344EC subtract it from current.angle.y each frame (the turn round the player) and raise it by the acceleration up to the limit; init_get_player_move 02233EA8 HIO mGetOrbitSpeed, init_up1 02233154 0 (tww the same) |
| 0x8A8 | `m78C` | `mOrbitSpeedMax` | bottle_appear_move 022344EC caps mOrbitSpeed at it; init_up1 02233154 0, init_down 022351A0 HIO mDownOrbitSpeedMax (tww the same) |
| 0x8AA | `m78E` | `mOrbitAccel` | bottle_appear_move 022344EC adds it to mOrbitSpeed each frame; init_up1 02233154 0, init_down 022351A0 HIO mDownOrbitAccel (tww the same) |
| 0x8B4 | `m798` | `mWanderTimer` | areaMove 02234FA4 / areaOutMove 0223509C count it down and, at 0, flip mWanderDir and restart it at a random 15-35 frames; init_areaMove 022332A0 / init_areaOutMove 02234F80 0; the bottle-baba inits 022347F4 / 02234778 store 1 / 0 that those modes never read (tww the same) |
| 0x8B6 | `m79A` | `mWanderDir` | areaMove 02234FA4 / areaOutMove 0223509C toggle it when mWanderTimer runs out and steer -0x2000 / +0x2000 off the course by it; init_areaMove 022332A0 / init_areaOutMove 02234F80 0 (tww the same) |
| 0x8B8 | `m79C` | `mBabaDownMaxSpeed` | init_bottle_baba_move 022347F4 / init_bottle_baba_move2 02234778 set it from the bottle-baba HIO; init_baba_down 022346FC makes it maxFallSpeed for the downward phase (tww the same) |
| 0x8BC | `m7A0` | `mBabaUpMaxSpeed` | init_bottle_baba_move 022347F4 / init_bottle_baba_move2 02234778 set it from the bottle-baba HIO; init_baba_up 02234C3C makes it maxFallSpeed for the upward phase (tww the same) |
| HIO 0x04 | `prm.m08` | `mNormalSpeed` | init_normal_move 022332E0 / normal_move 02234078 speedF (tww the same) |
| HIO 0x08 | `prm.m0C` | `mGetRiseSpeed` | get_player_move 02234204 adds it to mPlayerOffset.y each frame while the fairy circles the player (tww the same) |
| HIO 0x0C | `prm.m10` | `mMoveRadius` | createInit 0223350C / straight 02234E60 scale.x, the distance from home.pos past which straight turns back (tww the same) |
| HIO 0x10 | `prm.m14` | `mEscapeMaxSpeed` | init_escape_move 0223404C maxFallSpeed, the rise speed escape_move climbs to (tww the same) |
| HIO 0x14 | `prm.m18` | `mEscapeGravity` | init_escape_move 0223404C gravity, escape_move's upward acceleration (tww the same) |
| HIO 0x18 | `prm.m1C` | `mCatchHorizontalDist` | checkBinCatch 02233F40 bottle catch request (tww dComIfGp_att_CatchRequest horizontalDist) |
| HIO 0x1C | `prm.m20` | `mCatchUpDist` | checkBinCatch 02233F40 bottle catch request (tww dComIfGp_att_CatchRequest upDist) |
| HIO 0x20 | `prm.m24` | `mCatchDownDist` | checkBinCatch 02233F40 bottle catch request (tww dComIfGp_att_CatchRequest downDist) |
| HIO 0x24 | `prm.m28` | `mUp1SpeedY` | init_up1 02233154 speed.y, the bottle fairy's first rise (tww the same) |
| HIO 0x28 | `prm.m2C` | `mUp2MaxSpeed` | init_up2 022351DC maxFallSpeed (tww the same) |
| HIO 0x2C | `prm.m30` | `mUp1MaxSpeed` | init_up1 02233154 maxFallSpeed (tww the same) |
| HIO 0x30 | `prm.m34` | `mUp1Gravity` | init_up1 02233154 gravity (tww the same) |
| HIO 0x34 | `prm.m38` | `mDownEndY` | down 02235238 starts up2 once mPlayerOffset.y is below it (tww the same) |
| HIO 0x38 | `prm.m3C` | `mGetOrbitSpeed` | init_get_player_move 02233EA8 mOrbitSpeed (tww the same) |
| HIO 0x3A | `prm.m3E` | `mGetOrbitSpeedMax` | get_player_move 02234204 caps mOrbitSpeed at it (tww the same) |
| HIO 0x3C | `prm.m40` | `mGetOrbitAccel` | get_player_move 02234204 adds it to mOrbitSpeed each frame (tww the same) |
| HIO 0x3E | `prm.m42` | `mGetTimer` | init_get_player_move 02233EA8 mTimer; get_player_move 02234204 gives the recovery fairy and deletes at 0 (tww the same) |
| HIO 0x40 | `prm.m44` | `mNormalTimer` | init_normal_move 022332E0 mTimer = a random value from it (+0-60); normal_move 02234078 escapes at 0 when status bit 1 is set (tww the same) |
| HIO 0x42 | `prm.m46` | `mEscapeTimer` | init_escape_move 0223404C mTimer; escape_move 0223434C deletes the fairy at 0 (tww init_turn also reloads it; the WWHD init_turn 02234E14 does not) |
| HIO 0x44 | `prm.m48` | `mCatchAngle` | checkBinCatch 02233F40 bottle catch request (tww dComIfGp_att_CatchRequest angle) |
| HIO 0x46 | `prm.m4A` | `mBottleTimer` | init_up1 02233154 mTimer; bottle_appear_move 022344EC gives the item and deletes at 0 (tww the same) |
| HIO 0x48 | `prm.m4C` | `mDownOrbitSpeedMax` | init_down 022351A0 mOrbitSpeedMax (tww the same) |
| HIO 0x4A | `prm.m4E` | `mDownOrbitAccel` | init_down 022351A0 mOrbitAccel (tww the same) |
| HIO 0x4C | `m50` | `mBottleBabaHio` | the daNpc_Fa1_HIO2_c whose prm only the bottle-baba modes read (init_bottle_baba_wait 0223311C .. init_baba_down 022346FC); daNpc_Fa1_HIO_c__ct 02235350 constructs it (tww l_HIO.m50) |
| HIO 0x74 | `m78` | `mHoverHio` | the daNpc_Fa1_HIO3_c whose prm hover_move 0223447C and bigelf_change 02234D1C read; daNpc_Fa1_HIO_c__ct 02235350 constructs it (tww l_HIO.m78) |
| HIO2 0x04 | `prm.m08` | `mBabaSpeed` | init_bottle_baba_move 022347F4 / init_bottle_baba_move2 02234778 and every bottle_baba_move frame (Fa1_babaPosition) speedF (tww the same) |
| HIO2 0x08 | `prm.m0C` | `mBabaMoveUpMaxSpeed` | init_bottle_baba_move 022347F4 mBabaUpMaxSpeed (tww the same) |
| HIO2 0x0C | `prm.m10` | `mBabaMoveDownMaxSpeed` | init_bottle_baba_move 022347F4 mBabaDownMaxSpeed (tww the same) |
| HIO2 0x10 | `prm.m14` | `mBabaMove2UpMaxSpeed` | init_bottle_baba_move2 02234778 mBabaUpMaxSpeed (tww the same) |
| HIO2 0x14 | `prm.m18` | `mBabaMove2DownMaxSpeed` | init_bottle_baba_move2 02234778 mBabaDownMaxSpeed (tww the same) |
| HIO2 0x18 | `prm.m1C` | `mBabaGravity` | init_baba_down 022346FC gravity (tww the same) |
| HIO2 0x1C | `prm.m20` | `mBabaAngleOffset` | bottle_baba_move (Fa1_babaPosition) / bottle_baba_move2: current.angle.y = the angle to mpTargetActor plus it (tww the same) |
| HIO2 0x1E | `prm.m22` | `mBabaMoveTimer` | init_bottle_baba_move 022347F4 mTimer (tww the same) |
| HIO2 0x20 | `prm.m24` | `mBabaWaitTimer` | init_bottle_baba_wait 0223311C mTimer; bottle_baba_wait 022347E4 starts bottle_baba_move2 at 0 (tww the same) |
| HIO3 0x04 | `prm.m08` | `mHoverSpeedY` | init_hover_move 0223326C maxFallSpeed = -it; hover_move 0223447C passes it to position_move as the vertical speed (tww the same) |
| HIO3 0x08 | `prm.m0C` | `mHoverGravity` | init_hover_move 0223326C gravity (tww the same) |
| HIO3 0x0C | `prm.m10` | `mHoverHeight` | hover_move 0223447C passes it to position_move, which turns the vertical speed round this far above / below home.pos.y (tww the same) |
| HIO3 0x10 | `prm.m14` | `mBigelfSpeed` | init_bigelf_change 02234CB8 speedF (tww the same) |
| HIO3 0x14 | `prm.m18` | `mBigelfAngleOffset` | bigelf_change 02234D1C: current.angle.y = the angle to home.pos plus it (tww the same) |

## daNpc_kam_c (d_a_npc_kamome.h)

zeldaret/tww (d217e2b) has the full seagull code and keeps the GameCube offset names `mXXX` for these fields; the WWHD player-action code matches it, while the HD waitNpcAction 02260220 waits hidden at home and no longer runs tww's wander (so `mTurnTargetAngleY` / `mIdleAnmTimer` are only cleared in WWHD; their names come from tww's use). Open: the padding `mB92`, `mBD0`, `mC04`, `mC07`, `mC10`, `mC18` (never used in WWHD or tww). Not this class: `m4E4` in the daPy_npc_c inlines (d_a_player_npc.h) and the audio interface's `field_0x0062`.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0xBAC | `mC08` | `mActionTimer` | the current action's timer: setAction 0225DDF0 clears it on every action change; waitPlayerAction 02260510 sets 60-150 at the start and, counted to 0, plays the sing animation and restarts at 90-180; damagePlayerAction 022608A4 sets 30 and returns Link at 0 (tww the same; tww waitNpcAction also times its wander phases with it) |
| 0xBAE | `mC0A` | `mTurnTargetAngleY` | setAction 0225DDF0 / waitPlayerAction 02260510 clear it, never read in WWHD (the HD waitNpcAction 02260220 waits hidden and no longer wanders); after tww waitNpcAction, which picks it from l_turn_angleY and steers to it in the turn phase |
| 0xBB0 | `mC0C` | `mIdleAnmTimer` | setAction 0225DDF0 clears it, never read in WWHD (the HD waitNpcAction 02260220 no longer wanders); after tww waitNpcAction, which counts it down and plays a random idle animation (l_anm_type) at 0, restarting it at 10-90 |

## daNpc_Btsw_c (d_a_npc_btsw.h)

zeldaret/tww (d217e2b) has the full code of the letter-sorting NPC and keeps the GameCube `field_0x` names; WWHD runs the same code (the shop cursor and STControl drive the sorting game, not tww Rsh1's shop), so the names come from the uses, with the shared names of daNpc_Btsw2_c / daNpc_Ba1_c where the code is the same. Open: `field_0x950` (never used), `field_0x96E` (CreateInit 02218D58 copies current.angle, never read in WWHD or tww), `field_0x9B4` (CreateInit 0 only).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x7E8 | `field_0x6D0` | `mpLetterModel` | CreateHeap 022182F4 creates it from the letter model data; set_mtx 02218958 places it; _draw 02219840 lights it with mLetterTevStr and draws it (tww the same) |
| 0x7EC | `field_0x6D4` | `mLetterTevStr` | CreateInit 02218D58 dKy_tevstr_init; _draw 02219840 sets it up for the actor, overrides mColorK0 / mColorC0 and lights mpLetterModel with it (tww the same) |
| 0xA18 | `field_0x7E8` | `mBtpAnm` | initTexPatternAnm 02217C94 sets it up with m_head_tex_pattern; _draw 02219840 enters it at mBtpFrame and removes it (tww the same) |
| 0xA8C | `field_0x7FC` | `mBtpFrame` | initTexPatternAnm 02217C94 0; playTexPatternAnm 0221914C steps it through m_head_tex_pattern's frames once mBlinkTimer runs out; _draw 02219840 enters mBtpAnm at it (tww the same); same role as daNpc_Btsw2_c::mBtpFrame |
| 0xCC4 | `field_0x920` | `mpMails` | the three sorting-game letters (mSwMail0-2): shiwake_game_action 0221B3A8 inits, appears, throws and moves them; checkNextMailThrowOK 0221B300 tests the next one's proc; _draw 02219840 draws them while mbDrawMails is set (tww the same) |
| 0xCD0 | `field_0x92C` | `mMailIdx` | shiwake_game_action 0221B3A8 0 at the start, throws mpMails[it] and moves on to the next (0-2); checkNextMailThrowOK 0221B300 looks one past it (tww the same) |
| 0xCF6 | `field_0x952` | `mbTimerCreated` | shiwake_game_action 0221B3A8 creates the game timer (and starts the minigame) only while it is 0, then sets 1; 0 at the action's end and in CreateInit 02218D58 (tww the same) |
| 0xCF7 | `field_0x953` | `mbDrawMails` | shiwake_game_action 0221B3A8 sets 1 when the game starts and 0 at the action's end; _draw 02219840 draws mpMails only while it is set; CreateInit 02218D58 0 (tww the same) |
| 0xCF8 | `field_0x954` | `mBlinkTimer` | initTexPatternAnm 02217C94 0; playTexPatternAnm 0221914C holds the head pattern while it counts down and restarts it at 30-130 (tww the same); same role as daNpc_Ba1_c::mBlinkTimer |
| 0xD0E | `field_0x96A` | `mLookAtMaxVel` | lookBack 0221AA40: cLib_addCalcAngleS2 moves it toward the HIO mMaxHeadTurnVel while turning (else 0) and passes it as the look-at speed (tww the same); same code as daNpc_Ba1_c::mLookAtMaxVel |
| 0xD1C | `field_0x978` | `mStickControl` | the STControl: CreateInit 02218D58 / _execute 02219618 setWaitParm on it (tww shiwake_game_action reads the cursor triggers from it; WWHD reads the pad trigger words instead) |
| 0xD44 | `field_0x9A0` | `mCursorX` | shiwake_game_action 0221B3A8: the sorting cursor's column (-1..1), 0 at the start, moved left / right, picks the cursor and aim position and the box number (tww the same) |
| 0xD45 | `field_0x9A1` | `mCursorY` | shiwake_game_action 0221B3A8: the sorting cursor's row (0..1), 0 at the start, moved up / down (tww the same) |
| 0xD48 | `field_0x9A4` | `mNextTimerSeMs` | TimerCountDown 0221B254: plays the countdown sound once the game timer's rest time drops below it, then lowers it by 1000 (10000 above 10 s); shiwake_game_action 0221B3A8 20000 at the start (tww the same) |
| 0xD4D | `field_0x9A9` | `mTalking` | checkOrder 02219210 sets 1 on the talk command; wait01 0221ABCC switches mMode to the talk (2) while it is set; talk01 0221ACDC clears it when the talk ends (tww the same); same code as daNpc_Btsw2_c::mTalking |
| 0xD54 | `field_0x9B0` | `mPendingMsgNo` | shiwake_game_action 0221B3A8 stores the game-end message; getMsg 0221A840 returns it once instead of the normal message and resets it to fpcM_ERROR_PROCESS_ID_e; CreateInit 02218D58 the same (tww the same); same role as daNpc_Ds1_c::mPendingMsgNo |
| 0xD64 | `field_0x9C4` | `mBtpIdx` | CreateHeap 022182F4 0; tww initTexPatternAnm loads l_btp_ix_tbl[it] (WWHD's initTexPatternAnm 02217C94 always takes entry 0); same as daNpc_Btsw2_c::mBtpIdx |
| 0xD65 | `field_0x9C5` | `mAnmIdx` | setAnm 0221A3CC skips a repeat, stores the animation number and plays its table entries; anmAtr 0221A960 tests it (tww the same); same name as daNpc_Btsw2_c::mAnmIdx |
| 0xD66 | `field_0x9C6` | `mIdleLoopCount` | wait01 0221ABCC counts it down at each end of the wait animation and switches between the idle animations 9 / 10 at 0 (reloads 1 or 2-5); talk01 0221ACDC / wait_action 0221AF2C 1 (tww the same) |
| 0xD67 | `field_0x9C7` | `mOrderEvent` | eventOrder 022195A4: 1 / 2 the talk (1 orders the speak event), 3 GETMOTHERLETTER, 4 mEventIdx; checkOrder 02219210 clears it when those start (tww the same); same code as daNpc_Btsw2_c::mOrderEvent |
| 0xD68 | `field_0x9C8` | `mEventIdx` | shiwake_game_action 0221B3A8 BTSW_TALK_SUGOI's index (-1 otherwise); eventOrder 022195A4 orders it at mOrderEvent 4; dummy_event_action 0221B03C waits for its end; CreateInit 02218D58 -1 (tww the same) |
| 0xD6A | `field_0x9CA` | `mMode` | wait_action 0221AF2C runs wait01 at 1, talk01 at 2; wait01 0221ABCC sets 2, talk01 0221ACDC 1, shiwake_game_action 3; lookBack 0221AA40 looks at Link at 1 / 2 (tww the same); same code as daNpc_Btsw2_c::mMode |

### SwMail2_c, SwCam2_c and daNpc_Btsw_HIO_c (d_a_npc_btsw.h)

Same source (tww keeps `field_0x` names, same code). Camera: SwCam2_c 0x00 / 0x0C are `mCenter` / `mEye` and SwMail2_c 0x58 / 0x5C `mpCamCenter` / `mpCamEye`: tww's dCamera_c::Set is `Set(cXyz center, cXyz eye, f32 fovY, s16 bank)` (d_camera.cpp:4906, HD 02514F88) and Move 0221949C passes (0x00, 0x0C); 0x00 is loaded from camera_center_data, 0x0C from camera_eye. tww's accessor names `getEyeP()` (0x00) / `getCenterP()` (0x0C) are swapped; the accessors keep their names for now. Open: SwMail2_c `field_0x30` (ThrowInit's aim, then End's fall velocity) and `field_0x4E` (Appear / Wait angle offset, Throw's wobble phase): two roles each; HIO `field_0x32`, `field_0x34` (timer start / end arguments), `field_0x48`, `field_0x4C` (dTimer_createTimer float arguments), `field_0x50`, `field_0x54` (STControl setWaitParm floats), `field_0x58` (unused); these have no named parameter in tww.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| SwMail2_c 0x0C | `field_0x10` | `mMailBtpAnm` | MailCreateInit 022181EC sets it up with the letter's texture pattern; SwMail2_c::draw 022197C8 enters it at mBoxNo (tww the same) |
| SwMail2_c 0x80 | `field_0x24` | `mPos` | init 02217E74 starts it below the camera point; Appear 02219C1C / Wait 02219D88 / Throw 02219F20 / End 0221A1E0 move it; set_mtx 02217D98 / set_mtx_throw 022199FC place the model at it plus mPosOffset (tww the same) |
| SwMail2_c 0x98 | `field_0x3C` | `mPosOffset` | init 02217E74 zero; Throw 02219F20 raises its y in an arc while the letter flies, End 0221A1E0 eases it back to zero; set_mtx 02217D98 / set_mtx_throw 022199FC add it to mPos (tww the same) |
| SwMail2_c 0xA4 | `field_0x48` | `mAngle` | the letter model's rotation: set_mtx 02217D98 (ZXY) / set_mtx_throw 022199FC (X, Y) rotate by it; Appear / Wait / Throw / End turn it (tww the same) |
| SwMail2_c 0xB0 | `field_0x54` | `mBoxNo` | the box the letter belongs to (the texture frame): MailCreateInit 022181EC / init 02217E74 0, DummyInit 02218114 / AppearInit 02219B24 getNextNo; Throw 02219F20 scores when it equals mThrownBoxNo; draw 022197C8 enters mMailBtpAnm at it (tww the same) |
| SwMail2_c 0xB1 | `field_0x55` | `mThrownBoxNo` | ThrowInit 02219EA0 stores the box the cursor threw the letter at; Throw 02219F20 / End 0221A1E0 compare it with mBoxNo (right / wrong); init 02217E74 0 (tww the same) |
| SwMail2_c 0xB2 | `field_0x56` | `mThrowPhase` | ThrowInit 02219EA0 / init 02217E74 / AppearInit 02219B24 0; Throw 02219F20 moves the letter to the throw start while 0 and sets 1 to fly it to the box (tww the same) |
| SwMail2_c 0xB4 | `field_0x58` | `mpCamCenter` | MailCreateInit 022181EC stores &mSwCam.mCenter there (CreateHeap 022182F4 passes it first); Appear 02219C1C / Wait 02219D88 / Throw 02219F20 turn the letter by cLib_targetAngleY(it, mpCamEye), Appear / Wait place the letter 120 from mpCamEye towards it (tww the same; tww's accessor getEyeP() returns this point, its getEyeP / getCenterP names are swapped: dCamera_c::Set takes (center, eye)) |
| SwMail2_c 0xB8 | `field_0x5C` | `mpCamEye` | MailCreateInit 022181EC stores &mSwCam.mEye there (CreateHeap 022182F4 passes it second); init 02217E74 / DummyInit 02218114 / AppearInit 02219B24 start the letter 200 below it; Appear 02219C1C / Wait 02219D88 place the letter 120 from it towards mpCamCenter; Throw 02219F20 starts the throw from it (tww the same; tww's accessor getCenterP() returns this point, its getEyeP / getCenterP names are swapped: dCamera_c::Set takes (center, eye)) |
| SwCam2_c 0x00 | `field_0x00` | `mCenter` | the camera's centre: daNpc_Btsw_c_ct 022186EC / talk01 0221ACDC load it from camera_center_data[0][1]; SwCam2_c::Move 0221949C eases it to camera_center_data[mAimY][mAimX + 1] and passes it as dCamera_c::Set's first argument, the center (02514F88, tww d_camera.cpp Set(cXyz center, cXyz eye, ...)) (tww the same; tww's accessor getEyeP() returns it, its getEyeP / getCenterP names are swapped) |
| SwCam2_c 0x0C | `field_0x0C` | `mEye` | the camera's eye: daNpc_Btsw_c_ct 022186EC / talk01 0221ACDC load it from camera_eye; SwCam2_c::Move 0221949C passes it as dCamera_c::Set's second argument, the eye (02514F88, tww d_camera.cpp Set(cXyz center, cXyz eye, ...)) (tww the same; tww's accessor getCenterP() returns it, its getEyeP / getCenterP names are swapped) |
| SwCam2_c 0x18 | `field_0x18` | `mFovy` | talk01 0221ACDC 58.0; SwCam2_c::Move 0221949C passes it to the camera Set as the field of view (tww the same) |
| SwCam2_c 0x1C | `field_0x1C` | `mAimX` | shiwake_game_action 0221B3A8 sets it to mCursorX (tww setAimIdx); SwCam2_c::Move 0221949C picks the camera_center_data column + 1 by it; talk01 0221ACDC 0 |
| SwCam2_c 0x1D | `field_0x1D` | `mAimY` | shiwake_game_action 0221B3A8 sets it to mCursorY (tww setAimIdx); SwCam2_c::Move 0221949C picks the camera_center_data row by it; talk01 0221ACDC 0 |
| SwCam2_c 0x1E | `field_0x1E` | `mbActive` | talk01 0221ACDC sets 1 (tww ActiveOn) at the game-start message; shiwake_game_action 0221B3A8 0 at the end (ActiveOff); SwCam2_c::Move 0221949C drives the camera only while it is set (tww the same) |
| HIO 0x2C | `field_0x30` | `mTimerLimit` | shiwake_game_action 0221B3A8 creates the game timer with it (dTimer_createTimer's u16 time; HIO default 30, tww the same) |
| HIO 0x40 | `field_0x44` | `mMailThrowSpeed` | SwMail2_c::Throw 02219F20: cLib_addCalcPos maxStep of the flying letter (tww the same); default 40.0 |
| HIO 0x2E | `field_0x32` | `mTimerStartDelay` | shiwake_game_action 02210F60: dComIfG_TimerStart(6, it): dTimer_c::start() at 0, else dTimer_c::start(s16) (our d_timer.cpp startDelay(s16 delay)); daNpc_Bmsw_HIO_c_ct 02211C80 sets the default, never written elsewhere in WWHD or tww (0xF) |
| HIO 0x30 | `field_0x34` | `mTimerEndDelay` | shiwake_game_action 02210F60: dTimer_c::end(it) when the sorting game ends (our d_timer.cpp end(s32 delay)); daNpc_Bmsw_HIO_c_ct 02211C80 sets the default, never written elsewhere in WWHD or tww (0x3C) |
| HIO 0x44 | `field_0x48` | `mTimerPosY` | shiwake_game_action 02210F60: the y argument of dTimer_createTimer(6, mTimerLimit, 3, 4, 221, it, 32, mRupeePosY) (our d_timer.cpp x, y, rx, ry); daNpc_Bmsw_HIO_c_ct 02211C80 sets the default, never written elsewhere in WWHD or tww (80) |
| HIO 0x48 | `field_0x4C` | `mRupeePosY` | shiwake_game_action 02210F60: the ry argument of dTimer_createTimer (the rupee counter position, as dTimer_createStockTimer's l_stockRupeeY); daNpc_Bmsw_HIO_c_ct 02211C80 sets the default, never written elsewhere in WWHD or tww (50) |
| HIO 0x58 | `field_0x5C` | `mAnm56MorfFrames` | setAnm 0221A3CC morphs with it instead of a_morf_frame_tbl when the current animation is 5 or 6 (tww the same); default 10.0 |

## daNpc_Bmsw_c (d_a_npc_bmsw.h)

zeldaret/tww (d217e2b) has the full code of Koboli, the Rito mail sorter, and keeps the GameCube `field_0x` names; WWHD runs the same code as daNpc_Btsw_c (the same sorting game with the shop cursor and STControl), so the names come from the uses, with daNpc_Btsw_c's names where the code is the same. `mArmLMtx` / `mArmRMtx` after tww's accessors getArmLMtx() / getArmRMtx(); `mGameGetRupee` after tww's setGameGetRupee(). Open: `field_0x97E` (CreateInit 0220E6E8 copies current.angle, never read in WWHD or tww), `field_0x9C4` (CreateInit 0 only); same as daNpc_Btsw_c's open fields.

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x7EC | `field_0x6D4` | `mpHeadModel` | CreateHeap 0220D98C creates it from the BMHEAD11 head model data; set_mtx 0220E340 places it at the head joint; initTexPatternAnm 0220D320 sets up mBtpAnm on its model data; _draw 0220F06C lights it, enters mBtpAnm and draws it (tww the same) |
| 0x7F0 | `field_0x6D8` | `mpBagModel` | CreateHeap 0220D98C creates it from the BM_BAG model data; set_mtx 0220E340 places it at the backbone joint; _draw 0220F06C lights and draws it (tww the same) |
| 0x7F4 | `field_0x6DC` | `mpLetterModel` | CreateHeap 0220D98C creates it from the BM_LETTER model data; set_mtx 0220E340 puts it in the right or left hand during animations 5 / 6; _draw 0220F06C lights it with mLetterTevStr and draws it then (tww the same); same role as daNpc_Btsw_c::mpLetterModel |
| 0x7F8 | `field_0x6E0` | `mLetterTevStr` | HD 0x1C8; daNpc_Bmsw_c_ct 0220E0D4 constructs it; CreateInit 0220E6E8 dKy_tevstr_init; _draw 0220F06C sets it up for the actor during animations 5 / 6, overrides mColorK0 / mColorC0 from the HIO and lights mpLetterModel with it (tww the same); same role as daNpc_Btsw_c::mLetterTevStr |
| 0x9C4 | `field_0x794` | `mArmLMtx` | after tww getArmLMtx(): nodeCallBack 0220CCD4 copies the body's armL joint matrix into it; nodeCallBackArm 0220D134 gives the arm model's armLloc joint that matrix |
| 0x9F4 | `field_0x7C4` | `mArmRMtx` | after tww getArmRMtx(): nodeCallBack 0220CCD4 copies the body's armR joint matrix into it; nodeCallBackArm 0220D134 gives the arm model's armRloc joint that matrix |
| 0xA28 | `field_0x7F8` | `mBtpAnm` | mDoExt_btpAnm (HD 0x74); daNpc_Bmsw_c_ct 0220E0D4 constructs it; initTexPatternAnm 0220D320 sets it up with m_head_tex_pattern; _draw 0220F06C enters it at mBtpFrame (tww the same); same as daNpc_Btsw_c::mBtpAnm |
| 0xA9C | `field_0x80C` | `mBtpFrame` | initTexPatternAnm 0220D320 0; playTexPatternAnm 0220EC00 steps it through m_head_tex_pattern's frames once mBlinkTimer runs out; _draw 0220F06C enters mBtpAnm at it (tww the same); same role as daNpc_Btsw_c::mBtpFrame |
| 0xCD4 | `field_0x930` | `mpMails` | the three sorting-game letters (mSwMail0-2): CreateHeap 0220D98C points them at mSwMail0-2 and MailCreateInits them; shiwake_game_action 02210F60 inits, appears, throws and moves them; checkNextMailThrowOK 02210DCC tests the next one's proc; _draw 0220F06C draws them while mbDrawMails is set (tww the same); same as daNpc_Btsw_c::mpMails |
| 0xCE0 | `field_0x93C` | `mMailIdx` | CreateHeap 0220D98C / shiwake_game_action 02210F60 0 at the start; shiwake_game_action throws mpMails[it] and moves on to the next (0-2); checkNextMailThrowOK 02210DCC looks one past it (tww the same); same as daNpc_Btsw_c::mMailIdx |
| 0xD04 | `field_0x960` | `mGameGetRupee` | after tww setGameGetRupee(): setGameGetRupee 02210E74 stores the sorting game's rupee reward (by the round in event register 0xC203) and shows it as the message count; talk01 02210804 adds it to the rupee count at the reward messages (tww the same) |
| 0xD06 | `field_0x962` | `mbTimerCreated` | shiwake_game_action 02210F60 creates the game timer (and starts the minigame) only while it is 0, then sets 1; 0 at the action's end and in CreateInit 0220E6E8 (tww the same); same as daNpc_Btsw_c::mbTimerCreated |
| 0xD07 | `field_0x963` | `mbDrawMails` | shiwake_game_action 02210F60 sets 1 when the game starts and 0 at the action's end; _draw 0220F06C draws mpMails only while it is set; CreateInit 0220E6E8 0 (tww the same); same as daNpc_Btsw_c::mbDrawMails |
| 0xD08 | `field_0x964` | `mBlinkTimer` | initTexPatternAnm 0220D320 0; playTexPatternAnm 0220EC00 holds the head pattern while it counts down and restarts it at 30-130 (tww the same); same role as daNpc_Btsw_c::mBlinkTimer |
| 0xD1E | `field_0x97A` | `mLookAtMaxVel` | lookBack 022104A4: cLib_addCalcAngleS2 moves it toward the HIO mMaxHeadTurnVel while turning (else 0) and passes it as the look-at speed (tww the same); same code as daNpc_Btsw_c::mLookAtMaxVel |
| 0xD2C | `field_0x988` | `mStickControl` | the STControl: daNpc_Bmsw_c_ct 0220E0D4 constructs and inits it, daNpc_Bmsw_c_dt 02211F5C destroys it; CreateInit 0220E6E8 / _execute 0220EE44 setWaitParm on it (tww shiwake_game_action reads the cursor triggers from it; WWHD reads the pad trigger words instead); same as daNpc_Btsw_c::mStickControl |
| 0xD54 | `field_0x9B0` | `mCursorX` | shiwake_game_action 02210F60: the sorting cursor's column (-1..1), 0 at the start, moved left / right, picks the cursor and aim position, the box number and mSwCam.mAimX (tww the same); same as daNpc_Btsw_c::mCursorX |
| 0xD55 | `field_0x9B1` | `mCursorY` | shiwake_game_action 02210F60: the sorting cursor's row (0..1), 0 at the start, moved up / down, picks the cursor and aim position and mSwCam.mAimY (tww the same); same as daNpc_Btsw_c::mCursorY |
| 0xD58 | `field_0x9B4` | `mNextTimerSeMs` | TimerCountDown 02210D20: plays the countdown sound once the game timer's rest time drops below it, then lowers it by 1000 (10000 above 10 s); shiwake_game_action 02210F60 20000 at the start (tww the same); same as daNpc_Btsw_c::mNextTimerSeMs |
| 0xD5D | `field_0x9B9` | `mTalking` | checkOrder 0220ECC4 sets 1 on the talk command; wait01 0221068C switches mMode to the talk (2) while it is set; talk01 02210804 clears it when the talk ends (tww the same); same code as daNpc_Btsw_c::mTalking |
| 0xD64 | `field_0x9C0` | `mPendingMsgNo` | shiwake_game_action 02210F60 stores the game-end message (daNpc_Bmsw_getGameEndMsg); getMsg 022102CC returns it once instead of the normal message and resets it to fpcM_ERROR_PROCESS_ID_e; CreateInit 0220E6E8 the same (tww the same); same as daNpc_Btsw_c::mPendingMsgNo |
| 0xD74 | `field_0x9D4` | `mBtpIdx` | CreateHeap 0220D98C 0; initTexPatternAnm 0220D320 loads the head pattern l_btp_ix_tbl[it] (tww the same); same as daNpc_Btsw_c::mBtpIdx |
| 0xD75 | `field_0x9D5` | `mAnmIdx` | setAnm 0220FC70 skips a repeat, stores the animation number and plays l_bck_ix_tbl / l_arm_bck_ix_tbl[it]; set_mtx 0220E340 / _draw 0220F06C / wait01 0221068C test it for the sorting animations 5 / 6 (tww the same); same name as daNpc_Btsw_c::mAnmIdx |
| 0xD76 | `field_0x9D6` | `mIdleLoopCount` | wait01 0221068C counts it down at each end of the wait animation and switches between the animations 5 / 6 at 0 (reloads 1 or 2-5); talk01 02210804 / wait_action 02210AC0 1 (tww the same); same as daNpc_Btsw_c::mIdleLoopCount |
| 0xD77 | `field_0x9D7` | `mOrderEvent` | eventOrder 0220EE0C: 1 / 2 the talk (1 orders the speak event); checkOrder 0220ECC4 clears it when the talk starts; wait01 0221068C 2 while Link is in range; shiwake_game_action 02210F60 1 after the game (tww the same); same code as daNpc_Btsw_c::mOrderEvent |
| 0xD78 | `field_0x9D8` | `mEventIdx` | shiwake_game_action 02210F60 stores the SHIWAKEGAME / SHIWAKEGAME2 event index, orders it (fopAcM_orderOtherEventId) and waits for its end (tww the same); same name as daNpc_Btsw_c::mEventIdx |
| 0xD7A | `field_0x9DA` | `mMode` | wait_action 02210AC0 runs wait01 at 1, talk01 at 2; wait01 0221068C sets 2, talk01 02210804 / wait_action 1, shiwake_game_action 02210F60 3; lookBack 022104A4 looks at Link at 1 / 2; setAnm 0220FC70 also indexes its play tables by it (tww the same); same code as daNpc_Btsw_c::mMode |
| 0xD80 | `field_0x9E0` | `mBtswId` | CreateInit 0220E6E8: the process id of the Btsw child it creates (fopAcM_createChild "Btsw") once event bit 0x2701 is set, else fpcM_ERROR_PROCESS_ID_e; lookBack 022104A4 looks at that actor's eyePos (fopAcM_SearchByID) when Link is not in range (tww the same) |

### SwMail_c, SwCam_c and daNpc_Bmsw_HIO_c (d_a_npc_bmsw.h)

Same source and the same names as SwMail2_c / SwCam2_c / daNpc_Btsw_HIO_c. Camera: tww names SwCam_c's 0x00 / 0x0C `mCenter` / `mEye` (kept), its accessors getCenterP() / getEyeP() return them the right way round, and CreateHeap 0220D98C passes MailCreateInit(&mCenter, &mEye); SwCam_c::Move 0220ED04 passes them to dCamera_c::Set (02514F88) as (center, eye), so SwMail_c 0x58 / 0x5C are `mpCamCenter` / `mpCamEye`, the same as for the btsw classes. Open: SwMail_c `field_0x30` and `field_0x4E` (two roles each, as in SwMail2_c); HIO `field_0x32`, `field_0x34`, `field_0x48`, `field_0x4C`, `field_0x50`, `field_0x54` (timer and setWaitParm arguments without a named parameter in tww, as in daNpc_Btsw_HIO_c).

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| SwMail_c 0x0C | `field_0x10` | `mMailBtpAnm` | mDoExt_btpAnm (HD 0x74); daNpc_Bmsw_c_ct 0220E0D4 constructs it; MailCreateInit 0220D884 sets it up with the letter's texture pattern; SwMail_c::draw 0220EFF4 enters it at mBoxNo (tww the same); same as SwMail2_c::mMailBtpAnm |
| SwMail_c 0x80 | `field_0x24` | `mPos` | init 0220D50C / DummyInit 0220D7AC / AppearInit 0220F3C8 start it below mpCamEye; Appear 0220F4C0 / Wait 0220F62C / Throw 0220F7C4 / End 0220FA84 move it; set_mtx 0220D430 / set_mtx_throw 0220F2A0 place the model at it plus mPosOffset; SeDelete 0220EB68 (tww the same); same as SwMail2_c::mPos |
| SwMail_c 0x98 | `field_0x3C` | `mPosOffset` | init 0220D50C / DummyInit 0220D7AC / AppearInit 0220F3C8 zero; Throw 0220F7C4 raises its y in an arc while the letter flies, End 0220FA84 eases it back to zero; set_mtx 0220D430 / set_mtx_throw 0220F2A0 add it to mPos (tww the same); same as SwMail2_c::mPosOffset |
| SwMail_c 0xA4 | `field_0x48` | `mAngle` | the letter model's rotation: set_mtx 0220D430 (ZXY) / set_mtx_throw 0220F2A0 (X, Y) rotate by it; DummyInit / AppearInit / Appear / Wait / Throw / End turn it (tww the same); same as SwMail2_c::mAngle |
| SwMail_c 0xB0 | `field_0x54` | `mBoxNo` | the box the letter belongs to (the texture frame): MailCreateInit 0220D884 / init 0220D50C 0, DummyInit 0220D7AC / AppearInit 0220F3C8 getNextNo; Throw 0220F7C4 scores when it equals mThrownBoxNo; draw 0220EFF4 enters mMailBtpAnm at it (tww the same); same as SwMail2_c::mBoxNo |
| SwMail_c 0xB1 | `field_0x55` | `mThrownBoxNo` | ThrowInit 0220F744 stores the box the cursor threw the letter at; Throw 0220F7C4 / End 0220FA84 compare it with mBoxNo (right / wrong); init 0220D50C 0 (tww the same); same as SwMail2_c::mThrownBoxNo |
| SwMail_c 0xB2 | `field_0x56` | `mThrowPhase` | ThrowInit 0220F744 / init 0220D50C / AppearInit 0220F3C8 0; Throw 0220F7C4 moves the letter to the throw start while 0 and sets 1 to fly it to the box (tww the same); same as SwMail2_c::mThrowPhase |
| SwMail_c 0xB3 | `field_0x57` | `_B3` | padding: tww declares it as field_0x57[0x58 - 0x57]; never used in WWHD or tww; same as SwMail2_c::_B3 |
| SwMail_c 0xB4 | `field_0x58` | `mpCamCenter` | MailCreateInit 0220D884 stores its first argument there, &mSwCam.mCenter (CreateHeap 0220D98C); Appear 0220F4C0 / Wait 0220F62C / Throw 0220F7C4 turn the letter by cLib_targetAngleY(it, mpCamEye), Appear / Wait place the letter 120 from mpCamEye towards it (tww the same, MailCreateInit(getCenterP(), getEyeP())); same as SwMail2_c::mpCamCenter (camera, see SwMail2_c) |
| SwMail_c 0xB8 | `field_0x5C` | `mpCamEye` | MailCreateInit 0220D884 stores its second argument there, &mSwCam.mEye (CreateHeap 0220D98C); init 0220D50C / DummyInit 0220D7AC / AppearInit 0220F3C8 start the letter 200 below it; Appear 0220F4C0 / Wait 0220F62C place the letter 120 from it towards mpCamCenter; Throw 0220F7C4 starts the throw from it (tww the same); same as SwMail2_c::mpCamEye (camera, see SwMail2_c) |
| SwCam_c 0x1C | `field_0x1C` | `mAimX` | shiwake_game_action 02210F60 sets it to mCursorX (tww setAimIdx); SwCam_c::Move 0220ED04 picks the camera_center_data column + 1 by it; daNpc_Bmsw_c_ct 0220E0D4 / talk01 02210804 0; same as SwCam2_c::mAimX |
| SwCam_c 0x1D | `field_0x1D` | `mAimY` | shiwake_game_action 02210F60 sets it to mCursorY (tww setAimIdx); SwCam_c::Move 0220ED04 picks the camera_center_data row by it; daNpc_Bmsw_c_ct 0220E0D4 / talk01 02210804 0; same as SwCam2_c::mAimY |
| HIO 0x2C | `field_0x30` | `mTimerLimit` | shiwake_game_action 02210F60 creates the game timer with it (dTimer_createTimer's u16 time); daNpc_Bmsw_HIO_c_ct 02211C80 30 (tww the same); same as daNpc_Btsw_HIO_c::mTimerLimit |
| HIO 0x40 | `field_0x44` | `mMailThrowSpeed` | SwMail_c::Throw 0220F7C4: cLib_addCalcPos maxStep of the flying letter; daNpc_Bmsw_HIO_c_ct 02211C80 40.0 (tww the same); same as daNpc_Btsw_HIO_c::mMailThrowSpeed |

## daNpc_Co1_c (d_a_npc_co1.h)

zeldaret/tww (d217e2b) has code for this class with the GameCube offset names. Named so far:

| Struct / offset | Old | New | Evidence |
|---|---|---|---|
| HIO 0x08 | `field_0x8` | `mRefCount` | daNpc_Co1_HIO_c_ct 0222A73C -1; never read in WWHD or tww; after our decomp's daNpc_Pm1_HIO_c / daNpc_Ba1_HIO_c mRefCount (the same HIO layout) |

Open (declared only, no use in WWHD or tww): `m9CE`, `m9D0`, `m9DC`, `m9DE`, `m9E2`, `m9E4`; anm_prm_c `field_0x02` (as in daNpc_Ba1_c).

## daNpcMn_c (d_a_npc_mn.h)

Open (declared only, no use in WWHD or tww): `field_0x6C4[0x60]`, `field_0x75C`, `field_0x795`, `field_0x7A6`, `field_0x7BC`; NpcDatStruct `field_0x12`, `field_0x2E`, `field_0x3C`, `field_0x3E`.

## daNpc_Hr_c (d_a_npc_hr.h)

Open: `field_0x60A` (HD 0x786; only written, 5, in WWHD and tww, never read).

## Views of NPC layouts in other files

| View | File | Field | Class field |
|---|---|---|---|
| `KmonOrcaHD` | d_a_kmon.h | `mPrevAction`, `mEventReturnAction`, `mActionPhase`, `mEventNo` | daNpc_Ji1_c |
| `MedliView` | d_a_tag_md_cb.h | `mStatus` (was `m30F0`) | daNpc_Md_c |

## daNpc_Ji1_c (d_a_npc_ji1.h)

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x3D4 | `field_0x2BC` | `mPrevAction` | setAction (inline, e.g. in normalAction 02250A1C) copies mAction here before switching; talkAction 0225268C / speakAction 02252FD0 / speakBadAction 02254A64 switch back to it |
| 0x3DC | `field_0x2C8` | `mEventReturnAction` | normalSubActionGuard 022504E4 (guard event) copies mAction here before eventAction; eventAction 022596CC switches back to it; evn_talk_init 022583A4 tests it |
| 0x42C | `field_0x320` | `mSmokePos` | battleSubActionTateAttack 0225B85C copies the spear tip (y 0) / YokoAttack 0225BBA4 sets it 200 around the body; position of the ID_AK_JT_ELEMENTSMOKE00 emitter (0225B720) |
| 0x438 | `field_0x32C` | `mSmokeAngle` | battleSubActionYokoAttack 0225BBA4: steps -0x1400 per frame, mSmokePos = pos + 200 * (sin, cos) of it |
| 0x658 | `field_0x430` | `mpTearEmitter` | setAnm 0224DFF0 sets dPa_ID_AK_SN_JITEAR00 at current.pos when NULL; setAnimFromMsgNo 02251A8C ends it (becomeInvalidEmitter) |
| 0x860 | `field_0x638` | `mStts` | CreateInit 0224C650: Init(0xFF, 0xFF, this), status of mCyl, mAtCyl and mAtCps; _execute 0224B268 moves by its m_cc_move (fopAcM_posMoveF) |
| 0x89C | `field_0x674` | `mGuardStts` | CreateInit 0224C650: Init(0xFF, 0xFF, this), status of mGuardCyl |
| 0x8D8 | `field_0x6B0` | `mCyl` | CreateInit 0224C650: Set(l_cyl_src), mStts; _execute 0224B268 SetC(current.pos) every frame |
| 0xA08 | `field_0x7E0` | `mGuardCyl` | CreateInit 0224C650: Set(l_cyl2_src); battleSubActionNockBack 0225C208 OnTgShield/ChkTgShieldHit; setGuardParticle 02248D3C spark at its TG hit position |
| 0xB38 | `field_0x910` | `mAtCyl` | CreateInit 0224C650: Set(l_cylAt_src); MoveToPlayer 0224F628 SetC(current.pos) and sets it at the attack's last frames |
| 0xC68 | `field_0xA40` | `mAtCps` | CreateInit 0224C650: Set(l_cpsAt_src) from mSpearButtPos to mSpearTipPos; battleAtSet 0225B4AC sets radius and start/end (mSpearTipOldPos -> mSpearTipPos) |
| 0xDA0 | `field_0xB78` | `mSpearTipPos` | set_mtx 0224AEF0: the spear joint matrix times (0, 0, 100) (70 for the side attack) |
| 0xDAC | `field_0xB84` | `mSpearButtPos` | set_mtx 0224AEF0: the spear joint matrix times (0, 0, -100); _delete 0224C550 deletes its sound object |
| 0xDB8 | `field_0xB90` | `mSpearTipOldPos` | set_mtx 0224AEF0 copies mSpearTipPos here before updating it; battleAtSet 0225B4AC uses it as the capsule start |
| 0xDCA | `field_0xBA2` | `mArmShakeAmp` | battleSubActionNockBack 0225C208 sets t * 0x100; nodeCallBack3 022494C8 X-rotates both arm joints by it * sin(mArmShakePhase) |
| 0xDCC | `field_0xBA4` | `mArmShakePhase` | battleSubActionNockBack 0225C208 sets t * 0x2800; nodeCallBack3 022494C8: sin phase of the arm shake |
| 0xDCE | `field_0xBA6` | `mHeadPitch` | setHairAngle (inlined in _execute 0224B268): atan2 of the head front vector's y against its xz length |
| 0xDD0 | `field_0xBA8` | `mHeadYaw` | setHairAngle (inlined in _execute 0224B268): atan2(x, z) of the head front vector |
| 0xDD2 | `field_0xBAA` | `mHair1AngleZ` | setHairAngle (in _execute 0224B268) eases and clamps it (HIO limits); nodeCallBack2 02249224 Z-rotates the hair1 joint by it |
| 0xDD4 | `field_0xBAC` | `mHair1AngleY` | setHairAngle (in _execute 0224B268) eases and clamps it; nodeCallBack2 02249224 Y-rotates the hair1 joint by it |
| 0xDD6 | `field_0xBAE` | `mHair2AngleZ` | setHairAngle (in _execute 0224B268); nodeCallBack2 02249224 Z-rotates the hair2 joint by it |
| 0xDD8 | `field_0xBB0` | `mHair2AngleY` | setHairAngle (in _execute 0224B268); nodeCallBack2 02249224 Y-rotates the hair2 joint by it |
| 0xDDA | `field_0xBB2` | `mHair3AngleZ` | setHairAngle (in _execute 0224B268); nodeCallBack2 02249224 Z-rotates the hair3 joint by it |
| 0xDDC | `field_0xBB4` | `mHair3AngleY` | setHairAngle (in _execute 0224B268); nodeCallBack2 02249224 Y-rotates the hair3 joint by it |
| 0xDDE | `field_0xBB6` | `mHair1SpeedZ` | setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair1AngleZ, added next frame |
| 0xDE0 | `field_0xBB8` | `mHair1SpeedY` | setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair1AngleY, added next frame |
| 0xDE2 | `field_0xBBA` | `mHair2SpeedZ` | setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair2AngleZ, added next frame |
| 0xDE4 | `field_0xBBC` | `mHair2SpeedY` | setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair2AngleY, added next frame |
| 0xDE6 | `field_0xBBE` | `mHair3SpeedZ` | setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair3AngleZ, added next frame |
| 0xDE8 | `field_0xBC0` | `mHair3SpeedY` | setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair3AngleY, added next frame |
| 0xDEC | `field_0xBC4` | `mHair1LastPos` | setHairAngle (in _execute 0224B268): the hair1 joint position, kept for the next frame (its motion drives the hair) |
| 0xDF8 | `field_0xBD0` | `mHairWindPhase` | setHairAngle (in _execute 0224B268): advanced by 1500 + 4600 * wind each frame; cos phase of the wind sway |
| 0xDFA | `field_0xBD2` | `mHair1WindAngle` | setHairAngle (in _execute 0224B268): wind * 2280 * cos(mHairWindPhase); nodeCallBack2 02249224 adds it to the hair1 Z angle |
| 0xDFC | `field_0xBD4` | `mHair2WindAngle` | setHairAngle (in _execute 0224B268): wind * 3908 * cos(...); nodeCallBack2 02249224 adds it to the hair2 Z angle |
| 0xDFE | `field_0xBD6` | `mHair3WindAngle` | setHairAngle (in _execute 0224B268): wind * 7568 * cos(...); nodeCallBack2 02249224 adds it to the hair3 Z angle |
| 0xE00 | `field_0xBD8` | `mBackbonePos` | nodeCallBack1 02249084: the backbone joint matrix times l_hio's three offsets (HIO 0x0C0) |
| 0xE48 | `field_0xC24` | `mHitCutType` | daJi1_TgHitCallback 02248CF0 stores the player's cut type; teachAction 02255908 / teachSPRollCutAction 02256F1C / battleSubActionGuard 0225C7EC switch on it; HD: pad_0xC20 gone |
| 0xE4C | `field_0xC28` | `mCoHit` | daJi1_CoHitCallback 02248CA4 sets 1; _execute 0224B268 clears it every frame |
| 0xE50 | `field_0xC2C` | `mAttackEndWait` | MoveToPlayer 0224F628 sets 0x14 at the attack animation's end, then counts it down returning TRUE while > 0 |
| 0xE54 | `field_0xC30` | `mBattleTimer` | battleGameSetTimer 02259A6C sets a random time; battleSubActionWait 0225B344 counts it down and acts at < 0; battleAction 0225AEC8 sets 30 + rnd(150) |
| 0xE58 | `field_0xC34` | `mNockBackTimer` | the attack inits (0225B2B8, 0225B22C, 0225B1A0) clear it; battleSubActionNockBack 0225C208 counts it and scales the arm shake by it |
| 0xE60 | `field_0xC3C` | `mAtHitCount` | daJi1_AtHitCallback 02248EA0 adds 1; battleAction 0225AEC8 clears it and shows 3 - it as life; battleGuardCheck 0225A534 ends the game when > 2 |
| 0xE68 | `field_0xC40` | `mBgCorrectVec` | _execute 0224B268 copies the BG correction difference; calcBgCorrectValue 0224F59C dots it with the facing direction |
| 0xE74 | `field_0xC4C` | `mCoCorrectSum` | calcCoCorrectValue 0224F528 adds mStts' m_cc_move along the facing direction each call; cleared by CreateInit 0224C650 / MoveToPlayer 0224F628 |
| 0xE78 | `field_0xC50` | `mBgCorrectSum` | calcBgCorrectValue 0224F59C adds mBgCorrectVec along the facing direction each call; cleared by CreateInit 0224C650 / MoveToPlayer 0224F628 |
| 0xEA0 | `field_0xC78` | `mActionPhase` | setAction (inline) sets -1 before calling the old action, 0 for the new one; every action (e.g. battleAction 0225AEC8, eventAction 022596CC) inits at 0 and adds 1 |
| 0xEAC | `field_0xC84` | `mEventNo` | eventAction 022596CC orders mEventIdx[it], then sets 0x12 (none); _create 0224D280 sets 0x12; the guard event (022504E4) / kaitenwaitAction 02250D54 set the event to play |
| 0xEB0 | `field_0xC88` | `mLookMaxVel` | lookBack 0224ABA4 eases it to HIO 0x18 while m_jnt turns, else clears it; passed as lookAtTarget's maxVel (TWW d_npc.h parameter name) |
| 0xEB4 | `field_0xC8C` | `mTeachCutType` | teachAction 02255908 / teachSPRollCutAction 02256F1C copy mHitCutType here and test it (CUT_TYPE_CUT_ROLL) |
| 0xEB8 | `field_0xC90` | `mTeachResult` | the result of a lesson cut: teachAction 02255908 sets 1 for a right cut with hits left (orders event 1, then speakAction), 2 for a wrong cut (event 3, speakBadAction), 3 for the last needed cut (event 1 / 2 / 15, then speakAction or endspeakAction), and copies mRollTeachResult into it after a spin attack; speakAction 02252FD0 picks the reply message by it (1 / 3 per lesson step); CreateInit 0224C650 / teachSPRollCutAction 02256F1C clear it (tww the same) |
| 0xEC0 | `field_0xC98` | `mTeachHitsLeft` | CreateInit 0224C650 sets HIO 0x54[mTeachStep]; teachAction 02255908 counts it down per good cut, the lesson ends at 0 |
| 0xEC4 | `field_0xC9C` | `mBackSlideDist` | BackSlideInit 0224DE14 and every battle subaction init clear it; BackSlide 022525B4 eases it to the slide distance behind mBackSlideStartPos |
| 0xEC8 | `field_0xCA0` | `mHarpoonMtx` | harpoonRelease 0224DE3C takes the held harpoon's matrix; harpoonMove 0224A2B8 concatenates it |
| 0xEF8 | `field_0xCD0` | `mHarpoonPos` | harpoonRelease 0224DE3C: translation of mHarpoonMtx; harpoonMove 0224A2B8 adds mHarpoonSpeed each frame |
| 0xF04 | `field_0xCDC` | `mHarpoonSpeed` | harpoonRelease 0224DE3C copies the release velocity; harpoonMove 0224A2B8 adds it to mHarpoonPos and gravity (HIO) to y |
| 0xF1C | `field_0xCF4` | `mHarpoonRot` | harpoonRelease 0224DE3C sets the identity quaternion; harpoonMove 0224A2B8 multiplies it by mHarpoonRotStep and normalizes |
| 0xF2C | `field_0xD04` | `mHarpoonRotStep` | harpoonRelease 0224DE3C builds it from mHarpoonSwing and the facing; harpoonMove 0224A2B8 multiplies mHarpoonRot by it |
| 0xF3C | `field_0xD14` | `mHarpoonSwing` | harpoonRelease 0224DE3C sets 0x400 and builds mHarpoonRotStep from sin of it |
| 0xF3E | `field_0xD16` | `mRollAtAngle` | evn_RollAtControl_init 02258A14 sets the start angle + 0x2000; evn_RollAtControl 02258E6C adds 0x540 per frame, mRollAtPos = pos + mRollAtRadius * (sin, cos) |
| 0xF40 | `field_0xD18` | `mRollAtRadius` | evn_RollAtControl_init 02258A14 sets 400; evn_RollAtControl 02258E6C chases it to 0 |
| 0xF44 | `field_0xD1C` | `mRollAtPos` | evn_RollAtControl 02258E6C: position on the circle of mRollAtAngle / mRollAtRadius |
| 0xF50 | `field_0xD28` | `mInitPos` | CreateInit 0224C650 / kaitenAction 02251ED0 copy home.pos; initPos 02254390 puts current.pos and old.pos there |
| 0xF5C | `field_0xD34` | `mRollTeachResult` | teachAction 02255908: the same result values (1 / 2 / 3) for a spin attack (CUT_TYPE_CUT_ROLL), held until Link's cut ends, then copied into mTeachResult (speakAction / speakBadAction); teachSPRollCutAction 02256F1C clears it (tww the same) |
| 0xF60 | `field_0xD38` | `mBackSlideStartPos` | BackSlideInit 0224DE14 and the subaction inits copy current.pos; BackSlide 022525B4 slides back from it |
| 0xF78 | `field_0xD50` | `mHeldItemOffset` | CreateInit 0224C650 sets (-13, -4, 10); _execute 0224B268 eases it to the item/normal aim offset; set_mtx 0224AEF0 translates by it |
| 0xF84 | `field_0xD5C` | `mHeldItemAngle` | CreateInit 0224C650 sets x 0x4000; _execute 0224B268 sets it to the aim angle; set_mtx 0224AEF0 rotates by it (XYZ) |
| 0xF94 | `field_0xD6C` | `mSubActionPhase` | every battle subaction (0225C208, 0225CA04, 0225C660, 0225C7EC, ...) starts at 0 and steps it; normalSubActionGuard 022504E4 sets 1 on a guard hit |
| 0xF98 | `field_0xD70` | `mTeachStep` | CreateInit 0224C650 clears it and picks HIO 0x54[it]; teachAction 02255908 tests == 4; teachSPRollCutAction sets the message count number to it |
| 0xF9C | `field_0xD74` | `mGuardCount` | normalSubActionHarpoonGuard 0224FCC4 adds 1 per guarded hit; normalSubActionGuard 022504E4 plays the guard event at 3, 5 and > 5 (then 0) |
| 0xFA0 | `field_0xD78` | `mFirstMsgDone` | getMsg1stType 0224D8E0: returns 0x978 once and sets it, 0x97B after |
| 0xFAC | `field_0xD84` | `mHarpoonState` | _draw 02249638 / set_mtx 0224AEF0: Ji1Draw_HARPOON_HELD (1) / _RELEASED; harpoonRelease 0224DE3C and evn_harpoon_proc_init 0225896C set it |

## daPz_c (Princess Zelda in the Ganondorf battle; d_a_pz.h)

Old names are the GameCube offsets (zeldaret/tww `mXXXX`, unnamed there). Named from the uses in d_a_pz*.cpp.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x7EB | `m06D3` | `mAnm` | setAnm 02454654 stores the animation number (15 keeps the current one); _execute 02454B7C sets the head splash in ANM_WAKE; modeAttackInit 02456D34 tests the bow animations |
| 0x857 | `m073F` | `mAfraidRequest` | _execute 02454B7C switches to MODE_AFRAID while it is set (outside modes 5 / 6); modeAfraid 02457D68, modeSideStep 02458000 and modeBackStep 02458274 clear it; nothing here sets it |
| 0xADC | `m08B0` | `mBattlePhase` | createInit 024528D8 clears it; _execute 02454B7C goes to MODE_DOWN in phase 1 and auto-talks only in 0 / 2; modeMoveInit 02455AB8, modeAttackInit 02456D34 and modeTalkInit 024584C0 index the l_HIO tables (move wait, circle radius, attack times, talk distance / wait) with it; getMsg 02458AEC picks the message by it |
| 0xAF0 | `m08C4` | `mTargetPos` | modeWait 0245588C / modeMoveInit 02455AB8: the player eye position or mGanondorfPos4; the circle path centre; modeAttack 02456ED8 aims at it; the eye animation copies it to mEyeTargetPos |
| 0xB4C | `m0920` | `mCircleDir` | createInit 024528D8 sets 1; modeMoveInit 02455AB8 flips it by chance (mReverseDirectionChance); the sign of mCircleAngleSpeed and of the side-step waist turn (modeSideStep 02458000) |
| 0x1178 | `m0F48` | `mAimAtPlayer` | modeAttackInit 02456D34: 1 by l_HIO mPlayerAimChance (head lock on the player), else 0; modeAttack 02456ED8 aims at mGanondorfPos4 only while it is 0 |
| 0x1195 | `m0F65` | `mAdviceGiven` | getMsg 02458AEC: in phase 2 the first message is PZ_MSG_ADVICE (then sets it), later PZ_MSG_REPEAT; modeAttackInit 02456D34 uses phase 0 timings until it is set; modeMove 024563D4 tests it |
| 0x11AC | `m0F7C` | `mTalkWaitTimer` | createInit 024528D8 / modeDown 02457718 / modeTalkInit 024584C0 load l_HIO mTalkWaitTime[...]; _execute 02454B7C counts it down and starts the talk when it reaches 0 near the player |
| 0x11B0 | `m0F80` | `mAutoTalkOff` | modeTalkInit 024584C0 sets 1 after a phase 0 talk; modeDown 02457718 and createInit 024528D8 clear it; _execute 02454B7C starts no automatic talk while it is set |
| 0x11B2 | `m0F82` | `mEventType` | _execute 02454B7C sets 1 (speak event) for the automatic talk, modeMove 024563D4 / modeFollow 02458550 set 2; eventOrder 02453A00 offers the talk (and orders the speak event for 1); checkOrder 024539BC switches to MODE_TALK and clears it |
| 0x11B8 | `m0F88` | `mHasBow` | createInit 024528D8 sets 1 for mArg 0; _draw 0245562C draws the bow only while it is set |

Open: none besides the padding (`m08A8` is only quoted in a comment).

## daBigelf_c (Great Fairy; d_a_bigelf.h)

Old names are the GameCube offsets (zeldaret/tww `mXXX`, unnamed there). Named from the uses in d_a_bigelf.cpp.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x5E4 | `m3A0` | `mDarkRatio` | darkProc 0209744C eases it toward mDarkRatioTarget while mDarkActive and scales the actor, background and sky light ratios by it; demoInitFlDmMd 020967A4 sets 1 |
| 0x5E8 | `m3A4` | `mDarkRatioTarget` | demoProcFlDmMd 02097130 sets it from the frames left before the flower frame (0 after it); demoInitFlDmMd 020967A4 / demoProcFlDelete 02096EEC set 1; darkProc 0209744C eases mDarkRatio toward it |
| 0x5EC | `m3A8` | `mColorBlendTarget` | _execute 020953C4 eases mColorBlend toward it; demoInitAppear 0209611C sets 0, demoProcExit 02096B20 / ready0 020977AC / CreateHeap 020948EC set 1 |
| 0x5F0 | `m3AC` | `mColorBlend` | _execute 020953C4 passes it to bigelf_customColorSet(4, 0, it) (not for type 6) and eases it toward mColorBlendTarget; CreateHeap 020948EC sets 1; HD (tww has padding here) |
## daAuction_c (d_a_auction.h; partial view auction_cpp::Manager in d_a_npc_auction.h)

The Windfall auction controller. Old names are the GameCube offsets (zeldaret/tww `mXXX`); the header writes the HD offsets without 0x (`/* 850 */`). Ten fields take the names of the existing d_a_npc_auction.h Manager view of this class, which the auction NPCs read and write through (`m81C` / `m81D` adapted from its array `mPairedCostumes[2]`). Functions without a class are daAuction_c members; daNpcAuction_c functions are named.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x850 | `m738` | `mNpcIds` | constructor 02058EF4 sets all 8 to 0xFFFFFFFF; daNpcAuction_c phase_1 021EB8DC stores its process id in [its mNpcNo]; executeWait 0205AD44 goes to MOVE_NORMAL once all 8 are set; getNpcActorP 02059854 finds the NPC by it (tww the same; tww setNpcID()); same name as the d_a_npc_auction.h Manager view mNpcIds |
| 0x8A4 | `m78C` | `mCameraCenter` | privateCut 0205A104 passes it as the center of dCamera_c::Set(center, eye) while mCameraFlags 0x4 and 0x8 are set; setCameraNpc 020598BC sets it to the NPC's pos + (0, mCenterOffsetY, 0); eventMainInit 020596F0 / eventMainKai 0205AF48 copy l_camera_pos[i][0] (tww the same) |
| 0x8B0 | `m798` | `mCameraEye` | privateCut 0205A104 passes it as the eye of dCamera_c::Set(center, eye); setCameraNpc 020598BC sets it to mCameraCenter + the rotated mEyeDistance offset; eventMainInit 020596F0 / eventMainMsgEnd 0205C014 copy l_camera_pos[i][1] (tww the same) |
| 0x8C0 | `m7A8` | `mBikonPos` | eventMainMsgBikonC 0205C248: the bidder's pos + getPiconDispOfs, where it sets particle 0x8153 (tww dPa_name::ID_IT_SN_PF_BIKON01) once the camera arrived (tww the same) |
| 0x8DC | `m7C4` | `mBidGauge` | eventMainKai 0205AF48 / eventMainUri 0205B9C0 / nextBet 0205AD98: one gauge per bidder ([0] Link, raised on A; [1..6] the NPCs, by NpcDatStruct mGaugeIncMin/Max); a bidder bids once it reaches 100 (l_gaugeMaximum) and is reset; eventMain 02059DB4 shows [0] as mAuctionGauge (tww the same) |
| 0x904 | `m7EC` | `mNextMsgNo` | next_msgStatus 0205C55C stores the next message number when it continues (status 15), else 0; eventMainMsgEnd 0205C014 turns the camera to mReplyNpc when the button is pressed on an NPC reply (0x1D3A-0x1D4B, 0x339D-0x33A8) (tww the same) |
| 0x908 | `m7F0` | `mBidMsgNo` | eventMainKai 0205AF48 sets 0x1CF9 (NPC bid) / 0x1CFA (Link's bid), eventMainUri 0205B9C0 0x1CF9; eventMainMsgBikonW 0205C4B4 shows it with setMessage2 when mTimer runs out; constructor 02058EF4 0 (tww the same) |
| 0x918 | `m800` | `mDisplayBid` | eventMainKai 0205AF48 / eventMainUri 0205B9C0: the new total after an NPC bid, mCurrBid + 1 for Link's bid; eventMainKai / eventMainMsgBikonC 0205C248 copy it into mMsgCountNumber, the number the bid message shows (tww the same) |
| 0x91E | `m806` | `mBidWaitTimer` | eventMainKai 0205AF48 / eventMainUri 0205B9C0 count it down each frame and keep the NPC gauges still while it runs; set after a bid to (rnd + 1) * 60 frames and by next_msgStatus 0205C55C (0x1CFA) to l_after_bet_wait * 60 (tww the same) |
| 0x920 | `m808` | `mCameraSwayAngle` | eventMainKai 0205AF48: while mGestureFlags 0x2 is set, mCameraCenter.x = Link's x + sin(it) * 256 (l_paralyzedCameraRadius) and it grows by 200 a frame; next_msgStatus 0205C55C sets 0 with that bit (tww the same) |
| 0x922 | `m80A` | `mEmitterFadeDelay` | _execute 0205AAC8: while it is nonzero the emitter fade-in (mEmitterFadeFlags 0x1) waits and counts it down; never set (only counted down) in WWHD or tww |
| 0x924 | `m80C` | `mNpcKinds` | constructor 02058EF4: 0..7, then entries 1-6 swapped 100 times by getRand; daNpcAuction_c phase_1 021EB8DC takes [its prm npc no] as its mNpcNo (tww the same shuffle); same name as the d_a_npc_auction.h Manager view mNpcKinds |
| 0x934 | `m81C` | `mPairedCostume0` | constructor 02058EF4 0xFF; daNpcAuction_c phase_1 021EB8DC stores the costume of the first NPC of kinds 2-3 and moves the second one off it if it rolls the same; the d_a_npc_auction.h Manager view mPairedCostumes[0] |
| 0x935 | `m81D` | `mPairedCostume1` | constructor 02058EF4 0xFF; daNpcAuction_c phase_1 021EB8DC: the same for NPC kinds 4-5; the d_a_npc_auction.h Manager view mPairedCostumes[1] |
| 0x937 | `m81F` | `mMainState` | eventMain 02059DB4 runs eventProc[it] (enum MainState MAIN_KAI .. MAIN_BIKON_W); eventMainInit 020596F0 sets MAIN_URI for mAuctionType 2, else MAIN_KAI; setMessage2 0205AD6C MAIN_MSG_SET; the Kai / Uri / MsgSet / MsgEnd / BikonC states move it on (tww the same) |
| 0x938 | `m820` | `mActiveNpcs` | constructor 02058EF4 1, eventStartInit 02059544 0xFE; daNpcAuction_c isExecute 021EC0E8 runs an NPC only while bit mNpcNo is set (tww the same writes); same name as the d_a_npc_auction.h Manager view mActiveNpcs |
| 0x939 | `m821` | `mSelection` | daNpcAuction_c wait_action 021ED81C / eventMove 021ECB68 set 1 (auction) or 2 (second auction, with mItemChoice) after the talk; eventOrder 0205A724 orders AUCTION_START / AUCTION_START2 for it, copies it to mAuctionType and clears it (tww the same); Manager view mSelection |
| 0x93A | `m822` | `mAuctionType` | eventOrder 0205A724 copies mSelection; for 2 eventStartInit 02059544 uses a 30 s timer (else 60), eventMainInit 020596F0 starts in MAIN_URI, eventGetItemInit 02059A44 l_item_dat2, next_msgStatus 0205C55C 0x1D1B / 0x1D1C; for 1 the gauge is shown (tww the same) |
| 0x93C | `m824` | `mCurrentNpc` | the top bidder (0 = Link): createInit 020590FC 0; next_msgStatus 0205C55C sets it to mResult when the bid message runs; eventMove 0205A428 orders NO_ITEM while nonzero, else GET_ITEM; daNpcAuction_c getMsg 021EDBF8 picks its line by it (tww the same); Manager view mCurrentNpc |
| 0x93D | `m825` | `mReplyNpc` | next_msgStatus 0205C55C: the outbid top bidder when Link bids, or the random NPC 1-6 answering the after-bet line (0x1D20-0x1D23); eventMainMsgEnd 0205C014 turns the camera to it and sets mSpeakingNpc on the reply messages (tww the same) |
| 0x93E | `m826` | `mTopBidder` | createInit 020590FC 0xFF (no bid yet); next_msgStatus 0205C55C sets it with mCurrentNpc from mResult; eventMainKai 0205AF48 taunts only once it is not 0xFF; setLinkAnm 02059D50 uses animation 0x1D for 1 while it is 0 (Link leads) (tww the same) |
| 0x93F | `m827` | `mResult` | the bidder of the bid in progress (0 = Link): eventMainMsgBikonC 0205C248 copies mNewBidder, the effect / camera / bid message use it; next_msgStatus 0205C55C copies it to mCurrentNpc / mTopBidder; daNpcAuction_c next_msgStatus 021ED964 shows 0x1D39 while nonzero (tww the same); Manager view mResult |
| 0x940 | `m828` | `mNewBidder` | eventMainKai 0205AF48 / eventMainUri 0205B9C0: the first bidder whose mBidGauge reached 100 below its mSpendingLimit; eventMainMsgBikonC 0205C248 copies it into mResult and mSpeakingNpc (tww the same) |
| 0x941 | `m829` | `mSpeakingNpc` | the NPC whose line runs: eventMainMsgBikonC 0205C248 the bidder, eventMainMsgEnd 0205C014 mReplyNpc, eventGetItemNpcInit 02059B54 mCurrentNpc; daNpcAuction_c eventMain 021EC400 plays the mGestureFlags gestures for the NPC with this number (tww the same); Manager view mSpeakingNpc |
| 0x942 | `m82A` | `mRestMsgIdx` | eventMainKai 0205AF48 / eventMainUri 0205B9C0: once the time left drops below l_rest_msg_time1/2[it], show l_rest_msg_no[it] with sound l_rest_se_no[it] and add 1; eventMainInit 020596F0 0 (tww the same) |
| 0x943 | `m82B` | `mAuctionEnded` | set 1 when the time runs out (eventMainKai 0205AF48 / eventMainUri 0205B9C0) and by the closing messages (next_msgStatus 0205C55C 0x1D1A / 0x1D1F / 0x1D24); eventMain 02059DB4 then ends the main cut (screens hidden, Link demo 2) (tww the same) |
| 0x944 | `m82C` | `mAfterBetIdx` | next_msgStatus 0205C55C (0x1CFA): the l_after_bet_rate / _chk / _wait tier Link's bid met (4 = none); on 0x1CF9 a value below 4 shows l_after_bet_msg_no[it], else the NPC reply; NPC bids set 4 (eventMainKai 0205AF48 / eventMainUri 0205B9C0) (tww the same) |
| 0x946 | `m82E` | `mFirstBidMade` | eventMainUri 0205B9C0 sets 1 at the first NPC bid; while it is set A shows 0x1D1A (stop the auction) and the Do status is 0x25; eventMainInit 020596F0 0 (tww the same) |
| 0x947 | `m82F` | `mCameraPosIdx` | eventMainKai 0205AF48 / eventMainMsgEnd 0205C014 set getRand(3) and copy l_camera_pos[it] into mCameraCenter / mCameraEye; eventMainInit 020596F0 0 (WWHD uses the register copy; tww reads the field back) |
| 0x948 | `m830` | `mTauntShown` | eventMainKai 0205AF48 / eventMainUri 0205B9C0: after a bid it is only cleared while set; else a taunt (0x1CFD-0x1D00, 1 in 3) may run and sets it; eventStartInit 02059544 1 (tww the same) |
| 0x949 | `m831` | `mNextBetDelay` | nextBet 0205AD98: while nonzero it is counted down and nothing else happens; else every NPC gauge gets a random push and it becomes the number of NPCs at 100 plus 2 (tww the same) |
| 0x94A | `m832` | `mSetLinkFace` | eventMain 02059DB4 sets Link's mFace to mFace while it is set; eventMainMsgBikonC 0205C248 sets 1 with a random l_link_face when an NPC bids; constructor 02058EF4 / eventMainMsgEnd 0205C014 0 (tww the same) |
| 0x94C | `m834` | `mGestureFlags` | bit field: 0x1 eventMainMsgBikonW 0205C4B4 (NPC bid line), 0x2 / 0x4 the sway camera (next_msgStatus 0205C55C, eventMainKai 0205AF48), 0x8 eventMainMsgEnd 0205C014 reply, 0x10 eventGetItemNpcInit 02059B54, 0x20 taunt; daNpcAuction_c eventMain 021EC400 gestures by them (tww the same); Manager view mGestureFlags |
| 0x94D | `m835` | `mCameraFlags` | bit field read by privateCut 0205A104: 0x1 stop the camera (then 0x4), 0x2 restart it (clears 0x4 / 0x8), 0x4 + 0x8 set mCameraCenter / mCameraEye; setCameraNpc 020598BC sets 0x9, offCamera 02059B40 / eventCameraOffNpc 0205A010 0x2 (tww the same; tww offCamera()) |
| 0x94E | `m836` | `mEmitterFadeFlags` | _execute 0205AAC8: 0x1 fades the emitter's alpha in to the maximum (then clears it), 0x2 fades it out and deletes the emitter at 0; eventMainInit 020596F0 sets 0x1, eventMain 02059DB4 0x2 when the auction ends (tww the same) |
| 0x950 | `m838` | `mOrderEvent` | eventOrder 0205A724: 1 / 2 the talk (1 orders the speak event), 3 AUCTION_START, 4 GET_ITEM, 5 NO_ITEM, 6 START2, 7 END2; checkOrder 02059380 clears it once that event starts; eventOrder / eventMove 0205A428 set it (tww the same); same role as daNpc_Hi1_c::mOrderEvent |

Open: `m7E0` / `m7E4` (an oscillator that eventMainKai bounces between -1 and 1; never read for anything else in WWHD or tww), `m804` (only written, 3600, never read in WWHD or tww); the padding `_8CC[8]`, `_900[4]`, `_952[2]` (no use in WWHD or tww; kept as declared).

## NpcDatStruct, NpcMsgDatStruct, NpcCameraDatStruct (d_a_auction.h; the per-bidder data tables l_npc_dat, l_npc_msg_dat, l_npc_camera_dat)

Old names are the GameCube offsets (zeldaret/tww `field_0x...`, unnamed there). These NpcDatStruct / NpcMsgDatStruct / NpcCameraDatStruct are d_a_auction.cpp's own tables, not the same layout as the daNpcRoten_c / daNpcPhoto_c NpcDatStruct. Several fields share one declaration line (`be<f32> field_0x00, field_0x04;`); renamed in place.

| Struct | Old | New | Evidence |
|---|---|---|---|
| NpcDatStruct | `field_0x00` | `mGaugeIncMin` | eventMainKai 0205AF48 / eventMainUri 0205B9C0: mBidGauge[i] += it + rnd(mGaugeIncMax - it) (row 0 on Link's A press, rows 1-6 each frame outside mBidWaitTimer; Uri doubles it) (tww the same) |
| NpcDatStruct | `field_0x04` | `mGaugeIncMax` | eventMainKai 0205AF48 / eventMainUri 0205B9C0: the top of the random mBidGauge step, mGaugeIncMin + rnd(it - mGaugeIncMin) (tww the same) |
| NpcMsgDatStruct | `field_0x00` | `mNpcNameMsgID` | createInit 020590FC, eventMain 02059DB4, eventMainKai 0205AF48 / eventMainUri 0205B9C0, eventMainMsgBikonC 0205C248, next_msgStatus 0205C55C: the bidder kind's name message, set as mNpcNameMessageID (tww dComIfGp_setNpcNameMessageID) |
| NpcMsgDatStruct | `field_0x02` | `mOutbidMsgNo` | next_msgStatus 0205C55C (0x1CF9 after Link's bid with no after-bet line): the outbid top bidder kind's reply while 10 s or more remain (tww: 60 s); 0 (kinds 10 / 11) = no reply (tww the same) |
| NpcMsgDatStruct | `field_0x04` | `mOutbidLateMsgNo` | next_msgStatus 0205C55C (0x1CF9): the same reply as mOutbidMsgNo when under 10 s remain (tww: 60 s) (tww the same) |
| NpcMsgDatStruct | `field_0x06` | `mAfterBetReplyMsgNo` | next_msgStatus 0205C55C: after the after-bet lines 0x1D20-0x1D23 (l_after_bet_msg_no) the next message is this one of a random NPC 1-6's kind (mReplyNpc) (tww the same) |
| NpcMsgDatStruct | `field_0x08` | `mWinMsgNo` | eventGetItemMesInit 02059BD4: the message of the GET_ITEM_MES cut, of the winning NPC's kind (mCurrentNpc) (tww the same) |
| NpcCameraDatStruct | `field_0x00` | `mEyeDistance` | setCameraNpc 020598BC: mCameraEye starts at (sin, 0, cos)(the NPC's angle y) * it before the rotations, the eye's distance from mCameraCenter (tww the same) |
| NpcCameraDatStruct | `field_0x04` | `mCenterOffsetY` | setCameraNpc 020598BC: mCameraCenter = the NPC's pos + (0, it, 0) (tww the same) |
| NpcCameraDatStruct | `field_0x08` | `mEyeAngleX` | setCameraNpc 020598BC: the XrotM angle applied to the eye offset (-500 in Link's row) (tww the same) |
| NpcCameraDatStruct | `field_0x0A` | `mEyeAngleY` | setCameraNpc 020598BC: the YrotS base angle, plus the passed angle or a random cM_rndFX(l_cameraRandomAngle) (tww the same) |

Open: AuctionLight `_1C` (LIGHT_INFLUENCE +0x1C; no use in d_a_auction.cpp).

<!-- ===== mt (from mt/rows-npcs.md) ===== -->

## Still open after round 2

Re-checked in round 2, no name:

- daNpc_Os_c (d_a_npc_os.h): the existing Open line stands (no new evidence).
- daNpcRoten_c (d_a_npc_roten.h): the existing Open line stands; add dNpc_PathRun_r `field_0x07` (view of d_npc.h
  dNpc_PathRun_c; no use in WWHD or tww).
- daNpc_P2_c (d_a_npc_p2.h): round 3 gives `m8C2` / `m8C9` the names of the copy in d_a_npc_p2_act.h,
  `mbGoalTalkStarted` (goal_talkpos_wait 022BC868) / `mbMessageGestureStarted` (anmAtr 022B8B24, talk 022B8E24).
  Open: `m978` (demo_lift 022BC79C only sets 0; the copy calls it mLiftPhase), `m924` (only cleared by
  cutRopeTalkStart 022B7F30).
- kg1_core::daNpc_Kg1_c (d_a_npc_kg1_core.h): `m70C` (only constructed in _create 02266268), `m778` (only cleared by
  wait_action 02266D60).
- daNpc_Ym1_c views (d_a_npc_ym1_a2.h): Ym1StateView `m8A0` (view of daNpc_Ym1_c mA10, unnamed there).
- Ji1EventTsuboHD (d_a_npc_ji1_event.h): `m688`..`m692`, a view of daTsubo::Act_c (d_a_tsubo*.h), unnamed there.


## daNpc_Rsh1_HIO_c (l_HIO; d_a_npc_rsh1.h)

Round 4 (two-digit offset names). The stall owner's HIO: the ShopCursor_c values it passes to ShopCursor_create and setScale take the cursor's names, plus the walk step and the path-point search distance. Same meanings in zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `m08` | `mRefCount` | _create 022D7998 creates the HIO child while it is < 0, then adds 1; _delete 022D7BF4 subtracts 1 and deletes the child when it falls below 0; daNpc_Rsh1_HIO_ct 022DB690 sets -1 (tww the same); named after daNpc_Ac1_HIO_c / daNpc_Jb1 HIO mRefCount (same code) |
| 0x30 | `m34` | `mCursorScale` | CreateHeap 022D6CC4 passes it as ShopCursor_create's scale; lookBack 022D83A8 passes it to the cursor's setScale as mPieceScale; HIO ctor 1.0 (tww the same) |
| 0x34 | `m38` | `mCursorScaleYFar` | lookBack 022D83A8: the cursor's setScale argument stored as ShopCursor_c::mPieceScaleYFar; HIO ctor 0.9 (tww the same) |
| 0x38 | `m3C` | `mCursorScaleYNear` | lookBack 022D83A8: the cursor's setScale argument stored as ShopCursor_c::mPieceScaleYNear; HIO ctor 0.5 (tww the same) |
| 0x3C | `m40` | `mCursorDistFar` | lookBack 022D83A8: the cursor's setScale argument stored as ShopCursor_c::mPieceDistFar; HIO ctor 40 (tww the same) |
| 0x40 | `m44` | `mCursorDistNear` | lookBack 022D83A8: the cursor's setScale argument stored as ShopCursor_c::mPieceDistNear; HIO ctor 30 (tww the same) |
| 0x44 | `m48` | `mMoveSpeed` | shopPosMove 022D8E84 and pathMove 022D9F74: the step of cLib_chasePosXZ towards the shop position / next path point; HIO ctor 7.5 (tww the same) |
| 0x48 | `m4C` | `mAimSearchDist` | getAimShopPosIdx 022D9E04: the starting distance of the search for the path point nearest to Link (only points closer than it count; then the nearest one's distance); HIO ctor 1300 (tww the same) |

Open (no name yet): `m5C`[12], `m68`, `m69`, `m6A`, `m6B` (the HIO ctor clears them; only tww's VERSION_DEMO block in _execute reads them, as GetItemReserve / event-bit debug switches; no retail read in WWHD or tww), padding `_01`.

## daNpc_Bs1_childHIO_c and daNpc_Bs1_HIO_c (l_HIO.mChild[mType], l_HIO; d_a_npc_bs1.h)

Round 4 (two-digit offset names). The shop NPC's per-type HIO (two children, by mType): the helmet switch and the ShopCursor_c values passed to ShopCursor_create and setScale, which take the cursor's names. The parent HIO's m8 is the HIO reference count. Same meanings in zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x2C | `m2C` | `mDrawHelmet` | _draw 02214104 draws mpHelmetModel on the head joint only while it is set; daNpc_Bs1_HIO_ct 022176B4 sets it to the child's index (0 for mType 0, 1 for mType 1) (tww the same) |
| 0x30 | `m30` | `mCursorScale` | CreateHeap 02212818 passes it as ShopCursor_create's scale; lookBack 022163B0 stores it in the cursor's mPieceScale (setScale); HIO ctor 0.65 (tww the same) |
| 0x34 | `m34` | `mCursorScaleYFar` | lookBack 022163B0 stores it in the cursor's mPieceScaleYFar (setScale); HIO ctor 0.9 (tww the same) |
| 0x38 | `m38` | `mCursorScaleYNear` | lookBack 022163B0 stores it in the cursor's mPieceScaleYNear (setScale); HIO ctor 0.5 (tww the same) |
| 0x3C | `m3C` | `mCursorDistFar` | lookBack 022163B0 stores it in the cursor's mPieceDistFar (setScale); HIO ctor 27 (tww the same) |
| 0x40 | `m40` | `mCursorDistNear` | lookBack 022163B0 stores it in the cursor's mPieceDistNear (setScale); HIO ctor 20 (tww the same) |
| 0x04 (daNpc_Bs1_HIO_c) | `m8` | `mRefCount` | _create 02213610 creates the HIO child while it is < 0, then adds 1; _delete 02213894 subtracts 1 and deletes the child when it falls below 0; daNpc_Bs1_HIO_ct 022176B4 sets -1 (tww the same); as the other NPC HIOs' mRefCount |

Open (no name yet): `unk0` (+0x00, never written or read in WWHD or tww), padding `_2D`.

## daNpc_Ds1_childHIO_c (l_HIO.mChild[mType]; d_a_npc_ds1.h)

Round 4 (two-digit offset names). The attention box of chkAttention and the ShopCursor_c values passed to setScale, which take the cursor's names as in daNpc_Bs1_childHIO_c / daNpc_Rsh1_HIO_c. tww's d_a_npc_ds1 has stubs only.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x2C | `m2C` | `mAttnMaxX` | chkAttention 0222FDBC: Link counts as in front only while \|x\| of his offset from the given position is below it; HIO ctor 300 (tww d_a_npc_ds1 has stubs only) |
| 0x30 | `m30` | `mAttnMaxZ` | chkAttention 0222FDBC: Link counts as in front only while z of his offset from the given position is below it; HIO ctor 300 (tww d_a_npc_ds1 has stubs only) |
| 0x38 | `m38` | `mCursorScaleYFar` | lookBack 02230B24 stores it in the cursor's mPieceScaleYFar (setScale); HIO ctor 0.9 (tww d_a_npc_ds1 has stubs only; as Bs1 / Rsh1) |
| 0x3C | `m3C` | `mCursorScaleYNear` | lookBack 02230B24 stores it in the cursor's mPieceScaleYNear (setScale); HIO ctor 0.5 (tww d_a_npc_ds1 has stubs only; as Bs1 / Rsh1) |
| 0x40 | `m40` | `mCursorDistFar` | lookBack 02230B24 stores it in the cursor's mPieceDistFar (setScale); HIO ctor 27 (tww d_a_npc_ds1 has stubs only; as Bs1 / Rsh1) |
| 0x44 | `m44` | `mCursorDistNear` | lookBack 02230B24 stores it in the cursor's mPieceDistNear (setScale); HIO ctor 20 (tww d_a_npc_ds1 has stubs only; as Bs1 / Rsh1) |

Open (no name yet): padding `_00`.

## daNpc_Ls1_HIO_c::hio_prm_c (l_HIO.mPrm; d_a_npc_ls1.h)

Round 5 (two-digit offset names). Aryll's HIO values: the look-at speed, the distances at which she greets Link and starts the telescope lesson, the telescope zoom thresholds and screen radius around the Rito, and the eye texture divisor. Same meanings in zeldaret/tww d217e2b.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x12 | `m12` | `mMaxHeadTurnVel` | lookBack 02280C40 passes it to the look-at (lookAtTarget_2) as the max turn speed; HIO 0x960 (tww the same); same role as daNpc_Hi1_HIO_c::hio_prm_c::mMaxHeadTurnVel |
| 0x18 | `m18` | `mDebugDraw` | _draw 02281318: only gates the debug-colour statics (tww: a dead debug block); HIO 0, never written elsewhere in WWHD or tww; same slot and role as daNpc_Aj1_HIO_c::hio_prm_c::mDebugDraw |
| 0x1C | `m1C` | `mTelescopeDist` | wait_2 02282B54: telescope_proc runs only while Link is within it (xz, with 100 height) and looking through the telescope; HIO 700 (tww the same) |
| 0x20 | `m29` | `mOmedetoDist` | wait_1 02282A60: Link within it (xz, with 100 height) orders event 4, tww l_evn_tbl[1] "omedeto" (the birthday greeting); HIO 200 (tww the same) |
| 0x24 | `m24` | `mZoomMsgScale` | telescope_proc 022825A0 step 1: with the Rito (bm1_npc_p) in view the hint is message 0xBCA below this camera zoom scale, else 0xBCB; HIO 2.0 (tww the same) |
| 0x28 | `m28` | `mZoomDoneScale` | telescope_proc 022825A0 step 2: once the camera zoom scale reaches it the camera focuses on the Rito (bm1 setFocus) and the step goes to 3; HIO 9.0 (tww the same) |
| 0x2C | `m2C` | `mScopeScreenRadius` | telescope_proc 022825A0 passes it to chkTelescope_sph 02282380: bm1_npc_p's (the Rito's) projected attention point must lie within it of the screen centre (319.5, 186.5); HIO 50 (tww the same) |
| 0x30 | `m30` | `mEyeAngleDiv` | eye_ctrl 0227F2DC: the eye texture offset is mEyeAngleX / mEyeAngleY divided by it, * 0.1, clamped to +-1; HIO 0x1A2C (tww the same) |

Open (no name yet): `daNpc_Ls1_HIO_c::m04` and `m08` (set to -1 by the constructor, read nowhere in WWHD or tww), padding `_19`, `_32`.


## daNpc_Ds1_HIO_c (l_HIO; d_a_npc_ds1.h), alignment

Round 5. The instance counter is aligned with the other NPC HIOs' mRefCount (same code as daNpc_Bs1_HIO_c::mRefCount).

| WWHD offset | Old | New | Evidence |
|---|---|---|---|
| 0x04 | `mCount` | `mRefCount` | _create 0222E8F0 creates the HIO child while it is < 0, then adds 1; _delete 0222EA54 subtracts 1 and deletes the child when it falls below 0; daNpc_Ds1_HIO_c_ct 02232B9C sets -1; the same code as daNpc_Bs1_HIO_c::mRefCount and the other NPC HIOs' mRefCount |

## daNpc_Km1_HIO_c::hio_prm_c (l_HIO.mPrmTbl; d_a_npc_km1.h)

Round 6 (one-digit offset names). The joint control limits passed to m_jnt.setParam, in the order of tww's dNpc_JntCtrl_c::setParam (backbone first, then head); the same slots as daNpc_Ls1_HIO_c / daNpc_Hi1_HIO_c::hio_prm_c.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x00 | `field_0` | `mMaxHeadX` | _execute 022726A0 passes it to m_jnt.setParam as argument 5 (tww dNpc_JntCtrl_c::setParam max_head_x), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c / daNpc_Hi1_HIO_c::hio_prm_c::mMaxHeadX |
| 0x02 | `field_2` | `mMaxHeadY` | _execute 022726A0 passes it to m_jnt.setParam as argument 6 (tww dNpc_JntCtrl_c::setParam max_head_y), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c / daNpc_Hi1_HIO_c::hio_prm_c::mMaxHeadY |
| 0x04 | `field_4` | `mMinHeadX` | _execute 022726A0 passes it to m_jnt.setParam as argument 7 (tww dNpc_JntCtrl_c::setParam min_head_x), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c / daNpc_Hi1_HIO_c::hio_prm_c::mMinHeadX |
| 0x06 | `field_6` | `mMinHeadY` | _execute 022726A0 passes it to m_jnt.setParam as argument 8 (tww dNpc_JntCtrl_c::setParam min_head_y), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c / daNpc_Hi1_HIO_c::hio_prm_c::mMinHeadY |
| 0x08 | `field_8` | `mMaxBackboneX` | _execute 022726A0 passes it to m_jnt.setParam as argument 1 (tww dNpc_JntCtrl_c::setParam max_backbone_x), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c::mMaxBackBoneX / daNpc_Hi1_HIO_c::hio_prm_c::mMaxBackboneX |
| 0x0A | `field_A` | `mMaxBackboneY` | _execute 022726A0 passes it to m_jnt.setParam as argument 2 (tww dNpc_JntCtrl_c::setParam max_backbone_y), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c::mMaxBackBoneY / daNpc_Hi1_HIO_c::hio_prm_c::mMaxBackboneY |
| 0x0C | `field_C` | `mMinBackboneX` | _execute 022726A0 passes it to m_jnt.setParam as argument 3 (tww dNpc_JntCtrl_c::setParam min_backbone_x), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c::mMinBackBoneX / daNpc_Hi1_HIO_c::hio_prm_c::mMinBackboneX |
| 0x0E | `field_E` | `mMinBackboneY` | _execute 022726A0 passes it to m_jnt.setParam as argument 4 (tww dNpc_JntCtrl_c::setParam min_backbone_y), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c::mMinBackBoneY / daNpc_Hi1_HIO_c::hio_prm_c::mMinBackboneY |

Open (no name yet): padding `_19`.


## setParam backbone limits, spelling alignment

Round 7. The joint control's backbone limits (tww dNpc_JntCtrl_c::setParam arguments 1-4) had two spellings in the
port: 30 headers used tww dNpc_HIO_c's mMaxBackboneX / mMaxBackboneY / mMinBackboneX / mMinBackboneY, 8 used
mMaxBackBoneX etc. The 7 below are our own spellings and take dNpc_HIO_c's. d_a_npc_ls1.h keeps mMaxBackBoneX etc.:
that is tww's own name in its d_a_npc_ls1.h, and tww's code does not show it wrong. Not offset names, so the census
count is unchanged.

| Header | Struct | WWHD offset | Old | New |
|---|---|---|---|---|
| d_a_npc_aj1.h | hio_prm_c | 0x08 | `mMaxBackBoneX` | `mMaxBackboneX` |
| d_a_npc_aj1.h | hio_prm_c | 0x0A | `mMaxBackBoneY` | `mMaxBackboneY` |
| d_a_npc_aj1.h | hio_prm_c | 0x0C | `mMinBackBoneX` | `mMinBackboneX` |
| d_a_npc_aj1.h | hio_prm_c | 0x0E | `mMinBackBoneY` | `mMinBackboneY` |
| d_a_npc_bmcon1.h | daNpcBmcon_c__l_npc_dat | 0x04 | `mMaxBackBoneX` | `mMaxBackboneX` |
| d_a_npc_bmcon1.h | daNpcBmcon_c__l_npc_dat | 0x06 | `mMaxBackBoneY` | `mMaxBackboneY` |
| d_a_npc_bmcon1.h | daNpcBmcon_c__l_npc_dat | 0x0C | `mMinBackBoneX` | `mMinBackboneX` |
| d_a_npc_bmcon1.h | daNpcBmcon_c__l_npc_dat | 0x0E | `mMinBackBoneY` | `mMinBackboneY` |
| d_a_npc_gk1.h | hio_prm_c | 0x08 | `mMaxBackBoneX` | `mMaxBackboneX` |
| d_a_npc_gk1.h | hio_prm_c | 0x0A | `mMaxBackBoneY` | `mMaxBackboneY` |
| d_a_npc_gk1.h | hio_prm_c | 0x0C | `mMinBackBoneX` | `mMinBackboneX` |
| d_a_npc_gk1.h | hio_prm_c | 0x0E | `mMinBackBoneY` | `mMinBackboneY` |
| d_a_npc_gp1.h | hio_prm_c | 0x08 | `mMaxBackBoneX` | `mMaxBackboneX` |
| d_a_npc_gp1.h | hio_prm_c | 0x0A | `mMaxBackBoneY` | `mMaxBackboneY` |
| d_a_npc_gp1.h | hio_prm_c | 0x0C | `mMinBackBoneX` | `mMinBackboneX` |
| d_a_npc_gp1.h | hio_prm_c | 0x0E | `mMinBackBoneY` | `mMinBackboneY` |
| d_a_npc_kf1.h | hio_prm_c | 0x08 | `mMaxBackBoneX` | `mMaxBackboneX` |
| d_a_npc_kf1.h | hio_prm_c | 0x0A | `mMaxBackBoneY` | `mMaxBackboneY` |
| d_a_npc_kf1.h | hio_prm_c | 0x0C | `mMinBackBoneX` | `mMinBackboneX` |
| d_a_npc_kf1.h | hio_prm_c | 0x0E | `mMinBackBoneY` | `mMinBackboneY` |
| d_a_npc_pf1.h | hio_prm_c | 0x08 | `mMaxBackBoneX` | `mMaxBackboneX` |
| d_a_npc_pf1.h | hio_prm_c | 0x0A | `mMaxBackBoneY` | `mMaxBackboneY` |
| d_a_npc_pf1.h | hio_prm_c | 0x0C | `mMinBackBoneX` | `mMinBackboneX` |
| d_a_npc_pf1.h | hio_prm_c | 0x0E | `mMinBackBoneY` | `mMinBackboneY` |
| d_a_npc_km1.h | hio_prm_c | 0x08 | `mMaxBackBoneX` | `mMaxBackboneX` |
| d_a_npc_km1.h | hio_prm_c | 0x0A | `mMaxBackBoneY` | `mMaxBackboneY` |
| d_a_npc_km1.h | hio_prm_c | 0x0C | `mMinBackBoneX` | `mMinBackboneX` |
| d_a_npc_km1.h | hio_prm_c | 0x0E | `mMinBackBoneY` | `mMinBackboneY` |

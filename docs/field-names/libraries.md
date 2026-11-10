# Field names: libraries

Offset-named fields of library code (JAudio/JAZelAudio, JSystem, f_op/f_pc/m_Do framework, sead/agl/nw), named in the
field naming pass (2026-11-10; the rules below are those of every file in docs/field-names/). A
name comes from how the original code uses the field (the address of the use and what it shows), from names already
in this decomp, or, for JSystem, from the CC0 zeldaret/tww and zeldaret/tp decomps. sead/agl/nw names are our own
(layouts only cross-checked, never code). Renames only: offsets, types and code are unchanged (objcmp:
object-identical). Each declaration keeps the old name (`was field_0x...`) with the evidence. Fields without enough
evidence keep their offset name and are listed as open.

## JAIZelBasic (JAZelAudio/JAIZelBasic.h; views in JAIZelBasic_3/_5/_6/_7/_8.h, JAIZelAtmos.h, JAIZelAnime.h)

Old names are the GameCube offsets (zeldaret/tww include/JAZelAudio/JAIZelBasic.h at d217e2b, which has no names for
them). The eight per-unit headers are views of the same class; every view follows the names of JAIZelBasic.h (the
views carry `was ...: JAIZelBasic::NAME (JAZelAudio/JAIZelBasic.h)`). Sword/shield argument meanings follow the callers
in zeldaret/tww src/d/actor/d_a_player_main.cpp (CC0). The main BGM volume is the product of the eight `mMainBgm*Volume`
factors (+0x90 .. +0xAC and +0xBC), the sub BGM inter-volume the product of the three `mSubBgm*Volume` factors
(+0xB0 .. +0xB8).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x4 | `field_0x8` | `mHeap` | JAIBasic::mHeap (JSystem/JAudio/JAIBasic.h, the base class: heap at +4); makeSound 0201D414 allocates the Sound arrays from it (l_defaultHeap / the default audio heap when NULL) |
| 0x30 | `field_0x0020` | `mIsMenuIn` | menuIn 0201E5E0 sets 1, menuOut 0201DCD4 clears; seaEnvSePlay 0201C524 / seaShoreSE 0201CA10 play at 0.33 while set |
| 0x31 | `field_0x0021` | `mSceneBgmStartedOnce` | sceneBgmStart 020234A4 sets 1 (never cleared); setScene 0202677C checks the dungeon stage items only once it is set |
| 0x34 | `field_0x0024` | `mpEventBits` | setEventBit 020277A4 stores the pointer; checkEventBit 0201E480 tests bit (flag & 0xFF) of byte (flag >> 8) |
| 0x38 | `field_0x0028` | `mSceneFrameCount` | zeldaGFrameWork 02024478 counts up (saturating) while mSceneActive, else 0; seStart 0201EBA0 / startAnimSound 0201B71C test < 30 |
| 0x40 | `field_0x0030` | `mPrevTime` | processTime 020239C8: the previous (hour << 8) + minute, compared against 0x53B / 0x1400 for the morning and night BGM |
| 0x42 | `field_0x0032` | `mMorningBgmPlaying` | processTime 020239C8 sets 1 when it starts JA_BGM_MORNING; processMorningToNormal 02023930 / bgmStop 02021F28 clear; sceneBgmStart 020234A4 / changeSeaBgm 02023A94 return while set |
| 0x43 | `field_0x0033` | `mDayNightSwitched` | seStart 0201EBA0 sets 1 on JA_SE_CHUYA_SWITCH (with JA_BGM_TAKT_CHUYA); sceneBgmStart 020234A4 clears; sceneChange 020264F8 drops the sea BGM by day while set |
| 0x44 | `field_0x0034` | `mLinkHp` | setLinkHp 02027B0C first argument; processHeartGaugeSound 02024334 picks the ALMOST_DIE alarm from it (<= 2 / 4 / 6) |
| 0x48 | `field_0x0038` | `mLinkMaxHp` | setLinkHp 02027B0C second argument (divisor of the hp ratio) |
| 0x4C | `field_0x003c` | `mLinkHpRatio` | setLinkHp 02027B0C: hp / max clamped to 1 (1 when max is 0); drives the battle BGM tempo |
| 0x50 | `field_0x0040` | `mHeartGaugeTimer` | heartGaugeOn 02027D40 sets 2; processHeartGaugeSound 02024334 plays the low-health alarm while nonzero and counts it down |
| 0x51 | `field_0x0041` | `mLinkSwordType` | setLinkSwordType 02027C50 first argument; setLinkShieldType 02027C9C reads it (TWW d_a_player_main.cpp setLinkSwordType callers) |
| 0x52 | `field_0x0042` | `mLinkSwordPlace` | setLinkSwordType 02027C50 second argument (TWW d_a_player_main.cpp callers: 1 = in the hand, 2 = on the back) |
| 0x53 | `field_0x0043` | `mLinkShieldType` | setLinkShieldType 02027C9C first argument (TWW d_a_player_main.cpp: 0 none, 1 shield, 2 mirror shield); setScene 0202677C scene 0xB tests it |
| 0x54 | `field_0x0044` | `mLinkShieldPlace` | setLinkShieldType 02027C9C second argument (TWW d_a_player_main.cpp callers: 1 = in the hand, 2 = on the back) |
| 0x55 | `field_0x0045` | `mLinkSwordShieldOnBack` | setLinkSwordType 02027C50 / setLinkShieldType 02027C9C: 1 when sword and shield are both on the back; seStart 0201EBA0 adds JA_SE_LK_FT_SW_SHIELD to ladder steps |
| 0x56 | `field_0x0046` | `mLinkBootsType` | setLinkBootsType 02027CEC; seStart 0201EBA0 turns ladder steps into JA_SE_LK_WALK_HEAVY when 1 (startAnimSound 0201B71C likewise) |
| 0x57 | `field_0x0047` | `mLinkOnBoard` | setLinkOnBoard 02027CF4; checkLinkOnBoardSea 02025AF4, subBgmStart 0201DE00 (JA_BGM_SEA_ENEMY mute) read it |
| 0x58 | `field_0x0048` | `mCameraOldPos` | getCameraInfo 02027558 copies the previous camera position here (1e7 when none); camera +4 always points here, camera +0 too when no position |
| 0x64 | `field_0x0054` | `mCameraPolygonPos` | setCameraPolygonPos 02027754 copies the position; riverSePlay 0201CDA0 registers it as a river point when the camera map info is 0x18 |
| 0x71 | `field_0x0061` | `mEventTimer` | zeldaGFrameWork 02024478: 5 while dComIfGp_event_runCheck(), else counted down to 0 |
| 0x72 | `field_0x0062` | `mKamomeFlag` | set 1/0 by the seagull (d_a_npc_kamome kam_setAudioFlag, GC interface byte +0x62); changeSeaBgm 02023A94 returns while set; load1stDynamicWave 02026E04 clears |
| 0x73 | `field_0x0063` | `mBgmDisabled` | only cleared by createJAIZelBasic 02025B44; bgmStart 0202204C, subBgmStart 0201DE00, bgmStreamPrepare 02027168, bgmStreamPlay 02027F04, bgmNowBattle 0202879C return while set |
| 0x74 | `field_0x0064` | `mTalkInBlocked` | subBgmStart 0201DE00 (item-get fanfares) and bgmStart 0202204C (0xC0000006) set 1, subBgmStopInner 0201D9C8 / bgmStop 02021F28 clear; talkIn 02027D4C returns while 1 |
| 0x75 | `field_0x0065` | `mMtDragonMuteRequest` | bgmMuteMtDragon 020283D8 sets 1; zeldaGFrameWork 02024478 then mutes JA_BGM_D_RYU_MT (mMainBgmSceneVolume 0) and clears it |
| 0x76 | `field_0x0066` | `mSelectBgmOnly` | setSceneName 02027814 sets 1 for spot 0x75, else 0; bgmStart 0202204C then only starts JA_BGM_SELECT |
| 0x90 | `field_0x0080` | `mMainBgmMuteVolume` | main BGM volume factor: bgmAllMute 0202844C sets 0, subBgmStopInner 0201D9C8 1 (first of the eight factors multiplied in talkIn 02027D4C etc.) |
| 0x94 | `field_0x0084` | `mMainBgmTalkVolume` | main BGM volume factor: talkIn 02027D4C sets VOL_BGM_TALKING, talkOut 0201DB50 VOL_BGM_DEFAULT |
| 0x98 | `field_0x0088` | `mMainBgmMenuVolume` | main BGM volume factor: menuIn 0201E5E0 sets VOL_BGM_PAUSING, menuOut 0201DCD4 VOL_BGM_DEFAULT |
| 0x9C | `field_0x008c` | `mMainBgmSubBgmVolume` | main BGM volume factor: subBgmStart 0201DE00 / bgmNowBattle 0202879C set 0 while a sub BGM plays, subBgmStopInner 0201D9C8 / bgmBattleGFrame 02023E0C 1 |
| 0xA0 | `field_0x0090` | `mMainBgmSceneVolume` | main BGM volume factor set by scene logic: Forest Haven / scene 0x35 distance fades (zeldaGFrameWork 02024478, bgmStart 0202204C), D_RYU_MT mute, sea-game talk (linkVoiceStart 020293CC) |
| 0xA4 | `field_0x0094` | `mMainBgmSeaVolume` | main BGM volume factor: changeSeaBgm 02023A94 fades it by the distance to the island area (calcMainBgmVolWithSeaFactor); processMorningToNormal 02023930 sets 1 |
| 0xA8 | `field_0x0098` | `mMainBgmTaktVolume` | main BGM volume factor: taktModeMute 02028518 sets 0, taktModeMuteOff 0202862C 1; talkIn 02027D4C / bgmNowBattle 0202879C return while 0 |
| 0xAC | `field_0x009c` | `mMainBgmCbPracticeVolume` | main BGM volume factor: cbPracticePlay 02028038 / cbPracticeProcess 02021534 duck it against the practice position volume, 1 when practice stops |
| 0xB0 | `field_0x00a0` | `mSubBgmTalkMenuVolume` | sub BGM volume factor: talkIn 02027D4C VOL_BGM_TALKING, menuIn 0201E5E0 VOL_BGM_PAUSING, talkOut 0201DB50 VOL_BGM_DEFAULT; subBgmStart 0201DE00 setSeqInterVolume |
| 0xB4 | `field_0x00a4` | `mSubBgmTaktVolume` | sub BGM volume factor: taktModeMute 02028518 sets 0, taktModeMuteOff 0202862C 1 |
| 0xB8 | `field_0x00a8` | `mSubBgmCbPracticeVolume` | sub BGM volume factor: cbPracticePlay 02028038 / cbPracticeProcess 02021534 set the position volume of the practice sound |
| 0xBC | `field_0x00ac` | `mMainBgmHeightVolume` | main BGM volume factor: zeldaGFrameWork 02024478 fades it by camera height (Outset, Dragon Roost, scene 0x19) |
| 0xC0 | `field_0x00b0` | `mSubBgmStopTimer` | subBgmStop 0201D8BC sets 0xF; zeldaGFrameWork 02024478 counts down and calls subBgmStopInner at 0; -1 idle (subBgmStopInner 0201D9C8), -2 from seStart 0201EBA0 |
| 0xC7 | `field_0x00b7` | `mSkyCloistersTimer` | stSkyCloisters 020283E4 sets 4 (JA_BGM_UNK_126); stSkyCloistersProcess 02024278 counts down and restarts JA_BGM_KAMI_TOWER at 1; stWaterLevelUp 020284B0 waits for 0 |
| 0xC8 | `field_0x00b8` | `mStreamBgmStarted` | bgmStreamPlay 02027F04 sets 1, bgmStop 02021F28 clears; linkVoiceStart 020293CC skips voice 0x10 during JA_STRM_DEMO_GO_MAJU while set |
| 0xC9 | `field_0x00b9` | `mDemoFanfareMuted` | processDemoFanfareMute 02021DC4: 1 while a demo fanfare plays (muteMainBgmAll on the rising edge, unmuteMainBgmAll on the falling edge) |
| 0xCA | `field_0x00ba` | `mTetraFlyStreamStarted` | bgmStreamPlay 02027F04 sets 1 for JA_STRM_DEMO_TETRA_FLY; initSe 02025760 clears |
| 0xCB | `field_0x00bb` | `mFanfareResumePending` | subBgmStart 0201DE00 sets 2 for item-get fanfares; zeldaGFrameWork 02024478 restarts the miniboss sub BGM (mLastMinibossSubBGMType) when the fanfare ends and clears it |
| 0xCC | `field_0x00bc` | `mPendingBgmStart` | zeldaGFrameWork 02024478: once no BGM plays, 1 starts the sea BGM, 2 sceneBgmStart, 3 the island BGM, then clears |
| 0xCD | `field_0x00bd` | `mSceneBgmLocked` | setScene 0202677C sets 1 at the Flight Control Platform layers 2/3 (else 0); changeSeaBgm 02023A94 / setCameraGroupInfo 02027670 return while set |
| 0xCE | `field_0x00be` | `mTalkSeBlocked` | seStart 0201EBA0 (JA_SE_CM_BST_GOKOU) / bgmStreamPrepare 02027168 (JA_STRM_BST_START) set 1, bgmStart / subBgmStart clear; seStart drops the talk-window SEs while set |
| 0xCF | `field_0x00bf` | `mCameraGroupLocked` | mDoAud_setSpecialBgmFlag 025E201C sets it (d_a_npc_so_cut mini game); setCameraGroupInfo 02027670 returns while set; resetRecover 0202736C clears |
| 0xD0 | `field_0x00c0` | `mBattleBgmFadeDelay` | bgmNowBattle 0202879C sets COMMON_BATTLE_FO_DELAY; bgmBattleGFrame 02023E0C counts down and stops the battle BGM at 0 |
| 0xD1 | `field_0x00c1` | `mEnemyNearbyOn` | bgmNowBattle 0202879C / mbossBgmNearByProcess 02025940 set 1 when the enemy-nearby track is faded in, 0 when it fades out |
| 0xD4 | `field_0x00c4` | `mEnemyNearbyFadeTimer` | bgmNowBattle 0202879C / mbossBgmNearByProcess 02025940 set ENEMY_NEARBY_FADEOUT_TIME; bgmBattleGFrame 02023E0C / mbossBgmMuteProcess 02024018 fade the track out at 1 |
| 0xD8 | `field_0x00c8` | `mKaitengiriTimer` | bgmNowKaitengiri 02028D6C sets 2 (fades the spin-attack track in); bgmBattleGFrame 02023E0C / mbossBgmMuteProcess 02024018 fade it out once counted down |
| 0xD9 | `field_0x00c9` | `mSwordUsing` | bgmSetSwordUsing 02028FE0; bgmNowBattle 0202879C / mbossBgmMuteProcess 02024018 pick the SWORD_USING / SWORD_NOT_USING tracks |
| 0xDA | `field_0x00ca` | `mBattleBgmDisabled` | bgmStart 0202204C (JA_BGM_DIE_LINK) / seStart 0201EBA0 (JA_SE_OBJ_OTOSHIANA_OPEN) set 1, sceneBgmStart 020234A4 clears; bgmNowBattle 0202879C returns while 1 |
| 0xDB | `field_0x00cb` | `mEnemyNearbyDelay` | enemyNearBy 020259E8 sets COMMON_BATTLE_FO_DELAY; enemyNearByGFrame 02024190 counts down and restarts JA_BGM_DK_BATTLE / JA_BGM_MJ_TOWER_BATTLE at 0; -1 idle |
| 0xDC | `field_0x00cc` | `mDkMaskBroken` | seStart 0201EBA0 sets 1 on JA_SE_CM_DK_BREAK_MASK, bgmStop 02021F28 clears; enemyNearBy 020259E8 / enemyNearByGFrame 02024190 pick the JA_BGM_DK_BATTLE variant |
| 0xDD | `field_0x00cd` | `mSkipSceneStream` | setScene 0202677C sets 1 when isStageItem(.., 5); load2ndDynamicWave 020271E4 then does not prepare the scene's start stream |
| 0xDE | `field_0x00ce` | `mBattleBgmHoldTimer` | subBgmStop 0201D8BC / subBgmStopInner 0201D9C8 set 0x14 after a miniboss sub BGM; zeldaGFrameWork 02024478 counts down; bgmNowBattle 0202879C returns while nonzero |
| 0xE0 | `field_0x00d0` | `mpCbPracticePos` | cbPracticePlay 02028038 stores the position; cbPracticeProcess 02021534 positions the practice sound from it (returns when NULL) |
| 0x1E4 | `field_0x0194` | `mSePosAddr` | seStart 0201EBA0 stores the position address per SE slot and restarts the slot whose id and position match; seStop 0201E538 clears |
| 0x264 | `field_0x01f4` | `mSeSlotIndex` | seStart 0201EBA0: rotating index of the next free mpSeSound slot (mod 32) |
| 0x268 | `field_0x01f8` | `mNoCameraTimer` | getCameraInfo 02027558 sets 10 when there is no camera position, else counts down; seaEnvSePlay 0201C524, riverSePlay 0201CDA0, changeSeaBgm 02023A94 return while set |
| 0x269 | `field_0x01f9` | `mAddGrassSePlayed` | seStart 0201EBA0: JA_SE_FT_ADD_GRASS once per frame (zeldaGFrameWork 02024478 clears) |
| 0x26A | `field_0x01fa` | `mCutGrassSeTimer` | seStart 0201EBA0: JA_SE_LK_CUT_GRASS at most every 4 frames (zeldaGFrameWork 02024478 counts down) |
| 0x26B | `field_0x01fb` | `mBrkWrailKSePlayed` | seStart 0201EBA0: JA_SE_OBJ_BRK_WRAIL_K once per frame (zeldaGFrameWork 02024478 clears) |
| 0x26C | `field_0x01fc` | `mColBrkWrailingSePlayed` | seStart 0201EBA0: JA_SE_OBJ_COL_BRK_WRAILING once per frame (zeldaGFrameWork 02024478 clears) |
| 0x26D | `field_0x01fd` | `mBgnStringRippleSeTimer` | seStart 0201EBA0: JA_SE_CM_BGN_STRING_RIPPLE at most every 0x14 frames (zeldaGFrameWork 02024478 counts down) |
| 0x26E | `field_0x01fe` | `mBgnBodyRippleSeTimer` | seStart 0201EBA0: JA_SE_CM_BGN_BODY_RIPPLE at most every 0x14 frames (zeldaGFrameWork 02024478 counts down) |
| 0x26F | `field_0x01ff` | `mBgnHeadRippleSeTimer` | seStart 0201EBA0: JA_SE_CM_BGN_M_HEAD_RIPPLE at most every 0x14 frames (zeldaGFrameWork 02024478 counts down) |
| 0x270 | `field_0x0200` | `mBgnTailRippleSeTimer` | seStart 0201EBA0: JA_SE_CM_BGN_M_TAIL_RIPPL at most every 0x14 frames (zeldaGFrameWork 02024478 counts down) |
| 0x271 | `field_0x0201` | `mAllSeMuted` | bgmStreamPlay 02027F04 sets 1 for JA_STRM_DEMO_TETRA_FLY, bgmStop 02021F28 / resetRecover 0202736C clear; seStart 0201EBA0 and the ambience players return while 1 |
| 0x272 | `field_0x0202` | `mSwingSeTimers` | seStart 0201EBA0: [0] JA_SE_OBJ_TREE_SWING(_S), [1] JA_SE_OBJ_COL_SWC_CHNS at most every 2 frames (zeldaGFrameWork 02024478 counts down) |
| 0x274 | `field_0x0204` | `mSeaEnvSeOff` | seStart 0201EBA0 (JA_SE_ATM_MAGMA_CHILL) / setScene 0202677C (scene 2 with stage item 3) set 1, load1stDynamicWave 02026E04 clears; seaEnvSePlay 0201C524 returns while 1 |
| 0x275 | `field_0x0205` | `mForceBack` | seStart 0201EBA0 sets 1 on JA_SE_FORCE_BACK, sceneBgmStart 020234A4 clears; zeldaGFrameWork 02024478 keeps the Forest Haven distance volume while set |
| 0x276 | `field_0x0206` | `mFanfareActive` | subBgmStart 0201DE00 sets 1 for item-get / open-box fanfares, cleared when they end; seStart 0201EBA0 then plays only system SEs; ambience, alarm, battle BGM wait |
| 0x277 | `field_0x0207` | `mTaktSeTimer` | seStart 0201EBA0 sets 2 on JA_SE_WTAKT_AMBIENT (zeldaGFrameWork 02024478 counts down); while set only the Wind Waker SEs play, talk/level-object/waterfall SEs and battle BGM wait |
| 0x278 | `field_0x0208` | `mPirateSailSePlayed` | seStart 0201EBA0: JA_SE_OBJ_PIRATE_SAIL on Windfall layer 0xA only once (unless stream 0x17 plays); load1stDynamicWave 02026E04 clears |
| 0x284 | `field_0x0214` | `mpTelescopeSound` | seStart 0201EBA0: handle of JA_SE_TELESCOPE_ZOOM (pitch and volume set from the zoom) |
| 0x288 | `field_0x0218` | `mCameraMapInfo` | getCameraMapInfo 02027668 stores it; riverSePlay 0201CDA0 tests 0x18 (JAIZelAtmos.h reads the sea-env group from its low byte) |
| 0x290 | `field_0x0220` | `mNextSceneNo` | setScene 0202677C stores the scene; load1stDynamicWave 02026E04 copies it to mSceneNo |
| 0x294 | `field_0x0224` | `mSceneNo` | load1stDynamicWave 02026E04 copies mNextSceneNo; tested against scene numbers throughout (seStart 0201EBA0, zeldaGFrameWork 02024478, bgmNowBattle 0202879C, ...) |
| 0x29C | `field_0x022c` | `mSceneSetPending` | setScene 0202677C sets 1 (returns while set), load1stDynamicWave 02026E04 clears |
| 0x29D | `field_0x022d` | `mSceneActive` | sceneBgmStart 020234A4 sets 1, setScene 0202677C / load1stDynamicWave 02026E04 / resetProcess 020263C0 clear; gates sea BGM, shore SE, Link voice, battle BGM, mSceneFrameCount |
| 0x29E | `field_0x022e` | `mSceneBgmChanged` | sceneChange 020264F8: 1 when the next scene BGM differs from the playing one (stops the BGM); sceneBgmStart 020234A4 starts mNextSceneBgmId only then, and clears it |
| 0x29F | `field_0x022f` | `mWaveToReload` | load1stDynamicWave 02026E04 loads this wave when mWaveBankReload is set |
| 0x2A0 | `field_0x0230` | `mWaveBankToErase` | load1stDynamicWave 02026E04 erases this wave bank and heap (eraseWaveBank / eraseWaveHeap / resetWaveBank) when mWaveBankReload is set |
| 0x2A1 | `field_0x0231` | `mWaveBankReload` | load1stDynamicWave 02026E04 erases mWaveBankToErase and loads mWaveToReload while set; sceneChange 020264F8 clears |
| 0x2A3 | `field_0x0233` | `mPrevFirstDynamicSceneWaveIndex` | sceneChange 020264F8 saves mFirstDynamicSceneWaveIndex here before replacing it; load1stDynamicWave 02026E04 unloads its banks |
| 0x2A6 | `field_0x0236` | `mPrevSecondDynamicSceneWaveIndex` | sceneChange 020264F8 saves mSecondDynamicSceneWaveIndex here before replacing it; load1stDynamicWave 02026E04 unloads its banks |
| 0x2A7 | `field_0x0237` | `mSecondDynamicWaveChanged` | sceneChange 020264F8 sets 1 when the 2nd wave set changes; load1stDynamicWave 02026E04 / load2ndDynamicWave 020271E4 reload the 2nd banks while set |
| 0x2A9 | `field_0x0239` | `mLayerNo` | setScene 0202677C stores the layer argument; seStart 0201EBA0 / startAnimSound 0201B71C test it with the scene and island room |
| 0x2AA | `field_0x023a` | `mInitialized` | init 02026044 sets 1 at the end of the audio setup |
| 0x1BF0 | `field_0x1b80` | `mSeaEnvPosCount` | initSeaEnvPos 0201C4B8 / registSeaEnvPos 0201C4C4 (max 0x3F) / seaEnvSePlay 0201C524: number of registered sea ambience points |
| 0x1E40 | `field_0x1dd0` | `mRiverPosCount` | initRiverPos 0201CC44 / registRiverPos 0201CC50 (max 0x2F) / riverSePlay 0201CDA0: number of registered river points |
| 0x1E44 | `field_0x1dd4` | `mBstHoriSeOn` | riverSePlay 0201CDA0: river type 3 plays JA_SE_CM_BST_HORI only while set; load1stDynamicWave 02026E04 clears |
| 0x1F08 | `field_0x1e98` | `mWaterfallPosCount` | waterfallSePlay 0201CF54: index of the next mWaterfallPos entry (max 0xF), zeldaGFrameWork 02024478 resets it each frame |
| 0x1F30 | `field_0x1ec0` | `mWindowPosCount` | initWindowPos 0201D140 / registWindowPos 0201D14C (max 2) / rainPlay 0201D1AC: number of registered window points |
| 0x1F34 | `field_0x1ec4` | `mpSeaEnvSoundL` | seaEnvSePlay 0201C524: handle of the sea ambience played at pan 1.0 with the left volume |
| 0x1F38 | `field_0x1ec8` | `mpSeaEnvSoundR` | seaEnvSePlay 0201C524: handle of the sea ambience played at pan 0.0 with the right volume |
| 0x1F3C | `field_0x1ecc` | `mpSeaEnvSoundS` | seaEnvSePlay 0201C524: handle of the sea ambience played at pan 0.5, dolby 1.0 with the surround volume |
| 0x1F40 | `field_0x1ed0` | `mpSeaShoreSound` | seaShoreSE 0201CA10: handle of the JA_SE_SEASHORE_* sound at mSeaShorePos |
| 0x1F44 | `field_0x1ed4` | `mFireblastPos` | seStart 0201EBA0: JA_SE_FIREBLAST_BLOW stores a height-clamped copy of each position here (mFireblastPosCount entries) and plays it from the copy |
| 0x1FA4 | `field_0x1f34` | `mFireblastPosCount` | seStart 0201EBA0: next mFireblastPos entry; zeldaGFrameWork 02024478 resets it each frame |
| 0x1FA8 | `field_0x1f38` | `mpWindSound` | zeldaGFrameWork 02024478: handle of JA_SE_ATM_WIND_I_LINK / JA_SE_ATM_WIND_MJT, volume by camera height |
| 0x1FAC | `field_0x1f3c` | `mIslandLinkBgm` | bgmStart 0202204C sets 1 for JA_BGM_ISLAND_LINK(_0/_2); bgmStop 02021F28, bgmStreamPlay 02027F04, load1stDynamicWave 02026E04 clear |
| 0x1FB0 | `field_0x1f40` | `mShipSpeed` | shipCruiseSePlay 02029638 stores the speed argument; bgmStart 0202204C mutes the sea BGM track when not sailing and below 0.2 |
| 0x1FB4 | `field_0x1f44` | `mpShipCruiseSound` | shipCruiseSePlay 02029638: handle of JA_SE_SHIP_GO_NORMAL |
| 0x1FB8 | `field_0x1f48` | `mpShipWaveSound` | shipCruiseSePlay 02029638: handle of JA_SE_SHIP_WAVE_S (or the second wave sound) |
| 0x1FBC | `field_0x1f4c` | `mMonsSe` | monsSeStart 020299D8: 30 {object, sound handle} slots for monster sounds; monsSeInit 0202573C resets them |
| 0x20AC | `field_0x203c` | `mKuroboMotionCount` | kuroboMotionPlay 0201D57C: at most 4 motion sounds per frame (zeldaGFrameWork 02024478 resets) |
| 0x20AD | `field_0x203d` | `mKuroboVoiceCount` | kuroboVoicePlay 0202587C: at most 4 voices per frame (zeldaGFrameWork 02024478 resets) |
| 0x20B0 | `field_0x2040` | `mpKuroboMotionSound` | kuroboMotionPlay 0201D57C: 4 handles, the first free one is used |
| 0x20C0 | `field_0x2050` | `mpKuroboVoiceSound` | kuroboVoicePlay 0202587C: 4 handles, the first free one is used |
| 0x20D0 | `field_0x2060` | `mpCharVoiceSound` | charVoicePlay 02029074: handle of JA_SE_CV_COMMON_PEOPLE (ports 8 and 9 set) |
| 0x20E4 | `field_0x2074` | `mSystemSoundHD` | same slot (+0x20E4) as JAIZelBasic::mSystemSoundHD in JAZelAudio/JAIZelBasic.h; initSe 02025760 clears it |

Open (only initialised; no use that shows a meaning): `field_0x002f` (+0x3F), `field_0x00b4` (+0xC4), `field_0x020c`
(+0x27C), `field_0x0210` (+0x280) (createJAIZelBasic 02025B44 / initSe 02025760 zero them); `field_0x021e` (+0x28E: set
by load1stDynamicWave 02026E04, cleared by zeldaGFrameWork 02024478 and setCameraGroupInfo 02027670, never read).
Comments in d_a_npc_kamome.{h,cpp} and d_a_npc_so_cut.cpp quote the GameCube code (`getInterface()->field_0x0062`,
`zel_basic->field_0x00bf`) and are left as they are: they are now `mKamomeFlag` and `mCameraGroupLocked`.

## JAZelAudio helper structs (JAZelAudio/*.h)

Views of JSystem classes follow the names already in this decomp (JAInter::Camera in JSystem/JAudio/JAIBasic.h,
JAISound and JAInter::SePositionInfo in JSystem/JAudio/JAISound.h, JMath::TRandom_enough_ in JSystem/JMath/random.h).
Old names are the GameCube names (zeldaret/tww, which has none for these).

| Struct (header) | Old | New | Evidence |
|---|---|---|---|
| AudioCamera (JAIZelBasic.h, _3, _5, _6, _7, _8 (view of JAInter::Camera)) | `field_0x0` | `position` | JAInter::Camera::position (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); getCameraInfo 02027558 stores the camera position (or mCameraOldPos) |
| AudioCamera (JAIZelBasic.h, _3, _5, _6, _7, _8 (view of JAInter::Camera)) | `field_0x4` | `previousPosition` | JAInter::Camera::previousPosition (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); getCameraInfo 02027558 points it at mCameraOldPos |
| AudioCamera (JAIZelBasic.h, _3, _5, _6, _7, _8 (view of JAInter::Camera)) | `field_0x8` | `matrix` | JAInter::Camera::matrix (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); getCameraInfo 02027558 stores the camera matrix |
| AtmosCamera (JAIZelAtmos.h (view of JAInter::Camera)) | `field_0x0` | `position` | JAInter::Camera::position (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); seaEnvSePlay 0201C524 places the four sea points around it |
| AtmosCamera (JAIZelAtmos.h (view of JAInter::Camera)) | `field_0x8` | `matrix` | JAInter::Camera::matrix (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); seaEnvSePlay 0201C524 / waterfallSePlay 0201CF54 transform by it |
| JAIZelCameraView (JAIZelAnime.h (view of JAInter::Camera)) | `field_0x8` | `matrix` | JAInter::Camera::matrix (JSystem/JAudio/JAIBasic.h; the base class JAIBasic's mAudioCamera); startAnimSound 0201B71C transforms the actor position by it |
| MonsterSlot (JAIZelBasic.h, _7, _8) | `field_0x00` | `mActorId` | monsSeStart 020299D8: the actor id argument (fopAcM_monsSeStart passes the actor's id) owning the slot; -1 free (monsSeInit 0202573C) |
| MonsterSlot (JAIZelBasic.h, _7, _8) | `field_0x04` | `mpSound` | monsSeStart 020299D8: handle of the slot's sound (stopped when the actor starts a new one) |
| Random (JAIZelBasic.h, _7, _8 (view of JMath::TRandom_enough_)) | `field_0x0` | `mSeed` | JMath::TRandom_enough_::mSeed (JSystem/JMath/random.h): mRandomHD is a TRandom_enough_ (createJAIZelBasic seeds it, 0x88 bytes) |
| Random (JAIZelBasic.h, _7, _8 (view of JMath::TRandom_enough_)) | `field_0x4` | `mValue` | JMath::TRandom_enough_::mValue (JSystem/JMath/random.h) |
| Random (JAIZelBasic.h, _7, _8 (view of JMath::TRandom_enough_)) | `field_0x8` | `mShuffle` | JMath::TRandom_enough_::mShuffle (JSystem/JMath/random.h) |
| SoundHead (JAIZelBasic.h, _7 (view of JAISound)) | `field_0x4` | `track` | JAISound::track (JSystem/JAudio/JAISound.h; byte 0 of the sound handle, GC +4) |
| SoundHeader (JAIZelBasic_3.h (view of JAISound)) | `field_0x4` | `track` | JAISound::track (JSystem/JAudio/JAISound.h; byte 0 of the sound handle, GC +4) |
| scene_info_s (JAIZelBasic_5/_6/_7.h) | `field_0x2` | `dyWave1st` | setScene 0202677C passes it to sceneChange 020264F8 as the m_dy_wave_set_1st index (mFirstDynamicSceneWaveIndex) |
| scene_info_s (JAIZelBasic_5/_6/_7.h) | `field_0x3` | `dyWave2nd` | setScene 0202677C passes it to sceneChange 020264F8 as the m_dy_wave_set_2nd index (mSecondDynamicSceneWaveIndex) |
| isle_area_s (JAIZelBasic_5.h) | `field_0x0` | `centerX` | checkOnOuterSea 02023670: x of the island area centre in its 100000-unit sea room (compared with the camera x; > 50000 means none) |
| isle_area_s (JAIZelBasic_5.h) | `field_0x8` | `centerZ` | checkOnOuterSea 02023670: z of the island area centre in its sea room (compared with the camera z) |
| isle_area_s (JAIZelBasic_5.h) | `field_0xC` | `radius` | checkOnOuterSea 02023670 / changeSeaBgm 02023A94: area radius (+2000 / 3000 / 4000 rings fade the sea BGM) |
| JAIZelInst (JAIZelInst.h) | `field_0x18` | `mJudgedNotes` | judge 0202A3B4: the last six conducted notes (cposNote), shifted when full and matched against mMelodyPattern |
| JAIZelInst (JAIZelInst.h) | `field_0x1e` | `mJudgedNoteCount` | judge 0202A3B4: number of entries in mJudgedNotes (0 at beat 0, max 6) |
| JAIZelInst (JAIZelInst.h) | `field_0x20` | `mVolume` | setVolume 0202A14C: 0.55 .. 1.0 from the stick; volume of every conducting SE (metronomePlay, judge, ambientPlay) |
| JAIZelInst (JAIZelInst.h) | `field_0x2c` | `mBeatRate` | setBeat 0202A040: 1.0 / 0.75 / 0.857 for 3 / 6 / 4 beats; mBeatFrames = 30 * it |
| JAIZelInst (JAIZelInst.h) | `field_0x34` | `mArmNote` | armSoundPlay 0202A87C stores cposNote(mBeat, cpos); reset 02029F10 sets 0x37 |
| JAIZelInst (JAIZelInst.h) | `field_0x3c` | `mpMetronomeSound` | judge 0202A3B4: handle of JA_SE_WTAKT_METRONOME; ambientPlay 0202A77C sets its volume |
| JAIZelInst (JAIZelInst.h) | `field_0x40` | `mpDroneSound` | metronomePlay 0202A1B8: handle of JA_SE_WTAKT_DRONE(_4/_6) / JA_SE_WTAKT_AUFTAKT(_4/_6); ambientPlay 0202A77C sets its volume |
| JAIZelInst (JAIZelInst.h) | `field_0x44` | `mpPulseSound` | judge 0202A3B4: handle of JA_SE_WTAKT_PULSE_3/4/6; ambientPlay 0202A77C sets its volume |
| JAIZelInst (JAIZelInst.h) | `field_0x48` | `mUseNote62` | judge 0202A3B4 sets it for a 6 beat starting at cpos 1; cposNote then takes mCPosToNote62 instead of mCPosToNote61 |
| PositionInfo_t (JAIZelSound.h, JAISound::PositionInfo_t (view of JAInter::SePositionInfo)) | `field_0x0` | `currentX` | JAInter::SePositionInfo::current.x (JSystem/JAudio/JAISound.h) |
| PositionInfo_t (JAIZelSound.h, JAISound::PositionInfo_t (view of JAInter::SePositionInfo)) | `field_0x8` | `currentZ` | JAInter::SePositionInfo::current.z (JSystem/JAudio/JAISound.h); setSeDistanceDolby 0202B3C8 dolby from it |
| PositionInfo_t (JAIZelSound.h, JAISound::PositionInfo_t (view of JAInter::SePositionInfo)) | `field_0x18` | `distance` | JAInter::SePositionInfo::distance (JSystem/JAudio/JAISound.h); setDistanceVolumeCommon 0202AD20 reads it per camera |
| JAISound (JAIZelSound.h (view of JAISound)) | `field_0x8` | `cameraIndex` | JAISound::cameraIndex (JSystem/JAudio/JAISound.h, +4); setDistanceVolumeCommon 0202AD20 indexes mPositionInfo with it (4 = nearest camera) |
| JAISound (JAIZelSound.h (view of JAISound)) | `field_0x18` | `frameCounter` | JAISound::frameCounter (JSystem/JAudio/JAISound.h, +0x14); setSeDistance* skip after frame 1 when the sw bit says so |
| JAISound (JAIZelSound.h (view of JAISound)) | `field_0x28` | `position` | JAISound::position (JSystem/JAudio/JAISound.h, +0x24); setSeDistanceDolby 0202B3C8 returns dolby 0 when NULL |

Open: isle_area_s `field_0x4` (never read); JAIZelInst `field_0x24`, `field_0x28` (setBeat 0202A040 / reset 02029F10 only write them: 3 / 4.5 / 3.43 and 60 / 80 / 70 for 3 / 6 / 4 beats) and `field_0x38` (a sound handle ambientPlay 0202A77C adjusts, set nowhere). JAInter::Actor's offset-name union aliases in JAIZelAnime.h belong to the JAudio batch.

## JAudio: DSP interface, outer parameters, oscillator (JSystem/JAudio)

zeldaret/tww (d217e2b, libs/JSystem/include/JSystem/JAudio/JASDSPInterface.h, JASOscillator.h) has no names for these;
names marked "after TP" are the same fields in zeldaret/tp (c8fa8c9e, libs/JSystem/include/JSystem/JAudio2/
JASDSPInterface.h JASDsp::TChannel, JASWaveInfo.h, JASOscillator.h), matched by offset and checked against the WWHD use.

| Struct (header) | Old | New | Evidence |
|---|---|---|---|
| DSPBuffer (JASDSPInterface.h) | `field_0x0` | `mIsActive` | after TP JASDsp::TChannel::mIsActive; playStart 02815E3C sets 1, stop 02815EB4 / acknowledge 0 |
| DSPBuffer (JASDSPInterface.h) | `field_0x2` | `mIsFinished` | after TP JASDsp::TChannel::mIsFinished; isFinished 02815EDC returns it, acknowledge 02815ECC clears |
| DSPBuffer (JASDSPInterface.h) | `field_0x4` | `mPitch` | after TP JASDsp::TChannel::mPitch; setPitch 0281608C |
| DSPBuffer (JASDSPInterface.h) | `field_0xc` | `mPauseFlag` | after TP JASDsp::TChannel::mPauseFlag; setPauseFlag 02816118 |
| DSPBuffer (JASDSPInterface.h) | `field_0xe` | `mMixerInitDelayMax` | setMixerInitDelayMax 028160A0 stores it (TP: unused) |
| DSPBuffer (JASDSPInterface.h) | `field_0x10` | `mOutputChannels` | after TP JASDsp::TChannel::mOutputChannels (per output: bus connect, target volume, current volume, delay); setMixerInitVolume 028160A8 / setMixerDelay 02816100 / setBusConnect 0281619C index it |
| DSPBuffer (JASDSPInterface.h) | `field_0x50` | `mAutoMixerPanDolby` | after TP JASDsp::TChannel::mAutoMixerPanDolby; setAutoMixer 02816064 (high << 8 | low) |
| DSPBuffer (JASDSPInterface.h) | `field_0x52` | `mAutoMixerFxMix` | after TP JASDsp::TChannel::mAutoMixerFxMix; setAutoMixer 02816064 |
| DSPBuffer (JASDSPInterface.h) | `field_0x54` | `mAutoMixerInitVolume` | after TP JASDsp::TChannel::mAutoMixerInitVolume; initAutoMixer 02816038 copies mAutoMixerVolume or clears it |
| DSPBuffer (JASDSPInterface.h) | `field_0x56` | `mAutoMixerVolume` | after TP JASDsp::TChannel::mAutoMixerVolume; setAutoMixer 02816064 |
| DSPBuffer (JASDSPInterface.h) | `field_0x58` | `mAutoMixerBeenSet` | after TP JASDsp::TChannel::mAutoMixerBeenSet; initAutoMixer 02816038 sets 1, allocInit clears |
| DSPBuffer (JASDSPInterface.h) | `field_0x5a` | `mWaveReady` | markWaveReady 028161B8 sets 1, setWaveInfo 02815EEC / allocInit clear |
| DSPBuffer (JASDSPInterface.h) | `field_0x64` | `mSamplesPerBlock` | after TP JASDsp::TChannel::mSamplesPerBlock; setWaveInfo 02815EEC from COMP_BLOCKSAMPLES[format], setOscInfo 16 |
| DSPBuffer (JASDSPInterface.h) | `field_0x68` | `mSamplePosition` | after TP JASDsp::TChannel::mSamplePosition; setWaveInfo 02815EEC stores the start offset |
| DSPBuffer (JASDSPInterface.h) | `field_0x100` | `mBytesPerBlock` | after TP JASDsp::TChannel::mBytesPerBlock; setWaveInfo 02815EEC from COMP_BLOCKBYTES[format] |
| DSPBuffer (JASDSPInterface.h) | `field_0x102` | `mLoopFlag` | setWaveInfo 02815EEC stores the wave's mLoopFlag |
| DSPBuffer (JASDSPInterface.h) | `field_0x104` | `mpLast` | after TP JASDsp::TChannel::mpLast; setWaveInfo 02815EEC from the wave's ADPCM history |
| DSPBuffer (JASDSPInterface.h) | `field_0x106` | `mpPenult` | after TP JASDsp::TChannel::mpPenult; setWaveInfo 02815EEC from the wave's ADPCM history |
| DSPBuffer (JASDSPInterface.h) | `field_0x108` | `mFilterMode` | after TP JASDsp::TChannel::mFilterMode; setFilterMode 02816120 |
| DSPBuffer (JASDSPInterface.h) | `field_0x10a` | `mForcedStop` | after TP JASDsp::TChannel::mForcedStop; forceStop 02815EC0 sets 1; setMixerVolumeOnly 028160DC tests it |
| DSPBuffer (JASDSPInterface.h) | `field_0x110` | `mLoopStartSample` | after TP JASDsp::TChannel::mLoopStartSample; setWaveInfo 02815EEC |
| DSPBuffer (JASDSPInterface.h) | `field_0x114` | `mEndSample` | after TP JASDsp::TChannel::mEndSample; setWaveInfo 02815EEC (loop end or sample count) |
| DSPBuffer (JASDSPInterface.h) | `field_0x118` | `mWaveAramAddress` | after TP JASDsp::TChannel::mWaveAramAddress; setWaveInfo 02815EEC stores the address argument |
| DSPBuffer (JASDSPInterface.h) | `field_0x11c` | `mSampleCount` | after TP JASDsp::TChannel::mSampleCount; setWaveInfo 02815EEC |
| DSPBuffer (JASDSPInterface.h) | `field_0x120` | `mFirFilterParams` | after TP JASDsp::TChannel::fir_filter_params; initFilter 02815DCC / setFIR8FilterParam 02816188 |
| Wave_ (JASDSPInterface.h, Driver::Wave_) | `field_0x1` | `mWaveFormat` | after TP JASWaveInfo::mWaveFormat (HD: at +0); setWaveInfo 02815EEC indexes COMP_BLOCKSAMPLES / COMP_BLOCKBYTES with it |
| Wave_ (JASDSPInterface.h, Driver::Wave_) | `field_0x14` | `mLoopStartSample` | after TP JASWaveInfo::mLoopStartSample; setWaveInfo 02815EEC |
| Wave_ (JASDSPInterface.h, Driver::Wave_) | `field_0x18` | `mLoopEndSample` | after TP JASWaveInfo::mLoopEndSample; setWaveInfo 02815EEC |
| Wave_ (JASDSPInterface.h, Driver::Wave_) | `field_0x1c` | `mSampleCount` | after TP JASWaveInfo::mSampleCount; setWaveInfo 02815EEC |
| Wave_ (JASDSPInterface.h, Driver::Wave_) | `field_0x20` | `mpLast` | after TP JASWaveInfo::mpLast; setWaveInfo 02815EEC |
| Wave_ (JASDSPInterface.h, Driver::Wave_) | `field_0x22` | `mpPenult` | after TP JASWaveInfo::mpPenult; setWaveInfo 02815EEC |
| FXBuffer (JASDSPInterface.h) | `field_0x0` | `mActive` | setFXLine 02816254: the line config's mActive when a buffer is set, else 0 |
| FXBuffer (JASDSPInterface.h) | `field_0x2` | `mBufferBlocks` | setFXLine 02816254: the config's mBufferBlocks (buffer bytes = blocks * 160) |
| FXBuffer (JASDSPInterface.h) | `field_0x4` | `mpBuffer` | setFXLine 02816254: the FX line's sample buffer (kept when no new one is given) |
| FXBuffer (JASDSPInterface.h) | `field_0x10` | `mFilterParams` | setFXLine 02816254: setFilterTable of the config's 8 filter words |
| FxlineConfig_ (JASDSPInterface.h) | `field_0x0` | `mActive` | setFXLine 02816254 copies it to the FX buffer's mActive when a buffer is set |
| FxlineConfig_ (JASDSPInterface.h) | `field_0xc` | `mBufferBlocks` | setFXLine 02816254: buffer size = it * 160 bytes; copied to the FX buffer |
| FxlineConfig_ (JASDSPInterface.h) | `field_0x10` | `mFilterParams` | setFXLine 02816254: 8 filter words copied with setFilterTable |
| TOuterParam (JASOuterParam.h) | `field_0x0` | `mOuterSwitch` | setOuterSwitch 02816C68 / checkOuterSwitch 02816C70: the OUTERPARAM_* flags that are switched on |
| TOuterParam (JASOuterParam.h) | `field_0x2` | `mOuterUpdate` | setOuterUpdate 02816C84 / getOuterUpdate 02816C8C: the OUTERPARAM_* flags to update |
| TOuterParam (JASOuterParam.h) | `field_0x1c` | `mFirFilter` | setFirFilter 02816D7C / getIntFirFilter 02816C94: 8 FIR8 filter words; initExtBuffer 02816C58 clears |
| TOscillator (JASOscillator.h) | `field_0x6` | `mCurPoint` | after TP JASOscillator::mCurPoint; calc 02816480: index of the current envelope table point, stepped per point |
| TOscillator (JASOscillator.h) | `field_0x5` | `mEnvelopeMode` | after TP JASOscillator::mEnvelopeMode; calc 02816480: curve of the current point, from the table / the direct release (>> 14 & 3) |

Open (only cleared, or values whose meaning the code does not show): DSPBuffer `field_0x8`, `field_0x60`, `field_0x66`,
`field_0x78`, `field_0x80`, `field_0xa8`, `field_0xb0`, `field_0xd0`, `field_0x10c` (TP leaves these unnamed too);
FXBuffer `field_0x8`, `field_0xa`, `field_0xc`, `field_0xe` and FxlineConfig_ `field_0x2`, `field_0x4`, `field_0x6`, `field_0x8`
(send levels / indices copied by setFXLine 02816254). TOscillator::Osc_ and TRegisterParam keep their offset-name union
aliases next to named members for now (removing an alias is not a pure rename; to be asked).

## JStudio, JAudio parameter view, m_Do old-frame blend

zeldaret/tww d217e2b keeps GameCube offset names for these (libs/JSystem/include/JSystem/JStudio/JStudio/jstudio-object.h
TVariableValue, JStudio_JParticle/object-particle.h; include/m_Do/m_Do_ext.h mDoExt_MtxCalcOldFrame); zeldaret/tp c8fa8c9e
likewise. Names come from the WWHD uses or from this decomp's own names for the same bytes.

| Struct (header) | Old | New | Evidence |
|---|---|---|---|
| TVariableValue (JSystem/JStudio/JST_timeline.h) | `field_0x4` | `mAge` | update_time_ 02839DB4 / update_functionValue_ 02839DF4 evaluate at age * seconds; TAdaptor::adaptor_updateVariableValue 0283A48C adds the frames (saturating); every adaptor_setVariableValue_* (e.g. 02839E88) resets it to 0 |
| TVariableValue (JSystem/JStudio/JST_timeline.h) | `field_0x8` | `mUpdate` | update 02839D38 calls it when set (then outputs mValue); the adaptor_setVariableValue_* setters store update_immediate_ / update_time_ / update_functionValue_, VOID 02839E70 clears it; update_immediate_ 02839DA0 clears it after one use |
| TVariableValue (JSystem/JStudio/JST_timeline.h) | `field_0xc` | `mParam` | union {fv, val, bits}: the setters store the immediate value, the time rate or the function value (02839E88, 02839EB4, 02839EE0); the update functions read it |
| TAdaptor_particle (JSystem/JStudio/JST_adaptors.h) | `field_0x1B0` | `mParticleID` | adaptor_do_PARTICLE 0283EA94: operation 0x19 stores the particle id; TAdaptor_particle_construct 0283E784 sets -1 |
| JAIBasicView (JSystem/JAudio/JAIGlobalParameter.h (view of JAIBasic)) | `field_0xd` | `mOutputMode` | JAIBasic::mOutputMode (JSystem/JAudio/JAIBasic.h, +0x11); setParamSoundOutputMode 02804508 stores it |
| JAIBasicView (JSystem/JAudio/JAIGlobalParameter.h (view of JAIBasic)) | `field_0xe` | `mFlags` | JAIBasic::mFlags (JSystem/JAudio/JAIBasic.h, +0x12); setParamStreamInsideBufferCut 02804598 sets bit 11 |
| mDoExt_MtxCalcOldFrame (m_Do/m_Do_ext_animation.h) | `field_0x8` | `mOldFrameMorfStep` | initOldFrameMorf 025E3F3C: 1 / duration; decOldFrameMorfCounter 025E3EC8: remain = counter * it |
| mDoExt_MtxCalcOldFrame (m_Do/m_Do_ext_animation.h) | `field_0x10` | `mOldFrameMorfRemain` | decOldFrameMorfCounter 025E3EC8: counter * mOldFrameMorfStep (1 at initOldFrameMorf 025E3F3C) |
| mDoExt_MtxCalcOldFrame (m_Do/m_Do_ext_animation.h) | `field_0x14` | `mOldFrameMorfRemainOld` | decOldFrameMorfCounter 025E3EC8: the previous mOldFrameMorfRemain; mOldFrameRate = 1 - (old - new) / old |
| TAdaptor_particle (JSystem/JStudio/JST_adaptors.h) | `field_0x1B4` | `mFadeState` | Particle_beginFade 0283EB98 sets 1, Particle_endFade 0283ECE8 sets 2, adaptor_do_update 0283EA48 clears a finished fade-in; endFade reads it |
| TAdaptor_particle (JSystem/JStudio/JST_adaptors.h) | `field_0x1B8` | `mFadeDuration` | Particle_beginFade 0283EB98 / Particle_endFade 0283ECE8 store the fade frames; adaptor_do_update 0283EA48 ends the fade when mFadeElapsed reaches it |
| TAdaptor_particle (JSystem/JStudio/JST_adaptors.h) | `field_0x1BC` | `mFadeElapsed` | adaptor_do_update 0283EA48 adds the frames; the emitter callback derives the fade alpha from it and mFadeDuration |
| TAdaptor_particle (JSystem/JStudio/JST_adaptors.h) | `field_0x1C8` | `mNodeID` | same word as ParticleParentPrefix::mNodeID (JStudio_JParticle/object-particle.h; adaptor_do_PARENT_NODE 0283EB00 sets it); the emitter callback passes it to JSGGetNodeTransformation |
| TAdaptor_particle (JSystem/JStudio/JST_adaptors.h) | `field_0x1CC` | `mEnabled` | same byte as ParticleParentPrefix::mEnabled (adaptor_do_PARENT_ENABLE 0283EB7C); the emitter callback follows the parent node when set |

Open: mDoExt_MtxCalcOldFrame `field_0x1` (only the constructor sets it, d_a_player_main_01.cpp); mDoMtxQuatStack (only its
static initialiser touches it), MaterialWork `field_0x44`, mDoExt_bpkAnm/btpAnm `field_0x14` (only cleared), actor_place `field_0x13`
(unused), msg_class `field_0xf0`/`field_0xf4` and fopMsg_prm_class `field_0x14`/`field_0x18` (only copied by fopMsg_Create 025DB150),
MyPicture m124..m134 and mDoMain_StartupRecord: no use that shows a meaning.

## Alias batch: offset-name union aliases removed

Rule (2026-11-10): an offset name that aliases an already named member of exactly the same type in a
union is removed and its uses move to the named member (object-identical; WWHD_OFFSET asserts of the named members kept).
Aliases of a different type stay: Osc_ `field_0x0` (byte in a word union with mTargetWord), JAInter::Actor `field_0x0`
(gptr<cXyz> next to gptr<void> mActor) and `field_0x8` (gptr<void> next to be<u32> mPositionState), TRegisterParam
`field_0xc` (u16 next to the bank/program byte pair).

| Struct (header) | Removed alias | Named member | Uses moved |
|---|---|---|---|
| JASystem::TOscillator::Osc_ (JSystem/JAudio/JASOscillator.h) | `field_0x4` | `mRate` | 2 (JASOscillator.cpp) |
| JASystem::TOscillator::Osc_ | `field_0x10` | `mScale` | 4 (JASOscillator.cpp) |
| JASystem::TOscillator::Osc_ | `field_0x14` | `mOffset` | 5 (JASOscillator.cpp) |
| JAISound (JSystem/JAudio/JAISound.h) | `field_0x18` | `frameCounter` | 1 (JAIZelAnime.cpp) |
| JAISound | `field_0x1c` | `mapInfo` | 2 (JAIZelAnime.cpp) |
| JAInter::Actor (JAZelAudio/JAIZelAnime.h) | `field_0x4` | `mPosition` | 9 (JAIZelAnime.cpp 6, JAIAnimation.cpp, JAIBasic.cpp, JAIConst.cpp) |
| JAInter::Actor | `field_0xc` | `mMapInfo` | 7 (JAIZelAnime.cpp 4, JAIAnimation.cpp, JAIBasic.cpp, JAIConst.cpp) |

## HD-only code (hd/)

HD additions with no GameCube counterpart; names from the WWHD uses (the callers of the setters are named in this
decomp's hd_scene_* sources).

| Struct (header) | Old | New | Evidence |
|---|---|---|---|
| hdSoundPlayer (hd/hd_snd_0202EE08.h; view hdSoundInterfacePlayer in hd_snd_020306F0.h) | `field_0x1f5` | `mPictographMode` | setPlayerFlagF5 02030CD0 is called by hdIngameUi_c::toPictographBoxEnter 027106E0, clearPlayerFlagF5 02030CF8 by hdIngameUi_c::toPlayEnter 0270D208; filterBridgeSound 0202CCA0 treats it as a demo state (getPlayerFlagF5 02030D20) |
| hdSoundPlayer (hd/hd_snd_0202EE08.h; view hdSoundInterfacePlayer in hd_snd_020306F0.h) | `field_0x1f6` | `mSaveDataMode` | setPlayerFlagF6 02030D44 is called by hdSaveDataScene_c::initialize 02709D24, clearPlayerFlagF6 02030D6C by hdIngameUi_c::initializeScreens 02713368; the volume update (hd_snd_0202EE08) gives the menu player full TV/DRC volume while set |
| hdSoundPlayer (hd/hd_snd_0202EE08.h; view hdSoundInterfacePlayer in hd_snd_020306F0.h) | `field_0x1f7` | `mGameOverMode` | setPlayerFlagF7 02030D94 is called by ingameGameOverEnter 0271201C, clearPlayerFlagF7 02030DC0 by hdIngameUi_c::toPlayEnter 0270D208; the volume update keeps the DRC game channel and the menu player at full volume while set |
| hdSoundPlayer (hd/hd_snd_0202EE08.h; view hdSoundInterfacePlayer in hd_snd_020306F0.h) | `field_0x1f8` | `mWarpMode` | setPlayerFlagF8 02030DE8 is called by hdIngameUi_c::warpEnter 0270F9F8, clearPlayerFlagF8 02030E10 by hdIngameUi_c::warpExit 0270FB48; the volume update treats it like mGameOverMode |
| hdSoundInterfaceBridge (hd/hd_snd_020306F0.h) | `field_0x38` | `mNoSceneStream` | setBridgeFlag 02030E38 is called by JAIZelBasic::load1stDynamicWave 02026E04 for scenes without a scene stream; clearBridgeFlag 02030E64 when scene 0x76 / 0x77 load their stream |
| hdProfileTask (hd/hd_sys_0203E1D0.h) | `field_0x70` | `mSecondaryVtable` | the task constructor 0203E380 stores the vtable 10006230 here after hdProfileConstructTaskBase (a second base; its type is unconfirmed) |

Open (only cleared by a constructor or reset, no reader): hdUiEventBox `field_0x0`/`field_0x4`/`field_0x30`,
hdUiInputStateHolder, hdLayoutScreen, hdUiVisitor `field_0x0`, hdScreenDrawInfo `field_0x0`/`field_0x1`, hdSoundBridge
`field_0x42`, hdSoundHandleHolder `field_0x8`/`field_0x10`, hdStateId `field_0x4`, hdSoundHeapMemberArgument `field_0x0`,
hdBgmController and hdSoundArchiveAccessor `field_0x4`/`field_0x8`, hdSoundArchiveLoadArg `field_0x18`..`field_0x28`.

## JASystem::TRegisterParam (JSystem/JASRegisterParam.h)

| Struct (header) | Old | New | Evidence |
|---|---|---|---|
| TRegisterParam | `field_0x20` | `mAddress` | TWW name (zeldaret/tww d217e2b libs/JSystem/include/JSystem/JAudio/JASRegisterParam.h: u32 mAddress[4], getAddress/setAddress); init 028457C0 / inherit 02845840 clear the four words |

Open: `field_0x0[6]` (tww's getFlag() reads element 3, the array itself stays), `field_0xe` and `field_0x1a` (tww comments
"BendSense?" / "Priority?"; WWHD only sets 12 / 0x40 in init and copies them in inherit), `field_0x1c` (unused); the
`field_0xc` union alias stays (different type).

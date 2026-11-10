# Field names: meter (HUD)

Fields the decomp still called by offset (`field_0x…`, the zeldaret/tww GameCube offset names) get names in batches (field
naming pass, 2026-11-10). Rules: a name comes from how the original code uses the field (address of the use
and what it shows), from names already in this decomp, or from the CC0 zeldaret/tww and zeldaret/tp decomps (same field of
the same class). The dMeter_*HIO_c classes are tuning parameters: they are named only from real uses, never from their
default values alone. Renames only (objcmp: object-identical). The header line keeps the old name (`was field_0x…`) with
the evidence. Pane fields also give the pane tag set by dMeter_screenDataSet (inlined into dMeter_Create 02595C6C).

## Batch M1: key counter (dMeter_keyMove 0259C7FC)

### sub_meter_class (d_meter_02.h)

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x1984 | `field_0x1980` | `mKeyLight1` | pane 'kyl1' (dMeter_screenDataSet, inlined into dMeter_Create 02595C6C); dMeter_keyMove 0259C7FC runs dMeter_keyLight on it with the fixed frames {30,40,45,50,58} (its mUserArea is the frame counter); dMeter_Create sets the counter to 1; same code TWW d_meter.cpp dMeter_keyMove/dMeter_keyInit |
| 0x19BC | `field_0x19b8` | `mKeyLight2` | pane 'kyl2' (dMeter_screenDataSet in dMeter_Create 02595C6C); dMeter_keyMove 0259C7FC runs dMeter_keyLight on it with mKeyLight2Frames and re-rolls those frames when its counter is 1; dMeter_Create sets the counter to 1 |
| 0x2FDE | `field_0x2fda` | `mKeyLight2Frames` | dMeter_keyMove 0259C7FC: the five dMeter_keyLight frame thresholds of mKeyLight2, rolled from the dMeter_HIO_c mKeyLight* values when mKeyLight2's counter is 1 |
| 0x1C94 | `field_0x1c90` | `mKeyIcon` | pane 'key0' (dMeter_screenDataSet in dMeter_Create 02595C6C); dMeter_keyMove 0259C7FC fades it with the key digits (its mUserArea is the dMeter_alphaOpen/alphaClose counter); dMeter_Create clears it |
| 0x1AD4 | `field_0x1ad0` | `mKeyDigitA` | panes 'nm03'/'nm04' (dMeter_screenDataSet in dMeter_Create 02595C6C): the tens/ones digits of the key count (TWW dMeter_keyInit/dMeter_keyMove blendInit/blendDraw them with the same digit as mKeyDigitB); dMeter_keyMove 0259C7FC sets their alpha, [0].mUserArea is the second dMeter_alphaOpen/alphaClose counter |
| 0x1C24 | `field_0x1c20` | `mKeyDigitB` | panes 'nm08'/'nm09' (dMeter_screenDataSet in dMeter_Create 02595C6C): the second copy of the key count digits (same digit as mKeyDigitA in TWW dMeter_keyInit/dMeter_keyMove); dMeter_keyMove 0259C7FC sets their alpha |
| 0x3021 | `field_0x301d` | `mDispKeyNum` | dMeter_Create 02595C6C stores dComIfGs_getKeyNum(); dMeter_keyMove 0259C7FC steps it by one per frame toward the saved key count (mKeyNumPending); the shown key count |
| 0x3012 | `field_0x300e` | `mKeyNumPending` | dMeter_keyMove 0259C7FC sets it to the new saved key count minus mDispKeyNum when play.mItemKeyNumCount is applied, then counts it to 0 while mDispKeyNum follows |

### dMeter_HIO_c (class in d_meter_01.h; view in d_meter_02.h)

The view groups 0x88/0x8A/0x8C and 0x92/0x94/0x96 as arrays (`mKeyLightStep[3]`, `mKeyLightStepRange[3]`); the class
keeps single fields and names them `...0/1/2`. Defaults set by the constructor 025974DC: start 20 (range 0), steps 15/5/5
(ranges 12/3/3), end step 5 (range 3).

| WWHD offset | Old | New (class / view) | Evidence |
|---|---|---|---|
| 0x088 | `field_0x88` | `mKeyLightStep0` / `mKeyLightStep[0]` | dMeter_keyMove 0259C7FC: mKeyLight2Frames[i+1] = previous frame + mKeyLightStep[i] + cM_rndFX(mKeyLightStepRange[i]), i = 0..2 (0x88/0x8A/0x8C) |
| 0x08A | `field_0x8a` | `mKeyLightStep1` / `mKeyLightStep[1]` | second mKeyLightStep (the d_meter_02.h view reads 0x88..0x8C as mKeyLightStep[3], dMeter_keyMove 0259C7FC) |
| 0x08C | `field_0x8c` | `mKeyLightStep2` / `mKeyLightStep[2]` | third mKeyLightStep (view mKeyLightStep[2], dMeter_keyMove 0259C7FC) |
| 0x08E | `field_0x8e` | `mKeyLightEndStep` | dMeter_keyMove 0259C7FC: mKeyLight2Frames[4] = mKeyLight2Frames[3] + mKeyLightEndStep + cM_rndFX(mKeyLightEndRange) |
| 0x090 | `field_0x90` | `mKeyLightStart` | dMeter_keyMove 0259C7FC: base of mKeyLight2Frames[0] (plus cM_rndFX(mKeyLightStartRange)) |
| 0x092 | `field_0x92` | `mKeyLightStepRange0` / `mKeyLightStepRange[0]` | dMeter_keyMove 0259C7FC: cM_rndFX ranges for the mKeyLightStep steps (0x92/0x94/0x96) |
| 0x094 | `field_0x94` | `mKeyLightStepRange1` / `mKeyLightStepRange[1]` | second mKeyLightStepRange (view mKeyLightStepRange[1], dMeter_keyMove 0259C7FC) |
| 0x096 | `field_0x96` | `mKeyLightStepRange2` / `mKeyLightStepRange[2]` | third mKeyLightStepRange (view mKeyLightStepRange[2], dMeter_keyMove 0259C7FC) |
| 0x098 | `field_0x98` | `mKeyLightEndRange` | dMeter_keyMove 0259C7FC: cM_rndFX range of the last mKeyLight2Frames step |
| 0x09A | `field_0x9a` | `mKeyLightStartRange` | dMeter_keyMove 0259C7FC: cM_rndFX range added to mKeyLightStart for mKeyLight2Frames[0] |

## Batch M2: magic and heart gauge (dMeter_magicGaugeMove 0259B698, dMeter_gaugeAlpha 0259C2BC)

### sub_meter_class (d_meter_02.h) and its view dMeterPrefixHD (d_meter_00.h)

The view keeps the GameCube offsets in its old names (HD offset = GameCube + 4) and now takes the class names; it does
not reach 0x301A / 0x301C. The 'k' pane tags are the shadow panes, as for the hearts ('ht'/'hk' = mHeart/mHeartShadow).

| WWHD offset | Old (GameCube name) | New | In the view | Evidence |
|---|---|---|---|---|
| 0x0F04 | `field_0x0f00` | `mHeartLight` | yes | pane 'htfl' (dMeter_screenDataSet in dMeter_Create 02595C6C): the light on the current heart; dMeter_heartInit 02591B4C copies the current heart's position and size into it and clears its mUserArea (frame counter, also cleared by the life change in dMeter_Execute 02593B10); dMeter_gaugeAlpha 0259C2BC scales its alpha by mHeartAlpha; TWW d_meter.cpp dMeter_heartLightMove pulses its alpha and size |
| 0x0F3C | `field_0x0f38` | `mMagicBar[8]` | yes | panes 'mbr1'..'mbr8' (dMeter_screenDataSet in dMeter_Create 02595C6C): the magic fill segments; dMeter_magicChange 02592A88 shows, shortens or hides each by the shown magic (mDispMagic / 100), dMeter_gaugeAlpha 0259C2BC fades them with mMagicAlpha; their mUserArea fields are dMeter_magicGaugeMove 0259B698 / dMeter_flyGaugeMove 02592EE8 state ([0] fly colours set, [2] capacity reached, [3]/[4] pulse timer/flag, [5] redraw) |
| 0x10FC | `field_0x10f8` | `mMagicGauge3` | yes | pane 'mbb3' (dMeter_screenDataSet in dMeter_Create 02595C6C): dMeter_magicLength 02592888 sets its width from the shown capacity (mDispMaxMagic), mMagicGauge1/2 follow that width; its mUserArea is the fade counter behind mMagicAlpha (dMeter_magicGaugeMove 0259B698); moved with the gauge (dMeter_magicTransNowInit 0259295C, dMeter_magicTransScale 02592CD8) and faded by dMeter_gaugeAlpha 0259C2BC |
| 0x1134 | `field_0x1130` | `mMagicGauge2` | yes | pane 'mbb2' (dMeter_screenDataSet in dMeter_Create 02595C6C): dMeter_magicLength 02592888 sizes it to mMagicGauge3's width plus its own margin; dMeter_magicTransScale 02592CD8 scales its position and size (a child of mMagicGauge1); its mUserArea is dMeter_flyGaugeMove 02592EE8's 50-frame move-to-the-flyer timer; TWW dMeter_magicColor colours it with mMagicGauge1 |
| 0x11A4 | `field_0x11a0` | `mMagicGauge1` | yes | pane 'mbb1' (dMeter_screenDataSet in dMeter_Create 02595C6C): dMeter_magicLength 02592888 sizes it to mMagicGauge3's width plus its own margin; its centre is the anchor dMeter_magicTransScale 02592CD8 scales the other gauge panes about; its mUserArea is dMeter_flyGaugeMove 02592EE8's fade counter; moved with the gauge (dMeter_magicTransNowInit 0259295C) and faded by dMeter_gaugeAlpha 0259C2BC |
| 0x11DC | `field_0x11d8` | `mMagicGaugeShadow1` | yes | pane 'mbk1' (dMeter_screenDataSet in dMeter_Create 02595C6C): moves, scales and fades with mMagicGauge1 (dMeter_magicTransNowInit 0259295C, dMeter_magicInitTrans 02592BC4, dMeter_magicTransScale 02592CD8, dMeter_gaugeAlpha 0259C2BC); dMeter_magicGaugeMove 0259B698 clears its mUserArea; the 'k' tags are the shadow panes as for mHeart/mHeartShadow ('ht'/'hk') |
| 0x116C | `field_0x1168` | `mMagicGaugeShadow2` | yes | pane 'mbk2' (dMeter_screenDataSet in dMeter_Create 02595C6C): dMeter_magicTransScale 02592CD8 scales its position and size like mMagicGauge2 (a child of mMagicGauge1); the shadow of 'mbb2' by the tag pattern of mHeart/mHeartShadow ('ht'/'hk') |
| 0x2FC0 | `field_0x2fbc` | `mHeartAlpha` | yes | alpha ratio of the hearts: dMeter_Execute 02593B10 (inlined heart alpha step) sets valueIncrease(5, field_0x3014); dMeter_gaugeAlpha 0259C2BC applies it to mHeart, mHeartShadow and mHeartLight; dMeter_heartInit 02591B4C clears it; dMeter_Execute copies it to the HD state (MeterUpdateSharedState mHeartAlpha); same code TWW dMeter_heartAlpha / dMeter_gaugeAlpha |
| 0x2FC4 | `field_0x2fc0` | `mMagicAlpha` | yes | alpha ratio of the magic gauge: dMeter_magicGaugeMove 0259B698 sets valueIncrease(5, mMagicGauge3.mUserArea), dMeter_flyGaugeMove 02592EE8 sets its own fade; dMeter_gaugeAlpha 0259C2BC applies it to mMagicBar, mMagicGauge3, mMagicGauge1, mMagicGaugeShadow1; dMeter_Create 02595C6C clears it; dMeter_Execute 02593B10 copies it to the HD state (MeterUpdateSharedState mMagicAlpha); same code TWW dMeter_magicAlpha / dMeter_gaugeAlpha |
| 0x301A | `field_0x3016` | `mDispMagic` | no | the shown magic, in 1/100 of the mMagicBar width: dMeter_Create 02595C6C sets it from the saved magic; dMeter_magicGaugeMove 0259B698 steps it by 625 toward magic * mMagicBar[0] width * 25 and passes it / 100 to dMeter_magicChange; same code TWW dMeter_magicInit / dMeter_magicGaugeMove |
| 0x301C | `field_0x3018` | `mDispMaxMagic` | no | the shown magic capacity: dMeter_Create 02595C6C sets it from the saved mMaxMagic; dMeter_magicGaugeMove 0259B698 steps it by one per frame toward the saved mMaxMagic and calls dMeter_magicLength with it / 32; same code TWW dMeter_magicInit / dMeter_magicGaugeMove |

### MeterUpdateSharedState (d_meter_01.h): misnamed fields corrected (own commit)

| Offset | Old | New | Evidence |
|---|---|---|---|
| +0x18 | `mActionAlpha` (misnamed) | `mHeartAlpha` | dMeter_Execute 02593B10 copies sub_meter_class mHeartAlpha (the heart gauge alpha ratio) into it |
| +0x1C | `mSecondaryAlpha` (misnamed) | `mMagicAlpha` | dMeter_Execute 02593B10 copies sub_meter_class mMagicAlpha (the magic gauge alpha ratio) into it |

## Batch M3: dMeter_HIO_c's read fields

Only these four dMeter_HIO_c fields are read by the meter code; every other offset-named field is only written by the
constructor 025974DC and stays open. The class is in d_meter_01.h, the view in d_meter_02.h.

| WWHD offset | Old | New | In the view | Evidence |
|---|---|---|---|---|
| 0x004 | `field_0x6` | `mCompassFadeFrames` (was `mHudFadeFrames`, misnamed) | no | WWHD offset 0x4 (GameCube 0x6): the fade length in frames of the compass alpha (sub_meter_class field_0x2f6c, which TWW dMeter_compassAlpha applies to the compass and map panes); the alpha step inlined into dMeter_Execute 02593B10 (meterUpdateAlphaControl) fades field_0x2f6c by valueIncrease(mCompassFadeFrames, field_0x2fd8) and sets the timer to it when fully shown; dMeter_Create 02595C6C starts the timer at it; constructor 025974DC default 10; same code TWW d_meter.cpp (dMeter_alphaControl, dMeter_Create) |
| 0x00C | `field_0xc` | `mBossLifeRatioDefault` | yes | dMeter_enemyMove 0259ACD4: the boss life ratio shown when the boss actor's max_health is 0 (else health / max_health); only the constructor 025974DC writes it (1.0); same use TWW dMeter_bossEnemyMove |
| 0x062 | `field_0x62` | `mIgnoreArrowNumCount` | yes | dMeter_xyMove 02599584: when set, the pending play mItemArrowNumCount is cleared before it is added to the saved arrow count (the change is dropped); only the constructor 025974DC writes it (0); same use TWW dMeter_xyItemCountUp |
| 0x063 | `field_0x63` | `mIgnoreBombNumCount` | yes | dMeter_xyMove 02599584: when set, the pending play mItemBombNumCount is cleared before it is added to the saved bomb count (the change is dropped); only the constructor 025974DC writes it (0); same use TWW dMeter_xyItemCountUp |

## Batch M4: rupee counter, compass alpha, heart alpha step

### sub_meter_class (d_meter_02.h) and its view dMeterPrefixHD (d_meter_00.h)

dMeter_walletChange 025931CC is the function d_meter_00.cpp calls dMeter_setMagicPositionHD (its ext line in d_meter_02.cpp
names it dMeter_walletChange).

| WWHD offset | Old (GameCube name) | New | In the view | Evidence |
|---|---|---|---|---|
| 0x19F4 | `field_0x19f0` | `mRupyDigitA[4]` | yes | panes 'nm00'..'nm03' on sMainParts2 (dMeter_screenDataSet in dMeter_Create 02595C6C): the rupee count digits (TWW dMeter_rupyInit / dMeter_walletChange give them the same digit textures as mRupyDigitB); dMeter_rupyMove 0259C454 fades them; [0].mUserArea is its dMeter_alphaOpen/alphaClose second counter (dMeter_Create sets 1), [1].mUserArea the shown wallet size (dMeter_rupyMove calls dMeter_walletChange 025931CC when the saved mWalletSize differs); dMeter_walletChange moves mRupyIcon by the [1]-[0] x distance |
| 0x1B44 | `field_0x1b40` | `mRupyDigitB[4]` | no | panes 'nm04'..'nm07' on sMainParts2 (dMeter_screenDataSet in dMeter_Create 02595C6C): the second copy of the rupee count digits (same digit textures as mRupyDigitA in TWW dMeter_rupyInit / dMeter_walletChange); dMeter_rupyMove 0259C454 fades them with mRupyDigitA |
| 0x2A24 | `field_0x2a20` | `mRupyIcon` | yes | pane 'rup1' on sMainParts2 (dMeter_screenDataSet in dMeter_Create 02595C6C): the rupee icon; dMeter_rupyMove 0259C454 fades it (its mUserArea is the dMeter_alphaOpen/alphaClose counter, dMeter_Create clears it), dMeter_walletChange 025931CC moves it with mRupyIconB; TWW dMeter_walletChange sets its wallet colours |
| 0x2A5C | `field_0x2a58` | `mRupyIconB` | yes | pane 'rup2' on sMainParts2 (dMeter_screenDataSet in dMeter_Create 02595C6C): the second rupee icon pane; dMeter_rupyMove 0259C454 fades it and dMeter_walletChange 025931CC moves it together with mRupyIcon (same in TWW dMeter_rupyMove / dMeter_walletChange) |
| 0x2F70 | `field_0x2f6c` | `mCompassAlpha` | no | the compass alpha ratio: the alpha step inlined into dMeter_Execute 02593B10 (meterUpdateAlphaControl) fades it over dMeter_HIO_c mCompassFadeFrames when the 2D show flag changes and holds it at 0.5 / 0 while the menu window is open; dMeter_Create 02595C6C sets 1.0; TWW dMeter_alphaControl computes it the same way and dMeter_compassAlpha applies it to the compass and map panes |
| 0x2FDC | `field_0x2fd8` | `mCompassFadeTimer` | no | the frame counter of the mCompassAlpha fade: the alpha step inlined into dMeter_Execute 02593B10 counts it down / up and passes it to valueIncrease(mCompassFadeFrames, ...); dMeter_Create 02595C6C starts it at mCompassFadeFrames; same code TWW dMeter_alphaControl / dMeter_Create |
| 0x3018 | `field_0x3014` | `mHeartAlphaStep` | yes | the heart fade step 0..5: the heart alpha step inlined into dMeter_Execute 02593B10 (meterUpdateActionFade) moves it toward 0, 3 or 5 by the meter state and sets mHeartAlpha = valueIncrease(5, it); dMeter_heartInit 02591B4C sets 5; same code TWW dMeter_heartAlpha / dMeter_heartInit |

## Open (no use beyond the constructor or not yet reviewed)

- dMeter_menuHIO_c (90), dMeter_msg_HIO_c (80), dMeter_message_HIO_c (65): every offset-named field is only written by
  its constructor (02597B00 / 02597E34 / 02598058) in the decompiled code; no read to name them from.
- dMeter_HIO_c: the read fields are named (M1, M3); the other fields are only written by the constructor 025974DC.

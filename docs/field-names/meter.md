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

## Batch W1: weapon and action buttons (dMeter_weponMove 025986DC, action steps in dMeter_Execute 02593B10)

### sub_meter_class (d_meter_02.h)

The weapon button shows the AStatus (sword and the other weapons as an icon, any other status as a word), the action
button the DoStatus. The pane tags follow 'ba' + button letter for the button panes ('baa' action, 'bab' weapon, as
'bax1'/'bay'/'baz' and 'bar1' of the item and R buttons) and 'k' for shadow panes. The action steps that TWW keeps
in dMeter_actionMove are inlined into dMeter_Execute 02593B10 in WWHD (meterUpdateActionChange,
meterUpdateActionPanes); the views do not reach these fields.

| WWHD offset | Old (GameCube name) | New | In the view | Evidence |
|---|---|---|---|---|
| 0x1D04 | `field_0x1d00` | `mWeaponText` | no | pane 'bawd' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the action word of the weapon (AStatus) button: dMeter_weponMove 025986DC gives it the button alpha when the AStatus is neither blank nor a weapon (sword, boko stick, hatchet, iron club, longsword, spear) and clears its alpha otherwise; its mInitAlpha is the full alpha mActionText blinks down from; its mUserArea is cleared when mLastAStatus was BID; TWW dMeter_weponChange gives it the action texture actionTex[0], dMeter_weponAlpha / weponTrans fade and move it the same way |
| 0x282C | `field_0x2828` | `mWeaponIcon` | no | pane 'weit' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the weapon icon of the weapon (AStatus) button: dMeter_weponMove 025986DC gives it the button alpha for a weapon AStatus (sword, boko stick, hatchet, iron club, longsword, spear) and clears its alpha otherwise; its mUserArea is the texture-change flag (set when the AStatus differs from mLastAStatus, cleared by the next dMeter_weponMove; dMeter_weponInit in dMeter_Create clears it); TWW dMeter_weponChange loads the weapon texture actionTex[0] into it and dMeter_weponAlpha hides the button word and icons while the flag is set |
| 0x2864 | `field_0x2860` | `mWeaponIconShadow` | no | pane 'witk' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the shadow of mWeaponIcon (the 'k' tags are the shadow panes, as 'ht'/'hk' for mHeart/mHeartShadow): dMeter_weponMove 025986DC fades it with mWeaponIcon; TWW dMeter_weponChange gives it the same texture and dMeter_Create moves it as a child of mWeaponIcon |
| 0x289C | `field_0x2898` | `mWeaponButton` | no | pane 'bab' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the weapon (AStatus) button ('ba' + button letter, as 'baa' mActionButton, 'bax1'/'bay'/'baz' and 'bar1' of the item and R buttons): dMeter_weponMove 025986DC gives it the button alpha for every AStatus; drawn by dMeter_Draw 02593630; TWW dMeter_weponAnime / weponTrans move it with the weapon panes |
| 0x28D4 | `field_0x28d0` | `mWeaponButton2` | no | pane 'baat' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the second weapon button pane, faded with mWeaponButton for every AStatus by dMeter_weponMove 025986DC; its mUserArea is the button bounce frame: dMeter_weponMove sets it to 1 when the AStatus changes and steps it to 12, then 0; dMeter_weponInit in dMeter_Create clears it; TWW dMeter_weponAnime moves the weapon panes by that frame |
| 0x290C | `field_0x2908` | `mButtonCenter` | no | pane 'cent' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): faded by both button groups (dMeter_weponMove 025986DC for a non-blank AStatus, meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10 for every DoStatus) and drawn by dMeter_Draw 02593630; TWW dMeter_Create makes it the parent pane (dMeter_parentPaneTrans) that the action, weapon, item and R button panes are moved with |
| 0x1D3C | `field_0x1d38` | `mActionText` | no | pane 'bawp' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the action word of the action (DoStatus) button: meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10 gives it the button alpha for a DoStatus that is not blank, parry, ba_sake / ba_nageru / ba_motu or bow, and clears it otherwise; dMeter_weponMove 025986DC blinks its mNowAlpha down from mWeaponText's mInitAlpha while the DoStatus is BID (its mUserArea is the 20-frame blink counter); TWW dMeter_actionChange gives it the action texture actionTex[1] |
| 0x1DAC | `field_0x1da8` | `mActionButton[2]` | no | panes 'baa' ([0]) and 'baa2' ([1]) on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the action (DoStatus) button ('ba' + button letter, as 'bab' mWeaponButton): meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10 fades [0] for a blank, bow or other action and [1] for parry and ba_sake / ba_nageru / ba_motu, together with mActionButton2; drawn by dMeter_Draw 02593630; same in TWW dMeter_actionAlpha / actionTrans |
| 0x1E1C | `field_0x1e18` | `mActionButton2[2]` | no | panes 'bawe' ([0]) and 'bat2' ([1]) on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the second action button pane pair, faded like mActionButton ([0] for a blank, bow or other action, [1] for parry and ba_sake / ba_nageru / ba_motu; meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10); [1].mUserArea is the 20-frame blink counter dMeter_weponMove 025986DC steps for ba_sake / ba_nageru / ba_motu (sound 0x8F8 / 0x90A / 0x852 at each wrap); [0].mUserArea is set to 1 by meterUpdateActionChange (dMeter_Execute 02593B10) when the DoStatus becomes blank or another action and cleared by dMeter_actionInit (dMeter_Create); TWW dMeter_weponMove blinks [1]'s black colour |
| 0x274C | `field_0x2748` | `mArrowDigit10` | no | pane 'a10' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the tens digit of the action button's arrow count ('a10'/'a01' as 'x10'/'x01' of the item counts): meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10 shows it only for DoStatus bow_01; drawn by dMeter_Draw 02593630; TWW dMeter_actionAlpha sets it to the So mini-game's rest arrows / 10 (hidden below 10) |
| 0x2784 | `field_0x2780` | `mArrowDigit1` | no | pane 'a01' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the ones digit of the action button's arrow count: meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10 shows it only for DoStatus bow_01; drawn by dMeter_Draw 02593630; TWW dMeter_actionAlpha sets it to the So mini-game's rest arrows % 10 |
| 0x27BC | `field_0x27b8` | `mActionBowIcon` | no | pane 'yum' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the bow icon of the action button: meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10 shows it only for DoStatus bow_01; its mUserArea is the action-change flag (set when the DoStatus differs from mLastDoStatus, cleared by meterUpdateActionChange in dMeter_Execute 02593B10; dMeter_actionInit in dMeter_Create clears it); TWW dMeter_actionChange loads the bow texture actionTex[1] into it |
| 0x27F4 | `field_0x27f0` | `mActionBowIconShadow` | no | pane 'yumk' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the shadow of mActionBowIcon (the 'k' tags are the shadow panes, as 'ht'/'hk' for mHeart/mHeartShadow): meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10 fades it with mActionBowIcon; TWW dMeter_actionChange gives it the same bow texture |
| 0x301E | `field_0x301a` | `mLastAStatus` | no | the AStatus the weapon button shows: dMeter_weponMove 025986DC compares it with play mAStatus, sets mWeaponIcon / mWeaponButton2 mUserArea (texture change, bounce) and stores the new status when they differ; dMeter_weponInit (dMeter_Create 02595C6C) clears it; same code TWW dMeter_weponChange / weponInit |
| 0x301F | `field_0x301b` | `mLastDoStatus` | no | the DoStatus the action button shows: meterUpdateActionChange (dMeter_Execute 02593B10) stores play mDoStatus after a change step and sets mActionBowIcon.mUserArea when they differ; dMeter_actionInit (dMeter_Create 02595C6C) clears it; same code TWW dMeter_actionChange / actionInit |
| 0x3028 | `field_0x3024` | `mButtonActionMode` | no | the button action mode bits: dMeter_Execute 02593B10 clears it each frame, the button steps set 1 (action alpha, inlined into dMeter_Execute), 2 (dMeter_weponMove 025986DC), 4 / 8 / 0x20 / 0x40 (dMeter_xyMove 02599584) and 0x10 (dMeter_rAlpha 0259A5B0); dMeter_Execute keeps only 0x13 while the HUD context masks the buttons and copies it to play mButtonMode; TWW dMeter_Execute passes it to dComIfGp_setButtonActionMode |
| 0x302A | `field_0x3026` | `mButtonMoveFlags` | no | one bit per button group that has finished moving out (1 action, inlined into dMeter_Execute 02593B10; 2 weapon, dMeter_weponMove 025986DC; 4 item, dMeter_xyMove 02599584; 8 R, dMeter_rAlpha 0259A5B0): a group clears its bit when its move status changes, counts its move frame up to 5 and sets the bit; once all four are set (15) every group counts its frame back down; same code TWW dMeter_weponTrans / actionTrans / xyTrans / rTrans |

## Batch X1: X / Y / Z item buttons (dMeter_xyMove 02599584)

### sub_meter_class (d_meter_02.h)

Index 0/1/2 is the X/Y/Z button (TWW dItemBtn_X_e/Y_e/Z_e). WWHD sets every pane up (dMeter_screenDataSet) and draws
it (dMeter_Draw 02593630); the count digits, icon textures and fades that TWW does in dMeter_xyItemNumberSet /
xyItemChange / xyAlpha give the roles. The second and third X/Y pane pairs `field_0x25c0[2]` ('b001'/'b002') and
`field_0x2630[2]` ('ba01'/'ba02') stay open: TWW fades and moves them exactly like mXYButton, WWHD only sets and
draws them. The views do not reach these fields.

| WWHD offset | Old (GameCube name) | New | In the view | Evidence |
|---|---|---|---|---|
| 0x1F34 | `field_0x1f30` | `mItemDigit10[3]` | no | panes 'x10' / 'y10' / 'z10' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the tens digit of each item button's count; drawn by dMeter_Draw 02593630; TWW dMeter_xyItemNumberSet sets it to the arrow, bomb or beast count / 10 (hidden below 10) and dMeter_xyAlpha fades it with the button |
| 0x1FDC | `field_0x1fd8` | `mItemDigit1[3]` | no | panes 'x01' / 'y01' / 'z01' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the ones digit of each item button's count; drawn by dMeter_Draw 02593630; TWW dMeter_xyItemNumberSet sets it to the arrow, bomb or beast count % 10, or to the whole picture or bait count and dMeter_xyAlpha fades it with the button |
| 0x2084 | `field_0x2080` | `mItemIcon[3]` | no | panes 'xitm' / 'yitm' / 'zitm' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the item icon of each item button: dMeter_xyMove 02599584 puts the forest water particles at its mPosCenter and gives them its mNowAlpha while that item is set; drawn by dMeter_Draw 02593630; TWW dMeter_xyItemChange loads xyIconTex into it |
| 0x212C | `field_0x2128` | `mItemIconShadow[3]` | no | panes 'xitk' / 'yitk' / 'zitk' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the shadow of mItemIcon (the 'k' tags are the shadow panes, as 'ht'/'hk' for mHeart/mHeartShadow); drawn by dMeter_Draw 02593630; TWW dMeter_xyItemChange gives it the same xyIconTex texture and shows / hides it with the icon |
| 0x2324 | `field_0x2320` | `mBowLight1[3]` | no | panes 'blx1' / 'bly1' / 'blz1' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the first bow light pane of each item button; dMeter_xyMove 02599584 pulses the three bow light panes of a button whose item is the magic or light arrow: the frame counter in mBowLight1[0].mUserArea (the X entry, as in TWW) counts to 120, alpha falls from mBowLight1[0].mInitAlpha toward l_bowAlphaMin and the scale toward 1 - l_bowPulseDepth; TWW dMeter_xyBowLightAnime; dMeter_xyMove clears [i].mUserArea when button i changes to an item that is not the magic or light arrow; drawn by dMeter_Draw 02593630 |
| 0x23CC | `field_0x23c8` | `mBowLight2[3]` | no | panes 'blx2' / 'bly2' / 'blz2' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the second bow light pane of each item button; dMeter_xyMove 02599584 pulses the three bow light panes of a button whose item is the magic or light arrow: the frame counter in mBowLight1[0].mUserArea (the X entry, as in TWW) counts to 120, alpha falls from mBowLight1[0].mInitAlpha toward l_bowAlphaMin and the scale toward 1 - l_bowPulseDepth; TWW dMeter_xyBowLightAnime; drawn by dMeter_Draw 02593630 |
| 0x2474 | `field_0x2470` | `mBowLight3[3]` | no | panes 'blx3' / 'bly3' / 'blz3' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the third bow light pane of each item button; dMeter_xyMove 02599584 pulses the three bow light panes of a button whose item is the magic or light arrow: the frame counter in mBowLight1[0].mUserArea (the X entry, as in TWW) counts to 120, alpha falls from mBowLight1[0].mInitAlpha toward l_bowAlphaMin and the scale toward 1 - l_bowPulseDepth; TWW dMeter_xyBowLightAnime; drawn by dMeter_Draw 02593630 |
| 0x251C | `field_0x2518` | `mZButton` | no | pane 'baz' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the Z item button ('ba' + button letter, as 'bab' mWeaponButton and 'baa' mActionButton); drawn by dMeter_Draw 02593630; TWW dMeter_xyAlpha fades it with the Z item panes |
| 0x2554 | `field_0x2550` | `mXYButton[2]` | no | panes 'bax1' ([0]) and 'bay' ([1]) on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the X and Y item buttons ('ba' + button letter, as 'baz' mZButton); dMeter_xyInit (dMeter_Create) clears [0].mUserArea; drawn by dMeter_Draw 02593630; TWW dMeter_xyAlpha fades [0] with the X and [1] with the Y item panes |
| 0x2F30 | `field_0x2f2c` | `mpForestWaterParticle[2]` | no | the two 2D particle emitters on the forest water icon: dMeter_xyMove 02599584 creates them (particle 0x2D) at mItemIcon's centre while a button holds forest water, moves them with it and gives them its alpha, and ends and clears them otherwise; dMeter_xyInit (dMeter_Create 02595C6C) clears them, dMeter_Delete 02595850 ends them with mpRupyParticle; same code TWW dMeter_xyMove / xyInit |
| 0x3025 | `field_0x3021` | `mLastSelectItem[3]` | no | the item each X / Y / Z button shows: dMeter_xyMove 02599584 compares it with play mSelectItem[i], calls dMeter_xyItemChange and stores the new item when they differ; dMeter_xyInit (dMeter_Create 02595C6C) sets it from mSelectItem; same code TWW dMeter_xyMove / xyInit |

## Batch C1: compass needle, base and clock pane

### sub_meter_class (d_meter_02.h) and its view dMeterPrefixHD (d_meter_00.h)

WWHD draws the compass in its HD code; the meter code only sets the panes up (dMeter_screenDataSet), draws them
(dMeter_Draw 02593630) and runs TWW's dMeter_compassValueInit (02593434 in d_meter_00.cpp, called
dMeter_gaugeInitialPositionHD there). The roles come from the pane tags and TWW's dMeter_compass* code.

| WWHD offset | Old (GameCube name) | New | In the view | Evidence |
|---|---|---|---|---|
| 0x124C | `field_0x1248` | `mCompassNeedle` | no | pane 'cmmg' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the compass needle; WWHD only sets it up there (the HD compass draws elsewhere); TWW dMeter_compassInit sets its MyPicture m124..m134 values and dMeter_compassMove turns it about mCompassBase by the ship's shape angle (dMeter_compassRotate) |
| 0x1284 | `field_0x1280` | `mCompassNeedleShadow` | no | pane 'cmmk' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the shadow of mCompassNeedle (the 'k' tags are the shadow panes, as 'ht'/'hk' for mHeart/mHeartShadow); WWHD only sets it up there; TWW dMeter_compassInit / compassMove treat it exactly like mCompassNeedle |
| 0x12F4 | `field_0x12f0` | `mCompassBase` | yes | pane 'cmb1' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the compass base: 02593434 (d_meter_00.cpp dMeter_gaugeInitialPositionHD; the same code as TWW dMeter_compassValueInit) clears its mUserArea and moves it left by mCompassClock's original left + width together with mCompassClock; drawn by dMeter_Draw 02593630; TWW dMeter_compassMove turns the needles about it and dMeter_compassDirOpen / WindOpen / WindClose count its mUserArea as their open and close frame |
| 0x194C | `field_0x1948` | `mCompassClock` | yes | pane 'cmtm' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C): 02593434 (d_meter_00.cpp dMeter_gaugeInitialPositionHD; the same code as TWW dMeter_compassValueInit) moves it and mCompassBase left by its own original left + width (mPosTopLeftOrig.x + mSizeOrig.x); drawn by dMeter_Draw 02593630; TWW dMeter_compassValueInit also turns it by 180 degrees, dMeter_clockShow gives it the day or night corner colours and its mUserArea is dMeter_compassGetOnProc's wind-open request |

## Batch K1: the clock panes

### sub_meter_class (d_meter_02.h)

The clock screen sMainParts3 has two clocks: the running one (shown while the room's time passes) and the fixed
one (shown otherwise, with a day and a night set). WWHD only sets the panes up (dMeter_screenDataSet in
dMeter_Create 02595C6C) and draws them (dMeter_Draw 02593630); TWW, which writes them (dMeter_clockShow, clockInit,
clockMove, clockMultiMove, compassValueInit, compassDirOpen / WindOpen), gives the roles. The `mClock` names are
the running clock, the `mFixedClock` names the fixed one; no view of sub_meter_class declares these offsets.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x139C | `field_0x1398` | `mClockBar` | pane 'bar' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise, dMeter_compassAlpha fades it with the clock (tww writes it, the same code) |
| 0x13D4 | `field_0x13d0` | `mClockRing` | pane 'rng2' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise; TWW dMeter_compassDirOpen / WindOpen / DirClose / WindClose / ValueInit turn mCompassClock and mClockCenter about a point offset by half its width; the ring of the running clock, 'rng1' mFixedClockRing is the fixed one (tww writes it, the same code) |
| 0x140C | `field_0x1408` | `mClockHorizon1` | pane 'hrz1' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise, dMeter_compassAlpha fades it (tww writes it, the same code) |
| 0x1444 | `field_0x1440` | `mClockHorizon2` | pane 'hrz2' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise, dMeter_compassAlpha fades it (tww writes it, the same code) |
| 0x147C | `field_0x1478` | `mClockStar` | panes 'str5'..'str7' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise; TWW dMeter_clockInit sets each mUserArea to 1 and dMeter_clockMove twinkles them with dMeter_clockStarLight while the time passes (mFixedClockStar while it does not) (tww writes them, the same code) |
| 0x1524 | `field_0x1520` | `mClockMoon` | pane 'mon2' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise ('mon1' mFixedClockMoon is the fixed clock's moon) (tww writes it, the same code) |
| 0x155C | `field_0x1558` | `mClockSun` | pane 'sun2' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise ('sun1' mFixedClockSun is the fixed clock's sun) (tww writes it, the same code) |
| 0x1594 | `field_0x1590` | `mClockCircle` | panes 'cir1'..'cir4' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise, dMeter_compassAlpha fades them (tww writes them, the same code) |
| 0x1674 | `field_0x1670` | `mClockTimeDial` | pane 'cnt2' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise; TWW dMeter_compassGetOnProc turns it about its own centre by -(time of day) - 90 degrees (dMeter_compassRotate), so it shows the time (tww writes it, the same code) |
| 0x16AC | `field_0x16a8` | `mClockCenter` | pane 'cnt1' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise; TWW dMeter_screenDataSet clears its white colour, dMeter_compassDirOpen / WindOpen / DirClose / WindClose / ValueInit slide it with mCompassBase and mCompassClock and turn it by 180 degrees with mCompassClock (tww writes it, the same code) |
| 0x16E4 | `field_0x16e0` | `mFixedClockRing` | pane 'rng1' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit hide it while the room's time passes and show it otherwise, by day and by night: the ring of the fixed clock ('rng2' mClockRing is the running one) (tww writes it, the same code) |
| 0x171C | `field_0x1718` | `mFixedClockStar` | panes 'str1'..'str4' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit hide it while the room's time passes and show it otherwise at night only; TWW dMeter_clockInit sets each mUserArea to 1 and dMeter_clockMove twinkles them with dMeter_clockStarLight while the time does not pass (tww writes them, the same code) |
| 0x17FC | `field_0x17f8` | `mFixedClockMoon` | pane 'mon1' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit hide it while the room's time passes and show it otherwise at night only (between 270 and 90 degrees of the day) (tww writes it, the same code) |
| 0x1834 | `field_0x1830` | `mFixedClockCloudClip` | pane 'clp' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit hide it while the room's time passes and show it otherwise by day only; TWW dMeter_screenDataSet clears its white colour, dMeter_clockInit and dMeter_clockMultiMove take mCompassClock's origin + its origin and size as the rectangle the three cloud layers are drawn into (tww writes it, the same code) |
| 0x186C | `field_0x1868` | `mFixedClockCloud` | panes 'cld1'..'cld3' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW hides them always and uses mPosCenter as data: dMeter_clockInit sets it from mCompassClock + mFixedClockCloudClip's origin, dMeter_clockMove from HIO 0x6C / 0x72, and dMeter_clockMultiMove passes it as the scroll of cloud layer clock[i] (tww writes them, the same code) |
| 0x1914 | `field_0x1910` | `mFixedClockSun` | pane 'sun1' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit hide it while the room's time passes and show it otherwise by day only; its mUserArea is a 0..120 frame cycle (TWW dMeter_clockInit clears it, dMeter_clockMove steps it) (tww writes it, the same code) |

## Batch P1: the menu guide and enemy gauge panes

### MeterPrefixHD (d_meter_01.h), the first 0x644 bytes of sub_meter_class

MeterPrefixHD is d_meter_01.cpp's view of the start of sub_meter_class (which declares this range as before_mHeart). WWHD sets the panes up (dMeter_screenDataSet in dMeter_Create 02595C6C), initialises some mUserArea counters (dMeter_menuInit / dMeter_enemyInit, inlined into dMeter_Create) and draws them (dMeter_Draw 02593630); TWW (dMeter_menuLRMove, dMeter_menuPlusMove, dMeter_bossEnemyMove / Hide, dMeter_bossAnime, dMeter_bossEyeAnime, dMeter_zakoEnemyMove / Hide), which writes them, gives the roles. Open: `field_0x01a8`, `field_0x01e0`, `field_0x0218`, `field_0x0330` (never set up or used in WWHD or tww).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x0104 | `field_0x0100` | `mMapCrossArrow` | panes 'yrig' / 'yup' / 'ylef' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C) (moved 2 / 2 / 1 units there), drawn by dMeter_Draw 02593630: the right, up and left arrows of the map cross-key guide; dMeter_menuInit and dMeter_Create store dComIfGs_isDungeonItemCompass in [0].mUserArea; TWW dMeter_menuPlusMove re-checks that flag, shows [0..2] by the map / compass state, places them around the map or the compass and fades them with mMapCrossKey (tww writes it, the same code) |
| 0x0254 | `field_0x0250` | `mMenuArrowR` | pane 'r' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_menuInit (in dMeter_Create) sets its mUserArea to 10; TWW dMeter_menuLRMove slides it right by up to 20 and fades it while the menu button is pushed, its mUserArea being that slide frame (tww writes it, the same code) |
| 0x028C | `field_0x0288` | `mMenuArrowL` | pane 'l' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_menuInit (in dMeter_Create) clears its mUserArea; TWW dMeter_menuLRMove slides it left by up to 20 with mMenuArrowR; its mUserArea keeps both fully visible after a menu button change (tww writes it, the same code) |
| 0x02C4 | `field_0x02c0` | `mMapCrossKey` | pane 'crs1' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_menuInit (in dMeter_Create) clears its mUserArea; TWW dMeter_menuPlusMove places it at HIO 0x9C / 0x9E, shows it by the map state and fades it (and the map) by its mUserArea, a 0..5 frame counter (tww writes it, the same code) |
| 0x02FC | `field_0x02f8` | `mMapWindArrow` | pane 'wiy1' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; TWW dMeter_menuPlusMove turns it to the wind direction (dKyw_get_wind_vec), places it at the map corner + HIO 0x64 / 0x66 and hides it unless the wind is in use (dKyw_gbwind_use_check) (tww writes it, the same code) |
| 0x036C | `field_0x0368` | `mBossIconEye1` | pane 'bey1' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_enemyInit (in dMeter_Create) clears its mUserArea; TWW dMeter_bossEnemyMove / Hide fade it with the boss gauge, dMeter_bossAnime moves it with mBossIconTop and dMeter_bossEyeAnime blinks it, its mUserArea counting up to mBossIconEye2's (tww writes it, the same code) |
| 0x03A4 | `field_0x03a0` | `mBossIconEye2` | pane 'bey2' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_enemyInit (in dMeter_Create) sets its mUserArea to (40 + rnd 18) * 2, the blink period; TWW dMeter_bossEyeAnime blinks it with mBossIconEye1 and rolls a new period after each blink (tww writes it, the same code) |
| 0x03DC | `field_0x03d8` | `mBossIconTop` | pane 'enem' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_enemyInit (in dMeter_Create) clears its mUserArea; TWW dMeter_bossAnime moves it up (and mBossIconBottom down) by a curve over its mUserArea frame, which restarts at mBossIconBottom's period; dMeter_bossEnemyMove / Hide fade it (tww writes it, the same code) |
| 0x0414 | `field_0x0410` | `mBossIconBottom` | pane 'enm2' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_enemyInit (in dMeter_Create) sets its mUserArea to 300 + rnd 200, the period of dMeter_bossAnime, which TWW re-rolls each cycle; TWW dMeter_bossAnime moves it down opposite mBossIconTop, dMeter_bossEnemyMove / Hide fade it (tww writes it, the same code) |
| 0x044C | `field_0x0448` | `mBossLifeBar` | panes 'ben1'..'ben4' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_enemyInit (in dMeter_Create) clears [1].mUserArea; TWW dMeter_bossEnemyMove opens the boss life gauge over [0].mUserArea (0..30 frames), fades it in over [1].mUserArea (0..10, faded out by dMeter_bossEnemyHide) and scales [2] / [3] in x by the boss's health / max_health, right-aligned (tww writes it, the same code) |
| 0x052C | `field_0x0528` | `mEnemyLifePart3` | pane 'ene3' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; TWW dMeter_zakoEnemyMove keeps it at its layout offset from mEnemyLifeBar beside the lock-on target and fades it with it (dMeter_zakoEnemyHide) (tww writes it, the same code) |
| 0x0564 | `field_0x0560` | `mEnemyLifeBar` | pane 'ene2' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_enemyInit (in dMeter_Create) clears its mUserArea; TWW dMeter_zakoEnemyMove places it 50 beside the lock-on target's projected attention position, sizes its width by the target's health / max_health and fades it in over its mUserArea (0..10, out in dMeter_zakoEnemyHide) (tww writes it, the same code) |
| 0x059C | `field_0x0598` | `mEnemyLifePart1` | pane 'ene1' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; TWW dMeter_zakoEnemyMove keeps it at its layout offset from mEnemyLifeBar and fades it with it (dMeter_zakoEnemyHide) (tww writes it, the same code) |
| 0x05D4 | `field_0x05d0` | `mEnemyLifeShadow2` | pane 'enk2' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C) (the 'k' tags are the shadow panes); not in dMeter_Draw's list; TWW dMeter_zakoEnemyMove only resets it to its layout position and size (tww writes it, the same code) |
| 0x060C | `field_0x0608` | `mEnemyLifeShadow` | pane 'enek' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630 (the 'k' tags are the shadow panes); TWW dMeter_zakoEnemyMove keeps it at its layout offset from mEnemyLifeBar and fades it with it (dMeter_zakoEnemyHide) (tww writes it, the same code) |

## Batch P2: R button, arrow type, swim timer, road arrows and the saved colours

### sub_meter_class (d_meter_02.h)

WWHD sets most of these panes up (dMeter_screenDataSet / dMeter_screenDataArrowSet in dMeter_Create 02595C6C) and draws them (dMeter_Draw 02593630); the swim timer panes (sScrTimer1 / sScrTimer2, set up by TWW dMeter_screenDataTimeSet) are only drawn. The roles come from WWHD's dMeter_rAlpha 0259A5B0 and heart move (dMeter_Execute 02593B10) and from TWW, which writes the others (dMeter_rMove, dMeter_arwInit / arwMove, dMeter_swim*, dMeter_arrow*, dMeter_magicInit / magicGaugeMove, dMeter_clockMove, dMeter_metronomeMove, dMeter_mapMove, dMeter_compass*). The WWHD offset is the GameCube name + 4 (the message base grows by four bytes).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x1CCC | `field_0x1cc8` | `mRActionIcon` | pane 'basv' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_rInit (in dMeter_Create 02595C6C) clears its mUserArea; dMeter_rAlpha 0259A5B0 sets its mUserArea to 1 when the R status differs from mRStatusShown, then clears it and stores the status there, and fades it by the R pane counter (hidden while there is no R status); TWW dMeter_rMove changes its texture to the R action icon on that request (tww the same) |
| 0x26A4 | `field_0x26a0` | `mRButton` | pane 'bar1' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630: the R button ('ba' + button letter, as 'baz' mZButton); dMeter_rAlpha 0259A5B0 fades it with mRActionIcon; TWW dMeter_rTrans moves it (tww the same) |
| 0x3020 | `field_0x301c` | `mRStatusShown` | dMeter_rInit (in dMeter_Create 02595C6C) clears it; dMeter_rAlpha 0259A5B0 stores dComIfGp_getRStatus() when it takes mRActionIcon's change request and requests a change while the R status differs from it (TWW dMeter_rMove the same) |
| 0x2944 | `field_0x2940` | `mArrowTypeIcon` | panes 'arw1' / 'arw2' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C); dMeter_arwInit (in dMeter_Create 02595C6C) sets their alpha to zero; TWW dMeter_arwInit / dMeter_arwMove load the old ([0]) and the new ([1]) arrow type's icon texture into them when the kept arrow type changes while aiming the bow (tww writes it, the same code) |
| 0x29B4 | `field_0x29b0` | `mArrowTypeSlot` | panes 'ip00' / 'ip02' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C); dMeter_arwInit (in dMeter_Create 02595C6C) sets their alpha to zero; TWW dMeter_arwMove slides between their centres on an arrow type change ([0].mUserArea the slide frame, [1].mUserArea the change count up to 2) and puts the arrow particles at [0] (tww writes it, the same code) |
| 0x2F28 | `field_0x2f24` | `mpArrowTypeParticle` | the two 2D particle emitters (fire / ice / light arrow, ID_HM_J2_ARW*) on the arrow type icons: dMeter_arwInit (in dMeter_Create 02595C6C) clears them, dMeter_Delete ends them with mpRupyParticle; TWW dMeter_arwMove creates them at mArrowTypeSlot[0] and ends them on a type change (tww the same) |
| 0x2A94 | `field_0x2a90` | `mSwimTimerLight` | panes 'hk11'..'hk33' of the swim timer screen sScrTimer2 (TWW dMeter_screenDataTimeSet; 'hk' = hikari, light), drawn by dMeter_Draw 02593630 while sScrTimer2 exists; TWW dMeter_swimLightBirth starts one at the top of mSwimTimerGauge (mUserArea 1), dMeter_swimLightAnime rises it by mSwimLightRiseSpeed and sways it by mSwimLightSwayX (tww writes it, the same code) |
| 0x2C8C | `field_0x2c88` | `mSwimTimerFrame` | pane 'wk00' of the swim timer screen sScrTimer2 (TWW dMeter_screenDataTimeSet; 'wk' = waku, frame), drawn by dMeter_Draw 02593630; TWW dMeter_swimInit saves its colours in mSwimFrameBlack / mSwimFrameWhite, dMeter_swimMainBlink blinks them, its mUserArea is the open-animation frame (dMeter_swimOpenProc) and then mSwimTimerSoul's rotation frame (dMeter_swimMainRotate) (tww writes it, the same code) |
| 0x2CC4 | `field_0x2cc0` | `mSwimTimerMask2` | pane 'msk2' of the swim timer screen sScrTimer2 (TWW dMeter_screenDataTimeSet), drawn by dMeter_Draw 02593630; TWW only moves (dMeter_swimTransY) and hides it with the timer (tww writes it, the same code) |
| 0x2CFC | `field_0x2cf8` | `mSwimTimerSoul` | pane 'soul' of the swim timer screen sScrTimer1 (TWW dMeter_screenDataTimeSet), drawn by dMeter_Draw 02593630; TWW dMeter_swimMainRotate turns it once per HIO 0x132 frames, dMeter_swimMainDown keeps it at mSwimTimerMain's height, its mUserArea is dMeter_swimMainBlink's blink counter (tww writes it, the same code) |
| 0x2D34 | `field_0x2d30` | `mSwimTimerGauge` | pane 'msk1' of the swim timer screen sScrTimer1 (TWW dMeter_screenDataTimeSet), drawn by dMeter_Draw 02593630; TWW dMeter_swimMove stores the item time count in its mUserArea when the timer starts and dMeter_swimMainDown scales it from the bottom by the remaining count / that start count (tww writes it, the same code) |
| 0x2DA4 | `field_0x2da0` | `mSwimTimerMain` | pane 'main' of the swim timer screen sScrTimer1 (TWW dMeter_screenDataTimeSet), drawn by dMeter_Draw 02593630; its mUserArea is the swim timer state in TWW dMeter_swimMove (0 off, 1 opening, 2 / 3 running); dMeter_swimMainDown lowers it with the gauge (tww writes it, the same code) |
| 0x2DDC | `field_0x2dd8` | `mSwimTimerShadow` | pane 'kg00' of the swim timer screen sScrTimer1 (TWW dMeter_screenDataTimeSet; 'kg' = kage, shadow), drawn by dMeter_Draw 02593630; TWW dMeter_swimOpenProc pops it in by scale, its mUserArea is dMeter_swimMainBlink's colour blink frame (tww writes it, the same code) |
| 0x2E14 | `field_0x2e10` | `mRoadArrow` | panes 'yaz4'..'yaz1' on sChoiceRoad (dMeter_screenDataArrowSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_arrowInit (in dMeter_Create 02595C6C) clears each mUserArea; TWW dMeter_arrowCheckStatus shows (mUserArea 1) or erases each direction arrow, dMeter_arrowTransScale / dMeter_arrowAnime place and animate the shown ones (tww writes it, the same code) |
| 0x2EF4 | `field_0x2ef0` | `mSwimFrameBlack` | TWW dMeter_swimInit saves mSwimTimerFrame's black colour here; dMeter_swimMove restores it, dMeter_swimMainBlink blinks from it to mSwimFrameBlinkBlack; no WWHD use (tww writes it) |
| 0x2EF8 | `field_0x2ef4` | `mSwimFrameWhite` | TWW dMeter_swimInit saves mSwimTimerFrame's white colour here; dMeter_swimMove restores it, dMeter_swimMainBlink blinks from it to mSwimFrameBlinkWhite; no WWHD use (tww writes it) |
| 0x2EFC | `field_0x2ef8` | `mSwimFrameBlinkBlack` | the black colour mSwimTimerFrame blinks to: TWW dMeter_swimMove copies mSwimFrameBlack, dMeter_swimMainBlink sets HIO 0x14A..0x14C below half the time and blends towards it; no WWHD use (tww writes it) |
| 0x2F00 | `field_0x2efc` | `mSwimFrameBlinkWhite` | the white colour mSwimTimerFrame blinks to: TWW dMeter_swimMove copies mSwimFrameWhite, dMeter_swimMainBlink sets the HIO colour below half the time and blends towards it; no WWHD use (tww writes it) |
| 0x2F04 | `field_0x2f00` | `mSwimMainBlack` | TWW dMeter_swimInit saves mSwimTimerMain's black colour here; dMeter_swimMove gives it to mSwimTimerSoul and mSwimTimerMain, dMeter_swimMainBlink blinks from it; no WWHD use (tww writes it) |
| 0x2F08 | `field_0x2f04` | `mSwimMainBlinkColor` | the black colour mSwimTimerMain / mSwimTimerSoul blink to: TWW dMeter_swimMove sets (200, 200, 255), dMeter_swimMainBlink sets HIO 0x14A..0x14C below half the time; no WWHD use (tww writes it) |
| 0x2F0C | `field_0x2f08` | `mMagicGaugeWhite` | TWW dMeter_magicInit saves mMagicGauge1's white colour here; dMeter_magicGaugeMove gives it back to mMagicGauge1 / mMagicGauge2, dMeter_magicColor blends from it to white; no WWHD use (tww writes it) |
| 0x2F10 | `field_0x2f0c` | `mMagicGaugeBlack` | TWW dMeter_magicInit saves mMagicGauge1's black colour here; dMeter_magicGaugeMove gives it back to mMagicGauge1 / mMagicGauge2, dMeter_magicColor blends from it; no WWHD use (tww writes it) |
| 0x2F14 | `field_0x2f10` | `mMagicBarEvenWhite` | TWW dMeter_magicInit saves mMagicBar[0]'s white colour here; dMeter_magicGaugeMove gives it back to mMagicBar[0, 2, 4, 6]; no WWHD use (tww writes it) |
| 0x2F18 | `field_0x2f14` | `mMagicBarEvenBlack` | TWW dMeter_magicInit saves mMagicBar[0]'s black colour here; dMeter_magicGaugeMove gives it back to mMagicBar[0, 2, 4, 6]; no WWHD use (tww writes it) |
| 0x2F1C | `field_0x2f18` | `mMagicBarOddWhite` | TWW dMeter_magicInit saves mMagicBar[1]'s white colour here; dMeter_magicGaugeMove gives it back to mMagicBar[1, 3, 5, 7]; no WWHD use (tww writes it) |
| 0x2F20 | `field_0x2f1c` | `mMagicBarOddBlack` | TWW dMeter_magicInit saves mMagicBar[1]'s black colour here; dMeter_magicGaugeMove gives it back to mMagicBar[1, 3, 5, 7]; no WWHD use (tww writes it) |
| 0x2F78 | `field_0x2f74` | `mSwimLightRiseSpeed` | per mSwimTimerLight: TWW dMeter_swimLightBirth sets 5 + rnd 3, dMeter_swimLightAnime moves the light up by it and slows it by 0.8..1.0 each frame; no WWHD use (tww writes it) |
| 0x2F9C | `field_0x2f98` | `mSwimLightSwayX` | per mSwimTimerLight: TWW dMeter_swimLightBirth sets rndFX 5, dMeter_swimLightAnime sways the light by it * sin around its x and shrinks it by 0.9..1.0 each frame; no WWHD use (tww writes it) |
| 0x2FC8 | `field_0x2fc4` | `mSwimTekariFrame` | TWW dMeter_swimMove clears it, dMeter_swimTekariScroll counts it as the frame of the shine scroll over mSwimTimerFrame and wraps it; no WWHD use (tww writes it) |
| 0x2FE8 | `field_0x2fe4` | `mStarLightFrames0` | s16 pair (the second half is the 2 bytes after it): TWW dMeter_starLightFrame rolls the light-up and end frames (HIO 0x90 / 0x88 + random), dMeter_clockMove / dMeter_clockStarLight twinkle mClockStar[0] / mFixedClockStar[0] by them; no WWHD use (tww writes it) |
| 0x2FEC | `field_0x2fe8` | `mStarLightFrames1` | s16 pair: as mStarLightFrames0 for mClockStar[1] / mFixedClockStar[1] (TWW dMeter_clockMove); no WWHD use (tww writes it) |
| 0x2FF0 | `field_0x2fec` | `mStarLightFrames2` | s16 pair: as mStarLightFrames0 for mClockStar[2] / mFixedClockStar[2] (TWW dMeter_clockMove); no WWHD use (tww writes it) |
| 0x2FF4 | `field_0x2ff0` | `mStarLightFrames3` | s16 pair: as mStarLightFrames0 for mFixedClockStar[3] (TWW dMeter_clockMove); no WWHD use (tww writes it) |
| 0x3004 | `field_0x3000` | `mSwimBlinkPeriod` | the half period of the swim timer colour blink: TWW dMeter_swimMove sets HIO 0x130, dMeter_swimMainBlink blends over it and below half the time moves it towards HIO 0x134 (0 when the time is out); no WWHD use (tww writes it) |
| 0x3010 | `field_0x300c` | `mMaxLifeChange` | the heart containers still to add or remove: dMeter_Execute 02593B10 (the heart move inlined) sets the new max life - mMaxHP when the max-life item count arrives and steps mMaxHP by 1 towards it each frame (TWW dMeter_LifeMove / dMeter_maxLifeChange the same) |
| 0x3014 | `field_0x3010` | `mSwimLightTimer` | the frames until the next mSwimTimerLight: TWW dMeter_swimMove clears it, dMeter_swimLightMove counts it down and births a light at 0, dMeter_swimLightBirth rolls rnd(HIO 0x138); no WWHD use (tww writes it) |
| 0x3029 | `field_0x3025` | `mRoadArrowFrame` | dMeter_arrowInit (in dMeter_Create 02595C6C) clears it; TWW dMeter_arrowMove counts it to 30 and back to 10, dMeter_arrowAnime animates mRoadArrow by it and dMeter_arrowCheckStatus changes the arrows only at 0 or 20 (tww writes it, the same code) |
| 0x302C | `field_0x3028` | `mMetronomeState` | dMeter_Create 02595C6C clears it; TWW dMeter_metronomeMove: 1 while the metronome (dMn_c) is open, 2 while it closes, 0 once deleted; dMeter_Draw draws it only at 1 (tww writes it, the same code) |
| 0x302D | `field_0x3029` | `mMapOn` | 1 while the map is switched on: TWW dMeter_compassInit sets it from dMeter_mMapCtrlDisp, dMeter_mapMove sets 1 / 0 on the d-pad right / left (JA_SE_MAP_SHOW / CANCEL), dMeter_menuPlusMove / dMeter_compassGetOnProc set it; dMeter_mapMove tests it; no WWHD use (tww writes it) |
| 0x302E | `field_0x302a` | `mCompassSlideSe` | 1 while the compass slide sound runs: TWW dMeter_compassDirOpen / WindOpen / WindClose / DirClose start JA_SE_COMPASS_MOVE and set it, clear it with JA_SE_MAP_SHOW_END at the end; dMeter_compassValueInit clears it; no WWHD use (tww writes it) |

## Batch H1: dMeter_map_HIO_c, the map positions

### dMeter_map_HIO_c (d_meter_01.h)

The fields WWHD reads (mapCtrlDisp_c::initMapCtrlDisp 0259359C, dMeter_Create 02595C6C). The declarations were split one per line in their own commit first. Open: `field_0x5` (constructor 0 only), `field_0x7` (constructor 1 only); `field_0x6` / `field_0x10` are named in batch H6.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x04 | `field_0x8` | `mMapPosX` | the map's left x when shown: mapCtrlDisp_c::initMapCtrlDisp 0259359C copies it into l_mapState mPosLeftUpX; dMeter_Create 02595C6C starts the slide offset at field_0xc - it; constructor 02598254 default 0x23 (tww the same; TWW mapCtrlDisp_c::moveMapCtrlDisp sets the map x to it + the slide offset) |
| 0x06 | `field_0xa` | `mMapPosY` | the map's top y: mapCtrlDisp_c::initMapCtrlDisp 0259359C copies it into l_mapState mPosLeftUpY; constructor 02598254 default 0x152 (tww the same; TWW moveMapCtrlDisp sets the map y to it, raised when the scissor ends above 452) |
| 0x08 | `field_0xc` | `mMapHiddenPosX` | the map's left x when hidden: dMeter_Create 02595C6C sets mapCtrlDisp field_0x4 (the slide offset) to it - mMapPosX, so the map starts hidden; constructor 02598254 default -0xB4 (tww the same; TWW moveMapCtrlDisp eases the offset to it - mMapPosX while the map is off and to 0 while it is on) |
| 0x10 | `field_0x14` | `mIconFreePosX` | mapCtrlDisp_c::initMapCtrlDisp 0259359C copies it into l_mapState mIconFreePosX; constructor 02598254 default 0x24E (tww initMapCtrlDisp dMap_setIconFreePosX the same) |
| 0x12 | `field_0x16` | `mIconFreePosY` | mapCtrlDisp_c::initMapCtrlDisp 0259359C copies it into l_mapState mIconFreePosY; constructor 02598254 default 0x96 (tww initMapCtrlDisp dMap_setIconFreePosY the same) |

## Batch P3: the clock and compass state (written in WWHD, read in tww)

### sub_meter_class (d_meter_02.h) and its view dMeterPrefixHD (d_meter_00.h)

WWHD writes these in 02593434 (d_meter_00.cpp, the same code as TWW dMeter_compassValueInit) and never reads them; TWW reads them in dMeter_compassWindClose / AnimeMove / GetOnProc / menuPlusMove / Draw. Named after TWW's reads (see the rule in batch H2).

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x2F6C | `field_0x2f68` | `mClockTimePass` | 02593434 (d_meter_00.cpp dMeter_gaugeInitialPositionHD; TWW dMeter_compassValueInit / clockShow the same) sets 1.0 while the room's time passes (roomControl_getTimePass) and 0.0 otherwise; TWW dMeter_compassWindClose / dMeter_compassAnimeMove compare it with the current state to rebuild the clock |
| 0x3016 | `field_0x3012` | `mClockNight` | 02593434 (d_meter_00.cpp; TWW dMeter_compassValueInit / clockShow the same) sets 0 between 90 and 270 degrees of the day and 1 otherwise; TWW dMeter_compassWindClose / dMeter_compassAnimeMove rebuild the fixed clock when it changes |
| 0x3024 | `field_0x3020` | `mCompassState` | the compass panel state: 02593434 (d_meter_00.cpp; TWW dMeter_compassValueInit) sets 1; TWW dMeter_compassGetOnProc / GetOffProc switch on it and GetOnProc sets 2 / 5, GetOffProc 9 / 8, dMeter_compassDirOpen 3, WindOpen 7, WindClose 6 / 3, AnimeMove 8; dMeter_menuPlusMove, dMeter_mapMove and dMeter_Draw test it |

## Batch H2: dMeter_HIO_c, the fields tww reads

### dMeter_HIO_c (d_meter_01.h)

Rule (2026-11-11): a field WWHD writes only in a constructor / init and only tww reads may take its name from tww's reads when they give one clear role. WWHD writes all of these only in the constructor 025974DC; the evidence is tww's read. 0xA0..0x11F became the array mButtonPos[8][2][4] in its own type-change commit first. Still open: `field_0x138` (tww uses it both as the swim light hold length and as the random range of the next light's delay: two roles) and the fields read nowhere in WWHD or tww: `field_0x14`, `0x18`, `0x1c`, `0x20`, `0x2d`, `0x2e`, `0x30`..`0x38`, `0x42`, `0x54`, `0x56`, `0x60`, `0x61`, `0x68`, `0x6a`, `0x121`..`0x123`, `0x126`..`0x12e`, `0x13e`, `0x140`, `0x142`..`0x149`.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x10 | `field_0x10` | `mHeartScale` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_setNowHeartScaleXY sizes the current heart to its original size * it (tww default 1.4) (tww d_meter.cpp unless named; no WWHD read) |
| 0x24 | `field_0x24` | `mShutterBladeAngle` | WWHD writes it only in the constructor 025974DC; tww reads it: d_picture_box.cpp dJle_Pb_c::clickShutterMode / shutterChange turn each of the 12 shutter blades by its base angle + it * the shutter progress (tww default 150.0) (tww d_meter.cpp unless named; no WWHD read) |
| 0x3C | `field_0x3c` | `mHeartLightScale` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_heartLightMove scales the heart light by 1 + it * the pulse value (tww default 0.65) (tww d_meter.cpp unless named; no WWHD read) |
| 0x40 | `field_0x40` | `mButtonScale` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_Execute scales the button group about mButtonCenter by it / 100 (dMeter_parentPaneTrans / dMeter_childPaneTrans for the weapon, action and item buttons; tww default 0x5A) (tww d_meter.cpp unless named; no WWHD read) |
| 0x44 | `field_0x44` | `mButtonTextScale` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_Execute passes it / 100 as the second scale of dMeter_childPaneTransOnly for mWeaponText and mActionText (tww default 100) (tww d_meter.cpp unless named; no WWHD read) |
| 0x46 | `field_0x46` | `mButtonIconScale` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_Execute passes it / 100 as the second scale for mWeaponIcon / mWeaponIconShadow and the X / Y / Z mItemIcon, digits and shadows (dMeter_childPaneTransOnly / ChildTrans; tww default 0x5A) (tww d_meter.cpp unless named; no WWHD read) |
| 0x48 | `field_0x48` | `mBattleHeartScale` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_setHeartScaleXY2 / dMeter_setNowHeartScaleXY2, used by dMeter_BattleLifeMove, multiply the heart size by it (tww default 1.8) (tww d_meter.cpp unless named; no WWHD read) |
| 0x4C | `field_0x4c` | `mBattleLifePosX` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_BattleLifeMove adds it to the hearts' original x (tww default 12.0) (tww d_meter.cpp unless named; no WWHD read) |
| 0x50 | `field_0x50` | `mButtonOffsetX` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_parentPaneTrans and the dMeter_childPaneTrans* helpers add it to the button panes' x (tww default 7) (tww d_meter.cpp unless named; no WWHD read) |
| 0x52 | `field_0x52` | `mButtonOffsetY` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_parentPaneTrans and the dMeter_childPaneTrans* helpers add it to the button panes' y (tww default -12) (tww d_meter.cpp unless named; no WWHD read) |
| 0x58 | `field_0x58` | `mBattleLifeStepX` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_BattleLifeMove spaces the hearts by it * scale * index in x (tww default 2.3) (tww d_meter.cpp unless named; no WWHD read) |
| 0x5C | `field_0x5c` | `mBattleLifePosY` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_BattleLifeMove adds it to the hearts' original y (tww default 12.0) (tww d_meter.cpp unless named; no WWHD read) |
| 0x64 | `field_0x64` | `mMapWindArrowOffsetX` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_menuPlusMove places mMapWindArrow at the map corner x - 7 + it (tww d_meter.cpp unless named; no WWHD read) |
| 0x66 | `field_0x66` | `mMapWindArrowOffsetY` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_menuPlusMove places mMapWindArrow at the map corner y + 9 + it (tww d_meter.cpp unless named; no WWHD read) |
| 0x6C | `field_0x6c` | `mClockCloudPosX` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_clockMove sets mFixedClockCloud[i].mPosCenter.x (the scroll of cloud layer i) to [i] (tww d_meter.cpp unless named; no WWHD read) |
| 0x72 | `field_0x72` | `mClockCloudPosY` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_clockMove sets mFixedClockCloud[i].mPosCenter.y (the scroll of cloud layer i) to [i] (tww d_meter.cpp unless named; no WWHD read) |
| 0x78 | `field_0x78` | `mClockCloudScale` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_clockMultiMove scales cloud layer clock[i] by 100 / [i] (percent) (tww d_meter.cpp unless named; no WWHD read) |
| 0x7E | `field_0x7e` | `mSwimLightSize` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimLightAnime sets each mSwimTimerLight's width and height to it (tww default 0x14) (tww d_meter.cpp unless named; no WWHD read) |
| 0x9C | `field_0x9c` | `mMapCrossKeyPosX` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_menuPlusMove places mMapCrossKey (and the map cross arrows around it) at it, + 0x52 while the compass is open (tww d_meter.cpp unless named; no WWHD read) |
| 0x9E | `field_0x9e` | `mMapCrossKeyPosY` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_menuPlusMove places mMapCrossKey (and the map cross arrows around it) at it (tww d_meter.cpp unless named; no WWHD read) |
| 0xA0 | `field_0xa0` | `mButtonPos` | WWHD writes it only in the constructor 025974DC; tww reads it: 8 rows (one per HUD layout state) of 4 x then 4 y positions: dMeter_actionTrans reads column 0 (A, its second / exp position from column 2 in rows 1, 4, 6), dMeter_weponTrans column 1 (B, also column 3 in row 6), dMeter_xyTrans column 2 (X / Y / Z, rows 2, 5, 7), dMeter_rTrans column 3 (R, rows 1, 4, 5, 7), each by the layout's moveStatus (tww d_meter.cpp unless named; no WWHD read) |
| 0x120 | `field_0x120` | `mShutterLineAlpha` | WWHD writes it only in the constructor 025974DC; tww reads it: d_picture_box.cpp dJle_Pb_c::draw passes it to shutterLineDraw as the alpha of the 12 shutter lines (tww default 0x8C) (tww d_meter.cpp unless named; no WWHD read) |
| 0x124 | `field_0x124` | `mShutterFrames` | WWHD writes it only in the constructor 025974DC; tww reads it: d_picture_box.cpp dJle_Pb_c::clickShutterMode / changePicture / shutterChange: the shutter closes over it frames and opens over the next it (tww default 5) (tww d_meter.cpp unless named; no WWHD read) |
| 0x130 | `field_0x130` | `mSwimBlinkPeriodStart` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimMove / dMeter_swimMainBlink set mSwimBlinkPeriod to it while at least half the time is left and ease it towards mSwimBlinkPeriodEnd below that (tww default 0x14) (tww d_meter.cpp unless named; no WWHD read) |
| 0x132 | `field_0x132` | `mSwimSoulTurnFrames` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimMainRotate turns mSwimTimerSoul once around in it frames (tww default 0x27) (tww d_meter.cpp unless named; no WWHD read) |
| 0x134 | `field_0x134` | `mSwimBlinkPeriodEnd` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimMainBlink eases mSwimBlinkPeriod towards it as the time runs out (tww default 3) (tww d_meter.cpp unless named; no WWHD read) |
| 0x136 | `field_0x136` | `mSwimLightFadeFrames` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimLightAnime fades each mSwimTimerLight in over its first it frames (tww default 4) (tww d_meter.cpp unless named; no WWHD read) |
| 0x13A | `field_0x13a` | `mSwimTimerOffsetY` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimInit sets the swim timer panes' y offset to it + 32 (dMeter_swimTransY; tww default 0) (tww d_meter.cpp unless named; no WWHD read) |
| 0x13C | `field_0x13c` | `mSwimLightStartY` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimLightBirth starts a new mSwimTimerLight at mSwimTimerGauge's top + it (tww default 10) (tww d_meter.cpp unless named; no WWHD read) |
| 0x14A | `field_0x14a` | `mSwimLowTimeR` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimMainBlink: red of the blink colour below half the time (mSwimMainBlinkColor / mSwimFrameBlink*; tww default 0xFF) (tww d_meter.cpp unless named; no WWHD read) |
| 0x14B | `field_0x14b` | `mSwimLowTimeG` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimMainBlink: green of the blink colour below half the time (tww default 0x32) (tww d_meter.cpp unless named; no WWHD read) |
| 0x14C | `field_0x14c` | `mSwimLowTimeB` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimMainBlink: blue of the blink colour below half the time (tww default 0x32) (tww d_meter.cpp unless named; no WWHD read) |
| 0x14E | `field_0x14e` | `mSwimTekariSweepFrames` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimTekariScroll: each of the three shine sweeps over mSwimTimerFrame lasts it frames (tww default 10) (tww d_meter.cpp unless named; no WWHD read) |
| 0x150 | `field_0x150` | `mSwimTekariPause1` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimTekariScroll: the pause after the first shine sweep (tww default 0x14) (tww d_meter.cpp unless named; no WWHD read) |
| 0x152 | `field_0x152` | `mSwimTekariPause2` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimTekariScroll: the pause after the second shine sweep (tww default 4) (tww d_meter.cpp unless named; no WWHD read) |
| 0x154 | `field_0x154` | `mSwimTekariPauseEnd` | WWHD writes it only in the constructor 025974DC; tww reads it: dMeter_swimTekariScroll: the pause after the third sweep before the cycle restarts (tww default 0x32) (tww d_meter.cpp unless named; no WWHD read) |

## Batch H3: dMeter_menuHIO_c, the fields tww reads

### dMeter_menuHIO_c (d_meter_01.h)

Under the 2026-11-11 rule (WWHD writes only in the constructor 02597B00, tww reads give one role). Still open: `field_0x82` (read once in dMenu_Item_c::cursorSubMove as a cursor step value, no role told apart) and the 68 fields read nowhere in WWHD or tww.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x4 | `field_0x8` | `mItemScale` | WWHD writes it only in the constructor 02597B00; tww reads it: d_menu_collect.cpp dMenu_Collect_c::itemScale scales the collection screen's item panes by it, d_meter.cpp dMeter_moveItemMove sizes the moving item icon by it (no WWHD read) |
| 0x10 | `field_0x14` | `mStarterScale` | WWHD writes it only in the constructor 02597B00; tww reads it: d_minigame_starter.cpp anime2 scales the minigame start sign by its animation factor * it (no WWHD read) |
| 0x8C | `field_0x90` | `mMenuOpenDelay` | WWHD writes it only in the constructor 02597B00; tww reads it: d_menu_window.cpp dMs_Execute opens the item / collection sub-menu once its timer passes it (no WWHD read) |
| 0x8E | `field_0x92` | `mMenuOpenFrames` | WWHD writes it only in the constructor 02597B00; tww reads it: d_menu_collect.cpp _open2 / _close2 (and the other menu screens) slide and fade the menu in and out over it frames; d_meter.cpp dMeter_menuLRMove uses it * 2 as the L / R arrow cycle (no WWHD read) |
| 0x90 | `field_0x94` | `mTimerClockScale` | WWHD writes it only in the constructor 02597B00; tww reads it: d_timer.cpp dTimer_c::anime scales the timer's clock icon and the panes parented to it (clock background, numbers, timer background and shadow) by it (no WWHD read) |
| 0x96 | `field_0x9a` | `mTimerRupeePosX` | WWHD writes it only in the constructor 02597B00; tww reads it: d_timer.cpp dTimer_c::anime / closeAnime place the timer's rupee pane at its x + it (no WWHD read) |
| 0x98 | `field_0x9c` | `mTimerRupeePosY` | WWHD writes it only in the constructor 02597B00; tww reads it: d_timer.cpp dTimer_c::anime / closeAnime place the timer's rupee pane at y it (no WWHD read) |
| 0x9C | `field_0xa0` | `mTimerRupeeScale` | WWHD writes it only in the constructor 02597B00; tww reads it: d_timer.cpp dTimer_c::anime scales the timer's rupee pane and its shadow by it (no WWHD read) |
| 0xB2 | `field_0xb6` | `mGameOverDelay` | WWHD writes it only in the constructor 02597B00; tww reads it: d_gameover.cpp dGameover_c::_create copies it into the countdown that _execute runs down before going on (no WWHD read) |
| 0xC4 | `field_0xc8` | `mItemMoveFrames` | WWHD writes it only in the constructor 02597B00; tww reads it: d_meter.cpp dMeter_moveItemMove / dMeter_moveItemDraw: the item-get icon move lasts it frames (the move flag drops once dMeter_itemTimer reaches it) (no WWHD read) |
| 0xC5 | `field_0xc9` | `mItemMoveScaleMode` | WWHD writes it only in the constructor 02597B00; tww reads it: d_meter.cpp dMeter_moveItemMove switches on it to choose how the moving item icon is scaled (1: mItemScale, 2: shrinking from it) (no WWHD read) |

## Batch H4: dMeter_msg_HIO_c, the fields tww reads

### dMeter_msg_HIO_c (d_meter_01.h)

Under the 2026-11-11 rule (WWHD writes only in the constructor 02597E34, tww reads give one role). Still open: `field_0x74` (dMsg_subTextSizeSet: a size the choice box is compared with, no role told apart), `field_0x76` (dMsg_subTextClose / dMsg_Execute: a second close phase length, not told apart from mSelectCloseFrames), `field_0x81` (dMsg_subTextOpen / Close: a second fade target with no pane named), `field_0x7e` (two roles: the choice box margin in dMsg_subTextSizeSet and the arrow frames in d_scope.cpp dScp_arrowMove) and the 46 fields read nowhere in WWHD or tww.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x4 | `field_0x5` | `mTalkWindowWhite` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_screenDataSetTalk / dMsg_Execute give the talk window pane m049C this white colour (no WWHD read) |
| 0x8 | `field_0x9` | `mTalkWindowBlack` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_screenDataSetTalk / dMsg_Execute give the talk window pane m049C this black colour (no WWHD read) |
| 0x2B | `field_0x2c` | `mMsgArrowAlphaA` | WWHD writes it only in the constructor 02597E34; tww reads it: f_op_msg_mng.cpp fopMsgM_arrowAnime blends the message arrow's alpha between it and mMsgArrowAlphaB and back (no WWHD read) |
| 0x2F | `field_0x30` | `mMsgArrowAlphaB` | WWHD writes it only in the constructor 02597E34; tww reads it: f_op_msg_mng.cpp fopMsgM_arrowAnime blends the message arrow's alpha between mMsgArrowAlphaA and it and back (no WWHD read) |
| 0x3C | `field_0x40` | `mSelectCloseScale` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextClose scales the choice box by 1 + (it - 1) * the close progress (no WWHD read) |
| 0x40 | `field_0x44` | `mSelectOpenDelay` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextOpen / dMsg_selectProc: the choice box starts opening after it frames (no WWHD read) |
| 0x42 | `field_0x46` | `mSelectOpenFrames` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextOpen opens the choice box over it frames after mSelectOpenDelay (no WWHD read) |
| 0x44 | `field_0x48` | `mSelectCloseFrames` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextClose closes the choice box over it frames (no WWHD read) |
| 0x54 | `field_0x58` | `mDemoFontSize` | WWHD writes it only in the constructor 02597E34; tww reads it: the font size of the demo message (d_msg.cpp dMsg_screenDataSetDemo), the picture box (d_picture_box.cpp screenSet2) and the telescope text (d_scope.cpp dScp_ScreenDataSet) (no WWHD read) |
| 0x56 | `field_0x5a` | `mCharSpace` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_screenDataSetTalk / d_mesg.cpp setCommonData: the character spacing of the text panes ([0] and its shadow [2]) (no WWHD read) |
| 0x58 | `field_0x5c` | `mRubyCharSpace` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_screenDataSetTalk / d_mesg.cpp setCommonData and ruby_character: the character spacing of the ruby panes ([1] and its shadow [3]) (no WWHD read) |
| 0x5A | `field_0x5e` | `mLineSpace` | WWHD writes it only in the constructor 02597E34; tww reads it: the line spacing of the message text (d_mesg.cpp initialize / do_tag / do_systemTagCode, the menus' itemnoteSet) (no WWHD read) |
| 0x5C | `field_0x60` | `mRubyOffsetY` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_ScreenDataValueInitItem / InitTact / dMsg_mesgOutPos / dMsg_Execute place the ruby panes ([1], [3]) it above the text line (no WWHD read) |
| 0x5E | `field_0x62` | `mTextOffsetY` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_ScreenDataValueInitItem / InitTact / dMsg_mesgOutPos / dMsg_Execute place the text panes ([0], [2]) it above the line position (no WWHD read) |
| 0x62 | `field_0x66` | `mTalkWindowOffsetY` | WWHD writes it only in the constructor 02597E34; tww reads it: added to the y of the talk window pane, its text panes and the message arrow (d_msg.cpp dMsg_Execute / dMsg_arrowMove, d_mesg.cpp arwAnime) (no WWHD read) |
| 0x64 | `field_0x68` | `mRubyFontSize` | WWHD writes it only in the constructor 02597E34; tww reads it: the ruby font size (d_msg.cpp dMsg_screenDataSetItem / Demo, d_message.cpp dMsg2_screenDataInit, d_mesg.cpp ruby_character) (no WWHD read) |
| 0x66 | `field_0x6a` | `mOutFontOffsetY` | WWHD writes it only in the constructor 02597E34; tww reads it: f_op_msg_mng.cpp fopMsgM_outFontDraw draws the inline button icon and its shadow it lower (no WWHD read) |
| 0x68 | `field_0x6c` | `mSpaceTimer` | WWHD writes it only in the constructor 02597E34; tww reads it: the message space timer (setSpaceTimer in d_picture_box.cpp messageSet, d_scope.cpp dScp_talkBeforeProc; f_op_msg_mng.cpp stringSet) (no WWHD read) |
| 0x6A | `field_0x6e` | `mTalkWindowSplitY` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_mesgOutPos puts the talk window at the top or the bottom by whether the speaker's screen y is below it (no WWHD read) |
| 0x6C | `field_0x70` | `mFontSize` | WWHD writes it only in the constructor 02597E34; tww reads it: the message font size (f_op_msg_mng.cpp, d_msg.cpp, d_mesg.cpp, the menu screens' itemnoteSet, d_picture_box.cpp, d_scope.cpp) (no WWHD read) |
| 0x74 | `field_0x78` | `mSelectWidthAdd` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextSizeSet widens the choice box by it plus the choice text length (no WWHD read) |
| 0x76 | `field_0x7a` | `mSelectHeightAdd` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextSizeSet adds it to the choice box height (no WWHD read) |
| 0x78 | `field_0x7c` | `mSelectOffsetY` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextSizeSet places the choice box it from the talk window's edge (no WWHD read) |
| 0x7B | `field_0x7f` | `mCornerColorFrames` | WWHD writes it only in the constructor 02597E34; tww reads it: d_mesg.cpp cornerMove turns the corner colours of the message cursor one step every it frames (a 4-step cycle) (no WWHD read) |
| 0x7C | `field_0x80` | `mSelectWindowAlpha` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_subTextOpen / dMsg_subTextClose fade the talk window pane m049C to it / 255 while the choice box is open (no WWHD read) |
| 0x7E | `field_0x82` | `mSendSpeed` | WWHD writes it only in the constructor 02597E34; tww reads it: the text speed: setSendSpeed in d_msg.cpp dMsg_ScreenDataValueInit* / dMsg_continueProc and d_picture_box.cpp messageSet (no WWHD read) |
| 0x7F | `field_0x83` | `mSpaceFlag` | WWHD writes it only in the constructor 02597E34; tww reads it: d_msg.cpp dMsg_ScreenDataValueInit* / dMsg_continueProc and d_picture_box.cpp messageSet switch the message space flag on while it is not 0 (no WWHD read) |
| 0x83 | `field_0x87` | `mStickBlendFrames` | WWHD writes it only in the constructor 02597E34; tww reads it: f_op_msg_mng.cpp / d_2dnumber.cpp outFontStickAnime*: the frames of each blend between the two stick icon frames (no WWHD read) |
| 0x84 | `field_0x88` | `mStickStartDelay` | WWHD writes it only in the constructor 02597E34; tww reads it: f_op_msg_mng.cpp / d_2dnumber.cpp outFontStickAnime*: the frames before the stick icon animation starts (no WWHD read) |
| 0x85 | `field_0x89` | `mStickHoldFrames` | WWHD writes it only in the constructor 02597E34; tww reads it: f_op_msg_mng.cpp / d_2dnumber.cpp outFontStickAnime*: the frames the second stick icon frame is held (no WWHD read) |

## Batch H5: dMeter_message_HIO_c, the fields tww reads

### dMeter_message_HIO_c (d_meter_01.h)

Under the 2026-11-11 rule (WWHD writes only in the constructor 02598058, tww's d_message.cpp reads give one role). Still open: the 48 fields read nowhere in WWHD or tww.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x4 | `field_0x5` | `mTextColor` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the font colour of the text panes (no WWHD read) |
| 0x8 | `field_0x9` | `mTextWhite` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the white colour of the text panes (no WWHD read) |
| 0xC | `field_0xd` | `mTextBlack` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the black colour of the text panes (no WWHD read) |
| 0x10 | `field_0x11` | `mShadowColor` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the font colour of the text and ruby shadow panes (no WWHD read) |
| 0x14 | `field_0x15` | `mShadowWhite` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the white colour of the text and ruby shadow panes (no WWHD read) |
| 0x18 | `field_0x19` | `mShadowBlack` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the black colour of the text and ruby shadow panes (no WWHD read) |
| 0x1C | `field_0x1d` | `mRubyColor` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the font colour of the ruby panes (no WWHD read) |
| 0x20 | `field_0x21` | `mRubyWhite` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the white colour of the ruby panes (no WWHD read) |
| 0x24 | `field_0x25` | `mRubyBlack` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute: the black colour of the ruby panes (no WWHD read) |
| 0x28 | `field_0x29` | `mAimBrightness` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_aimBrightness returns 0xFF up to it and fades above it (no WWHD read) |
| 0x31 | `field_0x32` | `mMsg2FontSize` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_screenDataInit: the font size (no WWHD read) |
| 0x36 | `field_0x38` | `mMsg2RubyOffsetY` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute places the ruby panes and their shadows it above the text; dMsg2_setCharAlpha uses the same offset (no WWHD read) |
| 0x39 | `field_0x3b` | `mPopSpeed` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Create copies it into dMsg2_popSpeed (no WWHD read) |
| 0x3A | `field_0x3c` | `mLineHeightAdd` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_ScreenDataValueInit: the line height is the text pane's height + it (no WWHD read) |
| 0x47 | `field_0x4b` | `mShowRuby` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_rubySet hides the ruby panes while it is 0 (no WWHD read) |
| 0x54 | `field_0x58` | `mTextOffsetX` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_Execute places the text, ruby and shadow panes at the text pane's x + it (no WWHD read) |
| 0x57 | `field_0x5b` | `mMultiTexAlpha` | WWHD writes it only in the constructor 02598058; tww reads it: d_message.cpp dMsg2_multiTexDraw scales the overlay alpha by it / 255 (no WWHD read) |

## Batch H6: dMeter_map_HIO_c, the free-icon alpha and scale

### dMeter_map_HIO_c (d_meter_01.h)

Checked again under the batch H2 rule: kept open in H1 because only tww reads them; WWHD writes them only in the constructor 02598254.

| WWHD offset | Old (GameCube name) | New | Evidence |
|---|---|---|---|
| 0x02 | `field_0x6` | `mIconFreeAlpha` | WWHD writes it only in the constructor 02598254 (200); tww reads it: mapCtrlDisp_c::moveMapCtrlDisp sets the free icon's alpha to the fade value * it / 255 (dMap_c::setIconFreeAlpha) (no WWHD read) |
| 0x0C | `field_0x10` | `mIconFreeScale` | WWHD writes it only in the constructor 02598254 (l_mapIconScale); tww reads it: mapCtrlDisp_c::moveMapCtrlDisp sets the free icon's scale to 1.0 / 1.1 * it (dMap_c::setIconFreeScale; tww default 0.8) (no WWHD read) |

## Type fix: dMeterPrefixHD::mHeartAlphaStep (own commit)

The view dMeterPrefixHD (d_meter_00.h) declared 0x3018 (`mHeartAlphaStep`, M4) as be<u16>; the class declares it
be<s16> and the heart alpha step inlined into dMeter_Execute 02593B10 compares it as a signed count. The view now
uses be<s16> (objcmp identical, decl_lint ok).

## Open (no use beyond the constructor or not yet reviewed)

- dMeter_menuHIO_c, dMeter_msg_HIO_c, dMeter_message_HIO_c: every offset-named field is only written by its constructor
  (02597B00 / 02597E34 / 02598058) in WWHD; the ones tww reads with one role are named in batches H3 / H4 / H5, which list
  the rest.
- dMeter_HIO_c: the fields WWHD reads are named (M1, M3), the fields only tww reads in batch H2; the rest are listed there.
- sub_meter_class panes with no role told apart yet: `field_0x1210` ('cmcv'; TWW only fades and hides it),
  `field_0x12b8` ('cmrg'; no use beyond the set-up), `field_0x25c0[2]` / `field_0x2630[2]` ('b001'/'b002',
  'ba01'/'ba02'; faded and moved exactly like mXYButton). The clock panes on sMainParts3 are named in batch K1.
- `field_0x3019`: only cleared (dMeter_magicInit in WWHD and tww). `field_0x2f70`: only set to 1.0 (dMeter_heartInit
  02591B4C, tww the same), never read.
- `field_0x26d8` ('ba0r'): faded and moved with mRButton; its mUserArea is set and cleared but never read (WWHD
  dMeter_rAlpha, TWW dMeter_rInit / rMove), so no role apart from mRButton.
- `field_0x3027`: TWW dMeter_mapInit sets 0xFF, dMeter_menuPlusMove / dMeter_mapMove clear it, dMeter_mapMove and
  dMeter_compassMove test it; no single role (stage-start marker and map step at once).
- Never used in WWHD or tww: `field_0x1328`, `field_0x1360`, `field_0x1d70`, `field_0x1e88`, `field_0x1ec0`,
  `field_0x1ef8`, `field_0x21d0`..`field_0x22e8`, `field_0x2710`, `field_0x2d68`, `field_0x2f54[3]`.

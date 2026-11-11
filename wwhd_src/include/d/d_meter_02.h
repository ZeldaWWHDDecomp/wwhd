// GameCube meter names, with HD layouts for the trailing HUD range.
#pragma once
#include "bindings.h"
#include "f_op/f_op_msg_mng.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
namespace meter_detail {
enum dMeterStatus_e {
    dMtrStts_UNK1_e = 0x00000001,
    dMtrStts_UNK2_e = 0x00000002,
    dMtrStts_UNK4_e = 0x00000004,
    dMtrStts_UNK8_e = 0x00000008,
    dMtrStts_UNK10_e = 0x00000010,
    dMtrStts_UNK20_e = 0x00000020,
    dMtrStts_UNK40_e = 0x00000040,
    dMtrStts_UNK80_e = 0x00000080,
    dMtrStts_UNK100_e = 0x00000100,
    dMtrStts_UNK200_e = 0x00000200,
    dMtrStts_UNK400_e = 0x00000400,
    dMtrStts_UNK800_e = 0x00000800,
    dMtrStts_UNK1000_e = 0x00001000,
    dMtrStts_UNK2000_e = 0x00002000,
    dMtrStts_UNK4000_e = 0x00004000,
    dMtrStts_UNK8000_e = 0x00008000,
    dMtrStts_UNK10000_e = 0x00010000,
    dMtrStts_UNK20000_e = 0x00020000,
    dMtrStts_UNK40000_e = 0x00040000,
    dMtrStts_UNK80000_e = 0x00080000,
    dMtrStts_UNK100000_e = 0x00100000,
    dMtrStts_UNK200000_e = 0x00200000,
    dMtrStts_UNK400000_e = 0x00400000,
    dMtrStts_UNK800000_e = 0x00800000,
    dMtrStts_HD1000000_e = 0x01000000, // HD: additional HUD status bit.
    dMtrStts_HD2000000_e = 0x02000000, // HD: additional HUD status bit.
};
// HD: process numbers differ from GameCube; names follow tww.
enum ActorName {
    fpcNm_BTD_e = 0xEA,
    fpcNm_BMD_e = 0xEB,
    fpcNm_BDK_e = 0xEE,
    fpcNm_BPW_e = 0xD3,
    fpcNm_BGN_e = 0xF3,
    fpcNm_BGN2_e = 0xF4,
    fpcNm_BGN3_e = 0xF5,
    fpcNm_GND_e = 0xF6,
    fpcNm_BST_e = 0xF0,
    fpcNm_BWD_e = 0xD9,
    fpcNm_NPC_CB1_e = 0x14E,
    fpcNm_NPC_MD_e = 0x16F,
};
enum ItemName {
    dItemNo_BOW_e = 0x27,
    dItemNo_MAGIC_ARROW_e = 0x35,
    dItemNo_LIGHT_ARROW_e = 0x36,
    dItemNo_BOMB_BAG_e = 0x31,
    dItemNo_FOREST_WATER_e = 0x59,
    dItemNo_BIRD_BAIT_5_e = 0x82,
    dItemNo_WIND_WAKER_e = 0x22,
};
inline u32 meterPictureRecord(u32 save) {
    return save + 0x12C0;
}
struct StageData;
struct StageDataVtable {
    u8 beforeStag[0x15C];
    gabi::gfn mGetStag;
    u32 getStag(StageData *stage) {
        return gabi::call_ptr<u32>(mGetStag.get(), stage);
    }
};
struct StageData {
    gptr<StageDataVtable> mVtable;
};
struct SafeStringView;
struct SafeStringVtable {
    u8 beforeCleanup[0x14];
    gabi::gfn mCleanup;
    void cleanup(SafeStringView *string) {
        gabi::call_ptr<void>(mCleanup.get(), string);
    }
};
struct PlayerVtable {
    u8 beforeTactMusic[0xAC];
    gabi::gfn mGetTactMusic;
    s32 getTactMusic(fopAc_ac_c *actor) {
        return gabi::call_ptr<s32>(mGetTactMusic.get(), actor);
    }
};

// HD: message base grows by four bytes. All numeric field names below are tww names.
struct sub_meter_class {
    u8 before_mHeart[0x644];
    fopMsgM_pane_class mHeart[20];
    fopMsgM_pane_class mHeartShadow[20];
    fopMsgM_pane_class mHeartLight;  /* was field_0x0f00: pane 'htfl' (dMeter_screenDataSet in dMeter_Create 02595C6C): the light on the current heart; dMeter_heartInit 02591B4C copies the current heart's position and size into it and clears its mUserArea (frame counter, also cleared by the life change in dMeter_Execute 02593B10); dMeter_gaugeAlpha 0259C2BC scales its alpha by mHeartAlpha; TWW d_meter.cpp dMeter_heartLightMove pulses its alpha and size */
    fopMsgM_pane_class mMagicBar[8];  /* was field_0x0f38: panes 'mbr1'..'mbr8' (dMeter_screenDataSet in dMeter_Create 02595C6C): the magic fill segments; dMeter_magicChange 02592A88 shows, shortens or hides each by the shown magic (mDispMagic / 100), dMeter_gaugeAlpha 0259C2BC fades them with mMagicAlpha; their mUserArea fields are dMeter_magicGaugeMove 0259B698 / dMeter_flyGaugeMove 02592EE8 state ([0] fly colours set, [2] capacity reached, [3]/[4] pulse timer/flag, [5] redraw) */
    fopMsgM_pane_class mMagicGauge3;  /* was field_0x10f8: pane 'mbb3' (dMeter_screenDataSet in dMeter_Create 02595C6C): dMeter_magicLength 02592888 sets its width from the shown capacity (mDispMaxMagic), mMagicGauge1/2 follow that width; its mUserArea is the fade counter behind mMagicAlpha (dMeter_magicGaugeMove 0259B698); moved with the gauge (dMeter_magicTransNowInit 0259295C, dMeter_magicTransScale 02592CD8) and faded by dMeter_gaugeAlpha 0259C2BC */
    fopMsgM_pane_class mMagicGauge2;  /* was field_0x1130: pane 'mbb2' (dMeter_screenDataSet in dMeter_Create 02595C6C): dMeter_magicLength 02592888 sizes it to mMagicGauge3's width plus its own margin; dMeter_magicTransScale 02592CD8 scales its position and size (a child of mMagicGauge1); its mUserArea is dMeter_flyGaugeMove 02592EE8's 50-frame move-to-the-flyer timer; TWW dMeter_magicColor colours it with mMagicGauge1 */
    fopMsgM_pane_class mMagicGaugeShadow2;  /* was field_0x1168: pane 'mbk2' (dMeter_screenDataSet in dMeter_Create 02595C6C): dMeter_magicTransScale 02592CD8 scales its position and size like mMagicGauge2 (a child of mMagicGauge1); the shadow of 'mbb2' by the tag pattern of mHeart/mHeartShadow ('ht'/'hk') */
    fopMsgM_pane_class mMagicGauge1;  /* was field_0x11a0: pane 'mbb1' (dMeter_screenDataSet in dMeter_Create 02595C6C): dMeter_magicLength 02592888 sizes it to mMagicGauge3's width plus its own margin; its centre is the anchor dMeter_magicTransScale 02592CD8 scales the other gauge panes about; its mUserArea is dMeter_flyGaugeMove 02592EE8's fade counter; moved with the gauge (dMeter_magicTransNowInit 0259295C) and faded by dMeter_gaugeAlpha 0259C2BC */
    fopMsgM_pane_class mMagicGaugeShadow1;  /* was field_0x11d8: pane 'mbk1' (dMeter_screenDataSet in dMeter_Create 02595C6C): moves, scales and fades with mMagicGauge1 (dMeter_magicTransNowInit 0259295C, dMeter_magicInitTrans 02592BC4, dMeter_magicTransScale 02592CD8, dMeter_gaugeAlpha 0259C2BC); dMeter_magicGaugeMove 0259B698 clears its mUserArea; the 'k' tags are the shadow panes as for mHeart/mHeartShadow ('ht'/'hk') */
    fopMsgM_pane_class field_0x1210;
    fopMsgM_pane_class mCompassNeedle;  /* was field_0x1248: pane 'cmmg' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the compass needle; WWHD only sets it up there (the HD compass draws elsewhere); TWW dMeter_compassInit sets its MyPicture m124..m134 values and dMeter_compassMove turns it about mCompassBase by the ship's shape angle (dMeter_compassRotate) */
    fopMsgM_pane_class mCompassNeedleShadow;  /* was field_0x1280: pane 'cmmk' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the shadow of mCompassNeedle (the 'k' tags are the shadow panes, as 'ht'/'hk' for mHeart/mHeartShadow); WWHD only sets it up there; TWW dMeter_compassInit / compassMove treat it exactly like mCompassNeedle */
    fopMsgM_pane_class field_0x12b8;
    fopMsgM_pane_class mCompassBase;  /* was field_0x12f0: pane 'cmb1' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the compass base: 02593434 (d_meter_00.cpp dMeter_gaugeInitialPositionHD; the same code as TWW dMeter_compassValueInit) clears its mUserArea and moves it left by mCompassClock's original left + width together with mCompassClock; drawn by dMeter_Draw 02593630; TWW dMeter_compassMove turns the needles about it and dMeter_compassDirOpen / WindOpen / WindClose count its mUserArea as their open and close frame */
    fopMsgM_pane_class field_0x1328;
    fopMsgM_pane_class field_0x1360;
    fopMsgM_pane_class mClockBar;  /* was field_0x1398: pane 'bar' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise, dMeter_compassAlpha fades it with the clock (tww writes it, the same code) */
    fopMsgM_pane_class mClockRing;  /* was field_0x13d0: pane 'rng2' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise; TWW dMeter_compassDirOpen / WindOpen / DirClose / WindClose / ValueInit turn mCompassClock and mClockCenter about a point offset by half its width; the ring of the running clock, 'rng1' mFixedClockRing is the fixed one (tww writes it, the same code) */
    fopMsgM_pane_class mClockHorizon1;  /* was field_0x1408: pane 'hrz1' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise, dMeter_compassAlpha fades it (tww writes it, the same code) */
    fopMsgM_pane_class mClockHorizon2;  /* was field_0x1440: pane 'hrz2' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise, dMeter_compassAlpha fades it (tww writes it, the same code) */
    fopMsgM_pane_class mClockStar[3];  /* was field_0x1478: panes 'str5'..'str7' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise; TWW dMeter_clockInit sets each mUserArea to 1 and dMeter_clockMove twinkles them with dMeter_clockStarLight while the time passes (mFixedClockStar while it does not) (tww writes them, the same code) */
    fopMsgM_pane_class mClockMoon;  /* was field_0x1520: pane 'mon2' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise ('mon1' mFixedClockMoon is the fixed clock's moon) (tww writes it, the same code) */
    fopMsgM_pane_class mClockSun;  /* was field_0x1558: pane 'sun2' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise ('sun1' mFixedClockSun is the fixed clock's sun) (tww writes it, the same code) */
    fopMsgM_pane_class mClockCircle[4];  /* was field_0x1590: panes 'cir1'..'cir4' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise, dMeter_compassAlpha fades them (tww writes them, the same code) */
    fopMsgM_pane_class mClockTimeDial;  /* was field_0x1670: pane 'cnt2' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise; TWW dMeter_compassGetOnProc turns it about its own centre by -(time of day) - 90 degrees (dMeter_compassRotate), so it shows the time (tww writes it, the same code) */
    fopMsgM_pane_class mClockCenter;  /* was field_0x16a8: pane 'cnt1' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit show it while the room's time passes (roomControl_getTimePass) and hide it otherwise; TWW dMeter_screenDataSet clears its white colour, dMeter_compassDirOpen / WindOpen / DirClose / WindClose / ValueInit slide it with mCompassBase and mCompassClock and turn it by 180 degrees with mCompassClock (tww writes it, the same code) */
    fopMsgM_pane_class mFixedClockRing;  /* was field_0x16e0: pane 'rng1' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit hide it while the room's time passes and show it otherwise, by day and by night: the ring of the fixed clock ('rng2' mClockRing is the running one) (tww writes it, the same code) */
    fopMsgM_pane_class mFixedClockStar[4];  /* was field_0x1718: panes 'str1'..'str4' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit hide it while the room's time passes and show it otherwise at night only; TWW dMeter_clockInit sets each mUserArea to 1 and dMeter_clockMove twinkles them with dMeter_clockStarLight while the time does not pass (tww writes them, the same code) */
    fopMsgM_pane_class mFixedClockMoon;  /* was field_0x17f8: pane 'mon1' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit hide it while the room's time passes and show it otherwise at night only (between 270 and 90 degrees of the day) (tww writes it, the same code) */
    fopMsgM_pane_class mFixedClockCloudClip;  /* was field_0x1830: pane 'clp' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit hide it while the room's time passes and show it otherwise by day only; TWW dMeter_screenDataSet clears its white colour, dMeter_clockInit and dMeter_clockMultiMove take mCompassClock's origin + its origin and size as the rectangle the three cloud layers are drawn into (tww writes it, the same code) */
    fopMsgM_pane_class mFixedClockCloud[3];  /* was field_0x1868: panes 'cld1'..'cld3' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW hides them always and uses mPosCenter as data: dMeter_clockInit sets it from mCompassClock + mFixedClockCloudClip's origin, dMeter_clockMove from HIO 0x6C / 0x72, and dMeter_clockMultiMove passes it as the scroll of cloud layer clock[i] (tww writes them, the same code) */
    fopMsgM_pane_class mFixedClockSun;  /* was field_0x1910: pane 'sun1' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; no other WWHD use; TWW dMeter_clockShow / dMeter_compassValueInit hide it while the room's time passes and show it otherwise by day only; its mUserArea is a 0..120 frame cycle (TWW dMeter_clockInit clears it, dMeter_clockMove steps it) (tww writes it, the same code) */
    fopMsgM_pane_class mCompassClock;  /* was field_0x1948: pane 'cmtm' on sMainParts3 (the clock screen; dMeter_screenDataSet in dMeter_Create 02595C6C): 02593434 (d_meter_00.cpp dMeter_gaugeInitialPositionHD; the same code as TWW dMeter_compassValueInit) moves it and mCompassBase left by its own original left + width (mPosTopLeftOrig.x + mSizeOrig.x); drawn by dMeter_Draw 02593630; TWW dMeter_compassValueInit also turns it by 180 degrees, dMeter_clockShow gives it the day or night corner colours and its mUserArea is dMeter_compassGetOnProc's wind-open request */
    fopMsgM_pane_class mKeyLight1;  /* was field_0x1980: pane 'kyl1' (dMeter_screenDataSet, inlined into dMeter_Create 02595C6C); dMeter_keyMove 0259C7FC runs dMeter_keyLight on it with the fixed frames {30,40,45,50,58} (its mUserArea is the frame counter); dMeter_Create sets the counter to 1; same code TWW d_meter.cpp dMeter_keyMove/dMeter_keyInit */
    fopMsgM_pane_class mKeyLight2;  /* was field_0x19b8: pane 'kyl2' (dMeter_screenDataSet in dMeter_Create 02595C6C); dMeter_keyMove 0259C7FC runs dMeter_keyLight on it with mKeyLight2Frames and re-rolls those frames when its counter is 1; dMeter_Create sets the counter to 1 */
    fopMsgM_pane_class mRupyDigitA[4];  /* was field_0x19f0: panes 'nm00'..'nm03' on sMainParts2 (dMeter_screenDataSet in dMeter_Create 02595C6C): the rupee count digits (TWW dMeter_rupyInit / dMeter_walletChange give them the same digit textures as mRupyDigitB); dMeter_rupyMove 0259C454 fades them; [0].mUserArea is its dMeter_alphaOpen/alphaClose second counter (dMeter_Create sets 1), [1].mUserArea the shown wallet size (dMeter_rupyMove calls dMeter_walletChange 025931CC when the saved mWalletSize differs); dMeter_walletChange moves mRupyIcon by the [1]-[0] x distance */
    fopMsgM_pane_class mKeyDigitA[2];  /* was field_0x1ad0: panes 'nm03'/'nm04' (dMeter_screenDataSet in dMeter_Create 02595C6C): the tens/ones digits of the key count (TWW dMeter_keyInit/dMeter_keyMove blendInit/blendDraw them with the same digit as mKeyDigitB); dMeter_keyMove 0259C7FC sets their alpha, [0].mUserArea is the second dMeter_alphaOpen/alphaClose counter */
    fopMsgM_pane_class mRupyDigitB[4];  /* was field_0x1b40: panes 'nm04'..'nm07' on sMainParts2 (dMeter_screenDataSet in dMeter_Create 02595C6C): the second copy of the rupee count digits (same digit textures as mRupyDigitA in TWW dMeter_rupyInit / dMeter_walletChange); dMeter_rupyMove 0259C454 fades them with mRupyDigitA */
    fopMsgM_pane_class mKeyDigitB[2];  /* was field_0x1c20: panes 'nm08'/'nm09' (dMeter_screenDataSet in dMeter_Create 02595C6C): the second copy of the key count digits (same digit as mKeyDigitA in TWW dMeter_keyInit/dMeter_keyMove); dMeter_keyMove 0259C7FC sets their alpha */
    fopMsgM_pane_class mKeyIcon;  /* was field_0x1c90: pane 'key0' (dMeter_screenDataSet in dMeter_Create 02595C6C); dMeter_keyMove 0259C7FC fades it with the key digits (its mUserArea is the dMeter_alphaOpen/alphaClose counter); dMeter_Create clears it */
    fopMsgM_pane_class mRActionIcon;  /* was field_0x1cc8: pane 'basv' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_rInit (in dMeter_Create 02595C6C) clears its mUserArea; dMeter_rAlpha 0259A5B0 sets its mUserArea to 1 when the R status differs from mRStatusShown, then clears it and stores the status there, and fades it by the R pane counter (hidden while there is no R status); TWW dMeter_rMove changes its texture to the R action icon on that request (tww the same) */
    fopMsgM_pane_class mWeaponText;  /* was field_0x1d00: pane 'bawd' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the action word of the weapon (AStatus) button: dMeter_weponMove 025986DC gives it the button alpha when the AStatus is neither blank nor a weapon (sword, boko stick, hatchet, iron club, longsword, spear) and clears its alpha otherwise; its mInitAlpha is the full alpha mActionText blinks down from; its mUserArea is cleared when mLastAStatus was BID; TWW dMeter_weponChange gives it the action texture actionTex[0], dMeter_weponAlpha / weponTrans fade and move it the same way */
    fopMsgM_pane_class mActionText;  /* was field_0x1d38: pane 'bawp' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the action word of the action (DoStatus) button: meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10 gives it the button alpha for a DoStatus that is not blank, parry, ba_sake / ba_nageru / ba_motu or bow, and clears it otherwise; dMeter_weponMove 025986DC blinks its mNowAlpha down from mWeaponText's mInitAlpha while the DoStatus is BID (its mUserArea is the 20-frame blink counter); TWW dMeter_actionChange gives it the action texture actionTex[1] */
    fopMsgM_pane_class field_0x1d70;
    fopMsgM_pane_class mActionButton[2];  /* was field_0x1da8: panes 'baa' ([0]) and 'baa2' ([1]) on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the action (DoStatus) button ('ba' + button letter, as 'bab' mWeaponButton): meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10 fades [0] for a blank, bow or other action and [1] for parry and ba_sake / ba_nageru / ba_motu, together with mActionButton2; drawn by dMeter_Draw 02593630; same in TWW dMeter_actionAlpha / actionTrans */
    fopMsgM_pane_class mActionButton2[2];  /* was field_0x1e18: panes 'bawe' ([0]) and 'bat2' ([1]) on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the second action button pane pair, faded like mActionButton ([0] for a blank, bow or other action, [1] for parry and ba_sake / ba_nageru / ba_motu; meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10); [1].mUserArea is the 20-frame blink counter dMeter_weponMove 025986DC steps for ba_sake / ba_nageru / ba_motu (sound 0x8F8 / 0x90A / 0x852 at each wrap); [0].mUserArea is set to 1 by meterUpdateActionChange (dMeter_Execute 02593B10) when the DoStatus becomes blank or another action and cleared by dMeter_actionInit (dMeter_Create); TWW dMeter_weponMove blinks [1]'s black colour */
    fopMsgM_pane_class field_0x1e88;
    fopMsgM_pane_class field_0x1ec0;
    fopMsgM_pane_class field_0x1ef8;
    fopMsgM_pane_class mItemDigit10[3];  /* was field_0x1f30: panes 'x10' / 'y10' / 'z10' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the tens digit of each item button's count; drawn by dMeter_Draw 02593630; TWW dMeter_xyItemNumberSet sets it to the arrow, bomb or beast count / 10 (hidden below 10) and dMeter_xyAlpha fades it with the button */
    fopMsgM_pane_class mItemDigit1[3];  /* was field_0x1fd8: panes 'x01' / 'y01' / 'z01' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the ones digit of each item button's count; drawn by dMeter_Draw 02593630; TWW dMeter_xyItemNumberSet sets it to the arrow, bomb or beast count % 10, or to the whole picture or bait count and dMeter_xyAlpha fades it with the button */
    fopMsgM_pane_class mItemIcon[3];  /* was field_0x2080: panes 'xitm' / 'yitm' / 'zitm' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the item icon of each item button: dMeter_xyMove 02599584 puts the forest water particles at its mPosCenter and gives them its mNowAlpha while that item is set; drawn by dMeter_Draw 02593630; TWW dMeter_xyItemChange loads xyIconTex into it */
    fopMsgM_pane_class mItemIconShadow[3];  /* was field_0x2128: panes 'xitk' / 'yitk' / 'zitk' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the shadow of mItemIcon (the 'k' tags are the shadow panes, as 'ht'/'hk' for mHeart/mHeartShadow); drawn by dMeter_Draw 02593630; TWW dMeter_xyItemChange gives it the same xyIconTex texture and shows / hides it with the icon */
    fopMsgM_pane_class field_0x21d0;
    fopMsgM_pane_class field_0x2208;
    fopMsgM_pane_class field_0x2240;
    fopMsgM_pane_class field_0x2278;
    fopMsgM_pane_class field_0x22b0;
    fopMsgM_pane_class field_0x22e8;
    fopMsgM_pane_class mBowLight1[3];  /* was field_0x2320: panes 'blx1' / 'bly1' / 'blz1' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the first bow light pane of each item button; dMeter_xyMove 02599584 pulses the three bow light panes of a button whose item is the magic or light arrow: the frame counter in mBowLight1[0].mUserArea (the X entry, as in TWW) counts to 120, alpha falls from mBowLight1[0].mInitAlpha toward l_bowAlphaMin and the scale toward 1 - l_bowPulseDepth; TWW dMeter_xyBowLightAnime; dMeter_xyMove clears [i].mUserArea when button i changes to an item that is not the magic or light arrow; drawn by dMeter_Draw 02593630 */
    fopMsgM_pane_class mBowLight2[3];  /* was field_0x23c8: panes 'blx2' / 'bly2' / 'blz2' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the second bow light pane of each item button; dMeter_xyMove 02599584 pulses the three bow light panes of a button whose item is the magic or light arrow: the frame counter in mBowLight1[0].mUserArea (the X entry, as in TWW) counts to 120, alpha falls from mBowLight1[0].mInitAlpha toward l_bowAlphaMin and the scale toward 1 - l_bowPulseDepth; TWW dMeter_xyBowLightAnime; drawn by dMeter_Draw 02593630 */
    fopMsgM_pane_class mBowLight3[3];  /* was field_0x2470: panes 'blx3' / 'bly3' / 'blz3' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the third bow light pane of each item button; dMeter_xyMove 02599584 pulses the three bow light panes of a button whose item is the magic or light arrow: the frame counter in mBowLight1[0].mUserArea (the X entry, as in TWW) counts to 120, alpha falls from mBowLight1[0].mInitAlpha toward l_bowAlphaMin and the scale toward 1 - l_bowPulseDepth; TWW dMeter_xyBowLightAnime; drawn by dMeter_Draw 02593630 */
    fopMsgM_pane_class mZButton;  /* was field_0x2518: pane 'baz' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the Z item button ('ba' + button letter, as 'bab' mWeaponButton and 'baa' mActionButton); drawn by dMeter_Draw 02593630; TWW dMeter_xyAlpha fades it with the Z item panes */
    fopMsgM_pane_class mXYButton[2];  /* was field_0x2550: panes 'bax1' ([0]) and 'bay' ([1]) on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the X and Y item buttons ('ba' + button letter, as 'baz' mZButton); dMeter_xyInit (dMeter_Create) clears [0].mUserArea; drawn by dMeter_Draw 02593630; TWW dMeter_xyAlpha fades [0] with the X and [1] with the Y item panes */
    fopMsgM_pane_class field_0x25c0[2];
    fopMsgM_pane_class field_0x2630[2];
    fopMsgM_pane_class mRButton;  /* was field_0x26a0: pane 'bar1' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630: the R button ('ba' + button letter, as 'baz' mZButton); dMeter_rAlpha 0259A5B0 fades it with mRActionIcon; TWW dMeter_rTrans moves it (tww the same) */
    fopMsgM_pane_class field_0x26d8;
    fopMsgM_pane_class field_0x2710;
    fopMsgM_pane_class mArrowDigit10;  /* was field_0x2748: pane 'a10' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the tens digit of the action button's arrow count ('a10'/'a01' as 'x10'/'x01' of the item counts): meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10 shows it only for DoStatus bow_01; drawn by dMeter_Draw 02593630; TWW dMeter_actionAlpha sets it to the So mini-game's rest arrows / 10 (hidden below 10) */
    fopMsgM_pane_class mArrowDigit1;  /* was field_0x2780: pane 'a01' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the ones digit of the action button's arrow count: meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10 shows it only for DoStatus bow_01; drawn by dMeter_Draw 02593630; TWW dMeter_actionAlpha sets it to the So mini-game's rest arrows % 10 */
    fopMsgM_pane_class mActionBowIcon;  /* was field_0x27b8: pane 'yum' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the bow icon of the action button: meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10 shows it only for DoStatus bow_01; its mUserArea is the action-change flag (set when the DoStatus differs from mLastDoStatus, cleared by meterUpdateActionChange in dMeter_Execute 02593B10; dMeter_actionInit in dMeter_Create clears it); TWW dMeter_actionChange loads the bow texture actionTex[1] into it */
    fopMsgM_pane_class mActionBowIconShadow;  /* was field_0x27f0: pane 'yumk' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the shadow of mActionBowIcon (the 'k' tags are the shadow panes, as 'ht'/'hk' for mHeart/mHeartShadow): meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10 fades it with mActionBowIcon; TWW dMeter_actionChange gives it the same bow texture */
    fopMsgM_pane_class mWeaponIcon;  /* was field_0x2828: pane 'weit' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the weapon icon of the weapon (AStatus) button: dMeter_weponMove 025986DC gives it the button alpha for a weapon AStatus (sword, boko stick, hatchet, iron club, longsword, spear) and clears its alpha otherwise; its mUserArea is the texture-change flag (set when the AStatus differs from mLastAStatus, cleared by the next dMeter_weponMove; dMeter_weponInit in dMeter_Create clears it); TWW dMeter_weponChange loads the weapon texture actionTex[0] into it and dMeter_weponAlpha hides the button word and icons while the flag is set */
    fopMsgM_pane_class mWeaponIconShadow;  /* was field_0x2860: pane 'witk' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the shadow of mWeaponIcon (the 'k' tags are the shadow panes, as 'ht'/'hk' for mHeart/mHeartShadow): dMeter_weponMove 025986DC fades it with mWeaponIcon; TWW dMeter_weponChange gives it the same texture and dMeter_Create moves it as a child of mWeaponIcon */
    fopMsgM_pane_class mWeaponButton;  /* was field_0x2898: pane 'bab' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the weapon (AStatus) button ('ba' + button letter, as 'baa' mActionButton, 'bax1'/'bay'/'baz' and 'bar1' of the item and R buttons): dMeter_weponMove 025986DC gives it the button alpha for every AStatus; drawn by dMeter_Draw 02593630; TWW dMeter_weponAnime / weponTrans move it with the weapon panes */
    fopMsgM_pane_class mWeaponButton2;  /* was field_0x28d0: pane 'baat' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): the second weapon button pane, faded with mWeaponButton for every AStatus by dMeter_weponMove 025986DC; its mUserArea is the button bounce frame: dMeter_weponMove sets it to 1 when the AStatus changes and steps it to 12, then 0; dMeter_weponInit in dMeter_Create clears it; TWW dMeter_weponAnime moves the weapon panes by that frame */
    fopMsgM_pane_class mButtonCenter;  /* was field_0x2908: pane 'cent' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C): faded by both button groups (dMeter_weponMove 025986DC for a non-blank AStatus, meterUpdateActionPanes, the action step inlined into dMeter_Execute 02593B10 for every DoStatus) and drawn by dMeter_Draw 02593630; TWW dMeter_Create makes it the parent pane (dMeter_parentPaneTrans) that the action, weapon, item and R button panes are moved with */
    fopMsgM_pane_class mArrowTypeIcon[2];  /* was field_0x2940: panes 'arw1' / 'arw2' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C); dMeter_arwInit (in dMeter_Create 02595C6C) sets their alpha to zero; TWW dMeter_arwInit / dMeter_arwMove load the old ([0]) and the new ([1]) arrow type's icon texture into them when the kept arrow type changes while aiming the bow (tww writes it, the same code) */
    fopMsgM_pane_class mArrowTypeSlot[2];  /* was field_0x29b0: panes 'ip00' / 'ip02' on sMainParts1 (dMeter_screenDataSet in dMeter_Create 02595C6C); dMeter_arwInit (in dMeter_Create 02595C6C) sets their alpha to zero; TWW dMeter_arwMove slides between their centres on an arrow type change ([0].mUserArea the slide frame, [1].mUserArea the change count up to 2) and puts the arrow particles at [0] (tww writes it, the same code) */
    fopMsgM_pane_class mRupyIcon;  /* was field_0x2a20: pane 'rup1' on sMainParts2 (dMeter_screenDataSet in dMeter_Create 02595C6C): the rupee icon; dMeter_rupyMove 0259C454 fades it (its mUserArea is the dMeter_alphaOpen/alphaClose counter, dMeter_Create clears it), dMeter_walletChange 025931CC moves it with mRupyIconB; TWW dMeter_walletChange sets its wallet colours */
    fopMsgM_pane_class mRupyIconB;  /* was field_0x2a58: pane 'rup2' on sMainParts2 (dMeter_screenDataSet in dMeter_Create 02595C6C): the second rupee icon pane; dMeter_rupyMove 0259C454 fades it and dMeter_walletChange 025931CC moves it together with mRupyIcon (same in TWW dMeter_rupyMove / dMeter_walletChange) */
    fopMsgM_pane_class mSwimTimerLight[9];  /* was field_0x2a90: panes 'hk11'..'hk33' of the swim timer screen sScrTimer2 (TWW dMeter_screenDataTimeSet; 'hk' = hikari, light), drawn by dMeter_Draw 02593630 while sScrTimer2 exists; TWW dMeter_swimLightBirth starts one at the top of mSwimTimerGauge (mUserArea 1), dMeter_swimLightAnime rises it by mSwimLightRiseSpeed and sways it by mSwimLightSwayX (tww writes it, the same code) */
    fopMsgM_pane_class mSwimTimerFrame;  /* was field_0x2c88: pane 'wk00' of the swim timer screen sScrTimer2 (TWW dMeter_screenDataTimeSet; 'wk' = waku, frame), drawn by dMeter_Draw 02593630; TWW dMeter_swimInit saves its colours in mSwimFrameBlack / mSwimFrameWhite, dMeter_swimMainBlink blinks them, its mUserArea is the open-animation frame (dMeter_swimOpenProc) and then mSwimTimerSoul's rotation frame (dMeter_swimMainRotate) (tww writes it, the same code) */
    fopMsgM_pane_class mSwimTimerMask2;  /* was field_0x2cc0: pane 'msk2' of the swim timer screen sScrTimer2 (TWW dMeter_screenDataTimeSet), drawn by dMeter_Draw 02593630; TWW only moves (dMeter_swimTransY) and hides it with the timer (tww writes it, the same code) */
    fopMsgM_pane_class mSwimTimerSoul;  /* was field_0x2cf8: pane 'soul' of the swim timer screen sScrTimer1 (TWW dMeter_screenDataTimeSet), drawn by dMeter_Draw 02593630; TWW dMeter_swimMainRotate turns it once per HIO 0x132 frames, dMeter_swimMainDown keeps it at mSwimTimerMain's height, its mUserArea is dMeter_swimMainBlink's blink counter (tww writes it, the same code) */
    fopMsgM_pane_class mSwimTimerGauge;  /* was field_0x2d30: pane 'msk1' of the swim timer screen sScrTimer1 (TWW dMeter_screenDataTimeSet), drawn by dMeter_Draw 02593630; TWW dMeter_swimMove stores the item time count in its mUserArea when the timer starts and dMeter_swimMainDown scales it from the bottom by the remaining count / that start count (tww writes it, the same code) */
    fopMsgM_pane_class field_0x2d68;
    fopMsgM_pane_class mSwimTimerMain;  /* was field_0x2da0: pane 'main' of the swim timer screen sScrTimer1 (TWW dMeter_screenDataTimeSet), drawn by dMeter_Draw 02593630; its mUserArea is the swim timer state in TWW dMeter_swimMove (0 off, 1 opening, 2 / 3 running); dMeter_swimMainDown lowers it with the gauge (tww writes it, the same code) */
    fopMsgM_pane_class mSwimTimerShadow;  /* was field_0x2dd8: pane 'kg00' of the swim timer screen sScrTimer1 (TWW dMeter_screenDataTimeSet; 'kg' = kage, shadow), drawn by dMeter_Draw 02593630; TWW dMeter_swimOpenProc pops it in by scale, its mUserArea is dMeter_swimMainBlink's colour blink frame (tww writes it, the same code) */
    fopMsgM_pane_class mRoadArrow[4];  /* was field_0x2e10: panes 'yaz4'..'yaz1' on sChoiceRoad (dMeter_screenDataArrowSet in dMeter_Create 02595C6C), drawn by dMeter_Draw 02593630; dMeter_arrowInit (in dMeter_Create 02595C6C) clears each mUserArea; TWW dMeter_arrowCheckStatus shows (mUserArea 1) or erases each direction arrow, dMeter_arrowTransScale / dMeter_arrowAnime place and animate the shown ones (tww writes it, the same code) */
    be<u32> mSwimFrameBlack;  /* was field_0x2ef0: TWW dMeter_swimInit saves mSwimTimerFrame's black colour here; dMeter_swimMove restores it, dMeter_swimMainBlink blinks from it to mSwimFrameBlinkBlack; no WWHD use (tww writes it) */
    be<u32> mSwimFrameWhite;  /* was field_0x2ef4: TWW dMeter_swimInit saves mSwimTimerFrame's white colour here; dMeter_swimMove restores it, dMeter_swimMainBlink blinks from it to mSwimFrameBlinkWhite; no WWHD use (tww writes it) */
    be<u32> mSwimFrameBlinkBlack;  /* was field_0x2ef8: the black colour mSwimTimerFrame blinks to: TWW dMeter_swimMove copies mSwimFrameBlack, dMeter_swimMainBlink sets HIO 0x14A..0x14C below half the time and blends towards it; no WWHD use (tww writes it) */
    be<u32> mSwimFrameBlinkWhite;  /* was field_0x2efc: the white colour mSwimTimerFrame blinks to: TWW dMeter_swimMove copies mSwimFrameWhite, dMeter_swimMainBlink sets the HIO colour below half the time and blends towards it; no WWHD use (tww writes it) */
    be<u32> mSwimMainBlack;  /* was field_0x2f00: TWW dMeter_swimInit saves mSwimTimerMain's black colour here; dMeter_swimMove gives it to mSwimTimerSoul and mSwimTimerMain, dMeter_swimMainBlink blinks from it; no WWHD use (tww writes it) */
    be<u32> mSwimMainBlinkColor;  /* was field_0x2f04: the black colour mSwimTimerMain / mSwimTimerSoul blink to: TWW dMeter_swimMove sets (200, 200, 255), dMeter_swimMainBlink sets HIO 0x14A..0x14C below half the time; no WWHD use (tww writes it) */
    be<u32> mMagicGaugeWhite;  /* was field_0x2f08: TWW dMeter_magicInit saves mMagicGauge1's white colour here; dMeter_magicGaugeMove gives it back to mMagicGauge1 / mMagicGauge2, dMeter_magicColor blends from it to white; no WWHD use (tww writes it) */
    be<u32> mMagicGaugeBlack;  /* was field_0x2f0c: TWW dMeter_magicInit saves mMagicGauge1's black colour here; dMeter_magicGaugeMove gives it back to mMagicGauge1 / mMagicGauge2, dMeter_magicColor blends from it; no WWHD use (tww writes it) */
    be<u32> mMagicBarEvenWhite;  /* was field_0x2f10: TWW dMeter_magicInit saves mMagicBar[0]'s white colour here; dMeter_magicGaugeMove gives it back to mMagicBar[0, 2, 4, 6]; no WWHD use (tww writes it) */
    be<u32> mMagicBarEvenBlack;  /* was field_0x2f14: TWW dMeter_magicInit saves mMagicBar[0]'s black colour here; dMeter_magicGaugeMove gives it back to mMagicBar[0, 2, 4, 6]; no WWHD use (tww writes it) */
    be<u32> mMagicBarOddWhite;  /* was field_0x2f18: TWW dMeter_magicInit saves mMagicBar[1]'s white colour here; dMeter_magicGaugeMove gives it back to mMagicBar[1, 3, 5, 7]; no WWHD use (tww writes it) */
    be<u32> mMagicBarOddBlack;  /* was field_0x2f1c: TWW dMeter_magicInit saves mMagicBar[1]'s black colour here; dMeter_magicGaugeMove gives it back to mMagicBar[1, 3, 5, 7]; no WWHD use (tww writes it) */
    be<u32> mpRupyParticle;
    be<u32> mpArrowTypeParticle[2];  /* was field_0x2f24: the two 2D particle emitters (fire / ice / light arrow, ID_HM_J2_ARW*) on the arrow type icons: dMeter_arwInit (in dMeter_Create 02595C6C) clears them, dMeter_Delete ends them with mpRupyParticle; TWW dMeter_arwMove creates them at mArrowTypeSlot[0] and ends them on a type change (tww the same) */
    be<u32> mpForestWaterParticle[2];  /* was field_0x2f2c: the two 2D particle emitters on the forest water icon: dMeter_xyMove 02599584 creates them (particle 0x2D) at mItemIcon's centre while a button holds forest water, moves them with it and gives them its alpha, and ends and clears them otherwise; dMeter_xyInit (dMeter_Create 02595C6C) clears them, dMeter_Delete 02595850 ends them with mpRupyParticle; same code TWW dMeter_xyMove / xyInit */
    be<u32> arrowTex[2];
    be<u32> actionTex[3];
    be<u32> xyIconTex[3];
    be<u32> field_0x2f54[3];
    be<u32> moveIconTex[2];
    be<f32> mClockTimePass;  /* was field_0x2f68: 02593434 (d_meter_00.cpp dMeter_gaugeInitialPositionHD; TWW dMeter_compassValueInit / clockShow the same) sets 1.0 while the room's time passes (roomControl_getTimePass) and 0.0 otherwise; TWW dMeter_compassWindClose / dMeter_compassAnimeMove compare it with the current state to rebuild the clock */
    be<f32> mCompassAlpha;  /* was field_0x2f6c: the compass alpha ratio: the alpha step inlined into dMeter_Execute 02593B10 (meterUpdateAlphaControl) fades it over dMeter_HIO_c mCompassFadeFrames when the 2D show flag changes and holds it at 0.5 / 0 while the menu window is open; dMeter_Create 02595C6C sets 1.0; TWW dMeter_alphaControl computes it the same way and dMeter_compassAlpha applies it to the compass and map panes */
    be<f32> field_0x2f70;
    be<f32> mSwimLightRiseSpeed[9];  /* was field_0x2f74: per mSwimTimerLight: TWW dMeter_swimLightBirth sets 5 + rnd 3, dMeter_swimLightAnime moves the light up by it and slows it by 0.8..1.0 each frame; no WWHD use (tww writes it) */
    be<f32> mSwimLightSwayX[9];  /* was field_0x2f98: per mSwimTimerLight: TWW dMeter_swimLightBirth sets rndFX 5, dMeter_swimLightAnime sways the light by it * sin around its x and shrinks it by 0.9..1.0 each frame; no WWHD use (tww writes it) */
    be<f32> mHeartAlpha;  /* was field_0x2fbc: alpha ratio of the hearts: dMeter_Execute 02593B10 (inlined heart alpha step) sets valueIncrease(5, mHeartAlphaStep); dMeter_gaugeAlpha 0259C2BC applies it to mHeart, mHeartShadow and mHeartLight; dMeter_heartInit 02591B4C clears it; dMeter_Execute copies it to the HD state (MeterUpdateSharedState mHeartAlpha); same code TWW dMeter_heartAlpha / dMeter_gaugeAlpha */
    be<f32> mMagicAlpha;  /* was field_0x2fc0: alpha ratio of the magic gauge: dMeter_magicGaugeMove 0259B698 sets valueIncrease(5, mMagicGauge3.mUserArea), dMeter_flyGaugeMove 02592EE8 sets its own fade; dMeter_gaugeAlpha 0259C2BC applies it to mMagicBar, mMagicGauge3, mMagicGauge1, mMagicGaugeShadow1; dMeter_Create 02595C6C clears it; dMeter_Execute 02593B10 copies it to the HD state (MeterUpdateSharedState mMagicAlpha); same code TWW dMeter_magicAlpha / dMeter_gaugeAlpha */
    be<s16> mSwimTekariFrame;  /* was field_0x2fc4: TWW dMeter_swimMove clears it, dMeter_swimTekariScroll counts it as the frame of the shine scroll over mSwimTimerFrame and wraps it; no WWHD use (tww writes it) */
    u8 before_field_0x2fd8[0x12];
    be<s16> mCompassFadeTimer;  /* was field_0x2fd8: the frame counter of the mCompassAlpha fade: the alpha step inlined into dMeter_Execute 02593B10 counts it down / up and passes it to valueIncrease(mCompassFadeFrames, ...); dMeter_Create 02595C6C starts it at mCompassFadeFrames; same code TWW dMeter_alphaControl / dMeter_Create */
    be<s16> mKeyLight2Frames[5];  /* was field_0x2fda: dMeter_keyMove 0259C7FC: the five dMeter_keyLight frame thresholds of mKeyLight2, rolled from the dMeter_HIO_c mKeyLight* values when mKeyLight2's counter is 1 */
    be<s16> mStarLightFrames0;  /* was field_0x2fe4: s16 pair (the second half is the 2 bytes after it): TWW dMeter_starLightFrame rolls the light-up and end frames (HIO 0x90 / 0x88 + random), dMeter_clockMove / dMeter_clockStarLight twinkle mClockStar[0] / mFixedClockStar[0] by them; no WWHD use (tww writes it) */
    u8 before_field_0x2fe8[0x2];
    be<s16> mStarLightFrames1;  /* was field_0x2fe8: s16 pair: as mStarLightFrames0 for mClockStar[1] / mFixedClockStar[1] (TWW dMeter_clockMove); no WWHD use (tww writes it) */
    u8 before_field_0x2fec[0x2];
    be<s16> mStarLightFrames2;  /* was field_0x2fec: s16 pair: as mStarLightFrames0 for mClockStar[2] / mFixedClockStar[2] (TWW dMeter_clockMove); no WWHD use (tww writes it) */
    u8 before_field_0x2ff0[0x2];
    be<s16> mStarLightFrames3;  /* was field_0x2ff0: s16 pair: as mStarLightFrames0 for mFixedClockStar[3] (TWW dMeter_clockMove); no WWHD use (tww writes it) */
    u8 before_mAdjustRupy[0xA];
    be<s32> mAdjustRupy;
    be<s32> mSwimBlinkPeriod;  /* was field_0x3000: the half period of the swim timer colour blink: TWW dMeter_swimMove sets HIO 0x130, dMeter_swimMainBlink blends over it and below half the time moves it towards HIO 0x134 (0 when the time is out); no WWHD use (tww writes it) */
    be<u32> mStatusFlags;
    be<u16> mRupyCount;
    be<s16> mAdjustHp;
    be<s16> mMaxLifeChange;  /* was field_0x300c: the heart containers still to add or remove: dMeter_Execute 02593B10 (the heart move inlined) sets the new max life - mMaxHP when the max-life item count arrives and steps mMaxHP by 1 towards it each frame (TWW dMeter_LifeMove / dMeter_maxLifeChange the same) */
    be<s16> mKeyNumPending;  /* was field_0x300e: dMeter_keyMove 0259C7FC sets it to the new saved key count minus mDispKeyNum when play.mItemKeyNumCount is applied, then counts it to 0 while mDispKeyNum follows */
    be<s16> mSwimLightTimer;  /* was field_0x3010: the frames until the next mSwimTimerLight: TWW dMeter_swimMove clears it, dMeter_swimLightMove counts it down and births a light at 0, dMeter_swimLightBirth rolls rnd(HIO 0x138); no WWHD use (tww writes it) */
    be<s16> mClockNight;  /* was field_0x3012: 02593434 (d_meter_00.cpp; TWW dMeter_compassValueInit / clockShow the same) sets 0 between 90 and 270 degrees of the day and 1 otherwise; TWW dMeter_compassWindClose / dMeter_compassAnimeMove rebuild the fixed clock when it changes */
    be<s16> mHeartAlphaStep;  /* was field_0x3014: the heart fade step 0..5: the heart alpha step inlined into dMeter_Execute 02593B10 (meterUpdateActionFade) moves it toward 0, 3 or 5 by the meter state and sets mHeartAlpha = valueIncrease(5, it); dMeter_heartInit 02591B4C sets 5; same code TWW dMeter_heartAlpha / dMeter_heartInit */
    be<u16> mDispMagic;  /* was field_0x3016: the shown magic, in 1/100 of the mMagicBar width: dMeter_Create 02595C6C sets it from the saved magic; dMeter_magicGaugeMove 0259B698 steps it by 625 toward magic * mMagicBar[0] width * 25 and passes it / 100 to dMeter_magicChange; same code TWW dMeter_magicInit / dMeter_magicGaugeMove */
    be<u8> mDispMaxMagic;  /* was field_0x3018: the shown magic capacity: dMeter_Create 02595C6C sets it from the saved mMaxMagic; dMeter_magicGaugeMove 0259B698 steps it by one per frame toward the saved mMaxMagic and calls dMeter_magicLength with it / 32; same code TWW dMeter_magicInit / dMeter_magicGaugeMove */
    be<u8> field_0x3019;
    be<u8> mLastAStatus;  /* was field_0x301a: the AStatus the weapon button shows: dMeter_weponMove 025986DC compares it with play mAStatus, sets mWeaponIcon / mWeaponButton2 mUserArea (texture change, bounce) and stores the new status when they differ; dMeter_weponInit (dMeter_Create 02595C6C) clears it; same code TWW dMeter_weponChange / weponInit */
    be<u8> mLastDoStatus;  /* was field_0x301b: the DoStatus the action button shows: meterUpdateActionChange (dMeter_Execute 02593B10) stores play mDoStatus after a change step and sets mActionBowIcon.mUserArea when they differ; dMeter_actionInit (dMeter_Create 02595C6C) clears it; same code TWW dMeter_actionChange / actionInit */
    be<u8> mRStatusShown;  /* was field_0x301c: dMeter_rInit (in dMeter_Create 02595C6C) clears it; dMeter_rAlpha 0259A5B0 stores dComIfGp_getRStatus() when it takes mRActionIcon's change request and requests a change while the R status differs from it (TWW dMeter_rMove the same) */
    be<u8> mDispKeyNum;  /* was field_0x301d: dMeter_Create 02595C6C stores dComIfGs_getKeyNum(); dMeter_keyMove 0259C7FC steps it by one per frame toward the saved key count (mKeyNumPending); the shown key count */
    be<u8> mMaxHP;
    be<u8> mCurrHP;
    be<u8> mCompassState;  /* was field_0x3020: the compass panel state: 02593434 (d_meter_00.cpp; TWW dMeter_compassValueInit) sets 1; TWW dMeter_compassGetOnProc / GetOffProc switch on it and GetOnProc sets 2 / 5, GetOffProc 9 / 8, dMeter_compassDirOpen 3, WindOpen 7, WindClose 6 / 3, AnimeMove 8; dMeter_menuPlusMove, dMeter_mapMove and dMeter_Draw test it */
    be<u8> mLastSelectItem[3];  /* was field_0x3021: the item each X / Y / Z button shows: dMeter_xyMove 02599584 compares it with play mSelectItem[i], calls dMeter_xyItemChange and stores the new item when they differ; dMeter_xyInit (dMeter_Create 02595C6C) sets it from mSelectItem; same code TWW dMeter_xyMove / xyInit */
    be<u8> mButtonActionMode;  /* was field_0x3024: the button action mode bits: dMeter_Execute 02593B10 clears it each frame, the button steps set 1 (action alpha, inlined into dMeter_Execute), 2 (dMeter_weponMove 025986DC), 4 / 8 / 0x20 / 0x40 (dMeter_xyMove 02599584) and 0x10 (dMeter_rAlpha 0259A5B0); dMeter_Execute keeps only 0x13 while the HUD context masks the buttons and copies it to play mButtonMode; TWW dMeter_Execute passes it to dComIfGp_setButtonActionMode */
    be<s8> mRoadArrowFrame;  /* was field_0x3025: dMeter_arrowInit (in dMeter_Create 02595C6C) clears it; TWW dMeter_arrowMove counts it to 30 and back to 10, dMeter_arrowAnime animates mRoadArrow by it and dMeter_arrowCheckStatus changes the arrows only at 0 or 20 (tww writes it, the same code) */
    be<u8> mButtonMoveFlags;  /* was field_0x3026: one bit per button group that has finished moving out (1 action, inlined into dMeter_Execute 02593B10; 2 weapon, dMeter_weponMove 025986DC; 4 item, dMeter_xyMove 02599584; 8 R, dMeter_rAlpha 0259A5B0): a group clears its bit when its move status changes, counts its move frame up to 5 and sets the bit; once all four are set (15) every group counts its frame back down; same code TWW dMeter_weponTrans / actionTrans / xyTrans / rTrans */
    be<u8> field_0x3027;
    be<u8> mMetronomeState;  /* was field_0x3028: dMeter_Create 02595C6C clears it; TWW dMeter_metronomeMove: 1 while the metronome (dMn_c) is open, 2 while it closes, 0 once deleted; dMeter_Draw draws it only at 1 (tww writes it, the same code) */
    be<u8> mMapOn;  /* was field_0x3029: 1 while the map is switched on: TWW dMeter_compassInit sets it from dMeter_mMapCtrlDisp, dMeter_mapMove sets 1 / 0 on the d-pad right / left (JA_SE_MAP_SHOW / CANCEL), dMeter_menuPlusMove / dMeter_compassGetOnProc set it; dMeter_mapMove tests it; no WWHD use (tww writes it) */
    be<u8> mCompassSlideSe;  /* was field_0x302a: 1 while the compass slide sound runs: TWW dMeter_compassDirOpen / WindOpen / WindClose / DirClose start JA_SE_COMPASS_MOVE and set it, clear it with JA_SE_MAP_SHOW_END at the end; dMeter_compassValueInit clears it; no WWHD use (tww writes it) */
    u8 before_mRupyAlpha[0x1];
    be<f32> mRupyAlpha;
    be<f32> mKeyAlpha;
    be<u8> mButtonShowFlags;
    u8 before_mWindID[0x3];
    be<u32> mWindID;
    be<u16> mWindItem;
};
WWHD_OFFSET(sub_meter_class, mHeart, 0x644);
WWHD_OFFSET(sub_meter_class, mHeartShadow, 0xAA4);
WWHD_OFFSET(sub_meter_class, mHeartLight, 0xF04);
WWHD_OFFSET(sub_meter_class, mMagicBar, 0xF3C);
WWHD_OFFSET(sub_meter_class, mMagicGauge3, 0x10FC);
WWHD_OFFSET(sub_meter_class, mMagicGauge2, 0x1134);
WWHD_OFFSET(sub_meter_class, mMagicGaugeShadow2, 0x116C);
WWHD_OFFSET(sub_meter_class, mMagicGauge1, 0x11A4);
WWHD_OFFSET(sub_meter_class, mMagicGaugeShadow1, 0x11DC);
WWHD_OFFSET(sub_meter_class, field_0x1210, 0x1214);
WWHD_OFFSET(sub_meter_class, mCompassNeedle, 0x124C);
WWHD_OFFSET(sub_meter_class, mCompassNeedleShadow, 0x1284);
WWHD_OFFSET(sub_meter_class, field_0x12b8, 0x12BC);
WWHD_OFFSET(sub_meter_class, mCompassBase, 0x12F4);
WWHD_OFFSET(sub_meter_class, field_0x1328, 0x132C);
WWHD_OFFSET(sub_meter_class, field_0x1360, 0x1364);
WWHD_OFFSET(sub_meter_class, mClockBar, 0x139C);
WWHD_OFFSET(sub_meter_class, mClockRing, 0x13D4);
WWHD_OFFSET(sub_meter_class, mClockHorizon1, 0x140C);
WWHD_OFFSET(sub_meter_class, mClockHorizon2, 0x1444);
WWHD_OFFSET(sub_meter_class, mClockStar, 0x147C);
WWHD_OFFSET(sub_meter_class, mClockMoon, 0x1524);
WWHD_OFFSET(sub_meter_class, mClockSun, 0x155C);
WWHD_OFFSET(sub_meter_class, mClockCircle, 0x1594);
WWHD_OFFSET(sub_meter_class, mClockTimeDial, 0x1674);
WWHD_OFFSET(sub_meter_class, mClockCenter, 0x16AC);
WWHD_OFFSET(sub_meter_class, mFixedClockRing, 0x16E4);
WWHD_OFFSET(sub_meter_class, mFixedClockStar, 0x171C);
WWHD_OFFSET(sub_meter_class, mFixedClockMoon, 0x17FC);
WWHD_OFFSET(sub_meter_class, mFixedClockCloudClip, 0x1834);
WWHD_OFFSET(sub_meter_class, mFixedClockCloud, 0x186C);
WWHD_OFFSET(sub_meter_class, mFixedClockSun, 0x1914);
WWHD_OFFSET(sub_meter_class, mCompassClock, 0x194C);
WWHD_OFFSET(sub_meter_class, mKeyLight1, 0x1984);
WWHD_OFFSET(sub_meter_class, mKeyLight2, 0x19BC);
WWHD_OFFSET(sub_meter_class, mRupyDigitA, 0x19F4);
WWHD_OFFSET(sub_meter_class, mKeyDigitA, 0x1AD4);
WWHD_OFFSET(sub_meter_class, mRupyDigitB, 0x1B44);
WWHD_OFFSET(sub_meter_class, mKeyDigitB, 0x1C24);
WWHD_OFFSET(sub_meter_class, mKeyIcon, 0x1C94);
WWHD_OFFSET(sub_meter_class, mRActionIcon, 0x1CCC);
WWHD_OFFSET(sub_meter_class, mWeaponText, 0x1D04);
WWHD_OFFSET(sub_meter_class, mActionText, 0x1D3C);
WWHD_OFFSET(sub_meter_class, field_0x1d70, 0x1D74);
WWHD_OFFSET(sub_meter_class, mActionButton, 0x1DAC);
WWHD_OFFSET(sub_meter_class, mActionButton2, 0x1E1C);
WWHD_OFFSET(sub_meter_class, field_0x1e88, 0x1E8C);
WWHD_OFFSET(sub_meter_class, field_0x1ec0, 0x1EC4);
WWHD_OFFSET(sub_meter_class, field_0x1ef8, 0x1EFC);
WWHD_OFFSET(sub_meter_class, mItemDigit10, 0x1F34);
WWHD_OFFSET(sub_meter_class, mItemDigit1, 0x1FDC);
WWHD_OFFSET(sub_meter_class, mItemIcon, 0x2084);
WWHD_OFFSET(sub_meter_class, mItemIconShadow, 0x212C);
WWHD_OFFSET(sub_meter_class, field_0x21d0, 0x21D4);
WWHD_OFFSET(sub_meter_class, field_0x2208, 0x220C);
WWHD_OFFSET(sub_meter_class, field_0x2240, 0x2244);
WWHD_OFFSET(sub_meter_class, field_0x2278, 0x227C);
WWHD_OFFSET(sub_meter_class, field_0x22b0, 0x22B4);
WWHD_OFFSET(sub_meter_class, field_0x22e8, 0x22EC);
WWHD_OFFSET(sub_meter_class, mBowLight1, 0x2324);
WWHD_OFFSET(sub_meter_class, mBowLight2, 0x23CC);
WWHD_OFFSET(sub_meter_class, mBowLight3, 0x2474);
WWHD_OFFSET(sub_meter_class, mZButton, 0x251C);
WWHD_OFFSET(sub_meter_class, mXYButton, 0x2554);
WWHD_OFFSET(sub_meter_class, field_0x25c0, 0x25C4);
WWHD_OFFSET(sub_meter_class, field_0x2630, 0x2634);
WWHD_OFFSET(sub_meter_class, mRButton, 0x26A4);
WWHD_OFFSET(sub_meter_class, field_0x26d8, 0x26DC);
WWHD_OFFSET(sub_meter_class, field_0x2710, 0x2714);
WWHD_OFFSET(sub_meter_class, mArrowDigit10, 0x274C);
WWHD_OFFSET(sub_meter_class, mArrowDigit1, 0x2784);
WWHD_OFFSET(sub_meter_class, mActionBowIcon, 0x27BC);
WWHD_OFFSET(sub_meter_class, mActionBowIconShadow, 0x27F4);
WWHD_OFFSET(sub_meter_class, mWeaponIcon, 0x282C);
WWHD_OFFSET(sub_meter_class, mWeaponIconShadow, 0x2864);
WWHD_OFFSET(sub_meter_class, mWeaponButton, 0x289C);
WWHD_OFFSET(sub_meter_class, mWeaponButton2, 0x28D4);
WWHD_OFFSET(sub_meter_class, mButtonCenter, 0x290C);
WWHD_OFFSET(sub_meter_class, mArrowTypeIcon, 0x2944);
WWHD_OFFSET(sub_meter_class, mArrowTypeSlot, 0x29B4);
WWHD_OFFSET(sub_meter_class, mRupyIcon, 0x2A24);
WWHD_OFFSET(sub_meter_class, mRupyIconB, 0x2A5C);
WWHD_OFFSET(sub_meter_class, mSwimTimerLight, 0x2A94);
WWHD_OFFSET(sub_meter_class, mSwimTimerFrame, 0x2C8C);
WWHD_OFFSET(sub_meter_class, mSwimTimerMask2, 0x2CC4);
WWHD_OFFSET(sub_meter_class, mSwimTimerSoul, 0x2CFC);
WWHD_OFFSET(sub_meter_class, mSwimTimerGauge, 0x2D34);
WWHD_OFFSET(sub_meter_class, field_0x2d68, 0x2D6C);
WWHD_OFFSET(sub_meter_class, mSwimTimerMain, 0x2DA4);
WWHD_OFFSET(sub_meter_class, mSwimTimerShadow, 0x2DDC);
WWHD_OFFSET(sub_meter_class, mRoadArrow, 0x2E14);
WWHD_OFFSET(sub_meter_class, mSwimFrameBlack, 0x2EF4);
WWHD_OFFSET(sub_meter_class, mSwimFrameWhite, 0x2EF8);
WWHD_OFFSET(sub_meter_class, mSwimFrameBlinkBlack, 0x2EFC);
WWHD_OFFSET(sub_meter_class, mSwimFrameBlinkWhite, 0x2F00);
WWHD_OFFSET(sub_meter_class, mSwimMainBlack, 0x2F04);
WWHD_OFFSET(sub_meter_class, mSwimMainBlinkColor, 0x2F08);
WWHD_OFFSET(sub_meter_class, mMagicGaugeWhite, 0x2F0C);
WWHD_OFFSET(sub_meter_class, mMagicGaugeBlack, 0x2F10);
WWHD_OFFSET(sub_meter_class, mMagicBarEvenWhite, 0x2F14);
WWHD_OFFSET(sub_meter_class, mMagicBarEvenBlack, 0x2F18);
WWHD_OFFSET(sub_meter_class, mMagicBarOddWhite, 0x2F1C);
WWHD_OFFSET(sub_meter_class, mMagicBarOddBlack, 0x2F20);
WWHD_OFFSET(sub_meter_class, mpRupyParticle, 0x2F24);
WWHD_OFFSET(sub_meter_class, mpArrowTypeParticle, 0x2F28);
WWHD_OFFSET(sub_meter_class, mpForestWaterParticle, 0x2F30);
WWHD_OFFSET(sub_meter_class, arrowTex, 0x2F38);
WWHD_OFFSET(sub_meter_class, actionTex, 0x2F40);
WWHD_OFFSET(sub_meter_class, xyIconTex, 0x2F4C);
WWHD_OFFSET(sub_meter_class, field_0x2f54, 0x2F58);
WWHD_OFFSET(sub_meter_class, moveIconTex, 0x2F64);
WWHD_OFFSET(sub_meter_class, mClockTimePass, 0x2F6C);
WWHD_OFFSET(sub_meter_class, mCompassAlpha, 0x2F70);
WWHD_OFFSET(sub_meter_class, field_0x2f70, 0x2F74);
WWHD_OFFSET(sub_meter_class, mSwimLightRiseSpeed, 0x2F78);
WWHD_OFFSET(sub_meter_class, mSwimLightSwayX, 0x2F9C);
WWHD_OFFSET(sub_meter_class, mHeartAlpha, 0x2FC0);
WWHD_OFFSET(sub_meter_class, mMagicAlpha, 0x2FC4);
WWHD_OFFSET(sub_meter_class, mSwimTekariFrame, 0x2FC8);
WWHD_OFFSET(sub_meter_class, mCompassFadeTimer, 0x2FDC);
WWHD_OFFSET(sub_meter_class, mKeyLight2Frames, 0x2FDE);
WWHD_OFFSET(sub_meter_class, mStarLightFrames0, 0x2FE8);
WWHD_OFFSET(sub_meter_class, mStarLightFrames1, 0x2FEC);
WWHD_OFFSET(sub_meter_class, mStarLightFrames2, 0x2FF0);
WWHD_OFFSET(sub_meter_class, mStarLightFrames3, 0x2FF4);
WWHD_OFFSET(sub_meter_class, mAdjustRupy, 0x3000);
WWHD_OFFSET(sub_meter_class, mSwimBlinkPeriod, 0x3004);
WWHD_OFFSET(sub_meter_class, mStatusFlags, 0x3008);
WWHD_OFFSET(sub_meter_class, mRupyCount, 0x300C);
WWHD_OFFSET(sub_meter_class, mAdjustHp, 0x300E);
WWHD_OFFSET(sub_meter_class, mMaxLifeChange, 0x3010);
WWHD_OFFSET(sub_meter_class, mKeyNumPending, 0x3012);
WWHD_OFFSET(sub_meter_class, mSwimLightTimer, 0x3014);
WWHD_OFFSET(sub_meter_class, mClockNight, 0x3016);
WWHD_OFFSET(sub_meter_class, mHeartAlphaStep, 0x3018);
WWHD_OFFSET(sub_meter_class, mDispMagic, 0x301A);
WWHD_OFFSET(sub_meter_class, mDispMaxMagic, 0x301C);
WWHD_OFFSET(sub_meter_class, field_0x3019, 0x301D);
WWHD_OFFSET(sub_meter_class, mLastAStatus, 0x301E);
WWHD_OFFSET(sub_meter_class, mLastDoStatus, 0x301F);
WWHD_OFFSET(sub_meter_class, mRStatusShown, 0x3020);
WWHD_OFFSET(sub_meter_class, mDispKeyNum, 0x3021);
WWHD_OFFSET(sub_meter_class, mMaxHP, 0x3022);
WWHD_OFFSET(sub_meter_class, mCurrHP, 0x3023);
WWHD_OFFSET(sub_meter_class, mCompassState, 0x3024);
WWHD_OFFSET(sub_meter_class, mLastSelectItem, 0x3025);
WWHD_OFFSET(sub_meter_class, mButtonActionMode, 0x3028);
WWHD_OFFSET(sub_meter_class, mRoadArrowFrame, 0x3029);
WWHD_OFFSET(sub_meter_class, mButtonMoveFlags, 0x302A);
WWHD_OFFSET(sub_meter_class, field_0x3027, 0x302B);
WWHD_OFFSET(sub_meter_class, mMetronomeState, 0x302C);
WWHD_OFFSET(sub_meter_class, mMapOn, 0x302D);
WWHD_OFFSET(sub_meter_class, mCompassSlideSe, 0x302E);
WWHD_OFFSET(sub_meter_class, mRupyAlpha, 0x3030);
WWHD_OFFSET(sub_meter_class, mKeyAlpha, 0x3034);
WWHD_OFFSET(sub_meter_class, mButtonShowFlags, 0x3038);
WWHD_OFFSET(sub_meter_class, mWindID, 0x303C);
WWHD_OFFSET(sub_meter_class, mWindItem, 0x3040);
struct SaveRoot {
    u8 beforeSave[0x20];
    dSv_info_c mSave;
};
struct StageInfo {
    u8 beforeDisplay[9];
    be<u8> mDisplayFlags;
    be<u16> mStageFlags;
    be<u32> mTypeFlags;
};
struct Particle {
    u8 beforeMaxFrame[0x5C];
    be<s32> mMaxFrame;
    u8 beforeTranslation[0x22C - 0x60];
    cXyz mGlobalTranslation;
    u8 beforeAlpha[0x247 - 0x238];
    be<u8> mGlobalAlpha;
    u8 beforeFlags[0x254 - 0x248];
    be<u32> mFlags;
    u8 beforeMode[0x262 - 0x258];
    be<u8> mDrawMode;
};
struct Gallery {
    u8 beforePictures[0x3C030C];
    be<u8> mPictureNum;
};
struct SafeStringView {
    be<u32> text;
    be<u32> vtable;
};
struct ScenePair {
    SafeStringView left, right;
};
struct Thresholds {
    be<s16> values[5];
};
struct dMeter_HIO_c {
    u8 before_C[0xC];
    be<f32> mBossLifeRatioDefault;  /* was field_0xc: named as the dMeter_HIO_c class field (d_meter_01.h), evidence there */
    u8 before_62[0x52];
    be<u8> mIgnoreArrowNumCount;  /* was field_0x62: named as the dMeter_HIO_c class field (d_meter_01.h), evidence there */
    be<u8> mIgnoreBombNumCount;  /* was field_0x63: named as the dMeter_HIO_c class field (d_meter_01.h), evidence there */
    u8 before_88[0x24];
    // Contiguous tww fields 0x88/0x8a/0x8c and 0x92/0x94/0x96 are indexed as groups.
    be<s16> mKeyLightStep[3];  /* was field_0x88: dMeter_keyMove 0259C7FC: mKeyLight2Frames[i+1] = previous frame + mKeyLightStep[i] + cM_rndFX(mKeyLightStepRange[i]), i = 0..2 (0x88/0x8A/0x8C) */
    be<s16> mKeyLightEndStep;  /* was field_0x8e: dMeter_keyMove 0259C7FC: mKeyLight2Frames[4] = mKeyLight2Frames[3] + mKeyLightEndStep + cM_rndFX(mKeyLightEndRange) */
    be<s16> mKeyLightStart;  /* was field_0x90: dMeter_keyMove 0259C7FC: base of mKeyLight2Frames[0] (plus cM_rndFX(mKeyLightStartRange)) */
    be<s16> mKeyLightStepRange[3];  /* was field_0x92: dMeter_keyMove 0259C7FC: cM_rndFX ranges for the mKeyLightStep steps (0x92/0x94/0x96) */
    be<s16> mKeyLightEndRange;  /* was field_0x98: dMeter_keyMove 0259C7FC: cM_rndFX range of the last mKeyLight2Frames step */
    be<s16> mKeyLightStartRange;  /* was field_0x9a: dMeter_keyMove 0259C7FC: cM_rndFX range added to mKeyLightStart for mKeyLight2Frames[0] */
};
// HD: these verified scene bytes have different semantics from the inferred shared fields/gaps.
// This private view stays until the scene block is verified.
struct SceneState {
    u8 beforeWind[0x5BE8];
    be<u8> mOperateWind;
    u8 beforeBlocked[3];
    be<u8> mWindBlocked;
    u8 beforeMagicPulse[2];
    be<u8> mMagicPulse;
    u8 beforeArrowMode[0x5C2E - 0x5BF0];
    be<u8> mArrowMode;
    be<u8> mArrowModeForce;
};
inline dComIfG_inf_c *meterGame(u32 address) {
    return gabi::at<dComIfG_inf_c>(address);
}
inline SaveRoot *meterSave(u32 address) {
    return gabi::at<SaveRoot>(address);
}
inline fopAc_ac_c *meterActor(u32 address) {
    return gabi::at<fopAc_ac_c>(address);
}
inline Particle *meterParticle(u32 address) {
    return gabi::at<Particle>(address);
}
inline SceneState *meterScene(u32 address) {
    return gabi::at<SceneState>(address);
}
inline StageInfo *meterStageView(u32 address) {
    return gabi::at<StageInfo>(address);
}
inline Gallery *meterGallery(u32 address) {
    return gabi::at<Gallery>(address);
}
} // namespace meter_detail
void dMeter_hideCounter(u32);
void dMeter_showCounter(u32);
void fopMsgM_setNowAlpha(fopMsgM_pane_class *, f32);
void fopMsgM_paneScaleOne(fopMsgM_pane_class *, f32);
void fopMsgM_setNowAlphaZero(fopMsgM_pane_class *);
f32 fopMsgM_valueIncrease(s32, s32, u32);
u32 meterIsMagicBlocked();
void dMeter_compassValueInit(meter_detail::sub_meter_class *);
u32 meterGetMetronome();
void meterUpdateEvent(u32);
void dMeter_walletChange(meter_detail::sub_meter_class *);
u32 meterRupyStep(u32);
void mDoAud_seStart(u32);
void dMeter_keyLight(fopMsgM_pane_class *, void *, f32);
void dMeter_alphaFlagOff(void *, u32);
u32 dMeter_alphaFlagCheck(void *, u32);
void dMeter_alphaFlagOn(void *, u32);
u32 meterActorJudge(u32, u32);
u32 meterCb1MaxFlight();
u32 meterMdMaxFlight();
void dMeter_flyGaugeMove(meter_detail::sub_meter_class *, u32, u32, s32);
void dMeter_magicTransScale(meter_detail::sub_meter_class *, f32, f32, f32);
void dMeter_magicInitTrans(meter_detail::sub_meter_class *);
void dMeter_magicChange(meter_detail::sub_meter_class *, f32);
void dMeter_magicLength(meter_detail::sub_meter_class *, f32);
u32 meterOverlapIsDoingReq();
u32 meterCheckStartDemo(u32);
void mDoAud_seStartLevel(u32, u32, u32);
u32 meterPlayerMagicUse(u32);
void meterAssertion(u32, u32, u32);
u32 meterLockonTarget(u32, u32);
u32 meterIsCollect(u32, u32, u32);
u32 meterLockonTruth(u32);
void meterCopyPosition(u32, u32);
u32 meterIsStageBoss(u32, u32);
void dMeter_bossEnemyHide(meter_detail::sub_meter_class *);
void dMeter_xyItemChange(meter_detail::sub_meter_class *, u32);
u32 meterGetPictureRecord(u32);
void meterSetPictureNum(u32, u32);
u32 meterCheckGetItem(u32);
u32 meterItemTable(u32);
void meterRemoveBagItem(u32, u32);
void dMeter_xyItemCountUp(meter_detail::sub_meter_class *, u32);
u32 meterItemDim(meter_detail::sub_meter_class *, u32);
u32 meterParticleSet(u32, u32, u32, u32, u32, u32, u32, u32, s32, u32, u32, u32);
f32 dMeter_alphaClose(be<s16> *, be<s16> *);
f32 dMeter_alphaOpen(be<s16> *, be<s16> *);

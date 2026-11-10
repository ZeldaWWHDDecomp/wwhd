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
    fopMsgM_pane_class field_0x1248;
    fopMsgM_pane_class field_0x1280;
    fopMsgM_pane_class field_0x12b8;
    fopMsgM_pane_class field_0x12f0;
    fopMsgM_pane_class field_0x1328;
    fopMsgM_pane_class field_0x1360;
    fopMsgM_pane_class field_0x1398;
    fopMsgM_pane_class field_0x13d0;
    fopMsgM_pane_class field_0x1408;
    fopMsgM_pane_class field_0x1440;
    fopMsgM_pane_class field_0x1478[3];
    fopMsgM_pane_class field_0x1520;
    fopMsgM_pane_class field_0x1558;
    fopMsgM_pane_class field_0x1590[4];
    fopMsgM_pane_class field_0x1670;
    fopMsgM_pane_class field_0x16a8;
    fopMsgM_pane_class field_0x16e0;
    fopMsgM_pane_class field_0x1718[4];
    fopMsgM_pane_class field_0x17f8;
    fopMsgM_pane_class field_0x1830;
    fopMsgM_pane_class field_0x1868[3];
    fopMsgM_pane_class field_0x1910;
    fopMsgM_pane_class field_0x1948;
    fopMsgM_pane_class mKeyLight1;  /* was field_0x1980: pane 'kyl1' (dMeter_screenDataSet, inlined into dMeter_Create 02595C6C); dMeter_keyMove 0259C7FC runs dMeter_keyLight on it with the fixed frames {30,40,45,50,58} (its mUserArea is the frame counter); dMeter_Create sets the counter to 1; same code TWW d_meter.cpp dMeter_keyMove/dMeter_keyInit */
    fopMsgM_pane_class mKeyLight2;  /* was field_0x19b8: pane 'kyl2' (dMeter_screenDataSet in dMeter_Create 02595C6C); dMeter_keyMove 0259C7FC runs dMeter_keyLight on it with mKeyLight2Frames and re-rolls those frames when its counter is 1; dMeter_Create sets the counter to 1 */
    fopMsgM_pane_class mRupyDigitA[4];  /* was field_0x19f0: panes 'nm00'..'nm03' on sMainParts2 (dMeter_screenDataSet in dMeter_Create 02595C6C): the rupee count digits (TWW dMeter_rupyInit / dMeter_walletChange give them the same digit textures as mRupyDigitB); dMeter_rupyMove 0259C454 fades them; [0].mUserArea is its dMeter_alphaOpen/alphaClose second counter (dMeter_Create sets 1), [1].mUserArea the shown wallet size (dMeter_rupyMove calls dMeter_walletChange 025931CC when the saved mWalletSize differs); dMeter_walletChange moves mRupyIcon by the [1]-[0] x distance */
    fopMsgM_pane_class mKeyDigitA[2];  /* was field_0x1ad0: panes 'nm03'/'nm04' (dMeter_screenDataSet in dMeter_Create 02595C6C): the tens/ones digits of the key count (TWW dMeter_keyInit/dMeter_keyMove blendInit/blendDraw them with the same digit as mKeyDigitB); dMeter_keyMove 0259C7FC sets their alpha, [0].mUserArea is the second dMeter_alphaOpen/alphaClose counter */
    fopMsgM_pane_class mRupyDigitB[4];  /* was field_0x1b40: panes 'nm04'..'nm07' on sMainParts2 (dMeter_screenDataSet in dMeter_Create 02595C6C): the second copy of the rupee count digits (same digit textures as mRupyDigitA in TWW dMeter_rupyInit / dMeter_walletChange); dMeter_rupyMove 0259C454 fades them with mRupyDigitA */
    fopMsgM_pane_class mKeyDigitB[2];  /* was field_0x1c20: panes 'nm08'/'nm09' (dMeter_screenDataSet in dMeter_Create 02595C6C): the second copy of the key count digits (same digit as mKeyDigitA in TWW dMeter_keyInit/dMeter_keyMove); dMeter_keyMove 0259C7FC sets their alpha */
    fopMsgM_pane_class mKeyIcon;  /* was field_0x1c90: pane 'key0' (dMeter_screenDataSet in dMeter_Create 02595C6C); dMeter_keyMove 0259C7FC fades it with the key digits (its mUserArea is the dMeter_alphaOpen/alphaClose counter); dMeter_Create clears it */
    fopMsgM_pane_class field_0x1cc8;
    fopMsgM_pane_class field_0x1d00;
    fopMsgM_pane_class field_0x1d38;
    fopMsgM_pane_class field_0x1d70;
    fopMsgM_pane_class field_0x1da8[2];
    fopMsgM_pane_class field_0x1e18[2];
    fopMsgM_pane_class field_0x1e88;
    fopMsgM_pane_class field_0x1ec0;
    fopMsgM_pane_class field_0x1ef8;
    fopMsgM_pane_class field_0x1f30[3];
    fopMsgM_pane_class field_0x1fd8[3];
    fopMsgM_pane_class field_0x2080[3];
    fopMsgM_pane_class field_0x2128[3];
    fopMsgM_pane_class field_0x21d0;
    fopMsgM_pane_class field_0x2208;
    fopMsgM_pane_class field_0x2240;
    fopMsgM_pane_class field_0x2278;
    fopMsgM_pane_class field_0x22b0;
    fopMsgM_pane_class field_0x22e8;
    fopMsgM_pane_class field_0x2320[3];
    fopMsgM_pane_class field_0x23c8[3];
    fopMsgM_pane_class field_0x2470[3];
    fopMsgM_pane_class field_0x2518;
    fopMsgM_pane_class field_0x2550[2];
    fopMsgM_pane_class field_0x25c0[2];
    fopMsgM_pane_class field_0x2630[2];
    fopMsgM_pane_class field_0x26a0;
    fopMsgM_pane_class field_0x26d8;
    fopMsgM_pane_class field_0x2710;
    fopMsgM_pane_class field_0x2748;
    fopMsgM_pane_class field_0x2780;
    fopMsgM_pane_class field_0x27b8;
    fopMsgM_pane_class field_0x27f0;
    fopMsgM_pane_class field_0x2828;
    fopMsgM_pane_class field_0x2860;
    fopMsgM_pane_class field_0x2898;
    fopMsgM_pane_class field_0x28d0;
    fopMsgM_pane_class field_0x2908;
    fopMsgM_pane_class field_0x2940[2];
    fopMsgM_pane_class field_0x29b0[2];
    fopMsgM_pane_class mRupyIcon;  /* was field_0x2a20: pane 'rup1' on sMainParts2 (dMeter_screenDataSet in dMeter_Create 02595C6C): the rupee icon; dMeter_rupyMove 0259C454 fades it (its mUserArea is the dMeter_alphaOpen/alphaClose counter, dMeter_Create clears it), dMeter_walletChange 025931CC moves it with mRupyIconB; TWW dMeter_walletChange sets its wallet colours */
    fopMsgM_pane_class mRupyIconB;  /* was field_0x2a58: pane 'rup2' on sMainParts2 (dMeter_screenDataSet in dMeter_Create 02595C6C): the second rupee icon pane; dMeter_rupyMove 0259C454 fades it and dMeter_walletChange 025931CC moves it together with mRupyIcon (same in TWW dMeter_rupyMove / dMeter_walletChange) */
    fopMsgM_pane_class field_0x2a90[9];
    fopMsgM_pane_class field_0x2c88;
    fopMsgM_pane_class field_0x2cc0;
    fopMsgM_pane_class field_0x2cf8;
    fopMsgM_pane_class field_0x2d30;
    fopMsgM_pane_class field_0x2d68;
    fopMsgM_pane_class field_0x2da0;
    fopMsgM_pane_class field_0x2dd8;
    fopMsgM_pane_class field_0x2e10[4];
    be<u32> field_0x2ef0;
    be<u32> field_0x2ef4;
    be<u32> field_0x2ef8;
    be<u32> field_0x2efc;
    be<u32> field_0x2f00;
    be<u32> field_0x2f04;
    be<u32> field_0x2f08;
    be<u32> field_0x2f0c;
    be<u32> field_0x2f10;
    be<u32> field_0x2f14;
    be<u32> field_0x2f18;
    be<u32> field_0x2f1c;
    be<u32> mpRupyParticle;
    be<u32> field_0x2f24[2];
    be<u32> field_0x2f2c[2];
    be<u32> arrowTex[2];
    be<u32> actionTex[3];
    be<u32> xyIconTex[3];
    be<u32> field_0x2f54[3];
    be<u32> moveIconTex[2];
    be<f32> field_0x2f68;
    be<f32> mCompassAlpha;  /* was field_0x2f6c: the compass alpha ratio: the alpha step inlined into dMeter_Execute 02593B10 (meterUpdateAlphaControl) fades it over dMeter_HIO_c mCompassFadeFrames when the 2D show flag changes and holds it at 0.5 / 0 while the menu window is open; dMeter_Create 02595C6C sets 1.0; TWW dMeter_alphaControl computes it the same way and dMeter_compassAlpha applies it to the compass and map panes */
    be<f32> field_0x2f70;
    be<f32> field_0x2f74[9];
    be<f32> field_0x2f98[9];
    be<f32> mHeartAlpha;  /* was field_0x2fbc: alpha ratio of the hearts: dMeter_Execute 02593B10 (inlined heart alpha step) sets valueIncrease(5, mHeartAlphaStep); dMeter_gaugeAlpha 0259C2BC applies it to mHeart, mHeartShadow and mHeartLight; dMeter_heartInit 02591B4C clears it; dMeter_Execute copies it to the HD state (MeterUpdateSharedState mHeartAlpha); same code TWW dMeter_heartAlpha / dMeter_gaugeAlpha */
    be<f32> mMagicAlpha;  /* was field_0x2fc0: alpha ratio of the magic gauge: dMeter_magicGaugeMove 0259B698 sets valueIncrease(5, mMagicGauge3.mUserArea), dMeter_flyGaugeMove 02592EE8 sets its own fade; dMeter_gaugeAlpha 0259C2BC applies it to mMagicBar, mMagicGauge3, mMagicGauge1, mMagicGaugeShadow1; dMeter_Create 02595C6C clears it; dMeter_Execute 02593B10 copies it to the HD state (MeterUpdateSharedState mMagicAlpha); same code TWW dMeter_magicAlpha / dMeter_gaugeAlpha */
    be<s16> field_0x2fc4;
    u8 before_field_0x2fd8[0x12];
    be<s16> mCompassFadeTimer;  /* was field_0x2fd8: the frame counter of the mCompassAlpha fade: the alpha step inlined into dMeter_Execute 02593B10 counts it down / up and passes it to valueIncrease(mCompassFadeFrames, ...); dMeter_Create 02595C6C starts it at mCompassFadeFrames; same code TWW dMeter_alphaControl / dMeter_Create */
    be<s16> mKeyLight2Frames[5];  /* was field_0x2fda: dMeter_keyMove 0259C7FC: the five dMeter_keyLight frame thresholds of mKeyLight2, rolled from the dMeter_HIO_c mKeyLight* values when mKeyLight2's counter is 1 */
    be<s16> field_0x2fe4;
    u8 before_field_0x2fe8[0x2];
    be<s16> field_0x2fe8;
    u8 before_field_0x2fec[0x2];
    be<s16> field_0x2fec;
    u8 before_field_0x2ff0[0x2];
    be<s16> field_0x2ff0;
    u8 before_mAdjustRupy[0xA];
    be<s32> mAdjustRupy;
    be<s32> field_0x3000;
    be<u32> mStatusFlags;
    be<u16> mRupyCount;
    be<s16> mAdjustHp;
    be<s16> field_0x300c;
    be<s16> mKeyNumPending;  /* was field_0x300e: dMeter_keyMove 0259C7FC sets it to the new saved key count minus mDispKeyNum when play.mItemKeyNumCount is applied, then counts it to 0 while mDispKeyNum follows */
    be<s16> field_0x3010;
    be<s16> field_0x3012;
    be<s16> mHeartAlphaStep;  /* was field_0x3014: the heart fade step 0..5: the heart alpha step inlined into dMeter_Execute 02593B10 (meterUpdateActionFade) moves it toward 0, 3 or 5 by the meter state and sets mHeartAlpha = valueIncrease(5, it); dMeter_heartInit 02591B4C sets 5; same code TWW dMeter_heartAlpha / dMeter_heartInit */
    be<u16> mDispMagic;  /* was field_0x3016: the shown magic, in 1/100 of the mMagicBar width: dMeter_Create 02595C6C sets it from the saved magic; dMeter_magicGaugeMove 0259B698 steps it by 625 toward magic * mMagicBar[0] width * 25 and passes it / 100 to dMeter_magicChange; same code TWW dMeter_magicInit / dMeter_magicGaugeMove */
    be<u8> mDispMaxMagic;  /* was field_0x3018: the shown magic capacity: dMeter_Create 02595C6C sets it from the saved mMaxMagic; dMeter_magicGaugeMove 0259B698 steps it by one per frame toward the saved mMaxMagic and calls dMeter_magicLength with it / 32; same code TWW dMeter_magicInit / dMeter_magicGaugeMove */
    be<u8> field_0x3019;
    be<u8> field_0x301a;
    be<u8> field_0x301b;
    be<u8> field_0x301c;
    be<u8> mDispKeyNum;  /* was field_0x301d: dMeter_Create 02595C6C stores dComIfGs_getKeyNum(); dMeter_keyMove 0259C7FC steps it by one per frame toward the saved key count (mKeyNumPending); the shown key count */
    be<u8> mMaxHP;
    be<u8> mCurrHP;
    be<u8> field_0x3020;
    be<u8> field_0x3021[3];
    be<u8> field_0x3024;
    be<s8> field_0x3025;
    be<u8> field_0x3026;
    be<u8> field_0x3027;
    be<u8> field_0x3028;
    be<u8> field_0x3029;
    be<u8> field_0x302a;
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
WWHD_OFFSET(sub_meter_class, field_0x1248, 0x124C);
WWHD_OFFSET(sub_meter_class, field_0x1280, 0x1284);
WWHD_OFFSET(sub_meter_class, field_0x12b8, 0x12BC);
WWHD_OFFSET(sub_meter_class, field_0x12f0, 0x12F4);
WWHD_OFFSET(sub_meter_class, field_0x1328, 0x132C);
WWHD_OFFSET(sub_meter_class, field_0x1360, 0x1364);
WWHD_OFFSET(sub_meter_class, field_0x1398, 0x139C);
WWHD_OFFSET(sub_meter_class, field_0x13d0, 0x13D4);
WWHD_OFFSET(sub_meter_class, field_0x1408, 0x140C);
WWHD_OFFSET(sub_meter_class, field_0x1440, 0x1444);
WWHD_OFFSET(sub_meter_class, field_0x1478, 0x147C);
WWHD_OFFSET(sub_meter_class, field_0x1520, 0x1524);
WWHD_OFFSET(sub_meter_class, field_0x1558, 0x155C);
WWHD_OFFSET(sub_meter_class, field_0x1590, 0x1594);
WWHD_OFFSET(sub_meter_class, field_0x1670, 0x1674);
WWHD_OFFSET(sub_meter_class, field_0x16a8, 0x16AC);
WWHD_OFFSET(sub_meter_class, field_0x16e0, 0x16E4);
WWHD_OFFSET(sub_meter_class, field_0x1718, 0x171C);
WWHD_OFFSET(sub_meter_class, field_0x17f8, 0x17FC);
WWHD_OFFSET(sub_meter_class, field_0x1830, 0x1834);
WWHD_OFFSET(sub_meter_class, field_0x1868, 0x186C);
WWHD_OFFSET(sub_meter_class, field_0x1910, 0x1914);
WWHD_OFFSET(sub_meter_class, field_0x1948, 0x194C);
WWHD_OFFSET(sub_meter_class, mKeyLight1, 0x1984);
WWHD_OFFSET(sub_meter_class, mKeyLight2, 0x19BC);
WWHD_OFFSET(sub_meter_class, mRupyDigitA, 0x19F4);
WWHD_OFFSET(sub_meter_class, mKeyDigitA, 0x1AD4);
WWHD_OFFSET(sub_meter_class, mRupyDigitB, 0x1B44);
WWHD_OFFSET(sub_meter_class, mKeyDigitB, 0x1C24);
WWHD_OFFSET(sub_meter_class, mKeyIcon, 0x1C94);
WWHD_OFFSET(sub_meter_class, field_0x1cc8, 0x1CCC);
WWHD_OFFSET(sub_meter_class, field_0x1d00, 0x1D04);
WWHD_OFFSET(sub_meter_class, field_0x1d38, 0x1D3C);
WWHD_OFFSET(sub_meter_class, field_0x1d70, 0x1D74);
WWHD_OFFSET(sub_meter_class, field_0x1da8, 0x1DAC);
WWHD_OFFSET(sub_meter_class, field_0x1e18, 0x1E1C);
WWHD_OFFSET(sub_meter_class, field_0x1e88, 0x1E8C);
WWHD_OFFSET(sub_meter_class, field_0x1ec0, 0x1EC4);
WWHD_OFFSET(sub_meter_class, field_0x1ef8, 0x1EFC);
WWHD_OFFSET(sub_meter_class, field_0x1f30, 0x1F34);
WWHD_OFFSET(sub_meter_class, field_0x1fd8, 0x1FDC);
WWHD_OFFSET(sub_meter_class, field_0x2080, 0x2084);
WWHD_OFFSET(sub_meter_class, field_0x2128, 0x212C);
WWHD_OFFSET(sub_meter_class, field_0x21d0, 0x21D4);
WWHD_OFFSET(sub_meter_class, field_0x2208, 0x220C);
WWHD_OFFSET(sub_meter_class, field_0x2240, 0x2244);
WWHD_OFFSET(sub_meter_class, field_0x2278, 0x227C);
WWHD_OFFSET(sub_meter_class, field_0x22b0, 0x22B4);
WWHD_OFFSET(sub_meter_class, field_0x22e8, 0x22EC);
WWHD_OFFSET(sub_meter_class, field_0x2320, 0x2324);
WWHD_OFFSET(sub_meter_class, field_0x23c8, 0x23CC);
WWHD_OFFSET(sub_meter_class, field_0x2470, 0x2474);
WWHD_OFFSET(sub_meter_class, field_0x2518, 0x251C);
WWHD_OFFSET(sub_meter_class, field_0x2550, 0x2554);
WWHD_OFFSET(sub_meter_class, field_0x25c0, 0x25C4);
WWHD_OFFSET(sub_meter_class, field_0x2630, 0x2634);
WWHD_OFFSET(sub_meter_class, field_0x26a0, 0x26A4);
WWHD_OFFSET(sub_meter_class, field_0x26d8, 0x26DC);
WWHD_OFFSET(sub_meter_class, field_0x2710, 0x2714);
WWHD_OFFSET(sub_meter_class, field_0x2748, 0x274C);
WWHD_OFFSET(sub_meter_class, field_0x2780, 0x2784);
WWHD_OFFSET(sub_meter_class, field_0x27b8, 0x27BC);
WWHD_OFFSET(sub_meter_class, field_0x27f0, 0x27F4);
WWHD_OFFSET(sub_meter_class, field_0x2828, 0x282C);
WWHD_OFFSET(sub_meter_class, field_0x2860, 0x2864);
WWHD_OFFSET(sub_meter_class, field_0x2898, 0x289C);
WWHD_OFFSET(sub_meter_class, field_0x28d0, 0x28D4);
WWHD_OFFSET(sub_meter_class, field_0x2908, 0x290C);
WWHD_OFFSET(sub_meter_class, field_0x2940, 0x2944);
WWHD_OFFSET(sub_meter_class, field_0x29b0, 0x29B4);
WWHD_OFFSET(sub_meter_class, mRupyIcon, 0x2A24);
WWHD_OFFSET(sub_meter_class, mRupyIconB, 0x2A5C);
WWHD_OFFSET(sub_meter_class, field_0x2a90, 0x2A94);
WWHD_OFFSET(sub_meter_class, field_0x2c88, 0x2C8C);
WWHD_OFFSET(sub_meter_class, field_0x2cc0, 0x2CC4);
WWHD_OFFSET(sub_meter_class, field_0x2cf8, 0x2CFC);
WWHD_OFFSET(sub_meter_class, field_0x2d30, 0x2D34);
WWHD_OFFSET(sub_meter_class, field_0x2d68, 0x2D6C);
WWHD_OFFSET(sub_meter_class, field_0x2da0, 0x2DA4);
WWHD_OFFSET(sub_meter_class, field_0x2dd8, 0x2DDC);
WWHD_OFFSET(sub_meter_class, field_0x2e10, 0x2E14);
WWHD_OFFSET(sub_meter_class, field_0x2ef0, 0x2EF4);
WWHD_OFFSET(sub_meter_class, field_0x2ef4, 0x2EF8);
WWHD_OFFSET(sub_meter_class, field_0x2ef8, 0x2EFC);
WWHD_OFFSET(sub_meter_class, field_0x2efc, 0x2F00);
WWHD_OFFSET(sub_meter_class, field_0x2f00, 0x2F04);
WWHD_OFFSET(sub_meter_class, field_0x2f04, 0x2F08);
WWHD_OFFSET(sub_meter_class, field_0x2f08, 0x2F0C);
WWHD_OFFSET(sub_meter_class, field_0x2f0c, 0x2F10);
WWHD_OFFSET(sub_meter_class, field_0x2f10, 0x2F14);
WWHD_OFFSET(sub_meter_class, field_0x2f14, 0x2F18);
WWHD_OFFSET(sub_meter_class, field_0x2f18, 0x2F1C);
WWHD_OFFSET(sub_meter_class, field_0x2f1c, 0x2F20);
WWHD_OFFSET(sub_meter_class, mpRupyParticle, 0x2F24);
WWHD_OFFSET(sub_meter_class, field_0x2f24, 0x2F28);
WWHD_OFFSET(sub_meter_class, field_0x2f2c, 0x2F30);
WWHD_OFFSET(sub_meter_class, arrowTex, 0x2F38);
WWHD_OFFSET(sub_meter_class, actionTex, 0x2F40);
WWHD_OFFSET(sub_meter_class, xyIconTex, 0x2F4C);
WWHD_OFFSET(sub_meter_class, field_0x2f54, 0x2F58);
WWHD_OFFSET(sub_meter_class, moveIconTex, 0x2F64);
WWHD_OFFSET(sub_meter_class, field_0x2f68, 0x2F6C);
WWHD_OFFSET(sub_meter_class, mCompassAlpha, 0x2F70);
WWHD_OFFSET(sub_meter_class, field_0x2f70, 0x2F74);
WWHD_OFFSET(sub_meter_class, field_0x2f74, 0x2F78);
WWHD_OFFSET(sub_meter_class, field_0x2f98, 0x2F9C);
WWHD_OFFSET(sub_meter_class, mHeartAlpha, 0x2FC0);
WWHD_OFFSET(sub_meter_class, mMagicAlpha, 0x2FC4);
WWHD_OFFSET(sub_meter_class, field_0x2fc4, 0x2FC8);
WWHD_OFFSET(sub_meter_class, mCompassFadeTimer, 0x2FDC);
WWHD_OFFSET(sub_meter_class, mKeyLight2Frames, 0x2FDE);
WWHD_OFFSET(sub_meter_class, field_0x2fe4, 0x2FE8);
WWHD_OFFSET(sub_meter_class, field_0x2fe8, 0x2FEC);
WWHD_OFFSET(sub_meter_class, field_0x2fec, 0x2FF0);
WWHD_OFFSET(sub_meter_class, field_0x2ff0, 0x2FF4);
WWHD_OFFSET(sub_meter_class, mAdjustRupy, 0x3000);
WWHD_OFFSET(sub_meter_class, field_0x3000, 0x3004);
WWHD_OFFSET(sub_meter_class, mStatusFlags, 0x3008);
WWHD_OFFSET(sub_meter_class, mRupyCount, 0x300C);
WWHD_OFFSET(sub_meter_class, mAdjustHp, 0x300E);
WWHD_OFFSET(sub_meter_class, field_0x300c, 0x3010);
WWHD_OFFSET(sub_meter_class, mKeyNumPending, 0x3012);
WWHD_OFFSET(sub_meter_class, field_0x3010, 0x3014);
WWHD_OFFSET(sub_meter_class, field_0x3012, 0x3016);
WWHD_OFFSET(sub_meter_class, mHeartAlphaStep, 0x3018);
WWHD_OFFSET(sub_meter_class, mDispMagic, 0x301A);
WWHD_OFFSET(sub_meter_class, mDispMaxMagic, 0x301C);
WWHD_OFFSET(sub_meter_class, field_0x3019, 0x301D);
WWHD_OFFSET(sub_meter_class, field_0x301a, 0x301E);
WWHD_OFFSET(sub_meter_class, field_0x301b, 0x301F);
WWHD_OFFSET(sub_meter_class, field_0x301c, 0x3020);
WWHD_OFFSET(sub_meter_class, mDispKeyNum, 0x3021);
WWHD_OFFSET(sub_meter_class, mMaxHP, 0x3022);
WWHD_OFFSET(sub_meter_class, mCurrHP, 0x3023);
WWHD_OFFSET(sub_meter_class, field_0x3020, 0x3024);
WWHD_OFFSET(sub_meter_class, field_0x3021, 0x3025);
WWHD_OFFSET(sub_meter_class, field_0x3024, 0x3028);
WWHD_OFFSET(sub_meter_class, field_0x3025, 0x3029);
WWHD_OFFSET(sub_meter_class, field_0x3026, 0x302A);
WWHD_OFFSET(sub_meter_class, field_0x3027, 0x302B);
WWHD_OFFSET(sub_meter_class, field_0x3028, 0x302C);
WWHD_OFFSET(sub_meter_class, field_0x3029, 0x302D);
WWHD_OFFSET(sub_meter_class, field_0x302a, 0x302E);
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

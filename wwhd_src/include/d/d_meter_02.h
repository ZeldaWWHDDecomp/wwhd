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
    fopMsgM_pane_class field_0x0f00;
    fopMsgM_pane_class field_0x0f38[8];
    fopMsgM_pane_class field_0x10f8;
    fopMsgM_pane_class field_0x1130;
    fopMsgM_pane_class field_0x1168;
    fopMsgM_pane_class field_0x11a0;
    fopMsgM_pane_class field_0x11d8;
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
    fopMsgM_pane_class field_0x1980;
    fopMsgM_pane_class field_0x19b8;
    fopMsgM_pane_class field_0x19f0[4];
    fopMsgM_pane_class field_0x1ad0[2];
    fopMsgM_pane_class field_0x1b40[4];
    fopMsgM_pane_class field_0x1c20[2];
    fopMsgM_pane_class field_0x1c90;
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
    fopMsgM_pane_class field_0x2a20;
    fopMsgM_pane_class field_0x2a58;
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
    be<f32> field_0x2f6c;
    be<f32> field_0x2f70;
    be<f32> field_0x2f74[9];
    be<f32> field_0x2f98[9];
    be<f32> field_0x2fbc;
    be<f32> field_0x2fc0;
    be<s16> field_0x2fc4;
    u8 before_field_0x2fd8[0x12];
    be<s16> field_0x2fd8;
    be<s16> field_0x2fda[5];
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
    be<s16> field_0x300e;
    be<s16> field_0x3010;
    be<s16> field_0x3012;
    be<s16> field_0x3014;
    be<u16> field_0x3016;
    be<u8> field_0x3018;
    be<u8> field_0x3019;
    be<u8> field_0x301a;
    be<u8> field_0x301b;
    be<u8> field_0x301c;
    be<u8> field_0x301d;
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
WWHD_OFFSET(sub_meter_class, field_0x0f00, 0xF04);
WWHD_OFFSET(sub_meter_class, field_0x0f38, 0xF3C);
WWHD_OFFSET(sub_meter_class, field_0x10f8, 0x10FC);
WWHD_OFFSET(sub_meter_class, field_0x1130, 0x1134);
WWHD_OFFSET(sub_meter_class, field_0x1168, 0x116C);
WWHD_OFFSET(sub_meter_class, field_0x11a0, 0x11A4);
WWHD_OFFSET(sub_meter_class, field_0x11d8, 0x11DC);
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
WWHD_OFFSET(sub_meter_class, field_0x1980, 0x1984);
WWHD_OFFSET(sub_meter_class, field_0x19b8, 0x19BC);
WWHD_OFFSET(sub_meter_class, field_0x19f0, 0x19F4);
WWHD_OFFSET(sub_meter_class, field_0x1ad0, 0x1AD4);
WWHD_OFFSET(sub_meter_class, field_0x1b40, 0x1B44);
WWHD_OFFSET(sub_meter_class, field_0x1c20, 0x1C24);
WWHD_OFFSET(sub_meter_class, field_0x1c90, 0x1C94);
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
WWHD_OFFSET(sub_meter_class, field_0x2a20, 0x2A24);
WWHD_OFFSET(sub_meter_class, field_0x2a58, 0x2A5C);
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
WWHD_OFFSET(sub_meter_class, field_0x2f6c, 0x2F70);
WWHD_OFFSET(sub_meter_class, field_0x2f70, 0x2F74);
WWHD_OFFSET(sub_meter_class, field_0x2f74, 0x2F78);
WWHD_OFFSET(sub_meter_class, field_0x2f98, 0x2F9C);
WWHD_OFFSET(sub_meter_class, field_0x2fbc, 0x2FC0);
WWHD_OFFSET(sub_meter_class, field_0x2fc0, 0x2FC4);
WWHD_OFFSET(sub_meter_class, field_0x2fc4, 0x2FC8);
WWHD_OFFSET(sub_meter_class, field_0x2fd8, 0x2FDC);
WWHD_OFFSET(sub_meter_class, field_0x2fda, 0x2FDE);
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
WWHD_OFFSET(sub_meter_class, field_0x300e, 0x3012);
WWHD_OFFSET(sub_meter_class, field_0x3010, 0x3014);
WWHD_OFFSET(sub_meter_class, field_0x3012, 0x3016);
WWHD_OFFSET(sub_meter_class, field_0x3014, 0x3018);
WWHD_OFFSET(sub_meter_class, field_0x3016, 0x301A);
WWHD_OFFSET(sub_meter_class, field_0x3018, 0x301C);
WWHD_OFFSET(sub_meter_class, field_0x3019, 0x301D);
WWHD_OFFSET(sub_meter_class, field_0x301a, 0x301E);
WWHD_OFFSET(sub_meter_class, field_0x301b, 0x301F);
WWHD_OFFSET(sub_meter_class, field_0x301c, 0x3020);
WWHD_OFFSET(sub_meter_class, field_0x301d, 0x3021);
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
    be<f32> field_0xc;
    u8 before_62[0x52];
    be<u8> field_0x62;
    be<u8> field_0x63;
    u8 before_88[0x24];
    // Contiguous tww fields 0x88/0x8a/0x8c and 0x92/0x94/0x96 are indexed as groups.
    be<s16> field_0x88[3];
    be<s16> field_0x8e;
    be<s16> field_0x90;
    be<s16> field_0x92[3];
    be<s16> field_0x98;
    be<s16> field_0x9a;
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

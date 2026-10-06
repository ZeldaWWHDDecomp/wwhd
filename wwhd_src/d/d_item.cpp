/* d_item: item get functions (execItemGet, checkItemGet, item_func_*, item_getcheck_func_*,
 * the item classification helpers and the life-ball table lookups), WWHD.
 *
 * Translation unit 0254DA38..02551B8F (395 functions), from the image: d_grass ends with its
 * __sinit (0254D9A4); d_item starts with execItemGet (0254DA38) and follows the GameCube order
 * up to getItemFromLifeBallTableWithoutEmono (02551684), then its __sinit (0255198C), then
 * GHS's grouped empty functions (30 empty item_func_*, 02551A50..02551AC4) and the 25
 * `return -1` item_getcheck_func_* (02551AC8..02551B88). d_item_data follows (02551B90).
 * The identities come from the two function tables in .data: item_func_ptr[0x100] at
 * 101E3A98 and item_getcheck_func_ptr[0x100] at 101E3E98 (GameCube order of the slots).
 * HD adds two items: slot 0x14 (item_func 0254DF64, getcheck -1) and slot 0x77 (swift sail:
 * item_func 0254F5D0, getcheck 02550B5C); the GameCube slots are item_func_noentry.
 *
 * Parts: d_item.cpp (execItemGet .. item_func_bomb_bag), d_item_2.cpp (item_func_hummer ..
 * item_func_collectmap62), d_item_3.cpp (the HD collect-map helper and item_func_collectmap61..01),
 * d_item_4.cpp (item_getcheck_func_*), d_item_5.cpp (getRotenItemNumInBag .. the life-ball
 * tables, __sinit, the grouped empty functions and the `return -1` getchecks).
 *
 * Layout (HD, from the code): play object counters at +0x5B44.. (see d_item_local.h), select
 * items [5] at +0x5BBB; save object (*101F84DC): select items [5] at +0x29 (buttons 3 and 4
 * are HD-only: the wind waker and the sail are put on them when obtained), inventory at +0x5C,
 * get-item flags +0x71, bag items +0x96, get-bag flags +0xB0, collect +0xD4, map +0xE4,
 * events +0x644, stage memory bits +0x798, HD statistics block +0x12C0. */
#include "bindings.h"

namespace d_item_cpp {
#include "d_item_local.h"

/* 0254DA38: item_func_ptr[itemNo]() (tail call; itemNo is not masked) */
void execItemGet(u32 itemNo) {
    WWHD_FUNC(0x0254DA38, void, itemNo);
    gabi::call_ptr(ld(0x101E3A98 + (itemNo << 2)));
}
VERIFY(0x0254DA38, execItemGet);

/* 0254DA50 */
s32 checkItemGet(u32 itemNo, s32 defaultVal) {
    WWHD_FUNC(0x0254DA50, s32, itemNo, defaultVal);
    s32 result = gabi::call_ptr<s32>(ld(0x101E3E98 + (itemNo << 2)));
    if (result == -1) return defaultVal;
    return result;
}
VERIFY(0x0254DA50, checkItemGet);

/* 0254DA9C */
void item_func_heart() {
    WWHD_FUNC(0x0254DA9C, void);
    dComIfGp_setItemLifeCount(4.0f);
}
VERIFY(0x0254DA9C, item_func_heart);

/* 0254DAD0 */
void item_func_green_rupee() {
    WWHD_FUNC(0x0254DAD0, void);
    dComIfGp_setItemRupeeCount(1);
}
VERIFY(0x0254DAD0, item_func_green_rupee);

/* 0254DAFC */
void item_func_blue_rupee() {
    WWHD_FUNC(0x0254DAFC, void);
    dComIfGp_setItemRupeeCount(5);
}
VERIFY(0x0254DAFC, item_func_blue_rupee);

/* 0254DB28 */
void item_func_white_rupee() {
    WWHD_FUNC(0x0254DB28, void);
    dComIfGp_setItemRupeeCount(10);
}
VERIFY(0x0254DB28, item_func_white_rupee);

/* 0254DB54 */
void item_func_red_rupee() {
    WWHD_FUNC(0x0254DB54, void);
    dComIfGp_setItemRupeeCount(20);
}
VERIFY(0x0254DB54, item_func_red_rupee);

/* 0254DB80 */
void item_func_purple_rupee() {
    WWHD_FUNC(0x0254DB80, void);
    dComIfGp_setItemRupeeCount(50);
}
VERIFY(0x0254DB80, item_func_purple_rupee);

/* 0254DBAC */
void item_func_orange_rupee() {
    WWHD_FUNC(0x0254DBAC, void);
    dComIfGp_setItemRupeeCount(100);
}
VERIFY(0x0254DBAC, item_func_orange_rupee);

/* 0254DBD8: HD also counts the heart piece in the HD statistics block */
void item_func_kakera_heart() {
    WWHD_FUNC(0x0254DBD8, void);
    dComIfGp_setItemMaxLifeCount(1);
    dSv_hdstats_add1C_l(dSv_hdstats_get_l(svbase() + SV_HDSTATS), 1);
}
VERIFY(0x0254DBD8, item_func_kakera_heart);

/* 0254DC1C */
void item_func_utuwa_heart() {
    WWHD_FUNC(0x0254DC1C, void);
    dComIfGp_setItemMaxLifeCount(4);
    f32 maxLife = (f32)ld16(svbase() + SV_MAXLIFE);
    dComIfGp_setItemLifeCount(maxLife);

    /* dComIfGp_getStageStagInfo(): virtual getStagInfo (vtable +0x15C) of the stage data */
    u32 stage = play() + PL_STAGE;
    u32 stag_info = gabi::call_ptr<u32>(ld(ld(stage) + 0x15C), stage);
    if (((ld8(stag_info + 9) >> 1) & 0x7F) == 0 /* dSv_save_c::STAGE_SEA */) {
        // Didn't get the Heart Container immediately after defeating Helmaroc King.
        // Instead got it from outside Forsaken Fortress, on the sea stage.
        dComIfGs_onStageLife_l(2 /* dSv_save_c::STAGE_FF */);
    } else {
        dSv_memBit_onDungeonItem_l(svbase() + SV_MEMBIT, 4 /* STAGE_LIFE */);
    }
}
VERIFY(0x0254DC1C, item_func_utuwa_heart);

/* 0254DCFC */
void item_func_s_magic() {
    WWHD_FUNC(0x0254DCFC, void);
    dComIfGp_setItemMagicCount(4);
}
VERIFY(0x0254DCFC, item_func_s_magic);

/* 0254DD28 */
void item_func_l_magic() {
    WWHD_FUNC(0x0254DD28, void);
    dComIfGp_setItemMagicCount(8);
}
VERIFY(0x0254DD28, item_func_l_magic);

/* 0254DD54 */
void item_func_bomb_5() {
    WWHD_FUNC(0x0254DD54, void);
    dComIfGs_onGetItem(0xD /* dInvSlot_BOMB_e */, 0);
    u32 p = play(); /* dComIfGp_setItem(dInvSlot_BOMB_e, dItemNo_BOMB_5_e) */
    st8(p + PL_ITEMSLOT, 0xD);
    st8(p + PL_ITEMNO, 0xB);
    dComIfGp_setItemBombNumCount(5);
}
VERIFY(0x0254DD54, item_func_bomb_5);

/* 0254DDAC */
void item_func_bomb_10() {
    WWHD_FUNC(0x0254DDAC, void);
    dComIfGs_onGetItem(0xD, 0);
    dComIfGs_setItem(0xD, 0x31 /* dItemNo_BOMB_BAG_e */);
    dComIfGp_setItemBombNumCount(10);
}
VERIFY(0x0254DDAC, item_func_bomb_10);

/* 0254DE04 */
void item_func_bomb_20() {
    WWHD_FUNC(0x0254DE04, void);
    dComIfGs_onGetItem(0xD, 0);
    dComIfGs_setItem(0xD, 0x31);
    dComIfGp_setItemBombNumCount(20);
}
VERIFY(0x0254DE04, item_func_bomb_20);

/* 0254DE5C */
void item_func_bomb_30() {
    WWHD_FUNC(0x0254DE5C, void);
    dComIfGs_onGetItem(0xD, 0);
    dComIfGs_setItem(0xD, 0x31);
    dComIfGp_setItemBombNumCount(30);
}
VERIFY(0x0254DE5C, item_func_bomb_30);

/* 0254DEB4 */
void item_func_silver_rupee() {
    WWHD_FUNC(0x0254DEB4, void);
    dComIfGp_setItemRupeeCount(200);
}
VERIFY(0x0254DEB4, item_func_silver_rupee);

/* 0254DEE0 */
void item_func_arrow_10() {
    WWHD_FUNC(0x0254DEE0, void);
    dComIfGp_setItemArrowNumCount(10);
}
VERIFY(0x0254DEE0, item_func_arrow_10);

/* 0254DF0C */
void item_func_arrow_20() {
    WWHD_FUNC(0x0254DF0C, void);
    dComIfGp_setItemArrowNumCount(20);
}
VERIFY(0x0254DF0C, item_func_arrow_20);

/* 0254DF38 */
void item_func_arrow_30() {
    WWHD_FUNC(0x0254DF38, void);
    dComIfGp_setItemArrowNumCount(30);
}
VERIFY(0x0254DF38, item_func_arrow_30);

/* 0254DF64: HD-only item 0x14 (GameCube: noentry). It runs 0203A4B8 on the object at
 * *1018F4AC (an HD system with ten 0x3EC-byte message slots; probably the Tingle Bottle) and
 * counts the event in the HD statistics block. */
void item_func_hd_14() {
    WWHD_FUNC(0x0254DF64, void);
    gabi::call(0x0203A4B8, ld(0x1018F4AC));
    dSv_hdstats_add14_l(dSv_hdstats_get_l(svbase() + SV_HDSTATS), 1);
}
VERIFY(0x0254DF64, item_func_hd_14);

/* 0254DFA4 */
void item_func_small_key() {
    WWHD_FUNC(0x0254DFA4, void);
    dComIfGp_setItemKeyNumCount(1);
}
VERIFY(0x0254DFA4, item_func_small_key);

/* 0254DFD0 */
void item_func_recover_faily() {
    WWHD_FUNC(0x0254DFD0, void);
    dComIfGp_setItemLifeCount(40.0f);
}
VERIFY(0x0254DFD0, item_func_recover_faily);

/* 0254E004 */
void item_func_subdun_rupee() {
    WWHD_FUNC(0x0254E004, void);
    dComIfGp_setItemRupeeCount(10);
}
VERIFY(0x0254E004, item_func_subdun_rupee);

/* 0254E030 */
void item_func_pendant() {
    WWHD_FUNC(0x0254E030, void);
    dSv_bag_setBeastItem_l(svbase() + SV_BAGITEM, 0x1F /* dItemNo_JOY_PENDANT_e */);
    dSv_getbag_onBeast_l(svbase() + SV_GETBAG, 7 /* dBeastIdx_JOY_PENDANT_e */);
    dComIfGp_setItemBeastNumCount(7, 1);
}
VERIFY(0x0254E030, item_func_pendant);

/* 0254E088 */
void item_func_telescope() {
    WWHD_FUNC(0x0254E088, void);
    dComIfGs_onGetItem(0x0 /* dInvSlot_TELESCOPE_e */, 0);
    dComIfGs_setItem(0x0, 0x20 /* dItemNo_TELESCOPE_e */);
}
VERIFY(0x0254E088, item_func_telescope);

/* 0254E0D0 */
void item_func_tncl_whitsl() {
    WWHD_FUNC(0x0254E0D0, void);
    dComIfGs_onGetItem(0x7 /* dInvSlot_TINGLE_TUNER_e */, 0);
    dComIfGs_setItem(0x7, 0x21 /* dItemNo_TINGLE_TUNER_e */);
}
VERIFY(0x0254E0D0, item_func_tncl_whitsl);

/* 0254E118: HD puts the Wind Waker on the HD-only item button 3 */
void item_func_wind_tact() {
    WWHD_FUNC(0x0254E118, void);
    dComIfGs_onGetItem(0x2 /* dInvSlot_WIND_WAKER_e */, 0);
    dComIfGs_setItem(0x2, 0x22 /* dItemNo_WIND_WAKER_e */);
    st8(svbase() + SV_SELITEM + 3, 0x2);
    dComIfGp_setSelectItem(3);
}
VERIFY(0x0254E118, item_func_wind_tact);

/* 0254E318 */
void item_func_camera() {
    WWHD_FUNC(0x0254E318, void);
    dComIfGs_onGetItem(0x8 /* dInvSlot_CAMERA_e */, 0);
    dComIfGs_setItem(0x8, 0x23 /* dItemNo_PICTO_BOX_e */);
}
VERIFY(0x0254E318, item_func_camera);

/* 0254E360 */
void item_func_emono_bag() {
    WWHD_FUNC(0x0254E360, void);
    dComIfGs_onGetItem(0x4 /* dInvSlot_SPOILS_BAG_e */, 0);
    dComIfGs_setItem(0x4, 0x24 /* dItemNo_SPOILS_BAG_e */);
}
VERIFY(0x0254E360, item_func_emono_bag);

/* 0254E3A8 */
void item_func_rope() {
    WWHD_FUNC(0x0254E3A8, void);
    dComIfGs_onGetItem(0x3 /* dInvSlot_GRAPPLING_HOOK_e */, 0);
    dComIfGs_setItem(0x3, 0x25 /* dItemNo_GRAPPLING_HOOK_e */);
}
VERIFY(0x0254E3A8, item_func_rope);

/* 0254E3F0: HD (like the GameCube USA/PAL arrows) refreshes the X/Y/Z buttons that showed the
 * old Picto Box */
void item_func_camera2() {
    WWHD_FUNC(0x0254E3F0, void);
    dComIfGs_onGetItem(0x8, 1);
    dComIfGs_setItem(0x8, 0x26 /* dItemNo_DELUXE_PICTO_BOX_e */);
    refreshSelectItem_inl(0x23 /* dItemNo_PICTO_BOX_e */);
}
VERIFY(0x0254E3F0, item_func_camera2);

/* 0254E630 */
void item_func_bow() {
    WWHD_FUNC(0x0254E630, void);
    dComIfGs_onGetItem(0xC /* dInvSlot_BOW_e */, 0);
    dComIfGs_setItem(0xC, 0x27 /* dItemNo_BOW_e */);
    st8(svbase() + SV_ARROWNUM, 30); /* dComIfGs_setArrowNum(30) */
    st8(svbase() + SV_ARROWMAX, 30); /* dComIfGs_setArrowMax(30) */
}
VERIFY(0x0254E630, item_func_bow);

/* 0254E68C */
void item_func_pwr_groove() {
    WWHD_FUNC(0x0254E68C, void);
    dComIfGs_onCollect(0x2, 0);
    dComIfGs_setSelectEquip_l(0x2, 0x28 /* dItemNo_POWER_BRACELETS_e */);
}
VERIFY(0x0254E68C, item_func_pwr_groove);

/* 0254E6CC */
void item_func_hvy_boots() {
    WWHD_FUNC(0x0254E6CC, void);
    dComIfGs_onGetItem(0x9 /* dInvSlot_IRON_BOOTS_e */, 0);
    dComIfGs_setItem(0x9, 0x29 /* dItemNo_IRON_BOOTS_e */);
}
VERIFY(0x0254E6CC, item_func_hvy_boots);

/* 0254E714 */
void item_func_drgn_shield() {
    WWHD_FUNC(0x0254E714, void);
    dComIfGs_onGetItem(0xA /* dInvSlot_MAGIC_ARMOR_e */, 0);
    dComIfGs_setItem(0xA, 0x2A /* dItemNo_MAGIC_ARMOR_e */);
}
VERIFY(0x0254E714, item_func_drgn_shield);

/* 0254E75C */
void item_func_esa_bag() {
    WWHD_FUNC(0x0254E75C, void);
    dComIfGs_onGetItem(0xB /* dInvSlot_BAIT_BAG_e */, 0);
    dComIfGs_setItem(0xB, 0x2C /* dItemNo_BAIT_BAG_e */);
}
VERIFY(0x0254E75C, item_func_esa_bag);

/* 0254E7A4 */
void item_func_boomerang() {
    WWHD_FUNC(0x0254E7A4, void);
    dComIfGs_onGetItem(0x5 /* dInvSlot_BOOMERANG_e */, 0);
    dComIfGs_setItem(0x5, 0x2D /* dItemNo_BOOMERANG_e */);
}
VERIFY(0x0254E7A4, item_func_boomerang);

/* 0254E7EC */
void item_func_bare_hand() {
    WWHD_FUNC(0x0254E7EC, void);
    dComIfGs_setSelectEquip_l(0x02, 0x2E /* dItemNo_BARE_HAND_e */);
}
VERIFY(0x0254E7EC, item_func_bare_hand);

/* 0254E7F8 */
void item_func_hookshot() {
    WWHD_FUNC(0x0254E7F8, void);
    dComIfGs_onGetItem(0x13 /* dInvSlot_HOOKSHOT_e */, 0);
    dComIfGs_setItem(0x13, 0x2F /* dItemNo_HOOKSHOT_e */);
}
VERIFY(0x0254E7F8, item_func_hookshot);

/* 0254E840 */
void item_func_warasibe_bag() {
    WWHD_FUNC(0x0254E840, void);
    dComIfGs_onGetItem(0x12 /* dInvSlot_DELIVERY_BAG_e */, 0);
    dComIfGs_setItem(0x12, 0x30 /* dItemNo_DELIVERY_BAG_e */);
}
VERIFY(0x0254E840, item_func_warasibe_bag);

/* 0254E888 */
void item_func_bomb_bag() {
    WWHD_FUNC(0x0254E888, void);
    dComIfGs_onGetItem(0xD /* dInvSlot_BOMB_e */, 0);
    dComIfGs_setItem(0xD, 0x31 /* dItemNo_BOMB_BAG_e */);
    st8(svbase() + SV_BOMBNUM, 30); /* dComIfGs_setBombNum(30) */
    st8(svbase() + SV_BOMBMAX, 30); /* dComIfGs_setBombMax(30) */
}
VERIFY(0x0254E888, item_func_bomb_bag);

}  // namespace d_item_cpp

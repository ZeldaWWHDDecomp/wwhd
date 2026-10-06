/* d_item part 2: item_func_hummer .. item_func_collectmap62 (0254E8E4..0255030F). WWHD.
 * See d_item.cpp for the unit's range and layout. */
#include "bindings.h"

namespace d_item_2_cpp {
#include "d_item_local.h"

/* 0254E8E4 */
void item_func_hummer() {
    WWHD_FUNC(0x0254E8E4, void);
    dComIfGs_onGetItem(0x14 /* dInvSlot_SKULL_HAMMER_e */, 0);
    dComIfGs_setItem(0x14, 0x33 /* dItemNo_SKULL_HAMMER_e */);
}
VERIFY(0x0254E8E4, item_func_hummer);

/* 0254E92C */
void item_func_deku_leaf() {
    WWHD_FUNC(0x0254E92C, void);
    dComIfGs_onGetItem(0x6 /* dInvSlot_DEKU_LEAF_e */, 0);
    dComIfGs_setItem(0x6, 0x34 /* dItemNo_DEKU_LEAF_e */);
    dComIfGp_setItemMaxMagicCount(16);
    dComIfGp_setItemMagicCount(16);
}
VERIFY(0x0254E92C, item_func_deku_leaf);

/* 0254E994 */
void item_func_magic_arrow() {
    WWHD_FUNC(0x0254E994, void);
    dComIfGs_onGetItem(0xC /* dInvSlot_BOW_e */, 1);
    dComIfGs_setItem(0xC, 0x35 /* dItemNo_MAGIC_ARROW_e */);
    // If the regular bow was equipped on an X/Y/Z button, force it to update.
    refreshSelectItem_inl(0x27 /* dItemNo_BOW_e */);
}
VERIFY(0x0254E994, item_func_magic_arrow);

/* 0254EBD4 */
void item_func_light_arrow() {
    WWHD_FUNC(0x0254EBD4, void);
    dComIfGs_onGetItem(0xC /* dInvSlot_BOW_e */, 2);
    dComIfGs_setItem(0xC, 0x36 /* dItemNo_LIGHT_ARROW_e */);
    // If the fire/ice bow was equipped on an X/Y/Z button, force it to update.
    refreshSelectItem_inl(0x35 /* dItemNo_MAGIC_ARROW_e */);
}
VERIFY(0x0254EBD4, item_func_light_arrow);

/* 0254EE14 */
void item_func_sword() {
    WWHD_FUNC(0x0254EE14, void);
    dComIfGs_onCollect(0x0, 0);
    dComIfGs_setSelectEquip_l(0x0, 0x38 /* dItemNo_SWORD_e */);
}
VERIFY(0x0254EE14, item_func_sword);

/* 0254EE54 */
void item_func_master_sword() {
    WWHD_FUNC(0x0254EE54, void);
    dComIfGs_onCollect(0x0, 1);
    dComIfGs_setSelectEquip_l(0x0, 0x39 /* dItemNo_MASTER_SWORD_1_e */);
}
VERIFY(0x0254EE54, item_func_master_sword);

/* 0254EE94 */
void item_func_lv3_sword() {
    WWHD_FUNC(0x0254EE94, void);
    dComIfGs_onCollect(0x0, 2);
    dComIfGs_setSelectEquip_l(0x0, 0x3A /* dItemNo_MASTER_SWORD_2_e */);
}
VERIFY(0x0254EE94, item_func_lv3_sword);

/* 0254EED4 */
void item_func_shield() {
    WWHD_FUNC(0x0254EED4, void);
    dComIfGs_onCollect(0x1, 0);
    dComIfGs_setSelectEquip_l(0x1, 0x3B /* dItemNo_SHIELD_e */);
}
VERIFY(0x0254EED4, item_func_shield);

/* 0254EF14 */
void item_func_mirror_shield() {
    WWHD_FUNC(0x0254EF14, void);
    dComIfGs_onCollect(0x1, 1);
    dComIfGs_setSelectEquip_l(0x1, 0x3C /* dItemNo_MIRROR_SHIELD_e */);
}
VERIFY(0x0254EF14, item_func_mirror_shield);

/* 0254EF54 */
void item_func_dropped_sword() {
    WWHD_FUNC(0x0254EF54, void);
    dComIfGs_onCollect(0x0, 0);
    dComIfGs_setSelectEquip_l(0x0, 0x38 /* dItemNo_SWORD_e */);
}
VERIFY(0x0254EF54, item_func_dropped_sword);

/* 0254EF94 */
void item_func_master_sword_ex() {
    WWHD_FUNC(0x0254EF94, void);
    dComIfGs_onCollect(0x0, 3);
    dComIfGs_setSelectEquip_l(0x0, 0x3E /* dItemNo_MASTER_SWORD_3_e */);
}
VERIFY(0x0254EF94, item_func_master_sword_ex);

/* 0254EFD4 */
void item_func_pirates_omamori() {
    WWHD_FUNC(0x0254EFD4, void);
    dComIfGs_onCollect(0x3, 0);
}
VERIFY(0x0254EFD4, item_func_pirates_omamori);

/* 0254EFEC */
void item_func_heros_omamori() {
    WWHD_FUNC(0x0254EFEC, void);
    dComIfGs_onCollect(0x4, 0);
}
VERIFY(0x0254EFEC, item_func_heros_omamori);

/* 0254F004 */
void item_func_skull_necklace() {
    WWHD_FUNC(0x0254F004, void);
    dSv_bag_setBeastItem_l(svbase() + SV_BAGITEM, 0x45 /* dItemNo_SKULL_NECKLACE_e */);
    dSv_getbag_onBeast_l(svbase() + SV_GETBAG, 0 /* dBeastIdx_SKULL_NECKLACE_e */);
    dComIfGp_setItemBeastNumCount(0, 1);
}
VERIFY(0x0254F004, item_func_skull_necklace);

/* 0254F05C */
void item_func_bokobaba_seed() {
    WWHD_FUNC(0x0254F05C, void);
    dSv_bag_setBeastItem_l(svbase() + SV_BAGITEM, 0x46 /* dItemNo_BOKOBABA_SEED_e */);
    dSv_getbag_onBeast_l(svbase() + SV_GETBAG, 1 /* dBeastIdx_BOKOBABA_SEED_e */);
    dComIfGp_setItemBeastNumCount(1, 1);
}
VERIFY(0x0254F05C, item_func_bokobaba_seed);

/* 0254F0B4 */
void item_func_golden_feather() {
    WWHD_FUNC(0x0254F0B4, void);
    dSv_bag_setBeastItem_l(svbase() + SV_BAGITEM, 0x47 /* dItemNo_GOLDEN_FEATHER_e */);
    dSv_getbag_onBeast_l(svbase() + SV_GETBAG, 2 /* dBeastIdx_GOLDEN_FEATHER_e */);
    dComIfGp_setItemBeastNumCount(2, 1);
}
VERIFY(0x0254F0B4, item_func_golden_feather);

/* 0254F10C */
void item_func_boko_belt() {
    WWHD_FUNC(0x0254F10C, void);
    dSv_bag_setBeastItem_l(svbase() + SV_BAGITEM, 0x48 /* dItemNo_KNIGHTS_CREST_e */);
    dSv_getbag_onBeast_l(svbase() + SV_GETBAG, 3 /* dBeastIdx_KNIGHTS_CREST_e */);
    dComIfGp_setItemBeastNumCount(3, 1);
}
VERIFY(0x0254F10C, item_func_boko_belt);

/* 0254F164 */
void item_func_red_jerry() {
    WWHD_FUNC(0x0254F164, void);
    dSv_bag_setBeastItem_l(svbase() + SV_BAGITEM, 0x49 /* dItemNo_RED_JELLY_e */);
    dSv_getbag_onBeast_l(svbase() + SV_GETBAG, 4 /* dBeastIdx_RED_JELLY_e */);
    dComIfGp_setItemBeastNumCount(4, 1);
}
VERIFY(0x0254F164, item_func_red_jerry);

/* 0254F1BC */
void item_func_green_jerry() {
    WWHD_FUNC(0x0254F1BC, void);
    dSv_bag_setBeastItem_l(svbase() + SV_BAGITEM, 0x4A /* dItemNo_GREEN_JELLY_e */);
    dSv_getbag_onBeast_l(svbase() + SV_GETBAG, 5 /* dBeastIdx_GREEN_JELLY_e */);
    dComIfGp_setItemBeastNumCount(5, 1);
}
VERIFY(0x0254F1BC, item_func_green_jerry);

/* 0254F214 */
void item_func_blue_jerry() {
    WWHD_FUNC(0x0254F214, void);
    dSv_bag_setBeastItem_l(svbase() + SV_BAGITEM, 0x4B /* dItemNo_BLUE_JELLY_e */);
    dSv_getbag_onBeast_l(svbase() + SV_GETBAG, 6 /* dBeastIdx_BLUE_JELLY_e */);
    dComIfGp_setItemBeastNumCount(6, 1);
}
VERIFY(0x0254F214, item_func_blue_jerry);

/* 0254F26C: HD also sets a flag at *101F8344 + 0x228 */
void item_func_map() {
    WWHD_FUNC(0x0254F26C, void);
    dSv_memBit_onDungeonItem_l(svbase() + SV_MEMBIT, 0 /* MAP */);
    st(ld(0x101F8344) + 0x228, 1); /* HD: flag in the object at *101F8344 (probably the map menu: new map) */
}
VERIFY(0x0254F26C, item_func_map);

/* 0254F2AC */
void item_func_compass() {
    WWHD_FUNC(0x0254F2AC, void);
    dSv_memBit_onDungeonItem_l(svbase() + SV_MEMBIT, 1 /* COMPASS */);
}
VERIFY(0x0254F2AC, item_func_compass);

/* 0254F2C0 */
void item_func_boss_key() {
    WWHD_FUNC(0x0254F2C0, void);
    dSv_memBit_onDungeonItem_l(svbase() + SV_MEMBIT, 2 /* BOSS_KEY */);
}
VERIFY(0x0254F2C0, item_func_boss_key);

/* 0254F2D4 */
void item_func_empty_bship() {
    WWHD_FUNC(0x0254F2D4, void);
    dSv_get_item_onBottleItem_l(svbase() + SV_GETITEM, 0x4F /* dItemNo_EMPTY_BSHIP_e */);
}
VERIFY(0x0254F2D4, item_func_empty_bship);

/* 0254F2E8 */
void item_func_empty_bottle() {
    WWHD_FUNC(0x0254F2E8, void);
    dSv_item_setEmptyBottle_l(svbase() + SV_ITEM);
    dSv_get_item_onBottleItem_l(svbase() + SV_GETITEM, 0x50 /* dItemNo_EMPTY_BOTTLE_e */);
}
VERIFY(0x0254F2E8, item_func_empty_bottle);

/* 0254F32C */
void item_func_red_bottle() {
    WWHD_FUNC(0x0254F32C, void);
    dSv_item_setEmptyBottleItemIn_l(svbase() + SV_ITEM, 0x51 /* dItemNo_RED_POTION_e */);
    dSv_get_item_onBottleItem_l(svbase() + SV_GETITEM, 0x51);
}
VERIFY(0x0254F32C, item_func_red_bottle);

/* 0254F374 */
void item_func_green_bottle() {
    WWHD_FUNC(0x0254F374, void);
    dSv_item_setEmptyBottleItemIn_l(svbase() + SV_ITEM, 0x52 /* dItemNo_GREEN_POTION_e */);
    dSv_get_item_onBottleItem_l(svbase() + SV_GETITEM, 0x52);
}
VERIFY(0x0254F374, item_func_green_bottle);

/* 0254F3BC */
void item_func_blue_bottle() {
    WWHD_FUNC(0x0254F3BC, void);
    dSv_item_setEmptyBottleItemIn_l(svbase() + SV_ITEM, 0x53 /* dItemNo_BLUE_POTION_e */);
    dSv_get_item_onBottleItem_l(svbase() + SV_GETITEM, 0x53);
}
VERIFY(0x0254F3BC, item_func_blue_bottle);

/* 0254F404 */
void item_func_bottleship() {
    WWHD_FUNC(0x0254F404, void);
    dSv_get_item_onBottleItem_l(svbase() + SV_GETITEM, 0x54 /* dItemNo_HALF_SOUP_BOTTLE_e */);
}
VERIFY(0x0254F404, item_func_bottleship);

/* 0254F418 */
void item_func_soup_bottle() {
    WWHD_FUNC(0x0254F418, void);
    dSv_get_item_onBottleItem_l(svbase() + SV_GETITEM, 0x55 /* dItemNo_SOUP_BOTTLE_e */);
}
VERIFY(0x0254F418, item_func_soup_bottle);

/* 0254F42C */
void item_func_bin_in_water() {
    WWHD_FUNC(0x0254F42C, void);
    dSv_get_item_onBottleItem_l(svbase() + SV_GETITEM, 0x56 /* dItemNo_WATER_BOTTLE_e */);
}
VERIFY(0x0254F42C, item_func_bin_in_water);

/* 0254F440 */
void item_func_fairy_bottle() {
    WWHD_FUNC(0x0254F440, void);
    dSv_get_item_onBottleItem_l(svbase() + SV_GETITEM, 0x57 /* dItemNo_FAIRY_BOTTLE_e */);
}
VERIFY(0x0254F440, item_func_fairy_bottle);

/* 0254F454 */
void item_func_firefly_bottle() {
    WWHD_FUNC(0x0254F454, void);
    dSv_get_item_onBottleItem_l(svbase() + SV_GETITEM, 0x58 /* dItemNo_FIREFLY_BOTTLE_e */);
}
VERIFY(0x0254F454, item_func_firefly_bottle);

/* 0254F468 */
void item_func_fwater_bottle() {
    WWHD_FUNC(0x0254F468, void);
    dSv_get_item_onBottleItem_l(svbase() + SV_GETITEM, 0x59 /* dItemNo_FOREST_WATER_e */);
}
VERIFY(0x0254F468, item_func_fwater_bottle);

/* 0254F47C */
void item_func_triforce1() {
    WWHD_FUNC(0x0254F47C, void);
    dSv_collect_onTriforce_l(svbase() + SV_COLLECT, 0);
}
VERIFY(0x0254F47C, item_func_triforce1);

/* 0254F490 */
void item_func_triforce2() {
    WWHD_FUNC(0x0254F490, void);
    dSv_collect_onTriforce_l(svbase() + SV_COLLECT, 1);
}
VERIFY(0x0254F490, item_func_triforce2);

/* 0254F4A4 */
void item_func_triforce3() {
    WWHD_FUNC(0x0254F4A4, void);
    dSv_collect_onTriforce_l(svbase() + SV_COLLECT, 2);
}
VERIFY(0x0254F4A4, item_func_triforce3);

/* 0254F4B8 */
void item_func_triforce4() {
    WWHD_FUNC(0x0254F4B8, void);
    dSv_collect_onTriforce_l(svbase() + SV_COLLECT, 3);
}
VERIFY(0x0254F4B8, item_func_triforce4);

/* 0254F4CC */
void item_func_triforce5() {
    WWHD_FUNC(0x0254F4CC, void);
    dSv_collect_onTriforce_l(svbase() + SV_COLLECT, 4);
}
VERIFY(0x0254F4CC, item_func_triforce5);

/* 0254F4E0 */
void item_func_triforce6() {
    WWHD_FUNC(0x0254F4E0, void);
    dSv_collect_onTriforce_l(svbase() + SV_COLLECT, 5);
}
VERIFY(0x0254F4E0, item_func_triforce6);

/* 0254F4F4 */
void item_func_triforce7() {
    WWHD_FUNC(0x0254F4F4, void);
    dSv_collect_onTriforce_l(svbase() + SV_COLLECT, 6);
}
VERIFY(0x0254F4F4, item_func_triforce7);

/* 0254F508 */
void item_func_triforce8() {
    WWHD_FUNC(0x0254F508, void);
    dSv_collect_onTriforce_l(svbase() + SV_COLLECT, 7);
}
VERIFY(0x0254F508, item_func_triforce8);

/* 0254F51C */
void item_func_pearl1() {
    WWHD_FUNC(0x0254F51C, void);
    dSv_collect_onSymbol_l(svbase() + SV_COLLECT, 0 /* dSymbol_NAYRU_e */);
}
VERIFY(0x0254F51C, item_func_pearl1);

/* 0254F530 */
void item_func_pearl2() {
    WWHD_FUNC(0x0254F530, void);
    dSv_collect_onSymbol_l(svbase() + SV_COLLECT, 1 /* dSymbol_DIN_e */);
}
VERIFY(0x0254F530, item_func_pearl2);

/* 0254F544 */
void item_func_pearl3() {
    WWHD_FUNC(0x0254F544, void);
    dSv_collect_onSymbol_l(svbase() + SV_COLLECT, 2 /* dSymbol_FARORE_e */);
}
VERIFY(0x0254F544, item_func_pearl3);

/* 0254F558 */
void item_func_tact_song1() {
    WWHD_FUNC(0x0254F558, void);
    dSv_collect_onTact_l(svbase() + SV_COLLECT, 0);
}
VERIFY(0x0254F558, item_func_tact_song1);

/* 0254F56C */
void item_func_tact_song2() {
    WWHD_FUNC(0x0254F56C, void);
    dSv_collect_onTact_l(svbase() + SV_COLLECT, 1);
}
VERIFY(0x0254F56C, item_func_tact_song2);

/* 0254F580 */
void item_func_tact_song3() {
    WWHD_FUNC(0x0254F580, void);
    dSv_collect_onTact_l(svbase() + SV_COLLECT, 2);
}
VERIFY(0x0254F580, item_func_tact_song3);

/* 0254F594 */
void item_func_tact_song4() {
    WWHD_FUNC(0x0254F594, void);
    dSv_collect_onTact_l(svbase() + SV_COLLECT, 3);
}
VERIFY(0x0254F594, item_func_tact_song4);

/* 0254F5A8 */
void item_func_tact_song5() {
    WWHD_FUNC(0x0254F5A8, void);
    dSv_collect_onTact_l(svbase() + SV_COLLECT, 4);
}
VERIFY(0x0254F5A8, item_func_tact_song5);

/* 0254F5BC */
void item_func_tact_song6() {
    WWHD_FUNC(0x0254F5BC, void);
    dSv_collect_onTact_l(svbase() + SV_COLLECT, 5);
}
VERIFY(0x0254F5BC, item_func_tact_song6);

/* 0254F5D0: HD-only item 0x77 (GameCube: noentry): the swift sail, a second sail bit */
void item_func_swift_sail() {
    WWHD_FUNC(0x0254F5D0, void);
    dComIfGs_setItem(0x1 /* dInvSlot_SAIL_e */, 0x77 /* HD: swift sail */);
    dComIfGs_onGetItem(0x1, 1);
    st8(svbase() + SV_SELITEM + 4, 0x1 /* dInvSlot_SAIL_e */); /* HD: the sail goes on item button 4 */
    dComIfGp_setSelectItem(4);
}
VERIFY(0x0254F5D0, item_func_swift_sail);

/* 0254F7D0: HD puts the sail on the HD-only item button 4 */
void item_func_normal_sail() {
    WWHD_FUNC(0x0254F7D0, void);
    dComIfGs_setItem(0x1 /* dInvSlot_SAIL_e */, 0x78 /* dItemNo_SAIL_e */);
    dComIfGs_onGetItem(0x1, 0);
    st8(svbase() + SV_SELITEM + 4, 0x1 /* dInvSlot_SAIL_e */); /* HD: the sail goes on item button 4 */
    dComIfGp_setSelectItem(4);
}
VERIFY(0x0254F7D0, item_func_normal_sail);

/* 0254F9D0 */
void item_func_bird_esa_5() {
    WWHD_FUNC(0x0254F9D0, void);
    dSv_getbag_onBait_l(svbase() + SV_GETBAG, 0x0);
    dSv_bag_setBaitItem_l(svbase() + SV_BAGITEM, 0x82 /* dItemNo_BIRD_BAIT_5_e */);
}
VERIFY(0x0254F9D0, item_func_bird_esa_5);

/* 0254FA18 */
void item_func_animal_esa() {
    WWHD_FUNC(0x0254FA18, void);
    dSv_getbag_onBait_l(svbase() + SV_GETBAG, 0x1);
    dSv_bag_setBaitItem_l(svbase() + SV_BAGITEM, 0x83 /* dItemNo_HYOI_PEAR_e */);
}
VERIFY(0x0254FA18, item_func_animal_esa);

/* 0254FA60 */
void item_func_flower_1() {
    WWHD_FUNC(0x0254FA60, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x0);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x8C /* dItemNo_TOWN_FLOWER_e */);
}
VERIFY(0x0254FA60, item_func_flower_1);

/* 0254FAA8 */
void item_func_flower_2() {
    WWHD_FUNC(0x0254FAA8, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x1);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x8D /* dItemNo_SEA_FLOWER_e */);
}
VERIFY(0x0254FAA8, item_func_flower_2);

/* 0254FAF0 */
void item_func_flower_3() {
    WWHD_FUNC(0x0254FAF0, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x2);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x8E /* dItemNo_EXOTIC_FLOWER_e */);
}
VERIFY(0x0254FAF0, item_func_flower_3);

/* 0254FB38 */
void item_func_heros_flag() {
    WWHD_FUNC(0x0254FB38, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x3);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x8F /* dItemNo_HEROS_FLAG_e */);
}
VERIFY(0x0254FB38, item_func_heros_flag);

/* 0254FB80 */
void item_func_tairyo_flag() {
    WWHD_FUNC(0x0254FB80, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x4);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x90 /* dItemNo_BIG_CATCH_FLAG_e */);
}
VERIFY(0x0254FB80, item_func_tairyo_flag);

/* 0254FBC8 */
void item_func_sales_flag() {
    WWHD_FUNC(0x0254FBC8, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x5);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x91 /* dItemNo_BIG_SALE_FLAG_e */);
}
VERIFY(0x0254FBC8, item_func_sales_flag);

/* 0254FC10 */
void item_func_wind_flag() {
    WWHD_FUNC(0x0254FC10, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x6);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x92 /* dItemNo_PINWHEEL_e */);
}
VERIFY(0x0254FC10, item_func_wind_flag);

/* 0254FC58 */
void item_func_red_flag() {
    WWHD_FUNC(0x0254FC58, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x7);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x93 /* dItemNo_SICKLE_MOON_FLAG_e */);
}
VERIFY(0x0254FC58, item_func_red_flag);

/* 0254FCA0 */
void item_func_fossil_head() {
    WWHD_FUNC(0x0254FCA0, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x8);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x94 /* dItemNo_SKULL_TOWER_IDOL_e */);
}
VERIFY(0x0254FCA0, item_func_fossil_head);

/* 0254FCE8 */
void item_func_water_statue() {
    WWHD_FUNC(0x0254FCE8, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x9);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x95 /* dItemNo_FOUNTAIN_IDOL_e */);
}
VERIFY(0x0254FCE8, item_func_water_statue);

/* 0254FD30 */
void item_func_postman_statue() {
    WWHD_FUNC(0x0254FD30, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0xA);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x96 /* dItemNo_POSTMAN_STATUE_e */);
}
VERIFY(0x0254FD30, item_func_postman_statue);

/* 0254FD78 */
void item_func_president_statue() {
    WWHD_FUNC(0x0254FD78, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0xB);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x97 /* dItemNo_SHOP_GURU_STATUE_e */);
}
VERIFY(0x0254FD78, item_func_president_statue);

/* 0254FDC0 */
void item_func_letter00() {
    WWHD_FUNC(0x0254FDC0, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0xC);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x98 /* dItemNo_FATHER_LETTER_e */);
}
VERIFY(0x0254FDC0, item_func_letter00);

/* 0254FE08 */
void item_func_magic_seed() {
    WWHD_FUNC(0x0254FE08, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0xD);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x99 /* dItemNo_NOTE_TO_MOM_e */);
}
VERIFY(0x0254FE08, item_func_magic_seed);

/* 0254FE50 */
void item_func_magys_letter() {
    WWHD_FUNC(0x0254FE50, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0xE);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x9A /* dItemNo_MAGGIES_LETTER_e */);
}
VERIFY(0x0254FE50, item_func_magys_letter);

/* 0254FE98 */
void item_func_mo_letter() {
    WWHD_FUNC(0x0254FE98, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0xF);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x9B /* dItemNo_MOBLINS_LETTER_e */);
}
VERIFY(0x0254FE98, item_func_mo_letter);

/* 0254FEE0 */
void item_func_cottage_paper() {
    WWHD_FUNC(0x0254FEE0, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x10);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x9C /* dItemNo_CABANA_DEED_e */);
}
VERIFY(0x0254FEE0, item_func_cottage_paper);

/* 0254FF28 */
void item_func_kaisen_present1() {
    WWHD_FUNC(0x0254FF28, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x11);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x9D /* dItemNo_COMPLIMENTARY_ID_e */);
}
VERIFY(0x0254FF28, item_func_kaisen_present1);

/* 0254FF70 */
void item_func_kaisen_present2() {
    WWHD_FUNC(0x0254FF70, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x12);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0x9E /* dItemNo_FILL_UP_COUPON_e */);
}
VERIFY(0x0254FF70, item_func_kaisen_present2);

/* 0254FFB8 */
void item_func_salvage_item2() {
    WWHD_FUNC(0x0254FFB8, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x14);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0xA0 /* dItemNo_SALVAGE_ITEM_2_e */);
}
VERIFY(0x0254FFB8, item_func_salvage_item2);

/* 02550000 */
void item_func_salvage_item3() {
    WWHD_FUNC(0x02550000, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x15);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0xA1 /* dItemNo_SALVAGE_ITEM_3_e */);
}
VERIFY(0x02550000, item_func_salvage_item3);

/* 02550048 */
void item_func_xxx_039() {
    WWHD_FUNC(0x02550048, void);
    dSv_getbag_onReserve_l(svbase() + SV_GETBAG, 0x16);
    dSv_bag_setReserveItem_l(svbase() + SV_BAGITEM, 0xA2 /* dItemNo_XXX_039_e */);
}
VERIFY(0x02550048, item_func_xxx_039);

/* 02550090 */
void item_func_max_rupee_up1() {
    WWHD_FUNC(0x02550090, void);
    st8(svbase() + SV_WALLET, 0x1); /* dComIfGs_setWalletSize */
}
VERIFY(0x02550090, item_func_max_rupee_up1);

/* 025500A4 */
void item_func_max_rupee_up2() {
    WWHD_FUNC(0x025500A4, void);
    st8(svbase() + SV_WALLET, 0x2);
}
VERIFY(0x025500A4, item_func_max_rupee_up2);

/* 025500B8 */
void item_func_max_bomb_up1() {
    WWHD_FUNC(0x025500B8, void);
    st8(svbase() + SV_BOMBNUM, 60); /* dComIfGs_setBombNum */
    st8(svbase() + SV_BOMBMAX, 60); /* dComIfGs_setBombMax */
}
VERIFY(0x025500B8, item_func_max_bomb_up1);

/* 025500D4 */
void item_func_max_bomb_up2() {
    WWHD_FUNC(0x025500D4, void);
    st8(svbase() + SV_BOMBNUM, 99); /* dComIfGs_setBombNum */
    st8(svbase() + SV_BOMBMAX, 99); /* dComIfGs_setBombMax */
}
VERIFY(0x025500D4, item_func_max_bomb_up2);

/* 025500F0 */
void item_func_max_arrow_up1() {
    WWHD_FUNC(0x025500F0, void);
    st8(svbase() + SV_ARROWNUM, 60); /* dComIfGs_setArrowNum */
    st8(svbase() + SV_ARROWMAX, 60); /* dComIfGs_setArrowMax */
}
VERIFY(0x025500F0, item_func_max_arrow_up1);

/* 0255010C */
void item_func_max_arrow_up2() {
    WWHD_FUNC(0x0255010C, void);
    st8(svbase() + SV_ARROWNUM, 99); /* dComIfGs_setArrowNum */
    st8(svbase() + SV_ARROWMAX, 99); /* dComIfGs_setArrowMax */
}
VERIFY(0x0255010C, item_func_max_arrow_up2);

/* 02550128 */
void item_func_max_mp_up1() {
    WWHD_FUNC(0x02550128, void);
    dComIfGp_setItemMaxMagicCount(32);
}
VERIFY(0x02550128, item_func_max_mp_up1);

/* 02550154 */
void item_func_tincle_rupee1() {
    WWHD_FUNC(0x02550154, void);
    dComIfGp_setItemRupeeCount(50);
}
VERIFY(0x02550154, item_func_tincle_rupee1);

/* 02550180 */
void item_func_tincle_rupee2() {
    WWHD_FUNC(0x02550180, void);
    dComIfGp_setItemRupeeCount(100);
}
VERIFY(0x02550180, item_func_tincle_rupee2);

/* 025501AC */
void item_func_tincle_rupee3() {
    WWHD_FUNC(0x025501AC, void);
    dComIfGp_setItemRupeeCount(150);
}
VERIFY(0x025501AC, item_func_tincle_rupee3);

/* 025501D8 */
void item_func_tincle_rupee4() {
    WWHD_FUNC(0x025501D8, void);
    dComIfGp_setItemRupeeCount(200);
}
VERIFY(0x025501D8, item_func_tincle_rupee4);

/* 02550204 */
void item_func_tincle_rupee5() {
    WWHD_FUNC(0x02550204, void);
    dComIfGp_setItemRupeeCount(250);
}
VERIFY(0x02550204, item_func_tincle_rupee5);

/* 02550230 */
void item_func_tincle_rupee6() {
    WWHD_FUNC(0x02550230, void);
    dComIfGp_setItemRupeeCount(500);
}
VERIFY(0x02550230, item_func_tincle_rupee6);

/* 0255025C */
void item_func_lithograph1() {
    WWHD_FUNC(0x0255025C, void);
    dSv_event_onEventBit_l(svbase() + SV_EVENT, 0x3508 /* LITHOGRAPH_1 */);
}
VERIFY(0x0255025C, item_func_lithograph1);

/* 02550270 */
void item_func_lithograph2() {
    WWHD_FUNC(0x02550270, void);
    dSv_event_onEventBit_l(svbase() + SV_EVENT, 0x3504 /* LITHOGRAPH_2 */);
}
VERIFY(0x02550270, item_func_lithograph2);

/* 02550284 */
void item_func_lithograph3() {
    WWHD_FUNC(0x02550284, void);
    dSv_event_onEventBit_l(svbase() + SV_EVENT, 0x3502 /* LITHOGRAPH_3 */);
}
VERIFY(0x02550284, item_func_lithograph3);

/* 02550298 */
void item_func_lithograph4() {
    WWHD_FUNC(0x02550298, void);
    dSv_event_onEventBit_l(svbase() + SV_EVENT, 0x3501 /* LITHOGRAPH_4 */);
}
VERIFY(0x02550298, item_func_lithograph4);

/* 025502AC */
void item_func_lithograph5() {
    WWHD_FUNC(0x025502AC, void);
    dSv_event_onEventBit_l(svbase() + SV_EVENT, 0x3680 /* LITHOGRAPH_5 */);
}
VERIFY(0x025502AC, item_func_lithograph5);

/* 025502C0 */
void item_func_lithograph6() {
    WWHD_FUNC(0x025502C0, void);
    dSv_event_onEventBit_l(svbase() + SV_EVENT, 0x3640 /* LITHOGRAPH_6 */);
}
VERIFY(0x025502C0, item_func_lithograph6);

/* 025502D4 */
void item_func_collectmap64() {
    WWHD_FUNC(0x025502D4, void);
    dSv_event_onEventBit_l(svbase() + SV_EVENT, 0x3620 /* LITHOGRAPH_7 */);
}
VERIFY(0x025502D4, item_func_collectmap64);

/* 025502E8 */
void item_func_collectmap63() {
    WWHD_FUNC(0x025502E8, void);
    dSv_event_onEventBit_l(svbase() + SV_EVENT, 0x3610 /* LITHOGRAPH_8 */);
}
VERIFY(0x025502E8, item_func_collectmap63);

/* 025502FC */
void item_func_collectmap62() {
    WWHD_FUNC(0x025502FC, void);
    dSv_event_onEventBit_l(svbase() + SV_EVENT, 0x3608 /* LITHOGRAPH_9 */);
}
VERIFY(0x025502FC, item_func_collectmap62);

}  // namespace d_item_2_cpp

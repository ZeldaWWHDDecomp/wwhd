/* Acquisition handlers: tww names and native HD layouts. */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
// HD: map acquisition also notifies this object; its full identity is unverified.
struct ItemAcquireMapView {
    u8 mUnaccessed[0x228];
    be<u32> mNewMap;
};
WWHD_OFFSET(ItemAcquireMapView, mNewMap, 0x228);
struct ItemAcquireMapGlobal {
    gptr<ItemAcquireMapView> mMenu;
};
void itemAcquireMarkGet(dSv_player_get_item_c* items, s32 slot, u32 bit);
void itemAcquireSelectEquip(s32 equip, u32 item);
void itemAcquireSetBeast(dSv_player_bag_item_c* bag, u32 item);
void itemAcquireSetBait(dSv_player_bag_item_c* bag, u32 item);
void itemAcquireSetReserve(dSv_player_bag_item_c* bag, u32 item);
void itemAcquireDungeonItem(dSv_memBit_c* bits, s32 item);
void itemAcquireBottleFlag(dSv_player_get_item_c* items, u32 item);
void itemAcquireEmptyBottle(dSv_player_item_c* items);
void itemAcquireTriforce(dSv_player_collect_c* collect, u32 piece);
void itemAcquireSymbol(dSv_player_collect_c* collect, u32 symbol);
void itemAcquireTact(dSv_player_collect_c* collect, u32 song);
// Missing shared accessors use unit-local names; every call re-reads the save pointer.
inline void itemAcquireOnGetItem(s32 slot, u32 bit) {
    itemAcquireMarkGet(&dComIfGs_info()->mSavedata.mPlayer.mGetItem, slot, bit);
}
inline u8 itemAcquireInventory(dSv_player_c& player, u32 idx) {
    if (idx < 21)
        return player.mPlayerItem.mItems[idx];
    if (idx < 24)
        return 0xFF;
    if (idx < 32)
        return player.mBagItem.mBeast[idx - 24];
    if (idx < 36)
        return 0xFF;
    if (idx < 44)
        return player.mBagItem.mBait[idx - 36];
    if (idx - 48 < 8)
        return player.mBagItem.mReserve[idx - 48];
    return 0xFF;
}
// HD: five item buttons; preserve the initial selected slot and repeated save-pointer loads.
inline void itemAcquireSelectItem(u32 button) {
    u8 selected = dComIfGs_info()->mSavedata.mPlayer.mPlayerStatusA.mSelectItem[button];
    dComIfG_play_c* play = &((dComIfG_inf_c*)dComIfGp_get())->play;
    if (selected == 0xFF) {
        play->mSelectItem[button] = 0xFF;
        return;
    }
    dSv_player_c* player = &dComIfGs_info()->mSavedata.mPlayer;
    play->mSelectItem[button] =
        itemAcquireInventory(*player, player->mPlayerStatusA.mSelectItem[button]);
    player = &dComIfGs_info()->mSavedata.mPlayer;
    if (itemAcquireInventory(*player, player->mPlayerStatusA.mSelectItem[button]) == 0xFF)
        player->mPlayerStatusA.mSelectItem[button] = 0xFF;
}
inline void itemAcquireRefreshSelectItem(u8 oldItem) {
    for (u32 button = 0; button < 3; button++) {
        if (dComIfGp_getSelectItem(button) == oldItem)
            itemAcquireSelectItem(button);
    }
}
enum ItemAcquireValue {
    ITEM_HD_SWIFT_SAIL = 0x77, // HD-only item
    dInvSlot_SKULL_HAMMER_e = 0x14,
    dItemNo_SKULL_HAMMER_e = 0x33,
    dInvSlot_DEKU_LEAF_e = 0x6,
    dItemNo_DEKU_LEAF_e = 0x34,
    dInvSlot_BOW_e = 0xC,
    dItemNo_MAGIC_ARROW_e = 0x35,
    dItemNo_BOW_e = 0x27,
    dItemNo_LIGHT_ARROW_e = 0x36,
    dItemNo_SWORD_e = 0x38,
    dItemNo_MASTER_SWORD_1_e = 0x39,
    dItemNo_MASTER_SWORD_2_e = 0x3A,
    dItemNo_SHIELD_e = 0x3B,
    dItemNo_MIRROR_SHIELD_e = 0x3C,
    dItemNo_MASTER_SWORD_3_e = 0x3E,
    dItemNo_SKULL_NECKLACE_e = 0x45,
    dBeastIdx_SKULL_NECKLACE_e = 0x0,
    dItemNo_BOKOBABA_SEED_e = 0x46,
    dBeastIdx_BOKOBABA_SEED_e = 0x1,
    dItemNo_GOLDEN_FEATHER_e = 0x47,
    dBeastIdx_GOLDEN_FEATHER_e = 0x2,
    dItemNo_KNIGHTS_CREST_e = 0x48,
    dBeastIdx_KNIGHTS_CREST_e = 0x3,
    dItemNo_RED_JELLY_e = 0x49,
    dBeastIdx_RED_JELLY_e = 0x4,
    dItemNo_GREEN_JELLY_e = 0x4A,
    dBeastIdx_GREEN_JELLY_e = 0x5,
    dItemNo_BLUE_JELLY_e = 0x4B,
    dBeastIdx_BLUE_JELLY_e = 0x6,
    dItemNo_EMPTY_BSHIP_e = 0x4F,
    dItemNo_EMPTY_BOTTLE_e = 0x50,
    dItemNo_RED_POTION_e = 0x51,
    dItemNo_GREEN_POTION_e = 0x52,
    dItemNo_BLUE_POTION_e = 0x53,
    dItemNo_HALF_SOUP_BOTTLE_e = 0x54,
    dItemNo_SOUP_BOTTLE_e = 0x55,
    dItemNo_WATER_BOTTLE_e = 0x56,
    dItemNo_FAIRY_BOTTLE_e = 0x57,
    dItemNo_FIREFLY_BOTTLE_e = 0x58,
    dItemNo_FOREST_WATER_e = 0x59,
    dSymbol_NAYRU_e = 0x0,
    dSymbol_DIN_e = 0x1,
    dSymbol_FARORE_e = 0x2,
    dInvSlot_SAIL_e = 0x1,
    dItemNo_SAIL_e = 0x78,
    dItemNo_BIRD_BAIT_5_e = 0x82,
    dItemNo_HYOI_PEAR_e = 0x83,
    dItemNo_TOWN_FLOWER_e = 0x8C,
    dItemNo_SEA_FLOWER_e = 0x8D,
    dItemNo_EXOTIC_FLOWER_e = 0x8E,
    dItemNo_HEROS_FLAG_e = 0x8F,
    dItemNo_BIG_CATCH_FLAG_e = 0x90,
    dItemNo_BIG_SALE_FLAG_e = 0x91,
    dItemNo_PINWHEEL_e = 0x92,
    dItemNo_SICKLE_MOON_FLAG_e = 0x93,
    dItemNo_SKULL_TOWER_IDOL_e = 0x94,
    dItemNo_FOUNTAIN_IDOL_e = 0x95,
    dItemNo_POSTMAN_STATUE_e = 0x96,
    dItemNo_SHOP_GURU_STATUE_e = 0x97,
    dItemNo_FATHER_LETTER_e = 0x98,
    dItemNo_NOTE_TO_MOM_e = 0x99,
    dItemNo_MAGGIES_LETTER_e = 0x9A,
    dItemNo_MOBLINS_LETTER_e = 0x9B,
    dItemNo_CABANA_DEED_e = 0x9C,
    dItemNo_COMPLIMENTARY_ID_e = 0x9D,
    dItemNo_FILL_UP_COUPON_e = 0x9E,
    dItemNo_SALVAGE_ITEM_2_e = 0xA0,
    dItemNo_SALVAGE_ITEM_3_e = 0xA1,
    dItemNo_XXX_039_e = 0xA2,
};

struct dSv_event_flag_c {
    enum {
        LITHOGRAPH_1 = 0x3508,
        LITHOGRAPH_2 = 0x3504,
        LITHOGRAPH_3 = 0x3502,
        LITHOGRAPH_4 = 0x3501,
        LITHOGRAPH_5 = 0x3680,
        LITHOGRAPH_6 = 0x3640,
        LITHOGRAPH_7 = 0x3620,
        LITHOGRAPH_8 = 0x3610,
        LITHOGRAPH_9 = 0x3608,
        LITHOGRAPH_10 = 0x3604,
    };
};

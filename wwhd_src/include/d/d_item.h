/* Item acquisition: typed native save/play fields and HD statistics. */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
struct ItemFunctionTable {
    gfn mFunctions[256];
};
// HD: PPC indexing wraps at 32 bits; itemNo must not be narrowed to the GameCube u8.
inline u32 itemFunction(ItemFunctionTable& table, u32 itemNo) {
    return gabi::at<gfn>((u32)(gabi::ea(&table) + (itemNo << 2)))->get();
}
struct ItemStageInfo {
    u8 mUnaccessed[9];
    be<u8> mProp;
};
WWHD_OFFSET(ItemStageInfo, mProp, 9);
enum ItemStage { ITEM_STAGE_SEA = 0, ITEM_STAGE_FF = 2 };
struct ItemMessageBottle;
struct ItemBottleGlobal {
    gptr<ItemMessageBottle> mBottle;
};
struct ItemStatisticsCounters;
struct ItemHdStatistics {
    u8 mUnaccessed[0x64C];
};
// HD: the statistics object follows the ordinary save info; its native accessor is retained.
struct ItemSaveView {
    dSv_info_c mSave;
    ItemHdStatistics mStatistics;
};
WWHD_OFFSET(ItemSaveView, mStatistics, 0x12A0);
ItemStatisticsCounters* itemGetStatistics(ItemHdStatistics* statistics);
void itemStatistics_addMessageItem(ItemStatisticsCounters* counters, s32 count);
void itemStatistics_addHeartPiece(ItemStatisticsCounters* counters, s32 count);
void itemReceiveMessageBottle(ItemMessageBottle* bottle);
void itemMarkGet(dSv_player_get_item_c* items, s32 slot, u32 bit);
void itemMarkDungeonItem(dSv_memBit_c* bits, s32 item);
void itemOnStageLife(s32 stage);
void itemSetSelectEquip(s32 equip, u32 item);
void itemSetBeastItem(dSv_player_bag_item_c* bag, u32 item);
inline ItemStatisticsCounters* itemStatistics() {
    return itemGetStatistics(&((ItemSaveView*)dComIfGs_info())->mStatistics);
}
// Shared inline accessors not yet in the common headers use unit-local names.
inline void itemOnGetItem(s32 slot, u32 bit) {
    itemMarkGet(&dComIfGs_info()->mSavedata.mPlayer.mGetItem, slot, bit);
}
inline u8 itemInventory(dSv_player_c& player, u32 idx) {
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
inline void itemSetSelectItem(u32 button) {
    u8 selected = dComIfGs_info()->mSavedata.mPlayer.mPlayerStatusA.mSelectItem[button];
    dComIfG_play_c* play = &((dComIfG_inf_c*)dComIfGp_get())->play;
    if (selected == 0xFF) {
        play->mSelectItem[button] = 0xFF;
        return;
    }
    dSv_player_c* player = &dComIfGs_info()->mSavedata.mPlayer;
    play->mSelectItem[button] = itemInventory(*player, player->mPlayerStatusA.mSelectItem[button]);
    player = &dComIfGs_info()->mSavedata.mPlayer;
    if (itemInventory(*player, player->mPlayerStatusA.mSelectItem[button]) == 0xFF)
        player->mPlayerStatusA.mSelectItem[button] = 0xFF;
}
inline void itemRefreshSelectItem(u8 oldItem) {
    for (u32 button = 0; button < 3; button++) {
        if (dComIfGp_getSelectItem(button) == oldItem)
            itemSetSelectItem(button);
    }
}
enum ItemAcquisitionValue {
    dItemNo_JOY_PENDANT_e = 0x1F,
    dInvSlot_BOMB_e = 0xD,
    dItemNo_BOMB_BAG_e = 0x31,
    dInvSlot_TELESCOPE_e = 0x0,
    dItemNo_TELESCOPE_e = 0x20,
    dInvSlot_TINGLE_TUNER_e = 0x7,
    dItemNo_TINGLE_TUNER_e = 0x21,
    dInvSlot_WIND_WAKER_e = 0x2,
    dItemNo_WIND_WAKER_e = 0x22,
    dInvSlot_CAMERA_e = 0x8,
    dItemNo_PICTO_BOX_e = 0x23,
    dInvSlot_SPOILS_BAG_e = 0x4,
    dItemNo_SPOILS_BAG_e = 0x24,
    dInvSlot_GRAPPLING_HOOK_e = 0x3,
    dItemNo_GRAPPLING_HOOK_e = 0x25,
    dItemNo_DELUXE_PICTO_BOX_e = 0x26,
    dInvSlot_BOW_e = 0xC,
    dItemNo_BOW_e = 0x27,
    dItemNo_POWER_BRACELETS_e = 0x28,
    dInvSlot_IRON_BOOTS_e = 0x9,
    dItemNo_IRON_BOOTS_e = 0x29,
    dInvSlot_MAGIC_ARMOR_e = 0xA,
    dItemNo_MAGIC_ARMOR_e = 0x2A,
    dInvSlot_BAIT_BAG_e = 0xB,
    dItemNo_BAIT_BAG_e = 0x2C,
    dInvSlot_BOOMERANG_e = 0x5,
    dItemNo_BOOMERANG_e = 0x2D,
    dItemNo_BARE_HAND_e = 0x2E,
    dInvSlot_HOOKSHOT_e = 0x13,
    dItemNo_HOOKSHOT_e = 0x2F,
    dInvSlot_DELIVERY_BAG_e = 0x12,
    dItemNo_DELIVERY_BAG_e = 0x30,
    dItemNo_BOMB_5_e = 0xB,
    dBeastIdx_JOY_PENDANT_e = 0x7,
};

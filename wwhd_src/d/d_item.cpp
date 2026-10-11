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
 * HD save/play layouts use the shared dComIfG and dSv fields. The statistics object follows
 * dSv_info_c, and buttons 3/4 extend the GameCube item selection. */
#include "d/d_item.h"

// HD: the dispatch index retains all 32 incoming bits.
static ItemFunctionTable item_func_ptr;
static ItemBottleGlobal l_messageBottle;
static ItemFunctionTable item_getcheck_func_ptr;

// HD: item_func_ptr[itemNo]() (tail call; itemNo is not masked)
/* 0254DA38 */
void execItemGet(u32 itemNo) {
    gabi::call_ptr<void>(itemFunction(item_func_ptr, itemNo));
}

// HD: internal linkage keeps this verified body separate from the companion native-call binding.
/* 0254DA50 */
static s32 checkItemGet(u32 itemNo, s32 defaultVal) {
    s32 result = gabi::call_ptr<s32>(itemFunction(item_getcheck_func_ptr, itemNo));
    if (result == -1)
        return defaultVal;
    return result;
}

/* 0254DA9C */
void item_func_heart() {
    dComIfGp_setItemLifeCount(4.0f);
}

/* 0254DAD0 */
void item_func_green_rupee() {
    dComIfGp_setItemRupeeCount(1);
}

/* 0254DAFC */
void item_func_blue_rupee() {
    dComIfGp_setItemRupeeCount(5);
}

/* 0254DB28 */
void item_func_white_rupee() {
    dComIfGp_setItemRupeeCount(10);
}

/* 0254DB54 */
void item_func_red_rupee() {
    dComIfGp_setItemRupeeCount(20);
}

/* 0254DB80 */
void item_func_purple_rupee() {
    dComIfGp_setItemRupeeCount(50);
}

/* 0254DBAC */
void item_func_orange_rupee() {
    dComIfGp_setItemRupeeCount(100);
}

// HD: also counts the heart piece in the HD statistics block
/* 0254DBD8 */
void item_func_kakera_heart() {
    dComIfGp_setItemMaxLifeCount(1);
    itemStatistics_addHeartPiece(itemStatistics(), 1);
}

/* 0254DC1C */
void item_func_utuwa_heart() {
    dComIfGp_setItemMaxLifeCount(4);
    f32 maxLife = (f32)dComIfGs_getMaxLife();
    dComIfGp_setItemLifeCount(maxLife);

    ItemStageInfo* stag_info = (ItemStageInfo*)dComIfGp_getStageStagInfo();
    if (((stag_info->mProp >> 1) & 0x7F) == ITEM_STAGE_SEA) {
        // Didn't get the Heart Container immediately after defeating Helmaroc King.
        // Instead got it from outside Forsaken Fortress, on the sea stage.
        itemOnStageLife(ITEM_STAGE_FF);
    } else {
        itemMarkDungeonItem(&dComIfGs_info()->mMemory.mMembit, dSv_memBit_c::STAGE_LIFE);
    }
}

/* 0254DCFC */
void item_func_s_magic() {
    dComIfGp_setItemMagicCount(4);
}

/* 0254DD28 */
void item_func_l_magic() {
    dComIfGp_setItemMagicCount(8);
}

/* 0254DD54 */
void item_func_bomb_5() {
    itemOnGetItem(dInvSlot_BOMB_e, 0);
    dComIfG_play_c* play = &((dComIfG_inf_c*)dComIfGp_get())->play;
    play->mItemSlot = dInvSlot_BOMB_e;
    play->mItemNo = dItemNo_BOMB_5_e;
    dComIfGp_setItemBombNumCount(5);
}

/* 0254DDAC */
void item_func_bomb_10() {
    itemOnGetItem(dInvSlot_BOMB_e, 0);
    dComIfGs_setItem(dInvSlot_BOMB_e, dItemNo_BOMB_BAG_e);
    dComIfGp_setItemBombNumCount(10);
}

/* 0254DE04 */
void item_func_bomb_20() {
    itemOnGetItem(dInvSlot_BOMB_e, 0);
    dComIfGs_setItem(dInvSlot_BOMB_e, dItemNo_BOMB_BAG_e);
    dComIfGp_setItemBombNumCount(20);
}

/* 0254DE5C */
void item_func_bomb_30() {
    itemOnGetItem(dInvSlot_BOMB_e, 0);
    dComIfGs_setItem(dInvSlot_BOMB_e, dItemNo_BOMB_BAG_e);
    dComIfGp_setItemBombNumCount(30);
}

/* 0254DEB4 */
void item_func_silver_rupee() {
    dComIfGp_setItemRupeeCount(200);
}

/* 0254DEE0 */
void item_func_arrow_10() {
    dComIfGp_setItemArrowNumCount(10);
}

/* 0254DF0C */
void item_func_arrow_20() {
    dComIfGp_setItemArrowNumCount(20);
}

/* 0254DF38 */
void item_func_arrow_30() {
    dComIfGp_setItemArrowNumCount(30);
}

// HD: additional item 0x14 (GameCube: noentry). It runs 0203A4B8 on the object at *1018F4AC (an HD
// system with ten 0x3EC-byte message slots; probably the Tingle Bottle) and counts the event in the
// HD statistics block.
/* 0254DF64 */
void item_func_hd_14() {
    itemReceiveMessageBottle(l_messageBottle.mBottle.get());
    itemStatistics_addMessageItem(itemStatistics(), 1);
}

/* 0254DFA4 */
void item_func_small_key() {
    dComIfGp_setItemKeyNumCount(1);
}

/* 0254DFD0 */
void item_func_recover_faily() {
    dComIfGp_setItemLifeCount(40.0f);
}

/* 0254E004 */
void item_func_subdun_rupee() {
    dComIfGp_setItemRupeeCount(10);
}

/* 0254E030 */
void item_func_pendant() {
    itemSetBeastItem(&dComIfGs_info()->mSavedata.mPlayer.mBagItem, dItemNo_JOY_PENDANT_e);
    dComIfGs_onGetItemBeast(dBeastIdx_JOY_PENDANT_e);
    dComIfGp_setItemBeastNumCount(dBeastIdx_JOY_PENDANT_e, 1);
}

/* 0254E088 */
void item_func_telescope() {
    itemOnGetItem(dInvSlot_TELESCOPE_e, 0);
    dComIfGs_setItem(dInvSlot_TELESCOPE_e, dItemNo_TELESCOPE_e);
}

/* 0254E0D0 */
void item_func_tncl_whitsl() {
    itemOnGetItem(dInvSlot_TINGLE_TUNER_e, 0);
    dComIfGs_setItem(dInvSlot_TINGLE_TUNER_e, dItemNo_TINGLE_TUNER_e);
}

// HD: puts the Wind Waker on the HD-only item button 3
/* 0254E118 */
void item_func_wind_tact() {
    itemOnGetItem(dInvSlot_WIND_WAKER_e, 0);
    dComIfGs_setItem(dInvSlot_WIND_WAKER_e, dItemNo_WIND_WAKER_e);
    dComIfGs_info()->mSavedata.mPlayer.mPlayerStatusA.mSelectItem[3] = dInvSlot_WIND_WAKER_e;
    itemSetSelectItem(3);
}

/* 0254E318 */
void item_func_camera() {
    itemOnGetItem(dInvSlot_CAMERA_e, 0);
    dComIfGs_setItem(dInvSlot_CAMERA_e, dItemNo_PICTO_BOX_e);
}

/* 0254E360 */
void item_func_emono_bag() {
    itemOnGetItem(dInvSlot_SPOILS_BAG_e, 0);
    dComIfGs_setItem(dInvSlot_SPOILS_BAG_e, dItemNo_SPOILS_BAG_e);
}

/* 0254E3A8 */
void item_func_rope() {
    itemOnGetItem(dInvSlot_GRAPPLING_HOOK_e, 0);
    dComIfGs_setItem(dInvSlot_GRAPPLING_HOOK_e, dItemNo_GRAPPLING_HOOK_e);
}

// HD: (like the GameCube USA/PAL arrows) refreshes the X/Y/Z buttons that showed the old Picto
// Box
/* 0254E3F0 */
void item_func_camera2() {
    itemOnGetItem(dInvSlot_CAMERA_e, 1);
    dComIfGs_setItem(dInvSlot_CAMERA_e, dItemNo_DELUXE_PICTO_BOX_e);
    itemRefreshSelectItem(dItemNo_PICTO_BOX_e);
}

/* 0254E630 */
void item_func_bow() {
    itemOnGetItem(dInvSlot_BOW_e, 0);
    dComIfGs_setItem(dInvSlot_BOW_e, dItemNo_BOW_e);
    dComIfGs_info()->mSavedata.mPlayer.mItemRecord.mItemRecord2.mArrowNum = 30;
    dComIfGs_info()->mSavedata.mPlayer.mItemMax.mItemMax2.mArrowNum = 30;
}

/* 0254E68C */
void item_func_pwr_groove() {
    dComIfGs_onCollect(0x2, 0);
    itemSetSelectEquip(0x2, dItemNo_POWER_BRACELETS_e);
}

/* 0254E6CC */
void item_func_hvy_boots() {
    itemOnGetItem(dInvSlot_IRON_BOOTS_e, 0);
    dComIfGs_setItem(dInvSlot_IRON_BOOTS_e, dItemNo_IRON_BOOTS_e);
}

/* 0254E714 */
void item_func_drgn_shield() {
    itemOnGetItem(dInvSlot_MAGIC_ARMOR_e, 0);
    dComIfGs_setItem(dInvSlot_MAGIC_ARMOR_e, dItemNo_MAGIC_ARMOR_e);
}

/* 0254E75C */
void item_func_esa_bag() {
    itemOnGetItem(dInvSlot_BAIT_BAG_e, 0);
    dComIfGs_setItem(dInvSlot_BAIT_BAG_e, dItemNo_BAIT_BAG_e);
}

/* 0254E7A4 */
void item_func_boomerang() {
    itemOnGetItem(dInvSlot_BOOMERANG_e, 0);
    dComIfGs_setItem(dInvSlot_BOOMERANG_e, dItemNo_BOOMERANG_e);
}

/* 0254E7EC */
void item_func_bare_hand() {
    itemSetSelectEquip(0x02, dItemNo_BARE_HAND_e);
}

/* 0254E7F8 */
void item_func_hookshot() {
    itemOnGetItem(dInvSlot_HOOKSHOT_e, 0);
    dComIfGs_setItem(dInvSlot_HOOKSHOT_e, dItemNo_HOOKSHOT_e);
}

/* 0254E840 */
void item_func_warasibe_bag() {
    itemOnGetItem(dInvSlot_DELIVERY_BAG_e, 0);
    dComIfGs_setItem(dInvSlot_DELIVERY_BAG_e, dItemNo_DELIVERY_BAG_e);
}

/* 0254E888 */
void item_func_bomb_bag() {
    itemOnGetItem(dInvSlot_BOMB_e, 0);
    dComIfGs_setItem(dInvSlot_BOMB_e, dItemNo_BOMB_BAG_e);
    dComIfGs_info()->mSavedata.mPlayer.mItemRecord.mItemRecord2.mBombNum = 30;
    dComIfGs_info()->mSavedata.mPlayer.mItemMax.mItemMax2.mBombNum = 30;
}
/* WWHD glue
data item_func_ptr 101E3A98
data item_getcheck_func_ptr 101E3E98
data l_messageBottle 1018F4AC
ext 025B5CCC void itemMarkGet(dSv_player_get_item_c* items, s32 slot, u32 bit)
ext 025B9098 void itemMarkDungeonItem(dSv_memBit_c* bits, s32 item)
ext 02520B08 void itemOnStageLife(s32 stage)
ext 02522398 void itemSetSelectEquip(s32 equip, u32 item)
ext 025B62A4 void itemSetBeastItem(dSv_player_bag_item_c* bag, u32 item)
ext 027201DC ItemStatisticsCounters* itemGetStatistics(ItemHdStatistics* statistics)
ext 02726DCC void itemStatistics_addMessageItem(ItemStatisticsCounters* counters, s32 count)
ext 02726E0C void itemStatistics_addHeartPiece(ItemStatisticsCounters* counters, s32 count)
ext 0203A4B8 void itemReceiveMessageBottle(ItemMessageBottle* bottle)
*/

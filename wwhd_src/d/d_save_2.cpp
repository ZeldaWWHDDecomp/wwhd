/* d_save — Spoils, bait, delivery-bag inventory and collection flags.
 * GameCube names with verified WWHD layouts and behavior. Unit: d_save. */
#include "bindings.h"
#include "d/d_save_readable.h"

namespace d_save_cpp {
#include "d/d_save_local.h"

/* change of a bag item on an item button (inline body of setBaitItemChange /
 * setReserveItemChange): the inventory slot, the play item slot, then the select item */
static inline void changeBagItem(u32 btn, u8 invIdx, u32 itemNo) {
    setInventoryItem(svbase(), invIdx, (u8)itemNo);
    u32 pl = play();
    playInfo(pl)->play.mItemNo = (u8)itemNo;
    playInfo(pl)->play.mItemSlot = invIdx;
    u8 sel = (u8)indexed(savePlayer().mPlayerStatusA.mSelectItem, btn);
    refreshSelectedItem(btn, sel);
}

/* 025B6214 */
void d_save_cpp::dSv_player_bag_item_c::init() {
    for (u32 i = 0; i < 8; i++) indexed(mBeast, i) = dInvSlot_NONE_e;
    for (u32 i = 0; i < 8; i++) indexed(mBait, i) = dInvSlot_NONE_e;
    for (u32 i = 0; i < 8; i++) indexed(mReserve, i) = dInvSlot_NONE_e;
}

/* 025B6274 */
bool d_save_cpp::dSv_player_bag_item_c::checkBeastItem(u32 itemNo) {
    for (u32 i = 0; i < 8; i++)
        if ((u8)indexed(mBeast, i) == itemNo) return true;
    return false;
}

/* 025B62A4 */
void d_save_cpp::dSv_player_bag_item_c::setBeastItem(u32 itemNo) {
    if (!checkBeastItem(itemNo)) {
        for (u32 i = 0; i < 8; i++) {
            if ((u8)indexed(mBeast, i) == dInvSlot_NONE_e) {
                indexed(mBeast, i) = (u8)itemNo;
                return;
            }
        }
    }
}

/* 025B6308 */
void d_save_cpp::dSv_player_bag_item_c::setBeastItemEmpty(u32 itemNo) {
    if (!checkBeastItem(itemNo)) return;
    for (u32 i = 0; i < 8; i++) {
        if ((u8)indexed(mBeast, i) == itemNo) {
            indexed(mBeast, i) = dInvSlot_NONE_e;
            for (u32 btn = 0; btn < ITEM_BUTTON_COUNT; btn++) {
                if ((u8)indexed(playInfo()->play.mSelectItem, btn) == itemNo) {
                    u32 b = svbase();
                    u8 invIdx = (u8)indexed(savePlayer(b).mPlayerStatusA.mSelectItem, btn);
                    setInventoryItem(b, invIdx, dInvSlot_NONE_e);
                    u32 pl = play();
                    playInfo(pl)->play.mItemSlot = invIdx;
                    playInfo(pl)->play.mItemNo = dInvSlot_NONE_e;
                    u8 sel = (u8)indexed(savePlayer().mPlayerStatusA.mSelectItem, btn);
                    refreshSelectedItem(btn, sel);
                }
            }
            return;
        }
    }
}

/* 025B6638 */
void d_save_cpp::dSv_player_bag_item_c::setBaitItemChange(u32 itemNo) {
    u32 btn = talkItemButton();
    if (btn == dInvSlot_NONE_e) return;
    u8 invIdx = (u8)indexed(savePlayer().mPlayerStatusA.mSelectItem, btn);
    if ((u32)(invIdx - 0x24) >= 8) return;
    changeBagItem(btn, invIdx, itemNo);
}

/* 025B6AB0 */
void d_save_cpp::dSv_player_bag_item_c::setBaitItemChange(u32 btn, u32 itemNo) {
    if (btn > 2) return;
    u8 invIdx = (u8)indexed(savePlayer().mPlayerStatusA.mSelectItem, btn);
    if ((u32)(invIdx - 0x24) >= 8) return;
    changeBagItem(btn, invIdx, itemNo);
}

/* bait use (inline body of both setBaitItemEmpty): HD rereads the play select item for each
 * compare; Hyoi pear (0x83) empties the slot, bird bait (0x82) counts down */
static inline bool consumeBait(u32 btn, u8 invIdx) {
    u32 slot = (u32)invIdx - 0x24;
    if ((u8)indexed(playInfo()->play.mSelectItem, btn) == item_data::dItemNo_HYOI_PEAR_e) return true;
    if ((u8)indexed(playInfo()->play.mSelectItem, btn) != item_data::dItemNo_BIRD_BAIT_5_e) return false;
    u32 b = svbase();
    u8 num = (u8)indexed(savePlayer(b).mBagItemRecord.mBaitNum, slot);
    if (num != 0) num = (u8)(num - 1);
    indexed(savePlayer(b).mBagItemRecord.mBaitNum, slot) = num;
    return num == 0;
}

/* 025B6994 */
void d_save_cpp::dSv_player_bag_item_c::setBaitItemEmpty() {
    u32 btn = talkItemButton();
    if (btn == dInvSlot_NONE_e) return;
    u8 invIdx = (u8)indexed(savePlayer().mPlayerStatusA.mSelectItem, btn);
    if ((u32)(invIdx - 0x24) >= 8) return;
    if (consumeBait(btn, invIdx)) setBaitItemChange(dInvSlot_NONE_e);
}

/* 025B6DA0 */
void d_save_cpp::dSv_player_bag_item_c::setBaitItemEmpty(u32 btn) {
    if (btn > 2) return;
    u8 invIdx = (u8)indexed(savePlayer().mPlayerStatusA.mSelectItem, btn);
    if ((u32)(invIdx - 0x24) >= 8) return;
    if (consumeBait(btn, invIdx)) setBaitItemChange(btn, dInvSlot_NONE_e);
}

/* 025B6E5C */
u8 d_save_cpp::dSv_player_bag_item_c::checkBaitItem(u32 itemNo) {
    u8 ret = 0;
    for (u32 i = 0; i < 8; i++)
        if ((u8)indexed(mBait, i) == itemNo) ret++;
    return ret;
}

/* 025B6E90 */
u8 d_save_cpp::dSv_player_bag_item_c::checkBaitItemEmpty() {
    return checkBaitItem(dInvSlot_NONE_e);
}

/* 025B6E98 */
void d_save_cpp::dSv_player_bag_item_c::setBaitItem(u32 itemNo) {
    if (checkBaitItemEmpty() != 0) {
        for (u32 i = 0; i < 8; i++) {
            if ((u8)indexed(mBait, i) == dInvSlot_NONE_e) {
                indexed(mBait, i) = (u8)itemNo;
                indexed(savePlayer().mBagItemRecord.mBaitNum, i) = 3;
                return;
            }
        }
    }
}

/* 025B6F14 */
void d_save_cpp::dSv_player_bag_item_c::setReserveItemChange(u32 itemNo) {
    u32 btn = talkItemButton();
    if (btn == dInvSlot_NONE_e) return;
    u8 invIdx = (u8)indexed(savePlayer().mPlayerStatusA.mSelectItem, btn);
    if ((u32)(invIdx - 0x30) >= 8) return;
    changeBagItem(btn, invIdx, itemNo);
}

/* 025B7270 */
void d_save_cpp::dSv_player_bag_item_c::setReserveItemEmpty() {
    setReserveItemChange(dInvSlot_NONE_e);
}

/* 025B7278 */
void d_save_cpp::dSv_player_bag_item_c::setReserveItemChange(u32 btn, u32 itemNo) {
    if (btn > 2) return;
    u8 invIdx = (u8)indexed(savePlayer().mPlayerStatusA.mSelectItem, btn);
    if ((u32)(invIdx - 0x30) >= 8) return;
    changeBagItem(btn, invIdx, itemNo);
}

/* 025B7568 */
void d_save_cpp::dSv_player_bag_item_c::setReserveItemEmpty(u32 btn) {
    setReserveItemChange(btn, dInvSlot_NONE_e);
}

/* 025B7570 */
u8 d_save_cpp::dSv_player_bag_item_c::checkReserveItem(u32 itemNo) {
    u8 ret = 0;
    for (u32 i = 0; i < 8; i++)
        if ((u8)indexed(mReserve, i) == itemNo) ret++;
    return ret;
}

/* 025B75A4 */
u8 d_save_cpp::dSv_player_bag_item_c::checkReserveItemEmpty() {
    return checkReserveItem(dInvSlot_NONE_e);
}

/* 025B75AC */
void d_save_cpp::dSv_player_bag_item_c::setReserveItem(u32 itemNo) {
    if (checkReserveItemEmpty() != 0) {
        for (u32 i = 0; i < 8; i++) {
            if ((u8)indexed(mReserve, i) == dInvSlot_NONE_e) {
                indexed(mReserve, i) = (u8)itemNo;
                return;
            }
        }
    }
}

/* 025B7614 */
void d_save_cpp::dSv_player_get_bag_item_c::init() {
    mBeastFlags = 0;
    mBaitFlags = 0;
    mReserveFlags = 0;
}

/* 025B7628 */
void d_save_cpp::dSv_player_get_bag_item_c::onBeast(u32 no) {
    setByteBit(gabi::ea(&mBeastFlags), no, 8, "d_save.cpp", 0x4F8, "0 <= i_no && i_no < 8");
}

/* 025B7690 */
BOOL d_save_cpp::dSv_player_get_bag_item_c::isBeast(u32 no) {
    return testByteBit(gabi::ea(&mBeastFlags), no, 8, "d_save.cpp", 0x516, "0 <= i_no && i_no < 8");
}

/* 025B7700 */
void d_save_cpp::dSv_player_get_bag_item_c::onBait(u32 no) {
    setByteBit(gabi::ea(&mBaitFlags), no, 8, "d_save.cpp", 0x525, "0 <= i_no && i_no < 8");
}

/* 025B7768 */
BOOL d_save_cpp::dSv_player_get_bag_item_c::isBait(u32 no) {
    return testByteBit(gabi::ea(&mBaitFlags), no, 8, "d_save.cpp", 0x543, "0 <= i_no && i_no < 8");
}

/* 025B77D8 */
void d_save_cpp::dSv_player_get_bag_item_c::onReserve(u32 no) {
    setScalarWordBit(gabi::ea(&mReserveFlags), no, 0x20, "d_save.cpp", 0x552, "0 <= i_no && i_no < 32");
}

/* 025B7840 */
BOOL d_save_cpp::dSv_player_get_bag_item_c::isReserve(u32 no) {
    return testScalarWordBit(gabi::ea(&mReserveFlags), no, 0x20, "d_save.cpp", 0x570, "0 <= i_no && i_no < 32");
}

/* 025B78AC */
void d_save_cpp::dSv_player_bag_item_record_c::init() {
    for (u32 i = 0; i < 8; i++) indexed(mBeastNum, i) = 0;
    for (u32 i = 0; i < 8; i++) indexed(mBaitNum, i) = 0;
    for (u32 i = 0; i < 8; i++) indexed(mReserveNum, i) = 0;
}

/* 025B790C */
void d_save_cpp::dSv_player_collect_c::init() {
    for (u32 i = 0; i < 8; i++) indexed(mCollect, i) = 0;
    field_0xc = 0;
    mSymbol = 0;
    field_0x8 = 0;
    mTriforce = 0;
    mTact = 0;
}

/* 025B7944 */
void d_save_cpp::dSv_player_collect_c::onCollect(u32 field, u32 no) {
    setByteBit(gabi::ea(&indexed(mCollect, field)), no, 8, "d_save.cpp", 0x5A9, "0 <= i_item && i_item < 8");
}

/* 025B79B8 */
void d_save_cpp::dSv_player_collect_c::offCollect(u32 field, u32 no) {
    clearByteBit(gabi::ea(&indexed(mCollect, field)), no, 8, "d_save.cpp", 0x5B8, "0 <= i_item && i_item < 8");
}

/* 025B7A2C */
BOOL d_save_cpp::dSv_player_collect_c::isCollect(u32 field, u32 no) {
    return testByteBit(gabi::ea(&indexed(mCollect, field)), no, 8, "d_save.cpp", 0x5C7, "0 <= i_item && i_item < 8");
}

/* 025B7AA8 */
void d_save_cpp::dSv_player_collect_c::onTact(u32 no) {
    setByteBit(gabi::ea(&mTact), no, 8, "d_save.cpp", 0x603, "0 <= i_no && i_no < 8");
}

/* 025B7B10 */
BOOL d_save_cpp::dSv_player_collect_c::isTact(u32 no) {
    return testByteBit(gabi::ea(&mTact), no, 8, "d_save.cpp", 0x621, "0 <= i_no && i_no < 8");
}

/* 025B7B80 */
void d_save_cpp::dSv_player_collect_c::onTriforce(u32 no) {
    setByteBit(gabi::ea(&mTriforce), no, 8, "d_save.cpp", 0x630, "0 <= i_no && i_no < 8");
}

/* 025B7BE8 */
void d_save_cpp::dSv_player_collect_c::offTriforce(u32 no) {
    clearByteBit(gabi::ea(&mTriforce), no, 8, "d_save.cpp", 0x63F, "0 <= i_no && i_no < 8");
}

/* 025B7C50 */
BOOL d_save_cpp::dSv_player_collect_c::isTriforce(u32 no) {
    return testByteBit(gabi::ea(&mTriforce), no, 8, "d_save.cpp", 0x64E, "0 <= i_no && i_no < 8");
}

/* 025B7CC0 */
void d_save_cpp::dSv_player_collect_c::onSymbol(u32 no) {
    setByteBit(gabi::ea(&mSymbol), no, 8, "d_save.cpp", 0x65D, "0 <= i_no && i_no < 8");
}

/* 025B7D28 */
void d_save_cpp::dSv_player_collect_c::offSymbol(u32 no) {
    clearByteBit(gabi::ea(&mSymbol), no, 8, "d_save.cpp", 0x66C, "0 <= i_no && i_no < 8");
}

/* 025B7D90 */
BOOL d_save_cpp::dSv_player_collect_c::isSymbol(u32 no) {
    return testByteBit(gabi::ea(&mSymbol), no, 8, "d_save.cpp", 0x67B, "0 <= i_no && i_no < 8");
}

/* 025B7E00 */
s32 d_save_cpp::dSv_player_collect_c::getTriforceNum() {
    s32 n = 0;
    for (u32 i = 0; i < 8; i++)
        if (isTriforce((u8)i)) n++;
    return n;
}

}  // namespace d_save_cpp

/* WWHD glue
str 100536B8 "d_save.cpp" in 025B7628
str 100536A0 "0 <= i_no && i_no < 8" in 025B7628
str 100536E0 "d_save.cpp" in 025B7690
str 100536C8 "0 <= i_no && i_no < 8" in 025B7690
str 10053704 "d_save.cpp" in 025B7700
str 100536EC "0 <= i_no && i_no < 8" in 025B7700
str 1005372C "d_save.cpp" in 025B7768
str 10053714 "0 <= i_no && i_no < 8" in 025B7768
str 10053738 "d_save.cpp" in 025B77D8
str 10053744 "0 <= i_no && i_no < 32" in 025B77D8
str 10053760 "d_save.cpp" in 025B7840
str 1005376C "0 <= i_no && i_no < 32" in 025B7840
str 100537A0 "d_save.cpp" in 025B7944
str 10053784 "0 <= i_item && i_item < 8" in 025B7944
str 100537C8 "d_save.cpp" in 025B79B8
str 100537AC "0 <= i_item && i_item < 8" in 025B79B8
str 100537F0 "d_save.cpp" in 025B7A2C
str 100537D4 "0 <= i_item && i_item < 8" in 025B7A2C
str 10053820 "d_save.cpp" in 025B7AA8
str 10053808 "0 <= i_no && i_no < 8" in 025B7AA8
str 10053848 "d_save.cpp" in 025B7B10
str 10053830 "0 <= i_no && i_no < 8" in 025B7B10
str 1005386C "d_save.cpp" in 025B7B80
str 10053854 "0 <= i_no && i_no < 8" in 025B7B80
str 10053890 "d_save.cpp" in 025B7BE8
str 10053878 "0 <= i_no && i_no < 8" in 025B7BE8
str 100538B4 "d_save.cpp" in 025B7C50
str 1005389C "0 <= i_no && i_no < 8" in 025B7C50
str 100538D8 "d_save.cpp" in 025B7CC0
str 100538C0 "0 <= i_no && i_no < 8" in 025B7CC0
str 100538FC "d_save.cpp" in 025B7D28
str 100538E4 "0 <= i_no && i_no < 8" in 025B7D28
str 10053920 "d_save.cpp" in 025B7D90
str 10053908 "0 <= i_no && i_no < 8" in 025B7D90
*/

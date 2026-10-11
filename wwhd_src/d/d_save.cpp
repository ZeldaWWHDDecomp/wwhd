/* d_save — Player status and inventory; sole owner of the five-part save translation unit’s data.
 * GameCube names with verified WWHD layouts and behavior. Unit: d_save. */
#include "bindings.h"
#include "d/d_save_readable.h"

const gc_tu_data gc_tu = { .headerStatics = {0x1047C900, 0x101EADD8, 0x1047C90C} };

namespace d_save_cpp {

// The five source parts share one owner for this translation unit’s static data.
static u8 l_defaultName[8] = {0x4c, 0x69, 0x6e, 0x6b, 0x0, 0x0, 0x0, 0x0};
static u8 l_kg1HighScore = 20;
static cXyz l_offsetPos;
static u32 l_offsetPosGuard;
static u16 l_holdEventReg[17] = {0x95ff, 0x94ff, 0x93ff, 0x92ff, 0x91ff, 0x90ff, 0x8fff, 0x8eff, 0x8dff, 0x8cff, 0xb1ff, 0x9cff, 0x84ff, 0x83ff, 0x82ff, 0x81ff, 0x80ff};
static u16 l_onEventBit[5] = {0x2f08, 0x2f04, 0x2f02, 0x3a01, 0x3401};
static u32 l_shipRaceRupee;
static u32 l_shipRaceResult;

#include "d/d_save_local.h"

// HD: initializes life before max life and refreshes five selected-item buttons.
/* 025B4DC0 */
void d_save_cpp::dSv_player_status_a_c::init() {
    mLife = 12;
    mMaxLife = 12;
    field_0x8 = 0;
    for (u32 i = 0; i < HD_ITEM_BUTTON_COUNT; i++) {
        indexed(mSelectItem, i) = dInvSlot_NONE_e;
        u8 sel = (u8)indexed(savePlayer().mPlayerStatusA.mSelectItem, i);
        refreshSelectedItem(i, sel);
    }
    for (u32 i = 0; i < 4; i++) indexed(mSelectEquip, i) = dInvSlot_NONE_e;
    mRupee = 0;
    mWalletSize = 0;
    mMaxMagic = 0;
    mMagic = 0;
    field_0x15 = 0;
    field_0x6 = 0;
    field_0x16 = 0;
}

// HD: the initial time is 150 rather than the GameCube value of 165.
/* 025B5028 */
void d_save_cpp::dSv_player_status_b_c::init() {
    mDateIPL[1] = 0;
    mTactWindAngleY = 0xFFFF;
    mTactWindAngleX = 0xFFFF;
    mDateIPL[0] = 0;
    mTime = 150.0f;
    mDate = 0;
    field_0x8 = 0.0f;
}

/* strncpy(dst, src, 8) (inline) */
static inline void copyStageName(u32 dst, u32 src) {
    u32 i = 0;
    for (; i < 8; i++) {
        u8 c = (u8)indexed(view<be<u8>>(src), i);
        indexed(view<be<u8>>(dst), i) = c;
        if (c == 0) break;
    }
    for (; i < 8; i++) indexed(view<be<u8>>(dst), i) = 0;
}

// HD: the stage name uses an eight-byte bounded copy.
/* 025B5060 */
void d_save_cpp::dSv_player_return_place_c::init() {
    copyStageName(gabi::ea(mName), gabi::guest_addr("sea"));
    mRoomNo = 0x2C;
    mPoint = 0xCE;
}

/* 025B50DC */
void d_save_cpp::dSv_player_return_place_c::set(u32 name, u32 roomNo, u32 status) {
    u32 len = 0;
    while ((u8)indexed(view<be<u8>>(name), len) != 0) len++;
    if (len > 7) saveAssert("d_save.cpp", 0xBF, "std::strlen(i_name) <= 7");
    copyStageName(gabi::ea(mName), name);
    mRoomNo = (u8)roomNo;
    mPoint = (u8)status;
}

/* 025B51BC */
void d_save_cpp::dSv_player_item_c::init() {
    for (u32 i = 0; i < 0x15; i++) indexed(mItems, i) = dInvSlot_NONE_e;
}

// HD: refreshes the three X/Y/Z buttons after replacing a bottle item.
/* 025B51DC */
void d_save_cpp::dSv_player_item_c::setBottleItemIn(u32 prevItemNo, u32 newItemNo) {
    for (u32 i = 0; i < 4; i++) {
        if ((u8)indexed(mItems, dInvSlot_BOTTLE0_e + i) == prevItemNo) {
            indexed(mItems, dInvSlot_BOTTLE0_e + i) = (u8)newItemNo;
            for (u32 btn = 0; btn < ITEM_BUTTON_COUNT; btn++) {
                u8 sel = (u8)indexed(savePlayer().mPlayerStatusA.mSelectItem, btn);
                if (sel == dInvSlot_BOTTLE0_e + i) refreshSelectedItem(btn, sel);
            }
            return;
        }
    }
}

/* 025B5448 */
void d_save_cpp::dSv_player_item_c::setEmptyBottleItemIn(u32 itemNo) {
    setBottleItemIn(item_data::dItemNo_EMPTY_BOTTLE_e, itemNo);
}

/* 025B5454 */
void d_save_cpp::dSv_player_item_c::setEmptyBottle() {
    u32 b = svbase();
    for (u32 i = 0; i < 4; i++) {
        u32 idx = (u8)(dInvSlot_BOTTLE0_e + i);
        if (getInventoryItem(b, idx) == dInvSlot_NONE_e) {
            setInventoryItem(b, idx, item_data::dItemNo_EMPTY_BOTTLE_e);
            return;
        }
    }
}

/* body shared by both setEquipBottleItemIn (inline) */
static inline void changeBottleItem(dSv_player_item_c* i_this, u32 btn, u8 invIdx, u32 itemNo) {
    indexed(i_this->mItems, invIdx) = (u8)itemNo;
    setInventoryItem(svbase(), invIdx, (u8)itemNo);
    u32 pl = play();
    playInfo(pl)->play.mItemSlot = invIdx;
    playInfo(pl)->play.mItemNo = (u8)itemNo;
    u8 sel = (u8)indexed(savePlayer().mPlayerStatusA.mSelectItem, btn);
    refreshSelectedItem(btn, sel);
}

/* 025B5578 */
void d_save_cpp::dSv_player_item_c::setEquipBottleItemIn(u32 btn, u32 itemNo) {
    u8 invIdx = (u8)indexed(savePlayer().mPlayerStatusA.mSelectItem, btn);
    if ((u32)(invIdx - dInvSlot_BOTTLE0_e) >= 4) return;
    changeBottleItem(this, btn, invIdx, itemNo);
}

/* 025B58B8 */
void d_save_cpp::dSv_player_item_c::setEquipBottleItemEmpty(u32 btn) {
    setEquipBottleItemIn(btn, item_data::dItemNo_EMPTY_BOTTLE_e);
}

/* 025B58C0 */
void d_save_cpp::dSv_player_item_c::setEquipBottleItemIn(u32 itemNo) {
    u32 btn;
    if ((u8)playInfo()->play.mEvtCtrl.mTalkButton == 1) btn = 0;
    else if ((u8)playInfo()->play.mEvtCtrl.mTalkButton == 2) btn = 1;
    else if ((u8)playInfo()->play.mEvtCtrl.mTalkButton == 3) btn = 2;
    else return;
    u8 invIdx = (u8)indexed(savePlayer().mPlayerStatusA.mSelectItem, btn);
    if ((u32)(invIdx - dInvSlot_BOTTLE0_e) >= 4) return;
    changeBottleItem(this, btn, invIdx, itemNo);
}

/* 025B5C4C */
void d_save_cpp::dSv_player_item_c::setEquipBottleItemEmpty() {
    setEquipBottleItemIn(item_data::dItemNo_EMPTY_BOTTLE_e);
}

/* 025B5C54 */
u8 d_save_cpp::dSv_player_item_c::checkEmptyBottle() {
    u8 ret = 0;
    for (u32 i = 0; i < 4; i++)
        if ((u8)indexed(mItems, dInvSlot_BOTTLE0_e + i) == item_data::dItemNo_EMPTY_BOTTLE_e) ret++;
    return ret;
}

/* 025B5C80 */
u8 d_save_cpp::dSv_player_item_c::checkBottle(u32 itemNo) {
    u8 ret = 0;
    for (u32 i = 0; i < 4; i++)
        if ((u8)indexed(mItems, dInvSlot_BOTTLE0_e + i) == itemNo) ret++;
    return ret;
}

/* 025B5CAC */
void d_save_cpp::dSv_player_get_item_c::init() {
    for (u32 i = 0; i < 0x15; i++) indexed(mItemFlags, i) = 0;
}

/* 025B5CCC */
void d_save_cpp::dSv_player_get_item_c::onItem(u32 field, u32 item) {
    if (item >= 8) saveAssert("d_save.cpp", 0x1B4, "0 <= i_item && i_item < 8");
    indexed(mItemFlags, field) = (u8)((u8)indexed(mItemFlags, field) | slw(1, item));
}

/* 025B5D40 */
BOOL d_save_cpp::dSv_player_get_item_c::isItem(u32 field, u32 item) {
    if (item >= 8) saveAssert("d_save.cpp", 0x1D2, "0 <= i_item && i_item < 8");
    return ((u8)indexed(mItemFlags, field) & (u8)slw(1, item)) ? TRUE : FALSE;
}

/* bottle items 0x4F..0x60: byte idx/7, bit 1 + idx%7 */
/* 025B5DBC */
void d_save_cpp::dSv_player_get_item_c::onBottleItem(u32 item) {
    u32 idx = item - item_data::dItemNo_EMPTY_BSHIP_e;
    if (idx > 0x11) return;
    u32 byte = idx / 7, bit = 1 + idx % 7;
    indexed(mItemFlags, byte) = (u8)((u8)indexed(mItemFlags, byte) | (1u << bit));
}

/* 025B5F44 */
BOOL d_save_cpp::dSv_player_get_item_c::isBottleItem(u32 item) {
    u32 idx = item - item_data::dItemNo_EMPTY_BSHIP_e;
    if (idx > 0x11) return FALSE;
    u32 byte = idx / 7, bit = 1 + idx % 7;
    return ((u8)indexed(mItemFlags, byte) & (1u << bit)) ? TRUE : FALSE;
}

/* 025B60D8 */
void d_save_cpp::dSv_player_item_record2_c::init() {
    mPictureNum = 0;
    mArrowNum = 0;
    mBombNum = 0;
}

/* 025B60EC */
void d_save_cpp::dSv_player_item_record_c::init() {
    mTimer = 0;
    mItemRecord2.mArrowNum = 0;
    mItemRecord2.mPictureNum = 0;
    mItemRecord2.mBombNum = 0;
    for (u32 i = 0; i < 3; i++) indexed(mBottleNum, i) = 0;
}

/* 025B6120 */
void d_save_cpp::dSv_player_item_record_c::resetTimer(u32 timer) {
    mTimer = (u16)timer;
    playInfo()->play.mFwaterTimer = 0; /* dComIfGs_stopFwaterTimer */
}

/* 025B614C */
void d_save_cpp::dSv_player_item_record_c::decTimer() {
    if ((u8)playInfo()->play.mFwaterTimer != 1) return;
    u16 v = (u16)mTimer;
    if (v != 0) {
        mTimer = (u16)(v - 1);
        return;
    }
    mTimer = 0;
    playInfo()->play.mFwaterTimer = 0;
}

/* 025B61C8 */
u16 d_save_cpp::dSv_player_item_record_c::getTimer() {
    return (u16)mTimer;
}

/* 025B61D0 */
void d_save_cpp::dSv_player_item_max2_c::init() {
    mReserved1Num = 0;
    mArrowNum = 0;
    mBombNum = 0;
}

/* 025B61E4 */
void d_save_cpp::dSv_player_item_max_c::init() {
    mItemMax2.mArrowNum = 0;
    mItemMax2.mReserved1Num = 0;
    mItemMax2.mBombNum = 0;
    for (u32 i = 0; i < HD_ITEM_BUTTON_COUNT; i++) indexed(field_0x3, i) = 0;
}

}  // namespace d_save_cpp

/* WWHD glue
data l_defaultName 101EADA0
data l_kg1HighScore 1004BD18
data l_offsetPos 1047C91C
data l_offsetPosGuard 1047C928
data l_holdEventReg 101EADA8
data l_onEventBit 101EADCC
data l_shipRaceRupee 101D5F1C
data l_shipRaceResult 101D5F18
str 10053624 "sea" in 025B5060
ext 0273AA24 void saveAssert(const char* file, s32 line, const char* message)
ext 020198D8 f32 saveRandom(f32 max)
ext 025E1C74 void saveAudioOutput(u32 mode)
ext 0273AD10 u32 saveAllocate(u32 size)
ext 028F521C void saveZero(u32 address, u32 size)
ext 0200F9D4 void saveOffsetPosition(u32 out, u32 pos, s32 angle, u32 offset)
ext C000A848 void saveCopy(u32 dst, u32 src, u32 size)
ext C000A858 void saveFill(u32 dst, u32 value, u32 size)
ext 027200D0 u32 saveHdProgress(u32 save)
ext 02720144 u32 savePictureInfo(u32 save)
ext 027262B0 void savePictureCount(u32 picture, u32 count)
str 10053628 "d_save.cpp" in 025B50DC
str 10053634 "std::strlen(i_name) <= 7" in 025B50DC
str 1005366C "d_save.cpp" in 025B5CCC
str 10053650 "0 <= i_item && i_item < 8" in 025B5CCC
str 10053694 "d_save.cpp" in 025B5D40
str 10053678 "0 <= i_item && i_item < 8" in 025B5D40
*/

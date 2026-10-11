/* Shop item, camera and cursor layouts from tww.
 * HD: member-function pointers are eight bytes; camera vectors shift by eight bytes.
 * The cursor embeds the 0x78-byte HD brk animation, shifting its final fields by 0x60. */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/MSL_C/string.h"

struct ShopItems_c__ItemData {
    // HD: these callers read only the low byte of the 32-bit item fields.
    u8 itemNoByte() { return *gabi::at<be<u8>>(gabi::ea(&mItemNo) + 3); }
    u8 mustNotOwnItemByte() { return *gabi::at<be<u8>>(gabi::ea(&mMustNotOwnItemNo) + 3); }
    be<u32> mItemNo;
    be<s32> mMustNotOwnItemNo, mMustOwnItemNo;
    be<u8> mBuyConditions;
    u8 _00D[3];
};
struct __shop_items_set_data {
    be<u32> mpItemData, mShowMsgNo, mBuyMsgNo, m0C;
    be<s32> mCount;
};
struct ShopItems_c__select_list_data {
    be<s16> mLeftIdx, mRightIdx, mUpIdx, mDownIdx;
};
struct ShopItems_c__shop_cam_data {
    cXyz m00, m0C;
    be<f32> m18;
};
struct ShopAction {
    be<s16> mThisDelta, mVtableIndex;
    be<u32> mFunction;
};
struct ShopCam_action_c {
    void Save();
    void move();
    u32 rsh_talk_cam_action();
    u32 ds_normal_cam_action();
    void Reset();
    u32 ds_normal_cam_action_init();
    u32 shop_cam_action_init();
    u32 shop_cam_action();
    u32 rsh_talk_cam_action_init(void* actor, void* desiredCenter, void* desiredEye, f32 fovy);
    /* 0x00 */ ShopAction mCurrActionFunc;
    /* 0x08 */ ShopAction mPreviousAction;
    /* 0x10 */ cXyz m18;
    /* 0x1C */ cXyz m24;
    /* 0x28 */ be<f32> m30;
    /* 0x2C */ cXyz mOrigCenter;
    /* 0x38 */ cXyz mOrigEye;
    /* 0x44 */ be<f32> mOrigFovy;
    /* 0x48 */ be<u16> m50, m52;
    /* 0x4C */ be<s16> m54, mCamDataIdx;
};
struct ShopItems_c {
    // HD: preserve 32-bit guest-address wrapping for unconstrained signed indices.
    be<u32>& itemActorProcessId(s32 index) {
        return *gabi::at<be<u32>>(gabi::ea(&mItemActorProcessIds[0]) + u32(index) * 4);
    }
    be<u8>& itemSoldOutFlag(s32 index) {
        return *gabi::at<be<u8>>(gabi::ea(&mItemIsSoldOut[0]) + u32(index));
    }
    __shop_items_set_data* itemSetData(s32 index) {
        u32 entry = *gabi::at<be<u32>>(u32(mpItemSetList) + u32(index) * 4);
        return gabi::at<__shop_items_set_data>(entry);
    }
    u32 isSoldOutItemAll();
    void setItemSetDataList();
    void setItemSetDataListExplicit(u32 list);
    u32 getSelectItemNo();
    u32 getItemNo(s32 index);
    u32 getSelectItemShowMsg();
    u32 getSelectItemBuyMsg();
    u32 Item_ZoomUp(void* pos);
    void hideSelectItem();
    void SoldOutItem(s32 index);
    u32 getSelectItemPos(void* out);
    u32 getSelectItemBasePos(void* out);
    u32 Item_Move();
    void showItem();
    u32 Item_Wait(s32 index);
    u32 Item_Select(s32 index);
    void createItem(s32 count, s32 room);
    /* 0x00 */ be<s16> mSelectedItemIdx;
    u8 _002[2];
    /* 0x04 */ be<u32> mItemActorProcessIds[8];
    /* 0x24 */ be<u32> mpItemSetList;
    /* 0x28 */ be<u8> mItemIsSoldOut[8];
    /* 0x30 */ cXyz m30;
    /* 0x3C */ be<s16> m3C, mbIsHide, mNumItems, mItemSetListGlobalIdx;
};
struct ShopCursor_c {
    void setPos(void* pos);
    void anm_play();
    void draw();
    /* 0x00 */ cXyz mPos;
    /* 0x0C */ be<u32> mpModels[4];
    /* 0x1C */ be<u32> mpBrkData;
    /* 0x20 */ mDoExt_brkAnm mBrkAnm;
    /* 0x98 */ be<f32> m38, m3C, m40;
    /* 0xA4 */ be<s16> m44;
    u8 _0A6[2];
    /* 0xA8 */ be<f32> m48, m4C, m50;
    /* 0xB4 */ be<u8> m54;
    u8 _0B5[3];
};
enum ShopItems_c__BuyConditions {
    Shop_BuyCond_MUST_OWN_ITEM_e = 0x01,
    Shop_BuyCond_MUST_NOT_OWN_ITEM_e = 0x02,
    Shop_BuyCond_MAXIMUM_e = 0x04,
    Shop_BuyCond_ANY_BOTTLE_e = 0x08,
    Shop_BuyCond_EMPTY_BOTTLE_e = 0x10,
    Shop_BuyCond_RUPEES_e = 0x20,
};
WWHD_OFFSET(ShopItems_c__ItemData, mBuyConditions, 0xC);
WWHD_SIZE(ShopItems_c__ItemData, 0x10);
WWHD_SIZE(__shop_items_set_data, 0x14);
WWHD_SIZE(ShopItems_c__select_list_data, 8);
WWHD_SIZE(ShopItems_c__shop_cam_data, 0x1C);
WWHD_SIZE(ShopAction, 8);
WWHD_OFFSET(ShopCam_action_c, m18, 0x10);
WWHD_OFFSET(ShopCam_action_c, mOrigCenter, 0x2C);
WWHD_OFFSET(ShopCam_action_c, mCamDataIdx, 0x4E);
WWHD_SIZE(ShopCam_action_c, 0x50);
WWHD_OFFSET(ShopItems_c, mItemActorProcessIds, 4);
WWHD_OFFSET(ShopItems_c, mpItemSetList, 0x24);
WWHD_OFFSET(ShopItems_c, m30, 0x30);
WWHD_OFFSET(ShopItems_c, mNumItems, 0x40);
WWHD_SIZE(ShopItems_c, 0x44);
WWHD_OFFSET(ShopCursor_c, mBrkAnm, 0x20);
WWHD_OFFSET(ShopCursor_c, m38, 0x98);
WWHD_OFFSET(ShopCursor_c, m54, 0xB4);
WWHD_SIZE(ShopCursor_c, 0xB8);

// HD: camera vectors are passed by address. Keep the verified 32-bit bank argument.
namespace shop_camera {
inline void Stay(void* camera) { gabi::call<void>(0x02514F44, camera); }
inline void SetTrimSize(void* camera, s32 size) { gabi::call<void>(0x02515280, camera, size); }
inline void Set(void* camera, void* center, void* eye, s32 bank, f32 fovy) {
    gabi::call<void>(0x02514F88, camera, center, eye, bank, fovy);
}
inline void Reset(void* camera, void* center, void* eye, s32 bank, f32 fovy) {
    gabi::call<void>(0x025151BC, camera, center, eye, bank, fovy);
}
} // namespace shop_camera

struct daShopItem_c;
// HD: explicit hidden result pointer preserves the existing temporary allocation.
void shopItem_getCenter(daShopItem_c*, cXyz*);

// HD: unchecked table indices wrap in the 32-bit guest address space.
template <class T> inline u32 shopTableValue(const T& table, u32 index) {
    return gabi::load<u32>(gabi::ea(&table[0]) + index * 4);
}

// HD: the manager adds send-state and default-message fields to the GC flow.
struct ShopMessageHD {
    u8 _000[0x922];
    be<u8> mSendOn;
    u8 _923;
    be<u32> mDefaultMsgNo;
};
WWHD_OFFSET(ShopMessageHD, mSendOn, 0x922);
WWHD_OFFSET(ShopMessageHD, mDefaultMsgNo, 0x924);
u32 shopMessage_getStatus(void*);
void shopMessage_setStatus(void*, u32);
void shopMessage_messageSet(void*, u32, u32);
void shopSound_start(u32);

// HD ABI: preserve full-register results, double precision and original argument widths.
void shop_registerDestructor(void*);
void shop_initializeByte(void*);
void shop_initializeFlag(void*);
u32 shop_positionAdd(void*, void*, cXyz*);
u32 shop_maxCheckRegister(s32, s32);
u32 shop_cursorConstruct(void*, void*, void*, f32);
void shop_addCalcAngleS(void*, s32, s32, s32, s32);
void shop_PSVECSubtract(cXyz*, cXyz*, cXyz*);
f64 shop_squareMagnitude(cXyz*);
f64 shop_sqrt(f64);
void shop_checkTrigger(void*);
s32 shop_nextSelectRegister(s32, void*);
u32 shop_checkLeftTrigger(void*);
u32 shop_checkRightTrigger(void*);
u32 shop_checkUpTrigger(void*);
u32 shop_checkDownTrigger(void*);
u32 shop_padLeftTrigger(s32);
u32 shop_padRightTrigger(s32);
u32 shop_padUpTrigger(s32);
u32 shop_padDownTrigger(s32);
u32 shop_isEventBit(void*, s32);
u32 shop_createShopItem(void*, u32, void*, s32, s32, s32);
s32 shop_targetAngleY(void*, void*);
s32 shop_targetAngleX(void*, void*);
void shop_XYZrotM(void*, s32, s32, s32);

struct ShopCameraViewHD {
    u8 _000[0xD4];
    be<f32> mFovy;
    u8 _0D8[4];
    cXyz mEye, mCenter;
};
WWHD_OFFSET(ShopCameraViewHD, mFovy, 0xD4);
WWHD_OFFSET(ShopCameraViewHD, mEye, 0xDC);
WWHD_OFFSET(ShopCameraViewHD, mCenter, 0xE8);

struct ShopHeaderStaticsHD {
    be<f32> negativePi, positivePi;
    u8 firstObject, secondObject;
    u8 _00A[2];
    be<u32> zero[4];
};
WWHD_OFFSET(ShopHeaderStaticsHD, zero, 0xC);
WWHD_SIZE(ShopHeaderStaticsHD, 0x1C);
u32 shop_brkInitHD(void*, void*, void*, s32, s32, s32, s32, s32, s32, f32);

namespace shop_item {
// tww item names missing from the current shared d_item enum.
enum {
    dItemNo_BOMB_5_e = 0x0B,
    dItemNo_BOW_e = 0x27,
    dItemNo_TOWN_FLOWER_e = 0x8C,
};
} // namespace shop_item

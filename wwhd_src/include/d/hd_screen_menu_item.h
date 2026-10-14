/* HD-only item menu parts. Names are descriptive except the StateID strings. */
#pragma once
#include "d/hd_parts_basic_button.h"
#include "d/hd_parts_lyt.h"
#include "d/hd_msg_text.h"
#include "d/hd_parts_text_pane.h"
#include "hd/hd_ui_02001084.h"
#include "d/d_save.h"
#include "d/hd_save_holders.h"
#include "d/d_picture_box.h"

enum hdItemAnimationId {
    HD_ITEM_ANIM_TOUCH_SLIDE_DECIDE = 7, HD_ITEM_ANIM_COMMAND = 8,
    HD_ITEM_ANIM_ICON_CHANGE = 9, HD_ITEM_ANIM_ARROW_FIRE = 10,
    HD_ITEM_ANIM_ARROW_ICE = 11, HD_ITEM_ANIM_ARROW_LIGHT = 12,
    HD_ITEM_ANIM_FLOAT = 13, HD_ITEM_ANIM_FLOAT_CANCEL = 14,
    HD_ITEM_ANIM_COUNT = 15, HD_ITEM_ANIM_SWITCH = 16,
    HD_ITEM_ANIMATION_COUNT = 17, HD_ITEM_EXTRA_ANIMATION_COUNT = 10
};
enum hdItemAnimationSlot {
    HD_ITEM_SLOT_BUTTON = 0, HD_ITEM_SLOT_TOUCH_DECIDE = 2,
    HD_ITEM_SLOT_ICON_CHANGE = 3, HD_ITEM_SLOT_FLOAT = 4,
    HD_ITEM_SLOT_COUNT = 5, HD_ITEM_SLOT_ARROW_FIRE = 6,
    HD_ITEM_SLOT_ARROW_ICE = 7, HD_ITEM_SLOT_ARROW_LIGHT = 8
};
enum hdItemMode { HD_ITEM_MODE_NORMAL = 0, HD_ITEM_MODE_TOUCH = 1, HD_ITEM_MODE_EQUIPPED = 2 };
enum hdItemCountPhase { HD_ITEM_COUNT_NORMAL = 0, HD_ITEM_COUNT_MAXIMUM = 1, HD_ITEM_COUNT_HIDDEN = 2, HD_ITEM_COUNT_UNINITIALIZED = 3 };
enum hdItemConstants {
    HD_ITEM_NO_ITEM = 0x37, HD_ITEM_TIMED_INVENTORY_ITEM = 0x59,
    HD_ITEM_INVALID_SLOT = 0xFF, HD_ITEM_UNKNOWN_COUNT = 0xFF,
    HD_ITEM_MAPPING_COUNT = 57, HD_ITEM_PANE_GROUP_COUNT = 3,
    HD_ITEM_PANE_VISIBLE = 1, HD_ITEM_PANE_TRANSLATION_DIRTY = 0x10,
    HD_ITEM_WIDE_BUFFER_CAPACITY = 32, HD_ITEM_WIDE_SCAN_LIMIT = 0x40000,
    HD_ITEM_TEXT_SETTER_SLOT = 0xC4 / 4
};
constexpr f32 HD_ITEM_TIMER_UNINITIALIZED = 65535.0f;

struct hdItemPoint_c {
    union { be<f32> x; be<u32> xBits; };
    union { be<f32> y; be<u32> yBits; };
};
// Pure PPC coordinate copies preserve signaling NaN payloads; arithmetic uses x/y.
WWHD_OFFSET(hdItemPoint_c, xBits, 0);
WWHD_OFFSET(hdItemPoint_c, yBits, 4);
WWHD_SIZE(hdItemPoint_c, 8);
// Pane position getters write all XYZ coordinates; callers use the XY prefix.
struct hdItemPosition3_c {
    union { be<f32> x; be<u32> xBits; };
    union { be<f32> y; be<u32> yBits; };
    union { be<f32> z; be<u32> zBits; };
};
WWHD_OFFSET(hdItemPosition3_c, xBits, 0);
WWHD_OFFSET(hdItemPosition3_c, yBits, 4);
WWHD_OFFSET(hdItemPosition3_c, zBits, 8);
WWHD_SIZE(hdItemPosition3_c, 12);
/* the item panes are canonical SRT panes; translation is kept as raw words, written as floats here */
inline void hdItemPaneSetTranslateX(hdPartsPaneSRT_c* pane, f32 x) { reinterpret_cast<be<f32>&>(pane->mTranslationXBits) = x; }
inline void hdItemPaneSetTranslateY(hdPartsPaneSRT_c* pane, f32 y) { reinterpret_cast<be<f32>&>(pane->mTranslationYBits) = y; }
struct hdItemPaneArray_c {
    be<u32> mCount;
    gptr<gptr<hdPartsPaneSRT_c>> mpData;
    gptr<hdPartsPaneSRT_c>* safeSlot(u32 index) {
        return gabi::at<gptr<hdPartsPaneSRT_c>>(gabi::ea(mpData.get()) + (index < mCount ? index : 0) * 4);
    }
    hdPartsPaneSRT_c* at(u32 index) { return *safeSlot(index); }
};
WWHD_SIZE(hdItemPaneArray_c, 8);
WWHD_OFFSET(hdButtonReceiver_l, mEnabled, 0x58);
WWHD_OFFSET(hdButtonPart_c, mTouchEnabled, 0x58);
WWHD_OFFSET(hdButtonPart_c, mEnablePane, 0x5A);
WWHD_OFFSET(hdBasicButton_c, mButton, 0);
WWHD_OFFSET(hdBasicButton_c, mSelectPhase, 0x74);
struct hdItemFader_l { be<f32> mValue; u8 _04[0x14]; };
WWHD_OFFSET(hdItemFader_l, mValue, 0);
WWHD_SIZE(hdItemFader_l, 0x18);
struct hdMenuItemVtbl_l {
    u8 _00[0xDC]; gfn mCanLeaveCursor;
    u8 _E0[4]; gfn mCanEnterTouch;
    u8 _E8[4]; gfn mShouldUnselectTouch;
    u8 _F0[0x10C - 0xF0]; gfn mSelectAction;
    u8 _110[0x244 - 0x110]; gfn mSetActive;
    u8 _248[0x34C - 0x248]; gfn mSetVisible;
    u8 _350[4]; gfn mCachePosition;
    u8 _358[4]; gfn mTranslatePoint;
};
WWHD_OFFSET(hdMenuItemVtbl_l, mCanLeaveCursor, 0xDC);
WWHD_OFFSET(hdMenuItemVtbl_l, mCanEnterTouch, 0xE4);
WWHD_OFFSET(hdMenuItemVtbl_l, mShouldUnselectTouch, 0xEC);
WWHD_OFFSET(hdMenuItemVtbl_l, mSelectAction, 0x10C);
WWHD_OFFSET(hdMenuItemVtbl_l, mSetActive, 0x244);
WWHD_OFFSET(hdMenuItemVtbl_l, mSetVisible, 0x34C);
WWHD_OFFSET(hdMenuItemVtbl_l, mCachePosition, 0x354);
WWHD_OFFSET(hdMenuItemVtbl_l, mTranslatePoint, 0x35C);
struct hdMenuItem_c : hdBasicButton_c {
    hdItemPaneArray_c mPaneGroups[3];
    hdItemPoint_c mPanePosition, mPaneTranslation;
    hdItemPoint_c mPoints[4];
    be<f32> mScale;
    be<u32> mMode, mPhase;
    be<u8> mObservedValue;
    u8 _E1[3];
    be<f32> mLimit;
    be<u32> mItem, mSlot, mPreviousSlot, mAnimationSlot, mOtherSlot;
    be<u8> mVisible, mDragFlag, mFlag;
    u8 _FF;
    hdItemFader_l mInterpolator;

    static hdMenuItem_c* construct(hdMenuItem_c* self);
    static void destroy(hdMenuItem_c* self, s32 flags);
    u32 load(void* resource, u32 arg1, u32 arg2);
    u32 loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
    void preparePane();
    u32 preparePanes();
    void executeButton();
    void updateItem();
    void updateAnimation();
    void updateVisible();
    void resetSelectionAnimation();
    void setSelection(u32 animationSlot, u32 slot);
    void storeSelection(u32 animationSlot, u32 slot);
    void applySelection();
    void updateTimer(u32 item);
    u32 currentCount();
    u32 maximumCount();
    u32 updateCountPhase();
    void updateCountText();
    void updateArrowVariants(u32 slot);
    void setTimerItem(u32 item);
    void drawTimer();
    void setActive(u32 active);
    void finalize();
    void releasePanes();
    void cachePosition();
    void translatePoint(hdItemPoint_c* point);
    void setVisible(u32 visible);
    void decideExit();
    void decideEnter();
    void unselectEnter();
    void unselectExecute();
    void decideExecute();
    void floatCancelExecute();
    void beginCursorSelection(hdItemPoint_c* point);
    void finishDragSelection();
    void finishFloatSelection();
    void moveDragSelection(hdItemPoint_c* point);
    void updateDragSound(hdItemPoint_c* delta);
    void changeItemByDragExecute();
    void beginDragSelection(hdItemPoint_c* from, hdItemPoint_c* to);
    void beginKeySelection(hdItemPoint_c* to);
    void changeItemByKeyExecute();
    void arrowLoopEnter();
    void changeItemByKeyEnter();
    void changeItemByKeyExit();
    void changeItemByDragEnter();
    void changeItemByDragExit();
    void touchSlideDecideEnter();
    void touchSlideDecideExecute();
    void touchSlideDecideCmdEnter();
    void touchSlideDecideCmdExecute();
    void iconChangeEnter();
    void iconChangeExecute();
    void arrowLoopExecute();
    void floatEnter();
    void floatExecute();
    void floatCancelEnter();
    void numEnter();
    void numExecute();
    void switchEnter();
    void switchExecute();
    u32 isDragState();
    u32 isKeyState();
    u32 isCursorState();
    u32 isHighlightPlaying();
    s32 selectedTexture();
    s32 currentTexture();
    void offExecute();
    void onTouchExecute();
    void onCursorExecute();
    void selectTouchExecute();
    void selectTouchEnter();
    void selectCursorEnter();
    void selectCursorExecute();
    void unselectExit();
    void restorePaneVisible();
    void syncSelection(u32 animationSlot, u32 slot);
    void refreshSelection(u32 animationSlot, u32 slot);
    void beginTouchSelection();
    void prepareTouchSelection();
    void updateAnimationOptions();
    void updateDirection(u32 direction);
    u32 classifyDirection(u32 direction, u32 first, u32 second, u32 third, u32 fourth);
    void updateEquippedMode();
    void updateEquippedSelection();
    hdMenuItemVtbl_l* menuVtable() { return (hdMenuItemVtbl_l*)mButton.mVtable.get(); }
    void vSetActive(u32 active) { gabi::call_ptr(menuVtable()->mSetActive.get(), this, active); }
    void vSetVisible(u32 visible) { gabi::call_ptr(menuVtable()->mSetVisible.get(), this, visible); }
    void vCachePosition() { gabi::call_ptr(menuVtable()->mCachePosition.get(), this); }
    hdPartsPaneSRT_c* rootPane() {
        return (hdPartsPaneSRT_c*)mButton.mpLayout->mPanes.mData.get()[0].get();
    }
};
WWHD_OFFSET(hdMenuItem_c, mPaneGroups, 0x8C);
WWHD_OFFSET(hdMenuItem_c, mPanePosition, 0xA4);
WWHD_OFFSET(hdMenuItem_c, mPaneTranslation, 0xAC);
WWHD_OFFSET(hdMenuItem_c, mPoints, 0xB4);
WWHD_OFFSET(hdMenuItem_c, mScale, 0xD4);
WWHD_OFFSET(hdMenuItem_c, mMode, 0xD8);
WWHD_OFFSET(hdMenuItem_c, mPhase, 0xDC);
WWHD_OFFSET(hdMenuItem_c, mObservedValue, 0xE0);
WWHD_OFFSET(hdMenuItem_c, mLimit, 0xE4);
WWHD_OFFSET(hdMenuItem_c, mItem, 0xE8);
WWHD_OFFSET(hdMenuItem_c, mSlot, 0xEC);
WWHD_OFFSET(hdMenuItem_c, mPreviousSlot, 0xF0);
WWHD_OFFSET(hdMenuItem_c, mAnimationSlot, 0xF4);
WWHD_OFFSET(hdMenuItem_c, mOtherSlot, 0xF8);
WWHD_OFFSET(hdMenuItem_c, mVisible, 0xFC);
WWHD_OFFSET(hdMenuItem_c, mDragFlag, 0xFD);
WWHD_OFFSET(hdMenuItem_c, mFlag, 0xFE);
WWHD_OFFSET(hdMenuItem_c, mInterpolator, 0x100);
WWHD_SIZE(hdMenuItem_c, 0x118);
void hdItemAnim_operationD4(hdAnimSet* set, u32 entry, u32 slot);

// Narrow image views used only by this item-screen unit.
struct hdItemSaveGlobal_l {
    u8 _00[0x20];
    dSv_info_c mInfo;
    hdSaveHolders_c mHdSave;
};
WWHD_OFFSET(hdItemSaveGlobal_l, mInfo, 0x20);
WWHD_OFFSET(hdItemSaveGlobal_l, mHdSave, 0x12C0);
WWHD_OFFSET(hdButtonEvent_l, mButton, 8);
struct hdItemCountModes_c { be<u32> mModes[55]; };
struct hdItemProjectile_l { u8 _00[0xCF0]; be<u32> mUsedCapacity; };
WWHD_OFFSET(hdItemProjectile_l, mUsedCapacity, 0xCF0);

struct hdItemMapping_c { be<u32> mSlot, mItem; be<s32> mSelectedTexture, mCurrentTexture; };
WWHD_SIZE(hdItemMapping_c, 0x10);
struct hdItemMappingTable_c { hdItemMapping_c mEntries[57]; };
struct hdMenuAnimationNames_c { hdUiSafeString mNames[10]; };
struct hdMenuGroupNames_c { hdUiSafeString mNames[12]; };
struct hdMenuGroupRemap_c { be<u32> mIndices[10]; };

// Public pane and message string models share their three-word ABI.
static_assert(sizeof(PaneString_l) == sizeof(HdMsgString));
static_assert(offsetof(PaneString_l, mpBuffer) == offsetof(HdMsgString, mpBuffer));
static_assert(offsetof(PaneString_l, mpVTable) == offsetof(HdMsgString, mpVTable));
static_assert(offsetof(PaneString_l, mCapacity) == offsetof(HdMsgString, mCapacity));
static_assert(offsetof(HdMsgText, mLineFlags) + 2 * sizeof(u32) <= sizeof(PaneWork_l));
struct hdItemCountText_c { HdMsgString mString; be<u16> mBuffer[3]; };
WWHD_OFFSET(hdItemCountText_c, mBuffer, 0xC);



struct hdItemQueuedEvent_l { u8 _00[0x30]; hdUiEvent mEvent; };
WWHD_OFFSET(hdItemQueuedEvent_l, mEvent, 0x30);

struct hdMenuPaneNames_c {
    hdUiSafeString mRoot;
    u8 _08[0x64];
    hdUiSafeString mPictures[3];
    hdUiSafeString mTexts[2];
};
WWHD_OFFSET(hdMenuPaneNames_c, mPictures, 0x6C);
WWHD_OFFSET(hdMenuPaneNames_c, mTexts, 0x84);

struct hdItemHeaderLimits_c { be<f32> mFarX, mFarY, mNearX, mNearY; };
WWHD_SIZE(hdItemHeaderLimits_c, 0x10);

struct hdItemHeaderZero_c { be<u32> mWords[4]; };
WWHD_SIZE(hdItemHeaderZero_c, 0x10);

// Compiler-generated header statics of a translation unit: the pi pair and the two one-byte
// objects constructed by the header (sinit_header_statics_rodata's P) ...
struct hdItemHeaderStatics_c { be<f32> mPiBounds[2]; u8 mPrimary; u8 mSecondary; u8 _0A[2]; };
WWHD_OFFSET(hdItemHeaderStatics_c, mPrimary, 8);
WWHD_SIZE(hdItemHeaderStatics_c, 0xC);
// ... and the three __register_global_object descriptors (D, D+0xC, D+0x18) in .data.
struct hdItemGlobalDescriptor_c { be<u32> mWords[3]; };
WWHD_SIZE(hdItemGlobalDescriptor_c, 0xC);
struct hdItemHeaderDescriptors_c { hdItemGlobalDescriptor_c mDescriptors[3]; };
WWHD_SIZE(hdItemHeaderDescriptors_c, 0x24);

WWHD_OFFSET(hdItemPoint_c, x, 0);
WWHD_OFFSET(hdItemPoint_c, y, 4);
WWHD_OFFSET(hdItemPaneArray_c, mCount, 0);
WWHD_OFFSET(hdItemPaneArray_c, mpData, 4);
WWHD_OFFSET(hdButtonEvent_l, mType, 0);
WWHD_OFFSET(hdItemMapping_c, mSlot, 0);
WWHD_OFFSET(hdItemMapping_c, mItem, 4);
WWHD_OFFSET(hdItemMapping_c, mSelectedTexture, 8);
WWHD_OFFSET(hdItemMapping_c, mCurrentTexture, 0xC);
WWHD_OFFSET(hdItemMappingTable_c, mEntries, 0);
WWHD_OFFSET(hdItemCountModes_c, mModes, 0);
WWHD_OFFSET(hdMenuAnimationNames_c, mNames, 0);
WWHD_OFFSET(hdMenuGroupNames_c, mNames, 0);
WWHD_OFFSET(hdMenuGroupRemap_c, mIndices, 0);
WWHD_OFFSET(hdItemCountText_c, mString, 0);
WWHD_OFFSET(hdMenuPaneNames_c, mRoot, 0);
WWHD_OFFSET(hdItemHeaderLimits_c, mFarX, 0);
WWHD_OFFSET(hdItemHeaderLimits_c, mFarY, 4);
WWHD_OFFSET(hdItemHeaderLimits_c, mNearX, 8);
WWHD_OFFSET(hdItemHeaderLimits_c, mNearY, 0xC);
WWHD_OFFSET(hdItemHeaderZero_c, mWords, 0);

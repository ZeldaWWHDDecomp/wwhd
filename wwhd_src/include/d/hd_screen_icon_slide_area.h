/* HD item-slot slide area: three equipped-item buttons and their floating hit panes.
 * Names describe binary behavior; this screen has no GameCube counterpart. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_button.h"
#include "hd/hd_ui_020015C8.h"
#include "hd/hd_ui_02001084.h"
#include "d/hd_input_eventmgr.h"
#include "d/hd_screen_menu_item.h"

enum hdIconSlideAnimation_e {
    ICON_FLOAT_X, ICON_FLOAT_Y, ICON_FLOAT_R,
    ICON_CANCEL_X, ICON_CANCEL_Y, ICON_CANCEL_R,
    ICON_SWITCH_X, ICON_SWITCH_Y, ICON_SWITCH_R,
};
enum hdIconSlideGroup_e {
    ICON_GLOW_X, ICON_GLOW_Y, ICON_GLOW_R,
    ICON_DROP_X, ICON_DROP_Y, ICON_DROP_R,
};

struct hdIconSlideArea_c : hdParts_c {
    static constexpr u32 ITEM_SLOT_COUNT = 3;
    /* 0x50 */ be<s32> mItemPaneCount;
    /* 0x54 */ gptr<gptr<hdPartsPane_c>> mpItemPanes;
    /* 0x58 */ be<s32> mHitPaneCount;
    /* 0x5C */ gptr<gptr<hdWidget_touch_c>> mpHitPanes;
    /* 0x60 */ be<u32> mFloatingSlots;
    /* 0x64 */ be<u32> mGlowSlots;

    static hdIconSlideArea_c* construct(hdIconSlideArea_c* area);
    static void destroy(hdIconSlideArea_c* area, s32 flags);
    void enterFloat();
    void executeFloat();
    void enterFloatCancel();
    void executeFloatCancel();
    void enterSwitch();
    void executeSwitch();
    u32 bindItemPanes();
    u32 createHitPanes();
    u32 load(u32 first, u32 second, hdPartsDesc_c* desc);
    void deleteHitPanes();
    void releaseItemPanes();
    void release();
    void execute();
    void refreshItems();
    void registerHitPanes();
    void startFloat(u32 slot, u32 playSound);
    void cancelFloat(u32 slot);
    void startGlow(u32 slot);
    void cancelGlow(u32 slot);
    u32 findItemByButton(u32 button);
    u32 findHitPane(u32 receiverId);
    Pair32 itemPosition(u32 slot);
    u32 hasItemEvent();
    u32 hasActiveItem();
    u32 hasSelectedItem();
    u32 containsPoint(hdWidget_vec2_c* point);
    void getAnimationSet(gptr<hdAnimSet>* out);
    u32 createChild(hdPartsDesc_c* desc);
    u32 loadLayout(void* name, u32 a, u32 b, u32 c, void* desc);
};
WWHD_OFFSET(hdIconSlideArea_c, mItemPaneCount, 0x50);
WWHD_OFFSET(hdIconSlideArea_c, mpItemPanes, 0x54);
WWHD_OFFSET(hdIconSlideArea_c, mHitPaneCount, 0x58);
WWHD_OFFSET(hdIconSlideArea_c, mpHitPanes, 0x5C);
WWHD_OFFSET(hdIconSlideArea_c, mFloatingSlots, 0x60);
WWHD_OFFSET(hdIconSlideArea_c, mGlowSlots, 0x64);
WWHD_SIZE(hdIconSlideArea_c, 0x68);



struct hdIconSlideArrayStorage_c {
    be<u32> mCount;
    be<u32> mData;
};
WWHD_SIZE(hdIconSlideArrayStorage_c, 8);


struct hdIconSlideGroupNames_c { hdUiSafeString mNames[6]; };
struct hdIconSlideAnimationNames_c { hdUiSafeString mNames[9]; };
struct hdIconSlideGroupIndices_c { be<u32> mIndices[9]; };

/* the slide area's children are the item-screen item buttons (DRC-item_screen); their input receiver is a touch widget */
inline hdWidget_touch_c* iconSlideItemTouch(hdMenuItem_c* item) {
    return reinterpret_cast<hdWidget_touch_c*>(item->mButton.mpReceiver.get());
}
struct hdIconSlidePositionBits_c { be<u32> x, y; };
WWHD_SIZE(hdIconSlidePositionBits_c, 8);

struct hdIconSlideEquipped_l { u8 mPrefix[0x29]; be<u8> mSlots[hdIconSlideArea_c::ITEM_SLOT_COUNT]; };
WWHD_OFFSET(hdIconSlideEquipped_l, mSlots, 0x29);

struct hdIconSlideTouchVtable_l { u8 mPrefix[0xC]; gfn mDestroy; };
WWHD_OFFSET(hdIconSlideTouchVtable_l, mDestroy, 0xC);

struct hdIconSlidePaneArray_c {
    be<s32> mCount;
    gptr<gptr<hdPartsPane_c>> mpData;
};
WWHD_SIZE(hdIconSlidePaneArray_c, 8);
u32 iconSlideBindPanes(hdPartsLayout_c* layout, hdIconSlidePaneArray_c* panes, hdUiSafeString* names);

struct hdIconSlideItemPaneNames_c { hdUiSafeString mNames[hdIconSlideArea_c::ITEM_SLOT_COUNT]; };

struct hdIconSlideChildDesc_l {
    u8 mPrefix[8];
    hdUiSafeString mClassName;
    u8 mNameGap[4];
    hdUiSafeString mPaneName;
};
WWHD_OFFSET(hdIconSlideChildDesc_l, mClassName, 8);
WWHD_OFFSET(hdIconSlideChildDesc_l, mPaneName, 0x14);

u32 iconSlideCreateChildBase(hdParts_c* owner, u32 index, hdPartsDesc_c* desc);

struct hdIconSlideHitWidget_l : hdWidget_touch_c {
    be<u8> mDragEnabled, mRegistered, mReserved;
    u8 mAlign;
    be<u32> mContext;
};
WWHD_OFFSET(hdIconSlideHitWidget_l, mRegistered, 0x59);
WWHD_SIZE(hdIconSlideHitWidget_l, 0x60);
struct hdIconSlideStartup_c {
    be<f32> mMinAngle, mMaxAngle;
    be<u8> mFlags[4];
    be<u32> mZeros[4];
};
WWHD_SIZE(hdIconSlideStartup_c, 0x1C);
struct hdIconSlideDestructorRecords_c { u8 mRecords[3][12]; };
void iconSlideRegisterGlobal(void* record);
void iconSlideInitializeFirstFlag(void* flag);
void iconSlideInitializeSecondFlag(void* flag);

struct hdIconSlidePanePosition_c { be<f32> x, y, z; };
WWHD_SIZE(hdIconSlidePanePosition_c, 12);

/* Unit-local bindings; one ext definition per target across the two sources. */
hdParts_c* iconSlideConstructBase(hdParts_c*);
void iconSlideDestroyBase(hdParts_c*, s32);
void* iconSlideAllocate(u32);
void iconSlideDelete(void*);
void iconSlideUpdateState(hdStateMachine*);
void* iconSlideAllocateOnHeap(u32, void*, u32);
void iconSlideBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
void iconSlideItemStartFloat(hdParts_c*);
void iconSlideItemCancelFloat(hdParts_c*);
void iconSlideStartAnimation(hdAnimSet*, u32, u32);
void iconSlidePlaySound(const char*);
u32 iconSlideReceiverId(hdParts_c*);
u32 iconSlideItemIsActive(hdParts_c*);
u32 iconSlideItemIsSelected(hdParts_c*);
void iconSlideReleaseBase(hdParts_c*);
void iconSlideGetEvent(hdUiEvent*, void*);
void iconSlidePanePosition(hdIconSlidePanePosition_c*, hdPartsPane_c*);
u32 iconSlidePaneContains(void*, hdWidget_vec2_c*);
u32 iconSlideItemTexture(u32);
void iconSlideSetItem(hdParts_c*, u32, u32);
void iconSlideUpdateItem(hdParts_c*);
void iconSlideRefreshItemState(hdParts_c*);
void iconSlideRefreshItemCount(hdParts_c*);
// Legacy button manager current event is +0x30; cking input manager has a different queue layout.
WWHD_OFFSET(hdButtonEventManager_l, mCurrentEvent, 0x30);
WWHD_OFFSET(hdButtonEventManager_l, mCurrentReceiver, 0xD0);
u32 iconSlideCurrentReceiverId(hdButtonEventManager_l*);
void iconSlideRegisterHit(hdButtonEventManager_l*, hdWidget_touch_c*);
hdAnimSetHeap* iconSlideFindHeap(void*, void*);
hdAnimSetHeap* iconSlideCurrentHeap(void*);
hdParts_c* iconSlideControllerItemConstruct(hdParts_c*);
u32 iconSlideControllerItemInitialize(hdParts_c*, u32, u32, hdPartsDesc_c*);
u32 iconSlideCreateChildBase(hdParts_c*, u32, hdPartsDesc_c*);
hdWidget_touch_c* iconSlideConstructHit(hdWidget_touch_c*, hdPartsPane_c*, u32);
void iconSlideRegisterGlobal(void*);
void iconSlideInitializeFirstFlag(void*);
void iconSlideInitializeSecondFlag(void*);
hdPartsPane_c* iconSlideFindPane(hdPartsLayout_c*, hdUiSafeString*, hdUiSafeString*);
hdParts_c* iconSlideMirrorItemConstruct(hdParts_c*);
u32 iconSlideMirrorItemInitialize(hdParts_c*, u32, u32, hdPartsDesc_c*);

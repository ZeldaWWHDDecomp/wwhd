/* HD-only touch-slide overlay. It mirrors the selected item's animations and panes. */
#pragma once
#include "d/hd_screen_btn_item_icon.h"
enum hdTouchAnimationSlot {
    HD_TOUCH_SLOT_BUTTON = 0, HD_TOUCH_SLOT_ICON_CHANGE = 1,
    HD_TOUCH_SLOT_FLOAT = 2, HD_TOUCH_SLOT_COUNT = 3,
    HD_TOUCH_SLOT_ARROW_FIRE = 4, HD_TOUCH_SLOT_ARROW_ICE = 5,
    HD_TOUCH_SLOT_ARROW_LIGHT = 6
};
struct hdTouchSlideDecide_c : hdBasicButton_c {
    hdItemIconContext_c mSource;
    hdItemPaneArray_c mPaneGroups[3];
    static hdTouchSlideDecide_c* construct(hdTouchSlideDecide_c* self);
    static void destroy(hdTouchSlideDecide_c* self, s32 flags);
    u32 load(void* resource, u32 arg1, u32 arg2);
    u32 loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
    void releasePanes();
    u32 preparePanes();
    void finalize();
    void execute();
    void setSource(hdItemIconContext_c* source);
    void syncAnimations();
    void syncPanes();
    void touchSlideDecideEnter(); void touchSlideDecideExecute();
    void commandEnter(); void commandExecute();
    void iconChangeEnter(); void iconChangeExecute();
    void arrowLoopEnter(); void arrowLoopExecute();
    void floatEnter(); void floatExecute();
    void floatCancelEnter(); void floatCancelExecute();
    void numEnter(); void numExecute();
    void switchEnter(); void switchExecute();
};
WWHD_OFFSET(hdTouchSlideDecide_c, mSource, 0x8C);
WWHD_OFFSET(hdTouchSlideDecide_c, mPaneGroups, 0xA4);
WWHD_SIZE(hdTouchSlideDecide_c, 0xBC);

struct hdTouchAnimationNames_c { hdUiSafeString mNames[10]; };
struct hdTouchGroupNames_c { hdUiSafeString mNames[12]; };
struct hdTouchGroupRemap_c { be<u32> mIndices[10]; };

struct hdTouchPaneNames_c {
    hdUiSafeString mRoot;
    u8 _08[0x54]; // the TU's header statics and group names sit in this gap
    hdUiSafeString mPictures[3];
    hdUiSafeString mTexts[2];
};
WWHD_OFFSET(hdTouchPaneNames_c, mPictures, 0x5C);
WWHD_OFFSET(hdTouchPaneNames_c, mTexts, 0x74);

struct hdItemPaneTypeTable_l { u8 _00[0x14]; gfn mGetType; };
struct hdItemTypedPane_l {
    u8 _00[8]; gptr<hdItemPaneTypeTable_l> mTypeVtable;
    hdScreenType* type() { return gabi::call_ptr<hdScreenType*>(mTypeVtable->mGetType.get(), this); }
};
WWHD_OFFSET(hdItemTypedPane_l, mTypeVtable, 8);
WWHD_OFFSET(hdItemPaneTypeTable_l, mGetType, 0x14);

struct hdTouchWideString_l {
    gptr<be<u16>> mpText;
    gptr<hdUiStringTable> mpVtable;
    void assureTermination() { gabi::call_ptr<void>(mpVtable->mAssureTermination.get(), this); }
};
struct hdTouchWideBuffer_c : hdTouchWideString_l {
    be<s32> mCapacity;
    be<u16> mBuffer[HD_ITEM_WIDE_BUFFER_CAPACITY];
};
WWHD_SIZE(hdTouchWideString_l, 8);
WWHD_OFFSET(hdTouchWideBuffer_c, mCapacity, 8);
WWHD_OFFSET(hdTouchWideBuffer_c, mBuffer, 0xC);
WWHD_SIZE(hdTouchWideBuffer_c, 0x4C);

WWHD_OFFSET(hdTouchPaneNames_c, mRoot, 0);
WWHD_OFFSET(hdTouchWideString_l, mpText, 0);
WWHD_OFFSET(hdTouchWideString_l, mpVtable, 4);
WWHD_OFFSET(hdTouchAnimationNames_c, mNames, 0);
WWHD_OFFSET(hdTouchGroupNames_c, mNames, 0);
WWHD_OFFSET(hdTouchGroupRemap_c, mIndices, 0);

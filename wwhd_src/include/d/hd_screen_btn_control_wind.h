/** HD wind-control button, built on the canonical BasicButton. */
#pragma once
#include "d/hd_parts_basic_button.h"
struct hdBtnControlWind_c : hdBasicButton_c {
    void offExecute();
    void selectTouchExecute();
    void onTouchExecute();
    void selectTouchEnter();
    void selectCursorEnter();
    void selectCursorExecute();
    u32 setup(void* archive, void* resource, void* flags);
};
WWHD_SIZE(hdBtnControlWind_c, 0x8C);
void windButtonDestroyBase(hdBtnControlWind_c* button, u32 flags);
hdBtnControlWind_c* windButtonConstructBase(hdBtnControlWind_c* button);
u32 windButtonSetupBase(hdBtnControlWind_c* button, hdUiSafeString* name, void* archive, void* resource, void* flags);
void windButtonSynchronizeFocus(hdBtnControlWind_c* button);

struct hdWindButtonVtbl_l : hdPartsVtbl_c {
    u8 _C8[0xDC - 0xC8];
    gfn mAcceptCursor;
    u8 _E0[4];
    gfn mAcceptTouch;
    u8 _E8[4];
    gfn mAcceptTouchRelease;
    u8 _F0[4];
    gfn mAcceptEvent;
    u8 _F8[0x10C - 0xF8];
    gfn mBeginTouch;
    u8 _110[0x254 - 0x110];
    gfn mPrepareDecide;
};
WWHD_OFFSET(hdWindButtonVtbl_l, mAcceptCursor, 0xDC);
WWHD_OFFSET(hdWindButtonVtbl_l, mAcceptTouch, 0xE4);
WWHD_OFFSET(hdWindButtonVtbl_l, mAcceptTouchRelease, 0xEC);
WWHD_OFFSET(hdWindButtonVtbl_l, mAcceptEvent, 0xF4);
WWHD_OFFSET(hdWindButtonVtbl_l, mBeginTouch, 0x10C);
WWHD_OFFSET(hdWindButtonVtbl_l, mPrepareDecide, 0x254);
void windButtonCopyEvent(hdButtonEvent_l* out, hdButtonEvent_l* event);
u32 windButtonGetMode(hdButtonEventManager_l* manager);
u32 windButtonInputEnabled(hdBtnControlWind_c* button);
void windButtonPendingRelease(hdBtnControlWind_c* button);

u32 windButtonReceiverId(hdBtnControlWind_c* button);
void windButtonClearFocus(hdBtnControlWind_c* button);
u32 windButtonIsDecideEvent(u32 type);

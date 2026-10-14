/** HD floor button: a NoDecideButton part (canonical NoDecideButton_l base). */
#pragma once
#include "d/hd_screen_btn_dungeon_item_icon.h"
#include "d/hd_input_eventmgr.h"
struct hdFloorButtonVtbl_l : hdPartsVtbl_c {
    u8 mFocusGap[0xC]; gfn mAcquireFocus;
    u8 mDispatchGap[4]; gfn mDispatch;
    u8 mAcceptGap[4]; gfn mAcceptEvent;
    u8 mCursorGap[0x24]; gfn mCursorMove;
};
struct hdFloorButton_c : hdDungeonButton_c {
    be<u8> mFloor, mSelectedFloor;
    u8 mTail[2];
    static hdFloorButton_c* construct(hdFloorButton_c*);
    static void destroy(hdFloorButton_c*, u32 flags);
    u32 load(void*, void*, void*);
    u32 isSelected();
    void updateSelection(); void update();
    void offExecute(); void onCursorExecute(); void selectCursorEnter(); void selectCursorExecute();
    u32 acquireFocus(u32 event); u32 dispatch(u32 event);
    hdFloorButtonVtbl_l* vtbl() { return gabi::at<hdFloorButtonVtbl_l>(gabi::ea(mVtable.get())); }
};
WWHD_OFFSET(hdFloorButtonVtbl_l, mAcquireFocus, 0xD4);
WWHD_OFFSET(hdFloorButtonVtbl_l, mDispatch, 0xDC);
WWHD_OFFSET(hdFloorButtonVtbl_l, mAcceptEvent, 0xE4);
WWHD_OFFSET(hdFloorButtonVtbl_l, mCursorMove, 0x10C);
WWHD_OFFSET(hdFloorButton_c, mFloor, 0x78);
WWHD_OFFSET(hdFloorButton_c, mSelectedFloor, 0x79);
WWHD_SIZE(hdFloorButton_c, 0x7C);
u32 floorButtonInputEnabled();
u32 floorButtonCanDispatch(u32 event);
u32 floorButtonEventMask(u32 event);
u32 floorButtonReceiverAccepts(hdButtonReceiver_l* receiver, u32 mask);
hdPartsStateId_c* floorButtonCurrentState(hdStateMachine* machine);
void floorButtonCopyEvent(hdButtonEvent_l* dst, hdButtonEvent_l* src);
void floorButtonBuildEvent(hdButtonEvent_l* event, u32 type, u32 receiver, void* payload);
void floorButtonQueueEvent(void* queue, hdButtonEvent_l* event);
WWHD_SIZE(hdFloorButtonVtbl_l, 0x110);
WWHD_OFFSET(hdButtonPart_c, mTouchEnabled, 0x58);
WWHD_OFFSET(hdButtonEvent_l, mType, 0);
WWHD_OFFSET(hdButtonEvent_l, mReceiverId, 4);
WWHD_SIZE(hdButtonEvent_l, 0x28);
WWHD_OFFSET(hdButtonEventManager_l, mCurrentEvent, 0x30);
WWHD_OFFSET(cking::ui::input::EventMgr, mEventQueue, 0x14);

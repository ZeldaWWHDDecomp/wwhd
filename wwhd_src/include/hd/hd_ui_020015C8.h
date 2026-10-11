/* HD: UI widgets, GamePad pointer and event queue; no GameCube counterpart. */
#pragma once
#include "bindings.h"
struct hdWidget_c {
    gptr<void> mpPrevious, mpNext, mpOwner; be<u32> mNodeState;
    be<u32> mSerial; be<u32> mVtable;
};
WWHD_SIZE(hdWidget_c, 0x18);
WWHD_OFFSET(hdWidget_c, mSerial, 0x10);
WWHD_OFFSET(hdWidget_c, mVtable, 0x14);
struct hdWidget_vec2_c { be<f32> x, y; };
struct hdWidget_cursor_c { be<s32> x, y; };
WWHD_SIZE(hdWidget_vec2_c, 8);
WWHD_SIZE(hdWidget_cursor_c, 8);
struct hdWidget_pointer_c {
    u8 _000[0x110]; hdWidget_vec2_c mRawPosition;
    u8 _118[0x130-0x118]; be<u32> mId; u8 _134[8]; be<u32> mSecondaryVtable;
    u8 _140[0x170-0x140]; be<u32> mVtable; u8 _174[0x194-0x174];
    hdWidget_cursor_c mCursor; hdWidget_vec2_c mPosition, mCenter, mScale;
    void update();
    void setScreen(f32 width, f32 height, f32 targetWidth, f32 targetHeight);
};
WWHD_SIZE(hdWidget_pointer_c, 0x1B4);
WWHD_OFFSET(hdWidget_pointer_c, mRawPosition, 0x110);
WWHD_OFFSET(hdWidget_pointer_c, mId, 0x130);
WWHD_OFFSET(hdWidget_pointer_c, mSecondaryVtable, 0x13C);
WWHD_OFFSET(hdWidget_pointer_c, mVtable, 0x170);
WWHD_OFFSET(hdWidget_pointer_c, mCursor, 0x194);
WWHD_OFFSET(hdWidget_pointer_c, mPosition, 0x19C);
WWHD_OFFSET(hdWidget_pointer_c, mCenter, 0x1A4);
WWHD_OFFSET(hdWidget_pointer_c, mScale, 0x1AC);
struct hdWidget_input_c { u8 _000[0x140]; gptr<hdWidget_pointer_c> mpPointer; };
// HD: names of the input/touch/frame flags below are provisional; verified offsets and widths are fixed.
struct hdWidget_touch_c : hdWidget_c {
    gptr<void> mpPane; be<u32> mState; be<u16> mFlags, mInputFlags;
    be<u32> mInputId; be<u16> mTouchFlags, mPreviousTouchFlags;
    be<u32> mPointerId; be<u16> mFrame, mPreviousFrame; be<u32> mEventFlags;
    be<u32> mUserData[8];
    u32 hitTest(f32 x, f32 y);
    u32 isStateZero();
    u32 isStateOne();
    void setUserData(u32 index, u32 value);
    u32 getUserData(u32 index);
};
WWHD_SIZE(hdWidget_touch_c, 0x58);
WWHD_OFFSET(hdWidget_touch_c, mpPane, 0x18);
WWHD_OFFSET(hdWidget_touch_c, mState, 0x1C);
WWHD_OFFSET(hdWidget_touch_c, mFlags, 0x20);
WWHD_OFFSET(hdWidget_touch_c, mInputFlags, 0x22);
WWHD_OFFSET(hdWidget_touch_c, mInputId, 0x24);
WWHD_OFFSET(hdWidget_touch_c, mTouchFlags, 0x28);
WWHD_OFFSET(hdWidget_touch_c, mPreviousTouchFlags, 0x2A);
WWHD_OFFSET(hdWidget_touch_c, mPointerId, 0x2C);
WWHD_OFFSET(hdWidget_touch_c, mFrame, 0x30);
WWHD_OFFSET(hdWidget_touch_c, mPreviousFrame, 0x32);
WWHD_OFFSET(hdWidget_touch_c, mEventFlags, 0x34);
WWHD_OFFSET(hdWidget_touch_c, mUserData, 0x38);
struct hdWidget_eventBox_c;
struct hdWidget_list_c { gptr<hdWidget_eventBox_c> mpPrevious, mpNext; be<u32> mCount; };
struct hdWidget_event_c { be<u32> mType, mActive; u8 _08[0x28-8]; };
struct hdWidget_eventBox_c {
    gptr<hdWidget_eventBox_c> mpPrevious, mpNext; hdWidget_event_c mEvent;
    gptr<hdWidget_list_c> mpOwner; be<u32> mFlags;
};
WWHD_SIZE(hdWidget_event_c, 0x28);
WWHD_SIZE(hdWidget_eventBox_c, 0x38);
WWHD_OFFSET(hdWidget_eventBox_c, mEvent, 8);
WWHD_OFFSET(hdWidget_eventBox_c, mpOwner, 0x30);
struct hdWidget_queue_c {
    gptr<void> mpHeap; hdWidget_list_c mList; gptr<hdWidget_eventBox_c> mpBoxes;
    be<u32> mBoxCount; be<u32> mVtable;
    s32 initialize(void* heap, u32 count);
    void finalize();
    s32 pop(hdWidget_event_c* out);
    void link(hdWidget_eventBox_c* box);
    s32 post(hdWidget_event_c* event);
    void clear();
};
WWHD_SIZE(hdWidget_list_c, 0xC);
WWHD_SIZE(hdWidget_queue_c, 0x1C);
WWHD_OFFSET(hdWidget_queue_c, mList, 4);
WWHD_OFFSET(hdWidget_queue_c, mpBoxes, 0x10);
WWHD_OFFSET(hdWidget_queue_c, mBoxCount, 0x14);
WWHD_OFFSET(hdWidget_queue_c, mVtable, 0x18);
struct hdWidget_paneTransform_c { be<u32> mWords[5]; };
WWHD_SIZE(hdWidget_paneTransform_c, 0x14);

void hdWidget_destroyPointerBase(hdWidget_pointer_c* pointer, u32 flags);
void hdWidget_destroyPointerEntry(hdWidget_pointer_c* pointer, s32 flags);
void hdWidget_constructPointerBase(hdWidget_pointer_c* pointer);
void hdWidget_updatePointerBase(hdWidget_pointer_c* pointer);
hdWidget_c* hdWidget_constructBase(hdWidget_c* widget);
void hdWidget_destroyBase(hdWidget_c* widget, u32 flags);
void hdWidget_getPaneTransform(void* pane, hdWidget_paneTransform_c* transform);
u32 hdWidget_hitPane(void* pane, hdWidget_vec2_c* point);
void* hdWidget_allocateBoxes(u32 size, void* heap, u32 alignment);
hdWidget_eventBox_c* hdWidget_constructBoxes(void* allocation, u32 count, u32 stride, u32 constructor, u32 flags);
void hdWidget_destroyBoxes(hdWidget_eventBox_c* boxes, s32 count, u32 stride, u32 destructor, u32 flags, u32 reserved);
hdWidget_eventBox_c* hdWidget_popList(hdWidget_list_c* list);
void hdWidget_copyEvent(hdWidget_event_c* out, hdWidget_event_c* event);
void hdWidget_clearEvent(hdWidget_event_c* event);
void hdWidget_unlinkNode(hdWidget_eventBox_c* box);
void hdWidget_appendNode(hdWidget_list_c* list, hdWidget_eventBox_c* box);
void hdWidget_linkBox(hdWidget_queue_c* queue, hdWidget_eventBox_c* box);

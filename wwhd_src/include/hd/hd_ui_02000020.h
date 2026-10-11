/** HD UI input receiver layouts. No GameCube counterpart; private decomp source. */
#pragma once
#include "wwhd.h"
union hdReceiverCoordinate { be<f32> mValue; be<u32> mBits; };
struct hdReceiverPosition { hdReceiverCoordinate mX; hdReceiverCoordinate mY; };
struct hdReceiverPoint { be<u32> mId; hdReceiverPosition mPosition; };
struct hdReceiverEvent { u8 _opaque[0x28]; };
struct hdReceiverQueue { u8 _opaque[0x1C]; };
struct hdReceiverAccumulator { u8 _opaque[0x28]; };
struct hdReceiverInput {
    be<u32> mTrigger; be<u32> mRelease; be<u32> mRepeat; be<u32> mOptions;
    u8 _opaque10[0xFC]; be<u32> mHold;
    u8 _opaque110[0x8C]; hdReceiverPosition mPointerPosition;
};
struct hdReceiverInputHolder { gptr<hdReceiverInput> mInput; };
struct hdReceiverItemTable {
    u8 _opaque00[0x14]; gfn mContainsPoint; u8 _opaque18[4]; gfn mAcceptKindB;
    u8 _opaque20[4]; gfn mAcceptKindA;
};
struct hdReceiverItem { u8 _opaque00[0x10]; be<u32> mId; gptr<hdReceiverItemTable> mVtable; };
struct hdReceiverNode {
    gptr<hdReceiverNode> mPrevious; gptr<hdReceiverNode> mNext;
    gptr<hdReceiverItem> mItem; be<u32> mRemoved;
};
struct hdReceiverTable {
    u8 _opaque00[0x24];
    gfn mDispatch;
    u8 _opaque28[0x4];
    gfn mTouchOn;
    u8 _opaque30[0x4];
    gfn mTouchHold;
    u8 _opaque38[0x4];
    gfn mTouchOff;
    u8 _opaque40[0x4];
    gfn mButton5;
    u8 _opaque48[0x4];
    gfn mButton8;
    u8 _opaque50[0x4];
    gfn mButton11;
    u8 _opaque58[0x4];
    gfn mButton12;
    u8 _opaque60[0x4];
    gfn mButton6;
    u8 _opaque68[0x4];
    gfn mButton7;
    u8 _opaque70[0x4];
    gfn mButton9;
    u8 _opaque78[0x4];
    gfn mButton10;
    u8 _opaque80[0x4];
    gfn mButton15;
    u8 _opaque88[0x4];
    gfn mButton13;
    u8 _opaque90[0x4];
    gfn mButton16;
    u8 _opaque98[0x4];
    gfn mButton14;
    u8 _opaqueA0[0x4];
    gfn mCheckHold16;
    u8 _opaqueA8[0x4];
    gfn mCheckHold17;
    u8 _opaqueB0[0x4];
    gfn mCheckHold18;
    u8 _opaqueB8[0x4];
    gfn mCheckHold19;
    u8 _opaqueC0[0x4];
    gfn mCheckHold0;
    u8 _opaqueC8[0x24];
    gfn mCheckTrigger0;
    u8 _opaqueF0[0x24];
    gfn mCheckRelease0;
    u8 _opaque118[0x24];
    gfn mCheckRepeat0;
    u8 _opaque140[0x4];
    gfn mCheckKindA;
    u8 _opaque148[0x4];
    gfn mCheckKindB;
};
struct hdInputReceiver {
    gptr<hdReceiverNode> mPrevious; gptr<hdReceiverNode> mNext; be<u32> mCount;
    be<u32> mFocusedId; be<s32> mSuspendCounter;
    hdReceiverQueue mQueue; hdReceiverAccumulator mAccumulator; gptr<hdReceiverTable> mVtable;
    hdReceiverNode* sentinel() { return (hdReceiverNode*)this; }
    hdReceiverQueue* queue() {
        // HD: preserve wrapping guest addresses when passing embedded subobjects.
        return gabi::at<hdReceiverQueue>(gabi::ea(this) + offsetof(hdInputReceiver, mQueue));
    }
    hdReceiverAccumulator* accumulator() {
        return gabi::at<hdReceiverAccumulator>(gabi::ea(this) + offsetof(hdInputReceiver, mAccumulator));
    }
};
enum hdReceiverEventKind {
    UI_EVENT_TOUCH_ON = 2, UI_EVENT_TOUCH_HOLD = 3, UI_EVENT_TOUCH_OFF = 4,
    // HD: button-to-device mappings are unconfirmed; retain the verified event IDs.
    UI_EVENT_BUTTON_5 = 5,
    UI_EVENT_BUTTON_6 = 6,
    UI_EVENT_BUTTON_7 = 7,
    UI_EVENT_BUTTON_8 = 8,
    UI_EVENT_BUTTON_9 = 9,
    UI_EVENT_BUTTON_10 = 10,
    UI_EVENT_BUTTON_11 = 11,
    UI_EVENT_BUTTON_12 = 12,
    UI_EVENT_BUTTON_13 = 13,
    UI_EVENT_BUTTON_14 = 14,
    UI_EVENT_BUTTON_15 = 15,
    UI_EVENT_BUTTON_16 = 16,
};
enum : u32 {
    UI_INPUT_POINTER_VALID = 1, UI_INPUT_POINTER_BUTTON = 0x8000,
    UI_INPUT_HOLD_16 = 1 << 16, UI_INPUT_HOLD_17 = 1 << 17,
    UI_INPUT_HOLD_18 = 1 << 18, UI_INPUT_HOLD_19 = 1 << 19,
};
void hdReceiverConstructQueue(hdReceiverQueue*);
void hdReceiverConstructAccumulator(hdReceiverAccumulator*);
void hdReceiverDestroyQueue(hdReceiverQueue*, u32);
u32 hdReceiverQueueOperationFirst(hdReceiverQueue*);
u32 hdReceiverQueueOperationSecond(hdReceiverQueue*);
void hdReceiverEraseNode(hdReceiverNode*);
void hdReceiverFlushAccumulator(hdReceiverAccumulator*);
void hdReceiverEnqueueAccumulator(hdReceiverQueue*, hdReceiverAccumulator*);
void hdReceiverDispatchQueue(hdReceiverQueue*);
void hdReceiverConstructEvent(hdReceiverEvent*, s32, s32, u32);
void hdReceiverEnqueueEvent(hdReceiverQueue*, hdReceiverEvent*);
u32 hdReceiverInputId(hdReceiverInput*);
void hdReceiverPost(hdInputReceiver*, s32, s32, u32);
WWHD_SIZE(hdReceiverCoordinate, 4);
WWHD_SIZE(hdReceiverPosition, 8);
WWHD_SIZE(hdReceiverPoint, 0xC);
WWHD_SIZE(hdReceiverEvent, 0x28);
WWHD_SIZE(hdReceiverQueue, 0x1C);
WWHD_SIZE(hdReceiverAccumulator, 0x28);
WWHD_SIZE(hdReceiverInput, 0x1A4);
WWHD_OFFSET(hdReceiverInput, mHold, 0x10C);
WWHD_OFFSET(hdReceiverInput, mPointerPosition, 0x19C);
WWHD_SIZE(hdReceiverInputHolder, 4);
WWHD_SIZE(hdReceiverItemTable, 0x28);
WWHD_SIZE(hdReceiverItem, 0x18);
WWHD_SIZE(hdReceiverNode, 0x10);
WWHD_SIZE(hdReceiverTable, 0x150);
WWHD_SIZE(hdInputReceiver, 0x5C);
WWHD_OFFSET(hdInputReceiver, mQueue, 0x14);
WWHD_OFFSET(hdInputReceiver, mAccumulator, 0x30);
WWHD_OFFSET(hdInputReceiver, mVtable, 0x58);

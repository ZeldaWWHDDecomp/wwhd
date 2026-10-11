/** HD UI input receiver: dispatches button/touch events and maintains focus. No GameCube counterpart. */
#include "bindings.h"
#include "hd/hd_ui_02000020.h"
static hdReceiverInputHolder* sReceiverInputHolder;
static hdReceiverPosition sReceiverFallbackPosition;
static f32 sReceiverPointerLimit;
static hdReceiverTable sReceiverTable;

/* 02000020 */
static hdInputReceiver* constructInputReceiver(hdInputReceiver* i_this) {
    if (i_this == NULL) {
        i_this = (hdInputReceiver*)operator_new(sizeof(hdInputReceiver));
        if (i_this == NULL) return NULL;
    }
    i_this->mVtable = &sReceiverTable;
    i_this->mNext = i_this->sentinel();
    i_this->mCount = 0;
    i_this->mPrevious = i_this->sentinel();
    i_this->mFocusedId = 0;
    i_this->mSuspendCounter = 0;
    hdReceiverConstructQueue(i_this->queue());
    hdReceiverConstructAccumulator(i_this->accumulator());
    return i_this;
}
/* 020000B4 */
static void destroyInputReceiver(hdInputReceiver* i_this, s32 flags) {
    if (i_this == NULL) return;
    hdReceiverDestroyQueue(i_this->queue(), 2);
    if (flags & 1) operator_delete(i_this);
}
/* 02000108 */
static u32 receiverQueueOperationFirst(hdInputReceiver* i_this) {
    return hdReceiverQueueOperationFirst(i_this->queue());
}
/* 02000110 */
static u32 receiverQueueOperationSecond(hdInputReceiver* i_this) {
    return hdReceiverQueueOperationSecond(i_this->queue());
}
static void sweepReceiverItems(hdInputReceiver* i_this) {
    hdReceiverNode* node = i_this->mNext;
    while (node != i_this->sentinel()) {
        hdReceiverNode* current = node;
        u32 removed = current->mRemoved;
        node = current->mNext;
        if (removed != 0) {
            current->mRemoved = 0;
            hdReceiverEraseNode(current);
            i_this->mCount = (u32)i_this->mCount - 1;
        }
    }
}
/* 02000118 */
static void updateInputReceiver(hdInputReceiver* i_this) {
    hdReceiverInputHolder* holder = sReceiverInputHolder;
    if (holder == NULL) { sweepReceiverItems(i_this); return; }
    hdReceiverInput* input = holder->mInput;
    if (input == NULL) { sweepReceiverItems(i_this); return; }
    s32 count = i_this->mSuspendCounter;
    if (count > 0) {
        i_this->mSuspendCounter = count - 1;
        sweepReceiverItems(i_this);
    }
    gabi::call_ptr(i_this->mVtable->mDispatch.get(), i_this, input);
    sweepReceiverItems(i_this);
}
/* 020002E0 */
static void flushInputReceiver(hdInputReceiver* i_this) {
    hdReceiverFlushAccumulator(i_this->accumulator());
    hdReceiverEnqueueAccumulator(i_this->queue(), i_this->accumulator());
    hdReceiverDispatchQueue(i_this->queue());
}
/* 02000324 */
static void postReceiverEvent(hdInputReceiver* i_this, s32 kind, s32 id, u32 argument) {
    hdReceiverEvent event;
    hdReceiverConstructEvent(&event, kind, id, argument);
    hdReceiverEnqueueEvent(i_this->queue(), &event);
}
static hdReceiverPosition* receiverPosition(hdReceiverInput* input) {
    return ((u32)input->mOptions & UI_INPUT_POINTER_VALID) ? &input->mPointerPosition : &sReceiverFallbackPosition;
}
/* 02000360 */
static s32 dispatchReceiverInput(hdInputReceiver* i_this, hdReceiverInput* input) {
    if (input == NULL) return 1;
    hdReceiverPosition* position = receiverPosition(input);
    f32 lim = sReceiverPointerLimit;
    f32 x = position->mX.mValue;
    f32 y = position->mY.mValue;
    if (x > lim && y > lim) { // HD: both strict comparisons must pass; NaNs use the release branch.
        if ((u32)input->mTrigger & UI_INPUT_POINTER_BUTTON) { gabi::call_ptr(i_this->mVtable->mTouchOn.get(), i_this, input); return 1; }
        if ((u32)input->mHold & UI_INPUT_POINTER_BUTTON) { gabi::call_ptr(i_this->mVtable->mTouchHold.get(), i_this, input); return 1; }
    } else {
        if ((u32)input->mRelease & UI_INPUT_POINTER_BUTTON) { gabi::call_ptr(i_this->mVtable->mTouchOff.get(), i_this, input); return 1; }
        i_this->mFocusedId = 0;
    }
    if (gabi::call_ptr<u32>(i_this->mVtable->mCheckHold16.get(), i_this, input) != 0) {
        if (gabi::call_ptr<u32>(i_this->mVtable->mCheckHold18.get(), i_this, input) != 0) { gabi::call_ptr(i_this->mVtable->mButton6.get(), i_this); return 1; }
        if (gabi::call_ptr<u32>(i_this->mVtable->mCheckHold19.get(), i_this, input) != 0) { gabi::call_ptr(i_this->mVtable->mButton7.get(), i_this); return 1; }
        gabi::call_ptr(i_this->mVtable->mButton5.get(), i_this);
        return 1;
    }
    if (gabi::call_ptr<u32>(i_this->mVtable->mCheckHold17.get(), i_this, input) != 0) {
        if (gabi::call_ptr<u32>(i_this->mVtable->mCheckHold18.get(), i_this, input) != 0) { gabi::call_ptr(i_this->mVtable->mButton9.get(), i_this); return 1; }
        if (gabi::call_ptr<u32>(i_this->mVtable->mCheckHold19.get(), i_this, input) != 0) { gabi::call_ptr(i_this->mVtable->mButton10.get(), i_this); return 1; }
        gabi::call_ptr(i_this->mVtable->mButton8.get(), i_this);
        return 1;
    }
    if (gabi::call_ptr<u32>(i_this->mVtable->mCheckHold18.get(), i_this, input) != 0) { gabi::call_ptr(i_this->mVtable->mButton11.get(), i_this); return 1; }
    if (gabi::call_ptr<u32>(i_this->mVtable->mCheckHold19.get(), i_this, input) != 0) { gabi::call_ptr(i_this->mVtable->mButton12.get(), i_this); return 1; }
    if (gabi::call_ptr<u32>(i_this->mVtable->mCheckTrigger0.get(), i_this, input) != 0) { gabi::call_ptr(i_this->mVtable->mButton13.get(), i_this); return 1; }
    if (gabi::call_ptr<u32>(i_this->mVtable->mCheckRepeat0.get(), i_this, input) != 0) { gabi::call_ptr(i_this->mVtable->mButton14.get(), i_this); return 1; }
    if (gabi::call_ptr<u32>(i_this->mVtable->mCheckHold0.get(), i_this, input) != 0) { gabi::call_ptr(i_this->mVtable->mButton15.get(), i_this); return 1; }
    if (gabi::call_ptr<u32>(i_this->mVtable->mCheckRelease0.get(), i_this, input) != 0) { gabi::call_ptr(i_this->mVtable->mButton16.get(), i_this); return 1; }
    return 0;
}

/* inlined into receiverTouchOn, receiverTouchHold, receiverTouchOff */
static bool receiverHit(hdInputReceiver* i_this, hdReceiverItem* item, hdReceiverPoint* point) {
    if (gabi::call_ptr<u32>(i_this->mVtable->mCheckKindA.get(), i_this, point) != 0) {
        if (gabi::call_ptr<u32>(item->mVtable->mAcceptKindA.get(), item) == 0) return false;
    }
    if (gabi::call_ptr<u32>(i_this->mVtable->mCheckKindB.get(), i_this, point) != 0) {
        if (gabi::call_ptr<u32>(item->mVtable->mAcceptKindB.get(), item) == 0) return false;
    }
    f32 x = point->mPosition.mX.mValue;
    f32 y = point->mPosition.mY.mValue;
    return gabi::call_ptr<u32>(item->mVtable->mContainsPoint.get(), item, x, y) != 0;
}
/* 02000870 */
static s32 receiverTouchOn(hdInputReceiver* i_this, hdReceiverInput* input) {
    if (input == NULL) { i_this->mFocusedId = 0; return 0; }
    hdReceiverPoint point;
    u32 id = hdReceiverInputId(input);
    hdReceiverPosition* position = receiverPosition(input);
    point.mId = id;
    // HD: copy the position words without changing signalling-NaN payloads.
    point.mPosition.mX.mBits = position->mX.mBits;
    point.mPosition.mY.mBits = position->mY.mBits;
    for (hdReceiverNode* node = i_this->mNext; node != i_this->sentinel(); node = node->mNext) {
        hdReceiverItem* item = node->mItem;
        if (receiverHit(i_this, item, &point)) {
            hdReceiverPost(i_this, UI_EVENT_TOUCH_ON, item->mId, 0);
            i_this->mFocusedId = item->mId;
            return 1;
        }
    }
    return 0;
}
/* 02000A04 */
static s32 receiverTouchHold(hdInputReceiver* i_this, hdReceiverInput* input) {
    if (input == NULL) { i_this->mFocusedId = 0; return 0; }
    hdReceiverPoint point;
    u32 id = hdReceiverInputId(input);
    hdReceiverPosition* position = receiverPosition(input);
    point.mId = id;
    point.mPosition.mX.mBits = position->mX.mBits;
    point.mPosition.mY.mBits = position->mY.mBits;
    for (hdReceiverNode* node = i_this->mNext; node != i_this->sentinel(); node = node->mNext) {
        hdReceiverItem* item = node->mItem;
        if (receiverHit(i_this, item, &point)) {
            hdReceiverPost(i_this, UI_EVENT_TOUCH_HOLD, item->mId, 0);
            return 1;
        }
    }
    return 0;
}
/* 02000B8C */
static s32 receiverTouchOff(hdInputReceiver* i_this, hdReceiverInput* input) {
    if (input != NULL) {
        hdReceiverPoint point;
        u32 id = hdReceiverInputId(input);
        hdReceiverNode* node = i_this->mNext;
        // HD: release uses the supplied coordinates even when the valid-position bit is clear.
        point.mPosition.mX.mBits = input->mPointerPosition.mX.mBits;
        point.mId = id;
        point.mPosition.mY.mBits = input->mPointerPosition.mY.mBits;
        for (; node != i_this->sentinel(); node = node->mNext) {
            hdReceiverItem* item = node->mItem;
            if (receiverHit(i_this, item, &point)) {
                u32 itemId = item->mId;
                if (i_this->mFocusedId == itemId) hdReceiverPost(i_this, UI_EVENT_TOUCH_OFF, itemId, 0);
                i_this->mFocusedId = 0;
                return 1;
            }
        }
    }
    i_this->mFocusedId = 0;
    return 0;
}
/* 02000D00 */
static s32 postReceiverButton5(hdInputReceiver* i_this) {
    hdReceiverPost(i_this, UI_EVENT_BUTTON_5, -1, 0);
    return 0;
}
/* 02000D30 */
static s32 postReceiverButton8(hdInputReceiver* i_this) {
    hdReceiverPost(i_this, UI_EVENT_BUTTON_8, -1, 0);
    return 0;
}
/* 02000D60 */
static s32 postReceiverButton11(hdInputReceiver* i_this) {
    hdReceiverPost(i_this, UI_EVENT_BUTTON_11, -1, 0);
    return 0;
}
/* 02000D90 */
static s32 postReceiverButton12(hdInputReceiver* i_this) {
    hdReceiverPost(i_this, UI_EVENT_BUTTON_12, -1, 0);
    return 0;
}
/* 02000DC0 */
static s32 postReceiverButton6(hdInputReceiver* i_this) {
    hdReceiverPost(i_this, UI_EVENT_BUTTON_6, -1, 0);
    return 0;
}
/* 02000DF0 */
static s32 postReceiverButton7(hdInputReceiver* i_this) {
    hdReceiverPost(i_this, UI_EVENT_BUTTON_7, -1, 0);
    return 0;
}
/* 02000E20 */
static s32 postReceiverButton9(hdInputReceiver* i_this) {
    hdReceiverPost(i_this, UI_EVENT_BUTTON_9, -1, 0);
    return 0;
}
/* 02000E50 */
static s32 postReceiverButton10(hdInputReceiver* i_this) {
    hdReceiverPost(i_this, UI_EVENT_BUTTON_10, -1, 0);
    return 0;
}
/* 02000E80 */
static s32 postReceiverButton15(hdInputReceiver* i_this) {
    hdReceiverPost(i_this, UI_EVENT_BUTTON_15, -1, 0);
    return 0;
}
/* 02000EB0 */
static s32 postReceiverButton13(hdInputReceiver* i_this) {
    hdReceiverPost(i_this, UI_EVENT_BUTTON_13, -1, 0);
    return 0;
}
/* 02000EE0 */
static s32 postReceiverButton16(hdInputReceiver* i_this) {
    hdReceiverPost(i_this, UI_EVENT_BUTTON_16, -1, 0);
    return 0;
}
/* 02000F10 */
static s32 postReceiverButton14(hdInputReceiver* i_this) {
    hdReceiverPost(i_this, UI_EVENT_BUTTON_14, -1, 0);
    return 0;
}
/* 02000F40 */
static u32 receiverHold16(hdInputReceiver* i_this, hdReceiverInput* input) {
    return ((u32)input->mHold & UI_INPUT_HOLD_16) != 0;
}
/* 02000F4C */
static u32 receiverHold17(hdInputReceiver* i_this, hdReceiverInput* input) {
    return ((u32)input->mHold & UI_INPUT_HOLD_17) != 0;
}
/* 02000F58 */
static u32 receiverHold18(hdInputReceiver* i_this, hdReceiverInput* input) {
    return ((u32)input->mHold & UI_INPUT_HOLD_18) != 0;
}
/* 02000F64 */
static u32 receiverHold19(hdInputReceiver* i_this, hdReceiverInput* input) {
    return ((u32)input->mHold & UI_INPUT_HOLD_19) != 0;
}
/* 02000F70 */
static u32 receiverHold0(hdInputReceiver* i_this, hdReceiverInput* input) {
    return (u32)input->mHold & 1;
}
/* 02000F7C */
static u32 receiverTrigger0(hdInputReceiver* i_this, hdReceiverInput* input) {
    return (u32)input->mTrigger & 1;
}
/* 02000F88 */
static u32 receiverRelease0(hdInputReceiver* i_this, hdReceiverInput* input) {
    return (u32)input->mRelease & 1;
}
/* 02000F94 */
static u32 receiverRepeat0(hdInputReceiver* i_this, hdReceiverInput* input) {
    return (u32)input->mRepeat & 1;
}
/* 02000FA0 */
static u32 receiverIsKindA(hdInputReceiver* i_this, hdReceiverInput* input) {
    // HD: these predicates interpret the first event word as a kind rather than button bits.
    u32 kind = input->mTrigger;
    if (kind >= 1 && (kind <= 2 || kind == 8)) return 1;
    return 0;
}
/* 02000FCC */
static u32 receiverIsKindB(hdInputReceiver* i_this, hdReceiverInput* input) {
    u32 kind = input->mTrigger;
    return (kind == 4 || kind == 7) ? 1 : 0;
}
/* 02000FF0 */
static void __sinit_hd_input_receiver_cpp() {
    sinit_header_statics_rodata(0x101FF060, 0x1018C0C0, 0x101FF06C, 0x10000004);
}
/* WWHD glue (tools/verify/autoglue.py): private HD receiver constants and queue calls
 data sReceiverInputHolder 1018C12C
 data sReceiverFallbackPosition 1049FFA0
 data sReceiverPointerLimit 10000000
 data sReceiverTable 10000010
 ext 02001CB0 void hdReceiverConstructQueue(hdReceiverQueue*)
 ext 020012CC void hdReceiverConstructAccumulator(hdReceiverAccumulator*)
 ext 02001D38 void hdReceiverDestroyQueue(hdReceiverQueue*, u32)
 ext 02001D4C u32 hdReceiverQueueOperationFirst(hdReceiverQueue*)
 ext 02001E18 u32 hdReceiverQueueOperationSecond(hdReceiverQueue*)
 ext 0273B310 void hdReceiverEraseNode(hdReceiverNode*)
 ext 02001480 void hdReceiverFlushAccumulator(hdReceiverAccumulator*)
 ext 02001E80 void hdReceiverEnqueueAccumulator(hdReceiverQueue*, hdReceiverAccumulator*)
 ext 0200209C void hdReceiverDispatchQueue(hdReceiverQueue*)
 ext 02001338 void hdReceiverConstructEvent(hdReceiverEvent*, s32, s32, u32)
 ext 02001FF4 void hdReceiverEnqueueEvent(hdReceiverQueue*, hdReceiverEvent*)
 ext 0200190C u32 hdReceiverInputId(hdReceiverInput*)
 ext 02000324 void hdReceiverPost(hdInputReceiver*, s32, s32, u32)
*/

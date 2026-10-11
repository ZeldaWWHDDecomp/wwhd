/* HD: widget serials, GamePad cursor mapping, pane hit testing and queued UI events. */
#include "hd/hd_ui_020015C8.h"
static u32 sWidgetSerial;
static u32 sArrayCookieSize;
static hdWidget_cursor_c sInitialCursor;
static f32 sZero = 0.0f;
static f32 sOne = 1.0f;
static f32 sHalf = 0.5f;
static const u32 widgetVtable = 0x100001B0;
static const u32 pointerVtable = 0x10000220;
static const u32 pointerSecondaryVtable = 0x10000258;
static const u32 pointerBaseVtable = 0x101432B0;
static const u32 touchVtable = 0x10000270;
static const u32 queueVtable = 0x100002B0;
static const u32 eventBoxConstructor = 0x02001084;
static const u32 eventBoxDestructor = 0x02001110;

/* 020015C8 */
static hdWidget_c* hdWidget_construct(hdWidget_c* widget) {
    if (!widget) {
        widget = static_cast<hdWidget_c*>(operator_new(sizeof(hdWidget_c)));
        if (!widget) return nullptr;
    }
    widget->mpOwner = widget;
    widget->mpNext = nullptr;
    widget->mpPrevious = nullptr;
    widget->mNodeState = 0;
    widget->mVtable = widgetVtable;
    u32 serial = sWidgetSerial;
    sWidgetSerial = serial + 1;
    widget->mSerial = serial;
    if (!sWidgetSerial) sWidgetSerial = 1;
    return widget;
}
/* 02001668 */
static void hdWidget_destroy(hdWidget_c* widget, s32 flags) { if (widget && (flags & 1)) operator_delete(widget); }
/* 0200167C */
static void hdWidget_initStatics() { sinit_header_statics_rodata(0x101FF0EC, 0x1018C17C, 0x101FF0F8, 0x100001A8); }
/* 02001710 */
static u32 hdWidget_defaultFalse() { return 0; }
/* 02001718 */
static u32 hdWidget_defaultTrue() { return 1; }
/* 02001720 */
static u32 hdWidget_defaultEnabled() { return 1; }
/* 02001728 */
static void hdWidget_destroyPointer(hdWidget_pointer_c* pointer, s32 flags) {
    if (!pointer) return;
    pointer->mSecondaryVtable = pointerBaseVtable;
    hdWidget_destroyPointerBase(pointer, 0);
    if (flags & 1) operator_delete(pointer);
}
/* 02001788 */
static void hdWidget_destroyPointerSecondary(void* secondary, s32 flags) {
    hdWidget_destroyPointerEntry(gabi::at<hdWidget_pointer_c>(gabi::ea(secondary) - 0x130), flags);
}
/* 02001790 */
static hdWidget_pointer_c* hdWidget_constructPointer(hdWidget_pointer_c* pointer) {
    if (!pointer) {
        pointer = static_cast<hdWidget_pointer_c*>(operator_new(sizeof(hdWidget_pointer_c)));
        if (!pointer) return nullptr;
    }
    hdWidget_constructPointerBase(pointer);
    pointer->mVtable = pointerVtable;
    pointer->mSecondaryVtable = pointerSecondaryVtable;
    pointer->mCursor.x = s32(sInitialCursor.x);
    pointer->mCursor.y = s32(sInitialCursor.y);
    // HD: this member constructor's allocation branch remains reachable when its guest address wraps.
    hdWidget_vec2_c* position = gabi::at<hdWidget_vec2_c>(gabi::ea(pointer) + 0x19C);
    if (!position) position = static_cast<hdWidget_vec2_c*>(operator_new(sizeof(hdWidget_vec2_c)));
    if (position) {
        s32 x = sInitialCursor.x, y = sInitialCursor.y;
        position->x = f32(f64(x));
        position->y = f32(f64(y));
    }
    f32 zero = sZero, one = sOne;
    pointer->mCenter.x = zero;
    pointer->mCenter.y = zero;
    pointer->mScale.x = one;
    pointer->mScale.y = one;
    return pointer;
}
/* 0200188C */
void hdWidget_pointer_c::update() {
    hdWidget_updatePointerBase(this);
    f32 centerY = mCenter.y, rawY = mRawPosition.y, centerX = mCenter.x;
    f32 deltaY = gabi::fsubs_ppc(rawY, centerY);
    f32 rawX = mRawPosition.x, scaleY = mScale.y;
    f32 deltaX = gabi::fsubs_ppc(rawX, centerX);
    f32 scaleX = mScale.x;
    f32 y = gabi::fmuls_ppc(deltaY, scaleY);
    f32 x = gabi::fmuls_ppc(deltaX, scaleX);
    mPosition.x = x;
    mPosition.y = y;
    mCursor.y = gabi::ftoi(y);
    mCursor.x = gabi::ftoi(x);
}
/* 0200190C */
static u32 hdWidget_getInputPointerId(hdWidget_input_c* input) {
    hdWidget_pointer_c* pointer = input->mpPointer;
    if (!pointer) return 0;
    return pointer->mId;
}
static f32 hdWidget_divide(f32 a, f32 b) {
    // HD: match PowerPC's first-NaN propagation and single-precision division.
    if (a != a) return gabi::ppc_qnan(a);
    if (b != b) return gabi::ppc_qnan(b);
    return f32(f64(a) / f64(b));
}
/* 02001924 */
void hdWidget_pointer_c::setScreen(f32 width, f32 height, f32 targetWidth, f32 targetHeight) {
    f32 half = sHalf, zero = sZero;
    mCenter.x = width * half;
    mCenter.y = height * half;
    if (targetWidth == zero || targetHeight == zero) return;
    mScale.y = -hdWidget_divide(targetHeight, height);
    mScale.x = hdWidget_divide(targetWidth, width);
}
/* 0200196C */
static void hdWidget_initPointerStatics() { sinit_header_statics_rodata(0x101FF108, 0x1018C1A0, 0x101FF114, 0x10000218); }
/* 02001A00 */
static hdWidget_touch_c* hdWidget_constructTouch(hdWidget_touch_c* widget, void* pane) {
    if (!widget) {
        widget = static_cast<hdWidget_touch_c*>(operator_new(sizeof(hdWidget_touch_c)));
        if (!widget) return nullptr;
    }
    hdWidget_constructBase(widget);
    widget->mpPane = pane;
    widget->mPointerId = 0;
    widget->mState = 1;
    widget->mInputFlags = 0;
    widget->mVtable = touchVtable;
    widget->mFlags = 0;
    widget->mTouchFlags = 0;
    widget->mPreviousTouchFlags = 0;
    widget->mEventFlags = 0;
    widget->mInputId = 0;
    widget->mPreviousFrame = 0;
    widget->mFrame = 0;
    // HD: allocation is observable when the embedded array wraps to null; its result is unused.
    if (gabi::ea(widget) + 0x38 == 0) operator_new(0x20);
    for (u32 i = 0; i < 8; ++i) widget->mUserData[i] = 0;
    return widget;
}
/* 02001ABC */
static void hdWidget_destroyTouch(hdWidget_touch_c* widget, s32 flags) {
    if (!widget) return;
    hdWidget_destroyBase(widget, 0);
    if (flags & 1) operator_delete(widget);
}
/* 02001B10 */
u32 hdWidget_touch_c::hitTest(f32 x, f32 y) {
    void* pane = mpPane;
    if (!pane) return 0;
    gabi::Local<hdWidget_paneTransform_c> transform;
    hdWidget_getPaneTransform(pane, transform);
    gabi::Local<hdWidget_vec2_c> point;
    point->y = y;
    pane = mpPane;
    point->x = x;
    return hdWidget_hitPane(pane, point);
}
/* 02001BC0 */
u32 hdWidget_touch_c::isStateZero() { return mState == 0 ? 1 : 0; }
/* 02001BD0 */
u32 hdWidget_touch_c::isStateOne() { return mState == 1 ? 1 : 0; }
/* 02001BE4 */
void hdWidget_touch_c::setUserData(u32 index, u32 value) { mUserData[index < 8 ? index : 0] = value; }
/* 02001C00 */
u32 hdWidget_touch_c::getUserData(u32 index) { return mUserData[index < 8 ? index : 0]; }
/* 02001C1C */
static void hdWidget_initTouchStatics() { sinit_header_statics_rodata(0x101FF124, 0x1018C1C4, 0x101FF130, 0x10000268); }
/* 02001CB0 */
static hdWidget_queue_c* hdWidget_constructQueue(hdWidget_queue_c* queue) {
    if (!queue) {
        queue = static_cast<hdWidget_queue_c*>(operator_new(sizeof(hdWidget_queue_c)));
        if (!queue) return nullptr;
    }
    queue->mpHeap = nullptr;
    queue->mVtable = queueVtable;
    // HD: retain the list member's separate null-address allocation path.
    hdWidget_list_c* list = gabi::at<hdWidget_list_c>(gabi::ea(queue) + 4);
    if (!list) list = static_cast<hdWidget_list_c*>(operator_new(sizeof(hdWidget_list_c)));
    if (list) {
        list->mpNext.v = gabi::ea(list);
        list->mCount = 0;
        list->mpPrevious.v = gabi::ea(list);
    }
    queue->mBoxCount = 0;
    queue->mpBoxes = nullptr;
    return queue;
}
/* 02001D38 */
static void hdWidget_destroyQueue(hdWidget_queue_c* queue, s32 flags) { if (queue && (flags & 1)) operator_delete(queue); }
/* 02001D4C */
s32 hdWidget_queue_c::initialize(void* heap, u32 count) {
    mpHeap = heap;
    if (!heap) return 0;
    mBoxCount = count;
    if (!count) return 0;
    u32 cookie = sArrayCookieSize;
    void* allocation = hdWidget_allocateBoxes(count * sizeof(hdWidget_eventBox_c) + cookie, mpHeap, 4);
    if (!allocation) { mpBoxes = nullptr; return 0; }
    hdWidget_eventBox_c* boxes = hdWidget_constructBoxes(gabi::at<void>(gabi::ea(allocation) + sArrayCookieSize), count, sizeof(hdWidget_eventBox_c), eventBoxConstructor, 0);
    mpBoxes = boxes;
    return boxes ? 1 : 0;
}
/* 02001E18 */
void hdWidget_queue_c::finalize() {
    hdWidget_eventBox_c* boxes = mpBoxes;
    if (boxes) {
        hdWidget_destroyBoxes(boxes, -1, sizeof(hdWidget_eventBox_c), eventBoxDestructor, 1, 0);
        mpBoxes = nullptr;
    }
    mBoxCount = 0;
    mpHeap = nullptr;
}
/* 02001E80 */
s32 hdWidget_queue_c::pop(hdWidget_event_c* out) {
    hdWidget_eventBox_c* box = hdWidget_popList(gabi::at<hdWidget_list_c>(gabi::ea(this) + 4));
    if (box) box->mpOwner = nullptr;
    if (!box) return 0;
    hdWidget_copyEvent(out, &box->mEvent);
    if (box->mpOwner) {
        box->mpOwner = nullptr;
        hdWidget_unlinkNode(box);
        mList.mCount = u32(mList.mCount) - 1;
    }
    hdWidget_clearEvent(&box->mEvent);
    return 1;
}
/* 02001F44 */
void hdWidget_queue_c::link(hdWidget_eventBox_c* box) {
    hdWidget_list_c* list = gabi::at<hdWidget_list_c>(gabi::ea(this) + 4);
    if (box->mpOwner) {
        box->mpOwner = nullptr;
        hdWidget_unlinkNode(box);
        mList.mCount = u32(mList.mCount) - 1;
        hdWidget_list_c* owner = box->mpOwner;
        if (owner) {
            box->mpOwner = nullptr;
            hdWidget_unlinkNode(box);
            owner->mCount = u32(owner->mCount) - 1;
        }
    }
    box->mpOwner = list;
    hdWidget_appendNode(list, box);
    list->mCount = u32(list->mCount) + 1;
}
/* 02001FF4 */
s32 hdWidget_queue_c::post(hdWidget_event_c* event) {
    hdWidget_eventBox_c* boxes = mpBoxes;
    if (!boxes) return 0;
    u32 count = mBoxCount;
    for (u32 i = 0, offset = 0; i < count; ++i, offset += sizeof(hdWidget_eventBox_c)) {
        hdWidget_eventBox_c* box = gabi::at<hdWidget_eventBox_c>(gabi::ea(boxes) + offset);
        if (!box->mEvent.mActive) {
            hdWidget_copyEvent(&box->mEvent, event);
            hdWidget_linkBox(this, gabi::at<hdWidget_eventBox_c>(gabi::ea(mpBoxes) + offset));
            return 1;
        }
    }
    return 0;
}
/* 0200209C */
void hdWidget_queue_c::clear() {
    hdWidget_list_c* list = gabi::at<hdWidget_list_c>(gabi::ea(this) + 4);
    hdWidget_eventBox_c* node = mList.mpNext;
    while (gabi::ea(node) != gabi::ea(list)) {
        hdWidget_eventBox_c* current = node;
        hdWidget_list_c* owner = current->mpOwner;
        node = current->mpNext;
        if (owner) {
            current->mpOwner = nullptr;
            hdWidget_unlinkNode(current);
            list->mCount = u32(list->mCount) - 1;
        }
    }
    hdWidget_eventBox_c* boxes = mpBoxes;
    if (!boxes) return;
    u32 i = 0;
    if (i >= u32(mBoxCount)) return;
    for (u32 offset = 0;;) {
        hdWidget_clearEvent(gabi::at<hdWidget_event_c>(gabi::ea(boxes) + offset + 8));
        offset += sizeof(hdWidget_eventBox_c);
        ++i;
        if (i >= u32(mBoxCount)) break;
        boxes = mpBoxes;
    }
}
/* 02002168 */
static void hdWidget_initQueueStatics() { sinit_header_statics_rodata(0x101FF140, 0x1018C1E8, 0x101FF14C, 0x100002A8); }

/* WWHD glue
data sWidgetSerial 1018C178 nocheck
data sArrayCookieSize 101FCD08 nocheck
data sInitialCursor 1049FFA8 nocheck
data sZero 10000210
data sOne 10000200
data sHalf 10000214
ext 0273D0E0 void hdWidget_destroyPointerBase(hdWidget_pointer_c* pointer, u32 flags)
ext 02001728 void hdWidget_destroyPointerEntry(hdWidget_pointer_c* pointer, s32 flags)
ext 0273CDD4 void hdWidget_constructPointerBase(hdWidget_pointer_c* pointer)
ext 0273CE94 void hdWidget_updatePointerBase(hdWidget_pointer_c* pointer)
ext 020015C8 hdWidget_c* hdWidget_constructBase(hdWidget_c* widget)
ext 02001668 void hdWidget_destroyBase(hdWidget_c* widget, u32 flags)
ext 02876368 void hdWidget_getPaneTransform(void* pane, hdWidget_paneTransform_c* transform)
ext 0287F05C u32 hdWidget_hitPane(void* pane, hdWidget_vec2_c* point)
ext 0273B0D4 void* hdWidget_allocateBoxes(u32 size, void* heap, u32 alignment)
ext 028EFF8C hdWidget_eventBox_c* hdWidget_constructBoxes(void* allocation, u32 count, u32 stride, u32 constructor, u32 flags)
ext 028F0164 void hdWidget_destroyBoxes(hdWidget_eventBox_c* boxes, s32 count, u32 stride, u32 destructor, u32 flags, u32 reserved)
ext 0273B494 hdWidget_eventBox_c* hdWidget_popList(hdWidget_list_c* list)
ext 0200143C void hdWidget_copyEvent(hdWidget_event_c* out, hdWidget_event_c* event)
ext 02001480 void hdWidget_clearEvent(hdWidget_event_c* event)
ext 0273B310 void hdWidget_unlinkNode(hdWidget_eventBox_c* box)
ext 0273B2F0 void hdWidget_appendNode(hdWidget_list_c* list, hdWidget_eventBox_c* box)
ext 02001F44 void hdWidget_linkBox(hdWidget_queue_c* queue, hdWidget_eventBox_c* box)
*/

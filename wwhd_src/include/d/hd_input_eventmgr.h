/* HD: GamePad touch and pointer-event dispatch, receiver focus, dragging and inertia.
 * No GameCube counterpart. Observed cking::ui::input::EventMgr layout and descriptive
 * member names. */
#pragma once
#include "gc_api/gc_api.h"

namespace cking::ui::input {
struct Receiver;
struct ReceiverList;
union Vec2 {
    struct {
        be<f32> x, y;
    } value;
    be<u32> words[2];
};
WWHD_SIZE(Vec2, 8);
struct ReceiverNode {
    gptr<ReceiverNode> mPrevious, mNext;
    gptr<Receiver> mpReceiver;
    gptr<ReceiverList> mpOwner;
};
WWHD_OFFSET(ReceiverNode, mpOwner, 0xC);
WWHD_SIZE(ReceiverNode, 0x10);
struct ReceiverList {
    gptr<ReceiverNode> mLast, mFirst;
    be<u32> mCount, mReceiverId;
};
WWHD_SIZE(ReceiverList, 0x10);
struct ReceiverVtable {
    u8 prefix[0x14];
    gfn mContains;
    u8 gapAccept[0x3C - 0x18];
    gfn mAccepts;
    u8 gapDrag[4];
    gfn mCanDrag;
    u8 gapDrop[4];
    gfn mCanDrop;
    u8 gapSwipe[4];
    gfn mCanSwipe;
};
WWHD_OFFSET(ReceiverVtable, mCanDrag, 0x44);
WWHD_OFFSET(ReceiverVtable, mCanDrop, 0x4C);
WWHD_OFFSET(ReceiverVtable, mCanSwipe, 0x54);
WWHD_SIZE(ReceiverVtable, 0x58);
struct ReceiverLayout {
    u8 prefix[0x48];
    Mtx34 mTransform;
};
WWHD_SIZE(ReceiverLayout, 0x78);
struct Receiver {
    u8 prefix[0x10];
    be<u32> mId;
    gptr<ReceiverVtable> mpVtable;
    gptr<ReceiverLayout> mpLayout;
};
WWHD_OFFSET(Receiver, mpVtable, 0x14);
WWHD_SIZE(Receiver, 0x1C);
struct Event {
    be<u32> mHold, mRelease, mTrigger, mPointerFlags;
    u8 gapRepeat[0x10C - 0x10];
    be<u32> mRepeat;
    u8 gapStick[8];
    Vec2 mStick;
    u8 gapTouch[0x19C - 0x120];
    Vec2 mTouch;
};
WWHD_OFFSET(Event, mRepeat, 0x10C);
WWHD_OFFSET(Event, mStick, 0x118);
WWHD_OFFSET(Event, mTouch, 0x19C);
WWHD_SIZE(Event, 0x1A4);
struct EventMgrVtable {
    gfn mSlots[0x214 / 4];
};
WWHD_SIZE(EventMgrVtable, 0x214);
struct Disposer {
    u8 prefix[0xC];
    be<u32> mVtable;
};
WWHD_SIZE(Disposer, 0x10);
struct EventMgr : ReceiverList {
    void prepare();
    void commitId();
    void setMode(u32 mode);
    void calc();
    void forward();
    void setFocus(u32 v);
    void setGrab(u32 v);
    void setHover(u32 v);
    void reset();
    u32 getMode();
    void setParams(u32 a, u32 b);
    void setModeDefault(u32 mode);
    void setNextMode(u32 v);
    u8 isModeChanged();
    u32 getParamA();
    u32 getParamB();
    u32 getHover();
    Pair32 touchFromStart();
    Pair32 padTouchFromStart();
    Pair32 padTouchFromPrev();
    u32 getId();
    u32 dispatch(u32 ev);
    u32 touchDown(u32 ev);
    u32 findTarget();
    u32 touchMove(u32 ev);
    void bounce();
    u32 touchUp(u32 ev);
    u32 sendButton0();
    u32 sendButton1();
    u32 sendButton3();
    u32 sendButton4();
    u32 sendButtonD();
    u32 sendButtonE();
    u32 sendButton2();
    u32 sendButton5();
    u32 sendEvent62();
    u32 sendEvent63();
    u8 isLeft(u32 ev);
    u8 isRight(u32 ev);
    u8 isUp(u32 ev);
    u8 isDown(u32 ev);
    u32 holdBit0(u32 ev);
    u32 holdBit1(u32 ev);
    u32 holdBit3(u32 ev);
    u32 holdBit4(u32 ev);
    u32 holdBit13(u32 ev);
    u32 holdBit14(u32 ev);
    u32 holdBit2(u32 ev);
    u32 holdBit5(u32 ev);
    u32 holdBit11(u32 ev);
    u32 holdBit12(u32 ev);

    u8 gapQueue[4];
    u8 mEventQueue[0x44];
    gptr<EventMgrVtable> mpVtable;
    be<u32> mSecondaryVtable;
    Disposer mDisposer;
    be<u32> mMode, mNextMode, mParamA;
    gptr<Receiver> mpFocus, mpGrab, mpHover;
    Vec2 mTouchStart, mTouchCurrent, mTouchPrevious, mVelocity;
    be<u32> mInertiaFrames;
    Vec2 mSamplePrevious, mSampleCurrent, mSwipe;
    be<u8> mModeChanged;
    u8 padding[3];
    be<u32> mParamB, mLastId, mCurrentId, mInertiaLength;
    be<f32> mFriction, mStopSpeed, mMargin;
};
WWHD_OFFSET(EventMgr, mEventQueue, 0x14);
WWHD_OFFSET(EventMgr, mpVtable, 0x58);
WWHD_OFFSET(EventMgr, mDisposer, 0x60);
WWHD_OFFSET(EventMgr, mMode, 0x70);
WWHD_OFFSET(EventMgr, mpFocus, 0x7C);
WWHD_OFFSET(EventMgr, mTouchStart, 0x88);
WWHD_OFFSET(EventMgr, mInertiaFrames, 0xA8);
WWHD_OFFSET(EventMgr, mSamplePrevious, 0xAC);
WWHD_OFFSET(EventMgr, mSwipe, 0xBC);
WWHD_OFFSET(EventMgr, mParamB, 0xC8);
WWHD_OFFSET(EventMgr, mInertiaLength, 0xD4);
WWHD_OFFSET(EventMgr, mMargin, 0xE0);
WWHD_SIZE(EventMgr, 0xE4);
} // namespace cking::ui::input

namespace cking::ui::input {
struct Constants {
    be<f32> mZero, mFriction, mStopSpeed, mMargin;
    u8 heapName[0x24];
    be<f32> mTouchLimit, mDragDistanceSquared, mHalfWidth, mHalfHeight, mBounceOffset;
    be<f32> mSwipeXThreshold, mSwipeYThreshold, mHalf, mThree, mMinAngle, mMaxAngle;
    be<f32> mTouchDefault, mDeltaDefault;
};
WWHD_OFFSET(Constants, mTouchLimit, 0x34);
WWHD_SIZE(Constants, 0x68);
struct InputManager {
    u8 prefix[0x18];
    Event mPointer;
};
WWHD_OFFSET(InputManager, mPointer, 0x18);
WWHD_SIZE(InputManager, 0x1BC);
struct Options {
    u8 prefix[0x14];
    be<u8> mDirectionalInputEnabled;
};
WWHD_SIZE(Options, 0x15);
struct StaticData {
    be<f32> mMinAngle, mMaxAngle;
    Vec2 mDefaultTouch, mDefaultDelta;
    u8 mFirstFlag, mSecondFlag, padding[2];
    be<u32> mZeros[4];
};
WWHD_OFFSET(StaticData, mFirstFlag, 0x18);
WWHD_OFFSET(StaticData, mZeros, 0x1C);
WWHD_SIZE(StaticData, 0x2C);
struct DestructorRecords {
    u8 mRecords[3][12];
};
WWHD_SIZE(DestructorRecords, 0x24);
struct TouchUpScratch {
    Vec2 mPosition, mDelta, mCalculatedDelta;
};
WWHD_SIZE(TouchUpScratch, 0x18);
struct WordMatrix {
    be<u32> mWords[12];
};
WWHD_SIZE(WordMatrix, 0x30);
struct ButtonParameter {
    be<u8> mCode;
    u8 padding[7];
};
WWHD_SIZE(ButtonParameter, 8);
enum : u32 { ManagerVtable = 0x100E26D0, SecondaryVtable = 0x100E26C0, DisposerVtable = 0x100E26B0 };
} // namespace cking::ui::input

namespace cking::ui::input {
struct HeapVtable {
    u8 prefix[0x2C];
    gfn mReset;
};
struct Heap {
    u8 prefix[0xC];
    gptr<HeapVtable> mpVtable;
};
WWHD_SIZE(HeapVtable, 0x30);
WWHD_SIZE(Heap, 0x10);
} // namespace cking::ui::input

using cking::ui::input::EventMgr;
void hdUiEventMgrBaseConstruct(EventMgr*);
void hdUiEventMgrBaseDestroy(u32, u32);
void hdUiEventQueuePrepare(u32, u32, u32);
void hdUiEventQueueForward(u32);
void hdUiEventQueueBeginFrame(u32);
void hdUiEventQueueProcess(u32);
void hdUiQueueEvent(u32, u32, u32, u32);
void hdUiEventQueueClear(u32);
u32 hdUiGetParentHeap();
f32 hdUiReceiverDistance(u32, u32);
u32 hdUiAllocate(u32);
void hdUiFree(u32);
u32 hdUiAllocateAligned(u32, u32, u32);
void hdUiListAppend(u32, u32);
void hdUiListErase(u32);
void hdUiDisposerConstruct(u32, u32, u32);
void hdUiDisposerDestroy(u32, u32);
u32 hdUiCreateHeap(u32, u32, u32, u32, u32);
u32 hdUiHeapHeaderSize(u32);
void hdUiConstructSecondFlag(u32);
void hdUiConstructFirstFlag(u32);
void hdUiRegisterObject(u32);

namespace cking::ui::input {
enum VirtualSlot : u16 {
    SLOT_PointerDown = 0x2C,
    SLOT_PointerMove = 0x34,
    SLOT_PointerUp = 0x3C,
    SLOT_OnLeft = 0x44,
    SLOT_OnRight = 0x4C,
    SLOT_OnUp = 0x54,
    SLOT_OnDown = 0x5C,
    SLOT_OnLeftUp = 0x64,
    SLOT_OnLeftDown = 0x6C,
    SLOT_OnRightUp = 0x74,
    SLOT_OnRightDown = 0x7C,
    SLOT_OnFallbackThird = 0x84,
    SLOT_OnFallbackFirst = 0x8C,
    SLOT_OnFallbackFourth = 0x94,
    SLOT_OnFallbackSecond = 0x9C,
    SLOT_CheckFallbackThird = 0xC4,
    SLOT_CheckFallbackFirst = 0xEC,
    SLOT_CheckFallbackFourth = 0x114,
    SLOT_CheckFallbackSecond = 0x13C,
    SLOT_OnHoldBit0 = 0x154,
    SLOT_OnHoldBit1 = 0x15C,
    SLOT_OnHoldBit3 = 0x164,
    SLOT_OnHoldBit4 = 0x16C,
    SLOT_OnHoldBit13 = 0x174,
    SLOT_OnHoldBit14 = 0x17C,
    SLOT_OnHoldBit2 = 0x184,
    SLOT_OnHoldBit5 = 0x18C,
    SLOT_OnHoldBit11 = 0x194,
    SLOT_OnHoldBit12 = 0x19C,
    SLOT_CheckLeft = 0x1A4,
    SLOT_CheckRight = 0x1AC,
    SLOT_CheckUp = 0x1B4,
    SLOT_CheckDown = 0x1BC,
    SLOT_CheckHoldBit0 = 0x1C4,
    SLOT_CheckHoldBit1 = 0x1CC,
    SLOT_CheckHoldBit3 = 0x1D4,
    SLOT_CheckHoldBit4 = 0x1DC,
    SLOT_CheckHoldBit13 = 0x1E4,
    SLOT_CheckHoldBit14 = 0x1EC,
    SLOT_CheckHoldBit2 = 0x1F4,
    SLOT_CheckHoldBit5 = 0x1FC,
    SLOT_CheckHoldBit11 = 0x204,
    SLOT_CheckHoldBit12 = 0x20C,
};
enum EventId : u32 {
    EVENT_TOUCH_DOWN = 0x2,
    EVENT_TOUCH_MOVE = 0x3,
    EVENT_TOUCH_UP = 0x4,
    EVENT_BUTTON = 0xD,
    EVENT_DRAG_START = 0x43,
    EVENT_DRAG_MOVE = 0x44,
    EVENT_DROP = 0x45,
    EVENT_DRAG_ENTER = 0x46,
    EVENT_DRAG_OVER = 0x47,
    EVENT_DRAG_LEAVE = 0x48,
    EVENT_BUTTON11 = 0x62,
    EVENT_BUTTON12 = 0x63,
    EVENT_SWIPE_X = 0x64,
    EVENT_SWIPE_Y = 0x65,
};
enum : u32 { BUTTON_TOUCH = 0x8000, POINTER_VALID = 1, INERTIA_IDLE = 0xFFFFFFFF, INVALID_RECEIVER_ID = 0xFFFFFFFF };
} // namespace cking::ui::input

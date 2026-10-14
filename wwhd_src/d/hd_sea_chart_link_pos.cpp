#include "d/hd_sea_chart_link_pos.h"
#include "d/d_com_inf_game.h"
#include "d/hd_input_ctrl.h"
#include <bit>
using namespace gabi;

struct hdSeaChartLinkMode_l {
    u8 mReserved[0xA0];
    be<u8> mSeaMode;
};
struct hdSeaChartLinkOwnerView_l {
    u8 mReserved[0x1C4];
    gptr<hdSeaChartLinkMode_l> mMode;
};
WWHD_OFFSET(hdSeaChartLinkMode_l, mSeaMode, 0xA0);
WWHD_OFFSET(hdSeaChartLinkOwnerView_l, mMode, 0x1C4);
struct hdSeaChartLinkEventQueue_l {
    u8 mOpaque;
};
WWHD_OFFSET(hdButtonEventManager_l, mQueue, 0x14);
struct hdSeaChartLinkInputSingleton_c {
    gptr<hdInputManager> mValue;
};
struct hdSeaChartLinkEventSingleton_c {
    gptr<HdPadEventSource> mValue;
};
struct hdSeaChartLinkInputActiveSingleton_c {
    gptr<HdPadInputActive> mValue;
};
struct hdSeaChartLinkOwnerSingleton_c {
    gptr<HdPadUiOwner> mValue;
};
struct hdSeaChartLinkUiSingleton_c {
    gptr<hdScreenUiSystem> mValue;
};
struct hdSeaChartLinkCursor_l {
    u8 mReserved[0x18];
    be<u32> mState;
};
struct hdSeaChartLinkCursorSingleton_c {
    gptr<hdSeaChartLinkCursor_l> mValue;
};
static hdSeaChartLinkInputSingleton_c sLinkInput;
static hdSeaChartLinkEventSingleton_c sLinkEvents;
static hdSeaChartLinkInputActiveSingleton_c sLinkInputActive;
static hdSeaChartLinkOwnerSingleton_c sLinkOwner;
static hdSeaChartLinkUiSingleton_c sLinkUi;
static hdSeaChartLinkCursorSingleton_c sLinkCursor;
static HdPadPoint sLinkFallbackPosition;
static f32 sLinkAnimationRate;
static f32 sLinkZero;
static f32 sLinkPositionDivisor;
static hdPartsStateId_c sLinkBlankState;
static hdPartsStateId_c sLinkDisplayState;
static hdPartsStateId_c sLinkInState;
static hdPartsStateId_c sLinkOutState;
static u32 sLinkSafeStringVtable;
static u32 sLinkVtable;
static u32 sLinkHolderVtable;
static u32 sLinkSecondaryVtable;
void seaChartLinkBuildGrid(hdSeaChartLinkPos_c* self, hdSeaChartPane_l* pane, HdPadPoint* size,
                           HdPadPoint* positions);
hdSeaChartPane_l* seaChartLinkFindLayoutPane(hdPartsLayout_c* layout, hdUiSafeString* layoutName,
                                             hdUiSafeString* paneName);
void seaChartLinkSetButtonSound(hdBasicButton_c* button, hdUiSafeString* sound, u32 flags);
void* seaChartLinkAllocate(u32 size);
hdParts_c* seaChartLinkConstructParts(hdParts_c* self);
dComIfG_inf_c* seaChartLinkGameInfo();
u32 seaChartLinkUseSeaScale(hdMapDisplayCoordinator_c* coordinator);
u32 seaChartLinkButtonId(hdButtonPart_c* button);
void seaChartLinkConstructEvent(HdPadEvent* event, u32 kind, u32 id, u32 parameter);
void seaChartLinkAppendEvent(hdSeaChartLinkEventQueue_l* queue, HdPadEvent* event);
void seaChartLinkDestroyEvent(HdPadEvent* event);
u32 seaChartLinkCancelEvent(HdPadEvent* event);
u32 seaChartLinkEventBusy(HdPadEventSource* source);
void seaChartLinkCopyEvent(HdPadEvent* output, HdPadEvent* input);
void seaChartLinkDestroyParts(hdParts_c* self, s32 flags);
void seaChartLinkFree(void* self);
void seaChartLinkSetEventReceiver(HdPadEventSource* events, HdInputReceiver* receiver);
u32 seaChartLinkPointOutside(hdSeaChartPane_l* pane, HdPadPoint* point);
void seaChartLinkPlay(hdAnimSet* set, u32 entry, u32 slot, f32 rate);
u32 seaChartLinkAnimationFinished(hdAnimSet* set, u32 entry);
void seaChartLinkAnimationOperation(hdAnimSet* set, u32 entry, u32 slot);
void seaChartLinkStopAnimation(hdAnimSet* set, u32 entry, u32 slot);
void seaChartLinkChangeState(hdStateMachine* machine, hdPartsStateId_c* state);
void seaChartLinkPlaySound(const char* name);
void seaChartLinkReleaseArray(hdSeaChartPaneArray_c* array);
void seaChartLinkAllocateArray(hdSeaChartPaneArray_c* array, s32 capacity, hdScreenHeap* heap,
                               u32 alignment);

/* 02683938 */
void seaChartLinkDestroy(hdSeaChartLinkPos_c* self, s32 flags) {
    if (!self)
        return;
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sLinkHolderVtable);
    self->mSecondaryVtable.v = gabi::ea(&sLinkSecondaryVtable);
    seaChartLinkDestroyParts(self, 0);
    if (flags & 1)
        seaChartLinkFree(self);
}

/* 026839A4 */
void hdSeaChartLinkPos_c::selectInputReceiver() {
    hdBasicButton_c* child = selectedChild();
    if (child)
        seaChartLinkSetEventReceiver(sLinkEvents.mValue, reinterpret_cast<HdInputReceiver*>(
                                                             child->mButton.mpReceiver.get()));
    sLinkCursor.mValue->mState = 0;
}

static u32 chartLinkTouchOutside(hdSeaChartPane_l* pane, u32 buttons) {
    hdInputManager* input = sLinkInput.mValue;
    if (!(buttons & 0x8000))
        return 0;
    HdPadPoint* source = &sLinkFallbackPosition;
    if (u32(input->mPointer.mPointerFlags) & 1)
        source = reinterpret_cast<HdPadPoint*>(&input->mPointer.mCalibratedX);
    Local<HdPadPoint> position;
    position->x = source->x;
    position->y = source->y;
    return pane && !seaChartLinkPointOutside(pane, position);
}

/* 02683A08 */
u32 hdSeaChartLinkPos_c::isTouchHeldOutside(hdSeaChartPane_l* pane) {
    return chartLinkTouchOutside(pane, sLinkInput.mValue->mPointer.mHold);
}
/* 02683A98 */
u32 hdSeaChartLinkPos_c::isTouchTriggeredOutside(hdSeaChartPane_l* pane) {
    return chartLinkTouchOutside(pane, sLinkInput.mValue->mPointer.mTrigger);
}
/* 02683B28 */
u32 hdSeaChartLinkPos_c::isTouchRepeatedOutside(hdSeaChartPane_l* pane) {
    return chartLinkTouchOutside(pane, sLinkInput.mValue->mPointer.mRepeat);
}

/* 02683BB8 */
void hdSeaChartLinkPos_c::updateInput() {
    if (!sLinkEvents.mValue)
        return;
    Local<HdPadEvent> event;
    seaChartLinkCopyEvent(event, &sLinkEvents.mValue->mEvent);
    if (u32(event->mKind) == 17) {
        hdBasicButton_c* child = selectedChild();
        if (child && u32(event->mId) == seaChartLinkButtonId(&child->mButton)) {
            const u32 id = seaChartLinkButtonId(&child->mButton);
            Local<HdPadEvent> notification;
            seaChartLinkConstructEvent(notification, 97, id, 0);
            auto* owner = reinterpret_cast<hdButtonEventManager_l*>(sLinkEvents.mValue.get());
            seaChartLinkAppendEvent(reinterpret_cast<hdSeaChartLinkEventQueue_l*>(owner->mQueue),
                                    notification);
            seaChartLinkDestroyEvent(notification);
        }
    }
    const u8 enabled = mInputEnabled;
    mCancelRequested = 0;
    if (!enabled)
        return;
    if (sLinkInputActive.mValue->mActive) {
        const u32 cancel = seaChartLinkCancelEvent(event);
        const u8 stillEnabled = mInputEnabled;
        if (cancel)
            mCancelRequested = 1;
        if (!stillEnabled)
            return;
    }
    if (seaChartLinkEventBusy(sLinkEvents.mValue))
        return;
    if (!mTouchHeld) {
        if (isTouchHeldOutside(mHitPane) && isTouchHeldOutside(mIconHitPane)) {
            mTouchHeld = 1;
            return;
        }
        mTouchHeld = 0;
        mTouchReleased = 0;
        return;
    }
    if (isTouchTriggeredOutside(mHitPane) && isTouchTriggeredOutside(mIconHitPane)) {
        mTouchReleased = 1;
        return;
    }
    if (!isTouchRepeatedOutside(mHitPane) || !isTouchRepeatedOutside(mIconHitPane))
        mTouchHeld = 0;
}

/* 02683DE4 */
void hdSeaChartLinkPos_c::updatePlayerPosition() {
    hdSeaChartPane_l* position = mPlayerPositionPane;
    if (!position || !mPlayerRotationPane)
        return;
    auto* coordinator = reinterpret_cast<hdMapDisplayCoordinator_c*>(sLinkOwner.mValue->mUi.get());
    const f32 xProduct = f32(f64(f32(coordinator->mSeaPosition.x)) * f64(f32(mPlayerCellSize.x)));
    const f32 x = f32(f64(xProduct) / f64(sLinkPositionDivisor));
    const u8 flags = position->mFlags;
    const f32 yProduct = f32(f64(f32(coordinator->mSeaPosition.y)) * f64(f32(mPlayerCellSize.y)));
    position->mFlags = flags | 0x10;
    const f32 y = f32(f64(yProduct) / f64(sLinkPositionDivisor));
    position->mPosition.x = x;
    position->mPosition.z = sLinkZero;
    position->mPosition.y = -y;
    hdSeaChartPane_l* rotation = mPlayerRotationPane;
    // PPC snapshots these components with word loads before the game-state calls.
    const u32 rotationX = u32(*reinterpret_cast<be<u32>*>(&rotation->mRotation.x));
    const u32 rotationY = u32(*reinterpret_cast<be<u32>*>(&rotation->mRotation.y));
    bool dungeonScale = false;
    if (u32(seaChartLinkGameInfo()->play.mPlayerStatus[0][0]) & 0x10000) {
        auto* owner = reinterpret_cast<hdSeaChartLinkOwnerView_l*>(sLinkOwner.mValue.get());
        if (!owner->mMode->mSeaMode) {
            coordinator =
                reinterpret_cast<hdMapDisplayCoordinator_c*>(sLinkOwner.mValue->mUi.get());
            dungeonScale = !seaChartLinkUseSeaScale(coordinator);
        }
    }
    coordinator = reinterpret_cast<hdMapDisplayCoordinator_c*>(sLinkOwner.mValue->mUi.get());
    const f32 scale = dungeonScale ? f32(coordinator->mDungeonScale) : f32(coordinator->mSeaScale);
    rotation = mPlayerRotationPane;
    const u8 rotationFlags = rotation->mFlags;
    rotation->mRotation.x = std::bit_cast<f32>(rotationX);
    rotation->mRotation.z = scale;
    rotation->mRotation.y = std::bit_cast<f32>(rotationY);
    rotation->mFlags = rotationFlags | 0x10;
}

/* 02683F54 */
void hdSeaChartLinkPos_c::updateSelectedMark() {
    hdSeaChartPane_l* pane = mSelectedMark;
    if (!pane)
        return;
    hdMapDisplayCoordinator_c* coordinator =
        reinterpret_cast<hdMapDisplayCoordinator_c*>(sLinkOwner.mValue->mUi.get());
    const s32 selected = s8(coordinator->mSelectedEntry);
    if (selected < 0)
        return;
    HdPadPoint* position = &mPositions[u32(selected) < 49 ? selected : 0];
    const f32 x = position->x;
    const u8 flags = pane->mFlags;
    const f32 y = position->y;
    pane->mPosition.x = x;
    pane->mFlags = flags | 0x10;
    pane->mPosition.z = sLinkZero;
    pane->mPosition.y = y;
}

/* 02683FD8 */
void hdSeaChartLinkPos_c::updateSelectedInput() {
    if (!mInputEnabled)
        return;
    hdBasicButton_c* child = selectedChild();
    if (child) {
        auto* table = reinterpret_cast<hdButtonPartVtable_l*>(child->mButton.mVtable.get());
        gabi::call_ptr<void>(table->mOnFocusChanged.get(), child);
    }
}

/* 0268401C */
void hdSeaChartLinkPos_c::executeDisplay() {
    updateInput();
    Local<HdPadEvent> event;
    seaChartLinkCopyEvent(event, &sLinkEvents.mValue->mEvent);
    if (u32(event->mKind) == 97) {
        seaChartLinkChangeState(stateMachine(), &sLinkOutState);
    } else if (mCancelRequested || mTouchReleased) {
        seaChartLinkChangeState(stateMachine(), &sLinkOutState);
        seaChartLinkPlaySound("SE_MENU_CHART_CLOSE");
    }
    updatePlayerPosition();
    updateSelectedMark();
    updateSelectedInput();
    gabi::call_ptr<void>(mVtable->mExecuteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 02684120 */
void hdSeaChartLinkPos_c::enterIn() {
    seaChartLinkPlay(mpLayout->animSet(), mInAnimation, mAnimationSlot, sLinkAnimationRate);
    seaChartLinkPlaySound("SE_MENU_CHART_OPEN_CLOSE");
    mTouchHeld = 0;
    mCancelRequested = 0;
    mTouchReleased = 0;
    gabi::call_ptr<void>(vtbl()->mEnterIn.get(), this);
}
/* 02684194 */
void hdSeaChartLinkPos_c::executeIn() {
    if (seaChartLinkAnimationFinished(mpLayout->animSet(), mInAnimation))
        seaChartLinkChangeState(stateMachine(), &sLinkDisplayState);
    updatePlayerPosition();
    updateSelectedMark();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 02684204 */
void hdSeaChartLinkPos_c::enterOut() {
    seaChartLinkSetEventReceiver(sLinkEvents.mValue, nullptr);
    seaChartLinkPlay(mpLayout->animSet(), mOutAnimation, mAnimationSlot, sLinkAnimationRate);
    seaChartLinkPlaySound("SE_MENU_CHART_OPEN_CLOSE");
}
/* 02684264 */
void hdSeaChartLinkPos_c::executeOut() {
    if (seaChartLinkAnimationFinished(mpLayout->animSet(), mOutAnimation))
        seaChartLinkChangeState(stateMachine(), &sLinkBlankState);
    updatePlayerPosition();
    updateSelectedMark();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
template <class T> static T* chartLinkEmbeddedMember(hdSeaChartLinkPos_c* self, u32 offset) {
    // GHS tests the member address after unsigned guest-address wrapping.
    const u32 address = u32(gabi::ea(self) + offset);
    return address ? gabi::at<T>(address) : static_cast<T*>(seaChartLinkAllocate(sizeof(T)));
}

/* 026842D4 */
hdSeaChartLinkPos_c* seaChartLinkConstruct(hdSeaChartLinkPos_c* self) {
    if (!self) {
        self = static_cast<hdSeaChartLinkPos_c*>(seaChartLinkAllocate(sizeof(hdSeaChartLinkPos_c)));
        if (!self)
            return nullptr;
    }
    seaChartLinkConstructParts(self);
    self->mVtable.v = gabi::ea(&sLinkVtable);
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sLinkHolderVtable);
    self->mSecondaryVtable.v = gabi::ea(&sLinkSecondaryVtable);
    self->mInAnimation = 0;
    self->mPaneNameCount = 0;
    self->mAnimationSlot = 0;
    self->mOutAnimation = 0;
    self->mSelectedChild = 0;
    auto* panes =
        chartLinkEmbeddedMember<hdSeaChartPaneArray_c>(self, offsetof(hdSeaChartLinkPos_c, mPanes));
    if (panes) {
        panes->mCapacity = 0;
        panes->mCount = 0;
        panes->mData = nullptr;
    }
    self->mIconHitPane = nullptr;
    const f32 zero = sLinkZero;
    self->mIconPane = nullptr;
    self->mHitPane = nullptr;
    self->mGridPane = nullptr;
    self->mSelectedMark = nullptr;
    self->mPlayerRotationPane = nullptr;
    self->mPlayerPositionPane = nullptr;
    self->mPaneNames = nullptr;
    self->mLinkGridPane = nullptr;
    HdPadPoint* cellSize =
        chartLinkEmbeddedMember<HdPadPoint>(self, offsetof(hdSeaChartLinkPos_c, mCellSize));
    if (cellSize) {
        cellSize->y = zero;
        cellSize->x = zero;
    }
    HdPadPoint* playerCellSize =
        chartLinkEmbeddedMember<HdPadPoint>(self, offsetof(hdSeaChartLinkPos_c, mPlayerCellSize));
    if (playerCellSize) {
        playerCellSize->y = zero;
        playerCellSize->x = zero;
    }
    // These two embedded-array checks allocate on wrapping but retain the self-relative storage.
    chartLinkEmbeddedMember<HdPadPoint[49]>(self, offsetof(hdSeaChartLinkPos_c, mPositions));
    chartLinkEmbeddedMember<HdPadPoint[49]>(self, offsetof(hdSeaChartLinkPos_c, mPlayerPositions));
    self->mInputEnabled = 0;
    self->mCompactLayout = 0;
    self->mCancelRequested = 0;
    self->mTouchReleased = 0;
    self->mTouchHeld = 0;
    for (u32 index = 0; index < 49; ++index) {
        self->mPositions[index].x = zero;
        self->mPositions[index].y = zero;
    }
    for (u32 index = 0; index < 49; ++index) {
        self->mPlayerPositions[index].x = zero;
        self->mPlayerPositions[index].y = zero;
    }
    return self;
}

/* 026844AC */
void hdSeaChartLinkPos_c::releasePanes() {
    if (mPanes.mData)
        seaChartLinkReleaseArray(&mPanes);
}
/* 026844C0 */
void hdSeaChartLinkPos_c::destroyLayout() {
    gabi::call_ptr<void>(mVtable->mDeleteChildren.get(), this);
    gabi::call_ptr<void>(mVtable->mDestroyLayout.get(), this);
    releasePanes();
}
/* 02684514 */
void hdSeaChartLinkPos_c::enterBlank() {
    seaChartLinkSetEventReceiver(sLinkEvents.mValue, nullptr);
    seaChartLinkStopAnimation(mpLayout->animSet(), mOutAnimation, mAnimationSlot);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    seaChartLinkChangeState(stateMachine(), &sLinkBlankState);
}
/* 02684584 */
void hdSeaChartLinkPos_c::allocatePanes(s32 capacity) {
    releasePanes();
    seaChartLinkAllocateArray(&mPanes, capacity, sLinkUi.mValue->mHeap, 4);
}
/* 026845D8 */
void hdSeaChartLinkPos_c::findPanes() {
    for (s32 index = 0; index < s32(mPaneNameCount); ++index) {
        hdPartsLyt_c* lyt = mpLayout->lyt();
        hdUiSafeString* name = &mPaneNames[index];
        name->assureTermination();
        hdSeaChartPane_l* root = reinterpret_cast<hdSeaChartPane_l*>(lyt->mpRootPane.get());
        hdSeaChartPane_l* pane = reinterpret_cast<hdSeaChartPane_l*>(gabi::call_ptr<hdPartsPane_c*>(
            reinterpret_cast<hdButtonPaneVtable_l*>(root->mVtable.get())->mFindPane.get(), root,
            name->mText.get(), 1));
        if (pane && s32(mPanes.mCount) < s32(mPanes.mCapacity)) {
            mPanes.mData[u32(mPanes.mCount)] = pane;
            mPanes.mCount = u32(mPanes.mCount) + 1;
        }
    }
}
static hdSeaChartPane_l* chartLinkPaneAt(hdSeaChartPaneArray_c* panes, s32 index) {
    return u32(panes->mCount) > u32(index) ? panes->mData[index].get() : nullptr;
}

/* 026847F4 */
void hdSeaChartLinkPos_c::setupLayout() {
    allocatePanes(mPaneNameCount);
    findPanes();
    if (mCompactLayout) {
        mHitPane = chartLinkPaneAt(&mPanes, 0);
        mIconPane = chartLinkPaneAt(&mPanes, 1);
        mLinkGridPane = nullptr;
        mSelectedMark = nullptr;
        mPlayerPositionPane = nullptr;
        mGridPane = nullptr;
    } else {
        mHitPane = chartLinkPaneAt(&mPanes, 0);
        mGridPane = chartLinkPaneAt(&mPanes, 1);
        mLinkGridPane = chartLinkPaneAt(&mPanes, 2);
        mSelectedMark = chartLinkPaneAt(&mPanes, 3);
        mIconPane = chartLinkPaneAt(&mPanes, 4);
        mPlayerPositionPane = chartLinkPaneAt(&mPanes, 5);
        Local<hdUiSafeString> paneName;
        paneName->mText.v = gabi::guest_addr("N_Rotate_00");
        paneName->mVtable.v = gabi::ea(&sLinkSafeStringVtable);
        Local<hdUiSafeString> layoutName;
        layoutName->mText.v = gabi::guest_addr("L_LinkPos_00");
        layoutName->mVtable.v = gabi::ea(&sLinkSafeStringVtable);
        mPlayerRotationPane = seaChartLinkFindLayoutPane(mpLayout, layoutName, paneName);
        if (mGridPane && mLinkGridPane) {
            seaChartLinkBuildGrid(this, mGridPane, &mCellSize, mPositions);
            seaChartLinkBuildGrid(this, mLinkGridPane, &mPlayerCellSize, mPlayerPositions);
        }
    }
    hdSeaChartPane_l* icon = mIconPane;
    if (icon) {
        auto* table = reinterpret_cast<hdButtonPaneVtable_l*>(icon->mVtable.get());
        mIconHitPane = reinterpret_cast<hdSeaChartPane_l*>(
            gabi::call_ptr<hdPartsPane_c*>(table->mFindPane.get(), icon, "B_Btn_00", 1));
    }
    hdBasicButton_c* child = selectedChild();
    if (child)
        child->mRequestedState = 1;
    seaChartLinkAnimationOperation(mpLayout->animSet(), mInAnimation, mAnimationSlot);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    hdStateMachine* machine = stateMachine();
    machine->mpCurrentState =
        machine->mpOwner->createState(reinterpret_cast<hdStateId*>(&sLinkBlankState));
    machine->mpCurrentState->enter();
    child = selectedChild();
    if (child) {
        Local<hdUiSafeString> sound;
        sound->mText.v = gabi::guest_addr("SE_MENU_CHART_CLOSE");
        sound->mVtable.v = gabi::ea(&sLinkSafeStringVtable);
        seaChartLinkSetButtonSound(child, sound, 0);
    }
}

/* 02684AB4 */
void hdSeaChartLinkPos_c::configureAnimations(u32 inAnimation, u32 outAnimation, u32 slot,
                                              u32 selected) {
    mAnimationSlot = slot;
    mOutAnimation = outAnimation;
    mSelectedChild = selected;
    mInAnimation = inAnimation;
}

struct hdSeaChartLinkStatics_c {
    be<f32> mMinimum, mMaximum;
    HdPadPoint mOrigin, mOffset;
    be<u8> mFirstFlag, mSecondFlag;
    u8 mPadding[2];
    be<u32> mStorage[4];
};
WWHD_OFFSET(hdSeaChartLinkStatics_c, mMinimum, 0);
WWHD_OFFSET(hdSeaChartLinkStatics_c, mMaximum, 4);
WWHD_OFFSET(hdSeaChartLinkStatics_c, mOrigin, 8);
WWHD_OFFSET(hdSeaChartLinkStatics_c, mOffset, 0x10);
WWHD_OFFSET(hdSeaChartLinkStatics_c, mFirstFlag, 0x18);
WWHD_OFFSET(hdSeaChartLinkStatics_c, mSecondFlag, 0x19);
WWHD_OFFSET(hdSeaChartLinkStatics_c, mStorage, 0x1C);
struct hdSeaChartLinkRegistration_c {
    be<u32> mWords[3];
};
struct hdSeaChartLinkCounter_c {
    be<u32> mValue;
};
static hdSeaChartLinkStatics_c sLinkStatics;
static hdSeaChartLinkRegistration_c sLinkStorageRegistration;
static hdSeaChartLinkRegistration_c sLinkFirstFlagRegistration;
static hdSeaChartLinkRegistration_c sLinkSecondFlagRegistration;
static hdSeaChartLinkCounter_c sLinkStateGuard;
static hdSeaChartLinkCounter_c sLinkStateCounter;
static hdStateId sLinkDisplayNull;
static hdStateId sLinkBlankNull;
static hdStateId sLinkTransitionNull;
static hdPartsStateIdVtbl_c sLinkStateVtable;
static f32 sLinkMinimum;
static f32 sLinkMaximum;
static f32 sLinkOrigin;
static f32 sLinkOffset;
void seaChartLinkRegisterStatic(hdSeaChartLinkRegistration_c* record);
void seaChartLinkFirstFlag(be<u8>* flag);
void seaChartLinkSecondFlag(be<u8>* flag);

/* 02684AC8 */
void seaChartLinkInitializeStatics() {
    sLinkStatics.mStorage[3] = 0;
    sLinkStatics.mStorage[2] = 0;
    sLinkStatics.mStorage[1] = 0;
    sLinkStatics.mStorage[0] = 0;
    seaChartLinkRegisterStatic(&sLinkStorageRegistration);
    const f32 minimum = sLinkMinimum;
    const f32 maximum = sLinkMaximum;
    sLinkStatics.mMinimum = minimum;
    sLinkStatics.mMaximum = maximum;
    seaChartLinkFirstFlag(&sLinkStatics.mFirstFlag);
    seaChartLinkRegisterStatic(&sLinkFirstFlagRegistration);
    seaChartLinkSecondFlag(&sLinkStatics.mSecondFlag);
    seaChartLinkRegisterStatic(&sLinkSecondFlagRegistration);
    const f32 origin = sLinkOrigin;
    const f32 offset = sLinkOffset;
    const u32 initialized = sLinkStateGuard.mValue;
    sLinkStatics.mOffset.x = offset;
    sLinkStatics.mOrigin.x = origin;
    sLinkStatics.mOrigin.y = origin;
    sLinkStatics.mOffset.y = offset;
    u32 number;
    if (initialized) {
        number = sLinkStateCounter.mValue;
    } else {
        sLinkStateGuard.mValue = 1;
        number = 0;
    }
    sLinkDisplayState.setup(s32(++number), "::StateID_Display", &sLinkStateVtable, 7,
                            &sLinkDisplayNull);
    sLinkBlankState.setup(s32(++number), "::StateID_Blank", &sLinkStateVtable, 4, &sLinkBlankNull);
    sLinkInState.setup(s32(++number), "::StateID_InAll", &sLinkStateVtable, 25,
                       &sLinkTransitionNull);
    sLinkOutState.setup(s32(++number), "::StateID_OutAll", &sLinkStateVtable, 28,
                        &sLinkTransitionNull);
    sLinkStateCounter.mValue = number;
}

/* WWHD glue (tools/verify/autoglue.py)
 data sLinkSafeStringVtable 100F92D4 nocheck
 data sLinkVtable 100F9480 nocheck
 data sLinkHolderVtable 100F9580 nocheck
 data sLinkSecondaryVtable 100F9590 nocheck
data sLinkStatics 104961D4
data sLinkStorageRegistration 101F6290
data sLinkFirstFlagRegistration 101F629C
data sLinkSecondFlagRegistration 101F62A8
data sLinkStateGuard 101FD8E4
data sLinkStateCounter 101FDD50
data sLinkDisplayNull 1049DD90
data sLinkBlankNull 1049DD60
data sLinkTransitionNull 101FF32C
data sLinkStateVtable 100F9334
data sLinkMinimum 100F9424
data sLinkMaximum 100F9428
data sLinkOrigin 100F942C
data sLinkOffset 100F9430
ext 028F026C void seaChartLinkRegisterStatic(hdSeaChartLinkRegistration_c*)
ext 028ED6F8 void seaChartLinkFirstFlag(be<u8>*)
ext 028EAB2C void seaChartLinkSecondFlag(be<u8>*)
str 100F9434 "::StateID_Blank" in 02684AC8
str 100F9444 "::StateID_InAll" in 02684AC8
str 100F9454 "::StateID_Display" in 02684AC8
str 100F9468 "::StateID_OutAll" in 02684AC8
data sLinkInput 101F5088
data sLinkEvents 101F4FF0
data sLinkInputActive 101F8378
data sLinkOwner 101F8344
data sLinkUi 1018C404
data sLinkCursor 101F5A44
data sLinkFallbackPosition 1049FFA0
data sLinkAnimationRate 100F9380
data sLinkZero 100F9388
data sLinkPositionDivisor 100F9384
data sLinkDisplayState 10496200
data sLinkBlankState 10496230
data sLinkInState 10496260
data sLinkOutState 10496290
ext 02684698 void seaChartLinkBuildGrid(hdSeaChartLinkPos_c*, hdSeaChartPane_l*, HdPadPoint*, HdPadPoint*)
ext 02704FC0 hdSeaChartPane_l* seaChartLinkFindLayoutPane(hdPartsLayout_c*, hdUiSafeString*, hdUiSafeString*)
ext 026F2540 void seaChartLinkSetButtonSound(hdBasicButton_c*, hdUiSafeString*, u32)
str 100F93E8 "N_Rotate_00"
str 100F9408 "L_LinkPos_00"
str 100F9418 "B_Btn_00"
str 100F93F4 "SE_MENU_CHART_CLOSE" in 026847F4
ext 0273AD10 void* seaChartLinkAllocate(u32)
ext 026F89E8 hdParts_c* seaChartLinkConstructParts(hdParts_c*)
ext 025200D4 dComIfG_inf_c* seaChartLinkGameInfo()
ext 026890E8 u32 seaChartLinkUseSeaScale(hdMapDisplayCoordinator_c*)
ext 026F5B74 u32 seaChartLinkButtonId(hdButtonPart_c*)
ext 02001338 void seaChartLinkConstructEvent(HdPadEvent*, u32, u32, u32)
ext 02001FF4 void seaChartLinkAppendEvent(hdSeaChartLinkEventQueue_l*, HdPadEvent*)
ext 02001490 void seaChartLinkDestroyEvent(HdPadEvent*)
ext 026F6340 u32 seaChartLinkCancelEvent(HdPadEvent*)
ext 02615398 u32 seaChartLinkEventBusy(HdPadEventSource*)
ext 020013C0 void seaChartLinkCopyEvent(HdPadEvent*, HdPadEvent*)
str 100F938C "SE_MENU_CHART_CLOSE"
ext 026F88F0 void seaChartLinkDestroyParts(hdParts_c*, s32)
ext 0273AF40 void seaChartLinkFree(void*)
ext 02615154 void seaChartLinkSetEventReceiver(HdPadEventSource*, HdInputReceiver*)
ext 0287F05C u32 seaChartLinkPointOutside(hdSeaChartPane_l*, HdPadPoint*)
ext 020053E4 void seaChartLinkPlay(hdAnimSet*, u32, u32, f32)
ext 02005840 u32 seaChartLinkAnimationFinished(hdAnimSet*, u32)
ext 02005674 void seaChartLinkAnimationOperation(hdAnimSet*, u32, u32)
ext 02005708 void seaChartLinkStopAnimation(hdAnimSet*, u32, u32)
ext 020063C0 void seaChartLinkChangeState(hdStateMachine*, hdPartsStateId_c*)
ext 02030B38 void seaChartLinkPlaySound(const char*)
ext 0273B5DC void seaChartLinkReleaseArray(hdSeaChartPaneArray_c*)
ext 0273B584 void seaChartLinkAllocateArray(hdSeaChartPaneArray_c*, s32, hdScreenHeap*, u32)
str 100F93A0 "SE_MENU_CHART_OPEN_CLOSE" in 02684120
str 100F93BC "SE_MENU_CHART_OPEN_CLOSE" in 02684204
*/

#include "d/hd_input_ctrl.h"
#include "d/hd_sea_chart_components.h"

static f32 sChartCellAnimationRate;
static f32 sChartCellZero;
struct hdSeaChartCellStatics_c {
    be<f32> mMinimum;
    be<f32> mMaximum;
    HdPadPoint mDefaultOrigin;
    HdPadPoint mDefaultOffset;
    be<u8> mFirstFlag;
    be<u8> mSecondFlag;
    u8 mPadding[2];
    be<u32> mStorage[4];
};
WWHD_OFFSET(hdSeaChartCellStatics_c, mMinimum, 0);
WWHD_OFFSET(hdSeaChartCellStatics_c, mMaximum, 4);
WWHD_OFFSET(hdSeaChartCellStatics_c, mDefaultOrigin, 8);
WWHD_OFFSET(hdSeaChartCellStatics_c, mDefaultOffset, 0x10);
WWHD_OFFSET(hdSeaChartCellStatics_c, mFirstFlag, 0x18);
WWHD_OFFSET(hdSeaChartCellStatics_c, mSecondFlag, 0x19);
WWHD_OFFSET(hdSeaChartCellStatics_c, mStorage, 0x1C);
struct hdSeaChartCellRegistration_c {
    be<u32> mWords[3];
};
WWHD_SIZE(hdSeaChartCellRegistration_c, 0xC);
static hdSeaChartCellStatics_c sChartCellStatics;
static hdSeaChartCellRegistration_c sChartCellRegistrations[3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
static f32 sChartCellMinimum;
static f32 sChartCellMaximum;
static f32 sChartCellDefaultOrigin;
static f32 sChartCellDefaultOffset;
void seaChartCellRegisterStatic(hdSeaChartCellRegistration_c* record);
void seaChartCellInitializeFirstFlag(be<u8>* flag);
void seaChartCellInitializeSecondFlag(be<u8>* flag);

static u32 sChartCellVtable;
struct hdSeaChartInputSingleton_c {
    gptr<hdInputManager> mValue;
};
struct hdSeaChartUiSingleton_c {
    gptr<hdScreenUiSystem> mValue;
};
struct hdSeaChartEventSingleton_c {
    gptr<void> mValue;
};
static hdSeaChartInputSingleton_c sChartCellInputManager;
static hdSeaChartUiSingleton_c sChartCellUiSystem;
static hdSeaChartEventSingleton_c sChartCellEventManager;
static HdPadPoint sChartCellFallbackPointer;
// Three controller input kinds map to the chart's own interaction phases.
struct hdSeaChartPointPhases_c {
    be<u8> mValues[3];
};
static hdSeaChartPointPhases_c sChartCellPointPhases;
void seaChartCellFree(void* allocation);
void seaChartAnimationOperationCC(hdAnimSet* animations, u32 entry, u32 slot);
void seaChartAnimationOperationD4(hdAnimSet* animations, u32 entry, u32 slot);
void seaChartPlayAnimation(hdAnimSet* animations, u32 entry, u32 slot, f32 rate);
u32 seaChartAnimationEnd(hdAnimSet* animations, u32 entry);
HdInputReceiver* seaChartWidgetUserData(HdInputReceiver* widget, u32 index);
void seaChartSetWidgetUserData(HdInputReceiver* widget, u32 index, u32 value);
void* seaChartCellAllocateOnHeap(u32 size, hdScreenHeap* heap, u32 alignment);
HdInputReceiver* seaChartCellConstructReceiver(HdInputReceiver* receiver, void* pane, u32 value);
void* seaChartCellAllocate(u32 size);
void seaChartCellSetEventMode(void* manager, HdInputReceiver* receiver);
void seaChartCellPlaySound(const char* name);
hdSeaChartCell_c* seaChartFindWidgetCell(HdPadGrid* grid, HdInputReceiver* receiver);

template <class T> static T* chartCellEmbeddedMember(hdSeaChartCell_c* self, u32 offset) {
    // GHS tests the wrapped guest member address and allocates if it is null.
    const u32 address = u32(gabi::ea(self) + offset);
    return address ? gabi::at<T>(address) : static_cast<T*>(seaChartCellAllocate(sizeof(T)));
}
static void chartCellInitializePoint(HdPadPoint* point, f32 zero) {
    if (point) {
        point->y = zero;
        point->x = zero;
    }
}
static void chartCellInitializeBounds(HdPadRect* bounds, f32 zero) {
    if (bounds) {
        bounds->mRight = zero;
        bounds->mTop = zero;
        bounds->mLeft = zero;
        bounds->mBottom = zero;
    }
}
struct hdSeaChartIndexPair_c {
    be<u16> mValues[2];
};
static void chartCellInitializeIndices(hdSeaChartIndexPair_c* indices) {
    if (indices) {
        indices->mValues[1] = 0;
        indices->mValues[0] = 0;
    }
}

/* 0267FA74 */
hdSeaChartCell_c* seaChartCellConstruct(hdSeaChartCell_c* self) {
    if (!self)
        self = static_cast<hdSeaChartCell_c*>(seaChartCellAllocate(sizeof(*self)));
    if (!self)
        return nullptr;
    self->mId = 0xFFFFFFFF;
    self->mVtable.v = gabi::ea(&sChartCellVtable);
    HdPadCoordinate* coordinate =
        chartCellEmbeddedMember<HdPadCoordinate>(self, offsetof(hdSeaChartCell_c, mCoordinate));
    if (coordinate) {
        coordinate->y = -10;
        coordinate->x = -10;
    }
    chartCellInitializeIndices(chartCellEmbeddedMember<hdSeaChartIndexPair_c>(
        self, offsetof(hdSeaChartCell_c, mMapIndices)));
    self->mMapFlags = 0;
    chartCellInitializeIndices(chartCellEmbeddedMember<hdSeaChartIndexPair_c>(
        self, offsetof(hdSeaChartCell_c, mResourceIndices)));
    self->mMarkAnimationActive = 0;
    const f32 zero = sChartCellZero;
    chartCellInitializePoint(
        chartCellEmbeddedMember<HdPadPoint>(self, offsetof(hdSeaChartCell_c, mPosition)), zero);
    chartCellInitializePoint(
        chartCellEmbeddedMember<HdPadPoint>(self, offsetof(hdSeaChartCell_c, mSize)), zero);
    chartCellInitializeBounds(
        chartCellEmbeddedMember<HdPadRect>(self, offsetof(hdSeaChartCell_c, mBounds)), zero);
    chartCellInitializePoint(
        chartCellEmbeddedMember<HdPadPoint>(self, offsetof(hdSeaChartCell_c, mScaledPosition)),
        zero);
    chartCellInitializePoint(
        chartCellEmbeddedMember<HdPadPoint>(self, offsetof(hdSeaChartCell_c, mScaledSize)), zero);
    chartCellInitializeBounds(
        chartCellEmbeddedMember<HdPadRect>(self, offsetof(hdSeaChartCell_c, mScaledBounds)), zero);
    self->mGrid = nullptr;
    self->mSelectionLoopSlot = -1;
    self->mSelectionSlot = -1;
    self->mMarkSlot = -1;
    self->mFocusSlot = -1;
    self->mSelectionEntry = -1;
    self->mMarkEntry = -1;
    self->mAnimations = nullptr;
    self->mSelectionLoopEntry = -1;
    self->mFocusEntry = -1;
    chartCellInitializePoint(
        chartCellEmbeddedMember<HdPadPoint>(self, offsetof(hdSeaChartCell_c, mInputPosition)),
        zero);
    self->mInputEnabled = 0;
    self->mTouchPane = nullptr;
    self->mInputValid = 0;
    self->mInputEntered = 0;
    self->mTrackingInput = 0;
    self->mSelected = 0;
    self->mInputPhase = 0;
    self->mFocusRequested = 0;
    self->mPreviousInputPhase = 0;
    self->mInputKind = 0;
    self->mFocused = 0;
    self->mSelectionRequested = 0;
    self->mDiscovered = 0;
    self->mSelectionReleased = 0;
    self->mTouchWidget = nullptr;
    self->mInputExited = 0;
    return self;
}

/* 0267FCBC */
void hdSeaChartCell_c::releaseTouchWidget() {
    HdInputReceiver* widget = mTouchWidget;
    if (widget) {
        gabi::call_ptr<void>(widget->mVtable[3].get(), widget, 3);
        mTouchWidget = nullptr;
    }
}

/* 0267FD0C */
void hdSeaChartCell_c::destroy(u32 flags) {
    if (this) {
        mVtable.v = gabi::ea(&sChartCellVtable);
        releaseTouchWidget();
        if (flags & 1)
            seaChartCellFree(this);
    }
}

/* 0267FD68 */
void hdSeaChartCell_c::resetInteraction() {
    const f32 zero = sChartCellZero;
    mInputEntered = 0;
    mPreviousInputPhase = 0;
    mInputPosition.y = zero;
    mInputValid = 0;
    mSelected = 0;
    mInputPhase = 0;
    mFocusRequested = 0;
    mDiscovered = 0;
    mFocused = 0;
    mInputExited = 0;
    mSelectionRequested = 0;
    mInputPosition.x = zero;
    mInputKind = 0;
    mTrackingInput = 0;
}

/* 0267FDC8 */
void hdSeaChartCell_c::playFocus() {
    hdAnimSet* animations = mAnimations;
    if (animations && s32(mFocusEntry) != -1 && s32(mFocusSlot) != -1)
        seaChartAnimationOperationCC(animations, mFocusEntry, mFocusSlot);
}

/* 0267FDF4 */
void hdSeaChartCell_c::resetFocus() {
    hdAnimSet* animations = mAnimations;
    if (animations && s32(mFocusEntry) != -1 && s32(mFocusSlot) != -1)
        seaChartAnimationOperationD4(animations, mFocusEntry, mFocusSlot);
}

/* 0267FE20 */
void hdSeaChartCell_c::resetSelectionAnimations() {
    hdAnimSet* animations = mAnimations;
    if (!animations)
        return;
    if (s32(mSelectionEntry) != -1 && s32(mSelectionSlot) != -1)
        seaChartAnimationOperationD4(animations, mSelectionEntry, mSelectionSlot);
    if (s32(mSelectionLoopEntry) != -1 && s32(mSelectionLoopSlot) != -1)
        seaChartAnimationOperationD4(mAnimations, mSelectionLoopEntry, mSelectionLoopSlot);
}

/* 0267FE90 */
void hdSeaChartCell_c::playSelectionOperations() {
    hdAnimSet* animations = mAnimations;
    if (!animations)
        return;
    if (s32(mSelectionEntry) != -1 && s32(mSelectionSlot) != -1)
        seaChartAnimationOperationCC(animations, mSelectionEntry, mSelectionSlot);
    if (s32(mSelectionLoopEntry) != -1 && s32(mSelectionLoopSlot) != -1)
        seaChartAnimationOperationCC(mAnimations, mSelectionLoopEntry, mSelectionLoopSlot);
}

/* 0267FF00 */
void hdSeaChartCell_c::applyFocusAnimation() {
    if (mFocused)
        playFocus();
    else
        resetFocus();
    if (mDiscovered)
        resetSelectionAnimations();
    else
        playSelectionOperations();
}

/* 0267FF78 */
void hdSeaChartCell_c::clearInputState() {
    mInputValid = 0;
    mFocused = 0;
    mFocusRequested = 0;
    mSelectionRequested = 0;
    mTrackingInput = 0;
    mSelected = 0;
    resetFocus();
}

/* 0267FF98 */
void hdSeaChartCell_c::finishInput() {
    const u32 previousPhase = mInputPhase;
    mInputKind = 0;
    mPreviousInputPhase = previousPhase;
    mInputExited = 0;
    mInputEntered = 0;
    mSelectionRequested = 0;
    mFocusRequested = 0;
    mInputPhase = 0;
}

/* 0267FFC0 */
u32 hdSeaChartCell_c::isPointInput(u32 kind) {
    return kind - 1 <= 2;
}

/* 0267FFDC */
u32 hdSeaChartCell_c::containsScaledPoint(HdPadPoint* point) {
    const f32 x = point->x;
    if (f32(mScaledBounds.mLeft) > x)
        return false;
    if (x > f32(mScaledBounds.mRight))
        return false;
    const f32 y = point->y;
    if (f32(mScaledBounds.mTop) > y)
        return false;
    if (y > f32(mScaledBounds.mBottom))
        return false;
    return true;
}

/* 02680088 */
u32 hdSeaChartCell_c::canBeginFocus() {
    const u32 previous = mPreviousInputPhase;
    if (previous != 0 && previous != 3 && previous != 5)
        return 0;
    const u32 phase = mInputPhase;
    return phase == 2 || phase == 3;
}

/* 02680024 */
void hdSeaChartCell_c::readPointInput() {
    hdInputManager* input = sChartCellInputManager.mValue;
    const bool hasPointer = (u32(input->mPointer.mPointerFlags) & 1) != 0;
    mInputValid = 0;
    const u32 kind = mInputKind;
    if (kind < 1 || kind > 3)
        return;
    if (kind < 3) {
        if (hasPointer) {
            mInputPosition.x = input->mPointer.mCalibratedX;
            mInputPosition.y = input->mPointer.mCalibratedY;
        } else {
            mInputPosition.x = sChartCellFallbackPointer.x;
            mInputPosition.y = sChartCellFallbackPointer.y;
        }
    }
    mInputValid = 1;
}

/* 026800C8 */
u32 hdSeaChartCell_c::shouldEndFocus() {
    if (mSelected && mFocused)
        return 1;
    const u32 previous = mPreviousInputPhase;
    return previous == 2 || previous == 1 || previous == 4;
}

/* 0268010C */
void hdSeaChartCell_c::processPointInput() {
    if (!mInputEnabled)
        return;
    readPointInput();
    if (!mInputValid)
        return;
    if (containsScaledPoint(&mInputPosition)) {
        const u32 kind = mInputKind;
        if (kind >= 1 && kind <= 3)
            mInputPhase = sChartCellPointPhases.mValues[kind - 1];
        if (canBeginFocus())
            mInputPhase = 4;
    } else if (shouldEndFocus()) {
        mInputPhase = 5;
    }
    switch (u32(mInputPhase)) {
    case 1:
        mInputEntered = 1;
        // Fall through to the focus acquired path.
    case 4:
        seaChartCellSetEventMode(sChartCellEventManager.mValue, mTouchWidget);
        mFocused = 1;
        mTrackingInput = 1;
        seaChartCellPlaySound("SE_DRC_TOUCH_ON");
        return;
    case 3:
        mInputExited = 1;
        if (!mSelected || mSelectionReleased) {
            mInputPhase = 0;
            mSelected = 1;
        } else {
            mSelected = 0;
            mFocused = 0;
        }
        return;
    case 5:
        mFocused = 0;
        mTrackingInput = 0;
        mSelected = 0;
        return;
    default:
        return;
    }
}

/* 026802B4 */
u32 hdSeaChartCell_c::isWidgetInput(u32 kind) {
    return kind - 7 <= 7;
}

/* 026802D0 */
HdInputReceiver* hdSeaChartCell_c::getWidgetTarget(u32 index) {
    HdInputReceiver* widget = mTouchWidget;
    return widget ? seaChartWidgetUserData(widget, index) : nullptr;
}

/* 026802FC */
HdInputReceiver* hdSeaChartCell_c::getInputTarget(u32 kind) {
    return isWidgetInput(kind) ? getWidgetTarget(kind - 7) : nullptr;
}

/* 0268037C */
void hdSeaChartCell_c::processWidgetInput() {
    if (!mTrackingInput)
        return;
    HdInputReceiver* target = getInputTarget(mInputKind);
    if (!target)
        return;
    hdSeaChartCell_c* cell = seaChartFindWidgetCell(mGrid, target);
    if (!cell)
        return;
    cell->requestFocus();
    if (mFocused)
        cell->requestSelection();
    stopTrackingInput();
    if (mFocused)
        cancelFocus();
}

/* 02680404 */
void hdSeaChartCell_c::updateInput() {
    const u32 kind = mInputKind;
    if (!kind)
        return;
    if (isPointInput(kind))
        processPointInput();
    else if (isWidgetInput(kind))
        processWidgetInput();
}

/* 02680340 */
void hdSeaChartCell_c::requestFocus() {
    mFocusRequested = 1;
}

/* 0268034C */
void hdSeaChartCell_c::requestSelection() {
    mSelectionRequested = 1;
}

/* 02680358 */
void hdSeaChartCell_c::stopTrackingInput() {
    mTrackingInput = 0;
}

/* 02680364 */
void hdSeaChartCell_c::cancelFocus() {
    mInputPhase = 5;
    mFocused = 0;
    mSelected = 0;
}

/* 0268046C */
void hdSeaChartCell_c::applyRequests() {
    const u8 focus = mFocusRequested;
    const u8 selection = mSelectionRequested;
    if (focus)
        mTrackingInput = 1;
    if (selection) {
        mInputPhase = 4;
        mSelected = 1;
        mFocused = 1;
    }
}

/* 0268056C */
void hdSeaChartCell_c::beginSelection() {
    mSelected = 1;
}

/* 02680508 */
void hdSeaChartCell_c::createTouchWidget() {
    hdScreenHeap* heap = sChartCellUiSystem.mValue->mHeap;
    if (mTouchPane) {
        HdInputReceiver* receiver = static_cast<HdInputReceiver*>(
            seaChartCellAllocateOnHeap(sizeof(HdInputReceiver), heap, 4));
        if (receiver)
            receiver = seaChartCellConstructReceiver(receiver, mTouchPane, 0);
        mTouchWidget = receiver;
    }
}

/* 026804A4 */
void hdSeaChartCell_c::playFocusAnimation() {
    hdAnimSet* animations = mAnimations;
    if (animations && s32(mFocusEntry) != -1 && s32(mFocusSlot) != -1)
        seaChartPlayAnimation(animations, mFocusEntry, mFocusSlot, sChartCellAnimationRate);
}

/* 026804D8 */
void hdSeaChartCell_c::playFocusAnimationForPhase() {
    switch (u32(mInputPhase)) {
    case 1:
    case 4:
        playFocus();
        return;
    case 3:
    case 5:
        playFocusAnimation();
        return;
    default:
        return;
    }
}

/* 02680578 */
void hdSeaChartCell_c::playSelectionAnimations() {
    hdAnimSet* animations = mAnimations;
    if (!animations)
        return;
    const f32 rate = sChartCellAnimationRate;
    if (s32(mSelectionEntry) != -1 && s32(mSelectionSlot) != -1)
        seaChartPlayAnimation(animations, mSelectionEntry, mSelectionSlot, rate);
    if (s32(mSelectionLoopEntry) != -1 && s32(mSelectionLoopSlot) != -1)
        seaChartPlayAnimation(mAnimations, mSelectionLoopEntry, mSelectionLoopSlot, rate);
}

/* 02680610 */
u32 hdSeaChartCell_c::isSelectionAnimationEnd() {
    hdAnimSet* animations = mAnimations;
    if (animations && s32(mSelectionEntry) != -1)
        return seaChartAnimationEnd(animations, mSelectionEntry);
    return 1;
}

/* 02680638 */
void hdSeaChartCell_c::playMarkAnimation() {
    hdAnimSet* animations = mAnimations;
    if (animations && s32(mMarkEntry) != -1 && s32(mMarkSlot) != -1) {
        seaChartPlayAnimation(animations, mMarkEntry, mMarkSlot, sChartCellAnimationRate);
        mMarkAnimationActive = 1;
    }
}

/* 02680698 */
void hdSeaChartCell_c::resetMarkAnimation() {
    hdAnimSet* animations = mAnimations;
    if (animations && s32(mMarkEntry) != -1 && s32(mMarkSlot) != -1) {
        seaChartAnimationOperationD4(animations, mMarkEntry, mMarkSlot);
        mMarkAnimationActive = 1;
    }
}

/* 026806F0 */
void hdSeaChartCell_c::activateMarkAnimation() {
    hdAnimSet* animations = mAnimations;
    if (animations && s32(mMarkEntry) != -1 && s32(mMarkSlot) != -1) {
        seaChartAnimationOperationCC(animations, mMarkEntry, mMarkSlot);
        mMarkAnimationActive = 0;
    }
}

/* 02680748 */
void hdSeaChartCell_c::setWidgetUserData(u32 index, u32 value) {
    HdInputReceiver* widget = mTouchWidget;
    if (widget)
        seaChartSetWidgetUserData(widget, index, value);
}

/* 02680758 */
void seaChartInitializeCellStatics() {
    sChartCellStatics.mStorage[3] = 0;
    sChartCellStatics.mStorage[2] = 0;
    sChartCellStatics.mStorage[1] = 0;
    sChartCellStatics.mStorage[0] = 0;
    seaChartCellRegisterStatic(&sChartCellRegistrations[0]);
    const f32 minimum = sChartCellMinimum;
    const f32 maximum = sChartCellMaximum;
    sChartCellStatics.mMinimum = minimum;
    sChartCellStatics.mMaximum = maximum;
    seaChartCellInitializeFirstFlag(&sChartCellStatics.mFirstFlag);
    seaChartCellRegisterStatic(&sChartCellRegistrations[1]);
    seaChartCellInitializeSecondFlag(&sChartCellStatics.mSecondFlag);
    seaChartCellRegisterStatic(&sChartCellRegistrations[2]);
    const f32 origin = sChartCellDefaultOrigin;
    const f32 offset = sChartCellDefaultOffset;
    sChartCellStatics.mDefaultOrigin.x = origin;
    sChartCellStatics.mDefaultOffset.x = offset;
    sChartCellStatics.mDefaultOrigin.y = origin;
    sChartCellStatics.mDefaultOffset.y = offset;
}

/* WWHD glue (tools/verify/autoglue.py)
 data sChartCellVtable 100F91B8 nocheck
data sChartCellAnimationRate 100F9178
data sChartCellZero 100F9190
data sChartCellStatics 1049617C
data sChartCellRegistrations 101F6248 nocheck
data sChartCellMinimum 100F91A8
data sChartCellMaximum 100F91AC
data sChartCellDefaultOrigin 100F91B0
data sChartCellDefaultOffset 100F91B4
ext 028F026C void seaChartCellRegisterStatic(hdSeaChartCellRegistration_c*)
ext 028ED6F8 void seaChartCellInitializeFirstFlag(be<u8>*)
ext 028EAB2C void seaChartCellInitializeSecondFlag(be<u8>*)
data sChartCellInputManager 101F5088
data sChartCellUiSystem 1018C404
data sChartCellFallbackPointer 1049FFA0
data sChartCellEventManager 101F4FF0
data sChartCellPointPhases 100F9194 nocheck
str 100F9198 "SE_DRC_TOUCH_ON"
ext 02615154 void seaChartCellSetEventMode(void*, HdInputReceiver*)
ext 02030B38 void seaChartCellPlaySound(const char*)
ext 02682C30 hdSeaChartCell_c* seaChartFindWidgetCell(HdPadGrid*, HdInputReceiver*)
ext 0273B050 void* seaChartCellAllocateOnHeap(u32, hdScreenHeap*, u32)
ext 02616ED4 HdInputReceiver* seaChartCellConstructReceiver(HdInputReceiver*, void*, u32)
ext 02005674 void seaChartAnimationOperationCC(hdAnimSet*, u32, u32)
ext 02005708 void seaChartAnimationOperationD4(hdAnimSet*, u32, u32)
ext 020053E4 void seaChartPlayAnimation(hdAnimSet*, u32, u32, f32)
ext 02005840 u32 seaChartAnimationEnd(hdAnimSet*, u32)
ext 02001C00 HdInputReceiver* seaChartWidgetUserData(HdInputReceiver*, u32)
ext 02001BE4 void seaChartSetWidgetUserData(HdInputReceiver*, u32, u32)
ext 0273AF40 void seaChartCellFree(void*)
ext 0273AD10 void* seaChartCellAllocate(u32)
*/

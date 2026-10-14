#include "d/hd_sea_chart_state_handlers.h"
using namespace gabi;
static hdPartsStateId_c sTransitionBlankState;
static hdPartsStateId_c sTransitionDisplayState;
static hdPartsStateId_c sTransitionInState;
static hdPartsStateId_c sTransitionOutState;
static u32 sTransitionVtable;
static u32 sTransitionHolderVtable;
static u32 sTransitionSecondaryVtable;
static f32 sTransitionUnitScale;
static f32 sTransitionFullAlpha;
static f32 sTransitionZero;
static f32 sTransitionAlphaStep;
static f32 sTransitionAlphaRate;
static f32 sTransitionAlphaLimit;
void seaChartTransitionApproachAlpha(be<f32>* alpha, f32 target, f32 step, f32 rate, f32 limit);
void seaChartTransitionBaseDisplay(hdParts_c* self);
void seaChartTransitionApply(hdSeaChartStateHandlers_c* self);
void seaChartTransitionRotatePoint(HdPadPoint* output, HdPadPoint* input, HdPadPoint* offset,
                                   f32 angle);
void seaChartTransitionCopyAnchorPosition(hdSeaChartTransform_c* transform, HdPadPoint* output);
void* seaChartTransitionAllocate(u32 size);
hdParts_c* seaChartTransitionConstructParts(hdParts_c* self);
void seaChartTransitionDestroyParts(hdParts_c* self, s32 flags);
void seaChartTransitionFree(void* self);
void seaChartTransitionConstructPane(hdSeaChartPaneTransition_c* transition);
void seaChartTransitionReleasePane(hdSeaChartPaneTransition_c* transition);
void seaChartTransitionDestroyPane(hdSeaChartPaneTransition_c* transition, s32 flags);
void seaChartTransitionApplyPane(hdSeaChartPaneTransition_c* transition);
void seaChartTransitionGetPanePosition(HdPadPoint3* output, hdSeaChartPane_l* pane);
void seaChartTransitionChangeState(hdStateMachine* machine, hdPartsStateId_c* state);

/* 02684D94 */
s32 seaChartTransitionStateNumber(hdPartsStateId_c* state) {
    return state->mNumber;
}
/* 02684E94 */
void seaChartTransitionDestroy(hdSeaChartStateHandlers_c* self, s32 flags) {
    if (!self)
        return;
    self->mVtable.v = gabi::ea(&sTransitionVtable);
    self->mSecondaryVtable.v = gabi::ea(&sTransitionSecondaryVtable);
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sTransitionHolderVtable);
    seaChartTransitionReleasePane(&self->mTransition);
    self->mMap = nullptr;
    self->mViewport = nullptr;
    seaChartTransitionDestroyPane(&self->mTransition, 2);
    seaChartTransitionDestroyParts(self, 0);
    if (flags & 1)
        seaChartTransitionFree(self);
}

static hdSeaChartPaneTransition_c* chartTransitionEmbedded(hdSeaChartStateHandlers_c* self) {
    // Forward the exact wrapped guest address, including null, to the game constructor.
    return gabi::at<hdSeaChartPaneTransition_c>(
        u32(gabi::ea(self) + offsetof(hdSeaChartStateHandlers_c, mTransition)));
}

/* 02685134 */
hdSeaChartStateHandlers_c* seaChartTransitionConstruct(hdSeaChartStateHandlers_c* self) {
    if (!self) {
        self = static_cast<hdSeaChartStateHandlers_c*>(
            seaChartTransitionAllocate(sizeof(hdSeaChartStateHandlers_c)));
        if (!self)
            return nullptr;
    }
    seaChartTransitionConstructParts(self);
    self->mVtable.v = gabi::ea(&sTransitionVtable);
    self->mSecondaryVtable.v = gabi::ea(&sTransitionSecondaryVtable);
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sTransitionHolderVtable);
    seaChartTransitionConstructPane(chartTransitionEmbedded(self));
    const f32 zero = sTransitionUnitScale;
    self->mMap = nullptr;
    self->mMinimumBlend = zero;
    self->mViewport = nullptr;
    self->mMaximumBlend = zero;
    return self;
}

/* 02684F34 */
void hdSeaChartStateHandlers_c::calcLayout() {
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 02684F44 */
void hdSeaChartStateHandlers_c::clearAlpha() {
    mTransition.mAlpha = 0;
}
/* 0268503C */
void hdSeaChartStateHandlers_c::fillAlpha() {
    mTransition.mAlpha = 255;
}
static u8 chartTransitionAdvanceAlpha(hdSeaChartStateHandlers_c* self, f32 target) {
    const f32 full = sTransitionFullAlpha;
    Local<be<f32>> normalized;
    *normalized = f32(f64(f32(u8(self->mTransition.mAlpha))) / f64(full));
    seaChartTransitionApproachAlpha(normalized, target, sTransitionAlphaStep, sTransitionAlphaRate,
                                    sTransitionAlphaLimit);
    const u8 alpha = u8(ftoi(fmuls_ppc(full, *normalized)));
    auto* table = self->mVtable.get();
    self->mTransition.mAlpha = alpha;
    gabi::call_ptr<void>(table->mCalcLayout.get(), self);
    return alpha;
}
/* 02684F50 */
void hdSeaChartStateHandlers_c::executeIn() {
    if (chartTransitionAdvanceAlpha(this, sTransitionUnitScale) == 255)
        seaChartTransitionChangeState(stateMachine(), &sTransitionDisplayState);
}
/* 02685048 */
void hdSeaChartStateHandlers_c::executeOut() {
    if (chartTransitionAdvanceAlpha(this, sTransitionZero) == 0)
        seaChartTransitionChangeState(stateMachine(), &sTransitionBlankState);
}
/* 026852BC */
void hdSeaChartStateHandlers_c::executeDisplay() {
    gabi::call_ptr<void>(vtbl()->mUpdatePosition.get(), this);
    gabi::call_ptr<void>(vtbl()->mUpdateRotation.get(), this);
    updateBlend();
    seaChartTransitionApply(this);
    seaChartTransitionBaseDisplay(this);
}
/* 0268531C */
void hdSeaChartStateHandlers_c::updatePosition() {
    Local<HdPadPoint> position;
    position->x = sTransitionZero;
    position->y = sTransitionZero;
    gabi::call_ptr<void>(vtbl()->mScaleMapPoint.get(), this, &mTransition.mSourcePosition,
                         position.get());
    gabi::call_ptr<void>(vtbl()->mRotateViewportPoint.get(), this, position.get(), position.get());
    gabi::call_ptr<void>(vtbl()->mScaleViewportPoint.get(), this, position.get(), position.get());
    gabi::call_ptr<void>(vtbl()->mOffsetViewportPoint.get(), this, position.get(), position.get());
    mTransition.mPosition.x = position->x;
    mTransition.mPosition.y = position->y;
}

/* 026851C4 */
void hdSeaChartStateHandlers_c::requestIn() {
    seaChartTransitionChangeState(stateMachine(), &sTransitionInState);
}
/* 026851D4 */
void hdSeaChartStateHandlers_c::requestOut() {
    seaChartTransitionChangeState(stateMachine(), &sTransitionOutState);
}
/* 026851E4 */
void hdSeaChartStateHandlers_c::displayImmediately() {
    seaChartTransitionChangeState(stateMachine(), &sTransitionDisplayState);
    mTransition.mAlpha = 255;
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 02685238 */
void hdSeaChartStateHandlers_c::blankImmediately() {
    seaChartTransitionChangeState(stateMachine(), &sTransitionBlankState);
    mTransition.mAlpha = 0;
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 0268528C */
void hdSeaChartStateHandlers_c::updateBlend() {
    hdSeaChartTransitionMap_l* map = mMap;
    if (map) {
        const f32 maximum = mMaximumBlend;
        const f32 minimum = mMinimumBlend;
        const f32 blend = map->mBlend;
        mTransition.mBlend = fmadds(blend, fsubs_ppc(maximum, minimum), minimum);
    }
}
/* 026852B4 */
void seaChartTransitionApply(hdSeaChartStateHandlers_c* self) {
    seaChartTransitionApplyPane(&self->mTransition);
}
/* 026853E8 */
void seaChartTransitionSetPosition(hdSeaChartStateHandlers_c* self, hdSeaChartPane_l* pane) {
    if (pane) {
        Local<HdPadPoint3> position;
        seaChartTransitionGetPanePosition(position, pane);
        self->mTransition.mPosition.x = position->x;
        self->mTransition.mPosition.y = position->y;
    }
}

/* 026853D0 */
void hdSeaChartStateHandlers_c::updateRotation() {
    if (mMap)
        mTransition.mRotation = mMap->mRotation;
}
/* 02685430 */
void hdSeaChartStateHandlers_c::scaleMapPoint(HdPadPoint* input, HdPadPoint* output) {
    hdSeaChartTransitionMap_l* map = mMap;
    if (!map || !output || !map->mScale)
        return;
    const f32 scale = *map->mScale;
    output->x = fmuls_ppc(input->x, scale);
    output->y = -fmuls_ppc(input->y, scale);
}
/* 02685478 */
void hdSeaChartStateHandlers_c::rotateViewportPoint(HdPadPoint* input, HdPadPoint* output) {
    if (!output || !mViewport || !mMap)
        return;
    hdSeaChartTransform_c* viewport = mViewport;
    Local<HdPadPoint> offset;
    auto* destination = reinterpret_cast<be<u32>*>(offset.get());
    auto* source = reinterpret_cast<be<u32>*>(&viewport->mPreviousPosition);
    destination[0] = u32(source[0]);
    destination[1] = u32(source[1]);
    seaChartTransitionRotatePoint(output, input, offset, mMap->mRotation);
}
/* 02685534 */
void hdSeaChartStateHandlers_c::offsetViewportPoint(HdPadPoint* input, HdPadPoint* output) {
    if (!output || !mViewport)
        return;
    Local<HdPadPoint> offset;
    offset->y = sTransitionZero;
    offset->x = sTransitionZero;
    seaChartTransitionCopyAnchorPosition(mViewport, offset);
    const f32 x = fadds_ppc(input->x, offset->x);
    const f32 y = fadds_ppc(input->y, offset->y);
    output->x = x;
    output->y = y;
}
/* 026855D0 */
void hdSeaChartStateHandlers_c::bindMap(hdSeaChartTransitionMap_l* map) {
    if (map) {
        mMap = map;
        mViewport = gabi::call_ptr<hdSeaChartTransform_c*>(map->mVtable->mGetViewport.get(), map,
                                                           u32(mTransition.mPaneIndex));
    }
}

/* 026854D8 */
void hdSeaChartStateHandlers_c::scaleViewportPoint(HdPadPoint* input, HdPadPoint* output) {
    hdSeaChartTransform_c* viewport = mViewport;
    if (!output || !viewport)
        return;
    const f32 x = input->x;
    const f32 scale = viewport->mScaleMultiplier;
    const f32 y = input->y;
    const f32 scaledX = fmuls_ppc(x, scale);
    const f32 scaledY = fmuls_ppc(y, scale);
    output->x = scaledX;
    output->y = scaledY;
}
static hdSeaChartPane_l* chartTransitionFindPane(hdPartsLyt_c* layout, hdUiSafeString* name) {
    name->assureTermination();
    auto* root = reinterpret_cast<hdSeaChartPane_l*>(layout->mpRootPane.get());
    auto* table = reinterpret_cast<hdButtonPaneVtable_l*>(root->mVtable.get());
    return reinterpret_cast<hdSeaChartPane_l*>(
        gabi::call_ptr<hdPartsPane_c*>(table->mFindPane.get(), root, name->mText.get(), 1));
}
static void chartTransitionBindPanes(hdSeaChartStateHandlers_c* self, hdSeaChartPane_l* root,
                                     hdSeaChartPane_l* target) {
    seaChartTransitionReleasePane(&self->mTransition);
    self->mTransition.mRoot = root;
    self->mTransition.mTarget = target;
    self->mMap = nullptr;
    self->mViewport = nullptr;
    // Both coordinates are read before either is stored (y, then x), as in the binary: the transition's
    // position may alias the pane.
    f32 y = root->mPosition.y;
    f32 x = root->mPosition.x;
    self->mTransition.mPosition.y = y;
    self->mTransition.mPosition.x = x;
    self->mTransition.mScale = root->mScale.x;
    self->mTransition.mRotation = root->mRotation.z;
    self->mTransition.mDirty = 0;
    self->mTransition.mAlpha = 0;
    self->mTransition.mHidden = 0;
}
static void chartTransitionStartBlank(hdSeaChartStateHandlers_c* self) {
    gabi::call_ptr<void>(self->mVtable->mCalcLayout.get(), self);
    hdStateMachine* machine = self->stateMachine();
    machine->mpCurrentState =
        machine->mpOwner->createState(reinterpret_cast<hdStateId*>(&sTransitionBlankState));
    machine->mpCurrentState->enter();
}
/* 0268562C */
void hdSeaChartStateHandlers_c::bindPane(hdUiSafeString* name) {
    const bool empty = *name->mText.get() == 0;
    hdPartsLyt_c* layout = mpLayout->lyt();
    hdSeaChartPane_l* pane;
    if (empty) {
        pane = reinterpret_cast<hdSeaChartPane_l*>(layout->mpRootPane.get());
        if (pane)
            pane->mFlags = u8(pane->mFlags) | 2;
    } else {
        pane = chartTransitionFindPane(layout, name);
    }
    if (pane)
        chartTransitionBindPanes(this, pane, pane);
    chartTransitionStartBlank(this);
}
/* 02685784 */
void hdSeaChartStateHandlers_c::bindRootAndPane(hdUiSafeString* name) {
    hdPartsLyt_c* layout = mpLayout->lyt();
    auto* root = reinterpret_cast<hdSeaChartPane_l*>(layout->mpRootPane.get());
    if (root)
        root->mFlags = u8(root->mFlags) | 2;
    // The original reloads the layout after the root's visibility flag store.
    layout = mpLayout->lyt();
    hdSeaChartPane_l* target = chartTransitionFindPane(layout, name);
    if (root && target)
        chartTransitionBindPanes(this, root, target);
    chartTransitionStartBlank(this);
}

/* 02685620 */
void hdSeaChartStateHandlers_c::setBlendRange(f32 minimum, f32 maximum) {
    mMinimumBlend = minimum;
    mMaximumBlend = maximum;
}

struct hdSeaChartTransitionStatics_c {
    be<f32> mMinimum, mMaximum;
    HdPadPoint mOrigin, mOffset;
    be<u8> mFirstFlag, mSecondFlag;
    u8 mPadding[2];
    be<u32> mStorage[4];
};
WWHD_OFFSET(hdSeaChartTransitionStatics_c, mMinimum, 0);
WWHD_OFFSET(hdSeaChartTransitionStatics_c, mMaximum, 4);
WWHD_OFFSET(hdSeaChartTransitionStatics_c, mOrigin, 8);
WWHD_OFFSET(hdSeaChartTransitionStatics_c, mOffset, 0x10);
WWHD_OFFSET(hdSeaChartTransitionStatics_c, mFirstFlag, 0x18);
WWHD_OFFSET(hdSeaChartTransitionStatics_c, mSecondFlag, 0x19);
WWHD_OFFSET(hdSeaChartTransitionStatics_c, mStorage, 0x1C);
struct hdSeaChartTransitionRegistration_c {
    be<u32> mWords[3];
};
struct hdSeaChartTransitionCounter_c {
    be<u32> mValue;
};
static hdSeaChartTransitionStatics_c sTransitionStatics;
static hdSeaChartTransitionRegistration_c sTransitionStorageRegistration;
static hdSeaChartTransitionRegistration_c sTransitionFirstFlagRegistration;
static hdSeaChartTransitionRegistration_c sTransitionSecondFlagRegistration;
static hdSeaChartTransitionCounter_c sTransitionStateGuard;
static hdSeaChartTransitionCounter_c sTransitionStateCounter;
static hdStateId sTransitionDisplayNull;
static hdStateId sTransitionBlankNull;
static hdStateId sTransitionTransitionNull;
static hdPartsStateIdVtbl_c sTransitionStateVtable;
static f32 sTransitionMinimum;
static f32 sTransitionMaximum;
static f32 sTransitionOrigin;
static f32 sTransitionOffset;
void seaChartTransitionRegisterStatic(hdSeaChartTransitionRegistration_c* record);
void seaChartTransitionFirstFlag(be<u8>* flag);
void seaChartTransitionSecondFlag(be<u8>* flag);

/* 026858C0 */
void seaChartTransitionInitializeStatics() {
    sTransitionStatics.mStorage[3] = 0;
    sTransitionStatics.mStorage[2] = 0;
    sTransitionStatics.mStorage[1] = 0;
    sTransitionStatics.mStorage[0] = 0;
    seaChartTransitionRegisterStatic(&sTransitionStorageRegistration);
    const f32 minimum = sTransitionMinimum;
    const f32 maximum = sTransitionMaximum;
    sTransitionStatics.mMinimum = minimum;
    sTransitionStatics.mMaximum = maximum;
    seaChartTransitionFirstFlag(&sTransitionStatics.mFirstFlag);
    seaChartTransitionRegisterStatic(&sTransitionFirstFlagRegistration);
    seaChartTransitionSecondFlag(&sTransitionStatics.mSecondFlag);
    seaChartTransitionRegisterStatic(&sTransitionSecondFlagRegistration);
    const f32 offset = sTransitionOffset;
    const u32 initialized = sTransitionStateGuard.mValue;
    const f32 origin = sTransitionOrigin;
    sTransitionStatics.mOffset.x = offset;
    sTransitionStatics.mOrigin.x = origin;
    sTransitionStatics.mOrigin.y = origin;
    sTransitionStatics.mOffset.y = offset;
    u32 number;
    if (initialized) {
        number = sTransitionStateCounter.mValue;
    } else {
        sTransitionStateGuard.mValue = 1;
        number = 0;
    }
    sTransitionDisplayState.setup(s32(++number), "::StateID_Display", &sTransitionStateVtable, 7,
                                  &sTransitionDisplayNull);
    sTransitionBlankState.setup(s32(++number), "::StateID_Blank", &sTransitionStateVtable, 4,
                                &sTransitionBlankNull);
    sTransitionInState.setup(s32(++number), "::StateID_InAll", &sTransitionStateVtable, 25,
                             &sTransitionTransitionNull);
    sTransitionOutState.setup(s32(++number), "::StateID_OutAll", &sTransitionStateVtable, 28,
                              &sTransitionTransitionNull);
    sTransitionStateCounter.mValue = number;
}

/* WWHD glue (tools/verify/autoglue.py)
 data sTransitionVtable 100F96C0 nocheck
 data sTransitionHolderVtable 100F9800 nocheck
 data sTransitionSecondaryVtable 100F9810 nocheck
data sTransitionStatics 104962C0
data sTransitionStorageRegistration 101F62B4
data sTransitionFirstFlagRegistration 101F62C0
data sTransitionSecondFlagRegistration 101F62CC
data sTransitionStateGuard 101FD8E4
data sTransitionStateCounter 101FDD50
data sTransitionDisplayNull 1049DD90
data sTransitionBlankNull 1049DD60
data sTransitionTransitionNull 101FF32C
data sTransitionStateVtable 100F95FC
data sTransitionMinimum 100F9668
data sTransitionMaximum 100F966C
data sTransitionOrigin 100F9670
data sTransitionOffset 100F9674
ext 028F026C void seaChartTransitionRegisterStatic(hdSeaChartTransitionRegistration_c*)
ext 028ED6F8 void seaChartTransitionFirstFlag(be<u8>*)
ext 028EAB2C void seaChartTransitionSecondFlag(be<u8>*)
str 100F9678 "::StateID_Blank" in 026858C0
str 100F9688 "::StateID_InAll" in 026858C0
str 100F9698 "::StateID_Display" in 026858C0
str 100F96AC "::StateID_OutAll" in 026858C0
data sTransitionUnitScale 100F9648
data sTransitionFullAlpha 100F964C
data sTransitionZero 100F9664
data sTransitionAlphaStep 100F9658
data sTransitionAlphaRate 100F965C
data sTransitionAlphaLimit 100F9660
ext 026893C8 void seaChartTransitionRotatePoint(HdPadPoint*, HdPadPoint*, HdPadPoint*, f32)
ext 0267F8F8 void seaChartTransitionCopyAnchorPosition(hdSeaChartTransform_c*, HdPadPoint*)
ext 02689934 void seaChartTransitionApproachAlpha(be<f32>*, f32, f32, f32, f32)
ext 026F8CD0 void seaChartTransitionBaseDisplay(hdParts_c*)
ext 0273AD10 void* seaChartTransitionAllocate(u32)
ext 026F89E8 hdParts_c* seaChartTransitionConstructParts(hdParts_c*)
ext 026F88F0 void seaChartTransitionDestroyParts(hdParts_c*, s32)
ext 0273AF40 void seaChartTransitionFree(void*)
ext 0268B26C void seaChartTransitionConstructPane(hdSeaChartPaneTransition_c*)
ext 0268B398 void seaChartTransitionReleasePane(hdSeaChartPaneTransition_c*)
ext 0268B400 void seaChartTransitionDestroyPane(hdSeaChartPaneTransition_c*, s32)
ext 0268B434 void seaChartTransitionApplyPane(hdSeaChartPaneTransition_c*)
ext 02704D2C void seaChartTransitionGetPanePosition(HdPadPoint3*, hdSeaChartPane_l*)
data sTransitionDisplayState 104962EC
data sTransitionBlankState 1049631C
data sTransitionInState 1049634C
data sTransitionOutState 1049637C
ext 020063C0 void seaChartTransitionChangeState(hdStateMachine*, hdPartsStateId_c*)
*/

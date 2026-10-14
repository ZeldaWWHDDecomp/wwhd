// HD keyboard text cursor with its looping animation. Private source.
#include "d/hd_keyboard_screen.h"
static u32 sCursorVtable;
static u32 sCursorDelegateA;
static u32 sCursorDelegateB;
static u32 sCursorSafeStringVtable;
static u32 sCursorStateVtable;
static hdUiSafeString sCursorPartName;
static hdUiSafeString sCursorLayoutName;
static hdUiSafeString sCursorAnimationName;
static hdKeyboardStringSingle_c sCursorGroups;
static const u32 sCursorAnimationGroup;
static hdKeyboardStatRange_c sCursorRange;
static hdKeyboardStatRegistrations_c sCursorRegistrations;
static const hdKeyboardStatLimits_c sCursorLimits;
static f32 sCursorInitialFrame;
static hdPartsStateId_c sCursorLoop;
static u32 sCursorStateGuard;
static u32 sCursorNextStateId;
static hdStateId sCursorNullState;
static hdScreenUiSystem* sCursorHeap;
/* 02673964 */
static hdScreenType* keyboardCursorBaseType(hdScreenType* type) {
    return type->mParent;
}
/* 02673A64 */
void hdKeyboardCursor_c::destroy(u32 flags) {
    if (!this)
        return;
    mStateHolder.mHolderVtable.v = gabi::ea(&sCursorDelegateA);
    mSecondaryVtable.v = gabi::ea(&sCursorDelegateB);
    keyboardLoopDestroy(this, 0);
    if (flags & 1)
        keyboardFree(this);
}
/* 02673AD0 */
void hdKeyboardCursor_c::enterLoop() {
    keyboardSetAnimationFrame(mpLayout->animSet(), 0, 0, sCursorInitialFrame);
}
/* 02673AEC */
void hdKeyboardCursor_c::startLoop() {
    gabi::tail_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 02673AFC */
static hdKeyboardCursor_c* constructKeyboardCursor(hdKeyboardCursor_c* self) {
    if (!self) {
        self = static_cast<hdKeyboardCursor_c*>(keyboardAllocate(sizeof(hdKeyboardCursor_c)));
        if (!self)
            return NULL;
    }
    keyboardLoopConstruct(self);
    self->mVtable.v = gabi::ea(&sCursorVtable);
    self->mStateHolder.mHolderVtable.v = gabi::ea(&sCursorDelegateA);
    self->mSecondaryVtable.v = gabi::ea(&sCursorDelegateB);
    return self;
}
/* 02673B68 */
u32 hdKeyboardCursor_c::create(u32 parent, u32 archive, u32 layout) {
    keyboardSetLayoutName(this, &sCursorPartName, 1);
    u32 result = gabi::call_ptr<u32>(mVtable->mLoad.get(), this, &sCursorLayoutName,
                                     mpResAccessor.get(), parent, archive, layout);
    if (!result)
        return 0;
    hdStateOwner* state = mStateHolder.mMachine.mpOwner;
    hdStateObject* action =
        gabi::call_ptr<hdStateObject*>(state->mpVTable->mCreateState.get(), state, &sCursorLoop);
    mStateHolder.mMachine.mpCurrentState = action;
    gabi::call_ptr<void>(action->mpVTable->mEnter.get(), action);
    return 1;
}
/* 02673C50 */
void hdKeyboardCursor_c::update() {
    gabi::call_ptr<void>(mVtable->mDestroyLayout.get(), this);
    keyboardLoopRemove(this);
}
/* 02673C90 */
void hdKeyboardCursor_c::executeState() {
    keyboardExecuteState(&mStateHolder.mMachine);
}
/* 02673C98 */
void hdKeyboardCursor_c::setPosition(const be<u32>* position) {
    hdKeyboardPane_l* root = reinterpret_cast<hdKeyboardPane_l*>(mpLayout->lyt()->mpRootPane.get());
    u32 x = position[0];
    u8 flags = root->mFlags;
    root->mPosition[0] = x;
    u32 y = position[1];
    root->mPosition[1] = y;
    u32 z = position[2];
    root->mFlags = flags | 0x10;
    root->mPosition[2] = z;
}
/* 02673CCC */
u32 hdKeyboardCursor_c::isAnimationFinished() {
    return keyboardLayoutFinished(mpLayout);
}
/* 02673CD4 */
u32 hdKeyboardCursor_c::createLayout(u32 resource, u32 parent, u32 archive, u32 pane, u32 flags) {
    hdPartsLayout_c* layout =
        static_cast<hdPartsLayout_c*>(keyboardAllocateAligned(0x110, sCursorHeap->mHeap, 4));
    if (layout)
        layout = keyboardConstructLayout(layout);
    mpLayout = layout;
    if (!layout)
        return 0;
    u32 result = gabi::call_ptr<u32>(layout->vtbl()->mLoad.get(), layout, resource, parent,
                                     archive, pane, 1, 1, flags);
    if (!result)
        return 0;
    u32 group = sCursorAnimationGroup;
    keyboardBindAnimation(mpLayout->animSet(), 0, &sCursorAnimationName,
                          &sCursorGroups.mItems[group]);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    return 1;
}
/* 02673DD4 */
static void initializeKeyboardCursor() {
    initializeKeyboardStatics(&sCursorRange, &sCursorRegistrations, &sCursorLimits);
    sCursorGroups.mItems[0].mVtable.v = gabi::ea(&sCursorSafeStringVtable);
    sCursorGroups.mItems[0].mText.v = gabi::guest_str("G_LoopAll");
    sCursorAnimationName.mVtable.v = gabi::ea(&sCursorSafeStringVtable);
    sCursorAnimationName.mText.v = gabi::guest_str("LoopAll");
    sCursorPartName.mVtable.v = gabi::ea(&sCursorSafeStringVtable);
    sCursorPartName.mText.v = gabi::guest_str("KeyboardTextCursor");
    sCursorLayoutName.mVtable.v = gabi::ea(&sCursorSafeStringVtable);
    sCursorLayoutName.mText.v = gabi::guest_str("KeyboardTextCursor");
    u32 previous;
    if (sCursorStateGuard)
        previous = sCursorNextStateId;
    else {
        sCursorStateGuard = 1;
        previous = 0;
    }
    initializeKeyboardState(&sCursorLoop, previous + 1, "::StateID_LoopAll", gabi::ea(&sCursorStateVtable),
                            0x19, &sCursorNullState);
    sCursorNextStateId = previous + 1;
}
/* WWHD glue (tools/verify/autoglue.py)
 data sCursorVtable 100F6D00 nocheck
 data sCursorDelegateA 100F6DE0 nocheck
 data sCursorDelegateB 100F6DF0 nocheck
 data sCursorSafeStringVtable 100F6C10 nocheck
 data sCursorStateVtable 100F6C70 nocheck
 data sCursorPartName 10494F6C
 data sCursorLayoutName 10494FC0
 data sCursorAnimationName 10494F64
 data sCursorGroups 10494F5C
 data sCursorAnimationGroup 100F6C0C
 data sCursorRange 10494F74
 data sCursorRegistrations 101F6104
 data sCursorLimits 100F6CBC
 data sCursorInitialFrame 100F6CB8
 data sCursorLoop 10494F90
 data sCursorStateGuard 101FD8E4
 data sCursorNextStateId 101FDD50
 data sCursorNullState 101FF32C
 data sCursorHeap 1018C404
 str 100F6CCC "G_LoopAll"
 str 100F6CC4 "LoopAll"
 str 100F6CEC "KeyboardTextCursor"
 str 100F6CD8 "::StateID_LoopAll"
*/

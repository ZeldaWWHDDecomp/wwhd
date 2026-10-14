/** HD Heart UI part: fill value, visibility and transitions between full/normal hearts. */
#include "d/hd_parts_heart.h"
static u32 sPartVtable;
static u32 sInterfaceVtable;
static u32 sHeartVtable;
static hdPartsStateId_c sWaitState;
static hdPartsStateId_c sToMaxState;
static hdPartsStateId_c sToNormalState;
static hdHeartSave_l* sSave;
static hdHeartHud_l sHud;
static hdScreenUiSystem* sHeapManager;
void hdHeart_baseDestroy(hdHeartPart_c*, u32);
void hdHeart_baseConstruct(hdHeartPart_c*);
void hdHeart_changeState(hdStateMachine*, hdPartsStateId_c*);
void hdHeart_updateState(hdStateMachine*);
void hdHeart_play(void*, u32, u32, f32);
void hdHeart_stop(void*, u32, u32);
void hdHeart_pause(void*, u32, u32);
BOOL hdHeart_isFinished(void*, u32);
void hdHeart_setFrame(void*, u32, u32, f32);
void* hdHeart_allocate(u32, void*, u32);
hdPartsLayout_c* hdHeart_constructLayout(hdPartsLayout_c*);

/* 026F6BC8 */
void hdHeartPart_c::destroy(hdHeartPart_c* part, u32 flags) {
    if (!part)
        return;
    part->mStateHolder.mHolderVtable = &sPartVtable;
    part->mSecondaryVtable = &sInterfaceVtable;
    hdHeart_baseDestroy(part, 0);
    if (flags & 1)
        operator_delete(part);
}
/* 026F6C34 */
void hdHeartPart_c::waitMove() {
    if (mRequestedState == 1)
        hdHeart_changeState(&mStateHolder.mMachine, &sToMaxState);
    else if (mRequestedState == 2)
        hdHeart_changeState(&mStateHolder.mMachine, &sToNormalState);
}
/* 026F6C6C */
void hdHeartPart_c::waitLeave() {
    mRequestedState = 0;
}
/* 026F6C78 */
void hdHeartPart_c::toMaxEnter() {
    void* animation = mpLayout->animSet();
    if (mSkipTransition)
        hdHeart_stop(animation, 0, 0);
    else
        hdHeart_play(animation, 0, 0, 1.0f);
    if (mLowLifeMode)
        hdHeart_pause(mpLayout->animSet(), 1, 1);
    else {
        f32 speed = sSave->mLife <= 4 ? 2.0f : 1.0f;
        hdHeart_play(mpLayout->animSet(), 1, 1, speed);
    }
}
/* 026F6D6C */
void hdHeartPart_c::toMaxMove() {
    if (mSkipTransition || hdHeart_isFinished(mpLayout->animSet(), 0))
        hdHeart_changeState(&mStateHolder.mMachine, &sWaitState);
}
/* 026F6DC8 */
void hdHeartPart_c::toMaxLeave() {
    mSkipTransition = 0;
}
/* 026F6DD4 */
void hdHeartPart_c::toNormalEnter() {
    void* animation = mpLayout->animSet();
    if (mSkipTransition)
        hdHeart_pause(animation, 0, 0);
    else
        hdHeart_play(animation, 0, 0, -1.0f);
    hdHeart_pause(mpLayout->animSet(), 1, 1);
}
/* 026F6E54 */
void hdHeartPart_c::toNormalMove() {
    if (mSkipTransition || hdHeart_isFinished(mpLayout->animSet(), 0))
        hdHeart_changeState(&mStateHolder.mMachine, &sWaitState);
}
/* 026F6EB0 */
void hdHeartPart_c::toNormalLeave() {
    mSkipTransition = 0;
}
/* 026F6EBC */
static hdHeartPart_c* hdHeartPart_c__ct(hdHeartPart_c* i_this) {
    if (!i_this) {
        i_this = static_cast<hdHeartPart_c*>(operator_new(sizeof(hdHeartPart_c)));
        if (!i_this)
            return NULL;
    }
    hdHeart_baseConstruct(i_this);
    i_this->mpPane = NULL;
    i_this->mReserved = 0;
    i_this->mSkipTransition = 0;
    i_this->mStateHolder.mHolderVtable = &sPartVtable;
    i_this->mVtable.v = gabi::ea(&sHeartVtable);
    i_this->mSecondaryVtable = &sInterfaceVtable;
    i_this->mValue = 5;
    i_this->mLowLifeMode = 0;
    i_this->mRequestedState = 0;
    return i_this;
}
/* 026F6F48 */
BOOL hdHeartPart_c::load(void* name, void* archive, void* heap, void* parent) {
    if (!gabi::call_ptr<BOOL>(mVtable->mLoad.get(), this, name, 0, archive, heap, parent))
        return FALSE;
    mpPane = reinterpret_cast<hdHeartPane_l*>(mpLayout->lyt()->mpRootPane.get());
    hdStateOwner* owner = mStateHolder.mMachine.mpOwner;
    hdStateObject* action =
        gabi::call_ptr<hdStateObject*>(owner->mpVTable->mCreateState.get(), owner, &sWaitState);
    mStateHolder.mMachine.mpCurrentState = action;
    gabi::call_ptr<void>(action->mpVTable->mEnter.get(), action);
    return TRUE;
}
/* 026F6FF4 */
void hdHeartPart_c::update() {
    gabi::call_ptr<void>(mVtable->mDestroyLayout.get(), this);
}
/* 026F7004 */
void hdHeartPart_c::tick() {
    u8 low = mLowLifeMode;
    u16 mode = sHud.mMode;
    if (low) {
        if (mode != 6)
            mLowLifeMode = 0;
    } else if (mode == 6) {
        mLowLifeMode = 1;
        hdHeart_setFrame(mpLayout->animSet(), 1, 1, 0.0f);
    }
    hdHeart_updateState(&mStateHolder.mMachine);
}
static const u32 sAnimationModes[6] = {0, 1, 2, 2, 2, 2};
/* 026F7090 */
void hdHeartPart_c::setValue(u32 value) {
    if (mValue == value)
        return;
    mValue = value;
    f32 frame = value == 4 ? 0.0f : 1.0f;
    u32 animations[5] = {5, 4, 3, 2, 2};
    u32 animation = animations[value];
    hdHeart_setFrame(mpLayout->animSet(), animation, sAnimationModes[animation], frame);
}
/* 026F711C */
void hdHeartPart_c::setVisible(u32 visible) {
    mpPane->mFlags = (u8(mpPane->mFlags) & ~1u) | visible;
}
/* 026F7134 */
BOOL hdHeartPart_c::loadLayout(void* name, void* archive, void* heap, void* parent, void* argument) {
    hdPartsLayout_c* layout =
        static_cast<hdPartsLayout_c*>(hdHeart_allocate(sizeof(hdPartsLayout_c), sHeapManager->mHeap, 4));
    if (layout)
        layout = hdHeart_constructLayout(layout);
    mpLayout = layout;
    if (!layout)
        return FALSE;
    gabi::call_ptr<void>(layout->vtbl()->mLoad.get(), layout, name, archive, heap, parent, 6, 3, argument);
    return TRUE;
}
static hdHeartInitLists_l sInitLists;
static hdHeartInitAngles_l sInitAngles;
static hdHeartAngleLimits_l sAngleLimits;
static hdHeartInitRecord_l sFirstInitRecord;
static hdHeartInitRecord_l sSecondInitRecord;
static hdHeartInitRecord_l sThirdInitRecord;
static hdUiSafeString sClassName;
static u32 sStringVtable;
static hdPartsStateIdVtbl_c sStateVtable;
static hdStateId sStateContext;
static u32 sStateGuard;
static u32 sStateCounter;
void hdHeart_registerStatic(hdHeartInitRecord_l*);
void hdHeart_initFlag(be<u8>*);
void hdHeart_initByte(be<u8>*);
static void initHeartState(hdPartsStateId_c& state, u32 id, const char* name, s16 firstSlot) {
    state.mNumber = id;
    state.mName.v = gabi::guest_addr(name);
    state.mVtable = &sStateVtable;
    state.mEnter.mThisDelta = 0;
    state.mEnter.mVirtualIndex = firstSlot;
    state.mEnter.mFunction = 4;
    state.mExecute.mThisDelta = 0;
    state.mExecute.mVirtualIndex = firstSlot + 1;
    state.mExecute.mFunction = 4;
    state.mExit.mThisDelta = 0;
    state.mExit.mVirtualIndex = firstSlot + 2;
    state.mExit.mFunction = 4;
    state.mUnused.mThisDelta = 0;
    state.mUnused.mVirtualIndex = 0;
    state.mUnused.mFunction = 0;
    state.mpNullStateId = &sStateContext;
}
/* 026F71E8 */
static void hdHeart_staticInit() {
    sInitLists.mFourth = 0;
    sInitLists.mThird = 0;
    sInitLists.mSecond = 0;
    sInitLists.mFirst = 0;
    hdHeart_registerStatic(&sFirstInitRecord);
    f32 minimum = sAngleLimits.mMinimum;
    f32 maximum = sAngleLimits.mMaximum;
    sInitAngles.mMinimum = minimum;
    sInitAngles.mMaximum = maximum;
    hdHeart_initFlag(&sInitAngles.mFlag);
    hdHeart_registerStatic(&sSecondInitRecord);
    hdHeart_initByte(&sInitAngles.mByte);
    hdHeart_registerStatic(&sThirdInitRecord);
    sClassName.mVtable.v = gabi::ea(&sStringVtable);
    sClassName.mText.v = gabi::guest_addr("Heart");
    u32 id;
    if (sStateGuard)
        id = sStateCounter;
    else {
        sStateGuard = 1;
        id = 0;
    }
    initHeartState(sWaitState, ++id, "::StateID_Wait", 25);
    initHeartState(sToMaxState, ++id, "::StateID_ToMax", 28);
    initHeartState(sToNormalState, ++id, "::StateID_ToNormal", 31);
    sStateCounter = id;
}
/* WWHD glue
 data sInitLists 1049DAA4
 data sInitAngles 1049DA98
 data sAngleLimits 10109D98
 data sFirstInitRecord 101F7178
 data sSecondInitRecord 101F7184
 data sThirdInitRecord 101F7190
 data sClassName 1049DB44
 data sStringVtable 10109CC4
 data sStateVtable 10109D3C
 data sStateContext 101FF32C
 data sStateGuard 101FD8E4
 data sStateCounter 101FDD50
 str 10109DA0 "Heart"
 str 10109DB8 "::StateID_Wait"
 str 10109DA8 "::StateID_ToMax"
 str 10109DC8 "::StateID_ToNormal"
 ext 028F026C void hdHeart_registerStatic(hdHeartInitRecord_l*)
 ext 028ED6F8 void hdHeart_initFlag(be<u8>*)
 ext 028EAB2C void hdHeart_initByte(be<u8>*)
 data sPartVtable 10109EF0
 data sInterfaceVtable 10109F00
 data sHeartVtable 10109DE0
 data sWaitState 1049DAB4
 data sToMaxState 1049DAE4
 data sToNormalState 1049DB14
 data sSave 101F84DC
 data sHud 1047B03C
 data sHeapManager 1018C404
 data sAnimationModes 10109CDC
 ext 026F88F0 void hdHeart_baseDestroy(hdHeartPart_c*,u32)
 ext 026F89E8 void hdHeart_baseConstruct(hdHeartPart_c*)
 ext 020063C0 void hdHeart_changeState(hdStateMachine*,hdPartsStateId_c*)
 ext 02006364 void hdHeart_updateState(hdStateMachine*)
 ext 020053E4 void hdHeart_play(void*,u32,u32,f32)
 ext 02005708 void hdHeart_stop(void*,u32,u32)
 ext 02005674 void hdHeart_pause(void*,u32,u32)
 ext 02005840 BOOL hdHeart_isFinished(void*,u32)
 ext 0200552C void hdHeart_setFrame(void*,u32,u32,f32)
 ext 0273B050 void* hdHeart_allocate(u32,void*,u32)
 ext 026FADB0 hdPartsLayout_c* hdHeart_constructLayout(hdPartsLayout_c*)
*/

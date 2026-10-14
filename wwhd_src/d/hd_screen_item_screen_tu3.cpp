/* HD-only item-screen Decide part.
 * StateID_Decide is an HD name; the remaining names describe the observed operations. */
#include "d/hd_screen_item_screen_tu3.h"

static hdPartsVtbl_c sDecideVtable;
static u32 sDecideHolderVtable;
static u32 sDecideSecondaryVtable;
static hdPartsStateIdVtbl_c sDecideStateVtable;
static hdPartsStateId_c sMenuDecideBaseState;
static hdPartsStateId_c sDecideState;
static hdPartsStateId_c sDecideMenuOffState;
static u32 sStateCounterReady;
static u32 sStateCounter;
static void* sEventManager;
void hdItemPane_getPosition(hdItemPosition3_c* out, hdPartsPaneSRT_c* pane);
Pair32 hdItemPane_getScale(hdPartsPaneSRT_c* pane);

/* 02635D04 */
static s32 hdItemDecide_stateNumber(hdPartsStateId_c* state) { return state->mNumber; }

/* 02635E04 */
void hdItemDecide_c::destroy(hdItemDecide_c* self, s32 flags) {
    if (self == NULL) return;
    self->mButton.mStateHolder.mHolderVtable = &sDecideHolderVtable;
    self->mButton.mSecondaryVtable = &sDecideSecondaryVtable;
    hdMenuItem_c::destroy(self, 0);
    if (flags & 1) operator_delete(self);
}

/* 02635E70 */
void hdItemDecide_c::decideEnter() {
    mButton.mpReceiver->mEnabled = 0;
    mButton.synchronizeFocus();
    if (mMode == HD_ITEM_MODE_TOUCH) {
        hdItemAnim_operationD4(mButton.animSet(), HD_ITEM_ANIM_FLOAT_CANCEL, HD_ITEM_SLOT_FLOAT);
        hdAnimSet_play(mButton.animSet(), HD_ITEM_ANIM_COMMAND, HD_ITEM_SLOT_TOUCH_DECIDE, 1.0f);
    } else {
        hdAnimSet_play(mButton.animSet(), 2, HD_ITEM_SLOT_BUTTON, 1.0f);
    }
    mPhase = HD_ITEM_COUNT_HIDDEN;
    hdAnimSet_setFrame(mButton.animSet(), HD_ITEM_ANIM_COUNT, HD_ITEM_SLOT_COUNT, 2.0f);
}

/* 02635F44 */
void hdItemDecide_c::decideExecute() {
    if (sEventManager != NULL) {
        mButton.synchronizeFocus();
        if (hdAnimSet_isEnd(mButton.animSet(), HD_ITEM_ANIM_COMMAND) && hdAnimSet_isEnd(mButton.animSet(), 2)) {
            mButton.changeState(&sDecideMenuOffState);
        }
    }
}

/* 02635FC4 */
void hdItemDecide_c::decideExit() { mButton.mpReceiver->mEnabled = 1; }

/* 02635FD4 */
hdItemDecide_c* hdItemDecide_c::construct(hdItemDecide_c* self) {
    if (self == NULL) self = (hdItemDecide_c*)operator_new(sizeof(hdItemDecide_c));
    if (self != NULL) {
        hdMenuItem_c::construct(self);
        self->mButton.mVtable = &sDecideVtable;
        self->mButton.mStateHolder.mHolderVtable = &sDecideHolderVtable;
        self->mButton.mSecondaryVtable = &sDecideSecondaryVtable;
    }
    return self;
}

/* 02636040 */
u32 hdItemDecide_c::load(void* resource, u32 arg1, u32 arg2) {
    if (!hdMenuItem_c::load(resource, arg1, arg2)) return 0;
    preparePane();
    mButton.mTouchEnabled = 0;
    return 1;
}

/* 026360A0 */
void hdItemDecide_c::execute() {
    updateItem();
    updateAnimation();
    executeButton();
}

/* 026360DC */
void hdItemDecide_c::update() { if (mVisible != 0) updateVisible(); }

/* 026360EC */
void hdItemDecide_c::beginAnimation() {
    hdItemAnim_operationD4(mButton.animSet(), HD_ITEM_ANIM_SWITCH, HD_ITEM_SLOT_TOUCH_DECIDE);
    mButton.setPaneEnabled(1);
}

/* 02636134 */
void hdItemDecide_c::endAnimation() {
    hdAnimSet_operationCC(mButton.animSet(), HD_ITEM_ANIM_SWITCH, HD_ITEM_SLOT_TOUCH_DECIDE);
    mButton.setPaneEnabled(1);
}

/* 0263617C */
void hdItemDecide_c::getContext(hdItemDecideContext_c* context) {
    context->mpAnimation = mButton.animSet();
    context->mReserved = 0;
    context->mpPaneGroups[0] = &mPaneGroups[0];
    context->mpPaneGroups[1] = &mPaneGroups[1];
    context->mpPaneGroups[2] = &mPaneGroups[2];
    context->mOption = 0;
}

/* 026361B0 */
void hdItemDecide_c::setVisible(u32 visible) {
    if (mVisible == visible) return;
    mVisible = visible;
    hdPartsPaneSRT_c* pane = mPaneGroups[0].mpData.get()[0];
    if (pane != NULL) pane->mFlags = (pane->mFlags & ~u32(HD_ITEM_PANE_VISIBLE)) | visible;
}

/* 026361E8 */
void hdItemDecide_c::cachePosition() {
    hdPartsPaneSRT_c* pane = rootPane();
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::FrameLocal<hdItemPosition3_c> position(8);
    hdItemPane_getPosition(position, pane);
    u32 x = position->xBits, y = position->yBits;
    mPanePosition.xBits = x;
    mPanePosition.yBits = y;
    u32 translateX = pane->mTranslationXBits, translateY = pane->mTranslationYBits;
    mPaneTranslation.xBits = translateX;
    mPaneTranslation.yBits = translateY;
}

/* 02636250 */
void hdItemDecide_c::translatePoint(hdItemPoint_c* point) {
    hdPartsPaneSRT_c* pane = rootPane();
    Pair32 scale{hdItemPane_getScale(pane)};
    union FloatBits { u32 mBits; f32 mValue; };
    FloatBits scaleX{scale.r3}, scaleY{scale.r4};
    f32 x = (mPaneTranslation.x + (point->x - mPanePosition.x)) / scaleX.mValue;
    f32 y = (mPaneTranslation.y + (point->y - mPanePosition.y)) / scaleY.mValue;
    u32 z = pane->mTranslationZBits;
    hdItemPaneSetTranslateX(pane, x);
    hdItemPaneSetTranslateY(pane, y);
    pane->mTranslationZBits = z;
    pane->mFlags = pane->mFlags | 0x10;
}

static hdItemHeaderStatics_c sDecideHeaderStatics;
static hdItemHeaderDescriptors_c sDecideHeaderDescriptors;
static hdItemHeaderZero_c sDecideHeaderZero;
static hdItemPoint_c sDecidePiConstants;
/* 02636354 */
static void __sinit_hd_screen_item_screen_tu3_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sDecideHeaderStatics), gabi::ea(&sDecideHeaderDescriptors), gabi::ea(&sDecideHeaderZero),
                                gabi::ea(&sDecidePiConstants));
    u32 number;
    if (sStateCounterReady) number = sStateCounter;
    else { number = 0; sStateCounterReady = 1; }
    sStateCounter = number + 1;
    sDecideState.setup(number + 1, "::StateID_Decide", &sDecideStateVtable, 0x38, (hdStateId*)&sMenuDecideBaseState);
}

/* WWHD glue
data sDecideHeaderStatics 10490148
data sDecideHeaderDescriptors 101F5530
data sDecideHeaderZero 10490154
data sDecidePiConstants 100E8D08
data sDecideVtable 100E8D28
data sDecideHolderVtable 100E9088
data sDecideSecondaryVtable 100E9098
data sDecideStateVtable 100E8CB4
data sMenuDecideBaseState 1048FCCC
data sDecideState 10490164
data sDecideMenuOffState 1048FBAC
data sStateCounterReady 101FD8E4
data sStateCounter 101FDD50
data sEventManager 101F4FF0
str 100E8D10 "::StateID_Decide" in 02636354

ext 02005708 void hdItemAnim_operationD4(hdAnimSet*, u32, u32)
ext 02704D2C void hdItemPane_getPosition(hdItemPosition3_c*, hdPartsPaneSRT_c*)
ext 02704EA8 Pair32 hdItemPane_getScale(hdPartsPaneSRT_c*)
*/

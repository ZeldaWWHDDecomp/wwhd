/* HD event-message frame parts. Own game functions under development against WWHD;
 * library animation and parts implementations remain external bindings. */
#include "d/hd_msg_window_event_companion.h"

static u32 sFrameFirstAnimation;
static hdPartsVtbl_c sFrameVtable;
static u8 sFrameOwnerVtable;
static u8 sFrameHolderVtable;
static hdUiSafeString sFrameResource;
static hdPartsStateId_c sFrameLoopState;
static hdStateId sFrameInitialState;
static hdMsgEventFrameUiPointer_l sFrameUiSystem;

/* 026B54DC */
static u32 hdMsgEvent_stateValue(hdStateId* state) {
    return u32(state->mId);
}

/* 026B55DC */
void hdMsgEventFrame_c::destroy(hdMsgEventFrame_c* self, s32 flags) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (self) {
        self->mStateHolder.mHolderVtable = &sFrameOwnerVtable;
        self->mSecondaryVtable = &sFrameHolderVtable;
        hdParts_c::destroy(self, 0);
        if (flags & 1) operator_delete(self);
    }
}

/* 026B5648 */
void hdMsgEventFrame_c::playAnimation(u32 animation) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    hdAnimSet* animations = mpLayout->animSet();
    u32 first = sFrameFirstAnimation;
    hdAnimSet_play(animations, animation, first, 1.0f);
}

/* 026B5664 */
void hdMsgEventFrame_c::playDefaultAnimation() {
    gabi::AutoFrame automatic{gabi::Activation::current}; playAnimation(0); }

/* 026B566C */
hdMsgEventFrame_c* hdMsgEventFrame_c::construct(hdMsgEventFrame_c* self) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!self) {
        self = (hdMsgEventFrame_c*)operator_new(sizeof(hdMsgEventFrame_c));
        if (!self) return NULL;
    }
    hdParts_c::construct(self);
    self->mVtable = &sFrameVtable;
    self->mStateHolder.mHolderVtable = &sFrameOwnerVtable;
    self->mSecondaryVtable = &sFrameHolderVtable;
    return self;
}

/* 026B56D8 */
void hdMsgEventFrame_c::setAnimationFrame(u32 animation, u32 frame) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    f32 frameValue = f32(frame);
    hdAnimSet* animations = mpLayout->animSet();
    u32 first = sFrameFirstAnimation;
    hdAnimSet_setFrame(animations, animation, first, frameValue);
}

/* 026B5728 */
u32 hdMsgEventFrame_c::loadFrame(u32 resource, u32 kind, void* description) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!vLoad(&sFrameResource, 0, resource, kind, description)) return 0;
    setAnimationFrame(0, 0);
    hdStateObject* state = mStateHolder.mMachine.mpOwner->createState(&sFrameInitialState);
    mStateHolder.mMachine.mpCurrentState = state;
    state->enter();
    return 1;
}

/* 026B57DC */
void hdMsgEventFrame_c::destroyFrameLayout() {
    gabi::AutoFrame automatic{gabi::Activation::current}; vDestroyLayout(); }

/* 026B57EC */
void hdMsgEventFrame_c::executeFrame() {
    gabi::AutoFrame automatic{gabi::Activation::current}; hdParts_executeStateMachine(stateMachine()); }

/* 026B57F4 */
void hdMsgEventFrame_c::enterLoop() {
    gabi::AutoFrame automatic{gabi::Activation::current}; hdParts_changeState(stateMachine(), &sFrameLoopState); }

/* 026B5804 */
u32 hdMsgEventFrame_c::createFrameLayout(void* name, u32 kind, u32 option, u32 flags, void* description) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), sFrameUiSystem.mpSystem->mHeap, 4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout) return 0;
    layout->vLoad(name, kind, option, flags, 1, 1, description);
    return 1;
}

static hdMsgEventFrameHeader_l sFrameHeader;
static hdMsgEventFrameAngle_l sFrameMinimumAngle;
static hdMsgEventFrameAngle_l sFrameMaximumAngle;
static hdMsgEventFrameAngleLimits_l sFrameAngleLimits;
static u8 sFrameAngle;
static u8 sFramePhase;
static hdMsgEventFrameRegistration_l sFrameHeaderRegistration;
static hdMsgEventFrameRegistration_l sFrameAngleRegistration;
static hdMsgEventFrameRegistration_l sFramePhaseRegistration;
static u32 sFrameStateGuard;
static hdPartsStateCounter_c sFrameStateCounter;
static hdPartsStateIdVtbl_c sFrameStateVtable;
static hdUiStringTable sFrameStringVtable;
static hdStateId sFrameNullState;

/* 026B58B8 */
static void hdMsgEventFrame_initializeStatics() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    for (s32 i = 3; i >= 0; --i) sFrameHeader.mWords[i] = 0;
    hdMsgEventFrame_register(&sFrameHeaderRegistration);
    // Both source loads precede the stores; typed loads retain the runtime NaN conversion.
    f32 minimum = sFrameAngleLimits.mMinimum.mValue;
    f32 maximum = sFrameAngleLimits.mMaximum.mValue;
    sFrameMinimumAngle.mValue = minimum;
    sFrameMaximumAngle.mValue = maximum;
    hdMsgEventFrame_constructAngle(&sFrameAngle);
    hdMsgEventFrame_register(&sFrameAngleRegistration);
    hdMsgEventFrame_constructPhase(&sFramePhase);
    hdMsgEventFrame_register(&sFramePhaseRegistration);
    u32 initialized = sFrameStateGuard;
    sFrameResource.mVtable = &sFrameStringVtable;
    sFrameResource.mText.v = gabi::guest_addr("MsgWindowEventFrame_00");
    s32 number;
    if (initialized) {
        number = u32(sFrameStateCounter.mValue) + 1;
    } else {
        sFrameStateGuard = 1;
        number = 1;
    }
    sFrameLoopState.setup(number, "::StateID_Loop", &sFrameStateVtable, 0x19, &sFrameNullState);
    sFrameStateCounter.mValue = number;
}

/* WWHD glue
 ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c*)
 ext 026F88F0 void hdParts_c::destroy(hdParts_c*, s32)
 data sFrameFirstAnimation 100FE45C
 data sFrameVtable 100FE548
 data sFrameOwnerVtable 100FE628
 data sFrameHolderVtable 100FE638
 data sFrameResource 10498690
 data sFrameLoopState 10498660
 data sFrameInitialState 1049DD60
 data sFrameUiSystem 1018C404
 data sFrameHeader 10498650
 data sFrameMinimumAngle 10498644
 data sFrameMaximumAngle 10498648
 data sFrameAngleLimits 100FE518
 data sFrameAngle 1049864C
 data sFramePhase 1049864D
 data sFrameHeaderRegistration 101F65F4
 data sFrameAngleRegistration 101F6600
 data sFramePhaseRegistration 101F660C
 data sFrameStateGuard 101FD8E4
 data sFrameStateCounter 101FDD50
 data sFrameStateVtable 100FE4C0
 data sFrameStringVtable 100FE460
 data sFrameNullState 101FF32C
 str 100FE520 "MsgWindowEventFrame_00"
 str 100FE538 "::StateID_Loop"
 ext 028F026C void hdMsgEventFrame_register(hdMsgEventFrameRegistration_l*)
 ext 028ED6F8 void hdMsgEventFrame_constructAngle(be<u8>*)
 ext 028EAB2C void hdMsgEventFrame_constructPhase(be<u8>*)
*/

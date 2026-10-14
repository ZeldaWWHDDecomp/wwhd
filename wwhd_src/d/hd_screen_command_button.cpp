/** HD button-specific command guide; private binary-derived source. */
#include "d/hd_screen_command_extra.h"
#include "d/hd_screen_map_display_coordinator.h"
#include "d/d_com_inf_game.h"
#include "d/hd_input_ctrl.h"

static hdPartsStateId_c sButtonGuideHiddenState;
static hdPartsStateId_c sButtonGuideDisplayState;

static hdCommandGuideVtbl_c sButtonGuideVtable;
static u32 sButtonGuideHolderVtable;
static u32 sButtonGuideSecondaryVtable;
struct hdCommandGameInfo_l {
    u8 _padding00[0x5C28];
    be<u8> mACommand;
    be<u8> mBCommand;
    be<u8> mRCommand;
};
WWHD_OFFSET(hdCommandGameInfo_l, mACommand, 0x5C28);
WWHD_OFFSET(hdCommandGameInfo_l, mBCommand, 0x5C29);
WWHD_OFFSET(hdCommandGameInfo_l, mRCommand, 0x5C2A);
static hdCommandGameInfo_l* commandGameInfo() {
    return (hdCommandGameInfo_l*)dComIfGp_get();
}

static hdPartsStateId_c sButtonGuideInState;
static hdPartsStateId_c sButtonGuideOutState;

struct hdCommandMapOwner_l {
    u8 _padding00[0x17C];
    gptr<void> mTransition;
    u8 _padding180[0x98];
    gptr<hdMapDisplayCoordinator_c> mCoordinator;
    u8 _padding21C[8];
    be<u32> mMode;
};
WWHD_OFFSET(hdCommandMapOwner_l, mTransition, 0x17C);
WWHD_OFFSET(hdCommandMapOwner_l, mCoordinator, 0x218);
WWHD_OFFSET(hdCommandMapOwner_l, mMode, 0x224);
static u8 sButtonGuideOverride;
static u8 sButtonGuideOverrideCommand;
s32 dMenu_flag(); // d_meter
u32 commandMapTransitionReady(void*);

/* 0264B838 */
u32 hdCommandButtonGuide_c::getCurrentCommand() {
    if (sButtonGuideOverride != 0) return sButtonGuideOverrideCommand;
    HdPadUiOwner* mapOwner = l_hdMapUi.mOwner.get();
    if (mapOwner == NULL) return 0;
    hdCommandMapOwner_l* owner = hdPartsView<hdCommandMapOwner_l>(gabi::ea(mapOwner));
    void* transition = owner->mTransition.get();
    if (dMenu_flag() == 0) return 0;
    u32 ready = commandMapTransitionReady(transition);
    u32 mode = owner->mMode;
    u32 command = ready == 0 ? 3 : 0;
    if (mode == 1 && owner->mCoordinator->mMode == 0) {
        if (dComIfGs_info()->mSavedata.mEvent.isEventBit(0x901) != 0) command |= 4;
    }
    return command;
}

/* 0264C054 */
u32 hdCommandButtonGuide_c::getVisibleButtons() {
    u32 command = mCachedCommand;
    hdCommandMapOwner_l* owner = hdPartsView<hdCommandMapOwner_l>(gabi::ea(l_hdMapUi.mOwner.get()));
    u32 buttons = command & 3;
    if (command & 4) {
        if (owner->mMode == 1) {
            u32 event = dComIfGs_info()->mSavedata.mEvent.isEventBit(0x901);
            command = mCachedCommand;
            if (event == 0) buttons |= 0x1C;
            else buttons |= 4;
        } else buttons |= 0x1C;
    }
    if (command & 8) buttons |= 0x20;
    return buttons;
}


/* 0264BB2C */
void hdCommandButtonGuide_c::enterOut() {
    if (hdInputGetManager()->mMode == 0) {
        setAnimationFrame(mInAnimation, 0);
        vCalcLayout();
    } else playAnimation(mOutAnimation);
}

/* 0264BCB8 */
u32 hdCommandButtonGuide_c::initializeButtonGuide(u32 first, u32 second, hdPartsDesc_c* descriptor) {
    attachGuideArchives();
    if (vLoad(&sCommandGuideLayoutName, gabi::ea(mpResAccessor.get()), first, second, descriptor) == 0) return 0;
    if (gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this) == 0) return 0;
    bindTextPanes();
    stopAnimation(1);
    refreshMessages();
    hideMessageGroup();
    vCalcLayout();
    startState(&sButtonGuideHiddenState);
    return 1;
}

/* 0264BEAC */
void hdCommandButtonGuide_c::updateButtonGuideMessages() {
    updateCommandButton(0);
    updateCommandButton(1);
    updateItemButton(2);
    updateItemButton(3);
    updateItemButton(4);
}

/* 0264B900 */
void hdCommandButtonGuide_c::executeHidden() {
    u32 command = getCurrentCommand();
    if (command != 0) {
        mCachedCommand = command;
        mCachedButtons = getButtons();
        changeState(&sButtonGuideInState);
    }
    vExecuteChildren();
    vCalcLayout();
}

/* 0264B988 */
void hdCommandButtonGuide_c::enterIn() {
    mInAnimation = gabi::call_ptr<u32>(guideVtable()->mGetInAnimation.get(), this);
    mOutAnimation = gabi::call_ptr<u32>(guideVtable()->mGetOutAnimation.get(), this);
    resetButtons();
    refreshMessages();
    playAnimation(mInAnimation);
}

/* 0264BA7C */
void hdCommandButtonGuide_c::executeDisplay() {
    u32 command = getCurrentCommand();
    if (u32(mCachedCommand) != command) changeState(&sButtonGuideOutState);
    else refreshMessages();
    vExecuteChildren();
    vCalcLayout();
}

/* 0264B7CC */
void hdCommandButtonGuide_c::destroy(hdCommandButtonGuide_c* self, u32 flags) {
    if (self == NULL) return;
    self->mStateHolder.mHolderVtable = &sButtonGuideHolderVtable;
    self->mSecondaryVtable = &sButtonGuideSecondaryVtable;
    hdCommandGuide_c::destroy(self, 0);
    if (flags & 1) operator_delete(self);
}

/* 0264BC1C */
hdCommandButtonGuide_c* hdCommandButtonGuide_c::construct(hdCommandButtonGuide_c* self) {
    if (self == NULL) {
        self = (hdCommandButtonGuide_c*)operator_new(sizeof(hdCommandButtonGuide_c));
        if (self == NULL) return NULL;
    }
    hdCommandGuide_c::construct(self);
    self->mVtable = &sButtonGuideVtable;
    self->mStateHolder.mHolderVtable = &sButtonGuideHolderVtable;
    self->mSecondaryVtable = &sButtonGuideSecondaryVtable;
    self->mInputTimer = 0;
    commandGameInfo()->mACommand = 0x21;
    commandGameInfo()->mBCommand = 7;
    commandGameInfo()->mRCommand = 0;
    return self;
}

/* 0264B5BC */
hdParts_c* createCommandItemButton(void* context, u32 kind, hdPartsDesc_c* descriptor) {
    hdItemDecide_c* child = (hdItemDecide_c*)hdParts_new(sizeof(hdItemDecide_c), sCommandUiSystem->mHeap, 4);
    if (child == NULL) return NULL;
    child = hdItemDecide_c::construct(child);
    if (child == NULL) return NULL;
    child->load(context, kind, gabi::ea(descriptor));
    return &child->mButton;
}

/* 0264B66C */
hdParts_c* createCommandCollectButton(void* context, u32 kind, hdPartsDesc_c* descriptor) {
    hdBtnCollectIcon_c* child = (hdBtnCollectIcon_c*)hdParts_new(sizeof(hdBtnCollectIcon_c), sCommandUiSystem->mHeap, 4);
    if (child == NULL) return NULL;
    child = collectIconConstruct(child);
    if (child == NULL) return NULL;
    child->init(gabi::ea(context), kind, descriptor);
    return child;
}

/* 0264B71C */
hdParts_c* createCommandAPrompt(void* context, u32 kind, hdPartsDesc_c* descriptor) {
    hdCommandPromptA_c* child = (hdCommandPromptA_c*)hdParts_new(sizeof(hdCommandPromptA_c),
                                                              sCommandUiSystem->mHeap, 4);
    if (child == NULL) return NULL;
    child = hdCommandPromptA_c::construct(child);
    if (child == NULL) return NULL;
    child->initialize(gabi::ea(context), kind, descriptor);
    return child;
}

/* 0264B4BC */
static u32 getButtonGuideStateNumber(hdPartsStateId_c* state) {
    return state->mNumber;
}

/* 0264BA04 */
void hdCommandButtonGuide_c::executeIn() {
    if (hdAnimSet_isEnd(animSet(), mInAnimation) != 0) {
        changeState(&sButtonGuideDisplayState);
    }
    vExecuteChildren();
    vCalcLayout();
}

/* 0264BBA4 */
void hdCommandButtonGuide_c::executeOut() {
    if (hdAnimSet_isEnd(animSet(), mOutAnimation) != 0) {
        changeState(&sButtonGuideHiddenState);
    }
    vExecuteChildren();
    vCalcLayout();
}

/* 0264BDF4 */
void hdCommandButtonGuide_c::finalizeGuide() {
    vDeleteChildren();
    vDestroyLayout();
    detachArchives();
}

/* 0264BE48 */
void hdCommandButtonGuide_c::executeState() {
    hdParts_executeStateMachine(stateMachine());
}

/* 0264BE50 */
void hdCommandButtonGuide_c::drawGuide(void* info) {
    vDrawLayout(info);
    vDrawChildren(info);
}


/* 0264BF0C */
u32 hdCommandButtonGuide_c::getEnabledButtons() {
    HdPadUiOwner* owner = l_hdMapUi.mOwner;
    u32 buttons = 0xFF;
    if (owner != NULL) {
        hdMapDisplayCoordinator_c* map = (hdMapDisplayCoordinator_c*)owner->mUi.get();
        if (map->canSaveSelectedCell() == 0) buttons = 0xFB;
    }
    return buttons;
}

/* 0264BF64 */
u32 hdCommandButtonGuide_c::getButtonCommand(u32 button) {
    HdPadUiOwner* owner = l_hdMapUi.mOwner;
    mButtonCounters[button < 6 ? button : 0] = 0;
    u32 command = 0xFFFFFFFFu;
    if (button == 2) {
        if (owner != NULL) {
            hdMapDisplayCoordinator_c* map = (hdMapDisplayCoordinator_c*)owner->mUi.get();
            if (map->canSaveSelectedCell() != 0) command = 0x47;
        }
    } else if (button == 0) {
        command = commandGameInfo()->mACommand;
        if (command - 1 >= 0x4C) command = 0xFFFFFFFFu;
    } else if (button == 1) {
        command = commandGameInfo()->mBCommand;
        if (command - 1 >= 0x4C) command = 0xFFFFFFFFu;
    } else if (button == 5) {
        command = commandGameInfo()->mRCommand;
        if (command - 1 >= 0x4C) command = 0xFFFFFFFFu;
    }
    return command;
}

/* 0264BF5C */
u32 hdCommandButtonGuide_c::getGuideKind() {
    return 2;
}

/* 0264C04C */
u32 hdCommandButtonGuide_c::getPrimaryCommand() {
    return hdCommandGuide_c::getPrimaryCommand();
}

/* 0264C050 */
u32 hdCommandButtonGuide_c::getSecondaryCommand() {
    return hdCommandGuide_c::getSecondaryCommand();
}

struct hdCommandButtonStatics_l {
    be<f32> mPiPair[2];
    be<f32> mBounds[4];
    be<u8> mAngle;
    be<u8> mShortVector;
    u8 _padding1A[2];
    be<u32> mZero[4];
};
struct hdCommandButtonRegistration_l { be<u32> mWords[3]; };
typedef hdCommandButtonRegistration_l hdButtonRegistrations_t[3];
static hdCommandButtonStatics_l sButtonGuideStatics;
static hdButtonRegistrations_t sButtonGuideRegistrations;
static f32 sButtonGuidePiLower;
static f32 sButtonGuidePiUpper;
static f32 sButtonGuideBoundLower;
static f32 sButtonGuideBoundUpper;
static hdPartsStateIdVtbl_c sButtonGuideStateVtable;

/* 0264C10C */
static void initializeCommandButtonStatics() {
    sButtonGuideStatics.mZero[3] = 0;
    sButtonGuideStatics.mZero[2] = 0;
    sButtonGuideStatics.mZero[1] = 0;
    sButtonGuideStatics.mZero[0] = 0;
    __register_global_object(gabi::ea(&sButtonGuideRegistrations[0]));
    f32 lower = sButtonGuidePiLower;
    f32 upper = sButtonGuidePiUpper;
    sButtonGuideStatics.mPiPair[0] = lower;
    sButtonGuideStatics.mPiPair[1] = upper;
    hdCommandInitializeAngle(&sButtonGuideStatics.mAngle);
    __register_global_object(gabi::ea(&sButtonGuideRegistrations[1]));
    hdCommandInitializeShortVector(&sButtonGuideStatics.mShortVector);
    __register_global_object(gabi::ea(&sButtonGuideRegistrations[2]));
    lower = sButtonGuideBoundLower;
    upper = sButtonGuideBoundUpper;
    sButtonGuideStatics.mBounds[0] = lower;
    sButtonGuideStatics.mBounds[3] = upper;
    sButtonGuideStatics.mBounds[1] = lower;
    sButtonGuideStatics.mBounds[2] = upper;
    u32 number;
    if (sCommandStateCounterReady != 0) number = sCommandStateCounter;
    else {
        sCommandStateCounterReady = 1;
        number = 0;
    }
    sButtonGuideHiddenState.setup(s32(++number), "::StateID_NoButton", &sButtonGuideStateVtable, 0x19, &sCommandNullState);
    sButtonGuideInState.setup(s32(++number), "::StateID_In", &sButtonGuideStateVtable, 0x1C, &sCommandNullState);
    sButtonGuideDisplayState.setup(s32(++number), "::StateID_Wait", &sButtonGuideStateVtable, 0x1F, &sCommandNullState);
    sButtonGuideOutState.setup(s32(++number), "::StateID_Out", &sButtonGuideStateVtable, 0x22, &sCommandNullState);
    sCommandStateCounter = number;
}

/* WWHD glue
 ext 020063C0 void hdPadChangeState(HdPadStateController*, HdPadState*)
 ext 0267BF30 f32 hdPadZoomRatio(HdPadMap*, u32, f32, f32)
 ext 0267C068 void hdPadZoomPosition(HdPadMap*, HdPadPoint*, HdPadPoint*, u32)
 ext 0267C188 void hdPadZoomScale(HdPadMap*, u32, f32, f32, f32)
 ext 02655CE8 u32 hdPadMapMode(void*)
 ext 02689BA0 f32 hdMapDisplayChase(HdPadPoint*, HdPadPoint*, f32, f32, f32)
 data sButtonGuideStatics 10491708
 data sButtonGuideRegistrations 101F58AC
 data sButtonGuidePiLower 100EE5A4
 data sButtonGuidePiUpper 100EE5A8
 data sButtonGuideBoundLower 100EE5AC
 data sButtonGuideBoundUpper 100EE5B0
 data sButtonGuideStateVtable 100EE554
 str 100EE5C4 "::StateID_NoButton" in 0264C10C
 str 100EE5D8 "::StateID_In" in 0264C10C
 str 100EE5E8 "::StateID_Wait" in 0264C10C
 str 100EE5B4 "::StateID_Out" in 0264C10C
 data sButtonGuideOverride 1047B096
 data sButtonGuideOverrideCommand 1047B07C
 ext 025986BC s32 dMenu_flag()
 ext 026946DC u32 commandMapTransitionReady(void*)
 data sButtonGuideInState 10491764
 data sButtonGuideOutState 104917C4
 ext 02635FD4 hdItemDecide_c* hdItemDecide_c::construct(hdItemDecide_c*)
 ext 02636040 u32 hdItemDecide_c::load(void*, u32, u32)
 ext 0262A088 hdBtnCollectIcon_c* collectIconConstruct(hdBtnCollectIcon_c*)
 ext 0262A130 u32 hdBtnCollectIcon_c::init(u32, u32, hdPartsDesc_c*)
 ext 026891F8 u32 hdMapDisplayCoordinator_c::canSaveSelectedCell()
 data sButtonGuideVtable 100EE5F8
 data sButtonGuideHolderVtable 100EE758
 data sButtonGuideSecondaryVtable 100EE768
 data sButtonGuideHiddenState 10491734
 data sButtonGuideDisplayState 10491794
*/

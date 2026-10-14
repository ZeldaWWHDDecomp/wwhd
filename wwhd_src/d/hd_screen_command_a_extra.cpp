/** HD command-A prompt animation and state transitions; binary-derived private source. */
#include "d/hd_screen_command_extra.h"
#include "d/hd_screen_command_a.h"

static hdPartsVtbl_c sCommandAVtable;
static u32 sCommandAHolderVtable;
static u32 sCommandASecondaryVtable;
static f32 sCommandAStoppedFrame;
static f32 sCommandAAnimationRate;
static hdPartsStateId_c sCommandAHiddenState;
static hdPartsStateId_c sCommandADisplayState;

typedef hdUiSafeString sCommandAPaneNames_t[2];
static sCommandAPaneNames_t sCommandAPaneNames;
static hdUiSafeString sCommandATextPaneName;
static hdUiSafeString sCommandALayoutName;
static f32 sCommandATextMargin;
static f32 sCommandATextAnchor;

struct hdCommandPaneVtbl_c {
    u8 _padding00[0x5C];
    gfn mFindPane;
};
static hdCommandPane_c* findCommandPane(hdCommandPane_c* root, hdUiSafeString* name) {
    hdCommandPaneVtbl_c* table = hdPartsView<hdCommandPaneVtbl_c>(gabi::ea(root->mVtable.get()));
    name->assureTermination();
    u32 find = table->mFindPane.get();
    return gabi::call_ptr<hdCommandPane_c*>(find, root, name->mText.get(), 1);
}

static hdScreenUiSystem* sCommandUiSystem;
typedef hdUiSafeString sCommandAAnimationNames_t[4];
static sCommandAAnimationNames_t sCommandAAnimationNames;
typedef hdUiSafeString sCommandAAnimationGroups_t[2];
static sCommandAAnimationGroups_t sCommandAAnimationGroups;
typedef be<u32> sCommandAAnimationGroupIndices_t[4];
static sCommandAAnimationGroupIndices_t sCommandAAnimationGroupIndices;
void hdCommandBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);

/* 026483F8 */
u32 hdCommandPromptA_c::loadLayout(void* name, u32 first, u32 second, u32 third, void* descriptor) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c),
                                                          sCommandUiSystem->mHeap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL) {
        return 0;
    }
    if (layout->vLoad(name, first, second, third, 4, 2, descriptor) == 0) {
        return 0;
    }
    for (u32 i = 0; i < 4; ++i) {
        u32 group = sCommandAAnimationGroupIndices[i];
        hdUiSafeString* groupName = gabi::at<hdUiSafeString>(gabi::ea(sCommandAAnimationGroups) + group * 8);
        hdCommandBindAnimation(animSet(), i, &sCommandAAnimationNames[i], groupName);
    }
    return 1;
}

/* 02648034 */
void hdCommandPromptA_c::bindPanes() {
    hdCommandPane_c* root = hdPartsView<hdCommandPane_c>(gabi::ea(mpLayout->lyt()->mpRootPane.get()));
    for (u32 i = 0; i < 2; ++i) {
        mpPanes[i] = findCommandPane(root, &sCommandAPaneNames[i]);
    }
    mPaneWidths[0] = mpPanes[0]->mWidth;
    mPaneWidths[1] = mpPanes[1]->mWidth;
    mTextPosition[0] = mpPanes[1]->mPosition[0];
    mTextPosition[1] = mpPanes[1]->mPosition[1];
    mTextPosition[2] = mpPanes[1]->mPosition[2];
}

/* 02648148 */
void hdCommandPromptA_c::bindTextPane() {
    hdCommandPane_c* root = hdPartsView<hdCommandPane_c>(gabi::ea(mpLayout->lyt()->mpRootPane.get()));
    mpTextPane = (hdTextPane_c*)findCommandPane(root, &sCommandATextPaneName);
    mpTextPane->setCharacterCapacity(18);
}

/* 026481D8 */
u32 hdCommandPromptA_c::initialize(u32 first, u32 second, hdPartsDesc_c* descriptor) {
    if (create(&sCommandALayoutName, gabi::at<u8>(first), second, gabi::ea(descriptor)) == 0) {
        return 0;
    }
    bindPanes();
    bindTextPane();
    startState(&sCommandAHiddenState);
    return 1;
}

/* 02648294 */
void hdCommandPromptA_c::alignText() {
    f32 width = mpTextPane->getTextWidth();
    f32 paneWidth = mPaneWidths[0];
    f32 y = mTextPosition[1];
    f32 combined = gabi::fadds_ppc(paneWidth, width);
    f32 z = mTextPosition[2];
    combined = gabi::fadds_ppc(combined, sCommandATextMargin);
    f32 delta = gabi::fsubs_ppc(mPaneWidths[1], combined);
    f32 x = fmadds(delta, sCommandATextAnchor, mTextPosition[0]);
    hdCommandPane_c* pane = mpPanes[1];
    pane->mPosition[0] = x;
    pane->mPosition[1] = y;
    u8 flags = pane->mFlags;
    pane->mFlags = flags | 0x10;
    pane->mPosition[2] = z;
}

/* 02648324 */
void hdCommandPromptA_c::setMessage(PaneWork_l* message) {
    f32 rate = sCommandAAnimationRate;
    if (message != NULL) {
        mpTextPane->resetMessage((hdUiSafeString*)&message->mArguments[0],
                                 (hdUiSafeString*)&message->mLabelName);
        hdAnimSet_setFrame(animSet(), 1, 0, rate);
        alignText();
        mEnabled = true;
    } else {
        hdAnimSet_setFrame(animSet(), 1, 0, sCommandAStoppedFrame);
        hdAnimSet_play(animSet(), 2, 0, rate);
        mEnabled = true;
    }
}

/* 02647C08 */
void hdCommandPromptA_c::destroy(hdCommandPromptA_c* self, u32 flags) {
    if (self == NULL) {
        return;
    }
    self->mStateHolder.mHolderVtable = &sCommandAHolderVtable;
    self->mSecondaryVtable = &sCommandASecondaryVtable;
    self->NoControlPart_l::destroy(0);
    if (flags & 1) {
        operator_delete(self);
    }
}

/* 02647F50 */
hdCommandPromptA_c* hdCommandPromptA_c::construct(hdCommandPromptA_c* self) {
    if (self == NULL) {
        self = (hdCommandPromptA_c*)operator_new(sizeof(hdCommandPromptA_c));
        if (self == NULL) {
            return NULL;
        }
    }
    NoControlPart_l::construct(self);
    self->mVtable = &sCommandAVtable;
    self->mSecondaryVtable = &sCommandASecondaryVtable;
    self->mStateHolder.mHolderVtable = &sCommandAHolderVtable;
    // HD: retain the inline array constructor's wrapped-null allocation side effect.
    if (gabi::ea(&self->mpPanes[0]) == 0) {
        operator_new(8);
    }
    f32 zero = sCommandAStoppedFrame;
    self->mEnabled = false;
    self->mPaneWidths[1] = zero;
    self->mPaneWidths[0] = zero;
    self->mTextPosition[2] = zero;
    self->mpTextPane = NULL;
    self->mTextPosition[1] = zero;
    self->mWaitTimer = 0;
    self->mTextPosition[0] = zero;
    self->mpPanes[0] = NULL;
    self->mpPanes[1] = NULL;
    return self;
}

/* 02647C74 */
void hdCommandPromptA_c::enterHidden() {
    hdAnimSet_setFrame(animSet(), 0, 1, sCommandAStoppedFrame);
    mWaitTimer = 0;
}

/* 02647DBC */
void hdCommandPromptA_c::enterIn() {
    hdAnimSet_play(animSet(), 0, 1, sCommandAAnimationRate);
}

/* 02647DD8 */
void hdCommandPromptA_c::executeIn() {
    if (hdAnimSet_isEnd(animSet(), 0) != 0) {
        changeState(&sCommandADisplayState);
    }
    vCalcLayout();
}

/* 02647EEC */
void hdCommandPromptA_c::executeOut() {
    if (hdAnimSet_isEnd(animSet(), 3) != 0) {
        changeState(&sCommandAHiddenState);
    }
    vCalcLayout();
}

/* 02647E3C */
void hdCommandPromptA_c::enterWait() {
    mWaitTimer = 10;
}

/* 02647ED0 */
void hdCommandPromptA_c::enterOut() {
    hdAnimSet_play(animSet(), 3, 1, sCommandAAnimationRate);
}

/* 0264827C */
void hdCommandPromptA_c::hide() {
    mEnabled = false;
    changeState(&sCommandAHiddenState);
}

/* 026483EC */
void hdCommandPromptA_c::disable() {
    mEnabled = false;
}

// Definitions of the two existing records remain in the merged companion owner.

static hdUiStringTable sCommandASafeStringVtable;
static hdPartsStateIdVtbl_c sCommandAStateVtable;
static hdStateId sCommandNullState;
static u32 sCommandStateCounterReady;
static u32 sCommandStateCounter;
struct hdCommandHeaderStatics_c {
    be<f32> mPiPair[2];
    be<u8> mAngle;
    be<u8> mShortVector;
    u8 _paddingA[2];
    be<u32> mZero[4];
};
struct hdCommandRegistration_c { be<u32> mWords[3]; };
typedef hdCommandRegistration_c hdCommandRegistrations_t[3];
static hdCommandHeaderStatics_c sCommandAHeaderStatics;
static hdCommandRegistrations_t sCommandARegistrations;
static f32 sCommandAPiLower;
static f32 sCommandAPiUpper;
void hdCommandInitializeAngle(be<u8>*);
void hdCommandInitializeShortVector(be<u8>*);

typedef be<u8> hdCommandSpecialGroupText_t[9];
static hdCommandSpecialGroupText_t sCommandSpecialGroupText;

static void initializeCommandName(hdUiSafeString* name, const char* text) {
    name->mText.v = gabi::guest_addr(text);
    name->mVtable = &sCommandASafeStringVtable;
}

/* 02648510 */
static void initializeCommandAPromptStatics() {
    for (u32 i = 0; i < 4; ++i) {
        sCommandAHeaderStatics.mZero[i] = 0;
    }
    __register_global_object(gabi::ea(&sCommandARegistrations[0]));
    f32 lower = sCommandAPiLower;
    f32 upper = sCommandAPiUpper;
    sCommandAHeaderStatics.mPiPair[0] = lower;
    sCommandAHeaderStatics.mPiPair[1] = upper;
    hdCommandInitializeAngle(&sCommandAHeaderStatics.mAngle);
    __register_global_object(gabi::ea(&sCommandARegistrations[1]));
    hdCommandInitializeShortVector(&sCommandAHeaderStatics.mShortVector);
    __register_global_object(gabi::ea(&sCommandARegistrations[2]));

    initializeCommandName(&sCommandAAnimationGroups[0], "G_A_00");
    sCommandAAnimationGroups[1].mText = sCommandSpecialGroupText;
    sCommandAAnimationGroups[1].mVtable = &sCommandASafeStringVtable;
    initializeCommandName(&sCommandAAnimationNames[0], "InAll");
    initializeCommandName(&sCommandAAnimationNames[1], "ASpecial");
    initializeCommandName(&sCommandAAnimationNames[2], "ASpecialLoop");
    initializeCommandName(&sCommandAAnimationNames[3], "OutAll");
    initializeCommandName(&sCommandAPaneNames[0], "P_A_00");
    initializeCommandName(&sCommandAPaneNames[1], "N_Center_01");
    initializeCommandName(&sCommandATextPaneName, "T_CommandA_00");
    initializeCommandName(&sCommandALayoutName, "CommandA_00");

    u32 number;
    if (sCommandStateCounterReady != 0) {
        number = sCommandStateCounter;
    } else {
        sCommandStateCounterReady = 1;
        number = 0;
    }
    sCommandAHiddenState.setup(s32(++number), "::StateID_Wait", &sCommandAStateVtable, 0x19, &sCommandNullState);
    hdPartsStateId_c* inState = &sCommandActiveState;
    inState->setup(s32(++number), "::StateID_InAll", &sCommandAStateVtable, 0x1C, &sCommandNullState);
    sCommandADisplayState.setup(s32(++number), "::StateID_Disp", &sCommandAStateVtable, 0x1F, &sCommandNullState);
    hdPartsStateId_c* outState = &sCommandWaitState;
    outState->setup(s32(++number), "::StateID_OutAll", &sCommandAStateVtable, 0x22, &sCommandNullState);
    sCommandStateCounter = number;
}

/* WWHD glue
 ext 02704A08 f32 hdTextPane_c::getTextWidth()
 ext 02704AFC void hdTextPane_c::setCharacterCapacity(u32)
 ext 027045B0 void hdTextPane_c::resetMessage(hdUiSafeString*, hdUiSafeString*)
 data sCommandASafeStringVtable 100ED7B8
 data sCommandAStateVtable 100ED828
 data sCommandNullState 101FF32C nocheck
 data sCommandStateCounterReady 101FD8E4 nocheck
 data sCommandStateCounter 101FDD50 nocheck
 data sCommandAHeaderStatics 10491100
 data sCommandARegistrations 101F5864
 data sCommandAPiLower 100ED880
 data sCommandAPiUpper 100ED884
 str 100ED908 "::StateID_Wait" in 02648510
 str 100ED8A8 "::StateID_InAll" in 02648510
 str 100ED918 "::StateID_Disp" in 02648510
 str 100ED928 "::StateID_OutAll" in 02648510
 str 100ED890 "G_A_00" in 02648510
 str 100ED888 "InAll" in 02648510
 str 100ED8EC "ASpecial" in 02648510
 str 100ED8F8 "ASpecialLoop" in 02648510
 str 100ED898 "OutAll" in 02648510
 str 100ED8A0 "P_A_00" in 02648510
 str 100ED8B8 "N_Center_01" in 02648510
 str 100ED8C4 "CommandA_00" in 02648510
 str 100ED8D0 "T_CommandA_00" in 02648510
 ext 028ED6F8 void hdCommandInitializeAngle(be<u8>*)
 ext 028EAB2C void hdCommandInitializeShortVector(be<u8>*)
 data sCommandSpecialGroupText 100ED8E0
 data sCommandUiSystem 1018C404
 data sCommandAAnimationNames 1049112C
 data sCommandAAnimationGroups 1049111C
 data sCommandAAnimationGroupIndices 100ED7D0
 data sCommandAPaneNames 1049114C
 data sCommandATextPaneName 104910F8
 data sCommandALayoutName 1049121C
 data sCommandATextMargin 100ED878
 data sCommandATextAnchor 100ED87C
 data sCommandAVtable 100ED940
 data sCommandAHolderVtable 100EDA68
 data sCommandASecondaryVtable 100EDA78
 ext 02004E04 void hdCommandBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*)
 ext 02700A70 void NoControlPart_l::destroy(u32)
 ext 02700ADC NoControlPart_l* NoControlPart_l::construct(NoControlPart_l*)
 ext 02700B48 u32 NoControlPart_l::create(void*,void*,u32,u32)
 data sCommandAStoppedFrame 100ED874
 data sCommandAAnimationRate 100ED870
 data sCommandAHiddenState 1049115C
 data sCommandADisplayState 104911BC
*/

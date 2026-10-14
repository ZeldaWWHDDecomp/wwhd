/** HD elapsed-time counter and its bottom/top transition states. */
#include "d/hd_parts_text_pane.h"
#include "d/hd_screen_time.h"
static u32 sTimeStateInterfaceVtable;
static u32 sTimeDrawInterfaceVtable;
static hdPartsVtbl_c sTimeVtable;
static hdPartsStateId_c sTimeWaitState;
static hdPartsStateId_c sTimeInState;
static hdPartsStateId_c sTimeDisplayState;
static hdPartsStateId_c sTimeOutState;
struct TimeInAnimationsView_l {
    be<u32> mValues[2];
    be<u32>& operator[](u32 index) {
        return *hdPartsView<be<u32>>(gabi::ea(this) + index * u32(sizeof(be<u32>)));
    }
};
static TimeInAnimationsView_l sTimeInAnimations;
struct TimeOutAnimationsView_l {
    be<u32> mValues[2];
    be<u32>& operator[](u32 index) {
        return *hdPartsView<be<u32>>(gabi::ea(this) + index * u32(sizeof(be<u32>)));
    }
};
static TimeOutAnimationsView_l sTimeOutAnimations;
static u8 sTimeEnabled;
struct hdTimePlay_l {
    u8 mPrefix[0x5CD8]; be<u32> mDisplayFlags;
    u8 mToTicks[0x5CF4 - 0x5CDC];
    be<u32> mStartTicks, mEndTicks;
};
WWHD_OFFSET(hdTimePlay_l, mDisplayFlags, 0x5CD8);
WWHD_OFFSET(hdTimePlay_l, mStartTicks, 0x5CF4);
WWHD_OFFSET(hdTimePlay_l, mEndTicks, 0x5CF8);
struct hdTimeText_l {
    gptr<be<u16>> mText; gptr<void> mVtable; be<u32> mCapacity; be<u16> mBuffer[9];
};
WWHD_OFFSET(hdTimeText_l, mText, 0);
WWHD_OFFSET(hdTimeText_l, mVtable, 4);
WWHD_OFFSET(hdTimeText_l, mCapacity, 8);
static u32 sTimeTextVtable;
struct TimeFormatView_l {
    be<u16> mValues[13];
    be<u16>& operator[](u32 index) { return mValues[index]; }
};
static TimeFormatView_l sTimeFormat;
hdTimePlay_l* hdTime_getPlay();
u32 hdTime_isEvent();
void* hdTime_allocate(u32);
void hdTime_delete(void*);
void hdTime_baseConstruct(NoControlPart_l*);
void hdTime_baseDestroy(NoControlPart_l*, u32);


static inline void hdTime_drawLayout(hdTimeScreen_c* receiver, void* info) { receiver->hdParts_c::draw(info); }


u32 hdTime_animationEnded(hdAnimSet*, u32);
void hdTime_formatText(hdTimeText_l*, const be<u16>*, s32, s32, s32);
static inline void hdTime_setPaneText(void* pane, hdTimeText_l* text, u32 work) {
    reinterpret_cast<hdTextPane_c*>(pane)->setStringPrepared(reinterpret_cast<PaneString_l*>(text), work ? hdPartsView<PaneWork_l>(work) : nullptr);
}
/* 026E9A38 */
void hdTimeScreen_c::destroy(hdTimeScreen_c* screen, u32 flags) {
    if (!screen)
        return;
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sTimeStateInterfaceVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sTimeDrawInterfaceVtable);
    hdTime_baseDestroy(screen, 0);
    if (flags & 1)
        hdTime_delete(screen);
}
/* 026E9AA4 */
void hdTimeScreen_c::resetAnimation() {
    u32 placement = mPlacement;
    hdAnimSet* animations = mpLayout->animSet();
    hdAnimSet_operationCC(animations, sTimeInAnimations[placement], 0);
}
/* 026E9AC8 */
BOOL hdTimeScreen_c::canDisplay() {
    if (!sTimeEnabled)
        return FALSE;
    if (hdTime_isEvent())
        return TRUE;
    return !(u32(hdTime_getPlay()->mDisplayFlags) & 0x10);
}
/* 026E9B24 */
void hdTimeScreen_c::waitExecute() {
    if (canDisplay())
        hdParts_changeState(stateMachine(), &sTimeInState);
}
/* 026E9B68 */
void hdTimeScreen_c::inEnter() {
    u32 placement = mPlacement;
    hdPartsLayout_c* layout = mpLayout;
    u32 animation = sTimeInAnimations[placement];
    mAnimation = animation;
    hdAnimSet_play(layout->animSet(), animation, 0, 1.0f);
    mVisible = 1;
}
/* 026E9BC4 */
void hdTimeScreen_c::updateText() {
    if (!mpTimePane)
        return;
    u32 end = hdTime_getPlay()->mEndTicks;
    u32 start = hdTime_getPlay()->mStartTicks;
    s32 milliseconds = s32((end - start) * 1000) / 30;
    if (milliseconds < 0)
        milliseconds = 0;
    s32 seconds = milliseconds / 1000;
    s32 minutes = seconds / 60;
    s32 hundredths = (milliseconds - seconds * 1000) / 10;
    seconds -= minutes * 60;
    hdTimeText_l text;
    text.mText = &text.mBuffer[0];
    text.mBuffer[0] = 0;
    text.mBuffer[8] = 0;
    text.mCapacity = 9;
    text.mVtable.v = gabi::ea(&sTimeTextVtable);
    hdTime_formatText(&text, sTimeFormat.mValues, minutes, seconds, hundredths);
    hdTime_setPaneText(mpTimePane, &text, 0);
}
/* 026E9CE4 */
void hdTimeScreen_c::inExecute() {
    updateText();
    if (hdTime_animationEnded(mpLayout->animSet(), mAnimation))
        hdParts_changeState(stateMachine(), &sTimeDisplayState);
}
/* 026E9D38 */
void hdTimeScreen_c::displayExecute() {
    updateText();
    if (!canDisplay())
        hdParts_changeState(stateMachine(), &sTimeOutState);
}
/* 026E9D84 */
void hdTimeScreen_c::outEnter() {
    u32 placement = mPlacement;
    hdPartsLayout_c* layout = mpLayout;
    u32 animation = sTimeOutAnimations[placement];
    mAnimation = animation;
    hdAnimSet_play(layout->animSet(), animation, 0, 1.0f);
}
/* 026E9DB0 */
void hdTimeScreen_c::outExecute() {
    if (hdTime_animationEnded(mpLayout->animSet(), mAnimation))
        hdParts_changeState(stateMachine(), &sTimeWaitState);
}
/* 026E9E00 */
void hdTimeScreen_c::outExit() {
    mVisible = 0;
}
/* 026E9E0C */
hdTimeScreen_c* hdTimeScreen_c::construct(hdTimeScreen_c* screen) {
    if (!screen)
        screen = static_cast<hdTimeScreen_c*>(hdTime_allocate(sizeof(hdTimeScreen_c)));
    if (screen) {
        hdTime_baseConstruct(screen);
        screen->mStateHolder.mHolderVtable.v = gabi::ea(&sTimeStateInterfaceVtable);
        screen->mVtable = &sTimeVtable;
        screen->mSecondaryVtable.v = gabi::ea(&sTimeDrawInterfaceVtable);
        screen->mPlacement = 0;
        screen->mVisible = 0;
        screen->mpTimePane = NULL;
        screen->mAnimation = 0;
    }
    return screen;
}
/* 026E9F8C */
void hdTimeScreen_c::calc() {
    hdTime_getPlay();
    hdParts_executeStateMachine(stateMachine());
}
/* 026E9FC0 */
void hdTimeScreen_c::draw(void* info) {
    if (mVisible)
        hdTime_drawLayout(this, info);
}
/* 026E9FD0 */
void hdTimeScreen_c::resetState() {
    hdParts_changeState(stateMachine(), &sTimeWaitState);
}
/* 026E9FE0 */
void hdTimeScreen_c::setBottom() {
    mPlacement = 0;
}
/* 026E9FEC */
void hdTimeScreen_c::setTop() {
    mPlacement = 1;
}
typedef gptr<hdScreenUiSystem> hdTimeUiSystemPointer_l;
static hdTimeUiSystemPointer_l sTimeUiSystem;
struct TimeGroupNamesView_l {
    hdUiSafeString mValues[1];
    hdUiSafeString& operator[](u32 index) { return mValues[index]; }
};
static TimeGroupNamesView_l sTimeGroupNames;
struct TimeAnimationNamesView_l {
    hdUiSafeString mValues[4];
    hdUiSafeString& operator[](u32 index) { return mValues[index]; }
};
static TimeAnimationNamesView_l sTimeAnimationNames;
struct TimeGroupIndicesView_l {
    be<u32> mValues[4];
    be<u32>& operator[](u32 index) { return mValues[index]; }
};
static TimeGroupIndicesView_l sTimeGroupIndices;
void* hdTime_allocateLayout(u32,hdScreenHeap*,u32);

void hdTime_registerAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*);
/* 026E9FF8 */
BOOL hdTimeScreen_c::loadLayout(void* name,u32 arg1,u32 arg2,u32 arg3,void* desc) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdTime_allocateLayout(sizeof(hdPartsLayout_c),sTimeUiSystem->mHeap,4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout) return FALSE;
    if (!layout->vLoad(name,arg1,arg2,arg3,4,1,desc)) return FALSE;
    for (u32 i=0; i<4; ++i) {
        u32 groupAddress = gabi::ea(&sTimeGroupNames) + u32(sTimeGroupIndices[i]) * sizeof(hdUiSafeString);
        hdTime_registerAnimation(mpLayout->animSet(),i,&sTimeAnimationNames[i],hdPartsView<hdUiSafeString>(groupAddress));
    }
    return TRUE;
}
struct hdTimeFindPaneVtbl_l { u8 mPrefix[0x5C]; gfn mFindPane; };
WWHD_OFFSET(hdTimeFindPaneVtbl_l, mFindPane, 0x5C);
static hdUiSafeString sTimePaneName;
static hdUiSafeString sTimeCreateDescriptor;
u32 hdTime_baseCreate(NoControlPart_l*,void*,u32,u32,u32);
/* 026E9E8C */
BOOL hdTimeScreen_c::create(u32 arg1,u32 arg2,u32 arg3) {
    if (!hdTime_baseCreate(this,&sTimeCreateDescriptor,arg1,arg2,arg3)) return FALSE;
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    hdTimeFindPaneVtbl_l* table = hdPartsView<hdTimeFindPaneVtbl_l>(gabi::ea(root->mVtable.get()));
    sTimePaneName.assureTermination();
    mpTimePane = gabi::call_ptr<void*>(table->mFindPane.get(),root,sTimePaneName.mText.get(),1u);
    startState(&sTimeWaitState);
    return TRUE;
}
struct hdTimeStaticRegistration_l { be<u32> mWords[3]; };
struct TimeStaticWordsView_l {
    be<u32> mValues[4];
    be<u32>& operator[](u32 index) { return mValues[index]; }
};
static TimeStaticWordsView_l sTimeStaticWords;
typedef be<u32> hdTimeWord_l;
static hdTimeWord_l sTimeCounterReady;
static hdTimeWord_l sTimeCounter;
typedef be<f32> hdTimeFloat_l;
static hdTimeFloat_l sTimeMinimumAngle;
static hdTimeFloat_l sTimeMaximumAngle;
static u8 sTimeFirstLibraryStatic;
static u8 sTimeSecondLibraryStatic;
static hdTimeStaticRegistration_l sTimeWordsRegistration;
static hdTimeStaticRegistration_l sTimeFirstRegistration;
static hdTimeStaticRegistration_l sTimeSecondRegistration;
static hdUiStringTable sTimeStringVtable;
static hdPartsStateIdVtbl_c sTimeStateVtable;
static hdStateId sTimeNullState;
struct TimeInitializedStatesView_l {
    hdPartsStateId_c mValues[4];
    hdPartsStateId_c& operator[](u32 index) { return mValues[index]; }
};
static TimeInitializedStatesView_l sTimeInitializedStates;
void hdTime_registerGlobal(void*);
void hdTime_constructFirstStatic(void*);
void hdTime_constructSecondStatic(void*);
static hdUiSafeString sTimeArchiveName;
/* 026EA110 */
void hdTimeScreen_c::initializeStatics() {
    sTimeStaticWords[3] = 0;
    sTimeStaticWords[2] = 0;
    sTimeStaticWords[1] = 0;
    sTimeStaticWords[0] = 0;
    hdTime_registerGlobal(&sTimeWordsRegistration);
    sTimeMinimumAngle = -3.1415927410125732f;
    sTimeMaximumAngle = 3.1415927410125732f;
    hdTime_constructFirstStatic(&sTimeFirstLibraryStatic);
    hdTime_registerGlobal(&sTimeFirstRegistration);
    hdTime_constructSecondStatic(&sTimeSecondLibraryStatic);
    hdTime_registerGlobal(&sTimeSecondRegistration);
    sTimeGroupNames[0].mVtable = &sTimeStringVtable;
    sTimeGroupNames[0].mText.v = gabi::guest_addr("G_All_00");
    sTimePaneName.mVtable = &sTimeStringVtable;
    sTimePaneName.mText.v = gabi::guest_addr("T_Time_00");
    sTimeArchiveName.mVtable = &sTimeStringVtable;
    sTimeArchiveName.mText.v = gabi::guest_addr("L_Time_00-T_Time_00");
    sTimeAnimationNames[0].mVtable = &sTimeStringVtable;
    sTimeAnimationNames[0].mText.v = gabi::guest_addr("InBottom");
    sTimeAnimationNames[1].mVtable = &sTimeStringVtable;
    sTimeAnimationNames[1].mText.v = gabi::guest_addr("OutBottom");
    sTimeAnimationNames[2].mVtable = &sTimeStringVtable;
    sTimeAnimationNames[2].mText.v = gabi::guest_addr("InTop");
    sTimeAnimationNames[3].mVtable = &sTimeStringVtable;
    sTimeAnimationNames[3].mText.v = gabi::guest_addr("OutTop");
    sTimeCreateDescriptor.mVtable = &sTimeStringVtable;
    sTimeCreateDescriptor.mText.v = gabi::guest_addr("Time_00");
    u32 number = 0;
    if (sTimeCounterReady) number = sTimeCounter;
    else sTimeCounterReady = 1;
    sTimeInitializedStates[0].setup(++number,"::StateID_Wait",&sTimeStateVtable,0x19,&sTimeNullState);
    sTimeInitializedStates[1].setup(++number,"::StateID_In",&sTimeStateVtable,0x1C,&sTimeNullState);
    sTimeInitializedStates[2].setup(++number,"::StateID_Disp",&sTimeStateVtable,0x1F,&sTimeNullState);
    sTimeInitializedStates[3].setup(++number,"::StateID_Out",&sTimeStateVtable,0x22,&sTimeNullState);
    sTimeCounter = number;
}
/* WWHD glue (tools/verify/autoglue.py)
 str 10107CC8 "G_All_00" in 026EA110
 str 10107CAC "T_Time_00" in 026EA110
 str 10107C8C "L_Time_00-T_Time_00" in 026EA110
 str 10107CD4 "InBottom" in 026EA110
 str 10107CA0 "OutBottom" in 026EA110
 str 10107C7C "InTop" in 026EA110
 str 10107C84 "OutTop" in 026EA110
 str 10107C74 "Time_00" in 026EA110
 str 10107CE0 "::StateID_Wait" in 026EA110
 str 10107CF0 "::StateID_In" in 026EA110
 str 10107D00 "::StateID_Disp" in 026EA110
 str 10107CB8 "::StateID_Out" in 026EA110
 data sTimeStaticWords 1049CCB0
 data sTimeMinimumAngle 1049CCA4
 data sTimeMaximumAngle 1049CCA8
 data sTimeFirstLibraryStatic 1049CCAC
 data sTimeSecondLibraryStatic 1049CCAD
 data sTimeWordsRegistration 101F6FA0
 data sTimeFirstRegistration 101F6FAC
 data sTimeSecondRegistration 101F6FB8
 data sTimeStringVtable 10107B5C
 data sTimeStateVtable 10107BFC
 data sTimeNullState 101FF32C
 data sTimeInitializedStates 1049CCE0
 data sTimeCounterReady 101FD8E4
 data sTimeCounter 101FDD50
 data sTimeArchiveName 1049CC9C
 ext 028F026C void hdTime_registerGlobal(void*)
 ext 028ED6F8 void hdTime_constructFirstStatic(void*)
 ext 028EAB2C void hdTime_constructSecondStatic(void*)
 data sTimePaneName 1049CC94
 data sTimeCreateDescriptor 1049CDA0
 ext 02700B48 u32 hdTime_baseCreate(NoControlPart_l*,void*,u32,u32,u32)
 data sTimeUiSystem 1018C404
 data sTimeGroupNames 1049CC8C
 data sTimeAnimationNames 1049CCC0
 data sTimeGroupIndices 10107BA4
 ext 0273B050 void* hdTime_allocateLayout(u32,hdScreenHeap*,u32)

 ext 02004E04 void hdTime_registerAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
 data sTimeStateInterfaceVtable 10107E38
 data sTimeDrawInterfaceVtable 10107E48
 data sTimeVtable 10107D10
 data sTimeWaitState 1049CCE0
 data sTimeInState 1049CD10
 data sTimeDisplayState 1049CD40
 data sTimeOutState 1049CD70
 data sTimeInAnimations 10107B48
 data sTimeOutAnimations 10107B50
 data sTimeEnabled 1047B099
 data sTimeTextVtable 10107B8C
 data sTimeFormat 10107C4C
 ext 025200D4 hdTimePlay_l* hdTime_getPlay()
 ext 025986B0 u32 hdTime_isEvent()
 ext 0273AD10 void* hdTime_allocate(u32)
 ext 0273AF40 void hdTime_delete(void*)
 ext 02700ADC void hdTime_baseConstruct(NoControlPart_l*)
 ext 02700A70 void hdTime_baseDestroy(NoControlPart_l*,u32)

 ext 02005840 u32 hdTime_animationEnded(hdAnimSet*,u32)
 ext 02759D3C void hdTime_formatText(hdTimeText_l*,const be<u16>*,s32,s32,s32)


 ext 026F8B54 void hdParts_c::draw(void*)
*/

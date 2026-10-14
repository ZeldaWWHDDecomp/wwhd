/** HD swim timer entry/exit states; merged blink/flash/update bodies remain separate. */
#include "d/hd_screen_swim_time_hud.h"
static u32 sSwimStateInterfaceVtable;
static u32 sSwimDrawInterfaceVtable;
static hdPartsVtbl_c sSwimVtable;
struct SwimAnimationModesView_l {
    be<u32> mValues[16];
    be<u32>& operator[](u32 index) { return mValues[index]; }
};
static SwimAnimationModesView_l sSwimAnimationModes;
static hdPartsStateId_c sSwimWaitState;
static hdPartsStateId_c sSwimInState;
static hdPartsStateId_c sSwimDisplayState;
struct hdSwimPlay_l {
    u8 mPrefix[0x5292]; be<u8> mEventRunning;
    u8 mToTimer[0x5B4C - 0x5293]; be<s32> mTime, mMaximum;
    u8 mToRunning[0x5BB0 - 0x5B54]; be<u8> mTimerRunning;
};
WWHD_OFFSET(hdSwimPlay_l, mEventRunning, 0x5292);
WWHD_OFFSET(hdSwimPlay_l, mTime, 0x5B4C);
WWHD_OFFSET(hdSwimPlay_l, mTimerRunning, 0x5BB0);
hdSwimPlay_l* hdSwim_getPlay();
u32 hdSwim_isEvent(), hdSwim_isBlocked();
void hdSwim_baseDestroy(NoControlPart_l*, u32);
void hdSwim_delete(void*);
void* hdSwim_allocate(u32);
void hdSwim_baseConstruct(NoControlPart_l*);



u32 hdSwim_animationEnded(hdAnimSet*, u32);
void hdSwim_sound(u32);

/* 026E6340 */
void hdSwimTimeHud_c::destroy(hdSwimTimeHud_c* screen, u32 flags) {
    if (!screen)
        return;
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sSwimStateInterfaceVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sSwimDrawInterfaceVtable);
    hdSwim_baseDestroy(screen, 0);
    if (flags & 1)
        hdSwim_delete(screen);
}
/* 026E63AC */
void hdSwimTimeHud_c::resetAnimations() {
    hdAnimSet_operationCC(mpLayout->animSet(), 0, 0);
    for (u32 i = 0; i < 6; ++i) {
        u32 animation = i + 4;
        hdAnimSet_operationCC(mpLayout->animSet(), animation, sSwimAnimationModes[animation]);
        mFlashing[i] = 0;
    }
}
/* 026E6428 */
BOOL hdSwimTimeHud_c::canDisplay() {
    u32 event = hdSwim_isEvent();
    u32 blocked = hdSwim_isBlocked();
    u32 runningEvent = hdSwim_getPlay()->mEventRunning != 0;
    u8 running = hdSwim_getPlay()->mTimerRunning;
    if (!(event | blocked | runningEvent) && running) {
        if (!mAvailable)
            mAvailable = 1;
        return TRUE;
    }
    if (mAvailable)
        mAvailable = 0;
    return FALSE;
}
/* 026E64F4 */
void hdSwimTimeHud_c::waitExecute() {
    s32 time = hdSwim_getPlay()->mTime;
    if (time && canDisplay())
        hdParts_changeState(stateMachine(), &sSwimInState);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026E6560 */
void hdSwimTimeHud_c::inEnter() {
    s32 time = hdSwim_getPlay()->mTime;
    s32 maximum = hdSwim_getPlay()->mMaximum;
    if (time < maximum) {
        hdAnimSet* animations = mpLayout->animSet();
        mInAnimation = 12;
        hdAnimSet_play(animations, 12, sSwimAnimationModes[12], 1.0f);
        hdSwim_sound(0x83F);
        mRatio = 1.0f;
    } else {
        mInAnimation = 0;
        hdAnimSet_operationCC(mpLayout->animSet(), 10, 1);
        hdAnimSet_operationCC(mpLayout->animSet(), 1, 3);
        hdAnimSet_operationCC(mpLayout->animSet(), 2, 2);
        u32 animation = mInAnimation;
        hdAnimSet_play(mpLayout->animSet(), animation, sSwimAnimationModes[animation], 1.0f);
        hdSwim_sound(0x83F);
        mRatio = 1.0f;
    }
}
/* 026E667C */
void hdSwimTimeHud_c::inExecute() {
    if (hdSwim_animationEnded(mpLayout->animSet(), mInAnimation))
        hdParts_changeState(stateMachine(), &sSwimDisplayState);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026E66E0 */
void hdSwimTimeHud_c::displayEnter() {
    hdAnimSet_play(mpLayout->animSet(), 1, 3, 1.0f);
    hdAnimSet_play(mpLayout->animSet(), 2, 2, 1.0f);
    mBlinkFrame = 0;
    mBlinkPeriod = 20;
    mFlashDelay = 0;
    mBlinking = 0;
    mSoundCooldown = 0;
}
/* 026E6D2C */
void hdSwimTimeHud_c::outEnter() {
    hdAnimSet_play(mpLayout->animSet(), 11, 0, 1.0f);
}
/* 026E6D48 */
void hdSwimTimeHud_c::outExecute() {
    if (hdSwim_animationEnded(mpLayout->animSet(), 11))
        hdParts_changeState(stateMachine(), &sSwimWaitState);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026E6DAC */
void hdSwimTimeHud_c::finishedExecute() {
    if (!canDisplay())
        hdParts_changeState(stateMachine(), &sSwimWaitState);
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}
/* 026E6E04 */
hdSwimTimeHud_c* hdSwimTimeHud_c::construct(hdSwimTimeHud_c* screen) {
    if (!screen)
        screen = static_cast<hdSwimTimeHud_c*>(hdSwim_allocate(sizeof(hdSwimTimeHud_c)));
    if (screen) {
        hdSwim_baseConstruct(screen);
        screen->mVtable = &sSwimVtable;
        screen->mSecondaryVtable.v = gabi::ea(&sSwimDrawInterfaceVtable);
        u32 paneAddress = gabi::ea(screen) + offsetof(hdSwimTimeHud_c, mpPanes);
        screen->mStateHolder.mHolderVtable.v = gabi::ea(&sSwimStateInterfaceVtable);
        if (!paneAddress)
            hdSwim_allocate(24);
        u32 positionAddress = gabi::ea(screen) + offsetof(hdSwimTimeHud_c, mPaneY);
        if (!positionAddress)
            hdSwim_allocate(24);
        screen->mBlinkPeriod = 0;
        screen->mBlinkRange = 5.0f;
        screen->mSoundCooldown = 0;
        screen->mInAnimation = 0;
        screen->mRatio = 1.0f;
        screen->mBlinking = 0;
        screen->mFlashDelay = 0;
        screen->mBlinkFrame = 0;
        screen->mDistance = 0;
        screen->mAvailable = 1;
        for (u32 i = 0; i < 6; ++i) {
            hdPartsView<gptr<void>>(paneAddress)[i] = NULL;
            hdPartsView<be<f32>>(positionAddress)[i] = 0;
            screen->mFlashing[i] = 0;
        }
    }
    return screen;
}
/* 026E7164 */
void hdSwimTimeHud_c::resetState() {
    hdParts_changeState(stateMachine(), &sSwimWaitState);
}
typedef gptr<hdScreenUiSystem> hdSwimUiSystemPointer_l;
static hdSwimUiSystemPointer_l sSwimUiSystem;
struct SwimGroupNamesView_l {
    hdUiSafeString mValues[10];
    hdUiSafeString& operator[](u32 index) { return mValues[index]; }
};
static SwimGroupNamesView_l sSwimGroupNames;
struct SwimAnimationNamesView_l {
    hdUiSafeString mValues[13];
    hdUiSafeString& operator[](u32 index) { return mValues[index]; }
};
static SwimAnimationNamesView_l sSwimAnimationNames;
struct SwimGroupIndicesView_l {
    be<u32> mValues[13];
    be<u32>& operator[](u32 index) { return mValues[index]; }
};
static SwimGroupIndicesView_l sSwimGroupIndices;
void* hdSwim_allocateLayout(u32,hdScreenHeap*,u32);

void hdSwim_registerAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*);
/* 026E7174 */
BOOL hdSwimTimeHud_c::loadLayout(void* name,u32 arg1,u32 arg2,u32 arg3,void* desc) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdSwim_allocateLayout(sizeof(hdPartsLayout_c),sSwimUiSystem->mHeap,4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout) return FALSE;
    if (!layout->vLoad(name,arg1,arg2,arg3,13,10,desc)) return FALSE;
    for (u32 i=0; i<13; ++i) {
        u32 groupAddress = gabi::ea(&sSwimGroupNames) + u32(sSwimGroupIndices[i]) * sizeof(hdUiSafeString);
        hdSwim_registerAnimation(mpLayout->animSet(),i,&sSwimAnimationNames[i],hdPartsView<hdUiSafeString>(groupAddress));
    }
    return TRUE;
}
struct hdSwimFindPaneVtbl_l { u8 mPrefix[0x5C]; gfn mFindPane; };
WWHD_OFFSET(hdSwimFindPaneVtbl_l, mFindPane, 0x5C);
struct SwimPaneNamesView_l {
    hdUiSafeString mValues[6];
    hdUiSafeString& operator[](u32 index) { return mValues[index]; }
};
static SwimPaneNamesView_l sSwimPaneNames;
static hdUiSafeString sSwimCreateDescriptor;
u32 hdSwim_baseCreate(NoControlPart_l*,void*,u32,u32,u32);
struct hdSwimPanePosition_l { u8 mPrefix[0x20]; be<f32> mY; };
struct hdSwimPaneRect_l { be<f32> mLeft,mTop,mRight,mBottom; };
void hdSwim_getPaneRect(void*,hdSwimPaneRect_l*);
/* 026E6F48 */
BOOL hdSwimTimeHud_c::create(u32 arg1,u32 arg2,u32 arg3) {
    if (!hdSwim_baseCreate(this,&sSwimCreateDescriptor,arg1,arg2,arg3)) return FALSE;
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    for (u32 i=0; i<6; ++i) {
        hdAnimSet_operationCC(mpLayout->animSet(),i+4,sSwimAnimationModes[i+4]);
        hdUiSafeString* name = &sSwimPaneNames[i];
        hdSwimFindPaneVtbl_l* table = hdPartsView<hdSwimFindPaneVtbl_l>(gabi::ea(root->mVtable.get()));
        name->assureTermination();
        mpPanes[i] = gabi::call_ptr<void*>(table->mFindPane.get(),root,name->mText.get(),1u);
        if (mpPanes[i]) mPaneY[i] = ((hdSwimPanePosition_l*)mpPanes[i].get())->mY;
    }
    hdSwimFindPaneVtbl_l* table = hdPartsView<hdSwimFindPaneVtbl_l>(gabi::ea(root->mVtable.get()));
    void* meter = gabi::call_ptr<void*>(table->mFindPane.get(),root,"SwimTimeBase_00",1u);
    if (meter) {
        hdSwimPaneRect_l rect;
        hdSwim_getPaneRect(meter,&rect);
        mDistance = f32(rect.mTop) - f32(rect.mBottom);
    }
    startState(&sSwimWaitState);
    return TRUE;
}
struct hdSwimStaticRegistration_l { be<u32> mWords[3]; };
struct SwimStaticWordsView_l {
    be<u32> mValues[4];
    be<u32>& operator[](u32 index) { return mValues[index]; }
};
static SwimStaticWordsView_l sSwimStaticWords;
typedef be<u32> hdSwimWord_l;
static hdSwimWord_l sSwimCounterReady;
static hdSwimWord_l sSwimCounter;
typedef be<f32> hdSwimFloat_l;
static hdSwimFloat_l sSwimMinimumAngle;
static hdSwimFloat_l sSwimMaximumAngle;
static u8 sSwimFirstLibraryStatic;
static u8 sSwimSecondLibraryStatic;
static hdSwimStaticRegistration_l sSwimWordsRegistration;
static hdSwimStaticRegistration_l sSwimFirstRegistration;
static hdSwimStaticRegistration_l sSwimSecondRegistration;
static hdUiStringTable sSwimStringVtable;
static hdPartsStateIdVtbl_c sSwimStateVtable;
static hdStateId sSwimNullState;
struct SwimInitializedStatesView_l {
    hdPartsStateId_c mValues[5];
    hdPartsStateId_c& operator[](u32 index) { return mValues[index]; }
};
static SwimInitializedStatesView_l sSwimInitializedStates;
void hdSwim_registerGlobal(void*);
void hdSwim_constructFirstStatic(void*);
void hdSwim_constructSecondStatic(void*);
struct SwimFarLimitsView_l {
    be<f32> mValues[2];
    be<f32>& operator[](u32 index) { return mValues[index]; }
};
static SwimFarLimitsView_l sSwimFarLimits;
struct SwimNearLimitsView_l {
    be<f32> mValues[2];
    be<f32>& operator[](u32 index) { return mValues[index]; }
};
static SwimNearLimitsView_l sSwimNearLimits;
/* 026E728C */
void hdSwimTimeHud_c::initializeStatics() {
    sSwimStaticWords[3] = 0;
    sSwimStaticWords[2] = 0;
    sSwimStaticWords[1] = 0;
    sSwimStaticWords[0] = 0;
    hdSwim_registerGlobal(&sSwimWordsRegistration);
    sSwimMinimumAngle = -3.1415927410125732f;
    sSwimMaximumAngle = 3.1415927410125732f;
    hdSwim_constructFirstStatic(&sSwimFirstLibraryStatic);
    hdSwim_registerGlobal(&sSwimFirstRegistration);
    hdSwim_constructSecondStatic(&sSwimSecondLibraryStatic);
    hdSwim_registerGlobal(&sSwimSecondRegistration);
    sSwimFarLimits[0] = 50000.0f;
    sSwimFarLimits[1] = 50000.0f;
    sSwimNearLimits[0] = 10000.0f;
    sSwimNearLimits[1] = 10000.0f;
    sSwimPaneNames[0].mVtable = &sSwimStringVtable;
    sSwimPaneNames[0].mText.v = gabi::guest_addr("N_Light_00");
    sSwimPaneNames[1].mVtable = &sSwimStringVtable;
    sSwimPaneNames[1].mText.v = gabi::guest_addr("N_Light_01");
    sSwimPaneNames[2].mVtable = &sSwimStringVtable;
    sSwimPaneNames[2].mText.v = gabi::guest_addr("N_Light_02");
    sSwimPaneNames[3].mVtable = &sSwimStringVtable;
    sSwimPaneNames[3].mText.v = gabi::guest_addr("N_Light_03");
    sSwimPaneNames[4].mVtable = &sSwimStringVtable;
    sSwimPaneNames[4].mText.v = gabi::guest_addr("N_Light_04");
    sSwimPaneNames[5].mVtable = &sSwimStringVtable;
    sSwimPaneNames[5].mText.v = gabi::guest_addr("N_Light_05");
    sSwimGroupNames[0].mVtable = &sSwimStringVtable;
    sSwimGroupNames[0].mText.v = gabi::guest_addr("G_SwimTime_00");
    sSwimGroupNames[1].mVtable = &sSwimStringVtable;
    sSwimGroupNames[1].mText.v = gabi::guest_addr("G_SwimTimeMeter_00");
    sSwimGroupNames[2].mVtable = &sSwimStringVtable;
    sSwimGroupNames[2].mText.v = gabi::guest_addr("G_SwimTimeColor_00");
    sSwimGroupNames[3].mVtable = &sSwimStringVtable;
    sSwimGroupNames[3].mText.v = gabi::guest_addr("G_SwimTimeSpec_00");
    sSwimGroupNames[4].mVtable = &sSwimStringVtable;
    sSwimGroupNames[4].mText.v = gabi::guest_addr("G_Light_00");
    sSwimGroupNames[5].mVtable = &sSwimStringVtable;
    sSwimGroupNames[5].mText.v = gabi::guest_addr("G_Light_01");
    sSwimGroupNames[6].mVtable = &sSwimStringVtable;
    sSwimGroupNames[6].mText.v = gabi::guest_addr("G_Light_02");
    sSwimGroupNames[7].mVtable = &sSwimStringVtable;
    sSwimGroupNames[7].mText.v = gabi::guest_addr("G_Light_03");
    sSwimGroupNames[8].mVtable = &sSwimStringVtable;
    sSwimGroupNames[8].mText.v = gabi::guest_addr("G_Light_04");
    sSwimGroupNames[9].mVtable = &sSwimStringVtable;
    sSwimGroupNames[9].mText.v = gabi::guest_addr("G_Light_05");
    sSwimAnimationNames[0].mVtable = &sSwimStringVtable;
    sSwimAnimationNames[0].mText.v = gabi::guest_addr("SwimTimeIn");
    sSwimAnimationNames[1].mVtable = &sSwimStringVtable;
    sSwimAnimationNames[1].mText.v = gabi::guest_addr("SwimTimeSpec");
    sSwimAnimationNames[2].mVtable = &sSwimStringVtable;
    sSwimAnimationNames[2].mText.v = gabi::guest_addr("SwimTimeColor_00");
    sSwimAnimationNames[3].mVtable = &sSwimStringVtable;
    sSwimAnimationNames[3].mText.v = gabi::guest_addr("SwimTimeColor_01");
    sSwimAnimationNames[4].mVtable = &sSwimStringVtable;
    sSwimAnimationNames[4].mText.v = gabi::guest_addr("Light");
    sSwimAnimationNames[5].mVtable = &sSwimStringVtable;
    sSwimAnimationNames[5].mText.v = gabi::guest_addr("Light");
    sSwimAnimationNames[6].mVtable = &sSwimStringVtable;
    sSwimAnimationNames[6].mText.v = gabi::guest_addr("Light");
    sSwimAnimationNames[7].mVtable = &sSwimStringVtable;
    sSwimAnimationNames[7].mText.v = gabi::guest_addr("Light");
    sSwimAnimationNames[8].mVtable = &sSwimStringVtable;
    sSwimAnimationNames[8].mText.v = gabi::guest_addr("Light");
    sSwimAnimationNames[9].mVtable = &sSwimStringVtable;
    sSwimAnimationNames[9].mText.v = gabi::guest_addr("Light");
    sSwimAnimationNames[10].mVtable = &sSwimStringVtable;
    sSwimAnimationNames[10].mText.v = gabi::guest_addr("SwimTimeMeter");
    sSwimAnimationNames[11].mVtable = &sSwimStringVtable;
    sSwimAnimationNames[11].mText.v = gabi::guest_addr("SwimTimeOut");
    sSwimAnimationNames[12].mVtable = &sSwimStringVtable;
    sSwimAnimationNames[12].mText.v = gabi::guest_addr("HalfwayIn");
    sSwimCreateDescriptor.mVtable = &sSwimStringVtable;
    sSwimCreateDescriptor.mText.v = gabi::guest_addr("SwimTime_00");
    u32 number = 0;
    if (sSwimCounterReady) number = sSwimCounter;
    else sSwimCounterReady = 1;
    sSwimInitializedStates[0].setup(++number,"::StateID_Wait",&sSwimStateVtable,0x19,&sSwimNullState);
    sSwimInitializedStates[1].setup(++number,"::StateID_In",&sSwimStateVtable,0x1C,&sSwimNullState);
    sSwimInitializedStates[2].setup(++number,"::StateID_Disp",&sSwimStateVtable,0x1F,&sSwimNullState);
    sSwimInitializedStates[3].setup(++number,"::StateID_Out",&sSwimStateVtable,0x22,&sSwimNullState);
    sSwimInitializedStates[4].setup(++number,"::StateID_Dead",&sSwimStateVtable,0x25,&sSwimNullState);
    sSwimCounter = number;
}
/* WWHD glue (tools/verify/autoglue.py)
 str 1010727C "N_Light_00" in 026E728C
 str 10107288 "N_Light_01" in 026E728C
 str 10107294 "N_Light_02" in 026E728C
 str 101072A0 "N_Light_03" in 026E728C
 str 101072AC "N_Light_04" in 026E728C
 str 101072B8 "N_Light_05" in 026E728C
 str 10107178 "G_SwimTime_00" in 026E728C
 str 101071C8 "G_SwimTimeMeter_00" in 026E728C
 str 101071DC "G_SwimTimeColor_00" in 026E728C
 str 10107188 "G_SwimTimeSpec_00" in 026E728C
 str 101071F0 "G_Light_00" in 026E728C
 str 101071FC "G_Light_01" in 026E728C
 str 10107208 "G_Light_02" in 026E728C
 str 10107214 "G_Light_03" in 026E728C
 str 10107220 "G_Light_04" in 026E728C
 str 1010722C "G_Light_05" in 026E728C
 str 10107238 "SwimTimeIn" in 026E728C
 str 10107244 "SwimTimeSpec" in 026E728C
 str 10107254 "SwimTimeColor_00" in 026E728C
 str 10107268 "SwimTimeColor_01" in 026E728C
 str 10107158 "Light" in 026E728C
 str 10107158 "Light" in 026E728C
 str 10107158 "Light" in 026E728C
 str 10107158 "Light" in 026E728C
 str 10107158 "Light" in 026E728C
 str 10107158 "Light" in 026E728C
 str 1010719C "SwimTimeMeter" in 026E728C
 str 10107160 "SwimTimeOut" in 026E728C
 str 101071AC "HalfwayIn" in 026E728C
 str 1010716C "SwimTime_00" in 026E728C
 str 101072C4 "::StateID_Wait" in 026E728C
 str 101072D4 "::StateID_In" in 026E728C
 str 101072E4 "::StateID_Disp" in 026E728C
 str 101071B8 "::StateID_Out" in 026E728C
 str 101072F4 "::StateID_Dead" in 026E728C
 data sSwimStaticWords 1049C6BC
 data sSwimMinimumAngle 1049C6A0
 data sSwimMaximumAngle 1049C6A4
 data sSwimFirstLibraryStatic 1049C6B8
 data sSwimSecondLibraryStatic 1049C6B9
 data sSwimWordsRegistration 101F6F1C
 data sSwimFirstRegistration 101F6F28
 data sSwimSecondRegistration 101F6F34
 data sSwimStringVtable 1010702C
 data sSwimStateVtable 101070C0
 data sSwimNullState 101FF32C
 data sSwimInitializedStates 1049C7B4
 data sSwimCounterReady 101FD8E4
 data sSwimCounter 101FDD50
 data sSwimFarLimits 1049C6A8
 data sSwimNearLimits 1049C6B0
 ext 028F026C void hdSwim_registerGlobal(void*)
 ext 028ED6F8 void hdSwim_constructFirstStatic(void*)
 ext 028EAB2C void hdSwim_constructSecondStatic(void*)
 data sSwimPaneNames 1049C6CC
 data sSwimCreateDescriptor 1049C8A4
 ext 02700B48 u32 hdSwim_baseCreate(NoControlPart_l*,void*,u32,u32,u32)
 str 10107138 "SwimTimeBase_00" in 026E6F48
 ext 02876368 void hdSwim_getPaneRect(void*,hdSwimPaneRect_l*)
 data sSwimUiSystem 1018C404
 data sSwimGroupNames 1049C6FC
 data sSwimAnimationNames 1049C74C
 data sSwimGroupIndices 10107044
 ext 0273B050 void* hdSwim_allocateLayout(u32,hdScreenHeap*,u32)

 ext 02004E04 void hdSwim_registerAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
 data sSwimVtable 10107308
 data sSwimStateInterfaceVtable 10107448
 data sSwimDrawInterfaceVtable 10107458
 data sSwimAnimationModes 10107044
 data sSwimWaitState 1049C7B4
 data sSwimInState 1049C7E4
 data sSwimDisplayState 1049C814
 ext 025200D4 hdSwimPlay_l* hdSwim_getPlay()
 ext 025986BC u32 hdSwim_isEvent()
 ext 027172E4 u32 hdSwim_isBlocked()
 ext 02700A70 void hdSwim_baseDestroy(NoControlPart_l*,u32)
 ext 0273AF40 void hdSwim_delete(void*)
 ext 0273AD10 void* hdSwim_allocate(u32)
 ext 02700ADC void hdSwim_baseConstruct(NoControlPart_l*)

 ext 02005840 u32 hdSwim_animationEnded(hdAnimSet*,u32)
 ext 025E1988 void hdSwim_sound(u32)
*/

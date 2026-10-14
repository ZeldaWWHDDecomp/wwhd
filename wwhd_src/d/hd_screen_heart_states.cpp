/* HD HeartAll_00 companion: visibility, refill and special-life layout states.
 * The three merged refresh/refill functions remain immutable in hd_screen_heart.cpp. */
#include "d/hd_screen_heart_states.h"
#include "d/hd_screen_heart_part.h"
static u32 sHeartAllHeartScreenVtable;
static u32 sHeartAllHolderVtable;
static u32 sHeartAllInterfaceVtable;
static hdHeartModeView_l sHeartAllMode;
static hdPartsStateId_c sHeartAllBlank;
static hdPartsStateId_c sHeartAllIn;
static hdPartsStateId_c sHeartAllDisplay;
static hdPartsStateId_c sHeartAllSpecial;
void heartRefresh(hdHudHeart*);
void heartSelect(hdHudHeart*);

/* 0266D260 */
void hdHudHeartScreen_c::destroy(hdHudHeartScreen_c* screen, u32 flags) {
    if (!screen) return;
    screen->mStateHolder.mHolderVtable = &sHeartAllHolderVtable;
    screen->mSecondaryVtable = &sHeartAllInterfaceVtable;
    screen->NoControlPart_l::destroy(0);
    if (flags & 1) operator_delete(screen);
}
/* 0266D2CC */
void hdHudHeartScreen_c::blankEnter() { hdAnimSet_setFrame(animSet(), 0, 0, 0.0f); }
/* 0266D2E8 */
void hdHudHeartScreen_c::blankMove() { if (sHeartVisible) changeState(&sHeartAllIn); }
/* 0266D308 */
u32 hdHudHeartScreen_c::heartValue(u32 index) {
    u32 start = index * 4;
    u32 life = mShownLife;
    if (life < start) return 0;
    u32 amount = life - start;
    return amount <= 4 ? amount : 4;
}
/* 0266D3B4 */
void hdHudHeartScreen_c::updateValues() {
    if (sHeartAllMode.mMode == 6) {
        mShownLife = u32(sHeartAllMode.mLife) * 4;
        u32 containers = sHeartAllMode.mContainers;
        mSpecialMode = 1;
        mShownContainers = containers;
    } else {
        mShownLife = sSaveInfo->mStatusA.mLife;
        u32 containers = u32(sSaveInfo->mStatusA.mMaxLife) >> 2;
        mSpecialMode = 0;
        mShownContainers = containers;
    }
    heartRefresh(legacyView());
    for (u32 index = 0; index < 20; ++index) {
        hdHeartPart_c* part = heart(index);
        u32 containers = mShownContainers;
        u32 visible = 0;
        if (index < containers) {
            visible = 1;
            u32 requested = index < containers - 1 ? 2 : 1;
            part->mSkipTransition = 1;
            part->mRequestedState = requested;
        }
        part->setVisible(visible);
    }
}
/* 0266D4D4 */
void hdHudHeartScreen_c::inEnter() { hdAnimSet_play(animSet(),0,0,1.0f); updateValues(); }
/* 0266D520 */
void hdHudHeartScreen_c::inMove() {
    if (hdAnimSet_isEnd(animSet(),0)) changeState(sHeartAllMode.mMode == 6 ? &sHeartAllSpecial : &sHeartAllDisplay);
}
/* 0266D77C */
void hdHudHeartScreen_c::outEnter() { hdAnimSet_play(animSet(),1,0,1.0f); }
/* 0266D798 */
void hdHudHeartScreen_c::outMove() { if (hdAnimSet_isEnd(animSet(),1)) changeState(&sHeartAllBlank); }
/* 0266D7E8 */
void hdHudHeartScreen_c::hideEnter() {
    hdAnimSet_play(animSet(),0,0,1.0f);
    for (u32 index=0; index<20; ++index) {
        hdHeartPart_c* part=heart(index);
        part->mRequestedState=2;
        part->setVisible(0);
    }
    mFreezeValues=0;
    mHidden=1;
    mShownContainers=0;
    mShownLife=0;
}
/* 0266D89C */
void hdHudHeartScreen_c::hideMove() {
    if (mFreezeValues) return;
    hdHudSaveInfo* save=sSaveInfo;
    u32 containers=u32(save->mStatusA.mMaxLife)>>2;
    if (mShownContainers != containers) {
        mShownContainers=containers;
        for(u32 index=0;index<20;++index) {
            hdHeartPart_c* part=heart(index);
            part->setVisible(index<containers ? 1 : 0);
            if(index!=19) containers=mShownContainers;
        }
        save=sSaveInfo;
    }
    u32 life=save->mStatusA.mLife;
    if(mShownLife!=life) { mShownLife=life; heartRefresh(legacyView()); }
}
/* 0266D974 */
void hdHudHeartScreen_c::specialMove() {
    u32 life=u32(sHeartAllMode.mLife)*4;
    if(mShownLife!=life) { mShownLife=life;heartRefresh(legacyView());heartSelect(legacyView()); }
    if(!sHeartAllMode.mVisible) changeState(&sHeartHiddenState);
}
/* 0266D9F0 */
hdHudHeartScreen_c* hdHudHeartScreen_c::construct(hdHudHeartScreen_c* screen) {
    if(!screen) { screen=static_cast<hdHudHeartScreen_c*>(operator_new(sizeof(hdHudHeartScreen_c)));if(!screen)return NULL; }
    NoControlPart_l::construct(screen);
    screen->mVtable.v=gabi::ea(&sHeartAllHeartScreenVtable);
    screen->mSecondaryVtable=&sHeartAllInterfaceVtable;
    screen->mStateHolder.mHolderVtable=&sHeartAllHolderVtable;
    // HD: GHS still invokes the array allocation path for a wrapped NULL member address.
    gptr<hdPartsPane_c>* hearts=gabi::at<gptr<hdPartsPane_c>>(gabi::ea(screen)+u32(offsetof(hdHudHeartScreen_c,mpHeartPanes)));
    if(!hearts) operator_new(0x50);
    gptr<hdPartsPane_c>* loops=gabi::at<gptr<hdPartsPane_c>>(gabi::ea(screen)+u32(offsetof(hdHudHeartScreen_c,mpLoopPanes)));
    if(!loops) operator_new(0x50);
    screen->mSpecialMode=0;
    screen->mSelectedHeart=0xFFFFFFFFu;
    screen->mFreezeValues=0;
    screen->mHidden=0;
    screen->mShownLife=0;
    screen->mShownContainers=0;
    screen->mRefillTimer=0;
    for(u32 index=0;index<20;++index) { hearts[index]=NULL; loops[index]=NULL; }
    return screen;
}
/* 0266DC58 */
void hdHudHeartScreen_c::destroyContents() { vDeleteChildren();vDestroyLayout(); }
/* 0266DCA4 */
void hdHudHeartScreen_c::execute() { hdParts_executeStateMachine(stateMachine());vExecuteChildren(); }
/* 0266DCE8 */
void hdHudHeartScreen_c::drawChildren() { gabi::call_ptr(mVtable->mDrawChildren.get(),this); }
/* 0266DCF8 */
void hdHudHeartScreen_c::reset() { changeState(&sHeartAllBlank); }
struct hdHeartAllNames_c { hdUiSafeString mNames[20]; };
struct hdHeartAllGroups_c { hdUiSafeString mAll; hdUiSafeString mUnused; };
struct hdHeartAllAnimations_c { hdUiSafeString mIn; hdUiSafeString mOut; };
struct hdHeartAllSlots_c { be<u32> mSlots[2]; };
struct hdHeartAllDesc_l { u8 mPrefix[8]; hdUiSafeString mLayout; u8 mGap[4]; hdUiSafeString mPane; };
WWHD_OFFSET(hdHeartAllDesc_l,mPane,0x14);
static hdHeartAllNames_c sHeartAllPaneNames;
static hdHeartAllNames_c sHeartAllLoopNames;
static hdHeartAllGroups_c sHeartAllGroups;
static hdHeartAllAnimations_c sHeartAllAnimations;
static hdHeartAllSlots_c sHeartAllSlots;
static hdUiSafeString sHeartAllName;
static hdUiSafeString sHeartAllPartName;
static hdScreenUiSystem* sHeartAllUi;
static hdHeartInitLists_l sHeartAllInitLists;
static hdHeartInitAngles_l sHeartAllInitAngles;
static hdHeartAngleLimits_l sHeartAllAngleLimits;
static hdHeartInitRecord_l sHeartAllFirstRecord;
static hdHeartInitRecord_l sHeartAllSecondRecord;
static hdHeartInitRecord_l sHeartAllThirdRecord;
static u32 sHeartAllStringVtable;
static hdPartsStateIdVtbl_c sHeartAllStateVtable;
static hdStateId sHeartAllStateContext;
static u32 sHeartAllStateGuard;
static u32 sHeartAllStateCounter;
static hdPartsStateId_c sHeartAllHide;
void heartAllBind(void*,u32,hdUiSafeString*,hdUiSafeString*);
void heartAllRegister(hdHeartInitRecord_l*);
void heartAllInitFlag(be<u8>*);
void heartAllInitByte(be<u8>*);

/* inlined into createHeart */
static BOOL heartAllNameMatches(hdUiSafeString* name,hdUiSafeString* expected) {
    name->assureTermination();
    name->assureTermination();
    be<u8>* first=name->mText;
    expected->assureTermination();
    if(first==expected->mText.get())return TRUE;
    be<u8>* a=name->mText;
    be<u8>* b=expected->mText;
    for(u32 index=0;index<0x40001;++index) {
        u32 ca=*gabi::at<be<u8>>(gabi::ea(a)+index);
        u32 cb=*gabi::at<be<u8>>(gabi::ea(b)+index);
        if(ca!=cb)return FALSE;
        if(!ca)return TRUE;
    }
    return FALSE;
}
/* 0266DB04 */
BOOL hdHudHeartScreen_c::init(void* resource,u32 option,void* parent) {
    if(!hdPartsNoControl_init(this,&sHeartAllName,gabi::ea(resource),option,static_cast<hdPartsDesc_c*>(parent)))return FALSE;
    hdPartsPaneSRT_c* root=reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    for(u32 index=0;index<20;++index) {
        hdUiSafeString* name=&sHeartAllPaneNames.mNames[index];
        hdPartsPaneSRTVtbl_c* vtable=root->srtVtbl();
        name->assureTermination();
        mpHeartPanes[index]=gabi::call_ptr<hdPartsPane_c*>(vtable->mFindPane.get(),root,name->mText.get(),1);
    }
    startState(&sHeartAllBlank);
    return TRUE;
}
/* 0266DD08 */
BOOL hdHudHeartScreen_c::createHeart(void* descriptor) {
    hdHeartAllDesc_l* desc=static_cast<hdHeartAllDesc_l*>(descriptor);
    hdScreenHeap* heap=sHeartAllUi->mHeap;
    for(u32 index=0;index<20;++index) {
        if(!heartAllNameMatches(&desc->mPane,&sHeartAllPaneNames.mNames[index]))continue;
        if(!heartAllNameMatches(&desc->mLayout,&sHeartAllPartName))return hdParts_c::createChild(index,static_cast<hdPartsDesc_c*>(descriptor));
        gptr<hdParts_c>* slot=childSlot(index);
        hdHudHeartPart_c* part=static_cast<hdHudHeartPart_c*>(hdParts_new(sizeof(hdHudHeartPart_c),heap,4));
        if(part)part=hdHudHeartPart_c::construct(part);
        *slot=part;
        if(!child(index))return FALSE;
        return static_cast<hdHudHeartPart_c*>(child(index))->loadScreen(NULL,NULL,descriptor);
    }
    return TRUE;
}
/* 0266DF60 */
BOOL hdHudHeartScreen_c::loadLayout(void* name,u32 accessor,u32 option,u32 parent,void* desc) {
    hdPartsLayout_c* layout=static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c),sHeartAllUi->mHeap,4));
    if(layout)layout=hdPartsLayout_c::construct(layout);
    mpLayout=layout;
    if(!layout||!layout->vLoad(name,accessor,option,parent,2,1,desc))return FALSE;
    for(u32 index=0;index<2;++index) {
        u32 group=sHeartAllSlots.mSlots[index];
        hdUiSafeString* animation=gabi::at<hdUiSafeString>(gabi::ea(&sHeartAllAnimations)+index*8);
        hdUiSafeString* pane=gabi::at<hdUiSafeString>(gabi::ea(&sHeartAllGroups)+group*8);
        heartAllBind(animSet(),index,animation,pane);
    }
    return TRUE;
}
/* 0266E078 */
static void initHudHeartAll() {
    sHeartAllInitLists.mThird=0;sHeartAllInitLists.mFirst=0;
    sHeartAllInitLists.mFourth=0;sHeartAllInitLists.mSecond=0;
    heartAllRegister(&sHeartAllFirstRecord);
    f32 minimum=sHeartAllAngleLimits.mMinimum;
    f32 maximum=sHeartAllAngleLimits.mMaximum;
    sHeartAllInitAngles.mMinimum=minimum;sHeartAllInitAngles.mMaximum=maximum;
    heartAllInitFlag(&sHeartAllInitAngles.mFlag);
    heartAllRegister(&sHeartAllSecondRecord);
    heartAllInitByte(&sHeartAllInitAngles.mByte);
    heartAllRegister(&sHeartAllThirdRecord);
    sHeartAllAnimations.mIn.mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllAnimations.mIn.mText.v=gabi::guest_addr("InAll");
    sHeartAllAnimations.mOut.mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllAnimations.mOut.mText.v=gabi::guest_addr("OutAll");
    sHeartAllGroups.mAll.mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllGroups.mAll.mText.v=gabi::guest_addr("G_All_00");
    sHeartAllPaneNames.mNames[0].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[0].mText.v=gabi::guest_addr("L_Heart_00");
    sHeartAllLoopNames.mNames[0].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[0].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[1].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[1].mText.v=gabi::guest_addr("L_Heart_01");
    sHeartAllLoopNames.mNames[1].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[1].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[2].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[2].mText.v=gabi::guest_addr("L_Heart_02");
    sHeartAllLoopNames.mNames[2].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[2].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[3].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[3].mText.v=gabi::guest_addr("L_Heart_03");
    sHeartAllLoopNames.mNames[3].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[3].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[4].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[4].mText.v=gabi::guest_addr("L_Heart_04");
    sHeartAllLoopNames.mNames[4].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[4].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[5].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[5].mText.v=gabi::guest_addr("L_Heart_05");
    sHeartAllLoopNames.mNames[5].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[5].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[6].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[6].mText.v=gabi::guest_addr("L_Heart_06");
    sHeartAllLoopNames.mNames[6].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[6].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[7].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[7].mText.v=gabi::guest_addr("L_Heart_07");
    sHeartAllLoopNames.mNames[7].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[7].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[8].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[8].mText.v=gabi::guest_addr("L_Heart_08");
    sHeartAllLoopNames.mNames[8].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[8].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[9].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[9].mText.v=gabi::guest_addr("L_Heart_09");
    sHeartAllLoopNames.mNames[9].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[9].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[10].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[10].mText.v=gabi::guest_addr("L_Heart_10");
    sHeartAllLoopNames.mNames[10].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[10].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[11].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[11].mText.v=gabi::guest_addr("L_Heart_11");
    sHeartAllLoopNames.mNames[11].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[11].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[12].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[12].mText.v=gabi::guest_addr("L_Heart_12");
    sHeartAllLoopNames.mNames[12].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[12].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[13].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[13].mText.v=gabi::guest_addr("L_Heart_13");
    sHeartAllLoopNames.mNames[13].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[13].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[14].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[14].mText.v=gabi::guest_addr("L_Heart_14");
    sHeartAllLoopNames.mNames[14].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[14].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[15].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[15].mText.v=gabi::guest_addr("L_Heart_15");
    sHeartAllLoopNames.mNames[15].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[15].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[16].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[16].mText.v=gabi::guest_addr("L_Heart_16");
    sHeartAllLoopNames.mNames[16].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[16].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[17].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[17].mText.v=gabi::guest_addr("L_Heart_17");
    sHeartAllLoopNames.mNames[17].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[17].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[18].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[18].mText.v=gabi::guest_addr("L_Heart_18");
    sHeartAllLoopNames.mNames[18].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[18].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllPaneNames.mNames[19].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllPaneNames.mNames[19].mText.v=gabi::guest_addr("L_Heart_19");
    sHeartAllLoopNames.mNames[19].mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllLoopNames.mNames[19].mText.v=gabi::guest_addr("Heart_00");
    sHeartAllName.mVtable.v=gabi::ea(&sHeartAllStringVtable);
    sHeartAllName.mText.v=gabi::guest_addr("HeartAll_00");
    u32 number;
    if(sHeartAllStateGuard)number=sHeartAllStateCounter;
    else {sHeartAllStateGuard=1;number=0;}
    hdPartsStateIdVtbl_c* vtable=&sHeartAllStateVtable;
    sHeartAllBlank.setup(++number,"::StateID_Wait",vtable,25,&sHeartAllStateContext);
    sHeartAllIn.setup(++number,"::StateID_In",vtable,28,&sHeartAllStateContext);
    sHeartAllDisplay.setup(++number,"::StateID_Disp",vtable,31,&sHeartAllStateContext);
    sHeartHiddenState.setup(++number,"::StateID_Out",vtable,34,&sHeartAllStateContext);
    sHeartAllHide.setup(++number,"::StateID_FileSelect",vtable,37,&sHeartAllStateContext);
    sHeartAllSpecial.setup(++number,"::StateID_Battle",vtable,40,&sHeartAllStateContext);
    sHeartAllStateCounter=number;
}
/* WWHD glue
 ext 020063C0 void hdHudChangeState(hdHudStateMachine*,hdHudStateDescriptor*)
 data sHeartAllPaneNames 10494624
 data sHeartAllLoopNames 104946C4
 data sHeartAllGroups 104945F0
 data sHeartAllAnimations 10494614
 data sHeartAllSlots 100F55C0
 data sHeartAllName 10494884
 data sHeartAllPartName 104945E8
 data sHeartAllUi 1018C404
 data sHeartAllInitLists 10494604
 data sHeartAllInitAngles 104945F8
 data sHeartAllAngleLimits 100F5680
 data sHeartAllFirstRecord 101F5FD4
 data sHeartAllSecondRecord 101F5FE0
 data sHeartAllThirdRecord 101F5FEC
 data sHeartAllStringVtable 100F55CC
 data sHeartAllStateVtable 100F562C
 data sHeartAllStateContext 101FF32C
 data sHeartAllStateGuard 101FD8E4
 data sHeartAllStateCounter 101FDD50
 data sHeartAllHide 10494824
 str 100F5688 "InAll" in 0266E078
 str 100F5690 "OutAll" in 0266E078
 str 100F5698 "HeartAll_00" in 0266E078
 str 100F56A4 "::StateID_Out" in 0266E078
 str 100F56B4 "G_All_00" in 0266E078
 str 100F57B0 "Heart_00" in 0266E078
 str 100F57BC "::StateID_Wait" in 0266E078
 str 100F57CC "::StateID_In" in 0266E078
 str 100F57DC "::StateID_Disp" in 0266E078
 str 100F57EC "::StateID_FileSelect" in 0266E078
 str 100F5804 "::StateID_Battle" in 0266E078
 str 100F56C0 "L_Heart_00" in 0266E078
 str 100F56CC "L_Heart_01" in 0266E078
 str 100F56D8 "L_Heart_02" in 0266E078
 str 100F56E4 "L_Heart_03" in 0266E078
 str 100F56F0 "L_Heart_04" in 0266E078
 str 100F56FC "L_Heart_05" in 0266E078
 str 100F5708 "L_Heart_06" in 0266E078
 str 100F5714 "L_Heart_07" in 0266E078
 str 100F5720 "L_Heart_08" in 0266E078
 str 100F572C "L_Heart_09" in 0266E078
 str 100F5738 "L_Heart_10" in 0266E078
 str 100F5744 "L_Heart_11" in 0266E078
 str 100F5750 "L_Heart_12" in 0266E078
 str 100F575C "L_Heart_13" in 0266E078
 str 100F5768 "L_Heart_14" in 0266E078
 str 100F5774 "L_Heart_15" in 0266E078
 str 100F5780 "L_Heart_16" in 0266E078
 str 100F578C "L_Heart_17" in 0266E078
 str 100F5798 "L_Heart_18" in 0266E078
 str 100F57A4 "L_Heart_19" in 0266E078
 ext 02004E04 void heartAllBind(void*,u32,hdUiSafeString*,hdUiSafeString*)
 ext 028F026C void heartAllRegister(hdHeartInitRecord_l*)
 ext 028ED6F8 void heartAllInitFlag(be<u8>*)
 ext 028EAB2C void heartAllInitByte(be<u8>*)

 data sHeartAllHeartScreenVtable 100F5818
 data sHeartAllHolderVtable 100F5970
 data sHeartAllInterfaceVtable 100F5980
 data sHeartAllMode 1047B03C
 data sHeartAllBlank 10494764
 data sHeartAllIn 10494794
 data sHeartAllDisplay 104947C4
 data sHeartAllSpecial 10494854
 ext 0266D334 void heartRefresh(hdHudHeart*)
 ext 0266D588 void heartSelect(hdHudHeart*)
 ext 026F711C void hdHeartPart_c::setVisible(u32)
 ext 02700A70 void NoControlPart_l::destroy(u32)
 ext 02700ADC NoControlPart_l* NoControlPart_l::construct(NoControlPart_l*)
*/

/* HD Information_00: InAll, LoopAll and OutAll animations. */
#include "d/hd_screen_information.h"
static u32 sInformationVtable;
static u32 sInformationHolderVtable;
static u32 sInformationInterfaceVtable;
static u32 sInformationStringVtable;
static hdPartsStateIdVtbl_c sInformationStateVtable;
static hdPartsStateId_c sInformationIn;
static hdPartsStateId_c sInformationLoop;
static hdPartsStateId_c sInformationOut;
static hdPartsStateId_c sInformationBlank;
static hdStateId sInformationStateContext;
static u32 sInformationStateGuard;
static u32 sInformationStateCounter;
static hdHeartInitLists_l sInformationInitLists;
static hdHeartInitAngles_l sInformationInitAngles;
static hdHeartAngleLimits_l sInformationAngleLimits;
static hdHeartInitRecord_l sInformationFirstRecord;
static hdHeartInitRecord_l sInformationSecondRecord;
static hdHeartInitRecord_l sInformationThirdRecord;
struct hdInformationAnimations_c { hdUiSafeString mIn,mLoop,mOut; };
struct hdInformationGroups_c { hdUiSafeString mAll,mLoop; };
struct hdInformationSlots_c { be<u32> mSlots[3]; };
static hdInformationAnimations_c sInformationAnimations;
static hdInformationGroups_c sInformationGroups;
static hdInformationSlots_c sInformationSlots;
static hdUiSafeString sInformationName;
static hdUiSafeString sInformationArchive;
static hdScreenUiSystem* sInformationUi;
void informationBindAnimation(void*,u32,hdUiSafeString*,hdUiSafeString*);
void informationRegister(hdHeartInitRecord_l*);
void informationInitFlag(be<u8>*);
void informationInitByte(be<u8>*);

/* 02671AF4 */
u32 hdInformationStateNumber_c::getNumber() { return mNumber; }
/* 02671BF4 */
void hdHudInformation_c::destroy(hdHudInformation_c* screen,u32 flags) {
    if(!screen)return;
    screen->mStateHolder.mHolderVtable=&sInformationHolderVtable;
    screen->mSecondaryVtable=&sInformationInterfaceVtable;
    hdParts_c::destroy(screen,0);
    if(flags&1)operator_delete(screen);
}
/* 02671C60 */
void hdHudInformation_c::inEnter() { hdAnimSet_play(animSet(),0,0,1.0f); }
/* 02671C7C */
void hdHudInformation_c::inMove() { if(hdAnimSet_isEnd(animSet(),0))changeState(&sInformationLoop);vCalcLayout(); }
/* 02671CE0 */
void hdHudInformation_c::loopEnter() { hdAnimSet_play(animSet(),1,1,1.0f); }
/* 02671CFC */
void hdHudInformation_c::loopMove() { vCalcLayout(); }
/* 02671D0C */
void hdHudInformation_c::outEnter() { hdAnimSet_play(animSet(),2,0,1.0f); }
/* 02671D28 */
void hdHudInformation_c::outMove() { if(hdAnimSet_isEnd(animSet(),2))changeState(&sInformationBlank);vCalcLayout(); }
/* 02671D8C */
hdHudInformation_c* hdHudInformation_c::construct(hdHudInformation_c* screen) {
    if(!screen) { screen=static_cast<hdHudInformation_c*>(operator_new(sizeof(hdHudInformation_c)));if(!screen)return NULL; }
    hdParts_c::construct(screen);
    screen->mVtable.v=gabi::ea(&sInformationVtable);
    screen->mStateHolder.mHolderVtable=&sInformationHolderVtable;
    screen->mSecondaryVtable=&sInformationInterfaceVtable;
    return screen;
}
/* 02671DF8 */
BOOL hdHudInformation_c::init(void* resource,u32 option,u32 parent) {
    attachArchives(&sInformationArchive,1);
    if(!gabi::call_ptr<BOOL>(mVtable->mLoad.get(),this,&sInformationName,mpResAccessor.get(),resource,option,parent))return FALSE;
    hdAnimSet_operationCC(animSet(),0,0);
    vCalcLayout();
    startState(&sInformationBlank);
    return TRUE;
}
/* 02671F08 */
void hdHudInformation_c::finalize() { vDestroyLayout();detachArchives(); }
/* 02671F48 */
void hdHudInformation_c::execute() { hdParts_executeStateMachine(stateMachine()); }
/* 02671F50 */
BOOL hdHudInformation_c::loadLayout(void* name,u32 accessor,u32 option,u32 parent,void* desc) {
    hdPartsLayout_c* layout=static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c),sInformationUi->mHeap,4));
    if(layout)layout=hdPartsLayout_c::construct(layout);
    mpLayout=layout;
    if(!layout||!layout->vLoad(name,accessor,option,parent,3,2,desc))return FALSE;
    for(u32 index=0;index<3;++index) {
        u32 group=sInformationSlots.mSlots[index];
        hdUiSafeString* animation=gabi::at<hdUiSafeString>(gabi::ea(&sInformationAnimations)+index*8);
        hdUiSafeString* pane=gabi::at<hdUiSafeString>(gabi::ea(&sInformationGroups)+group*8);
        informationBindAnimation(animSet(),index,animation,pane);
    }
    vCalcLayout();
    return TRUE;
}
/* 0267207C */
static void initHudInformation() {
    sInformationInitLists.mFourth=0;sInformationInitLists.mThird=0;
    sInformationInitLists.mSecond=0;sInformationInitLists.mFirst=0;
    informationRegister(&sInformationFirstRecord);
    f32 minimum=sInformationAngleLimits.mMinimum;
    f32 maximum=sInformationAngleLimits.mMaximum;
    sInformationInitAngles.mMinimum=minimum;sInformationInitAngles.mMaximum=maximum;
    informationInitFlag(&sInformationInitAngles.mFlag);
    informationRegister(&sInformationSecondRecord);
    informationInitByte(&sInformationInitAngles.mByte);
    informationRegister(&sInformationThirdRecord);
    sInformationGroups.mAll.mVtable.v=gabi::ea(&sInformationStringVtable);
    sInformationGroups.mAll.mText.v=gabi::guest_addr("G_All_00");
    sInformationAnimations.mIn.mVtable.v=gabi::ea(&sInformationStringVtable);
    sInformationAnimations.mIn.mText.v=gabi::guest_addr("InAll");
    sInformationAnimations.mLoop.mVtable.v=gabi::ea(&sInformationStringVtable);
    sInformationAnimations.mLoop.mText.v=gabi::guest_addr("LoopAll");
    sInformationAnimations.mOut.mVtable.v=gabi::ea(&sInformationStringVtable);
    sInformationAnimations.mOut.mText.v=gabi::guest_addr("OutAll");
    sInformationGroups.mLoop.mVtable.v=gabi::ea(&sInformationStringVtable);
    sInformationName.mVtable.v=gabi::ea(&sInformationStringVtable);
    sInformationName.mText.v=gabi::guest_addr("Information_00");
    sInformationArchive.mVtable.v=gabi::ea(&sInformationStringVtable);
    sInformationGroups.mLoop.mText.v=gabi::guest_addr("G_LoopAll_00");
    sInformationArchive.mText.v=gabi::guest_addr("Information_00");
    u32 number;
    if(sInformationStateGuard)number=sInformationStateCounter;
    else { sInformationStateGuard=1;number=0; }
    hdPartsStateIdVtbl_c* vtable=&sInformationStateVtable;
    sInformationIn.setup(++number,"::StateID_InAll",vtable,25,&sInformationStateContext);
    sInformationLoop.setup(++number,"::StateID_LoopAll",vtable,28,&sInformationStateContext);
    sInformationOut.setup(++number,"::StateID_OutAll",vtable,31,&sInformationStateContext);
    sInformationStateCounter=number;
}
/* WWHD glue
 data sInformationVtable 100F6580
 data sInformationHolderVtable 100F6690
 data sInformationInterfaceVtable 100F66A0
 data sInformationStringVtable 100F6444
 data sInformationStateVtable 100F64B0
 data sInformationIn 10494D40
 data sInformationLoop 10494D70
 data sInformationOut 10494DA0
 data sInformationBlank 1049DD60
 data sInformationStateContext 101FF32C
 data sInformationStateGuard 101FD8E4
 data sInformationStateCounter 101FDD50
 data sInformationInitLists 10494D08
 data sInformationInitAngles 10494CFC
 data sInformationAngleLimits 100F64FC
 data sInformationFirstRecord 101F6088
 data sInformationSecondRecord 101F6094
 data sInformationThirdRecord 101F60A0
 data sInformationAnimations 10494D28
 data sInformationGroups 10494D18
 data sInformationSlots 100F645C
 data sInformationName 10494DD0
 data sInformationArchive 10494CF4
 data sInformationUi 1018C404
 str 100F6540 "G_All_00" in 0267207C
 str 100F650C "InAll" in 0267207C
 str 100F6504 "LoopAll" in 0267207C
 str 100F6514 "OutAll" in 0267207C
 str 100F655C "Information_00" in 0267207C
 str 100F654C "G_LoopAll_00" in 0267207C
 str 100F651C "::StateID_InAll" in 0267207C
 str 100F652C "::StateID_LoopAll" in 0267207C
 str 100F656C "::StateID_OutAll" in 0267207C
 ext 02004E04 void informationBindAnimation(void*,u32,hdUiSafeString*,hdUiSafeString*)
 ext 028F026C void informationRegister(hdHeartInitRecord_l*)
 ext 028ED6F8 void informationInitFlag(be<u8>*)
 ext 028EAB2C void informationInitByte(be<u8>*)
*/

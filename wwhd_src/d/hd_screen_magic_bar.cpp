/* HD MagicBar_00. */
#include "d/hd_screen_magic_bar.h"
#include "d/d_meter_00.h"
#include "d/hd_hud_b.h"
#include "d/hd_msg_window_base.h"
static u32 magicFloatBits(f32 value) { u32 bits;memcpy(&bits,&value,4);return bits; }
static u32 sMagicVtable;
static u32 sMagicHolderVtable;
static u32 sMagicInterfaceVtable;
static f32 sMagicZero;
static f32 sMagicOne;
static f32 sMagicUnset;
static u8 sMagicVisible;
static u8 sMagicWarning;
struct hdMagicAnimationSlots_c { be<u32> mSlots[7]; };
static hdMagicAnimationSlots_c sMagicAnimationSlots;
struct hdMagicPlacement_l { u8 _00[0xC]; be<f32> mX,mY; u8 _14[0x1C]; be<f32> mBlend,mRemaining; };
static hdMagicPlacement_l sMagicPlacement;
static hdPartsStateId_c sMagicBlank;
static hdPartsStateId_c sMagicIn;
static hdPartsStateId_c sMagicDisplay;
static hdPartsStateId_c sMagicOut;
void hdMsgProgress_updateProgress(WindowProgress*);
dMeterGameInfoHD* magicGetPlay();
/* 02674C84 */
u32 hdMagicStateNumber_c::getNumber() { return mNumber; }
/* 02674D84 */
void hdHudMagicBar_c::destroy(hdHudMagicBar_c* screen,u32 flags) {
    if(!screen)return;
    screen->mStateHolder.mHolderVtable=&sMagicHolderVtable;
    screen->mSecondaryVtable=&sMagicInterfaceVtable;
    screen->ProgressBarPart_l::destroy(0);
    if(flags&1)operator_delete(screen);
}
/* 02674DF0 */
void hdHudMagicBar_c::setFrame(u32 animation,f32 frame) {
    u32 slot=sMagicAnimationSlots.mSlots[animation];
    hdAnimSet_setFrame(animSet(),animation,slot,frame);
}
/* 02674E0C */
void hdHudMagicBar_c::blankEnter() { setFrame(2,sMagicZero); }
/* 02674E1C */
void hdHudMagicBar_c::blankMove() { if(sMagicVisible)changeState(&sMagicIn); }
/* 02674E3C */
void hdHudMagicBar_c::play(u32 animation) {
    f32 rate=sMagicOne;
    u32 slot=sMagicAnimationSlots.mSlots[animation];
    hdAnimSet_play(animSet(),animation,slot,rate);
}
/* 02674E60 */
void hdHudMagicBar_c::inEnter() {
    loadProgress();play(2);mPlayerFrame=sMagicZero;
    u32 secondary=magicGetPlay()->mpCb1Player.v;
    if(secondary!=reinterpret_cast<HdHudPlay*>(magicGetPlay())->mActor.v)setFrame(4,mPlayerFrame);
    else { f32 frame=sMagicOne;mPlayerFrame=frame;setFrame(4,frame); }
}
/* 02674EF0 */
void hdHudMagicBar_c::updatePlacement() {
    f32 placement=sMagicZero,player=placement;
    u32 secondary=magicGetPlay()->mpCb1Player.v;
    if(secondary==reinterpret_cast<HdHudPlay*>(magicGetPlay())->mActor.v) {
        f32 originX=mOrigin[0],translationZ=gabi::f32_from_bits(u32(mOriginalTranslation[2]));
        player=sMagicOne;
        f32 originY=mOrigin[1];mSpecialPlacement=1;
        placement=gabi::fsubs_ppc(player,sMagicPlacement.mRemaining);
        f32 deltaY=gabi::fsubs_ppc(sMagicPlacement.mY,originY);
        f32 deltaX=gabi::fsubs_ppc(sMagicPlacement.mX,originX);
        f32 blend=sMagicPlacement.mBlend;
        f32 translationY=gabi::f32_from_bits(u32(mOriginalTranslation[1]));
        f32 spacing=mPaneSpacing;
        hdPartsPaneSRT_c* pane=mPanes[0];
        deltaX=gabi::fmadds(spacing,placement,deltaX);
        f32 translationX=gabi::f32_from_bits(u32(mOriginalTranslation[0]));
        f32 y=gabi::fmadds(deltaY,blend,translationY);
        u8 flags=pane->mFlags;
        f32 x=gabi::fmadds(deltaX,blend,translationX);
        pane->mTranslationYBits=magicFloatBits(y);
        pane->mTranslationXBits=magicFloatBits(x);
        pane->mFlags=flags|0x10;
        pane->mTranslationZBits=magicFloatBits(translationZ);
    } else if(mSpecialPlacement) {
        hdPartsPaneSRT_c* pane=mPanes[0];
        u32 x=mOriginalTranslation[0];mSpecialPlacement=0;
        pane->mTranslationXBits=x;
        u32 y=mOriginalTranslation[1];u8 flags=pane->mFlags;
        pane->mTranslationYBits=y;
        u32 z=mOriginalTranslation[2];pane->mFlags=flags|0x10;pane->mTranslationZBits=z;
    }
    if(f32(mPlacementFrame)!=placement) { mPlacementFrame=placement;setFrame(5,placement); }
    if(f32(mPlayerFrame)!=player) { mPlayerFrame=player;setFrame(4,player);loadProgress(); }
}
/* 02675074 */
void hdHudMagicBar_c::inMove() { updatePlacement();if(hdAnimSet_isEnd(animSet(),2))changeState(&sMagicDisplay); }
/* 026750C8 */
void hdHudMagicBar_c::displayMove() { updatePlacement();if(!sMagicVisible)changeState(&sMagicOut); }
/* 02675114 */
void hdHudMagicBar_c::outEnter() { play(6); }
/* 0267511C */
void hdHudMagicBar_c::outMove() { if(hdAnimSet_isEnd(animSet(),6))changeState(&sMagicBlank); }
/* 0267516C */
hdHudMagicBar_c* hdHudMagicBar_c::construct(hdHudMagicBar_c* screen) {
    if(!screen) { screen=static_cast<hdHudMagicBar_c*>(operator_new(sizeof(hdHudMagicBar_c)));if(!screen)return NULL; }
    ProgressBarPart_l::construct(screen);
    screen->mVtable.v=gabi::ea(&sMagicVtable);
    screen->mSecondaryVtable=&sMagicInterfaceVtable;
    screen->mStateHolder.mHolderVtable=&sMagicHolderVtable;
    gptr<hdPartsPaneSRT_c>* panes=gabi::at<gptr<hdPartsPaneSRT_c>>(gabi::ea(screen)+u32(offsetof(hdHudMagicBar_c,mPanes)));
    if(!panes)operator_new(12);
    be<f32>* origin=gabi::at<be<f32>>(gabi::ea(screen)+u32(offsetof(hdHudMagicBar_c,mOrigin)));
    f32 zero=sMagicZero;
    if(!origin)origin=static_cast<be<f32>*>(operator_new(12));
    if(origin) { origin[1]=zero;origin[0]=zero;origin[2]=zero; }
    screen->mPlayerFrame=zero;screen->mOriginalTranslation[2]=magicFloatBits(zero);
    screen->mOriginalTranslation[0]=magicFloatBits(zero);
    f32 unset=sMagicUnset;screen->mWarning=0;screen->mPlacementFrame=unset;screen->mSpecialPlacement=0;
    screen->mOriginalTranslation[1]=magicFloatBits(zero);screen->mPaneSpacing=zero;
    for(u32 i=0;i<3;++i)panes[i]=NULL;
    return screen;
}
/* 02675414 */
void hdHudMagicBar_c::execute() {
    hdMsgProgress_updateProgress(reinterpret_cast<WindowProgress*>(this));
    u8 warning=sMagicWarning;u8 previous=mWarning;
    if(warning) { if(!previous) { mWarning=1;play(3); } }
    else if(previous) { f32 zero=sMagicZero;mWarning=0;setFrame(3,zero); }
    hdParts_executeStateMachine(stateMachine());
}
/* 026754A8 */
void hdHudMagicBar_c::reset() { ProgressBarPart_l::reset();mPlacementFrame=sMagicUnset;changeState(&sMagicBlank); }
static hdScreenUiSystem* sMagicUi;
static u32 sMagicStringVtable;
static hdPartsStateIdVtbl_c sMagicStateVtable;
static u32 sMagicStateGuard;
static u32 sMagicStateCounter;
static hdStateId sMagicStateContext;
static hdHeartInitLists_l sMagicInitLists;
static hdHeartInitAngles_l sMagicInitAngles;
static hdHeartAngleLimits_l sMagicAngleLimits;
static hdHeartInitRecord_l sMagicFirstRecord;
static hdHeartInitRecord_l sMagicSecondRecord;
static hdHeartInitRecord_l sMagicThirdRecord;
struct hdMagicGroups_c { hdUiSafeString mNames[4]; };
struct hdMagicAnimations_c { hdUiSafeString mNames[5]; };
struct hdMagicPaneNames_c { hdUiSafeString mNames[3]; };
struct hdMagicBindSlots_c { be<u32> mSlots[5]; };
static hdMagicGroups_c sMagicGroups;
static hdMagicAnimations_c sMagicAnimations;
static hdMagicPaneNames_c sMagicPaneNames;
static hdMagicBindSlots_c sMagicBindSlots;
static hdUiSafeString sMagicName;
void magicBindAnimation(void*,u32,hdUiSafeString*,hdUiSafeString*);
void magicRegister(hdHeartInitRecord_l*);
void magicInitFlag(be<u8>*);
void magicInitByte(be<u8>*);
/* 02675294 */
BOOL hdHudMagicBar_c::init(void* resource,u32 option,hdPartsDesc_c* desc) {
    if(!ProgressBarPart_l::create(&sMagicName,resource,option,gabi::ea(desc)))return FALSE;
    hdPartsPaneSRT_c* root=static_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    for(u32 index=0;index<3;++index) {
        hdUiSafeString* name=&sMagicPaneNames.mNames[index];
        hdPartsPaneSRTVtbl_c* vtable=root->srtVtbl();
        name->assureTermination();
        mPanes[index]=gabi::call_ptr<hdPartsPaneSRT_c*>(vtable->mFindPane.get(),root,name->mText.get(),1);
    }
    hdPartsPaneSRT_c* pane=mPanes[0];
    mOriginalTranslation[0]=pane->mTranslationXBits;
    mOriginalTranslation[1]=pane->mTranslationYBits;
    mOriginalTranslation[2]=pane->mTranslationZBits;
    hdPartsPaneSRT_c* last=mPanes[2];hdPartsPaneSRT_c* middle=mPanes[1];
    f32 lastWidth=last->mSizeX,middleWidth=middle->mSizeX;
    mPaneSpacing=gabi::fsubs_ppc(lastWidth,middleWidth);
    startState(&sMagicBlank);return TRUE;
}
/* 026754F0 */
BOOL hdHudMagicBar_c::loadLayout(void* name,u32 accessor,u32 option,u32 parent,void* desc) {
    hdPartsLayout_c* layout=static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c),sMagicUi->mHeap,4));
    if(layout)layout=hdPartsLayout_c::construct(layout);
    mpLayout=layout;
    if(!layout||!layout->vLoad(name,accessor,option,parent,7,5,desc))return FALSE;
    for(u32 index=0;index<5;++index) {
        u32 group=sMagicBindSlots.mSlots[index];
        hdUiSafeString* pane=gabi::at<hdUiSafeString>(gabi::ea(&sMagicGroups)+group*8);
        magicBindAnimation(animSet(),index+2,&sMagicAnimations.mNames[index],pane);
    }
    return TRUE;
}
/* 02675608 */
static void initHudMagicBar() {
    sMagicInitLists.mFourth=0;sMagicInitLists.mThird=0;sMagicInitLists.mSecond=0;sMagicInitLists.mFirst=0;
    magicRegister(&sMagicFirstRecord);
    f32 minimum=sMagicAngleLimits.mMinimum,maximum=sMagicAngleLimits.mMaximum;
    sMagicInitAngles.mMinimum=minimum;sMagicInitAngles.mMaximum=maximum;
    magicInitFlag(&sMagicInitAngles.mFlag);magicRegister(&sMagicSecondRecord);
    magicInitByte(&sMagicInitAngles.mByte);magicRegister(&sMagicThirdRecord);
    sMagicGroups.mNames[0].mVtable.v=gabi::ea(&sMagicStringVtable);
    sMagicGroups.mNames[0].mText.v=gabi::guest_addr("G_Bar_00");
    sMagicGroups.mNames[1].mVtable.v=gabi::ea(&sMagicStringVtable);
    sMagicGroups.mNames[1].mText.v=gabi::guest_addr("G_Flash_00");
    sMagicGroups.mNames[2].mVtable.v=gabi::ea(&sMagicStringVtable);
    sMagicGroups.mNames[2].mText.v=gabi::guest_addr("G_All_00");
    sMagicGroups.mNames[3].mVtable.v=gabi::ea(&sMagicStringVtable);
    sMagicGroups.mNames[3].mText.v=gabi::guest_addr("G_Change_00");
    sMagicAnimations.mNames[0].mVtable.v=gabi::ea(&sMagicStringVtable);
    sMagicAnimations.mNames[0].mText.v=gabi::guest_addr("InAll");
    sMagicAnimations.mNames[1].mVtable.v=gabi::ea(&sMagicStringVtable);
    sMagicAnimations.mNames[1].mText.v=gabi::guest_addr("Flash");
    sMagicAnimations.mNames[2].mVtable.v=gabi::ea(&sMagicStringVtable);
    sMagicAnimations.mNames[2].mText.v=gabi::guest_addr("ColorChange");
    sMagicAnimations.mNames[3].mVtable.v=gabi::ea(&sMagicStringVtable);
    sMagicAnimations.mNames[3].mText.v=gabi::guest_addr("ScaleChange");
    sMagicAnimations.mNames[4].mVtable.v=gabi::ea(&sMagicStringVtable);
    sMagicAnimations.mNames[4].mText.v=gabi::guest_addr("OutAll");
    sMagicPaneNames.mNames[0].mVtable.v=gabi::ea(&sMagicStringVtable);
    sMagicPaneNames.mNames[0].mText.v=gabi::guest_addr("N_All_00");
    sMagicPaneNames.mNames[1].mVtable.v=gabi::ea(&sMagicStringVtable);
    sMagicPaneNames.mNames[1].mText.v=gabi::guest_addr("P_MagicBase_00");
    sMagicPaneNames.mNames[2].mVtable.v=gabi::ea(&sMagicStringVtable);
    sMagicPaneNames.mNames[2].mText.v=gabi::guest_addr("P_MagicBase_01");
    sMagicName.mVtable.v=gabi::ea(&sMagicStringVtable);sMagicName.mText.v=gabi::guest_addr("MagicBar_00");
    u32 number;if(sMagicStateGuard)number=sMagicStateCounter;else { sMagicStateGuard=1;number=0; }
    hdPartsStateIdVtbl_c* vtable=&sMagicStateVtable;
    sMagicBlank.setup(++number,"::StateID_Wait",vtable,25,&sMagicStateContext);
    sMagicIn.setup(++number,"::StateID_In",vtable,28,&sMagicStateContext);
    sMagicDisplay.setup(++number,"::StateID_Disp",vtable,31,&sMagicStateContext);
    sMagicOut.setup(++number,"::StateID_Out",vtable,34,&sMagicStateContext);
    sMagicStateCounter=number;
}
/* WWHD glue
 data sMagicVtable 100F7548
 data sMagicHolderVtable 100F7670
 data sMagicInterfaceVtable 100F7680
 data sMagicZero 100F745C
 data sMagicOne 100F7458
 data sMagicUnset 100F7460
 data sMagicVisible 1047B090
 data sMagicWarning 1047B098
 data sMagicPlacement 1047B03C
 data sMagicAnimationSlots 100F73A8
 data sMagicBlank 10495128
 data sMagicIn 10495158
 data sMagicDisplay 10495188
 data sMagicOut 104951B8
 ext 02702AD4 void ProgressBarPart_l::destroy(u32)
 ext 02702C80 void ProgressBarPart_l::loadProgress()
 ext 02702D50 void hdMsgProgress_updateProgress(WindowProgress*)
 ext 02702E08 void ProgressBarPart_l::reset()
 ext 02702B40 ProgressBarPart_l* ProgressBarPart_l::construct(ProgressBarPart_l*)
 ext 02702CCC u32 ProgressBarPart_l::create(void*,void*,u32,u32)
 ext 025200D4 dMeterGameInfoHD* magicGetPlay()
 data sMagicUi 1018C404
 data sMagicStringVtable 100F737C
 data sMagicStateVtable 100F740C
 data sMagicStateGuard 101FD8E4
 data sMagicStateCounter 101FDD50
 data sMagicStateContext 101FF32C
 data sMagicInitLists 104950B8
 data sMagicInitAngles 104950AC
 data sMagicAngleLimits 100F7464
 data sMagicFirstRecord 101F6194
 data sMagicSecondRecord 101F61A0
 data sMagicThirdRecord 101F61AC
 data sMagicGroups 104950C8
 data sMagicAnimations 104950E8
 data sMagicPaneNames 10495110
 data sMagicBindSlots 100F7394
 data sMagicName 104951E8
 str 100F74C4 "G_Bar_00" in 02675608
 str 100F74D0 "G_Flash_00" in 02675608
 str 100F74DC "G_All_00" in 02675608
 str 100F7484 "G_Change_00" in 02675608
 str 100F746C "InAll" in 02675608
 str 100F7474 "Flash" in 02675608
 str 100F7490 "ColorChange" in 02675608
 str 100F749C "ScaleChange" in 02675608
 str 100F747C "OutAll" in 02675608
 str 100F74E8 "N_All_00" in 02675608
 str 100F74F4 "P_MagicBase_00" in 02675608
 str 100F7504 "P_MagicBase_01" in 02675608
 str 100F74A8 "MagicBar_00" in 02675608
 str 100F7514 "::StateID_Wait" in 02675608
 str 100F7524 "::StateID_In" in 02675608
 str 100F7534 "::StateID_Disp" in 02675608
 str 100F74B4 "::StateID_Out" in 02675608
 ext 02004E04 void magicBindAnimation(void*,u32,hdUiSafeString*,hdUiSafeString*)
 ext 028F026C void magicRegister(hdHeartInitRecord_l*)
 ext 028ED6F8 void magicInitFlag(be<u8>*)
 ext 028EAB2C void magicInitByte(be<u8>*)
*/

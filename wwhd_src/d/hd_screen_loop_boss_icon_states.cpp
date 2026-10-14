/* HD LoopBossIcon_00: icon and eye animation timers. */
#include "d/hd_screen_loop_boss_icon_states.h"
static u32 sLoopBossVtable;
static u32 sLoopBossHolderVtable;
static u32 sLoopBossInterfaceVtable;
static u32 sLoopBossStringVtable;
static hdPartsStateIdVtbl_c sLoopBossStateVtable;
static hdPartsStateId_c sLoopBossBlank;
static hdPartsStateId_c sLoopBossWait;
static hdStateId sLoopBossStateContext;
static u32 sLoopBossStateGuard;
static u32 sLoopBossStateCounter;
static hdHeartInitLists_l sLoopBossInitLists;
static hdHeartInitAngles_l sLoopBossInitAngles;
static hdHeartAngleLimits_l sLoopBossAngleLimits;
static hdHeartInitRecord_l sLoopBossFirstRecord;
static hdHeartInitRecord_l sLoopBossSecondRecord;
static hdHeartInitRecord_l sLoopBossThirdRecord;
struct hdLoopBossAnimations_c { hdUiSafeString mIcon,mEye; };
struct hdLoopBossGroups_c { hdUiSafeString mIcon,mEye; };
struct hdLoopBossSlots_c { be<u32> mSlots[2]; };
static hdLoopBossAnimations_c sLoopBossAnimations;
static hdLoopBossGroups_c sLoopBossGroups;
static hdLoopBossSlots_c sLoopBossSlots;
static hdUiSafeString sLoopBossName;

static hdScreenUiSystem* sLoopBossUi;
static f32 sLoopBossZero;
static f32 sLoopBossFirstRange;
static f32 sLoopBossFirstBase;
static f32 sLoopBossUnsignedThreshold;
static f32 sLoopBossSecondRange;
static f32 sLoopBossSecondBase;
void loopBossBindAnimation(void*,u32,hdUiSafeString*,hdUiSafeString*);
void loopBossRegister(hdHeartInitRecord_l*);
void loopBossInitFlag(be<u8>*);
void loopBossInitByte(be<u8>*);

/* 026744D4 */
void hdHudLoopBoss_c::destroy(hdHudLoopBoss_c* screen,u32 flags) {
    if(!screen)return;
    screen->mStateHolder.mHolderVtable=&sLoopBossHolderVtable;
    screen->mSecondaryVtable=&sLoopBossInterfaceVtable;
    screen->NoControlPart_l::destroy(0);
    if(flags&1)operator_delete(screen);
}
/* 02674540 */
void hdHudLoopBoss_c::resetTimers() {
    f32 frame=sLoopBossZero;
    hdAnimSet_setFrame(animSet(),0,0,frame);
    hdAnimSet_setFrame(animSet(),1,1,frame);
    f32 random=cM_rndF(sLoopBossFirstRange);
    f32 interval=gabi::fadds_ppc(random,sLoopBossFirstBase);
    f32 threshold=sLoopBossUnsignedThreshold;
    mFirstTimer=interval<threshold?u32(gabi::ftoi(interval)):u32(gabi::ftoi(gabi::fsubs_ppc(interval,threshold)))+0x80000000u;
    random=cM_rndF(sLoopBossSecondRange);
    interval=gabi::fadds_ppc(gabi::fadds_ppc(random,random),sLoopBossSecondBase);
    mSecondTimer=interval<threshold?u32(gabi::ftoi(interval)):u32(gabi::ftoi(gabi::fsubs_ppc(interval,threshold)))+0x80000000u;
}
/* 02674830 */
hdHudLoopBoss_c* hdHudLoopBoss_c::construct(hdHudLoopBoss_c* screen) {
    if(!screen) { screen=static_cast<hdHudLoopBoss_c*>(operator_new(sizeof(hdHudLoopBoss_c)));if(!screen)return NULL; }
    hdPartsNoControl_construct(screen);
    screen->mVtable.v=gabi::ea(&sLoopBossVtable);
    screen->mStateHolder.mHolderVtable=&sLoopBossHolderVtable;
    screen->mSecondaryVtable=&sLoopBossInterfaceVtable;
    screen->mSecondTimer=0;screen->mFirstTimer=0;
    return screen;
}
/* 026748A8 */
BOOL hdHudLoopBoss_c::init(void* resource,u32 option,hdPartsDesc_c* desc) {
    if(!hdPartsNoControl_init(this,&sLoopBossName,gabi::ea(resource),option,desc))return FALSE;
    startState(&sLoopBossBlank);
    return TRUE;
}
/* 0267493C */
BOOL hdHudLoopBoss_c::loadLayout(void* name,u32 accessor,u32 option,u32 parent,void* desc) {
    hdPartsLayout_c* layout=static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c),sLoopBossUi->mHeap,4));
    if(layout)layout=hdPartsLayout_c::construct(layout);
    mpLayout=layout;
    if(!layout||!layout->vLoad(name,accessor,option,parent,2,2,desc))return FALSE;
    for(u32 index=0;index<2;++index) {
        u32 group=sLoopBossSlots.mSlots[index];
        hdUiSafeString* animation=gabi::at<hdUiSafeString>(gabi::ea(&sLoopBossAnimations)+index*8);
        hdUiSafeString* pane=gabi::at<hdUiSafeString>(gabi::ea(&sLoopBossGroups)+group*8);
        loopBossBindAnimation(animSet(),index,animation,pane);
    }
    return TRUE;
}
/* 02674A54 */
static void initHudLoopBoss() {
    sLoopBossInitLists.mFourth=0;sLoopBossInitLists.mThird=0;
    sLoopBossInitLists.mSecond=0;sLoopBossInitLists.mFirst=0;
    loopBossRegister(&sLoopBossFirstRecord);
    f32 minimum=sLoopBossAngleLimits.mMinimum;
    f32 maximum=sLoopBossAngleLimits.mMaximum;
    sLoopBossInitAngles.mMinimum=minimum;sLoopBossInitAngles.mMaximum=maximum;
    loopBossInitFlag(&sLoopBossInitAngles.mFlag);
    loopBossRegister(&sLoopBossSecondRecord);
    loopBossInitByte(&sLoopBossInitAngles.mByte);
    loopBossRegister(&sLoopBossThirdRecord);
    sLoopBossAnimations.mIcon.mVtable.v=gabi::ea(&sLoopBossStringVtable);
    sLoopBossAnimations.mIcon.mText.v=gabi::guest_addr("BossIcon");
    sLoopBossAnimations.mEye.mVtable.v=gabi::ea(&sLoopBossStringVtable);
    sLoopBossAnimations.mEye.mText.v=gabi::guest_addr("BossEye");
    sLoopBossGroups.mIcon.mVtable.v=gabi::ea(&sLoopBossStringVtable);
    sLoopBossGroups.mIcon.mText.v=gabi::guest_addr("G_BossIcon_00");
    sLoopBossGroups.mEye.mVtable.v=gabi::ea(&sLoopBossStringVtable);
    sLoopBossGroups.mEye.mText.v=gabi::guest_addr("G_BossEye_00");
    sLoopBossName.mVtable.v=gabi::ea(&sLoopBossStringVtable);
    sLoopBossName.mText.v=gabi::guest_addr("LoopBossIcon_00");
    u32 number;
    if(sLoopBossStateGuard)number=sLoopBossStateCounter;
    else { sLoopBossStateGuard=1;number=0; }
    hdPartsStateIdVtbl_c* vtable=&sLoopBossStateVtable;
    sLoopBossWait.setup(++number,"::StateID_Wait",vtable,25,&sLoopBossStateContext);
    sLoopBossStateCounter=number;
}
/* WWHD glue
 data sLoopBossVtable 100F7278
 data sLoopBossHolderVtable 100F7358
 data sLoopBossInterfaceVtable 100F7368
 data sLoopBossStringVtable 100F7154
 data sLoopBossStateVtable 100F71B4
 data sLoopBossBlank 1049DD60
 data sLoopBossStateContext 101FF32C
 data sLoopBossStateGuard 101FD8E4
 data sLoopBossStateCounter 101FDD50
 data sLoopBossInitLists 10495044
 data sLoopBossInitAngles 10495038
 data sLoopBossAngleLimits 100F721C
 data sLoopBossFirstRecord 101F6170
 data sLoopBossSecondRecord 101F617C
 data sLoopBossThirdRecord 101F6188
 data sLoopBossAnimations 10495064
 data sLoopBossGroups 10495054
 data sLoopBossSlots 100F7148
 data sLoopBossName 104950A4
 data sLoopBossUi 1018C404
 str 100F723C "G_BossIcon_00" in 02674A54
 str 100F724C "G_BossEye_00" in 02674A54
 str 100F725C "BossIcon" in 02674A54
 str 100F7224 "BossEye" in 02674A54
 str 100F722C "LoopBossIcon_00" in 02674A54
 str 100F7268 "::StateID_Wait" in 02674A54
 data sLoopBossWait 10495074
 data sLoopBossZero 100F7204
 data sLoopBossFirstRange 100F7208
 data sLoopBossFirstBase 100F720C
 data sLoopBossUnsignedThreshold 100F7210
 data sLoopBossSecondRange 100F7214
 data sLoopBossSecondBase 100F7218
 ext 02004E04 void loopBossBindAnimation(void*,u32,hdUiSafeString*,hdUiSafeString*)
 ext 028F026C void loopBossRegister(hdHeartInitRecord_l*)
 ext 028ED6F8 void loopBossInitFlag(be<u8>*)
 ext 028EAB2C void loopBossInitByte(be<u8>*)
*/

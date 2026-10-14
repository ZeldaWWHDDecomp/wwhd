/* Main HD HUD: child-screen placement and visibility coordination. */
#include "d/hd_screen_hud_main.h"
#include "d/hd_input_ctrl.h"
#include "d/hd_screen_dungeon_key_states.h"
#include "d/hd_screen_swim_time_hud.h"
#include "d/hd_screen_time.h"
#include "d/hd_screen_item_shortcut.h"
#include "d/hd_pane_util.h"

#include "d/hd_screen_command_a.h"
#include "d/hd_screen_magic_bar.h"
#include "d/hd_screen_heart_states.h"
#include "d/hd_screen_arrow.h"
#include "d/d_meter_00.h"
#include "d/hd_screen_rupy.h"
#include "d/hd_screen_compass_clock.h"
#include "d/hd_screen_boss_hp.h"
#include "d/hd_screen_rupy_sword_counter_2.h"
static u32 hudMainFloatBits(f32 value) { u32 bits;memcpy(&bits,&value,4);return bits; }
static u32 sHudMainVtable;
static u32 sHudMainHolderVtable;
static u32 sHudMainInterfaceVtable;
static f32 sHudMainZero;
struct hdHudMainSlots_c { be<u32> mSlots[8]; };
static hdHudMainSlots_c sHudMainSlots;
struct hdHudMainSettingsView_c { u8 mPrefix[0x18];hdPointerController mPointerController;be<u32> mLayout; };
WWHD_OFFSET(hdHudMainSettingsView_c,mPointerController,0x18);
WWHD_OFFSET(hdHudMainSettingsView_c,mLayout,0x1D0);
WWHD_SIZE(hdHudMainSettingsView_c,0x1D4);
static hdHudMainSettingsView_c* sHudMainSettings;
struct hdHudMainMode_l { u8 _00[0x58];be<u8> mArrow;u8 _59[7];be<u8> mEnemyHpVisible; };
static hdHudMainMode_l sHudMainMode;
struct hdHudMainBattery_l { u8 _00[0x50];be<u8> mVisible; };
/* The helper writes vector bits; pure lfs/stfs copies preserve signalling NaNs. */
struct hdHudMainVector_c { be<u32> mXBits,mYBits,mZBits; };
WWHD_SIZE(hdHudMainVector_c,0xC);
WWHD_OFFSET(hdHudMainVector_c,mYBits,4);
WWHD_OFFSET(hdHudMainVector_c,mZBits,8);
dMeterGameInfoHD* hudMainGetPlay();
u32 hudMainEventActive(hdHudMain_c*);
/* 0267598C */
u32 hdHudMainStateNumber_c::getNumber() { return mNumber; }
/* 02675A8C */
void hdHudMain_c::destroy(hdHudMain_c* screen,u32 flags) {
    if(!screen)return;
    screen->mStateHolder.mHolderVtable=&sHudMainHolderVtable;
    screen->mSecondaryVtable=&sHudMainInterfaceVtable;
    hdParts_c::destroy(screen,0);if(flags&1)operator_delete(screen);
}
/* 02675AF8 */
hdHudMain_c* hdHudMain_c::construct(hdHudMain_c* screen) {
    if(!screen) { screen=static_cast<hdHudMain_c*>(operator_new(sizeof(hdHudMain_c)));if(!screen)return NULL; }
    hdParts_c::construct(screen);
    screen->mVtable.v=gabi::ea(&sHudMainVtable);
    screen->mSecondaryVtable=&sHudMainInterfaceVtable;
    screen->mStateHolder.mHolderVtable=&sHudMainHolderVtable;
    gptr<hdPartsPaneSRT_c>* panes=gabi::at<gptr<hdPartsPaneSRT_c>>(gabi::ea(screen)+u32(offsetof(hdHudMain_c,mPanes)));
    if(!panes)operator_new(20);
    be<f32>* origins=gabi::at<be<f32>>(gabi::ea(screen)+u32(offsetof(hdHudMain_c,mMagicOrigins)));
    if(!origins)operator_new(24);
    screen->mSwimTimePlacement=2;
    screen->mFlags[Reserved9A]=0;
    f32 zero=sHudMainZero;
    screen->mFlags[Reserved9B]=0;
    screen->mOriginalTranslation[1]=hudMainFloatBits(zero);
    screen->mOriginalTranslation[0]=hudMainFloatBits(zero);
    screen->mOriginalTranslation[2]=hudMainFloatBits(zero);
    screen->mFlags[Reserved99]=0;screen->mMagicAvailable=0;screen->mFlags[ReservedA6]=0;
    screen->mHeartLayout=2;screen->mPlayerMode=2;
    screen->mFlags[BatteryVisible]=0;screen->mFlags[ReservedA3]=0;screen->mFlags[ArrowVisible]=0;
    screen->mFlags[Reserved9C]=0;screen->mFlags[EnemyHpVisible]=0;screen->mFlags[Reserved98]=0;
    screen->mFlags[EventActive]=0;screen->mFlags[EventEnded]=0;screen->mFlags[Reserved9E]=0;
    screen->mFlags[HeartBattle]=0;screen->mFlags[Reserved9D]=0;screen->mFlags[Reserved9F]=0;
    screen->mFlags[AlternateLayout]=0;
    for(u32 i=0;i<6;++i)origins[i]=zero;
    for(u32 i=0;i<5;++i)panes[i]=NULL;
    return screen;
}
/* 02675C80 */
void hdHudMain_c::setFrame(u32 animation,u32 frame) {
    f32 value=f32(frame);u32 slot=sHudMainSlots.mSlots[animation];
    hdAnimSet_setFrame(animSet(),animation,slot,value);
}
/* 02675CD8 */
void hdHudMain_c::captureMagicOrigins() {
    for(u32 i=0;i<2;++i) {
        setFrame(3,i);vCalcLayout();
        hdHudMainVector_c origin;
        hdPaneUtil_c::getGlobalTranslation(reinterpret_cast<cXyz*>(&origin),reinterpret_cast<PaneUtilPane_l*>(mPanes[4].get()));
        be<u32>* bits=reinterpret_cast<be<u32>*>(mMagicOrigins[i]);
        bits[0]=origin.mXBits;bits[1]=origin.mYBits;bits[2]=origin.mZBits;
    }
    hdPartsPaneSRT_c* pane=mPanes[4];
    mOriginalTranslation[0]=pane->mTranslationXBits;
    mOriginalTranslation[1]=pane->mTranslationYBits;
    mOriginalTranslation[2]=pane->mTranslationZBits;
}
/* 02675DC0 */
void hdHudMain_c::resetVisibility() {
    mFlags[EnemyHpVisible]=0;setFrame(0,0);
    mFlags[ArrowVisible]=0;setFrame(1,0);
    mFlags[BatteryVisible]=0;setFrame(2,0);
}
/* 02675E2C */
void hdHudMain_c::updateHeartLayout() {
    u32 frame=u16(sSaveInfo->mStatusA.mMaxLife)>=44?1:0;
    if(u32(mHeartLayout)==frame)return;
    mHeartLayout=frame;setFrame(3,frame);
    u32 layout=mHeartLayout;
    hdHudMagicBar_c* magic=reinterpret_cast<hdHudMagicBar_c*>(child(MagicBar));
    be<u32>* origin=reinterpret_cast<be<u32>*>(mMagicOrigins[layout<2?layout:0]);
    be<u32>* target=reinterpret_cast<be<u32>*>(magic->mOrigin);
    target[0]=origin[0];target[1]=origin[1];target[2]=origin[2];
}
/* 02675ED4 */
void hdHudMain_c::updateMagicAvailability() {
    u32 frame=sSaveInfo->mStatusA.mMaxMagic?0:1;
    if(u32(mMagicAvailable)!=frame) { mMagicAvailable=frame;setFrame(4,frame); }
}
/* 02675F08 */
void hdHudMain_c::updateSwimTimePlacement() {
    dMeterGameInfoHD* play=hudMainGetPlay();
    if(!gabi::at<void>(gabi::ea(play)+u32(offsetof(dMeterGameInfoHD,mStage))))return;
    play=hudMainGetPlay();dMeterStageAccessHD* stage=&play->mStage;
    if(!gabi::call_ptr<dMeterStageInfoHD*>(u32(stage->__vtbl->getStagInfo),stage))return;
    play=hudMainGetPlay();stage=&play->mStage;
    dMeterStageInfoHD* result=gabi::call_ptr<dMeterStageInfoHD*>(u32(stage->__vtbl->getStagInfo),stage);
    u32 frame=(u8(result->mSaveTable)&1)?0:1;
    if(u32(mSwimTimePlacement)!=frame) { mSwimTimePlacement=frame;setFrame(5,frame); }
}
/* 02675FAC */
void hdHudMain_c::updateCounterPlacement() {
    u32 frame=hudMainGetPlay()->mEventMode==7?1:0;
    if(u32(mPlayerMode)!=frame) { mPlayerMode=frame;setFrame(7,frame); }
}
/* 02676014 */
void hdHudMain_c::updateLayout() {
    hdPartsPaneSRT_c* source=mPanes[mFlags[AlternateLayout]?2:1];
    u32 x=source->mTranslationXBits,y=source->mTranslationYBits,z=source->mTranslationZBits;
    u32 sx=*reinterpret_cast<be<u32>*>(&source->mScaleX),sy=*reinterpret_cast<be<u32>*>(&source->mScaleY);
    hdPartsPaneSRT_c* target=mPanes[0];u8 flags=target->mFlags;
    target->mTranslationXBits=x;target->mTranslationYBits=y;target->mFlags=flags|0x10;target->mTranslationZBits=z;
    target=mPanes[0];flags=target->mFlags;
    *reinterpret_cast<be<u32>*>(&target->mScaleX)=sx;target->mFlags=flags|0x10;*reinterpret_cast<be<u32>*>(&target->mScaleY)=sy;
}
/* 02676104 */
void hdHudMain_c::initializeValues() {
    resetVisibility();updateHeartLayout();updateMagicAvailability();updateSwimTimePlacement();updateCounterPlacement();
    mFlags[AlternateLayout]=u32(sHudMainSettings->mLayout)==2;updateLayout();
}
/* 02676314 */
void hdHudMain_c::finalize() { vDeleteChildren();vDestroyLayout();detachArchives(); }
/* 02676368 */
void hdHudMain_c::updateEnemyHpVisible() {
    if(mFlags[EnemyHpVisible]) { if(!sHudMainMode.mEnemyHpVisible) { mFlags[EnemyHpVisible]=0;setFrame(0,0); } }
    else if(sHudMainMode.mEnemyHpVisible) { mFlags[EnemyHpVisible]=1;setFrame(0,1); }
}
/* 026763B4 */
void hdHudMain_c::updateArrowVisibility() {
    if(mFlags[ArrowVisible]) {
        if(sHudMainMode.mArrow)return;
        mFlags[ArrowVisible]=0;setFrame(1,0);
        reinterpret_cast<hdArrow_screen_c*>(child(Arrow))->mShown=0;
    } else if(sHudMainMode.mArrow) {
        mFlags[ArrowVisible]=1;setFrame(1,1);
        reinterpret_cast<hdArrow_screen_c*>(child(Arrow))->mShown=1;
    }
}
/* 02676490 */
void hdHudMain_c::updateBatteryVisibility() {
    u32 mode=sHudMainSettings->mPointerController.controllerType();
    BOOL visible=mode==0||mode==1;
    if(mFlags[BatteryVisible]) {
        if(visible)return;
        mFlags[BatteryVisible]=0;setFrame(2,0);
        reinterpret_cast<hdHudMainBattery_l*>(child(Battery))->mVisible=0;
    } else if(visible) {
        mFlags[BatteryVisible]=1;setFrame(2,1);
        reinterpret_cast<hdHudMainBattery_l*>(child(Battery))->mVisible=1;
    }
}
/* 02676584 */
void hdHudMain_c::updateVisibility() { updateEnemyHpVisible();updateArrowVisibility();updateBatteryVisibility(); }
/* 026765C0 */
void hdHudMain_c::updateBattle() {
    u8 battle=reinterpret_cast<hdHudHeartScreen_c*>(child(Heart))->mSpecialMode;
    if(battle) { if(!mFlags[HeartBattle]) { mFlags[HeartBattle]=1;setFrame(6,1); } }
    else if(mFlags[HeartBattle]) { mFlags[HeartBattle]=0;setFrame(6,0); }
}
/* 02676610 */
void hdHudMain_c::updateEvent() {
    mFlags[EventEnded]=0;
    if(hudMainEventActive(this))mFlags[EventActive]=1;
    else { if(mFlags[EventActive])mFlags[EventEnded]=1;mFlags[EventActive]=0; }
}
/* 02676674 */
void hdHudMain_c::execute() {
    u32 alternate=u32(sHudMainSettings->mLayout)==2;
    if(u8(mFlags[AlternateLayout])!=alternate) { mFlags[AlternateLayout]=alternate;updateLayout(); }
    updateHeartLayout();updateMagicAvailability();updateSwimTimePlacement();updateCounterPlacement();updateVisibility();
    vExecuteChildren();updateBattle();vCalcLayout();updateEvent();
}
static hdScreenUiSystem* sHudMainUi;
static u32 sHudMainStringVtable;
struct hdHudMainGroups_c { hdUiSafeString mNames[8]; };
struct hdHudMainAnimations_c { hdUiSafeString mNames[8]; };
struct hdHudMainPaneNames_c { hdUiSafeString mNames[5]; };
struct hdHudMainChildNames_c { hdUiSafeString mNames[13]; };
struct hdHudMainArchives_c { hdUiSafeString mNames[18]; };
struct hdHudMainBindSlots_c { be<u32> mSlots[8]; };
static hdHudMainGroups_c sHudMainGroups;
static hdHudMainAnimations_c sHudMainAnimations;
static hdHudMainPaneNames_c sHudMainPaneNames;
static hdHudMainChildNames_c sHudMainChildNames;
static hdHudMainArchives_c sHudMainArchives;
static hdHudMainBindSlots_c sHudMainBindSlots;
static hdUiSafeString sHudMainName;
void hudMainBindAnimation(void*,u32,hdUiSafeString*,hdUiSafeString*);
/* 02676174 */
BOOL hdHudMain_c::init(void* resource,u32 option,u32 parent) {
    attachArchives(&sHudMainArchives.mNames[0],18);
    if(!gabi::call_ptr<BOOL>(mVtable->mLoad.get(),this,&sHudMainName,mpResAccessor.get(),resource,option,parent))return FALSE;
    if(!gabi::call_ptr<BOOL>(mVtable->mCreateChildren.get(),this))return FALSE;
    hdPartsPaneSRT_c* root=static_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    for(u32 index=0;index<5;++index) {
        hdUiSafeString* name=&sHudMainPaneNames.mNames[index];hdPartsPaneSRTVtbl_c* vtable=root->srtVtbl();
        name->assureTermination();
        mPanes[index]=gabi::call_ptr<hdPartsPaneSRT_c*>(vtable->mFindPane.get(),root,name->mText.get(),1);
    }
    captureMagicOrigins();initializeValues();vExecuteChildren();vCalcLayout();return TRUE;
}
void hudMainResetEnemyHp(hdParts_c*);
void hudMainResetBattery(hdParts_c*);
/* 0267672C */
void hdHudMain_c::reset() {
    initializeValues();
    reinterpret_cast<hdHudHeartScreen_c*>(child(Heart))->reset();
    reinterpret_cast<hdRupyScreen_c*>(child(Rupy))->endExecute();
    reinterpret_cast<hdHudMagicBar_c*>(child(MagicBar))->reset();
    reinterpret_cast<hdDungeonKeyStates_c*>(child(DungeonKey))->toWait();
    reinterpret_cast<hdCompassClock_c*>(child(CompassClock))->toWait();
    reinterpret_cast<hdSwimTimeHud_c*>(child(SwimTime))->resetState();
    reinterpret_cast<hdTimeScreen_c*>(child(Time))->resetState();
    reinterpret_cast<hdBossHp_c*>(child(BossHp))->toWait();
    hudMainResetEnemyHp(child(EnemyHp));
    reinterpret_cast<hdArrow_screen_c*>(child(Arrow))->toWait();
    hudMainResetBattery(child(Battery));
    reinterpret_cast<hdScreenRupySwordCounter_c*>(child(RupySwordCounter))->hide();
    reinterpret_cast<hdScreenItemShortcut_c*>(child(Shortcut))->reset();
    vExecuteChildren();vCalcLayout();
}
/* 026768E4 */
void hdHudMain_c::beginSecondary() { reinterpret_cast<hdTimeScreen_c*>(child(Time))->setBottom(); }
/* 02676900 */
void hdHudMain_c::endSecondary() { reinterpret_cast<hdTimeScreen_c*>(child(Time))->setTop(); }
/* 02676D38 */
BOOL hdHudMain_c::loadLayout(void* name,u32 accessor,u32 option,u32 parent,void* desc) {
    hdPartsLayout_c* layout=static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c),sHudMainUi->mHeap,4));
    if(layout)layout=hdPartsLayout_c::construct(layout);mpLayout=layout;
    if(!layout)return FALSE;
    layout->vLoad(name,accessor,option,parent,8,8,desc);
    for(u32 i=0;i<8;++i) {
        u32 group=sHudMainBindSlots.mSlots[i];
        hdUiSafeString* pane=gabi::at<hdUiSafeString>(gabi::ea(&sHudMainGroups)+group*8);
        hudMainBindAnimation(animSet(),i,&sHudMainAnimations.mNames[i],pane);
    }
    vCalcLayout();return TRUE;
}
struct hdHudMainInitSettings_c { be<f32> mMinimum,mMaximum,mDistance[4];be<u8> mFlag,mByte;u8 _1A[2]; };
static hdHudMainInitSettings_c sHudMainInitSettings;
static hdHeartInitLists_l sHudMainInitLists;
static hdHeartAngleLimits_l sHudMainAngleLimits;
struct hdHudMainDistanceLimits_c { be<f32> mFar,mNear; };
static hdHudMainDistanceLimits_c sHudMainDistanceLimits;
static hdHeartInitRecord_l sHudMainFirstRecord;
static hdHeartInitRecord_l sHudMainSecondRecord;
static hdHeartInitRecord_l sHudMainThirdRecord;
void hudMainRegister(hdHeartInitRecord_l*);
void hudMainInitFlag(be<u8>*);
void hudMainInitByte(be<u8>*);
/* 02676E5C */
static void initHudMain() {
    sHudMainInitLists.mFourth=0;sHudMainInitLists.mThird=0;sHudMainInitLists.mSecond=0;sHudMainInitLists.mFirst=0;
    hudMainRegister(&sHudMainFirstRecord);
    f32 minimum=sHudMainAngleLimits.mMinimum,maximum=sHudMainAngleLimits.mMaximum;
    sHudMainInitSettings.mMinimum=minimum;sHudMainInitSettings.mMaximum=maximum;
    hudMainInitFlag(&sHudMainInitSettings.mFlag);hudMainRegister(&sHudMainSecondRecord);
    hudMainInitByte(&sHudMainInitSettings.mByte);hudMainRegister(&sHudMainThirdRecord);
    f32 far=sHudMainDistanceLimits.mFar,near=sHudMainDistanceLimits.mNear;
    sHudMainInitSettings.mDistance[0]=far;sHudMainInitSettings.mDistance[1]=far;
    sHudMainInitSettings.mDistance[3]=near;sHudMainInitSettings.mDistance[2]=near;
    sHudMainGroups.mNames[0].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainGroups.mNames[0].mText.v=gabi::guest_addr("G_ShortcutIcon_00");
    sHudMainGroups.mNames[1].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainGroups.mNames[1].mText.v=gabi::guest_addr("G_HeartPos_00");
    sHudMainGroups.mNames[2].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainGroups.mNames[2].mText.v=gabi::guest_addr("G_EnemyHP_00");
    sHudMainGroups.mNames[3].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainGroups.mNames[3].mText.v=gabi::guest_addr("G_SwimTimePos_00");
    sHudMainGroups.mNames[4].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainGroups.mNames[4].mText.v=gabi::guest_addr("G_Arrow_00");
    sHudMainGroups.mNames[5].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainGroups.mNames[5].mText.v=gabi::guest_addr("G_Battery_00");
    sHudMainGroups.mNames[6].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainGroups.mNames[6].mText.v=gabi::guest_addr("G_HeartBig_00");
    sHudMainGroups.mNames[7].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainGroups.mNames[7].mText.v=gabi::guest_addr("G_CounterPos_00");
    sHudMainAnimations.mNames[0].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainAnimations.mNames[0].mText.v=gabi::guest_addr("Visible");
    sHudMainAnimations.mNames[1].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainAnimations.mNames[1].mText.v=gabi::guest_addr("Visible");
    sHudMainAnimations.mNames[2].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainAnimations.mNames[2].mText.v=gabi::guest_addr("Visible");
    sHudMainAnimations.mNames[3].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainAnimations.mNames[3].mText.v=gabi::guest_addr("HeartPos");
    sHudMainAnimations.mNames[4].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainAnimations.mNames[4].mText.v=gabi::guest_addr("MagicPos");
    sHudMainAnimations.mNames[5].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainAnimations.mNames[5].mText.v=gabi::guest_addr("SwimTimePos");
    sHudMainAnimations.mNames[6].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainAnimations.mNames[6].mText.v=gabi::guest_addr("HeartBig");
    sHudMainAnimations.mNames[7].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainAnimations.mNames[7].mText.v=gabi::guest_addr("CounterPos");
    sHudMainPaneNames.mNames[0].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainPaneNames.mNames[0].mText.v=gabi::guest_addr("N_Default_00");
    sHudMainPaneNames.mNames[1].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainPaneNames.mNames[1].mText.v=gabi::guest_addr("N_TV_00");
    sHudMainPaneNames.mNames[2].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainPaneNames.mNames[2].mText.v=gabi::guest_addr("N_DRC_00");
    sHudMainPaneNames.mNames[3].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainPaneNames.mNames[3].mText.v=gabi::guest_addr("N_HeartPos_00");
    sHudMainPaneNames.mNames[4].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainPaneNames.mNames[4].mText.v=gabi::guest_addr("L_MagicBar_00");
    sHudMainChildNames.mNames[0].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainChildNames.mNames[0].mText.v=gabi::guest_addr("L_HeartAll_00");
    sHudMainChildNames.mNames[1].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainChildNames.mNames[1].mText.v=gabi::guest_addr("L_Rupy_00");
    sHudMainChildNames.mNames[2].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainChildNames.mNames[2].mText.v=gabi::guest_addr("L_MagicBar_00");
    sHudMainChildNames.mNames[3].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainChildNames.mNames[3].mText.v=gabi::guest_addr("L_DungeonKey_00");
    sHudMainChildNames.mNames[4].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainChildNames.mNames[4].mText.v=gabi::guest_addr("L_CompassClock_00");
    sHudMainChildNames.mNames[5].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainChildNames.mNames[5].mText.v=gabi::guest_addr("L_SwimTime_00");
    sHudMainChildNames.mNames[6].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainChildNames.mNames[6].mText.v=gabi::guest_addr("L_Time_00");
    sHudMainChildNames.mNames[7].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainChildNames.mNames[7].mText.v=gabi::guest_addr("L_BossHP_00");
    sHudMainChildNames.mNames[8].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainChildNames.mNames[8].mText.v=gabi::guest_addr("L_EnemyHP_00");
    sHudMainChildNames.mNames[9].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainChildNames.mNames[9].mText.v=gabi::guest_addr("L_Arrow_00");
    sHudMainChildNames.mNames[10].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainChildNames.mNames[10].mText.v=gabi::guest_addr("L_BatteryVol1_00");
    sHudMainChildNames.mNames[11].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainChildNames.mNames[11].mText.v=gabi::guest_addr("L_RupySwordCounter_00");
    sHudMainChildNames.mNames[12].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainChildNames.mNames[12].mText.v=gabi::guest_addr("L_Shortcut_00");
    sHudMainArchives.mNames[0].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[0].mText.v=gabi::guest_addr("Main_00");
    sHudMainArchives.mNames[1].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[1].mText.v=gabi::guest_addr("HeartAll_00");
    sHudMainArchives.mNames[2].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[2].mText.v=gabi::guest_addr("Rupy_00");
    sHudMainArchives.mNames[3].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[3].mText.v=gabi::guest_addr("MagicBar_00");
    sHudMainArchives.mNames[4].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[4].mText.v=gabi::guest_addr("DungeonKey_00");
    sHudMainArchives.mNames[5].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[5].mText.v=gabi::guest_addr("CompassClock_00");
    sHudMainArchives.mNames[6].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[6].mText.v=gabi::guest_addr("SwimTime_00");
    sHudMainArchives.mNames[7].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[7].mText.v=gabi::guest_addr("Time_00");
    sHudMainArchives.mNames[8].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[8].mText.v=gabi::guest_addr("BossHP_00");
    sHudMainArchives.mNames[9].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[9].mText.v=gabi::guest_addr("EnemyHP_00");
    sHudMainArchives.mNames[10].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[10].mText.v=gabi::guest_addr("Arrow_00");
    sHudMainArchives.mNames[11].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[11].mText.v=gabi::guest_addr("Battery_00");
    sHudMainArchives.mNames[12].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[12].mText.v=gabi::guest_addr("RupySwordCounter_00");
    sHudMainArchives.mNames[13].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[13].mText.v=gabi::guest_addr("Shortcut_00");
    sHudMainArchives.mNames[14].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[14].mText.v=gabi::guest_addr("Heart_00");
    sHudMainArchives.mNames[15].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[15].mText.v=gabi::guest_addr("LoopBossIcon_00");
    sHudMainArchives.mNames[16].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[16].mText.v=gabi::guest_addr("BtnCollectIcon_00");
    sHudMainArchives.mNames[17].mVtable.v=gabi::ea(&sHudMainStringVtable);
    sHudMainArchives.mNames[17].mText.v=gabi::guest_addr("UnreadMiiverse_00");
    sHudMainName.mVtable.v=gabi::ea(&sHudMainStringVtable);sHudMainName.mText.v=gabi::guest_addr("Main_00");
}
struct hdHudMainFactoryEntry_c { gptr<be<u8>> mName;gfn mCreate; };
struct hdHudMainFactoryEntries_c { hdHudMainFactoryEntry_c mEntries[13]; };
static hdHudMainFactoryEntries_c sHudMainFactoryEntries;
struct hdHudMainFactoryDesc_l { u8 _00[8];hdUiSafeString mClassName;u8 _10[4];hdUiSafeString mPaneName; };
static hdUiSafeString sHudMainFactoryRupy;
static hdUiSafeString sHudMainFactoryHeart;
static hdUiSafeString sHudMainFactoryMagic;
static hdUiSafeString sHudMainFactoryDungeonKey;
static hdUiSafeString sHudMainFactoryCompassClock;
static hdUiSafeString sHudMainFactorySwimTime;
static hdUiSafeString sHudMainFactoryTime;
static hdUiSafeString sHudMainFactoryBossHp;
static hdUiSafeString sHudMainFactoryEnemyHp;
static hdUiSafeString sHudMainFactoryArrow;
static hdUiSafeString sHudMainFactoryBattery;
static hdUiSafeString sHudMainFactoryRupySwordCounter;
static hdUiSafeString sHudMainFactoryShortcut;
static BOOL hudMainNameMatches(hdUiSafeString* input,hdUiSafeString* expected) {
    input->assureTermination();input->assureTermination();
    be<u8>* first=input->mText;
    expected->assureTermination();
    if(first==expected->mText.get())return TRUE;
    first=input->mText;be<u8>* second=expected->mText;
    for(u32 i=0;i<0x40001;++i) {
        u8 a=first[i],b=second[i];if(a!=b)return FALSE;if(!a)return TRUE;
    }
    return FALSE;
}
/* 0267691C */
BOOL hdHudMain_c::createChild(void* description) {
    hdHudMainFactoryDesc_l* desc=static_cast<hdHudMainFactoryDesc_l*>(description);
    hdHudMainFactoryEntries_c entries;
    for(u32 i=0;i<13;++i) {
        entries.mEntries[i].mName=sHudMainFactoryEntries.mEntries[i].mName;
        entries.mEntries[i].mCreate.v=sHudMainFactoryEntries.mEntries[i].mCreate.v;
    }
    sHudMainFactoryRupy.assureTermination();entries.mEntries[0].mName=sHudMainFactoryRupy.mText;
    sHudMainFactoryHeart.assureTermination();entries.mEntries[1].mName=sHudMainFactoryHeart.mText;
    sHudMainFactoryMagic.assureTermination();entries.mEntries[2].mName=sHudMainFactoryMagic.mText;
    sHudMainFactoryDungeonKey.assureTermination();entries.mEntries[3].mName=sHudMainFactoryDungeonKey.mText;
    sHudMainFactoryCompassClock.assureTermination();entries.mEntries[4].mName=sHudMainFactoryCompassClock.mText;
    sHudMainFactorySwimTime.assureTermination();entries.mEntries[5].mName=sHudMainFactorySwimTime.mText;
    sHudMainFactoryTime.assureTermination();entries.mEntries[6].mName=sHudMainFactoryTime.mText;
    sHudMainFactoryBossHp.assureTermination();entries.mEntries[7].mName=sHudMainFactoryBossHp.mText;
    sHudMainFactoryEnemyHp.assureTermination();entries.mEntries[8].mName=sHudMainFactoryEnemyHp.mText;
    sHudMainFactoryArrow.assureTermination();entries.mEntries[9].mName=sHudMainFactoryArrow.mText;
    sHudMainFactoryBattery.assureTermination();entries.mEntries[10].mName=sHudMainFactoryBattery.mText;
    sHudMainFactoryRupySwordCounter.assureTermination();entries.mEntries[11].mName=sHudMainFactoryRupySwordCounter.mText;
    sHudMainFactoryShortcut.assureTermination();entries.mEntries[12].mName=sHudMainFactoryShortcut.mText;
    for(u32 pane=0;pane<13;++pane) {
        if(!hudMainNameMatches(&desc->mPaneName,&sHudMainChildNames.mNames[pane]))continue;
        for(u32 type=0;type<13;++type) {
            hdUiSafeString className;
            className.mText=entries.mEntries[type].mName;
            className.mVtable.v=gabi::ea(&sHudMainStringVtable);
            if(!hudMainNameMatches(&desc->mClassName,&className))continue;
            gptr<hdParts_c>* slot=childSlot(pane);
            *slot=gabi::call_ptr<hdParts_c*>(entries.mEntries[type].mCreate.get(),static_cast<void*>(NULL),0,desc);
            if(!child(pane))return FALSE;
            return TRUE;
        }
        return hdParts_c::createChild(pane,static_cast<hdPartsDesc_c*>(description));
    }
    return TRUE;
}
/* WWHD glue
 data sHudMainVtable 100F7A48
 data sHudMainHolderVtable 100F7B10
 data sHudMainInterfaceVtable 100F7B20
 data sHudMainZero 100F7704
 data sHudMainSlots 100F76DC
 data sHudMainSettings 101F5088
 data sHudMainMode 1047B03C
 ext 025200D4 dMeterGameInfoHD* hudMainGetPlay()
 ext 027172E4 u32 hudMainEventActive(hdHudMain_c*)
 data sHudMainUi 1018C404
 data sHudMainStringVtable 100F76A4
 data sHudMainGroups 1049521C
 data sHudMainAnimations 1049525C
 data sHudMainPaneNames 1049529C
 data sHudMainChildNames 104952C4
 data sHudMainArchives 1049532C
 data sHudMainBindSlots 100F76BC
 data sHudMainName 104953BC
 data sHudMainInitSettings 104951F0
 data sHudMainInitLists 1049520C
 data sHudMainAngleLimits 100F7778
 data sHudMainDistanceLimits 100F7780
 data sHudMainFirstRecord 101F61B8
 data sHudMainSecondRecord 101F61C4
 data sHudMainThirdRecord 101F61D0
 str 100F784C "G_ShortcutIcon_00" in 02676E5C
 str 100F7860 "G_HeartPos_00" in 02676E5C
 str 100F7958 "G_EnemyHP_00" in 02676E5C
 str 100F7968 "G_SwimTimePos_00" in 02676E5C
 str 100F797C "G_Arrow_00" in 02676E5C
 str 100F7988 "G_Battery_00" in 02676E5C
 str 100F7870 "G_HeartBig_00" in 02676E5C
 str 100F77B0 "G_CounterPos_00" in 02676E5C
 str 100F7788 "Visible" in 02676E5C
 str 100F7998 "HeartPos" in 02676E5C
 str 100F79A4 "MagicPos" in 02676E5C
 str 100F77F0 "SwimTimePos" in 02676E5C
 str 100F79B0 "HeartBig" in 02676E5C
 str 100F79BC "CounterPos" in 02676E5C
 str 100F7A28 "N_Default_00" in 02676E5C
 str 100F77A8 "N_TV_00" in 02676E5C
 str 100F7A38 "N_DRC_00" in 02676E5C
 str 100F7948 "N_HeartPos_00" in 02676E5C
 str 100F789C "L_MagicBar_00" in 02676E5C
 str 100F7880 "L_HeartAll_00" in 02676E5C
 str 100F7890 "L_Rupy_00" in 02676E5C
 str 100F77C0 "L_DungeonKey_00" in 02676E5C
 str 100F78AC "L_CompassClock_00" in 02676E5C
 str 100F78C0 "L_SwimTime_00" in 02676E5C
 str 100F78D0 "L_Time_00" in 02676E5C
 str 100F77FC "L_BossHP_00" in 02676E5C
 str 100F79C8 "L_EnemyHP_00" in 02676E5C
 str 100F79D8 "L_Arrow_00" in 02676E5C
 str 100F79E4 "L_BatteryVol1_00" in 02676E5C
 str 100F78DC "L_RupySwordCounter_00" in 02676E5C
 str 100F78F4 "L_Shortcut_00" in 02676E5C
 str 100F7790 "Main_00" in 02676E5C
 str 100F7808 "HeartAll_00" in 02676E5C
 str 100F7798 "Rupy_00" in 02676E5C
 str 100F7814 "MagicBar_00" in 02676E5C
 str 100F7904 "DungeonKey_00" in 02676E5C
 str 100F77D0 "CompassClock_00" in 02676E5C
 str 100F7820 "SwimTime_00" in 02676E5C
 str 100F77A0 "Time_00" in 02676E5C
 str 100F7914 "BossHP_00" in 02676E5C
 str 100F79F8 "EnemyHP_00" in 02676E5C
 str 100F7A04 "Arrow_00" in 02676E5C
 str 100F7A10 "Battery_00" in 02676E5C
 str 100F782C "RupySwordCounter_00" in 02676E5C
 str 100F7840 "Shortcut_00" in 02676E5C
 str 100F7A1C "Heart_00" in 02676E5C
 str 100F77E0 "LoopBossIcon_00" in 02676E5C
 str 100F7920 "BtnCollectIcon_00" in 02676E5C
 str 100F7934 "UnreadMiiverse_00" in 02676E5C
 ext 02668AA8 void hudMainResetEnemyHp(hdParts_c*)
 ext 026233C4 void hudMainResetBattery(hdParts_c*)
 ext 02004E04 void hudMainBindAnimation(void*,u32,hdUiSafeString*,hdUiSafeString*)
 ext 028F026C void hudMainRegister(hdHeartInitRecord_l*)
 ext 028ED6F8 void hudMainInitFlag(be<u8>*)
 ext 028EAB2C void hudMainInitByte(be<u8>*)
 data sHudMainFactoryEntries 100F7710
 data sHudMainFactoryRupy 1049A2BC
 data sHudMainFactoryHeart 10494884
 data sHudMainFactoryMagic 104951E8
 data sHudMainFactoryDungeonKey 10491998
 data sHudMainFactoryCompassClock 1049221C
 data sHudMainFactorySwimTime 1049C8A4
 data sHudMainFactoryTime 1049B8C4
 data sHudMainFactoryBossHp 1048F46C
 data sHudMainFactoryEnemyHp 10493FF0
 data sHudMainFactoryArrow 1049CDA0
 data sHudMainFactoryBattery 1048DFF0
 data sHudMainFactoryRupySwordCounter 1048EBD0
 data sHudMainFactoryShortcut 1049A3D8
 ext 026F88F0 void hdParts_c::destroy(hdParts_c*,s32)
 ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c*)
 ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString*,u32)
 ext 026F91F8 void hdParts_c::detachArchives()
 ext 026F92C0 u32 hdParts_c::createChild(u32,hdPartsDesc_c*)
 ext 02618D18 u32 hdPointerController::controllerType()
 ext 026CACBC void hdRupyScreen_c::endExecute()
 ext 026541A8 void hdDungeonKeyStates_c::toWait()
 ext 0264CE34 void hdCompassClock_c::toWait()
 ext 026E7164 void hdSwimTimeHud_c::resetState()
 ext 026E9FD0 void hdTimeScreen_c::resetState()
 ext 026298A8 void hdBossHp_c::toWait()
 ext 0261B714 void hdArrow_screen_c::toWait()
 ext 026CB738 void hdScreenRupySwordCounter_c::hide()
 ext 026DDCCC void hdScreenItemShortcut_c::reset()
 ext 026E9FE0 void hdTimeScreen_c::setBottom()
 ext 026E9FEC void hdTimeScreen_c::setTop()
 ext 02704D2C void hdPaneUtil_c::getGlobalTranslation(cXyz*,PaneUtilPane_l*)
*/

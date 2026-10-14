/** Telescope screen resource loading and cover animation coordination. */
#include "d/hd_screen_telescope_cover.h"
#include "d/hd_screen_telescope_scale.h"
static hdUiSafeString sCoverArchive;
static hdUiSafeString sCoverDescriptor;
static hdPartsStateId_c sCoverWait;
typedef gptr<hdScreenUiSystem> hdCoverUiSystemPointer_l;
static hdCoverUiSystemPointer_l sCoverUiSystem;
struct CoverGroupsView_l {
    hdUiSafeString mValues[4];
    hdUiSafeString& operator[](u32 index) { return mValues[index]; }
};
static CoverGroupsView_l sCoverGroups;
struct CoverAnimationsView_l {
    hdUiSafeString mValues[7];
    hdUiSafeString& operator[](u32 index) { return mValues[index]; }
};
static CoverAnimationsView_l sCoverAnimations;
struct CoverGroupIndicesView_l {
    be<u32> mValues[7];
    be<u32>& operator[](u32 index) { return mValues[index]; }
};
static CoverGroupIndicesView_l sCoverGroupIndices;
struct hdCoverPlay_l { u8 mPrefix[0x5BB3]; be<u8> mNextMode; u8 mToSound[5]; be<u8> mSoundMode; };
WWHD_OFFSET(hdCoverPlay_l, mSoundMode, 0x5BB9);
hdCoverPlay_l* hdCover_getPlay();

static inline void hdCover_attachArchives(hdTelescopeHud_c* receiver, hdUiSafeString* names, u32 count) { receiver->hdParts_c::attachArchives(names, count); }
static inline void hdCover_baseDestroyLayout(hdTelescopeHud_c* receiver) { receiver->hdParts_c::detachArchives(); }


void* hdCover_allocateLayout(u32,hdScreenHeap*,u32);

void hdCover_registerAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*);
/* 026E8820 */
BOOL hdTelescopeCover_c::create(hdTelescopeHud_c* screen,u32 arg1,u32 arg2,u32 arg3) {
    hdCover_attachArchives(screen,&sCoverArchive,1);
    if (!gabi::call_ptr<u32>(screen->mVtable->mLoad.get(),screen,&sCoverDescriptor,screen->mpResAccessor.get(),arg1,arg2,arg3))
        return FALSE;
    screen->findPanes();
    hdTelescopeScaleSetup_c::findScalePane(screen);
    hdAnimSet_operationCC(screen->mpLayout->animSet(),0,0);
    hdAnimSet_operationCC(screen->mpLayout->animSet(),1,2);
    gabi::call_ptr<void>(screen->mVtable->mCalcLayout.get(),screen);
    screen->startState(&sCoverWait);
    hdCover_getPlay()->mNextMode = 0;
    screen->mScale = 0.0f;
    return TRUE;
}
/* 026E896C */
void hdTelescopeCover_c::destroyLayout(hdTelescopeHud_c* screen) {
    gabi::call_ptr<void>(screen->mVtable->mDestroyLayout.get(),screen);
    hdCover_baseDestroyLayout(screen);
}
/* 026E89AC */
void hdTelescopeCover_c::calc(hdTelescopeHud_c* screen) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(screen->stateMachine());
    hdPartsStateIdVtbl_c* waitTable = sCoverWait.mVtable;
    u32 currentKey = current->getKey();
    u32 waitKey = gabi::call_ptr<u32>(waitTable->mGetKey.get(),&sCoverWait);
    if (currentKey != waitKey) hdCover_getPlay()->mSoundMode = 0;
    hdParts_executeStateMachine(screen->stateMachine());
    gabi::call_ptr<void>(screen->mVtable->mCalcLayout.get(),screen);
}
/* 026E8A5C */
BOOL hdTelescopeCover_c::loadLayout(hdTelescopeHud_c* screen,void* name,u32 arg1,u32 arg2,u32 arg3,void* desc) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdCover_allocateLayout(sizeof(hdPartsLayout_c),sCoverUiSystem->mHeap,4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    screen->mpLayout = layout;
    if (!layout || !layout->vLoad(name,arg1,arg2,arg3,7,4,desc)) return FALSE;
    for (u32 i=0; i<7; ++i) {
        u32 groupAddress = gabi::ea(&sCoverGroups) + u32(sCoverGroupIndices[i]) * sizeof(hdUiSafeString);
        hdCover_registerAnimation(screen->mpLayout->animSet(),i,&sCoverAnimations[i],hdPartsView<hdUiSafeString>(groupAddress));
    }
    return TRUE;
}
struct hdCoverStaticRegistration_l { be<u32> mWords[3]; };
struct CoverStaticWordsView_l {
    be<u32> mValues[4];
    be<u32>& operator[](u32 index) { return mValues[index]; }
};
static CoverStaticWordsView_l sCoverStaticWords;
typedef be<u32> hdCoverWord_l;
static hdCoverWord_l sCoverCounterReady;
static hdCoverWord_l sCoverCounter;
typedef be<f32> hdCoverFloat_l;
static hdCoverFloat_l sCoverMinimumAngle;
static hdCoverFloat_l sCoverMaximumAngle;
static u8 sCoverFirstLibraryStatic;
static u8 sCoverSecondLibraryStatic;
static hdCoverStaticRegistration_l sCoverWordsRegistration;
static hdCoverStaticRegistration_l sCoverFirstRegistration;
static hdCoverStaticRegistration_l sCoverSecondRegistration;
static hdUiStringTable sCoverStringVtable;
static hdPartsStateIdVtbl_c sCoverStateVtable;
static hdStateId sCoverNullState;
struct CoverInitializedStatesView_l {
    hdPartsStateId_c mValues[13];
    hdPartsStateId_c& operator[](u32 index) { return mValues[index]; }
};
static CoverInitializedStatesView_l sCoverInitializedStates;
void hdCover_registerGlobal(void*);
void hdCover_constructFirstStatic(void*);
void hdCover_constructSecondStatic(void*);
static hdStateId sCoverBlankState;
/* 026E8B74 */
void hdTelescopeCover_c::initializeStatics() {
    sCoverStaticWords[3] = 0;
    sCoverStaticWords[2] = 0;
    sCoverStaticWords[1] = 0;
    sCoverStaticWords[0] = 0;
    hdCover_registerGlobal(&sCoverWordsRegistration);
    sCoverMinimumAngle = -3.1415927410125732f;
    sCoverMaximumAngle = 3.1415927410125732f;
    hdCover_constructFirstStatic(&sCoverFirstLibraryStatic);
    hdCover_registerGlobal(&sCoverFirstRegistration);
    hdCover_constructSecondStatic(&sCoverSecondLibraryStatic);
    hdCover_registerGlobal(&sCoverSecondRegistration);
    sCoverArchive.mVtable = &sCoverStringVtable;
    sCoverArchive.mText.v = gabi::guest_addr("Telescope_00");
    sCoverGroups[0].mVtable = &sCoverStringVtable;
    sCoverGroups[0].mText.v = gabi::guest_addr("G_All_00");
    sCoverGroups[1].mVtable = &sCoverStringVtable;
    sCoverGroups[1].mText.v = gabi::guest_addr("G_Scale_00");
    sCoverGroups[2].mVtable = &sCoverStringVtable;
    sCoverGroups[2].mText.v = gabi::guest_addr("G_ChangeMode_00");
    sCoverGroups[3].mVtable = &sCoverStringVtable;
    sCoverGroups[3].mText.v = gabi::guest_addr("G_Cover_00");
    sCoverAnimations[0].mVtable = &sCoverStringVtable;
    sCoverAnimations[0].mText.v = gabi::guest_addr("InAll");
    sCoverAnimations[1].mVtable = &sCoverStringVtable;
    sCoverAnimations[1].mText.v = gabi::guest_addr("NormalDemoMode");
    sCoverAnimations[2].mVtable = &sCoverStringVtable;
    sCoverAnimations[2].mText.v = gabi::guest_addr("Scale");
    sCoverAnimations[3].mVtable = &sCoverStringVtable;
    sCoverAnimations[3].mText.v = gabi::guest_addr("DemoLookMode");
    sCoverAnimations[4].mVtable = &sCoverStringVtable;
    sCoverAnimations[4].mText.v = gabi::guest_addr("OutAll");
    sCoverAnimations[5].mVtable = &sCoverStringVtable;
    sCoverAnimations[5].mText.v = gabi::guest_addr("InCover");
    sCoverAnimations[6].mVtable = &sCoverStringVtable;
    sCoverAnimations[6].mText.v = gabi::guest_addr("OutCover");
    sCoverDescriptor.mVtable = &sCoverStringVtable;
    sCoverDescriptor.mText.v = gabi::guest_addr("Telescope_00");
    u32 number = 0;
    if (sCoverCounterReady) number = sCoverCounter;
    else sCoverCounterReady = 1;
    sCoverInitializedStates[0].setup(++number,"::StateID_Blank",&sCoverStateVtable,0x4,&sCoverBlankState);
    sCoverInitializedStates[1].setup(++number,"::StateID_InAll",&sCoverStateVtable,0x19,&sCoverNullState);
    sCoverInitializedStates[2].setup(++number,"::StateID_InCoverDemo",&sCoverStateVtable,0x1C,&sCoverNullState);
    sCoverInitializedStates[3].setup(++number,"::StateID_OutCoverDemo",&sCoverStateVtable,0x1F,&sCoverNullState);
    sCoverInitializedStates[4].setup(++number,"::StateID_Scale",&sCoverStateVtable,0x22,&sCoverNullState);
    sCoverInitializedStates[5].setup(++number,"::StateID_OutAll",&sCoverStateVtable,0x25,&sCoverNullState);
    sCoverInitializedStates[6].setup(++number,"::StateID_Open2",&sCoverStateVtable,0x28,&sCoverNullState);
    sCoverInitializedStates[7].setup(++number,"::StateID_Demo",&sCoverStateVtable,0x2B,&sCoverNullState);
    sCoverInitializedStates[8].setup(++number,"::StateID_Demo2",&sCoverStateVtable,0x2E,&sCoverNullState);
    sCoverInitializedStates[9].setup(++number,"::StateID_Demo3",&sCoverStateVtable,0x31,&sCoverNullState);
    sCoverInitializedStates[10].setup(++number,"::StateID_Demo4",&sCoverStateVtable,0x34,&sCoverNullState);
    sCoverInitializedStates[11].setup(++number,"::StateID_DemoEndWait",&sCoverStateVtable,0x37,&sCoverNullState);
    sCoverInitializedStates[12].setup(++number,"::StateID_Reset",&sCoverStateVtable,0x3A,&sCoverNullState);
    sCoverCounter = number;
}
/* WWHD glue (tools/verify/autoglue.py)
 str 101076E4 "Telescope_00" in 026E8B74
 str 10107694 "G_All_00" in 026E8B74
 str 101076A0 "G_Scale_00" in 026E8B74
 str 101075D4 "G_ChangeMode_00" in 026E8B74
 str 101076AC "G_Cover_00" in 026E8B74
 str 101075BC "InAll" in 026E8B74
 str 101076B8 "NormalDemoMode" in 026E8B74
 str 101075C4 "Scale" in 026E8B74
 str 101076C8 "DemoLookMode" in 026E8B74
 str 101075CC "OutAll" in 026E8B74
 str 101075B4 "InCover" in 026E8B74
 str 101076D8 "OutCover" in 026E8B74
 str 101076E4 "Telescope_00" in 026E8B74
 str 101075E4 "::StateID_Blank" in 026E8B74
 str 101075F4 "::StateID_InAll" in 026E8B74
 str 10107664 "::StateID_InCoverDemo" in 026E8B74
 str 101076F4 "::StateID_OutCoverDemo" in 026E8B74
 str 10107604 "::StateID_Scale" in 026E8B74
 str 1010770C "::StateID_OutAll" in 026E8B74
 str 10107614 "::StateID_Open2" in 026E8B74
 str 10107720 "::StateID_Demo" in 026E8B74
 str 10107624 "::StateID_Demo2" in 026E8B74
 str 10107634 "::StateID_Demo3" in 026E8B74
 str 10107644 "::StateID_Demo4" in 026E8B74
 str 1010767C "::StateID_DemoEndWait" in 026E8B74
 str 10107654 "::StateID_Reset" in 026E8B74
 data sCoverStaticWords 1049C8D8
 data sCoverMinimumAngle 1049C8BC
 data sCoverMaximumAngle 1049C8C0
 data sCoverFirstLibraryStatic 1049C8C4
 data sCoverSecondLibraryStatic 1049C8C5
 data sCoverWordsRegistration 101F6F58
 data sCoverFirstRegistration 101F6F64
 data sCoverSecondRegistration 101F6F70
 data sCoverStringVtable 1010746C
 data sCoverStateVtable 10107518
 data sCoverNullState 101FF32C
 data sCoverInitializedStates 1049C948
 data sCoverCounterReady 101FD8E4
 data sCoverCounter 101FDD50
 data sCoverBlankState 1049DD60
 ext 028F026C void hdCover_registerGlobal(void*)
 ext 028ED6F8 void hdCover_constructFirstStatic(void*)
 ext 028EAB2C void hdCover_constructSecondStatic(void*)
 data sCoverArchive 1049C8B4
 data sCoverDescriptor 1049CBB8
 data sCoverWait 1049C948
 data sCoverUiSystem 1018C404
 data sCoverGroups 1049C8E8
 data sCoverAnimations 1049C908
 data sCoverGroupIndices 101074B4

 ext 025200D4 hdCoverPlay_l* hdCover_getPlay()

 ext 0273B050 void* hdCover_allocateLayout(u32,hdScreenHeap*,u32)

 ext 02004E04 void hdCover_registerAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)

 ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString*,u32)
 ext 026F91F8 void hdParts_c::detachArchives()
*/

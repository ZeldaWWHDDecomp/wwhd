/* HD alternate map icon/pattern button states. */
#include "d/hd_btn_map_pattern.h"
extern void mapIconPlay(hdAnimSet*,u32,u32,f32);
extern void mapIconBaseDestroy(hdBasicButton_c*,u32);
extern void mapIconDelete(void*);
extern void* mapPatternAllocate(u32);
extern hdBasicButton_c* mapPatternBasicConstruct(hdBasicButton_c*);
extern u32 mapPatternBaseInitialize(hdBasicButton_c*,hdUiSafeString*,u32,u32,u32);
struct MapPatternVtableWords_c { be<u32> mWords[2]; };
static MapPatternVtableWords_c sMapPatternVtable;
static MapPatternVtableWords_c sMapPatternStateVtable;
static MapPatternVtableWords_c sMapPatternOwnerVtable;
static hdUiSafeString sMapPatternLayoutName;
/* 0263B4A8 */
void hdBtnMapPattern_c::destroy(hdBtnMapPattern_c* icon,u32 flags) {
    if (!icon) return;
    icon->mButton.mStateHolder.mHolderVtable=sMapPatternStateVtable.mWords;
    icon->mButton.mSecondaryVtable=sMapPatternOwnerVtable.mWords;
    mapIconBaseDestroy(icon,0);
    if (flags&1) mapIconDelete(icon);
}
/* 0263B514 */
void hdBtnMapPattern_c::iconEnter() { mapIconPlay(mButton.animSet(),7,2,1.0f); }
/* 0263B530 */
void hdBtnMapPattern_c::iconMove() { mButton.vCalcLayout(); }
/* 0263B540 */
void hdBtnMapPattern_c::patternEnter() { mapIconPlay(mButton.animSet(),8,3,1.0f); }
/* 0263B55C */
void hdBtnMapPattern_c::patternMove() { mButton.vCalcLayout(); }
/* 0263B56C */
hdBtnMapPattern_c* hdBtnMapPattern_c::construct(hdBtnMapPattern_c* icon) {
    if (!icon) icon=(hdBtnMapPattern_c*)mapPatternAllocate(sizeof(hdBtnMapPattern_c));
    if (!icon) return icon;
    mapPatternBasicConstruct(icon);
    icon->mButton.mVtable=reinterpret_cast<hdPartsVtbl_c*>(sMapPatternVtable.mWords);
    icon->mButton.mStateHolder.mHolderVtable=sMapPatternStateVtable.mWords;
    icon->mButton.mSecondaryVtable=sMapPatternOwnerVtable.mWords;
    return icon;
}
/* 0263B5D8 */
u32 hdBtnMapPattern_c::initialize(u32 first,u32 second,u32 third) {
    return mapPatternBaseInitialize(this,&sMapPatternLayoutName,first,second,third);
}
extern void mapIconSetFrame(hdAnimSet*,u32,u32,f32);
struct hdMapPatternIsland_c { be<s8> mIsland; };
extern hdMapPatternIsland_c* mapPatternFindIsland(HdMapLocations*,u32);
static HdMapManagerPointer sMapPatternManager;
/* 0263B708 */
void hdBtnMapPattern_c::setIcon(u32 kind,u32 cell) {
    if (kind-5<=4) {
        hdMapPatternIsland_c* island=mapPatternFindIsland(sMapPatternManager.mpManager->mpLocations,cell);
        if (island) {
            s32 index=(s32)island->mIsland-1;
            if ((u32)index<49) mapIconSetFrame(mButton.animSet(),8,3,(f32)index);
        }
    } else {
        mapIconSetFrame(mButton.animSet(),8,2,0.0f);
    }
    mapIconSetFrame(mButton.animSet(),7,2,(f32)(s32)kind);
}
extern hdPartsLayout_c* mapIconAllocateLayout(u32,void*,u32);
extern void mapIconBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*);
static hdMapLayoutHeapPointer_c sMapPatternLayoutHeap;
struct sMapPatternLayoutIndices_c { be<u32> mEntries[2]; };
static sMapPatternLayoutIndices_c sMapPatternLayoutIndices;
struct sMapPatternLayoutGroups_c { hdUiSafeString mEntries[2]; };
static sMapPatternLayoutGroups_c sMapPatternLayoutGroups;
struct sMapPatternLayoutAnimations_c { hdUiSafeString mEntries[2]; };
static sMapPatternLayoutAnimations_c sMapPatternLayoutAnimations;
/* 0263B5F0 */
u32 hdBtnMapPattern_c::createLayout(void* name,u32 first,u32 second,u32 third,void* descriptor) {
    hdPartsLayout_c* layout=mapIconAllocateLayout(sizeof(hdPartsLayout_c),sMapPatternLayoutHeap.mpContext->mpHeap,4);
    if (layout) layout=hdPartsLayout_c::construct(layout);
    mButton.mpLayout=layout;
    if (!layout || !layout->vLoad(name,first,second,third,9,4,descriptor)) return 0;
    for (u32 i=0;i<2;++i)
        mapIconBindAnimation(mButton.animSet(),i+7,&sMapPatternLayoutGroups.mEntries[i],&sMapPatternLayoutAnimations.mEntries[sMapPatternLayoutIndices.mEntries[i]]);
    return 1;
}
struct MapPatternStaticList_c { be<u32> mWords[4]; };
struct MapPatternRegistration_c { be<u32> mWords[3]; };
struct MapPatternStaticFlag_c { be<u8> mValue; };
struct MapPatternStaticBounds_c { be<f32> mMinimumAngle,mMaximumAngle; HdMapPoint mWorldHalf,mExtents; };
struct MapPatternCounter_c { be<u32> mValue; };
static MapPatternStaticList_c sMapPatternStaticList;
static MapPatternRegistration_c sMapPatternRegistrationList;
static MapPatternRegistration_c sMapPatternRegistrationFirstFlag;
static MapPatternRegistration_c sMapPatternRegistrationSecondFlag;
static MapPatternStaticFlag_c sMapPatternFlagFirst;
static MapPatternStaticFlag_c sMapPatternFlagSecond;
static MapPatternStaticBounds_c sMapPatternStaticBounds;
static hdUiStringTable sMapPatternStaticStringVtable;
static hdPartsStateIdVtbl_c sMapPatternStaticStateVtable;
static hdStateId sMapPatternStaticNullState;
static MapPatternCounter_c sMapPatternCounterGuard;
static MapPatternCounter_c sMapPatternCounter;
extern void mapCountRegister(void*);
extern void mapCountConstructFirstFlag(void*);
extern void mapCountConstructSecondFlag(void*);
static hdPartsStateId_c sMapPatternStateIconChange;
static hdPartsStateId_c sMapPatternStateMapPattern;
/* 0263B808 */
void hdBtnMapPattern_c::initializeStatics() {
    sMapPatternStaticList.mWords[3]=0; sMapPatternStaticList.mWords[2]=0;
    sMapPatternStaticList.mWords[1]=0; sMapPatternStaticList.mWords[0]=0;
    mapCountRegister(&sMapPatternRegistrationList);
    sMapPatternStaticBounds.mMinimumAngle=-3.1415927410125732f;
    sMapPatternStaticBounds.mMaximumAngle=3.1415927410125732f;
    mapCountConstructFirstFlag(&sMapPatternFlagFirst);
    mapCountRegister(&sMapPatternRegistrationFirstFlag);
    mapCountConstructSecondFlag(&sMapPatternFlagSecond);
    mapCountRegister(&sMapPatternRegistrationSecondFlag);
    sMapPatternStaticBounds.mWorldHalf.x=50000.0f; sMapPatternStaticBounds.mWorldHalf.y=50000.0f;
    sMapPatternStaticBounds.mExtents.x=10000.0f; sMapPatternStaticBounds.mExtents.y=10000.0f;
    sMapPatternLayoutName.mVtable=&sMapPatternStaticStringVtable;
    sMapPatternLayoutName.mText.v=gabi::guest_addr("BtnMapIcon_01");
    sMapPatternLayoutAnimations.mEntries[0].mVtable=&sMapPatternStaticStringVtable;
    sMapPatternLayoutAnimations.mEntries[0].mText.v=gabi::guest_addr("G_Btn_00");
    sMapPatternLayoutAnimations.mEntries[1].mVtable=&sMapPatternStaticStringVtable;
    sMapPatternLayoutAnimations.mEntries[1].mText.v=gabi::guest_addr("G_MapPattern_00");
    sMapPatternLayoutGroups.mEntries[0].mVtable=&sMapPatternStaticStringVtable;
    sMapPatternLayoutGroups.mEntries[0].mText.v=gabi::guest_addr("IconChange");
    sMapPatternLayoutGroups.mEntries[1].mVtable=&sMapPatternStaticStringVtable;
    sMapPatternLayoutGroups.mEntries[1].mText.v=gabi::guest_addr("MapPattern");
    u32 number;
    if (sMapPatternCounterGuard.mValue) number=sMapPatternCounter.mValue;
    else { sMapPatternCounterGuard.mValue=1; number=0; }
    sMapPatternStateIconChange.setup(++number,"::StateID_IconChange",&sMapPatternStaticStateVtable,75,&sMapPatternStaticNullState);
    sMapPatternStateMapPattern.setup(++number,"::StateID_MapPattern",&sMapPatternStaticStateVtable,78,&sMapPatternStaticNullState);
    sMapPatternCounter.mValue=number;
}
/* WWHD glue
data sMapPatternStaticList 1049054C
data sMapPatternRegistrationList 101F55C8
data sMapPatternRegistrationFirstFlag 101F55D4
data sMapPatternRegistrationSecondFlag 101F55E0
data sMapPatternFlagFirst 10490548
data sMapPatternFlagSecond 10490549
data sMapPatternStaticBounds 10490530
data sMapPatternStaticStringVtable 100E9EAC
data sMapPatternStaticStateVtable 100E9F0C
data sMapPatternStaticNullState 101FF32C
data sMapPatternCounterGuard 101FD8E4
data sMapPatternCounter 101FDD50
str 100E9F8C "BtnMapIcon_01" in 0263B808
str 100E9F9C "G_Btn_00" in 0263B808
str 100E9F7C "G_MapPattern_00" in 0263B808
str 100E9FA8 "IconChange" in 0263B808
str 100E9FB4 "MapPattern" in 0263B808
data sMapPatternStateIconChange 1049057C
str 100E9FC0 "::StateID_IconChange" in 0263B808
data sMapPatternStateMapPattern 104905AC
str 100E9FD8 "::StateID_MapPattern" in 0263B808
data sMapPatternLayoutHeap 1018C404
data sMapPatternLayoutIndices 100E9E90
data sMapPatternLayoutGroups 1049056C
data sMapPatternLayoutAnimations 1049055C
ext 02687A64 hdMapPatternIsland_c* mapPatternFindIsland(HdMapLocations*,u32)
data sMapPatternManager 101F8344
ext 0273AD10 void* mapPatternAllocate(u32)
ext 026F1C3C hdBasicButton_c* mapPatternBasicConstruct(hdBasicButton_c*)
ext 026F1D04 u32 mapPatternBaseInitialize(hdBasicButton_c*,hdUiSafeString*,u32,u32,u32)
data sMapPatternVtable 100E9FF0
data sMapPatternStateVtable 100EA278
data sMapPatternOwnerVtable 100EA288
data sMapPatternLayoutName 104905DC
*/

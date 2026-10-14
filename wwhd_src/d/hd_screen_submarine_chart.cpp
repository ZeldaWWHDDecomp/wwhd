/** Sea-chart marker screen for the submarine positions (layout Submarine_00). */
#include "d/hd_screen_submarine_chart.h"
using namespace gabi;
using ChartStrings6 = hdChartStrings_l<6>;
using ChartStrings4 = hdChartStrings_l<4>;
using ChartStrings3 = hdChartStrings_l<3>;
using ChartStrings1 = hdChartStrings_l<1>;
using ChartStrings2 = hdChartStrings_l<2>;
using ChartIndices2 = hdChartIndices_l<2>;
using ChartUiSystemPointer = gptr<hdScreenUiSystem>;
using ChartFlag = be<u8>;
using ChartFloat = be<f32>;
static ChartStrings6 sSubmarinePaneNames;
static ChartStrings4 sSubmarineArchiveNames;
static hdUiSafeString sSubmarineLayoutName;
static ChartStrings3 sSubmarineChildNames;
static ChartStrings3 sSubmarineChildTypes;
static ChartStrings1 sSubmarineAnimationGroups;
static ChartStrings2 sSubmarineAnimationNames;
static hdUiStringTable sSubmarineStringVtable;
static hdPartsVtbl_c sSubmarinePartVtable;
static u8 sSubmarineHolderVtable;
static u8 sSubmarineSecondaryVtable;
static ChartIndices2 sSubmarineAnimationGroupIndices;
static ChartUiSystemPointer sSubmarineUiSystem;
static hdLayoutRegistration_c sSubmarineRegistration;
static ChartFlag sSubmarinePrimaryLock;
static hdLayoutRegistration_c sSubmarinePrimaryLockRegistration;
static ChartFlag sSubmarineSecondaryLock;
static hdLayoutRegistration_c sSubmarineSecondaryLockRegistration;
static ChartFloat sSubmarineMinimumAngle;
static ChartFloat sSubmarineMinimumAngleSeed;
static ChartFloat sSubmarineMaximumAngle;
static ChartFloat sSubmarineMaximumAngleSeed;
static ChartFloat sSubmarineWorldExtentX;
static ChartFloat sSubmarineWorldExtentSeed;
static ChartFloat sSubmarineWorldExtentY;
static ChartFloat sSubmarineZoomLimitX;
static ChartFloat sSubmarineZoomLimitSeed;
static ChartFloat sSubmarineZoomLimitY;
static hdChartRegistrationList_l sSubmarineRegistrationList;
extern void* submarineChartAllocate(u32);
extern void submarineChartDelete(void*);
extern void submarineChartDestroyLinkPos(hdSeaChartLinkPos_c*,u32);
extern void submarineChartConstructLinkPos(hdSeaChartLinkPos_c*);
extern void submarineChartConfigureAnimations(hdSeaChartLinkPos_c*,u32,u32,u32,u32);
extern void submarineChartSetupLayout(hdSeaChartLinkPos_c*);
extern void submarineChartDestroyLinkPosLayout(hdSeaChartLinkPos_c*);
extern void submarineChartDetachArchives(hdParts_c*);
extern void submarineChartAttachArchives(hdParts_c*,hdUiSafeString*,u32);
extern u32 submarineChartCreateChild(hdParts_c*,u32,hdPartsDesc_c*);
extern void submarineChartBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*);
extern void submarineChartRegister(hdLayoutRegistration_c*);
extern void submarineChartConstructPrimaryLock(be<u8>*);
extern void submarineChartConstructSecondaryLock(be<u8>*);
/* 026E5C94 */
// hdSubmarineChart_c::~hdSubmarineChart_c() (GHS deleting destructor: restores the base vtables, frees when flags & 1)
void hdSubmarineChart_c::destroy(hdSubmarineChart_c* chart, u32 flags) {
    if (!chart) return;
    chart->mStateHolder.mHolderVtable = &sSubmarineHolderVtable;
    chart->mSecondaryVtable = &sSubmarineSecondaryVtable;
    submarineChartDestroyLinkPos(chart,0);
    if (flags & 1) submarineChartDelete(chart);
}
/* 026E5D00 */
hdSubmarineChart_c* hdSubmarineChart_c::construct(hdSubmarineChart_c* part) {
    if (!part) part = static_cast<hdSubmarineChart_c*>(submarineChartAllocate(sizeof(hdSubmarineChart_c)));
    if (part) {
        submarineChartConstructLinkPos(part);
        part->mVtable = &sSubmarinePartVtable;
        part->mStateHolder.mHolderVtable = &sSubmarineHolderVtable;
        part->mSecondaryVtable = &sSubmarineSecondaryVtable;
    }
    return part;
}
/* 026E5D6C */
void hdSubmarineChart_c::initialize() {
    mPaneNameCount = 6;
    mPaneNames = sSubmarinePaneNames.mValues;
    submarineChartConfigureAnimations(this,0,1,0,1);
    submarineChartSetupLayout(this);
}
/* 026E5DC4 */
u32 hdSubmarineChart_c::create(u32 first,u32 second,u32 third) {
    submarineChartAttachArchives(this,sSubmarineArchiveNames.mValues,4);
    u32 target = mVtable->mLoad.get();
    void* resource = mpResAccessor.get();
    if (!call_ptr<u32>(target,this,&sSubmarineLayoutName,resource,first,second,third)) return 0;
    if (!call_ptr<u32>(mVtable->mCreateChildren.get(),this)) return 0;
    initialize();
    return 1;
}
/* 026E5EA0 */
void hdSubmarineChart_c::destroyLayout() {
    submarineChartDestroyLinkPosLayout(this);
    submarineChartDetachArchives(this);
}
/* 026E5ED4 */
void hdSubmarineChart_c::executeState() {
    hdParts_executeStateMachine(stateMachine());
}
/* 026E5EDC */
u32 hdSubmarineChart_c::createChild(hdPartsDesc_c* descriptor) {
    hdUiSafeString* actual=&hdChartChildName(descriptor)->mName;
    for (u32 i=0;i<3;++i) {
        actual->assureTermination();
        actual->assureTermination();
        hdUiSafeString* expected=&sSubmarineChildNames.mValues[i];
        be<u8>* saved=actual->mText.get();
        expected->assureTermination();
        bool matches=ea(saved)==ea(expected->mText.get());
        if (!matches) {
            be<u8>* left=actual->mText.get();
            be<u8>* right=expected->mText.get();
            for (u32 count=0;count<0x40001;++count) {
                u32 a=*left++; u32 b=*right++;
                if (a!=b) break;
                if (!a) { matches=true; break; }
            }
        }
        if (matches) return submarineChartCreateChild(this,i,descriptor);
    }
    return 1;
}
/* 026E5FE4 */
u32 hdSubmarineChart_c::createLayout(void* name,u32 resource,u32 first,u32 second,void* descriptor) {
    auto* layout=static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c),sSubmarineUiSystem->mHeap.get(),4));
    if (layout) layout=hdPartsLayout_c::construct(layout);
    mpLayout=layout;
    if (!layout || !layout->vLoad(name,resource,first,second,2,1,descriptor)) return 0;
    for (u32 i=0;i<2;++i) {
        hdPartsLayout_c* current=mpLayout.get();
        u32 group=sSubmarineAnimationGroupIndices.mValues[i];
        hdAnimSet* set=current->animSet();
        submarineChartBindAnimation(set,i,&sSubmarineAnimationNames.mValues[i],&sSubmarineAnimationGroups.mValues[group]);
    }
    vCalcLayout();
    return 1;
}
/* 026E6110 */
void hdSubmarineChart_c::initializeStatics() {
    sSubmarineRegistrationList.mCapacity = 0;
    sSubmarineRegistrationList.mCount = 0;
    sSubmarineRegistrationList.mTail = 0;
    sSubmarineRegistrationList.mHead = 0;
    submarineChartRegister(&sSubmarineRegistration);
    sSubmarineMinimumAngle = sSubmarineMinimumAngleSeed;
    sSubmarineMaximumAngle = sSubmarineMaximumAngleSeed;
    submarineChartConstructPrimaryLock(&sSubmarinePrimaryLock);
    submarineChartRegister(&sSubmarinePrimaryLockRegistration);
    submarineChartConstructSecondaryLock(&sSubmarineSecondaryLock);
    submarineChartRegister(&sSubmarineSecondaryLockRegistration);
    sSubmarineWorldExtentX = sSubmarineWorldExtentSeed;
    sSubmarineWorldExtentY = sSubmarineWorldExtentSeed;
    sSubmarinePaneNames.mValues[0].mVtable = &sSubmarineStringVtable;
    sSubmarinePaneNames.mValues[0].mText = at<be<u8>>(guest_addr("N_All_01"));
    sSubmarinePaneNames.mValues[1].mVtable = &sSubmarineStringVtable;
    sSubmarineZoomLimitX = sSubmarineZoomLimitSeed;
    sSubmarinePaneNames.mValues[1].mText = at<be<u8>>(guest_addr("N_Grid_00"));
    sSubmarinePaneNames.mValues[2].mVtable = &sSubmarineStringVtable;
    sSubmarinePaneNames.mValues[2].mText = at<be<u8>>(guest_addr("N_Grid_01"));
    sSubmarinePaneNames.mValues[3].mVtable = &sSubmarineStringVtable;
    sSubmarinePaneNames.mValues[3].mText = at<be<u8>>(guest_addr("L_MapAreaNow_00"));
    sSubmarinePaneNames.mValues[4].mVtable = &sSubmarineStringVtable;
    sSubmarinePaneNames.mValues[4].mText = at<be<u8>>(guest_addr("L_BtnBack_00"));
    sSubmarineAnimationGroups.mValues[0].mVtable = &sSubmarineStringVtable;
    sSubmarineAnimationGroups.mValues[0].mText = at<be<u8>>(guest_addr("G_All_00"));
    sSubmarineZoomLimitY = sSubmarineZoomLimitSeed;
    sSubmarineChildTypes.mValues[1].mText = at<be<u8>>(guest_addr("BtnBack_00"));
    sSubmarineChildNames.mValues[0].mVtable = &sSubmarineStringVtable;
    sSubmarineChildNames.mValues[0].mText = at<be<u8>>(guest_addr("L_MapAreaNow_00"));
    sSubmarineAnimationNames.mValues[0].mVtable = &sSubmarineStringVtable;
    sSubmarineAnimationNames.mValues[0].mText = at<be<u8>>(guest_addr("InAll"));
    sSubmarineChildNames.mValues[1].mVtable = &sSubmarineStringVtable;
    sSubmarineAnimationNames.mValues[1].mVtable = &sSubmarineStringVtable;
    sSubmarineAnimationNames.mValues[1].mText = at<be<u8>>(guest_addr("OutAll"));
    sSubmarineChildTypes.mValues[0].mVtable = &sSubmarineStringVtable;
    sSubmarineChildNames.mValues[1].mText = at<be<u8>>(guest_addr("L_BtnBack_00"));
    sSubmarinePaneNames.mValues[5].mVtable = &sSubmarineStringVtable;
    sSubmarinePaneNames.mValues[5].mText = at<be<u8>>(guest_addr("L_LinkPos_00"));
    sSubmarineChildTypes.mValues[0].mText = at<be<u8>>(guest_addr("MapAreaNow_00"));
    sSubmarineLayoutName.mVtable = &sSubmarineStringVtable;
    sSubmarineChildTypes.mValues[2].mText = at<be<u8>>(guest_addr("LinkPos_00"));
    sSubmarineChildNames.mValues[2].mVtable = &sSubmarineStringVtable;
    sSubmarineChildNames.mValues[2].mText = at<be<u8>>(guest_addr("L_LinkPos_00"));
    sSubmarineArchiveNames.mValues[0].mVtable = &sSubmarineStringVtable;
    sSubmarineArchiveNames.mValues[0].mText = at<be<u8>>(guest_addr("Submarine_00"));
    sSubmarineArchiveNames.mValues[1].mVtable = &sSubmarineStringVtable;
    sSubmarineArchiveNames.mValues[1].mText = at<be<u8>>(guest_addr("MapAreaNow_00"));
    sSubmarineArchiveNames.mValues[2].mVtable = &sSubmarineStringVtable;
    sSubmarineArchiveNames.mValues[2].mText = at<be<u8>>(guest_addr("BtnBack_00"));
    sSubmarineArchiveNames.mValues[3].mVtable = &sSubmarineStringVtable;
    sSubmarineLayoutName.mText = at<be<u8>>(guest_addr("Submarine_00"));
    sSubmarineChildTypes.mValues[2].mVtable = &sSubmarineStringVtable;
    sSubmarineChildTypes.mValues[1].mVtable = &sSubmarineStringVtable;
    sSubmarineArchiveNames.mValues[3].mText = at<be<u8>>(guest_addr("LinkPos_00"));
}
/* 026E5B8C */
u32 hdSubmarineChart_c::isInteractive() { return 0; }
/* 026E5B94 */
u32 hdSubmarineChart_c::visitorWord() { return mVisitorWord; }
/* WWHD glue (tools/verify/autoglue.py)
ext 0273AD10 void* submarineChartAllocate(u32)
ext 0273AF40 void submarineChartDelete(void*)
ext 02683938 void submarineChartDestroyLinkPos(hdSeaChartLinkPos_c*,u32)
ext 026842D4 void submarineChartConstructLinkPos(hdSeaChartLinkPos_c*)
ext 02684AB4 void submarineChartConfigureAnimations(hdSeaChartLinkPos_c*,u32,u32,u32,u32)
ext 026847F4 void submarineChartSetupLayout(hdSeaChartLinkPos_c*)
ext 026844C0 void submarineChartDestroyLinkPosLayout(hdSeaChartLinkPos_c*)
ext 026F91F8 void submarineChartDetachArchives(hdParts_c*)
ext 026F90D8 void submarineChartAttachArchives(hdParts_c*,hdUiSafeString*,u32)
ext 026F92C0 u32 submarineChartCreateChild(hdParts_c*,u32,hdPartsDesc_c*)
ext 02004E04 void submarineChartBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 028F026C void submarineChartRegister(hdLayoutRegistration_c*)
ext 028ED6F8 void submarineChartConstructPrimaryLock(be<u8>*)
ext 028EAB2C void submarineChartConstructSecondaryLock(be<u8>*)
data sSubmarinePaneNames 1049C608
data sSubmarineArchiveNames 1049C678
data sSubmarineLayoutName 1049C698
data sSubmarineChildNames 1049C648
data sSubmarineChildTypes 1049C660
data sSubmarineAnimationGroups 1049C5D4
data sSubmarineAnimationNames 1049C638
data sSubmarineStringVtable 10106E1C
data sSubmarinePartVtable 10106EF8
data sSubmarineHolderVtable 10106FF8
data sSubmarineSecondaryVtable 10107008
data sSubmarineAnimationGroupIndices 10106E00
data sSubmarineUiSystem 1018C404
data sSubmarineRegistration 101F6EF8
data sSubmarinePrimaryLock 1049C5F4
data sSubmarinePrimaryLockRegistration 101F6F04
data sSubmarineSecondaryLock 1049C5F5
data sSubmarineSecondaryLockRegistration 101F6F10
data sSubmarineMinimumAngle 1049C5DC
data sSubmarineMinimumAngleSeed 10106E3C
data sSubmarineMaximumAngle 1049C5E0
data sSubmarineMaximumAngleSeed 10106E40
data sSubmarineWorldExtentX 1049C5E4
data sSubmarineWorldExtentSeed 10106E44
data sSubmarineWorldExtentY 1049C5E8
data sSubmarineZoomLimitX 1049C5F0
data sSubmarineZoomLimitSeed 10106E48
data sSubmarineZoomLimitY 1049C5EC
data sSubmarineRegistrationList 1049C5F8
str 10106E94 "N_All_01" in 026E6110
str 10106E6C "N_Grid_00" in 026E6110
str 10106E78 "N_Grid_01" in 026E6110
str 10106E5C "L_MapAreaNow_00" in 026E6110
str 10106EA0 "L_BtnBack_00" in 026E6110
str 10106EC0 "G_All_00" in 026E6110
str 10106ECC "BtnBack_00" in 026E6110
str 10106E4C "InAll" in 026E6110
str 10106E54 "OutAll" in 026E6110
str 10106EB0 "L_LinkPos_00" in 026E6110
str 10106E84 "MapAreaNow_00" in 026E6110
str 10106ED8 "LinkPos_00" in 026E6110
str 10106EE4 "Submarine_00" in 026E6110
*/

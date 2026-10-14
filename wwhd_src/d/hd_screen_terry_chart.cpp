/** Sea-chart marker screen for the Beedle's (Terry) shop ship positions (layout Terry_00). */
#include "d/hd_screen_terry_chart.h"
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
static ChartStrings6 sTerryPaneNames;
static ChartStrings4 sTerryArchiveNames;
static hdUiSafeString sTerryLayoutName;
static ChartStrings3 sTerryChildNames;
static ChartStrings3 sTerryChildTypes;
static ChartStrings1 sTerryAnimationGroups;
static ChartStrings2 sTerryAnimationNames;
static hdUiStringTable sTerryStringVtable;
static hdPartsVtbl_c sTerryPartVtable;
static u8 sTerryHolderVtable;
static u8 sTerrySecondaryVtable;
static ChartIndices2 sTerryAnimationGroupIndices;
static ChartUiSystemPointer sTerryUiSystem;
static hdLayoutRegistration_c sTerryRegistration;
static ChartFlag sTerryPrimaryLock;
static hdLayoutRegistration_c sTerryPrimaryLockRegistration;
static ChartFlag sTerrySecondaryLock;
static hdLayoutRegistration_c sTerrySecondaryLockRegistration;
static ChartFloat sTerryMinimumAngle;
static ChartFloat sTerryMinimumAngleSeed;
static ChartFloat sTerryMaximumAngle;
static ChartFloat sTerryMaximumAngleSeed;
static ChartFloat sTerryWorldExtentX;
static ChartFloat sTerryWorldExtentSeed;
static ChartFloat sTerryWorldExtentY;
static ChartFloat sTerryZoomLimitX;
static ChartFloat sTerryZoomLimitSeed;
static ChartFloat sTerryZoomLimitY;
static hdChartRegistrationList_l sTerryRegistrationList;
extern void* terryChartAllocate(u32);
extern void terryChartDelete(void*);
extern void terryChartDestroyLinkPos(hdSeaChartLinkPos_c*,u32);
extern void terryChartConstructLinkPos(hdSeaChartLinkPos_c*);
extern void terryChartConfigureAnimations(hdSeaChartLinkPos_c*,u32,u32,u32,u32);
extern void terryChartSetupLayout(hdSeaChartLinkPos_c*);
extern void terryChartDestroyLinkPosLayout(hdSeaChartLinkPos_c*);
extern void terryChartDetachArchives(hdParts_c*);
extern void terryChartAttachArchives(hdParts_c*,hdUiSafeString*,u32);
extern u32 terryChartCreateChild(hdParts_c*,u32,hdPartsDesc_c*);
extern void terryChartBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*);
extern void terryChartRegister(hdLayoutRegistration_c*);
extern void terryChartConstructPrimaryLock(be<u8>*);
extern void terryChartConstructSecondaryLock(be<u8>*);
/* 026E938C */
// hdTerryChart_c::~hdTerryChart_c() (GHS deleting destructor: restores the base vtables, frees when flags & 1)
void hdTerryChart_c::destroy(hdTerryChart_c* chart, u32 flags) {
    if (!chart) return;
    chart->mStateHolder.mHolderVtable = &sTerryHolderVtable;
    chart->mSecondaryVtable = &sTerrySecondaryVtable;
    terryChartDestroyLinkPos(chart,0);
    if (flags & 1) terryChartDelete(chart);
}
/* 026E93F8 */
hdTerryChart_c* hdTerryChart_c::construct(hdTerryChart_c* part) {
    if (!part) part = static_cast<hdTerryChart_c*>(terryChartAllocate(sizeof(hdTerryChart_c)));
    if (part) {
        terryChartConstructLinkPos(part);
        part->mVtable = &sTerryPartVtable;
        part->mStateHolder.mHolderVtable = &sTerryHolderVtable;
        part->mSecondaryVtable = &sTerrySecondaryVtable;
    }
    return part;
}
/* 026E9464 */
void hdTerryChart_c::initialize() {
    mPaneNameCount = 6;
    mPaneNames = sTerryPaneNames.mValues;
    terryChartConfigureAnimations(this,0,1,0,1);
    terryChartSetupLayout(this);
}
/* 026E94BC */
u32 hdTerryChart_c::create(u32 first,u32 second,u32 third) {
    terryChartAttachArchives(this,sTerryArchiveNames.mValues,4);
    u32 target = mVtable->mLoad.get();
    void* resource = mpResAccessor.get();
    if (!call_ptr<u32>(target,this,&sTerryLayoutName,resource,first,second,third)) return 0;
    if (!call_ptr<u32>(mVtable->mCreateChildren.get(),this)) return 0;
    initialize();
    return 1;
}
/* 026E9598 */
void hdTerryChart_c::destroyLayout() {
    terryChartDestroyLinkPosLayout(this);
    terryChartDetachArchives(this);
}
/* 026E95CC */
void hdTerryChart_c::executeState() {
    hdParts_executeStateMachine(stateMachine());
}
/* 026E95D4 */
u32 hdTerryChart_c::createChild(hdPartsDesc_c* descriptor) {
    hdUiSafeString* actual=&hdChartChildName(descriptor)->mName;
    for (u32 i=0;i<3;++i) {
        actual->assureTermination();
        actual->assureTermination();
        hdUiSafeString* expected=&sTerryChildNames.mValues[i];
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
        if (matches) return terryChartCreateChild(this,i,descriptor);
    }
    return 1;
}
/* 026E96DC */
u32 hdTerryChart_c::createLayout(void* name,u32 resource,u32 first,u32 second,void* descriptor) {
    auto* layout=static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c),sTerryUiSystem->mHeap.get(),4));
    if (layout) layout=hdPartsLayout_c::construct(layout);
    mpLayout=layout;
    if (!layout || !layout->vLoad(name,resource,first,second,2,1,descriptor)) return 0;
    for (u32 i=0;i<2;++i) {
        hdPartsLayout_c* current=mpLayout.get();
        u32 group=sTerryAnimationGroupIndices.mValues[i];
        hdAnimSet* set=current->animSet();
        terryChartBindAnimation(set,i,&sTerryAnimationNames.mValues[i],&sTerryAnimationGroups.mValues[group]);
    }
    vCalcLayout();
    return 1;
}
/* 026E9808 */
void hdTerryChart_c::initializeStatics() {
    sTerryRegistrationList.mCapacity = 0;
    sTerryRegistrationList.mCount = 0;
    sTerryRegistrationList.mTail = 0;
    sTerryRegistrationList.mHead = 0;
    terryChartRegister(&sTerryRegistration);
    sTerryMinimumAngle = sTerryMinimumAngleSeed;
    sTerryMaximumAngle = sTerryMaximumAngleSeed;
    terryChartConstructPrimaryLock(&sTerryPrimaryLock);
    terryChartRegister(&sTerryPrimaryLockRegistration);
    terryChartConstructSecondaryLock(&sTerrySecondaryLock);
    terryChartRegister(&sTerrySecondaryLockRegistration);
    sTerryWorldExtentX = sTerryWorldExtentSeed;
    sTerryWorldExtentY = sTerryWorldExtentSeed;
    sTerryPaneNames.mValues[0].mVtable = &sTerryStringVtable;
    sTerryPaneNames.mValues[0].mText = at<be<u8>>(guest_addr("N_All_01"));
    sTerryPaneNames.mValues[1].mVtable = &sTerryStringVtable;
    sTerryZoomLimitX = sTerryZoomLimitSeed;
    sTerryPaneNames.mValues[1].mText = at<be<u8>>(guest_addr("N_Grid_00"));
    sTerryPaneNames.mValues[2].mVtable = &sTerryStringVtable;
    sTerryPaneNames.mValues[2].mText = at<be<u8>>(guest_addr("N_Grid_01"));
    sTerryPaneNames.mValues[3].mVtable = &sTerryStringVtable;
    sTerryPaneNames.mValues[3].mText = at<be<u8>>(guest_addr("L_MapAreaNow_00"));
    sTerryPaneNames.mValues[4].mVtable = &sTerryStringVtable;
    sTerryPaneNames.mValues[4].mText = at<be<u8>>(guest_addr("L_BtnBack_00"));
    sTerryAnimationGroups.mValues[0].mVtable = &sTerryStringVtable;
    sTerryAnimationGroups.mValues[0].mText = at<be<u8>>(guest_addr("G_All_00"));
    sTerryZoomLimitY = sTerryZoomLimitSeed;
    sTerryChildTypes.mValues[1].mText = at<be<u8>>(guest_addr("BtnBack_00"));
    sTerryChildNames.mValues[0].mVtable = &sTerryStringVtable;
    sTerryChildNames.mValues[0].mText = at<be<u8>>(guest_addr("L_MapAreaNow_00"));
    sTerryAnimationNames.mValues[0].mVtable = &sTerryStringVtable;
    sTerryAnimationNames.mValues[0].mText = at<be<u8>>(guest_addr("InAll"));
    sTerryChildNames.mValues[1].mVtable = &sTerryStringVtable;
    sTerryAnimationNames.mValues[1].mVtable = &sTerryStringVtable;
    sTerryAnimationNames.mValues[1].mText = at<be<u8>>(guest_addr("OutAll"));
    sTerryChildTypes.mValues[0].mVtable = &sTerryStringVtable;
    sTerryChildNames.mValues[1].mText = at<be<u8>>(guest_addr("L_BtnBack_00"));
    sTerryPaneNames.mValues[5].mVtable = &sTerryStringVtable;
    sTerryPaneNames.mValues[5].mText = at<be<u8>>(guest_addr("L_LinkPos_00"));
    sTerryChildTypes.mValues[0].mText = at<be<u8>>(guest_addr("MapAreaNow_00"));
    sTerryLayoutName.mVtable = &sTerryStringVtable;
    sTerryChildTypes.mValues[2].mText = at<be<u8>>(guest_addr("LinkPos_00"));
    sTerryChildNames.mValues[2].mVtable = &sTerryStringVtable;
    sTerryChildNames.mValues[2].mText = at<be<u8>>(guest_addr("L_LinkPos_00"));
    sTerryArchiveNames.mValues[0].mVtable = &sTerryStringVtable;
    sTerryArchiveNames.mValues[0].mText = at<be<u8>>(guest_addr("Terry_00"));
    sTerryArchiveNames.mValues[1].mVtable = &sTerryStringVtable;
    sTerryArchiveNames.mValues[1].mText = at<be<u8>>(guest_addr("MapAreaNow_00"));
    sTerryArchiveNames.mValues[2].mVtable = &sTerryStringVtable;
    sTerryArchiveNames.mValues[2].mText = at<be<u8>>(guest_addr("BtnBack_00"));
    sTerryArchiveNames.mValues[3].mVtable = &sTerryStringVtable;
    sTerryLayoutName.mText = at<be<u8>>(guest_addr("Terry_00"));
    sTerryChildTypes.mValues[2].mVtable = &sTerryStringVtable;
    sTerryChildTypes.mValues[1].mVtable = &sTerryStringVtable;
    sTerryArchiveNames.mValues[3].mText = at<be<u8>>(guest_addr("LinkPos_00"));
}
/* 026E928C */
u32 hdTerryChart_c::visitorWord() { return mVisitorWord; }
/* WWHD glue (tools/verify/autoglue.py)
ext 0273AD10 void* terryChartAllocate(u32)
ext 0273AF40 void terryChartDelete(void*)
ext 02683938 void terryChartDestroyLinkPos(hdSeaChartLinkPos_c*,u32)
ext 026842D4 void terryChartConstructLinkPos(hdSeaChartLinkPos_c*)
ext 02684AB4 void terryChartConfigureAnimations(hdSeaChartLinkPos_c*,u32,u32,u32,u32)
ext 026847F4 void terryChartSetupLayout(hdSeaChartLinkPos_c*)
ext 026844C0 void terryChartDestroyLinkPosLayout(hdSeaChartLinkPos_c*)
ext 026F91F8 void terryChartDetachArchives(hdParts_c*)
ext 026F90D8 void terryChartAttachArchives(hdParts_c*,hdUiSafeString*,u32)
ext 026F92C0 u32 terryChartCreateChild(hdParts_c*,u32,hdPartsDesc_c*)
ext 02004E04 void terryChartBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 028F026C void terryChartRegister(hdLayoutRegistration_c*)
ext 028ED6F8 void terryChartConstructPrimaryLock(be<u8>*)
ext 028EAB2C void terryChartConstructSecondaryLock(be<u8>*)
data sTerryPaneNames 1049CBF4
data sTerryArchiveNames 1049CC64
data sTerryLayoutName 1049CC84
data sTerryChildNames 1049CC34
data sTerryChildTypes 1049CC4C
data sTerryAnimationGroups 1049CBC0
data sTerryAnimationNames 1049CC24
data sTerryStringVtable 10107954
data sTerryPartVtable 10107A28
data sTerryHolderVtable 10107B28
data sTerrySecondaryVtable 10107B38
data sTerryAnimationGroupIndices 10107938
data sTerryUiSystem 1018C404
data sTerryRegistration 101F6F7C
data sTerryPrimaryLock 1049CBE0
data sTerryPrimaryLockRegistration 101F6F88
data sTerrySecondaryLock 1049CBE1
data sTerrySecondaryLockRegistration 101F6F94
data sTerryMinimumAngle 1049CBC8
data sTerryMinimumAngleSeed 10107974
data sTerryMaximumAngle 1049CBCC
data sTerryMaximumAngleSeed 10107978
data sTerryWorldExtentX 1049CBD0
data sTerryWorldExtentSeed 1010797C
data sTerryWorldExtentY 1049CBD4
data sTerryZoomLimitX 1049CBDC
data sTerryZoomLimitSeed 10107980
data sTerryZoomLimitY 1049CBD8
data sTerryRegistrationList 1049CBE4
str 101079CC "N_All_01" in 026E9808
str 101079A4 "N_Grid_00" in 026E9808
str 101079B0 "N_Grid_01" in 026E9808
str 10107994 "L_MapAreaNow_00" in 026E9808
str 101079D8 "L_BtnBack_00" in 026E9808
str 101079F8 "G_All_00" in 026E9808
str 10107A04 "BtnBack_00" in 026E9808
str 10107984 "InAll" in 026E9808
str 1010798C "OutAll" in 026E9808
str 101079E8 "L_LinkPos_00" in 026E9808
str 101079BC "MapAreaNow_00" in 026E9808
str 10107A10 "LinkPos_00" in 026E9808
str 10107A1C "Terry_00" in 026E9808
*/

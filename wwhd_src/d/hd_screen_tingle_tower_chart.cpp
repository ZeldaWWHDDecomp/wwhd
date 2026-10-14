/** Sea-chart marker screen for the Tingle Tower / Tingle Island (layout TincleTower_00). */
#include "d/hd_screen_tingle_tower_chart.h"
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
static ChartStrings6 sTincleTowerPaneNames;
static ChartStrings4 sTincleTowerArchiveNames;
static hdUiSafeString sTincleTowerLayoutName;
static ChartStrings3 sTincleTowerChildNames;
static ChartStrings3 sTincleTowerChildTypes;
static ChartStrings1 sTincleTowerAnimationGroups;
static ChartStrings2 sTincleTowerAnimationNames;
static hdUiStringTable sTincleTowerStringVtable;
static hdPartsVtbl_c sTincleTowerPartVtable;
static u8 sTincleTowerHolderVtable;
static u8 sTincleTowerSecondaryVtable;
static ChartIndices2 sTincleTowerAnimationGroupIndices;
static ChartUiSystemPointer sTincleTowerUiSystem;
static hdLayoutRegistration_c sTincleTowerRegistration;
static ChartFlag sTincleTowerPrimaryLock;
static hdLayoutRegistration_c sTincleTowerPrimaryLockRegistration;
static ChartFlag sTincleTowerSecondaryLock;
static hdLayoutRegistration_c sTincleTowerSecondaryLockRegistration;
static ChartFloat sTincleTowerMinimumAngle;
static ChartFloat sTincleTowerMinimumAngleSeed;
static ChartFloat sTincleTowerMaximumAngle;
static ChartFloat sTincleTowerMaximumAngleSeed;
static ChartFloat sTincleTowerWorldExtentX;
static ChartFloat sTincleTowerWorldExtentSeed;
static ChartFloat sTincleTowerWorldExtentY;
static ChartFloat sTincleTowerZoomLimitX;
static ChartFloat sTincleTowerZoomLimitSeed;
static ChartFloat sTincleTowerZoomLimitY;
static hdChartRegistrationList_l sTincleTowerRegistrationList;
extern void* tincleTowerChartAllocate(u32);
extern void tincleTowerChartDelete(void*);
extern void tincleTowerChartDestroyLinkPos(hdSeaChartLinkPos_c*,u32);
extern void tincleTowerChartConstructLinkPos(hdSeaChartLinkPos_c*);
extern void tincleTowerChartConfigureAnimations(hdSeaChartLinkPos_c*,u32,u32,u32,u32);
extern void tincleTowerChartSetupLayout(hdSeaChartLinkPos_c*);
extern void tincleTowerChartDestroyLinkPosLayout(hdSeaChartLinkPos_c*);
extern void tincleTowerChartDetachArchives(hdParts_c*);
extern void tincleTowerChartAttachArchives(hdParts_c*,hdUiSafeString*,u32);
extern u32 tincleTowerChartCreateChild(hdParts_c*,u32,hdPartsDesc_c*);
extern void tincleTowerChartBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*);
extern void tincleTowerChartRegister(hdLayoutRegistration_c*);
extern void tincleTowerChartConstructPrimaryLock(be<u8>*);
extern void tincleTowerChartConstructSecondaryLock(be<u8>*);
/* 026EA560 */
// hdTincleTowerChart_c::~hdTincleTowerChart_c() (GHS deleting destructor: restores the base vtables, frees when flags & 1)
void hdTincleTowerChart_c::destroy(hdTincleTowerChart_c* chart, u32 flags) {
    if (!chart) return;
    chart->mStateHolder.mHolderVtable = &sTincleTowerHolderVtable;
    chart->mSecondaryVtable = &sTincleTowerSecondaryVtable;
    tincleTowerChartDestroyLinkPos(chart,0);
    if (flags & 1) tincleTowerChartDelete(chart);
}
/* 026EA5CC */
hdTincleTowerChart_c* hdTincleTowerChart_c::construct(hdTincleTowerChart_c* part) {
    if (!part) part = static_cast<hdTincleTowerChart_c*>(tincleTowerChartAllocate(sizeof(hdTincleTowerChart_c)));
    if (part) {
        tincleTowerChartConstructLinkPos(part);
        part->mVtable = &sTincleTowerPartVtable;
        part->mStateHolder.mHolderVtable = &sTincleTowerHolderVtable;
        part->mSecondaryVtable = &sTincleTowerSecondaryVtable;
    }
    return part;
}
/* 026EA638 */
void hdTincleTowerChart_c::initialize() {
    mPaneNameCount = 6;
    mPaneNames = sTincleTowerPaneNames.mValues;
    tincleTowerChartConfigureAnimations(this,0,1,0,0);
    tincleTowerChartSetupLayout(this);
}
/* 026EA690 */
u32 hdTincleTowerChart_c::create(u32 first,u32 second,u32 third) {
    tincleTowerChartAttachArchives(this,sTincleTowerArchiveNames.mValues,4);
    u32 target = mVtable->mLoad.get();
    void* resource = mpResAccessor.get();
    if (!call_ptr<u32>(target,this,&sTincleTowerLayoutName,resource,first,second,third)) return 0;
    if (!call_ptr<u32>(mVtable->mCreateChildren.get(),this)) return 0;
    initialize();
    return 1;
}
/* 026EA76C */
void hdTincleTowerChart_c::destroyLayout() {
    tincleTowerChartDestroyLinkPosLayout(this);
    tincleTowerChartDetachArchives(this);
}
/* 026EA7A0 */
void hdTincleTowerChart_c::executeState() {
    hdParts_executeStateMachine(stateMachine());
}
/* 026EA7A8 */
u32 hdTincleTowerChart_c::createChild(hdPartsDesc_c* descriptor) {
    hdUiSafeString* actual=&hdChartChildName(descriptor)->mName;
    for (u32 i=0;i<3;++i) {
        actual->assureTermination();
        actual->assureTermination();
        hdUiSafeString* expected=&sTincleTowerChildNames.mValues[i];
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
        if (matches) return tincleTowerChartCreateChild(this,i,descriptor);
    }
    return 1;
}
/* 026EA8B0 */
u32 hdTincleTowerChart_c::createLayout(void* name,u32 resource,u32 first,u32 second,void* descriptor) {
    auto* layout=static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c),sTincleTowerUiSystem->mHeap.get(),4));
    if (layout) layout=hdPartsLayout_c::construct(layout);
    mpLayout=layout;
    if (!layout || !layout->vLoad(name,resource,first,second,2,1,descriptor)) return 0;
    for (u32 i=0;i<2;++i) {
        hdPartsLayout_c* current=mpLayout.get();
        u32 group=sTincleTowerAnimationGroupIndices.mValues[i];
        hdAnimSet* set=current->animSet();
        tincleTowerChartBindAnimation(set,i,&sTincleTowerAnimationNames.mValues[i],&sTincleTowerAnimationGroups.mValues[group]);
    }
    vCalcLayout();
    return 1;
}
/* 026EA9DC */
void hdTincleTowerChart_c::initializeStatics() {
    sTincleTowerRegistrationList.mCapacity = 0;
    sTincleTowerRegistrationList.mCount = 0;
    sTincleTowerRegistrationList.mTail = 0;
    sTincleTowerRegistrationList.mHead = 0;
    tincleTowerChartRegister(&sTincleTowerRegistration);
    sTincleTowerMinimumAngle = sTincleTowerMinimumAngleSeed;
    sTincleTowerMaximumAngle = sTincleTowerMaximumAngleSeed;
    tincleTowerChartConstructPrimaryLock(&sTincleTowerPrimaryLock);
    tincleTowerChartRegister(&sTincleTowerPrimaryLockRegistration);
    tincleTowerChartConstructSecondaryLock(&sTincleTowerSecondaryLock);
    tincleTowerChartRegister(&sTincleTowerSecondaryLockRegistration);
    sTincleTowerWorldExtentX = sTincleTowerWorldExtentSeed;
    sTincleTowerWorldExtentY = sTincleTowerWorldExtentSeed;
    sTincleTowerPaneNames.mValues[0].mVtable = &sTincleTowerStringVtable;
    sTincleTowerPaneNames.mValues[0].mText = at<be<u8>>(guest_addr("N_All_01"));
    sTincleTowerPaneNames.mValues[1].mVtable = &sTincleTowerStringVtable;
    sTincleTowerZoomLimitX = sTincleTowerZoomLimitSeed;
    sTincleTowerPaneNames.mValues[1].mText = at<be<u8>>(guest_addr("N_Grid_00"));
    sTincleTowerPaneNames.mValues[2].mVtable = &sTincleTowerStringVtable;
    sTincleTowerPaneNames.mValues[2].mText = at<be<u8>>(guest_addr("N_Grid_01"));
    sTincleTowerPaneNames.mValues[3].mVtable = &sTincleTowerStringVtable;
    sTincleTowerPaneNames.mValues[3].mText = at<be<u8>>(guest_addr("L_MapAreaNow_00"));
    sTincleTowerPaneNames.mValues[4].mVtable = &sTincleTowerStringVtable;
    sTincleTowerPaneNames.mValues[4].mText = at<be<u8>>(guest_addr("L_BtnBack_00"));
    sTincleTowerAnimationGroups.mValues[0].mVtable = &sTincleTowerStringVtable;
    sTincleTowerAnimationGroups.mValues[0].mText = at<be<u8>>(guest_addr("G_All_00"));
    sTincleTowerZoomLimitY = sTincleTowerZoomLimitSeed;
    sTincleTowerChildTypes.mValues[1].mText = at<be<u8>>(guest_addr("MapAreaNow_00"));
    sTincleTowerChildNames.mValues[0].mVtable = &sTincleTowerStringVtable;
    sTincleTowerChildNames.mValues[0].mText = at<be<u8>>(guest_addr("L_BtnBack_00"));
    sTincleTowerAnimationNames.mValues[0].mVtable = &sTincleTowerStringVtable;
    sTincleTowerAnimationNames.mValues[0].mText = at<be<u8>>(guest_addr("InAll"));
    sTincleTowerChildNames.mValues[1].mVtable = &sTincleTowerStringVtable;
    sTincleTowerAnimationNames.mValues[1].mVtable = &sTincleTowerStringVtable;
    sTincleTowerAnimationNames.mValues[1].mText = at<be<u8>>(guest_addr("OutAll"));
    sTincleTowerChildTypes.mValues[0].mVtable = &sTincleTowerStringVtable;
    sTincleTowerChildNames.mValues[1].mText = at<be<u8>>(guest_addr("L_MapAreaNow_00"));
    sTincleTowerPaneNames.mValues[5].mVtable = &sTincleTowerStringVtable;
    sTincleTowerPaneNames.mValues[5].mText = at<be<u8>>(guest_addr("L_LinkPos_00"));
    sTincleTowerChildTypes.mValues[0].mText = at<be<u8>>(guest_addr("BtnBack_00"));
    sTincleTowerLayoutName.mVtable = &sTincleTowerStringVtable;
    sTincleTowerChildTypes.mValues[2].mText = at<be<u8>>(guest_addr("LinkPos_00"));
    sTincleTowerChildNames.mValues[2].mVtable = &sTincleTowerStringVtable;
    sTincleTowerChildNames.mValues[2].mText = at<be<u8>>(guest_addr("L_LinkPos_00"));
    sTincleTowerArchiveNames.mValues[0].mVtable = &sTincleTowerStringVtable;
    sTincleTowerArchiveNames.mValues[0].mText = at<be<u8>>(guest_addr("TincleTower_00"));
    sTincleTowerArchiveNames.mValues[1].mVtable = &sTincleTowerStringVtable;
    sTincleTowerArchiveNames.mValues[1].mText = at<be<u8>>(guest_addr("BtnBack_00"));
    sTincleTowerArchiveNames.mValues[2].mVtable = &sTincleTowerStringVtable;
    sTincleTowerArchiveNames.mValues[2].mText = at<be<u8>>(guest_addr("MapAreaNow_00"));
    sTincleTowerArchiveNames.mValues[3].mVtable = &sTincleTowerStringVtable;
    sTincleTowerLayoutName.mText = at<be<u8>>(guest_addr("TincleTower_00"));
    sTincleTowerChildTypes.mValues[2].mVtable = &sTincleTowerStringVtable;
    sTincleTowerChildTypes.mValues[1].mVtable = &sTincleTowerStringVtable;
    sTincleTowerArchiveNames.mValues[3].mText = at<be<u8>>(guest_addr("LinkPos_00"));
}
/* 026EA460 */
u32 hdTincleTowerChart_c::visitorWord() { return mVisitorWord; }
/* WWHD glue (tools/verify/autoglue.py)
ext 0273AD10 void* tincleTowerChartAllocate(u32)
ext 0273AF40 void tincleTowerChartDelete(void*)
ext 02683938 void tincleTowerChartDestroyLinkPos(hdSeaChartLinkPos_c*,u32)
ext 026842D4 void tincleTowerChartConstructLinkPos(hdSeaChartLinkPos_c*)
ext 02684AB4 void tincleTowerChartConfigureAnimations(hdSeaChartLinkPos_c*,u32,u32,u32,u32)
ext 026847F4 void tincleTowerChartSetupLayout(hdSeaChartLinkPos_c*)
ext 026844C0 void tincleTowerChartDestroyLinkPosLayout(hdSeaChartLinkPos_c*)
ext 026F91F8 void tincleTowerChartDetachArchives(hdParts_c*)
ext 026F90D8 void tincleTowerChartAttachArchives(hdParts_c*,hdUiSafeString*,u32)
ext 026F92C0 u32 tincleTowerChartCreateChild(hdParts_c*,u32,hdPartsDesc_c*)
ext 02004E04 void tincleTowerChartBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 028F026C void tincleTowerChartRegister(hdLayoutRegistration_c*)
ext 028ED6F8 void tincleTowerChartConstructPrimaryLock(be<u8>*)
ext 028EAB2C void tincleTowerChartConstructSecondaryLock(be<u8>*)
data sTincleTowerPaneNames 1049CDDC
data sTincleTowerArchiveNames 1049CE4C
data sTincleTowerLayoutName 1049CE6C
data sTincleTowerChildNames 1049CE1C
data sTincleTowerChildTypes 1049CE34
data sTincleTowerAnimationGroups 1049CDA8
data sTincleTowerAnimationNames 1049CE0C
data sTincleTowerStringVtable 10107E74
data sTincleTowerPartVtable 10107F50
data sTincleTowerHolderVtable 10108050
data sTincleTowerSecondaryVtable 10108060
data sTincleTowerAnimationGroupIndices 10107E58
data sTincleTowerUiSystem 1018C404
data sTincleTowerRegistration 101F6FC4
data sTincleTowerPrimaryLock 1049CDC8
data sTincleTowerPrimaryLockRegistration 101F6FD0
data sTincleTowerSecondaryLock 1049CDC9
data sTincleTowerSecondaryLockRegistration 101F6FDC
data sTincleTowerMinimumAngle 1049CDB0
data sTincleTowerMinimumAngleSeed 10107E94
data sTincleTowerMaximumAngle 1049CDB4
data sTincleTowerMaximumAngleSeed 10107E98
data sTincleTowerWorldExtentX 1049CDB8
data sTincleTowerWorldExtentSeed 10107E9C
data sTincleTowerWorldExtentY 1049CDBC
data sTincleTowerZoomLimitX 1049CDC4
data sTincleTowerZoomLimitSeed 10107EA0
data sTincleTowerZoomLimitY 1049CDC0
data sTincleTowerRegistrationList 1049CDCC
str 10107EEC "N_All_01" in 026EA9DC
str 10107EC4 "N_Grid_00" in 026EA9DC
str 10107ED0 "N_Grid_01" in 026EA9DC
str 10107EB4 "L_MapAreaNow_00" in 026EA9DC
str 10107EF8 "L_BtnBack_00" in 026EA9DC
str 10107F18 "G_All_00" in 026EA9DC
str 10107EDC "MapAreaNow_00" in 026EA9DC
str 10107EA4 "InAll" in 026EA9DC
str 10107EAC "OutAll" in 026EA9DC
str 10107F08 "L_LinkPos_00" in 026EA9DC
str 10107F24 "BtnBack_00" in 026EA9DC
str 10107F30 "LinkPos_00" in 026EA9DC
str 10107F3C "TincleTower_00" in 026EA9DC
*/

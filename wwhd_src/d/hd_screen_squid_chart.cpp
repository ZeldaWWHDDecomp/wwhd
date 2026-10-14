/** Sea-chart marker screen for the Big Octo (squid) positions (layout Squid_00). */
#include "d/hd_screen_squid_chart.h"
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
static ChartStrings6 sSquidPaneNames;
static ChartStrings4 sSquidArchiveNames;
static hdUiSafeString sSquidLayoutName;
static ChartStrings3 sSquidChildNames;
static ChartStrings3 sSquidChildTypes;
static ChartStrings1 sSquidAnimationGroups;
static ChartStrings2 sSquidAnimationNames;
static hdUiStringTable sSquidStringVtable;
static hdPartsVtbl_c sSquidPartVtable;
static u8 sSquidHolderVtable;
static u8 sSquidSecondaryVtable;
static ChartIndices2 sSquidAnimationGroupIndices;
static ChartUiSystemPointer sSquidUiSystem;
static hdLayoutRegistration_c sSquidRegistration;
static ChartFlag sSquidPrimaryLock;
static hdLayoutRegistration_c sSquidPrimaryLockRegistration;
static ChartFlag sSquidSecondaryLock;
static hdLayoutRegistration_c sSquidSecondaryLockRegistration;
static ChartFloat sSquidMinimumAngle;
static ChartFloat sSquidMinimumAngleSeed;
static ChartFloat sSquidMaximumAngle;
static ChartFloat sSquidMaximumAngleSeed;
static ChartFloat sSquidWorldExtentX;
static ChartFloat sSquidWorldExtentSeed;
static ChartFloat sSquidWorldExtentY;
static ChartFloat sSquidZoomLimitX;
static ChartFloat sSquidZoomLimitSeed;
static ChartFloat sSquidZoomLimitY;
static hdChartRegistrationList_l sSquidRegistrationList;
extern void* squidChartAllocate(u32);
extern void squidChartDelete(void*);
extern void squidChartDestroyLinkPos(hdSeaChartLinkPos_c*,u32);
extern void squidChartConstructLinkPos(hdSeaChartLinkPos_c*);
extern void squidChartConfigureAnimations(hdSeaChartLinkPos_c*,u32,u32,u32,u32);
extern void squidChartSetupLayout(hdSeaChartLinkPos_c*);
extern void squidChartDestroyLinkPosLayout(hdSeaChartLinkPos_c*);
extern void squidChartDetachArchives(hdParts_c*);
extern void squidChartAttachArchives(hdParts_c*,hdUiSafeString*,u32);
extern u32 squidChartCreateChild(hdParts_c*,u32,hdPartsDesc_c*);
extern void squidChartBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*);
extern void squidChartRegister(hdLayoutRegistration_c*);
extern void squidChartConstructPrimaryLock(be<u8>*);
extern void squidChartConstructSecondaryLock(be<u8>*);
/* 026DF668 */
// hdSquidChart_c::~hdSquidChart_c() (GHS deleting destructor: restores the base vtables, frees when flags & 1)
void hdSquidChart_c::destroy(hdSquidChart_c* chart, u32 flags) {
    if (!chart) return;
    chart->mStateHolder.mHolderVtable = &sSquidHolderVtable;
    chart->mSecondaryVtable = &sSquidSecondaryVtable;
    squidChartDestroyLinkPos(chart,0);
    if (flags & 1) squidChartDelete(chart);
}
/* 026DF6D4 */
hdSquidChart_c* hdSquidChart_c::construct(hdSquidChart_c* part) {
    if (!part) part = static_cast<hdSquidChart_c*>(squidChartAllocate(sizeof(hdSquidChart_c)));
    if (part) {
        squidChartConstructLinkPos(part);
        part->mVtable = &sSquidPartVtable;
        part->mStateHolder.mHolderVtable = &sSquidHolderVtable;
        part->mSecondaryVtable = &sSquidSecondaryVtable;
    }
    return part;
}
/* 026DF740 */
void hdSquidChart_c::initialize() {
    mPaneNameCount = 6;
    mPaneNames = sSquidPaneNames.mValues;
    squidChartConfigureAnimations(this,0,1,0,0);
    squidChartSetupLayout(this);
}
/* 026DF798 */
u32 hdSquidChart_c::create(u32 first,u32 second,u32 third) {
    squidChartAttachArchives(this,sSquidArchiveNames.mValues,4);
    u32 target = mVtable->mLoad.get();
    void* resource = mpResAccessor.get();
    if (!call_ptr<u32>(target,this,&sSquidLayoutName,resource,first,second,third)) return 0;
    if (!call_ptr<u32>(mVtable->mCreateChildren.get(),this)) return 0;
    initialize();
    return 1;
}
/* 026DF874 */
void hdSquidChart_c::destroyLayout() {
    squidChartDestroyLinkPosLayout(this);
    squidChartDetachArchives(this);
}
/* 026DF8A8 */
void hdSquidChart_c::executeState() {
    hdParts_executeStateMachine(stateMachine());
}
/* 026DF8B0 */
u32 hdSquidChart_c::createChild(hdPartsDesc_c* descriptor) {
    hdUiSafeString* actual=&hdChartChildName(descriptor)->mName;
    for (u32 i=0;i<3;++i) {
        actual->assureTermination();
        actual->assureTermination();
        hdUiSafeString* expected=&sSquidChildNames.mValues[i];
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
        if (matches) return squidChartCreateChild(this,i,descriptor);
    }
    return 1;
}
/* 026DF9B8 */
u32 hdSquidChart_c::createLayout(void* name,u32 resource,u32 first,u32 second,void* descriptor) {
    auto* layout=static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c),sSquidUiSystem->mHeap.get(),4));
    if (layout) layout=hdPartsLayout_c::construct(layout);
    mpLayout=layout;
    if (!layout || !layout->vLoad(name,resource,first,second,2,1,descriptor)) return 0;
    for (u32 i=0;i<2;++i) {
        hdPartsLayout_c* current=mpLayout.get();
        u32 group=sSquidAnimationGroupIndices.mValues[i];
        hdAnimSet* set=current->animSet();
        squidChartBindAnimation(set,i,&sSquidAnimationNames.mValues[i],&sSquidAnimationGroups.mValues[group]);
    }
    vCalcLayout();
    return 1;
}
/* 026DFAE4 */
void hdSquidChart_c::initializeStatics() {
    sSquidRegistrationList.mCapacity = 0;
    sSquidRegistrationList.mCount = 0;
    sSquidRegistrationList.mTail = 0;
    sSquidRegistrationList.mHead = 0;
    squidChartRegister(&sSquidRegistration);
    sSquidMinimumAngle = sSquidMinimumAngleSeed;
    sSquidMaximumAngle = sSquidMaximumAngleSeed;
    squidChartConstructPrimaryLock(&sSquidPrimaryLock);
    squidChartRegister(&sSquidPrimaryLockRegistration);
    squidChartConstructSecondaryLock(&sSquidSecondaryLock);
    squidChartRegister(&sSquidSecondaryLockRegistration);
    sSquidWorldExtentX = sSquidWorldExtentSeed;
    sSquidWorldExtentY = sSquidWorldExtentSeed;
    sSquidPaneNames.mValues[0].mVtable = &sSquidStringVtable;
    sSquidPaneNames.mValues[0].mText = at<be<u8>>(guest_addr("N_All_01"));
    sSquidPaneNames.mValues[1].mVtable = &sSquidStringVtable;
    sSquidZoomLimitX = sSquidZoomLimitSeed;
    sSquidPaneNames.mValues[1].mText = at<be<u8>>(guest_addr("N_Grid_00"));
    sSquidPaneNames.mValues[2].mVtable = &sSquidStringVtable;
    sSquidPaneNames.mValues[2].mText = at<be<u8>>(guest_addr("N_Grid_01"));
    sSquidPaneNames.mValues[3].mVtable = &sSquidStringVtable;
    sSquidPaneNames.mValues[3].mText = at<be<u8>>(guest_addr("L_MapAreaNow_00"));
    sSquidPaneNames.mValues[4].mVtable = &sSquidStringVtable;
    sSquidPaneNames.mValues[4].mText = at<be<u8>>(guest_addr("L_BtnBack_00"));
    sSquidAnimationGroups.mValues[0].mVtable = &sSquidStringVtable;
    sSquidAnimationGroups.mValues[0].mText = at<be<u8>>(guest_addr("G_All_00"));
    sSquidZoomLimitY = sSquidZoomLimitSeed;
    sSquidChildTypes.mValues[1].mText = at<be<u8>>(guest_addr("MapAreaNow_00"));
    sSquidChildNames.mValues[0].mVtable = &sSquidStringVtable;
    sSquidChildNames.mValues[0].mText = at<be<u8>>(guest_addr("L_BtnBack_00"));
    sSquidAnimationNames.mValues[0].mVtable = &sSquidStringVtable;
    sSquidAnimationNames.mValues[0].mText = at<be<u8>>(guest_addr("InAll"));
    sSquidChildNames.mValues[1].mVtable = &sSquidStringVtable;
    sSquidAnimationNames.mValues[1].mVtable = &sSquidStringVtable;
    sSquidAnimationNames.mValues[1].mText = at<be<u8>>(guest_addr("OutAll"));
    sSquidChildTypes.mValues[0].mVtable = &sSquidStringVtable;
    sSquidChildNames.mValues[1].mText = at<be<u8>>(guest_addr("L_MapAreaNow_00"));
    sSquidPaneNames.mValues[5].mVtable = &sSquidStringVtable;
    sSquidPaneNames.mValues[5].mText = at<be<u8>>(guest_addr("L_LinkPos_00"));
    sSquidChildTypes.mValues[0].mText = at<be<u8>>(guest_addr("BtnBack_00"));
    sSquidLayoutName.mVtable = &sSquidStringVtable;
    sSquidChildTypes.mValues[2].mText = at<be<u8>>(guest_addr("LinkPos_00"));
    sSquidChildNames.mValues[2].mVtable = &sSquidStringVtable;
    sSquidChildNames.mValues[2].mText = at<be<u8>>(guest_addr("L_LinkPos_00"));
    sSquidArchiveNames.mValues[0].mVtable = &sSquidStringVtable;
    sSquidArchiveNames.mValues[0].mText = at<be<u8>>(guest_addr("Squid_00"));
    sSquidArchiveNames.mValues[1].mVtable = &sSquidStringVtable;
    sSquidArchiveNames.mValues[1].mText = at<be<u8>>(guest_addr("BtnBack_00"));
    sSquidArchiveNames.mValues[2].mVtable = &sSquidStringVtable;
    sSquidArchiveNames.mValues[2].mText = at<be<u8>>(guest_addr("MapAreaNow_00"));
    sSquidArchiveNames.mValues[3].mVtable = &sSquidStringVtable;
    sSquidLayoutName.mText = at<be<u8>>(guest_addr("Squid_00"));
    sSquidChildTypes.mValues[2].mVtable = &sSquidStringVtable;
    sSquidChildTypes.mValues[1].mVtable = &sSquidStringVtable;
    sSquidArchiveNames.mValues[3].mText = at<be<u8>>(guest_addr("LinkPos_00"));
}
/* 026DF568 */
u32 hdSquidChart_c::visitorWord() { return mVisitorWord; }
/* WWHD glue (tools/verify/autoglue.py)
ext 0273AD10 void* squidChartAllocate(u32)
ext 0273AF40 void squidChartDelete(void*)
ext 02683938 void squidChartDestroyLinkPos(hdSeaChartLinkPos_c*,u32)
ext 026842D4 void squidChartConstructLinkPos(hdSeaChartLinkPos_c*)
ext 02684AB4 void squidChartConfigureAnimations(hdSeaChartLinkPos_c*,u32,u32,u32,u32)
ext 026847F4 void squidChartSetupLayout(hdSeaChartLinkPos_c*)
ext 026844C0 void squidChartDestroyLinkPosLayout(hdSeaChartLinkPos_c*)
ext 026F91F8 void squidChartDetachArchives(hdParts_c*)
ext 026F90D8 void squidChartAttachArchives(hdParts_c*,hdUiSafeString*,u32)
ext 026F92C0 u32 squidChartCreateChild(hdParts_c*,u32,hdPartsDesc_c*)
ext 02004E04 void squidChartBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 028F026C void squidChartRegister(hdLayoutRegistration_c*)
ext 028ED6F8 void squidChartConstructPrimaryLock(be<u8>*)
ext 028EAB2C void squidChartConstructSecondaryLock(be<u8>*)
data sSquidPaneNames 1049BA74
data sSquidArchiveNames 1049BAE4
data sSquidLayoutName 1049BB04
data sSquidChildNames 1049BAB4
data sSquidChildTypes 1049BACC
data sSquidAnimationGroups 1049BA40
data sSquidAnimationNames 1049BAA4
data sSquidStringVtable 10104FEC
data sSquidPartVtable 101050C0
data sSquidHolderVtable 101051C0
data sSquidSecondaryVtable 101051D0
data sSquidAnimationGroupIndices 10104FD0
data sSquidUiSystem 1018C404
data sSquidRegistration 101F6B74
data sSquidPrimaryLock 1049BA60
data sSquidPrimaryLockRegistration 101F6B80
data sSquidSecondaryLock 1049BA61
data sSquidSecondaryLockRegistration 101F6B8C
data sSquidMinimumAngle 1049BA48
data sSquidMinimumAngleSeed 1010500C
data sSquidMaximumAngle 1049BA4C
data sSquidMaximumAngleSeed 10105010
data sSquidWorldExtentX 1049BA50
data sSquidWorldExtentSeed 10105014
data sSquidWorldExtentY 1049BA54
data sSquidZoomLimitX 1049BA5C
data sSquidZoomLimitSeed 10105018
data sSquidZoomLimitY 1049BA58
data sSquidRegistrationList 1049BA64
str 10105064 "N_All_01" in 026DFAE4
str 1010503C "N_Grid_00" in 026DFAE4
str 10105048 "N_Grid_01" in 026DFAE4
str 1010502C "L_MapAreaNow_00" in 026DFAE4
str 10105070 "L_BtnBack_00" in 026DFAE4
str 10105090 "G_All_00" in 026DFAE4
str 10105054 "MapAreaNow_00" in 026DFAE4
str 1010501C "InAll" in 026DFAE4
str 10105024 "OutAll" in 026DFAE4
str 10105080 "L_LinkPos_00" in 026DFAE4
str 1010509C "BtnBack_00" in 026DFAE4
str 101050A8 "LinkPos_00" in 026DFAE4
str 101050B4 "Squid_00" in 026DFAE4
*/

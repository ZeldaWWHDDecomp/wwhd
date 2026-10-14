/** Sea-chart marker screen for the secret cave entrances (layout SecretEntrance_00). */
#include "d/hd_screen_secret_entrance_chart.h"
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
static ChartStrings6 sSecretEntrancePaneNames;
static ChartStrings4 sSecretEntranceArchiveNames;
static hdUiSafeString sSecretEntranceLayoutName;
static ChartStrings3 sSecretEntranceChildNames;
static ChartStrings3 sSecretEntranceChildTypes;
static ChartStrings1 sSecretEntranceAnimationGroups;
static ChartStrings2 sSecretEntranceAnimationNames;
static hdUiStringTable sSecretEntranceStringVtable;
static hdPartsVtbl_c sSecretEntrancePartVtable;
static u8 sSecretEntranceHolderVtable;
static u8 sSecretEntranceSecondaryVtable;
static ChartIndices2 sSecretEntranceAnimationGroupIndices;
static ChartUiSystemPointer sSecretEntranceUiSystem;
static hdLayoutRegistration_c sSecretEntranceRegistration;
static ChartFlag sSecretEntrancePrimaryLock;
static hdLayoutRegistration_c sSecretEntrancePrimaryLockRegistration;
static ChartFlag sSecretEntranceSecondaryLock;
static hdLayoutRegistration_c sSecretEntranceSecondaryLockRegistration;
static ChartFloat sSecretEntranceMinimumAngle;
static ChartFloat sSecretEntranceMinimumAngleSeed;
static ChartFloat sSecretEntranceMaximumAngle;
static ChartFloat sSecretEntranceMaximumAngleSeed;
static ChartFloat sSecretEntranceWorldExtentX;
static ChartFloat sSecretEntranceWorldExtentSeed;
static ChartFloat sSecretEntranceWorldExtentY;
static ChartFloat sSecretEntranceZoomLimitX;
static ChartFloat sSecretEntranceZoomLimitSeed;
static ChartFloat sSecretEntranceZoomLimitY;
static hdChartRegistrationList_l sSecretEntranceRegistrationList;
extern void* secretEntranceChartAllocate(u32);
extern void secretEntranceChartDelete(void*);
extern void secretEntranceChartDestroyLinkPos(hdSeaChartLinkPos_c*,u32);
extern void secretEntranceChartConstructLinkPos(hdSeaChartLinkPos_c*);
extern void secretEntranceChartConfigureAnimations(hdSeaChartLinkPos_c*,u32,u32,u32,u32);
extern void secretEntranceChartSetupLayout(hdSeaChartLinkPos_c*);
extern void secretEntranceChartDestroyLinkPosLayout(hdSeaChartLinkPos_c*);
extern void secretEntranceChartDetachArchives(hdParts_c*);
extern void secretEntranceChartAttachArchives(hdParts_c*,hdUiSafeString*,u32);
extern u32 secretEntranceChartCreateChild(hdParts_c*,u32,hdPartsDesc_c*);
extern void secretEntranceChartBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*);
extern void secretEntranceChartRegister(hdLayoutRegistration_c*);
extern void secretEntranceChartConstructPrimaryLock(be<u8>*);
extern void secretEntranceChartConstructSecondaryLock(be<u8>*);
/* 026D3B00 */
// hdSecretEntranceChart_c::~hdSecretEntranceChart_c() (GHS deleting destructor: restores the base vtables, frees when flags & 1)
void hdSecretEntranceChart_c::destroy(hdSecretEntranceChart_c* chart, u32 flags) {
    if (!chart) return;
    chart->mStateHolder.mHolderVtable = &sSecretEntranceHolderVtable;
    chart->mSecondaryVtable = &sSecretEntranceSecondaryVtable;
    secretEntranceChartDestroyLinkPos(chart,0);
    if (flags & 1) secretEntranceChartDelete(chart);
}
/* 026D3B6C */
hdSecretEntranceChart_c* hdSecretEntranceChart_c::construct(hdSecretEntranceChart_c* part) {
    if (!part) part = static_cast<hdSecretEntranceChart_c*>(secretEntranceChartAllocate(sizeof(hdSecretEntranceChart_c)));
    if (part) {
        secretEntranceChartConstructLinkPos(part);
        part->mVtable = &sSecretEntrancePartVtable;
        part->mStateHolder.mHolderVtable = &sSecretEntranceHolderVtable;
        part->mSecondaryVtable = &sSecretEntranceSecondaryVtable;
    }
    return part;
}
/* 026D3BD8 */
void hdSecretEntranceChart_c::initialize() {
    mPaneNameCount = 6;
    mPaneNames = sSecretEntrancePaneNames.mValues;
    secretEntranceChartConfigureAnimations(this,0,1,0,1);
    secretEntranceChartSetupLayout(this);
}
/* 026D3C30 */
u32 hdSecretEntranceChart_c::create(u32 first,u32 second,u32 third) {
    secretEntranceChartAttachArchives(this,sSecretEntranceArchiveNames.mValues,4);
    u32 target = mVtable->mLoad.get();
    void* resource = mpResAccessor.get();
    if (!call_ptr<u32>(target,this,&sSecretEntranceLayoutName,resource,first,second,third)) return 0;
    if (!call_ptr<u32>(mVtable->mCreateChildren.get(),this)) return 0;
    initialize();
    return 1;
}
/* 026D3D0C */
void hdSecretEntranceChart_c::destroyLayout() {
    secretEntranceChartDestroyLinkPosLayout(this);
    secretEntranceChartDetachArchives(this);
}
/* 026D3D40 */
void hdSecretEntranceChart_c::executeState() {
    hdParts_executeStateMachine(stateMachine());
}
/* 026D3D48 */
u32 hdSecretEntranceChart_c::createChild(hdPartsDesc_c* descriptor) {
    hdUiSafeString* actual=&hdChartChildName(descriptor)->mName;
    for (u32 i=0;i<3;++i) {
        actual->assureTermination();
        actual->assureTermination();
        hdUiSafeString* expected=&sSecretEntranceChildNames.mValues[i];
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
        if (matches) return secretEntranceChartCreateChild(this,i,descriptor);
    }
    return 1;
}
/* 026D3E50 */
u32 hdSecretEntranceChart_c::createLayout(void* name,u32 resource,u32 first,u32 second,void* descriptor) {
    auto* layout=static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c),sSecretEntranceUiSystem->mHeap.get(),4));
    if (layout) layout=hdPartsLayout_c::construct(layout);
    mpLayout=layout;
    if (!layout || !layout->vLoad(name,resource,first,second,2,1,descriptor)) return 0;
    for (u32 i=0;i<2;++i) {
        hdPartsLayout_c* current=mpLayout.get();
        u32 group=sSecretEntranceAnimationGroupIndices.mValues[i];
        hdAnimSet* set=current->animSet();
        secretEntranceChartBindAnimation(set,i,&sSecretEntranceAnimationNames.mValues[i],&sSecretEntranceAnimationGroups.mValues[group]);
    }
    vCalcLayout();
    return 1;
}
/* 026D3F7C */
void hdSecretEntranceChart_c::initializeStatics() {
    sSecretEntranceRegistrationList.mCapacity = 0;
    sSecretEntranceRegistrationList.mCount = 0;
    sSecretEntranceRegistrationList.mTail = 0;
    sSecretEntranceRegistrationList.mHead = 0;
    secretEntranceChartRegister(&sSecretEntranceRegistration);
    sSecretEntranceMinimumAngle = sSecretEntranceMinimumAngleSeed;
    sSecretEntranceMaximumAngle = sSecretEntranceMaximumAngleSeed;
    secretEntranceChartConstructPrimaryLock(&sSecretEntrancePrimaryLock);
    secretEntranceChartRegister(&sSecretEntrancePrimaryLockRegistration);
    secretEntranceChartConstructSecondaryLock(&sSecretEntranceSecondaryLock);
    secretEntranceChartRegister(&sSecretEntranceSecondaryLockRegistration);
    sSecretEntranceWorldExtentX = sSecretEntranceWorldExtentSeed;
    sSecretEntranceWorldExtentY = sSecretEntranceWorldExtentSeed;
    sSecretEntrancePaneNames.mValues[0].mVtable = &sSecretEntranceStringVtable;
    sSecretEntrancePaneNames.mValues[0].mText = at<be<u8>>(guest_addr("N_All_01"));
    sSecretEntrancePaneNames.mValues[1].mVtable = &sSecretEntranceStringVtable;
    sSecretEntranceZoomLimitX = sSecretEntranceZoomLimitSeed;
    sSecretEntrancePaneNames.mValues[1].mText = at<be<u8>>(guest_addr("N_Grid_00"));
    sSecretEntrancePaneNames.mValues[2].mVtable = &sSecretEntranceStringVtable;
    sSecretEntrancePaneNames.mValues[2].mText = at<be<u8>>(guest_addr("N_Grid_01"));
    sSecretEntrancePaneNames.mValues[3].mVtable = &sSecretEntranceStringVtable;
    sSecretEntrancePaneNames.mValues[3].mText = at<be<u8>>(guest_addr("L_MapAreaNow_00"));
    sSecretEntrancePaneNames.mValues[4].mVtable = &sSecretEntranceStringVtable;
    sSecretEntrancePaneNames.mValues[4].mText = at<be<u8>>(guest_addr("L_BtnBack_00"));
    sSecretEntranceAnimationGroups.mValues[0].mVtable = &sSecretEntranceStringVtable;
    sSecretEntranceAnimationGroups.mValues[0].mText = at<be<u8>>(guest_addr("G_All_00"));
    sSecretEntranceZoomLimitY = sSecretEntranceZoomLimitSeed;
    sSecretEntranceChildTypes.mValues[1].mText = at<be<u8>>(guest_addr("BtnBack_00"));
    sSecretEntranceChildNames.mValues[0].mVtable = &sSecretEntranceStringVtable;
    sSecretEntranceChildNames.mValues[0].mText = at<be<u8>>(guest_addr("L_MapAreaNow_00"));
    sSecretEntranceAnimationNames.mValues[0].mVtable = &sSecretEntranceStringVtable;
    sSecretEntranceAnimationNames.mValues[0].mText = at<be<u8>>(guest_addr("InAll"));
    sSecretEntranceChildNames.mValues[1].mVtable = &sSecretEntranceStringVtable;
    sSecretEntranceAnimationNames.mValues[1].mVtable = &sSecretEntranceStringVtable;
    sSecretEntranceAnimationNames.mValues[1].mText = at<be<u8>>(guest_addr("OutAll"));
    sSecretEntranceChildTypes.mValues[0].mVtable = &sSecretEntranceStringVtable;
    sSecretEntranceChildNames.mValues[1].mText = at<be<u8>>(guest_addr("L_BtnBack_00"));
    sSecretEntrancePaneNames.mValues[5].mVtable = &sSecretEntranceStringVtable;
    sSecretEntrancePaneNames.mValues[5].mText = at<be<u8>>(guest_addr("L_LinkPos_00"));
    sSecretEntranceChildTypes.mValues[0].mText = at<be<u8>>(guest_addr("MapAreaNow_00"));
    sSecretEntranceLayoutName.mVtable = &sSecretEntranceStringVtable;
    sSecretEntranceChildTypes.mValues[2].mText = at<be<u8>>(guest_addr("LinkPos_00"));
    sSecretEntranceChildNames.mValues[2].mVtable = &sSecretEntranceStringVtable;
    sSecretEntranceChildNames.mValues[2].mText = at<be<u8>>(guest_addr("L_LinkPos_00"));
    sSecretEntranceArchiveNames.mValues[0].mVtable = &sSecretEntranceStringVtable;
    sSecretEntranceArchiveNames.mValues[0].mText = at<be<u8>>(guest_addr("SecretEntrance_00"));
    sSecretEntranceArchiveNames.mValues[1].mVtable = &sSecretEntranceStringVtable;
    sSecretEntranceArchiveNames.mValues[1].mText = at<be<u8>>(guest_addr("MapAreaNow_00"));
    sSecretEntranceArchiveNames.mValues[2].mVtable = &sSecretEntranceStringVtable;
    sSecretEntranceArchiveNames.mValues[2].mText = at<be<u8>>(guest_addr("BtnBack_00"));
    sSecretEntranceArchiveNames.mValues[3].mVtable = &sSecretEntranceStringVtable;
    sSecretEntranceLayoutName.mText = at<be<u8>>(guest_addr("SecretEntrance_00"));
    sSecretEntranceChildTypes.mValues[2].mVtable = &sSecretEntranceStringVtable;
    sSecretEntranceChildTypes.mValues[1].mVtable = &sSecretEntranceStringVtable;
    sSecretEntranceArchiveNames.mValues[3].mText = at<be<u8>>(guest_addr("LinkPos_00"));
}
/* WWHD glue (tools/verify/autoglue.py)
ext 0273AD10 void* secretEntranceChartAllocate(u32)
ext 0273AF40 void secretEntranceChartDelete(void*)
ext 02683938 void secretEntranceChartDestroyLinkPos(hdSeaChartLinkPos_c*,u32)
ext 026842D4 void secretEntranceChartConstructLinkPos(hdSeaChartLinkPos_c*)
ext 02684AB4 void secretEntranceChartConfigureAnimations(hdSeaChartLinkPos_c*,u32,u32,u32,u32)
ext 026847F4 void secretEntranceChartSetupLayout(hdSeaChartLinkPos_c*)
ext 026844C0 void secretEntranceChartDestroyLinkPosLayout(hdSeaChartLinkPos_c*)
ext 026F91F8 void secretEntranceChartDetachArchives(hdParts_c*)
ext 026F90D8 void secretEntranceChartAttachArchives(hdParts_c*,hdUiSafeString*,u32)
ext 026F92C0 u32 secretEntranceChartCreateChild(hdParts_c*,u32,hdPartsDesc_c*)
ext 02004E04 void secretEntranceChartBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 028F026C void secretEntranceChartRegister(hdLayoutRegistration_c*)
ext 028ED6F8 void secretEntranceChartConstructPrimaryLock(be<u8>*)
ext 028EAB2C void secretEntranceChartConstructSecondaryLock(be<u8>*)
data sSecretEntrancePaneNames 1049A928
data sSecretEntranceArchiveNames 1049A998
data sSecretEntranceLayoutName 1049A9B8
data sSecretEntranceChildNames 1049A968
data sSecretEntranceChildTypes 1049A980
data sSecretEntranceAnimationGroups 1049A8F4
data sSecretEntranceAnimationNames 1049A958
data sSecretEntranceStringVtable 1010297C
data sSecretEntrancePartVtable 10102A58
data sSecretEntranceHolderVtable 10102B58
data sSecretEntranceSecondaryVtable 10102B68
data sSecretEntranceAnimationGroupIndices 10102960
data sSecretEntranceUiSystem 1018C404
data sSecretEntranceRegistration 101F6A10
data sSecretEntrancePrimaryLock 1049A914
data sSecretEntrancePrimaryLockRegistration 101F6A1C
data sSecretEntranceSecondaryLock 1049A915
data sSecretEntranceSecondaryLockRegistration 101F6A28
data sSecretEntranceMinimumAngle 1049A8FC
data sSecretEntranceMinimumAngleSeed 1010299C
data sSecretEntranceMaximumAngle 1049A900
data sSecretEntranceMaximumAngleSeed 101029A0
data sSecretEntranceWorldExtentX 1049A904
data sSecretEntranceWorldExtentSeed 101029A4
data sSecretEntranceWorldExtentY 1049A908
data sSecretEntranceZoomLimitX 1049A910
data sSecretEntranceZoomLimitSeed 101029A8
data sSecretEntranceZoomLimitY 1049A90C
data sSecretEntranceRegistrationList 1049A918
str 10102A08 "N_All_01" in 026D3F7C
str 101029CC "N_Grid_00" in 026D3F7C
str 101029D8 "N_Grid_01" in 026D3F7C
str 101029BC "L_MapAreaNow_00" in 026D3F7C
str 10102A14 "L_BtnBack_00" in 026D3F7C
str 10102A34 "G_All_00" in 026D3F7C
str 10102A40 "BtnBack_00" in 026D3F7C
str 101029AC "InAll" in 026D3F7C
str 101029B4 "OutAll" in 026D3F7C
str 10102A24 "L_LinkPos_00" in 026D3F7C
str 101029E4 "MapAreaNow_00" in 026D3F7C
str 10102A4C "LinkPos_00" in 026D3F7C
str 101029F4 "SecretEntrance_00" in 026D3F7C
*/

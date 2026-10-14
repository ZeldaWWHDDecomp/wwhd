/** Sea-chart marker screen for the Triforce chart and shard locations (layout Triforce_00). */
#include "d/hd_screen_triforce_chart.h"
using namespace gabi;
using ChartStrings6 = hdChartStrings_l<6>;
using ChartStrings4 = hdChartStrings_l<4>;
using ChartStrings3 = hdChartStrings_l<3>;
using ChartStrings9 = hdChartStrings_l<9>;
using ChartStrings10 = hdChartStrings_l<10>;
using ChartIndices10 = hdChartIndices_l<10>;
using ChartUiSystemPointer = gptr<hdScreenUiSystem>;
using ChartFlag = be<u8>;
using ChartFloat = be<f32>;
using ChartSavePointer = gptr<hdChartSave_l>;
static ChartStrings6 sTriforcePaneNames;
static ChartStrings4 sTriforceArchiveNames;
static hdUiSafeString sTriforceLayoutName;
static ChartStrings3 sTriforceChildNames;
static ChartStrings3 sTriforceChildTypes;
static ChartStrings9 sTriforceAnimationGroups;
static ChartStrings10 sTriforceAnimationNames;
static hdUiStringTable sTriforceStringVtable;
static hdPartsVtbl_c sTriforcePartVtable;
static u8 sTriforceHolderVtable;
static u8 sTriforceSecondaryVtable;
static ChartIndices10 sTriforceAnimationGroupIndices;
static ChartUiSystemPointer sTriforceUiSystem;
static hdLayoutRegistration_c sTriforceRegistration;
static ChartFlag sTriforcePrimaryLock;
static hdLayoutRegistration_c sTriforcePrimaryLockRegistration;
static ChartFlag sTriforceSecondaryLock;
static hdLayoutRegistration_c sTriforceSecondaryLockRegistration;
static ChartFloat sTriforceMinimumAngle;
static ChartFloat sTriforceMinimumAngleSeed;
static ChartFloat sTriforceMaximumAngle;
static ChartFloat sTriforceMaximumAngleSeed;
static ChartFloat sTriforceWorldExtentX;
static ChartFloat sTriforceWorldExtentSeed;
static ChartFloat sTriforceWorldExtentY;
static ChartFloat sTriforceZoomLimitX;
static ChartFloat sTriforceZoomLimitSeed;
static ChartFloat sTriforceZoomLimitY;
static hdChartRegistrationList_l sTriforceRegistrationList;
extern void* triforceChartAllocate(u32);
extern void triforceChartDelete(void*);
extern void triforceChartDestroyLinkPos(hdSeaChartLinkPos_c*,u32);
extern void triforceChartConstructLinkPos(hdSeaChartLinkPos_c*);
extern void triforceChartConfigureAnimations(hdSeaChartLinkPos_c*,u32,u32,u32,u32);
extern void triforceChartSetupLayout(hdSeaChartLinkPos_c*);
extern void triforceChartDestroyLinkPosLayout(hdSeaChartLinkPos_c*);
extern void triforceChartDetachArchives(hdParts_c*);
extern void triforceChartAttachArchives(hdParts_c*,hdUiSafeString*,u32);
extern u32 triforceChartCreateChild(hdParts_c*,u32,hdPartsDesc_c*);
extern void triforceChartBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*);
extern void triforceChartRegister(hdLayoutRegistration_c*);
extern void triforceChartConstructPrimaryLock(be<u8>*);
extern void triforceChartConstructSecondaryLock(be<u8>*);
/* 026ED828 */
// hdTriforceChart_c::~hdTriforceChart_c() (GHS deleting destructor: restores the base vtables, frees when flags & 1)
void hdTriforceChart_c::destroy(hdTriforceChart_c* chart, u32 flags) {
    if (!chart) return;
    chart->mStateHolder.mHolderVtable = &sTriforceHolderVtable;
    chart->mSecondaryVtable = &sTriforceSecondaryVtable;
    triforceChartDestroyLinkPos(chart,0);
    if (flags & 1) triforceChartDelete(chart);
}
/* 026ED894 */
hdTriforceChart_c* hdTriforceChart_c::construct(hdTriforceChart_c* part) {
    if (!part) part = static_cast<hdTriforceChart_c*>(triforceChartAllocate(sizeof(hdTriforceChart_c)));
    if (part) {
        triforceChartConstructLinkPos(part);
        part->mVtable = &sTriforcePartVtable;
        part->mSecondaryVtable = &sTriforceSecondaryVtable;
        part->mStateHolder.mHolderVtable = &sTriforceHolderVtable;
        if (!ea(part->mPieceStatus)) triforceChartAllocate(sizeof(part->mPieceStatus));
        for (u32 i=0;i<8;++i) part->mPieceStatus[i]=0;
    }
    return part;
}
/* 026ED928 */
void hdTriforceChart_c::initialize() {
    mPaneNameCount = 6;
    mPaneNames = sTriforcePaneNames.mValues;
    triforceChartConfigureAnimations(this,0,9,0,1);
    triforceChartSetupLayout(this);
}
/* 026ED980 */
u32 hdTriforceChart_c::create(u32 first,u32 second,u32 third) {
    triforceChartAttachArchives(this,sTriforceArchiveNames.mValues,4);
    u32 target = mVtable->mLoad.get();
    void* resource = mpResAccessor.get();
    if (!call_ptr<u32>(target,this,&sTriforceLayoutName,resource,first,second,third)) return 0;
    if (!call_ptr<u32>(mVtable->mCreateChildren.get(),this)) return 0;
    initialize();
    return 1;
}
/* 026EDA5C */
void hdTriforceChart_c::destroyLayout() {
    triforceChartDestroyLinkPosLayout(this);
    triforceChartDetachArchives(this);
}
static ChartSavePointer sTriforceSave;
extern u32 triforceChartHasTriforceMap(void*,u32);
extern u32 triforceChartHasTriforcePiece(void*,u32);
/* 026EDA90 */
void hdTriforceChart_c::refreshPieces() {
    for (u32 i=0;i<8;++i) {
        u32 status=0;
        if (triforceChartHasTriforceMap(sTriforceSave.get()->mMap,i)) {
            status=1;
            if (triforceChartHasTriforcePiece(sTriforceSave.get()->mCollect,u8(i))) status=2;
        }
        if (mPieceStatus[i] != status) {
            mPieceStatus[i]=status;
            hdAnimSet_setFrame(mpLayout->animSet(),i+1,i+1,f32(status));
        }
    }
}
/* 026EDBC0 */
void hdTriforceChart_c::executeState() {
    refreshPieces();
    hdParts_executeStateMachine(stateMachine());
}
/* 026EDBF4 */
u32 hdTriforceChart_c::createChild(hdPartsDesc_c* descriptor) {
    hdUiSafeString* actual=&hdChartChildName(descriptor)->mName;
    for (u32 i=0;i<3;++i) {
        actual->assureTermination();
        actual->assureTermination();
        hdUiSafeString* expected=&sTriforceChildNames.mValues[i];
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
        if (matches) return triforceChartCreateChild(this,i,descriptor);
    }
    return 1;
}
/* 026EDCFC */
u32 hdTriforceChart_c::createLayout(void* name,u32 resource,u32 first,u32 second,void* descriptor) {
    auto* layout=static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c),sTriforceUiSystem->mHeap.get(),4));
    if (layout) layout=hdPartsLayout_c::construct(layout);
    mpLayout=layout;
    if (!layout || !layout->vLoad(name,resource,first,second,10,9,descriptor)) return 0;
    for (u32 i=0;i<10;++i) {
        hdPartsLayout_c* current=mpLayout.get();
        u32 group=sTriforceAnimationGroupIndices.mValues[i];
        hdAnimSet* set=current->animSet();
        triforceChartBindAnimation(set,i,&sTriforceAnimationNames.mValues[i],&sTriforceAnimationGroups.mValues[group]);
    }
    vCalcLayout();
    return 1;
}
/* 026EDE28 */
void hdTriforceChart_c::initializeStatics() {
    sTriforceRegistrationList.mCapacity = 0;
    sTriforceRegistrationList.mCount = 0;
    sTriforceRegistrationList.mTail = 0;
    sTriforceRegistrationList.mHead = 0;
    triforceChartRegister(&sTriforceRegistration);
    sTriforceMinimumAngle = sTriforceMinimumAngleSeed;
    sTriforceMaximumAngle = sTriforceMaximumAngleSeed;
    triforceChartConstructPrimaryLock(&sTriforcePrimaryLock);
    triforceChartRegister(&sTriforcePrimaryLockRegistration);
    triforceChartConstructSecondaryLock(&sTriforceSecondaryLock);
    triforceChartRegister(&sTriforceSecondaryLockRegistration);
    sTriforceWorldExtentX = sTriforceWorldExtentSeed;
    sTriforcePaneNames.mValues[0].mVtable = &sTriforceStringVtable;
    sTriforcePaneNames.mValues[0].mText = at<be<u8>>(guest_addr("N_All_01"));
    sTriforceWorldExtentY = sTriforceWorldExtentSeed;
    sTriforceZoomLimitX = sTriforceZoomLimitSeed;
    sTriforcePaneNames.mValues[1].mVtable = &sTriforceStringVtable;
    sTriforceAnimationGroups.mValues[0].mVtable = &sTriforceStringVtable;
    sTriforcePaneNames.mValues[1].mText = at<be<u8>>(guest_addr("N_Grid_00"));
    sTriforcePaneNames.mValues[2].mVtable = &sTriforceStringVtable;
    sTriforcePaneNames.mValues[2].mText = at<be<u8>>(guest_addr("N_Grid_01"));
    sTriforcePaneNames.mValues[3].mVtable = &sTriforceStringVtable;
    sTriforceAnimationGroups.mValues[0].mText = at<be<u8>>(guest_addr("G_All_00"));
    sTriforcePaneNames.mValues[3].mText = at<be<u8>>(guest_addr("L_MapAreaNow_00"));
    sTriforceAnimationGroups.mValues[1].mVtable = &sTriforceStringVtable;
    sTriforceAnimationGroups.mValues[1].mText = at<be<u8>>(guest_addr("G_Triforce_01"));
    sTriforceAnimationGroups.mValues[2].mVtable = &sTriforceStringVtable;
    sTriforceAnimationGroups.mValues[2].mText = at<be<u8>>(guest_addr("G_Triforce_02"));
    sTriforcePaneNames.mValues[4].mVtable = &sTriforceStringVtable;
    sTriforceAnimationGroups.mValues[3].mVtable = &sTriforceStringVtable;
    sTriforceAnimationGroups.mValues[3].mText = at<be<u8>>(guest_addr("G_Triforce_03"));
    sTriforceAnimationGroups.mValues[4].mVtable = &sTriforceStringVtable;
    sTriforceAnimationGroups.mValues[4].mText = at<be<u8>>(guest_addr("G_Triforce_04"));
    sTriforceAnimationGroups.mValues[5].mVtable = &sTriforceStringVtable;
    sTriforceAnimationGroups.mValues[5].mText = at<be<u8>>(guest_addr("G_Triforce_05"));
    sTriforceAnimationGroups.mValues[6].mVtable = &sTriforceStringVtable;
    sTriforceAnimationGroups.mValues[6].mText = at<be<u8>>(guest_addr("G_Triforce_06"));
    sTriforceAnimationGroups.mValues[7].mVtable = &sTriforceStringVtable;
    sTriforcePaneNames.mValues[4].mText = at<be<u8>>(guest_addr("L_BtnBack_00"));
    sTriforcePaneNames.mValues[5].mVtable = &sTriforceStringVtable;
    sTriforcePaneNames.mValues[5].mText = at<be<u8>>(guest_addr("L_LinkPos_00"));
    sTriforceAnimationNames.mValues[0].mVtable = &sTriforceStringVtable;
    sTriforceAnimationNames.mValues[0].mText = at<be<u8>>(guest_addr("InAll"));
    sTriforceAnimationNames.mValues[1].mVtable = &sTriforceStringVtable;
    sTriforceAnimationNames.mValues[1].mText = at<be<u8>>(guest_addr("TriforceGet"));
    sTriforceAnimationNames.mValues[2].mVtable = &sTriforceStringVtable;
    sTriforceAnimationNames.mValues[2].mText = at<be<u8>>(guest_addr("TriforceGet"));
    sTriforceAnimationNames.mValues[3].mVtable = &sTriforceStringVtable;
    sTriforceAnimationGroups.mValues[7].mText = at<be<u8>>(guest_addr("G_Triforce_07"));
    sTriforceAnimationNames.mValues[3].mText = at<be<u8>>(guest_addr("TriforceGet"));
    sTriforceAnimationNames.mValues[4].mVtable = &sTriforceStringVtable;
    sTriforceAnimationNames.mValues[4].mText = at<be<u8>>(guest_addr("TriforceGet"));
    sTriforceAnimationNames.mValues[5].mVtable = &sTriforceStringVtable;
    sTriforceChildTypes.mValues[0].mVtable = &sTriforceStringVtable;
    sTriforceAnimationNames.mValues[5].mText = at<be<u8>>(guest_addr("TriforceGet"));
    sTriforceAnimationNames.mValues[6].mVtable = &sTriforceStringVtable;
    sTriforceAnimationNames.mValues[6].mText = at<be<u8>>(guest_addr("TriforceGet"));
    sTriforceAnimationNames.mValues[7].mVtable = &sTriforceStringVtable;
    sTriforceAnimationNames.mValues[7].mText = at<be<u8>>(guest_addr("TriforceGet"));
    sTriforceAnimationNames.mValues[8].mVtable = &sTriforceStringVtable;
    sTriforceAnimationGroups.mValues[8].mVtable = &sTriforceStringVtable;
    sTriforceAnimationNames.mValues[8].mText = at<be<u8>>(guest_addr("TriforceGet"));
    sTriforceChildTypes.mValues[2].mVtable = &sTriforceStringVtable;
    sTriforceAnimationGroups.mValues[8].mText = at<be<u8>>(guest_addr("G_Triforce_08"));
    sTriforceChildTypes.mValues[1].mText = at<be<u8>>(guest_addr("BtnBack_00"));
    sTriforceChildTypes.mValues[1].mVtable = &sTriforceStringVtable;
    sTriforceAnimationNames.mValues[9].mVtable = &sTriforceStringVtable;
    sTriforceChildTypes.mValues[2].mText = at<be<u8>>(guest_addr("LinkPos_00"));
    sTriforceArchiveNames.mValues[0].mVtable = &sTriforceStringVtable;
    sTriforceArchiveNames.mValues[0].mText = at<be<u8>>(guest_addr("Triforce_00"));
    sTriforceArchiveNames.mValues[1].mVtable = &sTriforceStringVtable;
    sTriforceArchiveNames.mValues[1].mText = at<be<u8>>(guest_addr("MapAreaNow_00"));
    sTriforceArchiveNames.mValues[2].mVtable = &sTriforceStringVtable;
    sTriforceAnimationNames.mValues[9].mText = at<be<u8>>(guest_addr("OutAll"));
    sTriforceArchiveNames.mValues[2].mText = at<be<u8>>(guest_addr("BtnBack_00"));
    sTriforceArchiveNames.mValues[3].mVtable = &sTriforceStringVtable;
    sTriforceArchiveNames.mValues[3].mText = at<be<u8>>(guest_addr("LinkPos_00"));
    sTriforceChildNames.mValues[0].mVtable = &sTriforceStringVtable;
    sTriforceChildNames.mValues[0].mText = at<be<u8>>(guest_addr("L_MapAreaNow_00"));
    sTriforceChildNames.mValues[1].mVtable = &sTriforceStringVtable;
    sTriforceChildNames.mValues[1].mText = at<be<u8>>(guest_addr("L_BtnBack_00"));
    sTriforceChildNames.mValues[2].mVtable = &sTriforceStringVtable;
    sTriforceChildTypes.mValues[0].mText = at<be<u8>>(guest_addr("MapAreaNow_00"));
    sTriforceChildNames.mValues[2].mText = at<be<u8>>(guest_addr("L_LinkPos_00"));
    sTriforceLayoutName.mVtable = &sTriforceStringVtable;
    sTriforceLayoutName.mText = at<be<u8>>(guest_addr("Triforce_00"));
    sTriforceZoomLimitY = sTriforceZoomLimitSeed;
}
/* 026ED728 */
u32 hdTriforceChart_c::visitorWord() { return mVisitorWord; }
/* WWHD glue (tools/verify/autoglue.py)
ext 0273AD10 void* triforceChartAllocate(u32)
ext 0273AF40 void triforceChartDelete(void*)
ext 02683938 void triforceChartDestroyLinkPos(hdSeaChartLinkPos_c*,u32)
ext 026842D4 void triforceChartConstructLinkPos(hdSeaChartLinkPos_c*)
ext 02684AB4 void triforceChartConfigureAnimations(hdSeaChartLinkPos_c*,u32,u32,u32,u32)
ext 026847F4 void triforceChartSetupLayout(hdSeaChartLinkPos_c*)
ext 026844C0 void triforceChartDestroyLinkPosLayout(hdSeaChartLinkPos_c*)
ext 026F91F8 void triforceChartDetachArchives(hdParts_c*)
ext 026F90D8 void triforceChartAttachArchives(hdParts_c*,hdUiSafeString*,u32)
ext 026F92C0 u32 triforceChartCreateChild(hdParts_c*,u32,hdPartsDesc_c*)
ext 02004E04 void triforceChartBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 028F026C void triforceChartRegister(hdLayoutRegistration_c*)
ext 028ED6F8 void triforceChartConstructPrimaryLock(be<u8>*)
ext 028EAB2C void triforceChartConstructSecondaryLock(be<u8>*)
ext 025B8308 u32 triforceChartHasTriforceMap(void*,u32)
ext 025B7C50 u32 triforceChartHasTriforcePiece(void*,u32)
data sTriforcePaneNames 1049D2F8
data sTriforceArchiveNames 1049D358
data sTriforceLayoutName 1049D410
data sTriforceChildNames 1049D328
data sTriforceChildTypes 1049D340
data sTriforceAnimationGroups 1049D378
data sTriforceAnimationNames 1049D3C0
data sTriforceStringVtable 10108964
data sTriforcePartVtable 10108AF0
data sTriforceHolderVtable 10108BF0
data sTriforceSecondaryVtable 10108C00
data sTriforceAnimationGroupIndices 1010897C
data sTriforceUiSystem 1018C404
data sTriforceRegistration 101F7078
data sTriforcePrimaryLock 1049D2E4
data sTriforcePrimaryLockRegistration 101F7084
data sTriforceSecondaryLock 1049D2E5
data sTriforceSecondaryLockRegistration 101F7090
data sTriforceMinimumAngle 1049D2CC
data sTriforceMinimumAngleSeed 101089B0
data sTriforceMaximumAngle 1049D2D0
data sTriforceMaximumAngleSeed 101089B4
data sTriforceWorldExtentX 1049D2D8
data sTriforceWorldExtentSeed 101089B8
data sTriforceWorldExtentY 1049D2D4
data sTriforceZoomLimitX 1049D2E0
data sTriforceZoomLimitSeed 101089BC
data sTriforceZoomLimitY 1049D2DC
data sTriforceRegistrationList 1049D2E8
data sTriforceSave 101F84DC
str 10108AA0 "N_All_01" in 026EDE28
str 101089F8 "N_Grid_00" in 026EDE28
str 10108A04 "N_Grid_01" in 026EDE28
str 10108ACC "G_All_00" in 026EDE28
str 101089D0 "L_MapAreaNow_00" in 026EDE28
str 10108A10 "G_Triforce_01" in 026EDE28
str 10108A20 "G_Triforce_02" in 026EDE28
str 10108A30 "G_Triforce_03" in 026EDE28
str 10108A40 "G_Triforce_04" in 026EDE28
str 10108A50 "G_Triforce_05" in 026EDE28
str 10108A60 "G_Triforce_06" in 026EDE28
str 10108AAC "L_BtnBack_00" in 026EDE28
str 10108ABC "L_LinkPos_00" in 026EDE28
str 101089C0 "InAll" in 026EDE28
str 101089E0 "TriforceGet" in 026EDE28
str 10108A70 "G_Triforce_07" in 026EDE28
str 10108A80 "G_Triforce_08" in 026EDE28
str 10108AD8 "BtnBack_00" in 026EDE28
str 10108AE4 "LinkPos_00" in 026EDE28
str 101089EC "Triforce_00" in 026EDE28
str 10108A90 "MapAreaNow_00" in 026EDE28
str 101089C8 "OutAll" in 026EDE28
*/

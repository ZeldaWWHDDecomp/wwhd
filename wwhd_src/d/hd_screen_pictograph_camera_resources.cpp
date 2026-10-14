/* HD Picto Box camera layout resources and state dispatch. */
#include "d/hd_screen_pictograph_camera.h"
#include "d/hd_screen_best_shot_mark.h"
#include "d/d_com_inf_game.h"
#include "d/d_mesg_routing.h"
#include "d/hd_msg_window_base.h"
using namespace gabi;
extern gptr<hdScreenUiSystem>& sBackdropUi;
extern hdPartsStateId_c& sCameraBlankState;
extern be<f32>& sCameraZero;
struct hdPictoCameraArchives { hdUiSafeString mNames[2]; };
struct hdPictoCameraAnimations { hdUiSafeString mNames[5]; };
struct hdPictoCameraGroups { hdUiSafeString mNames[7]; };
struct hdPictoCameraGroupIndices { be<u32> mIndices[7]; };
static hdPictoCameraArchives sCameraArchives;
static hdUiSafeString sCameraLayoutName;
static hdPictoCameraAnimations sCameraAnimationNames;
static hdPictoCameraGroups sCameraGroupNames;
static hdPictoCameraGroupIndices sCameraGroupIndices;
static hdPartsStateId_c sCameraOpenCoverState;
static hdPartsStateId_c sCameraCloseCoverState;
void pictographBoxAttachArchives(hdParts_c*, hdUiSafeString*, u32);
void pictographBoxStopAnimation(hdAnimSet*, u32, u32);
void pictographBoxFinalizeBase(hdParts_c*);
void pictographBoxExecuteMachine(hdStateMachine*);
u32 pictographBoxAnimationFinished(hdAnimSet*, u32);
void pictographBoxBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
/* 026C2C70 */
u32 hdPictographCamera::initialize(u32 heap, u32 resource, void* description) {
    pictographBoxAttachArchives(this, sCameraArchives.mNames, 2);
    if (!vLoad(&sCameraLayoutName, mpResAccessor.v.get(), heap, resource, description) ||
        !call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    bindNumberPanes();
    pictographBoxStopAnimation(animSet(), 0, 0);
    vCalcLayout();
    startState(&sCameraBlankState);
    reinterpret_cast<MesgRouting::Play*>(dComIfGp_get())->mScopeMesgStatus = 0;
    f32 zero = sCameraZero;
    auto* layout = mpLayout.get();
    mScale = zero;
    mpShutter = reinterpret_cast<hdPartsPaneSRT_c*>(layout->lyt()->mpRootPane.get())->find("PF_Pictograph_00");
    if (mpShutter) mpShutter->mFlags &= ~u32(1);
    mSelfieMode = 0;
    return 1;
}
/* 026C2E00 */
void hdPictographCamera::finalizeResources() {
    vDeleteChildren();
    vDestroyLayout();
    pictographBoxFinalizeBase(this);
}
/* 026C2E54 */
void hdPictographCamera::executeState() {
    auto* current = reinterpret_cast<hdPartsStateId_c*>(hdParts_getCurrentStateId(stateMachine()));
    auto* blankTable = sCameraBlankState.mVtable.get();
    u32 currentKey = current->getKey();
    u32 blankKey = call_ptr<u32>(blankTable->mGetKey.get(), &sCameraBlankState);
    if (currentKey != blankKey) {
        reinterpret_cast<WindowPlay*>(dComIfGp_get())->mMessageSoundMode = 0;
        reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mSecondaryButtonIcon = 0;
        reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPrimaryButtonIcon = 0;
        reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mAuxButtonIcon = 0;
    }
    if (!mShutterVisible && mpShutter) mpShutter->mFlags &= ~u32(1);
    pictographBoxExecuteMachine(stateMachine());
}
/* 026C2F2C */
u32 hdPictographCamera::isAnimationFinished() {
    return pictographBoxAnimationFinished(animSet(), 0);
}
/* 026C2F3C */
void hdPictographCamera::requestCoverState() {
    changeState(mSelfieMode?&sCameraCloseCoverState:&sCameraOpenCoverState);
}
struct hdPictoCameraChildDescription { u8 mPrefix[8]; hdUiSafeString mResource; };
u32 pictographBoxCreateBaseChild(hdParts_c*, u32, hdPartsDesc_c*);
/* 026C2F60 */
u32 hdPictographCamera::createNamedChild(hdPartsDesc_c* description) {
    auto* names = reinterpret_cast<hdPictoCameraChildDescription*>(description);
    hdScreenHeap* heap = sBackdropUi->mHeap;
    auto& resource = names->mResource;
    resource.assureTermination();
    resource.assureTermination();
    auto* cached = resource.mText.get();
    sBestShotPartNameBestShotInit.assureTermination();
    bool equal = cached == sBestShotPartNameBestShotInit.mText.get();
    if (!equal) {
        auto* left = reinterpret_cast<be<u8>*>(resource.mText.get());
        auto* right = reinterpret_cast<be<u8>*>(sBestShotPartNameBestShotInit.mText.get());
        for (u32 index = 0; index<0x40001; ++index) {
            u8 a = left[index], b = right[index];
            if (a != b) break;
            if (!a) { equal = true; break; }
        }
    }
    if (!equal) return pictographBoxCreateBaseChild(this, 0, description);
    auto* slot = childSlot(0);
    auto* part = static_cast<hdBestShotMark_c*>(hdParts_new(sizeof(hdBestShotMark_c), heap, 4));
    if (part) part = hdBestShotMark_c::construct(part);
    *slot=part;
    if (!child(0)) return 0;
    return static_cast<hdBestShotMark_c*>(child(0))->initialize(0, 0, description);
}
/* 026C30C0 */
u32 hdPictographCamera::loadLayout(hdUiSafeString* name, u32 accessor, u32 heap, u32 resource, void* description) {
    auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sBackdropUi->mHeap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, accessor, heap, resource, 7, 5, description)) return 0;
    for (u32 index = 0; index<7; ++index) {
        u32 group = sCameraGroupIndices.mIndices[index];
        pictographBoxBindAnimation(animSet(), index, &sCameraGroupNames.mNames[index], &sCameraAnimationNames.mNames[group]);
    }
    vCalcLayout();
    return 1;
}
extern hdPartsStateId_c& sCameraScaleState;
extern hdPartsStateId_c& sCameraConfirmState;
extern hdPartsStateId_c& sCameraFullState;
extern hdPartsStateId_c& sCameraSelfieCloseState;
extern hdPartsStateId_c& sCameraWaitCoverState;
extern hdPartsStateId_c& sCameraSelfieReverseState;
extern be<u32>& sBoxStateGuard;
extern be<u32>& sBoxStateCounter;
extern hdStateId& sBoxNullState;
static hdPartsStateId_c sCameraInAllState;
static hdPartsStateId_c sCameraShutterState;
static hdPartsStateId_c sCameraMaxCountState;
static hdPartsStateId_c sCameraOutAllState;
static hdPartsStateId_c sCameraListState;
static hdPartsStateId_c sCameraResetState;
static hdStateId sCameraBlankNullState;
static hdPartsStateIdVtbl_c sCameraStateTable;
extern be<u8>& sCameraPaneStringTable;
struct hdPictoCameraStaticWords { be<u32> mWords[4]; };
struct hdPictoCameraStaticRange { be<f32> mMinimum, mMaximum; be<u8> mFirst, mSecond; };
static hdPictoCameraStaticWords sCameraStaticWords;
static hdPictoCameraStaticRange sCameraStaticRange;
static hdPictoRegistration sCameraWordsRegistration;
static hdPictoRegistration sCameraFirstRegistration;
static hdPictoRegistration sCameraSecondRegistration;
static f32 sCameraMinimum;
static f32 sCameraMaximum;
static hdUiSafeString sCameraChildName;
static hdUiSafeString sCameraChildClass;
void pictographRegisterStatic(hdPictoRegistration*);
void pictographConstructFirstFlag(be<u8>*);
void pictographConstructSecondFlag(be<u8>*);
static void setCameraString(hdUiSafeString& value, const char* text) {
    value.mVtable = reinterpret_cast<hdUiStringTable*>(&sCameraPaneStringTable);
    value.mText.v = guest_addr(text);
}
/* 026C31EC */
void initializePictographCameraStatics() {
    sCameraStaticWords.mWords[3] = 0;
    sCameraStaticWords.mWords[2] = 0;
    sCameraStaticWords.mWords[1] = 0;
    sCameraStaticWords.mWords[0] = 0;
    pictographRegisterStatic(&sCameraWordsRegistration);
    f32 minimum = sCameraMinimum, maximum = sCameraMaximum;
    sCameraStaticRange.mMinimum = minimum;
    sCameraStaticRange.mMaximum = maximum;
    pictographConstructFirstFlag(&sCameraStaticRange.mFirst);
    pictographRegisterStatic(&sCameraFirstRegistration);
    pictographConstructSecondFlag(&sCameraStaticRange.mSecond);
    pictographRegisterStatic(&sCameraSecondRegistration);
    setCameraString(sCameraAnimationNames.mNames[0], "G_All_00");
    setCameraString(sCameraAnimationNames.mNames[1], "G_Scale_00");
    setCameraString(sCameraAnimationNames.mNames[2], "G_Shutter_00");
    setCameraString(sCameraAnimationNames.mNames[3], "G_MaxPictogaraph_00");
    setCameraString(sCameraAnimationNames.mNames[4], "G_Cover_00");
    setCameraString(sCameraGroupNames.mNames[0], "InAll");
    setCameraString(sCameraGroupNames.mNames[1], "Scale");
    setCameraString(sCameraGroupNames.mNames[2], "Shutter");
    setCameraString(sCameraGroupNames.mNames[3], "MaxPictogaraph");
    setCameraString(sCameraGroupNames.mNames[4], "InCoverSelfie");
    setCameraString(sCameraGroupNames.mNames[5], "OutCoverSelfie");
    setCameraString(sCameraGroupNames.mNames[6], "OutAll");
    setCameraString(sCameraArchives.mNames[0], "PictographBox_00");
    setCameraString(sCameraArchives.mNames[1], "BestShotMark_00");
    setCameraString(sCameraLayoutName, "PictographBox_00");
    setCameraString(sCameraChildName, "L_BestShotMark_00");
    setCameraString(sCameraChildClass, "BestShotMark_00");
    u32 number = 0;
    if (sBoxStateGuard) number = sBoxStateCounter;
    else sBoxStateGuard = 1;
    sCameraBlankState.setup(++number, "::StateID_Blank", &sCameraStateTable, 4, &sCameraBlankNullState);
    sCameraInAllState.setup(++number, "::StateID_InAll", &sCameraStateTable, 25, &sBoxNullState);
    sCameraScaleState.setup(++number, "::StateID_Scale", &sCameraStateTable, 28, &sBoxNullState);
    sCameraShutterState.setup(++number, "::StateID_Shutter", &sCameraStateTable, 31, &sBoxNullState);
    sCameraMaxCountState.setup(++number, "::StateID_MaxPictogaraph", &sCameraStateTable, 34, &sBoxNullState);
    sCameraConfirmState.setup(++number, "::StateID_Inquiry", &sCameraStateTable, 37, &sBoxNullState);
    sCameraFullState.setup(++number, "::StateID_Full", &sCameraStateTable, 40, &sBoxNullState);
    sCameraOpenCoverState.setup(++number, "::StateID_InCoverSelfie", &sCameraStateTable, 43, &sBoxNullState);
    sCameraSelfieCloseState.setup(++number, "::StateID_OutCoverSelfie", &sCameraStateTable, 46, &sBoxNullState);
    sCameraWaitCoverState.setup(++number, "::StateID_WaitCover", &sCameraStateTable, 49, &sBoxNullState);
    sCameraCloseCoverState.setup(++number, "::StateID_OutCoverSelfieReverse", &sCameraStateTable, 52, &sBoxNullState);
    sCameraSelfieReverseState.setup(++number, "::StateID_InCoverSelfieReverse", &sCameraStateTable, 55, &sBoxNullState);
    sCameraOutAllState.setup(++number, "::StateID_OutAll", &sCameraStateTable, 58, &sBoxNullState);
    sCameraListState.setup(++number, "::StateID_List", &sCameraStateTable, 61, &sBoxNullState);
    sCameraResetState.setup(++number, "::StateID_Reset", &sCameraStateTable, 64, &sBoxNullState);
    sBoxStateCounter = number;
}
/* WWHD glue
data sCameraInAllState 104996F0
data sCameraShutterState 10499750
data sCameraMaxCountState 10499780
data sCameraOutAllState 10499900
data sCameraListState 10499930
data sCameraResetState 10499960
data sCameraBlankNullState 1049DD60
data sCameraStateTable 101006E0
data sCameraStaticWords 1049963C
data sCameraStaticRange 10499620
data sCameraWordsRegistration 101F68A8
data sCameraFirstRegistration 101F68B4
data sCameraSecondRegistration 101F68C0
data sCameraMinimum 101007B0
data sCameraMaximum 101007B4
data sCameraChildName 10499610
data sCameraChildClass 10499618
str 101007B8 "Shutter" in 026C31EC
str 101007C0 "InAll" in 026C31EC
str 101007C8 "Scale" in 026C31EC
str 101007D0 "OutAll" in 026C31EC
str 101007D8 "BestShotMark_00" in 026C31EC
str 101007E8 "::StateID_Blank" in 026C31EC
str 101007F8 "::StateID_InAll" in 026C31EC
str 10100808 "::StateID_Scale" in 026C31EC
str 10100818 "::StateID_InCoverSelfie" in 026C31EC
str 10100830 "::StateID_OutCoverSelfieReverse" in 026C31EC
str 10100850 "::StateID_Reset" in 026C31EC
str 10100860 "G_MaxPictogaraph_00" in 026C31EC
str 10100874 "::StateID_WaitCover" in 026C31EC
str 10100888 "InCoverSelfie" in 026C31EC
str 10100898 "L_BestShotMark_00" in 026C31EC
str 101008AC "::StateID_Shutter" in 026C31EC
str 101008C0 "::StateID_Inquiry" in 026C31EC
str 101008D4 "G_All_00" in 026C31EC
str 101008E0 "G_Scale_00" in 026C31EC
str 101008EC "G_Shutter_00" in 026C31EC
str 101008FC "G_Cover_00" in 026C31EC
str 10100908 "MaxPictogaraph" in 026C31EC
str 10100918 "OutCoverSelfie" in 026C31EC
str 10100928 "PictographBox_00" in 026C31EC
str 1010093C "::StateID_MaxPictogaraph" in 026C31EC
str 10100958 "::StateID_Full" in 026C31EC
str 10100968 "::StateID_OutCoverSelfie" in 026C31EC
str 10100984 "::StateID_InCoverSelfieReverse" in 026C31EC
str 101009A4 "::StateID_OutAll" in 026C31EC
str 101009B8 "::StateID_List" in 026C31EC
ext 02624D84 hdBestShotMark_c* hdBestShotMark_c::construct(hdBestShotMark_c*)
ext 02624E10 void hdBestShotMark_c::initializePane()
data sCameraArchives 104996AC
data sCameraLayoutName 10499990
data sCameraAnimationNames 1049964C
data sCameraGroupNames 10499674
data sCameraGroupIndices 1010067C
data sCameraOpenCoverState 10499810
data sCameraCloseCoverState 104998A0
str 1010079C "PF_Pictograph_00" in 026C2C70
*/

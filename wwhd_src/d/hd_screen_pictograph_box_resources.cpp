/* HD PictographBox resource loading and child construction. */
#include "d/hd_screen_pictograph_box.h"
#include "d/hd_parts_button.h"
using namespace gabi;
extern gptr<hdScreenUiSystem>& sBackdropUi;
struct hdPictoAnimationNames { hdUiSafeString mNames[1]; };
struct hdPictoGroupNames { hdUiSafeString mNames[2]; };
struct hdPictoGroupIndices { be<u32> mIndices[2]; };
static hdPictoAnimationNames sBoxAnimationNames;
static hdPictoGroupNames sBoxGroupNames;
static hdPictoGroupIndices sBoxGroupIndices;
void pictographBoxBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
struct hdPictoBoxArchives { hdUiSafeString mNames[4]; };
struct hdPictoSequenceButton : hdButtonPart_c { be<u32> mReserved74, mSequenceMode; };
WWHD_OFFSET(hdPictoSequenceButton, mSequenceMode, 0x78);
static hdPictoBoxArchives sBoxArchives;
static hdUiSafeString sBoxLayoutName;
struct hdPictoChildNames { hdUiSafeString mNames[3]; };
static hdPictoChildNames sBoxChildNames;
extern hdUiSafeString& sBackdropArchive;
struct hdPictoChildDescription {
    u8 mPrefix[8];
    hdUiSafeString mResource;
    u8 mReserved10[4];
    hdUiSafeString mPaneName;
};
WWHD_OFFSET(hdPictoChildDescription, mPaneName, 0x14);
u32 pictographBoxCreateBaseChild(hdParts_c*, u32, hdPartsDesc_c*);
static bool samePictographName(hdUiSafeString& left, hdUiSafeString& right) {
    left.assureTermination();
    left.assureTermination();
    auto* cached = left.mText.get();
    right.assureTermination();
    if (cached == right.mText.get()) return true;
    auto* a = reinterpret_cast<hdPartsChar_t*>(left.mText.get());
    auto* b = reinterpret_cast<hdPartsChar_t*>(right.mText.get());
    for (u32 i = 0; i<0x40001; ++i) {
        u8 first = u8(a[i]), second = u8(b[i]);
        if (first != second) return false;
        if (!first) return true;
    }
    return false;
}
extern hdPartsStateId_c& sBoxWaitState;
void pictographInEnter(hdPictographBox*);
void pictographBoxAttachArchives(hdParts_c*, hdUiSafeString*, u32);
void pictographBoxStopAnimation(hdAnimSet*, u32, u32);
void pictographBoxLinkButtons(hdButtonPart_c*, u32, u32, u32, u32, u32, u32, u32, u32);
void pictographBoxPanePosition(cXyz*, hdPartsPaneSRT_c*);
void pictographBoxFinalizeBase(hdParts_c*);
void pictographBoxExecuteMachine(hdStateMachine*);
struct hdPictoStaticWords { be<u32> mWords[4]; };
struct hdPictoStaticRange { be<f32> mMinimum, mMaximum; be<u8> mFirst, mSecond; };
static hdPictoStaticWords sBoxStaticWords;
static hdPictoStaticRange sBoxStaticRange;
static hdPictoRegistration sBoxWordsRegistration;
static hdPictoRegistration sBoxFirstRegistration;
static hdPictoRegistration sBoxSecondRegistration;
static f32 sBoxMinimum;
static f32 sBoxMaximum;
static u32 sBoxStateGuard;
static u32 sBoxStateCounter;
static u8 sBoxSafeStringTable;
static hdPictoChildNames sBoxClassNames;
static hdPartsStateId_c sBoxInAllState;
static hdPartsStateId_c sBoxOutAllState;
static hdPartsStateId_c sBoxInState;
static hdPartsStateId_c sBoxOutState;
extern hdPartsStateId_c& sBoxDisplayState;
static hdPartsStateId_c sBoxHideState;
static hdPartsStateId_c sBoxSaveState;
static hdPartsStateIdVtbl_c sBoxVirtualStateTable;
static hdPartsStateIdVtbl_c sBoxDirectStateTable;
static hdStateId sBoxNullState;
void pictographRegisterStatic(hdPictoRegistration*);
void pictographConstructFirstFlag(be<u8>*);
void pictographConstructSecondFlag(be<u8>*);
void pictographInExecute(hdPictographBox*);
void pictographInExit(hdPictographBox*);
void pictographInCheck(hdPictographBox*);
void pictographOutEnter(hdPictographBox*);
void pictographOutExecute(hdPictographBox*);
void pictographOutExit(hdPictographBox*);
void pictographOutCheck(hdPictographBox*);
void pictographDisplayExit(hdPictographBox*);
void pictographOutHide(hdPictographBox*);
void pictographOutWait(hdPictographBox*);
void pictographOutFinish(hdPictographBox*);
void pictographOutDone(hdPictographBox*);
void pictographWaitEnter(hdPictographBox*);
void pictographWaitExecute(hdPictographBox*);
void pictographWaitExit(hdPictographBox*);
void pictographWaitCheck(hdPictographBox*);
void pictographSaveEnter(hdPictographBox*);
void pictographSaveExecute(hdPictographBox*);
void pictographSaveExit(hdPictographBox*);
void pictographSaveCheck(hdPictographBox*);
static void setPictographString(hdUiSafeString& value, const char* text) {
    value.mVtable = reinterpret_cast<hdUiStringTable*>(&sBoxSafeStringTable);
    value.mText.v = guest_addr(text);
}
static void setPictographDelegate(hdPartsPtmf_c& delegate, const void* function) {
    delegate.mThisDelta = 0;
    delegate.mVirtualIndex = -1;
    delegate.mFunction = guest_fn(function);
}
static void setPictographDirectState(hdPartsStateId_c& state, u32 number, const char* name,
    const void* enter, const void* execute, const void* exit, const void* check) {
    state.mNumber = number;
    state.mName.v = guest_addr(name);
    state.mVtable = &sBoxDirectStateTable;
    setPictographDelegate(state.mEnter, enter);
    setPictographDelegate(state.mExecute, execute);
    setPictographDelegate(state.mExit, exit);
    setPictographDelegate(state.mUnused, check);
}
/* 026C4B9C */
u32 hdPictographBox::initialize(u32 heap, u32 resource, void* description) {
    pictographBoxAttachArchives(this, sBoxArchives.mNames, 4);
    if (!vLoad(&sBoxLayoutName, mpResAccessor.v.get(), heap, resource, description) ||
        !call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    pictographBoxStopAnimation(animSet(), 0, 0);
    vCalcLayout();
    bindRootPanes();
    bindBackground();
    bindPicturePanes();
    bindMessageText();
    auto* pane = mPicturePanes[0].get();
    mPictureWidth = pane->mSizeX;
    mPictureHeight = pane->mSizeY;
    reinterpret_cast<hdPictoSequenceButton*>(child(1))->mSequenceMode = 2;
    reinterpret_cast<hdPictoSequenceButton*>(child(2))->mSequenceMode = 2;
    auto* left = reinterpret_cast<hdButtonPart_c*>(child(1));
    auto* right = reinterpret_cast<hdButtonPart_c*>(child(2));
    u32 rightReceiver = right->mpReceiver.v.get();
    pictographBoxLinkButtons(left, 0, 0, 0, 0, 0, 0, rightReceiver, rightReceiver);
    right = reinterpret_cast<hdButtonPart_c*>(child(2));
    left = reinterpret_cast<hdButtonPart_c*>(child(1));
    u32 leftReceiver = left->mpReceiver.v.get();
    pictographBoxLinkButtons(right, 0, 0, 0, 0, 0, 0, leftReceiver, leftReceiver);
    Local<cXyz> position;
    pictographBoxPanePosition(position, mPicturePanes[0]);
    mAnchorPosition.x = position->x;
    mAnchorPosition.y = position->y;
    mAnchorPosition.z = position->z;
    pane = mPicturePanes[0].get();
    reinterpret_cast<be<u32>&>(mLocalPosition.x) = pane->mTranslationXBits;
    reinterpret_cast<be<u32>&>(mLocalPosition.y) = pane->mTranslationYBits;
    reinterpret_cast<be<u32>&>(mLocalPosition.z) = pane->mTranslationZBits;
    startState(&sBoxWaitState);
    return 1;
}
/* 026C4E70 */
void hdPictographBox::finalizeResources() {
    vDeleteChildren();
    vDestroyLayout();
    pictographBoxFinalizeBase(this);
}
/* 026C4EC4 */
void hdPictographBox::executeState() {
    pictographBoxExecuteMachine(stateMachine());
}
/* 026C4ECC */
void hdPictographBox::setPictureTexture(void* texture) {
    reinterpret_cast<hdPictographBackdrop*>(child(0))->setTexture(texture);
}
/* 026C4EE0 */
u32 hdPictographBox::createNamedChild(hdPartsDesc_c* description) {
    auto* names = reinterpret_cast<hdPictoChildDescription*>(description);
    hdScreenHeap* heap = sBackdropUi->mHeap;
    for (u32 index = 0; index<3; ++index) {
        if (!samePictographName(names->mPaneName, sBoxChildNames.mNames[index])) continue;
        if (!samePictographName(names->mResource, sBackdropArchive))
            return pictographBoxCreateBaseChild(this, index, description);
        auto* slot = childSlot(index);
        auto* backdrop = static_cast<hdPictographBackdrop*>(hdParts_new(sizeof(hdPictographBackdrop), heap, 4));
        if (backdrop) backdrop = hdPictographBackdrop::construct(backdrop);
        *slot=backdrop;
        if (!child(index)) return 0;
        return reinterpret_cast<hdPictographBackdrop*>(child(index))->loadBackdrop(0, 0, description);
    }
    return 1;
}
/* 026C5138 */
u32 hdPictographBox::loadLayout(hdUiSafeString* name, u32 accessor, u32 heap, u32 resource, void* description) {
    auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sBackdropUi->mHeap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, accessor, heap, resource, 2, 1, description)) return 0;
    for (u32 index = 0; index<2; ++index) {
        u32 group = sBoxGroupIndices.mIndices[index];
        pictographBoxBindAnimation(animSet(), index, &sBoxGroupNames.mNames[index], &sBoxAnimationNames.mNames[group]);
    }
    vCalcLayout();
    return 1;
}
/* 026C5264 */
void initializePictographBoxStatics() {
    sBoxStaticWords.mWords[2] = 0;
    sBoxStaticWords.mWords[0] = 0;
    sBoxStaticWords.mWords[3] = 0;
    sBoxStaticWords.mWords[1] = 0;
    pictographRegisterStatic(&sBoxWordsRegistration);
    f32 minimum = sBoxMinimum, maximum = sBoxMaximum;
    sBoxStaticRange.mMinimum = minimum;
    sBoxStaticRange.mMaximum = maximum;
    pictographConstructFirstFlag(&sBoxStaticRange.mFirst);
    pictographRegisterStatic(&sBoxFirstRegistration);
    pictographConstructSecondFlag(&sBoxStaticRange.mSecond);
    pictographRegisterStatic(&sBoxSecondRegistration);
    setPictographString(sBoxGroupNames.mNames[0], "InAll");
    setPictographString(sBoxGroupNames.mNames[1], "OutAll");
    setPictographString(sBoxClassNames.mNames[0], "Pictograph_00");
    setPictographString(sBoxClassNames.mNames[1], "BtnSequence_01");
    setPictographString(sBoxClassNames.mNames[2], "BtnSequence_01");
    setPictographString(sBoxAnimationNames.mNames[0], "G_All_00");
    setPictographString(sBoxArchives.mNames[0], "PictographConfirm_00");
    setPictographString(sBoxArchives.mNames[1], "Pictograph_00");
    setPictographString(sBoxArchives.mNames[2], "BestShotMark_00");
    setPictographString(sBoxArchives.mNames[3], "BtnSequence_01");
    setPictographString(sBoxChildNames.mNames[0], "L_Pictograph_00");
    setPictographString(sBoxChildNames.mNames[1], "L_BtnSeqenceL_00");
    setPictographString(sBoxChildNames.mNames[2], "L_BtnSeqenceR_00");
    setPictographString(sBoxLayoutName, "PictographConfirm_00");
    u32 number = 0;
    if (sBoxStateGuard) number = sBoxStateCounter;
    else sBoxStateGuard = 1;
    sBoxInAllState.setup(++number, "::StateID_InAll", &sBoxVirtualStateTable, 25, &sBoxNullState);
    sBoxOutAllState.setup(++number, "::StateID_OutAll", &sBoxVirtualStateTable, 28, &sBoxNullState);
    setPictographDirectState(sBoxInState, ++number, "::StateID_InScaling",
        reinterpret_cast<const void*>(pictographInEnter), reinterpret_cast<const void*>(pictographInExecute), reinterpret_cast<const void*>(pictographInExit), reinterpret_cast<const void*>(pictographInCheck));
    setPictographDirectState(sBoxOutState, ++number, "::StateID_OutScaling",
        reinterpret_cast<const void*>(pictographOutEnter), reinterpret_cast<const void*>(pictographOutExecute), reinterpret_cast<const void*>(pictographOutExit), reinterpret_cast<const void*>(pictographOutCheck));
    sBoxDisplayState.mNumber = ++number;
    sBoxDisplayState.mName.v = guest_addr("::StateID_Display");
    sBoxDisplayState.mVtable = &sBoxDirectStateTable;
    sBoxDisplayState.mEnter.setVirtual(7);
    sBoxDisplayState.mExecute.setVirtual(8);
    sBoxDisplayState.mExit.setVirtual(9);
    setPictographDelegate(sBoxDisplayState.mUnused, reinterpret_cast<const void*>(pictographDisplayExit));
    setPictographDirectState(sBoxHideState, ++number, "::StateID_Out",
        reinterpret_cast<const void*>(pictographOutHide), reinterpret_cast<const void*>(pictographOutWait), reinterpret_cast<const void*>(pictographOutFinish), reinterpret_cast<const void*>(pictographOutDone));
    setPictographDirectState(sBoxWaitState, ++number, "::StateID_OutWait",
        reinterpret_cast<const void*>(pictographWaitEnter), reinterpret_cast<const void*>(pictographWaitExecute), reinterpret_cast<const void*>(pictographWaitExit), reinterpret_cast<const void*>(pictographWaitCheck));
    setPictographDirectState(sBoxSaveState, ++number, "::StateID_SavePicture",
        reinterpret_cast<const void*>(pictographSaveEnter), reinterpret_cast<const void*>(pictographSaveExecute), reinterpret_cast<const void*>(pictographSaveExit), reinterpret_cast<const void*>(pictographSaveCheck));
    sBoxStateCounter = number;
}
/* WWHD glue
data sBoxStaticWords 104999E4
data sBoxStaticRange 104999B0
data sBoxWordsRegistration 101F68FC
data sBoxFirstRegistration 101F6908
data sBoxSecondRegistration 101F6914
data sBoxMinimum 10100D8C
data sBoxMaximum 10100D90
data sBoxStateGuard 101FD8E4
data sBoxStateCounter 101FDD50
data sBoxSafeStringTable 10100C1C
data sBoxClassNames 10499A1C
data sBoxInAllState 10499A64
data sBoxOutAllState 10499A94
data sBoxInState 10499ACC
data sBoxOutState 10499AF8
data sBoxHideState 10499B50
data sBoxSaveState 10499BA8
data sBoxVirtualStateTable 10100CC4
data sBoxDirectStateTable 10100C7C
data sBoxNullState 101FF32C
ext 026C3FD4 void pictographInExecute(hdPictographBox*) callback
ext 026C57AC void pictographInExit(hdPictographBox*) callback
ext 026C57B0 void pictographInCheck(hdPictographBox*) callback
ext 026C4080 void pictographOutEnter(hdPictographBox*) callback
ext 026C4128 void pictographOutExecute(hdPictographBox*) callback
ext 026C57B8 void pictographOutExit(hdPictographBox*) callback
ext 026C57BC void pictographOutCheck(hdPictographBox*) callback
ext 026C57C8 void pictographDisplayExit(hdPictographBox*) callback
ext 026C4204 void pictographOutHide(hdPictographBox*) callback
ext 026C4220 void pictographOutWait(hdPictographBox*) callback
ext 026C57D0 void pictographOutFinish(hdPictographBox*) callback
ext 026C57D4 void pictographOutDone(hdPictographBox*) callback
ext 026C4268 void pictographWaitEnter(hdPictographBox*) callback
ext 026C57DC void pictographWaitExecute(hdPictographBox*) callback
ext 026C57E0 void pictographWaitExit(hdPictographBox*) callback
ext 026C57E4 void pictographWaitCheck(hdPictographBox*) callback
ext 026C4284 void pictographSaveEnter(hdPictographBox*) callback
ext 026C4450 void pictographSaveExecute(hdPictographBox*) callback
ext 026C44A4 void pictographSaveExit(hdPictographBox*) callback
ext 026C57EC void pictographSaveCheck(hdPictographBox*) callback
str 10100D94 "InAll" in 026C5264
str 10100D9C "OutAll" in 026C5264
str 10100DA4 "L_Pictograph_00" in 026C5264
str 10100DB4 "BestShotMark_00" in 026C5264
str 10100DC4 "::StateID_InAll" in 026C5264
str 10100DD4 "::StateID_InScaling" in 026C5264
str 10100DE8 "Pictograph_00" in 026C5264
str 10100DF8 "::StateID_Display" in 026C5264
str 10100E0C "::StateID_Out" in 026C5264
str 10100E1C "::StateID_OutWait" in 026C5264
str 10100E30 "::StateID_SavePicture" in 026C5264
str 10100E48 "G_All_00" in 026C5264
str 10100E54 "L_BtnSeqenceL_00" in 026C5264
str 10100E68 "L_BtnSeqenceR_00" in 026C5264
str 10100E7C "BtnSequence_01" in 026C5264
str 10100E8C "PictographConfirm_00" in 026C5264
str 10100EA4 "::StateID_OutAll" in 026C5264
str 10100EB8 "::StateID_OutScaling" in 026C5264

ext 026C3C60 void pictographInEnter(hdPictographBox*) callback
data sBoxArchives 10499A34
data sBoxChildNames 10499A04
ext 026F92C0 u32 pictographBoxCreateBaseChild(hdParts_c*,u32,hdPartsDesc_c*)
data sBoxLayoutName 10499AC4
ext 026F90D8 void pictographBoxAttachArchives(hdParts_c*,hdUiSafeString*,u32)
ext 02005674 void pictographBoxStopAnimation(hdAnimSet*,u32,u32)
ext 026F5C08 void pictographBoxLinkButtons(hdButtonPart_c*,u32,u32,u32,u32,u32,u32,u32,u32)
ext 02704D2C void pictographBoxPanePosition(cXyz*,hdPartsPaneSRT_c*)
data sBoxAnimationNames 104999A8
data sBoxGroupNames 104999F4
data sBoxGroupIndices 10100C00
ext 02004E04 void pictographBoxBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
ext 026F91F8 void pictographBoxFinalizeBase(hdParts_c*)
ext 02006364 void pictographBoxExecuteMachine(hdStateMachine*)
*/

/* HD saved-picture list layout and child positioning. */
#include "d/hd_screen_pictograph_list.h"
#include "d/d_com_inf_game.h"
#include "d/hd_msg_window_base.h"
using namespace gabi;
extern gptr<hdScreenUiSystem>& sBackdropUi;
extern hdPartsStateId_c& sListBlankState;
extern be<f32>& sListPageWidth;
extern be<f32>& sListDoublePageWidth;
static hdPartsStateId_c sListWaitState;
struct hdPictoListAnimationNames { hdUiSafeString mNames[2]; };
struct hdPictoListGroupNames { hdUiSafeString mNames[3]; };
struct hdPictoListGroupIndices { be<u32> mIndices[3]; };
static hdPictoListAnimationNames sListAnimationNames;
static hdPictoListGroupNames sListGroupNames;
static hdPictoListGroupIndices sListGroupIndices;
void pictographBoxExecuteMachine(hdStateMachine*);
u32 pictographBoxAnimationFinished(hdAnimSet*, u32);
void pictographBoxBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
/* 026C85E0 */
void hdPictographList::executeState() {
    auto* current = reinterpret_cast<hdPartsStateId_c*>(hdParts_getCurrentStateId(stateMachine()));
    auto* blankTable = sListBlankState.mVtable.get();
    u32 currentKey = current->getKey();
    u32 blankKey = call_ptr<u32>(blankTable->mGetKey.get(), &sListBlankState);
    if (currentKey == blankKey) return;
    current = reinterpret_cast<hdPartsStateId_c*>(hdParts_getCurrentStateId(stateMachine()));
    auto* waitTable = sListWaitState.mVtable.get();
    currentKey = current->getKey();
    u32 waitKey = call_ptr<u32>(waitTable->mGetKey.get(), &sListWaitState);
    if (currentKey == waitKey) return;
    reinterpret_cast<WindowPlay*>(dComIfGp_get())->mMessageSoundMode = 0;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mSecondaryButtonIcon = 0;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPrimaryButtonIcon = 0;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mAuxButtonIcon = 0;
    pictographBoxExecuteMachine(stateMachine());
}
/* 026C86D4 */
u32 hdPictographList::isInAnimationFinished() { return pictographBoxAnimationFinished(animSet(), 0); }
/* 026C86E4 */
u32 hdPictographList::isOutAnimationFinished() { return pictographBoxAnimationFinished(animSet(), 2); }
/* 026C86F4 */
void hdPictographList::clearTextureImage(s32 index) { mTextures[index].mTexture.mSurface.mImage = NULL; }
/* 026C8960 */
u32 hdPictographList::loadLayout(hdUiSafeString* name, u32 accessor, u32 heap, u32 resource, void* description) {
    auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sBackdropUi->mHeap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, accessor, heap, resource, 3, 2, description)) return 0;
    for (u32 index = 0; index<3; ++index) {
        u32 group = sListGroupIndices.mIndices[index];
        pictographBoxBindAnimation(animSet(), index, &sListGroupNames.mNames[index], &sListAnimationNames.mNames[group]);
    }
    vCalcLayout();
    return 1;
}
/* 026C8A8C */
void hdPictographList::executePictureChildren() {
    for (u32 index = 0; index<5; ++index) {
        if (child(index)) call_ptr(child(index)->mVtable->mExecuteState.get(), child(index));
    }
}
/* 026C8B34 */
void hdPictographList::positionFollowingPictures(f32 translation) {
    f32 width = sListPageWidth;
    reinterpret_cast<hdPictographButton_c*>(child(3))->setHorizontalPosition(fadds_ppc(translation, width));
    f32 doubleWidth = sListDoublePageWidth;
    reinterpret_cast<hdPictographButton_c*>(child(4))->setHorizontalPosition(fadds_ppc(translation, doubleWidth));
}
/* 026C8BC8 */
void hdPictographList::positionAdjacentPictures(f32 translation) {
    f32 width = sListPageWidth;
    reinterpret_cast<hdPictographButton_c*>(child(1))->setHorizontalPosition(fsubs_ppc(translation, width));
    reinterpret_cast<hdPictographButton_c*>(child(3))->setHorizontalPosition(fadds_ppc(translation, width));
    f32 doubleWidth = sListDoublePageWidth;
    reinterpret_cast<hdPictographButton_c*>(child(4))->setHorizontalPosition(fadds_ppc(translation, doubleWidth));
}
/* 026C8C90 */
void hdPictographList::positionNextPicture(f32 translation) {
    f32 width = sListPageWidth;
    reinterpret_cast<hdPictographButton_c*>(child(3))->setHorizontalPosition(fadds_ppc(translation, width));
}
/* 026C8CB8 */
void hdPictographList::positionPreviousPicture(f32 translation) {
    f32 width = sListPageWidth;
    reinterpret_cast<hdPictographButton_c*>(child(1))->setHorizontalPosition(fsubs_ppc(translation, width));
}
struct hdPictoListChildNames { hdUiSafeString mNames[5]; };
struct hdPictoListChildDescription { u8 mPrefix[8]; hdUiSafeString mResource; u8 mReserved10[4]; hdUiSafeString mPaneName; };
WWHD_OFFSET(hdPictoListChildDescription, mPaneName, 0x14);
static hdPictoListChildNames sListChildNames;
static hdUiSafeString sListPictureButtonResource;
u32 pictographBoxCreateBaseChild(hdParts_c*, u32, hdPartsDesc_c*);
hdParts_c* pictographListConstructPictureButton(hdParts_c*);
u32 pictographListInitializePictureButton(hdParts_c*, u32, u32, hdPartsDesc_c*);
static bool sameListChildName(hdUiSafeString& left, hdUiSafeString& right) {
    left.assureTermination();
    left.assureTermination();
    auto* cached = left.mText.get();
    right.assureTermination();
    if (cached == right.mText.get()) return true;
    auto* a = reinterpret_cast<hdPartsChar_t*>(left.mText.get());
    auto* b = reinterpret_cast<hdPartsChar_t*>(right.mText.get());
    for (u32 index = 0; index<0x40001; ++index) {
        u8 first = a[index], second = b[index];
        if (first != second) return false;
        if (!first) return true;
    }
    return false;
}
/* 026C8708 */
u32 hdPictographList::createNamedChild(hdPartsDesc_c* description) {
    auto* names = reinterpret_cast<hdPictoListChildDescription*>(description);
    hdScreenHeap* heap = sBackdropUi->mHeap;
    for (u32 index = 0; index<5; ++index) {
        if (!sameListChildName(names->mPaneName, sListChildNames.mNames[index])) continue;
        if (!sameListChildName(names->mResource, sListPictureButtonResource))
            return pictographBoxCreateBaseChild(this, index, description);
        auto* slot = childSlot(index);
        auto* part = static_cast<hdParts_c*>(hdParts_new(0x134, heap, 4));
        if (part) part = pictographListConstructPictureButton(part);
        *slot=part;
        if (!child(index)) return 0;
        return pictographListInitializePictureButton(child(index), 0, 0, description);
    }
    return 1;
}
struct hdPictoAnimationDeleteTable { u8 mReserved0[0xC]; gfn mDelete; };
struct hdPictoTextureHeapTable { u8 mReserved0[0x24]; gfn mDestroy; };
WWHD_OFFSET(hdPictoAnimationDeleteTable, mDelete, 0xC);
WWHD_OFFSET(hdPictoTextureHeapTable, mDestroy, 0x24);
static void* sListHeapManager;
hdScreenHeap* pictographListFindAllocationHeap(void*, void*);
void pictographCameraFreePixels(void*);
void pictographBoxFinalizeBase(hdParts_c*);
/* 026C8418 */
void hdPictographList::destroyTextureAnimations() {
    for (s32 index = 0; index<s32(mTextureAnimations.mCount); ++index) {
        if (*mTextureAnimations.slot(index)) {
            auto* animation = mTextureAnimations.slot(index)->get();
            if (animation) {
                auto* table = reinterpret_cast<hdPictoAnimationDeleteTable*>(animation->mVtable.get());
                call_ptr(table->mDelete.get(), animation, 3);
            }
            *mTextureAnimations.slot(index)=NULL;
        }
    }
    if (mTextureAnimations.mpData) {
        auto* heap = pictographListFindAllocationHeap(sListHeapManager, mTextureAnimations.mpData.get());
        heap->free(mTextureAnimations.mpData.get());
        mTextureAnimations.mCount = 0;
        mTextureAnimations.mpData = NULL;
    }
}
/* 026C8544 */
void hdPictographList::finalizeResources() {
    for (u32 index = 0; index<12; ++index) pictographCameraFreePixels(mPixelBuffers[index]);
    auto* heap = mpTextureHeap.get();
    auto* table = reinterpret_cast<hdPictoTextureHeapTable*>(heap->mVtable.get());
    call_ptr(table->mDestroy.get(), heap);
    destroyTextureAnimations();
    vDeleteChildren();
    vDestroyLayout();
    pictographBoxFinalizeBase(this);
}
static hdUiSafeString sListTexturePaneName;
static u32 sListTextureTypeReady;
static u32 sListMaterialTypeReady;
static hdScreenType sListTextureType;
static hdScreenType sListMaterialType;
extern be<u32>& sPaneTypeReady;
extern hdScreenType& sPaneType;
struct hdPictoTextureReceiverView { u8 mReserved0[0x58]; be<u8> mEnabled; };
WWHD_OFFSET(hdPictoTextureReceiverView, mEnabled, 0x58);
hdPartsAnim_c* pictographListConstructReceiver(hdPartsAnim_c*, hdTextPane_c*, u32);
/* 026C7FA0 */
u32 hdPictographList::createTextureAnimations() {
    hdScreenHeap* heap = sBackdropUi->mHeap;
    auto* allocationHeap = heap;
    if (!allocationHeap) allocationHeap = hdParts_getCurrentHeap(sListHeapManager);
    auto* data = static_cast<gptr<hdPartsAnim_c>*>(allocationHeap->allocate(4));
    if (data) { mTextureAnimations.mpData = data; mTextureAnimations.mCount = 1; }
    s32 count = mTextureAnimations.mCount;
    for (s32 index = 0; index<count; ++index) mTextureAnimations.mpData[index] = NULL;
    for (s32 index = 0; index<s32(mTextureAnimations.mCount); ++index) {
        auto* layout = mpLayout->lyt();
        auto* name = &sListTexturePaneName+index;
        auto* nameTable = sListTexturePaneName.mVtable.get();
        call_ptr(nameTable->mAssureTermination.get(), name);
        auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(layout->mpRootPane.get());
        auto* rootTable = root->srtVtbl();
        hdTextPane_c* pane = call_ptr<hdTextPane_c*>(rootTable->mFindPane.get(), root, sListTexturePaneName.mText.get(), 1);
        if (!sListTextureTypeReady) {
            sListTextureTypeReady = 1;
            if (!sListMaterialTypeReady) {
                sListMaterialTypeReady = 1;
                if (!sPaneTypeReady) { sPaneType.mParent = NULL; sPaneTypeReady = 1; }
                sListMaterialType.mParent = &sPaneType;
            }
            sListTextureType.mParent = &sListMaterialType;
        }
        if (pane) {
            auto* paneTable = reinterpret_cast<hdPartsPaneVtbl_c*>(pane->mpVTable.get());
            auto* type = call_ptr<hdScreenType*>(paneTable->mGetRuntimeTypeInfo.get(), pane);
            while (type && type != &sListTextureType) type = type->mParent;
            if (!type) pane = NULL;
        }
        auto* slot = mTextureAnimations.slot(index);
        auto* receiver = static_cast<hdPartsAnim_c*>(hdParts_new(0x60, heap, 4));
        if (receiver) receiver = pictographListConstructReceiver(receiver, pane, 0);
        *slot=receiver;
        if (!*mTextureAnimations.slot(index)) return 0;
    }
    reinterpret_cast<hdPictoTextureReceiverView*>(mTextureAnimations.mpData[0].get())->mEnabled = 1;
    return 1;
}
static hdPictoListGroupNames sListArchives;
static hdUiSafeString sListLayoutName;
extern gptr<hdPictoSaveView>& sCameraSave;
extern be<f32>& sListZero;
extern be<u8>& sListNameStringTable;
hdPictoPictureArchiveView* pictographCameraPictureArchive(be<u32>*);
void pictographListResetArchiveCount(hdPictoPictureArchiveView*, u32);
void pictographBoxAttachArchives(hdParts_c*, hdUiSafeString*, u32);
void pictographBoxStopAnimation(hdAnimSet*, u32, u32);
hdScreenHeap* pictographListCurrentHeap();
hdScreenHeap* pictographListConstructTextureHeap(hdScreenHeap*, hdUiSafeString*, hdScreenHeap*, u32, u32);
void* pictographCameraAllocatePixels(u32, void*, u32);
struct hdPictoTextureHeapActivateTable { u8 mReserved0[0x2C]; gfn mActivate; };
WWHD_OFFSET(hdPictoTextureHeapActivateTable, mActivate, 0x2C);
/* 026C821C */
u32 hdPictographList::initialize(u32 heap, u32 resource, void* description) {
    pictographBoxAttachArchives(this, sListArchives.mNames, 3);
    if (!vLoad(&sListLayoutName, ea(mpResAccessor), heap, resource, description) ||
        !call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    bindNumberPanes();
    createTextureAnimations();
    startState(&sListBlankState);
    positionPictures(sListZero);
    pictographBoxStopAnimation(animSet(), 0, 0);
    vCalcLayout();
    mPage = 1;
    pictographCameraPictureArchive(&sCameraSave->mPictureHolder);
    auto* archive = pictographCameraPictureArchive(&sCameraSave->mPictureHolder);
    if (archive->mCount>12) pictographListResetArchiveCount(pictographCameraPictureArchive(&sCameraSave->mPictureHolder), 0);
    NativeFrame<0x28> frame;
    auto* name = at<hdUiSafeString>(frame+8);
    name->mVtable = reinterpret_cast<hdUiStringTable*>(&sListNameStringTable);
    name->mText.v = guest_addr("PictographListTexture");
    auto* parent = pictographListCurrentHeap();
    mpTextureHeap = pictographListConstructTextureHeap(NULL, name, parent, 1, 0);
    for (u32 index = 0; index<12; ++index) {
        mTextures[index].mTexture.mSurface.mImage = NULL;
        mPixelBuffers[index] = pictographCameraAllocatePixels(0x16A800, mpTextureHeap.get(), 0x800);
    }
    auto* textureHeap = mpTextureHeap.get();
    auto* table = reinterpret_cast<hdPictoTextureHeapActivateTable*>(textureHeap->mVtable.get());
    call_ptr(table->mActivate.get(), textureHeap);
    return 1;
}
struct hdPictoListStaticWords { be<u32> mWords[4]; };
struct hdPictoListStaticRange {
    be<f32> mMinimum, mMaximum, mFarX, mFarY, mNearX, mNearY;
    be<u8> mFirst, mSecond;
};
WWHD_OFFSET(hdPictoListStaticRange, mFirst, 0x18);
static hdPictoListStaticWords sListStaticWords;
static hdPictoListStaticRange sListStaticRange;
static hdPictoRegistration sListWordsRegistration;
static hdPictoRegistration sListFirstRegistration;
static hdPictoRegistration sListSecondRegistration;
static f32 sListMinimum;
static f32 sListMaximum;
static f32 sListFarLimit;
static f32 sListNearLimit;
static hdPictoListChildNames sListChildClasses;
static hdPartsStateIdVtbl_c sListStateTable;
static hdStateId sListBlankNullState;
static hdPartsStateId_c sListInAllState;
static hdPartsStateId_c sListOutAllState;
static hdPartsStateId_c sListBackToBoxState;
static hdPartsStateId_c sListResetState;
extern hdPartsStateId_c& sListPageReadyState;
extern hdPartsStateId_c& sListDragState;
extern hdPartsStateId_c& sListScrollPreviousState;
extern hdPartsStateId_c& sListScrollNextState;
extern hdPartsStateId_c& sListSettleState;
extern hdPartsStateId_c& sListChangePageState;
extern hdPartsStateId_c& sListDoublePageState;
extern hdPartsStateId_c& sListEraseMessageState;
extern hdPartsStateId_c& sListEraseState;
extern hdPartsStateId_c& sListEraseTransitionState;
extern hdPartsStateId_c& sListReplaceMessageState;
extern hdPartsStateId_c& sListPictureSelectedState;
extern be<u32>& sBoxStateGuard;
extern be<u32>& sBoxStateCounter;
extern hdStateId& sBoxNullState;
void pictographRegisterStatic(hdPictoRegistration*);
void pictographConstructFirstFlag(be<u8>*);
void pictographConstructSecondFlag(be<u8>*);
static void setPictographListString(hdUiSafeString& name, const char* text) {
    name.mText.v = guest_addr(text);
    name.mVtable = reinterpret_cast<hdUiStringTable*>(&sListNameStringTable);
}
/* 026C8CE0 */
void initializePictographListStatics() {
    sListStaticWords.mWords[3] = 0;
    sListStaticWords.mWords[2] = 0;
    sListStaticWords.mWords[1] = 0;
    sListStaticWords.mWords[0] = 0;
    pictographRegisterStatic(&sListWordsRegistration);
    f32 minimum = sListMinimum, maximum = sListMaximum;
    sListStaticRange.mMinimum = minimum;
    sListStaticRange.mMaximum = maximum;
    pictographConstructFirstFlag(&sListStaticRange.mFirst);
    pictographRegisterStatic(&sListFirstRegistration);
    pictographConstructSecondFlag(&sListStaticRange.mSecond);
    pictographRegisterStatic(&sListSecondRegistration);
    f32 nearLimit = sListNearLimit, farLimit = sListFarLimit;
    sListStaticRange.mFarX = farLimit;
    sListStaticRange.mNearX = nearLimit;
    sListStaticRange.mFarY = farLimit;
    sListStaticRange.mNearY = nearLimit;
    setPictographListString(sListTexturePaneName, "B_Slide_00");
    setPictographListString(sListAnimationNames.mNames[0], "G_All_00");
    setPictographListString(sListAnimationNames.mNames[1], "G_State_00");
    setPictographListString(sListGroupNames.mNames[0], "InAll");
    setPictographListString(sListGroupNames.mNames[1], "State");
    setPictographListString(sListGroupNames.mNames[2], "OutAll");
    setPictographListString(sListChildNames.mNames[0], "L_BtnPictograph_00");
    setPictographListString(sListChildNames.mNames[1], "L_BtnPictograph_01");
    setPictographListString(sListChildNames.mNames[2], "L_BtnPictograph_02");
    setPictographListString(sListChildNames.mNames[3], "L_BtnPictograph_03");
    setPictographListString(sListChildNames.mNames[4], "L_BtnPictograph_04");
    for (u32 index = 0; index<5; ++index) setPictographListString(sListChildClasses.mNames[index], "BtnPictograph_00");
    setPictographListString(sListArchives.mNames[0], "PictographList_00");
    setPictographListString(sListArchives.mNames[1], "BtnPictograph_00");
    setPictographListString(sListArchives.mNames[2], "BestShotMark_00");
    setPictographListString(sListLayoutName, "PictographList_00");
    u32 number = 0;
    if (sBoxStateGuard) number = sBoxStateCounter;
    else sBoxStateGuard = 1;
    sListBlankState.setup(++number, "::StateID_Blank", &sListStateTable, 4, &sListBlankNullState);
    sListInAllState.setup(++number, "::StateID_InAll", &sListStateTable, 25, &sBoxNullState);
    sListWaitState.setup(++number, "::StateID_State", &sListStateTable, 28, &sBoxNullState);
    sListOutAllState.setup(++number, "::StateID_OutAll", &sListStateTable, 31, &sBoxNullState);
    sListPageReadyState.setup(++number, "::StateID_Wait", &sListStateTable, 34, &sBoxNullState);
    sListDragState.setup(++number, "::StateID_Drag", &sListStateTable, 37, &sBoxNullState);
    sListScrollPreviousState.setup(++number, "::StateID_SlideLeft", &sListStateTable, 40, &sBoxNullState);
    sListScrollNextState.setup(++number, "::StateID_SlideRight", &sListStateTable, 43, &sBoxNullState);
    sListSettleState.setup(++number, "::StateID_SlideEnd0", &sListStateTable, 46, &sBoxNullState);
    sListChangePageState.setup(++number, "::StateID_SlideEnd1", &sListStateTable, 49, &sBoxNullState);
    sListDoublePageState.setup(++number, "::StateID_SlideEnd2", &sListStateTable, 52, &sBoxNullState);
    sListEraseMessageState.setup(++number, "::StateID_CheckDelete", &sListStateTable, 55, &sBoxNullState);
    sListEraseState.setup(++number, "::StateID_Delete", &sListStateTable, 58, &sBoxNullState);
    sListEraseTransitionState.setup(++number, "::StateID_MoveOver", &sListStateTable, 61, &sBoxNullState);
    sListReplaceMessageState.setup(++number, "::StateID_CheckDecide", &sListStateTable, 64, &sBoxNullState);
    sListBackToBoxState.setup(++number, "::StateID_BackToBox", &sListStateTable, 67, &sBoxNullState);
    sListPictureSelectedState.setup(++number, "::StateID_Posting", &sListStateTable, 70, &sBoxNullState);
    sListResetState.setup(++number, "::StateID_Reset", &sListStateTable, 73, &sBoxNullState);
    sBoxStateCounter = number;
}
/* WWHD glue
str 1010117C "InAll" in 026C8CE0
str 10101184 "State" in 026C8CE0
str 1010118C "OutAll" in 026C8CE0
str 10101194 "BestShotMark_00" in 026C8CE0
str 101011A4 "::StateID_Blank" in 026C8CE0
str 101011B4 "::StateID_InAll" in 026C8CE0
str 101011C4 "::StateID_State" in 026C8CE0
str 101011D4 "::StateID_Reset" in 026C8CE0
str 101011E4 "::StateID_SlideLeft" in 026C8CE0
str 101011F8 "::StateID_SlideEnd0" in 026C8CE0
str 1010120C "::StateID_SlideEnd1" in 026C8CE0
str 10101220 "::StateID_SlideEnd2" in 026C8CE0
str 10101234 "::StateID_BackToBox" in 026C8CE0
str 10101248 "PictographList_00" in 026C8CE0
str 1010125C "::StateID_CheckDelete" in 026C8CE0
str 10101274 "::StateID_CheckDecide" in 026C8CE0
str 1010128C "::StateID_Posting" in 026C8CE0
str 101012A0 "G_All_00" in 026C8CE0
str 101012AC "G_State_00" in 026C8CE0
str 101012B8 "L_BtnPictograph_00" in 026C8CE0
str 101012CC "L_BtnPictograph_01" in 026C8CE0
str 101012E0 "L_BtnPictograph_02" in 026C8CE0
str 101012F4 "L_BtnPictograph_03" in 026C8CE0
str 10101308 "L_BtnPictograph_04" in 026C8CE0
str 1010131C "BtnPictograph_00" in 026C8CE0
str 10101330 "B_Slide_00" in 026C8CE0
str 1010133C "::StateID_OutAll" in 026C8CE0
str 10101350 "::StateID_Wait" in 026C8CE0
str 10101360 "::StateID_Drag" in 026C8CE0
str 10101370 "::StateID_SlideRight" in 026C8CE0
str 10101388 "::StateID_Delete" in 026C8CE0
str 1010139C "::StateID_MoveOver" in 026C8CE0
data sListStaticWords 10499C08
data sListStaticRange 10499BDC
data sListWordsRegistration 101F692C
data sListFirstRegistration 101F6938
data sListSecondRegistration 101F6944
data sListMinimum 1010116C
data sListMaximum 10101170
data sListFarLimit 10101174
data sListNearLimit 10101178
data sListChildClasses 10499C68
data sListStateTable 101010A0
data sListBlankNullState 1049DD60
data sListInAllState 10499CE4
data sListOutAllState 10499D44
data sListBackToBoxState 10499F84
data sListResetState 10499FE4
data sListTexturePaneName 10499BD4
data sListTextureTypeReady 101FD938
data sListMaterialTypeReady 101FD88C
data sListTextureType 101FD93C
data sListMaterialType 101FD940
data sListArchives 10499C90
data sListLayoutName 1049A014
ext 02616ED4 hdPartsAnim_c* pictographListConstructReceiver(hdPartsAnim_c*,hdTextPane_c*,u32)
ext 027262B0 void pictographListResetArchiveCount(hdPictoPictureArchiveView*,u32)
ext 0203E8D0 hdScreenHeap* pictographListCurrentHeap()
ext 02753004 hdScreenHeap* pictographListConstructTextureHeap(hdScreenHeap*,hdUiSafeString*,hdScreenHeap*,u32,u32)
str 10101150 "PictographListTexture" in 026C821C
data sListHeapManager 101F8B4C
ext 02755FEC hdScreenHeap* pictographListFindAllocationHeap(void*,void*)
data sListChildNames 10499C40
data sListPictureButtonResource 10490A6C
ext 02641FC8 hdParts_c* pictographListConstructPictureButton(hdParts_c*)
ext 02642044 u32 pictographListInitializePictureButton(hdParts_c*,u32,u32,hdPartsDesc_c*)
data sListWaitState 10499D14
data sListAnimationNames 10499C18
data sListGroupNames 10499C28
data sListGroupIndices 1010104C
*/

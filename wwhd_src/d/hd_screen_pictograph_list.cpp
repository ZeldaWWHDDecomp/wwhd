/* HD saved-picture list, transitions and message states. */
#include "d/hd_screen_pictograph_list.h"
#include "d/hd_screen_pictograph_box.h"
#include "d/d_com_inf_game.h"
#include "d/d_mesg_routing.h"
#include "d/hd_msg_window_base.h"
#include "d/hd_parts_button.h"
#include "d/hd_gamepad_map.h"
using namespace gabi;
static hdPartsVtbl_c sListTable;
static u8 sListHolderTable;
static u8 sListSecondaryTable;
static f32 sListZero;
static f32 sListForward;
static hdPartsStateId_c sListBlankState;
extern be<u8>& sCameraMessageSelection;
extern gptr<hdPictoInputSystem>& sCameraUi;
void* pictographAllocate(u32);
void pictographFree(void*);
void pictographBoxDestroyBase(hdParts_c*, u32);
hdParts_c* pictographBoxConstructBase(hdParts_c*);
void pictographListConstructTextures(HdJpegTextureMap*, u32, u32, u32);
void pictographListTextureConstruct(HdJpegTextureMap*);
void pictographBoxStopAnimation(hdAnimSet*, u32, u32);
void pictographBoxPlayAnimation(hdAnimSet*, u32, u32, f32);
void pictographCameraSetMessage(WindowCore*, u32, u32);
void pictographCameraSetChoices(WindowCore*, u32, u32);
void pictographListLockInput(hdPictoInputSystem*);
/* 026C5918 */
void hdPictographList::destroy(hdPictographList* self, u32 flags) {
    if (!self) return;
    self->mStateHolder.mHolderVtable = &sListHolderTable;
    self->mSecondaryVtable = &sListSecondaryTable;
    pictographBoxDestroyBase(self, 0);
    if (flags&1) pictographFree(self);
}
/* 026C5984 */
void hdPictographList::initializeInAll() {
    reinterpret_cast<MesgRouting::Play*>(dComIfGp_get())->mScopeMesgStatus = 0;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mMessageVariant = 0;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPictureInputMode = 0;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mItemOpen = 0;
}
/* 026C6D4C */
void hdPictographList::resetInProgress() { mTransitionProgress = sListZero; }
/* 026C6E18 */
void hdPictographList::resetOutProgress() { mTransitionProgress = sListZero; }
/* 026C74E8 */
void hdPictographList::initializeEraseMessage() {
    pictographCameraSetMessage(l_windowCore, 0xEE2, 0);
    pictographCameraSetChoices(l_windowCore, 0x8D8, 0x8D9);
    sCameraMessageSelection = 0x18;
}
/* 026C7944 */
void hdPictographList::initializeReplaceMessage() {
    pictographCameraSetMessage(l_windowCore, 0xEE3, 0);
    pictographCameraSetChoices(l_windowCore, 0x90C, 0x8D9);
}
/* 026C7AF0 */
void hdPictographList::initializeOutAll() {
    f32 forward = sListForward;
    pictographBoxPlayAnimation(animSet(), 2, 0, forward);
    pictographListLockInput(sCameraUi);
}
/* 026C7C44 */
void hdPictographList::resetAnimation() {
    pictographBoxStopAnimation(animSet(), 0, 0);
    vCalcLayout();
}
/* 026C7C94 */
void hdPictographList::enterBlank() { changeState(&sListBlankState); }
/* 026C7CA4 */
hdPictographList* hdPictographList::construct(hdPictographList* self) {
    if (!self) {
        self = static_cast<hdPictographList*>(pictographAllocate(sizeof(hdPictographList)));
        if (!self) return NULL;
    }
    pictographBoxConstructBase(self);
    self->mVtable = &sListTable;
    self->mSecondaryVtable = &sListSecondaryTable;
    self->mStateHolder.mHolderVtable = &sListHolderTable;
    if (!u32(ea(self)+offsetof(hdPictographList, mNumberPanes))) pictographAllocate(8);
    auto* panes = &self->mTextureAnimations;
    if (!u32(ea(panes))) panes = static_cast<hdPictoTextureAnimationArray*>(pictographAllocate(8));
    if (panes) { panes->mpData = NULL; panes->mCount = 0; }
    pictographListConstructTextures(self->mTextures, 12, sizeof(HdJpegTextureMap), guest_fn(reinterpret_cast<const void*>(pictographListTextureConstruct)));
    self->mPositionDelegate.mThisDelta = 0;
    self->mPositionDelegate.mVirtualIndex = 0;
    self->mCaptureIndex = -1;
    self->mSelectionChanged = 0;
    self->mpTextureHeap = NULL;
    self->mPositionDelegate.mFunction = 0;
    return self;
}
static hdUiStringTable sListNumberStringTable;
static f32 sListPageWidth;
static f32 sListNegativePageWidth;
void pictographCameraFormatCount(PaneString_l*, const char*, s32);
/* 026C59CC */
void hdPictographList::setDenominator(s32 value) {
    if (!mNumberPanes[1]) return;
    NativeFrame<0x28> frame;
    auto* text = at<hdPictoNumberText>(frame+8);
    text->mString.mpBuffer = text->mBuffer;
    text->mString.mCapacity = 3;
    text->mBuffer[2] = 0;
    text->mBuffer[0] = 0;
    text->mString.mpVTable = reinterpret_cast<gfn*>(&sListNumberStringTable);
    pictographCameraFormatCount(&text->mString, "pictograph list denominator format", value);
    auto* table = reinterpret_cast<hdUiStringTable*>(text->mString.mpVTable.get());
    auto* pane = mNumberPanes[1].get();
    auto* paneTable = reinterpret_cast<hdPictoTextPaneTable*>(pane->mpVTable.get());
    call_ptr(table->mAssureTermination.get(), &text->mString);
    call_ptr(paneTable->mSetText.get(), pane, text->mString.mpBuffer.get(), 0);
}
/* 026C5A80 */
void hdPictographList::setNumerator(s32 value) {
    if (!mNumberPanes[0]) return;
    NativeFrame<0x28> frame;
    auto* text = at<hdPictoNumberText>(frame+8);
    text->mString.mpBuffer = text->mBuffer;
    text->mString.mCapacity = 3;
    text->mBuffer[2] = 0;
    text->mBuffer[0] = 0;
    text->mString.mpVTable = reinterpret_cast<gfn*>(&sListNumberStringTable);
    pictographCameraFormatCount(&text->mString, "pictograph list numerator format", value);
    auto* table = reinterpret_cast<hdUiStringTable*>(text->mString.mpVTable.get());
    auto* pane = mNumberPanes[0].get();
    auto* paneTable = reinterpret_cast<hdPictoTextPaneTable*>(pane->mpVTable.get());
    call_ptr(table->mAssureTermination.get(), &text->mString);
    call_ptr(paneTable->mSetText.get(), pane, text->mString.mpBuffer.get(), 0);
}
/* 026C6B98 */
f32 hdPictographList::clampPageTranslation(f32 value) {
    if (mPageCount != 2) return value;
    f32 zero = sListZero;
    if (mPage == 1) {
        if (zero<value) return zero;
        f32 minimum = sListNegativePageWidth;
        if (value<minimum) return minimum;
    } else {
        if (value<zero) return zero;
        f32 maximum = sListPageWidth;
        if (maximum<value) return maximum;
    }
    return value;
}
static f32 sListDoublePageWidth;
static f32 sListSettleFrames;
static hdPartsStateId_c sListPageReadyState;
u32 pictographListChaseTranslation(be<f32>*, f32, f32);
/* 026C6358 */
void hdPictographList::positionPictures(f32 translation) {
    reinterpret_cast<hdPictographButton_c*>(child(2))->setHorizontalPosition(translation);
    f32 width = sListPageWidth;
    reinterpret_cast<hdPictographButton_c*>(child(1))->setHorizontalPosition(fsubs_ppc(translation, width));
    f32 doubleWidth = sListDoublePageWidth;
    reinterpret_cast<hdPictographButton_c*>(child(0))->setHorizontalPosition(fsubs_ppc(translation, doubleWidth));
    reinterpret_cast<hdPictographButton_c*>(child(3))->setHorizontalPosition(fadds_ppc(translation, width));
    reinterpret_cast<hdPictographButton_c*>(child(4))->setHorizontalPosition(fadds_ppc(translation, doubleWidth));
}
/* 026C6EDC */
void hdPictographList::initializeSettlePage() {
    f32 step = fabsf((f32(mTransitionProgress)/f32(sListSettleFrames)));
    f32 one = sListForward, zero = sListZero;
    mTransitionStep = step;
    mTransitionTarget = zero;
    if (step<one) {
        setNumerator(mPage);
        changeState(&sListPageReadyState);
    }
}
/* 026C6F54 */
void hdPictographList::updateSettlePage() {
    if (pictographListChaseTranslation(&mTransitionProgress, mTransitionTarget, mTransitionStep))
        changeState(&sListPageReadyState);
    else positionPictures(mTransitionProgress);
    vCalcLayout();
}
/* 026C6FDC */
void hdPictographList::initializeChangePage() {
    f32 progress = mTransitionProgress;
    f32 zero = sListZero, width = sListPageWidth;
    if (zero<progress) {
        u8 page = u8(mPage)-1;
        mTransitionTarget = width;
        mPage = page;
        if (!page) mPage = mPageCount;
    } else {
        u8 page = u8(mPage)+1;
        u8 count = mPageCount;
        mTransitionTarget = sListNegativePageWidth;
        mPage = page;
        if (count<page) mPage = 1;
    }
    f32 remaining = fsubs_ppc(width, fabsf(mTransitionProgress));
    f32 step = (remaining/f32(sListSettleFrames));
    f32 one = sListForward;
    mTransitionStep = step;
    if (step<one) {
        setNumerator(mPage);
        changeState(&sListPageReadyState);
    }
}
/* 026C70BC */
void hdPictographList::updateChangePage() {
    if (pictographListChaseTranslation(&mTransitionProgress, mTransitionTarget, mTransitionStep)) {
        setNumerator(mPage);
        changeState(&sListPageReadyState);
    } else positionPictures(mTransitionProgress);
    vCalcLayout();
}
extern gptr<hdPictoSaveView>& sCameraSave;
extern gptr<hdPictoInputOptions>& sCameraOptions;
hdPictoPictureArchiveView* pictographCameraPictureArchive(be<u32>*);
s32 pictographListArchiveIndex(hdPictoPictureArchiveView*, s32);
void* pictographListArchivePixels(hdPictoPictureArchiveView*, s32);
void pictographListDecodePicture(hdPictoInputOptions*, void*, HdJpegTextureMap*, void*, u32);
u32 pictographListArchivePhoto(hdPictoPictureArchiveView*, s32);
u32 pictographCameraConvertPhoto(u32);
u32 pictographListArchivePhotoUsed(hdPictoPictureArchiveView*, s32);
void* pictographCameraPictureSlot(hdPictoPictureArchiveView*, u32);
u32 pictographListPictureIsBestShot(void*);
u32 pictographBoxAnimationFinished(hdAnimSet*, u32);
/* 026C5B34 */
s32 hdPictographList::resolvePictureIndex(s32 index) {
    if (index<0) {
        index = s32(u32(index)+u8(mPageCount));
        if (index<0) return -1;
    } else {
        s32 count = u8(mPageCount);
        if (count <= index) {
            index = s32(u32(index)-u32(count));
            if (index<0) return -1;
        }
    }
    return pictographListArchiveIndex(pictographCameraPictureArchive(&sCameraSave->mPictureHolder), index);
}
/* 026C5BB0 */
void hdPictographList::bindPictureTexture(u32 childIndex, s32 textureIndex) {
    if (textureIndex >= 0) {
        auto* texture = &mTextures[textureIndex];
        if (texture->mTexture.mSurface.mImage) {
            reinterpret_cast<hdPictographButton_c*>(child(childIndex))->setThumbnail(reinterpret_cast<hdPictoTextureExtent_c*>(texture));
            return;
        }
        void* image = pictographListArchivePixels(pictographCameraPictureArchive(&sCameraSave->mPictureHolder), textureIndex);
        auto* options = sCameraOptions.get();
        void* pixels = mPixelBuffers[textureIndex];
        pictographListDecodePicture(options, image, texture, pixels, 1);
    }
    reinterpret_cast<hdPictographButton_c*>(child(childIndex))->restoreThumbnail();
}
/* 026C5CC4 */
void hdPictographList::bindVisibleTextures() {
    s32 picture = resolvePictureIndex(s32(u8(mPage))-1); bindPictureTexture(2, picture);
    picture = resolvePictureIndex(s32(u8(mPage))-2); bindPictureTexture(1, picture);
    picture = resolvePictureIndex(s32(u8(mPage))-3); bindPictureTexture(0, picture);
    picture = resolvePictureIndex(u8(mPage)); bindPictureTexture(3, picture);
    picture = resolvePictureIndex(s32(u8(mPage))+1); bindPictureTexture(4, picture);
}
/* 026C5D84 */
void hdPictographList::updatePictureBestShot(u32 childIndex, s32 pictureIndex) {
    if (pictureIndex >= 0) {
        auto* archive = pictographCameraPictureArchive(&sCameraSave->mPictureHolder);
        u32 photo = u8(pictographListArchivePhoto(archive, pictureIndex));
        photo = pictographCameraConvertPhoto(photo);
        if (u32(photo-0x49)<0x86) {
            archive = pictographCameraPictureArchive(&sCameraSave->mPictureHolder);
            if (!pictographListArchivePhotoUsed(archive, pictureIndex)) {
                archive = pictographCameraPictureArchive(&sCameraSave->mPictureHolder);
                void* picture = pictographCameraPictureSlot(archive, pictureIndex);
                if (pictographListPictureIsBestShot(picture)) {
                    reinterpret_cast<hdPictographButton_c*>(child(childIndex))->showBestShot();
                    return;
                }
            }
        }
    }
    reinterpret_cast<hdPictographButton_c*>(child(childIndex))->hideBestShot();
}
/* 026C5ED8 */
void hdPictographList::updateVisibleBestShots() {
    s32 picture = resolvePictureIndex(s32(u8(mPage))-1); updatePictureBestShot(2, picture);
    picture = resolvePictureIndex(s32(u8(mPage))-2); updatePictureBestShot(1, picture);
    picture = resolvePictureIndex(s32(u8(mPage))-3); updatePictureBestShot(0, picture);
    picture = resolvePictureIndex(u8(mPage)); updatePictureBestShot(3, picture);
    picture = resolvePictureIndex(s32(u8(mPage))+1); updatePictureBestShot(4, picture);
}
/* 026C65D4 */
void hdPictographList::updateInAll() {
    if (pictographBoxAnimationFinished(animSet(), 0)) changeState(&sListPageReadyState);
    vCalcLayout();
}
/* 026C66A4 */
void hdPictographList::updateOutAll() {
    if (pictographBoxAnimationFinished(animSet(), 2)) changeState(&sListBlankState);
    vCalcLayout();
}
/* Box state delegate tails precede the saved-picture list in this unit. */
/* 026C57B0 */
u32 hdPictographBox::checkIn() { return 0; }
/* 026C57BC */
u32 hdPictographBox::checkOut() { return 0; }
/* 026C57C8 */
u32 hdPictographBox::checkDisplayExit() { return 0; }
/* 026C57D4 */
u32 hdPictographBox::checkOutFinished() { return 0; }
/* 026C57E4 */
u32 hdPictographBox::checkWait() { return 0; }
/* 026C57EC */
u32 hdPictographBox::checkSave() { return 0; }
/* 026C57F4 */
u32 pictographDirectStateNumber(hdPartsStateId_c* state) { return state->mNumber; }
struct hdPictoListNumberNames { hdUiSafeString mNames[2]; };
static hdPictoListNumberNames sListNumberNames;
void pictographDestroyStrings(void*, u32, u32, u32, u32, u32);
void pictographListDestroySafeString(hdUiSafeString*);
/* 026C58F4 */
void destroyPictographListNumberNames() {
    pictographDestroyStrings(&sListNumberNames, 2, 8, guest_fn(reinterpret_cast<const void*>(pictographListDestroySafeString)), 0, 0);
}
static u32 sListNumberNamesReady;
static u8 sListNameStringTable;
static hdPictoRegistration sListNumberNameRegistration;
extern be<u32>& sPartsTypeReady;
extern hdScreenType& sPartsType;
extern be<u32>& sPaneTypeReady;
extern hdScreenType& sPaneType;
void pictographRegisterStatic(hdPictoRegistration*);
/* 026C7D80 */
void hdPictographList::bindNumberPanes() {
    if (!sListNumberNamesReady) {
        sListNumberNamesReady = 1;
        sListNumberNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sListNameStringTable);
        sListNumberNames.mNames[0].mText.v = guest_addr("T_Numerator_00");
        sListNumberNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sListNameStringTable);
        sListNumberNames.mNames[1].mText.v = guest_addr("T_Denominator_00");
        pictographRegisterStatic(&sListNumberNameRegistration);
    }
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    for (u32 index = 0; index<2; ++index) {
        auto* name = &sListNumberNames.mNames[index];
        auto* table = root->srtVtbl();
        name->assureTermination();
        hdTextPane_c* pane = call_ptr<hdTextPane_c*>(table->mFindPane.get(), root, name->mText.get(), 1);
        if (!sPartsTypeReady) {
            sPartsTypeReady = 1;
            if (!sPaneTypeReady) { sPaneType.mParent = NULL; sPaneTypeReady = 1; }
            sPartsType.mParent = &sPaneType;
        }
        if (pane) {
            auto* paneTable = reinterpret_cast<hdPartsPaneVtbl_c*>(pane->mpVTable.get());
            auto* type = call_ptr<hdScreenType*>(paneTable->mGetRuntimeTypeInfo.get(), pane);
            while (type && type != &sPartsType) type = type->mParent;
            if (!type) pane = NULL;
        }
        mNumberPanes[index] = pane;
    }
}
static hdPartsStateId_c sListEraseState;
u32 pictographCameraMessageState(WindowCore*);
/* 026C753C */
void hdPictographList::updateEraseMessage() {
    auto* core = l_windowCore.get();
    if (pictographCameraMessageState(core) == 0x12)
        changeState(core->mSelection?&sListEraseState:&sListPageReadyState);
    vCalcLayout();
    reinterpret_cast<WindowPlay*>(dComIfGp_get())->mMessageSoundMode = 0x17;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mSecondaryButtonIcon = 7;
}
static f32 sListScrollStep;
static hdPartsStateId_c sListEraseTransitionState;
void pictographListEraseArchivePicture(hdPictoPictureArchiveView*, u32);
u32 pictographListIsPictureReady(hdParts_c*);
struct hdPictoCaptureControl { u8 mReserved0[0x14]; be<u8> mActive; };
WWHD_OFFSET(hdPictoCaptureControl, mActive, 0x14);
static hdPictoCaptureControl* sListCaptureControl;
/* 026C6468 */
u32 hdPictographList::isPictureListMode() {
    return reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPictureListMode == 2;
}
/* 026C6844 */
void hdPictographList::updateButtonIcons() {
    bool changed = mSelectionChanged;
    auto* play = reinterpret_cast<WindowPlay*>(dComIfGp_get());
    play->mMessageSoundMode = changed?0x17:0x44;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mSecondaryButtonIcon = 7;
    if (!changed) {
        reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPrimaryButtonIcon = 0x48;
        reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mAuxButtonIcon = 0x45;
    }
}
/* 026C6B64 */
void hdPictographList::clearCapturedPicture() {
    s32 index = s8(mCaptureIndex);
    if (index >= 0) {
        mTextures[index].mTexture.mSurface.mImage = NULL;
        mCaptureIndex = -1;
    }
    sListCaptureControl->mActive = 0;
}
/* 026C6D5C */
void hdPictographList::updateScrollNext() {
    f32 translation = mTransitionProgress;
    f32 minimum = sListNegativePageWidth;
    if (!(translation>minimum)) {
        u8 page = u8(mPage)+1;
        u8 count = mPageCount;
        mPage = page;
        if (count<page) { page = 1; mPage = page; }
        setNumerator(page);
        changeState(&sListPageReadyState);
    } else {
        translation = fsubs_ppc(translation, sListScrollStep);
        mTransitionProgress = translation;
        positionPictures(translation);
    }
    vCalcLayout();
}
/* 026C6E28 */
void hdPictographList::updateScrollPrevious() {
    f32 translation = mTransitionProgress;
    f32 maximum = sListPageWidth;
    if (!(maximum>translation)) {
        u8 page = u8(mPage)-1;
        mPage = page;
        if (!page) { page = mPageCount; mPage = page; }
        setNumerator(page);
        changeState(&sListPageReadyState);
    } else {
        translation = fadds_ppc(translation, sListScrollStep);
        mTransitionProgress = translation;
        positionPictures(translation);
    }
    vCalcLayout();
}
/* 026C75D4 */
void hdPictographList::updateErasePicture() {
    if (pictographListIsPictureReady(child(2))) {
        mCaptureIndex = resolvePictureIndex(s32(u8(mPage))-1);
        u32 selected = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mSelectedPicture;
        auto* archive = pictographCameraPictureArchive(&sCameraSave->mPictureHolder);
        pictographListEraseArchivePicture(archive, selected);
        auto* play = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get());
        play->mPendingPictureCount = s16(play->mPendingPictureCount)-1;
        mPageCount = u8(mPageCount)-1;
        changeState(&sListEraseTransitionState);
    }
    vCalcLayout();
}
static f32 sListNegativeDoublePageWidth;
static f32 sListTriplePageWidth;
static u32 sListDoubleWidthReady;
static f32 sListCachedDoubleWidth;
/* 026C714C */
void hdPictographList::initializeDoublePageChange() {
    f32 translation = mTransitionProgress;
    f32 zero = sListZero, doubleWidth = sListDoublePageWidth;
    if (zero<translation) {
        u8 page = u8(mPage)-1;
        mTransitionTarget = doubleWidth;
        if (!page) { mPage = page; page = mPageCount; }
        page = u8(page-1);
        mPage = page;
        if (!page) mPage = mPageCount;
    } else {
        f32 target = sListNegativeDoublePageWidth;
        u8 page = u8(mPage)+1;
        u8 count = mPageCount;
        mTransitionTarget = target;
        mPage = page;
        if (count<page) page = 1;
        page = u8(page+1);
        mPage = page;
        if (count<page) mPage = 1;
    }
    f32 remaining = fsubs_ppc(doubleWidth, fabsf(mTransitionProgress));
    f32 step = remaining/f32(sListSettleFrames);
    f32 one = sListForward;
    mTransitionStep = step;
    if (step<one) {
        setNumerator(mPage);
        changeState(&sListPageReadyState);
    }
}
/* 026C7258 */
void hdPictographList::positionDoublePageChange(f32 translation) {
    f32 zero = sListZero, tripleWidth = sListTriplePageWidth;
    f32 width = sListPageWidth, doubleWidth = sListDoublePageWidth;
    if (zero<translation) {
        reinterpret_cast<hdPictographButton_c*>(child(2))->setHorizontalPosition(translation);
        reinterpret_cast<hdPictographButton_c*>(child(1))->setHorizontalPosition(fsubs_ppc(translation, width));
        reinterpret_cast<hdPictographButton_c*>(child(0))->setHorizontalPosition(fsubs_ppc(translation, doubleWidth));
        reinterpret_cast<hdPictographButton_c*>(child(4))->setHorizontalPosition(fsubs_ppc(translation, tripleWidth));
        s32 picture = resolvePictureIndex(s32(u8(mPage))-2);
        bindPictureTexture(4, picture);
    } else {
        reinterpret_cast<hdPictographButton_c*>(child(2))->setHorizontalPosition(translation);
        reinterpret_cast<hdPictographButton_c*>(child(3))->setHorizontalPosition(fadds_ppc(translation, width));
        reinterpret_cast<hdPictographButton_c*>(child(4))->setHorizontalPosition(fadds_ppc(translation, doubleWidth));
        reinterpret_cast<hdPictographButton_c*>(child(0))->setHorizontalPosition(fadds_ppc(translation, tripleWidth));
        s32 picture = resolvePictureIndex(u8(mPage));
        bindPictureTexture(0, picture);
    }
}
/* 026C7430 */
void hdPictographList::updateDoublePageChange() {
    if (!sListDoubleWidthReady) {
        f32 width = sListDoublePageWidth;
        sListDoubleWidthReady = 1;
        sListCachedDoubleWidth = width;
    }
    if (pictographListChaseTranslation(&mTransitionProgress, mTransitionTarget, mTransitionStep)) {
        setNumerator(mPage);
        changeState(&sListPageReadyState);
    } else positionDoublePageChange(mTransitionProgress);
    vCalcLayout();
}
static f32 sListEmptyFrame;
void pictographListShowPicture(hdParts_c*);
void pictographCameraSetFrame(hdAnimSet*, u32, u32, f32);
/* 026C5F98 */
void hdPictographList::rebuildVisiblePictures() {
    u32 count = mPageCount;
    f32 zero = sListZero;
    bool show[5] = {false, false, false, false, false};
    if (count>2) {
        for (u32 index = 0; index<5; ++index) show[index] = true;
    } else if (count == 2) {
        bool first = mPage == 1;
        show[2] = true;
        show[first?3:1] = true;
    } else if (count) show[2] = true;
    for (u32 index = 0; index<5; ++index) {
        if (show[index]) pictographListShowPicture(child(index));
        else reinterpret_cast<hdPictographButton_c*>(child(index))->hide();
    }
    f32 frame = zero;
    if (!count) frame = sListEmptyFrame;
    else if (count == 1) frame = sListForward;
    pictographCameraSetFrame(animSet(), 1, 1, frame);
    bindVisibleTextures();
    updateVisibleBestShots();
}
/* 026C6764 */
void hdPictographList::initializeWait() {
    rebuildVisiblePictures();
    positionPictures(sListZero);
    u32 selection;
    if (mPageCount) {
        auto* archive = pictographCameraPictureArchive(&sCameraSave->mPictureHolder);
        s32 picture = pictographListArchiveIndex(archive, s32(u8(mPage))-1);
        reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mSelectedPicture = picture;
        vCalcLayout();
        selection = isPictureListMode();
        if (selection) selection = 0xC;
    } else {
        mPage = 0;
        setNumerator(0);
        vCalcLayout();
        selection = isPictureListMode();
        if (selection) selection = 0xC;
    }
    sCameraMessageSelection = selection;
}
static f32 sListNegativeScrollStep;
void pictographListPositionAdjacent(hdPictographList*, f32);
void pictographListPositionNext(hdPictographList*, f32);
void pictographListPositionPrevious(hdPictographList*, f32);
void pictographListPositionFollowing(hdPictographList*, f32);
void pictographListPlaySound(u32);
/* 026C769C */
void hdPictographList::initializeEraseTransition() {
    u32 count = mPageCount;
    mTransitionVelocity = sListScrollStep;
    mTransitionProgress = sListZero;
    mPositionDelegate.mThisDelta = 0;
    mPositionDelegate.mVirtualIndex = -1;
    const void* callback;
    if (count == 2) {
        if (mPage == 1) callback = reinterpret_cast<const void*>(pictographListPositionAdjacent);
        else { mPage = 2; callback = reinterpret_cast<const void*>(pictographListPositionNext); }
    } else if (count == 1) {
        if (mPage == 1) callback = reinterpret_cast<const void*>(pictographListPositionNext);
        else {
            mPage = 1;
            callback = reinterpret_cast<const void*>(pictographListPositionPrevious);
            mTransitionVelocity = sListNegativeScrollStep;
        }
    } else callback = reinterpret_cast<const void*>(pictographListPositionFollowing);
    mPositionDelegate.mFunction = guest_fn(callback);
    if (u8(mPage)>u8(mPageCount)) mPage = 1;
    if (mPageCount) pictographListPlaySound(0x8D6);
}
/* 026C77E8 */
void hdPictographList::updateEraseTransition() {
    f32 translation = mTransitionProgress;
    if (!(sListPageWidth>__builtin_fabsf(translation))) {
        setDenominator(mPageCount);
        if (mPageCount == 2) {
            reinterpret_cast<hdPictographButton_c*>(child(0))->hide();
            reinterpret_cast<hdPictographButton_c*>(child(4))->hide();
            reinterpret_cast<hdPictographButton_c*>(child(mPage == 1?1:3))->hide();
        }
        setNumerator(mPage);
        changeState(&sListPageReadyState);
    } else {
        translation = fsubs_ppc(translation, mTransitionVelocity);
        mTransitionProgress = translation;
        auto& delegate = reinterpret_cast<ptmf<void (hdPictographList::*)(f32)>&>(mPositionDelegate);
        (this->*delegate)(translation);
    }
    vCalcLayout();
}
static hdPartsStateId_c sListReplaceOutState;
extern hdPartsStateId_c& sListWaitState;
u32 pictographBoxAnimationFinished(hdAnimSet*, u32);
void* pictographCameraPictureSlot(hdPictoPictureArchiveView*, u32);
/* 026C798C */
void hdPictographList::updateReplaceMessage() {
    auto* core = l_windowCore.get();
    if (pictographCameraMessageState(core) == 0x12) {
        if (core->mSelection) {
            s32 picture = resolvePictureIndex(s32(u8(mPage))-1);
            reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mSelectedPicture = picture;
            auto* archive = pictographCameraPictureArchive(&sCameraSave->mPictureHolder);
            auto* slot = pictographCameraPictureSlot(archive, picture);
            u32 bestShot = pictographListPictureIsBestShot(slot);
            reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mSelectedBestShot = bestShot;
            reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPictureListMode = 1;
            archive = pictographCameraPictureArchive(&sCameraSave->mPictureHolder);
            u32 photo = pictographListArchivePhoto(archive, picture);
            reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mSelectedPhotoNumber = photo;
            archive = pictographCameraPictureArchive(&sCameraSave->mPictureHolder);
            u32 used = pictographListArchivePhotoUsed(archive, picture);
            reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mSelectedPhotoUsed = used;
            auto* player = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mpPlayer.get();
            player->mCaptureFlags = u32(player->mCaptureFlags)|0x80000;
            reinterpret_cast<MesgRouting::Play*>(dComIfGp_get())->mScopeMesgStatus = 11;
            changeState(&sListReplaceOutState);
        } else changeState(&sListPageReadyState);
    }
    vCalcLayout();
    reinterpret_cast<WindowPlay*>(dComIfGp_get())->mMessageSoundMode = 0x17;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mSecondaryButtonIcon = 7;
}
/* 026C7B34 */
void hdPictographList::updateOutToWait() {
    if (pictographBoxAnimationFinished(animSet(), 2)) changeState(&sListWaitState);
    vCalcLayout();
    updateButtonIcons();
}
extern gptr<EventMgr>& sBoxInput;
void pictographBoxCopyEvent(hdButtonEvent_l*, hdButtonEvent_l*);
Pair32 pictographListDragDisplacement(EventMgr*);
void pictographListTrackReceiver(EventMgr*, hdPartsAnim_c*);
u32 pictographListInputTracking(hdPictoInputSystem*);
void pictographListSelectPicture(void*, s32);
struct hdPictoAnimationReceiverView { u8 mReserved0[0x10]; be<u32> mReceiverId; };
WWHD_OFFSET(hdPictoAnimationReceiverView, mReceiverId, 0x10);
static f32 sListSettleThreshold;
static f32 sListDoubleThreshold;
static hdPartsStateId_c sListDragState;
static hdPartsStateId_c sListScrollNextState;
static hdPartsStateId_c sListScrollPreviousState;
static hdPartsStateId_c sListSettleState;
static hdPartsStateId_c sListChangePageState;
static hdPartsStateId_c sListDoublePageState;
static hdPartsStateId_c sListPictureSelectedState;
static hdPartsStateId_c sListEraseMessageState;
static hdPartsStateId_c sListReplaceMessageState;
static void* sListPictureSelection;
/* 026C6BF8 */
void hdPictographList::updateDrag() {
    auto* manager = sBoxInput.get();
    auto* input = sCameraUi.get();
    NativeFrame<0x48> frame;
    auto* event = at<hdButtonEvent_l>(frame+0x10);
    pictographBoxCopyEvent(event, reinterpret_cast<hdButtonEvent_l*>(&manager->mEventQueue[0x1C]));
    if ((u32(input->mEvent.mRepeat)&0x8000) && event->mType == 0x44 &&
        event->mReceiverId == reinterpret_cast<hdPictoAnimationReceiverView*>(mTextureAnimations.mpData[0].get())->mReceiverId) {
        auto displacement = pictographListDragDisplacement(manager);
        auto* words = at<be<u32>>(frame+8);
        words[0] = displacement.r3; words[1] = displacement.r4;
        f32 translation = clampPageTranslation(*reinterpret_cast<be<f32>*>(words));
        mTransitionProgress = translation;
        positionPictures(translation);
    } else {
        f32 distance = __builtin_fabsf(f32(mTransitionProgress));
        if (distance<sListSettleThreshold) changeState(&sListSettleState);
        else if (distance<sListDoubleThreshold || mPageCount == 2) changeState(&sListChangePageState);
        else changeState(&sListDoublePageState);
    }
    auto* receiver = mTextureAnimations.mpData[0].get();
    pictographListTrackReceiver(sBoxInput, receiver);
    vCalcLayout();
}
/* 026C68D0 */
void hdPictographList::updateWait() {
    auto* input = sCameraUi.get();
    auto* buttons = &input->mEvent;
    bindVisibleTextures();
    auto* manager = sBoxInput.get();
    NativeFrame<0x48> frame;
    auto* event = at<hdButtonEvent_l>(frame+8);
    pictographBoxCopyEvent(event, reinterpret_cast<hdButtonEvent_l*>(&manager->mEventQueue[0x1C]));
    if ((u32(input->mEvent.mRepeat)&0x8000) && event->mType == 0x44 &&
        event->mReceiverId == reinterpret_cast<hdPictoAnimationReceiverView*>(mTextureAnimations.mpData[0].get())->mReceiverId) {
        changeState(&sListDragState);
        pictographListPlaySound(0x8D6);
    }
    u32 press = u32(buttons->mHold)|u32(buttons->mTrigger);
    bool stick = buttons->mStick.value.x != sListZero || buttons->mStick.value.y != sListZero;
    u32 nextMask = stick?0x400000:0x40000;
    u32 previousMask = stick?0x800000:0x80000;
    if ((press&nextMask) && (mPageCount>2 || (mPageCount == 2 && mPage == 2))) {
        changeState(&sListScrollNextState);
        pictographListPlaySound(0x8D6);
    } else if ((press&previousMask) && (mPageCount>2 || (mPageCount == 2 && mPage == 1))) {
        changeState(&sListScrollPreviousState);
        pictographListPlaySound(0x8D6);
    }
    u32 listMode = isPictureListMode();
    u32 count = mPageCount;
    if (!listMode && count) {
        u32 hold = buttons->mHold;
        if (hold&1) {
            s32 picture = resolvePictureIndex(s32(u8(mPage))-1);
            pictographListSelectPicture(sListPictureSelection, picture);
            changeState(&sListPictureSelectedState);
        } else if (hold&8) {
            changeState(&sListEraseMessageState);
            pictographListPlaySound(0x8D7);
        }
    } else if (listMode && count && (u32(buttons->mHold)&1)) {
        changeState(&sListReplaceMessageState);
        pictographListPlaySound(0x8D7);
    }
    updateButtonIcons();
    if (mPageCount>1 && pictographListInputTracking(sCameraUi)) {
        auto* receiver = mTextureAnimations.mpData[0].get();
        pictographListTrackReceiver(sBoxInput, receiver);
    }
    vCalcLayout();
}
struct hdPictoMenuStatusView {
    u8 mReserved0[0x3F]; be<u8> mSelection, mBusy, mFlags;
    u8 mReserved42[0x5A-0x42]; be<u8> mPictureListActive;
};
WWHD_OFFSET(hdPictoMenuStatusView, mSelection, 0x3F);
WWHD_OFFSET(hdPictoMenuStatusView, mPictureListActive, 0x5A);
struct hdPictoUiChildView { u8 mReserved0[0xDF]; be<u8> mTransitionActive; };
WWHD_OFFSET(hdPictoUiChildView, mTransitionActive, 0xDF);
struct hdPictoUiOwnerView {
    u8 mReserved0[0x10]; hdStateMachine mStateMachine;
    u8 mReserved24[0x188-0x24]; gptr<hdPictoUiChildView> mpPictureUi;
};
WWHD_OFFSET(hdPictoUiOwnerView, mpPictureUi, 0x188);
struct hdPictoPictureControllerView { u8 mReserved0[0x8260]; be<u32> mTransitionFrames; };
WWHD_OFFSET(hdPictoPictureControllerView, mTransitionFrames, 0x8260);
static hdPictoMenuStatusView sListMenuStatus;
static HdPadUiPointer sListUi;
static u8 sListExternalFinished;
static u8 sListExternalAccepted;
static hdPartsStateId_c sListExternalState;
/* 026C6498 */
void hdPictographList::beginInAll() {
    auto* play = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get());
    u8 right = play->mPictureButtonRight, aux = play->mPictureButtonAux, left = play->mPictureButtonLeft;
    play->mSavedPictureButtonAux = aux;
    play->mSavedPictureButtonLeft = left;
    play->mSavedPictureButtonRight = right;
    sListMenuStatus.mBusy = 0;
    sListMenuStatus.mPictureListActive = 1;
    pictographBoxPlayAnimation(animSet(), 0, 0, sListForward);
    auto* archive = pictographCameraPictureArchive(&sCameraSave->mPictureHolder);
    mPageCount = archive->mCount;
    setDenominator(mPageCount);
    mPage = mPageCount;
    setNumerator(mPageCount);
    const void* callback = u8(mPageCount) <= 2?reinterpret_cast<const void*>(pictographListPositionAdjacent):reinterpret_cast<const void*>(pictographListPositionFollowing);
    mPositionDelegate.mVirtualIndex = -1;
    mPositionDelegate.mFunction = guest_fn(callback);
    mPositionDelegate.mThisDelta = 0;
    rebuildVisiblePictures();
    positionPictures(sListZero);
    pictographListPlaySound(0x823);
    u32 mode = isPictureListMode();
    mSelectionChanged = mode;
    if (mode) { u8 flags = sListMenuStatus.mFlags; sListMenuStatus.mSelection = 0xFF; sListMenuStatus.mFlags = flags|3; }
}
/* 026C6638 */
void hdPictographList::beginOutToExternal() {
    pictographBoxPlayAnimation(animSet(), 2, 0, sListForward);
    pictographListLockInput(sCameraUi);
    reinterpret_cast<hdPictoUiOwnerView*>(sListUi.mOwner.get())->mpPictureUi->mTransitionActive = 1;
    auto* play = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get());
    reinterpret_cast<hdPictoPictureControllerView*>(play->mpPictureController.get())->mTransitionFrames = 30;
}
/* 026C6708 */
void hdPictographList::finishOutToExternal() {
    auto* play = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get());
    u8 right = play->mSavedPictureButtonRight, left = play->mSavedPictureButtonLeft;
    play->mPictureButtonRight = right;
    play->mPictureButtonLeft = left;
    play->mPictureButtonAux = play->mSavedPictureButtonAux;
    auto* ui = reinterpret_cast<hdPictoUiOwnerView*>(sListUi.mOwner.get());
    sListMenuStatus.mPictureListActive = 0;
    sListMenuStatus.mSelection = 0;
    ui->mpPictureUi->mTransitionActive = 0;
}
/* 026C7BA0 */
void hdPictographList::updateExternalTransition() {
    if (sListExternalFinished) {
        if (sListExternalAccepted) {
            auto* ui = reinterpret_cast<hdPictoUiOwnerView*>(sListUi.mOwner.get());
            hdParts_changeState(&ui->mStateMachine, &sListExternalState);
            auto* play = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get());
            play->mPlayerFlags = u32(play->mPlayerFlags)|0x200000;
            play = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get());
            play->mPlayerFlags = u32(play->mPlayerFlags)&~0x100000;
        } else changeState(&sListPageReadyState);
    }
    updateButtonIcons();
}
/* WWHD glue
data sListMenuStatus 1047B03C
data sListUi 101F8344
data sListExternalFinished 1018F511
data sListExternalAccepted 1018F510
data sListExternalState 1049F440
data sListSettleThreshold 10101110
data sListDoubleThreshold 10101114
data sListDragState 10499DA4
data sListScrollNextState 10499E04
data sListScrollPreviousState 10499DD4
data sListSettleState 10499E34
data sListChangePageState 10499E64
data sListDoublePageState 10499E94
data sListPictureSelectedState 10499FB4
data sListEraseMessageState 10499EC4
data sListReplaceMessageState 10499F54
data sListPictureSelection 1018F504
ext 026151A8 Pair32 pictographListDragDisplacement(EventMgr*)
ext 026150C0 void pictographListTrackReceiver(EventMgr*,hdPartsAnim_c*)
ext 026184AC u32 pictographListInputTracking(hdPictoInputSystem*)
ext 0203C118 void pictographListSelectPicture(void*,s32)
data sListReplaceOutState 1049DD90
data sListNegativeScrollStep 10101128
ext 026C8BC8 void pictographListPositionAdjacent(hdPictographList*,f32) callback
ext 026C8C90 void pictographListPositionNext(hdPictographList*,f32) callback
ext 026C8CB8 void pictographListPositionPrevious(hdPictographList*,f32) callback
ext 026C8B34 void pictographListPositionFollowing(hdPictographList*,f32) callback
ext 025E1988 void pictographListPlaySound(u32)
data sListEmptyFrame 10101100
ext 02642330 void pictographListShowPicture(hdParts_c*)
data sListNegativeDoublePageWidth 10101120
data sListTriplePageWidth 10101124
data sListDoubleWidthReady 10499CA8
data sListCachedDoubleWidth 10499CAC
data sListScrollStep 10101118
data sListCaptureControl 101F8378
data sListEraseTransitionState 10499F24
ext 026422AC u32 pictographListIsPictureReady(hdParts_c*)
ext 02725EE4 void pictographListEraseArchivePicture(hdPictoPictureArchiveView*,u32)
data sListNumberNamesReady 10499CB0
data sListNameStringTable 10101004
data sListNumberNameRegistration 101F6920
data sListEraseState 10499EF4
str 1010112C "T_Numerator_00" in 026C7D80
str 1010113C "T_Denominator_00" in 026C7D80
data sListNumberNames 10499BF8
ext 026C9610 void pictographListDestroySafeString(hdUiSafeString*) callback
ext 027261AC s32 pictographListArchiveIndex(hdPictoPictureArchiveView*,s32)
ext 02726164 void* pictographListArchivePixels(hdPictoPictureArchiveView*,s32)
ext 02036474 void pictographListDecodePicture(hdPictoInputOptions*,void*,HdJpegTextureMap*,void*,u32)
ext 027260DC u32 pictographListArchivePhoto(hdPictoPictureArchiveView*,s32)
ext 02726120 u32 pictographListArchivePhotoUsed(hdPictoPictureArchiveView*,s32)
ext 02725A00 u32 pictographListPictureIsBestShot(void*)
data sListDoublePageWidth 10101108
data sListSettleFrames 1010111C
data sListPageReadyState 10499D74
ext 0268B070 u32 pictographListChaseTranslation(be<f32>*,f32,f32)
data sListNumberStringTable 10101034
data sListPageWidth 10101104
data sListNegativePageWidth 1010110C
str 101010EC "pictograph list denominator format" renamed in 026C59CC
str 101010F4 "pictograph list numerator format" renamed in 026C5A80
data sListTable 101013B0
data sListHolderTable 10101610
data sListSecondaryTable 10101620
data sListZero 101010FC
data sListForward 101010E8
data sListBlankState 10499CB4
ext 028EFFD0 void pictographListConstructTextures(HdJpegTextureMap*,u32,u32,u32)
ext 026C9648 void pictographListTextureConstruct(HdJpegTextureMap*) callback
ext 026185BC void pictographListLockInput(hdPictoInputSystem*)
ext 026420F0 void hdPictographButton_c::setHorizontalPosition(f32)
ext 02642364 void hdPictographButton_c::hide()
ext 0264237C void hdPictographButton_c::setThumbnail(hdPictoTextureExtent_c*)
ext 026423DC void hdPictographButton_c::restoreThumbnail()
ext 0264243C void hdPictographButton_c::showBestShot()
ext 0264245C void hdPictographButton_c::hideBestShot()
*/

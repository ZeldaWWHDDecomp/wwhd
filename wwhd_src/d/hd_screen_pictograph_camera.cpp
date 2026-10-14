/* HD Picto Box capture screen, shutter and selfie-cover states. */
#include "d/hd_screen_pictograph_camera.h"
#include "d/d_com_inf_game.h"
#include "d/d_mesg_routing.h"
#include "d/hd_msg_window_base.h"
using namespace gabi;
static u8 sCameraHolderTable;
static u8 sCameraSecondaryTable;
static hdPartsVtbl_c sCameraTable;
static f32 sCameraZero;
static f32 sCameraForward;
static f32 sCameraReverse;
static hdPartsStateId_c sCameraBlankState;
static hdPartsStateId_c sCameraWaitCoverState;
static hdPartsStateId_c sCameraScaleState;
static u8 sCameraMessageSelection;
static hdPictoSaveView* sCameraSave;
static f32 sCameraFullCountFrame;
static f32 sCameraUnsignedThreshold;
static f32 sCameraZoomFrameScale;
static hdUiStringTable sCameraTextStringTable;
static hdPartsStateId_c sCameraSelfieCloseState;
static hdPartsStateId_c sCameraSelfieReverseState;
static hdPictoInputSystem* sCameraUi;
static hdPictoInputOptions* sCameraOptions;
static void* sCameraCaptureManager;
static void* sCameraGraphicsLock;
static f32 sCameraCaptureFrame;
static f32 sCameraBindFrame;
static f32 sCameraFinishFrame;
static hdPartsStateId_c sCameraConfirmState;
static hdPartsStateId_c sCameraBestShotState;
static hdPartsStateId_c sCameraBestShotOutState;
static hdPartsStateId_c sCameraFullState;
static f32 sCameraMaxZoomFraction;
static f32 sCameraSoundScale;
struct hdPictoCameraPaneNames { hdUiSafeString mNames[2]; };
static hdPictoCameraPaneNames sCameraPaneNames;
static u32 sCameraPaneNamesReady;
static u8 sCameraPaneStringTable;
static hdPictoRegistration sCameraPaneRegistration;
extern be<u32>& sPartsTypeReady;
extern hdScreenType& sPartsType;
extern be<u32>& sPaneTypeReady;
extern hdScreenType& sPaneType;
void pictographRegisterStatic(hdPictoRegistration*);
void pictographBoxDestroyBase(hdParts_c*, u32);
hdParts_c* pictographBoxConstructBase(hdParts_c*);
void* pictographAllocate(u32);
void pictographFree(void*);
void pictographBoxPlayAnimation(hdAnimSet*, u32, u32, f32);
u32 pictographBoxAnimationFinished(hdAnimSet*, u32);
void pictographBoxStopAnimation(hdAnimSet*, u32, u32);
void pictographCameraDismissMessage(u32);
void pictographCameraSetMessageStatus(WindowCore*, u32);
void pictographCameraSetMessage(WindowCore*, u32, u32);
void pictographCameraShowInquiry(u32);
void pictographCameraZoomSound(u32, u32, u32);
hdPictoCaptureSurface* pictographCameraGetCapture(void*, u32);
void pictographCameraLockGraphics(void*);
void pictographCameraUnlockGraphics(void*);
void pictographCameraCopySurface(HdJpegSurface*, HdJpegSurface*, u32, u32);
void pictographCameraPrepareSurface(HdJpegTexture*);
void pictographCameraTextureFormat(be<u32>*, u32, u32);
u32 pictographCameraPhotoNumber();
u32 pictographCameraConvertPhoto(u32);
u32 pictographCameraPhotoAlreadyTaken();
u32 pictographCameraHasItem(u32);
void pictographCameraCapture();
void pictographCameraCaptureFinished();
u32 pictographCameraMessageState(WindowCore*);
u32 pictographCameraAvailableSlot(hdPictoPictureArchiveView*);
void* pictographCameraCurrentHeap();
void* pictographCameraAllocatePixels(u32, void*, u32);
void pictographCameraCopyPixels(void*, u32, u32, u32, u32, void*, u32, u32, u32, s32, s32, u32, u32, u32);
void pictographCameraMarkSlot(hdPictoPictureArchiveView*, u32);
void* pictographCameraPictureSlot(hdPictoPictureArchiveView*, u32);
void pictographCameraMarkBestShot(void*, u32);
u32 pictographCameraSavePixels(hdPictoInputOptions*, void*, s32, s32, u32, u32);
void pictographCameraFreePixels(void*);
void pictographSetPaneTexture(hdPartsPaneSRT_c*, void*, u32, u32);
void pictographCameraSetChoices(WindowCore*, u32, u32);
void pictographCameraFinishUi(void*);
void pictographCameraSetFrame(hdAnimSet*, u32, u32, f32);
void pictographCameraSetFrameTarget(hdAnimSet*, u32, u32, f32);
void pictographCameraFormatCount(PaneString_l*, const char*, s32);
void pictographCameraFormatScale(PaneString_l*, const char*, u32, u32);
void pictographCameraDestroyString(hdUiSafeString*, u32);
void pictographDestroyStrings(void*, u32, u32, u32, u32, u32);
hdPictoPictureArchiveView* pictographCameraPictureArchive(be<u32>*);
/* 026C138C */
void destroyPictographCameraPaneNames() {
    pictographDestroyStrings(&sCameraPaneNames, 2, 8, guest_fn(reinterpret_cast<const void*>(pictographCameraDestroyString)), 0, 0);
}
/* 026C13B0 */
void hdPictographCamera::destroy(hdPictographCamera* self, u32 flags) {
    if (!self) return;
    self->mStateHolder.mHolderVtable = &sCameraHolderTable;
    self->mSecondaryVtable = &sCameraSecondaryTable;
    pictographBoxDestroyBase(self, 0);
    if (flags&1) pictographFree(self);
}
/* 026C141C */
void hdPictographCamera::resetCapture() {
    pictographBoxStopAnimation(animSet(), 0, 0);
    vCalcLayout();
    WindowCore* core = l_windowCore.get();
    reinterpret_cast<hdPictoCoreView*>(core)->mCaptureMessageActive = 0;
    pictographCameraSetMessageStatus(core, 0);
    reinterpret_cast<MesgRouting::Play*>(dComIfGp_get())->mScopeMesgStatus = 0;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mMessageVariant = 0;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPictureInputMode = 0;
    mSelfieMode = 0;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mItemOpen = 0;
    mShutterVisible = 0;
}
/* 026C14B4 */
void hdPictographCamera::setScaleText(f32 value) {
    if (!mNumberPanes[0]) return;
    NativeFrame<0x30> frame;
    auto* text = at<hdPictoNumberText>(frame+8);
    text->mString.mCapacity = 3;
    text->mString.mpBuffer = text->mBuffer;
    text->mBuffer[0] = 0;
    text->mBuffer[2] = 0;
    text->mString.mpVTable = reinterpret_cast<gfn*>(&sCameraTextStringTable);
    f32 threshold = sCameraUnsignedThreshold;
    u32 number = value<threshold?u32(ftoi(value)):u32(ftoi(fsubs_ppc(value, threshold)))+0x80000000u;
    pictographCameraFormatScale(&text->mString, "pictograph scale format", 0xD7, number);
    auto* stringTable = reinterpret_cast<hdUiStringTable*>(text->mString.mpVTable.get());
    auto* pane = mNumberPanes[0].get();
    auto* paneTable = reinterpret_cast<hdPictoTextPaneTable*>(pane->mpVTable.get());
    call_ptr(stringTable->mAssureTermination.get(), &text->mString);
    call_ptr(paneTable->mSetText.get(), pane, text->mString.mpBuffer.get(), 0);
}
/* 026C15B8 */
void hdPictographCamera::setCount(s32 value) {
    if (!mNumberPanes[1]) return;
    NativeFrame<0x28> frame;
    auto* text = at<hdPictoNumberText>(frame+8);
    text->mString.mpBuffer = text->mBuffer;
    text->mString.mpVTable = reinterpret_cast<gfn*>(&sCameraTextStringTable);
    text->mString.mCapacity = 3;
    text->mBuffer[2] = 0;
    text->mBuffer[0] = 0;
    pictographCameraFormatCount(&text->mString, "pictograph count format", value);
    auto* stringTable = reinterpret_cast<hdUiStringTable*>(text->mString.mpVTable.get());
    auto* pane = mNumberPanes[1].get();
    auto* paneTable = reinterpret_cast<hdPictoTextPaneTable*>(pane->mpVTable.get());
    call_ptr(stringTable->mAssureTermination.get(), &text->mString);
    call_ptr(paneTable->mSetText.get(), pane, text->mString.mpBuffer.get(), 0);
}
/* 026C166C */
void hdPictographCamera::initializeInAll() {
    reinterpret_cast<hdPictoCoreView*>(l_windowCore.get())->mCaptureMessageActive = 1;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mMessageInputStatus = 0;
    reinterpret_cast<MesgRouting::Play*>(dComIfGp_get())->mScopeMesgStatus = 0;
    f32 one = sCameraForward;
    pictographBoxPlayAnimation(animSet(), 0, 0, one);
    f32 zoom = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPictureZoom;
    setScaleText(zoom);
    f32 fraction = fsubs_ppc(zoom, one);
    f32 frame = fmuls_ppc(fraction, sCameraZoomFrameScale);
    pictographCameraSetFrameTarget(animSet(), 1, 1, frame);
    s32 count = u8(pictographCameraPictureArchive(&sCameraSave->mPictureHolder)->mCount);
    count += s16(reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPendingPictureCount);
    setCount(count);
    hdAnimSet* animation = animSet();
    f32 countFrame = count == 12?sCameraFullCountFrame:sCameraZero;
    pictographCameraSetFrame(animation, 3, 3, countFrame);
    animation = animSet();
    mSelfieMode = 0;
    pictographBoxStopAnimation(animation, 4, 4);
}
/* 026C17E8 */
void hdPictographCamera::updateInAll() {
    if (pictographBoxAnimationFinished(animSet(), 0)) {
        reinterpret_cast<MesgRouting::Play*>(dComIfGp_get())->mScopeMesgStatus = 11;
        changeState(&sCameraScaleState);
    }
    vCalcLayout();
}
/* 026C1858 */
void hdPictographCamera::initializeCount() {
    f32 frame = sCameraZero;
    mScale = frame;
    s32 count = u8(pictographCameraPictureArchive(&sCameraSave->mPictureHolder)->mCount);
    count += s16(reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPendingPictureCount);
    setCount(count);
    hdAnimSet* animation = animSet();
    if (count == 12) frame = sCameraFullCountFrame;
    pictographCameraSetFrame(animation, 3, 3, frame);
}
/* 026C1904 */
void hdPictographCamera::updateMessageButtons() {
    auto* archive = pictographCameraPictureArchive(&sCameraSave->mPictureHolder);
    if (u8(archive->mCount)<12)
        reinterpret_cast<WindowPlay*>(dComIfGp_get())->mMessageSoundMode = 0x20;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mSecondaryButtonIcon = 7;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPrimaryButtonIcon = 0x49;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mAuxButtonIcon = 0x4A;
}
/* 026C1974 */
void hdPictographCamera::updateScale() {
    u32 blocked = sCameraOptions->mInputBlocked;
    hdPictoInputSystem* input = sCameraUi;
    bool captureRequested = false;
    if (!blocked) {
        if (u32(input->mEvent.mHold)&0x6001) {
            u32 count = pictographCameraPictureArchive(&sCameraSave->mPictureHolder)->mCount;
            if (count<12) captureRequested = true;
            else blocked = sCameraOptions->mInputBlocked;
        }
        if (!captureRequested && !blocked) {
            u32 hold = input->mEvent.mHold;
            if ((hold&0x6001) && !(hold&0x20)) {
                u32 count = pictographCameraPictureArchive(&sCameraSave->mPictureHolder)->mCount;
                if (count >= 12) {
                    pictographCameraShowInquiry(0x883);
                    changeState(&sCameraFullState);
                    return;
                }
            }
        }
    }
    if (!captureRequested) {
        f32 zoom = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPictureZoom;
        setScaleText(zoom);
        f32 fraction = fsubs_ppc(zoom, sCameraForward);
        hdAnimSet* animation = animSet();
        f32 frame = fmuls_ppc(fraction, sCameraZoomFrameScale);
        pictographCameraSetFrameTarget(animation, 1, 1, frame);
        f32 previous = mScale;
        if (fraction != previous) {
            if (fraction == sCameraZero || fraction == sCameraMaxZoomFraction) {
                pictographCameraShowInquiry(0x809);
            } else {
                f32 delta = fsubs_ppc(fraction, previous);
                f32 amplitude = fmuls_ppc(sCameraSoundScale, std::fabs(delta));
                f32 threshold = sCameraUnsignedThreshold;
                u32 magnitude = amplitude<threshold?u32(ftoi(amplitude)):u32(ftoi(fsubs_ppc(amplitude, threshold)))+0x80000000u;
                pictographCameraZoomSound(8, 0, magnitude);
            }
        }
        mScale = fraction;
    }
    updateMessageButtons();
    vCalcLayout();
}
/* 026C1BA8 */
void hdPictographCamera::initializeInquiry() {
    auto* play = reinterpret_cast<MesgRouting::Play*>(dComIfGp_get());
    f32 rate = sCameraForward;
    play->mScopeMesgStatus = 7;
    pictographBoxPlayAnimation(animSet(), 2, 2, rate);
    pictographCameraShowInquiry(0x878);
    sCameraMessageSelection = 0xFF;
}
/* 026C22E4 */
void hdPictographCamera::initializeMessage() {
    pictographCameraSetMessage(l_windowCore.get(), 0xEDC, 0);
    reinterpret_cast<MesgRouting::Play*>(dComIfGp_get())->mScopeMesgStatus = 6;
    sCameraMessageSelection = 0x19;
}
/* 026C1C0C */
void hdPictographCamera::bindCapturedTexture() {
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    auto* pane = root->find("PF_Pictograph_00");
    auto* capture = pictographCameraGetCapture(sCameraCaptureManager, 9);
    if (!capture) return;
    pictographCameraLockGraphics(sCameraGraphicsLock);
    HdJpegTexture* texture = &mCapturedTexture.mTexture;
    pictographCameraCopySurface(&texture->mSurface, &capture->mSurface, sizeof(HdJpegSurface), 0);
    u32 mipLevels = texture->mSurface.mMipLevels;
    u32 depth = texture->mSurface.mDepth;
    texture->mFirstSlice = 0;
    texture->mSliceCount = depth;
    texture->mComponentSelect = 0x10203;
    texture->mMipCount = mipLevels;
    texture->mFirstMip = 0;
    texture->mSurface.mFormat = 0x41A;
    pictographCameraPrepareSurface(texture);
    Local<be<u32>> format;
    pictographCameraTextureFormat(format, 0x41A, 1);
    s32 width = capture->mSurface.mWidth;
    s32 minimumWidth = u8(capture->mMinimumWidth);
    s32 height = capture->mSurface.mHeight;
    if (minimumWidth>width) width = minimumWidth;
    s32 minimumHeight = u8(capture->mMinimumHeight);
    u32 formatWord = *format;
    mCapturedTexture.mFormat = u8(formatWord);
    if (minimumHeight>height) height = minimumHeight;
    mCapturedTexture.mType = 1;
    mCapturedTexture.mWidth = u16(width);
    mCapturedTexture.mHeight = u16(height);
    pictographCameraUnlockGraphics(sCameraGraphicsLock);
    pictographSetPaneTexture(pane, &mCapturedTexture, 0, 0);
    pane->mFlags = (u8(pane->mFlags)&~1u)+1;
}
/* 026C1D68 */
void hdPictographCamera::updateShutter() {
    if (pictographBoxAnimationFinished(animSet(), 2)) {
        u32 number = pictographCameraConvertPhoto(u8(pictographCameraPhotoNumber()));
        if (u32(number-0x49)<0x86 && !pictographCameraPhotoAlreadyTaken() && pictographCameraHasItem(0x26))
            child(0)->changeState(&sCameraBestShotState);
        changeState(&sCameraConfirmState);
        vCalcLayout();
        return;
    }
    hdPartsLayout_c* layout = mpLayout;
    f32 frame = (*layout->animSet()->mActiveSlots.at(2))->mController->mSpeed;
    if (frame == sCameraCaptureFrame) {
        mShutterVisible = 1;
        pictographCameraCapture();
    } else {
        frame = (*layout->animSet()->mActiveSlots.at(2))->mController->mSpeed;
        if (frame == sCameraBindFrame) bindCapturedTexture();
        else {
            frame = (*layout->animSet()->mActiveSlots.at(2))->mController->mSpeed;
            if (frame == sCameraFinishFrame) pictographCameraCaptureFinished();
        }
    }
    vCalcLayout();
}
/* 026C1F20 */
void dismissPictographInquiry() { pictographCameraDismissMessage(0x878); }
/* 026C1F28 */
void hdPictographCamera::calcLayout() { vCalcLayout(); }
/* 026C1F38 */
void hdPictographCamera::initializeConfirmMessage() {
    pictographCameraSetMessage(l_windowCore.get(), 0xEDD, 0);
    pictographCameraSetChoices(l_windowCore.get(), 0x8D4, 0x8D5);
    sCameraMessageSelection = 0x18;
}
/* 026C1F8C */
void hdPictographCamera::updateConfirmMessage() {
    WindowCore* core = l_windowCore.get();
    if (pictographCameraMessageState(core) == 0x12) {
        if (core->mSelection) {
            u32 slot = pictographCameraAvailableSlot(pictographCameraPictureArchive(&sCameraSave->mPictureHolder));
            if (slot<12) {
                auto* capture = pictographCameraGetCapture(sCameraCaptureManager, 9);
                if (capture) {
                    void* heap = pictographCameraCurrentHeap();
                    void* pixels = pictographCameraAllocatePixels(u32(capture->mSurface.mImageSize)*4, heap, capture->mSurface.mAlignment);
                    s32 width = capture->mSurface.mWidth;
                    s32 minimumWidth = u8(capture->mMinimumWidth);
                    s32 height = capture->mSurface.mHeight;
                    s32 minimumHeight = u8(capture->mMinimumHeight);
                    if (minimumWidth>width) width = minimumWidth;
                    if (height>minimumHeight) minimumHeight = height;
                    void* image = capture->mSurface.mImage;
                    u32 size = capture->mSurface.mImageSize;
                    pictographCameraCopyPixels(pixels, size, 0, 0, 0, image, 0, 2, 0x1D, width, minimumHeight, 1, 0, 0);
                    pictographCameraMarkSlot(pictographCameraPictureArchive(&sCameraSave->mPictureHolder), slot);
                    u32 hasItem = pictographCameraHasItem(0x26) != 0;
                    void* picture = pictographCameraPictureSlot(pictographCameraPictureArchive(&sCameraSave->mPictureHolder), slot);
                    pictographCameraMarkBestShot(picture, hasItem);
                    auto* play = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get());
                    play->mPendingPictureCount = s16(play->mPendingPictureCount)+1;
                    s32 count = u8(pictographCameraPictureArchive(&sCameraSave->mPictureHolder)->mCount);
                    count += s16(reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPendingPictureCount);
                    setCount(count);
                    if (count == 12) pictographCameraSetFrame(animSet(), 3, 3, sCameraForward);
                    width = capture->mSurface.mWidth;
                    minimumWidth = u8(capture->mMinimumWidth);
                    height = capture->mSurface.mHeight;
                    minimumHeight = u8(capture->mMinimumHeight);
                    if (width>minimumWidth) minimumWidth = width;
                    hdPictoInputOptions* options = sCameraOptions;
                    if (height>minimumHeight) minimumHeight = height;
                    if (!pictographCameraSavePixels(options, pixels, minimumWidth, minimumHeight, 0x50, slot))
                        pictographCameraFreePixels(pixels);
                }
            }
        }
        u32 photo = pictographCameraConvertPhoto(u8(pictographCameraPhotoNumber()));
        if (u32(photo-0x49)<0x86 && !pictographCameraPhotoAlreadyTaken() && pictographCameraHasItem(0x26))
            child(0)->changeState(&sCameraBestShotOutState);
        changeState(&sCameraScaleState);
    }
    vCalcLayout();
    child(0)->vExecuteState();
    reinterpret_cast<WindowPlay*>(dComIfGp_get())->mMessageSoundMode = 0x17;
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mSecondaryButtonIcon = 7;
}
/* 026C228C */
void hdPictographCamera::finishInquiry() {
    pictographCameraFinishUi(sCameraUi);
    mShutterVisible = 0;
    reinterpret_cast<MesgRouting::Play*>(dComIfGp_get())->mScopeMesgStatus = 11;
    sCameraMessageSelection = 0;
}
/* 026C232C */
void hdPictographCamera::updateMessage() {
    if (!pictographCameraMessageState(l_windowCore.get())) changeState(&sCameraScaleState);
    f32 zoom = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPictureZoom;
    setScaleText(zoom);
    f32 fraction = fsubs_ppc(zoom, sCameraForward);
    f32 frame = fmuls_ppc(fraction, sCameraZoomFrameScale);
    pictographCameraSetFrameTarget(animSet(), 1, 1, frame);
    f32 previous = mScale;
    if (fraction != previous) {
        if (fraction == sCameraZero || fraction == sCameraMaxZoomFraction) pictographCameraShowInquiry(0x809);
        else {
            f32 delta = fsubs_ppc(fraction, previous);
            f32 amplitude = fmuls_ppc(sCameraSoundScale, std::fabs(delta));
            f32 threshold = sCameraUnsignedThreshold;
            u32 magnitude = amplitude<threshold?u32(ftoi(amplitude)):u32(ftoi(fsubs_ppc(amplitude, threshold)))+0x80000000u;
            pictographCameraZoomSound(8, 0, magnitude);
        }
    }
    mScale = fraction;
    vCalcLayout();
    reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mSecondaryButtonIcon = 7;
}
/* 026C24B8 */
void hdPictographCamera::finishMessage() {
    pictographCameraFinishUi(sCameraUi);
    mShutterVisible = 0;
    reinterpret_cast<MesgRouting::Play*>(dComIfGp_get())->mScopeMesgStatus = 11;
    sCameraMessageSelection = 0;
}
/* 026C2510 */
void hdPictographCamera::openSelfieCover() { pictographBoxPlayAnimation(animSet(), 4, 4, sCameraForward); }
/* 026C252C */
void hdPictographCamera::updateOpenSelfieCover() {
    if (pictographBoxAnimationFinished(animSet(), 4)) changeState(&sCameraWaitCoverState);
    updateMessageButtons();
    vCalcLayout();
}
/* 026C2598 */
void hdPictographCamera::closeSelfieCover() { pictographBoxPlayAnimation(animSet(), 5, 4, sCameraForward); }
/* 026C25B4 */
void hdPictographCamera::updateCloseSelfieCover() {
    if (pictographBoxAnimationFinished(animSet(), 5)) changeState(&sCameraScaleState);
    updateMessageButtons();
    vCalcLayout();
}
/* 026C2620 */
void hdPictographCamera::enableSelfieMode() { mSelfieMode = 1; }
/* 026C262C */
void hdPictographCamera::initializeSelfieToggle() {
    mSelfieDelay = 0;
    u32 wasSelfie = u32(reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPlayerFlags)&0x100000;
    auto* play = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get());
    u32 flags = play->mPlayerFlags;
    play->mPlayerFlags = wasSelfie?(flags&~0x100000u):(flags|0x100000u);
}
/* 026C269C */
void hdPictographCamera::updateSelfieToggle() {
    if (u8(mSelfieDelay) == 5) {
        changeState(mSelfieMode?&sCameraSelfieReverseState:&sCameraSelfieCloseState);
        f32 zoom = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mPictureZoom;
        setScaleText(zoom);
        f32 fraction = fsubs_ppc(zoom, sCameraForward);
        hdAnimSet* animation = animSet();
        f32 frame = fmuls_ppc(fraction, sCameraZoomFrameScale);
        pictographCameraSetFrameTarget(animation, 1, 1, frame);
    }
    updateMessageButtons();
    mSelfieDelay = u8(mSelfieDelay)+1;
}
/* 026C2760 */
void hdPictographCamera::reverseCloseSelfieCover() { pictographBoxPlayAnimation(animSet(), 5, 4, sCameraReverse); }
/* 026C277C */
void hdPictographCamera::updateReverseCloseSelfieCover() {
    if (pictographBoxAnimationFinished(animSet(), 5)) changeState(&sCameraWaitCoverState);
    updateMessageButtons();
    vCalcLayout();
}
/* 026C27E8 */
void hdPictographCamera::reverseOpenSelfieCover() { pictographBoxPlayAnimation(animSet(), 4, 4, sCameraReverse); }
/* 026C2804 */
void hdPictographCamera::updateReverseOpenSelfieCover() {
    if (pictographBoxAnimationFinished(animSet(), 4)) changeState(&sCameraScaleState);
    updateMessageButtons();
    vCalcLayout();
}
/* 026C2870 */
void hdPictographCamera::disableSelfieMode() { mSelfieMode = 0; }
/* 026C287C */
void hdPictographCamera::initializeOutAll() {
    pictographBoxPlayAnimation(animSet(), 6, 0, sCameraForward);
    auto* player = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get())->mpPlayer.get();
    player->mCaptureFlags = u32(player->mCaptureFlags)|0x80000;
    auto* play = reinterpret_cast<hdPictoPlayView*>(dComIfGp_get());
    play->mPlayerFlags = u32(play->mPlayerFlags)&~0x100000u;
}
/* 026C28D8 */
void hdPictographCamera::updateOutAll() {
    if (pictographBoxAnimationFinished(animSet(), 6)) changeState(&sCameraBlankState);
    vCalcLayout();
}
/* 026C293C */
void clearPictographMessageSelection() { sCameraMessageSelection = 0; }
/* 026C2950 */
void hdPictographCamera::resetAnimation() {
    pictographBoxStopAnimation(animSet(), 0, 0);
    vCalcLayout();
}
/* 026C29A0 */
void hdPictographCamera::enterBlank() { changeState(&sCameraBlankState); }
/* 026C29B0 */
hdPictographCamera* hdPictographCamera::construct(hdPictographCamera* self) {
    if (!self) {
        self = static_cast<hdPictographCamera*>(pictographAllocate(sizeof(hdPictographCamera)));
        if (!self) return NULL;
    }
    pictographBoxConstructBase(self);
    self->mVtable = &sCameraTable;
    self->mSecondaryVtable = &sCameraSecondaryTable;
    self->mStateHolder.mHolderVtable = &sCameraHolderTable;
    if (!u32(ea(self)+offsetof(hdPictographCamera, mNumberPanes))) pictographAllocate(8);
    f32 zero = sCameraZero;
    self->mCapturedTexture.mHeight = 0;
    self->mScale = zero;
    self->mpShutter = NULL;
    self->mCapturedTexture.mWidth = 0;
    self->mShutterVisible = 0;
    self->mCapturedTexture.mType = 0;
    return self;
}
/* 026C2A50 */
void hdPictographCamera::bindNumberPanes() {
    if (!sCameraPaneNamesReady) {
        sCameraPaneNamesReady = 1;
        sCameraPaneNames.mNames[0].mVtable = reinterpret_cast<hdUiStringTable*>(&sCameraPaneStringTable);
        sCameraPaneNames.mNames[0].mText.v = guest_addr("T_NumScale_00");
        sCameraPaneNames.mNames[1].mVtable = reinterpret_cast<hdUiStringTable*>(&sCameraPaneStringTable);
        sCameraPaneNames.mNames[1].mText.v = guest_addr("T_Numerator_00");
        pictographRegisterStatic(&sCameraPaneRegistration);
    }
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    for (u32 index = 0; index<2; ++index) {
        auto* name = &sCameraPaneNames.mNames[index];
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
/* WWHD glue
data sCameraPaneNamesReady 104996BC
data sCameraPaneStringTable 10100634
data sCameraPaneRegistration 101F689C
str 1010077C "T_NumScale_00" in 026C2A50
str 1010078C "T_Numerator_00" in 026C2A50
data sCameraBestShotOutState 1048EE88
ext 025F795C u32 pictographCameraMessageState(WindowCore*)
ext 02726218 u32 pictographCameraAvailableSlot(hdPictoPictureArchiveView*)
ext 0203E90C void* pictographCameraCurrentHeap()
ext 0273B0D4 void* pictographCameraAllocatePixels(u32,void*,u32)
ext 027B6A24 void pictographCameraCopyPixels(void*,u32,u32,u32,u32,void*,u32,u32,u32,s32,s32,u32,u32,u32)
ext 027261D0 void pictographCameraMarkSlot(hdPictoPictureArchiveView*,u32)
ext 0272609C void* pictographCameraPictureSlot(hdPictoPictureArchiveView*,u32)
ext 02725A14 void pictographCameraMarkBestShot(void*,u32)
ext 020363E4 u32 pictographCameraSavePixels(hdPictoInputOptions*,void*,s32,s32,u32,u32)
ext 0273AFC8 void pictographCameraFreePixels(void*)
data sCameraCaptureManager 101F9968
data sCameraGraphicsLock 101F8B18
data sCameraCaptureFrame 1010076C
data sCameraBindFrame 10100770
data sCameraFinishFrame 10100774
data sCameraConfirmState 104997B0
data sCameraBestShotState 1048EE28
str 10100758 "PF_Pictograph_00" in 026C1C0C
ext 027F81A4 hdPictoCaptureSurface* pictographCameraGetCapture(void*,u32)
ext 0274FBF8 void pictographCameraLockGraphics(void*)
ext 0274FCCC void pictographCameraUnlockGraphics(void*)
ext C0009988 void pictographCameraCopySurface(HdJpegSurface*,HdJpegSurface*,u32,u32)
ext C0006498 void pictographCameraPrepareSurface(HdJpegTexture*)
ext 0287F1EC void pictographCameraTextureFormat(be<u32>*,u32,u32)
ext 025BE8E8 u32 pictographCameraPhotoNumber()
ext 022A1F1C u32 pictographCameraConvertPhoto(u32)
ext 025BE8F8 u32 pictographCameraPhotoAlreadyTaken()
ext 02520C0C u32 pictographCameraHasItem(u32)
ext 025BDC48 void pictographCameraCapture()
ext 025BE8DC void pictographCameraCaptureFinished()
data sCameraOptions 1018F3F4
data sCameraFullState 104997E0
data sCameraMaxZoomFraction 10100750
data sCameraSoundScale 10100754
ext 025E1A04 void pictographCameraZoomSound(u32,u32,u32)
data sCameraUnsignedThreshold 1010072C
data sCameraZoomFrameScale 10100744
data sCameraTextStringTable 10100664
data sCameraSelfieCloseState 10499840
data sCameraSelfieReverseState 104998D0
str 10100730 "pictograph scale format" renamed in 026C14B4
str 1010073C "pictograph count format" renamed in 026C15B8
ext 02759D3C void pictographCameraFormatCount(PaneString_l*,const char*,s32)
ext 02759D3C void pictographCameraFormatScale(PaneString_l*,const char*,u32,u32)
ext 0200579C void pictographCameraSetFrameTarget(hdAnimSet*,u32,u32,f32)
data sCameraSave 101F84DC
data sCameraFullCountFrame 10100748
data sCameraUi 101F5088
data sCameraPaneNames 1049962C
ext 026C39A4 void pictographCameraDestroyString(hdUiSafeString*,u32) callback
ext 025F85A8 void pictographCameraSetChoices(WindowCore*,u32,u32)
ext 026185BC void pictographCameraFinishUi(void*)
ext 0200552C void pictographCameraSetFrame(hdAnimSet*,u32,u32,f32)
ext 02720144 hdPictoPictureArchiveView* pictographCameraPictureArchive(be<u32>*)
data sCameraHolderTable 10100BE0
data sCameraSecondaryTable 10100BF0
data sCameraTable 101009C8
data sCameraZero 1010074C
data sCameraForward 10100728
data sCameraReverse 10100778
data sCameraBlankState 104996C0
data sCameraWaitCoverState 10499870
data sCameraScaleState 10499720
data sCameraMessageSelection 1047B07B
ext 025F74D0 void pictographCameraSetMessageStatus(WindowCore*,u32)
ext 025F7DB0 void pictographCameraSetMessage(WindowCore*,u32,u32)
ext 025E1988 void pictographCameraShowInquiry(u32)
ext 025E1ACC void pictographCameraDismissMessage(u32)
*/

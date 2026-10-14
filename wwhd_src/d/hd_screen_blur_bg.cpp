/** HD pause-menu blur background: capture, pane texture and fade states. */
#include "d/hd_screen_blur_bg.h"
static hdPartsVtbl_c sBlurVtable;
static u32 sBlurHolderVtable;
static u32 sBlurSecondaryVtable;
static hdUiStringTable sBlurStringVtable;
static hdPartsStateIdVtbl_c sBlurStateVtable;
static hdStateId sBlurNullState;
static hdStateId sBlurPartsBlankState;
static hdPartsStateId_c sBlurPartsDisplayState;
static u8 sBlurRequested;
static hdPartsStateId_c sBlurWait;
static hdPartsStateId_c sBlurCapture;
static hdPartsStateId_c sBlurIn;
static hdPartsStateId_c sBlurOut;
static hdUiSafeString sBlurArchiveName;
static hdUiSafeString sBlurLayoutName;
static hdUiSafeString sBlurAnimationNames[2] = {};
static hdUiSafeString sBlurGroupName;
static u32 sBlurAnimationSlots[2] = {};
static f32 sBlurOne;
struct hdBlurController_l { u8 mPrefix[0x1D0]; be<u32> mMode; };
struct hdBlurControllerSlot_l { gptr<hdBlurController_l> mpController; };
struct hdBlurCaptureResource_l { be<u32> mHeader; hdBlurSurface_l mSurface; be<u8> mWidthLimit, mHeightLimit; };
WWHD_OFFSET(hdBlurCaptureResource_l, mWidthLimit, 0x78);
struct hdBlurCaptureSlot_l { gptr<void> mpManager; };
struct hdBlurUiSlot_l { gptr<hdScreenUiSystem> mpSystem; };
static hdBlurControllerSlot_l sBlurController;
static hdBlurCaptureSlot_l sBlurCaptureManager;
static hdBlurCaptureSlot_l sBlurGraphicsManager;
static hdBlurUiSlot_l sBlurUi;
hdBlurCaptureResource_l* blurGetCapture(void*, u32);
void blurLockGraphics(void*);
void blurUnlockGraphics(void*);
void blurCopySurface(void*, const void*, u32, u32);
void blurInitializeTextureRegs(hdBlurSurface_l*);
void blurMakeFormat(void*, u32, u32);
void blurSetPaneTexture(hdPartsPane_c*, hdBlurTexture_l*, u32, u32);
void blurBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
void blurSetAnimationFrame(hdAnimSet*, u32, u32, f32);
void blurRestartAnimation(hdAnimSet*, u32, u32, f32);
u32 blurAnimationEnded(hdAnimSet*, u32);
u32 blurAnimationStopped(hdAnimSet*, u32);
void blurStopAnimation(hdAnimSet*, u32, u32);
/* 026253FC */
static u32 blurStateNumber(hdPartsStateId_c* state) { return state->mNumber; }
/* 026254FC */
void hdBlurBackground_c::destroy(hdBlurBackground_c* screen, s32 flags) {
    if (!screen) return;
    screen->mStateHolder.mHolderVtable = &sBlurHolderVtable;
    screen->mSecondaryVtable = &sBlurSecondaryVtable;
    hdParts_c::destroy(screen, 0);
    if (flags&1) operator_delete(screen);
}
/* 02625568 */
void hdBlurBackground_c::waitExecute() { if (sBlurRequested) changeState(&sBlurCapture); }
/* 02625588 */
void hdBlurBackground_c::captureEnter() {
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    auto* table = hdPartsView<hdScreenResourceTable>(root->mVtable.v);
    hdPartsPane_c* pane = gabi::call_ptr<hdPartsPane_c*>(table->mFindPane.get(), root, "PF_BG_00", 1);
    hdBlurCaptureResource_l* capture = nullptr;
    const u32 mode = sBlurController.mpController->mMode;
    if (mode == 1 || mode == 3) capture = blurGetCapture(sBlurCaptureManager.mpManager, 8);
    else if (mode<3) capture = blurGetCapture(sBlurCaptureManager.mpManager, 7);
    if (u32(mCaptureKind) != 19) { capture = blurGetCapture(sBlurCaptureManager.mpManager, mCaptureKind); mCaptureKind = 19; }
    if (capture) {
        blurLockGraphics(sBlurGraphicsManager.mpManager);
        blurCopySurface(&mTexture.mSurface, &capture->mSurface, 0x74, 0);
        mTexture.mFirstMip = 0;
        mTexture.mFirstSlice = 0;
        const u32 height = mTexture.mSurface.mHeight;
        const u32 width = mTexture.mSurface.mWidth;
        mTexture.mComponentSelect = 0x10203;
        mTexture.mMipHeight = height;
        mTexture.mSliceWidth = width;
        blurInitializeTextureRegs(&mTexture.mSurface);
        gabi::NativeFrame<0x30> frame;
        auto* format = hdPartsView<be<u32>>(frame+8);
        blurMakeFormat(format, 0x1A, 1);
        u32 widthLimit = capture->mWidthLimit;
        s32 widthSource = s32(capture->mSurface.mFormat);
        u32 heightLimit = capture->mHeightLimit;
        if (s32(widthLimit)>widthSource) widthSource = s32(widthLimit);
        s32 heightSource = s32(capture->mSurface.mBufferMode);
        mTexture.mFormat = u8(u32(*format));
        if (s32(heightLimit)>heightSource) heightSource = s32(heightLimit);
        mTexture.mInitialized = 1;
        mTexture.mWidth = u16(widthSource);
        mTexture.mHeight = u16(heightSource);
        blurUnlockGraphics(sBlurGraphicsManager.mpManager);
        blurSetPaneTexture(pane, &mTexture, 0, 0);
    }
    changeState(&sBlurIn);
}
/* 02625748 */
void hdBlurBackground_c::inEnter() {
    const f32 frame = sBlurOne;
    blurSetAnimationFrame(mpLayout->animSet(), 0, 0, frame);
    blurRestartAnimation(mpLayout->animSet(), 0, 0, frame);
}
/* 026257C0 */
void hdBlurBackground_c::inExecute() { if (blurAnimationEnded(mpLayout->animSet(), 0)) changeState(&sBlurPartsDisplayState); vCalcLayout(); }
/* 02625824 */
void hdBlurBackground_c::outEnter() { const f32 frame = sBlurOne; blurSetAnimationFrame(mpLayout->animSet(), 1, 0, frame); }
/* 02625840 */
void hdBlurBackground_c::outExecute() { if (blurAnimationStopped(mpLayout->animSet(), 1)) changeState(&sBlurWait); vCalcLayout(); }
/* 026258A4 */
hdBlurBackground_c* hdBlurBackground_c::construct(hdBlurBackground_c* screen) {
    if (!screen) { screen = static_cast<hdBlurBackground_c*>(operator_new(sizeof(hdBlurBackground_c))); if (!screen) return nullptr; }
    hdParts_c::construct(screen);
    screen->mVtable = &sBlurVtable;
    screen->mStateHolder.mHolderVtable = &sBlurHolderVtable;
    screen->mSecondaryVtable = &sBlurSecondaryVtable;
    screen->mTexture.mInitialized = 0;
    screen->mCaptureKind = 19;
    screen->mTexture.mHeight = 0;
    screen->mTexture.mWidth = 0;
    return screen;
}
/* 02625928 */
u32 hdBlurBackground_c::load(u32 resource, u32 options, u32 descriptor) {
    attachArchives(&sBlurArchiveName, 1);
    if (!vLoad(&sBlurLayoutName, gabi::ea(mpResAccessor.get()), resource, options, hdPartsView<void>(descriptor))) return 0;
    blurStopAnimation(mpLayout->animSet(), 0, 0);
    vCalcLayout();
    startState(&sBlurWait);
    return 1;
}
/* 02625A38 */
void hdBlurBackground_c::unload() { vDestroyLayout(); detachArchives(); }
/* 02625A78 */
void hdBlurBackground_c::execute() { hdParts_executeStateMachine(stateMachine()); }
/* 02625A80 */
void hdBlurBackground_c::requestOut() {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(stateMachine());
    auto* table = current->mVtable.get();
    auto* expectedTable = sBlurWait.mVtable.get();
    u32 currentKey = gabi::call_ptr<u32>(table->mGetKey.get(), current);
    u32 expectedKey = gabi::call_ptr<u32>(expectedTable->mGetKey.get(), &sBlurWait);
    if (currentKey != expectedKey) changeState(&sBlurOut);
}
/* 02625B18 */
u32 hdBlurBackground_c::loadLayout(void* name, void* accessor, u32 resource, u32 options, void* descriptor) {
    auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sBlurUi.mpSystem->mHeap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, gabi::ea(accessor), resource, options, 2, 1, descriptor)) return 0;
    for (u32 i = 0; i<2; ++i) {
        auto* group = hdPartsView<hdUiSafeString>(gabi::ea(&sBlurGroupName)+sBlurAnimationSlots[i]*8);
        blurBindAnimation(mpLayout->animSet(), i, &sBlurAnimationNames[i], group);
    }
    vCalcLayout();
    return 1;
}
static u32 sBlurInitializerWords[4] = {};
static u32 sBlurRegistration[3][3] = {};
static f32 sBlurHeaderValues[6] = {};
static f32 sBlurScaleSource;
static f32 sBlurRateSource;
static f32 sBlurZeroSource;
static f32 sBlurLimitSource;
static u8 sBlurHeaderByte;
static u8 sBlurHeaderBool;
static u32 sBlurStateGuard;
static u32 sBlurStateCounter;
void blurRegisterStatic(void*);
void blurConstructByte(void*);
void blurConstructBool(void*);
/* 02625C44 */
static void initializeBlurBackgroundStatics() {
    sBlurInitializerWords[3] = 0; sBlurInitializerWords[2] = 0;
    sBlurInitializerWords[1] = 0; sBlurInitializerWords[0] = 0;
    blurRegisterStatic(sBlurRegistration[0]);
    sBlurHeaderValues[0] = sBlurScaleSource;
    sBlurHeaderValues[1] = sBlurRateSource;
    blurConstructByte(&sBlurHeaderByte);
    blurRegisterStatic(sBlurRegistration[1]);
    blurConstructBool(&sBlurHeaderBool);
    blurRegisterStatic(sBlurRegistration[2]);
    sBlurHeaderValues[4] = sBlurLimitSource; sBlurHeaderValues[5] = sBlurLimitSource;
    sBlurHeaderValues[3] = sBlurZeroSource; sBlurHeaderValues[2] = sBlurZeroSource;
    sBlurGroupName.mVtable = &sBlurStringVtable; sBlurGroupName.mText.v = gabi::guest_addr("G_All_00");
    sBlurArchiveName.mVtable = &sBlurStringVtable; sBlurArchiveName.mText.v = gabi::guest_addr("BlurBG_00");
    sBlurLayoutName.mVtable = &sBlurStringVtable; sBlurLayoutName.mText.v = gabi::guest_addr("BlurBG_00");
    sBlurAnimationNames[0].mVtable = &sBlurStringVtable; sBlurAnimationNames[0].mText.v = gabi::guest_addr("InAll");
    sBlurAnimationNames[1].mVtable = &sBlurStringVtable; sBlurAnimationNames[1].mText.v = gabi::guest_addr("OutAll");
    u32 number = 0;
    if (sBlurStateGuard) number = sBlurStateCounter; else sBlurStateGuard = 1;
    auto* table = &sBlurStateVtable;
    auto* nullState = &sBlurNullState;
    sBlurWait.setup(++number, "::StateID_Blank", table, 4, &sBlurPartsBlankState);
    sBlurCapture.setup(++number, "::StateID_SetTexture", table, 25, nullState);
    sBlurIn.setup(++number, "::StateID_InAll", table, 28, nullState);
    sBlurOut.setup(++number, "::StateID_OutAll", table, 31, nullState);
    sBlurStateCounter = number;
}
/* WWHD glue
 data sBlurVtable 100E52C0 nocheck
 data sBlurHolderVtable 100E53D0 nocheck
 data sBlurSecondaryVtable 100E53E0 nocheck
 data sBlurStringVtable 100E517C nocheck
 data sBlurStateVtable 100E51DC nocheck
 data sBlurNullState 101FF32C nocheck
 data sBlurPartsBlankState 1049DD60 nocheck
 data sBlurPartsDisplayState 1049DD90 nocheck
 data sBlurInitializerWords 1048EF1C nocheck
 data sBlurRegistration 101F5314 nocheck
 data sBlurHeaderValues 1048EF00 nocheck
 data sBlurScaleSource 100E5238 nocheck
 data sBlurRateSource 100E523C nocheck
 data sBlurZeroSource 100E5240 nocheck
 data sBlurLimitSource 100E5244 nocheck
 data sBlurHeaderByte 1048EF18 nocheck
 data sBlurHeaderBool 1048EF19 nocheck
 data sBlurStateGuard 101FD8E4 nocheck
 data sBlurStateCounter 101FDD50 nocheck
 ext 028F026C void blurRegisterStatic(void*)
 ext 028ED6F8 void blurConstructByte(void*)
 ext 028EAB2C void blurConstructBool(void*)
 str 100E5284 "G_All_00"
 str 100E5278 "BlurBG_00"
 str 100E5248 "InAll"
 str 100E5250 "OutAll"
 str 100E5258 "::StateID_Blank"
 str 100E5290 "::StateID_SetTexture"
 str 100E5268 "::StateID_InAll"
 str 100E52A8 "::StateID_OutAll"
 data sBlurRequested 101F8351 nocheck
 data sBlurWait 1048EF3C nocheck
 data sBlurCapture 1048EF6C nocheck
 data sBlurIn 1048EF9C nocheck
 data sBlurOut 1048EFCC nocheck
 data sBlurArchiveName 1048EEF8 nocheck
 data sBlurLayoutName 1048EFFC nocheck
 data sBlurAnimationNames 1048EF2C nocheck
 data sBlurGroupName 1048EEF0 nocheck
 data sBlurAnimationSlots 100E5160 nocheck
 data sBlurOne 100E5228 nocheck
 data sBlurController 101F5088 nocheck
 data sBlurCaptureManager 101F9968 nocheck
 data sBlurGraphicsManager 101F8B18 nocheck
 data sBlurUi 1018C404 nocheck
 str 100E522C "PF_BG_00"
 ext 026F88F0 void hdParts_c::destroy(hdParts_c*,s32)
 ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c*)
 ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString*,u32)
 ext 026F91F8 void hdParts_c::detachArchives()
 ext 027F81A4 hdBlurCaptureResource_l* blurGetCapture(void*,u32)
 ext 0274FBF8 void blurLockGraphics(void*)
 ext 0274FCCC void blurUnlockGraphics(void*)
 ext C0009988 void blurCopySurface(void*,const void*,u32,u32)
 ext C0006498 void blurInitializeTextureRegs(hdBlurSurface_l*)
 ext 0287F1EC void blurMakeFormat(void*,u32,u32)
 ext 02704F3C void blurSetPaneTexture(hdPartsPane_c*,hdBlurTexture_l*,u32,u32)
 ext 02004E04 void blurBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
 ext 020053E4 void blurSetAnimationFrame(hdAnimSet*,u32,u32,f32)
 ext 0200579C void blurRestartAnimation(hdAnimSet*,u32,u32,f32)
 ext 02005840 u32 blurAnimationEnded(hdAnimSet*,u32)
 ext 0200588C u32 blurAnimationStopped(hdAnimSet*,u32)
 ext 02005674 void blurStopAnimation(hdAnimSet*,u32,u32)
*/

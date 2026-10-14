/* HD Miiverse menu chart factories; retain original heap and lifetime calls. */
#include "d/hd_screen_menu_miiverse.h"
#include "d/hd_save_info_holder.h"
#include "d/hd_jpeg.h"
#include "d/d_picture_box.h"
#include "d/d_operate_wind.h"
#include "d/hd_input_eventmgr.h"
#include "d/hd_parts_text_pane.h"
#include "hd/hd_ui_02001084.h"
static hdUiSafeString sMiiverseMessageNames[3] = {};
static hdPartsStateId_c sMiiverseBoxDisplayState;
static hdPartsStateId_c sMiiverseBoxInState;
static hdPartsStateId_c sMiiverseBoxOutState;
static hdPartsStateId_c sMiiverseBoxSaveState;
static hdPartsStateId_c sMiiverseBoxWaitState;
static hdPartsStateId_c sMiiverseCheckInputState;
static hdPartsStateId_c sMiiverseCommentCheckInputState;
static hdPartsStateId_c sMiiverseCommentInState;
static hdPartsStateId_c sMiiverseCommentPhotoState;
static hdPartsStateId_c sMiiverseCommentSlideDownState;
static hdPartsStateId_c sMiiverseCommentWaitState;
static hdPartsStateIdVtbl_c sMiiverseDirectStateVtable;
static hdPartsStateId_c sMiiverseDragState;
static u32 sMiiverseHolderVtable;
static hdPartsStateId_c sMiiverseInAllState;
static hdPartsStateId_c sMiiverseNewCommentCheckInputState;
static hdPartsStateId_c sMiiverseNewCommentInOutState;
static hdPartsStateId_c sMiiverseNewCommentWaitState;
static hdStateId sMiiverseNullState;
static hdPartsStateId_c sMiiverseOutAllState;
static hdPartsStateId_c sMiiversePartsBlankState;
static hdUiSafeString sMiiversePatternName;
static hdPartsStateId_c sMiiversePhotoViewState;
static hdPartsTypeInfo_c sMiiverseRootType;
static u32 sMiiverseRootTypeReady;
static u32 sMiiverseSecondaryVtable;
static hdPartsStateId_c sMiiverseSettingCheckInputState;
static hdPartsStateId_c sMiiverseSettingInState;
static hdPartsStateId_c sMiiverseSettingSlideDownState;
static hdPartsStateId_c sMiiverseSettingWaitState;
static hdPartsStateIdVtbl_c sMiiverseStateVtable;
static hdUiStringTable sMiiverseStringVtable;
static hdUiSafeString sMiiverseTextName;
static hdPartsVtbl_c sMiiverseVtable;
static hdUiSafeString sMiiverseWindowNames;
struct hdMiiverseUiSystemSlot_c { gptr<hdScreenUiSystem> mpSystem; };
WWHD_SIZE(hdMiiverseUiSystemSlot_c, 4);
static hdMiiverseUiSystemSlot_c sMiiverseUiSystem;

/* 0269EFA4 */
static u32 miiverseStateNumber(hdPartsStateId_c* state) {
    return state->mNumber;
}

/* 0269F0A4 */
u32 hdMiiverseChartFactory_c::createTincleTower(gptr<hdParts_c>* destination, void* resource, u32 options) {
    hdParts_c* screen = (hdParts_c*)hdParts_new(0x3BC, sMiiverseUiSystem.mpSystem->mHeap, 4);
    if (screen) screen = miiverseConstructTincleTower(screen);
    *destination = screen;
    return miiverseInitializeTincleTower(screen, resource, options, 0);
}

/* 0269F644 */
void hdMiiverseChartFactory_c::destroyTincleTower(hdParts_c* screen) {
    if (!screen) return;
    screen->vFinalize();
    screen->vDelete(3);
}

/* 0269F11C */
u32 hdMiiverseChartFactory_c::createFairy(gptr<hdParts_c>* destination, void* resource, u32 options) {
    hdParts_c* screen = (hdParts_c*)hdParts_new(0x3BC, sMiiverseUiSystem.mpSystem->mHeap, 4);
    if (screen) screen = miiverseConstructFairy(screen);
    *destination = screen;
    return miiverseInitializeFairy(screen, resource, options, 0);
}

/* 0269F69C */
void hdMiiverseChartFactory_c::destroyFairy(hdParts_c* screen) {
    if (!screen) return;
    screen->vFinalize();
    screen->vDelete(3);
}

/* 0269F194 */
u32 hdMiiverseChartFactory_c::createHeartIsland(gptr<hdParts_c>* destination, void* resource, u32 options) {
    hdParts_c* screen = (hdParts_c*)hdParts_new(0x3BC, sMiiverseUiSystem.mpSystem->mHeap, 4);
    if (screen) screen = miiverseConstructHeartIsland(screen);
    *destination = screen;
    return miiverseInitializeHeartIsland(screen, resource, options, 0);
}

/* 0269F6F4 */
void hdMiiverseChartFactory_c::destroyHeartIsland(hdParts_c* screen) {
    if (!screen) return;
    screen->vFinalize();
    screen->vDelete(3);
}

/* 0269F20C */
u32 hdMiiverseChartFactory_c::createSquid(gptr<hdParts_c>* destination, void* resource, u32 options) {
    hdParts_c* screen = (hdParts_c*)hdParts_new(0x3BC, sMiiverseUiSystem.mpSystem->mHeap, 4);
    if (screen) screen = miiverseConstructSquid(screen);
    *destination = screen;
    return miiverseInitializeSquid(screen, resource, options, 0);
}

/* 0269F74C */
void hdMiiverseChartFactory_c::destroySquid(hdParts_c* screen) {
    if (!screen) return;
    screen->vFinalize();
    screen->vDelete(3);
}

/* 0269F284 */
u32 hdMiiverseChartFactory_c::createTriforce(gptr<hdParts_c>* destination, void* resource, u32 options) {
    hdParts_c* screen = (hdParts_c*)hdParts_new(0x3DC, sMiiverseUiSystem.mpSystem->mHeap, 4);
    if (screen) screen = miiverseConstructTriforce(screen);
    *destination = screen;
    return miiverseInitializeTriforce(screen, resource, options, 0);
}

/* 0269F7A4 */
void hdMiiverseChartFactory_c::destroyTriforce(hdParts_c* screen) {
    if (!screen) return;
    screen->vFinalize();
    screen->vDelete(3);
}

/* 0269F2FC */
u32 hdMiiverseChartFactory_c::createTerry(gptr<hdParts_c>* destination, void* resource, u32 options) {
    hdParts_c* screen = (hdParts_c*)hdParts_new(0x3BC, sMiiverseUiSystem.mpSystem->mHeap, 4);
    if (screen) screen = miiverseConstructTerry(screen);
    *destination = screen;
    return miiverseInitializeTerry(screen, resource, options, 0);
}

/* 0269F7FC */
void hdMiiverseChartFactory_c::destroyTerry(hdParts_c* screen) {
    if (!screen) return;
    screen->vFinalize();
    screen->vDelete(3);
}

/* 0269F374 */
u32 hdMiiverseChartFactory_c::createSecretEntrance(gptr<hdParts_c>* destination, void* resource, u32 options) {
    hdParts_c* screen = (hdParts_c*)hdParts_new(0x3BC, sMiiverseUiSystem.mpSystem->mHeap, 4);
    if (screen) screen = miiverseConstructSecretEntrance(screen);
    *destination = screen;
    return miiverseInitializeSecretEntrance(screen, resource, options, 0);
}

/* 0269F854 */
void hdMiiverseChartFactory_c::destroySecretEntrance(hdParts_c* screen) {
    if (!screen) return;
    screen->vFinalize();
    screen->vDelete(3);
}

/* 0269F3EC */
u32 hdMiiverseChartFactory_c::createSubmarine(gptr<hdParts_c>* destination, void* resource, u32 options) {
    hdParts_c* screen = (hdParts_c*)hdParts_new(0x3BC, sMiiverseUiSystem.mpSystem->mHeap, 4);
    if (screen) screen = miiverseConstructSubmarine(screen);
    *destination = screen;
    return miiverseInitializeSubmarine(screen, resource, options, 0);
}

/* 0269F8AC */
void hdMiiverseChartFactory_c::destroySubmarine(hdParts_c* screen) {
    if (!screen) return;
    screen->vFinalize();
    screen->vDelete(3);
}

/* 0269F464 */
u32 hdMiiverseChartFactory_c::createEnemyTower(gptr<hdParts_c>* destination, void* resource, u32 options) {
    hdParts_c* screen = (hdParts_c*)hdParts_new(0x3BC, sMiiverseUiSystem.mpSystem->mHeap, 4);
    if (screen) screen = miiverseConstructEnemyTower(screen);
    *destination = screen;
    return miiverseInitializeEnemyTower(screen, resource, options, 0);
}

/* 0269F904 */
void hdMiiverseChartFactory_c::destroyEnemyTower(hdParts_c* screen) {
    if (!screen) return;
    screen->vFinalize();
    screen->vDelete(3);
}

/* 0269F4DC */
u32 hdMiiverseChartFactory_c::createMoon(gptr<hdParts_c>* destination, void* resource, u32 options) {
    hdParts_c* screen = (hdParts_c*)hdParts_new(0x3BC, sMiiverseUiSystem.mpSystem->mHeap, 4);
    if (screen) screen = miiverseConstructMoon(screen);
    *destination = screen;
    return miiverseInitializeMoon(screen, resource, options, 0);
}

/* 0269F95C */
void hdMiiverseChartFactory_c::destroyMoon(hdParts_c* screen) {
    if (!screen) return;
    screen->vFinalize();
    screen->vDelete(3);
}

/* 0269F554 */
u32 hdMiiverseChartFactory_c::createHeartSea(gptr<hdParts_c>* destination, void* resource, u32 options) {
    hdParts_c* screen = (hdParts_c*)hdParts_new(0x3BC, sMiiverseUiSystem.mpSystem->mHeap, 4);
    if (screen) screen = miiverseConstructHeartSea(screen);
    *destination = screen;
    return miiverseInitializeHeartSea(screen, resource, options, 0);
}

/* 0269F9B4 */
void hdMiiverseChartFactory_c::destroyHeartSea(hdParts_c* screen) {
    if (!screen) return;
    screen->vFinalize();
    screen->vDelete(3);
}

/* 0269F5CC */
u32 hdMiiverseChartFactory_c::createGhostShip(gptr<hdParts_c>* destination, void* resource, u32 options) {
    hdParts_c* screen = (hdParts_c*)hdParts_new(0x3E0, sMiiverseUiSystem.mpSystem->mHeap, 4);
    if (screen) screen = miiverseConstructGhostShip(screen);
    *destination = screen;
    return miiverseInitializeGhostShip(screen, resource, options, 0);
}

/* 0269FA0C */
void hdMiiverseChartFactory_c::destroyGhostShip(hdParts_c* screen) {
    if (!screen) return;
    screen->vFinalize();
    screen->vDelete(3);
}


static hdUiSafeString sMiiverseRootNames[2] = {{0, 0}};
static u32 sMiiverseRootNamesReady = 0;
static u32 sMiiverseRootRegistration[3] = {0, 0, 0};

/* 0269FA64 */
static void destroyMiiverseRootNames() {
    miiverseDestroyStringNames(sMiiverseRootNames, 2, 8, 0x026A67E8, 0, 0);
}

/* 0269FA88 */
static void destroyMiiverseTextName() {
    miiverseDestroyStringNames(&sMiiverseTextName, 1, 8, 0x026A67E8, 0, 0);
}

/* 0269FAAC */
static void destroyMiiversePatternName() {
    miiverseDestroyStringNames(&sMiiversePatternName, 1, 8, 0x026A67E8, 0, 0);
}

/* 0269FAD0 */
static void destroyMiiverseWindowNames() {
    miiverseDestroyStringNames(&sMiiverseWindowNames, 3, 8, 0x026A67E8, 0, 0);
}

/* 026A3C88 */
void hdMenuMiiverse_c::findRootPanes() {
    if (!sMiiverseRootNamesReady) {
        sMiiverseRootNamesReady = 1;
        sMiiverseRootNames[0].mVtable = &sMiiverseStringVtable;
        sMiiverseRootNames[0].mText.v = gabi::guest_addr("N_All_00");
        sMiiverseRootNames[1].mVtable = &sMiiverseStringVtable;
        sMiiverseRootNames[1].mText.v = gabi::guest_addr("N_Msg_00");
        miiverseRegisterStringNames(sMiiverseRootRegistration);
    }
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    for (u32 index = 0; index < 2; ++index) {
        mpRootPanes[index] = miiverseFindNamedPane(root, &sMiiverseRootNames[index]);
    }
}


static f32 sMiiverseUpperBound = 250.0f;
static f32 sMiiverseLowerBound = -260.0f;
void miiverseDelete(void*);

/* 0269FAF4 */
void hdMenuMiiverse_c::destroy(hdMenuMiiverse_c* screen, u32 flags) {
    if (!screen) return;
    screen->mStateHolder.mHolderVtable = &sMiiverseHolderVtable;
    screen->mSecondaryVtable = &sMiiverseSecondaryVtable;
    hdParts_c::destroy(screen, 0);
    if (flags & 1) miiverseDelete(screen);
}

/* 0269FF34 */
u32 hdMenuMiiverse_c::chartControlId(u32 chart, u32 child) {
    hdParts_c* button = mpChartButtons[chart < 10 ? chart : 0];
    hdParts_c* control = *button->mChildren.safeSlot(child);
    return static_cast<hdMiiverseControl_l*>(control)->mControlId;
}

/* 0269FF70 */
u32 hdMenuMiiverse_c::previousControlId() {
    hdParts_c* control = *mpPreviousButton->mChildren.slot(0);
    return static_cast<hdMiiverseControl_l*>(control)->mControlId;
}

/* 0269FF8C */
u32 hdMenuMiiverse_c::nextControlId() {
    hdParts_c* control = *mpNextButton->mChildren.slot(0);
    return static_cast<hdMiiverseControl_l*>(control)->mControlId;
}

/* 026A170C */
f32 hdMenuMiiverse_c::messageUpperBound() const { return sMiiverseUpperBound; }

/* 026A1718 */
f32 hdMenuMiiverse_c::messageLowerBound() const { return sMiiverseLowerBound; }


void miiverseStopChartButton(hdMiiverseChartButton_l*);

/* 0269FB60 */
void hdMenuMiiverse_c::clearSelectionFlags() {
    for (u32 index = 0; index < 10; ++index) mpChartButtons[index]->mSelectionPending = 0;
    static_cast<hdMiiversePager_l*>(mpPreviousButton.get())->mSelectionPending = 0;
    static_cast<hdMiiversePager_l*>(mpNextButton.get())->mSelectionPending = 0;
}

/* 0269FBBC */
void hdMenuMiiverse_c::resetConfirmation() {
    mConfirmationStep = 0;
    mTouchActive = 0;
    clearSelectionFlags();
    mClosingRequested = 0;
    mDialogCancelled = 0;
}

/* 0269FFA8 */
u32 hdMenuMiiverse_c::selectPendingControl() {
    for (u32 index = 0; index < 10; ++index) {
        if (mpChartButtons[index]->mSelectionPending) {
            mInputMode = chartControlId(index, 0);
            return 1;
        }
    }
    if (static_cast<hdMiiversePager_l*>(mpPreviousButton.get())->mSelectionPending) {
        mInputMode = previousControlId();
        return 1;
    }
    if (static_cast<hdMiiversePager_l*>(mpNextButton.get())->mSelectionPending) {
        mInputMode = nextControlId();
        return 1;
    }
    return 0;
}

/* 026A1D58 */
void hdMenuMiiverse_c::finishDrag() { mDragActive = 0; }

/* 026A207C */
void hdMenuMiiverse_c::resetButtonsForClose() {
    clearSelectionFlags();
    miiverseStopChartButton(mpPrimaryButton);
    for (u32 index = 0; index < 10; ++index) miiverseStopChartButton(mpChartButtons[index]);
}

/* 026A20F4 */
void hdMenuMiiverse_c::finishButtonClose() {
    resetButtonsForClose();
    mTouchActive = 0;
}

/* 026A2624 */
f32 hdMenuMiiverse_c::setScrollFrame(s32 frame) {
    hdAnimSet_setFrame(mpLayout->animSet(), 0, 0, f32(frame));
    vCalcLayout();
    return static_cast<hdMiiverseLayoutPane_l*>(mpRootPanes[0].get())->mTranslationY;
}

void miiversePrepareChartAnimation(hdMiiverseChartButton_l*);
void miiversePreparePreviousAnimation(hdParts_c*);
void miiversePrepareNextAnimation(hdParts_c*);

/* 026A5740 */
void hdMenuMiiverse_c::drawMessage(void* info) {
    mpMessageScreen->vDraw(info);
}

u32 miiverseAnimationFrameCount(hdAnimSetController*);
void miiverseSetChartPresentation(hdMiiverseChartButton_l*, u32, f32);
void miiverseSetPreviousPresentation(hdParts_c*, u32, f32);
void miiverseSetNextPresentation(hdParts_c*, u32, f32);

/* 026A2934 */
void hdMenuMiiverse_c::finishScrollAnimation() {
    hdAnimSetEntry* animation = mpLayout->animSet()->mEntries.mData.get()[0];
    const u32 frameCount = miiverseAnimationFrameCount(animation->mController);
    hdPartsLayout_c* layout = mpLayout;
    mDragActive = 0;
    const s32 frame = gabi::ftoi(f32(frameCount));
    mSkipScrollAnimation = 0;
    mAnimationFrame = u32(frame);
    hdAnimSet_setFrame(layout->animSet(), 0, 0, f32(frame));
    vCalcLayout();
    const f32 position = setScrollFrame(s32(mAnimationFrame));
    for (u32 index = 0; index < 10; ++index)
        miiverseSetChartPresentation(mpChartButtons[index], 255, position);
    miiverseSetPreviousPresentation(mpPreviousButton, 255, position);
    miiverseSetNextPresentation(mpNextButton, 255, position);
}

/* 026A2A9C */
void hdMenuMiiverse_c::prepareFadeAnimation() {
    miiversePrepareChartAnimation(mpPrimaryButton);
    for (u32 index = 0; index < 10; ++index)
        miiversePrepareChartAnimation(mpChartButtons[index]);
    miiversePreparePreviousAnimation(mpPreviousButton);
    miiversePrepareNextAnimation(mpNextButton);
    vCalcLayout();
    vExecuteChildren();
    mDragActive = 0;
    mAnimationFrame = 0;
}

/* 026A2B5C */
f32 hdMenuMiiverse_c::setFadeFrameAlpha(s32 frame) {
    hdAnimSet_setFrame(mpLayout->animSet(), 1, 0, f32(frame));
    vCalcLayout();
    return f32(static_cast<hdMiiverseLayoutPane_l*>(mpRootPanes[0].get())->mAlpha);
}

/* 026A2BF8 */
f32 hdMenuMiiverse_c::setFadeFramePosition(s32 frame) {
    hdAnimSet_setFrame(mpLayout->animSet(), 1, 0, f32(frame));
    vCalcLayout();
    return static_cast<hdMiiverseLayoutPane_l*>(mpRootPanes[0].get())->mTranslationY;
}


void miiverseDisplayChartButton(hdMiiverseChartButton_l*);

/* 026A01AC */
void hdMenuMiiverse_c::setButtonsEnabled(u32 enabled) {
    mButtonsEnabled = enabled;
    for (u32 index = 0; index < 10; ++index) mpChartButtons[index]->mEnabled = enabled;
    static_cast<hdMiiversePager_l*>(mpPreviousButton.get())->mEnabled = enabled;
    static_cast<hdMiiversePager_l*>(mpNextButton.get())->mEnabled = enabled;
}

/* 026A0200 */
u32 hdMenuMiiverse_c::filledChartCount() const {
    u32 count = 0;
    for (u32 index = 0; index < 10; ++index) {
        if (mpChartButtons[index]->mFilled) ++count;
    }
    return count;
}

/* 026A2588 */
f32 hdMenuMiiverse_c::setScrollFrameAlpha(s32 frame) {
    hdAnimSet_setFrame(mpLayout->animSet(), 0, 0, f32(frame));
    vCalcLayout();
    return f32(static_cast<hdMiiverseLayoutPane_l*>(mpRootPanes[0].get())->mAlpha);
}

/* 026A2C74 */
void hdMenuMiiverse_c::displayChartButtons() {
    for (u32 index = 0; index < 10; ++index) miiverseDisplayChartButton(mpChartButtons[index]);
    miiverseDisplayChartButton(mpPrimaryButton);
}


static f32 sMiiverseZero = 0.0f;
static f32 sMiiverseOne = 1.0f;
static f32 sMiiverseMinScrollStep = 3.0f;
static f32 sMiiverseDragDamping = 0.2f;
static f32 sMiiverseDragMaxStep = 300.0f;
struct hdMiiverseScrollArchiveSlot_c { gptr<void> mpArchive; };
static hdMiiverseScrollArchiveSlot_c sMiiverseScrollArchive;
f32 miiverseGetScrollDivisor(void*);
f32 miiverseAddCalc(be<f32>*, f32, f32, f32, f32);
u32 miiverseChaseFloat(be<f32>*, f32, f32);

/* 026A0254 */
void hdMenuMiiverse_c::translateWindowPanes(f32 distance) {
    for (u32 index = 0; index < 3; ++index) {
        hdMiiverseLayoutPane_l* pane = static_cast<hdMiiverseLayoutPane_l*>(mpWindowPanes[index].get());
        f32 x = pane->mTranslationX;
        f32 z = pane->mTranslationZ;
        f32 y = pane->mTranslationY;
        u8 flags = pane->mTransformFlags;
        pane->mTranslationY = f32(y + distance);
        pane->mTranslationX = x;
        pane->mTransformFlags = flags | 0x10;
        pane->mTranslationZ = z;
    }
}

/* 026A0318 */
void hdMenuMiiverse_c::scrollBy(f32 distance) {
    hdMiiverseLayoutPane_l* pane = static_cast<hdMiiverseLayoutPane_l*>(mpWindowPanes[0].get());
    f32 current = pane->mTranslationY;
    f32 next = f32(current + distance);
    f32 upper = mScrollUpperBound;
    if (upper < next) distance = f32(upper - current);
    else {
        f32 lower = mScrollLowerBound;
        if (next < lower) distance = f32(lower - current);
    }
    if (distance == sMiiverseZero) return;
    translateWindowPanes(distance);
    for (u32 index = 0; index < 10; ++index) miiverseScrollChartButton(mpChartButtons[index], distance);
    miiverseScrollPreviousButton(mpPreviousButton, distance);
    miiverseScrollNextButton(mpNextButton, distance);
}

/* 026A040C */
u32 hdMenuMiiverse_c::easeScrollToLowerBound() {
    f32 current = static_cast<hdMiiverseLayoutPane_l*>(mpWindowPanes[0].get())->mTranslationY;
    f32 divisor = miiverseGetScrollDivisor(sMiiverseScrollArchive.mpArchive);
    f32 difference = f32(mScrollLowerBound - current);
    f32 magnitude = __builtin_fabsf(difference);
    f32 step = f32(magnitude / divisor);
    if (step < sMiiverseMinScrollStep) step = sMiiverseMinScrollStep;
    scrollBy(-step);
    return mScrollLowerBound == current;
}

/* 026A097C */
u32 hdMenuMiiverse_c::updateWindowScale(u32 snap) {
    u32 reached = miiverseChaseFloat(&mWindowScale, mWindowTargetScale, mWindowScaleStep) != 0;
    f32 firstScale = mWindowScale;
    if (snap) { firstScale = mWindowTargetScale; mWindowScale = firstScale; }
    hdMiiverseLayoutPane_l* edge = static_cast<hdMiiverseLayoutPane_l*>(mpWindowPanes[0].get());
    u8 flags = edge->mTransformFlags;
    edge->mScaleY = firstScale;
    edge->mTransformFlags = flags | 0x10;
    hdMiiverseLayoutPane_l* base = static_cast<hdMiiverseLayoutPane_l*>(mpWindowPanes[1].get());
    flags = base->mTransformFlags;
    base->mScaleY = mWindowScale;
    base->mTransformFlags = flags | 0x10;
    base = static_cast<hdMiiverseLayoutPane_l*>(mpWindowPanes[1].get());
    hdMiiverseLayoutPane_l* glow = static_cast<hdMiiverseLayoutPane_l*>(mpWindowPanes[2].get());
    f32 x = base->mScaleX;
    flags = glow->mTransformFlags;
    f32 y = mWindowScale;
    glow->mScaleX = x;
    glow->mScaleY = y;
    glow->mTransformFlags = flags | 0x10;
    return reached;
}

/* 026A1B54 */
void hdMenuMiiverse_c::dampDragVelocity() {
    if (mDragMode == 1 && mVelocityY != sMiiverseZero) {
        miiverseAddCalc(&mVelocityY, sMiiverseZero, sMiiverseDragDamping, sMiiverseDragMaxStep, sMiiverseOne);
        scrollBy(mVelocityY);
        mDragActive = 1;
    }
}

/* 026A2128 */
void hdMenuMiiverse_c::scrollToLowerBound() {
    f32 current = static_cast<hdMiiverseLayoutPane_l*>(mpWindowPanes[0].get())->mTranslationY;
    scrollBy(f32(mScrollLowerBound - current));
}


static hdSaveInfoHolder_c* sMiiverseMainSaveInfo;
hdSavePair_c* miiverseScratchPair(hdSaveHolders_c*);
struct hdMiiverseTouchDevice_l {
    u8 mDeviceStorage[0x10C];
    be<u32> mHeldButtons;
};
struct hdMiiverseTouchPoint_l { be<f32> mX, mY; };
struct hdMiiverseNavigationInput_l {
    gptr<hdMiiverseTouchDevice_l> mpDevice;
    u8 mInputStorage[0x14];
    be<u32> mPrimaryButtons;
    u8 mInputPadding[4];
    be<u32> mSecondaryButtons;
    be<u32> mTouchFlags;
    u8 mTouchPadding[0x18C];
    hdMiiverseTouchPoint_l mTouchPosition;
    u8 mIdentityPadding[0x14];
    be<u32> mInputId;
};
WWHD_OFFSET(hdMiiverseNavigationInput_l, mInputId, 0x1D0);
WWHD_OFFSET(hdMiiverseNavigationInput_l, mTouchFlags, 0x24);
WWHD_OFFSET(hdMiiverseNavigationInput_l, mTouchPosition, 0x1B4);
WWHD_OFFSET(hdMiiverseTouchDevice_l, mHeldButtons, 0x10C);
WWHD_OFFSET(hdMiiverseNavigationInput_l, mPrimaryButtons, 0x18);
WWHD_OFFSET(hdMiiverseNavigationInput_l, mSecondaryButtons, 0x20);
static hdMiiverseNavigationInput_l* sMiiverseNavigationInput;

/* 026A0160 */
static u32 miiverseIsSupportedSaveMode() {
    hdSavePair_c* pair = miiverseScratchPair(&sMiiverseMainSaveInfo->mHolders);
    u8 mode = pair->mPrimary;
    return mode == 0 || mode == 1;
}

/* 026A1724 */
static u32 miiverseNavigationButtonsHeld() {
    const u32 kNavigationButtons = 0x00FF0000;
    return ((sMiiverseNavigationInput->mPrimaryButtons | sMiiverseNavigationInput->mSecondaryButtons) & kNavigationButtons) != 0;
}


void* miiverseAllocateStorage(u32);
void miiverseConstructSoundFader(void*, f32);
void miiverseSetSoundFade(void*, u32, f32);

/* Embedded storage constructors allocate only when the member address wraps to NULL. */
static inline void miiverseEnsureStorage(hdMenuMiiverse_c* screen, u32 offset, u32 size) {
    if (u32(gabi::ea(screen) + offset) == 0) miiverseAllocateStorage(size);
}

/* 026A3A4C */
hdMenuMiiverse_c* hdMenuMiiverse_c::construct(hdMenuMiiverse_c* screen) {
    if (!screen) {
        screen = static_cast<hdMenuMiiverse_c*>(miiverseAllocateStorage(0x204));
        if (!screen) return nullptr;
    }
    hdParts_c::construct(screen);
    screen->mVtable = &sMiiverseVtable;
    screen->mStateHolder.mHolderVtable = &sMiiverseHolderVtable;
    screen->mSecondaryVtable = &sMiiverseSecondaryVtable;
    screen->mInputEnabled = 0;
    miiverseEnsureStorage(screen, 0x58, 0x28);
    screen->mButtonsEnabled = 0;
    screen->mConfirmationStep = 0;
    screen->mHasPreviousPage = 0;
    screen->mpPreviousButton = nullptr;
    screen->mpNextButton = nullptr;
    screen->mpContentResource = nullptr;
    screen->mSelectionStart = 0;
    screen->mClosingRequested = 0;
    hdScreenPaneArray* pictures = gabi::at<hdScreenPaneArray>(gabi::ea(screen) + offsetof(hdMenuMiiverse_c, mPicturePanes));
    if (!pictures) pictures = static_cast<hdScreenPaneArray*>(miiverseAllocateStorage(8));
    if (pictures) { pictures->mData = nullptr; pictures->mCount = 0; }
    screen->mSkipScrollAnimation = 0;
    screen->mMessageOption = 0;
    f32 zero = sMiiverseZero;
    screen->mContentReady = 0;
    screen->mTouchPosition = zero;
    screen->mSessionValue = 0;
    screen->mMessageValue = 0;
    screen->mSavedInputMode = 0;
    screen->mSavedButtonMode = 0;
    screen->mInitialized = 0;
    screen->mMessageCategory = 0;
    screen->mMessageId = 0;
    screen->mTouchActive = 0;
    screen->mInputMode = 0;
    screen->mScrollFraction = zero;
    be<f32>* touch = gabi::at<be<f32>>(gabi::ea(screen) + offsetof(hdMenuMiiverse_c, mLastTouchX));
    if (!touch) touch = static_cast<be<f32>*>(miiverseAllocateStorage(8));
    if (touch) { touch[1] = zero; touch[0] = zero; }
    screen->mDragMode = 0;
    screen->mScrollUpperBound = zero;
    screen->mDragActive = 0;
    screen->mScrollLowerBound = zero;
    miiverseEnsureStorage(screen, 0x190, 4);
    f32 one = sMiiverseOne;
    screen->mVelocityY = zero;
    miiverseConstructSoundFader(screen->mSoundStorage, one);
    screen->mWindowScale = zero;
    screen->mWindowScaleStep = zero;
    screen->mWindowTargetScale = zero;
    screen->mWindowWidth = zero;
    be<f32>* bounds = gabi::at<be<f32>>(gabi::ea(screen) + offsetof(hdMenuMiiverse_c, mTouchMinX));
    if (!bounds) bounds = static_cast<be<f32>*>(miiverseAllocateStorage(0x10));
    if (bounds) { bounds[2] = zero; bounds[1] = zero; bounds[0] = zero; bounds[3] = zero; }
    miiverseEnsureStorage(screen, 0x1D4, 8);
    miiverseEnsureStorage(screen, 0x1DC, 4);
    miiverseEnsureStorage(screen, 0x1E0, 4);
    miiverseEnsureStorage(screen, 0x1E4, 0xC);
    miiverseEnsureStorage(screen, 0x1F0, 4);
    screen->mMessageResult = 0;
    screen->mSelectedChart = 0;
    screen->mMessageKind = 0;
    for (u32 index = 0; index < 10; ++index) screen->mpChartButtons[index] = nullptr;
    screen->mVelocityX = zero;
    miiverseSetSoundFade(screen->mSoundStorage, 0, zero);
    screen->mTransferHandle = 0;
    return screen;
}


static EventMgr* sMiiverseMainEventMgr;
hdUiEvent* miiverseCopyInputEvent(hdUiEvent*, hdUiEvent*);
Pair32 miiverseTouchFromStart(EventMgr*);

/* 026A1D64 */
void hdMenuMiiverse_c::beginDrag() {
    gabi::NativeFrame<0x40> frame;
    f32 zero = sMiiverseZero;
    mVelocityX = zero;
    mVelocityY = zero;
    EventMgr* manager = sMiiverseMainEventMgr;
    hdUiEvent* event = hdPartsView<hdUiEvent>(frame + 0x10);
    hdUiEvent* current = hdPartsView<hdUiEvent>(gabi::ea(manager) + 0x30);
    miiverseCopyInputEvent(event, current);
    miiverseTouchFromStart(manager);
    const auto position = miiverseTouchFromStart(manager);
    mTouchPosition = gabi::f32_from_bits(position.r4);
    clearSelectionFlags();
    mDragActive = 1;
}

static f32 sMiiverseScrollSoundScale = 0.03f;
static f32 sMiiverseNegativeOne = -1.0f;
static f32 sMiiverseHalf = 0.5f;
void miiversePlayScrollSound(const char*, f32, f32);

/* 026A1DE8 */
void hdMenuMiiverse_c::updateScrollSound(f32 speed) {
    if (!mDragActive) return;
    f32 amount = f32(speed * sMiiverseScrollSoundScale);
    const f32 minimum = sMiiverseNegativeOne;
    const f32 one = sMiiverseOne;
    if (amount < minimum) amount = minimum;
    else if (amount > one) amount = one;
    miiverseSetSoundFade(mSoundStorage, 3, f32(amount * amount));
    const f32 fade = *hdPartsView<be<f32>>(gabi::ea(mSoundStorage));
    const f32 volume = f32(fade * sMiiverseHalf);
    miiversePlayScrollSound("SE_MIIVERSE_SCROLL", volume, f32(volume + one));
}

static hdMiiverseTouchPoint_l sMiiverseDefaultTouch;
static f32 sMiiverseDragThresholdSquared = 400.0f;

/* 026A1400 */
u32 hdMenuMiiverse_c::checkTouchDrag() {
    gabi::NativeFrame<0x50> frame;
    EventMgr* manager = sMiiverseMainEventMgr;
    hdMiiverseNavigationInput_l* input = sMiiverseNavigationInput;
    miiverseCopyInputEvent(hdPartsView<hdUiEvent>(frame + 0x20),
                           hdPartsView<hdUiEvent>(gabi::ea(manager) + 0x30));
    if (!(input->mpDevice->mHeldButtons & 0x8000)) {
        mTouchActive = 0;
        return 0;
    }
    mDragMode = 1;
    if (!mTouchActive) {
        hdMiiverseTouchPoint_l* start = &sMiiverseDefaultTouch;
        if (input->mTouchFlags & 1) start = &input->mTouchPosition;
        mLastTouchX = start->mX;
        const f32 initialY = start->mY;
        mTouchActive = 1;
        mLastTouchY = initialY;
    }
    hdMiiverseTouchPoint_l* point = &sMiiverseDefaultTouch;
    if (input->mTouchFlags & 1) point = &input->mTouchPosition;
    const f32 x = point->mX;
    const f32 y = point->mY;
    const f32 dx = f32(x - mLastTouchX);
    const f32 dy = f32(y - mLastTouchY);
    const f32 distanceSquared = gabi::fmadds(dx, dx, f32(dy * dy));
    if (!(distanceSquared > sMiiverseDragThresholdSquared)) return 0;
    if (mTouchMinX > x || x > mTouchMaxX || mTouchMinY > y || y > mTouchMaxY) return 0;
    return 1;
}

void miiverseSetInputMode(EventMgr*, u32);
void miiverseFocusPicture(EventMgr*, void*);

/* 026A1584 */
u32 hdMenuMiiverse_c::beginTouchDrag() {
    if (!mButtonsEnabled) return 0;
    EventMgr* manager = sMiiverseMainEventMgr;
    if (checkTouchDrag()) {
        miiverseSetInputMode(manager, 0);
        return 1;
    }
    miiverseFocusPicture(manager, mPicturePanes.mData.get()[0].get());
    return 0;
}

f32 miiverseNextButtonPosition(hdParts_c*);
f32 miiverseNextButtonHeight(hdParts_c*);
f32 miiverseChartSpacing(void*);
f32 miiverseWindowScaleDuration(void*);

/* 026A082C */
void hdMenuMiiverse_c::prepareWindowScale() {
    const f32 windowY = static_cast<hdMiiverseLayoutPane_l*>(mpWindowPanes[1].get())->mTranslationY;
    const f32 half = sMiiverseHalf;
    void* archive = sMiiverseScrollArchive.mpArchive;
    const f32 buttonPosition = miiverseNextButtonPosition(mpNextButton);
    const f32 buttonHeight = miiverseNextButtonHeight(mpNextButton);
    const f32 buttonEdge = gabi::fnmsubs(buttonHeight, half, buttonPosition);
    const f32 spacing = miiverseChartSpacing(archive);
    const f32 target = f32(windowY - f32(buttonEdge - spacing));
    if (mWindowWidth < target) {
        const f32 scale = static_cast<hdMiiverseLayoutPane_l*>(mpWindowPanes[0].get())->mScaleY;
        mWindowTargetScale = target;
        mWindowScale = scale;
        const f32 duration = miiverseWindowScaleDuration(archive);
        const f32 current = mWindowScale;
        const f32 desired = mWindowTargetScale;
        const f32 difference = __builtin_fabsf(f32(desired - current));
        const f32 upper = gabi::fnmsubs(mWindowWidth, half, target);
        const f32 step = f32(difference / duration);
        mScrollUpperBound = upper;
        mWindowScaleStep = step;
    } else {
        const f32 lower = mScrollLowerBound;
        const f32 width = mWindowWidth;
        mScrollUpperBound = lower;
        mWindowScale = width;
        mWindowTargetScale = width;
    }
}

/* 026A0094 */
u32 hdMenuMiiverse_c::findSelectedChart() {
    hdPartsStateId_c* selectedState = &sMiiverseCommentPhotoState;
    for (u32 index = 0; index < 10; ++index) {
        hdPartsStateIdVtbl_c* wantedTable = selectedState->mVtable;
        hdPartsStateId_c* current = hdParts_getCurrentStateId(mpChartButtons[index]->stateMachine());
        const u32 currentKey = current->getKey();
        const u32 wantedKey = gabi::call_ptr<u32>(wantedTable->mGetKey.get(), selectedState);
        if (currentKey == wantedKey) {
            mSelectedChart = index;
            return 1;
        }
    }
    return 0;
}

/* 026A50DC */
void hdMenuMiiverse_c::executeActiveParts() {
    mpPrimaryButton->vExecuteState();
    for (u32 index = 0; index < 10; ++index)
        mpChartButtons[index]->vExecuteState();
    hdPartsStateId_c* current = hdParts_getCurrentStateId(stateMachine());
    hdPartsStateId_c* photoView = &sMiiversePhotoViewState;
    hdPartsStateIdVtbl_c* photoTable = photoView->mVtable;
    const u32 currentKey = current->getKey();
    const u32 photoKey = gabi::call_ptr<u32>(photoTable->mGetKey.get(), photoView);
    if (currentKey == photoKey) {
        mpMessageScreen->vExecuteState();
    } else {
        mpPreviousButton->vExecuteState();
        mpNextButton->vExecuteState();
    }
}

/* 026A35D4 */
void hdMenuMiiverse_c::setChartConfirmationStates() {
    hdPartsStateId_c* filledState = &sMiiverseCommentCheckInputState;
    hdPartsStateId_c* emptyState = &sMiiverseCommentWaitState;
    for (u32 index = 0; index < 10; ++index) {
        hdMiiverseChartButton_l* chart = mpChartButtons[index];
        chart->changeState(chart->mFilled ? filledState : emptyState);
    }
}

/* 026A5098 */
void hdMenuMiiverse_c::updateInputIdentity() {
    u32 inputId = sMiiverseNavigationInput->mInputId;
    if (inputId != mLastInputId) {
        clearSelectionFlags();
        inputId = sMiiverseNavigationInput->mInputId;
    }
    mLastInputId = inputId;
}

u32 miiverseGetInputMode(EventMgr*);

/* 026A1630 */
u32 hdMenuMiiverse_c::isChartOrPagerInputMode() {
    const u32 mode = miiverseGetInputMode(sMiiverseMainEventMgr);
    for (u32 index = 0; index < 10; ++index) {
        if (mpChartButtons[index]->mFilled) {
            for (u32 child = 0; child < 4; ++child) {
                if (mode == chartControlId(index, child)) return 1;
            }
        }
    }
    if (mode == previousControlId()) return 1;
    return mode == nextControlId();
}

struct hdMiiverseInputOptionsSlot_c { gptr<cking::ui::input::Options> mpOptions; };
static hdMiiverseInputOptionsSlot_c sMiiverseInputOptions;
static u8 sMiiverseGlobalInputMode;
void miiverseFinishSelectedChart(hdMiiverseChartButton_l*);
dOwPlayView_c* miiverseGetPlayInfo();

/* 026A39CC */
void hdMenuMiiverse_c::restoreSavedControls() {
    const u32 selected = mSelectedChart;
    miiverseFinishSelectedChart(mpChartButtons[selected < 10 ? selected : 0]);
    const u8 buttonMode = mSavedButtonMode;
    sMiiverseInputOptions.mpOptions->mDirectionalInputEnabled = buttonMode;
    dOwPlayView_c* play = miiverseGetPlayInfo();
    const u8 savedA = play->mSavedActionButtonA;
    const u8 savedB = play->mSavedActionButtonB;
    play->mActionButtonA = savedA;
    const u8 savedR = play->mSavedActionButtonR;
    play->mActionButtonB = savedB;
    play->mActionButtonR = savedR;
    sMiiverseGlobalInputMode = mSavedInputMode;
}

struct hdMiiversePictureNameSlot_c { gptr<const char> mpName; };
static hdMiiversePictureNameSlot_c sMiiversePictureName;
static u32 sMiiversePictureTypeReady = 0;
static u32 sMiiversePictureBaseTypeReady = 0;
static hdPartsTypeInfo_c sMiiversePictureType = {0};
static hdPartsTypeInfo_c sMiiversePictureBaseType = {0};

/* 026A421C */
void hdMenuMiiverse_c::findPicturePane() {
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    hdScreenResourceTable* table = hdPartsView<hdScreenResourceTable>(root->mVtable.v);
    hdPartsPane_c* pane = gabi::call_ptr<hdPartsPane_c*>(
        table->mFindPane.get(), root, sMiiversePictureName.mpName.get(), 1);
    if (!sMiiversePictureTypeReady) {
        sMiiversePictureTypeReady = 1;
        if (!sMiiversePictureBaseTypeReady) {
            sMiiversePictureBaseTypeReady = 1;
            hdPartsTypeInfo_c* rootType = &sMiiverseRootType;
            if (!sMiiverseRootTypeReady) {
                sMiiverseRootTypeReady = 1;
                rootType->mParent = nullptr;
            }
            sMiiversePictureBaseType.mParent = rootType;
        }
        sMiiversePictureType.mParent = &sMiiversePictureBaseType;
    }
    if (pane) {
        hdPartsTypeInfo_c* type = pane->getRuntimeTypeInfo();
        while (type && type != &sMiiversePictureType) type = type->mParent;
        if (!type) pane = nullptr;
    }
    mpPicturePane = pane;
}

u32 miiverseGetFocusedReceiver(EventMgr*);
void miiverseResetAnimation(hdAnimSet*, u32, u32);
struct hdMiiversePageArchive_l {
    u8 mArchiveStorage[0x4FD5];
    be<u8> mHasPreviousPage;
};
WWHD_OFFSET(hdMiiversePageArchive_l, mHasPreviousPage, 0x4FD5);

/* 026A26A0 */
void hdMenuMiiverse_c::updateScrollAnimation() {
    hdPartsLayout_c* layout = mpLayout;
    mAnimationFrame = u32(mAnimationFrame) + 1;
    hdAnimSetEntry* animation = layout->animSet()->mEntries.mData.get()[0];
    const u32 frameCount = miiverseAnimationFrameCount(animation->mController);
    const s32 frame = s32(mAnimationFrame);
    if (f32(frameCount) < f32(frame)) {
        if (miiverseGetFocusedReceiver(sMiiverseMainEventMgr)) return;
        mSkipScrollAnimation = 0;
        changeState(&sMiiverseCheckInputState);
        for (u32 index = 0; index < 10; ++index) {
            hdMiiverseChartButton_l* chart = mpChartButtons[index];
            if (chart->mFilled) chart->changeState(&sMiiverseCommentCheckInputState);
        }
        mpNextButton->changeState(&sMiiverseSettingCheckInputState);
        hdMiiversePageArchive_l* archive = hdPartsView<hdMiiversePageArchive_l>(gabi::ea(sMiiverseScrollArchive.mpArchive.get()));
        if (archive->mHasPreviousPage)
            mpPreviousButton->changeState(&sMiiverseNewCommentCheckInputState);
        return;
    }
    const u32 alpha = u32(gabi::ftoi(setScrollFrameAlpha(frame)));
    const f32 position = setScrollFrame(s32(mAnimationFrame));
    for (u32 index = 0; index < 10; ++index)
        miiverseSetChartPresentation(mpChartButtons[index], alpha, position);
    miiverseSetPreviousPresentation(mpPreviousButton, alpha, position);
    miiverseSetNextPresentation(mpNextButton, alpha, position);
    hdAnimSet* animations = mpLayout->animSet();
    if (!mSkipScrollAnimation)
        hdAnimSet_setFrame(animations, 0, 0, f32(s32(mAnimationFrame)));
    else
        miiverseResetAnimation(animations, 0, 0);
    vCalcLayout();
}

struct hdMiiversePictureSaveSlot_c { gptr<void> mpSave; };
static hdMiiversePictureSaveSlot_c sMiiversePictureSave;
void miiverseAdvanceArchivePage(void*);
void miiverseDisplayPreviousButton(hdParts_c*);
void miiverseResetPictureSave(void*);
void miiverseFinishAnimation(hdAnimSet*, u32, u32);

/* 026A2CF0 */
void hdMenuMiiverse_c::updateFadeAnimation() {
    hdPartsLayout_c* layout = mpLayout;
    mAnimationFrame = u32(mAnimationFrame) + 1;
    hdAnimSetEntry* animation = *layout->animSet()->mEntries.at(1);
    const u32 frameCount = miiverseAnimationFrameCount(animation->mController);
    const s32 frame = s32(mAnimationFrame);
    if (f32(frameCount) < f32(frame)) {
        if (mSkipScrollAnimation) {
            miiverseAdvanceArchivePage(sMiiverseScrollArchive.mpArchive);
            displayChartButtons();
            miiverseDisplayPreviousButton(mpPreviousButton);
            scrollToLowerBound();
            mHasPreviousPage = 0;
            miiverseResetPictureSave(sMiiversePictureSave.mpSave);
            const u32 mode = nextControlId();
            miiverseSetInputMode(sMiiverseMainEventMgr, mode);
            changeState(&sMiiverseInAllState);
        } else {
            changeState(&sMiiversePartsBlankState);
            mSkipScrollAnimation = 0;
        }
        return;
    }
    const u32 alpha = u32(gabi::ftoi(setFadeFrameAlpha(frame)));
    const f32 position = setFadeFramePosition(s32(mAnimationFrame));
    miiverseSetChartPresentation(mpPrimaryButton, alpha, position);
    for (u32 index = 0; index < 10; ++index)
        miiverseSetChartPresentation(mpChartButtons[index], alpha, position);
    miiverseSetPreviousPresentation(mpPreviousButton, alpha, position);
    miiverseSetNextPresentation(mpNextButton, alpha, position);
    hdAnimSet* animations = mpLayout->animSet();
    if (!mSkipScrollAnimation)
        hdAnimSet_setFrame(animations, 1, 0, f32(s32(mAnimationFrame)));
    else
        miiverseFinishAnimation(animations, 1, 0);
    vCalcLayout();
}

/* 026A2FB4 */
void hdMenuMiiverse_c::finishFadeAnimation() {
    hdAnimSetEntry* animation = *mpLayout->animSet()->mEntries.at(1);
    const u32 frameCount = miiverseAnimationFrameCount(animation->mController);
    const s32 frame = gabi::ftoi(f32(frameCount));
    mAnimationFrame = u32(frame);
    const f32 position = setFadeFramePosition(frame);
    miiverseSetChartPresentation(mpPrimaryButton, 0, position);
    for (u32 index = 0; index < 10; ++index)
        miiverseSetChartPresentation(mpChartButtons[index], 0, position);
    miiverseSetPreviousPresentation(mpPreviousButton, 0, position);
    miiverseSetNextPresentation(mpNextButton, 0, position);
    hdAnimSet* animations = mpLayout->animSet();
    if (!mSkipScrollAnimation)
        hdAnimSet_setFrame(animations, 1, 0, f32(s32(mAnimationFrame)));
    else
        miiverseFinishAnimation(animations, 1, 0);
}

/* 026A1EC8 */
void hdMenuMiiverse_c::updateTouchDrag() {
    gabi::NativeFrame<0x58> frame;
    const u32 navigationHeld = miiverseNavigationButtonsHeld();
    const f32 zero = sMiiverseZero;
    if (navigationHeld) {
        mTouchPosition = zero;
        mVelocityX = zero;
        changeState(&sMiiverseCheckInputState);
        vCalcLayout();
        return;
    }
    EventMgr* manager = sMiiverseMainEventMgr;
    miiverseCopyInputEvent(hdPartsView<hdUiEvent>(frame + 0x10),
                           hdPartsView<hdUiEvent>(gabi::ea(manager) + 0x30));
    if (checkTouchDrag()) {
        const auto touch = miiverseTouchFromStart(manager);
        const f32 position = gabi::f32_from_bits(touch.r4);
        f32 distance = f32(position - mTouchPosition);
        const f32 current = static_cast<hdMiiverseLayoutPane_l*>(mpWindowPanes[0].get())->mTranslationY;
        const f32 next = f32(current + distance);
        f32 bound = mScrollUpperBound;
        if (bound < next) distance = f32(bound - current);
        else {
            bound = mScrollLowerBound;
            if (next < bound) distance = f32(bound - current);
        }
        mVelocityX = distance;
        mVelocityY = distance;
        mTouchPosition = position;
        updateScrollSound(distance);
        scrollBy(distance);
    } else {
        mTouchPosition = zero;
        mVelocityX = zero;
        const f32 velocity = mVelocityY;
        changeState(&sMiiverseCheckInputState);
        scrollBy(velocity);
    }
    if (mButtonsEnabled)
        miiverseFocusPicture(sMiiverseMainEventMgr, mPicturePanes.mData.get()[0].get());
    vCalcLayout();
}

/* 026A4650 */
void hdMenuMiiverse_c::updateMessageText() {
    gabi::NativeFrame<0x20> frame;
    const u32 result = mMessageResult;
    u32 stringOffset;
    u32 nameAddress;
    if (result == 0 || result == 1) {
        stringOffset = 8;
        nameAddress = gabi::ea(&sMiiverseMessageNames[2]);
    } else if (result == 2) {
        stringOffset = 0x10;
        nameAddress = gabi::ea(&sMiiverseMessageNames[1]);
    } else if (result == 3) {
        stringOffset = 0x18;
        nameAddress = gabi::ea(&sMiiverseMessageNames[0]);
    } else {
        return;
    }
    be<u8>* messageResource = static_cast<be<u8>*>(mpLayout->lyt()->mpMessageData.get());
    hdUiSafeString* resource = hdPartsView<hdUiSafeString>(frame + stringOffset);
    resource->mVtable = &sMiiverseStringVtable;
    resource->mText = messageResource;
    mpTextPane->resetMessage(resource, hdPartsView<hdUiSafeString>(nameAddress));
}

struct hdMiiverseArchivePicture_l {
    u8 mMetadataStorage[0x25A];
    be<u8> mDisplayed;
    be<u8> mAvailable;
    be<u8> mLabel;
    u8 mSubjectPadding[3];
    be<u32> mSubject;
    be<u8> mState;
    u8 mChecksumPadding[3];
    be<u32> mPictureId, mDataSize;
    gptr<void> mpJpegData;
    u8 mPictureStorage[0x178];
};
WWHD_OFFSET(hdMiiverseArchivePicture_l, mPictureId, 0x268);
WWHD_SIZE(hdMiiverseArchivePicture_l, 0x3EC);
struct hdMiiverseArchivePictures_l {
    u8 mHeaderStorage[0x14];
    hdMiiverseArchivePicture_l mPictures[10];
    u8 mPageStorage[0x2878];
    be<u32> mLastEntry;
    u8 mStatusStorage[0xD];
    be<u8> mHasPreviousPage, mPreviousPageDisplayed;
};
WWHD_OFFSET(hdMiiverseArchivePictures_l, mPictures, 0x14);
void miiverseSetChartAvailable(hdMiiverseChartButton_l*, u32);
u32 miiverseChartHasPicture(hdMiiverseChartButton_l*);
hdSavePicture_l* miiverseGetPictureHolder(hdSaveHolders_c*);
hdJpeg_c* miiverseGetSavedPicture(hdSavePicture_l*, u32);
u32 miiverseSavedPictureId(hdJpeg_c*);
void miiverseCopyChart(hdMiiverseChartButton_l*, hdMiiverseChartButton_l*);

/* 026A0A38 */
void hdMenuMiiverse_c::refreshDuplicatePictureFlags() {
    for (u32 chart = 0; chart < 10; ++chart)
        miiverseSetChartAvailable(mpChartButtons[chart], 1);
    hdSavePicture_l* holder = miiverseGetPictureHolder(&sMiiverseMainSaveInfo->mHolders);
    hdMiiverseArchivePictures_l* archive = hdPartsView<hdMiiverseArchivePictures_l>(gabi::ea(sMiiverseScrollArchive.mpArchive.get()));
    for (u32 chart = 0; chart < 10; ++chart) {
        if (!mpChartButtons[chart]->mFilled) continue;
        if (!miiverseChartHasPicture(mpChartButtons[chart])) continue;
        u32 entry = mpChartButtons[chart]->mArchiveIndex;
        if (entry >= 10) entry = 0;
        const u32 pictureId = archive->mPictures[entry].mPictureId;
        for (u32 picture = 0; picture < 12; ++picture) {
            hdJpeg_c* saved = miiverseGetSavedPicture(holder, picture);
            if (miiverseSavedPictureId(saved) != pictureId) continue;
            miiverseSetChartAvailable(mpChartButtons[chart], 0);
            hdMiiverseChartButton_l* button = mpChartButtons[chart];
            button->vExecuteChildren();
            button->vCalcLayout();
        }
    }
}

/* 026A0C28 */
void hdMenuMiiverse_c::prependPrimaryChart() {
    for (u32 destination = 9; destination > 0; --destination) {
        if (mpChartButtons[destination - 1]->mFilled)
            miiverseCopyChart(mpChartButtons[destination], mpChartButtons[destination - 1]);
    }
    miiverseCopyChart(mpChartButtons[0], mpPrimaryButton);
    miiverseDisplayChartButton(mpPrimaryButton);
    refreshDuplicatePictureFlags();
}

struct hdMiiverseAnimationGroups_l { be<u32> mGroup[3]; };
static hdMiiverseAnimationGroups_l sMiiverseAnimationGroups;
static hdUiSafeString sMiiverseAnimationNames[2] = {};
static hdUiSafeString sMiiverseGroupNames[3] = {};
void miiverseBindAnimationGroup(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);

/* 026A62C0 */
u32 hdMenuMiiverse_c::loadLayout(void* name, u32 resource, u32 options, u32 heapOptions, void* descriptor) {
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(0x110, sMiiverseUiSystem.mpSystem->mHeap, 4);
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, resource, options, heapOptions, 3, 2, descriptor)) return 0;
    for (u32 group = 0; group < 3; ++group) {
        u32 animation = sMiiverseAnimationGroups.mGroup[group];
        miiverseBindAnimationGroup(mpLayout->animSet(), group, &sMiiverseGroupNames[group], &sMiiverseAnimationNames[animation]);
    }
    vCalcLayout();
    return 1;
}

void miiverseSetInputMode(EventMgr*, u32);
f32 miiversePreviousButtonPosition(hdParts_c*);
f32 miiversePreviousButtonHeight(hdParts_c*);
f32 miiverseChartButtonPosition(hdMiiverseChartButton_l*);

/* 026A1748 */
void hdMenuMiiverse_c::focusFirstVisibleControl(u32 requireEnabled) {
    if (requireEnabled && !mButtonsEnabled) return;
    const f32 upper = messageUpperBound();
    hdMiiversePageArchive_l* archive = hdPartsView<hdMiiversePageArchive_l>(gabi::ea(sMiiverseScrollArchive.mpArchive.get()));
    if (archive->mHasPreviousPage) {
        const f32 position = miiversePreviousButtonPosition(mpPreviousButton);
        const f32 height = miiversePreviousButtonHeight(mpPreviousButton);
        const f32 edge = gabi::fnmsubs(height, sMiiverseHalf, position);
        if (edge < upper) {
            const u32 mode = previousControlId();
            miiverseSetInputMode(sMiiverseMainEventMgr, mode);
            return;
        }
    }
    for (u32 chart = 0; chart < 10; ++chart) {
        if (!mpChartButtons[chart]->mFilled) continue;
        const f32 position = miiverseChartButtonPosition(mpChartButtons[chart]);
        if (position < upper) {
            const u32 mode = chartControlId(chart, 0);
            miiverseSetInputMode(sMiiverseMainEventMgr, mode);
            return;
        }
    }
    const u32 mode = nextControlId();
    miiverseSetInputMode(sMiiverseMainEventMgr, mode);
}

u32 miiverseQueryPrimaryMode();
u32 miiverseQuerySecondaryMode();

/* 026A5214 */
u32 hdMenuMiiverse_c::updateSaveModeMessage() {
    const u32 oldResult = mMessageResult;
    if (oldResult == 3) return 0;
    u32 newResult;
    if (oldResult == 0) {
        if (miiverseQuerySecondaryMode()) newResult = 1;
        else {
            if (miiverseIsSupportedSaveMode()) return 0;
            newResult = 2;
        }
    } else if (oldResult == 1) {
        if (miiverseQueryPrimaryMode()) newResult = 0;
        else {
            if (miiverseIsSupportedSaveMode()) return 0;
            newResult = 2;
        }
    } else {
        if (miiverseQuerySecondaryMode()) newResult = 1;
        else if (miiverseQueryPrimaryMode()) newResult = 0;
        else return 0;
    }
    mMessageResult = newResult;
    updateMessageText();
    mVelocityY = sMiiverseZero;
    const s32 count = s32(filledChartCount());
    hdMiiversePager_l* previous = static_cast<hdMiiversePager_l*>(mpPreviousButton.get());
    if (count > 0 || previous->mFilled || mpPrimaryButton->mFilled) {
        mConfirmationStep = 0;
        mSkipScrollAnimation = 1;
        changeState(&sMiiverseOutAllState);
    }
    return 1;
}

/* 026A53A4 */
void hdMenuMiiverse_c::refreshDraggingLayouts() {
    if (!mDragActive) return;
    hdParts_c* button = mpPrimaryButton;
    button->vExecuteChildren();
    button->vCalcLayout();
    for (u32 chart = 0; chart < 10; ++chart) {
        button = mpChartButtons[chart];
        button->vExecuteChildren();
        button->vCalcLayout();
    }
    button = mpPreviousButton;
    button->vExecuteChildren();
    button->vCalcLayout();
    button = mpNextButton;
    button->vExecuteChildren();
    button->vCalcLayout();
}

/* 026A55E8 */
void hdMenuMiiverse_c::drawVisibleParts(void* drawInfo) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(stateMachine());
    hdPartsStateId_c* hidden = &sMiiversePartsBlankState;
    hdPartsStateIdVtbl_c* hiddenTable = hidden->mVtable;
    const u32 currentKey = current->getKey();
    const u32 hiddenKey = gabi::call_ptr<u32>(hiddenTable->mGetKey.get(), hidden);
    if (currentKey == hiddenKey) return;
    vDrawLayout(drawInfo);
    vDrawChildren(drawInfo);
    mpPrimaryButton->vDraw(drawInfo);
    for (u32 chart = 0; chart < 10; ++chart)
        mpChartButtons[chart]->vDraw(drawInfo);
    mpPreviousButton->vDraw(drawInfo);
    mpNextButton->vDraw(drawInfo);
}

struct hdMiiversePictureReceiverTable_l {
    u8 mTablePrefix[0xC];
    gfn mDelete;
};
struct hdMiiversePictureReceiver_l {
    u8 mReceiverPrefix[0x14];
    gptr<hdMiiversePictureReceiverTable_l> mVtable;
    u8 mInputStorage[0x40];
    be<u8> mEnabled;
};
WWHD_OFFSET(hdMiiversePictureReceiver_l, mVtable, 0x14);
WWHD_OFFSET(hdMiiversePictureReceiverTable_l, mDelete, 0xC);
struct hdMiiverseHeapManagerSlot_l { gptr<void> mpManager; };
static hdMiiverseHeapManagerSlot_l sMiiverseHeapManager;

/* 026A4BFC */
void hdMenuMiiverse_c::destroyPictureReceivers() {
    for (u32 index = 0; s32(index) < s32(mPicturePanes.mCount); ++index) {
        const u32 slot = index < u32(mPicturePanes.mCount) ? index : 0;
        gptr<void>* entry = hdPartsView<gptr<void>>(gabi::ea(mPicturePanes.mData.get()) + slot * 4);
        if (!entry->get()) continue;
        hdMiiversePictureReceiver_l* receiver = hdPartsView<hdMiiversePictureReceiver_l>(gabi::ea(entry->get()));
        gabi::call_ptr<void>(receiver->mVtable->mDelete.get(), receiver, 3);
        const u32 clearSlot = index < u32(mPicturePanes.mCount) ? index : 0;
        entry = hdPartsView<gptr<void>>(gabi::ea(mPicturePanes.mData.get()) + clearSlot * 4);
        *entry = NULL;
    }
    if (mPicturePanes.mData.get()) {
        hdScreenHeap* heap = hdParts_findContainHeap(sMiiverseHeapManager.mpManager, mPicturePanes.mData.get());
        heap->free(mPicturePanes.mData.get());
        mPicturePanes.mCount = 0;
        mPicturePanes.mData = NULL;
    }
}

struct hdMiiverseNavigationPane_l {
    u8 mPanePrefix[0x40];
    be<f32> mHeight;
};
struct hdMiiverseNavigationReceiver_l {
    u8 mReceiverPrefix[0x18];
    gptr<hdMiiverseNavigationPane_l> mpPane;
};
WWHD_OFFSET(hdMiiverseNavigationReceiver_l, mpPane, 0x18);
WWHD_OFFSET(hdMiiverseNavigationPane_l, mHeight, 0x40);
struct hdMiiverseNavigationPosition_l { be<f32> mX, mY; };
static f32 sMiiverseNavigationScrollDivisor = 5.0f;
void miiverseGetNavigationPosition(hdMiiverseNavigationPosition_l*, hdMiiverseNavigationPane_l*);

/* 026A18C8 */
u32 hdMenuMiiverse_c::scrollForNavigation() {
    if (!mButtonsEnabled) return 0;
    if (mDragMode == 1 && miiverseNavigationButtonsHeld()) {
        if (!isChartOrPagerInputMode() && miiverseGetInputMode(sMiiverseMainEventMgr)) {
            const u32 buttons = sMiiverseNavigationInput->mPrimaryButtons | sMiiverseNavigationInput->mSecondaryButtons;
            if (!(buttons & 0x00220000)) return 0;
        }
        mTouchActive = 0;
        mDragActive = 1;
        mDragMode = 2;
        focusFirstVisibleControl(1);
    }
    if (!miiverseGetInputMode(sMiiverseMainEventMgr)) return 0;
    if (!miiverseNavigationButtonsHeld() && mDragMode != 2) return 0;
    mDragActive = 1;
    mDragMode = 2;
    if (!isChartOrPagerInputMode()) {
        easeScrollToLowerBound();
        return 0;
    }
    const f32 upper = messageUpperBound();
    const f32 lower = messageLowerBound();
    gabi::NativeFrame<0x50> frame;
    hdMiiverseNavigationPosition_l* position = hdPartsView<hdMiiverseNavigationPosition_l>(frame + 8);
    hdMiiverseNavigationReceiver_l* receiver = hdPartsView<hdMiiverseNavigationReceiver_l>(miiverseGetInputMode(sMiiverseMainEventMgr));
    miiverseGetNavigationPosition(position, receiver->mpPane);
    const f32 top = position->mY;
    receiver = hdPartsView<hdMiiverseNavigationReceiver_l>(miiverseGetInputMode(sMiiverseMainEventMgr));
    const f32 bottom = f32(f32(position->mY) - f32(receiver->mpPane->mHeight));
    f32 distance = sMiiverseZero;
    if (upper < top) {
        distance = f32(upper - top);
    } else if (bottom < lower) {
        distance = f32(lower - bottom);
    }
    scrollBy(f32(distance / sMiiverseNavigationScrollDivisor));
    return 1;
}

struct hdMiiverseMessageSessionTable_l {
    u8 mTablePrefix[0x24];
    gfn mRelease;
};
struct hdMiiverseMessageSession_l {
    u8 mSessionPrefix[0xC];
    gptr<hdMiiverseMessageSessionTable_l> mVtable;
};
WWHD_OFFSET(hdMiiverseMessageSession_l, mVtable, 0xC);
WWHD_OFFSET(hdMiiverseMessageSessionTable_l, mRelease, 0x24);
void miiverseReleaseSessionStorage(void*);

/* 026A4D28 */
void hdMenuMiiverse_c::finalizeMenu() {
    destroyPictureReceivers();
    vDeleteChildren();
    vDestroyLayout();
    for (u32 chart = 0; chart < 10; ++chart) {
        hdParts_c* button = hdPartsView<hdParts_c>(gabi::ea(mpChartButtons[chart].get()));
        button->vFinalize();
        button = mpChartButtons[chart];
        if (button) button->vDelete(3);
    }
    {
        hdParts_c* part = hdPartsView<hdParts_c>(gabi::ea(mpPrimaryButton.get()));
        part->vFinalize();
        part = mpPrimaryButton;
        if (part) part->vDelete(3);
    }
    {
        hdParts_c* part = hdPartsView<hdParts_c>(gabi::ea(mpPreviousButton.get()));
        part->vFinalize();
        part = mpPreviousButton;
        if (part) part->vDelete(3);
    }
    {
        hdParts_c* part = hdPartsView<hdParts_c>(gabi::ea(mpNextButton.get()));
        part->vFinalize();
        part = mpNextButton;
        if (part) part->vDelete(3);
    }
    {
        hdParts_c* part = hdPartsView<hdParts_c>(gabi::ea(mpMessageScreen.get()));
        part->vFinalize();
        part = mpMessageScreen;
        if (part) part->vDelete(3);
    }
    hdParts_c::detachArchives();
    miiverseReleaseSessionStorage(hdPartsView<void>(mSessionValue));
    hdMiiverseMessageSession_l* session = hdPartsView<hdMiiverseMessageSession_l>(mMessageValue);
    gabi::call_ptr<void>(session->mVtable->mRelease.get(), session);
}

WWHD_OFFSET(hdMiiverseArchivePicture_l, mDisplayed, 0x25A);
WWHD_OFFSET(hdMiiverseArchivePictures_l, mLastEntry, 0x4FC4);
WWHD_OFFSET(hdMiiverseArchivePictures_l, mPreviousPageDisplayed, 0x4FD6);
s32 miiverseArchiveChartCount(void*);
void miiverseRestorePreviousButton(hdParts_c*);
void miiverseAssignChartEntry(hdMiiverseChartButton_l*, u32);

/* 026A213C */
void hdMenuMiiverse_c::restorePageButtons() {
    scrollToLowerBound();
    hdMiiverseArchivePictures_l* archive = hdPartsView<hdMiiverseArchivePictures_l>(gabi::ea(sMiiverseScrollArchive.mpArchive.get()));
    if (archive->mHasPreviousPage) {
        mHasPreviousPage = 1;
        miiverseRestorePreviousButton(mpPreviousButton);
    }
    miiverseDisplayChartButton(mpPrimaryButton);
    for (u32 chart = 0; chart < 10; ++chart)
        miiverseDisplayChartButton(mpChartButtons[chart]);
    u32 entry = 0;
    if (miiverseArchiveChartCount(archive) >= 10)
        entry = u32(s32(u32(archive->mLastEntry) + 1) % 10);
    const s32 count = miiverseArchiveChartCount(archive);
    if (count > 0) {
        u32 chart = u32(count) - 1;
        for (u32 remaining = u32(count); remaining != 0; --remaining, --chart) {
            miiverseAssignChartEntry(mpChartButtons[chart < 10 ? chart : 0], entry);
            archive->mPictures[entry < 10 ? entry : 0].mDisplayed = 1;
            entry += 1;
            if (s32(entry) > 9) entry = 0;
        }
    }
    if (mHasPreviousPage) archive->mPreviousPageDisplayed = 1;
    mDialogCancelled = 0;
    refreshDuplicatePictureFlags();
}

WWHD_OFFSET(hdMiiverseArchivePicture_l, mLabel, 0x25C);
WWHD_OFFSET(hdMiiverseArchivePicture_l, mSubject, 0x260);
WWHD_OFFSET(hdMiiverseArchivePicture_l, mState, 0x264);
WWHD_OFFSET(hdMiiverseArchivePicture_l, mDataSize, 0x26C);
WWHD_OFFSET(hdMiiverseArchivePicture_l, mpJpegData, 0x270);
struct hdMiiversePictureCount_l {
    u8 mPlayStorage[0x5B66];
    be<s16> mSavedPictureCount;
};
WWHD_OFFSET(hdMiiversePictureCount_l, mSavedPictureCount, 0x5B66);
s32 miiverseFirstEmptyPicture(hdSavePicture_l*);
hdJpeg_c* miiverseMutablePicture(hdSavePicture_l*, u32);
void miiverseAppendPictureOrder(hdSavePicture_l*, u32);
void miiverseCopyJpegData(void*, const void*, u32, u32);

/* 026A3488 */
void hdMenuMiiverse_c::saveSelectedPicture() {
    const u32 selected = mSelectedChart;
    hdMiiverseArchivePictures_l* archive = hdPartsView<hdMiiverseArchivePictures_l>(gabi::ea(sMiiverseScrollArchive.mpArchive.get()));
    u32 entryIndex = mpChartButtons[selected < 10 ? selected : 0]->mArchiveIndex;
    if (entryIndex >= 10) entryIndex = 0;
    hdMiiverseArchivePicture_l* entry = &archive->mPictures[entryIndex];
    hdSavePicture_l* holder = miiverseGetPictureHolder(&sMiiverseMainSaveInfo->mHolders);
    const u32 slot = u32(miiverseFirstEmptyPicture(holder));
    hdJpeg_c* record = miiverseMutablePicture(holder, slot);
    u8* destination = record->data();
    miiverseCopyJpegData(destination, entry->mpJpegData, 0x50000, 0);
    record = miiverseMutablePicture(holder, slot);
    record->setSubject(entry->mSubject);
    record = miiverseMutablePicture(holder, slot);
    record->setLabel(entry->mLabel);
    record = miiverseMutablePicture(holder, slot);
    record->enable();
    miiverseAppendPictureOrder(holder, slot);
    record = miiverseMutablePicture(holder, slot);
    record->markSaved();
    record = miiverseMutablePicture(holder, slot);
    record->setDataSize(entry->mDataSize);
    record = miiverseMutablePicture(holder, slot);
    record->setState(entry->mState);
    record = miiverseMutablePicture(holder, slot);
    record->setChecksum(entry->mPictureId);
    hdMiiversePictureCount_l* play = hdPartsView<hdMiiversePictureCount_l>(gabi::ea(miiverseGetPlayInfo()));
    play->mSavedPictureCount = s32(play->mSavedPictureCount) + 1;
}

u32 miiverseArchiveBusy(void*);
void miiverseOpenSelectedPicture(hdMenuMiiverse_c*);

/* 0269FE94 */
void hdMenuMiiverse_c::openPendingPicture() {
    if (miiverseArchiveBusy(sMiiverseScrollArchive.mpArchive)) return;
    for (u32 chart = 0; chart < 10; ++chart) {
        if (!mpChartButtons[chart]->mOpenPending) continue;
        mSelectedChart = chart;
        miiverseOpenSelectedPicture(this);
        return;
    }
}

u32 miiverseHandleControlActions(hdMenuMiiverse_c*);

/* 026A1BD0 */
void hdMenuMiiverse_c::updateWaitingInput() {
    const u8 inputEnabled = mInputEnabled;
    mDragActive = 0;
    if (inputEnabled) {
        vCalcLayout();
        return;
    }
    openPendingPicture();
    if (selectPendingControl() || findSelectedChart()) {
        mTouchActive = 0;
        return;
    }
    if (miiverseHandleControlActions(this)) {
        hdPartsVtbl_c* table = mVtable;
        mVelocityY = sMiiverseZero;
        mTouchActive = 0;
        gabi::call_ptr<void>(table->mCalcLayout.get(), this);
        return;
    }
    if (beginTouchDrag()) {
        changeState(&sMiiverseDragState);
        mDragActive = 1;
        return;
    }
    if (!scrollForNavigation()) dampDragVelocity();
    vCalcLayout();
}

u32 miiverseControllerMode();
u32 miiverseUsesTouchController(hdMiiverseNavigationInput_l*);
void miiverseRestoreInputPresentation(hdMenuMiiverse_c*);
void miiverseUpdateChartSound(hdMiiverseChartButton_l*);
void miiverseUpdateSoundFader(void*);

/* 026A4EF8 */
void hdMenuMiiverse_c::updateControllerState() {
    bool controllerActivated = false;
    if (!mInitialized && miiverseControllerMode() == 1) controllerActivated = true;
    if (controllerActivated && !miiverseGetInputMode(sMiiverseMainEventMgr)) {
        mTouchActive = 0;
        mDragMode = 2;
        focusFirstVisibleControl(1);
    }
    bool touchActivated = false;
    if (controllerActivated && miiverseUsesTouchController(sMiiverseNavigationInput)) touchActivated = true;
    bool touchChanged = false;
    if (miiverseControllerMode() == 1 && !mContentReady && miiverseUsesTouchController(sMiiverseNavigationInput))
        touchChanged = true;
    if (touchActivated || touchChanged) {
        miiverseRestoreInputPresentation(this);
        for (u32 chart = 0; chart < 10; ++chart) {
            hdMiiverseChartButton_l* button = mpChartButtons[chart];
            if (button->mFilled) button->changeState(&sMiiverseCommentCheckInputState);
        }
        mpNextButton->changeState(&sMiiverseSettingCheckInputState);
        hdMiiversePageArchive_l* archive = hdPartsView<hdMiiversePageArchive_l>(gabi::ea(sMiiverseScrollArchive.mpArchive.get()));
        if (archive->mHasPreviousPage)
            mpPreviousButton->changeState(&sMiiverseNewCommentCheckInputState);
        miiverseResetPictureSave(sMiiversePictureSave.mpSave);
        mConfirmationStep = 0;
    }
    mContentReady = miiverseUsesTouchController(sMiiverseNavigationInput);
    mInitialized = miiverseControllerMode();
}

/* 026A54F8 */
void hdMenuMiiverse_c::executeMenu() {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(stateMachine());
    hdPartsStateId_c* hidden = &sMiiversePartsBlankState;
    hdPartsStateIdVtbl_c* hiddenTable = hidden->mVtable;
    const u32 currentKey = current->getKey();
    const u32 hiddenKey = gabi::call_ptr<u32>(hiddenTable->mGetKey.get(), hidden);
    if (currentKey == hiddenKey) return;
    updateControllerState();
    updateInputIdentity();
    executeActiveParts();
    updateSaveModeMessage();
    hdParts_executeStateMachine(stateMachine());
    refreshDraggingLayouts();
    miiverseUpdateChartSound(mpPrimaryButton);
    for (u32 chart = 0; chart < 10; ++chart)
        miiverseUpdateChartSound(mpChartButtons[chart]);
    miiverseUpdateSoundFader(mSoundStorage);
}

u32 miiverseArchivePageChanged(void*);
void miiverseLayoutChartButtons(hdMenuMiiverse_c*, u32);
void miiverseSetNextButtonPosition(hdParts_c*, f32);
void miiversePrepareChartInput(hdMiiverseChartButton_l*);
void miiversePrepareNextInput(hdParts_c*);
static f32 sMiiverseEmptyNextPosition = -420.0f;

/* 026A22C4 */
void hdMenuMiiverse_c::restoreInputPresentation() {
    hdMiiversePageArchive_l* archive = hdPartsView<hdMiiversePageArchive_l>(gabi::ea(sMiiverseScrollArchive.mpArchive.get()));
    mpPreviousButton->changeState(&sMiiverseNewCommentWaitState);
    if (miiverseArchivePageChanged(archive) || archive->mHasPreviousPage || mClosingRequested) {
        mClosingRequested = 0;
        restorePageButtons();
        miiverseLayoutChartButtons(this, 0);
        prepareWindowScale();
        updateWindowScale(1);
        miiverseResetPictureSave(sMiiversePictureSave.mpSave);
        miiverseFinishSelectedChart(mpChartButtons[0]);
    }
    if (s32(filledChartCount()) > 0) {
        hdMiiverseLayoutPane_l* text = hdPartsView<hdMiiverseLayoutPane_l>(gabi::ea(mpTextPane.get()));
        text->mTransformFlags = u8(text->mTransformFlags) & 0xFE;
        return;
    }
    hdMiiversePager_l* next = static_cast<hdMiiversePager_l*>(mpNextButton.get());
    next->mPositionY = sMiiverseEmptyNextPosition;
    next->changeState(&sMiiverseSettingSlideDownState);
    next = static_cast<hdMiiversePager_l*>(mpNextButton.get());
    miiverseSetNextButtonPosition(next, next->mPositionY);
    miiverseFinishAnimation(mpLayout->animSet(), 2, 1);
    hdMiiverseLayoutPane_l* text = hdPartsView<hdMiiverseLayoutPane_l>(gabi::ea(mpTextPane.get()));
    text->mTransformFlags = (u8(text->mTransformFlags) & 0xFE) + 1;
    prepareWindowScale();
    updateWindowScale(1);
}

/* 026A241C */
void hdMenuMiiverse_c::initializeDragState() {
    mDialogCancelled = 0;
    clearSelectionFlags();
    miiverseGetPlayInfo()->mActionButtonA = 0x17;
    miiverseGetPlayInfo()->mActionButtonB = 7;
    mDragActive = 1;
    restoreInputPresentation();
    for (u32 chart = 0; chart < 10; ++chart)
        miiversePrepareChartInput(mpChartButtons[chart]);
    miiversePrepareNextInput(mpNextButton);
    for (u32 chart = 0; chart < 10; ++chart) {
        hdParts_c* button = mpChartButtons[chart];
        button->vExecuteChildren();
        button->vCalcLayout();
    }
    hdParts_c* next = mpNextButton;
    next->vExecuteChildren();
    next->vCalcLayout();
    vCalcLayout();
    mAnimationFrame = 0;
    mDragMode = 1;
}

WWHD_OFFSET(hdMiiversePictureReceiver_l, mEnabled, 0x58);
static hdUiSafeString sMiiversePictureInputName;
void* miiverseConstructPictureReceiver(void*, hdPartsPane_c*, u32);

/* 026A4354 */
u32 hdMenuMiiverse_c::createPictureReceivers() {
    hdScreenHeap* ownerHeap = sMiiverseUiSystem.mpSystem->mHeap;
    hdScreenHeap* arrayHeap = ownerHeap;
    if (!arrayHeap) arrayHeap = hdParts_getCurrentHeap(sMiiverseHeapManager.mpManager);
    void* storage = arrayHeap->allocate(4);
    if (storage) {
        mPicturePanes.mData = static_cast<gptr<void>*>(storage);
        mPicturePanes.mCount = 1;
    }
    const s32 clearCount = s32(mPicturePanes.mCount);
    if (clearCount > 0) {
        for (u32 index = 0; index < u32(clearCount); ++index) {
            gptr<void>* slot = hdPartsView<gptr<void>>(gabi::ea(mPicturePanes.mData.get()) + index * 4);
            *slot = NULL;
        }
    }
    for (u32 index = 0; s32(index) < s32(mPicturePanes.mCount); ++index) {
        hdPartsLyt_c* layout = mpLayout->lyt();
        hdUiSafeString* name = hdPartsView<hdUiSafeString>(gabi::ea(&sMiiversePictureInputName) + index * 8);
        gabi::call_ptr<void>(sMiiversePictureInputName.mVtable->mAssureTermination.get(), name);
        hdPartsPane_c* root = layout->mpRootPane;
        hdScreenResourceTable* table = hdPartsView<hdScreenResourceTable>(root->mVtable.v);
        hdPartsPane_c* pane = gabi::call_ptr<hdPartsPane_c*>(table->mFindPane.get(), root, sMiiversePictureInputName.mText.get(), 1);
        if (!sMiiversePictureTypeReady) {
            sMiiversePictureTypeReady = 1;
            if (!sMiiversePictureBaseTypeReady) {
                sMiiversePictureBaseTypeReady = 1;
                hdPartsTypeInfo_c* rootType = &sMiiverseRootType;
                if (!sMiiverseRootTypeReady) {
                    rootType->mParent = nullptr;
                    sMiiverseRootTypeReady = 1;
                }
                sMiiversePictureBaseType.mParent = rootType;
            }
            sMiiversePictureType.mParent = &sMiiversePictureBaseType;
        }
        if (pane) {
            hdPartsTypeInfo_c* type = pane->getRuntimeTypeInfo();
            while (type && type != &sMiiversePictureType) type = type->mParent;
            if (!type) pane = nullptr;
        }
        const u32 slotIndex = index < u32(mPicturePanes.mCount) ? index : 0;
        gptr<void>* slot = hdPartsView<gptr<void>>(gabi::ea(mPicturePanes.mData.get()) + slotIndex * 4);
        void* receiver = hdParts_new(0x60, ownerHeap, 4);
        if (receiver) receiver = miiverseConstructPictureReceiver(receiver, pane, 0);
        *slot = receiver;
        const u32 checkIndex = index < u32(mPicturePanes.mCount) ? index : 0;
        slot = hdPartsView<gptr<void>>(gabi::ea(mPicturePanes.mData.get()) + checkIndex * 4);
        if (!slot->get()) return 0;
    }
    gptr<void>* first = hdPartsView<gptr<void>>(gabi::ea(mPicturePanes.mData.get()));
    hdMiiversePictureReceiver_l* receiver = hdPartsView<hdMiiversePictureReceiver_l>(gabi::ea(first->get()));
    receiver->mEnabled = 1;
    return 1;
}

void miiverseSetPrimaryTarget(hdMiiverseChartButton_l*, f32);
f32 miiverseChartHeight(hdMiiverseChartButton_l*);
u32 miiverseChartEndsGroup(hdMiiverseChartButton_l*);
f32 miiverseChartGroupSpacing(void*);
void miiverseApplyChartPosition(hdMiiverseChartButton_l*, f32);

/* 026A04A0 */
void hdMenuMiiverse_c::layoutChartButtons(u32 deferPosition) {
    const f32 nextLimit = sMiiverseEmptyNextPosition;
    hdMiiversePager_l* next = static_cast<hdMiiversePager_l*>(mpNextButton.get());
    next->mPositionY = nextLimit;
    next->changeState(&sMiiverseSettingSlideDownState);
    if (!deferPosition) {
        next = static_cast<hdMiiversePager_l*>(mpNextButton.get());
        miiverseSetNextButtonPosition(next, next->mPositionY);
    }
    if (s32(filledChartCount()) <= 0 && !mClosingRequested && !mHasPreviousPage) return;
    void* archive = sMiiverseScrollArchive.mpArchive;
    f32 position = mScrollFraction;
    if (mHasPreviousPage) {
        const f32 height = miiversePreviousButtonHeight(mpPreviousButton);
        position = f32(position - f32(height + sMiiverseMinScrollStep));
    }
    if (mClosingRequested && deferPosition) {
        miiverseSetPrimaryTarget(mpPrimaryButton, position);
        const f32 height = miiverseChartHeight(mpPrimaryButton);
        const f32 spacing = miiverseChartSpacing(archive);
        position = f32(position - f32(height + spacing));
        if (u8(miiverseChartEndsGroup(mpPrimaryButton)) != 1)
            position = f32(position - miiverseChartGroupSpacing(archive));
    }
    if (s32(filledChartCount()) <= 0) return;
    const u32 chartCount = mClosingRequested ? 9 : 10;
    hdPartsStateId_c* moveState = &sMiiverseCommentSlideDownState;
    for (u32 chart = 0; chart < chartCount; ++chart) {
        hdMiiverseChartButton_l* button = mpChartButtons[chart];
        if (!button->mFilled) continue;
        button->mTargetY = position;
        hdPartsStateId_c* current = hdParts_getCurrentStateId(button->stateMachine());
        hdPartsStateIdVtbl_c* moveTable = moveState->mVtable;
        const u32 currentKey = current->getKey();
        const u32 moveKey = gabi::call_ptr<u32>(moveTable->mGetKey.get(), moveState);
        if (currentKey != moveKey) button->changeState(moveState);
        const f32 height = miiverseChartHeight(mpChartButtons[chart]);
        const f32 spacing = miiverseChartSpacing(archive);
        position = f32(position - f32(height + spacing));
        if (u8(miiverseChartEndsGroup(mpChartButtons[chart])) != 1)
            position = f32(position - miiverseChartGroupSpacing(archive));
    }
    f32 nextPosition = f32(position - miiverseChartSpacing(archive));
    if (nextPosition > nextLimit) nextPosition = nextLimit;
    next = static_cast<hdMiiversePager_l*>(mpNextButton.get());
    next->mPositionY = nextPosition;
    next->changeState(&sMiiverseSettingSlideDownState);
    if (!deferPosition) {
        for (u32 chart = 0; chart < 10; ++chart) {
            hdMiiverseChartButton_l* button = mpChartButtons[chart];
            if (button->mFilled) miiverseApplyChartPosition(button, button->mTargetY);
        }
        next = static_cast<hdMiiversePager_l*>(mpNextButton.get());
        miiverseSetNextButtonPosition(next, next->mPositionY);
    }
}

struct hdMiiversePhotoPosition_l { be<f32> mX, mY, mZ; };
struct hdMiiverseConfirmationScreen_l : hdParts_c {
    u8 mDialogStorage[0x30];
    hdMiiversePhotoPosition_l mPosition;
    u8 mChoiceStorage[0x28];
    be<u8> mAllowSave;
};
WWHD_OFFSET(hdMiiverseConfirmationScreen_l, mPosition, 0x80);
WWHD_OFFSET(hdMiiverseConfirmationScreen_l, mAllowSave, 0xB4);
struct hdMiiverseMessageCoreSlot_l { gptr<void> mpCore; };
static hdMiiverseMessageCoreSlot_l sMiiverseMessageCore;
static u8 sMiiverseDialogMode;
void miiverseGetChartPosition(hdMiiverseChartButton_l*, hdMiiversePhotoPosition_l*);
u32 miiverseHasCameraItem(u32);
void miiverseSetDialogMessage(void*, hdUiSafeString*, hdUiSafeString*, u32, u32);
void miiversePlayPhotoSound(const char*);

/* 026A3158 */
void hdMenuMiiverse_c::initializePictureConfirmation() {
    gabi::NativeFrame<0x50> frame;
    mSavedInputMode = sMiiverseGlobalInputMode != 0;
    const u32 selected = mSelectedChart;
    hdMiiversePhotoPosition_l* position = hdPartsView<hdMiiversePhotoPosition_l>(frame + 0x28);
    miiverseGetChartPosition(mpChartButtons[selected < 10 ? selected : 0], position);
    dOwPlayView_c* play = miiverseGetPlayInfo();
    const u8 buttonB = play->mActionButtonB;
    const u8 buttonR = play->mActionButtonR;
    play->mSavedActionButtonB = buttonB;
    const u8 buttonA = play->mActionButtonA;
    play->mSavedActionButtonR = buttonR;
    play->mSavedActionButtonA = buttonA;
    sMiiverseGlobalInputMode = 1;
    miiverseGetPlayInfo()->mActionButtonA = 0x17;
    miiverseGetPlayInfo()->mActionButtonB = 7;
    if (!miiverseHasCameraItem(0x23) && !miiverseHasCameraItem(0x26)) {
        miiverseGetPlayInfo()->mActionButtonB = 7;
        void* core = sMiiverseMessageCore.mpCore;
        hdUiSafeString* layout = hdPartsView<hdUiSafeString>(frame + 8);
        hdUiSafeString* message = hdPartsView<hdUiSafeString>(frame + 0x10);
        layout->mText.v = gabi::guest_addr("MsgOnly_00");
        layout->mVtable = &sMiiverseStringVtable;
        message->mText.v = gabi::guest_addr("T_Msg_SaveError_01");
        message->mVtable = &sMiiverseStringVtable;
        sMiiverseDialogMode = 2;
        miiverseSetDialogMessage(core, layout, message, 2, 0);
        hdMiiverseConfirmationScreen_l* dialog = static_cast<hdMiiverseConfirmationScreen_l*>(mpMessageScreen.get());
        const f32 z = position->mZ;
        mMessageKind = 0;
        const f32 y = position->mY;
        const f32 x = position->mX;
        dialog->mPosition.mY = y;
        dialog->mPosition.mZ = z;
        dialog->mPosition.mX = x;
        dialog->changeState(&sMiiverseBoxInState);
    } else {
        hdSavePicture_l* holder = miiverseGetPictureHolder(&sMiiverseMainSaveInfo->mHolders);
        dPictureStoreHD_c* pictures = hdPartsView<dPictureStoreHD_c>(gabi::ea(holder));
        const u32 selectedSlot = pictures->mSelectedPhotoSlot;
        hdMiiversePictureCount_l* count = hdPartsView<hdMiiversePictureCount_l>(gabi::ea(miiverseGetPlayInfo()));
        const s32 total = s32(u32(selectedSlot + s32(count->mSavedPictureCount)));
        play = miiverseGetPlayInfo();
        if (total < 12) {
            play->mActionButtonA = 0x17;
            miiverseGetPlayInfo()->mActionButtonB = 7;
            sMiiverseDialogMode = 3;
            hdMiiverseConfirmationScreen_l* dialog = static_cast<hdMiiverseConfirmationScreen_l*>(mpMessageScreen.get());
            dialog->mAllowSave = 1;
            const f32 x = position->mX;
            dialog = static_cast<hdMiiverseConfirmationScreen_l*>(mpMessageScreen.get());
            const f32 z = position->mZ;
            const f32 y = position->mY;
            mMessageKind = 1;
            dialog->mPosition.mY = y;
            dialog->mPosition.mX = x;
            dialog->mPosition.mZ = z;
            dialog->changeState(&sMiiverseBoxInState);
        } else {
            play->mActionButtonB = 7;
            void* core = sMiiverseMessageCore.mpCore;
            hdUiSafeString* layout = hdPartsView<hdUiSafeString>(frame + 0x18);
            hdUiSafeString* message = hdPartsView<hdUiSafeString>(frame + 0x20);
            message->mVtable = &sMiiverseStringVtable;
            sMiiverseDialogMode = 2;
            layout->mText.v = gabi::guest_addr("MsgOnly_00");
            layout->mVtable = &sMiiverseStringVtable;
            message->mText.v = gabi::guest_addr("T_Msg_SaveError");
            miiverseSetDialogMessage(core, layout, message, 2, 0);
            hdMiiverseConfirmationScreen_l* dialog = static_cast<hdMiiverseConfirmationScreen_l*>(mpMessageScreen.get());
            mMessageKind = 2;
            const f32 z = position->mZ;
            const f32 y = position->mY;
            const f32 x = position->mX;
            dialog->mPosition.mZ = z;
            dialog->mPosition.mX = x;
            dialog->mPosition.mY = y;
            dialog->changeState(&sMiiverseBoxInState);
        }
    }
    miiversePlayPhotoSound("SE_MIIVERSE_PHOTO_OPEN");
    for (u32 chart = 0; chart < 10; ++chart)
        mpChartButtons[chart]->changeState(&sMiiverseCommentWaitState);
    mDragActive = 0;
    clearSelectionFlags();
    mConfirmationStep = 0;
}

struct hdMiiverseDialogInput_l {
    u8 mInputStorage[0x30];
    be<u32> mChoice;
};
WWHD_OFFSET(hdMiiverseDialogInput_l, mChoice, 0x30);
u32 miiverseFindDialog(void*);
void* miiverseGetPictureSave(hdSaveHolders_c*);
void miiverseBeginPictureSave(void*, u32);

/* 026A36AC */
void hdMenuMiiverse_c::updatePictureConfirmation() {
    switch (u32(mConfirmationStep)) {
    case 0: {
        hdPartsStateId_c* current = hdParts_getCurrentStateId(mpMessageScreen->stateMachine());
        hdPartsStateId_c* opened = &sMiiverseBoxDisplayState;
        hdPartsStateIdVtbl_c* openedTable = opened->mVtable;
        const u32 currentKey = current->getKey();
        const u32 openedKey = gabi::call_ptr<u32>(openedTable->mGetKey.get(), opened);
        if (currentKey != openedKey) return;
        if (mMessageKind == 2 || mMessageKind == 0) {
            mConfirmationStep = 1;
        } else {
            hdMiiverseConfirmationScreen_l* dialog = static_cast<hdMiiverseConfirmationScreen_l*>(mpMessageScreen.get());
            dialog->mAllowSave = 1;
            dialog->changeState(&sMiiverseBoxSaveState);
            mConfirmationStep = 2;
        }
        return;
    }
    case 1: {
        const u32 dialog = miiverseFindDialog(sMiiverseMessageCore.mpCore);
        if ((mMessageKind == 2 || mMessageKind == 0) && dialog == 0)
            mConfirmationStep = 4;
        return;
    }
    case 2: {
        if ((sMiiverseNavigationInput->mPrimaryButtons | sMiiverseNavigationInput->mSecondaryButtons) & 2) {
            mConfirmationStep = 4;
            return;
        }
        hdMiiverseDialogInput_l* input = hdPartsView<hdMiiverseDialogInput_l>(gabi::ea(sMiiverseMainEventMgr));
        const u32 choice = input->mChoice;
        if (choice == 0x6B) {
            saveSelectedPicture();
            const u32 selected = mSelectedChart;
            miiverseSetChartAvailable(mpChartButtons[selected < 10 ? selected : 0], 0);
            void* save = miiverseGetPictureSave(&sMiiverseMainSaveInfo->mHolders);
            miiverseBeginPictureSave(save, 1);
            mConfirmationStep = 4;
        } else if (choice == 0x6C) {
            mConfirmationStep = 4;
        }
        return;
    }
    case 3: {
        hdMiiverseNavigationInput_l* input = sMiiverseNavigationInput;
        if ((input->mpDevice->mHeldButtons & 0x8000) ||
            ((input->mPrimaryButtons | input->mSecondaryButtons) & 3))
            mConfirmationStep = 4;
        return;
    }
    case 4:
        mpMessageScreen->changeState(&sMiiverseBoxOutState);
        miiversePlayPhotoSound("SE_MIIVERSE_PHOTO_CLOSE");
        mConfirmationStep = 5;
        return;
    case 5: {
        hdPartsStateId_c* current = hdParts_getCurrentStateId(mpMessageScreen->stateMachine());
        hdPartsStateId_c* closed = &sMiiverseBoxWaitState;
        hdPartsStateIdVtbl_c* closedTable = closed->mVtable;
        const u32 currentKey = current->getKey();
        const u32 closedKey = gabi::call_ptr<u32>(closedTable->mGetKey.get(), closed);
        if (currentKey != closedKey) return;
        setChartConfirmationStates();
        changeState(&sMiiverseCheckInputState);
        return;
    }
    default:
        return;
    }
}

void miiverseRemovePreviousLink(hdParts_c*, u32);
void miiverseSetPreviousBackLink(hdParts_c*, u32);
void miiverseAddPreviousLink(hdParts_c*, hdParts_c*);
void miiverseSetPreviousForwardLink(hdParts_c*, hdParts_c*);
void miiverseSetChartUp(hdMiiverseChartButton_l*, u32);
void miiverseSetChartDown(hdMiiverseChartButton_l*, u32);
void miiverseSetChartPhoto(hdMiiverseChartButton_l*, u32);
void miiverseRemoveChartUp(hdMiiverseChartButton_l*, u32);
void miiverseRemoveChartDown(hdMiiverseChartButton_l*, u32);
void miiverseRemoveChartPhoto(hdMiiverseChartButton_l*, u32);

/* 026A5754 */
void hdMenuMiiverse_c::updateFirstNavigationLinks(u32 up, u32 down, u32 photo) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(stateMachine());
    hdPartsStateId_c* hidden = &sMiiverseInAllState;
    hdPartsStateIdVtbl_c* wantedTable = hidden->mVtable;
    const u32 currentKey = current->getKey();
    if (currentKey == gabi::call_ptr<u32>(wantedTable->mGetKey.get(), hidden)) return;
    hdPartsStateId_c* chartMoving = &sMiiverseCommentInState;
    for (u32 index = 0; index < 10; ++index) {
        wantedTable = chartMoving->mVtable;
        current = hdParts_getCurrentStateId(mpChartButtons[index]->stateMachine());
        const u32 key = current->getKey();
        if (key == gabi::call_ptr<u32>(wantedTable->mGetKey.get(), chartMoving)) return;
    }
    current = hdParts_getCurrentStateId(mpPreviousButton->stateMachine());
    hdPartsStateId_c* previousMoving = &sMiiverseNewCommentInOutState;
    wantedTable = previousMoving->mVtable;
    const u32 previousKey = current->getKey();
    if (previousKey == gabi::call_ptr<u32>(wantedTable->mGetKey.get(), previousMoving)) return;
    current = hdParts_getCurrentStateId(mpNextButton->stateMachine());
    hdPartsStateId_c* nextMoving = &sMiiverseSettingInState;
    wantedTable = nextMoving->mVtable;
    const u32 nextKey = current->getKey();
    if (nextKey == gabi::call_ptr<u32>(wantedTable->mGetKey.get(), nextMoving)) return;
    hdMiiversePageArchive_l* archive = hdPartsView<hdMiiversePageArchive_l>(gabi::ea(sMiiverseScrollArchive.mpArchive.get()));
    const s32 count = miiverseArchiveChartCount(archive);
    const bool hasCharts = count > 0;
    if (archive->mHasPreviousPage) {
        miiverseRemovePreviousLink(mpPreviousButton, up);
        miiverseRemovePreviousLink(mpPreviousButton, down);
        miiverseRemovePreviousLink(mpPreviousButton, photo);
        miiverseSetPreviousBackLink(mpPreviousButton, photo);
        if (!hasCharts || !mpChartButtons[0]->mFilled) return;
        hdParts_c* previous = mpPreviousButton;
        hdMiiverseChartButton_l* first = mpChartButtons[0];
        hdParts_c* child = first->mChildren.mpData.get()[0];
        miiverseAddPreviousLink(previous, child);
        miiverseSetPreviousForwardLink(previous, child);
        first = mpChartButtons[0];
        miiverseAddPreviousLink(mpPreviousButton, *first->mChildren.safeSlot(1));
        if (miiverseChartHasPicture(mpChartButtons[0])) {
            first = mpChartButtons[0];
            miiverseAddPreviousLink(mpPreviousButton, *first->mChildren.safeSlot(2));
        }
    } else if (hasCharts && mpChartButtons[0]->mFilled) {
        miiverseSetChartUp(mpChartButtons[0], up);
        miiverseSetChartDown(mpChartButtons[0], down);
        miiverseSetChartPhoto(mpChartButtons[0], photo);
        miiverseRemoveChartUp(mpChartButtons[0], up);
        miiverseRemoveChartDown(mpChartButtons[0], down);
        miiverseRemoveChartPhoto(mpChartButtons[0], photo);
    }
}

void miiversePrepareChartNeighbors(hdMiiverseChartButton_l*);
void miiverseClearChartNeighbors(hdMiiverseChartButton_l*);
hdParts_c* miiverseChartReceiver(hdMiiverseChartButton_l*);
void miiverseSetChartNeighbors(hdMiiverseChartButton_l*, hdParts_c*, hdParts_c*);

/* 026A5AAC */
void hdMenuMiiverse_c::updateChartNavigationLinks() {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(stateMachine());
    hdPartsStateId_c* hidden = &sMiiverseInAllState;
    hdPartsStateIdVtbl_c* wantedTable = hidden->mVtable;
    const u32 currentKey = current->getKey();
    if (currentKey == gabi::call_ptr<u32>(wantedTable->mGetKey.get(), hidden)) return;
    hdPartsStateId_c* chartMoving = &sMiiverseCommentInState;
    for (u32 index = 0; index < 10; ++index) {
        wantedTable = chartMoving->mVtable;
        current = hdParts_getCurrentStateId(mpChartButtons[index]->stateMachine());
        const u32 key = current->getKey();
        if (key == gabi::call_ptr<u32>(wantedTable->mGetKey.get(), chartMoving)) return;
    }
    current = hdParts_getCurrentStateId(mpPreviousButton->stateMachine());
    hdPartsStateId_c* previousMoving = &sMiiverseNewCommentInOutState;
    wantedTable = previousMoving->mVtable;
    const u32 previousKey = current->getKey();
    if (previousKey == gabi::call_ptr<u32>(wantedTable->mGetKey.get(), previousMoving)) return;
    current = hdParts_getCurrentStateId(mpNextButton->stateMachine());
    hdPartsStateId_c* nextMoving = &sMiiverseSettingInState;
    wantedTable = nextMoving->mVtable;
    const u32 nextKey = current->getKey();
    if (nextKey == gabi::call_ptr<u32>(wantedTable->mGetKey.get(), nextMoving)) return;
    if (s32(filledChartCount()) <= 0) return;
    for (u32 index = 0; index < 10; ++index) {
        if (mpChartButtons[index]->mFilled) {
            miiversePrepareChartNeighbors(mpChartButtons[index]);
            miiverseClearChartNeighbors(mpChartButtons[index]);
        }
    }
    u32 index = 0;
    s32 previous = -1;
    for (s32 processed = 0; processed < s32(filledChartCount()); ++processed) {
        hdParts_c* before = nullptr;
        hdParts_c* after = nullptr;
        const s32 next = s32(index + 1);
        if (previous >= 0) {
            const u32 slot = u32(previous) < 10 ? u32(previous) : 0;
            if (mpChartButtons[slot]->mFilled)
                before = miiverseChartReceiver(mpChartButtons[slot]);
        }
        if (index != lastFilledChart() && next < 10) {
            const u32 slot = u32(next) < 10 ? u32(next) : 0;
            if (mpChartButtons[slot]->mFilled)
                after = miiverseChartReceiver(mpChartButtons[slot]);
        }
        miiverseSetChartNeighbors(mpChartButtons[index < 10 ? index : 0], before, after);
        ++index;
        ++previous;
    }
}

void miiverseAddNextLink(hdParts_c*, hdParts_c*);
void miiverseSetNextBackLink(hdParts_c*, hdParts_c*);
static inline void miiverseAddNextControlLink(hdParts_c* next, u32 receiver) {
    miiverseAddNextLink(next, hdPartsView<hdParts_c>(receiver));
}
static inline void miiverseSetNextControlBackLink(hdParts_c* next, u32 receiver) {
    miiverseSetNextBackLink(next, hdPartsView<hdParts_c>(receiver));
}

/* 026A5E20 */
void hdMenuMiiverse_c::updateLastNavigationLinks(u32 up, u32 down, u32 photo) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(stateMachine());
    hdPartsStateId_c* hidden = &sMiiverseInAllState;
    hdPartsStateIdVtbl_c* wantedTable = hidden->mVtable;
    const u32 currentKey = current->getKey();
    if (currentKey == gabi::call_ptr<u32>(wantedTable->mGetKey.get(), hidden)) return;
    hdPartsStateId_c* chartMoving = &sMiiverseCommentInState;
    for (u32 index = 0; index < 10; ++index) {
        wantedTable = chartMoving->mVtable;
        current = hdParts_getCurrentStateId(mpChartButtons[index]->stateMachine());
        const u32 key = current->getKey();
        if (key == gabi::call_ptr<u32>(wantedTable->mGetKey.get(), chartMoving)) return;
    }
    current = hdParts_getCurrentStateId(mpPreviousButton->stateMachine());
    hdPartsStateId_c* previousMoving = &sMiiverseNewCommentInOutState;
    wantedTable = previousMoving->mVtable;
    const u32 previousKey = current->getKey();
    if (previousKey == gabi::call_ptr<u32>(wantedTable->mGetKey.get(), previousMoving)) return;
    current = hdParts_getCurrentStateId(mpNextButton->stateMachine());
    hdPartsStateId_c* nextMoving = &sMiiverseSettingInState;
    wantedTable = nextMoving->mVtable;
    const u32 nextKey = current->getKey();
    if (nextKey == gabi::call_ptr<u32>(wantedTable->mGetKey.get(), nextMoving)) return;
    hdMiiversePageArchive_l* archive = hdPartsView<hdMiiversePageArchive_l>(gabi::ea(sMiiverseScrollArchive.mpArchive.get()));
    if (miiverseArchiveChartCount(archive) > 0) {
        const u32 last = lastFilledChart();
        const u32 slot = last < 10 ? last : 0;
        hdParts_c* next = mpNextButton;
        hdParts_c* receiver = miiverseChartReceiver(mpChartButtons[slot]);
        miiverseSetNextBackLink(next, receiver);
        miiverseAddNextLink(next, receiver);
        hdMiiverseChartButton_l* chart = mpChartButtons[slot];
        miiverseAddNextLink(mpNextButton, *chart->mChildren.safeSlot(1));
        if (!miiverseChartHasPicture(mpChartButtons[slot])) return;
        chart = mpChartButtons[slot];
        miiverseAddNextLink(mpNextButton, *chart->mChildren.safeSlot(2));
    } else if (archive->mHasPreviousPage) {
        hdParts_c* previous = mpPreviousButton;
        hdParts_c* receiver = previous->mChildren.mpData.get()[0];
        miiverseAddNextLink(mpNextButton, receiver);
        previous = mpPreviousButton;
        receiver = previous->mChildren.mpData.get()[0];
        miiverseSetNextBackLink(mpNextButton, receiver);
    } else {
        miiverseAddNextControlLink(mpNextButton, up);
        miiverseAddNextControlLink(mpNextButton, down);
        miiverseAddNextControlLink(mpNextButton, photo);
        miiverseSetNextControlBackLink(mpNextButton, photo);
    }
}

struct hdMiiverseInitWords_l { be<u32> mWords[4]; };
struct hdMiiverseInitSettings_l {
    be<f32> mMinimumAngle, mMaximumAngle, mNearDistance, mFarDistance, mViewDistance, mClipDistance;
    be<u8> mMathInitializer, mRandomInitializer;
};
static hdMiiverseInitWords_l sMiiverseInitWords;
static hdMiiverseInitSettings_l sMiiverseInitSettings;
static u32 sMiiverseInitRegistration[3] = {};
static u32 sMiiverseMathRegistration[3] = {};
static u32 sMiiverseRandomRegistration[3] = {};
static hdUiSafeString sMiiverseLayoutName;
static hdUiSafeString sMiiverseMenuName;
static u32 sMiiverseStateCounterReady;
static hdPartsStateCounter_c sMiiverseStateCounter;
void miiverseInitializeMath(void*);
void miiverseInitializeRandom(void*);

static inline void miiverseSetupDirectState(hdPartsStateId_c* state, s32 number,
        const char* name, u32 enter, u32 execute, u32 exit, u32 destroy) {
    state->mNumber = number;
    state->mName.v = gabi::guest_addr(name);
    state->mVtable = &sMiiverseDirectStateVtable;
    state->mEnter.mThisDelta = 0;
    state->mEnter.mVirtualIndex = -1;
    state->mEnter.mFunction = enter;
    state->mExecute.mThisDelta = 0;
    state->mExecute.mVirtualIndex = -1;
    state->mExecute.mFunction = execute;
    state->mExit.mThisDelta = 0;
    state->mExit.mVirtualIndex = -1;
    state->mExit.mFunction = exit;
    state->mUnused.mThisDelta = 0;
    state->mUnused.mVirtualIndex = -1;
    state->mUnused.mFunction = destroy;
}

/* 026A63EC */
static void initializeMiiverseMenuStatics() {
    for (s32 word = 3; word >= 0; --word) sMiiverseInitWords.mWords[word] = 0;
    miiverseRegisterStringNames(sMiiverseInitRegistration);
    sMiiverseInitSettings.mMinimumAngle = -3.1415927410125732f;
    sMiiverseInitSettings.mMaximumAngle = 3.1415927410125732f;
    miiverseInitializeMath(&sMiiverseInitSettings.mMathInitializer);
    miiverseRegisterStringNames(sMiiverseMathRegistration);
    miiverseInitializeRandom(&sMiiverseInitSettings.mRandomInitializer);
    miiverseRegisterStringNames(sMiiverseRandomRegistration);
    sMiiverseInitSettings.mFarDistance = 50000.0f;
    sMiiverseInitSettings.mViewDistance = 10000.0f;
    sMiiverseInitSettings.mNearDistance = 50000.0f;
    sMiiverseInitSettings.mClipDistance = 10000.0f;
    sMiiverseGroupNames[0].mVtable = &sMiiverseStringVtable;
    sMiiverseGroupNames[0].mText.v = gabi::guest_addr("InAll");
    sMiiverseGroupNames[1].mVtable = &sMiiverseStringVtable;
    sMiiverseGroupNames[1].mText.v = gabi::guest_addr("OutAll");
    sMiiverseGroupNames[2].mVtable = &sMiiverseStringVtable;
    sMiiverseGroupNames[2].mText.v = gabi::guest_addr("OutMsg");
    sMiiverseAnimationNames[0].mVtable = &sMiiverseStringVtable;
    sMiiverseAnimationNames[0].mText.v = gabi::guest_addr("G_All_00");
    sMiiverseAnimationNames[1].mVtable = &sMiiverseStringVtable;
    sMiiverseAnimationNames[1].mText.v = gabi::guest_addr("G_Msg_00");
    sMiiverseLayoutName.mVtable = &sMiiverseStringVtable;
    sMiiverseLayoutName.mText.v = gabi::guest_addr("MenuMiiverse_00");
    sMiiverseMenuName.mVtable = &sMiiverseStringVtable;
    sMiiverseMenuName.mText.v = gabi::guest_addr("MenuMiiverse_00");
    sMiiverseMessageNames[0].mVtable = &sMiiverseStringVtable;
    sMiiverseMessageNames[0].mText.v = gabi::guest_addr("T_Msg_00_03");
    sMiiverseMessageNames[1].mVtable = &sMiiverseStringVtable;
    sMiiverseMessageNames[1].mText.v = gabi::guest_addr("T_Msg_00_02");
    sMiiverseMessageNames[2].mVtable = &sMiiverseStringVtable;
    sMiiverseMessageNames[2].mText.v = gabi::guest_addr("T_Msg_00_01");
    sMiiversePictureInputName.mVtable = &sMiiverseStringVtable;
    sMiiversePictureInputName.mText.v = gabi::guest_addr("B_Slide_00");
    u32 number = 0;
    if (sMiiverseStateCounterReady) number = sMiiverseStateCounter.mValue;
    else sMiiverseStateCounterReady = 1;
    miiverseSetupDirectState(&sMiiverseCheckInputState, s32(++number),
        "::StateID_CheckInput", 0x0269FBBC, 0x026A1BD0, 0x026A1D58, 0x026A6800);
    miiverseSetupDirectState(&sMiiverseDragState, s32(++number),
        "::StateID_Drag", 0x026A1D64, 0x026A1EC8, 0x026A20F4, 0x026A6808);
    hdPartsStateId_c* in = &sMiiverseInAllState;
    in->setup(s32(++number), "::StateID_InAll", &sMiiverseStateVtable,
        0x19, &sMiiverseNullState);
    hdPartsStateId_c* out = &sMiiverseOutAllState;
    out->setup(s32(++number), "::StateID_OutAll", &sMiiverseStateVtable,
        0x1C, &sMiiverseNullState);
    sMiiverseStateCounter.mValue = ++number;
    miiverseSetupDirectState(&sMiiversePhotoViewState, s32(number),
        "::StateID_PhotoView", 0x026A3158, 0x026A36AC, 0x026A39CC, 0x026A6810);
}

/* GX2 descriptors are typed guest views; their library operations remain address calls. */
struct hdMiiverseSurface_l {
    be<u32> mDimension, mWidth, mHeight, mDepth, mMipLevels, mFormat, mAntialias, mUsage;
    be<u32> mImageSize;
    gptr<void> mpImage;
    be<u32> mMipSize;
    gptr<void> mpMipImage;
    be<u32> mTileMode, mSwizzle, mAlignment, mPitch;
    be<u32> mMipOffsets[13];
};
WWHD_SIZE(hdMiiverseSurface_l, 0x74);
WWHD_OFFSET(hdMiiverseSurface_l, mImageSize, 0x20);
WWHD_OFFSET(hdMiiverseSurface_l, mpImage, 0x24);
WWHD_OFFSET(hdMiiverseSurface_l, mAlignment, 0x38);
struct hdMiiverseTexture_l {
    hdMiiverseSurface_l mSurface;
    be<u32> mFirstMip, mMipCount, mFirstSlice, mSliceCount, mComponentSelection;
    be<u32> mRegisters[5];
};
WWHD_SIZE(hdMiiverseTexture_l, 0x9C);
struct hdMiiverseJpegDimensions_l { be<u32> mWidth, mHeight; };
struct hdMiiverseTextureDefaults_l { be<u32> mWords[54]; };
static hdMiiverseTextureDefaults_l sMiiverseTextureDefaults;
struct hdMiiverseDecodeHeapSlot_l { gptr<void> mpHeap; };
static hdMiiverseDecodeHeapSlot_l sMiiverseDecodeHeap;
void* miiversePhotoDecodeHeap();
void miiverseQueryJpeg(void*, u32, u32, hdMiiverseJpegDimensions_l*, be<u32>*);
void* miiverseAllocateAligned(u32, void*, u32);
void miiverseLockDecodeHeap(void*);
void miiverseUnlockDecodeHeap(void*);
void miiverseCalculateSurface(hdMiiverseSurface_l*);
void miiverseInitializeTextureRegisters(hdMiiverseTexture_l*);
void miiverseCopySurface(hdMiiverseSurface_l*, u32, u32, hdMiiverseSurface_l*, u32, u32);
void miiverseDecodeJpeg(void*, u32, void*, u32, u32, void*, u32);
void miiverseInitializeTextureFormat(be<u32>*, u32, u32);
void miiverseSetPhotoTexture(hdParts_c*, void*);

/* 0269FBF0 */
void hdMenuMiiverse_c::openSelectedPicture() {
    gabi::NativeFrame<0x228> frame;
    const u32 selected = mSelectedChart;
    hdMiiverseArchivePictures_l* archive = hdPartsView<hdMiiverseArchivePictures_l>(gabi::ea(sMiiverseScrollArchive.mpArchive.get()));
    const u32 entryIndex = mpChartButtons[selected < 10 ? selected : 0]->mArchiveIndex;
    hdMiiverseArchivePicture_l* entry = &archive->mPictures[entryIndex < 10 ? entryIndex : 0];
    void* heap = miiversePhotoDecodeHeap();
    void* jpeg = entry->mpJpegData;
    be<u32>* workSize = hdPartsView<be<u32>>(frame + 8);
    hdMiiverseJpegDimensions_l* dimensions = hdPartsView<hdMiiverseJpegDimensions_l>(frame + 0x14);
    *workSize = 0;
    miiverseQueryJpeg(jpeg, 0x50000, 1, dimensions, workSize);
    void* work = miiverseAllocateAligned(*workSize, heap, 4);
    miiverseLockDecodeHeap(sMiiverseDecodeHeap.mpHeap);
    hdMiiverseSurface_l* decoded = hdPartsView<hdMiiverseSurface_l>(frame + 0x20);
    const u32 height = dimensions->mHeight;
    decoded->mpMipImage = nullptr;
    decoded->mUsage = 1;
    decoded->mTileMode = 0x10;
    decoded->mFormat = 0x1A;
    decoded->mDimension = 1;
    decoded->mAntialias = 0;
    decoded->mSwizzle = 0;
    decoded->mpImage = nullptr;
    decoded->mDepth = 0;
    const u32 width = dimensions->mWidth;
    decoded->mMipLevels = 0;
    decoded->mWidth = width;
    decoded->mHeight = height;
    miiverseCalculateSurface(decoded);
    const u32 imageSize = decoded->mImageSize;
    const u32 alignment = decoded->mAlignment;
    void* pixels = miiverseAllocateAligned((imageSize + 3) & ~3u, heap, alignment);
    miiverseDecodeJpeg(work, *workSize, jpeg, 0x50000, 1, pixels, decoded->mImageSize);
    miiverseReleaseSessionStorage(work);
    decoded->mpImage = pixels;
    be<u32>* defaults = hdPartsView<be<u32>>(frame + 0x130);
    for (u32 word = 0; word < 54; ++word) defaults[word] = sMiiverseTextureDefaults.mWords[word];
    hdMiiverseTexture_l* texture = hdPartsView<hdMiiverseTexture_l>(frame + 0x94);
    texture->mSurface.mWidth = dimensions->mWidth;
    texture->mSurface.mMipLevels = 0;
    texture->mComponentSelection = defaults[26];
    texture->mMipCount = 0;
    texture->mSliceCount = 0;
    texture->mSurface.mDepth = 0;
    texture->mFirstMip = 0;
    texture->mSurface.mDimension = 1;
    texture->mSurface.mAntialias = 0;
    texture->mSurface.mTileMode = 0;
    texture->mFirstSlice = 0;
    texture->mSurface.mUsage = 1;
    texture->mSurface.mFormat = 0x1A;
    texture->mSurface.mSwizzle = 0;
    texture->mSurface.mHeight = dimensions->mHeight;
    miiverseCalculateSurface(&texture->mSurface);
    miiverseInitializeTextureRegisters(texture);
    texture->mSurface.mpImage.v = mSessionValue;
    miiverseCopySurface(decoded, 0, 0, &texture->mSurface, 0, 0);
    hdMiiverseTexture_l* display = hdPartsView<hdMiiverseTexture_l>(gabi::ea(this) + 0xBC);
    miiverseCopyJpegData(&display->mSurface, &texture->mSurface, 0x74, 0);
    display->mFirstSlice = 0;
    const u32 displayDepth = display->mSurface.mDepth;
    display->mFirstMip = 0;
    display->mSliceCount = displayDepth;
    const u32 displayLevels = display->mSurface.mMipLevels;
    display->mSurface.mFormat = 0x41A;
    display->mMipCount = displayLevels;
    display->mComponentSelection = 0x10203;
    miiverseInitializeTextureRegisters(display);
    be<u32>* format = hdPartsView<be<u32>>(frame + 0xC);
    miiverseInitializeTextureFormat(format, 0x41A, 1);
    const u16 option = hdPartsView<be<u16>>(frame + 0x9A)->get();
    const u16 category = hdPartsView<be<u16>>(frame + 0x9E)->get();
    const u32 convertedFormat = *format;
    mMessageId = 1;
    mMessageOption = option;
    mTextureFormat = u8(convertedFormat);
    mMessageCategory = category;
    miiverseReleaseSessionStorage(pixels);
    miiverseUnlockDecodeHeap(sMiiverseDecodeHeap.mpHeap);
    miiverseSetPhotoTexture(mpMessageScreen, &mMessageId);
}

s32 miiversePendingChartCount(void*);
void miiverseShowPreviousButton(hdParts_c*);
void miiverseHidePreviousButton(hdParts_c*);
f32 miiverseMessageAnimationRate(void*);

/* 026A0D0C */
u32 hdMenuMiiverse_c::handleControlActions() {
    const s32 result = mMessageResult;
    if (result == 3 || result == 2 || !miiverseIsSupportedSaveMode()) return 0;
    if (!miiverseControllerMode() && miiverseUsesTouchController(sMiiverseNavigationInput)) return 0;
    hdMiiverseArchivePictures_l* archive = hdPartsView<hdMiiverseArchivePictures_l>(gabi::ea(sMiiverseScrollArchive.mpArchive.get()));
    hdPartsStateId_c* emptyState = &sMiiverseCommentWaitState;
    switch (u32(mConfirmationStep)) {
    case 0: {
        bool pending = false;
        if (miiversePendingChartCount(archive) > 0) {
            mClosingRequested = 1;
            u32 entry = 0;
            if (miiverseArchiveChartCount(archive) >= 10)
                entry = u32(s32(u32(archive->mLastEntry) + 1) % 10);
            bool found = false;
            for (u32 scanned = 0; scanned < 10; ++scanned) {
                hdMiiverseArchivePicture_l* picture = &archive->mPictures[entry < 10 ? entry : 0];
                if (picture->mAvailable && !picture->mDisplayed) {
                    mSelectionStart = entry;
                    found = true;
                    break;
                }
                ++entry;
                if (s32(entry) > 9) entry = 0;
            }
            if (!found) mSelectionStart = 0xFFFFFFFF;
            pending = true;
        }
        const u8 hasPrevious = archive->mHasPreviousPage;
        if (hasPrevious && !archive->mPreviousPageDisplayed) {
            mHasPreviousPage = 1;
        } else if (mHasPreviousPage && !hasPrevious) {
            miiverseHidePreviousButton(mpPreviousButton);
            mHasPreviousPage = 0;
        } else if (!pending) {
            return 0;
        }
        mDialogCancelled = 1;
        setButtonsEnabled(0);
        const bool hasCharts = s32(filledChartCount()) > 0;
        if (!hasCharts && !mClosingRequested) {
            miiverseFinishAnimation(mpLayout->animSet(), 2, 1);
        } else if (!hasCharts) {
            hdPartsLayout_c* layout = mpLayout;
            const f32 rate = miiverseMessageAnimationRate(archive);
            hdAnimSet_play(layout->animSet(), 2, 1, rate);
        }
        mDragActive = 1;
        mConfirmationStep = u32(mConfirmationStep) + 1;
        clearSelectionFlags();
        if (mClosingRequested) miiverseSetInputMode(sMiiverseMainEventMgr, 0);
        return 1;
    }
    case 1:
        if (easeScrollToLowerBound()) {
            mDragActive = 0;
            mConfirmationStep = u32(mConfirmationStep) + 1;
        }
        return 1;
    case 2:
        if (mHasPreviousPage) {
            archive->mPreviousPageDisplayed = 1;
            miiverseShowPreviousButton(mpPreviousButton);
        }
        if (mClosingRequested) {
            const u32 selected = mSelectionStart;
            archive->mPictures[selected < 10 ? selected : 0].mDisplayed = 1;
            miiverseAssignChartEntry(mpPrimaryButton, mSelectionStart);
            mpPrimaryButton->mFilled = 1;
            if (s32(filledChartCount()) >= 10) {
                hdMiiverseChartButton_l* last = mpChartButtons[9];
                last->mFilled = 0;
                miiverseDisplayChartButton(last);
                last->changeState(emptyState);
            }
        }
        layoutChartButtons(1);
        prepareWindowScale();
        mConfirmationStep = u32(mConfirmationStep) + 1;
        return 1;
    case 3: {
        if (!updateWindowScale(0)) return 1;
        if (mClosingRequested) {
            hdPartsStateId_c* current = hdParts_getCurrentStateId(mpPrimaryButton->stateMachine());
            hdPartsStateIdVtbl_c* table = emptyState->mVtable;
            const u32 key = current->getKey();
            if (key != gabi::call_ptr<u32>(table->mGetKey.get(), emptyState)) return 1;
        }
        for (u32 chart = 0; chart < 10; ++chart) {
            if (!mpChartButtons[chart]->mFilled) continue;
            hdPartsStateId_c* current = hdParts_getCurrentStateId(mpChartButtons[chart]->stateMachine());
            hdPartsStateIdVtbl_c* table = emptyState->mVtable;
            const u32 key = current->getKey();
            if (key != gabi::call_ptr<u32>(table->mGetKey.get(), emptyState)) return 1;
        }
        hdPartsStateId_c* current = hdParts_getCurrentStateId(mpNextButton->stateMachine());
        hdPartsStateId_c* next = &sMiiverseSettingWaitState;
        hdPartsStateIdVtbl_c* table = next->mVtable;
        const u32 key = current->getKey();
        if (key != gabi::call_ptr<u32>(table->mGetKey.get(), next)) return 1;
        mConfirmationStep = u32(mConfirmationStep) + 1;
        return 1;
    }
    case 4:
        mConfirmationStep = 0;
        if (mClosingRequested) {
            prependPrimaryChart();
            hdMiiverseChartButton_l* first = mpChartButtons[0];
            mClosingRequested = 0;
            miiverseFinishSelectedChart(first);
        }
        if (miiverseArchiveBusy(archive)) return 1;
        for (u32 chart = 0; chart < 10; ++chart) {
            if (mpChartButtons[chart]->mFilled) {
                hdMiiverseChartButton_l* button = mpChartButtons[chart];
                if (button->mFilled) button->changeState(&sMiiverseCommentCheckInputState);
            }
        }
        mpNextButton->changeState(&sMiiverseSettingCheckInputState);
        mDialogCancelled = 0;
        miiverseResetPictureSave(sMiiversePictureSave.mpSave);
        setButtonsEnabled(1);
        miiverseFinishSelectedChart(mpChartButtons[0]);
        return 1;
    default:
        return 0;
    }
}

struct hdMiiversePhotoDialogSize_l : hdParts_c {
    u8 mDialogStorage[0x18];
    be<f32> mPhotoWidth, mPhotoHeight, mNaturalWidth, mNaturalHeight, mScale, mScaleStep;
};
WWHD_OFFSET(hdMiiversePhotoDialogSize_l, mPhotoWidth, 0x68);
WWHD_OFFSET(hdMiiversePhotoDialogSize_l, mScale, 0x78);
struct hdMiiverseSessionPrepareTable_l { u8 mTableStorage[0x2C]; gfn mPrepare; };
struct hdMiiverseSessionPrepare_l { u8 mSessionStorage[0xC]; gptr<hdMiiverseSessionPrepareTable_l> mVtable; };
hdMiiverseChartButton_l* miiverseConstructChart(hdMiiverseChartButton_l*);
u32 miiverseInitializeChart(hdMiiverseChartButton_l*, u32, u32, u32);
hdParts_c* miiverseConstructPrevious(hdParts_c*);
u32 miiverseInitializePrevious(hdParts_c*, u32, u32, u32);
hdParts_c* miiverseConstructNext(hdParts_c*);
u32 miiverseInitializeNext(hdParts_c*, u32, u32, u32);
hdParts_c* miiverseConstructPhotoDialog(hdParts_c*);
u32 miiverseInitializePhotoDialog(hdParts_c*, u32, u32, u32);
f32 miiverseChartPhotoWidth(hdMiiverseChartButton_l*);
f32 miiverseChartPhotoHeight(hdMiiverseChartButton_l*);
f32 miiverseChartScrollFraction(hdMiiverseChartButton_l*);
u32 miiverseQueryArchiveStatus();
void* miiverseMessageSessionParent();
void* miiverseCreateMessageSession(u32, hdUiSafeString*, void*, u32, u32);

/* 026A4710 */
u32 hdMenuMiiverse_c::initializeMenu(u32 resource, u32 options, u32 flags) {
    gabi::NativeFrame<0x48> frame;
    mLastInputId = sMiiverseNavigationInput->mInputId;
    mContentReady = miiverseUsesTouchController(sMiiverseNavigationInput);
    attachArchives(&sMiiverseMenuName, 1);
    if (!gabi::call_ptr<u32>(mVtable->mLoad.get(), this, &sMiiverseLayoutName,
            mpResAccessor.get(), resource, options, flags)) return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    findRootPanes();
    findTextPane();
    findBackgroundPattern();
    findWindowPanes();
    findPicturePane();
    mpTextPane->setCharacterCapacity(200);
    miiverseFinishAnimation(mpLayout->animSet(), 0, 0);
    vCalcLayout();
    hdScreenHeap* heap = sMiiverseUiSystem.mpSystem->mHeap;
    for (u32 index = 0; index < 10; ++index) {
        hdMiiverseChartButton_l* chart = static_cast<hdMiiverseChartButton_l*>(hdParts_new(0xD8, heap, 4));
        if (chart) chart = miiverseConstructChart(chart);
        mpChartButtons[index] = chart;
        miiverseInitializeChart(mpChartButtons[index], resource, options, 0);
    }
    hdMiiverseChartButton_l* primary = static_cast<hdMiiverseChartButton_l*>(hdParts_new(0xD8, heap, 4));
    if (primary) primary = miiverseConstructChart(primary);
    mpPrimaryButton = primary;
    miiverseInitializeChart(primary, resource, options, 0);
    hdParts_c* previous = static_cast<hdParts_c*>(hdParts_new(0x5C, heap, 4));
    if (previous) previous = miiverseConstructPrevious(previous);
    mpPreviousButton = previous;
    miiverseInitializePrevious(previous, resource, options, 0);
    hdParts_c* next = static_cast<hdParts_c*>(hdParts_new(0x68, heap, 4));
    if (next) next = miiverseConstructNext(next);
    mpNextButton = next;
    miiverseInitializeNext(next, resource, options, 0);
    hdMiiversePager_l* pager = static_cast<hdMiiversePager_l*>(mpNextButton.get());
    pager->mPositionY = sMiiverseEmptyNextPosition;
    pager->changeState(&sMiiverseSettingSlideDownState);
    pager = static_cast<hdMiiversePager_l*>(mpNextButton.get());
    miiverseSetNextButtonPosition(pager, pager->mPositionY);
    hdParts_c* message = static_cast<hdParts_c*>(hdParts_new(0xD4, heap, 4));
    if (message) message = miiverseConstructPhotoDialog(message);
    mpMessageScreen = message;
    miiverseInitializePhotoDialog(message, resource, options, 0);
    hdMiiversePhotoDialogSize_l* dialog = static_cast<hdMiiversePhotoDialogSize_l*>(mpMessageScreen.get());
    const f32 photoWidth = miiverseChartPhotoWidth(mpChartButtons[0]);
    const f32 photoHeight = miiverseChartPhotoHeight(mpChartButtons[0]);
    const f32 scale = f32(photoWidth / f32(dialog->mNaturalWidth));
    const f32 one = sMiiverseOne;
    const f32 divisor = sMiiverseMinScrollStep;
    const f32 remainder = f32(one - scale);
    dialog->mScale = scale;
    const f32 scaleStep = f32(remainder / divisor);
    dialog->mPhotoWidth = photoWidth;
    dialog->mPhotoHeight = photoHeight;
    dialog->mScaleStep = scaleStep;
    createPictureReceivers();
    const f32 scrollFraction = miiverseChartScrollFraction(mpChartButtons[0]);
    hdMiiverseLayoutPane_l* upperWindow = static_cast<hdMiiverseLayoutPane_l*>(mpWindowPanes[1].get());
    hdMiiverseLayoutPane_l* lowerWindow = static_cast<hdMiiverseLayoutPane_l*>(mpWindowPanes[0].get());
    mScrollFraction = scrollFraction;
    mWindowWidth = upperWindow->mScaleY;
    const f32 half = sMiiverseHalf;
    const f32 windowWidth = lowerWindow->mScaleX;
    const f32 windowHeight = lowerWindow->mScaleY;
    const f32 halfWidth = f32(windowWidth * half);
    hdMiiverseLayoutPane_l* picture = static_cast<hdMiiverseLayoutPane_l*>(mpPicturePane.get());
    const f32 halfHeight = f32(windowHeight * half);
    const f32 x = picture->mTranslationX;
    const f32 right = f32(x + halfWidth);
    const f32 left = f32(x - halfWidth);
    const f32 bottom = f32(f32(picture->mTranslationY) - halfHeight);
    const f32 top = f32(x + halfHeight);
    if (left < right) {
        mTouchMaxX = right;
        mTouchMinX = left;
    } else {
        mTouchMaxX = left;
        mTouchMinX = right;
    }
    if (!(bottom < top)) {
        mTouchMinY = top;
        mTouchMaxY = bottom;
    } else {
        mTouchMaxY = top;
        mTouchMinY = bottom;
    }
    lowerWindow = static_cast<hdMiiverseLayoutPane_l*>(mpWindowPanes[0].get());
    const f32 width = mWindowWidth;
    const f32 lower = lowerWindow->mTranslationY;
    const f32 upper = gabi::fnmsubs(width, half, width);
    mScrollLowerBound = lower;
    mScrollUpperBound = upper;
    if (!miiverseQueryArchiveStatus()) mMessageResult = 3;
    else if (miiverseQueryPrimaryMode()) mMessageResult = 0;
    else mMessageResult = miiverseQuerySecondaryMode() ? 1 : 2;
    updateMessageText();
    hdUiSafeString* name = hdPartsView<hdUiSafeString>(frame + 8);
    name->mVtable = &sMiiverseStringVtable;
    name->mText.v = gabi::guest_addr("MenuMiiverseTexture");
    void* parent = miiverseMessageSessionParent();
    void* session = miiverseCreateMessageSession(0, name, parent, 1, 0);
    mMessageValue = gabi::ea(session);
    mTransferHandle = 0;
    mSessionValue = gabi::ea(miiverseAllocateAligned(0x16A800, session, 0x800));
    hdMiiverseSessionPrepare_l* prepared = hdPartsView<hdMiiverseSessionPrepare_l>(mMessageValue);
    gabi::call_ptr<void>(prepared->mVtable->mPrepare.get(), prepared);
    startState(&sMiiversePartsBlankState);
    return 1;
}

/* WWHD glue (tools/verify/autoglue.py): Miiverse menu
 data sMiiverseBoxDisplayState 10499B24 nocheck
 data sMiiverseBoxInState 10499ACC nocheck
 data sMiiverseBoxOutState 10499AF8 nocheck
 data sMiiverseBoxSaveState 10499BA8 nocheck
 data sMiiverseBoxWaitState 10499B7C nocheck
 data sMiiverseCheckInputState 10497740 nocheck
 data sMiiverseCommentCheckInputState 104978EC nocheck
 data sMiiverseCommentInState 10497918 nocheck
 data sMiiverseCommentPhotoState 10497970 nocheck
 data sMiiverseCommentSlideDownState 10497894 nocheck
 data sMiiverseCommentWaitState 104978C0 nocheck
 data sMiiverseDirectStateVtable 100FBBA8 nocheck
 data sMiiverseDragState 1049776C nocheck
 data sMiiverseHolderVtable 100FC030 nocheck
 data sMiiverseInAllState 104976D8 nocheck
 data sMiiverseNewCommentCheckInputState 10497A28 nocheck
 data sMiiverseNewCommentInOutState 10497A54 nocheck
 data sMiiverseNewCommentWaitState 104979FC nocheck
 data sMiiverseNullState 101FF32C nocheck
 data sMiiverseOutAllState 10497708 nocheck
 data sMiiversePartsBlankState 1049DD60 nocheck
 data sMiiversePatternName 1049761C nocheck
 data sMiiversePhotoViewState 10497798 nocheck
 data sMiiverseRootType 101FD8FC nocheck
 data sMiiverseRootTypeReady 101FD888 nocheck
 data sMiiverseSecondaryVtable 100FC040 nocheck
 data sMiiverseSettingCheckInputState 10497B28 nocheck
 data sMiiverseSettingInState 10497B54 nocheck
 data sMiiverseSettingSlideDownState 10497BAC nocheck
 data sMiiverseSettingWaitState 10497AFC nocheck
 data sMiiverseStateVtable 100FBBF0 nocheck
 data sMiiverseStringVtable 100FBB3C nocheck
 data sMiiverseTextName 10497614 nocheck
 data sMiiverseVtable 100FBF30 nocheck
 data sMiiverseWindowNames 10497660 nocheck
 ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString*, u32)
 ext 02704AFC void hdTextPane_c::setCharacterCapacity(u32 count)
 ext 026A7700 hdMiiverseChartButton_l* miiverseConstructChart(hdMiiverseChartButton_l*)
 ext 026A79C8 u32 miiverseInitializeChart(hdMiiverseChartButton_l*, u32, u32, u32)
 ext 026AA2B4 hdParts_c* miiverseConstructPrevious(hdParts_c*)
 ext 026AA410 u32 miiverseInitializePrevious(hdParts_c*, u32, u32, u32)
 ext 026AB770 hdParts_c* miiverseConstructNext(hdParts_c*)
 ext 026AB8CC u32 miiverseInitializeNext(hdParts_c*, u32, u32, u32)
 ext 026C44E4 hdParts_c* miiverseConstructPhotoDialog(hdParts_c*)
 ext 026C4B9C u32 miiverseInitializePhotoDialog(hdParts_c*, u32, u32, u32)
 ext 026A8E1C f32 miiverseChartPhotoWidth(hdMiiverseChartButton_l*)
 ext 026A8E38 f32 miiverseChartPhotoHeight(hdMiiverseChartButton_l*)
 ext 026A6A98 f32 miiverseChartScrollFraction(hdMiiverseChartButton_l*)
 ext 026A45D0 u32 miiverseQueryArchiveStatus()
 ext 0203E8C4 void* miiverseMessageSessionParent()
 ext 02753004 void* miiverseCreateMessageSession(u32, hdUiSafeString*, void*, u32, u32)
 str 100FBE3C "MenuMiiverseTexture"
 ext 026AA828 void miiverseShowPreviousButton(hdParts_c*)
 ext 02039838 s32 miiversePendingChartCount(void*)
 ext 026AA91C void miiverseHidePreviousButton(hdParts_c*)
 ext 0203A958 f32 miiverseMessageAnimationRate(void*)
 data sMiiverseTextureDefaults 100FBC38 nocheck
 data sMiiverseDecodeHeap 101F8B18 nocheck
 ext 0203E90C void* miiversePhotoDecodeHeap()
 ext 0284DDB0 void miiverseQueryJpeg(void*, u32, u32, hdMiiverseJpegDimensions_l*, be<u32>*)
 ext 0273B0D4 void* miiverseAllocateAligned(u32, void*, u32)
 ext 0274FBF8 void miiverseLockDecodeHeap(void*)
 ext 0274FCCC void miiverseUnlockDecodeHeap(void*)
 ext C00060D0 void miiverseCalculateSurface(hdMiiverseSurface_l*)
 ext C0006498 void miiverseInitializeTextureRegisters(hdMiiverseTexture_l*)
 ext C0006120 void miiverseCopySurface(hdMiiverseSurface_l*, u32, u32, hdMiiverseSurface_l*, u32, u32)
 ext 0284BE8C void miiverseDecodeJpeg(void*, u32, void*, u32, u32, void*, u32)
 ext 0287F1EC void miiverseInitializeTextureFormat(be<u32>*, u32, u32)
 ext 026C4ECC void miiverseSetPhotoTexture(hdParts_c*, void*)
 data sMiiverseInitWords 10497678 nocheck
 data sMiiverseInitSettings 10497634 nocheck
 data sMiiverseInitRegistration 101F6420 nocheck
 data sMiiverseMathRegistration 101F642C nocheck
 data sMiiverseRandomRegistration 101F6438 nocheck
 data sMiiverseLayoutName 10497738 nocheck
 data sMiiverseMenuName 10497624 nocheck
 data sMiiverseMessageNames 104976B0 nocheck
 data sMiiverseStateCounterReady 101FD8E4 nocheck
 data sMiiverseStateCounter 101FDD50 nocheck
 ext 028ED6F8 void miiverseInitializeMath(void*)
 ext 028EAB2C void miiverseInitializeRandom(void*)
 str 100FBE60 "InAll"
 str 100FBE68 "OutAll"
 str 100FBE70 "OutMsg"
 str 100FBE78 "MenuMiiverse_00"
 str 100FBE88 "::StateID_InAll"
 str 100FBE98 "T_Msg_00_03"
 str 100FBEA4 "T_Msg_00_02"
 str 100FBEB0 "T_Msg_00_01"
 str 100FBEBC "::StateID_PhotoView"
 str 100FBED0 "G_All_00"
 str 100FBEDC "G_Msg_00"
 str 100FBEE8 "B_Slide_00"
 str 100FBEF4 "::StateID_CheckInput"
 str 100FBF0C "::StateID_Drag"
 str 100FBF1C "::StateID_OutAll"
 ext 026ABA84 void miiverseAddNextLink(hdParts_c*, hdParts_c*)
 ext 026ABAAC void miiverseSetNextBackLink(hdParts_c*, hdParts_c*)
 ext 026A78DC void miiversePrepareChartNeighbors(hdMiiverseChartButton_l*)
 ext 026A8BE8 void miiverseClearChartNeighbors(hdMiiverseChartButton_l*)
 ext 026A8D2C hdParts_c* miiverseChartReceiver(hdMiiverseChartButton_l*)
 ext 026A8A00 void miiverseSetChartNeighbors(hdMiiverseChartButton_l*, hdParts_c*, hdParts_c*)
 ext 026AA6A8 void miiverseRemovePreviousLink(hdParts_c*, u32)
 ext 026AA6F8 void miiverseSetPreviousBackLink(hdParts_c*, u32)
 ext 026AA6D0 void miiverseAddPreviousLink(hdParts_c*, hdParts_c*)
 ext 026AA720 void miiverseSetPreviousForwardLink(hdParts_c*, hdParts_c*)
 ext 026A879C void miiverseSetChartUp(hdMiiverseChartButton_l*, u32)
 ext 026A87C4 void miiverseSetChartDown(hdMiiverseChartButton_l*, u32)
 ext 026A87EC void miiverseSetChartPhoto(hdMiiverseChartButton_l*, u32)
 ext 026A8814 void miiverseRemoveChartUp(hdMiiverseChartButton_l*, u32)
 ext 026A88B8 void miiverseRemoveChartDown(hdMiiverseChartButton_l*, u32)
 ext 026A895C void miiverseRemoveChartPhoto(hdMiiverseChartButton_l*, u32)
 str 100FBDB0 "SE_MIIVERSE_PHOTO_CLOSE"
 ext 025F795C u32 miiverseFindDialog(void*)
 ext 027201DC void* miiverseGetPictureSave(hdSaveHolders_c*)
 ext 02726DEC void miiverseBeginPictureSave(void*, u32)
 data sMiiverseMessageCore 101F4B5C nocheck
 data sMiiverseDialogMode 1047B07C nocheck
 ext 026A8DD0 void miiverseGetChartPosition(hdMiiverseChartButton_l*, hdMiiversePhotoPosition_l*)
 ext 02520C0C u32 miiverseHasCameraItem(u32)
 ext 025F8388 void miiverseSetDialogMessage(void*, hdUiSafeString*, hdUiSafeString*, u32, u32)
 ext 02030B38 void miiversePlayPhotoSound(const char*)
 str 100FBD78 "MsgOnly_00"
 str 100FBD84 "T_Msg_SaveError_01"
 str 100FBD68 "T_Msg_SaveError"
 str 100FBD98 "SE_MIIVERSE_PHOTO_OPEN"
 ext 026A9048 void miiverseSetPrimaryTarget(hdMiiverseChartButton_l*, f32)
 ext 026A8D18 f32 miiverseChartHeight(hdMiiverseChartButton_l*)
 ext 026A9148 u32 miiverseChartEndsGroup(hdMiiverseChartButton_l*)
 ext 0203B7CC f32 miiverseChartGroupSpacing(void*)
 ext 026A73C4 void miiverseApplyChartPosition(hdMiiverseChartButton_l*, f32)
 data sMiiversePictureInputName 1049762C nocheck
 ext 02616ED4 void* miiverseConstructPictureReceiver(void*, hdPartsPane_c*, u32)
 ext 02039970 u32 miiverseArchivePageChanged(void*)
 ext 026A04A0 void miiverseLayoutChartButtons(hdMenuMiiverse_c*, u32)
 ext 026AB3A0 void miiverseSetNextButtonPosition(hdParts_c*, f32)
 ext 026A8E54 void miiversePrepareChartInput(hdMiiverseChartButton_l*)
 ext 026ABB44 void miiversePrepareNextInput(hdParts_c*)
 data sMiiverseEmptyNextPosition 100FBD1C
 ext 025986BC u32 miiverseControllerMode()
 ext 026184AC u32 miiverseUsesTouchController(hdMiiverseNavigationInput_l*)
 ext 026A22C4 void miiverseRestoreInputPresentation(hdMenuMiiverse_c*)
 ext 026A9150 void miiverseUpdateChartSound(hdMiiverseChartButton_l*)
 ext 027621EC void miiverseUpdateSoundFader(void*)
 ext 026A0D0C u32 miiverseHandleControlActions(hdMenuMiiverse_c*)
 ext 020399DC u32 miiverseArchiveBusy(void*)
 ext 0269FBF0 void miiverseOpenSelectedPicture(hdMenuMiiverse_c*)
 ext 02726218 s32 miiverseFirstEmptyPicture(hdSavePicture_l*)
 ext 0272609C hdJpeg_c* miiverseMutablePicture(hdSavePicture_l*, u32)
 ext 027261D0 void miiverseAppendPictureOrder(hdSavePicture_l*, u32)
 ext C0009988 void miiverseCopyJpegData(void*, const void*, u32, u32)
 ext 02725A1C u8* hdJpeg_c::data()
 ext 027259BC void hdJpeg_c::setSubject(u32)
 ext 027259CC void hdJpeg_c::setLabel(u32)
 ext 0272599C void hdJpeg_c::enable()
 ext 027259E4 void hdJpeg_c::markSaved()
 ext 027259F8 void hdJpeg_c::setDataSize(u32)
 ext 02725A14 void hdJpeg_c::setState(u32)
 ext 02725B4C void hdJpeg_c::setChecksum(u32)
 ext 02039A08 s32 miiverseArchiveChartCount(void*)
 ext 026AA7D0 void miiverseRestorePreviousButton(hdParts_c*)
 ext 026A8F90 void miiverseAssignChartEntry(hdMiiverseChartButton_l*, u32)
 ext 026F91F8 void hdParts_c::detachArchives()
 ext 0273AFC8 void miiverseReleaseSessionStorage(void*)
 data sMiiverseNavigationScrollDivisor 100FBD30
 ext 02704D2C void miiverseGetNavigationPosition(hdMiiverseNavigationPosition_l*, hdMiiverseNavigationPane_l*)
 data sMiiverseHeapManager 101F8B4C nocheck
 ext 026A45DC u32 miiverseQueryPrimaryMode()
 ext 026A4614 u32 miiverseQuerySecondaryMode()
 ext 026AA798 f32 miiversePreviousButtonPosition(hdParts_c*)
 ext 026AA7C4 f32 miiversePreviousButtonHeight(hdParts_c*)
 ext 026A8CEC f32 miiverseChartButtonPosition(hdMiiverseChartButton_l*)
 data sMiiverseAnimationGroups 100FBB54 nocheck
 data sMiiverseAnimationNames 10497688 nocheck
 data sMiiverseGroupNames 10497698 nocheck
 ext 02004E04 void miiverseBindAnimationGroup(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*)
 ext 026A8D40 void miiverseSetChartAvailable(hdMiiverseChartButton_l*, u32)
 ext 026A9140 u32 miiverseChartHasPicture(hdMiiverseChartButton_l*)
 ext 02720144 hdSavePicture_l* miiverseGetPictureHolder(hdSaveHolders_c*)
 ext 027260BC hdJpeg_c* miiverseGetSavedPicture(hdSavePicture_l*, u32)
 ext 02725B44 u32 miiverseSavedPictureId(hdJpeg_c*)
 ext 026A868C void miiverseCopyChart(hdMiiverseChartButton_l*, hdMiiverseChartButton_l*)
 ext 027045B0 void hdTextPane_c::resetMessage(hdUiSafeString* message, hdUiSafeString* label)
 data sMiiversePictureSave 101F8344 nocheck
 ext 0203A24C void miiverseAdvanceArchivePage(void*)
 ext 026AA94C void miiverseDisplayPreviousButton(hdParts_c*)
 ext 027143F0 void miiverseResetPictureSave(void*)
 ext 02005674 void miiverseFinishAnimation(hdAnimSet*, u32, u32)
 ext 02615398 u32 miiverseGetFocusedReceiver(EventMgr*)
 ext 02005708 void miiverseResetAnimation(hdAnimSet*, u32, u32)
 ext 02615140 u32 miiverseGetInputMode(EventMgr*)
 ext 026ABB24 f32 miiverseNextButtonPosition(hdParts_c*)
 ext 026ABB38 f32 miiverseNextButtonHeight(hdParts_c*)
 ext 0203B7C0 f32 miiverseChartSpacing(void*)
 ext 0203A910 f32 miiverseWindowScaleDuration(void*)
 data sMiiverseInputOptions 101F8378 nocheck
 data sMiiverseGlobalInputMode 1047B096 nocheck
 ext 026A8CCC void miiverseFinishSelectedChart(hdMiiverseChartButton_l*)
 ext 025200D4 dOwPlayView_c* miiverseGetPlayInfo()
 ext 028713B8 u32 miiverseAnimationFrameCount(hdAnimSetController*)
 ext 026A8FA0 void miiverseSetChartPresentation(hdMiiverseChartButton_l*, u32, f32)
 ext 026AA9D0 void miiverseSetPreviousPresentation(hdParts_c*, u32, f32)
 ext 026ABBE0 void miiverseSetNextPresentation(hdParts_c*, u32, f32)
 ext 026150C0 void miiverseFocusPicture(EventMgr*, void*)
 data sMiiverseDefaultTouch 1049FFA0 nocheck
 data sMiiverseDragThresholdSquared 100FBD24
 data sMiiversePictureName 101F641C nocheck
 data sMiiversePictureTypeReady 101FD938 nocheck
 data sMiiversePictureBaseTypeReady 101FD88C nocheck
 data sMiiversePictureType 101FD93C nocheck
 data sMiiversePictureBaseType 101FD940 nocheck
 data sMiiverseScrollSoundScale 100FBD3C
 data sMiiverseNegativeOne 100FBD40
 data sMiiverseHalf 100FBD20
 str 100FBD44 "SE_MIIVERSE_SCROLL"
 ext 02030B6C void miiversePlayScrollSound(const char*, f32, f32)
 ext 026A8F20 void miiversePrepareChartAnimation(hdMiiverseChartButton_l*)
 ext 026AA810 void miiversePreparePreviousAnimation(hdParts_c*)
 ext 026ABB7C void miiversePrepareNextAnimation(hdParts_c*)
 ext 0273AD10 void* miiverseAllocateStorage(u32)
 ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c* i_this)
 ext 02762170 void miiverseConstructSoundFader(void*, f32)
 ext 02762258 void miiverseSetSoundFade(void*, u32, f32)
 data sMiiverseMainEventMgr 101F4FF0 nocheck
 ext 020013C0 hdUiEvent* miiverseCopyInputEvent(hdUiEvent*, hdUiEvent*)
 ext 026151A8 Pair32 miiverseTouchFromStart(EventMgr*)
 data sMiiverseMainSaveInfo 101F84DC nocheck
 data sMiiverseNavigationInput 101F5088 nocheck
 data sMiiverseZero 100FBD14
 data sMiiverseOne 100FBD10
 data sMiiverseMinScrollStep 100FBD18
 data sMiiverseDragDamping 100FBD34
 data sMiiverseDragMaxStep 100FBD38
 data sMiiverseScrollArchive 1018F4AC nocheck
 ext 0203B7B4 f32 miiverseGetScrollDivisor(void*)
 ext 0200ECD4 f32 miiverseAddCalc(be<f32>*, f32, f32, f32, f32)
 ext 0200F5C8 u32 miiverseChaseFloat(be<f32>*, f32, f32)
 ext 026A8F3C void miiverseScrollChartButton(hdMiiverseChartButton_l*, f32)
 ext 026AA97C void miiverseScrollPreviousButton(hdParts_c*, f32)
 ext 026ABB8C void miiverseScrollNextButton(hdParts_c*, f32)
 ext 026A80B8 void miiverseDisplayChartButton(hdMiiverseChartButton_l*)
 ext 026A80F4 void miiverseStopChartButton(hdMiiverseChartButton_l*)
 data sMiiverseUpperBound 100FBD28
 data sMiiverseLowerBound 100FBD2C
 ext 026F88F0 void hdParts_c::destroy(hdParts_c* i_this, s32 flags)
 ext 0273AF40 void miiverseDelete(void*)
 data sMiiverseRootNames 10497650 nocheck
 data sMiiverseRootNamesReady 104976C8 nocheck
 data sMiiverseRootRegistration 101F63EC nocheck
 str 100FBDC8 "N_All_00"
 str 100FBDD4 "N_Msg_00"
 ext 028F026C void miiverseRegisterStringNames(void*)
 ext 028F0164 void miiverseDestroyStringNames(void*, u32, u32, u32, u32, u32)
 data sMiiverseUiSystem 1018C404 nocheck
 ext 026EA5CC hdParts_c* miiverseConstructTincleTower(hdParts_c*)
 ext 026EA690 u32 miiverseInitializeTincleTower(hdParts_c*, void*, u32, u32)
 ext 0266A69C hdParts_c* miiverseConstructFairy(hdParts_c*)
 ext 0266A760 u32 miiverseInitializeFairy(hdParts_c*, void*, u32, u32)
 ext 0266E79C hdParts_c* miiverseConstructHeartIsland(hdParts_c*)
 ext 0266E860 u32 miiverseInitializeHeartIsland(hdParts_c*, void*, u32, u32)
 ext 026DF6D4 hdParts_c* miiverseConstructSquid(hdParts_c*)
 ext 026DF798 u32 miiverseInitializeSquid(hdParts_c*, void*, u32, u32)
 ext 026ED894 hdParts_c* miiverseConstructTriforce(hdParts_c*)
 ext 026ED980 u32 miiverseInitializeTriforce(hdParts_c*, void*, u32, u32)
 ext 026E93F8 hdParts_c* miiverseConstructTerry(hdParts_c*)
 ext 026E94BC u32 miiverseInitializeTerry(hdParts_c*, void*, u32, u32)
 ext 026D3B6C hdParts_c* miiverseConstructSecretEntrance(hdParts_c*)
 ext 026D3C30 u32 miiverseInitializeSecretEntrance(hdParts_c*, void*, u32, u32)
 ext 026E5D00 hdParts_c* miiverseConstructSubmarine(hdParts_c*)
 ext 026E5DC4 u32 miiverseInitializeSubmarine(hdParts_c*, void*, u32, u32)
 ext 02668C18 hdParts_c* miiverseConstructEnemyTower(hdParts_c*)
 ext 02668CDC u32 miiverseInitializeEnemyTower(hdParts_c*, void*, u32, u32)
 ext 026AC230 hdParts_c* miiverseConstructMoon(hdParts_c*)
 ext 026AC2F4 u32 miiverseInitializeMoon(hdParts_c*, void*, u32, u32)
 ext 0266EE48 hdParts_c* miiverseConstructHeartSea(hdParts_c*)
 ext 0266EF0C u32 miiverseInitializeHeartSea(hdParts_c*, void*, u32, u32)
 ext 0266C5B0 hdParts_c* miiverseConstructGhostShip(hdParts_c*)
 ext 0266C7AC u32 miiverseInitializeGhostShip(hdParts_c*, void*, u32, u32)
*/

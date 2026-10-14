/* HD scrolling message window controls and animation helpers. */
#include "d/hd_msg_window_scroll_extra.h"
using namespace gabi;
extern void scrollSetText(ScrollMessage_l *, const char *, u32);
extern void scrollSetPaneText(ScrollPane_l *, ScrollMessage_l *, u32);
extern void scrollSetFrame(void *, u32, u32, f32);
extern void scrollPlay(void *, u32, u32, f32);
extern void scrollStop(void *, u32, u32, f32);
extern void scrollSound(u32);
extern void scrollCloseSound();
extern be<s16> *scrollBackgroundColor();
extern f32 scrollCurve(f32);
static SScrollAnimationKindsView_l sScrollAnimationKinds;
static HdMsgState sScrollButtonWaitState;
static HdMsgState sScrollCloseState;

extern void scrollSetLines(MsgWindowScroll_l *, const char *);
extern void scrollOpenSound();
static HdMsgControlPointer scrollControlManager;
extern void scrollSetControl(void *, u32);
extern u32 scrollGetControl(void *);
extern ScrollPlay_l *scrollGetPlay();
static u8 sScrollDirection;
static HdMsgState sScrollEndState;
static HdMsgState sScrollUpState;
static HdMsgState sScrollDownState;
static HdMsgState sScrollResetState;
static SScrollMaterialVisibleView_l sScrollMaterialVisible;

extern void scrollBaseDraw(MsgWindowScroll_l *);

extern void scrollSetPaneCapacity(ScrollPane_l *, u32);
static ScrollStringVirtual_l sScrollStringVtable;
static SScrollRootNamesView_l sScrollRootNames;
static SScrollTextNamesView_l sScrollTextNames;
static SScrollMaterialNamesView_l sScrollMaterialNames;

static u32 sScrollRootNamesGuard;

struct ScrollRegistration_l {
  u8 mOpaque[12];
};
static ScrollRegistration_l sScrollRootRegistration;

extern void scrollRegister(ScrollRegistration_l *);
/* inlined into message window pane searches */
static ScrollPane_l *scrollFindPane(hdPartsPane_c *root,
                                    ScrollString_l *name) {
  ScrollRootVirtual_l *vtable = reinterpret_cast<ScrollRootVirtual_l *>(root->mVtable.get());
  gabi::call_ptr<void>(name->mpVtable->mpEnsure, name);
  return gabi::call_ptr<ScrollPane_l *>(vtable->mpFindPane, root,
                                        name->mpText.get(), 1u);
}

struct ScrollVisibility_l {
  be<u8> mFirst[3];
  be<u8> mSecond[3];
};

extern void scrollViewportSave(ScrollBounds_l *, void *);
extern void scrollViewportApply(ScrollBounds_l *, void *);

extern u32 renderTvWidth();
extern u32 renderTvHeight();
extern u32 renderTvTargetWidth();
extern u32 renderTvTargetHeight();
extern hdPartsStateId_c *scrollCurrentState(HdMsgController *);
extern void scrollStateUpdate(HdMsgController *);

extern void scrollDestroyArray(ScrollString_l *, u32, u32,
                               ScrollStringDestructor_l, u32, u32);
extern void scrollStringDestroy(ScrollString_l *, u32);
extern void scrollBaseDestroy(MsgWindowScroll_l *, u32);
extern MsgWindowScroll_l *scrollBaseConstruct(MsgWindowScroll_l *);
extern void *scrollAllocate(u32);
extern void scrollDelete(void *);
extern void scrollBoundsConstruct(ScrollBounds_l *);
static SScrollStateVtableView_l sScrollStateVtable;
static SScrollStateOwnerVtableView_l sScrollStateOwnerVtable;
static SScrollWindowVtableView_l sScrollWindowVtable;

extern void scrollBaseCreate(MsgWindowScroll_l *, ScrollString_l *, u32);

extern u32 scrollEventBit(void *, u32);
extern void scrollMessageLookup(ScrollMessage_l *, u32, u32);

extern void *scrollAllocateFromHeap(u32, void *, u32);
extern hdPartsKeyIcon_c *scrollKeyIconConstruct(void *);
extern hdPartsKeyIcon_c *scrollScrollKeyIconConstruct(void *);
extern u32 scrollKeyIconCreate(hdPartsKeyIcon_c *, u32, u32, ScrollResource_l *);
extern u32 scrollScrollKeyIconCreate(hdPartsKeyIcon_c *, u32, u32,
                                     ScrollResource_l *);
extern u32 scrollBaseCreateIcon(MsgWindowScroll_l *, u32, ScrollResource_l *);
extern hdPartsLayout_c *scrollLayoutConstruct(void *);
extern void scrollBindAnimation(void *, u32, ScrollString_l *,
                                ScrollString_l *);

extern void scrollRandomConstruct(be<u8> *);
extern void scrollUtilityConstruct(be<u8> *);

/* 026B98A8 */
static void *scrollGetObject(gptr<void> *object) { return *object; }

/* 026B9A80 */
void MsgWindowScroll_l::hideButton(u32 index) {
  ScrollPane_l *pane = mpButtonPanes[index < 3 ? index : 0];
  pane->mFlags = (pane->mFlags & 0xFE);
  const u32 count = mButtonCount;
  gptr<hdPartsKeyIcon_c> *buttons = mpButtons;
  buttons[index < count ? index : 0]->hide();
}

/* 026BA3F8 */
void MsgWindowScroll_l::showButton(u32 index) {
  ScrollPane_l *pane = mpButtonPanes[index < 3 ? index : 0];
  pane->mFlags = (pane->mFlags & 0xFE) | 1;
  const u32 count = mButtonCount;
  gptr<hdPartsKeyIcon_c> *buttons = mpButtons;
  buttons[index < count ? index : 0]->requestIn();
}

/* 026BA80C */
void MsgWindowScroll_l::pressButton(u32 index) {
  ScrollPane_l *pane = mpButtonPanes[index < 3 ? index : 0];
  pane->mFlags = (pane->mFlags & 0xFE) | 1;
  const u32 count = mButtonCount;
  gptr<hdPartsKeyIcon_c> *buttons = mpButtons;
  buttons[index < count ? index : 0]->requestOut();
}

/* 026B9AC4 */
void MsgWindowScroll_l::hideMiddleButton() {
  ScrollPane_l *pane = mpButtonPanes[1];
  pane->mFlags = (pane->mFlags & 0xFE);
  const u32 count = mButtonCount;
  gptr<hdPartsKeyIcon_c> *buttons = mpButtons;
  buttons[count > 1 ? 1 : 0]->hide();
}

/* 026BA440 */
void MsgWindowScroll_l::showMiddleButton() {
  ScrollPane_l *pane = mpButtonPanes[1];
  pane->mFlags = (pane->mFlags & 0xFE) | 1;
  const u32 count = mButtonCount;
  gptr<hdPartsKeyIcon_c> *buttons = mpButtons;
  buttons[count > 1 ? 1 : 0]->requestIn();
}

/* 026BA854 */
void MsgWindowScroll_l::pressMiddleButton() {
  ScrollPane_l *pane = mpButtonPanes[1];
  pane->mFlags = (pane->mFlags & 0xFE) | 1;
  const u32 count = mButtonCount;
  gptr<hdPartsKeyIcon_c> *buttons = mpButtons;
  buttons[count > 1 ? 1 : 0]->requestOut();
}

/* 026BAEA8 */
void MsgWindowScroll_l::startMiddleButton() {
  ScrollPane_l *pane = mpButtonPanes[1];
  pane->mFlags = (pane->mFlags & 0xFE) | 1;
  const u32 count = mButtonCount;
  gptr<hdPartsKeyIcon_c> *buttons = mpButtons;
  buttons[count > 1 ? 1 : 0]->requestIn();
}

/* 026BAF9C */
void MsgWindowScroll_l::finishMiddleButton() {
  ScrollPane_l *pane = mpButtonPanes[1];
  pane->mFlags = (pane->mFlags & 0xFE) | 1;
  const u32 count = mButtonCount;
  gptr<hdPartsKeyIcon_c> *buttons = mpButtons;
  buttons[count > 1 ? 1 : 0]->requestDecide();
}

/* 026BA52C */
u32 MsgWindowScroll_l::buttonInEnded(u32 index) {
  const u32 count = mButtonCount;
  gptr<hdPartsKeyIcon_c> *buttons = mpButtons;
  return buttons[index < count ? index : 0]->isWaiting();
}

/* 026BAD54 */
u32 MsgWindowScroll_l::buttonPressEnded(u32 index) {
  const u32 count = mButtonCount;
  gptr<hdPartsKeyIcon_c> *buttons = mpButtons;
  return buttons[index < count ? index : 0]->isHidden();
}

/* 026BA54C */
u32 MsgWindowScroll_l::middleInEnded() {
  const u32 count = mButtonCount;
  gptr<hdPartsKeyIcon_c> *buttons = mpButtons;
  return buttons[count > 1 ? 1 : 0]->isWaiting();
}

/* 026BAD74 */
u32 MsgWindowScroll_l::middlePressEnded() {
  const u32 count = mButtonCount;
  gptr<hdPartsKeyIcon_c> *buttons = mpButtons;
  return buttons[count > 1 ? 1 : 0]->isHidden();
}

/* 026B9AF0 */
void MsgWindowScroll_l::setLine(u32 index) {
  const u32 paneIndex = mPaneIndices[index < 3 ? index : 0];
  const u32 line = mLineIndex + index;
  ScrollMessage_l *message = mpMessage;
  const char *text = mpLines[line < 20 ? line : 0];
  scrollSetText(message, text, 0x14F);
  mpMessage->mScrollText = 1;
  scrollSetPaneText(mpTextPanes[paneIndex < 3 ? paneIndex : 0], mpMessage, 0);
  ScrollPane_l *pane = mpTextPanes[paneIndex < 3 ? paneIndex : 0];
  pane->mFlags = (pane->mFlags & 0xFE) | 1;
}

/* 026B9D9C */
void MsgWindowScroll_l::playAnimation(u32 animation) {
  const u32 kind = sScrollAnimationKinds.mValues[animation];
  scrollSetFrame(mpAnimation->animSet(), animation, kind, 0.0f);
  scrollPlay(mpAnimation->animSet(), animation, kind, 1.0f);
}

/* 026B9E1C */
void MsgWindowScroll_l::positionLines() {
  for (u32 i = 0; i < 3; ++i) {
    const u32 index = mPaneIndices[i];
    ScrollPane_l *pane = mpTextPanes[index < 3 ? index : 0];
    const u32 positionIndex = mPaneIndices[i];
    const f32 position = fadds_ppc(
        mLinePositions[positionIndex < 3 ? positionIndex : 0], mPosition);
    pane->mFlags = pane->mFlags | 0x10;
    pane->mTranslationY = position;
  }
}

/* 026BA3A0 */
u32 MsgWindowScroll_l::canScrollDown() {
  const u32 line = mLineIndex + 2;
  if (s32(gabi::ea((const char *)mpLines[line < 20 ? line : 0])) >= 0)
    return 1;
  return mLineCount == 2 && mPosition == 0.0f;
}

/* 026BA470 */
u32 MsgWindowScroll_l::canScrollUp() {
  if (mLineIndex != 0 || mTarget > 0.0f)
    return 1;
  return mLineCount == 2 && mPosition != 0.0f;
}

/* 026BA4B8 */
void MsgWindowScroll_l::showScrollButtons() {
  if (canScrollDown())
    showButton(0);
  else
    showMiddleButton();
  if (canScrollUp())
    showButton(2);
}

/* 026BA568 */
void MsgWindowScroll_l::waitButtonsIn() {
  bool ended = true;
  if ((mpButtonPanes[0]->mFlags & 1) && !buttonInEnded(0))
    ended = false;
  if ((mpButtonPanes[1]->mFlags & 1) && !middleInEnded())
    ended = false;
  if ((mpButtonPanes[2]->mFlags & 1) && !buttonInEnded(2))
    return;
  if (ended)
    hdMsgChangeState(&mState, &sScrollButtonWaitState);
}

/* 026BAD90 */
void MsgWindowScroll_l::waitButtonsPress() {
  bool ended = true;
  if (mpButtonPanes[0]->mFlags & 1) {
    if (!buttonPressEnded(0))
      ended = false;
    else
      mpButtonPanes[0]->mFlags = mpButtonPanes[0]->mFlags & 0xFE;
  }
  if (mpButtonPanes[1]->mFlags & 1) {
    if (!middlePressEnded())
      ended = false;
    else
      mpButtonPanes[1]->mFlags = mpButtonPanes[1]->mFlags & 0xFE;
  }
  if (mpButtonPanes[2]->mFlags & 1) {
    if (!buttonPressEnded(2))
      return;
    mpButtonPanes[2]->mFlags = mpButtonPanes[2]->mFlags & 0xFE;
  }
  if (ended)
    hdMsgChangeState(&mState, &l_scrollTextState);
}

/* 026BAFCC */
void MsgWindowScroll_l::beginClosing() {
  finishMiddleButton();
  scrollSound(0x80B);
}

/* 026BAFF4 */
void MsgWindowScroll_l::waitMiddleButton() {
  const u32 count = mButtonCount;
  gptr<hdPartsKeyIcon_c> *buttons = mpButtons;
  if (buttons[count > 1 ? 1 : 0]->isNotDeciding())
    hdMsgChangeState(&mState, &sScrollCloseState);
}

/* 026BB050 */
void MsgWindowScroll_l::startCloseAnimation() {
  mTimer = 5;
  playAnimation(1);
  scrollCloseSound();
  scrollSound(0x80D);
}

/* 026BB278 */
void MsgWindowScroll_l::stopAnimation(u32 animation, f32 frame) {
  void *player = mpAnimation->animSet();
  const u32 kind = sScrollAnimationKinds.mValues[animation];
  scrollStop(player, animation, kind, frame);
}

/* 026BA0C0 */
u32 MsgWindowScroll_l::backgroundLuma() {
  be<s16> *color = scrollBackgroundColor();
  const f32 red = color[0], green = color[1], blue = color[2];
  const f32 weightedGreen = fmuls_ppc(0.587f, green);
  const f32 weightedRed = fmadds(0.299f, red, weightedGreen);
  return u8(ftoi(fmadds(0.114f, blue, weightedRed)));
}

/* 026BA16C */
u32 MsgWindowScroll_l::openingAlphaLimit() {
  const u32 luma = backgroundLuma();
  return luma > 128 ? u8(383 - luma) : 255;
}

/* 026BA1A4 */
f32 MsgWindowScroll_l::openingAlpha(s32 duration, s32 timer) {
  const s32 clamped = timer < 0 ? 0 : (timer > duration ? duration : timer);
  const f32 progress = f32(clamped) / f32(duration);
  const f32 weight = scrollCurve(progress);
  const f32 limit = f32(openingAlphaLimit());
  return fnmsubs(fsubs_ppc(255.0f, limit), weight, 255.0f);
}

/* 026BB088 */
f32 MsgWindowScroll_l::closingAlpha(s32 duration, s32 timer) {
  const s32 clamped = timer < 0 ? 0 : (timer > duration ? duration : timer);
  const f32 time = f32(clamped), total = f32(duration);
  const f32 weight = fmuls_ppc(time, time) / fmuls_ppc(total, total);
  const f32 limit = f32(openingAlphaLimit());
  return fnmsubs(fsubs_ppc(255.0f, limit), weight, 255.0f);
}

/* 026B9EF8 */
void MsgWindowScroll_l::beginOpening() {
  scrollSetControl(scrollControlManager.mManager, 2);
  mpRoot->mFlags = (mpRoot->mFlags & 0xFE) | 1;
  for (u32 i = 0; i < 3; ++i)
    mpTextPanes[i]->mFlags = mpTextPanes[i]->mFlags & 0xFE;
  for (u32 i = 0; i < 3; ++i)
    mpButtonPanes[i]->mFlags = mpButtonPanes[i]->mFlags & 0xFE;
  hideButton(0);
  hideMiddleButton();
  hideButton(2);
  for (u32 i = 0; i < 3; ++i) {
    const u32 material = mMaterial;
    ScrollPane_l *pane = mpMaterialPanes[i];
    pane->mFlags = (pane->mFlags & 0xFE) |
                   sScrollMaterialVisible.mValues[material * 3 + i];
  }
  scrollSetLines(this, NULL);
  mTimer = 0;
  playAnimation(0);
  mPosition = 0.0f;
  positionLines();
  scrollOpenSound();
  scrollSound(0x80C);
}

/* 026BA628 */
void MsgWindowScroll_l::updateInput() {
  const u32 input = mInput;
  if (input & 3) {
    if (canScrollDown()) {
      sScrollDirection = 2;
      scrollSound(0x803);
      hdMsgChangeState(&mState, &sScrollUpState);
    } else {
      sScrollDirection = 0;
      hdMsgChangeState(&mState, &sScrollEndState);
    }
    sScrollDirection = 2;
    return;
  }
  if (input & 0x110000) {
    if (canScrollUp()) {
      sScrollDirection = canScrollDown() ? 2 : 0;
      scrollSound(0x803);
      hdMsgChangeState(&mState, &sScrollDownState);
    }
    return;
  }
  if (input & 0x220000) {
    if (canScrollDown()) {
      sScrollDirection = 2;
      scrollSound(0x803);
      hdMsgChangeState(&mState, &sScrollUpState);
    } else {
      sScrollDirection = 0;
      hdMsgChangeState(&mState, &sScrollEndState);
    }
    return;
  }
  sScrollDirection = canScrollDown() ? 1 : 4;
  const u32 line = mLineIndex;
  scrollSetLines(this, mpLines[line < 20 ? line : 0]);
}

/* 026BA884 */
void MsgWindowScroll_l::beginScrollUp() {
  mTarget = fadds_ppc(mPosition, mLineSpacing);
  for (u32 i = 0; i < 3; ++i) {
    const u32 line = mLineIndex + i;
    const bool visible =
        s32(gabi::ea((const char *)mpLines[line < 20 ? line : 0])) >= 0;
    const u32 index = mPaneIndices[i];
    ScrollPane_l *pane = mpTextPanes[index < 3 ? index : 0];
    pane->mFlags = (pane->mFlags & 0xFE) | (visible ? 1 : 0);
  }
  if (mpButtonPanes[0]->mFlags & 1)
    pressButton(0);
  if (mpButtonPanes[1]->mFlags & 1)
    pressMiddleButton();
  if (mpButtonPanes[2]->mFlags & 1)
    pressButton(2);
}

/* 026BAB78 */
void MsgWindowScroll_l::beginScrollDown() {
  mTarget = fsubs_ppc(mPosition, mLineSpacing);
  if (mpButtonPanes[0]->mFlags & 1)
    pressButton(0);
  if (mpButtonPanes[1]->mFlags & 1)
    pressMiddleButton();
  if (mpButtonPanes[2]->mFlags & 1)
    pressButton(2);
}

/* 026BAED8 */
void MsgWindowScroll_l::updateEmpty() {
  if (mInput & 0x220003) {
    const u32 count = mButtonCount;
    gptr<hdPartsKeyIcon_c> *buttons = mpButtons;
    buttons[count > 1 ? 1 : 0]->requestDecide();
    sScrollDirection = 0;
    hdMsgChangeState(&mState, &sScrollEndState);
    return;
  }
  sScrollDirection = 4;
  const u32 line = mLineIndex;
  scrollSetLines(this, mpLines[line < 20 ? line : 0]);
}

/* 026BB294 */
void MsgWindowScroll_l::reset() {
  mTarget = 0.0f;
  for (u32 i = 0; i < 20; ++i)
    mpLines[i] = (const char *)gabi::at<void>(0xFFFFFFFF);
  mLineIndex = 0;
  mpLines[0] = NULL;
  stopAnimation(0, 0.0f);
  gabi::call_ptr<void>(mpOwner->mSlots[0xB4 / 4], this);
  gabi::call_ptr<void>(mpOwner->mSlots[0x8C / 4], this);
  mpRoot->mFlags = mpRoot->mFlags & 0xFE;
}

/* 026BB330 */
void MsgWindowScroll_l::close() {
  reset();
  scrollGetPlay()->mScrollAlpha = 255;
  scrollGetPlay()->mScrollMessage = 0xFFFFFFFF;
  scrollGetPlay()->mScrollMode = 0;
  scrollGetPlay()->mScrollCounter = 0;
  hdMsgChangeState(&mState, &sScrollResetState);
}

/* 026BB3A0 */
void MsgWindowScroll_l::forceClose() {
  void *manager = scrollControlManager.mManager;
  if (scrollGetControl(manager) == 19) {
    scrollSetControl(manager, 0);
    close();
  }
}

/* 026BB614 */
void MsgWindowScroll_l::findRootPanes() {
  hdPartsPane_c *root = mpAnimation->lyt()->mpRootPane;
  if (!sScrollRootNamesGuard) {
    sScrollRootNamesGuard = 1;
    sScrollRootNames.mValues[0].mpVtable = &sScrollStringVtable;
    sScrollRootNames.mValues[0].mpText = scrollGuestText("N_All_00");
    sScrollRootNames.mValues[1].mpVtable = &sScrollStringVtable;
    sScrollRootNames.mValues[1].mpText = scrollGuestText("N_Scissor_00");
    scrollRegister(&sScrollRootRegistration);
  }
  mpRoot = scrollFindPane(root, &sScrollRootNames.mValues[0]);
  mpScissor = scrollFindPane(root, &sScrollRootNames.mValues[1]);
}

/* 026B99A8 */
static void scrollDestroyRootNames() {
  scrollDestroyArray(sScrollRootNames.mValues, 2, 8, scrollStringDestroy, 0, 0);
}

/* 026B99CC */
static void scrollDestroyTextNames() {
  scrollDestroyArray(sScrollTextNames.mValues, 3, 8, scrollStringDestroy, 0, 0);
}

/* 026B99F0 */
static void scrollDestroyMaterialNames() {
  scrollDestroyArray(sScrollMaterialNames.mValues, 3, 8, scrollStringDestroy, 0,
                     0);
}

/* 026B9A14 */
static void scrollWindowDestroy(MsgWindowScroll_l *window, u32 flags) {
  if (!window)
    return;
  window->mpStateVtable = sScrollStateVtable.mValues;
  window->mpStateOwnerVtable = sScrollStateOwnerVtable.mValues;
  scrollBaseDestroy(window, 0);
  if (flags & 1)
    scrollDelete(window);
}

/* 026BB3FC */
static MsgWindowScroll_l *scrollWindowConstruct(MsgWindowScroll_l *window) {
  if (!window) {
    window = (MsgWindowScroll_l *)scrollAllocate(sizeof(MsgWindowScroll_l));
    if (!window)
      return NULL;
  }
  scrollBaseConstruct(window);
  window->mButtonsCombined = 0;
  window->mMessageID = 0;
  window->mpOwner = (ScrollVirtuals_l *)sScrollWindowVtable.mValues;
  window->mpStateVtable = sScrollStateVtable.mValues;
  window->mButtonsHeld = 0;
  window->mMaterial = 3;
  window->mInput = 0;
  window->mpStateOwnerVtable = sScrollStateOwnerVtable.mValues;
  if (gabi::ea(&window->mpRoot) == 0)
    scrollAllocate(8);
  if (gabi::ea(window->mpTextPanes) == 0)
    scrollAllocate(12);
  if (gabi::ea(window->mPaneIndices) == 0)
    scrollAllocate(12);
  if (gabi::ea(window->mpMaterialPanes) == 0)
    scrollAllocate(12);
  if (gabi::ea(window->mpButtonPanes) == 0)
    scrollAllocate(12);
  window->mScrollMode = 0;
  window->mTimer = 0;
  if (gabi::ea(window->mLinePositions) == 0)
    scrollAllocate(12);
  window->mTextAlignment = 0;
  if (gabi::ea(&window->mUpperLimit) == 0)
    scrollAllocate(8);
  window->mTarget = 0.0f;
  window->mLineSpacing = 0.0f;
  window->mPosition = 0.0f;
  if (gabi::ea(window->mpLines) == 0)
    scrollAllocate(80);
  window->mLineIndex = 0;
  window->mLineCount = 0;
  scrollBoundsConstruct(&window->mBounds);
  window->mpMessage = NULL;
  window->mpRoot = NULL;
  window->mpScissor = NULL;
  for (u32 i = 0; i < 3; ++i)
    window->mpTextPanes[i] = NULL;
  for (u32 i = 0; i < 3; ++i)
    window->mPaneIndices[i] = 0xFFFFFFFF;
  for (u32 i = 0; i < 3; ++i)
    window->mpMaterialPanes[i] = NULL;
  for (u32 i = 0; i < 3; ++i)
    window->mpButtonPanes[i] = NULL;
  for (u32 i = 0; i < 3; ++i)
    window->mLinePositions[i] = 0.0f;
  window->mUpperLimit = 0.0f;
  window->mLowerLimit = 0.0f;
  for (u32 i = 0; i < 20; ++i)
    window->mpLines[i] = gabi::at<const char>(0xFFFFFFFF);
  return window;
}

/* WWHD glue

 ext 028ED6F8 void scrollRandomConstruct(be<u8>*)
 ext 028EAB2C void scrollUtilityConstruct(be<u8>*)
 str 100FF360 "InAll" in 026BD2DC
 str 100FF368 "OutAll" in 026BD2DC
 str 100FF3D8 "G_All_00" in 026BD2DC
 str 100FF3E4 "L_ScrollKeyIcon_00" in 026BD2DC
 str 100FF3F8 "L_KeyIcon_00" in 026BD2DC
 str 100FF408 "L_ScrollKeyIcon_01" in 026BD2DC
 str 100FF41C "ScrollKeyIcon_00" in 026BD2DC
 str 100FF430 "KeyIcon_00" in 026BD2DC
 str 100FF43C "MsgWindowScroll_00" in 026BD2DC
 str 100FF450 "::StateID_Open"
 str 100FF460 "::StateID_BeforeStop"
 str 100FF478 "::StateID_Stop"
 str 100FF488 "::StateID_ScrollUp"
 str 100FF49C "::StateID_ScrollDown"
 str 100FF394 "::StateID_AfterScroll"
 str 100FF380 "::StateID_CloseWait"
 str 100FF3AC "::StateID_KeyIconDecideAnimEndWaitToClose"
 str 100FF370 "::StateID_Close"
 str 100FF4B4 "::StateID_Delete"
 str 100FF338 "KeyIcon_00"
 str 100FF344 "ScrollKeyIcon_00"
 ext 0273B050 void* scrollAllocateFromHeap(u32,void*,u32)
 ext 02674164 hdPartsKeyIcon_c* scrollKeyIconConstruct(void*)
 ext 026D39B0 hdPartsKeyIcon_c* scrollScrollKeyIconConstruct(void*)
 ext 026741D0 u32 scrollKeyIconCreate(hdPartsKeyIcon_c*,u32,u32,ScrollResource_l*)
 ext 026D3A1C u32 scrollScrollKeyIconCreate(hdPartsKeyIcon_c*,u32,u32,ScrollResource_l*)
 ext 026F92C0 u32 scrollBaseCreateIcon(MsgWindowScroll_l*,u32,ScrollResource_l*)
 ext 026FADB0 hdPartsLayout_c* scrollLayoutConstruct(void*)
 ext 02004E04 void scrollBindAnimation(void*,u32,ScrollString_l*,ScrollString_l*)
 ext 026F90D8 void scrollBaseCreate(MsgWindowScroll_l*,ScrollString_l*,u32)
 ext 025B8B94 u32 scrollEventBit(void*,u32)
 ext 025F6B18 void scrollMessageLookup(ScrollMessage_l*,u32,u32)
 data sScrollStateVtable 100FF680
 data sScrollStateOwnerVtable 100FF690
 data sScrollWindowVtable 100FF4C8
 ext 028F0164 void scrollDestroyArray(ScrollString_l*,u32,u32,ScrollStringDestructor_l,u32,u32)
 ext 026BD890 void scrollStringDestroy(ScrollString_l*,u32) callback
 ext 026F88F0 void scrollBaseDestroy(MsgWindowScroll_l*,u32)
 ext 026F89E8 MsgWindowScroll_l* scrollBaseConstruct(MsgWindowScroll_l*)
 ext 0273AD10 void* scrollAllocate(u32)
 ext 0273AF40 void scrollDelete(void*)
 ext 0274F358 void scrollBoundsConstruct(ScrollBounds_l*)
 ext 02739398 u32 renderTvWidth()
 ext 027393E8 u32 renderTvHeight()
 ext 02739594 u32 renderTvTargetWidth()
 ext 027395E4 u32 renderTvTargetHeight()
 ext 02006478 hdPartsStateId_c* scrollCurrentState(HdMsgController*)
 ext 02006364 void scrollStateUpdate(HdMsgController*)
 ext 0274F5F0 void scrollViewportSave(ScrollBounds_l*,void*)
 ext 0274FA70 void scrollViewportApply(ScrollBounds_l*,void*)
 data sScrollStringVtable 100FF1E0
 data sScrollRootNames 10498AFC
 data sScrollTextNames 10498B0C
 data sScrollMaterialNames 10498B24
 data sScrollRootNamesGuard 10498BA4
 data sScrollRootRegistration 101F6708
 str 100FF2C4 "N_All_00"
 str 100FF2D0 "N_Scissor_00"
 str 100FF2E8 "T_Msg_00"
 str 100FF2F4 "T_Msg_01"
 str 100FF300 "T_Msg_02"
 str 100FF30C "P_Wood_00"
 str 100FF318 "P_Paper_00"
 str 100FF324 "P_Stone_00"
 ext 028F026C void scrollRegister(ScrollRegistration_l*)
 ext 02704AFC void scrollSetPaneCapacity(ScrollPane_l*,u32)
 ext 026F91F8 void scrollBaseDraw(MsgWindowScroll_l*)
 data sScrollDirection 1047B09E
 data sScrollEndState 10498D08
 data sScrollUpState 10498C48
 data sScrollDownState 10498C78
 data sScrollResetState 1049DD60
 data sScrollMaterialVisible 100FF290
 data scrollControlManager 101F4B5C
 ext 026B9BD8 void scrollSetLines(MsgWindowScroll_l*,const char*)
 ext 025E1C58 void scrollOpenSound()
 ext 025F74D0 void scrollSetControl(void*,u32)
 ext 025F795C u32 scrollGetControl(void*)
 ext 025200D4 ScrollPlay_l* scrollGetPlay()
 data sScrollAnimationKinds 100FF1D0
 data sScrollButtonWaitState 10498C18
 data sScrollCloseState 10498D38
 ext 026F826C void hdPartsKeyIcon_c::hide()
 ext 026F8228 void hdPartsKeyIcon_c::requestIn()
 ext 026F8260 void hdPartsKeyIcon_c::requestOut()
 ext 026F8238 void hdPartsKeyIcon_c::requestDecide()
 ext 026F82CC u32 hdPartsKeyIcon_c::isWaiting()
 ext 026F83B4 u32 hdPartsKeyIcon_c::isHidden()
 ext 026F833C u8 hdPartsKeyIcon_c::isNotDeciding()
 ext 025F6FCC void scrollSetText(ScrollMessage_l*,const char*,u32)
 ext 02704504 void scrollSetPaneText(ScrollPane_l*,ScrollMessage_l*,u32)
 ext 0200579C void scrollSetFrame(void*,u32,u32,f32)
 ext 020053E4 void scrollPlay(void*,u32,u32,f32)
 ext 0200552C void scrollStop(void*,u32,u32,f32)
 ext 025E1988 void scrollSound(u32)
 ext 025E1C64 void scrollCloseSound()
 ext 0255F2F8 be<s16>* scrollBackgroundColor()
 ext 028F4384 f32 scrollCurve(f32)
*/

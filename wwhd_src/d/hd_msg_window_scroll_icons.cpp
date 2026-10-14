/* HD scrolling message window controls and animation helpers. */
#include "d/hd_msg_window_scroll_icons.h"
#include "d/hd_msg_window_base.h"
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

extern void scrollSetLines(MsgWindowScroll_l *, const char *);
extern void scrollOpenSound();

extern void scrollSetControl(void *, u32);
extern u32 scrollGetControl(void *);
extern ScrollPlay_l *scrollGetPlay();

static ScrollInputGlobal_l sIconScrollInput;
static ScrollControlGlobal_l sIconScrollControl;
extern void scrollBaseDraw(MsgWindowScroll_l *);

extern void scrollSetPaneCapacity(ScrollPane_l *, u32);
static ScrollStringVirtual_l sIconScrollStringVtable;

static SScrollButtonNamesView_l sIconScrollButtonNames;

struct ScrollRegistration_l {
  u8 mOpaque[12];
};

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
/* inlined into message window layer drawing */
static void scrollHidePanes(gptr<ScrollPane_l> *panes, be<u8> *visibility) {
  for (u32 i = 0; i < 3; ++i) {
    visibility[i] = panes[i]->mFlags & 1;
    panes[i]->mFlags = panes[i]->mFlags & 0xFE;
  }
}
/* inlined into message window layer drawing */
static void scrollRestorePanes(gptr<ScrollPane_l> *panes, be<u8> *visibility) {
  for (u32 i = 0; i < 3; ++i) {
    ScrollPane_l *pane = panes[i];
    pane->mFlags = (pane->mFlags & 0xFE) | visibility[i];
  }
}
extern void scrollViewportSave(ScrollBounds_l *, void *);
extern void scrollViewportApply(ScrollBounds_l *, void *);

extern u32 renderTvWidth();
extern u32 renderTvHeight();
extern u32 renderTvTargetWidth();
extern u32 renderTvTargetHeight();
extern hdPartsStateId_c *scrollCurrentState(HdMsgController *);
extern void scrollStateUpdate(HdMsgController *);
static hdPartsStateId_c sIconScrollClosedStateExtra;
/* inlined into scrolling message window state checks */
static bool scrollIsClosed(MsgWindowScroll_l *window) {
  hdPartsStateId_c *state = scrollCurrentState(&window->mState);
  hdPartsStateIdVtbl_c *closedVtable = sIconScrollClosedStateExtra.mVtable;
  const u32 id = state->getKey();
  return id == gabi::call_ptr<u32>(closedVtable->mGetKey.get(), &sIconScrollClosedStateExtra);
}

extern void scrollDestroyArray(ScrollString_l *, u32, u32,
                               ScrollStringDestructor_l, u32, u32);
extern void scrollStringDestroy(ScrollString_l *, u32);
extern void scrollBaseDestroy(MsgWindowScroll_l *, u32);
extern MsgWindowScroll_l *scrollBaseConstruct(MsgWindowScroll_l *);
extern void *scrollAllocate(u32);
extern void scrollDelete(void *);
extern void scrollBoundsConstruct(ScrollBounds_l *);

extern void scrollBaseCreate(MsgWindowScroll_l *, ScrollString_l *, u32);
static SScrollLayoutNamesView_l sIconScrollLayoutNames;
static ScrollString_l sIconScrollLayoutName;
static hdPartsStateId_c sIconScrollOpeningState;
static ScrollSaveGlobal_l sIconScrollSave;
extern u32 scrollEventBit(void *, u32);
extern void scrollMessageLookup(ScrollMessage_l *, u32, u32);

static ScrollHeapGlobal_l sIconScrollHeap;
static ScrollString_l sIconScrollKeyIconName;
static ScrollString_l sIconScrollScrollKeyIconName;

static u32 sIconScrollKeyIconGuard;
static u32 sIconScrollScrollKeyIconGuard;
static ScrollRegistration_l sIconScrollKeyIconRegistration;
static ScrollRegistration_l sIconScrollScrollKeyIconRegistration;
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
/* inlined into scrolling message window icon creation */
static bool scrollStringEqual(ScrollString_l *value, ScrollString_l *expected) {
  gabi::call_ptr<void>(value->mpVtable->mpEnsure, value);
  gabi::call_ptr<void>(value->mpVtable->mpEnsure, value);
  const char *first = value->mpText;
  gabi::call_ptr<void>(expected->mpVtable->mpEnsure, expected);
  const char *second = expected->mpText;
  if (first == second)
    return true;
  const be<u8> *left = (const be<u8> *)(const char *)value->mpText;
  const be<u8> *right = (const be<u8> *)(const char *)expected->mpText;
  for (u32 i = 0; i < 0x40001; ++i) {
    const u32 a = left[i], b = right[i];
    if (a != b)
      return false;
    if (!a)
      return true;
  }
  return false;
}

extern void scrollRandomConstruct(be<u8> *);
extern void scrollUtilityConstruct(be<u8> *);

/* 026BC0E4 */
void MsgWindowScroll_l::clearInputAndDraw() {
  sIconScrollControl.mpControl->mInputDisabled = 0;
  gabi::call_ptr<void>(mpOwner->mSlots[0xAC / 4], this);
  gabi::call_ptr<void>(mpOwner->mSlots[0x84 / 4], this);
  scrollBaseDraw(this);
}

/* 026BC148 */
void MsgWindowScroll_l::readInput() {
  ScrollInputPointer_l *receiver = sIconScrollInput.mpReceiver;
  if (!receiver)
    return;
  ScrollInput_l *input = receiver->mpInput;
  if (!input)
    return;
  mButtonsHeld = input->mButtonsHeld;
  mInput = input->mButtonsDown;
  const u32 down = input->mButtonsDown;
  const u32 repeat = input->mButtonsRepeat;
  mButtonsCombined = down | repeat;
}

/* 026BC188 */
void MsgWindowScroll_l::fadeLines() {
  const f32 spacing = mLineSpacing;
  const f32 position = mPosition;
  for (u32 i = 0; i < 3; ++i) {
    const u32 index = mPaneIndices[i];
    f32 distance = fadds_ppc(position, mLinePositions[index < 3 ? index : 0]);
    f32 ratio;
    if (distance > spacing)
      ratio = std::fabs(spacing) / spacing;
    else {
      if (distance < -spacing)
        distance = -spacing;
      ratio = std::fabs(distance) / spacing;
    }
    const u8 alpha = u8(ftoi(fmadds(205.0f, fsubs_ppc(1.0f, ratio), 50.0f)));
    const u32 paneIndex = mPaneIndices[i];
    mpTextPanes[paneIndex < 3 ? paneIndex : 0]->mAlpha = alpha;
  }
}

/* 026BBC58 */
void MsgWindowScroll_l::findButtonPanes() {
  hdPartsPane_c *root = mpAnimation->lyt()->mpRootPane;
  for (u32 i = 0; i < 3; ++i)
    mpButtonPanes[i] = scrollFindPane(root, &sIconScrollButtonNames.mValues[i]);
}

/* 026BC3BC */
void MsgWindowScroll_l::drawTextLayer(u32 target) {
  ScrollVisibility_l saved;
  scrollHidePanes(mpTextPanes, saved.mFirst);
  scrollHidePanes(mpButtonPanes, saved.mSecond);
  hdPartsLayout_c *layout = mpAnimation;
  if (layout)
    gabi::call_ptr<void>(layout->vtbl()->mDraw.get(), layout, target);
  scrollRestorePanes(mpButtonPanes, saved.mSecond);
  scrollRestorePanes(mpTextPanes, saved.mFirst);
}

/* 026BC8E0 */
void MsgWindowScroll_l::drawButtonLayer(u32 target) {
  ScrollVisibility_l saved;
  scrollHidePanes(mpTextPanes, saved.mFirst);
  scrollHidePanes(mpMaterialPanes, saved.mSecond);
  hdPartsLayout_c *layout = mpAnimation;
  if (layout)
    gabi::call_ptr<void>(layout->vtbl()->mDraw.get(), layout, target);
  scrollRestorePanes(mpMaterialPanes, saved.mSecond);
  scrollRestorePanes(mpTextPanes, saved.mFirst);
}

/* 026BC634 */
void MsgWindowScroll_l::drawMaterialLayer(ScrollDrawContext_l *context,
                                          u32 target) {
  ScrollVisibility_l saved;
  scrollHidePanes(mpMaterialPanes, saved.mFirst);
  scrollHidePanes(mpButtonPanes, saved.mSecond);
  void *viewport = context->mpViewport;
  ScrollBounds_l oldBounds;
  scrollViewportSave(&oldBounds, viewport);
  scrollViewportApply(&mBounds, viewport);
  hdPartsLayout_c *layout = mpAnimation;
  if (layout)
    gabi::call_ptr<void>(layout->vtbl()->mDraw.get(), layout, target);
  scrollViewportApply(&oldBounds, viewport);
  scrollRestorePanes(mpButtonPanes, saved.mSecond);
  scrollRestorePanes(mpMaterialPanes, saved.mFirst);
}

/* 026BBD34 */
void MsgWindowScroll_l::updateScissor() {
  ScrollMatrix_l matrix;
  ScrollPane_l *scissor = mpScissor;
  for (u32 i = 0; i < 12; ++i)
    matrix.mWords[i] = scissor->mMatrix[i];
  scissor = mpScissor;
  ScrollLayoutDimensions_l *info = reinterpret_cast<ScrollLayoutDimensions_l *>(mpAnimation->lyt());
  const u32 display = sIconScrollInput.mpReceiver->mDisplay;
  f32 width = scissor->mWidth;
  f32 height = scissor->mHeight;
  const f32 layoutWidth = info->mWidth, layoutHeight = info->mHeight;
  u32 screenWidth, screenHeight;
  if (display == 2) {
    screenWidth = renderTvTargetWidth();
    screenHeight = renderTvTargetHeight();
  } else {
    screenWidth = renderTvWidth();
    screenHeight = renderTvHeight();
  }
  const f32 viewWidth = f32(screenWidth), viewHeight = f32(screenHeight);
  width = fmuls_ppc(width, viewWidth / layoutWidth);
  height = fmuls_ppc(height, viewHeight / layoutHeight);
  const f32 x = fmadds(viewWidth, 0.5f, matrix.mX);
  const f32 y = fmadds(viewHeight, 0.5f, matrix.mY);
  const f32 halfWidth = fmuls_ppc(width, 0.5f),
            halfHeight = fmuls_ppc(height, 0.5f);
  const f32 left = fsubs_ppc(x, halfWidth), right = fadds_ppc(x, halfWidth);
  const f32 bottom = fsubs_ppc(y, halfHeight), top = fadds_ppc(y, halfHeight);
  if (left < right) {
    mBounds.mLeft = left;
    mBounds.mRight = right;
  } else {
    mBounds.mLeft = right;
    mBounds.mRight = left;
  }
  if (bottom < top) {
    mBounds.mBottom = bottom;
    mBounds.mTop = top;
  } else {
    mBounds.mBottom = top;
    mBounds.mTop = bottom;
  }
}

/* 026BC2DC */
void MsgWindowScroll_l::update() {
  const u32 mode = sIconScrollControl.mpControl->mMessageMode;
  if (mode != 2 && mode != 3 && mode != 4)
    return;
  if (scrollIsClosed(this))
    return;
  readInput();
  scrollStateUpdate(&mState);
  fadeLines();
  gabi::call_ptr<void>(mpOwner->mSlots[0xB4 / 4], this);
  gabi::call_ptr<void>(mpOwner->mSlots[0x8C / 4], this);
  updateScissor();
}

/* 026BCB58 */
void MsgWindowScroll_l::draw(ScrollDrawContext_l *context, u32 target) {
  if (scrollIsClosed(this))
    return;
  drawTextLayer(target);
  drawMaterialLayer(context, target);
  drawButtonLayer(target);
}

/* 026BBFAC */
u32 MsgWindowScroll_l::create(u32 resource, u32 heap, u32 option) {
  sIconScrollControl.mpControl->mInputDisabled = gabi::ea(this);
  scrollBaseCreate(this, sIconScrollLayoutNames.mValues, 3);
  const u32 layoutMode = mStateLayoutMode;
  if (!gabi::call_ptr<u32>(mpOwner->mSlots[0x7C / 4], this, &sIconScrollLayoutName,
                           layoutMode, resource, heap, option))
    return 0;
  if (!gabi::call_ptr<u32>(mpOwner->mSlots[0xA4 / 4], this))
    return 0;
  findRootPanes();
  findTextPanes();
  findMaterialPanes();
  findButtonPanes();
  mpMessage = &sIconScrollControl.mpControl->mMessage;
  mTextAlignment = mpTextPanes[0]->mAlignment & 3;
  updateScissor();
  reset();
  hdStateOwner *owner = mpStateOwner;
  hdStateObject *node = owner->createState(reinterpret_cast<hdStateId *>(&sIconScrollClosedStateExtra));
  mpCurrentState = node;
  node->enter();
  return 1;
}

/* 026BCBF0 */
u32 MsgWindowScroll_l::setMessage(u32 material, u32 unused, u32 messageID) {
  hdPartsStateId_c *state = scrollCurrentState(&mState);
  hdPartsStateIdVtbl_c *openingVtable = sIconScrollOpeningState.mVtable;
  const u32 current = state->getKey();
  if (current ==
      gabi::call_ptr<u32>(openingVtable->mGetKey.get(), &sIconScrollOpeningState))
    return 0;
  if (messageID == 0x33A &&
      scrollEventBit(&sIconScrollSave.mpSave->mEvents, 0x2D80))
    messageID = 0x33B;
  mMessageID = messageID;
  mMaterial = material;
  const WindowMessage *message =
      reinterpret_cast<const WindowMessage *>(mpMessage.get());
  const be<u8> *textStyle =
      reinterpret_cast<const be<u8> *>(message->mpInput.get());
  u32 alignment = mTextAlignment;
  if (textStyle && (textStyle[0] == 'A' || textStyle[0] == '_'))
    alignment = 1;
  for (u32 i = 0; i < 3; ++i) {
    const u32 index = mPaneIndices[i];
    ScrollPane_l *pane = mpTextPanes[index < 3 ? index : 0];
    const u32 old = pane->mAlignment & 3;
    const bool changed = old != alignment;
    pane->mTextFlags = pane->mTextFlags | (changed ? 0x20 : 0);
    if (changed)
      pane->mAlignment = (pane->mAlignment & 0xFC) | alignment;
  }
  scrollMessageLookup(mpMessage, messageID, 0);
  mpMessage->mNewText = 1;
  hdMsgChangeState(&mState, (HdMsgState *)&sIconScrollOpeningState);
  return 1;
}

/* 026BCDC4 */
u32 MsgWindowScroll_l::createIcons(ScrollResource_l *resource) {
  if (!sIconScrollKeyIconGuard) {
    sIconScrollKeyIconGuard = 1;
    sIconScrollKeyIconName.mpVtable = &sIconScrollStringVtable;
    sIconScrollKeyIconName.mpText = scrollGuestText("KeyIcon_00");
    scrollRegister(&sIconScrollKeyIconRegistration);
  }
  if (!sIconScrollScrollKeyIconGuard) {
    sIconScrollScrollKeyIconGuard = 1;
    sIconScrollScrollKeyIconName.mpVtable = &sIconScrollStringVtable;
    sIconScrollScrollKeyIconName.mpText = scrollGuestText("ScrollKeyIcon_00");
    scrollRegister(&sIconScrollScrollKeyIconRegistration);
  }
  void *heap = sIconScrollHeap.mpManager->mpHeap;
  for (u32 i = 0; i < 3; ++i) {
    if (!scrollStringEqual(&resource->mPane, &sIconScrollButtonNames.mValues[i]))
      continue;
    bool scrollIcon =
        scrollStringEqual(&resource->mLayout, &sIconScrollScrollKeyIconName);
    bool keyIcon = false;
    if (!scrollIcon)
      keyIcon = scrollStringEqual(&resource->mLayout, &sIconScrollKeyIconName);
    if (!scrollIcon && !keyIcon)
      return scrollBaseCreateIcon(this, i, resource);
    const u32 count = mButtonCount;
    gptr<hdPartsKeyIcon_c> *slot =
        &((gptr<hdPartsKeyIcon_c> *)mpButtons)[i < count ? i : 0];
    void *allocation = scrollAllocateFromHeap(0x58, heap, 4);
    hdPartsKeyIcon_c *button = (hdPartsKeyIcon_c *)allocation;
    if (button)
      button = scrollIcon ? scrollScrollKeyIconConstruct(allocation)
                          : scrollKeyIconConstruct(allocation);
    *slot = button;
    const u32 reloadCount = mButtonCount;
    gptr<hdPartsKeyIcon_c> *buttons = mpButtons;
    if (!buttons[i < reloadCount ? i : 0])
      return 0;
    const u32 finalCount = mButtonCount;
    buttons = mpButtons;
    button = buttons[i < finalCount ? i : 0];
    const u32 created = scrollIcon
                            ? scrollScrollKeyIconCreate(button, 0, 0, resource)
                            : scrollKeyIconCreate(button, 0, 0, resource);
    if (!created)
      return 0;
  }
  return 1;
}

/* WWHD glue

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
 data sIconScrollHeap 1018C404
 data sIconScrollKeyIconName 10498AD8
 data sIconScrollScrollKeyIconName 10498AE0
 data sIconScrollKeyIconGuard 10498BB0
 data sIconScrollScrollKeyIconGuard 10498BB4
 data sIconScrollKeyIconRegistration 101F672C
 data sIconScrollScrollKeyIconRegistration 101F6738
 str 100FF338 "KeyIcon_00"
 str 100FF344 "ScrollKeyIcon_00"
 data sIconScrollLayoutNames 10498B8C
 data sIconScrollLayoutName 10498D98
 data sIconScrollOpeningState 10498BB8
 data sIconScrollSave 101F84DC
 data sIconScrollClosedStateExtra 1049DD60
 data sIconScrollStringVtable 100FF1E0
 data sIconScrollButtonNames 10498B5C
 str 100FF2C4 "N_All_00"
 str 100FF2D0 "N_Scissor_00"
 str 100FF2E8 "T_Msg_00"
 str 100FF2F4 "T_Msg_01"
 str 100FF300 "T_Msg_02"
 str 100FF30C "P_Wood_00"
 str 100FF318 "P_Paper_00"
 str 100FF324 "P_Stone_00"
 data sIconScrollInput 101F5088
 data sIconScrollControl 101F4B5C
*/

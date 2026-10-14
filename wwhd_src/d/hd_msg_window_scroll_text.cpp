/* HD scrolling message window controls and animation helpers. */
#include "d/hd_msg_window_scroll_text.h"
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

extern void scrollBaseDraw(MsgWindowScroll_l *);

extern void scrollSetPaneCapacity(ScrollPane_l *, u32);
static ScrollStringVirtual_l sTextScrollStringVtable;

static SScrollTextNamesView_l sTextScrollTextNames;

static u32 sTextScrollTextNamesGuard;

struct ScrollRegistration_l {
  u8 mOpaque[12];
};

static ScrollRegistration_l sTextScrollTextRegistration;

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

/* 026BB768 */
void MsgWindowScroll_l::findTextPanes() {
  hdPartsPane_c *root = mpAnimation->lyt()->mpRootPane;
  if (!sTextScrollTextNamesGuard) {
    sTextScrollTextNamesGuard = 1;
    sTextScrollTextNames.mValues[0].mpVtable = &sTextScrollStringVtable;
    sTextScrollTextNames.mValues[0].mpText = scrollGuestText("T_Msg_00");
    sTextScrollTextNames.mValues[1].mpVtable = &sTextScrollStringVtable;
    sTextScrollTextNames.mValues[1].mpText = scrollGuestText("T_Msg_01");
    sTextScrollTextNames.mValues[2].mpVtable = &sTextScrollStringVtable;
    sTextScrollTextNames.mValues[2].mpText = scrollGuestText("T_Msg_02");
    scrollRegister(&sTextScrollTextRegistration);
  }
  for (u32 i = 0; i < 3; ++i) {
    mpTextPanes[i] = scrollFindPane(root, &sTextScrollTextNames.mValues[i]);
    if (mpTextPanes[i]) {
      scrollSetPaneCapacity(mpTextPanes[i], 0x14F);
      mpTextPanes[i]->mFlags = mpTextPanes[i]->mFlags & 0xFE;
    }
    mLinePositionBits[i] = mpTextPanes[i]->mTranslationYBits;
  }
  for (u32 i = 0; i < 3; ++i)
    mPaneIndices[i] = i;
  for (u32 i = 0; i < 3; ++i) {
    const u32 index = mPaneIndices[i];
    if (mpTextPanes[index < 3 ? index : 0]) {
      const u32 reload = mPaneIndices[i];
      ScrollPane_l *pane = mpTextPanes[reload < 3 ? reload : 0];
      pane->mFlags = (pane->mFlags & 0xFE) | 1;
    }
  }
  const u32 index = mPaneIndices[0];
  if (mpTextPanes[index < 3 ? index : 0]) {
    const u32 reload = mPaneIndices[0];
    ScrollPane_l *pane = mpTextPanes[reload < 3 ? reload : 0];
    const f32 height = pane->mHeight;
    mLineSpacingBits = pane->mHeightBits;
    mUpperLimit = fmuls_ppc(height, 1.5f);
    mLowerLimit = fmuls_ppc(height, 0.5f);
  }
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
 str 100FF338 "KeyIcon_00"
 str 100FF344 "ScrollKeyIcon_00"
 data sTextScrollStringVtable 100FF1E0
 data sTextScrollTextNames 10498B0C
 data sTextScrollTextNamesGuard 10498BA8
 data sTextScrollTextRegistration 101F6714
 str 100FF2C4 "N_All_00"
 str 100FF2D0 "N_Scissor_00"
 str 100FF2E8 "T_Msg_00"
 str 100FF2F4 "T_Msg_01"
 str 100FF300 "T_Msg_02"
 str 100FF30C "P_Wood_00"
 str 100FF318 "P_Paper_00"
 str 100FF324 "P_Stone_00"
*/

/* HD scrolling message window controls and animation helpers. */
#include "d/hd_msg_window_scroll_layout.h"
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
static SScrollAnimationKindsView_l sLayoutScrollAnimationKinds;

extern void scrollSetLines(MsgWindowScroll_l *, const char *);
extern void scrollOpenSound();

extern void scrollSetControl(void *, u32);
extern u32 scrollGetControl(void *);
extern ScrollPlay_l *scrollGetPlay();

extern void scrollBaseDraw(MsgWindowScroll_l *);

extern void scrollSetPaneCapacity(ScrollPane_l *, u32);
static ScrollStringVirtual_l sLayoutScrollStringVtable;

static SScrollButtonNamesView_l sLayoutScrollButtonNames;

struct ScrollRegistration_l {
  u8 mOpaque[12];
};

extern void scrollRegister(ScrollRegistration_l *);

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
static SScrollLayoutNamesView_l sLayoutScrollLayoutNames;
static ScrollString_l sLayoutScrollLayoutName;

extern u32 scrollEventBit(void *, u32);
extern void scrollMessageLookup(ScrollMessage_l *, u32, u32);

static ScrollHeapGlobal_l sLayoutScrollHeap;

static SScrollAnimationNamesView_l sLayoutScrollAnimationNames;
static SScrollIconPaneNamesView_l sLayoutScrollIconPaneNames;

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

static SScrollIconResourceNamesView_l sLayoutScrollIconResourceNames;
static SScrollInitializerWordsView_l sLayoutScrollInitializerWords;
static ScrollRegistration_l sLayoutScrollInitRegistration;
static ScrollRegistration_l sLayoutScrollRandomRegistration;
static ScrollRegistration_l sLayoutScrollUtilityRegistration;
static f32 sLayoutScrollNegativePi;
static f32 sLayoutScrollPositivePi;
static u8 sLayoutScrollRandom;
static u8 sLayoutScrollUtility;
static u32 sLayoutScrollStateCounterGuard;
static u32 sLayoutScrollStateCounter;
static SScrollDescriptorVtableView_l sLayoutScrollDescriptorVtable;
static SScrollDescriptorOwnerView_l sLayoutScrollDescriptorOwner;
static hdPartsStateId_c sScrollOpenState;
static hdPartsStateId_c sScrollStopState;
static hdPartsStateId_c sScrollScrollUpState;
static hdPartsStateId_c sScrollScrollDownState;
static hdPartsStateId_c sScrollKeyIconDecideWaitState;
static hdPartsStateId_c sScrollCloseStateRecord;
extern void scrollRandomConstruct(be<u8> *);
extern void scrollUtilityConstruct(be<u8> *);
/* inlined into scrolling message window static initialization */
static void scrollInitializeState(hdPartsStateId_c *state, u32 id,
                                  const char *name, u32 firstSlot) {
  state->mNumber = id;
  state->mName = scrollGuestText(name);
  state->mVtable = reinterpret_cast<hdPartsStateIdVtbl_c *>(sLayoutScrollDescriptorVtable.mValues);
  state->mEnter.setVirtual(firstSlot);
  state->mExecute.setVirtual(firstSlot + 1);
  state->mExit.setVirtual(firstSlot + 2);
  state->mUnused.clear();
  state->mpNullStateId = reinterpret_cast<hdStateId *>(sLayoutScrollDescriptorOwner.mValues);
}

/* 026BD1B0 */
u32 MsgWindowScroll_l::createLayout(void *resource, u32 heap, u32 option,
                                    u32 flags, void *parent) {
  void *allocation =
      scrollAllocateFromHeap(0x110, sLayoutScrollHeap.mpManager->mpHeap, 4);
  hdPartsLayout_c *layout = (hdPartsLayout_c *)allocation;
  if (layout)
    layout = scrollLayoutConstruct(allocation);
  mpAnimation = layout;
  if (!layout)
    return 0;
  if (!layout->vLoad(resource, heap, option, flags, 2u, 1u, parent))
    return 0;
  for (u32 i = 0; i < 2; ++i) {
    const u32 kind = sLayoutScrollAnimationKinds.mValues[i];
    scrollBindAnimation(mpAnimation->animSet(), i,
                        &sLayoutScrollIconPaneNames.mValues[i],
                        &sLayoutScrollAnimationNames.mValues[kind]);
  }
  gabi::call_ptr<void>(mpOwner->mSlots[0x8C / 4], this);
  return 1;
}

/* 026BD2DC */
static void scrollStaticInitialize() {
  sLayoutScrollInitializerWords.mValues[3] = 0;
  sLayoutScrollInitializerWords.mValues[2] = 0;
  sLayoutScrollInitializerWords.mValues[1] = 0;
  sLayoutScrollInitializerWords.mValues[0] = 0;
  scrollRegister(&sLayoutScrollInitRegistration);
  sLayoutScrollNegativePi = -3.1415927410125732f;
  sLayoutScrollPositivePi = 3.1415927410125732f;
  scrollRandomConstruct(&sLayoutScrollRandom);
  scrollRegister(&sLayoutScrollRandomRegistration);
  scrollUtilityConstruct(&sLayoutScrollUtility);
  scrollRegister(&sLayoutScrollUtilityRegistration);
  sLayoutScrollAnimationNames.mValues[0].mpVtable = &sLayoutScrollStringVtable;
  sLayoutScrollAnimationNames.mValues[0].mpText = scrollGuestText("G_All_00");
  sLayoutScrollIconPaneNames.mValues[0].mpVtable = &sLayoutScrollStringVtable;
  sLayoutScrollIconPaneNames.mValues[0].mpText = scrollGuestText("InAll");
  sLayoutScrollIconPaneNames.mValues[1].mpVtable = &sLayoutScrollStringVtable;
  sLayoutScrollIconPaneNames.mValues[1].mpText = scrollGuestText("OutAll");
  sLayoutScrollButtonNames.mValues[0].mpVtable = &sLayoutScrollStringVtable;
  sLayoutScrollButtonNames.mValues[0].mpText = scrollGuestText("L_ScrollKeyIcon_00");
  sLayoutScrollButtonNames.mValues[1].mpVtable = &sLayoutScrollStringVtable;
  sLayoutScrollButtonNames.mValues[1].mpText = scrollGuestText("L_KeyIcon_00");
  sLayoutScrollButtonNames.mValues[2].mpVtable = &sLayoutScrollStringVtable;
  sLayoutScrollButtonNames.mValues[2].mpText = scrollGuestText("L_ScrollKeyIcon_01");
  sLayoutScrollIconResourceNames.mValues[0].mpVtable = &sLayoutScrollStringVtable;
  sLayoutScrollIconResourceNames.mValues[0].mpText = scrollGuestText("ScrollKeyIcon_00");
  sLayoutScrollIconResourceNames.mValues[1].mpVtable = &sLayoutScrollStringVtable;
  sLayoutScrollIconResourceNames.mValues[1].mpText = scrollGuestText("KeyIcon_00");
  sLayoutScrollIconResourceNames.mValues[2].mpVtable = &sLayoutScrollStringVtable;
  sLayoutScrollIconResourceNames.mValues[2].mpText = scrollGuestText("ScrollKeyIcon_00");
  sLayoutScrollLayoutNames.mValues[0].mpVtable = &sLayoutScrollStringVtable;
  sLayoutScrollLayoutNames.mValues[0].mpText = scrollGuestText("MsgWindowScroll_00");
  sLayoutScrollLayoutNames.mValues[1].mpVtable = &sLayoutScrollStringVtable;
  sLayoutScrollLayoutNames.mValues[1].mpText = scrollGuestText("ScrollKeyIcon_00");
  sLayoutScrollLayoutNames.mValues[2].mpVtable = &sLayoutScrollStringVtable;
  sLayoutScrollLayoutNames.mValues[2].mpText = scrollGuestText("KeyIcon_00");
  sLayoutScrollLayoutName.mpVtable = &sLayoutScrollStringVtable;
  sLayoutScrollLayoutName.mpText = scrollGuestText("MsgWindowScroll_00");
  u32 id = 0;
  if (sLayoutScrollStateCounterGuard)
    id = sLayoutScrollStateCounter;
  else
    sLayoutScrollStateCounterGuard = 1;
  scrollInitializeState(&sScrollOpenState, ++id, "::StateID_Open",
                        25);
  scrollInitializeState(hdMsgStateId(&l_scrollTextState), ++id,
                        "::StateID_BeforeStop", 28);
  scrollInitializeState(&sScrollStopState, ++id, "::StateID_Stop",
                        31);
  scrollInitializeState(&sScrollScrollUpState, ++id,
                        "::StateID_ScrollUp", 34);
  scrollInitializeState(&sScrollScrollDownState, ++id,
                        "::StateID_ScrollDown", 37);
  scrollInitializeState(hdMsgStateId(&l_scrollWaitState), ++id,
                        "::StateID_AfterScroll", 40);
  scrollInitializeState(hdMsgStateId(&l_scrollEmptyState), ++id,
                        "::StateID_CloseWait", 43);
  scrollInitializeState(&sScrollKeyIconDecideWaitState, ++id,
                        "::StateID_KeyIconDecideAnimEndWaitToClose", 46);
  scrollInitializeState(&sScrollCloseStateRecord, ++id,
                        "::StateID_Close", 49);
  scrollInitializeState(hdMsgStateId(&l_scrollClosedState), ++id,
                        "::StateID_Delete", 52);
  sLayoutScrollStateCounter = id;
}

/* WWHD glue

 data sLayoutScrollIconResourceNames 10498B74
 data sLayoutScrollInitializerWords 10498B3C
 data sLayoutScrollInitRegistration 101F6744
 data sLayoutScrollRandomRegistration 101F6750
 data sLayoutScrollUtilityRegistration 101F675C
 data sLayoutScrollNegativePi 10498AF0
 data sLayoutScrollPositivePi 10498AF4
 data sLayoutScrollRandom 10498AF8
 data sLayoutScrollUtility 10498AF9
 data sLayoutScrollStateCounterGuard 101FD8E4
 data sLayoutScrollStateCounter 101FDD50
 data sLayoutScrollDescriptorVtable 100FF240
 data sLayoutScrollDescriptorOwner 101FF32C
 data sScrollOpenState 10498BB8
 data sScrollStopState 10498C18
 data sScrollScrollUpState 10498C48
 data sScrollScrollDownState 10498C78
 data sScrollKeyIconDecideWaitState 10498D08
 data sScrollCloseStateRecord 10498D38
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
 data sLayoutScrollHeap 1018C404
 data sLayoutScrollAnimationNames 10498AE8
 data sLayoutScrollIconPaneNames 10498B4C
 str 100FF338 "KeyIcon_00"
 str 100FF344 "ScrollKeyIcon_00"
 data sLayoutScrollLayoutNames 10498B8C
 data sLayoutScrollLayoutName 10498D98
 data sLayoutScrollStringVtable 100FF1E0
 data sLayoutScrollButtonNames 10498B5C
 str 100FF2C4 "N_All_00"
 str 100FF2D0 "N_Scissor_00"
 str 100FF2E8 "T_Msg_00"
 str 100FF2F4 "T_Msg_01"
 str 100FF300 "T_Msg_02"
 str 100FF30C "P_Wood_00"
 str 100FF318 "P_Paper_00"
 str 100FF324 "P_Stone_00"
 data sLayoutScrollAnimationKinds 100FF1D0
*/

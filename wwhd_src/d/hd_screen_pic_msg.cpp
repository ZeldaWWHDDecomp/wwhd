/* HD opening picture story screen lifecycle and skip prompt. */
#include "d/hd_screen_pic_msg.h"
static u32 sPicMsgStringVtable;
static u32 sPicMsgStateVtable;
static f32 sSkipAnimationStart = 1.0f;

/* 026BF3A0 */
void hdOpening_screen_c::finish() {
    gabi::call_ptr<void>(mVtable->mDestroyLayout.get(), this);
    openingFinishBase(this);
}

/* 026BF3E0 */
void hdOpening_screen_c::update() {
    openingUpdateState(&mStateHolder.mMachine);
}

/* 026BF3E8 */
void hdOpening_screen_c::showSkip() {
    if (mSkipRequested) return;
    openingPlaySkipAnimation(mpLayout->animSet(), 0, 2, sSkipAnimationStart);
    mSkipRequested = 1;
}

/* 026BF440 */
void hdOpening_screen_c::hideSkip() {
    hdPartsLayout_c* layout = mpLayout;
    mSkipRequested = 0;
    openingPlaySkipAnimation(layout->animSet(), 1, 2, sSkipAnimationStart);
}

static hdUiSafeString sOpeningScreenName;
static hdUiSafeString sOpeningLayoutName;
static hdUiSafeString sOpeningAnimationGroups[3] = {{0, 0}, {0, 0}, {0, 0}};
static hdUiSafeString sOpeningAnimationNames[25] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
static u32 sOpeningGroupIndices[25] = {2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
struct hdOpening_heapPointer_l { gptr<hdOpening_heapManager_l> mpManager; };
static hdOpening_heapPointer_l sOpeningHeapManager;
static hdPartsStateId_c sFirstPictureState;

/* 026BF28C */
s32 hdOpening_screen_c::setup(u32 archive, u32 flags, u32 layoutOptions) {
    openingSetupBase(this, &sOpeningScreenName, 1);
    if (!gabi::call_ptr<s32>(mVtable->mLoad.get(), this, &sOpeningLayoutName,
                            mpResAccessor.get(), archive, flags, layoutOptions)) return 0;
    findRootPanes();
    findMessagePane();
    findPicturePanes();
    hdOpening_textBox_l* textBox = mpTextBox;
    mpMessagePane = &textBox->mMessagePane;
    /* The lfs/stfs copy preserves even signaling-NaN payloads; no arithmetic occurs. */
    textBox->mSavedScaleBits = textBox->mInitialScaleBits;
    hdStateOwner* owner = mStateHolder.mMachine.mpOwner;
    hdStateObject* state = owner->createState((hdStateId*)&sFirstPictureState);
    mStateHolder.mMachine.mpCurrentState = state;
    state->enter();
    return 1;
}

/* 026BF464 */
s32 hdOpening_screen_c::loadLayout(hdUiSafeString* name, void* context,
                                  u32 archive, u32 flags, u32 options) {
    hdPartsLayout_c* layout = openingAllocateLayout(0x110, sOpeningHeapManager.mpManager->mpHeap, 4);
    if (layout) layout = openingConstructLayout(layout);
    mpLayout = layout;
    if (!layout) return 0;
    if (!gabi::call_ptr<s32>(layout->vtbl()->mLoad.get(), layout, name, context,
                            archive, flags, 25, 3, options)) return 0;
    for (u32 i = 0; i < 25; ++i) {
        openingBindAnimation(mpLayout->animSet(), i, &sOpeningAnimationNames[i],
                             &sOpeningAnimationGroups[sOpeningGroupIndices[i]]);
    }
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
    return 1;
}

static u32 sOpeningRuntimeState[4] = {0, 0, 0, 0};
static u32 sOpeningRuntimeRegistration[3] = {0, 0, 0};
static u32 sOpeningFirstContextRegistration[3] = {0, 0, 0};
static u32 sOpeningSecondContextRegistration[3] = {0, 0, 0};
static f32 sOpeningMinimumAngle;
static f32 sOpeningMaximumAngle;
static u8 sOpeningFirstContext;
static u8 sOpeningSecondContext;
static u32 sOpeningStateCounterReady;
static u32 sOpeningStateCounter;
static u8 sOpeningStateContext;
static hdPartsStateId_c sOpeningStates[23] = {};

/* 026BF590 */
static void initializeOpeningStatics() {
    for (auto& word : sOpeningRuntimeState) word = 0;
    openingRegisterStatics(sOpeningRuntimeRegistration);
    sOpeningMinimumAngle = -3.1415927410125732f;
    sOpeningMaximumAngle = 3.1415927410125732f;
    openingConstructFirstContext(&sOpeningFirstContext);
    openingRegisterStatics(sOpeningFirstContextRegistration);
    openingConstructSecondContext(&sOpeningSecondContext);
    openingRegisterStatics(sOpeningSecondContextRegistration);
    sOpeningAnimationGroups[0].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationGroups[0], "G_PicMsg_00");
    sOpeningAnimationGroups[1].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationGroups[1], "G_All_00");
    sOpeningAnimationGroups[2].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationGroups[2], "G_Skip_00");
    sOpeningAnimationNames[0].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[0], "InSkip");
    sOpeningAnimationNames[1].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[1], "OutSkip");
    sOpeningAnimationNames[2].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[2], "PicMsg_00");
    sOpeningAnimationNames[3].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[3], "PicMsg_01");
    sOpeningAnimationNames[4].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[4], "PicMsg_02");
    sOpeningAnimationNames[5].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[5], "PicMsg_03");
    sOpeningAnimationNames[6].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[6], "PicMsg_04");
    sOpeningAnimationNames[7].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[7], "PicMsg_05");
    sOpeningAnimationNames[8].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[8], "PicMsg_06");
    sOpeningAnimationNames[9].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[9], "PicMsg_07");
    sOpeningAnimationNames[10].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[10], "PicMsg_08");
    sOpeningAnimationNames[11].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[11], "PicMsg_09");
    sOpeningAnimationNames[12].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[12], "PicMsg_10");
    sOpeningAnimationNames[13].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[13], "PicMsg_11");
    sOpeningAnimationNames[14].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[14], "PicMsg_12");
    sOpeningAnimationNames[15].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[15], "PicMsg_13");
    sOpeningAnimationNames[16].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[16], "PicMsg_14");
    sOpeningAnimationNames[17].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[17], "PicMsg_15");
    sOpeningAnimationNames[18].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[18], "PicMsg_16");
    sOpeningAnimationNames[19].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[19], "PicMsg_17");
    sOpeningAnimationNames[20].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[20], "PicMsg_18");
    sOpeningAnimationNames[21].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[21], "PicMsg_19");
    sOpeningAnimationNames[22].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[22], "PicMsg_20");
    sOpeningAnimationNames[23].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[23], "PicMsg_21");
    sOpeningAnimationNames[24].mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningAnimationNames[24], "OutAll");
    openingSetString(sOpeningScreenName, "Opening_00");
    sOpeningScreenName.mVtable.v = gabi::ea(&sPicMsgStringVtable);
    openingSetString(sOpeningLayoutName, "Opening_00");
    sOpeningLayoutName.mVtable.v = gabi::ea(&sPicMsgStringVtable);
    u32 id = 0;
    if (sOpeningStateCounterReady) id = sOpeningStateCounter;
    else sOpeningStateCounterReady = 1;
    sOpeningStates[0].mName.v = gabi::guest_addr("::StateID_PicMsg_00");
    sOpeningStates[1].mName.v = gabi::guest_addr("::StateID_PicMsg_01");
    sOpeningStates[2].mName.v = gabi::guest_addr("::StateID_PicMsg_02");
    sOpeningStates[3].mName.v = gabi::guest_addr("::StateID_PicMsg_03");
    sOpeningStates[4].mName.v = gabi::guest_addr("::StateID_PicMsg_04");
    sOpeningStates[5].mName.v = gabi::guest_addr("::StateID_PicMsg_05");
    sOpeningStates[6].mName.v = gabi::guest_addr("::StateID_PicMsg_06");
    sOpeningStates[7].mName.v = gabi::guest_addr("::StateID_PicMsg_07");
    sOpeningStates[8].mName.v = gabi::guest_addr("::StateID_PicMsg_08");
    sOpeningStates[9].mName.v = gabi::guest_addr("::StateID_PicMsg_09");
    sOpeningStates[10].mName.v = gabi::guest_addr("::StateID_PicMsg_10");
    sOpeningStates[11].mName.v = gabi::guest_addr("::StateID_PicMsg_11");
    sOpeningStates[12].mName.v = gabi::guest_addr("::StateID_PicMsg_12");
    sOpeningStates[13].mName.v = gabi::guest_addr("::StateID_PicMsg_13");
    sOpeningStates[14].mName.v = gabi::guest_addr("::StateID_PicMsg_14");
    sOpeningStates[15].mName.v = gabi::guest_addr("::StateID_PicMsg_15");
    sOpeningStates[16].mName.v = gabi::guest_addr("::StateID_PicMsg_16");
    sOpeningStates[17].mName.v = gabi::guest_addr("::StateID_PicMsg_17");
    sOpeningStates[18].mName.v = gabi::guest_addr("::StateID_PicMsg_18");
    sOpeningStates[19].mName.v = gabi::guest_addr("::StateID_PicMsg_19");
    sOpeningStates[20].mName.v = gabi::guest_addr("::StateID_PicMsg_20");
    sOpeningStates[21].mName.v = gabi::guest_addr("::StateID_PicMsg_21");
    sOpeningStates[22].mName.v = gabi::guest_addr("::StateID_OutAll");
    u32 i = 0;
    for (hdPartsStateId_c& entry : sOpeningStates) {
        hdPartsStateId_c* state = &entry;
        state->mNumber = id + i + 1;
        state->mVtable.v = gabi::ea(&sPicMsgStateVtable);
        state->mEnter.mThisDelta = 0;
        state->mEnter.mVirtualIndex = 25 + 3 * i;
        state->mEnter.mFunction = 4;
        state->mExecute.mThisDelta = 0;
        state->mExecute.mVirtualIndex = 26 + 3 * i;
        state->mExecute.mFunction = 4;
        state->mExit.mThisDelta = 0;
        state->mExit.mVirtualIndex = 27 + 3 * i;
        state->mExit.mFunction = 4;
        state->mUnused.mThisDelta = 0;
        state->mUnused.mVirtualIndex = 0;
        state->mUnused.mFunction = 0;
        state->mpNullStateId.v = gabi::ea(&sOpeningStateContext);
        ++i;
    }
    sOpeningStateCounter = id + 23;
}

/* WWHD glue (tools/verify/autoglue.py): opening lifecycle
 data sPicMsgStringVtable 100FF91C nocheck
 data sPicMsgStateVtable 100FF97C nocheck
 data sOpeningRuntimeState 10498EFC nocheck
 data sOpeningRuntimeRegistration 101F67D4 nocheck
 data sOpeningFirstContextRegistration 101F67E0 nocheck
 data sOpeningSecondContextRegistration 101F67EC nocheck
 data sOpeningMinimumAngle 10498EE0 nocheck
 data sOpeningMaximumAngle 10498EE4 nocheck
 data sOpeningFirstContext 10498EE8 nocheck
 data sOpeningSecondContext 10498EE9 nocheck
 data sOpeningStateCounterReady 101FD8E4 nocheck
 data sOpeningStateCounter 101FDD50 nocheck
 data sOpeningStateContext 101FF32C nocheck
 data sOpeningStates 10499040 nocheck
 ext 028F026C void openingRegisterStatics(void*)
 ext 028ED6F8 void openingConstructFirstContext(void*)
 ext 028EAB2C void openingConstructSecondContext(void*)
 str 100FFBC8 "G_PicMsg_00" in 026BF590
 str 100FFEA0 "G_All_00" in 026BF590
 str 100FFD8C "G_Skip_00" in 026BF590
 str 100FFBB8 "InSkip" in 026BF590
 str 100FFBB0 "OutSkip" in 026BF590
 str 100FFD98 "PicMsg_00" in 026BF590
 str 100FFDA4 "PicMsg_01" in 026BF590
 str 100FFDB0 "PicMsg_02" in 026BF590
 str 100FFDBC "PicMsg_03" in 026BF590
 str 100FFDC8 "PicMsg_04" in 026BF590
 str 100FFDD4 "PicMsg_05" in 026BF590
 str 100FFDE0 "PicMsg_06" in 026BF590
 str 100FFDEC "PicMsg_07" in 026BF590
 str 100FFDF8 "PicMsg_08" in 026BF590
 str 100FFE04 "PicMsg_09" in 026BF590
 str 100FFE10 "PicMsg_10" in 026BF590
 str 100FFE1C "PicMsg_11" in 026BF590
 str 100FFE28 "PicMsg_12" in 026BF590
 str 100FFE34 "PicMsg_13" in 026BF590
 str 100FFE40 "PicMsg_14" in 026BF590
 str 100FFE4C "PicMsg_15" in 026BF590
 str 100FFE58 "PicMsg_16" in 026BF590
 str 100FFE64 "PicMsg_17" in 026BF590
 str 100FFE70 "PicMsg_18" in 026BF590
 str 100FFE7C "PicMsg_19" in 026BF590
 str 100FFE88 "PicMsg_20" in 026BF590
 str 100FFE94 "PicMsg_21" in 026BF590
 str 100FFBC0 "OutAll" in 026BF590
 str 100FFEAC "Opening_00" in 026BF590
 str 100FFBD4 "::StateID_PicMsg_00" in 026BF590
 str 100FFBE8 "::StateID_PicMsg_01" in 026BF590
 str 100FFBFC "::StateID_PicMsg_02" in 026BF590
 str 100FFC10 "::StateID_PicMsg_03" in 026BF590
 str 100FFC24 "::StateID_PicMsg_04" in 026BF590
 str 100FFC38 "::StateID_PicMsg_05" in 026BF590
 str 100FFC4C "::StateID_PicMsg_06" in 026BF590
 str 100FFC60 "::StateID_PicMsg_07" in 026BF590
 str 100FFC74 "::StateID_PicMsg_08" in 026BF590
 str 100FFC88 "::StateID_PicMsg_09" in 026BF590
 str 100FFC9C "::StateID_PicMsg_10" in 026BF590
 str 100FFCB0 "::StateID_PicMsg_11" in 026BF590
 str 100FFCC4 "::StateID_PicMsg_12" in 026BF590
 str 100FFCD8 "::StateID_PicMsg_13" in 026BF590
 str 100FFCEC "::StateID_PicMsg_14" in 026BF590
 str 100FFD00 "::StateID_PicMsg_15" in 026BF590
 str 100FFD14 "::StateID_PicMsg_16" in 026BF590
 str 100FFD28 "::StateID_PicMsg_17" in 026BF590
 str 100FFD3C "::StateID_PicMsg_18" in 026BF590
 str 100FFD50 "::StateID_PicMsg_19" in 026BF590
 str 100FFD64 "::StateID_PicMsg_20" in 026BF590
 str 100FFD78 "::StateID_PicMsg_21" in 026BF590
 str 100FFEB8 "::StateID_OutAll" in 026BF590
 data sOpeningScreenName 10498ED8 nocheck
 data sOpeningLayoutName 10499490 nocheck
 data sOpeningAnimationGroups 10498F0C nocheck
 data sOpeningAnimationNames 10498F6C nocheck
 data sOpeningGroupIndices 100FF9C4
 data sOpeningHeapManager 1018C404
 data sFirstPictureState 10499040 nocheck
 ext 026F90D8 void openingSetupBase(hdOpening_screen_c*, hdUiSafeString*, u32)
 ext 0273B050 hdPartsLayout_c* openingAllocateLayout(u32, void*, u32)
 ext 026FADB0 hdPartsLayout_c* openingConstructLayout(hdPartsLayout_c*)
 ext 02004E04 void openingBindAnimation(void*, u32, hdUiSafeString*, hdUiSafeString*)
 data sSkipAnimationStart 100FFA88
 ext 026F91F8 void openingFinishBase(hdOpening_screen_c*)
 ext 02006364 void openingUpdateState(void*)
 ext 020053E4 void openingPlaySkipAnimation(void*, u32, u32, f32)
*/

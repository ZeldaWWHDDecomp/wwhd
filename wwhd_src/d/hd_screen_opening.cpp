/* HD opening picture story state progression; binary-derived, */
#include "d/hd_screen_opening.h"
static u32 sOpeningScreenVtable;
static u32 sOpeningFirstBaseVtable;
static u32 sOpeningSecondBaseVtable;
static u32 sOpeningStringVtable;
static const u32 sOpeningStringDestructor = 0x026C00F8;
static u32 sPictureAnimations[23] = {0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18};
static u32 sPictureMessageIds[22] = {0x58E, 0x579, 0x57A, 0x57B, 0x57C, 0x57D, 0x57E, 0x57F, 0x580, 0x581, 0x582, 0x583, 0x584, 0x585, 0x586, 0x587, 0x588, 0x589, 0x58A, 0x58B, 0x58C, 0x58D};
static hdOpening_stateSlot_l sPictureStates[23] = {{0x10499040}, {0x10499070}, {0x104990A0}, {0x104990D0}, {0x10499100}, {0x10499130}, {0x10499160}, {0x10499190}, {0x104991C0}, {0x104991F0}, {0x10499220}, {0x10499250}, {0x10499280}, {0x104992B0}, {0x104992E0}, {0x10499310}, {0x10499340}, {0x10499370}, {0x104993A0}, {0x104993D0}, {0x10499400}, {0x10499430}, {0x10499460}};
static f32 sAnimationStart = 1.0f;

static hdUiSafeString sOpeningPaneNames[2] = {{0, 0}, {0, 0}};
static hdUiSafeString sOpeningMessageName;
static hdUiSafeString sOpeningPictureNames[9] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};

/* 026BE448 */
static u32 openingStateNumber(hdPartsStateId_c* state) {
    return state->mNumber;
}

/* 026BE548 */
static void destroyOpeningPaneNames() {
    openingDestroyStringArray(sOpeningPaneNames, 2, 8, sOpeningStringDestructor, 0, 0);
}

/* 026BE56C */
static void destroyOpeningMessageName() {
    openingDestroyStringArray(&sOpeningMessageName, 1, 8, sOpeningStringDestructor, 0, 0);
}

/* 026BE590 */
static void destroyOpeningPictureNames() {
    openingDestroyStringArray(sOpeningPictureNames, 9, 8, sOpeningStringDestructor, 0, 0);
}

/* 026BE5B4 */
static void destroyOpening(hdOpening_screen_c* screen, u32 flags) {
    if (!screen) return;
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sOpeningFirstBaseVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sOpeningSecondBaseVtable);
    openingDestroyBase(screen, 0);
    if (flags & 1) openingDelete(screen);
}

/* 026BE620 */
void hdOpening_screen_c::startPictureMessage() {
    if (mPictureMessageIndex >= 23) return;
    openingPlayAnimation(mpLayout->animSet(), sPictureAnimations[mPictureMessageIndex], 0, sAnimationStart);
    if (mPictureMessageIndex < 22) {
        openingSetMessage(mpMessagePane, sPictureMessageIds[mPictureMessageIndex], 0);
        mpMessagePane->mTextChanged = 1;
        openingBindText(mpTextBox, mpMessagePane, 1);
    }
}

/* 026BE6B8 */
void hdOpening_screen_c::enterPictureStory() {
    mFinished = 0;
    startPictureMessage();
}

/* 026BE6C4 */
s32 hdOpening_screen_c::advancePictureMessage() {
    if (mPictureMessageIndex >= 23) return 0;
    if (!openingAnimationFinished(mpLayout->animSet(), sPictureAnimations[mPictureMessageIndex])) return 0;
    mPictureMessageIndex = s32(u32(mPictureMessageIndex) + 1);
    if (mPictureMessageIndex < 23) openingChangeState(&mStateHolder.mMachine, sPictureStates[mPictureMessageIndex].mpState.get());
    return 1;
}

/* 026BE760 */
void hdOpening_screen_c::picMsg00Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BE7A0 */
void hdOpening_screen_c::picMsg01Enter() { startPictureMessage(); }

/* 026BE7A4 */
void hdOpening_screen_c::picMsg01Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BE7E4 */
void hdOpening_screen_c::picMsg02Enter() { startPictureMessage(); }

/* 026BE7E8 */
void hdOpening_screen_c::picMsg02Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BE828 */
void hdOpening_screen_c::picMsg03Enter() { startPictureMessage(); }

/* 026BE82C */
void hdOpening_screen_c::picMsg03Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BE86C */
void hdOpening_screen_c::picMsg04Enter() { startPictureMessage(); }

/* 026BE870 */
void hdOpening_screen_c::picMsg04Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BE8B0 */
void hdOpening_screen_c::picMsg05Enter() { startPictureMessage(); }

/* 026BE8B4 */
void hdOpening_screen_c::picMsg05Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BE8F4 */
void hdOpening_screen_c::picMsg06Enter() { startPictureMessage(); }

/* 026BE8F8 */
void hdOpening_screen_c::picMsg06Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BE938 */
void hdOpening_screen_c::picMsg07Enter() { startPictureMessage(); }

/* 026BE93C */
void hdOpening_screen_c::picMsg07Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BE97C */
void hdOpening_screen_c::picMsg08Enter() { startPictureMessage(); }

/* 026BE980 */
void hdOpening_screen_c::picMsg08Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BE9C0 */
void hdOpening_screen_c::picMsg09Enter() { startPictureMessage(); }

/* 026BE9C4 */
void hdOpening_screen_c::picMsg09Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BEA04 */
void hdOpening_screen_c::picMsg10Enter() { startPictureMessage(); }

/* 026BEA08 */
void hdOpening_screen_c::picMsg10Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BEA48 */
void hdOpening_screen_c::picMsg11Enter() { startPictureMessage(); }

/* 026BEA4C */
void hdOpening_screen_c::picMsg11Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BEA8C */
void hdOpening_screen_c::picMsg12Enter() { startPictureMessage(); }

/* 026BEA90 */
void hdOpening_screen_c::picMsg12Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BEAD0 */
void hdOpening_screen_c::picMsg13Enter() { startPictureMessage(); }

/* 026BEAD4 */
void hdOpening_screen_c::picMsg13Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BEB14 */
void hdOpening_screen_c::picMsg14Enter() { startPictureMessage(); }

/* 026BEB18 */
void hdOpening_screen_c::picMsg14Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BEB58 */
void hdOpening_screen_c::picMsg15Enter() { startPictureMessage(); }

/* 026BEB5C */
void hdOpening_screen_c::picMsg15Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BEB9C */
void hdOpening_screen_c::picMsg16Enter() { startPictureMessage(); }

/* 026BEBA0 */
void hdOpening_screen_c::picMsg16Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BEBE0 */
void hdOpening_screen_c::picMsg17Enter() { startPictureMessage(); }

/* 026BEBE4 */
void hdOpening_screen_c::picMsg17Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BEC24 */
void hdOpening_screen_c::picMsg18Enter() { startPictureMessage(); }

/* 026BEC28 */
void hdOpening_screen_c::picMsg18Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BEC68 */
void hdOpening_screen_c::picMsg19Enter() { startPictureMessage(); }

/* 026BEC6C */
void hdOpening_screen_c::picMsg19Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BECAC */
void hdOpening_screen_c::picMsg20Enter() { startPictureMessage(); }

/* 026BECB0 */
void hdOpening_screen_c::picMsg20Move() {
    advancePictureMessage();
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BECF0 */
void hdOpening_screen_c::picMsg21Enter() { startPictureMessage(); }

/* 026BECF4 */
void hdOpening_screen_c::picMsg21Move() {
    if (advancePictureMessage()) mFinished = 1;
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BED44 */
void hdOpening_screen_c::outAllEnter() {
    openingPlayAnimation(mpLayout->animSet(), 24, 0, sAnimationStart);
}

/* 026BED60 */
void hdOpening_screen_c::outAllMove() {
    gabi::call_ptr<void>(mVtable->mCalcLayout.get(), this);
}

/* 026BED70 */
static hdOpening_screen_c* constructOpening(hdOpening_screen_c* screen) {
    if (!screen) {
        screen = openingAllocateScreen(0x8C);
        if (!screen) return nullptr;
    }
    openingConstructBase(screen);
    screen->mVtable.v = gabi::ea(&sOpeningScreenVtable);
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sOpeningFirstBaseVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sOpeningSecondBaseVtable);
    screen->mPictureMessageIndex = 0;
    screen->mpMessagePane = nullptr;
    if (gabi::ea(&screen->mpAllPane) == 0) openingAllocateScreen(8);
    if (gabi::ea(&screen->mpTextBox) == 0) openingAllocateScreen(4);
    if (gabi::ea(&screen->mpBackgroundPane) == 0) openingAllocateScreen(0x24);
    screen->mSkipRequested = 0;
    screen->mFinished = 0;
    return screen;
}

static u32 sRootPanesReady;
static u32 sRootPaneRegistration[3] = {0, 0, 0};

/* 026BEE28 */
void hdOpening_screen_c::findRootPanes() {
    if (!sRootPanesReady) {
        sRootPanesReady = 1;
        sOpeningPaneNames[0].mVtable.v = gabi::ea(&sOpeningStringVtable);
        openingSetString(sOpeningPaneNames[0], "N_All_00");
        sOpeningPaneNames[1].mVtable.v = gabi::ea(&sOpeningStringVtable);
        openingSetString(sOpeningPaneNames[1], "N_OpeningPic_00");
        openingRegisterRootPanes(sRootPaneRegistration);
    }
    hdPartsPane_c* root = mpLayout->lyt()->mpRootPane;
    for (u32 i = 0; i < 2; ++i) {
        hdUiSafeString* name = &sOpeningPaneNames[i];
        hdOpening_paneVtable_l* paneVtable = hdPartsView<hdOpening_paneVtable_l>(root->mVtable.v);
        gabi::call_ptr<void>(name->mVtable->mAssureTermination.get(), name);
        void* pane = gabi::call_ptr<void*>(paneVtable->mFindPane, root, name->mText.get(), 1);
        if (i == 0) mpAllPane = pane;
        else mpPicturePane = pane;
    }
}

/* WWHD glue (tools/verify/autoglue.py): opening picture-story states
 data sOpeningScreenVtable 100FFED0 nocheck
 data sOpeningFirstBaseVtable 101001C0 nocheck
 data sOpeningSecondBaseVtable 101001D0 nocheck
 data sOpeningStringVtable 100FF91C nocheck
 data sRootPanesReady 10499034 nocheck
 data sRootPaneRegistration 101F67B0 nocheck
 str 100FFAF4 "N_All_00"
 str 100FFAE4 "N_OpeningPic_00"
 ext 028F026C void openingRegisterRootPanes(void*)
 data sOpeningPaneNames 10498EEC nocheck
 data sOpeningMessageName 10498ED0 nocheck
 data sOpeningPictureNames 10498F24 nocheck
 ext 028F0164 void openingDestroyStringArray(void*, u32, u32, u32, u32, u32)
 ext 0273AD10 hdOpening_screen_c* openingAllocateScreen(u32)
 ext 026F89E8 void openingConstructBase(hdOpening_screen_c*)
 data sPictureAnimations 100FFA28
 data sPictureMessageIds 100FFA8C
 data sPictureStates 101F67F8
 data sAnimationStart 100FFA88
 ext 026F88F0 void openingDestroyBase(hdOpening_screen_c*, u32)
 ext 0273AF40 void openingDelete(void*)
 ext 020053E4 void openingPlayAnimation(void*, u32, u32, f32)
 ext 025F6B18 void openingSetMessage(hdOpening_textPane_l*, u32, u32)
 ext 02704504 void openingBindText(void*, hdOpening_textPane_l*, u32)
 ext 02005840 s32 openingAnimationFinished(void*, u32)
 ext 020063C0 void openingChangeState(void*, void*)
*/

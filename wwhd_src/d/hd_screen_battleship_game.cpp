/**
 * hd_screen_battleship_game.cpp
 * HD - the battleship (squid hunt) minigame screen "BattleshipGame_00": 24 cannonball icons and three squids
 * as child parts, the cannonballs-fired and best-score numbers; states ::StateID_InAll / _OutAll / _Wait
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_mini_auction_battleship_3more). Each function is marked with its WWHD address; the glue block at
 * the end lists this translation unit's data (tools/verify/autoglue.py). Layout, pane, class and state
 * names are HD strings; all other names are ours.
 */

#include "d/hd_screen_battleship_game.h"

static hdPartsVtbl_c sBattleshipGameVtable;
static u32 sBattleshipGameHolderVtable;
static u32 sBattleshipGameSecondaryVtable;
static hdUiStringTable sBattleshipGameSafeStringVtable;
static hdPartsStateIdVtbl_c sBattleshipGameStateIdVtable;
static hdStateId sBattleshipGameNoStateId;
static hdPartsStateId_c sBattleshipGamePartsBlank; /* hdParts_c's ::StateID_Blank (hd_parts.cpp) */
static hdScreenUiSystem* sBattleshipGameUiSystem;
static u32 sBattleshipGameTextVtable;
static hdUiSafeString sBattleshipGameOneTimeClassName;  /* "Cannonball_00" (hd_screen_cannonball.cpp) */
static hdUiSafeString sBattleshipGameSquidClassName;     /* "SquidIcon_00" (hd_screen_squid_icon.cpp) */

static hdUiSafeString sBattleshipGameTextPaneNames[2] = {};
static hdUiSafeString sBattleshipGameArchiveNames[3] = {};
static hdUiSafeString sBattleshipGamePaneNames[33] = {};
static hdPartsStateId_c sBattleshipGameInAll;
static hdPartsStateId_c sBattleshipGameOutAll;
static hdPartsStateId_c sBattleshipGameWait;
static hdUiSafeString sBattleshipGameLayoutName;
static u32 sBattleshipGameStateNumberReady;
static s32 sBattleshipGameStateNumber;

/* inlined into createChild: sead SafeString equality (both strings terminated first) */
static bool hdBattleshipGame_isSameName(hdUiSafeString* name, hdUiSafeString* other) {
    name->assureTermination();
    u32 text = gabi::ea(name->mText.get());
    other->assureTermination();
    if (text == gabi::ea(other->mText.get())) {
        return true;
    }
    u32 left = gabi::ea(name->mText.get());
    u32 right = gabi::ea(other->mText.get());
    for (u32 n = 0; n < 0x40001; n++) {
        u8 c = *hdPartsView<be<u8>>(left + n);
        if (c != *hdPartsView<be<u8>>(right + n)) {
            return false;
        }
        if (c == 0) {
            return true;
        }
    }
    return false;
}

/* the descriptor of a child part as the createChild hook reads it (our view): class and pane name */
struct hdBattleshipGameDesc_l {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ hdUiSafeString mClassName;
    /* 0x10 */ u8 _10[4];
    /* 0x14 */ hdUiSafeString mPaneName;
};

/* 02623A60 */
// hdScreenBattleship_c::~hdScreenBattleship_c() (GHS deleting destructor)
void hdScreenBattleship_c::destroy(hdScreenBattleship_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sBattleshipGameHolderVtable;
    i_this->mSecondaryVtable = &sBattleshipGameSecondaryVtable;
    hdParts_c::destroy(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 02623ACC */
// the best score, two digits, in "T_NumBestScore_00" (the sibling of updateDisplay, 02623B44)
void hdScreenBattleship_c::updateBestScore() {
    // HD: keep the original frame offsets and the untouched middle buffer word.
    gabi::NativeFrame<0x20> frame;
    MinigameText* text = gabi::at<MinigameText>(frame + 8);
    MinigameTextBuffer3* buffer = gabi::at<MinigameTextBuffer3>(frame + 0x14);
    buffer->characters[2] = 0;
    buffer->characters[0] = 0;
    text->mBuffer = buffer->characters;
    text->__vtbl = gabi::ea(&sBattleshipGameTextVtable);
    text->mCapacity = 3;
    minigameFormatCount(text, "battleship best score format", mBestScore);
    minigameSetText(mpTextPanes[1].get(), text, 0);
}

/* 02623BBC */
// a new game: all icons back to their first frame, the best score from the save data, nothing hit
void hdScreenBattleship_c::reset() {
    mRevealedMisses = 0;
    for (u32 i = 0; i < BATTLESHIP_SQUID_COUNT; i++) {
        hdOneTimeChild(child(BATTLESHIP_SQUID_FIRST + i))->resetAnimation();
    }
    for (u32 i = 0; i < BATTLESHIP_CANNONBALL_COUNT; i++) {
        hdOneTimeChild(child(i))->resetAnimation();
    }
    mBestScore = dComIfGs_info()->mSavedata.mEvent.getEventReg(BATTLESHIP_BEST_SCORE_REG);
    updateBestScore();
    mRevealedHits = 0;
    updateDisplay();
    vCalcLayout();
}

/* 02623CB4 */
void hdScreenBattleship_c::inAllEnter() {
    reset();
    mVisible = TRUE;
}

/* 02623CE8 */
void hdScreenBattleship_c::inAllExecute() {
    changeState(&sBattleshipGameWait);
}

/* 02623CF8 */
void hdScreenBattleship_c::outAllExecute() {
    changeState(&sBattleshipGamePartsBlank);
}

/* 02623D08 */
void hdScreenBattleship_c::outAllExit() {
    mVisible = FALSE;
}

/* 02623D14 */
// hdScreenBattleship_c::hdScreenBattleship_c() (GHS: allocates when called with NULL)
hdScreenBattleship_c* hdScreenBattleship_c::construct(hdScreenBattleship_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenBattleship_c*)operator_new(sizeof(hdScreenBattleship_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdParts_c::construct(i_this);
    i_this->mVtable = &sBattleshipGameVtable;
    i_this->mSecondaryVtable = &sBattleshipGameSecondaryVtable;
    i_this->mStateHolder.mHolderVtable = &sBattleshipGameHolderVtable;
    // HD: GHS tests the address of the (trivially constructed) pane pointer array
    if (gabi::ea(i_this->mpTextPanes) == 0) {
        operator_new(sizeof(i_this->mpTextPanes));
    }
    i_this->mRevealedMisses = 0;
    i_this->mRevealedHits = 0;
    i_this->mVisible = FALSE;
    i_this->mBestScore = 0;
    for (int i = 0; i < 2; i++) {
        i_this->mpTextPanes[i] = NULL;
    }
    return i_this;
}

/* 02623DE0 */
void hdScreenBattleship_c::findTextPanes() {
    hdCursorPane_c* root = (hdCursorPane_c*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    for (int i = 0; i < 2; i++) {
        hdUiSafeString* name = &sBattleshipGameTextPaneNames[i];
        hdCursorPaneVtbl_c* rootVtable = root->mVtable;
        name->assureTermination();
        mpTextPanes[i] = gabi::call_ptr<void*>(rootVtable->mFindPaneByName.get(), root, (hdCursorChar_t*)name->mText, 1u);
    }
}

/* 02623EBC */
u32 hdScreenBattleship_c::init(u32 arg1, u32 arg2, void* desc) {
    attachArchives(sBattleshipGameArchiveNames, 3);
    if (!vLoad(&sBattleshipGameLayoutName, gabi::ea(mpResAccessor.get()), arg1, arg2, desc)) {
        return FALSE;
    }
    if (!vCreateChildren()) {
        return FALSE;
    }
    findTextPanes();
    startState(&sBattleshipGamePartsBlank);
    return TRUE;
}

/* 02623FC8 */
void hdScreenBattleship_c::finalize() {
    vDeleteChildren();
    vDestroyLayout();
    detachArchives();
}

/* 0262401C */
// nothing runs while hidden (the parts' Blank state)
void hdScreenBattleship_c::executeState() {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(stateMachine());
    hdPartsStateIdVtbl_c* blankVtable = sBattleshipGamePartsBlank.mVtable;
    u32 key = current->getKey();
    if (key == gabi::call_ptr<u32>(blankVtable->mGetKey.get(), &sBattleshipGamePartsBlank)) {
        return;
    }
    hdParts_executeStateMachine(stateMachine());
    vExecuteChildren();
    vCalcLayout();
}

/* 026240C4 */
void hdScreenBattleship_c::draw(void* info) {
    if (mVisible) {
        vDrawLayout(info);
        vDrawChildren(info);
    }
}

/* 026241AC */
// the child parts hook: the grid's "L_Cannonball_*" panes get "Cannonball_00" parts (the first 24; the
// other six are hidden), the "L_Squid_*" panes "SquidIcon_00" parts; other classes go to the base
u32 hdScreenBattleship_c::createChild(hdPartsDesc_c* desc) {
    hdScreenHeap* heap = sBattleshipGameUiSystem->mHeap;
    hdBattleshipGameDesc_l* names = (hdBattleshipGameDesc_l*)desc;
    for (s32 i = 0; i < BATTLESHIP_PANE_COUNT; i++) {
        names->mPaneName.assureTermination();
        if (!hdBattleshipGame_isSameName(&names->mPaneName, &sBattleshipGamePaneNames[i])) {
            continue;
        }
        names->mClassName.assureTermination();
        if (hdBattleshipGame_isSameName(&names->mClassName, &sBattleshipGameSquidClassName)) {
            gptr<hdParts_c>* slot = childSlot(i);
            void* parts = hdParts_new(sizeof(hdScreenSquidIcon_c), heap, 4);
            if (parts != NULL) {
                parts = hdScreenSquidIcon_c::construct((hdScreenSquidIcon_c*)parts);
            }
            *slot = (hdParts_c*)parts;
            if (child(i) == NULL) {
                return FALSE;
            }
            return ((hdScreenSquidIcon_c*)child(i))->init(0, 0, desc);
        }
        names->mClassName.assureTermination();
        if (!hdBattleshipGame_isSameName(&names->mClassName, &sBattleshipGameOneTimeClassName)) {
            return hdParts_c::createChild(i, desc);
        }
        if ((u32)i < BATTLESHIP_CANNONBALL_COUNT) {
            gptr<hdParts_c>* slot = childSlot(i);
            void* parts = hdParts_new(sizeof(hdScreenCannonball_c), heap, 4);
            if (parts != NULL) {
                parts = hdScreenCannonball_c::construct((hdScreenCannonball_c*)parts);
            }
            *slot = (hdParts_c*)parts;
            if (child(i) == NULL) {
                return FALSE;
            }
            return ((hdScreenCannonball_c*)child(i))->init(0, 0, desc);
        }
        // the six spare grid places stay empty: their pane is hidden
        hdCursorPane_c* root = (hdCursorPane_c*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
        hdCursorPaneVtbl_c* rootVtable = root->mVtable;
        sBattleshipGamePaneNames[i].assureTermination();
        hdCursorPane_c* pane = gabi::call_ptr<hdCursorPane_c*>(rootVtable->mFindPaneByName.get(), root, (hdCursorChar_t*)sBattleshipGamePaneNames[i].mText, 1u);
        if (pane != NULL) {
            pane->mFlags = pane->mFlags & ~CURSOR_PANE_VISIBLE;
        }
        return TRUE;
    }
    return TRUE;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 02624590 */
static void __sinit_hd_screen_battleship_game_cpp() {
    sinit_header_statics_rodata(0x1048EC04, 0x101F52C0, 0x1048EC10, 0x100E4AB4);
    hdParts_setSafeString(&sBattleshipGameTextPaneNames[0], &sBattleshipGameSafeStringVtable, "T_NumCannonball_00");
    hdParts_setSafeString(&sBattleshipGameTextPaneNames[1], &sBattleshipGameSafeStringVtable, "T_NumBestScore_00");
    hdParts_setSafeString(&sBattleshipGameArchiveNames[0], &sBattleshipGameSafeStringVtable, "BattleshipGame_00");
    hdParts_setSafeString(&sBattleshipGameArchiveNames[1], &sBattleshipGameSafeStringVtable, "Cannonball_00");
    hdParts_setSafeString(&sBattleshipGameArchiveNames[2], &sBattleshipGameSafeStringVtable, "SquidIcon_00");
    hdParts_setSafeString(&sBattleshipGamePaneNames[0], &sBattleshipGameSafeStringVtable, "L_Cannonball_00");
    hdParts_setSafeString(&sBattleshipGamePaneNames[1], &sBattleshipGameSafeStringVtable, "L_Cannonball_01");
    hdParts_setSafeString(&sBattleshipGamePaneNames[2], &sBattleshipGameSafeStringVtable, "L_Cannonball_02");
    hdParts_setSafeString(&sBattleshipGamePaneNames[3], &sBattleshipGameSafeStringVtable, "L_Cannonball_03");
    hdParts_setSafeString(&sBattleshipGamePaneNames[4], &sBattleshipGameSafeStringVtable, "L_Cannonball_04");
    hdParts_setSafeString(&sBattleshipGamePaneNames[5], &sBattleshipGameSafeStringVtable, "L_Cannonball_05");
    hdParts_setSafeString(&sBattleshipGamePaneNames[6], &sBattleshipGameSafeStringVtable, "L_Cannonball_06");
    hdParts_setSafeString(&sBattleshipGamePaneNames[7], &sBattleshipGameSafeStringVtable, "L_Cannonball_07");
    hdParts_setSafeString(&sBattleshipGamePaneNames[8], &sBattleshipGameSafeStringVtable, "L_Cannonball_10");
    hdParts_setSafeString(&sBattleshipGamePaneNames[9], &sBattleshipGameSafeStringVtable, "L_Cannonball_11");
    hdParts_setSafeString(&sBattleshipGamePaneNames[10], &sBattleshipGameSafeStringVtable, "L_Cannonball_12");
    hdParts_setSafeString(&sBattleshipGamePaneNames[11], &sBattleshipGameSafeStringVtable, "L_Cannonball_13");
    hdParts_setSafeString(&sBattleshipGamePaneNames[12], &sBattleshipGameSafeStringVtable, "L_Cannonball_14");
    hdParts_setSafeString(&sBattleshipGamePaneNames[13], &sBattleshipGameSafeStringVtable, "L_Cannonball_15");
    hdParts_setSafeString(&sBattleshipGamePaneNames[14], &sBattleshipGameSafeStringVtable, "L_Cannonball_16");
    hdParts_setSafeString(&sBattleshipGamePaneNames[15], &sBattleshipGameSafeStringVtable, "L_Cannonball_17");
    hdParts_setSafeString(&sBattleshipGamePaneNames[16], &sBattleshipGameSafeStringVtable, "L_Cannonball_20");
    hdParts_setSafeString(&sBattleshipGamePaneNames[17], &sBattleshipGameSafeStringVtable, "L_Cannonball_21");
    hdParts_setSafeString(&sBattleshipGamePaneNames[18], &sBattleshipGameSafeStringVtable, "L_Cannonball_22");
    hdParts_setSafeString(&sBattleshipGamePaneNames[19], &sBattleshipGameSafeStringVtable, "L_Cannonball_23");
    hdParts_setSafeString(&sBattleshipGamePaneNames[20], &sBattleshipGameSafeStringVtable, "L_Cannonball_24");
    hdParts_setSafeString(&sBattleshipGamePaneNames[21], &sBattleshipGameSafeStringVtable, "L_Cannonball_25");
    hdParts_setSafeString(&sBattleshipGamePaneNames[22], &sBattleshipGameSafeStringVtable, "L_Cannonball_26");
    hdParts_setSafeString(&sBattleshipGamePaneNames[23], &sBattleshipGameSafeStringVtable, "L_Cannonball_27");
    hdParts_setSafeString(&sBattleshipGamePaneNames[24], &sBattleshipGameSafeStringVtable, "L_Cannonball_08");
    hdParts_setSafeString(&sBattleshipGamePaneNames[25], &sBattleshipGameSafeStringVtable, "L_Cannonball_09");
    hdParts_setSafeString(&sBattleshipGamePaneNames[26], &sBattleshipGameSafeStringVtable, "L_Cannonball_18");
    hdParts_setSafeString(&sBattleshipGamePaneNames[27], &sBattleshipGameSafeStringVtable, "L_Cannonball_19");
    hdParts_setSafeString(&sBattleshipGamePaneNames[28], &sBattleshipGameSafeStringVtable, "L_Cannonball_28");
    hdParts_setSafeString(&sBattleshipGamePaneNames[29], &sBattleshipGameSafeStringVtable, "L_Cannonball_29");
    hdParts_setSafeString(&sBattleshipGamePaneNames[30], &sBattleshipGameSafeStringVtable, "L_Squid_00");
    hdParts_setSafeString(&sBattleshipGamePaneNames[31], &sBattleshipGameSafeStringVtable, "L_Squid_01");
    hdParts_setSafeString(&sBattleshipGamePaneNames[32], &sBattleshipGameSafeStringVtable, "L_Squid_02");
    s32 number;
    if (sBattleshipGameStateNumberReady) {
        number = sBattleshipGameStateNumber;
    } else {
        number = 0;
        sBattleshipGameStateNumberReady = 1;
    }
    sBattleshipGameInAll.setup(number + 1, "::StateID_InAll", &sBattleshipGameStateIdVtable, 0x19, &sBattleshipGameNoStateId);
    sBattleshipGameOutAll.setup(number + 2, "::StateID_OutAll", &sBattleshipGameStateIdVtable, 0x1C, &sBattleshipGameNoStateId);
    sBattleshipGameWait.setup(number + 3, "::StateID_Wait", &sBattleshipGameStateIdVtable, 0x1F, &sBattleshipGameNoStateId);
    sBattleshipGameStateNumber = number + 3;
    hdParts_setSafeString(&sBattleshipGameLayoutName, &sBattleshipGameSafeStringVtable, "BattleshipGame_00");
}

/* WWHD glue
data sBattleshipGameVtable 100E4D50 nocheck
data sBattleshipGameHolderVtable 100E4E60 nocheck
data sBattleshipGameSecondaryVtable 100E4E70 nocheck
data sBattleshipGameSafeStringVtable 100E49C4 nocheck
data sBattleshipGameStateIdVtable 100E4A54 nocheck
data sBattleshipGameNoStateId 101FF32C nocheck
data sBattleshipGamePartsBlank 1049DD60 nocheck
data sBattleshipGameUiSystem 1018C404 nocheck
data sBattleshipGameTextVtable 100E49F4 nocheck
data sBattleshipGameOneTimeClassName 10490F20 nocheck
data sBattleshipGameSquidClassName 1049BB30 nocheck
data sBattleshipGameTextPaneNames 1048EC20 nocheck
data sBattleshipGameArchiveNames 1048EC30 nocheck
data sBattleshipGamePaneNames 1048EC48 nocheck
data sBattleshipGameInAll 1048ED50 nocheck
data sBattleshipGameOutAll 1048ED80 nocheck
data sBattleshipGameWait 1048EDB0 nocheck
data sBattleshipGameLayoutName 1048EDE0 nocheck
data sBattleshipGameStateNumberReady 101FD8E4 nocheck
data sBattleshipGameStateNumber 101FDD50 nocheck
str 100E4AA4 "battleship best score format" renamed
str 100E4D08 "T_NumCannonball_00" in 02624590
str 100E4CAC "T_NumBestScore_00" in 02624590
str 100E4CC0 "BattleshipGame_00" in 02624590
str 100E4CD4 "Cannonball_00" in 02624590
str 100E4D1C "SquidIcon_00" in 02624590
str 100E4ABC "L_Cannonball_00" in 02624590
str 100E4ACC "L_Cannonball_01" in 02624590
str 100E4ADC "L_Cannonball_02" in 02624590
str 100E4AEC "L_Cannonball_03" in 02624590
str 100E4AFC "L_Cannonball_04" in 02624590
str 100E4B0C "L_Cannonball_05" in 02624590
str 100E4B1C "L_Cannonball_06" in 02624590
str 100E4B2C "L_Cannonball_07" in 02624590
str 100E4B3C "L_Cannonball_10" in 02624590
str 100E4B4C "L_Cannonball_11" in 02624590
str 100E4B5C "L_Cannonball_12" in 02624590
str 100E4B6C "L_Cannonball_13" in 02624590
str 100E4B7C "L_Cannonball_14" in 02624590
str 100E4B8C "L_Cannonball_15" in 02624590
str 100E4B9C "L_Cannonball_16" in 02624590
str 100E4BAC "L_Cannonball_17" in 02624590
str 100E4BBC "L_Cannonball_20" in 02624590
str 100E4BCC "L_Cannonball_21" in 02624590
str 100E4BDC "L_Cannonball_22" in 02624590
str 100E4BEC "L_Cannonball_23" in 02624590
str 100E4BFC "L_Cannonball_24" in 02624590
str 100E4C0C "L_Cannonball_25" in 02624590
str 100E4C1C "L_Cannonball_26" in 02624590
str 100E4C2C "L_Cannonball_27" in 02624590
str 100E4C3C "L_Cannonball_08" in 02624590
str 100E4C4C "L_Cannonball_09" in 02624590
str 100E4C5C "L_Cannonball_18" in 02624590
str 100E4C6C "L_Cannonball_19" in 02624590
str 100E4C7C "L_Cannonball_28" in 02624590
str 100E4C8C "L_Cannonball_29" in 02624590
str 100E4CE4 "L_Squid_00" in 02624590
str 100E4CF0 "L_Squid_01" in 02624590
str 100E4CFC "L_Squid_02" in 02624590
str 100E4C9C "::StateID_InAll" in 02624590
str 100E4D2C "::StateID_OutAll" in 02624590
str 100E4D40 "::StateID_Wait" in 02624590
ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c* i_this)
ext 026F88F0 void hdParts_c::destroy(hdParts_c* i_this, s32 flags)
ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString* names, u32 count)
ext 026F91F8 void hdParts_c::detachArchives()
ext 026F92C0 u32 hdParts_c::createChild(u32 index, hdPartsDesc_c* desc)
ext 02623B44 void hdScreenBattleship_c::updateDisplay()
ext 02759D3C void minigameFormatCount(MinigameText*, const char*, u32)
ext 027046D4 void minigameSetText(void*, MinigameText*, u32)
ext 02702654 void OneTimePart_l::resetAnimation()
*/

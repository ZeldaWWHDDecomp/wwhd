/**
 * hd_screen_cannon_game_2.cpp
 * HD - the cannon minigame screen "CannonGame_00": the cannon's angle, shots fired and ships sunk as child
 * parts; states ::StateID_InAll / _OutAll / _Wait
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_mini_auction_battleship_3more). Each function is marked with its WWHD address; the glue block at
 * the end lists this translation unit's data (tools/verify/autoglue.py). Layout, pane, class, animation
 * and state names are HD strings; all other names are ours.
 */

#include "d/hd_screen_cannon_game_2.h"

static hdPartsVtbl_c sCannonGameVtable;
static u32 sCannonGameHolderVtable;
static u32 sCannonGameSecondaryVtable;
static hdUiStringTable sCannonGameSafeStringVtable;
static hdPartsStateIdVtbl_c sCannonGameStateIdVtable;
static hdStateId sCannonGameNoStateId;
static hdPartsStateId_c sCannonGamePartsBlank; /* hdParts_c's ::StateID_Blank (hd_parts.cpp) */
static hdScreenUiSystem* sCannonGameUiSystem;
static u32 sCannonGameTextVtable;
static hdUiSafeString sCannonGameShipClassName;      /* "Battleship_00" (hd_screen_battleship_2.cpp) */
static hdUiSafeString sCannonGameCannonballClassName; /* "Cannonball_00" (hd_screen_cannonball.cpp) */

static hdUiSafeString sCannonGameGroupNames[1] = {};
static hdUiSafeString sCannonGameAnglePaneName;
static hdUiSafeString sCannonGameDegreePaneName;
static hdUiSafeString sCannonGameAnimNames[2] = {};
static hdUiSafeString sCannonGameArchiveNames[3] = {};
static hdUiSafeString sCannonGamePaneNames[15] = {};
static hdPartsStateId_c sCannonGameInAll;
static hdPartsStateId_c sCannonGameOutAll;
static hdPartsStateId_c sCannonGameWait;
static hdUiSafeString sCannonGameLayoutName;
static const u32 sCannonGameAnimGroups[2] = {0, 0};
static u32 sCannonGameStateNumberReady;
static s32 sCannonGameStateNumber;

/* inlined into createChild: sead SafeString equality (both strings terminated first) */
static bool hdCannonGame_isSameName(hdUiSafeString* name, hdUiSafeString* other) {
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
struct hdCannonGameDesc_l {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ hdUiSafeString mClassName;
    /* 0x10 */ u8 _10[4];
    /* 0x14 */ hdUiSafeString mPaneName;
};

/* 02646120 */
// hdScreenCannonGame_c::~hdScreenCannonGame_c() (GHS deleting destructor)
void hdScreenCannonGame_c::destroy(hdScreenCannonGame_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sCannonGameHolderVtable;
    i_this->mSecondaryVtable = &sCannonGameSecondaryVtable;
    hdParts_c::destroy(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 0264618C */
// the angle in whole degrees, in "T_NumDegree_00"
void hdScreenCannonGame_c::updateDegreeText() {
    // HD: keep the original frame offsets (text +8, buffer +0x14, conversion slot +0x20)
    gabi::NativeFrame<0x30> frame;
    MinigameText* text = gabi::at<MinigameText>(frame + 8);
    MinigameTextBuffer3* buffer = gabi::at<MinigameTextBuffer3>(frame + 0x14);
    buffer->characters[2] = 0;
    text->mCapacity = 3;
    text->__vtbl = gabi::ea(&sCannonGameTextVtable);
    gabi::store<u32>(frame + 0x20, (u32)gabi::ftoi(mAngle));
    text->mBuffer = buffer->characters;
    buffer->characters[0] = 0;
    minigameFormatCount(text, "cannon game degree format", gabi::load<u32>(frame + 0x20));
    minigameSetText(mpDegreeText.get(), text, 0);
}

/* 02646214 */
// the cannon pane turns by the angle (about z)
void hdScreenCannonGame_c::turnCannon() {
    hdCursorPane_c* pane = mpAnglePane;
    f32 x = pane->mRotate.x;
    f32 y = pane->mRotate.y;
    pane->mRotate.x = x;
    pane->mRotate.y = y;
    pane->mRotate.z = -mAngle;
    pane->mFlags = pane->mFlags | CURSOR_PANE_DIRTY;
}

/* 02646264 */
// a new game: all icons back to their first frame, nothing fired, the cannon at 27 degrees
void hdScreenCannonGame_c::reset() {
    mRevealedTargets = 0;
    for (u32 i = 0; i < CANNON_GAME_SHIP_COUNT; i++) {
        hdOneTimeChild(child(CANNON_GAME_SHIP_FIRST + i))->resetAnimation();
    }
    mShotCount = 0;
    for (u32 i = 0; i < CANNON_GAME_SHOT_COUNT; i++) {
        hdOneTimeChild(child(i))->resetAnimation();
    }
    mAngle = 27.0f;
    updateDegreeText();
    turnCannon();
    vCalcLayout();
}

/* 02646348 */
void hdScreenCannonGame_c::inAllEnter() {
    reset();
    hdAnimSet_play(animSet(), CANNON_GAME_ANIM_IN_ALL, 0, 1.0f);
    mVisible = TRUE;
}

/* 02646398 */
void hdScreenCannonGame_c::inAllExecute() {
    changeState(&sCannonGameWait);
}

/* 026463A8 */
void hdScreenCannonGame_c::outAllEnter() {
    hdAnimSet_play(animSet(), CANNON_GAME_ANIM_OUT_ALL, 0, 1.0f);
}

/* 026463C4 */
void hdScreenCannonGame_c::outAllExecute() {
    changeState(&sCannonGamePartsBlank);
}

/* 026463D4 */
void hdScreenCannonGame_c::outAllExit() {
    mVisible = FALSE;
}

/* 026463E0 */
// hdScreenCannonGame_c::hdScreenCannonGame_c() (GHS: allocates when called with NULL)
hdScreenCannonGame_c* hdScreenCannonGame_c::construct(hdScreenCannonGame_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenCannonGame_c*)operator_new(sizeof(hdScreenCannonGame_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdParts_c::construct(i_this);
    i_this->mVisible = FALSE;
    i_this->mAngle = 0.0f;
    i_this->mVtable = &sCannonGameVtable;
    i_this->mpAnglePane = NULL;
    i_this->mRevealedTargets = 0;
    i_this->mSecondaryVtable = &sCannonGameSecondaryVtable;
    i_this->mShotCount = 0;
    i_this->mpDegreeText = NULL;
    i_this->mStateHolder.mHolderVtable = &sCannonGameHolderVtable;
    return i_this;
}

/* 02646470 */
u32 hdScreenCannonGame_c::init(u32 arg1, u32 arg2, void* desc) {
    attachArchives(sCannonGameArchiveNames, 3);
    if (!vLoad(&sCannonGameLayoutName, gabi::ea(mpResAccessor.get()), arg1, arg2, desc)) {
        return FALSE;
    }
    if (!vCreateChildren()) {
        return FALSE;
    }
    hdCursorPane_c* root = (hdCursorPane_c*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    hdCursorPaneVtbl_c* rootVtable = root->mVtable;
    sCannonGameAnglePaneName.assureTermination();
    mpAnglePane = gabi::call_ptr<hdCursorPane_c*>(rootVtable->mFindPaneByName.get(), root, (hdCursorChar_t*)sCannonGameAnglePaneName.mText, 1u);
    rootVtable = root->mVtable;
    sCannonGameDegreePaneName.assureTermination();
    mpDegreeText = gabi::call_ptr<void*>(rootVtable->mFindPaneByName.get(), root, (hdCursorChar_t*)sCannonGameDegreePaneName.mText, 1u);
    startState(&sCannonGamePartsBlank);
    return TRUE;
}

/* 026465F8 */
void hdScreenCannonGame_c::finalize() {
    vDeleteChildren();
    vDestroyLayout();
    detachArchives();
}

/* 0264664C */
// nothing runs while hidden (the parts' Blank state)
void hdScreenCannonGame_c::executeState() {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(stateMachine());
    hdPartsStateIdVtbl_c* blankVtable = sCannonGamePartsBlank.mVtable;
    u32 key = current->getKey();
    if (key == gabi::call_ptr<u32>(blankVtable->mGetKey.get(), &sCannonGamePartsBlank)) {
        return;
    }
    hdParts_executeStateMachine(stateMachine());
    vExecuteChildren();
    vCalcLayout();
}

/* 026466F4 */
void hdScreenCannonGame_c::draw(void* info) {
    if (mVisible) {
        vDrawLayout(info);
        vDrawChildren(info);
    }
}

/* 02646764 */
// the next cannonball icon plays (one per call, however far the count moved)
void hdScreenCannonGame_c::setShotCount(u32 count) {
    u32 shots = mShotCount;
    if (shots >= count) {
        return;
    }
    hdOneTimeChild(child(shots))->mTriggered = TRUE;
    mShotCount = count;
}

/* 026467E0 */
// the cannon's elevation (22.5 at rest) mapped to the degrees shown: 15.5 + 35 * (angle - 22.5) / 22.5
void hdScreenCannonGame_c::setAngle(f32 angle) {
    mAngle = gabi::fmadds(35.0f, (angle - 22.5f) / 22.5f, 15.5f);
    updateDegreeText();
    turnCannon();
}

/* 0264683C */
// the child parts hook: "L_Cannonball_*" / "L_Battleship_*" panes get "Battleship_00" or "Cannonball_00"
// parts by their class; other classes go to the base
u32 hdScreenCannonGame_c::createChild(hdPartsDesc_c* desc) {
    hdScreenHeap* heap = sCannonGameUiSystem->mHeap;
    hdCannonGameDesc_l* names = (hdCannonGameDesc_l*)desc;
    for (s32 i = 0; i < CANNON_GAME_PANE_COUNT; i++) {
        names->mPaneName.assureTermination();
        if (!hdCannonGame_isSameName(&names->mPaneName, &sCannonGamePaneNames[i])) {
            continue;
        }
        names->mClassName.assureTermination();
        if (hdCannonGame_isSameName(&names->mClassName, &sCannonGameShipClassName)) {
            gptr<hdParts_c>* slot = childSlot(i);
            void* parts = hdParts_new(sizeof(hdScreenBattleshipIcon_c), heap, 4);
            if (parts != NULL) {
                parts = hdScreenBattleshipIcon_c::construct((hdScreenBattleshipIcon_c*)parts);
            }
            *slot = (hdParts_c*)parts;
            if (child(i) == NULL) {
                return FALSE;
            }
            return ((hdScreenBattleshipIcon_c*)child(i))->init(0, 0, desc);
        }
        names->mClassName.assureTermination();
        if (!hdCannonGame_isSameName(&names->mClassName, &sCannonGameCannonballClassName)) {
            return hdParts_c::createChild(i, desc);
        }
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
    return TRUE;
}

/* 02646BBC */
u32 hdScreenCannonGame_c::load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sCannonGameUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, arg1, arg2, arg3, 2, 1, desc)) {
        return FALSE;
    }
    for (u32 i = 0; i < 2; i++) {
        hdAnimSet_bindGroup(animSet(), i, &sCannonGameAnimNames[i], &sCannonGameGroupNames[sCannonGameAnimGroups[i]]);
    }
    vCalcLayout();
    return TRUE;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 02646CE8 */
static void __sinit_hd_screen_cannon_game_2_cpp() {
    sinit_header_statics_rodata(0x10490F40, 0x101F581C, 0x10490F4C, 0x100ED36C);
    hdParts_setSafeString(&sCannonGameGroupNames[0], &sCannonGameSafeStringVtable, "G_All_00");
    hdParts_setSafeString(&sCannonGameAnglePaneName, &sCannonGameSafeStringVtable, "N_BatteryAngle_00");
    hdParts_setSafeString(&sCannonGameDegreePaneName, &sCannonGameSafeStringVtable, "T_NumDegree_00");
    hdParts_setSafeString(&sCannonGameAnimNames[0], &sCannonGameSafeStringVtable, "InAll");
    hdParts_setSafeString(&sCannonGameAnimNames[1], &sCannonGameSafeStringVtable, "OutAll");
    hdParts_setSafeString(&sCannonGameArchiveNames[0], &sCannonGameSafeStringVtable, "CannonGame_00");
    hdParts_setSafeString(&sCannonGameArchiveNames[1], &sCannonGameSafeStringVtable, "Battleship_00");
    hdParts_setSafeString(&sCannonGameArchiveNames[2], &sCannonGameSafeStringVtable, "Cannonball_00");
    hdParts_setSafeString(&sCannonGamePaneNames[0], &sCannonGameSafeStringVtable, "L_Cannonball_00");
    hdParts_setSafeString(&sCannonGamePaneNames[1], &sCannonGameSafeStringVtable, "L_Cannonball_01");
    hdParts_setSafeString(&sCannonGamePaneNames[2], &sCannonGameSafeStringVtable, "L_Cannonball_02");
    hdParts_setSafeString(&sCannonGamePaneNames[3], &sCannonGameSafeStringVtable, "L_Cannonball_03");
    hdParts_setSafeString(&sCannonGamePaneNames[4], &sCannonGameSafeStringVtable, "L_Cannonball_04");
    hdParts_setSafeString(&sCannonGamePaneNames[5], &sCannonGameSafeStringVtable, "L_Cannonball_05");
    hdParts_setSafeString(&sCannonGamePaneNames[6], &sCannonGameSafeStringVtable, "L_Cannonball_06");
    hdParts_setSafeString(&sCannonGamePaneNames[7], &sCannonGameSafeStringVtable, "L_Cannonball_07");
    hdParts_setSafeString(&sCannonGamePaneNames[8], &sCannonGameSafeStringVtable, "L_Cannonball_08");
    hdParts_setSafeString(&sCannonGamePaneNames[9], &sCannonGameSafeStringVtable, "L_Cannonball_09");
    hdParts_setSafeString(&sCannonGamePaneNames[10], &sCannonGameSafeStringVtable, "L_Battleship_00");
    hdParts_setSafeString(&sCannonGamePaneNames[11], &sCannonGameSafeStringVtable, "L_Battleship_01");
    hdParts_setSafeString(&sCannonGamePaneNames[12], &sCannonGameSafeStringVtable, "L_Battleship_02");
    hdParts_setSafeString(&sCannonGamePaneNames[13], &sCannonGameSafeStringVtable, "L_Battleship_03");
    hdParts_setSafeString(&sCannonGamePaneNames[14], &sCannonGameSafeStringVtable, "L_Battleship_04");
    s32 number;
    if (sCannonGameStateNumberReady) {
        number = sCannonGameStateNumber;
    } else {
        number = 0;
        sCannonGameStateNumberReady = 1;
    }
    sCannonGameInAll.setup(number + 1, "::StateID_InAll", &sCannonGameStateIdVtable, 0x19, &sCannonGameNoStateId);
    sCannonGameOutAll.setup(number + 2, "::StateID_OutAll", &sCannonGameStateIdVtable, 0x1C, &sCannonGameNoStateId);
    sCannonGameWait.setup(number + 3, "::StateID_Wait", &sCannonGameStateIdVtable, 0x1F, &sCannonGameNoStateId);
    sCannonGameStateNumber = number + 3;
    hdParts_setSafeString(&sCannonGameLayoutName, &sCannonGameSafeStringVtable, "CannonGame_00");
}

/* WWHD glue
data sCannonGameVtable 100ED508 nocheck
data sCannonGameHolderVtable 100ED618 nocheck
data sCannonGameSecondaryVtable 100ED628 nocheck
data sCannonGameSafeStringVtable 100ED26C nocheck
data sCannonGameStateIdVtable 100ED2FC nocheck
data sCannonGameNoStateId 101FF32C nocheck
data sCannonGamePartsBlank 1049DD60 nocheck
data sCannonGameUiSystem 1018C404 nocheck
data sCannonGameTextVtable 100ED29C nocheck
data sCannonGameShipClassName 1048EBFC nocheck
data sCannonGameCannonballClassName 10490F20 nocheck
data sCannonGameGroupNames 10490F28 nocheck
data sCannonGameAnglePaneName 10490F30 nocheck
data sCannonGameDegreePaneName 10490F38 nocheck
data sCannonGameAnimNames 10490F5C nocheck
data sCannonGameArchiveNames 10490F6C nocheck
data sCannonGamePaneNames 10490F84 nocheck
data sCannonGameInAll 10490FFC nocheck
data sCannonGameOutAll 1049102C nocheck
data sCannonGameWait 1049105C nocheck
data sCannonGameLayoutName 1049108C nocheck
data sCannonGameAnimGroups 100ED260
data sCannonGameStateNumberReady 101FD8E4 nocheck
data sCannonGameStateNumber 101FDD50 nocheck
str 100ED34C "cannon game degree format" renamed
str 100ED4C8 "G_All_00" in 02646CE8
str 100ED4B4 "N_BatteryAngle_00" in 02646CE8
str 100ED4D4 "T_NumDegree_00" in 02646CE8
str 100ED374 "InAll" in 02646CE8
str 100ED37C "OutAll" in 02646CE8
str 100ED484 "CannonGame_00" in 02646CE8
str 100ED494 "Battleship_00" in 02646CE8
str 100ED4A4 "Cannonball_00" in 02646CE8
str 100ED384 "L_Cannonball_00" in 02646CE8
str 100ED394 "L_Cannonball_01" in 02646CE8
str 100ED3A4 "L_Cannonball_02" in 02646CE8
str 100ED3B4 "L_Cannonball_03" in 02646CE8
str 100ED3C4 "L_Cannonball_04" in 02646CE8
str 100ED3D4 "L_Cannonball_05" in 02646CE8
str 100ED3E4 "L_Cannonball_06" in 02646CE8
str 100ED3F4 "L_Cannonball_07" in 02646CE8
str 100ED404 "L_Cannonball_08" in 02646CE8
str 100ED414 "L_Cannonball_09" in 02646CE8
str 100ED424 "L_Battleship_00" in 02646CE8
str 100ED434 "L_Battleship_01" in 02646CE8
str 100ED444 "L_Battleship_02" in 02646CE8
str 100ED454 "L_Battleship_03" in 02646CE8
str 100ED464 "L_Battleship_04" in 02646CE8
str 100ED474 "::StateID_InAll" in 02646CE8
str 100ED4E4 "::StateID_OutAll" in 02646CE8
str 100ED4F8 "::StateID_Wait" in 02646CE8
*/

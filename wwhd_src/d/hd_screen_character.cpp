/**
 * hd_screen_character.cpp
 * HD - the "Character" parts of the staff roll bubbles (layout "Bubble_00"): the frame of the "Character"
 * animation selects the character shown; the "Loop" animation starts at a random frame; state
 * ::StateID_Loop
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_mini_auction_battleship_3more). Each function is marked with its WWHD address; the glue block at
 * the end lists this translation unit's data (tools/verify/autoglue.py). Layout, pane, animation and state
 * names are HD strings; all other names are ours.
 */

#include "d/hd_screen_character.h"

static hdPartsVtbl_c sCharacterVtable;
static u32 sCharacterHolderVtable;
static u32 sCharacterSecondaryVtable;
static hdUiStringTable sCharacterSafeStringVtable;
static hdPartsStateIdVtbl_c sCharacterStateIdVtable;
static hdStateId sCharacterNoStateId;
static hdScreenUiSystem* sCharacterUiSystem;
static void* sCharacterRandom;

static hdUiSafeString sCharacterGroupNames[2] = {};
static hdUiSafeString sCharacterAnimNames[2] = {};
/* the layout's panes (not used by this code) */
static hdUiSafeString sCharacterPaneNames[8] = {};
static hdPartsStateId_c sCharacterLoop;
static hdUiSafeString sCharacterLayoutName;
static const u32 sCharacterAnimGroups[2] = {1, 0};
static u32 sCharacterStateNumberReady;
static s32 sCharacterStateNumber;

/* 02645724 */
// hdScreenCharacter_c::~hdScreenCharacter_c() (GHS deleting destructor)
void hdScreenCharacter_c::destroy(hdScreenCharacter_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sCharacterHolderVtable;
    i_this->mSecondaryVtable = &sCharacterSecondaryVtable;
    i_this->NoControlPart_l::destroy(0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 02645790 */
// the loop starts at a random frame, so bubbles side by side do not move in step
void hdScreenCharacter_c::loopEnter() {
    hdAnimSet_play(animSet(), CHARACTER_ANIM_LOOP, 0, 1.0f);
    hdPartsLayout_c* layout = mpLayout;
    hdAnimSetEntry* entry = *layout->animSet()->mEntries.at(CHARACTER_ANIM_LOOP);
    void* random = sCharacterRandom;
    f32 frames = (f32)hdCharacter_getFrameCount(entry->mController.get());
    u32 value = hdCharacter_random(random);
    hdCharacter_setAnimFrame(layout->animSet(), CHARACTER_ANIM_LOOP, 0, ((f32)value * 2.3283064e-10f) * frames);
}

/* 02645890 */
void hdScreenCharacter_c::loopExecute() {
    vCalcLayout();
}

/* 026458A0 */
// hdScreenCharacter_c::hdScreenCharacter_c() (GHS: allocates when called with NULL)
hdScreenCharacter_c* hdScreenCharacter_c::construct(hdScreenCharacter_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenCharacter_c*)operator_new(sizeof(hdScreenCharacter_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    NoControlPart_l::construct(i_this);
    i_this->mVtable = &sCharacterVtable;
    i_this->mStateHolder.mHolderVtable = &sCharacterHolderVtable;
    i_this->mSecondaryVtable = &sCharacterSecondaryVtable;
    return i_this;
}

/* 0264590C */
u32 hdScreenCharacter_c::init(u32 arg1, u32 arg2, void* desc) {
    if (!create(&sCharacterLayoutName, gabi::at<u8>(arg1), arg2, gabi::ea(desc))) {
        return FALSE;
    }
    startState(&sCharacterLoop);
    return TRUE;
}

/* 026459A0 */
u32 hdScreenCharacter_c::load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sCharacterUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, arg1, arg2, arg3, 2, 2, desc)) {
        return FALSE;
    }
    for (u32 i = 0; i < 2; i++) {
        hdCharacter_bindGroup(animSet(), i, &sCharacterAnimNames[i], &sCharacterGroupNames[sCharacterAnimGroups[i]]);
    }
    return TRUE;
}

/* 02645AB8 */
// show character `frame`: the "Character" animation's frame, clamped to 0..its length
void hdScreenCharacter_c::setCharacter(s32 frame) {
    u32 frames = hdCharacter_getFrameCount(animSet()->mEntries.mData.get()->get()->mController.get());
    f32 value = (f32)frame;
    f32 last = (f32)frames;
    if (value < 0.0f) {
        hdAnimSet_setFrame(animSet(), CHARACTER_ANIM_CHARACTER, 1, 0.0f);
        return;
    }
    if (value > last) {
        value = last;
    }
    hdAnimSet_setFrame(animSet(), CHARACTER_ANIM_CHARACTER, 1, value);
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 02645B88 */
static void __sinit_hd_screen_character_cpp() {
    sinit_header_statics_rodata(0x10490E48, 0x101F57D4, 0x10490E54, 0x100ECF6C);
    hdParts_setSafeString(&sCharacterGroupNames[0], &sCharacterSafeStringVtable, "G_Loop_00");
    hdParts_setSafeString(&sCharacterGroupNames[1], &sCharacterSafeStringVtable, "G_Character_00");
    hdParts_setSafeString(&sCharacterAnimNames[0], &sCharacterSafeStringVtable, "Character");
    hdParts_setSafeString(&sCharacterAnimNames[1], &sCharacterSafeStringVtable, "Loop");
    hdParts_setSafeString(&sCharacterPaneNames[0], &sCharacterSafeStringVtable, "N_Bubble_00");
    hdParts_setSafeString(&sCharacterPaneNames[1], &sCharacterSafeStringVtable, "N_Character_00");
    hdParts_setSafeString(&sCharacterPaneNames[2], &sCharacterSafeStringVtable, "P_Character_00");
    hdParts_setSafeString(&sCharacterPaneNames[3], &sCharacterSafeStringVtable, "P_Rainbow_00");
    hdParts_setSafeString(&sCharacterPaneNames[4], &sCharacterSafeStringVtable, "P_Bubble_00");
    hdParts_setSafeString(&sCharacterPaneNames[5], &sCharacterSafeStringVtable, "P_Light_00");
    hdParts_setSafeString(&sCharacterPaneNames[6], &sCharacterSafeStringVtable, "P_Character_01");
    hdParts_setSafeString(&sCharacterPaneNames[7], &sCharacterSafeStringVtable, "P_Light_01");
    s32 number;
    if (sCharacterStateNumberReady) {
        number = sCharacterStateNumber;
    } else {
        number = 0;
        sCharacterStateNumberReady = 1;
    }
    sCharacterLoop.setup(number + 1, "::StateID_Loop", &sCharacterStateIdVtable, 0x19, &sCharacterNoStateId);
    sCharacterStateNumber = number + 1;
    hdParts_setSafeString(&sCharacterLayoutName, &sCharacterSafeStringVtable, "Bubble_00");
}

/* 02645624 */
static s32 hdScreenCharacter_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* 02645E4C */
// a second copy of the template, after this TU's static initialiser
static s32 hdScreenCharacter_getStateNumberCopy(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue
data sCharacterVtable 100ED030 nocheck
data sCharacterHolderVtable 100ED110 nocheck
data sCharacterSecondaryVtable 100ED120 nocheck
data sCharacterSafeStringVtable 100ECE9C nocheck
data sCharacterStateIdVtable 100ECEFC nocheck
data sCharacterNoStateId 101FF32C nocheck
data sCharacterUiSystem 1018C404 nocheck
data sCharacterRandom 101F8B60 nocheck
data sCharacterGroupNames 10490E64 nocheck
data sCharacterAnimNames 10490E74 nocheck
data sCharacterPaneNames 10490E84 nocheck
data sCharacterLoop 10490EC4 nocheck
data sCharacterLayoutName 10490EF4 nocheck
data sCharacterAnimGroups 100ECE90
data sCharacterStateNumberReady 101FD8E4 nocheck
data sCharacterStateNumber 101FDD50 nocheck
str 100ECF94 "G_Loop_00" in 02645B88
str 100ECFB8 "G_Character_00" in 02645B88
str 100ECFA0 "Character" in 02645B88
str 100ECF74 "Loop" in 02645B88
str 100ECF7C "N_Bubble_00" in 02645B88
str 100ECFC8 "N_Character_00" in 02645B88
str 100ECFD8 "P_Character_00" in 02645B88
str 100ECFE8 "P_Rainbow_00" in 02645B88
str 100ECF88 "P_Bubble_00" in 02645B88
str 100ECFF8 "P_Light_00" in 02645B88
str 100ED004 "P_Character_01" in 02645B88
str 100ED014 "P_Light_01" in 02645B88
str 100ED020 "::StateID_Loop" in 02645B88
str 100ECFAC "Bubble_00" in 02645B88
ext 02700A70 void NoControlPart_l::destroy(u32)
ext 02700ADC NoControlPart_l* NoControlPart_l::construct(NoControlPart_l*)
ext 02700B48 u32 NoControlPart_l::create(void*,void*,u32,u32)
ext 02004E04 void hdCharacter_bindGroup(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*)
ext 0200579C void hdCharacter_setAnimFrame(hdAnimSet*, u32, u32, f32)
ext 028713B8 u32 hdCharacter_getFrameCount(hdAnimSetController*)
ext 0275CEE4 u32 hdCharacter_random(void*)
*/

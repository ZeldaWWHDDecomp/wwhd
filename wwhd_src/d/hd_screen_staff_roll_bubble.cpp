/**
 * hd_screen_staff_roll_bubble.cpp
 * HD - the staff roll's big bubble "StaffRollBubble_00": a bubble with a character (the character parts, layout
 * "Bubble_00", in pane "L_Bubble_00") that takes a motion and rises (states of hd_screen_staff_roll_bubble_base.cpp)
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_title_staff_roll). Each function is marked with its WWHD address; the glue block at the end
 * lists this translation unit's data (tools/verify/autoglue.py). Layout, pane and class names are HD
 * strings; all other names are ours.
 */

#include "d/hd_screen_staff_roll_bubble.h"

static hdPartsVtbl_c sBubbleVtable;
static u32 sBubbleHolderVtable;
static u32 sBubbleSecondaryVtable;
static hdUiStringTable sBubbleSafeStringVtable;
static hdScreenUiSystem* sBubbleUiSystem;
static hdUiSafeString sBubbleCharacterPartsName; /* "Bubble_00": the character parts' layout (hd_screen_character.cpp) */
static hdStaffRollBubbleStateId_c sBubbleWait;   /* the base's states */
static hdStaffRollBubbleStateId_c sBubbleMove;

static hdUiSafeString sBubblePaneName;
static hdUiSafeString sBubbleCharacterLayoutName; /* the second archive's name, "Bubble_00" (not used by this code) */
static hdUiSafeString sBubbleArchiveNames[2] = {};
static hdUiSafeString sBubbleLayoutName;

/* 026E2B04 */
// hdStaffRollBubble_c::~hdStaffRollBubble_c() (GHS deleting destructor)
void hdStaffRollBubble_c::destroy(hdStaffRollBubble_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sBubbleHolderVtable;
    i_this->mSecondaryVtable = &sBubbleSecondaryVtable;
    hdStaffRollBubbleBase_c::destroy(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 026E2B70 */
// hdStaffRollBubble_c::hdStaffRollBubble_c() (GHS: allocates when called with NULL)
hdStaffRollBubble_c* hdStaffRollBubble_c::construct(hdStaffRollBubble_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdStaffRollBubble_c*)operator_new(sizeof(hdStaffRollBubble_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdStaffRollBubbleBase_c::construct(i_this);
    i_this->mVtable = &sBubbleVtable;
    i_this->mSecondaryVtable = &sBubbleSecondaryVtable;
    i_this->mStateHolder.mHolderVtable = &sBubbleHolderVtable;
    // HD: GHS tests the address of the (trivially constructed) pane pointer member
    if (gabi::ea(&i_this->mpCharacterPane) == 0) {
        operator_new(sizeof(i_this->mpCharacterPane));
    }
    i_this->mStartPos.x = 0.0f;
    i_this->mStartPos.z = 0.0f;
    i_this->mStartPos.y = 0.0f;
    return i_this;
}

/* 026E2C00 */
// the layout and its character part, placed at the start height; waits until started
u32 hdStaffRollBubble_c::init(u32 arg1, u32 arg2, void* desc) {
    attachArchives(sBubbleArchiveNames, 2);
    if (!vLoad(&sBubbleLayoutName, gabi::ea(mpResAccessor.get()), arg1, arg2, desc)) {
        return FALSE;
    }
    if (!vCreateChildren()) {
        return FALSE;
    }
    hdStaffRollPane_c* root = (hdStaffRollPane_c*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    hdStaffRollPaneVtbl_c* rootVtable = root->mVtable;
    sBubblePaneName.assureTermination();
    mpCharacterPane = gabi::call_ptr<hdStaffRollPane_c*>(rootVtable->mFindPaneByName.get(), root, (be<u8>*)sBubblePaneName.mText, 1u);
    hdStaffRollBubblePane_c* pane = rootPane();
    pane->mFlags = pane->mFlags | STAFF_ROLL_BUBBLE_PANE_INFLUENCED_ALPHA;
    f32 y = getStartY();
    mStartPos.x = 0.0f;
    mStartPos.z = 0.0f;
    mStartPos.y = y;
    pane = rootPane();
    pane->mTranslate.x = 0.0f;
    pane->mTranslate.y = mStartPos.y;
    pane->mTranslate.z = 0.0f;
    pane->mFlags = pane->mFlags | STAFF_ROLL_BUBBLE_PANE_DIRTY;
    vExecuteChildren();
    vCalcLayout();
    startState((hdPartsStateId_c*)&sBubbleWait);
    return TRUE;
}

/* 026E2DAC */
void hdStaffRollBubble_c::finalize() {
    vDeleteChildren();
    vDestroyLayout();
    detachArchives();
}

/* 026E2E00 */
// the child parts hook: pane "L_Bubble_00" gets the character parts ("Bubble_00"); other panes are left to the base
u32 hdStaffRollBubble_c::createChild(hdPartsDesc_c* desc) {
    hdScreenHeap* heap = sBubbleUiSystem->mHeap;
    hdStaffRollBubbleDesc_l* names = (hdStaffRollBubbleDesc_l*)desc;
    names->mPaneName.assureTermination();
    if (!hdStaffRollBubble_isSameName(&names->mPaneName, &sBubblePaneName)) {
        return TRUE;
    }
    names->mClassName.assureTermination();
    if (!hdStaffRollBubble_isSameName(&names->mClassName, &sBubbleCharacterPartsName)) {
        return hdParts_c::createChild(0, desc);
    }
    gptr<hdParts_c>* slot = childSlot(0);
    void* parts = hdParts_new(STAFF_ROLL_BUBBLE_CHARACTER_SIZE, heap, 4);
    if (parts != NULL) {
        parts = hdStaffRollBubble_constructCharacter(parts);
    }
    *slot = (hdParts_c*)parts;
    if (child(0) == NULL) {
        return FALSE;
    }
    return hdStaffRollBubble_initCharacter(child(0), 0, 0, desc);
}

/* 026E3008 */
void hdStaffRollBubble_c::showCharacter(u32 frame) {
    hdStaffRollBubble_setCharacterFrame(child(0), frame);
    vExecuteChildren();
    vCalcLayout();
}

/* 026E306C */
// take `motion` (character, kind, scale) and start rising
void hdStaffRollBubble_c::start(hdStaffRollBubbleMotion_c* motion) {
    mMotion.copy(motion);
    showCharacter(mMotion.mType);
    hdStaffRollBubblePane_c* pane = rootPane();
    f32 scaleX = mMotion.getScale();
    f32 scaleY = mMotion.getScale();
    pane->mScaleY = scaleY;
    pane->mScaleX = scaleX;
    pane->mFlags = pane->mFlags | STAFF_ROLL_BUBBLE_PANE_DIRTY;
    pane = rootPane();
    pane->mAlpha = (u8)gabi::ftoi(getAlpha());
    changeState((hdPartsStateId_c*)&sBubbleMove);
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 026E3134 */
static void __sinit_hd_screen_staff_roll_bubble_cpp() {
    hdStaffRoll_initHeaderStatics(0x1049C29C, 0x101F6C04, 0x1049C290, 0x10106478, 0x1049C298);
    hdParts_setSafeString(&sBubblePaneName, &sBubbleSafeStringVtable, "L_Bubble_00");
    hdParts_setSafeString(&sBubbleCharacterLayoutName, &sBubbleSafeStringVtable, "Bubble_00");
    hdParts_setSafeString(&sBubbleLayoutName, &sBubbleSafeStringVtable, "StaffRollBubble_00");
    hdParts_setSafeString(&sBubbleArchiveNames[0], &sBubbleSafeStringVtable, "StaffRollBubble_00");
    hdParts_setSafeString(&sBubbleArchiveNames[1], &sBubbleSafeStringVtable, "Bubble_00");
}

/* WWHD glue
data sBubbleVtable 101064B0 nocheck
data sBubbleHolderVtable 10106578 nocheck
data sBubbleSecondaryVtable 10106588 nocheck
data sBubbleSafeStringVtable 10106454 nocheck
data sBubbleUiSystem 1018C404 nocheck
data sBubbleCharacterPartsName 10490EF4 nocheck
data sBubbleWait 1049C36C nocheck
data sBubbleMove 1049C398 nocheck
data sBubblePaneName 1049C280 nocheck
data sBubbleCharacterLayoutName 1049C288 nocheck
data sBubbleArchiveNames 1049C2AC nocheck
data sBubbleLayoutName 1049C2BC nocheck
str 10106480 "L_Bubble_00" in 026E3134
str 1010648C "Bubble_00" in 026E3134
str 10106498 "StaffRollBubble_00" in 026E3134
ext 0275CEE4 u32 hdStaffRollBubble_random(void*)
ext 028F43F8 f64 hdStaffRollBubble_sin(f64)
ext 0275CE48 void hdStaffRollBubble_seedRandom(void*, u32)
ext 026458A0 void* hdStaffRollBubble_constructCharacter(void*)
ext 0264590C u32 hdStaffRollBubble_initCharacter(void*, u32, u32, void*)
ext 02645AB8 void hdStaffRollBubble_setCharacterFrame(void*, u32)
*/

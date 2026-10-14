/**
 * hd_screen_staff_roll_bubble_s.cpp
 * HD - the staff roll's small bubble "StaffRollBubbleS_00": a bubble with a character (the character
 * parts, layout "Bubble_00", in pane "L_BubbleS_00") whose size, velocity and sway come from its own random seed (states of
 * hd_screen_staff_roll_bubble_base.cpp)
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_title_staff_roll). Each function is marked with its WWHD address; the glue block at the end
 * lists this translation unit's data (tools/verify/autoglue.py). Layout, pane and class names are HD
 * strings; all other names are ours.
 */

#include "d/hd_screen_staff_roll_bubble.h"

static hdPartsVtbl_c sBubbleSVtable;
static u32 sBubbleSHolderVtable;
static u32 sBubbleSSecondaryVtable;
static hdUiStringTable sBubbleSSafeStringVtable;
static hdScreenUiSystem* sBubbleSUiSystem;
static void* sBubbleSRandom;
static hdUiSafeString sBubbleSCharacterPartsName; /* "Bubble_00": the character parts' layout (hd_screen_character.cpp) */
static hdStaffRollBubbleStateId_c sBubbleSWait;   /* the base's states */
static hdStaffRollBubbleStateId_c sBubbleSMove;

static hdUiSafeString sBubbleSPaneName;
static hdUiSafeString sBubbleSCharacterLayoutName; /* the second archive's name, "Bubble_00" (not used by this code) */
static hdUiSafeString sBubbleSArchiveNames[2] = {};
static hdUiSafeString sBubbleSLayoutName;

/* 026E4688 */
// hdStaffRollBubbleS_c::~hdStaffRollBubbleS_c() (GHS deleting destructor)
void hdStaffRollBubbleS_c::destroy(hdStaffRollBubbleS_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sBubbleSHolderVtable;
    i_this->mSecondaryVtable = &sBubbleSSecondaryVtable;
    hdStaffRollBubbleBase_c::destroy(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 026E46F4 */
// hdStaffRollBubbleS_c::hdStaffRollBubbleS_c() (GHS: allocates when called with NULL)
hdStaffRollBubbleS_c* hdStaffRollBubbleS_c::construct(hdStaffRollBubbleS_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdStaffRollBubbleS_c*)operator_new(sizeof(hdStaffRollBubbleS_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdStaffRollBubbleBase_c::construct(i_this);
    i_this->mVtable = &sBubbleSVtable;
    i_this->mSecondaryVtable = &sBubbleSSecondaryVtable;
    i_this->mStateHolder.mHolderVtable = &sBubbleSHolderVtable;
    // HD: GHS tests the address of the (trivially constructed) pane pointer member
    if (gabi::ea(&i_this->mpCharacterPane) == 0) {
        operator_new(sizeof(i_this->mpCharacterPane));
    }
    i_this->mSeed = 0;
    return i_this;
}

/* 026E4778 */
// the layout and its character part; size, velocity, acceleration and sway drawn from `seed`
u32 hdStaffRollBubbleS_c::init(u32 seed, u32 arg1, u32 arg2, void* desc) {
    mSeed = seed;
    attachArchives(sBubbleSArchiveNames, 2);
    if (!vLoad(&sBubbleSLayoutName, gabi::ea(mpResAccessor.get()), arg1, arg2, desc)) {
        return FALSE;
    }
    if (!vCreateChildren()) {
        return FALSE;
    }
    hdStaffRollPane_c* root = (hdStaffRollPane_c*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    hdStaffRollPaneVtbl_c* rootVtable = root->mVtable;
    sBubbleSPaneName.assureTermination();
    mpCharacterPane = gabi::call_ptr<hdStaffRollPane_c*>(rootVtable->mFindPaneByName.get(), root, (be<u8>*)sBubbleSPaneName.mText, 1u);
    hdStaffRollBubble_seedRandom(sBubbleSRandom, mSeed);
    f32 y = getStartY();
    gabi::Local<cXyz> pos;
    pos->x = 0.0f;
    pos->y = y;
    pos->z = 0.0f;
    u32 random = hdStaffRollBubble_random(sBubbleSRandom);
    f32 swayAmplitude = ((f32)random * 2.3283064e-10f) * 0.5f;
    random = hdStaffRollBubble_random(sBubbleSRandom);
    s32 swayStep = (u32)(((u64)random * 15) >> 32);
    f32 paneScale = getRandomScale();
    f32 velocityX = getRandomSwayX();
    gabi::Local<cXyz> velocity;
    velocity->y = getRandomRise();
    velocity->z = 0.0f;
    velocity->x = velocityX;
    f32 accel = getRandomAccel();
    f32 scale = getRandomScale();
    mMotion.set(0, pos, velocity, swayStep, 0, accel, swayAmplitude, scale);
    hdStaffRollBubblePane_c* pane = rootPane();
    pane->mScaleY = paneScale;
    pane->mScaleX = paneScale;
    pane->mFlags = pane->mFlags | STAFF_ROLL_BUBBLE_PANE_DIRTY;
    pane = rootPane();
    pane->mFlags = pane->mFlags | STAFF_ROLL_BUBBLE_PANE_INFLUENCED_ALPHA;
    pane = rootPane();
    pane->mAlpha = (u8)gabi::ftoi(getAlpha());
    pane = rootPane();
    pane->mTranslate.copy(*pos);
    pane->mFlags = pane->mFlags | STAFF_ROLL_BUBBLE_PANE_DIRTY;
    vExecuteChildren();
    vCalcLayout();
    startState((hdPartsStateId_c*)&sBubbleSWait);
    return TRUE;
}

/* 026E4AB8 */
void hdStaffRollBubbleS_c::finalize() {
    vDeleteChildren();
    vDestroyLayout();
    detachArchives();
}

/* 026E4B0C */
// the child parts hook: pane "L_BubbleS_00" gets the character parts ("Bubble_00"); other panes are left to the base
u32 hdStaffRollBubbleS_c::createChild(hdPartsDesc_c* desc) {
    hdScreenHeap* heap = sBubbleSUiSystem->mHeap;
    hdStaffRollBubbleDesc_l* names = (hdStaffRollBubbleDesc_l*)desc;
    names->mPaneName.assureTermination();
    if (!hdStaffRollBubble_isSameName(&names->mPaneName, &sBubbleSPaneName)) {
        return TRUE;
    }
    names->mClassName.assureTermination();
    if (!hdStaffRollBubble_isSameName(&names->mClassName, &sBubbleSCharacterPartsName)) {
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

/* 026E4D14 */
// from the same seed again: back to a random start place and rising
void hdStaffRollBubbleS_c::restart() {
    hdStaffRollBubble_seedRandom(sBubbleSRandom, mSeed);
    mMotion.respawn();
    changeState((hdPartsStateId_c*)&sBubbleSMove);
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 026E4D64 */
static void __sinit_hd_screen_staff_roll_bubble_s_cpp() {
    hdStaffRoll_initHeaderStatics(0x1049C3E0, 0x101F6EA4, 0x1049C3D4, 0x10106880, 0x1049C3DC);
    hdParts_setSafeString(&sBubbleSPaneName, &sBubbleSSafeStringVtable, "L_BubbleS_00");
    hdParts_setSafeString(&sBubbleSCharacterLayoutName, &sBubbleSSafeStringVtable, "Bubble_00");
    hdParts_setSafeString(&sBubbleSLayoutName, &sBubbleSSafeStringVtable, "StaffRollBubbleS_00");
    hdParts_setSafeString(&sBubbleSArchiveNames[0], &sBubbleSSafeStringVtable, "StaffRollBubbleS_00");
    hdParts_setSafeString(&sBubbleSArchiveNames[1], &sBubbleSSafeStringVtable, "Bubble_00");
}

/* WWHD glue
data sBubbleSVtable 101068B8 nocheck
data sBubbleSHolderVtable 10106988 nocheck
data sBubbleSSecondaryVtable 10106998 nocheck
data sBubbleSSafeStringVtable 1010684C nocheck
data sBubbleSUiSystem 1018C404 nocheck
data sBubbleSRandom 101F8B60 nocheck
data sBubbleSCharacterPartsName 10490EF4 nocheck
data sBubbleSWait 1049C36C nocheck
data sBubbleSMove 1049C398 nocheck
data sBubbleSPaneName 1049C3C4 nocheck
data sBubbleSCharacterLayoutName 1049C3CC nocheck
data sBubbleSArchiveNames 1049C3F0 nocheck
data sBubbleSLayoutName 1049C400 nocheck
str 101068A8 "L_BubbleS_00" in 026E4D64
str 1010689C "Bubble_00" in 026E4D64
str 10106888 "StaffRollBubbleS_00" in 026E4D64
*/

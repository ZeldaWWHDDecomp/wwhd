/**
 * hd_screen_rupy_sword_counter_2.cpp
 * HD - the sword minigame's rupee counter "RupySwordCounter_00": a "BCollectIcon_00" part, the count in
 * "T_Num_00"; states ::StateID_Wait / _In / _Disp / _Out
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_mini_auction_battleship_3more). Each function is marked with its WWHD address; the glue block at
 * the end lists this translation unit's data (tools/verify/autoglue.py). Layout, pane, class, animation
 * and state names are HD strings; all other names are ours.
 */

#include "d/hd_screen_rupy_sword_counter_2.h"

static hdPartsVtbl_c sRupySwordVtable;
static u32 sRupySwordHolderVtable;
static u32 sRupySwordSecondaryVtable;
static hdUiStringTable sRupySwordSafeStringVtable;
static hdPartsStateIdVtbl_c sRupySwordStateIdVtable;
static hdStateId sRupySwordNoStateId;
static hdScreenUiSystem* sRupySwordUiSystem;
static hdUiSafeString sRupySwordCollectIconClassName; /* the collection icon's class (MENU-btn_collect_icon) */
static u8 sRupySwordActive;      /* the counter is wanted (set by the minigame) */
static u8 sRupySwordKind;        /* the minigame kind: selects the picture */
static const u32 sRupySwordPictures[5] = {1, 0, 0, 0, 0};

static hdUiSafeString sRupySwordIconPaneName;
static hdUiSafeString sRupySwordNumPaneName;
static hdUiSafeString sRupySwordGroupNames[2] = {};
static hdUiSafeString sRupySwordAnimNames[3] = {};
static hdPartsStateId_c sRupySwordWait;
static hdPartsStateId_c sRupySwordIn;
static hdPartsStateId_c sRupySwordDisp;
static hdPartsStateId_c sRupySwordOut;
static hdUiSafeString sRupySwordLayoutName;
static const u32 sRupySwordAnimGroups[3] = {1, 0, 1};
static u32 sRupySwordStateNumberReady;
static s32 sRupySwordStateNumber;

/* inlined into createChild: sead SafeString equality (both strings terminated first) */
static bool hdRupySword_isSameName(hdUiSafeString* name, hdUiSafeString* other) {
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
struct hdRupySwordDesc_l {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ hdUiSafeString mClassName;
    /* 0x10 */ u8 _10[4];
    /* 0x14 */ hdUiSafeString mPaneName;
};

/* 026CB24C */
// hdScreenRupySwordCounter_c::~hdScreenRupySwordCounter_c() (GHS deleting destructor)
void hdScreenRupySwordCounter_c::destroy(hdScreenRupySwordCounter_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sRupySwordHolderVtable;
    i_this->mSecondaryVtable = &sRupySwordSecondaryVtable;
    i_this->NoControlPart_l::destroy(0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 026CB2B8 */
void hdScreenRupySwordCounter_c::waitEnter() {
    hdAnimSet_setFrame(animSet(), RUPY_SWORD_ANIM_IN_ALL, 1, 0.0f);
}

/* 026CB2D4 */
void hdScreenRupySwordCounter_c::waitExecute() {
    if (sRupySwordActive) {
        changeState(&sRupySwordIn);
    }
}

/* 026CB370 */
// slide in with the picture of this minigame (the sword training shows picture 2) and the current count
void hdScreenRupySwordCounter_c::inEnter() {
    hdAnimSet_play(animSet(), RUPY_SWORD_ANIM_IN_ALL, 1, 1.0f);
    u32 kind = sRupySwordKind;
    if (kind >= 5) {
        kind = 0;
    }
    u32 picture = sRupySwordPictures[kind];
    if (((hdRupySwordPlay_l*)dComIfGp_get())->mMinigame == 7) {
        picture = 2;
    }
    hdAnimSet_setFrame(animSet(), RUPY_SWORD_ANIM_RUPY_SWORD, 0, (f32)picture);
    if (picture == 0) {
        hdBtnCollectIcon_update_l(firstChild());
    }
    mShownCount = ((hdRupySwordPlay_l*)dComIfGp_get())->mTargetCount;
    updateDisplay();
}

/* 026CB450 */
void hdScreenRupySwordCounter_c::inExecute() {
    if (hdAnimSet_isEnd(animSet(), RUPY_SWORD_ANIM_IN_ALL)) {
        changeState(&sRupySwordDisp);
    }
}

/* 026CB544 */
void hdScreenRupySwordCounter_c::outEnter() {
    hdAnimSet_play(animSet(), RUPY_SWORD_ANIM_OUT_ALL, 1, 1.0f);
}

/* 026CB560 */
void hdScreenRupySwordCounter_c::outExecute() {
    if (hdAnimSet_isEnd(animSet(), RUPY_SWORD_ANIM_OUT_ALL)) {
        changeState(&sRupySwordWait);
    }
}

/* 026CB5B0 */
// hdScreenRupySwordCounter_c::hdScreenRupySwordCounter_c() (GHS: allocates when called with NULL)
hdScreenRupySwordCounter_c* hdScreenRupySwordCounter_c::construct(hdScreenRupySwordCounter_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenRupySwordCounter_c*)operator_new(sizeof(hdScreenRupySwordCounter_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    NoControlPart_l::construct(i_this);
    i_this->mStateHolder.mHolderVtable = &sRupySwordHolderVtable;
    i_this->mVtable = &sRupySwordVtable;
    i_this->mSecondaryVtable = &sRupySwordSecondaryVtable;
    i_this->mShownCount = 0;
    i_this->mpNumText = NULL;
    return i_this;
}

/* 026CB628 */
u32 hdScreenRupySwordCounter_c::init(u32 arg1, u32 arg2, void* desc) {
    if (!create(&sRupySwordLayoutName, gabi::at<u8>(arg1), arg2, gabi::ea(desc))) {
        return FALSE;
    }
    ((hdBtnCollectIcon_l*)firstChild())->mKind = 0;
    hdCursorPane_c* root = (hdCursorPane_c*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    hdCursorPaneVtbl_c* rootVtable = root->mVtable;
    sRupySwordNumPaneName.assureTermination();
    mpNumText = gabi::call_ptr<void*>(rootVtable->mFindPaneByName.get(), root, (hdCursorChar_t*)sRupySwordNumPaneName.mText, 1u);
    startState(&sRupySwordWait);
    return TRUE;
}

/* 026CB738 */
void hdScreenRupySwordCounter_c::hide() {
    changeState(&sRupySwordWait);
}

/* 026CB748 */
// the child parts hook: pane "L_BCollectIcon_00" gets the collection icon; other classes go to the base
u32 hdScreenRupySwordCounter_c::createChild(hdPartsDesc_c* desc) {
    hdRupySwordDesc_l* names = (hdRupySwordDesc_l*)desc;
    sRupySwordCollectIconClassName.assureTermination();
    u32 iconClass = gabi::ea(sRupySwordCollectIconClassName.mText.get());
    names->mPaneName.assureTermination();
    if (!hdRupySword_isSameName(&names->mPaneName, &sRupySwordIconPaneName)) {
        return TRUE;
    }
    gabi::Local<hdUiSafeString> className;
    className->mText.v = iconClass;
    className->mVtable = &sRupySwordSafeStringVtable;
    names->mClassName.assureTermination();
    if (!hdRupySword_isSameName(&names->mClassName, className)) {
        return hdParts_c::createChild(0, desc);
    }
    gptr<hdParts_c>* slot = childSlot(0);
    *slot = hdBtnCollectIcon_create_l(0, 0, desc);
    if (child(0) == NULL) {
        return FALSE;
    }
    return TRUE;
}

/* 026CB950 */
u32 hdScreenRupySwordCounter_c::load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sRupySwordUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, arg1, arg2, arg3, 3, 2, desc)) {
        return FALSE;
    }
    for (u32 i = 0; i < 3; i++) {
        hdAnimSet_bindGroup(animSet(), i, &sRupySwordAnimNames[i], &sRupySwordGroupNames[sRupySwordAnimGroups[i]]);
    }
    return TRUE;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 026CBA68 */
static void __sinit_hd_screen_rupy_sword_counter_2_cpp() {
    sinit_header_statics_rodata(0x1049A2D4, 0x101F6998, 0x1049A2E0, 0x10101E54);
    hdParts_setSafeString(&sRupySwordIconPaneName, &sRupySwordSafeStringVtable, "L_BCollectIcon_00");
    hdParts_setSafeString(&sRupySwordNumPaneName, &sRupySwordSafeStringVtable, "T_Num_00");
    hdParts_setSafeString(&sRupySwordGroupNames[0], &sRupySwordSafeStringVtable, "G_RupySword_00");
    hdParts_setSafeString(&sRupySwordGroupNames[1], &sRupySwordSafeStringVtable, "G_All_00");
    hdParts_setSafeString(&sRupySwordAnimNames[0], &sRupySwordSafeStringVtable, "InAll");
    hdParts_setSafeString(&sRupySwordAnimNames[1], &sRupySwordSafeStringVtable, "RupySword");
    hdParts_setSafeString(&sRupySwordAnimNames[2], &sRupySwordSafeStringVtable, "OutAll");
    s32 number;
    if (sRupySwordStateNumberReady) {
        number = sRupySwordStateNumber;
    } else {
        number = 0;
        sRupySwordStateNumberReady = 1;
    }
    sRupySwordWait.setup(number + 1, "::StateID_Wait", &sRupySwordStateIdVtable, 0x19, &sRupySwordNoStateId);
    sRupySwordIn.setup(number + 2, "::StateID_In", &sRupySwordStateIdVtable, 0x1C, &sRupySwordNoStateId);
    sRupySwordDisp.setup(number + 3, "::StateID_Disp", &sRupySwordStateIdVtable, 0x1F, &sRupySwordNoStateId);
    sRupySwordOut.setup(number + 4, "::StateID_Out", &sRupySwordStateIdVtable, 0x22, &sRupySwordNoStateId);
    sRupySwordStateNumber = number + 4;
    hdParts_setSafeString(&sRupySwordLayoutName, &sRupySwordSafeStringVtable, "RupySwordCounter_00");
}

/* 026CB14C */
static s32 hdScreenRupySwordCounter_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue
data sRupySwordVtable 10101F08 nocheck
data sRupySwordHolderVtable 10102030 nocheck
data sRupySwordSecondaryVtable 10102040 nocheck
data sRupySwordSafeStringVtable 10101D3C nocheck
data sRupySwordStateIdVtable 10101DD8 nocheck
data sRupySwordNoStateId 101FF32C nocheck
data sRupySwordUiSystem 1018C404 nocheck
data sRupySwordCollectIconClassName 1048F4D8 nocheck
data sRupySwordActive 1047B09A nocheck
data sRupySwordKind 1047B07E nocheck
data sRupySwordPictures 10101E40
data sRupySwordIconPaneName 1049A2C4 nocheck
data sRupySwordNumPaneName 1049A2CC nocheck
data sRupySwordGroupNames 1049A2F0 nocheck
data sRupySwordAnimNames 1049A300 nocheck
data sRupySwordWait 1049A318 nocheck
data sRupySwordIn 1049A348 nocheck
data sRupySwordDisp 1049A378 nocheck
data sRupySwordOut 1049A3A8 nocheck
data sRupySwordLayoutName 1049A3D8 nocheck
data sRupySwordAnimGroups 10101D84
data sRupySwordStateNumberReady 101FD8E4 nocheck
data sRupySwordStateNumber 101FDD50 nocheck
str 10101E8C "L_BCollectIcon_00" in 026CBA68
str 10101ECC "T_Num_00" in 026CBA68
str 10101EB0 "G_RupySword_00" in 026CBA68
str 10101EC0 "G_All_00" in 026CBA68
str 10101E5C "InAll" in 026CBA68
str 10101E80 "RupySword" in 026CBA68
str 10101E64 "OutAll" in 026CBA68
str 10101ED8 "::StateID_Wait" in 026CBA68
str 10101EE8 "::StateID_In" in 026CBA68
str 10101EF8 "::StateID_Disp" in 026CBA68
str 10101EA0 "::StateID_Out" in 026CBA68
str 10101E6C "RupySwordCounter_00" in 026CBA68
ext 026CB2F4 void hdScreenRupySwordCounter_c::updateDisplay()
ext 0264B66C hdParts_c* hdBtnCollectIcon_create_l(u32, u32, void*)
ext 0262A7BC void hdBtnCollectIcon_update_l(hdParts_c*)
*/

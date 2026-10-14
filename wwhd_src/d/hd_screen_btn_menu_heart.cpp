/**
 * hd_screen_btn_menu_heart.cpp
 * HD - the pause menu's heart-piece button "BtnMenuHeart_00": the "HeartPiece" animation's frame shows the
 * heart pieces collected towards the next heart; state ::StateID_HeartPiece
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_menu_btn_menu). Each function is marked with its WWHD address; the glue block at the end lists
 * this translation unit's data (tools/verify/autoglue.py). Layout, group, animation and state names are HD
 * strings; all other names are ours.
 */

#include "d/hd_screen_btn_menu.h"

static hdPartsVtbl_c sHeartVtable;
static u32 sHeartHolderVtable;
static u32 sHeartSecondaryVtable;
static hdUiStringTable sHeartSafeStringVtable;
static hdPartsStateIdVtbl_c sHeartStateIdVtable;
static hdStateId sHeartNoStateId;
static hdScreenUiSystem* sHeartUiSystem;

static hdUiSafeString sHeartAnimName;
static hdUiSafeString sHeartGroupNames[2] = {};
static hdPartsStateId_c sHeartHeartPiece;
static hdUiSafeString sHeartLayoutName;
static const u32 sHeartAnimGroups[1] = {1};
static u32 sHeartStateNumberReady;
static s32 sHeartStateNumber;

/* 0263DD18 */
// hdScreenBtnMenuHeart_c::~hdScreenBtnMenuHeart_c() (GHS deleting destructor)
void hdScreenBtnMenuHeart_c::destroy(hdScreenBtnMenuHeart_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sHeartHolderVtable;
    i_this->mSecondaryVtable = &sHeartSecondaryVtable;
    i_this->noDecideButton()->destroy(0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 0263DD84 */
void hdScreenBtnMenuHeart_c::heartPieceEnter() {
    hdAnimSet_play(animSet(), BTN_MENU_HEART_ANIM_HEART_PIECE, 1, 1.0f);
}

/* 0263DDA0 */
void hdScreenBtnMenuHeart_c::heartPieceExecute() {
    vCalcLayout();
}

/* 0263DDB0 */
// hdScreenBtnMenuHeart_c::hdScreenBtnMenuHeart_c() (GHS: allocates when called with NULL)
hdScreenBtnMenuHeart_c* hdScreenBtnMenuHeart_c::construct(hdScreenBtnMenuHeart_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenBtnMenuHeart_c*)operator_new(sizeof(hdScreenBtnMenuHeart_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    NoDecideButton_l::construct(i_this->noDecideButton());
    i_this->mStateHolder.mHolderVtable = &sHeartHolderVtable;
    i_this->mVtable = &sHeartVtable;
    i_this->mSecondaryVtable = &sHeartSecondaryVtable;
    i_this->mHeartPieces = 4;
    return i_this;
}

/* 0263DE24 */
u32 hdScreenBtnMenuHeart_c::init(u32 arg1, u32 arg2, void* desc) {
    // the merged base names its arguments (archive, resource, mode, flags); this layout passes the parts
    // descriptor in the last one
    return noDecideButton()->create(&sHeartLayoutName, gabi::at<u8>(arg1), arg2, gabi::ea(desc));
}

/* 0263DE3C */
// the heart pieces towards the next heart: the maximum life's quarters
void hdScreenBtnMenuHeart_c::updateHeartPieces() {
    u32 pieces = dComIfGs_info()->mSavedata.mPlayer.mPlayerStatusA.mMaxLife & 3;
    if (mHeartPieces == pieces) {
        return;
    }
    mHeartPieces = pieces;
    hdAnimSet_setFrame(animSet(), BTN_MENU_HEART_ANIM_HEART_PIECE, 1, (f32)pieces);
    setPaneEnabled(TRUE);
}

/* 0263DEC4 */
BOOL hdScreenBtnMenuHeart_c::loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sHeartUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, arg1, arg2, arg3, 6, 2, desc)) {
        return FALSE;
    }
    hdAnimSet_bindGroup(animSet(), BTN_MENU_HEART_ANIM_HEART_PIECE, &sHeartAnimName, &sHeartGroupNames[sHeartAnimGroups[0]]);
    return TRUE;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 0263DFB0 */
static void __sinit_hd_screen_btn_menu_heart_cpp() {
    sinit_header_statics_rodata(0x10490778, 0x101F5640, 0x10490784, 0x100EAB48);
    hdParts_setSafeString(&sHeartAnimName, &sHeartSafeStringVtable, "HeartPiece");
    hdParts_setSafeString(&sHeartGroupNames[0], &sHeartSafeStringVtable, "G_Btn_00");
    hdParts_setSafeString(&sHeartGroupNames[1], &sHeartSafeStringVtable, "G_HeartPiece_00");
    s32 number;
    if (sHeartStateNumberReady) {
        number = sHeartStateNumber;
    } else {
        number = 0;
        sHeartStateNumberReady = 1;
    }
    sHeartHeartPiece.setup(number + 1, "::StateID_HeartPiece", &sHeartStateIdVtable, 0x40, &sHeartNoStateId);
    sHeartStateNumber = number + 1;
    hdParts_setSafeString(&sHeartLayoutName, &sHeartSafeStringVtable, "BtnMenuHeart_00");
}

/* 0263DC18 */
static s32 hdScreenBtnMenuHeart_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue
data sHeartVtable 100EABA0 nocheck
data sHeartHolderVtable 100EADB8 nocheck
data sHeartSecondaryVtable 100EADC8 nocheck
data sHeartSafeStringVtable 100EAA90 nocheck
data sHeartStateIdVtable 100EAAF0 nocheck
data sHeartNoStateId 101FF32C nocheck
data sHeartUiSystem 1018C404 nocheck
data sHeartAnimName 10490770 nocheck
data sHeartGroupNames 10490794 nocheck
data sHeartHeartPiece 104907A4 nocheck
data sHeartLayoutName 104907D4 nocheck
data sHeartAnimGroups 100EAA8C
data sHeartStateNumberReady 101FD8E4 nocheck
data sHeartStateNumber 101FDD50 nocheck
str 100EAB7C "HeartPiece" in 0263DFB0
str 100EAB70 "G_Btn_00" in 0263DFB0
str 100EAB50 "G_HeartPiece_00" in 0263DFB0
str 100EAB88 "::StateID_HeartPiece" in 0263DFB0
str 100EAB60 "BtnMenuHeart_00" in 0263DFB0
ext 026F5CCC void hdButtonPart_c::setPaneEnabled(u32)
ext 02700C9C void NoDecideButton_l::destroy(u32)
ext 02701718 NoDecideButton_l* NoDecideButton_l::construct(NoDecideButton_l*)
ext 0270178C u32 NoDecideButton_l::create(void*,void*,u32,u32)
*/

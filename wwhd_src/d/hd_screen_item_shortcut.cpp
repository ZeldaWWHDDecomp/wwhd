/**
 * hd_screen_item_shortcut.cpp
 * HD - the GamePad's item shortcut "Shortcut_00": Wind Waker, or on the boat the salvage arm or the cannon
 * (with its bomb count), and the unread-Miiverse marker
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_hud_shortcut). Each function is marked with its WWHD address; the glue block at the end lists
 * this translation unit's data (tools/verify/autoglue.py). Layout, pane, group, animation and state names
 * are HD strings; all other names are ours.
 */

#include "d/hd_screen_item_shortcut.h"
static u32 sItemShortcutHeaderStatics;
static u32 sItemShortcutHeaderDescriptors;
static u32 sItemShortcutHeaderZero;
static u32 sItemShortcutPiConstants;

static hdPartsVtbl_c sItemShortcutVtable;
static u32 sItemShortcutHolderVtable;
static u32 sItemShortcutSecondaryVtable;
static hdUiStringTable sItemShortcutSafeStringVtable;
static hdPartsStateIdVtbl_c sItemShortcutStateIdVtable;
static hdStateId sItemShortcutNoStateId;
static hdScreenUiSystem* sItemShortcutUiSystem;
static hdUiStringTable sItemShortcutWideStringVtable;
static u8 sItemShortcutShown; /* the shortcut is shown at all (probably an option) */
static u8 sItemShortcutMiiverse; /* bit 0x40: no unread Miiverse posts, probably */
static hdUiSafeString sItemShortcutUnreadPartsName; /* the unread-Miiverse parts class name (its own TU) */

static hdUiSafeString sItemShortcutUnreadPaneName;
static hdUiSafeString sItemShortcutUnreadLayoutName;
static hdUiSafeString sItemShortcutNumPaneName;
static hdUiSafeString sItemShortcutGroupNames[6] = {};
static hdUiSafeString sItemShortcutAnimNames[9] = {};
static hdPartsStateId_c sItemShortcutWait;
static hdPartsStateId_c sItemShortcutIn;
static hdPartsStateId_c sItemShortcutDisp;
static hdPartsStateId_c sItemShortcutOut;
static hdPartsStateId_c sItemShortcutChange;
static hdUiSafeString sItemShortcutLayoutName;
static u16 sItemShortcutNumFormat[4] = {0x25, 0x32, 0x64, 0}; /* L"%2d" */
static u32 sItemShortcutAnimGroups[9] = {0, 2, 3, 2, 3, 5, 4, 1, 0};
static u32 sItemShortcutStateNumberReady;
static s32 sItemShortcutStateNumber;

/* 026DD464 */
// hdScreenItemShortcut_c::~hdScreenItemShortcut_c() (GHS deleting destructor)
void hdScreenItemShortcut_c::destroy(hdScreenItemShortcut_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sItemShortcutHolderVtable;
    i_this->mSecondaryVtable = &sItemShortcutSecondaryVtable;
    i_this->NoControlPart_l::destroy(0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 026DD4D0 */
void hdScreenItemShortcut_c::setAnimFrame(u32 anim, u32 frame) {
    hdAnimSet_setFrame(animSet(), anim, sItemShortcutAnimGroups[anim], (f32)frame);
}

/* 026DD528 */
void hdScreenItemShortcut_c::waitEnter() {
    setAnimFrame(ITEM_SHORTCUT_ANIM_IN_ALL, 0);
    setAnimFrame(ITEM_SHORTCUT_ANIM_MIIVERSE_POS, 1);
}

/* 026DD56C */
// on the boat the shortcut works the salvage arm (grappling hook) or the cannon (bombs); else the baton
void hdScreenItemShortcut_c::updateKind() {
    BOOL onShip = FALSE;
    if (dComIfGp_checkPlayerStatus0(0, 0x00010000 /* daPyStts0_SHIP_RIDE_e */)) {
        onShip = TRUE;
    }
    mKind = ITEM_SHORTCUT_NONE;
    if (onShip) {
        if (dComIfGs_getItem(ITEM_SHORTCUT_SLOT_BOMBS) == item_data::dItemNo_BOMB_BAG_e) {
            mKind = ITEM_SHORTCUT_CANNON;
            return;
        }
        if (dComIfGs_getItem(ITEM_SHORTCUT_SLOT_GRAPPLING_HOOK) == item_data::dItemNo_GRAPPLING_HOOK_e) {
            mKind = ITEM_SHORTCUT_SALVAGE;
            return;
        }
    }
    if (dComIfGs_getItem(ITEM_SHORTCUT_SLOT_WIND_WAKER) == item_data::dItemNo_WIND_WAKER_e) {
        mKind = ITEM_SHORTCUT_WIND_WAKER;
    }
}

/* 026DD618 */
void hdScreenItemShortcut_c::waitExecute() {
    updateKind();
    if (sItemShortcutShown && mKind != ITEM_SHORTCUT_NONE) {
        changeState(&sItemShortcutIn);
    }
}

/* 026DD670 */
void hdScreenItemShortcut_c::startAnim(u32 anim) {
    hdAnimSet_play(animSet(), anim, sItemShortcutAnimGroups[anim], 1.0f);
}

/* 026DD694 */
void hdScreenItemShortcut_c::updateMiiverse() {
    u32 frame = (sItemShortcutMiiverse & 0x40) ? 0 : 1;
    if (mMiiverseFrame == frame) {
        return;
    }
    mMiiverseFrame = frame;
    setAnimFrame(ITEM_SHORTCUT_ANIM_STATE, frame);
}

/* 026DD6C4 */
void hdScreenItemShortcut_c::inEnter() {
    startAnim(ITEM_SHORTCUT_ANIM_IN_ALL);
    setAnimFrame(ITEM_SHORTCUT_ANIM_MIIVERSE_POS, mKind == ITEM_SHORTCUT_NONE ? 1 : 0);
    setAnimFrame(mKind < ITEM_SHORTCUT_SALVAGE ? ITEM_SHORTCUT_ANIM_IN_SALVAGE : ITEM_SHORTCUT_ANIM_OUT_SALVAGE, 0);
    setAnimFrame(mKind < ITEM_SHORTCUT_CANNON ? ITEM_SHORTCUT_ANIM_IN_CANNON : ITEM_SHORTCUT_ANIM_OUT_CANNON, 0);
    mMiiverseFrame = 2;
    updateMiiverse();
}

/* 026DD764 */
void hdScreenItemShortcut_c::inExecute() {
    if (hdAnimSet_isEnd(animSet(), ITEM_SHORTCUT_ANIM_IN_ALL)) {
        changeState(&sItemShortcutDisp);
    }
}

/* 026DD7B4 */
void hdScreenItemShortcut_c::dispExecute() {
    mPrevKind = mKind;
    updateKind();
    updateMiiverse();
    if (!sItemShortcutShown || mKind == ITEM_SHORTCUT_NONE) {
        changeState(&sItemShortcutOut);
    } else if (mKind != mPrevKind) {
        changeState(&sItemShortcutChange);
    }
}

/* 026DD83C */
void hdScreenItemShortcut_c::outEnter() {
    startAnim(ITEM_SHORTCUT_ANIM_OUT_ALL);
}

/* 026DD844 */
void hdScreenItemShortcut_c::outExecute() {
    if (hdAnimSet_isEnd(animSet(), ITEM_SHORTCUT_ANIM_OUT_ALL)) {
        changeState(&sItemShortcutWait);
    }
}

/* 026DD894 */
// the salvage / cannon groups slide in or out when the kind changes between them
void hdScreenItemShortcut_c::changeEnter() {
    u32 prev = mPrevKind;
    mChangeAnimIn = ITEM_SHORTCUT_ANIM_NONE;
    u32 kind = mKind;
    mChangeAnimOut = ITEM_SHORTCUT_ANIM_NONE;
    if (kind > prev) {
        if (kind >= ITEM_SHORTCUT_SALVAGE) {
            if (prev < ITEM_SHORTCUT_SALVAGE) {
                mChangeAnimIn = ITEM_SHORTCUT_ANIM_IN_SALVAGE;
            }
            if (kind >= ITEM_SHORTCUT_CANNON && mPrevKind < ITEM_SHORTCUT_CANNON) {
                mChangeAnimOut = ITEM_SHORTCUT_ANIM_IN_CANNON;
            }
        }
    } else if (kind >= ITEM_SHORTCUT_SALVAGE || prev >= ITEM_SHORTCUT_SALVAGE) {
        if (kind < ITEM_SHORTCUT_SALVAGE) {
            mChangeAnimIn = ITEM_SHORTCUT_ANIM_OUT_SALVAGE;
        }
        if (kind < ITEM_SHORTCUT_CANNON && mPrevKind >= ITEM_SHORTCUT_CANNON) {
            mChangeAnimOut = ITEM_SHORTCUT_ANIM_OUT_CANNON;
        }
    }
    if (mChangeAnimIn != ITEM_SHORTCUT_ANIM_NONE) {
        startAnim(mChangeAnimIn);
    }
    if (mChangeAnimOut != ITEM_SHORTCUT_ANIM_NONE) {
        startAnim(mChangeAnimOut);
    }
}

/* 026DD974 */
void hdScreenItemShortcut_c::changeExecute() {
    BOOL done = TRUE;
    if (mChangeAnimIn != ITEM_SHORTCUT_ANIM_NONE && !hdAnimSet_isEnd(animSet(), mChangeAnimIn)) {
        done = FALSE;
    }
    if (mChangeAnimOut != ITEM_SHORTCUT_ANIM_NONE && !hdAnimSet_isEnd(animSet(), mChangeAnimOut)) {
        return;
    }
    if (done) {
        changeState(&sItemShortcutDisp);
    }
}

/* 026DDA04 */
// hdScreenItemShortcut_c::hdScreenItemShortcut_c() (GHS: allocates when called with NULL)
hdScreenItemShortcut_c* hdScreenItemShortcut_c::construct(hdScreenItemShortcut_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenItemShortcut_c*)operator_new(sizeof(hdScreenItemShortcut_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    NoControlPart_l::construct(i_this);
    i_this->mPrevKind = ITEM_SHORTCUT_UNSET;
    i_this->mKind = ITEM_SHORTCUT_UNSET;
    i_this->mSecondaryVtable = &sItemShortcutSecondaryVtable;
    i_this->mVtable = &sItemShortcutVtable;
    i_this->mMiiverseFrame = 2;
    i_this->mpNumPane = NULL;
    i_this->mChangeAnimIn = 0;
    i_this->mStateHolder.mHolderVtable = &sItemShortcutHolderVtable;
    i_this->mBombNum = 0xFF;
    i_this->mChangeAnimOut = 0;
    return i_this;
}

/* 026DDA9C */
u32 hdScreenItemShortcut_c::init(u32 arg1, u32 arg2, void* desc) {
    if (!create(&sItemShortcutLayoutName, gabi::at<u8>(arg1), arg2, gabi::ea(desc))) {
        return FALSE;
    }
    hdItemShortcutPane_c* root = (hdItemShortcutPane_c*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    hdUiStringTable* nameVtable = sItemShortcutNumPaneName.mVtable;
    hdItemShortcutPaneVtbl_c* rootVtable = root->mVtable;
    gabi::call_ptr(nameVtable->mAssureTermination.get(), &sItemShortcutNumPaneName);
    mpNumPane = gabi::call_ptr<void*>(rootVtable->mFindPaneByName.get(), root, (be<u8>*)sItemShortcutNumPaneName.mText, 1u);
    startState(&sItemShortcutWait);
    return TRUE;
}

/* 026DDB9C */
// the cannon's bomb count: the number and whether the bag is full
void hdScreenItemShortcut_c::updateBombNum() {
    u8 num = dComIfGs_getBombNum();
    if (mBombNum == num) {
        return;
    }
    mBombNum = num;
    u32 full = num == dComIfGs_getBombMax() ? 1 : 0;
    hdAnimSet_setFrame(animSet(), ITEM_SHORTCUT_ANIM_NUM, 1, (f32)full);
    u32 value = mBombNum;
    gabi::Local<hdItemShortcutNumText_c> text;
    text->mBuffer[2] = 0;
    text->mCapacity = 3;
    text->mText = text->mBuffer;
    text->mBuffer[0] = 0;
    text->mVtable = &sItemShortcutWideStringVtable;
    hdItemShortcut_format(text, sItemShortcutNumFormat, value);
    hdItemShortcut_setText(mpNumPane, text, 0);
}

/* 026DDC84 */
void hdScreenItemShortcut_c::executeState() {
    updateBombNum();
    hdParts_executeStateMachine(stateMachine());
    vExecuteChildren();
}

/* 026DDCCC */
void hdScreenItemShortcut_c::reset() {
    changeState(&sItemShortcutWait);
    hdPartsUnreadMiiverse_reset(*mChildren.mpData.get());
}

/* inlined into visit: SafeString equality (the descriptor's string is terminated twice) */
static bool hdItemShortcut_isSameName(hdUiSafeString* name, hdUiSafeString* other) {
    name->assureTermination();
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

/* 026DDD10 */
// the unread-Miiverse marker pane gets its own parts class; other descriptors go to the factory
u32 hdScreenItemShortcut_c::visit(hdItemShortcutDesc_l* desc) {
    hdScreenHeap* heap = sItemShortcutUiSystem->mHeap;
    if (!hdItemShortcut_isSameName(&desc->mName, &sItemShortcutUnreadPaneName)) {
        return TRUE;
    }
    if (!hdItemShortcut_isSameName(&desc->mType, &sItemShortcutUnreadPartsName)) {
        return createChild(0, (hdPartsDesc_c*)desc);
    }
    gptr<hdParts_c>* data = mChildren.mpData;
    hdParts_c* part = (hdParts_c*)hdParts_new(0x58, heap, 4);
    if (part != NULL) {
        part = hdPartsUnreadMiiverse_construct(part);
    }
    *data = part;
    hdParts_c* child = *mChildren.mpData.get();
    if (child == NULL) {
        return FALSE;
    }
    return hdPartsUnreadMiiverse_init(child, 0, 0, (hdPartsDesc_c*)desc);
}

/* 026DDF18 */
u32 hdScreenItemShortcut_c::load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sItemShortcutUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, arg1, arg2, arg3, 9, 6, desc)) {
        return FALSE;
    }
    for (u32 i = 0; i < 9; i++) {
        hdItemShortcut_bindGroup(animSet(), i, &sItemShortcutAnimNames[i], &sItemShortcutGroupNames[sItemShortcutAnimGroups[i]]);
    }
    return TRUE;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 026DE030 */
static void __sinit_hd_screen_item_shortcut_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sItemShortcutHeaderStatics), gabi::ea(&sItemShortcutHeaderDescriptors), gabi::ea(&sItemShortcutHeaderZero),
                                gabi::ea(&sItemShortcutPiConstants));
    hdParts_setSafeString(&sItemShortcutGroupNames[0], &sItemShortcutSafeStringVtable, "G_All_00");
    hdParts_setSafeString(&sItemShortcutGroupNames[1], &sItemShortcutSafeStringVtable, "G_Num_00");
    hdParts_setSafeString(&sItemShortcutGroupNames[2], &sItemShortcutSafeStringVtable, "G_Salvage_00");
    hdParts_setSafeString(&sItemShortcutGroupNames[3], &sItemShortcutSafeStringVtable, "G_Cannon_00");
    hdParts_setSafeString(&sItemShortcutGroupNames[4], &sItemShortcutSafeStringVtable, "G_MiiversePos_00");
    hdParts_setSafeString(&sItemShortcutGroupNames[5], &sItemShortcutSafeStringVtable, "G_Shortcut_00");
    hdParts_setSafeString(&sItemShortcutAnimNames[0], &sItemShortcutSafeStringVtable, "InAll");
    hdParts_setSafeString(&sItemShortcutAnimNames[1], &sItemShortcutSafeStringVtable, "InSalvageCannon");
    hdParts_setSafeString(&sItemShortcutAnimNames[2], &sItemShortcutSafeStringVtable, "InSalvageCannon");
    hdParts_setSafeString(&sItemShortcutAnimNames[3], &sItemShortcutSafeStringVtable, "OutSalvageCannon");
    hdParts_setSafeString(&sItemShortcutAnimNames[4], &sItemShortcutSafeStringVtable, "OutSalvageCannon");
    hdParts_setSafeString(&sItemShortcutAnimNames[5], &sItemShortcutSafeStringVtable, "State");
    hdParts_setSafeString(&sItemShortcutAnimNames[6], &sItemShortcutSafeStringVtable, "MiiversePos");
    hdParts_setSafeString(&sItemShortcutAnimNames[7], &sItemShortcutSafeStringVtable, "Num");
    hdParts_setSafeString(&sItemShortcutAnimNames[8], &sItemShortcutSafeStringVtable, "OutAll");
    hdParts_setSafeString(&sItemShortcutUnreadPaneName, &sItemShortcutSafeStringVtable, "L_UnreadMiiverse_00");
    hdParts_setSafeString(&sItemShortcutUnreadLayoutName, &sItemShortcutSafeStringVtable, "UnreadMiiverse_00");
    hdParts_setSafeString(&sItemShortcutNumPaneName, &sItemShortcutSafeStringVtable, "T_Num_00");
    hdParts_setSafeString(&sItemShortcutLayoutName, &sItemShortcutSafeStringVtable, "Shortcut_00");
    s32 number;
    if (sItemShortcutStateNumberReady) {
        number = sItemShortcutStateNumber;
    } else {
        number = 0;
        sItemShortcutStateNumberReady = 1;
    }
    sItemShortcutWait.setup(number + 1, "::StateID_Wait", &sItemShortcutStateIdVtable, 0x19, &sItemShortcutNoStateId);
    sItemShortcutIn.setup(number + 2, "::StateID_In", &sItemShortcutStateIdVtable, 0x1C, &sItemShortcutNoStateId);
    sItemShortcutDisp.setup(number + 3, "::StateID_Disp", &sItemShortcutStateIdVtable, 0x1F, &sItemShortcutNoStateId);
    sItemShortcutOut.setup(number + 4, "::StateID_Out", &sItemShortcutStateIdVtable, 0x22, &sItemShortcutNoStateId);
    sItemShortcutChange.setup(number + 5, "::StateID_Change", &sItemShortcutStateIdVtable, 0x25, &sItemShortcutNoStateId);
    sItemShortcutStateNumber = number + 5;
}

/* WWHD glue
data sItemShortcutHeaderStatics 1049B740 nocheck
data sItemShortcutHeaderDescriptors 101F6B2C nocheck
data sItemShortcutHeaderZero 1049B74C nocheck
data sItemShortcutPiConstants 10104A48 nocheck
ext 026EE4B4 hdParts_c* hdPartsUnreadMiiverse_construct(hdParts_c* parts)
ext 026EE5B4 u32 hdPartsUnreadMiiverse_init(hdParts_c* parts, u32 arg1, u32 arg2, hdPartsDesc_c* desc)
ext 026EE658 void hdPartsUnreadMiiverse_reset(hdParts_c* parts)
ext 02004E04 void hdItemShortcut_bindGroup(hdAnimSet* set, u32 entry, hdUiSafeString* name, hdUiSafeString* group)
ext 027046D4 void hdItemShortcut_setText(void* textBox, void* string, u32 option)
ext 02759D3C void hdItemShortcut_format(void* string, const void* format, u32 value)
data sItemShortcutVtable 10104B88 nocheck
data sItemShortcutHolderVtable 10104CC8 nocheck
data sItemShortcutSecondaryVtable 10104CD8 nocheck
data sItemShortcutSafeStringVtable 10104934 nocheck
data sItemShortcutStateIdVtable 101049E8 nocheck
data sItemShortcutNoStateId 101FF32C nocheck
data sItemShortcutUiSystem 1018C404 nocheck
data sItemShortcutWideStringVtable 10104964 nocheck
data sItemShortcutShown 1047B095 nocheck
data sItemShortcutMiiverse 1047B07D nocheck
data sItemShortcutUnreadPartsName 1049D51C nocheck
data sItemShortcutUnreadPaneName 1049B728 nocheck
data sItemShortcutUnreadLayoutName 1049B730 nocheck
data sItemShortcutNumPaneName 1049B738 nocheck
data sItemShortcutGroupNames 1049B75C nocheck
data sItemShortcutAnimNames 1049B78C nocheck
data sItemShortcutWait 1049B7D4 nocheck
data sItemShortcutIn 1049B804 nocheck
data sItemShortcutDisp 1049B834 nocheck
data sItemShortcutOut 1049B864 nocheck
data sItemShortcutChange 1049B894 nocheck
data sItemShortcutLayoutName 1049B8C4 nocheck
data sItemShortcutAnimGroups 1010497C
data sItemShortcutNumFormat 10104A40
data sItemShortcutStateNumberReady 101FD8E4 nocheck
data sItemShortcutStateNumber 101FDD50 nocheck
str 10104AE8 "G_All_00" in 026DE030
str 10104AF4 "G_Num_00" in 026DE030
str 10104B00 "G_Salvage_00" in 026DE030
str 10104A7C "G_Cannon_00" in 026DE030
str 10104B10 "G_MiiversePos_00" in 026DE030
str 10104AB4 "G_Shortcut_00" in 026DE030
str 10104A54 "InAll" in 026DE030
str 10104A6C "InSalvageCannon" in 026DE030
str 10104B24 "OutSalvageCannon" in 026DE030
str 10104A5C "State" in 026DE030
str 10104A88 "MiiversePos" in 026DE030
str 10104A50 "Num" in 026DE030
str 10104A64 "OutAll" in 026DE030
str 10104A94 "L_UnreadMiiverse_00" in 026DE030
str 10104AC4 "UnreadMiiverse_00" in 026DE030
str 10104B38 "T_Num_00" in 026DE030
str 10104AA8 "Shortcut_00" in 026DE030
str 10104B44 "::StateID_Wait" in 026DE030
str 10104B54 "::StateID_In" in 026DE030
str 10104B64 "::StateID_Disp" in 026DE030
str 10104AD8 "::StateID_Out" in 026DE030
str 10104B74 "::StateID_Change" in 026DE030
ext 026F92C0 u32 hdParts_c::createChild(u32 index, hdPartsDesc_c* desc)
ext 02700A70 void NoControlPart_l::destroy(u32)
ext 02700ADC NoControlPart_l* NoControlPart_l::construct(NoControlPart_l*)
ext 02700B48 u32 NoControlPart_l::create(void*,void*,u32,u32)
*/

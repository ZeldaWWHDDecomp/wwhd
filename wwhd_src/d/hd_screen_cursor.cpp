/**
 * hd_screen_cursor.cpp
 * HD - the menu cursor "Cursor_00": a window frame ("W_Cursor_00") that follows and wraps the UI
 * element the event manager has focused; states ::StateID_Blank / _InAll / _LoopAll / _OutAll
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_menu_cursor). Each function is marked with its WWHD address; the glue block at the end lists
 * this translation unit's data (tools/verify/autoglue.py). Layout, pane, animation and state names are
 * HD strings; all other names are ours.
 */

#include "d/hd_screen_cursor.h"
static u32 sScreenCursorRegistration;
static u32 sCursorHeaderStatics;
static u32 sCursorHeaderDescriptors;
static u32 sCursorHeaderZero;
static u32 sCursorPiConstants;

static hdPartsVtbl_c sCursorVtable;
static u32 sCursorHolderVtable;
static u32 sCursorSecondaryVtable;
static hdUiStringTable sCursorSafeStringVtable;
static hdPartsStateIdVtbl_c sCursorStateIdVtable;
static hdStateId sCursorNoStateId;
static hdPartsStateId_c sCursorPartsBlank; /* hdParts_c's ::StateID_Blank (hd_parts.cpp) */
static hdScreenUiSystem* sCursorUiSystem;
static hdCursorController_c* sCursorController;
static void* sCursorEventMgr;
static u32 sCursorRootTypeReady;
static hdPartsTypeInfo_c sCursorRootType;

static u32 sCursorWindowNameReady;
static hdUiSafeString sCursorWindowName;
static hdUiSafeString sCursorArchiveName;
static hdCursorSize_c sCursorFrameMargin;
static hdUiSafeString sCursorGroupNames[2] = {};
static hdUiSafeString sCursorAnimNames[3] = {};
static hdPartsStateId_c sCursorBlank;
static hdPartsStateId_c sCursorInAll;
static hdPartsStateId_c sCursorLoopAll;
static hdPartsStateId_c sCursorOutAll;
static hdUiSafeString sCursorLayoutName;
static hdCursorSize_c sCursorFrameScale;
static u32 sCursorAnimGroups[3] = {1, 0, 1};
static u32 sCursorStateNumberReady;
static s32 sCursorStateNumber;

/* 0265071C */
// exit-time destruction of the window pane's name (findWindowPane's function-local static)
static void hdScreenCursor_destroyWindowName() {
    hdCursor_destroyArray(&sCursorWindowName, 1, sizeof(hdUiSafeString), 0x0265142C);
}

/* 02650740 */
// hdScreenCursor_c::~hdScreenCursor_c() (GHS deleting destructor)
void hdScreenCursor_c::destroy(hdScreenCursor_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sCursorHolderVtable;
    i_this->mSecondaryVtable = &sCursorSecondaryVtable;
    hdParts_c::destroy(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 026507AC */
// hidden: shows itself once the cursor drives the menus and an element of this event manager has focus
void hdScreenCursor_c::blankExecute() {
    if (sCursorController->mCursorActive) {
        hdCursorTarget_c* focus = hdCursor_getFocus(sCursorEventMgr);
        if (focus != NULL && (void*)focus->mpEventMgr == (void*)sCursorEventMgr) {
            changeState(&sCursorInAll);
        }
    }
}

/* 02650824 */
void hdScreenCursor_c::inAllEnter() {
    hdAnimSet_play(animSet(), CURSOR_ANIM_IN_ALL, 1, 1.0f);
}

/* 02650840 */
void hdScreenCursor_c::inAllExecute() {
    if (hdAnimSet_isEnd(animSet(), CURSOR_ANIM_IN_ALL)) {
        changeState(&sCursorLoopAll);
    }
    vCalcLayout();
}

/* 026508A4 */
void hdScreenCursor_c::loopAllEnter() {
    hdAnimSet_play(animSet(), CURSOR_ANIM_LOOP_ALL, 0, 1.0f);
}

/* 026508C0 */
void hdScreenCursor_c::loopAllExecute() {
    bool shown = false;
    if (sCursorController->mCursorActive) {
        hdCursorTarget_c* focus = hdCursor_getFocus(sCursorEventMgr);
        if (focus != NULL && (void*)focus->mpEventMgr == (void*)sCursorEventMgr) {
            shown = true;
        }
    }
    if (!shown) {
        changeState(&sCursorOutAll);
    }
    vCalcLayout();
}

/* 02650954 */
void hdScreenCursor_c::outAllEnter() {
    hdAnimSet_play(animSet(), CURSOR_ANIM_OUT_ALL, 1, 1.0f);
}

/* 02650970 */
void hdScreenCursor_c::outAllExecute() {
    if (hdAnimSet_isEnd(animSet(), CURSOR_ANIM_OUT_ALL)) {
        changeState(&sCursorBlank);
    }
    vCalcLayout();
}

/* 026509D4 */
// hdScreenCursor_c::hdScreenCursor_c() (GHS: allocates when called with NULL)
hdScreenCursor_c* hdScreenCursor_c::construct(hdScreenCursor_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenCursor_c*)operator_new(sizeof(hdScreenCursor_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdParts_c::construct(i_this);
    i_this->mVtable = &sCursorVtable;
    i_this->mSecondaryVtable = &sCursorSecondaryVtable;
    i_this->mStateHolder.mHolderVtable = &sCursorHolderVtable;
    // HD: GHS tests the address of the (trivially constructed) pane pointer member
    if (gabi::ea(&i_this->mpWindowPane) == 0) {
        operator_new(sizeof(i_this->mpWindowPane));
    }
    return i_this;
}

/* inlined into findWindowPane: the runtime type every pane derives from */
static hdPartsTypeInfo_c* hdScreenCursor_getRootTypeInfo() {
    if (!sCursorRootTypeReady) {
        sCursorRootTypeReady = 1;
        sCursorRootType.mParent = NULL;
    }
    return &sCursorRootType;
}

/* inlined: the type test of a dynamic cast */
static bool hdScreenCursor_isDerivedFrom(hdPartsTypeInfo_c* type, hdPartsTypeInfo_c* base) {
    for (; type != NULL; type = type->mParent) {
        if (type == base) {
            return true;
        }
    }
    return false;
}

/* 02650A50 */
void hdScreenCursor_c::findWindowPane() {
    if (!sCursorWindowNameReady) {
        sCursorWindowNameReady = 1;
        hdParts_setSafeString(&sCursorWindowName, &sCursorSafeStringVtable, "W_Cursor_00");
        __register_global_object(gabi::ea(&sScreenCursorRegistration));
    }
    hdCursorPane_c* root = (hdCursorPane_c*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    hdCursorPaneVtbl_c* rootVtable = root->mVtable;
    sCursorWindowName.assureTermination();
    hdCursorPane_c* pane = gabi::call_ptr<hdCursorPane_c*>(rootVtable->mFindPaneByName.get(), root, (hdCursorChar_t*)sCursorWindowName.mText, 1u);
    hdPartsTypeInfo_c* rootType = hdScreenCursor_getRootTypeInfo();
    if (pane != NULL && hdScreenCursor_isDerivedFrom(pane->getRuntimeTypeInfo(), rootType)) {
        mpWindowPane = pane;
    } else {
        mpWindowPane = NULL;
    }
}

/* 02650B9C */
u32 hdScreenCursor_c::init(u32 arg1, u32 arg2, void* desc) {
    attachArchives(&sCursorArchiveName, 1);
    if (!vLoad(&sCursorLayoutName, gabi::ea(mpResAccessor.get()), arg1, arg2, desc)) {
        return FALSE;
    }
    findWindowPane();
    hdAnimSet_operationCC(animSet(), CURSOR_ANIM_IN_ALL, 1);
    vCalcLayout();
    startState(&sCursorBlank);
    return TRUE;
}

/* 02650CB4 */
void hdScreenCursor_c::finalize() {
    vDestroyLayout();
    detachArchives();
}

/* 02650CF4 */
void hdScreenCursor_c::executeState() {
    hdParts_executeStateMachine(stateMachine());
}

/* 02650CFC */
// only the cursor's own layout is drawn (no child parts)
void hdScreenCursor_c::draw(void* info) {
    vDrawLayout(info);
}

/* 02650D0C */
// the frame's size factors (a zero factor keeps the old one)
void hdScreenCursor_c::setFrameScale(hdCursorSize_c* scale) {
    f32 x = scale->x;
    if (x == 0.0f) {
        return;
    }
    f32 y = scale->y;
    if (y == 0.0f) {
        return;
    }
    sCursorFrameScale.y = y;
    sCursorFrameScale.x = x;
}

/* 02650D40 */
u32 hdScreenCursor_c::load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sCursorUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, arg1, arg2, arg3, 3, 2, desc)) {
        return FALSE;
    }
    for (u32 i = 0; i < 3; i++) {
        hdAnimSet_bindGroup(animSet(), i, &sCursorAnimNames[i], &sCursorGroupNames[sCursorAnimGroups[i]]);
    }
    vCalcLayout();
    return TRUE;
}

/* 02650E6C */
// the pane's corner the cursor frame anchors to, from its base position
void hdScreenCursor_c::getAnchorPos(cXyz* out, hdScreenCursor_c* i_this, hdCursorPane_c* pane) {
    gabi::Local<cXyz> pos;
    hdCursor_getPanePos(pos, pane);
    f32 halfWidth = pane->mWidth * 0.5f;
    u8 basePosition = pane->mBasePosition;
    f32 halfHeight = pane->mHeight * 0.5f;
    switch (basePosition & 3) {
    case 1:
        pos->x = pos->x + halfWidth;
        break;
    case 2:
        pos->x = pos->x - halfWidth;
        break;
    }
    switch ((basePosition >> 2) & 3) {
    case 1:
        pos->y = pos->y - halfHeight;
        break;
    case 2:
        pos->y = pos->y + halfHeight;
        break;
    }
    *out = *pos;
}

/* 02650F8C */
// move the layout to the target's pane (or its own cursor pane) and size the frame around it
void hdScreenCursor_c::moveTo(hdCursorTarget_c* target) {
    if (target == NULL) {
        return;
    }
    hdCursorPane_c* pane = target->mpCursorPane;
    bool scaled = true;
    hdCursorPane_c* elementPane = target->mpPane;
    if (pane != NULL) {
        scaled = false;
    } else {
        pane = elementPane;
        if (pane == NULL) {
            return;
        }
    }
    gabi::Local<cXyz> pos;
    getAnchorPos(pos, this, pane);
    hdCursorPane_c* root = (hdCursorPane_c*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    root->mTranslate = *pos;
    root->mFlags = root->mFlags | CURSOR_PANE_DIRTY;
    f32 width = pane->mWidth;
    f32 height = pane->mHeight;
    if (scaled) {
        width = width * sCursorFrameScale.x;
        height = height * sCursorFrameScale.y;
    }
    hdCursorPane_c* window = mpWindowPane;
    window->mWidth = width + sCursorFrameMargin.x;
    window->mHeight = height + sCursorFrameMargin.y;
    window->mFlags = window->mFlags | CURSOR_PANE_DIRTY;
}

/* 026510B8 */
void hdScreenCursor_c::calcLayout() {
    if (mpLayout == NULL) {
        return;
    }
    moveTo(hdCursor_getFocus(sCursorEventMgr));
    mpLayout->calc();
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 02651118 */
static void __sinit_hd_screen_cursor_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sCursorHeaderStatics), gabi::ea(&sCursorHeaderDescriptors), gabi::ea(&sCursorHeaderZero),
                                gabi::ea(&sCursorPiConstants));
    sCursorFrameMargin.x = 20.0f;
    sCursorFrameMargin.y = 20.0f;
    sCursorFrameScale.x = 1.0f;
    sCursorFrameScale.y = 1.0f;
    hdParts_setSafeString(&sCursorAnimNames[0], &sCursorSafeStringVtable, "InAll");
    hdParts_setSafeString(&sCursorAnimNames[1], &sCursorSafeStringVtable, "LoopAll");
    hdParts_setSafeString(&sCursorAnimNames[2], &sCursorSafeStringVtable, "OutAll");
    hdParts_setSafeString(&sCursorGroupNames[0], &sCursorSafeStringVtable, "G_LoopAll_00");
    hdParts_setSafeString(&sCursorGroupNames[1], &sCursorSafeStringVtable, "G_All_00");
    hdParts_setSafeString(&sCursorArchiveName, &sCursorSafeStringVtable, "Cursor_00");
    hdParts_setSafeString(&sCursorLayoutName, &sCursorSafeStringVtable, "Cursor_00");
    s32 number;
    if (sCursorStateNumberReady) {
        number = sCursorStateNumber;
    } else {
        number = 0;
        sCursorStateNumberReady = 1;
    }
    // the cursor's Blank refines hdParts_c's (its field +0x2C names the parent state)
    sCursorBlank.setup(number + 1, "::StateID_Blank", &sCursorStateIdVtable, 4, (hdStateId*)&sCursorPartsBlank);
    sCursorInAll.setup(number + 2, "::StateID_InAll", &sCursorStateIdVtable, 0x19, &sCursorNoStateId);
    sCursorLoopAll.setup(number + 3, "::StateID_LoopAll", &sCursorStateIdVtable, 0x1C, &sCursorNoStateId);
    sCursorOutAll.setup(number + 4, "::StateID_OutAll", &sCursorStateIdVtable, 0x1F, &sCursorNoStateId);
    sCursorStateNumber = number + 4;
}

/* 02651458 */
static s32 hdScreenCursor_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue
data sScreenCursorRegistration 101F5984 nocheck
data sCursorHeaderStatics 10491D88 nocheck
data sCursorHeaderDescriptors 101F5990 nocheck
data sCursorHeaderZero 10491D94 nocheck
data sCursorPiConstants 100EF660 nocheck
ext 02615140 hdCursorTarget_c* hdCursor_getFocus(void* eventMgr)
ext 02704D2C void hdCursor_getPanePos(cXyz* out, hdCursorPane_c* pane)
data sCursorVtable 100EF6F8 nocheck
data sCursorHolderVtable 100EF808 nocheck
data sCursorSecondaryVtable 100EF818 nocheck
data sCursorSafeStringVtable 100EF594 nocheck
data sCursorStateIdVtable 100EF600 nocheck
data sCursorNoStateId 101FF32C nocheck
data sCursorPartsBlank 1049DD60 nocheck
data sCursorUiSystem 1018C404 nocheck
data sCursorController 101F8378 nocheck
data sCursorEventMgr 101F4FF0 nocheck
data sCursorRootTypeReady 101FD888 nocheck
data sCursorRootType 101FD8FC nocheck
data sCursorWindowNameReady 10491DCC nocheck
data sCursorWindowName 10491D70 nocheck
data sCursorArchiveName 10491D78 nocheck
data sCursorFrameMargin 10491D80 nocheck
data sCursorGroupNames 10491DA4 nocheck
data sCursorAnimNames 10491DB4 nocheck
data sCursorBlank 10491DD0 nocheck
data sCursorInAll 10491E00 nocheck
data sCursorLoopAll 10491E30 nocheck
data sCursorOutAll 10491E60 nocheck
data sCursorLayoutName 10491E90 nocheck
data sCursorFrameScale 10491E98 nocheck
data sCursorAnimGroups 100EF5AC
data sCursorStateNumberReady 101FD8E4 nocheck
data sCursorStateNumber 101FDD50 nocheck
str 100EF64C "W_Cursor_00" in 02650A50
str 100EF674 "InAll" in 02651118
str 100EF66C "LoopAll" in 02651118
str 100EF67C "OutAll" in 02651118
str 100EF6C4 "G_LoopAll_00" in 02651118
str 100EF6D4 "G_All_00" in 02651118
str 100EF6A4 "Cursor_00" in 02651118
str 100EF684 "::StateID_Blank" in 02651118
str 100EF694 "::StateID_InAll" in 02651118
str 100EF6B0 "::StateID_LoopAll" in 02651118
str 100EF6E0 "::StateID_OutAll" in 02651118
ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c* i_this)
ext 026F88F0 void hdParts_c::destroy(hdParts_c* i_this, s32 flags)
ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString* names, u32 count)
ext 026F91F8 void hdParts_c::detachArchives()
*/

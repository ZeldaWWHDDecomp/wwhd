/**
 * hd_screen_fixed_cursor.cpp
 * HD - the "Cursor_02" screen: a cursor frame placed at a set position and size while an element has
 * focus; states ::StateID_Blank / _InAll / _LoopAll / _OutAll
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_menu_cursor). Each function is marked with its WWHD address; the glue block at the end lists
 * this translation unit's data (tools/verify/autoglue.py). Layout, pane, animation and state names are
 * HD strings; all other names are ours.
 */

#include "d/hd_screen_fixed_cursor.h"
static u32 sScreenFixedCursorRegistration;
static u32 sFixedCursorHeaderStatics;
static u32 sFixedCursorHeaderDescriptors;
static u32 sFixedCursorHeaderZero;
static u32 sFixedCursorPiConstants;

static hdPartsVtbl_c sFixedCursorVtable;
static u32 sFixedCursorHolderVtable;
static u32 sFixedCursorSecondaryVtable;
static hdUiStringTable sFixedCursorSafeStringVtable;
static hdPartsStateIdVtbl_c sFixedCursorStateIdVtable;
static hdStateId sFixedCursorNoStateId;
static hdPartsStateId_c sFixedCursorPartsBlank; /* hdParts_c's ::StateID_Blank (hd_parts.cpp) */
static hdScreenUiSystem* sFixedCursorUiSystem;
static hdCursorController_c* sFixedCursorController;
static void* sFixedCursorEventMgr;
static u32 sFixedCursorRootTypeReady;
static hdPartsTypeInfo_c sFixedCursorRootType;

static hdUiSafeString sFixedCursorWindowName;
static u32 sFixedCursorWindowNameReady;
static hdUiSafeString sFixedCursorArchiveName;
static hdUiSafeString sFixedCursorGroupNames[2] = {};
static hdUiSafeString sFixedCursorAnimNames[3] = {};
static hdPartsStateId_c sFixedCursorBlank;
static hdPartsStateId_c sFixedCursorInAll;
static hdPartsStateId_c sFixedCursorLoopAll;
static hdPartsStateId_c sFixedCursorOutAll;
static hdUiSafeString sFixedCursorLayoutName;
static hdCursorSize_c sFixedCursorScale; /* set to 1 by the static initialiser, not read here */
static hdCursorSize_c sFixedCursorPosition;
static hdCursorSize_c sFixedCursorSize;
static u32 sFixedCursorAnimGroups[3] = {1, 0, 1};
static u32 sFixedCursorStateNumberReady;
static s32 sFixedCursorStateNumber;

/* 0265288C */
// exit-time destruction of findWindowPane's function-local static name
static void hdScreenFixedCursor_destroyWindowName() {
    hdCursor_destroyArray(&sFixedCursorWindowName, 1, sizeof(hdUiSafeString), 0x02653350);
}

/* 026528B0 */
// hdScreenFixedCursor_c::~hdScreenFixedCursor_c() (GHS deleting destructor)
void hdScreenFixedCursor_c::destroy(hdScreenFixedCursor_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sFixedCursorHolderVtable;
    i_this->mSecondaryVtable = &sFixedCursorSecondaryVtable;
    hdParts_c::destroy(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 0265291C */
// hidden: shows itself once the cursor drives the menus and an element has focus
void hdScreenFixedCursor_c::blankExecute() {
    if (sFixedCursorController->mCursorActive && hdCursor_getFocus(sFixedCursorEventMgr) != NULL) {
        changeState(&sFixedCursorInAll);
    }
}

/* 0265297C */
void hdScreenFixedCursor_c::inAllEnter() {
    hdAnimSet_play(animSet(), CURSOR_ANIM_IN_ALL, 1, 1.0f);
}

/* 02652998 */
void hdScreenFixedCursor_c::inAllExecute() {
    if (hdAnimSet_isEnd(animSet(), CURSOR_ANIM_IN_ALL)) {
        changeState(&sFixedCursorLoopAll);
    }
    vCalcLayout();
}

/* 026529FC */
void hdScreenFixedCursor_c::loopAllEnter() {
    hdAnimSet_play(animSet(), CURSOR_ANIM_LOOP_ALL, 0, 1.0f);
}

/* 02652A18 */
void hdScreenFixedCursor_c::loopAllExecute() {
    if (!sFixedCursorController->mCursorActive || hdCursor_getFocus(sFixedCursorEventMgr) == NULL) {
        changeState(&sFixedCursorOutAll);
    }
    vCalcLayout();
}

/* 02652A8C */
void hdScreenFixedCursor_c::outAllEnter() {
    hdAnimSet_play(animSet(), CURSOR_ANIM_OUT_ALL, 1, 1.0f);
}

/* 02652AA8 */
void hdScreenFixedCursor_c::outAllExecute() {
    if (hdAnimSet_isEnd(animSet(), CURSOR_ANIM_OUT_ALL)) {
        changeState(&sFixedCursorBlank);
    }
    vCalcLayout();
}

/* 02652B0C */
// hdScreenFixedCursor_c::hdScreenFixedCursor_c() (GHS: allocates when called with NULL)
hdScreenFixedCursor_c* hdScreenFixedCursor_c::construct(hdScreenFixedCursor_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenFixedCursor_c*)operator_new(sizeof(hdScreenFixedCursor_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdParts_c::construct(i_this);
    i_this->mSecondaryVtable = &sFixedCursorSecondaryVtable;
    i_this->mVtable = &sFixedCursorVtable;
    i_this->mStateHolder.mHolderVtable = &sFixedCursorHolderVtable;
    // HD: GHS tests the address of the (trivially constructed) pane pointer member
    if (gabi::ea(&i_this->mpWindowPane) == 0) {
        operator_new(sizeof(i_this->mpWindowPane));
    }
    i_this->mpWindowPane = NULL;
    return i_this;
}

/* inlined into findWindowPane: the runtime type every pane derives from */
static hdPartsTypeInfo_c* hdScreenFixedCursor_getRootTypeInfo() {
    if (!sFixedCursorRootTypeReady) {
        sFixedCursorRootTypeReady = 1;
        sFixedCursorRootType.mParent = NULL;
    }
    return &sFixedCursorRootType;
}

/* inlined: the type test of a dynamic cast */
static bool hdScreenFixedCursor_isDerivedFrom(hdPartsTypeInfo_c* type, hdPartsTypeInfo_c* base) {
    for (; type != NULL; type = type->mParent) {
        if (type == base) {
            return true;
        }
    }
    return false;
}

/* 02652B90 */
void hdScreenFixedCursor_c::findWindowPane() {
    if (!sFixedCursorWindowNameReady) {
        sFixedCursorWindowNameReady = 1;
        hdParts_setSafeString(&sFixedCursorWindowName, &sFixedCursorSafeStringVtable, "W_Cursor_00");
        __register_global_object(gabi::ea(&sScreenFixedCursorRegistration));
    }
    hdCursorPane_c* root = (hdCursorPane_c*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    hdCursorPaneVtbl_c* rootVtable = root->mVtable;
    sFixedCursorWindowName.assureTermination();
    hdCursorPane_c* pane = gabi::call_ptr<hdCursorPane_c*>(rootVtable->mFindPaneByName.get(), root, (hdCursorChar_t*)sFixedCursorWindowName.mText, 1u);
    hdPartsTypeInfo_c* rootType = hdScreenFixedCursor_getRootTypeInfo();
    if (pane != NULL && hdScreenFixedCursor_isDerivedFrom(pane->getRuntimeTypeInfo(), rootType)) {
        mpWindowPane = pane;
    } else {
        mpWindowPane = NULL;
    }
}

/* 02652CDC */
u32 hdScreenFixedCursor_c::init(u32 arg1, u32 arg2, void* desc) {
    attachArchives(&sFixedCursorArchiveName, 1);
    if (!vLoad(&sFixedCursorLayoutName, gabi::ea(mpResAccessor.get()), arg1, arg2, desc)) {
        return FALSE;
    }
    findWindowPane();
    hdAnimSet_operationCC(animSet(), CURSOR_ANIM_IN_ALL, 1);
    vCalcLayout();
    startState(&sFixedCursorBlank);
    return TRUE;
}

/* 02652DF4 */
void hdScreenFixedCursor_c::finalize() {
    vDestroyLayout();
    detachArchives();
}

/* 02652E34 */
void hdScreenFixedCursor_c::executeState() {
    hdParts_executeStateMachine(stateMachine());
}

/* 02652E3C */
void hdScreenFixedCursor_c::draw(void* info) {
    vDrawLayout(info);
}

/* 02652E4C */
void hdScreenFixedCursor_c::setPosition(hdCursorSize_c* position) {
    hdCursor_copyFloat(&sFixedCursorPosition.x, &position->x);
    hdCursor_copyFloat(&sFixedCursorPosition.y, &position->y);
}

/* 02652E64 */
void hdScreenFixedCursor_c::setSize(hdCursorSize_c* size) {
    hdCursor_copyFloat(&sFixedCursorSize.x, &size->x);
    hdCursor_copyFloat(&sFixedCursorSize.y, &size->y);
}

/* 02652E7C */
u32 hdScreenFixedCursor_c::load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sFixedCursorUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, arg1, arg2, arg3, 3, 2, desc)) {
        return FALSE;
    }
    for (u32 i = 0; i < 3; i++) {
        hdAnimSet_bindGroup(animSet(), i, &sFixedCursorAnimNames[i], &sFixedCursorGroupNames[sFixedCursorAnimGroups[i]]);
    }
    vCalcLayout();
    return TRUE;
}

/* 02652FA8 */
// the root moves to the set position; the window takes the set size
void hdScreenFixedCursor_c::calcLayout() {
    if (mpLayout == NULL) {
        return;
    }
    hdCursorPane_c* root = (hdCursorPane_c*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    hdCursor_copyFloat(&root->mTranslate.x, &sFixedCursorPosition.x);
    hdCursor_copyFloat(&root->mTranslate.y, &sFixedCursorPosition.y);
    root->mFlags = root->mFlags | CURSOR_PANE_DIRTY;
    root->mTranslate.z = 0.0f;
    if (mpWindowPane != NULL) {
        hdCursorPane_c* window = mpWindowPane;
        hdCursor_copyFloat(&window->mWidth, &sFixedCursorSize.x);
        hdCursor_copyFloat(&window->mHeight, &sFixedCursorSize.y);
        window->mFlags = window->mFlags | CURSOR_PANE_DIRTY;
    }
    mpLayout->calc();
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 02653030 */
static void __sinit_hd_screen_fixed_cursor_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sFixedCursorHeaderStatics), gabi::ea(&sFixedCursorHeaderDescriptors), gabi::ea(&sFixedCursorHeaderZero),
                                gabi::ea(&sFixedCursorPiConstants));
    sFixedCursorScale.x = 1.0f;
    sFixedCursorScale.y = 1.0f;
    sFixedCursorPosition.x = 0.0f;
    sFixedCursorPosition.y = 0.0f;
    sFixedCursorSize.x = 1.0f;
    sFixedCursorSize.y = 1.0f;
    hdParts_setSafeString(&sFixedCursorGroupNames[0], &sFixedCursorSafeStringVtable, "G_LoopAll_00");
    hdParts_setSafeString(&sFixedCursorGroupNames[1], &sFixedCursorSafeStringVtable, "G_All_00");
    hdParts_setSafeString(&sFixedCursorAnimNames[0], &sFixedCursorSafeStringVtable, "InAll");
    hdParts_setSafeString(&sFixedCursorAnimNames[1], &sFixedCursorSafeStringVtable, "LoopAll");
    hdParts_setSafeString(&sFixedCursorAnimNames[2], &sFixedCursorSafeStringVtable, "OutAll");
    hdParts_setSafeString(&sFixedCursorArchiveName, &sFixedCursorSafeStringVtable, "Cursor_02");
    hdParts_setSafeString(&sFixedCursorLayoutName, &sFixedCursorSafeStringVtable, "Cursor_02");
    s32 number;
    if (sFixedCursorStateNumberReady) {
        number = sFixedCursorStateNumber;
    } else {
        number = 0;
        sFixedCursorStateNumberReady = 1;
    }
    // the cursor's Blank refines hdParts_c's (its field +0x2C names the parent state)
    sFixedCursorBlank.setup(number + 1, "::StateID_Blank", &sFixedCursorStateIdVtable, 4, (hdStateId*)&sFixedCursorPartsBlank);
    sFixedCursorInAll.setup(number + 2, "::StateID_InAll", &sFixedCursorStateIdVtable, 0x19, &sFixedCursorNoStateId);
    sFixedCursorLoopAll.setup(number + 3, "::StateID_LoopAll", &sFixedCursorStateIdVtable, 0x1C, &sFixedCursorNoStateId);
    sFixedCursorOutAll.setup(number + 4, "::StateID_OutAll", &sFixedCursorStateIdVtable, 0x1F, &sFixedCursorNoStateId);
    sFixedCursorStateNumber = number + 4;
}

/* 0265337C */
static s32 hdScreenFixedCursor_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue
data sScreenFixedCursorRegistration 101F59F0 nocheck
data sFixedCursorHeaderStatics 10491FAC nocheck
data sFixedCursorHeaderDescriptors 101F59FC nocheck
data sFixedCursorHeaderZero 10491FB8 nocheck
data sFixedCursorPiConstants 100EFB84 nocheck
data sFixedCursorVtable 100EFC18 nocheck
data sFixedCursorHolderVtable 100EFD28 nocheck
data sFixedCursorSecondaryVtable 100EFD38 nocheck
data sFixedCursorSafeStringVtable 100EFABC nocheck
data sFixedCursorStateIdVtable 100EFB28 nocheck
data sFixedCursorNoStateId 101FF32C nocheck
data sFixedCursorPartsBlank 1049DD60 nocheck
data sFixedCursorUiSystem 1018C404 nocheck
data sFixedCursorController 101F8378 nocheck
data sFixedCursorEventMgr 101F4FF0 nocheck
data sFixedCursorRootTypeReady 101FD888 nocheck
data sFixedCursorRootType 101FD8FC nocheck
data sFixedCursorWindowName 10491F9C nocheck
data sFixedCursorWindowNameReady 10491FF0 nocheck
data sFixedCursorArchiveName 10491FA4 nocheck
data sFixedCursorGroupNames 10491FC8 nocheck
data sFixedCursorAnimNames 10491FD8 nocheck
data sFixedCursorBlank 10491FF4 nocheck
data sFixedCursorInAll 10492024 nocheck
data sFixedCursorLoopAll 10492054 nocheck
data sFixedCursorOutAll 10492084 nocheck
data sFixedCursorLayoutName 104920B4 nocheck
data sFixedCursorScale 104920BC nocheck
data sFixedCursorPosition 104920C4 nocheck
data sFixedCursorSize 104920CC nocheck
data sFixedCursorAnimGroups 100EFAD4
data sFixedCursorStateNumberReady 101FD8E4 nocheck
data sFixedCursorStateNumber 101FDD50 nocheck
str 100EFB74 "W_Cursor_00" in 02652B90
str 100EFBE4 "G_LoopAll_00" in 02653030
str 100EFBF4 "G_All_00" in 02653030
str 100EFB94 "InAll" in 02653030
str 100EFB8C "LoopAll" in 02653030
str 100EFB9C "OutAll" in 02653030
str 100EFBC4 "Cursor_02" in 02653030
str 100EFBA4 "::StateID_Blank" in 02653030
str 100EFBB4 "::StateID_InAll" in 02653030
str 100EFBD0 "::StateID_LoopAll" in 02653030
str 100EFC00 "::StateID_OutAll" in 02653030
*/

/**
 * hd_screen_msg_cursor.cpp
 * HD - the "Cursor_01" screen: the cursor frame of message choices (sized from the measured choice
 * lines, moved to the chosen line); states ::StateID_InAll / _LoopAll / _OutAll
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_menu_cursor). Each function is marked with its WWHD address; the glue block at the end lists
 * this translation unit's data (tools/verify/autoglue.py). Layout, pane, animation and state names are
 * HD strings; all other names are ours.
 */

#include "d/hd_screen_msg_cursor.h"
static u32 sScreenMsgCursorRegistration;
static u32 sScreenMsgCursorSecondRegistration;
static u32 sMsgCursorHeaderStatics;
static u32 sMsgCursorHeaderDescriptors;
static u32 sMsgCursorHeaderZero;
static u32 sMsgCursorPiConstants;

static hdPartsVtbl_c sMsgCursorVtable;
static u32 sMsgCursorHolderVtable;
static u32 sMsgCursorSecondaryVtable;
static hdUiStringTable sMsgCursorSafeStringVtable;
static hdPartsStateIdVtbl_c sMsgCursorStateIdVtable;
static hdStateId sMsgCursorNoStateId;
static hdPartsStateId_c sMsgCursorPartsBlank; /* hdParts_c's ::StateID_Blank (hd_parts.cpp) */
static hdScreenUiSystem* sMsgCursorUiSystem;
static hdMsgCursorMsgMgr_c* sMsgCursorMsgMgr;

static hdUiSafeString sMsgCursorAllName;
static hdUiSafeString sMsgCursorWindowName;
static u32 sMsgCursorAllNameReady;
static u32 sMsgCursorWindowNameReady;
static hdUiSafeString sMsgCursorArchiveName;
static hdUiSafeString sMsgCursorGroupNames[2] = {};
static hdUiSafeString sMsgCursorAnimNames[3] = {};
static hdPartsStateId_c sMsgCursorInAll;
static hdPartsStateId_c sMsgCursorLoopAll;
static hdPartsStateId_c sMsgCursorOutAll;
static hdUiSafeString sMsgCursorLayoutName;
static u32 sMsgCursorAnimGroups[3] = {1, 0, 1};
static u32 sMsgCursorStateNumberReady;
static s32 sMsgCursorStateNumber;

/* 02651558 */
// exit-time destruction of findAllPane's function-local static name
static void hdScreenMsgCursor_destroyAllName() {
    hdCursor_destroyArray(&sMsgCursorAllName, 1, sizeof(hdUiSafeString), 0x02652764);
}

/* 0265157C */
// exit-time destruction of findWindowPane's function-local static name
static void hdScreenMsgCursor_destroyWindowName() {
    hdCursor_destroyArray(&sMsgCursorWindowName, 1, sizeof(hdUiSafeString), 0x02652764);
}

/* 026515A0 */
// hdScreenMsgCursor_c::~hdScreenMsgCursor_c() (GHS deleting destructor)
void hdScreenMsgCursor_c::destroy(hdScreenMsgCursor_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sMsgCursorHolderVtable;
    i_this->mSecondaryVtable = &sMsgCursorSecondaryVtable;
    hdParts_c::destroy(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 0265160C */
void hdScreenMsgCursor_c::startAnim(u32 anim, f32 rate) {
    hdAnimSet_play(animSet(), anim, sMsgCursorAnimGroups[anim], rate);
}

/* 02651628 */
void hdScreenMsgCursor_c::inAllEnter() {
    hdCursorPane_c* pane = mpAllPane;
    pane->mFlags = (pane->mFlags & ~CURSOR_PANE_VISIBLE) + CURSOR_PANE_VISIBLE;
    startAnim(CURSOR_ANIM_IN_ALL, 1.0f);
}

/* 0265164C */
u32 hdScreenMsgCursor_c::isAnimEnd(u32 anim) {
    return hdAnimSet_isEnd(animSet(), anim);
}

/* 02651658 */
void hdScreenMsgCursor_c::inAllExecute() {
    if (isAnimEnd(CURSOR_ANIM_IN_ALL)) {
        changeState(&sMsgCursorLoopAll);
    }
}

/* 026516A0 */
void hdScreenMsgCursor_c::loopAllEnter() {
    startAnim(CURSOR_ANIM_LOOP_ALL, 1.0f);
}

/* 026516B0 */
void hdScreenMsgCursor_c::outAllEnter() {
    startAnim(CURSOR_ANIM_OUT_ALL, 1.0f);
}

/* 026516C0 */
// invisible, animations rewound, back to Blank
void hdScreenMsgCursor_c::hide() {
    hdCursorPane_c* pane = mpAllPane;
    pane->mFlags = pane->mFlags & ~CURSOR_PANE_VISIBLE;
    startAnim(CURSOR_ANIM_IN_ALL, 0.0f);
    startAnim(CURSOR_ANIM_LOOP_ALL, 0.0f);
    changeState(&sMsgCursorPartsBlank);
}

/* 02651744 */
void hdScreenMsgCursor_c::outAllExecute() {
    if (isAnimEnd(CURSOR_ANIM_OUT_ALL)) {
        hide();
    }
}

/* 02651784 */
// hdScreenMsgCursor_c::hdScreenMsgCursor_c() (GHS: allocates when called with NULL)
hdScreenMsgCursor_c* hdScreenMsgCursor_c::construct(hdScreenMsgCursor_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenMsgCursor_c*)operator_new(sizeof(hdScreenMsgCursor_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdParts_c::construct(i_this);
    i_this->mVtable = &sMsgCursorVtable;
    i_this->mSecondaryVtable = &sMsgCursorSecondaryVtable;
    i_this->mStateHolder.mHolderVtable = &sMsgCursorHolderVtable;
    // HD: GHS tests the address of each (trivially constructed) member
    if (gabi::ea(&i_this->mpAllPane) == 0) {
        operator_new(sizeof(i_this->mpAllPane));
    }
    if (gabi::ea(&i_this->mpWindowPane) == 0) {
        operator_new(sizeof(i_this->mpWindowPane));
    }
    if (gabi::ea(i_this->mLinePositions) == 0) {
        operator_new(sizeof(i_this->mLinePositions));
    }
    i_this->mDrawFront = TRUE;
    i_this->mBaseWidth = 0.0f;
    i_this->mpAllPane = NULL;
    return i_this;
}

/* inlined into findAllPane and findWindowPane: a pane of the layout by name */
static hdCursorPane_c* hdScreenMsgCursor_findPane(hdParts_c* parts, hdUiSafeString* name) {
    hdCursorPane_c* root = (hdCursorPane_c*)(hdPartsPane_c*)parts->mpLayout->lyt()->mpRootPane;
    hdCursorPaneVtbl_c* rootVtable = root->mVtable;
    name->assureTermination();
    return gabi::call_ptr<hdCursorPane_c*>(rootVtable->mFindPaneByName.get(), root, (hdCursorChar_t*)name->mText, 1u);
}

/* 0265183C */
void hdScreenMsgCursor_c::findAllPane() {
    if (!sMsgCursorAllNameReady) {
        sMsgCursorAllNameReady = 1;
        hdParts_setSafeString(&sMsgCursorAllName, &sMsgCursorSafeStringVtable, "N_All_00");
        __register_global_object(gabi::ea(&sScreenMsgCursorRegistration));
    }
    mpAllPane = hdScreenMsgCursor_findPane(this, &sMsgCursorAllName);
}

/* 02651900 */
void hdScreenMsgCursor_c::findWindowPane() {
    if (!sMsgCursorWindowNameReady) {
        sMsgCursorWindowNameReady = 1;
        hdParts_setSafeString(&sMsgCursorWindowName, &sMsgCursorSafeStringVtable, "W_Cursor_00");
        __register_global_object(gabi::ea(&sScreenMsgCursorSecondRegistration));
    }
    mpWindowPane = hdScreenMsgCursor_findPane(this, &sMsgCursorWindowName);
}

/* 026519C4 */
u32 hdScreenMsgCursor_c::init(u32 arg1, u32 arg2, void* desc) {
    sMsgCursorMsgMgr->mpChoiceCursor = this;
    attachArchives(&sMsgCursorArchiveName, 1);
    if (!vLoad(&sMsgCursorLayoutName, gabi::ea(mpResAccessor.get()), arg1, arg2, desc)) {
        return FALSE;
    }
    findAllPane();
    findWindowPane();
    hdCursor_copyFloat(&mBaseWidth, &mpWindowPane->mWidth);
    startState(&sMsgCursorPartsBlank);
    return TRUE;
}

/* 02651AD4 */
void hdScreenMsgCursor_c::finalize() {
    sMsgCursorMsgMgr->mpChoiceCursor = NULL;
    vDestroyLayout();
    detachArchives();
}

/* inlined: is the state machine in `stateId` (the state's vtable is read before the current key) */
static bool hdScreenMsgCursor_isState(hdParts_c* parts, hdPartsStateId_c* stateId) {
    hdPartsStateId_c* current = hdParts_getCurrentStateId(parts->stateMachine());
    u32 getCurrentKey = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* vtable = stateId->mVtable;
    u32 key = gabi::call_ptr<u32>(getCurrentKey, current);
    return key == gabi::call_ptr<u32>(vtable->mGetKey.get(), stateId);
}

/* 02651B24 */
// nothing runs while hidden (Blank)
void hdScreenMsgCursor_c::executeState() {
    if (hdScreenMsgCursor_isState(this, &sMsgCursorPartsBlank)) {
        return;
    }
    hdParts_executeStateMachine(stateMachine());
    vCalcLayout();
}

/* 02651BB8 */
void hdScreenMsgCursor_c::draw(void* info) {
    if (!mDrawFront) {
        return;
    }
    if (!hdScreenMsgCursor_isState(this, &sMsgCursorPartsBlank)) {
        vDrawLayout(info);
    }
}

/* 02651C60 */
void hdScreenMsgCursor_c::drawBack(void* info) {
    if (mDrawFront) {
        return;
    }
    if (!hdScreenMsgCursor_isState(this, &sMsgCursorPartsBlank)) {
        vDrawLayout(info);
    }
}

/* inlined into measureLine: the line's width with the cursor's margin, scaled */
static s32 hdScreenMsgCursor_scaleWidth(s32 width, f32 scale) {
    return gabi::ftoi(gabi::fmadds((f32)width, scale, 1.0f));
}

/* 02651D08 */
// width of the text up to the next line break (tags skipped); the text cursor moves past it
s32 hdScreenMsgCursor_c::measureLine(gptr<hdMsgCursorChar_t>* text, f32 scale, hdMsgCursorFont_c* font) {
    s32 width = 0;
    u32 c = *hdPartsView(text->get());
    while (c != 0) {
        if (c == MSG_CURSOR_CODE_TAG || c == MSG_CURSOR_CODE_SHORT_TAG) {
            hdMsgCursorChar_t* p = hdPartsView(text->get());
            if (*p == MSG_CURSOR_CODE_TAG) {
                hdMsgCursorTag_c* tag = (hdMsgCursorTag_c*)p;
                p = hdMsgCursor_advance(p, tag->mParamSize + 8);
            } else if (*p == MSG_CURSOR_CODE_SHORT_TAG) {
                p = hdMsgCursor_advance(p, 6);
            }
            *text = p;
            c = *hdPartsView(p);
        } else if (c == MSG_CURSOR_CODE_NEWLINE) {
            *text = hdMsgCursor_advance(text->get(), 2);
            return hdScreenMsgCursor_scaleWidth(width, scale);
        } else {
            s32 charWidth = font->getCharWidth(c);
            hdMsgCursorChar_t* p = *text;
            *text = hdMsgCursor_advance(p, 2);
            c = *hdMsgCursor_advance(p, 2);
            width += charWidth;
        }
    }
    return hdScreenMsgCursor_scaleWidth(width, scale);
}

/* 02651E98 */
void hdScreenMsgCursor_c::setWidth(s32 textWidth) {
    hdCursorPane_c* window = mpWindowPane;
    window->mFlags = window->mFlags | CURSOR_PANE_DIRTY;
    window->mWidth = mBaseWidth + (f32)textWidth;
}

/* 02651EE4 */
// three choice lines of one message: their positions, and the window sized to the widest line
void hdScreenMsgCursor_c::setLines(cXyz* positions, hdUiSafeString* message, hdMsgCursorFont_c* font, f32 scale) {
    s32 maxWidth = 0;
    message->assureTermination();
    gabi::Local<gptr<hdMsgCursorChar_t>> text;
    text->v = message->mText.v;
    for (u32 i = 0; i < 3; i++) {
        mLinePositions[i] = positions[i];
        s32 width = measureLine(text, scale, font);
        if (maxWidth < width) {
            maxWidth = width;
        }
    }
    setWidth(maxWidth);
}

/* 02652020 */
// two choice lines from two messages
void hdScreenMsgCursor_c::setLines2(cXyz* positions, hdUiSafeString* first, hdUiSafeString* second, hdMsgCursorFont_c* font, f32 scale) {
    s32 maxWidth = 0;
    gabi::Local<hdMsgCursorLines_c> texts;
    for (u32 i = 0; i < 3; i++) {
        texts->mText[i] = NULL;
    }
    first->assureTermination();
    texts->mText[0].v = first->mText.v;
    second->assureTermination();
    texts->mText[1].v = second->mText.v;
    for (u32 i = 0; i < 3; i++) {
        mLinePositions[i] = positions[i];
        if (texts->mText[i] != NULL) {
            s32 width = measureLine(&texts->mText[i], scale, font);
            if (maxWidth < width) {
                maxWidth = width;
            }
        }
    }
    setWidth(maxWidth);
}

/* 026521B8 */
// open (from Blank) or close (from LoopAll) the cursor on choice `line`; the root group takes the line's
// position and the rotation of `rotationSource`
u32 hdScreenMsgCursor_c::select(BOOL open, u32 line, hdCursorPane_c* rotationSource, u8 drawFront) {
    if (open) {
        if (hdScreenMsgCursor_isState(this, &sMsgCursorPartsBlank)) {
            changeState(&sMsgCursorInAll);
        } else if (!hdScreenMsgCursor_isState(this, &sMsgCursorLoopAll)) {
            return FALSE;
        }
    } else {
        if (!hdScreenMsgCursor_isState(this, &sMsgCursorLoopAll)) {
            return FALSE;
        }
        changeState(&sMsgCursorOutAll);
    }
    hdCursorPane_c* pane = mpAllPane;
    // HD: sead SafeArray access: an index out of range uses element 0
    pane->mTranslate = mLinePositions[line < 3 ? line : 0];
    pane->mFlags = pane->mFlags | CURSOR_PANE_DIRTY;
    pane = mpAllPane;
    pane->mRotate = rotationSource->mRotate;
    pane->mFlags = pane->mFlags | CURSOR_PANE_DIRTY;
    mDrawFront = drawFront;
    return TRUE;
}

/* 0265233C */
u32 hdScreenMsgCursor_c::isLooping() {
    return hdScreenMsgCursor_isState(this, &sMsgCursorLoopAll);
}

/* 026523AC */
u32 hdScreenMsgCursor_c::load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sMsgCursorUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, arg1, arg2, arg3, 3, 2, desc)) {
        return FALSE;
    }
    for (u32 i = 0; i < 3; i++) {
        hdAnimSet_bindGroup(animSet(), i, &sMsgCursorAnimNames[i], &sMsgCursorGroupNames[sMsgCursorAnimGroups[i]]);
    }
    vCalcLayout();
    return TRUE;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 026524D8 */
static void __sinit_hd_screen_msg_cursor_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sMsgCursorHeaderStatics), gabi::ea(&sMsgCursorHeaderDescriptors), gabi::ea(&sMsgCursorHeaderZero),
                                gabi::ea(&sMsgCursorPiConstants));
    hdParts_setSafeString(&sMsgCursorGroupNames[0], &sMsgCursorSafeStringVtable, "G_LoopAll_00");
    hdParts_setSafeString(&sMsgCursorGroupNames[1], &sMsgCursorSafeStringVtable, "G_All_00");
    hdParts_setSafeString(&sMsgCursorAnimNames[0], &sMsgCursorSafeStringVtable, "InAll");
    hdParts_setSafeString(&sMsgCursorAnimNames[1], &sMsgCursorSafeStringVtable, "LoopAll");
    hdParts_setSafeString(&sMsgCursorAnimNames[2], &sMsgCursorSafeStringVtable, "OutAll");
    hdParts_setSafeString(&sMsgCursorArchiveName, &sMsgCursorSafeStringVtable, "Cursor_01");
    hdParts_setSafeString(&sMsgCursorLayoutName, &sMsgCursorSafeStringVtable, "Cursor_01");
    s32 number;
    if (sMsgCursorStateNumberReady) {
        number = sMsgCursorStateNumber;
    } else {
        number = 0;
        sMsgCursorStateNumberReady = 1;
    }
    sMsgCursorInAll.setup(number + 1, "::StateID_InAll", &sMsgCursorStateIdVtable, 0x19, &sMsgCursorNoStateId);
    sMsgCursorLoopAll.setup(number + 2, "::StateID_LoopAll", &sMsgCursorStateIdVtable, 0x1C, &sMsgCursorNoStateId);
    sMsgCursorOutAll.setup(number + 3, "::StateID_OutAll", &sMsgCursorStateIdVtable, 0x1F, &sMsgCursorNoStateId);
    sMsgCursorStateNumber = number + 3;
}

/* 0265278C */
static s32 hdScreenMsgCursor_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue
data sScreenMsgCursorRegistration 101F59B4 nocheck
data sScreenMsgCursorSecondRegistration 101F59C0 nocheck
data sMsgCursorHeaderStatics 10491EB8 nocheck
data sMsgCursorHeaderDescriptors 101F59CC nocheck
data sMsgCursorHeaderZero 10491EC4 nocheck
data sMsgCursorPiConstants 100EF908 nocheck
data sMsgCursorVtable 100EF988 nocheck
data sMsgCursorHolderVtable 100EFA98 nocheck
data sMsgCursorSecondaryVtable 100EFAA8 nocheck
data sMsgCursorSafeStringVtable 100EF82C nocheck
data sMsgCursorStateIdVtable 100EF898 nocheck
data sMsgCursorNoStateId 101FF32C nocheck
data sMsgCursorPartsBlank 1049DD60 nocheck
data sMsgCursorUiSystem 1018C404 nocheck
data sMsgCursorMsgMgr 101F4B5C nocheck
data sMsgCursorAllName 10491EA0 nocheck
data sMsgCursorWindowName 10491EA8 nocheck
data sMsgCursorAllNameReady 10491EFC nocheck
data sMsgCursorWindowNameReady 10491F00 nocheck
data sMsgCursorArchiveName 10491EB0 nocheck
data sMsgCursorGroupNames 10491ED4 nocheck
data sMsgCursorAnimNames 10491EE4 nocheck
data sMsgCursorInAll 10491F04 nocheck
data sMsgCursorLoopAll 10491F34 nocheck
data sMsgCursorOutAll 10491F64 nocheck
data sMsgCursorLayoutName 10491F94 nocheck
data sMsgCursorAnimGroups 100EF844
data sMsgCursorStateNumberReady 101FD8E4 nocheck
data sMsgCursorStateNumber 101FDD50 nocheck
str 100EF8E8 "N_All_00" in 0265183C
str 100EF8F4 "W_Cursor_00" in 02651900
str 100EF958 "G_LoopAll_00" in 026524D8
str 100EF968 "G_All_00" in 026524D8
str 100EF918 "InAll" in 026524D8
str 100EF910 "LoopAll" in 026524D8
str 100EF920 "OutAll" in 026524D8
str 100EF938 "Cursor_01" in 026524D8
str 100EF928 "::StateID_InAll" in 026524D8
str 100EF944 "::StateID_LoopAll" in 026524D8
str 100EF974 "::StateID_OutAll" in 026524D8
*/

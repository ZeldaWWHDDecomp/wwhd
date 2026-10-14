/**
 * hd_screen_staff_roll.cpp
 * HD - the staff roll "StaffRoll_00": fourteen text lines scroll up, fade near the top and bottom and are
 * refilled from the 187 staff roll labels; then the copyright line scrolls up alone
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_title_staff_roll). Each function is marked with its WWHD address; the glue block at the end
 * lists this translation unit's data (tools/verify/autoglue.py). Layout, pane, label and state names are
 * HD strings; all other names are ours.
 */

#include "d/hd_screen_staff_roll.h"

static hdPartsVtbl_c sStaffRollVtable;
static u32 sStaffRollHolderVtable;
static u32 sStaffRollSecondaryVtable;
static hdUiStringTable sStaffRollSafeStringVtable;
static hdPartsStateIdVtbl_c sStaffRollStateIdVtable;
static hdStateId sStaffRollNoStateId;
static hdScreenUiSystem* sStaffRollUiSystem;
static void* sStaffRollMsgBuilder;

/* the text box runtime type (lazily initialised; shared with the layout code) */
static u32 sStaffRollTextBoxTypeReady;
static hdPartsTypeInfo_c sStaffRollTextBoxType;
static u32 sStaffRollTextBoxBaseTypeReady;
static hdPartsTypeInfo_c sStaffRollTextBoxBaseType;
static u32 sStaffRollPaneTypeReady;
static hdPartsTypeInfo_c sStaffRollPaneType;
static u32 sStaffRollRootTypeReady;
static hdPartsTypeInfo_c sStaffRollRootType;

static hdUiSafeString sStaffRollArchiveName;
static f32 sStaffRollScreenHeight;     /* 720 */
static f32 sStaffRollScreenWidth;      /* 1280 */
static f32 sStaffRollConst75;
static f32 sStaffRollRecycleY;         /* a line above this is refilled at the bottom */
static f32 sStaffRollStartY;           /* the first line's y */
static f32 sStaffRollConst715;
static f32 sStaffRollConstMinus715;
static f32 sStaffRollFadeTop;          /* fully faded out above this */
static f32 sStaffRollFadeTopEnd;       /* fully visible below this */
static f32 sStaffRollFadeBottom;       /* fully faded out below this */
static f32 sStaffRollFadeBottomEnd;    /* fully visible above this */
static hdUiSafeString sStaffRollLabels[188] = {}; /* STAFF_ROLL_LABEL_COUNT labels and the copyright line */
static hdPartsStateId_c sStaffRollAll;
static hdPartsStateId_c sStaffRollCopyRight;
static hdUiSafeString sStaffRollLayoutName;
static u32 sStaffRollStateNumberReady;
static s32 sStaffRollStateNumber;

/* 026DFEE8 */
// hdScreenStaffRoll_c::~hdScreenStaffRoll_c() (GHS deleting destructor)
void hdScreenStaffRoll_c::destroy(hdScreenStaffRoll_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mStateHolder.mHolderVtable = &sStaffRollHolderVtable;
    i_this->mSecondaryVtable = &sStaffRollSecondaryVtable;
    hdParts_c::destroy(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 026DFF54 */
// a line's alpha at height y: faded out beyond the top / bottom edges, a 230-unit ramp, opaque between
f32 hdScreenStaffRoll_c::getAlpha(f32 y) {
    if (sStaffRollFadeTop < y) {
        return 0.0f;
    }
    if (y < sStaffRollFadeBottom) {
        return 0.0f;
    }
    u32 alpha = 0xFF;
    if (sStaffRollFadeTopEnd < y) {
        alpha = (u8)gabi::ftoi(((sStaffRollFadeTop - y) / 230.0f) * 255.0f);
    } else if (y < sStaffRollFadeBottomEnd) {
        alpha = (u8)gabi::ftoi(((y - sStaffRollFadeBottom) / 230.0f) * 255.0f);
    }
    return (f32)alpha;
}

/* 026E0040 */
// the name lines move up by the scroll speed and take the alpha of their new height
void hdScreenStaffRoll_c::scrollLines() {
    for (u32 i = STAFF_ROLL_FIRST_NAME; i <= STAFF_ROLL_LAST_NAME; i++) {
        hdStaffRollLine_c* src = line(i);
        f32 speed = mSpeed;
        f32 x = src->mX;
        f32 y = src->mY;
        hdStaffRollLine_c* dst = line(i);
        u8 flags = dst->mFlags;
        dst->mY = gabi::fadds_ppc(y, speed);
        dst->mX = x;
        dst->mFlags = flags | STAFF_ROLL_LINE_DIRTY;
        dst->mZ = 0.0f;
        f32 moved = gabi::fadds_ppc(y, mSpeed);
        hdStaffRollLine_c* faded = line(i);
        f32 alpha = getAlpha(moved);
        faded->mAlpha = (u8)gabi::ftoi(alpha);
    }
}

/* 026E0188 */
s32 hdScreenStaffRoll_c::getBottomLine() {
    return (mTop + mLineNum - 1) % mLineNum + 1;
}

/* 026E01AC */
s32 hdScreenStaffRoll_c::getTopLine() {
    return mTop + 1;
}

/* 026E01B8 */
// whether staff roll label `index` has no message (an empty line)
u32 hdScreenStaffRoll_c::isLabelEmpty(u32 index) {
    gabi::Local<hdStaffRollMsg_c> msg;
    hdStaffRoll_constructMessage(msg);
    hdStaffRoll_setMessageLabel(msg, &sStaffRollLayoutName, hdPartsView<hdUiSafeString>(gabi::ea(sStaffRollLabels) + index * 8));
    hdStaffRoll_buildMessage(sStaffRollMsgBuilder, msg, msg->mText, msg->mText2);
    return msg->mpEntry == NULL;
}

/* inlined: the text colours a line gets from its message (job title or name) */
static void hdStaffRoll_setColors(hdScreenStaffRoll_c* i_this, hdStaffRollColors_c* colors, hdStaffRollLine_c* line) {
    if (line->mIsHeading) {
        colors->mTop = i_this->mHeadingColors.mTop;
        colors->mBottom = i_this->mHeadingColors.mBottom;
    } else {
        colors->mTop = i_this->mNameColors.mTop;
        colors->mBottom = i_this->mNameColors.mBottom;
    }
}

/* inlined: show the next label on `line` (the layout's message data, the label's colours) */
static void hdStaffRoll_showLabel(hdScreenStaffRoll_c* i_this, u32 index, hdUiSafeString* label) {
    hdPartsLyt_c* lyt = i_this->mpLayout->lyt();
    gabi::Local<hdUiSafeString> data;
    data->mVtable = &sStaffRollSafeStringVtable;
    data->mText.v = lyt->mpMessageData.v;
    i_this->line(index)->textPane()->resetMessage(data, label);
}

/* 026E022C */
// the top line, once above the edge, takes the next label and moves below the bottom line
void hdScreenStaffRoll_c::refillTop() {
    hdStaffRollLine_c* top = line(getTopLine());
    if (sStaffRollRecycleY > top->mY) {
        return;
    }
    if (mNextLabel >= STAFF_ROLL_LABEL_COUNT) {
        changeState(&sStaffRollCopyRight);
        return;
    }
    u32 empty = isLabelEmpty(mNextLabel);
    s32 index = getTopLine();
    if (empty) {
        line(index)->mFlags = line(index)->mFlags & ~STAFF_ROLL_LINE_VISIBLE;
    } else {
        hdStaffRollLine_c* shown = line(index);
        shown->mFlags = (shown->mFlags & ~STAFF_ROLL_LINE_VISIBLE) + STAFF_ROLL_LINE_VISIBLE;
        index = getTopLine();
        hdStaffRoll_showLabel(this, index, hdPartsView<hdUiSafeString>(gabi::ea(sStaffRollLabels) + mNextLabel * 8));
        index = getTopLine();
        hdStaffRollLine_c* heading = line(index);
        hdStaffRollColors_c* colors = hdStaffRoll_getTextColors(line(index));
        if (colors != NULL) {
            hdStaffRoll_setColors(this, colors, heading);
        }
    }
    mNextLabel = mNextLabel + 1;
    f32 bottomY = line(getBottomLine())->mY;
    hdStaffRollLine_c* moved = line(getTopLine());
    f32 y = gabi::fsubs_ppc(bottomY, mLineHeight);
    f32 x = mX;
    u8 flags = moved->mFlags;
    moved->mY = y;
    moved->mX = x;
    moved->mFlags = flags | STAFF_ROLL_LINE_DIRTY;
    moved->mZ = 0.0f;
    mTop = (mTop + 1) % mLineNum;
}

/* 026E0618 */
void hdScreenStaffRoll_c::allExecute() {
    scrollLines();
    refillTop();
    vCalcLayout();
}

/* 026E0660 */
// the copyright line moves up like the names
void hdScreenStaffRoll_c::scrollAllRights() {
    f32 speed = mSpeed;
    hdStaffRollLine_c* last = mpLines[STAFF_ROLL_ALL_RIGHTS];
    f32 y = last->mY;
    f32 x = last->mX;
    u8 flags = last->mFlags;
    last->mX = x;
    last->mY = gabi::fadds_ppc(y, speed);
    last->mFlags = flags | STAFF_ROLL_LINE_DIRTY;
    last->mZ = 0.0f;
    f32 moved = gabi::fadds_ppc(y, mSpeed);
    hdStaffRollLine_c* faded = mpLines[STAFF_ROLL_ALL_RIGHTS];
    f32 alpha = getAlpha(moved);
    faded->mAlpha = (u8)gabi::ftoi(alpha);
}

/* 026E06D4 */
void hdScreenStaffRoll_c::copyRightExecute() {
    scrollLines();
    scrollAllRights();
    vCalcLayout();
}

/* 026E071C */
// hdScreenStaffRoll_c::hdScreenStaffRoll_c() (GHS: allocates when called with NULL)
hdScreenStaffRoll_c* hdScreenStaffRoll_c::construct(hdScreenStaffRoll_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenStaffRoll_c*)operator_new(sizeof(hdScreenStaffRoll_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdParts_c::construct(i_this);
    i_this->mVtable = &sStaffRollVtable;
    i_this->mSecondaryVtable = &sStaffRollSecondaryVtable;
    i_this->mStateHolder.mHolderVtable = &sStaffRollHolderVtable;
    // HD: GHS tests the address of the (trivially constructed) line array
    if (gabi::ea(i_this->mpLines) == 0) {
        operator_new(sizeof(i_this->mpLines));
    }
    i_this->mNextLabel = 0;
    i_this->mNameColors.mTop = 0xFFFFFFFF;
    i_this->mHeadingColors.mTop = 0xFFFFFFFF;
    i_this->mHeadingColors.mBottom = 0xFFFFFFFF;
    i_this->mLineHeight = 0.0f;
    i_this->mNameColors.mBottom = 0xFFFFFFFF;
    i_this->mLineNum = 0;
    i_this->mSpeed = 0.0f;
    i_this->mTop = 0;
    i_this->mX = 0.0f;
    return i_this;
}

/* inlined into init: the text box runtime type */
static hdPartsTypeInfo_c* hdStaffRoll_getTextBoxTypeInfo() {
    if (!sStaffRollTextBoxTypeReady) {
        sStaffRollTextBoxTypeReady = 1;
        if (!sStaffRollTextBoxBaseTypeReady) {
            sStaffRollTextBoxBaseTypeReady = 1;
            if (!sStaffRollPaneTypeReady) {
                sStaffRollPaneTypeReady = 1;
                if (!sStaffRollRootTypeReady) {
                    sStaffRollRootType.mParent = NULL;
                    sStaffRollRootTypeReady = 1;
                }
                sStaffRollPaneType.mParent = &sStaffRollRootType;
            }
            sStaffRollTextBoxBaseType.mParent = &sStaffRollPaneType;
        }
        sStaffRollTextBoxType.mParent = &sStaffRollTextBoxBaseType;
    }
    return &sStaffRollTextBoxType;
}

/* inlined: the type test of a dynamic cast */
static bool hdStaffRoll_isDerivedFrom(hdPartsTypeInfo_c* type, hdPartsTypeInfo_c* base) {
    for (; type != NULL; type = type->mParent) {
        if (type == base) {
            return true;
        }
    }
    return false;
}

/* 026E07D0 */
// the sixteen text panes, the colours of a job title and of a name, the first fourteen labels, the
// copyright line below them, and a scroll speed that brings everything through in 9750 frames
u32 hdScreenStaffRoll_c::init(u32 arg1, u32 arg2, void* desc) {
    attachArchives(&sStaffRollArchiveName, 1);
    if (!vLoad(&sStaffRollLayoutName, gabi::ea(mpResAccessor.get()), arg1, arg2, desc)) {
        return FALSE;
    }
    gabi::Local<hdStaffRollNames_c> names;
    hdParts_setSafeString(&names->mNames[0], &sStaffRollSafeStringVtable, "T_TypeOfJob_00");
    hdParts_setSafeString(&names->mNames[1], &sStaffRollSafeStringVtable, "T_Staff_00");
    hdParts_setSafeString(&names->mNames[2], &sStaffRollSafeStringVtable, "T_Staff_01");
    hdParts_setSafeString(&names->mNames[3], &sStaffRollSafeStringVtable, "T_Staff_02");
    hdParts_setSafeString(&names->mNames[4], &sStaffRollSafeStringVtable, "T_Staff_03");
    hdParts_setSafeString(&names->mNames[5], &sStaffRollSafeStringVtable, "T_Staff_04");
    hdParts_setSafeString(&names->mNames[6], &sStaffRollSafeStringVtable, "T_Staff_05");
    hdParts_setSafeString(&names->mNames[7], &sStaffRollSafeStringVtable, "T_Staff_06");
    hdParts_setSafeString(&names->mNames[8], &sStaffRollSafeStringVtable, "T_Staff_07");
    hdParts_setSafeString(&names->mNames[9], &sStaffRollSafeStringVtable, "T_Staff_08");
    hdParts_setSafeString(&names->mNames[10], &sStaffRollSafeStringVtable, "T_Staff_09");
    hdParts_setSafeString(&names->mNames[11], &sStaffRollSafeStringVtable, "T_Staff_10");
    hdParts_setSafeString(&names->mNames[12], &sStaffRollSafeStringVtable, "T_Staff_11");
    hdParts_setSafeString(&names->mNames[13], &sStaffRollSafeStringVtable, "T_Staff_12");
    hdParts_setSafeString(&names->mNames[14], &sStaffRollSafeStringVtable, "T_Staff_13");
    hdParts_setSafeString(&names->mNames[15], &sStaffRollSafeStringVtable, "T_AllRights_00");
    hdStaffRollPane_c* root = (hdStaffRollPane_c*)(hdPartsPane_c*)mpLayout->lyt()->mpRootPane;
    for (u32 i = 0; i < STAFF_ROLL_LINE_COUNT; i++) {
        hdUiSafeString* name = &names->mNames[i];
        u32 assure = name->mVtable->mAssureTermination.get();
        hdStaffRollPaneVtbl_c* rootVtable = root->mVtable;
        gabi::call_ptr(assure, name);
        hdStaffRollPane_c* pane = gabi::call_ptr<hdStaffRollPane_c*>(rootVtable->mFindPaneByName.get(), root, (be<u8>*)name->mText, 1u);
        hdPartsTypeInfo_c* textBoxType = hdStaffRoll_getTextBoxTypeInfo();
        if (pane != NULL && hdStaffRoll_isDerivedFrom(pane->getRuntimeTypeInfo(), textBoxType)) {
            mpLines[i] = (hdStaffRollLine_c*)pane;
        } else {
            mpLines[i] = NULL;
        }
    }
    hdStaffRoll_destroyArray(names, STAFF_ROLL_LINE_COUNT, sizeof(hdUiSafeString), 0x026E2244);
    mNextLabel = 0;
    mLineNum = STAFF_ROLL_LAST_NAME;
    mLineHeight = mpLines[STAFF_ROLL_FIRST_NAME]->mHeight;
    f32 firstX = mpLines[STAFF_ROLL_FIRST_NAME]->mX;
    mTop = 0;
    mX = firstX;
    if (mpLines[0] != NULL) {
        hdStaffRollColors_c* colors = hdStaffRoll_getTextColors(mpLines[0]);
        if (colors != NULL) {
            mHeadingColors.mTop = colors->mTop;
            mHeadingColors.mBottom = colors->mBottom;
        }
    }
    if (mpLines[STAFF_ROLL_FIRST_NAME] != NULL) {
        hdStaffRollColors_c* colors = hdStaffRoll_getTextColors(mpLines[STAFF_ROLL_FIRST_NAME]);
        if (colors != NULL) {
            mNameColors.mTop = colors->mTop;
            mNameColors.mBottom = colors->mBottom;
        }
    }
    for (u32 i = STAFF_ROLL_FIRST_NAME; i <= STAFF_ROLL_LAST_NAME; i++) {
        line(i)->textPane()->setCharacterCapacity(30);
        f32 x = mX;
        hdStaffRollLine_c* placed = line(i);
        f32 startY = sStaffRollStartY;
        f32 height = mLineHeight;
        u8 flags = placed->mFlags;
        placed->mX = x;
        placed->mFlags = flags | STAFF_ROLL_LINE_DIRTY;
        placed->mY = gabi::fnmsubs(height, (f32)(s32)i, startY);
        placed->mZ = 0.0f;
        if (isLabelEmpty(isLabelEmpty(mNextLabel))) {
            line(i)->mFlags = line(i)->mFlags & ~STAFF_ROLL_LINE_VISIBLE;
        } else {
            hdStaffRollLine_c* shown = line(i);
            shown->mFlags = (shown->mFlags & ~STAFF_ROLL_LINE_VISIBLE) + STAFF_ROLL_LINE_VISIBLE;
            hdStaffRoll_showLabel(this, i, hdPartsView<hdUiSafeString>(gabi::ea(sStaffRollLabels) + mNextLabel * 8));
            hdStaffRollLine_c* heading = line(i);
            hdStaffRollColors_c* colors = hdStaffRoll_getTextColors(line(i));
            if (colors != NULL) {
                hdStaffRoll_setColors(this, colors, heading);
            }
        }
        mNextLabel = mNextLabel + 1;
    }
    mpLines[STAFF_ROLL_ALL_RIGHTS]->textPane()->setCharacterCapacity(300);
    hdStaffRollLine_c* last = mpLines[STAFF_ROLL_ALL_RIGHTS];
    f32 startY = sStaffRollStartY;
    u8 flags = last->mFlags;
    f32 lastX = mX;
    last->mY = startY;
    last->mX = lastX;
    last->mFlags = flags | STAFF_ROLL_LINE_DIRTY;
    last->mZ = 0.0f;
    hdStaffRoll_showLabel(this, STAFF_ROLL_ALL_RIGHTS, &sStaffRollLabels[STAFF_ROLL_LABEL_COUNT]);
    f32 travel = gabi::fmadds(187.0f, mLineHeight, mpLines[STAFF_ROLL_ALL_RIGHTS]->mHeight);
    f32 margin = sStaffRollScreenHeight + sStaffRollScreenHeight;
    mSpeed = (travel + margin) / 9750.0f;
    vCalcLayout();
    startState(&sStaffRollAll);
    return TRUE;
}

/* 026E1218 */
void hdScreenStaffRoll_c::finalize() {
    vDestroyLayout();
    detachArchives();
}

/* 026E1258 */
void hdScreenStaffRoll_c::executeState() {
    hdParts_executeStateMachine(stateMachine());
}

/* 026E1260 */
u32 hdScreenStaffRoll_c::load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    hdScreenHeap* heap = sStaffRollUiSystem->mHeap;
    hdPartsLayout_c* layout = (hdPartsLayout_c*)hdParts_new(sizeof(hdPartsLayout_c), heap, 4);
    if (layout != NULL) {
        layout = hdPartsLayout_c::construct(layout);
    }
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, arg1, arg2, arg3, 0, 0, desc)) {
        return FALSE;
    }
    vCalcLayout();
    return TRUE;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* inlined into the static initialiser: the label names "T_StaffRoll_00" ... "T_StaffRoll_186" */
static void hdStaffRoll_formatLabel(char* out, u32 index) {
    const char prefix[] = "T_StaffRoll_";
    int n = 0;
    for (; prefix[n]; n++) out[n] = prefix[n];
    if (index >= 100) out[n++] = char('0' + index / 100);
    out[n++] = char('0' + index / 10 % 10);
    out[n++] = char('0' + index % 10);
    out[n] = 0;
}

/* 026E1330 */
static void __sinit_hd_screen_staff_roll_cpp() {
    hdStaffRoll_initHeaderStatics(0x1049BB78, 0x101F6BBC, 0x1049BB40, 0x101054B8, 0x1049BB74);
    sStaffRollScreenHeight = 720.0f;
    sStaffRollScreenWidth = 1280.0f;
    sStaffRollConst75 = 75.0f;
    sStaffRollRecycleY = 435.0f;
    sStaffRollStartY = -510.0f;
    sStaffRollConst715 = 715.0f;
    sStaffRollConstMinus715 = -715.0f;
    sStaffRollFadeTop = 330.0f;
    sStaffRollFadeTopEnd = 100.0f;
    sStaffRollFadeBottom = -330.0f;
    sStaffRollFadeBottomEnd = -100.0f;
    hdParts_setSafeString(&sStaffRollArchiveName, &sStaffRollSafeStringVtable, "StaffRoll_00");
    hdParts_setSafeString(&sStaffRollLayoutName, &sStaffRollSafeStringVtable, "StaffRoll_00");
    for (u32 i = 0; i < STAFF_ROLL_LABEL_COUNT; i++) {
        char name[20] = {};
        hdStaffRoll_formatLabel(name, i);
        hdParts_setSafeString(&sStaffRollLabels[i], &sStaffRollSafeStringVtable, name);
    }
    hdParts_setSafeString(&sStaffRollLabels[STAFF_ROLL_LABEL_COUNT], &sStaffRollSafeStringVtable, "T_AllRights_00");
    s32 number;
    if (sStaffRollStateNumberReady) {
        number = sStaffRollStateNumber;
    } else {
        number = 0;
        sStaffRollStateNumberReady = 1;
    }
    sStaffRollAll.setup(number + 1, "::StateID_All", &sStaffRollStateIdVtable, 0x19, &sStaffRollNoStateId);
    sStaffRollCopyRight.setup(number + 2, "::StateID_CopyRight", &sStaffRollStateIdVtable, 0x1C, &sStaffRollNoStateId);
    sStaffRollStateNumber = number + 2;
}

/* 026E226C */
static s32 hdScreenStaffRoll_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue
data sStaffRollVtable 101060E0 nocheck
data sStaffRollHolderVtable 101061D8 nocheck
data sStaffRollSecondaryVtable 101061E8 nocheck
data sStaffRollSafeStringVtable 10105320 nocheck
data sStaffRollStateIdVtable 10105380 nocheck
data sStaffRollNoStateId 101FF32C nocheck
data sStaffRollUiSystem 1018C404 nocheck
data sStaffRollMsgBuilder 101F4AE8 nocheck
data sStaffRollTextBoxTypeReady 101FDD40
data sStaffRollTextBoxType 101FDD3C nocheck
data sStaffRollTextBoxBaseTypeReady 101FD8F0
data sStaffRollTextBoxBaseType 101FD8F4 nocheck
data sStaffRollPaneTypeReady 101FD89C
data sStaffRollPaneType 101FD8F8 nocheck
data sStaffRollRootTypeReady 101FD888
data sStaffRollRootType 101FD8FC nocheck
data sStaffRollArchiveName 1049BB38 nocheck
data sStaffRollScreenHeight 1049BB48 nocheck
data sStaffRollScreenWidth 1049BB4C nocheck
data sStaffRollConst75 1049BB50 nocheck
data sStaffRollRecycleY 1049BB54 nocheck
data sStaffRollStartY 1049BB58 nocheck
data sStaffRollConst715 1049BB5C nocheck
data sStaffRollConstMinus715 1049BB60 nocheck
data sStaffRollFadeTop 1049BB64 nocheck
data sStaffRollFadeTopEnd 1049BB68 nocheck
data sStaffRollFadeBottom 1049BB6C nocheck
data sStaffRollFadeBottomEnd 1049BB70 nocheck
data sStaffRollLabels 1049BB88 nocheck
data sStaffRollAll 1049C168 nocheck
data sStaffRollCopyRight 1049C198 nocheck
data sStaffRollLayoutName 1049C1C8 nocheck
data sStaffRollStateNumberReady 101FD8E4 nocheck
data sStaffRollStateNumber 101FDD50 nocheck
str 101053F0 "T_TypeOfJob_00" in 026E07D0
str 10105400 "T_Staff_00" in 026E07D0
str 1010540C "T_Staff_01" in 026E07D0
str 10105418 "T_Staff_02" in 026E07D0
str 10105424 "T_Staff_03" in 026E07D0
str 10105430 "T_Staff_04" in 026E07D0
str 1010543C "T_Staff_05" in 026E07D0
str 10105448 "T_Staff_06" in 026E07D0
str 10105454 "T_Staff_07" in 026E07D0
str 10105460 "T_Staff_08" in 026E07D0
str 1010546C "T_Staff_09" in 026E07D0
str 10105478 "T_Staff_10" in 026E07D0
str 10105484 "T_Staff_11" in 026E07D0
str 10105490 "T_Staff_12" in 026E07D0
str 1010549C "T_Staff_13" in 026E07D0
str 101054A8 "T_AllRights_00" in 026E07D0
str 10105A80 "StaffRoll_00" in 026E1330
str 10105A70 "::StateID_All" in 026E1330
str 10105A5C "::StateID_CopyRight" in 026E1330
str 10105A90 "T_StaffRoll_00" in 026E1330
str 10105AA0 "T_StaffRoll_01" in 026E1330
str 10105AB0 "T_StaffRoll_02" in 026E1330
str 10105AC0 "T_StaffRoll_03" in 026E1330
str 10105AD0 "T_StaffRoll_04" in 026E1330
str 10105AE0 "T_StaffRoll_05" in 026E1330
str 10105AF0 "T_StaffRoll_06" in 026E1330
str 10105B00 "T_StaffRoll_07" in 026E1330
str 10105B10 "T_StaffRoll_08" in 026E1330
str 10105B20 "T_StaffRoll_09" in 026E1330
str 10105B30 "T_StaffRoll_10" in 026E1330
str 10105B40 "T_StaffRoll_11" in 026E1330
str 10105B50 "T_StaffRoll_12" in 026E1330
str 10105B60 "T_StaffRoll_13" in 026E1330
str 10105B70 "T_StaffRoll_14" in 026E1330
str 10105B80 "T_StaffRoll_15" in 026E1330
str 10105B90 "T_StaffRoll_16" in 026E1330
str 10105BA0 "T_StaffRoll_17" in 026E1330
str 10105BB0 "T_StaffRoll_18" in 026E1330
str 10105BC0 "T_StaffRoll_19" in 026E1330
str 10105BD0 "T_StaffRoll_20" in 026E1330
str 10105BE0 "T_StaffRoll_21" in 026E1330
str 10105BF0 "T_StaffRoll_22" in 026E1330
str 10105C00 "T_StaffRoll_23" in 026E1330
str 10105C10 "T_StaffRoll_24" in 026E1330
str 10105C20 "T_StaffRoll_25" in 026E1330
str 10105C30 "T_StaffRoll_26" in 026E1330
str 10105C40 "T_StaffRoll_27" in 026E1330
str 10105C50 "T_StaffRoll_28" in 026E1330
str 10105C60 "T_StaffRoll_29" in 026E1330
str 10105C70 "T_StaffRoll_30" in 026E1330
str 10105C80 "T_StaffRoll_31" in 026E1330
str 10105C90 "T_StaffRoll_32" in 026E1330
str 10105CA0 "T_StaffRoll_33" in 026E1330
str 10105CB0 "T_StaffRoll_34" in 026E1330
str 10105CC0 "T_StaffRoll_35" in 026E1330
str 10105CD0 "T_StaffRoll_36" in 026E1330
str 10105CE0 "T_StaffRoll_37" in 026E1330
str 10105CF0 "T_StaffRoll_38" in 026E1330
str 10105D00 "T_StaffRoll_39" in 026E1330
str 10105D10 "T_StaffRoll_40" in 026E1330
str 10105D20 "T_StaffRoll_41" in 026E1330
str 10105D30 "T_StaffRoll_42" in 026E1330
str 10105D40 "T_StaffRoll_43" in 026E1330
str 10105D50 "T_StaffRoll_44" in 026E1330
str 10105D60 "T_StaffRoll_45" in 026E1330
str 10105D70 "T_StaffRoll_46" in 026E1330
str 10105D80 "T_StaffRoll_47" in 026E1330
str 10105D90 "T_StaffRoll_48" in 026E1330
str 10105DA0 "T_StaffRoll_49" in 026E1330
str 10105DB0 "T_StaffRoll_50" in 026E1330
str 10105DC0 "T_StaffRoll_51" in 026E1330
str 10105DD0 "T_StaffRoll_52" in 026E1330
str 10105DE0 "T_StaffRoll_53" in 026E1330
str 10105DF0 "T_StaffRoll_54" in 026E1330
str 10105E00 "T_StaffRoll_55" in 026E1330
str 10105E10 "T_StaffRoll_56" in 026E1330
str 10105E20 "T_StaffRoll_57" in 026E1330
str 10105E30 "T_StaffRoll_58" in 026E1330
str 10105E40 "T_StaffRoll_59" in 026E1330
str 10105E50 "T_StaffRoll_60" in 026E1330
str 10105E60 "T_StaffRoll_61" in 026E1330
str 10105E70 "T_StaffRoll_62" in 026E1330
str 10105E80 "T_StaffRoll_63" in 026E1330
str 10105E90 "T_StaffRoll_64" in 026E1330
str 10105EA0 "T_StaffRoll_65" in 026E1330
str 10105EB0 "T_StaffRoll_66" in 026E1330
str 10105EC0 "T_StaffRoll_67" in 026E1330
str 10105ED0 "T_StaffRoll_68" in 026E1330
str 10105EE0 "T_StaffRoll_69" in 026E1330
str 10105EF0 "T_StaffRoll_70" in 026E1330
str 10105F00 "T_StaffRoll_71" in 026E1330
str 10105F10 "T_StaffRoll_72" in 026E1330
str 10105F20 "T_StaffRoll_73" in 026E1330
str 10105F30 "T_StaffRoll_74" in 026E1330
str 10105F40 "T_StaffRoll_75" in 026E1330
str 10105F50 "T_StaffRoll_76" in 026E1330
str 10105F60 "T_StaffRoll_77" in 026E1330
str 10105F70 "T_StaffRoll_78" in 026E1330
str 10105F80 "T_StaffRoll_79" in 026E1330
str 10105F90 "T_StaffRoll_80" in 026E1330
str 10105FA0 "T_StaffRoll_81" in 026E1330
str 10105FB0 "T_StaffRoll_82" in 026E1330
str 10105FC0 "T_StaffRoll_83" in 026E1330
str 10105FD0 "T_StaffRoll_84" in 026E1330
str 10105FE0 "T_StaffRoll_85" in 026E1330
str 10105FF0 "T_StaffRoll_86" in 026E1330
str 10106000 "T_StaffRoll_87" in 026E1330
str 10106010 "T_StaffRoll_88" in 026E1330
str 10106020 "T_StaffRoll_89" in 026E1330
str 10106030 "T_StaffRoll_90" in 026E1330
str 10106040 "T_StaffRoll_91" in 026E1330
str 10106050 "T_StaffRoll_92" in 026E1330
str 10106060 "T_StaffRoll_93" in 026E1330
str 10106070 "T_StaffRoll_94" in 026E1330
str 10106080 "T_StaffRoll_95" in 026E1330
str 10106090 "T_StaffRoll_96" in 026E1330
str 101060A0 "T_StaffRoll_97" in 026E1330
str 101060B0 "T_StaffRoll_98" in 026E1330
str 101060C0 "T_StaffRoll_99" in 026E1330
str 101054EC "T_StaffRoll_100" in 026E1330
str 101054FC "T_StaffRoll_101" in 026E1330
str 1010550C "T_StaffRoll_102" in 026E1330
str 1010551C "T_StaffRoll_103" in 026E1330
str 1010552C "T_StaffRoll_104" in 026E1330
str 1010553C "T_StaffRoll_105" in 026E1330
str 1010554C "T_StaffRoll_106" in 026E1330
str 1010555C "T_StaffRoll_107" in 026E1330
str 1010556C "T_StaffRoll_108" in 026E1330
str 1010557C "T_StaffRoll_109" in 026E1330
str 1010558C "T_StaffRoll_110" in 026E1330
str 1010559C "T_StaffRoll_111" in 026E1330
str 101055AC "T_StaffRoll_112" in 026E1330
str 101055BC "T_StaffRoll_113" in 026E1330
str 101055CC "T_StaffRoll_114" in 026E1330
str 101055DC "T_StaffRoll_115" in 026E1330
str 101055EC "T_StaffRoll_116" in 026E1330
str 101055FC "T_StaffRoll_117" in 026E1330
str 1010560C "T_StaffRoll_118" in 026E1330
str 1010561C "T_StaffRoll_119" in 026E1330
str 1010562C "T_StaffRoll_120" in 026E1330
str 1010563C "T_StaffRoll_121" in 026E1330
str 1010564C "T_StaffRoll_122" in 026E1330
str 1010565C "T_StaffRoll_123" in 026E1330
str 1010566C "T_StaffRoll_124" in 026E1330
str 1010567C "T_StaffRoll_125" in 026E1330
str 1010568C "T_StaffRoll_126" in 026E1330
str 1010569C "T_StaffRoll_127" in 026E1330
str 101056AC "T_StaffRoll_128" in 026E1330
str 101056BC "T_StaffRoll_129" in 026E1330
str 101056CC "T_StaffRoll_130" in 026E1330
str 101056DC "T_StaffRoll_131" in 026E1330
str 101056EC "T_StaffRoll_132" in 026E1330
str 101056FC "T_StaffRoll_133" in 026E1330
str 1010570C "T_StaffRoll_134" in 026E1330
str 1010571C "T_StaffRoll_135" in 026E1330
str 1010572C "T_StaffRoll_136" in 026E1330
str 1010573C "T_StaffRoll_137" in 026E1330
str 1010574C "T_StaffRoll_138" in 026E1330
str 1010575C "T_StaffRoll_139" in 026E1330
str 1010576C "T_StaffRoll_140" in 026E1330
str 1010577C "T_StaffRoll_141" in 026E1330
str 1010578C "T_StaffRoll_142" in 026E1330
str 1010579C "T_StaffRoll_143" in 026E1330
str 101057AC "T_StaffRoll_144" in 026E1330
str 101057BC "T_StaffRoll_145" in 026E1330
str 101057CC "T_StaffRoll_146" in 026E1330
str 101057DC "T_StaffRoll_147" in 026E1330
str 101057EC "T_StaffRoll_148" in 026E1330
str 101057FC "T_StaffRoll_149" in 026E1330
str 1010580C "T_StaffRoll_150" in 026E1330
str 1010581C "T_StaffRoll_151" in 026E1330
str 1010582C "T_StaffRoll_152" in 026E1330
str 1010583C "T_StaffRoll_153" in 026E1330
str 1010584C "T_StaffRoll_154" in 026E1330
str 1010585C "T_StaffRoll_155" in 026E1330
str 1010586C "T_StaffRoll_156" in 026E1330
str 1010587C "T_StaffRoll_157" in 026E1330
str 1010588C "T_StaffRoll_158" in 026E1330
str 1010589C "T_StaffRoll_159" in 026E1330
str 101058AC "T_StaffRoll_160" in 026E1330
str 101058BC "T_StaffRoll_161" in 026E1330
str 101058CC "T_StaffRoll_162" in 026E1330
str 101058DC "T_StaffRoll_163" in 026E1330
str 101058EC "T_StaffRoll_164" in 026E1330
str 101058FC "T_StaffRoll_165" in 026E1330
str 1010590C "T_StaffRoll_166" in 026E1330
str 1010591C "T_StaffRoll_167" in 026E1330
str 1010592C "T_StaffRoll_168" in 026E1330
str 1010593C "T_StaffRoll_169" in 026E1330
str 1010594C "T_StaffRoll_170" in 026E1330
str 1010595C "T_StaffRoll_171" in 026E1330
str 1010596C "T_StaffRoll_172" in 026E1330
str 1010597C "T_StaffRoll_173" in 026E1330
str 1010598C "T_StaffRoll_174" in 026E1330
str 1010599C "T_StaffRoll_175" in 026E1330
str 101059AC "T_StaffRoll_176" in 026E1330
str 101059BC "T_StaffRoll_177" in 026E1330
str 101059CC "T_StaffRoll_178" in 026E1330
str 101059DC "T_StaffRoll_179" in 026E1330
str 101059EC "T_StaffRoll_180" in 026E1330
str 101059FC "T_StaffRoll_181" in 026E1330
str 10105A0C "T_StaffRoll_182" in 026E1330
str 10105A1C "T_StaffRoll_183" in 026E1330
str 10105A2C "T_StaffRoll_184" in 026E1330
str 10105A3C "T_StaffRoll_185" in 026E1330
str 10105A4C "T_StaffRoll_186" in 026E1330
str 101060D0 "T_AllRights_00" in 026E1330
ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c* i_this)
ext 026F88F0 void hdParts_c::destroy(hdParts_c* i_this, s32 flags)
ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString* names, u32 count)
ext 026F91F8 void hdParts_c::detachArchives()
ext 026F92C0 u32 hdParts_c::createChild(u32 index, hdPartsDesc_c* desc)
ext 02704AFC void hdTextPane_c::setCharacterCapacity(u32 count)
ext 027045B0 void hdTextPane_c::resetMessage(hdUiSafeString* message, hdUiSafeString* label)
ext 02876428 hdStaffRollColors_c* hdStaffRoll_getTextColors(void*)
ext 025F65F4 void hdStaffRoll_constructMessage(hdStaffRollMsg_c*)
ext 025F6D70 void hdStaffRoll_setMessageLabel(hdStaffRollMsg_c*, hdUiSafeString*, hdUiSafeString*)
ext 025F50E8 void hdStaffRoll_buildMessage(void*, hdStaffRollMsg_c*, void*, void*)
*/

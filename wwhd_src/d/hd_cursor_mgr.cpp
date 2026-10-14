/**
 * hd_cursor_mgr.cpp
 * HD - the menu cursor manager: owns, runs and draws the focus cursor and the fixed cursor
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_menu_cursor). Each function is marked with its WWHD address; the glue block at the end lists
 * this translation unit's data (tools/verify/autoglue.py). All names are ours. Local game-derived
 *
 */

#include "d/hd_cursor_mgr.h"
static u32 sCursorMgrHeaderStatics;
static u32 sCursorMgrHeaderDescriptors;
static u32 sCursorMgrHeaderZero;
static u32 sCursorMgrPiConstants;

static hdCursorMgrVtbl_c sCursorMgrVtable;
static hdCursorMgrDisposerVtbl_c sCursorMgrDisposerVtable;
static hdCursorMgr_c* sCursorMgrInstance;
static hdCursorMgr_c* sCursorMgrDisposer;
static hdScreenUiSystem* sCursorMgrUiSystem;
static hdCursorSize_c sCursorMgrUnitScale; /* set to 1 by the static initialiser, not read here */

/* 0265347C */
// hdCursorMgr_c::hdCursorMgr_c() (GHS: allocates when called with NULL)
hdCursorMgr_c* hdCursorMgr_c::construct(hdCursorMgr_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdCursorMgr_c*)operator_new(sizeof(hdCursorMgr_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    i_this->mMgrVtable = &sCursorMgrVtable;
    // HD: GHS tests the address of the embedded cursor array before constructing it
    if (gabi::ea(i_this->mpCursors) == 0) {
        operator_new(sizeof(i_this->mpCursors));
    }
    i_this->mActive = CURSOR_MGR_FOCUS;
    for (int i = 0; i < 2; i++) {
        i_this->mpCursors[i] = NULL;
    }
    return i_this;
}

/* 026534F4 */
// the singleton, allocated on `heap` with a disposer that clears it when the heap goes away
void hdCursorMgr_c::createInstance(void* heap) {
    if (sCursorMgrInstance != NULL) {
        return;
    }
    hdCursorMgr_c* mgr = (hdCursorMgr_c*)hdCursorMgr_new(sizeof(hdCursorMgr_c), heap, 4);
    if (mgr != NULL) {
        hdCursorMgr_constructDisposer(mgr, heap, 3);
        mgr->mVtable = &sCursorMgrDisposerVtable;
    }
    sCursorMgrDisposer = mgr;
    if (mgr != NULL) {
        mgr = construct(mgr);
    }
    sCursorMgrInstance = mgr;
}

/* 02653588 */
void hdCursorMgr_c::deleteInstance() {
    hdCursorMgr_c* disposer = sCursorMgrDisposer;
    if (disposer == NULL) {
        return;
    }
    sCursorMgrDisposer = NULL;
    disposer->hdCursorMgrDisposer_c::destroy(2);
    hdCursorMgr_c* instance = sCursorMgrInstance;
    if (instance != NULL) {
        instance->destroy(3);
    }
    sCursorMgrInstance = NULL;
}

/* 0265361C */
u32 hdCursorMgr_c::create(u32 arg1, u32 arg2) {
    hdScreenHeap* heap = sCursorMgrUiSystem->mHeap;
    hdScreenCursor_c* cursor = (hdScreenCursor_c*)hdParts_new(sizeof(hdScreenCursor_c), heap, 4);
    if (cursor != NULL) {
        cursor = hdScreenCursor_c::construct(cursor);
    }
    mpCursors[CURSOR_MGR_FOCUS] = cursor;
    if (!cursor->init(arg1, arg2, NULL)) {
        return FALSE;
    }
    hdScreenFixedCursor_c* fixed = (hdScreenFixedCursor_c*)hdParts_new(sizeof(hdScreenFixedCursor_c), heap, 4);
    if (fixed != NULL) {
        fixed = hdScreenFixedCursor_c::construct(fixed);
    }
    mpCursors[CURSOR_MGR_FIXED] = fixed;
    if (!fixed->init(arg1, arg2, NULL)) {
        return FALSE;
    }
    return TRUE;
}

/* 0265370C */
void hdCursorMgr_c::destroyCursors() {
    for (u32 i = 0; i < 2; i++) {
        if (*cursorSlot(i) == NULL) {
            continue;
        }
        (*cursorSlot(i))->vFinalize();
        hdParts_c* cursor = *cursorSlot(i);
        if (cursor != NULL) {
            cursor->vDelete(3);
        }
    }
    for (int i = 0; i < 2; i++) {
        mpCursors[i] = NULL;
    }
}

/* 0265381C */
void hdCursorMgr_c::execute() {
    if (mpCursors[CURSOR_MGR_FOCUS] != NULL) {
        mpCursors[CURSOR_MGR_FOCUS]->vExecuteState();
    }
    if (mActive == CURSOR_MGR_FIXED && mpCursors[CURSOR_MGR_FIXED] != NULL) {
        mpCursors[CURSOR_MGR_FIXED]->vExecuteState();
    }
}

/* 026538A0 */
void hdCursorMgr_c::draw(void* info) {
    hdParts_c* cursor = *cursorSlot(mActive);
    if (cursor != NULL) {
        cursor->vDraw(info);
    }
}

/* 026538F4 */
void hdCursorMgr_c::setFrameScale(hdCursorSize_c* scale) {
    hdScreenCursor_c::setFrameScale(scale);
}

/* 026538FC */
void hdCursorMgr_c::setFixedPosition(hdCursorSize_c* position) {
    hdScreenFixedCursor_c::setPosition(position);
}

/* 02653904 */
void hdCursorMgr_c::setFixedSize(hdCursorSize_c* size) {
    hdScreenFixedCursor_c::setSize(size);
}

/* 0265390C */
// the singleton disposer's destructor: a disposed heap takes the manager with it
void hdCursorMgr_c::destroyDisposer(hdCursorMgr_c* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mVtable = &sCursorMgrDisposerVtable;
    if (i_this == sCursorMgrDisposer) {
        hdCursorMgr_c* instance = sCursorMgrInstance;
        sCursorMgrDisposer = NULL;
        instance->destroy(2);
        sCursorMgrInstance = NULL;
    }
    hdCursorMgr_destroyDisposerBase(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 026539B4 */
static void __sinit_hd_cursor_mgr_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sCursorMgrHeaderStatics), gabi::ea(&sCursorMgrHeaderDescriptors), gabi::ea(&sCursorMgrHeaderZero),
                                gabi::ea(&sCursorMgrPiConstants));
    sCursorMgrUnitScale.x = 1.0f;
    sCursorMgrUnitScale.y = 1.0f;
}

/* WWHD glue
data sCursorMgrHeaderStatics 104920D4 nocheck
data sCursorMgrHeaderDescriptors 101F5A20 nocheck
data sCursorMgrHeaderZero 104920E0 nocheck
data sCursorMgrPiConstants 100EFD50 nocheck
ext 0273B0D4 void* hdCursorMgr_new(u32 size, void* heap, u32 alignment)
ext 02752B0C void hdCursorMgr_constructDisposer(hdCursorMgrDisposer_c* disposer, void* heap, u32 option)
ext 02752BEC void hdCursorMgr_destroyDisposerBase(hdCursorMgrDisposer_c* disposer, s32 flags)
data sCursorMgrVtable 100EFD68 nocheck
data sCursorMgrDisposerVtable 100EFD58 nocheck
data sCursorMgrInstance 101F5A44 nocheck
data sCursorMgrDisposer 101F5A48 nocheck
data sCursorMgrUiSystem 1018C404 nocheck
data sCursorMgrUnitScale 104920F0 nocheck
*/

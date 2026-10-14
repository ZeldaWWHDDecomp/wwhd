/**
 * hd_parts_lyt.cpp
 * HD - the NW4F layout of a UI parts screen: game text boxes and parts panes, nested parts layouts,
 * text-box setup and the animation walk over nested layouts
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_fw_parts_loop_display). Each function is marked with its WWHD address; the glue block at the
 * end lists this translation unit's data (tools/verify/autoglue.py). All class, member and function
 * names are ours (no HD string names this code).
 */

#include "d/hd_parts_lyt.h"
#include "d/hd_tu_header_statics.h"

static hdPartsLytVtbl_c sLytVtable;

/* lazily initialised runtime types (function-local statics of the inlined type queries) */
static u32 sLytTypeReady;
static hdPartsTypeInfo_c sLytType;
static u32 sLayoutTypeReady;
static hdPartsTypeInfo_c sLayoutType;
static u32 sLayoutBaseTypeReady;
static hdPartsTypeInfo_c sLayoutBaseType;
static u32 sTextBoxTypeReady;
static hdPartsTypeInfo_c sTextBoxType;
static u32 sTextBoxBaseTypeReady;
static hdPartsTypeInfo_c sTextBoxBaseType;
static u32 sPaneTypeReady;
static hdPartsTypeInfo_c sPaneType;
static u32 sRootTypeReady;
static hdPartsTypeInfo_c sRootType;
static u32 sPartsTypeReady;
static hdPartsTypeInfo_c sPartsType;
static u32 sPartsBaseTypeReady;
static hdPartsTypeInfo_c sPartsBaseType;
static u32 sPartsParentTypeReady;
static hdPartsTypeInfo_c sPartsParentType;

/* inlined into buildPartsLayout: the runtime type of hdPartsLyt_c */
static hdPartsTypeInfo_c* hdPartsLyt_getTypeInfo() {
    if (!sLytTypeReady) {
        sLytTypeReady = 1;
        if (!sLayoutTypeReady) {
            sLayoutTypeReady = 1;
            if (!sLayoutBaseTypeReady) {
                sLayoutBaseTypeReady = 1;
                sLayoutBaseType.mParent = NULL;
            }
            sLayoutType.mParent = &sLayoutBaseType;
        }
        sLytType.mParent = &sLayoutType;
    }
    return &sLytType;
}

/* inlined into initTextBoxes: the runtime type of the game's text box */
static hdPartsTypeInfo_c* hdPartsLyt_getTextBoxTypeInfo() {
    if (!sTextBoxTypeReady) {
        sTextBoxTypeReady = 1;
        if (!sTextBoxBaseTypeReady) {
            sTextBoxBaseTypeReady = 1;
            if (!sPaneTypeReady) {
                sPaneTypeReady = 1;
                if (!sRootTypeReady) {
                    sRootTypeReady = 1;
                    sRootType.mParent = NULL;
                }
                sPaneType.mParent = &sRootType;
            }
            sTextBoxBaseType.mParent = &sPaneType;
        }
        sTextBoxType.mParent = &sTextBoxBaseType;
    }
    return &sTextBoxType;
}

/* inlined into animate and finishAnimate: the runtime type of the game's parts pane */
static hdPartsTypeInfo_c* hdPartsLyt_getPartsTypeInfo() {
    if (!sPartsTypeReady) {
        sPartsTypeReady = 1;
        if (!sPartsBaseTypeReady) {
            sPartsBaseTypeReady = 1;
            if (!sPartsParentTypeReady) {
                sPartsParentTypeReady = 1;
                if (!sRootTypeReady) {
                    sRootType.mParent = NULL;
                    sRootTypeReady = 1;
                }
                sPartsParentType.mParent = &sRootType;
            }
            sPartsBaseType.mParent = &sPartsParentType;
        }
        sPartsType.mParent = &sPartsBaseType;
    }
    return &sPartsType;
}

/* inlined: the type test of a dynamic cast */
static bool hdPartsLyt_isDerivedFrom(hdPartsTypeInfo_c* type, hdPartsTypeInfo_c* base) {
    for (; type != NULL; type = type->mParent) {
        if (type == base) {
            return true;
        }
    }
    return false;
}

/* inlined into buildPaneObj: the pane constructors take a copy of the argument block */
static void hdPartsLyt_copyArgs(hdPartsBuildArgs_c* dst, hdPartsBuildArgs_c* src) {
    for (int i = 0; i < 13; i++) {
        dst->mWords[i] = src->mWords[i];
    }
}

/* 026FA324 */
// hdPartsLyt_c::hdPartsLyt_c(owner, accessor) (GHS: allocates when called with NULL): a nested parts layout
hdPartsLyt_c* hdPartsLyt_c::construct(hdPartsLyt_c* i_this, hdPartsLyt_c* owner, hdPartsLyt_c* accessor) {
    if (i_this == NULL) {
        i_this = (hdPartsLyt_c*)operator_new(sizeof(hdPartsLyt_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdPartsLyt_constructBase(i_this, owner, accessor);
    i_this->mVtable = &sLytVtable;
    return i_this;
}

/* 026FA398 */
// hdPartsLyt_c::hdPartsLyt_c() (GHS: allocates when called with NULL): a screen's root layout
hdPartsLyt_c* hdPartsLyt_c::constructRoot(hdPartsLyt_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdPartsLyt_c*)operator_new(sizeof(hdPartsLyt_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    hdPartsLyt_constructBase(i_this);
    i_this->mVtable = &sLytVtable;
    return i_this;
}

/* 026FA3EC */
// parts panes and text boxes get the game's classes; a text box another layout already owns is
// linked instead of built; every other pane kind goes to the base layout
void* hdPartsLyt_c::buildPaneObj(u32 kind, hdPartsPaneRes_c* resource, void* context, hdPartsBuildArgs_c* args) {
    switch (kind) {
    case PANE_KIND_PARTS: {
        gabi::Local<hdPartsBuildArgs_c> copy;
        hdPartsLyt_copyArgs(copy, args);
        void* pane = hdPartsLyt_allocMemory(sizeof(hdPartsPartsPane_c), 4);
        if (pane == NULL) {
            return NULL;
        }
        return hdPartsLyt_constructPartsPane(pane, resource, context, copy);
    }
    case PANE_KIND_TEXT: {
        if (hdPartsLyt_isTextBoxSkipped(resource)) {
            return NULL;
        }
        void* shared = hdPartsLyt_findSharedTextBox(resource, this);
        if (shared != NULL) {
            hdPartsLyt_linkSharedTextBox(shared, resource->mName);
            return NULL;
        }
        gabi::Local<hdPartsBuildArgs_c> copy;
        hdPartsLyt_copyArgs(copy, args);
        void* pane = hdPartsLyt_allocMemory(0xB68, 4);
        if (pane == NULL) {
            hdPartsLyt_setupTextBox(NULL, resource, context, args);
            return NULL;
        }
        void* textBox = hdPartsLyt_constructTextBox(pane, resource, context, copy);
        hdPartsLyt_setupTextBox(textBox, resource, context, args);
        return textBox;
    }
    default:
        return hdPartsLyt_buildPaneObjBase(this, kind, resource, context, args);
    }
}

/* 026FA624 */
// a parts pane's layout: nested in this one; it shares this layout's resource accessor when that is a
// parts layout too
hdPartsLyt_c* hdPartsLyt_c::buildPartsLayout(void* resource, void* name, void* options) {
    void* data = hdPartsLyt_getPartsResource(this, resource, name, options);
    hdPartsLyt_c* accessor = mpResourceAccessor;
    hdPartsTypeInfo_c* lytType = hdPartsLyt_getTypeInfo();
    if (accessor == NULL || !hdPartsLyt_isDerivedFrom(accessor->getRuntimeTypeInfo(), lytType)) {
        accessor = NULL;
    }
    hdPartsLyt_c* child = (hdPartsLyt_c*)hdPartsLyt_allocMemory(sizeof(hdPartsLyt_c), 4);
    if (child != NULL) {
        child = construct(child, this, accessor);
    }
    // HD: the build call goes through the vtable read at child+0x30 even when allocation failed
    hdPartsLytVtbl_c* vtable = vtableOf(child);
    void* context = mpBuildContext;
    gabi::call_ptr(vtable->mBuild.get(), child, data, context, options, name);
    return child;
}

/* 026FA7BC */
// the game's text boxes in this pane's subtree get their per-layout setup
void hdPartsLyt_c::initTextBoxes(hdPartsPane_c* pane) {
    hdPartsTypeInfo_c* textBoxType = hdPartsLyt_getTextBoxTypeInfo();
    if (pane != NULL && hdPartsLyt_isDerivedFrom(pane->getRuntimeTypeInfo(), textBoxType)) {
        hdPartsLyt_initTextBox(pane);
    }
    // HD: the child list is read through the pane even when it is NULL
    hdPartsListNode_c* end = &hdPartsView(pane)->mChildren;
    for (hdPartsListNode_c* node = end->mpNext; gabi::ea(node) != gabi::ea(end); node = hdPartsView(node)->mpNext) {
        initTextBoxes((hdPartsPane_c*)node);
    }
}

/* 026FA920 */
void hdPartsLyt_c::initAllTextBoxes() {
    hdPartsPane_c* root = mpRootPane;
    if (hdPartsView(root)->mpParent == NULL) {
        initTextBoxes(root);
    }
}

/* 026FA934 */
// animate the transforms, then every nested parts layout that was animated before
void hdPartsLyt_c::animate() {
    for (hdPartsListNode_c* node = mAnims.mpNext; gabi::ea(node) != gabi::ea(&mAnims); node = hdPartsView(node)->mpNext) {
        hdPartsView((hdPartsAnim_c*)node)->animate();
    }
    for (hdPartsListNode_c* node = mPartsPanes.mpNext; gabi::ea(node) != gabi::ea(&mPartsPanes); node = hdPartsView(node)->mpNext) {
        hdPartsPartsPane_c* parts = hdPartsPartsPaneOf(node);
        hdPartsTypeInfo_c* partsType = hdPartsLyt_getPartsTypeInfo();
        if (gabi::ea(parts) == 0 || !hdPartsLyt_isDerivedFrom(parts->mPane.getRuntimeTypeInfo(), partsType)) {
            // HD: a failed cast goes on with a NULL pane (guest page 0)
            parts = hdPartsView<hdPartsPartsPane_c>(0u);
        }
        if (parts->mAnimated) {
            hdPartsLyt_c* layout = parts->mpLayout;
            layout->vAnimate();
        }
        parts->mAnimated = TRUE;
    }
}

/* 026FAABC */
// the same walk for the frame update after drawing
void hdPartsLyt_c::finishAnimate() {
    for (hdPartsListNode_c* node = mAnims.mpNext; gabi::ea(node) != gabi::ea(&mAnims); node = hdPartsView(node)->mpNext) {
        hdPartsView((hdPartsAnim_c*)node)->updateFrame();
    }
    for (hdPartsListNode_c* node = mPartsPanes.mpNext; gabi::ea(node) != gabi::ea(&mPartsPanes); node = hdPartsView(node)->mpNext) {
        hdPartsPartsPane_c* parts = hdPartsPartsPaneOf(node);
        hdPartsTypeInfo_c* partsType = hdPartsLyt_getPartsTypeInfo();
        if (gabi::ea(parts) == 0 || !hdPartsLyt_isDerivedFrom(parts->mPane.getRuntimeTypeInfo(), partsType)) {
            // HD: a failed cast goes on with a NULL pane (guest page 0)
            parts = hdPartsView<hdPartsPartsPane_c>(0u);
        }
        if (parts->mFinished) {
            hdPartsLyt_c* layout = parts->mpLayout;
            layout->vFinishAnimate();
        }
        parts->mFinished = TRUE;
    }
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

static hdTuHeaderStatics_c sPartsLytHeaderStatics;
static hdTuHeaderDescriptors_c sPartsLytHeaderDescriptors;
static hdTuHeaderZero_c sPartsLytHeaderZero;
static hdTuPiPair_c sPartsLytPiConstants;
/* 026FAC44 */
static void __sinit_hd_parts_lyt_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sPartsLytHeaderStatics), gabi::ea(&sPartsLytHeaderDescriptors), gabi::ea(&sPartsLytHeaderZero),
                                gabi::ea(&sPartsLytPiConstants));
}

/* WWHD glue
data sPartsLytHeaderStatics 1049DDC0
data sPartsLytHeaderDescriptors 101F7208
data sPartsLytHeaderZero 1049DDCC
data sPartsLytPiConstants 1010A664
data sLytVtable 1010A670 nocheck
data sLytTypeReady 101FDD9C nocheck
data sLytType 101FDDA8 nocheck
data sLayoutTypeReady 101FD8C4 nocheck
data sLayoutType 101FD900 nocheck
data sLayoutBaseTypeReady 101FD894 nocheck
data sLayoutBaseType 101FD904 nocheck
data sTextBoxTypeReady 101FDD40 nocheck
data sTextBoxType 101FDD3C nocheck
data sTextBoxBaseTypeReady 101FD8F0 nocheck
data sTextBoxBaseType 101FD8F4 nocheck
data sPaneTypeReady 101FD89C nocheck
data sPaneType 101FD8F8 nocheck
data sRootTypeReady 101FD888 nocheck
data sRootType 101FD8FC nocheck
data sPartsTypeReady 101FDDA0 nocheck
data sPartsType 101FDDA4 nocheck
data sPartsBaseTypeReady 101FD914 nocheck
data sPartsBaseType 101FD918 nocheck
data sPartsParentTypeReady 101FD890 nocheck
data sPartsParentType 101FD91C nocheck
*/

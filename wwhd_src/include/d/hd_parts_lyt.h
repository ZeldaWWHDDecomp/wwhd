/* HD: the NW4F layout of a UI parts screen (hd_parts_lyt.cpp): builds text boxes and parts panes
 * with the game's own pane classes, nests parts layouts and walks animations, text boxes and nested
 * layouts. No GameCube counterpart.
 *
 * Names: hdPartsLyt_c, hdPartsDesc_c and every member, method and static name here are ours
 * (descriptive, no HD string names them); "probably" marks a role read from its use only. The class
 * derives from the shared layout view hdLayout_c (hd/hd_ui_02003C58.h, base constructors 02003CDC /
 * 02003D84); this header keeps its own view because it needs fields that view leaves opaque
 * (+0x04, +0x24, +0x28). */
#pragma once
#include "bindings.h"
#include "hd/hd_ui_020021FC.h"

struct hdPartsLyt_c;

/* HD: list walks and failed dynamic casts dereference whatever pointer they hold, NULL included
 * (they then read guest page 0): a view of the guest object at `address` that is never a host NULL */
template <class T> inline T* hdPartsView(u32 address) { return reinterpret_cast<T*>(PPC_MEM_BASE + address); }
template <class T> inline T* hdPartsView(T* object) { return hdPartsView<T>(gabi::ea(object)); }

/* intrusive list node (next at +0, previous at +4) */
struct hdPartsListNode_c {
    /* 0x00 */ gptr<hdPartsListNode_c> mpNext;
    /* 0x04 */ gptr<hdPartsListNode_c> mpPrev;
};
WWHD_SIZE(hdPartsListNode_c, 8);
WWHD_OFFSET(hdPartsListNode_c, mpPrev, 4);

/* runtime type of a pane or layout (parent link only; hdScreenType in hd/hd_ui_020021FC.h) */
typedef hdScreenType hdPartsTypeInfo_c;

/* the parts pane whose layout-list node is `link` */
struct hdPartsPartsPane_c;
inline hdPartsPartsPane_c* hdPartsPartsPaneOf(hdPartsListNode_c* link);

/* a layout pane: list node, vtable at +8 (slot +0x14: runtime type), parent, child list */
struct hdPartsPaneVtbl_c {
    /* 0x00 */ u8 _00[0x14];
    /* 0x14 */ gfn mGetRuntimeTypeInfo;
};
WWHD_OFFSET(hdPartsPaneVtbl_c, mGetRuntimeTypeInfo, 0x14);
struct hdPartsPane_c {
    /* 0x00 */ hdPartsListNode_c mLink;     /* node in the parent's child list */
    /* 0x08 */ gptr<hdPartsPaneVtbl_c> mVtable;
    /* 0x0C */ gptr<hdPartsPane_c> mpParent;
    /* 0x10 */ u8 _10[4];
    /* 0x14 */ hdPartsListNode_c mChildren; /* list head */
    hdPartsTypeInfo_c* getRuntimeTypeInfo() { return gabi::call_ptr<hdPartsTypeInfo_c*>(mVtable->mGetRuntimeTypeInfo.get(), this); }
};
WWHD_OFFSET(hdPartsPane_c, mVtable, 8);
WWHD_OFFSET(hdPartsPane_c, mpParent, 0xC);
WWHD_OFFSET(hdPartsPane_c, mChildren, 0x14);
WWHD_SIZE(hdPartsPane_c, 0x1C);
/* a pane with its transform, scale, flags and alpha (the fields after the 0x1C base view; PICTO-best_shot_mark+
 * btn_pictograph). Translation and rotation as raw words: copies keep signalling-NaN bit patterns */
struct hdPartsPaneSRTVtbl_c : hdPartsPaneVtbl_c {
    /* 0x18 */ u8 _18[0x4C - 0x18];
    /* 0x4C */ gfn mGetMaterial;
    /* 0x50 */ u8 _50[0x5C - 0x50];
    /* 0x5C */ gfn mFindPane;           /* (name, recursive) */
};
WWHD_OFFSET(hdPartsPaneSRTVtbl_c, mGetMaterial, 0x4C);
WWHD_OFFSET(hdPartsPaneSRTVtbl_c, mFindPane, 0x5C);
struct hdPartsPaneSRT_c : hdPartsPane_c {
    /* 0x1C */ be<u32> mTranslationXBits, mTranslationYBits, mTranslationZBits;
    /* 0x28 */ be<u32> mRotationXBits, mRotationYBits, mRotationZBits;
    /* 0x34 */ be<f32> mScaleX, mScaleY;
    /* 0x3C */ be<f32> mSizeX, mSizeY;   /* the pane's width and height (MIIV-menu_miiverse#2: +0x40 read as height) */
    /* 0x44 */ be<u8> mFlags, mAlpha;
    hdPartsPaneSRTVtbl_c* srtVtbl() { return gabi::at<hdPartsPaneSRTVtbl_c>(mVtable.v.get()); }
    hdPartsPaneSRT_c* find(const char* name) { return gabi::call_ptr<hdPartsPaneSRT_c*>(srtVtbl()->mFindPane.get(), this, name, 1); }
};
WWHD_OFFSET(hdPartsPaneSRT_c, mTranslationXBits, 0x1C);
WWHD_OFFSET(hdPartsPaneSRT_c, mRotationXBits, 0x28);
WWHD_OFFSET(hdPartsPaneSRT_c, mScaleX, 0x34);
WWHD_OFFSET(hdPartsPaneSRT_c, mSizeY, 0x40);
WWHD_OFFSET(hdPartsPaneSRT_c, mFlags, 0x44);
WWHD_OFFSET(hdPartsPaneSRT_c, mAlpha, 0x45);

/* a parts pane (0xB4, constructed by 027028A8): the nested layout it shows and two update flags
 * (set once its layout was animated / finished; our reading) */
struct hdPartsPartsPane_c {
    /* 0x00 */ hdPartsPane_c mPane;
    /* 0x1C */ u8 _1C[0xA4 - 0x1C];
    /* 0xA4 */ hdPartsListNode_c mLayoutLink; /* node in the owning layout's parts-pane list */
    /* 0xAC */ gptr<hdPartsLyt_c> mpLayout;
    /* 0xB0 */ be<u8> mAnimated;
    /* 0xB1 */ be<u8> mFinished;
    /* 0xB2 */ u8 _B2[2];
};
WWHD_OFFSET(hdPartsPartsPane_c, mLayoutLink, 0xA4);
WWHD_OFFSET(hdPartsPartsPane_c, mpLayout, 0xAC);
WWHD_OFFSET(hdPartsPartsPane_c, mAnimated, 0xB0);
WWHD_OFFSET(hdPartsPartsPane_c, mFinished, 0xB1);
WWHD_SIZE(hdPartsPartsPane_c, 0xB4);
inline hdPartsPartsPane_c* hdPartsPartsPaneOf(hdPartsListNode_c* link) {
    return hdPartsView<hdPartsPartsPane_c>(gabi::ea(link) - offsetof(hdPartsPartsPane_c, mLayoutLink));
}

/* an animation transform of the layout: list node at +0, vtable at +0x14 */
struct hdPartsAnimVtbl_c {
    /* 0x00 */ u8 _00[0x1C];
    /* 0x1C */ gfn mUpdateFrame; /* probably */
    /* 0x20 */ u8 _20[0xC];
    /* 0x2C */ gfn mAnimate;     /* probably */
};
WWHD_OFFSET(hdPartsAnimVtbl_c, mUpdateFrame, 0x1C);
WWHD_OFFSET(hdPartsAnimVtbl_c, mAnimate, 0x2C);
struct hdPartsAnim_c {
    /* 0x00 */ hdPartsListNode_c mLink;
    /* 0x08 */ u8 _08[0xC];
    /* 0x14 */ gptr<hdPartsAnimVtbl_c> mVtable;
    void animate() { gabi::call_ptr(mVtable->mAnimate.get(), this); }
    void updateFrame() { gabi::call_ptr(mVtable->mUpdateFrame.get(), this); }
};
WWHD_OFFSET(hdPartsAnim_c, mVtable, 0x14);
WWHD_SIZE(hdPartsAnim_c, 0x18);

/* a parts descriptor the layout lists (visited by hdParts_c::createChildren): a resource string at
 * +0x08 (handed on to the part's load) and the parts class name at +0x24 */
struct hdPartsDesc_c {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ u8 mResource[0x24 - 8];
    /* 0x24 */ gptr<const char> mClassName;
};
WWHD_OFFSET(hdPartsDesc_c, mResource, 8);
WWHD_OFFSET(hdPartsDesc_c, mClassName, 0x24);
WWHD_SIZE(hdPartsDesc_c, 0x28);

/* a fixed array of descriptors (sead PtrArray: count, capacity, data) */
struct hdPartsDescList_c {
    /* 0x00 */ be<s32> mCount;
    /* 0x04 */ be<s32> mCapacity;
    /* 0x08 */ gptr<gptr<hdPartsDesc_c>> mpData;
};
WWHD_SIZE(hdPartsDescList_c, 0xC);

/* a pane resource block: its name at +0xC */
struct hdPartsPaneRes_c {
    /* 0x00 */ u8 _00[0xC];
    /* 0x0C */ be<u8> mName[0x18];
};
WWHD_OFFSET(hdPartsPaneRes_c, mName, 0xC);
WWHD_SIZE(hdPartsPaneRes_c, 0x24);

/* the 13-word argument block of a pane constructor (copied to the stack by buildPaneObj) */
struct hdPartsBuildArgs_c {
    be<u32> mWords[13];
};
WWHD_SIZE(hdPartsBuildArgs_c, 0x34);

enum hdPartsPaneKind_e {
    PANE_KIND_PARTS = 0x70727431, /* 'prt1' */
    PANE_KIND_TEXT = 0x74787431,  /* 'txt1' */
};

/* layout vtable (GHS: 8-byte entries, function word at +4) */
struct hdPartsLytVtbl_c {
    /* 0x00 */ u8 _00[0x0C];
    /* 0x0C */ gfn mGetRuntimeTypeInfo;
    /* 0x10 */ u8 _10[0x3C];
    /* 0x4C */ gfn mAnimate;
    /* 0x50 */ u8 _50[4];
    /* 0x54 */ gfn mFinishAnimate; /* probably */
    /* 0x58 */ u8 _58[0x24];
    /* 0x7C */ gfn mBuild;
};
WWHD_OFFSET(hdPartsLytVtbl_c, mGetRuntimeTypeInfo, 0xC);
WWHD_OFFSET(hdPartsLytVtbl_c, mAnimate, 0x4C);
WWHD_OFFSET(hdPartsLytVtbl_c, mFinishAnimate, 0x54);
WWHD_OFFSET(hdPartsLytVtbl_c, mBuild, 0x7C);

struct hdPartsLyt_c {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ hdPartsListNode_c mAnims;      /* animation transforms (list head) */
    /* 0x0C */ gptr<hdPartsPane_c> mpRootPane;
    /* 0x10 */ u8 _10[0xC];
    /* 0x1C */ gptr<void> mpMessageData;    /* the layout's message data (TITLE-staff_roll) */
    /* 0x20 */ gptr<void> mpBuildContext;
    /* 0x24 */ u8 _24[4];
    /* 0x28 */ hdPartsListNode_c mPartsPanes; /* parts panes (nodes at hdPartsPartsPane_c::mLayoutLink) */
    /* 0x30 */ gptr<hdPartsLytVtbl_c> mVtable;
    /* 0x34 */ gptr<hdPartsLyt_c> mpOwner;
    /* 0x38 */ gptr<hdPartsLyt_c> mpResourceAccessor;
    /* 0x3C */ hdPartsDescList_c mPartsDescs;  /* followed by its 128-entry storage */
    /* 0x48 */ u8 _48[0x248 - 0x48];

    static hdPartsLyt_c* construct(hdPartsLyt_c* i_this, hdPartsLyt_c* owner, hdPartsLyt_c* accessor);
    static hdPartsLyt_c* constructRoot(hdPartsLyt_c* i_this);
    void* buildPaneObj(u32 kind, hdPartsPaneRes_c* resource, void* context, hdPartsBuildArgs_c* args);
    hdPartsLyt_c* buildPartsLayout(void* resource, void* name, void* options);
    void initTextBoxes(hdPartsPane_c* pane);
    void initAllTextBoxes();
    void animate();
    void finishAnimate();

    /* HD: the vtable as the code reads it, through guest page 0 for a NULL layout */
    static hdPartsLytVtbl_c* vtableOf(hdPartsLyt_c* lyt) { return hdPartsView(lyt)->mVtable; }
    hdPartsTypeInfo_c* getRuntimeTypeInfo() { return gabi::call_ptr<hdPartsTypeInfo_c*>(vtableOf(this)->mGetRuntimeTypeInfo.get(), this); }
    void vAnimate() { gabi::call_ptr(vtableOf(this)->mAnimate.get(), this); }
    void vFinishAnimate() { gabi::call_ptr(vtableOf(this)->mFinishAnimate.get(), this); }
};
WWHD_OFFSET(hdPartsLyt_c, mAnims, 4);
WWHD_OFFSET(hdPartsLyt_c, mpRootPane, 0xC);
WWHD_OFFSET(hdPartsLyt_c, mpBuildContext, 0x20);
WWHD_OFFSET(hdPartsLyt_c, mpMessageData, 0x1C);
WWHD_OFFSET(hdPartsLyt_c, mPartsPanes, 0x28);
WWHD_OFFSET(hdPartsLyt_c, mVtable, 0x30);
WWHD_OFFSET(hdPartsLyt_c, mpOwner, 0x34);
WWHD_OFFSET(hdPartsLyt_c, mpResourceAccessor, 0x38);
WWHD_OFFSET(hdPartsLyt_c, mPartsDescs, 0x3C);
WWHD_SIZE(hdPartsLyt_c, 0x248);

/* ---- callees (library and other units; called by address, our names) ---- */
/* 02003CDC / 02003D84: the base layout's constructors (hd_ui_02003C58) */
inline void hdPartsLyt_constructBase(hdPartsLyt_c* lyt, hdPartsLyt_c* owner, hdPartsLyt_c* accessor) { gabi::call(0x02003CDC, lyt, owner, accessor); }
inline void hdPartsLyt_constructBase(hdPartsLyt_c* lyt) { gabi::call(0x02003D84, lyt); }
/* 02003EE8: the base layout's buildPaneObj */
inline void* hdPartsLyt_buildPaneObjBase(hdPartsLyt_c* lyt, u32 kind, void* resource, void* context, hdPartsBuildArgs_c* args) {
    return gabi::call<void*>(0x02003EE8, lyt, kind, resource, context, args);
}
/* 0287F3DC: layout memory (size, alignment) */
inline void* hdPartsLyt_allocMemory(u32 size, u32 alignment) { return gabi::call<void*>(0x0287F3DC, size, alignment); }
/* 02880AE0: the parts layout's resource (library BuildPartsLayout part) */
inline void* hdPartsLyt_getPartsResource(hdPartsLyt_c* lyt, void* resource, void* name, void* options) {
    return gabi::call<void*>(0x02880AE0, lyt, resource, name, options);
}
/* FW-parts_msg_base / FW-parts_text_valid: the game's parts pane and text box classes */
inline void* hdPartsLyt_constructPartsPane(void* pane, void* resource, void* context, hdPartsBuildArgs_c* args) {
    return gabi::call<void*>(0x027028A8, pane, resource, context, args);
}
inline void* hdPartsLyt_constructTextBox(void* pane, void* resource, void* context, hdPartsBuildArgs_c* args) {
    return gabi::call<void*>(0x027030FC, pane, resource, context, args);
}
inline void hdPartsLyt_setupTextBox(void* pane, void* resource, void* context, hdPartsBuildArgs_c* args) {
    gabi::call(0x02703538, pane, resource, context, args);
}
inline u32 hdPartsLyt_isTextBoxSkipped(void* resource) { return gabi::call<u32>(0x02705B34, resource); }
inline void* hdPartsLyt_findSharedTextBox(void* resource, hdPartsLyt_c* lyt) { return gabi::call<void*>(0x02705F44, resource, lyt); }
inline void hdPartsLyt_linkSharedTextBox(void* textBox, void* name) { gabi::call(0x02875B3C, textBox, name); }
inline void hdPartsLyt_initTextBox(hdPartsPane_c* textBox) { gabi::call(0x02704484, textBox); }

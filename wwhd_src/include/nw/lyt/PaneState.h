/* Pane storage views on the unchanged merged 0xA4 HD pane/bounding prefix. */
#pragma once
#include "hd/hd_ui_02003C58.h"
#include "nw/ut/FrameHeap.h"
#include "nw/lyt/GlyphWriter.h"
/* The method table of a pane, viewed as T (each caller names only the slots it uses). */
template <class T> inline T* paneMethods(const hdLayout_bounding_c* pane) { return gabi::at<T>(pane->mVtable); }
namespace nw { namespace lyt {
struct LayoutPane;
struct LayoutMaterial;
struct PaneTraversalContext;
struct PaneUserData { u8 prefix[8]; be<u16> count, reserved; be<u8> entries[1]; };
struct PaneContent {
    gptr<LayoutPane> parent;
    nw::ut::HeapChildren children;
    be<f32> translation[3], rotation[3], scale[2], width, height;
    be<u8> flags, alpha, inheritedAlpha, matrixFlag;
    Mtx34 matrix;
    gptr<void> userPointer;
    gptr<PaneUserData> userData;
    be<u8> name[25], userInfo[9];
    u8 padding[2];
};
struct PaneLinkPrefix {
    nw::ut::HeapLink link;
    be<u32> methods;
    static PaneLinkPrefix* create(PaneLinkPrefix* pane);
};
// The merged type supplies storage only. This view does not claim a runtime C++ base-class relation.
struct LayoutPane : hdLayout_bounding_c {
    PaneContent* content() { return reinterpret_cast<PaneContent*>(_0C); }
    const PaneContent* content() const { return reinterpret_cast<const PaneContent*>(_0C); }
    static LayoutPane* createDefault(LayoutPane* pane);
    static LayoutPane* copyFrom(LayoutPane* pane, LayoutPane* source);
    static LayoutPane* createFromResource(LayoutPane* pane, void* resource, void* context);
    void destroy(u32 flags);
    void initializeState();
    void setName(const be<u8>* name);
    void setUserInfo(const be<u8>* text);
    void setColorElement(u32 index, u32 value);
    void setVertexColorElement(u32 index, u32 value);
    void* materialAt(u32 index) const;
    void* firstMaterial();
    Pair32 originOffset() const;
    void bounds(TextBounds* output) const;
    void insertChild(gptr<nw::ut::HeapLink>* position, LayoutPane* child);
    void appendChild(LayoutPane* child);
    void updateMatrix(PaneTraversalContext* context, u32 force);
    void ancestorScale(be<f32>* output, LayoutPane* ancestor);
    LayoutPane* findPane(const be<u8>* name, u32 recursive);
    LayoutMaterial* findMaterial(const be<u8>* name, u32 recursive);
    u32 userDataCount() const;
    void* userDataEntries() const;
};
WWHD_SIZE(PaneLinkPrefix, 0xC);
WWHD_SIZE(PaneContent, 0x98);
WWHD_SIZE(LayoutPane, 0xA4);
WWHD_OFFSET(hdLayout_bounding_c, _0C, 0xC);
WWHD_OFFSET(PaneContent, children, 4);
WWHD_OFFSET(PaneContent, translation, 0x10);
WWHD_OFFSET(PaneContent, flags, 0x38);
WWHD_OFFSET(PaneContent, matrix, 0x3C);
WWHD_OFFSET(PaneContent, userData, 0x70);
WWHD_OFFSET(PaneContent, name, 0x74);
WWHD_OFFSET(PaneContent, userInfo, 0x8D);
} }
void* paneStateAllocate(u32 size);
void paneStateClearMemory(void* storage, u32 byte, u32 size);
void paneChildrenInsert(nw::ut::HeapChildren*, gptr<nw::ut::HeapLink>*, nw::ut::HeapLink*);

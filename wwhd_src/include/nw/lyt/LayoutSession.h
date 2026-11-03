/* Layout build options and part-resource indices for the canonical layout object. */
#pragma once
#include "nw/lyt/LayoutGpuResources.h"
#include "nw/lyt/LYT_animframe.h"
#include "nw/lyt/PartsPane.h"
#include "nw/lyt/BoundingPane.h"
#include "nw/lyt/GroupPlayback.h"
#include "nw/lyt/ResourceHelpers.h"
namespace nw { namespace lyt {
struct LayoutSessionMethods { u8 prefix[0x7C]; be<u32> build; u8 paneGap[4]; be<u32> createPane; u8 nestedGap[4]; be<u32> createNested; };
struct LayoutBuildListener;
struct LayoutBuildOptions {
    be<f32> scaleX,scaleY,offsetX,offsetY;
    gptr<LayoutBuildListener> listener;
    gptr<TextSearcher> textSearcher;
    be<u32> reserved;
};
struct LayoutPartIndex;
struct LayoutPartsResource;
void setPaneTagProcessor(LayoutPane* pane,WideTagProcessor* processor);
// A parts pane's link in its layout's part list: the two words after the pane (LayoutParts::stateA4/stateA8).
inline nw::ut::HeapLink* partLink(LayoutParts* parts) { return reinterpret_cast<nw::ut::HeapLink*>(&parts->stateA4); }
inline LayoutParts* partFromLink(nw::ut::HeapLink* link) {
    return layoutAtOffset<LayoutParts>(link,static_cast<u32>(-static_cast<s32>(offsetof(LayoutParts,stateA4))));
}
struct LayoutPartResource {
    be<u8> name[0x18];
    be<u8> primaryFlags,secondaryFlags; u8 padding[2];
    be<u32> resourceOffset,alternateOffset,flagsOffset;
};
struct LayoutPartIndex {
    be<u32> count;
    gptr<LayoutPartResource> entries;
    gptr<LayoutPane> parent;
    gptr<be<u8>> resource;
    gptr<void> userData;
    be<f32> scaleX,scaleY;
    static LayoutPartIndex* create(LayoutPartIndex* index,LayoutPane* parent,LayoutPartsResource* source,void* userData,const PaneSize* size);
    LayoutPartResource* find(const be<u8>* name);
    void* alternate(LayoutPartResource* entry);
    void* paneResource(LayoutPartResource* entry);
    void* flags(LayoutPartResource* entry);
};
WWHD_SIZE(LayoutBuildOptions,0x1C);
WWHD_SIZE(LayoutPartIndex,0x1C); WWHD_SIZE(LayoutPartResource,0x28); WWHD_OFFSET(LayoutPartIndex,resource,0xC);
} }
void* sessionAllocate(u32 size);
void sessionDelete(void* object);
void sessionErase(nw::ut::HeapChildren*,gptr<nw::ut::HeapLink>* iterator);
void sessionInsert(nw::ut::HeapChildren*,gptr<nw::ut::HeapLink>* iterator,void* element);
u32 sessionShortNameEqual(const be<u8>*,const be<u8>*);

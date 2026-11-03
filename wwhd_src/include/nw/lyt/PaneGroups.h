/* Named pane groups and linked membership, reconstructed from group constructors and searches. */
#pragma once
#include "hd/hd_ui_02003C58.h"
#include "nw/ut/FrameHeap.h"
namespace nw { namespace lyt {
struct PaneGroupResource {
    u8 prefix[8]; be<u8> name[24]; be<u16> count, reserved; be<u8> names[1][24];
};
struct NamedPaneGroup {
    nw::ut::HeapLink link;
    nw::ut::HeapChildren members;
    be<u8> name[25];
    be<u8> flags;
    u8 padding[2];
    be<u32> methods;
    static NamedPaneGroup* create(NamedPaneGroup* group);
    static NamedPaneGroup* createFromResource(NamedPaneGroup* group, PaneGroupResource* resource, hdLayout_lookup_c* lookup);
    void initializeFlags();
    void appendPane(void* pane);
    void destroy(u32 flags);
};
struct PaneGroupList : nw::ut::HeapChildren {
    void destroy(u32 flags);
    void append(NamedPaneGroup* group);
    NamedPaneGroup* find(const be<u8>* name);
};
WWHD_SIZE(NamedPaneGroup, 0x34);
WWHD_OFFSET(NamedPaneGroup, members, 8);
WWHD_OFFSET(NamedPaneGroup, name, 0x14);
WWHD_OFFSET(NamedPaneGroup, flags, 0x2D);
WWHD_OFFSET(NamedPaneGroup, methods, 0x30);
WWHD_OFFSET(PaneGroupResource, count, 0x20);
WWHD_OFFSET(PaneGroupResource, names, 0x24);
} }
void* paneGroupAllocate(u32 size);
void* paneGroupAlignedAllocate(u32 size, s32 alignment);
void paneGroupDelete(void* object);
void paneGroupRelease(void* storage);
void paneGroupClearMemory(void* storage, u32 byte, u32 size);
void paneGroupPrepareNode(void* storage, u32 size);
void paneGroupLinksInsert(nw::ut::HeapChildren*, gptr<nw::ut::HeapLink>*, nw::ut::HeapLink*);
void paneGroupLinksErase(nw::ut::HeapChildren*, gptr<nw::ut::HeapLink>*);

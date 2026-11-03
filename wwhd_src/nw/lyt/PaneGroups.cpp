/* Group ownership is established by the resource's pane-name lookup and member traversal.
 * Member nodes reuse the merged HD pane-node view; no external reference used. */
#include "nw/lyt/PaneGroups.h"
#include "nw/lyt/ResourceHelpers.h"
using namespace nw::lyt;
static u8 sNamedGroupMethods;

static void initializeGroupLinks(nw::ut::HeapChildren* links) {
    if (gabi::ea(links) == 0) links = static_cast<nw::ut::HeapChildren*>(paneGroupAllocate(sizeof(nw::ut::HeapChildren)));
    if (links) {
        links->count = 0;
        links->root.previous = &links->root;
        links->root.next = &links->root;
    }
}

/* 02874478 */
void NamedPaneGroup::initializeFlags() { flags = 0; }

/* 02874484 */
NamedPaneGroup* NamedPaneGroup::create(NamedPaneGroup* group) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!group) group = static_cast<NamedPaneGroup*>(paneGroupAllocate(sizeof(NamedPaneGroup)));
    if (group) {
        group->link.next = nullptr;
        group->link.previous = nullptr;
        group->methods = gabi::ea(&sNamedGroupMethods);
        initializeGroupLinks(&group->members);
        group->initializeFlags();
        paneGroupClearMemory(group->name, 0, 25);
    }
    return group;
}

/* 02874524 */
void NamedPaneGroup::appendPane(void* pane) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* node = static_cast<hdLayout_groupNode_c*>(paneGroupAlignedAllocate(sizeof(hdLayout_groupNode_c), 4));
    if (!node) return;
    paneGroupPrepareNode(node, sizeof(hdLayout_groupNode_c));
    node->mpPane = pane;
    *reinterpret_cast<be<u32>*>(node->_04) = 0;
    node->mpNext = nullptr;
    gabi::FrameLocal<gptr<nw::ut::HeapLink>> position(8);
    *position = &members.root;
    paneGroupLinksInsert(&members, position.get(), reinterpret_cast<nw::ut::HeapLink*>(node));
}

/* 028745B0 */
NamedPaneGroup* NamedPaneGroup::createFromResource(NamedPaneGroup* group, PaneGroupResource* resource, hdLayout_lookup_c* lookup) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!group) group = static_cast<NamedPaneGroup*>(paneGroupAllocate(sizeof(NamedPaneGroup)));
    if (group) {
        group->link.next = nullptr;
        group->link.previous = nullptr;
        group->methods = gabi::ea(&sNamedGroupMethods);
        initializeGroupLinks(&group->members);
        group->initializeFlags();
        auto* text = resource->name;
        auto* destination = group->name;
        for (u32 index = 0; index < 24; ++index) {
            u8 character = *text++;
            if (character == 0) break;
            *destination++ = character;
        }
        *destination = 0;
        u32 index = 0;
        while (index < u16(resource->count)) {
            hdLayout_lookupVtable_c* dispatch = lookup->mVtable;
            void* pane = gabi::call_ptr<void*>(dispatch->mFindPane, lookup, resource->names[index], 1u);
            if (pane) group->appendPane(pane);
            ++index;
        }
    }
    return group;
}

/* 028746C0 */
void NamedPaneGroup::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    nw::ut::HeapLink* next = members.root.next;
    auto* sentinel = &members.root;
    methods = gabi::ea(&sNamedGroupMethods);
    while (gabi::ea(next) != gabi::ea(sentinel)) {
        nw::ut::HeapLink* current = next;
        next = current->next;
        gabi::FrameLocal<gptr<nw::ut::HeapLink>> iterator(8);
        *iterator = current;
        paneGroupLinksErase(&members, iterator.get());
        if (current) paneGroupRelease(current);
    }
    if (flags & 1) paneGroupDelete(this);
}

/* 028747F0 */
void PaneGroupList::append(NamedPaneGroup* group) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<gptr<nw::ut::HeapLink>> position(8);
    *position = &root;
    paneGroupLinksInsert(this, position.get(), &group->link);
}

/* 02874820 */
NamedPaneGroup* PaneGroupList::find(const be<u8>* name) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* group = reinterpret_cast<NamedPaneGroup*>((nw::ut::HeapLink*)root.next);
    auto* sentinel = reinterpret_cast<NamedPaneGroup*>(&root);
    while (gabi::ea(group) != gabi::ea(sentinel)) {
        if (layoutShortNameEqual(group->name, name) != 0) return group;
        group = reinterpret_cast<NamedPaneGroup*>((nw::ut::HeapLink*)group->link.next);
    }
    return nullptr;
}

struct PaneGroupDestroyMethods { u8 prefix[0xC]; be<u32> destroy; };

/* 0287474C */
void PaneGroupList::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    auto* next = reinterpret_cast<NamedPaneGroup*>((nw::ut::HeapLink*)root.next);
    auto* sentinel = reinterpret_cast<NamedPaneGroup*>(&root);
    while (gabi::ea(next) != gabi::ea(sentinel)) {
        NamedPaneGroup* current = next;
        next = reinterpret_cast<NamedPaneGroup*>((nw::ut::HeapLink*)current->link.next);
        gabi::FrameLocal<gptr<nw::ut::HeapLink>> iterator(8);
        *iterator = reinterpret_cast<nw::ut::HeapLink*>(current);
        paneGroupLinksErase(this, iterator.get());
        if (u8(current->flags) == 0 && current) {
            auto* dispatch = gabi::at<PaneGroupDestroyMethods>(current->methods);
            gabi::call_ptr<void>(dispatch->destroy, current, 2u);
            paneGroupRelease(current);
        }
    }
    if (flags & 1) paneGroupDelete(this);
}

/* WWHD glue
ext 0273AD10 void* paneGroupAllocate(u32)
ext 0287F3DC void* paneGroupAlignedAllocate(u32, s32)
ext 0273AF40 void paneGroupDelete(void*)
ext 0287F3FC void paneGroupRelease(void*)
ext C000A858 void paneGroupClearMemory(void*, u32, u32)
ext 028F521C void paneGroupPrepareNode(void*, u32)
ext 0286DA70 void paneGroupLinksInsert(nw::ut::HeapChildren*, gptr<nw::ut::HeapLink>*, nw::ut::HeapLink*)
ext 0286DB18 void paneGroupLinksErase(nw::ut::HeapChildren*, gptr<nw::ut::HeapLink>*)
data sNamedGroupMethods 10180304 nocheck
*/

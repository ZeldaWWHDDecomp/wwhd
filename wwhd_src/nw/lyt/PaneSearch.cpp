/* Pane search and ancestor scaling from virtual slots and intrusive child links. */
#include "nw/lyt/PaneState.h"
#include "nw/lyt/PartsPane.h"
#include "nw/lyt/ResourceHelpers.h"
using namespace nw::lyt;
void paneCleanupErase(nw::ut::HeapChildren*, gptr<nw::ut::HeapLink>*);
void paneCleanupRelease(void*);
void paneCleanupDelete(void*);
static u8 sCleanupPaneMethods;
struct PaneCleanupMethods { u8 prefix[0xC]; be<u32> destroy; u8 gap[4]; be<u32> runtimeType; };
struct NestedLayoutCleanupMethods { u8 prefix[0x14]; be<u32> destroy; };
struct SearchMethods {
    u8 prefix[0x14]; be<u32> runtimeType;
    u8 beforeMaterial[0x34]; be<u32> material;
    u8 beforeCount[4]; be<u32> materialCount;
    u8 beforeSearch[0x4C]; be<u32> findPane;
    u8 beforeMaterialSearch[4]; be<u32> findMaterial;
};
static RuntimeTypeGuard sSearchPartsGuard;
static RuntimeTypeNode sSearchPartsType;

/* 02875BA4 */
void LayoutPane::ancestorScale(be<f32>* output, LayoutPane* ancestor) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    output[1] = 1.0f;
    output[0] = 1.0f;
    while (ancestor) {
        u32 ready = sSearchPartsGuard.ready;
        auto* dispatch = paneMethods<SearchMethods>(ancestor);
        if (ready == 0) {
            u32 parentReady = sPaneTypeGuard.ready;
            sSearchPartsGuard.ready = 1;
            if (parentReady == 0) {
                sPaneType.parent = nullptr;
                sPaneTypeGuard.ready = 1;
            }
            sSearchPartsType.parent = &sPaneType;
        }
        RuntimeTypeNode* type = gabi::call_ptr<RuntimeTypeNode*>(dispatch->runtimeType, ancestor);
        if (type == &sSearchPartsType) break;
        f32 x = ancestor->content()->scale[0];
        f32 outputX = output[0];
        f32 outputY = output[1];
        f32 nextX = gabi::fmuls_ppc(outputX, x);
        f32 y = ancestor->content()->scale[1];
        f32 nextY = gabi::fmuls_ppc(outputY, y);
        output[0] = nextX;
        output[1] = nextY;
        ancestor = ancestor->content()->parent;
    }
}

/* 02876554 */
LayoutPane* LayoutPane::findPane(const be<u8>* name, u32 recursive) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (layoutShortNameEqual(content()->name, name) != 0) return this;
    if (recursive == 0) return nullptr;
    auto* sentinel = &content()->children.root;
    nw::ut::HeapLink* child = sentinel->next;
    while (child != sentinel) {
        auto* pane = reinterpret_cast<LayoutPane*>(child);
        auto* dispatch = paneMethods<SearchMethods>(pane);
        auto* found = gabi::call_ptr<LayoutPane*>(dispatch->findPane, pane, name);
        if (found) return found;
        child = child->next;
    }
    return nullptr;
}

/* 028765F8 */
LayoutMaterial* LayoutPane::findMaterial(const be<u8>* name, u32 recursive) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* dispatch = paneMethods<SearchMethods>(this);
    u32 remaining = gabi::call_ptr<u32>(dispatch->materialCount, this);
    u32 index = 0;
    while (remaining != 0) {
        dispatch = paneMethods<SearchMethods>(this);
        auto* material = gabi::call_ptr<LayoutMaterial*>(dispatch->material, this, index);
        if (material && layoutLongNameEqual(material->name, name) != 0) return material;
        --remaining;
        ++index;
    }
    if (recursive == 0) return nullptr;
    auto* sentinel = &content()->children.root;
    nw::ut::HeapLink* child = sentinel->next;
    while (child != sentinel) {
        auto* pane = reinterpret_cast<LayoutPane*>(child);
        dispatch = paneMethods<SearchMethods>(pane);
        auto* found = gabi::call_ptr<LayoutMaterial*>(dispatch->findMaterial, pane, name);
        if (found) return found;
        child = child->next;
    }
    return nullptr;
}


/* 0287613C */
void LayoutPane::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    auto* sentinel = &content()->children.root;
    nw::ut::HeapLink* next = sentinel->next;
    mVtable = gabi::ea(&sCleanupPaneMethods);
    while (next != sentinel) {
        nw::ut::HeapLink* entry = next;
        next = entry->next;
        gabi::FrameLocal<gptr<nw::ut::HeapLink>> iterator(8);
        *iterator = entry;
        paneCleanupErase(&content()->children, iterator.get());
        auto* child = reinterpret_cast<LayoutPane*>(entry);
        if (u8(child->content()->flags) & 8) continue;
        if (u32(sSearchPartsGuard.ready) == 0) {
            u32 parentReady = sPaneTypeGuard.ready;
            sSearchPartsGuard.ready = 1;
            if (parentReady == 0) {
                sPaneType.parent = nullptr;
                sPaneTypeGuard.ready = 1;
            }
            sSearchPartsType.parent = &sPaneType;
        }
        bool parts = false;
        if (child) {
            auto* dispatch = paneMethods<PaneCleanupMethods>(child);
            RuntimeTypeNode* type = gabi::call_ptr<RuntimeTypeNode*>(dispatch->runtimeType, child);
            while (type) {
                if (type == &sSearchPartsType) { parts = true; break; }
                type = type->parent;
            }
        }
        if (parts && child) {
            hdLayout_c* nested = reinterpret_cast<LayoutParts*>(child)->nestedLayout;
            if (nested) {
                auto* methods = reinterpret_cast<NestedLayoutCleanupMethods*>((hdLayout_vtable_c*)nested->mVtable);
                gabi::call_ptr<void>(methods->destroy, nested, u32(2));
                paneCleanupRelease(nested);
                continue;
            }
        }
        if (child) {
            auto* dispatch = paneMethods<PaneCleanupMethods>(child);
            gabi::call_ptr<void>(dispatch->destroy, child, u32(2));
            paneCleanupRelease(child);
        }
    }
    if (flags & 1) paneCleanupDelete(this);
}

/* WWHD glue
ext 0286DB18 void paneCleanupErase(nw::ut::HeapChildren*, gptr<nw::ut::HeapLink>*)
ext 0287F3FC void paneCleanupRelease(void*)
ext 0273AF40 void paneCleanupDelete(void*)
data sCleanupPaneMethods 101805D4 nocheck
data sSearchPartsGuard 101FD890 nocheck
data sSearchPartsType 101FD91C nocheck
*/

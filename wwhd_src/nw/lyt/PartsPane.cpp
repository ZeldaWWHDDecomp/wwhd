/* Parts callbacks from table 10180314; local searches use actual virtual material methods. */
#include "nw/lyt/PartsPane.h"
#include "nw/lyt/ResourceHelpers.h"
using namespace nw::lyt;
static u8 sPartsMethods;
static RuntimeTypeGuard sPartsGuard;
static RuntimeTypeNode sPartsType;
struct PartsMaterialMethods {
    u8 prefix[0x4C]; be<u32> material; u8 padding[4]; be<u32> count;
};

/* 02874894 */
LayoutParts* LayoutParts::create(LayoutParts* pane, void* resource, void* unused, void* context) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!pane) pane = static_cast<LayoutParts*>(paneStateAllocate(sizeof(LayoutParts)));
    if (pane) {
        LayoutPane::createFromResource(pane, resource, context);
        pane->nestedLayout = nullptr;
        pane->mVtable = gabi::ea(&sPartsMethods);
        pane->stateA4 = 0;
        pane->stateA8 = 0;
    }
    return pane;
}

/* 02874918 */
void LayoutParts::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    LayoutPane::destroy(0);
    if (flags & 1) partsPaneDelete(this);
}

/* 0287496C */
LayoutPane* LayoutParts::findSelf(const be<u8>* name) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return layoutShortNameEqual(content()->name, name) != 0 ? this : nullptr;
}

/* 028749B8 */
LayoutMaterial* LayoutParts::findLocalMaterial(const be<u8>* name) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* dispatch = paneMethods<PartsMaterialMethods>(this);
    u32 remaining = gabi::call_ptr<u32>(dispatch->count, this);
    u32 index = 0;
    while (remaining != 0) {
        dispatch = paneMethods<PartsMaterialMethods>(this);
        auto* material = gabi::call_ptr<LayoutMaterial*>(dispatch->material, this, index);
        if (material && layoutLongNameEqual(material->name, name) != 0) return material;
        --remaining;
        ++index;
    }
    return nullptr;
}

/* 02874A48 */
RuntimeTypeNode* nw::lyt::partsRuntimeType() {
    if (u32(sPartsGuard.ready) == 0) {
        u32 parentReady = sPaneTypeGuard.ready;
        sPartsGuard.ready = 1;
        if (parentReady == 0) {
            sPaneTypeGuard.ready = 1;
            sPaneType.parent = nullptr;
        }
        sPartsType.parent = &sPaneType;
    }
    return &sPartsType;
}

/* WWHD glue
ext 0273AF40 void partsPaneDelete(void*)
data sPartsMethods 10180314 nocheck
data sPartsGuard 101FD890 nocheck
data sPartsType 101FD91C nocheck
*/

/* Bounding callback ownership follows constructor table 101804E4 and its exact slots. */
#include "nw/lyt/BoundingPane.h"
using namespace nw::lyt;
static u8 sBoundingMethods;
static RuntimeTypeGuard sBoundingGuard;
static RuntimeTypeNode sBoundingType;

/* 028757F0 */
LayoutBounding* LayoutBounding::create(LayoutBounding* pane, void* resource, void* unused, void* context) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!pane) pane = static_cast<LayoutBounding*>(paneStateAllocate(sizeof(LayoutBounding)));
    if (pane) {
        LayoutPane::createFromResource(pane, resource, context);
        pane->mVtable = gabi::ea(&sBoundingMethods);
    }
    return pane;
}

/* 02875864 */
void LayoutBounding::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    LayoutPane::destroy(0);
    if (flags & 1) boundingPaneDelete(this);
}

/* 028758B8 */
void LayoutBounding::noOp() {}

/* 028758BC */
RuntimeTypeNode* nw::lyt::boundingRuntimeType() {
    if (u32(sBoundingGuard.ready) == 0) {
        u32 parentReady = sPaneTypeGuard.ready;
        sBoundingGuard.ready = 1;
        if (parentReady == 0) {
            sPaneTypeGuard.ready = 1;
            sPaneType.parent = nullptr;
        }
        sBoundingType.parent = &sPaneType;
    }
    return &sBoundingType;
}

/* WWHD glue
ext 0273AF40 void boundingPaneDelete(void*)
data sBoundingMethods 101804E4 nocheck
data sBoundingGuard 101FD88C nocheck
data sBoundingType 101FD940 nocheck
*/

/* Bounding-pane callbacks on the canonical pane storage. */
#pragma once
#include "nw/lyt/PaneState.h"
#include "nw/lyt/ResourceProvider.h"
namespace nw { namespace lyt {
struct LayoutBounding : LayoutPane {
    static LayoutBounding* create(LayoutBounding* pane, void* resource, void* unused, void* context);
    void destroy(u32 flags);
    void noOp();
};
RuntimeTypeNode* boundingRuntimeType();
WWHD_SIZE(LayoutBounding, sizeof(hdLayout_bounding_c));
} }
void boundingPaneDelete(void* object);

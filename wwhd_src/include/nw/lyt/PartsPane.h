/* Parts-pane extension, with the nested layout at its constructor-proven offset. */
#pragma once
#include "nw/lyt/PaneState.h"
#include "nw/lyt/ResourceProvider.h"
#include "nw/lyt/LayoutMaterial.h"
namespace nw { namespace lyt {
struct LayoutParts : LayoutPane {
    be<u32> stateA4, stateA8;
    gptr<hdLayout_c> nestedLayout;
    static LayoutParts* create(LayoutParts*, void* resource, void* unused, void* context);
    void destroy(u32 flags);
    LayoutPane* findSelf(const be<u8>* name);
    LayoutMaterial* findLocalMaterial(const be<u8>* name);
};
RuntimeTypeNode* partsRuntimeType();
WWHD_SIZE(LayoutParts, 0xB0);
WWHD_OFFSET(LayoutParts, nestedLayout, 0xAC);
} }
void partsPaneDelete(void* storage);

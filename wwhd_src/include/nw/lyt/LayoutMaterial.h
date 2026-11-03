/* Material name, ownership flag and destructor table proven by picture lookup and cleanup. */
#pragma once
#include "nw/lyt/FontResource.h"
namespace nw { namespace lyt {
struct LayoutMaterialMethods { u8 prefix[0xC]; be<u32> destroy; };
struct LayoutMaterial {
    ColorWord colors[2];
    be<u32> layoutFlags, featureFlags;
    gptr<be<u8>> storage;
    gptr<void> resource;
    be<u8> name[28];
    u8 reserved;
    be<u8> ownershipFlags;
    u8 padding[2];
    gptr<LayoutMaterialMethods> methods;
};
WWHD_SIZE(LayoutMaterial, 0x3C);
WWHD_OFFSET(LayoutMaterial, name, 0x18);
WWHD_OFFSET(LayoutMaterial, ownershipFlags, 0x35);
WWHD_OFFSET(LayoutMaterial, methods, 0x38);
} }

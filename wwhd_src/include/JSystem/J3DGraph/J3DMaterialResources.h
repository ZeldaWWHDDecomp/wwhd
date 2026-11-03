// Relative material-resource references and the HD default descriptor.
#pragma once
#include "wwhd.h"
namespace j3d {
struct MaterialParameterHeader {
    u8 mPrefix[0x1C];
    be<u32> mParametersOffset;
};
struct MaterialResourceReference {
    gptr<MaterialParameterHeader> mResource;
    static s32 comparePriority(const MaterialResourceReference*,const MaterialResourceReference*);
};
struct MaterialPriority {
    u8 mPrefix[8];
    be<s32> mValue;
};
struct MaterialDefaults {
    be<u32> mWords[8];
    static MaterialDefaults* get();
};
struct MaterialDefaultsGuard { be<u32> mReady; };
}
WWHD_OFFSET(j3d::MaterialParameterHeader,mParametersOffset,0x1C);
WWHD_SIZE(j3d::MaterialResourceReference,4);
WWHD_OFFSET(j3d::MaterialPriority,mValue,8);
WWHD_SIZE(j3d::MaterialDefaults,32);

// Accessed prefix offsets and value strides; no unknown foreign extent is asserted.
WWHD_SIZE(j3d::MaterialDefaultsGuard,4);

// Narrow constructor views: only the observed storage extent and text prefix are established.
#pragma once
#include "sead/FileDevice.h"
namespace agl { namespace env {
struct EnvironmentReferencedText_l : sead::SafeText {
    static EnvironmentReferencedText_l* construct(EnvironmentReferencedText_l*,be<u8>* text);
};
struct EnvironmentAllocationStorage_l {
    u8 mStorage[64];
    static EnvironmentAllocationStorage_l* construct(EnvironmentAllocationStorage_l*);
};
}}
WWHD_SIZE(agl::env::EnvironmentReferencedText_l,8);
WWHD_SIZE(agl::env::EnvironmentAllocationStorage_l,64);

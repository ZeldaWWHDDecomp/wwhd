// WWHD mapped wrapper and input aggregation layouts from local instructions.
#pragma once
#include "sead/ControllerConstruction.h"
struct MappedInput : sead::ControllerWrapper {
    be<u8> mMappings[32];
    u32 mapButtons(u32 source);
    void calc(u32 previous,u32 previousPointer);
};
struct MappingDefaults { be<u8> mMappings[28]; };
WWHD_OFFSET(MappedInput,mMappings,0x174);
WWHD_SIZE(MappedInput,0x194);
WWHD_SIZE(MappingDefaults,28);

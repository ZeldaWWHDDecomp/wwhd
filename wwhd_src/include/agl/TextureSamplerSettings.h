/* Exact 0x40-byte sampler settings constructor layout from assigned WWHD stores. */
#pragma once
#include "wwhd.h"
namespace agl {
struct TextureSamplerSettings {
    be<u32> modes[6];
    be<f32> scalarParameters[7];
    be<u32> option;
    be<u8> enabled;
    u8 reserved[3];
    be<u32> tail;
    static TextureSamplerSettings* create(TextureSamplerSettings*);
};
WWHD_SIZE(TextureSamplerSettings, 0x40);
WWHD_OFFSET(TextureSamplerSettings, modes, 0);
WWHD_OFFSET(TextureSamplerSettings, scalarParameters, 0x18);
WWHD_OFFSET(TextureSamplerSettings, option, 0x34);
WWHD_OFFSET(TextureSamplerSettings, enabled, 0x38);
WWHD_OFFSET(TextureSamplerSettings, tail, 0x3C);
}

// Shadow pre-pass shader setup: the three shadow programs of the shader holder, on the merged agl program types.
#pragma once
#include "wwhd.h"
#include "agl/ShadowPrePass.h"
#include "agl/DrawProgram.h"
namespace agl::shadow_shader {
struct ShaderSetup {
    static u32 primary(shadow_prepass::ShadowPrePass* shadow, shadow_prepass::FrameResources* resources,
                         u32 previousMode, TextureSampler* depthSampler, TextureSampler* normalSampler, TextureSampler* decalSampler,
                         u32 sampleCount, TextureSampler* extraSampler, u8 useFirstSample, s32 width, s32 height);
    static u32 paired(void* unused, const shadow_prepass::FrameResources* resources, u32 pair, u32 member,
                        TextureSampler* firstSampler, TextureSampler* secondSampler, u32 previousMode);
    static u32 combined(void* unused, const shadow_prepass::FrameResources* resources, u32 previousMode,
                          TextureSampler* firstSampler, TextureSampler* secondSampler, u32 firstEnabled, u32 secondEnabled);
};
}

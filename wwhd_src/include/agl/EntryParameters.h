// Parameter subobjects identified in AGL#12's constructors and resets.
#pragma once
#include "agl/RenderEntry.h"
namespace agl {
struct EffectContext_l;
struct ScalarModeEntry : RenderEntry {
    FloatParameter mScalar;
    WordParameter mMode;
    u32 applyMode(EffectContext_l*, u32 shift, u32 initial);
    u32 applyCount(EffectContext_l*, u32 shift, u32 initial);
};
struct TripleSelectorEntry : RenderEntry {
    WordParameter mSelectors[3];
    u32 applySelector(EffectContext_l*, u32 kind, u32 mode, u32 initial);
};
struct ChannelSelectorEntry : RenderEntry { WordParameter mSelectors[5]; };
struct TriplePairEntry : RenderEntry { PairParameter mPairs[3]; u32 applyPairs(EffectContext_l*,u32 initial); };
struct DoublePairEntry : RenderEntry { PairParameter mPairs[2]; u32 applySize(EffectContext_l*,u32 initial); };
}
WWHD_OFFSET(agl::ScalarModeEntry,mScalar,0x118);
WWHD_OFFSET(agl::ScalarModeEntry,mMode,0x128);
WWHD_SIZE(agl::ScalarModeEntry,0x138);
WWHD_SIZE(agl::TripleSelectorEntry,0x148);
WWHD_SIZE(agl::ChannelSelectorEntry,0x168);
WWHD_SIZE(agl::TriplePairEntry,0x154);
WWHD_SIZE(agl::DoublePairEntry,0x140);

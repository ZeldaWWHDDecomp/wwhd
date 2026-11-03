// Render entry embedding an agl colour correction (agl/ColorCorrection.h): 027ADA14 initializes it and sets its
// enable flag (+0x234 = mEnable.mValue), and registers its parameter root (+4) under the entry's name.
#pragma once
#include "agl/RenderEntry.h"
#include "agl/ColorCorrection.h"
namespace agl {
struct EffectContext_l;
struct EmbeddedStorageEntry : RenderEntry {
    color_correction::ColorCorrection mStorage;
    u32 applyStorage(EffectContext_l*,u32 initial);
};
}
WWHD_OFFSET(agl::EmbeddedStorageEntry,mStorage,0x118);
WWHD_SIZE(agl::EmbeddedStorageEntry,0x15FC);
agl::EmbeddedStorageEntry* createEmbeddedEntry(agl::EmbeddedStorageEntry*, u32);

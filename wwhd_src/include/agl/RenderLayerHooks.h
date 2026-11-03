// Address-only interface view of the default render-layer callbacks.
#pragma once
#include "wwhd.h"
namespace agl {
// No receiver layout or size is claimed: these methods read no receiver data.
struct RenderLayerDefaultHooks_l {
    void prepare();
    void update();
    void beginRender();
    void beginMethods();
    void beforeMethod();
    void afterMethod();
    void endMethods();
    void endRender();
    u32 filter();
    u32 select();
    // Additional empty table body; semantic slot role remains unspecified.
    void emptyHook();
};
}

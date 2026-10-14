/** HD render resource registration; local game-derived layouts, */
#pragma once
#include "bindings.h"

#include "hd/hd_layer_base.h"
struct hdRenderResources_c {
    /* 0x00 */ u8 mDisposer[12];
    /* 0x0C */ be<u32> mVtable;
    /* 0x10 */ gptr<void> mpMainEnvironment;
    /* 0x14 */ gptr<void> mpSecondaryEnvironment;
    /* 0x18 */ gptr<void> mpShadowEnvironment;
    /* 0x1C */ gptr<void> mpPostEnvironment;
    /* 0x20 */ gptr<void> mpParticleEnvironments[4];
    /* 0x30 */ gptr<void> mpPostEnvironments[7];
    static hdRenderResources_c* create(void* heap);
    void load();
    void bindLayers();
    void destroy(u32 flags);
};
WWHD_OFFSET(hdRenderResources_c, mVtable, 0x0C);
WWHD_OFFSET(hdRenderResources_c, mpMainEnvironment, 0x10);
WWHD_OFFSET(hdRenderResources_c, mpParticleEnvironments, 0x20);
WWHD_OFFSET(hdRenderResources_c, mpPostEnvironments, 0x30);
WWHD_SIZE(hdRenderResources_c, 0x4C);

struct hdRenderLayerRegistry_l {
    u8 mRenderer[0x1020];
    be<u32> mLayerCount;
    gptr<gptr<hdLayerBase_c>> mpLayers;
    hdLayerBase_c* layer(u32 index) {
        gptr<hdLayerBase_c>* layers = mpLayers;
        return layers[u32(mLayerCount) > index ? index : 0];
    }
};
WWHD_OFFSET(hdRenderLayerRegistry_l, mLayerCount, 0x1020);
WWHD_OFFSET(hdRenderLayerRegistry_l, mpLayers, 0x1024);
WWHD_SIZE(hdRenderLayerRegistry_l, 0x1028);

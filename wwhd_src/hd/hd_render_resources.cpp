/** HD render resource routing into shadow, scene and particle layers. Private decomp source. */
#include "hd/hd_render_resources.h"
enum RenderLayer { LAYER_SHADOW = 0, LAYER_SCENE = 1, LAYER_PARTICLE = 4, LAYER_FOREGROUND = 8 };
static hdRenderLayerRegistry_l* sResourcesRenderLayers;
static hdRenderResources_c* sResourcesResources;
static hdRenderResources_c* sResourcesRegisteredResources;
static u8 sResourcesResourceVtable;
static void setShadowEnvironment(hdLayerBase_c*, void*);
static void setMainEnvironment(hdLayerBase_c*, void*);
static void setSecondaryEnvironment(hdLayerBase_c*, void*);
static void setLightEnvironment(hdLayerBase_c*, void*, u32, f32);
static void setPostEnvironment(hdLayerBase_c*, void*);
static void setParticleEnvironment(hdLayerBase_c*, void*);
static void releaseRenderResources(hdRenderResources_c*, u32);
static void destroyRenderDisposer(hdRenderResources_c*, u32);

/* 02727D40 */
void hdRenderResources_c::bindLayers() {
    hdRenderLayerRegistry_l* registry = sResourcesRenderLayers;
    setShadowEnvironment(registry->mpLayers[LAYER_SHADOW], mpShadowEnvironment);
    hdLayerBase_c* scene = registry->layer(LAYER_SCENE);
    setMainEnvironment(scene, mpMainEnvironment);
    setSecondaryEnvironment(scene, mpSecondaryEnvironment);
    setLightEnvironment(scene, mpPostEnvironments[0], 0, 1.0f);
    setPostEnvironment(scene, mpPostEnvironment);
    setParticleEnvironment(registry->layer(LAYER_PARTICLE), mpParticleEnvironments[0]);
    hdLayerBase_c* particles = registry->layer(LAYER_FOREGROUND);
    setParticleEnvironment(particles, mpParticleEnvironments[3]);
    setParticleEnvironment(particles, mpParticleEnvironments[0]);
}

/* 02727E34 */
void hdRenderResources_c::destroy(u32 flags) {
    if (this != NULL) {
        mVtable = gabi::ea(&sResourcesResourceVtable);
        if (this == sResourcesRegisteredResources) {
            hdRenderResources_c* resources = sResourcesResources;
            sResourcesRegisteredResources = NULL;
            releaseRenderResources(resources, 2);
            sResourcesResources = NULL;
        }
        destroyRenderDisposer(this, 0);
        if (flags & 1) {
            operator_delete(this);
        }
    }
}

/* 02727ED0 */
static void __sinit_hd_render_resources_cpp() {
    sinit_header_statics_rodata(0x1049F884, 0x101F8640, 0x1049F8E8, 0x10140FBC);
}

/* WWHD glue
 data sResourcesRenderLayers 101F95D0
 data sResourcesResources 101F8664
 data sResourcesRegisteredResources 101F8668
 data sResourcesResourceVtable 10140FC8 nocheck
 ext 02736328 void setShadowEnvironment(hdLayerBase_c*, void*)
 ext 0272FFE4 void setMainEnvironment(hdLayerBase_c*, void*)
 ext 0273004C void setSecondaryEnvironment(hdLayerBase_c*, void*)
 ext 0272FEE0 void setLightEnvironment(hdLayerBase_c*, void*, u32, f32)
 ext 027300B4 void setPostEnvironment(hdLayerBase_c*, void*)
 ext 02735538 void setParticleEnvironment(hdLayerBase_c*, void*)
 ext 02727860 void releaseRenderResources(hdRenderResources_c*, u32)
 ext 02752BEC void destroyRenderDisposer(hdRenderResources_c*, u32)
*/

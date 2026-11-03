/* Texture bindings and reciprocal texture dimensions used by material programs. */
#include "nw/lyt/MaterialGpuBindings.h"
using namespace nw::lyt;
struct MaterialDimensionConversion { be<u32> high,low; };
struct MaterialDimensionUniform { be<f32> width,height,unusedA,unusedB; };
/* 02882E74 */
void MaterialGpu::bindTextures(LayoutDrawContext* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count=static_cast<u32>(featureFlags)>>30;
    if (!count) return;
    u32 available=context->stateWordB8;
    if (count>3) count=3;
    if (available<count) count=available;
    if (!count) return;
    gabi::FrameLocal<MaterialDimensionConversion> conversion(8);
    gabi::FrameLocal<MaterialDimensionUniform> uniform(0x10);
    for (u32 i=0;i<count;++i) {
        auto* binding=reinterpret_cast<TextureBinding*>(storage.get()+i*sizeof(TextureBinding));
        u32 location=context->uniformLocation(22+i);
        materialBindTexture(binding->surface,location);
        materialBindSampler(&binding->sampler,location);
        if (i==0) {
            location=context->uniformLocation(18);
            u32 width=binding->width;
            conversion->low=width;
            u32 height=binding->height;
            conversion->high=0x43300000;
            conversion->low=height;
            f32 widthValue=static_cast<f32>(width);
            f32 heightValue=static_cast<f32>(height);
            uniform->unusedA=0.0f;
            f32 reciprocalWidth=gabi::fdivs_ppc(1.0f,widthValue);
            f32 reciprocalHeight=gabi::fdivs_ppc(1.0f,heightValue);
            uniform->width=reciprocalWidth;
            uniform->unusedB=0.0f;
            uniform->height=reciprocalHeight;
            materialBindingUniforms(location,4,uniform.get());
        }
    }
}
/* WWHD glue
ext C00068F8 void materialBindTexture(nw::lyt::TextureObject*,u32)
ext C00068E0 void materialBindSampler(nw::lyt::LayoutSampler*,u32)
ext C0006A40 void materialBindingUniforms(u32,u32,const void*)
*/

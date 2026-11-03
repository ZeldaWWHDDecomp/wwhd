/* Material program selection, opacity and ordered uniform/state application. */
#include "nw/lyt/MaterialGpuApply.h"
using namespace nw::lyt;
struct MaterialApplyConversion { be<u32> high,low; };
struct MaterialApplyUniform { be<f32> values[4]; };
struct MaterialApplyZeroUniform { be<u32> values[4]; };
/* 02883AB4 */
void MaterialGpu::apply(LayoutDrawContext* context,u32 alpha,u32 variant,u32 reset,const PaneSize* size) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 ownership=ownershipFlags;
    if (ownership&2) {
        uploadTransforms(context,size);
        bindTextures(context);
        return;
    }
    u32 textures=static_cast<u32>(featureFlags)>>30;
    if (textures>3) textures=3;
    context->stateWordB8=textures;
    u32 selector=programIndex();
    context->selectProgram(selector+variant*15);
    if (variant!=1) {
        u32 location=context->uniformLocation(8);
        gabi::FrameLocal<MaterialApplyConversion> conversion(0x18);
        conversion->low=alpha;
        conversion->high=0x43300000;
        constexpr f32 channelScale=0.003921568859368563f;
        constexpr f32 alphaScale=1.5378702300949953e-5f;
        gabi::FrameLocal<MaterialApplyUniform> uniform(8);
        uniform->values[1]=channelScale;
        uniform->values[0]=channelScale;
        f32 opacity=gabi::fmuls_ppc(alphaScale,static_cast<f32>(alpha));
        uniform->values[2]=channelScale;
        uniform->values[3]=opacity;
        materialApplyUniforms(location,4,uniform.get());
    }
    context->uploadProjection();
    if (reset) {
        u32 location=context->uniformLocation(10);
        gabi::FrameLocal<MaterialApplyZeroUniform> zero(0x20);
        zero->values[0]=0; zero->values[2]=0; zero->values[3]=0; zero->values[1]=0;
        materialApplyUniforms(location,4,zero.get());
    }
    uploadTransforms(context,size);
    bindTextures(context);
    uploadPixelUniforms(context);
    applyRenderState();
}
/* WWHD glue
ext 028740E4 void nw::lyt::LayoutDrawContext::selectProgram(u32)
ext 02874038 void nw::lyt::LayoutDrawContext::uploadProjection()
ext C0006A40 void materialApplyUniforms(u32,u32,const void*)
*/

/* Cafe rendering-state application at the library boundary, reconstructed from WWHD. */
#include "nw/lyt/RenderState.h"
struct PolygonMode { be<u32> frontFace,mode; };
struct PolygonModes { PolygonMode entries[4]; };
static PolygonModes sPolygonModes;
/* 0286DFB4 */
void nw::lyt::RenderState_l::apply() {
    gabi::NativeFrame<0x30> frame;
    u32 fail=stencilFail,reference=stencilFunction,depthFail=stencilDepthFail,pass=stencilPass;
    u32 stencil=stencilTest;
    layoutDepthControl(depthTest,depthWrite,depthFunction,stencil,stencil,reference,depthFail,pass,fail,reference,depthFail,pass,fail);
    u32 read=stencilWriteMask&255,write=stencilReadMask&255;
    layoutStencilMask(read,write,write,read,write,write);
    layoutColorControl(0xCC,(u8)colorWrite?255:0,0,1);
    layoutBlendControl(0,alphaBlendDestination,colorBlendSource,colorBlendEquation,1,alphaBlendSource,colorBlendDestination,alphaBlendEquation);
    layoutBlendColor((f32)(u8)blendColor[0],(f32)(u8)blendColor[1],(f32)(u8)blendColor[2],(f32)(u8)blendColor[3]);
    layoutAlphaTest(alphaTest,alphaFunction,alphaReference);
    u32 mask=channelMask[0];
    mask=(u8)channelMask[1]?(mask|2):(mask&~2u);
    mask=(u8)channelMask[2]?(mask|4):(mask&~4u);
    mask=(u8)channelMask[3]?(mask|8):(mask&~8u);
    layoutChannelMasks(mask,15,15,15,15,15,15,15);
    PolygonMode* mode=&sPolygonModes.entries[rasterMode];
    u32 front=frontMode,back=backMode;
    layoutPolygonControl(0,mode->frontFace,mode->mode,front==2&&back==2?0:1,front,back,offsetFront,offsetBack,offsetPoints);
}
/* WWHD glue
ext C0006778 void layoutAlphaTest(u32,u32,f32)
ext C00067A0 void layoutBlendColor(f32,f32,f32,f32)
ext C00067B0 void layoutBlendControl(u32,u32,u32,u32,u32,u32,u32,u32)
ext C00067D0 void layoutColorControl(u32,u32,u32,u32)
ext C0006848 void layoutDepthControl(u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32)
ext C0006930 void layoutPolygonControl(u32,u32,u32,u32,u32,u32,u32,u32,u32)
ext C0006990 void layoutStencilMask(u32,u32,u32,u32,u32,u32)
ext C00069F0 void layoutChannelMasks(u32,u32,u32,u32,u32,u32,u32,u32)
data sPolygonModes 1017F9F8
*/

#pragma once
#include "wwhd.h"
namespace nw { namespace lyt {
// Local view of the NW-UT render-state class.
struct RenderState_l {
    be<u8> depthTest,depthWrite;u8 depthPadding[2];
    be<u32> depthFunction,rasterMode;
    be<u8> colorWrite;u8 colorPadding[3];
    be<u32> alphaBlendDestination,alphaBlendSource,colorBlendSource,colorBlendDestination;
    be<u32> colorBlendEquation,alphaBlendEquation;
    be<u8> blendColor[4];
    be<u8> alphaTest;u8 alphaPadding[3];
    be<u32> alphaFunction;be<f32> alphaReference;
    be<u8> channelMask[4];
    be<u8> stencilTest;u8 stencilPadding[3];
    be<u32> stencilFunction,stencilReadMask,stencilWriteMask,stencilFail,stencilPass,stencilDepthFail;
    be<u32> frontMode,backMode;
    be<u8> offsetFront,offsetBack,offsetPoints,endPadding;
    void apply();
};
WWHD_SIZE(RenderState_l,0x64);WWHD_OFFSET(RenderState_l,blendColor,0x28);WWHD_OFFSET(RenderState_l,stencilTest,0x3C);WWHD_OFFSET(RenderState_l,frontMode,0x58);WWHD_OFFSET(RenderState_l,offsetFront,0x60);
} }
void layoutDepthControl(u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32);
void layoutStencilMask(u32,u32,u32,u32,u32,u32);
void layoutColorControl(u32,u32,u32,u32);
void layoutBlendControl(u32,u32,u32,u32,u32,u32,u32,u32);
void layoutBlendColor(f32,f32,f32,f32);
void layoutAlphaTest(u32,u32,f32);
void layoutChannelMasks(u32,u32,u32,u32,u32,u32,u32,u32);
void layoutPolygonControl(u32,u32,u32,u32,u32,u32,u32,u32,u32);

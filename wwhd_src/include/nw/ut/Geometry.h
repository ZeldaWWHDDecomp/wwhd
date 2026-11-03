#pragma once
#include "wwhd.h"
namespace nw { namespace ut {
struct Matrix34 { be<f32> values[3][4]; };
WWHD_SIZE(Matrix34,0x30);
struct Bounds3 {
    be<f32> lower[3];
    be<f32> upper[3];
    void order();
    static void transform(Bounds3*,const Bounds3*,const Matrix34*);
};
struct RenderSettings {
    be<u8> depthEnabled;
    be<u8> depthWrite;
    u8 padding[2];
    be<u32> depthCompare;
    be<u32> colorMask;
    be<u8> alphaEnabled;
    u8 alphaPadding[3];
    be<u32> alphaCompare;
    be<u32> alphaReference;
    be<u32> colorSource;
    be<u32> colorDestination;
    be<u32> colorOperation;
    be<u32> target;
    be<u32> logicMode;
    be<u8> blendEnabled;
    u8 blendPadding[3];
    be<u32> alphaSource;
    be<f32> blendConstant;
    be<u8> redEnabled;
    be<u8> greenEnabled;
    be<u8> blueEnabled;
    be<u8> alphaWrite;
    be<u8> separateAlpha;
    u8 separatePadding[3];
    be<u32> alphaDestination;
    be<u32> alphaOperation;
    be<u32> stencilMask;
    be<u32> stencilReference;
    be<u32> stencilCompare;
    be<u32> stencilFail;
    be<u32> stencilPass;
    be<u32> stencilDepthFail;
    be<u8> frontFace;
    be<u8> polygonOffset;
    be<u8> wireframe;
    u8 endPadding;
    be<u32> vtable;
    RenderSettings();
};
WWHD_SIZE(Bounds3,0x18); WWHD_OFFSET(Bounds3,upper,0xC);
WWHD_SIZE(RenderSettings,0x68); WWHD_OFFSET(RenderSettings,vtable,0x64); WWHD_OFFSET(RenderSettings,redEnabled,0x38);
} }

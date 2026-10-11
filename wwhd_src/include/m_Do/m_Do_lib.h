/* Utility projections and HD material helpers. */
#pragma once
#include "bindings.h"

struct mDoLibColor { be<f32> r, g, b, a; };
struct mDoLibPoint { be<f32> x, y; };
struct mDoLibRectangle { be<f32> left, top, right, bottom; u8 disposer[8]; };
struct mDoLibMatrix44 { be<f32> m[4][4]; };
struct scissor_class { be<f32> mXOrig, mYOrig, mWidth, mHeight; };
struct view_port_class {
    be<f32> mXOrig, mYOrig, mWidth, mHeight, mNearZ, mFarZ;
    scissor_class mScissor;
};
struct view_class {
    u8 padding[0x144];
    Mtx34 mViewMtx; // HD: four bytes later than GameCube.
};
struct mDoLibGameView {
    u8 padding[0x5FA0];
    gptr<view_port_class> mpViewPort;
    gptr<view_class> mpView;
};
// HD: render-state and material blocks replace the GameCube J3D material path.
struct mDoLibAlphaState { u8 storage[1]; };
struct mDoLibRenderState {
    u8 padding[8];
    be<u32> blendState;
    u8 padding2[12];
    mDoLibAlphaState alphaState;
};
struct mDoLibResource {
    u8 padding[0x20];
    be<u32> mRenderStateOffset;
};
struct mDoLibMaterialBlock {
    u8 padding[4];
    gptr<gfn> vtable;
};
struct mDoLibMaterialTarget { be<u32> flags; };
struct mDoLibMaterial {
    gptr<mDoLibResource> resource;
    u8 padding[20];
    gptr<mDoLibMaterialBlock> block;
    u8 padding2[132];
    mDoLibMaterialTarget target;
};
struct mDoLibCamera {
    u8 padding[72];
    gptr<Mtx34> view;
    gptr<mDoLibMatrix44> projection;
    be<u32> flags;
    u8 padding2[48];
    Mtx34 inlineView;
    u8 padding3[176];
    gptr<mDoLibMatrix44> indirectProjection;
};
struct mDoLibCameraSystem {
    u8 padding[0x1020];
    be<u32> cameraCount;
    gptr<gptr<mDoLibCamera>> cameras;
};
struct mDoLibCameraSystemPointer { gptr<mDoLibCameraSystem> system; };
struct mDoLibViewRecord {
    u8 padding[0x130];
    be<f32> mNearZ, mFarZ, mYOrig, mWidth, mXOrig, mHeight;
};
struct J3DUClipper {
    u8 padding[72];
    be<f32> mFovY, mAspect, mNear, mFar;
    be<u32> __vtbl; // HD: vtable follows the data instead of preceding it.
};
void mDoLib_clipper_setup(f32 fovY, f32 aspect, f32 n, f32 f);
struct mDoLibSinCosPair { be<f32> sine, cosine; };
struct mDoLibSinCosTable { mDoLibSinCosPair entries[8192]; };
struct mDoLibInitWords { be<u32> first, second, third, fourth; };
struct mDoLibInitFloats { be<f32> low, high; };
struct mDoLibRegistration { be<u32> next; u8 storage[8]; };
WWHD_SIZE(mDoLibColor, 16);
WWHD_SIZE(mDoLibRectangle, 24);
WWHD_SIZE(view_port_class, 40);
WWHD_OFFSET(mDoLibMaterial, block, 24);
WWHD_OFFSET(mDoLibMaterial, target, 160);
WWHD_OFFSET(mDoLibCamera, inlineView, 132);
WWHD_OFFSET(mDoLibCamera, indirectProjection, 0x164);
WWHD_SIZE(J3DUClipper, 92);
void libSetRenderMode(mDoLibRenderState*, u32);
void libSetAlphaCompareA(mDoLibAlphaState*, u32);
void libSetAlphaCompareB(mDoLibAlphaState*, u32);
void libSetAlphaOperationA(mDoLibAlphaState*, u32);
void libSetAlphaOperationB(mDoLibAlphaState*, u32);
void libSetBlendState(be<u32>*, u32);
void libLinearColor(mDoLibColor*, mDoLibColor*, f32);
mDoLibColor* libMaterialTarget(mDoLibMaterialTarget*, u32);
mDoLibMatrix44* dComIfGd_getProjViewMtx();
void cMtx_multVec(void*, const cXyz*, cXyz*);
void libRectangleConstruct(mDoLibRectangle*, f32, f32, f32, f32);
void libProjectScreen(Mtx34*, mDoLibPoint*, cXyz*, mDoLibMatrix44*, mDoLibRectangle*);
void libClipperCalc(J3DUClipper*);
void libClipperInit(J3DUClipper*);
void libRegisterGlobal(mDoLibRegistration*);
void libInitializeByteState(void*);
void libInitializeWideState(void*);

// HD: material and camera flags used by these helpers.
enum mDoLibMaterialFlags : u32 { mDoLibMaterial_LinearColor = 0x400 };
enum mDoLibCameraFlags : u32 {
    mDoLibCamera_InlineViewA = 0x40,
    mDoLibCamera_InlineViewB = 0x80,
    mDoLibCamera_IndirectProjection = 0x1000,
};
enum mDoLibMaterialVtableSlot : u32 {
    mDoLibMaterial_SetColor = 15,
    mDoLibMaterial_GetColor = 19,
};

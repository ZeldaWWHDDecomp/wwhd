/* Spilled water effect. */
#pragma once
#include "bindings.h"
#include "f_op/f_op_kankyo.h"
#include "gc_api/SSystem/c_phase.h"

// HD: field view extends the shared emitter's currently exposed prefix.
struct WpotEmitter {
    u8 padding[0x18E];
    be<u8> mDrawFlags;
    u8 padding2[0x25];
    be<u32> mAliveParticleCount;
    u8 padding3[8];
    be<u32> mAliveChildCount;
    u8 padding4[0x90];
    be<u32> mStatus;
};
struct dDlst_alphaVolPacket {
    u8 padding[0xC];
    be<u32> __vtbl;
    u8 padding2[0x88]; // HD: matrix follows the expanded packet state.
    Mtx34 mtx;
};
struct dWpotWater_EcallBack {
    be<u32> __vtbl;
    void draw(WpotEmitter* emtr);
};
class dWpotWater_c : public kankyo_class {
public:
    /* 0x0FC */ gptr<WpotEmitter> emtr;
    /* 0x100 */ dDlst_alphaVolPacket mPacket;
    BOOL draw();
    BOOL execute();
    cPhs_State create();
};
WWHD_OFFSET(WpotEmitter, mAliveParticleCount, 0x1B4);
WWHD_OFFSET(WpotEmitter, mAliveChildCount, 0x1C0);
WWHD_OFFSET(WpotEmitter, mStatus, 0x254);
WWHD_OFFSET(dDlst_alphaVolPacket, mtx, 0x98);
WWHD_OFFSET(dWpotWater_c, mPacket, 0x100);
WWHD_SIZE(dWpotWater_c, 0x1C8);
struct WpotDrawState {
    be<u8> mColorUpdate, mDepthUpdate;
    u8 padding[2];
    be<u32> mDepthCompare;
    u8 padding2[4];
    be<u32> mFlags;
    be<u32> mBlendFactors[10];
    u8 padding3[0xA8];
    be<u8> mAlphaUpdate;
    u8 padding4[3];
    be<u32> mAlphaCompare;
    be<f32> mAlphaReference;
    be<u32> mBlendMode;
    u8 padding5[0x2C];
};
WWHD_OFFSET(WpotDrawState, mAlphaUpdate, 0xE0);
WWHD_OFFSET(WpotDrawState, mBlendMode, 0xEC);
WWHD_SIZE(WpotDrawState, 284);
struct WpotSinCosPair { be<f32> sine, cosine; };
struct WpotSinCosTable { WpotSinCosPair entries[8192]; };
struct WpotDrawListView {
    u8 padding[0x5D7C];
    gptr<void> mpXluList;
};
struct WpotInitWords { be<u32> values[4]; };
struct WpotInitFloats { be<f32> low, high; };
struct WpotRegistration { be<u32> next; u8 storage[8]; };
enum WpotEmitterFlags : u32 {
    WpotEmitter_Delete = 8,
    WpotEmitter_Immortal = 0x40,
    WpotEmitter_ZDraw = 1,
};
enum WpotProcess : s16 { fpcNm_Obj_Magmarock_e = 0x2A, fpcNm_HITOBJ_e = 0xB7 };
enum WpotParticles : u16 {
    ID_IT_SN_WPOT_BITYA = 0x8083,
    ID_IT_SN_WPOT_SHIBUKI = 0x8084,
    ID_IT_SN_WPOT_YUKA = 0x8086,
};
enum WpotGround : u32 { WpotGround_Normal = 1, WpotGround_Lava = 4, WpotGround_LavaAttribute = 6 };
enum WpotRenderValue : u32 { WpotRender_Always = 7, WpotRender_LessEqual = 3, WpotRender_DestinationAlpha = 6, WpotRender_InverseDestinationAlpha = 7 };
void wpotDeleteStorage(void*, u32);
void wpotDrawEntry(void*, dDlst_alphaVolPacket*, u32);
void wpotKankyoDelete(dWpotWater_c*);
void wpotPacketDestruct(dDlst_alphaVolPacket*, u32);
void wpotBaseDestruct(dWpotWater_c*, u32);
void wpotGroundConstruct(dBgS_GndChk*);
void wpotGroundDestruct(dBgS_GndChk*, u32);
f64 wpotGroundCross(dBgS*, dBgS_GndChk*);
s32 wpotGetAttribute(dBgS*, cBgS_PolyInfo*);
void wpotBaseConstruct(dWpotWater_c*);
void wpotPacketConstruct(dDlst_alphaVolPacket*);
void wpotMatrixConcat(Mtx34*, Mtx34*, Mtx34*);
void wpotDrawStateConstruct(WpotDrawState*);
void wpotApplyBlend(WpotDrawState*);
void wpotApplyDepth(WpotDrawState*);
void wpotApplyCompare(WpotDrawState*);
void wpotApplyColor(WpotDrawState*);
void wpotRegisterGlobal(WpotRegistration*);
void wpotInitializeByteState(void*);
void wpotInitializeWideState(void*);

enum WpotChecks : u32 {
    WpotGround_InvalidPoly = 0xFFFF,
    WpotGround_InvalidBg = 0x100,
    WpotDestructor_Delete = 1,
    WpotRender_StateMask = 3,
    WpotRender_ModeLowMask = 0xF,
    WpotRender_ModeHighMask = 0xF0,
    WpotRender_ModeLowValue = 8,
    WpotRender_ModeHighValue = 0x80,
};

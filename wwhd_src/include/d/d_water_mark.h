/* Water-mark effect. */
#pragma once
#include "bindings.h"
#include "f_op/f_op_kankyo.h"
#include "SSystem/SComponent/c_m3d_g_pla.h"

// HD: these animations grew from GameCube's 0x18/0x14 to 0x78/0x74.
struct WaterBrkAnm {
    J3DFrameCtrl mFrameCtrl;
    u8 padding[0x68];
};
struct WaterBtpAnm {
    J3DFrameCtrl mFrameCtrl;
    u8 padding[0x64];
};
WWHD_SIZE(WaterBrkAnm, 0x78);
WWHD_SIZE(WaterBtpAnm, 0x74);
struct WaterHeapPointer { gptr<JKRHeap> mpHeap; };
WWHD_SIZE(WaterHeapPointer, 4);
struct WaterSolidHeap {
    u8 padding[0x10];
    be<u32> mDebugName;
};
WWHD_OFFSET(WaterSolidHeap, mDebugName, 0x10);
class dWaterMark_c : public kankyo_class {
public:
    /* 0x0FC */ gptr<WaterSolidHeap> mpHeap;
    /* 0x100 */ gptr<J3DModel> mpModel;
    /* 0x104 */ WaterBrkAnm mBrk;
    /* 0x17C */ WaterBtpAnm mBtp;
    /* 0x1F0 */ be<s16> field_0x12c;
    /* 0x1F2 */ be<s16> field_0x12e;
    /* 0x1F4 */ be<s16> field_0x130;
    /* 0x1F6 */ be<s16> field_0x132;
    /* 0x1F8 */ be<s16> field_0x134;
    u8 padding[2];
    u32 draw();
    u32 setMatrix();
    u32 execute();
    u32 wm_delete();
    u32 create();
};
WWHD_OFFSET(dWaterMark_c, mpHeap, 0xFC);
WWHD_OFFSET(dWaterMark_c, mpModel, 0x100);
WWHD_OFFSET(dWaterMark_c, mBrk, 0x104);
WWHD_OFFSET(dWaterMark_c, mBtp, 0x17C);
WWHD_OFFSET(dWaterMark_c, field_0x12c, 0x1F0);
WWHD_OFFSET(dWaterMark_c, field_0x134, 0x1F8);
WWHD_SIZE(dWaterMark_c, 0x1FC);

struct WaterResourceName {
    be<u32> __vtbl;
    be<u32> mString;
};
WWHD_SIZE(WaterResourceName, 8);
struct WaterSinCosPair { be<f32> sine; be<f32> cosine; };
struct WaterSinCosTable { WaterSinCosPair entries[8192]; };
WWHD_SIZE(WaterSinCosTable, 65536);

enum WaterResources : s32 {
    dRes_INDEX_ALWAYS_BDL_MPA_SIMI_e = 0x2F,
    dRes_INDEX_ALWAYS_BRK_MPA_SIMI_e = 0x4D,
    dRes_INDEX_ALWAYS_BTP_MPA_SIMI_e = 0x63,
};
u32 Water_clip(void*, Mtx34*, const cXyz*, f32);
void Water_brkEntry(WaterBrkAnm*, J3DModelData*, f32);
void Water_btpEntry(WaterBtpAnm*, J3DModelData*, s16);
void Water_modelUpdateDL(J3DModel*, u32);
f32 Water_GroundCross(dBgS*, dBgS_ObjGndChk*);
cM3dGPla* Water_GetTriPla(dBgS*, u16, u16);
s16 Water_ChkMoveBG(dBgS*, cBgS_PolyInfo*);
f32 Water_sqrt(f32);
void Water_ZXYrotM(Mtx34*, s16, s16, s16);
void Water_brkPlay(WaterBrkAnm*);
void Water_kankyoDelete(dWaterMark_c*);
void Water_destroySolidHeap(WaterSolidHeap*);
void Water_baseDelete(dWaterMark_c*, u32);
void Water_baseConstruct(dWaterMark_c*);
void Water_brkConstruct(WaterBrkAnm*);
void Water_btpConstruct(WaterBtpAnm*);
WaterSolidHeap* Water_createSolidHeap(u32, JKRHeap*, u32);
JKRHeap* Water_getGameHeap(u32);
void* Water_getResource(dRes_control_c*, WaterResourceName*, u32);
void Water_assert(const char*, u32, const char*);
u32 Water_brkInit(WaterBrkAnm*, J3DModelData*, void*, u32, u32, u32, u32, u32, u32, f32);
u32 Water_btpInit(WaterBtpAnm*, J3DModelData*, void*, u32, u32, u32, u32, u32, u32, f32);
void Water_restoreCurrentHeap();
void Water_adjustSolidHeap(WaterSolidHeap*);
void Water_groundConstruct(dBgS_ObjGndChk*);
void Water_groundDestruct(dBgS_ObjGndChk*, u32);
void Water_delete(void*);

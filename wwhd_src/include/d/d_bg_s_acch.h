// Actor background collision: GameCube fields, HD null checks and compiler-generated setup.
// Private method adapters leave the shared caller-facing inline declarations unchanged.
#pragma once
#include "bindings.h"
#include "SSystem/SComponent/c_m3d_g_pla.h"
namespace d_bg_s_acch_cpp {
struct AcchRead : dBgS_Acch {
    void GroundCheck(dBgS*);
    void LineCheck(dBgS*);
    void GroundCheckInit(dBgS*);
    void GroundRoofProc(dBgS*);
    void Set(cXyz*, cXyz*, fopAc_ac_c*, s32, dBgS_AcchCir*, cXyz*, csXyz*);
    f32 GetWallAllLowH_R();
    f32 GetWallAllLowH();
    void Init();
    void CrrPos(dBgS*);
    bool GetOnePolyInfo(u8*);
    f32 GetWallAllR();
    void SetWallCir();
    void CalcWallBmdCyl();
    f32 GetWallAddY(void*);
    void SetGroundUpY(f32);
};
WWHD_SIZE(AcchRead, 0x1C4);
struct AcchCirRead : dBgS_AcchCir {
    void SetWallR(f32);
    void SetWall(f32, f32);
};
WWHD_SIZE(AcchCirRead, 0x40);
// HD: complete query view of the shared opaque dBgS_WtrChk member.
struct AcchWaterRead : cBgS_PolyInfo, cBgS_Chk, dBgS_Chk {
    cXyz m_pos;
    be<f32> m_top;
    be<f32> m_height;
    u8 _4C[4];
};
WWHD_SIZE(AcchWaterRead, 0x50);
WWHD_OFFSET(AcchWaterRead, m_pos, 0x38);
WWHD_OFFSET(AcchWaterRead, m_top, 0x44);
WWHD_OFFSET(AcchWaterRead, m_height, 0x48);
struct AcchPlaneWords {
    be<u32> mNormal[3];
    be<f32> mD;
    be<u32> mVtable;
};
WWHD_SIZE(AcchPlaneWords, 0x14);
struct AcchPassFlags {
    be<u8> mPass[7];
};
WWHD_SIZE(AcchPassFlags, 7);
struct AcchRoomBg {
    be<u32> mpBgW;
    u8 _04[0x228];
};
WWHD_SIZE(AcchRoomBg, 0x22C);
struct AcchRooms {
    AcchRoomBg mRoom[64];
};
enum { UNK_1 = 1, DELETE_STORAGE = 1, INVALID_POLYGON = 0xFFFF, INVALID_BACKGROUND = 0x100 };
} // namespace d_bg_s_acch_cpp

f32 acch_cBgS_GroundCross(dBgS*, u32);
u32 acch_cBgS_GetTriPla(dBgS*, u32, u32);
void acch_dBgS_RideCallBack(dBgS*, u32, u32);
f32 acch_dBgS_RoofChk(dBgS*, u32);
bool acch_dBgS_SplGrpChk(dBgS*, u32);
void acch_dBgS_WallCorrect(dBgS*, dBgS_Acch*);
s32 acch_dBgS_GetRoomId(dBgS*, u32);
void acch_dBgS_MoveBgCrrPos(dBgS*, u32, u32, u32, u32, u32);
void acch_cBgS_LinChk_ct(u32);
void acch_cBgS_LinChk_Set2(u32, u32, u32, u32);
bool acch_cBgS_LineCross(dBgS*, u32);
void acch_cBgS_LinChk_dt(u32, s32);
void acch_PSVECAdd(u32, u32, u32);
f32 acch_PSVECSquareDistance(u32, u32);
void acch_cBgW_GetTopUnder(u32, u32, u32);
BOOL acch_daSea_ChkArea(f32, f32);
f64 acch_daSea_calcWave(f32, f32);
void acch_cM3dGCir_Set(u32, f32, f32, f32, f32);
void acch_cM3dGCyl_Set(u32, u32, f64, f32);
BOOL acch_cM3dGPla_getCrossYLessD(u32, void*, u32);
void acch_cBgS_Chk_dt(u32, s32);
void acch_cM3dGCir_ct(u32);
f64 acch_sqrtf(f32);
void acch_cBgS_Chk_ct(u32);
void acch_cM3dGCyl_ct(u32);
void acch_cM3dGPla_ct(u32);
void acch_cBgS_GndChk_ct(u32);
void acch_dBgS_RoofChk_ct(u32);
void acch_dBgS_WtrChk_ct(u32);
f64 acch_GetWallAllRUnrounded(dBgS_Acch*);

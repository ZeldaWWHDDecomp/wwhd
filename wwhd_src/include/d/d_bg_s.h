/* Background collision (d_bg_s*, c_bg_s*), WWHD layouts. 
 *
 * Sizes are unchanged from GameCube (dBgS_AcchCir 0x40, dBgS_Acch/ObjAcch 0x1C4: constructors
 * 024EFE94 and 024F0474 allocate these sizes; kamome's verified layout agrees).
 * [v] used by a verified function, [g] GameCube offset/signature not yet exercised. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "f_op/f_op_actor.h"
#include "wwhd.h"

struct cM3dGCir_l { u8 _[0x14]; };

struct dBgS_AcchCir {
    enum { WALL_HIT = 2, WALL_H_DIRECT = 4 };
    /* 0x00 */ u8 _00[0x10];          /* cBgS_PolyInfo */
    /* 0x10 */ be<u32> m_flags;
    /* 0x14 */ cM3dGCir_l m_cir;
    /* 0x28 */ be<f32> m_wall_rr;
    /* 0x2C */ be<f32> field_0x2c;
    /* 0x30 */ be<f32> m_wall_h;
    /* 0x34 */ be<f32> m_wall_r;
    /* 0x38 */ be<f32> m_wall_h_direct;
    /* 0x3C */ be<s16> m_wall_angle_y;
    /* 0x3E */ u8 _3E[2];
    /* 024EFF44 [g] */
    void SetWall(f32 halfHeight, f32 radius) { gabi::call(0x024EFF44, this, halfHeight, radius); }
    f32 GetWallH() { return m_wall_h; }
    f32 GetWallR() { return m_wall_r; }
    s16 GetWallAngleY() { return m_wall_angle_y; }
    bool ChkWallHit() { return (m_flags & WALL_HIT) != 0; }
};
WWHD_SIZE(dBgS_AcchCir, 0x40);

struct dBgS_Acch {
    enum {
        GRND_NONE = 1 << 1, WALL_NONE = 1 << 2, ROOF_NONE = 1 << 3, WALL_HIT = 1 << 4, GROUND_HIT = 1 << 5,
        GROUND_FIND = 1 << 6, GROUND_LANDING = 1 << 7, GROUND_AWAY = 1 << 8, ROOF_HIT = 1 << 9,
        WATER_NONE = 1 << 10, WATER_HIT = 1 << 11, WATER_IN = 1 << 12, LINE_CHECK = 1 << 13,
        LINE_CHECK_NONE = 1 << 14, CLR_SPEED_Y = 1 << 15, LINE_CHECK_HIT = 1 << 16, SEA_CHECK = 1 << 17,
        SEA_WATER_HEIGHT = 1 << 18, SEA_IN = 1 << 19,
    };
    /* 0x000 */ u8 _000[0x28];           /* cBgS_Chk, dBgS_Chk */
    /* 0x028 */ be<u32> m_flags;         /* [v] */
    /* 0x02C */ gptr<cXyz> pm_pos;
    /* 0x030 */ gptr<cXyz> pm_old_pos;
    /* 0x034 */ gptr<cXyz> pm_speed;
    /* 0x038 */ be<u32> pm_angle;
    /* 0x03C */ be<u32> pm_shape_angle;
    /* 0x040 */ u8 m_lin[0x1C];          /* cM3dGLin */
    /* 0x05C */ u8 m_wall_cyl[0x18];     /* cM3dGCyl */
    /* 0x074 */ be<s32> m_bg_index;
    /* 0x078 */ be<u32> field_0x78;
    /* 0x07C */ be<u32> m_ap_id;
    /* 0x080 */ gptr<fopAc_ac_c> m_my_ac;
    /* 0x084 */ be<s32> m_tbl_size;
    /* 0x088 */ gptr<dBgS_AcchCir> pm_acch_cir;
    /* 0x08C */ be<f32> m_ground_up_h;
    /* 0x090 */ be<f32> m_ground_up_h_diff;
    /* 0x094 */ be<f32> m_ground_h;      /* [v] */
    /* 0x098 */ be<f32> m_ground_check_offset;
    /* 0x09C */ u8 m_pla[0x14];          /* cM3dGPla */
    /* 0x0B0 */ be<u8> field_0xb0;
    /* 0x0B1 */ u8 _B1[3];
    /* 0x0B4 */ be<f32> field_0xb4;
    /* 0x0B8 */ be<f32> field_0xb8;
    /* 0x0BC */ be<f32> m_roof_height;
    /* 0x0C0 */ be<f32> m_roof_crr_height;
    /* 0x0C4 */ be<f32> field_0xC4;
    /* 0x0C8 */ be<f32> m_wtr_check_offset;
    /* 0x0CC */ be<u32> pm_out_poly_info;
    /* 0x0D0 */ be<f32> m_sea_height;
    /* 0x0D4 */ u8 m_gnd[0x54];          /* dBgS_GndChk; its cBgS_PolyInfo is the ground polygon */
    /* 0x128 */ u8 m_roof[0x4C];         /* dBgS_RoofChk */
    /* 0x174 */ u8 m_wtr[0x50];          /* dBgS_WtrChk */

    /* 024F06B4 [g] */
    void Set(cXyz* pos, cXyz* old_pos, fopAc_ac_c* actor, s32 tbl_size, dBgS_AcchCir* cir, cXyz* speed = nullptr,
             csXyz* angle = nullptr, csXyz* shape_angle = nullptr) {
        gabi::call(0x024F06B4, this, pos, old_pos, actor, tbl_size, cir, speed, angle, shape_angle);
    }
    /* 024F08A8 [v] */
    void CrrPos(dBgS* bgs) { gabi::call(0x024F08A8, this, bgs); }

    f32 GetGroundH() { return m_ground_h; }
    f32 GetRoofHeight() { return m_roof_height; }
    f32 GetSeaHeight() { return m_sea_height; }
    bool ChkGroundFind() { return (m_flags & GROUND_FIND) != 0; }
    bool ChkGroundHit() { return (m_flags & GROUND_HIT) != 0; }
    bool ChkGroundLanding() { return (m_flags & GROUND_LANDING) != 0; }
    u32 ChkWallHit() { return m_flags & WALL_HIT; }
    bool ChkRoofHit() { return (m_flags & ROOF_HIT) != 0; }
    bool ChkWaterHit() { return (m_flags & WATER_HIT) != 0; }
    bool ChkWaterIn() { return (m_flags & WATER_IN) != 0; }
    void ClrGroundHit() { m_flags &= ~(u32)GROUND_HIT; }
    void ClrWallHit() { m_flags &= ~(u32)WALL_HIT; }
    void SetGroundCheckOffset(f32 o) { m_ground_check_offset = o; }
    void SetWaterCheckOffset(f32 o) { m_wtr_check_offset = o; }
    void SetRoofCrrHeight(f32 h) { m_roof_crr_height = h; }
    void OnLineCheck() { m_flags |= LINE_CHECK; }
    void OffLineCheck() { m_flags &= ~(u32)LINE_CHECK; }
    void SetWallNone() { m_flags |= WALL_NONE; }
    void SetRoofNone() { m_flags |= ROOF_NONE; }
    void SetGrndNone() { m_flags |= GRND_NONE; }
    void ClrGrndNone() { m_flags &= ~(u32)GRND_NONE; }
    void SetWaterNone() { m_flags |= WATER_NONE; }
};
WWHD_OFFSET(dBgS_Acch, m_ground_h, 0x94);
WWHD_SIZE(dBgS_Acch, 0x1C4);

struct dBgS_ObjAcch : dBgS_Acch {};
WWHD_SIZE(dBgS_ObjAcch, 0x1C4);

/* line / ground checks on the stack: GameCube sizes (dBgS_LinChk 0x6C, dBgS_GndChk 0x54) [v kamome] */
struct dBgS_LinChk { u8 _[0x6C]; };
struct dBgS_GndChk { u8 _[0x54]; };

/* dBgS_MoveBgActor: HD size 0x3E0 (constructor 024F1D40); its C++ vtable is the actor's (+0xB4),
 * so its members follow fopAc_ac_c directly. Virtual slots in the HD vtable: CreateHeap,
 * Create, Execute(Mtx**), Draw 0x2C, Delete, IsDelete 0x3C, ToFore, ToBack. [v obj_table] */
struct dBgS_MoveBgActor : fopAc_ac_c {
    /* 0x3AC */ gptr<dBgW> mpBgW;
    /* 0x3B0 */ Mtx34 mBgMtx;
    static dBgS_MoveBgActor* ct(dBgS_MoveBgActor* p) { return gabi::call<dBgS_MoveBgActor*>(0x024F1D40, p); }
    cPhs_State MoveBGCreate(const char* arc, s32 dzb, u32 setFunc, u32 heapSize) {
        return gabi::call<cPhs_State>(0x024F1D9C, this, arc, dzb, setFunc, heapSize);
    }
    BOOL MoveBGDelete() { return gabi::call<BOOL>(0x024F1F64, this); }
    BOOL MoveBGExecute() { return gabi::call<BOOL>(0x024F1E9C, this); }
    /* virtual calls through the HD vtable at +0xB4 */
    BOOL vcall(u32 slot) { return gabi::call_ptr<BOOL>(gabi::load<u32>(__vtbl + slot), this); }
    BOOL Draw_v() { return vcall(0x2C); }
    BOOL IsDelete_v() { return vcall(0x3C); }
};
WWHD_OFFSET(dBgS_MoveBgActor, mBgMtx, 0x3B0);
WWHD_SIZE(dBgS_MoveBgActor, 0x3E0);
/* dBgW::ChkUsed (inline): the registration id below 0x100 [v obj_table] */
inline bool dBgW_ChkUsed(dBgW* w) { return gabi::load<u32>(gabi::ea(w)) < 0x100; }

/* dBgS_GndChk / dBgS_ObjGndChk members (0x54): pass-check pointers at +0/+4, vtables at
 * +0x10/+0x20/+0x40/+0x4C (per-TU copies), the position at +0x24, the cBgS_PolyInfo of the
 * hit at +0x14, the actor id at +0x8, mPass[7] at +0x44, mGrp at +0x50. [v kamome, ladder] */
struct dBgS_GndChk_vt { u32 v10, v20, v40, v4C; };
inline void dBgS_GndChk_ct(void* c, const dBgS_GndChk_vt& vt, bool obj) {
    u32 b = gabi::ea(c);
    gabi::call(0x02008E0C, c); /* cBgS_GndChk::cBgS_GndChk */
    for (int i = 0; i < 7; i++) gabi::store<u8>(b + 0x44 + i, (obj && i == 0) ? 1 : 0); /* dBgS_ObjGndChk: OnObj */
    gabi::store<u32>(b + 0x50, 1);
    gabi::store<u32>(b + 0x4, b + 0x4C);
    gabi::store<u32>(b + 0x0, b + 0x40);
    gabi::store<u32>(b + 0x4C, vt.v4C);
    gabi::store<u32>(b + 0x40, vt.v40);
    gabi::store<u32>(b + 0x20, vt.v20);
    gabi::store<u32>(b + 0x10, vt.v10);
}
inline void dBgS_GndChk_SetPos(void* c, const cXyz* p) { gabi::at<cXyz>(gabi::ea(c) + 0x24)->copy(*p); }
inline void dBgS_GndChk_SetActorPid(void* c, u32 pid) { gabi::store<u32>(gabi::ea(c) + 0x8, pid); }
inline cBgS_PolyInfo* dBgS_GndChk_PolyInfo(void* c) { return gabi::at<cBgS_PolyInfo>(gabi::ea(c) + 0x14); }
/* 024EECAC dBgS::GetMtrlSndId(cBgS_PolyInfo&) [v ladder] */
inline u32 dBgS_GetMtrlSndId(dBgS* bgs, cBgS_PolyInfo* p) { return gabi::call<u32>(0x024EECAC, bgs, p); }

/* dBgS_LinChk / dBgS_ObjLinChk (0x6C): pass-check pointers +0/+4, vtables +0x10/+0x20/+0x58/+0x64
 * (per-TU copies), mPass[7] +0x5C (ObjLinChk: mPass[0] = 1), mGrp +0x68; the cBgS_PolyInfo of a
 * hit at +0x14 (poly index u16 +0x14, bg index u16 +0x16). [v kamome, obj_hole] */
struct dBgS_LinChk_vt { u32 v10, v20, v58, v64; };
inline void dBgS_LinChk_ct(void* c, const dBgS_LinChk_vt& vt, bool obj) {
    u32 b = gabi::ea(c);
    gabi::call(0x02008FEC, c); /* cBgS_LinChk::ct */
    for (int i = 0; i < 7; i++) gabi::store<u8>(b + 0x5C + i, (obj && i == 0) ? 1 : 0);
    gabi::store<u32>(b + 0x68, 1);
    gabi::store<u32>(b + 0x4, b + 0x64);
    gabi::store<u32>(b + 0x0, b + 0x58);
    gabi::store<u32>(b + 0x10, vt.v10);
    gabi::store<u32>(b + 0x64, vt.v64);
    gabi::store<u32>(b + 0x58, vt.v58);
    gabi::store<u32>(b + 0x20, vt.v20);
}
/* dBgS_LinChk::Set 024F1AFC and cBgS::LineCross 02008860: see bindings.h */
/* cBgS::GetTriPla(bg_index, poly_index) -> cM3dGPla* [v obj_hole] */
inline void* cBgS_GetTriPla(dBgS* bgs, u16 bgIndex, u16 polyIndex) { return gabi::call<void*>(0x020084C8, bgs, bgIndex, polyIndex); }
inline void* dBgS_GetTriPla(dBgS* bgs, void* polyInfo) {
    u32 p = gabi::ea(polyInfo);
    return cBgS_GetTriPla(bgs, gabi::load<u16>(p + 2), gabi::load<u16>(p + 0));
}
/* cM3dGPla::CalcAngleXz(s16* x, s16* z), getCrossY(const cXyz&, f32*) [v obj_hole] */
inline void cM3dGPla_CalcAngleXz(void* pla, be<s16>* x, be<s16>* z) { gabi::call(0x02018BA8, pla, x, z); }
inline void cM3dGPla_getCrossY(void* pla, const cXyz* pos, be<f32>* y) { gabi::call(0x02018AE4, pla, pos, y); }

/* dBgS_ObjAcch inline constructor: dBgS_Acch::dBgS_Acch (024F0474) then the ObjAcch vtables at
 * +0x10/+0x14/+0x20 (per TU) and the "obj" pass flag at +0x18 [v obj_hole] */
struct dBgS_ObjAcch_vt { u32 v10, v14, v20; };
inline void dBgS_ObjAcch_ct(dBgS_Acch* a, const dBgS_ObjAcch_vt& vt) {
    u32 b = gabi::ea(a);
    gabi::call(0x024F0474, a);
    gabi::store<u8>(b + 0x18, 1);
    gabi::store<u32>(b + 0x10, vt.v10);
    gabi::store<u32>(b + 0x20, vt.v20);
    gabi::store<u32>(b + 0x14, vt.v14);
}
inline void dBgS_AcchCir_ct(dBgS_AcchCir* c) { gabi::call(0x024EFE94, c); }

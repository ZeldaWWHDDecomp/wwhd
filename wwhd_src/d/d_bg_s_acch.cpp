/* d_bg_s_acch: actor background collision (dBgS_Acch, dBgS_AcchCir), WWHD.
 *
 * Translation unit 024EFD08..024F12BC. Ported from the GameCube d_bg_s_acch.cpp on the shared
 * dBgS_Acch / dBgS_AcchCir layouts (wwhd_src/include/d/d_bg_s.h); sub-objects by offset:
 * cM3dGLin +0x40, cM3dGCyl +0x5C, cM3dGPla +0x9C (normal +0x9C, d +0xA8), dBgS_GndChk +0xD4
 * (poly info +0xE8, pos +0xF8), dBgS_RoofChk +0x128 (pos +0x160), dBgS_WtrChk +0x174 (pass
 * flags +0x19C, pos +0x1AC, top +0x1B8, height +0x1BC). */
#include "bindings.h"

namespace d_bg_s_acch_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline f32 cBgS_GroundCross_l(dBgS* bgs, u32 chk) { return gabi::call<f32>(0x02008974, bgs, chk); }
static inline u32 cBgS_GetTriPla_l(dBgS* bgs, u32 bg, u32 poly) { return gabi::call<u32>(0x020084C8, bgs, bg, poly); }
static inline void dBgS_RideCallBack_l(dBgS* bgs, u32 polyInfo, u32 ac) { gabi::call(0x024EFBC0, bgs, polyInfo, ac); }
static inline f32 dBgS_RoofChk_l(dBgS* bgs, u32 chk) { return gabi::call<f32>(0x024EF6E8, bgs, chk); }
static inline bool dBgS_SplGrpChk_l(dBgS* bgs, u32 chk) { return gabi::call<bool>(0x024EF7C0, bgs, chk); }
static inline void dBgS_WallCorrect_l(dBgS* bgs, dBgS_Acch* acch) { gabi::call(0x024EF5CC, bgs, acch); }
static inline s32 dBgS_GetRoomId_l(dBgS* bgs, u32 polyInfo) { return gabi::call<s32>(0x024EF130, bgs, polyInfo); }
static inline void dBgS_MoveBgCrrPos_l(dBgS* bgs, u32 polyInfo, u32 accept, u32 pos, u32 angle, u32 shape) {
    gabi::call(0x024EF968, bgs, polyInfo, accept, pos, angle, shape);
}
static inline void cBgS_LinChk_ct_l(u32 chk) { gabi::call(0x02008FEC, chk); }
static inline void cBgS_LinChk_Set2_l(u32 chk, u32 start, u32 end, u32 pid) { gabi::call(0x0200909C, chk, start, end, pid); }
static inline bool cBgS_LineCross_l(dBgS* bgs, u32 chk) { return gabi::call<bool>(0x02008860, bgs, chk); }
static inline void cBgS_LinChk_dt_l(u32 chk, s32 flags) { gabi::call(0x02008B4C, chk, flags); }
static inline void PSVECAdd_l(u32 a, u32 b, u32 dst) { gabi::call(0x028E8D88, a, b, dst); }
static inline f32 PSVECSquareDistance_l(u32 a, u32 b) { return gabi::call<f32>(0x028E8DE8, a, b); }
static inline void cBgW_GetTopUnder_l(u32 bgw, u32 top, u32 under) { gabi::call(0x0200A3FC, bgw, top, under); }
static inline BOOL daSea_ChkArea_l(f32 x, f32 z) { return gabi::call<BOOL>(0x0246B6A4, x, z); }
static inline f64 daSea_calcWave_l(f32 x, f32 z) { return gabi::call<f64>(0x0246BA0C, x, z); }
static inline void cM3dGCir_Set_l(u32 cir, f32 x, f32 y, f32 z, f32 r) { gabi::call(0x020180A8, cir, x, y, z, r); }
static inline void cM3dGCyl_Set_l(u32 cyl, u32 c, f64 r, f32 h) { gabi::call(0x0201864C, cyl, c, r, h); }
static inline BOOL cM3dGPla_getCrossYLessD_l(u32 pla, void* vec, u32 out) { return gabi::call<BOOL>(0x02018B38, pla, vec, out); }

static constexpr f32 G_CM3D_F_INF = 1000000000.0f;

static inline u32 A(const void* p) { return gabi::ea(p); }

/* dBgS_Chk::SetExtChk / dBgS_GndChk etc.: the pass-check pointers, actor id and flag byte */
static inline void SetExtChk_GndRoof(u32 dst, u32 src) {
    gabi::store<u32>(dst + 4, gabi::load<u32>(src + 4));
    gabi::store<u32>(dst + 0, gabi::load<u32>(src + 0));
    gabi::store<u32>(dst + 8, gabi::load<u32>(src + 8));
    gabi::store<u8>(dst + 0xC, gabi::load<u8>(src + 0xC));
}

/* 024EFD08: static initialisers (header statics) */
static void __sinit_d_bg_s_acch_cpp() {
    WWHD_FUNC(0x024EFD08, void, (u32)0);
    sinit_header_statics(0x1046ECF0, 0x101D51A8);
}
VERIFY(0x024EFD08, __sinit_d_bg_s_acch_cpp);

/* 024EFD9C: dBgS_Acch::~dBgS_Acch (deleting; matcher names 02515A70) */
static void dBgS_Acch_dt(dBgS_Acch* a, s32 flags) {
    WWHD_FUNC(0x024EFD9C, void, a, flags);
    if (a == nullptr) return;
    u32 b = A(a);
    gabi::store<u32>(b + 0x20, 0x10043A60);
    gabi::store<u32>(b + 0x14, 0x10043A70);
    gabi::store<u32>(b + 0x194, 0x100437B4);
    gabi::store<u32>(b + 0x198, 0x100437D4);
    gabi::store<u32>(b + 0x1A4, 0x10043734);
    gabi::call(0x02008B4C, b + 0x184, 0); /* m_wtr */
    gabi::store<u32>(b + 0x158, 0x10043734);
    gabi::store<u32>(b + 0x148, 0x10043784);
    gabi::store<u32>(b + 0x14C, 0x100437A4);
    gabi::call(0x02008B4C, b + 0x138, 0); /* m_roof */
    gabi::store<u32>(b + 0x120, 0x10043734);
    gabi::store<u32>(b + 0x114, 0x10043774);
    gabi::store<u32>(b + 0xF4, 0x10043754);
    gabi::call(0x02008DAC, b + 0xD4, 0); /* m_gnd: cBgS_Chk::~cBgS_Chk */
    gabi::store<u32>(b + 0x20, 0x10043734);
    gabi::call(0x02008B4C, b, 0);
    if (flags & 1) operator_delete(a);
}
VERIFY(0x024EFD9C, dBgS_Acch_dt);

/* 024EFE94 */
static dBgS_AcchCir* dBgS_AcchCir_ct(dBgS_AcchCir* c) {
    WWHD_FUNC(0x024EFE94, dBgS_AcchCir*, c);
    if (c == nullptr) {
        c = (dBgS_AcchCir*)operator_new(0x40);
        if (c == nullptr) return c;
    }
    u32 b = A(c);
    /* cBgS_PolyInfo */
    gabi::store<u32>(b + 0x4, 0);
    gabi::store<u32>(b + 0x10, 0);
    gabi::store<u16>(b + 0x0, 0xFFFF);
    gabi::store<u16>(b + 0x2, 0x100);
    gabi::store<u32>(b + 0x8, 0xFFFFFFFF);
    gabi::store<u32>(b + 0xC, 0x10043814);
    gabi::call(0x02018048, b + 0x14); /* cM3dGCir::cM3dGCir */
    c->m_wall_angle_y = 0;
    c->m_flags = 0;
    c->m_wall_rr = 0.0f;
    c->field_0x2c = 0.0f;
    c->m_wall_h = 0.0f;
    c->m_wall_r = 0.0f;
    c->m_wall_h_direct = 0.0f;
    return c;
}
VERIFY(0x024EFE94, dBgS_AcchCir_ct);

/* 024EFF3C */
static void dBgS_AcchCir_SetWallR(dBgS_AcchCir* c, f32 radius) {
    WWHD_FUNC(0x024EFF3C, void, c, radius);
    c->m_wall_r = radius;
}
VERIFY(0x024EFF3C, dBgS_AcchCir_SetWallR);

/* 024EFF44 */
static void dBgS_AcchCir_SetWall(dBgS_AcchCir* c, f32 height, f32 radius) {
    WWHD_FUNC(0x024EFF44, void, c, height, radius);
    c->m_wall_h = height;
    dBgS_AcchCir_SetWallR(c, radius);
}
VERIFY(0x024EFF44, dBgS_AcchCir_SetWall);

/* 024EFF50. HD: GetTriPla's result is null-checked before the plane copy. */
static void dBgS_Acch_GroundCheck(dBgS_Acch* a, dBgS* bgs) {
    WWHD_FUNC(0x024EFF50, void, a, bgs);
    if (a->m_flags & dBgS_Acch::GRND_NONE) return;
    u32 b = A(a);
    cXyz* p = a->pm_pos;
    f32 x = p->x, y = p->y, z = p->z;
    f32 t = gabi::fsubs_ppc(a->m_ground_check_offset, a->m_ground_up_h);
    t = gabi::fadds_ppc(t, a->m_ground_up_h_diff);
    y = gabi::fadds_ppc(y, t);
    a->m_ground_up_h_diff = 0.0f;
    gabi::store<f32>(b + 0xF8, x); /* m_gnd.SetPos */
    gabi::store<f32>(b + 0xFC, y);
    gabi::store<f32>(b + 0x100, z);
    f32 gh = cBgS_GroundCross_l(bgs, b + 0xD4);
    a->m_ground_h = gh;
    if (gh != -G_CM3D_F_INF) {
        f32 h = gabi::fadds_ppc(gh, a->m_ground_up_h);
        a->field_0xb8 = h;
        if (h > a->field_0xb4) {
            a->pm_pos->y = h;
            if (a->pm_speed.v != 0) a->pm_speed->y = 0.0f;
            u32 pla = cBgS_GetTriPla_l(bgs, gabi::load<u16>(b + 0xEA), gabi::load<u16>(b + 0xE8));
            if (pla != 0) {
                gabi::store<u32>(b + 0x9C, gabi::load<u32>(pla + 0));
                gabi::store<u32>(b + 0xA0, gabi::load<u32>(pla + 4));
                gabi::store<u32>(b + 0xA4, gabi::load<u32>(pla + 8));
                gabi::store<f32>(b + 0xA8, gabi::load<f32>(pla + 0xC));
            }
            a->m_flags |= dBgS_Acch::GROUND_FIND | dBgS_Acch::GROUND_HIT;
            dBgS_RideCallBack_l(bgs, b + 0xE8, a->m_my_ac.v);
            if (!a->field_0xb0) a->m_flags |= dBgS_Acch::GROUND_LANDING;
        }
    }
    if (a->field_0xb0 && !(a->m_flags & dBgS_Acch::GROUND_HIT)) a->m_flags |= dBgS_Acch::GROUND_AWAY;
}
VERIFY(0x024EFF50, dBgS_Acch_GroundCheck);

/* 024F00CC. HD: the line check is a plain cBgS_LinChk (0x58) without the SetSttsGroundOff, and
 * GetTriPla's result is null-checked (null: treated as ground). */
static void dBgS_Acch_LineCheck(dBgS_Acch* a, dBgS* bgs) {
    WWHD_FUNC(0x024F00CC, void, a, bgs);
    u32 b = A(a);
    /* one frame slot each, reused by every iteration (not re-initialised) */
    struct LinChk_l { u8 _[0x58]; };
    gabi::Local<LinChk_l> linChk;
    gabi::Local<cXyz> old_pos, pos;
    for (s32 i = 0; i < a->m_tbl_size; i++) {
        u32 l = A(linChk.get());
        cBgS_LinChk_ct_l(l);
        cXyz* po = a->pm_old_pos;
        old_pos->x = po->x;
        f32 oy = po->y;
        old_pos->y = oy;
        old_pos->z = po->z;
        cXyz* pp = a->pm_pos;
        pos->x = pp->x;
        f32 py = pp->y;
        pos->y = py;
        pos->z = pp->z;
        dBgS_AcchCir* cir = a->pm_acch_cir.get();
        old_pos->y = gabi::fadds_ppc(oy, cir[i].m_wall_h);
        pos->y = gabi::fadds_ppc(py, cir[i].m_wall_h);
        cBgS_LinChk_Set2_l(l, A(old_pos.get()), A(pos.get()), gabi::load<u32>(b + 8));
        /* SetExtChk */
        gabi::store<u32>(l + 0, gabi::load<u32>(b + 0));
        gabi::store<u8>(l + 0xC, gabi::load<u8>(b + 0xC));
        gabi::store<u32>(l + 8, gabi::load<u32>(b + 8));
        gabi::store<u32>(l + 4, gabi::load<u32>(b + 4));
        if (cBgS_LineCross_l(bgs, l)) {
            gabi::at<cXyz>(a->pm_pos.v)->copy(*gabi::at<cXyz>(l + 0x30)); /* GetCross */
            a->m_flags |= dBgS_Acch::LINE_CHECK_HIT;
            u32 out = a->pm_out_poly_info;
            if (out != 0) { /* SetPolyInfo */
                gabi::store<u16>(out + 0, gabi::load<u16>(l + 0x14));
                gabi::store<u16>(out + 2, gabi::load<u16>(l + 0x16));
                gabi::store<u32>(out + 4, gabi::load<u32>(l + 0x18));
                gabi::store<u32>(out + 8, gabi::load<u32>(l + 0x1C));
            }
            u32 pla = cBgS_GetTriPla_l(bgs, gabi::load<u16>(l + 0x16), gabi::load<u16>(l + 0x14));
            if (pla != 0 && gabi::load<f32>(pla + 4) < 0.5f) { /* !cBgW_CheckBGround */
                PSVECAdd_l(a->pm_pos.v, pla, a->pm_pos.v);
                f32 nz = gabi::load<f32>(pla + 8);
                f32 nx = gabi::load<f32>(pla + 0);
                f64 len = gabi::call<f64>(0x028F4384, gabi::fmadds(nx, nx, nz * nz)); /* std::sqrtf */
                if (!(std::fabs(len) < (f64)3.814697265625e-06f)) { /* !cM3d_IsZero */
                    dBgS_AcchCir& c = a->pm_acch_cir.get()[i];
                    c.m_flags |= dBgS_AcchCir::WALL_H_DIRECT; /* SetWallHDirect */
                    c.m_wall_h_direct = a->pm_pos->y;
                }
                dBgS_AcchCir& c = a->pm_acch_cir.get()[i];
                cXyz* q = a->pm_pos;
                q->y = gabi::fsubs_ppc(q->y, c.m_wall_h);
            } else {
                cXyz* q = a->pm_pos;
                q->y = gabi::fsubs_ppc(q->y, 1.0f);
                dBgS_Acch_GroundCheck(a, bgs);
            }
        }
        gabi::store<u32>(l + 0x20, 0x10043724);
        cBgS_LinChk_dt_l(l, 0);
    }
}
VERIFY(0x024F00CC, dBgS_Acch_LineCheck);

/* 024F034C */
static void dBgS_Acch_GroundCheckInit(dBgS_Acch* a, dBgS* bgs) {
    WWHD_FUNC(0x024F034C, void, a, bgs);
    a->m_ground_h = -G_CM3D_F_INF;
    if (a->m_flags & dBgS_Acch::GRND_NONE) return;
    SetExtChk_GndRoof(A(a) + 0xD4, A(a));
    u32 f = a->m_flags;
    a->field_0xb0 = (f >> 5) & 1; /* ChkGroundHit */
    a->m_flags = f & ~(u32)(dBgS_Acch::GROUND_HIT | dBgS_Acch::GROUND_LANDING | dBgS_Acch::GROUND_AWAY);
}
VERIFY(0x024F034C, dBgS_Acch_GroundCheckInit);

/* roof check of CrrPos / GroundRoofProc: SetExtChk, ClrRoofHit, SetPos, RoofChk */
static inline f32 RoofCheck(dBgS_Acch* a, dBgS* bgs) {
    u32 b = A(a);
    SetExtChk_GndRoof(b + 0x138, b);
    a->m_flags &= ~(u32)dBgS_Acch::ROOF_HIT;
    cXyz* p = a->pm_pos;
    f32 y = p->y, x = p->x, z = p->z;
    gabi::store<f32>(b + 0x160, x);
    gabi::store<f32>(b + 0x164, y);
    gabi::store<f32>(b + 0x168, z);
    return dBgS_RoofChk_l(bgs, b + 0x128);
}

/* 024F03A0 */
static void dBgS_Acch_GroundRoofProc(dBgS_Acch* a, dBgS* bgs) {
    WWHD_FUNC(0x024F03A0, void, a, bgs);
    if (a->m_ground_h != -G_CM3D_F_INF) {
        f32 c4 = a->field_0xC4;
        if (a->field_0xb8 < c4) {
            cXyz* p = a->pm_pos;
            if (c4 < p->y) p->y = c4;
        }
        if (!(a->m_flags & dBgS_Acch::ROOF_NONE)) {
            if (!(a->m_ground_h < a->m_roof_height)) {
                a->m_roof_height = RoofCheck(a, bgs);
            }
        }
    }
}
VERIFY(0x024F03A0, dBgS_Acch_GroundRoofProc);

/* 024F0474 */
static dBgS_Acch* dBgS_Acch_ct(dBgS_Acch* a) {
    WWHD_FUNC(0x024F0474, dBgS_Acch*, a);
    if (a == nullptr) {
        a = (dBgS_Acch*)operator_new(0x1C4);
        if (a == nullptr) return a;
    }
    u32 b = A(a);
    gabi::call(0x02008B60, b); /* cBgS_Chk::cBgS_Chk */
    gabi::store<u8>(b + 0x1C, 0);
    gabi::store<u8>(b + 0x19, 0);
    gabi::store<u32>(b + 0x20, 0x10043A60);
    gabi::store<u32>(b + 0x10, 0x10043A50);
    gabi::store<u32>(b + 0x30, 0);
    gabi::store<u8>(b + 0x1B, 0);
    gabi::store<u32>(b + 0x58, 0x10043704);
    gabi::store<u32>(b + 0x2C, 0);
    gabi::store<u32>(b + 0x28, 0);
    gabi::store<u32>(b + 0x24, 1);
    gabi::store<u8>(b + 0x1A, 0);
    gabi::store<u8>(b + 0x1D, 0);
    gabi::store<u32>(b + 0x3C, 0);
    gabi::store<u8>(b + 0x1E, 0);
    gabi::store<u32>(b + 0x34, 0);
    gabi::store<u32>(b + 0x38, 0);
    gabi::store<u32>(b + 0x14, 0x10043A70);
    gabi::store<u8>(b + 0x18, 0);
    gabi::call(0x02018590, b + 0x5C); /* cM3dGCyl::cM3dGCyl */
    gabi::store<u32>(b + 0x80, 0);
    gabi::store<u32>(b + 0x78, 0);
    gabi::store<u32>(b + 0x7C, 0);
    gabi::store<f32>(b + 0x8C, 0.0f);
    gabi::store<f32>(b + 0x98, 0.0f);
    gabi::store<f32>(b + 0x94, 0.0f);
    gabi::store<u32>(b + 0x88, 0);
    gabi::store<u32>(b + 0x74, 0);
    gabi::store<u32>(b + 0x84, 0);
    gabi::store<f32>(b + 0x90, 0.0f);
    gabi::call(0x020189A0, b + 0x9C); /* cM3dGPla::cM3dGPla */
    gabi::store<f32>(b + 0xC4, 0.0f);
    gabi::store<u32>(b + 0xCC, 0);
    gabi::store<f32>(b + 0xB8, 0.0f);
    gabi::store<f32>(b + 0xC0, 0.0f);
    gabi::store<u8>(b + 0xB0, 0);
    gabi::store<f32>(b + 0xD0, 0.0f);
    gabi::store<f32>(b + 0xB4, 0.0f);
    gabi::store<f32>(b + 0xC8, 0.0f);
    gabi::store<f32>(b + 0xBC, 0.0f);
    gabi::call(0x02008E0C, b + 0xD4); /* cBgS_GndChk::cBgS_GndChk */
    gabi::store<u8>(b + 0x11D, 0);
    gabi::store<u8>(b + 0x11E, 0);
    gabi::store<u32>(b + 0xD4, b + 0x114);
    gabi::store<u32>(b + 0x124, 1);
    gabi::store<u8>(b + 0x11B, 0);
    gabi::store<u32>(b + 0x114, 0x10043774);
    gabi::store<u8>(b + 0x119, 0);
    gabi::store<u32>(b + 0xE4, 0x10043744);
    gabi::store<u32>(b + 0x120, 0x10043764);
    gabi::store<u8>(b + 0x11A, 0);
    gabi::store<u32>(b + 0xF4, 0x10043754);
    gabi::store<u8>(b + 0x118, 0);
    gabi::store<u8>(b + 0x11C, 0);
    gabi::store<u32>(b + 0xD8, b + 0x120);
    gabi::call(0x024EE7AC, b + 0x128); /* dBgS_RoofChk::dBgS_RoofChk */
    gabi::call(0x024F22DC, b + 0x174); /* dBgS_WtrChk::dBgS_WtrChk */
    /* body */
    a->pm_acch_cir = nullptr;
    a->field_0xC4 = 0.0f;
    gabi::store<u32>(b + 0x0, b + 0x14);  /* SetPolyPassChk */
    a->pm_speed = nullptr;
    a->field_0xb8 = 0.0f;
    a->m_sea_height = -G_CM3D_F_INF;
    a->field_0xb4 = 0.0f;
    gabi::store<u32>(b + 0x4, b + 0x20);  /* SetGrpPassChk */
    a->pm_angle = 0;
    a->m_roof_crr_height = 0.0f;
    a->m_ground_check_offset = 60.0f;
    a->m_wtr_check_offset = 200.0f;
    a->field_0xb0 = 0;
    a->pm_shape_angle = 0;
    a->m_ground_up_h = 0.0f;
    a->m_roof_height = 0.0f;
    a->pm_old_pos = nullptr;
    a->pm_pos = nullptr;
    a->m_tbl_size = 0;
    a->m_ground_up_h_diff = 0.0f;
    a->m_flags = dBgS_Acch::ROOF_NONE;
    a->pm_out_poly_info = 0;
    a->m_my_ac = nullptr;
    a->m_ground_h = -G_CM3D_F_INF;
    return a;
}
VERIFY(0x024F0474, dBgS_Acch_ct);

/* 024F06B4: the 9th argument (shape angle) is on the stack */
static void dBgS_Acch_Set(dBgS_Acch* a, cXyz* pos, cXyz* old_pos, fopAc_ac_c* actor, s32 tbl_size, dBgS_AcchCir* cir,
                          cXyz* speed, csXyz* angle) {
    WWHD_FUNC(0x024F06B4, void, a, pos, old_pos, actor, tbl_size, cir, speed, angle);
    u32 shape_angle = gabi::load<u32>(gabi::cpu->r[1] + 8);
    a->pm_pos = pos;
    a->pm_old_pos = old_pos;
    if (a->pm_pos.v == 0) JUT_ASSERT_fail(STR(0x1004383C), 0x10B, STR(0x1004384C));
    if (a->pm_old_pos.v == 0) JUT_ASSERT_fail(STR(0x1004383C), 0x10C, STR(0x1004385C));
    a->m_my_ac = actor;
    u32 pid = actor != nullptr ? gabi::load<u32>(A(actor) + 4) : 0xFFFFFFFF;
    a->pm_angle = A(angle);
    gabi::store<u32>(A(a) + 8, pid); /* SetActorPid */
    a->pm_acch_cir = cir;
    a->pm_shape_angle = shape_angle;
    a->pm_speed = speed;
    a->m_tbl_size = tbl_size;
}
VERIFY(0x024F06B4, dBgS_Acch_Set);

/* 024F0768 */
static f32 dBgS_Acch_GetWallAllLowH_R(dBgS_Acch* a) {
    WWHD_FUNC(0x024F0768, f32, a);
    s32 n = a->m_tbl_size;
    if (!(n > 0)) return 0.0f;
    dBgS_AcchCir* cir = a->pm_acch_cir.get();
    s32 bestWall = 0;
    f32 min = cir[0].m_wall_h;
    for (s32 i = 1; i < n; i++) {
        if (min > cir[i].m_wall_h) {
            min = cir[i].m_wall_h;
            bestWall = i;
        }
    }
    return cir[bestWall].m_wall_r;
}
VERIFY(0x024F0768, dBgS_Acch_GetWallAllLowH_R);

/* 024F07D4 */
static f32 dBgS_Acch_GetWallAllLowH(dBgS_Acch* a) {
    WWHD_FUNC(0x024F07D4, f32, a);
    s32 n = a->m_tbl_size;
    if (!(n > 0)) return 0.0f;
    dBgS_AcchCir* cir = a->pm_acch_cir.get();
    f32 min = cir[0].m_wall_h;
    for (s32 i = 1; i < n; i++) {
        if (min > cir[i].m_wall_h) min = cir[i].m_wall_h;
    }
    return min;
}
VERIFY(0x024F07D4, dBgS_Acch_GetWallAllLowH);

/* 024F0824. HD: ClrWallHit of a circle also clears its poly info. */
static void dBgS_Acch_Init(dBgS_Acch* a) {
    WWHD_FUNC(0x024F0824, void, a);
    a->m_flags &= ~(u32)dBgS_Acch::WALL_HIT;
    for (s32 i = 0; i < a->m_tbl_size; i++) {
        dBgS_AcchCir& c = a->pm_acch_cir.get()[i];
        u32 b = A(&c);
        gabi::store<u16>(b + 0, 0xFFFF); /* ClearPi */
        gabi::store<u16>(b + 2, 0x100);
        c.m_flags &= ~(u32)dBgS_AcchCir::WALL_HIT;
        gabi::store<u32>(b + 4, 0);
        gabi::store<u32>(b + 8, 0xFFFFFFFF);
        dBgS_AcchCir& c2 = a->pm_acch_cir.get()[i];
        c2.m_flags &= ~(u32)dBgS_AcchCir::WALL_H_DIRECT;
    }
}
VERIFY(0x024F0824, dBgS_Acch_Init);

/* CHECK_PVEC3_RANGE / isnan asserts of CrrPos (HD keeps them; isnan tests the stored bits) */
static inline bool is_nan_bits(f32 v) {
    u32 bits;
    memcpy(&bits, &v, 4);
    return (u32)(bits << 1) > 0xFF000000u;
}
static inline void CheckPos(dBgS_Acch* a, u32 lx, u32 ly, u32 lz, u32 lr) {
    const u32 file = 0x10043880;
    if (is_nan_bits(a->pm_pos->x)) JUT_ASSERT_fail(STR(file), lx, STR(0x100438F0));
    if (is_nan_bits(a->pm_pos->y)) JUT_ASSERT_fail(STR(file), ly, STR(0x10043934));
    if (is_nan_bits(a->pm_pos->z)) JUT_ASSERT_fail(STR(file), lz, STR(0x10043978));
    cXyz* p = a->pm_pos;
    const f32 lo = -1.0e32f, hi = 1.0e32f;
    f32 x = p->x;
    bool bad = !(lo < x && x < hi);
    if (!bad) {
        f32 y = p->y;
        bad = !(lo < y && y < hi);
        if (!bad) {
            f32 z = p->z;
            bad = !(lo < z && z < hi);
        }
    }
    if (bad) JUT_ASSERT_fail(STR(file), lr, STR(0x100439BC));
}

/* 024F08A8 */
static void dBgS_Acch_CrrPos(dBgS_Acch* a, dBgS* bgs) {
    WWHD_FUNC(0x024F08A8, void, a, bgs);
    if (a->m_flags & 1) return;
    u32 b = A(a);
    if (a->pm_pos.v == 0) JUT_ASSERT_fail(STR(0x10043880), 0x21F, STR(0x10043890));
    if (a->pm_old_pos.v == 0) JUT_ASSERT_fail(STR(0x10043880), 0x220, STR(0x100438A0));
    CheckPos(a, 0x248, 0x249, 0x24A, 0x24C);

    u32 pos0 = a->pm_pos.v;
    u32 angle = a->pm_angle;
    u32 shape = a->pm_shape_angle;
    dBgS_MoveBgCrrPos_l(bgs, b + 0xE8, (a->m_flags >> 5) & 1, pos0, angle, shape);

    dBgS_Acch_GroundCheckInit(a, bgs);
    dBgS_Acch_Init(a);

    f32 lowH_R = dBgS_Acch_GetWallAllLowH_R(a);
    gabi::Local<cXyz> oldXZ, posXZ; /* abs2XZ temporaries */
    cXyz* op = a->pm_old_pos;
    cXyz* pp = a->pm_pos;
    oldXZ->x = op->x;
    oldXZ->z = op->z;
    oldXZ->y = 0.0f;
    posXZ->y = 0.0f;
    posXZ->z = pp->z;
    posXZ->x = pp->x;
    f32 distXZ2 = PSVECSquareDistance_l(A(oldXZ.get()), A(posXZ.get()));
    f32 posY = a->pm_pos->y;
    f32 distY = a->pm_old_pos->y - posY;
    f32 lowH = dBgS_Acch_GetWallAllLowH(a);
    u32 f = a->m_flags & ~(u32)dBgS_Acch::LINE_CHECK_HIT; /* OffLineCheckHit */
    a->field_0xb4 = posY;
    f32 temp8 = gabi::fadds_ppc(a->pm_pos->y, a->m_ground_check_offset);
    f32 temp7 = gabi::fadds_ppc(a->pm_old_pos->y, lowH);
    a->m_flags = f;
    bool ranLineCheck = false;
    if (!(f & dBgS_Acch::LINE_CHECK_NONE) && !(std::fabs(lowH_R) < 3.814697265625e-06f)) {
        if (distXZ2 > lowH_R * lowH_R || temp7 > temp8 || distY > a->m_ground_check_offset ||
            (f & dBgS_Acch::LINE_CHECK)) {
            ranLineCheck = true;
            dBgS_Acch_LineCheck(a, bgs);
            f = a->m_flags;
        }
    }
    if (!(f & dBgS_Acch::WALL_NONE)) {
        dBgS_WallCorrect_l(bgs, a);
        f = a->m_flags;
    }
    if ((f & dBgS_Acch::WALL_HIT) && ranLineCheck) dBgS_Acch_LineCheck(a, bgs);

    f = a->m_flags;
    a->field_0xC4 = G_CM3D_F_INF;
    if (!(f & dBgS_Acch::ROOF_NONE)) {
        f32 rh = RoofCheck(a, bgs);
        a->m_roof_height = rh;
        bool hit = false;
        if (rh != G_CM3D_F_INF) {
            f32 crr = a->m_roof_crr_height;
            if (gabi::fadds_ppc(a->pm_pos->y, crr) > rh) {
                f = a->m_flags | dBgS_Acch::ROOF_HIT;
                a->m_flags = f;
                a->field_0xC4 = gabi::fsubs_ppc(rh, crr);
                hit = true;
            }
        }
        if (!hit) f = a->m_flags;
    }
    if (!(f & dBgS_Acch::GRND_NONE)) {
        a->m_flags = f & ~(u32)dBgS_Acch::GROUND_FIND;
        dBgS_Acch_GroundCheck(a, bgs);
        dBgS_Acch_GroundRoofProc(a, bgs);
        f = a->m_flags;
    } else {
        cXyz* p = a->pm_pos;
        f32 c4 = a->field_0xC4;
        if (c4 < p->y) {
            p->y = c4;
            f = a->m_flags;
        }
    }

    if (!(f & dBgS_Acch::WATER_NONE)) {
        gabi::store<f32>(b + 0x1BC, -G_CM3D_F_INF); /* m_wtr.SetHeight */
        a->m_flags = f & ~(u32)(dBgS_Acch::WATER_HIT | dBgS_Acch::WATER_IN);
        s32 room_no = dBgS_GetRoomId_l(bgs, b + 0xE8);
        if (a->m_ground_h != -G_CM3D_F_INF && (u32)room_no < 64) {
            dComIfGp_get();
            u32 bgw = gabi::load<u32>(0x1047E8F4 + room_no * 0x22C); /* dComIfGp_roomControl_getBgW */
            if (bgw != 0) {
                gabi::Local<be<f32>> top, under;
                cBgW_GetTopUnder_l(bgw, A(top.get()), A(under.get()));
                cXyz* p = a->pm_pos;
                u32 gx = gabi::load<u32>(A(p) + 0), gz = gabi::load<u32>(A(p) + 8);
                f32 fUnder = *under;
                /* m_wtr.Set(ground, top), SetPassChkInfo */
                for (u32 k = 0; k < 7; k++) gabi::store<u8>(b + 0x19C + k, gabi::load<u8>(b + 0x18 + k));
                gabi::store<f32>(b + 0x1B0, fUnder);
                gabi::store<f32>(b + 0x1AC, gabi::f32_from_bits(gx));
                gabi::store<f32>(b + 0x1B8, (f32)*top);
                gabi::store<f32>(b + 0x1B4, gabi::f32_from_bits(gz));
                if (dBgS_SplGrpChk_l(bgs, b + 0x174)) {
                    a->m_flags |= dBgS_Acch::WATER_HIT;
                    f32 h = gabi::load<f32>(b + 0x1BC);
                    if (h > a->pm_pos->y) a->m_flags |= dBgS_Acch::WATER_IN;
                    if (h < fUnder) {
                        JUT_ASSERT_fail(STR(0x10043880), 0x2FF, STR(0x100438B4));
                        h = gabi::load<f32>(b + 0x1BC);
                    }
                    if (h > (f32)*top) JUT_ASSERT_fail(STR(0x10043880), 0x300, STR(0x100438D4));
                }
            }
        }
        f = a->m_flags;
    }

    if (f & dBgS_Acch::SEA_CHECK) {
        a->m_sea_height = -G_CM3D_F_INF;
        a->m_flags = f & ~(u32)dBgS_Acch::SEA_IN;
        f64 sea;
        cXyz* p = a->pm_pos;
        if (daSea_ChkArea_l(p->x, p->z)) {
            p = a->pm_pos;
            sea = daSea_calcWave_l(p->x, p->z);
            a->m_sea_height = (f32)sea;
        } else {
            sea = a->m_sea_height;
        }
        if (a->m_flags & dBgS_Acch::SEA_WATER_HEIGHT) {
            f32 wtr = gabi::load<f32>(b + 0x1BC);
            if (sea < wtr) {
                sea = wtr;
                a->m_sea_height = wtr;
            }
        }
        if (sea > a->pm_pos->y) a->m_flags |= dBgS_Acch::SEA_IN;
    }

    CheckPos(a, 0x33D, 0x33E, 0x33F, 0x341);
}
VERIFY(0x024F08A8, dBgS_Acch_CrrPos);

/* 024F0F98 */
static bool dBgS_Acch_GetOnePolyInfo(dBgS_Acch* a, u8* dst) {
    WWHD_FUNC(0x024F0F98, bool, a, dst);
    u32 d = A(dst), b = A(a);
    auto setPolyInfo = [d](u32 s) {
        gabi::store<u16>(d + 0, gabi::load<u16>(s + 0));
        gabi::store<u16>(d + 2, gabi::load<u16>(s + 2));
        gabi::store<u32>(d + 4, gabi::load<u32>(s + 4));
        gabi::store<u32>(d + 8, gabi::load<u32>(s + 8));
    };
    u32 f = a->m_flags;
    if (f & dBgS_Acch::GROUND_HIT) {
        setPolyInfo(b + 0xE8);
        return false;
    }
    if (f & dBgS_Acch::WALL_HIT) {
        for (s32 i = 0; i < a->m_tbl_size; i++) {
            dBgS_AcchCir& c = a->pm_acch_cir.get()[i];
            if (c.ChkWallHit()) setPolyInfo(A(&c));
        }
        return false;
    }
    if (f & dBgS_Acch::ROOF_HIT) {
        setPolyInfo(b + 0x128);
        return false;
    }
    return true;
}
VERIFY(0x024F0F98, dBgS_Acch_GetOnePolyInfo);

/* 024F1070 */
static f32 dBgS_Acch_GetWallAllR(dBgS_Acch* a) {
    WWHD_FUNC(0x024F1070, f32, a);
    f32 max = 0.0f;
    s32 n = a->m_tbl_size;
    dBgS_AcchCir* cir = n > 0 ? a->pm_acch_cir.get() : nullptr;
    for (s32 i = 0; i < n; i++) {
        if (max < cir[i].m_wall_r) max = cir[i].m_wall_r;
    }
    return max;
}
VERIFY(0x024F1070, dBgS_Acch_GetWallAllR);

/* 024F10A8 */
static void dBgS_Acch_SetWallCir(dBgS_Acch* a) {
    WWHD_FUNC(0x024F10A8, void, a);
    for (s32 i = 0; i < a->m_tbl_size; i++) {
        dBgS_AcchCir& c = a->pm_acch_cir.get()[i];
        cXyz* p = a->pm_pos;
        f32 y = gabi::fadds_ppc(p->y, c.m_wall_h);
        cM3dGCir_Set_l(A(&c) + 0x14, p->x, p->z, y, c.m_wall_r); /* SetCir */
    }
}
VERIFY(0x024F10A8, dBgS_Acch_SetWallCir);

/* 024F1134 */
static void dBgS_Acch_CalcWallBmdCyl(dBgS_Acch* a) {
    WWHD_FUNC(0x024F1134, void, a);
    s32 n = a->m_tbl_size;
    if (!(n > 0)) {
        cM3dGCyl_Set_l(A(a) + 0x5C, a->pm_pos.v, 0.0, 0.0f);
        return;
    }
    f64 all_r = gabi::call<f64>(0x024F1070, a); /* GetWallAllR (f1 passed on as is) */
    dBgS_AcchCir* cir = a->pm_acch_cir.get();
    f32 min_h = cir[0].m_wall_h;
    f32 max_h = min_h;
    for (s32 i = 0; i < n; i++) {
        if (min_h > cir[i].m_wall_h) min_h = cir[i].m_wall_h;
        if (max_h < cir[i].m_wall_h) max_h = cir[i].m_wall_h;
    }
    gabi::Local<cXyz> center;
    cXyz* p = a->pm_pos;
    center->x = p->x;
    f32 y = p->y;
    center->z = p->z;
    center->y = gabi::fadds_ppc(y, min_h);
    cM3dGCyl_Set_l(A(a) + 0x5C, A(center.get()), all_r, gabi::fsubs_ppc(max_h, min_h));
}
VERIFY(0x024F1134, dBgS_Acch_CalcWallBmdCyl);

/* 024F1204: HD signature GetWallAddY(Vec&) */
static f32 dBgS_Acch_GetWallAddY(dBgS_Acch* a, void* vec) {
    WWHD_FUNC(0x024F1204, f32, a, vec);
    if (!(a->m_flags & dBgS_Acch::GROUND_FIND)) return 0.0f;
    if (gabi::load<f32>(A(a) + 0xA0) < 0.5f) return 0.0f;
    gabi::Local<be<f32>> cross_y;
    if (!cM3dGPla_getCrossYLessD_l(A(a) + 0x9C, vec, A(cross_y.get()))) return 0.0f;
    f32 c = *cross_y;
    if (c > 0.0f) c = 0.0f;
    return -c;
}
VERIFY(0x024F1204, dBgS_Acch_GetWallAddY);

/* 024F12A8 */
static void dBgS_Acch_SetGroundUpY(dBgS_Acch* a, f32 y) {
    WWHD_FUNC(0x024F12A8, void, a, y);
    f32 old = a->m_ground_up_h;
    a->m_ground_up_h = y;
    a->m_ground_up_h_diff = gabi::fsubs_ppc(y, old);
}
VERIFY(0x024F12A8, dBgS_Acch_SetGroundUpY);

/* 024F12BC: the TU's second header-statics initializer:
 * the same header statics block at 1046ED0C (records 101D524C) */
static void __sinit_d_bg_s_acch_cpp_2() {
    WWHD_FUNC(0x024F12BC, void);
    sinit_header_statics(0x1046ED0C, 0x101D524C);
}
VERIFY(0x024F12BC, __sinit_d_bg_s_acch_cpp_2);

}  // namespace d_bg_s_acch_cpp

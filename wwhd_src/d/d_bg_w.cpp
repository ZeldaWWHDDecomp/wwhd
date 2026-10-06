/* d_bg_w: background collision mesh of one model (dBgW, derived from cBgW), WWHD.
 *
 * Translation unit 024F23F4..<024F49E8: the GameCube d_bg_w.cpp functions in source order (no WallCrrPos
 * family in HD) with CrrPos/TransPos (024F4484/024F44A4), then __sinit (024F4944; the init table 1018B5C4 lists it
 * between d_bg_s_wtr_chk's 024F2360 and d_bg_w_deform's 024F4A78: here a TU's __sinit follows its functions, as
 * the d_bg_s_wtr_chk / d_bg_w_deform attributions also have it) and the out-of-line copy of the inline virtual
 * dBgW::MatrixCrrPos (024F49D8). d_bg_w_deform starts at 024F49E8. cBgW is its own TU (c_bg_w,
 * 02009380..).
 *
 * HD layout: dBgW is 0xBC as on GameCube (constructor allocates 0xBC); element strides pm_tri 0x18,
 * pm_rwg 8, pm_grp 0x20 (aab at +0), m_nt_tbl 0x1C, cBgD_Tri_t 0xA, cBgD_Grp_t 0x34, cBgD_Tree_t 0x14,
 * cBgW_BlkElm 6 (roof, wall, ground). The inline asserts of the GameCube headers are present (per-instance
 * strings). dBgW vtable 10044018 (no destructor entry: 028F036C). */
#include "bindings.h"

namespace d_bg_w_cpp {

/* ---- local layouts (SHARED-CANDIDATE) ---- */

struct cBgD_t_l {
    /* 0x00 */ be<s32> m_v_num;
    /* 0x04 */ be<u32> m_v_tbl;
    /* 0x08 */ be<s32> m_t_num;
    /* 0x0C */ be<u32> m_t_tbl;     /* cBgD_Tri_t, stride 0xA: vtx0, vtx1, vtx2, id, grp */
    /* 0x10 */ be<s32> m_b_num;
    /* 0x14 */ be<u32> m_b_tbl;
    /* 0x18 */ be<s32> m_tree_num;
    /* 0x1C */ be<u32> m_tree_tbl;  /* cBgD_Tree_t, stride 0x14: flag, parent, child[8] / block */
    /* 0x20 */ be<s32> m_g_num;
    /* 0x24 */ be<u32> m_g_tbl;     /* cBgD_Grp_t, stride 0x34: next sibling +0x26, first child +0x28, tree +0x2E, info +0x30 */
    /* 0x28 */ be<s32> m_ti_num;
    /* 0x2C */ be<u32> m_ti_tbl;    /* cBgD_Ti_t, stride 0x10: info0..info3 */
};
WWHD_OFFSET(cBgD_t_l, m_ti_tbl, 0x2C);

struct dBgW_l {
    /* 0x00 */ be<u32> m_id;
    /* 0x04 */ be<u32> __vtbl;      /* ChkPolyThrough +0x2C, ChkGrpThrough +0x3C */
    /* 0x08 */ u8 _08[0x80];
    /* 0x88 */ be<u32> pm_tri;      /* cBgW_TriElm: plane at +0, stride 0x18 */
    /* 0x8C */ be<u32> pm_rwg;      /* cBgW_RwgElm: next at +0, stride 8 */
    /* 0x90 */ be<u32> pm_vtx_tbl;  /* stride 0xC */
    /* 0x94 */ be<u32> pm_bgd;     /* cBgD_t */
    /* 0x98 */ be<u32> pm_blk;      /* cBgW_BlkElm: roof +0, wall +2, ground +4, stride 6 */
    /* 0x9C */ be<u32> pm_grp;      /* cBgW_GrpElm: aab at +0, stride 0x20 */
    /* 0xA0 */ be<u32> m_nt_tbl;    /* cBgW_NodeTree: aab at +0, stride 0x1C */
    /* 0xA4 */ be<s32> m_rootGrpIdx;
    /* 0xA8 */ be<u32> m_crr_func;
    /* 0xAC */ be<s16> mOldRotY;
    /* 0xAE */ be<s16> mRotYDelta;
    /* 0xB0 */ be<u32> mpRideCb;
    /* 0xB4 */ be<u32> mpPushPullCb;
    /* 0xB8 */ be<u16> mRoomId;
    /* 0xBA */ be<u8> mFlag;
    /* 0xBB */ be<u8> mGrpRoomInf;
};
WWHD_OFFSET(dBgW_l, pm_tri, 0x88);
WWHD_OFFSET(dBgW_l, m_crr_func, 0xA8);
WWHD_SIZE(dBgW_l, 0xBC);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void cBgW_ct_l(dBgW_l* w) { gabi::call(0x020094A0, w); }
static inline bool cBgW_Set_l(dBgW_l* w, u32 bgd, u32 flag, u32 mtx) { return gabi::call<bool>(0x0200A030, w, bgd, flag, mtx); }
static inline void cBgW_Move_l(dBgW_l* w) { gabi::call(0x02009C84, w); }
static inline u32 dBgS_GetRoomPathPntNo_l(u32 info2) { return gabi::call<u32>(0x024EE650, info2); }
static inline void dBgS_ChangeAttributeCode_l(u32 code, u32 dst) { gabi::call(0x024EE638, code, dst); }
static inline bool cM3d_Cross_AabCyl_l(u32 aab, u32 cyl) { return gabi::call<bool>(0x02010E14, aab, cyl); }
static inline bool cM3d_Cross_AabSph_l(u32 aab, u32 sph) { return gabi::call<bool>(0x020119B0, aab, sph); }
static inline bool cM3dGAab_CrossY_l(u32 aab, u32 pos) { return gabi::call<bool>(0x02017D3C, aab, pos); }
static inline bool cM3dGAab_UnderPlaneYUnder_l(u32 aab, f32 y) { return gabi::call<bool>(0x02017D84, aab, y); }
static inline bool cM3dGAab_TopPlaneYUnder_l(u32 aab, f32 y) { return gabi::call<bool>(0x02017D98, aab, y); }
static inline bool cM3dGPla_getCrossY_l(u32 pla, u32 pos, u32 y) { return gabi::call<bool>(0x02018AE4, pla, pos, y); }
static inline bool cM3d_CrossY_Tri_l(u32 a, u32 b, u32 c, u32 pla, u32 pnt) { return gabi::call<bool>(0x020122A8, a, b, c, pla, pnt); }
static inline void cM3dGTri_ct_l(u32 tri) { gabi::call(0x02019040, tri); }
static inline void cM3dGTri_setBg_l(u32 tri, u32 a, u32 b, u32 c, u32 pla) { gabi::call(0x020192AC, tri, a, b, c, pla); }
static inline bool cM3d_Cross_SphTri_l(u32 sph, u32 tri, u32 out) { return gabi::call<bool>(0x02014200, sph, tri, out); }
static inline f32 dBgS_Acch_GetWallAddY_l(dBgS_Acch* a, u32 vec, s32 i) { return gabi::call<f32>(0x024F1204, a, vec, i); }
static inline void dBgS_Acch_SetWallCir_l(dBgS_Acch* a) { gabi::call(0x024F10A8, a); }
static inline void dBgS_Acch_CalcWallBmdCyl_l(dBgS_Acch* a) { gabi::call(0x024F1134, a); }
static inline void cM3dGLin_SetStartEnd_l(u32 lin, u32 start, u32 end) { gabi::call(0x02018808, lin, start, end); }
static inline bool cM3d_Len2dSqPntAndSegLine_l(f32 px, f32 pz, f32 x0, f32 z0, f32 x1, f32 z1, u32 ox, u32 oz, u32 od) {
    return gabi::call<bool>(0x020109FC, px, pz, x0, z0, x1, z1, ox, oz, od);
}
static inline f32 cM3d_Len2dSq_l(f32 x0, f32 z0, f32 x1, f32 z1) { return gabi::call<f32>(0x020109E8, x0, z0, x1, z1); }
static inline void cM2d_CrossCirLin_l(u32 cir, f32 x, f32 z, f32 dx, f32 dz, u32 ox, u32 oz) {
    gabi::call(0x02010284, cir, x, z, dx, dz, ox, oz);
}

static inline u32 A(const void* p) { return gabi::ea(p); }
static inline u32 L32(u32 a) { return gabi::load<u32>(a); }
static inline u32 LU16(u32 a) { return gabi::load<u16>(a); }
static inline f32 LF(u32 a) { return gabi::load<f32>(a); }
static inline void assert_fail(u32 file, s32 line, u32 msg) { JUT_ASSERT_fail(STR(file), line, STR(msg)); }

/* virtual calls through the dBgW vtable (+4) */
static inline bool vChkPolyThrough(dBgW_l* w, u32 poly, u32 chk) {
    return gabi::call_ptr<bool>(gabi::load<u32>(w->__vtbl + 0x2C), w, poly, chk);
}
static inline bool vChkGrpThrough(dBgW_l* w, u32 grp, u32 chk, s32 depth) {
    return gabi::call_ptr<bool>(gabi::load<u32>(w->__vtbl + 0x3C), w, grp, chk, depth);
}

static constexpr f32 G_CM3D_F_ABS_MIN = 3.814697265625e-06f;
static inline bool cM3d_IsZero(f32 f) { return std::fabs(f) < G_CM3D_F_ABS_MIN; }

/* 024F23F4 */
static dBgW_l* dBgW_ct(dBgW_l* w) {
    WWHD_FUNC(0x024F23F4, dBgW_l*, w);
    if (w == nullptr) {
        w = (dBgW_l*)operator_new(0xBC);
        if (w == nullptr) return w;
    }
    cBgW_ct_l(w);
    w->mpPushPullCb = 0;
    w->__vtbl = 0x10044018;
    w->mOldRotY = 0;
    w->mGrpRoomInf = 0xFF;
    w->mRotYDelta = 0;
    w->mFlag = 0;
    w->mRoomId = 0xFFFF;
    w->mpRideCb = 0;
    w->m_crr_func = 0;
    return w;
}
VERIFY(0x024F23F4, dBgW_ct);

/* 024F2478 */
static dBgW_l* dBgW_NewSet(u32 bgd, u32 flag, u32 mtx) {
    WWHD_FUNC(0x024F2478, dBgW_l*, bgd, flag, mtx);
    dBgW_l* rt = dBgW_ct(nullptr);
    if (rt == nullptr) return nullptr;
    if (cBgW_Set_l(rt, bgd, flag, mtx)) return nullptr;
    return rt;
}
VERIFY(0x024F2478, dBgW_NewSet);

/* 024F2514 */
static void dBgW_positionWallCorrect(dBgW_l* w, dBgS_Acch* acch, f32 dist, u32 plane, u32 pupper_pos, f32 speed) {
    WWHD_FUNC(0x024F2514, void, w, acch, dist, plane, pupper_pos, speed);
    acch->m_flags |= 0x10; /* SetWallHit */
    f32 move = speed * dist;
    f32 x = gabi::fmadds(move, LF(plane + 0), LF(pupper_pos + 0));
    gabi::store<f32>(pupper_pos + 0, x);
    f32 z = gabi::fmadds(move, LF(plane + 8), LF(pupper_pos + 8));
    gabi::store<f32>(pupper_pos + 8, z);
    if (std::isnan(x)) {
        assert_fail(0x10043D44, 0xD0, 0x10043D50);
        z = LF(pupper_pos + 8);
    }
    if (std::isnan(z)) assert_fail(0x10043D44, 0xD1, 0x10043D9C);
}
VERIFY(0x024F2514, dBgW_positionWallCorrect);

/* RwgWallCorrect, after a correction: dBgS_Acch::CalcMovePosWork, SetWallCirHit, SetWallPolyIndex and
 * SetWallAngleY (inline, with the asserts of d_bg_s_acch.h and c_bg_s_poly_info.h) */
static inline void wall_hit(dBgS_Acch* acch, s32 cir_index, u32 off, u32 poly, u32 tri) {
    dBgS_Acch_SetWallCir_l(acch);
    cM3dGLin_SetStartEnd_l(A(acch) + 0x40, L32(A(acch) + 0x30), L32(A(acch) + 0x2C));
    dBgS_Acch_CalcWallBmdCyl_l(acch);
    { u32 c = L32(A(acch) + 0x88) + off; gabi::store<u32>(c + 0x10, L32(c + 0x10) | 2); }
    if (!(cir_index <= acch->m_tbl_size)) assert_fail(0x10043E40, 0x274, 0x10043E2C);
    u32 c1 = L32(A(acch) + 0x88) + off;
    s32 bg = acch->m_bg_index;
    u32 f78 = acch->field_0x78;
    u32 ap = acch->m_ap_id;
    if (bg < 0) assert_fail(0x10043E60, 0x59, 0x10043E50);
    gabi::store<u32>(c1 + 8, ap);
    gabi::store<u16>(c1 + 2, (u16)bg);
    gabi::store<u32>(c1 + 4, f78);
    u32 c2 = L32(A(acch) + 0x88) + off;
    if ((s32)poly < 0) assert_fail(0x10043E60, 0x7B, 0x10043DF4);
    gabi::store<u16>(c2 + 0, (u16)poly);
    s16 ang = cM_atan2s(gabi::load<f32>(tri + 0), gabi::load<f32>(tri + 8));
    gabi::store<s16>(L32(A(acch) + 0x88) + off + 0x3C, ang);
}

/* 024F25D4 */
static bool dBgW_RwgWallCorrect(dBgW_l* w, dBgS_Acch* pwi, u32 i_poly_idx) {
    WWHD_FUNC(0x024F25D4, bool, w, pwi, i_poly_idx);
    bool correct = false;
    gabi::Local<cXyz> sp50;
    gabi::Local<be<f32>> spC8, spCC, spD0, spF0, spF4;
    while (true) {
        u32 rwg_elm = w->pm_rwg + i_poly_idx * 8;
        if (!vChkPolyThrough(w, i_poly_idx, gabi::load<u32>(A(pwi) + 0))) {
            u32 tri = w->pm_tri + i_poly_idx * 0x18;
            f32 nz0 = gabi::load<f32>(tri + 8);
            f32 nx0 = gabi::load<f32>(tri + 0);
            f32 sp68 = std_sqrtf(gabi::fmadds(nx0, nx0, nz0 * nz0));
            if (!cM3d_IsZero(sp68)) {
                s32 cir_index = 0;
                f32 sp6C = 1.0f / sp68;
                u32 tri_data = L32(w->pm_bgd + 0xC) + i_poly_idx * 0xA;
                u32 off = 0;
                for (; cir_index < pwi->m_tbl_size; cir_index++, off += 0x40) {
                    u32 cir = L32(A(pwi) + 0x88) + off;
                    f32 sp78 = gabi::fmuls_ppc(sp6C, LF(cir + 0x34));
                    sp50->y = 0.0f;
                    sp50->x = gabi::fmuls_ppc(gabi::load<f32>(tri + 0), sp78);
                    sp50->z = gabi::fmuls_ppc(gabi::load<f32>(tri + 8), sp78);

                    f32 sp7C;
                    if (!(L32(cir + 0x10) & 4)) {
                        u32 speed = L32(A(pwi) + 0x34);
                        u32 pos = L32(A(pwi) + 0x2C);
                        f32 wall_h = LF(cir + 0x30);
                        f32 pos_y = LF(pos + 4);
                        f32 speed_y = speed != 0 ? LF(speed + 4) : 0.0f;
                        f32 base = gabi::fadds_ppc(pos_y, wall_h);
                        f32 add = dBgS_Acch_GetWallAddY_l(pwi, A(sp50.get()), cir_index);
                        sp7C = gabi::fsubs_ppc(gabi::fadds_ppc(base, add), speed_y);
                    } else {
                        sp7C = LF(cir + 0x38);
                    }

                    u32 vtx = w->pm_vtx_tbl;
                    u32 v0 = vtx + gabi::load<u16>(tri_data + 0) * 0xC;
                    f32 s0 = gabi::load<f32>(v0 + 4) - sp7C;
                    u32 v1 = vtx + gabi::load<u16>(tri_data + 2) * 0xC;
                    f32 s1 = gabi::load<f32>(v1 + 4) - sp7C;
                    u32 v2 = vtx + gabi::load<u16>(tri_data + 4) * 0xC;
                    f32 s2 = gabi::load<f32>(v2 + 4) - sp7C;

                    if ((s0 > 0.0f && s1 > 0.0f && s2 > 0.0f) || (s0 < 0.0f && s1 < 0.0f && s2 < 0.0f)) continue;

                    s32 zeros = 0;
                    if (cM3d_IsZero(s0)) zeros++;
                    if (cM3d_IsZero(s1)) zeros++;
                    if (cM3d_IsZero(s2)) zeros++;
                    if (zeros == 1) continue;

                    /* the branches test the negated comparisons: a NaN counts as "<= 0" / ">= 0" here */
                    s32 sp80;
                    f32 a, b, sp90, sp94;
                    if ((s0 > 0.0f && !(s1 > 0.0f) && !(s2 > 0.0f)) || (s0 < 0.0f && !(s1 < 0.0f) && !(s2 < 0.0f))) {
                        sp80 = 0; a = s1; b = s2; sp90 = s0 - s1; sp94 = s0 - s2;
                    } else if ((s1 > 0.0f && !(s0 > 0.0f) && !(s2 > 0.0f)) || (s1 < 0.0f && !(s0 < 0.0f) && !(s2 < 0.0f))) {
                        sp80 = 1; a = s0; b = s2; sp90 = s1 - s0; sp94 = s1 - s2;
                    } else {
                        sp80 = 2; a = s0; b = s1; sp90 = s2 - s0; sp94 = s2 - s1;
                    }
                    if (cM3d_IsZero(sp90) || cM3d_IsZero(sp94)) continue;
                    f32 sp9C = -(b / sp94);
                    f32 sp98 = -(a / sp90);

                    f32 vtx0_x = gabi::load<f32>(v0 + 0), vtx0_z = gabi::load<f32>(v0 + 8);
                    f32 vtx1_x = gabi::load<f32>(v1 + 0), vtx1_z = gabi::load<f32>(v1 + 8);
                    f32 vtx2_x = gabi::load<f32>(v2 + 0), vtx2_z = gabi::load<f32>(v2 + 8);
                    f32 cx0, cy0, cx1, cy1;
                    if (sp80 == 0) {
                        cx0 = gabi::fmadds(vtx0_x - vtx1_x, sp98, vtx1_x);
                        cy0 = gabi::fmadds(vtx0_z - vtx1_z, sp98, vtx1_z);
                        cx1 = gabi::fmadds(vtx0_x - vtx2_x, sp9C, vtx2_x);
                        cy1 = gabi::fmadds(vtx0_z - vtx2_z, sp9C, vtx2_z);
                    } else if (sp80 == 1) {
                        cx0 = gabi::fmadds(vtx1_x - vtx0_x, sp98, vtx0_x);
                        cy0 = gabi::fmadds(vtx1_z - vtx0_z, sp98, vtx0_z);
                        cx1 = gabi::fmadds(vtx1_x - vtx2_x, sp9C, vtx2_x);
                        cy1 = gabi::fmadds(vtx1_z - vtx2_z, sp9C, vtx2_z);
                    } else {
                        cx0 = gabi::fmadds(vtx2_x - vtx0_x, sp98, vtx0_x);
                        cy0 = gabi::fmadds(vtx2_z - vtx0_z, sp98, vtx0_z);
                        cx1 = gabi::fmadds(vtx2_x - vtx1_x, sp9C, vtx1_x);
                        cy1 = gabi::fmadds(vtx2_z - vtx1_z, sp9C, vtx1_z);
                    }
                    f32 sp50x = sp50->x, sp50z = sp50->z;
                    cx0 = cx0 + sp50x;
                    cy0 = cy0 + sp50z;
                    cx1 = cx1 + sp50x;
                    cy1 = cy1 + sp50z;

                    u32 pos = L32(A(pwi) + 0x2C);
                    bool sp107 = cM3d_Len2dSqPntAndSegLine_l(LF(pos + 0), LF(pos + 8), cx0, cy0, cx1, cy1, A(spCC.get()),
                                                             A(spD0.get()), A(spC8.get()));
                    pos = L32(A(pwi) + 0x2C);
                    f32 pos_x = LF(pos + 0), pos_z = LF(pos + 8);
                    f32 spDC = LF(L32(A(pwi) + 0x88) + off + 0x28);
                    f32 c8 = *spC8;
                    f32 spD4 = *spCC - pos_x;
                    f32 spD8 = *spD0 - pos_z;
                    if (c8 > spDC) continue;
                    sp50x = sp50->x;
                    sp50z = sp50->z;
                    if (gabi::fmadds(spD4, sp50x, spD8 * sp50z) < 0.0f) continue;

                    if (sp107) {
                        f32 len = std_sqrtf(c8);
                        dBgW_positionWallCorrect(w, pwi, sp6C, tri, pos, len);
                        wall_hit(pwi, cir_index, off, i_poly_idx, tri);
                        correct = true;
                    } else {
                        cx0 = cx0 - sp50x;
                        cy0 = cy0 - sp50z;
                        cx1 = cx1 - sp50x;
                        cy1 = cy1 - sp50z;
                        f32 spE0 = cM3d_Len2dSq_l(cx0, cy0, pos_x, pos_z);
                        pos = L32(A(pwi) + 0x2C);
                        f32 spE4 = cM3d_Len2dSq_l(cx1, cy1, LF(pos + 0), LF(pos + 8));
                        f32 onx = -(f32)gabi::load<f32>(tri + 0);
                        f32 ony = -(f32)gabi::load<f32>(tri + 8);
                        if (cM3d_IsZero(onx) && cM3d_IsZero(ony)) assert_fail(0x10043E74, 0x1CE, 0x10043E04);

                        f32 ex, ez;
                        s32 l0, l1;
                        if (spE0 < spE4) {
                            if (spE0 > spDC || std::fabs(spE0 - spDC) < 0.008f) continue;
                            ex = cx0; ez = cy0; l0 = 0x1E4; l1 = 0x1E5;
                        } else {
                            if (spE4 > spDC || std::fabs(spE4 - spDC) < 0.008f) continue;
                            ex = cx1; ez = cy1; l0 = 0x20C; l1 = 0x20D;
                        }
                        if (!(cir_index <= pwi->m_tbl_size)) assert_fail(0x10043E40, 0x2F9, 0x10043E2C);
                        cM2d_CrossCirLin_l(L32(A(pwi) + 0x88) + off + 0x14, ex, ez, onx, ony, A(spF0.get()),
                                           A(spF4.get()));
                        pos = L32(A(pwi) + 0x2C);
                        gabi::store<f32>(pos + 0, LF(pos + 0) + (ex - *spF0));
                        pos = L32(A(pwi) + 0x2C);
                        gabi::store<f32>(pos + 8, LF(pos + 8) + (ez - *spF4));
                        pos = L32(A(pwi) + 0x2C);
                        if (std::isnan(LF(pos + 0))) {
                            assert_fail(0x10043E74, l0, 0x10043E80);
                            pos = L32(A(pwi) + 0x2C);
                        }
                        if (std::isnan(LF(pos + 8))) assert_fail(0x10043E74, l1, 0x10043ED4);
                        wall_hit(pwi, cir_index, off, i_poly_idx, tri);
                        pwi->m_flags |= 0x10; /* SetWallHit */
                        correct = true;
                    }
                }
            }
        }
        u16 next = gabi::load<u16>(rwg_elm);
        if (next == 0xFFFF) break;
        i_poly_idx = next;
    }
    return correct;
}
VERIFY(0x024F25D4, dBgW_RwgWallCorrect);

/* 024F326C */
static bool dBgW_WallCorrectRp(dBgW_l* w, dBgS_Acch* acch, s32 i) {
    WWHD_FUNC(0x024F326C, bool, w, acch, i);
    if (!cM3d_Cross_AabCyl_l(w->m_nt_tbl + i * 0x1C, A(acch) + 0x5C)) return false;
    u32 tree = L32(w->pm_bgd + 0x1C) + i * 0x14;
    bool ret = false;
    if (gabi::load<u16>(tree) & 1) {
        u32 wall = gabi::load<u16>(w->pm_blk + gabi::load<u16>(tree + 4) * 6 + 2);
        if (wall != 0xFFFF && dBgW_RwgWallCorrect(w, acch, wall)) ret = true;
        u32 roof = gabi::load<u16>(w->pm_blk + gabi::load<u16>(tree + 4) * 6 + 0);
        if (roof != 0xFFFF && dBgW_RwgWallCorrect(w, acch, roof)) ret = true;
        return ret;
    }
    for (int k = 0; k < 8; k++) {
        u32 child = gabi::load<u16>(tree + 4 + 2 * k);
        if (child != 0xFFFF && dBgW_WallCorrectRp(w, acch, child)) ret = true;
    }
    return ret;
}
VERIFY(0x024F326C, dBgW_WallCorrectRp);

/* 024F3498 */
static bool dBgW_WallCorrectGrpRp(dBgW_l* w, dBgS_Acch* acch, s32 grp_id, s32 depth) {
    WWHD_FUNC(0x024F3498, bool, w, acch, grp_id, depth);
    if (vChkGrpThrough(w, grp_id, gabi::load<u32>(A(acch) + 4), depth)) return false;
    if (!cM3d_Cross_AabCyl_l(w->pm_grp + grp_id * 0x20, A(acch) + 0x5C)) return false;
    bool ret = false;
    u32 tree_idx = gabi::load<u16>(L32(w->pm_bgd + 0x24) + grp_id * 0x34 + 0x2E);
    if (tree_idx != 0xFFFF && dBgW_WallCorrectRp(w, acch, tree_idx)) ret = true;
    depth++;
    u32 child_idx = gabi::load<u16>(L32(w->pm_bgd + 0x24) + grp_id * 0x34 + 0x28);
    while (child_idx != 0xFFFF) {
        if (dBgW_WallCorrectGrpRp(w, acch, child_idx, depth)) ret = true;
        child_idx = gabi::load<u16>(L32(w->pm_bgd + 0x24) + child_idx * 0x34 + 0x26);
    }
    return ret;
}
VERIFY(0x024F3498, dBgW_WallCorrectGrpRp);

/* RwgRoofChk / RwgSplGrpChk: the chain of polygons from poly_index; `roof` picks the comparison */
/* 024F35B4 */
static bool dBgW_RwgRoofChk(dBgW_l* w, u32 poly_index, u32 chk) {
    WWHD_FUNC(0x024F35B4, bool, w, poly_index, chk);
    bool ret = false;
    gabi::Local<be<f32>> y;
    while (true) {
        u32 off = poly_index * 0x18;
        if (cM3dGPla_getCrossY_l(w->pm_tri + off, chk + 0x38, A(y.get())) && *y > gabi::load<f32>(chk + 0x3C) &&
            *y < gabi::load<f32>(chk + 0x48)) {
            u32 tri = L32(w->pm_bgd + 0xC) + poly_index * 0xA;
            u32 vtx = w->pm_vtx_tbl;
            if (cM3d_CrossY_Tri_l(vtx + gabi::load<u16>(tri + 0) * 0xC, vtx + gabi::load<u16>(tri + 2) * 0xC,
                                  vtx + gabi::load<u16>(tri + 4) * 0xC, w->pm_tri + off, chk + 0x38) &&
                !vChkPolyThrough(w, poly_index, gabi::load<u32>(chk + 0x10))) {
                gabi::store<f32>(chk + 0x48, *y); /* SetNowY */
                if ((s32)poly_index < 0) assert_fail(0x10043F38, 0x7B, 0x10043F28);
                gabi::store<u16>(chk + 0, (u16)poly_index);
                ret = true;
            }
        }
        u32 next = gabi::load<u16>(w->pm_rwg + poly_index * 8);
        if (next == 0xFFFF) break;
        poly_index = next;
    }
    return ret;
}
VERIFY(0x024F35B4, dBgW_RwgRoofChk);

/* 024F36E0 */
static bool dBgW_RoofChkRp(dBgW_l* w, u32 chk, s32 i) {
    WWHD_FUNC(0x024F36E0, bool, w, chk, i);
    u32 node = w->m_nt_tbl + i * 0x1C;
    if (!cM3dGAab_CrossY_l(node, chk + 0x38) || !cM3dGAab_UnderPlaneYUnder_l(node, gabi::load<f32>(chk + 0x48)) ||
        cM3dGAab_TopPlaneYUnder_l(node, gabi::load<f32>(chk + 0x3C)))
        return false;
    u32 tree = L32(w->pm_bgd + 0x1C) + i * 0x14;
    if (gabi::load<u16>(tree) & 1) {
        u32 roof = gabi::load<u16>(w->pm_blk + gabi::load<u16>(tree + 4) * 6 + 0);
        if (roof != 0xFFFF && dBgW_RwgRoofChk(w, roof, chk)) return true;
        return false;
    }
    bool ret = false;
    for (int k = 0; k < 8; k++) {
        u32 child = gabi::load<u16>(tree + 4 + 2 * k);
        if (child != 0xFFFF && dBgW_RoofChkRp(w, chk, child)) ret = true;
    }
    return ret;
}
VERIFY(0x024F36E0, dBgW_RoofChkRp);

/* 024F3924 */
static bool dBgW_RoofChkGrpRp(dBgW_l* w, u32 chk, s32 grp_id, s32 depth) {
    WWHD_FUNC(0x024F3924, bool, w, chk, grp_id, depth);
    if (vChkGrpThrough(w, grp_id, gabi::load<u32>(chk + 0x14), depth)) return false;
    u32 grp = w->pm_grp + grp_id * 0x20;
    if (!cM3dGAab_CrossY_l(grp, chk + 0x38) || !cM3dGAab_UnderPlaneYUnder_l(grp, gabi::load<f32>(chk + 0x48)) ||
        cM3dGAab_TopPlaneYUnder_l(grp, gabi::load<f32>(chk + 0x3C)))
        return false;
    bool ret = false;
    u32 grpd = L32(w->pm_bgd + 0x24) + grp_id * 0x34;
    u32 tree_idx = gabi::load<u16>(grpd + 0x2E);
    if (tree_idx != 0xFFFF && dBgW_RoofChkRp(w, chk, tree_idx)) ret = true;
    depth++;
    u32 child_idx = gabi::load<u16>(grpd + 0x28);
    while (child_idx != 0xFFFF) {
        if (dBgW_RoofChkGrpRp(w, chk, child_idx, depth)) ret = true;
        child_idx = gabi::load<u16>(L32(w->pm_bgd + 0x24) + child_idx * 0x34 + 0x26);
    }
    return ret;
}
VERIFY(0x024F3924, dBgW_RoofChkGrpRp);

/* 024F3A60 */
static bool dBgW_RwgSplGrpChk(dBgW_l* w, u32 poly_index, u32 chk) {
    WWHD_FUNC(0x024F3A60, bool, w, poly_index, chk);
    bool ret = false;
    gabi::Local<be<f32>> y;
    while (true) {
        u32 off = poly_index * 0x18;
        if (cM3dGPla_getCrossY_l(w->pm_tri + off, chk + 0x38, A(y.get())) && *y < gabi::load<f32>(chk + 0x44) &&
            *y > gabi::load<f32>(chk + 0x48)) {
            u32 tri = L32(w->pm_bgd + 0xC) + poly_index * 0xA;
            u32 vtx = w->pm_vtx_tbl;
            if (cM3d_CrossY_Tri_l(vtx + gabi::load<u16>(tri + 0) * 0xC, vtx + gabi::load<u16>(tri + 2) * 0xC,
                                  vtx + gabi::load<u16>(tri + 4) * 0xC, w->pm_tri + off, chk + 0x38) &&
                !vChkPolyThrough(w, poly_index, gabi::load<u32>(chk + 0x10))) {
                gabi::store<f32>(chk + 0x48, *y); /* SetHeight */
                if ((s32)poly_index < 0) assert_fail(0x10043F5C, 0x7B, 0x10043F4C);
                gabi::store<u16>(chk + 0, (u16)poly_index);
                ret = true;
            }
        }
        u32 next = gabi::load<u16>(w->pm_rwg + poly_index * 8);
        if (next == 0xFFFF) break;
        poly_index = next;
    }
    return ret;
}
VERIFY(0x024F3A60, dBgW_RwgSplGrpChk);

/* 024F3B8C */
static bool dBgW_SplGrpChkRp(dBgW_l* w, u32 chk, s32 i) {
    WWHD_FUNC(0x024F3B8C, bool, w, chk, i);
    u32 node = w->m_nt_tbl + i * 0x1C;
    if (!cM3dGAab_CrossY_l(node, chk + 0x38) || !cM3dGAab_UnderPlaneYUnder_l(node, gabi::load<f32>(chk + 0x44)) ||
        cM3dGAab_TopPlaneYUnder_l(node, gabi::load<f32>(chk + 0x48)))
        return false;
    u32 tree = L32(w->pm_bgd + 0x1C) + i * 0x14;
    if (gabi::load<u16>(tree) & 1) {
        u32 ground = gabi::load<u16>(w->pm_blk + gabi::load<u16>(tree + 4) * 6 + 4);
        if (ground != 0xFFFF && dBgW_RwgSplGrpChk(w, ground, chk)) return true;
        return false;
    }
    bool ret = false;
    for (int k = 0; k < 8; k++) {
        u32 child = gabi::load<u16>(tree + 4 + 2 * k);
        if (child != 0xFFFF && dBgW_SplGrpChkRp(w, chk, child)) ret = true;
    }
    return ret;
}
VERIFY(0x024F3B8C, dBgW_SplGrpChkRp);

/* 024F3DD4 */
static bool dBgW_SplGrpChkGrpRp(dBgW_l* w, u32 chk, s32 grp_id, s32 depth) {
    WWHD_FUNC(0x024F3DD4, bool, w, chk, grp_id, depth);
    if (vChkGrpThrough(w, grp_id, gabi::load<u32>(chk + 0x14), depth)) return false;
    u32 grp = w->pm_grp + grp_id * 0x20;
    if (!cM3dGAab_CrossY_l(grp, chk + 0x38) || !cM3dGAab_UnderPlaneYUnder_l(grp, gabi::load<f32>(chk + 0x44)) ||
        cM3dGAab_TopPlaneYUnder_l(grp, gabi::load<f32>(chk + 0x48)))
        return false;
    bool ret = false;
    u32 grpd = L32(w->pm_bgd + 0x24) + grp_id * 0x34;
    u32 tree_idx = gabi::load<u16>(grpd + 0x2E);
    if (tree_idx != 0xFFFF && dBgW_SplGrpChkRp(w, chk, tree_idx)) ret = true;
    depth++;
    u32 child_idx = gabi::load<u16>(grpd + 0x28);
    while (child_idx != 0xFFFF) {
        if (dBgW_SplGrpChkGrpRp(w, chk, child_idx, depth)) ret = true;
        child_idx = gabi::load<u16>(L32(w->pm_bgd + 0x24) + child_idx * 0x34 + 0x26);
    }
    return ret;
}
VERIFY(0x024F3DD4, dBgW_SplGrpChkGrpRp);

/* 024F3F10 */
static bool dBgW_RwgSphChk(dBgW_l* w, u32 i_poly_idx, u32 chk, u32 data) {
    WWHD_FUNC(0x024F3F10, bool, w, i_poly_idx, chk, data);
    gabi::Local<u8[0x38]> tri; /* cM3dGTri */
    cM3dGTri_ct_l(A(tri.get()));
    bool ret = false;
    while (true) {
        u32 rwg = w->pm_rwg + i_poly_idx * 8;
        if (!vChkPolyThrough(w, i_poly_idx, gabi::load<u32>(chk + 0x24))) {
            u32 tri_t = L32(w->pm_bgd + 0xC) + i_poly_idx * 0xA;
            u32 vtx = w->pm_vtx_tbl;
            cM3dGTri_setBg_l(A(tri.get()), vtx + gabi::load<u16>(tri_t + 0) * 0xC, vtx + gabi::load<u16>(tri_t + 2) * 0xC,
                             vtx + gabi::load<u16>(tri_t + 4) * 0xC, w->pm_tri + i_poly_idx * 0x18);
            if (cM3d_Cross_SphTri_l(chk, A(tri.get()), 0)) {
                u32 cb = gabi::load<u32>(chk + 0x4C);
                if (cb != 0) {
                    u32 pla = w->pm_tri + i_poly_idx * 0x18;
                    gabi::call_ptr(cb, chk, (u32)w->pm_vtx_tbl, (u32)gabi::load<u16>(tri_t + 0),
                                   (u32)gabi::load<u16>(tri_t + 2), (u32)gabi::load<u16>(tri_t + 4), pla, data);
                }
                if ((s32)i_poly_idx < 0) assert_fail(0x10043F80, 0x7B, 0x10043F70);
                ret = true;
                gabi::store<u16>(chk + 0x14, (u16)i_poly_idx);
            }
        }
        u32 next = gabi::load<u16>(rwg);
        if (next == 0xFFFF) break;
        i_poly_idx = next;
    }
    return ret;
}
VERIFY(0x024F3F10, dBgW_RwgSphChk);

/* 024F4050 */
static bool dBgW_SphChkRp(dBgW_l* w, u32 chk, u32 user, s32 i) {
    WWHD_FUNC(0x024F4050, bool, w, chk, user, i);
    if (!cM3d_Cross_AabSph_l(w->m_nt_tbl + i * 0x1C, chk)) return false;
    u32 tree = L32(w->pm_bgd + 0x1C) + i * 0x14;
    bool ret = false;
    if (gabi::load<u16>(tree) & 1) {
        static const u32 kOrder[3] = {4, 0, 2}; /* ground, roof, wall */
        for (int k = 0; k < 3; k++) {
            u32 p = gabi::load<u16>(w->pm_blk + gabi::load<u16>(tree + 4) * 6 + kOrder[k]);
            if (p != 0xFFFF && dBgW_RwgSphChk(w, p, chk, user)) ret = true;
        }
        return ret;
    }
    for (int k = 0; k < 8; k++) {
        u32 child = gabi::load<u16>(tree + 4 + 2 * k);
        if (child != 0xFFFF && dBgW_SphChkRp(w, chk, user, child)) ret = true;
    }
    return ret;
}
VERIFY(0x024F4050, dBgW_SphChkRp);

/* 024F42BC */
static bool dBgW_SphChkGrpRp(dBgW_l* w, u32 chk, u32 user, s32 grp_id, s32 depth) {
    WWHD_FUNC(0x024F42BC, bool, w, chk, user, grp_id, depth);
    if (vChkGrpThrough(w, grp_id, gabi::load<u32>(chk + 0x28), depth)) return false;
    if (!cM3d_Cross_AabSph_l(w->pm_grp + grp_id * 0x20, chk)) return false;
    bool ret = false;
    u32 grpd = L32(w->pm_bgd + 0x24) + grp_id * 0x34;
    u32 tree_idx = gabi::load<u16>(grpd + 0x2E);
    if (tree_idx != 0xFFFF && dBgW_SphChkRp(w, chk, user, tree_idx)) ret = true;
    depth++;
    u32 child_idx = gabi::load<u16>(grpd + 0x28);
    while (child_idx != 0xFFFF) {
        if (dBgW_SphChkGrpRp(w, chk, user, child_idx, depth)) ret = true;
        child_idx = gabi::load<u16>(L32(w->pm_bgd + 0x24) + child_idx * 0x34 + 0x26);
    }
    return ret;
}
VERIFY(0x024F42BC, dBgW_SphChkGrpRp);

/* 024F43DC */
static void dBgW_Move(dBgW_l* w) {
    WWHD_FUNC(0x024F43DC, void, w);
    w->mFlag |= 1; /* OnMoveFlag */
    cBgW_Move_l(w);
}
VERIFY(0x024F43DC, dBgW_Move);

/* 024F43EC */
static void dBgW_ChangeAttributeCodeByPathPntNo(dBgW_l* w, u32 pnt_no, u32 attr) {
    WWHD_FUNC(0x024F43EC, void, w, pnt_no, attr);
    if (w->pm_bgd == 0) return;
    for (s32 i = 0; i < (s32)L32(w->pm_bgd + 0x28); i++) {
        u32 ti_pnt_no = dBgS_GetRoomPathPntNo_l(gabi::load<u32>(L32(w->pm_bgd + 0x2C) + i * 0x10 + 8));
        if (ti_pnt_no == pnt_no) dBgS_ChangeAttributeCode_l(attr, L32(w->pm_bgd + 0x2C) + i * 0x10 + 4);
    }
}
VERIFY(0x024F43EC, dBgW_ChangeAttributeCodeByPathPntNo);

/* 024F4484 */
static void dBgW_CrrPos(dBgW_l* w, u32 poly, u32 user, u32 accept, u32 pos, u32 angle, u32 shape_angle) {
    WWHD_FUNC(0x024F4484, void, w, poly, user, accept, pos, angle, shape_angle);
    if (w->m_crr_func != 0) gabi::call_ptr((u32)w->m_crr_func, w, user, poly, accept, pos, angle, shape_angle);
}
VERIFY(0x024F4484, dBgW_CrrPos);

/* 024F44A4 */
static void dBgW_TransPos(dBgW_l* w, u32 poly, u32 user, u32 accept, u32 pos, u32 angle, u32 shape_angle) {
    WWHD_FUNC(0x024F44A4, void, w, poly, user, accept, pos, angle, shape_angle);
    if (w->m_crr_func != 0) gabi::call_ptr((u32)w->m_crr_func, w, user, poly, accept, pos, angle, shape_angle);
}
VERIFY(0x024F44A4, dBgW_TransPos);

/* cBgW::GetPolyInfId + GetPolyInf3 (inline, with the c_bg_w.h asserts) */
static inline u32 GetPolyInf3(dBgW_l* w, s32 poly_index) {
    if (!(0 <= poly_index && poly_index < (s32)L32(w->pm_bgd + 0x8))) assert_fail(0x10043FDC, 0x2F1, 0x10043FAC);
    s32 id = gabi::load<u16>(L32(w->pm_bgd + 0xC) + poly_index * 0xA + 6);
    if (!(id < (s32)L32(w->pm_bgd + 0x28))) assert_fail(0x10043FDC, 0x307, 0x10043FE8);
    return gabi::load<u32>(L32(w->pm_bgd + 0x2C) + id * 0x10 + 0xC);
}

/* 024F44C4 */
static bool dBgW_ChkPolyThrough(dBgW_l* w, s32 poly_index, u32 chk) {
    WWHD_FUNC(0x024F44C4, bool, w, poly_index, chk);
    if (chk == 0) return false;
    /* dBgS_PolyPassChk flags +4..+0xA: obj, cam, link, arrow, bomb, boomerang, rope */
    static const u32 kMask[7] = {0x02, 0x01, 0x04, 0x08, 0x20, 0x40, 0x80};
    for (int k = 0; k < 7; k++)
        if (gabi::load<u8>(chk + 4 + k) != 0 && (GetPolyInf3(w, poly_index) & kMask[k])) return true;
    return false;
}
VERIFY(0x024F44C4, dBgW_ChkPolyThrough);

/* 024F48A4 */
static bool dBgW_ChkGrpThrough(dBgW_l* w, s32 grp_id, u32 chk, s32 depth) {
    WWHD_FUNC(0x024F48A4, bool, w, grp_id, chk, depth);
    if (depth != 2 || chk == 0) return false;
    u32 info = gabi::load<u32>(L32(w->pm_bgd + 0x24) + grp_id * 0x34 + 0x30);
    if (!(info & 0x80700) && (gabi::load<u32>(chk + 4) & 1)) return false; /* MaskNormalGrp */
    if ((info & 0x00100) && (gabi::load<u32>(chk + 4) & 2)) return false;  /* MaskWaterGrp */
    if ((info & 0x00200) && (gabi::load<u32>(chk + 4) & 4)) return false;  /* MaskYoganGrp */
    if ((info & 0x00400) && (gabi::load<u32>(chk + 4) & 8)) return false;  /* MaskDokuGrp */
    if ((info & 0x80000) && (gabi::load<u32>(chk + 4) & 0x10)) return false; /* MaskLightGrp */
    return true;
}
VERIFY(0x024F48A4, dBgW_ChkGrpThrough);

/* 024F4944: static initialisers (header statics), after the unit's functions */
static void __sinit_d_bg_w_cpp() {
    WWHD_FUNC(0x024F4944, void, (u32)0);
    sinit_header_statics(0x1046EDD0, 0x101D5354);
}
VERIFY(0x024F4944, __sinit_d_bg_w_cpp);

/* 024F49D8: dBgW::MatrixCrrPos (inline virtual, out-of-line copy of this unit): CrrPos through the vtable */
static void dBgW_MatrixCrrPos(dBgW_l* w, u32 poly, u32 user, u32 accept, u32 pos, u32 angle, u32 shape_angle) {
    WWHD_FUNC(0x024F49D8, void, w, poly, user, accept, pos, angle, shape_angle);
    gabi::call_ptr(gabi::load<u32>(w->__vtbl + 0x44), w, poly, user, accept, pos, angle, shape_angle);
}
VERIFY(0x024F49D8, dBgW_MatrixCrrPos);

}  // namespace d_bg_w_cpp

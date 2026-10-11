/* Actor background collision: dBgS_Acch and dBgS_AcchCir.
 * GameCube control flow with HD plane-null checks and water handling; verified local source.
 */
#include "d/d_bg_s_acch.h"

namespace d_bg_s_acch_cpp {
static AcchRooms l_roomBg;
static u8 l_linVtable;
static u8 l_linPolyInfoVtable;
static u8 l_baseGroupVtable;
static u8 l_groundCheckVtable;
static u8 l_groundPolyInfoVtable;
static u8 l_groundGroupVtable;
static u8 l_groundPassVtable;
static u8 l_roofCheckVtable;
static u8 l_roofPassVtable;
static u8 l_waterCheckVtable;
static u8 l_waterPassVtable;
static u8 l_circlePolyInfoVtable;
static u8 l_setAssertFile;
static u8 l_setPosAssert;
static u8 l_setOldPosAssert;
static u8 l_crrAssertFile;
static u8 l_crrPosAssert;
static u8 l_crrOldPosAssert;
static u8 l_waterUnderAssert;
static u8 l_waterTopAssert;
static u8 l_posXAssert;
static u8 l_posYAssert;
static u8 l_posZAssert;
static u8 l_posRangeAssert;
static u8 l_acchCheckVtable;
static u8 l_acchGroupVtable;
static u8 l_acchPassVtable;
static u8 l_headerRecords;
static u8 l_headerRecords2;
static u8 l_headerStatics;
static u8 l_headerStatics2;

static constexpr f32 G_CM3D_F_INF = 1000000000.0f;

/* dBgS_Chk::SetExtChk / dBgS_GndChk etc.: the pass-check pointers, actor id and flag byte */
static inline void SetExtChk_GndRoof(u32 dst, u32 src) {
    auto* dest = gabi::at<cBgS_Chk>(dst);
    auto* source = gabi::at<cBgS_Chk>(src);
    dest->mGrpPassChk = source->mGrpPassChk.get();
    dest->mPolyPassChk = source->mPolyPassChk.get();
    dest->mActorPid = source->mActorPid.get();
    dest->mSameActorChk = source->mSameActorChk.get();
}

/* 024EFE94 */

static dBgS_AcchCir* constructAcchCir(dBgS_AcchCir* c) {

    if (c == nullptr) {
        c = (dBgS_AcchCir*)operator_new(0x40);
        if (c == nullptr)
            return c;
    }
    /* cBgS_PolyInfo */
    static_cast<cBgS_PolyInfo*>(c)->mpBgW = 0;
    c->m_flags = 0;
    static_cast<cBgS_PolyInfo*>(c)->mPolyIndex = 0xFFFF;
    static_cast<cBgS_PolyInfo*>(c)->mBgIndex = 0x100;
    static_cast<cBgS_PolyInfo*>(c)->mActorId = 0xFFFFFFFF;
    static_cast<cBgS_PolyInfo*>(c)->__vtbl = gabi::ea(&l_circlePolyInfoVtable);
    acch_cM3dGCir_ct(gabi::ea(&c->m_cir)); /* cM3dGCir::cM3dGCir */
    c->m_wall_angle_y = 0;
    c->m_flags = 0;
    c->m_wall_rr = 0.0f;
    c->field_0x2c = 0.0f;
    c->m_wall_h = 0.0f;
    c->m_wall_r = 0.0f;
    c->m_wall_h_direct = 0.0f;
    return c;
}

/* 024EFF3C */

void AcchCirRead::SetWallR(f32 radius) {
    auto* c = this;

    c->m_wall_r = radius;
}

/* 024EFF44 */

void AcchCirRead::SetWall(f32 height, f32 radius) {
    auto* c = this;

    c->m_wall_h = height;
    static_cast<AcchCirRead*>(c)->SetWallR(radius);
}

/* 024EFD9C */
// HD: deleting destructor retains the generated subobject teardown and optional storage release.

static void destroyAcch(dBgS_Acch* a, s32 flags) {

    if (a == nullptr)
        return;
    u32 b = gabi::ea(a);
    static_cast<dBgS_GrpPassChk*>(a)->__vtbl = gabi::ea(&l_acchGroupVtable);
    static_cast<dBgS_PolyPassChk*>(a)->__vtbl = gabi::ea(&l_acchPassVtable);
    static_cast<cBgS_Chk*>(reinterpret_cast<AcchWaterRead*>(&a->m_wtr))->__vtbl =
        gabi::ea(&l_waterCheckVtable);
    static_cast<dBgS_PolyPassChk*>(reinterpret_cast<AcchWaterRead*>(&a->m_wtr))->__vtbl =
        gabi::ea(&l_waterPassVtable);
    static_cast<dBgS_GrpPassChk*>(reinterpret_cast<AcchWaterRead*>(&a->m_wtr))->__vtbl =
        gabi::ea(&l_baseGroupVtable);
    acch_cBgS_LinChk_dt(
        gabi::ea(static_cast<cBgS_Chk*>(reinterpret_cast<AcchWaterRead*>(&a->m_wtr))),
        0); /* m_wtr */
    static_cast<dBgS_GrpPassChk*>(&a->m_roof)->__vtbl = gabi::ea(&l_baseGroupVtable);
    static_cast<cBgS_Chk*>(&a->m_roof)->__vtbl = gabi::ea(&l_roofCheckVtable);
    static_cast<dBgS_PolyPassChk*>(&a->m_roof)->__vtbl = gabi::ea(&l_roofPassVtable);
    acch_cBgS_LinChk_dt(gabi::ea(static_cast<cBgS_Chk*>(&a->m_roof)), 0); /* m_roof */
    static_cast<dBgS_GrpPassChk*>(&a->m_gnd)->__vtbl = gabi::ea(&l_baseGroupVtable);
    static_cast<dBgS_PolyPassChk*>(&a->m_gnd)->__vtbl = gabi::ea(&l_groundPassVtable);
    static_cast<cBgS_PolyInfo*>(&a->m_gnd)->__vtbl = gabi::ea(&l_groundPolyInfoVtable);
    acch_cBgS_Chk_dt(gabi::ea(&a->m_gnd), 0); /* m_gnd: cBgS_Chk::~cBgS_Chk */
    static_cast<dBgS_GrpPassChk*>(a)->__vtbl = gabi::ea(&l_baseGroupVtable);
    acch_cBgS_LinChk_dt(b, 0);
    if (flags & DELETE_STORAGE)
        operator_delete(a);
}

/* 024F0474 */

static dBgS_Acch* constructAcch(dBgS_Acch* a) {

    if (a == nullptr) {
        a = (dBgS_Acch*)operator_new(0x1C4);
        if (a == nullptr)
            return a;
    }
    u32 b = gabi::ea(a);
    acch_cBgS_Chk_ct(b); /* cBgS_Chk::cBgS_Chk */
    static_cast<dBgS_PolyPassChk*>(a)->mbBombThrough = 0;
    static_cast<dBgS_PolyPassChk*>(a)->mbCamThrough = 0;
    static_cast<dBgS_GrpPassChk*>(a)->__vtbl = gabi::ea(&l_acchGroupVtable);
    static_cast<cBgS_Chk*>(a)->__vtbl = gabi::ea(&l_acchCheckVtable);
    a->pm_old_pos.v = 0;
    static_cast<dBgS_PolyPassChk*>(a)->mbArrowThrough = 0;
    a->m_lin.__vtbl = gabi::ea(&l_linVtable);
    a->pm_pos.v = 0;
    a->m_flags = 0;
    static_cast<dBgS_GrpPassChk*>(a)->mGrp = 1;
    static_cast<dBgS_PolyPassChk*>(a)->mbLinkThrough = 0;
    static_cast<dBgS_PolyPassChk*>(a)->mbBoomerangThrough = 0;
    a->pm_shape_angle = 0;
    static_cast<dBgS_PolyPassChk*>(a)->mbRopeThrough = 0;
    a->pm_speed.v = 0;
    a->pm_angle = 0;
    static_cast<dBgS_PolyPassChk*>(a)->__vtbl = gabi::ea(&l_acchPassVtable);
    static_cast<dBgS_PolyPassChk*>(a)->mbObjThrough = 0;
    acch_cM3dGCyl_ct(gabi::ea(a->m_wall_cyl)); /* cM3dGCyl::cM3dGCyl */
    a->m_my_ac.v = 0;
    a->field_0x78 = 0;
    a->m_ap_id = 0;
    a->m_ground_up_h = 0.0f;
    a->m_ground_check_offset = 0.0f;
    a->m_ground_h = 0.0f;
    a->pm_acch_cir.v = 0;
    a->m_bg_index = 0;
    a->m_tbl_size = 0;
    a->m_ground_up_h_diff = 0.0f;
    acch_cM3dGPla_ct(gabi::ea(a->m_pla)); /* cM3dGPla::cM3dGPla */
    a->field_0xC4 = 0.0f;
    a->pm_out_poly_info = 0;
    a->field_0xb8 = 0.0f;
    a->m_roof_crr_height = 0.0f;
    a->field_0xb0 = 0;
    a->m_sea_height = 0.0f;
    a->field_0xb4 = 0.0f;
    a->m_wtr_check_offset = 0.0f;
    a->m_roof_height = 0.0f;
    acch_cBgS_GndChk_ct(gabi::ea(&a->m_gnd)); /* cBgS_GndChk::cBgS_GndChk */
    static_cast<dBgS_PolyPassChk*>(&a->m_gnd)->mbBoomerangThrough = 0;
    static_cast<dBgS_PolyPassChk*>(&a->m_gnd)->mbRopeThrough = 0;
    static_cast<cBgS_Chk*>(&a->m_gnd)->mPolyPassChk =
        gabi::ea(static_cast<dBgS_PolyPassChk*>(&a->m_gnd));
    static_cast<dBgS_GrpPassChk*>(&a->m_gnd)->mGrp = 1;
    static_cast<dBgS_PolyPassChk*>(&a->m_gnd)->mbArrowThrough = 0;
    static_cast<dBgS_PolyPassChk*>(&a->m_gnd)->__vtbl = gabi::ea(&l_groundPassVtable);
    static_cast<dBgS_PolyPassChk*>(&a->m_gnd)->mbCamThrough = 0;
    static_cast<cBgS_Chk*>(&a->m_gnd)->__vtbl = gabi::ea(&l_groundCheckVtable);
    static_cast<dBgS_GrpPassChk*>(&a->m_gnd)->__vtbl = gabi::ea(&l_groundGroupVtable);
    static_cast<dBgS_PolyPassChk*>(&a->m_gnd)->mbLinkThrough = 0;
    static_cast<cBgS_PolyInfo*>(&a->m_gnd)->__vtbl = gabi::ea(&l_groundPolyInfoVtable);
    static_cast<dBgS_PolyPassChk*>(&a->m_gnd)->mbObjThrough = 0;
    static_cast<dBgS_PolyPassChk*>(&a->m_gnd)->mbBombThrough = 0;
    static_cast<cBgS_Chk*>(&a->m_gnd)->mGrpPassChk =
        gabi::ea(static_cast<dBgS_GrpPassChk*>(&a->m_gnd));
    acch_dBgS_RoofChk_ct(gabi::ea(&a->m_roof)); /* dBgS_RoofChk::dBgS_RoofChk */
    acch_dBgS_WtrChk_ct(gabi::ea(&a->m_wtr));   /* dBgS_WtrChk::dBgS_WtrChk */
    /* body */
    a->pm_acch_cir = nullptr;
    a->field_0xC4 = 0.0f;
    static_cast<cBgS_Chk*>(a)->mPolyPassChk =
        gabi::ea(static_cast<dBgS_PolyPassChk*>(a)); /* SetPolyPassChk */
    a->pm_speed = nullptr;
    a->field_0xb8 = 0.0f;
    a->m_sea_height = -G_CM3D_F_INF;
    a->field_0xb4 = 0.0f;
    static_cast<cBgS_Chk*>(a)->mGrpPassChk =
        gabi::ea(static_cast<dBgS_GrpPassChk*>(a)); /* SetGrpPassChk */
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

/* 024F0824 */
// HD: ClrWallHit of a circle also clears its poly info

void AcchRead::Init() {
    auto* a = this;

    a->m_flags &= ~(u32)dBgS_Acch::WALL_HIT;
    for (s32 i = 0; i < a->m_tbl_size; i++) {
        dBgS_AcchCir& c = a->pm_acch_cir.get()[i];
        static_cast<cBgS_PolyInfo*>(&c)->mPolyIndex = 0xFFFF; /* ClearPi */
        static_cast<cBgS_PolyInfo*>(&c)->mBgIndex = 0x100;
        c.m_flags &= ~(u32)dBgS_AcchCir::WALL_HIT;
        static_cast<cBgS_PolyInfo*>(&c)->mpBgW = 0;
        static_cast<cBgS_PolyInfo*>(&c)->mActorId = 0xFFFFFFFF;
        dBgS_AcchCir& c2 = a->pm_acch_cir.get()[i];
        c2.m_flags &= ~(u32)dBgS_AcchCir::WALL_H_DIRECT;
    }
}

/* CHECK_PVEC3_RANGE / isnan asserts of CrrPos (HD keeps them; isnan tests the stored bits) */
static inline bool is_nan_bits(f32 v) {
    u32 bits;
    memcpy(&bits, &v, 4);
    return (u32)(bits << 1) > 0xFF000000u;
}
static inline void CheckPos(dBgS_Acch* a, u32 lx, u32 ly, u32 lz, u32 lr) {
    const u32 file = gabi::ea(&l_crrAssertFile);
    if (is_nan_bits(a->pm_pos->x))
        JUT_ASSERT_fail(STR(file), lx, STR(gabi::ea(&l_posXAssert)));
    if (is_nan_bits(a->pm_pos->y))
        JUT_ASSERT_fail(STR(file), ly, STR(gabi::ea(&l_posYAssert)));
    if (is_nan_bits(a->pm_pos->z))
        JUT_ASSERT_fail(STR(file), lz, STR(gabi::ea(&l_posZAssert)));
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
    if (bad)
        JUT_ASSERT_fail(STR(file), lr, STR(gabi::ea(&l_posRangeAssert)));
}

/* 024F06B4 */
// the 9th argument (shape angle) is on the stack

void AcchRead::Set(cXyz* pos, cXyz* old_pos, fopAc_ac_c* actor, s32 tbl_size, dBgS_AcchCir* cir,
                   cXyz* speed, csXyz* angle) {
    auto* a = this;

    u32 shape_angle = gabi::load<u32>(gabi::cpu->r[1] + 8);
    a->pm_pos = pos;
    a->pm_old_pos = old_pos;
    if (a->pm_pos.v == 0)
        JUT_ASSERT_fail(STR(gabi::ea(&l_setAssertFile)), 0x10B, STR(gabi::ea(&l_setPosAssert)));
    if (a->pm_old_pos.v == 0)
        JUT_ASSERT_fail(STR(gabi::ea(&l_setAssertFile)), 0x10C, STR(gabi::ea(&l_setOldPosAssert)));
    a->m_my_ac = actor;
    u32 pid = actor != nullptr ? actor->mBsPcId.get() : 0xFFFFFFFF;
    a->pm_angle = gabi::ea(angle);
    static_cast<cBgS_Chk*>(a)->mActorPid = pid; /* SetActorPid */
    a->pm_acch_cir = cir;
    a->pm_shape_angle = shape_angle;
    a->pm_speed = speed;
    a->m_tbl_size = tbl_size;
}

/* 024F034C */

void AcchRead::GroundCheckInit(dBgS* bgs) {
    auto* a = this;

    a->m_ground_h = -G_CM3D_F_INF;
    if (a->m_flags & dBgS_Acch::GRND_NONE)
        return;
    SetExtChk_GndRoof(gabi::ea(&a->m_gnd), gabi::ea(a));
    u32 f = a->m_flags;
    a->field_0xb0 = (f >> 5) & 1; /* ChkGroundHit */
    a->m_flags =
        f & ~(u32)(dBgS_Acch::GROUND_HIT | dBgS_Acch::GROUND_LANDING | dBgS_Acch::GROUND_AWAY);
}

/* roof check of CrrPos / GroundRoofProc: SetExtChk, ClrRoofHit, SetPos, RoofChk */
static inline f32 RoofCheck(dBgS_Acch* a, dBgS* bgs) {
    u32 b = gabi::ea(a);
    SetExtChk_GndRoof(gabi::ea(static_cast<cBgS_Chk*>(&a->m_roof)), b);
    a->m_flags &= ~(u32)dBgS_Acch::ROOF_HIT;
    cXyz* p = a->pm_pos;
    f32 y = p->y, x = p->x, z = p->z;
    a->m_roof.m_pos.x = x;
    a->m_roof.m_pos.y = y;
    a->m_roof.m_pos.z = z;
    return acch_dBgS_RoofChk(bgs, gabi::ea(&a->m_roof));
}

/* 024EFF50 */
// HD: GetTriPla's result is null-checked before the plane copy

void AcchRead::GroundCheck(dBgS* bgs) {
    auto* a = this;

    if (a->m_flags & dBgS_Acch::GRND_NONE)
        return;
    cXyz* p = a->pm_pos;
    f32 x = p->x, y = p->y, z = p->z;
    f32 t = gabi::fsubs_ppc(a->m_ground_check_offset, a->m_ground_up_h);
    t = gabi::fadds_ppc(t, a->m_ground_up_h_diff);
    y = gabi::fadds_ppc(y, t);
    a->m_ground_up_h_diff = 0.0f;
    a->m_gnd.m_pos.x = x; /* m_gnd.SetPos */
    a->m_gnd.m_pos.y = y;
    a->m_gnd.m_pos.z = z;
    f32 gh = acch_cBgS_GroundCross(bgs, gabi::ea(&a->m_gnd));
    a->m_ground_h = gh;
    if (gh != -G_CM3D_F_INF) {
        f32 h = gabi::fadds_ppc(gh, a->m_ground_up_h);
        a->field_0xb8 = h;
        if (h > a->field_0xb4) {
            a->pm_pos->y = h;
            if (a->pm_speed.v != 0)
                a->pm_speed->y = 0.0f;
            u32 pla =
                acch_cBgS_GetTriPla(bgs, static_cast<cBgS_PolyInfo*>(&a->m_gnd)->mBgIndex.get(),
                                    static_cast<cBgS_PolyInfo*>(&a->m_gnd)->mPolyIndex.get());
            if (pla != 0) {
                reinterpret_cast<AcchPlaneWords*>(a->m_pla)->mNormal[0] =
                    gabi::at<AcchPlaneWords>(pla)->mNormal[0].get();
                reinterpret_cast<AcchPlaneWords*>(a->m_pla)->mNormal[1] =
                    gabi::at<AcchPlaneWords>(pla)->mNormal[1].get();
                reinterpret_cast<AcchPlaneWords*>(a->m_pla)->mNormal[2] =
                    gabi::at<AcchPlaneWords>(pla)->mNormal[2].get();
                reinterpret_cast<cM3dGPla*>(a->m_pla)->mD = gabi::at<cM3dGPla>(pla)->mD.get();
            }
            a->m_flags |= dBgS_Acch::GROUND_FIND | dBgS_Acch::GROUND_HIT;
            acch_dBgS_RideCallBack(bgs, gabi::ea(static_cast<cBgS_PolyInfo*>(&a->m_gnd)),
                                   a->m_my_ac.v);
            if (!a->field_0xb0)
                a->m_flags |= dBgS_Acch::GROUND_LANDING;
        }
    }
    if (a->field_0xb0 && !(a->m_flags & dBgS_Acch::GROUND_HIT))
        a->m_flags |= dBgS_Acch::GROUND_AWAY;
}

/* 024F03A0 */

void AcchRead::GroundRoofProc(dBgS* bgs) {
    auto* a = this;

    if (a->m_ground_h != -G_CM3D_F_INF) {
        f32 c4 = a->field_0xC4;
        if (a->field_0xb8 < c4) {
            cXyz* p = a->pm_pos;
            if (c4 < p->y)
                p->y = c4;
        }
        if (!(a->m_flags & dBgS_Acch::ROOF_NONE)) {
            if (!(a->m_ground_h < a->m_roof_height)) {
                a->m_roof_height = RoofCheck(a, bgs);
            }
        }
    }
}

/* 024F00CC */
// HD: the line check is a plain cBgS_LinChk (0x58) without the SetSttsGroundOff, and GetTriPla's
// result is null-checked (null: treated as ground)

void AcchRead::LineCheck(dBgS* bgs) {
    auto* a = this;

    // HD: one original frame slot per local, reused without reinitializing between iterations.
    gabi::Local<cBgS_LinChk> linChk;
    gabi::Local<cXyz> old_pos, pos;
    for (s32 i = 0; i < a->m_tbl_size; i++) {
        u32 l = gabi::ea(linChk.get());
        acch_cBgS_LinChk_ct(l);
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
        acch_cBgS_LinChk_Set2(l, gabi::ea(old_pos.get()), gabi::ea(pos.get()),
                              static_cast<cBgS_Chk*>(a)->mActorPid.get());
        /* SetExtChk */
        static_cast<cBgS_Chk*>(linChk.get())->mPolyPassChk =
            static_cast<cBgS_Chk*>(a)->mPolyPassChk.get();
        static_cast<cBgS_Chk*>(linChk.get())->mSameActorChk =
            static_cast<cBgS_Chk*>(a)->mSameActorChk.get();
        static_cast<cBgS_Chk*>(linChk.get())->mActorPid =
            static_cast<cBgS_Chk*>(a)->mActorPid.get();
        static_cast<cBgS_Chk*>(linChk.get())->mGrpPassChk =
            static_cast<cBgS_Chk*>(a)->mGrpPassChk.get();
        if (acch_cBgS_LineCross(bgs, l)) {
            a->pm_pos->copy(linChk->mLin.mEnd); /* GetCross */
            a->m_flags |= dBgS_Acch::LINE_CHECK_HIT;
            u32 out = a->pm_out_poly_info;
            if (out != 0) { /* SetPolyInfo */
                auto* outPoly = gabi::at<cBgS_PolyInfo>(out);
                outPoly->mPolyIndex = static_cast<cBgS_PolyInfo*>(linChk.get())->mPolyIndex.get();
                outPoly->mBgIndex = static_cast<cBgS_PolyInfo*>(linChk.get())->mBgIndex.get();
                outPoly->mpBgW = static_cast<cBgS_PolyInfo*>(linChk.get())->mpBgW.get();
                outPoly->mActorId = static_cast<cBgS_PolyInfo*>(linChk.get())->mActorId.get();
            }
            u32 pla =
                acch_cBgS_GetTriPla(bgs, static_cast<cBgS_PolyInfo*>(linChk.get())->mBgIndex.get(),
                                    static_cast<cBgS_PolyInfo*>(linChk.get())->mPolyIndex.get());
            if (pla != 0 &&
                gabi::at<cM3dGPla>(pla)->mNormal.y.get() < 0.5f) { /* !cBgW_CheckBGround */
                acch_PSVECAdd(a->pm_pos.v, pla, a->pm_pos.v);
                f32 nz = gabi::at<cM3dGPla>(pla)->mNormal.z.get();
                f32 nx = gabi::at<cM3dGPla>(pla)->mNormal.x.get();
                f64 len = acch_sqrtf(gabi::fmadds(nx, nx, nz * nz)); /* std::sqrtf */
                if (!(std::fabs(len) < (f64)3.814697265625e-06f)) {  /* !cM3d_IsZero */
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
                a->GroundCheck(bgs);
            }
        }
        static_cast<cBgS_PolyInfo*>(linChk.get())->__vtbl = gabi::ea(&l_linPolyInfoVtable);
        acch_cBgS_LinChk_dt(l, 0);
    }
}

/* 024F08A8 */

void AcchRead::CrrPos(dBgS* bgs) {
    auto* a = this;

    if (a->m_flags & UNK_1)
        return;
    if (a->pm_pos.v == 0)
        JUT_ASSERT_fail(STR(gabi::ea(&l_crrAssertFile)), 0x21F, STR(gabi::ea(&l_crrPosAssert)));
    if (a->pm_old_pos.v == 0)
        JUT_ASSERT_fail(STR(gabi::ea(&l_crrAssertFile)), 0x220, STR(gabi::ea(&l_crrOldPosAssert)));
    CheckPos(a, 0x248, 0x249, 0x24A, 0x24C);

    u32 pos0 = a->pm_pos.v;
    u32 angle = a->pm_angle;
    u32 shape = a->pm_shape_angle;
    acch_dBgS_MoveBgCrrPos(bgs, gabi::ea(static_cast<cBgS_PolyInfo*>(&a->m_gnd)),
                           (a->m_flags >> 5) & 1, pos0, angle, shape);

    a->GroundCheckInit(bgs);
    a->Init();

    f32 lowH_R = a->GetWallAllLowH_R();
    gabi::Local<cXyz> oldXZ, posXZ; /* abs2XZ temporaries */
    cXyz* op = a->pm_old_pos;
    cXyz* pp = a->pm_pos;
    oldXZ->x = op->x;
    oldXZ->z = op->z;
    oldXZ->y = 0.0f;
    posXZ->y = 0.0f;
    posXZ->z = pp->z;
    posXZ->x = pp->x;
    f32 distXZ2 = acch_PSVECSquareDistance(gabi::ea(oldXZ.get()), gabi::ea(posXZ.get()));
    f32 posY = a->pm_pos->y;
    f32 distY = a->pm_old_pos->y - posY;
    f32 lowH = a->GetWallAllLowH();
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
            a->LineCheck(bgs);
            f = a->m_flags;
        }
    }
    if (!(f & dBgS_Acch::WALL_NONE)) {
        acch_dBgS_WallCorrect(bgs, a);
        f = a->m_flags;
    }
    if ((f & dBgS_Acch::WALL_HIT) && ranLineCheck)
        a->LineCheck(bgs);

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
        if (!hit)
            f = a->m_flags;
    }
    if (!(f & dBgS_Acch::GRND_NONE)) {
        a->m_flags = f & ~(u32)dBgS_Acch::GROUND_FIND;
        a->GroundCheck(bgs);
        a->GroundRoofProc(bgs);
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
        reinterpret_cast<AcchWaterRead*>(&a->m_wtr)->m_height = -G_CM3D_F_INF; /* m_wtr.SetHeight */
        a->m_flags = f & ~(u32)(dBgS_Acch::WATER_HIT | dBgS_Acch::WATER_IN);
        s32 room_no = acch_dBgS_GetRoomId(bgs, gabi::ea(static_cast<cBgS_PolyInfo*>(&a->m_gnd)));
        if (a->m_ground_h != -G_CM3D_F_INF && (u32)room_no < 64) {
            dComIfGp_get();
            u32 bgw = gabi::at<AcchRoomBg>(gabi::ea(&l_roomBg.mRoom[0]) +
                                           (u32)room_no * sizeof(AcchRoomBg))
                          ->mpBgW.get(); /* dComIfGp_roomControl_getBgW */
            if (bgw != 0) {
                gabi::Local<be<f32>> top, under;
                acch_cBgW_GetTopUnder(bgw, gabi::ea(top.get()), gabi::ea(under.get()));
                cXyz* p = a->pm_pos;
                u32 gx = reinterpret_cast<be<u32>*>(&p->x)->get(),
                    gz = reinterpret_cast<be<u32>*>(&p->z)->get();
                f32 fUnder = *under;
                /* m_wtr.Set(ground, top), SetPassChkInfo */
                for (u32 k = 0; k < 7; k++)
                    reinterpret_cast<AcchPassFlags*>(
                        &reinterpret_cast<AcchWaterRead*>(&a->m_wtr)->mbObjThrough)
                        ->mPass[k] =
                        reinterpret_cast<AcchPassFlags*>(&a->mbObjThrough)->mPass[k].get();
                reinterpret_cast<AcchWaterRead*>(&a->m_wtr)->m_pos.y = fUnder;
                reinterpret_cast<AcchWaterRead*>(&a->m_wtr)->m_pos.x = gabi::f32_from_bits(gx);
                reinterpret_cast<AcchWaterRead*>(&a->m_wtr)->m_top = (f32)*top;
                reinterpret_cast<AcchWaterRead*>(&a->m_wtr)->m_pos.z = gabi::f32_from_bits(gz);
                if (acch_dBgS_SplGrpChk(bgs, gabi::ea(&a->m_wtr))) {
                    a->m_flags |= dBgS_Acch::WATER_HIT;
                    f32 h = reinterpret_cast<AcchWaterRead*>(&a->m_wtr)->m_height.get();
                    if (h > a->pm_pos->y)
                        a->m_flags |= dBgS_Acch::WATER_IN;
                    if (h < fUnder) {
                        JUT_ASSERT_fail(STR(gabi::ea(&l_crrAssertFile)), 0x2FF,
                                        STR(gabi::ea(&l_waterUnderAssert)));
                        h = reinterpret_cast<AcchWaterRead*>(&a->m_wtr)->m_height.get();
                    }
                    if (h > (f32)*top)
                        JUT_ASSERT_fail(STR(gabi::ea(&l_crrAssertFile)), 0x300,
                                        STR(gabi::ea(&l_waterTopAssert)));
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
        if (acch_daSea_ChkArea(p->x, p->z)) {
            p = a->pm_pos;
            sea = acch_daSea_calcWave(p->x, p->z);
            a->m_sea_height = (f32)sea;
        } else {
            sea = a->m_sea_height;
        }
        if (a->m_flags & dBgS_Acch::SEA_WATER_HEIGHT) {
            f32 wtr = reinterpret_cast<AcchWaterRead*>(&a->m_wtr)->m_height.get();
            if (sea < wtr) {
                sea = wtr;
                a->m_sea_height = wtr;
            }
        }
        if (sea > a->pm_pos->y)
            a->m_flags |= dBgS_Acch::SEA_IN;
    }

    CheckPos(a, 0x33D, 0x33E, 0x33F, 0x341);
}

/* 024F1070 */

f32 AcchRead::GetWallAllR() {
    auto* a = this;

    f32 max = 0.0f;
    s32 n = a->m_tbl_size;
    dBgS_AcchCir* cir = n > 0 ? a->pm_acch_cir.get() : nullptr;
    for (s32 i = 0; i < n; i++) {
        if (max < cir[i].m_wall_r)
            max = cir[i].m_wall_r;
    }
    return max;
}

/* 024F10A8 */

void AcchRead::SetWallCir() {
    auto* a = this;

    for (s32 i = 0; i < a->m_tbl_size; i++) {
        dBgS_AcchCir& c = a->pm_acch_cir.get()[i];
        cXyz* p = a->pm_pos;
        f32 y = gabi::fadds_ppc(p->y, c.m_wall_h);
        acch_cM3dGCir_Set(gabi::ea(&c.m_cir), p->x, p->z, y, c.m_wall_r); /* SetCir */
    }
}

/* 024F1134 */

void AcchRead::CalcWallBmdCyl() {
    auto* a = this;

    s32 n = a->m_tbl_size;
    if (!(n > 0)) {
        acch_cM3dGCyl_Set(gabi::ea(a->m_wall_cyl), a->pm_pos.v, 0.0, 0.0f);
        return;
    }
    f64 all_r = acch_GetWallAllRUnrounded(a); /* GetWallAllR (f1 passed on as is) */
    dBgS_AcchCir* cir = a->pm_acch_cir.get();
    f32 min_h = cir[0].m_wall_h;
    f32 max_h = min_h;
    for (s32 i = 0; i < n; i++) {
        if (min_h > cir[i].m_wall_h)
            min_h = cir[i].m_wall_h;
        if (max_h < cir[i].m_wall_h)
            max_h = cir[i].m_wall_h;
    }
    gabi::Local<cXyz> center;
    cXyz* p = a->pm_pos;
    center->x = p->x;
    f32 y = p->y;
    center->z = p->z;
    center->y = gabi::fadds_ppc(y, min_h);
    acch_cM3dGCyl_Set(gabi::ea(a->m_wall_cyl), gabi::ea(center.get()), all_r,
                      gabi::fsubs_ppc(max_h, min_h));
}

/* 024F12A8 */

void AcchRead::SetGroundUpY(f32 y) {
    auto* a = this;

    f32 old = a->m_ground_up_h;
    a->m_ground_up_h = y;
    a->m_ground_up_h_diff = gabi::fsubs_ppc(y, old);
}

/* 024F07D4 */

f32 AcchRead::GetWallAllLowH() {
    auto* a = this;

    s32 n = a->m_tbl_size;
    if (!(n > 0))
        return 0.0f;
    dBgS_AcchCir* cir = a->pm_acch_cir.get();
    f32 min = cir[0].m_wall_h;
    for (s32 i = 1; i < n; i++) {
        if (min > cir[i].m_wall_h)
            min = cir[i].m_wall_h;
    }
    return min;
}

/* 024F0768 */

f32 AcchRead::GetWallAllLowH_R() {
    auto* a = this;

    s32 n = a->m_tbl_size;
    if (!(n > 0))
        return 0.0f;
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

/* 024F0F98 */

bool AcchRead::GetOnePolyInfo(u8* dst) {
    auto* a = this;

    u32 d = gabi::ea(dst);
    auto setPolyInfo = [d](u32 s) {
        auto* dest = gabi::at<cBgS_PolyInfo>(d);
        auto* source = gabi::at<cBgS_PolyInfo>(s);
        dest->mPolyIndex = source->mPolyIndex.get();
        dest->mBgIndex = source->mBgIndex.get();
        dest->mpBgW = source->mpBgW.get();
        dest->mActorId = source->mActorId.get();
    };
    u32 f = a->m_flags;
    if (f & dBgS_Acch::GROUND_HIT) {
        setPolyInfo(gabi::ea(static_cast<cBgS_PolyInfo*>(&a->m_gnd)));
        return false;
    }
    if (f & dBgS_Acch::WALL_HIT) {
        for (s32 i = 0; i < a->m_tbl_size; i++) {
            dBgS_AcchCir& c = a->pm_acch_cir.get()[i];
            if (c.ChkWallHit())
                setPolyInfo(gabi::ea(&c));
        }
        return false;
    }
    if (f & dBgS_Acch::ROOF_HIT) {
        setPolyInfo(gabi::ea(&a->m_roof));
        return false;
    }
    return true;
}

/* 024F1204 */
// HD signature GetWallAddY(Vec&)

f32 AcchRead::GetWallAddY(void* vec) {
    auto* a = this;

    if (!(a->m_flags & dBgS_Acch::GROUND_FIND))
        return 0.0f;
    if (reinterpret_cast<cM3dGPla*>(a->m_pla)->mNormal.y.get() < 0.5f)
        return 0.0f;
    gabi::Local<be<f32>> cross_y;
    if (!acch_cM3dGPla_getCrossYLessD(gabi::ea(a->m_pla), vec, gabi::ea(cross_y.get())))
        return 0.0f;
    f32 c = *cross_y;
    if (c > 0.0f)
        c = 0.0f;
    return -c;
}

// HD: compiler-generated header-static initialization.
/* 024EFD08 */
// static initialisers (header statics)

static void __sinit_d_bg_s_acch_cpp() {

    sinit_header_statics(gabi::ea(&l_headerStatics), gabi::ea(&l_headerRecords));
}

/* 024F12BC */
// the TU's second header-statics initializer: the same
// header statics block at 1046ED0C (records 101D524C)

static void __sinit_d_bg_s_acch_cpp_2() {

    sinit_header_statics(gabi::ea(&l_headerStatics2), gabi::ea(&l_headerRecords2));
}

} // namespace d_bg_s_acch_cpp
using namespace d_bg_s_acch_cpp;

/* WWHD glue
data l_roomBg 1047E8F4
data l_linVtable 10043704
data l_linPolyInfoVtable 10043724
data l_baseGroupVtable 10043734
data l_groundCheckVtable 10043744
data l_groundPolyInfoVtable 10043754
data l_groundGroupVtable 10043764
data l_groundPassVtable 10043774
data l_roofCheckVtable 10043784
data l_roofPassVtable 100437A4
data l_waterCheckVtable 100437B4
data l_waterPassVtable 100437D4
data l_circlePolyInfoVtable 10043814
data l_setAssertFile 1004383C
data l_setPosAssert 1004384C
data l_setOldPosAssert 1004385C
data l_crrAssertFile 10043880
data l_crrPosAssert 10043890
data l_crrOldPosAssert 100438A0
data l_waterUnderAssert 100438B4
data l_waterTopAssert 100438D4
data l_posXAssert 100438F0
data l_posYAssert 10043934
data l_posZAssert 10043978
data l_posRangeAssert 100439BC
data l_acchCheckVtable 10043A50
data l_acchGroupVtable 10043A60
data l_acchPassVtable 10043A70
data l_headerRecords 101D51A8
data l_headerRecords2 101D524C
data l_headerStatics 1046ECF0
data l_headerStatics2 1046ED0C
ext 02008974 f32 acch_cBgS_GroundCross(dBgS*, u32)
ext 020084C8 u32 acch_cBgS_GetTriPla(dBgS*, u32, u32)
ext 024EFBC0 void acch_dBgS_RideCallBack(dBgS*, u32, u32)
ext 024EF6E8 f32 acch_dBgS_RoofChk(dBgS*, u32)
ext 024EF7C0 bool acch_dBgS_SplGrpChk(dBgS*, u32)
ext 024EF5CC void acch_dBgS_WallCorrect(dBgS*, dBgS_Acch*)
ext 024EF130 s32 acch_dBgS_GetRoomId(dBgS*, u32)
ext 024EF968 void acch_dBgS_MoveBgCrrPos(dBgS*, u32, u32, u32, u32, u32)
ext 02008FEC void acch_cBgS_LinChk_ct(u32)
ext 0200909C void acch_cBgS_LinChk_Set2(u32, u32, u32, u32)
ext 02008860 bool acch_cBgS_LineCross(dBgS*, u32)
ext 02008B4C void acch_cBgS_LinChk_dt(u32, s32)
ext 028E8D88 void acch_PSVECAdd(u32, u32, u32)
ext 028E8DE8 f32 acch_PSVECSquareDistance(u32, u32)
ext 0200A3FC void acch_cBgW_GetTopUnder(u32, u32, u32)
ext 0246B6A4 BOOL acch_daSea_ChkArea(f32, f32)
ext 0246BA0C f64 acch_daSea_calcWave(f32, f32)
ext 020180A8 void acch_cM3dGCir_Set(u32, f32, f32, f32, f32)
ext 0201864C void acch_cM3dGCyl_Set(u32, u32, f64, f32)
ext 02018B38 BOOL acch_cM3dGPla_getCrossYLessD(u32, void*, u32)
ext 02008DAC void acch_cBgS_Chk_dt(u32, s32)
ext 02018048 void acch_cM3dGCir_ct(u32)
ext 028F4384 f64 acch_sqrtf(f32)
ext 02008B60 void acch_cBgS_Chk_ct(u32)
ext 02018590 void acch_cM3dGCyl_ct(u32)
ext 020189A0 void acch_cM3dGPla_ct(u32)
ext 02008E0C void acch_cBgS_GndChk_ct(u32)
ext 024EE7AC void acch_dBgS_RoofChk_ct(u32)
ext 024F22DC void acch_dBgS_WtrChk_ct(u32)
ext 024F1070 f64 acch_GetWallAllRUnrounded(dBgS_Acch*)
*/

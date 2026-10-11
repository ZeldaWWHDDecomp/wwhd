// Background collision mesh.
#include "d/d_bg_w.h"
using namespace gabi;
namespace background_world {
/* 024F23F4 */
// HD: GHS allocating constructor, including its verified store order.
static dBgW *dBgW_ct(dBgW *w) {
    if (w == nullptr) {
        w = (dBgW *)operator_new(sizeof(dBgW));
        if (w == nullptr)
            return w;
    }
    cBgW_ct(w);
    w->mpPushPullCb = 0;
    w->__vtbl = worldVtable;
    w->mOldRotY = 0;
    w->mGrpRoomInf = 0xFF;
    w->mRotYDelta = 0;
    w->mFlag = 0;
    w->mRoomId = 0xFFFF;
    w->mpRideCb = 0;
    w->m_crr_func = 0;
    return w;
}

/* 024F2478 */
static dBgW *dBgW_NewSet(u32 bgd, u32 flag, u32 mtx) {
    dBgW *rt = dBgW_ct(nullptr);
    if (rt == nullptr)
        return nullptr;
    if (cBgW_Set(rt, bgd, flag, mtx))
        return nullptr;
    return rt;
}

/* 024F2514 */
void background_world::dBgW::positionWallCorrect(dBgS_Acch *acch, f32 dist, cBgW_Plane *plane,
                                                 cXyz *pupper_pos, f32 speed) {
    acch->m_flags |= dBgS_Acch::WALL_HIT;
    // HD: fused arithmetic and post-assert reloads retain the PowerPC result.
    f32 move = speed * dist;
    f32 x = gabi::fmadds(move, plane->mNormal.x, pupper_pos->x);
    pupper_pos->x = x;
    f32 z = gabi::fmadds(move, plane->mNormal.z, pupper_pos->z);
    pupper_pos->z = z;
    if (std::isnan(x)) {
        assertion(wallCorrectionFile, 0xD0, wallCorrectionXCheck);
        z = pupper_pos->z;
    }
    if (std::isnan(z))
        assertion(wallCorrectionFile, 0xD1, wallCorrectionZCheck);
}

/* RwgWallCorrect, after a correction: dBgS_Acch::CalcMovePosWork, SetWallCirHit, SetWallPolyIndex and
 * SetWallAngleY (inline, with the asserts of d_bg_s_acch.h and c_bg_s_poly_info.h) */
/* inlined into RwgWallCorrect */
static inline void wall_hit(dBgS_Acch *acch, s32 cir_index, u32 off, u32 poly, u32 tri) {
    dBgS_Acch_SetWallCir(acch);
    cM3dGLin_SetStartEnd(gabi::ea(&acch->m_lin), acch->pm_old_pos.v, acch->pm_pos.v);
    dBgS_Acch_CalcWallBmdCyl(acch);
    {
        u32 c = acch->pm_acch_cir.v + off;
        at<dBgS_AcchCir>(c)->m_flags = at<dBgS_AcchCir>(c)->m_flags | dBgS_AcchCir::WALL_HIT;
    }
    if (!(cir_index <= acch->m_tbl_size))
        assertion(acchHeaderFile, 0x274, circleIndexCheck);
    u32 c1 = acch->pm_acch_cir.v + off;
    s32 bg = acch->m_bg_index;
    u32 f78 = acch->field_0x78;
    u32 ap = acch->m_ap_id;
    if (bg < 0)
        assertion(polyInfoHeaderFile, 0x59, backgroundIndexCheck);
    at<dBgS_AcchCir>(c1)->mActorId = ap;
    at<dBgS_AcchCir>(c1)->mBgIndex = (u16)bg;
    at<dBgS_AcchCir>(c1)->mpBgW = f78;
    u32 c2 = acch->pm_acch_cir.v + off;
    if ((s32)poly < 0)
        assertion(polyInfoHeaderFile, 0x7B, wallPolyIndexCheck);
    at<dBgS_AcchCir>(c2)->mPolyIndex = (u16)poly;
    s16 ang = cM_atan2s(at<cBgW_Plane>(tri)->mNormal.x, at<cBgW_Plane>(tri)->mNormal.z);
    circle(acch, off)->m_wall_angle_y = ang;
}

/* 024F25D4 */
bool background_world::dBgW::RwgWallCorrect(dBgS_Acch *pwi, u32 i_poly_idx) {
    bool correct = false;
    gabi::Local<cXyz> sp50;
    gabi::Local<be<f32>> spC8, spCC, spD0, spF0, spF4;
    while (true) {
        u32 rwg_elm = tableAddress(this->pm_rwg, i_poly_idx);
        if (!dispatchPoly(this, i_poly_idx, pwi->mPolyPassChk)) {
            u32 tri = tableAddress(this->pm_tri, i_poly_idx);
            f32 nz0 = at<cBgW_Plane>(tri)->mNormal.z;
            f32 nx0 = at<cBgW_Plane>(tri)->mNormal.x;
            f32 sp68 = std_sqrtf(gabi::fmadds(nx0, nx0, nz0 * nz0));
            if (!cM3d_IsZero(sp68)) {
                s32 cir_index = 0;
                f32 sp6C = 1.0f / sp68;
                u32 tri_data = tableAddress(this->pm_bgd.get()->m_t_tbl, i_poly_idx);
                u32 off = 0;
                for (; cir_index < pwi->m_tbl_size; cir_index++, off += u32(sizeof(dBgS_AcchCir))) {
                    u32 cir = pwi->pm_acch_cir.v + off;
                    f32 sp78 = gabi::fmuls_ppc(sp6C, at<dBgS_AcchCir>(cir)->m_wall_r);
                    sp50->y = 0.0f;
                    sp50->x = gabi::fmuls_ppc(at<cBgW_Plane>(tri)->mNormal.x, sp78);
                    sp50->z = gabi::fmuls_ppc(at<cBgW_Plane>(tri)->mNormal.z, sp78);

                    f32 sp7C;
                    if (!(at<dBgS_AcchCir>(cir)->m_flags & dBgS_AcchCir::WALL_H_DIRECT)) {
                        u32 speed = pwi->pm_speed.v;
                        u32 pos = pwi->pm_pos.v;
                        f32 wall_h = at<dBgS_AcchCir>(cir)->m_wall_h;
                        f32 pos_y = at<cXyz>(pos)->y;
                        f32 speed_y = speed != 0 ? at<cXyz>(speed)->y : 0.0f;
                        f32 base = gabi::fadds_ppc(pos_y, wall_h);
                        f32 add = dBgS_Acch_GetWallAddY(pwi, gabi::ea(sp50.get()), cir_index);
                        sp7C = gabi::fsubs_ppc(gabi::fadds_ppc(base, add), speed_y);
                    } else {
                        sp7C = at<dBgS_AcchCir>(cir)->m_wall_h_direct;
                    }

                    u32 vtx = this->pm_vtx_tbl.v;
                    u32 v0 = vtx + at<cBgD_Tri_t>(tri_data)->vtx0 * u32(sizeof(cXyz));
                    f32 s0 = at<cXyz>(v0)->y - sp7C;
                    u32 v1 = vtx + at<cBgD_Tri_t>(tri_data)->vtx1 * u32(sizeof(cXyz));
                    f32 s1 = at<cXyz>(v1)->y - sp7C;
                    u32 v2 = vtx + at<cBgD_Tri_t>(tri_data)->vtx2 * u32(sizeof(cXyz));
                    f32 s2 = at<cXyz>(v2)->y - sp7C;

                    if ((s0 > 0.0f && s1 > 0.0f && s2 > 0.0f) || (s0 < 0.0f && s1 < 0.0f && s2 < 0.0f))
                        continue;

                    s32 zeros = 0;
                    if (cM3d_IsZero(s0))
                        zeros++;
                    if (cM3d_IsZero(s1))
                        zeros++;
                    if (cM3d_IsZero(s2))
                        zeros++;
                    if (zeros == 1)
                        continue;

                    // HD: negated comparisons preserve the unordered NaN branches.
                    s32 sp80;
                    f32 a, b, sp90, sp94;
                    if ((s0 > 0.0f && !(s1 > 0.0f) && !(s2 > 0.0f)) ||
                        (s0 < 0.0f && !(s1 < 0.0f) && !(s2 < 0.0f))) {
                        sp80 = 0;
                        a = s1;
                        b = s2;
                        sp90 = s0 - s1;
                        sp94 = s0 - s2;
                    } else if ((s1 > 0.0f && !(s0 > 0.0f) && !(s2 > 0.0f)) ||
                               (s1 < 0.0f && !(s0 < 0.0f) && !(s2 < 0.0f))) {
                        sp80 = 1;
                        a = s0;
                        b = s2;
                        sp90 = s1 - s0;
                        sp94 = s1 - s2;
                    } else {
                        sp80 = 2;
                        a = s0;
                        b = s1;
                        sp90 = s2 - s0;
                        sp94 = s2 - s1;
                    }
                    if (cM3d_IsZero(sp90) || cM3d_IsZero(sp94))
                        continue;
                    f32 sp9C = -(b / sp94);
                    f32 sp98 = -(a / sp90);

                    f32 vtx0_x = at<cXyz>(v0)->x, vtx0_z = at<cXyz>(v0)->z;
                    f32 vtx1_x = at<cXyz>(v1)->x, vtx1_z = at<cXyz>(v1)->z;
                    f32 vtx2_x = at<cXyz>(v2)->x, vtx2_z = at<cXyz>(v2)->z;
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

                    u32 pos = pwi->pm_pos.v;
                    bool sp107 = cM3d_Len2dSqPntAndSegLine(at<cXyz>(pos)->x, at<cXyz>(pos)->z, cx0, cy0, cx1,
                                                           cy1, gabi::ea(spCC.get()), gabi::ea(spD0.get()),
                                                           gabi::ea(spC8.get()));
                    pos = pwi->pm_pos.v;
                    f32 pos_x = at<cXyz>(pos)->x, pos_z = at<cXyz>(pos)->z;
                    f32 spDC = circle(pwi, off)->m_wall_rr;
                    f32 c8 = *spC8;
                    f32 spD4 = *spCC - pos_x;
                    f32 spD8 = *spD0 - pos_z;
                    if (c8 > spDC)
                        continue;
                    sp50x = sp50->x;
                    sp50z = sp50->z;
                    if (gabi::fmadds(spD4, sp50x, spD8 * sp50z) < 0.0f)
                        continue;

                    if (sp107) {
                        f32 len = std_sqrtf(c8);
                        this->positionWallCorrect(pwi, sp6C, at<cBgW_Plane>(tri), at<cXyz>(pos), len);
                        wall_hit(pwi, cir_index, off, i_poly_idx, tri);
                        correct = true;
                    } else {
                        cx0 = cx0 - sp50x;
                        cy0 = cy0 - sp50z;
                        cx1 = cx1 - sp50x;
                        cy1 = cy1 - sp50z;
                        f32 spE0 = cM3d_Len2dSq(cx0, cy0, pos_x, pos_z);
                        pos = pwi->pm_pos.v;
                        f32 spE4 = cM3d_Len2dSq(cx1, cy1, at<cXyz>(pos)->x, at<cXyz>(pos)->z);
                        f32 onx = -(f32)at<cBgW_Plane>(tri)->mNormal.x;
                        f32 ony = -(f32)at<cBgW_Plane>(tri)->mNormal.z;
                        if (cM3d_IsZero(onx) && cM3d_IsZero(ony))
                            assertion(wallCollisionFile, 0x1CE, wallNormalCheck);

                        f32 ex, ez;
                        s32 l0, l1;
                        if (spE0 < spE4) {
                            if (spE0 > spDC || std::fabs(spE0 - spDC) < 0.008f)
                                continue;
                            ex = cx0;
                            ez = cy0;
                            l0 = 0x1E4;
                            l1 = 0x1E5;
                        } else {
                            if (spE4 > spDC || std::fabs(spE4 - spDC) < 0.008f)
                                continue;
                            ex = cx1;
                            ez = cy1;
                            l0 = 0x20C;
                            l1 = 0x20D;
                        }
                        if (!(cir_index <= pwi->m_tbl_size))
                            assertion(acchHeaderFile, 0x2F9, circleIndexCheck);
                        cM2d_CrossCirLin(pwi->pm_acch_cir.v + off + u32(offsetof(dBgS_AcchCir, m_cir)), ex,
                                         ez, onx, ony, gabi::ea(spF0.get()), gabi::ea(spF4.get()));
                        pos = pwi->pm_pos.v;
                        at<cXyz>(pos)->x = at<cXyz>(pos)->x + (ex - *spF0);
                        pos = pwi->pm_pos.v;
                        at<cXyz>(pos)->z = at<cXyz>(pos)->z + (ez - *spF4);
                        pos = pwi->pm_pos.v;
                        if (std::isnan(at<cXyz>(pos)->x)) {
                            assertion(wallCollisionFile, l0, wallPositionXCheck);
                            pos = pwi->pm_pos.v;
                        }
                        if (std::isnan(at<cXyz>(pos)->z))
                            assertion(wallCollisionFile, l1, wallPositionZCheck);
                        wall_hit(pwi, cir_index, off, i_poly_idx, tri);
                        pwi->m_flags |= dBgS_Acch::WALL_HIT;
                        correct = true;
                    }
                }
            }
        }
        u16 next = at<cBgW_RwgElm>(rwg_elm)->next;
        if (next == dBgW::INVALID_INDEX)
            break;
        i_poly_idx = next;
    }
    return correct;
}

/* 024F326C */
bool background_world::dBgW::WallCorrectRp(dBgS_Acch *acch, s32 i) {
    if (!cM3d_Cross_AabCyl(tableAddress(this->m_nt_tbl, i), gabi::ea(acch->m_wall_cyl)))
        return false;
    u32 tree = tableAddress(this->pm_bgd.get()->m_tree_tbl, i);
    bool ret = false;
    if (at<cBgD_Tree_t>(tree)->mFlag & dBgW::LEAF) {
        u32 wall = block(this->pm_blk.v, at<cBgD_Tree_t>(tree)->mChild[0])->wall;
        if (wall != dBgW::INVALID_INDEX && this->RwgWallCorrect(acch, wall))
            ret = true;
        u32 roof = block(this->pm_blk.v, at<cBgD_Tree_t>(tree)->mChild[0])->roof;
        if (roof != dBgW::INVALID_INDEX && this->RwgWallCorrect(acch, roof))
            ret = true;
        return ret;
    }
    for (int k = 0; k < 8; k++) {
        u32 child = at<cBgD_Tree_t>(tree)->mChild[k];
        if (child != dBgW::INVALID_INDEX && this->WallCorrectRp(acch, child))
            ret = true;
    }
    return ret;
}

/* 024F3498 */
bool background_world::dBgW::WallCorrectGrpRp(dBgS_Acch *acch, s32 grp_id, s32 depth) {
    if (dispatchGroup(this, grp_id, acch->mGrpPassChk, depth))
        return false;
    if (!cM3d_Cross_AabCyl(tableAddress(this->pm_grp, grp_id), gabi::ea(acch->m_wall_cyl)))
        return false;
    bool ret = false;
    u32 tree_idx = group(this->pm_bgd.get()->m_g_tbl.v, grp_id)->m_tree_idx;
    if (tree_idx != dBgW::INVALID_INDEX && this->WallCorrectRp(acch, tree_idx))
        ret = true;
    depth++;
    u32 child_idx = group(this->pm_bgd.get()->m_g_tbl.v, grp_id)->m_first_child;
    while (child_idx != dBgW::INVALID_INDEX) {
        if (this->WallCorrectGrpRp(acch, child_idx, depth))
            ret = true;
        child_idx = group(this->pm_bgd.get()->m_g_tbl.v, child_idx)->m_next_sibling;
    }
    return ret;
}

/* 024F35B4 */
bool background_world::dBgW::RwgRoofChk(u32 poly_index, HeightCheck *check) {
    bool ret = false;
    gabi::Local<be<f32>> y;
    while (true) {
        u32 off = poly_index * u32(sizeof(cBgW_TriElm));
        if (cM3dGPla_getCrossY(this->pm_tri.v + off, gabi::ea(&check->m_pos), gabi::ea(y.get())) &&
            *y > check->m_pos.y && *y < check->m_height) {
            u32 tri = tableAddress(this->pm_bgd.get()->m_t_tbl, poly_index);
            u32 vtx = this->pm_vtx_tbl.v;
            if (cM3d_CrossY_Tri(vtx + at<cBgD_Tri_t>(tri)->vtx0 * u32(sizeof(cXyz)),
                                vtx + at<cBgD_Tri_t>(tri)->vtx1 * u32(sizeof(cXyz)),
                                vtx + at<cBgD_Tri_t>(tri)->vtx2 * u32(sizeof(cXyz)), this->pm_tri.v + off,
                                gabi::ea(&check->m_pos)) &&
                !dispatchPoly(this, poly_index, check->mCheck.mPolyPassChk)) {
                check->m_height = *y; /* SetNowY */
                if ((s32)poly_index < 0)
                    assertion(roofCollisionFile, 0x7B, roofPolyIndexCheck);
                check->mPolyInfo.mPolyIndex = (u16)poly_index;
                ret = true;
            }
        }
        u32 next = at<cBgW_RwgElm>(this->pm_rwg.v + poly_index * u32(sizeof(cBgW_RwgElm)))->next;
        if (next == dBgW::INVALID_INDEX)
            break;
        poly_index = next;
    }
    return ret;
}

/* 024F36E0 */
bool background_world::dBgW::RoofChkRp(HeightCheck *check, s32 i) {
    u32 node = tableAddress(this->m_nt_tbl, i);
    if (!cM3dGAab_CrossY(node, gabi::ea(&check->m_pos)) ||
        !cM3dGAab_UnderPlaneYUnder(node, check->m_height) || cM3dGAab_TopPlaneYUnder(node, check->m_pos.y))
        return false;
    u32 tree = tableAddress(this->pm_bgd.get()->m_tree_tbl, i);
    if (at<cBgD_Tree_t>(tree)->mFlag & dBgW::LEAF) {
        u32 roof = block(this->pm_blk.v, at<cBgD_Tree_t>(tree)->mChild[0])->roof;
        if (roof != dBgW::INVALID_INDEX && this->RwgRoofChk(roof, check))
            return true;
        return false;
    }
    bool ret = false;
    for (int k = 0; k < 8; k++) {
        u32 child = at<cBgD_Tree_t>(tree)->mChild[k];
        if (child != dBgW::INVALID_INDEX && this->RoofChkRp(check, child))
            ret = true;
    }
    return ret;
}

/* 024F3924 */
bool background_world::dBgW::RoofChkGrpRp(HeightCheck *check, s32 grp_id, s32 depth) {
    if (dispatchGroup(this, grp_id, check->mCheck.mGrpPassChk, depth))
        return false;
    u32 grp = tableAddress(this->pm_grp, grp_id);
    if (!cM3dGAab_CrossY(grp, gabi::ea(&check->m_pos)) || !cM3dGAab_UnderPlaneYUnder(grp, check->m_height) ||
        cM3dGAab_TopPlaneYUnder(grp, check->m_pos.y))
        return false;
    bool ret = false;
    u32 grpd = tableAddress(this->pm_bgd.get()->m_g_tbl, grp_id);
    u32 tree_idx = at<cBgD_Grp_t>(grpd)->m_tree_idx;
    if (tree_idx != dBgW::INVALID_INDEX && this->RoofChkRp(check, tree_idx))
        ret = true;
    depth++;
    u32 child_idx = at<cBgD_Grp_t>(grpd)->m_first_child;
    while (child_idx != dBgW::INVALID_INDEX) {
        if (this->RoofChkGrpRp(check, child_idx, depth))
            ret = true;
        child_idx = group(this->pm_bgd.get()->m_g_tbl.v, child_idx)->m_next_sibling;
    }
    return ret;
}

/* 024F3A60 */
bool background_world::dBgW::RwgSplGrpChk(u32 poly_index, HeightCheck *check) {
    bool ret = false;
    gabi::Local<be<f32>> y;
    while (true) {
        u32 off = poly_index * u32(sizeof(cBgW_TriElm));
        if (cM3dGPla_getCrossY(this->pm_tri.v + off, gabi::ea(&check->m_pos), gabi::ea(y.get())) &&
            *y < check->m_roof && *y > check->m_height) {
            u32 tri = tableAddress(this->pm_bgd.get()->m_t_tbl, poly_index);
            u32 vtx = this->pm_vtx_tbl.v;
            if (cM3d_CrossY_Tri(vtx + at<cBgD_Tri_t>(tri)->vtx0 * u32(sizeof(cXyz)),
                                vtx + at<cBgD_Tri_t>(tri)->vtx1 * u32(sizeof(cXyz)),
                                vtx + at<cBgD_Tri_t>(tri)->vtx2 * u32(sizeof(cXyz)), this->pm_tri.v + off,
                                gabi::ea(&check->m_pos)) &&
                !dispatchPoly(this, poly_index, check->mCheck.mPolyPassChk)) {
                check->m_height = *y; /* SetHeight */
                if ((s32)poly_index < 0)
                    assertion(specialCollisionFile, 0x7B, specialPolyIndexCheck);
                check->mPolyInfo.mPolyIndex = (u16)poly_index;
                ret = true;
            }
        }
        u32 next = at<cBgW_RwgElm>(this->pm_rwg.v + poly_index * u32(sizeof(cBgW_RwgElm)))->next;
        if (next == dBgW::INVALID_INDEX)
            break;
        poly_index = next;
    }
    return ret;
}

/* 024F3B8C */
bool background_world::dBgW::SplGrpChkRp(HeightCheck *check, s32 i) {
    u32 node = tableAddress(this->m_nt_tbl, i);
    if (!cM3dGAab_CrossY(node, gabi::ea(&check->m_pos)) || !cM3dGAab_UnderPlaneYUnder(node, check->m_roof) ||
        cM3dGAab_TopPlaneYUnder(node, check->m_height))
        return false;
    u32 tree = tableAddress(this->pm_bgd.get()->m_tree_tbl, i);
    if (at<cBgD_Tree_t>(tree)->mFlag & dBgW::LEAF) {
        u32 ground = block(this->pm_blk.v, at<cBgD_Tree_t>(tree)->mChild[0])->ground;
        if (ground != dBgW::INVALID_INDEX && this->RwgSplGrpChk(ground, check))
            return true;
        return false;
    }
    bool ret = false;
    for (int k = 0; k < 8; k++) {
        u32 child = at<cBgD_Tree_t>(tree)->mChild[k];
        if (child != dBgW::INVALID_INDEX && this->SplGrpChkRp(check, child))
            ret = true;
    }
    return ret;
}

/* 024F3DD4 */
bool background_world::dBgW::SplGrpChkGrpRp(HeightCheck *check, s32 grp_id, s32 depth) {
    if (dispatchGroup(this, grp_id, check->mCheck.mGrpPassChk, depth))
        return false;
    u32 grp = tableAddress(this->pm_grp, grp_id);
    if (!cM3dGAab_CrossY(grp, gabi::ea(&check->m_pos)) || !cM3dGAab_UnderPlaneYUnder(grp, check->m_roof) ||
        cM3dGAab_TopPlaneYUnder(grp, check->m_height))
        return false;
    bool ret = false;
    u32 grpd = tableAddress(this->pm_bgd.get()->m_g_tbl, grp_id);
    u32 tree_idx = at<cBgD_Grp_t>(grpd)->m_tree_idx;
    if (tree_idx != dBgW::INVALID_INDEX && this->SplGrpChkRp(check, tree_idx))
        ret = true;
    depth++;
    u32 child_idx = at<cBgD_Grp_t>(grpd)->m_first_child;
    while (child_idx != dBgW::INVALID_INDEX) {
        if (this->SplGrpChkGrpRp(check, child_idx, depth))
            ret = true;
        child_idx = group(this->pm_bgd.get()->m_g_tbl.v, child_idx)->m_next_sibling;
    }
    return ret;
}

/* 024F3F10 */
bool background_world::dBgW::RwgSphChk(u32 i_poly_idx, SphereCheck *check, u32 data) {
    u32 chk = gabi::ea(check);
    gabi::Local<cBgW_Triangle> tri;
    cM3dGTri_ct(gabi::ea(tri.get()));
    bool ret = false;
    while (true) {
        u32 rwg = tableAddress(this->pm_rwg, i_poly_idx);
        if (!dispatchPoly(this, i_poly_idx, check->mCheck.mPolyPassChk)) {
            u32 tri_t = tableAddress(this->pm_bgd.get()->m_t_tbl, i_poly_idx);
            u32 vtx = this->pm_vtx_tbl.v;
            cM3dGTri_setBg(gabi::ea(tri.get()), vtx + at<cBgD_Tri_t>(tri_t)->vtx0 * u32(sizeof(cXyz)),
                           vtx + at<cBgD_Tri_t>(tri_t)->vtx1 * u32(sizeof(cXyz)),
                           vtx + at<cBgD_Tri_t>(tri_t)->vtx2 * u32(sizeof(cXyz)),
                           tableAddress(this->pm_tri, i_poly_idx));
            if (cM3d_Cross_SphTri(chk, gabi::ea(tri.get()), 0)) {
                u32 cb = check->mpCallback;
                if (cb != 0) {
                    u32 pla = tableAddress(this->pm_tri, i_poly_idx);
                    gabi::call_ptr(cb, chk, (u32)this->pm_vtx_tbl.v, (u32)at<cBgD_Tri_t>(tri_t)->vtx0,
                                   (u32)at<cBgD_Tri_t>(tri_t)->vtx1, (u32)at<cBgD_Tri_t>(tri_t)->vtx2, pla,
                                   data);
                }
                if ((s32)i_poly_idx < 0)
                    assertion(sphereCollisionFile, 0x7B, spherePolyIndexCheck);
                ret = true;
                check->mPolyInfo.mPolyIndex = (u16)i_poly_idx;
            }
        }
        u32 next = at<cBgW_RwgElm>(rwg)->next;
        if (next == dBgW::INVALID_INDEX)
            break;
        i_poly_idx = next;
    }
    return ret;
}

/* 024F4050 */
bool background_world::dBgW::SphChkRp(SphereCheck *check, u32 user, s32 i) {
    u32 chk = gabi::ea(check);
    if (!cM3d_Cross_AabSph(tableAddress(this->m_nt_tbl, i), chk))
        return false;
    u32 tree = tableAddress(this->pm_bgd.get()->m_tree_tbl, i);
    bool ret = false;
    if (at<cBgD_Tree_t>(tree)->mFlag & dBgW::LEAF) {
        const be<u16> cBgW_BlkElm::*kOrder[3] = {&cBgW_BlkElm::ground, &cBgW_BlkElm::roof,
                                                 &cBgW_BlkElm::wall};
        for (int k = 0; k < 3; k++) {
            u32 p = block(this->pm_blk.v, at<cBgD_Tree_t>(tree)->mChild[0])->*kOrder[k];
            if (p != dBgW::INVALID_INDEX && this->RwgSphChk(p, check, user))
                ret = true;
        }
        return ret;
    }
    for (int k = 0; k < 8; k++) {
        u32 child = at<cBgD_Tree_t>(tree)->mChild[k];
        if (child != dBgW::INVALID_INDEX && this->SphChkRp(check, user, child))
            ret = true;
    }
    return ret;
}

/* 024F42BC */
bool background_world::dBgW::SphChkGrpRp(SphereCheck *check, u32 user, s32 grp_id, s32 depth) {
    u32 chk = gabi::ea(check);
    if (dispatchGroup(this, grp_id, check->mCheck.mGrpPassChk, depth))
        return false;
    if (!cM3d_Cross_AabSph(tableAddress(this->pm_grp, grp_id), chk))
        return false;
    bool ret = false;
    u32 grpd = tableAddress(this->pm_bgd.get()->m_g_tbl, grp_id);
    u32 tree_idx = at<cBgD_Grp_t>(grpd)->m_tree_idx;
    if (tree_idx != dBgW::INVALID_INDEX && this->SphChkRp(check, user, tree_idx))
        ret = true;
    depth++;
    u32 child_idx = at<cBgD_Grp_t>(grpd)->m_first_child;
    while (child_idx != dBgW::INVALID_INDEX) {
        if (this->SphChkGrpRp(check, user, child_idx, depth))
            ret = true;
        child_idx = group(this->pm_bgd.get()->m_g_tbl.v, child_idx)->m_next_sibling;
    }
    return ret;
}

/* 024F43DC */
void background_world::dBgW::Move() {
    this->mFlag |= MOVE;
    cBgW_Move(this);
}

/* 024F43EC */
void background_world::dBgW::ChangeAttributeCodeByPathPntNo(u32 pnt_no, u32 attr) {
    if (this->pm_bgd.v == 0)
        return;
    for (s32 i = 0; i < (s32)this->pm_bgd.get()->m_ti_num; i++) {
        u32 ti_pnt_no = dBgS_GetRoomPathPntNo(polyInfo(this->pm_bgd.get()->m_ti_tbl.v, i)->m_info2);
        if (ti_pnt_no == pnt_no)
            dBgS_ChangeAttributeCode(attr, gabi::ea(&polyInfo(this->pm_bgd.get()->m_ti_tbl.v, i)->m_info1));
    }
}

/* 024F4484 */
void background_world::dBgW::CrrPos(cBgS_PolyInfo *poly, void *user, u32 accept, cXyz *pos, csXyz *angle,
                                    csXyz *shape_angle) {
    if (this->m_crr_func != 0)
        dispatchCorrection(this, user, poly, accept, pos, angle, shape_angle);
}

/* 024F44A4 */
void background_world::dBgW::TransPos(cBgS_PolyInfo *poly, void *user, u32 accept, cXyz *pos, csXyz *angle,
                                      csXyz *shape_angle) {
    if (this->m_crr_func != 0)
        dispatchCorrection(this, user, poly, accept, pos, angle, shape_angle);
}

/* cBgW::GetPolyInfId + GetPolyInf3 (inline, with the c_bg_w.h asserts) */
/* inlined into ChkPolyThrough */
static inline u32 GetPolyInf3(dBgW *w, s32 poly_index) {
    if (!(0 <= poly_index && poly_index < (s32)w->pm_bgd.get()->m_t_num))
        assertion(meshHeaderFile, 0x2F1, meshPolyIndexCheck);
    s32 id = triangle(w->pm_bgd.get()->m_t_tbl.v, poly_index)->id;
    if (!(id < (s32)w->pm_bgd.get()->m_ti_num))
        assertion(meshHeaderFile, 0x307, meshInfoIndexCheck);
    return polyInfo(w->pm_bgd.get()->m_ti_tbl.v, id)->m_info3;
}

/* 024F44C4 */
bool background_world::dBgW::ChkPolyThrough(s32 poly_index, dBgS_PolyPassChk *chk) {
    if (chk == 0)
        return false;
    /* dBgS_PolyPassChk flags +4..+0xA: obj, cam, link, arrow, bomb, boomerang, rope */
    const u32 kMask[7] = {OBJ_THROUGH,  CAM_THROUGH,       LINK_THROUGH, ARROW_THROUGH,
                          BOMB_THROUGH, BOOMERANG_THROUGH, ROPE_THROUGH};
    for (int k = 0; k < 7; k++)
        if (polyPassFlag(chk, k) != 0 && (GetPolyInf3(this, poly_index) & kMask[k]))
            return true;
    return false;
}

/* 024F48A4 */
bool background_world::dBgW::ChkGrpThrough(s32 grp_id, dBgS_GrpPassChk *chk, s32 depth) {
    if (depth != 2 || chk == 0)
        return false;
    u32 info = group(this->pm_bgd.get()->m_g_tbl.v, grp_id)->m_info;
    if (!(info & SPECIAL_GROUPS) && (chk->mGrp & NORMAL_GRP))
        return false; /* MaskNormalGrp */
    if ((info & WATER_GROUP) && (chk->mGrp & WATER_GRP))
        return false; /* MaskWaterGrp */
    if ((info & YOGAN_GROUP) && (chk->mGrp & YOGAN_GRP))
        return false; /* MaskYoganGrp */
    if ((info & DOKU_GROUP) && (chk->mGrp & DOKU_GRP))
        return false; /* MaskDokuGrp */
    if ((info & LIGHT_GROUP) && (chk->mGrp & LIGHT_GRP))
        return false; /* MaskLightGrp */
    return true;
}

// HD: compiler-generated header initialization and out-of-line inline virtual copy.
/* 024F4944 */
// HD: static initialisers (header statics), after the unit's functions
static void __sinit_d_bg_w_cpp() { sinit_header_statics(headerObject, headerDestructor); }

/* 024F49D8 */
// HD: dBgW::MatrixCrrPos (inline virtual, out-of-line copy of this unit): CrrPos through the vtable
void background_world::dBgW::MatrixCrrPos(cBgS_PolyInfo *poly, void *user, u32 accept, cXyz *pos,
                                          csXyz *angle, csXyz *shape_angle) {
    dispatchCrrPos(this, poly, user, accept, pos, angle, shape_angle);
}

} // namespace background_world
using namespace background_world;

/* WWHD glue (tools/verify/autoglue.py)
*/

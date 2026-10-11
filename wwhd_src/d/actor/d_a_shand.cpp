/**
 * d_a_shand.cpp (WWHD)
 * Object - Forbidden Woods - Ceiling tentacle / 汎用触手 (general purpose tentacle)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_shand.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_shand.h"

// HD: checked collision-source fields; flags and shape match tww.
static ShandCylinderSource tg_cyl_src = {0,    0, 0, 0,    0,    0,    0,     0xFF1DFEFF, 3,
                                         0x75, 0, 0, 0,    0,    0,    0,     0,          0,
                                         0,    0, 0, 0.0f, 0.0f, 0.0f, 15.0f, 200.0f};
// HD: checked collision-source fields; flags and shape match tww.
static ShandSphereSource bm_sph_src = {0, 0, 0, 0, 0, 0, 5, 0xFF1DFEFF, 3,    0x0,  0,    0,    0,
                                       0, 0, 0, 0, 0, 0, 0, 0,          0.0f, 0.0f, 0.0f, 80.0f};
static GXColor hand_color = {0x50, 0x96, 0x96, 0xFF};
static u8 hio_set = 0;

// HD: GHS copies of inline data for this translation unit.
const gc_tu_data gc_tu = {
    .safeString = 0x10039F9C,
    .headerStatics = {0x1046DC9C, 0x101D02F4},
};
/* this TU's line/ground check vtables */
static const dBgS_LinChk_vt l_linchk_vt = {0x1003A0A4, 0x1003A0B4, 0x1003A0D4, 0x1003A0C4};
static const dBgS_GndChk_vt l_gndchk_vt = {0x10039FE4, 0x10039FF4, 0x1003A014, 0x1003A004};
static const dBgS_GndChk_vt l_splchk_vt = {0x1003A064, 0x1003A074, 0x1003A094, 0x1003A084};

enum {
    dRes_INDEX_SHAND_BTI_SHAND_e = 3,
    dRes_INDEX_SHAND_BTI_VHLIF_VINE_e = 4,
};
enum { JA_SE_OBJ_VINE_S_RECOVER = 0x5868 }; /* HD id */
enum { ID_AK_JN_SIBOUBAKUEN = 0x13, ID_AK_JN_SIBOUFLASH = 0x16 };

// HD: daShand_HIO_c::daShand_HIO_c (HD: allocates when this == NULL)
/* 02471064 */
static daShand_HIO_c* daShand_HIO_c_ct(daShand_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daShand_HIO_c*)operator_new(0xC);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = daShand_HIO_c::__vt;
    i_this->mNo = -1;
    i_this->field_6 = -3;
    i_this->field_8 = 50;
    return i_this;
}

// HD: hand_draw inlined
/* 0246FAEC */
static BOOL daShand_Draw(shand_class* i_this) {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->eyePos, &i_this->tevStr);
    /* hand_draw: mLineMat.update(0x14, color, &tevStr); dComIfGd_set3DlineMat(&mLineMat) */
    i_this->mLineMat.update(0x14, &hand_color, &i_this->tevStr); /* mDoExt_3DlineMat1_c::update */
    auto* packets = reinterpret_cast<ShandRenderContext*>(dComIfGp_get())->mSortPackets;
    s32 matId = i_this->mLineMat.getMaterialID();
    packets[matId].setMat(&i_this->mLineMat); /* mDoExt_3DlineMatSortPacket::setMat */
    return TRUE;
}

/* one link of the chain: rotate towards (x, y, z) and place `seg` at prev + offset */
static inline void chain_link(shand_s* seg, shand_s* prev, f32 x, f32 y, f32 z, cXyz* offset,
                              cXyz* out, cXyz* sum) {
    s16 yAng = cM_atan2s(x, z);
    s16 xAng = -cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
    mDoMtx_YrotS(calc_mtx(), yAng);
    mDoMtx_XrotM(calc_mtx(), xAng);
    MtxPosition(offset, out);
    cXyz_pl(&prev->mPos, sum, out);
    seg->mPos.copy(*sum);
}

/* control1 (inlined). HD: the sway factor of the last links is computed with fnmsubs */
static inline void control1(shand_class* i_this) {
    i_this->field_31C[0].mPos.copy(i_this->current.pos);
    mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);

    gabi::Local<cXyz> local94, cStack_a0, localac, sum;
    local94->y = i_this->field_2F8;
    local94->x = 0.0f;
    local94->z = i_this->field_2FC;
    MtxPosition(local94, localac);
    cLib_addCalc2(&i_this->field_2F8, REG0_F(7), 1.0f, 0.1f);
    cLib_addCalc2(&i_this->field_2FC, REG0_F(8), 1.0f, 1.0f);
    local94->z = i_this->field_2F4;
    f32 fVar1 = i_this->field_300;
    shand_s* shand_i = &i_this->field_31C[1];
    for (s32 i = 1; i < 19; i++, shand_i++) {
        f32 idkx =
            cM_ssin(i_this->mExecuteCount * (REG0_S(5) + 1100) + i * (REG0_S(6) + 4000)) * fVar1;
        f32 idkz =
            cM_scos(i_this->mExecuteCount * (REG0_S(7) + 800) + i * (REG0_S(8) + 4000)) * fVar1;
        f32 fVar2 = (i < 15) ? 1.0f : gabi::fnmsubs((f32)(i - 15), 0.2f, 1.0f);

        f32 fVar_y = (shand_i->mPos.y - shand_i[-1].mPos.y) + localac->y;
        f32 fVar_x = gabi::fmadds(
            idkx, fVar2, gabi::fmadds(localac->x, fVar2, shand_i->mPos.x - shand_i[-1].mPos.x));
        f32 fVar_z = gabi::fmadds(
            idkz, fVar2, gabi::fmadds(localac->z, fVar2, shand_i->mPos.z - shand_i[-1].mPos.z));
        chain_link(shand_i, &shand_i[-1], fVar_x, fVar_y, fVar_z, local94, cStack_a0, sum);
    }
}

/* control2 (inlined) */
static inline void control2(shand_class* i_this) {
    gabi::Local<cXyz> rel_offset, abs_offset, sum;
    rel_offset->x = 0.0f;
    rel_offset->y = 0.0f;
    rel_offset->z = i_this->field_2F4;

    cLib_addCalc2(&i_this->field_31C[19].mPos.x, i_this->field_2D4.x, 1.0f,
                  i_this->field_2F0 * 50.0f);
    cLib_addCalc2(&i_this->field_31C[19].mPos.y, i_this->field_2D4.y, 1.0f,
                  i_this->field_2F0 * 50.0f);
    cLib_addCalc2(&i_this->field_31C[19].mPos.z, i_this->field_2D4.z, 1.0f,
                  i_this->field_2F0 * 50.0f);
    cLib_addCalc2(&i_this->field_2F0, 1.0f, 1.0f, 0.01f);

    shand_s* shand_i = &i_this->field_31C[18];
    for (s32 i = 18; i >= 1; i--, shand_i--) {
        f32 dx = shand_i->mPos.x - shand_i[1].mPos.x;
        f32 dz = shand_i->mPos.z - shand_i[1].mPos.z;
        f32 dy = shand_i->mPos.y - shand_i[1].mPos.y;
        chain_link(shand_i, &shand_i[1], dx, dy, dz, rel_offset, abs_offset, sum);
    }
}

/* control3 / cut_control3 (inlined): the thickness of each link */
static inline void set_thickness(shand_class* i_this, f32 step) {
    shand_s* shand_i = i_this->field_31C;
    for (s32 counter = 0; counter < 20; counter++, shand_i++) {
        if (counter < 12) {
            shand_i->field_18 = i_this->field_304;
        } else {
            shand_i->field_18 = i_this->field_304 * gabi::fnmsubs((f32)(counter - 10), step, 1.0f);
        }
    }
}

/* normal (inlined) */
static inline void normal(shand_class* i_this) {
    fopAc_ac_c* actor = i_this;

    if (i_this->field_318 != 0) {
        gabi::Local<dBgS_LinChk> local94;
        dBgS_LinChk_ct(local94.get(), l_linchk_vt, false);
        gabi::Local<cXyz> chk_start, chk_end;
        f32 sy = actor->current.pos.y;
        chk_start->x = actor->current.pos.x;
        chk_start->y = sy;
        chk_start->z = actor->current.pos.z;
        chk_end->x = actor->current.pos.x;
        f32 ey = actor->current.pos.y;
        chk_end->y = ey;
        chk_end->z = actor->current.pos.z;
        chk_start->y = sy + 50.0f;
        chk_end->y = ey + 4000.0f;
        dBgS_LinChk_Set(local94.get(), chk_start, chk_end, actor);
        if (cBgS_LineCross(dComIfG_Bgsp(), local94.get())) {
            cXyz* cross = &local94->mLin.mEnd;
            f32 x = cross->x, y = cross->y, z = cross->z;
            i_this->field_2C8.x = x;
            i_this->field_2D4.x = x;
            i_this->field_2C8.y = y;
            i_this->field_2C8.z = z;
            i_this->field_2D4.y = y;
            i_this->field_2D4.z = z;
            i_this->field_31C[19].mPos.x = x;
            i_this->field_31C[19].mPos.y = y;
            i_this->field_31C[19].mPos.z = z;
        }
        i_this->field_318 -= 1;
        /* ~dBgS_LinChk (inline): this TU's vtables, then cBgS_LinChk::~cBgS_LinChk */
        local94->dBgS_GrpPassChk::__vtbl = shandGroundGroupVtable;
        local94->dBgS_PolyPassChk::__vtbl = l_linchk_vt.v58;
        local94->cBgS_PolyInfo::__vtbl = shandLinePolyVtable;
        shandLineCheckDestroyBase(local94.get(), 0);
    }

    switch ((u16)i_this->field_2BA) {
    case 0:
        if (std::fabs(i_this->field_31C[19].mPos.y - i_this->field_2D4.y) < 10.0f) {
            i_this->field_2BA = 1;
            *i_this->field_314 = 2;
        }
    case 1:
        i_this->field_2D4.copy(i_this->field_2C8);
        break;
    }

    f32 y_diff = std::fabs(actor->home.pos.y - i_this->field_2C8.y);
    cLib_addCalc2(&i_this->field_2F4, y_diff * (REG_F(14, 11) + 0.05f), 0.1f, 1.0f);
    cLib_addCalc2(&i_this->field_300, REG_F(14, 12) + 10.0f, 0.1f, 0.5f);
    control1(i_this);
    control2(i_this);
    set_thickness(i_this, 0.05f);
}

/* cut_control (inlined) */
static inline void cut_control(shand_class* i_this) {
    i_this->field_31C[0].mPos.copy(i_this->current.pos);
    mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);

    gabi::Local<cXyz> local_b8, cStack_c4, local_d0, sum;
    local_b8->x = 0.0f;
    local_b8->y = i_this->field_2F8;
    local_b8->z = i_this->field_2FC;
    MtxPosition(local_b8, local_d0);
    cLib_addCalc2(&i_this->field_2F8, REG_F(14, 7) + -5.0f, 1.0f, REG_F(12, 4) + 0.1f);
    cLib_addCalc2(&i_this->field_2FC, REG_F(14, 8) + 20.0f, 1.0f, REG_F(14, 5) + 0.2f);
    cLib_addCalc0(&i_this->field_300, 1.0f, REG_F(12, 6) + 1.0f);
    local_b8->z = i_this->field_2F4;

    shand_s* shand_i = &i_this->field_31C[1];
    for (s32 i = 1; i < 20; i++, shand_i++) {
        f32 f300 = i_this->field_300;
        s32 cnt = i_this->mExecuteCount;
        f32 e8x = cM_ssin(cnt * (REG0_S(4) + 3500) + i * (REG0_S(5) + 4000));
        f32 e8y = cM_scos(cnt * (REG0_S(6) + 4000) + i * (REG0_S(7) + 4000));
        f32 e8z = cM_scos(cnt * (REG0_S(8) + 3800) + i * (REG0_S(9) + 4000)) * f300;
        f32 factor = gabi::fnmsubs((f32)i, REG0_F(9) + 0.03763158f, 1.0f);

        f32 fVar_y = gabi::fmadds(e8y, f300, shand_i->mPos.y + local_d0->y);
        f32 fVar_x = gabi::fmadds(
            e8x, f300, gabi::fmadds(local_d0->x, factor, shand_i->mPos.x - shand_i[-1].mPos.x));
        if (fVar_y < i_this->ground_y)
            fVar_y = i_this->ground_y;
        f32 fVar_z = ((shand_i->mPos.z - shand_i[-1].mPos.z) + local_d0->z * factor) + e8z;
        f32 delta_y = fVar_y - shand_i[-1].mPos.y;
        chain_link(shand_i, &shand_i[-1], fVar_x, delta_y, fVar_z, local_b8, cStack_c4, sum);
    }
}

/* cut (inlined) */
static inline void cut(shand_class* i_this) {
    cLib_addCalc2(&i_this->field_2F4, REG_F(8, 12) + 20.0f, 0.1f, 0.5f);
    cut_control(i_this);
    set_thickness(i_this, 0.08f);
    if (i_this->field_2BC[0] == 0 && i_this->field_30C->health != 0) {
        i_this->field_2F0 = 0.0f;
        i_this->mState = 0;
        i_this->field_2BA = 0;
        i_this->field_300 = 0.0f;
        fopAcM_seStart(i_this, JA_SE_OBJ_VINE_S_RECOVER, 0);
    }
}

/* ground checks of the cut state (inlined) */
static inline void cut_ground(shand_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if ((fopAcM_GetParam(actor) & 0xFF) != 1) {
        gabi::Local<dBgS_GndChk> local_ac;
        dBgS_GndChk_ct(local_ac.get(), l_gndchk_vt, false);
        f32 z = actor->current.pos.z;
        f32 y = actor->current.pos.y;
        f32 x = actor->current.pos.x;
        local_ac->m_pos.z = z;
        local_ac->m_pos.x = x;
        local_ac->m_pos.y = y - 100.0f;
        i_this->ground_y = cBgS_GroundCross(dComIfG_Bgsp(), local_ac.get());

        gabi::Local<dBgS_GndChk> local_100; /* dBgS_ObjGndChk_Spl */
        dBgS_GndChk_ct(local_100.get(), l_splchk_vt, true);
        local_100->mGrp = 0xE; /* Spl: the special-material groups */
        z = actor->current.pos.z;
        y = actor->current.pos.y;
        x = actor->current.pos.x;
        local_100->m_pos.z = z;
        local_100->m_pos.x = x;
        local_100->m_pos.y = y + 200.0f;
        f32 spl_ground_y = cBgS_GroundCross(dComIfG_Bgsp(), local_100.get()) + 10.0f;
        if (spl_ground_y != -1000000000.0f) {
            i_this->ground_y = spl_ground_y;
        }
        /* ~dBgS_ObjGndChk_Spl, ~dBgS_GndChk (inline): this TU's vtables, then cBgS_Chk::~cBgS_Chk
         */
        local_100->cBgS_PolyInfo::__vtbl = l_gndchk_vt.v20;
        local_100->dBgS_PolyPassChk::__vtbl = l_gndchk_vt.v40;
        local_100->dBgS_GrpPassChk::__vtbl = shandGroundGroupVtable;
        cBgS_GndChk_dt(local_100.get(), 0);
        local_ac->dBgS_PolyPassChk::__vtbl = l_gndchk_vt.v40;
        local_ac->dBgS_GrpPassChk::__vtbl = shandGroundGroupVtable;
        local_ac->cBgS_PolyInfo::__vtbl = l_gndchk_vt.v20;
        cBgS_GndChk_dt(local_ac.get(), 0);
    } else {
        i_this->ground_y = -1000000000.0f;
    }
    i_this->mState = 2;
}

/* hand_move (inlined) */
static inline void hand_move(shand_class* i_this) {
    fopAc_ac_c* actor = i_this;

    i_this->field_30C = fopAcM_SearchByID(i_this->field_308);
    if (i_this->field_30C != NULL) {
        cXyz* src = i_this->field_310;
        actor->current.pos.copy(*src);
        actor->current.angle.y =
            actor->home.angle.y + i_this->field_30C->shape_angle.y + REG_S(14, 3);
        switch ((u16)i_this->mState) {
        case 0:
            normal(i_this);
            actor->attention_info.flags =
                fopAc_Attn_LOCKON_MISC_e; /* attention_info.flags = fopAc_Attn_LOCKON_MISC_e */
            if (i_this->field_30C->health == 0) {
                i_this->mState = 1;
                i_this->field_2BA = 0;
            }
            break;
        case 1:
            cut_ground(i_this);
        case 2:
            cut(i_this);
            actor->attention_info.flags = 0; /* attention_info.flags */
            break;
        }
    }

    {
        ShandLineData* lines = i_this->mLineMat.getLines();
        cXyz* line_data = lines->mpPos;
        be<u8>* line_size = lines->mpSize;
        shand_s* shand_i = i_this->field_31C;
        for (s32 i = 20; i != 0; i--) {
            line_data->copy(shand_i->mPos);
            *line_size = (u8)gabi::ftoi(shand_i->field_18);
            shand_i++;
            line_data++;
            line_size += 1;
        }
    }

    s16 seg = l_HIO.field_6;
    cXyz* line_segments = i_this->mLineMat.getPos(0);
    cXyz* eye = &line_segments[seg + 10];
    shandSetEyePosition(actor, eye);

    bool is_hit = false;
    gabi::Local<CcAtInfo> hit_atInfo;
    hit_atInfo->pParticlePos = nullptr;
    gabi::Local<cXyz> center;
    if (i_this->field_2BC[1] == 0 && i_this->mState == 0) {
        i_this->mSph.SetC(&actor->eyePos);
        i_this->mCylArr[0].SetC(&actor->current.pos);
        for (s32 i = 0; i < 5; i++) {
            if (i_this->field_2C4 == 0 && i_this->mCylArr[i].ChkTgHit() != 0) {
                hit_atInfo->mpObj = static_cast<dCcD_GObjInf*>(i_this->mCylArr[i].GetTgHitObj());
                hit_atInfo->pParticlePos = i_this->mCylArr[i].GetTgHitPosP();
                is_hit = true;
                break;
            }
            if (i > 0) {
                s32 seg_idx = ((i_this->mExecuteCount & 3) + (i * 4)) % 20;
                cXyz* p = &line_segments[seg_idx];
                center->x = p->x;
                f32 cy = p->y;
                center->y = cy;
                center->z = p->z;
                center->y = cy - 200.0f;
                i_this->mCylArr[i].SetC(center);
            }
        }
    } else {
        center->x = 0.0f;
        center->z = 0.0f;
        center->y = -20000.0f;
        i_this->mSph.SetC(center);
        for (s32 i = 0; i < 5; i++) {
            i_this->mCylArr[i].SetC(center);
        }
    }

    dComIfG_Ccsp_Set(&i_this->mSph);
    for (s32 i = 0; i < 5; i++) {
        dComIfG_Ccsp_Set(&i_this->mCylArr[i]);
    }

    if (is_hit || (i_this->field_2C4 == 0 && i_this->mSph.ChkTgHit() != 0)) {
        if (!is_hit) {
            hit_atInfo->mpObj = static_cast<dCcD_GObjInf*>(i_this->mSph.GetTgHitObj());
            hit_atInfo->pParticlePos = i_this->mSph.GetTgHitPosP();
        }
        fopAc_ac_c* hitActor = at_power_check(hit_atInfo.get());
        hit_atInfo->mpActor = hitActor;
        i_this->field_2C4 = 10;
        if (hitActor != 0) {
            def_se_set(actor, hit_atInfo->mpObj, 33);
            if (i_this->field_30C != NULL) {
                i_this->mState = 1;
                i_this->field_2BA = 0;
                i_this->field_2BC[0] = l_HIO.field_8;
                i_this->field_2BC[1] = l_HIO.field_8 + 90 + REG0_S(2);
                i_this->field_2F8 = 3.0f;
                i_this->field_2FC = 40.0f;
                i_this->field_300 = cM_rndF(20.0f) + 30.0f;
                *i_this->field_314 = 1;
                gabi::Local<cXyz> particle_scale;
                cXyz* eyeA = &hit_atInfo->mpActor->eyePos;
                particle_scale->y = 0.5f;
                particle_scale->z = 0.5f;
                particle_scale->x = 0.5f;
                dComIfGp_particle_set(ID_AK_JN_SIBOUBAKUEN, eyeA, NULL, particle_scale);
                cXyz* eyeB = &hit_atInfo->mpActor->eyePos;
                dComIfGp_particle_set(ID_AK_JN_SIBOUFLASH, eyeB, NULL, particle_scale);
            }
        }
    }
}

/* 0246FB78 */
static BOOL daShand_Execute(shand_class* i_this) {
    dComIfGp_get(); /* unused accessor call */
    i_this->mExecuteCount += 1;

    for (s32 i = 4, j = 0; i != 0; i--, j++) {
        if (i_this->field_2BC[j] != 0)
            i_this->field_2BC[j] -= 1;
    }

    if (i_this->field_2C4 != 0)
        i_this->field_2C4 -= 1;

    hand_move(i_this);
    return TRUE;
}

/* 02470D34 */
static BOOL daShand_IsDelete(shand_class*) { return TRUE; }

/* 02470D3C */
static BOOL daShand_Delete(shand_class* i_this) {
    dComIfG_resDelete(&i_this->mPhs, "Shand" /* "Shand" */);
    if (i_this->mHasHIO) {
        s8 no = l_HIO.mNo;
        hio_set = 0;
        mDoHIO_deleteChild(no);
    }
    return TRUE;
}

// HD: also the solid heap callback: daShand_solidHeapCB folded into it
/* 02470DA0 */
static BOOL useHeapInit(shand_class* i_this) {
    s32 bti_idx;
    if ((fopAcM_GetParam(i_this) & 0xff) == 53) {
        bti_idx = dRes_INDEX_SHAND_BTI_VHLIF_VINE_e;
    } else {
        bti_idx = dRes_INDEX_SHAND_BTI_SHAND_e;
    }
    void* img = dComIfG_getObjectRes("Shand" /* "Shand" */, bti_idx, gc_tu.safeString);
    /* mDoExt_3DlineMat1_c::init(1 line, 20 segments, img, TRUE) */
    if (i_this->mLineMat.init(1, 0x14, static_cast<ResTIMG*>(img), TRUE) == FALSE) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/* 02470E24 */
static cPhs_State daShand_Create(fopAc_ac_c* i_this) {
    shand_class* s_this = static_cast<shand_class*>(i_this);
    /* fopAcM_ct(i_this, shand_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = shand_class::__vt;
            shandLineConstruct(&s_this->mLineMat); /* mDoExt_3DlineMat1_c::mDoExt_3DlineMat1_c */
            dCcD_Stts_ct(&s_this->mStts);
            shandSphereConstruct(&s_this->mSph); /* dCcD_Sph::dCcD_Sph */
            shandConstructArray(s_this->mCylArr, 5, 0x130,
                                gabi::guest_fn((const void*)dCcD_Cyl_ct_tu));
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&s_this->mPhs, "Shand" /* "Shand" */);
    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, gabi::guest_fn((const void*)useHeapInit), 0x1040) !=
            false) {
            i_this->health = 2;
            s_this->mExecuteCount = (s16)gabi::ftoi(cM_rndF(10000.0f));
            s_this->field_318 = 10;
            s_this->field_2BA = 1;
            s_this->field_2F0 = 1.0f;
            if ((fopAcM_GetParam(i_this) & 0xff) == 1)
                s_this->field_304 = 15.75f;
            else
                s_this->field_304 = 10.5f;
            s_this->mStts.Init(0xff, 0xff, i_this);
            for (s32 i = 0; i < 5; i++) {
                s_this->mCylArr[i].Set(reinterpret_cast<dCcD_SrcCyl*>(&tg_cyl_src));
                s_this->mCylArr[i].SetStts(&s_this->mStts);
            }
            s_this->mSph.Set(reinterpret_cast<dCcD_SrcSph*>(&bm_sph_src));
            s_this->mSph.SetStts(&s_this->mStts);
            s_this->field_2C4 = 30;
            for (s32 i = 0; i < 3; i++) {
                daShand_Execute(s_this);
            }
        } else {
            ret = cPhs_ERROR_e;
        }

        if (hio_set == 0) {
            hio_set = 1;
            s_this->mHasHIO = 1;
            l_HIO.mNo = mDoHIO_createChild("汎用触手" /* HD: the guest label uses Shift-JIS; glue
                                                         retains its original address. */
                                           ,
                                           &l_HIO);
        }
    }
    return ret;
}

// HD: static initialisation (header statics, l_HIO)
// HD: compiler-generated and template code of this translation unit.
/* 024710BC */
static void __sinit_d_a_shand_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
    daShand_HIO_c_ct(&l_HIO);
}

// HD: sead::SafeString deleting destructor (this TU's vtable 0x10039F9C)
/* 0247115C */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

// HD: dCcD_Cyl::dCcD_Cyl (this TU's out-of-line copy for __construct_array; HD: allocates when this
// == NULL)
/* 02471170 */
static dCcD_Cyl* dCcD_Cyl_ct_tu(dCcD_Cyl* c) {
    if (c == nullptr) {
        c = (dCcD_Cyl*)operator_new(0x130);
        if (c == nullptr)
            return c;
    }
    dCcD_Cyl_ct(c, shandCylinderAabVtable);
    return c;
}

// HD: shand_class deleting destructor (compiler-generated, HD virtual destructor)
/* 024711FC */
static void shand_class_dt(shand_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        shandDestroyArray(i_this->mCylArr, 5, 0x130, shandCylinderDestructor, 0);
        shandSphereDestroy(&i_this->mSph); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mStts, 2);
        shandLineDestroy(&i_this->mLineMat);    /* ~mDoExt_3DlineMat1_c (matcher: draw) */
        __dt(static_cast<fopAc_ac_c*>(i_this)); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
/* 02471294 */
static void SafeString_assureTerminationImpl(void*) {}

// clang-format off
/* WWHD glue
data tg_cyl_src 101D02B0
data bm_sph_src 101D0270
data hand_color 101D0248
data hio_set 101D024C
str 1003A160 "Shand" in 02470D3C
str 10039F94 "Shand" in 02470DA0
str 1003A174 "Shand" in 02470E24
str 1003A17C "\u6c4e\u7528\u89e6\u624b" renamed in 02470E24
*/
// clang-format on

/**
 * d_a_mt_body.cpp (WWHD)
 * Enemy - Magtail: body control (segment chain) and the rolled-up movement.
 *
 * The GameCube decompilation has only "Nonmatching"
 * placeholders for these functions: written from the WWHD code (cking.rpx) and verified against it.
 */
#include "d/actor/d_a_mt.h"
#include <cmath>

/* JMath::TSinCosTable (HD): the sin/cos table at 0x104A44F8, entries {sin, cos}, indexed by the
 * angle >> 3 (sinShort/cosShort inline) */
static inline f32 jm_sinShort(s32 a) { return gabi::load<f32>(0x104A44F8 + (((u32)a & 0xFFFF) >> 3) * 8); }
static inline f32 jm_cosShort(s32 a) { return gabi::load<f32>(0x104A44F8 + (((u32)a & 0xFFFF) >> 3) * 8 + 4); }
static inline Mtx34* mtx_now() { return mDoMtx_stack_c::get(); }
static inline void PSMTXScale(Mtx34* m, f32 x, f32 y, f32 z) { gabi::call(0x028E945C, m, x, y, z); }

/* 021DE888: death: the segments fly apart (scatter speeds from bakuha), then burst into smoke */
void body_control4(mt_class* i_this) {
    WWHD_FUNC(0x021DE888, void, i_this);
    J3DModel* model = i_this->mpMorf[0]->getModel();
    PSMTXScale(mtx_now(), 0.0f, 0.0f, 0.0f);
    J3DModel_setBaseTRMtx(model, mtx_now());
    for (int i = 1; i < MT_PART_NUM; i++) {
        cXyz* pos = &i_this->m5BC[i];
        csXyz* ang = &i_this->m67C[i];
        cXyz* spd = &i_this->m6AC[i];
        if (i_this->m70C[i] != 0) {
            i_this->m70C[i] = i_this->m70C[i] - 1;
        } else {
            dComIfGp_particle_setSimple(0x8060, pos);
            pos->x = pos->x + spd->x;
            pos->y = pos->y + spd->y;
            pos->z = pos->z + spd->z;
            spd->y = spd->y - 2.5f;
            ang->x = ang->x + 0x1800;
            ang->y = ang->y + 0x1000;
            if (spd->y < 0.0f) {
                cLib_addCalc0(&i_this->m71C[i], 1.0f, 0.025f);
            }
        }
        model = i_this->mpMorf[i]->getModel();
        mDoMtx_stack_c::transS(pos->x, pos->y, pos->z);
        mDoMtx_stack_c::YrotM(ang->y);
        mDoMtx_XrotM(mtx_now(), ang->x);
        mDoMtx_ZrotM(mtx_now(), ang->z);
        mDoMtx_stack_scaleM(i_this->m71C[i], i_this->m71C[i], i_this->m71C[i]);
        if (i == 7) {
            mDoMtx_stack_scaleM(0.0f, 0.0f, 0.0f);
        }
        J3DModel_setBaseTRMtx(model, mtx_now());
    }
}
VERIFY(0x021DE888, body_control4);

/* 021DEBC8: the head leads, every segment follows the previous one at a fixed length, with a
 * sideways/vertical wobble (m586 phase, m590 amplitude); colliders follow the segments */
void body_control5(mt_class* i_this) {
    WWHD_FUNC(0x021DEBC8, void, i_this);
    f32 groundY = i_this->mAcch.m_ground_h + l_HIO().m18;
    i_this->m5BC[0].copy(i_this->current.pos);
    i_this->m67C[0].x = i_this->shape_angle.x;
    i_this->m67C[0].y = i_this->shape_angle.y;
    i_this->m67C[0].z = i_this->shape_angle.z;
    f32 rate = 0.2f;
    gabi::Local<cXyz> vec;
    gabi::Local<cXyz> out2;
    gabi::Local<cXyz> out;
    for (int i = 0; i < MT_PART_NUM; i++) {
        cXyz* pos = &i_this->m5BC[i];
        csXyz* ang = &i_this->m67C[i];
        if (i > 0) {
            s32 a1 = i_this->m586 * (REG0_S(5) + 0x1194) - i * (REG0_S(6) + 0x1B58);
            f32 r1 = REG0_F(4) + 50.0f;
            s32 a2 = i_this->m586 * (REG0_S(7) + 0xDAC) - i * (REG0_S(8) + 0x1770);
            f32 r2 = REG0_F(5) + 80.0f;
            f32 amp = i_this->m590;
            vec->x = jm_sinShort(a1) * r1 * amp * rate;
            vec->y = jm_sinShort(a2) * r2 * amp * rate;
            vec->z = REG0_F(3) + -30.0f;
            rate += 0.2f;
            mDoMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
            MtxPosition(vec, out);
            cXyz* prev = &i_this->m5BC[i - 1];
            f32 y = pos->y - 10.0f + out->y;
            if (y < groundY) {
                y = groundY;
            }
            f32 dx = pos->x - prev->x + out->x;
            f32 dz = pos->z - prev->z + out->z;
            f32 dy = y - prev->y;
            s16 yaw = cM_atan2s(dx, dz);
            f32 dist = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
            s16 pitch = -cM_atan2s(dy, dist);
            vec->y = 0.0f;
            vec->x = 0.0f;
            vec->z = REG0_F(7) + 35.0f;
            mDoMtx_YrotS(calc_mtx(), yaw);
            mDoMtx_XrotM(calc_mtx(), pitch);
            MtxPosition(vec, out2);
            ang->y = yaw + 0x8000;
            ang->x = -pitch;
            pos->x = prev->x + out2->x;
            pos->y = prev->y + out2->y;
            pos->z = prev->z + out2->z;
        }
        J3DModel* model = i_this->mpMorf[i]->getModel();
        f32 sz = i_this->scale.z, sx = i_this->scale.x, sy = i_this->scale.y;
        gabi::store<f32>(gabi::ea(model) + 0xBC, sx);
        gabi::store<f32>(gabi::ea(model) + 0xC0, sy);
        gabi::store<f32>(gabi::ea(model) + 0xC4, sz);
        mDoMtx_stack_c::transS(pos->x, pos->y, pos->z);
        mDoMtx_stack_c::YrotM(ang->y);
        mDoMtx_XrotM(mtx_now(), ang->x);
        mDoMtx_ZrotM(mtx_now(), ang->z);
        if (i == 0) {
            mDoMtx_stack_c::YrotM(i_this->m584);
            mDoMtx_stack_scaleM(l_HIO().m1C, l_HIO().m1C, l_HIO().m1C);
        } else {
            f32 s = i_this->m71C[i];
            mDoMtx_stack_scaleM(s, s * i_this->m73C[i], 1.0f);
            if (i == 7) {
                mDoMtx_stack_scaleM(i_this->mHeadScale, i_this->mHeadScale, i_this->mHeadScale);
            }
        }
        mDoMtx_stack_transM(0.0f, 0.0f, i_this->m58C);
        J3DModel_setBaseTRMtx(model, mtx_now());
        dCcD_Sph* sph = &i_this->mSph[i];
        if (i == 0) {
            vec->x = 0.0f;
            vec->y = 0.0f;
            vec->z = REG0_F(9) + 30.0f;
            PSMTXMultVec(mtx_now(), vec, &i_this->eyePos);
            i_this->mAtSph.SetC(&i_this->eyePos);
            i_this->mAtSph.SetR(30.0f);
            dComIfG_Ccsp_Set(&i_this->mAtSph);
            sph->OffAtSPrmBit(1);
            sph->OffCoSPrmBit(1);
            dComIfG_Ccsp_Set(sph);
        } else {
            sph->OffCoSPrmBit(1);
            sph->SetC(pos);
            sph->OffAtSPrmBit(1);
            sph->OffCoSPrmBit(1);
            dComIfG_Ccsp_Set(sph);
        }
    }
    cLib_addCalc2(&i_this->m58C, 20.0f, 1.0f, 1.0f);
    s32 a = i_this->m586 * (REG0_S(0) + 0xBB8);
    i_this->m584 = (s16)gabi::ftoi(jm_sinShort(a) * (REG0_F(7) + 3000.0f) * i_this->m590);
}
VERIFY(0x021DEBC8, body_control5);

/* 021DE1E0: crawling: each segment follows the previous one with a sideways wave (m582 phase,
 * m44C amplitude); the head collider grows with speed */
void body_control3(mt_class* i_this) {
    WWHD_FUNC(0x021DE1E0, void, i_this);
    i_this->m5BC[0].copy(i_this->current.pos);
    i_this->m67C[0].z = i_this->shape_angle.z;
    i_this->m67C[0].x = i_this->shape_angle.x;
    i_this->m67C[0].y = i_this->shape_angle.y;
    mDoMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
    mDoMtx_XrotM(calc_mtx(), i_this->shape_angle.x);
    mDoMtx_XrotM(calc_mtx(), i_this->shape_angle.z); /* sic: X twice */
    gabi::Local<cXyz> vec;
    gabi::Local<cXyz> out2;
    gabi::Local<cXyz> out;
    gabi::Local<cXyz> offset;
    f32 amp = i_this->m590;
    vec->x = 0.0f;
    vec->y = 32.0f * amp;
    vec->z = -17.0f * amp;
    MtxPosition(vec, offset);
    s16 pitchAdd = 0;
    if (i_this->m1A18 != 0) {
        pitchAdd = (s16)((i_this->m1A18 & 2) * 500);
    }
    s16 pitchAcc = 0;
    for (int i = 0; i < MT_PART_NUM; i++) {
        cXyz* pos = &i_this->m5BC[i];
        csXyz* ang = &i_this->m67C[i];
        if (i > 0) {
            s32 a = i_this->m582 * (REG0_S(5) + 0x1388) + i * (REG0_S(6) + 0x1B58);
            pitchAcc = (s16)(pitchAcc - (i_this->m5A6 + pitchAdd));
            vec->y = 0.0f;
            vec->z = -i_this->m594;
            vec->x = jm_sinShort(a) * i_this->m44C;
            mDoMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
            mDoMtx_XrotM(calc_mtx(), (s16)(i_this->shape_angle.x + pitchAcc));
            mDoMtx_ZrotM(calc_mtx(), i_this->shape_angle.z);
            MtxPosition(vec, out);
            cXyz* prev = &i_this->m5BC[i - 1];
            f32 ddy = pos->y - prev->y;
            f32 ddx = pos->x - prev->x;
            f32 ddz = pos->z - prev->z;
            f32 dx = ddx + out->x;
            f32 dz = ddz + out->z;
            f32 dy = ddy + out->y;
            s16 yaw = cM_atan2s(dx, dz);
            f32 dist = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
            s16 pitch = -cM_atan2s(dy, dist);
            vec->x = 0.0f;
            vec->y = 0.0f;
            vec->z = REG0_F(7) + 35.0f;
            mDoMtx_YrotS(calc_mtx(), yaw);
            mDoMtx_XrotM(calc_mtx(), pitch);
            MtxPosition(vec, out2);
            s16 d = (s16)(yaw - i_this->shape_angle.y);
            if (d < 0) d = -d;
            if ((u16)d < 0x4000) {
                ang->y = yaw;
                ang->x = (s16)(pitch - 0x8000);
            } else {
                ang->y = (s16)(yaw + 0x8000);
                ang->x = -pitch;
            }
            i_this->m61C[i].copy(*pos);
            pos->x = prev->x + out2->x;
            pos->y = prev->y + out2->y;
            pos->z = prev->z + out2->z;
        }
        J3DModel* model = i_this->mpMorf[i]->getModel();
        f32 sx = i_this->scale.x, sz = i_this->scale.z, sy = i_this->scale.y;
        gabi::store<f32>(gabi::ea(model) + 0xBC, sx);
        gabi::store<f32>(gabi::ea(model) + 0xC0, sy);
        gabi::store<f32>(gabi::ea(model) + 0xC4, sz);
        mDoMtx_stack_c::transS(pos->x + offset->x, pos->y + offset->y, pos->z + offset->z);
        mDoMtx_stack_c::YrotM(ang->y);
        mDoMtx_XrotM(mtx_now(), ang->x);
        mDoMtx_ZrotM(mtx_now(), ang->z);
        if (i == 0) {
            mDoMtx_stack_scaleM(l_HIO().m20, l_HIO().m20, l_HIO().m20);
        } else {
            f32 s = i_this->m71C[i];
            mDoMtx_stack_scaleM(s, s * i_this->m73C[i], 1.0f);
            if (i == 7) {
                mDoMtx_stack_scaleM(i_this->mHeadScale, i_this->mHeadScale, i_this->mHeadScale);
            }
        }
        mDoMtx_stack_transM(0.0f, 0.0f, i_this->m58C);
        J3DModel_setBaseTRMtx(model, mtx_now());
        if (i == 0) {
            i_this->eyePos.copy(i_this->current.pos);
            dCcD_Sph* sph = &i_this->mSph[0];
            sph->SetC(&i_this->current.pos);
            sph->OnAtSPrmBit(1);
            i_this->mAtSph.SetR(-30.0f);
            if (std::fabs((f32)i_this->speedF) > 2.0f) {
                sph->OnAtSPrmBit(0xA);
            } else {
                sph->OffAtSPrmBit(0xA);
            }
            if (i_this->m582 >= 0x5A) {
                sph->OffAtSPrmBit(4);
                sph->SetR(40.0f);
            } else {
                sph->OnAtSPrmBit(4);
                sph->SetR(60.0f);
            }
            dComIfG_Ccsp_Set(sph);
        }
    }
    cLib_addCalc0(&i_this->m1A10, 1.0f, 0.01f);
    cLib_addCalc2(&i_this->m58C, 20.0f, 1.0f, 1.0f);
    cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 2, 0x400);
}
VERIFY(0x021DE1E0, body_control3);

/* body_control2: trail start indices (.data, per segment), two tables */
static inline u32 mt_trail_idx(u8 sel, u32 i) { return gabi::load<u32>((sel != 0 ? 0x101BAE34 : 0x101BAE14) + 4 * i); }

/* 021DD634: on the ground: each segment follows the previous one, kept on the floor by two line
 * checks to its sides, with a sideways wave; segment speeds (m6AC) are the moves scaled by m1A10 */
void body_control2(mt_class* i_this) {
    WWHD_FUNC(0x021DD634, void, i_this);
    i_this->m5BC[0].copy(i_this->current.pos);
    i_this->m67C[0].x = i_this->shape_angle.x;
    i_this->m67C[0].y = i_this->shape_angle.y;
    i_this->m67C[0].z = i_this->shape_angle.z;
    f32 spdScale = i_this->m1A10;
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk, MT_LINCHK_VTBLS);
    gabi::Local<cXyz> wave;
    wave->x = 0.0f;
    wave->y = 0.0f;
    wave->z = 0.0f;
    gabi::Local<cXyz> vec;
    gabi::Local<cXyz> start;
    gabi::Local<cXyz> side1;
    gabi::Local<cXyz> side2;
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> out;
    for (int i = 0; i < MT_PART_NUM; i++) {
        cXyz* pos = &i_this->m5BC[i];
        csXyz* ang = &i_this->m67C[i];
        cXyz* spd = &i_this->m6AC[i];
        if (i > 0) {
            start->x = pos->x;
            start->y = pos->y + 50.0f;
            start->z = pos->z;
            u8 hits = 0;
            mDoMtx_YrotS(calc_mtx(), ang->y);
            vec->x = 3.0f;
            vec->y = -200.0f;
            vec->z = 0.0f;
            MtxPosition(vec, side1);
            PSVECAdd(side1, pos, side1);
            dBgS_LinChk_Set(linChk, start, side1, i_this);
            if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                side1->copy(*gabi::at<cXyz>(gabi::ea(linChk.get()) + 0x30)); /* GetCross() */
                hits = 1;
            }
            vec->x = -vec->x;
            MtxPosition(vec, side2);
            PSVECAdd(side2, pos, side2);
            dBgS_LinChk_Set(linChk, start, side2, i_this);
            if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                side2->copy(*gabi::at<cXyz>(gabi::ea(linChk.get()) + 0x30));
                hits = hits + 1;
            }
            f32 y = pos->y - 10.0f;
            s16 roll = 0;
            if (hits == 2) {
                f32 floorY = side1->y + l_HIO().m18;
                if (y < floorY) {
                    y = floorY;
                    cXyz_mi(side1, diff, side2);
                    f32 dx = diff->x, dy = diff->y, dz = diff->z;
                    vec->x = dx;
                    vec->y = dy;
                    vec->z = dz;
                    roll = cM_atan2s(dy, std_sqrtf(gabi::fmadds(dx, dx, dz * dz)));
                }
            }
            cLib_addCalcAngleS2(&ang->z, roll, 2, 0x400);
            cXyz* prev = &i_this->m5BC[i - 1];
            f32 dy = y - prev->y + spd->y;
            if (i_this->m5AA == 0) {
                s32 a = i_this->m586 * (REG0_S(5) + 0x5DC) + i * (REG0_S(6) + 0x1D4C);
                vec->y = 0.0f;
                vec->z = REG0_F(3) + -5.0f;
                vec->x = jm_sinShort(a) * 3.0f;
                mDoMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
                MtxPosition(vec, wave);
            }
            f32 dx = pos->x - prev->x + spd->x + wave->x;
            f32 dz = pos->z - prev->z + spd->z + wave->z;
            s16 yaw = cM_atan2s(dx, dz);
            f32 dist = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
            s16 pitch = -cM_atan2s(dy, dist);
            vec->y = 0.0f;
            vec->x = 0.0f;
            vec->z = REG0_F(7) + 35.0f;
            mDoMtx_YrotS(calc_mtx(), yaw);
            mDoMtx_XrotM(calc_mtx(), pitch);
            MtxPosition(vec, out);
            ang->x = -pitch;
            ang->y = yaw + 0x8000;
            cXyz* old = &i_this->m61C[i];
            old->copy(*pos);
            f32 nx = prev->x + out->x;
            pos->x = nx;
            pos->y = prev->y + out->y;
            pos->z = prev->z + out->z;
            spd->x = (nx - old->x) * spdScale;
            spd->y = (pos->y - old->y) * spdScale;
            spd->z = (pos->z - old->z) * spdScale;
        }
        J3DModel* model = i_this->mpMorf[i]->getModel();
        f32 sz = i_this->scale.z, sx = i_this->scale.x, sy = i_this->scale.y;
        gabi::store<f32>(gabi::ea(model) + 0xBC, sx);
        gabi::store<f32>(gabi::ea(model) + 0xC0, sy);
        gabi::store<f32>(gabi::ea(model) + 0xC4, sz);
        mDoMtx_stack_c::transS(pos->x, pos->y, pos->z);
        mDoMtx_stack_c::YrotM(ang->y);
        mDoMtx_XrotM(mtx_now(), ang->x);
        mDoMtx_ZrotM(mtx_now(), ang->z);
        if (i == 0) {
            mDoMtx_stack_c::YrotM(i_this->m584);
            mDoMtx_stack_scaleM(l_HIO().m1C, l_HIO().m1C, l_HIO().m1C);
        } else {
            f32 s = i_this->m71C[i];
            mDoMtx_stack_scaleM(s, s * i_this->m73C[i], 1.0f);
            if (i == 7) {
                mDoMtx_stack_scaleM(i_this->mHeadScale, i_this->mHeadScale, i_this->mHeadScale);
            }
        }
        mDoMtx_stack_transM(0.0f, 0.0f, i_this->m58C);
        J3DModel_setBaseTRMtx(model, mtx_now());
        dCcD_Sph* sph = &i_this->mSph[i];
        if (i == 0) {
            vec->x = 0.0f;
            vec->y = 0.0f;
            vec->z = REG0_F(9) + 30.0f;
            PSMTXMultVec(mtx_now(), vec, &i_this->eyePos);
            i_this->mAtSph.SetC(&i_this->eyePos);
            vec->x = 0.0f;
            vec->y = 0.0f;
            vec->z = REG_F(6, 9) + 100.0f;
            PSMTXMultVec(mtx_now(), vec, out);
            sph->SetC(out);
            sph->mObjAt.mSPrm = (sph->mObjAt.mSPrm & ~2u) | 8;
            u8 mode = i_this->mD20;
            if (mode == 1) {
                sph->OffAtSPrmBit(1);
                sph->OffCoSPrmBit(1);
                sph->OffTgSPrmBit(1);
                i_this->mAtSph.SetR(40.0f);
            } else {
                if (mode == 2) {
                    sph->OnAtSPrmBit(1);
                } else {
                    sph->OffAtSPrmBit(1);
                }
                sph->OnCoSPrmBit(1);
                sph->OnTgSPrmBit(1);
                sph->SetR(l_HIO().m40);
                i_this->mAtSph.SetR(l_HIO().m44);
            }
            dComIfG_Ccsp_Set(&i_this->mAtSph);
            dComIfG_Ccsp_Set(sph);
        } else {
            sph->SetC(pos);
            if (i_this->m57C != 0) {
                sph->SetR(-200.0f);
            } else {
                sph->SetR(l_HIO().m48);
            }
            dComIfG_Ccsp_Set(sph);
        }
        if (i_this->mD1D != 0 && i > 0) {
            u32 k = mt_trail_idx(i_this->mD1C, i);
            cXyz* prev = &i_this->m5BC[i - 1];
            for (int j = 0; j < 6; j++, k++) {
                f32 fj = (f32)j;
                u32 idx = k & 0x3F;
                f32 px = pos->x;
                f32 stepX = (prev->x - px) / 5.0f;
                f32 stepY = (prev->y - pos->y) / 5.0f;
                i_this->m810[idx].x = gabi::fmadds(stepX, fj, px);
                i_this->m810[idx].y = gabi::fmadds(stepY, fj, pos->y);
                f32 stepZ = (prev->z - pos->z) / 5.0f;
                i_this->m810[idx].z = gabi::fmadds(stepZ, fj, pos->z);
                i_this->mB10[idx].x = ang->x;
                i_this->mB10[idx].y = ang->y;
                i_this->mB10[idx].z = ang->z;
            }
        }
    }
    if (i_this->mD1D != 0) {
        i_this->m570 = 0;
        i_this->mD1D = 0;
        i_this->m574 = 100;
        i_this->m5AA = 0;
        i_this->m571 = 0;
        i_this->mD10 = 0;
        anm_init(i_this, 10, 20.0f, 2, 1.0f, 0);
    }
    cLib_addCalc0(&i_this->m1A10, 1.0f, 0.01f);
    cLib_addCalc2(&i_this->m58C, 20.0f, 1.0f, 1.0f);
    s32 a = i_this->m586 * (REG0_S(8) + 0x258);
    cLib_addCalcAngleS2(&i_this->current.angle.z, (s16)gabi::ftoi(jm_sinShort(a) * (REG0_F(5) + 2000.0f)), 2, 0x400);
    dBgS_LinChk_dt(linChk, MT_LINCHK_VTBLS);
}
VERIFY(0x021DD634, body_control2);

/* dBgS_GndChk on the stack (HD layout, as in d_a_mo2; this TU's vtables) */
struct dBgS_GndChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x40 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x4C */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ u8 _30[0x40 - 0x30];
    /* 0x40 */ be<u32> __vtbl_40;
    /* 0x44 */ be<u8> mPass[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> __vtbl_4C;
    /* 0x50 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_GndChk_l, 0x54);
static inline void mt_GndChk_ct(dBgS_GndChk_l* c) {
    gabi::call(0x02008E0C, c); /* cBgS_GndChk::cBgS_GndChk */
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->mpGrpPassChk = gabi::ea(c) + 0x4C;
    c->__vtbl_4C = 0x100154FC;
    c->mpPolyPassChk = gabi::ea(c) + 0x40;
    c->__vtbl_40 = 0x1001550C;
    c->__vtbl_20 = 0x100154EC;
    c->mGrp = 1;
    c->__vtbl_10 = 0x100154DC;
}
static inline void mt_GndChk_dt(dBgS_GndChk_l* c) {
    c->__vtbl_20 = 0x100154EC;
    c->__vtbl_40 = 0x1001550C;
    c->__vtbl_4C = 0x100154CC;
    gabi::call(0x02008DAC, c, 0); /* cBgS_Chk::~cBgS_Chk */
}
/* a debug-register float by address (REG_F(child, i) = 0x1047B610 + 0x90 * child + 4 * i) */
static inline f32 REGA_F(u32 a) { return gabi::load<f32>(a); }
static inline s16 REGA_S(u32 a) { return gabi::load<s16>(a); }
/* JPASetRMtxTVecfromMtx(mtx, rotMtx, trans) */
static inline void JPASetRMtxTVecfromMtx(Mtx34* m, void* r, void* t) { gabi::call(0x028249B0, m, r, t); }
static inline u32 f2u(f32 f) {
    if (f < 2147483648.0f) return (u32)gabi::ftoi(f);
    return (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000u;
}

/* 021DF238: rolled up into a ball (m571: 0 rolling, 1 carried by the player, else waiting);
 * m582 counts up to unrolling (0x46 the shake, 100 the end) */
void mt_move_maru(mt_class* i_this) {
    WWHD_FUNC(0x021DF238, void, i_this);
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER));
    u8 mode = i_this->m571;
    u32 se = 0;
    i_this->m580 = 3;
    if (mode == 0) {
        if (i_this->m572 == 0) {
            i_this->mSph[0].OnCoSPrmBit(1);
        }
        i_this->shape_angle.x = i_this->shape_angle.x + (s16)gabi::ftoi(i_this->speedF * 200.0f);
        mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        gabi::Local<cXyz> vec;
        gabi::Local<cXyz> move;
        vec->x = 0.0f;
        vec->y = 0.0f;
        vec->z = i_this->speedF;
        MtxPosition(vec, move);
        f32 vy = i_this->speed.y;
        f32 fall = vy + i_this->gravity;
        f32 mx = move->x;
        i_this->speed.x = mx;
        f32 nx = i_this->current.pos.x + mx;
        f32 ny = i_this->current.pos.y + vy;
        f32 mz = move->z;
        i_this->speed.z = mz;
        f32 nz = i_this->current.pos.z + mz;
        i_this->current.pos.x = nx;
        i_this->current.pos.y = ny;
        i_this->current.pos.z = nz;
        if (fall < -100.0f) {
            fall = -100.0f;
        }
        i_this->speed.y = fall;
        mt_bg_check(i_this);
        if (i_this->mAcch.m_flags & dBgS_Acch::GROUND_HIT) {
            if (fall < REG0_F(12) + -50.0f) {
                i_this->m1A14 = 2;
            }
            f32 target = 0.0f;
            f32 step = 1.0f;
            gabi::Local<dBgS_GndChk_l> gnd;
            mt_GndChk_ct(gnd);
            gnd->m_pos.x = i_this->current.pos.x;
            gnd->m_pos.y = i_this->current.pos.y + 50.0f;
            gnd->m_pos.z = i_this->current.pos.z;
            f32 groundY = cBgS_GroundCross(dComIfG_Bgsp(), gnd);
            mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
            vec->x = 0.0f;
            vec->y = 50.0f;
            vec->z = 5.0f;
            gabi::Local<cXyz> ofs;
            MtxPosition(vec, ofs);
            gabi::Local<cXyz> ahead;
            cXyz_pl(&i_this->current.pos, ahead, ofs);
            gnd->m_pos.copy(*ahead);
            f32 aheadY = cBgS_GroundCross(dComIfG_Bgsp(), gnd);
            if (aheadY != -1000000000.0f) {
                if (aheadY < groundY - 1.0f) {
                    target = 5.0f;
                    step = 0.3f;
                } else if (aheadY > groundY + 1.0f) {
                    target = -5.0f;
                    step = 0.3f;
                }
            }
            cLib_addCalc2(&i_this->speedF, target, 1.0f, l_HIO().m5C * step);
            if (fall < REG0_F(14) + -15.0f) {
                f32 bounce = fall * (REG0_F(15) + -0.4f);
                i_this->speed.y = bounce;
                se = f2u(bounce * (REG0_F(5) + 6.0f));
                if (se > 100) se = 100;
            } else {
                i_this->speed.y = -5.0f;
            }
            mt_GndChk_dt(gnd);
        }
        if ((i_this->mAcch.m_flags & dBgS_Acch::WALL_HIT) && std::fabs((f32)i_this->speedF) > 3.0f) {
            i_this->speedF = i_this->speedF * -0.5f;
            se = 0x32;
            fopAcM_seStart(i_this, 0x5815 /* JA_SE_CM_MT_BOUND */, se);
        } else if (se != 0) {
            fopAcM_seStart(i_this, 0x5815, se);
        }
        gabi::store<u32>(gabi::ea(i_this) + 0x39C, gabi::load<u32>(gabi::ea(i_this) + 0x39C) | 0x10); /* attention_info.flags */
        gabi::store<u8>(gabi::ea(i_this) + 0x38C, 9);                                                /* attention_info.distances[4] */
        if (i_this->actor_status & 0x2000 /* fopAcStts_CARRY_e */) {
            i_this->m571 = 1;
            gabi::store<u32>(gabi::ea(i_this) + 0x39C, gabi::load<u32>(gabi::ea(i_this) + 0x39C) & ~0x10u);
        }
        cLib_addCalcAngleS2(&i_this->m584, 0, 1, 0x100);
    } else if (mode == 1) {
        i_this->current.angle.x = 0;
        i_this->current.angle.y = i_this->shape_angle.y;
        i_this->current.angle.z = i_this->shape_angle.z;
        i_this->mSph[0].OffCoSPrmBit(1);
        if (!(i_this->actor_status & 0x2000)) {
            /* thrown: if the line from the player to the ball hits a wall, it drops at the player */
            gabi::Local<dBgS_LinChk_l> linChk;
            dBgS_LinChk_ct(linChk, MT_LINCHK_VTBLS);
            gabi::Local<cXyz> start;
            start->x = player->current.pos.x;
            start->y = player->current.pos.y + 50.0f;
            start->z = player->current.pos.z;
            mDoMtx_YrotS(calc_mtx(), player->shape_angle.y);
            gabi::Local<cXyz> vec;
            vec->x = 0.0f;
            vec->y = 0.0f;
            vec->z = REG_F(18, 8) + 25.0f;
            gabi::Local<cXyz> ofs;
            MtxPosition(vec, ofs);
            gabi::Local<cXyz> end;
            end->x = i_this->current.pos.x + ofs->x;
            end->y = i_this->current.pos.y + 50.0f;
            end->z = i_this->current.pos.z + ofs->z;
            dBgS_LinChk_Set(linChk, start, end, i_this);
            if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                f32 px = player->current.pos.x;
                i_this->current.pos.x = px;
                f32 py = player->current.pos.y + 20.0f + REGA_F(0x1047BBDC);
                i_this->current.pos.y = py;
                f32 pz = player->current.pos.z;
                i_this->old.pos.x = px;
                i_this->current.pos.z = pz;
                i_this->old.pos.y = py;
                i_this->old.pos.z = pz;
            }
            f32 spd = i_this->speedF;
            i_this->m571 = 0;
            if (spd > 0.0f) {
                i_this->speedF = 20.0f * l_HIO().m58;
                i_this->m572 = 20;
                i_this->speed.y = 20.0f * l_HIO().m58;
            } else {
                i_this->speedF = 0.0f;
                i_this->m572 = 20;
                i_this->speed.y = REG0_F(11) + -15.0f;
            }
            dBgS_LinChk_dt(linChk, MT_LINCHK_VTBLS);
        } else if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0) {
            i_this->m582 = i_this->m582 + 1;
        }
        cLib_addCalcAngleS2(&i_this->m584, 0, 1, 0x100);
    } else {
        cLib_addCalcAngleS2(&i_this->m584, 0, 1, 0x100);
    }

    s16 t = i_this->m582;
    if (t > 0x46) {
        cLib_addCalc2(&i_this->m590, 1.0f, 1.0f, 0.05f);
        s16 step = l_HIO().m52;
        if (i_this->m582 < 0x6E) {
            if ((u32)(i_this->m582 - 0x50) >= 0x15) {
                step = (s16)(step - 0x5DC);
            }
            cLib_addCalcAngleS2(&i_this->shape_angle.x, 0x7800, 4, 0x300);
        }
        cLib_addCalcAngleS2(&i_this->m5A6, step, 4, 0x1000);
        cLib_addCalc2(&i_this->m594, 1000.0f, 1.0f, 5.0f);
        cLib_addCalc2(&i_this->m454, 0.3f, 1.0f, 0.01f);
    } else {
        if (t == 0x46) {
            i_this->m44C = REG0_F(3) + 2500.0f;
            i_this->m454 = 0.5f;
            if (i_this->m1A16 == 0) {
                i_this->speed.y = REG0_F(11) + 30.0f;
            }
        }
        cLib_addCalc0(&i_this->m44C, 1.0f, 125.0f);
        s32 a = i_this->m582 * (REG0_S(5) + 0x1388);
        cLib_addCalcAngleS2(&i_this->m584, (s16)gabi::ftoi(jm_sinShort(a) * i_this->m44C * 5.0f), 2, 0x1000);
        cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 4, 0x1000);
        cLib_addCalc2(&i_this->m590, -0.4f, 1.0f, 0.2f);
        a = i_this->m582 * (REG0_S(5) + 0x1388);
        cLib_addCalcAngleS2(&i_this->m5A6, (s16)gabi::ftoi(jm_sinShort(a) * i_this->m44C * (REG0_F(14) + 4.0f)), 1, 0x1000);
        s16 m584 = i_this->m584;
        s16 ay = i_this->current.angle.y;
        i_this->current.angle.x = i_this->shape_angle.x;
        i_this->shape_angle.y = ay + m584;
        cLib_addCalc0(&i_this->speedF, 1.0f, 0.5f);
    }
    if (i_this->m582 == 100) {
        i_this->m400 = 2;
    }

    /* steam from the water (water_damage_se_set) */
    u8 st = i_this->m464;
    if (st == 0) return;
    if (st == 1) {
        s8 room = i_this->current.roomNo;
        JPABaseEmitter* em = dPa_control_set(dComIfGp_getParticle(), 2, 0x8095, &i_this->current.pos, &i_this->current.angle, nullptr,
                                             0xB4, nullptr, room, nullptr, nullptr, nullptr);
        i_this->mpEmitter = em;
        if (em != nullptr) {
            gabi::store<u32>(gabi::ea(em) + 0x254, gabi::load<u32>(gabi::ea(em) + 0x254) | 0x40);
        }
        i_this->m464 = 2;
        i_this->m466 = 80;
    }
    if (i_this->mpEmitter != nullptr) {
        MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
        mDoMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
        u32 e = gabi::ea(i_this->mpEmitter.get());
        JPASetRMtxTVecfromMtx(calc_mtx(), gabi::at<u8>(e + 0x1F0), gabi::at<u8>(e + 0x22C));
        s16 timer = i_this->m466;
        if (timer == 0) {
            JPABaseEmitter_becomeInvalidEmitter(i_this->mpEmitter);
            e = gabi::ea(i_this->mpEmitter.get());
            gabi::store<s32>(e + 0x5C, -1);
            gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) | 1);
            i_this->mpEmitter = nullptr;
            i_this->m464 = 0;
        } else {
            u8 alpha = timer < 30 ? (u8)(timer * 6) : 0xB4;
            gabi::store<u8>(gabi::ea(i_this->mpEmitter.get()) + 0x247, alpha);
        }
    }
}
VERIFY(0x021DF238, mt_move_maru);

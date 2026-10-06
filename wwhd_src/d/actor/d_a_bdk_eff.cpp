/**
 * d_a_bdk_eff.cpp (WWHD)
 * Boss - Helmaroc King (battle): my_effect_move (the feather and rock effects; eff_hane_move and
 * eff_Grock_move are inlined into it).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdk.h"

using gabi::fadds_ppc;
using gabi::fsubs_ppc;

/* inline dBgS_GndChk / dBgS_LinChk on the stack (this TU's vtables) */
static const dBgS_GndChk_vt bdk_gnd_vt = {0x10007E98, 0x10007EA8, 0x10007EC8, 0x10007EB8};
static const dBgS_LinChk_vt bdk_lin_vt = {0x10007F08, 0x10007F18, 0x10007F38, 0x10007F28};
static inline void bdk_chk_dt(void* lin, void* gnd) {
    u32 l = gabi::ea(lin), g = gabi::ea(gnd);
    gabi::store<u32>(l + 0x58, 0x10007F38);
    gabi::store<u32>(l + 0x64, 0x10007E88);
    gabi::store<u32>(l + 0x20, 0x10007E78);
    gabi::call(0x02008B4C, l, 0); /* cBgS_LinChk::~cBgS_LinChk */
    gabi::store<u32>(g + 0x40, 0x10007EC8);
    gabi::store<u32>(g + 0x4C, 0x10007E88);
    gabi::store<u32>(g + 0x20, 0x10007EA8);
    gabi::call(0x02008DAC, g, 0); /* cBgS_Chk::~cBgS_Chk */
}
static inline f32 bdk_GroundCross(void* chk) { return gabi::call<f32>(0x02008974, dComIfG_Bgsp(), chk); }
static inline BOOL bdk_LineCross(void* chk) { return gabi::call<BOOL>(0x02008860, dComIfG_Bgsp(), chk); }
static inline BOOL bdk_land_area_check(cXyz* pos, f32 r) { return gabi::call<BOOL>(0x02065208, pos, r); }

/* eff_hane_move (inlined). HD: the ground height is GroundCross + 10 (GameCube 20), capped at 9820 */
static inline void eff_hane_move(bdk_class* i_this, bdk_eff_s* i_eff, gabi::Local<cXyz>& vec1, gabi::Local<cXyz>& vec2,
                                 gabi::Local<cXyz>& tmp, gabi::Local<cXyz>& vec3, gabi::Local<u8[0x54]>& gndChk,
                                 gabi::Local<u8[0x6C]>& linChk) {
    fopAc_ac_c* actor = i_this;
    f32 fVar1 = -130.0f;
    f32 fVar2 = 2.0f;
    f32 fVar3;
    f32 fVar4;

    dBgS_GndChk_ct(gndChk.get(), bdk_gnd_vt, false);
    dBgS_LinChk_ct(linChk.get(), bdk_lin_vt, false);

    if (i_eff->m040 != 0) {
        i_eff->m040 = i_eff->m040 - 1;
    }
    cMtx_YrotS(calc_mtx(), i_eff->m030.y);
    vec1->x = 0.0f;
    vec1->y = 0.0f;
    bdk_fcopy(vec1->z, i_eff->m020);
    MtxPosition(vec1, vec2);
    bdk_fcopy(vec2->y, i_eff->m01C);
    for (int k = 0; k < 12; k += 4) gabi::store<u32>(gabi::ea(&i_eff->m010) + k, gabi::load<u32>(gabi::ea(&i_eff->m004) + k));
    PSVECAdd(&i_eff->m004, vec2, &i_eff->m004);
    cXyz_mi(&i_eff->m004, tmp, &i_eff->m010);
    for (int k = 0; k < 12; k += 4) gabi::store<u32>(gabi::ea(vec1.get()) + k, gabi::load<u32>(gabi::ea(tmp.get()) + k));
    if (std_sqrtf(PSVECSquareMag(vec1)) > 0.0f) {
        cMtx_YrotS(calc_mtx(), cM_atan2s(vec1->x, vec1->z));
        vec1->x = 0.0f;
        vec1->y = 30.0f;
        vec1->z = 20.0f;
        MtxPosition(vec1, vec2);
        bdk_fcopy(vec1->x, i_eff->m004.x);
        f32 y = i_eff->m004.y;
        bdk_fcopy(vec1->y, i_eff->m004.y);
        bdk_fcopy(vec1->z, i_eff->m004.z);
        vec1->y = fadds_ppc(y, 30.0f);
        PSVECAdd(vec2, &i_eff->m004, vec2);
        dBgS_LinChk_Set(linChk.get(), vec1, vec2, actor);
        if (bdk_LineCross(linChk.get())) {
            bdk_fcopy(i_eff->m004.x, i_eff->m010.x);
            bdk_fcopy(i_eff->m004.z, i_eff->m010.z);
            i_eff->m020 = 0.0f;
        }
    }

    {
        u32 g = gabi::ea(gndChk.get());
        f32 y = i_eff->m004.y;
        bdk_fcopy(vec1->x, i_eff->m004.x);
        bdk_fcopy(vec1->y, i_eff->m004.y);
        bdk_fcopy(vec1->z, i_eff->m004.z);
        f32 y2 = fadds_ppc(y, 200.0f);
        bdk_fbits(g + 0x24, gabi::ea(&i_eff->m004.x));
        gabi::store<f32>(g + 0x28, y2);
        bdk_fbits(g + 0x2C, gabi::ea(&i_eff->m004.z));
        vec1->y = y2;
    }
    f32 height = fadds_ppc(bdk_GroundCross(gndChk.get()), 10.0f);
    if (height > 9820.0f) {
        height = 9820.0f;
    }

    i_eff->m01C = fsubs_ppc(i_eff->m01C, fadds_ppc(REG8_F(14), 0.1f));
    f32 lim = fadds_ppc(REG8_F(15), -5.0f);
    if (i_eff->m01C < lim) {
        i_eff->m01C = lim;
    }

    if (!(i_eff->m004.y > height)) {
        i_eff->m004.y = height;
        cLib_addCalc0(&i_eff->m020, 1.0f, 0.1f);
        i_eff->m030.y = (s16)(i_eff->m030.y + i_eff->m036.x);
        cLib_addCalcAngleS2(&i_eff->m036.x, 0, 10, 10);
        cLib_addCalcAngleS2(&i_eff->m036.z, 0, 10, 10);
        cLib_addCalcAngleS2(&i_eff->m030.x, 0, 10, 200);
        cLib_addCalcAngleS2(&i_eff->m030.z, 0, 10, 200);
    } else {
        f32 m028 = i_eff->m028;
        i_eff->m030.x = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_ssin(i_eff->m03C * 0x500), m028));
        i_eff->m030.z = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_scos(i_eff->m03C * 0x400), m028));
        if (i_eff->m040 != 0) {
            fVar3 = fadds_ppc(REG8_F(7), 15000.0f);
            fVar4 = 1000.0f;
            fVar1 = fadds_ppc(REG8_F(8), -300.0f);
            fVar2 = 10.0f;
        } else {
            fVar3 = 8000.0f;
            fVar4 = 200.0f;
        }
        cLib_addCalc2(&i_eff->m028, fVar3, 1.0f, fVar4);
    }

    i_eff->m036.y = (s16)(i_eff->m036.y + i_eff->m036.z);
    if (i_eff->m004.y > fadds_ppc(REG8_F(11), 9930.0f)) {
        cLib_addCalc2(&i_eff->m024, fVar1, 0.1f, fVar2);
    } else {
        cLib_addCalc0(&i_eff->m024, 0.1f, 1.3f);
    }
    MtxTrans(i_eff->m004.x, i_eff->m004.y, i_eff->m004.z, 0);
    cMtx_YrotM(calc_mtx(), i_eff->m036.y);
    MtxTrans(0.0f, -i_eff->m024, 0.0f, 1);
    cMtx_XrotM(calc_mtx(), i_eff->m030.x);
    cMtx_ZrotM(calc_mtx(), i_eff->m030.z);
    MtxTrans(0.0f, i_eff->m024, 0.0f, 1);
    f32 scale = i_eff->m02C;
    MtxScale(scale, scale, scale, 1);
    bdk_setBaseTRMtx(i_eff->m044, calc_mtx());

    if (i_eff->m03E != 0) {
        i_eff->m03E = i_eff->m03E - 1;
    }

    if (i_eff->m004.y < 9700.0f || i_eff->m03E == 0 || bdk_land_area_check(&i_eff->m004, 3000.0f) == FALSE) {
        i_eff->m001 = 1;
    } else {
        i_eff->m048.SetC(&i_eff->m004);
        dComIfG_Ccsp_Set(&i_eff->m048);
    }

    if (i_eff->m048.ChkTgHit() && i_eff->m040 == 0) {
        u32 obj = gabi::ea(i_eff->m048.GetTgHitObj());
        if (obj != 0) {
            u32 stts = gabi::load<u32>(obj + 0x44);
            u32 tgActor = stts != 0 ? gabi::load<u32>(stts + 0xC) : 0; /* GetAc() */
            if (tgActor != 0) {
                cXyz_mi(&i_eff->m004, vec3, gabi::at<cXyz>(tgActor + 0x314));
                if (i_eff->m020 < 5.0f) {
                    s16 a = cM_atan2s(vec3->x, vec3->z);
                    i_eff->m030.y = (s16)(a + (s16)gabi::ftoi(cM_rndFX(4000.0f)));
                    i_eff->m020 = fadds_ppc(fadds_ppc(cM_rndF(5.0f), 10.0f), REG0_F(8));
                    i_eff->m01C = fadds_ppc(fadds_ppc(cM_rndF(5.0f), 10.0f), REG0_F(9));
                }
                i_eff->m036.z = (s16)gabi::ftoi(cM_rndFX(fadds_ppc(REG0_F(10), 1500.0f)));
                i_eff->m036.x = (s16)gabi::ftoi(cM_rndFX(fadds_ppc(REG0_F(16), 1500.0f)));
                i_eff->m040 = (s8)gabi::ftoi(fadds_ppc(cM_rndF(30.0f), 30.0f));
            }
        }
    }
    if (i_eff->m001 != 0) {
        cLib_addCalc0(&i_eff->m02C, 1.0f, 0.05f);
        if (i_eff->m02C < 0.05f) {
            i_eff->m000 = 0;
        }
    }
    bdk_chk_dt(linChk.get(), gndChk.get());
}

/* eff_Grock_move (inlined) */
static inline void eff_Grock_move(bdk_class* i_this, bdk_eff_s* eff) {
    f32 fVar1 = 0.0f;
    if (eff->m03C == 1) {
        fVar1 = -10.0f;
    }
    MtxTrans(eff->m004.x, fadds_ppc(eff->m004.y, fVar1), eff->m004.z, 0);
    bdk_setBaseTRMtx(eff->m044, calc_mtx());
}

/* 02071A68 (matcher: eff_hane_move; it is my_effect_move with eff_hane_move and eff_Grock_move
 * inlined) */
void my_effect_move(bdk_class* i_this) {
    WWHD_FUNC(0x02071A68, void, i_this);
    gabi::Local<cXyz> vec1;
    gabi::Local<cXyz> vec2;
    gabi::Local<cXyz> tmp;
    gabi::Local<cXyz> vec3;
    gabi::Local<u8[0x54]> gndChk;
    gabi::Local<u8[0x6C]> linChk;
    bdk_eff_s* eff = &i_this->m261C[0];
    for (s32 i = 0; i < 40; i++, eff++) {
        if (eff->m000 != 0) {
            eff->m03C = eff->m03C + 1;
            if (eff->m000 == 1) {
                eff_hane_move(i_this, eff, vec1, vec2, tmp, vec3, gndChk, linChk);
            } else if (eff->m000 == 2) {
                eff_Grock_move(i_this, eff);
            }
        }
    }
}
VERIFY(0x02071A68, my_effect_move);

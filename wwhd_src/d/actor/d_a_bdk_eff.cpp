/**
 * d_a_bdk_eff.cpp (WWHD)
 * Boss - Helmaroc King (battle): my_effect_move (the feather and rock effects; eff_hane_move and
 * eff_Grock_move are inlined into it).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdk_eff.h"

using gabi::fadds_ppc;
using gabi::fsubs_ppc;

/* inline dBgS_GndChk / dBgS_LinChk on the stack (this TU's vtables) */
static const dBgS_GndChk_vt bdk_gnd_vt = {0x10007E98, 0x10007EA8, 0x10007EC8, 0x10007EB8};
static const dBgS_LinChk_vt bdk_lin_vt = {0x10007F08, 0x10007F18, 0x10007F38, 0x10007F28};
/* eff_hane_move (inlined). HD: the ground height is GroundCross + 10 (GameCube 20), capped at 9820 */
static inline void eff_hane_move(bdk_class* i_this, bdk_eff_s* i_eff, gabi::Local<cXyz>& vec1, gabi::Local<cXyz>& vec2,
                                 gabi::Local<cXyz>& tmp, gabi::Local<cXyz>& vec3, gabi::Local<dBgS_GndChk>& gndChk,
                                 gabi::Local<dBgS_LinChk>& linChk) {
    fopAc_ac_c* actor = i_this;
    f32 pivotTarget = -130.0f;
    f32 pivotStep = 2.0f;
    f32 flutterTarget;
    f32 flutterStep;

    dBgS_GndChk_ct(gndChk.get(), bdk_gnd_vt, false);
    dBgS_LinChk_ct(linChk.get(), bdk_lin_vt, false);

    if (i_eff->m040 != 0) {
        i_eff->m040 = i_eff->m040 - 1;
    }
    cMtx_YrotS(calc_mtx(), i_eff->m030.y);
    vec1->x = 0.0f;
    vec1->y = 0.0f;
    bdkEffect_copyFloat(vec1->z, i_eff->m020);
    MtxPosition(vec1, vec2);
    bdkEffect_copyFloat(vec2->y, i_eff->m01C);
    bdkEffect_copyVector(&i_eff->m010, &i_eff->m004);
    PSVECAdd(&i_eff->m004, vec2, &i_eff->m004);
    cXyz_mi(&i_eff->m004, tmp, &i_eff->m010);
    bdkEffect_copyVector(vec1.get(), tmp.get());
    if (std_sqrtf(PSVECSquareMag(vec1)) > 0.0f) {
        cMtx_YrotS(calc_mtx(), cM_atan2s(vec1->x, vec1->z));
        vec1->x = 0.0f;
        vec1->y = 30.0f;
        vec1->z = 20.0f;
        MtxPosition(vec1, vec2);
        bdkEffect_copyFloat(vec1->x, i_eff->m004.x);
        f32 y = i_eff->m004.y;
        bdkEffect_copyFloat(vec1->y, i_eff->m004.y);
        bdkEffect_copyFloat(vec1->z, i_eff->m004.z);
        vec1->y = fadds_ppc(y, 30.0f);
        PSVECAdd(vec2, &i_eff->m004, vec2);
        linChk->Set(vec1.get(), vec2.get(), actor);
        if (dComIfG_Bgsp()->LineCross(linChk.get())) {
            bdkEffect_copyFloat(i_eff->m004.x, i_eff->m010.x);
            bdkEffect_copyFloat(i_eff->m004.z, i_eff->m010.z);
            i_eff->m020 = 0.0f;
        }
    }

    {
        f32 y = i_eff->m004.y;
        bdkEffect_copyFloat(vec1->x, i_eff->m004.x);
        bdkEffect_copyFloat(vec1->y, i_eff->m004.y);
        bdkEffect_copyFloat(vec1->z, i_eff->m004.z);
        f32 y2 = fadds_ppc(y, 200.0f);
        bdkEffect_copyFloat(gndChk->m_pos.x, i_eff->m004.x);
        gndChk->m_pos.y = y2;
        bdkEffect_copyFloat(gndChk->m_pos.z, i_eff->m004.z);
        vec1->y = y2;
    }
    f32 height = fadds_ppc(dComIfG_Bgsp()->GroundCross(gndChk.get()), 10.0f);
    if (height > 9820.0f) {
        height = 9820.0f;
    }

    i_eff->m01C = fsubs_ppc(i_eff->m01C, fadds_ppc(REG8_F(14), 0.1f));
    f32 lim = fadds_ppc(REG8_F(15), -5.0f);
    if (i_eff->m01C < lim) {
        i_eff->m01C = lim;
    }

    // HD: the unordered case takes the ground branch.
    if (!(i_eff->m004.y > height)) {
        i_eff->m004.y = height;
        cLib_addCalc0(&i_eff->m020, 1.0f, 0.1f);
        i_eff->m030.y = (s16)(i_eff->m030.y + i_eff->m036.x);
        cLib_addCalcAngleS2(&i_eff->m036.x, 0, 10, 10);
        cLib_addCalcAngleS2(&i_eff->m036.z, 0, 10, 10);
        cLib_addCalcAngleS2(&i_eff->m030.x, 0, 10, 200);
        cLib_addCalcAngleS2(&i_eff->m030.z, 0, 10, 200);
    } else {
        f32 flutterAmplitude = i_eff->m028;
        i_eff->m030.x = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_ssin(i_eff->m03C * 0x500), flutterAmplitude));
        i_eff->m030.z = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_scos(i_eff->m03C * 0x400), flutterAmplitude));
        if (i_eff->m040 != 0) {
            flutterTarget = fadds_ppc(REG8_F(7), 15000.0f);
            flutterStep = 1000.0f;
            pivotTarget = fadds_ppc(REG8_F(8), -300.0f);
            pivotStep = 10.0f;
        } else {
            flutterTarget = 8000.0f;
            flutterStep = 200.0f;
        }
        cLib_addCalc2(&i_eff->m028, flutterTarget, 1.0f, flutterStep);
    }

    i_eff->m036.y = (s16)(i_eff->m036.y + i_eff->m036.z);
    if (i_eff->m004.y > fadds_ppc(REG8_F(11), 9930.0f)) {
        cLib_addCalc2(&i_eff->m024, pivotTarget, 0.1f, pivotStep);
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
    bdkEffect_setBaseTRMtx(i_eff->m044, calc_mtx());

    if (i_eff->m03E != 0) {
        i_eff->m03E = i_eff->m03E - 1;
    }

    if (i_eff->m004.y < 9700.0f || i_eff->m03E == 0 || BdkEffect_land_area_check(&i_eff->m004, 3000.0f) == FALSE) {
        i_eff->m001 = 1;
    } else {
        i_eff->m048.SetC(&i_eff->m004);
        dComIfG_Ccsp_Set(&i_eff->m048);
    }

    if (i_eff->m048.ChkTgHit() && i_eff->m040 == 0) {
        dCcD_GObjInf* obj = static_cast<dCcD_GObjInf*>(i_eff->m048.GetTgHitObj());
        if (obj) {
            dCcD_Stts* stts = obj->mStts;
            fopAc_ac_c* tgActor = stts ? stts->mp_actor.get() : nullptr;
            if (tgActor) {
                cXyz_mi(&i_eff->m004, vec3, &tgActor->current.pos);
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
    bdkEffect_destroyChecks(linChk.get(), gndChk.get());
}

/* eff_Grock_move (inlined) */
static inline void eff_Grock_move(bdk_class* i_this, bdk_eff_s* eff) {
    f32 pivotTarget = 0.0f;
    if (eff->m03C == 1) {
        pivotTarget = -10.0f;
    }
    MtxTrans(eff->m004.x, fadds_ppc(eff->m004.y, pivotTarget), eff->m004.z, 0);
    bdkEffect_setBaseTRMtx(eff->m044, calc_mtx());
}

// HD: feather and rock updates are inlined into the effect loop.
/* 02071A68 */
void my_effect_move(bdk_class* i_this) {
    gabi::Local<cXyz> vec1;
    gabi::Local<cXyz> vec2;
    gabi::Local<cXyz> tmp;
    gabi::Local<cXyz> vec3;
    gabi::Local<dBgS_GndChk> gndChk;
    gabi::Local<dBgS_LinChk> linChk;
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


/* WWHD glue
 ext 02008B4C void BdkEffect_cBgS_LinChk_dt(cBgS_LinChk*, u32)
 ext 02008DAC void BdkEffect_cBgS_Chk_dt(cBgS_Chk*, u32)
 ext 02065208 BOOL BdkEffect_land_area_check(cXyz*, f32)
*/

/**
 * d_a_bdk_obj.cpp (WWHD)
 * Boss - Helmaroc King (battle): kankyo_cont, obj_move.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdk.h"

using gabi::fadds_ppc;
using gabi::fsubs_ppc;

/* 0206F9E0 */
void kankyo_cont(bdk_class* i_this) {
    WWHD_FUNC(0x0206F9E0, void, i_this);
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_38;
    i_this->m25DC.copy(actor->current.pos);
    local_38->x = 0.0f;
    local_38->y = 0.0f;
    local_38->z = 1.0f;
    cMtx_YrotS(calc_mtx(), actor->current.angle.y);
    MtxPosition(local_38, &i_this->m25E8);

    i_this->m25F4.z = 0.0f;
    i_this->m25F4.x = 2000.0f * i_this->m2608;
    cLib_addCalc0(&i_this->m2608, 1.0f, 0.02f);
    /* HD: fsel (m2608 > 1.0f, NaN gives 1.0f) */
    f32 d = 1.0f - i_this->m2608;
    i_this->m25F4.y = (d >= 0.0f) ? 0.0f : 1.0f;

    /* static cXyz non_pos (HD: guard word 0x104618C4, object 0x10461948) */
    if (gabi::load<u32>(0x104618C4) == 0) {
        gabi::store<u32>(0x104618C4, 1);
    }
    cXyz* non_pos = gabi::at<cXyz>(0x10461948);
    non_pos->x = 0.0f;
    non_pos->y = -10000.0f;
    non_pos->z = 10000.0f;

    for (s32 i = 0; i < 10; i++) {
        dCcD_Sph* sph = &i_this->mWindAtSph[i];
        if (i_this->m2488[i] != 0) {
            PSVECAdd(&i_this->m2494[i], &i_this->m250C[i], &i_this->m2494[i]);
            if (i_this->m2B4 == 1) {
                sph->SetR(fadds_ppc(REG8_F(7), 600.0f));
            } else {
                sph->SetR(fadds_ppc(REG8_F(6), 300.0f));
            }
            if (i_this->m2488[i] == 1) {
                dCcD_Sph_StartCAt(sph, &i_this->m2494[i]);
            } else {
                dCcD_Sph_MoveCAt(sph, &i_this->m2494[i]);
            }
            u8 v = (u8)(i_this->m2488[i] + 1);
            if (v > 25) {
                i_this->m2488[i] = 0;
            } else {
                i_this->m2488[i] = v;
            }
        } else {
            sph->SetC(non_pos);
        }
        dComIfG_Ccsp_Set(sph);
    }
}
VERIFY(0x0206F9E0, kankyo_cont);

/* 0206FC6C */
void obj_move(bdk_class* i_this) {
    WWHD_FUNC(0x0206FC6C, void, i_this);
    if (dSv_memBit_isDungeonItem(bdk_memBit(), 3) /* dComIfGs_isStageBossEnemy */) {
        cLib_addCalc2(&i_this->m62D4, -500.0f, 1.0f, 20.0f);
    }
    f32 y = fadds_ppc(fadds_ppc(REG0_F(11), 9800.0f), i_this->m62D4);
    MtxTrans(fadds_ppc(REG0_F(10), 4755.0f), y, fsubs_ppc(REG0_F(12), 4700.0f), 0);
    cMtx_YrotM(calc_mtx(), (s16)(REG0_S(0) - 7000));
    bdk_setBaseTRMtx(i_this->mp62D8, calc_mtx());
    PSMTXCopy(calc_mtx(), &i_this->m62DC);
    dBgW_Move(i_this->pm_bgw);
    MtxTrans(3595.0f, fadds_ppc(fadds_ppc(i_this->m6324, 9800.0f), REG8_F(2)), -3820.0f, 0);
    for (s32 i = 0; i < 3; i++) {
        MtxRotY(2.094395160675049f /* M_PI * 2 / 3 */, 1);
        MtxPush();
        MtxRotY(i_this->m631C, 1);
        MtxTrans(0.0f, 0.0f, fadds_ppc(i_this->m6320, REG8_F(3)), 1);
        bdk_setBaseTRMtx(i_this->mp6310[i], calc_mtx());
        PSMTXCopy(calc_mtx(), &i_this->m632C[i]);
        dBgW_Move(i_this->mp63BC[i]);
        MtxPull();
    }
}
VERIFY(0x0206FC6C, obj_move);

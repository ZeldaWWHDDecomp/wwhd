/**
 * d_a_bdk_obj.cpp (WWHD)
 * Boss - Helmaroc King (battle): kankyo_cont, obj_move.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdk_obj.h"

static cXyz non_pos;
static BdkObjectGuard non_pos_guard;
static BdkObjectSavePointer l_save;

using gabi::fadds_ppc;
using gabi::fsubs_ppc;

/* 0206F9E0 */
void kankyo_cont(bdk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> direction;
    i_this->m25DC.copy(actor->current.pos);
    direction->x = 0.0f;
    direction->y = 0.0f;
    direction->z = 1.0f;
    cMtx_YrotS(calc_mtx(), actor->current.angle.y);
    MtxPosition(direction, &i_this->m25E8);

    i_this->m25F4.z = 0.0f;
    i_this->m25F4.x = 2000.0f * i_this->m2608;
    cLib_addCalc0(&i_this->m2608, 1.0f, 0.02f);
    // HD: fsel selects one for NaN; preserve the subtraction and ordered comparison.
    f32 d = 1.0f - i_this->m2608;
    i_this->m25F4.y = (d >= 0.0f) ? 0.0f : 1.0f;

    // HD: guarded storage is initialized without a constructor call.
    if (!non_pos_guard.mInitialized) {
        non_pos_guard.mInitialized = 1;
    }
    non_pos.x = 0.0f;
    non_pos.y = -10000.0f;
    non_pos.z = 10000.0f;

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
            u8 age = (u8)(i_this->m2488[i] + 1);
            if (age > 25) {
                i_this->m2488[i] = 0;
            } else {
                i_this->m2488[i] = age;
            }
        } else {
            sph->SetC(&non_pos);
        }
        dComIfG_Ccsp_Set(sph);
    }
}

/* 0206FC6C */
void obj_move(bdk_class* i_this) {
    if (BdkObject_isDungeonItem(&l_save.mpSave->mSave.mMemory.mMembit, 3)) {
        cLib_addCalc2(&i_this->m62D4, -500.0f, 1.0f, 20.0f);
    }
    f32 y = fadds_ppc(fadds_ppc(REG0_F(11), 9800.0f), i_this->m62D4);
    MtxTrans(fadds_ppc(REG0_F(10), 4755.0f), y, fsubs_ppc(REG0_F(12), 4700.0f), 0);
    cMtx_YrotM(calc_mtx(), (s16)(REG0_S(0) - 7000));
    bdkObject_setBaseTRMtx(i_this->mp62D8, calc_mtx());
    PSMTXCopy(calc_mtx(), &i_this->m62DC);
    dBgW_Move(i_this->pm_bgw);
    MtxTrans(3595.0f, fadds_ppc(fadds_ppc(i_this->m6324, 9800.0f), REG8_F(2)), -3820.0f, 0);
    for (s32 i = 0; i < 3; i++) {
        MtxRotY(2.094395160675049f /* M_PI * 2 / 3 */, 1);
        MtxPush();
        MtxRotY(i_this->m631C, 1);
        MtxTrans(0.0f, 0.0f, fadds_ppc(i_this->m6320, REG8_F(3)), 1);
        bdkObject_setBaseTRMtx(i_this->mp6310[i], calc_mtx());
        PSMTXCopy(calc_mtx(), &i_this->m632C[i]);
        dBgW_Move(i_this->mp63BC[i]);
        MtxPull();
    }
}

/* WWHD glue
 data non_pos 10461948
 data non_pos_guard 104618C4
 data l_save 101F84DC
 ext 025B9100 BOOL BdkObject_isDungeonItem(dSv_memBit_c*, s32)
*/

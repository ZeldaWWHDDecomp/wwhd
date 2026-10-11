/**
 * d_a_bdk_draw.cpp (WWHD)
 * Boss - Helmaroc King (battle): daBdk_Draw (kamen_draw, tail_draw, kamen_break_draw,
 * my_effect_draw and obj_draw inlined).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdk_draw.h"

static BdkDrawTailScales tial_scale;
static BdkDrawMaskStages kamen_pt;
static BdkDrawBlur l_blur;

using gabi::fadds_ppc;



/* kamen_draw (inlined). HD: the crack stage is shown by switching the mask's joint meshes on and off
 * directly (no visibility animation) */
static inline void kamen_draw(bdk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    f32 amplitude = (f32)i_this->m2FA * fadds_ppc(REG0_F(14), 150.0f);
    s16 yaw = (s16)gabi::ftoi(cM_ssin((s32)i_this->m2C4 * 0x5100) * amplitude);
    J3DModel* kamen_model = i_this->mp8F0;
    s16 roll = (s16)gabi::ftoi(cM_scos((s32)i_this->m2C4 * 0x4300) * amplitude);
    f32 maskScale = 1.0f;
    if (i_this->m2FA & 2) {
        maskScale = fadds_ppc(REG8_F(18), 1.2f);
    }
    J3DModel_bdk* model = (J3DModel_bdk*)bdkDraw_getModel(i_this->mpMorf);
    PSMTXCopy(bdkDraw_getAnmMtx(model, DK_JNT_J_DK_ATAMA1_e), calc_mtx());
    cMtx_YrotM(calc_mtx(), (s16)(yaw + 0x4000));
    cMtx_ZrotM(calc_mtx(), (s16)(roll - 0x4000));
    MtxTrans(0.0f, 40.0f, 125.0f, 1);
    MtxScale(maskScale, maskScale, maskScale, 1);
    bdkDraw_setBaseTRMtx(kamen_model, calc_mtx());

    if (i_this->m8F8 <= 3) {
        setLightTevColorType(dKy_getEnvlight(), kamen_model, &actor->tevStr);
        u32 shown = 0;
        BdkDrawModel* mask = reinterpret_cast<BdkDrawModel*>(kamen_model);
        for (s32 i = 1; i < BdkDraw_getJointRegistry(mask->mpModelData)->mJointCount; i++, shown++) {
            BdkDrawModelData* data = mask->mpModelData;
            u32 count = data->mJointCount;
            BdkDrawJoint* joint = data->mpJoints;
            if (static_cast<u16>(i) < count) joint += static_cast<u16>(i);
            for (BdkDrawMesh* mesh = joint->mpMesh; mesh; mesh = mesh->mpNext) {
                u32 visibleJoint = kamen_pt.mJoint[i_this->m8F8];
                mesh->mpShape->mVisible = visibleJoint == shown;
            }
        }
        mDoExt_modelUpdateDL(kamen_model);
    }
}

// HD: no real shadow/model; blur also runs when the boss is beaten.
/* 02065AB4 */
static BOOL daBdk_Draw(bdk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    J3DModel* model = bdkDraw_getModel(i_this->mpMorf);

    if (i_this->m259E >= 1) {
        if (i_this->m259E > 1) {
            l_blur.mRate = static_cast<u8>(i_this->m259E);
            BdkDraw_onBlure();
        } else {
            i_this->m259E = 0;
            l_blur.mEnabled = 0;
        }
    }

    if (!BdkDraw_isDungeonItem(&l_save.mpSave->mSave.mMemory.mMembit, 3)) {
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        bdkDraw_copyTev(&actor->tevStr, &player->tevStr);
        setLightTevColorType(dKy_getEnvlight(), model, &actor->tevStr);
        i_this->mpMorf->entryDL();

        kamen_draw(i_this);

        /* tail_draw */
        for (s32 t = 0; t < 4; t++) {
            bdk_tail_s* tail = &i_this->m300[t];
            if (i_this->m2F6 != 0) {
                i_this->m2F6 = i_this->m2F6 - 1;
                continue;
            }
            for (s32 i = 0; i < 9; i++) {
                f32 scale = tial_scale.mScale[i] * l_HIO.m00C;
                cXyz* pos = &tail->m024[i];
                MtxTrans(pos->x, pos->y, pos->z, 0);
                MtxScale(scale, scale, scale, 1);
                csXyz* angle = &tail->m09C[i];
                cMtx_YrotM(calc_mtx(), angle->y);
                cMtx_XrotM(calc_mtx(), angle->x);
                J3DModel* m = tail->m000[i];
                bdkDraw_setBaseTRMtx(m, calc_mtx());
                setLightTevColorType(dKy_getEnvlight(), m, &actor->tevStr);
                mDoExt_modelUpdateDL(m);
            }
        }
        dSnap_RegistFig(DSNAP_TYPE_BDK, actor, 1.0f, 1.0f, 1.0f);
    }

    /* kamen_break_draw */
    for (s32 i = 0; i < 4; i++) {
        if (i_this->m90C[i] != 0) {
            J3DModel* m = i_this->m8FC[i];
            setLightTevColorType(dKy_getEnvlight(), m, &actor->tevStr);
            mDoExt_modelUpdateDL(m);
        }
    }
    /* my_effect_draw: eff_hane_draw / eff_Grock_draw */
    for (s32 i = 0; i < 0x28; i++) {
        bdk_eff_s* eff = &i_this->m261C[i];
        s8 type = eff->m000;
        if (type != 0) {
            if (type == 1) {
                J3DModel* m = eff->m044;
                setLightTevColorType(dKy_getEnvlight(), m, &actor->tevStr);
                mDoExt_modelUpdateDL(m);
            } else if (type == 2) {
                J3DModel* m = eff->m044;
                setLightTevColorType(dKy_getEnvlight(), m, &i_this->m6224);
                mDoExt_modelUpdateDL(m);
            }
        }
    }
    /* obj_draw */
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG3, &actor->current.pos, &i_this->m6224);
    J3DModel* m = i_this->mp62D8;
    setLightTevColorType(dKy_getEnvlight(), m, &i_this->m6224);
    mDoExt_modelUpdateDL(m);
    for (s32 i = 0; i < 3; i++) {
        m = i_this->mp6310[i];
        setLightTevColorType(dKy_getEnvlight(), m, &i_this->m6224);
        dComIfGd_setListBG();
        mDoExt_modelUpdateDL(m);
        dComIfGd_setList();
    }
    return TRUE;
}


/* WWHD glue
 data tial_scale 101906E8
 data kamen_pt 101906D8
 data l_blur 101F4825
 ext 027F3F94 BdkDrawJointRegistry* BdkDraw_getJointRegistry(BdkDrawModelData*)
 ext 025F064C void BdkDraw_onBlure()
 ext 025B9100 BOOL BdkDraw_isDungeonItem(dSv_memBit_c*, s32)
*/

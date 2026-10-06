/**
 * d_a_bdk_draw.cpp (WWHD)
 * Boss - Helmaroc King (battle): daBdk_Draw (kamen_draw, tail_draw, kamen_break_draw,
 * my_effect_draw and obj_draw inlined).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdk.h"

using gabi::fadds_ppc;

static inline f32 tial_scale(s32 i) { return gabi::load<f32>(0x101906E8 + 4 * i); }
static inline u32 kamen_pt(u32 i) { return gabi::load<u32>(0x101906D8 + 4 * i); }
/* 027F3F94 (the matcher names it __nw): the model data's joint tree; joint count (u16) at +8 */
static inline u16 draw_getJointNum(u32 modelData) { return gabi::load<u16>(gabi::ea(gabi::call<void*>(0x027F3F94, modelData)) + 8); }

/* kamen_draw (inlined). HD: the crack stage is shown by switching the mask's joint meshes on and off
 * directly (no visibility animation) */
static inline void kamen_draw(bdk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    f32 fVar1 = (f32)i_this->m2FA * fadds_ppc(REG0_F(14), 150.0f);
    s16 sVar1 = (s16)gabi::ftoi(cM_ssin((s32)i_this->m2C4 * 0x5100) * fVar1);
    J3DModel* kamen_model = i_this->mp8F0;
    s16 sVar2 = (s16)gabi::ftoi(cM_scos((s32)i_this->m2C4 * 0x4300) * fVar1);
    f32 fVar3 = 1.0f;
    if (i_this->m2FA & 2) {
        fVar3 = fadds_ppc(REG8_F(18), 1.2f);
    }
    J3DModel_bdk* model = (J3DModel_bdk*)morf_model(i_this->mpMorf);
    PSMTXCopy(bdk_getAnmMtx(model, 0x18 /* DK_JNT_J_DK_ATAMA1_e */), calc_mtx());
    cMtx_YrotM(calc_mtx(), (s16)(sVar1 + 0x4000));
    cMtx_ZrotM(calc_mtx(), (s16)(sVar2 - 0x4000));
    MtxTrans(0.0f, 40.0f, 125.0f, 1);
    MtxScale(fVar3, fVar3, fVar3, 1);
    bdk_setBaseTRMtx(kamen_model, calc_mtx());

    if (i_this->m8F8 <= 3) {
        setLightTevColorType(dKy_getEnvlight(), kamen_model, &actor->tevStr);
        u32 shown = 0;
        for (s32 i = 1; i < draw_getJointNum(gabi::load<u32>(gabi::ea(kamen_model) + 0xAC)); i++, shown++) {
            u32 data = gabi::load<u32>(gabi::ea(kamen_model) + 0xAC);
            u32 n = gabi::load<u32>(data + 4);
            u32 p = gabi::load<u32>(data + 8);
            if ((u16)i < n) p += (u16)i * 0x1C;
            for (u32 mesh = gabi::load<u32>(p + 0x10); mesh != 0; mesh = gabi::load<u32>(mesh + 4)) {
                u32 v = kamen_pt(i_this->m8F8);
                gabi::store<u8>(gabi::load<u32>(mesh + 8) + 4, v == shown);
            }
        }
        mDoExt_modelUpdateDL(kamen_model);
    }
}

/* 02065AB4 (HD: no real shadow and no shadow model; the blur also runs when the boss is beaten) */
static BOOL daBdk_Draw(bdk_class* i_this) {
    WWHD_FUNC(0x02065AB4, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    J3DModel* model = morf_model(i_this->mpMorf);

    if (i_this->m259E >= 1) {
        if (i_this->m259E > 1) {
            gabi::store<u8>(0x101F4826, (u8)i_this->m259E); /* mDoGph_gInf_c::setBlureRate */
            gabi::call(0x025F064C);                          /* mDoGph_gInf_c::onBlure */
        } else {
            i_this->m259E = 0;
            gabi::store<u8>(0x101F4825, 0); /* mDoGph_gInf_c::offBlure */
        }
    }

    if (!dSv_memBit_isDungeonItem(bdk_memBit(), 3) /* dComIfGs_isStageBossEnemy */) {
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        bdk_tevstr_copy(&actor->tevStr, &player->tevStr);
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
                f32 scale = tial_scale(i) * l_HIO.m00C;
                cXyz* pos = &tail->m024[i];
                MtxTrans(pos->x, pos->y, pos->z, 0);
                MtxScale(scale, scale, scale, 1);
                csXyz* angle = &tail->m09C[i];
                cMtx_YrotM(calc_mtx(), angle->y);
                cMtx_XrotM(calc_mtx(), angle->x);
                J3DModel* m = tail->m000[i];
                bdk_setBaseTRMtx(m, calc_mtx());
                setLightTevColorType(dKy_getEnvlight(), m, &actor->tevStr);
                mDoExt_modelUpdateDL(m);
            }
        }
        dSnap_RegistFig(0xCA /* DSNAP_TYPE_BDK */, actor, 1.0f, 1.0f, 1.0f);
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
    settingTevStruct(dKy_getEnvlight(), 4 /* TEV_TYPE_BG3 */, &actor->current.pos, &i_this->m6224);
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
VERIFY(0x02065AB4, daBdk_Draw);

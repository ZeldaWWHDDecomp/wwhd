/**
 * d_a_bmdhand.cpp (WWHD)
 * Boss - Kalle Demos (ceiling tentacles) / 森ボス触手 (Forest boss tentacle)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bmdhand.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bmdhand.h"

enum {
    dRes_INDEX_BMDHAND_BCK_FOOK_HIRAKU_e = 5,
    dRes_INDEX_BMDHAND_BCK_FOOK_TOJIRU_e = 6,
    dRes_INDEX_BMDHAND_BMD_BKM_FOOK_e = 9,
    dRes_INDEX_BMDHAND_BTI_SYOKUSYU_UE_e = 0xC,
};

const gc_tu_data gc_tu = {
    .safeString = 0x10009ED4,
    .headerStatics = {0x10462508, 0x10191CD8, 0x10462524},
};

static BmdhandBossView* boss;
static u8 hio_set;
static daBmdhand_HIO_c l_HIO;
static BmdhandJointIndices boss_joint_d;
static BmdhandJointOffsets boss_joint_xad;
static dCcD_SrcSph cc_sph_src;
static GXColor line_color;
static BmdhandArchiveName hand_close_arc;
static BmdhandArchiveName hand_open_arc;

// HD: process, sound and particle IDs differ from their GameCube values.
enum {
    fpcNm_BMD_e = 0xEB,
    JA_SE_CM_BKM_VINE_SET = 0x584B,
    JA_SE_CV_BKM_CUT_VINE_1 = 0x4853,
    JA_SE_CM_BKM_CUT_VINE = 0x584E,
    ID_AK_SN_O_BKMTENTACLEBLOOD00 = 0x8067,
    ID_AK_JN_SIBOUBAKUEN = 0x13,
    ID_AK_JN_SIBOUFLASH = 0x16,
};

enum BmdhandAction {
    Bmdhand_Attached = 0,
    Bmdhand_Cut = 1,
    Bmdhand_BossDown = 2,
    Bmdhand_Intro = 3,
    Bmdhand_Disappear = 4,
    Bmdhand_RegrowWait = 5,
};

/* 020B8650 */
static BOOL daBmdhand_Draw(bmdhand_class* i_this) {

    fopAc_ac_c* actor = i_this;
    if (i_this->m310 > 0.1f) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &actor->eyePos, &actor->tevStr);
        /* hand_draw */
        if (i_this->m320 > 0.01f) {
            J3DModel* model = i_this->mpMorf->getModel();
            setLightTevColorType(dKy_getEnvlight(), model, &actor->tevStr);
            i_this->mpMorf->updateDL();
        }
        Bmdhand_updateLine(&i_this->mLineMat, 0x14, &line_color, &actor->tevStr);
        dComIfGd_set3DlineMat(&i_this->mLineMat);
    }
    return TRUE;
}

/* 020B8724 */
void hand_mtx_set(bmdhand_class* i_this) {

    MtxTrans(i_this->m2F0.x, i_this->m2F0.y, i_this->m2F0.z, false);
    cMtx_XrotM(calc_mtx(), i_this->m300);
    cMtx_YrotM(calc_mtx(), i_this->m302);
    cMtx_XrotM(calc_mtx(), -0x4000);
    f32 x = i_this->m320;
    MtxScale(x, x, x, true);
    MtxTrans(0.0f, -130.0f, 0.0f, true);
    J3DModel* model = i_this->mpMorf->getModel();
    model->setBaseTRMtx(calc_mtx());
}

/* 020B882C */
void control3(bmdhand_class* i_this) {

    hand_s* hand_i = &i_this->m324[0];
    for (int i = 0; i < 20; i++, hand_i++) {
        if (i < 10) {
            hand_i->m18 = 10.5f;
        } else {
            hand_i->m18 = (15.0f - (f32)(int)(i - 10)) * 0.7f;
        }
    }
}

// HD: GHS inlines the control helpers into daBmdhand_Execute.

static inline void control1(bmdhand_class* i_this) {
    fopAc_ac_c* actor = i_this;
    i_this->m324[0].m00.copy(actor->current.pos);
    hand_s* hand_i = &i_this->m324[1];
    cMtx_YrotS(calc_mtx(), actor->current.angle.y);
    gabi::Local<cXyz> local_94;
    gabi::Local<cXyz> cStack_a0;
    gabi::Local<cXyz> local_ac;
    gabi::Local<cXyz> sum;
    local_94->x = 0.0f;
    local_94->y = i_this->m314;
    local_94->z = i_this->m318;
    MtxPosition(local_94, local_ac);
    cLib_addCalc2(&i_this->m314, -10.0f, 1.0f, 0.1f);
    cLib_addCalc2(&i_this->m318, 50.0f, 1.0f, 1.0f);
    local_94->z = i_this->m310;
    f32 dVar9 = i_this->m31C;
    for (int i = 1; i < 0x13; i++, ++hand_i) {
        s16 t = i_this->m2B8;
        f32 c4x = cM_ssin(t * 0x44C + i * 4000) * dVar9;
        f32 c4z = cM_scos(t * 800 + i * 4000) * dVar9;
        f32 fVar1 = (i < 15) ? 1.0f : gabi::fnmsubs((f32)(i - 15), 0.2f, 1.0f);
        f32 dVar8 =
            gabi::fmadds(c4x, fVar1, gabi::fmadds(local_ac->x, fVar1, hand_i->m00.x - hand_i[-1].m00.x));
        f32 dVar7 =
            gabi::fmadds(c4z, fVar1, gabi::fmadds(local_ac->z, fVar1, hand_i->m00.z - hand_i[-1].m00.z));
        f32 dVar10 = (hand_i->m00.y - hand_i[-1].m00.y) + local_ac->y;
        s16 iVar3 = cM_atan2s(dVar8, dVar7);
        s16 iVar4 = -cM_atan2s(dVar10, std_sqrtf(gabi::fmadds(dVar8, dVar8, dVar7 * dVar7)));
        cMtx_YrotS(calc_mtx(), iVar3);
        cMtx_XrotM(calc_mtx(), iVar4);
        MtxPosition(local_94, cStack_a0);
        cXyz_pl(&hand_i[-1].m00, sum, cStack_a0);
        hand_i->m00.copy(*sum);
    }
}

/* the base of the tentacle (hand 0x13) and the joint angles of the hand model */
static inline void control_tail(bmdhand_class* i_this, cXyz* local_7c) {
    i_this->m2F0.copy(i_this->m324[0x13].m00);
    gabi::Local<cXyz> d;
    cXyz_mi(&i_this->m324[0x12].m00, d, &i_this->m324[0x13].m00);
    local_7c->x = d->x;
    local_7c->y = d->y;
    local_7c->z = d->z;
    i_this->m300 = -cM_atan2s(local_7c->y, local_7c->z);
    f32 y = local_7c->y, z = local_7c->z;
    i_this->m302 = cM_atan2s(local_7c->x, std_sqrtf(gabi::fmadds(y, y, z * z)));
    hand_mtx_set(i_this);
}

static inline void control2(bmdhand_class* i_this) {
    gabi::Local<cXyz> local_7c;
    gabi::Local<cXyz> cStack_88;
    gabi::Local<cXyz> sum;

    local_7c->x = 0.0f;
    local_7c->y = 0.0f;
    local_7c->z = i_this->m310;
    cLib_addCalc2(&i_this->m324[0x13].m00.x, i_this->m2D8.x, 1.0f, 50.0f * i_this->m30C);
    cLib_addCalc2(&i_this->m324[0x13].m00.y, i_this->m2D8.y, 1.0f, 50.0f * i_this->m30C);
    cLib_addCalc2(&i_this->m324[0x13].m00.z, i_this->m2D8.z, 1.0f, 50.0f * i_this->m30C);
    cLib_addCalc2(&i_this->m30C, 1.0f, 1.0f, 0.01f);
    hand_s* hand_i = &i_this->m324[0x12];
    for (int i = 0x12; i >= 1; i--, hand_i--) {
        f32 fVar1 = hand_i->m00.x - hand_i[1].m00.x;
        f32 dVar9 = (hand_i->m00.y - hand_i[1].m00.y) - 10.0f;
        f32 fVar2 = hand_i->m00.z - hand_i[1].m00.z;
        s16 iVar3 = cM_atan2s(fVar1, fVar2);
        s16 iVar4 = -cM_atan2s(dVar9, std_sqrtf(gabi::fmadds(fVar1, fVar1, fVar2 * fVar2)));
        cMtx_YrotS(calc_mtx(), iVar3);
        cMtx_XrotM(calc_mtx(), iVar4);
        if (i == 0x12) {
            local_7c->z = i_this->m310 + 70.0f;
        } else {
            local_7c->z = i_this->m310;
        }
        MtxPosition(local_7c, cStack_88);
        cXyz_pl(&hand_i[1].m00, sum, cStack_88);
        hand_i->m00.copy(*sum);
    }
    control_tail(i_this, local_7c);
}

/* 020B88C4 */
void cut_control(bmdhand_class* i_this) {

    fopAc_ac_c* actor = i_this;

    i_this->m324[0].m00.copy(actor->current.pos);
    hand_s* hand_i = &i_this->m324[1];
    gabi::Local<cXyz> local_e8;
    gabi::Local<cXyz> cStack_f4;
    gabi::Local<cXyz> local_100;
    cMtx_YrotS(calc_mtx(), actor->current.angle.y);
    local_e8->x = 0.0f;
    local_e8->y = i_this->m314;
    local_e8->z = i_this->m318;
    MtxPosition(local_e8, local_100);
    cLib_addCalc2(&i_this->m314, -3.0f, 1.0f, 0.1f);
    cLib_addCalc2(&i_this->m318, 30.0f, 1.0f, 0.2f);
    cLib_addCalc0(&i_this->m31C, 1.0f, 1.0f);
    local_e8->z = i_this->m310;
    const f32 dVar9 = 0.5f;
    gabi::Local<cXyz> sum;
    for (int i = 1; i < 20; i++, hand_i++) {
        s16 t = i_this->m2B8;
        f32 m31C = i_this->m31C;
        f32 c124x = cM_ssin(t * 0xDAC + i * 4000);
        f32 c124y = cM_scos(t * 4000 + i * 4000);
        f32 c124z = cM_scos(t * 0xED8 + i * 4000);
        f32 factor = gabi::fnmsubs((f32)i, 0.03763158f, 1.0f);
        f32 fVar_x =
            hand_i->m0C.x +
            gabi::fmadds(c124x, m31C, gabi::fmadds(local_100->x, factor, hand_i->m00.x - hand_i[-1].m00.x));
        f32 fVar_y = hand_i->m0C.y + gabi::fmadds(c124y, m31C, hand_i->m00.y + local_100->y);
        f32 lim = 5.0f + boss->m328;
        if (fVar_y < lim) {
            fVar_y = lim;
        }
        f32 delta_y = fVar_y - hand_i[-1].m00.y;
        f32 fVar_z =
            hand_i->m0C.z + ((hand_i->m00.z - hand_i[-1].m00.z + local_100->z * factor) + c124z * m31C);
        s16 iVar4 = cM_atan2s(fVar_x, fVar_z);
        s16 iVar5 = -cM_atan2s(delta_y, std_sqrtf(gabi::fmadds(fVar_x, fVar_x, fVar_z * fVar_z)));
        cMtx_YrotS(calc_mtx(), iVar4);
        cMtx_XrotM(calc_mtx(), iVar5);
        MtxPosition(local_e8, cStack_f4);
        hand_i->m0C.copy(hand_i->m00);
        cXyz_pl(&hand_i[-1].m00, sum, cStack_f4);
        hand_i->m00.x = sum->x;
        hand_i->m00.y = sum->y;
        hand_i->m00.z = sum->z;
        hand_i->m0C.x = (hand_i->m00.x - hand_i->m0C.x) * dVar9;
        hand_i->m0C.y = (hand_i->m00.y - hand_i->m0C.y) * dVar9;
        hand_i->m0C.z = (hand_i->m00.z - hand_i->m0C.z) * dVar9;
        if ((i == 0x13) && (i_this->m2CA != 0)) {
            dComIfGp_particle_setSimple(ID_AK_SN_O_BKMTENTACLEBLOOD00, &hand_i->m00, 0xFF);
        }
    }
}

/* 020B8C74 */
void cut_control3(bmdhand_class* i_this) {

    hand_s* hand_i = &i_this->m324[0];
    for (int i = 0; i < 20; i++, hand_i++) {
        if (i < 10) {
            hand_i->m18 = 10.5f;
        } else {
            hand_i->m18 = (15.0f - (f32)(int)(i - 10)) * 0.7f;
        }
    }
    cLib_addCalc2(&i_this->m2F0.y, i_this->m2CC.y, 1.0f, 10.0f);
    hand_mtx_set(i_this);
}

static inline void start_control1(bmdhand_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_94;
    gabi::Local<cXyz> cStack_a0;
    gabi::Local<cXyz> local_ac;
    gabi::Local<cXyz> sum;

    i_this->m324[0].m00.copy(actor->current.pos);
    hand_s* hand_i = &i_this->m324[1];
    cMtx_YrotS(calc_mtx(), actor->current.angle.y);
    local_94->x = 0.0f;
    local_94->y = i_this->m314;
    local_94->z = i_this->m318;
    MtxPosition(local_94, local_ac);
    cLib_addCalc2(&i_this->m314, 0.0f, 1.0f, 0.1f);
    cLib_addCalc2(&i_this->m318, 5.0f, 1.0f, 1.0f);
    local_94->z = i_this->m310;
    f32 dVar9 = i_this->m31C;
    for (int i = 1; i < 0x13; i++, hand_i++) {
        s16 t = i_this->m2B8;
        f32 fVar1 = gabi::fmadds(cM_ssin(t * 0x640 + i * 3000), dVar9,
                                 (hand_i->m00.x - hand_i[-1].m00.x) + local_ac->x);
        f32 fVar2 = gabi::fmadds(cM_scos(t * 0x578 + i * 0xDAC), dVar9,
                                 (hand_i->m00.z - hand_i[-1].m00.z) + local_ac->z);
        f32 dVar10 = gabi::fmadds(cM_ssin(t * 0x6A4 + i * 4000), dVar9,
                                  (hand_i->m00.y - hand_i[-1].m00.y) + local_ac->y);
        s16 iVar3 = cM_atan2s(fVar1, fVar2);
        s16 iVar4 = -cM_atan2s(dVar10, std_sqrtf(gabi::fmadds(fVar1, fVar1, fVar2 * fVar2)));
        cMtx_YrotS(calc_mtx(), iVar3);
        cMtx_XrotM(calc_mtx(), iVar4);
        MtxPosition(local_94, cStack_a0);
        cXyz_pl(&hand_i[-1].m00, sum, cStack_a0);
        hand_i->m00.copy(*sum);
    }
}

static inline void start_control2(bmdhand_class* i_this) {
    gabi::Local<cXyz> local_7c;
    gabi::Local<cXyz> cStack_88;
    gabi::Local<cXyz> sum;

    local_7c->x = 0.0f;
    local_7c->y = 0.0f;
    local_7c->z = i_this->m310;
    i_this->m324[0x13].m00.copy(i_this->m2D8);
    hand_s* hand_i = &i_this->m324[0x12];
    for (int i = 0x12; i >= 1; i--, hand_i--) {
        f32 fVar1 = hand_i->m00.x - hand_i[1].m00.x;
        f32 dVar9 = (hand_i->m00.y - hand_i[1].m00.y) - 5.0f;
        f32 fVar2 = hand_i->m00.z - hand_i[1].m00.z;
        s16 iVar3 = cM_atan2s(fVar1, fVar2);
        s16 iVar4 = -cM_atan2s(dVar9, std_sqrtf(gabi::fmadds(fVar1, fVar1, fVar2 * fVar2)));
        cMtx_YrotS(calc_mtx(), iVar3);
        cMtx_XrotM(calc_mtx(), iVar4);
        MtxPosition(local_7c, cStack_88);
        cXyz_pl(&hand_i[1].m00, sum, cStack_88);
        hand_i->m00.copy(*sum);
    }
    control_tail(i_this, local_7c);
}

/* the ceiling above the tip (dBgS_LinChk up 2500) */
static inline void hand_ceiling_check(bmdhand_class* i_this, BmdhandLineCheck* linChk, cXyz* start,
                                      cXyz* end) {
    start->x = i_this->m2D8.x;
    start->y = i_this->m2D8.y;
    start->z = i_this->m2D8.z;
    end->x = i_this->m2D8.x;
    end->y = i_this->m2D8.y + 2500.0f;
    end->z = i_this->m2D8.z;
    dBgS_LinChk_Set(linChk, start, end, i_this);
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        i_this->m2CC.x = linChk->mCross.x;
        i_this->m2CC.y = linChk->mCross.y;
        i_this->m2CC.z = linChk->mCross.z;
        i_this->m2CC.y = i_this->m2CC.y + l_HIO.m08;
    }
}

static inline void hand_calc(bmdhand_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_b8;
    gabi::Local<cXyz> local_c4;

    local_b8->y = 0.0f;
    local_b8->x = 0.0f;
    if ((i_this->m2B8 & 0xF) == 0) {
        gabi::Local<BmdhandLineCheck> linChk;
        Bmdhand_constructLineCheck(linChk);
        gabi::Local<cXyz> local_d0;
        gabi::Local<cXyz> local_dc;
        hand_ceiling_check(i_this, linChk, local_d0, local_dc);
        Bmdhand_destroyLineCheck(linChk);
    }
    switch (i_this->m2BC) {
    case 0: {
        cMtx_YrotS(calc_mtx(), actor->current.angle.y);
        if ((fopAcM_GetParam(actor) & 1) == 0) {
            local_b8->z = 250.0f;
        } else {
            local_b8->z = 350.0f;
        }
        MtxPosition(local_b8, local_c4);
        gabi::Local<cXyz> sum;
        cXyz_pl(&actor->current.pos, sum, local_c4);
        i_this->m2D8.copy(*sum);
        i_this->m2D8.y = i_this->m2CC.y;
        i_this->m2E4.x = i_this->m2D8.x;
        i_this->m2E4.y = i_this->m2D8.y;
        i_this->m2E4.z = i_this->m2D8.z;
        if (i_this->m30C > 0.9f) {
            if (boss->m331 < 0x14) {
                boss->m331 = boss->m331 + 1;
            }
            i_this->m2BC = 1;
            i_this->m2C0[0] = (s16)gabi::ftoi(cM_rndF(30.0f) + 10.0f);
            mDoAud_seStart(JA_SE_CM_BKM_VINE_SET, &i_this->m2D8, 0,
                           dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        }
        break;
    }
    case 1: {
        cMtx_YrotS(calc_mtx(), actor->current.angle.y);
        if ((fopAcM_GetParam(actor) & 1) == 0) {
            local_b8->z = 250.0f;
        } else {
            local_b8->z = 350.0f;
        }
        MtxPosition(local_b8, local_c4);
        i_this->m2E4.x = actor->current.pos.x + local_c4->x;
        i_this->m2E4.z = actor->current.pos.z + local_c4->z;
        gabi::Local<cXyz> d;
        cXyz_mi(&i_this->m2E4, d, &i_this->m2D8);
        f32 dx = d->x, dy = d->y, dz = d->z;
        local_c4->x = dx;
        local_c4->y = dy;
        local_c4->z = dz;
        if (std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) > 100.0f) {
            if (i_this->m2C0[0] == 0) {
                i_this->m2BC = 2;
                i_this->m2C0[0] = 0x28;
                i_this->m308 = 0.0f;
            }
        } else {
            i_this->m2C0[0] = (s16)gabi::ftoi(cM_rndF(30.0f));
        }
        break;
    }
    case 2: {
        f32 fVar7;
        if (i_this->m2C0[0] <= 0x14) {
            if (i_this->m2C0[0] == 0x14) {
                i_this->m308 = 0.0f;
            }
            i_this->m2E4.y = i_this->m2CC.y;
            fVar7 = 1.0f;
        } else {
            i_this->m2E4.y = i_this->m2CC.y - 150.0f;
            fVar7 = 0.1f;
        }
        cLib_addCalc2(&i_this->m2D8.x, i_this->m2E4.x, fVar7, 30.0f * i_this->m308);
        cLib_addCalc2(&i_this->m2D8.z, i_this->m2E4.z, fVar7, 30.0f * i_this->m308);
        cLib_addCalc2(&i_this->m2D8.y, i_this->m2E4.y, fVar7, 30.0f * i_this->m308);
        cLib_addCalc2(&i_this->m308, 1.0f, 1.0f, 0.1f);
        if (i_this->m2C0[0] == 0) {
            i_this->m2BC = 1;
            i_this->m2C0[0] = (s16)gabi::ftoi(cM_rndF(30.0f));
        }
        break;
    }
    }
}

static inline void start_hand_calc(bmdhand_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_a8;
    gabi::Local<cXyz> cStack_b4;

    local_a8->y = 0.0f;
    local_a8->x = 0.0f;
    gabi::Local<BmdhandLineCheck> linChk;
    Bmdhand_constructLineCheck(linChk);
    gabi::Local<cXyz> local_c0;
    gabi::Local<cXyz> local_cc;
    hand_ceiling_check(i_this, linChk, local_c0, local_cc);
    switch (i_this->m2BC) {
    case 0: {
        cMtx_YrotS(calc_mtx(), actor->current.angle.y);
        f32 fVar1 = i_this->m2E4.y - i_this->m2D8.y;
        fVar1 *= 0.1f;
        fVar1 *= 2.0f;
        s16 t = i_this->m2B8;
        local_a8->x = cM_ssin(t * 0x5DC) * fVar1;
        local_a8->z = cM_scos(t * 0x4B0) * fVar1;
        local_a8->y = cM_ssin(t * 500) * fVar1;
        if ((fopAcM_GetParam(actor) & 1) == 0) {
            local_a8->z = 250.0f;
        } else {
            local_a8->z = 350.0f;
        }
        MtxPosition(local_a8, cStack_b4);
        gabi::Local<cXyz> sum;
        cXyz_pl(&actor->current.pos, sum, cStack_b4);
        i_this->m2E4.copy(*sum);
        i_this->m2E4.y = i_this->m2CC.y;
        cLib_addCalc2(&i_this->m2D8.x, i_this->m2E4.x, 1.0f, 50.0f * i_this->m308);
        cLib_addCalc2(&i_this->m2D8.z, i_this->m2E4.z, 1.0f, 50.0f * i_this->m308);
        cLib_addCalc2(&i_this->m2D8.y, i_this->m2E4.y, 1.0f, 10.0f * i_this->m308);
        cLib_addCalc2(&i_this->m308, 1.0f, 1.0f, 0.01f);
    }
    }
    Bmdhand_destroyLineCheck(linChk);
}

static inline void hand_set_anm(bmdhand_class* i_this, const char* arcname, s32 idx) {
    J3DAnmTransform* pAnimRes = (J3DAnmTransform*)dComIfG_getObjectRes(arcname, idx);
    i_this->mpMorf->setAnm(pAnimRes, 0, 5.0f, 1.0f, 0.0f, -1.0f, nullptr);
}
static inline void hand_close(bmdhand_class* i_this) {
    hand_set_anm(i_this, hand_close_arc.name, dRes_INDEX_BMDHAND_BCK_FOOK_TOJIRU_e);
}
static inline void hand_open(bmdhand_class* i_this) {
    hand_set_anm(i_this, hand_open_arc.name, dRes_INDEX_BMDHAND_BCK_FOOK_HIRAKU_e);
}

static inline void hand_move(bmdhand_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_40;
    gabi::Local<cXyz> local_4c;
    gabi::Local<cXyz> local_58;

    hand_s* hand_i = i_this->m324;
    if (boss != nullptr) {
        u32 prm = fopAcM_GetParam(actor);
        u32 jnt = prm & 0x1F;
        actor->current.angle.y = (s16)(jnt * -0xCCC + -13000 + boss->shape_angle.y);
        /* HD: boss_joint_d is a bounds-checked array */
        if (!(jnt < 20))
            JUT_ASSERT_fail("d_a_bmdhand.cpp", 0x3AA, "boss_joint_d.size() > static_cast<uint>(prm & 0x1F)");
        if (jnt < 20) {
            J3DModel* bmodel = boss->mpBodyMorf->getModel();
            PSMTXCopy(Bmdhand_getAnmMtx(bmodel, boss_joint_d.values[jnt]), calc_mtx());
        }
        local_40->x = 0.0f;
        local_40->y = 0.0f;
        local_40->z = boss_joint_xad.values[prm & 3];
        MtxPosition(local_40, &actor->current.pos);
        if ((i_this->m2BA != Bmdhand_BossDown) && (boss->m332 == 3)) {
            i_this->m2BA = Bmdhand_BossDown;
            i_this->m314 = 3.0f;
            i_this->m318 = 40.0f;
            i_this->m31C = cM_rndF(20.0f) + 30.0f;
            i_this->m2C0[1] = (s16)gabi::ftoi(cM_rndF(30.0f) + 50.0f);
        }
        switch ((u16)(s16)i_this->m2BA) {
        case Bmdhand_Attached:
            cLib_addCalc2(&i_this->m310, 30.0f, 0.1f, 1.0f);
            cLib_addCalc2(&i_this->m31C, 70.0f, 0.1f, 0.5f);
            cLib_addCalc2(&i_this->m320, 1.0f, 1.0f, 0.01f);
            hand_calc(i_this);
            control1(i_this);
            control2(i_this);
            control3(i_this);
            actor->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
            if ((i_this->m2BC > 0) && (i_this->m5CC.ChkTgHit() || (i_this->m6F8.ChkTgHit()))) {
                i_this->m2BA = Bmdhand_Cut;
                fopAcM_monsSeStart(actor, JA_SE_CV_BKM_CUT_VINE_1, 0);
                if (gabi::ea(&actor->eyePos) != 0)
                    mDoAud_seStart(JA_SE_CM_BKM_CUT_VINE, &actor->eyePos, 0,
                                   dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                local_58->x = 0.5f;
                local_58->y = 0.5f;
                local_58->z = 0.5f;
                dComIfGp_particle_set(ID_AK_JN_SIBOUBAKUEN, &actor->eyePos, nullptr, local_58, 0xFF);
                dComIfGp_particle_set(ID_AK_JN_SIBOUFLASH, &actor->eyePos, nullptr, local_58, 0xFF);
                if (boss->m331 > 0) {
                    boss->m331 = boss->m331 - 1;
                    boss->m312 = 3;
                }
                i_this->m2C0[0] = l_HIO.m0C;
                i_this->m314 = 3.0f;
                i_this->m318 = 40.0f;
                f32 r = cM_rndF(20.0f);
                i_this->m2C8 = 0x14;
                i_this->m31C = r + 30.0f;
            }
            break;
        case Bmdhand_Cut:
            i_this->m2C8 = 0x14;
            i_this->m2CA = 2;
            cLib_addCalc2(&i_this->m310, 20.0f, 0.1f, 0.5f);
            cut_control(i_this);
            cut_control3(i_this);
            if (i_this->m2C0[0] < 100) {
                cLib_addCalc2(&i_this->m320, 0.0f, 1.0f, 0.01f);
                if (i_this->m2C0[0] == 0) {
                    i_this->m2BA = Bmdhand_Attached;
                    i_this->m2BC = 0;
                    i_this->m30C = 0.0f;
                    i_this->m31C = 0.0f;
                }
            }
            break;
        case Bmdhand_BossDown:
            i_this->m2C8 = 0x14;
            if (i_this->m2C0[1] == 0) {
                cLib_addCalc2(&i_this->m310, 20.0f, 0.1f, 0.5f);
            }
            if ((boss->m302 == 2) && (boss->m308_0 < 100)) {
                cLib_addCalc0(&i_this->m320, 1.0f, 0.01f);
            }
            cut_control(i_this);
            cut_control3(i_this);
            if (boss->m332 == 4) {
                i_this->m2C0[0] = (s16)gabi::ftoi(cM_rndF(50.0f));
                i_this->m2BA = Bmdhand_RegrowWait;
                i_this->m2BC = 0;
            } else if (boss->m332 == 9) {
                i_this->m2BA = Bmdhand_Disappear;
            }
            break;
        case Bmdhand_Intro:
            i_this->m2C8 = 10; // HD: collision is disabled during the introduction.
            switch ((u16)(s16)i_this->m2BE) {
            case 0:
                if (boss->m332 == 5) {
                    i_this->m2BE = 1;
                    i_this->m2E4.y = 10000.0f;
                    i_this->m2D8.copy(actor->current.pos);
                    i_this->m30C = 1.0f;
                    i_this->m2C0[0] = (s16)gabi::ftoi(cM_rndF(50.0f));
                }
                break;
            case 1:
                if (i_this->m2C0[0] == 0) {
                    i_this->m2BE = 2;
                }
                break;
            case 2:
                if (i_this->m2E4.y - i_this->m2D8.y < 10.0f) {
                    boss->m331 = boss->m331 + 1;
                    i_this->m2BE = 3;
                }
                // fallthrough
            case 3:
                cLib_addCalc2(&i_this->m310, 30.0f, 0.1f, 0.5f);
                cLib_addCalc2(&i_this->m31C, 200.0f, 0.1f, 10.0f);
                if (i_this->m2E4.y - i_this->m2D8.y < 800.0f) {
                    cLib_addCalc2(&i_this->m320, 1.0f, 1.0f, 0.02f);
                }
                start_hand_calc(i_this);
                start_control1(i_this);
                start_control2(i_this);
                control3(i_this);
                break;
            }
            if (boss->m332 == 7) {
                i_this->m2BA = Bmdhand_Attached;
                i_this->m2BC = 0;
            }
            break;
        case Bmdhand_Disappear:
            i_this->m2C8 = 0x14;
            cLib_addCalc0(&i_this->m310, 0.1f, 0.2f);
            cLib_addCalc0(&i_this->m320, 1.0f, 0.01f);
            cut_control(i_this);
            cut_control3(i_this);
            break;
        case Bmdhand_RegrowWait:
            i_this->m2C8 = 0x46;
            if (i_this->m2C0[0] == 0) {
                i_this->m2BA = Bmdhand_Attached;
                i_this->m2BC = 0;
                i_this->m30C = cM_rndF(0.2f);
                i_this->m31C = 0.0f;
            }
            break;
        }
    }
    BmdhandLineData* line = Bmdhand_getLineData(i_this->mLineMat);
    cXyz* line_data = line->positions;
    u32 line_size = line->sizes;
    for (int i = 0; i < 20; i++, hand_i++, line_data++, line_size++) {
        line_data->copy(hand_i->m00);
        Bmdhand_setLineSize(line_size, (u8)gabi::ftoi(hand_i->m18));

        if (i == 6) { /* HD: 6 (GameCube 10) */
            actor->eyePos.copy(hand_i->m00);
            actor->attention_info.position.copy(hand_i->m00);
            i_this->m5CC.SetC(&actor->eyePos);
            dComIfG_Ccsp_Set(&i_this->m5CC);
        } else {
            u32 r0 = ((i_this->m2B8 & 3) * 4) + 3;
            if (r0 == (u32)i) {
                i_this->m6F8.SetC(&hand_i->m00);
                dComIfG_Ccsp_Set(&i_this->m6F8);
            }
        }
    }
    if (i_this->m2C8 != 0) {
        local_4c->x = 0.0f;
        local_4c->y = -20000.0f;
        local_4c->z = 0.0f;
        i_this->m5CC.SetC(local_4c);
        dComIfG_Ccsp_Set(&i_this->m5CC);
        i_this->m6F8.SetC(local_4c);
        dComIfG_Ccsp_Set(&i_this->m6F8);
        actor->attention_info.flags = 0;
    }
}

/* 020B8D4C */
static void* s_a_d_sub(void* param_1, void* param_2) {

    if (fopAcM_IsActor(param_1) && param_1 != nullptr &&
        static_cast<fopAc_ac_c*>(param_1)->mProcName == fpcNm_BMD_e) {
        return param_1;
    }
    return nullptr;
}

/* 020B8D9C */
static BOOL daBmdhand_Execute(bmdhand_class* i_this) {

    dComIfGp_get(); /* HD: an unused call */
    if (boss == nullptr) {
        boss = static_cast<BmdhandBossView*>(
            fpcM_Search(gabi::guest_fn(reinterpret_cast<const void*>(s_a_d_sub)), i_this));
    }
    i_this->m2B8 = i_this->m2B8 + 1;
    for (int i = 0; i < 4; i++) {
        if (i_this->m2C0[i] != 0) {
            i_this->m2C0[i] = i_this->m2C0[i] - 1;
        }
    }
    if (i_this->m2C8 != 0) {
        i_this->m2C8 = i_this->m2C8 - 1;
    }
    if (i_this->m2CA != 0) {
        i_this->m2CA = i_this->m2CA - 1;
    }
    i_this->m2FC = i_this->m2F0.y;
    hand_move(i_this);
    f32 dVar4 = i_this->m2CC.y - 1.0f;
    if ((i_this->m2F0.y < dVar4) && !(i_this->m2FC < dVar4)) {
        hand_open(i_this);
    }
    if ((i_this->m2F0.y > dVar4) && !(i_this->m2FC > dVar4)) {
        i_this->m304 = 3;
    }
    if (i_this->m304 != 0) {
        i_this->m304 = i_this->m304 - 1;
        if (i_this->m304 == 0) {
            hand_close(i_this);
        }
    }
    i_this->mpMorf->play(nullptr, 0, 0);
    return TRUE;
}

/* 020BABA4 */
static BOOL daBmdhand_IsDelete(bmdhand_class* i_this) { return TRUE; }

/* 020BABAC */
static BOOL daBmdhand_Delete(bmdhand_class* i_this) {

    dComIfG_resDelete(&i_this->mPhase, "Bmdhand"); /* dComIfG_resDeleteDemo */
    if (i_this->m824 != 0) {
        s8 no = l_HIO.mNo;
        hio_set = 0;
        mDoHIO_deleteChild(no);
    }
    boss = nullptr;
    mDoAud_seDeleteObject(&i_this->m2D8);
    return TRUE;
}

// HD: useHeapInit also serves as the solid-heap callback.
/* 020BAC28 */
static BOOL useHeapInit(bmdhand_class* i_this) {

    const char* arc = "Bmdhand";
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(arc, dRes_INDEX_BMDHAND_BMD_BKM_FOOK_e);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(arc, dRes_INDEX_BMDHAND_BCK_FOOK_HIRAKU_e);
    mDoExt_McaMorf* morf =
        mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 0 /* J3DFrameCtrl::EMode_NONE */,
                               1.0f, 0, -1, 1, nullptr, 0, 0x11020203);
    i_this->mpMorf = morf;
    /* HD: the morf is checked before its model */
    if (morf == nullptr || morf->getModel() == nullptr) {
        return FALSE;
    }
    void* pBti = dComIfG_getObjectRes(arc, dRes_INDEX_BMDHAND_BTI_SYOKUSYU_UE_e);
    if (!i_this->mLineMat.init(1, 20, static_cast<ResTIMG*>(pBti), 1)) {
        return FALSE;
    }
    return TRUE;
}

/* 020BAD4C */
static cPhs_State daBmdhand_Create(fopAc_ac_c* a_this) {

    fopAc_ac_c* actor = a_this;
    bmdhand_class* i_this = (bmdhand_class*)a_this;
    /* fopAcM_ct(actor, bmdhand_class): inline member constructors */
    if (!fopAcM_CheckCondition(actor, fopAcCnd_INIT_e)) {
        if (actor != nullptr) {
            fopAc_ac_c_ct(actor);
            actor->__vtbl = BmdhandVtable_Actor;
            Bmdhand_lineCtor(&i_this->mLineMat);                   /* mDoExt_3DlineMat1_c */
            Bmdhand_statusCtor(&i_this->mStts);                    /* cCcD_Stts */
            Bmdhand_globalStatusCtor(&i_this->mStts.__vtbl_gstts); /* dCcD_GStts */
            i_this->mStts.__vtbl = BmdhandVtable_Status;
            i_this->mStts.__vtbl_gstts = BmdhandVtable_GlobalStatus;
            Bmdhand_sphereCtor(&i_this->m5CC);
            Bmdhand_sphereCtor(&i_this->m6F8);
        }
        fopAcM_OnCondition(actor, fopAcCnd_INIT_e);
    }
    cPhs_State res = dComIfG_resLoad(&i_this->mPhase, "Bmdhand");
    if (res == cPhs_ERROR_e) {
        return cPhs_ERROR_e;
    } else if (res != cPhs_COMPLEATE_e) {
        return res;
    }
    if (!fopAcM_entrySolidHeap(actor, gabi::guest_fn(reinterpret_cast<const void*>(useHeapInit)), 0x3040)) {
        return cPhs_ERROR_e;
    }
    if (hio_set == 0) {
        hio_set = 1;
        i_this->m824 = 1;
        l_HIO.mNo = mDoHIO_createChild("森ボス触手", &l_HIO);
    }
    actor->health = 2;
    i_this->m2B8 = (s16)gabi::ftoi(cM_rndF(10000.0f));
    boss = nullptr;
    i_this->mStts.Init(0xFF, 0xFF, actor);
    i_this->m5CC.Set(&cc_sph_src);
    i_this->m5CC.SetStts(&i_this->mStts);
    i_this->m5CC.SetR(110.0f);
    i_this->m6F8.Set(&cc_sph_src);
    i_this->m6F8.SetStts(&i_this->mStts);
    i_this->m5CC.SetR(90.0f);
    if (!(dComIfGs_isStageBossDemo()) &&
        (*reinterpret_cast<const be<u8>*>(dComIfGp_getStartStageName()) != 'X')) {
        i_this->m2BA = Bmdhand_Intro;
    }
    return cPhs_COMPLEATE_e;
}

// HD: compiler-generated functions; the HIO constructor allocates when passed null.
/* 020BAF50 */
static daBmdhand_HIO_c* daBmdhand_HIO_c_ct(daBmdhand_HIO_c* h) {

    if (h == nullptr) {
        h = (daBmdhand_HIO_c*)operator_new(0x10);
        if (h == nullptr)
            return h;
    }
    h->mNo = -1;
    h->m08 = -20.0f;
    h->m0C = 1000;
    h->__vtbl = BmdhandVtable_Hio;
    return h;
}

/* 020BAFAC */
static void __sinit_d_a_bmdhand_cpp() {

    __sinit_header_statics();
    daBmdhand_HIO_c_ct(&l_HIO);
}

/* 020BB04C */
static void trivial_dt(void* p, s32 flags) {

    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 020BB060 */
static void bmdhand_class_dt(bmdhand_class* i_this, s32 flags) {

    if (i_this != nullptr) {
        Bmdhand_sphereDtor(&i_this->m6F8, 2); /* dCcD_Sph::~dCcD_Sph */
        Bmdhand_sphereDtor(&i_this->m5CC, 2);
        Bmdhand_statusDtor(&i_this->mStts, 2);  /* dCcD_Stts::~dCcD_Stts */
        Bmdhand_lineDtor(&i_this->mLineMat, 2); /* ~mDoExt_3DlineMat1_c (matcher: draw) */
        Bmdhand_actorDtor(i_this, 0);           /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 020BB0E4 */
static void empty_virtual(void* p) {}

// clang-format off

/* WWHD glue
data boss 10462504
data hio_set 10191C14
data l_HIO 10462514
data boss_joint_d 10191C48
data boss_joint_xad 10191C18
data cc_sph_src 10191C98
data line_color 10191C10
data hand_close_arc 10009EB8
data hand_open_arc 10009EC0
str 10009EC8 "Bmdhand" in 020BAC28
str 1000A038 "Bmdhand" in 020BABAC
str 1000A048 "Bmdhand" in 020BAD4C
str 1000A050 "\u68ee\u30dc\u30b9\u89e6\u624b" renamed
str 10009F6C "d_a_bmdhand.cpp" renamed
str 10009F7C "boss_joint_d.size() > static_cast<uint>(prm & 0x1F)" renamed
ext 025ED1BC void Bmdhand_updateLine(mDoExt_3DlineMat1_c*, s32, const GXColor*, dKy_tevstr_c*)
ext 025E1AA4 void Bmdhand_monsSeStart(u32, cXyz*, u32, u32, s32)
ext 025166F0 void Bmdhand_sphereCtor(dCcD_Sph*)
ext 025EB82C void Bmdhand_lineCtor(mDoExt_3DlineMat1_c*)
ext 0200BD2C void Bmdhand_statusCtor(dCcD_Stts*)
ext 02515DA0 void Bmdhand_globalStatusCtor(void*)
ext 02515AE8 void Bmdhand_sphereDtor(dCcD_Sph*, s32)
ext 02515860 void Bmdhand_statusDtor(dCcD_Stts*, s32)
ext 025EB8B8 void Bmdhand_lineDtor(mDoExt_3DlineMat1_c*, s32)
ext 025D50BC void Bmdhand_actorDtor(fopAc_ac_c*, s32)
*/
// clang-format on

/**
 * d_a_bdk_act.cpp (WWHD)
 * Boss - Helmaroc King (battle): the action functions called out of line by move()
 * (fly_attack, wind_attack, kamen_demo, start, end, t_fly, t_lastattack, t_down).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdk.h"

using gabi::fadds_ppc;
using gabi::fsubs_ppc;

#define foot_eff_pos(i) (*gabi::at<cXyz>(0x104618D0 + 0xC * (i)))
static inline void dBgS_Acch_SetGroundUpY_bdk(dBgS_Acch* a, f32 y) { gabi::call(0x024F12A8, a, y); }
static inline void bdk_vcopy(cXyz* d, const cXyz* s) {
    gabi::store<u32>(gabi::ea(d) + 0, gabi::load<u32>(gabi::ea(s) + 0));
    gabi::store<u32>(gabi::ea(d) + 4, gabi::load<u32>(gabi::ea(s) + 4));
    gabi::store<u32>(gabi::ea(d) + 8, gabi::load<u32>(gabi::ea(s) + 8));
}

/* 0206A610 */
void fly_attack(bdk_class* i_this) {
    WWHD_FUNC(0x0206A610, void, i_this);
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> offset;
    gabi::Local<cXyz> offset2;
    gabi::Local<cXyz> dest;
    gabi::Local<cXyz> sum;
    u8 bVar1 = 0;
    i_this->mF10 = 1;
    switch (i_this->mState) {
    case 0:
        i_this->mState = 1;
        i_this->m2EC[0] = 0x1E;
        i_this->m2E0 = 0.0f;
        i_this->m2EC[1] = 200;
        i_this->mF18 = cM_rndFX(100.0f);
        /* fall through */
    case 1: {
        bVar1 = 1;
        f32 px = player->current.pos.x;
        i_this->m2CC.x = px;
        bdk_fcopy(i_this->m2CC.y, player->current.pos.y);
        i_this->m2CC.x = fadds_ppc(px, i_this->mF18);
        i_this->m2CC.z = fadds_ppc(player->current.pos.z, i_this->mF18);
        if (i_this->m2EC[0] == 0) {
            i_this->m2DC = 400.0f;
        } else {
            i_this->m2CC.x = 3273.0f;
            i_this->m2CC.z = -3108.0f;
            i_this->m2DC = 1000.0f;
        }
        bdk_fcopy(i_this->m2E4, l_HIO.m024);
        i_this->m2E8 = 1.0f;
        cXyz_mi(&i_this->m2CC, offset, &actor->current.pos);
        if (std_sqrtf(PSVECSquareMag(offset)) < fadds_ppc(REG8_F(11), 1000.0f) || i_this->m2EC[1] == 0) {
            i_this->mState = 2;
            cMtx_YrotS(calc_mtx(), actor->current.angle.y);
            offset2->x = 0.0f;
            offset2->y = 500.0f;
            offset2->z = 4000.0f;
            MtxPosition(offset2, dest);
            cXyz_pl(&actor->current.pos, sum, dest);
            bdk_vcopy(&i_this->m2CC, sum);
            i_this->m2E0 = 0.0f;
            i_this->m2EC[0] = 0x28;
            eff_hane_set(i_this, &i_this->m1168, 10, 0);
        } else if (i_this->m2EC[0] == 1) {
            anm_init(i_this, 0x2F /* BCK_KAKKU1 */, 30.0f, 2, 1.0f, -1, 0);
        }
        break;
    }
    case 2:
        bVar1 = 1;
        offset2->x = 0.0f;
        offset2->y = 0.0f;
        offset2->z = fadds_ppc(REG0_F(11), -1000.0f);
        cMtx_YrotS(calc_mtx(), actor->current.angle.y);
        MtxPosition(offset2, dest);
        PSVECAdd(dest, &actor->current.pos, dest);
        wind_set(i_this, dest);
        if (i_this->m2EC[0] == 0) {
            i_this->mAction = 0; /* ACTION_FLY */
            i_this->m2CA = -1;
            i_this->mState = 0;
        }
        break;
    case 10: {
        anm_init(i_this, 0x24 /* BCK_DAMAGE4 */, 2.0f, 0, 1.0f, -1, 1);
        i_this->mState = 0xB;
        actor->speed.y = 0.0f;
        i_this->m2EC[0] = (s16)(REG0_S(5) + 0x28);
        i_this->m2EC[1] = (s16)(REG0_S(6) + 0x50);
        bdk_StartShock(5);
        bdk_seStart(actor, 0x5877 /* JA_SE_CM_DK_CRASH_WALL */, 0);
        bdk_monsSeStart_e(actor, 0x4869 /* JA_SE_CV_DK_KABE_DAMAGE */, 0);
        JPABaseEmitter* emitter;
        if (i_this->m6078[3] == 0) {
            i_this->m6078[3] = 100;
            s32 room = fopAcM_GetRoomNo(actor);
            emitter = bdk_particle_setToon(0xA135 /* ID_IT_ST_DK_BUTSUKARI_SMOKE00 */, &actor->current.pos, &actor->shape_angle, nullptr, 0xB9,
                                           &i_this->m6080[3], (s8)room);
            if (emitter != nullptr) {
                bdk_setGlobalRTMatrix(emitter, bdk_morfAnmMtx(i_this, 0x1C /* DK_JNT_J_DK_MABUTA_e */));
            }
        }
        s32 room = fopAcM_GetRoomNo(actor);
        emitter = dComIfGp_particle_set(0x8137 /* ID_IT_SN_DK_DMG_HANE_M00 */, &actor->current.pos, &actor->shape_angle, nullptr, 0xFF, nullptr,
                                        (s8)room, bdk_colorK0(actor), bdk_colorK0(actor));
        if (emitter != nullptr) {
            bdk_setGlobalRTMatrix(emitter, bdk_morfAnmMtx(i_this, 0x16 /* DK_JNT_J_DK_MUNE2_e */));
        }
    }
        /* fall through */
    case 0xB:
        actor->current.pos.y = fadds_ppc(actor->current.pos.y, actor->speed.y);
        if (i_this->mpMorf->isStop()) {
            anm_init(i_this, 0x35 /* BCK_PIKUPIKU1 */, 5.0f, 2, 1.0f, -1, 0);
        }
        if (i_this->m2EC[0] == 0) {
            actor->speed.y = fsubs_ppc(actor->speed.y, fadds_ppc(REG0_F(7), 0.5f));
            actor->current.angle.z = (s16)(actor->current.angle.z + (REG0_S(7) + 0x50));
            if (i_this->m2EC[1] == 0) {
                i_this->mState = 0xC;
                i_this->m2D8 = (s16)(actor->current.angle.y + 0x8000);
                i_this->m2EC[0] = (s16)(REG0_S(8) + 0x28);
                anm_init(i_this, 0x2B /* BCK_FLY2 */, 5.0f, 2, 2.0f, 0xC /* BAS_FLY2 */, 0);
            }
        }
        break;
    case 0xC:
        actor->speed.y = fadds_ppc(actor->speed.y, fadds_ppc(REG0_F(8), 2.0f));
        actor->current.pos.y = fadds_ppc(actor->current.pos.y, actor->speed.y);
        cLib_addCalcAngleS2(&actor->current.angle.y, i_this->m2D8, 8, 0x400);
        if (i_this->m2EC[0] == 0) {
            i_this->mAction = 0; /* ACTION_FLY */
            i_this->m2CA = -1;
            i_this->mState = 0;
            actor->speedF = 0.0f;
        }
        break;
    default:
        break;
    }
    if (i_this->mAction == 7 /* ACTION_FLY_ATTACK */ && i_this->mState < 10) {
        i_this->m2584 = 2;
        pos_move(i_this);
        i_this->mAcchCir.SetWall(fadds_ppc(REG0_F(17), 500.0f), 500.0f);
        dBgS_Acch_SetGroundUpY_bdk(&i_this->mAcch, REG0_F(18));
        if (i_this->mAcch.ChkGroundHit() && i_this->m2619 == 0) {
            for (s32 i = 0; i <= 1; i++) {
                bdk_vcopy(&foot_eff_pos(i), &i_this->m1174[i]);
                i_this->mp6214[0 + i] = dComIfGp_particle_set(0x812A /* ID_IT_SN_DK_TSUME_HIBANA_A00 */, &foot_eff_pos(i), &actor->shape_angle);
                i_this->mp6214[2 + i] = dComIfGp_particle_set(0x812B /* ID_IT_SN_DK_TSUME_HIBANA_B00 */, &foot_eff_pos(i), &actor->shape_angle);
            }
            i_this->m2619 = i_this->m2619 + 1;
        }
        if (i_this->mAcch.ChkWallHit()) {
            i_this->mState = 10;
        }
    }
    if (bVar1) {
        bdk_seStart_a(actor, 0x506F /* JA_SE_CM_DK_GLIDING */, 0);
    }
}
VERIFY(0x0206A610, fly_attack);

/* 0206AE64 */
void wind_attack(bdk_class* i_this) {
    WWHD_FUNC(0x0206AE64, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* actor = i_this;
    switch (i_this->mState) {
    case 0:
        anm_init(i_this, 0x3B /* BCK_TOBITATU1 */, 5.0f, 0, 1.0f, 0x16 /* BAS_TOBITATU1 */, 0);
        i_this->m2618 = 0x19;
        i_this->mState = i_this->mState + 1;
        i_this->m2CC.copy(actor->current.pos);
        i_this->m2E0 = 0.0f;
        /* fall through */
    case 1:
        if (morf_frame(i_this->mpMorf) == 23.0f) {
            i_this->mState = 2;
            i_this->m2CC.y = i_this->m2CC.y + 500.0f;
        }
        break;
    case 2:
        if (i_this->mpMorf->isStop()) {
            anm_init(i_this, 0x2B /* BCK_FLY2 */, 5.0f, 2, 2.0f, 0xC /* BAS_FLY2 */, 0);
            i_this->mState = 3;
            i_this->m2EC[0] = 200;
        }
        break;
    case 3: {
        wind_set(i_this, &actor->current.pos);
        if (!(i_this->m2C4 & 1)) {
            gabi::Local<cXyz> offset;
            f32 x = i_this->m1168.x;
            bdk_fcopy(offset->x, i_this->m1168.x);
            bdk_fcopy(offset->y, i_this->m1168.y);
            bdk_fcopy(offset->z, i_this->m1168.z);
            f32 r = cM_rndFX(300.0f);
            offset->x = fadds_ppc(x, r);
            r = cM_rndF(200.0f);
            offset->y = fsubs_ppc(offset->y, r);
            r = cM_rndFX(300.0f);
            offset->z = fadds_ppc(offset->z, r);
            eff_hane_set(i_this, offset, 1, 1);
        }
        cLib_addCalc2(&i_this->m2608, 1.0f, 1.0f, 0.05f);
        cLib_addCalc2(&i_this->m2E0, 1.0f, 1.0f, 0.05f);
        if (i_this->m2EC[0] == 0 || i_this->m2F8 != 0) {
            i_this->mAction = 2; /* ACTION_LANDING */
            i_this->mState = 3;
            i_this->m2CC.copy(player->current.pos);
            actor->speed.y = -20.0f;
            return;
        }
        break;
    }
    default:
        break;
    }

    cLib_addCalcAngleS2(&actor->current.angle.y, fopAcM_searchPlayerAngleY(actor), 4, 0x800);
    f32 t = cM_ssin((s32)i_this->m2C4 * 1500) * i_this->m2E0;
    cLib_addCalc2(&actor->current.pos.y, gabi::fmadds(t, 100.0f, i_this->m2CC.y), 0.1f, 60.0f);
    t = cM_ssin((s32)i_this->m2C4 * 700) * i_this->m2E0;
    cLib_addCalc2(&actor->current.pos.x, gabi::fmadds(t, 200.0f, i_this->m2CC.x), 0.1f, 40.0f);
    t = cM_ssin((s32)i_this->m2C4 * 500) * i_this->m2E0;
    cLib_addCalc2(&actor->current.pos.z, gabi::fmadds(t, 200.0f, i_this->m2CC.z), 0.1f, 40.0f);
}
VERIFY(0x0206AE64, wind_attack);

static inline f32 kamen_break_off_x(s32 i) { return gabi::load<f32>(0x10190668 + 4 * i); }
static inline f32 kamen_break_off_y(s32 i) { return gabi::load<f32>(0x10190678 + 4 * i); }
static inline f32 kamen_break_off_z(s32 i) { return gabi::load<f32>(0x10190688 + 4 * i); }
static inline f32 kamen_break_sd_x(s32 i) { return gabi::load<f32>(0x10190698 + 4 * i); }
static inline f32 kamen_break_sd_z(s32 i) { return gabi::load<f32>(0x101906A8 + 4 * i); }
static inline s8 kamen_break_time(s32 i) { return gabi::load<s8>(0x10190664 + i); }

/* 0206B22C */
void kamen_demo(bdk_class* i_this) {
    WWHD_FUNC(0x0206B22C, void, i_this);
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused dComIfGp_getPlayer(0) */
    i_this->m1134 = 0;
    switch (i_this->mState) {
    case 0:
        anm_init(i_this, 0x33 /* BCK_NUKENAI1 */, 0.0f, 0, 0.0f, -1, 0);
        i_this->mState = 1;
        i_this->m25A0 = 1;
        actor->actor_status |= 0x4000; /* fopAcStts_UNK4000_e */
        break;
    case 1: {
        i_this->m25A4 = 0;
        i_this->m25A6 = 0;
        i_this->m25C8 = 45.0f;
        gabi::Local<cXyz> pos;
        gabi::Local<cXyz> dist;
        for (s32 i = 0; i < 4; i++) {
            i_this->m90C[i] = 1;
            pos->x = 0.0f;
            pos->y = 0.0f;
            pos->z = 0.0f;
            PSMTXCopy(bdk_getAnmMtx((J3DModel_bdk*)morf_model(i_this->mpMorf), 0x18 /* DK_JNT_J_DK_ATAMA1_e */), calc_mtx());
            MtxPosition(pos, &i_this->m910[i]);
            i_this->m9A0[i].y = actor->shape_angle.y;
            i_this->m9A0[i].x = (s16)(REG0_S(6) + 0x3625);
            i_this->m9A0[i].z = 0;
            f32 zb = 110.0f - kamen_break_off_z(i);
            f32 yb = 55.0f - kamen_break_off_y(i);
            pos->x = -kamen_break_off_x(i);
            pos->y = gabi::fmadds(REG0_F(15), 0.01f, yb);
            pos->z = gabi::fmadds(REG0_F(16), 0.01f, zb);
            cMtx_YrotS(calc_mtx(), i_this->m9A0[i].y);
            cMtx_XrotM(calc_mtx(), i_this->m9A0[i].x);
            MtxPosition(pos, dist);
            PSVECAdd(&i_this->m910[i], dist, &i_this->m910[i]);
            i_this->m940[i].copy(i_this->m910[i]);
            cMtx_YrotS(calc_mtx(), i_this->m9A0[i].y);
            f32 t = fadds_ppc(REG0_F(7), 1.0f);
            pos->y = 0.0f;
            pos->x = gabi::fmuls_ppc(kamen_break_sd_x(i), t);
            pos->z = gabi::fmuls_ppc(kamen_break_sd_z(i), t);
            MtxPosition(pos, &i_this->m970[i]);
            i_this->m9E8[i] = kamen_break_time(i);
        }
        i_this->mState = 2;
        break;
    }
    case 2:
        if (i_this->m25A6 > (s32)(REG0_S(8) + 0x2D)) {
            i_this->m113A = 1;
            i_this->m1134 = 500;
        }
        if (i_this->m25A6 >= 0x6E && REG_S(8, 8) == 0) {
            i_this->m25A0 = 150;
            i_this->mAction = 5; /* ACTION_KUTI_ATTACK */
            anm_init(i_this, 0x33 /* BCK_NUKENAI1 */, 2.0f, 0, 1.0f, -1, 0);
            i_this->mState = 5;
            i_this->m2EC[0] = (s16)gabi::ftoi(fadds_ppc(cM_rndF(120.0f), 30.0f));
            mDoAud_bgmStart(0x80000110 /* JA_BGM_UNK_110 */);
        }
        break;
    default:
        break;
    }
}
VERIFY(0x0206B22C, kamen_demo);

/* 0206B67C (HD: the bk/boko search callbacks are not run; the start needs play+0x5BAC set; the
 * boss faces 0x7242 when it rises) */
void start(bdk_class* i_this) {
    WWHD_FUNC(0x0206B67C, void, i_this);
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    switch (i_this->mState) {
    case 0:
        if (!(player->current.pos.y < 9800.0f) &&
            !gabi::call_ptr<BOOL>(gabi::load<u32>(gabi::load<u32>(gabi::ea(player) + 0xB4) + 0x4C), player) /* checkPlayerFly */) {
            if (i_this->m2EC[0] == 0 && gabi::load<u16>(dComIfGp_ea() + 0x5BAC) != 0) {
                i_this->m25A4 = 0;
                i_this->m25A6 = 0;
                i_this->m25A0 = 10;
                i_this->mState = 1;
                fpcM_Search(0x02064CF4 /* kamome_delete_sub */, i_this);
                fpcM_Search(0x02064DF0 /* dk_delete_sub */, i_this);
                fpcM_Search(0x02064D48 /* kui_delete_sub */, i_this);
                fpcM_Search(0x0206505C /* sea_delete_sub */, i_this);
                fpcM_Search(0x02064E44 /* obj_delete_sub */, i_this);
                actor->actor_status |= 0x4000; /* fopAcStts_UNK4000_e */
            }
        } else {
            i_this->m2EC[0] = 0x14;
        }
        break;
    case 1:
        if (i_this->m25D8 != 0) {
            i_this->m25D8 = 0;
            i_this->mState = 2;
            i_this->m2F6 = (s16)(REG0_S(6) + 0x19);
            actor->current.pos.y = fadds_ppc(REG0_F(10), 8500.0f);
            actor->speed.y = fadds_ppc(REG0_F(12), 80.0f);
            anm_init(i_this, 0x39 /* BCK_S_DEMO1 */, 0.0f, 2, 1.0f, -1, 0);
            bdk_seStart(actor, 0x586B /* JA_SE_CM_DK_ROUND_UP */, 0);
            s32 room = fopAcM_GetRoomNo(actor);
            dComIfGp_particle_set(0x813C /* ID_IT_SN_DK_DEMO_HANE00 */, &actor->current.pos, &actor->shape_angle, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&i_this->m61B0, (s8)room, bdk_colorK0(actor), bdk_colorK0(actor));
            actor->current.angle.y = 0x7242;
        }
        break;
    case 2: {
        f32 sy = actor->speed.y;
        actor->current.pos.y = fadds_ppc(actor->current.pos.y, sy);
        actor->speed.y = fsubs_ppc(sy, fadds_ppc(REG0_F(13), 1.0f));
        actor->current.angle.y = actor->current.angle.y + 0x1300;
        if (i_this->m25D8 != 0) {
            i_this->mState = 3;
            i_this->m25D8 = 0;
            anm_init(i_this, 0x3A /* BCK_S_DEMO2 */, 5.0f, 0, 1.0f, 0x15 /* BAS_S_DEMO2 */, 0);
            i_this->m2EC[0] = 0x2D;
        }
        break;
    }
    case 3:
        if (i_this->m2EC[0] != 0) {
            f32 sy = actor->speed.y;
            actor->current.pos.y = fadds_ppc(actor->current.pos.y, sy);
            actor->speed.y = fsubs_ppc(sy, fadds_ppc(REG0_F(13), 1.0f));
        }
        if (i_this->m2EC[0] == (s16)(REG_S(8, 8) + 5)) {
            bdk_monsSeStart(actor, 0x4864 /* JA_SE_CV_DK_ENTER */, 0);
            dPa_followEcallBack_end(&i_this->m61B0);
            for (s32 i = 0; i < 2; i++) {
                s32 room = fopAcM_GetRoomNo(actor);
                JPABaseEmitter* emitter = dComIfGp_particle_set(0x8158 /* ID_IT_SN_DK_DEMO_HANE01 */, &actor->current.pos, &actor->shape_angle,
                                                                nullptr, 0xFF, nullptr, (s8)room, bdk_colorK0(actor), bdk_colorK0(actor));
                if (emitter != nullptr) {
                    bdk_setGlobalRTMatrix(emitter, bdk_morfAnmMtx(i_this, 0x16 /* DK_JNT_J_DK_MUNE2_e */));
                }
            }
        }
        cLib_addCalcAngleS2(&actor->current.angle.y, fopAcM_searchPlayerAngleY(actor), 10, 0x1300);
        break;
    default:
        break;
    }
}
VERIFY(0x0206B67C, start);

/* 0206D0FC (HD: the boss shakes for 10 frames before it falls, drifts to the centre with a speed
 * that builds up, the bk/boko are removed here, and the fall into the water also pushes the tower's
 * falling fragments away) */
void t_down(bdk_class* i_this) {
    WWHD_FUNC(0x0206D0FC, void, i_this);
    fopAc_ac_c* actor = i_this;
    switch (i_this->mState) {
    case 0:
        anm_init(i_this, 0x3D /* BCK_TO_DAMAGE_T1 */, 1.0f, 0, 0.0f, -1, 0);
        i_this->m25D4 = 10.0f;
        i_this->m2EC[0] = 0x5A;
        i_this->mState = i_this->mState + 1;
        i_this->m25A0 = 0x32;
        i_this->m2EC[1] = 0x5A;
        actor->actor_status |= 0x4000; /* fopAcStts_UNK4000_e */
        eff_hane_set(i_this, &i_this->m1168, 5, 0);
        dComIfGs_offSwitch_bdk(0x80, fopAcM_GetRoomNo(actor));
        fpcM_Search(0x020650B0 /* bk_delete_sub */, i_this);
        fpcM_Search(0x02065104 /* boko_delete_sub */, i_this);
        actor->speedF = 0.0f;
        /* fall through */
    case 1:
        if (i_this->m2EC[0] >= 0x50) {
            s16 t = i_this->m2EC[0];
            mDoExt_McaMorf* morf = i_this->mpMorf;
            if (t != 0x50) {
                morf_setFrame(morf, (t & 1) ? 0.0f : 1.0f);
                return;
            }
            morf_setPlaySpeed(morf, 1.0f);
            i_this->m259E = 0xB4;
        }
        /* fall through */
    case 2: {
        if (i_this->mpMorf->isStop()) {
            anm_init(i_this, 0x25 /* BCK_DAMAGE_T1 */, 2.0f, 2, 1.0f, -1, 0);
        }
        cLib_addCalc2(&actor->current.pos.x, center_pos.x, 0.1f, actor->speedF);
        cLib_addCalc2(&actor->current.pos.z, center_pos.z, 0.1f, actor->speedF);
        if (i_this->m2EC[0] < 0x46) {
            cLib_addCalc2(&actor->speedF, 10.0f, 1.0f, 0.5f);
        }
        cLib_addCalcAngleS2(&i_this->m259E, 1, 1, 2);
        if (i_this->m2EC[1] == 1) {
            anm_init(i_this, 0x36 /* BCK_RAKKA_T1 */, fadds_ppc(REG0_F(6), 20.0f), 2, 1.0f, -1, 0);
            i_this->m259E = 1;
            bdk_seStart(actor, 0x589F /* JA_SE_CM_DK_FALL_WATER */, 0);
        }
        if (i_this->m2EC[0] != 0) {
            return;
        }
        f32 sy = actor->speed.y;
        f32 y = fadds_ppc(actor->current.pos.y, sy);
        actor->current.pos.y = y;
        actor->speed.y = fsubs_ppc(sy, 5.0f);
        if (y < 6843.0f && i_this->mState == 1) {
            i_this->mState = 2;
            gabi::Local<cXyz> offset;
            offset->x = 3600.0f;
            offset->z = -3800.0f;
            offset->y = fadds_ppc(REG_F(10, 0), 6843.0f);
            dComIfGp_particle_set(0x8147 /* ID_IT_SN_DK_DOBON_HAMON00 */, offset);
            dComIfGp_particle_set(0x8148 /* ID_IT_SN_DK_DOBON_HAMON01 */, offset);
            dComIfGp_particle_set(0x8149 /* ID_IT_SN_DK_DOBON_SHIBUKI00 */, offset);
            dComIfGp_particle_set(0x814A /* ID_IT_SN_DK_DOBON_WP00 */, offset);
            bdk_seStart(actor, 0x782E /* JA_SE_CM_DK_WATER_COLUMN */, 0);
            fpcM_Search(0x02064EA8 /* obj_hahen_sub (HD) */, i_this);
            y = actor->current.pos.y;
        }
        if (y < 5500.0f) {
            i_this->mState = 0;
            actor->max_health = 20;
            i_this->mAction = 0xF; /* ACTION_START */
            actor->health = 20;
            actor->current.pos.y = -25000.0f;
        }
        break;
    }
    default:
        break;
    }
}
VERIFY(0x0206D0FC, t_down);

/* 0206C3AC (HD: also sets m2F8 = 5 every frame) */
void t_fly(bdk_class* i_this) {
    WWHD_FUNC(0x0206C3AC, void, i_this);
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->mF10 = 1;
    i_this->m2F8 = 5;
    fopAc_ac_c* bdkobj = (fopAc_ac_c*)fpcM_Search(0x020669B8 /* obj_s_sub */, actor);
    f32 dVar11 = -500.0f;
    f32 dVar12 = 0.0f;
    f32 dVar10 = 20.0f;
    if (i_this->m2C4 == 100) {
        dComIfGs_onSwitch_bdk(0x80, fopAcM_GetRoomNo(actor));
        fpcM_Search(0x02064CF4 /* kamome_delete_sub */, actor);
    }

    switch (i_this->mState) {
    case -10:
        anm_init(i_this, 0x3B /* BCK_TOBITATU1 */, 5.0f, 0, 1.0f, 0x16 /* BAS_TOBITATU1 */, 0);
        i_this->mState = i_this->mState + 1;
        break;
    case -9:
        if (i_this->mpMorf->isStop()) {
            i_this->mState = 0;
        }
        break;
    case 0:
        actor->actor_status &= ~0x4000u; /* fopAcStts_UNK4000_e */
        anm_init(i_this, 0x2B /* BCK_FLY2 */, 5.0f, 2, 1.0f, 0xC /* BAS_FLY2 */, 0);
        i_this->mState = i_this->mState + 1;
        /* fall through */
    case 1:
        if (bdkobj != nullptr) {
            i_this->m2CC.copy(bdkobj->current.pos);
            anm_init(i_this, 0x1D /* BCK_ATTACK_T1 */, 5.0f, 0, 1.0f, -1, 0);
            i_this->mState = i_this->mState + 1;
            bdk_monsSeStart(actor, 0x4865 /* JA_SE_CV_DK_ATTACK */, 0);
            i_this->mp2598 = bdkobj;
        } else {
            i_this->m2CC.copy(player->current.pos);
            if (i_this->m2EC[0] == 0) {
                anm_init(i_this, 0x1E /* BCK_ATTACK_T2 */, 5.0f, 0, 1.0f, -1, 0);
                i_this->mState = 10;
                if (actor != nullptr && gabi::ea(&actor->eyePos) != 0) {
                    bdk_monsSeStart_e(actor, 0x4865 /* JA_SE_CV_DK_ATTACK */, 0);
                    mDoAud_seStart(0x589D /* JA_SE_CM_DK_FOOT_ATTACK */, &actor->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                }
                i_this->m2EC[0] = l_HIO.m028;
            }
        }
        break;
    case 2:
        if (gabi::ftoi(morf_frame(i_this->mpMorf)) >= REG0_S(3) + 0x2B && gabi::ftoi(morf_frame(i_this->mpMorf)) <= REG0_S(4) + 0x32) {
            wind_set(i_this, &actor->current.pos);
            i_this->m2584 = 1;
            dVar11 = fadds_ppc(REG8_F(8), 150.0f);
            dVar10 = 300.0f;
            fopAc_ac_c* obj = i_this->mp2598;
            if (obj != nullptr && gabi::ftoi(morf_frame(i_this->mpMorf)) == 0x30) {
                obj->mParameters = 0xF; /* fopAcM_SetParam */
                i_this->mp2598 = nullptr;
                bdk_seStart_a(actor, 0x589C /* JA_SE_CM_DK_PECK */, 0);
            }
        }
        if (i_this->mpMorf->isStop()) {
            i_this->m2E0 = 0.0f;
            i_this->mState = 0;
        }
        break;
    case 10:
        if (gabi::ftoi(morf_frame(i_this->mpMorf)) >= REG0_S(3) + 0x17 && gabi::ftoi(morf_frame(i_this->mpMorf)) <= REG0_S(4) + 0x32) {
            wind_set(i_this, &actor->current.pos);
            s32 lim = REG0_S(5) + 0x23;
            s32 frame = gabi::ftoi(morf_frame(i_this->mpMorf));
            dVar12 = fadds_ppc(REG8_F(9), -50.0f);
            dVar11 = fadds_ppc(REG8_F(10), 250.0f);
            dVar10 = 300.0f;
            if (frame <= lim) {
                i_this->m2584 = 2;
            }
        }
        if (i_this->mFootCCSph[0].ChkAtHit() || i_this->mFootCCSph[1].ChkAtHit()) {
            bdk_seStart_a(actor, 0x589E /* JA_SE_CM_DK_FOOT_ATK_COL */, 0);
        }
        if (i_this->mpMorf->isStop()) {
            i_this->m2E0 = 0.0f;
            i_this->mState = 0;
        }
        break;
    default:
        break;
    }

    gabi::Local<cXyz> pos;
    gabi::Local<cXyz> dist;
    {
        gabi::Local<cXyz> tmp;
        cXyz_mi(&i_this->m2CC, tmp, &center_pos);
        bdk_fcopy(pos->x, tmp->x);
        bdk_fcopy(pos->y, tmp->y);
        bdk_fcopy(pos->z, tmp->z);
        s16 angle = cM_atan2s(tmp->x, tmp->z);
        cLib_addCalcAngleS2(&actor->current.angle.y, angle, 10, 0x800);
        pos->x = 0.0f;
        pos->y = dVar12;
        pos->z = fadds_ppc(dVar11, -450.0f);
        cMtx_YrotS(calc_mtx(), angle);
    }
    cMtx_XrotM(calc_mtx(), REG_S(8, 4));
    MtxPosition(pos, dist);
    cLib_addCalc2(&actor->current.pos.x, fadds_ppc(i_this->m2CC.x, dist->x), 0.2f, gabi::fmuls_ppc(dVar10, i_this->m2E0));
    f32 sy = actor->speed.y;
    if (sy > 0.0f) {
        actor->current.pos.y = fadds_ppc(actor->current.pos.y, sy);
        actor->speed.y = fsubs_ppc(actor->speed.y, 5.0f);
    } else {
        cLib_addCalc2(&actor->current.pos.y, fadds_ppc(i_this->m2CC.y, dist->y), 0.2f, gabi::fmuls_ppc(dVar10, i_this->m2E0));
    }
    cLib_addCalc2(&actor->current.pos.z, fadds_ppc(i_this->m2CC.z, dist->z), 0.2f, gabi::fmuls_ppc(dVar10, i_this->m2E0));
    cLib_addCalc2(&i_this->m2E0, 1.0f, 1.0f, 0.02f);

    if (player->current.pos.y > fadds_ppc(REG0_F(3), 9250.0f)) {
        i_this->mAction = 0x66; /* ACTION_T_LASTATTACK */
        i_this->mState = 0;
    }
}
VERIFY(0x0206C3AC, t_fly);

/* 0206CAD8 */
void t_lastattack(bdk_class* i_this) {
    WWHD_FUNC(0x0206CAD8, void, i_this);
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s8 isfalling = 0;

    f32 x = gabi::fmadds(REG0_F(7), 0.01f, 2785.0f);
    f32 fVar1 = gabi::fmadds(REG0_F(8), 0.01f, 9579.0f);
    f32 fVar2 = gabi::fmadds(REG0_F(9), 0.01f, -3872.0f);
    i_this->m2CC.x = x;
    i_this->m2CC.y = fVar1;
    i_this->m2CC.z = fVar2;
    cLib_addCalc2(&actor->current.pos.x, x, 0.5f, 30.0f);
    cLib_addCalc2(&actor->current.pos.z, i_this->m2CC.z, 0.5f, 30.0f);
    f32 sy = actor->speed.y;
    f32 y = fadds_ppc(actor->current.pos.y, sy);
    u32 cyb = gabi::load<u32>(gabi::ea(&i_this->m2CC.y));
    f32 cy = gabi::f32_from_bits(cyb);
    actor->current.pos.y = y;
    actor->speed.y = fsubs_ppc(sy, 5.0f);
    if (!(y > cy)) {
        isfalling = 1;
        gabi::store<u32>(gabi::ea(&actor->current.pos.y), cyb);
        actor->speed.y = -1.0f;
    }

    s16 angle = (s16)(fopAcM_searchPlayerAngleY(actor) - 0x8000);
    if (angle < -0xEB8) {
        cLib_addCalcAngleS2(&actor->current.angle.y, 0x7148, 2, 0x800);
    } else {
        if (angle > 0xE8) {
            angle = 0xE8;
        }
        cLib_addCalcAngleS2(&actor->current.angle.y, (s16)(angle + 0x8000), 2, 0x800);
    }

    switch (i_this->mState) {
    case 0:
        actor->speed.y = fadds_ppc(REG0_F(6), 80.0f);
        i_this->mState = i_this->mState + 1;
        anm_init(i_this, 0x3F /* BCK_TYAKUTI_T1 */, 10.0f, 0, 0.001f, 0x18 /* BAS_TYAKUTI_T1 */, 0);
        break;
    case 1:
        if (!isfalling) {
            break;
        }
        anm_init(i_this, 0x3F /* BCK_TYAKUTI_T1 */, 1.0f, 0, 1.0f, 0x18 /* BAS_TYAKUTI_T1 */, 0);
        i_this->mState = i_this->mState + 1;
        bdk_StartShock(5);
        {
            s32 room = fopAcM_GetRoomNo(actor);
            GXColor* k0 = gabi::at<GXColor>(gabi::ea(&i_this->m6224) + 0x98);
            dComIfGp_particle_set(0x8131 /* ID_IT_SN_DK_TYAKUTI_ROCK00 */, &actor->current.pos, nullptr, nullptr, 0xFF, nullptr, (s8)room, k0, k0);
        }
        if (i_this->m6078[0] == 0) {
            s32 room = fopAcM_GetRoomNo(actor);
            i_this->m6078[0] = 100;
            bdk_particle_setToon(0xA132 /* ID_IT_ST_DK_TYAKUTI_SMOKE00 */, &actor->current.pos, nullptr, nullptr, 0xB9, &i_this->m6080[0], (s8)room);
        }
        break;
    case 2:
        if (i_this->mpMorf->isStop()) {
            anm_init(i_this, 0x41 /* BCK_WAIT_T1 */, 5.0f, 2, 1.0f, -1, 0);
            i_this->mState = i_this->mState + 1;
        }
        break;
    case 3:
        if (i_this->m2EC[0] == 0 && fopAcM_searchPlayerDistance(actor) < fadds_ppc(REG0_F(13), 700.0f)) {
            anm_init(i_this, 0x1F /* BCK_ATTACK_T3 */, 5.0f, 0, 1.0f, -1, 0);
            bdk_monsSeStart_e(actor, 0x4865 /* JA_SE_CV_DK_ATTACK */, 0);
            i_this->mState = 4;
        }
        break;
    case 4: {
        mDoExt_McaMorf* morf = i_this->mpMorf;
        if (morf_frame(morf) > 10.0f && morf_frame(morf) < 15.0f) {
            i_this->m2584 = 1;
            morf = i_this->mpMorf;
        }
        if ((s16)gabi::ftoi(morf_frame(morf)) == (s16)(REG0_S(5) + 0xD)) {
            bdk_seStart(actor, 0x589C /* JA_SE_CM_DK_PECK */, 0);
            bdk_StartShock(3);
            eff_hane_set(i_this, &i_this->m1168, 5, 0);
            morf = i_this->mpMorf;
        }
        if (morf->isStop()) {
            anm_init(i_this, 0x41 /* BCK_WAIT_T1 */, 5.0f, 2, 1.0f, -1, 0);
            i_this->mState = 3;
            i_this->m2EC[0] = (s16)gabi::ftoi(fadds_ppc(cM_rndF(100.0f), 100.0f));
        }
        break;
    }
    default:
        break;
    }
    if (player->current.pos.y < fadds_ppc(REG0_F(3), 9240.0f)) {
        i_this->mAction = 0x64; /* ACTION_T_FLY */
        i_this->mState = -10;
        actor->speed.y = fadds_ppc(REG0_F(7), 70.0f);
    }
}
VERIFY(0x0206CAD8, t_lastattack);

/* 0206BAA8 (HD: the vanishing puff is created with 0x5A instead of 0x32) */
void end(bdk_class* i_this) {
    WWHD_FUNC(0x0206BAA8, void, i_this);
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused play-object read */
    s8 bVar1 = 0;
    u16 particle_id = 0;
    i_this->m1134 = 4000;
    i_this->m2F8 = 10;

    switch (i_this->mState) {
    case 0:
        anm_init(i_this, 0x30 /* BCK_LAST_DAMAGE1 */, 2.0f, 0, 1.0f, 0x10 /* BAS_LAST_DAMAGE1 */, 0);
        i_this->mState = 5;
        i_this->m2EC[0] = 0xB3;
        particle_id = 0x8139; /* ID_IT_SN_DK_LASTDMG_HANE_W00 */
        break;
    case 1:
        anm_init(i_this, 0x32 /* BCK_LAST_DAMAGE3 */, 2.0f, 0, 1.0f, 0x12 /* BAS_LAST_DAMAGE3 */, 0);
        i_this->mState = 5;
        i_this->m2EC[0] = 0xB3;
        particle_id = 0x815A; /* ID_IT_SN_DK_LASTDMG_HANE_W02 */
        break;
    case 2:
        anm_init(i_this, 0x31 /* BCK_LAST_DAMAGE2 */, 2.0f, 0, 1.0f, 0x11 /* BAS_LAST_DAMAGE2 */, 0);
        i_this->mState = 5;
        i_this->m2EC[0] = 0;
        particle_id = 0x8159; /* ID_IT_SN_DK_LASTDMG_HANE_W01 */
        break;
    case 5:
        if (i_this->mpMorf->isStop()) {
            i_this->mState = 6;
            anm_init(i_this, 0x26 /* BCK_DEATH1 */, 2.0f, 2, 1.0f, 0xA /* BAS_DEATH1 */, 0);
            i_this->m2EC[0] = (s16)(REG0_S(3) + 300);
            for (s32 i = 0; i <= 1; i++) {
                s32 room = fopAcM_GetRoomNo(actor);
                i_this->m6100[i] = dComIfGp_particle_set(0x813A /* ID_IT_SN_DK_DEAD_HANE_A00 */, &actor->current.pos, nullptr, nullptr, 0xFF,
                                                         nullptr, (s8)room, bdk_colorK0(actor), bdk_colorK0(actor));
            }
        } else {
            if (i_this->m2EC[0] == 0) {
                cLib_addCalc2(&actor->current.pos.y, 10800.0f, 0.1f, 50.0f);
                bVar1 = 1;
            }
            break;
        }
        /* fall through */
    case 6:
        bVar1 = 1;
        for (s32 i = 0; i <= 1; i++) {
            JPABaseEmitter* emitter = i_this->m6100[i];
            if (emitter != nullptr) {
                bdk_setGlobalRTMatrix(emitter, bdk_morfAnmMtx(i_this, 0x28 + i * 0xA /* DK_JNT_J_DK_HANE_L2_e + i * DK_JNT_J_DK_YUBI_LC1_e */));
            }
        }
        if (i_this->m2EC[0] == 0xAA) {
            i_this->m25A0 = 0x6E;
        }
        cLib_addCalc2(&actor->current.pos.x, 3600.0f, 0.05f, 50.0f);
        cLib_addCalc2(&actor->current.pos.y, fadds_ppc(REG8_F(18), 15800.0f), 0.1f, fadds_ppc(REG8_F(19), 20.0f));
        cLib_addCalc2(&actor->current.pos.z, -3800.0f, 0.05f, 50.0f);
        cLib_addCalcAngleS2(&actor->current.angle.y, -8000, 0x10, 0x80);
        if (i_this->m2EC[0] == 0x3C) {
            bdk_seStart_a(actor, 0x486C /* JA_SE_CV_DK_DIE */, 0);
            anm_init(i_this, 0x27 /* BCK_DEATH2 */, 5.0f, 0, 1.0f, -1, 0);
        }
        if (i_this->m2EC[0] == 0 && REG0_S(4) == 0) {
            i_this->mState = 7;
            for (s32 i = 0; i < 4; i++) {
                i_this->m90C[i] = 0;
            }
            fpcM_Search(0x02065158 /* obj2_delete_sub */, actor);
            bdk_fcopy(i_this->m2CC.x, actor->current.pos.x);
            f32 y = actor->current.pos.y;
            u32 zb = gabi::load<u32>(gabi::ea(&actor->current.pos.z));
            actor->current.angle.y = 0x4000;
            i_this->m2CC.y = fadds_ppc(y, 200.0f);
            gabi::store<u32>(gabi::ea(&i_this->m2CC.z), zb);
            fopAcM_createDisappear(actor, &i_this->m2CC, 0x5A, 2, 0xFF);
            mDoAud_seStart(0x5878 /* JA_SE_CM_DK_DIE_EXPLODE */, &i_this->m2CC, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            i_this->m2EC[0] = 0x1E;
            for (s32 i = 0; i < 2; i++) {
                JPABaseEmitter* e = i_this->m6100[i];
                if (e != nullptr) {
                    bdk_becomeInvalidEmitter(e);
                    i_this->m6100[i] = nullptr;
                }
            }
            s32 room = fopAcM_GetRoomNo(actor);
            dComIfGp_particle_set(0x813B /* ID_IT_SN_DK_DEAD_HANE_B00 */, &actor->current.pos, nullptr, nullptr, 0xFF, nullptr, (s8)room,
                                  bdk_colorK0(actor), bdk_colorK0(actor));
        }
        break;
    case 7:
        actor->current.pos.x = 100000.0f;
        actor->current.pos.y = 100000.0f;
        if (i_this->m2EC[0] == 0) {
            i_this->mState = 8;
            i_this->m25A0 = i_this->m25A0 + 1;
        }
        break;
    default:
        break;
    }
    if (bVar1 != 0 && i_this->m2EC[4] == 0) {
        i_this->m2EC[4] = (s16)gabi::ftoi(fadds_ppc(cM_rndF(30.0f), 25.0f));
        bdk_monsSeStart(actor, 0x486B /* JA_SE_CV_DK_MODAE */, 0);
    }
    cLib_addCalcAngleS2(&actor->current.angle.z, 0, 2, 0x800);
    cLib_addCalcAngleS2(&actor->current.angle.x, 0, 2, 0x800);
    if (particle_id != 0) {
        s32 room = fopAcM_GetRoomNo(actor);
        JPABaseEmitter* emitter = dComIfGp_particle_set(0x8138 /* ID_IT_SN_DK_LASTDMG_HANE_M00 */, &actor->current.pos, &actor->shape_angle, nullptr,
                                                        0xFF, nullptr, (s8)room, bdk_colorK0(actor), bdk_colorK0(actor));
        if (emitter != nullptr) {
            bdk_setGlobalRTMatrix(emitter, bdk_morfAnmMtx(i_this, 0x16 /* DK_JNT_J_DK_MUNE2_e */));
        }
        for (s32 i = 0; i <= 1; i++) {
            room = fopAcM_GetRoomNo(actor);
            emitter = dComIfGp_particle_set(particle_id, &actor->current.pos, &actor->shape_angle, nullptr, 0xFF, nullptr, (s8)room,
                                            bdk_colorK0(actor), bdk_colorK0(actor));
            if (emitter != nullptr) {
                bdk_setGlobalRTMatrix(emitter, bdk_morfAnmMtx(i_this, 0x28 + i * 0xA));
            }
        }
    }
}
VERIFY(0x0206BAA8, end);

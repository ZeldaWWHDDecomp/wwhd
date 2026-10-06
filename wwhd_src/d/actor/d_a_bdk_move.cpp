/**
 * d_a_bdk_move.cpp (WWHD)
 * Boss - Helmaroc King (battle): move (the action dispatcher; fly, up_fly, landing, wait, jump,
 * kuti_attack and jida_attack are inlined into it).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdk.h"

using gabi::fadds_ppc;
using gabi::fmuls_ppc;
using gabi::fsubs_ppc;

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0200F164 cLib_addCalcPos2(cXyz* pos, const cXyz& target, f32 scale, f32 maxStep) */
static inline void cLib_addCalcPos2_bdk(cXyz* pos, const cXyz* target, f32 scale, f32 maxStep) { gabi::call(0x0200F164, pos, target, scale, maxStep); }
static inline void JUTReport_bdk(s32 x, s32 y, u32 fmt, s32 v) { gabi::call(0x027EC9E8, x, y, fmt, v); }
static inline f32 bdk_REGF(u32 a) { return gabi::load<f32>(a); }
static inline void bdk_vbits(cXyz* d, const cXyz* s) {
    for (int k = 0; k < 12; k += 4) gabi::store<u32>(gabi::ea(d) + k, gabi::load<u32>(gabi::ea(s) + k));
}
static inline BOOL bdk_land_area_check(cXyz* pos, f32 r) { return gabi::call<BOOL>(0x02065208, pos, r); }
static inline BOOL bdk_LineCross(void* chk) { return gabi::call<BOOL>(0x02008860, dComIfG_Bgsp(), chk); }
static const dBgS_LinChk_vt bdk_lin_vt = {0x10007F08, 0x10007F18, 0x10007F38, 0x10007F28};
static inline void bdk_lin_dt(void* lin) {
    u32 l = gabi::ea(lin);
    gabi::store<u32>(l + 0x58, 0x10007F38);
    gabi::store<u32>(l + 0x64, 0x10007E88);
    gabi::store<u32>(l + 0x20, 0x10007E78);
    gabi::call(0x02008B4C, l, 0); /* cBgS_LinChk::~cBgS_LinChk */
}
static inline f32 bdk_dist(bdk_class* i_this, cXyz* a, cXyz* b) {
    gabi::Local<cXyz> d;
    cXyz_mi(a, d, b);
    gabi::Local<cXyz> c;
    bdk_vbits(c, d);
    return std_sqrtf(PSVECSquareMag(c));
}
/* fopAcM_seStart + fopAcM_monsSeStart (HD inline pair: one actor and eyePos test) */
static inline void bdk_se_pair(fopAc_ac_c* a, u32 se, u32 voice) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        mDoAud_seStart(se, &a->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
        bdk_monsSeStart_e(a, voice, 0);
    }
}
#define BDK_EFF27 (i_this->m261C[0x27])

/* fly (inlined) */
static inline void fly(bdk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    i_this->mF10 = 1;
    if (i_this->m2CA == -1) {
        anm_init(i_this, 0x2A /* BCK_FLY1 */, 10.0f, 2, 1.0f, 0xB /* BAS_FLY1 */, 0);
        i_this->m2CA = 0;
    }
    bdk_fcopy(i_this->m2E4, l_HIO.m020);
    i_this->m2E8 = fadds_ppc(REG0_F(9), 1.0f);
    i_this->m2DC = 600.0f;

    switch ((u16)i_this->mState) {
    case 0: {
        u8 fc;
        if (i_this->m2FC == 0) {
            fc = (u8)gabi::ftoi(cM_rndF(2.99f));
            i_this->m2FC = fc;
        } else if (cM_rndF(1.0f) < 0.8f) {
            fc = i_this->m2FC;
            if (fc == 1) {
                fc = 2;
                i_this->m2FC = fc;
            } else if (fc == 2) {
                fc = 1;
                i_this->m2FC = fc;
            }
        } else {
            fc = 0;
            i_this->m2FC = fc;
        }
        if (fc == 0) {
            actor->home.pos.x = fadds_ppc(REG0_F(0), 6500.0f);
            actor->home.pos.y = fadds_ppc(REG0_F(1), 11000.0f);
            actor->home.pos.z = fadds_ppc(REG0_F(2), 2600.0f);
            { f32 r = cM_rndFX(fadds_ppc(REG0_F(3), 3000.0f)); i_this->m2CC.x = fadds_ppc(actor->home.pos.x, r); }
            i_this->m2CC.y = fadds_ppc(actor->home.pos.y, 1000.0f);
            { f32 r = cM_rndFX(fadds_ppc(REG0_F(3), 3000.0f)); i_this->m2CC.z = fadds_ppc(actor->home.pos.z, r); }
        } else if (fc == 1) {
            actor->home.pos.x = fadds_ppc(REG0_F(0), 9800.0f);
            actor->home.pos.y = fadds_ppc(REG0_F(1), 12000.0f);
            actor->home.pos.z = fadds_ppc(REG0_F(2), -1500.0f);
            { f32 r = cM_rndFX(fadds_ppc(REG0_F(3), 1000.0f)); i_this->m2CC.x = fadds_ppc(actor->home.pos.x, r); }
            { f32 r = cM_rndF(1000.0f); i_this->m2CC.y = fadds_ppc(actor->home.pos.y, r); }
            { f32 r = cM_rndFX(fadds_ppc(REG0_F(3), 1000.0f)); i_this->m2CC.z = fadds_ppc(actor->home.pos.z, r); }
        } else if (fc == 2) {
            actor->home.pos.x = fadds_ppc(REG0_F(0), -1000.0f);
            actor->home.pos.y = fadds_ppc(REG0_F(1), 12000.0f);
            actor->home.pos.z = fadds_ppc(REG0_F(2), -2900.0f);
            { f32 r = cM_rndFX(fadds_ppc(REG0_F(3), 1000.0f)); i_this->m2CC.x = fadds_ppc(actor->home.pos.x, r); }
            i_this->m2CC.y = fadds_ppc(actor->home.pos.y, 1000.0f);
            { f32 r = cM_rndFX(fadds_ppc(REG0_F(3), 1000.0f)); i_this->m2CC.z = fadds_ppc(actor->home.pos.z, r); }
        }
        i_this->m2E0 = 0.0f;
        if (bdk_dist(i_this, &i_this->m2CC, &actor->current.pos) > 1000.0f) {
            i_this->mState = i_this->mState + 1;
            i_this->m2EC[0] = 0;
        }
        break;
    }
    case 1:
        if (bdk_dist(i_this, &i_this->m2CC, &actor->current.pos) < 1500.0f) {
            if (i_this->m2FC == 0 || cM_rndF(1.0f) < 0.3f || i_this->m2593 >= 4 || i_this->m8F8 < 2) {
                i_this->m2593 = 0;
                i_this->mAction = 2; /* ACTION_LANDING */
                i_this->mState = 0;
            } else {
                i_this->mAction = 7; /* ACTION_FLY_ATTACK */
                i_this->mState = 0;
                i_this->m2593 = i_this->m2593 + 1;
            }
        }
        break;
    default:
        break;
    }
    pos_move(i_this);
}

/* up_fly (inlined) */
static inline void up_fly(bdk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    u8 pos_flag = 0;
    switch ((u16)i_this->mState) {
    case 0: {
        anm_init(i_this, 0x3B /* BCK_TOBITATU1 */, 5.0f, 0, 1.0f, 0x16 /* BAS_TOBITATU1 */, 0);
        i_this->m2618 = 0x19;
        cMtx_YrotS(calc_mtx(), actor->current.angle.y);
        gabi::Local<cXyz> diff;
        gabi::Local<cXyz> out;
        gabi::Local<cXyz> sum;
        diff->x = 0.0f;
        diff->y = 2000.0f;
        diff->z = 5000.0f;
        MtxPosition(diff, out);
        cXyz_pl(&actor->current.pos, sum, out);
        actor->speedF = 0.0f;
        bdk_vbits(&i_this->m2CC, sum);
        i_this->mState = i_this->mState + 1;
        i_this->m2E4 = 0.0f;
        i_this->m2E8 = 1.0f;
        break;
    }
    case 1:
        if (gabi::ftoi(morf_frame(i_this->mpMorf)) != REG0_S(7) + 0x17) {
            pos_flag = 1;
            break;
        }
        i_this->mState = 2;
        i_this->m2E4 = 40.0f;
        actor->speedF = fadds_ppc(REG0_F(8), 50.0f);
        actor->current.angle.x = -0x3000;
        i_this->m2E8 = fadds_ppc(REG0_F(9), 1.0f);
        /* fall through */
    case 2:
        i_this->m2E0 = 0.0f;
        if (i_this->mpMorf->isStop()) {
            i_this->mAction = 0; /* ACTION_FLY */
            i_this->m2CA = -1;
            i_this->mState = 0;
        }
        break;
    default:
        break;
    }
    if (!pos_flag) {
        pos_move(i_this);
    }
}

/* landing (inlined). HD: the final approach moves with cLib_addCalcPos2 at speedF and starts in the
 * frame the boss gets close; the drop keeps the horizontal speed (damped by 0.9) */
static inline void landing(bdk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    u8 pos_flag = 0;
    u8 eff_hane_num = 0;
    i_this->mF10 = 1;
    switch ((u16)i_this->mState) {
    case 0xFFFE:
    case 0: {
        s16 angle = fopAcM_searchPlayerAngleY(actor);
        cMtx_YrotS(calc_mtx(), angle);
        gabi::Local<cXyz> diff2;
        gabi::Local<cXyz> out;
        gabi::Local<cXyz> sum;
        diff2->x = 0.0f;
        diff2->y = 0.0f;
        diff2->z = i_this->mState == 0 ? -800.0f : 0.0f;
        MtxPosition(diff2, out);
        cXyz_pl(&player->current.pos, sum, out);
        bdk_vbits(&i_this->m2CC, sum);
        if (!bdk_land_area_check(&i_this->m2CC, 1400.0f)) {
            i_this->mAction = 0; /* ACTION_FLY */
            i_this->mState = 0;
            break;
        }
        i_this->mState = 1;
        i_this->m2EC[0] = 0x32;
        i_this->m2CC.y = fadds_ppc(i_this->m2CC.y, fadds_ppc(REG0_F(2), 600.0f));
        i_this->m2DC = 1000.0f;
        i_this->m2E0 = 0.0f;
    }
        /* fall through */
    case 1:
        if (bdk_dist(i_this, &i_this->m2CC, &actor->current.pos) < 1300.0f) {
            i_this->mState = 2;
            anm_init(i_this, 0x3E /* BCK_TYAKUTI1 */, 15.0f, 0, 0.001f, 0x17 /* BAS_TYAKUTI1 */, 0);
            pos_flag = 1;
            goto approach; /* HD: no m2E0 = 1; the approach starts in this frame */
        }
        if (i_this->m2EC[0] == 1) {
            anm_init(i_this, 0x2F /* BCK_KAKKU1 */, 30.0f, 2, 1.0f, -1, 0);
        }
        break;
    case 2:
    approach:
        pos_flag = 1;
        cLib_addCalcPos2_bdk(&actor->current.pos, &i_this->m2CC, 1.0f, actor->speedF);
        cLib_addCalcAngleS2(&actor->current.angle.z, 0, 2, 0x800);
        cLib_addCalcAngleS2(&actor->current.angle.x, 0, 2, 0x800);
        if (bdk_dist(i_this, &i_this->m2CC, &actor->current.pos) < fadds_ppc(REG0_F(3), 300.0f)) {
            i_this->mState = 3;
        }
        break;
    case 3:
    case 4:
        pos_flag = 1;
        eff_hane_num = 3;
        {
            f32 sz = actor->speed.z;
            f32 sy = actor->speed.y;
            f32 sx = actor->speed.x;
            actor->current.pos.y = fadds_ppc(actor->current.pos.y, sy);
            actor->speed.y = fadds_ppc(sy, actor->gravity);
            actor->current.pos.x = fadds_ppc(actor->current.pos.x, sx);
            actor->current.pos.z = fadds_ppc(actor->current.pos.z, sz);
            actor->speed.x = fmuls_ppc(sx, 0.9f);
            actor->speed.z = fmuls_ppc(sz, 0.9f);
        }
        cLib_addCalcAngleS2(&actor->current.angle.z, 0, 2, 0x800);
        cLib_addCalcAngleS2(&actor->current.angle.x, 0, 2, 0x800);
        if (i_this->m2586 != 0 || i_this->mAcch.ChkGroundHit() ||
            !(actor->current.pos.y > fadds_ppc(i_this->mAcch.GetGroundH(), 1.0f))) {
            actor->speedF = 0.0f;
            actor->speed.z = 0.0f;
            actor->speed.x = 0.0f;
            actor->speed.y = -10.0f;
            if (i_this->mState == 3) {
                anm_init(i_this, 0x3E /* BCK_TYAKUTI1 */, 5.0f, 0, 1.0f, 0x17 /* BAS_TYAKUTI1 */, 0);
                i_this->mState = 4;
                eff_hane_num = 10;
                bdk_StartShock(5);
                dComIfGp_particle_set(0x8131 /* ID_IT_SN_DK_TYAKUTI_ROCK00 */, &actor->current.pos, nullptr, nullptr, 0xFF, nullptr,
                                      (s8)fopAcM_GetRoomNo(actor), gabi::at<GXColor>(gabi::ea(&i_this->m6224) + 0x98),
                                      gabi::at<GXColor>(gabi::ea(&i_this->m6224) + 0x98));
                if (i_this->m6078[0] == 0) {
                    i_this->m6078[0] = 100;
                    bdk_particle_setToon(0xA132 /* ID_IT_ST_DK_TYAKUTI_SMOKE00 */, &actor->current.pos, nullptr, nullptr, 0xB9, &i_this->m6080[0],
                                         (s8)fopAcM_GetRoomNo(actor));
                }
            }
        }
        if (i_this->mpMorf->isStop()) {
            i_this->mAction = 3; /* ACTION_WAIT */
            i_this->mState = 0;
            i_this->m2EC[3] = (s16)gabi::ftoi(fadds_ppc(cM_rndF(600.0f), 600.0f));
        }
        i_this->m2584 = 2;
        break;
    default:
        break;
    }
    if (!pos_flag) {
        pos_move(i_this);
    }
    if (eff_hane_num != 0) {
        if (eff_hane_num <= 7) {
            if (!(i_this->m2C4 & eff_hane_num)) {
                eff_hane_set(i_this, &i_this->m1168, 1, 0);
            }
        } else {
            eff_hane_set(i_this, &i_this->m1168, eff_hane_num, 0);
        }
    }
}

/* HD: a wind attack when the player stands near (4706, -4635) or near a wall */
static inline BOOL wait_wind_check(bdk_class* i_this, fopAc_ac_c* player) {
    f32 dz = fsubs_ppc(-4635.0f, player->current.pos.z);
    f32 dx = fsubs_ppc(4706.0f, player->current.pos.x);
    if (std_sqrtf(gabi::fmadds(dx, dx, fmuls_ppc(dz, dz))) < 400.0f) {
        return TRUE;
    }
    BOOL hit = FALSE;
    gabi::Local<u8[0x6C]> linChk;
    gabi::Local<cXyz> start;
    gabi::Local<cXyz> vec;
    gabi::Local<cXyz> end;
    dBgS_LinChk_ct(linChk.get(), bdk_lin_vt, false);
    bdk_fcopy(start->x, player->current.pos.x);
    f32 py = player->current.pos.y;
    bdk_fcopy(start->y, player->current.pos.y);
    bdk_fcopy(start->z, player->current.pos.z);
    vec->y = 0.0f;
    vec->x = 0.0f;
    vec->z = fadds_ppc(bdk_REGF(0x1047C038), 300.0f);
    start->y = fadds_ppc(py, 50.0f);
    s16 a = 0;
    for (s32 i = 8; i != 0; i--) {
        cMtx_YrotS(calc_mtx(), a);
        MtxPosition(vec, end);
        PSVECAdd(end, start, end);
        dBgS_LinChk_Set(linChk.get(), start, end, i_this);
        if (bdk_LineCross(linChk.get())) {
            hit = TRUE;
            break;
        }
        a = (s16)(a + 0x2000);
    }
    bdk_lin_dt(linChk.get());
    return hit;
}

static inline void wait_to_up_fly(bdk_class* i_this) {
    i_this->m2594 = 0;
    i_this->m2592 = 0;
    i_this->mAction = 1; /* ACTION_UP_FLY */
}

/* wait (inlined). HD: turns of more than 0x1000 are made by hopping (height by the angle); a
 * wall hit while walking and the new state 10 (random) end in a take-off; the wind attack is
 * forced near a fixed point or a wall; m2DC 0 / 100 while turning */
static inline void wait(bdk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dist = fopAcM_searchPlayerDistance(actor);
    i_this->mF10 = 1;
    bdk_vbits(&i_this->m2CC, &player->current.pos);
    s16 pa = fopAcM_searchPlayerAngleY(actor);
    s16 angle = (s16)(actor->current.angle.y - pa);
    i_this->m2E4 = 0.0f;
    i_this->m2E8 = 5.0f;
    i_this->m2DC = 500.0f;
    /* pl_view_check (inlined) */
    BOOL view;
    {
        s16 pa2 = fopAcM_searchPlayerAngleY(actor);
        s16 a = (s16)(actor->current.angle.y - pa2);
        if (a < 0) {
            a = (s16)-a;
        }
        view = (u16)a < 0x6000;
    }

    switch ((u16)i_this->mState) {
    case 0:
        if (i_this->m2594 >= 5 || (i_this->m2592 >= 2 && i_this->m8F8 < 4) || !view || i_this->m2EC[3] == 0) {
            i_this->m2594 = 0;
            i_this->m2592 = 0;
            i_this->mAction = 1; /* ACTION_UP_FLY */
            break;
        }
        anm_init(i_this, 0x40 /* BCK_WAIT1 */, 10.0f, 2, 1.0f, -1, 0);
        i_this->mState = 1;
        i_this->m2EC[0] = (s16)gabi::ftoi(fadds_ppc(cM_rndF(30.0f), 40.0f));
        actor->speedF = 0.0f;
        i_this->m2CA = 0;
        /* fall through */
    case 1:
        i_this->m2DC = 0.0f;
        if ((u32)(angle + 0x1000) >= 0x2001) {
            i_this->mState = 5;
            anm_init(i_this, 0x2E /* BCK_JUMP1 */, 5.0f, 0, 0.0f, 0xF /* BAS_JUMP1 */, 0);
            s16 aa = angle;
            if (aa < 0) {
                aa = (s16)-aa;
            }
            f32 sy = fmuls_ppc((f32)aa, 0.005f);
            if (sy > 50.0f) {
                sy = 50.0f;
            }
            actor->speed.y = sy;
            actor->speedF = 0.0f;
            break;
        }
        if (i_this->m2EC[1] == 0) {
            if (!view) {
                i_this->mState = 0;
                break;
            }
            if (angle > 0x800) {
                if (i_this->m2CA != 1) {
                    i_this->m2CA = 1;
                    anm_init(i_this, 0x37 /* BCK_SENKAI_L1 */, 10.0f, 2, 1.0f, 0x13 /* BAS_SENKAI_L1 */, 0);
                    i_this->m2EC[1] = 0x14;
                }
            } else if (angle < -0x800) {
                if (i_this->m2CA != 2) {
                    i_this->m2CA = 2;
                    anm_init(i_this, 0x38 /* BCK_SENKAI_R1 */, 10.0f, 2, 1.0f, 0x14 /* BAS_SENKAI_R1 */, 0);
                    i_this->m2EC[1] = 0x14;
                }
            } else if (i_this->m2CA != 3) {
                i_this->m2CA = 3;
                anm_init(i_this, 0x40 /* BCK_WAIT1 */, 10.0f, 2, 1.0f, -1, 0);
                i_this->m2EC[1] = 0x14;
            }
        }
        if (i_this->m2CA != 3) {
            i_this->m2DC = 100.0f; /* HD: 100 (GameCube REG0_F(0) + 800) */
        }
        if (i_this->m2EC[0] == 0 && i_this->m2CA == 3) {
            u8 mode = l_HIO.m014;
            if (mode != 0) {
                if (mode == 1) {
                    i_this->mAction = 5; /* ACTION_KUTI_ATTACK */
                } else if (mode == 2) {
                    i_this->mAction = 8; /* ACTION_WIND_ATTACK */
                }
                i_this->mState = 0;
            } else {
                i_this->m2592 = i_this->m2592 + 1;
                if (dist < fadds_ppc(REG0_F(11), 450.0f)) {
                    i_this->mAction = 6; /* ACTION_JIDA_ATTACK */
                    i_this->mState = 0;
                } else if (dist < fadds_ppc(REG0_F(12), 550.0f)) {
                    if (wait_wind_check(i_this, player)) {
                        i_this->mAction = 8; /* ACTION_WIND_ATTACK */
                    } else if (i_this->m8F8 < 2) {
                        i_this->mAction = 5; /* ACTION_KUTI_ATTACK */
                    } else if (cM_rndF(1.0f) < 0.25f) {
                        i_this->mAction = 8; /* ACTION_WIND_ATTACK */
                    } else {
                        i_this->mAction = 5; /* ACTION_KUTI_ATTACK */
                    }
                    i_this->mState = 0;
                } else if (dist < fadds_ppc(REG0_F(15), 1300.0f)) {
                    i_this->mState = 2;
                    anm_init(i_this, 0x42 /* BCK_WALK1 */, 3.0f, 2, l_HIO.m01C, 0x19 /* BAS_WALK1 */, 0);
                    i_this->m2E0 = 0.0f;
                    actor->speedF = 0.0f;
                } else {
                    f32 r = cM_rndF(1.0f);
                    i_this->mState = 0;
                    if (r < 0.5f) {
                        i_this->mAction = 4; /* ACTION_JUMP */
                    } else {
                        i_this->mAction = 8; /* ACTION_WIND_ATTACK */
                    }
                }
            }
        }
        break;
    case 2:
        bdk_fcopy(i_this->m2E4, l_HIO.m018);
        i_this->m2E8 = 1.0f;
        if (dist < fadds_ppc(REG0_F(12), 530.0f) || dist > 1350.0f) {
            i_this->mState = 1;
        }
        if (i_this->mAcch.ChkWallHit()) { /* HD */
            wait_to_up_fly(i_this);
            i_this->mState = 0;
        }
        break;
    case 5:
        i_this->mState = 6; /* HD: the jump animation is started in state 1 */
        /* fall through */
    case 6:
        i_this->m2DC = 700.0f;
        if (actor->speed.y < 0.0f && actor->current.pos.y < fadds_ppc(i_this->mAcch.GetGroundH(), 5.0f)) {
            anm_init(i_this, 0x2E /* BCK_JUMP1 */, 2.0f, 0, 1.0f, 0xF /* BAS_JUMP1 */, 0);
            i_this->mState = 7;
        }
        break;
    case 7:
        i_this->m2DC = 0.0f;
        if (i_this->mpMorf->isStop()) {
            i_this->mState = 1;
            anm_init(i_this, 0x40 /* BCK_WAIT1 */, 10.0f, 2, 1.0f, -1, 0); /* HD */
        }
        break;
    case 10: /* HD */
        if (i_this->mpMorf->isStop()) {
            if (cM_rndF(1.0f) < 0.5f) {
                wait_to_up_fly(i_this);
                i_this->mState = 0;
            } else {
                i_this->mState = 1;
                anm_init(i_this, 0x40 /* BCK_WAIT1 */, 10.0f, 2, 1.0f, -1, 0);
                i_this->m2EC[0] = 0;
                i_this->m2CA = 0;
            }
        }
        break;
    default:
        break;
    }
    i_this->m2586 = 2; /* HD */
    ground_move(i_this);
}

/* jump (inlined) */
static inline void jump(bdk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->m2E4 = 0.0f;
    i_this->mF10 = 1;
    i_this->m2E8 = 5.0f;
    i_this->m2DC = 500.0f;
    switch ((u16)i_this->mState) {
    case 0:
        anm_init(i_this, 0x3B /* BCK_TOBITATU1 */, 5.0f, 0, 1.0f, 0x16 /* BAS_TOBITATU1 */, 0);
        i_this->m2618 = 0x19;
        i_this->mState = 1;
        bdk_vbits(&i_this->m2CC, &player->current.pos);
        break;
    case 1: {
        if (REG0_S(7) + 0x17 != gabi::ftoi(morf_frame(i_this->mpMorf))) {
            break;
        }
        i_this->m2586 = 0;
        i_this->mState = 2;
        actor->speed.y = fadds_ppc(REG8_F(9), 50.0f);
        gabi::Local<cXyz> diff;
        gabi::Local<cXyz> out;
        gabi::Local<cXyz> sum;
        diff->x = 0.0f;
        diff->y = 0.0f;
        diff->z = 10000.0f;
        cMtx_YrotS(calc_mtx(), actor->current.angle.y);
        MtxPosition(diff, out);
        cXyz_pl(out, sum, &actor->current.pos);
        bdk_vbits(&i_this->m2CC, sum);
        i_this->m2EC[0] = (s16)(REG_S(8, 5) + 0x1E);
    }
        /* fall through */
    case 2:
        i_this->m2E4 = 30.0f;
        i_this->m2E8 = 30.0f;
        actor->speedF = fadds_ppc(REG8_F(10), 30.0f);
        if (actor->speed.y < 0.0f) {
            actor->speed.y = 0.0f;
        }
        if (i_this->m2EC[0] == 0) {
            anm_init(i_this, 0x3E /* BCK_TYAKUTI1 */, 15.0f, 0, 0.001f, 0x17 /* BAS_TYAKUTI1 */, 0);
            i_this->mAction = 2; /* ACTION_LANDING */
            i_this->mState = 3;
        }
        break;
    default:
        break;
    }
    ground_move(i_this);
}

/* jida_attack (inlined) */
static inline void jida_attack(bdk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    f32 dist = fopAcM_searchPlayerDistance(actor);
    i_this->mF10 = 1;
    switch ((u16)i_this->mState) {
    case 0:
        anm_init(i_this, 0x2D /* BCK_JIDANDA1 */, 10.0f, 2, 1.0f, 0xE /* BAS_JIDANDA1 */, 0);
        i_this->mState = 1;
        i_this->m2EC[0] = (s16)(gabi::ftoi(cM_rndF(5.0f)) * 0x18);
        /* fall through */
    case 1: {
        if (!(i_this->m2C4 & 7)) {
            eff_hane_set(i_this, &i_this->m1168, 1, 0);
        }
        i_this->m2584 = 2;
        s32 frame = gabi::ftoi(morf_frame(i_this->mpMorf));
        if (frame == 1 || frame == 0xD) {
            s32 index = 0;
            if (frame == 0xD) {
                index = 1;
            }
            cXyz* p = &i_this->m1174[index];
            dComIfGp_particle_set(0x8133 /* ID_IT_SN_DK_JIDANDA_ROCK00 */, p, nullptr, nullptr, 0xFF, nullptr, (s8)fopAcM_GetRoomNo(actor),
                                  gabi::at<GXColor>(gabi::ea(&i_this->m6224) + 0x98), gabi::at<GXColor>(gabi::ea(&i_this->m6224) + 0x98));
            if (i_this->m6078[1] == 0) {
                i_this->m6078[1] = 0xA;
                bdk_particle_setToon(0xA134 /* ID_IT_ST_DK_JIDANDA_SMOKE00 */, p, nullptr, nullptr, 0xB9, &i_this->m6080[1], (s8)fopAcM_GetRoomNo(actor));
            }
        }
        gabi::Local<cXyz> vec1;
        if ((u32)(frame - 4) < 8 || (u32)(frame - 0x10) < 8) {
            cLib_addCalcAngleS2(&actor->current.angle.y, fopAcM_searchPlayerAngleY(actor), 8, 0x200);
            vec1->z = fadds_ppc(REG0_F(1), 10.0f);
            if (frame == 4 || frame == 0x10) {
                bdk_StartShock(4);
            }
        } else {
            vec1->z = 0.0f;
        }
        cMtx_YrotS(calc_mtx(), fopAcM_searchPlayerAngleY(actor));
        vec1->x = 0.0f;
        vec1->y = -10.0f;
        MtxPosition(vec1, &actor->speed);
        PSVECAdd(&actor->current.pos, &actor->speed, &actor->current.pos);
        if (i_this->m2EC[0] == 0 || dist > 600.0f) {
            i_this->mAction = 3; /* ACTION_WAIT */
            i_this->mState = 0;
        }
        break;
    }
    default:
        break;
    }
}

/* kuti_attack (inlined) */
static inline void kuti_attack(bdk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused play-object read */
    s8 bVar1 = 0;
    s8 bVar2 = 0;
    i_this->m1134 = 2500;
    switch ((u16)i_this->mState) {
    case 0:
        anm_init(i_this, 0x1C /* BCK_ATTACK1 */, 5.0f, 0, 1.0f, 7 /* BAS_ATTACK1 */, 0);
        i_this->mState = 2;
        bdk_monsSeStart(actor, 0x4865 /* JA_SE_CV_DK_ATTACK */, 0);
        break;
    case 2:
        if (morf_frame(i_this->mpMorf) < 36.0f) {
            i_this->m2584 = 1;
        }
        if ((s16)(REG0_S(5) + 0x1E) == (s16)gabi::ftoi(morf_frame(i_this->mpMorf))) {
            i_this->m1138 = (s16)(REG0_S(6) + 0x1E);
            i_this->m1136 = 0;
            bdk_seStart_a(actor, 0x5872 /* JA_SE_CM_DK_ATTACK */, 0);
            bdk_StartShock(6);
            BDK_EFF27.m03C = 0;
            BDK_EFF27.m000 = 2;
            cMtx_YrotS(calc_mtx(), actor->shape_angle.y);
            gabi::Local<cXyz> vec1;
            vec1->x = 0.0f;
            vec1->y = 0.0f;
            vec1->z = 500.0f;
            MtxPosition(vec1, &BDK_EFF27.m004);
            PSVECAdd(&BDK_EFF27.m004, &actor->current.pos, &BDK_EFF27.m004);
            dComIfGp_particle_set(0x812D /* ID_IT_SN_DK_KUTI_SENKO00 */, &BDK_EFF27.m004, nullptr, nullptr, 0xFF);
            eff_hane_set(i_this, &i_this->m1168, 5, 0);
            bVar2 = 1;
        }
        if (i_this->mpMorf->isStop()) {
            anm_init(i_this, 0x33 /* BCK_NUKENAI1 */, 2.0f, 0, 1.0f, -1, 0);
            i_this->mState = 5;
            i_this->m2EC[0] = (s16)gabi::ftoi(fadds_ppc(cM_rndF(60.0f), 90.0f));
            bdk_se_pair(actor, 0x5873 /* JA_SE_CM_DK_JITABATA */, 0x4866 /* JA_SE_CV_DK_NUKEZU */);
        }
        break;
    case 3:
        if (i_this->mpMorf->isStop()) {
            i_this->mAction = 3; /* ACTION_WAIT */
            i_this->mState = 0;
        }
        break;
    case 5:
        if (i_this->mpMorf->isStop()) {
            if (i_this->m2EC[0] == 0) {
                bVar1 = 1;
            } else {
                morf_setPlaySpeed(i_this->mpMorf, -1.0f);
                i_this->mState = 6;
            }
        }
        break;
    case 6:
        if (i_this->mpMorf->isStop()) {
            morf_setPlaySpeed(i_this->mpMorf, 1.0f);
            i_this->mState = 5;
            bdk_se_pair(actor, 0x5873 /* JA_SE_CM_DK_JITABATA */, 0x4866 /* JA_SE_CV_DK_NUKEZU */);
            eff_hane_set(i_this, &i_this->m1168, 2, 0);
        }
        break;
    case 10:
        if (i_this->mpMorf->isStop()) {
            if (i_this->m2EC[0] == 0) {
                bVar1 = 1;
            } else {
                anm_init(i_this, 0x33 /* BCK_NUKENAI1 */, 2.0f, 0, 1.0f, -1, 0);
                i_this->mState = 5;
                bdk_se_pair(actor, 0x5873 /* JA_SE_CM_DK_JITABATA */, 0x4866 /* JA_SE_CV_DK_NUKEZU */);
            }
            eff_hane_set(i_this, &i_this->m1168, 2, 0);
        }
        break;
    default:
        break;
    }
    if (i_this->m2EC[2] == 1 || bVar1 != 0) {
        anm_init(i_this, 0x34 /* BCK_NUKU1 */, 1.0f, 0, 1.0f, -1, 0);
        i_this->mState = 3;
        bdk_se_pair(actor, 0x5874 /* JA_SE_CM_DK_PULL_UP_HEAD */, 0x4867 /* JA_SE_CV_DK_NUKERU */);
        BDK_EFF27.m000 = 0;
        bVar2 = 1;
    }
    if (bVar2) {
        if (i_this->m6078[2] == 0) {
            i_this->m6078[2] = 0x3C;
            bdk_particle_setToon(0xA12E /* ID_IT_ST_DK_KUTI_SMOKE00 */, &BDK_EFF27.m004, nullptr, nullptr, 0xB9, &i_this->m6080[2],
                                 (s8)fopAcM_GetRoomNo(actor));
        }
        dComIfGp_particle_set(0x812C /* ID_IT_SN_DK_KUTI_ROCK00 */, &BDK_EFF27.m004, nullptr, nullptr, 0xFF, nullptr, (s8)fopAcM_GetRoomNo(actor),
                              gabi::at<GXColor>(gabi::ea(&i_this->m6224) + 0x98), gabi::at<GXColor>(gabi::ea(&i_this->m6224) + 0x98));
    }
    actor->current.pos.y = fadds_ppc(actor->current.pos.y, actor->speed.y);
}

/* 0206D644 (matcher: fly; it is move with fly, up_fly, landing, wait, jump, kuti_attack and
 * jida_attack inlined). HD: two debug reports; no t_landing action */
void move(bdk_class* i_this) {
    WWHD_FUNC(0x0206D644, void, i_this);
    fopAc_ac_c* actor = i_this;
    gabi::store<u32>(gabi::ea(actor) + 0x39C, 4); /* attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e */
    JUTReport_bdk(0x1E, 0x168, 0x10007F68 /* "ACTION MODE  %d" */, i_this->mAction);
    JUTReport_bdk(0x1E, 0x17C, 0x10007F78 /* "MOVE MODE    %d" */, i_this->mState);
    switch ((u16)i_this->mAction) {
    case 0:
        fly(i_this);
        break;
    case 1:
        up_fly(i_this);
        break;
    case 2:
        landing(i_this);
        break;
    case 3:
        wait(i_this);
        break;
    case 4:
        jump(i_this);
        break;
    case 5:
        kuti_attack(i_this);
        break;
    case 6:
        jida_attack(i_this);
        break;
    case 7:
        gabi::call(0x0206A610, i_this); /* fly_attack */
        break;
    case 8:
        gabi::call(0x0206AE64, i_this); /* wind_attack */
        break;
    case 9:
        gabi::call(0x0206B22C, i_this); /* kamen_demo */
        break;
    case 10:
        gabi::call(0x0206BAA8, i_this); /* end */
        break;
    case 0xF:
        gabi::call(0x0206B67C, i_this); /* start */
        break;
    case 0x64:
        gabi::call(0x0206C3AC, i_this); /* t_fly */
        gabi::store<u32>(gabi::ea(actor) + 0x39C, 0);
        break;
    case 0x66:
        gabi::call(0x0206CAD8, i_this); /* t_lastattack */
        break;
    case 0x67:
        gabi::call(0x0206D0FC, i_this); /* t_down */
        gabi::store<u32>(gabi::ea(actor) + 0x39C, 0);
        break;
    case 0x6E:
        gabi::call(0x0206D638, i_this); /* after_fight */
        gabi::store<u32>(gabi::ea(actor) + 0x39C, 0);
        break;
    default:
        break;
    }
    if (i_this->m2591 != 0) {
        if (i_this->mpMorf->isStop()) {
            anm_init(i_this, i_this->m2588, 20.0f, i_this->m2590, 1.0f, i_this->m258C, 0);
            i_this->m2591 = 0;
        }
    }
}
VERIFY(0x0206D644, move);

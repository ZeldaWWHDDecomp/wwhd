/**
 * d_a_bdk_move.cpp (WWHD)
 * Boss - Helmaroc King (battle): move (the action dispatcher; fly, up_fly, landing, wait, jump,
 * kuti_attack and jida_attack are inlined into it).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdk_move.h"

using gabi::fadds_ppc;
using gabi::fmuls_ppc;
using gabi::fsubs_ppc;

// HD: word copies retain NaN payloads and interleave each load with its store.
static inline void bdk_vbits(cXyz *dst, const cXyz *src) {
    BdkMove_copyFloat(dst->x, src->x);
    BdkMove_copyFloat(dst->y, src->y);
    BdkMove_copyFloat(dst->z, src->z);
}
static inline f32 bdk_dist(bdk_class *i_this, cXyz *a, cXyz *b) {
    gabi::Local<cXyz> d;
    cXyz_mi(a, d, b);
    gabi::Local<cXyz> c;
    bdk_vbits(c, d);
    return std_sqrtf(PSVECSquareMag(c));
}
/* fopAcM_seStart + fopAcM_monsSeStart (HD inline pair: one actor and eyePos test) */
static inline void bdk_se_pair(fopAc_ac_c *a, u32 se, u32 voice) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        mDoAud_seStart(se, &a->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
        bdk_monsSeStart_e(a, voice, 0);
    }
}
#define BDK_EFF27 (i_this->m261C[0x27])

/* inlined into move */
static inline void fly(bdk_class *i_this) {
    fopAc_ac_c *actor = i_this;
    i_this->mF10 = 1;
    if (i_this->m2CA == -1) {
        anm_init(i_this, dRes_INDEX_BDK_BCK_FLY1_e, 10.0f, 2, 1.0f, dRes_INDEX_BDK_BAS_FLY1_e, 0);
        i_this->m2CA = 0;
    }
    BdkMove_copyFloat(i_this->m2E4, BdkMove_hio().m020);
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
            {
                f32 r = cM_rndFX(fadds_ppc(REG0_F(3), 3000.0f));
                i_this->m2CC.x = fadds_ppc(actor->home.pos.x, r);
            }
            i_this->m2CC.y = fadds_ppc(actor->home.pos.y, 1000.0f);
            {
                f32 r = cM_rndFX(fadds_ppc(REG0_F(3), 3000.0f));
                i_this->m2CC.z = fadds_ppc(actor->home.pos.z, r);
            }
        } else if (fc == 1) {
            actor->home.pos.x = fadds_ppc(REG0_F(0), 9800.0f);
            actor->home.pos.y = fadds_ppc(REG0_F(1), 12000.0f);
            actor->home.pos.z = fadds_ppc(REG0_F(2), -1500.0f);
            {
                f32 r = cM_rndFX(fadds_ppc(REG0_F(3), 1000.0f));
                i_this->m2CC.x = fadds_ppc(actor->home.pos.x, r);
            }
            {
                f32 r = cM_rndF(1000.0f);
                i_this->m2CC.y = fadds_ppc(actor->home.pos.y, r);
            }
            {
                f32 r = cM_rndFX(fadds_ppc(REG0_F(3), 1000.0f));
                i_this->m2CC.z = fadds_ppc(actor->home.pos.z, r);
            }
        } else if (fc == 2) {
            actor->home.pos.x = fadds_ppc(REG0_F(0), -1000.0f);
            actor->home.pos.y = fadds_ppc(REG0_F(1), 12000.0f);
            actor->home.pos.z = fadds_ppc(REG0_F(2), -2900.0f);
            {
                f32 r = cM_rndFX(fadds_ppc(REG0_F(3), 1000.0f));
                i_this->m2CC.x = fadds_ppc(actor->home.pos.x, r);
            }
            i_this->m2CC.y = fadds_ppc(actor->home.pos.y, 1000.0f);
            {
                f32 r = cM_rndFX(fadds_ppc(REG0_F(3), 1000.0f));
                i_this->m2CC.z = fadds_ppc(actor->home.pos.z, r);
            }
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
                i_this->mAction = ACTION_LANDING;
                i_this->mState = 0;
            } else {
                i_this->mAction = ACTION_FLY_ATTACK;
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

/* inlined into move */
static inline void up_fly(bdk_class *i_this) {
    fopAc_ac_c *actor = i_this;
    u8 pos_flag = 0;
    switch ((u16)i_this->mState) {
    case 0: {
        anm_init(i_this, dRes_INDEX_BDK_BCK_TOBITATU1_e, 5.0f, 0, 1.0f, dRes_INDEX_BDK_BAS_TOBITATU1_e, 0);
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
        if (gabi::ftoi(i_this->mpMorf->getFrame()) != REG0_S(7) + 0x17) {
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
            i_this->mAction = ACTION_FLY;
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

// HD: shared by the transition frame and the following approach frames.
static inline void landing_approach(bdk_class *i_this) {
    fopAc_ac_c *actor = i_this;
    cLib_addCalcPos2(&actor->current.pos, i_this->m2CC, 1.0f, actor->speedF);
    cLib_addCalcAngleS2(&actor->current.angle.z, 0, 2, 0x800);
    cLib_addCalcAngleS2(&actor->current.angle.x, 0, 2, 0x800);
    if (bdk_dist(i_this, &i_this->m2CC, &actor->current.pos) < fadds_ppc(REG0_F(3), 300.0f)) {
        i_this->mState = 3;
    }
}

/* inlined into move. HD: the final approach moves with cLib_addCalcPos2 at speedF and starts in the
 * frame the boss gets close; the drop keeps the horizontal speed (damped by 0.9) */
static inline void landing(bdk_class *i_this) {
    fopAc_ac_c *actor = i_this;
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
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
        if (!BdkMove_landAreaCheck(&i_this->m2CC, 1400.0f)) {
            i_this->mAction = ACTION_FLY;
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
            anm_init(i_this, dRes_INDEX_BDK_BCK_TYAKUTI1_e, 15.0f, 0, 0.001f, dRes_INDEX_BDK_BAS_TYAKUTI1_e,
                     0);
            pos_flag = 1;
            // HD: start the approach on the transition frame, without setting m2E0.
            landing_approach(i_this);
            break;
        }
        if (i_this->m2EC[0] == 1) {
            anm_init(i_this, dRes_INDEX_BDK_BCK_KAKKU1_e, 30.0f, 2, 1.0f, -1, 0);
        }
        break;
    case 2:
        pos_flag = 1;
        landing_approach(i_this);
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
                anm_init(i_this, dRes_INDEX_BDK_BCK_TYAKUTI1_e, 5.0f, 0, 1.0f, dRes_INDEX_BDK_BAS_TYAKUTI1_e,
                         0);
                i_this->mState = 4;
                eff_hane_num = 10;
                bdk_StartShock(5);
                dComIfGp_particle_set(ID_IT_SN_DK_TYAKUTI_ROCK00, &actor->current.pos, nullptr, nullptr, 0xFF,
                                      nullptr, (s8)fopAcM_GetRoomNo(actor), &i_this->m6224.mColorK0,
                                      &i_this->m6224.mColorK0);
                if (i_this->m6078[0] == 0) {
                    i_this->m6078[0] = 100;
                    bdk_particle_setToon(ID_IT_ST_DK_TYAKUTI_SMOKE00, &actor->current.pos, nullptr, nullptr,
                                         0xB9, &i_this->m6080[0], (s8)fopAcM_GetRoomNo(actor));
                }
            }
        }
        if (i_this->mpMorf->isStop()) {
            i_this->mAction = ACTION_WAIT;
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
static inline BOOL wait_wind_check(bdk_class *i_this, fopAc_ac_c *player) {
    f32 dz = fsubs_ppc(-4635.0f, player->current.pos.z);
    f32 dx = fsubs_ppc(4706.0f, player->current.pos.x);
    if (std_sqrtf(gabi::fmadds(dx, dx, fmuls_ppc(dz, dz))) < 400.0f) {
        return TRUE;
    }
    BOOL hit = FALSE;
    gabi::Local<BdkMoveLineCheck> linChk;
    gabi::Local<cXyz> start;
    gabi::Local<cXyz> vec;
    gabi::Local<cXyz> end;
    dBgS_LinChk_ct(linChk.get(), BdkMove_lineVtables, false);
    BdkMove_copyFloat(start->x, player->current.pos.x);
    f32 py = player->current.pos.y;
    BdkMove_copyFloat(start->y, player->current.pos.y);
    BdkMove_copyFloat(start->z, player->current.pos.z);
    vec->y = 0.0f;
    vec->x = 0.0f;
    vec->z = fadds_ppc(REG_F(18, 2), 300.0f);
    start->y = fadds_ppc(py, 50.0f);
    s16 a = 0;
    for (s32 i = 8; i != 0; i--) {
        cMtx_YrotS(calc_mtx(), a);
        MtxPosition(vec, end);
        PSVECAdd(end, start, end);
        dBgS_LinChk_Set(linChk.get(), start, end, i_this);
        if (dComIfG_Bgsp()->LineCross(reinterpret_cast<dBgS_LinChk *>(linChk.get()))) {
            hit = TRUE;
            break;
        }
        a = (s16)(a + 0x2000);
    }
    BdkMove_destroyLineCheck(linChk.get());
    return hit;
}

static inline void wait_to_up_fly(bdk_class *i_this) {
    i_this->m2594 = 0;
    i_this->m2592 = 0;
    i_this->mAction = ACTION_UP_FLY;
}

/* inlined into move. HD: turns of more than 0x1000 are made by hopping (height by the angle); a
 * wall hit while walking and the new state 10 (random) end in a take-off; the wind attack is
 * forced near a fixed point or a wall; m2DC 0 / 100 while turning */
static inline void wait(bdk_class *i_this) {
    fopAc_ac_c *actor = i_this;
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
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
            i_this->mAction = ACTION_UP_FLY;
            break;
        }
        anm_init(i_this, dRes_INDEX_BDK_BCK_WAIT1_e, 10.0f, 2, 1.0f, -1, 0);
        i_this->mState = 1;
        i_this->m2EC[0] = (s16)gabi::ftoi(fadds_ppc(cM_rndF(30.0f), 40.0f));
        actor->speedF = 0.0f;
        i_this->m2CA = 0;
        /* fall through */
    case 1:
        i_this->m2DC = 0.0f;
        if ((u32)(angle + 0x1000) >= 0x2001) {
            i_this->mState = 5;
            anm_init(i_this, dRes_INDEX_BDK_BCK_JUMP1_e, 5.0f, 0, 0.0f, dRes_INDEX_BDK_BAS_JUMP1_e, 0);
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
                    anm_init(i_this, dRes_INDEX_BDK_BCK_SENKAI_L1_e, 10.0f, 2, 1.0f,
                             dRes_INDEX_BDK_BAS_SENKAI_L1_e, 0);
                    i_this->m2EC[1] = 0x14;
                }
            } else if (angle < -0x800) {
                if (i_this->m2CA != 2) {
                    i_this->m2CA = 2;
                    anm_init(i_this, dRes_INDEX_BDK_BCK_SENKAI_R1_e, 10.0f, 2, 1.0f,
                             dRes_INDEX_BDK_BAS_SENKAI_R1_e, 0);
                    i_this->m2EC[1] = 0x14;
                }
            } else if (i_this->m2CA != 3) {
                i_this->m2CA = 3;
                anm_init(i_this, dRes_INDEX_BDK_BCK_WAIT1_e, 10.0f, 2, 1.0f, -1, 0);
                i_this->m2EC[1] = 0x14;
            }
        }
        if (i_this->m2CA != 3) {
            i_this->m2DC = 100.0f; /* HD: 100 (GameCube REG0_F(0) + 800) */
        }
        if (i_this->m2EC[0] == 0 && i_this->m2CA == 3) {
            u8 mode = BdkMove_hio().m014;
            if (mode != 0) {
                if (mode == 1) {
                    i_this->mAction = ACTION_KUTI_ATTACK;
                } else if (mode == 2) {
                    i_this->mAction = ACTION_WIND_ATTACK;
                }
                i_this->mState = 0;
            } else {
                i_this->m2592 = i_this->m2592 + 1;
                if (dist < fadds_ppc(REG0_F(11), 450.0f)) {
                    i_this->mAction = ACTION_JIDA_ATTACK;
                    i_this->mState = 0;
                } else if (dist < fadds_ppc(REG0_F(12), 550.0f)) {
                    if (wait_wind_check(i_this, player)) {
                        i_this->mAction = ACTION_WIND_ATTACK;
                    } else if (i_this->m8F8 < 2) {
                        i_this->mAction = ACTION_KUTI_ATTACK;
                    } else if (cM_rndF(1.0f) < 0.25f) {
                        i_this->mAction = ACTION_WIND_ATTACK;
                    } else {
                        i_this->mAction = ACTION_KUTI_ATTACK;
                    }
                    i_this->mState = 0;
                } else if (dist < fadds_ppc(REG0_F(15), 1300.0f)) {
                    i_this->mState = 2;
                    anm_init(i_this, dRes_INDEX_BDK_BCK_WALK1_e, 3.0f, 2, BdkMove_hio().m01C,
                             dRes_INDEX_BDK_BAS_WALK1_e, 0);
                    i_this->m2E0 = 0.0f;
                    actor->speedF = 0.0f;
                } else {
                    f32 r = cM_rndF(1.0f);
                    i_this->mState = 0;
                    if (r < 0.5f) {
                        i_this->mAction = ACTION_JUMP;
                    } else {
                        i_this->mAction = ACTION_WIND_ATTACK;
                    }
                }
            }
        }
        break;
    case 2:
        BdkMove_copyFloat(i_this->m2E4, BdkMove_hio().m018);
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
            anm_init(i_this, dRes_INDEX_BDK_BCK_JUMP1_e, 2.0f, 0, 1.0f, dRes_INDEX_BDK_BAS_JUMP1_e, 0);
            i_this->mState = 7;
        }
        break;
    case 7:
        i_this->m2DC = 0.0f;
        if (i_this->mpMorf->isStop()) {
            i_this->mState = 1;
            anm_init(i_this, dRes_INDEX_BDK_BCK_WAIT1_e, 10.0f, 2, 1.0f, -1, 0); /* HD */
        }
        break;
    case 10: /* HD */
        if (i_this->mpMorf->isStop()) {
            if (cM_rndF(1.0f) < 0.5f) {
                wait_to_up_fly(i_this);
                i_this->mState = 0;
            } else {
                i_this->mState = 1;
                anm_init(i_this, dRes_INDEX_BDK_BCK_WAIT1_e, 10.0f, 2, 1.0f, -1, 0);
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

/* inlined into move */
static inline void jump(bdk_class *i_this) {
    fopAc_ac_c *actor = i_this;
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    i_this->m2E4 = 0.0f;
    i_this->mF10 = 1;
    i_this->m2E8 = 5.0f;
    i_this->m2DC = 500.0f;
    switch ((u16)i_this->mState) {
    case 0:
        anm_init(i_this, dRes_INDEX_BDK_BCK_TOBITATU1_e, 5.0f, 0, 1.0f, dRes_INDEX_BDK_BAS_TOBITATU1_e, 0);
        i_this->m2618 = 0x19;
        i_this->mState = 1;
        bdk_vbits(&i_this->m2CC, &player->current.pos);
        break;
    case 1: {
        if (REG0_S(7) + 0x17 != gabi::ftoi(i_this->mpMorf->getFrame())) {
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
            anm_init(i_this, dRes_INDEX_BDK_BCK_TYAKUTI1_e, 15.0f, 0, 0.001f, dRes_INDEX_BDK_BAS_TYAKUTI1_e,
                     0);
            i_this->mAction = ACTION_LANDING;
            i_this->mState = 3;
        }
        break;
    default:
        break;
    }
    ground_move(i_this);
}

/* inlined into move */
static inline void jida_attack(bdk_class *i_this) {
    fopAc_ac_c *actor = i_this;
    f32 dist = fopAcM_searchPlayerDistance(actor);
    i_this->mF10 = 1;
    switch ((u16)i_this->mState) {
    case 0:
        anm_init(i_this, dRes_INDEX_BDK_BCK_JIDANDA1_e, 10.0f, 2, 1.0f, dRes_INDEX_BDK_BAS_JIDANDA1_e, 0);
        i_this->mState = 1;
        i_this->m2EC[0] = (s16)(gabi::ftoi(cM_rndF(5.0f)) * 0x18);
        /* fall through */
    case 1: {
        if (!(i_this->m2C4 & 7)) {
            eff_hane_set(i_this, &i_this->m1168, 1, 0);
        }
        i_this->m2584 = 2;
        s32 frame = gabi::ftoi(i_this->mpMorf->getFrame());
        if (frame == 1 || frame == 0xD) {
            s32 index = 0;
            if (frame == 0xD) {
                index = 1;
            }
            cXyz *p = &i_this->m1174[index];
            dComIfGp_particle_set(ID_IT_SN_DK_JIDANDA_ROCK00, p, nullptr, nullptr, 0xFF, nullptr,
                                  (s8)fopAcM_GetRoomNo(actor), &i_this->m6224.mColorK0,
                                  &i_this->m6224.mColorK0);
            if (i_this->m6078[1] == 0) {
                i_this->m6078[1] = 0xA;
                bdk_particle_setToon(ID_IT_ST_DK_JIDANDA_SMOKE00, p, nullptr, nullptr, 0xB9,
                                     &i_this->m6080[1], (s8)fopAcM_GetRoomNo(actor));
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
            i_this->mAction = ACTION_WAIT;
            i_this->mState = 0;
        }
        break;
    }
    default:
        break;
    }
}

/* inlined into move */
static inline void kuti_attack(bdk_class *i_this) {
    fopAc_ac_c *actor = i_this;
    dComIfGp_get(); /* HD: an unused play-object read */
    s8 bVar1 = 0;
    s8 bVar2 = 0;
    i_this->m1134 = 2500;
    switch ((u16)i_this->mState) {
    case 0:
        anm_init(i_this, dRes_INDEX_BDK_BCK_ATTACK1_e, 5.0f, 0, 1.0f, dRes_INDEX_BDK_BAS_ATTACK1_e, 0);
        i_this->mState = 2;
        bdk_monsSeStart(actor, JA_SE_CV_DK_ATTACK, 0);
        break;
    case 2:
        if (i_this->mpMorf->getFrame() < 36.0f) {
            i_this->m2584 = 1;
        }
        if ((s16)(REG0_S(5) + 0x1E) == (s16)gabi::ftoi(i_this->mpMorf->getFrame())) {
            i_this->m1138 = (s16)(REG0_S(6) + 0x1E);
            i_this->m1136 = 0;
            bdk_seStart_a(actor, JA_SE_CM_DK_ATTACK, 0);
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
            dComIfGp_particle_set(ID_IT_SN_DK_KUTI_SENKO00, &BDK_EFF27.m004, nullptr, nullptr, 0xFF);
            eff_hane_set(i_this, &i_this->m1168, 5, 0);
            bVar2 = 1;
        }
        if (i_this->mpMorf->isStop()) {
            anm_init(i_this, dRes_INDEX_BDK_BCK_NUKENAI1_e, 2.0f, 0, 1.0f, -1, 0);
            i_this->mState = 5;
            i_this->m2EC[0] = (s16)gabi::ftoi(fadds_ppc(cM_rndF(60.0f), 90.0f));
            bdk_se_pair(actor, JA_SE_CM_DK_JITABATA, JA_SE_CV_DK_NUKEZU);
        }
        break;
    case 3:
        if (i_this->mpMorf->isStop()) {
            i_this->mAction = ACTION_WAIT;
            i_this->mState = 0;
        }
        break;
    case 5:
        if (i_this->mpMorf->isStop()) {
            if (i_this->m2EC[0] == 0) {
                bVar1 = 1;
            } else {
                i_this->mpMorf->setPlaySpeed(-1.0f);
                i_this->mState = 6;
            }
        }
        break;
    case 6:
        if (i_this->mpMorf->isStop()) {
            i_this->mpMorf->setPlaySpeed(1.0f);
            i_this->mState = 5;
            bdk_se_pair(actor, JA_SE_CM_DK_JITABATA, JA_SE_CV_DK_NUKEZU);
            eff_hane_set(i_this, &i_this->m1168, 2, 0);
        }
        break;
    case 10:
        if (i_this->mpMorf->isStop()) {
            if (i_this->m2EC[0] == 0) {
                bVar1 = 1;
            } else {
                anm_init(i_this, dRes_INDEX_BDK_BCK_NUKENAI1_e, 2.0f, 0, 1.0f, -1, 0);
                i_this->mState = 5;
                bdk_se_pair(actor, JA_SE_CM_DK_JITABATA, JA_SE_CV_DK_NUKEZU);
            }
            eff_hane_set(i_this, &i_this->m1168, 2, 0);
        }
        break;
    default:
        break;
    }
    if (i_this->m2EC[2] == 1 || bVar1 != 0) {
        anm_init(i_this, dRes_INDEX_BDK_BCK_NUKU1_e, 1.0f, 0, 1.0f, -1, 0);
        i_this->mState = 3;
        bdk_se_pair(actor, JA_SE_CM_DK_PULL_UP_HEAD, JA_SE_CV_DK_NUKERU);
        BDK_EFF27.m000 = 0;
        bVar2 = 1;
    }
    if (bVar2) {
        if (i_this->m6078[2] == 0) {
            i_this->m6078[2] = 0x3C;
            bdk_particle_setToon(ID_IT_ST_DK_KUTI_SMOKE00, &BDK_EFF27.m004, nullptr, nullptr, 0xB9,
                                 &i_this->m6080[2], (s8)fopAcM_GetRoomNo(actor));
        }
        dComIfGp_particle_set(ID_IT_SN_DK_KUTI_ROCK00, &BDK_EFF27.m004, nullptr, nullptr, 0xFF, nullptr,
                              (s8)fopAcM_GetRoomNo(actor), &i_this->m6224.mColorK0, &i_this->m6224.mColorK0);
    }
    actor->current.pos.y = fadds_ppc(actor->current.pos.y, actor->speed.y);
}

// HD: two debug reports; no t_landing action.
/* 0206D644 */
void move(bdk_class *i_this) {
    fopAc_ac_c *actor = i_this;
    actor->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
    BdkMove_report(0x1E, 0x168, "ACTION MODE  %d", i_this->mAction);
    BdkMove_report(0x1E, 0x17C, "MOVE MODE    %d", i_this->mState);
    switch ((u16)i_this->mAction) {
    case ACTION_FLY:
        fly(i_this);
        break;
    case ACTION_UP_FLY:
        up_fly(i_this);
        break;
    case ACTION_LANDING:
        landing(i_this);
        break;
    case ACTION_WAIT:
        wait(i_this);
        break;
    case ACTION_JUMP:
        jump(i_this);
        break;
    case ACTION_KUTI_ATTACK:
        kuti_attack(i_this);
        break;
    case ACTION_JIDA_ATTACK:
        jida_attack(i_this);
        break;
    case ACTION_FLY_ATTACK:
        fly_attack(i_this);
        break;
    case ACTION_WIND_ATTACK:
        wind_attack(i_this);
        break;
    case ACTION_KAMEN_DEMO:
        kamen_demo(i_this);
        break;
    case ACTION_END:
        end(i_this);
        break;
    case ACTION_START:
        start(i_this);
        break;
    case ACTION_T_FLY:
        t_fly(i_this);
        actor->attention_info.flags = 0;
        break;
    case ACTION_T_LASTATTACK:
        t_lastattack(i_this);
        break;
    case ACTION_T_DOWN:
        t_down(i_this);
        actor->attention_info.flags = 0;
        break;
    case ACTION_AFTER_FIGHT:
        BdkMove_afterFight(i_this);
        actor->attention_info.flags = 0;
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

/* WWHD glue
str 10007F68 "ACTION MODE  %d"
str 10007F78 "MOVE MODE    %d"
ext 0206D638 void BdkMove_afterFight(bdk_class*)
ext 027EC9E8 void BdkMove_report(s32, s32, const char*, s32)
ext 02065208 BOOL BdkMove_landAreaCheck(cXyz*, f32)
ext 02008B4C void BdkMove_destroyLineBase(BdkMoveLineCheck*, s32)
*/

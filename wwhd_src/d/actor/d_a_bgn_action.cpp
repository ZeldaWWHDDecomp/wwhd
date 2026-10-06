/**
 * d_a_bgn_action.cpp (WWHD)
 * Boss - Puppet Ganon (Phase 1): action_main with the inlined dance_0 (dance_A, dance_B, ki_set),
 * punch_LR and body_attack.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bgn.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bgn.h"

void start(bgn_class* i_this);
void tail_attack(bgn_class* i_this);
void damage(bgn_class* i_this);
void head_recover(bgn_class* i_this);
void hensin(bgn_class* i_this);
void action_s(bgn_class* i_this, move_s* param_2, int param_3);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline s16 bgn_cM_rad2s_a(f32 x) { return gabi::call<s16>(0x02019510, x); }
/* fopAcM_monsSeStart (HD inline): 025E1AA4 mDoAud_monsSeStart(id, pos, actorId, param, reverb) [as in d_a_am] */
static inline void bgn_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, param, dComIfGp_getReverb(room));
    }
}
/* pose tables (this TU's statics, eight csXyz each) */
enum : u32 {
    DANCE_PAUSE_1 = 0x10461A5C, DANCE_PAUSE_2 = 0x10461A8C, DANCE_PAUSE_3 = 0x10461ABC, DANCE_PAUSE_4 = 0x10461AEC,
    PUNCH_LR1_D = 0x10461B1C, PUNCH_LR12_D = 0x10461B4C, PUNCH_LR2_D = 0x10461B7C, PUNCH_R1_D = 0x10461BAC,
    PUNCH_R2_D = 0x10461BDC, PUNCH_L1_D = 0x10461C0C, PUNCH_L2_D = 0x10461C3C,
};
static inline void bgn_pose(move_s* m, u32 tbl, s32 i) {
    u32 s = tbl + 6 * i;
    u32 d = gabi::ea(&m->m2E0);
    gabi::store<u16>(d, gabi::load<u16>(s));
    gabi::store<u16>(d + 2, gabi::load<u16>(s + 2));
    gabi::store<u16>(d + 4, gabi::load<u16>(s + 4));
}

/* dance_A (inline) */
static inline void dance_A(bgn_class* i_this) {
    fopAc_ac_c* actor = i_this;
    for (s32 i = 0; i < 8; i++) {
        move_s* tmp = &i_this->mAAA8[i];
        s16 st = i_this->mC74E;
        if ((u32)(s32)st > 2)
            continue;
        if (st == 0) {
            i_this->mC74E = (s16)(st + 1);
            i_this->mC752 = 0;
            st = 1;
        }
        if (st == 1) {
            bgn_pose(tmp, DANCE_PAUSE_1, i);
            if (i_this->mC7AC[3] == 0) {
                i_this->mC74E = (s16)(i_this->mC74E + 1);
                i_this->mC7AC[3] = l_HIO().m0DA;
            }
        } else {
            bgn_pose(tmp, DANCE_PAUSE_2, i);
            if (i_this->mC7AC[3] == 0) {
                i_this->mC74E = 1;
                i_this->mC7AC[3] = l_HIO().m0DA;
            }
        }
    }
    s16 step = i_this->mC752;
    s16 uVar2 = i_this->mC750;
    s16 now = (s16)(uVar2 + step);
    i_this->mC750 = now;
    if ((uVar2 > 0 && now <= i_this->mC752) ||
        ((u16)uVar2 > 0x8000 && (u16)now <= (u16)(i_this->mC752 + 0x8000))) {
        mDoAud_seStart(0x5973 /* JA_SE_CM_BGN_D_KAZEKIRI */, &i_this->mCA54, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
    }
    s8 health = actor->health;
    s16 target;
    if (health == 3)
        target = l_HIO().m0DC;
    else if (health == 2)
        target = l_HIO().m0DE;
    else
        target = l_HIO().m0E0;
    cLib_addCalcAngleS2(&i_this->mC752, target, 1, 0x10);
}

static inline s16 bgn_dance_B_time(fopAc_ac_c* actor) {
    s8 health = actor->health;
    if (health == 3)
        return l_HIO().m0E8;
    if (health == 2)
        return l_HIO().m0EA;
    return l_HIO().m0EC;
}

/* dance_B (inline) */
static inline void dance_B(bgn_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAcM_seStart(actor, 0x516F /* JA_SE_CM_BGN_D_DANCE */, 0);
    s16 st = i_this->mC74E;
    for (s32 i = 0; i < 8; i++) {
        move_s* tmp = &i_this->mAAA8[i];
        if ((u32)(s32)st > 1)
            continue;
        if (st == 0) {
            bgn_pose(tmp, DANCE_PAUSE_3, i);
            if (i_this->mC7AC[3] == 0) {
                i_this->mC74E = (s16)(i_this->mC74E + 1);
                i_this->mC7AC[3] = bgn_dance_B_time(actor);
            }
        } else {
            bgn_pose(tmp, DANCE_PAUSE_4, i);
            if (i_this->mC7AC[3] == 0) {
                i_this->mC74E = 0;
                i_this->mC7AC[3] = bgn_dance_B_time(actor);
            }
        }
        st = i_this->mC74E;
    }
    cLib_addCalcAngleS2(&i_this->mC750, st == 1 ? 0x1555 : -0x1555, 8, l_HIO().m0EE);
}

/* dance_0 (inline); returns false where the GameCube returns early (the head recovery starts) */
static inline bool dance_0(bgn_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (i_this->mC7AC[1] == 0) {
        i_this->mC7AC[1] = (s16)gabi::ftoi(gabi::fadds_ppc(cM_rndF(150.0f), 50.0f));
        i_this->mC758.x = cM_rndFX(1500.0f);
        i_this->mC758.z = cM_rndFX(1500.0f);
        i_this->mC76C = 0.0f;
    }
    cLib_addCalc2(&actor->current.pos.x, i_this->mC758.x, 0.05f, i_this->mC76C);
    cLib_addCalc2(&actor->current.pos.z, i_this->mC758.z, 0.05f, i_this->mC76C);
    cLib_addCalc2(&i_this->mC76C, gabi::fadds_ppc(REG0_F(19), 20.0f), 1.0f, 0.5f);
    cLib_addCalc2(&i_this->mC728.y, gabi::fmuls_ppc(cM_ssin(i_this->mC746 * (REG0_S(9) + 500)), gabi::fadds_ppc(REG0_F(9), 50.0f)), 0.5f,
                  gabi::fadds_ppc(REG0_F(8), 20.0f));
    if (i_this->mAAA8[0].m2D0 == 0 || i_this->mAAA8[1].m2D0 == 0)
        cLib_addCalcAngleS2(&actor->shape_angle.y, fopAcM_searchPlayerAngleY(actor), 10, 0x400);
    s16 a = i_this->mC74A;
    if (a == 0) {
        if (i_this->mC754 == 0) {
            i_this->mC74C = 0;
            i_this->mC7AC[0] = l_HIO().m0D8;
            /* ki_set (inline) */
            s8 health = actor->health;
            if (health == 3)
                i_this->mKeeseSpawnNum = (s8)l_HIO().mKeeseNum3HP;
            else if (health == 2)
                i_this->mKeeseSpawnNum = (s8)l_HIO().mKeeseNum2HP;
            else
                i_this->mKeeseSpawnNum = (s8)l_HIO().mKeeseNum1HP;
        } else {
            i_this->mC74C = 1;
            s8 health = actor->health;
            if (health == 3)
                i_this->mC7AC[0] = l_HIO().m0E2;
            else if (health == 2)
                i_this->mC7AC[0] = l_HIO().m0E4;
            else
                i_this->mC7AC[0] = l_HIO().m0E6;
        }
        i_this->mC74E = 0;
        i_this->mC74A = (s16)(i_this->mC74A + 1);
    } else if (a == 1) {
        if (i_this->mC7AC[0] == 0 && ki_check(i_this) == 0 && l_HIO().m028 != 0) {
            s16 n = i_this->mC754;
            if (n == 1 || n == 2 || n == 3) {
                if (i_this->mAAA8[0].m2D0 != 0 && i_this->mAAA8[1].m2D0 != 0 && i_this->mC7AC[2] == 0) {
                    i_this->mC748 = 4;
                    i_this->mC74A = 0;
                    i_this->mCA98 = 0.0f;
                    return false;
                }
                i_this->mC748 = 1;
                i_this->mC7AC[0] = (s16)(REG0_S(0) + 0x32);
                u8 u3 = i_this->mAAA8[3].m2D0;
                u8 u4 = i_this->mAAA8[4].m2D0;
                if (u3 == 0) {
                    if (u4 == 0) {
                        i_this->mC74A = 5;
                        i_this->mC7AC[0] = (s16)(REG0_S(6) + 0x40);
                    } else {
                        i_this->mC74A = 3;
                    }
                    bgn_monsSeStart(actor, 0x4971 /* JA_SE_CV_BGN_D_ATTACK */, 0);
                } else if (u4 == 0) {
                    i_this->mC74A = 1;
                    bgn_monsSeStart(actor, 0x4971 /* JA_SE_CV_BGN_D_ATTACK */, 0);
                } else if (i_this->mAAA8[7].m2D0 == 0) {
                    i_this->mC748 = 3;
                    i_this->mC74A = 0;
                } else {
                    i_this->mC748 = 2;
                    i_this->mC74A = 0;
                }
                n = i_this->mC754;
            } else if (n == 0 || n == 5) {
                i_this->mC74A = 0;
                n = i_this->mC754;
            }
            n = (s16)(n + 1);
            if (n > 5)
                i_this->mC754 = 0;
            else
                i_this->mC754 = n;
        }
    }
    s16 c = i_this->mC74C;
    if (c == 0)
        dance_A(i_this);
    else if (c == 1)
        dance_B(i_this);
    move_s* pmVar8 = i_this->mAAA8;
    gabi::Local<cXyz> local_34;
    for (s32 i = 0; i < 8; i++, pmVar8++) {
        if (pmVar8->m2D0 == 0) {
            local_34->x = gabi::fsubs_ppc((f32)(s32)(s16)pmVar8->m2E0.x, pmVar8->m2D4.x);
            local_34->y = gabi::fsubs_ppc((f32)(s32)(s16)pmVar8->m2E0.y, pmVar8->m2D4.y);
            local_34->z = gabi::fsubs_ppc((f32)(s32)(s16)pmVar8->m2E0.z, pmVar8->m2D4.z);
            f32 fVar9 = std_sqrtf(PSVECSquareMag(local_34));
            f32 t = gabi::fadds_ppc(REG0_F(0), 100.0f);
            if (fVar9 > t) {
                f32 f = gabi::fadds_ppc(gabi::fmadds(gabi::fsubs_ppc(fVar9, t), gabi::fadds_ppc(REG0_F(1), 0.06f), 50.0f), REG0_F(2));
                s16 v = (s16)gabi::ftoi(f);
                if (v > pmVar8->m2F8)
                    pmVar8->m2F8 = v;
            }
        }
    }
    return true;
}

/* the punch aim (inline in punch_LR): the arm's start, the direction to the target, the step */
static inline void bgn_punch_aim(bgn_class* i_this, cXyz* target, cXyz* from, cXyz* step) {
    gabi::Local<cXyz> local_84;
    gabi::Local<cXyz> local_6c;
    cXyz_mi(target, local_84, from);
    f32 x = local_84->x;
    f32 z = local_84->z;
    u32 m = gabi::load<u32>(0x1018C7B0);
    f32 y = local_84->y;
    cMtx_YrotS(gabi::at<Mtx34>(m), cM_atan2s(x, z));
    f32 d = gabi::fmadds(x, x, gabi::fmuls_ppc(z, z));
    m = gabi::load<u32>(0x1018C7B0);
    f32 s = std_sqrtf(d);
    cMtx_XrotM(gabi::at<Mtx34>(m), (s16)-cM_atan2s(y, s));
    local_6c->x = 0.0f;
    local_6c->y = 0.0f;
    local_6c->z = gabi::fadds_ppc(REG0_F(7), 200.0f);
    MtxPosition(local_6c, step);
}
static inline void bgn_copy_pos(cXyz* dst, cXyz* src) {
    u32 d = gabi::ea(dst), s = gabi::ea(src);
    gabi::store<u32>(d, gabi::load<u32>(s));
    gabi::store<u32>(d + 4, gabi::load<u32>(s + 4));
    gabi::store<u32>(d + 8, gabi::load<u32>(s + 8));
}
static inline part_s* bgn_rhand(bgn_class* i_this) { return &i_this->mRightArmParts[BGN_HAND_MAX() - 1]; }
static inline part_s* bgn_lhand(bgn_class* i_this) { return &i_this->mLeftArmParts[BGN_HAND_MAX() - 1]; }
/* the hit: the string's target pose is where the hand landed (s16), and the hit effect at a copy of it */
static inline void bgn_punch_land(move_s* m, bgn_class* i_this, bool right) {
    m->m2E0.x = (s16)gabi::ftoi((right ? bgn_rhand(i_this) : bgn_lhand(i_this))->m0D4.x);
    m->m2E0.y = (s16)gabi::ftoi((right ? bgn_rhand(i_this) : bgn_lhand(i_this))->m0D4.y);
    m->m2E0.z = (s16)gabi::ftoi((right ? bgn_rhand(i_this) : bgn_lhand(i_this))->m0D4.z);
}
static inline void bgn_punch_eff(bgn_class* i_this, bool right, s32 type, cXyz* tmp) {
    part_s* p = right ? bgn_rhand(i_this) : bgn_lhand(i_this);
    tmp->x = p->m0D4.x;
    tmp->y = p->m0D4.y;
    tmp->z = p->m0D4.z;
    attack_eff_set(i_this, tmp, type);
}
/* dComIfGp_getVibration().StartShock(REG0_S(2) + add, -0x21, cXyz(0, 1, 0)) */
static inline void bgn_shock(s32 add, cXyz* up) {
    dVibration_c* vib = dComIfGp_getVibration();
    up->x = 0.0f;
    up->y = 1.0f;
    up->z = 0.0f;
    StartShock(vib, REG0_S(2) + add, -0x21, up);
}

/* punch_LR (inline). HD: the hit happens in place (the GameCube collects it in bVar2) */
static inline void punch_LR(bgn_class* i_this) {
    fopAc_ac_c* actor = i_this;
    u32 player = gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER);
    cLib_addCalc0(&i_this->mC728.y, 0.5f, 20.0f);
    gabi::Local<cXyz> local_90;
    gabi::Local<cXyz> local_6c;
    gabi::Local<cXyz> tmp;
    local_90->x = gabi::at<fopAc_ac_c>(player)->current.pos.x;
    local_90->y = gabi::at<fopAc_ac_c>(player)->current.pos.y;
    local_90->z = gabi::at<fopAc_ac_c>(player)->current.pos.z;
    if (i_this->mAAA8[0].m2D0 != 0 && i_this->mAAA8[1].m2D0 != 0) {
        cMtx_YrotS(calc_mtx(), actor->shape_angle.y);
        local_6c->x = cM_rndFX(1000.0f);
        local_6c->y = 0.0f;
        local_6c->z = gabi::fadds_ppc(cM_rndF(1000.0f), 1000.0f);
        MtxPosition(local_6c, local_90);
        PSVECAdd(local_90, &actor->current.pos, local_90);
        local_90->y = 0.0f;
    }
    bool bVar3 = false;
    s32 uVar11 = 0;
    s16 a = i_this->mC74A;
    switch (a) {
    case 1:
    case 2:
        if (a == 1) {
            bVar3 = true;
            uVar11 = 2;
            i_this->mC338 = 130.0f;
            if (i_this->mC7AC[0] == 0x14) {
                bgn_copy_pos(&i_this->mC788, &i_this->mRightArmParts[BGN_HAND_MAX()].m0D4);
                bgn_punch_aim(i_this, local_90, &i_this->mC788, &i_this->mC7A0);
            }
            if (i_this->mC7AC[0] != 0)
                break;
            i_this->mC779 = 1;
            i_this->mC74A = (s16)(i_this->mC74A + 1);
            i_this->mC7AC[0] = (s16)(REG0_S(1) + 0x32);
            fopAcM_seStart(actor, 0x5971 /* JA_SE_CM_BGN_D_PUNCH_FIRE */, 0);
            i_this->mC338 = 200.0f;
            i_this->mC334 = 200.0f;
            OnAtSetBit(&bgn_rhand(i_this)->mPartSph);
        } else {
            i_this->mC338 = 200.0f;
            i_this->mC334 = 200.0f;
            OnAtSetBit(&bgn_rhand(i_this)->mPartSph);
        }
        uVar11 = 2;
        PSVECAdd(&i_this->mC788, &i_this->mC7A0, &i_this->mC788);
        i_this->mAAA8[4].m2E8 = 100.0f;
        if (i_this->mC7AC[0] == 0 || bgn_rhand(i_this)->m0D4.y < gabi::fadds_ppc(i_this->mC7BC, 30.0f)) {
            bgn_shock(5, tmp);
            i_this->mC779 = 0;
            i_this->mAAA8[4].m2F4 = gabi::fadds_ppc(REG0_F(15), 5000.0f);
            bgn_punch_land(&i_this->mAAA8[4], i_this, true);
            bgn_punch_eff(i_this, true, 1, tmp);
            i_this->mC748 = 0;
            i_this->mC74A = 0;
        }
        break;
    case 3:
    case 4:
        if (a == 3) {
            i_this->mC338 = 200.0f;
            i_this->mC334 = 200.0f;
            bVar3 = true;
            uVar11 = 1;
            if (i_this->mC7AC[0] == 0x14) {
                bgn_copy_pos(&i_this->mC77C, &bgn_lhand(i_this)->m0D4);
                bgn_punch_aim(i_this, local_90, &i_this->mC77C, &i_this->mC794);
            }
            if (i_this->mC7AC[0] != 0)
                break;
            i_this->mC74A = (s16)(i_this->mC74A + 1);
            i_this->mC7AC[0] = (s16)(REG0_S(1) + 0x32);
            i_this->mC778 = 1;
            fopAcM_seStart(actor, 0x5971 /* JA_SE_CM_BGN_D_PUNCH_FIRE */, 0);
        }
        uVar11 = 1;
        OnAtSetBit(&bgn_lhand(i_this)->mPartSph);
        PSVECAdd(&i_this->mC77C, &i_this->mC794, &i_this->mC77C);
        i_this->mC338 = 200.0f;
        if (i_this->mC7AC[0] == 0 || bgn_lhand(i_this)->m0D4.y < gabi::fadds_ppc(i_this->mC7BC, 30.0f)) {
            bgn_shock(5, tmp);
            i_this->mAAA8[3].m2F4 = gabi::fadds_ppc(REG0_F(15), 5000.0f);
            i_this->mC778 = 0;
            bgn_punch_land(&i_this->mAAA8[3], i_this, false);
            bgn_punch_eff(i_this, false, 0, tmp);
            i_this->mC748 = 0;
            i_this->mC74A = 0;
        }
        break;
    case 5:
    case 6:
        if (a == 5) {
            i_this->mC338 = 50.0f;
            bVar3 = true;
            uVar11 = 3;
            if (i_this->mC7AC[0] == 0x14) {
                bgn_copy_pos(&i_this->mC77C, &bgn_lhand(i_this)->m0D4);
                bgn_copy_pos(&i_this->mC788, &bgn_rhand(i_this)->m0D4);
                bgn_punch_aim(i_this, local_90, &i_this->mC77C, &i_this->mC794);
                bgn_punch_aim(i_this, local_90, &i_this->mC788, &i_this->mC7A0);
            }
            if (i_this->mC7AC[0] != 0)
                break;
            i_this->mC778 = 1;
            i_this->mC779 = 1;
            i_this->mC74A = (s16)(i_this->mC74A + 1);
            i_this->mC7AC[0] = (s16)(REG0_S(1) + 0x32);
            fopAcM_seStart(actor, 0x5971 /* JA_SE_CM_BGN_D_PUNCH_FIRE */, 0);
        }
        uVar11 = 3;
        OnAtSetBit(&bgn_rhand(i_this)->mPartSph);
        OnAtSetBit(&bgn_lhand(i_this)->mPartSph);
        PSVECAdd(&i_this->mC77C, &i_this->mC794, &i_this->mC77C);
        PSVECAdd(&i_this->mC788, &i_this->mC7A0, &i_this->mC788);
        i_this->mC338 = 200.0f;
        i_this->mC334 = 200.0f;
        if (i_this->mC7AC[0] == 0 || bgn_lhand(i_this)->m0D4.y < gabi::fadds_ppc(i_this->mC7BC, 30.0f)) {
            bgn_shock(8, tmp);
            i_this->mAAA8[3].m2F4 = gabi::fadds_ppc(REG0_F(15), 5000.0f);
            i_this->mAAA8[4].m2F4 = gabi::fadds_ppc(REG0_F(15), 5000.0f);
            i_this->mC778 = 0;
            i_this->mC779 = 0;
            bgn_punch_land(&i_this->mAAA8[3], i_this, false);
            bgn_punch_land(&i_this->mAAA8[4], i_this, true);
            bgn_punch_eff(i_this, true, 1, tmp);
            bgn_punch_eff(i_this, false, 0, tmp);
            i_this->mC748 = 0;
            i_this->mC74A = 0;
        }
        break;
    default:
        break;
    }
    if (uVar11 != 0) {
        if (uVar11 <= 2) {
            cLib_addCalc2(&i_this->mC32C[uVar11 - 1], gabi::fadds_ppc(REG0_F(14), 1.0f), 0.2f, 0.05f);
        } else {
            for (s32 i = 0; i < 2; i++)
                cLib_addCalc2(&i_this->mC32C[i], gabi::fadds_ppc(REG0_F(14), 1.0f), 0.2f, 0.05f);
        }
        if (bVar3 && (i_this->mAAA8[0].m2D0 == 0 || i_this->mAAA8[1].m2D0 == 0))
            cLib_addCalcAngleS2(&actor->shape_angle.y, fopAcM_searchPlayerAngleY(actor), 10, 0x400);
    }
    for (s32 i = 0; i < 8; i++) {
        move_s* m = &i_this->mAAA8[i];
        switch (i_this->mC74A) {
        case 1: bgn_pose(m, PUNCH_R1_D, i); break;
        case 2: bgn_pose(m, PUNCH_R2_D, i); break;
        case 3: bgn_pose(m, PUNCH_L1_D, i); break;
        case 4: bgn_pose(m, PUNCH_L2_D, i); break;
        case 5:
            if ((i_this->mC7AC[0] & 0x10) != 0)
                bgn_pose(m, PUNCH_LR1_D, i);
            else
                bgn_pose(m, PUNCH_LR12_D, i);
            break;
        case 6: bgn_pose(m, PUNCH_LR2_D, i); break;
        }
    }
}

static inline void bgn_strings_wave(bgn_class* i_this, s16 add) {
    for (s32 i = 0; i < 8; i++) {
        i_this->mAAA8[i].m2F8 = (s16)(REG0_S(7) + add);
        i_this->mAAA8[i].m2F4 = gabi::fadds_ppc(REG0_F(15), 5000.0f);
    }
}

/* body_attack (inline) */
static inline void body_attack(bgn_class* i_this) {
    fopAc_ac_c* actor = i_this;
    u32 player = gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER);
    gabi::Local<cXyz> local_50;
    gabi::Local<cXyz> tmp;
    cXyz_mi(&i_this->mC758, local_50, &actor->current.pos);
    local_50->y = 0.0f;
    f32 fVar10 = std_sqrtf(PSVECSquareMag(local_50));
    bool bVar2 = false;
    switch (i_this->mC74A) {
    case 0:
        i_this->mC76C = 0.0f;
        bgn_copy_pos(&i_this->mC758, &gabi::at<fopAc_ac_c>(player)->current.pos);
        i_this->mC758.y = 0.0f;
        if (i_this->mAAA8[0].m2D0 == 0 || i_this->mAAA8[1].m2D0 == 0)
            i_this->mC764 = fopAcM_searchPlayerAngleY(actor);
        i_this->mC74A = 1;
        break;
    case 1:
        bVar2 = true;
        if (fVar10 < 200.0f) {
            i_this->mC74A = 2;
            i_this->mC7AC[0] = 0x1E;
        }
        break;
    case 2:
        bVar2 = true;
        i_this->mC758.y = 500.0f;
        if (i_this->mC7AC[0] == 0) {
            i_this->mC74A = 3;
            actor->speed.y = 0.0f;
            i_this->mC770 = (s16)(REG0_S(0) + 0x14);
            bgn_strings_wave(i_this, 0x1E);
        }
        break;
    case 3:
        i_this->mC728.y = gabi::fadds_ppc(i_this->mC728.y, actor->speed.y);
        actor->speed.y = gabi::fsubs_ppc(actor->speed.y, gabi::fadds_ppc(REG0_F(4), 10.0f));
        OnAtSetBit(&i_this->mC7FC);
        OnAtSetBit(&i_this->mPelvisParts[0].mPartSph);
        if (!(i_this->mC728.y > -1000.0f)) {
            i_this->mC728.y = -1000.0f;
            actor->speed.y = -(f32)gabi::fmuls_ppc(actor->speed.y, gabi::fadds_ppc(REG0_F(19), 0.4f));
            bgn_strings_wave(i_this, 0x28);
            i_this->mC770 = (s16)(REG0_S(0) + 0x14);
            i_this->mC7AC[0] = 0x46;
            i_this->mC74A = 4;
            bgn_shock(9, tmp);
            tmp->x = i_this->mC308.x;
            tmp->y = i_this->mC308.y;
            tmp->z = i_this->mC308.z;
            attack_eff_set(i_this, tmp, 2);
        }
        break;
    case 4:
        i_this->mC728.y = gabi::fadds_ppc(i_this->mC728.y, actor->speed.y);
        actor->speed.y = gabi::fsubs_ppc(actor->speed.y, gabi::fadds_ppc(REG0_F(4), 10.0f));
        if (!(i_this->mC728.y > -1000.0f)) {
            i_this->mC728.y = -1000.0f;
            actor->speed.y = 0.0f;
        }
        if (i_this->mC7AC[0] > 10) {
            cLib_addCalc2(&i_this->mC774, gabi::fadds_ppc(REG0_F(9), 250.0f), 1.0f, gabi::fadds_ppc(REG0_F(10), 50.0f));
            for (s32 i = 0; i < 8; i++)
                cLib_addCalc2(&i_this->mAAA8[i].m2EC, gabi::fadds_ppc(REG0_F(9), 250.0f), 1.0f, gabi::fadds_ppc(REG0_F(10), 50.0f));
        }
        if (i_this->mC7AC[0] == 0) {
            i_this->mC748 = 0;
            i_this->mC74A = 0;
            bgn_strings_wave(i_this, 0x32);
        }
        break;
    }
    if (bVar2) {
        cLib_addCalc2(&actor->current.pos.x, i_this->mC758.x, 0.1f, i_this->mC76C);
        cLib_addCalc2(&actor->current.pos.z, i_this->mC758.z, 0.1f, i_this->mC76C);
        cLib_addCalc2(&i_this->mC76C, gabi::fadds_ppc(REG0_F(19), 30.0f), 1.0f, 0.5f);
        cLib_addCalc2(&i_this->mC728.y, i_this->mC758.y, 0.1f, 50.0f);
        cLib_addCalcAngleS2(&actor->shape_angle.y, i_this->mC764, 10, 0x400);
    }
}

/* 02086130 action_main. HD: the rope update of action_s runs later (020859AC, from shape_calc). */
void action_main(bgn_class* i_this) {
    WWHD_FUNC(0x02086130, void, i_this);
    gabi::Local<cXyz> local_9c;
    gabi::Local<cXyz> local_90;
    gabi::Local<cXyz> sum;
    local_9c->y = i_this->mC308.y;
    local_9c->z = i_this->mC308.z;
    local_9c->x = i_this->mC308.x;
    cLib_addCalc0(&i_this->mC774, 1.0f, 25.0f);
    u32 lines = i_this->mRedRopeMat.mpLines;
    u32 size = gabi::load<u32>(lines + 4);
    u32 pos = gabi::load<u32>(lines);
    for (s32 i = 0; i < 60; i++, pos += 0xC, size++) {
        f32 s = cM_ssin(bgn_cM_rad2s_a(gabi::fmuls_ppc((f32)i, 0.053247336f)));
        f32 dVar9 = gabi::fmuls_ppc(gabi::fmuls_ppc(s, i_this->mC774), gabi::fmuls_ppc((f32)(59 - i), 0.01666667f));
        s16 c746 = i_this->mC746;
        local_90->x = gabi::fmuls_ppc(cM_ssin(c746 * (REG0_S(3) + 300) + i * (REG0_S(4) + 2000)), dVar9);
        local_90->z = gabi::fmuls_ppc(cM_ssin(c746 * (REG0_S(5) + 0xFA) + i * (REG0_S(6) + 2000)), dVar9);
        cXyz_pl(local_9c, sum, local_90);
        gabi::store<u32>(pos, gabi::load<u32>(sum.a));
        gabi::store<u32>(pos + 4, gabi::load<u32>(sum.a + 4));
        gabi::store<u32>(pos + 8, gabi::load<u32>(sum.a + 8));
        gabi::store<u8>(size, (u8)(REG0_S(3) + 10));
        local_9c->y = gabi::fadds_ppc(local_9c->y, 50.0f);
    }
    if (l_HIO().m025 == 0) {
        s16 mode = i_this->mC748;
        bool after = true;
        switch (mode) {
        case 0:
            dance_0(i_this); /* (an early return of dance_0 also continues with move_se_set) */
            move_se_set(i_this);
            break;
        case 1:
            punch_LR(i_this);
            move_se_set(i_this);
            break;
        case 2:
            body_attack(i_this);
            move_se_set(i_this);
            break;
        case 3:
            tail_attack(i_this);
            move_se_set(i_this);
            break;
        case 4:
            head_recover(i_this);
            move_se_set(i_this);
            break;
        case 5:
            damage(i_this);
            break;
        case 6:
            hensin(i_this);
            break;
        case 7:
            start(i_this);
            break;
        case 10:
            i_this->mCC80 = 1.0f;
            i_this->mC754 = 1;
            break;
        default:
            after = false;
            if (mode > 0)
                cLib_addCalcAngleS2(&i_this->mC750, 0, 10, 0x200);
            break;
        }
        if (after && i_this->mC748 > 0)
            cLib_addCalcAngleS2(&i_this->mC750, 0, 10, 0x200);
    }
    for (s32 i = 0; i < 8; i++)
        action_s(i_this, &i_this->mAAA8[i], i);
    cLib_addCalc2(&i_this->mC334, i_this->mC338, 1.0f, 10.0f);
    i_this->mC338 = l_HIO().m0D4;
}
VERIFY(0x02086130, action_main);

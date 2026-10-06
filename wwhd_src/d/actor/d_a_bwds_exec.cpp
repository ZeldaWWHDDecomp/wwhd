/**
 * d_a_bwds_exec.cpp (WWHD)
 * Enemy - Molgera Larva: daBwds_Execute and the functions GHS inlined into it (move, ug_move,
 * pos_move, hook_on, hook_chance, fail, damage_check, easy_bg_check2, body_control).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bwds.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source. Comparisons are
 * written in the form the code tests (NaN behaviour).
 */
#include "d/actor/d_a_bwds.h"

/* this TU's functions (d_a_bwds.cpp) */
static inline void anm_init(bwds_class* i_this, int bck, f32 morf, u8 loopMode, f32 speed, int snd) {
    gabi::call(0x02105DD8, i_this, bck, morf, loopMode, speed, snd);
}
static inline void fopAcM_seStart_bw(fopAc_ac_c* a, u32 id, u32 param) { gabi::call(0x02108F10, a, id, param); }
/* fopAcM_seStart inlined: HD tests &eyePos only */
static inline void se_start(fopAc_ac_c* a, u32 id, u32 param = 0) {
    cXyz* eye = &a->eyePos;
    if (gabi::ea(eye) != 0)
        mDoAud_seStart(id, eye, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline fopAc_ac_c* daPy_getPlayerActorClass() { return dComIfGp_getPlayer(0); }
static inline s16 ftos(f32 f) { return (s16)gabi::ftoi(f); }

/* m0500 = (rnd < 0.3535) ? 0 : cM_rndFX(REG0_F(9) + 3000) */
static inline void set_spin(bwds_class* i_this) {
    s16 v = 0;
    if (!(cM_rndF(1.0f) < 0.3535f)) {
        v = ftos(cM_rndFX(REG0_F(9) + 3000.0f));
    }
    i_this->m0500 = v;
}

/* jump out towards local z = 2000 along current.angle.y */
static inline void set_jump_target(bwds_class* i_this) {
    fopAc_ac_c* actor = i_this;
    Mtx34* mtx = calc_mtx();
    gabi::Local<cXyz> local_34;
    local_34->x = 0.0f;
    local_34->y = 0.0f;
    local_34->z = 2000.0f;
    cMtx_YrotS(mtx, actor->current.angle.y);
    gabi::Local<cXyz> cStack_40;
    MtxPosition(local_34, cStack_40);
    gabi::Local<cXyz> sum;
    cXyz_pl(&actor->current.pos, sum, cStack_40);
    i_this->m02FC.copy(*sum);
}

/* pos_move (inlined) */
static inline void pos_move(bwds_class* i_this, s16 param_2) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> diff;
    cXyz_mi(&i_this->m02FC, diff, &actor->current.pos);
    gabi::Local<cXyz> local_2c;
    f32 dx = diff->x;
    f32 dy = diff->y;
    f32 dz = diff->z;
    local_2c->x = dx;
    local_2c->y = dy;
    local_2c->z = dz;
    s16 ang = cM_atan2s(dx, dz);
    s16 step = ftos(i_this->m030C * i_this->m0310);
    cLib_addCalcAngleS2(&actor->current.angle.y, (s16)(param_2 + ang), (s16)(REG0_S(3) + 5), step);
    cLib_addCalc2(&i_this->m0310, 1.0f, 1.0f, 0.05f);
    local_2c->x = 0.0f;
    local_2c->y = 0.0f;
    f32 spd = actor->speedF;
    Mtx34* mtx = calc_mtx();
    local_2c->z = spd;
    cMtx_YrotS(mtx, actor->current.angle.y);
    gabi::Local<cXyz> local_38;
    MtxPosition(local_2c, local_38);
    actor->speed.x = (f32)local_38->x;
    actor->speed.z = (f32)local_38->z;
    PSVECAdd(&actor->current.pos, &actor->speed, &actor->current.pos);
    f32 sz = actor->speed.z;
    f32 sx = actor->speed.x;
    f32 r = std_sqrtf(gabi::fmadds(sx, sx, sz * sz));
    actor->current.angle.x = (s16)-cM_atan2s(actor->speed.y, r);
}

/* ug_move (inlined) */
static inline void ug_move(bwds_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s16 sVar5 = 0;
    switch ((s16)i_this->m02F8) {
    case -10:
        anm_init(i_this, dRes_INDEX_BWDS_BCK_KOBOSS_PAKUPAKU_e, 2.0f, 2, 1.0f, -1);
        i_this->m02F8 = 3;
        i_this->m030C = REG0_F(3) + 3000.0f;
        actor->speedF = (f32)l_HIO.m010;
        {
            f32 base = REG0_F(13) + 60.0f;
            actor->speed.y = base + cM_rndF(20.0f);
        }
        actor->current.angle.y = ftos(cM_rndFX(32768.0f));
        set_jump_target(i_this);
        set_spin(i_this);
        se_start(actor, JA_SE_CM_BWD_C_JUMP_OUT);
        se_start(actor, JA_SE_CV_BWD_C_ATTACK);
        break;
    case 0:
        anm_init(i_this, dRes_INDEX_BWDS_BCK_KOBOSS_CLOSE_e, 2.0f, 0, 1.0f, -1);
        actor->current.pos.y = (f32)i_this->m0504;
        i_this->m02F8 = i_this->m02F8 + 1;
        actor->shape_angle.z = 0;
        i_this->m181C = 1;
        /* fall through */
    case 1: {
        actor->current.pos.y = (f32)i_this->m0504;
        attn_flags(actor) = 4; /* fopAc_Attn_LOCKON_BATTLE_e */
        i_this->m02FC.copy(player->current.pos);
        i_this->m030C = REG0_F(3) + 3000.0f;
        actor->speedF = (f32)l_HIO.m010;

        gabi::Local<cXyz> tmp;
        cXyz_mi(&i_this->m02FC, tmp, &actor->current.pos);
        gabi::Local<cXyz> local_34;
        local_34->copy(*tmp);

        f32 fVar8 = std_sqrtf(PSVECSquareMag(local_34));
        f32 fVar1 = fVar8 * 7.0f;
        f32 lim = l_HIO.m018;
        if (fVar1 > lim) {
            fVar1 = lim;
        }
        sVar5 = ftos(cM_ssin(i_this->m02F4 * (REG0_S(3) + 0x514)) * fVar1);

        if (fVar8 < REG0_F(12) + 600.0f) {
            anm_init(i_this, dRes_INDEX_BWDS_BCK_KOBOSS_PAKUPAKU_e, 2.0f, 2, 1.0f, -1);
            i_this->m02F8 = i_this->m02F8 + 1;
            actor->speedF = (f32)l_HIO.m010;
            actor->speed.y = REG0_F(13) + 50.0f;
            actor->current.angle.y = fopAcM_searchPlayerAngleY(actor);
            set_jump_target(i_this);
            set_spin(i_this);
            se_start(actor, JA_SE_CM_BWD_C_JUMP_OUT);
            se_start(actor, JA_SE_CV_BWD_C_ATTACK);
        }
        break;
    }
    case 2:
        i_this->m0670[0].OnAtSPrmBit(1); /* OnAtSetBit */
        i_this->m0670[0].OnCoSPrmBit(1); /* OnCoSetBit */
        /* fall through */
    case 3:
        i_this->m02FB = 1;
        actor->speed.y = actor->speed.y - (REG0_F(6) + 2.7f);
        if (!(actor->current.pos.y > i_this->m0504)) {
            i_this->m02F8 = 4;
            actor->speedF = REG0_F(5) + 5.0f;
            i_this->m0314[0] = REG0_S(3) + 0x23;
            attn_flags(actor) = 0;
            i_this->m04F5 = 2;
            se_start(actor, JA_SE_CM_BWD_C_IN_SAND);
        }
        break;
    case 4:
        i_this->m04F5 = 2;
        actor->speed.y = REG0_F(8) + -20.0f;
        if (i_this->m0314[0] == 0) {
            i_this->m02F8 = 5;
            actor->speed.y = 0.0f;
            actor->speedF = (f32)l_HIO.m010;
            i_this->m0314[0] = 0x1e;
            i_this->m0314[1] = ftos(cM_rndF(50.0f) + 120.0f);
            i_this->m0314[2] = ftos(cM_rndF(30.0f) + 30.0f);
        }
        break;
    case 5:
        actor->shape_angle.z = 0;
        i_this->m0500 = 0;
        if (i_this->m0314[0] == 0) {
            attn_flags(actor) = 4;
            i_this->m0314[0] = ftos(cM_rndF(20.0f) + 20.0f);
            f32 px = actor->current.pos.x;
            i_this->m02FC.x = px;
            i_this->m02FC.y = (f32)actor->current.pos.y;
            i_this->m02FC.z = (f32)actor->current.pos.z;
            i_this->m02FC.x = px + cM_rndFX(REG0_F(12) + 600.0f);
            f32 rz = cM_rndFX(REG0_F(12) + 600.0f);
            i_this->m02FC.z = i_this->m02FC.z + rz;
        }
        if (i_this->m0314[2] == 0) {
            i_this->m0314[2] = ftos(cM_rndF(60.0f) + 40.0f);
            f32 sy = REG0_F(14) + 30.0f;
            f32 py = actor->current.pos.y + sy;
            actor->speed.y = sy;
            actor->current.pos.y = py;
        }
        actor->speed.y = actor->speed.y - (REG0_F(15) + 3.0f);
        i_this->m030C = REG0_F(4) + 1000.0f;
        actor->speedF = (f32)l_HIO.m010;

        if (!(actor->current.pos.y > i_this->m0504)) {
            actor->current.pos.y = (f32)i_this->m0504;
            actor->speed.y = 0.0f;
            if (i_this->m0314[1] == 0) {
                i_this->m02F8 = 0;
            }
        }
        break;
    }
    pos_move(i_this, sVar5);
    if (actor->speed.y < 0.0f) {
        actor->shape_angle.z = actor->shape_angle.z + i_this->m0500;
    }
}

/* hook_on (inlined) */
static inline void hook_on(bwds_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->m031E = 3;
    switch ((s16)i_this->m02F8) {
    case 0: {
        anm_init(i_this, dRes_INDEX_BWDS_BCK_KOBOSS_CLOSE_e, 2.0f, 0, 1.0f, -1);
        i_this->m02F8 = i_this->m02F8 + 1;
        gabi::Local<cXyz> local_34;
        cXyz_mi(&player->eyePos, local_34, &actor->eyePos);
        f32 x = local_34->x;
        f32 z = local_34->z;
        f32 y = local_34->y;
        actor->current.angle.y = cM_atan2s(x, z);
        f32 r = std_sqrtf(gabi::fmadds(x, x, z * z));
        actor->current.angle.x = (s16)-cM_atan2s(y, r);
        actor->speedF = 0.0f;
        break;
    }
    case 1:
        if (!(actor->actor_status & 0x100000) /* fopAcStts_HOOK_CARRY_e */) {
            i_this->m02F6 = ACTION_HOOK_CHANCE;
            i_this->m02F8 = 0;
        }
        break;
    }
}

/* hook_chance (inlined) */
static inline void hook_chance(bwds_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused player lookup */
    i_this->m0670[0].OnCoSPrmBit(1); /* OnCoSetBit */
    switch ((s16)i_this->m02F8) {
    case 0:
        anm_init(i_this, dRes_INDEX_BWDS_BCK_KOBOSS_PAKUPAKU_e, 2.0f, 2, 1.0f, -1);
        i_this->m02F8 = i_this->m02F8 + 1;
        i_this->m0314[0] = (s16)l_HIO.m014;
        /* fall through */
    case 1: {
        f32 floor_y = (i_this->m0504 + 100.0f) + REG0_F(14);
        if (!(actor->current.pos.y > floor_y)) {
            actor->current.pos.y = floor_y;
            /* HD: speed.y = (rnd(REG18_F(10) + 20) + 20) + REG18_F(11) (GameCube REG6_F(7) + rnd(REG0_F(6) + 40)) */
            f32 r = cM_rndF(REG18_F(10) + 20.0f);
            actor->speed.y = (r + 20.0f) + REG18_F(11);
            actor->speed.x = cM_rndFX(REG0_F(8) + 5.0f);
            actor->speed.z = cM_rndFX(REG0_F(8) + 5.0f);
            s16 v = 0;
            if (!(cM_rndF(1.0f) < 0.5f)) {
                v = (s16)(REG0_S(5) + 0x7f00);
            }
            i_this->m04FC = v;

            i_this->m0500 = ftos(cM_rndFX(32768.0f));
            i_this->m0502 = ftos(cM_rndFX(REG0_F(5) + 2500.0f));
            if (cM_rndF(1.0f) < 0.5f) {
                if (cM_rndF(1.0f) < 0.5f) {
                    anm_init(i_this, dRes_INDEX_BWDS_BCK_KOBOSS_CLOSE_e, 5.0f, 0, 1.0f, -1);
                } else {
                    anm_init(i_this, dRes_INDEX_BWDS_BCK_KOBOSS_PAKUPAKU_e, 5.0f, 2, 1.0f, -1);
                }
            }
        }
        if (i_this->m0314[0] == 0) {
            i_this->m02F6 = ACTION_UG_MOVE;
            i_this->m02F8 = 3;
        }
        break;
    }
    }

    actor->current.angle.y = actor->current.angle.y + i_this->m0502;
    cLib_addCalcAngleS2(&actor->shape_angle.z, i_this->m0500, 1, (s16)(REG0_S(6) + 0x800));
    cLib_addCalcAngleS2(&actor->current.angle.x, i_this->m04FC, 1, (s16)(REG0_S(6) + 0x800));
    i_this->m04F5 = 1;
    PSVECAdd(&actor->current.pos, &actor->speed, &actor->current.pos);
    actor->speed.y = actor->speed.y - (REG0_F(4) + 5.0f);
    i_this->m02FB = 1;
}

/* fail (inlined) */
static inline void fail(bwds_class* i_this) {
    fopAc_ac_c* actor = i_this;
    daPy_getPlayerActorClass(); /* unused */

    i_this->m031E = 3;
    i_this->m04F5 = 1;
    switch ((s16)i_this->m02F8) {
    case 0: {
        gabi::Local<cXyz> local_34;
        local_34->x = 0.0f;
        local_34->y = REG0_F(19) + 60.0f;
        local_34->z = REG0_F(20) + -30.0f;
        Mtx34* mtx = calc_mtx();
        s16 angle = fopAcM_searchPlayerAngleY(actor);
        cMtx_YrotS(mtx, angle);
        MtxPosition(local_34, &actor->speed);
        i_this->m02F8 = i_this->m02F8 + 1;
        i_this->m0314[0] = REG0_S(4) + 10;
    } /* fall through */
    case 1:
        if (i_this->m0314[0] == 0) {
            i_this->m0314[0] = REG0_S(5) + 1;
            gabi::Local<cXyz> local_40;
            cXyz& src = i_this->m032C[REG0_S(7) - i_this->m04F0 + 0xc];
            local_40->x = (f32)src.x;
            local_40->y = (f32)src.y;
            local_40->z = (f32)src.z;
            gabi::Local<cXyz> local_34;
            local_34->x = 0.5f;
            local_34->y = 0.5f;
            local_34->z = 0.5f;

            gabi::Local<csXyz> local_54;
            csXyz_ct(local_54, 0, 0, 0);
            local_54->x = ftos(cM_rndF(65536.0f));
            local_54->y = ftos(cM_rndF(65536.0f));
            dComIfGp_particle_set(0x825C /* ID_IT_SN_BWK_SIBOUBAKUEN00 */, local_40, local_54, local_34);
            dComIfGp_particle_set(0x825B /* ID_IT_SN_BWK_SIBOUFLASH00 */, local_40, local_54, local_34);
            i_this->m04F0 = i_this->m04F0 + 1;
            if (i_this->m04F0 >= REG0_S(8) + 0xd) {
                i_this->m02F8 = i_this->m02F8 + 1;
            }
        }
        break;
    case 2:
        if (!(actor->current.pos.y > ((i_this->m0504 + 100.0f) + REG0_F(14)) + 5.0f)) {
            fopAcM_createDisappear(actor, &actor->current.pos, 10, 10 /* daDisItem_HEART_e */, 0xFF);
            fopAcM_delete(actor);
        }
        break;
    }
    PSVECAdd(&actor->current.pos, &actor->speed, &actor->current.pos);
    actor->speed.y = actor->speed.y - (REG0_F(4) + 5.0f);

    f32 floor_y = (i_this->m0504 + 100.0f) + REG0_F(14);
    if (!(actor->current.pos.y > floor_y)) {
        actor->current.pos.y = floor_y;
        actor->speed.x = 0.0f;
        actor->speed.y = 0.0f;
        actor->speed.z = 0.0f;
    } else {
        actor->current.angle.y = cM_atan2s(actor->speed.x, actor->speed.z);
        f32 sz = actor->speed.z;
        f32 sx = actor->speed.x;
        f32 r = std_sqrtf(gabi::fmadds(sx, sx, sz * sz));
        actor->current.angle.x = (s16)-cM_atan2s(actor->speed.y, r);
    }
}

/* damage_check (inlined) */
static inline void damage_check(bwds_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = daPy_getPlayerActorClass();
    if (i_this->m031E == 0) {
        if (actor->actor_status & 0x100000 /* fopAcStts_HOOK_CARRY_e */) {
            gabi::Local<cXyz> local_38;
            local_38->x = 0.0f;
            local_38->y = 0.0f;
            local_38->z = 0.0f;
            /* daPy_py_c::setHookshotCarryOffset (virtual, HD vtable +0x10C) */
            u32 fn = gabi::load<u32>(player->__vtbl + 0x10C);
            gabi::call_ptr(fn, player, fopAcM_GetID(actor), local_38.get());
            i_this->m02F6 = ACTION_HOOK_ON;
            i_this->m02F8 = 0;
            i_this->m031E = 10;
            se_start(actor, JA_SE_LK_HS_SPIKE, 0x20);
            se_start(actor, JA_SE_CV_BWD_C_HS_DAMAGE);
        } else {
            i_this->m0508.Move();
            gabi::Local<CcAtInfo_bw> local_2c;
            local_2c->pParticlePos = 0;
            if (i_this->m0670[0].ChkTgHit()) {
                if (i_this->m02FB == 1) {
                    i_this->m031E = 6;
                    local_2c->mpObj = gabi::ea(i_this->m0670[0].GetTgHitObj());
                    local_2c->pParticlePos = gabi::ea(i_this->m0670[0].GetTgHitPosP());
                    cc_at_check(actor, local_2c);

                    if (actor->health <= 0) {
                        i_this->m02F6 = ACTION_FAIL;
                        i_this->m02F8 = 0;
                        se_start(actor, JA_SE_CV_BWD_C_DIE);
                    } else {
                        /* HD: knockback away from the player (GameCube: m0320 = REG0_F(11) + 80, m0328 = REG0_S(2) + 7) */
                        i_this->m0324 = fopAcM_searchPlayerAngleY(actor);
                        i_this->m0320 = REG10_F(11) + 40.0f;
                        i_this->m0328 = REG10_S(2) + 0xF;
                        se_start(actor, JA_SE_CV_BWD_C_DAMAGE);
                    }
                }
            }
            for (s32 i = 3; i < 0xf; i++) {
                if (i_this->m0670[i].ChkTgHit()) {
                    def_se_set(actor, i_this->m0670[i].GetTgHitObj(), 0x40);
                    break;
                }
            }
            if (i_this->m04F4 != 0) {
                fopAcM_delete(actor);
            }
        }
    }
}

/* move (inlined) */
static inline void move(bwds_class* i_this) {
    fopAc_ac_c* actor = i_this;
    switch ((s16)i_this->m02F6) {
    case ACTION_UG_MOVE:
        ug_move(i_this);
        break;
    case ACTION_HOOK_ON:
        hook_on(i_this);
        break;
    case ACTION_HOOK_CHANCE:
        hook_chance(i_this);
        break;
    case ACTION_FAIL:
        fail(i_this);
        break;
    }

    i_this->m02FA = !(actor->current.pos.y > i_this->m0504 + 20.0f);
    damage_check(i_this);
    i_this->m02FB = 0;

    f32 k = i_this->m0320;
    if (k > 0.01f) {
        /* HD: backwards along the knockback yaw (GameCube: forwards along yaw m0324 and pitch m0326) */
        gabi::Local<cXyz> local_1c;
        local_1c->y = 0.0f;
        Mtx34* mtx = calc_mtx();
        local_1c->x = 0.0f;
        local_1c->z = -k;
        cMtx_YrotS(mtx, i_this->m0324);
        gabi::Local<cXyz> cStack_28;
        MtxPosition(local_1c, cStack_28);
        PSVECAdd(&actor->current.pos, cStack_28, &actor->current.pos);
        cLib_addCalc0(&i_this->m0320, 1.0f, 7.0f);
    }
}

/* easy_bg_check2 (inlined) */
static inline void easy_bg_check2(bwds_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_54;
    for (s32 i = 0; i < 0x14; i++) {
        local_54->z = (f32)actor->current.pos.z;
        local_54->x = (f32)actor->current.pos.x;
        local_54->y = 0.0f;
        f32 fVar3 = std_sqrtf(PSVECSquareMag(local_54));
        if (!(fVar3 > 3800.0f)) {
            break;
        }
        actor->current.pos.x = actor->current.pos.x * 0.99f;
        actor->current.pos.z = actor->current.pos.z * 0.99f;
    }
}

/* body_control (inlined) */
static inline void body_control(bwds_class* i_this) {
    fopAc_ac_c* actor = i_this;
    f32 fVar1 = 0.0f;
    J3DModel* model = i_this->mpMorf->getModel();
    PSMTXCopy(bw_getAnmMtx(model, 2 /* KOBOSS_HEAD_JNT_HEAD_e */), calc_mtx());
    gabi::Local<cXyz> vec1;
    vec1->z = (f32)REG0_F(2);
    vec1->y = (f32)REG0_F(1);
    vec1->x = (REG0_F(0) + 60.0f) * l_HIO.m008;
    MtxPosition(vec1, &i_this->m032C[0]);
    i_this->m03E0[0].x = (s16)actor->shape_angle.x;
    i_this->m03E0[0].y = (s16)actor->shape_angle.y;
    i_this->m03E0[0].z = (s16)actor->shape_angle.z;

    cMtx_YrotS(calc_mtx(), actor->shape_angle.y);
    cMtx_XrotM(calc_mtx(), actor->shape_angle.x);

    vec1->y = 0.0f;
    vec1->x = 0.0f;
    gabi::Local<cXyz> vec3;
    if (i_this->m04F5 != 0) {
        /* HD: REG18 registers, -30 (GameCube REG0_F(6) - 10) */
        vec1->z = (REG18_F(6) + -30.0f) * l_HIO.m008;
        fVar1 = REG18_F(5) + 0.85f;
    } else {
        vec1->z = (REG18_F(17) + -5.0f) * l_HIO.m008;
    }
    MtxPosition(vec1, vec3);
    /* HD: no vec3.y adjustment while hooked */

    gabi::Local<dBgS_GndChk> gndChk;
    dBgS_GndChk_ct(gndChk, BW_GNDCHK_VT, false);
    /* HD: the trail stiffness is geometric: (REG18_F(7) + 1) * (REG18_F(8) + 0.9)^i
     * (GameCube (0xe - i) * (REG0_F(17) + 0.5) + 1) */
    f32 fVar2 = REG18_F(7) + 1.0f;
    gabi::Local<cXyz> vec2; /* one frame slot for all iterations */
    for (s32 i = 1; i < 0xf; i++) {
        cXyz* p032c = &i_this->m032C[i];
        csXyz* p03e0 = &i_this->m03E0[i];
        cXyz* p043c = &i_this->m043C[i];
        fVar2 = fVar2 * (REG18_F(8) + 0.9f);
        f32 fVar3 = gabi::fmadds(vec3->y, fVar2, p032c->y + p043c->y);
        if (i_this->m04F5 == 1) {
            cXyz* pos = gabi::at<cXyz>(gabi::ea(gndChk.get()) + 0x24);
            f32 py = p032c->y + 200.0f;
            pos->x = (f32)p032c->x;
            pos->y = py;
            pos->z = (f32)p032c->z;
            f32 g = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
            f32 dVar8 = (((g + -90.0f) + REG0_F(16)) + 100.0f) + REG0_F(19);
            if (!(fVar3 > dVar8)) {
                fVar3 = dVar8;
            }
        }
        f32 diff_x = gabi::fmadds(vec3->x, fVar2, (p032c->x - p032c[-1].x) + p043c->x);
        f32 diff_z = gabi::fmadds(vec3->z, fVar2, (p032c->z - p032c[-1].z) + p043c->z);
        f32 diff_y = fVar3 - p032c[-1].y;

        s16 sVar2 = cM_atan2s(diff_x, diff_z);
        s16 sVar1 = (s16)-cM_atan2s(diff_y, std_sqrtf(gabi::fmadds(diff_x, diff_x, diff_z * diff_z)));

        Mtx34* mtx = calc_mtx();
        vec1->x = 0.0f;
        f32 len = (REG0_F(7) + 70.0f) * l_HIO.m008;
        vec1->y = 0.0f;
        vec1->z = len;
        cMtx_YrotS(mtx, sVar2);
        cMtx_XrotM(calc_mtx(), sVar1);
        MtxPosition(vec1, vec2);

        p03e0[-1].y = sVar2;
        p03e0[-1].x = (s16)(sVar1 + 0x8000);

        p043c->copy(*p032c);
        f32 nx = p032c[-1].x + vec2->x;
        p032c->x = nx;
        p032c->y = p032c[-1].y + vec2->y;
        p032c->z = p032c[-1].z + vec2->z;
        p043c->x = (nx - p043c->x) * fVar1;
        p043c->y = (p032c->y - p043c->y) * fVar1;
        p043c->z = (p032c->z - p043c->z) * fVar1;

        if (i < 0xe) {
            J3DModel* iVar5 = i_this->mp02BC[i - 1];
            mDoMtx_stack_c::transS(p032c->x, p032c->y, p032c->z);
            mDoMtx_stack_c::YrotM(p03e0->y);
            mDoMtx_XrotM(mDoMtx_stack_c::get(), p03e0->x);
            f32 s = cM_ssin(i_this->m02F4 * (REG0_S(5) + 3500) + i * (REG0_S(6) + 10000));
            f32 fVar4 = gabi::fmadds(s, REG0_F(4) + 0.1f, 1.0f);
            f32 fVar5 = fVar4 * l_HIO.m008;
            mDoMtx_stack_c::scaleM(fVar5, fVar5, l_HIO.m008);
            J3DModel_setBaseTRMtx(iVar5, mDoMtx_stack_c::get());
        }
        if (i >= 3) {
            i_this->m0670[i].SetC(p032c);
            i_this->m0670[i].SetR((REG0_F(9) + 10.0f) * l_HIO.m008);
            dComIfG_Ccsp_Set(&i_this->m0670[i]);
        }
    }
    bw_GndChk_dt(gndChk);
}

/* 0210602C */
static BOOL daBwds_Execute(bwds_class* i_this) {
    WWHD_FUNC(0x0210602C, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused player lookup */

    gabi::Local<cXyz> local_bc;
    gabi::Local<cXyz> cStack_c8;

    i_this->m02F4 = i_this->m02F4 + 1;
    for (s32 i = 0; i < 5; i++) {
        if (i_this->m0314[i] != 0) {
            i_this->m0314[i] = i_this->m0314[i] - 1;
        }
    }
    if (i_this->m031E != 0) {
        i_this->m031E = i_this->m031E - 1;
    }
    if (i_this->m0328 != 0) {
        i_this->m0328 = i_this->m0328 - 1;
    }

    if (l_HIO.m005 == 0) {
        i_this->m0670[0].OffAtSPrmBit(1); /* OffAtSetBit */
        i_this->m0670[0].OffCoSPrmBit(1); /* OffCoSetBit */
        move(i_this);
        easy_bg_check2(i_this);

        i_this->m1810.x = (f32)actor->current.pos.x;
        i_this->m1810.z = (f32)actor->current.pos.z;
        gabi::Local<dBgS_GndChk> gndChk;
        dBgS_GndChk_ct(gndChk, BW_GNDCHK_VT, false);
        {
            cXyz* pos = gabi::at<cXyz>(gabi::ea(gndChk.get()) + 0x24);
            f32 off = REG0_F(13) + 2500.0f;
            f32 x = actor->current.pos.x;
            f32 y = actor->current.pos.y + off;
            f32 z = actor->current.pos.z;
            pos->x = x;
            pos->y = y;
            pos->z = z;
        }
        f32 fVar11 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
        if (fVar11 < 850.0f && fVar11 != -1000000000.0f /* -G_CM3D_F_INF */) {
            i_this->m1804.y = fVar11;
            i_this->m1810.y = fVar11;
            /* HD: -90 folded (GameCube (fVar11 - 60) - 30) */
            i_this->m0504 = (f32)gabi::ftoi((fVar11 + -90.0f) + REG0_F(16));
        }

        i_this->mpMorf->play(&actor->eyePos, 0, 0);
        bw_GndChk_dt(gndChk);
    }

    actor->shape_angle.y = (s16)actor->current.angle.y;
    actor->shape_angle.x = (s16)actor->current.angle.x;
    J3DModel* pJVar8 = i_this->mpMorf->getModel();
    mDoMtx_stack_c::transS(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z);

    f32 fVar3 = (f32)(s32)i_this->m0328 * 500.0f;
    s16 s1 = ftos(cM_ssin(i_this->m02F4 * 0x2100) * fVar3);
    s16 s2 = ftos(cM_scos(i_this->m02F4 * 0x2300) * fVar3);
    mDoMtx_stack_c::YrotM((s16)(actor->shape_angle.y + s1));
    mDoMtx_XrotM(mDoMtx_stack_c::get(), (s16)(actor->shape_angle.x + s2));
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), actor->shape_angle.z);
    mDoMtx_stack_c::scaleM(l_HIO.m008, l_HIO.m008, l_HIO.m008);

    J3DModel_setBaseTRMtx(pJVar8, mDoMtx_stack_c::get());
    i_this->mpMorf->calc();
    PSMTXCopy(bw_getAnmMtx(pJVar8, 2 /* KOBOSS_HEAD_JNT_HEAD_e */), calc_mtx());

    local_bc->set(REG0_F(0), REG0_F(1), REG0_F(2));
    MtxPosition(local_bc, &actor->eyePos);

    if (i_this->m02FA != 0) {
        actor->eyePos.y = actor->eyePos.y + 90.0f;
    }

    {
        cXyz* att = gabi::at<cXyz>(gabi::ea(actor) + 0x390); /* attention_info.position */
        f32 ey = actor->eyePos.y;
        att->z = (f32)actor->eyePos.z;
        att->x = (f32)actor->eyePos.x;
        att->y = ey + 10.0f;
    }

    local_bc->set(REG0_F(0) + 50.0f, REG0_F(1), REG0_F(2));
    MtxPosition(local_bc, cStack_c8);
    i_this->m0670[0].SetC(cStack_c8);
    f32 r;
    if (i_this->m02FA != 0) {
        cStack_c8->y = cStack_c8->y + 60.0f;
        r = REG0_F(11) + 40.0f;
    } else if (i_this->m04F5 == 1) {
        r = REG0_F(11) + 80.0f;
    } else {
        r = REG0_F(10) + 60.0f;
    }
    i_this->m0670[0].SetR(r * l_HIO.m008);

    dComIfG_Ccsp_Set(&i_this->m0670[0]);
    i_this->m0544.SetR(l_HIO.m01C * l_HIO.m008);
    i_this->m0544.SetC(cStack_c8);
    dComIfG_Ccsp_Set(&i_this->m0544);
    body_control(i_this);
    i_this->m04F5 = 0;

    local_bc->set(0.0f, 0.0f, 0.0f);
    for (s32 i = 0; i < 0xd - i_this->m04F0; i++) {
        J3DModel* model = i_this->mp02BC[i];
        PSMTXCopy(J3DModel_getBaseTRMtx(model), calc_mtx());
        MtxPosition(local_bc, cStack_c8);
        dComIfGp_particle_setSimple(0x8240 /* ID_IT_SN_O_BWK_BODY_SUNA00 */, cStack_c8);
    }

    if (i_this->m181C != 0) {
        if (i_this->m181C == 1) {
            i_this->m181C = i_this->m181C + 1;
            dPa_smokeEcallBack_end(smoke(&i_this->m1820[0])); /* remove() */
            dComIfGp_particle_setToon(0xA247 /* ID_IT_ST_BWK_IDOU_MOKO00 */, &i_this->m1810, &actor->shape_angle, nullptr, 0xFF,
                                      &i_this->m1820[0], fopAcM_GetRoomNo(actor));
            smoke_setColor(&i_this->m1820[0], eff_col);
            dComIfGp_particle_set(0xA248 /* ID_IT_ST_BWK_IDOU_SUNA00 */, &i_this->m1810, &actor->shape_angle, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)&i_this->m1880, fopAcM_GetRoomNo(actor));
        }
        if (i_this->m181C < 0 || actor->current.pos.y > i_this->m1804.y) {
            i_this->m181C = 0;
            dPa_smokeEcallBack_end(smoke(&i_this->m1820[0]));
            dPa_followEcallBack_end(&i_this->m1880);
        }
        if (i_this->m181C != 0) {
            fopAcM_seStart_bw(actor, JA_SE_CM_BWD_C_MOVE_SAND, 0);
        }
    }

    if (i_this->m02F6 == 0) {
        if (i_this->m181D == 0) {
            if (actor->current.pos.y > i_this->m1804.y && !(actor->old.pos.y > i_this->m1804.y)) {
                if (i_this->m1894[0] == 0) {
                    i_this->m1894[0] = 1;
                    i_this->m1898[0].copy(actor->current.pos);
                }
                i_this->m1804.x = (f32)actor->current.pos.x;
                i_this->m1804.z = (f32)actor->current.pos.z;
                i_this->m181D = REG0_S(5) + 5;
                dPa_smokeEcallBack_end(smoke(&i_this->m1820[1]));
                s8 room = fopAcM_GetRoomNo(actor);
                u8 alpha = gabi::load<u8>(eff_col); /* eff_col.r */
                dComIfGp_particle_setToon(0xA24A /* ID_IT_ST_BWK_OUT_SMOKE00 */, &i_this->m1804, nullptr, nullptr, alpha,
                                          &i_this->m1820[1], room);
                smoke_setColor(&i_this->m1820[1], eff_col);
                dComIfGp_particle_set(0x8249 /* ID_IT_SN_BWK_OUT_SUNA00 */, &i_this->m1804);
            }
        } else {
            i_this->m181D = i_this->m181D - 1;
            if (i_this->m181D == 0) {
                dPa_smokeEcallBack_end(smoke(&i_this->m1820[1]));
            }
        }

        if (i_this->m181E == 0) {
            if (actor->current.pos.y < i_this->m1804.y && !(actor->old.pos.y < i_this->m1804.y)) {
                if (i_this->m1894[1] == 0) {
                    i_this->m1894[1] = 1;
                    i_this->m1898[1].copy(actor->current.pos);
                }
                dPa_smokeEcallBack_end(smoke(&i_this->m1820[2]));
                i_this->m1804.x = (f32)actor->current.pos.x;
                i_this->m1804.z = (f32)actor->current.pos.z;
                i_this->m181E = REG0_S(5) + 5;
                s8 room = fopAcM_GetRoomNo(actor);
                u8 alpha = gabi::load<u8>(eff_col);
                dComIfGp_particle_setToon(0xA24C /* ID_IT_ST_BWK_DIVE_SMOKE00 */, &i_this->m1804, nullptr, nullptr, alpha,
                                          &i_this->m1820[2], room);
                smoke_setColor(&i_this->m1820[2], eff_col);
                dComIfGp_particle_set(0x824B /* ID_IT_SN_BWK_DIVE_SUNA00 */, &i_this->m1804);
            }
        } else {
            i_this->m181E = i_this->m181E - 1;
            if (i_this->m181E == 0) {
                dPa_smokeEcallBack_end(smoke(&i_this->m1820[2]));
            }
        }
    }

    for (s32 i = 0; i < 2; i++) {
        if (i_this->m1894[i] != 0) {
            f32 fVar14 = (f32)(s32)i_this->m1894[i] - 1.0f;
            i_this->mp18B0[i]->setFrame(fVar14);
            gabi::store<f32>(gabi::ea(i_this->mp18B8[i].get()) + 4, fVar14); /* setFrame */
            gabi::store<f32>(gabi::ea(i_this->mp18C0[i].get()) + 4, fVar14);
            mDoMtx_stack_c::transS(i_this->m1898[i].x, i_this->m1898[i].y, i_this->m1898[i].z);
            mDoMtx_stack_c::scaleM(l_HIO.m00C, l_HIO.m00C, l_HIO.m00C);
            J3DModel_setBaseTRMtx(i_this->mp18B0[i]->getModel(), mDoMtx_stack_c::get());
            i_this->mp18B0[i]->calc();
            s8 n = (s8)(i_this->m1894[i] + 1);
            if (n > 60) {
                n = 0;
            }
            i_this->m1894[i] = n;
        }
    }
    return TRUE;
}
VERIFY(0x0210602C, daBwds_Execute);

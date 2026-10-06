/**
 * d_a_bmd_move.cpp (WWHD)
 * Boss - Kalle Demos: move() (020B1DE4) with its inlined states (wait, attack_1, attack_2,
 * damage, eat, start, end, damage_check).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bmd.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bmd.h"

#define ARC_BMD STR(0x10009A8C) /* "Bmd" */

/* daPy_py_c virtuals (HD vtable at +0xB4) */
static inline void player_setThrowDamage(fopAc_ac_c* p, cXyz* pos, s16 angle, f32 speedF, f32 speedY, s32 mode) {
    u32 fn = gabi::load<u32>(gabi::load<u32>(gabi::ea(p) + 0xB4) + 0x12C);
    gabi::call_ptr(fn, p, pos, angle, speedF, speedY, mode);
}
static inline void player_setPlayerPosAndAngle(fopAc_ac_c* p, cXyz* pos, s16 angle) {
    u32 fn = gabi::load<u32>(gabi::load<u32>(gabi::ea(p) + 0xB4) + 0x114);
    gabi::call_ptr(fn, p, pos, angle);
}
static inline f32 REG8_F(int i) { return REG_F(8, i); }
static inline f32 REG10_F(int i) { return REG_F(10, i); }
static inline f32 REG17_F(int i) { return REG_F(17, i); }

/* inline helpers of this unit */
static inline s16 frame_s16(mDoExt_McaMorf* m) { return (s16)gabi::ftoi(m->getFrame()); }

static inline void wait(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> local_30;
    gabi::Local<cXyz> local_3c;

    if (i_this->m308[3] == 1) {
        fopAcM_seStart(actor, 0x780D /* JA_SE_CM_BKM_BODY_UP */, 0);
    }
    switch (i_this->m302) {
    case -1:
        i_this->m336 = actor->shape_angle.y;
        i_this->m308[1] = 100;
        i_this->m308[2] = 0x28;
        if (i_this->m308[0] == 0) {
            i_this->m302 = 0;
            i_this->m304 = 0;
            anm_init(i_this, 0x20 /* HANA_WAIT */, 20.0f, 2, 1.0f, -1);
            /* (the inline's actor check is folded: the actor was dereferenced above) */
            if (gabi::ea(&actor->eyePos) != 0) {
                mDoAud_seStart(0x780C /* JA_SE_CM_BKM_VINE_OUT */, &actor->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            }
            i_this->m308[3] = 0x5A;
        }
        break;
    case 0: {
        f32 hx = actor->home.pos.x;
        cXyz_fcopy(&i_this->m318, &actor->home.pos);
        i_this->m940 = -0x4000;
        i_this->m302 = 1;
        f32 r = cM_rndFX(500.0f);
        i_this->m318.x = hx + r;
        r = cM_rndFX(500.0f);
        i_this->m318.z = i_this->m318.z + r;
        {
            gabi::Local<cXyz> tmp;
            cXyz_mi(&i_this->m318, tmp, &player->current.pos);
            local_3c->x = tmp->x;
            local_3c->y = tmp->y;
            local_3c->z = tmp->z;
        }
        f32 fVar7 = std_sqrtf(gabi::fmadds(local_3c->x, local_3c->x, local_3c->z * local_3c->z));
        if (fVar7 > REG0_F(11) + 1000.0f) {
            local_30->y = 0.0f;
            local_30->x = 0.0f;
            local_30->z = REG0_F(12) + 1300.0f;
            Mtx34* m = calc_mtx();
            s16 a = cM_atan2s(local_3c->x, local_3c->z);
            cMtx_YrotS(m, a);
            MtxPosition(local_30, local_3c);
            gabi::Local<cXyz> tmp;
            cXyz_pl(&player->current.pos, tmp, local_3c);
            i_this->m318.copy(*tmp);
        }
    }
        /* fallthrough */
    case 1: {
        f32 tmp = (f32)i_this->m331 * 0.15f;
        cLib_addCalc2(&actor->speedF, tmp, 1.0f, 0.1f);
        {
            gabi::Local<cXyz> t;
            cXyz_mi(&i_this->m318, t, &actor->current.pos);
            local_30->x = t->x;
            local_30->y = t->y;
            local_30->z = t->z;
        }
        cLib_addCalcAngleS2(&actor->current.angle.y, cM_atan2s(local_30->x, local_30->z), 0x10, 0x800);
        f32 fVar7 = std_sqrtf(gabi::fmadds(local_30->x, local_30->x, local_30->z * local_30->z));
        if (fVar7 < 50.0f) {
            i_this->m302 = 2;
            i_this->m308[0] = (s16)gabi::ftoi(cM_rndF(150.0f) + 150.0f);
        }
        break;
    }
    case 2:
        i_this->m330 = 1;
        cLib_addCalc0(&actor->speedF, 1.0f, 0.1f);
        if (i_this->m308[0] == 0) {
            i_this->m302 = 0;
        }
        break;
    }
    if (i_this->m302 >= 0) {
        move1(i_this);
        if (i_this->m308[1] == 0) {
            i_this->m308[1] = (s16)gabi::ftoi(cM_rndF(200.0f) + 100.0f);
            i_this->m336 = (s16)gabi::ftoi(cM_rndFX(32768.0f));
            i_this->m338 = 0.0f;
        }
        cLib_addCalcAngleS2(&actor->shape_angle.y, i_this->m336, 0x20, (s16)gabi::ftoi(i_this->m338));
        cLib_addCalc2(&i_this->m338, 64.0f, 1.0f, 1.0f);
        if (i_this->m308[2] == 0) {
            f32 dVar6 = fopAcM_searchPlayerDistance(actor);
            if (dVar6 < 1300.0f) {
                i_this->m308[2] = (s16)gabi::ftoi(cM_rndF(50.0f) + 30.0f);
                if (cM_rndF(1.0f) < l_HIO().m08) {
                    i_this->mMode = 1;
                    i_this->m302 = 0;
                }
            } else {
                i_this->m308[2] = (s16)gabi::ftoi(cM_rndF(50.0f) + 30.0f);
                if (cM_rndF(1.0f) < l_HIO().m08) {
                    i_this->mMode = 2;
                    i_this->m302 = 0;
                }
            }
        }
    }
}

static inline void attack_1(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused player fetch */
    switch (i_this->m302) {
    case 0:
        i_this->m332 = 1;
        i_this->m308[0] = 0x32;
        i_this->m302 = 1;
        break;
    case 1:
        if (i_this->m308[0] == 0) {
            i_this->mMode = 0;
            i_this->m302 = 0;
        }
        break;
    }
    cLib_addCalc0(&actor->speedF, 1.0f, 0.1f);
    move1(i_this);
}

static inline void attack_2(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused player fetch */
    switch (i_this->m302) {
    case 0:
        i_this->m332 = 2;
        i_this->m302 = 1;
        i_this->m334 = 10;
        anm_init(i_this, 0x16 /* HANA_ATTACK */, 20.0f, 2, 1.0f, -1);
        break;
    case 1:
        if (i_this->m334 == 0) {
            i_this->mMode = 0;
            i_this->m302 = 0;
            i_this->m332 = 0;
            anm_init(i_this, 0x20 /* HANA_WAIT */, 20.0f, 2, 1.0f, -1);
        }
        break;
    }
    cLib_addCalc0(&actor->speedF, 1.0f, 0.1f);
    move1(i_this);
}

/* dComIfGp_particle_setToon (HD: dPa_control_c::set group 2); the room is read before the
 * play object */
static inline JPABaseEmitter* particle_setToon(u16 id, cXyz* pos, u8 alpha, dPa_smokeEcallBack_l* cb, s8 room) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, nullptr, nullptr, alpha, (dPa_levelEcallBack*)cb, room, nullptr, nullptr, nullptr);
}
/* emitter->setGlobalRTMatrix(mtx): JPASetRMtxTVecfromMtx(mtx, mGlobalRot (+0x1F0), mGlobalTrs (+0x22C)) */
static inline void emitter_setGlobalRTMatrix(JPABaseEmitter* e, Mtx34* m) {
    gabi::call(0x028249B0, m, gabi::ea(e) + 0x1F0, gabi::ea(e) + 0x22C);
}

static inline void damage(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_40;
    gabi::Local<cXyz> local_4c;
    s8 bVar1 = false;
    local_40->x = 0.0f;
    local_40->y = 0.0f;
    local_40->z = 0.0f;
    switch (i_this->m302) {
    case 0:
        actor->speed.y = 0.0f;
        actor->gravity = 0.0f;
        i_this->m302 = 1;
        i_this->m308[0] = 0x28;
        /* fallthrough */
    case 1:
        if (i_this->m308[0] == 0x1E) {
            i_this->m332 = 3;
            actor->gravity = -3.0f;
        }
        if (!(actor->current.pos.y > i_this->m328 + 5.0f)) {
            anm_init(i_this, 0x1C /* HANA_OTIRU */, 2.0f, 0, 1.0f, -1);
            i_this->m304 = 1;
            i_this->m302 = 2;
            i_this->m93C = fopAcM_searchPlayerAngleY(actor);
            i_this->mBD8 = 2000.0f;
            dComIfGp_particle_set(0x80F3 /* ID_AK_SN_BKMOPENHOUSHI00 */, &actor->current.pos);
            if (dComIfGp_getStartStageName0() == 'X') {
                mDoAud_bgmStart(0x80000151 /* JA_BGM_UNK_151 */);
            } else {
                mDoAud_bgmStart(0x80000105 /* JA_BGM_UNK_105 */);
            }
        }
        break;
    case 2:
        if ((s16)(REG0_S(1) + 10) == frame_s16(i_this->mpBodyMorf) &&
            fopAcM_searchPlayerDistance(actor) < REG0_F(8) + 400.0f) {
            i_this->m314 = 0x1E;
            i_this->m904 = REG0_S(2) + 0x14;
        }
        if (frame_s16(i_this->mpBodyMorf) == 30) {
            for (s32 i = 0; i < 5; i++) {
                s8 room = fopAcM_GetRoomNo(actor);
                JPABaseEmitter* emitter = particle_setToon(0xA0F4 /* ID_AK_ST_BKMOPENSMOKE00 */, &actor->current.pos, 0xB9,
                                                           &i_this->mSmokeCb[i], room);
                if (emitter != nullptr) {
                    J3DModel* model = i_this->mpBodyMorf->getModel();
                    emitter_setGlobalRTMatrix(emitter, bl_getAnmMtx(model, gabi::load<s32>(0x101919DC + 4 * i) /* jno */));
                }
            }
        }
        if (frame_s16(i_this->mpBodyMorf) == 10) {
            i_this->mBD8 = 2000.0f;
        }
        if (i_this->mpBodyMorf->isStop()) {
            anm_init(i_this, 0x21 /* HIRAKU_WAIT */, 5.0f, 2, 1.0f, -1);
            i_this->m302 = 3;
            i_this->m308[0] = l_HIO().m14;
            i_this->m940 = 0;
        }
        if (frame_s16(i_this->mpBodyMorf) > 25) {
            cLib_addCalc2(&i_this->mBDC, 1.0f, 1.0f, 0.1f);
        }
        break;
    case 3:
        i_this->mB71 = 2;
        if (i_this->m308[0] == 0) {
            i_this->m302 = 4;
            anm_init(i_this, 0x1F /* HANA_TOJIRU */, 5.0f, 0, 1.0f, -1);
            i_this->m304 = 2;
            fopAcM_seStart(actor, 0x780A /* JA_SE_CM_BKM_FLW_WAVING */, 0);
        }
        cLib_addCalc2(&i_this->mBDC, 1.0f, 1.0f, 0.1f);
        break;
    case 4: {
        if (frame_s16(i_this->mpBodyMorf) == 23) {
            fopAcM_seStart(actor, 0x780F /* JA_SE_CM_BKM_BODY_CLOSE */, 0);
            i_this->m310 = 200; /* HD */
        }
        if (frame_s16(i_this->mpBodyMorf) == 27) {
            dComIfGp_particle_set(0x8100 /* ID_AK_SN_BKMCLOSEHOUSHI00 */, &actor->current.pos);
            if (dComIfGp_getStartStageName0() == 'X') {
                mDoAud_bgmStart(0x8000004A /* JA_BGM_PAST_BKM */);
            } else {
                mDoAud_bgmStart(0x80000005 /* JA_BGM_KINDAN_BOSS */);
            }
        }
        if (frame_s16(i_this->mpBodyMorf) <= 15) {
            cLib_addCalc2(&i_this->mBDC, 1.0f, 1.0f, 0.1f);
        }
        if (frame_s16(i_this->mpBodyMorf) == 15) {
            i_this->mBD8 = 2000.0f;
        }
        s16 sVar3;
        if (frame_s16(i_this->mpBodyMorf) < (s16)(REG0_S(5) + 0x14)) {
            sVar3 = REG0_S(3) + 200;
            i_this->m942 = 0;
        } else {
            i_this->m304 = 3;
            sVar3 = REG0_S(4) + 0x290;
            if ((i_this->m942 != 0) && (REG0_S(9) == 0)) {
                i_this->m942 = i_this->m942 - 1;
                i_this->mMode = 5;
                i_this->m302 = 0;
                return;
            }
        }
        cLib_addCalcAngleS2(&i_this->m940, -0x4000, 1, sVar3);
        if (i_this->mpBodyMorf->isStop()) {
            i_this->m302 = 5;
            i_this->m308[0] = 0x1E;
            bVar1 = true;
        }
        break;
    }
    case 5:
        cLib_addCalcAngleS2(&i_this->m940, -0x4000, 1, 0x290);
        if (i_this->m308[0] == 0) {
            i_this->mMode = 0;
            i_this->m302 = -1;
            i_this->m308[0] = 0x3C;
            i_this->m310 = 0x46; /* HD */
            i_this->m332 = 4;
            bVar1 = true;
        }
        break;
    }
    if ((i_this->m302 >= 2) && (i_this->m302 <= 4)) {
        i_this->mA88[0] = 3;
    }
    cLib_addCalcAngleS2(&actor->shape_angle.x, 0, 0x10, 0x200);
    cLib_addCalcAngleS2(&actor->shape_angle.z, 0, 0x10, 0x200);
    cMtx_YrotS(calc_mtx(), actor->current.angle.y);
    local_40->y = 0.0f;
    local_40->x = 0.0f;
    local_40->z = actor->speedF;
    MtxPosition(local_40, local_4c);
    actor->current.pos.x = actor->current.pos.x + local_4c->x;
    actor->current.pos.z = actor->current.pos.z + local_4c->z;
    actor->current.pos.y = actor->current.pos.y + actor->speed.y;
    actor->speed.y = actor->speed.y + actor->gravity;
    if (actor->current.pos.y < i_this->m328) {
        actor->current.pos.y = i_this->m328;
        if (actor->speed.y < -20.0f) {
            gabi::Local<cXyz> shock;
            shock->x = 0.0f;
            shock->y = 1.0f;
            shock->z = 0.0f;
            dVibration_c* vib = dComIfGp_getVibration();
            gabi::call(0x025CB374, vib, REG0_S(2) + 5, -0x21, shock.get()); /* dVibration_c::StartShock */
            if (gabi::ea(&actor->eyePos) != 0) {
                mDoAud_seStart(0x780E /* JA_SE_CM_BKM_BODY_FALL */, &actor->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            }
            actor->speed.y = 10.0f;
            dComIfGp_particle_set(0x80F1 /* ID_AK_SN_BKMDROPDOWNSOIL00 */, &actor->current.pos);
            s8 room = fopAcM_GetRoomNo(actor);
            particle_setToon(0xA0F2 /* ID_AK_ST_BKMDROPDOWNSMOKE00 */, &actor->current.pos, 0xB9, &i_this->mSmokeCb[5], room);
        } else {
            actor->speed.y = 0.0f;
        }
        cLib_addCalc0(&actor->speedF, 1.0f, 0.2f);
    }
    if (bVar1 && fopAcM_searchPlayerDistance(actor) < REG8_F(8) + 200.0f) {
        i_this->mMode = 5;
        i_this->m302 = 0;
        anm_init(i_this, 0x1F /* HANA_TOJIRU */, 1.0f, 0, 5.0f, -1);
    }
}

static inline void eat(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    switch (i_this->m302) {
    case 0:
        i_this->mB74 = 1;
        i_this->m302 = 1;
        i_this->mB72 = 0x96;
        i_this->m304 = 2;
        /* fallthrough */
    case 1:
        cLib_addCalcAngleS2(&i_this->m940, -0x4000, 1, 0x1000);
        if (i_this->mpBodyMorf->isStop()) {
            i_this->m302 = 2;
            anm_init(i_this, 0x1A /* HANA_EAT */, 10.0f, 2, 1.0f, -1);
            i_this->m308[0] = 0x32;
            if (dComIfGp_getStartStageName0() == 'X') {
                mDoAud_bgmStart(0x8000004A /* JA_BGM_PAST_BKM */);
            } else {
                mDoAud_bgmStart(0x80000005 /* JA_BGM_KINDAN_BOSS */);
            }
            fopAcM_seStart(actor, 0x7810 /* JA_SE_CM_BKM_BODY_EATING */, 0);
        }
        break;
    case 2:
        if (i_this->m308[0] != 0) {
            return;
        }
        i_this->m302 = 3;
        anm_init(i_this, 0x1B /* HANA_HAKIDASU */, 2.0f, 0, 1.0f, -1);
        fopAcM_seStart(player, 0x584F /* JA_SE_CM_BKM_LINK_OUT */, 0);
        return;
    case 3:
        if (i_this->mB78 == REG0_S(2) + 0x50) {
            dComIfGp_particle_set(0x80F8 /* ID_AK_SN_BKMHAKIDASHISMOKE00 */, &actor->current.pos);
            dComIfGp_particle_set(0x80F7 /* ID_AK_SN_BKMHAKIDASHIHOUSHI01 */, &actor->current.pos);
            i_this->m314 = 100;
        }
        if (i_this->mB78 == 0x55) {
            gabi::Local<cXyz> sp18;
            sp18->x = actor->current.pos.x;
            f32 y = actor->current.pos.y;
            sp18->y = y;
            sp18->z = actor->current.pos.z;
            sp18->y = y + (REG17_F(0) + 1000.0f);
            /* HD: the throw angle is +0x8000 (GameCube +0x5000) */
            player_setThrowDamage(player, sp18, (s16)((i_this->mB96 + REG0_S(6)) + 0x8000), REG17_F(1) + 50.0f, REG17_F(2) + 120.0f, 4);
            i_this->m904 = REG0_S(2) + 0x14;
        }
        if (i_this->mpBodyMorf->isStop()) {
            i_this->m302 = 4;
        }
        break;
    case 4:
        if (i_this->mB76 > (s16)(REG0_S(7) + 0x82)) {
            i_this->m302 = 5;
            i_this->m308[0] = 0x1A;
        }
        break;
    case 5:
        if (fopAcM_searchPlayerDistanceXZ(actor) < 300.0f) {
            gabi::Local<cXyz> sp30;
            gabi::Local<cXyz> sp24;
            cMtx_YrotS(calc_mtx(), i_this->mB96);
            sp30->x = 0.0f;
            sp30->y = 0.0f;
            sp30->z = 350.0f;
            MtxPosition(sp30, sp24);
            PSVECAdd(sp24, &actor->current.pos, sp24); /* sp24 += current.pos */
            player_setPlayerPosAndAngle(player, sp24, player->shape_angle.y);
        }
        if (i_this->m308[0] == 0) {
            i_this->mMode = 0;
            i_this->m302 = -1;
            i_this->m308[0] = 0x3C;
            i_this->mB74 = 0x96;
            dComIfGp_event_reset();
            i_this->m940 = -0x4000;
            i_this->m332 = 4;
            i_this->m314 = 0;
            i_this->mB72 = 1;
        }
        break;
    }
}

static inline void start_posMk(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    i_this->m93C = fopAcM_searchPlayerAngleY(actor);
    cMtx_YrotS(calc_mtx(), i_this->m93C);
    gabi::Local<cXyz> local_28;
    local_28->x = 0.0f;
    local_28->y = REG0_F(1) + 20.0f;
    local_28->z = REG0_F(2) + 90.0f;
    MtxPosition(local_28, &i_this->m2E0);
    PSVECAdd(&i_this->m2E0, &actor->current.pos, &i_this->m2E0); /* m2E0 += current.pos */
}

static inline void start(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get();
    i_this->m310 = 8;
    switch (i_this->m302) {
    case 0: {
        anm_init(i_this, 0x21 /* HIRAKU_WAIT */, 1.0f, 2, 1.0f, -1);
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(ARC_BMD, 0x11 /* COA_START1 */, SAFESTRING_VTBL);
        i_this->mpHeadMorf->setAnm(anm, 2, 1.0f, 1.0f, 0.0f, -1.0f, nullptr);
        i_this->m2DC = 1;
        i_this->m302 = 1;
        i_this->m304 = 2;
    }
        /* fallthrough */
    case 1:
        i_this->mBDC = 1.0f;
        start_posMk(i_this);
        i_this->m2FA = i_this->m93C;
        /* HD: REG10_F(0) + 700 (GameCube 500) */
        if (fopAcM_searchPlayerDistance(actor) < REG10_F(0) + 700.0f) {
            i_this->m302 = 2;
            i_this->mB74 = 5;
            i_this->m940 = -0x4000;
            mDoAud_bgmStreamPlay();
            i_this->m304 = 0; /* HD */
        }
        break;
    case 2:
        i_this->m314 = 0x1E;
        start_posMk(i_this);
        i_this->mBDC = 1.0f;
        i_this->m2FA = i_this->m93C;
        break;
    case 3:
        i_this->m314 = 0x1E;
        if (i_this->mB76 < 0xFA) {
            i_this->mBDC = 1.0f;
        }
        if (i_this->mB76 == 0x78) {
            anm_init(i_this, 0x1D /* HANA_START1 */, 1.0f, 2, 1.0f, -1);
            fopAcM_seStart(actor, 0x780A /* JA_SE_CM_BKM_FLW_WAVING */, 0);
        }
        if (i_this->mB76 == 0x96) {
            anm_init(i_this, 0x1E /* HANA_START2 */, 10.0f, 0, 1.0f, -1);
            i_this->mBD8 = 2000.0f;
            fopAcM_seStart(actor, 0x780B /* JA_SE_CM_BKM_FLW_TO_BUD */, 0);
        }
        if (i_this->mB76 == 0xB1) {
            dComIfGp_particle_set(0x8100 /* ID_AK_SN_BKMCLOSEHOUSHI00 */, &actor->current.pos);
        }
        if (i_this->mB76 >= 0x96) {
            if (i_this->m331 > 0) {
                move1(i_this);
            }
            if (i_this->mB76 == 0xBE) {
                anm_init(i_this, 0x20 /* HANA_WAIT */, 20.0f, 2, 1.0f, -1);
            }
        }
        if (i_this->mB76 == 0xD2) {
            i_this->m332 = 5;
            fopAcM_seStart(actor, 0x780C /* JA_SE_CM_BKM_VINE_OUT */, 0);
        }
        if (i_this->mB76 == 0x1C2) {
            i_this->m332 = 6;
            fopAcM_seStart(actor, 0x584C /* JA_SE_CM_BKM_ATKVINE_ENTER */, 0);
        }
        if (i_this->mB76 == 0x168) {
            fopAcM_seStart(actor, 0x780D /* JA_SE_CM_BKM_BODY_UP */, 0);
        }
        break;
    }
}

/* fopAcM_seStartCurrent (HD inline: null checks on the actor and its position) */
static inline void fopAcM_seStartCurrent(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->current.pos) != 0)
        mDoAud_seStart(id, &a->current.pos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}

static inline void end(bmd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->m304 = 1;
    i_this->m940 = 0;
    switch (i_this->m302) {
    case 0:
        if (dComIfGp_getStartStageName0() == 'X') {
            gabi::call(0x02587EFC, 0, (s32)fopAcM_GetRoomNo(actor)); /* dLib_setNextStageBySclsNum */
            mDoAud_seStart(0x2888 /* JA_SE_LK_B_BOSS_WARP */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            gabi::call(0x025B8B68, save_bl() + 0x644, 0x3220);  /* dComIfGs_onEventBit(KALLE_DEMOS_TRIALS_CLEAR) */
            gabi::call(0x025B8B68, save_bl() + 0x1178, 0x480);  /* dComIfGs_onTmpBit(UNK_0480) */
            return;
        }
        mDoAud_bgmStreamPrepare(0xC0000003 /* JA_STRM_BOSS_CLEAR */);
        anm_init(i_this, 0x17 /* HANA_DEAD1 */, 10.0f, 2, 1.0f, -1);
        i_this->m332 = 8;
        i_this->m302 = 1;
        i_this->m314 = 30000;
        /* fallthrough */
    case 1:
        if (i_this->mB76 < 0x1A4) {
            i_this->mBDC = 1.0f;
        }
        if (i_this->mB76 == REG0_S(2) + 300) {
            gabi::store<u32>(gabi::ea(player) + 0x430, 0x1D); /* player->changeDemoMode(DEMO_UNK_029_e) */
        }
        if (i_this->mB76 == REG0_S(4) + 0x17C) {
            anm_init(i_this, 0x18 /* HANA_DEAD2 */, 10.0f, 2, 1.0f, -1);
            fopAcM_seStartCurrent(actor, 0x7815 /* JA_SE_CM_BKM_END_FLW_WAVING */, 0);
        }
        if (i_this->mB76 == REG0_S(5) + 0x1C2) {
            anm_init(i_this, 0x19 /* HANA_DEAD3 */, 10.0f, 0, 1.0f, -1);
            fopAcM_seStartCurrent(actor, 0x7816 /* JA_SE_CM_BKM_END_FLW_LIFTUP */, 0);
            i_this->m332 = 9;
            i_this->mBD8 = 2000.0f;
            for (s32 i = 0; i < 5; i++) {
                JPABaseEmitter* emitter = dComIfGp_particle_set(0x80FF /* ID_AK_SN_BKMHANAKARERU00 */, &actor->current.pos);
                if (emitter != nullptr) {
                    J3DModel* model = i_this->mpBodyMorf->getModel();
                    emitter_setGlobalRTMatrix(emitter, bl_getAnmMtx(model, gabi::load<s32>(0x101919F0 + 4 * i) /* jno */));
                }
            }
        }
        if (i_this->mB76 == 0x230) {
            fopAcM_seStartCurrent(actor, 0x7817 /* JA_SE_CM_BKM_END_FLW_DIE */, 0);
        }
        if (i_this->mB76 == REG0_S(6) + 0x276) {
            dComIfGs_onDungeonItem_bl(3); /* dComIfGs_onStageBossEnemy */
            gabi::call(0x025D9874, &actor->current.pos, 0, (s32)fopAcM_GetRoomNo(actor), 0); /* fopAcM_createWarpFlower */
            i_this->mB71 = 0;
            i_this->m302 = 2;
        }
    case 2:
        break;
    }
}

/* 020B1DE4 */
void move(bmd_class* i_this) {
    WWHD_FUNC(0x020B1DE4, void, i_this);
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused player fetch */
    gabi::store<u32>(gabi::ea(actor) + 0x39C, 0); /* attention_info.flags (fopAcM_OffStatus(actor, 0): nothing) */
    i_this->m332 = 0;
    switch (i_this->mMode) {
    case 0:
        wait(i_this);
        break;
    case 1:
        attack_1(i_this);
        break;
    case 2:
        attack_2(i_this);
        break;
    case 3:
        damage(i_this);
        break;
    case 5:
        eat(i_this);
        break;
    case 10:
        start(i_this);
        break;
    case 11:
        end(i_this);
        break;
    }
    i_this->mAcch.CrrPos(dComIfG_Bgsp());
    if (i_this->mMode != 3) {
        f32 fVar1 = 0.0f;
        if (0 < i_this->m331) {
            fVar1 = gabi::fmadds((f32)i_this->m331, REG0_F(17) + 2.5f, 250.0f) + REG0_F(18);
            if (i_this->mMode == 10) {
                fVar1 += 100.0f;
            }
        }
        cLib_addCalc2(&actor->current.pos.y, (actor->home.pos.y + i_this->m324) + fVar1, 0.1f, 5.0f);
        dComIfGp_get(); /* HD: an unused player fetch */
        /* damage_check */
        if ((i_this->m331 == 0) && (i_this->m312 == 1)) {
            i_this->mMode = 3;
            i_this->m302 = 0;
            fopAcM_monsSeStart(actor, 0x4854 /* JA_SE_CV_BKM_CUT_VINE_ALL */, 0);
        }
    } else {
        i_this->m324 = 0.0f;
    }
}
VERIFY(0x020B1DE4, move);

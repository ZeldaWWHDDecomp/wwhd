/**
 * d_a_ks_exec.cpp (WWHD)
 * Enemy - Morth: daKS_Execute with the inlined action functions and naraku_check.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ks.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_ks.h"

#include <cmath>

enum { fpcNm_GM_e = 0xCC, fpcNm_KS_e = 0xCD };
enum {
    JA_SE_LK_LAST_HIT = 0x2828,
    JA_SE_CV_KS_ATTACK = 0x486D,
    JA_SE_CV_KS_DAMAGE = 0x486E,
    JA_SE_CV_KS_BLOW = 0x486F,
    JA_SE_CM_KS_MOVE = 0x587A,
    JA_SE_CM_KS_ATTACK = 0x587B,
};
enum : u32 { daPyStts0_SWIM_e = 0x100000 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02041C30-style: enemy_ice(enemyice*) 020402C8 (as in d_a_cc_exec) */
static inline BOOL enemy_ice(enemyice_l* e) { return gabi::call<BOOL>(0x020402C8, e); }
static inline void ripple_remove(void* r) { gabi::call(0x025A9270, r); } /* 025A9270 dPa_rippleEcallBack::end */
/* 025D5418 fopAcM_setRoomLayer(actor, s8 roomNo) */
static inline void fopAcM_setRoomLayer(fopAc_ac_c* a, s32 roomNo) { gabi::call(0x025D5418, a, roomNo); }
/* HD fopAcM_seStart inline where the actor is known non-null (only &eyePos is checked) */
static inline void fopAcM_seStart_nn(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0)
        mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb): HD fopAcM_monsSeStart inline */
static inline void monsSeStart_nn(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0) {
        s32 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, param, reverb);
    }
}
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr) monsSeStart_nn(a, id, param);
}
/* dComIfGp_checkPlayerStatus0(0, flag): play+0x5CD8 */
static inline bool dComIfGp_checkPlayerStatus0(u32 flag) { return (gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0) & flag) != 0; }
/* daPy_lk_c virtuals (HD vtable at +0xB4): getModelJointMtx +0xFC, checkFrontRoll +0x5C */
static inline u32 daPy_getModelJointMtx(fopAc_ac_c* link, u16 jnt) {
    return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(link) + 0xB4) + 0xFC), link, (u32)jnt);
}
static inline BOOL daPy_checkFrontRoll(fopAc_ac_c* link) {
    return gabi::call_ptr<BOOL>(gabi::load<u32>(gabi::load<u32>(gabi::ea(link) + 0xB4) + 0x5C), link);
}
/* u32 pl_harituki_joint_dt[20] (0x101B886C); the joint number is passed as u16 */
static inline u16 pl_harituki_joint_dt(s32 i) { return gabi::load<u16>(0x101B886C + 4 * i + 2); }
static inline f32 REG8_F(int i) { return REG_F(8, i); }
static inline f32 REG12_F(int i) { return REG_F(12, i); }

static inline void clear_m2F0(ks_class* i_this) {
    for (int i = 0; i < 5; i++) {
        i_this->m2F0[i] = 0;
    }
}

/* action_dousa_move (inlined) */
static inline void action_dousa_move(ks_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* player (unused) */
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();

    switch (i_this->mMode) {
    case 0:
        i_this->m30C = 0.0f;
        actor->gravity = -3.0f;
        clear_m2F0(i_this);
        i_this->mMode = i_this->mMode + 1;
        // Fall-through
    case 1: {
        cLib_addCalcAngleS2(&actor->shape_angle.z, 0, 1, 0x1000);

        if (fopAcM_searchPlayerDistance(actor) > 10000.0f) {
            break;
        }

        if (actor->shape_angle.z > 0x100) {
            return;
        }

        actor->shape_angle.z = 0;

        fopAcM_seStart_nn(actor, JA_SE_CM_KS_MOVE, 0);

        if (tyaku_check(i_this) != 0) {
            actor->gravity = -3.0f;
            /* HD: speed.y is not re-read after cM_rndF */
            actor->speed.y = 1.0f;
            actor->speed.y = 1.0f + cM_rndF(5.0f);
        }

        s16 ang = fopAcM_searchPlayerAngleY(actor);
        actor->current.angle.y = ang + i_this->m2FC;

        i_this->mMode = i_this->mMode + 1;
        // Fall-through
    }
    case 2: {
        if (i_this->mAcch.ChkGroundHit()) {
            s16 ang = fopAcM_searchPlayerAngleY(actor);
            actor->current.angle.y = ang + i_this->m2FC;
        }

        if ((u32)(s32)KUTTUKU_ALL_COUNT() < 0x14 && (link->speedF > 12.0f || HEAVY_IN()) &&
            fopAcM_searchPlayerDistance(actor) < 500.0f && !dComIfGp_checkPlayerStatus0(daPyStts0_SWIM_e) &&
            tyaku_check(i_this)) {
            i_this->mAction = 1;
            i_this->mMode = 10;
            return;
        }

        /* GHS: `ble` on speedF > 12 (taken on NaN) */
        if (!(link->speedF > 12.0f) || dComIfGp_checkPlayerStatus0(daPyStts0_SWIM_e)) {
            cLib_addCalc0(&actor->speedF, 0.3f, cM_rndF(1.0f) + 0.3f);
            cLib_addCalcAngleS2(&actor->shape_angle.z, 0, 1, 0x1000);
            i_this->m2FC = 0;
            break;
        }

        if (i_this->m2FC == 0) {
            f32 id = (f32)(fopAcM_GetID(actor) & 0xf);
            i_this->m2FC = (s16)gabi::ftoi(id * cM_rndFX(512.0f));
        }

        actor->shape_angle.z = (s16)gabi::ftoi(cM_rndFX(2000.0f));

        speed_keisan(i_this, actor->current.angle.y);

        i_this->m30C = 10.0f;
        actor->speedF = 12.0f;
        i_this->m2F0[0] += 1000;

        if (tyaku_check(i_this)) {
            fopAcM_seStart_nn(actor, JA_SE_CM_KS_MOVE, 0);
            actor->gravity = -3.0f;
            actor->speed.y = 1.0f;
            actor->speed.y = 1.0f + cM_rndF(5.0f);
        }
        break;
    }
    case 3: {
        if (fopAcM_searchPlayerDistance(actor) < 500.0f) {
            actor->current.angle.y = fopAcM_searchPlayerAngleY(actor);
        }
        break;
    }
    }

    if (i_this->mMode == 2 || i_this->mMode == 3) {
        cLib_addCalcAngleS2(&actor->shape_angle.y, actor->current.angle.y, 1, 0x1000);
    }

    if (body_atari_check(i_this) != 0) {
        fopAcM_seStart(actor, JA_SE_LK_LAST_HIT, 0);
    }
}

/* action_kougeki_move (inlined) */
static inline void action_kougeki_move(ks_class* i_this) {
    fopAc_ac_c* actor = i_this;
    f32 head_top_y = gabi::load<f32>(gabi::ea(dComIfGp_getPlayer(0)) + 0x3DC); /* getHeadTopPos().y */

    i_this->m31C = 20.0f;

    if (head_top_y > actor->current.pos.y) {
        i_this->m31C = 60.0f;
    }

    switch (i_this->mMode) {
    case 10: {
        i_this->mSph.mObjCo.mSPrm &= ~1u; /* OffCoSetBit */

        actor->speedF = 26.0f;
        actor->gravity = -4.0f;
        actor->speed.y = 28.0f;

        clear_m2F0(i_this);

        i_this->m30C = 0.0f;

        actor->current.angle.y = fopAcM_searchPlayerAngleY(actor);

        fopAcM_seStart_nn(actor, JA_SE_CM_KS_ATTACK, 0);
        monsSeStart_nn(actor, JA_SE_CV_KS_ATTACK, 0);

        i_this->mMode = i_this->mMode + 1;
        break;
    }
    case 11: {
        if (i_this->m2F0[1] == 0) {
            if (actor->speedF > 0.0f && (i_this->mSph.mGObjAt.mRPrm & 1) /* ChkAtShieldHit */) {
                actor->gravity = -4.0f;
                actor->speed.y = 25.0f;
                i_this->m2F0[1] = 1;
                actor->speedF = actor->speedF * -0.5f;
            } else if (ks_kuttuki_check(i_this)) {
                return;
            }
        }

        if (!i_this->mAcch.ChkGroundHit() && !i_this->mAcch.ChkWaterIn()) {
            break;
        }

        i_this->mSph.mObjCo.mSPrm |= 1u; /* OnCoSetBit */

        i_this->m2E8[2] = (s16)gabi::ftoi(cM_rndF(20.0f) + 20.0f);

        i_this->mMode = i_this->mMode + 1;
        // Fall-through
    }
    case 12:
        ks_kuttuki_check(i_this);

        if (tyaku_check(i_this)) {
            actor->gravity = -4.0f;
            actor->speed.y = 1.0f;
            actor->speed.y += cM_rndF(5.0f);
            actor->speedF = 16.0f;
        }

        if (i_this->m2E8[2] == 0) {
            i_this->m31C = 20.0f;
            i_this->mAction = 0;
            i_this->mMode = 0;
            actor->speedF = 0.0f;
        }
        break;
    }

    cLib_addCalcAngleS2(&actor->shape_angle.y, actor->current.angle.y, 1, 0x1000);

    if (body_atari_check(i_this)) {
        fopAcM_seStart_nn(actor, JA_SE_LK_LAST_HIT, 0);
    }
}

/* action_kaze_move (inlined) */
static inline void action_kaze_move(ks_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get();

    switch (i_this->mMode) {
    case 20: {
        clear_m2F0(i_this);

        i_this->m2E8[1] = 0x32;

        fopAcM_monsSeStart(actor, JA_SE_CV_KS_BLOW, 0);

        i_this->m310 = actor->current.pos.y;
        i_this->m30C = 0.0f;

        actor->speedF = cM_rndF(15.0f) + 10.0f;
        i_this->m314 = cM_rndF(15.0f) + 10.0f;

        i_this->m308 = 0.0f;

        i_this->mMode = i_this->mMode + 1;
        // Fall-through
    }
    case 21: {
        cLib_addCalc2(&actor->speed.y, i_this->m314, 0.8f, i_this->m308);
        cLib_addCalc2(&i_this->m308, 5.0f, 1.0f, 0.5f);

        if (i_this->m310 + 200.0f < actor->current.pos.y || i_this->m2E8[1] == 0) {
            i_this->mMode = i_this->mMode + 1;
        }
        break;
    }
    case 22: {
        cLib_addCalc2(&actor->gravity, -1.0f, 0.3f, 0.5f);

        if (actor->speed.y < -2.0f) {
            actor->speed.y = -2.0f;
        }

        cLib_addCalc0(&actor->speedF, 0.5f, 2.0f);

        speed_keisan(i_this, actor->shape_angle.y);

        i_this->m2F0[0] += 3000;
        i_this->m30C = 2.0f;

        if (tyaku_check(i_this)) {
            i_this->mAction = 0;
            i_this->mMode = 0;
            return;
        }
        break;
    }
    }

    actor->shape_angle.z += 0x500;

    if (body_atari_check(i_this)) {
        fopAcM_seStart_nn(actor, JA_SE_LK_LAST_HIT, 0);
    }
}

/* action_dead_move (inlined) */
static inline void action_dead_move(ks_class* i_this) {
    fopAc_ac_c* actor = i_this;

    switch (i_this->mMode) {
    case 30: {
        clear_m2F0(i_this);

        i_this->mSph.mObjTg.mSPrm &= ~1u; /* OffTgSetBit */
        i_this->mSph.mObjCo.mSPrm &= ~1u; /* ClrCoSet */
        i_this->mSph.ClrTgHit();

        fopAcM_monsSeStart(actor, JA_SE_CV_KS_DAMAGE, 0);

        actor->speedF = cM_rndF(5.0f) + 15.0f;
        actor->gravity = -3.0f;
        actor->speed.y = cM_rndF(5.0f) + 20.0f;

        i_this->mMode = i_this->mMode + 1;

        i_this->m2F0[1] = (s16)gabi::ftoi(cM_rndFX(0x1000));

        i_this->mSph.mObjAt.mSPrm &= ~1u; /* OffAtSPrmBit(cCcD_AtSPrm_Set_e) (twice in GameCube) */
        // Fall-through
    }
    case 31: {
        actor->shape_angle.z += i_this->m2F0[1];

        if (tyaku_check(i_this)) {
            actor->gravity = -3.0f;
            actor->speedF = actor->speedF * 0.5f;

            switch (i_this->m2F0[0]) {
            case 0:
                actor->speed.y = 13.0f;
                i_this->m2F0[0] += 1;
                break;
            case 1:
                actor->speed.y = 7.0f;
                i_this->m2F0[0] += 1;
                break;
            case 2: {
                gabi::Local<cXyz> local_28;
                local_28->x = actor->current.pos.x;
                local_28->y = actor->current.pos.y;
                local_28->z = actor->current.pos.z;
                local_28->y = local_28->y + 20.0f;
                dead_eff_set(i_this, local_28);
                break;
            }
            }
        }
        break;
    }
    case 32: {
        gabi::Local<cXyz> local_28;
        local_28->x = actor->current.pos.x;
        local_28->y = actor->current.pos.y;
        local_28->z = actor->current.pos.z;
        local_28->y = local_28->y + 45.0f;
        dead_eff_set(i_this, local_28);
        break;
    }
    }
}

/* action_omoi (inlined) */
static inline void action_omoi(ks_class* i_this) {
    fopAc_ac_c* actor = i_this;

    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();

    /* HD: instead of `if (m2CE == 1 && HEAVY_IN) link->onHeavyState()`, a counter in the link
     * player is bumped every frame */
    be<s16>& heavy = *gabi::at<be<s16>>(gabi::ea(link) + 0x69EE);
    heavy = heavy + 1;

    switch (i_this->mMode) {
    case 40: {
        ripple_remove(i_this->m52C);

        actor->speedF = 0.0f;
        actor->gravity = 0.0f;
        actor->speed.x = 0.0f;
        actor->speed.y = 0.0f;
        actor->speed.z = 0.0f;

        if (GORON_COUNT() != 0 || KUTTUKU_ALL_COUNT() >= 0x14) {
            i_this->m528 = 0;
            actor->actor_status &= ~0x4000u; /* fopAcM_OffStatus(actor, fopAcStts_UNK4000_e) */
            i_this->mMode = 42;
            break;
        }

        clear_m2F0(i_this);

        if (KUTTUKU_ALL_COUNT() >= 0x14) {
            i_this->m300 = 0x13;
        }
        s16 idx;
        if (KUTTUKU_ALL_COUNT() < 0) {
            idx = 0;
        } else {
            idx = (s16)KUTTUKU_ALL_COUNT();
        }
        i_this->m300 = idx;

        i_this->m528 = daPy_getModelJointMtx(link, pl_harituki_joint_dt(idx));

        if (KUTTUKU_ALL_COUNT() == 0) {
            GORON_COUNT() = 0;
            i_this->m2CE = 1;
        }

        KUTTUKU_ALL_COUNT() = KUTTUKU_ALL_COUNT() + 1;

        /* HD: no `if (!HEAVY_IN && KUTTUKU_ALL_COUNT >= 5) { onHeavyState(); HEAVY_IN = TRUE; }` */

        i_this->m2D2 = 0;

        i_this->m2E8[1] = 0;
        i_this->m2E8[2] = 0;

        actor->actor_status |= 0x4000u; /* fopAcM_OnStatus(actor, fopAcStts_UNK4000_e) */

        fopAcM_setStageLayer(actor);

        i_this->mMode = i_this->mMode + 1;
        // Fall-through
    }
    case 0x29: {
        actor->current.roomNo = player->current.roomNo; /* fopAcM_SetRoomNo */

        if (!(fopAcM_GetID(actor) & 1)) {
            actor->shape_angle.y = (s16)gabi::ftoi(gabi::fmadds((f32)(s16)i_this->m2FE, 0.25f, (f32)(s16)actor->shape_angle.y));
        } else {
            actor->shape_angle.y = (s16)gabi::ftoi(gabi::fnmsubs((f32)(s16)i_this->m2FE, 0.25f, (f32)(s16)actor->shape_angle.y));
        }

        f32 x, y, z;
        u32 m = i_this->m528;
        if (m != 0) {
            x = gabi::load<f32>(m + 0xC);
            z = gabi::load<f32>(m + 0x2C);
            y = gabi::load<f32>(m + 0x1C);
        } else {
            x = player->current.pos.x;
            z = player->current.pos.z;
            y = player->current.pos.y;
        }

        if (x == 0.0f && y == 0.0f && z == 0.0f) {
            y = player->current.pos.y;
            x = player->current.pos.x;
            z = player->current.pos.z;
        }

        f32 psx = player->speed.x;
        f32 bound = REG12_F(16) + 10.0f;
        f32 lx = std::fabs(psx * 10.0f);
        if (lx < bound) {
            lx = bound;
        }
        f32 psy = player->speed.y;
        f32 ly = std::fabs(psy * 10.0f);
        if (ly < bound) {
            ly = bound;
        }
        f32 psz = player->speed.z;
        f32 lz = std::fabs(psz * 10.0f);
        if (lz < bound) {
            lz = bound;
        }

        if (std_sqrtf(gabi::fmadds(psz, psz, gabi::fmadds(psx, psx, psy * psy))) < REG12_F(17) + 8.0f) {
            f32 l = REG12_F(18) + 8.0f;
            lx = l;
            ly = l;
            lz = l;

            f32 dy = actor->current.pos.y - y;
            f32 dx = actor->current.pos.x - x;
            f32 dz = actor->current.pos.z - z;

            if (std_sqrtf(gabi::fmadds(dz, dz, gabi::fmadds(dx, dx, dy * dy))) < 2.0f && i_this->m2E8[1] == 0) {
                i_this->m2E8[1] = (s16)gabi::ftoi(cM_rndF(10.0f) + 10.0f);

                switch (i_this->m2D2) {
                case 0:
                    if (i_this->m300 < 0x13) {
                        i_this->m300 += 1;
                    } else {
                        i_this->m2D2 = 1;
                        i_this->m300 -= 1;
                    }
                    break;
                case 1:
                    if (i_this->m300 > 0) {
                        i_this->m300 -= 1;
                    } else {
                        i_this->m2D2 = 0;
                        i_this->m300 += 1;
                    }
                    break;
                }

                if (i_this->m300 >= 0x14) {
                    i_this->m300 = 0x13;
                } else if (i_this->m300 < 0) {
                    i_this->m300 = 1;
                }

                i_this->m528 = daPy_getModelJointMtx(link, pl_harituki_joint_dt(i_this->m300));
            }
        }

        /* HD: the morth follows the player's movement; no status0 0x800000 / status1 0x10 test */
        gabi::Local<cXyz> move;
        cXyz_mi(&player->current.pos, move, &player->old.pos);
        PSVECAdd(&actor->current.pos, move, &actor->current.pos);

        cLib_addCalc2(&actor->current.pos.x, x, 1.0f, lx);
        cLib_addCalc2(&actor->current.pos.y, y, 1.0f, ly);
        cLib_addCalc2(&actor->current.pos.z, z, 1.0f, lz);

        u8 cut = gabi::load<u8>(gabi::ea(link) + 0x3AC); /* getCutType() */
        if ((gabi::load<u32>(gabi::ea(link) + 0x3C0) & 0x2000) /* checkFrontRollCrash() */ ||
            cut == 8 /* CUT_TURN */ || (gabi::load<u8>(gabi::ea(link) + 0x3AC) == 9) /* CUT_ROLL */ ||
            dComIfGp_checkPlayerStatus0(daPyStts0_SWIM_e)) {
            i_this->mSph.mObjCo.mSPrm |= 1u; /* OnCoSetBit */
            i_this->mSph.mObjTg.mSPrm |= 1u; /* OnTgSetBit */

            if (i_this->m2CE) {
                /* HD: no link->offHeavyState() */
                HEAVY_IN() = FALSE;
                KUTTUKU_ALL_COUNT() = 0;
                GORON_COUNT() = 0;
                i_this->m2F0[0] = 0;
                i_this->m2F0[1] = 0;
            }

            i_this->mMode = 42;
            return;
        }

        if (!daPy_checkFrontRoll(link)) {
            if (i_this->m2CE == 0) {
                return;
            }

            if (i_this->m2F0[1]) {
                i_this->m2F0[1] = 0;
                i_this->m2E8[2] = 0x32;
                return;
            }

            if (i_this->m2E8[2] != 1) {
                return;
            }

            if (i_this->m2F0[0] <= 0) {
                return;
            }

            i_this->m2F0[0] = 0;
            return;
        }

        if (i_this->m2CE) {
            if (i_this->m2F0[1]) {
                return;
            }

            switch (i_this->m2F0[0]) {
            case 0:
                GORON_COUNT() = gabi::ftoi((f32)(s32)KUTTUKU_ALL_COUNT() * 0.25f);
                break;
            case 1:
                GORON_COUNT() = gabi::ftoi((f32)(s32)KUTTUKU_ALL_COUNT() * 0.5f);
                break;
            case 2:
                GORON_COUNT() = KUTTUKU_ALL_COUNT();
                break;
            }

            if (GORON_COUNT() == 0) {
                GORON_COUNT() = 1;
            }
            i_this->m2F0[0] += 1;
            i_this->m2F0[1] = 1;

            if (i_this->m2F0[0] < 3 && KUTTUKU_ALL_COUNT() >= 3) {
                return;
            }

            i_this->m2CE = 0;

            /* HD: no link->offHeavyState() */
            HEAVY_IN() = FALSE;

            i_this->m2F0[0] = 0;
            i_this->m2F0[1] = 0;

            if (KUTTUKU_ALL_COUNT() > 0) {
                KUTTUKU_ALL_COUNT() = KUTTUKU_ALL_COUNT() - 1;
            }

            GORON_COUNT() = KUTTUKU_ALL_COUNT();

            i_this->mMode = 42;
            return;
        }

        if (KUTTUKU_ALL_COUNT() != 0 && GORON_COUNT() > 0) {
            GORON_COUNT() = GORON_COUNT() - 1;
            KUTTUKU_ALL_COUNT() = KUTTUKU_ALL_COUNT() - 1;

            if (HEAVY_IN()) {
                if (KUTTUKU_ALL_COUNT() < 5) {
                    HEAVY_IN() = FALSE;
                    /* HD: no link->offHeavyState() */
                }
            }

            i_this->mMode = 42;
        }
        break;
    }
    case 42: {
        if (i_this->m528) {
            actor->actor_status &= ~0x4000u;
            fopAcM_setRoomLayer(actor, actor->current.roomNo);
            i_this->m528 = 0;
        }

        actor->current.angle.y = fopAcM_searchPlayerAngleY(actor) + 0x8000;
        actor->current.angle.y += (s16)gabi::ftoi(cM_rndFX(0x4000));

        actor->speedF = 15.0f;
        actor->gravity = -3.0f;
        actor->speed.y = 26.0f;

        i_this->mMode = i_this->mMode + 1;
        // Fall-through
    }
    case 43: {
        if (i_this->mAcch.ChkGroundHit() || i_this->mAcch.ChkWaterIn()) {
            i_this->m2E8[2] = (s16)gabi::ftoi(cM_rndF(40.0f) + 40.0f);

            i_this->mAction = 1;
            i_this->mSph.mObjCo.mSPrm |= 1u; /* OnCoSetBit */
            i_this->mSph.mObjTg.mSPrm |= 1u; /* OnTgSetBit */
            i_this->mMode = 12;
        }
        break;
    }
    }

    if (i_this->mMode == 43 && body_atari_check(i_this) && i_this->m2CE) {
        fopAcM_seStart(actor, JA_SE_LK_LAST_HIT, 0);
        i_this->m2CE = 0;
    }
}

/* action_tubo_search (inlined) */
static inline void action_tubo_search(ks_class* i_this) {
    fopAc_ac_c* actor = i_this;
    switch (i_this->mMode) {
    case 50: {
        i_this->mKsID = fpcM_ERROR_PROCESS_ID_e;

        fpcM_Search(0x021A7E68 /* tsubo_search */, actor);

        if (i_this->mKsID == fpcM_ERROR_PROCESS_ID_e) {
            fopAcM_delete(actor);
        } else {
            i_this->mMode = i_this->mMode + 1;
        }
        break;
    }
    case 51: {
        fopAc_ac_c* ksActor = fopAcM_SearchByID(i_this->mKsID);

        if (ksActor) {
            actor->current.pos.copy(ksActor->current.pos);

            if (i_this->m318 == 0.0f) {
                return;
            }

            if (fopAcM_searchPlayerDistance(actor) > i_this->m318) {
                return;
            }
        }

        i_this->m31C = 100.0f;

        u32 sprm = i_this->mSph.mObjAt.mSPrm;
        sprm &= ~0x10u; /* OffAtNoTgHitInfSet */
        sprm |= 8;      /* OnAtVsBitSet(cCcD_AtSPrm_VsOther_e) */
        sprm &= ~0x14u; /* OffAtVsBitSet(VsPlayer), OffAtSPrmBit(NoTgHitInfSet) */
        sprm |= 1;      /* OnAtSPrmBit(cCcD_AtSPrm_Set_e) */
        i_this->mSph.mObjAt.mSPrm = sprm;
        i_this->mSph.mObjAt.mRPrm = 1; /* OnAtHitBit (HD: the word is set) */

        i_this->mSph.SetC(&actor->current.pos);
        i_this->mSph.SetR(i_this->m31C);
        dComIfG_Ccsp_Set(&i_this->mSph);

        i_this->mMode = i_this->mMode + 1;
        break;
    }
    case 52: {
        i_this->mSph.SetC(&actor->current.pos);
        i_this->mSph.SetR(i_this->m31C);
        dComIfG_Ccsp_Set(&i_this->mSph);

        if (i_this->m2C9 > 0x15 || i_this->m2C9 == 0) {
            i_this->m2C9 = 1;
        }

        for (int i = 0; i < i_this->m2C9; i++) {
            gabi::Local<cXyz> local_24;
            local_24->copy(actor->current.pos);
            local_24->y += cM_rndF(40.0f);

            if (i != 0) {
                local_24->x += cM_rndFX(40.0f);
                local_24->z += cM_rndFX(40.0f);
            }

            /* actor->shape_angle = actor->current.angle; actor->shape_angle.x = 0; */
            actor->shape_angle.x = 0;
            actor->shape_angle.y = actor->current.angle.y;
            actor->shape_angle.z = actor->current.angle.z;

            fopAcM_create(fpcNm_KS_e, 3, local_24, fopAcM_GetRoomNo(actor), &actor->shape_angle, &actor->scale, 0, 0);
        }

        fopAcM_delete(actor);
        break;
    }
    }
}

/* action_kb_birth_check (inlined) */
static inline void action_kb_birth_check(ks_class* i_this) {
    fopAc_ac_c* actor = i_this;
    switch (i_this->mMode) {
    case 60: {
        actor->current.pos.y += REG8_F(13) + 30.0f;
        actor->current.angle.y = (s16)gabi::ftoi(cM_rndFX(0x7FFF));

        f32 a = REG8_F(8) + 4.0f;
        actor->speedF = a + cM_rndF(REG8_F(9) + 4.0f);
        a = REG8_F(10) + 20.0f;
        actor->speed.y = a + cM_rndF(REG8_F(11) + 5.0f);
        actor->gravity = -(REG8_F(12) + 2.0f);

        i_this->mMode = i_this->mMode + 1;
        // Fall-through
    }
    case 61: {
        if (!(actor->speed.y > 0.0f) &&
            (i_this->mAcch.ChkGroundHit() || i_this->mAcch.GetGroundH() + (REG8_F(19) + 10.0f) > actor->current.pos.y)) {
            actor->actor_status &= ~0x4000u;

            i_this->mAction = 0;
            actor->speedF = 0.0f;
            actor->gravity = 0.0f;
            actor->speed.x = 0.0f;
            actor->speed.y = 0.0f;
            actor->speed.z = 0.0f;
            i_this->mMode = 0;
        }
        break;
    }
    }
}

/* 021A7F94 */
static BOOL daKS_Execute(ks_class* i_this) {
    WWHD_FUNC(0x021A7F94, BOOL, i_this);
    fopAc_ac_c* actor = i_this;

    if (enemy_ice(&i_this->mEnemyIce)) {
        J3DModel_setBaseTRMtx(i_this->mpBodyMorf->getModel(), mDoMtx_stack_c::get());
        J3DModel_setBaseTRMtx(i_this->mpEyeMorf->getModel(), mDoMtx_stack_c::get());
        if (i_this->m2D0) {
            i_this->m320 = i_this->m320 + 1.0f;
            if (i_this->m320 > 7.0f) {
                i_this->m320 = 7.0f;
            }
        }
        return TRUE;
    }

    for (int i = 0; i < 4; i++) {
        if (i_this->m2E8[i]) {
            i_this->m2E8[i] -= 1;
        }
    }

    if (i_this->mGmID != 0 && i_this->mAction != 3) {
        fopAc_ac_c* gmActor = fopAcM_SearchByID(i_this->mGmID);

        if (gmActor && ((fopAcM_GetParam(gmActor) & 0xff0000) == 0xff0000 || (fopAcM_GetParam(gmActor) & 0xff0000) == 0)) {
            bool bVar5;
            if (gmActor && fpcM_GetName(gmActor) == fpcNm_GM_e) {
                bVar5 = !(gmActor->health > 0);
            } else {
                bVar5 = true;
            }

            if (bVar5) {
                if (i_this->mAction != 4) {
                    i_this->mAction = 3;
                    i_this->mMode = 30;
                } else if (i_this->mMode != 43) {
                    dComIfGp_get(); /* daPy_getPlayerLinkActorClass()->offHeavyState(): HD, no effect */

                    GORON_COUNT() = 0;
                    HEAVY_IN() = FALSE;

                    i_this->m2F0[0] = 0;
                    i_this->m2F0[1] = 0;

                    i_this->mMode = 42;
                }
            }
        }
    }

    switch (i_this->mAction) {
    case 0:
        action_dousa_move(i_this);
        ks_kuttuki_check(i_this);
        break;
    case 1:
        action_kougeki_move(i_this);
        break;
    case 2:
        action_kaze_move(i_this);
        break;
    case 3:
        action_dead_move(i_this);
        break;
    case 4:
        action_omoi(i_this);
        break;
    case 10:
        action_tubo_search(i_this);
        break;
    case 20:
        action_kb_birth_check(i_this);
        break;
    }

    if (i_this->m2C8 == 6) {
        return TRUE;
    }

    if (i_this->m302) {
        s16 f = (s16)(i_this->m302 + 1);
        if (f > 7) {
            i_this->m302 = 0;
        } else {
            i_this->m302 = f;
        }
    }

    if (i_this->m2E8[0] == 0) {
        i_this->m2E8[0] = (s16)gabi::ftoi(cM_rndFX(25.0f) + 50.0f);
        i_this->m302 = 1;
    }

    cMtx_YrotS(calc_mtx(), actor->current.angle.y);

    gabi::Local<cXyz> local_18;
    local_18->x = 0.0f;
    local_18->y = 0.0f;
    local_18->z = actor->speedF;

    gabi::Local<cXyz> local_c;
    MtxPosition(local_18, local_c);

    actor->speed.x = local_c->x;
    actor->speed.z = local_c->z;

    if (i_this->mMode != 41 && !i_this->mAcch.ChkGroundHit() && !i_this->mAcch.ChkWaterIn()) {
        actor->speed.y = actor->speed.y + actor->gravity;

        if (actor->speed.y < -20.0f) {
            actor->speed.y = -20.0f;
        }
    }

    actor->eyePos.copy(actor->current.pos);
    gabi::at<cXyz>(gabi::ea(actor) + 0x390)->copy(actor->current.pos); /* attention_info.position = eyePos */

    i_this->mSph.SetC(&actor->current.pos);
    i_this->mSph.SetR(i_this->m31C);
    dComIfG_Ccsp_Set(&i_this->mSph);

    if ((i_this->mSph.mObjCo.mSPrm & 1) /* ChkCoSet */ && (i_this->mAction == 0 || i_this->mAction == 2)) {
        fopAcM_posMove(actor, &i_this->mStts.m_cc_move);
    } else {
        fopAcM_posMove(actor, nullptr);
    }

    if (i_this->mMode != 41) {
        BG_check(i_this);

        /* naraku_check (inlined). HD: falling out of the map is detected by a missing ground
         * (GetGroundH() == -G_CM3D_F_INF) for 50 frames; no ground-code / y < -500 test */
        if (i_this->mAcch.GetGroundH() == -1000000000.0f) {
            i_this->m2D3 = i_this->m2D3 + 1;
            if (i_this->m2D3 > 0x32) {
                fopAcM_delete(actor);
                draw_SUB(i_this);
                i_this->m2CD = 1;
                return TRUE;
            }
        } else {
            i_this->m2D3 = 0;
        }

        f32 wtrH = gabi::load<f32>(gabi::ea(&i_this->mAcch) + 0x174 + 0x48); /* m_wtr.GetHeight() */
        if (i_this->mAcch.ChkWaterIn() && actor->current.pos.y < wtrH + 20.0f) {
            if (!i_this->m2CF) {
                i_this->m2CF = 1;

                gabi::Local<cXyz> local_18b;
                local_18b->x = 0.5f;
                local_18b->z = 0.5f;
                local_18b->y = 0.5f;

                ripple_remove(i_this->m52C);
                /* dComIfGp_particle_setShipTail(ID_AK_JN_HAMON00, &current.pos, NULL, &scale, 0xFF, &m52C) */
                dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &actor->current.pos, nullptr, local_18b, 0xFF,
                                gabi::at<dPa_levelEcallBack>(gabi::ea(i_this->m52C)), -1, nullptr, nullptr, nullptr);
                gabi::store<f32>(gabi::ea(i_this->m52C) + 0x10, 0.0f); /* m52C.setRate(0.0f) */
            }

            cLib_addCalc2(&actor->current.pos.y, gabi::load<f32>(gabi::ea(&i_this->mAcch) + 0x174 + 0x48) + 20.0f, 1.0f, 10.0f);

            actor->gravity = 0.0f;
            actor->speed.y = 0.0f;
        } else if (i_this->m2CF && (i_this->mAcch.ChkGroundHit() || actor->current.pos.y > wtrH + 100.0f)) {
            i_this->m2CF = 0;
            ripple_remove(i_this->m52C);
        }
    }

    draw_SUB(i_this);

    i_this->m2CD = 1;

    return TRUE;
}
VERIFY(0x021A7F94, daKS_Execute);

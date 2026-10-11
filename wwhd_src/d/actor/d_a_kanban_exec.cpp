/**
 * d_a_kanban_exec.cpp (WWHD)
 * Object - Cuttable sign: daKanban_Execute with its inlined move functions (mother_move,
 * mother_water_swim, mother_return_move, parts_move, chield_parts_move, chield_water_swim)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kanban.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_kanban.h"

enum { AT_TYPE_SWORD = 0x2, AT_TYPE_SKULL_HAMMER = 0x10000 };
enum { CUT_TYPE_HAMMER_SIDESWING = 0x11 };
enum { dPa_name_ID_AK_JN_OK = 0xD };

// HD: manager status calls are distinct from play-object accessor loads.
enum { daPyStts1_WIND_WAKER_CONDUCT_e = 0x1 };
enum { fopMsgStts_BOX_CLOSED_e = 0x12, fopMsgStts_MSG_DESTROYED_e = 0x13 };
/* kanban_clearStatusMap / SetStatusMap: the low 6 bits of actor_status */
static inline void kanban_clearStatusMap(fopAc_ac_c* a) { a->actor_status = a->actor_status & ~0x3Fu; }
static inline void kanban_setStatusMap(fopAc_ac_c* a, u32 map) { a->actor_status = (a->actor_status & ~0x3Fu) | map; }
/* dCcD_GObjInf Set bits: Tg SPrm (+0x18), Co SPrm (+0x2C), bit 1 */
static inline void OnTgSetBit(dCcD_Cyl* c) { c->mObjTg.mSPrm |= 1; }
static inline void OffTgSetBit(dCcD_Cyl* c) { c->mObjTg.mSPrm &= ~1u; }
static inline void OnCoSetBit(dCcD_Cyl* c) { c->mObjCo.mSPrm |= 1; }
static inline void OffCoSetBit(dCcD_Cyl* c) { c->mObjCo.mSPrm &= ~1u; }
static inline void speed_zero(kanban_class* i_this) {
    i_this->actor.gravity = 0.0f;
    i_this->actor.speedF = 0.0f;
    i_this->actor.speed.set(0.0f, 0.0f, 0.0f);
}

/* mother_move (inlined) */
static inline void mother_move(kanban_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    switch (i_this->m2C0) {
    case 10:
        if (i_this->m5B0.ChkTgHit()) {
            void* hitObj = i_this->m5B0.GetTgHitObj();
            if (hitObj != nullptr) {
                kanban_clearStatusMap(a_this);
                u32 atType = reinterpret_cast<dCcD_GObjInf*>(hitObj)->mAtType;
                bool swordCut = atType == AT_TYPE_SWORD;
                if (atType == AT_TYPE_SKULL_HAMMER) {
                    i_this->m29A = 2;
                    if (kanban_player(player)->mCutType != CUT_TYPE_HAMMER_SIDESWING) {
                        gabi::Local<cXyz> sp18;
                        sp18->copy(*i_this->m5B0.GetTgHitPosP());
                        i_this->m2C0 = 20;
                        i_this->m29B = 0;
                        i_this->m29A = 1;
                        i_this->m2A4 = 0.0f;
                        i_this->m2A8 = 0.0f;
                        dComIfGp_particle_set(dPa_name_ID_AK_JN_OK, sp18, &player->shape_angle);
                        kanban_setPauseTimer(1);
                        return;
                    }
                    swordCut = true;
                }
                if (swordCut) {
                    u32 cut = kanban_player(player)->mCutType;
                    s32 i = 0;
                    for (; i < 32; i++) {
                        if ((u32)(s32)pl_cut_real_no_dt[i] == cut) break;
                    }
                    if (i < 32 && pl_cut_no_dt[i] != 0xFF) {
                        i_this->m2C4 = pl_cut_no_dt[i];
                        kanban_cutPointCheck(i_this);
                    }
                } else {
                    i_this->m2C4 = 4;
                    kanban_cutPointCheck(i_this);
                }
            }
        }
        break;

    case 11:
        a_this->current.angle.y = (s16)(player->shape_angle.y + -0x4000);
        a_this->shape_angle.y = a_this->current.angle.y;
        i_this->m298 = 0;
        if (i_this->m294 & 0x100) {
            a_this->speedF = 20.0f;
            a_this->speed.y = 10.0f;
            a_this->gravity = -3.0f;
        } else {
            a_this->speedF = 3.0f;
            a_this->speed.y = 7.0f;
            a_this->gravity = -3.0f;
        }
        i_this->m2C0 = i_this->m2C0 + 1;
        break;

    case 12:
        if (kanban_seaWaterCheck(i_this)) {
            i_this->m2BE = KANBAN_MOTHER_SWIM;
            i_this->m2C0 = 0x28;
        } else {
            f32 spy = a_this->speed.y;
            if (i_this->m350.ChkWallHit()) {
                a_this->speedF = -a_this->speedF;
            }
            if (spy < 0.0f) {
                cLib_addCalcAngleS2(&a_this->shape_angle.x, 0x4000, 1, 0x1000);
            }
            if (i_this->m350.ChkGroundHit()) {
                i_this->m2C6 = 0x800;
                fopAcM_getGroundAngle(a_this, &i_this->m2AC);
                a_this->speedF = 0.0f;
                a_this->gravity = 0.0f;
                i_this->m2C0 = 13;
            }
        }
        break;

    case 13:
        if (kanban_seaWaterCheck(i_this)) {
            i_this->m2BE = KANBAN_MOTHER_SWIM;
            i_this->m2C0 = 0x28;
            break;
        }
        cLib_addCalcAngleS2(&a_this->shape_angle.x, (s16)(i_this->m2C6 + 0x4000), 1, 0x1000);
        i_this->m2C6 = (s16)(i_this->m2C6 ^ 0xFF00);
        i_this->m298 = (i_this->m298 + 1) & 3;
        if (!(i_this->m298 & 1)) {
            i_this->m2C6 = 0x400;
        }
        if (!(i_this->m298 & 3)) {
            a_this->shape_angle.x = 0x4000;
            i_this->m2C6 = 0;
            OffCoSetBit(&i_this->m5B0);
            OffTgSetBit(&i_this->m5B0);
            i_this->m5B0.ClrTgHit();
            i_this->m2C0 = i_this->m2C0 + 1;
        }
        /* fallthrough */
    case 14:
        if (kanban_seaWaterCheck(i_this)) {
            i_this->m2BE = KANBAN_MOTHER_SWIM;
            i_this->m2C0 = 0x28;
        } else if (i_this->m2B2[4] == 0 && kanban_shockDamageCheck(i_this)) {
            a_this->speed.y = cM_rndF(10.0f) + 20.0f;
            a_this->gravity = -3.0f;
            i_this->m2C0 = 12;
        }
        break;

    case 0x14:
        i_this->m2C0 = i_this->m2C0 + 1;
        /* fallthrough */
    case 0x15: {
        i_this->m2B2[1] = 2;
        f32 fVar4 = 90.0f;
        if (i_this->m2C2 & 8) {
            fVar4 = 65.0f;
        }
        if (i_this->m2C2 & 0x10) {
            fVar4 = 20.0f;
        }
        cLib_addCalc2(&a_this->current.pos.y, i_this->m2F8.y - fVar4, 1.0f, 30.0f);
        break;
    }
    }
}

/* mother_water_swim / chield_water_swim (inlined) */
static inline void water_swim(kanban_class* i_this, s16 base) {
    dComIfGp_get(); /* HD: unused fetch */
    if (i_this->m2C0 == base) {
        i_this->m2C6 = 0;
        i_this->actor.shape_angle.x = 0x4000;
        OffCoSetBit(&i_this->m5B0);
        OffTgSetBit(&i_this->m5B0);
        i_this->m5B0.ClrTgHit();
        i_this->m2C0 = i_this->m2C0 + 1;
        kanban_seaWaterCheck(i_this);
    } else if (i_this->m2C0 == base + 1) {
        kanban_seaWaterCheck(i_this);
    }
}

/* the common return-home step of mother_return_move / chield_parts_move */
static inline void return_move(kanban_class* i_this) {
    if (i_this->m2B2[0] == 0) {
        i_this->m2CC.x = 0.0f;
        i_this->m2CC.y = 0.0f;
        i_this->m2CC.z = 0.0f;
        i_this->m2E4 = 0.0f;
        i_this->m2E8 = 0.0f;
        i_this->m2EC = 0.0f;
        i_this->m2C6 = 0;
        if (kanban_returnPosition(i_this)) {
            i_this->m2C0 = i_this->m2C0 + 1;
        }
    }
}
static inline void return_angle(kanban_class* i_this, s16 first) {
    if (i_this->m2B2[0] == 0 && i_this->m2C0 >= first) {
        cLib_addCalcAngleS2(&i_this->actor.current.angle.x, i_this->m2F0.x, 1, 0x1000);
        cLib_addCalcAngleS2(&i_this->actor.current.angle.z, i_this->m2F0.z, 1, 0x1000);
        i_this->actor.shape_angle.x = i_this->actor.current.angle.x;
        i_this->actor.shape_angle.y = i_this->actor.current.angle.y;
        i_this->actor.shape_angle.z = i_this->actor.current.angle.z;
    }
}

/* mother_return_move (inlined) */
static inline void mother_return_move(kanban_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    dComIfGp_get(); /* HD: unused fetch */

    switch (i_this->m2C0) {
    case 30:
        if (dComIfGp_checkPlayerStatus1(0, daPyStts1_WIND_WAKER_CONDUCT_e)) {
            break;
        }
        i_this->m2B2[0] = (s16)gabi::ftoi(cM_rndF(30.0f));
        i_this->m2C0 = i_this->m2C0 + 1;
        /* fallthrough */
    case 31:
        return_move(i_this);
        break;

    case 32: {
        cLib_addCalcAngleS2(&a_this->current.angle.y, i_this->m2F0.y, 1, 0x1000);
        s16 sVar2 = cLib_distanceAngleS(a_this->current.angle.y, i_this->m2F0.y);
        if (sVar2 < 0x100) {
            i_this->m2C0 = i_this->m2C0 + 1;
            a_this->current.angle.y = i_this->m2F0.y;
        }
        break;
    }
    case 33:
        if (i_this->m294 == 0x7FE) {
            i_this->m2C2 = 0;
            i_this->m2C4 = 0;
            OnTgSetBit(&i_this->m5B0);
            OnCoSetBit(&i_this->m5B0);
            i_this->m2BE = KANBAN_MOTHER;
            i_this->m2C0 = 10;
            i_this->m29B = 1;
            i_this->m2A4 = 105.0f;
            i_this->m2A8 = 50.0f;
            kanban_setStatusMap(a_this, 0x38);
            return;
        }
        break;
    }
    return_angle(i_this, 31);
}

/* parts_move (inlined) */
static inline void parts_move(kanban_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    switch (i_this->m2C0) {
    case 0x6e: {
        i_this->m298 = 0;
        s16 ay = fopAcM_searchActorAngleY(player, a_this);
        a_this->current.angle.y = ay;
        if (i_this->m2C4 != 1) {
            f32 fy = (f32)ay;
            a_this->current.angle.y = (s16)gabi::ftoi(fy + cM_rndFX(8000.0f));
            kanban_class* kanban = (kanban_class*)fopAcM_SearchByID(i_this->m2C8);
            if (kanban != nullptr) {
                a_this->speedF = cM_rndFX(2.0f) + 10.0f;
                a_this->speed.y = cM_rndFX(5.0f) + 20.0f;
                a_this->gravity = -5.0f;
                if (kanban->m29A == 2) {
                    a_this->current.angle.y = (s16)(player->shape_angle.y + -0x4000);
                    a_this->speedF = cM_rndFX(5.0f) + 15.0f;
                    a_this->speed.y = cM_rndFX(5.0f) + 30.0f;
                    a_this->gravity = -5.0f;
                }
            }
            a_this->shape_angle.y = a_this->current.angle.y;
            i_this->m2C0 = 0x6f;
        } else {
            a_this->shape_angle.y = ay;
            i_this->m298 = 0;
            if (i_this->m294 & 0x100) {
                a_this->speedF = 20.0f;
                a_this->speed.y = 10.0f;
                a_this->gravity = -3.0f;
            } else {
                a_this->speedF = 3.0f;
                a_this->speed.y = 7.0f;
                a_this->gravity = -3.0f;
            }
            i_this->m2C0 = 0x78;
        }
        break;
    }

    case 0x6f: {
        s16 ax = a_this->shape_angle.x;
        if (i_this->m350.ChkWallHit()) {
            a_this->speedF = -a_this->speedF;
        }
        a_this->shape_angle.x = (s16)(ax + 0x1000);
        if (i_this->m350.ChkGroundHit()) {
            a_this->speedF = 5.0f;
            a_this->speed.y = 10.0f;
            i_this->m2C0 = i_this->m2C0 + 1;
        }
        break;
    }

    case 0x70:
        cLib_addCalcAngleS2(&a_this->shape_angle.x, 0x4000, 1, 0x1000);
        if (i_this->m350.ChkGroundHit()) {
            i_this->m2C6 = 0x800;
            fopAcM_getGroundAngle(a_this, &i_this->m2AC);
            a_this->speedF = 0.0f;
            a_this->gravity = 0.0f;
            i_this->m2C0 = i_this->m2C0 + 1;
        }
        break;

    case 0x71:
        cLib_addCalcAngleS2(&a_this->shape_angle.x, (s16)(i_this->m2C6 + 0x4000), 1, 0x1000);
        i_this->m2C6 = (s16)(i_this->m2C6 ^ 0xFF00);
        i_this->m298 = (i_this->m298 + 1) & 3;
        if ((i_this->m298 & 1) == 0) {
            i_this->m2C6 = 0x400;
        }
        if ((i_this->m298 & 3) == 0) {
            a_this->shape_angle.x = 0x4000;
            i_this->m2C6 = 0;
            OffCoSetBit(&i_this->m5B0);
            OffTgSetBit(&i_this->m5B0);
            i_this->m5B0.ClrTgHit();
            i_this->m2C0 = i_this->m2C0 + 1;
        }
        break;

    case 0x72:
        if (i_this->m2B2[4] == 0 && kanban_shockDamageCheck(i_this)) {
            a_this->speed.y = cM_rndF(10.0f) + 20.0f;
            a_this->gravity = -3.0f;
            i_this->m2C0 = 0x70;
        }
        break;

    case 0x78: {
        f32 spy = a_this->speed.y;
        if (i_this->m350.ChkWallHit()) {
            a_this->speedF = -a_this->speedF;
        }
        if (spy < 0.0f) {
            cLib_addCalcAngleS2(&a_this->shape_angle.x, 0x4000, 1, 0x1000);
        }
        if (i_this->m350.ChkGroundHit()) {
            a_this->shape_angle.x = 0x4000;
            i_this->m2C6 = 0x800;
            fopAcM_getGroundAngle(a_this, &i_this->m2AC);
            a_this->speedF = 0.0f;
            a_this->gravity = 0.0f;
            i_this->m2C0 = 0x71;
        }
        break;
    }
    }
}

/* chield_parts_move (inlined) */
static inline void chield_parts_move(kanban_class* i_this) {
    dComIfGp_get(); /* HD: unused fetch */
    switch (i_this->m2C0) {
    case 0x82:
        if (dComIfGp_checkPlayerStatus1(0, daPyStts1_WIND_WAKER_CONDUCT_e)) {
            break;
        }
        i_this->m2B2[0] = (s16)gabi::ftoi(cM_rndF(30.0f));
        i_this->m2C0 = i_this->m2C0 + 1;
        /* fallthrough */
    case 0x83:
        return_move(i_this);
        break;

    case 0x84: {
        cLib_addCalcAngleS2(&i_this->actor.current.angle.y, i_this->m2F0.y, 1, 0x1000);
        s16 sVar3 = cLib_distanceAngleS(i_this->actor.current.angle.y, i_this->m2F0.y);
        if (sVar3 < 0x100) {
            i_this->actor.current.angle.y = i_this->m2F0.y;
            i_this->m2C0 = i_this->m2C0 + 1;
            kanban_class* kanban = (kanban_class*)fopAcM_SearchByID(i_this->m2C8);
            if (kanban != nullptr) {
                kanban->m294 = kanban->m294 ^ i_this->m294;
            }
        }
        break;
    }
    case 0x85: {
        kanban_class* kanban = (kanban_class*)fopAcM_SearchByID(i_this->m2C8);
        if (kanban != nullptr) {
            if (kanban->m29B == 1) {
                fopAcM_delete(&i_this->actor);
            }
            if (kanban->m29B == 2) {
                i_this->m2BE = KANBAN_PARTS;
                i_this->m2C0 = 0x6e;
            }
        }
        break;
    }
    }
    return_angle(i_this, 0x83);
}

static inline void tact_return(kanban_class* i_this, s16 mode, s16 proc) {
    speed_zero(i_this);
    i_this->m2BE = mode;
    i_this->m2C0 = proc;
}

/* 0218B234 */
static BOOL daKanban_Execute(kanban_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    for (s32 i = 0; i < 5; i++) {
        if (i_this->m2B2[i] != 0) {
            i_this->m2B2[i] = i_this->m2B2[i] - 1;
        }
    }

    switch (i_this->m2BE) {
    case KANBAN_MOTHER:
        if (i_this->actor.eventInfo.checkCommandTalk()) { /* eventInfo.checkCommandTalk() */
            u32 mng = kanban_msgManager();
            if (l_msgId == fpcM_ERROR_PROCESS_ID_e) {
                if (dComIfGp_checkCameraAttentionStatus(dComIfGp_getPlayerCameraID(0), 4)) {
                    l_msgId = kanban_messageSet(mng, fopAcM_GetParam(&i_this->actor), &i_this->actor.eyePos);
                }
            } else {
                /* HD: the message status lives in the play object */
                if (kanban_getMessageStatus(mng) == fopMsgStts_BOX_CLOSED_e) {
                    kanban_setMessageStatus(mng, fopMsgStts_MSG_DESTROYED_e);
                    dComIfGp_event_reset();
                    l_msgId = 0xFFFFFFFF;
                }
            }
        } else {
            if (i_this->m2C2 == 0 && i_this->m2C0 != 0x14 && i_this->m2C0 != 0x15) {
                eventInfo_onCondition(&i_this->actor, dEvtCnd_CANTALK_e);
            }
            mother_move(i_this);
        }

        if (i_this->m2C0 != 14) {
            kanban_rollPlayerCrash(&i_this->actor, 35.0f, 0);
        }

        i_this->m5B0.SetC(&i_this->actor.current.pos);
        i_this->m5B0.SetH(i_this->m2A4);
        i_this->m5B0.SetR(i_this->m2A8);
        dComIfG_Ccsp_Set(&i_this->m5B0);

        if (i_this->m294 != 1 && i_this->m294 != 0x7FE && kanban_getTactMusic(player) == 4) {
            tact_return(i_this, KANBAN_MOTHER_RETURN, 30);
        }
        break;

    case KANBAN_MOTHER_RETURN:
        mother_return_move(i_this);
        break;

    case KANBAN_MOTHER_SWIM:
        water_swim(i_this, 40);
        if (kanban_getTactMusic(player) == 4) {
            dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->m514);
            tact_return(i_this, KANBAN_MOTHER_RETURN, 30);
        }
        break;

    case KANBAN_PARTS:
        parts_move(i_this);
        if (kanban_getTactMusic(player) == 4) {
            tact_return(i_this, KANBAN_PARTS_RETURN, 0x82);
        } else if (kanban_seaWaterCheck(i_this)) {
            i_this->m2BE = KANBAN_PARTS_SWIM;
            i_this->m2C0 = 0x8c;
        }
        break;

    case KANBAN_PARTS_RETURN:
        chield_parts_move(i_this);
        break;

    case KANBAN_PARTS_SWIM:
        water_swim(i_this, 140);
        if (kanban_getTactMusic(player) == 4) {
            dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->m514);
            tact_return(i_this, KANBAN_PARTS_RETURN, 0x82);
        }
        break;
    }

    cMtx_YrotS(calc_mtx(), i_this->actor.current.angle.y);
    gabi::Local<cXyz> sp14;
    sp14->x = 0.0f;
    sp14->y = 0.0f;
    sp14->z = i_this->actor.speedF;
    gabi::Local<cXyz> sp08;
    MtxPosition(sp14, sp08);
    i_this->actor.speed.x = sp08->x;
    i_this->actor.speed.y = i_this->actor.speed.y + i_this->actor.gravity;
    i_this->actor.speed.z = sp08->z;

    if (i_this->m2A4 != 0.0f) {
        fopAcM_posMove(&i_this->actor, &i_this->m574.m_cc_move);
    } else {
        fopAcM_posMove(&i_this->actor, nullptr);
    }

    i_this->m310.SetWall(0.0f, 40.0f);

    if (i_this->m2BE != KANBAN_MOTHER_RETURN && i_this->m2BE != KANBAN_PARTS_RETURN && i_this->m2B2[1] == 0) {
        i_this->actor.current.pos.y = i_this->actor.current.pos.y + i_this->m2E8;
        i_this->actor.old.pos.y = i_this->actor.old.pos.y + i_this->m2E8;
        i_this->actor.current.pos.y = i_this->actor.current.pos.y - i_this->m2A0;
        i_this->actor.old.pos.y = i_this->actor.old.pos.y - i_this->m2A0;
        i_this->m350.CrrPos(dComIfG_Bgsp());
        i_this->actor.current.pos.y = i_this->actor.current.pos.y + i_this->m2A0;
        i_this->actor.old.pos.y = i_this->actor.old.pos.y + i_this->m2A0;
        i_this->actor.current.pos.y = i_this->actor.current.pos.y - i_this->m2E8;
        i_this->actor.old.pos.y = i_this->actor.old.pos.y - i_this->m2E8;
    }
    return TRUE;
}

/* WWHD glue (tools/verify/autoglue.py): HD guest calls between split sources
ext 0218A6F8 BOOL kanban_seaWaterCheck(kanban_class* sign)
ext 0218AB94 BOOL kanban_shockDamageCheck(kanban_class* sign)
ext 0218AD44 void kanban_cutPointCheck(kanban_class* sign)
ext 0218B00C BOOL kanban_returnPosition(kanban_class* sign)
ext 025F7DB0 u32 kanban_messageSet(u32 manager, u32 message, cXyz* position)
ext 025F795C u8 kanban_getMessageStatus(u32 manager)
ext 025F74D0 void kanban_setMessageStatus(u32 manager, u8 status)
ext 025D69FC BOOL kanban_rollPlayerCrash(fopAc_ac_c* actor, f32 distance, u32 flags)
*/

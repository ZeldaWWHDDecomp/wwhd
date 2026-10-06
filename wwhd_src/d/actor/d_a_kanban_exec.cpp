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

/* functions of d_a_kanban.cpp (guest calls by address) */
static inline BOOL sea_water_check(kanban_class* i_this) { return gabi::call<BOOL>(0x0218A6F8, i_this); }
static inline BOOL shock_damage_check(kanban_class* i_this) { return gabi::call<BOOL>(0x0218AB94, i_this); }
static inline void cut_point_check(kanban_class* i_this) { gabi::call(0x0218AD44, i_this); }
static inline BOOL ret_keisan_move(kanban_class* i_this) { return gabi::call<BOOL>(0x0218B00C, i_this); }

static be<u32>& l_msgId() { return *gabi::at<be<u32>>(0x101B7CB8); }
#define pl_cut_real_no_dt(i) gabi::load<s16>(0x101B7CBC + 2 * (i)) /* [32] */
#define pl_cut_no_dt(i) gabi::load<s16>(0x101B7CFC + 2 * (i))      /* [32] */

enum { AT_TYPE_SWORD = 0x2, AT_TYPE_SKULL_HAMMER = 0x10000 };
enum { CUT_TYPE_HAMMER_SIDESWING = 0x11 };
enum { dPa_name_ID_AK_JN_OK = 0xD };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* daPy_py_c::getTactMusic(): virtual, HD vtable (+0xB4) slot 0x2C */
static inline s32 daPy_getTactMusic(fopAc_ac_c* p) { return gabi::call_ptr<s32>(gabi::load<u32>(p->__vtbl + 0x2C), p); }
/* daPy_py_c::getCutType(): u8 at +0x3AC (HD) */
static inline u8 daPy_getCutType(fopAc_ac_c* p) { return gabi::load<u8>(gabi::ea(p) + 0x3AC); }
/* dComIfGp_checkPlayerStatus1(0, flag): the player status word 1 at play+0x5CDC */
static inline bool dComIfGp_checkPlayerStatus1(u32 flag) { return (gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & flag) != 0; }
enum { daPyStts1_WIND_WAKER_CONDUCT_e = 0x1 };
/* dComIfGp_checkCameraAttentionStatus(dComIfGp_getPlayerCameraID(0), flag): the camera id (s8) at
 * play+0x5B30, the camera entries (0x34 bytes) with the attention status word at play+0x5B00 */
static inline bool dComIfGp_checkCameraAttentionStatus0(u32 flag) {
    s32 id = gabi::load<s8>(dComIfGp_ea() + 0x5B30);
    return (gabi::load<u32>(dComIfGp_ea() + id * 0x34 + 0x5B00) & flag) != 0;
}
/* HD messages: the message manager (*(0x101F4B5C)) is the `this` of
 * 025F7DB0 messageSet(mgr, msgNo, cXyz* pos) -> id (GameCube fopMsgM_messageSet),
 * 025F795C getMesgStatus(): the status byte at play+0x5BB2 (matcher: "fopMsgM_SearchByID"),
 * 025F74D0 setMesgStatus(mgr, u8) */
static inline u32 msgMng() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 fopMsgM_messageSet(u32 mgr, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, mgr, msgNo, pos); }
static inline u8 dComIfGp_getMesgStatus(u32 mgr) { return gabi::call<u8>(0x025F795C, mgr); }
static inline void dComIfGp_setMesgStatus(u32 mgr, u8 st) { gabi::call(0x025F74D0, mgr, st); }
enum { fopMsgStts_BOX_CLOSED_e = 0x12, fopMsgStts_MSG_DESTROYED_e = 0x13 };
/* 025D69FC fopAcM_rollPlayerCrash(actor, f32 dist, u32 flag) */
static inline BOOL fopAcM_rollPlayerCrash(fopAc_ac_c* a, f32 d, u32 f) { return gabi::call<BOOL>(0x025D69FC, a, d, f); }
/* fopAcM_ClearStatusMap / SetStatusMap: the low 6 bits of actor_status */
static inline void fopAcM_ClearStatusMap(fopAc_ac_c* a) { a->actor_status = a->actor_status & ~0x3Fu; }
static inline void fopAcM_SetStatusMap(fopAc_ac_c* a, u32 map) { a->actor_status = (a->actor_status & ~0x3Fu) | map; }
/* dCcD_GObjInf Set bits: Tg SPrm (+0x18), Co SPrm (+0x2C), bit 1 */
static inline void OnTgSetBit(dCcD_Cyl* c) { c->mObjTg.mSPrm |= 1; }
static inline void OffTgSetBit(dCcD_Cyl* c) { c->mObjTg.mSPrm &= ~1u; }
static inline void OnCoSetBit(dCcD_Cyl* c) { c->mObjCo.mSPrm |= 1; }
static inline void OffCoSetBit(dCcD_Cyl* c) { c->mObjCo.mSPrm &= ~1u; }
/* dScnPly_ply_c::setPauseTimer(s8): the pause timer byte (HD 0x101EACB7) */
static inline void dScnPly_setPauseTimer(s8 t) { gabi::store<s8>(0x101EACB7, t); }

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
                fopAcM_ClearStatusMap(a_this);
                u32 atType = gabi::load<u32>(gabi::ea(hitObj) + 0x10); /* GetAtType */
                if (atType == AT_TYPE_SWORD) {
                    goto sword;
                } else if (atType == AT_TYPE_SKULL_HAMMER) {
                    i_this->m29A = 2;
                    if (daPy_getCutType(player) != CUT_TYPE_HAMMER_SIDESWING) {
                        gabi::Local<cXyz> sp18;
                        sp18->copy(*i_this->m5B0.GetTgHitPosP());
                        i_this->m2C0 = 20;
                        i_this->m29B = 0;
                        i_this->m29A = 1;
                        i_this->m2A4 = 0.0f;
                        i_this->m2A8 = 0.0f;
                        dComIfGp_particle_set(dPa_name_ID_AK_JN_OK, sp18, &player->shape_angle);
                        dScnPly_setPauseTimer(1);
                        return;
                    }
                sword: {
                    u32 cut = daPy_getCutType(player);
                    s32 i = 0;
                    for (; i < 32; i++) {
                        if ((u32)(s32)pl_cut_real_no_dt(i) == cut) {
                            break;
                        }
                    }
                    if (i < 32 && pl_cut_no_dt(i) != 0xFF) {
                        i_this->m2C4 = pl_cut_no_dt(i);
                        cut_point_check(i_this);
                    }
                    break;
                }
                } else {
                    i_this->m2C4 = 4;
                    cut_point_check(i_this);
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
        if (sea_water_check(i_this)) {
            i_this->m2BE = 2;
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
        if (sea_water_check(i_this)) {
            i_this->m2BE = 2;
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
        if (sea_water_check(i_this)) {
            i_this->m2BE = 2;
            i_this->m2C0 = 0x28;
        } else if (i_this->m2B2[4] == 0 && shock_damage_check(i_this)) {
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
        sea_water_check(i_this);
    } else if (i_this->m2C0 == base + 1) {
        sea_water_check(i_this);
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
        if (ret_keisan_move(i_this)) {
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
        if (dComIfGp_checkPlayerStatus1(daPyStts1_WIND_WAKER_CONDUCT_e)) {
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
            i_this->m2BE = 0;
            i_this->m2C0 = 10;
            i_this->m29B = 1;
            i_this->m2A4 = 105.0f;
            i_this->m2A8 = 50.0f;
            fopAcM_SetStatusMap(a_this, 0x38);
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
        if (i_this->m2B2[4] == 0 && shock_damage_check(i_this)) {
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
        if (dComIfGp_checkPlayerStatus1(daPyStts1_WIND_WAKER_CONDUCT_e)) {
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
                i_this->m2BE = 100;
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
    WWHD_FUNC(0x0218B234, BOOL, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    for (s32 i = 0; i < 5; i++) {
        if (i_this->m2B2[i] != 0) {
            i_this->m2B2[i] = i_this->m2B2[i] - 1;
        }
    }

    switch (i_this->m2BE) {
    case 0:
        if (gabi::load<u16>(gabi::ea(i_this) + 0xF8) == 1) { /* eventInfo.checkCommandTalk() */
            u32 mng = msgMng();
            if (l_msgId() == fpcM_ERROR_PROCESS_ID_e) {
                if (dComIfGp_checkCameraAttentionStatus0(4 /* dCamAttnStts_00000004_e */)) {
                    l_msgId() = fopMsgM_messageSet(mng, fopAcM_GetParam(&i_this->actor), &i_this->actor.eyePos);
                }
            } else {
                /* HD: the message status lives in the play object */
                if (dComIfGp_getMesgStatus(mng) == fopMsgStts_BOX_CLOSED_e) {
                    dComIfGp_setMesgStatus(mng, fopMsgStts_MSG_DESTROYED_e);
                    dComIfGp_event_reset();
                    l_msgId() = 0xFFFFFFFF;
                }
            }
        } else {
            if (i_this->m2C2 == 0 && i_this->m2C0 != 0x14 && i_this->m2C0 != 0x15) {
                eventInfo_onCondition(&i_this->actor, 1 /* dEvtCnd_CANTALK_e */);
            }
            mother_move(i_this);
        }

        if (i_this->m2C0 != 14) {
            fopAcM_rollPlayerCrash(&i_this->actor, 35.0f, 0);
        }

        i_this->m5B0.SetC(&i_this->actor.current.pos);
        i_this->m5B0.SetH(i_this->m2A4);
        i_this->m5B0.SetR(i_this->m2A8);
        dComIfG_Ccsp_Set(&i_this->m5B0);

        if (i_this->m294 != 1 && i_this->m294 != 0x7FE && daPy_getTactMusic(player) == 4) {
            tact_return(i_this, 1, 30);
        }
        break;

    case 1:
        mother_return_move(i_this);
        break;

    case 2:
        water_swim(i_this, 40);
        if (daPy_getTactMusic(player) == 4) {
            dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->m514);
            tact_return(i_this, 1, 30);
        }
        break;

    case 100:
        parts_move(i_this);
        if (daPy_getTactMusic(player) == 4) {
            tact_return(i_this, 0x65, 0x82);
        } else if (sea_water_check(i_this)) {
            i_this->m2BE = 0x66;
            i_this->m2C0 = 0x8c;
        }
        break;

    case 101:
        chield_parts_move(i_this);
        break;

    case 102:
        water_swim(i_this, 140);
        if (daPy_getTactMusic(player) == 4) {
            dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->m514);
            tact_return(i_this, 0x65, 0x82);
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

    if (i_this->m2BE != 1 && i_this->m2BE != 0x65 && i_this->m2B2[1] == 0) {
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
VERIFY(0x0218B234, daKanban_Execute);

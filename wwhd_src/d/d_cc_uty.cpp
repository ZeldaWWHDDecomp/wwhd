/* Collision attack utilities, GameCube names and fields with HD arithmetic.
 * Verified against WWHD. */
#include "d/d_cc_uty.h"

static const f32 l_speedThreshold = 50.0f;
static const f32 l_particleScale = 2.0f;
// HD: debug-register and pause bytes are guest globals, not host storage.
static be<s16>& l_lastHitPauseAdjustment = *gabi::at<be<s16>>(0x1047B696);
static be<s16>& l_criticalPauseAdjustment = *gabi::at<be<s16>>(0x1047B694);
static be<s16>& l_hitPauseAdjustment = *gabi::at<be<s16>>(0x1047B692);
static be<u8>& l_pauseTimer = *gabi::at<be<u8>>(0x101EACB7);

/* 02518B28 */
u16 cc_pl_cut_bit_get() {
    cc_gameInfo* play = (cc_gameInfo*)dComIfGp_get();
    cc_playerView* player = play->mpPlayer;
    u8 cut = player->mCutType;
    if (cut >= cc_uty::CUT_TYPE_CUT_A && cut <= cc_uty::CUT_TYPE_CUT_EB)
        return 1u << (cut - 1);
    if (cut == cc_uty::CUT_TYPE_CUT_TURN || cut == cc_uty::CUT_TYPE_CUT_ROLL)
        return 0x80;
    return cut == cc_uty::CUT_TYPE_JUMPCUT_SWORD ? 0x100 : 0;
}
static u32 attackSound(dCcD_GObjInf* obj, bool critical) {
    if (obj->mAtType & (cc_uty::AT_TYPE_SKULL_HAMMER | cc_uty::AT_TYPE_STALFOS_MACE))
        return cc_uty::JA_SE_LK_HAMMER_HIT;
    switch (obj->mGObjAt.mSe) {
    case cc_uty::dCcG_SE_WOOD:
        return critical ? cc_uty::JA_SE_LK_W_WEP_CRT_HIT : cc_uty::JA_SE_LK_W_WEP_HIT;
    case cc_uty::dCcG_SE_METAL:
        return critical ? cc_uty::JA_SE_LK_MS_WEP_CRT_HIT : cc_uty::JA_SE_LK_MS_WEP_HIT;
    case cc_uty::dCcG_SE_ARROW:
        return cc_uty::JA_SE_LK_ARROW_HIT;
    case cc_uty::dCcG_SE_HOOKSHOT:
        return cc_uty::JA_SE_LK_HS_SPIKE;
    default:
        return critical ? cc_uty::JA_SE_LK_SW_CRT_HIT : cc_uty::JA_SE_LK_SW_HIT_S;
    }
}
/* 02518BE8 */
u32 at_se_get(dCcD_GObjInf* obj) {
    dCcD_GObjInf* info = cc_getGObjInf(obj);
    if (!info) {
        cc_assert("d_cc_uty.cpp", 0xABu, "hit_obj");
        return cc_uty::JA_SE_LK_SW_HIT_S;
    }
    return attackSound(info, false);
}
static void startSound(fopAc_ac_c* actor, u32 id, u32 arg) {
    cXyz* pos = &actor->eyePos;
    if (gabi::ea(pos)) {
        u32 reverb = cc_getReverb((s32)actor->current.roomNo);
        cc_seStart(id, pos, arg, reverb);
    }
}
/* 02518CC8 */
void def_se_set(fopAc_ac_c* actor, dCcD_GObjInf* obj, u32 arg) {
    u32 id = at_se_get(obj);
    if (actor)
        startSound(actor, id, arg);
}
/* 02518D40 */
void def_se_set_p(fopAc_ac_c* actor, cXyz* pos, dCcD_GObjInf* obj, u32 arg) {
    u32 reverb = cc_getReverb((s32)actor->current.roomNo);
    u32 id = at_se_get(obj);
    cc_seStart(id, pos, arg, reverb);
}
/* 02518DB0 */
fopAc_ac_c* at_power_check(CcAtInfo* at) {
    cc_gameInfo* play = (cc_gameInfo*)dComIfGp_get();
    dCcD_GObjInf* obj = at->mpObj;
    cc_playerView* player = play->mpPlayer;
    if (!obj)
        return 0;
    dCcD_Stts* status = obj->mStts;
    fopAc_ac_c* actor = status ? status->mp_actor.get() : NULL;
    at->mDamage = 0;
    obj = at->mpObj;
    at->mResultingAttackType = cc_uty::RESULT_UNCLASSIFIED;
    at->mpActor = actor;
    u32 flags = obj->mAtType;
    if (flags & cc_uty::AT_TYPE_WATER) {
        at->mResultingAttackType = cc_uty::RESULT_WATER;
        return at->mpActor;
    }
    flags = obj->mAtType;
    if (flags & cc_uty::AT_TYPE_WIND) {
        at->mResultingAttackType = cc_uty::RESULT_WIND;
        return at->mpActor;
    }
    flags = obj->mAtType;
    if (flags & cc_uty::AT_TYPE_UNK20000) {
        at->mResultingAttackType = cc_uty::RESULT_FIRE;
        return at->mpActor;
    }
    flags = obj->mAtType;
    actor = at->mpActor;
    if (flags & (cc_uty::AT_TYPE_FIRE | cc_uty::AT_TYPE_FIRE_ARROW))
        at->mResultingAttackType = cc_uty::RESULT_FIRE;
    if (actor) {
        obj = at->mpObj;
        actor = at->mpActor;
        at->mDamage = obj->mAtp;
        if (actor) {
            s16 name = actor->mProcName;
            if (name == cc_uty::fpcNm_PLAYER_e) {
                obj = at->mpObj;
                if (obj->mAtType & cc_uty::AT_TYPE_SKULL_HAMMER)
                    at->mResultingAttackType = cc_uty::RESULT_HAMMER;
                else {
                    at->mResultingAttackType = cc_uty::RESULT_SWORD;
                    u16 bit = cc_pl_cut_bit_get();
                    at->mPlCutBit = bit;
                    u8 count = player->mCutCount;
                    u32 hit;
                    if (count == 3)
                        hit = 1;
                    else if (count == 4)
                        hit = 4;
                    else {
                        u8 cut = player->mCutType;
                        hit = cut == cc_uty::CUT_TYPE_JUMPCUT_SWORD ? 2
                              : (cut == cc_uty::CUT_TYPE_BT_JUMPCUT ||
                                 cut == cc_uty::CUT_TYPE_BT_ROLLCUT ||
                                 cut == cc_uty::CUT_TYPE_BT_VERTICALJUMPCUT)
                                  ? 3
                                  : 0;
                    }
                    at->mHitSoundId = hit;
                }
            } else if (name == cc_uty::fpcNm_BOMB_e) {
                u32 state = cc_bombCheckState(actor, 8u);
                at->mPlCutBit = 0x200;
                at->mResultingAttackType = state ? cc_uty::RESULT_BOMB_STATE8 : cc_uty::RESULT_BOMB;
            } else if (name == cc_uty::fpcNm_Bomb2_e) {
                at->mResultingAttackType = cc_uty::RESULT_BOMB;
                at->mPlCutBit = 0x200;
            } else if (name == cc_uty::fpcNm_HIMO2_e) {
                at->mResultingAttackType = cc_uty::RESULT_GRAPPLE;
                at->mPlCutBit = 0x400;
            } else if (name == cc_uty::fpcNm_BOOMERANG_e) {
                at->mResultingAttackType = cc_uty::RESULT_BOOMERANG;
                at->mPlCutBit = 0x800;
            } else if (name == cc_uty::fpcNm_ARROW_e)
                at->mPlCutBit = 0x1000;
            else if (name == cc_uty::fpcNm_BDK_e)
                at->mResultingAttackType = cc_uty::RESULT_BDK;
            else if (name == cc_uty::fpcNm_TSUBO_e) {
                u32 type = cc_tsuboGetType(actor, 4u, 0x18u);
                at->mResultingAttackType = type == 2 ? cc_uty::RESULT_WATER : cc_uty::RESULT_POT;
            } else if (name == cc_uty::fpcNm_MO2_e) {
                u8 damage = at->mDamage;
                at->mResultingAttackType = cc_uty::RESULT_MO2;
                if (damage > 2)
                    at->mDamage = 2;
            } else if (name == cc_uty::fpcNm_MT_e) {
                at->mResultingAttackType = cc_uty::RESULT_MT;
                at->mDamage = 4;
            }
        }
    }
    actor = at->mpActor;
    at->mbDead = 0;
    if (actor && actor->mProcName == cc_uty::fpcNm_PLAYER_e) {
        dCcD_GObjInf* info = cc_getGObjInf(at->mpObj);
        if (info->mGObjAt.mSpl == 1) {
            at->mbDead = 1;
            return at->mpActor;
        }
        return at->mpActor;
    }
    if (at->mDamage >= 2) {
        at->mbDead = 1;
        return at->mpActor;
    }
    return actor;
}
/* 025192A8 */
fopAc_ac_c* cc_at_check(fopAc_ac_c* target, CcAtInfo* at) {
    cc_gameInfo* play = (cc_gameInfo*)dComIfGp_get();
    cc_playerView* player = play->mpPlayer;
    at->mHitSoundId = 0;
    fopAc_ac_c* actor = at_power_check(at);
    at->mpActor = actor;
    if (!actor)
        return actor;
    // HD: retain the actor argument passed by the original call site.
    cc_onEnemyDamage(actor);
    actor = at->mpActor;
    cc_speedSquared(&actor->speed);
    // HD: the magnitude helper consumes the vector helper's preserved f1 result.
    f32 magnitude = cc_sqrt((f64)gabi::cpu->f[1].ps0);
    f32 limit = l_speedThreshold;
    actor = at->mpActor;
    f32 x, z;
    if (magnitude > limit) {
        z = actor->speed.z;
        x = actor->speed.x;
    } else {
        f32 tx = target->current.pos.x, ax = actor->current.pos.x, az = actor->current.pos.z;
        x = gabi::fsubs_ppc(tx, ax);
        f32 tz = target->current.pos.z;
        z = gabi::fsubs_ppc(tz, az);
    }
    u16 angle = cc_atan2s((f64)-x, (f64)-z);
    at->m0C.y = angle;
    dCcD_GObjInf* obj = at->mpObj;
    u32 flags = obj->mAtType;
    actor = at->mpActor;
    if ((flags & cc_uty::AT_TYPE_HOOKSHOT) &&
        (target->actor_status &
         (fopAcStts_UNK80000_e | fopAcStts_HOOK_CARRY_e | fopAcStts_UNK200000_e)))
        at->mDamage = 0;
    if (actor && actor->mProcName == cc_uty::fpcNm_HIMO2_e && target->stealItemLeft != 0) {
        s8 left = target->stealItemLeft;
        s32 room = target->current.roomNo;
        u8 bit = target->stealItemBitNo;
        u32 table = target->itemTableIdx;
        target->stealItemLeft = (u8)(left - 1);
        cc_createStealItem(&target->current.pos, table, room, 0u, (u32)bit);
        target->stealItemBitNo = target->stealItemBitNo + 1;
        at->mDamage = 0;
    } else {
        u8 damage = at->mDamage;
        s8 health = target->health;
        if ((s8)damage > 0) {
            health = (s8)(health - damage);
            target->health = (u8)health;
            // HD: spin attacks can apply the damage a second time.
            u8 cut = player->mCutType;
            if ((cut == cc_uty::CUT_TYPE_CUT_TURN || cut == cc_uty::CUT_TYPE_CUT_ROLL) &&
                player->mJumpCutExtraDamage != 0) {
                health = (s8)(health - at->mDamage);
                target->health = (u8)health;
            }
        }
    }
    // HD: the hammer branch skips the pause timer access entirely.
    s8 pause = 6;
    bool updatePause = true;
    if (target->health <= 0) {
        at->mbDead = 1;
        startSound(target, cc_uty::JA_SE_LK_LAST_HIT, 0);
        pause = (s8)(l_lastHitPauseAdjustment + 6);
        cXyz* pos = at->pParticlePos;
        if (pos) {
            play = (cc_gameInfo*)dComIfGp_get();
            dPa_control_c* particle = play->mpParticle;
            cc_particleSet(particle, 0u, cc_uty::ID_AK_JN_CRITICALHITFLASH, pos, 0u, 0u, 0xFFu, 0u,
                           0xFFFFFFFFu, 0u, 0u, 0u);
            gabi::Local<csXyz> rotation;
            gabi::Local<cXyz> scale;
            f32 value = l_particleScale;
            rotation->z = 0;
            scale->x = value;
            scale->z = value;
            rotation->x = 0;
            scale->y = value;
            play = (cc_gameInfo*)dComIfGp_get();
            cc_playerView* p = play->mpPlayer;
            u16 a = cc_searchPlayerAngleY(target, p);
            rotation->y = a;
            pos = at->pParticlePos;
            play = (cc_gameInfo*)dComIfGp_get();
            particle = play->mpParticle;
            cc_particleSet(particle, 0u, cc_uty::ID_AK_JN_OK, pos, rotation.get(), scale.get(),
                           0xFFu, 0u, 0xFFFFFFFFu, 0u, 0u, 0u);
        }
    } else {
        u32 arg = target && target->mProcName == cc_uty::fpcNm_ST_e ? 0x33 : 0x20;
        if (at->mbDead) {
            dCcD_GObjInf* info = cc_getGObjInf(at->mpObj);
            u32 id = attackSound(info, true);
            startSound(target, id, arg);
            if (at->mResultingAttackType == cc_uty::RESULT_HAMMER)
                updatePause = false;
            else
                pause = (s8)(l_criticalPauseAdjustment + 4);
        } else {
            u32 id;
            if (target && target->mProcName == cc_uty::fpcNm_MT_e &&
                ((cc_mtView*)target)->m454 == 2) {
                id = cc_uty::JA_SE_OBJ_MG_BALL_DMG;
                arg = 0;
            } else
                id = at_se_get(at->mpObj);
            startSound(target, id, arg);
            pause = (s8)(l_hitPauseAdjustment + 1);
        }
    }
    if (updatePause && at->mResultingAttackType == cc_uty::RESULT_SWORD)
        l_pauseTimer = (u8)pause;
    if (at->mDamage != 0)
        mDoAud_bgmHitSound(at->mHitSoundId);
    return at->mpActor;
}
/* 02519780 */
static void __sinit_d_cc_uty_cpp() {
    sinit_header_statics_rodata(0x1046F01C, 0x101D5800, 0x1046F028, 0x1004B474);
}
/* WWHD glue
data l_speedThreshold 1004B460
data l_particleScale 1004B468
str 1004B450 "d_cc_uty.cpp"
str 1004B448 "hit_obj"
ext 025157AC dCcD_GObjInf* cc_getGObjInf(dCcD_GObjInf* object)
ext 02520540 u32 cc_getReverb(s32 room)
ext 025E1A40 void cc_seStart(u32 id, cXyz* pos, u32 arg, u32 reverb)
ext 020CB92C u32 cc_bombCheckState(fopAc_ac_c* actor, u32 state)
ext 02048038 u32 cc_tsuboGetType(fopAc_ac_c* actor, u32 shift, u32 width)
ext 025E1FD8 void cc_onEnemyDamage(fopAc_ac_c* actor)
ext 028E8DD0 void cc_speedSquared(cXyz* speed)
ext 028F4384 f32 cc_sqrt(f64 value)
ext 020195B0 u16 cc_atan2s(f64 y, f64 x)
ext 025D9000 void cc_createStealItem(cXyz* pos, u32 table, s32 room, u32 unused, u32 bit)
ext 025A847C void cc_particleSet(dPa_control_c* particle, u32 group, u32 effect, cXyz* pos, csXyz* rotation, cXyz* scale, u32 alpha, u32 callback, u32 room, u32 color, u32 color2, u32 direction)
ext 025D6894 u16 cc_searchPlayerAngleY(fopAc_ac_c* actor, cc_playerView* player)
ext 025E1D30 void mDoAud_bgmHitSound(u32 id)
ext 0273AA24 void cc_assert(const char* file, u32 line, const char* condition)
*/

/**
 * d_a_am_exec.cpp (WWHD)
 * Enemy - Armos: daAM_Execute
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_am.cpp) to the WWHD layout and verified against cking.rpx.
 * GHS inlined medama_move, medama_atari_check, action_dousa, action_modoru_move,
 * action_handou_move and action_itai_move into daAM_Execute: they are inline helpers here.
 */
#include "d/actor/d_a_am.h"

static inline bool am_isStop(am_class* i_this) { return i_this->mpMorf->isStop(); }

/* dComIfGp_getVibration().StartShock(strength, -0x21, cXyz(0.0f, 1.0f, 0.0f)) */
static inline void am_shock(s32 strength) {
    dVibration_c* vib = dComIfGp_getVibration();
    gabi::Local<cXyz> dir;
    dir->set(0.0f, 1.0f, 0.0f);
    gabi::call(0x025CB374, vib, strength, -0x21, dir.get());
}

static inline void am_smoke(am_class* i_this, u16 id, cXyz* pos, s32 idx) {
    am_particle_setToon(id, pos, &i_this->shape_angle, 0xB9, &i_this->mSmokeCbs[idx], fopAcM_GetRoomNo(i_this));
}

/* medama_move (inline) */
static inline void medama_move(am_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (i_this->mCurrBckIdx == dRes_INDEX_AM_BCK_SLEEP_e || i_this->mCurrBckIdx == dRes_INDEX_AM_BCK_SLEEP_LOOP_e) {
        i_this->mEyeRot.x = 0;
        i_this->mEyeRot.y = 0;
        i_this->mEyeRot.z = 0;
        return;
    }

    f32 diffX = i_this->current.pos.x - player->current.pos.x;
    f32 diffY = i_this->eyePos.y - player->current.pos.y;
    f32 diffZ = i_this->current.pos.z - player->current.pos.z;

    s16 y = cM_atan2s(diffX, diffZ);
    if (y < -0x71C) {
        y = -0x71C;
    } else if (y > 0x71C) {
        y = 0x71C;
    }
    i_this->mTargetEyeRot.y = y;

    s16 x = cM_atan2s(diffY, std_sqrtf(gabi::fmadds(diffX, diffX, diffZ * diffZ)));
    if (x < -0x38E) {
        x = -0x38E;
    } else if (x > 0x38E) {
        x = 0x38E;
    }
    i_this->mTargetEyeRot.x = x;

    cLib_addCalcAngleS2(&i_this->mEyeRot.x, i_this->mTargetEyeRot.x, 1, 0x500);
    cLib_addCalcAngleS2(&i_this->mEyeRot.y, i_this->mTargetEyeRot.y, 1, 0x500);
}

/* medama_atari_check (inline) */
static inline BOOL medama_atari_check(am_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    void* hitObj = i_this->mEyeSph.GetTgHitObj();
    bool ret = false;

    if (i_this->mStartsInactive == 1 && i_this->mSwitch != 0xFF &&
        !dComIfGs_isSwitch(i_this->mSwitch, dComIfGp_roomControl_getStayNo()))
    {
        return ret;
    }

    i_this->mStts.Move();
    if (!i_this->mEyeSph.ChkTgHit()) {
        return ret;
    }
    if (!hitObj) {
        return ret;
    }

    gabi::Local<CcAtInfo_am> atInfo;
    gabi::Local<cXyz> hitPos;
    hitPos->copy(*i_this->mEyeSph.GetTgHitPosP());
    atInfo->pParticlePos = 0;

    switch (gabi::load<u32>(gabi::ea(hitObj) + 0x10) /* GetAtType() */) {
    case AT_TYPE_GRAPPLING_HOOK:
        if (i_this->mCurrBckIdx != dRes_INDEX_AM_BCK_SLEEP_e && i_this->mCurrBckIdx != dRes_INDEX_AM_BCK_SLEEP_LOOP_e) {
            if (i_this->stealItemLeft > 0) {
                i_this->max_health = 10;
                i_this->health = 10;
                atInfo->mpObj = gabi::ea(i_this->mEyeSph.GetTgHitObj());
                atInfo->pParticlePos = 0;
                cc_at_check(i_this, atInfo);
                i_this->max_health = 10;
                i_this->health = 10;
                dComIfGp_particle_set(ID_IT_JN_PIYOHIT00, attention_pos(i_this));
            } else {
                dComIfGp_particle_set(ID_AK_JN_NG, hitPos);
            }
            fopAcM_seStart(i_this, JA_SE_LK_MS_WEP_HIT, 0x42);
        }
        break;
    case AT_TYPE_SWORD:
    case AT_TYPE_MACHETE:
    case AT_TYPE_UNK800:
    case AT_TYPE_DARKNUT_SWORD:
    case AT_TYPE_MOBLIN_SPEAR:
        fopAcM_seStart(i_this, JA_SE_LK_SW_HIT_S, 0x42);
        break;
    case AT_TYPE_BOOMERANG:
    case AT_TYPE_BOKO_STICK:
    case AT_TYPE_UNK2000:
    case AT_TYPE_STALFOS_MACE:
        fopAcM_seStart(i_this, JA_SE_LK_W_WEP_HIT, 0x42);
        break;
    case AT_TYPE_LIGHT_ARROW:
        ret = true;
        i_this->mEnemyIce.mLightShrinkTimer = 1;
        i_this->mEnemyIce.mParticleScale = 1.0f;
        i_this->mEnemyIce.mYOffset = 80.0f;
        attention_flags(i_this) = 0;
        break;
    case AT_TYPE_NORMAL_ARROW:
    case AT_TYPE_FIRE_ARROW:
    case AT_TYPE_ICE_ARROW:
        fopAcM_seStart(i_this, JA_SE_LK_MS_WEP_HIT, 0x42);
        ret = true;
        if (i_this->mCurrBckIdx == dRes_INDEX_AM_BCK_SLEEP_e || i_this->mCurrBckIdx == dRes_INDEX_AM_BCK_SLEEP_LOOP_e) {
            anm_init(i_this, dRes_INDEX_AM_BCK_OKIRU_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            attention_flags(i_this) = fopAc_Attn_LOCKON_BATTLE_e;
            needle_onAt(&i_this->mNeedleCyl);
            i_this->mAction = ACTION_DOUSA;
            i_this->mMode = MODE_DOUSA_OKIRU;
        } else {
            dComIfGp_particle_set(ID_AK_JN_CRITICALHITFLASH, &i_this->mEyeballPos, &player->shape_angle);
            fopAcM_seStart(i_this, JA_SE_CM_AM_EYE_DAMAGE, 0);
            am_monsSeStart(i_this, JA_SE_CV_AM_EYE_DAMAGE, 0); /* HD: param 0 (GameCube 0x42) */
            i_this->mAction = ACTION_ITAI_MOVE;
            i_this->mMode = MODE_ITAI_MOVE_INIT;
        }
        break;
    default:
        dComIfGp_particle_set(ID_AK_JN_NG, hitPos);
        fopAcM_seStart(i_this, JA_SE_LK_MS_WEP_HIT, 0x42);
        break;
    }

    if (ret) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/* action_dousa (inline) */
static inline void action_dousa(am_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    switch (i_this->mMode) {
    case MODE_DOUSA_INIT:
        for (int i = 0; i < 5; i++) {
            i_this->mCountUpTimers[i] = 0;
        }
        anm_init(i_this, dRes_INDEX_AM_BCK_SLEEP_LOOP_e, 1.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
        i_this->mMode = i_this->mMode + 1;
        /* fall-through */
    case 1: {
        if (i_this->mStartsInactive == 1 && i_this->mSwitch != 0xFF &&
            !dComIfGs_isSwitch(i_this->mSwitch, dComIfGp_roomControl_getStayNo()))
        {
            break;
        }
        i_this->actor_status |= fopAcStts_SHOWMAP_e;
        if (fopAcM_searchPlayerDistance(i_this) < 1000.0f) {
            f32 yDist = player->current.pos.y - i_this->current.pos.y;
            yDist = std_sqrtf(yDist * yDist);
            if (yDist > 300.0f) {
                break;
            }
            gabi::Local<cXyz> dest;
            dest->copy(player->current.pos);
            if (Line_check(i_this, dest)) {
                anm_init(i_this, dRes_INDEX_AM_BCK_OKIRU_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
                am_monsSeStart(i_this, JA_SE_CV_AM_AWAKE, 0);
                attention_flags(i_this) = fopAc_Attn_LOCKON_BATTLE_e;
                needle_onAt(&i_this->mNeedleCyl);
                i_this->mMode = i_this->mMode + 1; /* MODE_DOUSA_OKIRU */
            }
        }
        break;
    }
    case MODE_DOUSA_OKIRU:
        if (!am_isStop(i_this)) {
            break;
        }
        i_this->mMode = i_this->mMode + 1;
        /* fall-through */
    case 3:
        if (i_this->mCurrBckIdx != dRes_INDEX_AM_BCK_CLOSE_e && i_this->mCurrBckIdx != dRes_INDEX_AM_BCK_CLOSE_LOOP_e) {
            if (i_this->mCurrBckIdx != dRes_INDEX_AM_BCK_DAMAGE_END_e) {
                fopAcM_seStart(i_this, JA_SE_CM_AM_NEEDLE_OUT, 0);
                fopAcM_seStart(i_this, JA_SE_CM_AM_MOUTH_CLOSE, 0);
            }
            anm_init(i_this, dRes_INDEX_AM_BCK_CLOSE_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->mCountDownTimers[2] = 6;
        }
        i_this->mTargetAngleY = fopAcM_searchPlayerAngleY(i_this);
        i_this->mMode = i_this->mMode + 1;
        /* fall-through */
    case 4: {
        if (i_this->mCountDownTimers[2] == 1) {
            needle_onAt(&i_this->mNeedleCyl);
        }
        if (i_this->mCountDownTimers[2] != 0) {
            break;
        }
        if (i_this->mType & 1) {
            f32 zDist = i_this->current.pos.z - i_this->mSpawnPos.z;
            f32 xDist = i_this->current.pos.x - i_this->mSpawnPos.x;
            f32 xzDist = std_sqrtf(gabi::fmadds(xDist, xDist, zDist * zDist));
            if (!(xzDist <= i_this->mAreaRadius)) { /* GHS: ble */
                i_this->mAction = ACTION_MODORU_MOVE;
                i_this->mMode = MODE_MODORU_MOVE_INIT;
                return;
            }
        } else {
            if (fopAcM_searchPlayerDistance(i_this) > 2000.0f) {
                i_this->mMode = MODE_DOUSA_SLEEP_INIT;
                break;
            }
            f32 yDist = player->current.pos.y - i_this->current.pos.y;
            yDist = std_sqrtf(yDist * yDist);
            if (!(yDist <= 300.0f)) { /* GHS: ble */
                i_this->mMode = MODE_DOUSA_SLEEP_INIT;
                break;
            }
        }
        s16 yRotDiff = (s16)cLib_distanceAngleS(i_this->shape_angle.y, i_this->mTargetAngleY);
        if (yRotDiff < 0x100) {
            i_this->mMode = i_this->mMode + 1;
        }
        break;
    }
    case 5: {
        i_this->speedF = 30.0f;
        i_this->gravity = -11.0f;
        i_this->speed.y = 40.0f;
        am_monsSeStart(i_this, JA_SE_CV_AM_JUMP, 0);
        gabi::Local<cXyz> dest;
        dest->copy(player->current.pos);
        if (!Line_check(i_this, dest) || daPy_getDamageWaitTimer(player) != 0) {
            i_this->speedF = 0.0f;
        }
        i_this->mMode = i_this->mMode + 1;
        break;
    }
    case 6:
        if (i_this->mCurrBckIdx == dRes_INDEX_AM_BCK_CLOSE_e) {
            if (am_isStop(i_this)) {
                anm_init(i_this, dRes_INDEX_AM_BCK_CLOSE_LOOP_e, 1.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            }
        }
        if (!i_this->mAcch.ChkGroundHit()) {
            break;
        }
        fopAcM_seStart(i_this, JA_SE_CM_AM_JUMP, 0);
        i_this->mSmokeCbs[0].remove();
        am_smoke(i_this, ID_AK_ST_AMOTHSMOKE00, &i_this->mWaistPos, 0);
        am_shock(3);
        i_this->speedF = 0.0f;
        i_this->mCountDownTimers[0] = 0;
        if (i_this->mCountUpTimers[0] < 2) {
            i_this->mCountDownTimers[0] = 10;
        }
        i_this->mMode = i_this->mMode + 1;
        /* fall-through */
    case 7:
        if (i_this->mCountDownTimers[0] != 0) {
            break;
        }
        i_this->mCountUpTimers[0] = i_this->mCountUpTimers[0] + 1;
        if (i_this->mCountUpTimers[0] > 2) {
            i_this->mCountDownTimers[0] = 100;
            i_this->mCountUpTimers[0] = 0;
            anm_init(i_this, dRes_INDEX_AM_BCK_DAMAGE_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            fopAcM_seStart(i_this, JA_SE_CM_AM_NEEDLE_IN, 0);
            fopAcM_seStart(i_this, JA_SE_CM_AM_MOUTH_OPEN, 0);
            am_monsSeStart(i_this, JA_SE_CV_AM_OPEN_MOUTH, 0);
            needle_offAt(&i_this->mNeedleCyl);
            needle_offAt(&i_this->mNeedleCyl);
            if (i_this->mSmokeCbs[2].getEmitter() == nullptr) {
                am_smoke(i_this, ID_AK_ST_AMOTHSMOKE02, &i_this->mWaistPos, 2);
            }
            i_this->mMode = 8;
        } else {
            i_this->mMode = 3;
        }
        break;
    case 8:
        if (i_this->mAcch.ChkGroundHit()) {
            i_this->gravity = -6.0f;
            i_this->speed.y = 15.0f;
            fopAcM_seStart(i_this, JA_SE_CM_AM_JUMP_S, 0);
            i_this->mTargetAngleY = fopAcM_searchPlayerAngleY(i_this);
        }
        if (i_this->mCountDownTimers[0] == 0) {
            i_this->mSmokeCbs[2].remove();
            i_this->mMode = 3;
        }
        break;
    case MODE_DOUSA_SLEEP_INIT:
        anm_init(i_this, dRes_INDEX_AM_BCK_SLEEP_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        fopAcM_seStart(i_this, JA_SE_CM_AM_NEEDLE_IN, 0);
        needle_offAt(&i_this->mNeedleCyl);
        needle_offAt(&i_this->mNeedleCyl);
        attention_flags(i_this) = 0;
        i_this->mMode = i_this->mMode + 1;
        break;
    case MODE_DOUSA_SLEEP_MAIN:
        if (am_isStop(i_this)) {
            i_this->mMode = 0;
        }
        break;
    }

    medama_move(i_this);

    if (i_this->mMode != 2 && medama_atari_check(i_this)) {
        i_this->mSmokeCbs[2].remove();
    } else if (bomb_nomi_check(i_this)) {
        i_this->mSmokeCbs[2].remove();
    }
}

/* action_modoru_move (inline) */
static inline void action_modoru_move(am_class* i_this) {
    switch (i_this->mMode) {
    case MODE_MODORU_MOVE_INIT: {
        anm_init(i_this, dRes_INDEX_AM_BCK_CLOSE_LOOP_e, 1.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
        needle_onAt(&i_this->mNeedleCyl);
        i_this->gravity = -11.0f;
        i_this->speed.y = 40.0f;
        i_this->speedF = 15.0f;
        am_monsSeStart(i_this, JA_SE_CV_AM_JUMP, 0); /* HD: param 0 (GameCube 0x42) */

        f32 xDistToSpawn = i_this->mSpawnPos.x - i_this->current.pos.x;
        f32 zDistToSpawn = i_this->mSpawnPos.z - i_this->current.pos.z;
        i_this->mTargetAngleY = cM_atan2s(xDistToSpawn, zDistToSpawn);
        i_this->mMode = i_this->mMode + 1;
        break;
    }
    case MODE_MODORU_MOVE_MAIN: {
        f32 xDistToSpawn = i_this->mSpawnPos.x - i_this->current.pos.x;
        f32 zDistToSpawn = i_this->mSpawnPos.z - i_this->current.pos.z;
        if (i_this->mAcch.ChkGroundHit()) {
            i_this->mSmokeCbs[0].remove();
            am_smoke(i_this, ID_AK_ST_AMOTHSMOKE00, &i_this->mWaistPos, 0);
            am_shock(1);
            fopAcM_seStart(i_this, JA_SE_CM_AM_JUMP, 0);
            am_monsSeStart(i_this, JA_SE_CV_AM_JUMP, 0); /* HD: param 0 (GameCube 0x42) */

            i_this->speed.y = 40.0f;
            i_this->speedF = 15.0f;
            i_this->mTargetAngleY = cM_atan2s(xDistToSpawn, zDistToSpawn);
        }

        f32 xzDist = std_sqrtf(gabi::fmadds(xDistToSpawn, xDistToSpawn, zDistToSpawn * zDistToSpawn));
        if (xzDist < 20.0f) {
            i_this->mTargetAngleY = i_this->mSpawnRotY;
            i_this->speedF = 0.0f;
            i_this->mMode = i_this->mMode + 1;
        }
        break;
    }
    case MODE_MODORU_MOVE_END: {
        s16 angleDiff = (s16)cLib_distanceAngleS(i_this->shape_angle.y, i_this->mTargetAngleY);
        if (angleDiff < 0x100) {
            needle_offAt(&i_this->mNeedleCyl);
            needle_offAt(&i_this->mNeedleCyl);
            attention_flags(i_this) = 0;
            i_this->mAction = ACTION_DOUSA;
            i_this->mMode = MODE_DOUSA_INIT;
        }
        break;
    }
    }
}

/* action_handou_move (inline) */
static inline void action_handou_move(am_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    switch (i_this->mMode) {
    case MODE_HANDOU_MOVE_INIT: {
        i_this->speedF = 20.0f;
        s16 angleToPlayer = fopAcM_searchPlayerAngleY(i_this);
        i_this->current.angle.y = angleToPlayer + 0x8000;
        if (i_this->mHugeKnockback == 1) {
            i_this->current.angle.y = player->shape_angle.y - 0x4000;
            i_this->speedF = 40.0f;
        }
        i_this->mTargetAngleY = i_this->current.angle.y;
        if (i_this->mCurrBckIdx != dRes_INDEX_AM_BCK_CLOSE_e && i_this->mCurrBckIdx != dRes_INDEX_AM_BCK_CLOSE_LOOP_e) {
            fopAcM_seStart(i_this, JA_SE_CM_AM_NEEDLE_OUT, 0);
            fopAcM_seStart(i_this, JA_SE_CM_AM_MOUTH_CLOSE, 0);
            anm_init(i_this, dRes_INDEX_AM_BCK_CLOSE_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        }
        i_this->mMode = i_this->mMode + 1;
        /* fall-through */
    }
    case MODE_HANDOU_MOVE_MAIN: {
        cLib_addCalc0(&i_this->speedF, 0.8f, 2.0f);
        if (i_this->speedF < 0.1f) {
            i_this->speedF = 0.0f;
            i_this->mCountDownTimers[2] = 6;
            i_this->current.angle.y = i_this->shape_angle.y;
            i_this->mAction = ACTION_DOUSA;
            i_this->mMode = 3;
        }
        break;
    }
    }
}

static inline void am_bomb_detonate_now(fopAc_ac_c* swallowedActor) {
    swallowedActor->scale.set(1.0f, 1.0f, 1.0f);
    if (swallowedActor != nullptr && am_GetName(swallowedActor) == PROC_BOMB) {
        daBomb_setBombRestTime(swallowedActor, 1);
    } else if (swallowedActor != nullptr && am_GetName(swallowedActor) == PROC_BOMB2) {
        daBomb2_set_time(swallowedActor, 1);
    }
}

/* action_itai_move (inline) */
static inline void action_itai_move(am_class* i_this) {
    switch (i_this->mMode) {
    case MODE_ITAI_MOVE_INIT:
        i_this->mEyeRot.x = 0;
        i_this->mEyeRot.y = 0;
        i_this->mEyeRot.z = 0;
        needle_offAt(&i_this->mNeedleCyl);
        needle_offAt(&i_this->mNeedleCyl);
        i_this->speedF = -20.0f;
        i_this->current.angle.y = fopAcM_searchPlayerAngleY(i_this);
        i_this->mTargetAngleY = i_this->current.angle.y;
        fopAcM_seStart(i_this, JA_SE_CM_AM_NEEDLE_IN, 0);
        anm_init(i_this, dRes_INDEX_AM_BCK_DAMAGE_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        i_this->mMode = i_this->mMode + 1;
        /* fall-through */
    case 41:
        cLib_addCalc0(&i_this->speedF, 0.8f, 2.0f);
        if (!am_isStop(i_this)) {
            break;
        }
        i_this->mCountDownTimers[0] = 100;
        i_this->speedF = 0.0f;
        anm_init(i_this, dRes_INDEX_AM_BCK_DAMAGE_LOOP_e, 1.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
        i_this->mMode = i_this->mMode + 1;
        break;
    case 42:
        if (i_this->mCountDownTimers[0] != 0) {
            break;
        }
        anm_init(i_this, dRes_INDEX_AM_BCK_DAMAGE_END_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        fopAcM_seStart(i_this, JA_SE_CM_AM_NEEDLE_OUT, 0);
        fopAcM_seStart(i_this, JA_SE_CM_AM_MOUTH_CLOSE, 0);
        i_this->mMode = i_this->mMode + 1;
        break;
    case 43:
        if (!am_isStop(i_this)) {
            break;
        }
        needle_onAt(&i_this->mNeedleCyl);
        i_this->mCountUpTimers[0] = 0;
        i_this->mAction = ACTION_DOUSA;
        i_this->mMode = 3;
        break;
    case 44:
        i_this->mSmokeCbs[3].remove();
        i_this->mStts.SetWeight(0xFF);
        am_smoke(i_this, ID_AK_ST_AMOTHSMOKE03, &i_this->mJawPos, 3);
        fopAcM_seStart(i_this, JA_SE_CM_AM_MOUTH_CLOSE, 0);
        anm_init(i_this, dRes_INDEX_AM_BCK_BOM_NOMI_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        mDoAud_onEnemyDamage();
        i_this->mEyeRot.x = 0;
        i_this->mEyeRot.y = 0;
        i_this->mEyeRot.z = 0;
        needle_offAt(&i_this->mNeedleCyl);
        needle_offAt(&i_this->mNeedleCyl);
        i_this->mCountDownTimers[1] = 10;
        i_this->mMode = i_this->mMode + 1;
        /* fall-through */
    case 45:
        bomb_move_set(i_this, 0);
        if (i_this->mpMorf->checkFrame(3.0f)) {
            fopAcM_seStart(i_this, JA_SE_CM_AM_EAT_BOMB, 0);
            am_monsSeStart(i_this, JA_SE_CV_AM_EAT_BOMB, 0);
        }
        if (i_this->mpMorf->checkFrame(6.0f)) {
            i_this->mSmokeCbs[1].remove();
            needle_onAt(&i_this->mNeedleCyl);
            am_smoke(i_this, ID_AK_ST_AMOTHSMOKE01, &i_this->mJawPos, 1);
            i_this->m033C = dComIfGp_particle_set(ID_AK_SN_AMOTHBOMBEYE, &i_this->mJawPos);
            i_this->m0340 = dComIfGp_particle_set(ID_AK_SN_AMOTHBOMBMOUTH, &i_this->mJawPos);
        }

        if (!am_isStop(i_this)) {
            break;
        }
        i_this->mCountDownTimers[0] = 100;
        i_this->mTargetAngleY = fopAcM_searchPlayerAngleY(i_this);
        i_this->mMode = i_this->mMode + 1;
        break;
    case 46:
        bomb_move_set(i_this, 1);
        i_this->shape_angle.y = i_this->shape_angle.y + 0x1000;
        if (i_this->mAcch.ChkGroundHit()) {
            i_this->mSmokeCbs[0].remove();
            fopAcM_seStart(i_this, JA_SE_CM_AM_JUMP, 0);
            am_smoke(i_this, ID_AK_ST_AMOTHSMOKE00, &i_this->mWaistPos, 0);
            am_shock(1);
            fopAcM_seStart(i_this, JA_SE_CM_AM_JUMP_L, 0);
            am_monsSeStart(i_this, JA_SE_CV_AM_JITABATA, 0);
            i_this->speed.y = 25.0f;
            i_this->gravity = -10.0f;
            i_this->speedF = 10.0f;
        }

        if (i_this->mCountDownTimers[0] != 0) {
            break;
        }
        anm_init(i_this, dRes_INDEX_AM_BCK_DEAD_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        dComIfGp_particle_set(ID_AK_SN_AMOTHFLASH00, &i_this->mWaistPos);
        dComIfGp_particle_set(ID_AK_SN_AMOTHHAHEN00, &i_this->mWaistPos);

        fopAcM_seStart(i_this, JA_SE_CM_AM_BEF_EXPLODE, 0);
        i_this->mTargetAngleY = i_this->current.angle.y;

        if (i_this->m033C) {
            JPA_becomeInvalidEmitter(i_this->m033C);
            i_this->m033C = nullptr;
        }
        if (i_this->m0340) {
            JPA_becomeInvalidEmitter(i_this->m0340);
            i_this->m0340 = nullptr;
        }
        i_this->speedF = 0.0f;
        i_this->mMode = i_this->mMode + 1;
        break;
    case 47: {
        bomb_move_set(i_this, 1);
        if (!am_isStop(i_this)) {
            break;
        }
        gabi::Local<cXyz> centerPos;
        centerPos->copy(i_this->current.pos);
        centerPos->y = i_this->current.pos.y + 150.0f;
        u32 pid = i_this->mSwallowedActorPID;
        if (pid != fpcM_ERROR_PROCESS_ID_e) {
            fopAc_ac_c* swallowedActor = fopAcM_SearchByID(pid);
            if (swallowedActor) {
                am_bomb_detonate_now(swallowedActor);
            }
        }

        fopAcM_seStart(i_this, JA_SE_CM_AM_EXPLODE, 0);
        fopAcM_seStart(i_this, JA_SE_LK_LAST_HIT, 0);

        fopAcM_createDisappear(i_this, centerPos, 5, 0, 0xFF);
        fopAcM_onActor(i_this);
        fopAcM_delete(i_this);
        break;
    }
    }

    if (i_this->m033C) {
        JPA_setGlobalRTMatrix(i_this->m033C, getAnmMtx(i_this->mpMorf->getModel(), AM_JNT_AGO_e));
    }
    if (i_this->m0340) {
        JPA_setGlobalRTMatrix(i_this->m0340, getAnmMtx(i_this->mpMorf->getModel(), AM_JNT_AGO_e));
    }

    if (i_this->mMode == 41 || i_this->mMode == 42) {
        bomb_nomi_check(i_this);
    }
}

/* 0204938C */
static BOOL daAM_Execute(am_class* i_this) {
    WWHD_FUNC(0x0204938C, BOOL, i_this);

    fopAcM_setGbaName(i_this, 0x27 /* dItemNo_BOW_e */, 0xC, 0x2A);

    if (enemy_ice(&i_this->mEnemyIce)) {
        J3DModel* model = i_this->mpMorf->getModel();
        J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
        i_this->mpMorf->calc();
        return TRUE;
    }

    for (int i = 0; i < 4; i++) {
        if (i_this->mCountDownTimers[i] != 0) {
            i_this->mCountDownTimers[i] = i_this->mCountDownTimers[i] - 1;
        }
    }

    switch (i_this->mAction) {
    case ACTION_DOUSA:
        action_dousa(i_this);
        break;
    case ACTION_MODORU_MOVE:
        action_modoru_move(i_this);
        break;
    case ACTION_HANDOU_MOVE:
        action_handou_move(i_this);
        break;
    case ACTION_ITAI_MOVE:
        action_itai_move(i_this);
        break;
    }

    if (i_this->mAction != ACTION_ITAI_MOVE && i_this->mSpawnPosY - 1500.0f > i_this->current.pos.y) {
        anm_init(i_this, dRes_INDEX_AM_BCK_DEAD_e, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);

        dComIfGp_particle_set(ID_AK_SN_AMOTHFLASH00, &i_this->mWaistPos);
        dComIfGp_particle_set(ID_AK_SN_AMOTHHAHEN00, &i_this->mWaistPos);

        am_seStart(i_this, JA_SE_CM_AM_BEF_EXPLODE, 0); /* the out-of-line fopAcM_seStart */

        i_this->mTargetAngleY = i_this->current.angle.y;

        /* becomeInvalidEmitter: here through the out-of-line status setter */
        if (i_this->m033C) {
            gabi::store<s32>(gabi::ea(i_this->m033C.get()) + 0x5C, -1);
            JPABaseEmitter_onStatus(i_this->m033C, 1);
            i_this->m033C = nullptr;
        }
        if (i_this->m0340) {
            gabi::store<s32>(gabi::ea(i_this->m0340.get()) + 0x5C, -1);
            JPABaseEmitter_onStatus(i_this->m0340, 1);
            i_this->m0340 = nullptr;
        }

        i_this->speedF = 0.0f;
        i_this->mAction = ACTION_ITAI_MOVE;
        i_this->mMode = 47;
    }

    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mTargetAngleY, 1, 0x500);
    if (i_this->mMode != 46 && i_this->mMode != 47 && i_this->mMode != 31) {
        cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 1, 0x500);
    }

    if (i_this->mCountDownTimers[1] == 0) {
        i_this->mpMorf->play(nullptr, 0, 0);
    }

    cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    offset->y = 0.0f;
    gabi::store<u32>(gabi::ea(&offset->z), gabi::load<u32>(gabi::ea(&i_this->speedF))); /* offset.z = speedF (bit copy) */
    gabi::Local<cXyz> rotOffset;
    MtxPosition(offset, rotOffset);
    gabi::store<u32>(gabi::ea(&i_this->speed.x), gabi::load<u32>(gabi::ea(&rotOffset->x)));
    gabi::store<u32>(gabi::ea(&i_this->speed.z), gabi::load<u32>(gabi::ea(&rotOffset->z)));
    f32 sy = i_this->speed.y + i_this->gravity;
    if (sy < -100.0f) {
        sy = -100.0f;
    }
    i_this->speed.y = sy;

    body_atari_check(i_this);

    cXyz* attn = attention_pos(i_this);
    gabi::store<u32>(gabi::ea(&attn->x), gabi::load<u32>(gabi::ea(&i_this->current.pos.x)));
    attn->y = i_this->current.pos.y + 330.0f;
    gabi::store<u32>(gabi::ea(&attn->z), gabi::load<u32>(gabi::ea(&i_this->current.pos.z)));
    gabi::store<u32>(gabi::ea(&i_this->eyePos.x), gabi::load<u32>(gabi::ea(&i_this->current.pos.x)));
    i_this->eyePos.y = i_this->current.pos.y + 250.0f;
    gabi::store<u32>(gabi::ea(&i_this->eyePos.z), gabi::load<u32>(gabi::ea(&i_this->current.pos.z)));

    gabi::Local<cXyz> needlePos;
    needlePos->copy(i_this->current.pos);

    i_this->mEyeSph.SetC(&i_this->mEyeballPos);
    i_this->mEyeSph.SetR(60.0f);
    dComIfG_Ccsp_Set(&i_this->mEyeSph);

    i_this->mMouthSph.SetC(&i_this->mMouthPos);
    i_this->mMouthSph.SetR(100.0f);
    dComIfG_Ccsp_Set(&i_this->mMouthSph);

    i_this->mBodyCyl.SetC(&i_this->current.pos);
    i_this->mBodyCyl.SetH(300.0f);
    i_this->mBodyCyl.SetR(80.0f);
    dComIfG_Ccsp_Set(&i_this->mBodyCyl);

    needlePos->y = needlePos->y + 40.0f;
    i_this->mNeedleCyl.SetC(needlePos);
    i_this->mNeedleCyl.SetH(30.0f);
    i_this->mNeedleCyl.SetR(130.0f);
    dComIfG_Ccsp_Set(&i_this->mNeedleCyl);

    fopAcM_posMove(i_this, &i_this->mStts.m_cc_move);
    BG_check(i_this);
    draw_SUB(i_this);

    return TRUE;
}
VERIFY(0x0204938C, daAM_Execute);

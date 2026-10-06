/**
 * d_a_npc_ji1_teachsp.cpp (WWHD)
 * NPC - Orca: spin attack lesson (teachSPRollCutAction, teachSpRollCutMove), checkCutType,
 * battleGuardCheck.
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* daPy_py_c::getCutType(): u8 at player+0x3AC */
static inline u8 player_getCutType(fopAc_ac_c* pl) { return gabi::load<u8>(gabi::ea(pl) + 0x3AC); }
/* daPy_py_c::getCutAtFlg(): mModeFlg (+0x3B8) & 0x40 */
static inline u32 player_getCutAtFlg(fopAc_ac_c* pl) { return gabi::load<u32>(gabi::ea(pl) + 0x3B8) & 0x40; }
/* daPy_py_c::checkComboCutTurn(): virtual, HD vtable (+0xB4) slot 0x94 */
static inline BOOL player_checkComboCutTurn(fopAc_ac_c* pl) {
    return gabi::call_ptr<BOOL>(gabi::load<u32>(gabi::load<u32>(gabi::ea(pl) + 0xB4) + 0x94), pl);
}
/* dComIfGs_getMaxMagic(): save info byte at *(0x101F84DC) + 0x33 */
static inline u8 dComIfGs_getMaxMagic() { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x33); }
/* dComIfGp_setItemMagicCount(n): play s16 at +0x5B60 += n */
static inline void dComIfGp_setItemMagicCount(s16 n) {
    u32 p = dComIfGp_ea() + 0x5B60;
    gabi::store<s16>(p, (s16)(gabi::load<s16>(p) + n));
}
/* dComIfGp_setItemBeastNumCount(dBeastIdx_KNIGHTS_CREST_e, n): play s16 at +0x5B76 += n */
static inline void dComIfGp_setItemBeastNumCount_crest(s16 n) {
    u32 p = dComIfGp_ea() + 0x5B76;
    gabi::store<s16>(p, (s16)(gabi::load<s16>(p) + n));
}
/* temporary event flags: dSv_event_c at *(0x101F84DC) + 0x1178 */
static inline dSv_event_c* dComIfGs_tmpEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178); }
static inline void dComIfGs_onTmpBit(u16 f) { dSv_event_onEventBit(dComIfGs_tmpEvent(), f); }
/* 025B8B7C dSv_event_c::offEventBit */
static inline void dComIfGs_offTmpBit(u16 f) { gabi::call(0x025B8B7C, dComIfGs_tmpEvent(), f); }
/* 0200F164 cLib_addCalcPos2(cXyz*, const cXyz&, f32, f32) */
static inline void cLib_addCalcPos2(cXyz* p, cXyz* t, f32 scale, f32 maxStep) { gabi::call(0x0200F164, p, t, scale, maxStep); }
/* dComIfGp_plusMiniGameRupee(n): play s16 at +0x5CEC, clamped at 0 */
static inline void dComIfGp_plusMiniGameRupee(s32 n) {
    u32 p = dComIfGp_ea() + 0x5CEC;
    s16 v = gabi::load<s16>(p);
    if (v + n > 0) {
        gabi::store<s16>(p, (s16)(v + n));
    } else {
        gabi::store<s16>(p, 0);
    }
}
/* dComIfGp_setMessageCountNumber(n): play s16 at +0x5BA0 */
static inline void dComIfGp_setMessageCountNumber(s16 n) { gabi::store<s16>(dComIfGp_ea() + 0x5BA0, n); }
/* dComIfG_getTimerPtr(): play+0x5CF0; 025C58D8 dTimer_c::deleteRequest */
static inline u32 dComIfG_getTimerPtr() { return gabi::load<u32>(dComIfGp_ea() + 0x5CF0); }
static inline void dTimer_deleteRequest(u32 t) { gabi::call(0x025C58D8, t); }
/* fopAcM_orderOtherEventId(this, idx) with the defaults (0xFF, 0xFFFF, 0, 1) */
static inline void orderOtherEvent(fopAc_ac_c* a, s16 idx) { fopAcM_orderOtherEventId(a, idx, 0xFF, 0xFFFF, 0, 1); }
/* dCcD_GObjInf::OnTgShield / OffTgShield: mGObjTg.mSPrm bit 0; ChkAtShieldHit: mGObjAt.mRPrm bit 0 */
static inline void OnTgShield(dCcD_GObjInf* o) { o->mGObjTg.mSPrm |= 1u; }
static inline void OffTgShield(dCcD_GObjInf* o) { o->mGObjTg.mSPrm &= ~1u; }
static inline bool ChkAtShieldHit(dCcD_GObjInf* o) { return (o->mGObjAt.mRPrm & 1) != 0; }
/* `this->*p == &fn` with the virtual index already loaded (GHS keeps it across a call) */
static inline bool ptmf_eq_i(s16 i, ProcFunc_l& p, u32 fn) { return i == -1 && p.d == 0 && p.f == fn; }

enum {
    CUT_TYPE_NONE = 0, CUT_TYPE_CUT_A = 1, CUT_TYPE_CUT_F = 2, CUT_TYPE_CUT_R = 3, CUT_TYPE_CUT_L = 4,
    CUT_TYPE_BT_JUMPCUT = 5, CUT_TYPE_CUT_EA = 6, CUT_TYPE_CUT_EB = 7, CUT_TYPE_CUT_TURN = 8,
    CUT_TYPE_CUT_ROLL = 9, CUT_TYPE_JUMPCUT_SWORD = 0xA, CUT_TYPE_BT_ROLLCUT = 0xF,
    CUT_TYPE_BT_VERTICALJUMPCUT = 0x10, CUT_TYPE_CUT_EXA = 0x1A, CUT_TYPE_CUT_EXB = 0x1B,
    CUT_TYPE_CUT_EXMJ = 0x1E, CUT_TYPE_CUT_KESA = 0x1F,
};
enum {
    JA_SE_CV_JI_DEFENCE_ = 0x4833,
    JA_SE_CV_JI_FUTTOBI = 0x4932,
    JA_SE_LK_SW_CRT_HIT = 0x2806,
    JA_SE_CM_JI_SLIP = 0x5021,
};

static inline f32 endFrame(daNpc_Ji1_c* t) { return t->mpOrcaMorf->getEndFrame(); }

/* 0224F368 */
BOOL daNpc_Ji1_c::teachSpRollCutMove(f32 param_1) {
    WWHD_FUNC(0x0224F368, BOOL, this, param_1);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> temp;
    cXyz_mi(&home.pos, temp, &current.pos);
    gabi::Local<cXyz> temp2;
    cXyz_mi(&player->current.pos, temp2, &current.pos);
    std_sqrtf(PSVECSquareMag(temp));
    std_sqrtf(PSVECSquareMag(temp2));
    gabi::Local<cXyz> temp3;
    cXyz_mi(&player->current.pos, temp3, &home.pos);
    std_sqrtf(PSVECSquareMag(temp3));

    cM_atan2s(temp->x, temp->z);
    s16 temp4 = cM_atan2s(temp2->x, temp2->z);
    BOOL temp5 = FALSE;
    s16 d = (s16)(temp4 - current.angle.y);
    if ((d < 0 ? -d : d) > 0x1000) {
        temp5 = TRUE;
    }
    cLib_addCalcAngleS2(&current.angle.y, temp4, 8, 0x1000);
    if (temp5 == TRUE) {
        if (!isGuardAnim() || mpOrcaMorf->checkFrame(endFrame(this) - 2.0f)) {
            setAnm(6, 4.0f, 0);
        }
        if (mAnimation == 6) {
            mpOrcaMorf->setPlaySpeed(l_HIO().field_0x48 * param_1);
        }
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0224F368, &daNpc_Ji1_c::teachSpRollCutMove);

/* 02255100 */
BOOL daNpc_Ji1_c::checkCutType(int cutType, int param_2) {
    WWHD_FUNC(0x02255100, BOOL, this, cutType, param_2);
    fopAc_ac_c* player = daPy_getPlayerActorClass();
    BOOL ret = false;
    switch (param_2) {
    case 0:
        switch (cutType) {
        case CUT_TYPE_CUT_R:
        case CUT_TYPE_CUT_L:
        case CUT_TYPE_CUT_EB:
        case CUT_TYPE_CUT_EXA:
        case CUT_TYPE_CUT_EXB:
        case CUT_TYPE_CUT_EXMJ:
        case CUT_TYPE_CUT_KESA:
            ret = true;
            break;
        case CUT_TYPE_CUT_TURN:
            if (player_checkComboCutTurn(player)) ret = true;
            break;
        default:
            break;
        }
        break;
    case 1:
        switch (cutType) {
        case CUT_TYPE_CUT_A:
        case CUT_TYPE_CUT_EA:
        case CUT_TYPE_CUT_EXA:
        case CUT_TYPE_CUT_EXB:
        case CUT_TYPE_CUT_EXMJ:
        case CUT_TYPE_CUT_KESA:
            ret = true;
            break;
        case CUT_TYPE_CUT_TURN:
            if (player_checkComboCutTurn(player)) ret = true;
            break;
        default:
            break;
        }
        break;
    case 2:
        switch (cutType) {
        case CUT_TYPE_CUT_F:
        case CUT_TYPE_CUT_EA:
        case CUT_TYPE_CUT_EXA:
        case CUT_TYPE_CUT_EXB:
        case CUT_TYPE_CUT_EXMJ:
        case CUT_TYPE_CUT_KESA:
            ret = true;
            break;
        case 8:
            if (player_checkComboCutTurn(player)) ret = true;
            break;
        default:
            break;
        }
        break;
    case 3:
        switch (cutType) {
        case CUT_TYPE_CUT_TURN:
        case CUT_TYPE_CUT_ROLL:
            ret = true;
            break;
        default:
            break;
        }
        break;
    case 4:
        switch (cutType) {
        case CUT_TYPE_BT_JUMPCUT:
        case CUT_TYPE_BT_ROLLCUT:
        case CUT_TYPE_BT_VERTICALJUMPCUT:
            ret = true;
            break;
        default:
            break;
        }
        break;
    default:
        switch (cutType) {
        case CUT_TYPE_JUMPCUT_SWORD:
            ret = true;
            break;
        }
        break;
    }
    return ret;
}
VERIFY(0x02255100, &daNpc_Ji1_c::checkCutType);

/* 02256F1C HD: the magic meter is refilled every frame (setItemMagicCount(max)), a spin attack
 * only counts when the player is within 20 units of height and costs 10 Knight's Crests */
BOOL daNpc_Ji1_c::teachSPRollCutAction(void* arg) {
    WWHD_FUNC(0x02256F1C, BOOL, this, arg);
    fopAc_ac_c* player = daPy_getPlayerActorClass();

    if (field_0xC78 == 0) {
        dComIfGp_setItemMagicCount(dComIfGs_getMaxMagic());
        setAnm(5, 0.0f, 0);
        attn_flags(this) |= 1; /* fopAc_Attn_LOCKON_MISC_e */
        field_0xC30 = 0;
        field_0xD34 = 0;
        field_0xC90 = 0;
        dComIfGs_onTmpBit(0x0402 /* UNK_0402 */);
        field_0xD70 = 6;
        field_0xC78 += 1;
    } else if (field_0xC78 != -1) {
        dComIfGp_setItemMagicCount(dComIfGs_getMaxMagic());
        if (daNpc_Ji1_plRoomOutCheck()) {
            field_0xC84 = 9;
            ji1_setAction(this, ACT_eventAction, nullptr);
            ptmf_set(field_0x2C8, ACT_teachSPRollCutAction);
            return true;
        }

        gabi::Local<cXyz> temp;
        cXyz_mi(&player->current.pos, temp, &current.pos);
        cM_atan2s(temp->x, temp->z);
        gabi::Local<cXyz> xz;
        xz->x = (f32)temp->x;
        xz->y = 0.0f;
        xz->z = (f32)temp->z;
        std_sqrtf(PSVECSquareMag(xz));

        s32 cutType = player_getCutType(player);
        f32 y_diff = std::fabs((f32)temp->y);
        if (cutType == CUT_TYPE_CUT_ROLL && y_diff < 20.0f) {
            dComIfGs_onEventBit(0x0B20 /* UNK_0B20 */);
            dComIfGs_offTmpBit(0x0402);
            dComIfGp_setItemBeastNumCount_crest(-10);
            OffTgShield(&field_0x7E0);
            field_0xC84 = 10;
            ji1_setAction(this, ACT_eventAction, nullptr);
            ptmf_set(field_0x2C8, ACT_normalAction);
            return TRUE;
        }
        if (cutType == CUT_TYPE_CUT_TURN) {
            field_0xC84 = 0xB;
            ji1_setAction(this, ACT_eventAction, nullptr);
            ptmf_set(field_0x2C8, ACT_teachSPRollCutAction);
            return TRUE;
        }

        OnTgShield(&field_0x7E0);
        if (field_0x7E0.ChkTgHit() && field_0xD6C != 1) {
            switch ((u32)field_0xC24) {
            case CUT_TYPE_CUT_A:
            case CUT_TYPE_CUT_EA:
            case CUT_TYPE_CUT_TURN:
                setAnm(8, 0.0f, 1);
                break;
            case CUT_TYPE_CUT_ROLL:
                break;
            case CUT_TYPE_JUMPCUT_SWORD:
                setAnm(10, 0.0f, 1);
                field_0xD38.copy(current.pos);
                field_0xC9C = 0.0f;
                break;
            default:
                setAnm(9, 0.0f, 1);
                break;
            }
            if (field_0xC24 == CUT_TYPE_CUT_ROLL) {
                setHitParticle(nullptr, JA_SE_CV_JI_DEFENCE_);
                field_0xD38.copy(current.pos);
                field_0xC9C = 0.0f;
                setParticle(0x10, 1.0f, 0.1f);
            } else {
                setGuardParticle();
            }
            field_0xD6C = 1;
            field_0xC8C = field_0xC24;
        }

        if (field_0xD6C == 1) {
            if (!player_getCutAtFlg(player) || mpOrcaMorf->getFrame() > endFrame(this) - 2.0f) {
                if (field_0xC90 == 0) {
                    if (field_0xC8C == CUT_TYPE_CUT_ROLL) {
                        field_0xC84 = 0xA;
                        ji1_setAction(this, ACT_eventAction, nullptr);
                        ptmf_set(field_0x2C8, ACT_normalAction);
                        return true;
                    }
                    orderOtherEvent(this, mEventIdx[3]);
                    eventInfo_onCondition(this, 2); /* dEvtCnd_UNK2_e */
                    field_0xC90 = 2;
                }
            }

            if (mAnimation == 10 && field_0xC8C == CUT_TYPE_CUT_ROLL) {
                f32 h34 = l_HIO().field_0x34;
                cLib_addCalc2(&field_0xC9C, h34 + h34, 0.25f, l_HIO().field_0x38);
                gabi::Local<cXyz> temp3;
                f32 x = field_0xD38.x;
                f32 c9c = field_0xC9C;
                f32 y = field_0xD38.y;
                f32 s = cM_ssin(current.angle.y);
                f32 c = cM_scos(current.angle.y);
                f32 z = field_0xD38.z;
                temp3->x = gabi::fnmsubs(c9c, s, x);
                temp3->y = y;
                temp3->z = gabi::fnmsubs(c9c, c, z);
                f32 temp2 = 45.0f;
                if (mAcch.ChkWallHit()) {
                    temp2 = 10.0f;
                }
                cLib_addCalcPos2(&current.pos, temp3, 0.8f, temp2);
                ji1_seStart(this, JA_SE_CM_JI_SLIP, 0);
            }

            if (mpOrcaMorf->getFrame() > endFrame(this) - 2.0f) {
                OffTgShield(&field_0x6B0);
                OnTgShield(&field_0x7E0);
                field_0xC9C = 0.0f;
                field_0xD6C = 0;
                if (field_0xC90 == 2) {
                    field_0xD38.copy(current.pos);
                    field_0xC4C = 0.0f;
                    field_0xC50 = 0.0f;
                    field_0xC2C = 0;
                    ji1_setAction(this, ACT_speakBadAction, nullptr);
                } else {
                    field_0xD38.copy(current.pos);
                    field_0xC90 = 0;
                    ji1_setAction(this, ACT_speakAction, nullptr);
                }
            }
        } else {
            if (!teachSpRollCutMove(1.0f)) {
                if (mpOrcaMorf->checkFrame(endFrame(this) - 2.0f)) {
                    setAnm(5, 4.0f, 0);
                }
            }
        }

        if (field_0xD6C != 1) {
            dtParticle();
        }
        if (field_0xD6C != 1 && isGuardAnim()) {
            if (mpOrcaMorf->getFrame() > endFrame(this) - 2.0f) {
                setAnm(5, 4.0f, 0);
            }
        }
    }
    return true;
}
VERIFY(0x02256F1C, &daNpc_Ji1_c::teachSPRollCutAction);

/* 0225A534 */
BOOL daNpc_Ji1_c::battleGuardCheck() {
    WWHD_FUNC(0x0225A534, BOOL, this);
    if (field_0x7E0.ChkTgHit() && field_0xD6C != 1) {
        int anm = 9;
        switch ((u32)field_0xC24) {
        case CUT_TYPE_BT_ROLLCUT:
        case CUT_TYPE_BT_VERTICALJUMPCUT:
            battleSubActionJumpInit();
            break;
        case CUT_TYPE_BT_JUMPCUT:
            current.angle.y += (s16)-0x8000;
            battleSubActionDamageInit();
            break;
        case CUT_TYPE_JUMPCUT_SWORD:
            battleSubActionJpGuardInit();
            break;
        case CUT_TYPE_CUT_A:
        case CUT_TYPE_CUT_EA:
        case CUT_TYPE_CUT_TURN:
        case CUT_TYPE_CUT_ROLL:
        case CUT_TYPE_CUT_EXA:
            anm = 8;
            /* fallthrough */
        default: {
            BOOL atk = isAttackAnim();
            s16 si = mSubAction.i;
            if (!atk) {
                if (!ptmf_eq_i(si, mSubAction, SUB_battleSubActionNockBack)) {
                    setAnm(anm, 0.0f, 1);
                    battleSubActionGuardInit();
                    break;
                }
            }
            int attackFrame = isAttackFrame();
            if (ptmf_eq_i(si, mSubAction, SUB_battleSubActionNockBack)) {
                setAnm(anm, 0.0f, 1);
                battleSubActionGuardInit();
            } else {
                if (attackFrame == 0) {
                    battleSubActionNockBackInit(1);
                    field_0xD6C = 1;
                    return 0;
                }
                if (attackFrame == 1) {
                    setAnm(anm, 0.0f, 1);
                    battleSubActionGuardInit();
                    break;
                }
                return 0;
            }
            break;
        }
        }

        s32 c24 = field_0xC24;
        if (c24 == CUT_TYPE_NONE) {
            setGuardParticle();
        } else {
            if (c24 == CUT_TYPE_BT_ROLLCUT || c24 == CUT_TYPE_BT_VERTICALJUMPCUT || c24 == CUT_TYPE_BT_JUMPCUT) {
                ji1_seStart(this, JA_SE_LK_SW_CRT_HIT, 0);
                /* static cXyz scale(1.25f, 1.25f, 1.25f): guard 0x104673C4, object 0x10467270 */
                cXyz* scale = gabi::at<cXyz>(0x10467270);
                if (gabi::load<u32>(0x104673C4) == 0) {
                    gabi::store<u32>(0x104673C4, 1);
                    scale->x = 1.25f;
                    scale->z = 1.25f;
                    scale->y = 1.25f;
                }
                if (field_0xC24 == CUT_TYPE_BT_JUMPCUT) {
                    setHitParticle(scale, JA_SE_CV_JI_FUTTOBI);
                } else {
                    setHitParticle(scale, JA_SE_CV_JI_DEFENCE_);
                }
                s32 lim = l_HIO().field_0x60[3] - 1;
                if (field_0xD70 < lim) {
                    field_0xD70 += 2;
                    dComIfGp_plusMiniGameRupee(2);
                } else {
                    field_0xD70 += 1;
                    dComIfGp_plusMiniGameRupee(1);
                }
            } else {
                setHitParticle(nullptr, JA_SE_CV_JI_DEFENCE_);
                field_0xD70 += 1;
                dComIfGp_plusMiniGameRupee(1);
            }

            if (field_0xD70 >= l_HIO().field_0x60[3]) {
                field_0xC84 = 8;
                s32 c38;
                if (dComIfGs_isEventBit(0x0F20)) {
                    c38 = 2;
                } else {
                    c38 = dComIfGs_isEventBit(0x0F10) == 0;
                }
                field_0xC38 = c38;
                ji1_setAction(this, ACT_eventAction, nullptr);
                dComIfGp_setMessageCountNumber((s16)field_0xD70);
                ptmf_set(field_0x2C8, ACT_normalAction);
                if (dComIfG_getTimerPtr()) {
                    dTimer_deleteRequest(dComIfG_getTimerPtr());
                    setClearRecord((s16)field_0xD70);
                }
                return TRUE;
            }
        }
        field_0xD6C = 1;
        mBtNowFrame = 30.0f; /* setBtNowFrame */
    }

    if (!ChkAtShieldHit(&field_0xA40) && field_0xA40.ChkAtHit()) {
        field_0xA40.ClrAtHit();
    }

    if (field_0xC3C > 2) {
        if (ptmf_eq(mAction, ACT_battleAction)) {
            if (dComIfG_getTimerPtr() != 0) {
                dTimer_deleteRequest(dComIfG_getTimerPtr());
            }
            if (isClearRecord((s16)field_0xD70)) {
                setClearRecord((s16)field_0xD70);
                field_0xC84 = 7;
                ji1_setAction(this, ACT_eventAction, nullptr);
                dComIfGp_setMessageCountNumber((s16)field_0xD70);
                ptmf_set(field_0x2C8, ACT_normalAction);
            } else {
                ji1_setAction(this, ACT_endspeakAction, nullptr);
            }
        }
    }
    return TRUE;
}
VERIFY(0x0225A534, &daNpc_Ji1_c::battleGuardCheck);

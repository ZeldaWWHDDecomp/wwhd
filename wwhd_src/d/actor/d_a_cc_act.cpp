/** ChuChu collision, Deku Tree demo and stone actions, using tww names and the HD layout.
 * Verified against the original WWHD instructions. */
#include "d/actor/d_a_cc_act.h"
#include "gc_api/tu.h"
const gc_tu_data gc_tu = {.safeString = 0x1000C548};
// HD: each inlined demo has its own checked cut-name table.
static char* come_cut_name_tbl[2] = {"START_RET", "NEXT_RET"};
static char* ret_cut_name_tbl[2] = {"START_RET", "NEXT_RET"};
static const char come_staff[7] = {0x43, 0x79, 0x75, 0x43, 0x79, 0x75, 0};
static const char ret_staff[7] = {0x43, 0x79, 0x75, 0x43, 0x79, 0x75, 0};

/* HD fopAcM_seStart / fopAcM_monsSeStart inlines where GHS already knows the actor is non-null
 * (only the &eyePos != NULL test is left) */
static inline void seStart_nn(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0)
        mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* full HD inline: if (a != NULL && &a->eyePos != NULL) */
static inline void monsSeStart_full(fopAc_ac_c* a, u32 se, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s32 room = fopAcM_GetRoomNo(a);
        u32 id = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        CcAction_monsSeStart(se, &a->eyePos, id, param, reverb); /* mDoAud_monsSeStart */
    }
}
static inline void monsSeStart_nn(fopAc_ac_c* a, u32 se, u32 param) {
    if (gabi::ea(&a->eyePos) != 0) {
        s32 room = fopAcM_GetRoomNo(a);
        u32 id = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        CcAction_monsSeStart(se, &a->eyePos, id, param, reverb); /* mDoAud_monsSeStart */
    }
}

static inline bool cc_isCriticalCut(u8 t) {
    switch (t) {
    case CUT_TYPE_CUT_EA:
    case CUT_TYPE_CUT_EB:
    case CUT_TYPE_CUT_TURN:
    case CUT_TYPE_CUT_ROLL:
    case CUT_TYPE_JUMPCUT_SWORD:
    case CUT_TYPE_JUMPCUT_STICK:
    case CUT_TYPE_JUMPCUT_MACHETE:
    case CUT_TYPE_BT_JUMPCUT:
    case CUT_TYPE_BT_ROLLCUT:
    case CUT_TYPE_BT_VERTICALJUMPCUT:
    case CUT_TYPE_JUMPCUT_CLUB:
    case CUT_TYPE_JUMPCUT_DN_SWORD:
    case CUT_TYPE_JUMPCUT_SPEAR:
    case CUT_TYPE_CUT_EXA:
    case CUT_TYPE_CUT_EXB:
    case CUT_TYPE_CUT_EXMJ:
    case CUT_TYPE_CUT_KESA:
        return true;
    }
    return false;
}

/* 0210CE9C */
static BOOL body_atari_check(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    i_this->mStts.Move();

    if (i_this->mCyl.ChkTgHit()) {
        auto* pcVar6 = static_cast<dCcD_GObjInf*>(i_this->mCyl.GetTgHitObj());
        fopAc_ac_c* pdVar7 = i_this->mCyl.GetTgHitAc();
        if (pcVar6 == nullptr) {
            return FALSE;
        }

        gabi::Local<cXyz> sp30;
        {
            cXyz* hp = i_this->mCyl.GetTgHitPosP();
            f32 x = hp->x, z = hp->z;
            sp30->x = x;
            sp30->z = z;
            f32 y = hp->y;
            sp30->y = y;
        }

        if ((i_this->m304 != 0) && (pdVar7 != nullptr) && (dComIfGp_getPlayer(0) != pdVar7)) {
            i_this->m304 = 2;
            a_this->health = 0;
            dComIfGp_particle_set(ID_AK_JN_CRITICALHITFLASH, sp30);
            gabi::Local<cXyz> sp24;
            sp24->set(2.0f, 2.0f, 2.0f);
            dComIfGp_particle_set(ID_AK_JN_CRITICALHIT, sp30, &player->shape_angle, sp24);
            i_this->mCurrAction = 3;
            i_this->m2F5 = 0x28;
            return TRUE;
        }

        i_this->actor.mBtNowFrame = 1000.0f; /* setBtNowFrame */

        gabi::Local<CcActionAtInfo> sp3C;
        sp3C->pParticlePos = nullptr;
        bool bVar4 = false;
        i_this->m2F6 = 0;

        u32 atType = pcVar6->mAtType;
        if (atType & AT_TYPE_LIGHT) {
            if (i_this->mColorType == 3) {
                if (i_this->mCurrAction != 6) {
                    i_this->mStts.SetWeight(0xFE);
                    i_this->mCurrAction = 6;
                    i_this->m2F5 = 0x50;
                    return TRUE;
                }
            } else {
                return FALSE;
            }
        }

        switch (atType) {
        case AT_TYPE_GRAPPLING_HOOK:
            if (a_this->stealItemLeft > 0) {
                s8 sVar9 = a_this->health;
                u8 bVar1 = a_this->stealItemBitNo;
                a_this->health = 10;
                sp3C->mpObj = static_cast<dCcD_GObjInf*>(i_this->mCyl.GetTgHitObj());
                cc_at_check(a_this, sp3C.get());
                if (i_this->mColorType == 2) {
                    a_this->stealItemBitNo = bVar1;
                }
                a_this->health = sVar9;
            }

            if (i_this->m2F5 == 0x15) {
                seStart_nn(a_this, JA_SE_LK_MS_WEP_HIT, 0x36);
                a_this->gravity = -(REG_F(12, 19) + 3.0f);
                i_this->m324 = 0.0f;
                i_this->m2F8 = 0;
                return FALSE;
            }
            bVar4 = true;
            i_this->m2F6 = 9;
            break;

        case AT_TYPE_MACHETE:
        case AT_TYPE_SWORD:
        case AT_TYPE_UNK800:
        case AT_TYPE_DARKNUT_SWORD:
        case AT_TYPE_MOBLIN_SPEAR:
            if (i_this->m2F7 != 0) {
                return FALSE;
            }

            if (cc_isCriticalCut(reinterpret_cast<CcActionPlayer*>(player)->getCutType())) {
                i_this->m2F6 = 1;
            }

            if (i_this->mColorType == 3) {
                fopAcM_seStart(a_this, JA_SE_LK_SW_HIT_S, 0x36);
                bVar4 = true;
                i_this->m2F6 = 0;
            }
            break;

        case AT_TYPE_WIND:
            bVar4 = true;
            i_this->m2F6 = 3;
            a_this->current.angle.y =
                cM_atan2s(a_this->current.pos.x - sp30->x, a_this->current.pos.z - sp30->z);
            if ((pdVar7 != nullptr) && (pdVar7 == player)) {
                a_this->current.angle.y = player->shape_angle.y;
                if (i_this->m2F5 == 0x15) {
                    a_this->speedF = REG_F(12, 18) + 20.0f;
                    return FALSE;
                }
            }
            break;

        case AT_TYPE_BOOMERANG:
            i_this->m2F6 = 4;
            bVar4 = true;
            if (i_this->m2F5 == 0x15) {
                fopAcM_seStart(a_this, JA_SE_LK_W_WEP_HIT, 0x36);
                a_this->gravity = -(REG_F(12, 19) + 3.0f);
                i_this->m324 = 0.0f;
                i_this->m2F8 = 0;
                return FALSE;
            }
            break;

        case AT_TYPE_BOKO_STICK:
        case AT_TYPE_UNK2000:
        case AT_TYPE_STALFOS_MACE:
            if (i_this->mColorType == 3) {
                fopAcM_seStart(a_this, JA_SE_LK_W_WEP_HIT, 0x36);
                bVar4 = true;
                i_this->m2F6 = 0;
            }
            break;

        case AT_TYPE_HOOKSHOT:
            if (i_this->mColorType == 3) {
                fopAcM_seStart(a_this, JA_SE_LK_MS_WEP_HIT, 0x36);
            }
            i_this->m2F6 = 7;
            bVar4 = true;
            if (i_this->m2F5 == 0x15) {
                a_this->speed.z = 0.0f;
                a_this->speed.x = 0.0f;
                a_this->speed.y = 0.0f;
                a_this->speedF = 0.0f;
                a_this->gravity = -(REG_F(12, 19) + 3.0f);
                i_this->m324 = 0.0f;
                i_this->m2F8 = 0;
                return FALSE;
            }
            break;

        case AT_TYPE_SKULL_HAMMER:
            if (i_this->mColorType == 3) {
                fopAcM_seStart(a_this, JA_SE_LK_HAMMER_HIT, 0x36);
            }

            i_this->m2F6 = 1;

            {
                u8 cut = reinterpret_cast<CcActionPlayer*>(player)->getCutType();
                if (cut == CUT_TYPE_HAMMER_FRONTSWING || cut == CUT_TYPE_JUMPCUT_HAMMER) {
                    i_this->m2F6 = 6;
                }
            }

            if (i_this->mColorType == 3) {
                bVar4 = true;
                i_this->m2F6 = 0;
            }
            break;

        case AT_TYPE_FIRE:
        case AT_TYPE_FIRE_ARROW:
            if (i_this->mColorType == 3) {
                bVar4 = true;
                i_this->m2F6 = 0;
                fopAcM_seStart(a_this, JA_SE_LK_MS_WEP_HIT, 0x36);
            } else {
                i_this->mEnemyFire.mFireDuration = 100;
                i_this->m2F6 = 8;
            }
            break;

        case AT_TYPE_LIGHT_ARROW:
            i_this->mEnemyIce.mLightShrinkTimer = 1;
            i_this->mEnemyIce.mParticleScale = 0.5f;
            i_this->mEnemyIce.mYOffset = 80.0f;
            fopAcM_seStart(a_this, JA_SE_CM_CC_DIE_SWING, 0);
            monsSeStart_full(a_this, JA_SE_CV_CC_DIE, 0);
            a_this->attention_info.flags = 0;
            denki_end(i_this);
            i_this->m2F6 = 8;
            break;

        case AT_TYPE_ICE_ARROW:
            if (i_this->mColorType == 3) {
                bVar4 = true;
                i_this->m2F6 = 0;
                fopAcM_seStart(a_this, JA_SE_LK_MS_WEP_HIT, 0x36);
            } else {
                i_this->mEnemyIce.mFreezeDuration = REG_S(8, 2) + 200;
                if (REG_S(8, 2) != 0) {
                    /* HD: after the store GHS treats the actor as non-null */
                    a_this->health = 100;
                    seStart_nn(a_this, JA_SE_CM_CC_DIE_SWING, 0);
                    monsSeStart_nn(a_this, JA_SE_CV_CC_DIE, 0);
                } else {
                    fopAcM_seStart(a_this, JA_SE_CM_CC_DIE_SWING, 0);
                    monsSeStart_full(a_this, JA_SE_CV_CC_DIE, 0);
                }
                enemy_fire_remove(&i_this->mEnemyFire);
                denki_end(i_this);
                bVar4 = true;
                i_this->m2F6 = 8;
            }
            break;

        default:
            if (i_this->mColorType == 3) {
                bVar4 = true;
                i_this->m2F6 = 0;
                fopAcM_seStart(a_this, JA_SE_LK_MS_WEP_HIT, 0x36);
            }
            break;
        }

        if (!bVar4) {
            sp3C->mpObj = static_cast<dCcD_GObjInf*>(i_this->mCyl.GetTgHitObj());
            cc_at_check(a_this, sp3C.get());

            if ((i_this->m2F6 == 1) || (i_this->m2F6 == 6) || (a_this->health <= 0)) {
                dComIfGp_particle_set(ID_AK_JN_CRITICALHITFLASH, sp30);
                gabi::Local<cXyz> sp18;
                sp18->set(2.0f, 2.0f, 2.0f);
                dComIfGp_particle_set(ID_AK_JN_CRITICALHIT, sp30, &player->shape_angle, sp18);
            } else {
                dComIfGp_particle_set(ID_AK_JN_OK, sp30, &player->shape_angle);
            }
        }

        if ((i_this->m2F6 == 7) && (i_this->mCurrAction != 3)) {
            i_this->mCyl.OffAtSPrmBit(1); /* OffAtSPrmBit(cCcD_AtSPrm_Set_e), ClrAtSet */
            i_this->m32C = 0.0f;
            i_this->m330 = 0.0f;
            i_this->m348 = 0;
            i_this->m310.x = 0;
            i_this->m310.y = 0;
            i_this->m310.z = 0;
            i_this->mCurrAction = 0;
            i_this->m2F5 = 10;
            return TRUE;
        }

        if ((i_this->mColorType == 3) && (i_this->m2F6 != 4 && (i_this->m2F6 != 3)) &&
            (i_this->m2F6 != 9)) {
            i_this->m2F6 = 0;
        }

        if (i_this->m2F6 == 6) {
            i_this->mCurrAction = 4;
            i_this->m2F5 = 0x34;
        } else {
            i_this->mCurrAction = 3;
            i_this->m2F5 = 0x28;
        }
        return TRUE;
    }
    return FALSE;
}

/* inlined into action_noboru */
static inline void deku_come_demo(cc_class* i_this) {
    s32 iVar2;
    switch (i_this->m2FC) {
    case 1:
        i_this->m2FC = i_this->m2FC + 1;
        i_this->m308 = dComIfGp_evmng_getEventIdx("CyuCyu_STR", 0xFF);
        break;

    case 2:
        if (eventInfo_checkCommandDemoAccrpt(&i_this->actor)) {
            i_this->m2FC = i_this->m2FC + 1;
            i_this->m34E[5] = 0x28;
            if (demoFlags().DEMO_SHORT_CUT_FLAG != 0) {
                i_this->m34E[5] = 0x14;
            }
            demoFlags().DEMO_COME_START_FLAG = 2;
        } else {
            fopAcM_orderOtherEventId(&i_this->actor, i_this->m308, 0xFF, 0xFFFF, 0, 1);
        }
        break;

    case 3:
        if (i_this->m34E[5] == 0) {
            iVar2 =
                dComIfGp_evmng_getMyStaffId(reinterpret_cast<const char*>(come_staff), nullptr, 0);
            dComIfGp_evmng_cutEnd(iVar2);
            i_this->m2FC = i_this->m2FC + 1;
        }
        break;

    case 4:
        iVar2 = dComIfGp_evmng_getMyStaffId(reinterpret_cast<const char*>(come_staff), nullptr, 0);
        if ((s8)dComIfGp_evmng_getMyActIdx(iVar2, come_cut_name_tbl, 2, TRUE, 0) == 1) {
            dComIfGp_evmng_cutEnd(iVar2);
            i_this->m2FC = i_this->m2FC + 1;
        }
        break;

    case 5:
        if (dComIfGp_evmng_endCheck(i_this->m308)) {
            dComIfGp_event_reset();
            i_this->m2FC = 0;
            demoFlags().DEMO_COME_START_FLAG = 0;
            dComIfGs_onTmpBit(CC_TMP_DEKU_RETURN);
        }
        break;
    }
}

/* inlined into action_noboru */
static inline void deku_ret_demo(cc_class* i_this) {
    s32 iVar2;
    switch (i_this->m2FD) {
    case 2:
        if (eventInfo_checkCommandDemoAccrpt(&i_this->actor)) {
            i_this->m2FD = i_this->m2FD + 1;
            i_this->m34E[5] = 0x28;
            if (demoFlags().DEMO_SHORT_CUT_FLAG != 0) {
                i_this->m34E[5] = 0x14;
            }
        } else {
            fopAcM_orderOtherEventId(&i_this->actor, i_this->m308, 0xFF, 0xFFFF, 0, 1);
        }
        break;

    case 3:
        if (i_this->m34E[5] == 0) {
            anm_init(i_this, dRes_INDEX_CC_BCK_START_e, 0.0f, 0, 1.0f, -1);
            demoFlags().DEMO_RET_START_FLAG = 2;
            i_this->m34E[5] = 0x50;
            if (demoFlags().DEMO_SHORT_CUT_FLAG != 0) {
                i_this->m34E[5] = 0x28;
            }
            i_this->m2FD = i_this->m2FD + 1;
        }
        break;

    case 4:
        if (i_this->m34E[5] == 0) {
            iVar2 =
                dComIfGp_evmng_getMyStaffId(reinterpret_cast<const char*>(ret_staff), nullptr, 0);
            dComIfGp_evmng_cutEnd(iVar2);
            i_this->m2FD = i_this->m2FD + 1;
        }
        break;

    case 5:
        iVar2 = dComIfGp_evmng_getMyStaffId(reinterpret_cast<const char*>(ret_staff), nullptr, 0);
        if ((s8)dComIfGp_evmng_getMyActIdx(iVar2, ret_cut_name_tbl, 2, TRUE, 0) == 1) {
            dComIfGp_evmng_cutEnd(iVar2);
            i_this->m2FD = i_this->m2FD + 1;
        }
        break;

    case 6:
        if (dComIfGp_evmng_endCheck(i_this->m308)) {
            dComIfGp_event_reset();
            i_this->m2FD = 0;
            demoFlags().DEMO_RET_START_FLAG = 0;
            demoFlags().DEMO_SHORT_CUT_FLAG = 1;
            i_this->m2F5 = 0x3F;
        }
        break;
    }
}

/* 021116B8 */
static void action_noboru(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    switch (i_this->m2F5) {
    case 0x3C:
        i_this->m3BA = -0x4000;
        i_this->m2FB = 0;
        a_this->attention_info.flags = 0;
        anm_init(i_this, dRes_INDEX_CC_BCK_DEKU_START_e, 0.0f, 0, 0.0f, -1);
        i_this->m2F5 = i_this->m2F5 + 1;
        [[fallthrough]];

    case 0x3D:
        if (i_this->m2FB != 0) {
            fopAcM_seStart(a_this, JA_SE_CM_CC_ENTER_GND, 0);
            anm_init(i_this, dRes_INDEX_CC_BCK_DEKU_START_e, 0.0f, 0, 1.0f, -1);
            i_this->m2F5 = i_this->m2F5 + 1;
        }
        break;

    case 0x3E:
        if (!i_this->m2B4->isStop()) {
            break;
        }
        i_this->m344 = cM_rndF(43.0f);
        anm_init(i_this, dRes_INDEX_CC_BCK_TACHI_WALK_e, 1.0f, 2, 1.0f, -1);
        i_this->m2F5 = i_this->m2F5 + 1;
        [[fallthrough]];

    case 0x3F:
        if (i_this->m2B4->checkFrame(i_this->m344)) {
            monsSeStart_full(a_this, JA_SE_CV_CC_PARASITE, 0);
        }

        if (!reinterpret_cast<CcActionPlayer*>(player)->checkFrontRollCrash()) {
            break;
        }

        anm_init(i_this, dRes_INDEX_CC_BCK_ATACK01_e, 0.0f, 0, 1.0f, -1);
        i_this->m2F5 = i_this->m2F5 + 1;
        [[fallthrough]];

    case 0x40:
        if (demoFlags().DEMO_COME_START_FLAG == 0) {
            i_this->m2FC = 1;
            demoFlags().DEMO_COME_START_FLAG = 1;
        } else if (demoFlags().DEMO_COME_START_FLAG == 2) {
            i_this->m2F5 = 0x41;
        }
        break;

    case 0x41:
        monsSeStart_full(a_this, JA_SE_CV_CC_ATTACK, 0);
        anm_init(i_this, dRes_INDEX_CC_BCK_ATACK02_e, 0.0f, 0, 1.0f, -1);
        i_this->m3BA = 0;
        a_this->current.angle.x = 0;
        a_this->current.angle.y = 0;
        a_this->current.angle.z = 0;
        a_this->current.angle.y = (s16)gabi::ftoi(cM_rndFX(4096.0f) + 12288.0f);
        a_this->shape_angle.x = a_this->current.angle.x;
        a_this->shape_angle.y = a_this->current.angle.y;
        a_this->shape_angle.z = a_this->current.angle.z;
        i_this->m2FB = 3;

        anm_init(i_this, dRes_INDEX_CC_BCK_ATACK02_e, 0.0f, 0, 1.0f, -1);
        /* HD: after the stores GHS treats the actor as non-null */
        seStart_nn(a_this, JA_SE_CM_CC_ATTACK, 0);

        a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
        i_this->mCyl.SetTgType(~(AT_TYPE_UNK400000 | AT_TYPE_UNK20000 | AT_TYPE_WATER));
        i_this->mCyl.OnTgSPrmBit(1);  /* OnTgSetBit */
        i_this->mCyl.OnCoSPrmBit(1);  /* OnCoSetBit */
        i_this->mCyl.OffAtSPrmBit(1); /* OffAtSPrmBit(cCcD_AtSPrm_Set_e), ClrAtSet */
        a_this->speedF = 15.0f;
        a_this->speed.y = 50.0f;
        a_this->gravity = -3.0f;
        i_this->m2F9 = 0;
        i_this->m2F5 = i_this->m2F5 + 1;
        break;

    case 0x42:
        if (i_this->mAcch.ChkGroundHit()) {
            anm_init(i_this, dRes_INDEX_CC_BCK_ATACK03_e, 0.0f, 0, 1.0f, -1);
            fopAcM_seStart(
                a_this, JA_SE_CM_CC_LANDING,
                dComIfG_Bgsp()->GetMtrlSndId(static_cast<cBgS_PolyInfo*>(&i_this->mAcch.m_gnd)));

            cc_eff_set(i_this, 1);
            a_this->speed.y = 0.0f;
            denki_start(i_this);
            i_this->m2F5 = i_this->m2F5 + 1;
        }
        break;

    case 0x43:
        if (demoFlags().DEMO_COME_START_FLAG == 0) {
            i_this->m34E[5] = 600;
            i_this->mCurrAction = 0;
            i_this->m2F5 = 3;
        }
        break;

    case 0x44:
        i_this->mCyl.OffTgSPrmBit(1); /* OffTgSetBit */
        i_this->mCyl.OffCoSPrmBit(1); /* OffCoSetBit */
        i_this->mCyl.OffAtSPrmBit(1); /* OffAtSPrmBit(cCcD_AtSPrm_Set_e), ClrAtSet */
        i_this->mCyl.ClrTgHit();
        i_this->m310.x = 0;
        i_this->m310.y = 0;
        i_this->m310.z = 0;
        i_this->m2F9 = 1;
        dPa_rippleEcallBack_end(
            reinterpret_cast<dPa_rippleEcallBack*>(i_this->m390)); /* HD: m390 (DEMO_SELECT) */
        a_this->attention_info.flags = 0;

        if (i_this->m320 != dRes_INDEX_CC_BCK_TACHI_WALK_e) {
            anm_init(i_this, dRes_INDEX_CC_BCK_TACHI2HUSE_e, 0.0f, 0, 0.0f, -1);
        }

        seStart_nn(a_this, JA_SE_CM_CC_STAND_TO_LIE, 0);

        if (demoFlags().DEMO_RET_START_FLAG == 0) {
            demoFlags().DEMO_RET_START_FLAG = 1;
            i_this->m2FD = 1;
        }

        a_this->speed.x = 0.0f;
        a_this->speed.y = 0.0f;
        a_this->speed.z = 0.0f;
        a_this->speedF = 0.0f;
        i_this->m2F5 = i_this->m2F5 + 1;
        [[fallthrough]];

    case 0x45:
        cLib_addCalc0(&i_this->m334, 0.1f, 0.2f);

        if (i_this->m334 > 0.1f) {
            break;
        }

        a_this->shape_angle.x = 0;
        a_this->shape_angle.y = 0;
        a_this->shape_angle.z = 0;
        a_this->current.angle.x = 0;
        a_this->current.angle.y = 0;
        a_this->current.angle.z = 0;
        a_this->gravity = 0.0f;
        a_this->speed.x = 0.0f;
        a_this->speed.y = 0.0f;
        a_this->speed.z = 0.0f;
        i_this->m34E[0] = 0;
        if (i_this->m2FD != 0) {
            i_this->m34E[0] = 0x3C;
        }
        anm_init(i_this, dRes_INDEX_CC_BCK_START_e, 0.0f, 0, 0.0f, -1);
        i_this->m2FB = 4;
        i_this->m3BA = -0x4000;
        i_this->m2F5 = i_this->m2F5 + 1;
        [[fallthrough]];

    case 0x46:
        if (demoFlags().DEMO_RET_START_FLAG == 2) {
            fopAcM_seStart(a_this, JA_SE_CM_CC_LIE_TO_STAND, 0);
            i_this->m334 = 1.0f;
            anm_init(i_this, dRes_INDEX_CC_BCK_START_e, 0.0f, 0, 1.0f, -1);
            i_this->m2F5 = 0x3E;
        }

        if (i_this->m34E[0] == 30 && i_this->m2FD == 1) {
            i_this->m2FD = 2;
            i_this->m308 = dComIfGp_evmng_getEventIdx("CyuCyu_RET", 0xFF);
            i_this->m334 = 1.0f;
            i_this->m2F5 = 0x47;
        }
        break;

    case 0x47:
        deku_ret_demo(i_this);
        if (demoFlags().DEMO_RET_START_FLAG >= 2 && i_this->m320 == dRes_INDEX_CC_BCK_START_e &&
            i_this->m2B4->isStop()) {
            anm_init(i_this, dRes_INDEX_CC_BCK_TACHI_WALK_e, 1.0f, 2, 1.0f, -1);
        }
    }

    if (i_this->m2FC != 0) {
        deku_come_demo(i_this);
    }
}

/* 02112414 */
static void action_up_check(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s16 bVar4 = 0;

    switch (i_this->m2F5) {
    case 0x50:
        i_this->m2F6 = 5;
        i_this->mCyl.mGObjTg.mSPrm |= 1; /* OnTgShield */
        i_this->mCyl.OnTgSPrmBit(1);     /* OnTgSetBit */
        i_this->mCyl.SetTgType(~(AT_TYPE_UNK400000 | AT_TYPE_UNK20000 | AT_TYPE_WATER));
        anm_init(i_this, dRes_INDEX_CC_BCK_MAHI_e, 0.0f, 0, 0.0f, -1);
        i_this->m2F9 = 0;
        fopAcM_seStart(a_this, JA_SE_CM_CC_STONED, 0);

        a_this->speed.x = 0.0f;
        a_this->speed.y = 0.0f;
        a_this->speed.z = 0.0f;

        i_this->mCyl.OnCoSPrmBit(1);  /* OnCoSetBit */
        i_this->mCyl.OffAtSPrmBit(1); /* OffAtSPrmBit(cCcD_AtSPrm_Set_e), ClrAtSet */

        for (s32 i = 0; i < 5; i++) {
            i_this->m35C[i] = 0;
        }

        a_this->speedF = 0.0f;
        /* HD: GHS drops the actor null test of the remaining inlines */
        seStart_nn(a_this, JA_SE_CM_CC_DMG_SWING, 0);
        monsSeStart_nn(a_this, JA_SE_CV_CC_DAMAGE, 0);
        i_this->m2F5 = i_this->m2F5 + 1;
        [[fallthrough]];

    case 0x51:
        Cc_view<CcAnimation>(gabi::ea(i_this->m2C8))->mFrame = 0.0f;
        Cc_view<CcAnimation>(gabi::ea(i_this->m2D0))->mFrame = 0.0f;
        i_this->m2FE = 1;
        i_this->m2FF = 0;
        i_this->m301 = 0;
        i_this->m34E[4] = 600;
        a_this->actor_status |= fopAcStts_FREEZE_e;
        i_this->m2F5 = i_this->m2F5 + 1;
        break;

    case 0x52:
        if (i_this->m2FE != 2) {
            a_this->attention_info.flags |= fopAc_Attn_ACTION_CARRY_e;
        }

        cLib_addCalc0(&a_this->speedF, 0.5f, 1.0f);

        if (((a_this->actor_status & fopAcStts_CARRY_e) != 0)) {
            i_this->m338 = a_this->current.pos.y;
            a_this->speedF = 0.0f;
            i_this->mCyl.OffCoSPrmBit(1);
            a_this->current.angle.y = player->shape_angle.y;
            a_this->gravity = 0.0f;
            a_this->speed.x = 0.0f;
            a_this->speed.y = 0.0f;
            a_this->speed.z = 0.0f;
            i_this->mCyl.OffCoSPrmBit(1);
            a_this->attention_info.flags = 0;
            i_this->m2F5 = i_this->m2F5 + 1;
        }
        break;

    case 0x53:
        a_this->current.angle.y = player->shape_angle.y;
        if (!((a_this->actor_status & fopAcStts_CARRY_e) != 0)) {
            a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
            i_this->mCyl.OnCoSPrmBit(1);
            a_this->gravity = -3.0f;
            if (a_this->speedF > 0.0f) {
                a_this->speed.y = 12.0f;
                a_this->speedF = 20.0f;
                i_this->m2F5 = 0x54;
            } else {
                seStart_nn(a_this, JA_SE_OBJ_PUT_STONE, 0);
                a_this->speedF = 0.0f;
                i_this->m2F5 = 0x52;
            }
        }
        break;

    case 0x54:
        if (i_this->mAcch.ChkWallHit()) {
            a_this->speedF = a_this->speedF * -0.5f;
        }

        if (i_this->mAcch.ChkGroundHit()) {
            a_this->speedF = 0.0f;
            seStart_nn(a_this, JA_SE_OBJ_PUT_STONE, 0);
            bVar4 = 1;
        }
        cLib_addCalc0(&a_this->speedF, 0.5f, 1.0f);
        break;
    }

    if (!(i_this->m338 + 10.0f > a_this->current.pos.y)) {
        cLib_addCalcAngleS2(&a_this->shape_angle.y, a_this->current.angle.y, 1, 0x1000);
    }

    if (i_this->m2F5 >= 0x52) {
        if (CcAction_detectLight(&reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mDetect,
                                 &a_this->current.pos) != FALSE) {
            i_this->m34E[4] = 600;
        }

        if (i_this->m34E[4] == 1) {
            i_this->m34E[2] = 0x28;
            seStart_nn(a_this, JA_SE_CM_CC_STONED_RECOVER, 0);
        }

        if (i_this->m34E[2] != 0) {
            i_this->m35C[0] += 20000;
            a_this->shape_angle.z = (s16)gabi::ftoi(cM_ssin(i_this->m35C[0]) * 2000.0f);
            if (i_this->m34E[2] == 1) {
                i_this->m2FE = 2;
                i_this->m2FF = 0;
                a_this->shape_angle.z = 0;
                Cc_view<CcAnimation>(gabi::ea(i_this->m2CC))->mFrame = 0.0f;
                Cc_view<CcAnimation>(gabi::ea(i_this->m2D4))->mFrame = 0.0f;
                a_this->attention_info.flags &= ~(u32)fopAc_Attn_ACTION_CARRY_e;
            }
        }

        if (i_this->m2FE == 2 && i_this->m2FF >= 0x19) {
            if (((a_this->actor_status & fopAcStts_CARRY_e) != 0)) {
                fopAcM_cancelCarryNow(a_this);
                a_this->attention_info.flags &= ~(u32)fopAc_Attn_ACTION_CARRY_e;
            }

            a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
            i_this->mCyl.mGObjTg.mSPrm &= ~1u; /* OffTgShield */
            i_this->m2FF = 0;
            i_this->m2FE = 0;
            a_this->shape_angle.z = 0;
            a_this->gravity = -3.0f;
            i_this->mCyl.OnTgSPrmBit(1);
            a_this->current.angle.y = a_this->shape_angle.y;
            a_this->actor_status &= ~(u32)fopAcStts_FREEZE_e;
            i_this->mStts.SetWeight(0x32);
            i_this->mCurrAction = 0;
            i_this->m2F5 = 6;
        }
    }

    if (i_this->mCurrAction != 0 && i_this->m2FE != 2) {
        i_this->mStts.Move();
        if (i_this->mCyl.ChkTgHit()) {
            player = dComIfGp_getPlayer(0);
            auto* pcVar6 = static_cast<dCcD_GObjInf*>(i_this->mCyl.GetTgHitObj());
            if (pcVar6 == nullptr) {
                return;
            }

            gabi::Local<cXyz> sp24;
            {
                /* lfs/stfs pairs (the recompiled code quiets a signalling NaN here) */
                cXyz* hp = i_this->mCyl.GetTgHitPosP();
                f32 z = hp->z, y = hp->y, x = hp->x;
                sp24->z = z;
                sp24->y = y;
                sp24->x = x;
            }
            dComIfGp_particle_set(ID_AK_JN_NG, sp24, &player->shape_angle);

            switch (pcVar6->mAtType) {
            case AT_TYPE_SWORD:
            case AT_TYPE_MACHETE:
            case AT_TYPE_UNK800:
            case AT_TYPE_DARKNUT_SWORD:
            case AT_TYPE_MOBLIN_SPEAR:
                seStart_nn(a_this, JA_SE_LK_SW_HIT_S, 13);
                break;

            case AT_TYPE_BOOMERANG:
            case AT_TYPE_BOKO_STICK:
            case AT_TYPE_UNK2000:
            case AT_TYPE_STALFOS_MACE:
                seStart_nn(a_this, JA_SE_LK_W_WEP_HIT, 13);
                break;

            case AT_TYPE_SKULL_HAMMER:
                bVar4 = 1;
                break;

            case AT_TYPE_NORMAL_ARROW:
            case AT_TYPE_SPIKE:
            case AT_TYPE_HOOKSHOT:
            case AT_TYPE_FIRE_ARROW:
            case AT_TYPE_ICE_ARROW:
            case AT_TYPE_LIGHT_ARROW:
            case AT_TYPE_GRAPPLING_HOOK:
                seStart_nn(a_this, JA_SE_LK_MS_WEP_HIT, 13);
                break;
            }
        }
    }

    if (bVar4) {
        JPABaseEmitter* pJVar7 = dComIfGp_particle_set(
            ID_AK_SN_CCBREAKHAHEN00, &a_this->attention_info.position, &a_this->current.angle);
        if (pJVar7 != nullptr) {
            auto* emitter = reinterpret_cast<CcEmitter*>(pJVar7);
            u8 prmr = a_this->tevStr.mColorK0.r;
            u8 prmg = a_this->tevStr.mColorK0.g;
            u8 prmb = a_this->tevStr.mColorK0.b;
            emitter->mPrmColor.r = prmr;
            emitter->mPrmColor.g = prmg;
            emitter->mPrmColor.b = prmb;
            u8 envr = a_this->tevStr.mColorK0.r;
            u8 envg = a_this->tevStr.mColorK0.g;
            u8 envb = a_this->tevStr.mColorK0.b;
            emitter->mEnvColor.r = envr;
            emitter->mEnvColor.g = envg;
            emitter->mEnvColor.b = envb;
        }

        gabi::Local<cXyz> sp18;
        sp18->copy(a_this->current.pos); /* lfs/stfs pairs folded by clang: bit-exact */
        sp18->y = a_this->current.pos.y + 60.0f;

        seStart_nn(a_this, JA_SE_OBJ_BREAK_STONE, 0);

        fopAcM_createDisappear(a_this, sp18, 5, daDisItem_IBALL_e, a_this->stealItemBitNo);
        fopAcM_delete(a_this);
        dComIfGs_onActor(a_this->setID, a_this->home.roomNo);
    }
}

/* WWHD glue
ext 0210C5C8 void anm_init(cc_class*, int, f32, u8, f32, int)
ext 0210CC00 void denki_end(cc_class*)
ext 0210CBC0 void denki_start(cc_class*)
ext 0210C364 void cc_eff_set(cc_class*, u8)
ext 025192A8 fopAc_ac_c* cc_at_check(fopAc_ac_c*, CcActionAtInfo*)
ext 02041C30 void enemy_fire_remove(enemyfire_l*)
ext 025E1AA4 void CcAction_monsSeStart(u32, cXyz*, u32, u32, s32)
ext 0252A038 BOOL CcAction_detectLight(dDetect_c*, cXyz*)
data come_cut_name_tbl 101B3AEC
data ret_cut_name_tbl 101B3AF4
str 1000C5D4 "CyuCyu_STR"
str 1000C5EC "CyuCyu_RET"
data come_staff 1000C538
data ret_staff 1000C540
*/

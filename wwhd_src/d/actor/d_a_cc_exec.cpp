/** ChuChu execution and inlined actions, following tww with verified HD behaviour. */
#include "d/actor/d_a_cc_exec.h"
#include "gc_api/tu.h"
const gc_tu_data gc_tu = {.safeString = 0x1000C548};
static const char splitArchive[3] = {0x43, 0x63, 0};
static const char deadArchive[3] = {0x43, 0x63, 0};

/* HD fopAcM_monsSeStart inline (the process id is read before the reverb call) */
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 se, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s32 room = fopAcM_GetRoomNo(a);
        u32 id = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        CcExec_monsSeStart(se, &a->eyePos, id, param, reverb); /* mDoAud_monsSeStart */
    }
}
/* mCyl (dCcD_Cyl) bits */
static inline void cyl_OnAtSet(cc_class* i) {
    i->mCyl.OnAtSPrmBit(1);
}
static inline void cyl_OffAtSet(cc_class* i) {
    i->mCyl.OffAtSPrmBit(1);
}
static inline void cyl_OnTgSet(cc_class* i) {
    i->mCyl.OnTgSPrmBit(1);
}
static inline void cyl_OffTgSet(cc_class* i) {
    i->mCyl.OffTgSPrmBit(1);
}
static inline void cyl_OnCoSet(cc_class* i) {
    i->mCyl.OnCoSPrmBit(1);
}
static inline void cyl_OffCoSet(cc_class* i) {
    i->mCyl.OffCoSPrmBit(1);
}

// HD: reading a joint matrix marks its matrix block dirty.
static Mtx34* cc_getAnmMtx(J3DModel* model, s32 jntNo) {
    u32 block = reinterpret_cast<CcModel*>(model)->mMatrixBlock;
    auto* matrices = Cc_view<CcMatrixBlock>(block);
    matrices->mFlags = matrices->mFlags | 0x10;
    return Cc_view<Mtx34>(matrices->mMatrices + jntNo * sizeof(Mtx34));
}

static inline void zero_m35C(cc_class* i_this) {
    for (s32 i = 0; i < 5; i++)
        i_this->m35C[i] = 0;
}

/* inlined into daCC_Execute */
static inline void action_nomal_move(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    switch (i_this->m2F5) {
    case 0:
        cyl_OffTgSet(i_this);
        cyl_OffCoSet(i_this);
        i_this->mCyl.ClrTgHit();
        anm_init(i_this, dRes_INDEX_CC_BCK_START_e, 0.0f, 0, 0.0f, -1);
        if (i_this->mBehaviorType == 0) {
            anm_init(i_this, dRes_INDEX_CC_BCK_TSTART01_e, 0.0f, 0, 0.0f, -1);
        }
        i_this->m2F5 += 1;
        [[fallthrough]];
    case 1:
        if (fopAcM_searchPlayerDistance(a_this) < i_this->mNoticeRange) {
            a_this->actor_status |= fopAcStts_SHOWMAP_e;
            if (i_this->mBehaviorType == 1) {
                anm_init(i_this, dRes_INDEX_CC_BCK_TSTART01_e, 0.0f, 0, 1.0f, -1);
                i_this->m2F5 = 2;
            } else {
                fopAcM_seStart(a_this, JA_SE_CM_CC_ENTER_GND, 0);
                anm_init(i_this, dRes_INDEX_CC_BCK_START_e, 0.0f, 0, 1.0f, -1);
                i_this->m2F9 = 0;
                a_this->gravity = -3.0f;
                i_this->m2F5 = 3;
                cyl_OnTgSet(i_this);
                a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
            }
        }
        break;

    case 2:
        if (i_this->m2B4->checkFrame(22.0f)) {
            i_this->m2F9 = 0;
            a_this->gravity = -11.0f;
        }
        if (i_this->m2B4->getFrame() > 22.0f) {
            cLib_addCalcAngleS2(&a_this->shape_angle.x, 0, 1, 0x1000);
        }
        if (i_this->mAcch.ChkGroundHit()) {
            u32 snd =
                dComIfG_Bgsp()->GetMtrlSndId(static_cast<cBgS_PolyInfo*>(&i_this->mAcch.m_gnd));
            fopAcM_seStart(a_this, JA_SE_CM_CC_LANDING, snd);
            cc_eff_set(i_this, 1);
            a_this->shape_angle.x = 0;
            a_this->speed.y = 0.0f;
            anm_init(i_this, dRes_INDEX_CC_BCK_TSTART02_e, 0.0f, 0, 1.0f, -1);
            i_this->m2F5 = 3;
            cyl_OnTgSet(i_this);
            a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
            denki_start(i_this);
        }
        break;

    case 3:
        if (!i_this->m2B4->isStop()) {
            break;
        }
        i_this->m2F5 += 1;
        [[fallthrough]];
    case 4: {
        a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
        zero_m35C(i_this);
        i_this->mCyl.SetTgType(CC_TG_TYPE_ALL);
        cyl_OnCoSet(i_this);
        cyl_OnTgSet(i_this);
        i_this->m34E[0] = 10;
        s16 r = (s16)gabi::ftoi(cM_rndF(15.0f));
        /* HD: the stored 10 is not reloaded after the call */
        i_this->m34E[0] = (s16)(r + 10 + i_this->m34E[1]);
        denki_start(i_this);
        if (i_this->m320 != dRes_INDEX_CC_BCK_TACHI_WALK_e) {
            anm_init(i_this, dRes_INDEX_CC_BCK_TACHI_WALK_e, 2.0f, 2, 1.0f, -1);
        }
        fopAcM_seStart(a_this, JA_SE_CM_CC_MOVE_STAND, 0);
        i_this->m2F5 += 1;
        break;
    }

    case 5:
        search_angle_set(i_this);
        if (i_this->m2B4->checkFrame(0.0f)) {
            fopAcM_seStart(a_this, JA_SE_CM_CC_MOVE_STAND, 0);
        }
        cLib_addCalc2(&a_this->speedF, 2.0f, 0.2f, 0.5f);
        if (i_this->m34E[0] != 0) {
            break;
        }
        if (i_this->mBehaviorType == 2 && i_this->m34E[5] == 0) {
            i_this->m2F5 = 6;
            break;
        }
        if (i_this->mColorType == 1 || i_this->mColorType == 4) {
            if (fopAcM_searchPlayerDistance(a_this) < 380.0f) {
                i_this->mCurrAction = 2;
                i_this->m2F5 = 0x1e;
                return;
            }
            break;
        }
        i_this->m2F5 += 1;
        [[fallthrough]];
    case 6:
        denki_start(i_this);
        fopAcM_seStart(a_this, JA_SE_CM_CC_STAND_TO_LIE, 0);
        i_this->mCyl.SetTgType(CC_TG_TYPE_CROUCH);
        cyl_OffCoSet(i_this);
        anm_init(i_this, dRes_INDEX_CC_BCK_TACHI2HUSE_e, 2.0f, 0, 1.0f, -1);
        i_this->m2F5 += 1;
        break;

    case 7:
        if (!i_this->m2B4->isStop()) {
            break;
        }
        if (i_this->mBehaviorType == 2 && i_this->m34E[5] == 0) {
            i_this->mCurrAction = 5;
            i_this->m2F5 = 0x44;
            break;
        }
        anm_init(i_this, dRes_INDEX_CC_BCK_HUSE_WALK_e, 0.0f, 2, 1.0f, -1);
        i_this->m2F5 += 1;
        [[fallthrough]];
    case 8: {
        i_this->m34E[0] = 0x46;
        s16 r = (s16)gabi::ftoi(cM_rndF(15.0f));
        i_this->m34E[0] = (s16)(r + 0x46);
        fopAcM_seStart(a_this, JA_SE_CM_CC_MOVE_LIE, 0);
        a_this->speedF = 1.0f;
        i_this->m2F5 += 1;
        break;
    }

    case 9:
        if (i_this->m2B4->checkFrame(0.0f)) {
            fopAcM_seStart(a_this, JA_SE_CM_CC_MOVE_LIE, 0);
        }
        search_angle_set(i_this);
        if (i_this->m34E[0] != 0) {
            break;
        }
        i_this->m2F5 += 1;
        [[fallthrough]];
    case 10:
        i_this->mCyl.SetTgType(CC_TG_TYPE_ALL);
        anm_init(i_this, dRes_INDEX_CC_BCK_HUSE2TACHI_e, 0.0f, 0, 1.0f, -1);
        fopAcM_seStart(a_this, JA_SE_CM_CC_LIE_TO_STAND, 0);
        i_this->m2F5 += 1;
        break;

    case 0xb:
        if (i_this->m2B4->isStop()) {
            fopAcM_monsSeStart(a_this, JA_SE_CV_CC_STAND_UP, 0);
            if (i_this->mBehaviorType != 3 && fopAcM_searchPlayerDistance(a_this) < 380.0f &&
                i_this->mAcch.ChkGroundHit()) {
                i_this->mCurrAction = 2;
                i_this->m2F5 = 0x1e;
                return;
            }
            i_this->m2F5 = 4;
        }
        break;
    }

    cLib_addCalcAngleS2(&a_this->shape_angle.y, a_this->current.angle.y, 1, 0x400);

    if ((i_this->m2F5 < 4 || i_this->m2F5 > 0xb || !shock_damage_check(i_this)) &&
        i_this->m2F5 >= 3) {
        body_atari_check(i_this);
        black_light_check(i_this);
    }
}

/* inlined into daCC_Execute */
static inline void action_oyogu(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;

    switch (i_this->m2F5) {
    case 0x14:
        zero_m35C(i_this);
        denki_start(i_this);
        i_this->m301 = 0;
        fopAcM_seStart(a_this, JA_SE_CM_CC_STAND_TO_LIE, 0);
        i_this->m310.x = 0;
        i_this->m310.y = 0;
        i_this->m310.z = 0;
        i_this->mCyl.SetTgType(CC_TG_TYPE_SWIM);
        cyl_OffCoSet(i_this);
        if (i_this->mColorType != 2 && i_this->mColorType != 4) {
            cyl_OffAtSet(i_this);
        }
        fopAcM_seStart(a_this, JA_SE_CM_CC_STAND_TO_LIE, 0);
        if (i_this->m320 != dRes_INDEX_CC_BCK_TACHI2HUSE_e) {
            anm_init(i_this, dRes_INDEX_CC_BCK_TACHI2HUSE_e, 2.0f, 0, 1.0f, -1);
        }
        a_this->speedF = 0.0f;
        a_this->speed.x = 0.0f;
        a_this->speed.y = 0.0f;
        a_this->speed.z = 0.0f;
        a_this->gravity = -3.0f;
        i_this->m324 = 0.0f;
        i_this->m2F8 = 0;
        i_this->m2F5 += 1;
        [[fallthrough]];
    case 0x15: {
        if (i_this->m320 == dRes_INDEX_CC_BCK_TACHI2HUSE_e) {
            if (i_this->m2B4->isStop()) {
                anm_init(i_this, dRes_INDEX_CC_BCK_HUSE_WALK_e, 0.0f, 2, 1.0f, -1);
            }
        }
        cLib_addCalc0(&a_this->gravity, 0.2f, 1.0f);
        cLib_addCalc0(&a_this->speed.y, 0.2f, 1.0f);
        i_this->m35C[0] += 0x800;
        f32 s = gabi::fmadds(cM_ssin(i_this->m35C[0]), 3.0f, -15.0f);
        cLib_addCalc2(&i_this->m328, s, 1.0f, 10.0f);

        if (i_this->mAcch.m_flags & dBgS_Acch::WATER_IN) {
            /* m_wtr.GetHeight(): mAcch + 0x1BC */
            cLib_addCalc2(&a_this->current.pos.y, i_this->mAcch.m_wtr.m_height - 20.0f, 1.0f,
                          i_this->m324);
            cLib_addCalc2(&i_this->m324, 6.0f, 1.0f, 1.0f);
            f32 h = i_this->mAcch.m_wtr.m_height - 20.0f;
            if (std::fabs((f32)(a_this->current.pos.y - h)) < 2.0f) {
                i_this->m2F8 = 1;
                cLib_addCalc2(&a_this->speedF, REG_F(8, 17) + 1.0f, 0.3f, 1.0f);
                if (REG_F(8, 17) != 0.0f || a_this->speedF < 2.0f) {
                    search_angle_set(i_this);
                }
            }
        }

        if (i_this->m2B4->checkFrame(0.0f)) {
            fopAcM_seStart(a_this, JA_SE_CM_CC_MOVE_LIE, 0);
        }
        break;
    }
    }

    body_atari_check(i_this);

    if (!(i_this->mAcch.m_flags & dBgS_Acch::WATER_IN)) {
        a_this->gravity = -3.0f;
        if (i_this->mAcch.ChkGroundHit() || (i_this->mBehaviorType == 2 && i_this->m34E[5] == 0)) {
            i_this->m2F8 = 0;
            i_this->m328 = 0.0f;
            if (i_this->mBehaviorType != 2) {
                i_this->mCurrAction = 0;
                i_this->m2F5 = 10;
            } else {
                i_this->mCurrAction = 5;
                i_this->m2F5 = 0x44;
            }
        }
    }
}

/* inlined into daCC_Execute */
static inline void action_attack_move(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    fopEn_enemy_c* e_this = &i_this->actor;

    switch (i_this->m2F5) {
    case 0x1E:
        zero_m35C(i_this);
        a_this->speedF = 0.0f;
        denki_start(i_this);
        a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
        cyl_OnCoSet(i_this);
        fopAcM_monsSeStart(a_this, JA_SE_CV_CC_ATTACK, 0);
        i_this->m306 = 0;
        anm_init(i_this, dRes_INDEX_CC_BCK_ATACK01_e, 0.0f, 0, 1.0f, -1);
        i_this->m2F5 += 1;
        [[fallthrough]];
    case 0x1F:
        if (!i_this->m2B4->isStop())
            break;
        i_this->m34E[0] = 10;
        i_this->m2F5 += 1;
        [[fallthrough]];
    case 0x20:
        if (i_this->m34E[0] == 0) {
            a_this->current.angle.y = fopAcM_searchPlayerAngleY(a_this);
            cyl_OnAtSet(i_this);
            i_this->mCyl.mObjAt.mRPrm = 1; /* OnAtHitBit (HD: a plain store) */
            anm_init(i_this, dRes_INDEX_CC_BCK_ATACK02_e, 0.0f, 0, 1.0f, -1);
            fopAcM_seStart(a_this, JA_SE_CM_CC_ATTACK, 0);
            a_this->speedF = 18.0f;
            a_this->gravity = -3.0f;
            a_this->speed.y = 25.0f;
            i_this->m2F5 += 1;
        }
        break;

    case 0x21:
        i_this->mAcch.OnLineCheck();
        /* ChkAtShieldHit: mGObjAt.mRPrm bit 0 */
        if (a_this->speedF > 0.0f && (i_this->mCyl.mGObjAt.mRPrm & 1)) {
            a_this->speedF *= -0.5f;
            a_this->speed.y = 0.0f;
        }
        if (i_this->mAcch.ChkGroundHit()) {
            if (i_this->mColorType != 2 && i_this->mColorType != 4) {
                cyl_OffAtSet(i_this);
            }
            u32 snd =
                dComIfG_Bgsp()->GetMtrlSndId(static_cast<cBgS_PolyInfo*>(&i_this->mAcch.m_gnd));
            fopAcM_seStart(a_this, JA_SE_CM_CC_LANDING, snd);
            cc_eff_set(i_this, 1);
            a_this->speedF = 0.0f;
            anm_init(i_this, dRes_INDEX_CC_BCK_ATACK03_e, 0.0f, 0, 1.0f, -1);
            i_this->m2F5 += 1;
        }
        break;

    case 0x22:
        if (i_this->m2B4->isStop()) {
            i_this->m34E[1] = 5;
            /* HD: m34E[1] += cM_rndF(m34E[1]) folded to 5 + cM_rndF(5) in float */
            f32 r = cM_rndF(5.0f);
            i_this->m34E[1] = (s16)gabi::ftoi(5.0f + r);
            i_this->mCurrAction = 0;
            i_this->m2F5 = 4;
        }
        break;
    }

    e_this->mBtNowFrame = 1000.0f;

    if (i_this->m2F5 >= 0x1F && i_this->m2F5 <= 0x22) {
        i_this->m306 += 1;
        if (i_this->m306 > (s16)gabi::ftoi(REG_F(8, 7)) &&
            i_this->m306 < (s16)gabi::ftoi(REG_F(8, 8) + 25.0f)) {
            e_this->mBtNowFrame = 5.0f;
        }
    }

    if (i_this->m2F5 > 0x20 || !shock_damage_check(i_this)) {
        cLib_addCalcAngleS2(&a_this->shape_angle.y, a_this->current.angle.y, 1, 0x1000);
        black_light_check(i_this);
        body_atari_check(i_this);
    }
}

/* inlined into daCC_Execute */
static inline void damage_swing(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    cMtx_YrotS(calc_mtx(), i_this->m34A);
    gabi::Local<cXyz> sp28;
    gabi::Local<cXyz> sp1C;
    sp28->x = 0.0f;
    sp28->y = 0.0f;
    // HD: retain the sine-first operand order and unextended angle passed by GHS.
    sp28->z = cM_ssin((u16)i_this->m348) * i_this->m32C;
    MtxPosition(sp28, sp1C);
    i_this->m310.z = (s16)gabi::ftoi(-sp1C->x);
    i_this->m310.x = (s16)gabi::ftoi(sp1C->z);
    cLib_addCalc0(&a_this->speedF, 0.8f, 4.0f);
    if (i_this->m2F6 != 3) {
        i_this->m348 = i_this->m348 + 0x3000;
    }
    cLib_addCalc0(&i_this->m32C, 1.0f, i_this->m330);
}

/* inlined into daCC_Execute */
static inline void action_damage_move(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;

    switch (i_this->m2F5) {
    case 0x28: {
        zero_m35C(i_this);
        cyl_OffAtSet(i_this);
        cyl_OnTgSet(i_this);
        cyl_OnCoSet(i_this);
        a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;

        if (i_this->mColorType == 3 && i_this->m2F6 == 0) {
            cyl_OffTgSet(i_this);
            i_this->mCyl.ClrTgHit();
            i_this->m32C = 0.0f;
            i_this->m330 = 0.0f;
            i_this->m348 = 0;
            i_this->m310.x = 0;
            i_this->m310.y = 0;
            i_this->m310.z = 0;
            a_this->speedF = 0.0f;
            i_this->m2F5 = 0x2d;
            break;
        }

        denki_end(i_this);
        anm_init(i_this, dRes_INDEX_CC_BCK_TACHI2HUSE_e, 0.0f, 0, 0.0f, -1);
        i_this->mCyl.SetTgType(CC_TG_TYPE_ALL);

        s16 ang = fopAcM_searchPlayerAngleY(a_this);
        i_this->m34A = (s16)((ang + 0x8000) - a_this->shape_angle.y);
        i_this->m348 = 0;
        i_this->m32C = 7000.0f;

        if (i_this->m2F6 == 1 || a_this->health <= 0) {
            i_this->m32C = 16000.0f;
            i_this->m2F6 = 1;
        }

        if (a_this->health > 0) {
            fopAcM_seStart(a_this, JA_SE_CM_CC_DMG_SWING, 0);
            fopAcM_monsSeStart(a_this, JA_SE_CV_CC_DAMAGE, 0);
        } else {
            fopAcM_seStart(a_this, JA_SE_CM_CC_DIE_SWING, 0);
            fopAcM_monsSeStart(a_this, JA_SE_CV_CC_DIE, 0);
        }

        cc_eff_set(i_this, 0);

        a_this->speed.y = 0.0f;
        i_this->m330 = 250.0f;

        switch (i_this->m2F6) {
        case 1:
            i_this->m330 = 500.0f;
            a_this->speedF = 60.0f;
            break;
        case 4:
        case 9:
            anm_init(i_this, dRes_INDEX_CC_BCK_MAHI_e, 0.0f, 0, 1.0f, -1);
            dComIfGp_particle_set(ID_IT_JN_PIYOHIT00, &a_this->attention_info.position);
            i_this->m32C = 0.0f;
            i_this->m330 = 0.0f;
            i_this->m348 = 0;
            break;
        case 3:
            if (((a_this->actor_status & fopAcStts_CARRY_e) != 0)) {
                fopAcM_cancelCarryNow(a_this);
                a_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
                cyl_OnCoSet(i_this);
                a_this->gravity = -3.0f;
                a_this->speedF = 0.0f;
            }
            i_this->m32C = 16000.0f;
            i_this->m348 = 10000;
            break;
        default:
            a_this->speedF = 20.0f;
            break;
        }

        if (i_this->mBehaviorType == 3) {
            a_this->speedF = 0.0f;
        }

        if (i_this->m2F6 != 3) {
            s16 a = fopAcM_searchPlayerAngleY(a_this);
            a_this->current.angle.y = (s16)(a + 0x8000);
        }

        i_this->m2F5 += 1;
    }
        [[fallthrough]];
    case 0x29:
        damage_swing(i_this);
        if (std::fabs((f32)i_this->m32C) < 1.0f) {
            a_this->speedF = 0.0f;
            i_this->m310.x = 0;
            i_this->m310.y = 0;
            i_this->m310.z = 0;

            if (i_this->m2F6 == 4 || i_this->m2F6 == 2) {
                i_this->m34E[4] = 0x5A; /* 0x2d + 0x2d */
                i_this->m34E[4] = (s16)(REG_S(8, 5) + 0x5A);
                enemy_piyo_set(a_this);
                fopAcM_seStart(a_this, JA_SE_CM_MD_PIYO, 0);
                i_this->m2F5 = 0x2b;
            } else if (i_this->m2F6 == 9) {
                i_this->m34E[4] = (s16)(REG_S(8, 4) + 0x14);
                i_this->m2F5 = 0x2c;
            } else {
                i_this->m2F5 += 1;
            }
        }
        break;

    case 0x2a:
        if (a_this->health > 0) {
            if (i_this->m34E[4] == 0) {
                i_this->mCurrAction = 0;
                i_this->m2F5 = 6;
                if (i_this->mColorType == 1 || i_this->mColorType == 4) {
                    i_this->m2F5 = 4;
                }
            }
        } else {
            i_this->mCurrAction = 4;
            i_this->m2F5 = 0x32;
        }
        break;

    case 0x2b:
        if (i_this->m34E[4] <= 1) {
            anm_init(i_this, dRes_INDEX_CC_BCK_HUKKATSU_e, 0.0f, 2, 1.0f, -1);
            i_this->m34E[4] = 0x3c;
            i_this->m34E[4] = (s16)(REG_S(8, 6) + 0x3c);
            i_this->m2F5 += 1;
        }
        break;

    case 0x2c:
        if (i_this->m34E[4] == 0) {
            i_this->mCurrAction = 0;
            i_this->m2F5 = 6;
            if (i_this->mColorType == 1 || i_this->mColorType == 4) {
                i_this->m2F5 = 4;
            }
        }
        break;

    case 0x2d: {
        i_this->m301 = 1;
        J3DAnmTransform* anm =
            (J3DAnmTransform*)dComIfG_getObjectRes(reinterpret_cast<const char*>(splitArchive),
                                                   dRes_INDEX_CC_BCK_CC_PTCL_e, gc_tu.safeString);
        i_this->m2D8->setAnm(anm, 0, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);

        JPABaseEmitter* emitter = dComIfGp_particle_set(
            ID_AK_SN_CCSPLIT00, &a_this->attention_info.position, &a_this->current.angle);
        if (emitter != nullptr) {
            /* HD: emitter->setGlobalPrmColor / setGlobalEnvColor inlines; the prm colour scales a
             * colour of the particle manager (dPa_control_c + 0xF4) by the tev colour K0 */
            auto* e = reinterpret_cast<CcExecEmitter*>(emitter);
            auto* pa = reinterpret_cast<CcParticle*>(dComIfGp_getParticle());
            u8 r = 0, g = 0, b = 0;
            if (pa != nullptr) {
                f32 k = (f32)a_this->tevStr.mColorK0.r / 255.0f;
                r = (u8)gabi::ftoi((f32)pa->mToonColor.r * k);
                k = (f32)a_this->tevStr.mColorK0.g / 255.0f;
                g = (u8)gabi::ftoi((f32)pa->mToonColor.g * k);
                k = (f32)a_this->tevStr.mColorK0.b / 255.0f;
                b = (u8)gabi::ftoi((f32)pa->mToonColor.b * k);
            }
            e->mPrmColor.r = r;
            e->mPrmColor.g = g;
            e->mPrmColor.b = b;
            e->mEnvColor.r = a_this->tevStr.mColorK0.r;
            e->mEnvColor.g = a_this->tevStr.mColorK0.g;
            e->mEnvColor.b = a_this->tevStr.mColorK0.b;
        }
        i_this->mStts.SetWeight(0xFE);
        i_this->m2F5 += 1;
        break;
    }

    case 0x2e:
        if (i_this->m2D8->checkFrame(12.0f)) {
            fopAcM_seStart(a_this, JA_SE_CM_B_CC_RECOVER, 0);
        }
        if (i_this->m2D8->isStop()) {
            cyl_OnTgSet(i_this);
            i_this->m301 = 0;
            i_this->mStts.SetWeight(0x32);
            i_this->mCyl.SetTgType(CC_TG_TYPE_ALL);
            anm_init(i_this, dRes_INDEX_CC_BCK_HUSE2TACHI_e, 0.0f, 0, 1.0f, -1);
            fopAcM_seStart(a_this, JA_SE_CM_CC_LIE_TO_STAND, 0);
            s16 a = fopAcM_searchPlayerAngleY(a_this);
            a_this->shape_angle.y = a;
            a_this->current.angle.y = a;
            i_this->mCurrAction = 0;
            i_this->m2F5 = 0xb;
        }
        break;
    }

    if (a_this->health > 0 && !body_atari_check(i_this)) {
        if (i_this->m2F5 >= 0x2A) {
            shock_damage_check(i_this);
        }
        black_light_check(i_this);
    }
}

/* inlined into daCC_Execute */
static inline void dead_drop(cc_class* i_this, cXyz* pos) {
    fopAc_ac_c* a_this = &i_this->actor;
    if (i_this->mBehaviorType == 3) {
        if (a_this->stealItemBitNo != 0xff) {
            fopAcM_createDisappear(a_this, pos, 5, daDisItem_IBALL_e, a_this->stealItemBitNo);
        }
    } else if (i_this->m304 == 0) {
        fopAcM_createDisappear(a_this, pos, 5, daDisItem_IBALL_e, a_this->stealItemBitNo);
    } else if (i_this->m304 != 2) {
        u8 item;
        if (dComIfGs_getLife() <= 4) {
            item = daDisItem_HEART_e;
        } else if (cM_rndF(1.0f) < 0.5f) {
            if (dComIfGs_getMagic() <= 0x10) {
                item = daDisItem_MAGIC_e;
            } else if (dComIfGs_getArrowNum() <= 10) {
                item = daDisItem_ARROW_e;
            } else {
                item = daDisItem_HEART_e;
            }
        } else {
            item = daDisItem_NONE13_e;
        }
        fopAcM_createDisappear(a_this, pos, 5, item, 0xFF);
    } else {
        fopAcM_createDisappear(a_this, pos, 5, daDisItem_NONE13_e, 0xFF);
    }
    fopAcM_delete(a_this);
    dComIfGs_onActor(a_this->setID, a_this->home.roomNo); /* fopAcM_onActor */
}

/* inlined into daCC_Execute */
static inline void action_dead_move(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    gabi::Local<cXyz> sp08;
    /* HD: recompiled lfs/stfs pairs keep a signalling NaN's bits (the host elides the double round trip);
     * the stack copy is compared exactly, so copy the untouched words bit for bit */
    reinterpret_cast<be<u32>&>(sp08->x) = reinterpret_cast<be<u32>&>(a_this->current.pos.x);
    sp08->y = a_this->current.pos.y;
    reinterpret_cast<be<u32>&>(sp08->z) = reinterpret_cast<be<u32>&>(a_this->current.pos.z);
    sp08->y += 60.0f;

    switch (i_this->m2F5) {
    case 50:
        denki_end(i_this);
        cyl_OffAtSet(i_this);
        cyl_OffTgSet(i_this);
        cyl_OffCoSet(i_this);
        i_this->mCyl.ClrTgHit();
        i_this->m2F5 += 1;
        [[fallthrough]];
    case 51:
        dead_drop(i_this, sp08);
        break;

    case 52: {
        denki_end(i_this);
        cc_eff_set(i_this, 1);
        fopAcM_seStart(a_this, JA_SE_CM_CC_DIE_SWING, 0);
        fopAcM_monsSeStart(a_this, JA_SE_CV_CC_DIE, 0);
        J3DAnmTransform* anm =
            (J3DAnmTransform*)dComIfG_getObjectRes(reinterpret_cast<const char*>(deadArchive),
                                                   dRes_INDEX_CC_BCK_CC_BETA_e, gc_tu.safeString);
        i_this->m2BC->setAnm(anm, 0, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
        a_this->speedF = 0.0f;
        a_this->speed.x = 0.0f;
        a_this->speed.y = 0.0f;
        a_this->speed.z = 0.0f;
        cyl_OffAtSet(i_this);
        cyl_OffTgSet(i_this);
        cyl_OffCoSet(i_this);
        i_this->mCyl.ClrTgHit();
        i_this->m34E[0] = 0;
        i_this->m2F5 += 1;
    }
        [[fallthrough]];
    case 53:
        if (i_this->m2BC->isStop()) {
            dead_drop(i_this, sp08);
        }
        break;
    }
}

/* 0210DD3C */
static BOOL daCC_Execute(cc_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;

    a_this->model = 0;
    switch (i_this->mColorType) {
    case 2:
        fopAcM_setGbaName(a_this, dItemNo_BOW_e, 30, 0x2b);
        break;
    case 3:
        fopAcM_setGbaName(a_this, dItemNo_MIRROR_SHIELD_e, 16, 0x2e);
        break;
    case 4:
        fopAcM_setGbaName(a_this, dItemNo_BOW_e, 10, 0x28);
        break;
    }

    if ((i_this->m2F5 >= 6 && i_this->m2F5 < 11) || i_this->mCurrAction == 1) {
        a_this->eyePos.x = a_this->current.pos.x;
        a_this->eyePos.y = a_this->current.pos.y;
        a_this->eyePos.z = a_this->current.pos.z;
        a_this->eyePos.y += 10.0f;
    } else {
        a_this->eyePos.x = a_this->current.pos.x;
        a_this->eyePos.y = a_this->current.pos.y;
        a_this->eyePos.z = a_this->current.pos.z;
        a_this->eyePos.y += 70.0f;
    }

    cXyz* attn = &a_this->attention_info.position; /* attention_info.position */
    if (i_this->mCurrAction != 5) {
        attn->x = a_this->eyePos.x;
        attn->y = a_this->eyePos.y;
        attn->z = a_this->eyePos.z;
        attn->y += 30.0f;
    } else {
        attn->x = a_this->current.pos.x;
        attn->y = a_this->current.pos.y;
        attn->z = a_this->current.pos.z;
        attn->y += 30.0f;
    }

    if (enemy_ice(&i_this->mEnemyIce)) {
        J3DModel* model = i_this->m2B4->getModel();
        J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
        i_this->m2B4->calc();
        naraku_check(i_this);
        return TRUE;
    }

    for (s32 i = 0; i < 7; i++) {
        if (i_this->m34E[i] != 0) {
            i_this->m34E[i] -= 1;
        }
    }

    if (i_this->mCurrAction != 5 && i_this->mCurrAction != 4 && a_this->health > 0 &&
        i_this->mBehaviorType == 2 && DEMO_RET_START_FLAG() != 0) {
        i_this->m34E[5] = 0;
        i_this->mCurrAction = 5;
        i_this->m2F5 = 0x44;
    }

    switch (i_this->mCurrAction) {
    case 0:
        action_nomal_move(i_this);
        break;
    case 1:
        action_oyogu(i_this);
        break;
    case 2:
        action_attack_move(i_this);
        break;
    case 3:
        action_damage_move(i_this);
        break;
    case 4:
        action_dead_move(i_this);
        break;
    case 5:
        action_noboru(i_this);
        break;
    case 6:
        action_up_check(i_this);
        break;
    case 7:
        action_tomaru(i_this);
        break;
    case 8:
        action_tubo_search(i_this);
        break;
    }

    if (i_this->mBehaviorType == 4) {
        return TRUE;
    }

    if (i_this->mColorType == 2 || i_this->mColorType == 4) {
        Mtx34* mtx = cc_getAnmMtx(i_this->m2B4->getModel(), CC_JNT_BODY03_e);
        i_this->m3A4.x = mtx->m[0][3];
        i_this->m3A4.y = mtx->m[1][3];
        i_this->m3A4.z = mtx->m[2][3];

        switch (i_this->m2F7) {
        case 0:
            /* HD: dPa_followEcallBack::end (GameCube remove()) */
            dPa_followEcallBack_end(&i_this->m368);
            dPa_followEcallBack_end(&i_this->m37C);
            break;

        case 1:
            if (i_this->m368.getEmitter() == nullptr) {
                dComIfGp_particle_set(ID_AK_JN_CCTHUNDER00, &i_this->m3A4, nullptr, nullptr, 0xff,
                                      (dPa_levelEcallBack*)&i_this->m368);
            }
            if (i_this->m37C.getEmitter() == nullptr) {
                dComIfGp_particle_set(ID_AK_JN_CCTHUNDER01, &i_this->m3A4, nullptr, nullptr, 0xff,
                                      (dPa_levelEcallBack*)&i_this->m37C);
            }
            if (i_this->m368.getEmitter() != nullptr && i_this->m37C.getEmitter() != nullptr) {
                i_this->m2F7 += 1;
            }
            break;

        case 2:
            /* emitter->setGlobalRTMatrix(getAnmMtx(BODY03)) */
            if (JPABaseEmitter* e = i_this->m368.getEmitter()) {
                Mtx34* mtx2 = cc_getAnmMtx(i_this->m2B4->getModel(), CC_JNT_BODY03_e);
                CcExecRotation* r =
                    CcExec_globalRotation(&reinterpret_cast<CcExecEmitter*>(e)->mGlobalRotation);
                JPASetRMtxTVecfromMtx(mtx2, r,
                                      &reinterpret_cast<CcExecEmitter*>(e)->mGlobalTranslation);
            }
            if (JPABaseEmitter* e = i_this->m37C.getEmitter()) {
                Mtx34* mtx2 = cc_getAnmMtx(i_this->m2B4->getModel(), CC_JNT_BODY03_e);
                CcExecRotation* r =
                    CcExec_globalRotation(&reinterpret_cast<CcExecEmitter*>(e)->mGlobalRotation);
                JPASetRMtxTVecfromMtx(mtx2, r,
                                      &reinterpret_cast<CcExecEmitter*>(e)->mGlobalTranslation);
            }
            if (i_this->m34E[3] == 0) {
                fopAcM_seStart_ool(a_this, JA_SE_CM_CC_BLUE_SPARK, 0);
                i_this->m34E[3] = (s16)gabi::ftoi(cM_rndF(42.0f) + 30.0f);
            }
            break;
        }
    }

    cMtx_YrotS(calc_mtx(), a_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), a_this->current.angle.x);
    gabi::Local<cXyz> sp30;
    sp30->x = 0.0f;
    sp30->y = 0.0f;
    reinterpret_cast<be<u32>&>(sp30->z) =
        reinterpret_cast<be<u32>&>(a_this->speedF); /* HD: word copy, as in dead_move */
    gabi::Local<cXyz> sp24;
    MtxPosition(sp30, sp24);

    a_this->speed.x = sp24->x;
    a_this->speed.z = sp24->z;
    a_this->speed.y += a_this->gravity;

    if (i_this->mCurrAction != 5) {
        i_this->m316.x = 0;
        i_this->m316.y = 0;
        i_this->m316.z = 0;
        if (a_this->speed.y < -55.0f) {
            a_this->speed.y = -55.0f;
        }
    } else if (a_this->speed.y < -55.0f) {
        a_this->speed.y = -55.0f;
    } else if (a_this->speed.y > 1.0f) {
        a_this->speed.y = 1.0f;
    }

    if (i_this->m2F5 != 0x35) {
        if (i_this->mBehaviorType == 2 && i_this->m2FB != 3) {
            J3DModel_setBaseTRMtx(i_this->m2B4->getModel(), &i_this->m7EC);
            a_this->current.pos.copy(i_this->m3BC);
        }

        if (i_this->m301 == 0) {
            i_this->m2B4->play(nullptr, 0, 0);
        } else {
            i_this->m2D8->play(nullptr, 0, 0);
        }

        if (i_this->m2FE != 0) {
            i_this->m2C4->play(nullptr, 0, 0);
            switch (i_this->m2FE) {
            case 1:
            case 2:
                i_this->m2FF += 1;
                if (i_this->m2FF > 0x19) {
                    i_this->m2FF = 0x19;
                }
                break;
            }
        }

        if (i_this->mBehaviorType == 2 && i_this->m2FB != 3) {
            gabi::Local<cXyz> sp18;
            sp18->x = 0.0f;
            sp18->y = 0.0f;
            sp18->z = 0.0f;
            PSMTXCopy(&i_this->m7EC, mDoMtx_stack_c::get());
            PSMTXMultVec(mDoMtx_stack_c::get(), sp18, &i_this->m3BC);
            a_this->current.pos.copy(i_this->m3BC);
        }
    } else {
        i_this->m2BC->play(nullptr, 0, 0);
    }

    i_this->mCyl.SetC(&a_this->current.pos);
    f32 h = 100.0f;
    if (i_this->m320 == dRes_INDEX_CC_BCK_HUSE_WALK_e) {
        h = 30.0f;
    }
    i_this->mCyl.SetH(h);
    i_this->mCyl.SetR(35.0f);
    dComIfG_Ccsp_Set(&i_this->mCyl);
    if (!((a_this->actor_status & fopAcStts_CARRY_e) != 0)) {
        if (i_this->mCyl.mObjCo.mSPrm & 1) { /* ChkCoSet */
            fopAcM_posMove(a_this, &i_this->mStts.m_cc_move);
        } else {
            fopAcM_posMove(a_this, nullptr);
        }
    }
    BG_check(i_this);
    draw_SUB(i_this);
    return TRUE;
}

/* WWHD glue
data splitArchive 1000C530
data deadArchive 1000C533
ext 0210C828 void naraku_check(cc_class*)
ext 0210BB58 void draw_SUB(cc_class*)
ext 0210C364 void cc_eff_set(cc_class*, u8)
ext 0210C5C8 void anm_init(cc_class*, int, f32, u8, f32, int)
ext 0210CBC0 void denki_start(cc_class*)
ext 0210CC00 void denki_end(cc_class*)
ext 0210CC58 BOOL shock_damage_check(cc_class*)
ext 0210CE2C void black_light_check(cc_class*)
ext 0210CE9C BOOL body_atari_check(cc_class*)
ext 0210DA18 BOOL search_angle_set(cc_class*)
ext 0210DC8C void BG_check(cc_class*)
ext 021116B8 void action_noboru(cc_class*)
ext 02112414 void action_up_check(cc_class*)
ext 02112DA0 void action_tomaru(cc_class*)
ext 02112E74 void action_tubo_search(cc_class*)
ext 02111638 void fopAcM_seStart_ool(fopAc_ac_c*, u32, u32)
ext 0211310C CcExecRotation* CcExec_globalRotation(CcExecRotation*)
ext 020402C8 BOOL enemy_ice(enemyice_l*)
ext 02041CAC void enemy_piyo_set(fopAc_ac_c*)
ext 025DA088 void fopAcM_setGbaName(fopAc_ac_c*, u8, u8, u8)
ext 028249B0 void JPASetRMtxTVecfromMtx(Mtx34*, CcExecRotation*, cXyz*)
ext 025E1AA4 void CcExec_monsSeStart(u32, cXyz*, u32, u32, s32)
*/

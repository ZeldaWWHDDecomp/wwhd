/**
 * d_a_cc_act.cpp (WWHD)
 * Enemy - ChuChu: action_up_check, body_atari_check, action_noboru
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_cc.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_cc.h"

#define CC_SAFESTRING_VTBL 0x1000C548 /* this TU's sead::SafeString vtable */

enum {
    dRes_INDEX_CC_BCK_ATACK01_e = 0x14,
    dRes_INDEX_CC_BCK_ATACK02_e = 0x15,
    dRes_INDEX_CC_BCK_ATACK03_e = 0x16,
    dRes_INDEX_CC_BCK_DEKU_START_e = 0x19,
    dRes_INDEX_CC_BCK_MAHI_e = 0x1D,
    dRes_INDEX_CC_BCK_START_e = 0x1E,
    dRes_INDEX_CC_BCK_TACHI2HUSE_e = 0x1F,
    dRes_INDEX_CC_BCK_TACHI_WALK_e = 0x20,
};
/* static demo flags of d_a_cc (HD .data) */
#define DEMO_COME_START_FLAG (*gabi::at<be<u8>>(0x101B3AFC))
#define DEMO_RET_START_FLAG (*gabi::at<be<u8>>(0x101B3AFD))
#define DEMO_SHORT_CUT_FLAG (*gabi::at<be<u8>>(0x101B3AFE))
/* string literals / cut_name_tbl of the two inlined demo functions (each copy has its own) */
#define STR_CyuCyu_STR 0x1000C5D4
#define STR_CyuCyu_RET 0x1000C5EC
#define STR_CyuCyu_come 0x1000C538
#define STR_CyuCyu_ret 0x1000C540
#define CUT_NAME_TBL_come 0x101B3AEC
#define CUT_NAME_TBL_ret 0x101B3AF4
enum {
    JA_SE_LK_SW_HIT_S = 0x2803,
    JA_SE_LK_W_WEP_HIT = 0x2833,
    JA_SE_LK_MS_WEP_HIT = 0x2834,
    JA_SE_LK_HAMMER_HIT = 0x2855,
    JA_SE_CV_CC_DIE = 0x4863,
    JA_SE_CM_CC_DIE_SWING = 0x5865,
    JA_SE_CV_CC_DAMAGE = 0x4862,
    JA_SE_CM_CC_ATTACK = 0x5863,
    JA_SE_CM_CC_DMG_SWING = 0x5864,
    JA_SE_CM_CC_ENTER_GND = 0x585D,
    JA_SE_CM_CC_LANDING = 0x585E,
    JA_SE_CM_CC_STAND_TO_LIE = 0x585F,
    JA_SE_CM_CC_LIE_TO_STAND = 0x5860,
    JA_SE_CV_CC_ATTACK = 0x4861,
    JA_SE_CV_CC_PARASITE = 0x48BB,
    JA_SE_CM_CC_STONED = 0x58FB,
    JA_SE_CM_CC_STONED_RECOVER = 0x58FC,
    JA_SE_OBJ_PUT_STONE = 0x6929,
    JA_SE_OBJ_BREAK_STONE = 0x692B,
};
enum {
    fopAcStts_FREEZE_e = 0x400,
    fopAcStts_CARRY_e = 0x2000,
    fopAc_Attn_LOCKON_BATTLE_e = 0x4,
    fopAc_Attn_ACTION_CARRY_e = 0x10,
};

/* ---- functions of the other cc files (guest calls by address) ---- */
static inline void anm_init(cc_class* i_this, int idx, f32 morf, u8 loopMode, f32 speed, int soundIdx) {
    gabi::call(0x0210C5C8, i_this, idx, morf, loopMode, speed, soundIdx);
}

static inline void denki_end(cc_class* i_this) { gabi::call(0x0210CC00, i_this); }
static inline void denki_start(cc_class* i_this) { gabi::call(0x0210CBC0, i_this); }
static inline void cc_eff_set(cc_class* i_this, u8 arg1) { gabi::call(0x0210C364, i_this, arg1); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02515BBC dCcD_GAtTgCoCommonBase::GetAc(): HD GetTgHitAc() is this out-of-line call on &mGObjTg
 * (the shared inline dCcD_GObjInf::GetTgHitAc() reads mGObjTg.mAc directly: wrong for HD) */
static inline fopAc_ac_c* dCcD_GetTgHitAc(dCcD_GObjInf* o) { return gabi::call<fopAc_ac_c*>(0x02515BBC, &o->mGObjTg); }
/* 025192A8 cc_at_check(fopAc_ac_c*, CcAtInfo*) */
static inline fopAc_ac_c* cc_at_check(fopAc_ac_c* a, void* info) { return gabi::call<fopAc_ac_c*>(0x025192A8, a, info); }
/* 02041C30 enemy_fire_remove(enemyfire*) */
static inline void enemy_fire_remove(enemyfire_l* f) { gabi::call(0x02041C30, f); }
/* CcAtInfo (stack object), GameCube layout */
struct CcAtInfo_l {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ be<u32> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(CcAtInfo_l, 0x1C);
/* 02542EDC dEvent_manager_c::getMyActIdx(staffId, const char* const* names, int n, BOOL force, int) */
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 names, s32 n, BOOL force, s32 p) {
    return (s8)gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, names, n, force, p);
}
/* 024EECAC dBgS::GetMtrlSndId(const cBgS_PolyInfo&) */
static inline u32 dBgS_GetMtrlSndId(void* poly) { return gabi::call<u32>(0x024EECAC, dComIfG_Bgsp(), poly); }
/* dComIfGs_onTmpBit(flag): dSv_event_c::onEventBit on the save info's tmp event block (+0x1178) */
static inline void dComIfGs_onTmpBit(u16 flag) {
    dSv_event_onEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178), flag);
}
/* daPy_py_c::checkFrontRollCrash(): HD +0x3C0 bit 0x2000 */
static inline bool daPy_checkFrontRollCrash(fopAc_ac_c* pl) { return (gabi::load<u32>(gabi::ea(pl) + 0x3C0) & 0x2000) != 0; }
/* daPy_py_c::getCutType(): mCutType (HD +0x3AC) */
static inline u8 daPy_getCutType(fopAc_ac_c* pl) { return gabi::load<u8>(gabi::ea(pl) + 0x3AC); }
/* 0252A038 dDetect_c::chk_light(const cXyz*) (dDetect_c at play+0x5A20) */
static inline BOOL dComIfGp_getDetect_chk_light(cXyz* pos) {
    return gabi::call<BOOL>(0x0252A038, gabi::at<dDetect_c>(dComIfGp_ea() + PLAY_DETECT), pos);
}
static inline bool fopAcM_checkCarryNow(fopAc_ac_c* a) { return (a->actor_status & fopAcStts_CARRY_e) != 0; }
static inline be<u32>& attention_flags(fopAc_ac_c* a) { return *gabi::at<be<u32>>(gabi::ea(a) + 0x39C); }
/* attention_info.position (HD +0x390) */
static inline cXyz* attention_pos(fopAc_ac_c* a) { return gabi::at<cXyz>(gabi::ea(a) + 0x390); }
/* fopAcM_onActor: dComIfGs_onActor(setID, home.roomNo) */
static inline void fopAcM_onActor(fopAc_ac_c* a) { dComIfGs_onActor(a->setID, a->home.roomNo); }
/* JPABaseEmitter::setGlobalPrmColor / setGlobalEnvColor (HD: +0x244 / +0x248) */
static inline void JPABaseEmitter_setGlobalPrmColor(JPABaseEmitter* e, u8 r, u8 g, u8 b) {
    gabi::store<u8>(gabi::ea(e) + 0x244, r);
    gabi::store<u8>(gabi::ea(e) + 0x245, g);
    gabi::store<u8>(gabi::ea(e) + 0x246, b);
}
static inline void JPABaseEmitter_setGlobalEnvColor(JPABaseEmitter* e, u8 r, u8 g, u8 b) {
    gabi::store<u8>(gabi::ea(e) + 0x248, r);
    gabi::store<u8>(gabi::ea(e) + 0x249, g);
    gabi::store<u8>(gabi::ea(e) + 0x24A, b);
}
/* dKy_tevstr_c::mColorK0 (HD +0x98 in tevStr, actor +0x1A8) */
static inline u8 tevK0(fopAc_ac_c* a, int i) { return gabi::load<u8>(gabi::ea(a) + 0x1A8 + i); }
/* mDoExt_brkAnm / btkAnm frame (HD +4) */
static inline void anm_setFrame(void* anm, f32 f) { gabi::store<f32>(gabi::ea(anm) + 4, f); }

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
        gabi::call(0x025E1AA4, se, &a->eyePos, id, param, reverb); /* mDoAud_monsSeStart */
    }
}
static inline void monsSeStart_nn(fopAc_ac_c* a, u32 se, u32 param) {
    if (gabi::ea(&a->eyePos) != 0) {
        s32 room = fopAcM_GetRoomNo(a);
        u32 id = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, se, &a->eyePos, id, param, reverb); /* mDoAud_monsSeStart */
    }
}

/* 02112414 */
static void action_up_check(cc_class* i_this) {
    WWHD_FUNC(0x02112414, void, i_this);
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s16 bVar4 = 0;

    switch (i_this->m2F5) {
    case 0x50:
        i_this->m2F6 = 5;
        i_this->mCyl.mGObjTg.mSPrm |= 1;      /* OnTgShield */
        i_this->mCyl.OnTgSPrmBit(1);           /* OnTgSetBit */
        i_this->mCyl.SetTgType(~(0x400000 | 0x20000 | 0x100));
        anm_init(i_this, dRes_INDEX_CC_BCK_MAHI_e, 0.0f, 0, 0.0f, -1);
        i_this->m2F9 = 0;
        fopAcM_seStart(a_this, JA_SE_CM_CC_STONED, 0);

        a_this->speed.x = 0.0f;
        a_this->speed.y = 0.0f;
        a_this->speed.z = 0.0f;

        i_this->mCyl.OnCoSPrmBit(1);           /* OnCoSetBit */
        i_this->mCyl.OffAtSPrmBit(1);          /* OffAtSPrmBit(cCcD_AtSPrm_Set_e), ClrAtSet */

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
        anm_setFrame(i_this->m2C8, 0.0f);
        anm_setFrame(i_this->m2D0, 0.0f);
        i_this->m2FE = 1;
        i_this->m2FF = 0;
        i_this->m301 = 0;
        i_this->m34E[4] = 600;
        a_this->actor_status |= fopAcStts_FREEZE_e;
        i_this->m2F5 = i_this->m2F5 + 1;
        break;

    case 0x52:
        if (i_this->m2FE != 2) {
            attention_flags(a_this) |= fopAc_Attn_ACTION_CARRY_e;
        }

        cLib_addCalc0(&a_this->speedF, 0.5f, 1.0f);

        if (fopAcM_checkCarryNow(a_this)) {
            i_this->m338 = a_this->current.pos.y;
            a_this->speedF = 0.0f;
            i_this->mCyl.OffCoSPrmBit(1);
            a_this->current.angle.y = player->shape_angle.y;
            a_this->gravity = 0.0f;
            a_this->speed.x = 0.0f;
            a_this->speed.y = 0.0f;
            a_this->speed.z = 0.0f;
            i_this->mCyl.OffCoSPrmBit(1);
            attention_flags(a_this) = 0;
            i_this->m2F5 = i_this->m2F5 + 1;
        }
        break;

    case 0x53:
        a_this->current.angle.y = player->shape_angle.y;
        if (!fopAcM_checkCarryNow(a_this)) {
            attention_flags(a_this) = fopAc_Attn_LOCKON_BATTLE_e;
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
        if (dComIfGp_getDetect_chk_light(&a_this->current.pos) != FALSE) {
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
                anm_setFrame(i_this->m2CC, 0.0f);
                anm_setFrame(i_this->m2D4, 0.0f);
                attention_flags(a_this) &= ~(u32)fopAc_Attn_ACTION_CARRY_e;
            }
        }

        if (i_this->m2FE == 2 && i_this->m2FF >= 0x19) {
            if (fopAcM_checkCarryNow(a_this)) {
                fopAcM_cancelCarryNow(a_this);
                attention_flags(a_this) &= ~(u32)fopAc_Attn_ACTION_CARRY_e;
            }

            attention_flags(a_this) = fopAc_Attn_LOCKON_BATTLE_e;
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
            void* pcVar6 = i_this->mCyl.GetTgHitObj();
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
            dComIfGp_particle_set(0xC /* dPa_name::ID_AK_JN_NG */, sp24, &player->shape_angle);

            switch (gabi::load<u32>(gabi::ea(pcVar6) + 0x10) /* GetAtType() */) {
            case 0x2:        /* AT_TYPE_SWORD */
            case 0x400:      /* AT_TYPE_MACHETE */
            case 0x800:      /* AT_TYPE_UNK800 */
            case 0x4000000:  /* AT_TYPE_DARKNUT_SWORD */
            case 0x10000000: /* AT_TYPE_MOBLIN_SPEAR */
                seStart_nn(a_this, JA_SE_LK_SW_HIT_S, 13);
                break;

            case 0x40:      /* AT_TYPE_BOKO_STICK */
            case 0x80:      /* AT_TYPE_BOOMERANG */
            case 0x2000:    /* AT_TYPE_UNK2000 */
            case 0x1000000: /* AT_TYPE_STALFOS_MACE */
                seStart_nn(a_this, JA_SE_LK_W_WEP_HIT, 13);
                break;

            case 0x10000: /* AT_TYPE_SKULL_HAMMER */
                bVar4 = 1;
                break;

            case 0x1000:
            case 0x4000:
            case 0x8000:
            case 0x40000:
            case 0x80000:
            case 0x100000:
            case 0x8000000:
                seStart_nn(a_this, JA_SE_LK_MS_WEP_HIT, 13);
                break;
            }
        }
    }

    if (bVar4) {
        JPABaseEmitter* pJVar7 = dComIfGp_particle_set(0x82A9 /* ID_AK_SN_CCBREAKHAHEN00 */, attention_pos(a_this), &a_this->current.angle);
        if (pJVar7 != nullptr) {
            JPABaseEmitter_setGlobalPrmColor(pJVar7, tevK0(a_this, 0), tevK0(a_this, 1), tevK0(a_this, 2));
            JPABaseEmitter_setGlobalEnvColor(pJVar7, tevK0(a_this, 0), tevK0(a_this, 1), tevK0(a_this, 2));
        }

        gabi::Local<cXyz> sp18;
        sp18->copy(a_this->current.pos); /* lfs/stfs pairs folded by clang: bit-exact */
        sp18->y = a_this->current.pos.y + 60.0f;

        seStart_nn(a_this, JA_SE_OBJ_BREAK_STONE, 0);

        fopAcM_createDisappear(a_this, sp18, 5, 0 /* daDisItem_IBALL_e */, a_this->stealItemBitNo);
        fopAcM_delete(a_this);
        fopAcM_onActor(a_this);
    }
}
VERIFY(0x02112414, action_up_check);

static inline bool cc_isCriticalCut(u8 t) {
    switch (t) {
    case 0x06: /* CUT_TYPE_CUT_EA */
    case 0x07: /* CUT_TYPE_CUT_EB */
    case 0x08: /* CUT_TYPE_CUT_TURN */
    case 0x09: /* CUT_TYPE_CUT_ROLL */
    case 0x0A: /* CUT_TYPE_JUMPCUT_SWORD */
    case 0x0C: /* CUT_TYPE_JUMPCUT_STICK */
    case 0x0E: /* CUT_TYPE_JUMPCUT_MACHETE */
    case 0x05: /* CUT_TYPE_BT_JUMPCUT */
    case 0x0F: /* CUT_TYPE_BT_ROLLCUT */
    case 0x10: /* CUT_TYPE_BT_VERTICALJUMPCUT */
    case 0x15: /* CUT_TYPE_JUMPCUT_CLUB */
    case 0x17: /* CUT_TYPE_JUMPCUT_DN_SWORD */
    case 0x19: /* CUT_TYPE_JUMPCUT_SPEAR */
    case 0x1A: /* CUT_TYPE_CUT_EXA */
    case 0x1B: /* CUT_TYPE_CUT_EXB */
    case 0x1E: /* CUT_TYPE_CUT_EXMJ */
    case 0x1F: /* CUT_TYPE_CUT_KESA */
        return true;
    }
    return false;
}

/* 0210CE9C */
static BOOL body_atari_check(cc_class* i_this) {
    WWHD_FUNC(0x0210CE9C, BOOL, i_this);
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    i_this->mStts.Move();

    if (i_this->mCyl.ChkTgHit()) {
        void* pcVar6 = i_this->mCyl.GetTgHitObj();
        fopAc_ac_c* pdVar7 = dCcD_GetTgHitAc(&i_this->mCyl);
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
            dComIfGp_particle_set(0x10 /* ID_AK_JN_CRITICALHITFLASH */, sp30);
            gabi::Local<cXyz> sp24;
            sp24->set(2.0f, 2.0f, 2.0f);
            dComIfGp_particle_set(0xF /* ID_AK_JN_CRITICALHIT */, sp30, &player->shape_angle, sp24);
            i_this->mCurrAction = 3;
            i_this->m2F5 = 0x28;
            return TRUE;
        }

        i_this->actor.mBtNowFrame = 1000.0f; /* setBtNowFrame */

        gabi::Local<CcAtInfo_l> sp3C;
        sp3C->pParticlePos = 0;
        bool bVar4 = false;
        i_this->m2F6 = 0;

        u32 atType = gabi::load<u32>(gabi::ea(pcVar6) + 0x10);
        if (atType & 0x800000 /* AT_TYPE_LIGHT */) {
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
        case 0x8000000: /* AT_TYPE_GRAPPLING_HOOK */
            if (a_this->stealItemLeft > 0) {
                s8 sVar9 = a_this->health;
                u8 bVar1 = a_this->stealItemBitNo;
                a_this->health = 10;
                sp3C->mpObj = gabi::ea(i_this->mCyl.GetTgHitObj());
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

        case 0x400:      /* AT_TYPE_MACHETE */
        case 0x2:        /* AT_TYPE_SWORD */
        case 0x800:      /* AT_TYPE_UNK800 */
        case 0x4000000:  /* AT_TYPE_DARKNUT_SWORD */
        case 0x10000000: /* AT_TYPE_MOBLIN_SPEAR */
            if (i_this->m2F7 != 0) {
                return FALSE;
            }

            if (cc_isCriticalCut(daPy_getCutType(player))) {
                i_this->m2F6 = 1;
            }

            if (i_this->mColorType == 3) {
                fopAcM_seStart(a_this, JA_SE_LK_SW_HIT_S, 0x36);
                bVar4 = true;
                i_this->m2F6 = 0;
            }
            break;

        case 0x200000: /* AT_TYPE_WIND */
            bVar4 = true;
            i_this->m2F6 = 3;
            a_this->current.angle.y = cM_atan2s(a_this->current.pos.x - sp30->x, a_this->current.pos.z - sp30->z);
            if ((pdVar7 != nullptr) && (pdVar7 == player)) {
                a_this->current.angle.y = player->shape_angle.y;
                if (i_this->m2F5 == 0x15) {
                    a_this->speedF = REG_F(12, 18) + 20.0f;
                    return FALSE;
                }
            }
            break;

        case 0x40: /* AT_TYPE_BOOMERANG */
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

        case 0x80:      /* AT_TYPE_BOKO_STICK */
        case 0x2000:    /* AT_TYPE_UNK2000 */
        case 0x1000000: /* AT_TYPE_STALFOS_MACE */
            if (i_this->mColorType == 3) {
                fopAcM_seStart(a_this, JA_SE_LK_W_WEP_HIT, 0x36);
                bVar4 = true;
                i_this->m2F6 = 0;
            }
            break;

        case 0x8000: /* AT_TYPE_HOOKSHOT */
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

        case 0x10000: /* AT_TYPE_SKULL_HAMMER */
            if (i_this->mColorType == 3) {
                fopAcM_seStart(a_this, JA_SE_LK_HAMMER_HIT, 0x36);
            }

            i_this->m2F6 = 1;

            {
                u8 cut = daPy_getCutType(player);
                if (cut == 0x12 /* CUT_TYPE_HAMMER_FRONTSWING */ || cut == 0x13 /* CUT_TYPE_JUMPCUT_HAMMER */) {
                    i_this->m2F6 = 6;
                }
            }

            if (i_this->mColorType == 3) {
                bVar4 = true;
                i_this->m2F6 = 0;
            }
            break;

        case 0x200:   /* AT_TYPE_FIRE */
        case 0x40000: /* AT_TYPE_FIRE_ARROW */
            if (i_this->mColorType == 3) {
                bVar4 = true;
                i_this->m2F6 = 0;
                fopAcM_seStart(a_this, JA_SE_LK_MS_WEP_HIT, 0x36);
            } else {
                i_this->mEnemyFire.mFireDuration = 100;
                i_this->m2F6 = 8;
            }
            break;

        case 0x100000: /* AT_TYPE_LIGHT_ARROW */
            i_this->mEnemyIce.mLightShrinkTimer = 1;
            i_this->mEnemyIce.mParticleScale = 0.5f;
            i_this->mEnemyIce.mYOffset = 80.0f;
            fopAcM_seStart(a_this, JA_SE_CM_CC_DIE_SWING, 0);
            monsSeStart_full(a_this, JA_SE_CV_CC_DIE, 0);
            attention_flags(a_this) = 0;
            denki_end(i_this);
            i_this->m2F6 = 8;
            break;

        case 0x80000: /* AT_TYPE_ICE_ARROW */
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
            sp3C->mpObj = gabi::ea(i_this->mCyl.GetTgHitObj());
            cc_at_check(a_this, sp3C.get());

            if ((i_this->m2F6 == 1) || (i_this->m2F6 == 6) || (a_this->health <= 0)) {
                dComIfGp_particle_set(0x10 /* ID_AK_JN_CRITICALHITFLASH */, sp30);
                gabi::Local<cXyz> sp18;
                sp18->set(2.0f, 2.0f, 2.0f);
                dComIfGp_particle_set(0xF /* ID_AK_JN_CRITICALHIT */, sp30, &player->shape_angle, sp18);
            } else {
                dComIfGp_particle_set(0xD /* ID_AK_JN_OK */, sp30, &player->shape_angle);
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

        if ((i_this->mColorType == 3) && (i_this->m2F6 != 4 && (i_this->m2F6 != 3)) && (i_this->m2F6 != 9)) {
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
VERIFY(0x0210CE9C, body_atari_check);

/* deku_come_demo (inlined into action_noboru) */
static inline void deku_come_demo(cc_class* i_this) {
    s32 iVar2;
    switch (i_this->m2FC) {
    case 1:
        i_this->m2FC = i_this->m2FC + 1;
        i_this->m308 = dComIfGp_evmng_getEventIdx(gabi::at<char>(STR_CyuCyu_STR), 0xFF);
        break;

    case 2:
        if (eventInfo_checkCommandDemoAccrpt(&i_this->actor)) {
            i_this->m2FC = i_this->m2FC + 1;
            i_this->m34E[5] = 0x28;
            if (DEMO_SHORT_CUT_FLAG != 0) {
                i_this->m34E[5] = 0x14;
            }
            DEMO_COME_START_FLAG = 2;
        } else {
            fopAcM_orderOtherEventId(&i_this->actor, i_this->m308, 0xFF, 0xFFFF, 0, 1);
        }
        break;

    case 3:
        if (i_this->m34E[5] == 0) {
            iVar2 = dComIfGp_evmng_getMyStaffId(gabi::at<char>(STR_CyuCyu_come), nullptr, 0);
            dComIfGp_evmng_cutEnd(iVar2);
            i_this->m2FC = i_this->m2FC + 1;
        }
        break;

    case 4:
        iVar2 = dComIfGp_evmng_getMyStaffId(gabi::at<char>(STR_CyuCyu_come), nullptr, 0);
        if (dComIfGp_evmng_getMyActIdx(iVar2, CUT_NAME_TBL_come, 2, TRUE, 0) == 1) {
            dComIfGp_evmng_cutEnd(iVar2);
            i_this->m2FC = i_this->m2FC + 1;
        }
        break;

    case 5:
        if (dComIfGp_evmng_endCheck(i_this->m308)) {
            dComIfGp_event_reset();
            i_this->m2FC = 0;
            DEMO_COME_START_FLAG = 0;
            dComIfGs_onTmpBit(0x304 /* dSv_event_tmp_flag_c::UNK_0304 */);
        }
        break;
    }
}

/* deku_ret_demo (inlined into action_noboru) */
static inline void deku_ret_demo(cc_class* i_this) {
    s32 iVar2;
    switch (i_this->m2FD) {
    case 2:
        if (eventInfo_checkCommandDemoAccrpt(&i_this->actor)) {
            i_this->m2FD = i_this->m2FD + 1;
            i_this->m34E[5] = 0x28;
            if (DEMO_SHORT_CUT_FLAG != 0) {
                i_this->m34E[5] = 0x14;
            }
        } else {
            fopAcM_orderOtherEventId(&i_this->actor, i_this->m308, 0xFF, 0xFFFF, 0, 1);
        }
        break;

    case 3:
        if (i_this->m34E[5] == 0) {
            anm_init(i_this, dRes_INDEX_CC_BCK_START_e, 0.0f, 0, 1.0f, -1);
            DEMO_RET_START_FLAG = 2;
            i_this->m34E[5] = 0x50;
            if (DEMO_SHORT_CUT_FLAG != 0) {
                i_this->m34E[5] = 0x28;
            }
            i_this->m2FD = i_this->m2FD + 1;
        }
        break;

    case 4:
        if (i_this->m34E[5] == 0) {
            iVar2 = dComIfGp_evmng_getMyStaffId(gabi::at<char>(STR_CyuCyu_ret), nullptr, 0);
            dComIfGp_evmng_cutEnd(iVar2);
            i_this->m2FD = i_this->m2FD + 1;
        }
        break;

    case 5:
        iVar2 = dComIfGp_evmng_getMyStaffId(gabi::at<char>(STR_CyuCyu_ret), nullptr, 0);
        if (dComIfGp_evmng_getMyActIdx(iVar2, CUT_NAME_TBL_ret, 2, TRUE, 0) == 1) {
            dComIfGp_evmng_cutEnd(iVar2);
            i_this->m2FD = i_this->m2FD + 1;
        }
        break;

    case 6:
        if (dComIfGp_evmng_endCheck(i_this->m308)) {
            dComIfGp_event_reset();
            i_this->m2FD = 0;
            DEMO_RET_START_FLAG = 0;
            DEMO_SHORT_CUT_FLAG = 1;
            i_this->m2F5 = 0x3F;
        }
        break;
    }
}

/* 021116B8 */
static void action_noboru(cc_class* i_this) {
    WWHD_FUNC(0x021116B8, void, i_this);
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    switch (i_this->m2F5) {
    case 0x3C:
        i_this->m3BA = -0x4000;
        i_this->m2FB = 0;
        attention_flags(a_this) = 0;
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

        if (!daPy_checkFrontRollCrash(player)) {
            break;
        }

        anm_init(i_this, dRes_INDEX_CC_BCK_ATACK01_e, 0.0f, 0, 1.0f, -1);
        i_this->m2F5 = i_this->m2F5 + 1;
        [[fallthrough]];

    case 0x40:
        if (DEMO_COME_START_FLAG == 0) {
            i_this->m2FC = 1;
            DEMO_COME_START_FLAG = 1;
        } else if (DEMO_COME_START_FLAG == 2) {
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

        attention_flags(a_this) = fopAc_Attn_LOCKON_BATTLE_e;
        i_this->mCyl.SetTgType(~(0x400000 | 0x20000 | 0x100));
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
            fopAcM_seStart(a_this, JA_SE_CM_CC_LANDING, dBgS_GetMtrlSndId(gabi::at<u8>(gabi::ea(i_this) + 0x6AC) /* mAcch.m_gnd */));

            cc_eff_set(i_this, 1);
            a_this->speed.y = 0.0f;
            denki_start(i_this);
            i_this->m2F5 = i_this->m2F5 + 1;
        }
        break;

    case 0x43:
        if (DEMO_COME_START_FLAG == 0) {
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
        dPa_rippleEcallBack_end(gabi::at<dPa_rippleEcallBack>(gabi::ea(i_this->m390))); /* HD: m390 (DEMO_SELECT) */
        attention_flags(a_this) = 0;

        if (i_this->m320 != dRes_INDEX_CC_BCK_TACHI_WALK_e) {
            anm_init(i_this, dRes_INDEX_CC_BCK_TACHI2HUSE_e, 0.0f, 0, 0.0f, -1);
        }

        seStart_nn(a_this, JA_SE_CM_CC_STAND_TO_LIE, 0);

        if (DEMO_RET_START_FLAG == 0) {
            DEMO_RET_START_FLAG = 1;
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
        if (DEMO_RET_START_FLAG == 2) {
            fopAcM_seStart(a_this, JA_SE_CM_CC_LIE_TO_STAND, 0);
            i_this->m334 = 1.0f;
            anm_init(i_this, dRes_INDEX_CC_BCK_START_e, 0.0f, 0, 1.0f, -1);
            i_this->m2F5 = 0x3E;
        }

        if (i_this->m34E[0] == 30 && i_this->m2FD == 1) {
            i_this->m2FD = 2;
            i_this->m308 = dComIfGp_evmng_getEventIdx(gabi::at<char>(STR_CyuCyu_RET), 0xFF);
            i_this->m334 = 1.0f;
            i_this->m2F5 = 0x47;
        }
        break;

    case 0x47:
        deku_ret_demo(i_this);
        if (DEMO_RET_START_FLAG >= 2 && i_this->m320 == dRes_INDEX_CC_BCK_START_e && i_this->m2B4->isStop()) {
            anm_init(i_this, dRes_INDEX_CC_BCK_TACHI_WALK_e, 1.0f, 2, 1.0f, -1);
        }
    }

    if (i_this->m2FC != 0) {
        deku_come_demo(i_this);
    }
}
VERIFY(0x021116B8, action_noboru);

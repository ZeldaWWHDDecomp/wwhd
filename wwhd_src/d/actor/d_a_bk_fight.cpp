/**
 * d_a_bk_fight.cpp (WWHD)
 * Enemy - Bokoblin: attack setup and the fight / run / retreat actions.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bk_local.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb) */
static inline void mDoAud_monsSeStart(u32 id, cXyz* pos, u32 procId, u32 param, s32 reverb) {
    gabi::call(0x025E1AA4, id, pos, procId, param, reverb);
}
/* fopAcM_monsSeStart: HD inline with the actor/eyePos null checks of fopAcM_seStart */
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
        mDoAud_monsSeStart(id, &a->eyePos, pid, param, reverb);
    }
}
/* 0200796C: CPad stick X of a port (HD; 0 for ports other than 0) */
static inline f32 CPad_GET_STICK_POS_X(s32 port) { return gabi::call<f32>(0x0200796C, port); }
/* dAttention_c (play+0x5804) */
static inline BOOL dAttention_LockonTruth(dAttention_c* a) { return gabi::call<BOOL>(0x024EDFCC, a); }
/* 024EC8D0 (matcher: ActionTarget; used where GameCube calls LockonTarget(0)) */
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
/* dAttention_c::Lockon(): LockonTruth() || mFlags (+0x20) & 0x20000000 */
static inline bool dAttention_Lockon(dAttention_c* a) {
    return dAttention_LockonTruth(a) || (gabi::load<u32>(gabi::ea(a) + 0x20) & 0x20000000) != 0;
}
static inline u16 cc_pl_cut_bit_get() { return gabi::call<u16>(0x02518B28); }
/* daPy_py_c::getCutType(): u8 at +0x3AC */
static inline u8 daPy_getCutType(fopAc_ac_c* p) { return gabi::load<u8>(gabi::ea(p) + 0x3AC); }
enum { daPy_CUT_TYPE_JUMPCUT_SWORD = 0xA };
/* dCcD_Sph At / mass (out of line) */
static inline void dCcD_Sph_StartCAt(dCcD_Sph* s, cXyz* p) { gabi::call(0x025167C0, s, p); }
static inline void dCcD_Sph_MoveCAt(dCcD_Sph* s, cXyz* p) { gabi::call(0x025167E4, s, p); }
static inline void dCcMassS_Mng_Set(void* mng, void* obj, u8 prio) { gabi::call(0x02516C14, mng, obj, prio); }
static inline void* dCcD_GObjInf_GetAtHitObj(dCcD_GObjInf* o) { return gabi::call<void*>(0x02516178, o); }
/* cCcD_Obj::GetAc(): mStts (+0x44) ? mStts->mp_actor (+0xC) : NULL */
static inline fopAc_ac_c* cCcD_Obj_GetAc(void* obj) {
    u32 stts = gabi::load<u32>(gabi::ea(obj) + 0x44);
    return stts ? gabi::at<fopAc_ac_c>(gabi::load<u32>(stts + 0xC)) : nullptr;
}
/* daPy_py_c::checkPlayerGuard(): virtual (vtable at +0xB4, slot +0x3C) */
static inline BOOL daPy_checkPlayerGuard(fopAc_ac_c* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 0xB4);
    return gabi::call_ptr<BOOL>(gabi::load<u32>(vt + 0x3C), p);
}
enum { fpcNm_PLAYER_e = 0xA8, fpcNm_BK_e = 0xBD };
static inline f32 cXyz_abs(cXyz* v) { return std_sqrtf(PSVECSquareMag(v)); }
#define REG0_F_(i) REG_F(0, i)
#define REG6_F(i) REG_F(6, i)
#define REG8_F(i) REG_F(8, i)

/* weak guest-call stubs for TU functions defined in d_a_bk.cpp (lets this part build alone;
 * the decompiled definitions replace them) */
__attribute__((weak)) void anm_init(bk_class* a, int b, f32 m, u8 l, f32 s, int snd) { gabi::call(0x02098DA4, a, b, m, l, s, snd); }
__attribute__((weak)) void wait_set(bk_class* a) { gabi::call(0x0209B264, a); }
__attribute__((weak)) BOOL daBk_player_view_check(bk_class* a, cXyz* p, s16 b, s16 c) { return gabi::call<BOOL>(0x0209B05C, a, p, b, c); }
__attribute__((weak)) BOOL daBk_player_bg_check(bk_class* a, cXyz* p) { return gabi::call<BOOL>(0x0209AE7C, a, p); }
__attribute__((weak)) BOOL daBk_player_way_check(bk_class* a) { return gabi::call<BOOL>(0x0209B208, a); }
__attribute__((weak)) BOOL daBk_bomb_view_check(bk_class* a) { return gabi::call<BOOL>(0x0209AE40, a); }

/* ---- d_a_bk data ---- */
struct attack_info_s {
    /* 0x00 */ be<s32> bckFileIdx;
    /* 0x04 */ be<f32> speed;
    /* 0x08 */ be<s32> soundFileIdx;
};
static inline attack_info_s* attack_info(s32 i) { return gabi::at<attack_info_s>(gabi::load<u32>(0x10191528 + 4 * i)); }
static inline u8 bk_at_kind(s32 i) { return (u8)gabi::load<u32>(0x1019148C + 4 * i); }
static inline s32 bk_attack_go_SE(s32 i) { return gabi::load<s32>(0x101914A4 + 4 * i); }
static inline s32 bk_attack_AP(s32 i) { return gabi::load<s32>(0x101914B0 + 4 * i); }
static inline s32 bk_attack_ready_SE(s32 i) { return gabi::load<s32>(0x10191498 + 4 * i); }
static inline be<u16>& learn_check() { return *gabi::at<be<u16>>(0x10462474); }

enum { JA_SE_CV_BK_ATTACK_L = 0x4826, JA_SE_CV_BK_SEARCH = 0x482A, JA_SE_CV_BK_FOUND_LINK = 0x482B, JA_SE_CV_BK_LOST_BOKO = 0x482C, JA_SE_CV_BK_JUMP = 0x482F };
enum { fpcNm_HIMO2_e = 0x1BE };

/* 0209B760 */
void attack_set(bk_class* i_this, u8 r28) {
    WWHD_FUNC(0x0209B760, void, i_this, r28);
    dComIfGp_get(); /* HD: leftover of an inlined player lookup */
    i_this->m0B5C = 0;
    i_this->m11F1 = 0;
    i_this->m11FC = fpcM_ERROR_PROCESS_ID_e;
    i_this->m11F2 = 0;
    i_this->m0B64 = 0.0f;
    i_this->m0300[4] = 0;
    /* HD: no cM_rndF(100.0f) */
    i_this->m1040.SetR(REG8_F(3) + 60.0f);

    if (r28 == 2) {
        i_this->m1040.SetAtType(0x2000);
        i_this->m1040.SetAtSe(6);
    } else if (i_this->m02D5 != 0) {
        i_this->m1040.SetAtType(0x800);
        i_this->m1040.SetAtSe(2);
    } else {
        i_this->m1040.SetAtType(0x2000);
        i_this->m1040.SetAtSe(4);
    }

    if (r28 == 0) {
        i_this->m0B5C = 0;
        i_this->m0B68 = REG6_F(4) + 23.0f;
        i_this->m0B6C = REG6_F(5) + 26.0f;
        i_this->m0B70 = 45.0f;
        i_this->m0B74 = l_bkHIO().m09C;
    } else if (r28 == 1) {
        i_this->m0B5C = 1;
        i_this->m0B68 = REG6_F(8) + 3.0f;
        i_this->m0B6C = REG6_F(9) + 9.0f;
        i_this->m0B70 = 45.0f;
        i_this->m0B74 = 0.0f;
    } else if (r28 == 2) {
        i_this->m0B5C = 2;
        i_this->m0B68 = REG6_F(4);
        i_this->m0B6C = REG6_F(5) + 20.0f;
        i_this->m0B70 = 45.0f;
        i_this->m0B74 = 0.0f;
        i_this->m1040.SetR(REG8_F(3) + 25.0f);
    }

    i_this->m0B7A = 1;
    i_this->m0B60 = 0;

    attack_info_s* info = attack_info(i_this->m0B5C);
    anm_init(i_this, info->bckFileIdx, 5.0f, 0 /* EMode_NONE */, info->speed, info->soundFileIdx);
    s32 se = bk_attack_ready_SE(i_this->m0B5C);
    if (se != -0xDCF) {
        fopAcM_monsSeStart(i_this, se, 0);
    }
}
VERIFY(0x0209B760, attack_set);

/* 0209BA18 */
void* shot_s_sub(void* param_1, void*) {
    WWHD_FUNC(0x0209BA18, void*, param_1, (void*)nullptr);
    if (fopAc_IsActor(param_1) && (learn_check() & 0x400) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_HIMO2_e) {
        return param_1;
    }
    return nullptr;
}
VERIFY(0x0209BA18, shot_s_sub);

/* 020A27F8 */
void p_lost(bk_class* i_this) {
    WWHD_FUNC(0x020A27F8, void, i_this);
    dComIfGp_get();
    i_this->dr.m710 = 0;
    switch ((s16)i_this->dr.mMode) {
    case -10:
        if (i_this->mpMorf->isStop() || i_this->m0300[0] == 0) {
            i_this->m0300[0] = 5;
            i_this->dr.mMode = i_this->dr.mMode + 1;
        }
        break;
    case -9:
        if (i_this->m0300[0] != 0) {
            break;
        }
        /* fall-through */
    case 0:
        i_this->dr.mMode = 1;
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO2_e, 5.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_KYORO2_e);
        i_this->speedF = 0.0f;
        i_this->m0300[1] = (s16)gabi::ftoi(cM_rndF(30.0f) + 30.0f);
        /* fall-through */
    case 1: {
        int frame = gabi::ftoi(i_this->mpMorf->getFrame());
        if ((frame == 0xB || frame == 0x19) && cM_rndF(1.0f) < 0.5f) {
            fopAcM_monsSeStart(i_this, JA_SE_CV_BK_SEARCH, 0);
        }
        if (i_this->m0300[1] == 0) {
            i_this->dr.mAction = 0;
            path_check(i_this, 0);
            wait_set(i_this);
            i_this->dr.mMode = 2;
        }
        break;
    }
    }

    if (i_this->m0300[1] < 10 && daBk_player_view_check(i_this, &i_this->dr.m714->current.pos, i_this->m0332, l_bkHIO().m034)) {
        i_this->m0300[1] = 0;
        i_this->dr.mAction = 4;
        i_this->dr.mMode = 2;
    }
}
VERIFY(0x020A27F8, p_lost);

/* 020A2A54 */
void b_nige(bk_class* i_this) {
    WWHD_FUNC(0x020A2A54, void, i_this);
    /* daBk_bomb_check (inline) */
    fopAc_ac_c* bomb = search_bomb(i_this, 0);
    i_this->m11F8 = bomb;
    if (bomb == nullptr) {
        i_this->dr.mAction = 0;
        path_check(i_this, 0);
        wait_set(i_this);
        i_this->dr.mMode = 2;
        return;
    }

    fopAc_ac_c* r3 = bomb;
    f32 x = r3->current.pos.x - i_this->current.pos.x;
    f32 z = r3->current.pos.z - i_this->current.pos.z;
    i_this->dr.m4D0 = cM_atan2s(-x, -z);

    switch ((u16)i_this->dr.mMode) {
    case 0:
        i_this->dr.mMode = 1;
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_HAKKEN_e, 3.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_HAKKEN_e);
        fopAcM_monsSeStart(i_this, JA_SE_CV_BK_FOUND_LINK, 0);
        i_this->m0300[1] = 20;
        /* fall-through */
    case 1:
        i_this->speedF = 0.0f;
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.m4D0 + 0x8000, 2, 0x3000);
        if (i_this->m0300[1] == 0) {
            i_this->dr.mMode = 2;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_NIGERU_e, 5.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_NIGERU_e);
            fopAcM_monsSeStart(i_this, JA_SE_CV_BK_LOST_BOKO, 0);
        }
        break;
    case 2:
        i_this->speedF = l_bkHIO().m05C;
        i_this->m034C = l_bkHIO().m00C + 3;
        i_this->m034E = 4;
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.m4D0, 4, 0x1000);
        if (std_sqrtf(gabi::fmadds(x, x, z * z)) > 800.0f) {
            i_this->dr.mMode = 3;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_WAIT_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_WAIT_e);
        }
        break;
    case 3:
        i_this->speedF = 0.0f;
        i_this->dr.m4D0 = i_this->m0332;
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.m4D0, 3, 0x1000);
        if (std_sqrtf(gabi::fmadds(x, x, z * z)) < 700.0f) {
            i_this->dr.mMode = 0;
        }
        break;
    }
}
VERIFY(0x020A2A54, b_nige);

/* 020A2E04 */
void defence(bk_class* i_this) {
    WWHD_FUNC(0x020A2E04, void, i_this);
    dComIfGp_get();
    i_this->dr.m710 = 1;
    i_this->dr.m4D0 = i_this->m0332;
    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.m4D0, 4, 0x400);

    switch ((u16)i_this->dr.mMode) {
    case 0:
        i_this->dr.mMode = 1;
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_BOUGYO1_e, 0.0f, 0, 1.0f, -1);
        i_this->speedF = 0.0f;
        /* fall-through */
    case 1:
        i_this->m0F14.SetR(60.0f);
        i_this->m11D8.copy(i_this->m11CC);
        if (i_this->m0300[1] == 0) {
            if (cM_rndF(1.0f) < 0.5f) {
                i_this->dr.mAction = 5;
                i_this->dr.mMode = 0;
            } else {
                i_this->dr.mAction = 4;
                i_this->m0300[1] = 0;
                i_this->dr.mMode = 0;
            }
        }
        break;
    }
}
VERIFY(0x020A2E04, defence);

/* 020A2F68 */
void oshi(bk_class* i_this) {
    WWHD_FUNC(0x020A2F68, void, i_this);
    dComIfGp_get();
    i_this->dr.mAction = 5;
    attack_set(i_this, 1);
    anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_JUMP1_e);
    i_this->dr.mMode = -10;
    i_this->speedF = REG6_F(10) + -90.0f;
    i_this->speed.y = REG6_F(11) + 85.0f;
    fopAcM_monsSeStart(i_this, JA_SE_CV_BK_JUMP, 0);
}
VERIFY(0x020A2F68, oshi);

/* fight_run_set (inline) */
static inline void fight_run_set(bk_class* i_this) {
    anm_init(i_this, dRes_INDEX_BK_BCK_BK_RUN_e, 10.0f, 2, l_bkHIO().m070, dRes_INDEX_BK_BAS_BK_RUN_e);
}

/* 020A0A98 */
void fight_run(bk_class* i_this) {
    WWHD_FUNC(0x020A0A98, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 stickPosX = CPad_GET_STICK_POS_X(0);
    s8 r29 = 0;
    if (i_this->dr.mAcch.ChkGroundHit() && i_this->dr.mAcch.ChkWallHit()) {
        r29 = 1;
    }
    i_this->dr.m4D0 = i_this->m0332;

    if (i_this->dr.mMode != 0) {
        s16 r6 = 0x400;
        if (i_this->dr.mMode == 1) {
            r6 = 0x800;
        }
        if (i_this->dr.mAcch.ChkGroundHit()) {
            cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.m4D0, 4, r6);
        }
    }

    switch ((s16)i_this->dr.mMode) {
    case 0:
        if (i_this->m0300[1] == 0) {
            fight_run_set(i_this);
            i_this->dr.mMode = 1;
            i_this->m120C = 0;
            i_this->m1212 = (s16)gabi::ftoi(cM_rndF(1000.0f));
        } else {
            i_this->speedF = 0.0f;
            break;
        }
        /* fall-through */
    case 1: {
        f32 scaleMag;
        if (i_this->m0B30 != 0 || i_this->m11F3 != 0) {
            scaleMag = l_bkHIO().m054;
        } else {
            scaleMag = l_bkHIO().m058;
        }
        cLib_addCalc2(&i_this->speedF, scaleMag, 1.0f, 5.0f);
        i_this->m1212 = i_this->m1212 + 1;
        if (daBk_player_way_check(i_this) && (i_this->m1212 & 0x30) && !r29) {
            if (i_this->m120C != 0) {
                anm_init(i_this, dRes_INDEX_BK_BCK_BK_RUN_e, 10.0f, 2, l_bkHIO().m070, dRes_INDEX_BK_BAS_BK_RUN_e);
                i_this->m120C = 0;
            }
            if (i_this->mPlayerDistance < l_bkHIO().m02C) {
                i_this->dr.mMode = 2;
                i_this->m0300[4] = 50;
            }
            break;
        } else {
            if (i_this->m120C == 0 && i_this->m0B30 != 0) {
                i_this->m120C = 1;
                i_this->m1210 = 0;
            }
        }

        if (i_this->m0B30 != 0) {
            i_this->speedF = l_bkHIO().m068;

            switch (i_this->m1210) {
            case 0:
                i_this->m1210 = 1;
                cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.m4D0, 2, 0x2000);
                anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP2_e, 5.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_JUMP2_e);
                break;
            case 1:
                if (i_this->dr.mAcch.ChkGroundHit() && i_this->mpMorf->isStop()) {
                    anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP2_e, 0.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_JUMP2_e);
                    i_this->m1210 = 2;
                    i_this->m034C = l_bkHIO().m00C + 2;
                    i_this->m034E = 4;
                }
                break;
            case 2:
                i_this->speedF = 0.0f;
                cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.m4D0, 2, 0x2000);
                if (i_this->dr.mAcch.ChkGroundHit() && i_this->mpMorf->isStop()) {
                    i_this->m1210 = 1;
                    anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_JUMP1_e);
                    i_this->speed.y = cM_rndF(REG8_F(7) + 10.0f) + 65.0f + REG8_F(8);
                    if ((i_this->m02DD & 0xC) == 0) {
                        s16 temp = (s16)gabi::ftoi(cM_rndFX(REG6_F(13) + 3000.0f));
                        i_this->current.angle.y = i_this->current.angle.y + temp;
                    }
                    fopAcM_monsSeStart(i_this, JA_SE_CV_BK_JUMP, 0);
                }
                break;
            }
        }

        if (i_this->mPlayerDistance < l_bkHIO().m030) {
            i_this->dr.mAction = 5;
            i_this->dr.mMode = 0;
            return;
        }
        break;
    }
    case 2:
        i_this->m120C = 0;

        if (cM_rndF(1.0f) < 0.3f && i_this->m0B30 == 0) {
            i_this->dr.mMode = 8;
            wait_set(i_this);
            i_this->m0300[1] = (s16)gabi::ftoi(cM_rndF(20.0f) + 20.0f);
            break;
        }
        /* HD: the m0B30/m11F3 branches play the same animation (merged) */
        if ((i_this->m02DD & 0xC) == 0 && std::fabs(stickPosX) > 0.1f) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_WALK2_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_WALK2_e);
            if (stickPosX > 0.0f) {
                i_this->dr.mMode = 5;
            } else {
                i_this->dr.mMode = 6;
            }
        } else if (i_this->mPlayerDistance < l_bkHIO().m030) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_WALK2_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_WALK2_e);
            i_this->dr.mMode = 4;
        } else {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_WALK2_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_WALK2_e);
            i_this->dr.mMode = 3;
        }
        i_this->m0300[1] = (s16)gabi::ftoi(cM_rndF(20.0f) + 20.0f);
        break;
    case 3:
        cLib_addCalc2(&i_this->speedF, l_bkHIO().m060, 1.0f, 20.0f);
        if (r29) {
            i_this->speed.y = REG0_F_(16) + 100.0f;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_JUMP1_e);
            fopAcM_monsSeStart(i_this, JA_SE_CV_BK_JUMP, 0);
            i_this->dr.mMode = 33;
            break;
        }
        if (i_this->m0300[1] == 0) {
            i_this->dr.mMode = 2;
        }
        break;
    case 33:
        if (i_this->dr.mAcch.ChkGroundHit()) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP2_e, 0.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_JUMP2_e);
            i_this->dr.mMode = i_this->dr.mMode + 1;
        }
        break;
    case 34:
        i_this->speedF = 0.0f;
        if (i_this->mpMorf->isStop()) {
            i_this->dr.mMode = 3;
        }
        break;
    case 4:
        if ((i_this->m02DD & 0x2) == 0) {
            cLib_addCalc2(&i_this->speedF, -l_bkHIO().m060, 1.0f, 20.0f);
            if (i_this->m0300[1] == 0) {
                i_this->dr.mMode = 2;
            }
            break;
        }
        i_this->dr.mMode = 3;
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_WALK2_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_WALK2_e);
        break;
    case 5:
        if ((i_this->m02DD & 0x4) == 0) {
            i_this->m0334 = 0x4000;
        } else {
            i_this->m0300[1] = 0;
        }
        goto temp_860;
    case 6:
        if ((i_this->m02DD & 0x8) == 0) {
            i_this->m0334 = -0x4000;
        } else {
            i_this->m0300[1] = 0;
        }
    temp_860:
        cLib_addCalc2(&i_this->speedF, l_bkHIO().m064, 1.0f, 30.0f);
        if (i_this->m0300[1] == 0) {
            i_this->dr.mMode = 2;
        }
        break;
    case 8:
        i_this->speedF = 0.0f;
        if (i_this->m0300[1] == 0) {
            i_this->dr.mMode = 2;
        }
        break;
    }

    if (i_this->dr.mMode >= 3 && i_this->m0314 <= 2) {
        if (i_this->mPlayerDistance > l_bkHIO().m02C + 75.0f) {
            i_this->dr.mAction = 0;
            i_this->dr.mMode = 0;
            path_check(i_this, 0);
        }
        f32 dist = i_this->mPlayerDistance;
        f32 m030 = l_bkHIO().m030;
        if (dist < m030 + 62.5f && dist > m030 - 62.5f) {
            if (i_this->m0300[4] == 0) {
                i_this->m0300[4] = l_bkHIO().m078;
                if (cM_rndF(100.0f) < l_bkHIO().m07C) {
                    i_this->dr.mAction = 5;
                    i_this->dr.mMode = 0;
                }
            }
        }
        if (i_this->m0310 == 0) {
            bool r27 = false;
            learn_check() = i_this->m1208;
            fopAc_ac_c* shot = (fopAc_ac_c*)fpcM_Search(0x0209BA18 /* shot_s_sub */, i_this);
            if (shot) {
                if (shot->speedF > 10.0f) {
                    gabi::Local<cXyz> tmp;
                    gabi::Local<cXyz> sp18;
                    cXyz_mi(&shot->current.pos, tmp, &i_this->eyePos);
                    sp18->copy(*tmp);
                    if (cXyz_abs(sp18) < shot->speedF * 10.0f) {
                        r27 = true;
                    }
                }
            }
            dAttention_c* attention = gabi::at<dAttention_c>(dComIfGp_ea() + PLAY_ATTENTION);
            if (i_this->m0B30 &&
                daBk_player_way_check(i_this) &&
                (
                    r27 || (
                        daPy_getCutType(player) != 0 &&
                        (cc_pl_cut_bit_get() & i_this->m1208) &&
                        dAttention_Lockon(attention) &&
                        (fopAc_ac_c*)i_this == dAttention_LockonTarget(attention, 0)
                    )
                )
            ) {
                /* GHS: `<= 0.5f` branches on `> 0.5f` */
                if (i_this->m02D4 != 0 && (!(cM_rndF(1.0f) > 0.5f) || l_bkHIO().m008 != 0)) {
                    i_this->dr.mAction = 10;
                    i_this->dr.mMode = 0;
                    if (daPy_getCutType(player) == daPy_CUT_TYPE_JUMPCUT_SWORD) {
                        i_this->m0300[1] = 0x1E;
                    } else {
                        i_this->m0300[1] = 0x0F;
                    }
                } else {
                    i_this->m030E = 0xA;
                    if ((ground_4_check(i_this, 4, i_this->current.angle.y, 200.0f) & 0xD) == 0) {
                        i_this->dr.mAction = 5;
                        attack_set(i_this, 1);
                        anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_JUMP1_e);
                        i_this->dr.mMode = -10;
                        i_this->speedF = REG0_F_(3) + -60.0f;
                        i_this->speed.y = REG0_F_(4) + 80.0f;
                        fopAcM_monsSeStart(i_this, JA_SE_CV_BK_JUMP, 0);
                    }
                }
            }
        }
    }

    if (i_this->m0B30 != 0 && i_this->mPlayerDistance < l_bkHIO().m030 - 62.5f &&
        daBk_player_view_check(i_this, &i_this->dr.m714->current.pos, i_this->m0332, l_bkHIO().m034)
    ) {
        i_this->m02FC = i_this->m02FC + 1;
        if (i_this->m02FC >= (s16)(0x19 + REG0_S(0))) {
            if (cM_rndF(1.0f) < REG0_F_(0) + 0.5f &&
                (ground_4_check(i_this, 4, i_this->current.angle.y, 200.0f) & 0xD) == 0
            ) {
                i_this->dr.mAction = 7;
                i_this->dr.mMode = 0;
            }
            i_this->m02FC = 0;
        }
    } else {
        i_this->m02FC = 0;
    }
    if (daBk_player_bg_check(i_this, &i_this->dr.m714->current.pos)) {
        i_this->dr.mAction = 0;
        i_this->dr.mMode = 0;
        path_check(i_this, 0);
    }
    if (i_this->m0B30 == 0 && daBk_wepon_view_check(i_this)) {
        i_this->dr.mAction = 12;
        i_this->dr.mMode = -1;
    }
    if (daBk_bomb_view_check(i_this)) {
        i_this->dr.mAction = 9;
        i_this->dr.mMode = 0;
    }
    i_this->m02DD = ground_4_check(i_this, 4, i_this->current.angle.y, REG6_F(7) + 90.0f);
    if (i_this->m0314 != 0) {
        if (std::fabs((f32)i_this->speedF) < 30.0f) {
            if (i_this->m0318 == 0) {
                i_this->dr.m710 = 3;
            } else if (i_this->m0318 == 1) {
                i_this->dr.m710 = 4;
            } else if (i_this->m02F8 & 0x10) {
                i_this->dr.m710 = 3;
            } else {
                i_this->dr.m710 = 4;
            }
            cLib_addCalcAngleS2(&i_this->m11F4, 12000, 2, 0x1800);
        } else {
            i_this->dr.m710 = 1;
        }
    } else {
        i_this->dr.m710 = 1;
        if (i_this->m0316 == 0) {
            i_this->m0316 = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
            if (i_this->m02DD == 4) {
                i_this->m0318 = 0;
                i_this->m0314 = 0x10;
            } else if (i_this->m02DD == 8) {
                i_this->m0318 = 1;
                i_this->m0314 = 0x10;
            } else if (i_this->m02DD == 2) {
                i_this->m0318 = 2;
                i_this->m0314 = 0x20;
            }
        }
    }
}
VERIFY(0x020A0A98, fight_run);

/* yari_hit_check (inline): the spear's attack sphere */
static inline fopAc_ac_c* yari_hit_check(bk_class* i_this) {
    i_this->m11F0 = 0;
    i_this->m11C0.copy(i_this->m11A8);
    if (i_this->m0B7A < 0) { return nullptr; }

    if (i_this->m0B5C == 2) {
        i_this->m11A8.copy(i_this->dr.m100[0xE]);
    } else {
        i_this->m11A8.copy(i_this->m1178);
    }

    if (i_this->m0B78 != 0) { return nullptr; }
    if (i_this->m0B64 < i_this->m0B68 || i_this->m0B64 > i_this->m0B6C) { return nullptr; }

    i_this->m11F0 = (u8)(i_this->m11F0 << 1);
    i_this->m1040.SetAtSpl(bk_at_kind(i_this->m0B5C));
    if (gabi::ftoi(i_this->m0B64) == gabi::ftoi(i_this->m0B68)) {
        s32 se = bk_attack_go_SE(i_this->m0B5C);
        if (se != -0xDCF) {
            fopAcM_monsSeStart(i_this, se, 0);
        }
    }

    if (i_this->m11F1 == 0) {
        /* HD: starts the At sphere at eyePos (GameCube: m11A8) */
        dCcD_Sph_StartCAt(&i_this->m1040, &i_this->eyePos);
        i_this->m11F1 = i_this->m11F1 + 1;
    } else {
        dCcD_Sph_MoveCAt(&i_this->m1040, &i_this->m11A8);
        dComIfG_Ccsp_Set(&i_this->m1040);
        if (i_this->m02D5 != 0) {
            dCcMassS_Mng_Set(gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS), &i_this->m1040, 3);
        }
        if (i_this->m1040.ChkAtHit()) {
            i_this->m0B78 = 5;
            void* hitObj = dCcD_GObjInf_GetAtHitObj(&i_this->m1040);
            return hitObj ? cCcD_Obj_GetAc(hitObj) : nullptr;
        }
    }
    return nullptr;
}

/* 020A1AAC */
void fight(bk_class* i_this) {
    WWHD_FUNC(0x020A1AAC, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    switch ((s16)i_this->dr.mMode) {
    case -10:
        if (i_this->dr.mAcch.ChkGroundHit()) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP2_e, 0.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_JUMP2_e);
            i_this->dr.mMode = -9;
            i_this->speedF = 0.0f;
        }
        break;
    case -9:
        if (i_this->mpMorf->isStop()) {
            i_this->dr.mMode = 1;
            i_this->m0300[2] = 8;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JATTACK1_e, 2.0f, 0, 1.0f, -1);
            i_this->speedF = REG0_F_(5) + 80.0f;
            i_this->speed.y = REG0_F_(6) + 115.0f;
            goto temp_1B8;
        }
        break;
    case 0:
        if (i_this->m0B30 != 0) {
            attack_set(i_this, 0);
        } else {
            attack_set(i_this, 2);
            i_this->speedF = REG0_F_(7) + 70.0f;
            i_this->speed.y = REG0_F_(8) + 105.0f;
            fopAcM_monsSeStart(i_this, JA_SE_CV_BK_ATTACK_L, 0);
        }
        i_this->dr.mMode = 1;
        i_this->m0300[2] = 8;
        /* fall-through */
    temp_1B8:
    case 1: {
        i_this->m1040.SetAtAtp(bk_attack_AP(i_this->m0B5C));
        f32 f1 = attack_info(i_this->m0B5C)[i_this->m0B60].speed;
        if (i_this->dr.mAcch.ChkGroundHit() || (i_this->m0B5C != 1 && i_this->m0B5C != 2)) {
            i_this->m0B64 = i_this->m0B64 + f1;
            cLib_addCalc2(&i_this->speedF, 0.0f, 1.0f, 20.0f);
        }
        if (i_this->m0B64 > i_this->m0B70 &&
            daBk_player_view_check(i_this, &i_this->dr.m714->current.pos, i_this->m0332, l_bkHIO().m034)) {
            i_this->dr.m710 = 1;
        }

        if (i_this->m0B7A > 0) {
            bkHIO_c& hio = l_bkHIO();
            if (i_this->m0B5C == 0) {
                int r3 = hio.m0A0 + hio.m0A2 + hio.m0A4 + hio.m0A6;
                if (gabi::ftoi(i_this->m0B64) >= hio.m0A0 && gabi::ftoi(i_this->m0B64) <= r3) {
                    i_this->m02F0 = 1;
                    if (gabi::ftoi(i_this->m0B64) >= (hio.m0A0 + hio.m0A2) &&
                        gabi::ftoi(i_this->m0B64) < (hio.m0A0 + hio.m0A2 + hio.m0A4)) {
                        i_this->m02F4 = 1;
                    }
                    int r0 = gabi::ftoi(i_this->m0B64) - hio.m0A0;
                    if (r0 < 10) {
                        i_this->m02EC = hio.m0A8[r0];
                    }
                }
            } else if (i_this->m0B5C == 1) {
                int r3 = hio.m0D0 + hio.m0D2 + hio.m0D4 + hio.m0D6;
                if (gabi::ftoi(i_this->m0B64) >= hio.m0D0 && gabi::ftoi(i_this->m0B64) <= r3) {
                    i_this->m02F0 = 1;
                    if (gabi::ftoi(i_this->m0B64) >= (hio.m0D0 + hio.m0D2) &&
                        gabi::ftoi(i_this->m0B64) < (hio.m0D0 + hio.m0D2 + hio.m0D4)) {
                        i_this->m02F4 = 1;
                    }
                    int r0 = gabi::ftoi(i_this->m0B64) - hio.m0D0;
                    if (r0 < 10) {
                        i_this->m02EC = hio.m0D8[r0];
                    }
                }
            }
        }

        if ((i_this->m0B64 < i_this->m0B74) || i_this->m11F2 != 0) {
            i_this->dr.m4D0 = i_this->m0332;
        }

        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.m4D0, 4, 0x800);

        u8 attackType = 0;
        f32 startFrame = 1000.0f;
        f32 endFrame = 1000.0f;
        if (i_this->m0B5C == 0) {
            startFrame = REG0_F_(8) + 12.0f;
            endFrame = REG0_F_(9) + 25.0f;
            attackType = 2;
        } else if (i_this->m0B5C == 1) {
            startFrame = REG0_F_(10);
            endFrame = REG0_F_(11) + 10.0f;
            attackType = 2;
        }
        /* setBtAttackData(startFrame, endFrame, 10000.0f, attackType) */
        i_this->mBtStartFrame = startFrame;
        i_this->mBtAttackType = attackType;
        i_this->mBtEndFrame = endFrame;
        i_this->mBtMaxDis = 10000.0f;
        i_this->mBtMaxDis = l_bkHIO().m014;
        i_this->mBtNowFrame = i_this->m0B64;

        dComIfGp_get(); /* HD: leftover call */
        fopAc_ac_c* hitActor = yari_hit_check(i_this);
        if (hitActor != nullptr) {
            if (fpcM_GetName(hitActor) == fpcNm_PLAYER_e) {
                if (daPy_checkPlayerGuard(player) && (i_this->m0B5C == 0 || i_this->m0B5C == 1)) {
                    i_this->mpMorf->setPlaySpeed(-1.0f);
                    if (i_this->m034C != 0) {
                        i_this->m034C = l_bkHIO().m00C + 6;
                    }
                    i_this->m0B7A = -1;
                    i_this->mpMorf->play(&i_this->eyePos, 0, 0);
                    if (i_this->m0B30 != 0 && i_this->m02D5 == 0 && cM_rndF(1.0f) < 0.5f) {
                        i_this->m0B34 = 1;
                        i_this->dr.mAction = 8;
                        i_this->dr.mMode = -10;
                        i_this->m0300[0] = 10;
                        i_this->m0300[1] = 100;
                    }
                }
            } else if (fpcM_GetName(hitActor) == fpcNm_BK_e) {
                i_this->m11FC = fopAcM_GetID(hitActor);
            }
        } else {
            i_this->m1040.ClrAtHit();
            if (i_this->m11F0 != 0) {
                if (i_this->m11F0 == 2) {
                    i_this->mpMorf->setPlaySpeed(-1.0f);
                    i_this->m02F0 = 0;
                    if (i_this->m034C != 0) {
                        i_this->m034C = l_bkHIO().m00C + 6;
                    }
                    i_this->m0B7A = -1;
                    i_this->mpMorf->play(&i_this->eyePos, 0, 0);
                } else {
                    i_this->dr.mAction = 0;
                    path_check(i_this, 0);
                    wait_set(i_this);
                    i_this->dr.mMode = 2;
                }
                i_this->m0318 = 1;
                i_this->m0314 = 0x10;
                gabi::Local<cXyz> sp18;
                sp18->x = 1.0f;
                sp18->y = 1.0f;
                sp18->z = 1.0f;
                dComIfGp_particle_set(0xC /* dPa_name::ID_AK_JN_NG */, &i_this->m11E4, nullptr, sp18);
            }
        }

        if (!i_this->mpMorf->isStop()) {
            break;
        }

        if (
            (i_this->m0B60 == 2 && i_this->m0B7A > 0) ||
            (i_this->m0B7A < 0 && i_this->m0B60 == 0) ||
            (i_this->m0B5C == 2 && i_this->m0B60 == 1)
        ) {
            if (i_this->m11FC != fpcM_ERROR_PROCESS_ID_e) {
                i_this->dr.mAction = 14;
                i_this->dr.mMode = 0;
            } else if (i_this->mPlayerDistance < l_bkHIO().m02C) {
                if (daBk_player_view_check(i_this, &i_this->dr.m714->current.pos, i_this->m0332, l_bkHIO().m034)) {
                    if (cM_rndF(1.0f) < 0.8f || i_this->m0B7A < 0) {
                        i_this->m0300[1] = 0;
                        i_this->dr.mAction = 4;
                        i_this->dr.mMode = 2;
                    } else {
                        i_this->dr.mMode = 0;
                    }
                } else {
                    if (i_this->m0B30 != 0 || i_this->m11F3 != 0) {
                        i_this->dr.mAction = 8;
                        i_this->dr.mMode = 0;
                    } else {
                        i_this->dr.mAction = 0;
                        wait_set(i_this);
                        i_this->dr.mMode = 2;
                    }
                }
            } else {
                i_this->dr.mAction = 0;
                path_check(i_this, 0);
                wait_set(i_this);
                i_this->dr.mMode = 2;
            }
        } else if (i_this->dr.mAcch.ChkGroundHit()) {
            attack_info_s* r6 = attack_info(i_this->m0B5C);
            f32 speed;
            if (i_this->m0B7A > 0) {
                i_this->m0B60 = i_this->m0B60 + 1;
                speed = r6[i_this->m0B60].speed;
            } else {
                i_this->m0B60 = i_this->m0B60 - 1;
                speed = -r6[i_this->m0B60].speed;
            }
            anm_init(i_this, r6[i_this->m0B60].bckFileIdx, 0.0f, 0, speed, r6[i_this->m0B60].soundFileIdx);
        }
        break;
    }
    }
}
VERIFY(0x020A1AAC, fight);

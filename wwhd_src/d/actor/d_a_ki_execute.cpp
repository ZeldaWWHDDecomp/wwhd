/**
 * d_a_ki_execute.cpp (WWHD)
 * Enemy - Keese: daKi_Execute with its inlined move functions (ki_wait_move, ki_fly_move,
 * ki_fire_set_move, ki_damage_move + wall_angle_get, ki_fail_move, ki_path_move, ki_eye_tex_anm)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ki.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_ki.h"

/* functions of d_a_ki.cpp / d_a_ki_exec.cpp (guest calls by address) */
static inline void ki_check(ki_class* i_this) { gabi::call(0x02199364, i_this); }
static inline void anm_init(ki_class* i_this, int anmResIdx, float morf, unsigned char loopMode, float playSpeed, int soundResIdx) {
    gabi::call(0x02199388, i_this, anmResIdx, morf, loopMode, playSpeed, soundResIdx);
}
static inline void tex_anm_set(ki_class* i_this, unsigned short idx) { gabi::call(0x021994B0, i_this, idx); }
static inline BOOL ki_player_bg_check(ki_class* i_this) { return gabi::call<BOOL>(0x021995BC, i_this); }
static inline void ki_pos_move(ki_class* i_this, s8 arg1) { gabi::call(0x02199A64, i_this, arg1); }
static inline void ki_atack_move(ki_class* i_this) { gabi::call(0x02199C48, i_this); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 020402C8 enemy_ice(enemyice*) / 02041570 enemy_fire(enemyfire*) / 02041C30 enemy_fire_remove */
static inline BOOL enemy_ice(enemyice_l* e) { return gabi::call<BOOL>(0x020402C8, e); }
static inline void enemy_fire(enemyfire_l* f) { gabi::call(0x02041570, f); }
static inline void enemy_fire_remove_x(enemyfire_l* f) { gabi::call(0x02041C30, f); }
/* 027EC9E8 JUTReport(int x, int y, const char* fmt, ...) */
static inline void JUTReport(s32 x, s32 y, const char* fmt, s32 v) { gabi::call(0x027EC9E8, x, y, fmt, v); }
/* 025192A8 cc_at_check(fopAc_ac_c*, CcAtInfo*) */
static inline fopAc_ac_c* cc_at_check(fopAc_ac_c* a, void* info) { return gabi::call<fopAc_ac_c*>(0x025192A8, a, info); }
/* dComIfGp_particle_forceDeleteEmitter: HD: the play object is fetched (unused); the emitter manager
 * is a global pointer (0x1047B2D4); 0282215C JPAEmitterManager::forceDeleteEmitter(emtr, 0) */
static inline void dComIfGp_particle_forceDeleteEmitter(u32 emtr) {
    dComIfGp_get();
    gabi::call(0x0282215C, gabi::load<u32>(0x1047B2D4), emtr, 0);
}
/* save info (HD, through the pointer at 0x101F84DC): life u16 +0x22, max magic u8 +0x33, magic
 * u8 +0x34, arrows u8 +0x89 */
static inline u16 dComIfGs_getLife() { return gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x22); }
static inline u8 dComIfGs_getMaxMagic() { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x33); }
static inline u8 dComIfGs_getMagic() { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x34); }
static inline u8 dComIfGs_getArrowNum() { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x89); }
static inline void fopAcM_onActor(fopAc_ac_c* a) { dComIfGs_onActor(a->setID, a->home.roomNo); }
#define REG8_F(i) REG_F(8, i)

/* CcAtInfo (stack object passed to cc_at_check) */
struct CcAtInfo_l {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ gptr<fopAc_ac_c> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ gptr<cXyz> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(CcAtInfo_l, 0x1C);

/* HD: strcmp(dComIfGp_getStartStageName(), "GanonK") == 0 as sead::SafeString operator== */
static bool ki_stage_is(u32 name) {
    gabi::Local<SafeString> a;
    a->mStringTop = name;
    a->__vtbl = 0x10012B5C;
    gabi::Local<SafeString> b;
    {
        u32 stage = dComIfGp_ea() + 0x5134;
        b->__vtbl = 0x10012B5C;
        b->mStringTop = stage;
    }
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 bv = b->__vtbl;
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(bv + 0x14), b.get());
    u32 s2 = b->mStringTop;
    if (s1 == s2) return true;
    for (u32 n = 0x40001; n != 0; n--) {
        u8 c1 = gabi::load<u8>(s1);
        u8 c2 = gabi::load<u8>(s2);
        if (c1 != c2) return false;
        if (c1 == 0) return true;
        s1++;
        s2++;
    }
    return false;
}

enum {
    JA_SE_CV_KI_WAKEUP = 0x481C,
    JA_SE_CV_KI_DAMAGE = 0x481E,
    JA_SE_OBJ_TORCH_BURNING = 0x6103,
};
enum {
    dPa_ID_IT_SN_O_FIREK_KASU = 0x8061,
    dPa_ID_IT_SN_FIREK_FIRE_A = 0x8099,
    dPa_ID_IT_SN_FIREK_FIRE_B = 0x809A,
    dPa_ID_IT_SN_FIREK_HAHEN = 0x809B,
};
enum {
    daDisItem_IBALL_e = 0,
    daDisItem_HEART_e = 0xA,
    daDisItem_MAGIC_e = 0xB,
    daDisItem_ARROW_e = 0xC,
    daDisItem_NONE13_e = 0xD,
};

/* point of an HD dPath: points at +8, 0x10 bytes each (arg3 at +3, position at +4) */
static inline u32 ki_path_point(ki_class* i_this) {
    return gabi::load<u32>(gabi::ea(i_this->ppd.get()) + 8) + (s32)i_this->m2D6 * 0x10;
}

static void ki_wait_move(ki_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* a_this = &i_this->actor;
    gabi::Local<cXyz> sp58;

    switch (i_this->mBehaviorType) {
    case 0:
        anm_init(i_this, dRes_INDEX_KI_BCK_WAIT1_e, 10.0f, 2, 1.0f, dRes_INDEX_KI_BAS_WAIT1_e);
        i_this->mBehaviorType = 1;
        tex_anm_set(i_this, 2);
        [[fallthrough]];

    case 1: {
        gabi::Local<cXyz> tmp;
        cXyz_mi(&player->current.pos, tmp, &a_this->current.pos);
        f32 x = tmp->x, z = tmp->z;
        if (std_sqrtf(gabi::fmadds(x, x, z * z)) < i_this->mMaxAttackMoveDist300) {
            gabi::Local<cXyz> tmp2;
            cXyz_mi(&player->current.pos, tmp2, &player->old.pos);
            sp58->copy(*tmp2);
            if (std_sqrtf(PSVECSquareMag(sp58)) > 1.0f) {
                i_this->mBehaviorType = 2;
                i_this->mTimers[0] = 10;
            }
        }
        break;
    }

    case 2:
        if (i_this->mTimers[0] == 0) {
            i_this->mBehaviorType = 3;
            tex_anm_set(i_this, 0);
            i_this->mTimers[0] = 0x28;
            fopAcM_monsSeStart(a_this, JA_SE_CV_KI_WAKEUP, 0);
        }
        break;

    case 3: {
        i_this->m338 = 1;
        if (i_this->mTimers[0] == 0) {
            gabi::Local<cXyz> tmp;
            cXyz_mi(&player->current.pos, tmp, &player->old.pos);
            sp58->copy(*tmp);
            f32 d = std_sqrtf(PSVECSquareMag(sp58));
            i_this->mBehaviorType = 0;
            if (d > 1.0f) {
                i_this->mAction = ki_class::ACT_ATTACK_MOVE_INDEX_e;
                a_this->current.angle.x = 0x4000;
            }
        }

        gabi::Local<cXyz> tmp;
        cXyz_mi(&player->current.pos, tmp, &a_this->current.pos);
        f32 x = tmp->x, y = tmp->y, z = tmp->z;
        i_this->m322 = cM_atan2s(x, z) - a_this->current.angle.y;
        i_this->m324 = -cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        if (i_this->m322 > 10000) {
            i_this->m322 = 10000;
        } else if (i_this->m322 < -10000) {
            i_this->m322 = -10000;
        }

        if (i_this->m324 > 0x2000) {
            i_this->m324 = 0x1FF6;
        } else if (i_this->m324 < -0x38E) {
            i_this->m324 = -0x38E;
        }
        break;
    }

    case 10:
        i_this->mPosMove.copy(a_this->home.pos);
        i_this->mPosMoveTarget = 20.0f;
        i_this->mPosMoveMaxSpeed = 20.0f;
        i_this->m304 = 10000.0f;
        i_this->mPosMoveDist = 1.0f;
        ki_pos_move(i_this, 0);

        {
            gabi::Local<cXyz> tmp;
            cXyz_mi(&i_this->mPosMove, tmp, &a_this->current.pos);
            sp58->copy(*tmp);
        }
        if (std_sqrtf(PSVECSquareMag(sp58)) < 100.0f) {
            i_this->mBehaviorType = 0xB;
            i_this->mTimers[0] = 0x32;
            anm_init(i_this, dRes_INDEX_KI_BCK_WAIT1_e, 10.0f, 2, 1.0f, dRes_INDEX_KI_BAS_WAIT1_e);
        }
        break;

    case 0xB:
        cLib_addCalc2(&a_this->current.pos.x, a_this->home.pos.x, 0.5f, std::fabs((f32)a_this->speed.x) + 1.0f);
        cLib_addCalc2(&a_this->current.pos.y, a_this->home.pos.y, 0.5f, std::fabs((f32)a_this->speed.y) + 1.0f);
        cLib_addCalc2(&a_this->current.pos.z, a_this->home.pos.z, 0.5f, std::fabs((f32)a_this->speed.z) + 1.0f);
        if (i_this->mTimers[0] == 0) {
            i_this->mBehaviorType = 0;
            i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(50.0f) + 70.0f);
        }
        break;
    }

    cLib_addCalcAngleS2(&a_this->shape_angle.y, a_this->current.angle.y, 1, 0x1000);
}

static void ki_fly_move(ki_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* a_this = &i_this->actor;
    kiHIO_c& hio = l_kiHIO();

    i_this->m540 = 1;

    gabi::Local<cXyz> sp28;
    cXyz_mi(&i_this->mPosMove, sp28, &a_this->current.pos);
    f32 sx = sp28->x, sz = sp28->z;
    f32 sqrt = std_sqrtf(gabi::fmadds(sx, sx, sz * sz));

    switch (i_this->mBehaviorType) {
    case 0:
        anm_init(i_this, dRes_INDEX_KI_BCK_FLY1_e, 10.0f, 2, hio.m08, dRes_INDEX_KI_BAS_FLY1_e);
        i_this->mBehaviorType = 1;
        tex_anm_set(i_this, 0);
        i_this->mPosMoveTarget = hio.m28;
        i_this->mPosMoveMaxSpeed = 2.0f;
        i_this->m304 = 3000.0f;
        [[fallthrough]];

    case 1:
        if (i_this->mTimers[1] == 0 || sqrt < 50.0f) {
            i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
            /* HD: home.pos.x is kept in a register across the random call */
            f32 hx = a_this->home.pos.x;
            i_this->mPosMove.x = hx;
            i_this->mPosMove.y = a_this->home.pos.y;
            i_this->mPosMove.z = a_this->home.pos.z;
            f32 r = cM_rndFX(hio.m0C);
            i_this->mPosMove.x = hx + r;
            i_this->mPosMove.y -= cM_rndF(200.0f);
            i_this->mPosMove.z += cM_rndFX(hio.m0C);
            i_this->mPosMoveDist = 0.0f;
        }

        if (i_this->mTimers[0] == 0) {
            gabi::Local<cXyz> tmp;
            cXyz_mi(&player->current.pos, tmp, &a_this->current.pos);
            f32 x = tmp->x, z = tmp->z;
            if (std_sqrtf(gabi::fmadds(x, x, z * z)) < i_this->mMaxAttackMoveDist300) {
                i_this->mAction = ki_class::ACT_ATTACK_MOVE_INDEX_e;
                i_this->mBehaviorType = 0;
            }
        }
        break;
    }

    if (i_this->mTimers[2] == 0 && i_this->mAcch.ChkWallHit()) {
        a_this->current.angle.y -= -0x8000;
        i_this->mTimers[2] = 20;
    }

    ki_pos_move(i_this, 0);
    cLib_addCalcAngleS2(&a_this->shape_angle.y, a_this->current.angle.y, 1, 0x2000);
}

static void ki_fire_set_move(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    dComIfGp_get(); /* player (unused) */

    i_this->m540 = 0;

    switch (i_this->mBehaviorType) {
    case 0: {
        f32 base = REG8_F(8) + 35.0f;
        a_this->speedF = base + cM_rndF(10.0f);
        f32 base2 = REG8_F(9) + 97.0f;
        a_this->speed.y = base2 + cM_rndF(30.0f);
        anm_init(i_this, dRes_INDEX_KI_BCK_WAIT1_e, 1.0f, 2, 1.0f, dRes_INDEX_KI_BAS_WAIT1_e);
        i_this->mBehaviorType = 1;
        i_this->m91C = dComIfGp_particle_set(dPa_ID_IT_SN_FIREK_FIRE_A, &a_this->current.pos, nullptr, nullptr, 0xFF,
                                             (dPa_levelEcallBack*)&i_this->m908);
    }
        [[fallthrough]];

    case 1:
        a_this->shape_angle.x += 0x1400;
        a_this->shape_angle.y += 0x1000;
        if (a_this->speed.y < REG8_F(11) + -70.0f) {
            i_this->mAction = ki_class::ACT_ATTACK_MOVE_INDEX_e;
            i_this->mBehaviorType = 0;
            a_this->shape_angle.x = 0;
            if (i_this->m91C != nullptr) {
                gabi::call_ptr(gabi::load<u32>(i_this->m908.__vtbl + 0x44), &i_this->m908); /* m908.remove() */
                dComIfGp_particle_forceDeleteEmitter(gabi::ea(i_this->m91C.get()));
                i_this->m91C = nullptr;
            }
            dComIfGp_particle_set(dPa_ID_IT_SN_FIREK_FIRE_B, &a_this->current.pos);
            dComIfGp_particle_set(dPa_ID_IT_SN_FIREK_HAHEN, &a_this->current.pos);
        }
        break;

    case 10:
        anm_init(i_this, dRes_INDEX_KI_BCK_FLY1_e, 10.0f, 2, l_kiHIO().m08, dRes_INDEX_KI_BAS_FLY1_e);
        i_this->mBehaviorType = 0xB;
        i_this->mTimers[1] = 100;
        [[fallthrough]];

    case 11:
        i_this->m540 = 1;
        a_this->speed.y = 20.0f;
        if (i_this->mTimers[1] == 0) {
            i_this->m2DC = 1;
        }
        break;
    }

    gabi::Local<cXyz> sp24;
    gabi::Local<cXyz> sp18;
    sp24->x = 0.0f;
    sp24->y = 0.0f;
    sp24->z = a_this->speedF;
    cMtx_YrotS(calc_mtx(), a_this->current.angle.y);
    MtxPosition(sp24, sp18);

    a_this->current.pos.x += sp18->x;
    a_this->current.pos.y += a_this->speed.y;
    a_this->current.pos.z += sp18->z;
    a_this->speed.y -= REG8_F(10) + 8.0f;

    if (fopAcM_searchPlayerDistance(a_this) > 2000.0f && ki_player_bg_check(i_this)) {
        i_this->m2DC = 1;
    }
}

/* wall_angle_get (inlined in ki_damage_move) */
static inline s16 wall_angle_get(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    gabi::Local<dBgS_LinChk_l> linChk;
    ki_LinChk_ct(linChk);
    gabi::Local<cXyz> sp2C0;
    gabi::Local<cXyz> sp2C1;
    cXyz* sp2C[2] = {sp2C0.get(), sp2C1.get()};
    gabi::Local<cXyz> sp20;
    gabi::Local<cXyz> sp14;

    cMtx_YrotS(calc_mtx(), a_this->shape_angle.y);
    sp20->x = 0.0f;
    sp20->y = 0.0f;
    sp20->z = 100.0f;
    MtxPosition(sp20, sp14);
    PSVECAdd(sp14, &a_this->current.pos, sp14);
    sp20->x = 10.0f;
    sp20->y = 0.0f;
    sp20->z = -300.0f;

    for (s32 i = 0; i < 2; i++) {
        MtxPosition(sp20, sp2C[i]);
        sp20->x = -sp20->x;
        PSVECAdd(sp2C[i], sp14, sp2C[i]);

        dBgS_LinChk_Set(linChk, sp14, sp2C[i], a_this);

        if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
            sp2C[i]->copy(*gabi::at<cXyz>(gabi::ea(linChk.get()) + 0x30)); /* linChk.GetCross() */
        } else {
            ki_LinChk_dt(linChk);
            return TRUE;
        }
    }

    gabi::Local<cXyz> tmp;
    cXyz_mi(sp2C[1], tmp, sp2C[0]);
    f32 x = tmp->x, z = tmp->z;
    s16 ret = cM_atan2s(x, z) + 0x4000;
    ki_LinChk_dt(linChk);
    return ret;
}

static void ki_damage_move(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    dComIfGp_get(); /* player (unused) */
    bool bVar1 = false;

    i_this->m540 = 1;
    i_this->m314 = 3;

    switch (i_this->mBehaviorType) {
    case 0:
        anm_init(i_this, dRes_INDEX_KI_BCK_DAMAGE1_e, 2.0f, 0, 1.0f, dRes_INDEX_KI_BAS_DAMAGE1_e);
        tex_anm_set(i_this, 3);
        i_this->mBehaviorType = 1;
        [[fallthrough]];

    case 1:
        if (i_this->m31C > 0.1f) {
            cLib_addCalcAngleS2(&a_this->shape_angle.y, i_this->m316, 1, 0x3000);
            gabi::Local<cXyz> sp1C;
            sp1C->x = 0.0f;
            sp1C->y = 0.0f;
            sp1C->z = -i_this->m31C;
            cMtx_YrotS(calc_mtx(), i_this->m316);
            cMtx_XrotM(calc_mtx(), i_this->mRand2000);

            if (i_this->mAcch.ChkWallHit()) {
                sp1C->z = 0.0f;
                if (!(i_this->m31C < l_kiHIO().m3C)) { /* m31C >= m3C (blt: NaN enters) */
                    i_this->mBehaviorType = 2;
                    i_this->mTimers[0] = 0x32;
                    anm_init(i_this, dRes_INDEX_KI_BCK_BITA1_e, 1.0f, 0, 1.0f, dRes_INDEX_KI_BAS_BITA1_e);
                    tex_anm_set(i_this, 3);
                    a_this->speed.y = 0.0f;
                    /* csXyz shapeAngle = shape_angle; shapeAngle.y -= -0x8000;  (unused) */
                    s16 sVar2 = wall_angle_get(i_this);
                    if (sVar2 != 1) {
                        a_this->current.angle.y = sVar2;
                    }
                    i_this->m904 = (u8)gabi::ftoi(cM_rndF(1.9f));
                    bVar1 = true;
                }
            }

            gabi::Local<cXyz> sp10;
            MtxPosition(sp1C, sp10);
            a_this->current.pos.x += sp10->x;
            a_this->current.pos.y += sp10->y;
            a_this->current.pos.z += sp10->z;
            cLib_addCalc0(&i_this->m31C, 1.0f, l_kiHIO().m40);
            a_this->speedF = 0.0f;
        } else {
            i_this->mAction = ki_class::ACT_ATTACK_MOVE_INDEX_e;
            i_this->mBehaviorType = 0;
            bVar1 = true;
        }
        break;

    case 2:
        cLib_addCalcAngleS2(&a_this->shape_angle.y, a_this->current.angle.y, 1, 0x4000);
        if (i_this->mTimers[0] < 30) {
            a_this->current.pos.y += a_this->speed.y;
            a_this->speed.y -= 0.1f;
            if (i_this->m904 == 0) {
                a_this->shape_angle.z += -0x250;
            } else {
                a_this->shape_angle.z += 0x250;
            }
        }

        if (i_this->mTimers[0] == 0) {
            i_this->mAction = ki_class::ACT_ATTACK_MOVE_INDEX_e;
            i_this->mBehaviorType = 0;
        }
        break;
    }

    if (bVar1 && a_this->health <= 0) {
        i_this->mAction = ki_class::ACT_FAIL_MOVE_e;
        i_this->mBehaviorType = 0;
    }
}

static void ki_fail_move(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    u8 dropType;

    if (ki_stage_is(0x10012B54 /* "GanonK" */)) {
        if (a_this->health != -10) {
            if (dComIfGs_getLife() <= 8) {
                dropType = daDisItem_HEART_e;
            } else if (cM_rndF(1.0f) < 0.5f) {
                if (dComIfGs_getArrowNum() == 0) {
                    dropType = daDisItem_ARROW_e;
                } else if (dComIfGs_getMagic() < dComIfGs_getMaxMagic() / 2) {
                    dropType = daDisItem_MAGIC_e;
                } else if (dComIfGs_getArrowNum() < 10) {
                    dropType = daDisItem_ARROW_e;
                } else {
                    /* static u8 item_tbl[] = {HEART, MAGIC, ARROW, HEART} (0x101B8164) */
                    dropType = gabi::load<u8>(0x101B8164 + gabi::ftoi(cM_rndF(2.99f)));
                }
            } else {
                dropType = daDisItem_NONE13_e;
            }
        } else {
            dropType = daDisItem_NONE13_e;
        }
        fopAcM_createDisappear(a_this, &a_this->current.pos, 5, dropType, 0xFF);
    } else {
        fopAcM_createDisappear(a_this, &a_this->current.pos, 5, daDisItem_IBALL_e, 0xFF);
    }
    fopAcM_delete(a_this);
    fopAcM_onActor(a_this);
}

static void ki_path_move(ki_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    dComIfGp_get(); /* player (unused) */
    kiHIO_c& hio = l_kiHIO();

    switch (i_this->mBehaviorType) {
    case 0:
        i_this->m2D6 += i_this->m2D7;
        /* HD dPath: m_num u16 at +0 (compared as s8), m_nextID u16 at +2, closed flag at +5 bit 0 */
        if (i_this->m2D6 >= gabi::load<s8>(gabi::ea(i_this->ppd.get()) + 1)) {
            u32 ppd = gabi::ea(i_this->ppd.get());
            if (gabi::load<u8>(ppd + 5) & 1) {
                i_this->m2D6 = 0;
            } else {
                i_this->m2D7 = -1;
                i_this->m2D6 = gabi::load<u16>(gabi::ea(i_this->ppd.get()) + 0) - 2;
            }

            if ((s32)gabi::load<u16>(ppd + 2) != 0xFFFF) {
                i_this->ppd = dPath_GetRoomPath(gabi::load<u16>(ppd + 2), fopAcM_GetRoomNo(a_this));
                if (i_this->ppd == nullptr) /* JUT_ASSERT(1779, i_this->ppd != NULL) */
                    JUT_ASSERT_fail(STR(0x10012C34), 0x6F3, STR(0x10012C40));
            }
        } else if (i_this->m2D6 < 0) {
            i_this->m2D7 = 1;
            i_this->m2D6 = 1;
        }
        [[fallthrough]];

    case -1: {
        i_this->mBehaviorType = 1;
        u32 point = ki_path_point(i_this);
        i_this->mPosMoveTarget = cM_rndF(5.0f) + 30.0f;
        i_this->mPosMoveMaxSpeed = REG0_F(13) + 1.0f;
        i_this->mPosMoveDist = REG0_F(7);
        i_this->mPosMove.x = gabi::load<f32>(point + 4);
        i_this->mPosMove.y = gabi::load<f32>(point + 8);
        i_this->mPosMove.z = gabi::load<f32>(point + 0xC);
        f32 r = cM_rndFX(150.0f);
        i_this->mPosMove.x = gabi::load<f32>(point + 4) + r;
        r = cM_rndFX(150.0f);
        i_this->mPosMove.y = gabi::load<f32>(point + 8) + r;
        r = cM_rndFX(150.0f);
        i_this->mPosMove.z = gabi::load<f32>(point + 0xC) + r;
        break;
    }
    case 1: {
        f32 x = i_this->mPosMove.x - a_this->current.pos.x;
        f32 y = i_this->mPosMove.y - a_this->current.pos.y;
        f32 z = i_this->mPosMove.z - a_this->current.pos.z;

        if (std_sqrtf(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y))) < gabi::fmadds(REG0_F(10), 10.0f, 200.0f)) {
            i_this->mBehaviorType = 0;
            if (gabi::load<u8>(ki_path_point(i_this) + 3) == 6) {
                fopAcM_delete(a_this);
            }
        }
        break;
    }
    }

    i_this->mPosMoveTarget = hio.m28;
    i_this->mPosMoveMaxSpeed = 2.0f;
    i_this->m304 = 4000.0f;
    i_this->mPosMoveDist = 1.0f;

    ki_pos_move(i_this, 0);

    cLib_addCalcAngleS2(&a_this->shape_angle.y, a_this->current.angle.y, 1, 0x2000);
    if (fopAcM_searchPlayerDistance(a_this) < i_this->mMaxAttackMoveDist300) {
        i_this->mCurrKiPathIndex = 0;
        i_this->mAction = ki_class::ACT_ATTACK_MOVE_INDEX_e;
        i_this->mBehaviorType = 0;
    }
}

static inline void ki_eye_tex_anm(ki_class* i_this) {
    if (i_this->m335 != 0) {
        if (i_this->m334 < i_this->m336) {
            i_this->m334 = i_this->m334 + 1;
        } else if (i_this->m337 != 0) {
            i_this->m334 = 0;
        } else {
            i_this->m335 = 0;
        }
    }
}

/* 0219AAE8 */
static BOOL daKi_Execute(ki_class* i_this) {
    WWHD_FUNC(0x0219AAE8, BOOL, i_this);
    fopAc_ac_c* a_this = &i_this->actor;
    dComIfGp_get(); /* player (unused) */
    kiHIO_c& hio = l_kiHIO();

    a_this->model = 0;

    if (enemy_ice(&i_this->mEnemyIce)) {
        i_this->mpMorf->setPlayMode(J3DFrameCtrl::EMode_NONE);
        i_this->mpMorf->setPlaySpeed(3.0f);
        i_this->mpMorf->play(&a_this->eyePos, 0, 0);
        J3DModel* model = i_this->mpMorf->getModel();
        J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
        i_this->mpMorf->calc();
        return TRUE;
    }

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &a_this->current.pos, &a_this->tevStr);
    if (i_this->m2DC != 0) {
        cLib_addCalc0(&a_this->scale.x, 1.0f, 0.05f);
        f32 s = a_this->scale.x;
        a_this->scale.z = s;
        a_this->scale.y = s;
        if (!(s > 0.04f)) {
            fopAcM_delete(a_this);
            return TRUE;
        }
    }

    if (i_this->m2D4 != 0) {
        if (dComIfGs_isSwitch(i_this->m2D4 - 1, fopAcM_GetRoomNo(a_this))) {
            i_this->m2D4 = 0;
            a_this->speedF = hio.m28;
            gabi::store<u32>(gabi::ea(a_this) + 0x39C, 4); /* attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e */
        } else {
            /* HD: no fopAcM_OnStatus(0) / OffStatus(0) */
            gabi::store<u32>(gabi::ea(a_this) + 0x39C, 0);
            return TRUE;
        }
    }

    a_this->actor_status |= 0x20; /* fopAcM_OnStatus(fopAcStts_SHOWMAP_e) */

    if (hio.m07 != 0) {
        ki_check(i_this);
        JUTReport(0x104, 0x186, STR(0x10012D2C), ki_all_count());
        JUTReport(0x104, 0x19A, STR(0x10012D38), ki_all_count() - ki_fight_count());
        JUTReport(0x104, 0x1AE, STR(0x10012D44), ki_fight_count());
    }

    i_this->m580.SetR(0.0f);
    i_this->mDamageSphere.SetR(40.0f);
    i_this->m6AC.SetR(hio.m58);

    if (hio.m06 == 0) {
        i_this->m540 = 0;

        for (s32 i = 0; i < 4; i++) {
            if (i_this->mTimers[i] != 0) {
                i_this->mTimers[i] -= 1;
            }
        }

        if (i_this->m314 != 0) {
            i_this->m314 -= 1;
        }

        i_this->m320 += 1;

        if (i_this->m338 == 0) {
            i_this->mpMorf->play(&a_this->current.pos, 0, 0);
        } else {
            i_this->m338 = 0;
        }

        if (i_this->mAction == ki_class::ACT_ATTACK_MOVE_e) {
            ki_atack_move(i_this);
        } else if (i_this->mCurrKiPathIndex != 0) {
            ki_path_move(i_this);
        } else if (i_this->mAction == ki_class::ACT_WAIT_MOVE_e) {
            ki_wait_move(i_this);
        } else if (i_this->mAction == ki_class::ACT_FIRE_SET_MOVE_e) {
            ki_fire_set_move(i_this);
        } else if (i_this->mAction == ki_class::ACT_FLY_MOVE_e) {
            ki_fly_move(i_this);
        } else if (i_this->mAction == ki_class::ACT_ATTACK_MOVE_INDEX_e) {
            ki_atack_move(i_this);
        } else if (i_this->mAction == ki_class::ACT_DAMAGE_MOVE_e) {
            ki_damage_move(i_this);
        } else if (i_this->mAction == ki_class::ACT_FAIL_MOVE_e) {
            ki_fail_move(i_this);
        }
        ki_eye_tex_anm(i_this);
    }

    if (i_this->mDamageType != 0) {
        mDoExt_baseAnm_play(i_this->m920);
        if (i_this->mAction != ki_class::ACT_FAIL_MOVE_e) {
            dComIfGp_particle_setSimple(dPa_ID_IT_SN_O_FIREK_KASU, &a_this->current.pos);
            fopAcM_seStart(a_this, JA_SE_OBJ_TORCH_BURNING, 0);
        }
    }

    cXyz* ccMoveP = &i_this->mStts.m_cc_move; /* mStts.GetCCMoveP() (GHS null-checks the address) */
    if (gabi::ea(i_this) + 0x6F8 != 0) {
        /* operand order as the recompiled original computes it (matters only for NaN payloads,
         * which an integer copy to eyePos makes observable) */
        a_this->current.pos.x = ccMoveP->x + a_this->current.pos.x;
        a_this->current.pos.y = a_this->current.pos.y + ccMoveP->y;
        a_this->current.pos.z = ccMoveP->z + a_this->current.pos.z;
    }

    MtxTrans(a_this->current.pos.x, a_this->current.pos.y, a_this->current.pos.z, false);
    cMtx_YrotM(calc_mtx(), a_this->shape_angle.y);
    cMtx_XrotM(calc_mtx(), a_this->shape_angle.x);
    cMtx_ZrotM(calc_mtx(), a_this->shape_angle.z);

    J3DModel* model = i_this->mpMorf->getModel();
    J3DModel_setBaseScale(model, &a_this->scale);
    J3DModel_setBaseTRMtx(model, calc_mtx());
    i_this->mpMorf->calc();
    enemy_fire(&i_this->mEnemyFire);

    a_this->eyePos.copy(a_this->current.pos);
    cXyz* attnPos = gabi::at<cXyz>(gabi::ea(a_this) + 0x390); /* attention_info.position */
    attnPos->x = a_this->eyePos.x;
    attnPos->y = a_this->eyePos.y;
    attnPos->z = a_this->eyePos.z;
    attnPos->y += 50.0f;

    i_this->m580.mSph.SetC(&a_this->current.pos);
    i_this->m6AC.mSph.SetC(&a_this->current.pos);
    i_this->mDamageSphere.mSph.SetC(&a_this->current.pos);

    dComIfG_Ccsp_Set(&i_this->m580);
    dComIfG_Ccsp_Set(&i_this->m6AC);
    dComIfG_Ccsp_Set(&i_this->mDamageSphere);

    if (i_this->m540 != 0) {
        a_this->current.pos.y -= REG0_F(5) + 30.0f;
        a_this->old.pos.y -= REG0_F(5) + 30.0f;
        i_this->mAcch.CrrPos(dComIfG_Bgsp());
        a_this->current.pos.y += REG0_F(5) + 30.0f;
        a_this->old.pos.y += REG0_F(5) + 30.0f;
    }

    i_this->mStts.Move();
    if (i_this->mDamageSphere.ChkTgHit() && i_this->m314 == 0) {
        i_this->m314 = 10;

        gabi::Local<CcAtInfo_l> atInfo;
        void* obj = i_this->mDamageSphere.GetTgHitObj();
        atInfo->mpObj = gabi::ea(obj);
        atInfo->pParticlePos = i_this->mDamageSphere.GetTgHitPosP();

        if (cCcD_Obj_ChkAtType(obj, 0x00100000 /* LIGHT_ARROW */ | 0x00080000 /* ICE_ARROW */)) {
            if (cCcD_Obj_ChkAtType(obj, 0x00080000)) {
                i_this->mEnemyIce.mFreezeDuration = REG0_S(3) + 300;
                i_this->mAction = ki_class::ACT_ATTACK_MOVE_INDEX_e;
                i_this->mBehaviorType = 0;
            } else {
                i_this->mEnemyIce.mLightShrinkTimer = 1;
            }
            enemy_fire_remove_x(&i_this->mEnemyFire);
            return TRUE;
        }

        if (cCcD_Obj_ChkAtType(obj, 0x00040000 /* FIRE_ARROW */ | 0x200 /* FIRE */)) {
            i_this->mEnemyFire.mFireDuration = REG0_S(2) + 100;
            i_this->m314 = 0x32;
            i_this->m580.OnAtSPrmBit(2 /* cCcD_AtSPrm_VsEnemy_e */);
        }

        if (cCcD_Obj_ChkAtType(obj, 0x00200000 /* WIND */)) {
            i_this->m31C = 100.0f;
        } else {
            atInfo->mpActor = cc_at_check(a_this, atInfo.get());

            fopAc_ac_c* ac = atInfo->mpActor;
            if (ac != nullptr) {
                if (fpcM_GetName(ac) == 0xF3 /* BGN */ || fpcM_GetName(ac) == 0xF4 /* BGN2 */ ||
                    fpcM_GetName(ac) == 0xF5 /* BGN3 */) {
                    a_this->health = -10;
                }
            }

            if (hio.m05 != 0) {
                a_this->health = 10;
            }

            if (atInfo->mResultingAttackType == 9) {
                i_this->m31C = 150.0f;
            } else if (i_this->mStts.mAtSpl == 1 /* dCcG_At_Spl_UNK1 */) {
                i_this->m31C = hio.m38;
            } else {
                i_this->m31C = hio.m34;
            }
        }

        i_this->m316 = fopAcM_searchPlayerAngleY(a_this);
        i_this->mRand2000 = (s16)gabi::ftoi(cM_rndF(2000.0f));
        i_this->mAction = ki_class::ACT_DAMAGE_MOVE_e;
        i_this->mBehaviorType = 0;

        fopAcM_monsSeStart(a_this, JA_SE_CV_KI_DAMAGE, 0);
    }

    s16 tmp;
    if (i_this->m322 != 0) {
        tmp = 0x1000;
    } else {
        tmp = 0x200;
    }
    cLib_addCalcAngleS2(&i_this->m326, i_this->m322, 5, tmp);

    if (i_this->m324 != 0) {
        tmp = 0x1000;
    } else {
        tmp = 0x200;
    }
    cLib_addCalcAngleS2(&i_this->m328, i_this->m324, 5, tmp);

    i_this->m324 = 0;
    i_this->m322 = 0;
    cLib_addCalcAngleS2(&a_this->shape_angle.z, 0, 1, 0x200);
    return TRUE;
}
VERIFY(0x0219AAE8, daKi_Execute);

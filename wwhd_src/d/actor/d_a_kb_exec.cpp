/**
 * d_a_kb_exec.cpp (WWHD)
 * NPC - Pig: daKb_Execute with pl_attack_hit_check, normal_move, carry_move and swim_move inlined.
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_kb.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 * GHS inlined pl_attack_hit_check, normal_move, carry_move and swim_move into daKb_Execute
 * (02190CE0, 10.9 KB); they are written as inline helpers with the GameCube names.
 */
#include "d/actor/d_a_kb.h"

/* calls to this unit's out-of-line functions (by address) */
static inline void anm_init_(kb_class* i_this, int bck, f32 morf, u8 loop, f32 speed, int bas) {
    gabi::call(0x0218EB64, i_this, bck, morf, loop, speed, bas);
}
static inline BOOL carry_check_(kb_class* i_this) { return gabi::call<BOOL>(0x0218EF4C, i_this); }
static inline void sibuki_set_(kb_class* i_this) { gabi::call(0x0218F1FC, i_this); }
static inline BOOL swim_mode_change_check_(kb_class* i_this) { return gabi::call<BOOL>(0x0218F380, i_this); }
static inline void he_set_(kb_class* i_this) { gabi::call(0x0218F4EC, i_this); }
static inline void smoke_set_(kb_class* i_this) { gabi::call(0x0218F600, i_this); }
static inline void draw_SUB_(kb_class* i_this) { gabi::call(0x0218FABC, i_this); }
static inline void target_set_(kb_class* i_this, u8 p) { gabi::call(0x02190400, i_this, p); }
static inline BOOL esa_demo_check_(kb_class* i_this) { return gabi::call<BOOL>(0x02190708, i_this); }
static inline void BG_check_(kb_class* i_this) { gabi::call(0x02190BFC, i_this); }
static inline void kb_eye_tex_anm_(kb_class* i_this) { gabi::call(0x0219463C, i_this); }
static inline void cLib_offBit_u32_(be<u32>* v, u32 bit) { gabi::call(0x02194644, v, bit); }
static inline void speed_pos_set_(kb_class* i_this) { gabi::call(0x02194654, i_this); }
static inline f32 JMASinShort_(u32 table, s16 a) { return gabi::call<f32>(0x02194798, table, a); }
static inline void attack_move_(kb_class* i_this) { gabi::call(0x021947AC, i_this); }
static inline void esa_demo_move_(kb_class* i_this) { gabi::call(0x0219509C, i_this); }

static inline void particle_tuba(kb_class* i_this) {
    /* dComIfGp_particle_set(ID_AK_JN_TUBA00, &current.pos, &shape_angle) */
    dPa_control_set(dComIfGp_getParticle(), 0, 0xE, &i_this->current.pos, &i_this->shape_angle, nullptr, 0xFF, nullptr, -1,
                    nullptr, nullptr, nullptr);
}
static inline void copy_angle(csXyz* dst, csXyz* src) {
    dst->x = (s16)src->x;
    dst->y = (s16)src->y;
    dst->z = (s16)src->z;
}
static inline bool checkPass(kb_class* i_this, f32 f) { return i_this->mpMorf->checkFrame(f) != 0; }
static inline void start_shock(kb_class*) {
    gabi::Local<cXyz> dir;
    dir->set(0.0f, 1.0f, 0.0f);
    dComIfGp_getVibration_StartShock(1, -0x21, dir);
}

/* 00001268 (GameCube) */
static inline void pl_attack_hit_check(kb_class* i_this) {
    if (i_this->mSph.ChkTgHit()) {
        if (i_this->m403 == 0) {
            i_this->m403 = 1;
            particle_tuba(i_this);
            for (int i = 0; i < 2; i++) {
                dPa_followEcallBack_end(&i_this->m5A8[i]);
            }
            i_this->m4DC.copy(*i_this->mSph.GetTgHitPosP());
            if (i_this->m420 != 5) {
                i_this->m420 = 4;
            }
            gabi::Local<CcAtInfo_l> atInfo;
            atInfo->pParticlePos = 0;
            atInfo->mpObj = gabi::ea(i_this->mSph.GetTgHitObj());
            cc_at_check(i_this, atInfo);
            if (dComIfGs_getSelectEquip0() == 0xFF /* dItemNo_NONE_e */) {
                i_this->health = 10;
            }
            if (i_this->health <= 0) {
                i_this->health = 10;
                ALL_ANGER() = 1;
                attn_flags(i_this) |= fopAc_Attn_LOCKON_BATTLE_e;
                i_this->m420 = 0x21;
                i_this->m41E = 3;
            }
            kb_catch_se(i_this);
        }
    } else {
        i_this->m403 = 0;
        if (ALL_ANGER()) {
            if (i_this->m50C != dRes_INDEX_KB_BCK_RUN1_e) {
                anm_init_(i_this, dRes_INDEX_KB_BCK_RUN1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 2.0f, dRes_INDEX_KB_BAS_RUN1_e);
            }
            attn_flags(i_this) |= fopAc_Attn_LOCKON_BATTLE_e;
            attn_flags(i_this) &= ~(u32)fopAc_Attn_ACTION_CARRY_e;
            i_this->m4C4 = 30.0f;
            i_this->m41E = 3;
            i_this->m420 = 0x23;
        }
    }
}

static inline bool kb_player_near(kb_class* i_this) { return fopAcM_searchPlayerDistance(i_this) < 200.0f; }
static inline bool kb_player_crawl() { return (gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0) & daPyStts0_CRAWL_e) != 0; }

/* 00002834 (GameCube) */
static inline void normal_move(kb_class* i_this) {
    if (swim_mode_change_check_(i_this)) {
        return;
    }
    f32 diffX = i_this->m45C.x - i_this->current.pos.x;
    f32 diffZ = i_this->m45C.z - i_this->current.pos.z;
    std_sqrtf(gabi::fmadds(diffZ, diffZ, diffX + diffX)); /* unused (GameCube: temp1) */
    s16 ang = cM_atan2s(diffX, diffZ);
    s16 mode = i_this->m420;
    i_this->m422 = ang;

    switch (mode) {
    case 0:
        i_this->m420 = mode + 1;
        i_this->m426[0] = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
        i_this->shape_angle.z = 0;
        if (i_this->m50C != dRes_INDEX_KB_BCK_WAIT1_e) {
            anm_init_(i_this, dRes_INDEX_KB_BCK_WAIT1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KB_BAS_WAIT1_e);
        }
        i_this->m4B4 = 0.0f;
        i_this->m4B8 = 2.5f;
        /* fallthrough */
    case 1:
        if (esa_demo_check_(i_this)) {
            return;
        }
        if (i_this->m426[0] == 0) {
            i_this->m420 = 2;
        }
        /* HD: GameCube m408 == 0 became mbCanBeBigPig != 4 */
        if (kb_player_near(i_this) && i_this->mbCanBeBigPig != 4) {
            if (kb_player_crawl()) {
                i_this->m426[1] = (s16)gabi::ftoi(cM_rndF(60.0f) + 60.0f);
            } else if (i_this->m426[1] == 0 && i_this->m405 == 0) {
                i_this->m420 = 4;
            }
        }
        break;
    case 2:
        i_this->m420 = mode + 1;
        i_this->m426[0] = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
        i_this->m4B4 = 3.0f;
        if (i_this->m50C != dRes_INDEX_KB_BCK_WALK1_e) {
            anm_init_(i_this, dRes_INDEX_KB_BCK_WALK1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KB_BAS_WALK1_e);
        }
        i_this->m4B8 = 2.0f;
        target_set_(i_this, 0);
        i_this->m424 = 0x800;
        /* fallthrough */
    case 3: {
        if (esa_demo_check_(i_this)) {
            return;
        }
        if (i_this->m426[0] == 0) {
            i_this->m420 = 0;
            break;
        }
        if (kb_player_near(i_this)) {
            if (i_this->mbCanBeBigPig == 4) {
                break;
            }
            if (kb_player_crawl()) {
                i_this->m426[1] = (s16)gabi::ftoi(cM_rndF(30.0f) + 30.0f);
                break;
            } else if (i_this->m426[1] == 0 && i_this->m405 == 0) {
                i_this->m420 = 4;
                break;
            }
        }
        if (i_this->m4CC != 0.0f) {
            if (i_this->m450.x != 0.0f || i_this->m450.y != 0.0f || i_this->m450.z != 0.0f) {
                f32 dx = i_this->m450.x - i_this->current.pos.x;
                f32 dz = i_this->m450.z - i_this->current.pos.z;
                f32 d = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
                f32 half = i_this->m4CC * 0.5f;
                if (d > half) {
                    s16 a = cM_atan2s(dx, dz);
                    i_this->m422 = a;
                    cMtx_YrotS(calc_mtx(), a);
                    gabi::Local<cXyz> temp;
                    temp->set(0.0f, 0.0f, 1000.0f);
                    MtxPosition(temp, &i_this->m45C);
                    PSVECAdd(&i_this->m45C, &i_this->current.pos, &i_this->m45C);
                }
            }
        }
        break;
    }
    case 4:
        if (i_this->mbCanBeBigPig != 2 && i_this->mbCanBeBigPig != 3 && i_this->mbCanBeBigPig != 4) {
            i_this->m4CC = 0.0f;
        }
        i_this->m420 += 1;
        i_this->m4B4 = 12.0f;
        if (i_this->mShapeType >= 8) {
            i_this->m4B4 = 6.0f; /* 12 * 0.5 */
        }
        if (i_this->m50C != dRes_INDEX_KB_BCK_RUN1_e) {
            anm_init_(i_this, dRes_INDEX_KB_BCK_RUN1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.5f, dRes_INDEX_KB_BAS_RUN1_e);
        }
        i_this->m4B8 = 2.0f;
        i_this->m426[0] = 0;
        i_this->m424 = 0x2000;
        /* fallthrough */
    case 5:
        if (i_this->m426[0] == 0) {
            target_set_(i_this, 1);
            i_this->m426[0] = (s16)gabi::ftoi(cM_rndF(10.0f) + 10.0f);
        }
        if (checkPass(i_this, 0.0f) || checkPass(i_this, 5.0f)) {
            if (i_this->mShapeType >= 8 && checkPass(i_this, 0.0f)) {
                start_shock(i_this);
                kb_mons_se_start(i_this, JA_SE_CM_PG_L_JUMP);
            }
            if (i_this->m426[2] == 0) {
                if (i_this->mShapeType >= 8) {
                    kb_mons_se_start(i_this, JA_SE_CV_PG_L_NORMAL);
                } else {
                    kb_mons_se_start(i_this, JA_SE_CV_PG_NORMAL);
                }
            }
        }
        if (fopAcM_searchPlayerDistance(i_this) > 400.0f) {
            i_this->m420 = 0;
        }
        break;
    case 6:
        if (i_this->mpMorf->isStop()) {
            i_this->m420 = 2;
            i_this->m4C4 = 30.0f;
            i_this->m426[1] = (s16)gabi::ftoi(cM_rndF(30.0f) + 30.0f);
        }
        break;
    }

    if (i_this->m420 > 1 && i_this->m420 != 6) {
        if (i_this->m40B == 0) {
            cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->m422, 4, i_this->m424);
        }
        if (i_this->m420 == 5) {
            if (i_this->m426[2] == 0 && (s16)cLib_distanceAngleS(i_this->current.angle.y, i_this->m422) > 0x4000) {
                if (i_this->mShapeType >= 8) {
                    kb_mons_se_start(i_this, JA_SE_CV_PG_L_TURN);
                } else {
                    kb_mons_se_start(i_this, JA_SE_CV_PG_TURN);
                }
                i_this->m426[2] = 0x18;
            }
            if (i_this->m40B == 0) {
                if (std::fabs(i_this->speedF - i_this->m4B4) < 0.2f) {
                    s16 z = (s16)gabi::ftoi((f32)(i_this->current.angle.y - i_this->m422) * 0.5f);
                    if (z < 0) {
                        if ((int)(u16)z < (u16)-0x2000) {
                            z = -0x2000;
                        }
                    } else if (z > 0x2000) {
                        z = 0x2000;
                    }
                    i_this->shape_angle.z = z;
                } else {
                    i_this->shape_angle.z = 0;
                }
            }
        }
    }

    cLib_addCalc2(&i_this->speedF, i_this->m4B4, 1.0f, i_this->m4B8);
    attn_flags(i_this) |= fopAc_Attn_ACTION_CARRY_e;
    carry_check_(i_this);
    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 2, 0x1000);
}

/* 00003120 (GameCube) */
static inline void carry_move(kb_class* i_this) {
    fopAc_ac_c* pPlayer = dComIfGp_getPlayer(0);

    switch (i_this->m420) {
    case 0xA:
        i_this->mSph.OffCoSPrmBit(1); /* OffCoSetBit */
        i_this->shape_angle.z = 0;
        cLib_addCalcAngleS2(&i_this->m4F4.x, 0, 2, 0x400);
        cLib_addCalcAngleS2(&i_this->m4F4.z, 0, 2, 0x400);
        if (i_this->m4BC != i_this->current.pos.y) {
            i_this->m438 += 1;
            if (i_this->m438 == 4) {
                particle_tuba(i_this);
            }
            cLib_addCalcAngleS2(&i_this->shape_angle.x, -0x7FFF, 1, 0x1000);
            i_this->m4C4 = 20.0f;
        }
        if (!(std::fabs(i_this->current.pos.y - pPlayer->current.pos.y) < 110.0f)) {
            i_this->shape_angle.x = -0x7FFF;
            if (i_this->mbCanBeBigPig == 3) {
                he_set_(i_this);
            }
            i_this->m426[2] = 0;
            i_this->m4C8 = 1.8f;
            particle_tuba(i_this);
            anm_init_(i_this, dRes_INDEX_KB_BCK_JITA2_e, 5.0f, J3DFrameCtrl::EMode_NONE, i_this->m4C8, dRes_INDEX_KB_BAS_JITA2_e);
            kb_catch_se(i_this);
            i_this->mAcch.OnLineCheck();
            i_this->m44A = 3;
            i_this->m420 = 0xB;
        }
        i_this->current.angle.y = (s16)i_this->shape_angle.y;
        break;
    case 0xB:
        if (i_this->mbCanBeBigPig == 3) {
            he_set_(i_this);
        }
        if (i_this->m44A != 0) {
            if (i_this->mpMorf->isStop()) {
                i_this->m44A -= 1;
                if (i_this->m44A <= 0) {
                    anm_init_(i_this, dRes_INDEX_KB_BCK_JITA1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KB_BAS_JITA1_e);
                    i_this->m426[2] = (s16)gabi::ftoi(cM_rndF(300.0f) + 600.0f);
                    i_this->m44A = 0;
                } else {
                    f32 r = i_this->m4C8 - 0.6f;
                    if (r < 0.0f) {
                        r = 0.0f;
                    }
                    i_this->m4C8 = r;
                    if (i_this->m44A > 1) {
                        if (i_this->mShapeType >= 8) {
                            kb_mons_se_start_a(i_this, JA_SE_CV_PG_L_CATCH);
                        } else {
                            kb_mons_se_start_a(i_this, JA_SE_CV_PG_CATCH);
                        }
                    }
                    anm_init_(i_this, dRes_INDEX_KB_BCK_JITA2_e, 0.0f, J3DFrameCtrl::EMode_NONE, i_this->m4C8, dRes_INDEX_KB_BAS_JITA2_e);
                }
            }
        } else {
            if (i_this->m426[2] == 0) {
                i_this->m44A = 3;
                i_this->m4C8 = 1.8f;
                anm_init_(i_this, dRes_INDEX_KB_BCK_JITA2_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.8f, dRes_INDEX_KB_BAS_JITA2_e);
                if (i_this->mShapeType >= 8) {
                    kb_mons_se_start_a(i_this, JA_SE_CV_PG_L_CATCH);
                } else {
                    kb_mons_se_start_a(i_this, JA_SE_CV_PG_CATCH);
                }
                particle_tuba(i_this);
            } else if (gabi::load<u32>(gabi::ea(pPlayer) + 0x3C0) & 0xC00 /* getFootOnGround */) {
                kb_mons_se_start_a(i_this, JA_SE_CV_PG_CARRY);
            }
        }
        cLib_addCalcAngleS2(&i_this->shape_angle.x, -0x7FFF, 1, 0x1000);
        i_this->current.angle.y = (s16)i_this->shape_angle.y;
        break;
    case 0xC:
        if (i_this->mAcch.ChkWaterHit() && kb_wtr_height(i_this) > i_this->current.pos.y && i_this->m440 == 0) {
            sibuki_set_(i_this);
        }
        if (i_this->mAcch.ChkWallHit() && std::fabs(i_this->speedF) > 3.0f) {
            i_this->speedF = i_this->speedF * -0.5f;
        }
        if (i_this->m444 > 3) {
            return;
        }
        {
            s16 m436 = i_this->m436;
            if (!i_this->mAcch.ChkGroundHit()) {
                i_this->speedF = i_this->speedF * 0.99f;
            }
            if (m436 == 0) {
                i_this->shape_angle.x += 0x1000;
                cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 2, 0x800);
            } else {
                cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 1, 0x800);
            }
        }
        if ((i_this->m440 == 0 && !i_this->mAcch.ChkGroundHit()) || (i_this->m440 && kb_wtr_height(i_this) < i_this->current.pos.y)) {
            return;
        }
        if (i_this->mShapeType >= 8) {
            start_shock(i_this);
            kb_mons_se_start_a(i_this, JA_SE_CM_PG_L_JUMP);
            if (dComIfGp_isStartStage(0x100127DC /* "sea" */) && fopAcM_GetRoomNo(i_this) == dIsleRoom_OutsetIsland_e &&
                i_this->mAcch.GetGroundH() != -1000000000.0f && dBgS_GetSpecialCode(dComIfG_Bgsp(), kb_gnd_poly(i_this)) != 4 &&
                !dKy_daynight_check()) {
                dSv_event_onEventBit(dComIfGs_getEvent(), 0x3402 /* UNK_3402 */);
            }
        }
        if (i_this->m440 != 0) {
            if (i_this->mShapeType >= 8) {
                i_this->mStts.SetWeight(0x64);
            } else {
                i_this->mStts.SetWeight(0x32);
            }
            dCamera_ForceLockOff(i_this);
            i_this->m41E = 2;
            i_this->m420 = 0x14;
            return;
        }
        cLib_addCalc0(&i_this->speedF, 1.0f, 4.0f);
        particle_tuba(i_this);
        if (i_this->mShapeType >= 8) {
            kb_mons_se_start_a(i_this, JA_SE_CV_PG_L_CATCH);
        } else {
            kb_mons_se_start_a(i_this, JA_SE_CV_PG_CATCH);
        }
        i_this->m444 = 6;
        if (i_this->m436 == 0) {
            i_this->speed.y = 20.0f;
            i_this->m436 = 1;
            if (i_this->mbCanBeBigPig == 4) {
                i_this->speed.y = 0.0f;
                i_this->speedF = 0.0f;
                i_this->m444 = 0x1E;
            }
            dCamera_ForceLockOff(i_this);
            if (i_this->m50C != dRes_INDEX_KB_BCK_WAIT1_e) {
                anm_init_(i_this, dRes_INDEX_KB_BCK_WAIT1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KB_BAS_WAIT1_e);
            }
            i_this->mSph.OnCoSPrmBit(1); /* OnCoSetBit */
        } else {
            i_this->m420 = 0xD;
            if (i_this->mbCanBeBigPig == 2) {
                i_this->m426[5] = 0x1E;
            }
            if (i_this->m404 == 0) {
                i_this->m450.copy(i_this->current.pos);
            }
            attn_flags(i_this) |= fopAc_Attn_LOCKON_MISC_e;
            i_this->m426[0] = 0x10;
            i_this->m4B4 = 0.0f;
            i_this->speedF = 0.0f;
            i_this->speed.set(0.0f, 0.0f, 0.0f);
            if (dComIfGs_getSelectEquip0() != 0xFF) {
                i_this->health -= 1;
            }
        }
        break;
    case 0xD:
        cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 1, 0x2000);
        if (i_this->m426[0] == 0 && !fopAcM_checkCarryNow(i_this) && i_this->m444 == 0) {
            i_this->shape_angle.x = 0;
            i_this->mAcch.OffLineCheck();
            i_this->m4C4 = 30.0f;
            i_this->mSph.OnCoSPrmBit(1);
            anm_init_(i_this, dRes_INDEX_KB_BCK_NAKU1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_KB_BAS_NAKU1_e);
            i_this->m422 = (s16)i_this->current.angle.y;
            i_this->m4B4 = 0.0f;
            i_this->speedF = 0.0f;
            i_this->speed.set(0.0f, 0.0f, 0.0f);
            i_this->m426[2] = 0;
            i_this->m45C.copy(i_this->current.pos);
            i_this->m41E = 0;
            i_this->m420 = 6;
            if (i_this->health <= 0) {
                i_this->health = 10;
                ALL_ANGER() = 1;
                /* HD: no OnAtSetBit / OnAtHitBit (mode 0x24 sets the collider) */
                i_this->speedF = cM_rndFX(2.0f) + 15.0f;
                i_this->m426[0] = 0x14;
                i_this->current.angle.y = (s16)i_this->shape_angle.y;
                i_this->m442 = fopAcM_searchPlayerAngleY(i_this);
                i_this->m5D4[0].copy(i_this->current.pos);
                copy_angle(&i_this->m5EC[0], &i_this->current.angle);
                smoke_set_(i_this);
                i_this->m448 = 0;
                i_this->m420 = 0x24;
            }
            if (swim_mode_change_check_(i_this)) {
                return;
            }
        }
        break;
    }

    if ((i_this->m420 == 0xA || i_this->m420 == 0xB) && !fopAcM_checkCarryNow(i_this)) {
        i_this->gravity = -3.0f;
        if (!(i_this->speedF > 0.0f)) { /* GHS: <= is true for NaN */
            i_this->m4C4 = 30.0f;
            i_this->m426[0] = 0;
            if (i_this->mbCanBeBigPig == 2) {
                i_this->m426[5] = 0x1E;
            }
            if (i_this->mbCanBeBigPig == 3) {
                dPa_followEcallBack_end(&i_this->m594);
            }
            i_this->m420 = 0xD;
            attn_flags(i_this) |= fopAc_Attn_LOCKON_MISC_e;
        } else {
            i_this->m436 = 0;
            i_this->m4C4 = 30.0f;
            if (i_this->mbCanBeBigPig == 3) {
                dPa_followEcallBack_end(&i_this->m594);
            }
            anm_init_(i_this, dRes_INDEX_KB_BCK_JITA2_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KB_BAS_JITA2_e);
            i_this->shape_angle.x = -0x8000;
            i_this->speedF = 20.0f;
            i_this->speed.y = 20.0f;
            i_this->m420 = 0xC;
        }
    }
}

/* 00003DEC (GameCube) */
static inline void swim_move(kb_class* i_this) {
    gabi::Local<dBgS_LinChk> linChk;
    dBgS_LinChk_ct(linChk, LINCHK_VT, false);
    gabi::Local<csXyz> temp;
    copy_angle(temp, &i_this->shape_angle);
    int temp3 = 0;
    f32 temp2 = kb_wtr_height(i_this);

    switch (i_this->m420) {
    case 0x14:
        i_this->mSph.OffTgSPrmBit(1); /* OffTgSetBit */
        i_this->mSph.ClrTgHit();
        i_this->health = 10;
        i_this->m436 = 0;
        i_this->m438 = 0;
        i_this->m43A = 0;
        i_this->shape_angle.z = 0;
        attn_flags(i_this) &= ~(u32)fopAc_Attn_LOCKON_BATTLE_e;
        attn_flags(i_this) &= ~(u32)fopAc_Attn_LOCKON_MISC_e;
        i_this->mSph.OnCoSPrmBit(1);
        anm_init_(i_this, dRes_INDEX_KB_BCK_JITA1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KB_BAS_JITA1_e);
        i_this->speedF = 1.5f;
        i_this->speed.x = 0.0f;
        i_this->speed.z = 0.0f;
        i_this->shape_angle.z = 0;
        i_this->gravity = 0.0f;
        i_this->speed.y = -20.0f;
        i_this->m420 += 1;
        /* fallthrough */
    case 0x15:
        cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 1, 0x2000);
        cLib_addCalcAngleS2(&i_this->shape_angle.z, 0, 1, 0x2000);
        cLib_addCalc0(&i_this->speed.y, 1.0f, 2.0f);
        if (i_this->speed.y > -0.1f) {
            i_this->shape_angle.x = 0;
            i_this->shape_angle.z = 0;
            i_this->speed.y = 0.0f;
            i_this->m420 += 1;
        }
        break;
    case 0x16: {
        f32 t3 = gabi::fmadds(i_this->m4D4, REG_F(8, 6) + 10.0f, 10.0f);
        if (i_this->m426[6] == 0) {
            i_this->m426[6] = (s16)gabi::ftoi(cM_rndF(15.0f) + 15.0f);
            if (i_this->mShapeType >= 8) {
                kb_mons_se_start(i_this, JA_SE_CV_PG_L_NORMAL);
            } else {
                kb_mons_se_start(i_this, JA_SE_CV_PG_NORMAL);
            }
        }
        i_this->m436 += 0x800;
        f32 t4 = gabi::fmadds(cM_ssin(i_this->m436), REG_F(8, 7) + 5.0f, 29.0f);
        t4 = gabi::fmadds(i_this->m4D4, REG_F(8, 8) + 29.0f, t4);
        cLib_addCalc2(&i_this->current.pos.y, temp2 - t4, 1.0f, 3.0f);
        /* HD: also when the pig touches the ground */
        if (i_this->mAcch.ChkGroundHit() || std::fabs(kb_wtr_height(i_this) - i_this->mAcch.GetGroundH()) < t3) {
            i_this->m5D0 = 0.5f;
            if (i_this->m540.mpEmitter.get() != nullptr) {
                i_this->m540.mRate = 0.0f;
            }
            if (i_this->m50C != dRes_INDEX_KB_BCK_WALK1_e) {
                anm_init_(i_this, dRes_INDEX_KB_BCK_WALK1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KB_BAS_WALK1_e);
            }
            i_this->m420 += 1;
            i_this->gravity = -3.0f;
        }
        carry_check_(i_this);
        break;
    }
    case 0x17:
        if (swim_mode_change_check_(i_this)) {
            dBgS_LinChk_dt(linChk);
            return;
        }
        if (i_this->m440 == 0) {
            dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->m540);
            i_this->m440 = 0;
            i_this->m420 = 0x18;
        }
        if (i_this->m426[6] == 0) {
            i_this->m426[6] = (s16)gabi::ftoi(cM_rndF(15.0f) + 15.0f);
            if (i_this->mShapeType >= 8) {
                kb_mons_se_start(i_this, JA_SE_CV_PG_L_NORMAL);
            } else {
                kb_mons_se_start(i_this, JA_SE_CV_PG_NORMAL);
            }
        }
        carry_check_(i_this);
        break;
    case 0x18:
        anm_init_(i_this, dRes_INDEX_KB_BCK_DASSUI_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        i_this->m436 = 0;
        i_this->speedF = 0.0f;
        i_this->m420 += 1;
        break;
    case 0x19:
        if (checkPass(i_this, 10.0f) || checkPass(i_this, 11.0f)) {
            if (checkPass(i_this, 11.0f)) {
                temp->y += 0x8000;
                temp3 = 1;
            }
            if (i_this->m5A8[temp3].mpEmitter.get() == nullptr) {
                s8 roomNo = fopAcM_GetRoomNo(i_this);
                dPa_control_set(dComIfGp_getParticle(), 0, 0x80B6 /* ID_AK_SN_PIGDASSUI */, &i_this->current.pos, temp, nullptr,
                                0xFF, (dPa_levelEcallBack*)&i_this->m5A8[temp3], roomNo, nullptr, nullptr, nullptr);
            }
            JPABaseEmitter* pEmtr = i_this->m5A8[temp3].mpEmitter;
            if (pEmtr != nullptr && i_this->m4D4 != 0.0f) {
                set_global_scale(gabi::ea(i_this->m5A8[temp3].mpEmitter.get()), 2.0f, 2.0f, 2.0f);
            }
        }
        if (i_this->mpMorf->isStop() || carry_check_(i_this) == TRUE) {
            for (int i = 0; i < 2; i++) {
                dPa_followEcallBack_end(&i_this->m5A8[i]);
            }
            i_this->m436 = 0;
            i_this->m438 = 0;
            i_this->m440 = 0;
            if (i_this->m41E != 1) {
                i_this->mSph.OnTgSPrmBit(1);
                attn_flags(i_this) |= fopAc_Attn_LOCKON_MISC_e;
                if (i_this->m404 == 0) {
                    i_this->m450.copy(i_this->current.pos);
                }
                i_this->m41E = 0;
                i_this->m420 = 2;
                i_this->m426[1] = (s16)gabi::ftoi(cM_rndF(20.0f) + 20.0f);
                i_this->m426[3] = (s16)gabi::ftoi(cM_rndF(20.0f) + 20.0f);
            }
        }
        break;
    }

    copy_angle(&i_this->current.angle, &i_this->shape_angle);
    if (i_this->m420 >= 0x18) {
        dBgS_LinChk_dt(linChk);
        return;
    }
    if (i_this->speed.y == 0.0f) {
        cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 4, 0x800);
        if (i_this->m43A == 0) {
            target_set_(i_this, 2);
            i_this->m43A = 1;
            if (i_this->mAcch.ChkWallHit()) {
                gabi::Local<cXyz> t1;
                gabi::Local<cXyz> t2;
                gabi::Local<cXyz> t3;
                cBgS_GetTriPnt(dComIfG_Bgsp(), &i_this->mAcchCir, t1, t2, t3);
                s16 a = cM_atan2s(t1->x - i_this->current.pos.x, t1->z - i_this->current.pos.z);
                if ((s16)cLib_distanceAngleS(i_this->current.angle.y, a) < 0x2000) {
                    if (cM_rnd() < 0.5f) {
                        i_this->m422 = i_this->shape_angle.y + 0x4000;
                    } else {
                        i_this->m422 = i_this->shape_angle.y - 0x4000;
                    }
                    i_this->m43A = (s16)gabi::ftoi(cM_rndF(20.0f) + 5.0f);
                }
            }
        }
        cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->m422, 2, 0x200);
        if ((s16)cLib_distanceAngleS(i_this->shape_angle.y, i_this->m422) < 0x100 && i_this->m43A > 0) {
            i_this->m43A -= 1;
        }
    }
    dBgS_LinChk_dt(linChk);
}

/* fadds with the PowerPC NaN rule (frA's NaN wins) */
static inline f32 kb_fadds_ppc(f32 a, f32 b) {
    if (a != a) return a;
    if (b != b) return b;
    return a + b;
}
static inline f32 clamp10(f32 v) {
    if (v > 10.0f) {
        v = 10.0f;
    } else if (v < -10.0f) {
        v = -10.0f;
    }
    return v;
}

/* 02190CE0 */
static BOOL daKb_Execute(kb_class* i_this) {
    WWHD_FUNC(0x02190CE0, BOOL, i_this);
    fopAc_ac_c* actor = i_this;

    /* HD: the esa demo cool-down (m426[4]) does not run while an event runs (play+0x5292) */
    dComIfGp_get();
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0) {
        i_this->m426[4] += 1;
    }
    for (int i = 0; i < 8; i++) {
        if (i_this->m426[i] != 0) {
            i_this->m426[i] -= 1;
        }
    }
    i_this->m446 += 1;
    i_this->mSph.OffAtSPrmBit(1); /* HD: ClrAtSet every frame */

    switch (i_this->m41E) {
    case 0:
        pl_attack_hit_check(i_this);
        normal_move(i_this);
        if (i_this->m426[7] == 0 && i_this->mAcch.ChkGroundHit()) {
            if (i_this->mShapeType < 8 && dComIfGp_isStartStage(0x100129C4 /* "sea" */) &&
                fopAcM_GetRoomNo(actor) == dIsleRoom_OutsetIsland_e) {
                if (i_this->mAcch.GetGroundH() != -1000000000.0f && dBgS_GetSpecialCode(dComIfG_Bgsp(), kb_gnd_poly(i_this)) == 4) {
                    u8 temp = 1 << (i_this->mShapeType & 3);
                    u8 temp2 = dSv_event_getEventReg(dComIfGs_getEvent(), 0xBFFF);
                    dSv_event_setEventReg(dComIfGs_getEvent(), 0xBFFF, temp2 | temp);
                    i_this->m405 = temp;
                } else {
                    i_this->m405 = 0;
                }
            }
            if (i_this->mShapeType >= 8 && dComIfGp_isStartStage(0x100129C4) && fopAcM_GetRoomNo(actor) == dIsleRoom_OutsetIsland_e &&
                i_this->mAcch.GetGroundH() != -1000000000.0f && dBgS_GetSpecialCode(dComIfG_Bgsp(), kb_gnd_poly(i_this)) != 4) {
                if (i_this->mShapeType >= 8) {
                    i_this->mStts.SetWeight(100);
                } else {
                    i_this->mStts.SetWeight(0x32);
                }
                dSv_event_onEventBit(dComIfGs_getEvent(), 0x3402 /* UNK_3402 */);
            }
        }
        break;
    case 1:
        carry_move(i_this);
        break;
    case 2:
        swim_move(i_this);
        break;
    case 3:
        attack_move_(i_this);
        break;
    case 4:
        esa_demo_move_(i_this);
        break;
    }

    cXyz* ccMoveP = &i_this->mStts.m_cc_move; /* GetCCMoveP: null-preserving */
    kb_eye_tex_anm_(i_this);
    if (gabi::ea(ccMoveP) != 0) {
        /* HD: only x/z, clamped to +-10 */
        f32 x = clamp10(ccMoveP->x);
        f32 z = clamp10(ccMoveP->z);
        /* with two NaNs the first operand's payload survives (the position is later copied with
         * integer moves, so the payload is compared) */
        actor->current.pos.x = kb_fadds_ppc(actor->current.pos.x, x);
        actor->current.pos.z = kb_fadds_ppc(actor->current.pos.z, z);
    }

    if (i_this->m41E != 3) {
        f32 temp = i_this->m4D4 + 1.0f;
        if (i_this->m444) {
            i_this->m444 -= 1;
            if (i_this->m444 >= 3) {
                temp = i_this->m4D4 + 0.5625f;
            } else {
                temp = i_this->m4D4 + 1.125f;
            }
        }
        cLib_addCalc2(&i_this->m4E8.y, temp, 0.5f, 0.5f);
    }

    cXyz* attnPos = gabi::at<cXyz>(gabi::ea(actor) + 0x390); /* attention_info.position */
    {
        f32 py = actor->current.pos.y, pz = actor->current.pos.z, px = actor->current.pos.x;
        attnPos->y = py;
        attnPos->z = pz;
        attnPos->x = px;
        attnPos->y = py + gabi::fmadds(i_this->m4D4, 50.0f, 50.0f);
        /* HD: the eye position has its own height */
        actor->eyePos.x = px;
        actor->eyePos.y = py;
        actor->eyePos.z = pz;
        f32 r = REG_F(10, 2);
        actor->eyePos.y = py + gabi::fmadds(i_this->m4D4, r + 50.0f, r + 30.0f);
    }
    gabi::Local<cXyz> temp;
    temp->copy(actor->current.pos);
    speed_pos_set_(i_this);
    BG_check_(i_this);
    {
        gabi::Local<cXyz> c;
        c->x = (f32)actor->current.pos.x;
        f32 y = actor->current.pos.y;
        c->y = y;
        c->z = (f32)actor->current.pos.z;
        c->y = y + (gabi::fmadds(i_this->m4D4, 35.0f, 25.0f) + REG_F(10, 4));
        i_this->mSph.SetC(c);
    }
    i_this->mSph.SetR(gabi::fmadds(i_this->m4D4, 25.0f, 30.0f));
    dComIfG_Ccsp_Set(&i_this->mSph);
    dCcMassS_Mng_Set(&i_this->mSph, 3);

    if (i_this->mAcch.GetGroundH() != -1000000000.0f) {
        u32 mtrlSndId;
        if (i_this->mAcch.ChkGroundHit()) {
            mtrlSndId = dBgS_GetMtrlSndId(dComIfG_Bgsp(), kb_gnd_poly(i_this));
        } else {
            mtrlSndId = 0;
        }
        s8 roomNo = fopAcM_GetRoomNo(actor);
        s32 reverb = dComIfGp_getReverb(roomNo);
        i_this->mpMorf->play(&actor->eyePos, mtrlSndId, (s8)reverb);
    } else {
        i_this->mpMorf->play(nullptr, 0, 0);
    }

    if (i_this->mAcch.ChkGroundHit()) {
        fopAcM_getGroundAngle(actor, &i_this->m4F4);
        if (i_this->m450.x == 0.0f && i_this->m450.y == 0.0f && i_this->m450.z == 0.0f) {
            i_this->m450.copy(actor->current.pos);
        }
        if (i_this->m41E != 1 && i_this->mAcch.GetGroundH() != -1000000000.0f) {
            i_this->m404 = 0;
            if (dBgS_GetAttributeCode(dComIfG_Bgsp(), kb_gnd_poly(i_this)) == 0x13 /* dBgS_Attr_WATER_e */) {
                i_this->m404 = 1;
            }
        }
    }

    draw_SUB_(i_this);

    if (fopAcM_checkCarryNow(actor)) {
        actor->current.pos.copy(*temp);
    }

    if (i_this->m41E == 3 && i_this->m420 >= 0x24) {
        /* HD: no speed.y update here (attack_move applies gravity) */
        gabi::Local<cXyz> temp3;
        cXyz_mi(&actor->old.pos, temp3, &actor->current.pos);
        f32 temp2 = std_sqrtf(PSVECSquareMag(temp3));
        if (temp2 < 1.0f) {
            i_this->m448 += 1;
        }
        if (i_this->m448 > 10) {
            ALL_ANGER() = 0;
            cLib_offBit_u32_(&attn_flags(actor), fopAc_Attn_LOCKON_BATTLE_e);
            /* HD: no mSph.OffAtSetBit() */
            i_this->m41E = 0;
            i_this->m420 = 2;
            dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->m554);
        }
    }

    f32 temp2 = 0.0f;
    if (i_this->m426[5] || (0x21 <= i_this->m420 && i_this->m420 <= 0x24)) {
        s16 a = i_this->m44C + (s16)gabi::ftoi(REG_F(8, 10) + 1000.0f);
        i_this->m44C = a;
        f32 s = JMASinShort_(0x104A44F8, a);
        temp2 = gabi::fnmsubs(std::fabs(s), REG_F(8, 0xB) + 0.5f, 1.0f);
    }
    cLib_addCalc2(&i_this->m4B0, temp2, 1.0f, 0.1f);
    return TRUE;
}
VERIFY(0x02190CE0, daKb_Execute);

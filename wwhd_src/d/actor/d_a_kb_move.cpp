/**
 * d_a_kb_move.cpp (WWHD)
 * NPC - Pig: attack_move and esa_demo_move.
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_kb.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_kb.h"

static inline void anm_init_(kb_class* i_this, int bck, f32 morf, u8 loop, f32 speed, int bas) {
    gabi::call(0x0218EB64, i_this, bck, morf, loop, speed, bas);
}
static inline void smoke_set_(kb_class* i_this) { gabi::call(0x0218F600, i_this); }
static inline BOOL swim_mode_change_check_(kb_class* i_this) { return gabi::call<BOOL>(0x0218F380, i_this); }

/* 021947AC */
void attack_move(kb_class* i_this) {
    WWHD_FUNC(0x021947AC, void, i_this);
    fopAc_ac_c* pPlayer = dComIfGp_getPlayer(0);

    switch (i_this->m420) {
    case 0x1E: {
        anm_init_(i_this, dRes_INDEX_KB_BCK_DAMAGE1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
        attn_flags(i_this) &= ~(u32)fopAc_Attn_ACTION_CARRY_e;
        f32 diffX = i_this->m4DC.x - pPlayer->current.pos.x;
        f32 diffZ = i_this->m4DC.z - pPlayer->current.pos.z;
        std_sqrtf(gabi::fmadds(diffZ, diffZ, diffX + diffX)); /* unused (GameCube: temp1) */
        s16 ang = cM_atan2s(diffX, diffZ);
        i_this->current.angle.y = ang;
        cMtx_YrotS(calc_mtx(), ang);
        gabi::Local<cXyz> temp;
        temp->set(0.0f, 0.0f, 12288.0f);
        gabi::Local<cXyz> temp2;
        MtxPosition(temp, temp2);
        i_this->m4FA.x = (s16)gabi::ftoi(temp2->x);
        i_this->m4FA.z = (s16)gabi::ftoi(temp2->z);
        i_this->speedF = 50.0f;
        i_this->m420 += 1;
        break;
    }
    case 0x1F:
        cLib_addCalcAngleS2(&i_this->shape_angle.x, i_this->m4FA.x, 1, 0x800);
        cLib_addCalcAngleS2(&i_this->shape_angle.z, i_this->m4FA.z, 1, 0x800);
        cLib_addCalc0(&i_this->speedF, 0.5f, 7.0f);
        if (i_this->mpMorf->isStop()) {
            i_this->speedF = 0.0f;
            i_this->m420 += 1;
        }
        break;
    case 0x20:
        cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 1, 0x800);
        cLib_addCalcAngleS2(&i_this->shape_angle.z, 0, 1, 0x800);
        if (i_this->shape_angle.x < 0x100 && i_this->shape_angle.z < 0x100) {
            i_this->shape_angle.x = 0;
            i_this->shape_angle.z = 0;
            i_this->m420 = 4;
        }
        break;
    case 0x21: {
        if (i_this->m50C != dRes_INDEX_KB_BCK_RUN1_e) {
            anm_init_(i_this, dRes_INDEX_KB_BCK_RUN1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 2.0f, dRes_INDEX_KB_BAS_RUN1_e);
        }
        attn_flags(i_this) &= ~(u32)fopAc_Attn_ACTION_CARRY_e;
        f32 diffX = i_this->m4DC.x - i_this->current.pos.x;
        f32 diffZ = i_this->m4DC.z - i_this->current.pos.z;
        i_this->shape_angle.x = 0;
        i_this->shape_angle.z = 0;
        s16 ang = cM_atan2s(diffX, diffZ);
        i_this->shape_angle.y = ang;
        i_this->current.angle.y = ang + 0x8000;
        i_this->speedF = cM_rndFX(3.0f) + 7.0f;
        if (i_this->mShapeType < 8) {
            i_this->speed.y = cM_rndFX(5.0f) + 30.0f;
        }
        i_this->mAcch.OnLineCheck();
        i_this->m4C4 = 30.0f;
        i_this->m420 += 1;
        break;
    }
    case 0x22:
        i_this->shape_angle.x -= 0x2000;
        if (i_this->mAcch.ChkGroundHit()) {
            ALL_ANGER() = 0;
            i_this->mAcch.OffLineCheck();
            gabi::Local<cXyz> dir;
            dir->set(0.0f, 1.0f, 0.0f);
            dComIfGp_getVibration_StartShock(1, -0x21, dir);
            i_this->m420 = 0x23;
        }
        break;
    case 0x23:
        if (std::abs((s32)i_this->shape_angle.x) > 0x100) {
            i_this->shape_angle.x -= 0x1000;
        }
        cLib_addCalc0(&i_this->speedF, 1.0f, 4.0f);
        if (std::abs((s32)i_this->shape_angle.x) < 0x100) {
            /* HD: the run speed and the attack collider are set in mode 0x24 */
            i_this->m426[0] = 0x14;
            i_this->current.angle.y = (s16)i_this->shape_angle.y;
            i_this->m442 = fopAcM_searchPlayerAngleY(i_this);
            i_this->m5D4[0].copy(i_this->current.pos);
            i_this->m5EC[0].x = (s16)i_this->current.angle.x;
            i_this->m5EC[0].y = (s16)i_this->current.angle.y;
            i_this->m5EC[0].z = (s16)i_this->current.angle.z;
            smoke_set_(i_this);
            i_this->m448 = 0;
            i_this->m420 = 0x24;
        }
        break;
    case 0x24: {
        /* HD: accelerates to 15, the attack collider is switched on above speed 10 */
        cLib_addCalc2(&i_this->speedF, 15.0f, 1.0f, 3.0f);
        if (i_this->speedF > 10.0f) {
            i_this->mSph.OnAtSPrmBit(1); /* OnAtSetBit */
        }
        bool temp = false;
        bool bounce = false;
        if (i_this->mSph.ChkAtHit()) {
            fopAc_ac_c* pl = dComIfGp_getPlayer(0);
            fopAc_ac_c* pHitActor = dCcD_GAtTgCoCommonBase_GetAc(&i_this->mSph.mGObjAt);
            if (pHitActor != nullptr && pHitActor == pl) {
                temp = true;
            }
            u32 shield = i_this->mSph.mGObjAt.mRPrm; /* ChkAtShieldHit */
            i_this->mSph.mObjAt.mRPrm &= ~1u;        /* HD: ClrAtHit */
            bounce = (shield & 1) || temp;
        }
        if (bounce) {
            if (i_this->m50C != dRes_INDEX_KB_BCK_RUN1_e) {
                anm_init_(i_this, dRes_INDEX_KB_BCK_RUN1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 2.0f, dRes_INDEX_KB_BAS_RUN1_e);
            }
            i_this->current.angle.y = i_this->shape_angle.y + 0x8000;
            i_this->speedF = cM_rndFX(3.0f) + 7.0f;
            if (i_this->mShapeType < 8) {
                i_this->speed.y = cM_rndFX(5.0f) + 30.0f;
                i_this->m4C4 = 30.0f;
                i_this->m41E = 3;
                i_this->m420 = 0x22;
            } else {
                /* HD: the big pig bounces back at a fixed speed */
                i_this->speed.y = 30.0f;
                i_this->m41E = 3;
                i_this->m420 = 0x22;
                i_this->m4C4 = 30.0f;
                i_this->speedF = REG_F(18, 7);
            }
            dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->m554);
        } else {
            if ((s16)cLib_distanceAngleS(i_this->current.angle.y, i_this->m442) < 0x100) {
                f32 a = (f32)fopAcM_searchPlayerAngleY(i_this);
                i_this->m442 = (s16)gabi::ftoi(a + cM_rndFX(4096.0f));
            }
            i_this->m5D4[0].copy(i_this->current.pos);
            i_this->m5EC[0].x = (s16)i_this->current.angle.x;
            i_this->m5EC[0].y = (s16)i_this->current.angle.y;
            i_this->m5EC[0].z = (s16)i_this->current.angle.z;
            cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->m442, 2, 0x2000);
            i_this->current.angle.y = (s16)i_this->shape_angle.y;
            if ((i_this->mpMorf->checkFrame(0.0f) || i_this->mpMorf->checkFrame(5.0f)) && i_this->m426[2] == 0) {
                kb_mons_se2(i_this, JA_SE_CV_PG_L_NORMAL, JA_SE_CV_PG_NORMAL);
            }
            if (i_this->m426[2] == 0 && (s16)cLib_distanceAngleS(i_this->current.angle.y, i_this->shape_angle.y) > 0x4000) {
                kb_mons_se2(i_this, JA_SE_CV_PG_L_TURN, JA_SE_CV_PG_TURN);
                i_this->m426[2] = 0x18;
            }
            s16 z = (s16)gabi::ftoi((f32)(i_this->current.angle.y - i_this->shape_angle.y) * 0.5f);
            if (z < 0) {
                if ((int)(u16)z < (u16)-0x1000) {
                    z = -0x1000;
                }
            } else if (z > 0x1000) {
                z = 0x1000;
            }
            i_this->shape_angle.z = z;
        }
        break;
    }
    }

    /* HD: a wall stops the pig; gravity is applied here */
    if (i_this->mAcch.ChkWallHit() && i_this->speedF > 0.0f) {
        i_this->speedF = 0.0f;
    }
    i_this->speed.y = i_this->speed.y - 3.0f;

    if (swim_mode_change_check_(i_this)) {
        i_this->speed.y = 0.0f;
        i_this->shape_angle.x = 0;
        i_this->gravity = 0.0f;
        if (i_this->m420 == 0x22) {
            i_this->mAcch.OffLineCheck();
        }
        ALL_ANGER() = 0;
        /* HD: no mSph.ClrAtSet() */
    }
}
VERIFY(0x021947AC, attack_move);

static inline void smoke_set2_(kb_class* i_this) { gabi::call(0x0218F71C, i_this); }
static inline void smoke_set3_(kb_class* i_this) { gabi::call(0x0218F868, i_this); }
static inline void money_drop_(kb_class* i_this) { gabi::call(0x021907F4, i_this); }

static inline void copy_angle(csXyz* dst, csXyz* src) {
    dst->x = (s16)src->x;
    dst->y = (s16)src->y;
    dst->z = (s16)src->z;
}
/* m5D4[1] / m5EC[1]: the dig smoke position on the ground */
static inline void set_dig_pos(kb_class* i_this, f32 dy) {
    i_this->m5D4[1].copy(i_this->current.pos);
    i_this->m5D4[1].y = i_this->mAcch.GetGroundH() + 15.0f + dy;
    copy_angle(&i_this->m5EC[1], &i_this->current.angle);
}
static inline void set_dig_pos0(kb_class* i_this) {
    i_this->m5D4[1].copy(i_this->current.pos);
    i_this->m5D4[1].y = i_this->mAcch.GetGroundH() + 15.0f;
    copy_angle(&i_this->m5EC[1], &i_this->current.angle);
}
static inline void set_cam_center(kb_class* i_this) {
    i_this->m520.x = i_this->current.pos.x + REG_F(12, 1);
    i_this->m520.y = i_this->current.pos.y + 60.0f + REG_F(12, 2);
    i_this->m520.z = i_this->current.pos.z + REG_F(12, 3);
}
static inline void copy_f(cXyz* dst, cXyz* src) {
    dst->x = (f32)src->x;
    dst->y = (f32)src->y;
    dst->z = (f32)src->z;
}

/* 0219509C */
void esa_demo_move(kb_class* i_this) {
    WWHD_FUNC(0x0219509C, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    u8* pCamera = kb_getPlayerCamera();
    gabi::Local<dBgS_LinChk> linChk;
    dBgS_LinChk_ct(linChk, LINCHK_VT, false);

    switch (i_this->m420) {
    case 0x28:
        if (i_this->m50C != dRes_INDEX_KB_BCK_WALK1_e) {
            anm_init_(i_this, dRes_INDEX_KB_BCK_WALK1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.5f, dRes_INDEX_KB_BAS_WALK1_e);
        }
        /* mSph.SetCoVsGrp(cCcD_CoSPrm_VsEnemy_e) */
        i_this->mSph.mObjCo.mSPrm = (i_this->mSph.mObjCo.mSPrm & ~0x70u) | 0x10;
        i_this->actor_status |= 0x4000;
        i_this->m420 += 1;
        /* fallthrough */
    case 0x29:
        if (!eventInfo_checkCommandDemoAccrpt(i_this)) {
            /* dComIfGp_event_onEventFlag(1) */
            u32 f = dComIfGp_ea() + 0x52B8;
            gabi::store<u16>(f, (u16)(gabi::load<u16>(f) | 1));
            fopAcM_orderPotentialEvent(i_this, 2 /* dEvtType_OTHER_e */, 0xFFFF, 0);
            eventInfo_onCondition(i_this, 2 /* dEvtCnd_UNK2_e */);
            break;
        }
        daPy_changeOriginalDemo(player);
        daPy_changeDemoMode(player, daPy_demo_DEMO_N_WAIT_e);
        dCamera_Stop(camera_body(pCamera));
        dCamera_SetTrimSize(camera_body(pCamera), 2);
        i_this->m426[0] = 200;
        i_this->m53C = 50.0f;
        i_this->m420 += 1;
        /* fallthrough */
    case 0x2A: {
        set_cam_center(i_this);
        cMtx_YrotS(calc_mtx(), (s16)gabi::ftoi(REG_F(12, 4)));
        gabi::Local<cXyz> temp;
        temp->x = 0.0f;
        temp->y = 0.0f;
        temp->z = REG_F(12, 5) + 300.0f;
        gabi::Local<cXyz> temp2;
        MtxPosition(temp, temp2);
        PSVECAdd(temp2, &i_this->field_0x498, temp2);
        temp2->y = temp2->y + (REG_F(12, 7) + 300.0f);
        dBgS_LinChk_Set(linChk, &i_this->m520, temp2, i_this);
        if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
            temp->z = -(REG_F(12, 5) + 300.0f);
            MtxPosition(temp, temp2);
            PSVECAdd(temp2, &i_this->field_0x498, temp2);
            temp2->y = temp2->y + (REG_F(12, 7) + 300.0f);
        }
        copy_f(&i_this->m514, temp2);
        i_this->speedF = REG_F(12, 9) + 3.0f;
        f32 diffX = i_this->field_0x498.x - i_this->current.pos.x;
        f32 diffZ = i_this->field_0x498.z - i_this->current.pos.z;
        cLib_addCalcAngleS2(&i_this->current.angle.y, cM_atan2s(diffX, diffZ), 4, 0x800);
        i_this->shape_angle.y = (s16)i_this->current.angle.y;
        if (i_this->mAcch.ChkWallHit()) {
            i_this->m426[0] = 0;
        }
        if (std_sqrtf(gabi::fmadds(diffX, diffX, diffZ * diffZ)) < 40.0f) {
            i_this->speedF = 0.0f;
            i_this->speed.set(0.0f, 0.0f, 0.0f);
            anm_init_(i_this, dRes_INDEX_KB_BCK_EAT1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 2.0f, dRes_INDEX_KB_BAS_EAT1_e);
            i_this->m44A = 1;
            i_this->m420 += 1;
        } else if (i_this->m426[0] == 0) {
            i_this->m420 = 0x31;
        }
        break;
    }
    case 0x2B:
        if (i_this->mpMorf->isStop()) {
            i_this->m44A -= 1;
            if (i_this->m44A == 0) {
                if (i_this->m4D8 != fpcM_ERROR_PROCESS_ID_e) {
                    fopAc_ac_c* pEsa = fopAcM_SearchByID(i_this->m4D8);
                    if (pEsa != nullptr) {
                        fopAcM_delete(pEsa);
                    }
                    i_this->m4D8 = fpcM_ERROR_PROCESS_ID_e;
                }
                anm_init_(i_this, dRes_INDEX_KB_BCK_NAKU1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_KB_BAS_NAKU1_e);
                if (i_this->mShapeType >= 8) {
                    kb_mons_se_start_a(i_this, JA_SE_CV_PG_L_TURN);
                } else {
                    kb_mons_se_start_a(i_this, JA_SE_CV_PG_TURN);
                }
                i_this->m420 += 1;
            }
        }
        break;
    case 0x2C:
        i_this->m40A = 0;
        if (i_this->mpMorf->isStop()) {
            if (i_this->mAcch.GetGroundH() != -1000000000.0f /* -G_CM3D_F_INF */) {
                if (dBgS_GetSpecialCode(dComIfG_Bgsp(), kb_gnd_poly(i_this)) == 5) {
                    if (i_this->mShapeType >= 8) {
                        anm_init_(i_this, dRes_INDEX_KB_BCK_RUN1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 2.0f, dRes_INDEX_KB_BAS_RUN1_e);
                        i_this->m409 = 1;
                        i_this->m5D4[0].copy(i_this->current.pos);
                        copy_angle(&i_this->m5EC[0], &i_this->current.angle);
                        smoke_set2_(i_this);
                        set_dig_pos0(i_this);
                        smoke_set3_(i_this);
                        i_this->m420 += 1;
                    } else {
                        i_this->m420 = 0x31;
                    }
                    break;
                }
                if (dBgS_GetAttributeCode(dComIfG_Bgsp(), kb_gnd_poly(i_this)) == 1 /* dBgS_Attr_DIRT_e */ ||
                    dBgS_GetAttributeCode(dComIfG_Bgsp(), kb_gnd_poly(i_this)) == 4 /* dBgS_Attr_GRASS_e */ ||
                    dBgS_GetAttributeCode(dComIfG_Bgsp(), kb_gnd_poly(i_this)) == 0xB /* dBgS_Attr_SAND_e */ ||
                    dBgS_GetAttributeCode(dComIfG_Bgsp(), kb_gnd_poly(i_this)) == 0x13 /* dBgS_Attr_WATER_e */) {
                    i_this->m5D4[0].copy(i_this->current.pos);
                    copy_angle(&i_this->m5EC[0], &i_this->current.angle);
                    smoke_set2_(i_this);
                    set_dig_pos0(i_this);
                    smoke_set3_(i_this);
                    anm_init_(i_this, dRes_INDEX_KB_BCK_RUN1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 2.0f, dRes_INDEX_KB_BAS_RUN1_e);
                    i_this->m420 += 1;
                    break;
                }
            }
            i_this->m420 = 0x31;
        }
        break;
    case 0x2D:
        if (i_this->m40A == 0) {
            if (i_this->mShapeType >= 8) {
                kb_se_start_a(i_this, JA_SE_CM_PG_L_DIG);
            } else {
                kb_se_start_a(i_this, JA_SE_CM_PG_DIG);
            }
            i_this->m40A = 1;
        }
        i_this->m5D4[0].copy(i_this->current.pos);
        copy_angle(&i_this->m5EC[0], &i_this->current.angle);
        set_dig_pos0(i_this);
        cLib_addCalcAngleS2(&i_this->shape_angle.x, (s16)gabi::ftoi(REG_F(12, 9) + 10000.0f), 1, (s16)gabi::ftoi(REG_F(12, 10) + 300.0f));
        cLib_addCalc2(&i_this->m4C0, REG_F(12, 0xB) + -33.0f, 1.0f, REG_F(12, 0xC) + 1.0f);
        if (std::fabs(i_this->m4C0 - (REG_F(12, 0xB) + -30.0f)) < 1.0f) {
            i_this->shape_angle.x = (s16)gabi::ftoi(REG_F(12, 9) + 10000.0f);
            i_this->m4C0 = -33.0f;
            dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->m554);
            dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->m574);
            if (i_this->m409 != 0) {
                /* search_get_item (inline) */
                void* pItem = fpcM_Search(0x0218EEB4 /* item_tag_search */, i_this);
                if (pItem != nullptr) {
                    daTagKbItem_kb_dig(pItem, i_this);
                    i_this->m407 = 1;
                    if (i_this->home.angle.z == 1) {
                        i_this->home.angle.z = 0;
                        i_this->m407 = 0;
                        for (int i = 0; i < 10; i++) {
                            fopAcM_create(PROC_KS, 0x00000007, &i_this->current.pos, -1, nullptr, nullptr, -1, 0);
                        }
                    }
                } else {
                    money_drop_(i_this);
                }
            } else {
                money_drop_(i_this);
            }
            i_this->m426[0] = (s16)gabi::ftoi(REG_F(12, 0xF) + 40.0f);
            i_this->m420 += 1;
        }
        break;
    case 0x2E:
        if (i_this->m426[0] == 0x23) {
            if (i_this->m407) {
                daPy_changeDemoMode(player, daPy_demo_DEMO_SMILE_e);
            } else {
                daPy_changeDemoMode(player, daPy_demo_DEMO_UNK_018_e);
            }
        }
        if (i_this->m426[0] == 0) {
            i_this->shape_angle.x = -0x7FFF;
            daPy_changeDemoMode(player, daPy_demo_DEMO_N_WAIT_e);
            set_dig_pos(i_this, REG_F(8, 0x11));
            smoke_set3_(i_this);
            i_this->speed.y = REG_F(12, 0xE) + 20.0f;
            kb_catch_se(i_this);
            i_this->m426[0] = (s16)gabi::ftoi(REG_F(12, 0xF) + 40.0f);
            i_this->m420 += 1;
        }
        break;
    case 0x2F:
        set_dig_pos(i_this, REG_F(8, 0x11));
        set_cam_center(i_this);
        cLib_addCalc2(&i_this->m4C0, REG_F(12, 0xB) + 30.0f, 1.0f, REG_F(12, 0xC) + 10.0f);
        cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 1, 0x1000);
        if (i_this->m426[0] == 0 ||
            (i_this->speed.y < 0.0f && i_this->mAcch.GetGroundH() + 10.0f + REG_F(8, 5) > i_this->current.pos.y)) {
            dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->m574);
            if (i_this->mShapeType >= 8) {
                gabi::Local<cXyz> dir;
                dir->set(0.0f, 1.0f, 0.0f);
                dComIfGp_getVibration_StartShock(1, -0x21, dir);
                kb_se_start(i_this, JA_SE_CM_PG_L_JUMP);
            } else {
                kb_se_start(i_this, JA_SE_CM_PG_DIG_LANDING);
            }
            i_this->shape_angle.x = 0;
            i_this->m4C0 = 30.0f;
            i_this->speedF = 0.0f;
            anm_init_(i_this, dRes_INDEX_KB_BCK_NAKU1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_KB_BAS_NAKU1_e);
            i_this->m420 += 1;
        }
        break;
    case 0x30:
        set_cam_center(i_this);
        if (i_this->mpMorf->isStop()) {
            i_this->m420 += 1;
        }
        break;
    case 0x31: {
        if (i_this->mShapeType >= 8) {
            kb_mons_se_start_a(i_this, JA_SE_CM_PG_L_JUMP);
        }
        daPy_cancelOriginalDemo(player);
        gabi::Local<cXyz> center;
        copy_f(center, &i_this->m520);
        gabi::Local<cXyz> eye;
        copy_f(eye, &i_this->m514);
        dCamera_Reset(camera_body(pCamera), center, eye);
        dCamera_Start(camera_body(pCamera));
        dCamera_SetTrimSize(camera_body(pCamera), 0);
        dComIfGp_event_reset();
        i_this->actor_status &= ~0x4000u;
        /* mSph.SetCoVsGrp(cCcD_CoSPrm_VsGrpAll_e) */
        i_this->mSph.mObjCo.mSPrm = (i_this->mSph.mObjCo.mSPrm & ~0x70u) | 0x70;
        DEMO_START() = 0;
        i_this->m406 = 0;
        i_this->m426[4] = 0x64;
        i_this->gravity = -3.0f;
        i_this->m420 = 2;
        i_this->m41E = 0;
        break;
    }
    }

    if (i_this->m420 >= 0x2A) {
        gabi::Local<cXyz> center;
        copy_f(center, &i_this->m520);
        gabi::Local<cXyz> eye;
        copy_f(eye, &i_this->m514);
        dCamera_Set(camera_body(pCamera), center, eye, i_this->m53C, 0);
        /* player->setPlayerPosAndAngle(&player->current.pos, angle) (virtual, +0x114) */
        u32 vt = player->__vtbl;
        s16 angle = fopAcM_searchActorAngleY(player, i_this);
        gabi::call_ptr(gabi::load<u32>(vt + 0x114), player, &player->current.pos, angle);
    }
    dBgS_LinChk_dt(linChk);
}
VERIFY(0x0219509C, esa_demo_move);

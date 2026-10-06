/**
 * d_a_bb_move.cpp (WWHD)
 * Enemy - Kargaroc: the movement functions (bb_wait_move, bb_su_wait_move, bb_auto_move).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bb.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bb.h"

enum {
    dRes_INDEX_BB_BAS_FLY01_e = 0x8,
    dRes_INDEX_BB_BAS_FLY03_e = 0xA,
    dRes_INDEX_BB_BAS_LAND01_e = 0xF,
    dRes_INDEX_BB_BAS_LAND02_e = 0x10,
    dRes_INDEX_BB_BAS_WAIT_e = 0x11,
    dRes_INDEX_BB_BCK_FLY01_e = 0x18,
    dRes_INDEX_BB_BCK_FLY03_e = 0x1A,
    dRes_INDEX_BB_BCK_LAND01_e = 0x1F,
    dRes_INDEX_BB_BCK_LAND02_e = 0x20,
    dRes_INDEX_BB_BCK_WAIT_e = 0x21,
    dRes_INDEX_BB_BAS_EAT_e = 0x7,
    dRes_INDEX_BB_BAS_WALK_e = 0x12,
    dRes_INDEX_BB_BCK_EAT_e = 0x17,
    dRes_INDEX_BB_BCK_WALK_e = 0x22,
};

/* bb_wait_move and bb_su_wait_move have the same body (GameCube too) */
static inline void wait_move_body(bb_class* i_this) {
    gabi::Local<cXyz> v;
    gabi::Local<cXyz> v2;
    s8 r30 = -1;
    dComIfGp_getPlayer(0); /* HD: the unused player pointer is still fetched */

    f32 speedX = std::fabs((f32)i_this->speed.x);
    if (speedX < 5.0f) {
        speedX = 5.0f;
    }
    f32 speedZ = std::fabs((f32)i_this->speed.z);
    if (speedZ < 5.0f) {
        speedZ = 5.0f;
    }

    i_this->unk_C60 = 2;

    switch (i_this->unk_2F1) {
    case -1:
        i_this->unk_57C = 1;
        if (!i_this->mpMorf->isStop()) {
            break;
        }
        i_this->unk_2F1 = 0;
        /* fallthrough */
    case 0:
        anm_init(i_this, dRes_INDEX_BB_BCK_WAIT_e, 1.0f, 2, 1.0f, dRes_INDEX_BB_BAS_WAIT_e);
        i_this->unk_2F1 = 1;
        i_this->unk_318[0] = 100;
        /* fallthrough */
    case 1:
        i_this->unk_57C = 1;
        cLib_addCalcAngleS2_l(&i_this->current.angle.y, i_this->home.angle.y, 5, 0x300);
        cLib_addCalcAngleS2_l(&i_this->current.angle.x, 0, 5, 0x800);
        cLib_addCalcAngleS2_l(&i_this->current.angle.z, 0, 5, 0x800);
        cLib_addCalc2_l(&i_this->current.pos.x, i_this->home.pos.x, 0.2f, speedX);
        cLib_addCalc2_l(&i_this->current.pos.z, i_this->home.pos.z, 0.2f, speedZ);
        i_this->current.pos.y = i_this->home.pos.y;
        if (i_this->unk_318[0] == 0 && bb_player_view_check(i_this)) {
            i_this->unk_2F1 = 2;
            i_this->unk_318[0] = 0x1E;
        }
        break;

    case 2:
        i_this->unk_C60 = 1;
        if (i_this->unk_318[0] != 0) {
            break;
        }
        i_this->unk_2F1 = 3;
        anm_init(i_this, dRes_INDEX_BB_BCK_FLY01_e, 5.0f, 2, l_bbHIO().unk_24 * 1.5f, dRes_INDEX_BB_BAS_FLY01_e);
        i_this->unk_318[0] = REG0_S(0) + 70;
        /* fallthrough */
    case 3:
        i_this->unk_C60 = 1;
        cLib_addCalcAngleS2_l(&i_this->current.angle.y, i_this->unk_336, 10, 0x200);
        v->x = 0.0f;
        v->y = gabi::fmadds(REG0_F(0), 10.0f, 300.0f);
        v->z = gabi::fmadds(REG0_F(1), 10.0f, 300.0f);
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        MtxPosition(v, v2);
        cLib_addCalc2_l(&i_this->current.pos.x, i_this->home.pos.x + v2->x, 0.1f, 10.0f);
        cLib_addCalc2_l(&i_this->current.pos.y, i_this->home.pos.y + v2->y, 0.1f, 10.0f);
        cLib_addCalc2_l(&i_this->current.pos.z, i_this->home.pos.z + v2->z, 0.1f, 10.0f);
        if (i_this->unk_318[0] == 0) {
            i_this->unk_2DD = 3;
            i_this->unk_2F1 = 0;
        }
        break;

    case 10:
    case 11:
    case 12: {
        r30 = 0;
        i_this->unk_C60 = 0;
        v->x = 0.0f;
        v->y = 0.0f;
        v->z = 0.0f;
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        MtxPosition(v, v2);

        f32 fVar2 = 80.0f;
        if (i_this->unk_2F1 == 10) {
            fVar2 = 300.0f;
        }
        f32 tx = i_this->home.pos.x + v2->x;
        f32 ty = i_this->home.pos.y + fVar2;
        f32 tz = i_this->home.pos.z + v2->z;
        i_this->unk_2F4.x = tx;
        i_this->unk_2F4.y = ty;
        i_this->unk_2F4.z = tz;
        i_this->unk_300 = 20.0f;
        i_this->unk_304 = 5.0f;
        i_this->unk_310 = 2000.0f;

        f32 y = ty - i_this->current.pos.y;
        f32 x = tx - i_this->current.pos.x;
        f32 z = tz - i_this->current.pos.z;
        f32 sqrt = std_sqrtf(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)));

        if (i_this->unk_2F1 == 10) {
            if (sqrt < 2000.0f) {
                i_this->unk_2F1 = 11;
            }
        } else if (i_this->unk_2F1 == 11) {
            if (sqrt < (f32)l_bbHIO().unk_50) {
                i_this->unk_2F1 = 12;
                anm_init(i_this, dRes_INDEX_BB_BCK_FLY03_e, 10.0f, 0, 1.0f, dRes_INDEX_BB_BAS_FLY03_e);
            }
        } else if (i_this->unk_2F1 == 12 && sqrt < gabi::fmadds(REG0_F(9), 10.0f, 300.0f)) {
            i_this->unk_2F1 = 20;
            anm_init(i_this, dRes_INDEX_BB_BCK_LAND01_e, 5.0f, 2, l_bbHIO().unk_44, dRes_INDEX_BB_BAS_LAND01_e);
            i_this->speed.y = 0.0f;
            i_this->unk_354 = 0;
            i_this->unk_2F0 = 10;
            r30 = -1;
        }
        break;
    }
    case 20: {
        s16 old_354 = i_this->unk_354;
        i_this->unk_C60 = 0;
        i_this->unk_354 = old_354 + 1;
        i_this->unk_57C = 1;
        cLib_addCalc2_l(&i_this->current.pos.x, i_this->home.pos.x, 0.1f, speedX);
        cLib_addCalc2_l(&i_this->current.pos.z, i_this->home.pos.z, 0.1f, speedZ);
        i_this->current.pos.y = i_this->current.pos.y + i_this->speed.y;
        if (old_354 > l_bbHIO().unk_40) {
            i_this->speed.y = i_this->speed.y - (REG0_F(7) + 0.8f);
        }
        cLib_addCalcAngleS2_l(&i_this->current.angle.x, 0, 5, 0x800);
        cLib_addCalcAngleS2_l(&i_this->current.angle.y, i_this->home.angle.y, 5, 0x300);
        cLib_addCalcAngleS2_l(&i_this->current.angle.z, 0, 5, 0x800);

        f32 homeY = i_this->home.pos.y;
        if (!(i_this->current.pos.y > homeY)) { /* GHS: bgt over the block */
            i_this->current.pos.y = homeY;
            i_this->speed.y = -0.5f;
            if (old_354 > l_bbHIO().unk_40) {
                anm_init(i_this, dRes_INDEX_BB_BCK_LAND02_e, 5.0f, 0, l_bbHIO().unk_48, dRes_INDEX_BB_BAS_LAND02_e);
                i_this->unk_2F1 = -1;
            }
        }
        break;
    }
    }

    if (r30 == 0) {
        bb_pos_move(i_this);
    }
}

/* 0206348C (unnamed by the matcher) */
void bb_wait_move(bb_class* i_this) {
    WWHD_FUNC(0x0206348C, void, i_this);
    wait_move_body(i_this);
}
VERIFY(0x0206348C, bb_wait_move);

/* 02063BDC (unnamed by the matcher) */
void bb_su_wait_move(bb_class* i_this) {
    WWHD_FUNC(0x02063BDC, void, i_this);
    wait_move_body(i_this);
}
VERIFY(0x02063BDC, bb_su_wait_move);

/* 020625E8 */
void bb_auto_move(bb_class* i_this) {
    WWHD_FUNC(0x020625E8, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s32 frame;
    s8 r29 = 0;
    gabi::Local<cXyz> sp34;

    i_this->unk_C60 = 2;

    switch (i_this->unk_2F0) {
    case 0:
        frame = gabi::ftoi(i_this->mpMorf->getFrame());
        if (i_this->unk_318[0] == 0 && frame == REG0_S(0) + 9) {
            i_this->unk_2F0 = 1;
            i_this->unk_318[0] = (s16)gabi::ftoi(cM_rndF(200.0f) + 50.0f);
            anm_init(i_this, dRes_INDEX_BB_BCK_FLY02_e, REG0_F(0) + 12.0f, 2, 1.0f, dRes_INDEX_BB_BAS_FLY02_e);
        }
        break;
    case 1:
        if (i_this->unk_318[0] == 0 && i_this->current.pos.y < i_this->unk_2F4.y) {
            i_this->unk_2F0 = 0;
            i_this->unk_318[0] = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
            anm_init(i_this, dRes_INDEX_BB_BCK_FLY01_e, 5.0f, 2, l_bbHIO().unk_24, dRes_INDEX_BB_BAS_FLY01_e);
        }
        break;
    case 10:
        break;
    case 20:
        frame = gabi::ftoi(i_this->mpMorf->getFrame());
        if (frame == 0x22) {
            i_this->unk_2F0 = 0;
            i_this->unk_318[0] = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
            anm_init(i_this, dRes_INDEX_BB_BCK_FLY01_e, 0.0f, 2, l_bbHIO().unk_24, dRes_INDEX_BB_BAS_FLY01_e);
        }
        break;
    }

    switch (i_this->unk_2F1) {
    case 0:
        if (i_this->unk_318[1] == 0) {
            f32 rx = cM_rndFX(l_bbHIO().unk_0C);
            f32 x = (i_this->home.pos.x + rx) - i_this->current.pos.x;
            f32 rz = cM_rndFX(l_bbHIO().unk_0C);
            f32 z = (i_this->home.pos.z + rz) - i_this->current.pos.z;

            if (std_sqrtf(gabi::fmadds(x, x, z * z)) > 200.0f) {
                i_this->unk_318[1] = (s16)gabi::ftoi(cM_rndF(150.0f) + 50.0f);
                i_this->unk_2F4.x = x + i_this->current.pos.x;
                f32 ry = cM_rndF(500.0f);
                i_this->unk_2F4.y = i_this->home.pos.y + ry;
                i_this->unk_2F4.z = z + i_this->current.pos.z;
                i_this->unk_308 = 0.0f;
                i_this->unk_300 = REG0_F(10) + 25.0f;
                i_this->unk_304 = REG0_F(11) + 1.0f;
                i_this->unk_310 = cM_rndF(300.0f) + 200.0f;

                if (l_bbHIO().unk_06 != 0 || (i_this->unk_318[3] == 0 && i_this->unk_2DF == 0)) {
                    esa_class* esa = search_esa(i_this);
                    if (esa != nullptr) {
                        i_this->unk_2F1 = 10;
                        i_this->unk_330 = fopAcM_GetID(esa);
                        i_this->unk_310 = 1000.0f;
                    } else if (bb_player_view_check(i_this) != 0) {
                        i_this->unk_2DD = 3;
                        i_this->unk_2F1 = 0;
                    }
                }
            }
        }
        break;

    case 10:
    case 11: {
        i_this->unk_C60 = 0;
        fopAc_ac_c* ac = fopAcM_SearchByID(i_this->unk_330);
        if (ac != nullptr) {
            sp34->x = 0.0f;
            sp34->y = 0.0f;
            sp34->z = gabi::fmadds(REG0_F(16), 10.0f, -200.0f);
            cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
            gabi::Local<cXyz> v2;
            MtxPosition(sp34, v2);

            f32 tx = ac->current.pos.x + v2->x;
            i_this->unk_2F4.x = tx;
            f32 ty = gabi::fmadds(REG0_F(8), 10.0f, ac->current.pos.y + 80.0f);
            i_this->unk_2F4.y = ty;
            f32 tz = ac->current.pos.z + v2->z;
            i_this->unk_2F4.z = tz;
            i_this->unk_300 = 20.0f;

            f32 y = ty - i_this->current.pos.y;
            f32 x = tx - i_this->current.pos.x;
            f32 z = tz - i_this->current.pos.z;
            f32 sqrt = std_sqrtf(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)));

            if (i_this->unk_2F1 == 10) {
                if (sqrt < (f32)l_bbHIO().unk_50) {
                    i_this->unk_2F1 = 11;
                    anm_init(i_this, dRes_INDEX_BB_BCK_FLY03_e, 10.0f, 0, 1.0f, dRes_INDEX_BB_BAS_FLY03_e);
                }
            } else if (sqrt < gabi::fmadds(REG0_F(9), 10.0f, 300.0f)) {
                i_this->unk_2F1 = 20;
                anm_init(i_this, dRes_INDEX_BB_BCK_LAND01_e, 5.0f, 2, l_bbHIO().unk_44, dRes_INDEX_BB_BAS_LAND01_e);
                i_this->speed.y = 0.0f;
                i_this->unk_354 = 0;
                i_this->unk_2F0 = 10;
                r29 = -1;
            }
        } else {
            i_this->unk_2F1 = 0;
        }
        break;
    }
    case 20: {
        r29 = -1;
        s16 old_354 = i_this->unk_354;
        f32 sx = std::fabs((f32)i_this->speed.x);
        i_this->unk_C60 = 0;
        i_this->unk_354 = old_354 + 1;
        cLib_addCalc2_l(&i_this->current.pos.x, i_this->unk_2F4.x, 0.1f, sx);
        cLib_addCalc2_l(&i_this->current.pos.z, i_this->unk_2F4.z, 0.1f, std::fabs((f32)i_this->speed.z));
        i_this->current.pos.y = i_this->current.pos.y + i_this->speed.y;
        if (old_354 > l_bbHIO().unk_40) {
            i_this->speed.y = i_this->speed.y - (REG0_F(7) + 0.8f);
        }
        cLib_addCalcAngleS2_l(&i_this->current.angle.x, 0, 5, 0x800);
        cLib_addCalcAngleS2_l(&i_this->current.angle.z, 0, 5, 0x800);

        if (dBgS_Acch_GetGroundH(&i_this->mAcch) - i_this->current.pos.y < -200.0f) {
            i_this->unk_2F1 = 25;
        } else if (dBgS_Acch_ChkGroundHit(&i_this->mAcch)) {
            i_this->speed.y = -0.5f;
            anm_init(i_this, dRes_INDEX_BB_BCK_LAND02_e, 5.0f, 0, l_bbHIO().unk_48, dRes_INDEX_BB_BAS_LAND02_e);
            i_this->unk_2F1 = 22;
            i_this->unk_318[2] = 50;
        }
        break;
    }
    case 21: {
        r29 = 1;
        esa_class* esa = search_esa(i_this);
        if (esa == nullptr) {
            i_this->unk_2F1 = 25;
        } else {
            i_this->unk_2F1 = 22;
        }
        break;
    }
    case 22:
        r29 = 1;
        i_this->unk_300 = 0.0f;
        i_this->unk_310 = 0.0f;
        i_this->speedF = 0.0f;
        if (i_this->unk_318[2] == 0) {
            i_this->unk_2F1 = 23;
            fopAc_ac_c* ac = fopAcM_SearchByID(i_this->unk_330);
            if (ac != nullptr) {
                i_this->unk_2F4.x = ac->current.pos.x;
                i_this->unk_2F4.z = ac->current.pos.z;
                i_this->unk_310 = 1000.0f;
                i_this->unk_308 = 1.0f;
                anm_init(i_this, dRes_INDEX_BB_BCK_WALK_e, 5.0f, 2, 1.0f, dRes_INDEX_BB_BAS_WALK_e);
            } else {
                i_this->unk_2F1 = 25;
            }
        }
        break;

    case 23: {
        r29 = 1;
        i_this->unk_C60 = 0;
        i_this->unk_300 = l_bbHIO().unk_54;
        f32 z = i_this->unk_2F4.z - i_this->current.pos.z;
        f32 x = i_this->unk_2F4.x - i_this->current.pos.x;
        i_this->speedF = 1.0f;
        if (std_sqrtf(gabi::fmadds(x, x, z * z)) < 110.0f) {
            i_this->unk_2F1 = 21;
            i_this->unk_318[2] = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
            anm_init(i_this, dRes_INDEX_BB_BCK_EAT_e, 5.0f, 0, 1.0f, dRes_INDEX_BB_BAS_EAT_e);
            i_this->unk_2F1 = 24;
        }
        break;
    }
    case 24: {
        r29 = 1;
        i_this->unk_300 = 0.0f;
        i_this->unk_C60 = 0;
        i_this->speedF = 0.0f;
        i_this->unk_C50 = 0;
        frame = gabi::ftoi(i_this->mpMorf->getFrame());
        if (frame == 9) {
            fopAc_ac_c* ac = fopAcM_SearchByID(i_this->unk_330);
            if (ac != nullptr) {
                fopAcM_delete(ac);
            }
        }
        if (i_this->mpMorf->isStop()) {
            i_this->unk_2F1 = 21;
            i_this->unk_318[2] = (s16)gabi::ftoi(cM_rndF(50.0f));
            anm_init(i_this, dRes_INDEX_BB_BCK_WAIT_e, 5.0f, 2, 1.0f, dRes_INDEX_BB_BAS_WAIT_e);
        }
        break;
    }
    case 25:
        r29 = 1;
        i_this->unk_2F1 = 0;
        anm_init(i_this, dRes_INDEX_BB_BCK_FLY01_e, 5.0f, 2, l_bbHIO().unk_24, dRes_INDEX_BB_BAS_FLY01_e);
        i_this->unk_310 = gabi::fmadds(REG0_F(4), 10.0f, 2000.0f);
        i_this->unk_300 = 25.0f;
        i_this->unk_304 = 1.0f;
        i_this->speedF = 0.0f;
        i_this->unk_324 = REG0_S(4) + 10;
        i_this->unk_318[3] = (s16)gabi::ftoi(cM_rndF(100.0f) + 100.0f);
        i_this->unk_324 = REG0_S(4) + 10;
        if (i_this->unk_2DA != 0xFF) {
            i_this->unk_35D = (s8)(i_this->unk_2DA + 1);
            path_check(i_this);
            i_this->unk_364 = 1;
        } else {
            i_this->unk_318[1] = 50;
            MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
            sp34->x = 0.0f;
            sp34->y = 1000.0f;
            sp34->z = 2000.0f;
            cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
            MtxPosition(sp34, &i_this->unk_2F4);
        }
        break;
    }

    switch (r29) {
    case 0:
        bb_pos_move(i_this);
        break;
    case 1:
        bb_ground_pos_move(i_this);
        if (i_this->unk_2F1 != 25) {
            f32 z = player->current.pos.z - i_this->current.pos.z;
            f32 x = player->current.pos.x - i_this->current.pos.x;
            if (std_sqrtf(gabi::fmadds(x, x, z * z)) < 400.0f) {
                i_this->unk_2F1 = 0;
                fopAc_ac_c* ac = fopAcM_SearchByID(i_this->unk_330);
                if (ac != nullptr) {
                    ((esa_class*)ac)->field_0x298 = 0;
                }
                i_this->unk_2F0 = 0;
                i_this->unk_318[0] = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
                anm_init(i_this, dRes_INDEX_BB_BCK_FLY01_e, 3.0f, 2, l_bbHIO().unk_24, dRes_INDEX_BB_BAS_FLY01_e);
                if (i_this->unk_2DA != 0xFF) {
                    i_this->unk_35D = (s8)(i_this->unk_2DA + 1);
                    path_check(i_this);
                    i_this->unk_318[3] = (s16)gabi::ftoi(cM_rndF(250.0f) + 250.0f);
                    i_this->unk_364 = 1;
                } else {
                    i_this->unk_318[1] = 50;
                    i_this->unk_310 = gabi::fmadds(REG0_F(4), 10.0f, 5000.0f);
                    i_this->unk_300 = 30.0f;
                    i_this->unk_304 = 3.0f;
                    i_this->speedF = 0.0f;
                    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
                    sp34->x = 0.0f;
                    sp34->y = 1000.0f;
                    sp34->z = 2000.0f;
                    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
                    MtxPosition(sp34, &i_this->unk_2F4);
                    i_this->unk_318[3] = (s16)gabi::ftoi(cM_rndF(250.0f) + 250.0f);
                }
            }
        }
        break;
    }
}
VERIFY(0x020625E8, bb_auto_move);

/**
 * d_a_bb_exec.cpp (WWHD)
 * Enemy - Kargaroc: daBb_Execute (inlines bb_fail_move, bb_path_move, bb_atack_move,
 * bb_kamome_attack, bb_setpos_bg_check and bb_eye_tex_anm).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bb.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bb.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* daPy_py_c::checkPlayerGuard(): virtual (vtable at +0xB4, slot +0x3C) */
/* this TU's out-of-line copies (decompiled in d_a_bb.cpp) */


#define PATH_ASSERT_FILE "d_a_bb.cpp"
#define PATH_ASSERT_MSG "i_this->ppd != (0)"

/* inlined into daBb_Execute */
static inline s32 morf_frame(bb_class* i_this) { return gabi::ftoi(i_this->mpMorf->getFrame()); }
/* inlined into daBb_Execute */
static inline BbPoint* path_point(bb_class* i_this) {
    BbPath* ppd = i_this->ppd;
    return ppd->getPoint(i_this->mPathPoint);
}
/* GHS copies a path point's position through FPRs */
/* inlined into daBb_Execute */
static inline void copy_pos_f(cXyz* dst, const cXyz* src) {
    dst->x = (f32)src->x;
    dst->y = (f32)src->y;
    dst->z = (f32)src->z;
}

/* bb_fail_move */
/* inlined into daBb_Execute */
static inline void bb_fail_move(bb_class* i_this) {
    fopAcM_createDisappear(i_this, &i_this->current.pos, 10, 0, i_this->stealItemBitNo);
    fopAcM_delete(i_this);
}

/* bb_path_move */
/* inlined into daBb_Execute */
static inline void bb_path_move(bb_class* i_this) {
    dComIfGp_get(); // HD: unused
    s8 r29 = 0;
    s32 frame;
    BbPoint* point;
    f32 x, y, z, d;

    i_this->mHeadLookMode = 2;

    switch ((s8)i_this->mFlyAnmState) {
    case 0:
        frame = morf_frame(i_this);
        if (i_this->current.pos.y > i_this->mTargetPos.y && frame == REG0_S(0) + 9) {
            i_this->mFlyAnmState = 1;
            anm_init(i_this, dRes_INDEX_BB_BCK_FLY02_e, REG0_F(0) + 12.0f, 2, 1.0f, dRes_INDEX_BB_BAS_FLY02_e);
        }
        break;
    case 1:
        if (!(i_this->current.pos.y > i_this->mTargetPos.y)) {
            i_this->mFlyAnmState = 0;
            anm_init(i_this, dRes_INDEX_BB_BCK_FLY01_e, 5.0f, 2, l_bbHIO.unk_24, dRes_INDEX_BB_BAS_FLY01_e);
        }
        break;
    }

    switch ((s8)i_this->mMode) {
    case 0: {
        i_this->mPathPoint = (s8)(i_this->mPathPoint + i_this->mPathDir);
        BbPath* ppd = i_this->ppd;
        if (i_this->mPathPoint >= (s8)ppd->m_num) {
            if (ppd->m_closed & 1) {
                i_this->mPathPoint = 0;
            } else {
                i_this->mPathDir = -1;
                i_this->mPathPoint = (s8)(i_this->ppd->m_num - 2);
            }
            if ((s32)ppd->m_nextID != 0xFFFF) {
                i_this->ppd = (BbPath*)dPath_GetRoomPath(ppd->m_nextID, fopAcM_GetRoomNo(i_this));
                if (i_this->ppd == nullptr) JUT_ASSERT_fail(PATH_ASSERT_FILE, 0x5D4, PATH_ASSERT_MSG);
            }
        } else if (i_this->mPathPoint < 0) {
            i_this->mPathDir = 1;
            i_this->mPathPoint = 1;
        }
    }
        /* fallthrough */
    case -1: {
        i_this->mMode = 1;
        point = path_point(i_this);
        if (i_this->mPathReentry != 0) {
            i_this->mTargetSpeed = 25.0f;
            i_this->mSpeedStep = 2.0f;
            i_this->mPathReentry = 0;
        } else if (i_this->current.pos.y < i_this->mTargetPos.y) {
            i_this->mTargetSpeed = 20.0f;
            i_this->mSpeedStep = REG0_F(11) + 1.0f;
        } else {
            i_this->mTargetSpeed = 30.0f;
            i_this->mSpeedStep = REG0_F(13) + 1.0f;
        }
        i_this->mTurnScale = REG0_F(7);
        copy_pos_f(&i_this->mTargetPos, &point->m_position);
        if (point->mArg3 == BB_PATH_LAND) {
            i_this->mMode = 10;
        } else if (i_this->mCarryState == 0) {
            i_this->mTargetPos.x = point->m_position.x + cM_rndFX(150.0f);
            i_this->mTargetPos.y = point->m_position.y + cM_rndFX(150.0f);
            i_this->mTargetPos.z = point->m_position.z + cM_rndFX(150.0f);
        }
        if (l_bbHIO.unk_06 != 0 || (i_this->mTimers[3] == 0 && i_this->mCarryState == 0)) {
            esa_class* esa = search_esa(i_this);
            if (esa != nullptr) {
                i_this->mEsaId = fopAcM_GetID(esa);
                i_this->mMode = 10;
                i_this->mTurnSpeed = 1000.0f;
                i_this->mPathOn = 0;
            } else if (bb_player_view_check(i_this) != 0) {
                i_this->mBehavior = BB_MODE_ATTACK;
                i_this->mMode = 0;
                i_this->mPathOn = 0;
            }
        }
        break;
    }
    case 1: {
        cLib_addCalc2(&i_this->mTurnScale, 1.0f, 1.0f, 0.04f);
        x = i_this->mTargetPos.x - i_this->current.pos.x;
        y = i_this->mTargetPos.y - i_this->current.pos.y;
        z = i_this->mTargetPos.z - i_this->current.pos.z;
        d = std_sqrtf(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)));
        f32 a = gabi::fmadds(REG0_F(5), 10.0f, 100.0f);
        f32 b = gabi::fmadds(REG0_F(6), 1000.0f, 500000.0f);
        f32 v = a + b / d;
        i_this->mTurnSpeed = v;
        f32 lim = gabi::fmadds(REG0_F(4), 10.0f, 5000.0f);
        if (v > lim) i_this->mTurnSpeed = lim;
        if (d < gabi::fmadds(REG0_F(10), 10.0f, 300.0f)) {
            i_this->mMode = 0;
            point = path_point(i_this);
            if (point->mArg3 == BB_PATH_RELEASE) {
                i_this->mDropCarried = 1;
            } else if (point->mArg3 == BB_PATH_DELETE) {
                fopAcM_delete(i_this);
            }
        }
        break;
    }
    case 10:
        i_this->mTargetSpeed = 20.0f;
        x = i_this->mTargetPos.x - i_this->current.pos.x;
        y = gabi::fmadds(REG0_F(8), 10.0f, i_this->mTargetPos.y + 50.0f) - i_this->current.pos.y;
        z = i_this->mTargetPos.z - i_this->current.pos.z;
        if (std_sqrtf(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y))) < gabi::fmadds(REG0_F(9), 10.0f, 300.0f)) {
            i_this->speed.y = 0.0f;
            i_this->mMode = 20;
            i_this->mFlyAnmState = 10;
            r29 = -1;
        }
        break;
    case 20: {
        r29 = -1;
        frame = morf_frame(i_this);
        cLib_addCalc2(&i_this->current.pos.x, i_this->mTargetPos.x, 0.1f, std::fabs((f32)i_this->speed.x));
        cLib_addCalc2(&i_this->current.pos.z, i_this->mTargetPos.z, 0.1f, std::fabs((f32)i_this->speed.z));
        i_this->current.pos.y = i_this->current.pos.y + i_this->speed.y;
        if (frame > REG0_S(2) + 15) {
            i_this->speed.y = i_this->speed.y - (REG0_F(7) + 0.8f);
        }
        cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 5, 0x800);
        cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 5, 0x800);
        if (dBgS_Acch_ChkGroundHit(&i_this->mAcch)) {
            i_this->speed.y = -0.5f;
        }
        if (i_this->mTimers[2] == 0) {
            i_this->mMode = 0x17;
        }
        break;
    }
    case 22:
        r29 = 1;
        if (i_this->mTimers[2] == 0) {
            i_this->mPathPoint = (s8)(i_this->mPathPoint + 1);
            if (i_this->mPathPoint >= (s32)i_this->ppd->m_num) {
                i_this->mPathPoint = 0;
            }
            point = path_point(i_this);
            copy_pos_f(&i_this->mTargetPos, &point->m_position);
            if (point->mArg3 == BB_PATH_TAKEOFF) {
                i_this->mMode = 0x19;
            } else {
                i_this->mMode = 0x17;
                i_this->mTurnSpeed = 1000.0f;
                i_this->mTurnScale = 1.0f;
            }
        }
        i_this->mTargetSpeed = 0.0f;
        i_this->speedF = 0.0f;
        break;
    case 23:
        r29 = 1;
        frame = morf_frame(i_this);
        if (frame >= REG0_S(3) && frame <= REG0_S(4) + 9) {
            f32 s = 5.0f * i_this->unk_328;
            i_this->mTargetSpeed = s;
            i_this->speedF = s;
        } else {
            i_this->speedF = 0.0f;
            i_this->mTargetSpeed = 0.0f;
            x = i_this->mTargetPos.x - i_this->current.pos.x;
            z = i_this->mTargetPos.z - i_this->current.pos.z;
            if (std_sqrtf(gabi::fmadds(x, x, z * z)) < 50.0f) {
                i_this->mMode = 0x16;
                if (path_point(i_this)->mArg3 == BB_PATH_WAIT) {
                    i_this->mTimers[2] = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
                }
            }
        }
        break;
    case 25:
        r29 = 1;
        anm_init(i_this, dRes_INDEX_BB_BCK_FLY01_e, 5.0f, 2, l_bbHIO.unk_24, dRes_INDEX_BB_BAS_FLY01_e);
        i_this->mMode = -1;
        i_this->mTurnSpeed = gabi::fmadds(REG0_F(4), 10.0f, 5000.0f);
        i_this->speedF = 0.0f;
        i_this->mTargetSpeed = 25.0f;
        i_this->mSpeedStep = 2.0f;
        i_this->mLiftTimer = (s16)(REG0_S(4) + 10);
        break;
    }

    switch (r29) {
    case 0:
        bb_pos_move(i_this);
        break;
    case 1:
        bb_ground_pos_move(i_this);
        break;
    }
}

/* sea check after a head attack (bb_kamome_attack / bb_atack_move) */
/* inlined into daBb_Execute */
static inline void head_water_check(bb_class* i_this) {
    cXyz* eye = &i_this->eyePos;
    if (daSea_ChkArea(eye->x, eye->z)) {
        f32 waveHeight = daSea_calcWave(eye->x, eye->z);
        if (eye->y < waveHeight && !(i_this->mPrevEyeY < waveHeight)) {
            cXyz pos;
            pos.x = (f32)eye->x;
            pos.y = (f32)eye->y;
            pos.y = waveHeight;
            pos.z = (f32)eye->z;
            fopKyM_createWpillar(&pos, REG0_F(9) + 0.7f, REG0_F(10) + 0.7f, 0);
            fopAcM_seStart(i_this, JA_SE_OBJ_FALL_WATER_S, 0);
        }
        i_this->mPrevEyeY = eye->y;
    }
}

/* bb_kamome_attack */
/* inlined into daBb_Execute */
static inline void bb_kamome_attack(bb_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    i_this->mHeadLookMode = 1;

    switch ((s8)i_this->mFlyAnmState) {
    case 0: {
        s32 frame = morf_frame(i_this);
        if (i_this->current.pos.y > i_this->mTargetPos.y && frame == REG0_S(0) + 9) {
            i_this->mFlyAnmState = 1;
            anm_init(i_this, dRes_INDEX_BB_BCK_FLY02_e, REG0_F(0) + 12.0f, 2, 1.0f, dRes_INDEX_BB_BAS_FLY02_e);
        }
        break;
    }
    case 1:
        if (!(i_this->current.pos.y > i_this->mTargetPos.y)) {
            i_this->mFlyAnmState = 0;
            anm_init(i_this, dRes_INDEX_BB_BCK_FLY01_e, 5.0f, 2, l_bbHIO.unk_24, dRes_INDEX_BB_BAS_FLY01_e);
        }
        break;
    default:
        i_this->mFlyAnmState = 0;
        break;
    }

    switch ((s8)i_this->mMode) {
    case 0:
        anm_init(i_this, dRes_INDEX_BB_BCK_FLY02_e, 10.0f, 2, 1.0f, dRes_INDEX_BB_BAS_FLY02_e);
        i_this->mMode = 1;
        /* fallthrough */
    case 1: {
        s32 t = i_this->mFrameCounter;
        f32 sx = cM_ssin(t * (REG0_S(5) + 400));
        f32 vx = sx * (REG0_F(5) + 300.0f);
        f32 sy = cM_ssin(t * (REG0_S(5) + 300));
        f32 vy = sy * (REG0_F(3) + 100.0f);
        f32 sz = cM_ssin(t * (REG0_S(6) + 100));
        f32 vz = gabi::fmadds(sz, REG0_F(7) + 100.0f, REG0_F(4) + 100.0f);
        cXyz v;
        v.x = vx;
        v.y = vy;
        v.z = vz;
        cMtx_YrotS(calc_mtx(), player->shape_angle.y);
        cXyz v2;
        MtxPosition(&v, &v2);
        cXyz sum;
        cXyz_pl(&v2, &sum, &player->current.pos);
        i_this->mTargetPos.copy(sum);
        break;
    }
    default:
        i_this->mMode = 0;
        break;
    }

    i_this->mTargetSpeed = l_bbHIO.unk_08;
    cXyz diff;
    cXyz_mi(&i_this->mTargetPos, &diff, &i_this->current.pos);
    cXyz tmp;
    tmp.copy(diff);
    if (std_sqrtf(PSVECSquareMag(&tmp)) < REG0_F(9) + 100.0f) {
        i_this->mTargetSpeed = 0.0f;
    }

    i_this->mSpeedStep = REG0_F(10) + 0.5f;
    i_this->mTurnSpeed = REG0_F(11) + 1000.0f;
    i_this->mTurnScale = 1.0f;

    bb_pos_move(i_this);

    if ((i_this->mFrameCounter & 0x3F) == 0 && cM_rndF(1.0f) < 0.5f) {
        kuti_open(i_this, 0x1B, JA_SE_CV_BB_ATTACK);
    }

    i_this->mHeadAtSph.SetC(&i_this->eyePos);
    dComIfG_Ccsp_Set(&i_this->mHeadAtSph);

    cXyz* eye = &i_this->eyePos;
    if (daSea_ChkArea(i_this->eyePos.x, i_this->eyePos.z)) {
        f32 waveHeight = daSea_calcWave(eye->x, eye->z);
        if (eye->y < waveHeight && !(i_this->mPrevEyeY < waveHeight)) {
            cXyz pos;
            pos.x = (f32)eye->x;
            pos.y = (f32)eye->y;
            pos.y = waveHeight;
            pos.z = (f32)eye->z;
            fopKyM_createWpillar(&pos, REG0_F(9) + 0.7f, REG0_F(10) + 0.7f, 0);
            fopAcM_seStart(i_this, JA_SE_OBJ_FALL_WATER_S, 0);
        }
        i_this->mPrevEyeY = eye->y;
        f32 minY = waveHeight + 30.0f;
        if (i_this->current.pos.y < minY) {
            i_this->current.pos.y = minY;
        }
    }
}

/* bb_setpos_bg_check */
/* inlined into daBb_Execute */
static inline BOOL bb_setpos_bg_check(bb_class* i_this) {
    dComIfGp_get(); // HD: unused
    BbLineCheck linChk;
    dBgS_LinChk_ct(&linChk);
    cXyz sp8;
    cXyz sp14;
    f32 hx = i_this->home.pos.x;
    f32 hy = i_this->home.pos.y;
    sp8.x = hx;
    sp8.y = hy;
    sp8.z = (f32)i_this->home.pos.z;
    sp8.y = hy + 100.0f;
    sp14.copy(i_this->eyePos);
    dBgS_LinChk_Set(&linChk, &sp14, &sp8, i_this);
    BOOL ret = cBgS_LineCross(dComIfG_Bgsp(), &linChk);
    dBgS_LinChk_dt(&linChk);
    return ret;
}

/* bb_atack_move */
/* inlined into daBb_Execute */
static inline void bb_atack_move(bb_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s8 r29 = 0;
    s8 r28 = 0;
    f32 x, y, z;
    bbHIO_c& hio = l_bbHIO;

    i_this->mHeadLookMode = 1;

    if (i_this->mWasHit != 0) {
        i_this->mWasHit = 0;
        i_this->mTimers[5] = (s16)gabi::ftoi((cM_rndF(30.0f) + 20.0f) * hio.unk_20);
        anm_init(i_this, dRes_INDEX_BB_BCK_FLYB01_e, 10.0f, 2, hio.unk_28, dRes_INDEX_BB_BAS_FLYB01_e);
        i_this->mTurnScale = 0.0f;
        i_this->mAttackTurnScale = 0.0f;
        i_this->mMode = 4;
        kuti_open(i_this, 0x30, JA_SE_CV_BB_NORMAL);
    }

    if (player == (fopAc_ac_c*)bbFindPlayerActor(i_this)) {
        bb_kamome_attack(i_this);
        return;
    }

    switch ((s8)i_this->mMode) {
    case 0:
        i_this->mMode = 1;
        i_this->mTurnScale = 0.0f;
        i_this->mTargetSpeed = 30.0f;
        i_this->mSpeedStep = 1.0f;
        anm_init(i_this, dRes_INDEX_BB_BCK_FLY02_e, 10.0f, 2, 1.0f, dRes_INDEX_BB_BAS_FLY02_e);
        /* fallthrough */
    case 1:
    case 2: {
        f32 py = player->current.pos.y;
        i_this->mTargetPos.x = (f32)player->current.pos.x;
        i_this->mTargetPos.y = py;
        i_this->mTargetPos.z = (f32)player->current.pos.z;
        i_this->mTargetPos.y = py + 200.0f;

        bb_pos_move(i_this);

        x = i_this->mTargetPos.x - i_this->current.pos.x;
        y = i_this->mTargetPos.y - i_this->current.pos.y;
        z = i_this->mTargetPos.z - i_this->current.pos.z;
        s8 st = i_this->mMode;
        f32 sq = gabi::fmadds(x, x, y * y);
        if (st == 1) {
            i_this->mTurnSpeed = 400.0f;
            if (std_sqrtf(gabi::fmadds(z, z, sq)) < (f32)hio.unk_50) {
                i_this->mMode = 2;
                anm_init(i_this, dRes_INDEX_BB_BCK_FLY03_e, 10.0f, 0, 1.0f, dRes_INDEX_BB_BAS_FLY03_e);
                kuti_open(i_this, 0x15, JA_SE_CV_BB_FIND);
            }
        } else {
            i_this->mTurnSpeed = 2000.0f;
            if (std_sqrtf(gabi::fmadds(z, z, sq)) < gabi::fmadds(REG0_F(9), 100.0f, 350.0f)) {
                i_this->mMode = 3;
            }
        }
        break;
    }
    case 3:
        i_this->mTargetSpeed = 0.0f;
        i_this->mSpeedStep = 3.0f;
        bb_pos_move(i_this);
        if (i_this->speedF < 0.1f) {
            i_this->mMode = 4;
            i_this->mTimers[0] = 0;
            if (i_this->mPrmMode == BB_MODE_WAIT || i_this->mPrmMode == BB_MODE_NEST_WAIT) {
                f32 r = cM_rndF((f32)(s32)(hio.unk_18 - hio.unk_16));
                i_this->mTimers[1] = (s16)gabi::ftoi(r + (f32)(s32)hio.unk_16);
            } else {
                f32 r = cM_rndF((f32)(s32)(hio.unk_14 - hio.unk_12));
                i_this->mTimers[1] = (s16)gabi::ftoi(r + (f32)(s32)hio.unk_12);
            }
            i_this->mTurnScale = 0.0f;
            i_this->mAttackTurnScale = 0.0f;
            anm_init(i_this, dRes_INDEX_BB_BCK_FLYB01_e, 5.0f, 2, hio.unk_28, dRes_INDEX_BB_BAS_FLYB01_e);
            kuti_open(i_this, 0x30, JA_SE_CV_BB_NORMAL);
        }
        break;
    case 4: {
        s32 t = (s16)(i_this->mMoveCounter + 1);
        i_this->mMoveCounter = (s16)t;
        f32 sx = cM_ssin(t * (REG0_S(2) + 1000));
        f32 sy = cM_ssin(t * (REG0_S(3) + 1200));
        f32 cz = cM_scos(t * (REG0_S(4) + 1500));

        cLib_addCalc2(&i_this->current.pos.x, gabi::fmadds(sx, 200.0f, i_this->mTargetPos.x), 0.1f, 20.0f * i_this->mTurnScale);
        cLib_addCalc2(&i_this->current.pos.y, gabi::fmadds(sy, 100.0f, i_this->mTargetPos.y), 0.1f, 20.0f * i_this->mTurnScale);
        cLib_addCalc2(&i_this->current.pos.z, gabi::fmadds(cz, 200.0f, i_this->mTargetPos.z), 0.1f, 20.0f * i_this->mTurnScale);
        cLib_addCalc2(&i_this->mTurnScale, hio.unk_1C, 1.0f, 0.1f * hio.unk_1C);

        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mPlayerAngleY, 5, (s16)gabi::ftoi(1280.0f * i_this->mAttackTurnScale));
        cLib_addCalcAngleS2(&i_this->current.angle.x, i_this->mPlayerAngleX, 5, (s16)gabi::ftoi(768.0f * i_this->mAttackTurnScale));
        cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 5, (s16)gabi::ftoi(768.0f * i_this->mAttackTurnScale));

        cLib_addCalc2(&i_this->mAttackTurnScale, 1.0f, 1.0f, 0.05f);

        if (i_this->mTimers[1] == 0) {
            r29 = 1;
            break;
        }

        if ((i_this->mTimers[0] & 0x1F) == 0 && cM_rndF(1.0f) < 0.5f) {
            kuti_open(i_this, 0x1B, JA_SE_CV_BB_ATTACK);
        }

        if (i_this->mTimers[0] == 0) {
            i_this->mTargetPos.x = player->current.pos.x + cM_rndFX(400.0f);
            f32 py = player->current.pos.y + 100.0f;
            i_this->mTargetPos.y = py + cM_rndF(200.0f);
            i_this->mTargetPos.z = player->current.pos.z + cM_rndFX(400.0f);
            i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(100.0f) + 20.0f);
            i_this->mTurnScale = 0.0f;
            i_this->mAttackTurnScale = 0.0f;
        }

        if (i_this->mTimers[5] == 0) {
            i_this->mTimers[5] = (s16)gabi::ftoi((cM_rndF(30.0f) + 20.0f) * hio.unk_20);
            f32 dist = i_this->mPlayerDistXZ;
            if (dist > gabi::fmadds(REG0_F(5), 10.0f, 200.0f) && dist < gabi::fmadds(REG0_F(6), 10.0f, 350.0f)) {
                i_this->mMode = 5;
                i_this->mTimers[0] = hio.unk_32;
                anm_init(i_this, dRes_INDEX_BB_BCK_FLYB01_e, 2.0f, 2, hio.unk_34, dRes_INDEX_BB_BAS_FLYB01_e);
                tex_anm_set(i_this, 0);
            }
        }
        break;
    }
    case 5: {
        i_this->mBlinkWait = 2;
        s32 t = i_this->mMoveCounter;
        f32 sx = cM_ssin(t * (REG0_S(2) + 1000));
        f32 cz = cM_scos(t * (REG0_S(4) + 1500));

        cLib_addCalc2(&i_this->current.pos.x, gabi::fmadds(sx, 200.0f, i_this->mTargetPos.x), 0.1f, 30.0f * i_this->mTurnScale);
        cLib_addCalc2(&i_this->current.pos.y, player->current.pos.y + 175.0f, 0.1f, 30.0f * i_this->mTurnScale);
        cLib_addCalc2(&i_this->current.pos.z, gabi::fmadds(cz, 200.0f, i_this->mTargetPos.z), 0.1f, 30.0f * i_this->mTurnScale);
        cLib_addCalc2(&i_this->mTurnScale, hio.unk_1C, 1.0f, 0.1f * hio.unk_1C);

        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mPlayerAngleY, 5, (s16)gabi::ftoi(2048.0f * i_this->mAttackTurnScale));
        cLib_addCalc2(&i_this->mAttackTurnScale, 1.0f, 1.0f, 0.05f);

        if (i_this->mTimers[0] == 0) {
            i_this->mMode = 6;
            anm_init(i_this, dRes_INDEX_BB_BCK_ATACK01_e, 2.0f, 0, hio.unk_2C, dRes_INDEX_BB_BAS_ATACK01_e);
            i_this->mTargetPos.x = (f32)player->current.pos.x;
            i_this->mTargetPos.y = player->current.pos.y + 100.0f;
            i_this->mTargetPos.z = (f32)player->current.pos.z;
            i_this->mTurnScale = 0.0f;
            i_this->mAttackTurnScale = 0.0f;
            i_this->speedF = 0.0f;
            i_this->mTargetSpeed = REG0_F(7) + 15.0f;
            i_this->mSpeedStep = 5.0f;
        }
        break;
    }
    case 6: {
        i_this->mHeadLookMode = 0;
        i_this->mBlinkWait = 2;
        s32 frame = morf_frame(i_this);
        if (frame == 0x12) {
            i_this->mTargetSpeed = -10.0f;
            i_this->mSpeedStep = 5.0f;
        }
        i_this->mTurnSpeed = 2000.0f;
        bb_pos_move(i_this);

        if (i_this->mpMorf->isStop()) {
            i_this->mMode = 4;
            anm_init(i_this, dRes_INDEX_BB_BCK_FLYB01_e, 3.0f, 2, hio.unk_28, dRes_INDEX_BB_BAS_FLYB01_e);
            i_this->mTimers[5] = (s16)gabi::ftoi((cM_rndF(30.0f) + 20.0f) * hio.unk_20);
            i_this->mTurnScale = 0.0f;
            i_this->mAttackTurnScale = 0.0f;
            tex_anm_set(i_this, 3);
        } else if (frame < REG0_S(2) + 18) {
            r28 = 1;
            if (bbPlayerGuard(player) && i_this->mHeadAtSph.ChkAtHit()) {
                i_this->mGuardedTimer = 10;
                fopAcM_seStart(i_this, JA_SE_LK_ROPE_HOOK_METAL, 0);
                cXyz scale;
                scale.z = 4.0f;
                scale.x = 4.0f;
                scale.y = 4.0f;
                dComIfGp_particle_set(ID_AK_JN_NG, &i_this->eyePos, nullptr, &scale);
                i_this->mMode = 10;
                i_this->mTimers[0] = hio.unk_30;
                anm_init(i_this, dRes_INDEX_BB_BCK_GUSYA01_e, 0.0f, 0, hio.unk_38, dRes_INDEX_BB_BAS_GUSYA01_e);
                i_this->mTargetSpeed = REG0_F(7);
                i_this->mSpeedStep = 1.0f;
                r28 = 0;
                i_this->speedF = -20.0f;
                fopAcM_monsSeStart(i_this, JA_SE_CV_BB_DAMAGE, 0);
                tex_anm_set(i_this, 1);
            }
        }
        break;
    }
    case 10:
    case 11:
        i_this->mHeadLookMode = 0;
        i_this->mBlinkWait = 2;
        bb_pos_move(i_this);
        if (i_this->mpMorf->isStop() && i_this->mMode == 10) {
            i_this->mMode = 11;
            anm_init(i_this, dRes_INDEX_BB_BCK_FURA01_e, 0.0f, 2, hio.unk_3C, dRes_INDEX_BB_BAS_FURA01_e);
        }
        if (i_this->mMode == 11 && (i_this->mTimers[0] & 7) == 5) {
            kuti_open(i_this, 0x12, JA_SE_CV_BB_FURAFURA);
        }
        if (i_this->mTimers[0] == 0) {
            i_this->mMode = 4;
            i_this->mTimers[5] = (s16)gabi::ftoi((cM_rndF(30.0f) + 20.0f) * hio.unk_20);
            anm_init(i_this, dRes_INDEX_BB_BCK_FLYB01_e, 10.0f, 2, hio.unk_28, dRes_INDEX_BB_BAS_FLYB01_e);
            i_this->mTurnScale = 0.0f;
            i_this->mAttackTurnScale = 0.0f;
            tex_anm_set(i_this, 5);
        }
        cLib_addCalc2(&i_this->current.pos.y, dBgS_Acch_GetGroundH(&i_this->mAcch) + 50.0f, 0.05f, 5.0f);
        break;
    }

    if (i_this->mPrmMode != BB_MODE_ATTACK) {
        x = i_this->mTargetPos.x - i_this->current.pos.x;
        y = i_this->mTargetPos.y - i_this->current.pos.y;
        z = i_this->mTargetPos.z - i_this->current.pos.z;
        BOOL hit = std_sqrtf(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y))) > hio.unk_7C;
        if (!hit) hit = bb_player_bg_check(i_this);
        if (!hit) hit = bb_setpos_bg_check(i_this) | r29;
        if (hit) {
            i_this->mBehavior = i_this->mPrmMode;
            u8 pathIdx = i_this->mPathNo;
            if (pathIdx != 0xFF) {
                i_this->mPathOn = (s8)(pathIdx + 1);
                path_check(i_this);
            }
            if (i_this->mBehavior == BB_MODE_WAIT || i_this->mBehavior == BB_MODE_NEST_WAIT) {
                i_this->mMode = 10;
                anm_init(i_this, dRes_INDEX_BB_BCK_FLY01_e, 5.0f, 2, hio.unk_24, dRes_INDEX_BB_BAS_FLY01_e);
            } else {
                i_this->mMode = 0;
                i_this->mTimers[0] = 0;
                i_this->mTimers[1] = 0;
                i_this->mFlyAnmState = 1;
                anm_init(i_this, dRes_INDEX_BB_BCK_FLY02_e, REG0_F(0) + 12.0f, 2, 1.0f, dRes_INDEX_BB_BAS_FLY02_e);
                i_this->speedF = 0.0f;
                i_this->mTargetPos.y = player->current.pos.y + 500.0f;
                i_this->mTimers[3] = (s16)gabi::ftoi(cM_rndF(200.0f) + 300.0f);
            }
            i_this->mTurnScale = 0.0f;
        }
    }

    if (r28 != 0) {
        i_this->mHeadAtSph.SetC(&i_this->eyePos);
        dComIfG_Ccsp_Set(&i_this->mHeadAtSph);
        head_water_check(i_this);
    } else {
        i_this->mHeadAtSph.ClrAtHit();
    }
}

/* bb_eye_tex_anm */
/* inlined into daBb_Execute */
static inline void bb_eye_tex_anm(bb_class* i_this) {
    if (i_this->mBlinkWait != 0) {
        i_this->mBlinkWait = (s16)(i_this->mBlinkWait - 1);
    } else {
        i_this->mBlinkWait = (s16)gabi::ftoi(cM_rndF(50.0f) + 10.0f);
        if (i_this->mEyeAnmOn == 0) {
            tex_anm_set(i_this, 4);
        }
    }
    if (i_this->mEyeAnmOn != 0) {
        if (i_this->mEyeFrame < i_this->mEyeMaxFrame) {
            i_this->mEyeFrame = (u8)(i_this->mEyeFrame + 1);
        } else {
            i_this->mEyeAnmOn = 0;
        }
    }
}

/* 0205EA58 */
BOOL daBb_Execute(bb_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    i_this->mEnemyIce.m02C = REG0_F(5) + 50.0f;

    if (enemy_ice(&i_this->mEnemyIce)) {
        i_this->mpMorf->setPlayMode(J3DFrameCtrl::EMode_NONE);
        i_this->mpMorf->setPlaySpeed(3.0f);
        i_this->mpMorf->play(&i_this->eyePos, 0, 0);
        MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, false);
        cMtx_YrotM(calc_mtx(), (s16)(i_this->current.angle.y + i_this->mBodyYawOffset));
        cMtx_XrotM(calc_mtx(), (s16)(i_this->current.angle.x + i_this->mBodyPitchOffset));
        cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
        // HD: copies mDoMtx_stack_c::now, not *calc_mtx
        J3DModel_setBaseTRMtx(i_this->mpMorf->getModel(), mDoMtx_stack_c::get());
        i_this->mpMorf->calc();
        i_this->mTailPos[0].copy(i_this->mTailBasePos[1]);
        tail_control(i_this);
        return TRUE;
    }

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);

    f32 zDiff = player->current.pos.z - i_this->current.pos.z;
    f32 xDiff = player->current.pos.x - i_this->current.pos.x;
    f32 yDiff = (player->current.pos.y + 100.0f) - i_this->current.pos.y;

    i_this->mPlayerDistXZ = std_sqrtf(gabi::fmadds(xDiff, xDiff, zDiff * zDiff));
    i_this->mPlayerAngleY = cM_atan2s(xDiff, zDiff);
    i_this->mPlayerAngleX = (s16)-cM_atan2s(yDiff, i_this->mPlayerDistXZ);

    i_this->mFrameCounter = (s16)(i_this->mFrameCounter + 1);
    if (i_this->mAppearWait != 0) {
        if (dComIfGs_isSwitch(i_this->mAppearWait - 1, fopAcM_GetRoomNo(i_this))) {
            i_this->mAppearWait = 0;
        } else {
            return TRUE;
        }
    }

    i_this->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
    i_this->actor_status |= fopAcStts_SHOWMAP_e;

    if (l_bbHIO.unk_05 == 0) {
        for (s32 i = 0; i < 6; i++) {
            if (i_this->mTimers[i] != 0) {
                i_this->mTimers[i] = (s16)(i_this->mTimers[i] - 1);
            }
        }
        if (i_this->mGuardedTimer != 0) {
            i_this->mGuardedTimer = (s16)(i_this->mGuardedTimer - 1);
        }
        if (i_this->mHitCooldown != 0) {
            i_this->mHitCooldown = (s16)(i_this->mHitCooldown - 1);
        }

        if (i_this->mKnockbackDist > 0.01f) {
            cXyz sp28;
            cXyz sp1C;
            sp28.x = 0.0f;
            sp28.y = 0.0f;
            sp28.z = -i_this->mKnockbackDist;
            cMtx_YrotS(calc_mtx(), i_this->mKnockbackAngleY);
            cMtx_XrotM(calc_mtx(), i_this->mKnockbackAngleX);
            MtxPosition(&sp28, &sp1C);
            i_this->current.pos.x = i_this->current.pos.x + sp1C.x;
            i_this->current.pos.y = i_this->current.pos.y + sp1C.y;
            i_this->current.pos.z = i_this->current.pos.z + sp1C.z;
            cLib_addCalc0(&i_this->mKnockbackDist, 1.0f, 2.0f);
            i_this->mTurnScale = 0.0f;
            i_this->mAttackTurnScale = 0.0f;
            i_this->current.angle.y = (s16)(i_this->current.angle.y + i_this->mKnockbackSpinSpeed);
            cLib_addCalcAngleS2(&i_this->mKnockbackSpinSpeed, 0, 1, 200);
            i_this->mpMorf->play(&i_this->current.pos, 0, 0);
        } else {
            i_this->mpMorf->play(&i_this->current.pos, 0, 0);
            if (i_this->mDying != 0) {
                bb_fail_move(i_this);
            } else if (i_this->mPathOn != 0) {
                bb_path_move(i_this);
            } else if (i_this->mBehavior == BB_MODE_ATTACK) {
                bb_atack_move(i_this);
            } else if (i_this->mBehavior == BB_MODE_WAIT) {
                bb_wait_move(i_this);
            } else if (i_this->mBehavior == BB_MODE_NEST_WAIT) {
                bb_su_wait_move(i_this);
            } else {
                bb_auto_move(i_this);
            }
        }
    }

    MtxTrans(i_this->current.pos.x, i_this->current.pos.y + i_this->mEnemyIce.m028, i_this->current.pos.z, false);

    s16 x = i_this->mJointRot[0].x / 2;
    s16 z = i_this->mJointRot[0].z / 2;
    cMtx_YrotM(calc_mtx(), (s16)(i_this->current.angle.y + i_this->mBodyYawOffset + x));
    cMtx_XrotM(calc_mtx(), (s16)(i_this->current.angle.x + i_this->mBodyPitchOffset + z));
    cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);

    J3DModel* model = i_this->mpMorf->getModel();
    J3DModel_setBaseScale(model, &i_this->scale);
    J3DModel_setBaseTRMtx(model, calc_mtx());
    i_this->mpMorf->calc();

    i_this->mTailPos[0].copy(i_this->mTailBasePos[1]);

    tail_control(i_this);

    enemy_fire(&i_this->mEnemyFire);

    if (i_this->mCarryState != 0) {
        switch (i_this->mCarryState) {
        case 1:
            anm_init(i_this, dRes_INDEX_BB_BCK_FLYC_e, 1.0f, 2, l_bbHIO.unk_4C, dRes_INDEX_BB_BAS_FLYC_e);
            i_this->mCarryState = 2;
            break;
        case 2: {
            fopAc_ac_c* ac = bbSearchByIdCopy(i_this->mCarriedRequestId);
            if (ac != nullptr) {
                i_this->mpCarriedActor = ac;
                i_this->mCarryState = 3;
            }
            break;
        }
        case 3: {
            f32 x0 = i_this->mFootPos[0].x, y0 = i_this->mFootPos[0].y, z0 = i_this->mFootPos[0].z;
            f32 px = gabi::fmadds(i_this->mFootPos[1].x - x0, 0.5f, x0);
            f32 py = gabi::fmadds(i_this->mFootPos[1].y - y0, 0.5f, y0);
            f32 pz = gabi::fmadds(i_this->mFootPos[1].z - z0, 0.5f, z0);
            fopAc_ac_c* ac = i_this->mpCarriedActor;
            ac->current.pos.x = px;
            ac->current.pos.y = py;
            ac->current.pos.z = pz;
            ac->current.angle.x = (s16)i_this->current.angle.x;
            ac->current.angle.y = (s16)i_this->current.angle.y;
            ac->current.angle.z = (s16)i_this->current.angle.z;
            ac->shape_angle.x = (s16)i_this->current.angle.x;
            ac->shape_angle.y = (s16)i_this->current.angle.y;
            ac->shape_angle.z = (s16)i_this->current.angle.z;

            BbDamageReaction* dr;
            if (i_this->mCarriedProcName == fpcNm_MO2_e) {
                /* mo2_class::mDamageReaction at 0x8BC: mCarryOffsetY/mCarryOffsetZ */
                dr = &static_cast<BbCarriedMo2*>(ac)->mDamageReaction;
                dr->mCarryOffsetY = gabi::fmadds(REG0_F(8), 10.0f, -110.0f);
                dr->mCarryOffsetZ = gabi::fmadds(REG0_F(9), 10.0f, 10.0f);
            } else if (i_this->mCarriedProcName == fpcNm_BK_e) {
                /* bk_class::dr at 0x49C */
                dr = &static_cast<BbCarriedBk*>(ac)->dr;
                dr->mCarryOffsetY = gabi::fmadds(REG0_F(8), 10.0f, -100.0f);
                dr->mCarryOffsetZ = REG0_F(9) * 10.0f;
            } else {
                break;
            }
            if (i_this->mDropCarried != 0) {
                i_this->mCarryState = 0;
                anm_init(i_this, dRes_INDEX_BB_BCK_FLY02_e, 12.0f, 2, 1.0f, dRes_INDEX_BB_BAS_FLY02_e);
                dr->mAction = 31; /* dr->mAction */
                ac->speedF = 40.0f;
            }
            break;
        }
        }
    }

    i_this->mHeadTgSph.SetC(&i_this->eyePos);
    i_this->mBodyTgSph.SetC(&i_this->current.pos);
    i_this->mBodyCoSph.SetC(&i_this->current.pos);

    dComIfG_Ccsp_Set(&i_this->mHeadTgSph);
    dComIfG_Ccsp_Set(&i_this->mBodyTgSph);
    dComIfG_Ccsp_Set(&i_this->mBodyCoSph);

    if (i_this->mNoBgCheck == 0) {
        i_this->current.pos.y = i_this->current.pos.y - (REG0_F(5) + 70.0f);
        i_this->old.pos.y = i_this->old.pos.y - (REG0_F(5) + 70.0f);
        dBgS_Acch_CrrPos(&i_this->mAcch, dComIfG_Bgsp());
        i_this->current.pos.y = i_this->current.pos.y + (REG0_F(5) + 70.0f);
        i_this->old.pos.y = i_this->old.pos.y + (REG0_F(5) + 70.0f);
    }

    i_this->mNoBgCheck = 0;
    bb_water_check(i_this);

    for (s32 i = 0; i < 11; i++) {
        i_this->mJointRot[i].z = 0;
        i_this->mJointRot[i].y = 0;
        i_this->mJointRot[i].x = 0;
    }

    if (i_this->mBodyShakeAmp > 0.1f) {
        f32 tmp = i_this->mBodyShakeAmp;
        if (tmp > 4000.0f) {
            tmp = 4000.0f;
        }
        for (s32 i = 0; i < 11; i++) {
            csXyz* c = &i_this->mJointRot[i];
            f32 s = bbSinShortCopy(bbTrigTable(), (s16)(i_this->mFrameCounter * (REG0_S(0) + 6000) + i * (REG0_S(1) + 13000)));
            c->x = (s16)gabi::ftoi(gabi::fmadds(s, tmp, (f32)(s32)c->x));
            s = bbSinShortCopy(bbTrigTable(), (s16)(i_this->mFrameCounter * (REG0_S(3) + 7000) + i * (REG0_S(4) + 18000)));
            f32 t = s * tmp;
            c->y = (s16)gabi::ftoi((f32)(s32)c->y + (t + t));
            s = bbCosShortCopy(bbTrigTable(), (s16)(i_this->mFrameCounter * (REG0_S(6) + 6500) + i * (REG0_S(7) + 24000)));
            c->z = (s16)gabi::ftoi(gabi::fmadds(s * tmp, 3.0f, (f32)(s32)c->z));
        }
        cLib_addCalc0(&i_this->mBodyShakeAmp, 1.0f, gabi::fmadds(REG0_F(16), 10.0f, 120.0f));
    }

    s16 atan = cM_atan2s(player->current.pos.x - i_this->eyePos.x, player->current.pos.z - i_this->eyePos.z);
    cMtx_YrotS(calc_mtx(), (s16)(i_this->current.angle.y - atan));

    {
        cXyz sp28;
        cXyz sp1C;
        sp28.y = 0.0f;
        sp28.z = i_this->mHeadShakeAmp;
        sp28.x = 0.0f;
        MtxPosition(&sp28, &sp1C);

        s16 dz = (s16)gabi::ftoi(sp1C.z);
        i_this->mJointRot[8].z = (s16)(i_this->mJointRot[8].z + dz);
        i_this->mJointRot[9].z = (s16)(i_this->mJointRot[9].z + dz);
        f32 sx = sp1C.x;
        i_this->mJointRot[8].y = (s16)(i_this->mJointRot[8].y + (s16)gabi::ftoi(sx * (REG0_F(10) + 1.0f)));
        i_this->mJointRot[9].y = (s16)(i_this->mJointRot[9].y + (s16)gabi::ftoi(sx * (REG0_F(10) + 1.0f)));
        i_this->mJointRot[10].y = (s16)(i_this->mJointRot[10].y + (s16)gabi::ftoi(sx * (REG0_F(10) + 1.0f)));
    }

    cLib_addCalc0(&i_this->mHeadShakeAmp, 0.1f, (REG0_F(12) + 400.0f) * 100.0f);

    damage_check(i_this);

    if (i_this->mHeadShakeAmp > 1.0f) {
        i_this->mHeadLookMode = 3;
    }

    s16 tmp2 = 0x800;
    if (i_this->mHeadLookMode != 0) {
        if (i_this->mHeadLookMode == 1) {
            s16 v = (s16)(i_this->current.angle.y - atan);
            if (v > 0x3A98) {
                i_this->mHeadTargetAngleY = 0x3A98;
            } else if (v < -0x3A98) {
                i_this->mHeadTargetAngleY = -0x3A98;
            } else {
                i_this->mHeadTargetAngleY = v;
            }
        } else if (i_this->mHeadLookMode == 2) {
            if ((i_this->mFrameCounter & 0xF) == 0 && cM_rndF(1.0f) < 0.4f) {
                i_this->mHeadTargetAngleY = (s16)gabi::ftoi(cM_rndFX(15000.0f));
                tmp2 = 0x200;
            }
            if ((i_this->mFrameCounter & 0x3F) == 0 && cM_rndF(1.0f) < 0.5f) {
                kuti_open(i_this, 0x30, JA_SE_CV_BB_NORMAL);
            }
        } else if (i_this->mHeadLookMode == 3) {
            f32 s = cM_ssin(i_this->mFrameCounter * (REG0_S(5) + 9000));
            i_this->mHeadTargetAngleY = (s16)gabi::ftoi(s * i_this->mHeadShakeAmp * 5.0f);
            tmp2 = 0x2000;
        }
        i_this->mHeadLookMode = 0;
    } else {
        i_this->mHeadTargetAngleY = 0;
    }

    cLib_addCalcAngleS2(&i_this->mHeadAngleY, i_this->mHeadTargetAngleY, 2, tmp2);

    i_this->mJointRot[10].x = (s16)((i_this->mJointRot[10].x - i_this->mHeadAngleY) / 2);
    i_this->mJointRot[9].x = (s16)((i_this->mJointRot[9].x - i_this->mHeadAngleY) / 2);

    tmp2 = 0;
    if (i_this->mBeakOpenTimer != 0) {
        i_this->mBeakOpenTimer = (s16)(i_this->mBeakOpenTimer - 1);
        tmp2 = 0x2000;
        if (i_this->mBeakOpenTimer == i_this->mBeakSeFrame && i_this->mBeakSeId != 0) {
            fopAcM_monsSeStart(i_this, i_this->mBeakSeId, 0);
        }
    }

    cLib_addCalcAngleS2(&i_this->mBeakOpenAngle, tmp2, 3, 0x2000);

    s16 div = i_this->mBeakOpenAngle / 8;
    i_this->mJointRot[8].z = (s16)(i_this->mJointRot[8].z + div);
    i_this->mJointRot[9].z = (s16)(i_this->mJointRot[9].z + div);
    i_this->mJointRot[10].z = (s16)(i_this->mJointRot[10].z + div);

    bb_eye_tex_anm(i_this);

    return TRUE;
}

/* WWHD glue
str 10007C9C in 0205EA58
str 10007CA8 in 0205EA58
ext 020402C8 BOOL enemy_ice(enemyice* ice)
ext 02041570 void enemy_fire(enemyfire* fire)
ext 02062584 fopAc_ac_c* bbSearchByIdCopy(u32 id)
ext 020625C0 f32 bbSinShortCopy(void* table, s16 angle)
ext 02063474 f32 bbCosShortCopy(void* table, s16 angle)
*/

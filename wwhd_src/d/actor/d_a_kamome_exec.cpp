/**
 * d_a_kamome.cpp (WWHD), part: daKamome_Execute and the move functions GHS inlined into it
 * (heisou_control, ko_check, kamome_heisou_move, search_master, kamome_path_move,
 * kamome_imouto_move, search_imouto, kamome_imouto2_move).
 *
 * Guest bindings preserve calls across verified parts.
 *
 * Float comparisons: GHS sometimes branches on the negated condition (`bgt skip` for `a <= b`),
 * which differs from C for NaN. Those are written as !(a > b) etc.
 */
#include "d/actor/d_a_kamome.h"

/* ---- functions of d_a_kamome.cpp (guest calls, same signatures) ---- */
static void anm_init(kamome_class* i_this, int a, float m, unsigned char l, float s, int snd) {
    kamomeExecute_animation(i_this, a, m, l, s, snd);
}
static fopAc_ac_c* search_esa(kamome_class* i_this) { return kamomeExecute_bait(i_this); }
static void kamome_pos_move(kamome_class* i_this) { kamomeExecute_flight(i_this); }
static void kamome_bgcheck(kamome_class* i_this) { kamomeExecute_background(i_this); }
static void kamome_ground_pos_move(kamome_class* i_this) { kamomeExecute_groundMove(i_this); }
static void daKamome_setMtx(kamome_class* i_this) { kamomeExecute_matrix(i_this); }
static void kamome_auto_move(kamome_class* i_this) { kamomeExecute_autoMove(i_this); }

/* ---- more bindings ---- */

static inline s32 McaMorf_getFrame(mDoExt_McaMorf* m) { return gabi::ftoi(m->getFrame()); }
/* J3DFrameCtrl: rate +0x98, state +0xA7 */
static inline bool McaMorf_isStop(mDoExt_McaMorf* m) {
    return m->isStop();
}
// HD: cXyz::operator- is out of line: (this, result, other)
/* cXyz::abs() of a difference, as HD computes it: a - b into a temporary, copied, squared mag */
/* inlined into daKamome_Execute */
static inline f32 diff_abs(cXyz* a, cXyz* b) {
    cXyz res;
    res.set(0.0f, 0.0f, 0.0f); /* the harness compares the (uninitialised) result buffer: start from a fresh slot */
    cXyz_mi(a, &res, b);
    cXyz tmp;
    tmp.copy(res);
    return std_sqrtf(PSVECSquareMag(&tmp));
}
/* cM_ssin: sin/cos table (8 bytes per entry) at 0x104A44F8 */

static inline be<s16>& timer(kamome_class* i_this, int i) { return i_this->mTimers[i]; }

/* dPath / dPnt (unchanged from GameCube) */


static inline dPath_l* path(kamome_class* i_this) { return (dPath_l*)(dPath*)i_this->mpPath; }

/* l_kamomeHIO (0x104648C8) */



/* dPath_GetNext: the stage's path table through dStage_stageDt_c::getPathInf() (virtual) */
static inline void path_next_point(kamome_class* i_this) {
    s8 idx = (s8)(i_this->mCurPointIdx + i_this->mPathIdxIncr);
    i_this->mCurPointIdx = idx;
    dPath_l* p = path(i_this);
    if (idx >= (s8)(u8)p->m_num) {
        u16 nextID;
        if (p->m_closed & 1) {
            i_this->mCurPointIdx = 0;
            nextID = path(i_this)->m_nextID;
        } else {
            i_this->mPathIdxIncr = -1;
            i_this->mCurPointIdx = (s8)(path(i_this)->m_num - 2);
            nextID = p->m_nextID;
        }
        if (nextID != 0xFFFF) {
            i_this->mpPath = kamomeNextPath(nextID);
        }
    } else if (idx < 0) {
        i_this->mPathIdxIncr = 1;
        i_this->mCurPointIdx = 1;
    }
}

static inline dPnt_l* cur_point(kamome_class* i_this) {
    return kamomePathPoint(path(i_this), i_this->mCurPointIdx);
}

/* fall back to the ground after landing (case 0x14 of the move functions) */
static inline void landing(kamome_class* i_this, f32 groundSpeedY, s8 nextState) {
    fopAc_ac_c* a_this = &i_this->actor;
    s32 frame = McaMorf_getFrame(i_this->mpMorf);
    a_this->current.pos.y += a_this->speed.y;
    if (frame > REG_S(6, 2) + 0xf) {
        a_this->speed.y -= REG_F(6, 7) + 0.8f;
        cLib_addCalcAngleS2(&a_this->current.angle.y, i_this->m2FC, 2, 1000);
    }
    cLib_addCalcAngleS2(&a_this->current.angle.x, 0, 5, 0x800);
    cLib_addCalcAngleS2(&a_this->current.angle.z, 0, 5, 0x800);
    kamome_bgcheck(i_this);

    if (i_this->mAcch.ChkGroundHit()) {
        a_this->speed.y = groundSpeedY;
    } else {
        cLib_addCalc2(&a_this->current.pos.x, i_this->mTargetPos.x, 0.1f, std::fabs((f32)a_this->speed.x));
        cLib_addCalc2(&a_this->current.pos.z, i_this->mTargetPos.z, 0.1f, std::fabs((f32)a_this->speed.z));
    }

    if (McaMorf_isStop(i_this->mpMorf)) {
        anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT3_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, 0);
    }

    if (timer(i_this, 2) == 0) {
        i_this->mMoveState = nextState;
    }
}

/* start landing when close to the target (case 10) */
static inline bool start_landing(kamome_class* i_this, f32 dist) {
    fopAc_ac_c* a_this = &i_this->actor;
    if (dist < gabi::fmadds(REG_F(6, 9), 10.0f, 300.0f)) {
        i_this->mMoveState = 0x14;
        anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_LAND1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_KAMOME_BAS_KA_LAND1_e);
        i_this->mAnimState = 10;
        a_this->speed.y = 0.0f;
        s16 r = (s16)gabi::ftoi(cM_rndFX(15000.0f));
        i_this->m2FC = a_this->current.angle.y + r;
        timer(i_this, 2) = 0x3c;
        return true;
    }
    return false;
}

static inline f32 dist3(kamome_class* i_this, f32 dy) {
    fopAc_ac_c* a_this = &i_this->actor;
    f32 x = i_this->mTargetPos.x - a_this->current.pos.x;
    f32 z = i_this->mTargetPos.z - a_this->current.pos.z;
    return std_sqrtf(gabi::fmadds(z, z, gabi::fmadds(x, x, dy * dy)));
}

static inline f32 dist2(kamome_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    f32 x = i_this->mTargetPos.x - a_this->current.pos.x;
    f32 z = i_this->mTargetPos.z - a_this->current.pos.z;
    return std_sqrtf(gabi::fmadds(x, x, z * z));
}

/* the shared animation states 0..2 */
static inline void anim_wait_sing(kamome_class* i_this, int animState, bool pathMove) {
    fopAc_ac_c* a_this = &i_this->actor;
    switch (animState) {
    case 0: {
        s32 frame = McaMorf_getFrame(i_this->mpMorf);
        bool go = pathMove ? (a_this->current.pos.y > i_this->mTargetPos.y && frame == REG0_S(0) + 9)
                           : (timer(i_this, 0) == 0 && frame == REG0_S(0) + 9);
        if (go) {
            i_this->mAnimState = 1;
            if (pathMove) timer(i_this, 0) = (s16)gabi::ftoi(cM_rndF(200.0f) + 50.0f);
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT1_e, REG0_F(0) + 12.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KAMOME_BAS_KA_WAIT1_e);
        }
        break;
    }
    case 1:
        if ((i_this->mGlobalTimer & 0x3f) == 0 && cM_rndF(1.0f) < 0.5f) {
            i_this->mGlobalTimer = (s16)gabi::ftoi(cM_rndF(10000.0f));
            i_this->mAnimState = 2;
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_SING1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, 0);
        } else if (pathMove) {
            if (!(a_this->current.pos.y > i_this->mTargetPos.y)) {
                i_this->mAnimState = 0;
                anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT2_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KAMOME_BAS_KA_WAIT2_e);
            }
        } else if (timer(i_this, 0) == 0 && a_this->current.pos.y < i_this->mTargetPos.y) {
            i_this->mAnimState = 0;
            timer(i_this, 0) = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT2_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KAMOME_BAS_KA_WAIT2_e);
        }
        break;
    case 2:
        if (McaMorf_getFrame(i_this->mpMorf) == 8) {
            fopAcM_seStart(a_this, JA_SE_CV_KAMOME, 0);
        }
        if (McaMorf_isStop(i_this->mpMorf)) {
            i_this->mAnimState = 1;
            if (pathMove) timer(i_this, 0) = (s16)gabi::ftoi(cM_rndF(100.0f) + 20.0f);
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KAMOME_BAS_KA_WAIT1_e);
        }
        break;
    }
}

/* inlined: heisou_control (mType 6, "escort": spawns followers while Link rides the boat) */
/* inlined into daKamome_Execute */
static inline void heisou_control(kamome_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->mbNoDraw = 1;
    f32 t = dComIfGs_getTime();
    if (!(t < 90.0f) && !(t > 270.0f) && dKy_rain_check() < 10) {
        if (dComIfGp_checkPlayerStatus0(0, daPyStts0_SHIP_RIDE_e) && (i_this->mGlobalTimer & 0x7f) == 0) {
            /* ko_check */
            ko_count = 0;
            fpcM_Search(KAMOME_CHILD_SEARCH, &i_this->actor);
            s32 ko = ko_count;
            if (ko < i_this->mKoMaxCount) {
                cXyz sp08;
                auto p = player->current.pos.get();
                sp08 = p;
                sp08.y = p.y + 4000.0f;
                fopAcM_create(fpcNm_KAMOME_e, (u32)ko << 8 | 0xffff0007, &sp08, fopAcM_GetRoomNo(&i_this->actor), nullptr,
                              nullptr, -1, 0);
            }
        }
    }
}

/* inlined: kamome_heisou_move (mType 7: flies along with Link's boat) */
/* inlined into daKamome_Execute */
static inline void kamome_heisou_move(kamome_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    anim_wait_sing(i_this, i_this->mAnimState, false);

    i_this->m2B4++;

    if (i_this->mMoveState == 0) {
        s16 c = i_this->m2B4;
        cXyz sp44;
        f32 s1 = cM_ssin(c * (REG_S(17, 5) + 200));
        f32 s2 = cM_ssin(c * (REG_S(17, 6) + 30));
        sp44.x = s1 * (REG_F(17, 5) + 300.0f);
        sp44.y = i_this->m2B8 + 600.0f + REG_F(17, 6);
        sp44.z = gabi::fmadds(s2, REG_F(17, 7) + 300.0f, REG_F(17, 4) + 150.0f);
        cMtx_YrotS(calc_mtx(), player->shape_angle.y);
        cXyz sp34;
        MtxPosition(&sp44, &sp34);
        f32 tx = sp34.x + player->current.pos.x;
        i_this->mTargetPos.x = tx;
        f32 tz = sp34.z + player->current.pos.z;
        i_this->mTargetPos.z = tz;
        if (daSea_ChkArea(tx, tz)) {
            f32 w = daSea_calcWave(i_this->mTargetPos.x, i_this->mTargetPos.z);
            i_this->mTargetPos.y = sp34.y + w;
        } else {
            i_this->mTargetPos.y = player->current.pos.y + sp34.y;
        }
    }

    BOOL iVar2 = FALSE;
    f32 t = dComIfGs_getTime();
    if (!(t < 90.0f) && !(t > 270.0f) && dKy_rain_check() < 10 && !dComIfGp_att_chkEnemySound() &&
        dComIfGp_checkPlayerStatus0(0, daPyStts0_SHIP_RIDE_e)) {
        /* search_master */
        kamome_class* master = (kamome_class*)fpcM_Search(KAMOME_MASTER_SEARCH, a_this);
        if (master != nullptr && i_this->mKoMaxCount < master->mKoMaxCount) iVar2 = TRUE;
    }

    f32 fVar6;
    i_this->mVelocityFwdTarget = diff_abs(&player->old.pos, &player->current.pos) * (REG_F(17, 8) + 1.2f);
    if (i_this->mVelocityFwdTarget > REG_F(17, 13) + 30.0f && iVar2) {
        fVar6 = 0.0f;
        if (diff_abs(&i_this->mTargetPos, &a_this->current.pos) < REG_F(17, 9) + 200.0f) {
            i_this->mVelocityFwdTarget = 0.0f;
        }
    } else {
        fVar6 = a_this->home.pos.y;
        i_this->mVelocityFwdTarget = 20.0f;
    }

    cLib_addCalc2(&i_this->m2B8, fVar6, 1.0f, 20.0f);
    i_this->mVelocityFwdTargetMaxVel = REG_F(17, 10) + 0.25f;
    i_this->mRotVel = REG_F(17, 11) + 300.0f;
    kamome_pos_move(i_this);
    s16 ang = fopAcM_searchActorAngleY(a_this, dComIfGp_getPlayer(0));
    i_this->mJointRotYTarget = (s16)(ang - a_this->current.angle.y);

    s16 sVar3 = REG_S(17, 3) + 5000;
    if (i_this->mJointRotYTarget > sVar3) {
        i_this->mJointRotYTarget = sVar3;
    } else if (i_this->mJointRotYTarget < (s16)-sVar3) {
        i_this->mJointRotYTarget = -sVar3;
    }

    if (a_this->current.pos.y > 3000.0f) {
        i_this->mbNoDraw = 1;
    }
}

/* inlined: kamome_path_move */
static inline void kamome_path_move(kamome_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    s8 moveType = 0;
    dComIfGp_get(); // HD: unused player lookup

    switch (i_this->mAnimState) {
    case 0:
    case 1:
    case 2:
        anim_wait_sing(i_this, i_this->mAnimState, true);
        break;
    case 20:
        if (McaMorf_isStop(i_this->mpMorf)) {
            i_this->mAnimState = 0;
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT2_e, 0.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KAMOME_BAS_KA_WAIT2_e);
        }
        break;
    }

    switch (i_this->mMoveState) {
    case 0:
        path_next_point(i_this);
        /* fallthrough */
    case -1: {
        i_this->mMoveState = 1;
        dPnt_l* pnt = cur_point(i_this);
        if (i_this->m308 != 0) {
            i_this->m308 = 0;
            i_this->mVelocityFwdTarget = 25.0f;
            i_this->mVelocityFwdTargetMaxVel = 2.0f;
        } else if (a_this->current.pos.y < i_this->mTargetPos.y) {
            i_this->mVelocityFwdTarget = cM_rndF(5.0f) + 20.0f;
            i_this->mVelocityFwdTargetMaxVel = REG0_F(11) + 1.0f;
        } else {
            i_this->mVelocityFwdTarget = cM_rndF(5.0f) + 30.0f;
            i_this->mVelocityFwdTargetMaxVel = REG0_F(13) + 1.0f;
        }
        i_this->mRotVelFade = REG0_F(7);
        if (pnt->mArg3 == 1) {
            i_this->mMoveState = 10;
            i_this->mTargetPos = pnt->m_position.get();
        } else {
            f32 r = cM_rndFX(150.0f);
            i_this->mTargetPos.x = pnt->m_position.x + r;
            r = cM_rndFX(150.0f);
            i_this->mTargetPos.y = pnt->m_position.y + r;
            r = cM_rndFX(150.0f);
            i_this->mTargetPos.z = pnt->m_position.z + r;
        }

        if (timer(i_this, 3) == 0) {
            fopAc_ac_c* esa = search_esa(i_this);
            if (esa != nullptr) {
                i_this->mEsaProcID = fopAcM_GetID(esa);
                i_this->mMoveState = 10;
                i_this->mRotVel = 1000.0f;
                i_this->mbUsePathMovement = 0;
            }
        }
        break;
    }

    case 1: {
        cLib_addCalc2(&i_this->mRotVelFade, 1.0f, 1.0f, 0.04f);
        f32 y = i_this->mTargetPos.y - a_this->current.pos.y;
        f32 sqrt = dist3(i_this, y);
        f32 rv = gabi::fmadds(REG0_F(5), 10.0f, 100.0f) + gabi::fmadds(REG0_F(6), 1000.0f, 500000.0f) / sqrt;
        i_this->mRotVel = rv;
        f32 lim = gabi::fmadds(REG0_F(4), 10.0f, 5000.0f);
        if (rv > lim) {
            i_this->mRotVel = lim;
        }

        if (sqrt < gabi::fmadds(REG0_F(10), 10.0f, 300.0f)) {
            dPnt_l* pnt = cur_point(i_this);
            i_this->mMoveState = 0;
            if (pnt->mArg3 == 6) {
                fopAcM_delete(a_this);
            }
        }
        break;
    }

    case 10: {
        i_this->mVelocityFwdTarget = 20.0f;
        f32 y = gabi::fmadds(REG_F(6, 8), 10.0f, i_this->mTargetPos.y + 50.0f) - a_this->current.pos.y;
        if (start_landing(i_this, dist3(i_this, y))) moveType = -1;
        break;
    }

    case 0x14:
        moveType = -1;
        landing(i_this, -0.5f, 0x17);
        break;

    case 0x16:
        moveType = 1;
        i_this->m2AB = 1;
        if (timer(i_this, 2) == 0) {
            path_next_point(i_this);
            dPnt_l* pnt = cur_point(i_this);
            i_this->mTargetPos = pnt->m_position.get();
            if (pnt->mArg3 == 2) {
                i_this->mMoveState = 0x19;
                anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_FLY1_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_KAMOME_BAS_KA_FLY1_e);
            } else {
                i_this->mMoveState = 0x17;
                i_this->mRotVel = 2500.0f;
                i_this->mRotVelFade = 1.0f;
                anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_MOVE1_e, 3.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KAMOME_BAS_KA_MOVE1_e);
            }
        }
        i_this->mVelocityFwdTarget = 0.0f;
        a_this->speedF = 0.0f;
        break;

    case 0x13:
        moveType = 1;
        if (McaMorf_isStop(i_this->mpMorf)) {
            i_this->mMoveState = 0x16;
        }
        i_this->mVelocityFwdTarget = 0.0f;
        a_this->speedF = 0.0f;
        break;

    case 0x17: {
        moveType = 1;
        s32 frame = McaMorf_getFrame(i_this->mpMorf);
        if (frame >= l_kamomeHIO.m06 && frame <= l_kamomeHIO.m08) {
            f32 v = 5.0f * i_this->mScale;
            a_this->speedF = v;
            i_this->mVelocityFwdTarget = v;
        } else {
            i_this->mVelocityFwdTarget = 0.0f;
            a_this->speedF = 0.0f;
            if (dist2(i_this) < 50.0f) {
                dPnt_l* pnt = cur_point(i_this);
                if (cM_rndF(1.0f) < 0.5f) {
                    i_this->mMoveState = 0x13;
                    anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_SING2_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, 0);
                } else {
                    i_this->mMoveState = 0x16;
                    if (pnt->mArg3 == 3) {
                        anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT3_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, 0);
                        timer(i_this, 2) = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
                    }
                }
            }
        }
        break;
    }

    case 0x19:
        moveType = 1;
        a_this->speed.y = 0.0f;
        if (McaMorf_getFrame(i_this->mpMorf) < 5) {
            i_this->mVelocityFwdTarget = 0.0f;
            i_this->mVelocityFwdTargetMaxVel = 1.0f;
        } else {
            i_this->mMoveState = -1;
            i_this->mRotVel = gabi::fmadds(REG0_F(4), 10.0f, 5000.0f);
            i_this->mVelocityFwdTarget = 25.0f;
            i_this->mVelocityFwdTargetMaxVel = 2.0f;
            a_this->speedF = 0.0f;
            i_this->mAnimState = 20;
            i_this->mRiseTimer = REG0_S(4) + 10;
        }
        break;
    }

    switch (moveType) {
    case 0:
        kamome_pos_move(i_this);
        break;
    case 1:
        kamome_ground_pos_move(i_this);
        break;
    }
}

/* dBgS_GndChk (stack object, HD layout) */




/* back to free flight after being disturbed on the ground (shared by the ground states) */
/* inlined into daKamome_Execute */
static inline void take_off(kamome_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    i_this->mMoveState = 0;
    i_this->mAnimState = 0;
    timer(i_this, 0) = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
    anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT2_e, 3.0f, J3DFrameCtrl::EMode_LOOP, 1.5f, dRes_INDEX_KAMOME_BAS_KA_WAIT2_e);

    if (i_this->mPathIdx != 0xff) {
        i_this->mbUsePathMovement = i_this->mPathIdx + 1;
        timer(i_this, 3) = (s16)gabi::ftoi(cM_rndF(250.0f) + 250.0f);
        i_this->m308 = 1;
    } else {
        timer(i_this, 1) = 0x32;
        i_this->mRotVel = gabi::fmadds(REG0_F(4), 10.0f, 5000.0f);
        i_this->mVelocityFwdTarget = 30.0f;
        i_this->mVelocityFwdTargetMaxVel = 3.0f;
        a_this->speedF = 0.0f;
        MtxTrans(a_this->current.pos.x, a_this->current.pos.y, a_this->current.pos.z, 0);
        cXyz sp34;
        sp34.set(0.0f, 1000.0f, 2000.0f);
        cMtx_YrotM(calc_mtx(), a_this->current.angle.y);
        MtxPosition(&sp34, &i_this->mTargetPos);
        timer(i_this, 3) = (s16)gabi::ftoi(cM_rndF(250.0f) + 250.0f);
    }
}

/* inlined: kamome_imouto_move (mType 4: Aryll's seagull) */
/* inlined into daKamome_Execute */
static inline void kamome_imouto_move(kamome_class* i_this) {
    fopAc_ac_c* a_this = &i_this->actor;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s8 cVar8 = 0;

    switch (i_this->mAnimState) {
    case 0:
    case 1:
    case 2:
        anim_wait_sing(i_this, i_this->mAnimState, false);
        break;
    case 20:
        if (McaMorf_isStop(i_this->mpMorf)) {
            i_this->mAnimState = 0;
            timer(i_this, 0) = (s16)gabi::ftoi(cM_rndF(60.0f) + 20.0f);
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT2_e, 0.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KAMOME_BAS_KA_WAIT2_e);
        }
        break;
    }

    switch (i_this->mMoveState) {
    case 0:
        if (timer(i_this, 1) == 0) {
            f32 r = cM_rndFX(1000.0f);
            f32 fVar11 = (a_this->home.pos.x + r) - a_this->current.pos.x;
            r = cM_rndFX(1000.0f);
            f32 fVar9 = (a_this->home.pos.z + r) - a_this->current.pos.z;
            if (std_sqrtf(gabi::fmadds(fVar11, fVar11, fVar9 * fVar9)) > 200.0f) {
                timer(i_this, 1) = (s16)gabi::ftoi(cM_rndF(150.0f) + 50.0f);
                i_this->mTargetPos.x = fVar11 + a_this->current.pos.x;
                r = cM_rndF(500.0f);
                f32 ty = a_this->home.pos.y + r;
                i_this->mTargetPos.y = ty;
                i_this->mTargetPos.z = fVar9 + a_this->current.pos.z;
                i_this->mRotVelFade = 0.0f;

                if (a_this->current.pos.y < ty) {
                    i_this->mVelocityFwdTarget = REG0_F(10) + 20.0f;
                    i_this->mVelocityFwdTargetMaxVel = REG0_F(11) + 0.2f;
                } else {
                    i_this->mVelocityFwdTarget = REG0_F(12) + 36.0f;
                    i_this->mVelocityFwdTargetMaxVel = REG0_F(13) + 0.5f;
                }

                i_this->mRotVel = cM_rndF(300.0f) + 200.0f;

                if (timer(i_this, 3) == 0) {
                    fopAc_ac_c* imouto = (fopAc_ac_c*)fpcM_Search(KAMOME_ARYLL_SEARCH, a_this); /* search_imouto */
                    i_this->mpTargetActor = imouto;
                    if (imouto != nullptr) {
                        if (diff_abs(&player->current.pos, &imouto->current.pos) < l_kamomeHIO.m10) {
                            timer(i_this, 3) = 0x32;
                            i_this->mpTargetActor = nullptr;
                        } else {
                            fopAc_ac_c* t = i_this->mpTargetActor;
                            r = cM_rndFX(150.0f);
                            i_this->mTargetPos.x = t->current.pos.x + r;
                            t = i_this->mpTargetActor;
                            i_this->mTargetPos.y = t->current.pos.y + 80.0f;
                            r = cM_rndFX(150.0f);
                            i_this->mTargetPos.z = t->current.pos.z + r;
                            // HD: only lands next to Aryll when she is on the ground
                            dBgS_GndChk_l chk;
                            kamomeExecute_groundCtor(&chk);
                            kamomeInitLandingCheck(chk);
                            chk.m_pos.copy(i_this->mTargetPos);
                            f32 gnd = kamomeExecute_groundCross(dComIfG_Bgsp(), &chk);
                            if (i_this->mpTargetActor->current.pos.y < gnd + 10.0f) {
                                i_this->mMoveState = 10;
                                i_this->mRotVel = 1000.0f;
                                i_this->mTargetPos.y = i_this->mTargetPos.y - 20.0f;
                            }
                            ground_chk_dt(&chk);
                        }
                    }
                }
            }
        }
        break;

    case 10:
        if (diff_abs(&player->current.pos, &i_this->mTargetPos) < l_kamomeHIO.m10) {
            i_this->mMoveState = 0;
            timer(i_this, 3) = 0x32;
        } else {
            f32 y = i_this->mTargetPos.y - a_this->current.pos.y;
            f32 x = i_this->mTargetPos.x - a_this->current.pos.x;
            f32 z = i_this->mTargetPos.z - a_this->current.pos.z;
            f32 sq = gabi::fmadds(z, z, gabi::fmadds(x, x, y * y));
            i_this->mVelocityFwdTarget = 20.0f;
            if (start_landing(i_this, std_sqrtf(sq))) cVar8 = -1;
        }
        break;

    case 0x14:
        cVar8 = -1;
        landing(i_this, -1.0f, 0x16);
        break;

    case 0x15:
        cVar8 = 1;
        i_this->mMoveState = 0x16;
        break;

    case 0x16:
        cVar8 = 1;
        i_this->mVelocityFwdTarget = 0.0f;
        i_this->mRotVel = 0.0f;
        i_this->m2AB = 1;
        a_this->speedF = 0.0f;

        if (timer(i_this, 2) == 0) {
            i_this->mMoveState = 0x17;
            f32 m = REG0_F(8) + 150.0f;
            fopAc_ac_c* t = i_this->mpTargetActor;
            f32 r = cM_rndFX(m);
            i_this->mTargetPos.x = t->current.pos.x + r;
            m = REG0_F(8) + 150.0f;
            t = i_this->mpTargetActor;
            r = cM_rndFX(m);
            i_this->mTargetPos.z = t->current.pos.z + r;
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_MOVE1_e, 3.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, dRes_INDEX_KAMOME_BAS_KA_MOVE1_e);
        }
        break;

    case 0x13:
        cVar8 = 1;
        if (McaMorf_isStop(i_this->mpMorf)) {
            i_this->mMoveState = 0x15;
            timer(i_this, 2) = (s16)gabi::ftoi(cM_rndF(30.0f) + 20.0f);
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT3_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, 0);
        }
        i_this->mVelocityFwdTarget = 0.0f;
        a_this->speedF = 0.0f;
        break;

    case 0x17: {
        cVar8 = 1;
        s32 frame = McaMorf_getFrame(i_this->mpMorf);
        if ((frame >= l_kamomeHIO.m06) && (frame <= l_kamomeHIO.m08)) {
            f32 v = 5.0f * i_this->mScale;
            i_this->mVelocityFwdTarget = v;
            i_this->mRotVel = 4000.0f;
            a_this->speedF = v;
            i_this->mRotVelFade = 1.0f;
        } else {
            i_this->mVelocityFwdTarget = 0.0f;
            i_this->mRotVel = 0.0f;
            a_this->speedF = 0.0f;
            if (dist2(i_this) < 85.0f || (i_this->mAcch.m_flags & 0x10) || i_this->mSph.ChkCoHit()) {
                anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_EAT1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, 0);
                i_this->mMoveState = 0x18;
            }
        }
        break;
    }

    case 0x18:
        cVar8 = 1;
        i_this->mVelocityFwdTarget = 0.0f;
        a_this->speedF = 0.0f;
        if (McaMorf_isStop(i_this->mpMorf)) {
            if (cM_rndF(1.0f) < 0.2f) {
                i_this->mMoveState = 0x13;
                anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_SING2_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, 0);
            } else {
                i_this->mMoveState = 0x15;
                timer(i_this, 2) = (s16)gabi::ftoi(cM_rndF(20.0f));
                anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT3_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, 0);
            }
        }
        break;

    case 0x19:
        cVar8 = 1;
        a_this->speed.y = 0.0f;
        if (McaMorf_getFrame(i_this->mpMorf) < 5) {
            i_this->mVelocityFwdTarget = 0.0f;
            i_this->mVelocityFwdTargetMaxVel = 1.0f;
        } else {
            i_this->mMoveState = 0;
            i_this->mAnimState = 20;
            timer(i_this, 3) = (s16)gabi::ftoi(cM_rndF(100.0f) + 100.0f);
            i_this->mRiseTimer = REG0_S(4) + 10;
            if (i_this->mPathIdx != 0xff) {
                i_this->mbUsePathMovement = i_this->mPathIdx + 1;
                i_this->m308 = 1;
            } else {
                timer(i_this, 1) = 0x32;
                i_this->mRotVel = gabi::fmadds(REG0_F(4), 10.0f, 1000.0f);
                i_this->mVelocityFwdTarget = 25.0f;
                i_this->mVelocityFwdTargetMaxVel = 2.0f;
                a_this->speedF = 0.0f;
                MtxTrans(a_this->current.pos.x, a_this->current.pos.y, a_this->current.pos.z, 0);
                cXyz sp34;
                sp34.set(0.0f, 1000.0f, 2000.0f);
                cMtx_YrotM(calc_mtx(), a_this->current.angle.y);
                MtxPosition(&sp34, &i_this->mTargetPos);
            }
        }
        break;
    }

    switch (cVar8) {
    case 0:
        kamome_pos_move(i_this);
        break;
    case 1:
        kamome_ground_pos_move(i_this);
        if (i_this->mMoveState != 0x19 &&
            (a_this->current.pos.y - i_this->mAcch.GetGroundH() /* GetGroundH */ > 200.0f ||
             fopAcM_searchActorDistance(a_this, dComIfGp_getPlayer(0)) < l_kamomeHIO.m10)) {
            take_off(i_this);
        }
        break;
    }

    cXyz* moveP = &i_this->mStts.m_cc_move; /* mStts.GetCCMoveP() */
    if (gabi::ea(moveP) != 0) {
        PSVECAdd(&a_this->current.pos, (cXyz*)moveP, &a_this->current.pos);
    }
    i_this->mSph.SetC(&a_this->current.pos);
    i_this->mSph.SetR(REG_F(17, 18) + 40.0f);
    dComIfG_Ccsp_Set(&i_this->mSph);
}

/* inlined: kamome_imouto2_move (mType 5) */
static inline void kamome_imouto2_move(kamome_class* i_this) {
    switch (i_this->mMoveState) {
    case 0:
        i_this->m2AB = 1;
        if (timer(i_this, 1) == 0) {
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_EAT1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, 0);
            i_this->mMoveState++;
        }
        break;
    case 1:
        if (McaMorf_isStop(i_this->mpMorf)) {
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT3_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, 0);
            timer(i_this, 1) = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
            i_this->mMoveState--;
        }
        break;
    }
}

/* 021861D4 */
static BOOL daKamome_Execute(kamome_class* i_this) {

    dComIfGp_get(); // HD: unused player lookup
    // HD: based on the USA version (mbNoDraw reset here)
    i_this->mGlobalTimer++;
    i_this->mbNoDraw = 0;

    if (i_this->mType == 6) {
        heisou_control(i_this);
        return TRUE;
    }

    if (i_this->mSwitchNo != 0) {
        if (dComIfGs_isSwitch(i_this->mSwitchNo - 1, fopAcM_GetRoomNo(&i_this->actor))) {
            i_this->mSwitchNo = 0;
            f32 r = cM_rndF(0.5f);
            anm_init(i_this, dRes_INDEX_KAMOME_BCK_KA_WAIT2_e, 1.0f, J3DFrameCtrl::EMode_LOOP, r + 1.0f, dRes_INDEX_KAMOME_BAS_KA_WAIT2_e);
            timer(i_this, 0) = (s16)gabi::ftoi(cM_rndF(60.0f) + 40.0f);
            i_this->mVelocityFwdTarget = 30.0f;
            i_this->actor.speedF = 30.0f;
        } else {
            return TRUE;
        }
    }

    if (l_kamomeHIO.m05 == 0) {
        for (s32 i = 0; i < 6; i++) {
            if (timer(i_this, i) != 0) {
                timer(i_this, i)--;
            }
        }

        if (i_this->mbUsePathMovement != 0) {
            kamome_path_move(i_this);
        } else if (i_this->mType == 4) {
            kamome_imouto_move(i_this);
        } else if (i_this->mType == 5) {
            kamome_imouto2_move(i_this);
        } else if (i_this->mType == 7) {
            kamome_heisou_move(i_this);
        } else {
            kamome_auto_move(i_this);
        }

        i_this->mpMorf->play(&i_this->actor.eyePos, 0, 0);
        cLib_addCalcAngleS2(&i_this->mJointRotY, i_this->mJointRotYTarget, 4, 0x800);
        cLib_addCalcAngleS2(&i_this->mJointRotZ, i_this->mJointRotZTarget, 4, 0x800);

        if (i_this->m2AB != 0) {
            if (timer(i_this, 5) == 0) {
                timer(i_this, 5) = (s16)gabi::ftoi(cM_rndF(20.0f) + 10.0f);
                i_this->mJointRotYTarget = (s16)gabi::ftoi(cM_rndFX(REG0_F(8) + 3000.0f));
                i_this->mJointRotZTarget = (s16)gabi::ftoi(cM_rndFX(REG0_F(8) + 3000.0f));
            }
            i_this->m2AB = 0;
        } else {
            i_this->mJointRotZTarget = 0;
            i_this->mJointRotYTarget = 0;
        }
    }

    daKamome_setMtx(i_this);

    i_this->actor.eyePos.copy(i_this->actor.current.pos);
    return TRUE;
}

/* WWHD glue
ext 02185558 void kamomeExecute_animation(kamome_class* actor, int resource, f32 blend, u8 mode, f32 speed, int sound)
ext 02185808 fopAc_ac_c* kamomeExecute_bait(kamome_class* actor)
ext 02185B78 void kamomeExecute_flight(kamome_class* actor)
ext 02185DF0 void kamomeExecute_background(kamome_class* actor)
ext 02185E80 void kamomeExecute_groundMove(kamome_class* actor)
ext 021860B0 void kamomeExecute_matrix(kamome_class* actor)
ext 02189270 void kamomeExecute_autoMove(kamome_class* actor)
ext 02008E0C void kamomeExecute_groundCtor(dBgS_GndChk_l* check)
ext 02008974 f32 kamomeExecute_groundCross(dBgS* bg,dBgS_GndChk_l* check)
*/

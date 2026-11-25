/**
 * d_a_bk_move.cpp (WWHD)
 * Enemy - Bokoblin: Bk_move, with the actions it inlines in WWHD (jyunkai, walk_set, stand,
 * stand2, path_run, search_target, daBk_wepon_view_check's call).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bk_move.h"
#include "d/actor/d_a_bk_t60.h"
#include <unordered_map>
#include "d/actor/d_a_bk_turnfs_t60.h" /* */
#include "d/actor/d_a_bk_failhold_t60.h" /* */

enum {
    JA_SE_CV_BK_SEARCH = 0x482A,
    JA_SE_CV_BK_FOUND_LINK = 0x482B,
    JA_SE_CV_BK_LOST_BOKO = 0x482C,
    JA_SE_CV_BK_SEARCH_BOKO = 0x482D,
    JA_SE_CV_BK_JUMP = 0x482F,
    JA_SE_CV_BK_NOBI = 0x4831,
};
enum { fopAcStts_UNK4000_e = 0x4000 };

/* true60 (the animation crossing-check rule): the animation frame entered v on this step (once per pass, as the 30 Hz frame check) */
static inline bool morf_frame_is(bk_class* i_this, int v) {
    return bk_t60_frame_is(i_this->mpMorf->getFrame(), i_this->mpMorf->getPlaySpeed(), v);
}

/* GameCube walk_set (inlined in jyunkai) */
static inline void walk_set(bk_class* i_this) {
    if (i_this->mHoldWeapon != 0 || i_this->mWeaponPending != 0) {
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_WALK_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_WALK_e);
    } else if (i_this->mBareHanded == 0) {
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_NIGERU_e, 5.0f, 2, 1.0f,
                 dRes_INDEX_BK_BAS_BK_NIGERU_e);
    } else {
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_WALK_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_WALK_e);
    }
    if (i_this->mHoldWeapon == 0 && i_this->mWeaponLost != 0) {
        fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_LOST_BOKO, 0);
        i_this->mWeaponLost = 0;
    }
}

/* jyunkai's notice (the tail of A's call, after its mode switch): the sound contact or Link in view -> fight_run.
 * true60 (pair frame 439): a mode change on the frame's first step holds the tick30 step (one
 * state step per frame), and the notice is a tick30 decision (first-step input addendum): the frame lost the notice A's call makes
 * right after its switch (A fight_run at 439, B a frame later). The held tick30 step runs it (Bk_move). */
static inline void jyunkai_notice(bk_class* i_this, s32 r3) {
    if (r3 != 0 || (i_this->mPlayerDistance < l_bkHIO().mNoticeDist &&
                    daBk_player_view_check(i_this, &i_this->dr.mpLookTarget->current.pos, i_this->mTargetAngleY,
                                           l_bkHIO().mViewAngle))) {
        i_this->dr.mAction = BK_ACTION_FIGHT_RUN;
        i_this->mTimers[1] = 0;
        i_this->dr.mMode = 0;
    }
}

/* GameCube jyunkai (inlined) */
static inline void jyunkai(bk_class* i_this) {
    fopAc_ac_c* i_actor = i_this;
    dComIfGp_get(); /* HD: unused accessor call */

    if (i_this->mType == 4 || i_this->mType == 0xA) {
        i_this->dr.mAction = BK_ACTION_STAND;
        i_this->dr.mMode = 50;
        return;
    } else if (i_this->mType == 6) {
        i_this->dr.mAction = BK_ACTION_STAND2;
        i_this->dr.mMode = 50;
        return;
    } else if (i_this->mType == 9) {
        i_this->dr.mAction = BK_ACTION_PATH_RUN;
        i_this->dr.mMode = 0;
        i_this->mTimers[2] = 0;
        i_this->mJumpMove = 0;
        i_this->mJumpStep = 0;
        path_check(i_this, 0);
        return;
    }

    bk_t60_jyunkai_ran(i_this); /* true60: its body runs in this call (for a held tick30 step's notice) */
    switch ((s16)i_this->dr.mMode) {
    case 0:
        walk_set(i_this);
        // Fall-through
    case -1:
        i_this->dr.mMode = 1;
        if (i_this->mHoldWeapon != 0 || i_this->mBareHanded != 0) {
            if (i_this->mPathOn != 0) {
                s8 idx = i_this->mPathPoint + i_this->mPathDir;
                i_this->mPathPoint = idx;
                if (idx >= (s8)path_num(i_this)) {
                    if (path_closed(i_this)) {
                        i_this->mPathPoint = 0;
                    } else {
                        i_this->mPathDir = -1;
                        i_this->mPathPoint = (s8)(path_num(i_this) - 2);
                    }
                    if (path_nextID(i_this) != 0xFFFF) {
                        i_this->ppd =
                            dPath_GetRoomPath(path_nextID(i_this), fopAcM_GetRoomNo(i_this));
                        if (i_this->ppd == nullptr) {
                            JUT_ASSERT_fail(STR(0x1000958C), 0xB95, STR(0x10009598));
                        }
                    }
                } else if (idx < 0) {
                    i_this->mPathDir = 1;
                    i_this->mPathPoint = 1;
                }
                dPnt_mv* point = path_point(i_this, i_this->mPathPoint);
                copy_f(&i_this->m0320, &point->m_position);
            } else {
                way_pos_check(i_this, &i_this->m0320);
            }
        } else {
            way_pos_check(i_this, &i_this->m0320);
            i_this->mTimers[1] = (s16)gabi::ftoi(30.0f + cM_rndF(25.0f));
        }
        i_this->mTimers[2] = 30;
        // Fall-through
    case 1: {
        s16 r29;
        f32 f31;
        if (i_this->mHoldWeapon != 0 || i_this->mBareHanded != 0) {
            r29 = 0x400;
            f31 = l_bkHIO().mPatrolSpeed;
        } else {
            r29 = 0x1000;
            f31 = l_bkHIO().mPatrolSpeedNoWeapon;
        }

        f32 x = i_this->m0320.x - i_this->current.pos.x;
        f32 z = i_this->m0320.z - i_this->current.pos.z;
        {
            f32 px = i_this->current.pos.x, pz = i_this->current.pos.z;
            if (t60bkturn::place(gabi::ea(i_this), &px, &pz)) /* true60 (d_a_bk_turnfs_t60.h): the turn target from the frame-start place */
                i_this->dr.mTurnTargetY = cM_atan2s(i_this->m0320.x - px, i_this->m0320.z - pz);
            else
                i_this->dr.mTurnTargetY = cM_atan2s(x, z);
        }

        /* true60 (the animation-driven decision rule, pair f337 / f362): the arrival / stop tests read the frame-start
         * Bokoblin (A's call, before the frame's move): the first step's answer. A later step tested its mid-frame position
         * (0.5 frame closer) and stopped a frame early. The first step's stop changes the state, which holds the later steps;
         * so a later step stops on nothing. dt = 1: every call. */
        const bool t60StopTest = bk_t60_first();
        if (i_this->mPathOn != 0 && (i_this->mHoldWeapon != 0 || i_this->mBareHanded != 0)) {
            f32 d = std_sqrtf(gabi::fmadds(x, x, z * z));
            f32 t = f31 * 0.25f;
            if (t60StopTest && d < t + t) {
                if (path_point(i_this, i_this->mPathPoint)->mArg3 == 3) {
                    wait_set(i_this);
                    i_this->dr.mMode = 2;
                } else {
                    i_this->dr.mMode = -1;
                }
            }
        } else {
            f32 d = std_sqrtf(gabi::fmadds(x, x, z * z));
            f32 t = f31 * 0.25f;
            if (t60StopTest && (d < t + t || (i_this->mTimers[2] == 0 &&
                                              (i_this->dr.mAcch.ChkWallHit() ||
                                               ground_4_check(i_this, 1, i_this->current.angle.y, 200.0f))))) {
                wait_set(i_this);
                i_this->dr.mMode = 2;
            }
        }
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 4, r29);
        cLib_addCalc2(&i_this->speedF, f31, 1.0f, 5.0f);
        bk_t60_walk_note(i_this, r29); /* true60: for a held tick30 step (d_a_bk_t60.h bk_t60_walk_due; f171) */
        break;
    }
    case 2:
        i_this->speedF = 0.0f;
        if (i_this->mHoldWeapon == 0 && i_this->mBareHanded == 0) {
            if ((morf_frame_is(i_this, 0xB) || morf_frame_is(i_this, 0x19)) && cM_rndF(1.0f) < 0.5f) {
                fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_SEARCH, 0);
            }
        }
        if (i_this->mTimers[1] == 0) {
            i_this->dr.mMode = 0;
            if (i_this->mHoldWeapon == 0 && i_this->mBareHanded == 0 && i_this->mWeaponRunTimer == 0) {
                i_this->mBareHanded = 1;
            }
        }
        break;
    case 3: {
        i_this->speedF = 0.0f;
        if (morf_frame_is(i_this, 3) || morf_frame_is(i_this, 0x35)) {
            fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_SEARCH, 0);
        }
        if (i_this->mTimers[1] == 0) {
            i_this->dr.mMode = 0;
        }
        break;
    }
    }

    /* true60 (first-step input addendum): the notice decision reads Link (and the sound contact): tick30 step
     * (jyunkai_notice; a held tick30 step runs it too, Bk_move) */
    s32 r3 = fopAcM_otoCheck(i_actor, 1000.0f);
    r3 += search_sp();
    if ((i_this->mHoldWeapon != 0 || i_this->mBareHanded != 0) && bk_t60_end()) {
        jyunkai_notice(i_this, r3);
    } else if (i_this->mHoldWeapon == 0 && i_this->mBareHanded == 0 && i_this->dr.mMode == 2) {
        if ((morf_frame_is(i_this, 0xB) || morf_frame_is(i_this, 0x19)) && cM_rndF(1.0f) < 0.5f) {
            fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_SEARCH_BOKO, 0);
        }
    }

    /* HD: daBk_wepon_view_check is an out-of-line function (0209AB64) */
    if (i_this->mHoldWeapon == 0 && daBk_wepon_view_check_mv(i_this)) {
        i_this->dr.mAction = BK_ACTION_WEPON_SEARCH;
        i_this->dr.mMode = -1;
    }

    if (bk_t60_end() && daBk_bomb_view_check(i_this)) { /* true60: reads the bombs and Link: tick30 step */
        i_this->dr.mAction = BK_ACTION_B_NIGE;
        i_this->dr.mMode = 0;
    }
}

/* common tail of stand / stand2: player sighting and bombs */
static inline void stand_sight(bk_class* i_this) {
    f32 f1;
    if (i_this->mSightRangePrm != 0xFF) {
        f1 = (f32)(u32)i_this->mSightRangePrm * 10.0f;
    } else {
        f1 = 500.0f;
    }

    /* true60 (first-step input addendum): the sighting reads Link: tick30 step */
    if (bk_t60_end() && i_this->mPlayerDistance < f1 &&
        daBk_player_view_check(i_this, &i_this->dr.mpLookTarget->current.pos, i_this->mTargetAngleY,
                               l_bkHIO().mViewAngle)) {
        if (i_this->dr.mMode >= 50) {
            i_this->dr.mAction = BK_ACTION_FIGHT_RUN;
            i_this->mTimers[1] = 0;
            i_this->dr.mMode = 0;
        } else {
            i_this->dr.mMode = 20;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_BIKKURI_e, 3.0f, 0, 1.0f, -1);
            i_this->mTimers[1] = 30;
            fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_FOUND_LINK, 0);
        }
    }

    if (bk_t60_end() && daBk_bomb_view_check(i_this)) { /* true60: reads the bombs and Link: tick30 step */
        i_this->dr.mAction = BK_ACTION_B_NIGE;
        i_this->dr.mMode = 0;
    }
}

/* GameCube stand (inlined) */
static inline void stand(bk_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->dr.mLookMode = 0;
    bool r28 = false;
    bool r27 = false;

    s32 r31 = fopAcM_otoCheck(i_this, 1000.0f);

    switch ((s16)i_this->dr.mMode) {
    case -20:
        i_this->actor_status |= fopAcStts_UNK4000_e;
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_WAIT_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_WAIT_e);
        i_this->dr.mMode = -19;
        i_this->mTimers[1] = 20;
        i_this->mSightRangePrm = 0;
        // Fall-through
    case -19:
        if (i_this->mTimers[1] == 0) {
            fopAc_ac_c* ken = (fopAc_ac_c*)fpcM_Search(0x0209B67C /* ken_s_sub */, i_this);
            set_ken(ken);
            if (ken) {
                gabi::Local<cXyz> tmp;
                gabi::Local<cXyz> sp28;
                cXyz_mi(&player->current.pos, tmp, &ken->current.pos);
                sp28->copy(*tmp);
                if (std_sqrtf(PSVECSquareMag(sp28)) < 800.0f) {
                    i_this->mDemoCamMode = 1;
                    i_this->dr.mMode = -18;
                }
            } else {
                i_this->dr.mMode = 1;
                break;
            }
        }
        r27 = true;
        break;
    case 0: {
        f32 r = cM_rndF((f32)(s32)(l_bkHIO().mWaitTimeMax - l_bkHIO().mWaitTimeMin));
        i_this->mTimers[1] = (s16)gabi::ftoi(r + (f32)(s32)l_bkHIO().mWaitTimeMin);
    }
        // Fall-through
    case -1:
        i_this->dr.mMode = 1;
        if (i_this->mHasTelescope != 0) {
            if (cM_rndF(1.0f) < 0.5f) {
                anm_init(i_this, dRes_INDEX_BK_BCK_BK_NOZOKU_e, 10.0f, 2, 1.0f, -1);
                i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(200.0f) + 200.0f);
            } else {
                anm_init(i_this, dRes_INDEX_BK_BCK_BK_TATAKU_e, 10.0f, 2, 1.0f, -1);
                i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(100.0f) + 100.0f);
            }
        } else {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_WAIT_e, 10.0f, 2, 1.0f,
                     dRes_INDEX_BK_BAS_BK_WAIT_e);
        }
        i_this->mStandWaitCount++;
        // Fall-through
    case 1:
        cLib_addCalc2(&i_this->current.pos.x, i_this->home.pos.x, 0.5f, i_this->speed.x * 0.25f);
        cLib_addCalc2(&i_this->current.pos.z, i_this->home.pos.z, 0.5f, i_this->speed.z * 0.25f);
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->home.angle.y, 2, 0x800);
        i_this->speedF = 0.0f;
        if (i_this->mTimers[1] == 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_NOBI_e, 10.0f, 0, 1.0f,
                     dRes_INDEX_BK_BAS_BK_NOBI_e);
            fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_NOBI, 0);
            if (i_this->mStandWaitCount < 3) {
                i_this->dr.mMode = 2;
            } else {
                i_this->dr.mMode = 3;
                i_this->mStandWaitCount = 0;
            }
        }
        break;
    case 2:
        r28 = true;
        i_this->mBlinkTimer = 2;
        if (en_t60_isStop(i_this->mpMorf)) {
            i_this->dr.mMode = 0;
        }
        break;
    case 3:
        r28 = true;
        i_this->mBlinkTimer = 2;
        if (en_t60_isStop(i_this->mpMorf)) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_SLEEP_e, 10.0f, 2, 1.0f,
                     dRes_INDEX_BK_BAS_BK_SLEEP_e);
            f32 r = cM_rndF((f32)(s32)(l_bkHIO().mSleepTimeMax - l_bkHIO().mSleepTimeMin));
            i_this->mTimers[1] = (s16)gabi::ftoi(r + (f32)(s32)l_bkHIO().mSleepTimeMin);
            i_this->dr.mMode += 1;
        }
        break;
    case 4:
        r28 = true;
        i_this->mBlinkTimer = 2;
        if (i_this->mTimers[1] == 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_NOBI_e, 10.0f, 0, 1.0f,
                     dRes_INDEX_BK_BAS_BK_NOBI_e);
            fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_NOBI, 0);
            i_this->dr.mMode = 2;
        }
        break;
    case 10:
        /* true60 (the first-step AI-timer rule): the timer reaches 30 on the first step: start the animation once */
        if (i_this->mTimers[1] == 30 && bk_t60_first()) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_BIKKURI_e, 3.0f, 0, 1.0f, -1);
        }
        if (i_this->mTimers[1] == 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO2_e, 10.0f, 2, 1.0f,
                     dRes_INDEX_BK_BAS_BK_KYORO2_e);
            i_this->dr.mMode = 11;
            i_this->mTimers[3] = (s16)gabi::ftoi(30.0f + cM_rndF(30.0f));
        }
        break;
    case 11:
        if (i_this->mTimers[3] == 0) {
            i_this->dr.mMode = 0;
        }
        break;
    case 20:
        if (i_this->mTimers[1] < 10) {
            i_this->dr.mLookMode = 1;
        }
        if (i_this->mTimers[1] == 0) {
            i_this->dr.mAction = BK_ACTION_FIGHT_RUN;
            i_this->mTimers[1] = 0;
            i_this->dr.mMode = 0;
            return;
        }
        break;
    case 50:
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO1_e, 10.0f, 2, 1.0f,
                 dRes_INDEX_BK_BAS_BK_KYORO1_e);
        i_this->dr.mMode += 1;
        i_this->mTimers[1] = 50;
        // Fall-through
    case 51:
        i_this->speedF = 0.0f;
        if (i_this->mTimers[1] > 25) {
            r28 = true;
        }
        if (i_this->mTimers[1] == 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_RUN_e, 10.0f, 2, 1.0f,
                     dRes_INDEX_BK_BAS_BK_RUN_e);
            i_this->dr.mMode += 1;
            i_this->mTimers[2] = 60;
        }
        break;
    case 52: {
        gabi::Local<cXyz> tmp;
        gabi::Local<cXyz> sp28;
        cXyz_mi(&i_this->home.pos, tmp, &i_this->current.pos);
        copy_f(sp28, tmp);
        {
            f32 px = i_this->current.pos.x, pz = i_this->current.pos.z;
            if (t60bkturn::place(gabi::ea(i_this), &px, &pz)) /* true60 (d_a_bk_turnfs_t60.h): the turn target from the frame-start place */
                i_this->dr.mTurnTargetY = cM_atan2s(i_this->home.pos.x - px, i_this->home.pos.z - pz);
            else
                i_this->dr.mTurnTargetY = cM_atan2s(sp28->x, sp28->z);
        }
        f32 d = std_sqrtf(gabi::fmadds(sp28->x, sp28->x, sp28->z * sp28->z));
        if (d < l_bkHIO().mRunSpeed * 0.25f * 5.0f) {
            i_this->dr.mMode = 0;
        }
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 4, 0x1000);
        cLib_addCalc2(&i_this->speedF, l_bkHIO().mRunSpeed, 1.0f, 5.0f);

        if (i_this->dr.mAcch.ChkGroundHit() && i_this->dr.mAcch.ChkWallHit()) {
            if (i_this->mHasTelescope != 0) {
                i_this->dr.mMode = 60;
                i_this->mTimers[2] = 20;
                break;
            }

            i_this->speed.y = REG0_F(16) + 100.0f;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, 0, 1.0f,
                     dRes_INDEX_BK_BAS_BK_JUMP1_e);
            fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_JUMP, 0);
            i_this->dr.mMode = 53;
        }
        break;
    }
    case 53:
        i_this->speedF = l_bkHIO().mRunSpeed * 0.5f;
        if (!i_this->dr.mAcch.ChkGroundHit()) {
            break;
        }
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP2_e, 0.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_JUMP2_e);
        i_this->dr.mMode += 1;
        break;
    case 54:
        i_this->speedF = 0.0f;
        if (en_t60_isStop(i_this->mpMorf)) {
            i_this->dr.mMode = 51;
        }
        break;
    case 60:
        cLib_addCalcAngleS2(&i_this->current.angle.y, (s16)(i_this->dr.mTurnTargetY + 0x4000), 4, 0x1000);
        cLib_addCalc2(&i_this->speedF, l_bkHIO().mRunSpeed, 1.0f, 5.0f);
        if (i_this->mTimers[2] == 0) {
            i_this->dr.mMode = 52;
        }
        break;
    }

    if (r27) {
        return;
    }
    if (i_this->dr.mMode < 10 && r31 != 0) {
        i_this->dr.mMode = 10;
        i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(10.0f) + 45.0f);
    }
    if (!r28 && i_this->mTimers[2] == 0 && i_this->dr.mMode != 20) {
        stand_sight(i_this);
    }
    if (i_this->mHoldWeapon == 0 && daBk_wepon_view_check_mv(i_this)) {
        i_this->dr.mAction = BK_ACTION_WEPON_SEARCH;
        i_this->dr.mMode = -1;
    }
}

/* GameCube stand2 (inlined): beside a search light */
static inline void stand2(bk_class* i_this) {
    dComIfGp_get(); /* HD: unused accessor call */
    i_this->dr.mLookMode = 0;
    s32 r31 = fopAcM_otoCheck(i_this, 1000.0f);

    if (i_this->mpSearchLight == nullptr) {
        daObj_Search_Act_c* light =
            (daObj_Search_Act_c*)fpcM_Search(0x0209B6D8 /* s_s_sub */, i_this);
        i_this->mpSearchLight = light;
        if (light != nullptr) {
            daObj_Search_setChildId(light, fopAcM_GetID(i_this));
        } else {
            return;
        }
    }

    if (i_this->mpSearchLight != nullptr) {
        cMtx_YrotS(calc_mtx(), search_ac(i_this->mpSearchLight)->current.angle.y);
        gabi::Local<cXyz> sp24;
        sp24->x = REG8_F(11) + 320.0f;
        sp24->y = REG8_F(12) + 114.0f;
        sp24->z = REG8_F(13) + -55.0f;
        MtxPosition(sp24, &i_this->home.pos);
        PSVECAdd(&i_this->home.pos, &search_ac(i_this->mpSearchLight)->current.pos,
                 &i_this->home.pos);
        i_this->home.angle.y = search_ac(i_this->mpSearchLight)->current.angle.y + REG8_S(4);
    }

    gabi::Local<cXyz> tmp;
    gabi::Local<cXyz> sp24;
    switch ((s16)i_this->dr.mMode) {
    case 0:
        i_this->dr.mMode = 1;
        // Fall-through
    case 1: {
        if (i_this->mpSearchLight != nullptr) {
            daObj_Search_setBkControl(i_this->mpSearchLight, true);
        }
        i_this->dr.mLookMode = 6;
        if (i_this->mTimers[3] == 0) {
            i_this->mTimers[3] = (s16)gabi::ftoi(cM_rndF(150.0f) + 80.0f);
            i_this->m1212 = (s16)gabi::ftoi(cM_rndF(30.0f));
        } else if (i_this->mTimers[3] < (s16)(i_this->m1212 + 30)) {
            i_this->dr.mLookYaw = 10000;
            i_this->dr.mLookPitch = -10000;
        } else if (i_this->mTimers[4] == 0) {
            i_this->mTimers[4] = (s16)gabi::ftoi(cM_rndF(30.0f) + 10.0f);
            i_this->dr.mLookYaw = (s16)gabi::ftoi(3000.0f - cM_rndF(10000.0f));
            i_this->dr.mLookPitch = (s16)gabi::ftoi(-cM_rndF(2000.0f));
        }

        i_this->mWeaponAtHome = 1;
        s16 r28 = i_this->mRotAngle - i_this->mPrevRotAngle;
        if (i_this->mRotStep >= 0 && r28 < 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_MIGIROT_e, 5.0f, 2, 1.0f, -1);
        }
        if (i_this->mRotStep <= 0 && r28 > 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_HIDARIROT_e, 5.0f, 2, 1.0f, -1);
        }
        i_this->mRotStep = r28;
        i_this->mPrevRotAngle = i_this->mRotAngle;

        cLib_addCalc2(&i_this->current.pos.x, i_this->home.pos.x, 0.5f,
                      gabi::fmadds(i_this->speed.x, 0.25f, 10.0f));
        cLib_addCalc2(&i_this->current.pos.z, i_this->home.pos.z, 0.5f,
                      gabi::fmadds(i_this->speed.z, 0.25f, 10.0f));
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->home.angle.y, 2, 0x800);
        i_this->speedF = 0.0f;
        break;
    }
    case 10:
        i_this->mWeaponAtHome = 1;
        /* true60 (the first-step AI-timer rule): the timer reaches 30 on the first step: start the animation once */
        if (i_this->mTimers[1] == 30 && bk_t60_first()) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_BIKKURI_e, 3.0f, 0, 1.0f, -1);
        }
        if (i_this->mTimers[1] == 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO2_e, 10.0f, 2, 1.0f,
                     dRes_INDEX_BK_BAS_BK_KYORO2_e);
            i_this->dr.mMode = 11;
            i_this->mTimers[3] = (s16)gabi::ftoi(30.0f + cM_rndF(30.0f));
        }
        break;
    case 11:
        if (i_this->mTimers[3] == 0) {
            i_this->dr.mMode = 0;
        }
        break;
    case 20:
        i_this->mWeaponAtHome = 1;
        if (i_this->mTimers[1] < 10) {
            i_this->dr.mLookMode = 1;
        }
        if (i_this->mTimers[1] == 0) {
            i_this->dr.mAction = BK_ACTION_FIGHT_RUN;
            i_this->mTimers[1] = 0;
            i_this->dr.mMode = 0;
            return;
        }
        break;
    case 50:
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO1_e, 10.0f, 2, 1.0f,
                 dRes_INDEX_BK_BAS_BK_KYORO1_e);
        i_this->dr.mMode += 1;
        i_this->mTimers[1] = 50;
        // Fall-through
    case 51:
        i_this->speedF = 0.0f;
        if (i_this->mTimers[1] == 0) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_RUN_e, 10.0f, 2, 1.0f,
                     dRes_INDEX_BK_BAS_BK_RUN_e);
            i_this->dr.mMode += 1;
            path_check(i_this, 1);
            if (i_this->mPathPoint >= 4) {
                i_this->mPathDir = 1;
            } else {
                i_this->mPathDir = -1;
            }
            i_this->mTimers[2] = 60;
        }
        break;
    case 52:
    case 60: {
        if (i_this->dr.mMode == 52) {
            dPnt_mv* pnt = path_point(i_this, i_this->mPathPoint);
            copy_f(&i_this->m0320, &pnt->m_position);
            cXyz_mi(&i_this->m0320, tmp, &i_this->current.pos);
            copy_f(sp24, tmp);
        } else {
            cXyz_mi(&i_this->home.pos, tmp, &i_this->current.pos);
            copy_f(sp24, tmp);
        }
        {
            f32 px = i_this->current.pos.x, pz = i_this->current.pos.z;
            if (t60bkturn::place(gabi::ea(i_this), &px, &pz)) { /* true60 (d_a_bk_turnfs_t60.h): the turn target from the frame-start place */
                const cXyz* tgt = i_this->dr.mMode == 52 ? &i_this->m0320 : &i_this->home.pos;
                i_this->dr.mTurnTargetY = cM_atan2s(tgt->x - px, tgt->z - pz);
            } else {
                i_this->dr.mTurnTargetY = cM_atan2s(sp24->x, sp24->z);
            }
        }
        f32 d = std_sqrtf(gabi::fmadds(sp24->x, sp24->x, sp24->z * sp24->z));
        if (d < l_bkHIO().mRunSpeed * 0.25f * 5.0f) {
            if (i_this->dr.mMode == 60) {
                i_this->dr.mMode = 0;
            } else if (i_this->mPathPoint == 0) {
                i_this->dr.mMode = 60;
            } else {
                s8 idx = i_this->mPathPoint + i_this->mPathDir;
                i_this->mPathPoint = idx;
                u16 num = path_num(i_this);
                if (idx >= (s8)num) {
                    i_this->mPathPoint = 0;
                } else if (idx < 0) {
                    i_this->mPathPoint = (s8)(num - 1);
                }
            }
        }
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 4, 0x1000);
        cLib_addCalc2(&i_this->speedF, l_bkHIO().mRunSpeed, 1.0f, 5.0f);
        break;
    }
    }

    if (i_this->dr.mMode < 10 && r31 != 0) {
        i_this->dr.mMode = 10;
        i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(10.0f) + 45.0f);
    }

    if (i_this->mTimers[2] == 0 && i_this->dr.mMode != 20) {
        stand_sight(i_this);
    }

    if (i_this->mHoldWeapon == 0 && daBk_wepon_view_check_mv(i_this)) {
        i_this->dr.mAction = BK_ACTION_WEPON_SEARCH;
        i_this->dr.mMode = -1;
    }
}

/* GameCube path_run (inlined) */
static inline void path_run(bk_class* i_this) {
    dComIfGp_get(); /* HD: unused accessor call */
    i_this->dr.mLookMode = 0;

    switch ((s16)i_this->dr.mMode) {
    case 0:
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_RUN_e, 10.0f, 2, l_bkHIO().mRunAnmSpeed,
                 dRes_INDEX_BK_BAS_BK_RUN_e);
        i_this->dr.mMode = 1;
        // Fall-through
    case 1: {
        dPnt_mv* point = path_point(i_this, i_this->mPathPoint);
        f32 x = point->m_position.x + i_this->m0320.x;
        f32 z = point->m_position.z + i_this->m0320.z;
        f32 dx = x - i_this->current.pos.x;
        f32 dz = z - i_this->current.pos.z;
        {
            f32 px = i_this->current.pos.x, pz = i_this->current.pos.z;
            if (t60bkturn::place(gabi::ea(i_this), &px, &pz)) /* true60 (d_a_bk_turnfs_t60.h): the turn target from the frame-start place */
                i_this->dr.mTurnTargetY = cM_atan2s(x - px, z - pz);
            else
                i_this->dr.mTurnTargetY = cM_atan2s(dx, dz);
        }

        if (std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) < 100.0f) {
            s8 idx = i_this->mPathPoint + i_this->mPathDir;
            i_this->mPathPoint = idx;
            u16 num = path_num(i_this);
            if (idx >= (s8)num) {
                i_this->mPathPoint = (s8)(num - 1);
                i_this->mPathDir = -1;
                i_this->mPathEnd = 1;
            } else if (idx < 0) {
                i_this->mPathPoint = 0;
                i_this->mPathDir = 1;
                i_this->mPathEnd = 1;
            }
        }

        if (i_this->mTimers[1] == 0) {
            i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(50.0f) + 30.0f);
            i_this->m0320.x = cM_rndFX(50.0f);
            i_this->m0320.z = cM_rndFX(50.0f);
        }

        if (i_this->dr.mAcch.ChkGroundHit()) {
            s16 maxStep = 0x600;
            if (i_this->mJumpMove != 0) {
                maxStep = 0x2000;
            }
            cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 4, maxStep);
        }

        if (i_this->mJumpMove == 0) {
            i_this->speedF = l_bkHIO().mRunSpeed;
            if (i_this->mTimers[2] == 0) {
                i_this->mTimers[2] = (s16)gabi::ftoi(cM_rndF(100.0f) + 50.0f);
                anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, 0, 1.0f,
                         dRes_INDEX_BK_BAS_BK_JUMP1_e);
                i_this->mJumpMove = 1;
                i_this->mJumpStep = 0;
            }
        } else {
            switch (i_this->mJumpStep) {
            case 0:
                i_this->speedF = l_bkHIO().mRunSpeed * 1.2f;
                if (i_this->dr.mAcch.ChkGroundHit() && en_t60_isStop(i_this->mpMorf)) {
                    anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP2_e, 0.0f, 0, 1.0f,
                             dRes_INDEX_BK_BAS_BK_JUMP2_e);
                    i_this->mJumpStep = 1;
                    i_this->mSmokeTimer = l_bkHIO().mSmokeTimerBase + 2;
                    i_this->mSmokeType = 4;
                }
                break;
            case 1:
                i_this->speedF = 0.0f;
                if (i_this->dr.mAcch.ChkGroundHit() && en_t60_isStop(i_this->mpMorf)) {
                    i_this->mJumpStep = 0;
                    anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, 0, 1.0f,
                             dRes_INDEX_BK_BAS_BK_JUMP1_e);
                    f32 r = cM_rndF(REG8_F(7) + 10.0f);
                    i_this->speed.y = r + 65.0f + REG8_F(8);
                    fopAcM_monsSeStart_mv(i_this, JA_SE_CV_BK_JUMP, 0);
                }

                if (i_this->mTimers[2] == 0) {
                    i_this->mTimers[2] = (s16)gabi::ftoi(cM_rndF(100.0f) + 50.0f);
                    anm_init(i_this, dRes_INDEX_BK_BCK_BK_RUN_e, 10.0f, 2, l_bkHIO().mRunAnmSpeed,
                             dRes_INDEX_BK_BAS_BK_RUN_e);
                    i_this->mJumpMove = 0;
                }
                break;
            }
        }
        break;
    }
    }

    /* true60 (first-step input addendum): the notice reads Link: tick30 step */
    if (bk_t60_end() && i_this->mPlayerDistance < l_bkHIO().mNoticeDist &&
        daBk_player_view_check(i_this, &i_this->dr.mpLookTarget->current.pos, i_this->mTargetAngleY,
                               l_bkHIO().mViewAngle)) {
        i_this->dr.mAction = BK_ACTION_FIGHT_RUN;
        i_this->mTimers[1] = 0;
        i_this->dr.mMode = 0;
    }
}

/* GameCube search_target (inlined) */
static inline BOOL search_target(bk_class* i_this) {
    i_this->dr.mpLookTarget = dComIfGp_getPlayer(0);
    return FALSE;
}

/* true60 (first-step input addendum; the process-order read rule: Link runs before the Bokoblin, so its reads of
 * Link take his current state on the tick30 step): the first step's sensing keeps the Bokoblin's frame-start position and
 * the sensed target; the tick30 step takes the sensing again from them, with the target's current (frame-end) position,
 * as A's frame computes it (the Darknut's form). Nothing when the frame's first step took no sensing.
 * dt = 1: nothing. Host memory, this unit only (split-actor state (1)). */
struct BkT60Sense {
    u32 frame, target;
    f32 x, z;
};
static std::unordered_map<u32, BkT60Sense> s_bkT60Sense;
static void bk_t60_sense(bk_class* i_this) {
    if (t60::dt() == 1.0f)
        return;
    if (t60::first_step()) {
        BkT60Sense& e = s_bkT60Sense[gabi::ea(i_this)];
        e.frame = bk_t60_frame_tag();
        e.target = gabi::ea(static_cast<fopAc_ac_c*>(i_this->dr.mpLookTarget));
        e.x = i_this->current.pos.x;
        e.z = i_this->current.pos.z;
        return;
    }
    if (!t60::tick30())
        return;
    auto it = s_bkT60Sense.find(gabi::ea(i_this));
    if (it == s_bkT60Sense.end() || it->second.frame != bk_t60_frame_tag())
        return;
    const BkT60Sense& e = it->second;
    const fopAc_ac_c* target = gabi::at<fopAc_ac_c>(e.target);
    f32 x = target->current.pos.x - e.x;
    f32 z = target->current.pos.z - e.z;
    i_this->mPlayerDistance = std_sqrtf(gabi::fmadds(x, x, z * z));
    i_this->mTargetAngleY = cM_atan2s(x, z);
}

/* true60 (d_a_bk_t60.h bk_t60_wsph_note): a held tick30 step registers the weapon sphere the frame's
 * dispatch call registered (the original's call that changed the state registered it for this frame's Move), as that
 * call left it, without a read or a reaction. kind 1: the attack sweep to the frame-start mAtPos (the held steps keep
 * it); 2: the knock-down sphere (Spl 1, VsPlayer off, mass 3); 3: bk_action's plain Set. The hasMachete mass as in
 * yari_hit_check. dt = 1: never (nothing is marked). */
static void bk_t60_wsph_carry(bk_class* i_this, bool knockDownBranch) {
    if (bk_t60_wsph_due(i_this, 1)) {
        gabi::call(0x025167E4, &i_this->mWeaponSph, &i_this->mAtPos); /* dCcD_Sph::MoveCAt */
        dComIfG_Ccsp_Set(&i_this->mWeaponSph);
        if (i_this->mHasMachete != 0)
            dComIfG_Ccsp_SetMass(&i_this->mWeaponSph, 3);
    }
    if (!knockDownBranch && bk_t60_wsph_due(i_this, 2)) {
        i_this->mWeaponSph.OffAtSPrmBit(4); /* OffAtVsPlayerBit, as the knock-down call left it */
        i_this->mWeaponSph.SetAtSpl(1);
        dComIfG_Ccsp_Set(&i_this->mWeaponSph);
        dComIfG_Ccsp_SetMass(&i_this->mWeaponSph, 3);
    }
    if (bk_t60_wsph_due(i_this, 3))
        dComIfG_Ccsp_Set(&i_this->mWeaponSph);
}

/* 020A5EF4 */
void Bk_move(bk_class* i_this) {
    fopAc_ac_c* actor = i_this;

    dComIfGp_get(); /* HD: unused accessor call */
    /* true60: a state entered on an earlier step of this 30 Hz frame is first run next frame (one state
     * step per frame, d_a_bk_t60.h bk_t60_ai_held): its remaining steps keep the outputs of the frame's dispatch (move
     * angle offset, look mode, guard pose, speed) and skip the decisions; the speed is still turned into the motion */
    const bool t60Held = bk_t60_ai_held(i_this);
    if (!t60Held) {
        i_this->mMoveAngleOffset = 0;
        i_this->dr.mLookMode = 0;
        i_this->dr.mDefencePose = 0;
    }
    cLib_addCalcAngleS2(&i_this->mHeadAngleZ, 0, 2, 0x800);

    /* true60 (d_a_bk_failhold_t60.h): a held step of a frame whose first step was knocked down takes the
     * knocked-down branch, as A's call did (FAIL set by damage_reaction on an earlier step) */
    const bool t60KdFrame = t60bkfh::knockedFrame(gabi::ea(i_this), (s16)i_this->dr.mMode, t60Held);
    if (i_this->dr.mMode <= -100 || t60KdFrame) {
        /* true60: the knock-down entered on an earlier step of this frame is first run next frame (held); the weapon
         * sphere the frame's call registered before damage_reaction's change still goes into the tick30 Move */
        if (t60Held) {
            bk_t60_wsph_carry(i_this, true);
            /* true60 (d_a_bk_t60.h): the tick30 step's sensing and fight_run's turn toward it, which
             * A's call made before damage_check's switch, as the held steps below */
            if (t60::tick30() && !bk_t60_dmgturn_nc()) {
                bk_t60_sense(i_this);
                const int t60Turn = bk_t60_turn_due(i_this);
                if (t60Turn >= 0)
                    i_this->dr.mTurnTargetY = i_this->mTargetAngleY;
                if (t60Turn > 0)
                    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 4, t60Turn);
            }
            return;
        }
        i_this->mNoHitTimer = 5;
        if (std::fabs((f32)i_this->dr.mKnockbackSpeed) > 40.0f && !i_this->dr.mAcch.ChkGroundHit()) {
            i_this->mWeaponSph.SetC(&i_this->dr.mJntPos[12]);
            i_this->mWeaponSph.SetR(40.0f);
            i_this->mWeaponSph.OffAtSPrmBit(4); /* OffAtVsPlayerBit */
            i_this->mWeaponSph.SetAtSpl(1);     /* dCcG_At_Spl_UNK1 */
            dComIfG_Ccsp_Set(&i_this->mWeaponSph);
            dComIfG_Ccsp_SetMass(&i_this->mWeaponSph, 3);
            bk_t60_wsph_note(i_this, 2); /* true60: a held tick30 step registers it again (d_a_bk_t60.h) */

            if (i_this->mWeaponSph.ChkAtHit() && actor->speed.y < -50.0f) {
                actor->speed.y = 0.0f;
                i_this->dr.mShakeAmplitude = 8000.0f;
            }
            i_this->mCoCyl.OffCoSPrmBit(1); /* OffCoSetBit */
        }
        return;
    }

    i_this->mWeaponSph.OnAtSPrmBit(4); /* OnAtVsPlayerBit */
    if (i_this->mDefeated == 0) {
        i_this->mCoCyl.OnCoSPrmBit(1);
    } else {
        i_this->mCoCyl.OffCoSPrmBit(1);
    }

    if (t60Held) {
        /* true60: held, no decisions on this step; the frame call's weapon sphere into the tick30 Move */
        bk_t60_wsph_carry(i_this, false);
        /* ... but the tick30 step's sensing and fight_run's turn toward it, which A's call made before its mode switch
         * (d_a_bk_t60.h bk_t60_turn_due; pair f77) */
        if (t60::tick30()) {
            bk_t60_sense(i_this);
            const int t60Turn = bk_t60_turn_due(i_this);
            if (t60Turn >= 0)
                i_this->dr.mTurnTargetY = i_this->mTargetAngleY;
            s16 t60Pre;
            if (t60Turn >= 0 && bk_t60_jlaunch_take(i_this, &t60Pre)) {
                /* ... a jump-run launch on an earlier step (fight_run jump step 2 -> 1): A's call turned twice before
                 * the launch's random yaw. Back to the pre-add angle (the c_lib entry's last value), the prelude's turn
                 * (pins the frame's full step), the jump step's at dt 1, then the yaw again (d_a_bk_t60.h
                 * bk_t60_jlaunch_note) */
                const s16 t60Add = (s16)((u32)(s16)i_this->current.angle.y - (u32)t60Pre);
                i_this->current.angle.y = t60Pre;
                if (t60Turn > 0) {
                    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 4, t60Turn);
                    const f32 t60Previous = t60_set_dt(1.0f);
                    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 2, 0x2000);
                    t60_set_dt(t60Previous);
                } else {
                    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 2, 0x2000);
                }
                i_this->current.angle.y = (s16)((u32)(s16)i_this->current.angle.y + (u32)t60Add);
            } else if (t60Turn > 0)
                cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 4, t60Turn);
            /* ... and fight_run's jump landing tail (d_a_bk_t60.h): the landing call's fight-distance
             * decision with this step's sensing */
            if (bk_t60_jland_due(i_this) && i_this->dr.mAction == BK_ACTION_FIGHT_RUN && i_this->dr.mMode == 1 &&
                i_this->mPlayerDistance < l_bkHIO().mFightDist) {
                i_this->dr.mAction = 5;
                i_this->dr.mMode = 0;
            }
        }
        /* ... and jyunkai's walk tail (turn + speed ramp) when its mode 1 ran on an earlier step (entered there from
         * mode 0 / -1: A's entry call turns and ramps in the same call; d_a_bk_t60.h bk_t60_walk_due, f171): on
         * every held step after it */
        const s16 t60Walk = bk_t60_walk_due(i_this);
        if (t60Walk != 0) {
            cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 4, t60Walk);
            cLib_addCalc2(&i_this->speedF,
                          t60Walk == 0x400 ? l_bkHIO().mPatrolSpeed : l_bkHIO().mPatrolSpeedNoWeapon, 1.0f, 5.0f);
        }
        /* ... except jyunkai's notice, the tail of A's call after the mode switch the first step made (jyunkai_notice),
         * with the tick30 step's sensing as on any tick30 step */
        if (t60::tick30() && i_this->dr.mAction == BK_ACTION_JYUNKAI &&
            (i_this->mHoldWeapon != 0 || i_this->mBareHanded != 0) &&
            bk_t60_jyunkai_ran_here(i_this) /* true60: only after jyunkai's own call (not an entry from another action,
                                              * aite_miru 14 -> 0; d_a_bk_t60.h) */) {
            bk_t60_sense(i_this);
            s32 r3 = fopAcM_otoCheck(i_this, 1000.0f);
            r3 += search_sp();
            jyunkai_notice(i_this, r3);
        }
    } else if (i_this->dr.mHitStunTimer != 0 && i_this->dr.mKnockedDown == 0) {
        if (i_this->dr.mHitStunTimer == 1) {
            i_this->dr.mAction = BK_ACTION_FIGHT_RUN;
            i_this->dr.mMode = 0;
            i_this->mTimers[1] = 0;
        } else {
            i_this->dr.mTurnTargetY = i_this->mTargetAngleY;
            actor->speedF = -30.0f;
            i_this->mSmokeTimer = l_bkHIO().mSmokeTimerBase + 3;
            i_this->mSmokeType = 4;
        }
    } else {
        search_target(i_this);

        /* true60 (the first-step AI-timer rule): the frame's sensing (distance and direction to the target) is taken on
         * the frame's FIRST step, as the original's one call takes it before the frame's motion; the decisions that read
         * it (fight_run's fight / attack distance, the notice distance, ...) then cannot cross on a later step of the
         * frame (kbkV1_4: fight entered at frame 53 on a later step in B, 54 in A), and the turn chases the frame's
         * target angle on every step. dt = 1: every call (the original). */
        if (bk_t60_first()) {
            f32 x = i_this->dr.mpLookTarget->current.pos.x - actor->current.pos.x;
            f32 z = i_this->dr.mpLookTarget->current.pos.z - actor->current.pos.z;
            i_this->mPlayerDistance = std_sqrtf(gabi::fmadds(x, x, z * z));

            i_this->mTargetAngleY = cM_atan2s(x, z);
        }
        bk_t60_sense(i_this); /* true60: the tick30 step's sensing (first-step input addendum / process-order read) */

        /* true60 (the 30 Hz timer rule): counted on the frame's first step (damage_check's set on another step starts
         * one lower, bk_exec) */
        if (i_this->mParalyzeDelay != 0 && bk_t60_first()) {
            i_this->mParalyzeDelay = i_this->mParalyzeDelay - 1;
            if (i_this->mParalyzeDelay == 0) {
                /* HD: also clears dr.mCarryOffsetY/mCarryOffsetZ and current.angle.x/z */
                i_this->dr.mCarryOffsetZ = 0.0f;
                i_this->dr.mCarryOffsetY = 0.0f;
                i_this->dr.mAction = BK_ACTION_D_MAHI;
                i_this->dr.mMode = 0;
                i_this->current.angle.z = 0;
                i_this->current.angle.x = 0;
            }
        }

        switch ((s16)i_this->dr.mAction) {
        case BK_ACTION_JYUNKAI:
            jyunkai(i_this);
            break;
        case BK_ACTION_STAND:
            stand(i_this);
            break;
        case BK_ACTION_STAND2:
            stand2(i_this);
            break;
        case BK_ACTION_PATH_RUN:
            path_run(i_this);
            break;
        case BK_ACTION_FIGHT_RUN:
            fight_run(i_this);
            break;
        case BK_ACTION_FIGHT:
            fight(i_this);
            break;
        case BK_ACTION_DEFENCE:
            defence(i_this);
            break;
        case BK_ACTION_OSHI:
            oshi(i_this);
            break;
        case BK_ACTION_P_LOST:
            p_lost(i_this);
            break;
        case BK_ACTION_B_NIGE:
            b_nige(i_this);
            break;
        case BK_ACTION_HUKKI:
            hukki(i_this);
            break;
        case BK_ACTION_WEPON_SEARCH:
            wepon_search(i_this);
            break;
        case BK_ACTION_AITE_MIRU:
            aite_miru(i_this);
            break;
        case BK_ACTION_FAIL:
            fail(i_this);
            break;
        case BK_ACTION_YOGAN_FAIL:
            yogan_fail(i_this);
            break;
        case BK_ACTION_WATER_FAIL:
            water_fail(i_this);
            break;
        case BK_ACTION_TUBO_WAIT:
            tubo_wait(i_this);
            break;
        case BK_ACTION_B_HANG:
            b_hang(i_this);
            break;
        case BK_ACTION_ROPE_ON:
            rope_on(i_this);
            break;
        case BK_ACTION_D_DOZOU:
            d_dozou(i_this);
            break;
        case BK_ACTION_CARRY:
            carry_mv(i_this);
            break;
        case BK_ACTION_CARRY_DROP:
            carry_drop(i_this);
            break;
        case BK_ACTION_D_MAHI:
            d_mahi(i_this);
            break;
        case BK_ACTION_Z_DEMO_1:
            z_demo_1(i_this);
            break;
        }
    }

    if (!t60Held && (i_this->mType == 4 || i_this->mType == 10 || i_this->mType == 6) &&
        i_this->dr.mAction == BK_ACTION_FIGHT_RUN) {
        gabi::Local<cXyz> tmp;
        gabi::Local<cXyz> sp28;
        cXyz_mi(&actor->home.pos, tmp, &actor->current.pos);
        sp28->copy(*tmp);

        f32 f31;
        if (i_this->mSightRangePrm != 0xFF) {
            f31 = (f32)(u32)i_this->mSightRangePrm * 10.0f * 1.5f;
        } else {
            f31 = 750.0f;
        }

        if (std_sqrtf(PSVECSquareMag(sp28)) > f31) {
            if (i_this->mType == 4 || i_this->mType == 10) {
                i_this->dr.mAction = BK_ACTION_STAND;
            }
            if (i_this->mType == 6) {
                i_this->dr.mAction = BK_ACTION_STAND2;
            }
            i_this->dr.mMode = 51;
            i_this->mTimers[1] = 0;
            i_this->mTimers[2] = 60;
        }
    }

    if (i_this->dr.mAction != BK_ACTION_CARRY) {
        gabi::Local<cXyz> sp28;
        gabi::Local<cXyz> sp1C;
        sp28->x = 0.0f;
        sp28->y = 0.0f;
        fcopy(sp28->z, actor->speedF);
        if (i_this->dr.mAction != BK_ACTION_HUKKI && i_this->dr.mAction != BK_ACTION_FAIL &&
            i_this->dr.mHitStunTimer == 0) {
            i_this->dr.mKnockbackAngleY = actor->current.angle.y;
            cMtx_YrotS(calc_mtx(), (s16)(actor->current.angle.y + i_this->mMoveAngleOffset));
        } else {
            cMtx_YrotS(calc_mtx(), i_this->dr.mTurnTargetY);
        }
        MtxPosition(sp28, sp1C);
        fcopy(actor->speed.x, sp1C->x);
        fcopy(actor->speed.z, sp1C->z);
    }
}

/* weak guest-call stubs for the functions defined in d_a_bk.cpp (so this part links alone in
 * its temporary unit; the real definitions replace them in the merged unit) */
__attribute__((weak)) void anm_init(bk_class* a, int b, f32 m, u8 l, f32 s, int f) {
    bkMoveGuest_anm_init(a, b, m, l, s, f);
}
__attribute__((weak)) BOOL daBk_wepon_view_check(bk_class* a) {
    return bkMoveGuest_daBk_wepon_view_check(a);
}
__attribute__((weak)) void wait_set(bk_class* a) { bkMoveGuest_wait_set(a); }
__attribute__((weak)) BOOL daBk_bomb_view_check(bk_class* a) {
    return bkMoveGuest_daBk_bomb_view_check(a);
}
__attribute__((weak)) BOOL daBk_player_view_check(bk_class* a, cXyz* p, s16 b, s16 c) {
    return bkMoveGuest_daBk_player_view_check(a, p, b, c);
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): this split has no separate data yet
*/
// clang-format on

/**
 * d_a_gnd_move.cpp (WWHD)
 * Boss - Ganondorf movement, sword combos, and light-arrow defence.
 *
 * The GameCube source of this unit is all "Nonmatching"
 * stubs. HD: movement logic below preserves the verified HD implementation.
 */
#include "d/actor/d_a_gnd_move.h"

static GndMoveHioHD l_moveHio;
struct GndMoveZeldaPointer {
    gptr<fopAc_ac_c> actor;
};
static GndMoveZeldaPointer l_moveZelda;

/* inlined into gnd_move */
static inline bool mv_isStop(mDoExt_McaMorf* morf) {
    return (morf->mFrameCtrl.mState & 1) || morf->mFrameCtrl.mRate == 0.0f;
}
/* inlined into gnd_move */
static inline void mv_mons_se(gnd_class* a, u32 id) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gndMoveMonsterSound(id, &a->eyePos, pid, 0, reverb);
    }
}
/* inlined into gnd_move */
static inline void mv_mons_se_eye(gnd_class* a, u32 id) {
    if (gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gndMoveMonsterSound(id, &a->eyePos, pid, 0, reverb);
    }
}
/* inlined into gnd_move */
static inline void mv_se(fopAc_ac_c* a, u32 id) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        mDoAud_seStart(id, &a->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
    }
}
/* inlined into gnd_move */
static inline void mv_se_eye(fopAc_ac_c* a, u32 id) {
    if (gabi::ea(&a->eyePos) != 0) {
        mDoAud_seStart(id, &a->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
    }
}
/* inlined into gnd_move */
static inline void mv_setTarget(gnd_class* i_this, fopAc_ac_c* pl) {
    moveState(i_this).mTargetPos.copy(pl->current.pos);
}
/* inlined into gnd_move */
static inline fopAc_ac_c* mv_zelda() { return l_moveZelda.actor; }

// HD: GHS shares this movement tail across the inlined states.
/* inlined into gnd_move */
static inline void mv_moveEnd(gnd_class* i_this) {
    pos_move(i_this, 0);
    moveState(i_this).mTurnRate = 2000.0f;
    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 2, 0x1000);
}

/* Movement and spacing. */
/* inlined into gnd_move */
static inline void move0(gnd_class* i_this) {
    fopAc_ac_c* pl = dComIfGp_getPlayer(0);
    f32 dist = fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0));
    s32 frame = gabi::ftoi(i_this->mpMorf->mFrameCtrl.mFrame);
    switch ((u32)(s32)moveState(i_this).mMode) {
    case 0:
        anm_init(i_this, GND_BCK_MOVE, 10.0f, 2, 1.0f, 0x21);
        moveState(i_this).mMode = moveState(i_this).mMode + 1;
        break;
    case 1:
        moveState(i_this).mTargetSpeed = l_moveHio.mMoveSpeed;
        if (dist < l_moveHio.mNearDistance) {
            anm_init(i_this, GND_BCK_WAIT, 10.0f, 2, 1.0f, -1);
            moveState(i_this).mMode = 2;
        } else if (dist > l_moveHio.mFarDistance) {
            anm_init(i_this, GND_BCK_JUMP_START, 10.0f, 0, 1.0f, 0x17);
            i_this->speedF = 0.0f;
            moveState(i_this).mTargetSpeed = 0.0f;
            moveState(i_this).mMode = 3;
        }
        mv_setTarget(i_this, pl);
        break;
    case 2: {
        moveState(i_this).mTargetSpeed = 0.0f;
        s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
        if (player_view_check(i_this, ang)) {
            s16 next;
            if (dist > l_moveHio.mComboDistance && moveState(i_this).mAttackCooldown == 0) {
                next = 0;
            } else if (dist < l_moveHio.mCloseAttackDistance) {
                next = 0x14;
            } else if (moveState(i_this).mAttackCooldown != 0) {
                next = -1;
            } else if (cM_rndF(100.0f) < l_moveHio.mAlternateAttackChance) {
                moveState(i_this).mAction = GND_ACTION_ATTACK_ALTERNATE;
                next = (cM_rndF(1.0f) < 0.5f) ? 0 : 1;
            } else {
                s8 g = moveState(i_this).mFightPhase;
                moveState(i_this).mAction = GND_ACTION_ATTACK_COMBO;
                if (g == 0) {
                    next = 0;
                } else {
                    u8 h = l_moveHio.mComboChoice;
                    if (h == 1) {
                        next = 0;
                    } else if (g == 1 || h == 2) {
                        next = 5;
                    } else {
                        next = (cM_rndF(1.0f) < 0.5f) ? 0 : 5;
                    }
                }
            }
            if (next >= 0)
                moveState(i_this).mMode = next;
        }
        mv_setTarget(i_this, pl);
        break;
    }
    case 3:
        i_this->gravity = l_moveHio.mJumpGravity;
        if (frame == 0xC) {
            i_this->speed.y = l_moveHio.mJumpY;
            f32 s = l_moveHio.mJumpSpeed;
            moveState(i_this).mTargetSpeed = s;
            i_this->speedF = s;
            splash_set(i_this);
        }
        if (mv_isStop(i_this->mpMorf)) {
            anm_init(i_this, GND_BCK_JUMP_LOOP, 5.0f, 2, 1.0f, -1);
            moveState(i_this).mMode = moveState(i_this).mMode + 1;
        }
        break;
    case 4: {
        mv_se(i_this, 0x5158);
        f32 sy = i_this->speed.y;
        i_this->gravity = l_moveHio.mJumpGravity;
        if (sy < 0.0f) { /* bge: taken on NaN */
            anm_init(i_this, GND_BCK_FALL, 5.0f, 0, 1.0f, -1);
            moveState(i_this).mMode = moveState(i_this).mMode + 1;
        }
        break;
    }
    case 5: {
        mv_se(i_this, 0x5158);
        f32 g = l_moveHio.mJumpGravity;
        moveState(i_this).mTurnRate = 0.0f;
        i_this->gravity = g;
        if (checkGround(i_this, 0.0f)) {
            anm_init(i_this, GND_BCK_LAND, 2.0f, 0, 1.0f, -1);
            moveState(i_this).mMode = 6;
            splash_set(i_this);
        }
        break;
    }
    case 6: {
        mDoExt_McaMorf* morf = i_this->mpMorf;
        moveState(i_this).mTargetSpeed = 0.0f;
        moveState(i_this).mTurnRate = 0.0f;
        if (mv_isStop(morf)) {
            moveState(i_this).mMode = 2;
            anm_init(i_this, GND_BCK_WAIT, 10.0f, 2, 1.0f, -1);
        }
        break;
    }
    case 0x14:
        anm_init(i_this, GND_BCK_BACK_JUMP_START, 5.0f, 0, 1.0f, 0x1B);
        moveState(i_this).mMode = 0x15;
        break;
    case 0x15: {
        mDoExt_McaMorf* morf = i_this->mpMorf;
        i_this->speedF = 0.0f;
        moveState(i_this).mTargetSpeed = 0.0f;
        if (mv_isStop(morf)) {
            i_this->speed.y = l_moveHio.mBackJumpY;
            f32 s = l_moveHio.mBackJumpSpeed;
            i_this->speedF = s;
            moveState(i_this).mTargetSpeed = s;
            anm_init(i_this, GND_BCK_BACK_JUMP, 5.0f, 0, 1.0f, -1);
            moveState(i_this).mMode = 0x16;
        }
        mv_setTarget(i_this, pl);
        break;
    }
    case 0x16:
        i_this->gravity = l_moveHio.mBackJumpGravity;
        if (checkGround(i_this, 0.0f)) {
            anm_init(i_this, GND_BCK_BACK_JUMP_LAND, 5.0f, 0, 1.0f, 0x1C);
            moveState(i_this).mMode = 0x17;
        }
        mv_setTarget(i_this, pl);
        break;
    case 0x17: {
        mDoExt_McaMorf* morf = i_this->mpMorf;
        moveState(i_this).mTargetSpeed = 0.0f;
        if (mv_isStop(morf)) {
            moveState(i_this).mMode = 2;
            wait_set(i_this);
            moveState(i_this).mAttackCooldown = 0x14;
        }
        break;
    }
    }
    mv_moveEnd(i_this);
}

/* Sword combos. */
/* inlined into gnd_move */
static inline void attack0(gnd_class* i_this) {
    fopAc_ac_c* pl = dComIfGp_getPlayer(0);
    f32 dist = fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0));
    mDoExt_McaMorf* morf0 = i_this->mpMorf;
    bool follow = false, guard = false, endAtk = false, home = false;
    int turn = 0;
    s16 mode = moveState(i_this).mMode;
    s32 frame = gabi::ftoi(morf0->mFrameCtrl.mFrame);
    switch ((u32)(s32)mode) {
    case 0:
        anm_init(i_this, GND_BCK_COMBO_LEFT_START, 3.0f, 0, 1.0f, 8);
        mv_mons_se(i_this, 0x4947);
        moveState(i_this).mMode = moveState(i_this).mMode + 1;
        guard = true;
        break;
    case 1: {
        follow = true;
        guard = true;
        mDoExt_McaMorf* morf = i_this->mpMorf;
        if (frame >= 0x17) {
            moveState(i_this).mTargetSpeed = l_moveHio.mComboAdvanceSpeed;
        } else {
            moveState(i_this).mTargetSpeed = l_moveHio.mComboWindupSpeed;
            if ((u32)(frame - 0xD) < 4)
                moveState(i_this).mLeftSwordAttack = 1;
        }
        if (mv_isStop(morf)) {
            anm_init(i_this, GND_BCK_COMBO_LEFT_FOLLOW, 1.0f, 0, 1.0f, 9);
            mv_mons_se(i_this, 0x4948);
            moveState(i_this).mMode = moveState(i_this).mMode + 1;
        }
        break;
    }
    case 2: {
        follow = true;
        guard = true;
        mDoExt_McaMorf* morf = i_this->mpMorf;
        if (frame >= 0xC) {
            moveState(i_this).mTargetSpeed = l_moveHio.mComboAdvanceSpeed;
        } else {
            moveState(i_this).mTargetSpeed = l_moveHio.mComboSwingSpeed;
            if ((u32)(frame - 5) < 6)
                moveState(i_this).mLeftSwordAttack = 1;
        }
        if (mv_isStop(morf)) {
            if (moveState(i_this).mCounterRequest != 0) {
                moveState(i_this).mCounterRequest = 0;
                anm_init(i_this, GND_BCK_COUNTER_START, 3.0f, 0, 1.0f, 0x1D);
                moveState(i_this).mMode = 0x15;
                break;
            }
            anm_init(i_this, GND_BCK_COMBO_LEFT_FINISH, 2.0f, 0, 1.0f, 0xA);
            mv_mons_se(i_this, 0x4949);
            moveState(i_this).mMode = moveState(i_this).mMode + 1;
        }
        if (frame > l_moveHio.mFirstParryFrame)
            moveState(i_this).mNextParryOpeningType = 1;
        break;
    }
    case 3:
        guard = true;
        if (frame >= 0x19) {
            moveState(i_this).mTargetSpeed = l_moveHio.mComboAdvanceSpeed;
        } else {
            moveState(i_this).mTargetSpeed = l_moveHio.mComboSwingSpeed;
            if ((u32)(frame - 6) < 0x11) {
                moveState(i_this).mLeftSwordAttack = 1;
                moveState(i_this).mRightSwordAttack = 1;
            }
        }
        if (frame >= l_moveHio.mComboParryStart && frame <= l_moveHio.mComboParryEnd)
            moveState(i_this).mNextParryOpeningType = 1;
        if (mv_isStop(i_this->mpMorf)) {
            moveState(i_this).mAction = GND_ACTION_MOVE;
            moveState(i_this).mMode = 0;
        }
        break;
    case 5:
        anm_init(i_this, GND_BCK_COMBO_RIGHT_START, 3.0f, 0, 1.0f, 0xB);
        mv_mons_se(i_this, 0x4947);
        moveState(i_this).mMode = moveState(i_this).mMode + 1;
        guard = true;
        break;
    case 6: {
        guard = true;
        if ((u32)(frame - 9) < 0xB) {
            moveState(i_this).mTargetSpeed = l_moveHio.mComboWindupSpeed;
        } else {
            moveState(i_this).mTargetSpeed = l_moveHio.mComboAdvanceSpeed;
        }
        mDoExt_McaMorf* morf = i_this->mpMorf;
        if ((u32)(frame - 0xD) < 5)
            moveState(i_this).mRightSwordAttack = 1;
        follow = true;
        if (mv_isStop(morf)) {
            anm_init(i_this, GND_BCK_COMBO_RIGHT_FOLLOW, 2.0f, 0, 1.0f, 0xC);
            mv_mons_se(i_this, 0x4949);
            moveState(i_this).mMode = 7;
        }
        break;
    }
    case 7: {
        follow = true;
        guard = true;
        moveState(i_this).mTargetSpeed = l_moveHio.mComboSwingSpeed;
        mDoExt_McaMorf* morf = i_this->mpMorf;
        if ((u32)(frame - 8) < 0x10) {
            moveState(i_this).mLeftSwordAttack = 1;
            moveState(i_this).mRightSwordAttack = 1;
        }
        if (!mv_isStop(morf))
            break;
        if (moveState(i_this).mCounterRequest != 0) {
            moveState(i_this).mCounterRequest = 0;
            anm_init(i_this, GND_BCK_COUNTER_START, 3.0f, 0, 1.0f, 0x1D);
            moveState(i_this).mMode = 0x15;
            break;
        }
        anm_init(i_this, GND_BCK_COMBO_RIGHT_FINISH, 5.0f, 0, 1.0f, 0xF);
        moveState(i_this).mMode = 8;
        break;
    }
    case 8: {
        mDoExt_McaMorf* morf = i_this->mpMorf;
        moveState(i_this).mTargetSpeed = 0.0f;
        guard = true;
        if (!mv_isStop(morf))
            break;
        anm_init(i_this, GND_BCK_JUMP_SLASH_START, 2.0f, 0, 1.0f, 0xD);
        mv_mons_se(i_this, 0x4948);
        moveState(i_this).mMode = 9;
        i_this->speed.y = l_moveHio.mAttackJumpY;
        moveState(i_this).mTargetSpeed = gabi::fmuls_ppc(dist, REG0_F(9) + 0.015f);
        splash_set(i_this);
        break;
    }
    case 9: {
        guard = true;
        f32 sy = i_this->speed.y;
        if (sy > 0.0f) { /* ble: taken on NaN */
            f32 sx = i_this->speed.x;
            f32 sz = i_this->speed.z;
            moveState(i_this).mSavedSpeed.x = sx;
            moveState(i_this).mSavedSpeed.y = sy;
            moveState(i_this).mSavedSpeed.z = sz;
        }
        home = true;
        turn = 1;
        if (frame > l_moveHio.mJumpParryFrame)
            moveState(i_this).mNextParryOpeningType = 2;
        f32 g = l_moveHio.mAttackGravity;
        moveState(i_this).mTurnRate = 0.0f;
        i_this->gravity = g;
        if (!checkGround(i_this, 0.0f))
            break;
        anm_init(i_this, GND_BCK_JUMP_SLASH_LAND, 2.0f, 0, 1.0f, 0xE);
        moveState(i_this).mMode = 0xA;
        splash_set(i_this);
        break;
    }
    case 0xA: {
        guard = true;
        s8 g = moveState(i_this).mFightPhase;
        s16 a = i_this->shape_angle.y;
        if (g >= 2)
            moveState(i_this).mCombatEffect = 2;
        i_this->current.angle.y = a;
        if (frame >= l_moveHio.mLandingParryStart && frame <= l_moveHio.mLandingParryEnd)
            moveState(i_this).mNextParryOpeningType = 2;
        moveState(i_this).mTargetSpeed = 0.0f;
        mDoExt_McaMorf* morf = i_this->mpMorf;
        if ((u32)(frame - 3) < 3) {
            moveState(i_this).mLeftSwordAttack = 1;
            moveState(i_this).mRightSwordAttack = 1;
            moveState(i_this).mRightSwordRadius = 60.0f;
            moveState(i_this).mLeftSwordRadius = 60.0f;
        }
        if (mv_isStop(morf)) {
            moveState(i_this).mMode = 0xB;
            moveState(i_this).mActionTimer = l_moveHio.mCounterParryFrame;
        }
        break;
    }
    case 0xB: {
        s8 g = moveState(i_this).mFightPhase;
        s16 a = i_this->shape_angle.y;
        if (g >= 2)
            moveState(i_this).mCombatEffect = 2;
        s16 t = moveState(i_this).mActionTimer;
        i_this->current.angle.y = a;
        if (t == 0)
            endAtk = true;
        break;
    }
    case 0xF: {
        s16 t = moveState(i_this).mActionTimer;
        moveState(i_this).mTurnRate = 8000.0f;
        moveState(i_this).mTargetSpeed = 0.0f;
        follow = true;
        if (t != 0)
            break;
        anm_init(i_this, GND_BCK_CHARGE_START, 5.0f, 0, 1.0f, 0x1A);
        moveState(i_this).mMode = moveState(i_this).mMode + 1;
        break;
    }
    case 0x10: {
        mDoExt_McaMorf* morf = i_this->mpMorf;
        moveState(i_this).mTurnRate = 8000.0f;
        follow = true;
        if (!mv_isStop(morf))
            break;
        anm_init(i_this, GND_BCK_CHARGE_JUMP, 2.0f, 0, 1.0f, -1);
        mv_mons_se(i_this, 0x494A);
        moveState(i_this).mMode = moveState(i_this).mMode + 1;
        i_this->speed.y = REG0_F(9) + 30.0f;
        moveState(i_this).mTargetSpeed = gabi::fmuls_ppc(dist, REG0_F(10) + 0.025f);
        splash_set(i_this);
        break;
    }
    case 0x11: {
        f32 g = REG0_F(11) + -2.0f;
        f32 sy = i_this->speed.y;
        moveState(i_this).mTurnRate = 5000.0f;
        i_this->gravity = g;
        if (sy > 0.0f)
            break;
        anm_init(i_this, GND_BCK_CHARGE_FALL, 2.0f, 0, 1.0f, 0x18);
        mv_mons_se_eye(i_this, 0x494B);
        s16 m = moveState(i_this).mMode;
        // HD: Preserve the integer word copies, including signaling NaNs.
        u32 sx = moveWord(i_this->speed.x);
        u32 syw = moveWord(i_this->speed.y);
        moveWord(moveState(i_this).mSavedSpeed.x) = sx;
        u32 sz = moveWord(i_this->speed.z);
        moveWord(moveState(i_this).mSavedSpeed.y) = syw;
        moveState(i_this).mMode = m + 1;
        moveWord(moveState(i_this).mSavedSpeed.z) = sz;
        {
            GndMovePlayerHD* lp = reinterpret_cast<GndMovePlayHD*>(dComIfGp_get())->mpLink;
            u32 f = lp->mStatus;
            lp->mCounterState = 0;
            lp->mStatus = f & ~8u;
        }
        break;
    }
    case 0x12:
        moveState(i_this).mLeftSwordAttack = 4;
        home = true;
        moveState(i_this).mRightSwordAttack = 4;
        if (!checkGround(i_this, 0.0f))
            break;
        anm_init(i_this, GND_BCK_CHARGE_LAND, 2.0f, 0, 1.0f, 0x19);
        {
            s16 m = moveState(i_this).mMode;
            i_this->speedF = 0.0f;
            moveState(i_this).mTargetSpeed = 0.0f;
            moveState(i_this).mMode = m + 1;
        }
        splash_set(i_this);
        break;
    case 0x13:
        if (frame <= 4) {
            moveState(i_this).mLeftSwordAttack = 4;
            moveState(i_this).mRightSwordAttack = 4;
        }
        if (mv_isStop(morf0))
            endAtk = true;
        break;
    case 0x14:
        guard = true;
        anm_init(i_this, GND_BCK_COUNTER_START, 3.0f, 0, 1.0f, 0x1D);
        moveState(i_this).mMode = 0x15;
        break;
    case 0x15: {
        moveState(i_this).mTurnRate = 5000.0f;
        mDoExt_McaMorf* morf = i_this->mpMorf;
        moveState(i_this).mTargetSpeed = l_moveHio.mCounterSpeed;
        guard = true;
        if (!mv_isStop(morf))
            break;
        anm_init(i_this, GND_BCK_COUNTER_SLASH, 2.0f, 0, 1.0f, -1);
        mv_mons_se(i_this, 0x494A);
        moveState(i_this).mMode = 0x16;
        break;
    }
    case 0x16: {
        guard = true;
        if ((u32)frame < 5) {
            moveState(i_this).mRightSwordAttack = 3;
            moveState(i_this).mRightSwordRadius = REG0_F(12) + 170.0f;
        }
        if (frame == 0x14 && moveState(i_this).mCounterRequest == 0) {
            moveState(i_this).mImpactTimer = (s8)(REG0_S(4) + 0xD);
        }
        if (!mv_isStop(i_this->mpMorf))
            break;
        if (moveState(i_this).mCounterRequest != 0) {
            s8 g = moveState(i_this).mFightPhase;
            moveState(i_this).mCounterRequest = 0;
            if (g == 0) {
                moveState(i_this).mTargetSpeed = 0.0f;
                wait_set(i_this);
                moveState(i_this).mMode = 0x17;
                moveState(i_this).mActionTimer = 0x96;
            } else {
                moveState(i_this).mMode = 0xF;
                moveState(i_this).mActionTimer = REG0_S(7);
            }
        } else {
            moveState(i_this).mMode = 0x1A;
            wait_set(i_this);
            moveState(i_this).mActionTimer = REG0_S(5) + 0x2D;
        }
        break;
    }
    case 0x17:
        if (moveState(i_this).mActionTimer != 0)
            break;
        anm_init(i_this, GND_BCK_COUNTER_RECOVER, 3.0f, 0, 1.0f, -1);
        moveState(i_this).mMode = 0x18;
        break;
    case 0x18: {
        if (!mv_isStop(morf0))
            break;
        s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
        if (player_view_check(i_this, ang)) {
            endAtk = true;
            break;
        }
        anm_init(i_this, GND_BCK_TURN, 3.0f, 0, 1.0f, -1);
        mv_mons_se(i_this, 0x4954);
        moveState(i_this).mMode = 0x19;
        moveState(i_this).mActionTimer = 0x14;
        break;
    }
    case 0x19:
        turn = 1;
        if (moveState(i_this).mActionTimer == 0)
            endAtk = true;
        break;
    case 0x1A: {
        s16 t = moveState(i_this).mActionTimer;
        guard = true;
        moveState(i_this).mTargetSpeed = 0.0f;
        if (t == 0)
            endAtk = true;
        break;
    }
    }
    i_this->speedF = moveState(i_this).mTargetSpeed;
    if (follow) {
        mv_setTarget(i_this, pl);
        pos_move(i_this, 0);
    } else {
        pos_move(i_this, 1);
    }
    moveState(i_this).mTurnRate = 2000.0f;
    if (turn == 0) {
        cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 4, 0x2000);
    } else {
        s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
        cLib_addCalcAngleS2(&i_this->shape_angle.y, ang, 4, (s16)(turn << 12));
    }
    if (home) {
        cLib_addCalc2(&i_this->current.pos.x, pl->current.pos.x, 0.8f,
                      fabsf(moveState(i_this).mSavedSpeed.x));
        cLib_addCalc2(&i_this->current.pos.z, pl->current.pos.z, 0.8f,
                      fabsf(moveState(i_this).mSavedSpeed.z));
    }
    if (endAtk) {
        s16 a = i_this->shape_angle.y;
        moveState(i_this).mAction = GND_ACTION_MOVE;
        moveState(i_this).mMode = 0;
        i_this->current.angle.y = a;
        moveState(i_this).mAttackCooldown = l_moveHio.mAttackCooldown;
    }
    if (l_moveHio.mDisableGuard == 0 && moveState(i_this).mFightPhase < 2 && guard) {
        moveState(i_this).mCheckLightArrows = 0;
        moveState(i_this).mCombatEffect = 5;
    }
}

/* Alternate sword attacks. */
/* inlined into gnd_move */
static inline void attack1(gnd_class* i_this) {
    u32 play = dComIfGp_ea();
    mDoExt_McaMorf* morf = i_this->mpMorf;
    f32 reg = REG0_F(13);
    s32 frame = gabi::ftoi(morf->mFrameCtrl.mFrame);
    fopAc_ac_c* pl = gabi::at<GndMovePlayHD>(play)->mpPlayer;
    bool done = false;
    f32 r = reg + 110.0f;
    s16 mode = moveState(i_this).mMode;
    moveState(i_this).mRightSwordRadius = r;
    moveState(i_this).mLeftSwordRadius = r;
    switch ((u32)(s32)mode) {
    case 0:
        anm_init(i_this, GND_BCK_ALTERNATE_LEFT, 5.0f, 0, 1.0f, 0x1F);
        mv_mons_se(i_this, 0x494A);
        moveState(i_this).mMode = 5;
        break;
    case 1:
        anm_init(i_this, GND_BCK_ALTERNATE_RIGHT, 5.0f, 0, 1.0f, 0x20);
        mv_mons_se(i_this, 0x494C);
        moveState(i_this).mMode = 6;
        break;
    case 5:
    case 6:
        if (mode == 5 ? (u32)(frame - 0xF) < 4 : (u32)(frame - 0xC) < 0x17) {
            mDoExt_McaMorf* m = i_this->mpMorf;
            (mode == 5 ? moveState(i_this).mLeftSwordAttack : moveState(i_this).mRightSwordAttack) = 1;
            done = mv_isStop(m);
        } else {
            mv_setTarget(i_this, pl);
            done = mv_isStop(i_this->mpMorf);
        }
        break;
    }
    moveState(i_this).mTargetSpeed = l_moveHio.mMoveSpeed;
    pos_move(i_this, 0);
    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 2, 0x1000);
    if (done) {
        moveState(i_this).mAction = GND_ACTION_MOVE;
        moveState(i_this).mMode = 0;
        s16 a = i_this->shape_angle.y;
        moveState(i_this).mAttackCooldown = 0;
        i_this->current.angle.y = a;
    }
}

/* Quick sword attacks. */
/* inlined into gnd_move */
static inline void attack2(gnd_class* i_this) {
    dComIfGp_get();
    mDoExt_McaMorf* morf = i_this->mpMorf;
    f32 reg = REG0_F(13);
    s16 mode = moveState(i_this).mMode;
    s32 frame = gabi::ftoi(morf->mFrameCtrl.mFrame);
    moveState(i_this).mCheckLightArrows = 0;
    f32 r = reg + 110.0f;
    moveState(i_this).mRightSwordRadius = r;
    moveState(i_this).mLeftSwordRadius = r;
    switch ((u32)(s32)mode) {
    case 0:
        anm_init(i_this, GND_BCK_QUICK_LEFT, 2.0f, 0, 1.0f, 0x13);
        mv_mons_se(i_this, 0x494A);
        moveState(i_this).mMode = 2;
        break;
    case 1:
        anm_init(i_this, GND_BCK_QUICK_RIGHT, 2.0f, 0, 1.0f, 0x14);
        mv_mons_se(i_this, 0x4947);
        moveState(i_this).mMode = 3;
        break;
    case 2:
    case 3:
        if (mode == 2) {
            if (frame >= 4)
                moveState(i_this).mLeftSwordAttack = 1;
        } else {
            if (frame >= 6)
                moveState(i_this).mRightSwordAttack = 1;
        }
        if (mv_isStop(i_this->mpMorf)) {
            moveState(i_this).mAction = GND_ACTION_MOVE;
            moveState(i_this).mMode = 0;
            s16 a = i_this->shape_angle.y;
            moveState(i_this).mAttackCooldown = 0;
            i_this->current.angle.y = a;
        }
        break;
    }
}

// HD: Follow a point beside Zelda after each fight state.
/* inlined into gnd_move */
static inline void mv_pzEnd(gnd_class* i_this, bool done) {
    i_this->speedF = moveState(i_this).mTargetSpeed;
    mDoMtx_YrotS(calc_mtx(), moveState(i_this).mZeldaAngle);
    gabi::Local<cXyz> l20;
    gabi::Local<cXyz> l4c;
    gabi::Local<cXyz> l64;
    l20->y = 0.0f;
    l20->x = REG0_F(16) + -20.0f;
    l20->z = REG0_F(17) + 100.0f;
    MtxPosition(l20, l4c);
    cXyz_pl(&mv_zelda()->current.pos, l64, l4c);
    moveState(i_this).mTargetPos.copy(*l64);
    pos_move(i_this, 0);
    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 4, 0x2000);
    if (done) {
        cLib_addCalc2(&i_this->current.pos.x, moveState(i_this).mTargetPos.x, 0.2f,
                      fabsf(moveState(i_this).mSavedSpeed.x));
        cLib_addCalc2(&i_this->current.pos.z, moveState(i_this).mTargetPos.z, 0.2f,
                      fabsf(moveState(i_this).mSavedSpeed.z));
    }
}

/* The fight beside Zelda. */
/* inlined into gnd_move */
static inline void attackPZ(gnd_class* i_this) {
    fopAc_ac_c* zelda0 = mv_zelda();
    gabi::Local<cXyz> d;
    cXyz_mi(&zelda0->current.pos, d, &i_this->current.pos);
    f32 dist = std_sqrtf(PSVECSquareMag(d));
    fopAc_ac_c* z = mv_zelda();
    f32 dx = gabi::fsubs_ppc(i_this->current.pos.x, z->current.pos.x);
    f32 dz = gabi::fsubs_ppc(i_this->current.pos.z, z->current.pos.z);
    s16 ang = cM_atan2s(dx, dz);
    mDoExt_McaMorf* morf0 = i_this->mpMorf;
    moveState(i_this).mCheckLightArrows = 0;
    moveState(i_this).mDefenceTimer = 5;
    s16 mode = moveState(i_this).mMode;
    s32 frame = gabi::ftoi(morf0->mFrameCtrl.mFrame);
    bool done = false;
    switch ((u32)(s32)mode) {
    case 0:
        if (dist > REG_F(6, 1) + 1000.0f) {
            mDoMtx_YrotS(calc_mtx(), ang);
            gabi::Local<cXyz> l20;
            gabi::Local<cXyz> l4c;
            gabi::Local<cXyz> l7c;
            l20->x = 0.0f;
            l20->y = 0.0f;
            l20->z = -(REG0_F(1) + 1000.0f);
            MtxPosition(l20, l4c);
            cXyz_pl(&i_this->current.pos, l7c, l4c);
            mv_zelda()->current.pos.copy(*l7c);
        }
        mv_zelda()->shape_angle.y = ang;
        mv_zelda()->current.angle.y = ang;
        moveState(i_this).mZeldaAngle = ang;
        moveState(i_this).mMode = 1;
        moveState(i_this).mActionTimer = 0x14;
        moveState(i_this).mDemoMode = 1;
        reinterpret_cast<GndMoveZeldaHD*>(zelda0)->mFightRequest = 1;
        i_this->speedF = 0.0f;
        moveState(i_this).mTargetSpeed = 0.0f;
        gndMoveChangeBgmStatus(1); /* mDoAud_changeBgmStatus */
        /* fall through */
    case 1: {
        s16 t = moveState(i_this).mActionTimer;
        moveState(i_this).mTurnRate = 0.0f;
        if (t != 0)
            break;
        anm_init(i_this, GND_BCK_ZELDA_WAIT, 10.0f, 0, 1.0f, -1);
        mv_mons_se_eye(i_this, 0x4954);
        J3DModel* model = i_this->mpMorf->getModel();
        void* res = dComIfG_getObjectRes("Gnd", 0x62, GND_SAFESTRING_VTBL);
        gnd_btkAnm_init(i_this->mpBtk, J3DModel_getModelData(model), res, 1, 0, 1.0f, 0, -1, true, 0);
        moveState(i_this).mMode = 2;
        break;
    }
    case 2:
        moveState(i_this).mTurnRate = 0.0f;
        break;
    case 3:
        moveState(i_this).mMode = 5;
        /* fall through */
    case 5:
        anm_init(i_this, GND_BCK_JUMP_START, 5.0f, 0, 1.0f, 0x17);
        moveState(i_this).mMode = moveState(i_this).mMode + 1;
        break;
    case 6:
        moveState(i_this).mTurnRate = 8000.0f;
        i_this->gravity = REG0_F(17) + -0.5f;
        if (frame == 0xC) {
            i_this->speed.y = REG0_F(19) + 10.0f;
            moveState(i_this).mTargetSpeed = gabi::fmuls_ppc(dist, REG0_F(18) + 0.018f);
            splash_set(i_this);
        }
        if (!mv_isStop(i_this->mpMorf))
            break;
        anm_init(i_this, GND_BCK_JUMP_LOOP, 2.0f, 2, 1.0f, -1);
        moveState(i_this).mMode = moveState(i_this).mMode + 1;
        break;
    case 7: {
        mv_se_eye(i_this, 0x5158);
        f32 g = REG0_F(17) + -0.5f;
        f32 sy = i_this->speed.y;
        i_this->gravity = g;
        if (sy < 0.0f) { /* bge: taken on NaN */
            anm_init(i_this, GND_BCK_FALL, 5.0f, 0, 1.0f, -1);
            moveState(i_this).mMode = moveState(i_this).mMode + 1;
        }
        break;
    }
    case 8: {
        mv_se_eye(i_this, 0x5158);
        f32 g = REG0_F(17) + -0.5f;
        moveState(i_this).mTurnRate = 0.0f;
        i_this->gravity = g;
        if (!checkGround(i_this, 0.0f))
            break;
        anm_init(i_this, GND_BCK_LAND, 2.0f, 0, 1.0f, -1);
        // HD: Preserve the integer word copies, including signaling NaNs.
        u32 sx = moveWord(i_this->speed.x);
        u32 sy = moveWord(i_this->speed.y);
        moveWord(moveState(i_this).mSavedSpeed.x) = sx;
        s16 m = moveState(i_this).mMode;
        moveWord(moveState(i_this).mSavedSpeed.y) = sy;
        u32 sz = moveWord(i_this->speed.z);
        moveState(i_this).mMode = m + 1;
        moveWord(moveState(i_this).mSavedSpeed.z) = sz;
        splash_set(i_this);
        break;
    }
    case 9: {
        mDoExt_McaMorf* morf = i_this->mpMorf;
        moveState(i_this).mTargetSpeed = 0.0f;
        moveState(i_this).mTurnRate = 0.0f;
        done = true;
        if (!mv_isStop(morf))
            break;
        moveState(i_this).mMode = moveState(i_this).mMode + 1;
        anm_init(i_this, GND_BCK_ZELDA_STRIKE_START, 3.0f, 0, 1.0f, 0x10);
        break;
    }
    case 0xA: {
        done = true;
        if (!mv_isStop(morf0))
            break;
        moveState(i_this).mMode = mode + 1;
        anm_init(i_this, GND_BCK_ZELDA_STRIKE_FINISH, 2.0f, 0, 1.0f, 0x11);
        mv_mons_se_eye(i_this, 0x4955);
        JPABaseEmitter* emitter = dComIfGp_particle_set(0x837E, &i_this->current.pos);
        moveState(i_this).mAttackEmitter = gabi::ea(emitter);
        if (emitter == nullptr)
            break;
        u32 e = gabi::ea(emitter);
        u8 r = moveState(i_this).mEffectRed;
        u8 b = moveState(i_this).mEffectBlue;
        gabi::at<GndMoveEmitterHD>(e)->mRed = r;
        u8 g = moveState(i_this).mEffectGreen;
        gabi::at<GndMoveEmitterHD>(e)->mGreen = g;
        gabi::at<GndMoveEmitterHD>(e)->mBlue = b;
        break;
    }
    case 0xB: {
        if ((u32)frame == (u32)(REG0_S(5) + 2)) {
            moveState(i_this).mZeldaTimer = 0x96;
            reinterpret_cast<GndMoveZeldaHD*>(zelda0)->mHitRequest = 1;
            mv_se(mv_zelda(), 0x2828);
            gabi::Local<cXyz> p;
            u32 vib = gabi::ea(dComIfGp_getVibration());
            s16 reg = REG0_S(2);
            p->x = 0.0f;
            p->y = 1.0f;
            p->z = 0.0f;
            gabi::at<dVibration_c>(vib)->StartShock(reg + 4, -0x21, p.get());
        }
        u32 e;
        {
            s16 reg6 = REG0_S(6);
            e = moveState(i_this).mAttackEmitter;
            if ((u32)frame == (u32)(reg6 + 2)) {
                moveState(i_this).mDemoMode = 4;
                moveState(i_this).mDemoTimer = 0;
            }
        }
        if (e == 0)
            break;
        mDoExt_McaMorf* morf = i_this->mpMorf;
        if (mv_isStop(morf)) {
            gnd_becomeInvalidEmitter(e);
            moveState(i_this).mAttackEmitter = 0;
        } else {
            gndMoveEmitterTransform(gnd_getAnmMtx(morf->getModel(), 0x23),
                                    &gabi::at<GndMoveEmitterHD>(e)->mRotation,
                                    &gabi::at<GndMoveEmitterHD>(e)->mTranslation);
        }
        break;
    }
    }
    mv_pzEnd(i_this, done);
}

/* 02159FC8 */
void gnd_move(gnd_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    switch ((u32)(s32)moveState(i_this).mAction) {
    case GND_ACTION_MOVE:
        move0(i_this);
        break;
    case GND_ACTION_ATTACK_COMBO:
        attack0(i_this);
        break;
    case GND_ACTION_ATTACK_ALTERNATE:
        attack1(i_this);
        break;
    case GND_ACTION_ATTACK_QUICK:
        attack2(i_this);
        break;
    case GND_ACTION_ATTACK_ZELDA:
        attackPZ(i_this);
        break;
    case GND_ACTION_ATTACK_LAST:
        attack_last(i_this);
        break;
    case GND_ACTION_DEFENCE:
        defence0(i_this);
        break;
    case GND_ACTION_DAMAGE:
        damage(i_this);
        break;
    case GND_ACTION_DEMO_WAIT:
        demowait(i_this);
        break;
    case GND_ACTION_YA_WAIT:
        yawait(i_this);
        break;
    case GND_ACTION_FINISH:
        finish(i_this);
        return;
    }
    if (moveState(i_this).mFightPhase >= 2 && moveState(i_this).mAction != GND_ACTION_DAMAGE)
        moveState(i_this).mDamageTimer = 3;
    {
        u8 t = reinterpret_cast<GndMoveActorFlagsHD*>(player)->mArrowState;
        if ((t == 5 || t == 0xF) && moveState(i_this).mFightPhase < 2) {
            moveState(i_this).mCombatEffect = 0;
            moveState(i_this).mCheckLightArrows = 0;
        }
    }
    if (static_cast<s8>(moveState(i_this).mCheckLightArrows) != 0) {
        /* the light arrows */
        bool hit = false;
        bool check = true;
        fopAc_ac_c* shot = (fopAc_ac_c*)fpcM_Search(0x02154E4C /* shot_s_sub */, i_this);
        if (shot != nullptr) {
            gabi::Local<cXyz> d;
            gabi::Local<cXyz> v;
            cXyz_mi(&shot->current.pos, d, &moveState(i_this).mArrowCheckPos);
            v->copy(*d);
            if (fpcM_GetName(shot) == 0x1D8) {
                f32 len = std_sqrtf(PSVECSquareMag(v));
                if (len < shot->speedF * 10.0f &&
                    reinterpret_cast<GndMoveActorFlagsHD*>(shot)->mArrowState != 0 &&
                    moveState(i_this).mFightPhase < 2) {
                    s16 dd = i_this->shape_angle.y - shot->current.angle.y;
                    if (dd < 0)
                        dd = -dd;
                    if ((u16)dd < 0x4000) {
                        moveState(i_this).mCheckLightArrows = 0;
                    } else {
                        moveState(i_this).mAction = GND_ACTION_DEFENCE;
                        s16 m = 0;
                        if (checkGround(i_this, 50.0f))
                            m = 0xF;
                        moveState(i_this).mMode = m;
                        moveState(i_this).mDefenceRequest = 0;
                        moveState(i_this).mDefenceTimer = 0x1E;
                    }
                }
            } else {
                f32 len = std_sqrtf(PSVECSquareMag(v));
                if (len < shot->speedF * 10.0f)
                    hit = true;
            }
        }
        f32 pd = fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0));
        if (!hit) {
            check = reinterpret_cast<GndMoveActorFlagsHD*>(player)->mArrowState != 0 &&
                    pd < l_moveHio.mDefenceDistance;
        }
        if (check) {
            if (moveState(i_this).mAction != GND_ACTION_DEFENCE) {
                moveState(i_this).mDefenceChoice = 0;
                moveState(i_this).mDefenceRequest = 0;
                moveState(i_this).mDefenceCount = 0;
                moveState(i_this).mAction = GND_ACTION_DEFENCE;
                moveState(i_this).mMode = 0;
            }
            moveState(i_this).mActionTimer = REG_S(10, 3) + 2;
        }
    }
    damage_check(i_this);
    moveState(i_this).mCheckLightArrows = 1;
    if (gabi::ea(&i_this->mStts) != 0) {
        cXyz* mv = &i_this->mStts.m_cc_move;
        i_this->current.pos.x = gabi::fadds_ppc(i_this->current.pos.x, mv->x);
        i_this->current.pos.y = gabi::fadds_ppc(i_this->current.pos.y, mv->y);
        i_this->current.pos.z = gabi::fadds_ppc(i_this->current.pos.z, mv->z);
    }
    if (moveState(i_this).mKnockbackSpeed > 0.01f) { /* ble: taken on NaN */
        gabi::Local<cXyz> p;
        gabi::Local<cXyz> o;
        p->z = moveState(i_this).mKnockbackSpeed;
        p->x = 0.0f;
        p->y = 0.0f;
        mDoMtx_YrotS(calc_mtx(), moveState(i_this).mKnockbackAngle);
        MtxPosition(p, o);
        PSVECAdd(&i_this->current.pos, o, &i_this->current.pos);
        cLib_addCalc0(&moveState(i_this).mKnockbackSpeed, 1.0f, REG0_F(12) + 7.0f);
    }
}
/* WWHD glue
ext 025E1AA4 void gndMoveMonsterSound(u32 id,cXyz* pos,u32 actorId,u32 info,s32 reverb)
ext 025E1950 void gndMoveChangeBgmStatus(s32 status)
ext 028249B0 void gndMoveEmitterTransform(Mtx34* source,Mtx34* rotation,cXyz* translation)
str 1000FFE4 "Gnd" in 02159FC8
data l_moveHio 104640BC
data l_moveZelda 10464008
*/

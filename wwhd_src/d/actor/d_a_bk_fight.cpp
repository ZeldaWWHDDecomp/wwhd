/**
 * d_a_bk_fight.cpp (WWHD)
 * Enemy - Bokoblin: attack setup and the fight / run / retreat actions.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bk_fight.h"
#include "sead/ElementAt.h"
#include "d/actor/d_a_bk_t60.h"
#include <unordered_map>

/* true60 (the animation-rate rule): the attack frame mAttackFrame is a hand-stepped animation frame (the step's play
 * speed per frame, beside the animation it follows): + speed x dt per step. Its window compares (At sphere, sound, blur)
 * fall on whole and half frames, so a 30 Hz frame the attack ran through on every step ends exactly on the original's
 * value: the frame's start and speed are held (host memory keyed by the actor, frame tag g_Counter.mCounter0) and its
 * tick30 step stores start + speed (the original's add); a frame that changed the attack inside it (a new step speed, an
 * attack set on an in-between step) keeps the steps' sum. */
namespace {
struct BkT60AttackFrame {
    u32 frame;  /* 30 Hz frame tag */
    f32 start;  /* mAttackFrame at the frame's first step */
    f32 speed;  /* the per-frame speed added */
    f32 expect; /* mAttackFrame after the last step's add (a change from outside breaks the hold) */
    u8 steps;
    bool valid;
};
std::unordered_map<u32, BkT60AttackFrame> s_bkT60AttackFrames;
/* the At position of the frame's first step (bk_t60_at_frame_start), key: the actor */
struct BkT60AtStart {
    u32 frame; /* 30 Hz frame tag */
    Vec3f pos; /* plain host values: a host cXyz would read / write guest memory at ea(host address) */
    Vec3f eye; /* eyePos on the first step: where the At sphere starts (StartCAt) */
};
std::unordered_map<u32, BkT60AtStart> s_bkT60AtStart;
}  // namespace
/* true60 (the one-sweep-per-frame attack rule: one sweep from the frame's start to its end positions): the weapon tip the
 * execute reads (mWeaponTipPos, dr.mJntPos) comes from the last model calc, the previous frame's in the original, the
 * frame's earlier step's at N steps. The tick30 registration takes the position the frame's first step read (the
 * original's), not the half-frame-later one (kbkB1_2 frame 75: B swept 103 units against A's 152, Link's knock-back
 * speed 19.50 / 20.83). A first call on a later step (no first-step position this frame) keeps its own. dt = 1: nothing. */
static inline void bk_t60_at_frame_start(bk_class* i_this) {
    if (!bk_t60_at_grid())
        return;
    if (t60::first_step()) {
        BkT60AtStart& e = s_bkT60AtStart[gabi::ea(i_this)];
        e.frame = bk_t60_frame_tag();
        e.pos = i_this->mAtPos.get();
        e.eye = i_this->eyePos.get();
    } else if (t60::tick30()) {
        auto it = s_bkT60AtStart.find(gabi::ea(i_this));
        if (it != s_bkT60AtStart.end() && it->second.frame == bk_t60_frame_tag())
            i_this->mAtPos = it->second.pos;
    }
}
/* true60 (the one-sweep-per-frame attack rule, as bk_t60_at_frame_start): the At sphere starts at eyePos (HD), which the execute
 * also takes from the last model calc: the tick30 registration starts it at the eyePos the frame's first step read (the
 * original's), not the half-frame-later one. kbkI2_2 frame 74: start (-30.80, 1048.12) vs A's (-38.96, 1041.30), so the
 * frame-75 At vec (pos - start, Link's damage vector, d_cc_s.cpp SetTgRVec) pointed 3.7 deg off with 5 % less length
 * (Link angle -722 / -92, knock-back speed 19.27 / 19.63; Link 11.9 apart by 94). dt = 1: eyePos (the original). */
static inline bool bk_t60_at_start_eye(bk_class* i_this, Vec3f* eye) {
    if (bk_t60_at_grid() && t60::tick30()) {
        auto it = s_bkT60AtStart.find(gabi::ea(i_this));
        if (it != s_bkT60AtStart.end() && it->second.frame == bk_t60_frame_tag()) {
            *eye = it->second.eye;
            return true;
        }
    }
    return false;
}
/* the step's new attack frame (dt = 1: the original's add); *frameEnd: the frame-end value the original frame reaches */
static f32 bk_t60_attack_advance(bk_class* i_this, f32 speed, f32* frameEnd) {
    f32 cur = i_this->mAttackFrame;
    if (t60::dt() == 1.0f) {
        *frameEnd = cur + speed;
        return cur + speed;
    }
    u32 key = gabi::ea(i_this), tag = bk_t60_frame_tag();
    if (t60::first_step())
        s_bkT60AttackFrames[key] = BkT60AttackFrame{tag, cur, speed, cur, 0, true};
    auto it = s_bkT60AttackFrames.find(key);
    bool held = it != s_bkT60AttackFrames.end() && it->second.valid && it->second.frame == tag;
    if (held && (it->second.speed != speed || it->second.expect != cur))
        held = it->second.valid = false;
    f32 next = cur + gabi::fmuls_ppc(speed, t60::dt());
    *frameEnd = held ? it->second.start + speed : next;
    if (held) {
        it->second.steps++;
        if (t60::tick30() && it->second.steps == (u8)t60::steps())
            next = it->second.start + speed;
        it->second.expect = next;
    }
    if (t60::tick30() && it != s_bkT60AttackFrames.end())
        s_bkT60AttackFrames.erase(it);
    return next;
}
/* the reversal's immediate extra play (a guard bounce) is one original frame of the animation: dt 1 */
static inline void bk_t60_play_frame(bk_class* i_this) {
    f32 previous = t60_set_dt(1.0f);
    i_this->mpMorf->play(&i_this->eyePos, 0, 0);
    t60_set_dt(previous);
}
/* true60 (option (b); the first-step AI-timer rule addendum: a launch decided on tick30 catches up the frame's
 * earlier steps). The jump-run start is decided on the tick30 step (the way check reads Link, rule (1)); A's frame
 * moved the whole frame at the jump-run speed and played the new JUMP2 animation one whole frame. The position: the
 * enemy move's speed term ends every frame where A's does (the speed-ramp position-gap rule (3), c_damagereaction.cpp t60dr_speed_step: the
 * tick30 step moves the rest of the frame at the tick30 speed), which includes this launch. The animation: the new
 * JUMP2 gets the earlier steps' share of play (1 - dt) at once. dt = 1: nothing. */
static inline void bk_t60_run_catch_up(bk_class* i_this) {
    if (t60::dt() == 1.0f || !t60::tick30() || t60::first_step())
        return;
    /* the JUMP2 set on this tick30 step gets the frame's whole play in daBk_Execute (d_a_bk_t60.h bk_t60_play, the
     * tick30 set's latch): no catch-up on top of it */
    if (wwhd_t60_latch_pending(bk_t60_anm_t30_key(&i_this->mpMorf), bk_t60_frame_tag()) != 0)
        return;
    f32 previous = t60_set_dt(1.0f - t60::dt());
    i_this->mpMorf->play(&i_this->eyePos, 0, 0);
    t60_set_dt(previous);
}

/* true60 (d_a_bk_t60.h bk_t60_jlaunch_note (a)): the jump step's turn (scale 2, 0x2000) after the
 * prelude's on the same angle. On the tick30 step after a prelude turn the first step noted (turnHeld > 0: the c_lib
 * entry pinned the prelude's full step from the frame start) it is the original's one step from there (dt 1), A's
 * frame. The same for jump step 0 of a jump-run launch decided on the tick30 step (launch) when the first step noted no
 * prelude turn (turnHeld 0: the angle had no ease earlier in the frame; a pair's frame 245, Bokoblin 47C5B4B4 from jyunkai:
 * fight_run 0 -> 1 on the first step, the tick30 launch's turn began a partial entry, +4096 for A's +8192). Else (and
 * dt = 1) the converted call. */
static inline void bk_t60_jump_turn(bk_class* i_this, int turnHeld, bool launch = false) {
    if (t60::dt() != 1.0f && t60::tick30() && (turnHeld > 0 || (launch && turnHeld == 0)) && !bk_t60_launch_nc()) {
        f32 previous = t60_set_dt(1.0f);
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 2, 0x2000);
        t60_set_dt(previous);
        return;
    }
    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 2, 0x2000);
}
/* true60 (d_a_bk_t60.h bk_t60_jlaunch_note (b)): the tick30 step after a jump-run launch on an earlier
 * step of the frame. When that step is not held, fight_run's own prelude would turn the noted 0x800 after the launch's
 * random yaw (a pair: 1028 -> 1154 for A's 988, as the held Bk_move turn did in). Back to the pre-add angle, the
 * prelude's turn (pins the frame's full step from the frame start), the jump step's at dt 1, then the yaw again: A's
 * order. True when the launch was noted (the caller skips its prelude turn). */
static inline bool bk_t60_jlaunch_complete(bk_class* i_this, int turnHeld) {
    s16 pre;
    if (turnHeld < 0 || !bk_t60_jlaunch_take(i_this, &pre))
        return false;
    const s16 add = (s16)((u32)(s16)i_this->current.angle.y - (u32)pre);
    i_this->current.angle.y = pre;
    if (turnHeld > 0)
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 4, turnHeld);
    bk_t60_jump_turn(i_this, turnHeld);
    i_this->current.angle.y = (s16)((u32)(s16)i_this->current.angle.y + (u32)add);
    return true;
}
/* true60 (pair A 3560): the jump-run -> run switch is decided on the tick30 step (t60Way); the
 * frame's earlier steps took the jump branch, whose jump-run speed overwrote their share of the run speed ramp
 * (cLib_addCalc2 by 5) above it. A's one call ramps the whole frame (90 -> 85); B ended on the tick30 share (87.5).
 * The switch's step catches up the earlier steps' share (1 - dt, the ramp's scale 1: the clamped steps sum to A's).
 * Only when the frame's first step ran fight_run (turnHeld >= 0) with the weapon held (the jump-run speed set).
 * dt = 1: nothing. */
static inline void bk_t60_run_speed_catch_up(bk_class* i_this, f32 target, int turnHeld) {
    if (t60::dt() == 1.0f || !t60::tick30() || t60::first_step() || turnHeld < 0 || i_this->mHoldWeapon == 0 ||
        bk_t60_launch_nc())
        return;
    f32 previous = t60_set_dt(1.0f - t60::dt());
    cLib_addCalc2(&i_this->speedF, target, 1.0f, 5.0f);
    t60_set_dt(previous);
}

/* weak guest-call stubs for TU functions defined in d_a_bk.cpp (lets this part build alone;
 * the decompiled definitions replace them) */
__attribute__((weak)) void anm_init(bk_class* a, int b, f32 m, u8 l, f32 s, int snd) {
    bkFight_anm_init(a, b, m, l, s, snd);
}
__attribute__((weak)) void wait_set(bk_class* a) { bkFight_wait_set(a); }
__attribute__((weak)) BOOL daBk_player_view_check(bk_class* a, cXyz* p, s16 b, s16 c) {
    return bkFight_daBk_player_view_check(a, p, b, c);
}
__attribute__((weak)) BOOL daBk_player_bg_check(bk_class* a, cXyz* p) {
    return bkFight_daBk_player_bg_check(a, p);
}
__attribute__((weak)) BOOL daBk_player_way_check(bk_class* a) {
    return bkFight_daBk_player_way_check(a);
}
__attribute__((weak)) BOOL daBk_bomb_view_check(bk_class* a) {
    return bkFight_daBk_bomb_view_check(a);
}

/* ---- d_a_bk data ---- */
static u32 bk_at_kind[3] = {0, 1, 0};
static s32 bk_attack_ready_SE[3] = {18469, 18469, -3535};
static s32 bk_attack_go_SE[3] = {18468, 18470, -3535};
static s32 bk_attack_AP[3] = {1, 2, 1};
static u32 attack_info[3] = {270079164, 270079200, 270079236};
static inline attack_info_s* bkFight_attackInfo(s32 i) {
    return gabi::at<attack_info_s>(attack_info[i]);
}
static inline be<u16>& learn_check() { return *gabi::at<be<u16>>(0x10462474); }

enum {
    JA_SE_CV_BK_ATTACK_L = 0x4826,
    JA_SE_CV_BK_SEARCH = 0x482A,
    JA_SE_CV_BK_FOUND_LINK = 0x482B,
    JA_SE_CV_BK_LOST_BOKO = 0x482C,
    JA_SE_CV_BK_JUMP = 0x482F
};
enum { fpcNm_HIMO2_e = 0x1BE };

/* 0209B760 */
void attack_set(bk_class* i_this, u8 r28) {
    dComIfGp_get(); /* HD: leftover of an inlined player lookup */
    i_this->mAttackKind = 0;
    i_this->mAtStarted = 0;
    i_this->mHitActorId = fpcM_ERROR_PROCESS_ID_e;
    i_this->mTurnAlways = 0;
    i_this->mAttackFrame = 0.0f;
    i_this->mTimers[4] = 0;
    /* HD: no cM_rndF(100.0f) */
    i_this->mWeaponSph.SetR(REG8_F(3) + 60.0f);

    if (r28 == 2) {
        i_this->mWeaponSph.SetAtType(0x2000);
        i_this->mWeaponSph.SetAtSe(6);
    } else if (i_this->mHasMachete != 0) {
        i_this->mWeaponSph.SetAtType(0x800);
        i_this->mWeaponSph.SetAtSe(2);
    } else {
        i_this->mWeaponSph.SetAtType(0x2000);
        i_this->mWeaponSph.SetAtSe(4);
    }

    if (r28 == 0) {
        i_this->mAttackKind = 0;
        i_this->mAtStartFrame = REG6_F(4) + 23.0f;
        i_this->mAtEndFrame = REG6_F(5) + 26.0f;
        i_this->mLookStartFrame = 45.0f;
        i_this->mTurnEndFrame = l_bkHIO().mAttackTurnEndFrame;
    } else if (r28 == 1) {
        i_this->mAttackKind = 1;
        i_this->mAtStartFrame = REG6_F(8) + 3.0f;
        i_this->mAtEndFrame = REG6_F(9) + 9.0f;
        i_this->mLookStartFrame = 45.0f;
        i_this->mTurnEndFrame = 0.0f;
    } else if (r28 == 2) {
        i_this->mAttackKind = 2;
        i_this->mAtStartFrame = REG6_F(4);
        i_this->mAtEndFrame = REG6_F(5) + 20.0f;
        i_this->mLookStartFrame = 45.0f;
        i_this->mTurnEndFrame = 0.0f;
        i_this->mWeaponSph.SetR(REG8_F(3) + 25.0f);
    }

    i_this->mAttackDir = 1;
    i_this->mAttackStep = 0;

    attack_info_s* info = bkFight_attackInfo(i_this->mAttackKind);
    anm_init(i_this, info->bckFileIdx, 5.0f, 0 /* EMode_NONE */, info->speed, info->soundFileIdx);
    s32 se = bk_attack_ready_SE[i_this->mAttackKind];
    if (se != -0xDCF) {
        fopAcM_monsSeStart(i_this, se, 0);
    }
}

/* 0209BA18 */
void* shot_s_sub(void* param_1, void* unused) {
    if (fopAc_IsActor(param_1) && (learn_check() & 0x400) && param_1 != nullptr &&
        fpcM_GetName(param_1) == fpcNm_HIMO2_e) {
        return param_1;
    }
    return nullptr;
}

/* inlined into fight_run */
static inline void fight_run_set(bk_class* i_this) {
    anm_init(i_this, dRes_INDEX_BK_BCK_BK_RUN_e, 10.0f, 2, l_bkHIO().mRunAnmSpeed,
             dRes_INDEX_BK_BAS_BK_RUN_e);
}

/* 020A0A98 */
void fight_run(bk_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    const uint64_t t60Entry = bk_t60_ai_state(i_this); /* true60: for bk_t60_frame_work (d_a_bk_t60.h) */
    f32 stickPosX = CPad_GET_STICK_POS_X(0);
    s8 r29 = 0;
    if (i_this->dr.mAcch.ChkGroundHit() && i_this->dr.mAcch.ChkWallHit()) {
        r29 = 1;
    }
    i_this->dr.mTurnTargetY = i_this->mTargetAngleY;

    /* true60: the turn as the frame's first call decided it (entry mode, ground: d_a_bk_t60.h bk_t60_turn_note /
     * bk_t60_turn_due; f141 addendum); noted for a held tick30 step */
    const int t60TurnHeld = bk_t60_turn_due(i_this);
    int t60Turn = 0;
    if (bk_t60_jlaunch_complete(i_this, t60TurnHeld)) {
        t60Turn = t60TurnHeld; /* true60: the launch frame's turns done in A's order */
    } else if (t60TurnHeld >= 0) {
        t60Turn = t60TurnHeld;
        if (t60Turn != 0)
            cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 4, t60Turn);
    } else if (i_this->dr.mMode != 0) {
        s16 r6 = 0x400;
        if (i_this->dr.mMode == 1) {
            r6 = 0x800;
        }
        if (i_this->dr.mAcch.ChkGroundHit()) {
            cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 4, r6);
            t60Turn = r6;
        }
    }
    bk_t60_turn_note(i_this, t60Turn);

    bool runStart = false; /* true60: mode 1 entered in this call */
    switch ((s16)i_this->dr.mMode) {
    case 0:
        if (i_this->mTimers[1] == 0) {
            fight_run_set(i_this);
            i_this->dr.mMode = 1;
            i_this->mJumpMove = 0;
            i_this->m1212 = (s16)gabi::ftoi(cM_rndF(1000.0f));
            runStart = true;
            bk_t60_ai_ran(i_this); /* true60: mode 1's body runs in this call (fall-through): not held */
        } else {
            i_this->speedF = 0.0f;
            break;
        }
        /* fall-through */
    case 1: {
        bool t60Launch = false; /* true60: the jump-run start decided in this (tick30) call */
        f32 scaleMag;
        if (i_this->mHoldWeapon != 0 || i_this->mBareHanded != 0) {
            scaleMag = l_bkHIO().mRunSpeed;
        } else {
            scaleMag = l_bkHIO().mRunSpeedNoWeapon;
        }
        bk_t60_jstop_restore(i_this); /* true60: an earlier step's jump stop undone before the ramp */
        cLib_addCalc2(&i_this->speedF, scaleMag, 1.0f, 5.0f);
        /* true60 (the 30 Hz timer rule, the first-step AI-timer rule): a per-frame phase counter (it times the jump start): counts on the
         * frame's first step (d_a_bk_t60.h bk_t60_frame_work) and in the call that started the run */
        if (runStart || bk_t60_frame_work(i_this, t60Entry))
            i_this->m1212 = i_this->m1212 + 1;
        /* true60 (first-step input addendum): the way check reads Link's shape_angle, so the run /
         * jump-run choice is a decision: made on the tick30 step, once per frame as A's call (with the run anim
         * switch and the jump-run start); the earlier steps keep the branch the last decision left (mJumpMove) for
         * the frame's speed and the jump steps. dt = 1: every call (the original). */
        const bool t60Way = bk_t60_end();
        if (t60Way ? (daBk_player_way_check(i_this) && (i_this->m1212 & 0x30) && !r29) : i_this->mJumpMove == 0) {
            if (t60Way && i_this->mJumpMove != 0) {
                anm_init(i_this, dRes_INDEX_BK_BCK_BK_RUN_e, 10.0f, 2, l_bkHIO().mRunAnmSpeed,
                         dRes_INDEX_BK_BAS_BK_RUN_e);
                bk_t60_run_speed_catch_up(i_this, scaleMag, t60TurnHeld); /* true60: the ramp's earlier-step share */
                i_this->mJumpMove = 0;
            }
            /* true60 (first-step input addendum): the attack-range decision reads Link: tick30 step (the
             * way check above still picks this step's run / jump-run motion from the step's state) */
            if (i_this->mPlayerDistance < l_bkHIO().mAttackDist && bk_t60_end()) {
                i_this->dr.mMode = 2;
                i_this->mTimers[4] = 50;
            }
            break;
        } else {
            if (t60Way && i_this->mJumpMove == 0 && i_this->mHoldWeapon != 0) {
                i_this->mJumpMove = 1;
                i_this->mJumpStep = 0;
                t60Launch = true;
            }
        }

        if (i_this->mHoldWeapon != 0) {
            i_this->speedF = l_bkHIO().mJumpRunSpeed;

            switch (i_this->mJumpStep) {
            case 0:
                i_this->mJumpStep = 1;
                bk_t60_jump_turn(i_this, t60TurnHeld, t60Launch); /* true60: the frame's turn */
                anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP2_e, 5.0f, 0, 1.0f,
                         dRes_INDEX_BK_BAS_BK_JUMP2_e);
                if (t60Launch) /* true60: the launch decided on tick30 catches up the frame's earlier steps */
                    bk_t60_run_catch_up(i_this);
                break;
            case 1:
                /* true60 (pair 47C5B4B4 frame 266): the landing reads
                 * the ground answer of the frame's first step (the previous frame's BG check, A's), held over the frame
                 * (bk_t60_ground_held). Read live, a landing met by the first step's BG check was decided on the tick30
                 * step, one frame before A (B 3769 / A 3770), and the next launch came a frame early with another yaw.
                 * Negative control WWHD_T60_BKLAND_NEGCTRL=1 (the live read). dt = 1: the live read (the original). */
                if ((bk_t60_land_nc() ? i_this->dr.mAcch.ChkGroundHit() != 0
                                      : bk_t60_ground_held(i_this, i_this->dr.mAcch.ChkGroundHit() != 0)) &&
                    en_t60_isStop(i_this->mpMorf)) {
                    anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP2_e, 0.0f, 0, 1.0f,
                             dRes_INDEX_BK_BAS_BK_JUMP2_e);
                    i_this->mJumpStep = 2;
                    i_this->mSmokeTimer = l_bkHIO().mSmokeTimerBase + 2;
                    i_this->mSmokeType = 4;
                }
                break;
            case 2:
                bk_t60_jstop_note(i_this); /* true60: the frame start's speed, for the tick30 step */
                i_this->speedF = 0.0f;
                {
                    const s16 t60Before = i_this->current.angle.y; /* true60 */
                    bk_t60_jump_turn(i_this, t60TurnHeld); /* true60: after the prelude's turn */
                    bk_t60_jstop_turn(i_this, t60Before); /* true60: its share, for the tick30 restore */
                }
                if (i_this->dr.mAcch.ChkGroundHit() && en_t60_isStop(i_this->mpMorf)) {
                    i_this->mJumpStep = 1;
                    anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, 0, 1.0f,
                             dRes_INDEX_BK_BAS_BK_JUMP1_e);
                    i_this->speed.y = cM_rndF(REG8_F(7) + 10.0f) + 65.0f + REG8_F(8);
                    bk_t60_jlaunch_note(i_this); /* true60: the pre-add angle for the held tick30 step */
                    if ((i_this->mGroundCheck & 0xC) == 0) {
                        s16 temp = (s16)gabi::ftoi(cM_rndFX(REG6_F(13) + 3000.0f));
                        i_this->current.angle.y = i_this->current.angle.y + temp;
                    }
                    fopAcM_monsSeStart(i_this, JA_SE_CV_BK_JUMP, 0);
                }
                break;
            }
        }

        if (i_this->mPlayerDistance < l_bkHIO().mFightDist && bk_t60_end()) { /* true60: reads Link: tick30 step */
            i_this->dr.mAction = 5;
            i_this->dr.mMode = 0;
            return;
        }
        break;
    }
    case 2:
        /* true60: this mode only decides (Link's distance and stick): its body on the tick30 step */
        if (!bk_t60_end())
            break;
        i_this->mJumpMove = 0;

        if (cM_rndF(1.0f) < 0.3f && i_this->mHoldWeapon == 0) {
            i_this->dr.mMode = 8;
            wait_set(i_this);
            i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(20.0f) + 20.0f);
            break;
        }
        /* HD: the mHoldWeapon/mBareHanded branches play the same animation (merged) */
        if ((i_this->mGroundCheck & 0xC) == 0 && std::fabs(stickPosX) > 0.1f) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_WALK2_e, 10.0f, 2, 1.0f,
                     dRes_INDEX_BK_BAS_BK_WALK2_e);
            if (stickPosX > 0.0f) {
                i_this->dr.mMode = 5;
            } else {
                i_this->dr.mMode = 6;
            }
        } else if (i_this->mPlayerDistance < l_bkHIO().mFightDist) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_WALK2_e, 10.0f, 2, 1.0f,
                     dRes_INDEX_BK_BAS_BK_WALK2_e);
            i_this->dr.mMode = 4;
        } else {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_WALK2_e, 10.0f, 2, 1.0f,
                     dRes_INDEX_BK_BAS_BK_WALK2_e);
            i_this->dr.mMode = 3;
        }
        i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(20.0f) + 20.0f);
        break;
    case 3:
        cLib_addCalc2(&i_this->speedF, l_bkHIO().mFightWalkSpeed, 1.0f, 20.0f);
        if (r29) {
            i_this->speed.y = REG0_F_(16) + 100.0f;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, 0, 1.0f,
                     dRes_INDEX_BK_BAS_BK_JUMP1_e);
            fopAcM_monsSeStart(i_this, JA_SE_CV_BK_JUMP, 0);
            i_this->dr.mMode = 33;
            break;
        }
        if (i_this->mTimers[1] == 0) {
            i_this->dr.mMode = 2;
        }
        break;
    case 33:
        if (i_this->dr.mAcch.ChkGroundHit()) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP2_e, 0.0f, 0, 1.0f,
                     dRes_INDEX_BK_BAS_BK_JUMP2_e);
            i_this->dr.mMode = i_this->dr.mMode + 1;
        }
        break;
    case 34:
        i_this->speedF = 0.0f;
        if (en_t60_isStop(i_this->mpMorf)) {
            i_this->dr.mMode = 3;
        }
        break;
    case 4:
        if ((i_this->mGroundCheck & 0x2) == 0) {
            cLib_addCalc2(&i_this->speedF, -l_bkHIO().mFightWalkSpeed, 1.0f, 20.0f);
            if (i_this->mTimers[1] == 0) {
                i_this->dr.mMode = 2;
            }
            break;
        }
        i_this->dr.mMode = 3;
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_WALK2_e, 10.0f, 2, 1.0f,
                 dRes_INDEX_BK_BAS_BK_WALK2_e);
        break;
    case 5:
    case 6:
        if (i_this->dr.mMode == 5) {
            if ((i_this->mGroundCheck & 0x4) == 0) {
                i_this->mMoveAngleOffset = 0x4000;
            } else {
                i_this->mTimers[1] = 0;
            }
        } else {
            if ((i_this->mGroundCheck & 0x8) == 0) {
                i_this->mMoveAngleOffset = -0x4000;
            } else {
                i_this->mTimers[1] = 0;
            }
        }
        cLib_addCalc2(&i_this->speedF, l_bkHIO().mSideStepSpeed, 1.0f, 30.0f);
        if (i_this->mTimers[1] == 0) {
            i_this->dr.mMode = 2;
        }
        break;
    case 8:
        i_this->speedF = 0.0f;
        if (i_this->mTimers[1] == 0) {
            i_this->dr.mMode = 2;
        }
        break;
    }

    if (i_this->dr.mMode >= 3 && i_this->mSideStepTimer <= 2) {
        /* true60 (first-step input addendum): the range decisions read Link: tick30 step (the guard / dodge
         * reaction below stays on the first step, the first-step AI-timer rule: it launches the dodge jump) */
        const bool t60Decide = bk_t60_end();
        if (t60Decide && i_this->mPlayerDistance > l_bkHIO().mAttackDist + 75.0f) {
            i_this->dr.mAction = 0;
            i_this->dr.mMode = 0;
            path_check(i_this, 0);
        }
        f32 dist = i_this->mPlayerDistance;
        f32 m030 = l_bkHIO().mFightDist;
        if (t60Decide && dist < m030 + 62.5f && dist > m030 - 62.5f) {
            if (i_this->mTimers[4] == 0) {
                i_this->mTimers[4] = l_bkHIO().mAttackChanceWait;
                if (cM_rndF(100.0f) < l_bkHIO().mAttackChance) {
                    i_this->dr.mAction = 5;
                    i_this->dr.mMode = 0;
                }
            }
        }
        /* true60 (the per-step chance rule, the first-step AI-timer rule): the guard / dodge reaction to the player's cut draws each frame
         * while the cut lasts: decided once per 30 Hz frame, on its first step, before the frame's motion (the original
         * call's decisions; bk_t60_frame_work) */
        if (i_this->mRecentHitTimer == 0 && bk_t60_frame_work(i_this, t60Entry)) {
            bool r27 = false;
            learn_check() = i_this->mCutBits;
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
            if (i_this->mHoldWeapon && daBk_player_way_check(i_this) &&
                (r27 || (daPy_getCutType(player) != 0 && (cc_pl_cut_bit_get() & i_this->mCutBits) &&
                         dAttention_Lockon(attention) &&
                         (fopAc_ac_c*)i_this == dAttention_LockonTarget(attention, 0)))) {
                /* GHS: `<= 0.5f` branches on `> 0.5f` */
                if (i_this->mHasShield != 0 && (!(cM_rndF(1.0f) > 0.5f) || l_bkHIO().mAlwaysGuard != 0)) {
                    i_this->dr.mAction = 10;
                    i_this->dr.mMode = 0;
                    if (daPy_getCutType(player) == daPy_CUT_TYPE_JUMPCUT_SWORD) {
                        i_this->mTimers[1] = 0x1E;
                    } else {
                        i_this->mTimers[1] = 0x0F;
                    }
                } else {
                    i_this->mNoHitTimer = 0xA;
                    if ((ground_4_check(i_this, 4, i_this->current.angle.y, 200.0f) & 0xD) == 0) {
                        i_this->dr.mAction = 5;
                        attack_set(i_this, 1);
                        anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, 0, 1.0f,
                                 dRes_INDEX_BK_BAS_BK_JUMP1_e);
                        i_this->dr.mMode = -10;
                        i_this->speedF = REG0_F_(3) + -60.0f;
                        i_this->speed.y = REG0_F_(4) + 80.0f;
                        fopAcM_monsSeStart(i_this, JA_SE_CV_BK_JUMP, 0);
                    }
                }
            }
        }
    }

    /* true60 (the 30 Hz timer rule): the push-back count is a per-frame count: counted (or cleared) once per 30 Hz
     * frame, on the tick30 step (first-step AI-timer addendum: order beats first-step placement): it reads
     * Link's distance / view and comes after the mode 2 body and the mode >= 3 range decisions (tick30) in A's call;
     * on the first step its push set the action there, the tick30 step was held and those decisions were lost (the
     * Moblin's case, pair frame 41). OSHI is no launch. The frame's last dispatch call: the tick30 step, or the
     * in-between step whose call changed the state before it (its tick30 step held; d_a_bk_t60.h bk_t60_last_call_once). */
    if (bk_t60_last_call_once(&i_this->mOshiCount, bk_t60_ai_state(i_this) != t60Entry)) {
        if (i_this->mHoldWeapon != 0 && i_this->mPlayerDistance < l_bkHIO().mFightDist - 62.5f &&
            daBk_player_view_check(i_this, &i_this->dr.mpLookTarget->current.pos, i_this->mTargetAngleY,
                                   l_bkHIO().mViewAngle)) {
            i_this->mOshiCount = i_this->mOshiCount + 1;
            if (i_this->mOshiCount >= (s16)(0x19 + REG0_S(0))) {
                if (cM_rndF(1.0f) < REG0_F_(0) + 0.5f &&
                    (ground_4_check(i_this, 4, i_this->current.angle.y, 200.0f) & 0xD) == 0) {
                    i_this->dr.mAction = 7;
                    i_this->dr.mMode = 0;
                }
                i_this->mOshiCount = 0;
            }
        } else {
            i_this->mOshiCount = 0;
        }
    }
    if (daBk_player_bg_check(i_this, &i_this->dr.mpLookTarget->current.pos)) {
        i_this->dr.mAction = 0;
        i_this->dr.mMode = 0;
        path_check(i_this, 0);
    }
    if (i_this->mHoldWeapon == 0 && daBk_wepon_view_check(i_this)) {
        i_this->dr.mAction = 12;
        i_this->dr.mMode = -1;
    }
    if (bk_t60_end() && daBk_bomb_view_check(i_this)) { /* true60: reads the bombs and Link: tick30 step */
        i_this->dr.mAction = 9;
        i_this->dr.mMode = 0;
    }
    i_this->mGroundCheck = ground_4_check(i_this, 4, i_this->current.angle.y, REG6_F(7) + 90.0f);
    if (i_this->mSideStepTimer != 0) {
        if (std::fabs((f32)i_this->speedF) < 30.0f) {
            if (i_this->mSideStepDir == 0) {
                i_this->dr.mLookMode = 3;
            } else if (i_this->mSideStepDir == 1) {
                i_this->dr.mLookMode = 4;
            } else if (i_this->mFrameCounter & 0x10) {
                i_this->dr.mLookMode = 3;
            } else {
                i_this->dr.mLookMode = 4;
            }
            cLib_addCalcAngleS2(&i_this->mHeadAngleZ, 12000, 2, 0x1800);
        } else {
            i_this->dr.mLookMode = 1;
        }
    } else {
        i_this->dr.mLookMode = 1;
        if (i_this->mSideStepWait == 0) {
            /* true60: the wait's draw at A's place, after the tick30 range decisions (d_a_bk_t60.h bk_t60_sswait_defer) */
            if (!bk_t60_sswait_defer(i_this))
                i_this->mSideStepWait = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
            if (i_this->mGroundCheck == 4) {
                i_this->mSideStepDir = 0;
                i_this->mSideStepTimer = 0x10;
            } else if (i_this->mGroundCheck == 8) {
                i_this->mSideStepDir = 1;
                i_this->mSideStepTimer = 0x10;
            } else if (i_this->mGroundCheck == 2) {
                i_this->mSideStepDir = 2;
                i_this->mSideStepTimer = 0x20;
            }
        }
    }
    if (bk_t60_sswait_due(i_this)) { /* true60: the first step's side step: its wait drawn here, A's place */
        bk_t60_sswait_clear(i_this);
        i_this->mSideStepWait = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
    }
}

/* yari_hit_check (inline): the spear's attack sphere. true60: prev = mAttackFrame before this step's
 * add, frameEnd = the value the 30 Hz frame ends on (bk_t60_attack_advance) */
static inline fopAc_ac_c* yari_hit_check(bk_class* i_this, f32 prev, f32 frameEnd) {
    i_this->m11F0 = 0;
    i_this->mAtPosOld.copy(i_this->mAtPos);
    if (i_this->mAttackDir < 0) {
        return nullptr;
    }

    if (i_this->mAttackKind == 2) {
        i_this->mAtPos.copy(i_this->dr.mJntPos[0xE]);
    } else {
        i_this->mAtPos.copy(i_this->mWeaponTipPos);
    }
    bk_t60_at_frame_start(i_this);

    if (i_this->mAtHitWait != 0) {
        return nullptr;
    }

    /* true60: the At sphere is swept once per 30 Hz frame (with an unconverted
     * player it meets it only in the tick30 collision Move anyway): the attack keeps the original's 30 Hz grid. The frame's registration (start, or the swing since the last one, at
     * the frame-end attack frame) goes on the tick30 step; its hit is read, the window tested with the frame's end
     * value and the swing sound started on the next frame's first step, where the original frame reads them. */
    bool grid = bk_t60_at_grid();
    if (grid && !t60::first_step() && !t60::tick30()) {
        return nullptr;
    }
    bool readStep = !grid || t60::first_step();
    f32 frame = grid && t60::first_step() ? frameEnd : (f32)i_this->mAttackFrame;
    if (frame < i_this->mAtStartFrame || frame > i_this->mAtEndFrame) {
        return nullptr;
    }

    i_this->m11F0 = (u8)(i_this->m11F0 << 1);
    i_this->mWeaponSph.SetAtSpl(bk_at_kind[i_this->mAttackKind]);
    /* true60: the swing sound once, on the step whose attack frame entered the start frame (the animation crossing-check rule) */
    if (readStep && gabi::ftoi(frame) == gabi::ftoi(i_this->mAtStartFrame) &&
        (grid || t60::dt() == 1.0f || gabi::ftoi(prev) != gabi::ftoi(i_this->mAtStartFrame) ||
         (prev == frame && t60::tick30()))) {
        s32 se = bk_attack_go_SE[i_this->mAttackKind];
        if (se != -0xDCF) {
            fopAcM_monsSeStart(i_this, se, 0);
        }
    }

    if (grid) {
        if (t60::first_step()) {
            /* true60: the original's call registers the sweep here, before its read; a state change after it holds the
             * tick30 step, which registers the frame's sweep anyway (d_a_bk_t60.h bk_t60_wsph_note, Bk_move) */
            if (i_this->mAtStarted != 0)
                bk_t60_wsph_note(i_this, 1);
            if (i_this->mAtStarted != 0 && i_this->mWeaponSph.ChkAtHit()) {
                i_this->mAtHitWait = 5;
                void* hitObj = dCcD_GObjInf_GetAtHitObj(&i_this->mWeaponSph);
                return hitObj ? cCcD_Obj_GetAc(hitObj) : nullptr;
            }
            return nullptr;
        }
        if (i_this->mAtStarted == 0) {
            Vec3f t60Eye;
            if (bk_t60_at_start_eye(i_this, &t60Eye)) { /* true60: the first step's eyePos, from a guest stack slot */
                gabi::Local<cXyz> eye;
                *eye.get() = t60Eye;
                dCcD_Sph_StartCAt(&i_this->mWeaponSph, eye.get());
            } else {
                dCcD_Sph_StartCAt(&i_this->mWeaponSph, &i_this->eyePos);
            }
            i_this->mAtStarted = i_this->mAtStarted + 1;
        } else {
            dCcD_Sph_MoveCAt(&i_this->mWeaponSph, &i_this->mAtPos);
            dComIfG_Ccsp_Set(&i_this->mWeaponSph);
            if (i_this->mHasMachete != 0) {
                dCcMassS_Mng_Set(gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS), &i_this->mWeaponSph, 3);
            }
        }
        return nullptr;
    }

    if (i_this->mAtStarted == 0) {
        /* HD: starts the At sphere at eyePos (GameCube: mAtPos) */
        dCcD_Sph_StartCAt(&i_this->mWeaponSph, &i_this->eyePos);
        i_this->mAtStarted = i_this->mAtStarted + 1;
    } else {
        dCcD_Sph_MoveCAt(&i_this->mWeaponSph, &i_this->mAtPos);
        dComIfG_Ccsp_Set(&i_this->mWeaponSph);
        if (i_this->mHasMachete != 0) {
            dCcMassS_Mng_Set(gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS), &i_this->mWeaponSph, 3);
        }
        if (i_this->mWeaponSph.ChkAtHit()) {
            i_this->mAtHitWait = 5;
            void* hitObj = dCcD_GObjInf_GetAtHitObj(&i_this->mWeaponSph);
            return hitObj ? cCcD_Obj_GetAc(hitObj) : nullptr;
        }
    }
    return nullptr;
}

/* inlined into fight */
// Guest EA arithmetic: the attack info table + mAttackStep * sizeof(attack_info_s) wraps at 32 bits as on the console (a host
// index on a NULL table reached host addresses past 4 GB).
static inline void fight_attack(bk_class* i_this, fopAc_ac_c* player) {
    i_this->mWeaponSph.SetAtAtp(bk_attack_AP[i_this->mAttackKind]);
    f32 f1 = sead::elementAt<attack_info_s>(gabi::ea(bkFight_attackInfo(i_this->mAttackKind)), u32(i_this->mAttackStep)).speed;
    f32 attackPrev = i_this->mAttackFrame, attackEnd = attackPrev;
    /* true60: the ground answer A's call reads (the previous frame's BG check), held over the frame (d_a_bk_t60.h) */
    if (bk_t60_ground_held(i_this, i_this->dr.mAcch.ChkGroundHit() != 0) ||
        (i_this->mAttackKind != 1 && i_this->mAttackKind != 2)) {
        i_this->mAttackFrame = bk_t60_attack_advance(i_this, f1, &attackEnd); /* true60: + f1 x dt (the animation-rate rule) */
        /* true60: the bare-handed jump's launch frame decays the launch speed once, the whole frame's step (d_a_bk_t60.h) */
        if (!bk_t60_launch_decay_skip(i_this)) {
            f32 t60Dt = bk_t60_launch_decay_dt(i_this);
            cLib_addCalc2(&i_this->speedF, 0.0f, 1.0f, 20.0f);
            t60_set_dt(t60Dt);
        }
    }
    if (i_this->mAttackFrame > i_this->mLookStartFrame &&
        daBk_player_view_check(i_this, &i_this->dr.mpLookTarget->current.pos, i_this->mTargetAngleY,
                               l_bkHIO().mViewAngle)) {
        i_this->dr.mLookMode = 1;
    }

    if (i_this->mAttackDir > 0) {
        bkHIO_c& hio = l_bkHIO();
        if (i_this->mAttackKind == 0) {
            int r3 = hio.mBlur0Start + hio.mBlur0ALen + hio.mBlur0BLen + hio.mBlur0TailLen;
            if (gabi::ftoi(i_this->mAttackFrame) >= hio.mBlur0Start && gabi::ftoi(i_this->mAttackFrame) <= r3) {
                i_this->mBlurOn = 1;
                if (gabi::ftoi(i_this->mAttackFrame) >= (hio.mBlur0Start + hio.mBlur0ALen) &&
                    gabi::ftoi(i_this->mAttackFrame) < (hio.mBlur0Start + hio.mBlur0ALen + hio.mBlur0BLen)) {
                    i_this->mBlurUseB = 1;
                }
                int r0 = gabi::ftoi(i_this->mAttackFrame) - hio.mBlur0Start;
                if (r0 < 10) {
                    i_this->mBlurScaleX = hio.mBlur0ScaleX[r0];
                }
            }
        } else if (i_this->mAttackKind == 1) {
            int r3 = hio.mBlur1Start + hio.mBlur1ALen + hio.mBlur1BLen + hio.mBlur1TailLen;
            if (gabi::ftoi(i_this->mAttackFrame) >= hio.mBlur1Start && gabi::ftoi(i_this->mAttackFrame) <= r3) {
                i_this->mBlurOn = 1;
                if (gabi::ftoi(i_this->mAttackFrame) >= (hio.mBlur1Start + hio.mBlur1ALen) &&
                    gabi::ftoi(i_this->mAttackFrame) < (hio.mBlur1Start + hio.mBlur1ALen + hio.mBlur1BLen)) {
                    i_this->mBlurUseB = 1;
                }
                int r0 = gabi::ftoi(i_this->mAttackFrame) - hio.mBlur1Start;
                if (r0 < 10) {
                    i_this->mBlurScaleX = hio.mBlur1ScaleX[r0];
                }
            }
        }
    }

    if ((i_this->mAttackFrame < i_this->mTurnEndFrame) || i_this->mTurnAlways != 0) {
        i_this->dr.mTurnTargetY = i_this->mTargetAngleY;
    }

    bk_t60_kdturn_note(&i_this->current.angle.y, i_this->current.angle.y); /* true60 */
    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 4, 0x800);

    u8 attackType = 0;
    f32 startFrame = 1000.0f;
    f32 endFrame = 1000.0f;
    if (i_this->mAttackKind == 0) {
        startFrame = REG0_F_(8) + 12.0f;
        endFrame = REG0_F_(9) + 25.0f;
        attackType = 2;
    } else if (i_this->mAttackKind == 1) {
        startFrame = REG0_F_(10);
        endFrame = REG0_F_(11) + 10.0f;
        attackType = 2;
    }
    /* setBtAttackData(startFrame, endFrame, 10000.0f, attackType) */
    i_this->mBtStartFrame = startFrame;
    i_this->mBtAttackType = attackType;
    i_this->mBtEndFrame = endFrame;
    i_this->mBtMaxDis = 10000.0f;
    i_this->mBtMaxDis = l_bkHIO().mAttackBtMaxDis;
    i_this->mBtNowFrame = i_this->mAttackFrame;

    dComIfGp_get(); /* HD: leftover call */
    fopAc_ac_c* hitActor = yari_hit_check(i_this, attackPrev, attackEnd);
    if (hitActor != nullptr) {
        if (fpcM_GetName(hitActor) == fpcNm_PLAYER_e) {
            if (daPy_checkPlayerGuard(player) && (i_this->mAttackKind == 0 || i_this->mAttackKind == 1)) {
                i_this->mpMorf->setPlaySpeed(-1.0f);
                if (i_this->mSmokeTimer != 0) {
                    i_this->mSmokeTimer = l_bkHIO().mSmokeTimerBase + 6;
                }
                i_this->mAttackDir = -1;
                bk_t60_play_frame(i_this); /* true60: the bounce's extra play is one frame */
                if (i_this->mHoldWeapon != 0 && i_this->mHasMachete == 0 && cM_rndF(1.0f) < 0.5f) {
                    i_this->mDropWeapon = 1;
                    i_this->dr.mAction = 8;
                    i_this->dr.mMode = -10;
                    i_this->mTimers[0] = 10;
                    i_this->mTimers[1] = 100;
                }
            }
        } else if (fpcM_GetName(hitActor) == fpcNm_BK_e) {
            i_this->mHitActorId = fopAcM_GetID(hitActor);
        }
    } else {
        i_this->mWeaponSph.ClrAtHit();
        if (i_this->m11F0 != 0) {
            if (i_this->m11F0 == 2) {
                i_this->mpMorf->setPlaySpeed(-1.0f);
                i_this->mBlurOn = 0;
                if (i_this->mSmokeTimer != 0) {
                    i_this->mSmokeTimer = l_bkHIO().mSmokeTimerBase + 6;
                }
                i_this->mAttackDir = -1;
                bk_t60_play_frame(i_this); /* true60: the bounce's extra play is one frame */
            } else {
                i_this->dr.mAction = 0;
                path_check(i_this, 0);
                wait_set(i_this);
                i_this->dr.mMode = 2;
            }
            i_this->mSideStepDir = 1;
            i_this->mSideStepTimer = 0x10;
            gabi::Local<cXyz> sp18;
            sp18->x = 1.0f;
            sp18->y = 1.0f;
            sp18->z = 1.0f;
            dComIfGp_particle_set(0xC /* dPa_name::ID_AK_JN_NG */, &i_this->m11E4, nullptr, sp18);
        }
    }

    /* true60 (the animation-driven decision rule at play-after enemies): the attack animation's end (attack1 plays at
     * 1.2, so its end falls inside a frame) is decided on the frame's first step, on the previous frame's end state, and
     * held over the frame (d_a_enemy_anm_t60.h) */
    if (!en_t60_isStop(i_this->mpMorf)) {
        return;
    }

    if ((i_this->mAttackStep == 2 && i_this->mAttackDir > 0) || (i_this->mAttackDir < 0 && i_this->mAttackStep == 0) ||
        (i_this->mAttackKind == 2 && i_this->mAttackStep == 1)) {
        /* true60: the attack's end reads Link's distance / view: decided on the tick30 step (the
         * held end, animation-driven decision, stays true over the frame) */
        if (!bk_t60_end()) {
        } else if (i_this->mHitActorId != fpcM_ERROR_PROCESS_ID_e) {
            i_this->dr.mAction = 14;
            i_this->dr.mMode = 0;
        } else if (i_this->mPlayerDistance < l_bkHIO().mAttackDist) {
            if (daBk_player_view_check(i_this, &i_this->dr.mpLookTarget->current.pos, i_this->mTargetAngleY,
                                       l_bkHIO().mViewAngle)) {
                if (cM_rndF(1.0f) < 0.8f || i_this->mAttackDir < 0) {
                    i_this->mTimers[1] = 0;
                    i_this->dr.mAction = 4;
                    i_this->dr.mMode = 2;
                } else {
                    i_this->dr.mMode = 0;
                }
            } else {
                if (i_this->mHoldWeapon != 0 || i_this->mBareHanded != 0) {
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
    } else if (bk_t60_ground_held(i_this, i_this->dr.mAcch.ChkGroundHit() != 0)) {
        /* true60 (pair f119 / f138): the next attack step after the landing reads the
         * ground state A's call reads (the previous frame's BG check), held over the frame (bk_t60_ground_held, as the
         * advance above): the live test saw the first step's landing and switched on the tick30 step a frame early
         * (kind 1 step 0 -> 1 at 119 instead of 120; every later step and the attack's end followed a frame early) */
        attack_info_s* r6 = bkFight_attackInfo(i_this->mAttackKind);
        f32 speed;
        if (i_this->mAttackDir > 0) {
            i_this->mAttackStep = i_this->mAttackStep + 1;
            speed = sead::elementAt<attack_info_s>(gabi::ea(r6), u32(i_this->mAttackStep)).speed;
        } else {
            i_this->mAttackStep = i_this->mAttackStep - 1;
            speed = -sead::elementAt<attack_info_s>(gabi::ea(r6), u32(i_this->mAttackStep)).speed;
        }
        anm_init(i_this, sead::elementAt<attack_info_s>(gabi::ea(r6), u32(i_this->mAttackStep)).bckFileIdx, 0.0f, 0, speed,
                 sead::elementAt<attack_info_s>(gabi::ea(r6), u32(i_this->mAttackStep)).soundFileIdx);
    }
    return;
}

/* 020A1AAC */
void fight(bk_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    switch ((s16)i_this->dr.mMode) {
    case -10:
        /* true60 (after pair, frame form as pt / esa): the landing that ends the jump /
         * knock-back is decided once per frame from the ground state the whole previous frame's move left (its tick30
         * ground check): A's call reads that state before its move, lands during the move and switches next frame
         * (f94 / f95). At N steps a half step's ground hit made the tick30 step switch a frame early, and the tick30 speed
         * term (c_damagereaction speed-ramp position-gap (3)) then took back the frame's move (21.66 off). The first step's answer is held over
         * the frame (bk_t60_ground_held, as fight_attack's ground test); dt = 1: the live test (the original) */
        if (bk_t60_ground_held(i_this, i_this->dr.mAcch.ChkGroundHit() != 0)) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP2_e, 0.0f, 0, 1.0f,
                     dRes_INDEX_BK_BAS_BK_JUMP2_e);
            i_this->dr.mMode = -9;
            i_this->speedF = 0.0f;
        }
        break;
    case -9:
        if (en_t60_isStop(i_this->mpMorf)) {
            i_this->dr.mMode = 1;
            i_this->mTimers[2] = 8;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_JATTACK1_e, 2.0f, 0, 1.0f, -1);
            i_this->speedF = REG0_F_(5) + 80.0f;
            i_this->speed.y = REG0_F_(6) + 115.0f;
            /* true60 (pair f99): the landing jump's launch decays its speed once, the
             * whole frame's step, as mode 0's (d_a_bk_t60.h bk_t60_launch_note; A 80 -> 60, B was 70) */
            bk_t60_launch_note(i_this);
            /* true60 (pair f99): mode 1's body runs in this call (A's one call: the
             * attack frame's whole advance 0 -> 1, the turn): not held on the frame's later steps, as mode 0's
             * fall-through (B was 0.5 behind in mAttackFrame from f99) */
            bk_t60_ai_ran(i_this);
            fight_attack(i_this, player);
        }
        break;
    case 0:
        if (i_this->mHoldWeapon != 0) {
            attack_set(i_this, 0);
        } else {
            attack_set(i_this, 2);
            i_this->speedF = REG0_F_(7) + 70.0f;
            i_this->speed.y = REG0_F_(8) + 105.0f;
            bk_t60_launch_note(i_this); /* true60: the launch frame's decay (d_a_bk_t60.h) */
            fopAcM_monsSeStart(i_this, JA_SE_CV_BK_ATTACK_L, 0);
        }
        i_this->dr.mMode = 1;
        i_this->mTimers[2] = 8;
        bk_t60_ai_ran(i_this); /* true60: mode 1's body runs in this call (fall-through): not held */
        /* fall-through */
    case 1:
        fight_attack(i_this, player);
        break;
    }
}

/* 020A27F8 */
void p_lost(bk_class* i_this) {
    dComIfGp_get();
    i_this->dr.mLookMode = 0;
    switch ((s16)i_this->dr.mMode) {
    case -10:
        /* true60 (the animation-driven decision rule: pair): the guard bounce's reversed attack animation (attack1 at
         * 1.2, reversed from a fractional frame) ends inside a frame: decided on the frame's first step on the previous
         * frame's end state and held over the frame (d_a_enemy_anm_t60.h), as fight_attack's end */
        if (en_t60_isStop(i_this->mpMorf) || i_this->mTimers[0] == 0) {
            i_this->mTimers[0] = 5;
            i_this->dr.mMode = i_this->dr.mMode + 1;
        }
        break;
    case -9:
        if (i_this->mTimers[0] != 0) {
            break;
        }
        /* fall-through */
    case 0:
        i_this->dr.mMode = 1;
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO2_e, 5.0f, 2, 1.0f,
                 dRes_INDEX_BK_BAS_BK_KYORO2_e);
        i_this->speedF = 0.0f;
        i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(30.0f) + 30.0f);
        bk_t60_ai_ran(i_this); /* true60: mode 1's body runs in this call (fall-through): not held */
        /* fall-through */
    case 1: {
        /* true60 (the animation crossing-check rule): the animation frame entered 11 / 25 on this step */
        f32 frame = i_this->mpMorf->getFrame(), rate = i_this->mpMorf->getPlaySpeed();
        if ((bk_t60_frame_is(frame, rate, 0xB) || bk_t60_frame_is(frame, rate, 0x19)) && cM_rndF(1.0f) < 0.5f) {
            fopAcM_monsSeStart(i_this, JA_SE_CV_BK_SEARCH, 0);
        }
        if (i_this->mTimers[1] == 0) {
            i_this->dr.mAction = 0;
            path_check(i_this, 0);
            wait_set(i_this);
            i_this->dr.mMode = 2;
        }
        break;
    }
    }

    /* true60: reads Link: tick30 step */
    if (i_this->mTimers[1] < 10 && bk_t60_end() &&
        daBk_player_view_check(i_this, &i_this->dr.mpLookTarget->current.pos, i_this->mTargetAngleY,
                               l_bkHIO().mViewAngle)) {
        i_this->mTimers[1] = 0;
        i_this->dr.mAction = 4;
        i_this->dr.mMode = 2;
    }
}

/* 020A2A54 */
void b_nige(bk_class* i_this) {
    /* daBk_bomb_check (inline) */
    fopAc_ac_c* bomb = search_bomb(i_this, 0);
    i_this->mpBomb = bomb;
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
    i_this->dr.mTurnTargetY = cM_atan2s(-x, -z);

    switch ((u16)i_this->dr.mMode) {
    case 0:
        i_this->dr.mMode = 1;
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_HAKKEN_e, 3.0f, 0, 1.0f,
                 dRes_INDEX_BK_BAS_BK_HAKKEN_e);
        fopAcM_monsSeStart(i_this, JA_SE_CV_BK_FOUND_LINK, 0);
        i_this->mTimers[1] = 20;
        bk_t60_ai_ran(i_this); /* true60: mode 1's body (stop, turn away) runs in this call (fall-through): not held */
        /* fall-through */
    case 1:
        i_this->speedF = 0.0f;
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY + 0x8000, 2, 0x3000);
        if (i_this->mTimers[1] == 0) {
            i_this->dr.mMode = 2;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_NIGERU_e, 5.0f, 2, 1.0f,
                     dRes_INDEX_BK_BAS_BK_NIGERU_e);
            fopAcM_monsSeStart(i_this, JA_SE_CV_BK_LOST_BOKO, 0);
        }
        break;
    case 2:
        i_this->speedF = l_bkHIO().mDashSpeed;
        i_this->mSmokeTimer = l_bkHIO().mSmokeTimerBase + 3;
        i_this->mSmokeType = 4;
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 4, 0x1000);
        if (std_sqrtf(gabi::fmadds(x, x, z * z)) > 800.0f) {
            i_this->dr.mMode = 3;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_WAIT_e, 10.0f, 2, 1.0f,
                     dRes_INDEX_BK_BAS_BK_WAIT_e);
        }
        break;
    case 3:
        i_this->speedF = 0.0f;
        i_this->dr.mTurnTargetY = i_this->mTargetAngleY;
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 3, 0x1000);
        if (std_sqrtf(gabi::fmadds(x, x, z * z)) < 700.0f) {
            i_this->dr.mMode = 0;
        }
        break;
    }
}

/* 020A2E04 */
void defence(bk_class* i_this) {
    dComIfGp_get();
    i_this->dr.mLookMode = 1;
    i_this->dr.mTurnTargetY = i_this->mTargetAngleY;
    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->dr.mTurnTargetY, 4, 0x400);

    switch ((u16)i_this->dr.mMode) {
    case 0:
        i_this->dr.mMode = 1;
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_BOUGYO1_e, 0.0f, 0, 1.0f, -1);
        i_this->speedF = 0.0f;
        bk_t60_ai_ran(i_this); /* true60: mode 1's body runs in this call (fall-through): not held */
        /* fall-through */
    case 1:
        i_this->mDefenceSph.SetR(60.0f);
        i_this->mDefenceSphPos.copy(i_this->mShieldPos);
        if (i_this->mTimers[1] == 0) {
            if (cM_rndF(1.0f) < 0.5f) {
                i_this->dr.mAction = 5;
                i_this->dr.mMode = 0;
            } else {
                i_this->dr.mAction = 4;
                i_this->mTimers[1] = 0;
                i_this->dr.mMode = 0;
            }
        }
        break;
    }
}

/* 020A2F68 */
void oshi(bk_class* i_this) {
    dComIfGp_get();
    i_this->dr.mAction = 5;
    attack_set(i_this, 1);
    anm_init(i_this, dRes_INDEX_BK_BCK_BK_JUMP1_e, 2.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_JUMP1_e);
    i_this->dr.mMode = -10;
    i_this->speedF = REG6_F(10) + -90.0f;
    i_this->speed.y = REG6_F(11) + 85.0f;
    fopAcM_monsSeStart(i_this, JA_SE_CV_BK_JUMP, 0);
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
data bk_at_kind 1019148C
data bk_attack_ready_SE 10191498
data bk_attack_go_SE 101914A4
data bk_attack_AP 101914B0
data attack_info 10191528
*/
// clang-format on

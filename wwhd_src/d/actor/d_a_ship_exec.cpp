/**
 * d_a_ship_exec.cpp (WWHD)
 * King of Red Lions: daShip_c::execute
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ship.cpp) to the WWHD layout and verified against cking.rpx.
 */
#include "d/actor/d_a_ship_exec.h"
#include "d/actor/d_a_ship_t60.h"
#include "d/t60conv.h"

u32 g_ship60TillerWholeStep = 0xFFFFFFFFu; /* true60 (frame form) */
u32 g_ship60YExactStep = 0xFFFFFFFFu; /* true60 (frame form): the step whose y the exact frame move set */
u32 g_ship60ProcSets = 0; /* true60 (the 30 Hz visual-motion rule (2)): procedure sets (daShip_c::setProc), host memory */
/* true60 (the 30 Hz visual-motion rule (2)): the procedure set inside the body on an in-between step, held to the next frame's first step */
static struct {
    bool on;
    const void* owner;
    u32 fn;
} s_ship60ProcHold;

static f32 sight_scroll_phase;
static cXyz cannon_top;
struct ShipExecCylinderOffsets {
    be<f32> mOffset[3];
};
static ShipExecCylinderOffsets cyl_offset;
static cXyz sail_offset_storage;
static cXyz sph_offset_storage;
static cXyz top_offset_storage;
static cXyz XZ_top_offset_storage;
static u32 sail_offset_guard;
static u32 sph_offset_guard;
static u32 top_offset_guard;
static u32 XZ_top_offset_guard;
/* g_Counter.mCounter0 (the 30 Hz frame counter): the frame id of the true60 per-frame latches (d_a_ship_t60.h) */
static u32 ship_exec_counter;
/* true60: the per-site carries and latches of execute on a general step length */
static Ship60Carry s_ship60PitchSpeed, s_ship60Pitch, s_ship60ForceTurn, s_ship60RopeCnt;
static Ship60Once s_ship60Rudder;
static Ship60NextMode s_ship60NextMode; /* true60: Link's mode request seen from the next frame (process-order read) */
static Ship60Eye s_ship60Eye; /* true60: KoRL's frame-start eye (ship60_eyePos) */
static Ship60AnmReq s_ship60BodyReq, s_ship60HeadReq; /* true60: the animation requests (ship60_playAnm) */
static Ship60Start s_ship60FlyTurnSpeed; /* true60: the out-of-range turn's frame-start speed (execute, FLY) */
static Ship60Start s_ship60TexScroll; /* true60: the sail texture scroll's frame start (frame form) */
/* true60 (the sailing's frame form; d_a_ship_t60.h): the frame-start values the frame's steps read */
static Ship60Start s_ship60ShapeX0, s_ship60ShapeZ0, s_ship60OldX0, s_ship60OldZ0, s_ship60Pos0X, s_ship60Pos0Z, s_ship60Fwd0,
    s_ship60MovePitch;
/* the frame's forward displacement from its exact move, made on the first step (mFwdVel's frame value) */
static struct {
    u32 stamp;
    f32 dx, dz;
    bool valid;
} s_ship60FwdPred;
/* true60 (the process-order read rule): fopAcM_seenActorAngleY(self, other) (025D68A0) on `other`'s frame-start
 * position when process-order read gives it (ship60_fsActor), its body: |cLib_targetAngleY(self pos, other pos) - shape_angle.y|;
 * else the original call. */
static inline s32 ship60_seenActorAngleY(fopAc_ac_c* self, fopAc_ac_c* other) {
    gabi::Local<cXyz> fs;
    if (!ship60_fsActor(other, 1, fs.get())) {
        return ShipExec_seenActorAngleY(self, other);
    }
    s16 d = (s16)(cLib_targetAngleY(&self->current.pos, fs.get()) - (s16)(u16)self->shape_angle.y);
    return d < 0 ? -(s32)d : (s32)d;
}
/* true60: the step counter of the frame's first step this unit saw (ship60_execRest: the frame's plays after this one) */
static u32 s_ship60ExecFirst = 0xFFFFFFFFu;
static inline int ship60_execRest() {
    if (t60::dt() == 1.0f) {
        return -1;
    }
    if (t60::first_step()) {
        s_ship60ExecFirst = t60_step_counter();
    }
    u32 idx = t60_step_counter() - s_ship60ExecFirst;
    return idx < t60::steps() ? (int)t60::steps() - 1 - (int)idx : -1;
}
/* true60 (the first-step push rule, 2026-11-16): the frame's collision push. The unconverted dCcS Move sets mStts.m_cc_move
 * once per 30 Hz frame; execute clears it after its move (ClrCcMove), so consumed on the frame's first step alone it
 * would move the ship by dt of the push only (fopAcM_posMove(F) takes m_cc_move x dt). Held from the value the
 * frame's first step sees (or the first step that sees one after a frame without it) and given to every step of the
 * frame whole (the posMove takes dt of it), cleared on the frame's last step: the frame's total stays. Host memory. */
static f32 s_ship60CcHold[3];
static const void* s_ship60CcOwner;
template <class V>
static inline void ship60_ccMoveStep(const void* owner, V* cc) {
    if (t60::dt() == 1.0f) {
        return;
    }
    if (s_ship60CcOwner != owner) {
        s_ship60CcOwner = owner;
        s_ship60CcHold[0] = s_ship60CcHold[1] = s_ship60CcHold[2] = 0.0f;
    }
    const bool fresh = (f32)cc->x != 0.0f || (f32)cc->y != 0.0f || (f32)cc->z != 0.0f;
    const bool held = s_ship60CcHold[0] != 0.0f || s_ship60CcHold[1] != 0.0f || s_ship60CcHold[2] != 0.0f;
    if (t60::first_step() || (fresh && !held)) {
        s_ship60CcHold[0] = cc->x;
        s_ship60CcHold[1] = cc->y;
        s_ship60CcHold[2] = cc->z;
    }
    cc->x = s_ship60CcHold[0];
    cc->y = s_ship60CcHold[1];
    cc->z = s_ship60CcHold[2];
    if (t60::tick30()) {
        s_ship60CcHold[0] = s_ship60CcHold[1] = s_ship60CcHold[2] = 0.0f;
    }
}
/* true60 (whirlpool / tornado orbit): the 30 Hz frame snaps the orbit once. setWhirlActor /
 * setTornadoActor re-measure mForceMoveDist / mForceMoveAngle from the ship's position; cM_atan2f is a 1024-entry table
 * read with a truncated index, so the angle lags the true one by ~5 s16 units on average (~1.7 units along a 3600-unit
 * orbit). The chord rescale (k = speedF / |chord|) turns that tangential lag into an inward radial step; the original
 * pays it once a frame, an N-step run N times a frame: the orbit closes faster (octo9: frame 900 radius A 3239.8 /
 * B 3168.8), and the smaller radius raises mForceMoveRate (speed cap) and the turn rate. The frame-anchored form: the
 * frame's first step latches the snapped start (dist, angle) and the frame-start position; every step advances the
 * stored orbit (dist chase and angle x dt, not re-snapped) and places the ship on the chord from the frame-start
 * position to the step's orbit point, rescaled to the frame's travel so far (sum of speedF x dt); the step moves by the
 * difference of the consecutive anchored ends (a correction made after a step, e.g. the BG check, is kept). The last
 * step of a frame of constant speed ends on A's point (sim: radius at 900 frames A 3271.95, N 2 3271.92, N 4 3271.91;
 * the per-step snap gave 3253.2 / 3116.7). Host memory; at dt = 1 not used (the original instructions run). */
struct Ship60Orbit {
    u32 stamp;
    const void* owner;
    bool valid;
    f32 dist, angle;   /* the stored orbit after the previous step (continued, not re-snapped) */
    f32 o0x, o0z;      /* the frame-start position */
    f32 ex, ez;        /* the previous step's anchored end */
    f32 travel, span;  /* sum of speedF x dt, sum of dt in this frame */
};
static Ship60Orbit s_ship60Orbit;
/* a later step of the frame whose previous step ran the orbit: the latch continues */
static inline bool ship60_orbitContinues(const void* owner) {
    return s_ship60Orbit.valid && s_ship60Orbit.owner == owner && !t60::first_step() &&
           t60_step_counter() - s_ship60Orbit.stamp == 1;
}

static inline cXyz* ship_exec_staticVec(be<u32>& guard, cXyz& value, f32 x, f32 y, f32 z) {
    if (guard == 0) {
        guard = 1;
        value.set(x, y, z);
    }
    return &value;
}

/* a float copied with lfs/stfs keeps its bits (a host float copy would quiet a signalling NaN) */
static inline void ship_cpf(be<f32>& d, const be<f32>& s) {
    reinterpret_cast<be<u32>&>(d) = reinterpret_cast<const be<u32>&>(s);
}

/* the stop() of the four particle callbacks */
static inline void ship_stopWaves(daShip_c* s) {
    gabi::at<ShipExecCallbackState>(gabi::ea(s->mWaveR))->stop();
    gabi::at<ShipExecCallbackState>(gabi::ea(s->mWaveL))->stop();
    gabi::at<ShipExecCallbackState>(gabi::ea(s->mSplash))->stop();
    gabi::at<ShipExecCallbackState>(gabi::ea(s->mTrack))->stop();
}

/* the cannon-sight end marker: placed at the stack matrix */
static inline void ship_sightMarker(daShip_c* s) {
    ShipExecEffectModels* effects = gabi::at<ShipExecEffectModels>(gabi::ea(s));
    J3DModel* m = effects->mpFallPointModel;
    if (m != nullptr) {
        J3DModel_setBaseTRMtx(m, mDoMtx_now());
        model_setBaseScale1(effects->mpFallPointModel, 1.0f);
    }
    effects->mpFallPointAnm->play(nullptr, 0, 0);
}

// HD: the cannon sight. Simulates the cannonball from the muzzle in 128 steps (mHD1B24), places a
// sight segment every eighth step (offset by a scrolling phase) and stops at the first enemy
// (process names 0xE1, 0xE2, 0xD1) hit by the step colliders (mRopeSph), at a wall (a line check
// every fourth step) or at the water surface, where the end marker is drawn.
/* true60 (the mixed-pair hit-flag rule, hit-volume audit 2026-11-16): the sight's step colliders hit unconverted enemies (mixed
 * pairs) only in the tick30 collision Move; the hit is read on the next frame's first step and cleared there, so the
 * frame's later steps would see none (the marker jumps past the enemy and the tick30 step registers the colliders
 * beyond it). The first step's hit index is held over the frame's later steps (a later step's own hit still counts).
 * Host memory; unused at dt = 1. */
static s32 s_ship60SightHit = -1;
static const void* s_ship60SightOwner;

static void ship_cannonSight(daShip_c* s) {
    ShipExecSight& sight = ship_exec_sight(s);
    gabi::Local<cXyz> start;
    start->copy(sight.mPosition);
    if ((u8)ship_exec_play()->mEventRun == 0 && ship_isLinkPlayer0() &&
        s->isProc(ShipExecTarget::procCannon)) {
        gabi::Local<dBgS_LinChk> linChk;
        dBgS_LinChk_ct(linChk.get(), SHIP_LINCHK_VT, false);
        gabi::Local<cXyz> previousLinePosition;
        previousLinePosition->copy(sight.mPosition);
        // HD: sight segments scroll by 0.75 per frame over [0, 8).
        if (t60::dt() != 1.0f && (t60::first_step() || s_ship60SightOwner != s)) {
            s_ship60SightOwner = s;
            s_ship60SightHit = -1;
        }
        f32 ph = t60::dt() != 1.0f ? (f32)sight_scroll_phase + 0.75f * t60::dt() : (f32)sight_scroll_phase + 0.75f;
        if (!(ph < 7.999f)) {
            ph = ph - 8.0f;
        }
        sight_scroll_phase = ph;
        daShip_sightPacket_setMtx(s->mHDPacket);
        s32 seg = 0;
        f32 k = 0.5f;
        gabi::Local<cXyz> prev;
        gabi::Local<cXyz> pos;
        gabi::Local<cXyz> w;
        gabi::Local<cXyz> d;
        gabi::Local<cXyz> w2;
        gabi::Local<cXyz> t9c;
        gabi::Local<cXyz> ta8;
        gabi::Local<cXyz> t60;
        for (s32 i = 0; i < 0x80; i++) {
            prev->copy(sight.mPosition);
            s->sightStep();
            pos->copy(sight.mPosition);
            dCcD_Sph* sph = &s->mRopeSph[i];
            sph->SetC(pos.get());
            dComIfG_Ccsp_Set(sph);
            if (sph->ChkCoHit()) {
                fopAc_ac_c* ac = sph->GetCoHitAc();
                if (ac && ((s16)ac->mProfName == ShipExecProcess::fpcNm_RD_e ||
                           (s16)ac->mProfName == ShipExecProcess::fpcNm_DAIOCTA_e ||
                           (s16)ac->mProfName == ShipExecProcess::fpcNm_WZ_e)) {
                    if (t60::dt() != 1.0f) {
                        s_ship60SightHit = i;
                    }
                    mDoMtx_stack_c::transS(pos->x, pos->y, pos->z);
                    ship_sightMarker(s);
                    break;
                }
            }
            if (t60::dt() != 1.0f && i == s_ship60SightHit) {
                /* the mixed-pair hit-flag rule: the frame's held hit */
                mDoMtx_stack_c::transS(pos->x, pos->y, pos->z);
                ship_sightMarker(s);
                break;
            }
            if (i - (i & ~3) == 2) {
                dBgS_LinChk_Set(linChk.get(), previousLinePosition.get(), pos.get(), nullptr);
                if (cBgS_LineCross(dComIfG_Bgsp(), linChk.get())) {
                    ShipExecLineHit* hit = gabi::at<ShipExecLineHit>(gabi::ea(linChk.get()));
                    mDoMtx_stack_c::transS(hit->mCross.x, hit->mCross.y, hit->mCross.z);
                    ship_sightMarker(s);
                    break;
                }
                previousLinePosition->copy(sight.mPosition);
            }
            w->copy(*pos);
            s->getMaxWaterY(w.get());
            s32 n = gabi::ftoi((f32)sight_scroll_phase);
            f32 wy = w->y;
            f32 py = pos->y;
            bool hitWater;
            if ((u32)(i - (i & ~7)) == (u32)n) {
                if (py - wy > 120.0f) {
                    cXyz_mi(pos.get(), d.get(), start.get());
                    cXyz_abs(d.get());
                    mDoMtx_stack_c::transS(pos->x, pos->y, pos->z);
                    daShip_sightPacket_setSegMtx(s->mHDPacket, seg, mDoMtx_now());
                    seg++;
                    continue;
                }
                hitWater = !(wy < py);
            } else {
                hitWater = !(wy < py);
            }
            if (!hitWater) {
                continue;
            }
            w2->copy(*prev);
            s->getMaxWaterY(w2.get());
            f32 a = std::fabs(wy - (f32)sight.mPosition.y);
            f32 c = std::fabs(w2->y - prev->y);
            f32 sum = a + c;
            if (sum != 0.0f) {
                k = a / sum;
            }
            cXyz_ml(&sight.mPosition, t9c.get(), 1.0f - k);
            cXyz_ml(prev.get(), ta8.get(), k);
            cXyz_pl(t9c.get(), t60.get(), ta8.get());
            mDoMtx_stack_c::transS(t60->x, t60->y, t60->z);
            ship_sightMarker(s);
            break;
        }
        for (s32 i = 0; i < 0x80; i++) {
            ShipExec_clearCollisionHit(&s->mRopeSph[i]);
        }
        ship_LinChk_dt(linChk.get());
    } else {
        s_ship60SightHit = -1;
        for (s32 i = 0; i < 0x80; i++) {
            ShipExec_removeCollision(dComIfG_Ccsp(), &s->mRopeSph[i]);
            ShipExec_clearCollisionHit(&s->mRopeSph[i]);
        }
    }
    sight.mPosition.copy(*start);
}

/* the sight launch: muzzle position, angles and velocity (mHD1B24) */
static inline void ship_sightLaunch(daShip_c* s) {
    ShipExecSight& sight = ship_exec_sight(s);
    gabi::Local<cXyz> top;
    PSMTXMultVec(model_getAnmMtx(s->mpCannonModel, VFNCN_JNT_CANON2_e), &cannon_top, top.get());
    s16 ax = s->shape_angle.x + s->mCannonAngleX - 0x4000;
    sight.mPosition.copy(*top);
    sight.mDelay = 8;
    sight.mAngle.y = s->shape_angle.y + s->mCannonAngleY;
    sight.mAngle.z = s->shape_angle.z;
    sight.mAngle.x = ax;
}

/* 02477A24 */
BOOL daShip_c::execute() {
    // HD: in the Tower of the Gods the boat waits at a fixed place
    if (daShip_checkSirenInside_fs()) { /* true60 (the process-order read rule addendum): Link's frame-start room */
        current.pos.x = 4.011528f;
        current.pos.y = -2111.8811f;
        shape_angle.y = 0x4000;
        current.pos.z = -1278.5222f;
        return TRUE;
    }
    // HD: execution waits while Link has the ship-inhibiting state flag.
    // true60 (the process-order read rule addendum): Link's frame-start flag word (f_pc_t60_frame_latch.h; N = 1: the live word).
    if (t60_flatch_load<u32>(gabi::ea(dComIfGp_getLinkPlayer()) + offsetof(ShipExecPlayer, mNoResetFlg1)) & 0x200) {
        return TRUE;
    }
    ship60_nextModeEnter(s_ship60NextMode, this, &mNextMode); /* true60 (the process-order read rule): Link's mode request */
    /* static cXyz sail_offset(0.5f, 155.0f, 50.0f); sph_offset(-5.0f, 0.0f, 0.0f) */
    ship_exec_staticVec(sail_offset_guard, sail_offset_storage, 0.5f, 155.0f, 50.0f);
    cXyz* sph_offset = ship_exec_staticVec(sph_offset_guard, sph_offset_storage, -5.0f, 0.0f, 0.0f);

    mForceMoveRate = 0.0f;
    f32 prev_speedF = speedF;
    mDemoMode = 0xFF;
    mpEvtAtnPos = nullptr;
    f32 prev_fwdvel = mFwdVel;
    /* true60 (frame form): the frame-start state (the first call of the frame latches; the process-order read rule readers) */
    const bool t60ff = ship60_frameForm();
    s16 t60ShapeX0 = 0, t60ShapeZ0 = 0, t60OldX0 = 0, t60OldZ0 = 0;
    f32 t60Pos0X = 0.0f, t60Pos0Z = 0.0f;
    if (t60ff) {
        t60ShapeX0 = (s16)ship60_start(s_ship60ShapeX0, this, ship_exec_counter, (s16)shape_angle.x);
        t60ShapeZ0 = (s16)ship60_start(s_ship60ShapeZ0, this, ship_exec_counter, (s16)shape_angle.z);
        t60OldX0 = (s16)ship60_start(s_ship60OldX0, this, ship_exec_counter, (s16)mOldShapeAngleX);
        t60OldZ0 = (s16)ship60_start(s_ship60OldZ0, this, ship_exec_counter, (s16)mOldShapeAngleZ);
        t60Pos0X = ship60_startF(s_ship60Pos0X, this, ship_exec_counter, current.pos.x);
        t60Pos0Z = ship60_startF(s_ship60Pos0Z, this, ship_exec_counter, current.pos.z);
        prev_fwdvel = ship60_startF(s_ship60Fwd0, this, ship_exec_counter, mFwdVel);
    }

    if (mTornadoID != fpcM_ERROR_PROCESS_ID_e) {
        setTornadoActor();
        mWhirlActor = nullptr;
    } else {
        mTornadoActor = nullptr;
        if (mWhirlID != fpcM_ERROR_PROCESS_ID_e) {
            setWhirlActor();
        } else {
            mWhirlActor = nullptr;
        }
    }

    if ((u8)ship_exec_play()->mEventRun != 0) {
        mStickMVal = 0.0f;
        mStickMAng = 0;
        mEvtStaffId = dComIfGp_evmng_getMyStaffId("Ship", this, 0);
        if (mEvtStaffId != -1) {
            u32 cutName =
                ShipExec_getMyNowCutName(ship_exec_play()->mEventManager, (s32)mEvtStaffId);
            if (cutName) {
                const be<u8>* name = gabi::at<be<u8>>(cutName);
                mDemoMode = (u8)(name[0] * 10 + name[1] - 0x210);
            }
        }
        offStateFlg(daSFLG_UNK2000000_e);
        m03B8 = 0;
    } else {
        // HD: "no control" became "the link player is not the current player"
        if (!ship_isLinkPlayer0()) {
            mStickMVal = 0.0f;
            mStickMAng = 0;
        } else {
            mStickMVal = (f32)ShipExec_getStickValue(0);
            mStickMAng = ShipExec_getStickAngle(0) + 0x8000;
        }
        mEvtStaffId = -1;
        // HD: the crash timer counts down here (GameCube: setCrashData)
        // true60 (the 30 Hz timer rule): a 30 Hz count, taken off on the frame's first step (before the procedures that set it, as
        // in the 30 Hz frame: a set on any step of the frame is the frame-end value)
        if (mCrashTimer != 0 && ship60_first()) {
            mCrashTimer--;
        }
    }
    // HD: also clears the Swift Sail flag (0x80000000)
    // true60: these flags say "this happened during the 30 Hz frame" (jump, landing, cannon shot, crash, Swift Sail
    // change, ...): Link and other unconverted readers look at them on the tick30 step, so at N steps they are cleared
    // on the frame's first step only and keep what any step of the frame set
    if (ship60_first()) {
        offStateFlg(daSFLG_JUMP_e | daSFLG_LAND_e | daSFLG_UNK100_e | daSFLG_JUMP_OK_e |
                    daSFLG_SHOOT_CANNON_e | daSFLG_UNK100000_e | daSFLG_UNK8000000_e |
                    daSFLG_UNK80000000_e);
    }
    if (mLandTimer > 0 && ship60_first()) { /* true60: as mCrashTimer (setYPos sets it after this) */
        mLandTimer--;
    }
    if (checkStateFlg(daSFLG_UNK20_e)) {
        offStateFlg(daSFLG_UNK20_e);
        speed.y = 5.0f;
    }
    /* true60 (the split-actor state rule (3): d_a_ship_t60.h ship60_markAnm): a morf set on an earlier step
     * of this frame (not its tick30 step) waits for the next frame's first play; the frame's remaining plays are noted
     * for the procedures' frame-end phases (ship60_anmRest). The set is a request (2026-11-21, form B,
     * d_a_ship_t60.h ship60_playAnm): the morf holds the requested words for every read, the old animation plays on
     * for the pose until the tick30 step's play, after which the requested words stay. dt 1: the original two plays. */
    ship60_noteRest(this, ship60_execRest());
    ship60_playAnm(s_ship60BodyReq, mpBodyAnm.get(), ship_exec_counter);
    ship60_playAnm(s_ship60HeadReq, mpHeadAnm.get(), ship_exec_counter);
    ship60_snapAnm(s_ship60BodyReq, mpBodyAnm.get(), ship_exec_counter); /* true60: the old words a set here replaces */
    ship60_snapAnm(s_ship60HeadReq, mpHeadAnm.get(), ship_exec_counter);

    if (mCurMode != 12 && mCurMode != 15 && mCurMode != MODE_START_MODE_WARP_e &&
        mCurMode != MODE_START_MODE_THROW_e && mCurMode != 7 && mCurMode != MODE_CRANE_UP_e &&
        mCurMode != 8) {
        /* true60 (the 30 Hz visual-motion rule (2), the rest of the init's frame holds; pairs under first-step frame-level in full): a procedure set
         * inside the body on an earlier step of this frame (procTalk's talk end: changeDemoEndProc) holds the frame's
         * later steps; their talk-command / sea-edge decisions here would see the talk command the event system clears
         * only after the frame (A decides these on the next frame): B re-entered the talk on the end frame's tick30 step
         * and ran it 60 frames longer. Held: no event decision on those steps. dt 1: never held. */
        const ShipExecProcedure* heldProc = gabi::at<ShipExecProcedure>(gabi::ea(this));
        if (t60::dt() != 1.0f && s_ship60ProcHold.on && !ship60_first() && s_ship60ProcHold.owner == this &&
            heldProc->mProc.f == s_ship60ProcHold.fn) {
        } else if (eventInfo.checkCommandTalk()) { /* eventInfo.checkCommandTalk() */
            if (mNextMessageNo == 0x1682) {
                ship_exec_play()->mMenuCollect = 1; /* dComIfGp_onMenuCollect() */
            }
            if (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) {
                procTalkReady_init();
            } else {
                procTalk_init();
            }
        } else if ((ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) && checkOutRange()) {
            if (checkStateFlg(daSFLG_FLY_e)) {
                /* true60 (the per-step smoothing rule; procTurn's form): the heading's max step is the frame-start turn speed on every
                 * step of the frame, the speed eased after it. dt 1: the original. */
                const s16 turnSpeed = t60::dt() != 1.0f ? (s16)ship60_start(s_ship60FlyTurnSpeed, this, ship_exec_counter,
                                                                            (s16)mTurnAngleSpeed)
                                                        : (s16)mTurnAngleSpeed;
                cLib_addCalcAngleS2(&shape_angle.y, mTurnTargetAngle, 8, turnSpeed);
                cLib_addCalcAngleS2(&mTurnAngleSpeed, 0x1800, 8, 0x200);
                // HD: the boat also stops
                speedF = 0.0f;
                current.angle.y = shape_angle.y;
            } else {
                eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
                fopAcM_orderSpeakEvent(this);
            }
        } else {
            if (checkOutRange()) {
                firstDecrementShipSpeed(0.0f);
            }
            eventInfo_onCondition(this, 1);
        }
    }

    mpGrid = 0;
    mpGrid = gabi::ea(fopAcM_SearchByID(mGridID));

    if (ship_demo_getActor(demoActorID)) {
        procToolDemo_init();
    } else if (mDemoMode == 2 /* DEMO_SALVAGE_e */) {
        procCraneUp_init();
    } else if (mEvtStaffId != -1 && mDemoMode != 9 /* DEMO_TORNADO_S_e */) {
        procZevDemo_init();
    }

    s16 sVar26 = mTillerAngle;
    if (mAnmTransform) {
        ShipExec_updateFrame(&mFrameCtrl);
        gabi::at<ShipExecAnimation>(gabi::ea(mAnmTransform.get()))->mFrame =
            mFrameCtrl.getFrame(); /* mAnmTransform->setFrame() */
        if (!checkStateFlg(daSFLG_UNK8000_e) && mFrameCtrl.checkState(2)) {
            mAnmTransform = nullptr;
        }
    }
    BOOL r24 = mDemoMode == 8 /* DEMO_OPEN_e */ || checkStateFlg(daSFLG_UNK4000000_e);

    /* (this->*mProc)() */
    ShipExecProcedure* procedure = gabi::at<ShipExecProcedure>(gabi::ea(this));
    if (procedure->mProc.i != 0) {
        /* true60 (the 30 Hz visual-motion rule (2), 2026-11-18, talk12 analysis): a procedure set inside the dispatched body (A's
         * dispatcher returns after the init) runs its body first on the NEXT frame's first step, as at 30 Hz; on the
         * rest of the init's frame the new body is held (not run; the execute's other work goes on). A set made before
         * the dispatch (the event demos' inits above) runs on the same step, as in A. dt 1: no hold. */
        if (s_ship60ProcHold.on && (ship60_first() || s_ship60ProcHold.owner != this ||
                                    procedure->mProc.f != s_ship60ProcHold.fn)) {
            s_ship60ProcHold.on = false;
        }
        if (!s_ship60ProcHold.on) {
            const u32 sets = g_ship60ProcSets;
            (this->*procedure->mProc)();
            if (t60::dt() != 1.0f && !ship60_tick30() && g_ship60ProcSets != sets) {
                s_ship60ProcHold.on = true;
                s_ship60ProcHold.owner = this;
                s_ship60ProcHold.fn = procedure->mProc.f;
            }
        }
    }

    if (mDemoMode == 8 || checkStateFlg(daSFLG_UNK4000000_e)) {
        mTillerAngle = -0x2000;
        sVar26 = -0x2000;
    } else if (r24) {
        mTillerAngle = 0;
        sVar26 = 0;
    }

    if (!checkStateFlg(daSFLG_FLY_e)) {
        if (mCurMode != 12 && mCurMode != 15 && mCurMode != 7 && mCurMode != MODE_CRANE_UP_e &&
            mCurMode != MODE_START_MODE_WARP_e && mCurMode != MODE_START_MODE_THROW_e &&
            (u8)ship_exec_play()->mEventRun == 0) {
            if (mTornadoActor) {
                s16 target = (s16)gabi::ftoi(gabi::fmadds(mForceMoveAngle, 10430.378f, 20480.0f));
                s16 sVar16 = shape_angle.y;
                cLib_addCalcAngleS(&shape_angle.y, target, 5, 0x2000, 0x200);
                setControllAngle(getAimControllAngle(sVar16));
                current.angle.y = shape_angle.y;
            } else if (mWhirlActor) {
                s16 sVar16 = shape_angle.y;
                f32 a = mForceMoveAngle;
                s16 sVar5;
                if (mWhirlCircleFlg) {
                    sVar5 = (s16)gabi::ftoi(gabi::fmadds(a, 10430.378f, 20480.0f));
                } else {
                    sVar5 = (s16)gabi::ftoi(gabi::fmadds(a, 10430.378f, 32768.0f));
                }
                cLib_addCalcAngleS(&shape_angle.y, sVar5, 5, 0x2000, 0x200);
                setControllAngle(getAimControllAngle(sVar16));
                current.angle.y = shape_angle.y;
            }
        }
        s16 sVar16 = -mPitchOffset;
        s16 sVar5 = (s16)gabi::ftoi((f32)sVar16 * 0.05f);
        if (sVar5 == 0) {
            if (sVar16 < 0) {
                sVar5 = -1;
            } else if (sVar16 > 0) {
                sVar5 = 1;
            }
        }
        // true60 (the per-step smoothing rule and 6): the pitch spring's per-frame adds take dt of themselves per step (carried); the
        // speed's damping is cLib_addCalcAngleS (central)
        mPitchOffsetSpeed += ship60_share(s_ship60PitchSpeed, this, sVar5);
        mPitchOffset += ship60_share(s_ship60Pitch, this, (s16)mPitchOffsetSpeed);
        cLib_addCalcAngleS(&mPitchOffsetSpeed, 0, 30, 0x1000, 1);
    } else {
        cLib_addCalcAngleS(&mPitchOffset, -0x800, 3, 0x500, 0x300);
    }

    mPrevPos.copy(current.pos);
    ship60_ccMoveStep(this, &mStts.m_cc_move); /* true60 (the first-step push rule): the frame's push on every step */
    if (!checkStateFlg(daSFLG_UNK2000000_e)) {
        cLib_chaseS(&m03B8, 0, 0x80);
    } else {
        cLib_chaseS(&m03B8, 0x4E8, 0x80);
    }

    if (mCurMode != 12 && mCurMode != 15) {
        if (mCurMode != 7 && mCurMode != MODE_CRANE_UP_e) {
            if (((u8)ship_exec_play()->mEventRun != 0 || !ship_isLinkPlayer0()) &&
                mEvtStaffId == -1 && mCurMode != MODE_TALK_e &&
                mCurMode != MODE_START_MODE_THROW_e && !checkStateFlg(daSFLG_UNK100000_e)) {
                if (checkStateFlg(daSFLG_UNK20000000_e)) {
                    // HD: down to 20 (GameCube 10)
                    if (!(speedF < 20.0f)) {
                        firstDecrementShipSpeed(20.0f);
                    }
                } else {
                    speedF = 0.0f;
                }
            }
            if ((u8)ship_exec_play()->mEventRun == 0 && !checkStateFlg(daSFLG_FLY_e) &&
                (mTornadoActor || mWhirlActor)) {
                speedF = prev_speedF;
                f32 fVar4;
                if (mTornadoActor) {
                    fVar4 = gabi::fmadds(40.0f, mForceMoveRate, 30.0f);
                    if (!(ship_playerStatus0() & 0x1000 /* daPyStts0_BOW_AIM_e */) ||
                        !((u32)ship_exec_play()->mCameraAttentionStatus &
                          0x20 /* camera attention status */)) {
                        fVar4 *= 1.2f;
                    }
                } else {
                    fVar4 = gabi::fmadds(30.0f, mForceMoveRate, 10.0f);
                    if ((u8)gabi::at<ShipExecSave>(dSv_base())->mBombNum ==
                            0 /* dComIfGs_getBombNum() */
                        && fopAcM_GetRoomNo(this) == 44) {
                        fVar4 *= 1.2f;
                    }
                }
                firstDecrementShipSpeed(fVar4);
            }
            s16 d = (s16)ship60_share(s_ship60ForceTurn, this, (s16)m03B8); /* true60: a turn per frame */
            current.angle.y = (s16)(current.angle.y + d);
            shape_angle.y += d;

            if (checkStateFlg(daSFLG_FLY_e)) {
                if (checkStateFlg(daSFLG_UNK2000000_e)) {
                    cLib_chaseF(&speed.y, 20.0f, 1.0f - gravity);
                    speedF = prev_speedF;
                    // HD: towards 100 (GameCube 55)
                    cLib_chaseF(&speedF, 100.0f, 3.0f);
                    offStateFlg(daSFLG_UNK2000000_e);
                }
                if (speed.y > 0.0f && !(speed.y > -gravity)) {
                    onStateFlg(daSFLG_UNK100_e);
                }
                fopAcM_posMoveF(this, &mStts.m_cc_move);
                mBeltSpeed.copy(*cXyz_Zero);
            } else if ((mTornadoActor || mWhirlActor) && (u8)ship_exec_play()->mEventRun == 0) {
                f32 step;
                if (mTornadoActor) {
                    if ((ship_playerStatus0() & 0x1000) &&
                        ((u32)ship_exec_play()->mCameraAttentionStatus & 0x20)) {
                        step = 5.0f;
                    } else {
                        step = 25.0f;
                    }
                } else if ((u8)gabi::at<ShipExecSave>(dSv_base())->mBombNum == 0 &&
                           fopAcM_GetRoomNo(this) == 44) {
                    step = 10.0f;
                } else {
                    step = 0.6f;
                }
                /* true60 (Ship60Orbit): a later step continues the stored orbit (setXxxActor re-snapped it) */
                const bool orbit60 = t60::dt() != 1.0f && (mTornadoActor || mWhirlCircleFlg);
                const bool orbit60Cont = orbit60 && ship60_orbitContinues(this);
                if (orbit60Cont) {
                    mForceMoveDist = s_ship60Orbit.dist;
                    mForceMoveAngle = s_ship60Orbit.angle;
                }
                cLib_chaseF(&mForceMoveDist, 0.0f, step);
                f32 r = std::fabs((f32)speedF) / mForceMoveDist;
                // true60 (the per-step movement rule): the orbit advances by r (radians per frame) x dt per step; the rate stays per frame
                f32 a = t60::dt() != 1.0f ? mForceMoveAngle + r * t60::dt() : mForceMoveAngle + r;
                mForceMoveAngleSpeed = r;
                mForceMoveAngle = a;
                bool r23 = false;
                if (mTornadoActor) {
                    s16 ang = ShipExec_radToAngle(a);
                    current.pos.x = gabi::fmadds(mForceMoveDist, cM_ssin(ang), mTornadoActor->current.pos.x);
                    s16 ang2 = ShipExec_radToAngle((f32)mForceMoveAngle);
                    current.pos.z =
                        gabi::fmadds(mForceMoveDist, cM_scos(ang2), mTornadoActor->current.pos.z);
                    r23 = true;
                } else if (mWhirlCircleFlg) {
                    s16 ang = ShipExec_radToAngle(a);
                    current.pos.x = gabi::fmadds(mForceMoveDist, cM_ssin(ang), mWhirlActor->current.pos.x);
                    s16 ang2 = ShipExec_radToAngle((f32)mForceMoveAngle);
                    current.pos.z = gabi::fmadds(mForceMoveDist, cM_scos(ang2), mWhirlActor->current.pos.z);
                    r23 = true;
                } else {
                    f32 y = current.pos.y;
                    fopAcM_posMoveF(this, &mStts.m_cc_move);
                    current.pos.y = y;
                }
                if (r23 && orbit60) {
                    /* true60 (Ship60Orbit): the chord from the frame-start position, rescaled to the frame's travel */
                    Ship60Orbit& o = s_ship60Orbit;
                    if (!orbit60Cont) {
                        o.o0x = old.pos.x;
                        o.o0z = old.pos.z;
                        o.ex = old.pos.x;
                        o.ez = old.pos.z;
                        o.travel = 0.0f;
                        o.span = 0.0f;
                    }
                    o.travel += (f32)speedF * t60::dt();
                    o.span += t60::dt();
                    const f32 cx = (f32)current.pos.x - o.o0x;
                    const f32 cz = (f32)current.pos.z - o.o0z;
                    const f32 len = std::sqrt(cx * cx + cz * cz);
                    f32 nx = current.pos.x, nz = current.pos.z;
                    if (len > 1.0f * o.span) {
                        const f32 k = o.travel / len;
                        mStts.m_cc_move.z = 0.0f;
                        mStts.m_cc_move.y = 0.0f;
                        mStts.m_cc_move.x = 0.0f;
                        nx = cx * k + o.o0x;
                        nz = cz * k + o.o0z;
                    }
                    current.pos.x = (f32)old.pos.x + (nx - o.ex);
                    current.pos.z = (f32)old.pos.z + (nz - o.ez);
                    o.ex = nx;
                    o.ez = nz;
                    o.dist = mForceMoveDist;
                    o.angle = mForceMoveAngle;
                    o.stamp = t60_step_counter();
                    o.owner = this;
                    o.valid = true;
                } else if (r23) {
                    gabi::Local<cXyz> sp114;
                    cXyz_mi(&current.pos, sp114.get(), &old.pos);
                    gabi::Local<cXyz> xz;
                    ship_cpf(xz->x, sp114->x);
                    xz->y = 0.0f;
                    ship_cpf(xz->z, sp114->z);
                    f32 displacementLength = cXyz_abs(xz.get());
                    // true60: the step moves speedF x dt (the guard scaled with it)
                    const f32 stepLen = t60::dt() != 1.0f ? 1.0f * t60::dt() : 1.0f;
                    if (displacementLength > stepLen) {
                        f32 k = t60::dt() != 1.0f ? (speedF * t60::dt()) / displacementLength : speedF / displacementLength;
                        mStts.m_cc_move.z = 0.0f;
                        mStts.m_cc_move.y = 0.0f;
                        mStts.m_cc_move.x = 0.0f;
                        current.pos.x = gabi::fmadds(sp114->x, k, old.pos.x);
                        current.pos.z = gabi::fmadds(sp114->z, k, old.pos.z);
                    }
                }
            } else {
                /* true60 (frame form): the frame-start wave pitch (A pitches the frame's move before
                 * setWaveAngle), the step's heading and the frame's held speedF; the frame's move exact (t60c::moveBegin /
                 * moveEnd: the tick30 step lands on the frame start + the frame-end velocity, A's single add) when there is
                 * no collision push. dt 1 / NEGCTRL: the original per step */
                const s16 t60pitch =
                    t60ff ? (s16)ship60_start(s_ship60MovePitch, this, ship_exec_counter, (s16)mWaveAngleX) : (s16)mWaveAngleX;
                speed.x = (speedF * cM_ssin(current.angle.y)) * cM_scos(t60pitch);
                speed.y = -(speedF * cM_ssin(t60pitch));
                speed.z = (speedF * cM_scos(current.angle.y)) * cM_scos(t60pitch);
                const bool t60mv = t60ff && (f32)mStts.m_cc_move.x == 0.0f && (f32)mStts.m_cc_move.y == 0.0f &&
                                   (f32)mStts.m_cc_move.z == 0.0f;
                if (t60mv) {
                    t60c::moveBegin(&current.pos.x, &current.pos.z, 0xA50);
                    t60c::moveBegin(&current.pos.y, &current.pos.y, 0xA51); /* (y on its own key) */
                }
                fopAcM_posMove(this, &mStts.m_cc_move);
                if (t60mv) {
                    t60c::moveEnd(&current.pos.x, &current.pos.z, &speed.x, &speed.z, 0xA50);
                    t60c::moveEnd(&current.pos.y, &current.pos.y, &speed.y, &speed.y, 0xA51);
                    g_ship60YExactStep = t60_step_counter();
                    if (t60::first_step()) { /* mFwdVel's frame value: the tick30 landing's own f32 operations */
                        s_ship60FwdPred.dx = (t60Pos0X + (f32)speed.x) - t60Pos0X;
                        s_ship60FwdPred.dz = (t60Pos0Z + (f32)speed.z) - t60Pos0Z;
                        s_ship60FwdPred.stamp = t60_step_counter();
                        s_ship60FwdPred.valid = true;
                    }
                }
                if ((u8)ship_exec_play()->mEventRun != 0) {
                    mBeltSpeed.copy(*cXyz_Zero);
                } else {
                    gabi::Local<cXyz> sp108;
                    gabi::Local<be<s32>> sp18;
                    gabi::Local<cXyz> nrm;
                    if (mAcch.GetGroundH() != -1000000000.0f &&
                        ShipExec_getPolyRoomPathVec(
                            gabi::ea(static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd)), sp108.get(),
                            sp18.get())) {
                        ShipExec_normalizeZP(sp108.get(), nrm.get());
                        PSVECScale(sp108.get(), sp108.get(), (f32)((s32)*sp18 >> 1));
                        cLib_addCalcPosXZ(&mBeltSpeed, sp108.get(), 0.5f, 5.0f, 1.0f);
                    } else {
                        cLib_addCalcPosXZ(&mBeltSpeed, cXyz_Zero, 0.05f, 0.1f, 0.02f);
                    }
                    if (t60::dt() != 1.0f) { /* true60: the belt speed is a per-frame displacement: x dt per step */
                        gabi::Local<cXyz> belt;
                        PSVECScale(&mBeltSpeed, belt.get(), t60::dt());
                        PSVECAdd(&current.pos, belt.get(), &current.pos);
                    } else {
                        PSVECAdd(&current.pos, &mBeltSpeed, &current.pos);
                    }
                }
            }
        } else {
            mBeltSpeed.copy(*cXyz_Zero);
            if (mEvtStaffId == -1) {
                gabi::Local<cXyz> spFC;
                cXyz_mi(&current.pos, spFC.get(), &old.pos);
                // true60: the step's displacement is dt of a frame's: speedF stays a per-frame speed
                if (checkStateFlg(daSFLG_FLY_e)) {
                    gabi::Local<cXyz> xz;
                    ship_cpf(xz->x, spFC->x);
                    xz->y = 0.0f;
                    ship_cpf(xz->z, spFC->z);
                    speedF = t60::dt() != 1.0f ? cXyz_abs(xz.get()) / t60::dt() : cXyz_abs(xz.get());
                } else {
                    speedF = t60::dt() != 1.0f ? cXyz_abs(spFC.get()) / t60::dt() : cXyz_abs(spFC.get());
                }
            } else {
                speed.x = (speedF * cM_ssin(current.angle.y)) * cM_scos(mWaveAngleX);
                if (mDemoMode != 7 && mDemoMode != 10 && mDemoMode != 11) {
                    speed.y = -(speedF * cM_ssin(mWaveAngleX));
                }
                speed.z = (speedF * cM_scos(current.angle.y)) * cM_scos(mWaveAngleX);
                fopAcM_posMove(this, &mStts.m_cc_move);
            }
        }
    }
    /* mStts.ClrCcMove() */
    mStts.m_cc_move.z = 0.0f;
    mStts.m_cc_move.y = 0.0f;
    mStts.m_cc_move.x = 0.0f;
    gabi::Local<cXyz> previousPosition;
    previousPosition->copy(current.pos);
    mAcchCir[3].SetWall(-600.0f - (f32)previousPosition->y, 250.0f);
    mAcch.CrrPos(dComIfG_Bgsp());
    if (mTornadoActor || mWhirlActor) {
        ship_cpf(current.pos.y, previousPosition->y);
        ship_cpf(current.pos.z, previousPosition->z);
        ship_cpf(current.pos.x, previousPosition->x);
    }
    setRoomInfo();
    if (mCurMode == MODE_TACT_WARP_e) {
        ship_cpf(current.pos.x, previousPosition->x);
        ship_cpf(current.pos.z, previousPosition->z);
    }
    setYPos();

    f32 diff = mWaterY - current.pos.y;
    gabi::at<ShipExecTrack>(gabi::ea(mTrack))->mBaseY = mWaterY; /* mTrack.setWaterY */
    gabi::at<ShipExecTrack>(gabi::ea(mTrack))->mMinY = mWaterFlatY;  /* mTrack.setWaterFlatY */
    gabi::Local<be<s16>> sp12;
    gabi::Local<be<s16>> sp10;
    if (!checkStateFlg(daSFLG_FLY_e)) {
        setWaveAngle(sp12.get(), sp10.get());
    } else if (t60ff) { /* true60 (frame form, process-order read): the frame-start values (A's: the previous frame's) */
        *sp10 = t60OldZ0;
        *sp12 = t60OldX0;
    } else {
        *sp10 = mOldShapeAngleZ;
        *sp12 = mOldShapeAngleX;
    }
    diff += 5.0f;
    /* true60 (frame form): m03D8 (the wake's height over the water) once a frame, on the tick30 step at dt 1
     * from the frame-end water and y (A's); the earlier steps keep it (the water is the previous frame's there) */
    if (t60ff && !t60::tick30()) {
        mOldShapeAngleX = shape_angle.x;
        mOldShapeAngleZ = shape_angle.z;
    } else if (diff > m03D8) {
        mOldShapeAngleX = shape_angle.x;
        m03D8 = diff;
        mOldShapeAngleZ = shape_angle.z;
    } else {
        Ship60Dt1<> t60dt(t60ff);
        cLib_chaseF(&m03D8, diff, 3.0f);
        mOldShapeAngleX = shape_angle.x;
        mOldShapeAngleZ = shape_angle.z;
    }
    if (t60ff) { /* true60 (frame form, process-order read): the shape angles the frame started with (A copies them before its update) */
        mOldShapeAngleX = t60ShapeX0;
        mOldShapeAngleZ = t60ShapeZ0;
    }
    if (mCurMode == MODE_TACT_WARP_e) {
        shape_angle.x = 0;
        shape_angle.z = 0;
    } else {
        shape_angle.x = mWaveAngleX + mPitchOffset;
        shape_angle.z = (s16)(mControllAngle + mWaveAngleZ);
    }
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_now(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel* model1 = mpBodyAnm->getModel();
    J3DModel* model2 = mpHeadAnm->getModel();
    J3DModel_setBaseTRMtx(model1, mDoMtx_now());
    ship60_calcAnm(s_ship60BodyReq, mpBodyAnm.get(), ship_exec_counter); /* true60: a request's pose: the old one */
    setHeadAnm();
    J3DModel_setBaseTRMtx(model2, model_getAnmMtx(model1, FN_BODY_JNT_J_FN_GATTAI_e));
    ship60_calcAnm(s_ship60HeadReq, mpHeadAnm.get(), ship_exec_counter);

    if (mPart == PART_CRANE_e) {
        if (mBodyAnmFileIdx == dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e) {
            f32 rate = getAnglePartRate();
            mCraneAngle = (s16)gabi::ftoi(rate * (f32)(s16)mCraneBaseAngle);
            f32 old039C = (s16)mCraneTurnAngle;
            mCraneTurnAngle = (s16)gabi::ftoi(old039C * getAnglePartRate());
            incRopeCnt(ship60_share(s_ship60RopeCnt, this, -20), 0); /* true60: 20 per frame */
        }
        J3DModel_calc(mpSalvageArmModel);
        setRopePos();
        Mtx34* am = model_getAnmMtx(mpSalvageArmModel, VFNCR_JNT_V_CRANE_ROTATION_e);
        ship_cpf(mCranePos.x, am->m[0][3]);
        ship_cpf(mCranePos.y, am->m[1][3]);
        ship_cpf(mCranePos.z, am->m[2][3]);
        if (isProc(ShipExecTarget::procCrane) || isProc(ShipExecTarget::procCraneUp)) {
            ShipExecRopeActor* rope = gabi::at<ShipExecRopeActor>(gabi::ea(this));
            mCraneTop =
                gabi::at<ShipExecRopePositions>(gabi::ea(rope->mpRopePositions))->mpPosition;
        } else {
            mCraneTop = nullptr;
        }
    } else {
        // HD: ended (GameCube remove())
        dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&mRopeRipple);
        dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&mRipple);
        dPa_followEcallBack_end_l(&mRopeDrip);
        mRopeCnt = 0;
        mCraneTop = nullptr;
        if (mPart == PART_CANNON_e) {
            Mtx34* cm = model_getAnmMtx(mpCannonModel, VFNCN_JNT_CANON2_e);
            ship_cpf(mCannonPos.x, cm->m[0][3]);
            ship_cpf(mCannonPos.y, cm->m[1][3]);
            ship_cpf(mCannonPos.z, cm->m[2][3]);
            if (!isProc(ShipExecTarget::procCannon)) {
                mCannonAngleX = (s16)gabi::ftoi(getAnglePartRate() * 16384.0f);
                f32 old0394 = (s16)mCannonAngleY;
                mCannonAngleY = (s16)gabi::ftoi(old0394 * getAnglePartRate());
            }
            J3DModel_calc(mpCannonModel);
            /* true60 (the one-emitter-per-event rule / 14): the bomb (unconverted, executes on tick30 only) is created on the frame's
             * tick30 step: created on an earlier step it would execute once more in its creation frame and run a whole
             * frame ahead of the 30 Hz world (pair: its flight sound one frame early). The press set mProcVar0 on
             * the frame's first step (first-step input); procCannon's countdown runs on first steps, so 30 holds until here. */
            if (mProcVar0 == 30 && ship60_tick30()) {
                gabi::Local<cXyz> spE4;
                PSMTXMultVec(model_getAnmMtx(mpCannonModel, VFNCN_JNT_CANON2_e), &cannon_top,
                             spE4.get());
                gabi::Local<csXyz> sp1C;
                sp1C->z = shape_angle.z;
                sp1C->x = shape_angle.x + mCannonAngleX - 0x4000; /* getCannonAngleX() */
                sp1C->y = shape_angle.y + mCannonAngleY;          /* getCannonAngleY() */
                u32 prm = ShipExec_makeBombParameter(4, 0, 1);
                fopAc_ac_c* bomb =
                    ShipExec_fastCreate(ShipExecProcess::fpcNm_BOMB_e, prm, spE4.get(),
                                        (s32)tevStr.mRoomNo, sp1C.get(), 0, -1, 0, 0);
                if (bomb) {
                    u32 id = bomb ? (u32)bomb->mBsPcId : 0xFFFFFFFFu;
                    ShipExec_forceCameraLockOn(dCam_getBody(), id);
                    ShipExec_setBombNoGravityTime(bomb, 8);
                    bomb->speedF = cM_scos(sp1C->x) * 110.0f;
                    bomb->speed.y = -(cM_ssin(sp1C->x) * 110.0f);
                    bomb->gravity = -2.5f;
                    seStart(ShipExecSound::JA_SE_LK_SHIP_CANNON_FIRE, &mCannonPos);
                    onStateFlg(daSFLG_SHOOT_CANNON_e);
                    ShipExecPlay* play = ship_exec_play();
                    play->mItemBombNumCount = play->mItemBombNumCount - 1;
                    mpHeadAnm->mFrameCtrl.mRate = 1.0f;
                    mpHeadAnm->mFrameCtrl.mFrame = 0.0f;
                    ship60_markAnm(mpHeadAnm.get(), ship_exec_counter); /* true60 (split-actor state (3)): first played next frame */
                }
                mProcVar0--;
            }
            // HD: the cannon sight
            ship_sightLaunch(this);
            ShipExecSight& sight = ship_exec_sight(this);
            u16 ax = sight.mAngle.x;
            sight.mSpeed = cM_scos(ax) * 110.0f;
            sight.mVelocity.y = -(cM_ssin(ax) * 110.0f);
            sight.mGravity = -2.5f;
            ship_cannonSight(this);
        }
    }

    PSMTXMultVec(model_getAnmMtx(model1, FN_BODY_JNT_J_FN_STEER1_e), &tiller_top_offset,
                 &mTillerTopPos);
    Mtx34* mtx = model_getAnmMtx(model1, FN_BODY_JNT_J_FN_SAIL1_e);
    ship_cpf(mSailPos.x, mtx->m[0][3]);
    ship_cpf(mSailPos.y, mtx->m[1][3]);
    u32 grid = mpGrid;
    ship_cpf(mSailPos.z, mtx->m[2][3]);
    if (grid) {
        /* static cXyz top_offset(0.0f, 0.0f, -365.0f); XZ_top_offset(265.0f, 0.0f, 0.0f) */
        cXyz* top_offset =
            ship_exec_staticVec(top_offset_guard, top_offset_storage, 0.0f, 0.0f, -365.0f);
        cXyz* XZ_top_offset =
            ship_exec_staticVec(XZ_top_offset_guard, XZ_top_offset_storage, 265.0f, 0.0f, 0.0f);
        fopAc_ac_c* g = gabi::at<fopAc_ac_c>(mpGrid);
        g->current.pos.copy(mSailPos);
        g = gabi::at<fopAc_ac_c>(mpGrid);
        g->current.angle.x = shape_angle.x;
        g->current.angle.y = shape_angle.y;
        g->current.angle.z = shape_angle.z;
        gabi::Local<cXyz> spD8;
        PSMTXMultVecSR(mtx, top_offset, spD8.get());
        gabi::at<fopAc_ac_c>(mpGrid)->scale.y = cXyz_abs(spD8.get()) / 365.0f;
        PSMTXMultVecSR(model_getAnmMtx(model1, 8 /* J_FN_SAIL2 */), XZ_top_offset, spD8.get());
        gabi::at<ShipExecGrid>(grid)->furl =
            1.0f - cXyz_abs(spD8.get()) / 265.0f; /* furl */
        if (mTornadoActor) {
            /* mpGrid->force_calc_wind_rel_angle(0x3000) */
            gabi::at<ShipExecGrid>(mpGrid)->force_calc_wind_rel_angle(0x3000);
        }
    }

    s16 dv = mTillerAngle - sVar26;
    s32 angleDifferenceMagnitude = dv < 0 ? -dv : dv;
    s32 angleProduct = dv * mTillerAngle;
    // true60 (the one-emitter-per-event rule): the tiller's change per frame against the thresholds (the step's change / dt), the sound once
    // per 30 Hz frame
    /* (frame form: a step whose tiller chase ran whole changed it by the frame's amount: no / dt) */
    f32 tillerRate = t60::dt() != 1.0f && g_ship60TillerWholeStep != t60_step_counter()
                         ? (f32)angleDifferenceMagnitude / t60::dt() : (f32)angleDifferenceMagnitude;
    if (angleProduct > 0 && tillerRate > 420.00003f) {
        if (ship60_once(s_ship60Rudder, this, ship_exec_counter))
            seStart(ShipExecSound::JA_SE_LK_SHIP_RUDDER_OUT, &mTillerTopPos);
    } else if (angleProduct < 0 && tillerRate > 525.0f) {
        if (ship60_once(s_ship60Rudder, this, ship_exec_counter))
            seStart(ShipExecSound::JA_SE_LK_SHIP_RUDDER_OUT, &mTillerTopPos);
    }

    if (!checkStateFlg(daSFLG_FLY_e)) {
        // HD: full at speed 100 (GameCube 55)
        f32 shipCruiseSpeed = std::fabs((f32)speedF) / 100.0f;
        if (shipCruiseSpeed > 1.0f) {
            shipCruiseSpeed = 1.0f;
        } else {
            shipCruiseSpeed = shipCruiseSpeed >= 0.0f ? shipCruiseSpeed : 0.0f;
        }
        if (ship60_tick30()) { /* true60: the cruise sound's level, once per 30 Hz frame (the audio frame) */
            ShipExec_playCruiseSound(&current.pos, shipCruiseSpeed);
        }
    }

    Mtx34* jnt_mtx = model_getAnmMtx(model2, 0x10 /* J_FN_ME_L */);
    /* true60 (sv4 776): the frame-start eye at N > 1 (d_a_ship_t60.h ship60_eyePos) */
    ship60_eyePos(s_ship60Eye, this, ship_exec_counter, jnt_mtx, &eyePos);

    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    /* true60 (the process-order read rule): the ship runs before Link in the process line; its attention /
     * talk reads of Link's position and eye take his frame-start state at N > 1 (A reads his previous frame end).
     * linkPos / linkEye point at the live fields unless process-order read gives the frame-start copy (d_a_ship_t60.h ship60_fsActor);
     * player 0 (fopAcM_searchActorDistanceXZ2 / fopAcM_seenActorAngleY below) likewise. */
    gabi::Local<cXyz> fsLinkPos, fsLinkEye, fsP0Pos;
    const cXyz* linkPos = ship60_fsActor(link, 1, fsLinkPos.get()) ? fsLinkPos.get() : &link->current.pos;
    const cXyz* linkEye = ship60_fsActor(link, 0, fsLinkEye.get()) ? fsLinkEye.get() : &link->eyePos;
    gabi::Local<cXyz> spCC;
    cXyz_mi(linkPos, spCC.get(), &current.pos);
    f32 distXz = gabi::fmadds(spCC->x, cM_ssin(shape_angle.y), spCC->z * cM_scos(shape_angle.y));
    ship_cpf(attention_info.position.x, eyePos.x);
    attention_info.position.y = eyePos.y + 30.0f;
    ship_cpf(attention_info.position.z, eyePos.z);
    attention_info.flags = 0;

    bool r23_2 = false;
    gabi::Local<cXyz> spC0;
    if (checkStateFlg(daSFLG_UNK1000000_e)) {
        cXyz_mi(&mAtnPos, spC0.get(), &eyePos);
        offStateFlg(daSFLG_UNK1000000_e);
        r23_2 = true;
    } else if (mpEvtAtnPos) {
        cXyz_mi(mpEvtAtnPos, spC0.get(), &eyePos);
        r23_2 = true;
    } else if ((mCurMode == MODE_CRANE_e || mCurMode == MODE_CRANE_UP_e) && mCraneTop &&
               mRopeCnt > 0) {
        cXyz_mi(mCraneTop, spC0.get(), &eyePos);
        r23_2 = true;
    } else if (mCurMode == MODE_TALK_e || distXz > 125.0f) {
        if (!(ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) &&
            (!dComIfGs_isEventBit(0x3910) || dComIfGs_isEventBit(0x2D02)) &&
            (u8)ship_exec_play()->mMiniGameType != 1 /* dComIfGp_getMiniGameType() */) {
            fopAc_ac_c* p0 = dComIfGp_getPlayer(0);
            f32 p0DistXz2;
            if (ship60_fsActor(p0, 1, fsP0Pos.get())) { /* fopAcM_searchActorDistanceXZ2's body on the frame-start pos */
                gabi::Local<cXyz> d, xz;
                cXyz_mi(fsP0Pos.get(), d.get(), &current.pos);
                xz->x = (f32)d->x;
                xz->y = 0.0f;
                xz->z = (f32)d->z;
                p0DistXz2 = PSVECSquareMag(xz.get());
            } else {
                p0DistXz2 = fopAcM_searchActorDistanceXZ2(this, p0);
            }
            if (p0DistXz2 < 250000.0f) {
                cXyz_mi(linkEye, spC0.get(), &eyePos);
                attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;
                r23_2 = true;
            }
        }
    } else {
        attention_info.position.copy(current.pos);
        if (!(ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) && dComIfGs_isEventBit(0x0908) &&
            !checkStateFlg(daSFLG_UNK800000_e)) {
            if (dComIfGs_isEventBit(0x1980) || !dComIfGs_isEventBit(0x0902)) {
                attention_info.flags = fopAc_Attn_ACTION_SHIP_e;
            } else if (dComIfGs_isEventBit(0x0A80)) {
                cXyz* windVec = dKyw_get_wind_vec();
                s32 a = cM_atan2s(windVec->x, windVec->z);
                if ((a < 0 ? -a : a) < 0x1000) {
                    attention_info.flags = fopAc_Attn_ACTION_SHIP_e;
                }
            }
        }
    }

    s16 sVar16, sVar5;
    if (r23_2) {
        f32 sx = spC0->x, sy = spC0->y, sz = spC0->z;
        gabi::Local<cXyz> xz;
        ship_cpf(xz->z, spC0->z);
        ship_cpf(xz->x, spC0->x);
        xz->y = 0.0f;
        sVar16 = cM_atan2s(-sy, cXyz_abs(xz.get())) - shape_angle.x;
        if (sVar16 > 0x2000) {
            sVar16 = 0x2000;
        } else if (sVar16 < -0x3000) {
            sVar16 = -0x3000;
        }
        sVar5 = cM_atan2s(sx, sz) - shape_angle.y;
        if (sVar5 > 0x7800) {
            sVar5 = 0x7800;
        } else if (sVar5 < -0x7800) {
            sVar5 = -0x7800;
        }
        sVar16 = sVar16 / 6;
        sVar5 = sVar5 / 6;
    } else {
        sVar16 = 0;
        sVar5 = 0;
    }
    if (sVar16 == 0 || cLib_distanceAngleS(sVar16, mHeadAngleX) > 0x100) {
        cLib_addCalcAngleS(&mHeadAngleX, sVar16, 10, 0x400, 4);
    }
    if (sVar5 == 0 || cLib_distanceAngleS(sVar5, mHeadAngleY) > 0x100) {
        cLib_addCalcAngleS(&mHeadAngleY, sVar5, 10, 0x400, 4);
    }

    if (mCurMode != MODE_TALK_e && checkForceMessage()) {
        attention_info.flags = (u32)attention_info.flags & ~(u32)fopAc_Attn_ACTION_SHIP_e;
        gabi::Local<cXyz> spB4;
        cXyz_mi(&attention_info.position, spB4.get(), linkPos);
        bool talk;
        if ((u8)ship_exec_play()->mEventRun == 0 &&
            (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e)) {
            talk = true;
        } else {
            talk = false;
            if (std::fabs(current.pos.y - linkPos->y) < 50.0f) {
                gabi::Local<cXyz> xz;
                ship_cpf(xz->x, spB4->x);
                xz->y = 0.0f;
                ship_cpf(xz->z, spB4->z);
                if (PSVECSquareMag(xz.get()) < 62500.0f &&
                    ship60_seenActorAngleY(this, dComIfGp_getPlayer(0)) < 0x6000 &&
                    mNextMessageNo != 0xD65) {
                    talk = true;
                }
            }
        }
        if (talk) {
            fopAcM_orderSpeakEvent(this);
            offStateFlg(daSFLG_UNK400000_e);
            attention_info.flags =
                (u32)attention_info.flags | fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;
            eventInfo_onCondition(this, 1);
        }
    }
    if ((dComIfGs_isEventBit(0x2D10) && !ship_checkMasterSwordEquip()) ||
        (dComIfGs_isEventBit(0x3804 /* HYRULE_COURTYARD_CUTSCENE */) &&
         !dComIfGs_isEventBit(0x2D02)) ||
        (dComIfGs_isEventBit(0x3E10) && !dComIfGs_isEventBit(0x3F80))) {
        attention_info.flags = (u32)attention_info.flags & ~(u32)fopAc_Attn_ACTION_SHIP_e;
    }
    if (mCurMode != MODE_TALK_e && (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e)) {
        u32 no = 0;
        if (dComIfGs_isEventBit(0x1E40) && !dComIfGs_isEventBit(0x3840)) {
            no = 0x168c;
        } else if (dComIfGs_isEventBit(0x2D02) && !dComIfGs_isEventBit(0x3201)) {
            no = 0x1645;
        } else if (dComIfGs_isEventBit(0x1820) && !dComIfGs_isEventBit(0x3380)) {
            no = 0x164d;
        } else if (dComIfGs_getTriforceNum() == 8 && !dComIfGs_isEventBit(0x3D04)) {
            no = 0x1682;
        } else if (dComIfGs_isEventBit(0x3E40) && !dComIfGs_isEventBit(0x3E20)) {
            no = 0x1683;
        }
        if (no) {
            mNextMessageNo = no;
            fopAcM_orderSpeakEvent(this);
            attention_info.flags =
                (u32)attention_info.flags | fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;
            offStateFlg(daSFLG_UNK400000_e);
            eventInfo_onCondition(this, 1);
        }
    }

    // HD: the sail texture scroll follows speed / 100 (GameCube 55)
    f32 p = (0.1f * speedF) / 100.0f;
    mTillerAngleRate = (f32)(s16)mTillerAngle * (1.0f / 0x2000);
    f32 t = mTexScrollX;
    /* true60 (the sailing's frame form): the scroll added per step at dt rounded differently from A's one
     * add a frame (sqPh record 60: 1 ulp). The frame's first call keeps the frame start; the earlier steps show the
     * scroll moved on by p x dt (the picture: only the draw reads it); the tick30 step adds the frame-end p to the
     * frame start, the original f32 add + wrap: A's frame end. dt 1 / SHIP60_FRAME_NEGCTRL: as before. */
    bool t60Step = t60::dt() != 1.0f;
    if (ship60_frameForm()) {
        const f32 t0 = ship60_startF(s_ship60TexScroll, this, ship_exec_counter, t);
        if (t60::tick30()) {
            t = t0;
            t60Step = false;
        }
    }
    if (p > 0.05f) {
        t = t60Step ? t + 0.05f * t60::dt() : t + 0.05f; /* true60: the scroll per frame x dt */
    } else {
        if (p < -0.05f) {
            p = -0.05f;
        }
        t = t60Step ? t + p * t60::dt() : t + p;
    }
    if (t > 1.0f) {
        t = t - 1.0f;
    } else if (t < 0.0f) {
        t = t + 1.0f;
    }
    mTexScrollX = t;

    gabi::Local<cXyz> spA8;
    cXyz_mi(&current.pos, spA8.get(), &old.pos);
    gabi::Local<cXyz> xz3;
    ship_cpf(xz3->x, spA8->x);
    ship_cpf(xz3->z, spA8->z);
    xz3->y = 0.0f;
    /* true60 (frame form): mFwdVel is the frame's forward speed (A: |the frame's displacement|): on the first
     * step from the frame's exact move (made above with the tick30 landing's operations), on the tick30 step from the
     * actual frame displacement (frame start -> frame end, the BG corrections included); the steps between keep it. The
     * wave stop test once a frame with it (prev_fwdvel: the previous frame's). */
    bool t60fwd = true;
    f32 fwdDt = t60::dt();
    if (t60ff) {
        if (t60::first_step() && s_ship60FwdPred.valid && s_ship60FwdPred.stamp == t60_step_counter()) {
            xz3->x = s_ship60FwdPred.dx;
            xz3->z = s_ship60FwdPred.dz;
            fwdDt = 1.0f;
        } else if (t60::tick30()) {
            xz3->x = (f32)current.pos.x - t60Pos0X;
            xz3->z = (f32)current.pos.z - t60Pos0Z;
            fwdDt = 1.0f;
        } else if (!t60::first_step()) {
            t60fwd = false; /* a step between: the frame's value holds */
        } /* else a first step without the exact move: the step's displacement / dt */
    }
    if (t60fwd) {
    // true60: mFwdVel is the forward speed per frame (Link, the sail, the Swift Sail jump check read it): the step's
    // displacement / dt (the frame form: the frame's displacement, fwdDt 1)
    if (!(speedF < 0.0f)) {
        mFwdVel = fwdDt != 1.0f ? cXyz_abs(xz3.get()) / fwdDt : cXyz_abs(xz3.get());
    } else {
        mFwdVel = fwdDt != 1.0f ? -(cXyz_abs(xz3.get()) / fwdDt) : -cXyz_abs(xz3.get());
    }
    f32 v = mFwdVel +
            gabi::fmadds(mBeltSpeed.x, cM_ssin(current.angle.y), mBeltSpeed.z * cM_scos(current.angle.y));
    mFwdVel = v;
    if (!(std::fabs(v) > 0.01f) || prev_fwdvel * v < 0.0f || v * speedF < 0.0f) {
        ship_stopWaves(this);
    }
    } /* true60 */

    setEffectData(mWaterY, *sp10);

    f32 sn = cM_ssin(shape_angle.y);
    f32 cs = cM_scos(shape_angle.y);
    gabi::Local<cXyz> sp9C;
    for (int i = 0; i < 3; i++) {
        f32 off = (f32)cyl_offset.mOffset[i]; /* cyl_offset[i] */
        sp9C->x = gabi::fmadds(off, sn, current.pos.x);
        sp9C->y = current.pos.y - 30.0f;
        sp9C->z = gabi::fmadds(off, cs, current.pos.z);
        mCyl[i].SetC(sp9C.get());
        dComIfG_Ccsp_Set(&mCyl[i]);
        /* SetTgGrp: IsPlayer, or IsPlayer | IsOther */
        u32 grp = (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) ? 4 : 0xC;
        mCyl[i].mObjTg.mSPrm = (mCyl[i].mObjTg.mSPrm & ~0xEu) | grp;
    }
    PSMTXMultVec(model_getAnmMtx(mpHeadAnm->getModel(), FN_HEAD_H_JNT_J_FN_ATAMA_e), sph_offset,
                 sp9C.get());
    {
        u32 grp = (ship_playerStatus0() & daPyStts0_SHIP_RIDE_e) ? 4 : 0xC;
        mSph.mObjTg.mSPrm = (mSph.mObjTg.mSPrm & ~0xEu) | grp;
    }
    mSph.SetC(sp9C.get());
    if (mCurMode != MODE_CANNON_e) {
        dComIfG_Ccsp_Set(&mSph);
    } else {
        ShipExec_resetTargetHit(&mSph);
    }
    u8 part = mPart;
    ShipExecPlay* play = ship_exec_play();
    if (part == PART_STEER_e) {
        play->mPlayerStatus1 = play->mPlayerStatus1 | 0x400; /* daPyStts1_SAIL_e */
        ShipExec_setSailSoundState(1);
    } else {
        play->mPlayerStatus1 = play->mPlayerStatus1 & ~0x400u;
        ShipExec_setSailSoundState(0);
    }
    // HD: the shadow texture
    setShadowBtkRate();
    offStateFlg(daSFLG_UNK40000000_e);
    mNextMode = mCurMode;
    ship60_nextModeLeave(s_ship60NextMode, this, &mNextMode);
    return TRUE;
}

// clang-format off
/* WWHD glue
 str 1003A5CC "Ship" in 02477A24
 data cyl_offset 101D03E4
 data cannon_top 101D03B4
 data sight_scroll_phase 101D0348
 data ship_exec_counter 101FF560
 data XZ_top_offset_guard 1046DDE4
 data XZ_top_offset_storage 1046DD94
 data top_offset_guard 1046DDE0
 data top_offset_storage 1046DD88
 data sph_offset_guard 1046DDDC
 data sph_offset_storage 1046DD7C
 data sail_offset_guard 1046DDD8
 data sail_offset_storage 1046DD70
 ext 0251641C void ShipExec_clearCollisionHit(dCcD_Sph*)
 ext 0200E2F4 void ShipExec_removeCollision(void*, dCcD_Sph*)
 ext 02544830 u32 ShipExec_getMyNowCutName(void*, s32)
 ext 020079B4 f64 ShipExec_getStickValue(u32)
 ext 02007A1C s32 ShipExec_getStickAngle(u32)
 ext 027F2FC4 void ShipExec_updateFrame(void*)
 ext 02019510 s16 ShipExec_radToAngle(f32)
 ext 025AB184 BOOL ShipExec_getPolyRoomPathVec(u32, cXyz*, be<s32>*)
 ext 0201B3C0 void ShipExec_normalizeZP(cXyz*, cXyz*)
 ext 020CB8D8 u32 ShipExec_makeBombParameter(u32, u32, u32)
 ext 025D5928 fopAc_ac_c* ShipExec_fastCreate(u32, u32, cXyz*, s32, csXyz*, u32, s32, u32, u32)
 ext 02515470 void ShipExec_forceCameraLockOn(u32, u32)
 ext 020CB89C void ShipExec_setBombNoGravityTime(fopAc_ac_c*, s32)
 ext 025E1CC4 void ShipExec_playCruiseSound(cXyz*, f32)
 ext 025D68A0 s32 ShipExec_seenActorAngleY(fopAc_ac_c*, fopAc_ac_c*)
 ext 02516264 void ShipExec_resetTargetHit(dCcD_Sph*)
 ext 025E1CD4 void ShipExec_setSailSoundState(u32)
*/

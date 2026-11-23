/**
 * d_camera_follow.cpp (WWHD)
 * Follow camera dCamera_c: followCamera, the default third-person engine.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp, a "Nonmatching" draft there) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera_follow.h"
#include "d/d_camera_key_t60.h"
#include "true60.h"
#include "d/d_camera_run_t60.h"
#include "d/d_camera_follow_t60.h"
#include "d/d_camera_engine_t60.h"
namespace camera_follow {
static MatrixPointer followMatrix;
static GuestByte cb1FlyingFlag;
static GuestByte mdFlyingFlag;
static GuestByte hyoiKamomeFlag;
static GuestShort seagullDebug;
static GuestFloat seagullYaw;
static GuestFloat seagullFovy;

/* work area of followCamera (mWork, GC Work::Follow) */

static inline bool cb1Flying() {
    return cb1FlyingFlag != 0;
}
static inline bool mdFlying() {
    return mdFlyingFlag != 0;
}
static inline bool hyoiKamome() {
    return hyoiKamomeFlag != 0;
}
static inline f32 bezierRatio(f32 x, f32 r) {
    return cameraFollowBezierRatio(x, r);
} /* dCamMath::rationalBezierRatio */
static inline s16 angLoad(cSAngle_l* a) {
    return *a;
}

/* bit-exact float copy (lfs/stfs pass-through) */
static inline void cpf(void* dst, const void* src) {
    *reinterpret_cast<GuestWord*>(dst) = *reinterpret_cast<const GuestWord*>(src);
}
static inline f32 angDegree(const cSAngle_l* a) {
    return cameraFollowAngleDegree(a);
}
static inline s16 angCt(cSAngle_l* tmp, s16 v) {
    return *cameraFollowAngleShortCtor(tmp, v);
}

/* stack dBgS_CamLinChk (same as d_camera_bg.cpp camLinChk_ct / dt) */
static void camLinChk_ct_follow(dBgS_LinChk* o) {
    cameraFollowLineCtor(o);
    o->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(o));
    o->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(o));
    o->cBgS_Chk::__vtbl = CAM_LINE;
    o->cBgS_PolyInfo::__vtbl = CAM_POLY_INFO;
    o->dBgS_PolyPassChk::__vtbl = CAM_POLY_PASS;
    o->dBgS_GrpPassChk::__vtbl = CAM_GROUP_PASS;
    o->mbObjThrough = 0;
    o->mbCamThrough = 1;
    o->mbLinkThrough = 0;
    o->mbArrowThrough = 0;
    o->mbBombThrough = 0;
    o->mbBoomerangThrough = 0;
    o->mbRopeThrough = 0;
    o->mGrp = 3;
}
static void camLinChk_dt_follow(dBgS_LinChk* o) {
    o->dBgS_PolyPassChk::__vtbl = BASE_POLY_PASS;
    o->dBgS_GrpPassChk::__vtbl = BASE_GROUP_PASS;
    o->cBgS_PolyInfo::__vtbl = BASE_POLY_INFO;
    cameraFollowLineDtor(o, 0);
}
/* line check from start to mViewCache.mCenter; on a hit the centre is put on the wall, pushed out
 * along the wall normal (HD: only when the plane exists) */
/* inlined into dCamera_c::followCamera */
static void followWallCheck(dCamera_c* c, cXyz* start) {
    dBgS_LinChk lc;
    dBgS_LinChk* o = &lc;
    camLinChk_ct_follow(o);
    if (c->lineBGCheck(start, &c->mViewCache.mCenter, (u8*)&lc, 0x7F)) {
        dBgS* bgs = dComIfG_Bgsp();
        cM3dGPla* plane =
            cameraFollowGetTriPla(bgs, (u32)o->mBgIndex, (u32)o->mPolyIndex); /* GetTriPla */
        c->mViewCache.mCenter.copy(o->mLin.mEnd);                             /* GetCross() */
        if (plane != 0)
            PSVECAdd(&c->mViewCache.mCenter, &plane->mNormal,
                     &c->mViewCache.mCenter); /* += mNormal */
    }
    camLinChk_dt_follow(o);
}

/* turn ratio from the stick angle (no turn-behind sequence running) */
static inline f32 followStickTurn(dCamera_c* c, cSAngle_l* rel) {
    if (c->mStickMainValueLast < 0.1f)
        return 0.05f;
    f32 s = cameraFollowAngleSin(rel); /* Sin */
    return bezierRatio(__builtin_fabsf(s), 12.0f);
}

/* true60 (the per-step smoothing rule and 4): the follow camera at N steps per 30 Hz frame. Each per-frame smoothing
 * ratio r becomes 1 - (1 - r)^dt for the step (fr; a jump r >= 1 and dt = 1 stay as they are: bit-exact at N = 1),
 * a per-frame decay d becomes d^dt (fd), additive per-frame amounts take dt of themselves; the follow work's counters
 * (m388 airborne frames, m392 charge frames, m38C turn frames) and the transition's weight sum (m380 -= m384, the FLLW
 * sum the retired recompiled True 60 double-stepped into a NaN crash) count 30 Hz frames: they step on tick30 only
 * (frameEnd), and the steps of a frame read the frame's values. */
static inline f32 fr(f32 r) { return t60::dt() == 1.0f ? r : t60_ratio(r, t60::dt()); }
static inline f32 fd(f32 d) { return t60::dt() == 1.0f ? d : (f32)__builtin_pow((double)d, (double)t60::dt()); }
static inline bool frameEnd() { return t60::dt() == 1.0f || t60::tick30(); }
/* The direction's ratios are nested in the eye chase: per frame the direction (yaw U, latitude V, radius R) moves a
 * share a toward its target, the eye then chases the resulting point by 0.75 and the direction is read back from the
 * eye (cSGlobe_Val), so the direction itself moves 0.75 x a per frame. Converting both ratios separately gives
 * fr(0.75) x fr(a) per step, and N such steps do not compose to 0.75 x a (N = 2, a = 0.08: 0.077 instead of 0.06 per
 * frame, the camera's latitude ~0.7x as fast as the original). frDir(a) is the direction's per-step ratio for which
 * fr(0.75) x frDir(a) = fr(0.75 x a): N steps compose to the original frame's product. The eye's own chase keeps fr(0.75)
 * (it also follows the centre's moves). dt = 1: a itself (bit-exact); a = 1 stays 1. */
static inline f32 frDir(f32 a) {
    if (t60::dt() == 1.0f)
        return a;
    f32 eye = fr(0.75f);
    f32 prod = 0.75f * a;
    if (!(a > 0.0f) || !(prod < 1.0f) || !(eye > 0.0f))
        return a;
    return fr(prod) / eye;
}
/* The yaw's remainder (crawl pair). The yaw block chases from g484.mU,
 * the eye's yaw read back by cM_atan2s, a table lookup with a truncated index (steps of 5-10 units; the error q = read -
 * exact yaw is one-signed within each 45 deg octant: -4.3 mean in the pair's crawl), and the chased share of the gap is an
 * s16 truncated toward zero. The chased point therefore stands e = q + dU - f (t + q) away from the exact proportional
 * point Y + f (T - Y) (Y the eye's exact yaw, t = T - g484.mU, dU the truncated share), and the eye takes 0.75 of that
 * offset: per 30 Hz frame once, from the frame's own values (the eye at the frame's start against the frame's centre).
 * Taken per step it is fr(0.75) of each step's own e (N x fr(0.75) > 0.75: 1.0 at N 2): the view turned a quarter of the
 * read-back error more per frame, one-signed: B's yaw ran 2 units / frame behind A's through the crawl's turn
 * (-22 -> -173 over frames 80-155) and Link, whose heading is camera-relative, reached the hole 5 frames late.
 * The N-step form: each step takes its exact proportional chase (its own remainder taken back: the eye moved by -c of
 * the step's remainder displacement, c = fr(0.75)), and the frame's end step adds the original frame's remainder,
 * computed with the original routines from the eye the frame's first step saw (s_followFrameEye) against the frame's
 * centre: 0.75 of its displacement. With equal states this is A's remainder exactly, so the quantisation is the
 * original's (a model of the yaw loop: A-B 0.0 with a moving target; 24-38 mean / 160 max without, 9-10 / 160 with a
 * dt share per step). When the frame's first step did not run the yaw block (the engine started inside the frame, or
 * a step of the frame missed it) the frame falls back to the dt share per step: (0.75 dt - c) of the step's remainder.
 * followYawRemainder returns e in s16 units (d = eye - centre as the read-back saw it). dt = 1: not used. */
static inline f32 followYawRemainder(const cXyz* d, s16 u0, s16 t, s16 dU, f32 f) {
    const double x = d->x, z = d->z;
    const double h = __builtin_sqrt(x * x + z * z);
    if (!(h > 0.0))
        return 0.0f;
    const double u = (double)u0 * (3.14159265358979323846 / 32768.0);
    double s = (__builtin_sin(u) * z - __builtin_cos(u) * x) / h; /* sin(read - exact) */
    s = s > 1.0 ? 1.0 : (s < -1.0 ? -1.0 : s);
    const double q = __builtin_asin(s) * (32768.0 / 3.14159265358979323846);
    return (f32)(q + (double)dU - (double)f * ((double)t + q));
}
/* the eye at the current frame's start (host memory, one camera): stored by the yaw block on the frame's first step,
 * valid while the yaw block runs on every step of the frame */
struct FollowFrameEye {
    f32 x, y, z;
    uint32_t lastStep; /* the step counter of the yaw block's last run */
    bool lastValid;
    bool valid; /* x, y, z are the eye the yaw block saw on the current frame's first step */
};
static camera_key_t60::PerCam<FollowFrameEye> s_followFrameEye;
static camera_key_t60::PerCam<CamChaseTarget> s_followCentreTarget; /* the speed-ramp position-gap rule (2): the centre chase target (d_camera_engine_t60.h) */
/* The latitude's remainder (crawl pair): the latitude block
 * chases from the STORED latitude, the eye's latitude read back by cSGlobe_Val (cM_atan2s, a table with a truncated
 * index) at the end of the previous update, and its chased share is an s16 truncated toward zero. As for the yaw
 * (followYawRemainder), the chased point stands e = q + dV - f (t + q) off the exact proportional point (q = stored -
 * exact latitude of the eye-centre vector it was read from, t = target - stored, dV the truncated share, f the share's
 * ratio) and the eye takes 0.75 of that offset once per 30 Hz frame; per step fr(0.75) of each step's own (1.0 per frame
 * at N 2). One-signed: with no latitude target (m3E0 = 0, the corner-hit state 0x80000) the view's latitude drifts down
 * 4/3 as fast (cy4 frames 182-204: A 8.7 / frame, B 12.6), B's eye ends 4 units lower and bumpCheck's corner -> wall
 * switch (the crawl hole, radius chase toward 145) comes 2 frames late. Same exact form as the yaw: each step takes
 * its own back (-c), the frame's end step adds the original frame's remainder (0.75): q read on the previous frame's
 * end step and the share terms of the frame's first step with the frame's ratio. Fallback (a step of the frame missed
 * the block): (0.75 dt - c) of the step's own. Not applied when the target is the eye's own latitude (vFromG: the
 * keep correction below covers it) or the 80 deg limit cut the step. Host memory, one camera; dt = 1: not used. */
struct FollowLatRem {
    uint32_t readStep; /* the step counter of the last read-back (end of the true60 block) */
    bool readValid;
    s16 readV;         /* the latitude read back there */
    f32 readQ;         /* its remainder q (s16 units) */
    uint32_t lastStep; /* the step counter of the latitude block's last run */
    bool lastValid;
    bool frameValid; /* every step of the current frame so far ran the remainder */
    f32 q1, t1;      /* the frame's first step: q and the target gap t (s16 units) */
};
static camera_key_t60::PerCam<FollowLatRem> s_followLatRem;
/* q of a read-back latitude v of the vector d (s16 units) */
static inline f32 followLatQ(const cXyz* d, s16 v) {
    const double x = d->x, y = d->y, z = d->z;
    const double h = __builtin_sqrt(x * x + z * z);
    if (!(h > 0.0))
        return 0.0f;
    double q = (double)v - __builtin_atan2(y, h) * (32768.0 / 3.14159265358979323846);
    if (q > 32768.0)
        q -= 65536.0;
    if (q < -32768.0)
        q += 65536.0;
    return (f32)q;
}
/* the latitude remainder's point displacement: px minus px turned back in latitude by a (s16 units) */
static inline void followLatDisp(double a, const cXyz* px, double* mx, double* my, double* mz) {
    const double x = px->x, y = px->y, z = px->z;
    const double h = __builtin_sqrt(x * x + z * z);
    const double R = __builtin_sqrt(x * x + y * y + z * z);
    if (!(h > 0.0) || !(R > 0.0)) {
        *mx = *my = *mz = 0.0;
        return;
    }
    const double v = __builtin_atan2(y, h) - a * (3.14159265358979323846 / 32768.0);
    const double h2 = R * __builtin_cos(v);
    *mx = x - x * h2 / h;
    *my = y - R * __builtin_sin(v);
    *mz = z - z * h2 / h;
}
/* turn the horizontal part of v by a s16 units (yaw += a) into out */
static inline void followTurnXZ(double a, double x, double z, double* ox, double* oz) {
    const double r = a * (3.14159265358979323846 / 32768.0);
    const double c = __builtin_cos(r), s = __builtin_sin(r);
    *ox = x * c + z * s;
    *oz = z * c - x * s;
}
/* the mode-start-once rule: the mode's start-up block (`mCurModeTimer == 0`, the timer counts 30 Hz frames in dCamera_c::Run) runs
 * once per mode start, on the first step that sees the timer at 0, not on every step of the mode's first frame. A first
 * frame continues while the calls come on consecutive steps that do not end a frame; a new 0 after a frame end, a
 * gap (another engine ran) or a non-zero timer is a new start. Host memory, this unit, one camera; dt = 1: timer == 0. */
static camera_key_t60::PerCam<uint32_t> s_followZeroStamp;
static camera_key_t60::PerCam<bool> s_followInFirstFrame;
static inline bool followModeStart(u32 timer) {
    if (t60::dt() == 1.0f)
        return timer == 0;
    if (timer != 0) {
        s_followInFirstFrame() = false;
        return false;
    }
    uint32_t now = t60_step_counter();
    bool start = !(s_followInFirstFrame() && s_followZeroStamp() + 1 == now);
    s_followInFirstFrame() = !t60::tick30();
    s_followZeroStamp() = now;
    return start;
}

/* The flying branch's forward-tilt decay (d_camera_run_t60.h FwdFrame). The original multiplies
 * mForwardTiltOffset by 0.33 on every call, once per 30 Hz frame (after Run's FwdCushion chase); at N steps the decay
 * ran 0.33^N per frame. Per-frame form: the frame's first step takes the original frame's chased tilt Run recorded on
 * that step (or the tilt as it stands when Run did not chase: the zeroed or untouched value, as in the original), scales
 * it by 0.33 with the original routine and keeps the result; the frame's later steps hold it (Run's per-step chase in
 * between is replaced), so each read and the frame's end see the original frame's value. A frame whose first step did
 * not reach this branch (the mode started inside the frame, another engine ran) decays by 0.33^dt per step. dt = 1: the
 * original. Host memory, one camera. */
struct FollowTilt {
    uint32_t step; /* the step counter of the branch's last run */
    s16 val;       /* the frame's tilt after the decay */
    bool valid;    /* val is the current frame's (set on its first step, the branch ran on every step since) */
};
static camera_key_t60::PerCam<FollowTilt> s_followTilt;
static inline void followTiltDecay(cSAngle_l* tilt) {
    if (t60::dt() == 1.0f) {
        cameraFollowAngleScaleAssign(tilt, 0.33f);
        return;
    }
    FollowTilt& ft = s_followTilt();
    const uint32_t now = t60_step_counter();
    if (t60::first_step()) {
        s16 v;
        if (camera_run_t60::fwdFrameNow(&v))
            *tilt = v;
        cameraFollowAngleScaleAssign(tilt, 0.33f);
        ft.val = (s16)*tilt;
        ft.valid = true;
    } else if (ft.valid && ft.step + 1 == now) {
        *tilt = ft.val;
    } else {
        ft.valid = false;
        cameraFollowAngleScaleAssign(tilt, fd(0.33f));
    }
    ft.step = now;
}

} // namespace camera_follow
using namespace camera_follow;
/* 025028B8 */
/* HD: the first-frame transition uses the frame weight for the radius, latitude, yaw
 * and fovy (GameCube: the constant follow ratio) and max(height, 10) as divisor; the free-turn
 * decay is Val20 * 0.86; the wall push-out checks the plane; flying with the seagull adds a
 * debug-register controlled yaw of the centre around the eye and a fovy limit */
bool dCamera_c::followCamera(s32 style) {
    Follow* p = (Follow*)mWork;
    cSAngle_l chargeLat;
    cSAngle_l lim80;
    cSAngle_l lo;
    cSAngle_l hi;
    cSAngle_l peep;
    cSAngle_l tmp;
    cSAngle_l tmp2;
    f32 f31 = 0.9f;
    cameraFollowAngleFloatCtor(&chargeLat,
                               (f32)setup(this)->mChargeLatitude); /* mCamSetup.ChargeLatitude() */
    f32 chargeBRatio = setup(this)->mChargeBRatio;
    s32 chargeTimer = setup(this)->mChargeTimer;
    cameraFollowAngleFloatCtor(&lim80, 80.0f);
    f32 v15 = camParamVal(style, 15);
    f32 v3 = camParamVal(style, 3);
    f32 v0 = camParamVal(style, 0);
    f32 v13 = camParamVal(style, 13);
    f32 v1 = camParamVal(style, 1);
    f32 v10 = camParamVal(style, 10);
    f32 v4 = camParamVal(style, 4);
    f32 v11 = camParamVal(style, 11);
    f32 v14 = camParamVal(style, 14);
    f32 v5 = camParamVal(style, 5);
    cameraFollowAngleFloatCtor(&lo, camParamVal(style, 16));
    cameraFollowAngleFloatCtor(&hi, camParamVal(style, 17));
    f32 v19 = camParamVal(style, 19);
    f32 v29 = camParamVal(style, 29);
    f32 v25 = camParamVal(style, 25);
    f32 v23 = camParamVal(style, 23);
    f32 v18 = camParamVal(style, 18);
    f32 v24 = camParamVal(style, 24);
    f32 v20 = camParamVal(style, 20) * 0.86f; /* HD */
    (void)dComIfGp_getAttention();            /* dComIfGp_getAttention() (unused) */
    bool bVar2 = false;
    const bool modeStart = followModeStart(mCurModeTimer); /* true60: the mode-start-once rule */
    if (modeStart) {
        p->m3AC = 0.0f;
        p->m3B0 = 0.0f;
        p->m3D9 = 0;
    }
    if (cb1Flying() || mdFlying()) {
        f31 = 0.66f;
        if (v15 < 35.0f)
            v15 = 35.0f;
        followTiltDecay(&mForwardTiltOffset); /* mForwardTiltOffset *= 0.33f (true60: once per 30 Hz frame) */
        if (v10 < 420.0f)
            v10 = 420.0f;
        if (v11 < 350.0f)
            v11 = 350.0f;
        cameraFollowAngleValFloat(&hi, 80.0f);
        if (cb1Flying() && mIsKazeMap != 0) {
            if (mInKazeCenter != 0) {
                cSAngle_l a;
                cSAngle_l* ap = cameraFollowAngleFloatCtor(&a, 30.0f);
                if ((s16)lo < (s16)*ap)
                    cameraFollowAngleValFloat(&lo, 30.0f);
                if (v15 < 50.0f)
                    v15 = 50.0f;
                v11 = 600.0f;
                v10 = 800.0f;
            } else {
                cSAngle_l a;
                cSAngle_l* ap = cameraFollowAngleFloatCtor(&a, 10.0f);
                if ((s16)lo < (s16)*ap)
                    cameraFollowAngleValFloat(&lo, 10.0f);
            }
        }
    }
    if (dComIfGp_getPlayerStatus0(mPadId) &
        (daPyStts0_SWIM_e | daPyStts0_CRAWL_e)) { /* CRAWL | SWIM */
        cSAngle_l a;
        cSAngle_l* ap = cameraFollowAngleFloatCtor(&a, 4.0f);
        if ((s16)lo < (s16)*ap)
            cameraFollowAngleValFloat(&lo, 4.0f);
        if (v5 < -10.0f)
            v5 = -10.0f;
    }
    if ((dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_HANG_e | daPyStts0_UNK200_e)) &&
        !(dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_UNK2000000_e))) {
        if (v0 > -10.0f)
            p->m3B0 = -10.0f;
    } else {
        f32 b0 = p->m3B0;
        p->m3B0 = gabi::fmadds(v0 - b0, fr(0.06f), b0);
    }
    if (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK40000_e)) {
        v10 = 420.0f;
        v15 = -24.0f;
        v5 = 140.0f;
        bVar2 = true;
        v25 = 80.0f;
        v11 = v10;
        mForwardTiltOffset = angLoad(&angle0());
    }
    cSAngle_ct(&peep, &angle0());
    if (dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_UNK20_e | daPyStts0_UNK40_e)) {
        cSAngle_l pa;
        calcPeepAngle(&pa);
        peep = (s16)pa;
        if (dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_UNK20_e))
            v1 = -v1;
    }
    u16 sflags = styleEntry(style)->mFlags;
    if (sflags & dCamPrmFlg_UNK200)
        bVar2 = true;
    if (((mEventFlags & 0x100000) && (dComIfGp_getPlayerStatus0(mPadId) &
                                      (daPyStts0_BOW_AIM_e | daPyStts0_HOOKSHOT_AIM_e |
                                       daPyStts0_ROPE_AIM_e | daPyStts0_BOOMERANG_AIM_e))) ||
        cb1Flying() || mdFlying()) {
        f32 tgt;
        if (mStickMainPosXLast < -0.2f)
            p->m3D9 = 1;
        if (mStickMainPosXLast > 0.2f) {
            p->m3D9 = 0;
            tgt = 45.0f;
        } else if (p->m3D9 != 0) {
            tgt = -45.0f;
        } else {
            tgt = 45.0f;
        }
        f32 ac = p->m3AC;
        p->m3AC = gabi::fmadds(tgt - ac, fr(0.04f), ac);
    } else {
        f32 ac = p->m3AC;
        p->m3AC = gabi::fmadds(v1 - ac, fr(0.06f), ac);
    }
    cXyz ofs;
    cpf(&ofs.z, &p->m3B0);
    cpf(&ofs.x, &p->m3AC);
    ofs.y = v5;
    /* (dt = 1: the timer read again here, in the original order: a callee may have changed it since) */
    if (t60::dt() == 1.0f ? mCurModeTimer == 0 : modeStart) {
        cXyz d;
        cXyz pos;
        cXyz_mi(&mViewCache.mEye, &d, &mViewCache.mCenter);
        cSGlobe_Val(&mViewCache.mDirection, &d);
        p->m394 = f31;
        p->m390 = 0;
        p->m392 = 0;
        p->m388 = 0x50;
        p->m398 = v11;
        p->m38C = 0;
        p->m378 = FOLLOW_WORK; /* 'FLLW' */
        p->m39C = v10;
        f32 deg = angDegree(&mDirection.mV);
        p->m3BC = deg;
        p->m3A0 = deg;
        p->m3C0.copy(mViewCache.mCenter);
        p->m3CC.copy(mViewCache.mEye);
        p->m3E4 = 0.01f;
        p->m3E0 = 0.01f;
        p->m3E8 = 0.01f;
        p->m3DC = 0.75f;
        p->m3EC = v3;
        p->m3F0 = v4;
        p->m3D8 = 1;
        p->m3B4 = 0;
        cpf(&p->m3A8, &mViewCache.mFovy);
        p->m3DA = 0;
        p->m3B8 = 0.0f;
        positionOf(&pos, mpPlayerActor.get());
        cpf(&p->m3A4, &pos.y);
        if ((mEventFlags & 0x8000) || mModeChangeTransition == 0) {
            mStyleSettle2 = 1;
            mStyleSettle1 = 1;
            mStyleSettle0 = 1;
            p->m37C = 1;
            cpf(&p->m39C, &mDirection.mRadius);
            cpf(&p->m398, &mDirection.mRadius);
            cpf(&p->m3A8, &mFovy);
            p->m380 = 1.0f;
            p->m384 = 0.0f;
        } else {
            cXyz c;
            cXyz x;
            cXyz e;
            cSAngle_l u;
            cSAngle_l dir;
            cSAngle_l v;
            cSGlobe_l g;
            relationalPos(&c, mpPlayerActor.get(), &ofs);
            cSAngle_ct(&u);
            if (mEventFlags & 0x100000) {
                directionOf(&dir, mpPlayerActor.get());
                cSAngle_Val(&u, cSAngle_Inv(&dir));
            } else {
                cameraFollowAngleVal(&u, &mViewCache.mDirection.mU); /* Val(const cSAngle&) */
            }
            cSAngle_l* vp = cameraFollowAngleFloatCtor(&v, v15);
            cameraFollowGlobeCtorAngles(&g, v10, vp, &u);
            cameraFollowGlobeXyz(&g, &x);
            cXyz_pl(&c, &e, &x);
            cXyz_mi(&mEye, &d, &e);
            f32 l1 = std_sqrtf(PSVECSquareMag(&d));
            cXyz_mi(&mCenter, &d, &c);
            f32 l2 = std_sqrtf(PSVECSquareMag(&d)) * 4.0f;
            f32 m = (l2 - l1 >= 0.0f) ? l2 : l1; /* fsel */
            m = __builtin_fabsf(m);
            f32 h = heightOf(mpPlayerActor.get());
            f32 hh = (10.0f - h >= 0.0f) ? 10.0f : h; /* fsel */
            f32 r = std_sqrtf(m / hh);
            p->m37C = gabi::ftoi(3.8f * r) + 1;
            s32 n = p->m37C;
            cpf(&p->m39C, &mDirection.mRadius);
            cpf(&p->m398, &mDirection.mRadius);
            cpf(&p->m3A8, &mFovy);
            p->m380 = (f32)((n * (n + 1)) >> 1);
            p->m384 = 0.0f;
        }
    }
    cXyz tgt;
    cSAngle_l pdir;
    cSAngle_l rel;
    relationalPos(&tgt, mpPlayerActor.get(), &ofs);
    directionOf(&pdir, mpPlayerActor.get());
    cameraFollowAngleSubtract(&pdir, &rel, &mViewCache.mDirection.mU); /* local_4ac */
    tgt.y = getWaterSurfaceHeight(&tgt);
    if (mStyleSettle0 == 0) {
        cXyz d;
        cXyz s;
        cXyz x;
        cXyz e;
        if (mFollowMoveBg != 0)
            cameraFollowMoveBgPosition(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mBG.mCameraGround.mGndChk), 1,
                                       &p->m3C0, 0, 0); /* MoveBgMatrixCrrPos */
        /* true60, the whole-frame form rule (whole form for this block, irol pair): the style settle
         * (m3C0 chase, centre chase, wall check, direction / fovy chase by the frame's weight and the eye placed from
         * them) runs once per 30 Hz frame, whole, on the tick30 step with the original frame's ratios; the frame's
         * first step holds the centre, eye, direction and fovy. Per step the nested centre / eye chases did not compose
         * to the frame's (the yaw read-back quantum split, A 3561 / 3580). The block's inputs (Link's position and
         * direction, the mode timer that steps after the engine on tick30) are the frame-end values A reads. The moving-BG
         * correction above stays per step (it follows the carrier's per-step move). dt = 1: the original. */
        if (t60::dt() != 1.0f && !t60::tick30())
            return true;
        f32 r = (f32)(s32)((s32)p->m37C - (s32)mCurModeTimer);
        f32 k = r / p->m380; /* true60: the original frame's weight (the block runs once per frame, see above) */
        p->m384 = r;
        cXyz_mi(&tgt, &d, &p->m3C0);
        cXyz_ml(&d, &s, k);
        PSVECAdd(&p->m3C0, &s, &p->m3C0);
        cXyz_mi(&p->m3C0, &s, &mViewCache.mCenter);
        cXyz_ml(&s, &d, v3);
        PSVECAdd(&mViewCache.mCenter, &d, &mViewCache.mCenter);
        f32 dist =
            cameraFollowHorizontalDistance(&tgt, &p->m3C0); /* dCamMath::xyzHorizontalDistance */
        f32 ox = ofs.x, oz = ofs.z;
        f32 om = (oz - ox >= 0.0f) ? oz : ox; /* fsel */
        if (dist < __builtin_fabsf(om) + 20.0f) {
            cXyz a;
            attentionPos(&a, mpPlayerActor.get());
            a.y = a.y - 15.0f;
            followWallCheck(this, &a);
        }
        f32 R = cameraFollowLimit((f32)mViewCache.mDirection.mRadius, v11, v10); /* limitf */
        cSAngle_l vv;
        cSAngle_l ay;
        cSAngle_l t42;
        cSAngle_l t40;
        cSAngle_l t30;
        cSAngle_l t48;
        cSAngle_l t46;
        cSAngle_l t44;
        cSGlobe_l g;
        cSAngle_ct(&vv);
        s16 V = mViewCache.mDirection.mV;
        if (V < (s16)lo)
            V = lo;
        if (V > (s16)hi)
            V = hi;
        vv = V;
        s16 inv = cSAngle_Inv(&mAngleY);
        cSAngle_l* ayp = cameraFollowAngleShortCtor(&ay, inv);
        cameraFollowGlobeCtorAngles(&g, R, &vv, ayp);
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(g.mRadius - rr, k, rr);
        cameraFollowAngleSubtract(&g.mV, &t42, &mViewCache.mDirection.mV);
        cameraFollowAngleScale(&t42, &t40, k);
        cameraFollowAngleAdd(&mViewCache.mDirection.mV, &t30, &t40);
        mViewCache.mDirection.mV = angCt(&tmp, t30);
        if (mEventFlags & 0x100000) {
            s16 pi = cSAngle_Inv(&pdir);
            cameraFollowShortSubtractAngle(&t48, pi, &mViewCache.mDirection.mU);
            cameraFollowAngleScale(&t48, &t46, k);
            cameraFollowAngleAdd(&mViewCache.mDirection.mU, &t44, &t46);
            mViewCache.mDirection.mU = angCt(&tmp, t44);
        }
        cameraFollowGlobeXyz(&mViewCache.mDirection, &x);
        cXyz_pl(&mViewCache.mCenter, &e, &x);
        mViewCache.mEye.copy(e);
        p->m3CC.copy(mViewCache.mEye);
        if (frameEnd() && (u32)mCurModeTimer >= (u32)((s32)p->m37C - 1)) { /* true60: at the frame's end */
            mStyleSettle2 = 1;
            mStyleSettle1 = 1;
            mStyleSettle0 = 1;
        }
        p->m3A0 = angDegree(&mViewCache.mDirection.mV);
        cpf(&p->m39C, &mViewCache.mDirection.mRadius);
        cpf(&p->m398, &mViewCache.mDirection.mRadius);
        f32 fv = mViewCache.mFovy;
        mViewCache.mFovy = gabi::fmadds(v25 - fv, k, fv);
        if (frameEnd()) /* true60: the weight sum counts 30 Hz frames (FLLW, lessons #4) */
            p->m380 = p->m380 - p->m384;
        return true;
    }

    cXyz pp;
    cXyz mul;
    cXyz d;
    cXyz s;
    positionOf(&pp, mpPlayerActor.get());
    pp.y = pp.y + 10.0f;
    f32 foot = footHeightOf(mpPlayerActor.get());
    f32 height = foot - groundHeight(&pp);
    if ((mPlayerOnFloor != 0 || (dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_SWIM_e)) || hyoiKamome()) &&
        !(dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_UNK200_e))) {
        p->m388 = 0;
        p->m394 = 0.0f;
    } else if (p->m388 < 0x50) {
        s32 n = p->m388 + 1;
        if (frameEnd()) /* true60: counts 30 Hz frames; the frame's steps use its count n */
            p->m388 = n;
        f32 old = p->m394;
        f32 b = bezierRatio((f32)n / 80.0f, 1.25f);
        p->m394 = gabi::fmadds(f31 - old, fr(b), old);
    }
    if (((dComIfGp_getPlayerStatus0(mPadId) &
          (daPyStts0_UNK1_e | daPyStts0_UNK20_e | daPyStts0_UNK40_e | daPyStts0_UNK800000_e |
           daPyStts0_UNK2000000_e | daPyStts0_UNK4000000_e)) ||
         (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK10000_e))) &&
        mDMCSystem.field_0x0 == 0)
        setDMCAngle();
    if (((dComIfGp_getPlayerStatus0(mPadId) &
          (daPyStts0_UNK1_e | daPyStts0_UNK20_e | daPyStts0_UNK40_e | daPyStts0_HANG_e |
           daPyStts0_UNK2000000_e)) ||
         (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK10000_e))) &&
        ((s16)rel <= angLoad(&angle270()) || (s16)rel >= angLoad(&angle90()))) {
        p->m3EC = 0.1f;
    } else {
        p->m3EC = v3;
    }
    if (p->m388 == 0) {
        p->m3F0 = v4;
        cpf(&mul.x, &p->m3EC);
        mul.y = v4;
        cpf(&mul.z, &p->m3EC);
    } else {
        f32 a = v4 * 0.1f;
        p->m3F0 = gabi::fmadds(1.0f - a, p->m394, a);
        if (((mEventFlags & 0x100000) && p->m3EC > 0.25f) || cb1Flying() || mdFlying() ||
            (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK40000_e))) {
            p->m3EC = gabi::fmadds(p->m394, 0.75f, 0.25f);
        }
        cpf(&mul.x, &p->m3EC);
        cpf(&mul.y, &p->m3F0);
        cpf(&mul.z, &p->m3EC);
    }
    bool bVar4 = true;
    bool bVar3 = false;
    if ((mEventFlags & 0x80) && mDirection.mRadius < v11)
        bVar3 = true;
    if ((mEventFlags & 0x100000) ||
        (dComIfGp_getPlayerStatus0(mPadId) &
         (daPyStts0_UNK1_e | daPyStts0_UNK20_e | daPyStts0_UNK40_e | daPyStts0_HANG_e |
          daPyStts0_ROPE_AIM_e | daPyStts0_UNK800000_e | daPyStts0_UNK2000000_e |
          daPyStts0_UNK4000000_e)) ||
        (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_DEKU_LEAF_FAN_e | daPyStts1_UNK10000_e)) ||
        p->m388 != 0)
        bVar4 = false;
    if (mCurMode == 1) {
        cXyz a;
        attentionPos(&a, mpPlayerActor.get());
        if (tgt.y < a.y + 50.0f) {
            cSAngle_l dd;
            cSGlobe_l g;
            cXyz a2;
            cXyz x;
            cXyz e;
            directionOf(&dd, mpPlayerActor.get());
            s16 inv = cSAngle_Inv(&dd);
            cameraFollowGlobeCtor(&g, v11, (s16)0, inv); /* cSGlobe(f32, s16, s16) */
            attentionPos(&a2, mpPlayerActor.get());
            cameraFollowGlobeXyz(&g, &x);
            cXyz_pl(&a2, &e, &x);
            if (cameraFollowLineBlocked(this, &a2, &e, 0x7F))
                tgt.y = a2.y + 50.0f;
        }
    }
    cXyz_mi(&tgt, &d, &mViewCache.mCenter);
    if (t60::dt() != 1.0f) { /* true60: the centre's per-axis ratios per step */
        /* true60, the speed-ramp position-gap rule (2) (camChaseLead, 2026-11-19 11:00; irol pair, first split A frame 3558):
         * with the free-turn factor 0 the yaw is the eye's own angle about the moved centre (g484.mU), an integer
         * direction; the centre's lag behind a moving Link decides when it crosses a read-back quantum (A's eye lags the
         * centre one frame's chase; B's per-step chase lagged less: the yaw 9 units later). The centre chases with the
         * exact frame form for a moving target: the target moved ahead by lead x the step's own move of it, per axis
         * with the frame's ratio (the hookshot centre's form, d_camera_engine.cpp) */
        cXyz dm;
        camChaseTargetMove(s_followCentreTarget(), tgt, &dm);
        d.x += camChaseLead(mul.x) * dm.x;
        d.y += camChaseLead(mul.y) * dm.y;
        d.z += camChaseLead(mul.z) * dm.z;
        mul.x = fr(mul.x);
        mul.y = fr(mul.y);
        mul.z = fr(mul.z);
    }
    cameraFollowVectorMultiply(&d, &s, &mul); /* cXyz * cXyz */
    PSVECAdd(&mViewCache.mCenter, &s, &mViewCache.mCenter);
    if (mIsSeaMap == 0) {
        cXyz a;
        attentionPos(&a, mpPlayerActor.get());
        if (dComIfGp_getPlayerStatus0(mPadId) &
            (daPyStts0_HANG_e | daPyStts0_SWIM_e | daPyStts0_CRAWL_e)) {
            cXyz ep;
            eyePos(&ep, mpPlayerActor.get());
            a.y = ep.y + 30.0f;
        } else {
            a.y = a.y - 15.0f;
        }
        followWallCheck(this, &a);
    }
    cSGlobe_l g484;
    {
        cXyz d2;
        cXyz_mi(&mViewCache.mEye, &d2, &mViewCache.mCenter);
        cameraFollowGlobeFromVector(&g484, &d2);
    }

    s32 c392 = (s16)p->m392;
    /* true60: how the free-turn factor v20 below applies to m3B8 at this step. The stick branch writes m3B8 afresh on
     * every call (a value of the frame's stick, not a recurrence): it takes v20 itself on every step. The charge branch's
     * recurrence runs on the 30 Hz grid (written on the frame's end, v20 then; held on the other steps). Only a value
     * carried from step to step (the flying / seagull / no-write branches) decays by v20^dt per step. dt = 1: v20. */
    int b8Form = 0; /* 0 carried (v20^dt), 1 written this step (v20), 2 held for the frame (unchanged) */
    if (c392 > 0 && c392 <= chargeTimer) {
        f32 b = bezierRatio((f32)c392 / (f32)chargeTimer, chargeBRatio);
        f32 old = p->m3B8;
        p->m3D8 = 1;
        b8Form = frameEnd() ? 1 : 2;
        if (frameEnd()) /* true60: a per-frame recurrence of the charge counter: once per 30 Hz frame */
            p->m3B8 = (1.0f - old) * b;
    } else if ((mEventFlags & 0x100000) || cb1Flying()) {
        if (p->m3D8 != 0)
            p->m3B8 = 0.05f;
        f32 o = p->m3B8;
        p->m3D8 = 0;
        p->m3B8 = gabi::fmadds(1.0f - o, fr(0.2f), o);
    } else if (mdFlying() || hyoiKamome()) {
        p->m3D8 = 1;
    } else if (dComIfGp_getPlayerStatus1(mPadId) &
               (daPyStts1_DEKU_LEAF_FAN_e | daPyStts1_UNK40000_e)) {
        if (p->m3D8 != 0)
            p->m3B8 = 0.05f;
        f32 o = p->m3B8;
        p->m3D8 = 0;
        p->m3B8 = gabi::fmadds(0.5f - o, fr(0.05f), o);
    } else {
        p->m3D8 = 1;
        b8Form = 1; /* true60: written afresh below */
        f32 v;
        if (mDMCSystem.field_0x0 != 0) {
            p->m3B8 = 0.0f;
            v = 0.0f * mStickMainValueLast;
            p->m3B8 = v;
        } else {
            cSAngle_l a;
            if (!(mStickMainPosYLast < 0.0f)) {
                f32 b = bezierRatio(mStickMainPosXLast, v23);
                cSAngle_l* ap = cameraFollowAngleFloatCtor(&a, 180.0f * b);
                f32 c = cameraFollowAngleCos(ap); /* Cos */
                v = 1.0f - gabi::fmadds(c, 0.5f, 0.5f);
            } else {
                f32 b = bezierRatio(mStickMainPosXLast, v24);
                cSAngle_l* ap = cameraFollowAngleFloatCtor(&a, 180.0f * b);
                f32 c = cameraFollowAngleCos(ap);
                v = 1.0f - gabi::fmadds(c, 0.25f, 0.75f);
            }
            p->m3B8 = v;
            v = v * mStickMainValueLast;
            p->m3B8 = v;
        }
        if (mEventFlags & 0x80000)
            p->m3B8 = v * 0.5f;
        else
            p->m3B8 = v * 0.1f;
        if ((dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_HANG_e | daPyStts0_UNK2000000_e)) ||
            (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK10000_e))) {
            s32 c = p->m38C;
            if (camera_follow_t60::turnStartHeld()) {
                /* true60: the counter started on an earlier step of this frame: the start frame's rest (the input rule) */
            } else if (c < 0) {
                c = c - 1;
                if (frameEnd()) /* true60: counts 30 Hz frames */
                    p->m38C = (c <= -0x20) ? 0x10 : c;
            } else if (c == 0) {
                if ((s16)rel > angLoad(&angle270()) && (s16)rel < angLoad(&angle90()))
                    p->m38C = 1;
                else
                    p->m38C = -1;
                camera_follow_t60::turnStartMark();
            } else if (c < 0xF) {
                p->m3B8 = (f32)c * 0.033333335f;
                if (frameEnd()) /* true60: counts 30 Hz frames */
                    p->m38C = p->m38C + 1;
            } else if (!(dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_UNK2000000_e))) {
                p->m3B8 = followStickTurn(this, &rel);
            }
        } else if (bVar2) {
            s32 c = p->m38C;
            if (camera_follow_t60::turnStartHeld()) {
                /* true60: the counter started on an earlier step of this frame: the start frame's rest (the input rule) */
            } else if (c == 0 && ((s16)rel <= angLoad(&angle270()) || (s16)rel >= angLoad(&angle90()))) {
                p->m38C = 1;
                camera_follow_t60::turnStartMark();
            } else if (c < 0xF) {
                p->m3B8 = (f32)c * 0.033333335f;
                if (frameEnd()) /* true60: counts 30 Hz frames */
                    p->m38C = p->m38C + 1;
            } else if (dComIfGp_getPlayerStatus0(mPadId) &
                       (daPyStts0_UNK20_e | daPyStts0_UNK40_e)) {
                p->m3B8 = 0.15f;
            } else {
                p->m3B8 = followStickTurn(this, &rel);
            }
        } else {
            /* true60: a carried value decays by v20^dt per step; a value written afresh this step takes v20 (b8Form) */
            if (b8Form != 2)
                p->m3B8 = p->m3B8 * (b8Form == 1 ? v20 : fd(v20));
            p->m38C = 0;
        }
    }

    f32 yawRem = 0.0f;     /* true60: the step's yaw remainder in s16 units (followYawRemainder) */
    f32 yawRemFrame = 0.0f; /* true60: the original frame's remainder, on the frame's end step (exact form) */
    bool yawExact = false;  /* true60: the frame-start eye is valid: each step takes its remainder back */
    f32 latRem = 0.0f;      /* true60: the step's latitude remainder in s16 units (FollowLatRem) */
    f32 latRemFrame = 0.0f; /* true60: the original frame's latitude remainder, on the frame's end step */
    bool latExact = false;  /* true60: every step of the frame ran the latitude remainder: exact form */
    {
        cSAngle_l a4b4;
        cSAngle_l z;
        cSAngle_l t28;
        cSAngle_l t26;
        cSAngle_l t24;
        cSAngle_l t34;
        cSAngle_ct(&a4b4);
        if ((mEventFlags & 0x80) && !(mEventFlags & 0x80000) && mCurMode == 0 && p->m38C == 0) {
            a4b4 = angCt(&tmp, mDirection.mU);
        } else if (bVar2) {
            a4b4 = (s16)pdir;
            if (dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_UNK20_e | daPyStts0_UNK40_e))
                cameraFollowAngleAddAssign(&a4b4, &peep); /* += */
        } else {
            s16 inv = cSAngle_Inv(&pdir);
            a4b4 = angCt(&tmp, inv);
        }
        cSAngle_ct(&z, (s16)0);
        cameraFollowAngleSubtract(&a4b4, &t28, &g484.mU);
        f32 c = cameraFollowAngleCos(&g484.mV); /* Cos */
        f32 f = frDir(p->m3B8 * c); /* true60: the yaw's per-frame ratio per step, nested in the eye chase */
        cameraFollowAngleScale(&t28, &t26, f);
        cameraFollowAngleAdd(&g484.mU, &t24, &t26);
        cameraFollowAngleAdd(&t24, &t34, &z);
        mViewCache.mDirection.mU = angCt(&tmp, t34);
        if (t60::dt() != 1.0f) { /* true60: the yaw's remainders (followYawRemainder, applied after the eye chase) */
            cXyz d2;
            cXyz_mi(&mViewCache.mEye, &d2, &mViewCache.mCenter);
            yawRem = followYawRemainder(&d2, (s16)g484.mU, (s16)t28, (s16)t26, f);
            FollowFrameEye& fe = s_followFrameEye();
            const uint32_t now = t60_step_counter();
            if (t60::first_step()) { /* the frame's first step: its eye is the original frame's (g484's) eye */
                fe.x = mViewCache.mEye.x;
                fe.y = mViewCache.mEye.y;
                fe.z = mViewCache.mEye.z;
                fe.valid = true;
            } else if (!(fe.lastValid && fe.lastStep + 1 == now)) { /* a step of the frame without the yaw block */
                fe.valid = false;
            }
            fe.lastStep = now;
            fe.lastValid = true;
            yawExact = fe.valid;
            if (yawExact && frameEnd()) { /* the original frame's remainder: its eye (the frame start) and centre */
                cXyz dA;
                cSGlobe_l gA;
                cSAngle_l tA;
                cSAngle_l dUA;
                dA.x = fe.x - (f32)mViewCache.mCenter.x;
                dA.y = fe.y - (f32)mViewCache.mCenter.y;
                dA.z = fe.z - (f32)mViewCache.mCenter.z;
                cameraFollowGlobeFromVector(&gA, &dA);
                cameraFollowAngleSubtract(&a4b4, &tA, &gA.mU);
                const f32 fA = p->m3B8 * cameraFollowAngleCos(&gA.mV);
                cameraFollowAngleScale(&tA, &dUA, fA);
                yawRemFrame = followYawRemainder(&dA, (s16)gA.mU, (s16)tA, (s16)dUA, fA);
            }
        }
    }

    cSAngle_l v4bc;
    cSAngle_ct(&v4bc);
    bool vFromG = false; /* true60: the eye's keep of the centre's move below */
    f32 vRatio = 0.0f;
    const s16 v0Stored = mViewCache.mDirection.mV;
    f32 r0Stored = 0.0f;
    bool rFromG = false;
    {
        cXyz pos;
        bool charging = (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK20000_e)) != 0;
        s32 n392 = (s16)p->m392;
        if (charging) {
            v4bc = (s16)chargeLat;
            if (n392 <= chargeTimer) {
                f32 b = bezierRatio((f32)(s32)(s16)p->m392 / (f32)chargeTimer, chargeBRatio);
                p->m3E0 = b;
                mEventFlags = mEventFlags | 0x4000000;
                if (frameEnd()) /* true60: counts 30 Hz frames */
                    p->m392 = (s16)(p->m392 + 1);
            } else {
                p->m3E0 = 1.0f;
            }
            f32 deg = angDegree(&v4bc);
            p->m3A0 = deg;
            p->m3BC = deg;
        } else {
            if (n392 != 0)
                p->m3E0 = 0.0f;
            p->m392 = 0;
            if (mCurMode == 1) {
                f32 a0 = p->m3A0;
                f32 nn = gabi::fmadds(v15 - a0, fr(p->m3B8), a0);
                p->m3E0 = 0.5f;
                p->m3A0 = nn;
                v4bc = angCt(&tmp, (s16)gabi::ftoi(nn * 182.04445f)); /* cAngle::d2s */
                p->m3BC = nn;
                f32 e = p->m3E0;
                p->m3E0 = gabi::fmadds(f31 - e, 0.5f, e);
            } else if (p->m388 == 0) {
                f32 bc = p->m3BC;
                bc = gabi::fmadds(v15 - bc, fr(v14), bc);
                f32 w = 0.01f;
                p->m3BC = bc;
                if (p->m3B4 != 0) {
                    w = 0.25f;
                    v19 = 0.25f;
                }
                f32 deg = angDegree(&mForwardTiltOffset);
                f32 a0 = p->m3A0;
                f32 nn = gabi::fmadds((bc + deg) - a0, fr(v19), a0);
                p->m3A0 = nn;
                v4bc = angCt(&tmp, (s16)gabi::ftoi(nn * 182.04445f));
                if (mEventFlags & 0x80000) {
                    p->m3E0 = 0.0f;
                } else if (p->m3B4 == 0 && mMonitor.field_0x0C.x < 0.01f && mCurMode == 0) {
                    f32 e = p->m3E0;
                    p->m3E0 = gabi::fmadds(0.1f - e, fr(0.05f), e);
                } else {
                    f32 e = p->m3E0;
                    p->m3E0 = gabi::fmadds(f31 - e, fr(w), e);
                }
            } else if ((dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_UNK2000000_e)) ||
                       (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK10000_e))) {
                v4bc = (s16)g484.mV;
                vFromG = true; /* true60: the latitude target is the eye's own direction */
                f32 deg = angDegree(&v4bc);
                p->m3A0 = deg;
                p->m3BC = deg;
                p->m3E0 = 0.95f;
            } else if ((dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_DEKU_LEAF_FLY_e)) ||
                       cb1Flying() || mdFlying() || hyoiKamome()) {
                cXyz pos2;
                v4bc = (s16)g484.mV;
                vFromG = true; /* true60: the latitude target is the eye's own direction */
                positionOf(&pos2, mpPlayerActor.get());
                if (p->m3A4 < pos2.y) {
                    p->m3E0 = bezierRatio(p->m394, v18);
                } else {
                    f32 e = p->m3E0;
                    p->m3E0 = gabi::fmadds(0.75f - e, fr(0.15f), e);
                }
                f32 deg = angDegree(&v4bc);
                p->m3A0 = deg;
                p->m3BC = deg;
            } else {
                v4bc = (s16)g484.mV;
                vFromG = true; /* true60: the latitude target is the eye's own direction */
                f32 deg = angDegree(&v4bc);
                p->m3A0 = deg;
                p->m3BC = deg;
                p->m3E0 = bezierRatio(p->m394, v18);
            }
        }
        positionOf(&pos, mpPlayerActor.get());
        cpf(&p->m3A4, &pos.y);
    }
    if ((dComIfGp_getPlayerStatus0(mPadId) &
         (daPyStts0_UNK1_e | daPyStts0_UNK20_e | daPyStts0_UNK40_e | daPyStts0_HANG_e |
          daPyStts0_UNK2000000_e)) ||
        (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK10000_e)))
        p->m3B4 = 1;
    else
        p->m3B4 = 0;
    if ((s16)v4bc < (s16)lo)
        cameraFollowAngleVal(&v4bc, &lo);
    else if ((s16)v4bc > (s16)hi)
        cameraFollowAngleVal(&v4bc, &hi);
    {
        cSAngle_l t24;
        cSAngle_l t26;
        cSAngle_l t28;
        cameraFollowAngleSubtract(&v4bc, &t24, &mViewCache.mDirection.mV);
        vFromG = vFromG && (s16)v4bc == (s16)g484.mV; /* (not when the limits moved it) */
        vRatio = p->m3E0;
        cameraFollowAngleScale(&t24, &t26, frDir((f32)p->m3E0)); /* true60: per step, nested in the eye chase */
        cameraFollowAngleAdd(&mViewCache.mDirection.mV, &t28, &t26);
        s16 nv = angCt(&tmp, t28);
        mViewCache.mDirection.mV = nv;
        if (nv > (s16)lim80)
            mViewCache.mDirection.mV = angCt(&tmp2, lim80);
        if (t60::dt() != 1.0f) { /* true60: the latitude's remainders (FollowLatRem, applied after the eye chase) */
            FollowLatRem& lr = s_followLatRem();
            const uint32_t now = t60_step_counter();
            const bool qOk = lr.readValid && lr.readStep + 1 == now && lr.readV == v0Stored;
            const bool on = qOk && !vFromG && !(nv > (s16)lim80);
            if (on) {
                const f32 q = lr.readQ;
                const f32 f = frDir((f32)p->m3E0);
                latRem = q + (f32)(s16)t26 - f * ((f32)(s16)t24 + q);
            }
            if (t60::first_step()) {
                lr.frameValid = on;
                lr.q1 = on ? lr.readQ : 0.0f;
                lr.t1 = (f32)(s16)t24;
            } else if (!(on && lr.frameValid && lr.lastValid && lr.lastStep + 1 == now)) {
                lr.frameValid = false;
            }
            lr.lastStep = now;
            lr.lastValid = on;
            latExact = on && lr.frameValid;
            if (latExact && frameEnd()) { /* the original frame's remainder: q of its stored latitude, its share */
                const f32 F = (f32)p->m3E0;
                cSAngle_l tA;
                cSAngle_l sA;
                tA = (s16)gabi::ftoi(lr.t1);
                cameraFollowAngleScale(&tA, &sA, F);
                latRemFrame = lr.q1 + (f32)(s16)sA - F * (lr.t1 + lr.q1);
            }
        }
    }
    {
        cXyz x;
        cXyz e;
        r0Stored = mViewCache.mDirection.mRadius;
        f32 a = p->m398, b = p->m39C;
        f32 n398 = gabi::fmadds(v11 - a, fr(v14), a);
        f32 n39C = gabi::fmadds(v10 - b, fr(v14), b);
        f32 r3 = g484.mRadius;
        p->m398 = n398;
        p->m39C = n39C;
        if (r3 < n398) {
            f32 dc = p->m3DC;
            dc = gabi::fmadds(v13 - dc, fr(0.01f), dc);
            p->m3DC = dc;
            f32 lim = p->m398;
            f32 R = mViewCache.mDirection.mRadius;
            mViewCache.mDirection.mRadius = gabi::fmadds(lim - R, frDir(dc), R); /* true60: nested in the eye chase */
        } else if (r3 > n39C) {
            f32 dc = p->m3DC;
            dc = gabi::fmadds(v13 - dc, fr(0.01f), dc);
            p->m3DC = dc;
            f32 lim = p->m39C;
            f32 R = mViewCache.mDirection.mRadius;
            mViewCache.mDirection.mRadius = gabi::fmadds(lim - R, frDir(dc), R); /* true60: nested in the eye chase */
        } else {
            p->m3DC = 1.0f;
            f32 R = mViewCache.mDirection.mRadius;
            mViewCache.mDirection.mRadius = gabi::fmadds(r3 - R, 1.0f, R);
            rFromG = true; /* true60: the radius is the eye's own distance */
        }
        cameraFollowGlobeXyz(&mViewCache.mDirection, &x);
        cXyz_pl(&mViewCache.mCenter, &e, &x);
        p->m3CC.copy(e);
    }
    if (bVar3 && bVar4 && (sflags & dCamPrmFlg_UNK001)) {
        cSGlobe_l gg;
        cSAngle_l a;
        cXyz x;
        cXyz e;
        cameraFollowGlobeCopyCtor(&gg, &mViewCache.mDirection); /* cSGlobe(const cSGlobe&) */
        cSAngle_l* av = cameraFollowAngleFloatCtor(&a, v15);
        gg.mV = angCt(&tmp, *av);
        cameraFollowGlobeXyz(&gg, &x);
        cXyz_pl(&mViewCache.mCenter, &e, &x);
        if (cameraFollowLineBlocked(this, &mViewCache.mCenter, &e, 0x7F))
            mEventFlags = mEventFlags | 8;
    }
    cXyz_mi(&p->m3CC, &d, &mViewCache.mEye);
    cXyz_ml(&d, &s, fr(0.75f)); /* true60: per step */
    PSVECAdd(&mViewCache.mEye, &s, &mViewCache.mEye);
    cXyz_mi(&mViewCache.mEye, &s, &mViewCache.mCenter);
    cSGlobe_Val(&mViewCache.mDirection, &s);
    if (t60::dt() != 1.0f) {
        /* true60 (2026-11-17, frames 216-240): the eye's keep of the centre's move.
         * The latitude and radius blocks above start from the STORED direction (read back from the eye last update),
         * so the part of the centre's move the eye does not follow (g484: the eye seen from the moved centre) stays in
         * the view for good: with a target of its own (v4bc from m3A0 / the charge or mode-1 latitude; the radius
         * limits) the original keeps 1 - 0.75 = 0.25 of a frame's move (219-226 measured 0.233 of the gap the
         * move opens) and fr(0.75) per step keeps 1 - fr(0.75) of each step's (N 2: 0.5, measured 0.48): twice the
         * original per frame (the view's latitude rose 2.3x as fast: B 6507 -> 8892 vs A 6079 -> 7049 over 216-226;
         * the eye 35-60 units higher). With the eye's own direction as the target (v4bc = g484.mV, the airborne
         * branches; the radius set to r3) the original keeps 1 - 0.75 (1 - a) and the steps 1 - fr(0.75) +
         * fr(0.75 a), close to it. Each step's read-back latitude / radius gives back its excess keep (0.75 -
         * fr(0.75) - (0.75 a - fr(0.75 a)) of the step's move, 0 for the radius set to r3) and the eye is put on the
         * corrected direction. The yaw is built from g484.mU (the eye's own yaw, the same form as A): unchanged.
         * The eye is MOVED by the correction (the difference of the corrected and the read-back direction's points),
         * not rebuilt from the direction: the read-back angles are table-quantised (cM_atan2s, about 10 units near
         * 0), and an eye rebuilt from them each step loses its sub-quantum offset, so a slowly opening yaw never
         * grows (214-239: B's eye yaw stuck at -112 on every step while A opened -122 ->
         * -1676 as the centre ran at the eye). The direction is then read back from the moved eye, as A keeps it.
         * dt = 1: not run (bit-exact). */
        const f32 c = fr(0.75f);
        f32 kV = 0.75f - c;
        if (vFromG && vRatio > 0.0f && vRatio * 0.75f < 1.0f)
            kV = kV - (0.75f * vRatio - fr(0.75f * vRatio));
        const f32 kR = rFromG ? 0.0f : 0.75f - c;
        const s16 dV = (s16)((s16)g484.mV - v0Stored);
        cSGlobe_l g0;
        cameraFollowGlobeCopyCtor(&g0, &mViewCache.mDirection); /* the read-back direction */
        const s16 nV = (s16)((s16)mViewCache.mDirection.mV - (s16)gabi::ftoi(kV * (f32)dV));
        mViewCache.mDirection.mV = nV;
        mViewCache.mDirection.mRadius = mViewCache.mDirection.mRadius - kR * ((f32)g484.mRadius - r0Stored);
        cXyz x0;
        cXyz x2;
        cXyz dx;
        cameraFollowGlobeXyz(&g0, &x0);
        cameraFollowGlobeXyz(&mViewCache.mDirection, &x2);
        cXyz_mi(&x2, &dx, &x0);
        PSVECAdd(&mViewCache.mEye, &dx, &mViewCache.mEye);
        /* the yaw's remainders (followYawRemainder): the chase moved the eye c of the chased point's remainder
         * displacement (the point m3CC against the same point turned back by the remainder). Exact form: the step's
         * own taken back (-c), the original frame's added on the frame's end step (0.75); fallback: the step keeps
         * the frame's 0.75 x dt of its own */
        if (yawRem != 0.0f || yawRemFrame != 0.0f) {
            cXyz px;
            cXyz_mi(&p->m3CC, &px, &mViewCache.mCenter);
            double mx = 0.0, mz = 0.0, x0, z0;
            followTurnXZ(-(double)yawRem, (double)px.x, (double)px.z, &x0, &z0);
            const double kU = yawExact ? -(double)c : 0.75 * (double)t60::dt() - (double)c;
            mx += kU * ((double)px.x - x0);
            mz += kU * ((double)px.z - z0);
            if (yawRemFrame != 0.0f) {
                followTurnXZ(-(double)yawRemFrame, (double)px.x, (double)px.z, &x0, &z0);
                mx += 0.75 * ((double)px.x - x0);
                mz += 0.75 * ((double)px.z - z0);
            }
            mViewCache.mEye.x = (f32)((double)mViewCache.mEye.x + mx);
            mViewCache.mEye.z = (f32)((double)mViewCache.mEye.z + mz);
        }
        /* the latitude's remainders (FollowLatRem), the same form: the chased point m3CC against the same point
         * turned back in latitude by the remainder */
        if (latRem != 0.0f || latRemFrame != 0.0f) {
            cXyz px;
            cXyz_mi(&p->m3CC, &px, &mViewCache.mCenter);
            double dx, dy, dz;
            const double kL = latExact ? -(double)c : 0.75 * (double)t60::dt() - (double)c;
            followLatDisp((double)latRem, &px, &dx, &dy, &dz);
            double mx = kL * dx, my = kL * dy, mz = kL * dz;
            if (latRemFrame != 0.0f) {
                followLatDisp((double)latRemFrame, &px, &dx, &dy, &dz);
                mx += 0.75 * dx;
                my += 0.75 * dy;
                mz += 0.75 * dz;
            }
            mViewCache.mEye.x = (f32)((double)mViewCache.mEye.x + mx);
            mViewCache.mEye.y = (f32)((double)mViewCache.mEye.y + my);
            mViewCache.mEye.z = (f32)((double)mViewCache.mEye.z + mz);
        }
        cXyz_mi(&mViewCache.mEye, &s, &mViewCache.mCenter);
        cSGlobe_Val(&mViewCache.mDirection, &s);
        { /* the read-back the next step's latitude block starts from, and its remainder q (FollowLatRem) */
            FollowLatRem& lr = s_followLatRem();
            lr.readStep = t60_step_counter();
            lr.readValid = true;
            lr.readV = (s16)mViewCache.mDirection.mV;
            lr.readQ = followLatQ(&s, lr.readV);
        }
    }
    f32 e8 = p->m3E8;
    if (mEventFlags & 8)
        p->m3DA = 1;
    e8 = gabi::fmadds(v29 - e8, fr(0.01f), e8);
    p->m3E8 = e8;
    f32 fv = mViewCache.mFovy;
    mViewCache.mFovy = gabi::fmadds(v25 - fv, fr(e8), fv);
    if ((dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_DEKU_LEAF_FAN_e)) && frameEnd()) { /* DEKU_LEAF_FAN (true60: the
                                                                                           * jitter draw once a frame, the per-step chance rule) */
        f32 r = cM_rndFX(setup(this)->m078);                               /* mCamSetup.m078 */
        mViewCache.mFovy = mViewCache.mFovy + r;
    }
    if ((dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_DEKU_LEAF_FLY_e)) || cb1Flying() ||
        mdFlying() || hyoiKamome()) {
        f32 bank, ratio;
        if (height < 200.0f && (!hyoiKamome() || seagullDebug != 0)) {
            f32 q = height / 200.0f;
            ratio = gabi::fnmsubs(0.96f, q, 1.0f);
            bank = mStickMainPosXLast * q;
        } else {
            bank = mStickMainPosXLast;
            ratio = 0.04f;
        }
        cSAngle_l t32;
        cSAngle_l t4E;
        cSAngle_l t4C;
        cSAngle_l* sp = cameraFollowAngleShortCtor(&t32, (s16)(mFrameCounter << 7));
        f32 sn = cameraFollowAngleSin(sp); /* Sin */
        mViewCache.mFovy =
            gabi::fmadds(t60::dt() == 1.0f ? (f32)setup(this)->m07C : (f32)setup(this)->m07C * t60::dt(), sn,
                         mViewCache.mFovy); /* mCamSetup.m07C (true60: dt of the per-frame wobble per step) */
        f32 fan = cameraFollowFanBank(mCamSetup);                  /* dCamSetup_c::FanBank */
        cameraFollowAngleFloatCtor(&t4E, fan * bank);
        cameraFollowAngleSubtract(&t4E, &t4C, &mViewCache.mBank);
        cameraFollowAngleScale(&t4C, &t32, fr(ratio)); /* true60: per step */
        cameraFollowAngleAddAssign(&mViewCache.mBank, &t32); /* += */
        mEventFlags = mEventFlags | 0x400;
        if (hyoiKamome() && seagullDebug == 0) {
            /* HD: debug-register yaw of the centre around the eye while riding a seagull */
            cXyz dv;
            cXyz rv;
            cXyz c;
            cXyz_mi(&mViewCache.mCenter, &dv, &mViewCache.mEye);
            // HD: follow-camera reads this mutable matrix pointer; the shared stack helper uses a
            // fixed address.
            Mtx34* mtx = followMatrix.get(); /* mDoMtx_stack_c::now */
            f32 deg = angDegree(&mViewCache.mBank);
            f32 k = seagullYaw + -75.0f;
            mDoMtx_YrotS(mtx, (s16)gabi::ftoi(deg * k)); /* mDoMtx_YrotS */
            MtxPosition(&dv, &rv);                       /* MtxPosition */
            cXyz_pl(&mViewCache.mEye, &c, &rv);
            mViewCache.mCenter.copy(c);
            f32 lim = seagullFovy + 60.0f;
            if (mViewCache.mFovy > lim)
                mViewCache.mFovy = lim;
        }
    }
    return true;
}

/* WWHD glue (tools/verify/autoglue.py)
ext 02008FEC void cameraFollowLineCtor(dBgS_LinChk*)
ext 02008B4C void cameraFollowLineDtor(dBgS_LinChk*, s32)
ext 020073AC void cameraFollowGlobeXyz(cSGlobe_l*, cXyz*)
ext 02007324 void cameraFollowGlobeFromVector(cSGlobe_l*, cXyz*)
ext 0200658C cSAngle_l* cameraFollowAngleShortCtor(cSAngle_l*, s32)
ext 020066C0 cSAngle_l* cameraFollowAngleFloatCtor(cSAngle_l*, f32)
ext 0200693C void cameraFollowAngleScale(cSAngle_l*, cSAngle_l*, f32)
ext 02006894 void cameraFollowAngleAdd(cSAngle_l*, cSAngle_l*, cSAngle_l*)
ext 020068B0 void cameraFollowAngleSubtract(cSAngle_l*, cSAngle_l*, cSAngle_l*)
ext 02007068 void cameraFollowGlobeCtor(cSGlobe_l*, f32, s16, s16)
ext 0201AF1C void cameraFollowVectorMultiply(cXyz*, cXyz*, cXyz*)
ext 020065EC void cameraFollowShortSubtractAngle(cSAngle_l*, s16, cSAngle_l*)
ext 02006638 void cameraFollowAngleVal(cSAngle_l*, cSAngle_l*)
ext 024FCBE8 bool cameraFollowLineBlocked(dCamera_c*, cXyz*, cXyz*, u32)
ext 024EFB08 void cameraFollowMoveBgPosition(dBgS*, cBgS_PolyInfo*, s32, cXyz*, u32, u32)
ext 024F6F9C f32 cameraFollowBezierRatio(f32, f32)
ext 02006720 f32 cameraFollowAngleDegree(const cSAngle_l*)
ext 02006814 f32 cameraFollowAngleSin(cSAngle_l*)
ext 02006838 f32 cameraFollowAngleCos(cSAngle_l*)
ext 020069A0 void cameraFollowAngleScaleAssign(cSAngle_l*, f32)
ext 02006694 void cameraFollowAngleValFloat(cSAngle_l*, f32)
ext 02007248 void cameraFollowGlobeCtorAngles(cSGlobe_l*, f32, cSAngle_l*, cSAngle_l*)
ext 024F7AC4 f32 cameraFollowHorizontalDistance(cXyz*, cXyz*)
ext 025028A0 f32 cameraFollowLimit(f32, f32, f32)
ext 020068CC void cameraFollowAngleAddAssign(cSAngle_l*, cSAngle_l*)
ext 02007178 void cameraFollowGlobeCopyCtor(cSGlobe_l*, cSGlobe_l*)
ext 024F78F0 f32 cameraFollowFanBank(u8*)
ext 020084C8 cM3dGPla* cameraFollowGetTriPla(dBgS*, u32, u32)
data followMatrix 1018C7B0
data cb1FlyingFlag 101BDC2D
data mdFlyingFlag 101D5F3E
data hyoiKamomeFlag 101D5F45
data seagullDebug 1047B688
data seagullYaw 1047C038
data seagullFovy 1047C03C
*/

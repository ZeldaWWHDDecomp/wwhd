/* d_ev_camera, part 7: possessedEvCamera. See
 * d_ev_camera.cpp for the unit's layout notes; written from the WWHD code (the GameCube
 * d_ev_camera.cpp is all "Nonmatching" stubs). */
#include "d/d_ev_camera_7.h"

using namespace gabi;
namespace ev7Detail {
enum PossessedState { POSSESSED_INIT, POSSESSED_EASE, POSSESSED_DONE = 0x63 };
enum GameOverState {
    GAMEOVER_SEARCH,
    GAMEOVER_SEARCH_MIRROR,
    GAMEOVER_WAIT_LONG,
    GAMEOVER_CLOSE,
    GAMEOVER_WAIT_SHORT,
    GAMEOVER_HOLD,
    GAMEOVER_FIXED_INIT = 0x32,
    GAMEOVER_FIXED_SET,
    GAMEOVER_FIXED_WAIT
};

static char ev7Target[7] = {84, 97, 114, 103, 101, 116, 0};
static char ev7Player[8] = {64, 80, 76, 65, 89, 69, 82, 0};
static char ev7Type[5] = {84, 121, 112, 101, 0};
static char ev7Timer[6] = {84, 105, 109, 101, 114, 0};
static char ev7Radius[7] = {82, 97, 100, 105, 117, 115, 0};
static char ev7Cushion[8] = {67, 117, 115, 104, 105, 111, 110, 0};
static char ev7Latitude[9] = {76, 97, 116, 105, 116, 117, 100, 101, 0};
static char ev7Longitude[10] = {76, 111, 110, 103, 105, 116, 117, 100, 101, 0};
static char ev7Fovy[5] = {70, 111, 118, 121, 0};
static char ev7Blure[6] = {66, 108, 117, 114, 101, 0};

static f32 ev7BlureUniformScale;
static f32 ev7Zero;
static f32 ev7One;
static f32 ev7DefaultRadius;
static f32 ev7DefaultLatitude;
static f32 ev7DefaultFovy;
static f32 ev7BlureAlphaBase;
static f32 ev7BlureStrongAlpha;
static f32 ev7BlureAlphaRate;
static f32 ev7BlureScaleXRate;
static f32 ev7BlureScaleXBase;
static f32 ev7BlureScaleYRate;
static f32 ev7BlureScaleYBase;
static f32 ev7Search85;
static f32 ev7SearchMinus50;
static f32 ev7Search165;
static f32 ev7Search72;
static f32 ev7SearchMinus64;
static f32 ev7SearchMinus20;
static f32 ev7Search40;
static f32 ev7Search10;
static f32 ev7SearchMinus70;
static f32 ev7Search110;
static f32 ev7InitialCenterY;
static f32 ev7CloseEyeY;
static f32 ev7CloseEyeZ;
static f32 ev7CloseCenterY;
static f32 ev7CloseCenterZ;
static s16 ev7HalfTurn;
static s16 ev7AngleZero;
static f32 ev7FixedCenterZ;
static f32 ev7FixedEyeZ;
static f32 ev7FixedEyeX;
static f32 ev7FixedEyeY;
static f32 ev7FixedCenterY;

// HD: word snapshots preserve the original load/store ordering and NaN payloads.
/* inlined into possessedEvCamera/gameOverEvCamera */
static inline void copy3(u32 d, u32 s) {
    *at<be<u32>>(d + 0) = at<be<u32>>(s + 0)->get();
    *at<be<u32>>(d + 4) = at<be<u32>>(s + 4)->get();
    *at<be<u32>>(d + 8) = at<be<u32>>(s + 8)->get();
}

/* inlined into possessedEvCamera/gameOverEvCamera */
static inline void setDone(u32 c) {
    at<dCamera_c>(c)->m102 = 1;
    at<dCamera_c>(c)->m101 = 1;
    at<dCamera_c>(c)->m100 = 1;
}
/* the blur effect and a short rumble */
/* inlined into possessedEvCamera/gameOverEvCamera */
static inline void blureStart(u32 c, u32 w, u32 reset, f32 alpha, bool scale) {
    ev7_ResetBlure(c, reset);                                 /* ResetBlure */
    ev7_SetBlurePositionType(c, 0xB);                         /* SetBlurePositionType */
    ev7_SetBlureTimer(c, at<PossessedCameraWork>(w)->mTimer); /* SetBlureTimer */
    ev7_SetBlureAlpha(c, alpha);                              /* SetBlureAlpha */
    if (scale)
        ev7_SetBlureScaleUniform(c, ev7BlureUniformScale); /* SetBlureScale(0.99) */
    u32 g = dComIfGp_ea();
    gabi::Local<cXyz> dir;
    u32 d = gabi::ea(dir.get());
    f32 z0 = ev7Zero, one = ev7One;
    at<cXyz>(d)->x = z0;
    at<cXyz>(d)->y = one;
    at<cXyz>(d)->z = z0;
    ev7_StartShock(gabi::ea(&at<dComIfG_inf_c>(g)->play.mVibration), 1, 0x20,
                   d); /* dVibration_c::StartShock */
}

/* possessedEvCamera (the camera follows a possessed actor, e.g. a Hyoi pear seagull).
 * work: +0 state (0 init, 1 easing, 0x63 done), +4 Type, +8 Timer, +0xC frames left, +0x10 Radius,
 * +0x14 Latitude/Longitude (cSAngle), +0x18 Fovy, +0x1C Cushion, +0x20 Blure, +0x24 Target,
 * +0x28 target globe */
} // namespace ev7Detail
using namespace ev7Detail;
/* 0253A998 */
u32 PossessedEventCamera_c::possessedEvCamera() {
    const u32 i_this = ea();
    u32 w = gabi::ea(mWork);
    auto& work = *at<PossessedCameraWork>(w);
    u32 state;
    if (this->m11C != 0) {
        state = work.mState;
    } else {
        state = POSSESSED_INIT;
        work.mState = POSSESSED_INIT;
    }
    if (state == POSSESSED_DONE) {
        setDone(i_this);
        return 1;
    }
    if (state != POSSESSED_EASE) {
        u32 a = ev7_getEvActor2(i_this, gabi::ea(ev7Target) /* "Target" */,
                                gabi::ea(ev7Player) /* "@PLAYER" */);
        work.mpTarget.v = a;
        if (a == 0)
            return 1;
        ev7_getEvIntData(i_this, gabi::ea(&work.mType), gabi::ea(ev7Type) /* "Type" */, 0);
        ev7_getEvIntData(i_this, gabi::ea(&work.mTimer), gabi::ea(ev7Timer) /* "Timer" */, 10);
        ev7_getEvFloatData(i_this, gabi::ea(&work.mRadius), gabi::ea(ev7Radius) /* "Radius" */,
                           ev7DefaultRadius);
        ev7_getEvFloatData(i_this, gabi::ea(&work.mCushion), gabi::ea(ev7Cushion) /* "Cushion" */,
                           ev7One);
        gabi::Local<be<f32>> fv;
        u32 fp = gabi::ea(fv.get());
        ev7_getEvFloatData(i_this, fp, gabi::ea(ev7Latitude) /* "Latitude" */, ev7DefaultLatitude);
        ev7_cSAngle_Val(gabi::ea(&work.mLatitude),
                        at<be<f32>>(fp)->get()); /* cSAngle::Val(f32 degrees) */
        ev7_getEvFloatData(i_this, fp, gabi::ea(ev7Longitude) /* "Longitude" */, ev7Zero);
        ev7_cSAngle_Val(gabi::ea(&work.mLongitude), at<be<f32>>(fp)->get());
        ev7_getEvFloatData(i_this, gabi::ea(&work.mFovy), gabi::ea(ev7Fovy) /* "Fovy" */,
                           ev7DefaultFovy);
        ev7_getEvIntData(i_this, gabi::ea(&work.mBlure), gabi::ea(ev7Blure) /* "Blure" */, 0);
        u32 type = work.mType;
        u32 act = work.mpTarget.v.get();
        gabi::Local<be<s16>[2]> ang;
        u32 ap = gabi::ea(ang.get());
        if (type == 0) {
            ev7_directionOf(i_this, ap, act);
            ev7_cSAngle_pl(gabi::ea(&work.mLongitude), ap + 2, ap);
            ev7_cSGlobe_ValRVU(gabi::ea(&work.mTargetDirection), work.mRadius,
                               gabi::ea(&work.mLatitude), ap + 2);
            /* keep the current view in m0A4[1] */
            s16 bank = this->mViewCache.mBank;
            u32 x = at<be<u32>>(gabi::ea(&this->mViewCache.mCenter.x))->get();
            this->m0A4[1].mBank = (u16)bank;
            for (u32 i = 0; i < 0x18; i += 4)
                *at<be<u32>>(gabi::ea(&this->m0A4[1].mCenter) + i) =
                    i == 0 ? x : at<be<u32>>(gabi::ea(&this->mViewCache.mCenter) + i)->get();
            this->m0A4[1].m1E = 2;
            this->m0A4[1].mFovY = this->mViewCache.mFovy;
        } else {
            gabi::Local<cXyz> e;
            gabi::Local<cXyz> x;
            u32 ee = gabi::ea(e.get()), xx = gabi::ea(x.get());
            ev7_eyePos(i_this, ee, act);
            copy3(gabi::ea(&this->mViewCache.mCenter), ee);
            ev7_cXyz_mi(gabi::ea(&this->m0A4[1].mEye), ee, gabi::ea(&this->m0A4[1].mCenter));
            ev7_cSGlobe_Val(gabi::ea(&work.mTargetDirection), ee);
            ev7_directionOf(i_this, ap, work.mpTarget.v.get());
            ev7_cSAngle_pl(gabi::ea(&work.mLongitude), ap + 2, ap);
            ev7_cSGlobe_ValRVU(gabi::ea(&this->mViewCache.mDirection), work.mRadius,
                               gabi::ea(&work.mLatitude), ap + 2);
            ev7_cSGlobe_Xyz(gabi::ea(&this->mViewCache.mDirection), xx);
            ev7_cXyz_pl(gabi::ea(&this->mViewCache.mCenter), ee, xx);
            copy3(gabi::ea(&this->mViewCache.mEye), ee);
            this->mViewCache.mFovy = work.mFovy;
        }
        u32 blure = work.mBlure;
        work.mState = POSSESSED_EASE;
        work.mFramesLeft = work.mTimer;
        if (blure == 1)
            blureStart(i_this, w, 1, ev7BlureAlphaBase, false);
        else if (blure == 2)
            blureStart(i_this, w, 0, ev7BlureStrongAlpha, true);
        return 0;
    }
    /* state 1: ease towards the target globe around the actor's eye */
    f32 r = ev7One / (f32)(f64)(s32)work.mFramesLeft;
    gabi::Local<cXyz> a;
    gabi::Local<cXyz> b;
    gabi::Local<cXyz> c;
    gabi::Local<be<s16>[4]> an;
    gabi::Local<be<s16>[2]> ct;
    u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get()), cc = gabi::ea(c.get()),
        ang = gabi::ea(an.get()), cto = gabi::ea(ct.get());
    ev7_eyePos(i_this, aa, work.mpTarget.v.get());
    ev7_cXyz_mi(aa, bb, gabi::ea(&this->mViewCache.mCenter));
    ev7_cXyz_ml(bb, cc, r);
    ev7_vecAdd(gabi::ea(&this->mViewCache.mCenter), cc, gabi::ea(&this->mViewCache.mCenter));
    f32 r0 = this->mViewCache.mDirection.mRadius;
    this->mViewCache.mDirection.mRadius = gabi::fmadds(work.mTargetDirection.mRadius - r0, r, r0);
    ev7_cSAngle_mi(gabi::ea(&work.mTargetDirection.mV), ang + 4,
                   gabi::ea(&this->mViewCache.mDirection.mV));
    ev7_cSAngle_ml(ang + 4, ang + 2, r);
    ev7_cSAngle_pl(gabi::ea(&this->mViewCache.mDirection.mV), ang + 0, ang + 2);
    u32 t = ev7_cSAngleS(cto + 0, at<be<s16>>(ang + 0)->get());
    this->mViewCache.mDirection.mV = (u16)at<be<s16>>(t)->get();
    ev7_cSAngle_mi(gabi::ea(&work.mTargetDirection.mU), ang + 0,
                   gabi::ea(&this->mViewCache.mDirection.mU));
    ev7_cSAngle_ml(ang + 0, ang + 2, r);
    ev7_cSAngle_pl(gabi::ea(&this->mViewCache.mDirection.mU), ang + 4, ang + 2);
    t = ev7_cSAngleS(cto + 2, at<be<s16>>(ang + 4)->get());
    this->mViewCache.mDirection.mU = (u16)at<be<s16>>(t)->get();
    ev7_cSGlobe_Xyz(gabi::ea(&this->mViewCache.mDirection), bb);
    ev7_cXyz_pl(gabi::ea(&this->mViewCache.mCenter), aa, bb);
    gabi::Local<cXyz> eye;
    u32 ey = gabi::ea(eye.get());
    copy3(ey, aa);
    ev7_cXyz_mi(ey, bb, gabi::ea(&this->mViewCache.mEye));
    ev7_cXyz_ml(bb, aa, work.mCushion);
    ev7_vecAdd(gabi::ea(&this->mViewCache.mEye), aa, gabi::ea(&this->mViewCache.mEye));
    f32 fv = this->mViewCache.mFovy;
    this->mViewCache.mFovy = gabi::fmadds(work.mFovy - fv, r, fv);
    if (work.mBlure == 1) {
        s32 camId = ev7_GetCameraParam(this->mpCamera.v.get()); /* fopCamM_GetParam */
        s32 idx = at<dComIfG_inf_c>(dComIfGp_ea())->play.mCameraInfo[camId].mDlstWindowIdx.get();
        u32 win = gabi::ea(at<dComIfG_inf_c>(dComIfGp_ea())->play.mDlstWindow) +
                  (u32)idx * sizeof(EventCameraWindowHD);
        gabi::Local<cXyz> ep;
        gabi::Local<be<f32>[2]> scr;
        u32 epp = gabi::ea(ep.get()), sp = gabi::ea(scr.get());
        ev7_eyePos(i_this, epp, work.mpTarget.v.get());
        ev7_project(epp, sp); /* mDoLib_project */
        f32 px = at<be<f32>>(sp + 0)->get() / at<EventCameraWindowHD>(win)->mWidth.get();
        f32 py = at<be<f32>>(sp + 4)->get() / at<EventCameraWindowHD>(win)->mHeight.get();
        f32 z0 = ev7Zero;
        ev7_SetBlurePosition(i_this, px, py, z0); /* SetBlurePosition */
        ev7_SetBlureAlpha(i_this, gabi::fmadds(ev7BlureAlphaRate, r,
                                               ev7BlureAlphaBase)); /* SetBlureAlpha(0.7 r + 0.5) */
        f32 sx = gabi::fmadds(ev7BlureScaleXRate, r, ev7BlureScaleXBase);  /* 0.09 r + 1.1 */
        f32 sy = gabi::fnmsubs(ev7BlureScaleYRate, r, ev7BlureScaleYBase); /* 0.98 - 0.18 r */
        ev7_SetBlureScale(i_this, sx, sy, z0);                             /* SetBlureScale */
    }
    u32 n = work.mFramesLeft - 1; /* wraps like addic. */
    work.mFramesLeft = n;
    if ((s32)n > 0)
        return 0;
    work.mState = POSSESSED_DONE;
    return 0;
}
namespace ev7Detail {

/* inlined into possessedEvCamera/gameOverEvCamera */
static inline void negX(u32 v) { at<cXyz>(v)->x = -at<cXyz>(v)->x; }
/* eye = player-relational offset (rotated by ang), lifted to the player's floor height + 0x36C */
/* inlined into possessedEvCamera/gameOverEvCamera */
static inline void gameOverEye(u32 c, u32 ofs, u32 ang, u32 eye, u32 tmpAng) {
    gabi::Local<cXyz> r;
    gabi::Local<cXyz> p;
    u32 rr = gabi::ea(r.get()), pp = gabi::ea(p.get());
    u32 ap = ev7_cSAngleCopy(tmpAng, ang);
    ev7_relationalPosA(c, rr, at<dCamera_c>(c)->mpPlayerActor.v.get(), ofs, ap);
    copy3(eye, rr);
    ev7_positionOf(c, rr, at<dCamera_c>(c)->mpPlayerActor.v.get());
    if (at<cXyz>(eye)->y.get() <
        at<cXyz>(rr)->y.get() + at<dCamera_c>(c)->m368) { /* bge: taken on NaN */
        ev7_positionOf(c, pp, at<dCamera_c>(c)->mpPlayerActor.v.get());
        at<cXyz>(eye)->y = at<cXyz>(pp)->y.get() + at<dCamera_c>(c)->m368;
    }
}

/* gameOverEvCamera state 3: centre C and eye B around the player; when the view is blocked,
 * mirror B.z, then flatten C.z, then mirror B.z back */
/* inlined into possessedEvCamera/gameOverEvCamera */
static inline void gameOverEye3(u32 c, u32 w, u32 ang, u32 B, u32 C, u32 ctr, u32 eye3, u32 f) {
    gabi::Local<cXyz> r;
    u32 rr = gabi::ea(r.get());
    u32 ap = ev7_cSAngleCopy(gabi::ea(&at<GameOverCameraFrame>(f)->mCloseAngle), ang);
    ev7_relationalPosA(c, rr, at<dCamera_c>(c)->mpPlayerActor.v.get(), C, ap);
    copy3(ctr, rr);
    gameOverEye(c, B, ang, eye3, gabi::ea(&at<GameOverCameraFrame>(f)->mCloseAngle));
    if (ev7_lineBGCheck(c, ctr, eye3, 0x7F)) {
        at<cXyz>(B)->z = -at<cXyz>(B)->z.get();
        gameOverEye(c, B, ang, eye3, gabi::ea(&at<GameOverCameraFrame>(f)->mCopyAngleB));
        if (ev7_lineBGCheck(c, ctr, eye3, 0x7F)) {
            at<cXyz>(C)->z = ev7Zero;
            ap = ev7_cSAngleCopy(gabi::ea(&at<GameOverCameraFrame>(f)->mCopyAngleB), ang);
            ev7_relationalPosA(c, rr, at<dCamera_c>(c)->mpPlayerActor.v.get(), C, ap);
            copy3(ctr, rr);
            gameOverEye(c, B, ang, eye3, gabi::ea(&at<GameOverCameraFrame>(f)->mCopyAngleB));
            if (ev7_lineBGCheck(c, ctr, eye3, 0x7F)) {
                at<cXyz>(B)->z = -at<cXyz>(B)->z.get();
                gameOverEye(c, B, ang, eye3, gabi::ea(&at<GameOverCameraFrame>(f)->mCopyAngleB));
                ev7_lineBGCheck(c, ctr, eye3, 0x7F);
            }
        }
    }
    copy3(gabi::ea(&at<GameOverCameraWork>(w)->mCenterOffset), C);
    copy3(gabi::ea(&at<GameOverCameraWork>(w)->mEyeOffset), B);
    u32 r12 = at<GameOverCameraWork>(w)->mState + 1;
    u32 n4 = at<GameOverCameraWork>(w)->mTimer;
    at<GameOverCameraWork>(w)->mState = r12;
    if (n4 == 0x1E) {
        at<GameOverCameraWork>(w)->mTimer = 0;
        at<GameOverCameraWork>(w)->mState = r12 + 1;
    }
}

/* gameOverEvCamera: a state machine in work+0 (0/1: pick a clear eye offset around the
 * player from five, mirrored as needed; 2/3: after 130 frames a closer shot (tried mirrored and
 * lowered); 4: wait 30 frames; 0x32..0x34: the fixed variant used when ... (pad status bit 0x100000);
 * 5: hold). The centre/eye offsets are kept in work+0xC / work+0x18 and applied every frame. */
} // namespace ev7Detail
/* 02535AA4 */
u32 PossessedEventCamera_c::gameOverEvCamera() {
    const u32 i_this = ea();
    u32 sr25 = gabi::cpu->r[25], sr26 = gabi::cpu->r[26], sr27 = gabi::cpu->r[27];
    u32 w = gabi::ea(mWork);
    auto& work = *at<GameOverCameraWork>(w);
    f32 c0 = ev7Zero;
    /* the original's frame: five eye offsets at +0xE0, followed by its register save area */
    gabi::Local<GameOverCameraFrame> frame;
    u32 f = gabi::ea(frame.get());
    u32 A = gabi::ea(&at<GameOverCameraFrame>(f)->mInitialCenterOffset),
        B = gabi::ea(&at<GameOverCameraFrame>(f)->mCloseEyeOffset),
        C = gabi::ea(&at<GameOverCameraFrame>(f)->mCloseCenterOffset);
    frame.get()->mSearch.mEyeOffsets[0].x = ev7Search85;
    frame.get()->mSearch.mEyeOffsets[0].y = ev7SearchMinus50;
    frame.get()->mSearch.mEyeOffsets[0].z = ev7Search165; /* (85, -50, 165) */
    frame.get()->mSearch.mEyeOffsets[1].x = ev7Search72;
    frame.get()->mSearch.mEyeOffsets[1].y = ev7SearchMinus64;
    frame.get()->mSearch.mEyeOffsets[1].z = ev7DefaultRadius; /* (72, -64, 60) */
    frame.get()->mSearch.mEyeOffsets[2].x = ev7Search165;
    frame.get()->mSearch.mEyeOffsets[2].y = ev7SearchMinus20;
    frame.get()->mSearch.mEyeOffsets[2].z = ev7DefaultFovy; /* (165, -20, 45) */
    frame.get()->mSearch.mEyeOffsets[3].x = ev7Search85;
    frame.get()->mSearch.mEyeOffsets[3].y = ev7Search165;
    frame.get()->mSearch.mEyeOffsets[3].z = ev7Search40; /* (85, 165, 40) */
    frame.get()->mSearch.mEyeOffsets[4].x = ev7Search10;
    frame.get()->mSearch.mEyeOffsets[4].y = ev7SearchMinus70;
    frame.get()->mSearch.mEyeOffsets[4].z = ev7Search110; /* (10, -70, 110) */
    frame.get()->mSearch.mSave.mSavedRegisters[0] = sr25;
    frame.get()->mSearch.mSave.mSavedRegisters[1] = sr26;
    frame.get()->mSearch.mSave.mSavedRegisters[2] = sr27; /* stmw r25: the sixth "entry" */
    at<cXyz>(A)->x = c0;
    at<cXyz>(A)->y = ev7InitialCenterY;
    at<cXyz>(A)->z = c0; /* (0, -25, 0) */
    at<cXyz>(B)->x = c0;
    at<cXyz>(B)->y = ev7CloseEyeY;
    at<cXyz>(B)->z = ev7CloseEyeZ; /* (0, 170, 115) */
    at<cXyz>(C)->x = c0;
    at<cXyz>(C)->y = ev7CloseCenterY;
    at<cXyz>(C)->z = ev7CloseCenterZ; /* (0, -40, -35) */
    if (this->m11C == 0) {
        work.mState = GAMEOVER_SEARCH;
        work.mTimer = 0;
        work.mMirror = ((this->m07C >> 1) & 1) ^ 1;
        s32 pad = (s32)this->mPadId;
        u32 g = dComIfGp_ea();
        if (at<dComIfG_inf_c>(g)->play.mPlayerStatus[pad][0].get() & 0x100000)
            work.mState = GAMEOVER_FIXED_INIT;
        this->m102 = 1;
        this->m100 = 1;
        this->m101 = 1;
    }
    u32 ang = gabi::ea(&at<GameOverCameraFrame>(f)->mAngle);
    ev7_cSAngle_default(ang); /* cSAngle() */
    {
        s32 pad = (s32)this->mPadId;
        u32 g = dComIfGp_ea();
        if (at<dComIfG_inf_c>(g)->play.mPlayerStatus[pad][0].get() & 0x10000)
            *at<be<u16>>(ang) = (u16)ev7HalfTurn;
        else
            *at<be<u16>>(ang) = (u16)ev7AngleZero;
    }
    u32 s = work.mState;
    u32 eye = gabi::ea(&at<GameOverCameraFrame>(f)->mEye),
        ctr0 = gabi::ea(&at<GameOverCameraFrame>(f)->mInitialCenter),
        ctr = gabi::ea(&at<GameOverCameraFrame>(f)->mCloseCenter),
        eye3 = gabi::ea(&at<GameOverCameraFrame>(f)->mCloseEye);
    bool toT = false;
    if (s == GAMEOVER_HOLD)
        toT = true;
    else if (s == GAMEOVER_FIXED_INIT || s == GAMEOVER_FIXED_SET || s == GAMEOVER_FIXED_WAIT) {
        if (s == GAMEOVER_FIXED_INIT)
            work.mState = GAMEOVER_FIXED_SET;
        if (s != GAMEOVER_FIXED_WAIT) {
            u32 v = work.mState;
            work.mCenterOffset.z = ev7FixedCenterZ; /* 30 */
            work.mEyeOffset.z = ev7FixedEyeZ;       /* 175 */
            work.mState = v + 1;
            work.mEyeOffset.x = ev7FixedEyeX; /* 70 */
            work.mEyeOffset.y = ev7FixedEyeY; /* 155 */
            work.mCenterOffset.x = c0;
            work.mCenterOffset.y = ev7FixedCenterY; /* 14 */
        }
        if (work.mTimer == 0xA0) {
            work.mTimer = 0;
            work.mState = GAMEOVER_HOLD;
        }
        toT = true;
    } else if (s == GAMEOVER_WAIT_SHORT) {
        if (work.mTimer == 0x1E) {
            work.mTimer = 0;
            work.mState = s + 1;
        }
        toT = true;
    }
    if (!toT) {
        u32 r12 = s;
        if (s != GAMEOVER_WAIT_LONG && s != GAMEOVER_CLOSE) {
            if (s != GAMEOVER_SEARCH_MIRROR)
                work.mState = GAMEOVER_SEARCH_MIRROR;
            gabi::Local<cXyz> r;
            u32 rr = gabi::ea(r.get());
            ev7_relationalPos(i_this, rr, this->mpPlayerActor.v.get(), A);
            copy3(ctr0, rr);
            u32 k = 0;
            for (; k < 5; k++) {
                u32 e = gabi::ea(&frame.get()->mSearch.mEyeOffsets[k]);
                if (work.mMirror != 0)
                    negX(e);
                gameOverEye(i_this, e, ang, eye,
                            gabi::ea(&at<GameOverCameraFrame>(f)->mCopyAngleA));
                if (!ev7_lineBGCheck(i_this, ctr0, eye, 0x7F))
                    break;
                negX(e);
                gameOverEye(i_this, e, ang, eye,
                            gabi::ea(&at<GameOverCameraFrame>(f)->mCopyAngleA));
                if (!ev7_lineBGCheck(i_this, ctr0, eye, 0x7F))
                    break;
                work.mMirror = work.mMirror ^ 1;
            }
            copy3(gabi::ea(&this->mViewCache.mEye), eye);
            copy3(gabi::ea(&work.mCenterOffset), A);
            copy3(gabi::ea(&work.mEyeOffset), gabi::ea(&frame.get()->mSearch.mEyeOffsets[k]));
            r12 = work.mState + 1;
            work.mState = r12;
            s = GAMEOVER_WAIT_LONG;
        }
        if (s == GAMEOVER_WAIT_LONG) {
            if (work.mTimer != 0x82)
                toT = true;
            else {
                work.mTimer = 0;
                work.mState = r12 + 1;
            }
        }
        if (!toT) {
            /* state 3: the closer shot */
            gameOverEye3(i_this, w, ang, B, C, ctr, eye3, f);
        }
    }
    /* T: apply the offsets */
    gabi::Local<cXyz> r;
    u32 rr = gabi::ea(r.get());
    u32 ap = ev7_cSAngleCopy(gabi::ea(&at<GameOverCameraFrame>(f)->mApplyAngle), ang);
    ev7_relationalPosA(i_this, rr, this->mpPlayerActor.v.get(), gabi::ea(&work.mCenterOffset), ap);
    copy3(gabi::ea(&this->mViewCache.mCenter), rr);
    ap = ev7_cSAngleCopy(gabi::ea(&at<GameOverCameraFrame>(f)->mApplyAngle), ang);
    ev7_relationalPosA(i_this, rr, this->mpPlayerActor.v.get(), gabi::ea(&work.mEyeOffset), ap);
    copy3(gabi::ea(&this->mViewCache.mEye), rr);
    work.mTimer = work.mTimer + 1;
    this->m102 = 1;
    this->m100 = 1;
    this->m101 = 1;
    return 1;
}

/* WWHD glue
ext 02530634 u32 ev7_getEvIntData(u32 c, u32 out, u32 name, s32 def)
ext 0253072C u32 ev7_getEvFloatData(u32 c, u32 out, u32 name, f32 def)
ext 02530CB4 u32 ev7_getEvActor2(u32 c, u32 name, u32 def)
ext 024FC0B0 void ev7_eyePos(u32 c, u32 out, u32 actor)
ext 024F8F30 void ev7_directionOf(u32 c, u32 out, u32 actor)
ext 0201ADE0 void ev7_cXyz_mi(u32 a, u32 out, u32 b)
ext 0201AD78 void ev7_cXyz_pl(u32 a, u32 out, u32 b)
ext 0201AE48 void ev7_cXyz_ml(u32 a, u32 out, f32 s)
ext 028E8D88 void ev7_vecAdd(u32 a, u32 b, u32 out)
ext 020072E0 void ev7_cSGlobe_Val(u32 g, u32 v)
ext 020071E0 void ev7_cSGlobe_ValRVU(u32 g, f32 r, u32 v, u32 u)
ext 020073AC void ev7_cSGlobe_Xyz(u32 g, u32 out)
ext 020068B0 void ev7_cSAngle_mi(u32 a, u32 out, u32 b)
ext 0200693C void ev7_cSAngle_ml(u32 a, u32 out, f32 f)
ext 02006894 void ev7_cSAngle_pl(u32 a, u32 out, u32 b)
ext 0200658C u32 ev7_cSAngleS(u32 tmp, s16 v)
ext 024F8D5C void ev7_positionOf(u32 c, u32 out, u32 actor)
ext 0250242C void ev7_relationalPos(u32 c, u32 out, u32 actor, u32 ofs)
ext 02514D00 void ev7_relationalPosA(u32 c, u32 out, u32 actor, u32 ofs, u32 ang)
ext 024FCBE8 bool ev7_lineBGCheck(u32 c, u32 a, u32 b, u32 flags)
ext 02006644 u32 ev7_cSAngleCopy(u32 out, u32 src)
ext 0250D464 void ev7_ResetBlure(u32,u32)
ext 025153FC void ev7_SetBlurePositionType(u32,u32)
ext 0250D4C0 void ev7_SetBlureTimer(u32,u32)
ext 0250D4C8 void ev7_SetBlureAlpha(u32,f32)
ext 0250D4D0 void ev7_SetBlureScaleUniform(u32,f32)
ext 025CB374 void ev7_StartShock(u32,s32,s32,u32)
ext 02006694 void ev7_cSAngle_Val(u32,f32)
ext 025DA64C s32 ev7_GetCameraParam(u32)
ext 025F0C48 void ev7_project(u32,u32)
ext 02515404 void ev7_SetBlurePosition(u32,f32,f32,f32)
ext 025153EC void ev7_SetBlureScale(u32,f32,f32,f32)
ext 020065FC void ev7_cSAngle_default(u32)
data ev7BlureUniformScale 1004D1EC
data ev7Zero 1004CE58
data ev7One 1004CEA4
data ev7DefaultRadius 1004D004
data ev7DefaultLatitude 1004CF68
data ev7DefaultFovy 1004D008
data ev7BlureAlphaBase 1004D4B0
data ev7BlureStrongAlpha 1004D4B4
data ev7BlureAlphaRate 1004D4B8
data ev7BlureScaleXRate 1004D4BC
data ev7BlureScaleXBase 1004D4C0
data ev7BlureScaleYRate 1004D4C8
data ev7BlureScaleYBase 1004D4C4
data ev7Search85 1004D0B8
data ev7SearchMinus50 1004D0BC
data ev7Search165 1004D0C0
data ev7Search72 1004D0C4
data ev7SearchMinus64 1004D0C8
data ev7SearchMinus20 1004D0CC
data ev7Search40 1004D0D0
data ev7Search10 1004CF74
data ev7SearchMinus70 1004D0D4
data ev7Search110 1004D0D8
data ev7InitialCenterY 1004D0B4
data ev7CloseEyeY 1004D0E4
data ev7CloseEyeZ 1004D0E8
data ev7CloseCenterY 1004D0DC
data ev7CloseCenterZ 1004D0E0
data ev7HalfTurn 101FF35C
data ev7AngleZero 101FF354
data ev7FixedCenterZ 1004CF84
data ev7FixedEyeZ 1004D0F4
data ev7FixedEyeX 1004CF70
data ev7FixedEyeY 1004D0F0
data ev7FixedCenterY 1004D0EC
data ev7Target 1004D4EC
data ev7Player 1004D4CC
data ev7Type 1004D4F4
data ev7Timer 1004D4DC
data ev7Radius 1004D4FC
data ev7Cushion 1004D4D4
data ev7Latitude 1004D518
data ev7Longitude 1004D50C
data ev7Fovy 1004D504
data ev7Blure 1004D4E4
*/

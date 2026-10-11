/* d_ev_camera, part 11: watchActorEvCamera. See
 * d_ev_camera.cpp for the unit's layout notes; written from the WWHD code (the GameCube
 * d_ev_camera.cpp is all "Nonmatching" stubs). */
#include "d/d_ev_camera_11.h"
#include <cmath>

using namespace gabi;
namespace watchActorDetail {

// HD: per-TU defaults and cSAngle::_0 remain guest-memory reads.
static s16 watchAngleZero;
static f32 watchZeroX;
static f32 watchZeroY;
static f32 watchZeroZ;
static f32 watchDefaultCushion;
static f32 watchDefaultNearDist;
static f32 watchDefaultFarDist;
static u32 watchDefaultNearTimer;
static u32 watchDefaultFarTimer;
static f32 watchDefaultZoomDist;
static f32 watchDefaultZoomVAngle;
static f32 watchDefaultFrontAngle;

enum WatchMode { WATCH_KEEP, WATCH_NEAR, WATCH_MIDDLE, WATCH_JUMP };

static cXyz watchZero;
static u32 watchZeroReady;
static char watchCtrGap[7] = {67, 116, 114, 71, 97, 112, 0};
static char watchCushion[8] = {67, 117, 115, 104, 105, 111, 110, 0};
static char watchNearDist[9] = {78, 101, 97, 114, 68, 105, 115, 116, 0};
static char watchZoomDist[9] = {90, 111, 111, 109, 68, 105, 115, 116, 0};
static char watchZoomVAngle[11] = {90, 111, 111, 109, 86, 65, 110, 103, 108, 101, 0};
static char watchFarDist[8] = {70, 97, 114, 68, 105, 115, 116, 0};
static char watchNearTimer[10] = {78, 101, 97, 114, 84, 105, 109, 101, 114, 0};
static char watchFarTimer[9] = {70, 97, 114, 84, 105, 109, 101, 114, 0};
static char watchFrontAngle[11] = {70, 114, 111, 110, 116, 65, 110, 103, 108, 101, 0};
static char watchBlure[6] = {66, 108, 117, 114, 101, 0};
static char watchTarget[7] = {84, 97, 114, 103, 101, 116, 0};
static char watchStarter[9] = {64, 83, 84, 65, 82, 84, 69, 82, 0};

// HD: preserve word copies separately from float copies (NaN payloads and call clobbers).
/* inlined into watchActorEvCamera */
static inline void copyGlobe(u32 d, u32 s) {
    *at<be<u32>>(d) = at<be<u32>>(s)->get();
    *at<be<u32>>(d + 4) = at<be<u32>>(s + 4)->get();
}
/* inlined into watchActorEvCamera */
static inline void copy3(u32 destination, u32 source) {
    auto* dst = at<be<u32>[3]>(destination);
    auto* src = at<be<u32>[3]>(source);
    (*dst)[0] = (*src)[0].get();
    (*dst)[1] = (*src)[1].get();
    (*dst)[2] = (*src)[2].get();
}
/* inlined into watchActorEvCamera */
static inline void copy3f(u32 destination, u32 source) {
    auto& dst = *at<cXyz>(destination);
    auto& src = *at<cXyz>(source);
    dst.x = src.x.get();
    dst.y = src.y.get();
    dst.z = src.z.get();
}

/* inlined into watchActorEvCamera */
static inline void setDone(u32 c) {
    auto& camera = *at<dCamera_c>(c);
    camera.m102 = 1;
    camera.m101 = 1;
    camera.m100 = 1;
}
/* *dst = cSAngle(*a + *b) through temporaries */
/* inlined into watchActorEvCamera */
static inline void angAdd(u32 dst, u32 a, u32 b) {
    gabi::Local<be<s16>[2]> t;
    u32 tp = gabi::ea(t.get());
    cSAngle_pl(a, tp, b);
    u32 r = cSAngleS(tp + 2, at<be<s16>>(tp)->get());
    *at<be<s16>>(dst) = at<be<s16>>(r)->get();
}

/* keep the eye direction U within FrontAngle of the actor's facing; diff = U - facing (before) */
/* inlined into watchActorEvCamera */
static inline void clampFront(u32 c, u32 w, u32 u, u32 diff) {
    auto& work = *at<WatchActorWork>(w);
    gabi::Local<be<s16>[8]> t;
    u32 tp = gabi::ea(t.get());
    ::directionOf(c, tp + 0, work.mpTarget.v.get());
    cSAngle_mi(u, diff, tp + 0);
    cSAngle_ct0(tp + 2);
    u32 lim = cSAngleF(tp + 4, -work.mFrontAngle);
    if (at<be<s16>>(diff)->get() < at<be<s16>>(lim)->get()) {
        ::directionOf(c, tp + 6, work.mpTarget.v.get());
        u32 a = cSAngleF(tp + 8, -work.mFrontAngle);
        angAdd(u, tp + 6, a);
    } else {
        u32 lim2 = cSAngleF(tp + 10, work.mFrontAngle);
        if (at<be<s16>>(diff)->get() > at<be<s16>>(lim2)->get()) {
            ::directionOf(c, tp + 12, work.mpTarget.v.get());
            u32 a = cSAngleF(tp + 14, work.mFrontAngle);
            angAdd(u, tp + 12, a);
        }
    }
}
/* turn the globe gl around the centre (U by `step`, V towards +-5 degrees) until the view from the
 * eye is clear (45 tries); `ok` stays 0 once the actor faces too far away (special actors) */
/* inlined into watchActorEvCamera */
static inline bool searchView(u32 c, u32 w, u32 gl, u32 step, bool special) {
    auto& camera = *at<dCamera_c>(c);
    auto& work = *at<WatchActorWork>(w);
    gabi::Local<cXyz> x;
    gabi::Local<cXyz> p;
    gabi::Local<cXyz> e;
    gabi::Local<be<s16>[8]> t;
    u32 xx = gabi::ea(x.get()), pp = gabi::ea(p.get()), ee = gabi::ea(e.get()),
        tp = gabi::ea(t.get());
    u32 ok = 1;
    for (u32 i = 0; i < 45; i++) {
        cSGlobe_Xyz(gl, xx);
        cXyz_pl(gabi::ea(&work.mCenter), pp, xx);
        copy3(ee, pp);
        if (ok != 0 && !::lineBGCheck(c, gabi::ea(&work.mCenter), ee, 0x8F)) {
            copy3f(xx, gabi::ea(&work.mCenter));
            copy3f(pp, ee);
            u32 player = camera.mpPlayerActor.v.get();
            u32 actor = work.mpTarget.v.get();
            u32 g = dComIfGp_ea();
            if (!cameraCollisionCheck(g + 0x26A4, xx, pp, 15.0f, player, actor))
                return true; /* dCcS::ChkCamera */
        }
        angAdd(gl + 6, gl + 6, step);
        if (special) {
            ::directionOf(c, tp + 0, work.mpTarget.v.get());
            cSAngle_mi(gl + 6, tp + 2, tp + 0);
            f32 deg = cSAngle_Degree(tp + 2); /* cSAngle::Degree */
            ok = std::fabs(deg) < 70.0f;
        }
        u32 a = cSAngleF(tp + 4, (i & 2) ? -5.0f : 5.0f);
        gabi::Local<be<s16>[4]> v;
        u32 vp = gabi::ea(v.get());
        cSAngle_pl(gl + 4, vp + 0, a);
        cSAngle_ml(gl + 4, vp + 2, 0.1f);
        cSAngle_mi(vp + 0, vp + 4, vp + 2);
        u32 r = cSAngleS(vp + 6, at<be<s16>>(vp + 4)->get());
        at<cSGlobe_l>(gl)->mV = (u16)at<be<s16>>(r)->get();
    }
    return false;
}
/* the search step: 8 degrees, against the side the actor faces */
/* inlined into watchActorEvCamera */
static inline void searchStep(u32 st_, u32 diff, const s16* zero) {
    cSAngle_ct0(st_);
    s16 z = zero ? *zero : watchAngleZero; /* mode 1 keeps cSAngle::_0 in a register */
    cSAngle_Val(st_, at<be<s16>>(diff)->get() < z ? 8.0f : -8.0f); /* cSAngle::Val(f32) */
}
/* move the view towards the target globe (work + 0x4C) and centre over `n` frames */
/* inlined into watchActorEvCamera */
static inline u32 blend(u32 c, u32 w, u32 n) {
    auto& camera = *at<dCamera_c>(c);
    auto& work = *at<WatchActorWork>(w);
    u32 t = camera.m11C;
    if (t >= n) {
        setDone(c);
        return 1;
    }
    f32 ratio = (f32)(f64)t / (f32)(f64)(s32)n;
    gabi::Local<cXyz> a;
    gabi::Local<cXyz> b;
    gabi::Local<be<s16>[6]> an;
    u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get()), ap = gabi::ea(an.get());
    cXyz_mi(gabi::ea(&work.mCenter), aa, gabi::ea(&camera.mViewCache.mCenter));
    cXyz_ml(aa, bb, ratio);
    PSVECAdd(gabi::ea(&camera.mViewCache.mCenter), bb,
             gabi::ea(&camera.mViewCache.mCenter)); /* PSVECAdd */
    f32 r0 = camera.mViewCache.mDirection.mRadius;
    camera.mViewCache.mDirection.mRadius =
        gabi::fmadds(work.mTargetDirection.mRadius - r0, ratio, r0);
    cSAngle_mi(gabi::ea(&work.mTargetDirection.mU), ap + 4,
               gabi::ea(&camera.mViewCache.mDirection.mU));
    cSAngle_ml(ap + 4, ap + 2, ratio);
    cSAngle_pl(gabi::ea(&camera.mViewCache.mDirection.mU), ap + 0, ap + 2);
    u32 r = cSAngleS(ap + 6, at<be<s16>>(ap + 0)->get());
    camera.mViewCache.mDirection.mU = (u16)at<be<s16>>(r)->get();
    cSAngle_mi(gabi::ea(&work.mTargetDirection.mV), ap + 0,
               gabi::ea(&camera.mViewCache.mDirection.mV));
    cSAngle_ml(ap + 0, ap + 2, ratio);
    cSAngle_pl(gabi::ea(&camera.mViewCache.mDirection.mV), ap + 4, ap + 2);
    r = cSAngleS(ap + 8, at<be<s16>>(ap + 4)->get());
    camera.mViewCache.mDirection.mV = (u16)at<be<s16>>(r)->get();
    cSGlobe_Xyz(gabi::ea(&camera.mViewCache.mDirection), bb);
    cXyz_pl(gabi::ea(&camera.mViewCache.mCenter), aa, bb);
    copy3(gabi::ea(&camera.mViewCache.mEye), aa);
    return 0;
}
/* the view from the target globe */
/* inlined into watchActorEvCamera */
static inline void viewFromTarget(u32 c, u32 w) {
    auto& camera = *at<dCamera_c>(c);
    auto& work = *at<WatchActorWork>(w);
    f32 r = work.mTargetDirection.mRadius;
    camera.mViewCache.mDirection.mRadius = r;
    camera.mViewCache.mDirection.mV = (u16)work.mTargetDirection.mV;
    camera.mViewCache.mDirection.mU = (u16)work.mTargetDirection.mU;
    gabi::Local<cXyz> x;
    gabi::Local<cXyz> e;
    u32 xx = gabi::ea(x.get()), ee = gabi::ea(e.get());
    cSGlobe_Xyz(gabi::ea(&camera.mViewCache.mDirection), xx);
    cXyz_pl(gabi::ea(&camera.mViewCache.mCenter), ee, xx);
    copy3(gabi::ea(&camera.mViewCache.mEye), ee);
}

/* watchActorEvCamera: look at the "Target" (centre = target + CtrGap) from a clear point.
 * Mode by the distance at the start: 0 the target is in sight (keep the view), 1 near (turn towards
 * it over NearTimer frames), 2 middle (move over FarTimer), 3 far (jump). The eye position is
 * searched around the target (45 tries, dCcS::ChkCamera) for a clear line of sight. work: +0 CtrGap,
 * +0xC centre, +0x18 Cushion, +0x1C NearTimer, +0x20 NearDist, +0x24 FarTimer, +0x28 FarDist, +0x2C
 * ZoomDist, +0x30 ZoomVAngle, +0x34 Target, +0x38 its process id, +0x3C globe to the eye, +0x4C
 * target globe, +0x54 mode, +0x58 Blure, +0x5C FrontAngle */
} // namespace watchActorDetail

using namespace watchActorDetail;

/* 02533450 */
u32 WatchActorCamera_c::watchActorEvCamera() {
    const u32 cameraAddress = ea();
    // HD: this function owns its guarded copy of cXyz::Zero.
    if (watchZeroReady == 0) {
        f32 x = watchZeroX, y = watchZeroY;
        watchZero.x = x;
        f32 z = watchZeroZ;
        watchZero.y = y;
        watchZero.z = z;
        watchZeroReady = 1;
    }
    u32 w = gabi::ea(mWork);
    auto& work = *at<WatchActorWork>(w);
    if (this->m11C == 0) {
        gabi::Local<cXyz> def;
        u32 d = gabi::ea(def.get());
        f32 z = watchZero.z.get(), y = watchZero.y.get(), x = watchZero.x.get();
        def.get()->z = z;
        def.get()->x = x;
        def.get()->y = y;
        getEvXyzData(cameraAddress, gabi::ea(&work.mCtrGap), gabi::ea(watchCtrGap) /* "CtrGap" */,
                     d);
        getEvFloatData(cameraAddress, gabi::ea(&work.mCushion),
                       gabi::ea(watchCushion) /* "Cushion" */, watchDefaultCushion);
        getEvFloatData(cameraAddress, gabi::ea(&work.mNearDist),
                       gabi::ea(watchNearDist) /* "NearDist" */, watchDefaultNearDist);
        getEvFloatData(cameraAddress, gabi::ea(&work.mZoomDist),
                       gabi::ea(watchZoomDist) /* "ZoomDist" */, watchDefaultZoomDist);
        getEvFloatData(cameraAddress, gabi::ea(&work.mZoomVAngle),
                       gabi::ea(watchZoomVAngle) /* "ZoomVAngle" */, watchDefaultZoomVAngle);
        getEvFloatData(cameraAddress, gabi::ea(&work.mFarDist),
                       gabi::ea(watchFarDist) /* "FarDist" */, watchDefaultFarDist);
        getEvIntData(cameraAddress, gabi::ea(&work.mNearTimer),
                     gabi::ea(watchNearTimer) /* "NearTimer" */, (s32)watchDefaultNearTimer);
        getEvIntData(cameraAddress, gabi::ea(&work.mFarTimer),
                     gabi::ea(watchFarTimer) /* "FarTimer" */, (s32)watchDefaultFarTimer);
        getEvFloatData(cameraAddress, gabi::ea(&work.mFrontAngle),
                       gabi::ea(watchFrontAngle) /* "FrontAngle" */, watchDefaultFrontAngle);
        getEvIntData(cameraAddress, gabi::ea(&work.mBlure), gabi::ea(watchBlure) /* "Blure" */, 0);
        u32 a = getEvActor2(cameraAddress, gabi::ea(watchTarget) /* "Target" */,
                            gabi::ea(watchStarter) /* "@STARTER" */);
        work.mpTarget = at<fopAc_ac_c>(a);
        if (a == 0)
            return 1;
        work.mTargetId = a != 0 ? at<fopAc_ac_c>(a)->mBsPcId : 0xFFFFFFFF;
        ::relationalPos(cameraAddress, d, work.mpTarget.v.get(), gabi::ea(&work.mCtrGap));
        copy3(gabi::ea(&work.mCenter), d);
        cXyz_mi(gabi::ea(&this->mViewCache.mEye), d, gabi::ea(&work.mCenter));
        cSGlobe_Val(gabi::ea(&work.mEyeDirection), d);
        f32 dist = work.mEyeDirection.mRadius;
        if (dist < work.mNearDist) { /* bge: taken on NaN */
            work.mMode =
                ::pointInSight(cameraAddress, gabi::ea(&work.mCenter)) ^ 1; /* !pointInSight */
        } else {
            work.mMode = dist < work.mFarDist ? WATCH_MIDDLE : WATCH_JUMP;
        }
        setDone(cameraAddress);
    }
    {
        gabi::Local<be<u32>> id;
        u32 ip = gabi::ea(id.get());
        u32 pid = work.mTargetId;
        *id.get() = pid;
        if (pid == 0xFFFFFFFF)
            return 1;
        if (judgeActorById(0x025E1234 /* fpcSch_JudgeByID */, ip) == 0)
            return 1;
    }
    u32 ac = work.mpTarget.v.get();
    bool special = false;
    if (ac != 0) {
        s16 nm = at<fopAc_ac_c>(ac)->mProfName;
        special = nm == 0x12C || nm == 0x12D || nm == 0x131 || nm == 0x130 || nm == 0x73;
    }
    u32 mode = work.mMode;
    if (mode == WATCH_KEEP) {
        setDone(cameraAddress);
        copy3(gabi::ea(&this->mViewCache.mEye), gabi::ea(&this->mEye));
        copyGlobe(gabi::ea(&this->mViewCache.mDirection), gabi::ea(&this->mDirection));
        return 1;
    }
    if (mode > WATCH_JUMP) {
        setDone(cameraAddress);
        return 1;
    }
    gabi::Local<cXyz> attentionPoint;
    gabi::Local<cXyz> dv;
    gabi::Local<cSGlobe_l> globe;
    gabi::Local<be<s16>[8]> an;
    gabi::Local<cSGlobe_l> gl;
    gabi::Local<be<s16>> step;
    u32 attentionAddress = gabi::ea(attentionPoint.get()), deltaAddress = gabi::ea(dv.get()),
        directionAddress = gabi::ea(globe.get()), tp = gabi::ea(an.get());
    u32 searchDirectionAddress = gabi::ea(gl.get()), stepAddress = gabi::ea(step.get());
    if (mode == WATCH_NEAR) {
        if (this->m11C == 0) {
            ::attentionPos(cameraAddress, attentionAddress, this->mpPlayerActor.v.get());
            attentionPoint.get()->y = attentionPoint.get()->y + 10.0f;
            cXyz_mi(attentionAddress, deltaAddress, gabi::ea(&work.mCenter));
            cSGlobe_ct(directionAddress, deltaAddress);
            gabi::Local<cXyz> pos;
            gabi::Local<cXyz> q;
            gabi::Local<cSGlobe_l> h;
            u32 targetPositionAddress = gabi::ea(pos.get()), eyeDeltaAddress = gabi::ea(q.get()),
                eyeDirectionAddress = gabi::ea(h.get());
            ::positionOf(cameraAddress, targetPositionAddress, work.mpTarget.v.get());
            cXyz_mi(gabi::ea(&this->mViewCache.mEye), eyeDeltaAddress, targetPositionAddress);
            cSGlobe_ct(eyeDirectionAddress, eyeDeltaAddress);
            ::directionOf(cameraAddress, tp + 0, work.mpTarget.v.get());
            cSAngle_mi(eyeDirectionAddress + 6, tp + 2, tp + 0);
            s16 zero = watchAngleZero; /* cSAngle::_0, kept in a register */
            u32 a5 = cSAngleF(tp + 4, (*an.get())[1].get() < zero ? 5.0f : -5.0f);
            angAdd(directionAddress + 6, directionAddress + 6, a5);
            clampFront(cameraAddress, w, directionAddress + 6, tp + 6);
            cSGlobe_ValRVU(gabi::ea(&work.mTargetDirection),
                           at<cSGlobe_l>(directionAddress)->mRadius + 120.0f, directionAddress + 4,
                           directionAddress + 6);
            searchStep(stepAddress, tp + 6, &zero);
            cSGlobe_copy(searchDirectionAddress,
                         gabi::ea(&work.mTargetDirection)); /* cSGlobe(const cSGlobe&) */
            if (searchView(cameraAddress, w, searchDirectionAddress, stepAddress, special)) {
                copyGlobe(gabi::ea(&work.mTargetDirection), searchDirectionAddress);
            }
        }
        return blend(cameraAddress, w, work.mNearTimer);
    }
    if (mode == WATCH_MIDDLE) {
        if (this->m11C == 0) {
            ::attentionPos(cameraAddress, attentionAddress, this->mpPlayerActor.v.get());
            cXyz_mi(attentionAddress, deltaAddress, gabi::ea(&work.mCenter));
            cSGlobe_ct(directionAddress, deltaAddress);
            f32 zv = work.mZoomVAngle;
            if (zv != 0.0f) {
                u32 a = cSAngleF(tp + 0, zv);
                u32 r = cSAngleS(tp + 2, at<be<s16>>(a)->get());
                at<cSGlobe_l>(directionAddress)->mV = (u16)at<be<s16>>(r)->get();
            }
            clampFront(cameraAddress, w, directionAddress + 6, tp + 4);
            f32 gr = at<cSGlobe_l>(directionAddress)->mRadius;
            f32 zd = work.mZoomDist;
            if (std::fabs(gr - zd) < 30.0f) {                          /* bge: taken on NaN */
                cSAngle_AddShort(directionAddress + 6, tp + 6, 0x384); /* cSAngle::operator+(s16) */
                u32 r = cSAngleS(tp + 8, (*an.get())[3].get());
                at<cSGlobe_l>(directionAddress)->mU = (u16)at<be<s16>>(r)->get();
                zd = work.mZoomDist;
            }
            cSGlobe_ValRVU(gabi::ea(&work.mTargetDirection), zd, directionAddress + 4,
                           directionAddress + 6);
            searchStep(stepAddress, tp + 4, nullptr);
            cSGlobe_copy(searchDirectionAddress, gabi::ea(&work.mTargetDirection));
            if (searchView(cameraAddress, w, searchDirectionAddress, stepAddress, special)) {
                copyGlobe(gabi::ea(&work.mTargetDirection), searchDirectionAddress);
            }
        }
        return blend(cameraAddress, w, work.mFarTimer);
    }
    /* mode 3: jump */
    if (this->m11C == 0) {
        copy3(gabi::ea(&this->mViewCache.mCenter), gabi::ea(&work.mCenter));
        ::attentionPos(cameraAddress, attentionAddress, this->mpPlayerActor.v.get());
        cXyz_mi(attentionAddress, deltaAddress, gabi::ea(&work.mCenter));
        cSGlobe_ct(directionAddress, deltaAddress);
        globe.get()->mRadius = work.mZoomDist.get();
        clampFront(cameraAddress, w, directionAddress + 6, tp + 4);
        f32 zv = work.mZoomVAngle;
        if (zv != 0.0f) {
            u32 a = cSAngleF(tp + 0, zv);
            u32 r = cSAngleS(tp + 2, at<be<s16>>(a)->get());
            at<cSGlobe_l>(directionAddress)->mV = (u16)at<be<s16>>(r)->get();
        }
        cSGlobe_ValRVU(gabi::ea(&work.mTargetDirection), work.mZoomDist, directionAddress + 4,
                       directionAddress + 6);
        searchStep(stepAddress, tp + 4, nullptr);
        cSGlobe_copy(searchDirectionAddress, gabi::ea(&work.mTargetDirection));
        if (searchView(cameraAddress, w, searchDirectionAddress, stepAddress, special)) {
            f32 r = at<cSGlobe_l>(searchDirectionAddress)->mRadius;
            work.mTargetDirection.mRadius = r;
            work.mTargetDirection.mV = (u16)at<cSGlobe_l>(searchDirectionAddress)->mV;
            work.mTargetDirection.mU = (u16)at<cSGlobe_l>(searchDirectionAddress)->mU;
        }
    }
    viewFromTarget(cameraAddress, w);
    setDone(cameraAddress);
    return 1;
}

/* WWHD glue
ext 02530634 u32 getEvIntData(u32 c, u32 out, u32 name, s32 def)
ext 0253072C u32 getEvFloatData(u32 c, u32 out, u32 name, f32 def)
ext 0253086C u32 getEvXyzData(u32 c, u32 out, u32 name, u32 def)
ext 02530CB4 u32 getEvActor2(u32 c, u32 name, u32 def)
ext 0250242C void relationalPos(u32 c, u32 out, u32 actor, u32 ofs)
ext 0201ADE0 void cXyz_mi(u32 a, u32 out, u32 b)
ext 0201AD78 void cXyz_pl(u32 a, u32 out, u32 b)
ext 0201AE48 void cXyz_ml(u32 a, u32 out, f32 s)
ext 020072E0 void cSGlobe_Val(u32 g, u32 v)
ext 020071E0 void cSGlobe_ValRVU(u32 g, f32 r, u32 v, u32 u)
ext 020073AC void cSGlobe_Xyz(u32 g, u32 out)
ext 02007324 void cSGlobe_ct(u32 g, u32 v)
ext 020065FC void cSAngle_ct0(u32 a)
ext 020068B0 void cSAngle_mi(u32 a, u32 out, u32 b)
ext 02006894 void cSAngle_pl(u32 a, u32 out, u32 b)
ext 0200693C void cSAngle_ml(u32 a, u32 out, f32 f)
ext 020066C0 u32 cSAngleF(u32 tmp, f32 deg)
ext 0200658C u32 cSAngleS(u32 tmp, s16 v)
ext 024F8D5C void positionOf(u32 c, u32 out, u32 actor)
ext 024F8F30 void directionOf(u32 c, u32 out, u32 actor)
ext 024F8F3C void attentionPos(u32 c, u32 out, u32 actor)
ext 024FCBE8 bool lineBGCheck(u32 c, u32 a, u32 b, u32 flags)
ext 025187D8 bool cameraCollisionCheck(u32, u32, u32, f32, u32, u32)
ext 02006720 f32 cSAngle_Degree(u32)
ext 02006694 void cSAngle_Val(u32, f32)
ext 028E8D88 void PSVECAdd(u32, u32, u32)
ext 025050E4 u32 pointInSight(u32, u32)
ext 025D5218 u32 judgeActorById(u32, u32)
ext 02007178 void cSGlobe_copy(u32, u32)
ext 020068F4 void cSAngle_AddShort(u32, u32, s32)
data watchZero 10475878
data watchZeroReady 104758AC
data watchCtrGap 1004CFA0
data watchCushion 1004CF88
data watchNearDist 1004CFBC
data watchZoomDist 1004CFC8
data watchZoomVAngle 1004CFD4
data watchFarDist 1004CF90
data watchNearTimer 1004CFB0
data watchFarTimer 1004CFE0
data watchFrontAngle 1004CFEC
data watchBlure 1004CF98
data watchTarget 1004CFA8
data watchStarter 1004CFF8
data watchAngleZero 101FF354
data watchZeroX 101FFBA8
data watchZeroY 101FFBAC
data watchZeroZ 101FFBB0
data watchDefaultCushion 101D6204
data watchDefaultNearDist 101D6208
data watchDefaultFarDist 101D620C
data watchDefaultNearTimer 101D6210
data watchDefaultFarTimer 101D6214
data watchDefaultZoomDist 101D6218
data watchDefaultZoomVAngle 101D621C
data watchDefaultFrontAngle 101D6220
*/

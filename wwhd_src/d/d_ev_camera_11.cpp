/* d_ev_camera, part 11: watchActorEvCamera. See
 * d_ev_camera.cpp for the unit's layout notes; written from the WWHD code (the GameCube
 * d_ev_camera.cpp is all "Nonmatching" stubs). */
#include "bindings.h"
#include <cmath>

namespace d_ev_camera_11_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline s16 lds16(u32 a) { return gabi::load<s16>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
static inline void copy3(u32 d, u32 s) {
    st(d + 0, ld(s + 0));
    st(d + 4, ld(s + 4));
    st(d + 8, ld(s + 8));
}
static inline void copy3f(u32 d, u32 s) {
    stf(d + 0, ldf(s + 0));
    stf(d + 4, ldf(s + 4));
    stf(d + 8, ldf(s + 8));
}
static inline u32 gi() { return gabi::call<u32>(0x025200D4); }
static inline u32 getEvIntData(u32 c, u32 out, u32 name, s32 def) { return gabi::call<u32>(0x02530634, c, out, name, def); }
static inline u32 getEvFloatData(u32 c, u32 out, u32 name, f32 def) { return gabi::call<u32>(0x0253072C, c, out, name, def); }
static inline u32 getEvXyzData(u32 c, u32 out, u32 name, u32 def) { return gabi::call<u32>(0x0253086C, c, out, name, def); }
static inline u32 getEvActor2(u32 c, u32 name, u32 def) { return gabi::call<u32>(0x02530CB4, c, name, def); }
static inline void relationalPos(u32 c, u32 out, u32 actor, u32 ofs) { gabi::call(0x0250242C, c, out, actor, ofs); }
static inline void cXyz_mi(u32 a, u32 out, u32 b) { gabi::call(0x0201ADE0, a, out, b); }
static inline void cXyz_pl(u32 a, u32 out, u32 b) { gabi::call(0x0201AD78, a, out, b); }
static inline void cXyz_ml(u32 a, u32 out, f32 s) { gabi::call(0x0201AE48, a, out, s); }
static inline void cSGlobe_Val(u32 g, u32 v) { gabi::call(0x020072E0, g, v); }
static inline void cSGlobe_ValRVU(u32 g, f32 r, u32 v, u32 u) { gabi::call(0x020071E0, g, r, v, u); }
static inline void cSGlobe_Xyz(u32 g, u32 out) { gabi::call(0x020073AC, g, out); }
static inline void cSGlobe_ct(u32 g, u32 v) { gabi::call(0x02007324, g, v); }
static inline void cSAngle_ct0(u32 a) { gabi::call(0x020065FC, a); } /* cSAngle() */
static inline void cSAngle_mi(u32 a, u32 out, u32 b) { gabi::call(0x020068B0, a, out, b); }
static inline void cSAngle_pl(u32 a, u32 out, u32 b) { gabi::call(0x02006894, a, out, b); }
static inline void cSAngle_ml(u32 a, u32 out, f32 f) { gabi::call(0x0200693C, a, out, f); }
static inline u32 cSAngleF(u32 tmp, f32 deg) { return gabi::call<u32>(0x020066C0, tmp, deg); } /* cSAngle(f32 degrees) */
static inline u32 cSAngleS(u32 tmp, s16 v) { return gabi::call<u32>(0x0200658C, tmp, (s32)v); }
static inline void positionOf(u32 c, u32 out, u32 actor) { gabi::call(0x024F8D5C, c, out, actor); }
static inline void directionOf(u32 c, u32 out, u32 actor) { gabi::call(0x024F8F30, c, out, actor); }
static inline void attentionPos(u32 c, u32 out, u32 actor) { gabi::call(0x024F8F3C, c, out, actor); }
static inline bool lineBGCheck(u32 c, u32 a, u32 b, u32 flags) { return gabi::call<bool>(0x024FCBE8, c, a, b, flags); }
static inline void setDone(u32 c) {
    st8(c + 0x102, 1);
    st8(c + 0x101, 1);
    st8(c + 0x100, 1);
}
/* *dst = cSAngle(*a + *b) through temporaries */
static inline void angAdd(u32 dst, u32 a, u32 b) {
    gabi::Local<s16[2]> t;
    u32 tp = gabi::ea(t.get());
    cSAngle_pl(a, tp, b);
    u32 r = cSAngleS(tp + 2, lds16(tp));
    st16(dst, (u16)lds16(r));
}

/* keep the eye direction U within FrontAngle of the actor's facing; diff = U - facing (before) */
static inline void clampFront(u32 c, u32 w, u32 u, u32 diff) {
    gabi::Local<s16[8]> t;
    u32 tp = gabi::ea(t.get());
    directionOf(c, tp + 0, ld(w + 0x34));
    cSAngle_mi(u, diff, tp + 0);
    cSAngle_ct0(tp + 2);
    u32 lim = cSAngleF(tp + 4, -ldf(w + 0x5C));
    if (lds16(diff) < lds16(lim)) {
        directionOf(c, tp + 6, ld(w + 0x34));
        u32 a = cSAngleF(tp + 8, -ldf(w + 0x5C));
        angAdd(u, tp + 6, a);
    } else {
        u32 lim2 = cSAngleF(tp + 10, ldf(w + 0x5C));
        if (lds16(diff) > lds16(lim2)) {
            directionOf(c, tp + 12, ld(w + 0x34));
            u32 a = cSAngleF(tp + 14, ldf(w + 0x5C));
            angAdd(u, tp + 12, a);
        }
    }
}
/* turn the globe gl around the centre (U by `step`, V towards +-5 degrees) until the view from the
 * eye is clear (45 tries); `ok` stays 0 once the actor faces too far away (special actors) */
static inline bool searchView(u32 c, u32 w, u32 gl, u32 step, bool special) {
    gabi::Local<cXyz> x;
    gabi::Local<cXyz> p;
    gabi::Local<cXyz> e;
    gabi::Local<s16[8]> t;
    u32 xx = gabi::ea(x.get()), pp = gabi::ea(p.get()), ee = gabi::ea(e.get()), tp = gabi::ea(t.get());
    u32 ok = 1;
    for (u32 i = 0; i < 45; i++) {
        cSGlobe_Xyz(gl, xx);
        cXyz_pl(w + 0xC, pp, xx);
        copy3(ee, pp);
        if (ok != 0 && !lineBGCheck(c, w + 0xC, ee, 0x8F)) {
            copy3f(xx, w + 0xC);
            copy3f(pp, ee);
            u32 player = ld(c + 0x128);
            u32 actor = ld(w + 0x34);
            u32 g = gi();
            if (!gabi::call<bool>(0x025187D8, g + 0x26A4, xx, pp, 15.0f, player, actor)) return true; /* dCcS::ChkCamera */
        }
        angAdd(gl + 6, gl + 6, step);
        if (special) {
            directionOf(c, tp + 0, ld(w + 0x34));
            cSAngle_mi(gl + 6, tp + 2, tp + 0);
            f32 deg = gabi::call<f32>(0x02006720, tp + 2); /* cSAngle::Degree */
            ok = std::fabs(deg) < 70.0f;
        }
        u32 a = cSAngleF(tp + 4, (i & 2) ? -5.0f : 5.0f);
        gabi::Local<s16[4]> v;
        u32 vp = gabi::ea(v.get());
        cSAngle_pl(gl + 4, vp + 0, a);
        cSAngle_ml(gl + 4, vp + 2, 0.1f);
        cSAngle_mi(vp + 0, vp + 4, vp + 2);
        u32 r = cSAngleS(vp + 6, lds16(vp + 4));
        st16(gl + 4, (u16)lds16(r));
    }
    return false;
}
/* the search step: 8 degrees, against the side the actor faces */
static inline void searchStep(u32 st_, u32 diff, const s16* zero) {
    cSAngle_ct0(st_);
    s16 z = zero ? *zero : lds16(0x101FF354); /* mode 1 keeps cSAngle::_0 in a register */
    gabi::call(0x02006694, st_, lds16(diff) < z ? 8.0f : -8.0f); /* cSAngle::Val(f32) */
}
/* move the view towards the target globe (work + 0x4C) and centre over `n` frames */
static inline u32 blend(u32 c, u32 w, u32 n) {
    u32 t = ld(c + 0x11C);
    if (t >= n) {
        setDone(c);
        return 1;
    }
    f32 ratio = (f32)(f64)t / (f32)(f64)(s32)n;
    gabi::Local<cXyz> a;
    gabi::Local<cXyz> b;
    gabi::Local<s16[6]> an;
    u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get()), ap = gabi::ea(an.get());
    cXyz_mi(w + 0xC, aa, c + 0x44);
    cXyz_ml(aa, bb, ratio);
    gabi::call(0x028E8D88, c + 0x44, bb, c + 0x44); /* PSVECAdd */
    f32 r0 = ldf(c + 0x3C);
    stf(c + 0x3C, gabi::fmadds(ldf(w + 0x4C) - r0, ratio, r0));
    cSAngle_mi(w + 0x52, ap + 4, c + 0x42);
    cSAngle_ml(ap + 4, ap + 2, ratio);
    cSAngle_pl(c + 0x42, ap + 0, ap + 2);
    u32 r = cSAngleS(ap + 6, lds16(ap + 0));
    st16(c + 0x42, (u16)lds16(r));
    cSAngle_mi(w + 0x50, ap + 0, c + 0x40);
    cSAngle_ml(ap + 0, ap + 2, ratio);
    cSAngle_pl(c + 0x40, ap + 4, ap + 2);
    r = cSAngleS(ap + 8, lds16(ap + 4));
    st16(c + 0x40, (u16)lds16(r));
    cSGlobe_Xyz(c + 0x3C, bb);
    cXyz_pl(c + 0x44, aa, bb);
    copy3(c + 0x50, aa);
    return 0;
}
/* the view from the target globe */
static inline void viewFromTarget(u32 c, u32 w) {
    f32 r = ldf(w + 0x4C);
    stf(c + 0x3C, r);
    st16(c + 0x40, (u16)lds16(w + 0x50));
    st16(c + 0x42, (u16)lds16(w + 0x52));
    gabi::Local<cXyz> x;
    gabi::Local<cXyz> e;
    u32 xx = gabi::ea(x.get()), ee = gabi::ea(e.get());
    cSGlobe_Xyz(c + 0x3C, xx);
    cXyz_pl(c + 0x44, ee, xx);
    copy3(c + 0x50, ee);
}

/* 02533450: watchActorEvCamera: look at the "Target" (centre = target + CtrGap) from a clear point.
 * Mode by the distance at the start: 0 the target is in sight (keep the view), 1 near (turn towards
 * it over NearTimer frames), 2 middle (move over FarTimer), 3 far (jump). The eye position is
 * searched around the target (45 tries, dCcS::ChkCamera) for a clear line of sight. work: +0 CtrGap,
 * +0xC centre, +0x18 Cushion, +0x1C NearTimer, +0x20 NearDist, +0x24 FarTimer, +0x28 FarDist, +0x2C
 * ZoomDist, +0x30 ZoomVAngle, +0x34 Target, +0x38 its process id, +0x3C globe to the eye, +0x4C
 * target globe, +0x54 mode, +0x58 Blure, +0x5C FrontAngle */
static u32 dCamera_c_watchActorEvCamera(u32 i_this) {
    WWHD_FUNC(0x02533450, u32, i_this);
    u32 dz = 0x10475878; /* function-local copy of cXyz::Zero */
    if (ld(0x104758AC) == 0) {
        f32 x = ldf(0x101FFBA8), y = ldf(0x101FFBAC);
        stf(dz + 0, x);
        f32 z = ldf(0x101FFBB0);
        stf(dz + 4, y);
        stf(dz + 8, z);
        st(0x104758AC, 1);
    }
    u32 w = i_this + 0x37C;
    if (ld(i_this + 0x11C) == 0) {
        gabi::Local<cXyz> def;
        u32 d = gabi::ea(def.get());
        f32 z = ldf(dz + 8), y = ldf(dz + 4), x = ldf(dz + 0);
        stf(d + 8, z);
        stf(d + 0, x);
        stf(d + 4, y);
        getEvXyzData(i_this, w + 0, 0x1004CFA0 /* "CtrGap" */, d);
        getEvFloatData(i_this, w + 0x18, 0x1004CF88 /* "Cushion" */, ldf(0x101D6204));
        getEvFloatData(i_this, w + 0x20, 0x1004CFBC /* "NearDist" */, ldf(0x101D6208));
        getEvFloatData(i_this, w + 0x2C, 0x1004CFC8 /* "ZoomDist" */, ldf(0x101D6218));
        getEvFloatData(i_this, w + 0x30, 0x1004CFD4 /* "ZoomVAngle" */, ldf(0x101D621C));
        getEvFloatData(i_this, w + 0x28, 0x1004CF90 /* "FarDist" */, ldf(0x101D620C));
        getEvIntData(i_this, w + 0x1C, 0x1004CFB0 /* "NearTimer" */, (s32)ld(0x101D6210));
        getEvIntData(i_this, w + 0x24, 0x1004CFE0 /* "FarTimer" */, (s32)ld(0x101D6214));
        getEvFloatData(i_this, w + 0x5C, 0x1004CFEC /* "FrontAngle" */, ldf(0x101D6220));
        getEvIntData(i_this, w + 0x58, 0x1004CF98 /* "Blure" */, 0);
        u32 a = getEvActor2(i_this, 0x1004CFA8 /* "Target" */, 0x1004CFF8 /* "@STARTER" */);
        st(w + 0x34, a);
        if (a == 0) return 1;
        st(w + 0x38, a != 0 ? ld(a + 4) : 0xFFFFFFFF);
        relationalPos(i_this, d, ld(w + 0x34), w + 0);
        copy3(w + 0xC, d);
        cXyz_mi(i_this + 0x50, d, w + 0xC);
        cSGlobe_Val(w + 0x3C, d);
        f32 dist = ldf(w + 0x3C);
        if (dist < ldf(w + 0x20)) { /* bge: taken on NaN */
            st(w + 0x54, gabi::call<u32>(0x025050E4, i_this, w + 0xC) ^ 1); /* !pointInSight */
        } else {
            st(w + 0x54, dist < ldf(w + 0x28) ? 2 : 3);
        }
        setDone(i_this);
    }
    {
        gabi::Local<u32> id;
        u32 ip = gabi::ea(id.get());
        u32 pid = ld(w + 0x38);
        st(ip, pid);
        if (pid == 0xFFFFFFFF) return 1;
        if (gabi::call<u32>(0x025D5218, 0x025E1234 /* fpcSch_JudgeByID */, ip) == 0) return 1;
    }
    u32 ac = ld(w + 0x34);
    bool special = false;
    if (ac != 0) {
        s16 nm = lds16(ac + 0xE);
        special = nm == 0x12C || nm == 0x12D || nm == 0x131 || nm == 0x130 || nm == 0x73;
    }
    u32 mode = ld(w + 0x54);
    if (mode == 0) {
        setDone(i_this);
        copy3(i_this + 0x50, i_this + 0x1C);
        st(i_this + 0x3C, ld(i_this + 8));
        st(i_this + 0x40, ld(i_this + 0xC));
        return 1;
    }
    if (mode > 3) {
        setDone(i_this);
        return 1;
    }
    gabi::Local<cXyz> at;
    gabi::Local<cXyz> dv;
    gabi::Local<u8[8]> globe;
    gabi::Local<s16[8]> an;
    gabi::Local<u8[8]> gl;
    gabi::Local<s16> step;
    u32 A = gabi::ea(at.get()), B = gabi::ea(dv.get()), G = gabi::ea(globe.get()), tp = gabi::ea(an.get());
    u32 GL = gabi::ea(gl.get()), ST = gabi::ea(step.get());
    if (mode == 1) {
        if (ld(i_this + 0x11C) == 0) {
            attentionPos(i_this, A, ld(i_this + 0x128));
            stf(A + 4, ldf(A + 4) + 10.0f);
            cXyz_mi(A, B, w + 0xC);
            cSGlobe_ct(G, B);
            gabi::Local<cXyz> pos;
            gabi::Local<cXyz> q;
            gabi::Local<u8[8]> h;
            u32 P = gabi::ea(pos.get()), Q = gabi::ea(q.get()), H = gabi::ea(h.get());
            positionOf(i_this, P, ld(w + 0x34));
            cXyz_mi(i_this + 0x50, Q, P);
            cSGlobe_ct(H, Q);
            directionOf(i_this, tp + 0, ld(w + 0x34));
            cSAngle_mi(H + 6, tp + 2, tp + 0);
            s16 zero = lds16(0x101FF354); /* cSAngle::_0, kept in a register */
            u32 a5 = cSAngleF(tp + 4, lds16(tp + 2) < zero ? 5.0f : -5.0f);
            angAdd(G + 6, G + 6, a5);
            clampFront(i_this, w, G + 6, tp + 6);
            cSGlobe_ValRVU(w + 0x4C, ldf(G) + 120.0f, G + 4, G + 6);
            searchStep(ST, tp + 6, &zero);
            gabi::call(0x02007178, GL, w + 0x4C); /* cSGlobe(const cSGlobe&) */
            if (searchView(i_this, w, GL, ST, special)) {
                st(w + 0x4C, ld(GL + 0));
                st(w + 0x50, ld(GL + 4));
            }
        }
        return blend(i_this, w, ld(w + 0x1C));
    }
    if (mode == 2) {
        if (ld(i_this + 0x11C) == 0) {
            attentionPos(i_this, A, ld(i_this + 0x128));
            cXyz_mi(A, B, w + 0xC);
            cSGlobe_ct(G, B);
            f32 zv = ldf(w + 0x30);
            if (zv != 0.0f) {
                u32 a = cSAngleF(tp + 0, zv);
                u32 r = cSAngleS(tp + 2, lds16(a));
                st16(G + 4, (u16)lds16(r));
            }
            clampFront(i_this, w, G + 6, tp + 4);
            f32 gr = ldf(G);
            f32 zd = ldf(w + 0x2C);
            if (std::fabs(gr - zd) < 30.0f) { /* bge: taken on NaN */
                gabi::call(0x020068F4, G + 6, tp + 6, 0x384); /* cSAngle::operator+(s16) */
                u32 r = cSAngleS(tp + 8, lds16(tp + 6));
                st16(G + 6, (u16)lds16(r));
                zd = ldf(w + 0x2C);
            }
            cSGlobe_ValRVU(w + 0x4C, zd, G + 4, G + 6);
            searchStep(ST, tp + 4, nullptr);
            gabi::call(0x02007178, GL, w + 0x4C);
            if (searchView(i_this, w, GL, ST, special)) {
                st(w + 0x4C, ld(GL + 0));
                st(w + 0x50, ld(GL + 4));
            }
        }
        return blend(i_this, w, ld(w + 0x24));
    }
    /* mode 3: jump */
    if (ld(i_this + 0x11C) == 0) {
        copy3(i_this + 0x44, w + 0xC);
        attentionPos(i_this, A, ld(i_this + 0x128));
        cXyz_mi(A, B, w + 0xC);
        cSGlobe_ct(G, B);
        stf(G, ldf(w + 0x2C));
        clampFront(i_this, w, G + 6, tp + 4);
        f32 zv = ldf(w + 0x30);
        if (zv != 0.0f) {
            u32 a = cSAngleF(tp + 0, zv);
            u32 r = cSAngleS(tp + 2, lds16(a));
            st16(G + 4, (u16)lds16(r));
        }
        cSGlobe_ValRVU(w + 0x4C, ldf(w + 0x2C), G + 4, G + 6);
        searchStep(ST, tp + 4, nullptr);
        gabi::call(0x02007178, GL, w + 0x4C);
        if (searchView(i_this, w, GL, ST, special)) {
            f32 r = ldf(GL);
            stf(w + 0x4C, r);
            st16(w + 0x50, (u16)lds16(GL + 4));
            st16(w + 0x52, (u16)lds16(GL + 6));
        }
    }
    viewFromTarget(i_this, w);
    setDone(i_this);
    return 1;
}
VERIFY(0x02533450, dCamera_c_watchActorEvCamera);

} // namespace d_ev_camera_11_cpp

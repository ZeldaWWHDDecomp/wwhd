/* d_ev_camera, part 2: fixed-position / fixed-frame event cameras. See d_ev_camera.cpp for the unit's layout notes; written from the WWHD
 * code (the GameCube d_ev_camera.cpp is all "Nonmatching" stubs). */
#include "bindings.h"

namespace d_ev_camera_2_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
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

/* the parameter helpers (d_ev_camera.cpp) */
static inline u32 getEvIntData(u32 c, u32 out, u32 name, s32 def) { return gabi::call<u32>(0x02530634, c, out, name, def); }
static inline u32 getEvFloatData(u32 c, u32 out, u32 name, f32 def) { return gabi::call<u32>(0x0253072C, c, out, name, def); }
static inline u32 getEvXyzData(u32 c, u32 out, u32 name, u32 def) { return gabi::call<u32>(0x0253086C, c, out, name, def); }
static inline u32 getEvStringData(u32 c, u32 out, u32 name, u32 def) { return gabi::call<u32>(0x02530998, c, out, name, def); }
static inline u32 getEvActor(u32 c, u32 name) { return gabi::call<u32>(0x02530B84, c, name); }
static inline u32 getEvActor2(u32 c, u32 name, u32 def) { return gabi::call<u32>(0x02530CB4, c, name, def); }
static inline void relationalPos(u32 c, u32 out, u32 actor, u32 ofs) { gabi::call(0x0250242C, c, out, actor, ofs); }
static inline void cXyz_mi(u32 a, u32 out, u32 b) { gabi::call(0x0201ADE0, a, out, b); }
static inline void cXyz_pl(u32 a, u32 out, u32 b) { gabi::call(0x0201AD78, a, out, b); }
static inline void cXyz_ml(u32 a, u32 out, f32 s) { gabi::call(0x0201AE48, a, out, s); }
static inline void cSGlobe_Val(u32 g, u32 v) { gabi::call(0x020072E0, g, v); }
static inline void cSGlobe_Xyz(u32 g, u32 out) { gabi::call(0x020073AC, g, out); }
static inline u32 fopAcIt_Judge_l(u32 fn, u32 data) { return gabi::call<u32>(0x025D5218, fn, data); }

enum : u32 { EV_FLAGS = 0x510, EV_TIMER = 0x11C, EV_WORK = 0x37C };
static inline void positionOf(u32 c, u32 out, u32 actor) { gabi::call(0x024F8D5C, c, out, actor); }
static inline void attentionPos(u32 c, u32 out, u32 actor) { gabi::call(0x024F8F3C, c, out, actor); }
static inline void directionOf(u32 c, u32 out, u32 actor) { gabi::call(0x024F8F30, c, out, actor); }
static inline void cSGlobe_ct(u32 g, u32 v) { gabi::call(0x02007324, g, v); }
static inline void cSAngle_mi(u32 a, u32 out, u32 b) { gabi::call(0x020068B0, a, out, b); }
static inline f32 vecLen(u32 v) { return gabi::call<f32>(0x028F4384, gabi::call<f32>(0x028E8DD0, v)); } /* sqrtf(PSVECSquareMag) */
static inline bool lineBGCheck(u32 c, u32 a, u32 b, u32 flags) { return gabi::call<bool>(0x024FCBE8, c, a, b, flags); }
static inline void negX(u32 v) { stf(v, -ldf(v)); }
/* the bank angle of an event camera: mBank = cSAngle(deg * 0x8000/180), flag 0x400 */
static inline void setBank(u32 c, f32 deg) {
    gabi::Local<s16> ang;
    u32 res = gabi::call<u32>(0x0200658C, gabi::ea(ang.get()), (s32)(s16)gabi::ftoi(deg * 182.04444885253906f));
    u32 fl = ld(c + EV_FLAGS);
    s16 bank = lds16(res);
    st(c + EV_FLAGS, fl | 0x400);
    st16(c + 0x5C, (u16)bank);
}
/* mViewCache: centre, eye and the direction between them */
static inline void setView(u32 c, u32 ctr, u32 eye) {
    gabi::Local<cXyz> d;
    u32 dd = gabi::ea(d.get());
    copy3(c + 0x44, ctr);
    copy3(c + 0x50, eye);
    cXyz_mi(c + 0x50, dd, c + 0x44);
    cSGlobe_Val(c + 0x3C, dd);
}


static inline void setDone(u32 c) {
    st8(c + 0x102, 1);
    st8(c + 0x101, 1);
    st8(c + 0x100, 1);
}
/* the function-local copy of cXyz::Zero (101FFBA8) used as a default */
static inline u32 localZero(u32 guard, u32 v) {
    if (ld(guard) == 0) {
        f32 x = ldf(0x101FFBA8), y = ldf(0x101FFBAC);
        stf(v + 0, x);
        f32 z = ldf(0x101FFBB0);
        stf(v + 4, y);
        stf(v + 8, z);
        st(guard, 1);
    }
    return v;
}
/* the float ratio timer / n as GHS converts it: unsigned timer, signed n */
static inline f32 timerRatio(u32 t, s32 n) { return (f32)(f64)t / (f32)(f64)n; }

/* relational position modes of fixedFrameEvCamera (one character of RelUseMask), result in dst:
 * 'n': the offset mirrored when the actor faces away from the camera eye, 'p': mirrored when that
 * puts the point closer to the player, 't': added to the actor's attention position */
static inline void relN(u32 c, u32 w, u32 actor, u32 ofs, u32 dst) {
    gabi::Local<cXyz> pos;
    gabi::Local<cXyz> d;
    gabi::Local<u8[8]> globe;
    gabi::Local<s16[2]> ang;
    gabi::Local<cXyz> r;
    u32 pp = gabi::ea(pos.get()), dd = gabi::ea(d.get()), gg = gabi::ea(globe.get()), aa = gabi::ea(ang.get());
    u32 rr = gabi::ea(r.get());
    positionOf(c, pp, actor);
    cXyz_mi(c + 0x1C, dd, pp);
    cSGlobe_ct(gg, dd);
    directionOf(c, aa, ld(w + 0x24));
    cSAngle_mi(gg + 6, aa + 2, aa);
    if (lds16(aa + 2) < lds16(0x101FF354)) negX(ofs); /* cSAngle::_0 */
    relationalPos(c, rr, ld(w + 0x24), ofs);
    copy3(dst, rr);
}
static inline void relP(u32 c, u32 w, u32 actor, u32 ofs, u32 dst) {
    gabi::Local<cXyz> t;
    gabi::Local<cXyz> p;
    gabi::Local<cXyz> d;
    u32 tt = gabi::ea(t.get()), pp = gabi::ea(p.get()), dd = gabi::ea(d.get());
    relationalPos(c, tt, actor, ofs);
    positionOf(c, pp, ld(c + 0x128));
    cXyz_mi(tt, dd, pp);
    f32 d1 = vecLen(dd);
    negX(ofs);
    relationalPos(c, pp, ld(w + 0x24), ofs);
    copy3(tt, pp);
    positionOf(c, dd, ld(c + 0x128));
    cXyz_mi(tt, pp, dd);
    f32 d2 = vecLen(pp);
    if (d1 > d2) negX(ofs); /* ble: not taken on NaN */
    relationalPos(c, pp, ld(w + 0x24), ofs);
    copy3(dst, pp);
}
static inline void relT(u32 c, u32 actor, u32 ofs, u32 dst) {
    gabi::Local<cXyz> at;
    gabi::Local<cXyz> r;
    u32 a = gabi::ea(at.get()), rr = gabi::ea(r.get());
    attentionPos(c, a, actor);
    cXyz_pl(a, rr, ofs);
    copy3(dst, rr);
}
static inline void relO(u32 c, u32 actor, u32 ofs, u32 dst) {
    gabi::Local<cXyz> r;
    u32 rr = gabi::ea(r.get());
    relationalPos(c, rr, actor, ofs);
    copy3(dst, rr);
}

/* 02531010: fixedPositionEvCamera. work: +0 timer set, +1 bank set, +4 eye, +0x10 CtrGap,
 * +0x1C centre, +0x28 Fovy, +0x2C Bank, +0x30 CtrCus, +0x34 StartRadius, +0x38 Radius,
 * +0x3C RelActor, +0x40 Target, +0x44 its process id, +0x48 RelUseMask, +0x4C Timer */
static u32 dCamera_c_fixedPositionEvCamera(u32 i_this) {
    WWHD_FUNC(0x02531010, u32, i_this);
    u32 zero = localZero(0x104758A8, 0x1047586C);
    u32 w = i_this + EV_WORK;
    u32 ret = 1;
    if (ld(i_this + EV_TIMER) == 0) {
        gabi::Local<cXyz> def;
        gabi::Local<cXyz> eye;
        gabi::Local<cXyz> rel;
        u32 d = gabi::ea(def.get()), e = gabi::ea(eye.get()), r = gabi::ea(rel.get());
        f32 ez = ldf(i_this + 0x24), ex = ldf(i_this + 0x1C), ey = ldf(i_this + 0x20);
        stf(d + 8, ez);
        stf(d + 0, ex);
        stf(d + 4, ey);
        getEvXyzData(i_this, e, 0x1004CDF4 /* "Eye" */, d);
        f32 zx = ldf(zero + 0), zz = ldf(zero + 8), zy = ldf(zero + 4);
        stf(d + 8, zz);
        stf(d + 4, zy);
        stf(d + 0, zx);
        getEvXyzData(i_this, w + 0x10, 0x1004CE04 /* "CtrGap" */, d);
        getEvFloatData(i_this, w + 0x28, 0x1004CE0C /* "Fovy" */, ldf(i_this + 0x38));
        getEvFloatData(i_this, w + 0x30, 0x1004CE14 /* "CtrCus" */, 1.0f);
        getEvFloatData(i_this, w + 0x38, 0x1004CE1C /* "Radius" */, 100000.0f);
        getEvFloatData(i_this, w + 0x34, 0x1004CE34 /* "StartRadius" */, ldf(w + 0x38));
        st8(w + 1, (u8)getEvFloatData(i_this, w + 0x2C, 0x1004CE24 /* "Bank" */, 0.0f));
        getEvStringData(i_this, w + 0x48, 0x1004CE40 /* "RelUseMask" */, 0x1004CDF8 /* "o" */);
        st8(w + 0, (u8)getEvIntData(i_this, w + 0x4C, 0x1004CDFC /* "Timer" */, (s32)ld(0x101D61FC)));
        u32 a = getEvActor2(i_this, 0x1004CE2C /* "Target" */, 0x1004CDEC /* "@PLAYER" */);
        st(w + 0x40, a);
        if (a == 0) return 1;
        st(w + 0x44, a != 0 ? ld(a + 4) : 0xFFFFFFFF);
        u32 b = getEvActor(i_this, 0x1004CE4C /* "RelActor" */);
        st(w + 0x3C, b);
        if (b != 0 && ld8(w + 0x48) != '-') {
            relationalPos(i_this, r, b, e);
            copy3(w + 4, r);
        } else {
            copy3(w + 4, e);
        }
        copy3(w + 0x1C, i_this + 0x10);
        setDone(i_this);
    }
    gabi::Local<u32> id;
    u32 idp = gabi::ea(id.get());
    u32 pid = ld(w + 0x44);
    st(idp, pid);
    if (pid == 0xFFFFFFFF) return 1;
    if (fopAcIt_Judge_l(0x025E1234 /* fpcSch_JudgeByID */, idp) == 0) return 1;
    gabi::Local<cXyz> t;
    gabi::Local<cXyz> u;
    u32 tt = gabi::ea(t.get()), uu = gabi::ea(u.get());
    relationalPos(i_this, tt, ld(w + 0x40), w + 0x10);
    copy3(w + 0x1C, tt);
    cXyz_mi(w + 0x1C, uu, i_this + 0x44);
    cXyz_ml(uu, tt, ldf(w + 0x30));
    gabi::call(0x028E8D88, i_this + 0x44, tt, i_this + 0x44); /* PSVECAdd: centre += gap * CtrCus */
    copy3(i_this + 0x50, w + 4);
    cXyz_mi(i_this + 0x50, uu, i_this + 0x44);
    cSGlobe_Val(i_this + 0x3C, uu);
    f32 radius = ldf(w + 0x38);
    if (ld8(w + 0) != 0 && ld(i_this + EV_TIMER) < ld(w + 0x4C)) {
        f32 start = ldf(w + 0x34);
        f32 ratio = timerRatio(ld(i_this + EV_TIMER), (s32)ld(w + 0x4C));
        radius = gabi::fmadds(radius - start, ratio, start);
        ret = 0;
    }
    if (ldf(i_this + 0x3C) > radius) { /* ble: not taken on NaN */
        gabi::Local<cXyz> x;
        gabi::Local<cXyz> y;
        u32 xx = gabi::ea(x.get()), yy = gabi::ea(y.get());
        stf(i_this + 0x3C, radius);
        cSGlobe_Xyz(i_this + 0x3C, xx);
        cXyz_pl(i_this + 0x44, yy, xx);
        copy3(i_this + 0x50, yy);
    }
    stf(i_this + 0x60, ldf(w + 0x28));
    if (ld8(w + 1) != 0) {
        gabi::Local<s16> ang;
        u32 ap = gabi::ea(ang.get());
        u32 res = gabi::call<u32>(0x0200658C, ap, (s32)(s16)gabi::ftoi(ldf(w + 0x2C) * 182.04444885253906f));
        u32 fl = ld(i_this + EV_FLAGS);
        s16 bank = lds16(res);
        st(i_this + EV_FLAGS, fl | 0x400);
        st16(i_this + 0x5C, (u16)bank);
    }
    if (ret != 0) setDone(i_this);
    return ret;
}
VERIFY(0x02531010, dCamera_c_fixedPositionEvCamera);

/* 025314BC: fixedFrameEvCamera. work: +0 timer set, +4 eye, +0x10 centre, +0x1C Fovy, +0x20 Bank,
 * +0x24 RelActor, +0x28 RelUseMask (centre, eye), +0x2C Timer, +0x30 bank set, +0x34 BasePos */
static u32 dCamera_c_fixedFrameEvCamera(u32 i_this) {
    WWHD_FUNC(0x025314BC, u32, i_this);
    u32 w = i_this + EV_WORK;
    if (ld(i_this + EV_TIMER) == 0) {
        gabi::Local<cXyz> eye;
        gabi::Local<cXyz> def;
        gabi::Local<cXyz> ctr;
        u32 e = gabi::ea(eye.get()), d = gabi::ea(def.get()), cc = gabi::ea(ctr.get());
        f32 x = ldf(i_this + 0x1C), y = ldf(i_this + 0x20);
        stf(d + 0, x);
        f32 z = ldf(i_this + 0x24);
        stf(d + 4, y);
        stf(d + 8, z);
        getEvXyzData(i_this, e, 0x1004CE64 /* "Eye" */, d);
        x = ldf(i_this + 0x10);
        y = ldf(i_this + 0x14);
        stf(d + 0, x);
        z = ldf(i_this + 0x18);
        stf(d + 4, y);
        stf(d + 8, z);
        getEvXyzData(i_this, cc, 0x1004CE70 /* "Center" */, d);
        x = ldf(0x101FFBA8);
        y = ldf(0x101FFBAC);
        stf(d + 0, x);
        z = ldf(0x101FFBB0);
        stf(d + 4, y);
        stf(d + 8, z);
        getEvXyzData(i_this, w + 0x34, 0x1004CE5C /* "BasePos" */, d);
        getEvFloatData(i_this, w + 0x1C, 0x1004CE78 /* "Fovy" */, ldf(i_this + 0x38));
        st8(w + 0x30, (u8)getEvFloatData(i_this, w + 0x20, 0x1004CE80 /* "Bank" */, 0.0f));
        st8(w + 0, (u8)getEvIntData(i_this, w + 0x2C, 0x1004CE68 /* "Timer" */, -1));
        getEvStringData(i_this, w + 0x28, 0x1004CE8C /* "RelUseMask" */, 0x1004CE88 /* "oo" */);
        u32 a = getEvActor(i_this, 0x1004CE98 /* "RelActor" */);
        st(w + 0x24, a);
        if (a == 0) {
            copy3(w + 0x10, cc);
            copy3(w + 4, e);
        } else {
            u8 c0 = ld8(w + 0x28);
            if (c0 == 'o') relO(i_this, a, cc, w + 0x10);
            else if (c0 == 'n') relN(i_this, w, a, cc, w + 0x10);
            else if (c0 == 'p') relP(i_this, w, a, cc, w + 0x10);
            else if (c0 == 't') relT(i_this, a, cc, w + 0x10);
            else copy3(w + 0x10, cc);
            u8 c1 = ld8(w + 0x29);
            u32 act = ld(w + 0x24);
            if (c1 == 'o') {
                relO(i_this, act, e, w + 4);
            } else if (act != 0 && c1 == 'r') {
                if (ld(i_this + 0x80) & 1) negX(e);
                relO(i_this, ld(w + 0x24), e, w + 4);
                if (lineBGCheck(i_this, w + 0x10, w + 4, 0x8F)) negX(e);
                relO(i_this, ld(w + 0x24), e, w + 4);
            } else if (c1 == 'n') {
                relN(i_this, w, act, e, w + 4);
                if (lineBGCheck(i_this, w + 0x10, w + 4, 0x8F)) negX(e);
            } else if (c1 == 'p') {
                relP(i_this, w, act, e, w + 4);
            } else if (c1 == 't') {
                relT(i_this, act, e, w + 4);
            } else {
                copy3(w + 4, e);
            }
        }
        setDone(i_this);
    }
    setView(i_this, w + 0x10, w + 4);
    stf(i_this + 0x60, ldf(w + 0x1C));
    if (ld8(w + 0x30) != 0) setBank(i_this, ldf(w + 0x20));
    if (ld8(w + 0) != 0 && ld(i_this + EV_TIMER) < ld(w + 0x2C)) return 0;
    return 1;
}
VERIFY(0x025314BC, dCamera_c_fixedFrameEvCamera);

/* globe at an actor's attention position: g = cSGlobe(gap); g.V += actor.shape V; g.U += actor.shape U;
 * out = attentionPos(actor) + g.Xyz() */
static inline void stokerPos(u32 c, u32 g, u32 gap, u32 actorAt, u32 out) {
    gabi::Local<s16> sum;
    gabi::Local<s16> a1;
    gabi::Local<s16> a2;
    gabi::Local<cXyz> at;
    gabi::Local<cXyz> x;
    gabi::Local<cXyz> r;
    u32 sm = gabi::ea(sum.get()), aa = gabi::ea(at.get()), xx = gabi::ea(x.get()), rr = gabi::ea(r.get());
    cSGlobe_Val(g, gap);
    gabi::call(0x020068F4, g + 4, sm, (s32)lds16(ld(actorAt) + 0x328)); /* cSAngle::operator+(s16) */
    u32 res = gabi::call<u32>(0x0200658C, gabi::ea(a1.get()), (s32)lds16(sm));
    st16(g + 4, (u16)lds16(res));
    gabi::call(0x020068F4, g + 6, sm, (s32)lds16(ld(actorAt) + 0x32A));
    res = gabi::call<u32>(0x0200658C, gabi::ea(a2.get()), (s32)lds16(sm));
    st16(g + 6, (u16)lds16(res));
    attentionPos(c, aa, ld(actorAt));
    cSGlobe_Xyz(g, xx);
    cXyz_pl(aa, rr, xx);
    copy3(out, rr);
}

/* 0253B7E4: stokerEvCamera: centre and eye on globes around two actors (Stoker, Target).
 * work: +0 timer set, +1 bank set, +4 EyeGap, +0x10 CtrGap, +0x1C Fovy, +0x20 Bank,
 * +0x24 Stoker, +0x28 Target, +0x2C/+0x30 their process ids, +0x38 Timer */
static u32 dCamera_c_stokerEvCamera(u32 i_this) {
    WWHD_FUNC(0x0253B7E4, u32, i_this);
    u32 w = i_this + EV_WORK;
    if (ld(i_this + EV_TIMER) == 0) {
        gabi::Local<cXyz> def;
        u32 d = gabi::ea(def.get());
        f32 zx = ldf(0x101FFBA8), zy = ldf(0x101FFBAC);
        stf(d + 0, zx);
        f32 zz = ldf(0x101FFBB0);
        stf(d + 4, zy);
        stf(d + 8, zz);
        getEvXyzData(i_this, w + 4, 0x1004D5D4 /* "EyeGap" */, d);
        stf(d + 0, zx);
        stf(d + 4, zy);
        stf(d + 8, zz);
        getEvXyzData(i_this, w + 0x10, 0x1004D5DC /* "CtrGap" */, d);
        getEvFloatData(i_this, w + 0x1C, 0x1004D5E4 /* "Fovy" */, ldf(i_this + 0x38));
        st8(w + 1, (u8)getEvFloatData(i_this, w + 0x20, 0x1004D5EC /* "Bank" */, 0.0f));
        st8(w + 0, (u8)getEvIntData(i_this, w + 0x38, 0x1004D5CC /* "Timer" */, -1));
        st(w + 0x24, getEvActor2(i_this, 0x1004D5F4 /* "Stoker" */, 0x1004D604 /* "@STARTER" */));
        u32 b = getEvActor2(i_this, 0x1004D5FC /* "Target" */, 0x1004D5C4 /* "@PLAYER" */);
        u32 a = ld(w + 0x24);
        st(w + 0x28, b);
        if (a == 0 || b == 0) return 1;
        u32 bb = ld(w + 0x28);
        st(w + 0x2C, a != 0 ? ld(a + 4) : 0xFFFFFFFF);
        st(w + 0x30, bb != 0 ? ld(bb + 4) : 0xFFFFFFFF);
        setDone(i_this);
    }
    gabi::Local<u8[8]> globe;
    u32 g = gabi::ea(globe.get());
    gabi::call(0x02007100, g); /* cSGlobe() */
    if (ld(w + 0x28) != 0) {
        gabi::Local<u32> id;
        u32 idp = gabi::ea(id.get());
        u32 pid = ld(w + 0x30);
        st(idp, pid);
        if (pid == 0xFFFFFFFF) return 1;
        if (fopAcIt_Judge_l(0x025E1234, idp) == 0) return 1;
        stokerPos(i_this, g, w + 0x10, w + 0x28, i_this + 0x44);
    }
    if (ld(w + 0x24) != 0) {
        gabi::Local<u32> id;
        u32 idp = gabi::ea(id.get());
        u32 pid = ld(w + 0x2C);
        st(idp, pid);
        if (pid == 0xFFFFFFFF) return 1;
        if (fopAcIt_Judge_l(0x025E1234, idp) == 0) return 1;
        stokerPos(i_this, g, w + 4, w + 0x24, i_this + 0x50);
    }
    {
        gabi::Local<cXyz> dd;
        u32 dv = gabi::ea(dd.get());
        cXyz_mi(i_this + 0x50, dv, i_this + 0x44);
        cSGlobe_Val(i_this + 0x3C, dv);
    }
    stf(i_this + 0x60, ldf(w + 0x1C));
    if (ld8(w + 1) != 0) setBank(i_this, ldf(w + 0x20));
    if (ld8(w + 0) != 0 && ld(i_this + EV_TIMER) < ld(w + 0x38)) return 0;
    return 1;
}
VERIFY(0x0253B7E4, dCamera_c_stokerEvCamera);

} // namespace d_ev_camera_2_cpp

/* d_ev_camera, part 10: uniformTrans/uniformBrake/uniformAcceleEvCamera. See
 * d_ev_camera.cpp for the unit's layout notes; written from the WWHD code (the GameCube
 * d_ev_camera.cpp is all "Nonmatching" stubs). */
#include "bindings.h"

namespace d_ev_camera_10_cpp {

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
static inline void copy3f(u32 d, u32 s) {
    stf(d + 0, ldf(s + 0));
    stf(d + 4, ldf(s + 4));
    stf(d + 8, ldf(s + 8));
}
static inline u32 gi() { return gabi::call<u32>(0x025200D4); }
static inline u32 getEvIntData2(u32 c, u32 out, u32 name) { return gabi::call<u32>(0x02530418, c, out, name); }
static inline u32 getEvIntData(u32 c, u32 out, u32 name, s32 def) { return gabi::call<u32>(0x02530634, c, out, name, def); }
static inline u32 getEvFloatData(u32 c, u32 out, u32 name, f32 def) { return gabi::call<u32>(0x0253072C, c, out, name, def); }
static inline u32 getEvXyzData(u32 c, u32 out, u32 name, u32 def) { return gabi::call<u32>(0x0253086C, c, out, name, def); }
static inline u32 getEvStringData(u32 c, u32 out, u32 name, u32 def) { return gabi::call<u32>(0x02530998, c, out, name, def); }
static inline u32 getEvActor(u32 c, u32 name) { return gabi::call<u32>(0x02530B84, c, name); }
static inline void relationalPos(u32 c, u32 out, u32 actor, u32 ofs) { gabi::call(0x0250242C, c, out, actor, ofs); }
static inline void cXyz_mi(u32 a, u32 out, u32 b) { gabi::call(0x0201ADE0, a, out, b); }
static inline void cXyz_pl(u32 a, u32 out, u32 b) { gabi::call(0x0201AD78, a, out, b); }
static inline void cXyz_ml(u32 a, u32 out, f32 s) { gabi::call(0x0201AE48, a, out, s); }
static inline void PSVECAdd(u32 a, u32 b, u32 out) { gabi::call(0x028E8D88, a, b, out); }
static inline void cSGlobe_Val(u32 g, u32 v) { gabi::call(0x020072E0, g, v); }
static inline void cSGlobe_Xyz(u32 g, u32 out) { gabi::call(0x020073AC, g, out); }
static inline void cSGlobe_ct(u32 g, u32 v) { gabi::call(0x02007324, g, v); }
static inline void cSAngle_mi(u32 a, u32 out, u32 b) { gabi::call(0x020068B0, a, out, b); }
static inline void cSAngle_pl(u32 a, u32 out, u32 b) { gabi::call(0x02006894, a, out, b); }
static inline void cSAngle_ml(u32 a, u32 out, f32 f) { gabi::call(0x0200693C, a, out, f); }
static inline u32 cSAngleS(u32 tmp, s16 v) { return gabi::call<u32>(0x0200658C, tmp, (s32)v); }
static inline void positionOf(u32 c, u32 out, u32 actor) { gabi::call(0x024F8D5C, c, out, actor); }
static inline void directionOf(u32 c, u32 out, u32 actor) { gabi::call(0x024F8F30, c, out, actor); }
static inline void attentionPos(u32 c, u32 out, u32 actor) { gabi::call(0x024F8F3C, c, out, actor); }
static inline f32 vecLen(u32 v) { return gabi::call<f32>(0x028F4384, gabi::call<f32>(0x028E8DD0, v)); } /* sqrtf(PSVECSquareMag) */
static inline bool lineBGCheck(u32 c, u32 a, u32 b, u32 flags) { return gabi::call<bool>(0x024FCBE8, c, a, b, flags); }
static inline bool lineBGCheckCross(u32 c, u32 a, u32 b, u32 cross, u32 flags) { return gabi::call<bool>(0x024FB608, c, a, b, cross, flags); }
static inline void negX(u32 v) { stf(v, -ldf(v)); }
static inline void setDone(u32 c) {
    st8(c + 0x102, 1);
    st8(c + 0x101, 1);
    st8(c + 0x100, 1);
}
static inline void cxyzDef(u32 d, u32 src) {
    f32 x = ldf(src + 0), y = ldf(src + 4), z = ldf(src + 8);
    stf(d + 0, x);
    stf(d + 4, y);
    stf(d + 8, z);
}

/* the stage name check "Asoko": two inline string holders {const char*, vtable 1004CDBC} whose
 * empty virtual (slot 0x14) is called before the compare */
static inline bool stageIsAsoko() {
    gabi::Local<u32[2]> a;
    gabi::Local<u32[2]> b;
    u32 sa = gabi::ea(a.get()), sb = gabi::ea(b.get());
    st(sa + 4, 0x1004CDBC);
    st(sa + 0, 0x1004CED4); /* "Asoko" */
    u32 g = gi();
    st(sb + 4, 0x1004CDBC);
    st(sb + 0, g + 0x5134); /* the stage name */
    gabi::call_ptr(ld(ld(sa + 4) + 0x14), sa);
    gabi::call_ptr(ld(ld(sa + 4) + 0x14), sa);
    gabi::call_ptr(ld(ld(sb + 4) + 0x14), sb);
    u32 p = ld(sa), q = ld(sb);
    if (p == q) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 x = ld8(p + i), y = ld8(q + i);
        if (x != y) return false;
        if (x == 0) return true;
    }
    return false;
}
/* the event start: the inverted globe of this + 8 kept at work + 0x5C */
static inline void finishInit(u32 c, u32 w) {
    u32 g = gabi::call<u32>(0x02007400, c + 8); /* cSGlobe::Invert */
    st(w + 0x5C, ld(g + 0));
    st(w + 0x60, ld(g + 4));
    setDone(c);
}
/* 'r': the eye offset mirrored by the camera's side flag, again when the view is blocked */
static inline void rBlock(u32 c, u32 w, u32 ctr, u32 eye) {
    gabi::Local<cXyz> r;
    gabi::Local<cXyz> a;
    gabi::Local<cXyz> b;
    u32 rr = gabi::ea(r.get()), aa = gabi::ea(a.get()), bb = gabi::ea(b.get());
    relationalPos(c, rr, ld(w + 0x40), ctr);
    copy3(aa, rr);
    if (ld(c + 0x80) & 1) negX(eye);
    relationalPos(c, rr, ld(w + 0x40), eye);
    copy3(bb, rr);
    if (lineBGCheck(c, aa, bb, 0x8F)) negX(eye);
}
/* 'n': both offsets mirrored when the actor faces away from `from`; the eye again when blocked */
static inline void nBlock(u32 c, u32 w, u32 from, u32 mc, u32 ctr, u32 me, u32 eye) {
    gabi::Local<cXyz> pos;
    gabi::Local<cXyz> d;
    gabi::Local<u8[8]> globe;
    gabi::Local<s16[2]> ang;
    gabi::Local<cXyz> a;
    gabi::Local<cXyz> b;
    u32 pp = gabi::ea(pos.get()), dd = gabi::ea(d.get()), gg = gabi::ea(globe.get()), an = gabi::ea(ang.get());
    u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get());
    positionOf(c, pp, ld(w + 0x40));
    cXyz_mi(from, dd, pp);
    cSGlobe_ct(gg, dd);
    directionOf(c, an, ld(w + 0x40));
    cSAngle_mi(gg + 6, an + 2, an);
    if (lds16(an + 2) < lds16(0x101FF354)) { /* cSAngle::_0 */
        u8 e = ld8(me);
        if (ld8(mc) == 'n') negX(ctr);
        if (e == 'n') negX(eye);
    }
    relationalPos(c, pp, ld(w + 0x40), ctr);
    copy3(aa, pp);
    relationalPos(c, pp, ld(w + 0x40), eye);
    copy3(bb, pp);
    if (lineBGCheck(c, aa, bb, 0x8F)) negX(eye);
}
/* 'p': the offset mirrored when that puts the point farther from the player */
static inline void pBlock(u32 c, u32 w, u32 ofs) {
    gabi::Local<cXyz> o;
    gabi::Local<cXyz> t;
    gabi::Local<cXyz> p;
    gabi::Local<cXyz> d;
    u32 oo = gabi::ea(o.get()), tt = gabi::ea(t.get()), pp = gabi::ea(p.get()), dd = gabi::ea(d.get());
    copy3f(oo, ofs);
    relationalPos(c, tt, ld(w + 0x40), oo);
    positionOf(c, pp, ld(c + 0x128));
    cXyz_mi(tt, dd, pp);
    f32 d1 = vecLen(dd);
    negX(oo);
    relationalPos(c, pp, ld(w + 0x40), oo);
    copy3(tt, pp);
    positionOf(c, dd, ld(c + 0x128));
    cXyz_mi(tt, pp, dd);
    f32 d2 = vecLen(pp);
    if (d1 < d2) negX(ofs); /* bge: taken on NaN */
}
/* a point of the move by its RelUseMask character: 't' attention position + offset, 'c' globe
 * around the attention position turned by the start direction, '-' the plain offset (TransType 2:
 * turned by the actor's direction when `rot`), else relationalPos */
struct MoveLayout { u32 trans, cushion, bankSet, globeU; bool bankS16; };
static inline void relPoint(u32 c, u32 w, u8 m, u32 ofs, u32 dst, bool rot, const MoveLayout& L) {
    gabi::Local<cXyz> a;
    gabi::Local<cXyz> x;
    gabi::Local<cXyz> r;
    u32 aa = gabi::ea(a.get()), xx = gabi::ea(x.get()), rr = gabi::ea(r.get());
    if (m == 't') {
        attentionPos(c, aa, ld(w + 0x40));
        cXyz_pl(aa, rr, ofs);
        copy3(dst, rr);
    } else if (m == 'c') {
        gabi::Local<u8[8]> globe;
        gabi::Local<s16[2]> an;
        u32 gg = gabi::ea(globe.get()), ap = gabi::ea(an.get());
        cSGlobe_ct(gg, ofs);
        cSAngle_pl(w + L.globeU, ap, gg + 6);
        u32 res = cSAngleS(ap + 2, lds16(ap));
        st16(gg + 6, (u16)lds16(res));
        attentionPos(c, aa, ld(w + 0x40));
        cSGlobe_Xyz(gg, xx);
        cXyz_pl(aa, rr, xx);
        copy3(dst, rr);
    } else if (m == '-') {
        if (rot && ld(w + L.trans) == 2) {
            gabi::Local<s16> dir;
            u32 dp = gabi::ea(dir.get());
            directionOf(c, dp, ld(w + 0x40));
            gabi::call(0x024F7A40, rr, ofs, dp); /* dCamMath::xyzRotateY */
            copy3(dst, rr);
        } else {
            copy3(dst, ofs);
        }
    } else {
        relationalPos(c, rr, ld(w + 0x40), ofs);
        copy3(dst, rr);
    }
}
/* fovy towards Start/Fovy, cushioned */
static inline void blendFovy(u32 c, u32 w, f32 ratio, u32 cu) {
    f32 s = ldf(w + 0x18);
    f32 f = gabi::fmadds(ldf(w + 0x38) - s, ratio, s);
    f32 cur = ldf(c + 0x60);
    stf(c + 0x60, gabi::fmadds(f - cur, ldf(cu), cur));
}
/* pos += (target - pos) * Cushion */
static inline void cushion(u32 cu, u32 pos, u32 target, u32 t1, u32 t2) {
    cXyz_mi(target, t1, pos);
    cXyz_ml(t1, t2, ldf(cu));
    PSVECAdd(pos, t2, pos);
}

/* the move: start/end points by RelUseMask, the centre and eye towards them (TransType 1: eye on a
 * globe, 2: end points as deltas, else linear), cushioned; fovy and bank */
static inline void moveTail(u32 c, u32 w, f32 ratio, const MoveLayout& L) {
    gabi::Local<cXyz> sc;
    gabi::Local<cXyz> se;
    gabi::Local<cXyz> ec;
    gabi::Local<cXyz> ee;
    u32 SC = gabi::ea(sc.get()), SE = gabi::ea(se.get()), EC = gabi::ea(ec.get()), EE = gabi::ea(ee.get());
    if (ld(w + 0x40) == 0) {
        copy3(SC, w + 0xC);
        copy3(SE, w + 0);
        copy3(EC, w + 0x2C);
        copy3(EE, w + 0x20);
    } else {
        relPoint(c, w, ld8(w + 0x48), w + 0xC, SC, false, L);
        relPoint(c, w, ld8(w + 0x49), w + 0, SE, false, L);
        relPoint(c, w, ld8(w + 0x4A), w + 0x2C, EC, true, L);
        relPoint(c, w, ld8(w + 0x4B), w + 0x20, EE, true, L);
    }
    gabi::Local<cXyz> t1;
    gabi::Local<cXyz> t2;
    gabi::Local<cXyz> t3;
    gabi::Local<cXyz> tgt;
    u32 a1 = gabi::ea(t1.get()), a2 = gabi::ea(t2.get()), a3 = gabi::ea(t3.get()), tg = gabi::ea(tgt.get());
    u32 tt = ld(w + L.trans);
    if (tt == 1) {
        cXyz_mi(EC, a1, SC);
        cXyz_ml(a1, a2, ratio);
        cXyz_pl(SC, a3, a2);
        copy3(tg, a3);
        cushion(w + L.cushion, c + 0x44, tg, a2, a1);
        gabi::Local<u8[8]> g1;
        gabi::Local<u8[8]> g2;
        gabi::Local<u8[8]> g3;
        gabi::Local<s16[6]> an;
        u32 G1 = gabi::ea(g1.get()), G2 = gabi::ea(g2.get()), G3 = gabi::ea(g3.get()), ap = gabi::ea(an.get());
        cXyz_mi(SE, a1, SC);
        cSGlobe_ct(G1, a1);
        cXyz_mi(EE, a2, EC);
        cSGlobe_ct(G2, a2);
        cSAngle_mi(G2 + 4, ap + 0, G1 + 4);
        cSAngle_ml(ap + 0, ap + 2, ratio);
        cSAngle_pl(G1 + 4, ap + 4, ap + 2);
        cSAngle_mi(G2 + 6, ap + 6, G1 + 6);
        cSAngle_ml(ap + 6, ap + 8, ratio);
        cSAngle_pl(G1 + 6, ap + 10, ap + 8);
        f32 r1 = ldf(G1);
        gabi::call(0x02007248, G3, gabi::fmadds(ldf(G2) - r1, ratio, r1), ap + 4, ap + 10); /* cSGlobe(r, V, U) */
        cSGlobe_Xyz(G3, a3);
        cXyz_pl(c + 0x44, a2, a3);
        copy3(tg, a2);
        cushion(w + L.cushion, c + 0x50, tg, a1, a3);
    } else if (tt == 2) {
        cXyz_ml(EC, a1, ratio);
        cXyz_pl(SC, a2, a1);
        copy3(tg, a2);
        cushion(w + L.cushion, c + 0x44, tg, a2, a1);
        cXyz_ml(EE, a2, ratio);
        cXyz_pl(SE, a1, a2);
        copy3(tg, a1);
        cushion(w + L.cushion, c + 0x50, tg, a2, a1);
    } else {
        cXyz_mi(EC, a1, SC);
        cXyz_ml(a1, a2, ratio);
        cXyz_pl(SC, a3, a2);
        copy3(tg, a3);
        cushion(w + L.cushion, c + 0x44, tg, a2, a1);
        cXyz_mi(EE, a3, SE);
        cXyz_ml(a3, a2, ratio);
        cXyz_pl(SE, a1, a2);
        copy3(tg, a1);
        cushion(w + L.cushion, c + 0x50, tg, a2, a1);
    }
    blendFovy(c, w, ratio, w + L.cushion);
    if (ld8(w + L.bankSet) != 0) {
        gabi::Local<s16[4]> b;
        u32 bp = gabi::ea(b.get());
        f32 s = ldf(w + 0x1C);
        f32 bank = gabi::fmadds(ldf(w + 0x3C) - s, ratio, s);
        if (L.bankS16) {
            gabi::call(0x020065EC, bp + 2, (s32)(s16)gabi::ftoi(bank * 182.04444885253906f), c + 0x5C); /* s16 - cSAngle */
        } else {
            u32 ang = gabi::call<u32>(0x020066C0, bp + 0, bank); /* cSAngle(f32) */
            cSAngle_mi(ang, bp + 2, c + 0x5C);
        }
        cSAngle_ml(bp + 2, bp + 4, ldf(w + L.cushion));
        gabi::call(0x020068CC, c + 0x5C, bp + 4); /* cSAngle::operator+= */
        st(c + 0x510, ld(c + 0x510) | 0x400);
    }
    gabi::Local<cXyz> v;
    u32 vv = gabi::ea(v.get());
    cXyz_mi(c + 0x50, vv, c + 0x44);
    cSGlobe_Val(c + 0x3C, vv);
}
static const MoveLayout kTransLayout = {0x54, 0x58, 0x64, 0x62, false};
static const MoveLayout kBrakeLayout = {0x64, 0x6C, 0x78, 0x76, true};

/* 02531D20: uniformTransEvCamera: a move from StartCenter/StartEye/StartFovy/StartBank to
 * Center/Eye/Fovy/Bank over Timer frames (linear, or along the "BSpCurve" ratio curve), the points
 * relative to RelActor by RelUseMask (start centre, start eye, centre, eye); TransType 1 moves the
 * eye on a globe, 2 adds the end points as deltas. HD: under the TACT_WINDOW event (staff
 * "Asoko" check aside) the eye is placed behind the player's facing. work: +0 StartEye, +0xC
 * StartCenter, +0x18 StartFovy, +0x1C StartBank, +0x20 Eye, +0x2C Center, +0x38 Fovy, +0x3C Bank,
 * +0x40 RelActor, +0x44 its process id, +0x48 RelUseMask, +0x50 Timer, +0x54 TransType, +0x58
 * Cushion, +0x5C start globe, +0x64 bank set, +0x68 BSpCurve */
static u32 dCamera_c_uniformTransEvCamera(u32 i_this) {
    WWHD_FUNC(0x02531D20, u32, i_this);
    u32 w = i_this + 0x37C;
    if (ld(i_this + 0x11C) == 0) {
        if (getEvIntData2(i_this, w + 0x50, 0x1004CECC /* "Timer" */) == 0) return 1;
        getEvIntData(i_this, w + 0x68, 0x1004CF38 /* "BSpCurve" */, 1);
        if (ld(w + 0x68) != 0) gabi::call(0x025C0B18, i_this + 0x4CC, 4, ld(w + 0x50)); /* d2DBSplinePath::Init */
        gabi::Local<cXyz> def;
        u32 d = gabi::ea(def.get());
        cxyzDef(d, i_this + 0x1C);
        getEvXyzData(i_this, w + 0x20, 0x1004CEC8 /* "Eye" */, d);
        cxyzDef(d, i_this + 0x10);
        getEvXyzData(i_this, w + 0x2C, 0x1004CEDC /* "Center" */, d);
        if (ld(i_this + 0x400) == 0x104 && stageIsAsoko() && ldf(w + 0x28) < -1000.0f) { /* bge: taken on NaN */
            f32 ex = ldf(w + 0x20), cx = ldf(w + 0x2C);
            stf(w + 0x20, ex + -80.0f);
            stf(w + 0x2C, cx + -80.0f);
        }
        getEvFloatData(i_this, w + 0x38, 0x1004CEE4 /* "Fovy" */, ldf(i_this + 0x38));
        cxyzDef(d, i_this + 0x1C);
        getEvXyzData(i_this, w + 0, 0x1004CF44 /* "StartEye" */, d);
        cxyzDef(d, i_this + 0x10);
        getEvXyzData(i_this, w + 0xC, 0x1004CEFC /* "StartCenter" */, d);
        getEvFloatData(i_this, w + 0x18, 0x1004CF14 /* "StartFovy" */, ldf(i_this + 0x38));
        f32 deg = gabi::call<f32>(0x02006720, i_this + 0x34); /* cSAngle::Degree */
        st8(w + 0x64, (u8)getEvFloatData(i_this, w + 0x3C, 0x1004CEEC /* "Bank" */, deg));
        deg = gabi::call<f32>(0x02006720, i_this + 0x34);
        u32 r = getEvFloatData(i_this, w + 0x1C, 0x1004CF20 /* "StartBank" */, deg);
        st8(w + 0x64, (u8)(ld8(w + 0x64) | r));
        getEvIntData(i_this, w + 0x54, 0x1004CF2C /* "TransType" */, 0);
        getEvStringData(i_this, w + 0x48, 0x1004CF50 /* "RelUseMask" */, 0x1004CEF4 /* "--oo" */);
        st(w + 0x40, getEvActor(i_this, 0x1004CF5C /* "RelActor" */));
        getEvFloatData(i_this, w + 0x58, 0x1004CEC0 /* "Cushion" */, 1.0f);
        u32 a = ld(w + 0x40);
        if (a != 0) {
            st(w + 0x44, a != 0 ? ld(a + 4) : 0xFFFFFFFF);
            if (ld8(w + 0x49) == 'r') rBlock(i_this, w, w + 0xC, w + 0);
            if (ld8(w + 0x48) == 'n' || ld8(w + 0x49) == 'n') nBlock(i_this, w, i_this + 0x1C, w + 0x48, w + 0xC, w + 0x49, w + 0);
            if (ld8(w + 0x4A) == 'n' || ld8(w + 0x4B) == 'n') nBlock(i_this, w, w + 0x20, w + 0x4A, w + 0x2C, w + 0x4B, w + 0x20);
            if (ld8(w + 0x4A) == 'p') pBlock(i_this, w, w + 0x2C);
            u8 m = ld8(w + 0x4B);
            if (m == 'p') pBlock(i_this, w, w + 0x20);
            else if (m == 'r') rBlock(i_this, w, w + 0x2C, w + 0x20);
        }
        finishInit(i_this, w);
    }
    /* HD: the tact window event: the eye goes behind the player at 800 / 100 */
    s32 idx = gabi::call<s32>(0x02543F10, gi() + 0x52C4, 0x1004CF08 /* "TACT_WINDOW" */, 0xFF); /* getEventIdx */
    u32 ev = gabi::call<u32>(0x02544044, gi() + 0x52C4, (s32)(s16)idx); /* getEventData */
    if (ev != 0 && ld(ev + 0xA4) == 2 && (ld(gi() + 0x5CD8) & 0x10000)) {
        u32 pl = ld(gi() + 0x5B34);
        if (pl != 0) {
            u32 o = ((u32)(u16)(lds16(pl + 0x32A) + -0x8000) >> 3) << 3;
            f32 s = ldf(0x104A44F8 + o), co = ldf(0x104A44F8 + o + 4);
            stf(w + 0x24, 100.0f);
            stf(w + 0x20, 800.0f * s);
            stf(w + 0x28, 800.0f * co);
            gabi::Local<s16> nm;
            u32 np = gabi::ea(nm.get());
            st16(np, 0xA5);
            u32 act = gabi::call<u32>(0x025D5218, 0x025E121C /* fpcSch_JudgeForPName */, np); /* fopAcIt_Judge */
            gabi::Local<cXyz> base;
            gabi::Local<cXyz> p1;
            gabi::Local<cXyz> p2;
            gabi::Local<cXyz> cr;
            gabi::Local<cXyz> tg;
            gabi::Local<cXyz> tmp;
            u32 bp = gabi::ea(base.get()), a1 = gabi::ea(p1.get()), a2 = gabi::ea(p2.get()), cp = gabi::ea(cr.get());
            u32 tp = gabi::ea(tg.get()), mp = gabi::ea(tmp.get());
            f32 bx = ldf(pl + 0x314);
            f32 by = ldf(act + 0x6D8) + 120.0f;
            f32 bz = ldf(pl + 0x31C);
            stf(bp + 0, bx);
            stf(bp + 8, bz);
            stf(bp + 4, by);
            cXyz_pl(bp, a1, w + 0x2C);
            cXyz_pl(bp, a2, w + 0x20);
            if (lineBGCheckCross(i_this, a1, a2, cp, 0x8F)) {
                cXyz_mi(cp, mp, bp);
                copy3(tp, mp);
            } else {
                copy3(tp, w + 0x20);
            }
            gabi::call(0x0200EE00, w + 0x20, tp, 5.0f, 20.0f, 1.0f); /* cLib_addCalcPos */
            cXyz_pl(bp, a2, w + 0x20);
            if (lineBGCheckCross(i_this, w + 0, a2, cp, 0x8F)) {
                cXyz_mi(cp, mp, bp);
                copy3(w + 0x20, mp);
            }
            finishInit(i_this, w);
        }
    }
    if (ld(w + 0x40) != 0) {
        gabi::Local<u32> id;
        u32 ip = gabi::ea(id.get());
        u32 pid = ld(w + 0x44);
        st(ip, pid);
        if (pid == 0xFFFFFFFF) return 1;
        if (gabi::call<u32>(0x025D5218, 0x025E1234 /* fpcSch_JudgeByID */, ip) == 0) return 1;
    }
    u32 ret = 0;
    f32 ratio = 1.0f;
    u32 t = ld(i_this + 0x11C);
    u32 n = ld(w + 0x50);
    if (t >= n) {
        ret = 1;
    } else if (ld(w + 0x68) != 0) {
        gabi::call(0x025C0C80, i_this + 0x4CC); /* d2DBSplinePath::Step */
        ratio = gabi::call<f32>(0x025C0F30, i_this + 0x4CC, 0x101D626C); /* Calc(f32*) */
    } else {
        ratio = (f32)(f64)(t + 1) / (f32)(f64)(s32)n;
    }
    moveTail(i_this, w, ratio, kTransLayout);
    return ret;
}
VERIFY(0x02531D20, dCamera_c_uniformTransEvCamera);

/* PPC slw: shift counts 32..63 give 0 */
static inline u32 slw(u32 v, u32 sh) {
    sh &= 63;
    return sh >= 32 ? 0 : v << sh;
}
/* the sum of the per-frame steps of a braking/accelerating move: k frames at full step n1 plus the
 * ramp n1..1 (linear) or 2^(n1-1)..1 (type 1) */
static inline void brakeTotal(u32 w) {
    s32 n1 = (s32)ld(w + 0x5C);
    if (ld(w + 0x60) != 1) {
        f32 a = (f32)(f64)(s32)((u32)ld(w + 0x58) * (u32)n1);
        f32 b = (f32)(f64)(s32)((s32)((u32)n1 * (u32)(n1 + 1)) >> 1);
        stf(w + 0x54, a);
        stf(w + 0x54, a + b);
    } else {
        u32 v = slw(1, (u32)n1 - 1);
        f32 p = (f32)(f64)(s32)v;
        f32 k = (f32)(f64)(s32)ld(w + 0x58);
        f32 r = (p + p) - 1.0f;
        f32 m = k * p;
        stf(w + 0x54, m + r);
    }
}
/* the start/end parameters of the braking/accelerating moves (names at `nm`: Eye, Center, Fovy,
 * StartEye, StartCenter, StartFovy, Bank, StartBank, TransType, RelUseMask, its default, RelActor,
 * Cushion) and the RelUseMask adjustments */
static inline void brakeParams(u32 c, u32 w, const u32* nm) {
    gabi::Local<cXyz> def;
    u32 d = gabi::ea(def.get());
    cxyzDef(d, c + 0x1C);
    getEvXyzData(c, w + 0x20, nm[0], d);
    cxyzDef(d, c + 0x10);
    getEvXyzData(c, w + 0x2C, nm[1], d);
    getEvFloatData(c, w + 0x38, nm[2], ldf(c + 0x38));
    cxyzDef(d, c + 0x1C);
    getEvXyzData(c, w + 0, nm[3], d);
    cxyzDef(d, c + 0x10);
    getEvXyzData(c, w + 0xC, nm[4], d);
    getEvFloatData(c, w + 0x18, nm[5], ldf(c + 0x38));
    f32 deg = gabi::call<f32>(0x02006720, c + 0x34); /* cSAngle::Degree */
    st8(w + 0x78, (u8)getEvFloatData(c, w + 0x3C, nm[6], deg));
    deg = gabi::call<f32>(0x02006720, c + 0x34);
    u32 r = getEvFloatData(c, w + 0x1C, nm[7], deg);
    st8(w + 0x78, (u8)(ld8(w + 0x78) | r));
    getEvIntData(c, w + 0x64, nm[8], 0);
    getEvStringData(c, w + 0x48, nm[9], nm[10]);
    st(w + 0x40, getEvActor(c, nm[11]));
    getEvFloatData(c, w + 0x6C, nm[12], 1.0f);
    u32 a = ld(w + 0x40);
    if (a != 0) {
        st(w + 0x44, a != 0 ? ld(a + 4) : 0xFFFFFFFF);
        if (ld8(w + 0x49) == 'r') rBlock(c, w, w + 0xC, w + 0);
        if (ld8(w + 0x48) == 'n' || ld8(w + 0x49) == 'n') nBlock(c, w, c + 0x1C, w + 0x48, w + 0xC, w + 0x49, w + 0);
        if (ld8(w + 0x4A) == 'n' || ld8(w + 0x4B) == 'n') nBlock(c, w, w + 0x20, w + 0x4A, w + 0x2C, w + 0x4B, w + 0x20);
        if (ld8(w + 0x4A) == 'p') pBlock(c, w, w + 0x2C);
        u8 m = ld8(w + 0x4B);
        if (m == 'p') pBlock(c, w, w + 0x20);
        else if (m == 'r') rBlock(c, w, w + 0x2C, w + 0x20);
    }
    u32 g = gabi::call<u32>(0x02007400, c + 8); /* cSGlobe::Invert */
    st(w + 0x70, ld(g + 0));
    stf(w + 0x68, 0.0f);
    st(w + 0x74, ld(g + 4));
    setDone(c);
}
/* the RelActor must still exist */
static inline bool relActorGone(u32 w) {
    if (ld(w + 0x40) == 0) return false;
    gabi::Local<u32> id;
    u32 ip = gabi::ea(id.get());
    u32 pid = ld(w + 0x44);
    st(ip, pid);
    if (pid == 0xFFFFFFFF) return true;
    return gabi::call<u32>(0x025D5218, 0x025E1234 /* fpcSch_JudgeByID */, ip) == 0;
}
static inline f32 brakeStep(u32 w, f32 f) {
    f32 acc = ldf(w + 0x68) + f;
    stf(w + 0x68, acc);
    return acc / ldf(w + 0x54);
}

static const u32 kBrakeNames[13] = {0x1004D618, 0x1004D624, 0x1004D62C, 0x1004D690, 0x1004D644, 0x1004D65C, 0x1004D634,
                                    0x1004D668, 0x1004D674, 0x1004D69C, 0x1004D63C, 0x1004D6A8, 0x1004D610};
static const u32 kAcceleNames[13] = {0x1004D6BC, 0x1004D6C8, 0x1004D6D0, 0x1004D730, 0x1004D6F4, 0x1004D700, 0x1004D6D8,
                                     0x1004D70C, 0x1004D718, 0x1004D73C, 0x1004D6E0, 0x1004D748, 0x1004D6B4};

/* 0253BC1C: uniformBrakeEvCamera: as uniformTrans, at full speed until "BrakingPoint", then braking
 * (BrakeType 0: linearly, 1: halving the step each frame). work: as uniformTrans up to +0x4C, +0x50
 * Timer, +0x54 total of the steps, +0x58 BrakingPoint, +0x5C Timer - BrakingPoint, +0x60 BrakeType,
 * +0x64 TransType, +0x68 steps so far, +0x6C Cushion, +0x70 start globe, +0x78 bank set */
static u32 dCamera_c_uniformBrakeEvCamera(u32 i_this) {
    WWHD_FUNC(0x0253BC1C, u32, i_this);
    u32 w = i_this + 0x37C;
    if (ld(i_this + 0x11C) == 0) {
        if (getEvIntData2(i_this, w + 0x50, 0x1004D61C /* "Timer" */) == 0) return 1;
        getEvIntData(i_this, w + 0x58, 0x1004D680 /* "BrakingPoint" */, 0);
        u32 bp = ld(w + 0x58);
        st(w + 0x5C, ld(w + 0x50) - bp);
        getEvIntData(i_this, w + 0x60, 0x1004D650 /* "BrakeType" */, 0);
        brakeTotal(w);
        brakeParams(i_this, w, kBrakeNames);
    }
    if (relActorGone(w)) return 1;
    u32 ret = 0;
    f32 ratio = 1.0f;
    u32 t = ld(i_this + 0x11C);
    u32 n = ld(w + 0x50);
    if (t >= n) {
        ret = 1;
    } else if (ld(w + 0x60) != 1) {
        if (t < ld(w + 0x58)) ratio = brakeStep(w, (f32)(f64)(s32)ld(w + 0x5C));
        else ratio = brakeStep(w, (f32)(f64)(n - t));
    } else {
        u32 sh = t < ld(w + 0x58) ? ld(w + 0x5C) - 1 : n - t - 1;
        ratio = brakeStep(w, (f32)(f64)(s32)slw(1, sh));
    }
    moveTail(i_this, w, ratio, kBrakeLayout);
    return ret;
}
VERIFY(0x0253BC1C, dCamera_c_uniformBrakeEvCamera);

/* 0253D1A4: uniformAcceleEvCamera: as uniformBrake, accelerating over the first "AcceleTimer" frames
 * (AcceleType 0: linearly, 1: doubling the step each frame), then at full speed. work: as
 * uniformBrake with +0x58 Timer - AcceleTimer, +0x5C AcceleTimer, +0x60 AcceleType */
static u32 dCamera_c_uniformAcceleEvCamera(u32 i_this) {
    WWHD_FUNC(0x0253D1A4, u32, i_this);
    u32 w = i_this + 0x37C;
    if (ld(i_this + 0x11C) == 0) {
        if (getEvIntData2(i_this, w + 0x50, 0x1004D6C0 /* "Timer" */) == 0) return 1;
        getEvIntData(i_this, w + 0x5C, 0x1004D6E8 /* "AcceleTimer" */, (s32)ld(w + 0x50));
        getEvIntData(i_this, w + 0x60, 0x1004D724 /* "AcceleType" */, 0);
        st(w + 0x58, ld(w + 0x50) - ld(w + 0x5C));
        brakeTotal(w);
        brakeParams(i_this, w, kAcceleNames);
    }
    if (relActorGone(w)) return 1;
    u32 ret = 0;
    f32 ratio = 1.0f;
    u32 t = ld(i_this + 0x11C);
    u32 n = ld(w + 0x50);
    if (t >= n) {
        ret = 1;
    } else if (ld(w + 0x60) != 1) {
        u32 at = ld(w + 0x5C);
        if (t < at) ratio = brakeStep(w, (f32)(f64)t);
        else ratio = brakeStep(w, (f32)(f64)(s32)at);
    } else {
        u32 at = ld(w + 0x5C);
        u32 sh = t < at ? t - 1 : at - 1;
        ratio = brakeStep(w, (f32)(f64)(s32)slw(1, sh));
    }
    moveTail(i_this, w, ratio, kBrakeLayout);
    return ret;
}
VERIFY(0x0253D1A4, dCamera_c_uniformAcceleEvCamera);

} // namespace d_ev_camera_10_cpp

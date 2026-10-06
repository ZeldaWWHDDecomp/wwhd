/* d_ev_camera, part 6: tornadoWarpEvCamera. See
 * d_ev_camera.cpp for the unit's layout notes; written from the WWHD code (the GameCube
 * d_ev_camera.cpp is all "Nonmatching" stubs). */
#include "bindings.h"

namespace d_ev_camera_6_cpp {

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
static inline void f3(u32 v, f32 x, f32 y, f32 z) {
    stf(v + 0, x);
    stf(v + 4, y);
    stf(v + 8, z);
}
static inline u32 gi() { return gabi::call<u32>(0x025200D4); }
static inline void relationalPos(u32 c, u32 out, u32 actor, u32 ofs) { gabi::call(0x0250242C, c, out, actor, ofs); }
static inline void relationalPosA(u32 c, u32 out, u32 actor, u32 ofs, u32 ang) { gabi::call(0x02514D00, c, out, actor, ofs, ang); }
static inline void attentionPos(u32 c, u32 out, u32 actor) { gabi::call(0x024F8F3C, c, out, actor); }
static inline bool lineBGCheck(u32 c, u32 a, u32 b, u32 flags) { return gabi::call<bool>(0x024FCBE8, c, a, b, flags); }
static inline void cXyz_mi(u32 a, u32 out, u32 b) { gabi::call(0x0201ADE0, a, out, b); }
static inline void cXyz_pl(u32 a, u32 out, u32 b) { gabi::call(0x0201AD78, a, out, b); }
static inline void cXyz_ml(u32 a, u32 out, f32 s) { gabi::call(0x0201AE48, a, out, s); }
static inline void vecAdd(u32 a, u32 b, u32 out) { gabi::call(0x028E8D88, a, b, out); } /* PSVECAdd */
static inline void cSGlobe_Val(u32 g, u32 v) { gabi::call(0x020072E0, g, v); }
static inline f32 vecLen(u32 v) { return gabi::call<f32>(0x028F4384, gabi::call<f32>(0x028E8DD0, v)); }
static inline u32 cSAngleCopy(u32 out, u32 src) { return gabi::call<u32>(0x02006644, out, src); }
static inline void setDone(u32 c) {
    st8(c + 0x102, 1);
    st8(c + 0x101, 1);
    st8(c + 0x100, 1);
}

/* 025383D0: tornadoWarpEvCamera (warping with the Ballad of Gales). work: +0 state (0: choose
 * the eye, 1: 100 frames of turning with a random bank, 2: 200 frames easing out, 3: done),
 * +4 frames left, +8 eye, +0x14 the item actor ('PNAME 0xA5') */
static u32 dCamera_c_tornadoWarpEvCamera(u32 i_this) {
    WWHD_FUNC(0x025383D0, u32, i_this);
    u32 w = i_this + 0x37C;
    if (ld(i_this + 0x11C) == 0) {
        st(w + 4, 0);
        st(w + 0, 0);
        setDone(i_this);
    }
    f32 c0 = ldf(0x1004CE58), c800 = ldf(0x1004CEB0), c900 = ldf(0x1004D264), cm900 = ldf(0x1004D268);
    gabi::Local<cXyz[4]> tab;
    gabi::Local<cXyz> ctrOfs;
    gabi::Local<cXyz> endOfs;
    u32 tb = gabi::ea(tab.get()), co = gabi::ea(ctrOfs.get()), eo = gabi::ea(endOfs.get());
    f3(tb + 0x00, c900, c800, c0);
    f3(tb + 0x0C, cm900, c800, c0);
    f3(tb + 0x18, c0, c800, c900);
    f3(tb + 0x24, c0, c800, cm900);
    f3(co, c0, ldf(0x1004D0DC), c0);  /* (0, -40, 0) */
    f3(eo, c0, ldf(0x1004D004), c0);  /* (0, 60, 0) */
    gabi::Local<s16> ang45;
    u32 a45 = gabi::ea(ang45.get());
    gabi::call(0x020066C0, a45, ldf(0x1004D008)); /* cSAngle(45.0) */
    u32 state = ld(w + 0);
    f32 quarter = ldf(0x1004D26C); /* 0.25 */
    if (state == 3) return 1;
    gabi::Local<cXyz> e;
    gabi::Local<cXyz> t1;
    gabi::Local<cXyz> t2;
    u32 ee = gabi::ea(e.get()), tt1 = gabi::ea(t1.get()), tt2 = gabi::ea(t2.get());
    if (state != 1 && state != 2) {
        gabi::Local<u8[8]> nm; /* the name at an address = 2 mod 4, as the original's sp+0x32 */
        u32 np = gabi::ea(nm.get()) + 2;
        st16(np, 0xA5);
        st(w + 0x14, gabi::call<u32>(0x025D5218, 0x025E121C, np)); /* fopAcIt_Judge(fpcSch_JudgeForPName) */
        st(w + 4, 100);
        st(w + 0, 1);
        gabi::Local<cXyz> far;
        gabi::Local<cXyz> ctr;
        u32 fa = gabi::ea(far.get()), cc = gabi::ea(ctr.get());
        u32 pl = ld(i_this + 0x128);
        f3(fa, ldf(0x1004D270), ldf(0x1004D274), ldf(0x1004D278)); /* (-180000, 750, -200000) */
        relationalPos(i_this, tt1, pl, co);
        copy3(cc, tt1);
        if (ld8(i_this + 0x8DA) != 0) {
            /* the table entry whose eye is closest to (-180000, 750, -200000) */
            s32 best = 3;
            f32 bestD = ldf(0x1004D27C); /* 1e8 */
            gabi::Local<s16> ac;
            u32 acp = gabi::ea(ac.get());
            for (s32 k = 0; k < 4; k++) {
                u32 ap = cSAngleCopy(acp, a45);
                relationalPosA(i_this, tt1, ld(i_this + 0x128), tb + (u32)k * 12, ap);
                copy3(ee, tt1);
                cXyz_mi(ee, tt1, fa);
                f32 d = vecLen(tt1);
                if (d < bestD) { /* bge: taken on NaN */
                    bestD = d;
                    best = k;
                }
            }
            gabi::Local<s16> ac2;
            gabi::Local<cXyz> rot;
            u32 ap = cSAngleCopy(gabi::ea(ac2.get()), a45);
            u32 rp = gabi::ea(rot.get());
            gabi::call(0x024F7A40, rp, tb + (u32)best * 12, ap); /* dCamMath::xyzRotateY */
            relationalPos(i_this, tt2, ld(i_this + 0x128), rp);
            f32 x = ldf(tt2 + 0), y = ldf(tt2 + 4), z = ldf(tt2 + 8);
            f3(ee, x, y, z);
            stf(w + 0xC, y);
            stf(w + 0x10, z);
            stf(w + 8, x);
        } else {
            gabi::Local<s16> ac;
            gabi::Local<cXyz> p1;
            gabi::Local<cXyz> p2;
            u32 acp = gabi::ea(ac.get()), pp1 = gabi::ea(p1.get()), pp2 = gabi::ea(p2.get());
            f32 r15 = ldf(0x1004CF80);
            for (u32 k = 0; k < 4; k++) {
                u32 ap = cSAngleCopy(acp, a45);
                relationalPosA(i_this, tt1, ld(i_this + 0x128), tb + k * 12, ap);
                copy3(ee, tt1);
                if (lineBGCheck(i_this, cc, ee, 0x7F)) continue;
                for (u32 i = 0; i < 3; i++) stf(pp1 + 4 * i, ldf(cc + 4 * i));
                for (u32 i = 0; i < 3; i++) stf(pp2 + 4 * i, ldf(ee + 4 * i));
                u32 pl2 = ld(i_this + 0x128);
                u32 item = ld(w + 0x14);
                u32 g = gi();
                if (!gabi::call<bool>(0x025187D8, g + 0x26A4, pp1, pp2, r15, pl2, item)) break; /* dCcS::ChkCamera */
            }
            f32 z = ldf(ee + 8), x = ldf(ee + 0), y = ldf(ee + 4);
            stf(w + 8, x);
            stf(w + 0xC, y);
            stf(w + 0x10, z);
        }
        state = 1;
    }
    if (state == 1) {
        relationalPos(i_this, ee, ld(i_this + 0x128), co);
        cXyz_mi(ee, tt1, i_this + 0x44);
        cXyz_ml(tt1, tt2, quarter);
        vecAdd(i_this + 0x44, tt2, i_this + 0x44);
        f32 inv = ldf(0x1004CEA4) / (f32)(f64)(s32)ld(w + 4);
        cXyz_mi(w + 8, tt2, i_this + 0x50);
        cXyz_ml(tt2, tt1, inv);
        cXyz_pl(i_this + 0x50, ee, tt1);
        gabi::Local<cXyz> tgt;
        u32 tg = gabi::ea(tgt.get());
        copy3(tg, ee);
        cXyz_mi(tg, tt1, i_this + 0x50);
        f32 k15 = ldf(0x1004D280); /* 0.15 */
        cXyz_ml(tt1, ee, k15);
        vecAdd(i_this + 0x50, ee, i_this + 0x50);
        f32 fv = ldf(i_this + 0x60);
        f32 f1 = gabi::fmadds(ldf(0x1004CF70) - fv, inv, fv); /* towards 70 */
        stf(i_this + 0x60, gabi::fmadds(f1 - fv, k15, fv));
        cXyz_mi(i_this + 0x50, ee, i_this + 0x44);
        cSGlobe_Val(i_this + 0x3C, ee);
        f32 rnd = gabi::call<f32>(0x02019918, ldf(0x1004D284) * inv); /* cM_rndFX(6 * inv) */
        gabi::Local<s16> d1;
        gabi::Local<s16> d2;
        u32 dd1 = gabi::ea(d1.get()), dd2 = gabi::ea(d2.get());
        gabi::call(0x020065EC, dd1, (s32)(s16)gabi::ftoi(rnd * ldf(0x1004CDE8)), i_this + 0x5C); /* s16 - cSAngle */
        gabi::call(0x0200693C, dd1, dd2, k15);       /* cSAngle::operator*(f32) */
        gabi::call(0x020068CC, i_this + 0x5C, dd2);  /* cSAngle::operator+= */
        st(i_this + 0x510, ld(i_this + 0x510) | 0x400);
        u32 n = ld(w + 4) - 1;
        st(w + 4, n);
        if (n != 0) return 1;
        st(w + 0, 2);
        st(w + 4, 200);
    }
    /* state 2 */
    relationalPos(i_this, ee, ld(i_this + 0x128), eo);
    cXyz_mi(ee, tt1, i_this + 0x44);
    cXyz_ml(tt1, tt2, quarter);
    vecAdd(i_this + 0x44, tt2, i_this + 0x44);
    gabi::Local<cXyz> tgt;
    u32 tg = gabi::ea(tgt.get());
    copy3(tg, w + 8);
    attentionPos(i_this, ee, ld(i_this + 0x128));
    stf(tg + 4, ldf(ee + 4));
    cXyz_mi(tg, tt1, i_this + 0x50);
    f32 k05 = ldf(0x1004D1E4); /* 0.05 */
    cXyz_ml(tt1, ee, k05);
    vecAdd(i_this + 0x50, ee, i_this + 0x50);
    f32 fv = ldf(i_this + 0x60);
    stf(i_this + 0x60, gabi::fmadds(ldf(0x1004D1D0) - fv, ldf(0x1004D1E4), fv)); /* towards 90 */
    cXyz_mi(i_this + 0x50, ee, i_this + 0x44);
    cSGlobe_Val(i_this + 0x3C, ee);
    gabi::Local<s16> b;
    u32 bp = gabi::ea(b.get());
    gabi::call(0x0200693C, i_this + 0x5C, bp, ldf(0x1004D1D8)); /* bank * 0.02 */
    gabi::call(0x020068E0, i_this + 0x5C, bp);                  /* cSAngle::operator-= */
    st(i_this + 0x510, ld(i_this + 0x510) | 0x400);
    u32 n = ld(w + 4) - 1;
    st(w + 4, n);
    if (n == 0) st(w + 0, 3);
    return 1;
}
VERIFY(0x025383D0, dCamera_c_tornadoWarpEvCamera);

static inline u32 getEvFloatData(u32 c, u32 out, u32 name, f32 def) { return gabi::call<u32>(0x0253072C, c, out, name, def); }
static inline u32 getEvXyzData(u32 c, u32 out, u32 name, u32 def) { return gabi::call<u32>(0x0253086C, c, out, name, def); }
static inline u32 getEvActor2(u32 c, u32 name, u32 def) { return gabi::call<u32>(0x02530CB4, c, name, def); }
static inline f32 degree(u32 a) { return gabi::call<f32>(0x02006720, a); } /* cSAngle::Degree */
static inline void cSGlobe_Xyz(u32 g, u32 out) { gabi::call(0x020073AC, g, out); }

/* 0253B168: twoActor0EvCamera: frame two actors: the centre on the pair (CtrRatio in 0..1:
 * between their attention positions, else relational with CtrGap), radius/latitude/longitude
 * eased (CtrCus/EyeCus) and clamped to [RadiusMin, RadiusMax], [LatitudeMin, LatitudeMax],
 * [LongitudeMin, LongitudeMax] (longitude relative to the pair's direction).
 * work: +0/+4 actors, +8/+0xC their process ids, +0x10 CtrCus, +0x14 EyeCus, +0x18 RadiusMin,
 * +0x1C RadiusMax, +0x20/+0x24 latitude range, +0x28/+0x2C longitude range, +0x30 Fovy,
 * +0x34 CtrRatio, +0x38 CtrGap, +0x44 radius, +0x48 latitude, +0x4C longitude (degrees) */
static u32 dCamera_c_twoActor0EvCamera(u32 i_this) {
    WWHD_FUNC(0x0253B168, u32, i_this);
    f32 zero = ldf(0x1004CE58);
    u32 z = 0x1047589C;
    if (ld(0x104758B8) == 0) {
        stf(z + 0, zero);
        stf(z + 8, zero);
        st(0x104758B8, 1);
        stf(z + 4, zero);
    }
    u32 w = i_this + 0x37C;
    if (ld(i_this + 0x11C) == 0) {
        st(w + 0, getEvActor2(i_this, 0x1004D52C /* "Actor1" */, 0x1004D524 /* "@PLAYER" */));
        u32 b = getEvActor2(i_this, 0x1004D534 /* "Actor2" */, 0x1004D58C /* "@STARTER" */);
        u32 a = ld(w + 0);
        st(w + 4, b);
        if (a == 0 || b == 0) return 1;
        u32 bb = ld(w + 4);
        st(w + 8, a != 0 ? ld(a + 4) : 0xFFFFFFFF);
        st(w + 0xC, bb != 0 ? ld(bb + 4) : 0xFFFFFFFF);
        gabi::Local<cXyz> def;
        u32 d = gabi::ea(def.get());
        f32 y = ldf(z + 4), zz = ldf(z + 8), x = ldf(z + 0);
        stf(d + 4, y);
        stf(d + 0, x);
        stf(d + 8, zz);
        getEvXyzData(i_this, w + 0x38, 0x1004D53C /* "CtrGap" */, d);
        getEvFloatData(i_this, w + 0x34, 0x1004D598 /* "CtrRatio" */, ldf(0x101D6268));
        getEvFloatData(i_this, w + 0x10, 0x1004D544 /* "CtrCus" */, ldf(0x101D6244));
        getEvFloatData(i_this, w + 0x14, 0x1004D54C /* "EyeCus" */, ldf(0x101D6248));
        getEvFloatData(i_this, w + 0x18, 0x1004D574 /* "RadiusMin" */, ldf(0x101D6250));
        getEvFloatData(i_this, w + 0x1C, 0x1004D580 /* "RadiusMax" */, ldf(0x101D6254));
        getEvFloatData(i_this, w + 0x20, 0x1004D55C /* "LatitudeMin" */, ldf(0x101D6258));
        getEvFloatData(i_this, w + 0x24, 0x1004D568 /* "LatitudeMax" */, ldf(0x101D625C));
        getEvFloatData(i_this, w + 0x28, 0x1004D5A4 /* "LongitudeMin" */, ldf(0x101D6260));
        getEvFloatData(i_this, w + 0x2C, 0x1004D5B4 /* "LongitudeMax" */, ldf(0x101D6264));
        getEvFloatData(i_this, w + 0x30, 0x1004D554 /* "Fovy" */, ldf(0x101D624C));
        stf(w + 0x44, ldf(i_this + 0x3C));
        stf(w + 0x48, degree(i_this + 0x40));
        stf(w + 0x4C, degree(i_this + 0x42));
    }
    gabi::Local<cXyz> A;
    gabi::Local<cXyz> B;
    gabi::Local<cXyz> D;
    gabi::Local<u8[8]> G;
    gabi::Local<cXyz> C;
    u32 aa = gabi::ea(A.get()), bb = gabi::ea(B.get()), dd = gabi::ea(D.get()), gg = gabi::ea(G.get()), cc = gabi::ea(C.get());
    attentionPos(i_this, aa, ld(w + 0));
    attentionPos(i_this, bb, ld(w + 4));
    cXyz_mi(aa, dd, bb);
    gabi::call(0x02007324, gg, dd); /* cSGlobe(const cXyz&): the direction between the actors */
    {
        gabi::Local<u32> id;
        u32 idp = gabi::ea(id.get());
        u32 pid = ld(w + 8);
        st(idp, pid);
        if (pid == 0xFFFFFFFF) return 1;
        if (gabi::call<u32>(0x025D5218, 0x025E1234, idp) == 0) return 1;
    }
    {
        gabi::Local<u32> id;
        u32 idp = gabi::ea(id.get());
        u32 pid = ld(w + 0xC);
        st(idp, pid);
        if (pid == 0xFFFFFFFF) return 1;
        if (gabi::call<u32>(0x025D5218, 0x025E1234, idp) == 0) return 1;
    }
    f32 ratio = ldf(w + 0x34);
    if (ratio < zero || ratio > ldf(0x1004CEA4)) { /* blt / ble: NaN takes the second branch */
        gabi::Local<cXyz> r;
        u32 rr = gabi::ea(r.get());
        gabi::call(0x02508804, i_this, rr, ld(w + 0), ld(w + 4), w + 0x38, ldf(0x1004D26C)); /* relationalPos(a1, a2, gap, 0.25) */
        copy3(cc, rr);
    } else {
        gabi::Local<cXyz> p1;
        gabi::Local<cXyz> p2;
        gabi::Local<cXyz> s;
        gabi::Local<cXyz> m;
        u32 q1 = gabi::ea(p1.get()), q2 = gabi::ea(p2.get()), ss = gabi::ea(s.get()), mm = gabi::ea(m.get());
        attentionPos(i_this, q1, ld(w + 0));
        attentionPos(i_this, q2, ld(w + 4));
        cXyz_pl(q1, ss, q2);
        cXyz_ml(ss, mm, ldf(w + 0x34));
        copy3(cc, mm);
    }
    cXyz_mi(cc, aa, i_this + 0x44);
    cXyz_ml(aa, bb, ldf(w + 0x10));
    vecAdd(i_this + 0x44, bb, i_this + 0x44);
    /* longitude */
    gabi::Local<s16> du;
    u32 dup = gabi::ea(du.get());
    gabi::call(0x020068B0, gg + 6, dup, i_this + 0x42); /* cSAngle::operator- */
    f32 lon = degree(dup);
    f32 cur = ldf(w + 0x4C);
    f32 lmin = ldf(w + 0x28);
    if (cur < lmin) lon = lmin; /* bge: taken on NaN */
    else if (cur > ldf(w + 0x2C)) lon = ldf(w + 0x2C);
    lon = lon + degree(gg + 6);
    cur = ldf(w + 0x4C);
    stf(w + 0x4C, gabi::fmadds(lon - cur, ldf(w + 0x14), cur));
    /* latitude */
    f32 lat = degree(i_this + 0x40);
    f32 vmin = ldf(w + 0x20);
    f32 v = ldf(w + 0x48);
    f32 tgt;
    if (v < vmin) tgt = vmin;
    else if (v > ldf(w + 0x24)) tgt = ldf(w + 0x24);
    else tgt = lat;
    f32 eyeCus = ldf(w + 0x14);
    f32 rad = ldf(w + 0x44);
    f32 rmin = ldf(w + 0x18);
    stf(w + 0x48, gabi::fmadds(tgt - v, eyeCus, v));
    f32 curR = ldf(i_this + 0x3C);
    f32 rt;
    if (rad < rmin) rt = rmin;
    else if (rad > ldf(w + 0x1C)) rt = ldf(w + 0x1C);
    else rt = curR;
    f32 newR = gabi::fmadds(rt - rad, ldf(w + 0x14), rad);
    stf(w + 0x44, newR);
    gabi::Local<s16[2]> va;
    u32 vap = gabi::ea(va.get());
    u32 av = gabi::call<u32>(0x020066C0, vap, ldf(w + 0x48));     /* cSAngle(latitude) */
    u32 au = gabi::call<u32>(0x020066C0, vap + 2, ldf(w + 0x4C)); /* cSAngle(longitude) */
    gabi::call(0x020071E0, i_this + 0x3C, newR, av, au);          /* cSGlobe::Val(r, V, U) */
    cSGlobe_Xyz(i_this + 0x3C, aa);
    cXyz_pl(i_this + 0x44, bb, aa);
    copy3(i_this + 0x50, bb);
    f32 fv = ldf(i_this + 0x60);
    stf(i_this + 0x60, gabi::fmadds(ldf(w + 0x30) - fv, ldf(w + 0x10), fv));
    return 1;
}
VERIFY(0x0253B168, dCamera_c_twoActor0EvCamera);

} // namespace d_ev_camera_6_cpp

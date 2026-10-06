/* d_ev_camera, part 5: getItemEvCamera. See
 * d_ev_camera.cpp for the unit's layout notes; written from the WWHD code (the GameCube
 * d_ev_camera.cpp is all "Nonmatching" stubs). */
#include "bindings.h"

namespace d_ev_camera_5_cpp {

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
static inline u32 gi() { return gabi::call<u32>(0x025200D4); }
static inline u32 getEvIntData(u32 c, u32 out, u32 name, s32 def) { return gabi::call<u32>(0x02530634, c, out, name, def); }
static inline void relationalPos(u32 c, u32 out, u32 actor, u32 ofs) { gabi::call(0x0250242C, c, out, actor, ofs); }
static inline void positionOf(u32 c, u32 out, u32 actor) { gabi::call(0x024F8D5C, c, out, actor); }
static inline bool lineBGCheck(u32 c, u32 a, u32 b, u32 flags) { return gabi::call<bool>(0x024FCBE8, c, a, b, flags); }
static inline void cXyz_mi(u32 a, u32 out, u32 b) { gabi::call(0x0201ADE0, a, out, b); }
static inline void cXyz_pl(u32 a, u32 out, u32 b) { gabi::call(0x0201AD78, a, out, b); }
static inline void cXyz_ml(u32 a, u32 out, f32 s) { gabi::call(0x0201AE48, a, out, s); }
static inline void cSGlobe_Val(u32 g, u32 v) { gabi::call(0x020072E0, g, v); }
static inline void cSGlobe_Xyz(u32 g, u32 out) { gabi::call(0x020073AC, g, out); }
static inline void cSAngle_mi(u32 a, u32 out, u32 b) { gabi::call(0x020068B0, a, out, b); }
static inline void cSAngle_ml(u32 a, u32 out, f32 f) { gabi::call(0x0200693C, a, out, f); }
static inline void cSAngle_addeq(u32 a, u32 b) { gabi::call(0x020068CC, a, b); }
static inline void setDone(u32 c) {
    st8(c + 0x102, 1);
    st8(c + 0x101, 1);
    st8(c + 0x100, 1);
}
static inline void f3(u32 v, f32 x, f32 y, f32 z) {
    stf(v + 0, x);
    stf(v + 4, y);
    stf(v + 8, z);
}
/* eye = centre + direction (mViewCache) */
static inline void eyeFromGlobe(u32 c) {
    gabi::Local<cXyz> x;
    gabi::Local<cXyz> e;
    u32 xx = gabi::ea(x.get()), ee = gabi::ea(e.get());
    cSGlobe_Xyz(c + 0x3C, xx);
    cXyz_pl(c + 0x44, ee, xx);
    copy3(c + 0x50, ee);
}

/* 025351A0: getItemEvCamera: a state machine in work+0 (0/1: wait "Timer1" frames, 0xA: pick the
 * first of four eye offsets around the player with a clear view, 0xB: blend to it over "Timer2"
 * frames, 0xC/0x63: hold). work: +4 frames of this state, +0xC centre, +0x18 target fovy (79),
 * +0x1C/+0x20 frame counter, +0x24 target direction (cSGlobe), +0x44 Timer1 (27), +0x48 Timer2 (5) */
static u32 dCamera_c_getItemEvCamera(u32 i_this) {
    WWHD_FUNC(0x025351A0, u32, i_this);
    u32 w = i_this + 0x37C;
    gabi::Local<cXyz> ctrOfs;
    gabi::Local<cXyz[4]> eyeOfs;
    u32 co = gabi::ea(ctrOfs.get()), eo = gabi::ea(eyeOfs.get());
    f3(co, ldf(0x1004D06C), ldf(0x1004D070), ldf(0x1004D074));
    f3(eo + 0x00, ldf(0x1004D078), ldf(0x1004D07C), ldf(0x1004D080));
    f3(eo + 0x0C, ldf(0x1004D084), ldf(0x1004D088), ldf(0x1004D08C));
    f3(eo + 0x18, ldf(0x1004D090), ldf(0x1004D094), ldf(0x1004D098));
    f3(eo + 0x24, ldf(0x1004D078), ldf(0x1004D07C), ldf(0x1004D080));
    gabi::Local<s16[2]> nameU; /* the original's sp+0x20: the process name searched for, then cSAngle U */
    gabi::Local<s16> angV;
    u32 nm = gabi::ea(nameU.get()), au = nm + 2, av = gabi::ea(angV.get());
    gabi::call(0x020065FC, au); /* cSAngle() */
    gabi::call(0x020065FC, av);
    u32 ret = 0;
    u32 state;
    if (ld(i_this + 0x11C) == 0) {
        state = 0;
        st(w + 0, 0);
    } else {
        state = ld(w + 0);
    }
    if (state != 1 && state != 0xA && state != 0xB && state != 0xC && state != 0x63) {
        getEvIntData(i_this, w + 0x44, 0x1004D0A0 /* "Timer1" */, 0x1B);
        getEvIntData(i_this, w + 0x48, 0x1004D0A8 /* "Timer2" */, 5);
        st(w + 0x1C, 0);
        u32 t1 = ld(w + 0x44);
        st(w + 0, 1);
        st(w + 0x20, 0);
        st(w + 4, t1);
        state = 1;
    }
    if (state == 1) {
        st(i_this + 0x510, ld(i_this + 0x510) | 1);
        if ((s32)ld(w + 0x20) < (s32)ld(w + 4)) {
            st(w + 0x20, ld(w + 0x20) + 1);
            return 0;
        }
        st(w + 0, 0xA);
        st(w + 0x20, ld(w + 0x20) + 1);
        return 0;
    }
    gabi::Local<cXyz> r;
    gabi::Local<cXyz> d;
    u32 rr = gabi::ea(r.get()), dd = gabi::ea(d.get());
    if (state == 0xA) {
        st(w + 0x20, 0);
        relationalPos(i_this, rr, ld(i_this + 0x128), co);
        copy3(w + 0xC, rr);
        gabi::Local<cXyz> e;
        gabi::Local<cXyz> eye;
        gabi::Local<cXyz> pos;
        gabi::Local<cXyz> a1;
        gabi::Local<cXyz> a2;
        u32 ee = gabi::ea(e.get()), ey = gabi::ea(eye.get()), pp = gabi::ea(pos.get());
        u32 p1 = gabi::ea(a1.get()), p2 = gabi::ea(a2.get());
        for (u32 k = 0; k < 4; k++) {
            relationalPos(i_this, ee, ld(i_this + 0x128), eo + k * 12);
            copy3(ey, ee);
            positionOf(i_this, ee, ld(i_this + 0x128));
            f32 floor = ldf(ee + 4) + ldf(i_this + 0x36C);
            if (ldf(ey + 4) < floor) { /* bge: taken on NaN */
                positionOf(i_this, pp, ld(i_this + 0x128));
                stf(ey + 4, ldf(pp + 4) + ldf(i_this + 0x36C));
            }
            u32 item = 0;
            s32 pad = (s32)ld(i_this + 0x124);
            u32 g = gi();
            if (ld(g + 0x5CD8 + (u32)(pad << 4)) & 0x10000) { /* the player holds an item up */
                st16(nm, 0xA5);
                item = gabi::call<u32>(0x025D5218, 0x025E121C /* fpcSch_JudgeForPName */, nm); /* fopAcIt_Judge */
            }
            if (lineBGCheck(i_this, w + 0xC, ey, 0x8F)) continue;
            for (u32 i = 0; i < 3; i++) stf(p1 + 4 * i, ldf(w + 0xC + 4 * i));
            for (u32 i = 0; i < 3; i++) stf(p2 + 4 * i, ldf(ey + 4 * i));
            u32 pl = ld(i_this + 0x128);
            g = gi();
            if (!gabi::call<bool>(0x025187D8, g + 0x26A4, p1, p2, ldf(0x1004CF80), pl, item)) break; /* dCcS::ChkCamera */
        }
        cXyz_mi(ey, rr, w + 0xC);
        cSGlobe_Val(w + 0x24, rr);
        u32 t2 = ld(w + 0x48);
        st(w + 0, 0xB);
        st(w + 4, t2);
        stf(w + 0x18, ldf(0x1004D09C)); /* 79 */
        state = 0xB;
    }
    if (state == 0xB) {
        f32 t = (f32)(f64)(s32)ld(w + 0x20) / (f32)(f64)(s32)ld(w + 4);
        f32 fovy = ldf(i_this + 0x60);
        stf(i_this + 0x60, gabi::fmadds(ldf(w + 0x18) - fovy, t, fovy));
        relationalPos(i_this, rr, ld(i_this + 0x128), co);
        copy3(w + 0xC, rr);
        gabi::Local<cXyz> s;
        u32 ss = gabi::ea(s.get());
        cXyz_mi(w + 0xC, ss, i_this + 0x44);
        cXyz_ml(ss, rr, t);
        gabi::call(0x028E8D88, i_this + 0x44, rr, i_this + 0x44); /* PSVECAdd */
        st16(av, (u16)lds16(i_this + 0x40));
        st16(au, (u16)lds16(i_this + 0x42));
        f32 rad = ldf(i_this + 0x3C);
        rad = gabi::fmadds(ldf(w + 0x24) - rad, t, rad);
        gabi::Local<s16> t1;
        gabi::Local<s16> t2;
        u32 a1 = gabi::ea(t1.get()), a2 = gabi::ea(t2.get());
        cSAngle_mi(w + 0x28, a1, av);
        cSAngle_ml(a1, a2, t);
        cSAngle_addeq(av, a2);
        cSAngle_mi(w + 0x2A, a2, au);
        cSAngle_ml(a2, a1, t);
        cSAngle_addeq(au, a1);
        gabi::call(0x020071E0, i_this + 0x3C, rad, av, au); /* cSGlobe::Val(r, V, U) */
        gabi::Local<cXyz> x;
        u32 xx = gabi::ea(x.get());
        cSGlobe_Xyz(i_this + 0x3C, rr);
        cXyz_pl(i_this + 0x44, xx, rr);
        copy3(i_this + 0x50, xx);
        if ((s32)ld(w + 0x20) < (s32)ld(w + 4)) {
            st(w + 0x20, ld(w + 0x20) + 1);
            return 0;
        }
        st(w + 0, 0xC);
        state = 0xC;
    }
    if (state == 0xC) {
        relationalPos(i_this, dd, ld(i_this + 0x128), co);
        copy3(i_this + 0x44, dd);
        eyeFromGlobe(i_this);
        st(w + 0, 0x63);
    }
    ret = 1;
    setDone(i_this);
    relationalPos(i_this, dd, ld(i_this + 0x128), co);
    copy3(i_this + 0x44, dd);
    eyeFromGlobe(i_this);
    st(w + 0x20, ld(w + 0x20) + 1);
    return ret;
}
VERIFY(0x025351A0, dCamera_c_getItemEvCamera);

static inline u32 getEvFloatData(u32 c, u32 out, u32 name, f32 def) { return gabi::call<u32>(0x0253072C, c, out, name, def); }
static inline u32 getEvXyzData(u32 c, u32 out, u32 name, u32 def) { return gabi::call<u32>(0x0253086C, c, out, name, def); }
static inline u32 getEvActor2(u32 c, u32 name, u32 def) { return gabi::call<u32>(0x02530CB4, c, name, def); }
static inline void attentionPos(u32 c, u32 out, u32 actor) { gabi::call(0x024F8F3C, c, out, actor); }
static inline void directionOf(u32 c, u32 out, u32 actor) { gabi::call(0x024F8F30, c, out, actor); }
static inline u32 cSAngleF(u32 tmp, f32 deg) { return gabi::call<u32>(0x020066C0, tmp, deg); } /* cSAngle(f32 degrees) */
static inline u32 cSAngleS(u32 tmp, s16 v) { return gabi::call<u32>(0x0200658C, tmp, (s32)v); }
static inline void cSAngle_pl(u32 a, u32 out, u32 b) { gabi::call(0x02006894, a, out, b); }
/* *dst = cSAngle(a + b) through a temporary (the GHS pattern of `x = a + b` with out-of-line ops) */
static inline void angAssignSum(u32 dst, u32 a, u32 b, u32 sumTmp, u32 ctTmp) {
    cSAngle_pl(a, sumTmp, b);
    u32 r = cSAngleS(ctTmp, lds16(sumTmp));
    st16(dst, (u16)lds16(r));
}

/* 02536404: turnToActorEvCamera: turn the camera towards an actor ("Target") over "Timer" frames,
 * keeping the angle to the actor's facing within FrontAngle. work: +0 CtrGap, +0xC target attention
 * position, +0x18 centre, +0x24 Cushion, +0x2C Timer, +0x30 target, +0x44 globe (r, V, U),
 * +0x50 FrontAngle */
static u32 dCamera_c_turnToActorEvCamera(u32 i_this) {
    WWHD_FUNC(0x02536404, u32, i_this);
    u32 dg = 0x10475890; /* function-local default CtrGap (0, 40, 0) */
    if (ld(0x104758B4) == 0) {
        f32 z0 = ldf(0x1004CE58), y40 = ldf(0x1004D0D0);
        stf(dg + 0, z0);
        stf(dg + 4, y40);
        st(0x104758B4, 1);
        stf(dg + 8, z0);
    }
    u32 w = i_this + 0x37C;
    if (ld(i_this + 0x11C) == 0) {
        gabi::Local<cXyz> def;
        u32 d = gabi::ea(def.get());
        f32 x = ldf(dg + 0), y = ldf(dg + 4);
        stf(d + 0, x);
        f32 z = ldf(dg + 8);
        stf(d + 4, y);
        stf(d + 8, z);
        getEvXyzData(i_this, w, 0x1004D108 /* "CtrGap" */, d);
        getEvFloatData(i_this, w + 0x24, 0x1004D0F8 /* "Cushion" */, 1.0f);
        getEvIntData(i_this, w + 0x2C, 0x1004D100 /* "Timer" */, (s32)ld(0x101D623C));
        getEvFloatData(i_this, w + 0x50, 0x1004D118 /* "FrontAngle" */, 179.0f);
        u32 a = getEvActor2(i_this, 0x1004D110 /* "Target" */, 0x1004D124 /* "@STARTER" */);
        st(w + 0x30, a);
        if (a == 0) {
            setDone(i_this);
            return 1;
        }
        attentionPos(i_this, d, a);
        copy3(w + 0xC, d);
        u32 t = ld(i_this + 0x11C);
        setDone(i_this);
        if (t == 0) {
            gabi::Local<cXyz> c;
            gabi::Local<cXyz> v;
            gabi::Local<u8[8]> g1;
            gabi::Local<cXyz> p;
            gabi::Local<cXyz> v2;
            gabi::Local<u8[8]> g2;
            gabi::Local<s16[2]> dir;
            gabi::Local<s16[2]> diff;
            gabi::Local<s16[24]> tmp;
            u32 cc = gabi::ea(c.get()), vv = gabi::ea(v.get()), G1 = gabi::ea(g1.get()), pp = gabi::ea(p.get());
            u32 vv2 = gabi::ea(v2.get()), G2 = gabi::ea(g2.get()), dr = gabi::ea(dir.get()), df = gabi::ea(diff.get());
            u32 tp = gabi::ea(tmp.get());
            relationalPos(i_this, cc, ld(i_this + 0x128), w);
            cXyz_mi(cc, vv, w + 0xC);
            gabi::call(0x02007324, G1, vv); /* cSGlobe(const cXyz&) */
            positionOf(i_this, pp, ld(w + 0x30));
            cXyz_mi(i_this + 0x50, vv2, pp);
            gabi::call(0x02007324, G2, vv2);
            directionOf(i_this, dr, ld(w + 0x30));
            cSAngle_mi(G2 + 6, df, dr);
            bool adjustNeg;
            if (lds16(df) < lds16(0x101FF354)) { /* the actor faces away: turn +5 degrees */
                u32 five = cSAngleF(tp + 0, ldf(0x1004CEB8));
                angAssignSum(G1 + 6, G1 + 6, five, tp + 2, tp + 4);
            } else {
                u32 five = cSAngleF(tp + 6, ldf(0x1004CF68));
                angAssignSum(G1 + 6, G1 + 6, five, tp + 8, tp + 10);
            }
            directionOf(i_this, dr, ld(w + 0x30));
            cSAngle_mi(G1 + 6, df + 2, dr);
            gabi::call(0x020065FC, tp + 12); /* cSAngle() */
            u32 lim = cSAngleF(tp + 14, -ldf(w + 0x50));
            adjustNeg = lds16(df + 2) < lds16(lim);
            if (adjustNeg) {
                directionOf(i_this, dr + 2, ld(w + 0x30));
                u32 b = cSAngleF(tp + 16, -ldf(w + 0x50));
                angAssignSum(G1 + 6, dr + 2, b, tp + 18, tp + 20);
            } else {
                u32 lim2 = cSAngleF(tp + 22, ldf(w + 0x50));
                if (lds16(df + 2) > lds16(lim2)) {
                    directionOf(i_this, dr + 2, ld(w + 0x30));
                    u32 b = cSAngleF(tp + 24, ldf(w + 0x50));
                    angAssignSum(G1 + 6, dr + 2, b, tp + 26, tp + 28);
                }
            }
            gabi::call(0x020071E0, w + 0x44, 120.0f, G1 + 4, G1 + 6); /* cSGlobe::Val(r, V, U) */
            relationalPos(i_this, pp, ld(i_this + 0x128), w);
            copy3(w + 0x18, pp);
        }
    }
    u32 t = ld(i_this + 0x11C);
    if (t >= ld(w + 0x2C)) {
        setDone(i_this);
        return 1;
    }
    f32 ratio = (f32)(f64)t / (f32)(f64)(s32)ld(w + 0x2C);
    gabi::Local<cXyz> a;
    gabi::Local<cXyz> b;
    gabi::Local<s16[4]> ang;
    gabi::Local<s16[2]> ct;
    u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get()), an = gabi::ea(ang.get()), cto = gabi::ea(ct.get());
    cXyz_mi(w + 0x18, aa, i_this + 0x44);
    cXyz_ml(aa, bb, ratio);
    gabi::call(0x028E8D88, i_this + 0x44, bb, i_this + 0x44); /* PSVECAdd */
    f32 r0 = ldf(i_this + 0x3C);
    stf(i_this + 0x3C, gabi::fmadds(ldf(w + 0x44) - r0, ratio, r0));
    /* U then V towards the target globe */
    cSAngle_mi(w + 0x4A, an + 4, i_this + 0x42);
    cSAngle_ml(an + 4, an + 2, ratio);
    cSAngle_pl(i_this + 0x42, an + 0, an + 2);
    u32 r = cSAngleS(cto + 0, lds16(an + 0));
    st16(i_this + 0x42, (u16)lds16(r));
    cSAngle_mi(w + 0x48, an + 0, i_this + 0x40);
    cSAngle_ml(an + 0, an + 2, ratio);
    cSAngle_pl(i_this + 0x40, an + 4, an + 2);
    r = cSAngleS(cto + 2, lds16(an + 4));
    st16(i_this + 0x40, (u16)lds16(r));
    cSGlobe_Xyz(i_this + 0x3C, bb);
    cXyz_pl(i_this + 0x44, aa, bb);
    copy3(i_this + 0x50, aa);
    cXyz_mi(i_this + 0x50, aa, i_this + 0x44);
    cSGlobe_Val(i_this + 0x3C, aa);
    return 0;
}
VERIFY(0x02536404, dCamera_c_turnToActorEvCamera);

static inline f32 vecLen(u32 v) { return gabi::call<f32>(0x028F4384, gabi::call<f32>(0x028E8DD0, v)); } /* sqrtf(PSVECSquareMag) */
/* g.Val(eye - centre), pushed out to 60 when closer than 45 */
static inline void restoreGlobe(u32 g, u32 eye, u32 ctr) {
    gabi::Local<cXyz> d;
    gabi::Local<cXyz> e;
    gabi::Local<cXyz> n;
    gabi::Local<cXyz> m;
    u32 dd = gabi::ea(d.get()), ee = gabi::ea(e.get()), nn = gabi::ea(n.get()), mm = gabi::ea(m.get());
    cXyz_mi(eye, dd, ctr);
    cSGlobe_Val(g, dd);
    cXyz_mi(eye, ee, ctr);
    if (vecLen(ee) < ldf(0x1004D008)) { /* 45; bge: taken on NaN */
        gabi::call(0x0201B31C, ee, nn); /* cXyz::normalize */
        cXyz_ml(nn, mm, ldf(0x1004D004)); /* 60 */
        copy3(ee, mm);
        cSGlobe_Val(g, ee);
    }
}

/* 02534964: restorePosEvCamera: back to a saved view ("Dest": 0/1 m0A4[], 9 the play's slot, else
 * the last view) depending on how far it is from the "Target" (mode 1: near, inside the view;
 * 2: middle distance with a clear line; 3: jump). work: +0 CtrGap, +0xC target centre, +0x18
 * Cushion, +0x1C NearTimer, +0x20 NearDist, +0x24 FarTimer (frames), +0x28 FarDist, +0x34 Target,
 * +0x38 globe, +0x40 mode, +0x44 Dest, +0x48 saved view (centre, eye, fovy, bank), +0x68 TargetType */
static u32 dCamera_c_restorePosEvCamera(u32 i_this) {
    WWHD_FUNC(0x02534964, u32, i_this);
    u32 dz = 0x10475884;
    if (ld(0x104758B0) == 0) {
        f32 x = ldf(0x101FFBA8), y = ldf(0x101FFBAC);
        stf(dz + 0, x);
        f32 z = ldf(0x101FFBB0);
        stf(dz + 4, y);
        stf(dz + 8, z);
        st(0x104758B0, 1);
    }
    u32 w = i_this + 0x37C;
    if (ld(i_this + 0x11C) == 0) {
        gabi::Local<cXyz> def;
        u32 d = gabi::ea(def.get());
        f32 z = ldf(dz + 8), y = ldf(dz + 4), x = ldf(dz + 0);
        stf(d + 8, z);
        stf(d + 0, x);
        stf(d + 4, y);
        getEvXyzData(i_this, w, 0x1004D024 /* "CtrGap" */, d);
        getEvFloatData(i_this, w + 0x18, 0x1004D00C /* "Cushion" */, 1.0f);
        getEvFloatData(i_this, w + 0x20, 0x1004D048 /* "NearDist" */, 750.0f);
        getEvFloatData(i_this, w + 0x28, 0x1004D014 /* "FarDist" */, 1500.0f);
        getEvIntData(i_this, w + 0x1C, 0x1004D03C /* "NearTimer" */, (s32)ld(0x101D6230));
        getEvIntData(i_this, w + 0x24, 0x1004D054 /* "FarTimer" */, (s32)ld(0x101D6234));
        getEvIntData(i_this, w + 0x44, 0x1004D02C /* "Dest" */, 2);
        getEvIntData(i_this, w + 0x68, 0x1004D060 /* "TargetType" */, 0);
        u32 dest = ld(w + 0x44);
        u32 src;
        if (dest == 9) {
            u32 g = gi();
            for (u32 i = 0; i < 0x18; i += 4) st(w + 0x48 + i, ld(g + 0x5B0C + i));
            stf(w + 0x60, ldf(g + 0x5B24));
            st16(w + 0x64, (u16)lds16(g + 0x5B28));
        } else {
            src = dest == 0 ? i_this + 0xA4 : dest == 1 ? i_this + 0xC4 : i_this + 0x84;
            for (u32 i = 0; i < 0x18; i += 4) st(w + 0x48 + i, ld(src + i));
            stf(w + 0x60, ldf(src + 0x18));
        }
        u32 a = getEvActor2(i_this, 0x1004D034 /* "Target" */, 0x1004D01C /* "@PLAYER" */);
        st(w + 0x34, a);
        if (a == 0) return 1;
        gabi::Local<cXyz> r;
        gabi::Local<u8[8]> globe;
        u32 rr = gabi::ea(r.get()), gg = gabi::ea(globe.get());
        relationalPos(i_this, rr, a, w);
        copy3(w + 0xC, rr);
        cXyz_mi(w + 0x54, rr, i_this + 0x44);
        gabi::call(0x02007324, gg, rr); /* cSGlobe(const cXyz&) */
        f32 dist = ldf(gg);
        u32 mode;
        if (dist < ldf(w + 0x20)) {
            mode = gabi::call<u32>(0x025050E4, i_this, w + 0xC) ^ 1; /* !pointInSight */
        } else if (dist < ldf(w + 0x28) && !lineBGCheck(i_this, i_this + 0x1C, w + 0xC, 0x8F)) {
            mode = 2;
        } else {
            mode = 3;
        }
        st(w + 0x40, mode);
        setDone(i_this);
    }
    u32 mode = ld(w + 0x40);
    if (mode < 1 || mode > 3) {
        setDone(i_this);
        return 1;
    }
    if (mode == 3) {
        if (ld(i_this + 0x11C) == 0) {
            if (ld(w + 0x68) == 1) copy3(i_this + 0x44, w + 0x48);
            else copy3(i_this + 0x44, w + 0xC);
            restoreGlobe(i_this + 0x3C, w + 0x54, w + 0x48);
            gabi::Local<cXyz> x;
            gabi::Local<cXyz> e;
            u32 xx = gabi::ea(x.get()), ee = gabi::ea(e.get());
            cSGlobe_Xyz(i_this + 0x3C, xx);
            cXyz_pl(i_this + 0x44, ee, xx);
            copy3(i_this + 0x50, ee);
            stf(i_this + 0x60, ldf(w + 0x60));
        }
        setDone(i_this);
        return 1;
    }
    if (ld(i_this + 0x11C) == 0) restoreGlobe(w + 0x38, w + 0x54, w + 0x48);
    u32 t = ld(i_this + 0x11C);
    if (t >= ld(w + 0x24)) {
        setDone(i_this);
        return 1;
    }
    f32 ratio = (f32)(f64)t / (f32)(f64)(s32)ld(w + 0x24);
    gabi::Local<cXyz> a;
    gabi::Local<cXyz> b;
    gabi::Local<s16[4]> an;
    gabi::Local<s16[2]> ct;
    u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get()), ang = gabi::ea(an.get()), cto = gabi::ea(ct.get());
    cXyz_mi(ld(w + 0x68) == 1 ? w + 0x48 : w + 0xC, aa, i_this + 0x44);
    cXyz_ml(aa, bb, ratio);
    gabi::call(0x028E8D88, i_this + 0x44, bb, i_this + 0x44); /* PSVECAdd */
    f32 r0 = ldf(i_this + 0x3C);
    stf(i_this + 0x3C, gabi::fmadds(ldf(w + 0x38) - r0, ratio, r0));
    cSAngle_mi(w + 0x3E, ang + 4, i_this + 0x42);
    cSAngle_ml(ang + 4, ang + 2, ratio);
    cSAngle_pl(i_this + 0x42, ang + 0, ang + 2);
    u32 r = cSAngleS(cto + 0, lds16(ang + 0));
    st16(i_this + 0x42, (u16)lds16(r));
    cSAngle_mi(w + 0x3C, ang + 0, i_this + 0x40);
    cSAngle_ml(ang + 0, ang + 2, ratio);
    cSAngle_pl(i_this + 0x40, ang + 4, ang + 2);
    r = cSAngleS(cto + 2, lds16(ang + 4));
    st16(i_this + 0x40, (u16)lds16(r));
    cSGlobe_Xyz(i_this + 0x3C, bb);
    cXyz_pl(i_this + 0x44, aa, bb);
    copy3(i_this + 0x50, aa);
    f32 fv = ldf(i_this + 0x60);
    stf(i_this + 0x60, gabi::fmadds(ldf(w + 0x60) - fv, ratio, fv));
    return 0;
}
VERIFY(0x02534964, dCamera_c_restorePosEvCamera);

} // namespace d_ev_camera_5_cpp

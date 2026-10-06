/* d_ev_camera, part 7: possessedEvCamera. See
 * d_ev_camera.cpp for the unit's layout notes; written from the WWHD code (the GameCube
 * d_ev_camera.cpp is all "Nonmatching" stubs). */
#include "bindings.h"

namespace d_ev_camera_7_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline s8 lds8(u32 a) { return gabi::load<s8>(a); }
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
static inline u32 getEvFloatData(u32 c, u32 out, u32 name, f32 def) { return gabi::call<u32>(0x0253072C, c, out, name, def); }
static inline u32 getEvActor2(u32 c, u32 name, u32 def) { return gabi::call<u32>(0x02530CB4, c, name, def); }
static inline void eyePos(u32 c, u32 out, u32 actor) { gabi::call(0x024FC0B0, c, out, actor); }
static inline void directionOf(u32 c, u32 out, u32 actor) { gabi::call(0x024F8F30, c, out, actor); }
static inline void cXyz_mi(u32 a, u32 out, u32 b) { gabi::call(0x0201ADE0, a, out, b); }
static inline void cXyz_pl(u32 a, u32 out, u32 b) { gabi::call(0x0201AD78, a, out, b); }
static inline void cXyz_ml(u32 a, u32 out, f32 s) { gabi::call(0x0201AE48, a, out, s); }
static inline void vecAdd(u32 a, u32 b, u32 out) { gabi::call(0x028E8D88, a, b, out); }
static inline void cSGlobe_Val(u32 g, u32 v) { gabi::call(0x020072E0, g, v); }
static inline void cSGlobe_ValRVU(u32 g, f32 r, u32 v, u32 u) { gabi::call(0x020071E0, g, r, v, u); }
static inline void cSGlobe_Xyz(u32 g, u32 out) { gabi::call(0x020073AC, g, out); }
static inline void cSAngle_mi(u32 a, u32 out, u32 b) { gabi::call(0x020068B0, a, out, b); }
static inline void cSAngle_ml(u32 a, u32 out, f32 f) { gabi::call(0x0200693C, a, out, f); }
static inline void cSAngle_pl(u32 a, u32 out, u32 b) { gabi::call(0x02006894, a, out, b); }
static inline u32 cSAngleS(u32 tmp, s16 v) { return gabi::call<u32>(0x0200658C, tmp, (s32)v); }
static inline void setDone(u32 c) {
    st8(c + 0x102, 1);
    st8(c + 0x101, 1);
    st8(c + 0x100, 1);
}
/* the blur effect and a short rumble */
static inline void blureStart(u32 c, u32 w, u32 reset, f32 alpha, bool scale) {
    gabi::call(0x0250D464, c, reset);            /* ResetBlure */
    gabi::call(0x025153FC, c, 0xB);              /* SetBlurePositionType */
    gabi::call(0x0250D4C0, c, ld(w + 8));        /* SetBlureTimer */
    gabi::call(0x0250D4C8, c, alpha);            /* SetBlureAlpha */
    if (scale) gabi::call(0x0250D4D0, c, ldf(0x1004D1EC)); /* SetBlureScale(0.99) */
    u32 g = gi();
    gabi::Local<cXyz> dir;
    u32 d = gabi::ea(dir.get());
    f32 z0 = ldf(0x1004CE58), one = ldf(0x1004CEA4);
    stf(d + 0, z0);
    stf(d + 4, one);
    stf(d + 8, z0);
    gabi::call(0x025CB374, g + 0x599C, 1, 0x20, d); /* dVibration_c::StartShock */
}

/* 0253A998: possessedEvCamera (the camera follows a possessed actor, e.g. a Hyoi pear seagull).
 * work: +0 state (0 init, 1 easing, 0x63 done), +4 Type, +8 Timer, +0xC frames left, +0x10 Radius,
 * +0x14 Latitude/Longitude (cSAngle), +0x18 Fovy, +0x1C Cushion, +0x20 Blure, +0x24 Target,
 * +0x28 target globe */
static u32 dCamera_c_possessedEvCamera(u32 i_this) {
    WWHD_FUNC(0x0253A998, u32, i_this);
    u32 w = i_this + 0x37C;
    u32 state;
    if (ld(i_this + 0x11C) != 0) {
        state = ld(w + 0);
    } else {
        state = 0;
        st(w + 0, 0);
    }
    if (state == 0x63) {
        setDone(i_this);
        return 1;
    }
    if (state != 1) {
        u32 a = getEvActor2(i_this, 0x1004D4EC /* "Target" */, 0x1004D4CC /* "@PLAYER" */);
        st(w + 0x24, a);
        if (a == 0) return 1;
        getEvIntData(i_this, w + 4, 0x1004D4F4 /* "Type" */, 0);
        getEvIntData(i_this, w + 8, 0x1004D4DC /* "Timer" */, 10);
        getEvFloatData(i_this, w + 0x10, 0x1004D4FC /* "Radius" */, ldf(0x1004D004));
        getEvFloatData(i_this, w + 0x1C, 0x1004D4D4 /* "Cushion" */, ldf(0x1004CEA4));
        gabi::Local<f32> fv;
        u32 fp = gabi::ea(fv.get());
        getEvFloatData(i_this, fp, 0x1004D518 /* "Latitude" */, ldf(0x1004CF68));
        gabi::call(0x02006694, w + 0x14, ldf(fp)); /* cSAngle::Val(f32 degrees) */
        getEvFloatData(i_this, fp, 0x1004D50C /* "Longitude" */, ldf(0x1004CE58));
        gabi::call(0x02006694, w + 0x16, ldf(fp));
        getEvFloatData(i_this, w + 0x18, 0x1004D504 /* "Fovy" */, ldf(0x1004D008));
        getEvIntData(i_this, w + 0x20, 0x1004D4E4 /* "Blure" */, 0);
        u32 type = ld(w + 4);
        u32 act = ld(w + 0x24);
        gabi::Local<s16[2]> ang;
        u32 ap = gabi::ea(ang.get());
        if (type == 0) {
            directionOf(i_this, ap, act);
            cSAngle_pl(w + 0x16, ap + 2, ap);
            cSGlobe_ValRVU(w + 0x28, ldf(w + 0x10), w + 0x14, ap + 2);
            /* keep the current view in m0A4[1] */
            s16 bank = lds16(i_this + 0x5C);
            u32 x = ld(i_this + 0x44);
            st16(i_this + 0xE0, (u16)bank);
            for (u32 i = 0; i < 0x18; i += 4) st(i_this + 0xC4 + i, i == 0 ? x : ld(i_this + 0x44 + i));
            st16(i_this + 0xE2, 2);
            stf(i_this + 0xDC, ldf(i_this + 0x60));
        } else {
            gabi::Local<cXyz> e;
            gabi::Local<cXyz> x;
            u32 ee = gabi::ea(e.get()), xx = gabi::ea(x.get());
            eyePos(i_this, ee, act);
            copy3(i_this + 0x44, ee);
            cXyz_mi(i_this + 0xD0, ee, i_this + 0xC4);
            cSGlobe_Val(w + 0x28, ee);
            directionOf(i_this, ap, ld(w + 0x24));
            cSAngle_pl(w + 0x16, ap + 2, ap);
            cSGlobe_ValRVU(i_this + 0x3C, ldf(w + 0x10), w + 0x14, ap + 2);
            cSGlobe_Xyz(i_this + 0x3C, xx);
            cXyz_pl(i_this + 0x44, ee, xx);
            copy3(i_this + 0x50, ee);
            stf(i_this + 0x60, ldf(w + 0x18));
        }
        u32 blure = ld(w + 0x20);
        st(w + 0, 1);
        st(w + 0xC, ld(w + 8));
        if (blure == 1) blureStart(i_this, w, 1, ldf(0x1004D4B0), false);
        else if (blure == 2) blureStart(i_this, w, 0, ldf(0x1004D4B4), true);
        return 0;
    }
    /* state 1: ease towards the target globe around the actor's eye */
    f32 r = ldf(0x1004CEA4) / (f32)(f64)(s32)ld(w + 0xC);
    gabi::Local<cXyz> a;
    gabi::Local<cXyz> b;
    gabi::Local<cXyz> c;
    gabi::Local<s16[4]> an;
    gabi::Local<s16[2]> ct;
    u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get()), cc = gabi::ea(c.get()), ang = gabi::ea(an.get()), cto = gabi::ea(ct.get());
    eyePos(i_this, aa, ld(w + 0x24));
    cXyz_mi(aa, bb, i_this + 0x44);
    cXyz_ml(bb, cc, r);
    vecAdd(i_this + 0x44, cc, i_this + 0x44);
    f32 r0 = ldf(i_this + 0x3C);
    stf(i_this + 0x3C, gabi::fmadds(ldf(w + 0x28) - r0, r, r0));
    cSAngle_mi(w + 0x2C, ang + 4, i_this + 0x40);
    cSAngle_ml(ang + 4, ang + 2, r);
    cSAngle_pl(i_this + 0x40, ang + 0, ang + 2);
    u32 t = cSAngleS(cto + 0, lds16(ang + 0));
    st16(i_this + 0x40, (u16)lds16(t));
    cSAngle_mi(w + 0x2E, ang + 0, i_this + 0x42);
    cSAngle_ml(ang + 0, ang + 2, r);
    cSAngle_pl(i_this + 0x42, ang + 4, ang + 2);
    t = cSAngleS(cto + 2, lds16(ang + 4));
    st16(i_this + 0x42, (u16)lds16(t));
    cSGlobe_Xyz(i_this + 0x3C, bb);
    cXyz_pl(i_this + 0x44, aa, bb);
    gabi::Local<cXyz> eye;
    u32 ey = gabi::ea(eye.get());
    copy3(ey, aa);
    cXyz_mi(ey, bb, i_this + 0x50);
    cXyz_ml(bb, aa, ldf(w + 0x1C));
    vecAdd(i_this + 0x50, aa, i_this + 0x50);
    f32 fv = ldf(i_this + 0x60);
    stf(i_this + 0x60, gabi::fmadds(ldf(w + 0x18) - fv, r, fv));
    if (ld(w + 0x20) == 1) {
        s32 camId = gabi::call<s32>(0x025DA64C, ld(i_this + 0)); /* fopCamM_GetParam */
        s32 idx = lds8(gi() + 0x5AFC + (u32)camId * 0x34);
        u32 win = gi() + (u32)idx * 0x2C;
        gabi::Local<cXyz> ep;
        gabi::Local<f32[2]> scr;
        u32 epp = gabi::ea(ep.get()), sp = gabi::ea(scr.get());
        eyePos(i_this, epp, ld(w + 0x24));
        gabi::call(0x025F0C48, epp, sp); /* mDoLib_project */
        f32 px = ldf(sp + 0) / ldf(win + 0x5AEC);
        f32 py = ldf(sp + 4) / ldf(win + 0x5AF0);
        f32 z0 = ldf(0x1004CE58);
        gabi::call(0x02515404, i_this, px, py, z0); /* SetBlurePosition */
        gabi::call(0x0250D4C8, i_this, gabi::fmadds(ldf(0x1004D4B8), r, ldf(0x1004D4B0))); /* SetBlureAlpha(0.7 r + 0.5) */
        f32 sx = gabi::fmadds(ldf(0x1004D4BC), r, ldf(0x1004D4C0)); /* 0.09 r + 1.1 */
        f32 sy = gabi::fnmsubs(ldf(0x1004D4C8), r, ldf(0x1004D4C4)); /* 0.98 - 0.18 r */
        gabi::call(0x025153EC, i_this, sx, sy, z0); /* SetBlureScale */
    }
    u32 n = ld(w + 0xC) - 1; /* wraps like addic. */
    st(w + 0xC, n);
    if ((s32)n > 0) return 0;
    st(w + 0, 0x63);
    return 0;
}
VERIFY(0x0253A998, dCamera_c_possessedEvCamera);

static inline void positionOf(u32 c, u32 out, u32 actor) { gabi::call(0x024F8D5C, c, out, actor); }
static inline void relationalPos(u32 c, u32 out, u32 actor, u32 ofs) { gabi::call(0x0250242C, c, out, actor, ofs); }
static inline void relationalPosA(u32 c, u32 out, u32 actor, u32 ofs, u32 ang) { gabi::call(0x02514D00, c, out, actor, ofs, ang); }
static inline bool lineBGCheck(u32 c, u32 a, u32 b, u32 flags) { return gabi::call<bool>(0x024FCBE8, c, a, b, flags); }
static inline u32 cSAngleCopy(u32 out, u32 src) { return gabi::call<u32>(0x02006644, out, src); }
static inline void negX(u32 v) { stf(v, -ldf(v)); }
/* eye = player-relational offset (rotated by ang), lifted to the player's floor height + 0x36C */
static inline void gameOverEye(u32 c, u32 ofs, u32 ang, u32 eye, u32 tmpAng) {
    gabi::Local<cXyz> r;
    gabi::Local<cXyz> p;
    u32 rr = gabi::ea(r.get()), pp = gabi::ea(p.get());
    u32 ap = cSAngleCopy(tmpAng, ang);
    relationalPosA(c, rr, ld(c + 0x128), ofs, ap);
    copy3(eye, rr);
    positionOf(c, rr, ld(c + 0x128));
    if (ldf(eye + 4) < ldf(rr + 4) + ldf(c + 0x36C)) { /* bge: taken on NaN */
        positionOf(c, pp, ld(c + 0x128));
        stf(eye + 4, ldf(pp + 4) + ldf(c + 0x36C));
    }
}

/* gameOverEvCamera state 3: centre C and eye B around the player; when the view is blocked,
 * mirror B.z, then flatten C.z, then mirror B.z back */
static inline void gameOverEye3(u32 c, u32 w, u32 ang, u32 B, u32 C, u32 ctr, u32 eye3, u32 f) {
    gabi::Local<cXyz> r;
    u32 rr = gabi::ea(r.get());
    u32 ap = cSAngleCopy(f + 0x4C, ang);
    relationalPosA(c, rr, ld(c + 0x128), C, ap);
    copy3(ctr, rr);
    gameOverEye(c, B, ang, eye3, f + 0x4C);
    if (lineBGCheck(c, ctr, eye3, 0x7F)) {
        stf(B + 8, -ldf(B + 8));
        gameOverEye(c, B, ang, eye3, f + 0x24);
        if (lineBGCheck(c, ctr, eye3, 0x7F)) {
            stf(C + 8, ldf(0x1004CE58));
            ap = cSAngleCopy(f + 0x24, ang);
            relationalPosA(c, rr, ld(c + 0x128), C, ap);
            copy3(ctr, rr);
            gameOverEye(c, B, ang, eye3, f + 0x24);
            if (lineBGCheck(c, ctr, eye3, 0x7F)) {
                stf(B + 8, -ldf(B + 8));
                gameOverEye(c, B, ang, eye3, f + 0x24);
                lineBGCheck(c, ctr, eye3, 0x7F);
            }
        }
    }
    copy3(w + 0xC, C);
    copy3(w + 0x18, B);
    u32 r12 = ld(w + 0) + 1;
    u32 n4 = ld(w + 4);
    st(w + 0, r12);
    if (n4 == 0x1E) {
        st(w + 4, 0);
        st(w + 0, r12 + 1);
    }
}

/* 02535AA4: gameOverEvCamera: a state machine in work+0 (0/1: pick a clear eye offset around the
 * player from five, mirrored as needed; 2/3: after 130 frames a closer shot (tried mirrored and
 * lowered); 4: wait 30 frames; 0x32..0x34: the fixed variant used when ... (pad status bit 0x100000);
 * 5: hold). The centre/eye offsets are kept in work+0xC / work+0x18 and applied every frame. */
static u32 dCamera_c_gameOverEvCamera(u32 i_this) {
    WWHD_FUNC(0x02535AA4, u32, i_this);
    u32 sr25 = gabi::cpu->r[25], sr26 = gabi::cpu->r[26], sr27 = gabi::cpu->r[27];
    u32 w = i_this + 0x37C;
    f32 c0 = ldf(0x1004CE58);
    /* the original's frame: five eye offsets at +0xE0, followed by its register save area */
    gabi::Local<u8[0x150]> frame;
    u32 f = gabi::ea(frame.get());
    u32 tb = f + 0xE0, A = f + 0x80, B = f + 0x34, C = f + 0x50;
    stf(tb + 0x00, ldf(0x1004D0B8)); stf(tb + 0x04, ldf(0x1004D0BC)); stf(tb + 0x08, ldf(0x1004D0C0)); /* (85, -50, 165) */
    stf(tb + 0x0C, ldf(0x1004D0C4)); stf(tb + 0x10, ldf(0x1004D0C8)); stf(tb + 0x14, ldf(0x1004D004)); /* (72, -64, 60) */
    stf(tb + 0x18, ldf(0x1004D0C0)); stf(tb + 0x1C, ldf(0x1004D0CC)); stf(tb + 0x20, ldf(0x1004D008)); /* (165, -20, 45) */
    stf(tb + 0x24, ldf(0x1004D0B8)); stf(tb + 0x28, ldf(0x1004D0C0)); stf(tb + 0x2C, ldf(0x1004D0D0)); /* (85, 165, 40) */
    stf(tb + 0x30, ldf(0x1004CF74)); stf(tb + 0x34, ldf(0x1004D0D4)); stf(tb + 0x38, ldf(0x1004D0D8)); /* (10, -70, 110) */
    st(tb + 0x3C, sr25); st(tb + 0x40, sr26); st(tb + 0x44, sr27); /* stmw r25: the sixth "entry" */
    stf(A + 0, c0); stf(A + 4, ldf(0x1004D0B4)); stf(A + 8, c0);                   /* (0, -25, 0) */
    stf(B + 0, c0); stf(B + 4, ldf(0x1004D0E4)); stf(B + 8, ldf(0x1004D0E8));      /* (0, 170, 115) */
    stf(C + 0, c0); stf(C + 4, ldf(0x1004D0DC)); stf(C + 8, ldf(0x1004D0E0));      /* (0, -40, -35) */
    if (ld(i_this + 0x11C) == 0) {
        st(w + 0, 0);
        st(w + 4, 0);
        st(w + 8, ((ld(i_this + 0x7C) >> 1) & 1) ^ 1);
        s32 pad = (s32)ld(i_this + 0x124);
        u32 g = gi();
        if (ld(g + 0x5CD8 + ((u32)pad << 4)) & 0x100000) st(w + 0, 0x32);
        st8(i_this + 0x102, 1);
        st8(i_this + 0x100, 1);
        st8(i_this + 0x101, 1);
    }
    u32 ang = f + 8;
    gabi::call(0x020065FC, ang); /* cSAngle() */
    {
        s32 pad = (s32)ld(i_this + 0x124);
        u32 g = gi();
        if (ld(g + 0x5CD8 + ((u32)pad << 4)) & 0x10000) st16(ang, (u16)lds16(0x101FF35C));
        else st16(ang, (u16)lds16(0x101FF354));
    }
    u32 s = ld(w + 0);
    u32 eye = f + 0xC, ctr0 = f + 0x5C, ctr = f + 0x8C, eye3 = f + 0x28;
    bool toT = false;
    if (s == 5) toT = true;
    else if (s == 0x32 || s == 0x33 || s == 0x34) {
        if (s == 0x32) st(w + 0, 0x33);
        if (s != 0x34) {
            u32 v = ld(w + 0);
            stf(w + 0x14, ldf(0x1004CF84)); /* 30 */
            stf(w + 0x20, ldf(0x1004D0F4)); /* 175 */
            st(w + 0, v + 1);
            stf(w + 0x18, ldf(0x1004CF70)); /* 70 */
            stf(w + 0x1C, ldf(0x1004D0F0)); /* 155 */
            stf(w + 0xC, c0);
            stf(w + 0x10, ldf(0x1004D0EC)); /* 14 */
        }
        if (ld(w + 4) == 0xA0) {
            st(w + 4, 0);
            st(w + 0, 5);
        }
        toT = true;
    } else if (s == 4) {
        if (ld(w + 4) == 0x1E) {
            st(w + 4, 0);
            st(w + 0, s + 1);
        }
        toT = true;
    }
    if (!toT) {
        u32 r12 = s;
        if (s != 2 && s != 3) {
            if (s != 1) st(w + 0, 1);
            gabi::Local<cXyz> r;
            u32 rr = gabi::ea(r.get());
            relationalPos(i_this, rr, ld(i_this + 0x128), A);
            copy3(ctr0, rr);
            u32 k = 0;
            for (; k < 5; k++) {
                u32 e = tb + k * 12;
                if (ld(w + 8) != 0) negX(e);
                gameOverEye(i_this, e, ang, eye, f + 0xA);
                if (!lineBGCheck(i_this, ctr0, eye, 0x7F)) break;
                negX(e);
                gameOverEye(i_this, e, ang, eye, f + 0xA);
                if (!lineBGCheck(i_this, ctr0, eye, 0x7F)) break;
                st(w + 8, ld(w + 8) ^ 1);
            }
            copy3(i_this + 0x50, eye);
            copy3(w + 0xC, A);
            copy3(w + 0x18, tb + k * 12);
            r12 = ld(w + 0) + 1;
            st(w + 0, r12);
            s = 2;
        }
        if (s == 2) {
            if (ld(w + 4) != 0x82) toT = true;
            else {
                st(w + 4, 0);
                st(w + 0, r12 + 1);
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
    u32 ap = cSAngleCopy(f + 0x26, ang);
    relationalPosA(i_this, rr, ld(i_this + 0x128), w + 0xC, ap);
    copy3(i_this + 0x44, rr);
    ap = cSAngleCopy(f + 0x26, ang);
    relationalPosA(i_this, rr, ld(i_this + 0x128), w + 0x18, ap);
    copy3(i_this + 0x50, rr);
    st(w + 4, ld(w + 4) + 1);
    st8(i_this + 0x102, 1);
    st8(i_this + 0x100, 1);
    st8(i_this + 0x101, 1);
    return 1;
}
VERIFY(0x02535AA4, dCamera_c_gameOverEvCamera);

} // namespace d_ev_camera_7_cpp

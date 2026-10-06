/* d_ev_camera, part 9: rollingEvCamera, windDirectionEvCamera. See
 * d_ev_camera.cpp for the unit's layout notes; written from the WWHD code (the GameCube
 * d_ev_camera.cpp is all "Nonmatching" stubs). */
#include "bindings.h"

namespace d_ev_camera_9_cpp {

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
static inline u32 getEvIntData(u32 c, u32 out, u32 name, s32 def) { return gabi::call<u32>(0x02530634, c, out, name, def); }
static inline u32 getEvFloatData(u32 c, u32 out, u32 name, f32 def) { return gabi::call<u32>(0x0253072C, c, out, name, def); }
static inline u32 getEvXyzData(u32 c, u32 out, u32 name, u32 def) { return gabi::call<u32>(0x0253086C, c, out, name, def); }
static inline u32 getEvStringData(u32 c, u32 out, u32 name, u32 def) { return gabi::call<u32>(0x02530998, c, out, name, def); }
static inline u32 getEvActor(u32 c, u32 name) { return gabi::call<u32>(0x02530B84, c, name); }
static inline void relationalPos(u32 c, u32 out, u32 actor, u32 ofs) { gabi::call(0x0250242C, c, out, actor, ofs); }
static inline void cXyz_mi(u32 a, u32 out, u32 b) { gabi::call(0x0201ADE0, a, out, b); }
static inline void cXyz_pl(u32 a, u32 out, u32 b) { gabi::call(0x0201AD78, a, out, b); }
static inline void cXyz_ml(u32 a, u32 out, f32 s) { gabi::call(0x0201AE48, a, out, s); }
static inline void cSGlobe_Val(u32 g, u32 v) { gabi::call(0x020072E0, g, v); }
static inline void cSGlobe_Xyz(u32 g, u32 out) { gabi::call(0x020073AC, g, out); }
static inline void cSGlobe_ct(u32 g, u32 v) { gabi::call(0x02007324, g, v); }
static inline void cSAngle_mi(u32 a, u32 out, u32 b) { gabi::call(0x020068B0, a, out, b); }
static inline void cSAngle_pl(u32 a, u32 out, u32 b) { gabi::call(0x02006894, a, out, b); }
static inline u32 cSAngleF(u32 tmp, f32 deg) { return gabi::call<u32>(0x020066C0, tmp, deg); } /* cSAngle(f32 degrees) */
static inline u32 cSAngleS(u32 tmp, s16 v) { return gabi::call<u32>(0x0200658C, tmp, (s32)v); }
static inline void positionOf(u32 c, u32 out, u32 actor) { gabi::call(0x024F8D5C, c, out, actor); }
static inline void directionOf(u32 c, u32 out, u32 actor) { gabi::call(0x024F8F30, c, out, actor); }
static inline f32 vecLen(u32 v) { return gabi::call<f32>(0x028F4384, gabi::call<f32>(0x028E8DD0, v)); } /* sqrtf(PSVECSquareMag) */
static inline bool lineBGCheck(u32 c, u32 a, u32 b, u32 flags) { return gabi::call<bool>(0x024FCBE8, c, a, b, flags); }
static inline void negX(u32 v) { stf(v, -ldf(v)); }
static inline void setDone(u32 c) {
    st8(c + 0x102, 1);
    st8(c + 0x101, 1);
    st8(c + 0x100, 1);
}

/* relational position modes (one character of RelUseMask) as in fixedFrameEvCamera, the actor
 * re-read from its work slot: 'o' plain, 'n' the offset mirrored when the actor faces away from
 * the camera eye, 'p' mirrored when that puts the point closer to the player */
static inline void relO(u32 c, u32 actor, u32 ofs, u32 dst) {
    gabi::Local<cXyz> r;
    u32 rr = gabi::ea(r.get());
    relationalPos(c, rr, actor, ofs);
    copy3(dst, rr);
}
static inline void relN(u32 c, u32 slot, u32 ofs, u32 dst) {
    gabi::Local<cXyz> pos;
    gabi::Local<cXyz> d;
    gabi::Local<u8[8]> globe;
    gabi::Local<s16[2]> ang;
    u32 pp = gabi::ea(pos.get()), dd = gabi::ea(d.get()), gg = gabi::ea(globe.get()), aa = gabi::ea(ang.get());
    positionOf(c, pp, ld(slot));
    cXyz_mi(c + 0x1C, dd, pp);
    cSGlobe_ct(gg, dd);
    directionOf(c, aa, ld(slot));
    cSAngle_mi(gg + 6, aa + 2, aa);
    if (lds16(aa + 2) < lds16(0x101FF354)) negX(ofs); /* cSAngle::_0 */
    relationalPos(c, pp, ld(slot), ofs);
    copy3(dst, pp);
}
static inline void relP(u32 c, u32 slot, u32 ofs, u32 dst) {
    gabi::Local<cXyz> t;
    gabi::Local<cXyz> p;
    gabi::Local<cXyz> d;
    u32 tt = gabi::ea(t.get()), pp = gabi::ea(p.get()), dd = gabi::ea(d.get());
    relationalPos(c, tt, ld(slot), ofs);
    positionOf(c, pp, ld(c + 0x128));
    cXyz_mi(tt, dd, pp);
    f32 d1 = vecLen(dd);
    negX(ofs);
    relationalPos(c, pp, ld(slot), ofs);
    copy3(tt, pp);
    positionOf(c, dd, ld(c + 0x128));
    cXyz_mi(tt, pp, dd);
    f32 d2 = vecLen(pp);
    if (d1 > d2) negX(ofs); /* ble: not taken on NaN */
    relationalPos(c, pp, ld(slot), ofs);
    copy3(dst, pp);
}
/* the centre by RelUseMask[0] */
static inline void relCenter(u32 c, u32 w, u8 m) {
    if (m == 'o') relO(c, ld(w + 0x3C), w + 0x28, w + 0x10);
    else if (m == 'n') relN(c, w + 0x3C, w + 0x28, w + 0x10);
    else if (m == 'p') relP(c, w + 0x3C, w + 0x28, w + 0x10);
}

/* 025369BC: rollingEvCamera: a fixed view (Eye/Center, relative to RelActor by RelUseMask) whose
 * longitude turns by Roll degrees and radius grows by RadiusAdd per frame; TransType 1/2 follow the
 * actor (2 also pins the latitude). work: +0 timer set, +1 bank set, +4 eye, +0x10 centre, +0x1C Eye,
 * +0x28 Center, +0x34 Fovy, +0x38 Bank, +0x3C RelActor, +0x40 RelUseMask, +0x44 Timer, +0x48
 * TransType, +0x4C Roll, +0x50 RadiusAdd, +0x54 Latitude, +0x58 CtrCus */
static u32 dCamera_c_rollingEvCamera(u32 i_this) {
    WWHD_FUNC(0x025369BC, u32, i_this);
    u32 w = i_this + 0x37C;
    if (ld(i_this + 0x11C) == 0) {
        gabi::Local<cXyz> def;
        u32 d = gabi::ea(def.get());
        f32 x = ldf(i_this + 0x1C), y = ldf(i_this + 0x20);
        stf(d + 0, x);
        f32 z = ldf(i_this + 0x24);
        stf(d + 4, y);
        stf(d + 8, z);
        getEvXyzData(i_this, w + 0x1C, 0x1004D134 /* "Eye" */, d);
        x = ldf(i_this + 0x10);
        y = ldf(i_this + 0x14);
        stf(d + 0, x);
        z = ldf(i_this + 0x18);
        stf(d + 4, y);
        stf(d + 8, z);
        getEvXyzData(i_this, w + 0x28, 0x1004D140 /* "Center" */, d);
        getEvFloatData(i_this, w + 0x58, 0x1004D148 /* "CtrCus" */, 1.0f);
        getEvIntData(i_this, w + 0x48, 0x1004D16C /* "TransType" */, 0);
        getEvFloatData(i_this, w + 0x34, 0x1004D150 /* "Fovy" */, ldf(i_this + 0x38));
        st8(w + 1, (u8)getEvFloatData(i_this, w + 0x38, 0x1004D158 /* "Bank" */, 0.0f));
        getEvFloatData(i_this, w + 0x4C, 0x1004D160 /* "Roll" */, 2.0f);
        getEvFloatData(i_this, w + 0x50, 0x1004D178 /* "RadiusAdd" */, 0.0f);
        gabi::Local<u8[8]> globe;
        u32 gg = gabi::ea(globe.get());
        cXyz_mi(w + 0x1C, d, w + 0x28);
        cSGlobe_ct(gg, d);
        f32 lat = gabi::call<f32>(0x02006720, gg + 4); /* cSAngle::Degree */
        getEvFloatData(i_this, w + 0x54, 0x1004D184 /* "Latitude" */, lat);
        st8(w + 0, (u8)getEvIntData(i_this, w + 0x44, 0x1004D138 /* "Timer" */, -1));
        getEvStringData(i_this, w + 0x40, 0x1004D190 /* "RelUseMask" */, 0x1004D168 /* "oo" */);
        u32 a = getEvActor(i_this, 0x1004D19C /* "RelActor" */);
        st(w + 0x3C, a);
        if (a == 0) {
            copy3(w + 0x10, w + 0x28);
            copy3(w + 4, w + 0x1C);
        } else {
            u8 m0 = ld8(w + 0x40);
            if (m0 == 'o' || m0 == 'n' || m0 == 'p') relCenter(i_this, w, m0);
            else copy3(w + 0x10, w + 0x28);
            u8 m1 = ld8(w + 0x41);
            if (m1 == 'o') {
                relO(i_this, ld(w + 0x3C), w + 0x1C, w + 4);
            } else if (m1 == 'r') {
                if (ld(i_this + 0x80) & 1) negX(w + 0x1C);
                relO(i_this, ld(w + 0x3C), w + 0x1C, w + 4);
                if (lineBGCheck(i_this, w + 0x10, w + 4, 0x8F)) negX(w + 0x1C);
                relO(i_this, ld(w + 0x3C), w + 0x1C, w + 4);
            } else if (m1 == 'n') {
                relN(i_this, w + 0x3C, w + 0x1C, w + 4);
            } else if (m1 == 'p') {
                relP(i_this, w + 0x3C, w + 0x1C, w + 4);
            } else {
                copy3(w + 4, w + 0x1C);
            }
        }
        setDone(i_this);
    }
    u32 tt = ld(w + 0x48);
    if ((tt == 1 || tt == 2) && ld(w + 0x3C) != 0) relCenter(i_this, w, ld8(w + 0x40));
    gabi::Local<cXyz> a;
    gabi::Local<cXyz> b;
    u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get());
    cXyz_mi(w + 0x10, aa, i_this + 0x44);
    cXyz_ml(aa, bb, ldf(w + 0x58));
    gabi::call(0x028E8D88, i_this + 0x44, bb, i_this + 0x44); /* PSVECAdd: centre += gap * CtrCus */
    cXyz_mi(w + 4, aa, w + 0x10);
    cSGlobe_Val(i_this + 0x3C, aa);
    gabi::Local<s16[4]> ang;
    gabi::Local<s16[4]> tmp;
    u32 an = gabi::ea(ang.get()), tp = gabi::ea(tmp.get());
    if (ld(w + 0x48) == 2) {
        u32 lat = cSAngleF(tp + 0, ldf(w + 0x54));
        u32 r = cSAngleS(tp + 2, lds16(lat));
        st16(i_this + 0x40, (u16)lds16(r));
    }
    /* U += Roll * timer; radius += RadiusAdd * timer */
    f32 t = (f32)(f64)ld(i_this + 0x11C);
    u32 roll = cSAngleF(an + 0, t * ldf(w + 0x4C));
    cSAngle_pl(i_this + 0x42, an + 2, roll);
    u32 r = cSAngleS(an + 4, lds16(an + 2));
    f32 t2 = (f32)(f64)ld(i_this + 0x11C);
    st16(i_this + 0x42, (u16)lds16(r));
    stf(i_this + 0x3C, gabi::fmadds(t2, ldf(w + 0x50), ldf(i_this + 0x3C)));
    cSGlobe_Xyz(i_this + 0x3C, bb);
    cXyz_pl(i_this + 0x44, aa, bb);
    copy3(i_this + 0x50, aa);
    stf(i_this + 0x60, ldf(w + 0x34));
    if (ld8(w + 1) != 0) {
        u32 res = cSAngleS(an + 6, (s16)gabi::ftoi(ldf(w + 0x38) * ldf(0x1004CDE8)));
        u32 fl = ld(i_this + 0x510);
        s16 bank = lds16(res);
        st(i_this + 0x510, fl | 0x400);
        st16(i_this + 0x5C, (u16)bank);
    }
    if (ld8(w + 0) != 0 && ld(i_this + 0x11C) < ld(w + 0x44)) return 0;
    return 1;
}
VERIFY(0x025369BC, dCamera_c_rollingEvCamera);

static inline void attentionPos(u32 c, u32 out, u32 actor) { gabi::call(0x024F8F3C, c, out, actor); }
static inline u32 gi() { return gabi::call<u32>(0x025200D4); }
static inline u32 tactWindDir() { return gabi::call<u32>(0x0257E560); } /* dKyw_get_tactwind_dir */
static inline void cSAngle_ml(u32 a, u32 out, f32 f) { gabi::call(0x0200693C, a, out, f); }
static inline void f3(u32 v, f32 x, f32 y, f32 z) {
    stf(v + 0, x);
    stf(v + 4, y);
    stf(v + 8, z);
}
/* the blur and the rumble when the bird swoops in */
static inline void windBlurShock(u32 c, u32 tmp) {
    gabi::call(0x0250D464, c, 0);     /* ResetBlure */
    gabi::call(0x0250D4C0, c, 0x6E);  /* SetBlureTimer */
    gabi::call(0x0250D4C8, c, 0.6f);  /* SetBlureAlpha */
    gabi::call(0x0250D4D0, c, 0.99f); /* SetBlureScale */
    u32 g = gi();
    f3(tmp, 0.0f, 1.0f, 0.0f);
    gabi::call(0x025CB374, g + 0x599C, 7, 0x20, tmp); /* dVibration_c::StartShock */
}
/* rp = relationalPos(player, ofs) */
static inline void relPlayer(u32 c, u32 tmp, u32 ofs, u32 dst) {
    relationalPos(c, tmp, ld(c + 0x128), ofs);
    copy3(dst, tmp);
}

/* 0253775C: windDirectionEvCamera: the wind-direction event (the bird "BirdFly" actor of the event
 * flies to the player). States (work+0): 0 look from the bird to the player, 1/2/3 wait UpCount
 * frames, then follow the bird and keep it in sight; 0xA..0xF the side view ("Type" 1, or when the
 * line of sight is blocked): 0xA/0xB a fixed side view, 0xC the swing over 40 frames, 0xD..0xF the
 * close view; 0x63 hold. work: +4 frame counter, +8 the wind direction (cSGlobe), +0x10 start
 * globe, +0x18 end globe, +0x20 the bird, +0x24 eye, +0x30 centre, +0x3C Torishita, +0x48 StopDist,
 * +0x4C FollowCushion, +0x50 UpCount / frame limit, +0x54 SideFlag, +0x58 Type, +0x5C start
 * distance, +0x60 BirdFlyDist, +0x64 NearFovy, +0x68 FarFovy, +0x6C FovyCushion, +0x70/+0x74 swing
 * length / progress */
static u32 dCamera_c_windDirectionEvCamera(u32 i_this) {
    WWHD_FUNC(0x0253775C, u32, i_this);
    u32 w = i_this + 0x37C;
    gabi::Local<cXyz[6]> ofs; /* side views: centre, centre, centre, eye, eye, eye (x mirrored by the wind) */
    u32 o = gabi::ea(ofs.get());
    u32 oc0 = o + 0x00, oc1 = o + 0x0C, oc2 = o + 0x18, oe0 = o + 0x24, oe1 = o + 0x30, oe2 = o + 0x3C;
    f3(oc0, -25.0f, 12.0f, 0.0f);
    f3(oc1, -25.0f, 12.0f, 0.0f);
    f3(oc2, -25.0f, 12.0f, 0.0f);
    f3(oe0, -40.0f, -10.0f, -90.0f);
    f3(oe1, -25.0f, 0.0f, 90.0f);
    f3(oe2, -25.0f, -30.0f, 90.0f);
    f32 k = 0.02f;
    f32 ratio = 1.0f;
    if (ld(i_this + 0x11C) == 0) {
        st(w + 0, 0);
        st(w + 4, 0);
        u32 env = gabi::call<u32>(0x02555D0C); /* dKy_getEnvlight */
        gabi::call(0x02006FE4, w + 8, 1.0f, (s32)lds16(env + 0xA24), (s32)lds16(env + 0xA26)); /* cSGlobe::Val(r, V, U) */
        u32 g = gi();
        st(w + 0x20, gabi::call<u32>(0x0253EE04, g + 0x51D0, ld(g + 0x52A0))); /* dEvt_control_c::convPId */
        getEvFloatData(i_this, w + 0x60, 0x1004D20C /* "BirdFlyDist" */, 1620.0f);
        gabi::Local<cXyz> def;
        u32 d = gabi::ea(def.get());
        f3(d, 40.0f, -95.0f, 10.0f);
        getEvXyzData(i_this, w + 0x3C, 0x1004D224 /* "Torishita" */, d);
        if (tactWindDir() != 0) negX(w + 0x3C);
        gabi::Local<u8[8]> gl;
        u32 gg = gabi::ea(gl.get());
        u32 inv = gabi::call<u32>(0x02007400, w + 8); /* cSGlobe::Invert */
        gabi::call(0x02007178, gg, inv);              /* cSGlobe(const cSGlobe&) */
        stf(gg, ldf(w + 0x60));
        gabi::Local<cXyz> mir;
        gabi::Local<cXyz> at;
        gabi::Local<cXyz> pr;
        gabi::Local<cXyz> br;
        u32 mm = gabi::ea(mir.get()), aa = gabi::ea(at.get()), pp = gabi::ea(pr.get()), bb = gabi::ea(br.get());
        f3(mm, -ldf(w + 0x3C), 0.0f, 0.0f);
        attentionPos(i_this, aa, ld(i_this + 0x128));
        relationalPos(i_this, pp, ld(i_this + 0x128), mm);
        relationalPos(i_this, bb, ld(w + 0x20), w + 0x3C);
        u32 blocked = 0;
        if (lineBGCheck(i_this, aa, bb, 0x7F) || lineBGCheck(i_this, pp, bb, 0x7F)) blocked = 1;
        getEvFloatData(i_this, w + 0x48, 0x1004D240 /* "StopDist" */, 155.0f);
        getEvIntData(i_this, w + 0x50, 0x1004D1F4 /* "UpCount" */, 8);
        getEvIntData(i_this, w + 0x54, 0x1004D24C /* "SideFlag" */, 0);
        getEvFloatData(i_this, w + 0x4C, 0x1004D230 /* "FollowCushion" */, k);
        getEvFloatData(i_this, w + 0x64, 0x1004D258 /* "NearFovy" */, 30.0f);
        getEvFloatData(i_this, w + 0x68, 0x1004D1FC /* "FarFovy" */, 85.0f);
        getEvFloatData(i_this, w + 0x6C, 0x1004D218 /* "FovyCushion" */, 0.05f);
        positionOf(i_this, aa, ld(w + 0x20));
        positionOf(i_this, pp, ld(i_this + 0x128));
        cXyz_mi(aa, d, pp);
        stf(w + 0x5C, vecLen(d));
        stf(i_this + 0x60, ldf(w + 0x68));
        getEvIntData(i_this, w + 0x58, 0x1004D204 /* "Type" */, (s32)blocked);
        if (ld(w + 0x58) == 1) st(w + 0, 0xA);
    }
    if (tactWindDir() != 0) {
        for (u32 i = 0; i < 6; i++) negX(o + i * 12);
    }
    gabi::Local<cXyz> t1;
    gabi::Local<cXyz> t2;
    gabi::Local<cXyz> t3;
    u32 a1 = gabi::ea(t1.get()), a2 = gabi::ea(t2.get()), a3 = gabi::ea(t3.get());
    u32 st0 = ld(w + 0);
    switch (st0) {
    case 1:
        if ((s32)ld(w + 4) > (s32)ld(w + 0x50)) st(w + 0, 2);
        break;
    case 2:
        copy3(w + 0x30, i_this + 0x44);
        st(w + 0, 3);
        /* fall through */
    case 3: {
        attentionPos(i_this, a1, ld(w + 0x20));
        cXyz_mi(a1, a2, w + 0x30);
        cXyz_ml(a2, a3, k);
        cXyz_pl(w + 0x30, a2, a3);
        copy3(w + 0x30, a2);
        gabi::Local<u8[0x20]> line;
        gabi::Local<cXyz> pa;
        gabi::Local<cXyz> near;
        gabi::Local<f32> tp;
        u32 ln = gabi::ea(line.get()), pa_ = gabi::ea(pa.get()), nr = gabi::ea(near.get()), tt = gabi::ea(tp.get());
        gabi::call(0x02018780, ln, w + 0x30, i_this + 0x50); /* cM3dGLin(start, end) */
        attentionPos(i_this, pa_, ld(i_this + 0x128));
        if (gabi::call<bool>(0x02010AE4, ln, pa_, nr, tt)) copy3(i_this + 0x44, nr); /* cM3d_Len3dSqPntAndSegLine */
        else copy3(i_this + 0x44, w + 0x30);
        s32 pad = (s32)ld(i_this + 0x124);
        u32 g = gi();
        if (ld(g + 0x5CD8 + (u32)(pad << 4)) & 0x10000) {
            gabi::call(0x024FC0B0, i_this, a3, ld(i_this + 0x128)); /* eyePos */
            if (ldf(i_this + 0x54) < ldf(a3 + 4)) { /* bge: taken on NaN */
                gabi::call(0x024FC0B0, i_this, a2, ld(i_this + 0x128));
                f32 ey = ldf(i_this + 0x54);
                stf(i_this + 0x54, gabi::fmadds(ldf(a2 + 4) - ey, 0.05f, ey));
            }
        }
        cXyz_mi(i_this + 0x50, a1, i_this + 0x44);
        cSGlobe_Val(i_this + 0x3C, a1);
        break;
    }
    case 0xA:
        relPlayer(i_this, a1, oc0, i_this + 0x44);
        relPlayer(i_this, a1, oe0, i_this + 0x50);
        cXyz_mi(i_this + 0x50, a1, i_this + 0x44);
        cSGlobe_Val(i_this + 0x3C, a1);
        st(w + 0x50, 0x1C);
        windBlurShock(i_this, a1);
        /* fall through */
    case 0xB:
        if ((s32)ld(w + 4) > (s32)ld(w + 0x50)) {
            st(w + 0, 0xC);
            relPlayer(i_this, a1, oc1, w + 0x30);
            relPlayer(i_this, a1, oe1, w + 0x24);
            cXyz_mi(w + 0x24, a1, w + 0x30);
            cSGlobe_Val(w + 0x18, a1);
            st(w + 0x10, ld(i_this + 0x3C));
            stf(w + 0x74, 0.0f);
            st(w + 0x50, 0x28);
            st(w + 0x14, ld(i_this + 0x40));
            stf(w + 0x70, (f32)(f64)(s32)0x334);
            st(w + 4, 0);
        }
        break;
    case 0xC:
        if ((s32)ld(w + 4) < (s32)ld(w + 0x50)) {
            f32 p = ldf(w + 0x74) + (f32)(f64)(s32)ld(w + 4);
            f32 r = p / ldf(w + 0x70);
            stf(w + 0x74, p);
            f32 r0 = ldf(w + 0x10);
            stf(i_this + 0x3C, gabi::fmadds(ldf(w + 0x18) - r0, r, r0));
            gabi::Local<s16[4]> an;
            gabi::Local<s16[2]> ct;
            u32 ap = gabi::ea(an.get()), cp = gabi::ea(ct.get());
            cSAngle_mi(w + 0x1C, ap + 4, w + 0x14);
            cSAngle_ml(ap + 4, ap + 2, r);
            cSAngle_pl(w + 0x14, ap + 0, ap + 2);
            u32 res = cSAngleS(ap + 6, lds16(ap + 0));
            st16(i_this + 0x40, (u16)lds16(res));
            cSAngle_mi(w + 0x1E, ap + 0, w + 0x16);
            cSAngle_ml(ap + 0, ap + 2, r);
            cSAngle_pl(w + 0x16, ap + 4, ap + 2);
            res = cSAngleS(cp, lds16(ap + 4));
            st16(i_this + 0x42, (u16)lds16(res));
            copy3(i_this + 0x44, w + 0x30);
            cSGlobe_Xyz(i_this + 0x3C, a1);
            cXyz_pl(i_this + 0x44, a2, a1);
            copy3(i_this + 0x50, a2);
            break;
        }
        /* fall through */
    case 0xD:
        st(w + 0x50, 0);
        st(w + 4, 0);
        st(w + 0, 0xE);
        if (0 > (s32)ld(w + 0x50)) st(w + 0, 0xF);
        relPlayer(i_this, a1, oc2, w + 0x30);
        relPlayer(i_this, a1, oe2, w + 0x24);
        break;
    case 0xE:
        if ((s32)ld(w + 4) > (s32)ld(w + 0x50)) st(w + 0, 0xF);
        relPlayer(i_this, a1, oc2, w + 0x30);
        relPlayer(i_this, a1, oe2, w + 0x24);
        break;
    case 0xF:
        cXyz_mi(w + 0x30, a1, i_this + 0x44);
        cXyz_ml(a1, a2, k);
        gabi::call(0x028E8D88, i_this + 0x44, a2, i_this + 0x44); /* PSVECAdd */
        cXyz_mi(w + 0x24, a2, i_this + 0x50);
        cXyz_ml(a2, a1, k);
        gabi::call(0x028E8D88, i_this + 0x50, a1, i_this + 0x50);
        cXyz_mi(i_this + 0x50, a1, i_this + 0x44);
        cSGlobe_Val(i_this + 0x3C, a1);
        break;
    case 0x63:
        break;
    default: {
        attentionPos(i_this, a1, ld(i_this + 0x128));
        copy3(i_this + 0x44, a1);
        relationalPos(i_this, a1, ld(w + 0x20), w + 0x3C);
        copy3(i_this + 0x50, a1);
        f32 hd = gabi::call<f32>(0x024F7AC4, i_this + 0x44, i_this + 0x50); /* dCamMath::xyzHorizontalDistance */
        windBlurShock(i_this, a1);
        if (hd < ldf(w + 0x48)) { /* bge: taken on NaN */
            st(w + 4, 0);
            st(w + 0, 1);
        }
        break;
    }
    }
    if (ld(w + 0x58) != 0) {
        st(i_this + 0x41C, 1);
        stf(i_this + 0x60, 82.0f);
    } else {
        positionOf(i_this, a1, ld(w + 0x20));
        positionOf(i_this, a2, ld(i_this + 0x128));
        cXyz_mi(a1, a3, a2);
        f32 dist = vecLen(a3);
        f32 near = ldf(w + 0x48);
        f32 f;
        if (dist < near) { /* bge: taken on NaN */
            f32 n = ldf(w + 0x64);
            f = gabi::fmadds(ldf(w + 0x68) - n, 0.0f, n);
        } else {
            f32 far = ldf(w + 0x5C);
            if (!(dist > far)) ratio = (dist - near) / (far - near); /* bgt: not taken on NaN */
            f32 n = ldf(w + 0x64);
            f = gabi::fmadds(ldf(w + 0x68) - n, ratio, n);
        }
        f32 fv = ldf(i_this + 0x60);
        stf(i_this + 0x60, gabi::fmadds(f - fv, ldf(w + 0x6C), fv));
    }
    st(w + 4, ld(w + 4) + 1);
    setDone(i_this);
    return 1;
}
VERIFY(0x0253775C, dCamera_c_windDirectionEvCamera);

} // namespace d_ev_camera_9_cpp

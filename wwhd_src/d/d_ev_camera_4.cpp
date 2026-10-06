/* d_ev_camera, part 4: event cameras on point lists (fixedFrames, bSpline). See d_ev_camera.cpp for the unit's layout notes; written from the WWHD
 * code (the GameCube d_ev_camera.cpp is all "Nonmatching" stubs). */
#include "bindings.h"

namespace d_ev_camera_4_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
static inline void copy3(u32 d, u32 s) {
    st(d + 0, ld(s + 0));
    st(d + 4, ld(s + 4));
    st(d + 8, ld(s + 8));
}
static inline u32 gi() { return gabi::call<u32>(0x025200D4); }
static inline s32 substanceNum(u32 c, u32 name) {
    s32 staff = (s32)ld(c + 0x400);
    return gabi::call<s32>(0x025448CC, gi() + 0x52C4, staff, name);
}
static inline u32 substanceP(u32 c, u32 name, s32 type) {
    s32 staff = (s32)ld(c + 0x400);
    return gabi::call<u32>(0x0254487C, gi() + 0x52C4, staff, name, type);
}
static inline u32 JUT_ASSERT_l(u32 file, s32 line, u32 msg) { return gabi::call<u32>(0x0273AA24, file, line, msg); }
static inline u32 getEvIntData2(u32 c, u32 out, u32 name) { return gabi::call<u32>(0x02530418, c, out, name); }
static inline u32 getEvIntData(u32 c, u32 out, u32 name, s32 def) { return gabi::call<u32>(0x02530634, c, out, name, def); }
static inline u32 getEvStringData(u32 c, u32 out, u32 name, u32 def) { return gabi::call<u32>(0x02530998, c, out, name, def); }
static inline u32 getEvActor(u32 c, u32 name) { return gabi::call<u32>(0x02530B84, c, name); }
static inline void relationalPos(u32 c, u32 out, u32 actor, u32 ofs) { gabi::call(0x0250242C, c, out, actor, ofs); }
static inline bool lineBGCheck(u32 c, u32 a, u32 b, u32 flags) { return gabi::call<bool>(0x024FCBE8, c, a, b, flags); }
static inline void setDone(u32 c) {
    st8(c + 0x102, 1);
    st8(c + 0x101, 1);
    st8(c + 0x100, 1);
}
static inline void setViewFromWork(u32 c, u32 ctr, u32 eye, u32 fovy) {
    gabi::Local<cXyz> d;
    u32 dd = gabi::ea(d.get());
    copy3(c + 0x44, ctr);
    copy3(c + 0x50, eye);
    stf(c + 0x60, ldf(fovy));
    gabi::call(0x0201ADE0, c + 0x50, dd, c + 0x44); /* cXyz::operator- */
    gabi::call(0x020072E0, c + 0x3C, dd);           /* cSGlobe::Val */
}

/* 0253A268: fixedFramesEvCamera: the first of the listed frames (Centers/Eyes/Fovys) whose line of
 * sight is clear. work: +0 timer set, +4 centre, +0x10 eye, +0x1C Eyes, +0x20 Centers, +0x24 Fovys,
 * +0x28 fovy, +0x2C RelActor, +0x30 RelUseMask, +0x34 Timer, +0x38 frame count */
static u32 dCamera_c_fixedFramesEvCamera(u32 i_this) {
    WWHD_FUNC(0x0253A268, u32, i_this);
    u32 w = i_this + 0x37C;
    if (ld(i_this + 0x11C) == 0) {
        st(w + 0x38, 9999);
        s32 n = substanceNum(i_this, 0x1004D3D4 /* "Centers" */);
        if (n == 0) return 1;
        u32 p = substanceP(i_this, 0x1004D3D4, 1);
        s32 m = (s32)ld(w + 0x38);
        st(w + 0x20, p);
        if (m > n) st(w + 0x38, (u32)n);
        n = substanceNum(i_this, 0x1004D3EC /* "Eyes" */);
        if (n == 0) return 1;
        p = substanceP(i_this, 0x1004D3EC, 1);
        m = (s32)ld(w + 0x38);
        st(w + 0x1C, p);
        if (m > n) st(w + 0x38, (u32)n);
        n = substanceNum(i_this, 0x1004D3DC /* "Fovys" */);
        if (n == 0) return 1;
        p = substanceP(i_this, 0x1004D3DC, 0);
        m = (s32)ld(w + 0x38);
        st(w + 0x24, p);
        if (m > n) st(w + 0x38, (u32)n);
        st8(w + 0, (u8)getEvIntData(i_this, w + 0x34, 0x1004D3E4 /* "Timer" */, 1));
        getEvStringData(i_this, w + 0x30, 0x1004D438 /* "RelUseMask" */, 0x1004D3F4 /* "oo" */);
        u32 a = getEvActor(i_this, 0x1004D444 /* "RelActor" */);
        u32 cs = ld(w + 0x20);
        st(w + 0x2C, a);
        if (cs == 0) JUT_ASSERT_l(0x1004D3F8, 0xF06, 0x1004D408);
        if (ld(w + 0x1C) == 0) JUT_ASSERT_l(0x1004D3F8, 0xF07, 0x1004D418);
        if (ld(w + 0x24) == 0) JUT_ASSERT_l(0x1004D3F8, 0xF08, 0x1004D428);
        gabi::Local<cXyz> ctr;
        gabi::Local<cXyz> eye;
        gabi::Local<cXyz> r;
        gabi::Local<cXyz> a1;
        gabi::Local<cXyz> a2;
        u32 cc = gabi::ea(ctr.get()), ee = gabi::ea(eye.get()), rr = gabi::ea(r.get());
        u32 p1 = gabi::ea(a1.get()), p2 = gabi::ea(a2.get());
        for (s32 i = 0; i < (s32)ld(w + 0x38); i++) {
            copy3(cc, ld(w + 0x20) + (u32)i * 12);
            copy3(ee, ld(w + 0x1C) + (u32)i * 12);
            u32 act = ld(w + 0x2C);
            if (act != 0 && ld8(w + 0x30) == 'o') {
                relationalPos(i_this, rr, act, cc);
                copy3(w + 4, rr);
            } else {
                copy3(w + 4, cc);
            }
            act = ld(w + 0x2C);
            bool hit;
            if (act != 0 && ld8(w + 0x31) == 'o') {
                relationalPos(i_this, rr, act, ee);
                copy3(w + 0x10, rr);
            } else {
                copy3(w + 0x10, ee);
            }
            stf(w + 0x28, ldf(ld(w + 0x24) + (u32)i * 4));
            hit = lineBGCheck(i_this, w + 4, w + 0x10, 0x8F);
            if (hit) continue;
            for (u32 k = 0; k < 3; k++) stf(p1 + 4 * k, ldf(w + 4 + 4 * k));
            for (u32 k = 0; k < 3; k++) stf(p2 + 4 * k, ldf(w + 0x10 + 4 * k));
            u32 pl = ld(i_this + 0x128);
            u32 ra = ld(w + 0x2C);
            u32 g = gi();
            if (!gabi::call<bool>(0x025187D8, g + 0x26A4, p1, p2, ldf(0x1004CF80), pl, ra)) break; /* dCcS::ChkCamera */
        }
        setDone(i_this);
    }
    setViewFromWork(i_this, w + 4, w + 0x10, w + 0x28);
    if (ld8(w + 0) != 0 && ld(i_this + 0x11C) < ld(w + 0x34)) return 0;
    return 1;
}
VERIFY(0x0253A268, dCamera_c_fixedFramesEvCamera);

/* 0253A704: bSplineEvCamera: a d2DBSplinePath (this + 0x4CC) through Centers/Eyes/Fovys.
 * work: +0 Centers, +4 Eyes, +8 Fovys, +0xC Timer, +0x10 point count */
static u32 dCamera_c_bSplineEvCamera(u32 i_this) {
    WWHD_FUNC(0x0253A704, u32, i_this);
    u32 w = i_this + 0x37C;
    if (ld(i_this + 0x11C) == 0) {
        st(w + 0x10, 9999);
        s32 n = substanceNum(i_this, 0x1004D450 /* "Centers" */);
        if (n == 0) return 1;
        u32 p = substanceP(i_this, 0x1004D450, 1);
        st(w + 0, p);
        if (p == 0) JUT_ASSERT_l(0x1004D470, 0xF3E, 0x1004D480);
        if ((s32)ld(w + 0x10) > n) st(w + 0x10, (u32)n);
        n = substanceNum(i_this, 0x1004D468 /* "Eyes" */);
        if (n == 0) return 1;
        p = substanceP(i_this, 0x1004D468, 1);
        st(w + 4, p);
        if (p == 0) JUT_ASSERT_l(0x1004D470, 0xF48, 0x1004D490);
        if ((s32)ld(w + 0x10) > n) st(w + 0x10, (u32)n);
        n = substanceNum(i_this, 0x1004D458 /* "Fovys" */);
        if (n == 0) return 1;
        p = substanceP(i_this, 0x1004D458, 0);
        st(w + 8, p);
        if (p == 0) JUT_ASSERT_l(0x1004D470, 0xF52, 0x1004D4A0);
        if ((s32)ld(w + 0x10) > n) st(w + 0x10, (u32)n);
        if (getEvIntData2(i_this, w + 0xC, 0x1004D460 /* "Timer" */) == 0) return 1;
        gabi::call(0x025C0B18, i_this + 0x4CC, ld(w + 0x10), ld(w + 0xC)); /* d2DBSplinePath::Init */
        setDone(i_this);
    }
    if (gabi::call<u32>(0x025C0C80, i_this + 0x4CC) == 0) return 1; /* d2DBSplinePath::Step */
    gabi::Local<cXyz> v;
    u32 vv = gabi::ea(v.get());
    gabi::call(0x025C0E38, i_this + 0x4CC, vv, ld(w + 0)); /* Calc(cXyz*) */
    copy3(i_this + 0x44, vv);
    gabi::call(0x025C0E38, i_this + 0x4CC, vv, ld(w + 4));
    copy3(i_this + 0x50, vv);
    f32 fovy = gabi::call<f32>(0x025C0F30, i_this + 0x4CC, ld(w + 8)); /* Calc(f32*) */
    stf(i_this + 0x60, fovy);
    gabi::call(0x0201ADE0, i_this + 0x50, vv, i_this + 0x44);
    gabi::call(0x020072E0, i_this + 0x3C, vv);
    return ld(i_this + 0x4D8) == 3 ? 1 : 0;
}
VERIFY(0x0253A704, dCamera_c_bSplineEvCamera);

} // namespace d_ev_camera_4_cpp

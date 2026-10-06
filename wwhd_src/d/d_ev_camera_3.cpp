/* d_ev_camera, part 3: style / save / load event cameras. See d_ev_camera.cpp for the unit's layout notes; written from the WWHD
 * code (the GameCube d_ev_camera.cpp is all "Nonmatching" stubs). */
#include "bindings.h"

namespace d_ev_camera_3_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline s16 lds16(u32 a) { return gabi::load<s16>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
static inline u32 gi() { return gabi::call<u32>(0x025200D4); }
static inline u32 getEvIntData(u32 c, u32 out, u32 name, s32 def) { return gabi::call<u32>(0x02530634, c, out, name, def); }
static inline u32 getEvStringPntData(u32 c, u32 name, u32 def) { return gabi::call<u32>(0x02530AAC, c, name, def); }
static inline void setDone(u32 c) {
    st8(c + 0x102, 1);
    st8(c + 0x101, 1);
    st8(c + 0x100, 1);
}
static inline u8 isDone(u32 c) {
    u8 r = 0;
    if (ld8(c + 0x100) != 0 && ld8(c + 0x101) != 0 && ld8(c + 0x102) != 0) r = 1;
    return r;
}

/* run the camera engine of a style: the engine table 101D5678 holds pointer-to-member entries
 * {this delta, vtable index (< 0: direct), function or vtable offset}, by the style's algorithm */
static inline void runStyleEngine(u32 c, s32 style) {
    u32 alg = ld(0x10044880 + (u32)style * 0x84);
    u32 e = 0x101D5678 + (alg << 3);
    s16 idx = lds16(e + 2);
    u32 obj = c + (u32)(s32)lds16(e + 0);
    u32 fn;
    if (idx < 0) fn = ld(e + 4);
    else fn = ld(ld(obj + (u32)(s32)lds16(e + 6)) + ((u32)(s32)idx << 3) + 4);
    gabi::call_ptr(fn, obj, style);
}
static inline void relationalPos(u32 c, u32 out, u32 actor, u32 ofs) { gabi::call(0x0250242C, c, out, actor, ofs); }
static inline void copy3(u32 d, u32 s) {
    st(d + 0, ld(s + 0));
    st(d + 4, ld(s + 4));
    st(d + 8, ld(s + 8));
}

/* 02535830: maptoolIdEvCamera: the stage's event camera list (dStage_Event_dt entries 0x18:
 * type 0x10, frames/10 0x11, flags 0x12, room 0x14), from the map tool id "ID" or the event's own */
static u32 dCamera_c_maptoolIdEvCamera(u32 i_this) {
    WWHD_FUNC(0x02535830, u32, i_this);
    u32 d;
    if (ld(i_this + 0x108) != 0) {
        d = ld(i_this + 0x4C8);
        if (d == 0) return 1;
    } else {
        gabi::Local<s32> idl;
        u32 ip = gabi::ea(idl.get());
        getEvIntData(i_this, ip, 0x1004D0B0 /* "ID" */, -1);
        s32 id = (s32)ld(ip);
        st(i_this + 0x404, 0);
        st(i_this + 0x11C, 0);
        u32 g = gi();
        if (id == -1) {
            d = gabi::call<u32>(0x02540348, g + 0x12A0 + 0x3F30); /* dEvt_control_c::getStageEventDt */
            st(i_this + 0x4C8, d);
        } else {
            u32 stg = g + 0x12A0 + 0x3EB0;
            u32 info = gabi::call_ptr<u32>(ld(ld(stg) + 0x1CC), stg);
            if (id >= 0 && id < (s32)ld(info + 0)) d = ld(info + 4) + (u32)id * 0x18;
            else d = 0;
            st(i_this + 0x4C8, d);
        }
        if (d == 0) return 1;
    }
    s32 room = (s8)ld8(d + 0x14);
    u32 fl = ld8(d + 0x12);
    u32 snd = 0xFFFFFFFF;
    u32 type = ld8(d + 0x10);
    if (fl != 0xFF) {
        if (fl & 1) {
            st(i_this + 0x510, ld(i_this + 0x510) & ~0x200000u);
            d = ld(i_this + 0x4C8);
            fl = ld8(d + 0x12);
        }
        if (fl & 2) {
            d = ld(i_this + 0x4C8);
            st(i_this + 0x68, 0);
            fl = ld8(d + 0x12);
        }
        if (fl & 0x80) snd = 0;
        if (fl & 0x40) snd = 0x1E;
    }
    if (ld(i_this + 0x108) == snd) gabi::call(0x025E1988, 0x806); /* mDoAud_seStart */
    s32 camType = gabi::call<s32>(0x024FA208, i_this, type, room); /* GetCameraTypeFromMapToolID */
    st(i_this + 0x408, (u32)camType);
    if (camType == 0xFF) {
        st(i_this + 0x4C8, 0);
        return 1;
    }
    s32 style = lds16(0x10049368 + ((u32)camType << 6)); /* types[type].mStyles[0] */
    runStyleEngine(i_this, style);
    d = ld(i_this + 0x4C8);
    u32 t = ld8(d + 0x11);
    if (t != 0xFF && ld(i_this + 0x11C) <= t * 10) return d == 0 ? 1 : 0;
    u32 g = gi();
    u32 n = gabi::call<u32>(0x025403D4, g + 0x51D0, d); /* dEvt_control_c::nextStageEventDt */
    st8(i_this + 0x100, 0);
    st8(i_this + 0x102, 0);
    st(i_this + 0x4C8, n);
    st8(i_this + 0x101, 0);
    if (n == 0) return 1;
    d = ld(i_this + 0x4C8);
    st(i_this + 0x11C, 0xFFFFFFFF);
    return d == 0 ? 1 : 0;
}
VERIFY(0x02535830, dCamera_c_maptoolIdEvCamera);

/* 0253754C: tactEvCamera (conducting the Wind Waker): fixed offsets around the player; the view is
 * saved to the play's slot first */
static u32 dCamera_c_tactEvCamera(u32 i_this) {
    WWHD_FUNC(0x0253754C, u32, i_this);
    u32 w = i_this + 0x37C;
    if (ld(i_this + 0x11C) == 0) {
        st(w + 4, 0);
        st(w + 8, ((ld(i_this + 0x7C) >> 1) & 1) ^ 1);
        f32 fovy = ldf(i_this + 0x60);
        st(i_this + 0x41C, 0);
        setDone(i_this);
        s16 bank = lds16(i_this + 0x5C);
        u32 g = gi();
        st(g + 0x5B0C, ld(i_this + 0x44));
        st(g + 0x5B10, ld(i_this + 0x48));
        st(g + 0x5B14, ld(i_this + 0x4C));
        st(g + 0x5B18, ld(i_this + 0x50));
        st(g + 0x5B1C, ld(i_this + 0x54));
        st(g + 0x5B20, ld(i_this + 0x58));
        st16(g + 0x5B28, (u16)bank);
        stf(g + 0x5B24, fovy);
    }
    gabi::Local<cXyz> ctrOfs;
    gabi::Local<cXyz> eyeOfs;
    gabi::Local<cXyz> r;
    gabi::Local<cXyz> r2;
    u32 co = gabi::ea(ctrOfs.get()), eo = gabi::ea(eyeOfs.get()), rr = gabi::ea(r.get()), r2r = gabi::ea(r2.get());
    stf(co + 4, ldf(0x1004D1AC));
    stf(co + 0, ldf(0x1004D1A8));
    stf(co + 8, ldf(0x1004D1B0));
    stf(eo + 4, ldf(0x1004D1B8));
    stf(eo + 8, ldf(0x1004D1BC));
    stf(eo + 0, ldf(0x1004D1B4));
    if (ld(w + 0) != 1) st(w + 0, 1);
    relationalPos(i_this, rr, ld(i_this + 0x128), co);
    copy3(i_this + 0x44, rr);
    u32 mirror = ld(w + 8);
    u32 pl = ld(i_this + 0x128);
    if (mirror != 0) stf(eo, -ldf(eo));
    relationalPos(i_this, rr, pl, eo);
    copy3(i_this + 0x50, rr);
    if (gabi::call<bool>(0x024FCBE8, i_this, i_this + 0x44, i_this + 0x50, 0x8F)) { /* lineBGCheck */
        stf(eo, -ldf(eo));
        relationalPos(i_this, r2r, ld(i_this + 0x128), eo);
        copy3(i_this + 0x50, r2r);
    }
    stf(i_this + 0x60, ldf(0x1004D1C0)); /* 55 */
    st(w + 4, ld(w + 4) + 1);
    return 1;
}
VERIFY(0x0253754C, dCamera_c_tactEvCamera);

/* 02538A54: styleEvCamera: run the engine of the style named by "Name" (default "FN01") through
 * the engine table 101D5678 (pointer-to-member entries {this delta, vtable index, function or
 * vtable offset}, by the style's algorithm) */
static u8 dCamera_c_styleEvCamera(u32 i_this) {
    WWHD_FUNC(0x02538A54, u8, i_this);
    if (ld(i_this + 0x108) == 0) {
        st(i_this + 0x11C, 0);
        st(i_this + 0x404, 0);
    }
    u32 name = getEvStringPntData(i_this, 0x1004D288 /* "Name" */, 0x1004D290 /* "FN01" */);
    s32 style = gabi::call<s32>(0x024F732C, i_this + 0x8A4, ld(name)); /* dCamParam_c::SearchStyle */
    runStyleEngine(i_this, style);
    return isDone(i_this);
}
VERIFY(0x02538A54, dCamera_c_styleEvCamera);

/* 02538B54: saveEvCamera: the current view into slot "Slot" (0..1: m0A4[], 9: the play's slot) */
static u32 dCamera_c_saveEvCamera(u32 i_this) {
    WWHD_FUNC(0x02538B54, u32, i_this);
    gabi::Local<s32> slot;
    u32 sp = gabi::ea(slot.get());
    getEvIntData(i_this, sp, 0x1004D298 /* "Slot" */, 0);
    s32 n = (s32)ld(sp);
    if (n == 9) {
        f32 fovy = ldf(i_this + 0x60);
        s16 bank = lds16(i_this + 0x5C);
        u32 g = gi();
        st(g + 0x5B0C, ld(i_this + 0x44));
        st(g + 0x5B10, ld(i_this + 0x48));
        st(g + 0x5B14, ld(i_this + 0x4C));
        st(g + 0x5B18, ld(i_this + 0x50));
        st(g + 0x5B1C, ld(i_this + 0x54));
        st(g + 0x5B20, ld(i_this + 0x58));
        stf(g + 0x5B24, fovy);
        st16(g + 0x5B28, (u16)bank);
        setDone(i_this);
        return 1;
    }
    u32 p = i_this + 0xA4 + ((u32)n << 5);
    for (u32 i = 0; i < 0x18; i += 4) st(p + i, ld(i_this + 0x44 + i));
    stf(p + 0x18, ldf(i_this + 0x60));
    s16 bank = lds16(i_this + 0x5C);
    st16(p + 0x1E, 1);
    st16(p + 0x1C, (u16)bank);
    setDone(i_this);
    return 1;
}
VERIFY(0x02538B54, dCamera_c_saveEvCamera);

/* 02538C7C: loadEvCamera */
static u32 dCamera_c_loadEvCamera(u32 i_this) {
    WWHD_FUNC(0x02538C7C, u32, i_this);
    gabi::Local<s32> slot;
    u32 sp = gabi::ea(slot.get());
    getEvIntData(i_this, sp, 0x1004D2A0 /* "Slot" */, 0);
    s32 n = (s32)ld(sp);
    if (n == 9) {
        u32 g = gi();
        st(i_this + 0x44, ld(g + 0x5B0C));
        st(i_this + 0x48, ld(g + 0x5B10));
        st(i_this + 0x4C, ld(g + 0x5B14));
        st(i_this + 0x50, ld(g + 0x5B18));
        st(i_this + 0x54, ld(g + 0x5B1C));
        st(i_this + 0x58, ld(g + 0x5B20));
        stf(i_this + 0x60, ldf(g + 0x5B24));
        gabi::Local<s16> ang;
        u32 res = gabi::call<u32>(0x0200658C, gabi::ea(ang.get()), (s32)lds16(g + 0x5B28));
        st16(i_this + 0x5C, (u16)lds16(res));
        setDone(i_this);
        return 1;
    }
    u32 p = i_this + 0xA4 + ((u32)n << 5);
    for (u32 i = 0; i < 0x18; i += 4) st(i_this + 0x44 + i, ld(p + i));
    stf(i_this + 0x60, ldf(p + 0x18));
    st16(i_this + 0x5C, (u16)lds16(p + 0x1C));
    gabi::Local<cXyz> d;
    u32 dd = gabi::ea(d.get());
    gabi::call(0x0201ADE0, i_this + 0x50, dd, i_this + 0x44);
    gabi::call(0x020072E0, i_this + 0x3C, dd);
    setDone(i_this);
    return 1;
}
VERIFY(0x02538C7C, dCamera_c_loadEvCamera);

} // namespace d_ev_camera_3_cpp

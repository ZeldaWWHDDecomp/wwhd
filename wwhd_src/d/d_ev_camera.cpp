/* d_ev_camera: event camera (dCamera_c::*EvCamera and the event parameter helpers), WWHD.
 *
 * Translation unit 025303B4..0253E948, from the image: d_envse ends with its __sinit 02530320;
 * this unit starts with searchEventArgData (025303B4; its assert file string "d_ev_camera.cpp"
 * is 1004CCB8), follows the GameCube order of d_ev_camera.cpp only loosely (the GameCube file is
 * all "Nonmatching" stubs: everything here is written from the WWHD code), and ends with its
 * __sinit 0253E89C and the per-TU inline copies 0253E930 (deleting destructor) and 0253E944
 * (empty virtual); d_event starts at 0253E948. Part 1 (this file): the parameter helpers,
 * getEvActor, pause/talkto, Start/EndEventCamera, the __sinit and the inline copies; the camera
 * engines are in d_ev_camera_*.cpp.
 *
 * dCamera_c (d/d_camera.h, HD 0x8E0): event data at 0x3FC (staff index 0x400, the "no data"
 * flag 0x40C, Start's ids 0x410/0x414, debug parameters 0x428: 8 x {name[16], value pointer}),
 * event flags 0x510 (bit 0x20000000: the event camera was started with explicit parameters,
 * which then come from the debug table instead of the event manager), the engine timer 0x11C,
 * the "done" flags 0x100..0x102, the work area 0x37C and the HD flag 0x610. */
#include "bindings.h"

namespace d_ev_camera_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 JUT_ASSERT_l(u32 file, s32 line, u32 msg) { return gabi::call<u32>(0x0273AA24, file, line, msg); }
static inline u32 gi() { return gabi::call<u32>(0x025200D4); }
static inline s32 evmng_getMySubstanceNum_l(s32 staff, u32 name) { return gabi::call<s32>(0x025448CC, gi() + 0x52C4, staff, name); }
static inline u32 evmng_getMySubstanceP_l(s32 staff, u32 name, s32 type) {
    u32 m = gi() + 0x52C4;
    return gabi::call<u32>(0x0254487C, m, staff, name, type);
}
static inline void strcpy_l(u32 d, u32 s) { gabi::call(0x028F040C, d, s); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline s16 lds16(u32 a) { return gabi::load<s16>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }

enum : u32 { EV_STAFF = 0x400, EV_NODATA = 0x40C, EV_ID1 = 0x410, EV_ID2 = 0x414, EV_PARAMS = 0x428, EV_FLAGS = 0x510,
             EV_TIMER = 0x11C, EV_WORK = 0x37C };
static inline bool debugParams(u32 c) { return (ld(c + EV_FLAGS) >> 29) & 1; }
static inline u32 paramValue(u32 c, s32 i) { return ld(c + EV_PARAMS + i * 0x14 + 0x10); }

/* 025303B4: the index of a debug parameter by name, -1 when absent (an empty name ends the list) */
static s32 dCamera_c_searchEventArgData(u32 i_this, u32 i_name) {
    WWHD_FUNC(0x025303B4, s32, i_this, i_name);
    for (s32 i = 0; i < 8; i++) {
        u32 n = i_this + EV_PARAMS + i * 0x14;
        if (ld8(n) == 0) return -1;
        for (u32 k = 0;; k++) {
            u8 a = ld8(n + k);
            u8 b = ld8(i_name + k);
            if (a != b) break;
            if (a == 0) return i;
        }
    }
    return -1;
}
VERIFY(0x025303B4, dCamera_c_searchEventArgData);

/* 02530418 */
static u32 dCamera_c_getEvIntData(u32 i_this, u32 o_data, u32 i_name) {
    WWHD_FUNC(0x02530418, u32, i_this, o_data, i_name);
    if (debugParams(i_this)) {
        s32 i = dCamera_c_searchEventArgData(i_this, i_name);
        if (i == -1) return 0;
        st(o_data, ld(paramValue(i_this, i)));
        return 1;
    }
    if (evmng_getMySubstanceNum_l((s32)ld(i_this + EV_STAFF), i_name) == 0) {
        st8(i_this + EV_NODATA, 1);
        return 0;
    }
    u32 p = evmng_getMySubstanceP_l((s32)ld(i_this + EV_STAFF), i_name, 3);
    if (p == 0) JUT_ASSERT_l(0x1004CCB8, 0x9A, 0x1004CCC8 /* "p_ret != (0)" */);
    st(o_data, ld(p));
    return 1;
}
VERIFY(0x02530418, dCamera_c_getEvIntData);

/* 02530538 */
static u32 dCamera_c_getEvStringPntData(u32 i_this, u32 i_name) {
    WWHD_FUNC(0x02530538, u32, i_this, i_name);
    if (debugParams(i_this)) {
        s32 i = dCamera_c_searchEventArgData(i_this, i_name);
        if (i == -1) return 0;
        return paramValue(i_this, i);
    }
    if (evmng_getMySubstanceNum_l((s32)ld(i_this + EV_STAFF), i_name) == 0) {
        st8(i_this + EV_NODATA, 1);
        return 0;
    }
    u32 p = evmng_getMySubstanceP_l((s32)ld(i_this + EV_STAFF), i_name, 4);
    if (p == 0) JUT_ASSERT_l(0x1004CCD8, 0x11C, 0x1004CCE8);
    return p;
}
VERIFY(0x02530538, dCamera_c_getEvStringPntData);

/* 02530634 */
static u32 dCamera_c_getEvIntData3(u32 i_this, u32 o_data, u32 i_name, s32 i_default) {
    WWHD_FUNC(0x02530634, u32, i_this, o_data, i_name, i_default);
    s32 v = i_default;
    if (debugParams(i_this)) {
        s32 i = dCamera_c_searchEventArgData(i_this, i_name);
        if (i != -1) v = (s32)ld(paramValue(i_this, i));
        st(o_data, (u32)v);
        return 1;
    }
    if (evmng_getMySubstanceNum_l((s32)ld(i_this + EV_STAFF), i_name) == 0) {
        st(o_data, (u32)v);
        return 0;
    }
    u32 p = evmng_getMySubstanceP_l((s32)ld(i_this + EV_STAFF), i_name, 3);
    if (p == 0) JUT_ASSERT_l(0x1004CCF8, 0x137, 0x1004CD08);
    st(o_data, ld(p));
    return 1;
}
VERIFY(0x02530634, dCamera_c_getEvIntData3);

/* 0253072C */
static u32 dCamera_c_getEvFloatData(u32 i_this, u32 o_data, u32 i_name, f32 i_default) {
    WWHD_FUNC(0x0253072C, u32, i_this, o_data, i_name, i_default);
    f32 v = i_default;
    if (debugParams(i_this)) {
        s32 i = dCamera_c_searchEventArgData(i_this, i_name);
        if (i != -1) v = ldf(paramValue(i_this, i));
        stf(o_data, v);
        return 1;
    }
    if (evmng_getMySubstanceNum_l((s32)ld(i_this + EV_STAFF), i_name) == 0) {
        stf(o_data, v);
        return 0;
    }
    u32 p = evmng_getMySubstanceP_l((s32)ld(i_this + EV_STAFF), i_name, 0);
    if (p == 0) JUT_ASSERT_l(0x1004CD18, 0x150, 0x1004CD28);
    stf(o_data, ldf(p));
    return 1;
}
VERIFY(0x0253072C, dCamera_c_getEvFloatData);

static inline void copy3(u32 d, u32 s) {
    st(d + 0, ld(s + 0));
    st(d + 4, ld(s + 4));
    st(d + 8, ld(s + 8));
}

/* 0253086C: getEvXyzData(out, name, default) */
static u32 dCamera_c_getEvXyzData(u32 i_this, u32 o_data, u32 i_name, u32 i_default) {
    WWHD_FUNC(0x0253086C, u32, i_this, o_data, i_name, i_default);
    if (debugParams(i_this)) {
        s32 i = dCamera_c_searchEventArgData(i_this, i_name);
        copy3(o_data, i == -1 ? i_default : paramValue(i_this, i));
        return 1;
    }
    if (evmng_getMySubstanceNum_l((s32)ld(i_this + EV_STAFF), i_name) == 0) {
        copy3(o_data, i_default);
        return 0;
    }
    u32 p = evmng_getMySubstanceP_l((s32)ld(i_this + EV_STAFF), i_name, 1);
    if (p == 0) JUT_ASSERT_l(0x1004CD38, 0x169, 0x1004CD48);
    copy3(o_data, p);
    return 1;
}
VERIFY(0x0253086C, dCamera_c_getEvXyzData);

/* 02530998: getEvStringData(out, name, default) */
static u32 dCamera_c_getEvStringData(u32 i_this, u32 o_data, u32 i_name, u32 i_default) {
    WWHD_FUNC(0x02530998, u32, i_this, o_data, i_name, i_default);
    if (debugParams(i_this)) {
        s32 i = dCamera_c_searchEventArgData(i_this, i_name);
        strcpy_l(o_data, i == -1 ? i_default : paramValue(i_this, i));
        return 1;
    }
    if (evmng_getMySubstanceNum_l((s32)ld(i_this + EV_STAFF), i_name) == 0) {
        strcpy_l(o_data, i_default);
        return 0;
    }
    u32 p = evmng_getMySubstanceP_l((s32)ld(i_this + EV_STAFF), i_name, 4);
    if (p == 0) JUT_ASSERT_l(0x1004CD58, 0x19C, 0x1004CD68);
    strcpy_l(o_data, p);
    return 1;
}
VERIFY(0x02530998, dCamera_c_getEvStringData);

/* 02530AAC: getEvStringPntData(name, default) */
static u32 dCamera_c_getEvStringPntData2(u32 i_this, u32 i_name, u32 i_default) {
    WWHD_FUNC(0x02530AAC, u32, i_this, i_name, i_default);
    if (debugParams(i_this)) {
        s32 i = dCamera_c_searchEventArgData(i_this, i_name);
        return i == -1 ? i_default : paramValue(i_this, i);
    }
    if (evmng_getMySubstanceNum_l((s32)ld(i_this + EV_STAFF), i_name) == 0) return i_default;
    u32 p = evmng_getMySubstanceP_l((s32)ld(i_this + EV_STAFF), i_name, 4);
    if (p == 0) JUT_ASSERT_l(0x1004CD78, 0x1B6, 0x1004CD88);
    return p;
}
VERIFY(0x02530AAC, dCamera_c_getEvStringPntData2);

/* dEvt_control_c::convPId(id) with the play pointer loaded after the call to gi() */
static inline u32 convPIdAt(u32 off) {
    u32 g = gi();
    return gabi::call<u32>(0x0253EE04, g + 0x51D0, ld(g + off));
}

/* 02530B84: getEvActor(name) */
static u32 dCamera_c_getEvActor(u32 i_this, u32 i_name) {
    WWHD_FUNC(0x02530B84, u32, i_this, i_name);
    u32 s = dCamera_c_getEvStringPntData(i_this, i_name);
    if (s == 0) return 0;
    u32 w = ld(s);
    if (w == 0x40504C41) return ld(i_this + 0x128);
    if (w == 0x40535441) return convPIdAt(0x5294);
    if (w == 0x40504152) return convPIdAt(0x5298);
    if (w == 0x4054414C) return convPIdAt(0x529C);
    if (w == 0x40544152 || w == 0x40495445) return convPIdAt(0x52A0);
    if (w == 0x4C696E6B) return ld(gi() + 0x5B34);
    return gabi::call<u32>(0x025D9F38, s, 0, 0);
}
VERIFY(0x02530B84, dCamera_c_getEvActor);

/* 02530CB4: getEvActor(name, default) through a 16-byte string buffer */
static u32 dCamera_c_getEvActor2(u32 i_this, u32 i_name, u32 i_default) {
    WWHD_FUNC(0x02530CB4, u32, i_this, i_name, i_default);
    gabi::Local<u8[0x20]> frame; /* the original's frame: the buffer at +8 (long strings overrun it alike) */
    u32 b = gabi::ea(frame.get()) + 8;
    st8(b, 0);
    dCamera_c_getEvStringData(i_this, b, i_name, i_default);
    u32 w = ld(b);
    if (w == 0x40504C41) return ld(i_this + 0x128);
    if (w == 0x40535441) return convPIdAt(0x5294);
    if (w == 0x40504152) return convPIdAt(0x5298);
    if (w == 0x4054414C) return convPIdAt(0x529C);
    if (w == 0x40544152 || w == 0x40495445) return convPIdAt(0x52A0);
    if (w == 0x4C696E6B) return ld(gi() + 0x5B34);
    return gabi::call<u32>(0x025D9F38, b, 0, 0);
}
VERIFY(0x02530CB4, dCamera_c_getEvActor2);

/* 02530DDC: pauseEvCamera; work: +0 timer set, +4 Stay, +8 Timer */
static u32 dCamera_c_pauseEvCamera(u32 i_this) {
    WWHD_FUNC(0x02530DDC, u32, i_this);
    u32 w = i_this + EV_WORK;
    if (ld(i_this + EV_TIMER) == 0) {
        st8(i_this + 0x102, 1);
        st8(i_this + 0x101, 1);
        st8(i_this + 0x100, 1);
        st8(w + 0, (u8)dCamera_c_getEvIntData3(i_this, w + 8, 0x1004CD9C /* "Timer" */, -1));
        dCamera_c_getEvIntData3(i_this, w + 4, 0x1004CDA4 /* "Stay" */, 0);
        if (ld8(i_this + 0x610) != 0) {
            /* HD: keep the eye 120 units from the centre along the current direction */
            gabi::Local<cXyz> d;
            gabi::Local<cXyz> n;
            gabi::Local<cXyz> s;
            u32 dd = gabi::ea(d.get()), nn = gabi::ea(n.get()), ss = gabi::ea(s.get());
            gabi::call(0x0201ADE0, i_this + 0x1C, dd, i_this + 0x10); /* eye - center */
            gabi::call(0x0201B31C, dd, nn);                          /* normalize */
            gabi::call(0x0201AE48, nn, ss, 120.0f);                  /* * 120 */
            copy3(dd, ss);
            gabi::call(0x0201AD78, i_this + 0x10, nn, dd);           /* center + */
            u32 x = ld(nn + 0), y = ld(nn + 4), z = ld(nn + 8);
            st(i_this + 0x58, z);
            st(i_this + 0x50, x);
            st(i_this + 0x54, y);
            gabi::call(0x0201ADE0, i_this + 0x50, nn, i_this + 0x44); /* eye - center */
            gabi::call(0x020072E0, i_this + 0x3C, nn);               /* mViewCache.mDirection.Val */
        }
    }
    if (ld(w + 4) != 0) st(i_this + EV_FLAGS, ld(i_this + EV_FLAGS) | 1);
    if (ld8(w + 0) != 0 && ld(i_this + EV_TIMER) < ld(w + 8)) return 0;
    return 1;
}
VERIFY(0x02530DDC, dCamera_c_pauseEvCamera);

/* 02530F5C: talktoEvCamera: talktoCamera with the event type's style (or "TT01") */
static u8 dCamera_c_talktoEvCamera(u32 i_this) {
    WWHD_FUNC(0x02530F5C, u8, i_this);
    convPIdAt(0x5294);
    s32 type = (s32)ld(i_this + 0x408);
    u32 t108 = ld(i_this + 0x108);
    s32 style = lds16(0x1004936E + (u32)(type << 6)); /* types[type].mStyles[0] */
    if (t108 == 0) st(i_this + EV_FLAGS, ld(i_this + EV_FLAGS) & ~0x200000u);
    if (style < 0) style = gabi::call<s32>(0x024F732C, i_this + 0x8A4, 0x54543031 /* 'TT01' */);
    gabi::call(0x02508A60, i_this, style); /* talktoCamera */
    u8 r = 0;
    if (ld8(i_this + 0x100) != 0 && ld8(i_this + 0x101) != 0 && ld8(i_this + 0x102) != 0) r = 1;
    return r;
}
VERIFY(0x02530F5C, dCamera_c_talktoEvCamera);

/* 0253E70C: StartEventCamera(id2, id1, name, value, ..., NULL): varargs name/value pairs into the
 * debug parameter table (at most 8) */
static u32 dCamera_c_StartEventCamera(u32 i_this, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7) {
    WWHD_FUNC(0x0253E70C, u32, i_this, a1, a2, a3, a4, a5, a6, a7);
    u32 entry = gabi::cpu->r[1];
    gabi::Local<u8[0x90]> frame; /* the original's frame: va_list at +8, register save area at +0x18 */
    u32 f = gabi::ea(frame.get());
    const u32 regs[8] = {i_this, a1, a2, a3, a4, a5, a6, a7};
    for (u32 i = 0; i < 8; i++) st(f + 0x18 + 4 * i, regs[i]);
    if (debugParams(i_this)) return 0;
    st(i_this + EV_ID1, a2);
    st(i_this + EV_ID2, a1);
    u32 va = f + 8;
    st(va + 4, entry + 8);
    st(va + 8, f + 0x18);
    st8(va + 0, 3);
    st8(va + 1, 0);
    for (u32 i = 0; i < 8; i++) {
        u32 name = ld(gabi::call<u32>(0x028F5C88, va, 1));
        u32 slot = i_this + EV_PARAMS + i * 0x14;
        if (name == 0) {
            st8(slot, 0);
            break;
        }
        strcpy_l(slot, name);
        u32 v = ld(gabi::call<u32>(0x028F5C88, va, 1));
        st(slot + 0x10, v);
    }
    u32 fl = ld(i_this + EV_FLAGS);
    st(i_this + EV_TIMER, 0);
    st(i_this + EV_FLAGS, fl | 0x20000000);
    return 1;
}
VERIFY(0x0253E70C, dCamera_c_StartEventCamera);

/* 0253E860: EndEventCamera(id) */
static u32 dCamera_c_EndEventCamera(u32 i_this, u32 i_id) {
    WWHD_FUNC(0x0253E860, u32, i_this, i_id);
    u32 fl = ld(i_this + EV_FLAGS);
    if (!((fl >> 29) & 1)) return 0;
    if ((s32)ld(i_this + 0xE8) != -1 && i_id != ld(i_this + EV_ID1)) return 0;
    st(i_this + EV_FLAGS, fl & ~0x20000000u);
    return 1;
}
VERIFY(0x0253E860, dCamera_c_EndEventCamera);

/* 0253E89C: static initialisers (header statics) */
static void __sinit_d_ev_camera_cpp() {
    WWHD_FUNC(0x0253E89C, void, (u32)0);
    sinit_header_statics(0x10475850, 0x101D627C);
}
VERIFY(0x0253E89C, __sinit_d_ev_camera_cpp);

/* 0253E930: deleting destructor without members (per-TU copy) */
static void dtor_0253E930(u32 i_this, s32 i_flag) {
    WWHD_FUNC(0x0253E930, void, i_this, i_flag);
    if (i_this == 0) return;
    if ((i_flag & 1) == 0) return;
    gabi::call(0x0273AF40, i_this);
}
VERIFY(0x0253E930, dtor_0253E930);

/* 0253E944: empty virtual (per-TU copy) */
static void empty_0253E944(u32 i_this) {
    WWHD_FUNC(0x0253E944, void, i_this);
}
VERIFY(0x0253E944, empty_0253E944);

} // namespace d_ev_camera_cpp

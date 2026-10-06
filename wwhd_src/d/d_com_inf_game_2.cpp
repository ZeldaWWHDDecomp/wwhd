/* d_com_inf_game, part 2: 02522F6C..02525F4F (stage resource helpers, dComIfG_play_c destructor,
 * itemInit, the grass/tree/wood/flower and magma packets, getLayerNo, the unit's __sinit 02525A48
 * and its per-TU inline copies). See
 * d_com_inf_game.cpp for the unit's layout notes.
 *
 * HD-only: the stage resources are set/synchronised/deleted through sead::SafeString names
 * (resource control *(101F4F54)); a stage's archive name is "ma3room" instead of "ma2room" once
 * event 0x1820 is set, and some stages load extra archives (tables 101D5DC8..101D5DDC). */
#include "bindings.h"

namespace d_com_inf_game_2_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 JUT_ASSERT_l(u32 file, s32 line, u32 msg) { return gabi::call<u32>(0x0273AA24, file, line, msg); }
static inline u32 operator_new_l(u32 size) { return gabi::call<u32>(0x0273AD10, size); }
static inline void __dl_l(u32 p) { gabi::call(0x0273AF40, p); }
static inline u32 gi() { return gabi::call<u32>(0x025200D4); }
static inline u32 savep() { return gabi::load<u32>(0x101F84DC); }
static inline u32 resctl() { return gabi::load<u32>(0x101F4F54); }
static inline u32 isEventBit_l(u32 flag) { return gabi::call<u32>(0x025B8B94, savep() + 0x644, flag); }
static inline u32 getTriforceNum_l() { return gabi::call<u32>(0x025B7E00, savep() + 0xD4); }
static inline void SafeString_format_l(u32 ss, u32 fmt, u32 arg) { gabi::call(0x02759C28, ss, fmt, arg); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline s8 lds8(u32 a) { return gabi::load<s8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }

static constexpr u32 SS_VT = 0x1004B8C0;  /* sead::SafeString vtable of this unit */
static constexpr u32 FSS_VT = 0x1004B930; /* sead::FixedSafeString<32> */

struct SafeString_l {
    be<u32> top;
    be<u32> vt;
};
/* sead::FixedSafeString<32> {top, vt, size, buffer[32]} */
struct FixedSafeString32_l {
    be<u32> top;
    be<u32> vt;
    be<u32> size;
    u8 buf[0x20];
};

static inline void vss(u32 ss) { gabi::call_ptr(ld(ld(ss + 4) + 0x14), ss); } /* assureTermination */

/* inlined strcmp: equal for the same pointer or equal characters up to a NUL (at most 0x40001) */
static inline bool str_equal(u32 a, u32 b) {
    if (a == b) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = ld8(a + i);
        u8 cb = ld8(b + i);
        if (ca != cb) return false;
        if (ca == 0) return true;
    }
    return false;
}
/* a == b for a SafeString object a and the literal SafeString b on the stack: a's
 * assureTermination twice, then b's */
static inline bool ss_eq(u32 a, u32 b) {
    vss(a);
    vss(a);
    u32 ta = ld(a + 0);
    vss(b);
    return str_equal(ta, ld(b + 0));
}
/* the start stage name (gi + 0x5134) compared with a literal: literal SafeString first */
static inline bool startStageIs(u32 lit) {
    gabi::Local<SafeString_l> a;
    gabi::Local<SafeString_l> b;
    u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get());
    st(aa + 0, lit);
    st(aa + 4, SS_VT);
    u32 g = gi();
    st(bb + 0, g + 0x5134);
    st(bb + 4, SS_VT);
    return ss_eq(aa, bb);
}
/* SafeString ss compared with a literal */
static inline bool ssIs(u32 ss, u32 lit) {
    gabi::Local<SafeString_l> l;
    u32 ll = gabi::ea(l.get());
    st(ll + 0, lit);
    st(ll + 4, SS_VT);
    return ss_eq(ss, ll);
}

/* 02522F6C: HD, i_arc == "Stage" && i_stage is one of the sea stages */
static u32 isSeaStageRes(u32 i_arc, u32 i_stage) {
    WWHD_FUNC(0x02522F6C, u32, i_arc, i_stage);
    if (!ssIs(i_arc, 0x1004BAC0 /* "Stage" */)) return 0;
    static const u32 names[5] = {0x1004BAD8 /* "ADMumi" */, 0x1004BAB8 /* "A_umikz" */, 0x1004BAE0 /* "ENDumi" */,
                                 0x1004BAC8 /* "sea_T" */, 0x1004BAD0 /* "sea_E" */};
    for (u32 k = 0; k < 5; k++)
        if (ssIs(i_stage, names[k])) return 1;
    return 0;
}
VERIFY(0x02522F6C, isSeaStageRes);

/* 02523318: HD, i_arc == "Stage" && i_stage == "ENDumi" */
static u32 isEndStageRes(u32 i_arc, u32 i_stage) {
    WWHD_FUNC(0x02523318, u32, i_arc, i_stage);
    if (!ssIs(i_arc, 0x1004BAE8 /* "Stage" */)) return 0;
    return ssIs(i_stage, 0x1004BAF0 /* "ENDumi" */) ? 1 : 0;
}
VERIFY(0x02523318, isEndStageRes);

/* 0252349C: HD, i_arc == "Stage" && i_stage is "M_NewD2", "Siren" or "sea_T" */
static u32 isExtraStageRes(u32 i_arc, u32 i_stage) {
    WWHD_FUNC(0x0252349C, u32, i_arc, i_stage);
    if (!ssIs(i_arc, 0x1004BB00 /* "Stage" */)) return 0;
    static const u32 names[3] = {0x1004BAF8 /* "M_NewD2" */, 0x1004BB08 /* "Siren" */, 0x1004BB10 /* "sea_T" */};
    for (u32 k = 0; k < 3; k++)
        if (ssIs(i_stage, names[k])) return 1;
    return 0;
}
VERIFY(0x0252349C, isExtraStageRes);

/* 02523730: HD, set the extra archives of a stage; true when all succeeded */
static u32 setExtraStageRes(u32 i_stage) {
    WWHD_FUNC(0x02523730, u32, i_stage);
    u32 ok = 1;
    if (ssIs(i_stage, 0x1004BB18 /* "M_NewD2" */)) {
        gabi::Local<SafeString_l> n;
        u32 nn = gabi::ea(n.get());
        for (u32 i = 0; i < 3; i++) {
            u32 rc = resctl();
            st(nn + 4, SS_VT);
            st(nn + 0, ld(0x101D5DDC + 4 * i));
            ok &= gabi::call<u32>(0x0260E0DC, rc, i_stage, nn);
        }
        return ok;
    }
    if (ssIs(i_stage, 0x1004BB20 /* "sea_T" */)) {
        gabi::Local<SafeString_l> n;
        u32 nn = gabi::ea(n.get());
        for (u32 i = 0; i < 2; i++) {
            u32 name = ld(0x101D5DC8 + 4 * i);
            u32 rc = resctl();
            st(nn + 4, SS_VT);
            st(nn + 0, name);
            ok &= gabi::call<u32>(0x0260D618, rc, i_stage, nn, 0);
        }
        gabi::Local<SafeString_l> m;
        u32 mm = gabi::ea(m.get());
        for (u32 i = 0; i < 2; i++) {
            u32 rc = resctl();
            st(mm + 4, SS_VT);
            st(mm + 0, ld(0x101D5DD0 + 4 * i));
            ok &= gabi::call<u32>(0x0260CBB4, rc, mm, 0);
        }
        return ok;
    }
    if (ssIs(i_stage, 0x1004BB28 /* "Siren" */)) {
        gabi::Local<SafeString_l> n;
        u32 nn = gabi::ea(n.get());
        u32 rc = resctl();
        st(nn + 4, SS_VT);
        st(nn + 0, ld(0x101D5DD8));
        return gabi::call<u32>(0x0260E0DC, rc, i_stage, nn) & 1;
    }
    return ok;
}
VERIFY(0x02523730, setExtraStageRes);

/* FixedSafeString<32> initialised from a SafeString temporary {i_arc, vt}: strlen (0 beyond
 * 0x40000), clamped to size - 1, copied with 028FDC00 and terminated */
static inline void fss_from(u32 fs, u32 ss, u32 i_arc) {
    st(ss + 0, i_arc);
    st(ss + 4, SS_VT);
    st(fs + 0, fs + 0xC);
    st(fs + 8, 0x20);
    st8(fs + 0xC + 0x1F, 0);
    st(fs + 4, 0x1004B918);
    gabi::call(0x02525C34, ss);
    u32 s = ld(ss + 0);
    s32 len = 0;
    if (ld8(s) != 0) {
        for (;;) {
            len++;
            if (len > 0x40000) {
                len = 0;
                break;
            }
            if (ld8(s + (u32)len) == 0) break;
        }
    }
    s32 size = (s32)ld(fs + 8);
    if (len >= size) len = size - 1;
    vss(ss);
    gabi::call(0xC0009988, fs + 0xC, ld(ss + 0), len, 0);
    st8(fs + 0xC + (u32)len, 0);
}
/* the stage name archive: "ma3room" for "ma2room" after event 0x1820, else the start stage */
static inline void stageArcName(u32 fs, u32 ma2, u32 ma3, u32 fmt) {
    bool ma = startStageIs(ma2);
    if (ma && isEventBit_l(0x1820) != 0) {
        SafeString_format_l(fs, fmt, ma3);
        return;
    }
    u32 g = gi();
    SafeString_format_l(fs, fmt, g + 0x5134);
}

/* 02523A04: HD dComIfG_setStageRes(arcName) (matcher: dRes_control_c::setStageRes) */
/* Returns the result of the final 0260D618 call (left in r3; callers read it: void_used / retvals type-only). */
static u32 dComIfG_setStageRes(u32 i_arc) {
    WWHD_FUNC(0x02523A04, u32, i_arc);
    gabi::Local<SafeString_l> ss;
    gabi::Local<FixedSafeString32_l> arc;
    gabi::Local<FixedSafeString32_l> stg;
    u32 a = gabi::ea(arc.get()), s = gabi::ea(stg.get());
    fss_from(a, gabi::ea(ss.get()), i_arc);
    st8(s + 0xC, 0);
    st(s + 0, s + 0xC);
    st8(s + 0xC + 0x1F, 0);
    st(a + 4, FSS_VT);
    st(s + 8, 0x20);
    st(s + 4, FSS_VT);
    stageArcName(s, 0x1004BB30 /* "ma2room" */, 0x1004BB38 /* "ma3room" */, 0x1004BB54 /* "%s" */);
    if (isSeaStageRes(a, s) != 0) {
        gabi::Local<SafeString_l> n1;
        gabi::Local<SafeString_l> n2;
        u32 x = gabi::ea(n1.get()), y = gabi::ea(n2.get());
        st(x + 4, SS_VT);
        st(y + 4, SS_VT);
        st(x + 0, 0x1004BB40 /* "sea" */);
        st(y + 0, 0x1004BB44 /* "Stage" */);
        gabi::call(0x0260D618, resctl(), x, y, 0);
    }
    if (isEndStageRes(a, s) != 0) {
        gabi::Local<SafeString_l> n1;
        gabi::Local<SafeString_l> n2;
        u32 x = gabi::ea(n1.get()), y = gabi::ea(n2.get());
        st(x + 4, SS_VT);
        st(y + 4, SS_VT);
        st(x + 0, 0x1004BB58 /* "Hyrule" */);
        st(y + 0, 0x1004BB4C /* "Room0" */);
        gabi::call(0x0260D618, resctl(), x, y, 0);
    }
    if (isExtraStageRes(a, s) != 0) setExtraStageRes(s);
    return gabi::call<u32>(0x0260D618, resctl(), s, a, 0);
}
VERIFY(0x02523A04, dComIfG_setStageRes);

/* 02523D08: HD dComIfG_syncStageRes(arcName) */
static u32 dComIfG_syncStageRes(u32 i_arc) {
    WWHD_FUNC(0x02523D08, u32, i_arc);
    gabi::Local<SafeString_l> ss;
    gabi::Local<FixedSafeString32_l> stg;
    u32 s = gabi::ea(stg.get()), q = gabi::ea(ss.get());
    st(s + 0, s + 0xC);
    st(s + 8, 0x20);
    st8(s + 0xC + 0x1F, 0);
    st(s + 4, FSS_VT);
    st8(s + 0xC, 0);
    stageArcName(s, 0x1004BB60 /* "ma2room" */, 0x1004BB68 /* "ma3room" */, 0x1004BB70 /* "%s" */);
    st(q + 0, i_arc);
    st(q + 4, SS_VT);
    return gabi::call<u32>(0x0260F4CC, resctl(), s, q);
}
VERIFY(0x02523D08, dComIfG_syncStageRes);

/* 02523E9C: HD, delete the extra archives of a stage */
static void deleteExtraStageRes(u32 i_stage) {
    WWHD_FUNC(0x02523E9C, void, i_stage);
    if (ssIs(i_stage, 0x1004BB74 /* "M_NewD2" */)) {
        gabi::Local<SafeString_l> n;
        u32 nn = gabi::ea(n.get());
        for (u32 i = 0; i < 3; i++) {
            u32 name = ld(0x101D5DDC + 4 * i);
            u32 rc = resctl();
            st(nn + 4, SS_VT);
            st(nn + 0, name);
            gabi::call(0x0260EEE8, rc, i_stage, nn);
        }
        return;
    }
    if (ssIs(i_stage, 0x1004BB7C /* "sea_T" */)) {
        gabi::Local<SafeString_l> n;
        u32 nn = gabi::ea(n.get());
        for (u32 i = 0; i < 2; i++) {
            u32 name = ld(0x101D5DC8 + 4 * i);
            u32 rc = resctl();
            st(nn + 4, SS_VT);
            st(nn + 0, name);
            gabi::call(0x0260E8A8, rc, i_stage, nn);
        }
        gabi::Local<SafeString_l> m;
        u32 mm = gabi::ea(m.get());
        for (u32 i = 0; i < 2; i++) {
            u32 name = ld(0x101D5DD0 + 4 * i);
            u32 rc = resctl();
            st(mm + 4, SS_VT);
            st(mm + 0, name);
            gabi::call(0x0260E500, rc, mm);
        }
        return;
    }
    if (ssIs(i_stage, 0x1004BB84 /* "Siren" */)) {
        gabi::Local<SafeString_l> n;
        u32 nn = gabi::ea(n.get());
        u32 rc = resctl();
        st(nn + 4, SS_VT);
        st(nn + 0, ld(0x101D5DD8));
        gabi::call(0x0260EEE8, rc, i_stage, nn);
    }
}
VERIFY(0x02523E9C, deleteExtraStageRes);

/* 02524180: HD dComIfG_deleteStageRes(arcName) */
static u32 dComIfG_deleteStageRes(u32 i_arc) {
    WWHD_FUNC(0x02524180, u32, i_arc);
    gabi::Local<SafeString_l> ss;
    gabi::Local<FixedSafeString32_l> arc;
    gabi::Local<FixedSafeString32_l> stg;
    u32 a = gabi::ea(arc.get()), s = gabi::ea(stg.get());
    fss_from(a, gabi::ea(ss.get()), i_arc);
    st8(s + 0xC, 0);
    st(s + 0, s + 0xC);
    st8(s + 0xC + 0x1F, 0);
    st(s + 4, FSS_VT);
    st(s + 8, 0x20);
    st(a + 4, FSS_VT);
    stageArcName(s, 0x1004BB8C /* "ma2room" */, 0x1004BB94 /* "ma3room" */, 0x1004BBB0 /* "%s" */);
    if (isSeaStageRes(a, s) != 0) {
        gabi::Local<SafeString_l> n1;
        gabi::Local<SafeString_l> n2;
        u32 x = gabi::ea(n1.get()), y = gabi::ea(n2.get());
        u32 rc = resctl();
        st(x + 4, SS_VT);
        st(y + 4, SS_VT);
        st(x + 0, 0x1004BB9C /* "sea" */);
        st(y + 0, 0x1004BBA0 /* "Stage" */);
        gabi::call(0x0260E8A8, rc, x, y);
    }
    if (isEndStageRes(a, s) != 0) {
        gabi::Local<SafeString_l> n1;
        gabi::Local<SafeString_l> n2;
        u32 x = gabi::ea(n1.get()), y = gabi::ea(n2.get());
        u32 rc = resctl();
        st(x + 4, SS_VT);
        st(y + 4, SS_VT);
        st(x + 0, 0x1004BBB4 /* "Hyrule" */);
        st(y + 0, 0x1004BBA8 /* "Room0" */);
        gabi::call(0x0260E8A8, rc, x, y);
    }
    if (isExtraStageRes(a, s) != 0) deleteExtraStageRes(s);
    gabi::call(0x0260E8A8, resctl(), s, a);
    return 1;
}
VERIFY(0x02524180, dComIfG_deleteStageRes);

/* 0252447C: HD dComIfG_getStageRes(arcName, resName) through dRes_control_c (*(101F4F28)) */
static u32 dComIfG_getStageRes(u32 i_arc, u32 i_res) {
    WWHD_FUNC(0x0252447C, u32, i_arc, i_res);
    gabi::Local<SafeString_l> ss;
    gabi::Local<FixedSafeString32_l> stg;
    gabi::Local<SafeString_l> rs;
    u32 s = gabi::ea(stg.get()), q = gabi::ea(ss.get()), r = gabi::ea(rs.get());
    st(s + 8, 0x20);
    st8(s + 0xC + 0x1F, 0);
    st(s + 0, s + 0xC);
    st8(s + 0xC, 0);
    st(s + 4, FSS_VT);
    stageArcName(s, 0x1004BBBC /* "ma2room" */, 0x1004BBC4 /* "ma3room" */, 0x1004BBCC /* "%s" */);
    st(q + 0, i_arc);
    st(q + 4, SS_VT);
    st(r + 4, SS_VT);
    st(r + 0, i_res);
    return gabi::call<u32>(0x02606BEC, ld(0x101F4F28), q, s, r);
}
VERIFY(0x0252447C, dComIfG_getStageRes);

/* 02524628: HD, the current view's projection matrix (copied to the static 104753AC) */
static u32 dComIfGd_getProjViewMtx() {
    WWHD_FUNC(0x02524628, u32);
    u32 p = ld(gi() + 0x5FA4);
    if (p != 0) return p + 0x1A4;
    u32 disp = ld(0x101F95D0);
    u32 cnt = ld(disp + 0x1020);
    u32 lst = ld(disp + 0x1024);
    if (cnt > 1) lst += 4;
    u32 guard = ld(0x101FD9AC);
    u32 view = ld(lst);
    if (guard == 0) {
        st(0x101FD9AC, 1);
        st(0x101FDCE4, 0x1004B9B8);
    }
    if (view != 0) {
        if (gabi::call_ptr<u32>(ld(ld(view + 0xC) + 0x14), view, 0x101FDCE4u) == 0) view = 0; /* DynamicCast: checkDerivedRuntimeTypeInfo(&type info 101FDCE4), r4 is live at the bctrl (livein check 2026-10-04) */
    }
    u32 flags = ld(view + 0x50);
    u32 cam;
    bool viaCam;
    if ((flags >> 12) & 1) {
        cam = ld(view + 0x164);
        viaCam = (flags >> 6) & 1;
    } else {
        u32 c = ld(view + 0x4C);
        cam = 0x104A20FC;
        if (c == 0) viaCam = (flags >> 6) & 1;
        else {
            cam = c;
            viaCam = (flags >> 6) & 1;
        }
    }
    gabi::Local<f32[16]> m;
    u32 mm = gabi::ea(m.get());
    u32 src;
    if (viaCam && ((flags >> 7) & 1)) {
        src = view + 0x84;
    } else {
        u32 v = ld(view + 0x48);
        src = v != 0 ? v : 0x104A2098;
    }
    u32 proj = gabi::call<u32>(0x0274D80C, cam);
    gabi::call(0x02525C94, mm, src, proj);
    for (u32 i = 0; i < 16; i++) stf(0x104753AC + 4 * i, ldf(mm + 4 * i));
    return 0x104753AC;
}
VERIFY(0x02524628, dComIfGd_getProjViewMtx);

/* 025247B8: dComIfG_play_c::~dComIfG_play_c */
static void dComIfG_play_c_dt(u32 i_this, s32 i_flag) {
    WWHD_FUNC(0x025247B8, void, i_this, i_flag);
    if (i_this == 0) return;
    gabi::call(0x0252A140, i_this + 0x4780, 2); /* ~dDetect_c */
    gabi::call(0x025CC110, i_this + 0x46FC, 2); /* ~dVibration_c */
    gabi::call(0x024EDB20, i_this + 0x4564, 2); /* ~dAttention_c */
    gabi::call(0x025C4604, i_this + 0x3EB0, 2);
    gabi::call(0x025ABCA8, i_this + 0x3DF8, 2);
    gabi::call(0x0200B708, i_this + 0x3DEC, 2);
    gabi::call(0x0200B600, i_this + 0x3DAC, 2);
    gabi::call(0x0200B680, i_this + 0x3D48, 2);
    gabi::call(0x028F0164, i_this + 0x3D18, 2, 0x18, 0x02525B6C, 0, 0); /* __destroy_arr */
    gabi::call(0x028F0164, i_this + 0x3C9C, 5, 0x18, 0x02525B6C, 0, 0);
    gabi::call(0x0200B7C0, i_this + 0x3C58, 2);
    gabi::call(0x0200B7C0, i_this + 0x3C14, 2);
    if (i_flag & 1) __dl_l(i_this);
}
VERIFY(0x025247B8, dComIfG_play_c_dt);

/* 025248BC: dComIfG_play_c::itemInit */
static void dComIfG_play_c_itemInit(u32 i_this) {
    WWHD_FUNC(0x025248BC, void, i_this);
    static const u16 h1[] = {0x48C8, 0x48BE, 0x48BC, 0x48C6, 0x48CC, 0x48C0, 0x48C2, 0x48CE, 0x48CA, 0x48C4};
    static const u16 w1[] = {0x48B8, 0x48A8, 0x48B4, 0x48B0, 0x48AC};
    stf(i_this + 0x48A4, 0.0f);
    stf(i_this + 0x48A0, 0.0f);
    for (u16 o : h1) st16(i_this + o, 0);
    for (u16 o : w1) st(i_this + o, 0);
    for (u32 i = 0; i < 8; i++) {
        st16(i_this + 0x48D0 + 2 * i, 0);
        st16(i_this + 0x48E0 + 2 * i, 0);
        st16(i_this + 0x48F0 + 2 * i, 0);
    }
    static const u16 b1[] = {0x4918, 0x4911, 0x4915, 0x4914, 0x4919, 0x4910, 0x491A, 0x4913, 0x4916, 0x4912, 0x4917};
    static const u16 h2[] = {0x4902, 0x490C, 0x4904, 0x490A, 0x4906, 0x4908, 0x490E, 0x4900};
    for (u16 o : b1) st8(i_this + o, 0);
    for (u16 o : h2) st16(i_this + o, 0);
    for (u32 i = 0; i < 5; i++) {
        st8(i_this + 0x491B + i, 0);
        st8(i_this + 0x4920 + i, 0);
    }
    for (u32 o = 0x4925; o <= 0x492C; o++) st8(i_this + o, 0);
    u8 btn = 0x15;
    if (gabi::call<u32>(0x02520C0C, 0x20) != 0) btn = 0; /* dComIfGs_checkGetItem(TELESCOPE) */
    st8(i_this + 0x492D, btn);
    for (u32 o = 0x492F; o <= 0x4933; o++) st8(i_this + o, 0);
    st8(i_this + 0x492E, 7);
    for (u32 i = 0; i < 6; i++) st8(i_this + 0x4934 + i, 0);
    for (u32 o = 0x493A; o <= 0x494C; o++)
        if (o != 0x494D) st8(i_this + o, 0);
    st8(i_this + 0x494F, 0);
    /* strcpy(mInputPassword, "") on the HD WFixedSafeString at +0x4950 */
    u32 dst = ld(i_this + 0x4950);
    gabi::Local<SafeString_l> w;
    u32 ww = gabi::ea(w.get());
    st(ww + 4, 0x1004B900);
    st(ww + 0, 0x1004BBD0);
    gabi::call(0x02525C4C, ww);
    u32 s = ld(ww + 0);
    s32 len = 0;
    if (ld16(s) != 0) {
        u32 p = s;
        for (;;) {
            len++;
            p += 2;
            if (len > 0x40000) {
                len = 0;
                break;
            }
            if (ld16(p) == 0) break;
        }
    }
    s32 size = (s32)ld(i_this + 0x4958);
    if (len >= size) len = size - 1;
    vss(ww);
    gabi::call(0xC0009988, dst, ld(ww + 0), (u32)len << 1, 0);
    st16(dst + ((u32)len << 1), 0);
    st8(i_this + 0x4983, 0);
    st8(i_this + 0x4980, 0);
    st8(i_this + 0x4982, 0);
    st8(i_this + 0x4981, 0);
    st8(i_this + 0x494D, ld8(savep() + 0x1C7)); /* option vibration */
    gabi::call(0x02526118, 0);                   /* daArrow_c::setKeepType(TYPE_NORMAL) */
    st(i_this + 0x4990, 0);
    st(i_this + 0x4994, 0);
    for (u32 i = 0; i < 10; i++) st(i_this + 0x4998 + 4 * i, 0);
    for (u32 o = 0x4988; o <= 0x498D; o++) st8(i_this + o, 0);
}
VERIFY(0x025248BC, dComIfG_play_c_itemInit);

/* 02524BA8: dComIfG_play_c::executeEvtManager */
static void dComIfG_play_c_executeEvtManager(u32 i_this) {
    WWHD_FUNC(0x02524BA8, void, i_this);
    gabi::call(0x02544374, i_this + 0x4024);
}
VERIFY(0x02524BA8, dComIfG_play_c_executeEvtManager);

/* 02524BB0 */
static void dComIfG_play_c_createParticle(u32 i_this) {
    WWHD_FUNC(0x02524BB0, void, i_this);
    u32 p = operator_new_l(0x41A4);
    if (p != 0) p = gabi::call<u32>(0x025A7398, p); /* dPa_control_c */
    st(i_this + 0x4810, p);
    if (p == 0) JUT_ASSERT_l(0x1004BBD4, 0x206, 0x1004BBE8);
}
VERIFY(0x02524BB0, dComIfG_play_c_createParticle);

/* 02524C10 */
/* Returns the magma packet: the existing one (lwz r3,0x4814) or the new one (dMagma_packet_c ctor result); daMagma and
 * daBtd test it (void_used triage 2026-10-05: a void declaration returned i_this / the store's leftover). */
static u32 dComIfG_play_c_createMagma(u32 i_this) {
    WWHD_FUNC(0x02524C10, u32, i_this);
    u32 packet = ld(i_this + 0x4814);
    if (packet != 0) return packet;
    packet = gabi::call<u32>(0x0258C4C0, 0);
    st(i_this + 0x4814, packet);
    return packet;
}
VERIFY(0x02524C10, dComIfG_play_c_createMagma);

/* packet slot: remove = delete with the HD virtual destructor (vtable +0xC, slot 0xC) */
static inline void removeVirt(u32 i_this, u32 off, s32 kind) {
    u32 p = ld(i_this + off);
    if (p == 0) return;
    gabi::call_ptr(ld(ld(p + 0xC) + 0xC), p, kind);
    if (kind == 3) {
        st(i_this + off, 0);
        return;
    }
    st(i_this + off, 0);
    gabi::call(0x025F0148, p);
}
static inline void callIf(u32 i_this, u32 off, u32 fn) {
    u32 p = ld(i_this + off);
    if (p == 0) return;
    gabi::call(fn, p);
}
/* create a packet in its own heap (025F01D8 scoped heap guard) */
static inline u32 createPacket(u32 i_this, u32 off, u32 name, u32 size, u32 ct) {
    u32 p = ld(i_this + off);
    if (p != 0) return p;
    gabi::Local<u8[0x10]> guard;
    u32 g = gabi::ea(guard.get());
    gabi::call(0x025F01D8, g, name, size, 0);
    u32 n = gabi::call<u32>(ct, 0);
    st(i_this + off, n);
    if (n == 0) gabi::call(0x025F02D0, g);
    gabi::call(0x025F0270, g, 2);
    return ld(i_this + off);
}

/* 02524C50 */
static void dComIfG_play_c_removeMagma(u32 i_this) {
    WWHD_FUNC(0x02524C50, void, i_this);
    removeVirt(i_this, 0x4814, 3);
}
VERIFY(0x02524C50, dComIfG_play_c_removeMagma);

/* 02524CA0 */
static void dComIfG_play_c_executeMagma(u32 i_this) {
    WWHD_FUNC(0x02524CA0, void, i_this);
    callIf(i_this, 0x4814, 0x0258C7C8);
}
VERIFY(0x02524CA0, dComIfG_play_c_executeMagma);

/* 02524CB0 */
static void dComIfG_play_c_drawMagma(u32 i_this) {
    WWHD_FUNC(0x02524CB0, void, i_this);
    callIf(i_this, 0x4814, 0x0258CA60);
}
VERIFY(0x02524CB0, dComIfG_play_c_drawMagma);

/* 02524CC0 */
static u32 dComIfG_play_c_createGrass(u32 i_this) {
    WWHD_FUNC(0x02524CC0, u32, i_this);
    return createPacket(i_this, 0x4818, 0x1004BBFC, 0x147740, 0x0254ACD8);
}
VERIFY(0x02524CC0, dComIfG_play_c_createGrass);

/* 02524D3C */
static void dComIfG_play_c_removeGrass(u32 i_this) {
    WWHD_FUNC(0x02524D3C, void, i_this);
    removeVirt(i_this, 0x4818, 2);
}
VERIFY(0x02524D3C, dComIfG_play_c_removeGrass);

/* 02524DA0 */
static void dComIfG_play_c_executeGrass(u32 i_this) {
    WWHD_FUNC(0x02524DA0, void, i_this);
    callIf(i_this, 0x4818, 0x0254C6C4);
}
VERIFY(0x02524DA0, dComIfG_play_c_executeGrass);

/* 02524DB0 */
static void dComIfG_play_c_drawGrass(u32 i_this) {
    WWHD_FUNC(0x02524DB0, void, i_this);
    callIf(i_this, 0x4818, 0x0254CF78);
}
VERIFY(0x02524DB0, dComIfG_play_c_drawGrass);

/* 02524DC0 */
static u32 dComIfG_play_c_createTree(u32 i_this) {
    WWHD_FUNC(0x02524DC0, u32, i_this);
    return createPacket(i_this, 0x481C, 0x1004BC10, 0xA0E40, 0x025C7A1C);
}
VERIFY(0x02524DC0, dComIfG_play_c_createTree);

/* 02524E3C */
static void dComIfG_play_c_removeTree(u32 i_this) {
    WWHD_FUNC(0x02524E3C, void, i_this);
    removeVirt(i_this, 0x481C, 2);
}
VERIFY(0x02524E3C, dComIfG_play_c_removeTree);

/* 02524EA0 */
static void dComIfG_play_c_executeTree(u32 i_this) {
    WWHD_FUNC(0x02524EA0, void, i_this);
    callIf(i_this, 0x481C, 0x025C9948);
}
VERIFY(0x02524EA0, dComIfG_play_c_executeTree);

/* 02524EB0 */
static void dComIfG_play_c_drawTree(u32 i_this) {
    WWHD_FUNC(0x02524EB0, void, i_this);
    callIf(i_this, 0x481C, 0x025CA6EC);
}
VERIFY(0x02524EB0, dComIfG_play_c_drawTree);

/* 02524EC0 */
static u32 dComIfG_play_c_createWood(u32 i_this) {
    WWHD_FUNC(0x02524EC0, u32, i_this);
    return createPacket(i_this, 0x4820, 0x1004BC28, 0x156240, 0x025CF20C);
}
VERIFY(0x02524EC0, dComIfG_play_c_createWood);

/* 02524F3C */
static void dComIfG_play_c_removeWood(u32 i_this) {
    WWHD_FUNC(0x02524F3C, void, i_this);
    removeVirt(i_this, 0x4820, 2);
}
VERIFY(0x02524F3C, dComIfG_play_c_removeWood);

/* 02524FA0 */
static void dComIfG_play_c_executeWood(u32 i_this) {
    WWHD_FUNC(0x02524FA0, void, i_this);
    callIf(i_this, 0x4820, 0x025D0994);
}
VERIFY(0x02524FA0, dComIfG_play_c_executeWood);

/* 02524FB0 */
static void dComIfG_play_c_drawWood(u32 i_this) {
    WWHD_FUNC(0x02524FB0, void, i_this);
    callIf(i_this, 0x4820, 0x025D15B0);
}
VERIFY(0x02524FB0, dComIfG_play_c_drawWood);

/* 02524FC0 */
static u32 dComIfG_play_c_createFlower(u32 i_this) {
    WWHD_FUNC(0x02524FC0, u32, i_this);
    return createPacket(i_this, 0x4824, 0x1004BC40, 0x99340, 0x02545A2C);
}
VERIFY(0x02524FC0, dComIfG_play_c_createFlower);

/* 0252503C */
static void dComIfG_play_c_removeFlower(u32 i_this) {
    WWHD_FUNC(0x0252503C, void, i_this);
    removeVirt(i_this, 0x4824, 2);
}
VERIFY(0x0252503C, dComIfG_play_c_removeFlower);

/* 025250A0 */
static void dComIfG_play_c_executeFlower(u32 i_this) {
    WWHD_FUNC(0x025250A0, void, i_this);
    callIf(i_this, 0x4824, 0x02548370);
}
VERIFY(0x025250A0, dComIfG_play_c_executeFlower);

/* 025250B0 */
static void dComIfG_play_c_drawFlower(u32 i_this) {
    WWHD_FUNC(0x025250B0, void, i_this);
    callIf(i_this, 0x4824, 0x02548B6C);
}
VERIFY(0x025250B0, dComIfG_play_c_drawFlower);

/* 025250C0: HD adds the A_mori (event 0x0F80) check before Asoko and the "kenroom" order */
static s32 dComIfG_play_c_getLayerNo(s32 i_roomNo) {
    WWHD_FUNC(0x025250C0, s32, i_roomNo);
    if (lds8(gi() + 0x513F) >= 0) return lds8(gi() + 0x513F);
    s32 hour = gabi::call<s32>(0x02556C34);
    s32 layer;
    if (gabi::call<u32>(0x02556BC0) != 0) layer = 1;
    else layer = ((u32)(hour - 6) < 12) ? 0 : 1;
    if (startStageIs(0x1004BC68 /* "sea" */)) {
        if (i_roomNo == 0x2C) {
            if (isEventBit_l(0x520)) return layer | 4;
            if (isEventBit_l(0xE20)) return layer | 2;
            if (isEventBit_l(0x101)) return 9;
            return layer;
        }
        if (i_roomNo == 0xB) {
            if (isEventBit_l(0x2D01)) return layer | 4;
            if (gabi::call<u32>(0x02556BC0) != 0) return layer | 2;
            return layer;
        }
        if (i_roomNo == 1) {
            if (isEventBit_l(0x1820)) return 3;
            return 1;
        }
        return layer;
    }
    if (startStageIs(0x1004BC74 /* "A_mori" */)) return isEventBit_l(0xF80) ? (layer | 2) : layer;
    if (startStageIs(0x1004BC6C /* "Asoko" */)) return isEventBit_l(0x520) ? (layer | 2) : layer;
    if (startStageIs(0x1004BC7C /* "Hyrule" */)) {
        if (getTriforceNum_l() == 8) return layer | 4;
        return isEventBit_l(0x3280) ? (layer | 2) : layer;
    }
    if (startStageIs(0x1004BC84 /* "Hyroom" */)) {
        if (getTriforceNum_l() == 8 && isEventBit_l(0x2C01) == 0) return layer | 4;
        if (isEventBit_l(0x3280)) return layer | 2;
        return isEventBit_l(0x3B40) ? (layer | 6) : layer;
    }
    if (startStageIs(0x1004BC58 /* "kenroom" */)) {
        if (isEventBit_l(0x2C01)) return layer | 6;
        if (isEventBit_l(0x3802) && isEventBit_l(0x3280) == 0) return layer | 6;
        if (getTriforceNum_l() == 8) return layer | 4;
        return isEventBit_l(0x3802) ? (layer | 2) : layer;
    }
    if (startStageIs(0x1004BC60 /* "M2tower" */)) return isEventBit_l(0x2D01) ? (layer | 2) : layer;
    if (startStageIs(0x1004BC8C /* "GanonK" */)) return isEventBit_l(0x3B02) == 0 ? 8 : layer;
    if (startStageIs(0x1004BC94 /* "GTower" */)) return isEventBit_l(0x4002) == 0 ? 8 : layer;
    return layer;
}
VERIFY(0x025250C0, dComIfG_play_c_getLayerNo);

/* 02525A48: static initialisers (header statics) */
static void __sinit_d_com_inf_game_cpp() {
    WWHD_FUNC(0x02525A48, void, (u32)0);
    sinit_header_statics(0x1046F090, 0x101D5E6C);
}
VERIFY(0x02525A48, __sinit_d_com_inf_game_cpp);

/* 02525ADC: dSv_memory_c::dSv_memory_c (per-TU copy) */
static u32 dSv_memory_c_ct(u32 i_this) {
    WWHD_FUNC(0x02525ADC, u32, i_this);
    if (i_this == 0) {
        i_this = operator_new_l(0x24);
        if (i_this == 0) return 0;
    }
    gabi::call(0x025B9170, i_this); /* dSv_memory_c::init */
    return i_this;
}
VERIFY(0x02525ADC, dSv_memory_c_ct);

/* 02525B24: sead::BufferedSafeString::assureTerminationImpl (per-TU copy) */
static void BufferedSafeString_assureTermination(u32 i_this) {
    WWHD_FUNC(0x02525B24, void, i_this);
    u32 top = ld(i_this + 0);
    u32 size = ld(i_this + 8);
    st8(top + size - 1, 0);
}
VERIFY(0x02525B24, BufferedSafeString_assureTermination);

static inline void dl_trivial(u32 i_this, s32 i_flag) {
    if (i_this == 0) return;
    if ((i_flag & 1) == 0) return;
    __dl_l(i_this);
}

/* 02525B3C: deleting destructor (no members) */
static void dtor_02525B3C(u32 i_this, s32 i_flag) {
    WWHD_FUNC(0x02525B3C, void, i_this, i_flag);
    dl_trivial(i_this, i_flag);
}
VERIFY(0x02525B3C, dtor_02525B3C);

/* 02525B50: phase_3 */
static s32 phase_3(u32 i_arcName) {
    WWHD_FUNC(0x02525B50, s32, i_arcName);
    return 4;
}
VERIFY(0x02525B50, phase_3);

/* 02525B58: deleting destructor (no members) */
static void dtor_02525B58(u32 i_this, s32 i_flag) {
    WWHD_FUNC(0x02525B58, void, i_this, i_flag);
    dl_trivial(i_this, i_flag);
}
VERIFY(0x02525B58, dtor_02525B58);

/* 02525B6C: deleting destructor of the 0x18-byte array element (member at +0xC) */
static void dtor_02525B6C(u32 i_this, s32 i_flag) {
    WWHD_FUNC(0x02525B6C, void, i_this, i_flag);
    if (i_this == 0) return;
    gabi::call(0x0200B708, i_this + 0xC, 2);
    if (i_flag & 1) __dl_l(i_this);
}
VERIFY(0x02525B6C, dtor_02525B6C);

/* 02525BC0: deleting destructor (no members) */
static void dtor_02525BC0(u32 i_this, s32 i_flag) {
    WWHD_FUNC(0x02525BC0, void, i_this, i_flag);
    dl_trivial(i_this, i_flag);
}
VERIFY(0x02525BC0, dtor_02525BC0);

/* 02525BD4: dComIfG_inf_c::~dComIfG_inf_c */
static void dComIfG_inf_c_dt(u32 i_this, s32 i_flag) {
    WWHD_FUNC(0x02525BD4, void, i_this, i_flag);
    if (i_this == 0) return;
    gabi::call(0x0252EBA0, i_this + 0x5D30, 2); /* ~dDlst_list_c */
    dComIfG_play_c_dt(i_this + 0x12A0, 2);
    if (i_flag & 1) __dl_l(i_this);
}
VERIFY(0x02525BD4, dComIfG_inf_c_dt);

/* 02525C34: sead::SafeString::assureTerminationImpl (empty, per-TU copy) */
static void SafeString_assureTermination(u32 i_this) {
    WWHD_FUNC(0x02525C34, void, i_this);
}
VERIFY(0x02525C34, SafeString_assureTermination);

/* 02525C38: deleting destructor (no members) */
static void dtor_02525C38(u32 i_this, s32 i_flag) {
    WWHD_FUNC(0x02525C38, void, i_this, i_flag);
    dl_trivial(i_this, i_flag);
}
VERIFY(0x02525C38, dtor_02525C38);

/* 02525C4C: sead::WSafeString::assureTerminationImpl (empty, per-TU copy) */
static void WSafeString_assureTermination(u32 i_this) {
    WWHD_FUNC(0x02525C4C, void, i_this);
}
VERIFY(0x02525C4C, WSafeString_assureTermination);

/* 02525C50: sead::WBufferedSafeString::assureTerminationImpl */
static void WBufferedSafeString_assureTermination(u32 i_this) {
    WWHD_FUNC(0x02525C50, void, i_this);
    u32 size = ld(i_this + 8);
    u32 top = ld(i_this + 0);
    st16(top + (size << 1) - 2, 0);
}
VERIFY(0x02525C50, WBufferedSafeString_assureTermination);

/* 02525C6C: deleting destructor (no members) */
static void dtor_02525C6C(u32 i_this, s32 i_flag) {
    WWHD_FUNC(0x02525C6C, void, i_this, i_flag);
    dl_trivial(i_this, i_flag);
}
VERIFY(0x02525C6C, dtor_02525C6C);

/* 02525C80: deleting destructor (no members) */
static void dtor_02525C80(u32 i_this, s32 i_flag) {
    WWHD_FUNC(0x02525C80, void, i_this, i_flag);
    dl_trivial(i_this, i_flag);
}
VERIFY(0x02525C80, dtor_02525C80);

/* 02525C94: the last of the
 * unit's out-of-line inline copies (after its __sinit 02525A48; the next TU is the sinit-only TU
 * 02525F50). out (4x4) = a (3x4, extended by the row 0 0 0 1) * b (4x4): rows 0..2 are the
 * products, row 3 is b's row 3. Probably sead::Matrix44CalcCommon<f32>::multiply(Matrix44&,
 * const Matrix34&, const Matrix44&). Called by dComIfGd_getProjViewMtx (02524628) and, as the kept
 * COMDAT copy, from d_s_play and two other units. All loads come before the first store; each
 * element is a1*b1j, then + a0*b0j, + a2*b2j, + a3*b3j (fmadds). */
static void mtx34x44Multiply(u32 out, u32 a, u32 b) {
    WWHD_FUNC(0x02525C94, void, out, a, b);
    f32 r[12];
    for (u32 i = 0; i < 3; i++)
        for (u32 j = 0; j < 4; j++) {
            u32 ai = a + i * 16;
            f32 v = gabi::fmuls_ppc(gabi::load<f32>(ai + 4), gabi::load<f32>(b + 0x10 + j * 4));
            v = gabi::fmadds(gabi::load<f32>(ai), gabi::load<f32>(b + j * 4), v);
            v = gabi::fmadds(gabi::load<f32>(ai + 8), gabi::load<f32>(b + 0x20 + j * 4), v);
            v = gabi::fmadds(gabi::load<f32>(ai + 12), gabi::load<f32>(b + 0x30 + j * 4), v);
            r[i * 4 + j] = v;
        }
    for (u32 k : {0u, 2u, 3u, 6u, 1u, 7u, 4u, 5u, 8u, 10u, 9u, 11u}) gabi::store<f32>(out + k * 4, r[k]);
    for (u32 k = 12; k < 16; k++) gabi::store<f32>(out + k * 4, gabi::load<f32>(b + k * 4));
}
VERIFY(0x02525C94, mtx34x44Multiply);

} // namespace d_com_inf_game_2_cpp

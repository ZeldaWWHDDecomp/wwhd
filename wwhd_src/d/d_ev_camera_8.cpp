/* d_ev_camera, part 8: useItem0EvCamera. See
 * d_ev_camera.cpp for the unit's layout notes; written from the WWHD code (the GameCube
 * d_ev_camera.cpp is all "Nonmatching" stubs). The per-item offset tables live in the stack frame
 * as in the original (built field by field from .rodata); the frame is reproduced at the same
 * offsets (F). */
#include "bindings.h"

namespace d_ev_camera_8_cpp {

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
/* centre from the item's centre offset, eye = centre + direction */
static inline void holdView(u32 c, u32 w, u32 F) {
    relationalPos(c, F + 0x44, ld(c + 0x128), ld(F + 0x200 + (ld(w + 8) << 2)));
    copy3(c + 0x44, F + 0x44);
    cSGlobe_Xyz(c + 0x3C, F + 0x38);
    cXyz_pl(c + 0x44, F + 0x44, F + 0x38);
    copy3(c + 0x50, F + 0x44);
}

/* 02538DB4: useItem0EvCamera (showing an item in use; "Type" selects one of seven offset sets).
 * work: +0 state (0xA: wait, then pick the first clear eye of the type's list; 1: blend over
 * work+4 frames; 2: hold (type 0 once more as type 4); 0x63: done), +8 Type, +0xC centre,
 * +0x18 target fovy, +0x1C/+0x20 counters, +0x24 target direction */
static u32 dCamera_c_useItem0EvCamera(u32 i_this) {
    WWHD_FUNC(0x02538DB4, u32, i_this);
    u32 entry = gabi::cpu->r[1];
    gabi::Local<u8[0x288]> frame; /* reserves the original frame [entry - 0x288, entry) */
    u32 F = entry - 0x288;
    (void)frame;
    st(F, entry);                  /* back chain (stwu) */
    st(entry + 4, gabi::cpu->lr);  /* LR save word in the caller's frame */
    /* the register save area (stmw r20 at +0x238, f30/f31 at +0x268/+0x278): an out-of-range
     * "Type" indexes the tables into it as in the original */
    for (u32 i = 20; i < 32; i++) st(F + 0x238 + (i - 20) * 4, gabi::cpu->r[i]);
    {
        u64 b;
        f64 d = gabi::cpu->f[30].ps0;
        memcpy(&b, &d, 8);
        gabi::store<u64>(F + 0x268, b);
        stf(F + 0x270, (f32)gabi::cpu->f[30].ps1);
        d = gabi::cpu->f[31].ps0;
        memcpy(&b, &d, 8);
        gabi::store<u64>(F + 0x278, b);
        stf(F + 0x280, (f32)gabi::cpu->f[31].ps1);
    }
    u32 w = i_this + 0x37C;
    stf(F + 0x84, ldf(0x1004CF80));
    stf(F + 0x7C, ldf(0x1004CE58));
    stf(F + 0x80, ldf(0x1004D2CC));
    stf(F + 0xE0, ldf(0x1004D2C4));
    stf(F + 0xD4, ldf(0x1004D2B8));
    stf(F + 0x78, ldf(0x1004D2B0));
    stf(F + 0xD8, ldf(0x1004D2BC));
    stf(F + 0x74, ldf(0x1004D2AC));
    stf(F + 0xD0, ldf(0x1004D2B4));
    stf(F + 0xDC, ldf(0x1004D2C0));
    stf(F + 0x70, ldf(0x1004D2A8));
    stf(F + 0xE4, ldf(0x1004D2C8));
    stf(F + 0x13C, ldf(0x1004D2D8));
    stf(F + 0x14C, ldf(0x1004D2E0));
    stf(F + 0x130, ldf(0x1004D2D0));
    stf(F + 0x144, ldf(0x1004D2D0));
    stf(F + 0x15C, ldf(0x1004CF70));
    stf(F + 0x154, ldf(0x1004D2E4));
    stf(F + 0x150, ldf(0x1004CEB4));
    stf(F + 0x164, ldf(0x1004D0D4));
    stf(F + 0x140, ldf(0x1004D2DC));
    stf(F + 0x158, ldf(0x1004D2E8));
    stf(F + 0x160, ldf(0x1004D1CC));
    stf(F + 0xAC, ldf(0x1004D2F0));
    stf(F + 0x148, ldf(0x1004D1C0));
    stf(F + 0x134, ldf(0x1004D0BC));
    stf(F + 0xC0, ldf(0x1004D0E8));
    stf(F + 0xB0, ldf(0x1004D004));
    stf(F + 0x138, ldf(0x1004D2D4));
    stf(F + 0xC4, ldf(0x1004CEB8));
    stf(F + 0x180, ldf(0x1004D0D8));
    stf(F + 0x5C, ldf(0x1004CE58));
    stf(F + 0x178, ldf(0x1004CEB8));
    stf(F + 0xB4, ldf(0x1004D2F4));
    stf(F + 0x168, ldf(0x1004D300));
    stf(F + 0x17C, ldf(0x1004D2F8));
    stf(F + 0x184, ldf(0x1004D30C));
    stf(F + 0x18C, ldf(0x1004D310));
    stf(F + 0x60, ldf(0x1004D0DC));
    stf(F + 0xBC, ldf(0x1004D0B4));
    stf(F + 0x16C, ldf(0x1004D2F8));
    stf(F + 0xB8, ldf(0x1004D2F8));
    stf(F + 0x174, ldf(0x1004D308));
    stf(F + 0x188, ldf(0x1004D0BC));
    stf(F + 0x170, ldf(0x1004D304));
    stf(F + 0x88, ldf(0x1004CE58));
    stf(F + 0x8C, ldf(0x1004D2FC));
    stf(F + 0x90, ldf(0x1004D2EC));
    stf(F + 0x64, ldf(0x1004D2EC));
    stf(F + 0x94, ldf(0x1004D314));
    stf(F + 0xCC, ldf(0x1004D0D8));
    stf(F + 0x98, ldf(0x1004D318));
    stf(F + 0xF4, ldf(0x1004D32C));
    stf(F + 0xC8, ldf(0x1004D2F8));
    stf(F + 0xF8, ldf(0x1004D330));
    stf(F + 0xF0, ldf(0x1004D328));
    stf(F + 0xE8, ldf(0x1004D320));
    stf(F + 0x9C, ldf(0x1004D31C));
    stf(F + 0x100, ldf(0x1004D320));
    stf(F + 0x10C, ldf(0x1004D340));
    stf(F + 0x118, ldf(0x1004D34C));
    stf(F + 0x11C, ldf(0x1004D350));
    stf(F + 0x120, ldf(0x1004D354));
    stf(F + 0xEC, ldf(0x1004D324));
    stf(F + 0x104, ldf(0x1004D324));
    stf(F + 0xFC, ldf(0x1004D334));
    stf(F + 0x110, ldf(0x1004D344));
    stf(F + 0x114, ldf(0x1004D348));
    stf(F + 0x124, ldf(0x1004D358));
    stf(F + 0x128, ldf(0x1004D35C));
    stf(F + 0x12C, ldf(0x1004D360));
    stf(F + 0xA0, ldf(0x1004D338));
    stf(F + 0x108, ldf(0x1004D328));
    st(F + 0x214, F + 0x94);
    st(F + 0x200, F + 0x5C);
    st(F + 0x208, F + 0x5C);
    st(F + 0x21C, F + 0xAC);
    st(F + 0x20C, F + 0x88);
    st(F + 0x204, F + 0x7C);
    stf(F + 0xA8, ldf(0x1004D2D4));
    st(F + 0x220, F + 0x130);
    st(F + 0x228, F + 0x160);
    st(F + 0x210, F + 0x70);
    st(F + 0x22C, F + 0xD0);
    st(F + 0x218, F + 0xA0);
    st(F + 0x224, F + 0xAC);
    stf(F + 0xA4, ldf(0x1004D33C));
    st(F + 0x190, 0x2);
    st(F + 0x194, 0x4);
    st(F + 0x198, 0x3);
    st(F + 0x19C, 0x4);
    st(F + 0x1A0, 0x3);
    st(F + 0x1AC, ld(0x1004D36C));
    st(F + 0x1B0, ld(0x1004D370));
    st(F + 0x1B4, ld(0x1004D374));
    st(F + 0x1B8, ld(0x1004D378));
    st(F + 0x1BC, ld(0x1004D37C));
    st(F + 0x1A4, 0x3);
    st(F + 0x1C8, 0x2D);
    st(F + 0x1C0, ld(0x1004D380));
    st(F + 0x1C4, ld(0x1004D384));
    st(F + 0x1A8, 0x3);
    st(F + 0x1CC, 0x28);
    st(F + 0x1D0, 0x28);
    st(F + 0x1D4, 0x28);
    st(F + 0x230, F + 0xE8);
    st(F + 0x1D8, 0xA);
    st(F + 0x1DC, 0x2D);
    st(F + 0x1E0, 0x37);
    st(F + 0x1E4, 0x0);
    st(F + 0x1E8, 0x0);
    st(F + 0x1EC, 0x0);
    st(F + 0x1F0, 0x0);
    st(F + 0x1F4, 0x0);
    st(F + 0x1F8, 0x49);
    st(F + 0x234, F + 0x10C);
    st(F + 0x1FC, 0x0);
    gabi::call(0x020065FC, F + 0x22); /* cSAngle() */
    gabi::call(0x020065FC, F + 0x30);
    u32 s;
    if (ld(i_this + 0x11C) == 0) {
        s = 0;
        st(w + 0, 0);
    } else {
        s = ld(w + 0);
    }
    if (s == 1) {
        /* blend towards the chosen view */
        f32 t = (f32)(f64)(s32)ld(w + 0x20) / (f32)(f64)(s32)ld(w + 4);
        f32 fv = ldf(i_this + 0x60);
        stf(i_this + 0x60, gabi::fmadds(ldf(w + 0x18) - fv, t, fv));
        relationalPos(i_this, F + 0x38, ld(i_this + 0x128), ld(F + 0x200 + (ld(w + 8) << 2)));
        copy3(w + 0xC, F + 0x38);
        cXyz_mi(w + 0xC, F + 0x44, i_this + 0x44);
        cXyz_ml(F + 0x44, F + 0x38, t);
        gabi::call(0x028E8D88, i_this + 0x44, F + 0x38, i_this + 0x44); /* PSVECAdd */
        st16(F + 0x30, (u16)lds16(i_this + 0x40));
        st16(F + 0x22, (u16)lds16(i_this + 0x42));
        f32 r0 = ldf(i_this + 0x3C);
        f32 rad = gabi::fmadds(ldf(w + 0x24) - r0, t, r0);
        cSAngle_mi(w + 0x28, F + 0x34, F + 0x30);
        cSAngle_ml(F + 0x34, F + 0x32, t);
        cSAngle_addeq(F + 0x30, F + 0x32);
        cSAngle_mi(w + 0x2A, F + 0x32, F + 0x22);
        cSAngle_ml(F + 0x32, F + 0x34, t);
        cSAngle_addeq(F + 0x22, F + 0x34);
        gabi::call(0x020071E0, i_this + 0x3C, rad, F + 0x30, F + 0x22); /* cSGlobe::Val(r, V, U) */
        cSGlobe_Xyz(i_this + 0x3C, F + 0x38);
        cXyz_pl(i_this + 0x44, F + 0x44, F + 0x38);
        copy3(i_this + 0x50, F + 0x44);
        u32 c = ld(w + 0x20);
        if ((s32)c < (s32)ld(w + 4)) {
            st(w + 0x20, c + 1);
            return 0;
        }
        st(w + 0, 2);
        s = 2;
    }
    if (s == 2) {
        holdView(i_this, w, F);
        u32 n = ld(w + 0x1C) + 1;
        u32 type = ld(w + 8);
        st(w + 0x1C, n);
        if (type == 0 && n == 1) {
            u32 c = ld(w + 0x20);
            st(w + 0, 0xA);
            st(w + 8, 4);
            st(w + 0x20, c + 1);
            return 0;
        }
        st(w + 0, 0x63);
        s = 0x63;
    }
    if (s == 0x63) {
        setDone(i_this);
        holdView(i_this, w, F);
        st(w + 0x20, ld(w + 0x20) + 1);
        return 1;
    }
    if (s != 0xA) {
        getEvIntData(i_this, w + 8, 0x1004D364 /* "Type" */, 0);
        st(w + 0x1C, 0);
        st(w + 0x20, 0);
    }
    /* state 0xA: wait the type's frames, then choose the eye */
    u32 c = ld(w + 0x20) + 1;
    st(w + 0, 0xA);
    st(w + 0x20, c);
    if ((s32)c < (s32)ld(F + 0x1E4 + (ld(w + 8) << 2))) {
        st(w + 0x20, c + 1);
        return 0;
    }
    st(w + 0x20, 0);
    if ((ld(i_this + 0x7C) & 7) == 0) {
        u32 type = ld(w + 8);
        if (type >= 2 && type <= 3) { /* swap the first two eye offsets */
            u32 p = ld(F + 0x21C + (type << 2));
            u32 x1 = ld(p + 0x10), x0 = ld(p + 0xC);
            f32 a0 = ldf(p + 0);
            st(p + 0, x0);
            f32 a1 = ldf(p + 4);
            st(p + 4, x1);
            u32 x2 = ld(p + 0x14);
            f32 a2 = ldf(p + 8);
            st(p + 8, x2);
            u32 q = ld(F + 0x21C + (ld(w + 8) << 2));
            stf(q + 0x10, a1);
            stf(q + 0x14, a2);
            stf(q + 0xC, a0);
        }
    }
    relationalPos(i_this, F + 0x38, ld(i_this + 0x128), ld(F + 0x200 + (ld(w + 8) << 2)));
    copy3(w + 0xC, F + 0x38);
    u32 E = F + 0x14;
    for (s32 i = 0; i < (s32)ld(F + 0x190 + (ld(w + 8) << 2)); i++) {
        relationalPos(i_this, F + 8, ld(i_this + 0x128), ld(F + 0x21C + (ld(w + 8) << 2)) + (u32)i * 12);
        copy3(E, F + 8);
        positionOf(i_this, F + 8, ld(i_this + 0x128));
        if (ldf(E + 4) < ldf(F + 0xC) + ldf(i_this + 0x36C)) { /* bge: taken on NaN */
            positionOf(i_this, F + 0x50, ld(i_this + 0x128));
            stf(E + 4, ldf(F + 0x54) + ldf(i_this + 0x36C));
        }
        u32 item = 0;
        s32 pad = (s32)ld(i_this + 0x124);
        u32 g = gi();
        if (ld(g + 0x5CD8 + ((u32)pad << 4)) & 0x10000) {
            st16(F + 0x20, 0xA5);
            item = gabi::call<u32>(0x025D5218, 0x025E121C, F + 0x20); /* fopAcIt_Judge(fpcSch_JudgeForPName) */
        }
        if (lineBGCheck(i_this, w + 0xC, E, 0x8F)) continue;
        for (u32 k = 0; k < 3; k++) stf(F + 8 + 4 * k, ldf(w + 0xC + 4 * k));
        for (u32 k = 0; k < 3; k++) stf(F + 0x24 + 4 * k, ldf(E + 4 * k));
        u32 pl = ld(i_this + 0x128);
        g = gi();
        if (!gabi::call<bool>(0x025187D8, g + 0x26A4, F + 8, F + 0x24, ldf(0x1004CF80), pl, item)) break; /* dCcS::ChkCamera */
    }
    cXyz_mi(E, F + 0x38, w + 0xC);
    cSGlobe_Val(w + 0x24, F + 0x38);
    u32 type = ld(w + 8);
    u32 cnt = ld(w + 0x20);
    stf(w + 0x18, ldf(F + 0x1AC + (type << 2)));
    u32 fr = ld(F + 0x1C8 + (type << 2));
    st(w + 0, 1);
    st(w + 4, fr);
    st(w + 0x20, cnt + 1);
    return 0;
}
VERIFY(0x02538DB4, dCamera_c_useItem0EvCamera);

/* the original's register save area at the top of a frame (stmw rFirst at saveOff; f30/f31 as
 * stfd + the paired-single slot) */
static inline void frameSaves(u32 F, u32 first, u32 saveOff, u32 f30Off) {
    for (u32 i = first; i < 32; i++) st(F + saveOff + (i - first) * 4, gabi::cpu->r[i]);
    u64 b;
    f64 d = gabi::cpu->f[30].ps0;
    memcpy(&b, &d, 8);
    gabi::store<u64>(F + f30Off, b);
    stf(F + f30Off + 8, (f32)gabi::cpu->f[30].ps1);
    d = gabi::cpu->f[31].ps0;
    memcpy(&b, &d, 8);
    gabi::store<u64>(F + f30Off + 0x10, b);
    stf(F + f30Off + 0x18, (f32)gabi::cpu->f[31].ps1);
}

/* 0253986C: useItem1EvCamera: as useItem0EvCamera without the initial wait and without the item
 * actor in the view check; tables: centre offsets +0x1E4, eye offsets +0x200, counts +0x190,
 * fovy +0x1AC, frames +0x1C8 */
static u32 dCamera_c_useItem1EvCamera(u32 i_this) {
    WWHD_FUNC(0x0253986C, u32, i_this);
    u32 entry = gabi::cpu->r[1];
    gabi::Local<u8[0x260]> frame;
    u32 F = entry - 0x260;
    (void)frame;
    st(F, entry);                  /* back chain (stwu) */
    st(entry + 4, gabi::cpu->lr);  /* LR save word in the caller's frame */
    frameSaves(F, 24, 0x220, 0x240);
    u32 w = i_this + 0x37C;
    stf(F + 0xD4, ldf(0x1004D1C0));
    stf(F + 0x70, ldf(0x1004CE58));
    stf(F + 0xD0, ldf(0x1004D390));
    stf(F + 0xE8, ldf(0x1004D1C8));
    stf(F + 0xF0, ldf(0x1004CF68));
    stf(F + 0xDC, ldf(0x1004D390));
    stf(F + 0xD8, ldf(0x1004D394));
    stf(F + 0xE0, ldf(0x1004D398));
    stf(F + 0x7C, ldf(0x1004CE58));
    stf(F + 0x80, ldf(0x1004D2CC));
    stf(F + 0xEC, ldf(0x1004D39C));
    stf(F + 0xE4, ldf(0x1004D1D4));
    stf(F + 0x84, ldf(0x1004CF80));
    stf(F + 0x78, ldf(0x1004D38C));
    stf(F + 0x74, ldf(0x1004D388));
    stf(F + 0x170, ldf(0x1004D2DC));
    stf(F + 0x168, ldf(0x1004D2D4));
    stf(F + 0x164, ldf(0x1004D0BC));
    stf(F + 0x88, ldf(0x1004CE58));
    stf(F + 0x174, ldf(0x1004D2D0));
    stf(F + 0x17C, ldf(0x1004D2E0));
    stf(F + 0x160, ldf(0x1004D2D0));
    stf(F + 0x180, ldf(0x1004CEB4));
    stf(F + 0x16C, ldf(0x1004D2D8));
    stf(F + 0x5C, ldf(0x1004D0DC));
    stf(F + 0x178, ldf(0x1004D1C0));
    stf(F + 0x8C, ldf(0x1004D2FC));
    stf(F + 0x60, ldf(0x1004D2EC));
    stf(F + 0xAC, ldf(0x1004D2F0));
    stf(F + 0x58, ldf(0x1004CE58));
    stf(F + 0xB0, ldf(0x1004D004));
    stf(F + 0x184, ldf(0x1004D2E4));
    stf(F + 0xB4, ldf(0x1004D2F4));
    stf(F + 0xC0, ldf(0x1004D0E8));
    stf(F + 0xBC, ldf(0x1004D0B4));
    stf(F + 0x18C, ldf(0x1004CF70));
    stf(F + 0x90, ldf(0x1004D2EC));
    stf(F + 0xF4, ldf(0x1004D1CC));
    stf(F + 0xF8, ldf(0x1004D0D4));
    stf(F + 0xB8, ldf(0x1004D2F8));
    stf(F + 0xFC, ldf(0x1004D300));
    stf(F + 0x118, ldf(0x1004CF80));
    stf(F + 0x188, ldf(0x1004D2E8));
    stf(F + 0x11C, ldf(0x1004D2D0));
    stf(F + 0x94, ldf(0x1004CF74));
    stf(F + 0x108, ldf(0x1004D308));
    stf(F + 0x100, ldf(0x1004D2F8));
    stf(F + 0x98, ldf(0x1004D3A0));
    stf(F + 0xA4, ldf(0x1004D33C));
    stf(F + 0x134, ldf(0x1004D0BC));
    stf(F + 0xC4, ldf(0x1004CEB8));
    stf(F + 0x10C, ldf(0x1004CEB8));
    stf(F + 0x124, ldf(0x1004D3A4));
    stf(F + 0x128, ldf(0x1004D0B8));
    stf(F + 0x130, ldf(0x1004D3AC));
    stf(F + 0x104, ldf(0x1004D304));
    stf(F + 0x13C, ldf(0x1004D340));
    stf(F + 0x120, ldf(0x1004D008));
    stf(F + 0x9C, ldf(0x1004D3A0));
    stf(F + 0x138, ldf(0x1004D2EC));
    stf(F + 0x110, ldf(0x1004D2F8));
    stf(F + 0xC8, ldf(0x1004D2F8));
    stf(F + 0xCC, ldf(0x1004D0D8));
    stf(F + 0xA8, ldf(0x1004D2D4));
    stf(F + 0x114, ldf(0x1004D0D8));
    stf(F + 0x12C, ldf(0x1004D3A8));
    stf(F + 0xA0, ldf(0x1004D338));
    stf(F + 0x140, ldf(0x1004D344));
    stf(F + 0x144, ldf(0x1004D348));
    st(F + 0x1F0, F + 0x88);
    st(F + 0x1E4, F + 0x58);
    st(F + 0x1F8, F + 0x94);
    st(F + 0x200, F + 0xAC);
    stf(F + 0x148, ldf(0x1004D34C));
    stf(F + 0x154, ldf(0x1004D358));
    st(F + 0x204, F + 0x160);
    st(F + 0x208, F + 0xAC);
    stf(F + 0x14C, ldf(0x1004D350));
    stf(F + 0x150, ldf(0x1004D354));
    st(F + 0x1EC, F + 0x58);
    st(F + 0x190, 0x3);
    st(F + 0x194, 0x4);
    st(F + 0x1F4, F + 0x70);
    st(F + 0x1E8, F + 0x7C);
    st(F + 0x198, 0x3);
    st(F + 0x210, F + 0xD0);
    stf(F + 0x15C, ldf(0x1004D360));
    st(F + 0x20C, F + 0xF4);
    st(F + 0x1AC, ld(0x1004D3B8));
    st(F + 0x1B0, ld(0x1004D3BC));
    st(F + 0x1B4, ld(0x1004D3C0));
    st(F + 0x1B8, ld(0x1004D3C4));
    st(F + 0x1BC, ld(0x1004D3C8));
    st(F + 0x19C, 0x3);
    st(F + 0x1A0, 0x3);
    st(F + 0x1A4, 0x3);
    st(F + 0x1FC, F + 0xA0);
    st(F + 0x214, F + 0x118);
    st(F + 0x1C0, ld(0x1004D3CC));
    stf(F + 0x158, ldf(0x1004D35C));
    st(F + 0x1C8, 0x2D);
    st(F + 0x1CC, 0x28);
    st(F + 0x1D0, 0x28);
    st(F + 0x1D4, 0x28);
    st(F + 0x1D8, 0xA);
    st(F + 0x1DC, 0x2D);
    st(F + 0x1E0, 0x28);
    st(F + 0x1A8, 0x3);
    st(F + 0x218, F + 0x13C);
    st(F + 0x1C4, ld(0x1004D3D0));
    gabi::call(0x020065FC, F + 0x2C); /* cSAngle() */
    gabi::call(0x020065FC, F + 0x2E);
    u32 ctrTab = F + 0x1E4, eyeTab = F + 0x200;
    u32 s;
    if (ld(i_this + 0x11C) == 0) {
        s = 0;
        st(w + 0, 0);
    } else {
        s = ld(w + 0);
    }
    if (s == 1) {
        f32 t = (f32)(f64)(s32)ld(w + 0x20) / (f32)(f64)(s32)ld(w + 4);
        f32 fv = ldf(i_this + 0x60);
        stf(i_this + 0x60, gabi::fmadds(ldf(w + 0x18) - fv, t, fv));
        relationalPos(i_this, F + 0x34, ld(i_this + 0x128), ld(ctrTab + (ld(w + 8) << 2)));
        copy3(w + 0xC, F + 0x34);
        cXyz_mi(w + 0xC, F + 0x40, i_this + 0x44);
        cXyz_ml(F + 0x40, F + 0x34, t);
        gabi::call(0x028E8D88, i_this + 0x44, F + 0x34, i_this + 0x44); /* PSVECAdd */
        s16 v = lds16(i_this + 0x40), u = lds16(i_this + 0x42);
        st16(F + 0x2C, (u16)u);
        st16(F + 0x2E, (u16)v);
        f32 r0 = ldf(i_this + 0x3C);
        f32 rad = gabi::fmadds(ldf(w + 0x24) - r0, t, r0);
        cSAngle_mi(w + 0x28, F + 0x32, F + 0x2E);
        cSAngle_ml(F + 0x32, F + 0x30, t);
        cSAngle_addeq(F + 0x2E, F + 0x30);
        cSAngle_mi(w + 0x2A, F + 0x30, F + 0x2C);
        cSAngle_ml(F + 0x30, F + 0x32, t);
        cSAngle_addeq(F + 0x2C, F + 0x32);
        gabi::call(0x020071E0, i_this + 0x3C, rad, F + 0x2E, F + 0x2C); /* cSGlobe::Val(r, V, U) */
        cSGlobe_Xyz(i_this + 0x3C, F + 0x34);
        cXyz_pl(i_this + 0x44, F + 0x40, F + 0x34);
        copy3(i_this + 0x50, F + 0x40);
        u32 c = ld(w + 0x20);
        if ((s32)c < (s32)ld(w + 4)) {
            st(w + 0x20, c + 1);
            return 0;
        }
        st(w + 0, 2);
        s = 2;
    }
    if (s == 2 || s == 0x63) {
        if (s == 2) {
            relationalPos(i_this, F + 0x40, ld(i_this + 0x128), ld(ctrTab + (ld(w + 8) << 2)));
            copy3(i_this + 0x44, F + 0x40);
            cSGlobe_Xyz(i_this + 0x3C, F + 0x34);
            cXyz_pl(i_this + 0x44, F + 0x40, F + 0x34);
            copy3(i_this + 0x50, F + 0x40);
            u32 n = ld(w + 0x1C) + 1;
            u32 type = ld(w + 8);
            st(w + 0x1C, n);
            if (type == 0 && n == 1) {
                st(w + 8, 4);
                u32 c = ld(w + 0x20);
                st(w + 0, 0xA);
                st(w + 0x20, c + 1);
                return 0;
            }
            st(w + 0, 0x63);
        }
        setDone(i_this);
        relationalPos(i_this, F + 0x40, ld(i_this + 0x128), ld(ctrTab + (ld(w + 8) << 2)));
        copy3(i_this + 0x44, F + 0x40);
        cSGlobe_Xyz(i_this + 0x3C, F + 0x34);
        cXyz_pl(i_this + 0x44, F + 0x40, F + 0x34);
        copy3(i_this + 0x50, F + 0x40);
        st(w + 0x20, ld(w + 0x20) + 1);
        return 1;
    }
    if (s != 0xA) {
        getEvIntData(i_this, w + 8, 0x1004D3B0 /* "Type" */, 0);
        st(w + 0x1C, 0);
    }
    st(w + 0x20, 0);
    if ((ld(i_this + 0x7C) & 7) == 0) {
        u32 type = ld(w + 8);
        if (type >= 2 && type <= 3) {
            u32 p = ld(eyeTab + (type << 2));
            u32 x1 = ld(p + 0x10), x0 = ld(p + 0xC);
            f32 a0 = ldf(p + 0);
            st(p + 0, x0);
            f32 a1 = ldf(p + 4);
            st(p + 4, x1);
            u32 x2 = ld(p + 0x14);
            f32 a2 = ldf(p + 8);
            st(p + 8, x2);
            u32 q = ld(eyeTab + (ld(w + 8) << 2));
            stf(q + 0x10, a1);
            stf(q + 0x14, a2);
            stf(q + 0xC, a0);
        }
    }
    relationalPos(i_this, F + 0x34, ld(i_this + 0x128), ld(ctrTab + (ld(w + 8) << 2)));
    copy3(w + 0xC, F + 0x34);
    u32 E = F + 0x14;
    for (s32 i = 0; i < (s32)ld(F + 0x190 + (ld(w + 8) << 2)); i++) {
        relationalPos(i_this, F + 8, ld(i_this + 0x128), ld(eyeTab + (ld(w + 8) << 2)) + (u32)i * 12);
        copy3(E, F + 8);
        positionOf(i_this, F + 8, ld(i_this + 0x128));
        if (ldf(E + 4) < ldf(F + 0xC) + ldf(i_this + 0x36C)) { /* bge: taken on NaN */
            positionOf(i_this, F + 0x4C, ld(i_this + 0x128));
            stf(E + 4, ldf(F + 0x50) + ldf(i_this + 0x36C));
        }
        if (lineBGCheck(i_this, w + 0xC, E, 0x8F)) continue;
        for (u32 k = 0; k < 3; k++) stf(F + 8 + 4 * k, ldf(w + 0xC + 4 * k));
        for (u32 k = 0; k < 3; k++) stf(F + 0x20 + 4 * k, ldf(E + 4 * k));
        u32 pl = ld(i_this + 0x128);
        u32 g = gi();
        if (!gabi::call<bool>(0x025187D8, g + 0x26A4, F + 8, F + 0x20, ldf(0x1004CF80), pl, 0)) break; /* dCcS::ChkCamera */
    }
    cXyz_mi(E, F + 0x34, w + 0xC);
    cSGlobe_Val(w + 0x24, F + 0x34);
    u32 type = ld(w + 8);
    u32 cnt = ld(w + 0x20);
    stf(w + 0x18, ldf(F + 0x1AC + (type << 2)));
    u32 fr = ld(F + 0x1C8 + (type << 2));
    st(w + 0, 1);
    st(w + 4, fr);
    st(w + 0x20, cnt + 1);
    return 0;
}
VERIFY(0x0253986C, dCamera_c_useItem1EvCamera);

} // namespace d_ev_camera_8_cpp

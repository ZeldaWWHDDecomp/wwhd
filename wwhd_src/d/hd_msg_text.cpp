/* hd_msg_text: HD message text state (current message id/label, text box line metrics), WWHD.
 * HD-only code (no GameCube source): written from the WWHD
 * code. Range 025F65F4..025F7423 (static initialiser 025F7304). Library companions (not verified):
 * 025F7398, 025F73C8, 025F73FC, 025F7410 (sead string deleting dtors), 025F73AC, 025F73DC (empty
 * SafeString assureTermination), 025F73B0 (BufferedSafeString assureTermination), 025F73E0
 * (WBufferedSafeString assureTermination).
 *
 * MsgText (0x924): +0/+4 state words, +8 message set, +0xC FixedSafeString<256> set name (size +0x14),
 * +0x118 label index (-1 none), +0x11C message number, +0x120 FixedSafeString<256> label (size +0x128),
 * +0x22C MSBT entry, +0x230 WFixedSafeString<512>, +0x63C..+0x65C counters/flags, +0x660 u32[5] and
 * +0x674 f32[5] (line widths, +0x644 count), +0x688..+0x6BC layout state, +0x6A0 sound position (3 f32),
 * +0x6C0/+0x7CC WFixedSafeString<128>, +0x8D8..+0x8F4 font/scale state (+0x8E0/+0x8E4 scale,
 * +0x8E8 1.0, +0x8EC current line width, +0x8F0 -1.0), +0x8F8 u32[5], +0x90C..+0x923 flags.
 * Font manager 101F4A50 (hd_font_mgr), message manager 101F4AE8 (hd_msg_res_mgr).
 */
#include "gabi.h"
using namespace gabi;

namespace hd_msg_text {

static s32 cstrLength(u32 p) {
    s32 n = 0;
    if (!load<u8>(p)) return 0;
    for (;;) {
        n++;
        if (n > 0x40000) return 0;
        p++;
        if (!load<u8>(p)) return n;
    }
}

static void vAssure(u32 s) { call_ptr<void>(load<u32>(load<u32>(s + 4) + 0x14), s); }

/* buffered dst (header at dst, buffer given) = string src */
static void copyStr(u32 dst, u32 buf, u32 src) {
    vAssure(src);
    s32 n = cstrLength(load<u32>(src));
    const s32 size = s32(load<u32>(dst + 8));
    const u32 fn = load<u32>(load<u32>(src + 4) + 0x14);
    if (n >= size) n = size - 1;
    call_ptr<void>(fn, src);
    call<void>(0xC0009988, buf, load<u32>(src), u32(n), 0u);
    store<u8>(buf + n, 0);
}

/* inline FixedSafeString<256> constructor (placement-new null checks as GHS emits them) */
static void fixed256(u32 q) {
    if (!q) {
        q = call<u32>(0x0273AD10, 0x10Cu);
        if (!q) return;
    }
    u32 b = q;
    if (!b) b = call<u32>(0x0273AD10, 0xCu);
    if (b) {
        store<u32>(b, q + 0xC);
        store<u32>(b + 4, 0x100E0A20);
        store<u32>(b + 8, 0x100);
        store<u8>(q + 0x10B, 0);
    }
    const u32 top = load<u32>(q);
    store<u32>(q + 4, 0x100E0A50);
    store<u8>(top, 0);
    store<u32>(q + 4, 0x100E0A68);
}

/* inline WFixedSafeString<128> constructor */
static void wfixed128(u32 q) {
    if (!q) {
        q = call<u32>(0x0273AD10, 0x10Cu);
        if (!q) return;
    }
    store<u32>(q + 4, 0x100E0A98);
    store<u16>(q + 0x10A, 0);
    store<u16>(q + 0xC, 0);
    store<u32>(q + 8, 0x80);
    store<u32>(q, q + 0xC);
}

static void fill5(u32 p, u32 v) {
    for (u32 i = 0; i < 5; i++) store<u32>(p + 4 * i, v);
}

/* 025F65F4: constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x025F65F4, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x924u);
        if (!p) return 0;
    }
    store<u32>(p + 4, 6);
    store<u32>(p + 8, 0);
    store<u32>(p, 6);
    fixed256(p + 0xC);
    store<u32>(p + 0x11C, 0);
    store<u32>(p + 0x118, 0xFFFFFFFF);
    fixed256(p + 0x120);
    store<u32>(p + 0x22C, 0);
    {
        u32 q = p + 0x230;
        bool ok = true;
        if (!q) {
            q = call<u32>(0x0273AD10, 0x40Cu);
            if (!q) ok = false;
        }
        if (ok) {
            store<u16>(q + 0x40A, 0);
            store<u32>(q, q + 0xC);
            store<u16>(q + 0xC, 0);
            store<u32>(q + 4, 0x100E0A80);
            store<u32>(q + 8, 0x200);
        }
    }
    store<u8>(p + 0x648, 0);
    store<u32>(p + 0x650, 0);
    store<u32>(p + 0x64C, 0);
    store<u32>(p + 0x65C, 0xFFFFFFFF);
    store<u32>(p + 0x640, 0);
    store<u8>(p + 0x659, 0);
    store<u32>(p + 0x63C, 0);
    store<u8>(p + 0x658, 0);
    store<u32>(p + 0x654, 0x14F);
    store<u32>(p + 0x644, 0);
    if (!(p + 0x660)) call<u32>(0x0273AD10, 0x14u);
    if (!(p + 0x674)) call<u32>(0x0273AD10, 0x14u);
    store<u32>(p + 0x6BC, 0);
    store<u8>(p + 0x698, 0);
    store<u32>(p + 0x6B4, 0);
    store<u32>(p + 0x6B0, 1);
    store<u8>(p + 0x695, 0);
    store<u32>(p + 0x68C, 0);
    store<u8>(p + 0x696, 0);
    store<u8>(p + 0x694, 0);
    store<u8>(p + 0x69A, 0);
    store<u8>(p + 0x69B, 0);
    store<u32>(p + 0x6AC, 0);
    store<u32>(p + 0x690, 0);
    store<u32>(p + 0x6B8, 0);
    store<u8>(p + 0x69C, 0);
    store<u8>(p + 0x697, 0);
    const f32 zero = load<f32>(0x100E0AB0);
    store<u8>(p + 0x699, 0);
    store<f32>(p + 0x688, zero);
    wfixed128(p + 0x6C0);
    wfixed128(p + 0x7CC);
    store<f32>(p + 0x8E0, zero);
    store<u32>(p + 0x8D8, 0);
    const f32 one = load<f32>(0x100E0A00);
    store<f32>(p + 0x8E4, zero);
    const f32 minus1 = load<f32>(0x100E0AB4);
    store<u32>(p + 0x8F4, 0xFFFFFFFF);
    store<f32>(p + 0x8E8, one);
    store<f32>(p + 0x8EC, zero);
    store<u8>(p + 0x8DC, 0);
    store<f32>(p + 0x8F0, minus1);
    if (!(p + 0x8F8)) call<u32>(0x0273AD10, 0x14u);
    store<u8>(p + 0x90C, 0);
    store<u8>(p + 0x90D, 0);
    store<u32>(p + 0x91D, 0xFFFFFFFF);
    store<u32>(p + 0x915, 0xFFFFFFFF);
    store<u8>(p + 0x90F, 0);
    store<u32>(p + 0x919, 0xFFFFFFFF);
    store<u8>(p + 0x922, 0);
    store<u32>(p + 0x910, 0);
    store<u8>(p + 0x914, 0);
    store<u8>(p + 0x90E, 0);
    store<u8>(p + 0x923, 0);
    const u32 s1 = load<u32>(p + 0xC);
    store<u8>(p + 0x921, 0);
    store<u8>(s1, 0);
    store<u8>(load<u32>(p + 0x120), 0);
    store<u16>(load<u32>(p + 0x230), 0);
    fill5(p + 0x660, 0xFFFFFFFF);
    for (u32 i = 0; i < 5; i++) store<f32>(p + 0x674 + 4 * i, minus1);
    store<u16>(load<u32>(p + 0x6C0), 0);
    store<u16>(load<u32>(p + 0x7CC), 0);
    fill5(p + 0x8F8, 0);
    return p;
}
VERIFY(0x025F65F4, ctor);

/* 025F69A4: clears the message (strings, ids, layout state) */
void clear(u32 p) {
    WWHD_FUNC(0x025F69A4, void, p);
    const u32 s1 = load<u32>(p + 0xC);
    store<u32>(p + 8, 0);
    store<u8>(s1, 0);
    const u32 s2 = load<u32>(p + 0x120);
    store<u32>(p + 0x11C, 0);
    store<u32>(p + 0x118, 0xFFFFFFFF);
    store<u8>(s2, 0);
    const u32 s3 = load<u32>(p + 0x230);
    store<u32>(p + 0x22C, 0);
    store<u16>(s3, 0);
    store<u8>(p + 0x694, 0);
    store<u8>(p + 0x648, 0);
    store<u8>(p + 0x69C, 0);
    store<u8>(p + 0x658, 0);
    store<u32>(p + 0x644, 0);
    store<u32>(p + 0x640, 4);
    store<u8>(p + 0x69A, 0);
    store<u8>(p + 0x698, 0);
    store<u8>(p + 0x699, 0);
    store<u8>(p + 0x697, 0);
    store<u8>(p + 0x659, 0);
    store<u32>(p + 0x65C, 0xFFFFFFFF);
    store<u32>(p + 0x650, 0);
    store<u8>(p + 0x696, 0);
    store<u8>(p + 0x695, 0);
    store<u32>(p + 0x63C, 0);
    store<u32>(p + 0x64C, 0);
    store<u32>(p + 0x654, 0x14F);
    store<u8>(p + 0x69B, 0);
    store<u32>(p + 0x6A0, load<u32>(0x101FFBA8));
    store<u32>(p + 0x6A4, load<u32>(0x101FFBAC));
    const u32 c2 = load<u32>(0x101FFBB0);
    store<u32>(p + 0x6AC, 0);
    store<u32>(p + 0x6A8, c2);
    store<u32>(p + 0x6B0, 1);
    store<u32>(p + 0x6B4, 0);
    store<u32>(p + 0x6BC, 0);
    const u32 w1 = load<u32>(p + 0x6C0);
    store<u32>(p + 0x6B8, 0);
    store<u16>(w1, 0);
    store<u32>(p + 0x8D8, 0);
    store<u8>(p + 0x90D, 0);
    const f32 zero = load<f32>(0x100E0AB0), one = load<f32>(0x100E0A00);
    store<f32>(p + 0x8E4, zero);
    store<f32>(p + 0x8E0, zero);
    store<f32>(p + 0x8EC, zero);
    store<u8>(p + 0x90F, 0);
    const f32 minus1 = load<f32>(0x100E0AB4);
    store<f32>(p + 0x8E8, one);
    store<f32>(p + 0x8F0, minus1);
    store<u8>(p + 0x90C, 0);
    store<u32>(p + 0x8F4, 0xFFFFFFFF);
    store<u8>(p + 0x90E, 0);
    store<u8>(p + 0x914, 0);
    fill5(p + 0x660, 0xFFFFFFFF);
    for (u32 i = 0; i < 5; i++) store<f32>(p + 0x674 + 4 * i, minus1);
    fill5(p + 0x8F8, 0);
    store<u8>(p + 0x922, 0);
    store<u8>(p + 0x921, 0);
    store<u8>(p + 0x923, 0);
    store<u8>(p + 0x8DC, 0);
}
VERIFY(0x025F69A4, clear);

/* 025F6B04 / 025F6C40: clear with the two state words set */
void reset(u32 p) {
    WWHD_FUNC(0x025F6B04, void, p);
    store<u32>(p, 6);
    store<u32>(p + 4, 0);
    clear(p);
}
VERIFY(0x025F6B04, reset);
void resetIdle(u32 p) {
    WWHD_FUNC(0x025F6C40, void, p);
    store<u32>(p, 0);
    store<u32>(p + 4, 6);
    clear(p);
}
VERIFY(0x025F6C40, resetIdle);

/* label = "%05d" of the message number */
static void setNumberLabel(u32 p, u32 num, u32 fmt) {
    Local<u8[0x110]> F;
    const u32 L = F.a, buf = F.a + 0xC;
    store<u8>(buf + 0xFF, 0);
    store<u32>(L, buf);
    store<u32>(L + 8, 0x100);
    store<u8>(buf, 0);
    store<u32>(L + 4, 0x100E0A68);
    call<void>(0x02759C28, L, fmt, num & 0xFFFF);
    store<u32>(p + 0x11C, num);
    const u32 dst = load<u32>(p + 0x120);
    copyStr(p + 0x120, dst, L);
}

/* 025F6B18: message by number, with an optional sound position */
void setNumber(u32 p, u32 num, u32 sePos) {
    WWHD_FUNC(0x025F6B18, void, p, num, sePos);
    reset(p);
    setNumberLabel(p, num, 0x100E0AB8);
    if (sePos) {
        store<u32>(p + 0x6A0, load<u32>(sePos));
        store<u32>(p + 0x6A4, load<u32>(sePos + 4));
        store<u32>(p + 0x6A8, load<u32>(sePos + 8));
    }
}
VERIFY(0x025F6B18, setNumber);

/* 025F6C54: message by number (idle state) */
void setNumberIdle(u32 p, u32 num) {
    WWHD_FUNC(0x025F6C54, void, p, num);
    resetIdle(p);
    setNumberLabel(p, num, 0x100E0AC0);
}
VERIFY(0x025F6C54, setNumberIdle);

/* 025F6D70: message by set name and label */
void setLabel(u32 p, u32 name, u32 label) {
    WWHD_FUNC(0x025F6D70, void, p, name, label);
    reset(p);
    copyStr(p + 0xC, load<u32>(p + 0xC), name);
    copyStr(p + 0x120, load<u32>(p + 0x120), label);
}
VERIFY(0x025F6D70, setLabel);

/* 025F6EC8: reset */
void reset2(u32 p) {
    WWHD_FUNC(0x025F6EC8, void, p);
    reset(p);
}
VERIFY(0x025F6EC8, reset2);

/* 025F6ECC: restarts the text layout (keeps the message) */
u32 restartLayout(u32 p, u32 a, u32 b) {
    WWHD_FUNC(0x025F6ECC, u32, p, a, b);
    store<u32>(p + 0x654, b);
    store<u32>(p + 0x650, 0);
    const f32 zero = load<f32>(0x100E0AB0);
    store<u8>(p + 0x90F, 0);
    store<f32>(p + 0x8E0, zero);
    store<u32>(p + 0x64C, a);
    store<f32>(p + 0x8E4, zero);
    store<u8>(p + 0x648, 0);
    store<u8>(p + 0x658, 0);
    store<u8>(p + 0x90E, 0);
    store<u8>(p + 0x659, 0);
    store<u8>(p + 0x914, 0);
    store<u8>(p + 0x90D, 0);
    store<u32>(p + 0x644, 0);
    store<u8>(p + 0x90C, 0);
    fill5(p + 0x660, 0xFFFFFFFF);
    const f32 minus1 = load<f32>(0x100E0AB4);
    for (u32 i = 0; i < 5; i++) store<f32>(p + 0x674 + 4 * i, minus1);
    fill5(p + 0x8F8, 0);
    store<u8>(p + 0x922, 0);
    store<u8>(p + 0x923, 0);
    return p;
}
VERIFY(0x025F6ECC, restartLayout);

/* 025F6F68: restarts the layout and the font/scale state */
void restart(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x025F6F68, void, self, a, b);
    const u32 p = restartLayout(self, a, b);
    const u32 w = load<u32>(p + 0x6C0);
    const f32 minus1 = load<f32>(0x100E0AB4);
    store<u32>(p + 0x6B8, 0);
    store<u32>(p + 0x6BC, 0);
    store<u16>(w, 0);
    store<u32>(p + 0x8D8, 0);
    store<f32>(p + 0x8F0, minus1);
    const f32 zero = load<f32>(0x100E0AB0), one = load<f32>(0x100E0A00);
    store<f32>(p + 0x8EC, zero);
    store<f32>(p + 0x8E8, one);
    store<u32>(p + 0x8F4, 0xFFFFFFFF);
}
VERIFY(0x025F6F68, restart);

/* 025F6FCC: restart in state 6 */
void restart6(u32 p, u32 a, u32 b) {
    WWHD_FUNC(0x025F6FCC, void, p, a, b);
    store<u32>(p, 6);
    restart(p, a, b);
}
VERIFY(0x025F6FCC, restart6);

/* 025F6FD8: restart, also setting the play object's message state byte (+0x5BB3) */
void restartPlay(u32 p, u32 a, u32 b) {
    WWHD_FUNC(0x025F6FD8, void, p, a, b);
    store<u32>(p + 4, 6);
    const u32 g = call<u32>(0x025200D4);
    store<u8>(g + 0x5BB3, 6);
    restart(p, a, b);
}
VERIFY(0x025F6FD8, restartPlay);

/* 025F7040: restart the layout in state 6 */
void restartLayout6(u32 p, u32 a, u32 b) {
    WWHD_FUNC(0x025F7040, void, p, a, b);
    store<u32>(p, 6);
    restartLayout(p, a, b);
}
VERIFY(0x025F7040, restartLayout6);

/* 025F704C: the message's text style from its MSBT attribute (project style table) */
void updateStyle(u32 p) {
    WWHD_FUNC(0x025F704C, void, p);
    const u32 set = load<u32>(p + 8);
    const u32 idx = load<u32>(p + 0x118);
    if (idx >= load<u32>(set + 4)) return;
    const s32 st = s32(call<u32>(0x0273A598, load<u32>(set), idx));
    if (st < 0) return;
    const u32 prj = call<u32>(0x025F4BC0, load<u32>(0x101F4AE8));
    u32 e = load<u32>(prj + 0x10);
    if (u32(st) < load<u32>(prj + 0xC)) e += u32(st) << 4;
    store<u32>(p + 0x640, load<u32>(e + 4));
}
VERIFY(0x025F704C, updateStyle);

/* 025F70CC: starts drawing: font (the pictograph font for 0x0C-tag entries), style, scale */
void beginDraw(u32 p, u32 font, u32 scale) {
    WWHD_FUNC(0x025F70CC, void, p, font, scale);
    const s32 idx = s32(load<u32>(p + 0x118));
    store<u32>(p + 0x68C, 0);
    store<u32>(p + 0x690, font);
    if (idx >= 0) {
        const u32 e = load<u32>(p + 0x22C);
        if (e && load<u8>(e + 1) == 0xC && !load<u8>(load<u32>(0x101F84DC) + 0x1C0)) {
            store<u32>(p + 0x690, call<u32>(0x025F36B8, load<u32>(0x101F4A50), 3u));
            store<u8>(p + 0x8DC, 1);
        }
        updateStyle(p);
    }
    store<f32>(p + 0x8E0, load<f32>(scale));
    store<f32>(p + 0x8E4, load<f32>(scale + 4));
}
VERIFY(0x025F70CC, beginDraw);

/* 025F7178: advances the current line width by a character of the given font */
void advance(u32 p, u32 ch, u32 font) {
    WWHD_FUNC(0x025F7178, void, p, ch, font);
    const s32 w = s32(call_ptr<u32>(load<u32>(load<u32>(font + 4) + 0x7C), font, ch));
    const u32 cur = load<u32>(p + 0x690);
    const u32 fn = load<u32>(load<u32>(cur + 4) + 0x1C);
    const f32 sc = fmuls_ppc(load<f32>(p + 0x8E0), load<f32>(p + 0x8E8));
    const s32 h = s32(call_ptr<u32>(fn, cur));
    const f32 hf = f32(f64(h)), wf = f32(f64(w));
    const f32 q = sc / hf;
    store<f32>(p + 0x8EC, fmadds(wf, q, load<f32>(p + 0x8EC)));
}
VERIFY(0x025F7178, advance);

/* 025F7240: advance by a character of the current font */
void advanceCur(u32 p, u32 chp) {
    WWHD_FUNC(0x025F7240, void, p, chp);
    advance(p, load<u16>(chp), load<u32>(p + 0x690));
}
VERIFY(0x025F7240, advanceCur);

/* 025F724C: advance by a character of the pictograph font */
void advancePic(u32 p, u32 chp) {
    WWHD_FUNC(0x025F724C, void, p, chp);
    const u32 f = call<u32>(0x025F36B8, load<u32>(0x101F4A50), 4u);
    advance(p, load<u16>(chp), f);
}
VERIFY(0x025F724C, advancePic);

/* 025F72A0: ends a line: records its value and width (at most 5 lines) */
void endLine(u32 p, u32 v) {
    WWHD_FUNC(0x025F72A0, void, p, v);
    u32 i = load<u32>(p + 0x644);
    store<u32>(i < 5 ? p + 0x660 + 4 * i : p + 0x660, v);
    i = load<u32>(p + 0x644);
    const f32 w = load<f32>(p + 0x8EC);
    store<f32>(i < 5 ? p + 0x674 + 4 * i : p + 0x674, w);
    s32 n = s32(load<u32>(p + 0x644)) + 1;
    if (n > 5) n = 5;
    const f32 zero = load<f32>(0x100E0AB0);
    store<u32>(p + 0x644, u32(n));
    store<f32>(p + 0x8EC, zero);
}
VERIFY(0x025F72A0, endLine);

void staticInit() {
    WWHD_FUNC(0x025F7304, void);
    const u32 b = 0x1048D70C;
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, 0x101F4AF0u);
    const f32 lo = load<f32>(0x100E0AD0), hi = load<f32>(0x100E0AD4);
    store<f32>(0x1048D700, lo);
    store<f32>(0x1048D704, hi);
    call<void>(0x028ED6F8, 0x1048D708u);
    call<void>(0x028F026C, 0x101F4AFCu);
    call<void>(0x028EAB2C, 0x1048D709u);
    call<void>(0x028F026C, 0x101F4B08u);
}
VERIFY(0x025F7304, staticInit);

} // namespace hd_msg_text

/* hd_text_unit_proc: HD message text builder, part 4: tag dispatch, ruby text, the text loop and the
 * static initialiser (0260001C..02601A6F), WWHD. HD-only
 * code (no GameCube source): written from the WWHD code. See hd_text_unit.cpp. */
#include "hd_text_unit.h"

namespace hd_text_unit {

/* 0260001C: puts a number with full-width (Shift-JIS) digits */
s32 putNumberFull(u32 self, u32 w, s32 pos, s32 v, u32 maxDigits, u32 noPad) {
    WWHD_FUNC(0x0260001C, s32, self, w, pos, v, maxDigits, noPad);
    static const u32 lits[10] = {0x100E1774, 0x100E1777, 0x100E177A, 0x100E177D, 0x100E1780,
                                 0x100E1783, 0x100E1786, 0x100E1789, 0x100E178C, 0x100E178F};
    staticArr(0x1048DCBC, 0x1048DAE4, lits, 10, kSafeVt, 0x101F4E68);
    return s32(call<u32>(0x025FC708, self, w, u32(pos), u32(v), 0x1048DAE4u, maxDigits, noPad));
}
VERIFY(0x0260001C, putNumberFull);

/* 0260026C: number tag: one of the five message values (+0x8F8), ASCII or full-width digits */
s32 putValueTag(u32 self, u32 w, s32 pos, u32 tag) {
    WWHD_FUNC(0x0260026C, s32, self, w, pos, tag);
    const u32 k = load<u16>(tag + 4) - 0x28;
    const u32 ascii = load<u8>(tag + 0xA);
    const u32 maxD = load<u8>(tag + 8);
    const u32 m = msg(self);
    const u32 noPad = load<u8>(tag + 9);
    const s32 v = s32(load<u32>(k < 5 ? m + 0x8F8 + 4 * k : m + 0x8F8));
    if (ascii) return putNumberAscii(self, w, pos, v, maxD, noPad);
    return putNumberFull(self, w, pos, v, maxD, noPad);
}
VERIFY(0x0260026C, putValueTag);

/* 026002E4: tag group 2: inserted values (names, counters, timers, messages, item units) */
s32 group2(u32 self, u32 w, s32 pos, u32 tag, u32 a7, u32 a8) {
    WWHD_FUNC(0x026002E4, s32, self, w, pos, tag, a7, a8);
    static const u32 fns[0x28] = {
        0x025FC444, 0x025FD05C, 0x025FD124, 0x025FD800, 0x025FD86C, 0x025FD124, 0x025FD124, 0x025FD8D8,
        0x025FDD80, 0x025FDEAC, 0x025FDF28, 0x025FD124, 0x025FE158, 0x025FE284, 0x025FE7F4, 0x025FE858,
        0x025FE858, 0x025FE960, 0x025FEA7C, 0x025FEBA8, 0x025FECF4, 0x025FD124, 0x025FED50, 0x025FEE7C,
        0x025FDF8C, 0x025FE000, 0x025FE064, 0x025FEF7C, 0x025FF0A0, 0x025FF1C4, 0x025FF2E8, 0x025FF40C,
        0x025FF530, 0x025FF654, 0x025FFB24, 0x025FFBF8, 0x025FFCCC, 0x025FFDA0, 0x025FFE74, 0x025FFF48};
    const u32 t = load<u16>(tag + 4);
    if (t > 0x2C) return 0;
    if (t >= 0x28) return s32(call<u32>(0x0260026C, self, w, u32(pos), tag, a7, a8));
    const u32 r6 = (t == 3 || t == 4 || t == 9) ? a8 : a7;
    return s32(call<u32>(fns[t], self, w, u32(pos), r6, a7, a8));
}
VERIFY(0x026002E4, group2);

/* 0260046C: sound tag (at the message's sound position, with the room's reverb) */
s32 tagSound(u32 self, u32 tag) {
    WWHD_FUNC(0x0260046C, s32, self, tag);
    if (!call<u32>(0x025FB18C, self)) return 0;
    u32 m = msg(self);
    store<u32>(m + 0x65C, load<u32>(m + 0x64C));
    m = msg(self);
    Local<f32[3]> P;
    const s32 room = s32(s8(load<u8>(0x1047E6C8)));
    store<f32>(P.a, load<f32>(m + 0x6A0));
    store<f32>(P.a + 4, load<f32>(m + 0x6A4));
    const u32 se = load<u16>(tag + 4);
    store<f32>(P.a + 8, load<f32>(m + 0x6A8));
    if (room) {
        const u32 rev = call<u32>(0x02520540, u32(room));
        call<void>(0x025E1C3C, se, se == 8 ? 0u : P.a, rev);
    } else {
        call<void>(0x025E1C3C, se, se == 8 ? 0u : P.a, 0u);
    }
    return 0;
}
VERIFY(0x0260046C, tagSound);

/* 02600548 / 02600598: play-info tags (a value / a camera mode byte) */
s32 tagPlayValue(u32 self, u32 tag) {
    WWHD_FUNC(0x02600548, s32, self, tag);
    if (call<u32>(0x025FB18C, self)) {
        const u32 m = msg(self);
        store<u32>(m + 0x65C, load<u32>(m + 0x64C));
        const u32 v = load<u16>(tag + 4);
        store<u32>(call<u32>(0x025200D4) + 0x5C30, v);
    }
    return 0;
}
VERIFY(0x02600548, tagPlayValue);
s32 tagPlayByte(u32 self, u32 tag) {
    WWHD_FUNC(0x02600598, s32, self, tag);
    if (call<u32>(0x025FB18C, self)) {
        const u32 m = msg(self);
        store<u32>(m + 0x65C, load<u32>(m + 0x64C));
        u32 v = load<u8>(tag + 5);
        if (v >= 7) v = (v + 4) & 0xFF;
        store<u8>(call<u32>(0x025200D4) + 0x5BC6, u8(v));
    }
    return 0;
}
VERIFY(0x02600598, tagPlayByte);

/* 026005F8: tag dispatch by group */
s32 tagDispatch(u32 self, u32 w, s32 pos, u32 tag, u32 a7, u32 a8) {
    WWHD_FUNC(0x026005F8, s32, self, w, pos, tag, a7, a8);
    const u32 g = load<u16>(tag + 2);
    if (g < 4) {
        if (g < 1) return s32(call<u32>(0x025FB10C, self, w, u32(pos), tag, a7, a8));
        if (g == 1) return s32(call<u32>(0x025FB928, self, w, u32(pos), tag, a7, a8));
        if (g == 2) return group2(self, w, pos, tag, a7, a8);
        return -1;
    }
    if (g < 5) return tagSound(self, tag);
    if (g == 5) return tagPlayValue(self, tag);
    if (g == 6) return tagPlayByte(self, tag);
    return -1;
}
VERIFY(0x026005F8, tagDispatch);

/* 02600660: pictograph tag: advances the line by the glyph (line break when it does not fit) */
s32 pictTag(u32 self, u32 w, u32 posp, u32 hdr, u32 tag) {
    WWHD_FUNC(0x02600660, s32, self, w, posp, hdr, tag);
    if (load<u16>(hdr + 2) != 3) return 0;
    s32 r = 0;
    Local<u16> C;
    store<u16>(C.a, u16(call<u32>(0x025F90C8, load<u32>(0x101F4A50) + 0x2C, u32(load<u8>(hdr + 5)))));
    call<void>(0x025F724C, msg(self), C.a);
    const u32 m = msg(self);
    const f32 lim = load<f32>(m + 0x688);
    if (!(lim == load<f32>(0x100E12FC)) && load<f32>(m + 0x8EC) > lim && !load<u8>(m + 0x69A)) {
        r = s32(call<u32>(0x025FBFE8, self, w, load<u32>(posp), tag + 2));
        if (r < 0) return -1;
        store<u32>(posp, load<u32>(posp) + u32(r));
        call<void>(0x025F724C, msg(self), C.a);
    }
    return r;
}
VERIFY(0x02600660, pictTag);

/* 02600760: collects the ruby base text (tags, characters, at most 1 or 2 line breaks) into +0x6C0 */
void collectRubyBase(u32 self, u32 text) {
    WWHD_FUNC(0x02600760, void, self, text);
    const u32 m = msg(self);
    const u32 dst = m + 0x6C0;
    s32 breaks = load<u32>(m) == 8 ? 1 : 2;
    store<u16>(load<u32>(m + 0x6C0), 0);
    store<u32>(msg(self) + 0x8D8, 0);
    s32 n = 0;
    Local<u32[2]> T8, T18, T20, T10;
    u32 c = load<u16>(text);
    while (c) {
        if (c == 0xE || c == 0xF) {
            u32 next;
            if (c == 0xE) next = text + load<u16>(text + 6) + 8;
            else next = text + 6;
            const s32 k = s32(next - text) >> 1;
            store<u32>(T8.a, text);
            store<u32>(T8.a + 4, kWSafeVt);
            wcopyAt(dst, n, T8.a, k);
            text = next;
            c = load<u16>(text);
            n += k;
        } else if (c == 0xA) {
            if (breaks) {
                store<u32>(T18.a, text);
                store<u32>(T18.a + 4, kWSafeVt);
                wcopyAt(dst, n, T18.a, 1);
                breaks--;
                n++;
            }
            text += 2;
            c = load<u16>(text);
        } else {
            store<u32>(T20.a, text);
            store<u32>(T20.a + 4, kWSafeVt);
            wcopyAt(dst, n, T20.a, 1);
            text += 2;
            c = load<u16>(text);
            n++;
        }
    }
    wtmp(T10.a, 0x100E0F8E);
    wcopyAt(dst, n, T10.a, 1);
    store<u32>(msg(self) + 0x8D8, u32(n + 1));
}
VERIFY(0x02600760, collectRubyBase);

/* 02600C48: copies ruby text into dst up to the ruby end tag (group 1, tag 9/10), spaces skipped;
 * returns the text there */
u32 copyRubyText(u32 self, u32 dst, u32 text) {
    WWHD_FUNC(0x02600C48, u32, self, dst, text);
    store<u16>(load<u32>(dst), 0);
    store<u32>(msg(self) + 0x8D8, 0);
    s32 n = 0;
    Local<u32[2]> T8, T18, T10;
    u32 c = load<u16>(text);
    while (c) {
        if (c == 0xE || c == 0xF) {
            u32 hdr, next;
            if (c == 0xE) {
                hdr = text;
                next = text + load<u16>(text + 6) + 8;
            } else {
                hdr = text;
                next = text + 6;
            }
            if (load<u16>(hdr + 2) == 1) {
                const u32 t = load<u16>(hdr + 4);
                if (t == 9 || t == 0xA) break;
            }
            const s32 k = s32(next - text) >> 1;
            store<u32>(T8.a, text);
            store<u32>(T8.a + 4, kWSafeVt);
            wcopyAt(dst, n, T8.a, k);
            text = next;
            c = load<u16>(text);
            n += k;
        } else if (c == 0x20) {
            text += 2;
            c = load<u16>(text);
        } else {
            store<u32>(T18.a + 4, kWSafeVt);
            store<u32>(T18.a, text);
            wcopyAt(dst, n, T18.a, 1);
            n++;
            text += 2;
            c = load<u16>(text);
        }
    }
    wtmp(T10.a, 0x100E0F8E);
    wcopyAt(dst, n, T10.a, 1);
    store<u32>(msg(self) + 0x8D8, u32(n + 1));
    return text;
}
VERIFY(0x02600C48, copyRubyText);

/* 02601060: processes one tag (pictograph advance, dispatch, raw copy, ruby collection, box check);
 * returns the text after it (0 to stop) */
u32 processTag(u32 self, u32 w, u32 posp, u32 tag, u32 ctx) {
    WWHD_FUNC(0x02601060, u32, self, w, posp, tag, ctx);
    const u32 c = load<u16>(tag);
    u32 hdr, next;
    if (c == 0xE) {
        hdr = tag;
        next = tag + load<u16>(tag + 6) + 8;
    } else if (c == 0xF) {
        hdr = tag;
        next = tag + 6;
    } else {
        hdr = 0;
        next = tag;
    }
    const s32 r = pictTag(self, w, posp, hdr, tag);
    if (r < 0) return 0;
    const u32 pos = load<u32>(posp) + u32(r);
    store<u32>(posp, pos);
    s32 put = s32(call<u32>(0x026005F8, self, w, pos, hdr, tag, ctx));
    const s32 k = s32(next - tag) >> 1;
    if (put < 0) {
        Local<u32[2]> T;
        store<u32>(T.a, tag);
        store<u32>(T.a + 4, kWSafeVt);
        put = k;
        wcopyAt(w, s32(load<u32>(posp)), T.a, k);
    }
    store<u32>(posp, load<u32>(posp) + u32(put));
    u32 m = msg(self);
    store<u32>(m + 0x64C, load<u32>(m + 0x64C) + u32(k));
    m = msg(self);
    if (load<u32>(m + 0x6B8) == 1) {
        collectRubyBase(self, next);
        return 0;
    }
    if (load<u32>(m + 0x6B8) == 2) {
        if (!load<u32>(m + 0x8D8)) return copyRubyText(self, m + 0x6C0, next);
        copyRubyText(self, m + 0x7CC, next);
        return 0;
    }
    if (call<u32>(0x025FBE54, self, w, load<u32>(posp) - 1, next)) return 0;
    return next;
}
VERIFY(0x02601060, processTag);

/* 0260137C: inserts the "line start" tag (group 1 tag 0xD) */
s32 insertLineTag(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x0260137C, s32, self, w, pos);
    Local<u8[0x4C]> L;
    wfixed(L.a, kWFixed32Vt, 0x20);
    const u32 b = load<u32>(L.a);
    store<u16>(b, 0xE);
    store<u16>(b + 2, 1);
    store<u16>(b + 4, 0xD);
    store<u16>(b + 6, 0);
    wcopyAt(w, pos, L.a, 4);
    return 4;
}
VERIFY(0x0260137C, insertLineTag);

/* 026014C8: inserts it once when the first visible character is reached */
void markLineStart(u32 self, u32 w, u32 posp) {
    WWHD_FUNC(0x026014C8, void, self, w, posp);
    const u32 m = msg(self);
    if (load<u8>(m + 0x658) || load<u8>(m + 0x696) || load<u8>(m + 0x699)) return;
    if (s32(load<u32>(m + 0x650)) < s32(load<u32>(m + 0x654)) && !load<u32>(m + 0x6AC)) return;
    const s32 n = insertLineTag(self, w, s32(load<u32>(posp)));
    store<u32>(posp, load<u32>(posp) + u32(n));
    store<u8>(msg(self) + 0x658, 1);
}
VERIFY(0x026014C8, markLineStart);

/* 02601564: the text loop: tags, line breaks and characters; then the last line and the cut */
u32 processText(u32 self, u32 w, u32 posp, u32 text, u32 ctx) {
    WWHD_FUNC(0x02601564, u32, self, w, posp, text, ctx);
    u32 c = load<u16>(text);
    while (c) {
        if (c == 0xE || c == 0xF) {
            store<u8>(msg(self) + 0x648, 1);
            const u32 r = processTag(self, w, posp, text, ctx);
            if (!r) break;
            text = r;
            markLineStart(self, w, posp);
        } else if (c == 0xA) {
            const s32 r = s32(call<u32>(0x025FBFE8, self, w, load<u32>(posp), text));
            const u32 m = msg(self);
            store<u32>(m + 0x64C, load<u32>(m + 0x64C) + 1);
            if (r < 0) return text;
            text += 2;
            store<u32>(posp, load<u32>(posp) + u32(r));
            markLineStart(self, w, posp);
        } else {
            store<u8>(msg(self) + 0x648, 1);
            const s32 r = s32(call<u32>(0x025FC1F4, self, w, load<u32>(posp), text));
            if (r < 0) return text;
            store<u32>(posp, load<u32>(posp) + u32(r));
            const u32 m = msg(self);
            text += 2;
            store<u32>(m + 0x64C, load<u32>(m + 0x64C) + 1);
            markLineStart(self, w, posp);
        }
        c = load<u16>(text);
    }
    store<u8>(msg(self) + 0x659, 1);
    const u32 m = msg(self);
    const u32 n = load<u32>(m + 0x644);
    if (!n) {
        call<void>(0x025F72A0, m, load<u32>(posp));
    } else {
        const u32 last = load<u32>(n - 1 < 5 ? m + 0x660 + 4 * (n - 1) : m + 0x660);
        if (load<u32>(posp) != last + 1) call<void>(0x025F72A0, m, load<u32>(posp));
    }
    call<void>(0x025FBA64, self, w);
    return text;
}
VERIFY(0x02601564, processText);

/* 02601710: after the text: end sound / state unless waiting */
void finishText(u32 self, u32 text) {
    WWHD_FUNC(0x02601710, void, self, text);
    const u32 m = msg(self);
    if (load<u32>(m + 0x6B8) || load<u8>(m + 0x658)) return;
    if (load<u8>(m + 0x699)) {
        store<u8>(m + 0x699, 0);
        return;
    }
    if (load<u8>(m + 0x659)) {
        call<void>(0x025FA938, self);
        call<void>(0x025FA9E8, self);
        return;
    }
    if (load<u8>(m + 0x69C) || load<u8>(m + 0x696)) return;
    u32 mm = m;
    if (!load<u8>(m + 0x697) && !load<u8>(m + 0x698)) {
        call<void>(0x025FA938, self);
        mm = msg(self);
    }
    store<u32>(mm, 7);
}
VERIFY(0x02601710, finishText);

/* 026017F0: builds the text box string of a message from its current position; returns the length */
s32 build(u32 self, u32 w, u32 text, u32 m, u32 ctx) {
    WWHD_FUNC(0x026017F0, s32, self, w, text, m, ctx);
    Local<u32> P;
    store<u32>(self, m);
    const u32 e = load<u32>(m + 0x22C);
    store<u32>(P.a, 0);
    const u32 start = load<u32>(m + 0x64C);
    if (e && load<u8>(e + 2) == 1) store<u8>(m + 0x696, 1);
    text += start << 1;
    if (!call<u32>(0x025FAA1C, self, text)) {
        const u32 r = processText(self, w, P.a, text, ctx);
        finishText(self, r);
    }
    return s32(load<u32>(P.a));
}
VERIFY(0x026017F0, build);

/* 026018B0: static initialiser: header statics and the colour constants */
void staticInit() {
    WWHD_FUNC(0x026018B0, void);
    const u32 b = 0x1048DAD4;
    store<u32>(b + 0xC, 0);
    store<u32>(b + 8, 0);
    store<u32>(b + 4, 0);
    store<u32>(b, 0);
    call<void>(0x028F026C, 0x101F4E74u);
    const f32 lo = load<f32>(0x100E1794), hi = load<f32>(0x100E1798);
    store<f32>(0x1048D960, lo);
    store<f32>(0x1048D964, hi);
    call<void>(0x028ED6F8, 0x1048D970u);
    call<void>(0x028F026C, 0x101F4E80u);
    call<void>(0x028EAB2C, 0x1048D971u);
    call<void>(0x028F026C, 0x101F4E8Cu);
    static const u32 tab[27] = {0x000000FF, 0xB40000FF, 0x008282FF, 0x0000AAFF, 0xF0F01EFF, 0x82FFFFFF, 0x6400FFFF, 0x505050FF, 0xFFB400FF, 0x00000000, 0xB4000000, 0x00828200, 0x0000AA00, 0xF0F01E00, 0x82FFFF00, 0x6400FF00, 0x50505000, 0xFFB40000, 0x000000FF, 0xB40000FF, 0x008282FF, 0x0000AAFF, 0xE67D0FFF, 0x82FFFFFF, 0x6400FFFF, 0x3C3C3CFF, 0xFFB400FF};
    for (u32 i = 0; i < 27; i++) store<u32>(0x1048DB84 + 4 * i, tab[i]);
    for (u32 i = 0; i < 4; i++) store<u8>(0x1048D96C + i, 0xFF);
    store<u8>(0x1048D968, 0xFF);
    store<u8>(0x1048D969, 0);
    store<u8>(0x1048D96A, 0);
    store<u8>(0x1048D96B, 0xFF);
}
VERIFY(0x026018B0, staticInit);

} // namespace hd_text_unit

/* hd_text_unit_line: HD message text builder, part 2: line breaking, characters, numbers and unit
 * strings (025FB9BC..025FD05B), WWHD. HD-only code (no
 * GameCube source): written from the WWHD code. See hd_text_unit.cpp. */
#include "hd_text_unit.h"

namespace hd_text_unit {

/* 025FB9BC: are the characters between two line ends only tags (no pictograph tag)? */
u32 onlyTagsBetween(u32 self, u32 buf, s32 start, s32 end) {
    WWHD_FUNC(0x025FB9BC, u32, self, buf, start, end);
    s32 i = start + 1;
    if (i >= end) return 1;
    do {
        const u32 p = buf + u32(i) * 2;
        const u32 c = load<u16>(p);
        if (c != 0xE && c != 0xF) return 0;
        const u32 c0 = load<u16>(p);
        u32 next;
        if (c0 == 0xE) {
            next = p + load<u16>(p + 6) + 8;
            if (load<u16>(p + 2) == 3) return 0;
        } else if (c0 == 0xF) {
            if (load<u16>(p + 2) == 3) return 0;
            next = p + 6;
        } else {
            if (load<u16>(2) == 3) return 0;
            next = p;
        }
        i = i + ((s32(next - p) >> 1) - 1) + 1;
    } while (i < end);
    return 1;
}
VERIFY(0x025FB9BC, onlyTagsBetween);

/* 025FBA64: cuts the text after the last recorded line end and removes the breaks between lines */
void cutLines(u32 self, u32 w) {
    WWHD_FUNC(0x025FBA64, void, self, w);
    u32 m = msg(self);
    const s32 n = s32(load<u32>(m + 0x644));
    if (!n) return;
    const u32 i0 = u32(n - 1);
    s32 cur = s32(load<u32>(i0 < 5 ? m + 0x660 + 4 * i0 : m + 0x660));
    Local<u32[2]> T18;
    wtmp(T18.a, 0x100E0F8E);
    wcopyAt(w, cur, T18.a, 1);
    if (n < 2) return;
    s32 k = n - 2;
    Local<u32[2]> T10;
    Local<u32[2]> T8;
    for (s32 left = n - 1; left; left--, k--) {
        m = msg(self);
        const s32 prev = cur;
        cur = s32(load<u32>(u32(k) < 5 ? m + 0x660 + 4 * u32(k) : m + 0x660));
        s32 gap = prev - cur;
        if (gap <= 1) {
            wtmp(T10.a, 0x100E0F8E);
            wcopyAt(w, cur, T10.a, 1);
            continue;
        }
        vAssure(w);
        const u32 buf = load<u32>(w);
        if (!call<u32>(0x025FB9BC, self, buf, u32(cur), u32(prev))) return;
        if (cur < prev) {
            u32 src = buf + u32(cur) * 2;
            for (; gap; gap--, cur++, src += 2) {
                store<u32>(T8.a + 4, kWSafeVt);
                store<u32>(T8.a, src + 2);
                wcopyAt(w, cur, T8.a, 1);
            }
        }
        wtmp(T10.a, 0x100E0F8E);
        cur = prev - 1;
        wcopyAt(w, cur, T10.a, 1);
    }
}
VERIFY(0x025FBA64, cutLines);

/* 025FBE54: is the box full (line count reached)? then the text is cut there */
u32 boxFull(u32 self, u32 w) {
    WWHD_FUNC(0x025FBE54, u32, self, w);
    const u32 m = msg(self);
    const u32 num = load<u32>(m + 0x11C);
    s32 maxLines = s32(load<u32>(m + 0x640));
    if (num == 0xFFFFFFF0 || (num <= 0xEE3 && (num >= 0xEE2 || num == 0xEDD || num == 0x266))) maxLines++;
    if (s32(load<u32>(m + 0x644)) < maxLines) return 0;
    if (!load<u32>(m + 0x11C) && maxLines == 1) {
        /* debug report "[WARNING!] Over Style Setting!" (the print itself is compiled out) */
        Local<u8[0x110]> F;
        const u32 s = F.a, buf = F.a + 0xC;
        store<u8>(buf + 0xFF, 0);
        store<u32>(s, buf);
        store<u8>(buf, 0);
        store<u32>(s + 8, 0x100);
        store<u32>(s + 4, 0x100E0FF0);
        call<void>(0x02759C28, s, 0x100E10D8u);
        vAssure(s);
        const u32 mm = msg(self);
        vAssure(mm + 0x120);
        call<void>(0x02759C28, s, 0x100E10F8u, load<u32>(mm + 0x120));
        vAssure(s);
        return 0;
    }
    vAssure(w);
    call<void>(0x025FBA64, self, w);
    store<u8>(msg(self) + 0x69C, 0);
    store<u8>(msg(self) + 0x696, 0);
    return 1;
}
VERIFY(0x025FBE54, boxFull);

/* 025FBFE8: line break: records the line, inserts '\n' when the box takes it; -1 when full */
s32 newLine(u32 self, u32 w, s32 pos, u32 next) {
    WWHD_FUNC(0x025FBFE8, s32, self, w, pos, next);
    s32 r = 0;
    call<void>(0x025F72A0, msg(self), u32(pos));
    if (load<u8>(msg(self) + 0x648)) {
        Local<u32[2]> T;
        wtmp(T.a, 0x100E0F8C);
        wcopyAt(w, pos, T.a, 1);
        r = 1;
        pos++;
    }
    if (call<u32>(0x025FBE54, self, w, u32(pos), next)) r = -1;
    return r;
}
VERIFY(0x025FBFE8, newLine);

/* 025FC154: character replacement table (font-specific forms) */
u32 mapChar(u32 self, u32 c) {
    WWHD_FUNC(0x025FC154, u32, self, c);
    for (u32 i = 0; i < 0x60; i++)
        if (load<u16>(0x100E1114 + 4 * i) == c) return load<u16>(0x100E1116 + 4 * i);
    return c;
}
VERIFY(0x025FC154, mapChar);

/* 025FC184: capitalises the character after a capital tag (ASCII and the accented table) */
void capitalize(u32 self, u32 chp) {
    WWHD_FUNC(0x025FC184, void, self, chp);
    const u32 m = msg(self);
    const u32 f = load<u8>(m + 0x922);
    const u32 c = load<u16>(chp);
    if (!f) return;
    store<u8>(m + 0x922, 0);
    if (c - 0x61 < 0x1A) {
        store<u16>(chp, u16(c - 0x20));
        return;
    }
    for (u32 i = 0; i < 0x19; i++)
        if (c == load<u16>(0x100E1294 + 2 * i)) {
            store<u16>(chp, load<u16>(0x100E12C8 + 2 * i));
            return;
        }
}
VERIFY(0x025FC184, capitalize);

/* 025FC1F4: puts one character (line break when it does not fit) */
s32 putChar(u32 self, u32 w, s32 pos, u32 chp) {
    WWHD_FUNC(0x025FC1F4, s32, self, w, pos, chp);
    Local<u8[0xC]> L; /* +0 the character, +4 WSafeString over it */
    const u32 ch = L.a, T = L.a + 4;
    const u32 m0 = msg(self);
    store<u16>(ch, load<u16>(chp));
    s32 r = 0;
    if (load<u8>(m0 + 0x8DC)) store<u16>(ch, u16(call<u32>(0x025FC154, self, u32(load<u16>(chp)))));
    call<void>(0x025F7240, m0, ch);
    const u32 m = msg(self);
    const f32 lim = load<f32>(m + 0x688);
    if (!(lim == load<f32>(0x100E12FC)) && load<f32>(m + 0x8EC) > lim && !load<u8>(m + 0x69A)) {
        r = s32(call<u32>(0x025FBFE8, self, w, u32(pos), chp + 2));
        if (r < 0) return -1;
        pos += r;
        call<void>(0x025F7240, msg(self), ch);
    }
    call<void>(0x025FC184, self, ch);
    wtmp(T, ch);
    wcopyAt(w, pos, T, 1);
    const u32 mm = msg(self);
    store<u32>(mm + 0x650, load<u32>(mm + 0x650) + 1);
    return r + 1;
}
VERIFY(0x025FC1F4, putChar);

/* 025FC3D0: puts n characters (stops at a full box) */
s32 putCharsF(u32 self, u32 w, s32 pos, u32 chars, s32 n) {
    WWHD_FUNC(0x025FC3D0, s32, self, w, pos, chars, n);
    s32 total = 0;
    for (; n > 0; n--, chars += 2) {
        const s32 r = putChar(self, w, pos, chars);
        if (r < 0) break;
        total += r;
        pos += r;
    }
    return total;
}
VERIFY(0x025FC3D0, putCharsF);

/* 025FC444: puts the player's name (the default name when it is empty) */
s32 putPlayerName(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FC444, s32, self, w, pos);
    const u32 name = call<u32>(0x02720170, saveInfo() + 0x12C0);
    vAssure(name);
    Local<u8[0x54]> F; /* +0 WSafeString over the name, +8 WFixedSafeString<32> */
    const u32 T = F.a, W = F.a + 8, Wbuf = F.a + 0x14;
    store<u32>(T, load<u32>(name));
    store<u16>(Wbuf + 0x3E, 0);
    store<u32>(W + 8, 0x20);
    store<u32>(W + 4, 0x100E1038);
    store<u32>(W, Wbuf);
    store<u32>(T + 4, kWSafeVt);
    call<void>(kWNoAssure, T);
    s32 n = wcsLength(load<u32>(T));
    if (n >= s32(load<u32>(W + 8))) n = s32(load<u32>(W + 8)) - 1;
    vAssure(T);
    call<void>(kMove, Wbuf, load<u32>(T), u32(n) * 2, 0u);
    store<u16>(Wbuf + u32(n) * 2, 0);
    store<u32>(W + 4, kWFixed32Vt);
    call<void>(kWFixedAssure, W);
    u32 top = load<u32>(W);
    if (!wcsLength(top)) {
        staticStr(0x1048DBF4, 0x1048D888, 0x100E1300, kWSafeVt, 0x101F4C10);
        top = load<u32>(W);
        vAssure(0x1048D888);
        s32 k = wcsLength(load<u32>(0x1048D888));
        if (k >= s32(load<u32>(W + 8))) k = s32(load<u32>(W + 8)) - 1;
        vAssure(0x1048D888);
        call<void>(kMove, top, load<u32>(0x1048D888), u32(k) * 2, 0u);
        store<u16>(top + u32(k) * 2, 0);
    }
    vAssure(W);
    const s32 len = wcsLength(load<u32>(W));
    vAssure(W);
    return putCharsF(self, w, pos, load<u32>(W), len) == len ? len : 0;
}
VERIFY(0x025FC444, putPlayerName);

/* FixedSafeString<32> dst += string str (sead append as inlined) */
static void appendStr(u32 dst, u32 base, u32 str) {
    vAssure(dst);
    const s32 cur = cstrLength(load<u32>(dst));
    const s32 c0 = cur < 0 ? 0 : cur;
    vAssure(str);
    s32 n = cstrLength(load<u32>(str));
    const s32 avail = s32(load<u32>(dst + 8)) - c0;
    if (n >= avail) n = avail - 1;
    if (n <= 0) return;
    vAssure(str);
    call<void>(kMove, base + u32(c0), load<u32>(str), u32(n), 0u);
    if (c0 + n > cur) store<u8>(base + u32(c0 + n), 0);
}

/* 025FC708: puts a number with the given digit strings (padded to maxDigits unless noPad) */
s32 putNumber(u32 self, u32 w, s32 pos, s32 v, u32 digits, u32 maxDigits, u32 noPad) {
    WWHD_FUNC(0x025FC708, s32, self, w, pos, v, digits, maxDigits, noPad);
    Local<u8[0xA0]> F; /* +0 FixedSafeString<32>, +0x2C WFixedSafeString<32>, +0x78 digits[10] */
    const u32 S = F.a, Sbuf = F.a + 0xC, W = F.a + 0x2C, Wbuf = F.a + 0x38, D = F.a + 0x78;
    store<u16>(Wbuf + 0x3E, 0);
    store<u32>(W, Wbuf);
    store<u32>(S + 8, 0x20);
    store<u32>(W + 8, 0x20);
    store<u16>(Wbuf, 0);
    store<u32>(S, Sbuf);
    store<u8>(Sbuf + 0x1F, 0);
    store<u32>(W + 4, kWFixed32Vt);
    store<u32>(S + 4, 0x100E1020);
    store<u8>(Sbuf, 0);
    for (u32 i = 0; i < 10; i++) store<u32>(D + 4 * i, 0);
    u32 cnt = 0;
    if (maxDigits) {
        u32 left = maxDigits;
        for (;;) {
            const u32 slot = cnt < 10 ? D + 4 * cnt : D;
            const s32 d = v % 10;
            v = v / 10;
            cnt++;
            left = (left - 1) & 0xFF;
            store<u32>(slot, u32(d));
            if (!v || !left) break;
        }
    } else {
        for (;;) {
            const s32 d = v % 10;
            v = v / 10;
            store<u32>(cnt < 10 ? D + 4 * cnt : D, u32(d));
            cnt++;
            if (!v) break;
        }
        maxDigits = 5;
    }
    if (!noPad) {
        for (s32 pad = s32(maxDigits) - s32(cnt); pad > 0; pad--) appendStr(S, load<u32>(S), digits);
    }
    while (cnt) {
        cnt--;
        const u32 d = load<u32>(cnt < 10 ? D + 4 * cnt : D);
        appendStr(S, load<u32>(S), digits + d * 8);
    }
    vAssure(W);
    const u32 wtop = load<u32>(W);
    vAssure(S);
    const s32 n = s32(call<u32>(0x0275BAA0, wtop, load<u32>(W + 8), load<u32>(S), 0xFFFFFFFFu));
    vAssure(W);
    return putCharsF(self, w, pos, load<u32>(W), n) == n ? n : 0;
}
VERIFY(0x025FC708, putNumber);

/* 025FCB90: puts a number with ASCII digits */
s32 putNumberAsciiF(u32 self, u32 w, s32 pos, s32 v, u32 maxDigits, u32 noPad) {
    WWHD_FUNC(0x025FCB90, s32, self, w, pos, v, maxDigits, noPad);
    static const u32 lits[10] = {0x100E130C, 0x100E130E, 0x100E1310, 0x100E1312, 0x100E1314,
                                 0x100E1316, 0x100E1318, 0x100E131A, 0x100E131C, 0x100E131E};
    staticArr(0x1048DBF8, 0x1048DB34, lits, 10, kSafeVt, 0x101F4C1C);
    return putNumber(self, w, pos, v, 0x1048DB34, maxDigits, noPad);
}
VERIFY(0x025FCB90, putNumberAsciiF);

/* 025FCDE0: number of leading '0' characters */
s32 leadingZeros(u32 self, u32 s) {
    WWHD_FUNC(0x025FCDE0, s32, self, s);
    vAssure(s);
    u32 p = load<u32>(s);
    s32 n = 0;
    if (!p || !load<u16>(p)) return 0;
    while (load<u16>(p) == 0x30) {
        p += 2;
        n++;
        if (!load<u16>(p)) break;
    }
    return n;
}
VERIFY(0x025FCDE0, leadingZeros);

/* 025FCE5C: puts the "unitString" text of a label (leading '0' placeholders skipped; a lone space
 * puts nothing) */
s32 putUnitStringF(u32 self, u32 w, s32 pos, u32 label) {
    WWHD_FUNC(0x025FCE5C, s32, self, w, pos, label);
    const u32 set = call<u32>(0x025F4CFC, load<u32>(0x101F4AE8));
    if (!set) return 0;
    vAssure(label);
    const u32 li = call<u32>(0x0273A1CC, load<u32>(set), load<u32>(label));
    u32 txt = 0;
    if (li < load<u32>(set + 4)) txt = call<u32>(0x0273A2E8, load<u32>(set), li);
    Local<u32[2]> T;
    store<u32>(T.a, txt);
    store<u32>(T.a + 4, kWSafeVt);
    call<void>(kWNoAssure, T.a);
    {
        const u32 p = load<u32>(T.a);
        u32 q = p;
        s32 n = 0;
        bool exhausted = false;
        if (load<u16>(q)) {
            for (;;) {
                n++;
                if (n > 0x40000) {
                    exhausted = true;
                    break;
                }
                q += 2;
                if (!load<u16>(q)) break;
            }
        }
        (void)exhausted;
        if (load<u16>(p) == 0x20) {
            vAssure(T.a);
            u32 r = load<u32>(T.a);
            s32 k = 0;
            bool ex2 = false;
            if (load<u16>(r)) {
                for (;;) {
                    k++;
                    if (k > 0x40000) {
                        ex2 = true;
                        break;
                    }
                    r += 2;
                    if (!load<u16>(r)) break;
                }
            }
            if (!ex2 && k == 1) return 0;
        }
    }
    const s32 z = s32(call<u32>(0x025FCDE0, self, T.a));
    vAssure(T.a);
    const s32 len = wcsLength(load<u32>(T.a));
    vAssure(T.a);
    const s32 n = len - z;
    return putCharsF(self, w, pos, load<u32>(T.a) + u32(z) * 2, n) == n ? n : 0;
}
VERIFY(0x025FCE5C, putUnitStringF);

} // namespace hd_text_unit

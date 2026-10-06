/* hd_text_unit_items: HD message text builder, part 3: inserted numbers, units, timers and messages
 * (025FD05C..0260001B), WWHD. HD-only code (no GameCube
 * source): written from the WWHD code. See hd_text_unit.cpp. Unit/ruby labels are function-local
 * static SafeStrings ("Unit_*", "Ruby_*" message labels of the "unitString" set). */
#include "hd_text_unit.h"

namespace hd_text_unit {

/* 025FD05C: number of the event counter (event reg 0xBEFF) with its unit */
s32 putEventCount(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FD05C, s32, self, w, pos);
    const s32 n = putNumberAscii(self, w, pos, s32(eventReg(0xBEFF)), 0, 1);
    staticStr(0x1048DBFC, 0x1048D890, 0x100E1320, kSafeVt, 0x101F4C28);
    return n + putUnitString(self, w, pos + n, 0x1048D890);
}
VERIFY(0x025FD05C, putEventCount);

/* 025FD124: number + unit (singular for 1) */
s32 putBombCount(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FD124, s32, self, w, pos);
    const s32 v = s32(load<s16>(call<u32>(0x025200D4) + 0x5BA0));
    const s32 n = putNumberAscii(self, w, pos, v, 0, 1);
    staticPair(0x1048DC00, 0x1048D974, 0x100E1330, 0x100E1340, 0x101F4C34);
    return n + putUnitString(self, w, pos + n, 0x1048D974 + (v == 1 ? 0u : 8u));
}
VERIFY(0x025FD124, putBombCount);

/* 025FD22C: a tag inside an inserted message: handled by the tag processor, else copied as is;
 * returns the text after it (0 when the box is full) */
u32 putTag(u32 self, u32 w, u32 posp, u32 tag, u32 ctx) {
    WWHD_FUNC(0x025FD22C, u32, self, w, posp, tag, ctx);
    const u32 c = load<u16>(tag);
    u32 next, hdr;
    if (c == 0xE) {
        next = tag + load<u16>(tag + 6) + 8;
        hdr = tag;
    } else if (c == 0xF) {
        hdr = tag;
        next = tag + 6;
    } else {
        hdr = 0;
        next = tag;
    }
    s32 r = s32(call<u32>(0x026005F8, self, w, load<u32>(posp), hdr, tag, ctx));
    if (r < 0) {
        Local<u32[2]> T;
        store<u32>(T.a, tag);
        store<u32>(T.a + 4, kWSafeVt);
        r = s32(next - tag) >> 1;
        wcopyAt(w, s32(load<u32>(posp)), T.a, r);
    }
    const u32 p = load<u32>(posp) + u32(r);
    store<u32>(posp, p);
    if (call<u32>(0x025FBE54, self, w, p - 1, next)) return 0;
    return next;
}
VERIFY(0x025FD22C, putTag);

/* 025FD46C: puts a message text (characters and tags); returns the number of characters */
s32 putText(u32 self, u32 w, s32 pos, u32 text, u32 ctx) {
    WWHD_FUNC(0x025FD46C, s32, self, w, pos, text, ctx);
    Local<u32> P;
    store<u32>(P.a, u32(pos));
    if (!text) return 0;
    const s32 start = pos;
    u32 c = load<u16>(text);
    while (c) {
        if (c == 0xE || c == 0xF) {
            text = putTag(self, w, P.a, text, ctx);
            if (!text) break;
            c = load<u16>(text);
        } else {
            const s32 r = s32(call<u32>(0x025FC1F4, self, w, load<u32>(P.a), text));
            if (r < 0) break;
            store<u32>(P.a, load<u32>(P.a) + u32(r));
            text += 2;
            c = load<u16>(text);
        }
    }
    return s32(load<u32>(P.a)) - start;
}
VERIFY(0x025FD46C, putText);

/* 025FD550: puts the message with that number (id 0: the fallback text) */
s32 putMessageByIdF(u32 self, u32 w, s32 pos, u32 id, u32 ctx) {
    WWHD_FUNC(0x025FD550, s32, self, w, pos, id, ctx);
    s32 r = 0;
    if (!id) {
        staticStr(0x1048DC04, 0x1048D958, 0x100E135C, kWSafeVt, 0x101F4C40);
        vAssure(0x1048D958);
        const s32 n = wcsLength(load<u32>(0x1048D958));
        vAssure(0x1048D958);
        if (s32(call<u32>(0x025FC3D0, self, w, u32(pos), load<u32>(0x1048D958), u32(n))) == n) r = n;
        return r;
    }
    const u32 Q = 0x1048DA94;
    Local<u8[0x924]> M;
    if (!load<u32>(0x1048DC08)) {
        /* the static FixedSafeString<1> set name "" */
        const u32 buf = Q + 0xC;
        store<u8>(buf, 0);
        store<u32>(Q + 4, 0x100E1080);
        store<u32>(Q, buf);
        store<u32>(Q + 8, 1);
        Local<u32[2]> T;
        store<u32>(T.a + 4, kSafeVt);
        store<u32>(0x1048DC08, 1);
        store<u32>(T.a, 0x100E1350);
        call<void>(kNoAssure, T.a);
        s32 n = cstrLength(load<u32>(T.a));
        if (n >= s32(load<u32>(Q + 8))) n = s32(load<u32>(Q + 8)) - 1;
        vAssure(T.a);
        call<void>(kMove, buf, load<u32>(T.a), u32(n), 0u);
        store<u8>(buf + n, 0);
        store<u32>(Q + 4, 0x100E1098);
        call<void>(0x028F026C, 0x101F4C4Cu);
    }
    call<void>(0x025F65F4, M.a);
    Local<u8[0x110]> F;
    const u32 S = F.a, Sbuf = F.a + 0xC;
    store<u8>(Sbuf + 0xFF, 0);
    store<u8>(Sbuf, 0);
    store<u32>(S, Sbuf);
    store<u32>(S + 8, 0x100);
    store<u32>(S + 4, 0x100E0FF0);
    call<void>(0x02759C28, S, 0x100E1354u, id & 0xFFFF);
    call<void>(0x025F50E8, load<u32>(0x101F4AE8), M.a, Q, S);
    const u32 set = load<u32>(M.a + 8);
    if (!set) return 0;
    const u32 li = load<u32>(M.a + 0x118);
    if (li >= load<u32>(set + 4)) return 0;
    const u32 txt = call<u32>(0x0273A2E8, load<u32>(set), li);
    if (!txt) return 0;
    return putText(self, w, pos, txt, ctx);
}
VERIFY(0x025FD550, putMessageByIdF);

/* 025FD800 / 025FD86C: messages whose numbers are kept in the play info */
s32 putPlayMessage1(u32 self, u32 w, s32 pos, u32 ctx) {
    WWHD_FUNC(0x025FD800, s32, self, w, pos, ctx);
    const u32 id = load<u32>(call<u32>(0x025200D4) + 0x5B54);
    return putMessageByIdF(self, w, pos, id, ctx);
}
VERIFY(0x025FD800, putPlayMessage1);
s32 putPlayMessage2(u32 self, u32 w, s32 pos, u32 ctx) {
    WWHD_FUNC(0x025FD86C, s32, self, w, pos, ctx);
    const u32 id = load<u32>(call<u32>(0x025200D4) + 0x5B58);
    return putMessageByIdF(self, w, pos, id, ctx);
}
VERIFY(0x025FD86C, putPlayMessage2);

/* 025FD8D8: sword-game digits (full-width, the highlighted digit marked for the layout) */
s32 putSwordGameDigits(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FD8D8, s32, self, w, pos);
    staticStr(0x1048DC0C, 0x1048D898, 0x100E136C, kSafeVt, 0x101F4C58);
    store<u32>(msg(self) + 0x6B8, 3);
    store<u32>(msg(self), 0x15);
    const u32 LABEL = 0x1048D898;
    Local<u8[0x28]> F; /* +0 WFixedSafeString<4> (buffer +0xC), +0x14 digit chars[4], +0x1C WSafeString */
    const u32 W = F.a, Wbuf = F.a + 0xC, DG = F.a + 0x14, T = F.a + 0x1C;
    s32 total = 0;
    const u32 set = call<u32>(0x025F4CFC, load<u32>(0x101F4AE8));
    if (!set) return 0;
    vAssure(LABEL);
    const u32 li = call<u32>(0x0273A1CC, load<u32>(set), load<u32>(LABEL));
    u32 txt = 0;
    if (li < load<u32>(set + 4)) txt = call<u32>(0x0273A2E8, load<u32>(set), li);
    store<u32>(T, txt);
    store<u32>(T + 4, kWSafeVt);
    s32 digits = s32(call<u32>(0x025FCDE0, self, T));
    store<u16>(Wbuf + 6, 0);
    store<u32>(W, Wbuf);
    store<u32>(W + 8, 4);
    store<u16>(Wbuf, 0);
    store<u32>(W + 4, 0x100E1068);
    s32 v = s32(load<s16>(call<u32>(0x025200D4) + 0x5BA2));
    for (s32 k = 0; k < digits; k++) {
        const s32 d = v % 10;
        store<u16>(u32(k) < 4 ? DG + u32(k) * 2 : DG, u16(0xFF10 + d));
        v = v / 10;
    }
    const s32 sel = digits - s32(load<u32>(load<u32>(0x101F4B5C) + 0x948)) - 1;
    s32 pending = 0;
    s32 idx = digits - 1;
    for (s32 i = 0; digits > 0; digits--, i++, idx--) {
        if (u32(i) == u32(sel)) {
            if (pending > 0) {
                vAssure(W);
                putChars(self, w, pos, load<u32>(W), pending);
                pos += pending;
                total += pending;
                pending = 0;
                store<u16>(load<u32>(W), 0);
            }
            u32 m = msg(self);
            store<f32>(m + 0x8F0, load<f32>(m + 0x8EC));
            m = msg(self);
            store<u32>(m + 0x8F4, load<u32>(m + 0x644));
        }
        const u16 ch = load<u16>(u32(idx) < 4 ? DG + u32(idx) * 2 : DG);
        vAssure(W);
        const u32 top = load<u32>(W);
        const s32 len = wcsLength(top);
        if (len < s32(load<u32>(W + 8)) - 1) {
            store<u16>(top + u32(len) * 2, ch);
            store<u16>(top + u32(len) * 2 + 2, 0);
        }
        pending++;
        if (u32(i) == u32(sel)) {
            vAssure(W);
            putChars(self, w, pos, load<u32>(W), pending);
            pos += pending;
            total += pending;
            store<u16>(load<u32>(W), 0);
            pending = 0;
        }
    }
    if (pending > 0) {
        vAssure(W);
        putChars(self, w, pos, load<u32>(W), pending);
        total += pending;
        pos += pending;
    }
    return total + putUnitString(self, w, pos, LABEL);
}
VERIFY(0x025FD8D8, putSwordGameDigits);

/* 025FDD08: number + unit (singular for 1); the ruby label is not used */
s32 putNumberUnitF(u32 self, u32 w, s32 pos, s32 v, u32 ruby, u32 units) {
    WWHD_FUNC(0x025FDD08, s32, self, w, pos, v, ruby, units);
    const s32 n = putNumberAscii(self, w, pos, v, 0, 1);
    return n + putUnitString(self, w, pos + n, units + (v == 1 ? 0u : 8u));
}
VERIFY(0x025FDD08, putNumberUnitF);

/* 025FDD80: number + unit with ruby */
s32 putBlows(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FDD80, s32, self, w, pos);
    staticStr(0x1048DC10, 0x1048D8A0, 0x100E137C, kSafeVt, 0x101F4C64);
    staticPair(0x1048DC14, 0x1048D984, 0x100E1394, 0x100E13A4, 0x101F4C70);
    return putNumberUnit(self, w, pos, s32(load<s16>(call<u32>(0x025200D4) + 0x5BA0)), 0x1048D8A0, 0x1048D984);
}
VERIFY(0x025FDD80, putBlows);

/* 025FDEAC: message selected by event reg 0xBA0F (+0x1B37) */
s32 putEventMessage(u32 self, u32 w, s32 pos, u32 ctx) {
    WWHD_FUNC(0x025FDEAC, s32, self, w, pos, ctx);
    return putMessageByIdF(self, w, pos, eventReg(0xBA0F) + 0x1B37, ctx);
}
VERIFY(0x025FDEAC, putEventMessage);

/* 025FDF28 / 025FDF8C / 025FE000: plain numbers */
s32 putPlayCount2(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FDF28, s32, self, w, pos);
    return putNumberAscii(self, w, pos, s32(load<s16>(call<u32>(0x025200D4) + 0x5CEC)), 0, 1);
}
VERIFY(0x025FDF28, putPlayCount2);
s32 putEvent8AFF(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FDF8C, s32, self, w, pos);
    return putNumberAscii(self, w, pos, s32(eventReg(0x8AFF)), 0, 1);
}
VERIFY(0x025FDF8C, putEvent8AFF);
s32 putPlayCount(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FE000, s32, self, w, pos);
    return putNumberAscii(self, w, pos, s32(load<s16>(call<u32>(0x025200D4) + 0x5BA0)), 0, 1);
}
VERIFY(0x025FE000, putPlayCount);

/* 025FE064: ten times the play counter with its unit */
s32 putPlayCountX10(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FE064, s32, self, w, pos);
    staticPair(0x1048DC18, 0x1048D994, 0x100E13B4, 0x100E13C4, 0x101F4C7C);
    return putNumberUnit(self, w, pos, s32(load<s16>(call<u32>(0x025200D4) + 0x5BA0)) * 10, 0, 0x1048D994);
}
VERIFY(0x025FE064, putPlayCountX10);

/* 025FE158: number + unit with ruby */
s32 putLetters(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FE158, s32, self, w, pos);
    staticStr(0x1048DC1C, 0x1048D8A8, 0x100E13D4, kSafeVt, 0x101F4C88);
    staticPair(0x1048DC20, 0x1048D9A4, 0x100E13E4, 0x100E13F4, 0x101F4C94);
    return putNumberUnit(self, w, pos, s32(load<s16>(call<u32>(0x025200D4) + 0x5BA0)), 0x1048D8A8, 0x1048D9A4);
}
VERIFY(0x025FE158, putLetters);

/* 025FE284: number + unit (singular for 1) */
s32 putArrowCount(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FE284, s32, self, w, pos);
    const s32 v = s32(load<s16>(call<u32>(0x025200D4) + 0x5BA0));
    const s32 n = putNumberAscii(self, w, pos, v, 0, 1);
    staticPair(0x1048DC24, 0x1048D9B4, 0x100E1404, 0x100E1414, 0x101F4CA0);
    return n + putUnitString(self, w, pos + n, 0x1048D9B4 + (v == 1 ? 0u : 8u));
}
VERIFY(0x025FE284, putArrowCount);

/* 025FE38C: a time (frames at 30 per second) as minutes and seconds, per language */
s32 putTimeF(u32 self, u32 w, s32 pos, s32 frames) {
    WWHD_FUNC(0x025FE38C, s32, self, w, pos, frames);
    const s32 minutes = frames / 1800;
    const s32 rem = frames - minutes * 1800;
    s32 secs = s32(u32(rem) * 0x4445) / 0x80000;
    if (!(minutes | secs)) secs = 1;
    s32 total = 0;
    const u32 cfg = 0x101F4BAC;
    u32 lang = load<u32>(load<u32>(cfg) + 0x14);
    if (lang == 1 || lang == 3) {
        total = putNumberAscii(self, w, pos, minutes, 0, 1);
        pos += total;
    } else if (minutes > 0) {
        const s32 n = putNumberAscii(self, w, pos, minutes, 0, 1);
        pos += n;
        total = n;
    }
    staticPair(0x1048DC28, 0x1048DAA4, 0x100E1430, 0x100E1440, 0x101F4CAC);
    lang = load<u32>(load<u32>(cfg) + 0x14);
    if (lang == 1 || lang == 3) {
        const s32 r = putUnitString(self, w, pos, 0x1048DAA4);
        pos += r;
        total += r;
        lang = load<u32>(load<u32>(cfg) + 0x14);
    } else if (minutes > 0) {
        const s32 r = putUnitString(self, w, pos, 0x1048DAA4 + (minutes > 1 ? 8u : 0u));
        pos += r;
        lang = load<u32>(load<u32>(cfg) + 0x14);
        total += r;
    }
    if (lang == 1 || lang == 3) {
        const s32 n = putNumberAscii(self, w, pos, secs, 2, 0);
        total += n;
        pos += n;
    } else if (!minutes) {
        const s32 n = putNumberAscii(self, w, pos, secs, 2, 0);
        total += n;
        pos += n;
    } else if (secs) {
        if (!load<u32>(0x1048DC2C)) {
            const u32 lits[2] = {0x100E142C, 0x100E1424};
            staticArr(0x1048DC2C, 0x1048DAB4, lits, 2, kWSafeVt, 0x101F4CB8);
            lang = load<u32>(load<u32>(cfg) + 0x14);
        }
        const u32 sep = 0x1048DAB4 + (lang == 5 ? 8u : 0u);
        vAssure(sep);
        const s32 len = wcsLength(load<u32>(sep));
        vAssure(sep);
        if (putChars(self, w, pos, load<u32>(sep), len) != len) return total;
        total += len;
        pos += len;
        const s32 n = putNumberAscii(self, w, pos, secs, 2, 0);
        total += n;
        pos += n;
    }
    staticPair(0x1048DC30, 0x1048DAC4, 0x100E1450, 0x100E1460, 0x101F4CC4);
    lang = load<u32>(load<u32>(cfg) + 0x14);
    if (lang == 1) return total;
    if (lang == 3) return total + putUnitString(self, w, pos, 0x1048DAC4);
    if (secs > 0) total += putUnitString(self, w, pos, 0x1048DAC4 + (secs > 1 ? 8u : 0u));
    return total;
}
VERIFY(0x025FE38C, putTimeF);

/* 025FE7F4 / 025FECF4: timers */
s32 putSaveTimer(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FE7F4, s32, self, w, pos);
    return putTimeF(self, w, pos, s32(call<u32>(0x025B61C8, saveInfo() + 0x86)));
}
VERIFY(0x025FE7F4, putSaveTimer);
s32 putPlayTimer(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FECF4, s32, self, w, pos);
    return putTimeF(self, w, pos, s32(load<s16>(call<u32>(0x025200D4) + 0x5BAA)));
}
VERIFY(0x025FECF4, putPlayTimer);

/* 025FE858: number + unit (singular for 1) */
s32 putCountE858(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FE858, s32, self, w, pos);
    const s32 v = s32(load<s16>(call<u32>(0x025200D4) + 0x5BA0));
    const s32 n = putNumberAscii(self, w, pos, v, 0, 1);
    staticPair(0x1048DC34, 0x1048D9C4, 0x100E1470, 0x100E1480, 0x101F4CD0);
    return n + putUnitString(self, w, pos + n, 0x1048D9C4 + (v == 1 ? 0u : 8u));
}
VERIFY(0x025FE858, putCountE858);

/* 025FE960: number + unit (singular for 1) */
s32 putEvent86FF(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FE960, s32, self, w, pos);
    const s32 v = s32(eventReg(0x86FF));
    const s32 n = putNumberAscii(self, w, pos, v, 0, 1);
    staticPair(0x1048DC38, 0x1048D9D4, 0x100E1490, 0x100E14A0, 0x101F4CDC);
    return n + putUnitString(self, w, pos + n, 0x1048D9D4 + (v == 1 ? 0u : 8u));
}
VERIFY(0x025FE960, putEvent86FF);

/* 025FEA7C: number + unit with ruby */
s32 putPendantsC3(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FEA7C, s32, self, w, pos);
    staticStr(0x1048DC3C, 0x1048D8B0, 0x100E14B0, kSafeVt, 0x101F4CE8);
    staticPair(0x1048DC40, 0x1048D9E4, 0x100E14C0, 0x100E14D0, 0x101F4CF4);
    return putNumberUnit(self, w, pos, s32(load<u8>(saveInfo() + 0xC3)), 0x1048D8B0, 0x1048D9E4);
}
VERIFY(0x025FEA7C, putPendantsC3);

/* 025FEBA8: number + unit with ruby */
s32 putPendantsEvent(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FEBA8, s32, self, w, pos);
    staticStr(0x1048DC44, 0x1048D8B8, 0x100E14E0, kSafeVt, 0x101F4D00);
    staticPair(0x1048DC48, 0x1048D9F4, 0x100E14F0, 0x100E1500, 0x101F4D0C);
    return putNumberUnit(self, w, pos, s32(eventReg(0xC0FF)), 0x1048D8B8, 0x1048D9F4);
}
VERIFY(0x025FEBA8, putPendantsEvent);

/* 025FED50: number + unit with ruby */
s32 putBombs90(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FED50, s32, self, w, pos);
    staticStr(0x1048DC4C, 0x1048D8C0, 0x100E1510, kSafeVt, 0x101F4D18);
    staticPair(0x1048DC50, 0x1048DA04, 0x100E1520, 0x100E1530, 0x101F4D24);
    return putNumberUnit(self, w, pos, s32(load<u8>(saveInfo() + 0x90)), 0x1048D8C0, 0x1048DA04);
}
VERIFY(0x025FED50, putBombs90);

/* 025FEE7C: number + unit (singular for 1) */
s32 putSave8F(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FEE7C, s32, self, w, pos);
    const s32 v = s32(load<u8>(saveInfo() + 0x8F));
    const s32 n = putNumberAscii(self, w, pos, v, 0, 1);
    staticPair(0x1048DC54, 0x1048DA14, 0x100E1540, 0x100E1550, 0x101F4D30);
    return n + putUnitString(self, w, pos + n, 0x1048DA14 + (v == 1 ? 0u : 8u));
}
VERIFY(0x025FEE7C, putSave8F);

/* 025FEF7C: number + unit with ruby */
s32 putSeeds(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FEF7C, s32, self, w, pos);
    staticStr(0x1048DC58, 0x1048D8C8, 0x100E1560, kSafeVt, 0x101F4D3C);
    staticPair(0x1048DC5C, 0x1048DA24, 0x100E1570, 0x100E1580, 0x101F4D48);
    return putNumberUnit(self, w, pos, s32(load<u8>(0x101D5F48)), 0x1048D8C8, 0x1048DA24);
}
VERIFY(0x025FEF7C, putSeeds);

/* 025FF0A0: number + unit with ruby */
s32 putNecklaces(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FF0A0, s32, self, w, pos);
    staticStr(0x1048DC60, 0x1048D8D0, 0x100E1590, kSafeVt, 0x101F4D54);
    staticPair(0x1048DC64, 0x1048DA34, 0x100E15A4, 0x100E15B8, 0x101F4D60);
    return putNumberUnit(self, w, pos, s32(load<u8>(0x101D5F48)), 0x1048D8D0, 0x1048DA34);
}
VERIFY(0x025FF0A0, putNecklaces);

/* 025FF1C4: number + unit with ruby */
s32 putChuchus(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FF1C4, s32, self, w, pos);
    staticStr(0x1048DC68, 0x1048D8D8, 0x100E15CC, kSafeVt, 0x101F4D6C);
    staticPair(0x1048DC6C, 0x1048DA44, 0x100E15DC, 0x100E15EC, 0x101F4D78);
    return putNumberUnit(self, w, pos, s32(load<u8>(0x101D5F48)), 0x1048D8D8, 0x1048DA44);
}
VERIFY(0x025FF1C4, putChuchus);

/* 025FF2E8: number + unit with ruby */
s32 putPendants(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FF2E8, s32, self, w, pos);
    staticStr(0x1048DC70, 0x1048D8E0, 0x100E15FC, kSafeVt, 0x101F4D84);
    staticPair(0x1048DC74, 0x1048DA54, 0x100E160C, 0x100E161C, 0x101F4D90);
    return putNumberUnit(self, w, pos, s32(load<u8>(0x101D5F48)), 0x1048D8E0, 0x1048DA54);
}
VERIFY(0x025FF2E8, putPendants);

/* 025FF40C: number + unit with ruby */
s32 putFeathers(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FF40C, s32, self, w, pos);
    staticStr(0x1048DC78, 0x1048D8E8, 0x100E162C, kSafeVt, 0x101F4D9C);
    staticPair(0x1048DC7C, 0x1048DA64, 0x100E163C, 0x100E164C, 0x101F4DA8);
    return putNumberUnit(self, w, pos, s32(load<u8>(0x101D5F48)), 0x1048D8E8, 0x1048DA64);
}
VERIFY(0x025FF40C, putFeathers);

/* 025FF530: number + unit with ruby */
s32 putCrests(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FF530, s32, self, w, pos);
    staticStr(0x1048DC80, 0x1048D8F0, 0x100E165C, kSafeVt, 0x101F4DB4);
    staticPair(0x1048DC84, 0x1048DA74, 0x100E166C, 0x100E167C, 0x101F4DC0);
    return putNumberUnit(self, w, pos, s32(load<u8>(0x101D5F48)), 0x1048D8F0, 0x1048DA74);
}
VERIFY(0x025FF530, putCrests);

/* 025FF654: number + unit for the counter at 101D5F3A (no ruby) */
s32 putCounter5F3A(u32 self, u32 w, s32 pos) {
    WWHD_FUNC(0x025FF654, s32, self, w, pos);
    staticPair(0x1048DC88, 0x1048DA84, 0x100E168C, 0x100E169C, 0x101F4DCC);
    return putNumberUnit(self, w, pos, s32(load<s16>(0x101D5F3A)), 0, 0x1048DA84);
}
VERIFY(0x025FF654, putCounter5F3A);

/* 025FF738: digits of the play counter with the unit label's text (as 025FD8D8, other labels) */
s32 putDigitsLabel(u32 self, u32 w, s32 pos, u32 ruby, u32 unit, u32 ctx) {
    WWHD_FUNC(0x025FF738, s32, self, w, pos, ruby, unit, ctx);
    store<u32>(msg(self) + 0x6B8, 3);
    store<u32>(msg(self), 0x15);
    const u32 LABEL = unit;
    Local<u8[0x28]> F; /* +0 WFixedSafeString<4> (buffer +0xC), +0x14 digit chars[4], +0x1C WSafeString */
    const u32 W = F.a, Wbuf = F.a + 0xC, DG = F.a + 0x14, T = F.a + 0x1C;
    s32 total = 0;
    const u32 set = call<u32>(0x025F4CFC, load<u32>(0x101F4AE8));
    if (!set) return 0;
    vAssure(LABEL);
    const u32 li = call<u32>(0x0273A1CC, load<u32>(set), load<u32>(LABEL));
    u32 txt = 0;
    if (li < load<u32>(set + 4)) txt = call<u32>(0x0273A2E8, load<u32>(set), li);
    store<u32>(T, txt);
    store<u32>(T + 4, kWSafeVt);
    s32 digits = s32(call<u32>(0x025FCDE0, self, T));
    store<u16>(Wbuf + 6, 0);
    store<u32>(W, Wbuf);
    store<u32>(W + 8, 4);
    store<u16>(Wbuf, 0);
    store<u32>(W + 4, 0x100E1068);
    s32 v = s32(load<s16>(call<u32>(0x025200D4) + 0x5BA2));
    for (s32 k = 0; k < digits; k++) {
        const s32 d = v % 10;
        store<u16>(u32(k) < 4 ? DG + u32(k) * 2 : DG, u16(0xFF10 + d));
        v = v / 10;
    }
    const s32 sel = digits - s32(load<u32>(load<u32>(0x101F4B5C) + 0x948)) - 1;
    s32 pending = 0;
    s32 idx = digits - 1;
    for (s32 i = 0; digits > 0; digits--, i++, idx--) {
        if (u32(i) == u32(sel)) {
            if (pending > 0) {
                vAssure(W);
                putChars(self, w, pos, load<u32>(W), pending);
                pos += pending;
                total += pending;
                pending = 0;
                store<u16>(load<u32>(W), 0);
            }
            u32 m = msg(self);
            store<f32>(m + 0x8F0, load<f32>(m + 0x8EC));
            m = msg(self);
            store<u32>(m + 0x8F4, load<u32>(m + 0x644));
        }
        const u16 ch = load<u16>(u32(idx) < 4 ? DG + u32(idx) * 2 : DG);
        vAssure(W);
        const u32 top = load<u32>(W);
        const s32 len = wcsLength(top);
        if (len < s32(load<u32>(W + 8)) - 1) {
            store<u16>(top + u32(len) * 2, ch);
            store<u16>(top + u32(len) * 2 + 2, 0);
        }
        pending++;
        if (u32(i) == u32(sel)) {
            vAssure(W);
            putChars(self, w, pos, load<u32>(W), pending);
            pos += pending;
            total += pending;
            store<u16>(load<u32>(W), 0);
            pending = 0;
        }
    }
    if (pending > 0) {
        vAssure(W);
        putChars(self, w, pos, load<u32>(W), pending);
        total += pending;
        pos += pending;
    }
    return total + putUnitString(self, w, pos, LABEL);
}
VERIFY(0x025FF738, putDigitsLabel);

/* 025FFB24: digit display with ruby and unit labels */
s32 putSeedDigits(u32 self, u32 w, s32 pos, u32 ctx) {
    WWHD_FUNC(0x025FFB24, s32, self, w, pos, ctx);
    staticStr(0x1048DC8C, 0x1048D8F8, 0x100E16AC, kSafeVt, 0x101F4DD8);
    staticStr(0x1048DC90, 0x1048D900, 0x100E16BC, kSafeVt, 0x101F4DE4);
    return s32(call<u32>(0x025FF738, self, w, u32(pos), 0x1048D8F8u, 0x1048D900u, ctx));
}
VERIFY(0x025FFB24, putSeedDigits);

/* 025FFBF8: digit display with ruby and unit labels */
s32 putNecklaceDigits(u32 self, u32 w, s32 pos, u32 ctx) {
    WWHD_FUNC(0x025FFBF8, s32, self, w, pos, ctx);
    staticStr(0x1048DC94, 0x1048D908, 0x100E16CC, kSafeVt, 0x101F4DF0);
    staticStr(0x1048DC98, 0x1048D910, 0x100E16E0, kSafeVt, 0x101F4DFC);
    return s32(call<u32>(0x025FF738, self, w, u32(pos), 0x1048D908u, 0x1048D910u, ctx));
}
VERIFY(0x025FFBF8, putNecklaceDigits);

/* 025FFCCC: digit display with ruby and unit labels */
s32 putChuchuDigits(u32 self, u32 w, s32 pos, u32 ctx) {
    WWHD_FUNC(0x025FFCCC, s32, self, w, pos, ctx);
    staticStr(0x1048DC9C, 0x1048D918, 0x100E16F4, kSafeVt, 0x101F4E08);
    staticStr(0x1048DCA0, 0x1048D920, 0x100E1704, kSafeVt, 0x101F4E14);
    return s32(call<u32>(0x025FF738, self, w, u32(pos), 0x1048D918u, 0x1048D920u, ctx));
}
VERIFY(0x025FFCCC, putChuchuDigits);

/* 025FFDA0: digit display with ruby and unit labels */
s32 putPendantDigits(u32 self, u32 w, s32 pos, u32 ctx) {
    WWHD_FUNC(0x025FFDA0, s32, self, w, pos, ctx);
    staticStr(0x1048DCA4, 0x1048D928, 0x100E1714, kSafeVt, 0x101F4E20);
    staticStr(0x1048DCA8, 0x1048D930, 0x100E1724, kSafeVt, 0x101F4E2C);
    return s32(call<u32>(0x025FF738, self, w, u32(pos), 0x1048D928u, 0x1048D930u, ctx));
}
VERIFY(0x025FFDA0, putPendantDigits);

/* 025FFE74: digit display with ruby and unit labels */
s32 putFeatherDigits(u32 self, u32 w, s32 pos, u32 ctx) {
    WWHD_FUNC(0x025FFE74, s32, self, w, pos, ctx);
    staticStr(0x1048DCAC, 0x1048D938, 0x100E1734, kSafeVt, 0x101F4E38);
    staticStr(0x1048DCB0, 0x1048D940, 0x100E1744, kSafeVt, 0x101F4E44);
    return s32(call<u32>(0x025FF738, self, w, u32(pos), 0x1048D938u, 0x1048D940u, ctx));
}
VERIFY(0x025FFE74, putFeatherDigits);

/* 025FFF48: digit display with ruby and unit labels */
s32 putCrestDigits(u32 self, u32 w, s32 pos, u32 ctx) {
    WWHD_FUNC(0x025FFF48, s32, self, w, pos, ctx);
    staticStr(0x1048DCB4, 0x1048D948, 0x100E1754, kSafeVt, 0x101F4E50);
    staticStr(0x1048DCB8, 0x1048D950, 0x100E1764, kSafeVt, 0x101F4E5C);
    return s32(call<u32>(0x025FF738, self, w, u32(pos), 0x1048D948u, 0x1048D950u, ctx));
}
VERIFY(0x025FFF48, putCrestDigits);

} // namespace hd_text_unit

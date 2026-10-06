/* hd_text_writer: HD message tag processor object and text splitting helpers, WWHD.
 * HD-only code (no GameCube source): written from the
 * WWHD code. Range 026025CC..02602A83; static initialiser 026029F0.
 *
 * TagProcessor (0x2C): nw4f TagProcessorBase (ctor 0286F9F0, Process 0286FBDC, CalcRect 0286FC44),
 * +0 vtable 100E1820, +4 message unit state (025FA8FC ctor, 026017F0 setup), +8 ruby/font tag
 * state (hd_text_ruby, 0x10), +0x18 unit tag state (025FA0F0 ctor, 025FA4CC CalcRect, 025FA50C
 * reset), +0x28 u32. Message text: u16 characters; 0x0E starts a tag {u16 0xE, u16 group,
 * u16 tag, u16 size, params[size]}, 0x0F a 6-byte end tag, 0x0A a new line.
 */
#include "gabi.h"
using namespace gabi;

namespace hd_text_writer {

/* 026025CC: TagProcessor constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x026025CC, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x2Cu);
        if (!p) return 0;
    }
    call<void>(0x0286F9F0, p);
    store<u32>(p, 0x100E1820);
    call<void>(0x025FA8FC, p + 4);
    call<void>(0x02601B60, p + 8);
    call<void>(0x025FA0F0, p + 0x18);
    store<u32>(p + 0x28, 0);
    return p;
}
VERIFY(0x026025CC, ctor);

/* the tag at ctx->str - 2 (0x0E begin tag or 0x0F end tag); 0 when it is no tag */
static u32 tagAt(u32 ctxStr, u32& next) {
    const u32 t = ctxStr - 2;
    const u32 c = load<u16>(t);
    if (c == 0xE) {
        next = t + load<u16>(t + 6) + 8;
        return t;
    }
    if (c == 0xF) {
        next = t + 6;
        return t;
    }
    return 0;
}

/* 02602640: TagProcessor::Process(code, context) */
u32 process(u32 self, u32 code, u32 ctx) {
    WWHD_FUNC(0x02602640, u32, self, code, ctx);
    u32 next = 0;
    const u32 t = tagAt(load<u32>(ctx + 4), next);
    if (!t) return call<u32>(0x0286FBDC, self, code, ctx);
    store<u32>(ctx + 4, next);
    /* r8 is not an argument: the colour tags assemble r8 with rlwimi (every byte replaced), which the
     * live-in analysis reports as a read; keep r8 as the original leaves it (tag + size for 0x0E) */
    if (load<u16>(t) == 0xE) cpu->r[8] = next - 8;
    return call<u32>(0x026024C8, self + 8, ctx, t, next);
}
VERIFY(0x02602640, process);

/* 02602694: TagProcessor::CalcRect(rect, code, context) */
u32 calcRect(u32 self, u32 rect, u32 code, u32 ctx) {
    WWHD_FUNC(0x02602694, u32, self, rect, code, ctx);
    u32 next = 0;
    const u32 t = tagAt(load<u32>(ctx + 4), next);
    if (!t) return call<u32>(0x0286FC44, self, rect, code, ctx);
    store<u32>(ctx + 4, next);
    if (load<u16>(t) == 0xE) cpu->r[9] = next - 8; /* as above (r9) */
    return call<u32>(0x025FA4CC, self + 0x18, rect, ctx, t, next);
}
VERIFY(0x02602694, calcRect);

/* 026026E8: resets both tag states */
void reset(u32 self) {
    WWHD_FUNC(0x026026E8, void, self);
    call<void>(0x026024F8, self + 8);
    call<void>(0x025FA50C, self + 0x18);
}
VERIFY(0x026026E8, reset);

/* 02602720: begins a message */
u32 begin(u32 self, u32 a, u32 b, u32 c, u32 d) {
    WWHD_FUNC(0x02602720, u32, self, a, b, c, d);
    store<u32>(self + 0x28, c);
    reset(self);
    return call<u32>(0x026017F0, self + 4, a, b, c, d);
}
VERIFY(0x02602720, begin);

/* 02602778: counts the printable characters and the lines of a text */
void countText(u32 self, u32 chars, u32 lines, u32 text, s32 bytes) {
    WWHD_FUNC(0x02602778, void, self, chars, lines, text, bytes);
    const s32 n = bytes / 2;
    u32 p = text;
    s32 nl = 1, nc = 0;
    while (s32(p - text) >> 1 < n) {
        const u32 c = load<u16>(p);
        if (c == 0xE || c == 0xF) {
            const u32 t = p;
            p = c == 0xE ? p + load<u16>(p + 6) + 8 : p + 6;
            if (t) continue;
        }
        if (load<u16>(p) == 0xA) {
            p += 2;
            nl++;
            continue;
        }
        p += 2;
        nc++;
    }
    if (chars) store<u32>(chars, u32(nc));
    if (lines) store<u32>(lines, u32(nl));
}
VERIFY(0x02602778, countText);

/* 02602834: distributes a text over a list of text panes, one character (or tag) per pane */
u32 splitText(u32 self, u32 list, u32 text, s32 bytes) {
    WWHD_FUNC(0x02602834, u32, self, list, text, bytes);
    u32 count = 0;
    u32 it = load<u32>(list + 8);
    const u32 end = it + 4 * load<u32>(list);
    u32 p = text;
    const s32 n = bytes / 2;
    if (it == end) return 0;
    while (s32(p - text) >> 1 < n) {
        const u32 c = load<u16>(p);
        u32 t = 0;
        bool isTag = false;
        if (c == 0xE || c == 0xF) {
            t = p;
            p = c == 0xE ? p + load<u16>(p + 6) + 8 : p + 6;
            isTag = t != 0;
        }
        u32 len;
        if (isTag) {
            const u32 g = load<u16>(t + 2);
            if ((g >= 1 && g <= 2) || (g >= 4 && g <= 6)) {
                if (it == end) return count;
                continue;
            }
            const u32 pane = load<u32>(it);
            len = u32(s32(load<u16>(t + 6) + 8) / 2);
            call_ptr<void>(load<u32>(load<u32>(pane + 8) + 0xCC), pane, t, 0u, len & 0xFFFF);
            if (s32(len) <= 0) {
                if (it == end) return count;
                continue;
            }
        } else {
            if (load<u16>(p) == 0xA) {
                p += 2;
                if (it == end) return count;
                continue;
            }
            const u32 pane = load<u32>(it);
            call_ptr<void>(load<u32>(load<u32>(pane + 8) + 0xCC), pane, p, 0u, 1u);
            p += 2;
            len = 1;
        }
        const u32 pane = load<u32>(it);
        call_ptr<void>(load<u32>(load<u32>(pane + 8) + 0xCC), pane, 0x10000162u, len & 0xFFFF, 1u);
        const u32 q = load<u32>(it);
        store<u8>(q + 0x44, u8((load<u8>(q + 0x44) & 0xFE) + 1));
        it += 4;
        count++;
        if (it == end) return count;
    }
    do {
        const u32 q = load<u32>(it);
        it += 4;
        store<u8>(q + 0x44, load<u8>(q + 0x44) & 0xFE);
    } while (it != end);
    return count;
}
VERIFY(0x02602834, splitText);

/* 026029F0: static initialiser */
void staticInit() {
    WWHD_FUNC(0x026029F0, void);
    const u32 b = 0x1048DCE8;
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, 0x101F4EBCu);
    const f32 lo = load<f32>(0x100E1818), hi = load<f32>(0x100E181C);
    store<f32>(0x1048DCDC, lo);
    store<f32>(0x1048DCE0, hi);
    call<void>(0x028ED6F8, 0x1048DCE4u);
    call<void>(0x028F026C, 0x101F4EC8u);
    call<void>(0x028EAB2C, 0x1048DCE5u);
    call<void>(0x028F026C, 0x101F4ED4u);
}
VERIFY(0x026029F0, staticInit);

/* 02602A84: TagProcessor::GetRuntimeTypeInfo (vtable 100E1820 +0xC): nw4f runtime type info of the
 * class, a function-local static initialised on first use (the base's info is a second static) */
u32 runtimeTypeInfo() {
    WWHD_FUNC(0x02602A84, u32);
    if (load<u32>(0x101FDD54)) return 0x101FDD58;
    const u32 baseGuard = load<u32>(0x101FDD5C);
    store<u32>(0x101FDD54, 1);
    if (!baseGuard) {
        store<u32>(0x101FDD5C, 1);
        store<u32>(0x101FDD60, 0);
    }
    store<u32>(0x101FDD58, 0x101FDD60);
    return 0x101FDD58;
}
VERIFY(0x02602A84, runtimeTypeInfo);

} // namespace hd_text_writer

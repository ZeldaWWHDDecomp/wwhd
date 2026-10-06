/* hd_text_unit.h: shared inline helpers of the hd_text_unit sources (HD message text builder), WWHD.
 * sead string operations as GHS inlines them. */
#pragma once
#include "gabi.h"

namespace hd_text_unit {
using namespace gabi;

/* vtables of the TU's sead string instances and their devirtualised assureTermination members */
static const u32 kSafeVt = 0x100E0F90;    /* SafeString */
static const u32 kWSafeVt = 0x100E0FC0;   /* WSafeString */
static const u32 kWFixed32Vt = 0x100E1050; /* WFixedSafeString<32> */
static const u32 kWNoAssure = 0x02601ADC; /* WSafeString::assureTerminationImpl_ (empty) */
static const u32 kNoAssure = 0x02601AAC;  /* SafeString::assureTerminationImpl_ (empty) */
static const u32 kWFixedAssure = 0x02601AE0; /* WBufferedSafeString::assureTerminationImpl_ */
static const u32 kMove = 0xC0009988;      /* OSBlockMove */

/* the message text object of the builder: *self */
static inline u32 msg(u32 self) { return load<u32>(self); }

static inline void vAssure(u32 s) { call_ptr<void>(load<u32>(load<u32>(s + 4) + 0x14), s); }

/* strlen / wcslen capped at 0x40000 as sead does (0 when exhausted) */
static inline s32 cstrLength(u32 p) {
    s32 n = 0;
    if (!load<u8>(p)) return 0;
    for (;;) {
        n++;
        if (n > 0x40000) return 0;
        p++;
        if (!load<u8>(p)) return n;
    }
}
static inline s32 wcsLength(u32 p) {
    s32 n = 0;
    if (!load<u16>(p)) return 0;
    for (;;) {
        n++;
        if (n > 0x40000) return 0;
        p += 2;
        if (!load<u16>(p)) return n;
    }
}

/* sead WBufferedSafeString::copyAt(pos, src, cnt) as inlined (cnt < 0: whole src) */
static inline void wcopyAt(u32 dst, s32 pos, u32 src, s32 cnt) {
    const u32 base = load<u32>(dst);
    vAssure(dst);
    const s32 len = wcsLength(load<u32>(dst));
    if (pos < 0) {
        pos = len + pos + 1;
        if (pos < 0) pos = 0;
    }
    if (cnt < 0) {
        vAssure(src);
        cnt = wcsLength(load<u32>(src));
    }
    const s32 avail = s32(load<u32>(dst + 8)) - pos;
    if (cnt >= avail) cnt = avail - 1;
    if (cnt <= 0) return;
    vAssure(src);
    call<void>(kMove, base + u32(pos) * 2, load<u32>(src), u32(cnt) * 2, 0u);
    if (pos + cnt > len) store<u16>(base + u32(pos + cnt) * 2, 0);
}

/* a stack WSafeString {top, WSafeString vtable} */
static inline void wtmp(u32 t, u32 top) {
    store<u32>(t, top);
    store<u32>(t + 4, kWSafeVt);
}

/* a stack WFixedSafeString<N> (header at s, buffer at s + 0xC), assured */
static inline void wfixed(u32 s, u32 vt, u32 n) {
    const u32 buf = s + 0xC;
    store<u16>(buf + 2 * n - 2, 0);
    store<u32>(s + 8, n);
    store<u32>(s, buf);
    store<u16>(buf, 0);
    store<u32>(s + 4, vt);
    call<void>(kWFixedAssure, s);
}

/* function-local static SafeString / WSafeString */
static inline void staticStr(u32 guard, u32 obj, u32 lit, u32 vt, u32 rec) {
    if (load<u32>(guard)) return;
    store<u32>(guard, 1);
    store<u32>(obj + 4, vt);
    store<u32>(obj, lit);
    call<void>(0x028F026C, rec);
}
/* function-local static string array (placement-new null checks after the first element) */
static inline void staticArr(u32 guard, u32 obj, const u32* lits, u32 n, u32 vt, u32 rec) {
    if (load<u32>(guard)) return;
    store<u32>(guard, 1);
    store<u32>(obj + 4, vt);
    store<u32>(obj, lits[0]);
    for (u32 i = 1; i < n; i++) {
        u32 q = obj + 8 * i;
        if (!q) q = call<u32>(0x0273AD10, 8u);
        if (q) {
            store<u32>(q + 4, vt);
            store<u32>(q, lits[i]);
        }
    }
    call<void>(0x028F026C, rec);
}
static inline void staticPair(u32 guard, u32 obj, u32 s0, u32 s1, u32 rec) {
    const u32 lits[2] = {s0, s1};
    staticArr(guard, obj, lits, 2, kSafeVt, rec);
}

/* save info (101F84DC) and play info (025200D4) */
static inline u32 saveInfo() { return load<u32>(0x101F84DC); }
static inline u32 eventReg(u32 reg) { return call<u32>(0x025B8BB0, saveInfo() + 0x644, reg); }

/* the builder's functions in the other source files (called through their addresses) */
static inline s32 putChars(u32 self, u32 w, s32 pos, u32 chars, s32 n) { return s32(call<u32>(0x025FC3D0, self, w, u32(pos), chars, u32(n))); }
static inline s32 putNumberAscii(u32 self, u32 w, s32 pos, s32 v, u32 maxD, u32 noPad) { return s32(call<u32>(0x025FCB90, self, w, u32(pos), u32(v), maxD, noPad)); }
static inline s32 putUnitString(u32 self, u32 w, s32 pos, u32 label) { return s32(call<u32>(0x025FCE5C, self, w, u32(pos), label)); }
static inline s32 putNumberUnit(u32 self, u32 w, s32 pos, s32 v, u32 ruby, u32 units) { return s32(call<u32>(0x025FDD08, self, w, u32(pos), u32(v), ruby, units)); }
static inline s32 putTime(u32 self, u32 w, s32 pos, s32 frames) { return s32(call<u32>(0x025FE38C, self, w, u32(pos), u32(frames))); }
static inline s32 putMessageById(u32 self, u32 w, s32 pos, u32 id, u32 ctx) { return s32(call<u32>(0x025FD550, self, w, u32(pos), id, ctx)); }

} // namespace hd_text_unit

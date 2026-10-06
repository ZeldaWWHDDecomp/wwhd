/* hd_snd_0202DAD0: two HD sound TUs (no GameCube source).
 *
 * 0202DAD0..0202DD2B: accessors of the HD sound player's NW4F objects (sead RTTI-checked; sound archive
 *                     +0x1C / sound heap +0x14 of 101F8BA8), NW4F sound id by name / by index; __sinit 0202DC98
 * 0202DD2C..0202EE07: the sound path strings: five FixedSafeString<256> statics (101FFE6C, 101FFF78,
 *                     10200084, 10200190, 1020029C) built by sead copy/append from the SafeString constants
 *                     101FFDF0..101FFE48 (set in the __sinit 0202EABC), their accessors, and companions
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_snd_0202DAD0 {

static inline u32 checked_obj(u32 field, u32 guard, u32 obj, u32 parent) {
    u32 o = ld(ld(0x101F8BA8) + field);
    if (ld(guard) == 0) {
        st(guard, 1);
        st(obj, parent);
    }
    if (o == 0) return 0;
    if (gabi::call_ptr<u32>(vfn(o, 0, 0xC), o, obj) == 0) return 0;
    return o;
}

static u32 SndPlayer_archiveObj() {
    WWHD_FUNC(0x0202DAD0, u32);
    return checked_obj(0x1C, 0x101FD634, 0x101FD950, 0x10004248);
}
VERIFY(0x0202DAD0, SndPlayer_archiveObj);

static u32 SndPlayer_heapObj() {
    WWHD_FUNC(0x0202DB6C, u32);
    return checked_obj(0x14, 0x101FD648, 0x101FD958, 0x10004258);
}
VERIFY(0x0202DB6C, SndPlayer_heapObj);

static u32 SndPlayer_archive() {
    WWHD_FUNC(0x0202DC08, u32);
    u32 p = gabi::call<u32>(0x0202DAD0);
    return gabi::call<u32>(0x02764774, ld(p + 0x144));
}
VERIFY(0x0202DC08, SndPlayer_archive);

static u32 SndPlayer_idByName(u32 name) {
    WWHD_FUNC(0x0202DC30, u32, name);
    u32 a = gabi::call<u32>(0x0202DC08);
    return gabi::call<u32>(0x0288E91C, a, name);
}
VERIFY(0x0202DC30, SndPlayer_idByName);

static u32 SndPlayer_itemById(u32 id) {
    WWHD_FUNC(0x0202DC64, u32, id);
    u32 a = gabi::call<u32>(0x0202DC08);
    return gabi::call<u32>(0x0288E914, a, id);
}
VERIFY(0x0202DC64, SndPlayer_itemById);

static void sinit_0202DC98() {
    WWHD_FUNC(0x0202DC98, void);
    header_sinit(0x101FFDE0, 0x1018ECB4, 0x10004268);
}
VERIFY(0x0202DC98, sinit_0202DC98);

/* sead BufferedSafeString::copy(src) into a buffer {data +0, vtable +4, capacity +8} */
static inline void bs_copy(u32 d, u32 src) {
    u32 fn = ld(ld(src + 4) + 0x14);
    u32 dd = ld(d + 0);
    gabi::call_ptr(fn, src);
    s32 len = sstrlen(ld(src + 0));
    s32 cap = (s32)ld(d + 8);
    if (!(len < cap)) len = cap - 1;
    sstr_assure(src);
    gabi::call(OSBlockMove, dd, ld(src + 0), len, 0);
    stb(dd + len, 0);
}

/* sead BufferedSafeString::append(src) */
static inline void bs_append(u32 d, u32 src) {
    u32 fn = ld(ld(d + 4) + 0x14);
    u32 dd = ld(d + 0);
    gabi::call_ptr(fn, d);
    s32 cur = sstrlen(ld(d + 0));
    s32 base = cur < 0 ? 0 : cur;
    sstr_assure(src);
    s32 n = sstrlen(ld(src + 0));
    s32 room = (s32)ld(d + 8) - base;
    if (!(n < room)) n = room - 1;
    if (n <= 0) return;
    sstr_assure(src);
    gabi::call(OSBlockMove, dd + base, ld(src + 0), n, 0);
    if (base + n > cur) stb(dd + base + n, 0);
}

struct sbuf128_l { u8 b[0x8C]; };

/* 0202DD2C: build the five sound path strings */
static void SndPath_build() {
    WWHD_FUNC(0x0202DD2C, void);
    bs_copy(0x101FFE6C, 0x101FFDF0);
    bs_append(0x101FFE6C, 0x101FFE18);
    bs_append(0x101FFE6C, 0x101FFE28);
    gabi::Local<sbuf128_l> t;
    stb(t.a + 0x8B, 0);
    st(t.a + 0, t.a + 0xC);
    st(t.a + 8, 0x80);
    stb(t.a + 0xC, 0);
    st(t.a + 4, 0x100042E8);
    bs_copy(t.a, 0x101FFDF8);
    bs_append(t.a, 0x101FFE10);
    bs_append(t.a, 0x101FFE18);
    bs_append(t.a, 0x101FFE20);
    bs_copy(0x101FFF78, t.a);
    bs_append(0x101FFF78, 0x101FFE30);
    bs_copy(0x10200084, t.a);
    bs_append(0x10200084, 0x101FFE38);
    bs_copy(0x10200190, t.a);
    bs_append(0x10200190, 0x101FFE40);
    bs_copy(0x1020029C, t.a);
    bs_append(0x1020029C, 0x101FFE48);
}
VERIFY(0x0202DD2C, SndPath_build);

static u32 SndPath_0202EA80() {
    WWHD_FUNC(0x0202EA80, u32);
    return 0x101FFE6C;
}
VERIFY(0x0202EA80, SndPath_0202EA80);

static u32 SndPath_0202EA8C() {
    WWHD_FUNC(0x0202EA8C, u32);
    return 0x101FFF78;
}
VERIFY(0x0202EA8C, SndPath_0202EA8C);

static u32 SndPath_0202EA98() {
    WWHD_FUNC(0x0202EA98, u32);
    return 0x10200084;
}
VERIFY(0x0202EA98, SndPath_0202EA98);

static u32 SndPath_0202EAA4() {
    WWHD_FUNC(0x0202EAA4, u32);
    return 0x10200190;
}
VERIFY(0x0202EAA4, SndPath_0202EAA4);

static u32 SndPath_0202EAB0() {
    WWHD_FUNC(0x0202EAB0, u32);
    return 0x1020029C;
}
VERIFY(0x0202EAB0, SndPath_0202EAB0);

/* 0202EABC: __sinit (header statics, twelve SafeString constants, five FixedSafeString<256> statics) */
static void sinit_0202EABC() {
    WWHD_FUNC(0x0202EABC, void);
    header_sinit(0x101FFE5C, 0x1018ECD8, 0x10004300);
    static const u32 strs[][2] = {
        {0x101FFDF0, 0x10004308}, {0x101FFE00, 0x10004310}, {0x101FFDF8, 0x1000431C}, {0x101FFE08, 0x10004314},
        {0x101FFE10, 0x10004318}, {0x101FFE18, 0x1000434C}, {0x101FFE38, 0x1000432C}, {0x101FFE48, 0x10004340},
        {0x101FFE40, 0x10004324}, {0x101FFE30, 0x10004334}, {0x101FFE28, 0x10004364}, {0x101FFE20, 0x10004358}};
    for (auto& p : strs) {
        st(p[0] + 4, 0x10004270);
        st(p[0], p[1]);
    }
    static const u32 bufs[] = {0x101FFE6C, 0x101FFF78, 0x10200084, 0x10200190, 0x1020029C};
    for (u32 i = 0; i < 5; ++i) {
        u32 d = bufs[i];
        stb(d + 0xC, 0);
        stb(d + 0x10B, 0);
        st(d + 0, d + 0xC);
        st(d + 8, 0x100);
        st(d + 4, 0x100042B8);
        gabi::call(0x028F026C, 0x1018ECFC + i * 0xC);
    }
}
VERIFY(0x0202EABC, sinit_0202EABC);

static void Comp_dt1(u32 self, s32 flags) {
    WWHD_FUNC(0x0202ED74, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202ED74, Comp_dt1);

static void SafeString_assureNone() {
    WWHD_FUNC(0x0202ED88, void);
}
VERIFY(0x0202ED88, SafeString_assureNone);

/* sead BufferedSafeString::assureTerminationImpl_ instance */
static void SafeString_assureTermination(u32 self) {
    WWHD_FUNC(0x0202ED8C, void, self);
    stb(ld(self + 0) + ld(self + 8) - 1, 0);
}
VERIFY(0x0202ED8C, SafeString_assureTermination);

static void Comp_dt2(u32 self, s32 flags) {
    WWHD_FUNC(0x0202EDA4, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202EDA4, Comp_dt2);

static void Comp_dt3(u32 self, s32 flags) {
    WWHD_FUNC(0x0202EDB8, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202EDB8, Comp_dt3);

static void Comp_dt4(u32 self, s32 flags) {
    WWHD_FUNC(0x0202EDCC, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202EDCC, Comp_dt4);

static void Comp_dt5(u32 self, s32 flags) {
    WWHD_FUNC(0x0202EDE0, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202EDE0, Comp_dt5);

static void Comp_dt6(u32 self, s32 flags) {
    WWHD_FUNC(0x0202EDF4, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202EDF4, Comp_dt6);

}  // namespace hd_snd_0202DAD0

/* hd_snd_0202B6B0: two HD sound TUs after JAIZel (no GameCube source).
 *
 * 0202B6B0..0202B9FF: the sound archive / bank-table accessor (0x10, vtable 10004008, table +0xC):
 *                     load of the "sound data" resource through the resource manager (0275DB94) with a
 *                     sead RTTI check, sound id decoding (bank from the top bits / bits 12..19), entry
 *                     lookup in the table (+0x250, 32-byte entries), stream test; __sinit 0202B924 sets
 *                     a static SafeString (+ companions)
 * 0202BA00..0202BD3B: the HD sound system container (0x18): six sub-systems created on a heap (0x1D0
 *                     020305A0, 0x20C 0202F048, 0x40 02030364, 0x4 020317C0, 0x6C 020312CC, 0x10 this
 *                     archive accessor), their destructor, and accessors; __sinit 0202BCA8
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_snd_0202B6B0 {

static u32 Archive_ct(u32 self) {
    WWHD_FUNC(0x0202B6B0, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x10);
        if (t == 0) return 0;
    }
    st(t + 8, 0);
    st(t + 0, 0x10004008);
    st(t + 0xC, 0);
    st(t + 4, 0);
    return t;
}
VERIFY(0x0202B6B0, Archive_ct);

static void Archive_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x0202B700, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202B700, Archive_dt);

static void Archive_setTable(u32 self, u32 tab) {
    WWHD_FUNC(0x0202B714, void, self, tab);
    st(self + 0xC, tab);
}
VERIFY(0x0202B714, Archive_setTable);

struct load_l { u8 b[0x2C]; }; /* resource-manager load argument (name SafeString first) */

/* 0202B71C: load the table resource SIZE bytes through the resource manager */
static void Archive_load(u32 self, u32 a, u32 size) {
    WWHD_FUNC(0x0202B71C, void, self, a, size);
    (void)a;
    gabi::Local<load_l> l;
    st(l.a + 4, 0x10003F98);
    st(l.a + 0, 0x10000160);
    st(l.a + 0x28, 0);
    st(l.a + 0x18, 0);
    st(l.a + 0x14, 0);
    st(l.a + 0x10, 0x20);
    st(l.a + 0x24, 0);
    st(l.a + 0x20, 0);
    st(l.a + 0xC, 0);
    st(l.a + 8, 0);
    st(l.a + 0x1C, 0);
    u32 nm = gabi::call<u32>(0x0202EAB0);
    u32 c = ld(nm + 0);
    st(l.a + 8, size);
    st(l.a + 0x10, 0x40);
    st(l.a + 0x14, 0x40);
    st(l.a + 0xC, size);
    st(l.a + 0, c);
    u32 r = gabi::call<u32>(0x0275DB94, ld(0x101F8B68), l.a);
    if (ld(0x101FD6E0) == 0) {
        st(0x101FD6E0, 1);
        st(0x101FD944, 0x10003FE0);
    }
    if (r != 0) {
        if (gabi::call_ptr<u32>(vfn(r, 0x10, 0xC), r, 0x101FD944) == 0) r = 0;
    }
    gabi::call(0x0202B714, self, ld(r + 0x14)); /* r may be null: the original reads 0x14 then */
}
VERIFY(0x0202B71C, Archive_load);

/* 0202B828: bank of a sound id: 0x10 / 0x11 for the 0x8/0xC top-bit kinds, -1 for 0x4, else bits 12..19 */
static s32 Archive_bank(u32 self, u32 id) {
    WWHD_FUNC(0x0202B828, s32, self, id);
    u32 k = id & 0xC0000000u;
    if (k == 0) return (s32)((id >> 12) & 0xFF);
    if (k == 0x80000000u) return 0x10;
    if (k == 0xC0000000u) return 0x11;
    return -1;
}
VERIFY(0x0202B828, Archive_bank);

static u32 Archive_bankBase(u32 self, s32 bank) {
    WWHD_FUNC(0x0202B868, u32, self, bank);
    return lhz(ld(self + 0xC) + (u32)bank * 32 + 0x2E);
}
VERIFY(0x0202B868, Archive_bankBase);

/* 0202B87C: table entry of a sound id */
static u32 Archive_entry(u32 self, u32 id) {
    WWHD_FUNC(0x0202B87C, u32, self, id);
    u32 tab = ld(self + 0xC);
    if (tab == 0) return 0;
    s32 bank = gabi::call<s32>(0x0202B828, self, id);
    u32 base = gabi::call<u32>(0x0202B868, self, bank);
    return tab + 0x250 + ((base + (id & 0x3FF)) << 5);
}
VERIFY(0x0202B87C, Archive_entry);

/* 0202B8EC: sound id of a sequence bank (< 0x10) with bit 11 clear */
static u32 Archive_isSeq(u32 self, u32 id) {
    WWHD_FUNC(0x0202B8EC, u32, self, id);
    s32 bank = gabi::call<s32>(0x0202B828, self, id);
    if (bank >= 0x10) return 0;
    return ((id >> 11) & 1) ^ 1;
}
VERIFY(0x0202B8EC, Archive_isSeq);

/* 0202B924: __sinit (header statics + a static SafeString) */
static void sinit_0202B924() {
    WWHD_FUNC(0x0202B924, void);
    header_sinit(0x101FFD54, 0x1018EBF8, 0x10003FF0);
    st(0x101FFD44, 0x10003F98);
    st(0x101FFD40, 0x10003FF8);
}
VERIFY(0x0202B924, sinit_0202B924);

static void Comp_dt1(u32 self, s32 flags) {
    WWHD_FUNC(0x0202B9D8, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202B9D8, Comp_dt1);

static void Comp_empty1() {
    WWHD_FUNC(0x0202B9EC, void);
}
VERIFY(0x0202B9EC, Comp_empty1);

static void Comp_empty2() {
    WWHD_FUNC(0x0202B9F0, void);
}
VERIFY(0x0202B9F0, Comp_empty2);

static u32 Comp_zero() {
    WWHD_FUNC(0x0202B9F4, u32);
    return 0;
}
VERIFY(0x0202B9F4, Comp_zero);

static void Comp_empty3() {
    WWHD_FUNC(0x0202B9FC, void);
}
VERIFY(0x0202B9FC, Comp_empty3);

static inline u32 make(u32 size, u32 heap, u32 ctor) {
    u32 p = gabi::call<u32>(0x0273B050, size, heap, 4);
    if (p != 0) p = gabi::call<u32>(ctor, p);
    return p;
}

/* 0202BA00: sound system container constructor (heap) */
static u32 SoundSys_ct(u32 self, u32 heap) {
    WWHD_FUNC(0x0202BA00, u32, self, heap);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x18);
        if (t == 0) return 0;
    }
    st(t + 0x14, 0);
    st(t + 0xC, 0);
    st(t + 8, 0);
    st(t + 0, 0);
    st(t + 0x10, 0);
    st(t + 4, 0);
    st(t + 0, make(0x1D0, heap, 0x020305A0));
    st(t + 4, make(0x20C, heap, 0x0202F048));
    st(t + 8, make(0x40, heap, 0x02030364));
    u32 p = gabi::call<u32>(0x0273B050, 4, heap, 4);
    if (p != 0) p = gabi::call<u32>(0x020317C0, p, 0x20, heap);
    st(t + 0xC, p);
    st(t + 0x10, make(0x6C, heap, 0x020312CC));
    st(t + 0x14, make(0x10, heap, 0x0202B6B0));
    return t;
}
VERIFY(0x0202BA00, SoundSys_ct);

/* 0202BB58: destructor: deletes the six sub-systems */
static void SoundSys_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x0202BB58, void, self, flags);
    if (self == 0) return;
    for (u32 off = 0; off <= 8; off += 4) {
        u32 x = ld(self + off);
        if (x != 0) {
            gabi::call_ptr(vfn(x, 0, 0x1C), x, 3);
            st(self + off, 0);
        }
    }
    u32 x = ld(self + 0xC);
    if (x != 0) {
        gabi::call(0x02031838, x, 3);
        st(self + 0xC, 0);
    }
    for (u32 off = 0x10; off <= 0x14; off += 4) {
        x = ld(self + off);
        if (x != 0) {
            gabi::call_ptr(vfn(x, 0, 0x24), x, 3);
            st(self + off, 0);
        }
    }
    if (flags & 1) op_delete(self);
}
VERIFY(0x0202BB58, SoundSys_dt);

static u32 SoundSys_get0(u32 self) {
    WWHD_FUNC(0x0202BC78, u32, self);
    return ld(self + 0x0);
}
VERIFY(0x0202BC78, SoundSys_get0);

static u32 SoundSys_get1(u32 self) {
    WWHD_FUNC(0x0202BC80, u32, self);
    return ld(self + 0x4);
}
VERIFY(0x0202BC80, SoundSys_get1);

static u32 SoundSys_get2(u32 self) {
    WWHD_FUNC(0x0202BC88, u32, self);
    return ld(self + 0x8);
}
VERIFY(0x0202BC88, SoundSys_get2);

static u32 SoundSys_get3(u32 self) {
    WWHD_FUNC(0x0202BC90, u32, self);
    return ld(self + 0xC);
}
VERIFY(0x0202BC90, SoundSys_get3);

static u32 SoundSys_get4(u32 self) {
    WWHD_FUNC(0x0202BC98, u32, self);
    return ld(self + 0x10);
}
VERIFY(0x0202BC98, SoundSys_get4);

static u32 SoundSys_get5(u32 self) {
    WWHD_FUNC(0x0202BCA0, u32, self);
    return ld(self + 0x14);
}
VERIFY(0x0202BCA0, SoundSys_get5);

static void sinit_0202BCA8() {
    WWHD_FUNC(0x0202BCA8, void);
    header_sinit(0x101FFD70, 0x1018EC1C, 0x100040A8);
}
VERIFY(0x0202BCA8, sinit_0202BCA8);

}  // namespace hd_snd_0202B6B0

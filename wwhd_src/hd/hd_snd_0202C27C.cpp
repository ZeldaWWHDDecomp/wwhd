/* hd_snd_0202C27C: the HD JAudio-to-NW4F sound bridge (no GameCube source).
 *
 * TU 0202C27C..0202DACF (__sinit 0202D9D0 + companions). A 0x44 object (vtables 100041F8 / 10004228):
 * JAI sound pool (list +8, count +0x10, free list +0x14, 64 x 0x50 pool +0x18, capacity +0x1C) and
 * NW4F handle pool (list +0x20, count +0x28, free +0x2C, 64 x 0x1C pool +0x30, capacity +0x34),
 * ducking counter +0x3C, flags +0x38 (enabled) +0x39 +0x40 +0x41 +0x42 +0x43 (debug print).
 * A JA sound is played on NW4F by converting its name "JA_..." to "NW_..." and looking the id up in
 * the NW4F sound archive (0288E91C / 0288E914); sounds without an NW4F counterpart are filtered
 * (Link's "JA_SE_LK" voice set, stage-specific streams for Demo10/Demo23/Demo45, a few id ranges).
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_snd_0202C27C {

/* free list of COUNT entries of SIZE bytes at P (each entry starts with the next pointer) */
static inline void make_pool(u32 p, u32 size) {
    for (u32 i = 0; i < 63; ++i) st(p + i * size, p + (i + 1) * size);
    st(p + 63 * size, 0);
}

static inline void list_ct(u32 a) {
    st(a + 8, 0);
    st(a + 0x10, 0);
    st(a + 0x14, 0);
    st(a + 0xC, 0);
    st(a + 0, a);
    st(a + 4, a);
}

/* 0202C27C: constructor */
static u32 Bridge_ct(u32 self) {
    WWHD_FUNC(0x0202C27C, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x44);
        if (t == 0) return 0;
    }
    st(t + 0, 0x100041F8);
    st(t + 4, 0x10004228);
    u32 a = t + 8;
    if (a == 0) a = op_new(0x18);
    if (a != 0) list_ct(a);
    u32 b = t + 0x20;
    if (b == 0) b = op_new(0x18);
    if (b != 0) list_ct(b);
    stb(t + 0x42, 0);
    st(t + 0x3C, 0);
    stb(t + 0x39, 1);
    stb(t + 0x41, 1);
    stb(t + 0x38, 1);
    stb(t + 0x40, 0);
    stb(t + 0x43, 0);
    u32 p = gabi::call<u32>(0x0273B0D4, 0x1400, 0, 4);
    if (p != 0) {
        st(t + 0x14, p);
        make_pool(p, 0x50);
        st(t + 0x18, p);
        st(t + 0x1C, 0x40);
    }
    u32 q = gabi::call<u32>(0x0273B0D4, 0x700, 0, 4);
    if (q == 0) return t;
    st(t + 0x2C, q);
    make_pool(q, 0x1C);
    st(t + 0x30, q);
    st(t + 0x34, 0x40);
    return t;
}
VERIFY(0x0202C27C, Bridge_ct);

/* destroy every pooled object of a list (node at +NODE inside the object, vtable pointer at +VPTR) */
static inline void pool_clear(u32 list, u32 node, u32 vptr) {
    for (u32 n = ld(list + 4); n != list; ) {
        u32 next = ld(n + 4);
        gabi::call(0x0273B310, n);
        st(list + 8, ld(list + 8) - 1);
        u32 obj = n - node;
        gabi::call_ptr(ld(ld(obj + vptr) + 0xC), obj, 2);
        st(obj, ld(list + 0xC));
        n = next;
        st(list + 0xC, obj);
    }
    gabi::call(0x0273AFC8, ld(list + 0x10));
    st(list + 0xC, 0);
    st(list + 0x14, 0);
    st(list + 0x10, 0);
}

/* 0202C3F8: destructor */
static void Bridge_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x0202C3F8, void, self, flags);
    if (self == 0) return;
    u32 poolA = ld(self + 0x18);
    st(self + 0, 0x100041F8);
    st(self + 4, 0x10004228);
    if (poolA != 0) pool_clear(self + 8, 0x48, 0x44);
    if (ld(self + 0x30) != 0) pool_clear(self + 0x20, 0x14, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x0202C3F8, Bridge_dt);

/* 0202C54C: the sound archive accessor of the sound manager (null without one) */
static u32 Bridge_archive() {
    WWHD_FUNC(0x0202C54C, u32);
    u32 m = ld(0x1018EC64);
    if (m == 0) return 0;
    return gabi::call<u32>(0x0202BF90, m);
}
VERIFY(0x0202C54C, Bridge_archive);

/* 0202C57C: the NW4F sound archive of the HD sound player (sead RTTI-checked), or null */
static u32 Bridge_nwArchive() {
    WWHD_FUNC(0x0202C57C, u32);
    u32 r = ld(0x101F8BA8);
    if (r == 0) return 0;
    u32 o = ld(r + 0x1C);
    if (o == 0) return 0;
    if (ld(0x101FD634) == 0) {
        st(0x101FD634, 1);
        st(0x101FD950, 0x10004178);
    }
    if (gabi::call_ptr<u32>(vfn(o, 0, 0xC), o, 0x101FD950) == 0) o = 0;
    u32 x = ld(o + 0x144);
    if (x == 0) return 0;
    return gabi::call<u32>(0x02764774, x);
}
VERIFY(0x0202C57C, Bridge_nwArchive);

struct str2_l { u8 b[8]; };
struct sbuf64_l { u8 b[0x4C]; };

/* copy the SafeString SRC into the FixedSafeString<64> BUF (sead copy: assure, bounded length) */
static inline void sbuf_copy(u32 buf, u32 src) {
    u32 chars = buf + 0xC;
    s32 len = sstrlen(ld(src + 0));
    s32 cap = (s32)ld(buf + 8);
    if (!(len < cap)) len = cap - 1;
    sstr_assure(src);
    gabi::call(OSBlockMove, chars, ld(src + 0), len, 0);
    stb(chars + len, 0);
}

static inline void sbuf64_ct(u32 buf) {
    st(buf + 0, buf + 0xC);
    stb(buf + 0xC, 0);
    stb(buf + 0x4B, 0);
}

/* 0202C630: NW4F sound id of a JA sound id: its name with "JA" replaced by "NW", looked up in the archive */
static u32 Bridge_nwId(u32 self, u32 id) {
    WWHD_FUNC(0x0202C630, u32, self, id);
    u32 m = gabi::call<u32>(0x0202C54C);
    if (m == 0) return 0;
    u32 e = gabi::call<u32>(0x0202B87C, m, id);
    if (e == 0) return 0;
    gabi::Local<str2_l> src;
    gabi::Local<sbuf64_l> buf;
    sbuf64_ct(buf.a);
    st(src.a + 0, e);
    st(buf.a + 8, 0x40);
    st(buf.a + 4, 0x100041A0);
    st(src.a + 4, 0x10004118);
    gabi::call(0x0202DA78, src.a);
    sbuf_copy(buf.a, src.a);
    sstr_assure(buf.a);
    u32 c = ld(buf.a + 0);
    stb(c + 0, 0x4E);
    stb(c + 1, 0x57);
    u32 h = gabi::call<u32>(0x0202C57C);
    if (h == 0) return 0;
    sstr_assure(buf.a);
    s32 n = gabi::call<s32>(0x0288E91C, h, ld(buf.a + 0));
    if (n == -1) return 0;
    return gabi::call<u32>(0x0288E914, h, n);
}
VERIFY(0x0202C630, Bridge_nwId);

/* 0202C7C4: archive entry of a JA sound id (not for stream sounds when +0x41 is set) */
static u32 Bridge_entry(u32 self, u32 id) {
    WWHD_FUNC(0x0202C7C4, u32, self, id);
    u32 m = gabi::call<u32>(0x0202C54C);
    if (m == 0) return 0;
    u32 e = gabi::call<u32>(0x0202B87C, m, id);
    if (e == 0) return 0;
    if (lbz(self + 0x41) != 0 && gabi::call<u32>(0x0202B8EC, m, id) != 0) return 0;
    return e;
}
VERIFY(0x0202C7C4, Bridge_entry);

/* 0202C870: "handled": clear the caller's JAI sound slot */
static u32 Bridge_handled(u32 self, u32 out) {
    WWHD_FUNC(0x0202C870, u32, self, out);
    if (out != 0) st(out, 0);
    return 1;
}
VERIFY(0x0202C870, Bridge_handled);

/* 0202C888: system sounds of the 0x8/0xC kinds except two (0x80000800, 0xC000004A) */
static u32 Bridge_isSystem(u32 self, u32 id) {
    WWHD_FUNC(0x0202C888, u32, self, id);
    u32 k = id & 0xC0000000u;
    if (k == 0xC0000000u) return id != 0xC000004Au ? 1 : 0;
    if (k == 0x80000000u) return id != 0x80000800u ? 1 : 0;
    return 0;
}
VERIFY(0x0202C888, Bridge_isSystem);

/* 0202C8D4: game over is starting */
static u32 Bridge_gameOver(u32 self) {
    WWHD_FUNC(0x0202C8D4, u32, self);
    u32 pl = ld(gabi::call<u32>(0x025200D4) + 0x5B34);
    if (pl == 0) return 0;
    return gabi::call<u32>(0x02443664, pl) != 0 ? 1 : 0;
}
VERIFY(0x0202C8D4, Bridge_gameOver);

static u32 Bridge_isSe(u32 self, u32 id) {
    WWHD_FUNC(0x0202C924, u32, self, id);
    return (id & 0xC0000000u) == 0 ? 1 : 0;
}
VERIFY(0x0202C924, Bridge_isSe);

static u32 Bridge_isSeBank0(u32 self, u32 id) {
    WWHD_FUNC(0x0202C934, u32, self, id);
    if (id & 0xC0000000u) return 0;
    if (id & 0x000FF000u) return 0;
    return 1;
}
VERIFY(0x0202C934, Bridge_isSeBank0);

/* 0202C954: the sound's name contains "JA_SE_LK" (Link's voice set) */
static u32 Bridge_isLinkVoice(u32 self, u32 id) {
    WWHD_FUNC(0x0202C954, u32, self, id);
    if (id & 0xC0000000u) return 0;
    u32 m = gabi::call<u32>(0x0202C54C);
    if (m == 0) return 0;
    u32 e = gabi::call<u32>(0x0202B87C, m, id);
    gabi::Local<str2_l> c;
    gabi::Local<str2_l> a;
    gabi::Local<sbuf64_l> buf;
    st(a.a + 0, e);
    sbuf64_ct(buf.a);
    st(a.a + 4, 0x10004118);
    st(buf.a + 8, 0x40);
    st(buf.a + 4, 0x100041A0);
    gabi::call(0x0202DA78, a.a);
    sbuf_copy(buf.a, a.a);
    st(a.a + 4, 0x10004118);
    st(a.a + 0, 0x100041B8); /* "JA_SE_LK" */
    sstr_assure(buf.a);
    s32 n = sstrlen(ld(buf.a + 0));
    sstr_assure(a.a);
    s32 k = sstrlen(ld(a.a + 0));
    s32 last = n - k;
    if (last < 0) return 0;
    for (s32 i = 0; i <= last; ++i) {
        st(c.a + 4, 0x10004118);
        st(c.a + 0, ld(buf.a + 0) + i);
        gabi::call(0x0202DA78, c.a);
        sstr_assure(c.a);
        u32 cd = ld(c.a + 0);
        sstr_assure(a.a);
        u32 ad = ld(a.a + 0);
        if (cd == ad || k <= 0) return 1;
        bool match = true;
        for (s32 j = 0; j < k; ++j) {
            u8 c0 = lbz(cd + j);
            u8 c1 = lbz(ad + j);
            if (c0 == 0) { if (c1 != 0) match = false; break; }
            if (c1 == 0 || c1 != c0) { match = false; break; }
        }
        if (match) return 1;
    }
    return 0;
}
VERIFY(0x0202C954, Bridge_isLinkVoice);

/* 0202CBF8: an NW4F handle plays sound ID */
static u32 Bridge_isPlaying(u32 self, u32 id) {
    WWHD_FUNC(0x0202CBF8, u32, self, id);
    if (ld(self + 0x28) == 0) return 0;
    u32 end = self + 0xC;
    for (u32 o = ld(self + 0x24) - 0x14; o != end; o = ld(o + 0x18) - 0x14)
        if (gabi::call_ptr<u32>(vfn(o, 0, 0x34), o) == id) return 1;
    return 0;
}
VERIFY(0x0202CBF8, Bridge_isPlaying);

/* 0202CCA0: filter: true when the JA sound must not be played on NW4F (OUT cleared through 0202C870) */
static u32 Bridge_filter(u32 self, u32 id, u32 out) {
    WWHD_FUNC(0x0202CCA0, u32, self, id, out);
    bool ok = false;
    if (lbz(self + 0x38) == 0) {
        ok = true;
    } else {
        if (lbz(self + 0x39) == 0 && gabi::call<u32>(0x0202C888, self, id) != 0) ok = true;
        if (!ok && gabi::call<u32>(0x0202C8D4, self) != 0 && gabi::call<u32>(0x0202C924, self, id) != 0 &&
            gabi::call<u32>(0x0202C934, self, id) == 0 && gabi::call<u32>(0x0202C954, self, id) == 0)
            ok = true;
        if (!ok) {
            bool demo = gabi::call<u32>(0x02030CAC) != 0 || gabi::call<u32>(0x02030D20) != 0;
            if (demo && id >= 0x804 && (id <= 0x805 || (id >= 0x81C && (id <= 0x81D || id == 0x8F8 || id == 0x90A))))
                ok = true;
        }
        if (!ok) {
            gabi::Local<str2_l> s1;
            gabi::Local<str2_l> s2;
            st(s1.a + 4, 0x10004118);
            st(s2.a + 4, 0x10004118);
            st(s1.a + 0, 0x100041C4); /* "Demo10" */
            st(s2.a + 0, 0x1047E6B8); /* current stage name */
            gabi::call(0x0202DA78, s1.a);
            sstr_assure(s1.a);
            u32 fn2 = ld(ld(s2.a + 4) + 0x14);
            u32 p = ld(s1.a + 0);
            gabi::call_ptr(fn2, s2.a);
            u32 q = ld(s2.a + 0);
            bool eq = p == q || sstr_eq(p, q);
            if (eq && ld(0x101D6010) == 1 && id == 0x380A) {
                ok = true;
            } else if (id == 0x806) {
                if (gabi::call<u32>(0x0202CBF8, self, 0x806) == 0) return 0;
                ok = true;
            } else if (id == 0x826) {
                if ((s32)ld(self + 0x3C) <= 0) return 0;
                ok = true;
            } else {
                return 0;
            }
        }
    }
    return gabi::call<u32>(0x0202C870, self, out);
}
VERIFY(0x0202CCA0, Bridge_filter);

/* 0202CED4: NW4F handle object bound to a JAI sound slot */
static u32 Bridge_findHandle(u32 self, u32 key) {
    WWHD_FUNC(0x0202CED4, u32, self, key);
    if (ld(self + 0x28) == 0) return 0;
    u32 end = self + 0xC;
    u32 node = ld(self + 0x24);
    u32 o = node - 0x14;
    u32 next = ld(node + 4);
    while (o != end) {
        if (ld(o + 0x10) == key) return o;
        o = next - 0x14;
        next = ld(next + 4);
    }
    return 0;
}
VERIFY(0x0202CED4, Bridge_findHandle);

/* 0202CF1C: stop what plays in the JAI sound slot SLOT (JAI sound and/or NW4F handle) */
static void Bridge_stopSlot(u32 self, u32 slot) {
    WWHD_FUNC(0x0202CF1C, void, self, slot);
    if (slot == 0) return;
    if (ld(slot) != 0) {
        u32 sid = gabi::call<u32>(0x0280D2D8, ld(slot));
        u32 m = gabi::call<u32>(0x0202C54C);
        if (m != 0 && gabi::call<u32>(0x0202B8EC, m, sid) == 0) {
            gabi::call(0x0280B1D8, ld(slot), 0);
            st(slot, 0);
        }
    }
    u32 h = gabi::call<u32>(0x0202CED4, self, slot);
    if (h == 0) return;
    gabi::call_ptr(vfn(h, 0, 0x14), h, 0);
    st(h + 0x10, 0);
    st(slot, 0);
}
VERIFY(0x0202CF1C, Bridge_stopSlot);

/* release a JAI sound object back to its pool */
static inline void release_jai(u32 self, u32 a) {
    gabi::call(0x0273B310, a + 0x48);
    st(self + 0x10, ld(self + 0x10) - 1);
    gabi::call_ptr(ld(ld(a + 0x44) + 0xC), a, 2);
    st(a, ld(self + 0x14));
    st(self + 0x14, a);
}

/* 0202CFDC: take a JAI sound object and an NW4F handle object from the pools */
static u32 Bridge_alloc(u32 self, u32 outA, u32 outB) {
    WWHD_FUNC(0x0202CFDC, u32, self, outA, outB);
    if (!((s32)ld(self + 0x10) < (s32)ld(self + 0x1C))) {
        st(outA, 0);
        return 0;
    }
    u32 n = ld(self + 0x14);
    if (n != 0) st(self + 0x14, ld(n));
    if (n != 0) {
        gabi::call(0x0280AF40, n);
        st(n + 0x4C, 0);
        st(n + 0x48, 0);
    }
    gabi::call(0x0273B2F0, self + 8, n + 0x48);
    st(self + 0x10, ld(self + 0x10) + 1);
    st(outA, n);
    if (n == 0) return 0;
    if (!((s32)ld(self + 0x28) < (s32)ld(self + 0x34))) {
        st(outB, 0);
        release_jai(self, ld(outA));
        return 0;
    }
    u32 h = ld(self + 0x2C);
    if (h != 0) st(self + 0x2C, ld(h));
    if (h != 0) {
        gabi::call(0x0202C0D0, h);
        st(h + 0x18, 0);
        st(h + 0x14, 0);
    }
    gabi::call(0x0273B2F0, self + 0x20, h + 0x14);
    st(self + 0x28, ld(self + 0x28) + 1);
    st(outB, h);
    u32 a = ld(outA);
    if (h == 0) {
        release_jai(self, a);
        return 0;
    }
    st(a + 0x40, h);
    st(ld(outB) + 8, ld(outA));
    return 1;
}
VERIFY(0x0202CFDC, Bridge_alloc);

/* 0202D1CC: give both objects back */
static void Bridge_free(u32 self, u32 a, u32 h) {
    WWHD_FUNC(0x0202D1CC, void, self, a, h);
    st(h + 8, 0);
    gabi::call(0x0273B310, h + 0x14);
    st(self + 0x28, ld(self + 0x28) - 1);
    gabi::call_ptr(ld(ld(h) + 0xC), h, 2);
    st(h, ld(self + 0x2C));
    st(self + 0x2C, h);
    release_jai(self, a);
}
VERIFY(0x0202D1CC, Bridge_free);

/* 0202D284: record the JA id in the handle and in the NW4F sound (null-unsafe like the original) */
static void Bridge_setId(u32 self, u32 a, u32 h, u32 id) {
    WWHD_FUNC(0x0202D284, void, self, a, h, id);
    u32 p = ld(h + 4);
    st(h + 0xC, id);
    u32 q = p == 0 ? 0 : ld(p + 0x268);
    st(q + 0, id);
    st(q + 4, a);
}
VERIFY(0x0202D284, Bridge_setId);

static u32 Bridge_player() {
    WWHD_FUNC(0x0202D2B4, u32);
    return gabi::call<u32>(0x0202BF98, ld(0x1018EC64));
}
VERIFY(0x0202D2B4, Bridge_player);

/* start a JA sound on NW4F (0202D2C0 SE / 0202D4D0 BGM, START = 02897F48 / 02897D08) */
static inline u32 start_common(u32 self, u32 id, u32 slot, u32 start) {
    if (lbz(self + 0x40) != 0) gabi::call(0x0202C7C4, self, id);
    gabi::call(0x0202CF1C, self, slot);
    u32 nw = gabi::call<u32>(0x0202C630, self, id);
    if (nw == 0) {
        u32 pl = gabi::call<u32>(0x0202D2B4);
        if (lbz(pl + 0x10) != 0) return 0;
        return gabi::call<u32>(0x0202C870, self, slot);
    }
    gabi::Local<be<u32>> b;
    gabi::Local<be<u32>> a;
    st(a.a, 0);
    st(b.a, 0);
    if (gabi::call<u32>(0x0202CFDC, self, a.a, b.a) == 0) {
        if (slot != 0) st(slot, 0);
        return 1;
    }
    u32 p = gabi::call<u32>(0x0202DAD0);
    u32 r = gabi::call<u32>(start, p + 4, ld(b.a) + 4, nw, 0);
    u32 av = ld(a.a), bv = ld(b.a);
    if (r != 0) {
        gabi::call(0x0202D1CC, self, av, bv);
        if (slot != 0) st(slot, 0);
        return 1;
    }
    gabi::call(0x0202D284, self, av, bv, id);
    if (slot != 0) {
        st(ld(b.a) + 0x10, slot);
        st(slot, ld(a.a));
    }
    return 1;
}

static u32 Bridge_startSe(u32 self, u32 id, u32 slot) {
    WWHD_FUNC(0x0202D2C0, u32, self, id, slot);
    if (gabi::call<u32>(0x0202CCA0, self, id, slot) != 0) return 1;
    return start_common(self, id, slot, 0x02897F48);
}
VERIFY(0x0202D2C0, Bridge_startSe);

/* 0202D428: call the handle virtual +0x14 (ARG) of every NW4F handle playing ID */
static void Bridge_forId(u32 self, u32 id, u32 arg) {
    WWHD_FUNC(0x0202D428, void, self, id, arg);
    if (ld(self + 0x28) == 0) return;
    u32 end = self + 0xC;
    for (u32 o = ld(self + 0x24) - 0x14; o != end; o = ld(o + 0x18) - 0x14)
        if (gabi::call_ptr<u32>(vfn(o, 0, 0x34), o) == id) gabi::call_ptr(vfn(o, 0, 0x14), o, arg);
}
VERIFY(0x0202D428, Bridge_forId);

static u32 Bridge_startBgm(u32 self, u32 id, u32 slot) {
    WWHD_FUNC(0x0202D4D0, u32, self, id, slot);
    if (gabi::call<u32>(0x0202CCA0, self, id, slot) != 0) return 1;
    if (id == 0x826) {
        gabi::call(0x0202D428, self, 0x826, 5);
        st(self + 0x3C, 2);
    }
    return start_common(self, id, slot, 0x02897D08);
}
VERIFY(0x0202D4D0, Bridge_startBgm);

/* 0202D658: per frame: recycle finished NW4F handles (and their JAI objects), debug print, ducking */
static void Bridge_update(u32 self) {
    WWHD_FUNC(0x0202D658, void, self);
    if (ld(self + 0x28) != 0) {
        u32 end = self + 0xC;
        u32 node = ld(self + 0x24);
        u32 o = node - 0x14;
        u32 next = ld(node + 4);
        while (o != end) {
            if (gabi::call_ptr<u32>(vfn(o, 0, 0x2C), o) == 0) {
                u32 slot = ld(o + 0x10);
                u32 a = ld(o + 8);
                st(o + 0xC, 0xFFFFFFFF);
                st(o + 8, 0);
                if (slot != 0) {
                    st(slot, 0);
                    st(o + 0x10, 0);
                }
                release_jai(self, a);
                gabi::call(0x0273B310, o + 0x14);
                st(self + 0x28, ld(self + 0x28) - 1);
                gabi::call_ptr(ld(ld(o) + 0xC), o, 2);
                st(o, ld(self + 0x2C));
                st(self + 0x2C, o);
            } else if (lbz(self + 0x43) != 0) {
                u32 w = ld(o + 4);
                if (w != 0) gabi::call(0x027EC9E8, 0x1C2, 0x190, 0x100041CC, ld(w + 0x100));
                else gabi::call(0x027EC9E8, 0x1C2, 0x190, 0x100041CC, -1);
            }
            o = next - 0x14;
            next = ld(next + 4);
        }
    }
    s32 c = (s32)ld(self + 0x3C);
    st(self + 0x3C, c > 0 ? (u32)(c - 1) : 0);
}
VERIFY(0x0202D658, Bridge_update);

/* 0202D7F8: stage-specific stream substitutions (Demo23 0x5800 -> 0x586E, Demo45 0x588D -> 0x59A3 when 101D6010 == 1) */
static u32 Bridge_remap(u32 self, u32 id) {
    WWHD_FUNC(0x0202D7F8, u32, self, id);
    gabi::Local<str2_l> s1;
    gabi::Local<str2_l> s3;
    gabi::Local<str2_l> s2;
    gabi::Local<str2_l> s4;
    st(s1.a + 4, 0x10004118);
    st(s1.a + 0, 0x100041E0); /* "Demo23" */
    st(s2.a + 4, 0x10004118);
    st(s2.a + 0, 0x1047E6B8);
    gabi::call_ptr(0x0202DA78, s1.a);
    sstr_assure(s1.a);
    u32 fn2 = ld(ld(s2.a + 4) + 0x14);
    u32 p = ld(s1.a + 0);
    gabi::call_ptr(fn2, s2.a);
    u32 q = ld(s2.a + 0);
    if ((p == q || sstr_eq(p, q)) && id == 0x5800) return 0x586E;
    st(s3.a + 4, 0x10004118);
    st(s3.a + 0, 0x100041E8); /* "Demo45" */
    st(s4.a + 0, 0x1047E6B8);
    st(s4.a + 4, 0x10004118);
    gabi::call_ptr(0x0202DA78, s3.a);
    sstr_assure(s3.a);
    u32 fn4 = ld(ld(s4.a + 4) + 0x14);
    p = ld(s3.a + 0);
    gabi::call_ptr(fn4, s4.a);
    q = ld(s4.a + 0);
    if ((p == q || sstr_eq(p, q)) && ld(0x101D6010) == 1 && id == 0x588D) return 0x59A3;
    return id;
}
VERIFY(0x0202D7F8, Bridge_remap);

static void sinit_0202D9D0() {
    WWHD_FUNC(0x0202D9D0, void);
    header_sinit(0x101FFDC4, 0x1018EC90, 0x100041F0);
}
VERIFY(0x0202D9D0, sinit_0202D9D0);

static void Comp_dt1(u32 self, s32 flags) {
    WWHD_FUNC(0x0202DA64, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202DA64, Comp_dt1);

static void SafeString_assureNone() {
    WWHD_FUNC(0x0202DA78, void);
}
VERIFY(0x0202DA78, SafeString_assureNone);

/* sead BufferedSafeString::assureTerminationImpl_ instance */
static void SafeString_assureTermination(u32 self) {
    WWHD_FUNC(0x0202DA7C, void, self);
    stb(ld(self + 0) + ld(self + 8) - 1, 0);
}
VERIFY(0x0202DA7C, SafeString_assureTermination);

static void Comp_dt2(u32 self, s32 flags) {
    WWHD_FUNC(0x0202DA94, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202DA94, Comp_dt2);

static void Comp_dt3(u32 self, s32 flags) {
    WWHD_FUNC(0x0202DAA8, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202DAA8, Comp_dt3);

static void Comp_dt4(u32 self, s32 flags) {
    WWHD_FUNC(0x0202DABC, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0202DABC, Comp_dt4);

}  // namespace hd_snd_0202C27C

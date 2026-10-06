/* hd_ui_02004A3C: the HD layout animation set (no GameCube source).
 *
 * TU 02004A3C..02005977 (__sinit 020058E4). A 0x18 object: layout +0, animation entries (SafeArray
 * {count +4, data +8}), active slots (SafeArray {count +0xC, data +0x10}), vtable 10000970 at +0x14.
 * Entries are the 0x3C animation entries of hd_ui_020032F4 (frame controller at +0x34). SafeArray
 * accesses with an out-of-range index use element 0 (sead SafeArray behaviour). Resource lookups run
 * under the UI lock 101F8B18 (0274FBF8 / 0274FCCC). The HD UI code calls these ~950 times.
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_ui_02004A3C {

static u32 AnimSet_rtti() {
    WWHD_FUNC(0x02004A3C, u32);
    return rtti_static3(0x101FD938, 0x101FD93C, 0x101FD88C, 0x101FD940, 0x101FD888, 0x101FD8FC);
}
VERIFY(0x02004A3C, AnimSet_rtti);

/* sead SafeArray element address: index out of range -> element 0 */
static inline u32 sa(u32 data, u32 count, u32 i) { return i < count ? data + i * 4 : data; }

static u32 AnimSet_ct(u32 self) {
    WWHD_FUNC(0x02004AC0, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x18);
        if (t == 0) return 0;
    }
    st(t + 0, 0);
    st(t + 0x14, 0x10000970);
    u32 a = t + 4;
    if (a == 0) a = op_new(8);
    if (a != 0) {
        st(a + 4, 0);
        st(a + 0, 0);
    }
    u32 b = t + 0xC;
    if (b == 0) {
        b = op_new(8);
        if (b == 0) return t;
    }
    st(b + 4, 0);
    st(b + 0, 0);
    return t;
}
VERIFY(0x02004AC0, AnimSet_ct);

static void AnimSet_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02004B5C, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02004B5C, AnimSet_dt);

static inline u32 heap_alloc(u32 heap, u32 size) {
    u32 h = heap;
    if (h == 0) h = gabi::call<u32>(0x02756140, ld(0x101F8B4C));
    return gabi::call_ptr<u32>(vfn(h, 0xC, 0x34), h, size, 4);
}

/* 02004B70: init: layout, NA animation entries and NB active slots, all cleared */
static u32 AnimSet_init(u32 self, u32 layout, s32 na, s32 nb) {
    WWHD_FUNC(0x02004B70, u32, self, layout, na, nb);
    st(self + 0, layout);
    u32 heap = ld(ld(0x1018C404) + 0x10);
    if (na > 0) {
        u32 p = heap_alloc(heap, (u32)na << 2);
        if (p != 0) {
            st(self + 8, p);
            st(self + 4, (u32)na);
        }
    }
    s32 m = (s32)ld(self + 4);
    for (s32 k = 0; k < m; ++k) st(ld(self + 8) + k * 4, 0);
    if (nb > 0) {
        u32 p = heap_alloc(heap, (u32)nb << 2);
        if (p != 0) {
            st(self + 0x10, p);
            st(self + 0xC, (u32)nb);
        }
    }
    m = (s32)ld(self + 0xC);
    for (s32 k = 0; k < m; ++k) st(ld(self + 0x10) + k * 4, 0);
    return 1;
}
VERIFY(0x02004B70, AnimSet_init);

/* 02004CAC: release: delete every entry, free both arrays */
static void AnimSet_fini(u32 self) {
    WWHD_FUNC(0x02004CAC, void, self);
    s32 n = (s32)ld(self + 4);
    u32 arr = ld(self + 8);
    for (s32 i = 0; i < n; ++i) {
        u32 off = (u32)i * 4;
        u32 e = (u32)i < (u32)n ? ld(arr + off) : ld(arr);
        if (e == 0) continue;
        u32 e2 = (u32)i < (u32)n ? ld(arr + off) : ld(arr);
        if (e2 != 0) {
            gabi::call_ptr(vfn(e2, 0x38, 0xC), e2, 3);
            n = (s32)ld(self + 4);
            arr = ld(self + 8);
        }
        st((u32)i < (u32)n ? arr + off : arr, 0);
        n = (s32)ld(self + 4);
        arr = ld(self + 8);
    }
    if (arr != 0) {
        u32 h = gabi::call<u32>(0x02755FEC, ld(0x101F8B4C), arr);
        gabi::call_ptr(vfn(h, 0xC, 0x3C), h, ld(self + 8));
        st(self + 4, 0);
        st(self + 8, 0);
    }
    u32 b = ld(self + 0x10);
    if (b != 0) {
        u32 h = gabi::call<u32>(0x02755FEC, ld(0x101F8B4C), b);
        gabi::call_ptr(vfn(h, 0xC, 0x3C), h, ld(self + 0x10));
        st(self + 0xC, 0);
        st(self + 0x10, 0);
    }
}
VERIFY(0x02004CAC, AnimSet_fini);

static inline u32 entry_at(u32 self, u32 i) { return sa(ld(self + 8), ld(self + 4), i); }
static inline u32 slot_at(u32 self, u32 i) { return sa(ld(self + 0x10), ld(self + 0xC), i); }

/* new entry (owner = layout) stored at index I; true when the slot is filled */
static inline u32 make_entry(u32 self, u32 i, u32 name, u32 ctrl) {
    u32 heap = ld(ld(0x1018C404) + 0x10);
    u32 slot = entry_at(self, i);
    u32 p = gabi::call<u32>(0x0273B050, 0x3C, heap, 4);
    if (p != 0) p = gabi::call<u32>(0x020034A0, p, ld(self + 0), name, ctrl);
    st(slot, p);
    return ld(entry_at(self, i)) != 0 ? 1 : 0;
}

/* 02004E04: bind entry IDX to the animation NAME of the pane group GROUP */
static u32 AnimSet_bindGroup(u32 self, u32 idx, u32 name, u32 group) {
    WWHD_FUNC(0x02004E04, u32, self, idx, name, group);
    if (ld(entry_at(self, idx)) != 0) return 0;
    gabi::call(0x0274FBF8, ld(0x101F8B18));
    u32 nvt = ld(name + 4);
    u32 lay = ld(self + 0);
    gabi::call_ptr(ld(nvt + 0x14), name);
    u32 gvt = ld(group + 4);
    u32 nm = ld(name + 0);
    gabi::call_ptr(ld(gvt + 0x14), group);
    u32 g = gabi::call<u32>(0x02874820, ld(lay + 0x10), ld(group + 0));
    u32 anim = gabi::call<u32>(0x0287F740, lay, nm, g, 0);
    gabi::call(0x0274FCCC, ld(0x101F8B18));
    return make_entry(self, idx, name, anim);
}
VERIFY(0x02004E04, AnimSet_bindGroup);

/* 02004F50: bind entry IDX to the animation NAME of the whole layout */
static u32 AnimSet_bind(u32 self, u32 idx, u32 name) {
    WWHD_FUNC(0x02004F50, u32, self, idx, name);
    if (ld(entry_at(self, idx)) != 0) return 0;
    gabi::call(0x0274FBF8, ld(0x101F8B18));
    gabi::call_ptr(ld(ld(name + 4) + 0x14), name);
    u32 anim = gabi::call<u32>(0x0287F944, ld(self + 0), ld(name + 0), 0);
    gabi::call(0x0274FCCC, ld(0x101F8B18));
    return make_entry(self, idx, name, anim);
}
VERIFY(0x02004F50, AnimSet_bind);

/* 0200506C: bind COUNT consecutive entries from BASE through the layout's virtual +0x94 */
static u32 AnimSet_bindRange(u32 self, u32 base, u32 name, u32 a6, u32 count) {
    WWHD_FUNC(0x0200506C, u32, self, base, name, a6, count);
    if (count == 0) return 1;
    for (u32 i = 0;; ++i) {
        if (ld(entry_at(self, base + i)) != 0) return 0;
        gabi::call(0x0274FBF8, ld(0x101F8B18));
        u32 lay = ld(self + 0);
        u32 anim = gabi::call_ptr<u32>(vfn(lay, 0x30, 0x94), lay, name, a6, i, 0);
        gabi::call(0x0274FCCC, ld(0x101F8B18));
        if (make_entry(self, base + i, name, anim) == 0) return 0;
        if (--count == 0) return 1;
    }
}
VERIFY(0x0200506C, AnimSet_bindRange);

/* 020051C4: put entry A into active slot B (stopping what plays there), then start it */
static void AnimSet_attach(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x020051C4, void, self, a, b);
    u32 n = ld(self + 4), arr = ld(self + 8);
    if (ld(sa(arr, n, a)) == 0) return;
    u32 m = ld(self + 0xC), act = ld(self + 0x10);
    if (ld(sa(act, m, b)) != 0 && lbz(ld(ld(sa(act, m, b)) + 0x34) + 0x10) != 0) {
        u32 o = ld(ld(sa(act, m, b)) + 0x34);
        gabi::call_ptr(vfn(o, 0x14, 0x24), o, 0);
        m = ld(self + 0xC);
        arr = ld(self + 8);
        act = ld(self + 0x10);
        n = ld(self + 4);
    }
    st(sa(act, m, b), ld(sa(arr, n, a)));
    u32 w = ld(slot_at(self, b));
    u32 o = ld(w + 0x34);
    gabi::call_ptr(vfn(o, 0x14, 0x24), o, 1);
}
VERIFY(0x020051C4, AnimSet_attach);

/* 02005334: stop and clear active slot B */
static void AnimSet_detach(u32 self, u32 b) {
    WWHD_FUNC(0x02005334, void, self, b);
    u32 m = ld(self + 0xC), act = ld(self + 0x10);
    if (ld(sa(act, m, b)) == 0) return;
    u32 o = ld(ld(sa(act, m, b)) + 0x34);
    gabi::call_ptr(vfn(o, 0x14, 0x24), o, 0);
    st(slot_at(self, b), 0);
}
VERIFY(0x02005334, AnimSet_detach);

/* 020053E4: attach entry A to slot B and start it at RATE (0200361C) */
static void AnimSet_play(u32 self, u32 a, u32 b, f32 rate) {
    WWHD_FUNC(0x020053E4, void, self, a, b, rate);
    if (ld(entry_at(self, a)) == 0) return;
    gabi::call(0x020051C4, self, a, b);
    gabi::call(0x0200361C, ld(slot_at(self, b)), rate);
}
VERIFY(0x020053E4, AnimSet_play);

/* 02005488: attach entry A to slot B and start it at RATE (02003654) */
static void AnimSet_play2(u32 self, u32 a, u32 b, f32 rate) {
    WWHD_FUNC(0x02005488, void, self, a, b, rate);
    if (ld(entry_at(self, a)) == 0) return;
    gabi::call(0x020051C4, self, a, b);
    gabi::call(0x02003654, ld(slot_at(self, b)), rate);
}
VERIFY(0x02005488, AnimSet_play2);

/* 0200552C: attach and call the controller's virtual +0xBC with a float (frame) */
static void AnimSet_setFrame(u32 self, u32 a, u32 b, f32 v) {
    WWHD_FUNC(0x0200552C, void, self, a, b, v);
    if (ld(entry_at(self, a)) == 0) return;
    gabi::call(0x020051C4, self, a, b);
    u32 o = ld(ld(slot_at(self, b)) + 0x34);
    gabi::call_ptr(vfn(o, 0x14, 0xBC), o, v);
}
VERIFY(0x0200552C, AnimSet_setFrame);

/* 020055E0: attach and call the controller's virtual +0xC4 */
static void AnimSet_vC4(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x020055E0, void, self, a, b);
    if (ld(entry_at(self, a)) == 0) return;
    gabi::call(0x020051C4, self, a, b);
    u32 o = ld(ld(slot_at(self, b)) + 0x34);
    gabi::call_ptr(vfn(o, 0x14, 0xC4), o);
}
VERIFY(0x020055E0, AnimSet_vC4);

/* 02005674: attach and call the controller's virtual +0xCC */
static void AnimSet_vCC(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x02005674, void, self, a, b);
    if (ld(entry_at(self, a)) == 0) return;
    gabi::call(0x020051C4, self, a, b);
    u32 o = ld(ld(slot_at(self, b)) + 0x34);
    gabi::call_ptr(vfn(o, 0x14, 0xCC), o);
}
VERIFY(0x02005674, AnimSet_vCC);

/* 02005708: attach and call the controller's virtual +0xD4 */
static void AnimSet_vD4(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x02005708, void, self, a, b);
    if (ld(entry_at(self, a)) == 0) return;
    gabi::call(0x020051C4, self, a, b);
    u32 o = ld(ld(slot_at(self, b)) + 0x34);
    gabi::call_ptr(vfn(o, 0x14, 0xD4), o);
}
VERIFY(0x02005708, AnimSet_vD4);

/* 0200579C: attach and set the controller's speed (+0xC) */
static void AnimSet_setSpeed(u32 self, u32 a, u32 b, f32 v) {
    WWHD_FUNC(0x0200579C, void, self, a, b, v);
    if (ld(entry_at(self, a)) == 0) return;
    gabi::call(0x020051C4, self, a, b);
    stf(ld(ld(slot_at(self, b)) + 0x34) + 0xC, v);
}
VERIFY(0x0200579C, AnimSet_setSpeed);

/* 02005840: entry A finished (true when there is no entry) */
static u32 AnimSet_isEnd(u32 self, u32 a) {
    WWHD_FUNC(0x02005840, u32, self, a);
    u32 e = ld(entry_at(self, a));
    if (e == 0) return 1;
    return gabi::call<u32>(0x0200368C, e);
}
VERIFY(0x02005840, AnimSet_isEnd);

/* 0200588C: entry A not playing (bit 0 of the controller's +0x2C; true when there is no entry) */
static u32 AnimSet_isStopped(u32 self, u32 a) {
    WWHD_FUNC(0x0200588C, u32, self, a);
    u32 e = ld(entry_at(self, a));
    if (e == 0) return 1;
    return ld(ld(e + 0x34) + 0x2C) & 1;
}
VERIFY(0x0200588C, AnimSet_isStopped);

static void sinit_020058E4() {
    WWHD_FUNC(0x020058E4, void);
    header_sinit(0x101FF29C, 0x1018C398, 0x10000968);
}
VERIFY(0x020058E4, sinit_020058E4);

}  // namespace hd_ui_02004A3C

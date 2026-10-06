/* hd_ui_02000020: HD UI input-event receiver (cking::ui, no GameCube source).
 *
 * TU 02000020..02001083 (first TU of .text; __sinit 02000FF0). A 0x5C object: a list header at +0
 * (next +4, count +8), the focused item id +0xC, a suspend counter +0x10, sub-objects at +0x14
 * (02001CB0, event queue) and +0x30 (020012CC), its vtable pointer at +0x58 (vtable 10000010).
 * Events (the 0x1A4-byte input state of the HD input code): trigger words +0 / +4 / +8, an
 * option word +0xC (bit 0: the pointer position at +0x19C is valid), hold bits +0x10C; the
 * fallback pointer position is the global 1049FFA0. List items are pointed to by node+8 and have
 * their own vtable pointer at +0x14 and an id at +0x10. */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_ui_02000020 {

struct ev_l { u8 b[0x28]; };       /* the event record built by 02001338 (0x28 bytes) */
struct pt_l { u8 b[0xC]; };        /* {id, x, y} handed to the hit tests */

static inline u32 vf(u32 self, u32 off) { return vfn(self, 0x58, off); }
static inline u32 ivf(u32 item, u32 off) { return vfn(item, 0x14, off); }

/* 02000020: constructor (allocates 0x5C when this is null) */
static u32 Receiver_ct(u32 self) {
    WWHD_FUNC(0x02000020, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x5C);
        if (t == 0) return 0;
    }
    st(t + 0x58, 0x10000010);
    /* inline list-header constructor (its null branch is dead: t is never null here) */
    st(t + 4, t);
    st(t + 8, 0);
    st(t + 0, t);
    st(t + 0xC, 0);
    st(t + 0x10, 0);
    gabi::call(0x02001CB0, t + 0x14);
    gabi::call(0x020012CC, t + 0x30);
    return t;
}
VERIFY(0x02000020, Receiver_ct);

/* 020000B4: destructor */
static void Receiver_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x020000B4, void, self, flags);
    if (self == 0) return;
    gabi::call(0x02001D38, self + 0x14, 2);
    if (flags & 1) op_delete(self);
}
VERIFY(0x020000B4, Receiver_dt);

/* 02000108 / 02000110: forwards to the event queue at +0x14 */
static u32 Receiver_queue1(u32 self) {
    WWHD_FUNC(0x02000108, u32, self);
    return gabi::call<u32>(0x02001D4C, self + 0x14);
}
VERIFY(0x02000108, Receiver_queue1);

static u32 Receiver_queue2(u32 self) {
    WWHD_FUNC(0x02000110, u32, self);
    return gabi::call<u32>(0x02001E18, self + 0x14);
}
VERIFY(0x02000110, Receiver_queue2);

/* unlink every item whose +0xC flag is set (0273B310 erases the node) */
static inline void sweep(u32 self) {
    u32 n = ld(self + 4);
    while (n != self) {
        u32 cur = n;
        u32 f = ld(cur + 0xC);
        n = ld(cur + 4);
        if (f != 0) {
            st(cur + 0xC, 0);
            gabi::call(0x0273B310, cur);
            st(self + 8, ld(self + 8) - 1);
        }
    }
}

/* 02000118: update: sweeps the list; with a current input state (global 1018C12C -> +0) it first
 * counts the suspend counter down and calls the virtual update (+0x24) */
static void Receiver_update(u32 self) {
    WWHD_FUNC(0x02000118, void, self);
    u32 g = ld(0x1018C12C);
    if (g == 0) { sweep(self); return; }
    u32 st_ = ld(g);
    if (st_ == 0) { sweep(self); return; }
    s32 c = (s32)ld(self + 0x10);
    if (c > 0) {
        st(self + 0x10, (u32)(c - 1));
        sweep(self);
    }
    gabi::call_ptr(vf(self, 0x24), self, st_);
    sweep(self);
}
VERIFY(0x02000118, Receiver_update);

/* 020002E0: flush: 02001480 on +0x30, then the queue takes it (02001E80) and dispatches (0200209C) */
static void Receiver_flush(u32 self) {
    WWHD_FUNC(0x020002E0, void, self);
    gabi::call(0x02001480, self + 0x30);
    gabi::call(0x02001E80, self + 0x14, self + 0x30);
    gabi::call(0x0200209C, self + 0x14);
}
VERIFY(0x020002E0, Receiver_flush);

/* 02000324: post an event (kind, id, arg) to the queue */
static void Receiver_post(u32 self, s32 kind, s32 id, u32 arg) {
    WWHD_FUNC(0x02000324, void, self, kind, id, arg);
    gabi::Local<ev_l> ev;
    gabi::call(0x02001338, ev.a, kind, id, arg);
    gabi::call(0x02001FF4, self + 0x14, ev.a);
}
VERIFY(0x02000324, Receiver_post);

static inline u32 pos_src(u32 ev) {
    return (ld(ev + 0xC) & 1) ? ev + 0x19C : 0x1049FFA0;
}

/* 02000360: dispatch one input state to the virtual handlers */
static s32 Receiver_dispatch(u32 self, u32 ev) {
    WWHD_FUNC(0x02000360, s32, self, ev);
    if (ev == 0) return 1;
    u32 p = pos_src(ev);
    f32 lim = ldf(0x10000000);
    f32 x = ldf(p);
    f32 y = ldf(p + 4);
    if (x > lim && y > lim) { /* fcmpu+ble: the recompiler takes ble on NaN */
        if (ld(ev + 0) & 0x8000) { gabi::call_ptr(vf(self, 0x2C), self, ev); return 1; }
        if (ld(ev + 0x10C) & 0x8000) { gabi::call_ptr(vf(self, 0x34), self, ev); return 1; }
    } else {
        if (ld(ev + 4) & 0x8000) { gabi::call_ptr(vf(self, 0x3C), self, ev); return 1; }
        st(self + 0xC, 0);
    }
    if (gabi::call_ptr<u32>(vf(self, 0xA4), self, ev) != 0) {
        if (gabi::call_ptr<u32>(vf(self, 0xB4), self, ev) != 0) { gabi::call_ptr(vf(self, 0x64), self); return 1; }
        if (gabi::call_ptr<u32>(vf(self, 0xBC), self, ev) != 0) { gabi::call_ptr(vf(self, 0x6C), self); return 1; }
        gabi::call_ptr(vf(self, 0x44), self);
        return 1;
    }
    if (gabi::call_ptr<u32>(vf(self, 0xAC), self, ev) != 0) {
        if (gabi::call_ptr<u32>(vf(self, 0xB4), self, ev) != 0) { gabi::call_ptr(vf(self, 0x74), self); return 1; }
        if (gabi::call_ptr<u32>(vf(self, 0xBC), self, ev) != 0) { gabi::call_ptr(vf(self, 0x7C), self); return 1; }
        gabi::call_ptr(vf(self, 0x4C), self);
        return 1;
    }
    if (gabi::call_ptr<u32>(vf(self, 0xB4), self, ev) != 0) { gabi::call_ptr(vf(self, 0x54), self); return 1; }
    if (gabi::call_ptr<u32>(vf(self, 0xBC), self, ev) != 0) { gabi::call_ptr(vf(self, 0x5C), self); return 1; }
    if (gabi::call_ptr<u32>(vf(self, 0xEC), self, ev) != 0) { gabi::call_ptr(vf(self, 0x8C), self); return 1; }
    if (gabi::call_ptr<u32>(vf(self, 0x13C), self, ev) != 0) { gabi::call_ptr(vf(self, 0x9C), self); return 1; }
    if (gabi::call_ptr<u32>(vf(self, 0xC4), self, ev) != 0) { gabi::call_ptr(vf(self, 0x84), self); return 1; }
    if (gabi::call_ptr<u32>(vf(self, 0x114), self, ev) != 0) { gabi::call_ptr(vf(self, 0x94), self); return 1; }
    return 0;
}
VERIFY(0x02000360, Receiver_dispatch);

/* hit test of one list item against the point {id, x, y} */
static inline bool hit(u32 self, u32 item, u32 q) {
    if (gabi::call_ptr<u32>(vf(self, 0x144), self, q) != 0) {
        if (gabi::call_ptr<u32>(ivf(item, 0x24), item) == 0) return false;
    }
    if (gabi::call_ptr<u32>(vf(self, 0x14C), self, q) != 0) {
        if (gabi::call_ptr<u32>(ivf(item, 0x1C), item) == 0) return false;
    }
    f32 x = ldf(q + 4), y = ldf(q + 8);
    return gabi::call_ptr<u32>(ivf(item, 0x14), item, x, y) != 0;
}

/* 02000870: touch on: find the item under the pointer, post kind 2 and focus it */
static s32 Receiver_touchOn(u32 self, u32 ev) {
    WWHD_FUNC(0x02000870, s32, self, ev);
    if (ev == 0) { st(self + 0xC, 0); return 0; }
    gabi::Local<pt_l> q;
    u32 id = gabi::call<u32>(0x0200190C, ev);
    u32 p = pos_src(ev);
    st(q.a, id);
    st(q.a + 4, ld(p));
    st(q.a + 8, ld(p + 4));
    for (u32 n = ld(self + 4); n != self; n = ld(n + 4)) {
        u32 item = ld(n + 8);
        if (hit(self, item, q.a)) {
            gabi::call(0x02000324, self, 2, ld(item + 0x10), 0);
            st(self + 0xC, ld(item + 0x10));
            return 1;
        }
    }
    return 0;
}
VERIFY(0x02000870, Receiver_touchOn);

/* 02000A04: touch hold: post kind 3 for the item under the pointer */
static s32 Receiver_touchHold(u32 self, u32 ev) {
    WWHD_FUNC(0x02000A04, s32, self, ev);
    if (ev == 0) { st(self + 0xC, 0); return 0; }
    gabi::Local<pt_l> q;
    u32 id = gabi::call<u32>(0x0200190C, ev);
    u32 p = pos_src(ev);
    st(q.a, id);
    st(q.a + 4, ld(p));
    st(q.a + 8, ld(p + 4));
    for (u32 n = ld(self + 4); n != self; n = ld(n + 4)) {
        u32 item = ld(n + 8);
        if (hit(self, item, q.a)) {
            gabi::call(0x02000324, self, 3, ld(item + 0x10), 0);
            return 1;
        }
    }
    return 0;
}
VERIFY(0x02000A04, Receiver_touchHold);

/* 02000B8C: touch off: post kind 4 when released over the focused item; clears the focus */
static s32 Receiver_touchOff(u32 self, u32 ev) {
    WWHD_FUNC(0x02000B8C, s32, self, ev);
    if (ev != 0) {
        gabi::Local<pt_l> q;
        u32 id = gabi::call<u32>(0x0200190C, ev);
        u32 n = ld(self + 4);
        st(q.a + 4, ld(ev + 0x19C));
        st(q.a, id);
        st(q.a + 8, ld(ev + 0x1A0));
        for (; n != self; n = ld(n + 4)) {
            u32 item = ld(n + 8);
            if (hit(self, item, q.a)) {
                u32 iid = ld(item + 0x10);
                if (ld(self + 0xC) == iid) gabi::call(0x02000324, self, 4, iid, 0);
                st(self + 0xC, 0);
                return 1;
            }
        }
    }
    st(self + 0xC, 0);
    return 0;
}
VERIFY(0x02000B8C, Receiver_touchOff);

/* 02000D00..02000F10: virtual button handlers: post their event kind with id -1 */
static s32 Receiver_on05(u32 self) {
    WWHD_FUNC(0x02000D00, s32, self);
    gabi::call(0x02000324, self, 5, -1, 0);
    return 0;
}
VERIFY(0x02000D00, Receiver_on05);
static s32 Receiver_on08(u32 self) {
    WWHD_FUNC(0x02000D30, s32, self);
    gabi::call(0x02000324, self, 8, -1, 0);
    return 0;
}
VERIFY(0x02000D30, Receiver_on08);
static s32 Receiver_on0B(u32 self) {
    WWHD_FUNC(0x02000D60, s32, self);
    gabi::call(0x02000324, self, 0xB, -1, 0);
    return 0;
}
VERIFY(0x02000D60, Receiver_on0B);
static s32 Receiver_on0C(u32 self) {
    WWHD_FUNC(0x02000D90, s32, self);
    gabi::call(0x02000324, self, 0xC, -1, 0);
    return 0;
}
VERIFY(0x02000D90, Receiver_on0C);
static s32 Receiver_on06(u32 self) {
    WWHD_FUNC(0x02000DC0, s32, self);
    gabi::call(0x02000324, self, 6, -1, 0);
    return 0;
}
VERIFY(0x02000DC0, Receiver_on06);
static s32 Receiver_on07(u32 self) {
    WWHD_FUNC(0x02000DF0, s32, self);
    gabi::call(0x02000324, self, 7, -1, 0);
    return 0;
}
VERIFY(0x02000DF0, Receiver_on07);
static s32 Receiver_on09(u32 self) {
    WWHD_FUNC(0x02000E20, s32, self);
    gabi::call(0x02000324, self, 9, -1, 0);
    return 0;
}
VERIFY(0x02000E20, Receiver_on09);
static s32 Receiver_on0A(u32 self) {
    WWHD_FUNC(0x02000E50, s32, self);
    gabi::call(0x02000324, self, 0xA, -1, 0);
    return 0;
}
VERIFY(0x02000E50, Receiver_on0A);
static s32 Receiver_on0F(u32 self) {
    WWHD_FUNC(0x02000E80, s32, self);
    gabi::call(0x02000324, self, 0xF, -1, 0);
    return 0;
}
VERIFY(0x02000E80, Receiver_on0F);
static s32 Receiver_on0D(u32 self) {
    WWHD_FUNC(0x02000EB0, s32, self);
    gabi::call(0x02000324, self, 0xD, -1, 0);
    return 0;
}
VERIFY(0x02000EB0, Receiver_on0D);
static s32 Receiver_on10(u32 self) {
    WWHD_FUNC(0x02000EE0, s32, self);
    gabi::call(0x02000324, self, 0x10, -1, 0);
    return 0;
}
VERIFY(0x02000EE0, Receiver_on10);
static s32 Receiver_on0E(u32 self) {
    WWHD_FUNC(0x02000F10, s32, self);
    gabi::call(0x02000324, self, 0xE, -1, 0);
    return 0;
}
VERIFY(0x02000F10, Receiver_on0E);

/* 02000F40..02000FCC: virtual predicates on the input state (r4) */
static u32 Receiver_hold16(u32 self, u32 ev) {
    WWHD_FUNC(0x02000F40, u32, self, ev);
    return (ld(ev + 0x10C) >> 16) & 1;
}
VERIFY(0x02000F40, Receiver_hold16);
static u32 Receiver_hold17(u32 self, u32 ev) {
    WWHD_FUNC(0x02000F4C, u32, self, ev);
    return (ld(ev + 0x10C) >> 17) & 1;
}
VERIFY(0x02000F4C, Receiver_hold17);
static u32 Receiver_hold18(u32 self, u32 ev) {
    WWHD_FUNC(0x02000F58, u32, self, ev);
    return (ld(ev + 0x10C) >> 18) & 1;
}
VERIFY(0x02000F58, Receiver_hold18);
static u32 Receiver_hold19(u32 self, u32 ev) {
    WWHD_FUNC(0x02000F64, u32, self, ev);
    return (ld(ev + 0x10C) >> 19) & 1;
}
VERIFY(0x02000F64, Receiver_hold19);
static u32 Receiver_hold0(u32 self, u32 ev) {
    WWHD_FUNC(0x02000F70, u32, self, ev);
    return (ld(ev + 0x10C) >> 0) & 1;
}
VERIFY(0x02000F70, Receiver_hold0);
static u32 Receiver_trig0(u32 self, u32 ev) {
    WWHD_FUNC(0x02000F7C, u32, self, ev);
    return (ld(ev + 0) >> 0) & 1;
}
VERIFY(0x02000F7C, Receiver_trig0);
static u32 Receiver_trig1(u32 self, u32 ev) {
    WWHD_FUNC(0x02000F88, u32, self, ev);
    return (ld(ev + 4) >> 0) & 1;
}
VERIFY(0x02000F88, Receiver_trig1);
static u32 Receiver_trig2(u32 self, u32 ev) {
    WWHD_FUNC(0x02000F94, u32, self, ev);
    return (ld(ev + 8) >> 0) & 1;
}
VERIFY(0x02000F94, Receiver_trig2);

/* 02000FA0: state kind 1, 2 or 8 */
static u32 Receiver_isKindA(u32 self, u32 ev) {
    WWHD_FUNC(0x02000FA0, u32, self, ev);
    u32 k = ld(ev);
    if (k >= 1 && (k <= 2 || k == 8)) return 1;
    return 0;
}
VERIFY(0x02000FA0, Receiver_isKindA);

/* 02000FCC: state kind 4 or 7 */
static u32 Receiver_isKindB(u32 self, u32 ev) {
    WWHD_FUNC(0x02000FCC, u32, self, ev);
    u32 k = ld(ev);
    return (k == 4 || k == 7) ? 1 : 0;
}
VERIFY(0x02000FCC, Receiver_isKindB);

/* 02000FF0: __sinit (header statics) */
static void sinit() {
    WWHD_FUNC(0x02000FF0, void);
    header_sinit(0x101FF06C, 0x1018C0C0, 0x10000004);
}
VERIFY(0x02000FF0, sinit);

}  // namespace hd_ui_02000020

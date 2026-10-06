/* hd_ui_020015C8: four HD UI TUs (no GameCube source). 
 *
 * 020015C8..02001727: UI widget base (0x18: list node +0..+0xC, serial number +0x10 from the counter
 *                     1018C178 (never 0), vtable 100001B0 at +0x14); __sinit 0200167C; three constant
 *                     virtuals 02001710 (0) / 02001718 (1) / 02001720 (1)
 * 02001728..020019FF: GamePad pointer (0x1B4, derived from the sead-side base 0273CDD4; vtables 10000220
 *                     at +0x170 and 10000258 at +0x13C): integer cursor +0x194/+0x198, float cursor
 *                     +0x19C/+0x1A0, centre +0x1A4/+0x1A8, scale +0x1AC/+0x1B0; __sinit 0200196C
 * 02001A00..02001CAF: UI touch widget (0x58 = widget base + layout pane +0x18, state +0x1C, eight user
 *                     words +0x38); __sinit 02001C1C
 * 02001CB0..020021FB: UI event queue (0x1C: heap +0, list header +4 (count +0xC), event-box array +0x10
 *                     with its count +0x14, vtable 100002B0 at +0x18); __sinit 02002168 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_ui_020015C8 {

/* 020015C8: widget base constructor */
static u32 Widget_ct(u32 self) {
    WWHD_FUNC(0x020015C8, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x18);
        if (t == 0) return 0;
    }
    /* inline list-node constructor (its null branch is dead) */
    st(t + 8, t);
    st(t + 4, 0);
    st(t + 0, 0);
    st(t + 0xC, 0);
    st(t + 0x14, 0x100001B0);
    u32 serial = ld(0x1018C178);
    st(0x1018C178, serial + 1);
    st(t + 0x10, serial);
    if (ld(0x1018C178) == 0) st(0x1018C178, 1);
    return t;
}
VERIFY(0x020015C8, Widget_ct);

static void Widget_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02001668, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02001668, Widget_dt);

static void sinit_0200167C() {
    WWHD_FUNC(0x0200167C, void);
    header_sinit(0x101FF0F8, 0x1018C17C, 0x100001A8);
}
VERIFY(0x0200167C, sinit_0200167C);

static u32 Widget_false() {
    WWHD_FUNC(0x02001710, u32);
    return 0;
}
VERIFY(0x02001710, Widget_false);

static u32 Widget_true1() {
    WWHD_FUNC(0x02001718, u32);
    return 1;
}
VERIFY(0x02001718, Widget_true1);

static u32 Widget_true2() {
    WWHD_FUNC(0x02001720, u32);
    return 1;
}
VERIFY(0x02001720, Widget_true2);

/* 02001728: pointer destructor */
static void Pointer_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02001728, void, self, flags);
    if (self == 0) return;
    st(self + 0x13C, 0x101432B0);
    gabi::call(0x0273D0E0, self, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x02001728, Pointer_dt);

/* 02001788: destructor thunk for the secondary base at +0x130 */
static void Pointer_dtThunk(u32 self, s32 flags) {
    WWHD_FUNC(0x02001788, void, self, flags);
    gabi::call(0x02001728, self - 0x130, flags);
}
VERIFY(0x02001788, Pointer_dtThunk);

/* 02001790: pointer constructor */
static u32 Pointer_ct(u32 self) {
    WWHD_FUNC(0x02001790, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x1B4);
        if (t == 0) return 0;
    }
    gabi::call(0x0273CDD4, t);
    st(t + 0x170, 0x10000220);
    st(t + 0x13C, 0x10000258);
    st(t + 0x194, ld(0x1049FFA8));
    st(t + 0x198, ld(0x1049FFAC));
    u32 v = t + 0x19C;
    if (v == 0) v = op_new(8);
    if (v != 0) {
        s32 x = (s32)ld(0x1049FFA8), y = (s32)ld(0x1049FFAC);
        stf(v + 0, (f32)(f64)x);
        stf(v + 4, (f32)(f64)y);
    }
    f32 zero = ldf(0x10000210), one = ldf(0x10000200);
    stf(t + 0x1A4, zero);
    stf(t + 0x1A8, zero);
    stf(t + 0x1AC, one);
    stf(t + 0x1B0, one);
    return t;
}
VERIFY(0x02001790, Pointer_ct);

/* 0200188C: update: base update, then map the raw position into the cursor */
static void Pointer_update(u32 self) {
    WWHD_FUNC(0x0200188C, void, self);
    gabi::call(0x0273CE94, self);
    f32 cy = ldf(self + 0x1A8), ry = ldf(self + 0x114), cx = ldf(self + 0x1A4);
    f32 dy = gabi::fsubs_ppc(ry, cy);
    f32 rx = ldf(self + 0x110), sy = ldf(self + 0x1B0);
    f32 dx = gabi::fsubs_ppc(rx, cx);
    f32 sx = ldf(self + 0x1AC);
    f32 y = gabi::fmuls_ppc(dy, sy);
    f32 x = gabi::fmuls_ppc(dx, sx);
    stf(self + 0x19C, x);
    stf(self + 0x1A0, y);
    st(self + 0x198, (u32)gabi::ftoi(y));
    st(self + 0x194, (u32)gabi::ftoi(x));
}
VERIFY(0x0200188C, Pointer_update);

/* 0200190C: id of the pointer that produced an input state (+0x140 -> +0x130) */
static u32 Input_pointerId(u32 ev) {
    WWHD_FUNC(0x0200190C, u32, ev);
    u32 p = ld(ev + 0x140);
    if (p == 0) return 0;
    return ld(p + 0x130);
}
VERIFY(0x0200190C, Input_pointerId);

/* 02001924: set the screen size (centre = half size) and, for a non-zero target size, the scale */
static void Pointer_setScreen(u32 self, f32 w, f32 h, f32 tw, f32 th) {
    WWHD_FUNC(0x02001924, void, self, w, h, tw, th);
    f32 half = ldf(0x10000214);
    f32 zero = ldf(0x10000210);
    stf(self + 0x1A4, w * half);
    stf(self + 0x1A8, h * half);
    if (tw == zero) return;
    if (th == zero) return;
    stf(self + 0x1B0, -fdivs_ppc(th, h));
    stf(self + 0x1AC, fdivs_ppc(tw, w));
}
VERIFY(0x02001924, Pointer_setScreen);

static void sinit_0200196C() {
    WWHD_FUNC(0x0200196C, void);
    header_sinit(0x101FF114, 0x1018C1A0, 0x10000218);
}
VERIFY(0x0200196C, sinit_0200196C);

/* 02001A00: touch widget constructor */
static u32 TouchWidget_ct(u32 self, u32 pane) {
    WWHD_FUNC(0x02001A00, u32, self, pane);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x58);
        if (t == 0) return 0;
    }
    gabi::call(0x020015C8, t);
    st(t + 0x18, pane);
    st(t + 0x2C, 0);
    st(t + 0x1C, 1);
    sth(t + 0x22, 0);
    st(t + 0x14, 0x10000270);
    sth(t + 0x20, 0);
    sth(t + 0x28, 0);
    sth(t + 0x2A, 0);
    st(t + 0x34, 0);
    st(t + 0x24, 0);
    sth(t + 0x32, 0);
    sth(t + 0x30, 0);
    if (t + 0x38 == 0) op_new(0x20); /* inline array constructor's null check (result unused) */
    for (u32 i = 0; i < 8; ++i) st(t + 0x38 + i * 4, 0);
    return t;
}
VERIFY(0x02001A00, TouchWidget_ct);

/* 02001ABC: touch widget destructor */
static void TouchWidget_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02001ABC, void, self, flags);
    if (self == 0) return;
    gabi::call(0x02001668, self, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x02001ABC, TouchWidget_dt);

struct mtx_l { u8 b[0x14]; };
struct vec2_l { u8 b[8]; };

/* 02001B10: hit test of the pane against a point (x, y) */
static u32 TouchWidget_hit(u32 self, f32 x, f32 y) {
    WWHD_FUNC(0x02001B10, u32, self, x, y);
    u32 pane = ld(self + 0x18);
    if (pane == 0) return 0;
    gabi::Local<mtx_l> m;
    gabi::call(0x02876368, pane, m.a);
    gabi::Local<vec2_l> p;
    stf(p.a + 4, y);
    u32 pane2 = ld(self + 0x18);
    stf(p.a + 0, x);
    return gabi::call<u32>(0x0287F05C, pane2, p.a);
}
VERIFY(0x02001B10, TouchWidget_hit);

static u32 TouchWidget_isState0(u32 self) {
    WWHD_FUNC(0x02001BC0, u32, self);
    return ld(self + 0x1C) == 0 ? 1 : 0;
}
VERIFY(0x02001BC0, TouchWidget_isState0);

static u32 TouchWidget_isState1(u32 self) {
    WWHD_FUNC(0x02001BD0, u32, self);
    return ld(self + 0x1C) == 1 ? 1 : 0;
}
VERIFY(0x02001BD0, TouchWidget_isState1);

/* 02001BE4 / 02001C00: user words (index >= 8 uses word 0) */
static void TouchWidget_setUser(u32 self, u32 i, u32 v) {
    WWHD_FUNC(0x02001BE4, void, self, i, v);
    u32 a = self + 0x38;
    if (i < 8) a += i * 4;
    st(a, v);
}
VERIFY(0x02001BE4, TouchWidget_setUser);

static u32 TouchWidget_getUser(u32 self, u32 i) {
    WWHD_FUNC(0x02001C00, u32, self, i);
    u32 a = self + 0x38;
    if (i < 8) a += i * 4;
    return ld(a);
}
VERIFY(0x02001C00, TouchWidget_getUser);

static void sinit_02001C1C() {
    WWHD_FUNC(0x02001C1C, void);
    header_sinit(0x101FF130, 0x1018C1C4, 0x10000268);
}
VERIFY(0x02001C1C, sinit_02001C1C);

/* 02001CB0: event queue constructor */
static u32 Queue_ct(u32 self) {
    WWHD_FUNC(0x02001CB0, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x1C);
        if (t == 0) return 0;
    }
    st(t + 0, 0);
    st(t + 0x18, 0x100002B0);
    u32 h = t + 4;
    if (h == 0) h = op_new(0xC);
    if (h != 0) {
        st(h + 4, h);
        st(h + 8, 0);
        st(h + 0, h);
    }
    st(t + 0x14, 0);
    st(t + 0x10, 0);
    return t;
}
VERIFY(0x02001CB0, Queue_ct);

static void Queue_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02001D38, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02001D38, Queue_dt);

/* 02001D4C: allocate COUNT event boxes from HEAP (array new with cookie) */
static s32 Queue_init(u32 self, u32 heap, u32 count) {
    WWHD_FUNC(0x02001D4C, s32, self, heap, count);
    st(self + 0, heap);
    if (heap == 0) return 0;
    st(self + 0x14, count);
    if (count == 0) return 0;
    u32 cookie = ld(0x101FCD08);
    u32 p = gabi::call<u32>(0x0273B0D4, count * 0x38 + cookie, ld(self + 0), 4);
    if (p == 0) {
        st(self + 0x10, 0);
        return 0;
    }
    u32 arr = gabi::call<u32>(0x028EFF8C, p + ld(0x101FCD08), count, 0x38, 0x02001084, 0);
    st(self + 0x10, arr);
    return arr != 0 ? 1 : 0;
}
VERIFY(0x02001D4C, Queue_init);

/* 02001E18: release the event boxes */
static void Queue_fini(u32 self) {
    WWHD_FUNC(0x02001E18, void, self);
    u32 arr = ld(self + 0x10);
    if (arr != 0) {
        gabi::call(0x028F0164, arr, -1, 0x38, 0x02001110, 1, 0);
        st(self + 0x10, 0);
    }
    st(self + 0x14, 0);
    st(self + 0, 0);
}
VERIFY(0x02001E18, Queue_fini);

/* 02001E80: pop the first queued event into OUT */
static s32 Queue_pop(u32 self, u32 out) {
    WWHD_FUNC(0x02001E80, s32, self, out);
    u32 n = gabi::call<u32>(0x0273B494, self + 4);
    if (n != 0) st(n + 0x30, 0);
    if (n == 0) return 0;
    gabi::call(0x0200143C, out, n + 8);
    if (ld(n + 0x30) != 0) {
        st(n + 0x30, 0);
        gabi::call(0x0273B310, n);
        st(self + 0xC, ld(self + 0xC) - 1);
    }
    gabi::call(0x02001480, n + 8);
    return 1;
}
VERIFY(0x02001E80, Queue_pop);

/* 02001F44: (re)link an event box at the end of the queue list */
static void Queue_link(u32 self, u32 box) {
    WWHD_FUNC(0x02001F44, void, self, box);
    u32 list = self + 4;
    if (ld(box + 0x30) != 0) {
        st(box + 0x30, 0);
        gabi::call(0x0273B310, box);
        st(self + 0xC, ld(self + 0xC) - 1);
        u32 owner = ld(box + 0x30);
        if (owner != 0) {
            st(box + 0x30, 0);
            gabi::call(0x0273B310, box);
            st(owner + 8, ld(owner + 8) - 1);
        }
    }
    st(box + 0x30, list);
    gabi::call(0x0273B2F0, list, box);
    st(list + 8, ld(list + 8) + 1);
}
VERIFY(0x02001F44, Queue_link);

/* 02001FF4: post: copy EV into the first free box and queue it */
static s32 Queue_post(u32 self, u32 ev) {
    WWHD_FUNC(0x02001FF4, s32, self, ev);
    u32 arr = ld(self + 0x10);
    if (arr == 0) return 0;
    u32 cnt = ld(self + 0x14);
    for (u32 i = 0, off = 0; i < cnt; ++i, off += 0x38) {
        u32 e = arr + off;
        if (ld(e + 0xC) == 0) {
            gabi::call(0x0200143C, e + 8, ev);
            gabi::call(0x02001F44, self, ld(self + 0x10) + off);
            return 1;
        }
    }
    return 0;
}
VERIFY(0x02001FF4, Queue_post);

/* 0200209C: drop all queued events and clear every box */
static void Queue_clear(u32 self) {
    WWHD_FUNC(0x0200209C, void, self);
    u32 list = self + 4;
    u32 n = ld(self + 8);
    while (n != list) {
        u32 cur = n;
        u32 f = ld(cur + 0x30);
        n = ld(cur + 4);
        if (f != 0) {
            st(cur + 0x30, 0);
            gabi::call(0x0273B310, cur);
            st(list + 8, ld(list + 8) - 1);
        }
    }
    u32 arr = ld(self + 0x10);
    if (arr == 0) return;
    u32 i = 0;
    if (i >= ld(self + 0x14)) return;
    for (u32 off = 0;; ) {
        gabi::call(0x02001480, arr + off + 8);
        off += 0x38;
        ++i;
        if (i >= ld(self + 0x14)) break;
        arr = ld(self + 0x10);
    }
}
VERIFY(0x0200209C, Queue_clear);

static void sinit_02002168() {
    WWHD_FUNC(0x02002168, void);
    header_sinit(0x101FF14C, 0x1018C1E8, 0x100002A8);
}
VERIFY(0x02002168, sinit_02002168);

}  // namespace hd_ui_020015C8

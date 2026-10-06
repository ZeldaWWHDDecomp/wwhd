/* hd_ui_020032F4: five HD UI TUs (no GameCube source).
 *
 * 020032F4..0200349F: an 8-byte list visitor (vtable 10000560): visit every element of a pointer array
 *                     {count +0, data +8} through virtual +0x1C; __sinit 02003404
 * 020034A0..020037CF: a named animation entry (0x3C: owner +0, rate +4, FixedSafeString<32> name +8,
 *                     frame controller +0x34, vtable 100005F8 at +0x38) and its play/stop/isStopped helpers;
 *                     __sinit 020036D0 (+ deleting-dtor / SafeString companions)
 * 020037D0..0200393B: a 0xB0 NW4F-lyt-derived object (02874894 / 02874918, vtable 10000610); __sinit 020038A8
 * 0200393C..02003AEB: a lyt adaptor (8 bytes, vtable 100006C8, target +4) with its sead RTTI static;
 *                     __sinit 02003A44
 * 02003AEC..02003C57: a 0xD8 NW4F-lyt-derived object (02878F9C / 02879474, vtable 100006F8); __sinit 02003BC4
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_ui_020032F4 {

static u32 Visitor_ct(u32 self) {
    WWHD_FUNC(0x020032F4, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(8);
        if (t == 0) return 0;
    }
    st(t + 0, 0);
    st(t + 4, 0x10000560);
    return t;
}
VERIFY(0x020032F4, Visitor_ct);

static void Visitor_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x0200333C, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0200333C, Visitor_dt);

/* 02003350: visit all elements of LIST {count +0, data +8}; stops (false) at the first refusal */
static u32 Visitor_visitAll(u32 self, u32 list) {
    WWHD_FUNC(0x02003350, u32, self, list);
    u32 p = ld(list + 8);
    u32 end = p + ld(list + 0) * 4;
    while (p != end) {
        u32 fn = vfn(self, 4, 0x1C);
        if (gabi::call_ptr<u32>(fn, self, ld(p)) == 0) return 0;
        p += 4;
        end = ld(list + 8) + ld(list + 0) * 4;
    }
    return 1;
}
VERIFY(0x02003350, Visitor_visitAll);

static void sinit_02003404() {
    WWHD_FUNC(0x02003404, void);
    header_sinit(0x101FF1D8, 0x1018C29C, 0x10000558);
}
VERIFY(0x02003404, sinit_02003404);

static u32 Visitor_true() {
    WWHD_FUNC(0x02003498, u32);
    return 1;
}
VERIFY(0x02003498, Visitor_true);

/* 020034A0: animation entry constructor (owner, name, frame controller) */
static u32 AnimEntry_ct(u32 self, u32 owner, u32 name, u32 ctrl) {
    WWHD_FUNC(0x020034A0, u32, self, owner, name, ctrl);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x3C);
        if (t == 0) return 0;
    }
    st(t + 0, owner);
    st(t + 0x38, 0x100005F8);
    stf(t + 4, ldf(0x10000580));
    u32 s = t + 8;
    if (s == 0) s = op_new(0x2C);
    if (s != 0) {
        /* inline FixedSafeString<32> constructor (its own null check is dead) */
        st(s + 0, s + 0xC);
        st(s + 4, 0x100005A0);
        st(s + 8, 0x20);
        stb(s + 0x2B, 0);
        st(s + 4, 0x100005B8);
        /* copy(name) */
        u32 nvt = ld(name + 4);
        u32 buf = ld(s + 0);
        gabi::call_ptr(ld(nvt + 0x14), name);
        u32 c = ld(name + 0);
        s32 len = 0;
        if (lbz(c) != 0) {
            for (;;) {
                ++len;
                ++c;
                if (len > 0x40000) { len = 0; break; }
                if (lbz(c) == 0) break;
            }
        }
        s32 cap = (s32)ld(s + 8);
        u32 nvt2 = ld(name + 4);
        if (!(len < cap)) len = cap - 1;
        gabi::call_ptr(ld(nvt2 + 0x14), name);
        gabi::call(OSBlockMove, buf, ld(name + 0), len, 0);
        stb(buf + len, 0);
        st(s + 4, 0x100005D0);
    }
    st(t + 0x34, ctrl);
    return t;
}
VERIFY(0x020034A0, AnimEntry_ct);

static void AnimEntry_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02003608, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02003608, AnimEntry_dt);

/* 0200361C / 02003654: set the rate and start / stop the frame controller (loop flag from its resource) */
static u32 AnimEntry_play(u32 self, f32 rate) {
    WWHD_FUNC(0x0200361C, u32, self, rate);
    u32 o = ld(self + 0x34);
    stf(self + 4, rate);
    u32 fn = vfn(o, 0x14, 0xA4);
    u32 loop = lbz(ld(o + 8) + 0xA) == 1 ? 1 : 0;
    return gabi::call_ptr<u32>(fn, o, loop);
}
VERIFY(0x0200361C, AnimEntry_play);

static u32 AnimEntry_play2(u32 self, f32 rate) {
    WWHD_FUNC(0x02003654, u32, self, rate);
    u32 o = ld(self + 0x34);
    stf(self + 4, rate);
    u32 fn = vfn(o, 0x14, 0xB4);
    u32 loop = lbz(ld(o + 8) + 0xA) == 1 ? 1 : 0;
    return gabi::call_ptr<u32>(fn, o, loop);
}
VERIFY(0x02003654, AnimEntry_play2);

/* 0200368C: finished: not looping, frame 0 and not playing */
static u32 AnimEntry_isEnd(u32 self) {
    WWHD_FUNC(0x0200368C, u32, self);
    u32 o = ld(self + 0x34);
    if (lbz(ld(o + 8) + 0xA) == 1) return 0;
    if (!(ldf(o + 0x24) == ldf(0x100005E8))) return 0;
    if (ld(o + 0x2C) & 1) return 0;
    return 1;
}
VERIFY(0x0200368C, AnimEntry_isEnd);

static void sinit_020036D0() {
    WWHD_FUNC(0x020036D0, void);
    header_sinit(0x101FF1F4, 0x1018C2C0, 0x100005EC);
}
VERIFY(0x020036D0, sinit_020036D0);

static void Comp_dt1(u32 self, s32 flags) {
    WWHD_FUNC(0x02003764, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02003764, Comp_dt1);

static void Comp_dt2(u32 self, s32 flags) {
    WWHD_FUNC(0x02003778, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02003778, Comp_dt2);

static void Comp_empty() {
    WWHD_FUNC(0x0200378C, void);
}
VERIFY(0x0200378C, Comp_empty);

/* sead BufferedSafeString::assureTerminationImpl_ instance */
static void SafeString_assureTermination(u32 self) {
    WWHD_FUNC(0x02003790, void, self);
    stb(ld(self + 0) + ld(self + 8) - 1, 0);
}
VERIFY(0x02003790, SafeString_assureTermination);

static void Comp_dt3(u32 self, s32 flags) {
    WWHD_FUNC(0x020037A8, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x020037A8, Comp_dt3);

static void Comp_dt4(u32 self, s32 flags) {
    WWHD_FUNC(0x020037BC, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x020037BC, Comp_dt4);

static u32 LytC_ct(u32 self, u32 a0, u32 a1, u32 a2) {
    WWHD_FUNC(0x020037D0, u32, self, a0, a1, a2);
    u32 t = self;
    if (t == 0) {
        t = op_new(0xB0);
        if (t == 0) return 0;
    }
    gabi::call(0x02874894, t, a0, a1, a2);
    st(t + 8, 0x10000610);
    return t;
}
VERIFY(0x020037D0, LytC_ct);

static void LytC_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02003854, void, self, flags);
    if (self == 0) return;
    gabi::call(0x02874918, self, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x02003854, LytC_dt);

static void sinit_020038A8() {
    WWHD_FUNC(0x020038A8, void);
    header_sinit(0x101FF210, 0x1018C2E4, 0x10000608);
}
VERIFY(0x020038A8, sinit_020038A8);

static u32 Adaptor_rtti() {
    WWHD_FUNC(0x0200393C, u32);
    return rtti_static3(0x101FD914, 0x101FD918, 0x101FD890, 0x101FD91C, 0x101FD888, 0x101FD8FC);
}
VERIFY(0x0200393C, Adaptor_rtti);

static u32 Adaptor_ct(u32 self, u32 target) {
    WWHD_FUNC(0x020039C0, u32, self, target);
    u32 t = self;
    if (t == 0) {
        t = op_new(8);
        if (t == 0) return 0;
    }
    st(t + 4, target);
    st(t + 0, 0x100006C8);
    return t;
}
VERIFY(0x020039C0, Adaptor_ct);

/* 02003A10: forward to the target's virtual +0x34 (argument: A, or B when A is 0) */
static u32 Adaptor_fwd34(u32 self, u32 a, u32 b, u32 c) {
    WWHD_FUNC(0x02003A10, u32, self, a, b, c);
    u32 o = ld(self + 4);
    u32 fn = vfn(o, 0xC, 0x34);
    return gabi::call_ptr<u32>(fn, o, a != 0 ? a : b, b, c);
}
VERIFY(0x02003A10, Adaptor_fwd34);

/* 02003A30: forward to the target's virtual +0x3C (other arguments passed through) */
static u32 Adaptor_fwd3C(u32 self, u32 a, u32 b, u32 c) {
    WWHD_FUNC(0x02003A30, u32, self, a, b, c);
    u32 o = ld(self + 4);
    return gabi::call_ptr<u32>(vfn(o, 0xC, 0x3C), o, a, b, c);
}
VERIFY(0x02003A30, Adaptor_fwd3C);

static void sinit_02003A44() {
    WWHD_FUNC(0x02003A44, void);
    header_sinit(0x101FF22C, 0x1018C308, 0x100006C0);
}
VERIFY(0x02003A44, sinit_02003A44);

static void Comp_dt5(u32 self, s32 flags) {
    WWHD_FUNC(0x02003AD8, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02003AD8, Comp_dt5);

static u32 LytD_ct(u32 self, u32 a0, u32 a1, u32 a2) {
    WWHD_FUNC(0x02003AEC, u32, self, a0, a1, a2);
    u32 t = self;
    if (t == 0) {
        t = op_new(0xD8);
        if (t == 0) return 0;
    }
    gabi::call(0x02878F9C, t, a0, a1, a2);
    st(t + 8, 0x100006F8);
    return t;
}
VERIFY(0x02003AEC, LytD_ct);

static void LytD_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02003B70, void, self, flags);
    if (self == 0) return;
    gabi::call(0x02879474, self, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x02003B70, LytD_dt);

static void sinit_02003BC4() {
    WWHD_FUNC(0x02003BC4, void);
    header_sinit(0x101FF248, 0x1018C32C, 0x100006F0);
}
VERIFY(0x02003BC4, sinit_02003BC4);

}  // namespace hd_ui_020032F4

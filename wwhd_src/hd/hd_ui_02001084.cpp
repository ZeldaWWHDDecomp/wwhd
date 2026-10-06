/* hd_ui_02001084: four small HD UI TUs (no GameCube source). 
 *
 * 02001084..020011CB: a 0x38 object (event buffer at +0 + vtable 10000170 at +0x34); __sinit 02001124
 * 020011CC..020012CB: the input-state holder (8 bytes, vtable 10000180 at +4) that publishes its
 *                     state pointer in the global 1018C12C; __sinit 02001228
 * 020012CC..02001533: the 0x28-byte event record {kind +0, id +4, 0x20 payload bytes +8}:
 *                     constructors, copy, assign, clear (OSBlockSet/OSBlockMove imports 028FDC08 /
 *                     028FDC00) and a short spin delay; __sinit 020014A0
 * 02001534..020015C7: initialiser-only TU (__sinit 02001534) */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_ui_02001084 {

/* 02001084: constructor of the 0x38 object (event record at +0 built inline) */
static u32 EvBox_ct(u32 self) {
    WWHD_FUNC(0x02001084, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x38);
        if (t == 0) return 0;
    }
    /* inline constructor of the sub-object at +0 (its null branch is dead) */
    st(t + 0, 0);
    st(t + 4, 0);
    gabi::call(0x020012CC, t + 8);
    st(t + 0x30, 0);
    st(t + 0x34, 0x10000170);
    return t;
}
VERIFY(0x02001084, EvBox_ct);

/* 02001110 / 020011B8: deleting destructors (trivial body) */
static void EvBox_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02001110, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02001110, EvBox_dt);

static void sinit_02001124() {
    WWHD_FUNC(0x02001124, void);
    header_sinit(0x101FF088, 0x1018C0E4, 0x10000168);
}
VERIFY(0x02001124, sinit_02001124);

static void Holder_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x020011B8, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x020011B8, Holder_dt);

/* 020011CC: input-state holder constructor: publishes STATE in 1018C12C */
static u32 Holder_ct(u32 self, u32 state) {
    WWHD_FUNC(0x020011CC, u32, self, state);
    u32 t = self;
    if (t == 0) {
        t = op_new(8);
        if (t == 0) return 0;
    }
    st(t + 0, 0);
    st(t + 4, 0x10000180);
    st(0x1018C12C, state);
    return t;
}
VERIFY(0x020011CC, Holder_ct);

static void sinit_02001228() {
    WWHD_FUNC(0x02001228, void);
    header_sinit(0x101FF0A4, 0x1018C108, 0x10000190);
}
VERIFY(0x02001228, sinit_02001228);

/* 020012BC: clear the 0x20 payload */
static u32 Ev_clearPayload(u32 self) {
    WWHD_FUNC(0x020012BC, u32, self);
    return gabi::call<u32>(0xC0009990, self + 8, 0, 0x20);
}
VERIFY(0x020012BC, Ev_clearPayload);

/* 020012CC: default constructor */
static u32 Ev_ct(u32 self) {
    WWHD_FUNC(0x020012CC, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x28);
        if (t == 0) return 0;
    }
    st(t + 0, 0);
    st(t + 4, 0);
    gabi::call(0x020012BC, t);
    return t;
}
VERIFY(0x020012CC, Ev_ct);

/* 02001320: copy a payload in (nothing for a null source) */
static void Ev_setPayload(u32 self, u32 src) {
    WWHD_FUNC(0x02001320, void, self, src);
    if (src == 0) return;
    gabi::call(0xC0009988, self + 8, src, 0x20, 0);
}
VERIFY(0x02001320, Ev_setPayload);

/* 02001338: constructor (kind, id, payload or null) */
static u32 Ev_ct2(u32 self, s32 kind, s32 id, u32 payload) {
    WWHD_FUNC(0x02001338, u32, self, kind, id, payload);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x28);
        if (t == 0) return 0;
    }
    st(t + 0, (u32)kind);
    st(t + 4, (u32)id);
    if (payload != 0) gabi::call(0x02001320, t, payload);
    else gabi::call(0x020012BC, t);
    return t;
}
VERIFY(0x02001338, Ev_ct2);

/* 020013C0: copy constructor */
static u32 Ev_ctCopy(u32 self, u32 o) {
    WWHD_FUNC(0x020013C0, u32, self, o);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x28);
        if (t == 0) return 0;
    }
    st(t + 0, ld(o + 0));
    st(t + 4, ld(o + 4));
    if (o + 8 != 0) gabi::call(0x02001320, t, o + 8);
    else gabi::call(0x020012BC, t);
    return t;
}
VERIFY(0x020013C0, Ev_ctCopy);

/* 0200143C: assignment */
static u32 Ev_assign(u32 self, u32 o) {
    WWHD_FUNC(0x0200143C, u32, self, o);
    st(self + 0, ld(o + 0));
    st(self + 4, ld(o + 4));
    gabi::call(0x02001320, self, o + 8);
    return self;
}
VERIFY(0x0200143C, Ev_assign);

/* 02001480: clear */
static u32 Ev_clear(u32 self) {
    WWHD_FUNC(0x02001480, u32, self);
    st(self + 0, 0);
    st(self + 4, 0);
    return gabi::call<u32>(0x020012BC, self);
}
VERIFY(0x02001480, Ev_clear);

/* 02001490: spin delay (32 iterations of an empty bdnz loop, no effect) */
static void spin_delay() {
    WWHD_FUNC(0x02001490, void);
}
VERIFY(0x02001490, spin_delay);

static void sinit_020014A0() {
    WWHD_FUNC(0x020014A0, void);
    header_sinit(0x101FF0C0, 0x1018C130, 0x10000198);
}
VERIFY(0x020014A0, sinit_020014A0);

static void sinit_02001534() {
    WWHD_FUNC(0x02001534, void);
    header_sinit(0x101FF0DC, 0x1018C154, 0x100001A0);
}
VERIFY(0x02001534, sinit_02001534);

}  // namespace hd_ui_02001084

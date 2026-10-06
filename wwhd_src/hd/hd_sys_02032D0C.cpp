/* hd_sys_02032D0C: the HD error viewer task (no GameCube source).
 *
 * TU 02032D0C..0203400B (__sinit 02033F60 + companions): cking::system::ErrorViewerTask (0x1AC4,
 * a sead::Task 027454E0 "ErrorViewerTask"): two FS-event delegates (+0xD0 / +0x110), mutex +0x150, a
 * ring buffer of (error code, flag) entries +0x18C {buffer, capacity 64, head, count}, the error-viewer
 * state +0x39C (0 idle, 1 shown, 2 check, 3 closing, 4 result, 5 recover), current entry +0x3A0 / +0x3A4,
 * heap +0xCC and work buffer +0x3A8 ("ErrorViewerHeap"), FS client +0x3BC, retry counters +0x3AC /
 * +0x3B4 / +0x3B8, saved screen-dim flag +0x3B0, watched volume +0x1ABC / state +0x1AC0. Error codes:
 * 0x192DB5 (disc), 0x170AD5 (storage), FS errors; the erreula calls are 0286C5F8.. (nn::erreula);
 * the system settings / account launches use sysapp.
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_sys_02032D0C {

static constexpr u32 OSReport = 0xC0009EE8;
static constexpr u32 FSAddClient = 0xC0008C78;
static constexpr u32 FSGetVolumeState = 0xC0008E30;
static constexpr u32 FSGetLastErrorCodeForViewer = 0xC0008DB8;
static constexpr u32 VPADGetTPCalibratedPoint = 0xC00075D0;
static constexpr u32 actGetSlotNo = 0xC0004660;
static constexpr u32 SYSLaunchAccount = 0xC000AA28;
static constexpr u32 SYSLaunchSettings = 0xC000AA40;

/* 02032D0C: destructor of the "registered as current viewer" flag object */
static void Reg_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02032D0C, void, self, flags);
    if (self == 0) return;
    if (lbz(self) != 0) st(0x1018F2BC, 0);
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02032D0C, Reg_dt);

static void ErrTask_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02032D38, void, self, flags);
    if (self == 0) return;
    st(self + 0x20, 0x10004C80);
    gabi::call(0x02760168, self + 0x150, 2);
    gabi::call(0x027C0804, self + 0x110, 2);
    gabi::call(0x027C0804, self + 0xD0, 2);
    gabi::call(0x02032D0C, self + 0xC8, 2);
    gabi::call(0x027452C8, self, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x02032D38, ErrTask_dt);

static void ErrTask_dtThunk(u32 self, s32 flags) {
    WWHD_FUNC(0x02032DC8, void, self, flags);
    gabi::call(0x02032D38, self - 0x14, flags);
}
VERIFY(0x02032DC8, ErrTask_dtThunk);

/* 02032DD0: register as the current error viewer (once) */
static void ErrTask_register(u32 self) {
    WWHD_FUNC(0x02032DD0, void, self);
    if (ld(0x1018F2BC) != 0) return;
    st(0x1018F2BC, self);
    stb(self + 0xC8, 1);
}
VERIFY(0x02032DD0, ErrTask_register);

static inline void sstr_ct(u32 p, u32 t) {
    if (p == 0) {
        p = op_new(8);
        if (p == 0) return;
    }
    (void)t;
    st(p + 0, 0x10004AF4);
    st(p + 4, 0x10004A7C);
}
static inline void deleg_ct(u32 p) {
    if (p == 0) return;
    st(p + 0xC, 0);
    st(p + 0, 0x10004A64);
    sth(p + 8, 0);
    sth(p + 0xA, 0);
    st(p + 4, 0);
    st(p + 0, 0x10004AC4);
}

/* 02032DF0: constructor */
static u32 ErrTask_ct(u32 self, u32 param) {
    WWHD_FUNC(0x02032DF0, u32, self, param);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x1AC4);
        if (t == 0) return 0;
    }
    gabi::call(0x027454E0, t, param, 0x10004AF8); /* "ErrorViewerTask" */
    st(t + 0x70, 0x10004BC8);
    st(t + 0x20, 0x10004C80);
    stb(t + 0xC8, 0);
    st(t + 0xCC, 0);
    gabi::call(0x02752A84, t + 0xD0);
    sstr_ct(t + 0xE0, t);
    st(t + 0xE8, 0);
    st(t + 0xDC, 0x10169E3C);
    st(t + 0xEC, 0);
    deleg_ct(t + 0xF0);
    gabi::call(0x02752A84, t + 0x110);
    sstr_ct(t + 0x120, t);
    st(t + 0x11C, 0x10169E3C);
    st(t + 0x128, 0);
    st(t + 0x12C, 0);
    deleg_ct(t + 0x130);
    gabi::call(0x02760084, t + 0x150);
    u32 rb = t + 0x18C;
    if (rb == 0) rb = op_new(0x210);
    if (rb != 0) {
        st(rb + 0xC, 0);
        st(rb + 8, 0);
        st(rb + 0, rb + 0x10);
        st(rb + 4, 0x40);
    }
    st(t + 0x39C, 0);
    gabi::call(0x028F521C, t + 0x3A0, 8);
    st(t + 0x3B4, 0);
    st(t + 0x3A8, 0);
    stb(t + 0x3B0, 1);
    st(t + 0x3B8, 0);
    st(t + 0x3AC, 0);
    gabi::call(0x028F521C, t + 0x3BC, 0x1700);
    st(t + 0x1AC0, 1);
    st(t + 0x1ABC, 0);
    return t;
}
VERIFY(0x02032DF0, ErrTask_ct);

static u32 ErrTask_isShowing(u32 self) {
    WWHD_FUNC(0x02032FE8, u32, self);
    u32 s = ld(self + 0x39C);
    return (s != 0 && s != 5) ? 1 : 0;
}
VERIFY(0x02032FE8, ErrTask_isShowing);

/* 0203300C: queue an error (CODE, FLAG) */
static void ErrTask_push(u32 self, u32 code, u32 flag) {
    WWHD_FUNC(0x0203300C, void, self, code, flag);
    gabi::call(OSReport, 0x10004B08, code, flag);
    gabi::call(0x027601BC, self + 0x150);
    s32 cnt = (s32)ld(self + 0x198);
    if (cnt < (s32)ld(self + 0x190)) {
        s32 idx = (s32)ld(self + 0x194) + cnt;
        s32 cap = (s32)ld(self + 0x190);
        u32 buf = ld(self + 0x18C);
        st(self + 0x198, (u32)(cnt + 1));
        if (!(idx < cap)) idx -= cap;
        st(buf + (u32)idx * 8, code);
        stb(buf + (u32)idx * 8 + 4, (u8)flag);
    }
    gabi::call(0x027601F0, self + 0x150);
}
VERIFY(0x0203300C, ErrTask_push);

struct w12_l { u8 b[0xC]; };

static void ErrTask_appear(u32 self) {
    WWHD_FUNC(0x020330BC, void, self);
    gabi::Local<w12_l> l;
    st(l.a, 2);
    gabi::call(0x0286CB7C, l.a);
}
VERIFY(0x020330BC, ErrTask_appear);

static void ErrTask_disappear(u32 self) {
    WWHD_FUNC(0x020330E8, void, self);
    u32 r = gabi::call<u32>(0x0286CA40, self);
    if (r != 0) gabi::call(0x0286CBB4, r);
}
VERIFY(0x020330E8, ErrTask_disappear);

static u32 ErrTask_workSize(u32 self) {
    WWHD_FUNC(0x02033114, u32, self);
    return gabi::call<u32>(0x0286CBC4, self);
}
VERIFY(0x02033114, ErrTask_workSize);

/* 02033118: erreula region from the system language/region settings */
static u32 ErrTask_region() {
    WWHD_FUNC(0x02033118, u32);
    u32 v = ld(ld(0x101F4BAC) + 0x10);
    if (v == 1) return 0;
    if (v == 2) return 1;
    if (v == 4) return 2;
    return 1;
}
VERIFY(0x02033118, ErrTask_region);

/* 0203315C: erreula language from the system language (table 10004B34) */
static u32 ErrTask_language() {
    WWHD_FUNC(0x0203315C, u32);
    u32 v = ld(ld(0x101F4BAC) + 0x14);
    if (v <= 5) return lbz(0x10004B34 + v);
    if (v < 8) return 1;
    if (v <= 10) return lbz(0x10004B34 + v);
    return 1;
}
VERIFY(0x0203315C, ErrTask_language);

struct create_l { u8 b[0x10]; };

/* 020331A8: create the error viewer heap and work buffer, then the erreula viewer */
static void ErrTask_createViewer(u32 self) {
    WWHD_FUNC(0x020331A8, void, self);
    u32 parent = gabi::call<u32>(0x0203E918);
    u32 sz = gabi::call<u32>(0x02033114, self);
    gabi::Local<w12_l> name;
    st(name.a + 4, 0x10004A7C);
    st(name.a + 0, 0x10004B40); /* "ErrorViewerHeap" */
    u32 heap = gabi::call<u32>(0x02753004, sz + 0x200, name.a, parent, 1, 1);
    st(self + 0xCC, heap);
    u32 sz2 = gabi::call<u32>(0x02033114, self);
    u32 work = gabi::call<u32>(0x0273B0D4, sz2, ld(self + 0xCC), 4);
    st(self + 0x3A8, work);
    u32 sz3 = gabi::call<u32>(0x02033114, self);
    gabi::call(OSBlockSet, work, 0, sz3);
    gabi::Local<create_l> c;
    st(c.a + 8, 0);
    st(c.a + 0xC, self + 0x3BC);
    st(c.a + 4, 0);
    st(c.a + 0, ld(self + 0x3A8));
    st(c.a + 4, gabi::call<u32>(0x02033118));
    st(c.a + 8, gabi::call<u32>(0x0203315C));
    gabi::call(0x0286C5F8, c.a);
}
VERIFY(0x020331A8, ErrTask_createViewer);

static inline u32 cpu_list() {
    u32 g = ld(0x101F95D0);
    u32 n = ld(g + 0x1020);
    return n;
}

/* 02033284: task prepare: two FS event delegates on the system's FS command block lists, FS client, viewer */
static void ErrTask_prepare(u32 self) {
    WWHD_FUNC(0x02033284, void, self);
    u32 d = self + 0xF0;
    if (d != 0) {
        st(d + 0, 0x10004ADC);
        st(d + 4, self);
        sth(d + 0xA, 0xFFFF);
        st(d + 0xC, 0x02033F58);
        sth(d + 8, 0);
    }
    st(self + 0xE0, 0x10004B50);
    st(self + 0xE8, 2);
    {
        u32 g = ld(0x101F95D0);
        u32 n = ld(g + 0x1020);
        u32 arr = ld(g + 0x1024);
        if (n > 0xC) arr += 0x30;
        gabi::call(0x027C23F8, ld(arr), 0, self + 0xD0);
    }
    u32 d2 = self + 0x130;
    if (d2 != 0) {
        st(d2 + 0, 0x10004ADC);
        st(d2 + 4, self);
        sth(d2 + 0xA, 0xFFFF);
        st(d2 + 0xC, 0x02033F5C);
        sth(d2 + 8, 0);
    }
    st(self + 0x120, 0x10004B50);
    st(self + 0x128, 2);
    {
        u32 g = ld(0x101F95D0);
        u32 n = ld(g + 0x1020);
        u32 arr = ld(g + 0x1024);
        if (n > 0xD) arr += 0x34;
        gabi::call(0x027C23F8, ld(arr), 0, self + 0x110);
    }
    gabi::call(FSAddClient, self + 0x3BC, 0);
    gabi::call(0x020331A8, self);
}
VERIFY(0x02033284, ErrTask_prepare);

struct ctrl_l { u8 b[0xC0]; };

/* 020333B4: feed the erreula viewer the controllers' and the GamePad's input of this frame */
static void ErrTask_feedInput(u32 self) {
    WWHD_FUNC(0x020333B4, void, self);
    gabi::Local<ctrl_l> l;
    for (u32 i = 0; i < 5; ++i) st(l.a + i * 4, 0);
    u32 base = gabi::call<u32>(0x02034130, ld(0x101F8AE8));
    u32 pads = base + 0x1118;
    for (u32 i = 0; i < 4; ++i) {
        u32 p = pads + i * 0xF08;
        if ((s32)ld(p + 0xF00) > 0 || ld(p + 0xF04) == 0xFFFFFFFF) st(l.a + 4 + i * 4, p);
    }
    u32 x = gabi::call<u32>(0x02034200, ld(0x101F8AE8));
    if ((s32)ld(x + 0xAD4) > 0 || ld(x + 0x14 + 0xAC4) == 0xFFFFFFFF) {
        for (u32 i = 0; i < 0x2B; ++i) st(l.a + 0x14 + i * 4, ld(x + 0x14 + i * 4));
        gabi::call(VPADGetTPCalibratedPoint, 0, l.a + 0x66, l.a + 0x66);
        st(l.a + 0, l.a + 0x14);
    }
    gabi::call(0x0286CA50, l.a);
}
VERIFY(0x020333B4, ErrTask_feedInput);

/* 020334C8: FS error code of a volume (0 while it is mounted) */
static u32 ErrTask_volumeError(u32 self, u32 client) {
    WWHD_FUNC(0x020334C8, u32, self, client);
    if (gabi::call<s32>(FSGetVolumeState, client) == 1) return 0;
    return gabi::call<u32>(FSGetLastErrorCodeForViewer, client);
}
VERIFY(0x020334C8, ErrTask_volumeError);

/* 0203351C: entering the viewer: pause sound, stop vibration, remember and force the screen dimming */
static void ErrTask_enter(u32 self) {
    WWHD_FUNC(0x0203351C, void, self);
    st(self + 0x3B8, 0);
    st(self + 0x3B4, 0);
    gabi::call(0x02031138);
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x025CB694, play + 0x599C);
    u32 d = gabi::call<u32>(0x02032C6C);
    stb(self + 0x3B0, (u8)gabi::call<u32>(0x02032BD0, d));
    u32 d2 = gabi::call<u32>(0x02032C6C);
    gabi::call(0x02032AD8, d2, 1);
}
VERIFY(0x0203351C, ErrTask_enter);

struct app_l { u8 b[0x2C]; };

/* 02033578: show the error CODE */
static void ErrTask_show(u32 self, u32 code) {
    WWHD_FUNC(0x02033578, void, self, code);
    gabi::call(OSReport, 0x10004B70, code);
    gabi::Local<app_l> a;
    st(a.a + 0x10, code);
    st(a.a + 0, 0);
    st(a.a + 0x24, 0);
    stb(a.a + 0x28, 1);
    st(a.a + 0x18, 0);
    st(a.a + 8, 0);
    st(a.a + 4, 2);
    st(a.a + 0x20, 0);
    st(a.a + 0x14, 2);
    st(a.a + 0x1C, 0);
    st(a.a + 0xC, 0);
    gabi::call(0x0286CB2C, a.a);
}
VERIFY(0x02033578, ErrTask_show);

static void ErrTask_setState(u32 self, u32 s) {
    WWHD_FUNC(0x020335F0, void, self, s);
    st(self + 0x39C, s);
}
VERIFY(0x020335F0, ErrTask_setState);

/* take the next queued error into +0x3A0 / +0x3A4 (caller holds the mutex) */
static inline void pop_entry(u32 self) {
    s32 head = (s32)ld(self + 0x194);
    s32 cap = (s32)ld(self + 0x190);
    u32 buf = ld(self + 0x18C);
    s32 idx = head < cap ? head : head - cap;
    u32 e = buf + (u32)idx * 8;
    s32 h2 = (s32)ld(self + 0x194) + 1;
    st(self + 0x3A0, ld(e));
    st(self + 0x194, (u32)h2);
    st(self + 0x3A4, ld(e + 4));
    s32 cnt = (s32)ld(self + 0x198);
    if (!(h2 < (s32)ld(self + 0x190))) st(self + 0x194, 0);
    st(self + 0x198, (u32)(cnt - 1));
}

/* 020335F8: state 0: watch disc / storage / FS errors and show the next queued one */
static void ErrTask_stateIdle(u32 self) {
    WWHD_FUNC(0x020335F8, void, self);
    bool reset1 = false;
    if (gabi::call<u32>(0x02618498, ld(0x101F5088)) == 0 && gabi::call<u32>(0x026184AC, ld(0x101F5088)) == 0) reset1 = true;
    else if (gabi::call<u32>(0x02617F14, ld(0x101F5088)) != 0) reset1 = true;
    bool check2;
    if (reset1) {
        st(self + 0x3B4, 0);
        check2 = gabi::call<u32>(0x02617AE4, ld(0x101F5088)) != 0;
    } else {
        u32 c = ld(self + 0x3B4) + 1;
        st(self + 0x3B4, c);
        if (c < 3) {
            check2 = gabi::call<u32>(0x02617AE4, ld(0x101F5088)) != 0;
        } else {
            gabi::call(0x0203300C, self, 0x192DB5, 0);
            check2 = gabi::call<u32>(0x02617AE4, ld(0x101F5088)) != 0;
        }
    }
    bool reset2 = true;
    if (check2 && gabi::call<u32>(0x02617FCC, ld(0x101F5088)) == 0) {
        reset2 = false;
        u32 c = ld(self + 0x3B8) + 1;
        st(self + 0x3B8, c);
        if (!(c < 0xD)) gabi::call(0x0203300C, self, 0x170AD5, 0);
    }
    if (reset2) st(self + 0x3B8, 0);
    u32 vol = ld(0x101F8B08) + 0x24;
    u32 e = gabi::call<u32>(0x020334C8, self, vol);
    if (e != 0) {
        st(self + 0x1ABC, vol);
        st(self + 0x1AC0, gabi::call<u32>(FSGetVolumeState, vol));
        gabi::call(0x0203300C, self, e, 1);
    }
    gabi::call(0x027601BC, self + 0x150);
    if ((s32)ld(self + 0x198) > 0) pop_entry(self);
    gabi::call(0x027601F0, self + 0x150);
    if (ld(self + 0x3A0) != 0) {
        gabi::call(0x0203351C, self);
        gabi::call(0x02033578, self, ld(self + 0x3A0));
        gabi::call(0x020335F0, self, 1);
    }
}
VERIFY(0x020335F8, ErrTask_stateIdle);

static void ErrTask_stateShown(u32 self) {
    WWHD_FUNC(0x02033804, void, self);
    if (gabi::call<s32>(0x0286CA30, self) == 2) gabi::call(0x020335F0, self, 2);
}
VERIFY(0x02033804, ErrTask_stateShown);

/* 02033844 / 0203389C / 02033900 / 02033954: is the error ENT still present (1) or resolved (0) */
static u32 ErrTask_discStill(u32 self, u32 ent) {
    WWHD_FUNC(0x02033844, u32, self, ent);
    if (ld(ent) != 0x192DB5) return 1;
    return gabi::call<u32>(0x02617F14, ld(0x101F5088)) != 0 ? 0 : 1;
}
VERIFY(0x02033844, ErrTask_discStill);

static u32 ErrTask_storageStill(u32 self, u32 ent) {
    WWHD_FUNC(0x0203389C, u32, self, ent);
    if (ld(ent) != 0x170AD5) return 1;
    u32 m = ld(0x101F5088);
    if (ld(m + 0x1D0) != 0) return 0;
    return gabi::call<u32>(0x02617FCC, m) != 0 ? 0 : 1;
}
VERIFY(0x0203389C, ErrTask_storageStill);

static u32 ErrTask_volumeStill(u32 self, u32 ent) {
    WWHD_FUNC(0x02033900, u32, self, ent);
    (void)ent;
    u32 v = gabi::call<u32>(FSGetVolumeState, ld(self + 0x1ABC));
    if (v == ld(self + 0x1AC0)) return 1;
    st(self + 0x1ABC, 0);
    return 0;
}
VERIFY(0x02033900, ErrTask_volumeStill);

static u32 ErrTask_otherStill(u32 self, u32 ent) {
    WWHD_FUNC(0x02033954, u32, self, ent);
    return gabi::call<u32>(0x0286CB4C, self, ent) == 0 ? 1 : 0;
}
VERIFY(0x02033954, ErrTask_otherStill);

static void ErrTask_close(u32 self) {
    WWHD_FUNC(0x02033990, void, self);
    u32 r = gabi::call<u32>(OSReport, 0x10004B94);
    gabi::call(0x0286CB3C, r);
}
VERIFY(0x02033990, ErrTask_close);

/* 020339C0: state 2: close the viewer once the error is resolved */
static void ErrTask_stateCheck(u32 self) {
    WWHD_FUNC(0x020339C0, void, self);
    u32 code = ld(self + 0x3A0);
    gabi::Local<be<u32>[2]> l1;
    gabi::Local<be<u32>[2]> l2;
    gabi::Local<be<u32>[2]> l3;
    gabi::Local<be<u32>[2]> l4;
    u32 still;
    if (code - 0x192D50u < 0x270Fu) {
        st(l1.a, code);
        st(l1.a + 4, ld(self + 0x3A4));
        still = gabi::call<u32>(0x02033844, self, l1.a);
    } else if (code == 0x170AD5) {
        st(l2.a, code);
        st(l2.a + 4, ld(self + 0x3A4));
        still = gabi::call<u32>(0x0203389C, self, l2.a);
    } else if (ld(self + 0x1ABC) != 0) {
        st(l3.a, code);
        st(l3.a + 4, ld(self + 0x3A4));
        still = gabi::call<u32>(0x02033900, self, l3.a);
    } else {
        st(l4.a, code);
        st(l4.a + 4, ld(self + 0x3A4));
        still = gabi::call<u32>(0x02033954, self, l4.a);
    }
    if (still != 0) return;
    gabi::call(0x02033990, self);
    gabi::call(0x020335F0, self, 3);
}
VERIFY(0x020339C0, ErrTask_stateCheck);

static void ErrTask_stateClosing(u32 self) {
    WWHD_FUNC(0x02033AAC, void, self);
    if (gabi::call<s32>(0x0286CA30, self) == 0) gabi::call(0x020335F0, self, 4);
}
VERIFY(0x02033AAC, ErrTask_stateClosing);

/* 02033AEC: jump to the account settings (0x14 / 0x15) or the system settings (0xB / 0xC / 0x10 pages) */
static u32 ErrTask_launch(u32 self, s32 k) {
    WWHD_FUNC(0x02033AEC, u32, self, k);
    gabi::Local<w12_l> a;
    gabi::Local<w12_l> b;
    if (k == 0x14 || k == 0x15) {
        gabi::call(OSBlockSet, a.a, 0, 0xC);
        st(a.a + 8, gabi::call<u32>(actGetSlotNo));
        gabi::call(SYSLaunchAccount, a.a);
        return 0;
    }
    gabi::call(OSBlockSet, b.a, 0, 0xC);
    if ((u32)k == 0xB) st(b.a + 8, 1);
    else if ((u32)k == 0xC) st(b.a + 8, 2);
    else if ((u32)k == 0x10) st(b.a + 8, 6);
    gabi::call(SYSLaunchSettings, b.a);
    return 0;
}
VERIFY(0x02033AEC, ErrTask_launch);

static void ErrTask_restoreDim(u32 self) {
    WWHD_FUNC(0x02033BAC, void, self);
    u32 d = gabi::call<u32>(0x02032C6C);
    gabi::call(0x02032AD8, d, lbz(self + 0x3B0));
}
VERIFY(0x02033BAC, ErrTask_restoreDim);

/* 02033BE0: state 4: act on the viewer's result, then the next queued error or recover */
static void ErrTask_stateResult(u32 self) {
    WWHD_FUNC(0x02033BE0, void, self);
    if (lbz(self + 0x3A4) != 0) {
        u32 r = gabi::call<u32>(0x0286CB6C, self);
        bool next;
        if (r == 0x1E5D7C) {
            next = gabi::call<u32>(0x02033AEC, self, 0xC) != 0;
        } else {
            s32 s = gabi::call<s32>(0x0286CB5C, r);
            if (s == 2) {
                u32 q = gabi::call<u32>(0x020335F0, self, 1);
                u32 c = gabi::call<u32>(0x0286CB6C, q);
                gabi::call(0x02033578, self, c);
                return;
            }
            if (s == 3) {
                u32 k = gabi::call<u32>(0x0286CB6C, (u32)s);
                next = gabi::call<u32>(0x02033AEC, self, k) != 0;
            } else {
                next = true;
            }
        }
        if (!next) {
            gabi::call(0x020335F0, self, 6);
            return;
        }
    }
    gabi::call(0x027601BC, self + 0x150);
    s32 cnt = (s32)ld(self + 0x198);
    if (cnt == 0) {
        stb(self + 0x3A4, 0);
        st(self + 0x3A0, 0);
    } else if (cnt > 0) {
        pop_entry(self);
    }
    gabi::call(0x027601F0, self + 0x150);
    if (ld(self + 0x3A0) == 0) {
        u32 q = gabi::call<u32>(0x020335F0, self, 5);
        gabi::call(0x02033BAC, q);
        return;
    }
    u32 q = gabi::call<u32>(0x020335F0, self, 1);
    gabi::call(0x02033578, q, ld(self + 0x3A0));
}
VERIFY(0x02033BE0, ErrTask_stateResult);

/* 02033DA4: state 5: after three frames resume sound and go idle */
static void ErrTask_stateRecover(u32 self) {
    WWHD_FUNC(0x02033DA4, void, self);
    u32 c = ld(self + 0x3AC) + 1;
    st(self + 0x3AC, c);
    if (c < 3) return;
    gabi::call(0x0203116C);
    gabi::call(0x020335F0, self, 0);
    st(self + 0x3AC, 0);
}
VERIFY(0x02033DA4, ErrTask_stateRecover);

/* 02033DF4: task calc */
static void ErrTask_calc(u32 self) {
    WWHD_FUNC(0x02033DF4, void, self);
    gabi::call(0x020333B4, self);
    static const u32 fn[6] = {0x020335F8, 0x02033804, 0x020339C0, 0x02033AAC, 0x02033BE0, 0x02033DA4};
    s32 s = (s32)ld(self + 0x39C);
    if (s >= 0 && s < 6) gabi::call(fn[s], self);
}
VERIFY(0x02033DF4, ErrTask_calc);

static void ErrTask_exit(u32 self) {
    WWHD_FUNC(0x02033EE8, void, self);
    gabi::call(0x0286C9CC, self);
    u32 w = ld(self + 0x3A8);
    if (w != 0) {
        gabi::call(0x0273AFC8, w);
        st(self + 0x3A8, 0);
    }
    u32 h = ld(self + 0xCC);
    if (h != 0) {
        gabi::call_ptr(vfn(h, 0xC, 0x24), h);
        st(self + 0xCC, 0);
    }
}
VERIFY(0x02033EE8, ErrTask_exit);

static u32 ErrTask_exitThunk(u32 self) {
    WWHD_FUNC(0x02033F54, u32, self);
    return gabi::call<u32>(0x02033EE8, self);
}
VERIFY(0x02033F54, ErrTask_exitThunk);

static u32 ErrTask_fsEvent1(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x02033F58, u32, self, a, b);
    return gabi::call<u32>(0x0286CAA4, self, a, b);
}
VERIFY(0x02033F58, ErrTask_fsEvent1);

static u32 ErrTask_fsEvent2(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x02033F5C, u32, self, a, b);
    return gabi::call<u32>(0x0286CAE8, self, a, b);
}
VERIFY(0x02033F5C, ErrTask_fsEvent2);

static void sinit_02033F60() {
    WWHD_FUNC(0x02033F60, void);
    header_sinit(0x10200820, 0x1018F298, 0x10004BC0);
}
VERIFY(0x02033F60, sinit_02033F60);

static void Comp_dt1(u32 self, s32 flags) {
    WWHD_FUNC(0x02033FF4, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02033FF4, Comp_dt1);

static void Comp_empty() {
    WWHD_FUNC(0x02034008, void);
}
VERIFY(0x02034008, Comp_empty);

}  // namespace hd_sys_02032D0C

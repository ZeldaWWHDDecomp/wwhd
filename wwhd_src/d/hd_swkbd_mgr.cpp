/* hd_swkbd_mgr: cking::ui::input::SwkbdMgr (GamePad software keyboard for names), WWHD.
 * HD-only code (no GameCube source): written from the
 * WWHD code. Range 02619220..0261A197 (02619220 is the pointer controller's deleting destructor, a
 * companion of the preceding TU); static initialiser 0261A0D8. Not verified here (sead library):
 * 0261A16C (SafeString deleting destructor), 0261A180 (SafeString assureTermination).
 *
 * SwkbdMgr (0x1C8): base IDisposer, +0x10 u16 text buffer, +0x34 work memory, +0x38 / +0x3C the two
 * keyboard layouts (02672D98 / 02673AFC), +0x40 controller info copy (0xAC), +0xEC controller
 * pointers, +0x100 keyboard create/appear arguments, +0x1A8..+0x1BC input form arguments,
 * +0x1C0 mode, +0x1C4 vtable 100E2AC0. nn::swkbd wrappers at 0286CD54..0286D400.
 */
#include "gabi.h"
using namespace gabi;

namespace hd_swkbd_mgr {

static const u32 kInstance = 0x101F50D8;
static const u32 kCtrlMgr = 0x101F8AE8;

/* 02619220: pointer controller deleting destructor */
void pointerDtor(u32 p, u32 flags) {
    WWHD_FUNC(0x02619220, void, p, flags);
    if (!p) return;
    store<u32>(p + 0x13C, 0x100E2A78);
    call<void>(0x02001728, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x02619220, pointerDtor);

/* 02619280: constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x02619280, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x1C8u);
        if (!p) return 0;
    }
    store<u32>(p + 0x34, 0);
    store<u32>(p + 0x3C, 0);
    store<u32>(p + 0x38, 0);
    store<u32>(p + 0x1C4, 0x100E2AC0);
    call<void>(0x028F521C, p + 0x40, 0xACu);
    u32 q = p + 0xEC;
    if (!q) q = call<u32>(0x0273AD10, 0x14u);
    if (q)
        for (u32 i = 0; i < 5; i++) store<u32>(q + 4 * i, 0);
    q = p + 0x100;
    if (!q) q = call<u32>(0x0273AD10, 0xA8u);
    if (q) {
        store<u32>(q + 0x30, 0);
        store<u32>(q, 0);
        store<u32>(q + 0x14, 0xFFFFFFFF);
        store<u32>(q + 0x10, 0x13);
        store<u8>(q + 0x18, 0);
        store<u32>(q + 0xA4, 0xFFFFFFFF);
        store<u32>(q + 0x28, 0);
        store<u32>(q + 0xC, 0x7FFFF);
        store<u32>(q + 4, 4);
        store<u32>(q + 0x2C, 0);
        store<u8>(q + 0xA0, 0);
        store<u16>(q + 0x22, 0);
        store<u32>(q + 8, 0);
        store<u16>(q + 0x20, 0);
        store<u8>(q + 0x24, 0);
        store<u32>(q + 0x1C, 0);
        store<u32>(q + 0x9C, 1);
        for (u32 i = 0; i < 13; i++) {
            store<u32>(q + 0x34 + 4 * i, 0);
            store<u32>(q + 0x68 + 4 * i, 0);
        }
    }
    store<u32>(p + 0x1A8, 0);
    store<u32>(p + 0x1B8, 0);
    store<u32>(p + 0x1B4, 0xFFFFFFFF);
    store<u32>(p + 0x1C0, 2);
    store<u32>(p + 0x1B0, 0);
    store<u32>(p + 0x1AC, 0);
    store<u32>(p + 0x1BC, 0xFFFFFFFF);
    return p;
}
VERIFY(0x02619280, ctor);

/* 026193D4: creates the singleton (the disposer is the object's base) */
void createInstance(u32 heap) {
    WWHD_FUNC(0x026193D4, void, heap);
    if (load<u32>(kInstance)) return;
    const u32 p = call<u32>(0x0273B0D4, 0x1C8u, heap, 4u);
    if (p) {
        call<void>(0x02752B0C, p, heap, 3u);
        store<u32>(p + 0xC, 0x100E2AF8);
    }
    store<u32>(0x101F50DC, p);
    store<u32>(kInstance, p ? ctor(p) : 0u);
}
VERIFY(0x026193D4, createInstance);

/* 02619468: creates the two keyboard layouts and gives them the system fonts */
void createLayouts(u32 self) {
    WWHD_FUNC(0x02619468, void, self);
    const u32 heap = call<u32>(0x0203E8E8);
    u32 a = call<u32>(0x0273B050, 0x60u, heap, 4u);
    if (a) a = call<u32>(0x02672D98, a);
    store<u32>(self + 0x38, a);
    u32 b = call<u32>(0x0273B050, 0x50u, heap, 4u);
    if (b) b = call<u32>(0x02673AFC, b);
    store<u32>(self + 0x3C, b);
    const u32 g = load<u32>(0x101F95D0);
    const u32 n = load<u32>(g + 0x1020);
    u32 e = load<u32>(g + 0x1024);
    if (n > 7) e += 0x1C;
    const u32 font = load<u32>(e);
    const u32 flags = load<u32>(font + 0x50);
    u32 f1, f2;
    bool alt = false;
    if ((flags >> 12) & 1) {
        f1 = load<u32>(font + 0x164);
        alt = (flags >> 6) & 1;
    } else {
        f1 = 0x104A20FC;
        const u32 v = load<u32>(font + 0x4C);
        if (v) f1 = v;
        alt = (flags >> 6) & 1;
    }
    if (alt && ((flags >> 7) & 1)) {
        f2 = font + 0x84;
    } else {
        const u32 v = load<u32>(font + 0x48);
        f2 = v ? v : 0x104A2098;
    }
    call<void>(0x026730B8, load<u32>(self + 0x38), f1, f2, 0u);
    call<void>(0x02673B68, load<u32>(self + 0x3C), f1, f2, 0u);
}
VERIFY(0x02619468, createLayouts);

/* 026195C4: prepare: work heap, keyboard creation for the console region and language */
void prepare(u32 self) {
    WWHD_FUNC(0x026195C4, void, self);
    createLayouts(self);
    Local<u32[6]> F; /* +0 create arg {work, region, 2, FS client}, +0x10 heap name */
    const u32 arg = F.a, name = F.a + 0x10;
    store<u32>(name + 4, 0x100E2A88);
    store<u32>(name, 0x100E2AD0);
    const u32 heap = call<u32>(0x02753004, 0x1900000u, name, call<u32>(0x0203E918), 1u, 0u);
    store<u32>(arg, 0);
    store<u32>(arg + 0xC, 0);
    store<u32>(arg + 4, 0);
    store<u32>(arg + 8, 2);
    const u32 size = call<u32>(0x0286CD54, 2u);
    const u32 work = call<u32>(0x0273B0D4, size, heap, 4u);
    store<u32>(arg, work);
    store<u32>(self + 0x34, work);
    const u32 fs = load<u32>(0x101F8B08) + 0x24;
    store<u32>(self + 0x19C, 2);
    store<u8>(self + 0x1A0, 0);
    store<u8>(self + 0x124, 0);
    store<u8>(self + 0x118, 1);
    const u32 cfg = load<u32>(0x101F4BAC);
    store<u32>(arg + 0xC, fs);
    const u32 region = load<u32>(cfg + 0x10), lang = load<u32>(cfg + 0x14);
    u32 kb, layout;
    if (region == 1) {
        store<u32>(arg + 4, 0);
        store<u32>(self + 0x100, 0);
        store<u32>(self + 0x10C, 0x6001);
        kb = 0xD;
        layout = 0;
        (void)layout;
        store<u32>(self + 0x128, 0xAD);
        store<u32>(self + 0x114, 0xFFFFFFFF);
        store<u32>(self + 0x110, kb);
        store<u32>(self + 0x108, 0);
    } else if (region == 4) {
        store<u32>(arg + 4, 2);
        store<u32>(self + 0x10C, 0x3E0);
        u32 k = 5, l = 1;
        if (lang == 2) { k = 6; l = 2; }
        else if (lang == 3) { k = 7; l = 3; }
        else if (lang == 4) { k = 8; l = 4; }
        else if (lang == 5) { k = 9; l = 5; }
        store<u32>(self + 0x108, 0);
        store<u32>(self + 0x128, 0xAD);
        store<u32>(self + 0x114, 0xFFFFFFFF);
        store<u32>(self + 0x110, k);
        store<u32>(self + 0x100, l);
    } else {
        store<u32>(arg + 4, 1);
        store<u32>(self + 0x10C, 0xE);
        u32 k = 1, l = 1;
        if (lang == 2) { k = 2; l = 2; }
        else if (lang == 5) { k = 3; l = 5; }
        store<u32>(self + 0x100, l);
        store<u32>(self + 0x110, k);
        store<u32>(self + 0x108, 0);
        store<u32>(self + 0x114, 0xFFFFFFFF);
        store<u32>(self + 0x128, 0xAD);
    }
    call<void>(0x0286CD7C, arg);
    call_ptr<void>(load<u32>(load<u32>(heap + 0xC) + 0x2C), heap);
}
VERIFY(0x026195C4, prepare);

/* 02619978: passes the GamePad and DRC controller states to the keyboard */
void setControllers(u32 self) {
    WWHD_FUNC(0x02619978, void, self);
    const u32 pad = call<u32>(0x0273CB8C, load<u32>(kCtrlMgr), 8u);
    if (!load<u32>(0x101FD694)) {
        store<u32>(0x101FD694, 1);
        store<u32>(0x101FD968, 0x100E2AB0);
    }
    if (pad && call_ptr<u32>(load<u32>(load<u32>(pad + 0x10) + 0xC), pad, 0x101FD968u) && pad &&
        s32(load<u32>(pad + 0xAD4)) > 0) {
        for (u32 i = 0; i < 0x2B; i++) store<u32>(self + 0x40 + 4 * i, load<u32>(pad + 0x14 + 4 * i));
        call<void>(0xC00075D0, 0u, self + 0x92, self + 0x92);
        store<u32>(self + 0xEC, self + 0x40);
    } else {
        store<u32>(self + 0xEC, 0);
    }
    const u32 drc = call<u32>(0x0273CB8C, load<u32>(kCtrlMgr), 7u);
    if (!load<u32>(0x101FD698)) {
        store<u32>(0x101FD978, 0x100E2AB0);
        store<u32>(0x101FD698, 1);
    }
    if (drc && call_ptr<u32>(load<u32>(load<u32>(drc + 0x10) + 0xC), drc, 0x101FD978u) && drc) {
        store<u32>(self + 0xF0, s32(load<u32>(drc + 0x2018)) > 0 ? drc + 0x1118 : 0u);
    } else {
        for (u32 i = 0; i < 4; i++) store<u32>(self + 0xF0 + 4 * i, 0);
    }
    call<void>(0x0286D280, self + 0xEC);
    if (call<u32>(0x0286D2D4)) call<void>(0x0286D2E4);
}
VERIFY(0x02619978, setControllers);

/* 02619B68: per-frame update while the keyboard is shown */
void calc(u32 self) {
    WWHD_FUNC(0x02619B68, void, self);
    if (!call<u32>(0x0286D270)) return;
    setControllers(self);
    call<void>(0x026732B8, load<u32>(self + 0x38), self + 0x10);
    u32 a = load<u32>(self + 0x38);
    call_ptr<void>(load<u32>(load<u32>(a + 4) + 0x5C), a);
    Local<u32[4]> st;
    call<void>(0x0267341C, load<u32>(self + 0x38), st.a);
    call<void>(0x02673C98, load<u32>(self + 0x3C), st.a);
    a = load<u32>(self + 0x3C);
    call_ptr<void>(load<u32>(load<u32>(a + 4) + 0x5C), a);
}
VERIFY(0x02619B68, calc);

/* 02619BF0: keyboard state 2 (shown)? */
u32 isShown(u32 self) {
    WWHD_FUNC(0x02619BF0, u32, self);
    return call<u32>(0x0286D270) == 2;
}
VERIFY(0x02619BF0, isShown);

static void drawBoth(u32 self, u32 port) {
    u32 a = load<u32>(self + 0x38);
    call_ptr<void>(load<u32>(load<u32>(a + 4) + 0x6C), a, port);
    if (isShown(self)) {
        a = load<u32>(self + 0x3C);
        call_ptr<void>(load<u32>(load<u32>(a + 4) + 0x6C), a, port);
    }
}
/* 02619C1C / 02619C90 / 02619D04: draw on the TV, on the DRC, layouts only */
void drawTV(u32 self, u32 port) { WWHD_FUNC(0x02619C1C, void, self, port); drawBoth(self, port); call<void>(0x0286D2F4); }
VERIFY(0x02619C1C, drawTV);
void drawDRC(u32 self, u32 port) { WWHD_FUNC(0x02619C90, void, self, port); drawBoth(self, port); call<void>(0x0286D338); }
VERIFY(0x02619C90, drawDRC);
void drawLayouts(u32 self, u32 port) { WWHD_FUNC(0x02619D04, void, self, port); drawBoth(self, port); }
VERIFY(0x02619D04, drawLayouts);
void drawKbTV() { WWHD_FUNC(0x02619D74, void); call<void>(0x0286D2F4); }
VERIFY(0x02619D74, drawKbTV);
void drawKbDRC() { WWHD_FUNC(0x02619D78, void); call<void>(0x0286D338); }
VERIFY(0x02619D78, drawKbDRC);

/* 02619D7C: opens the input form for the name buffer */
void open(u32 self) {
    WWHD_FUNC(0x02619D7C, void, self);
    call<void>(0x02673344, load<u32>(self + 0x38), load<u32>(self + 0x1C0));
    call<void>(0x020063C0, load<u32>(self + 0x38) + 0x18, 0x10494EF4u);
    store<u16>(self + 0x10, 0);
    const u32 mode0 = call<u32>(0x02617AE4, load<u32>(0x101F5088));
    const u32 a = load<u32>(self + 0x38);
    store<u32>(self + 0x104, mode0 ? 0u : 4u);
    const u32 m = load<u32>(self + 0x1C0);
    store<u32>(self + 0x1B4, 0xFFFFFFFF);
    store<u32>(self + 0x1BC, 0xFFFFFFFF);
    store<u32>(self + 0x1AC, self + 0x10);
    store<u32>(self + 0x1B0, m == 1 ? 0x11u : 9u);
    store<u32>(self + 0x1A8, a ? a + 0x50 : 0u);
    store<u32>(self + 0x1B8, 0);
    call<void>(0x0286D3E0, self + 0x100);
}
VERIFY(0x02619D7C, open);

/* 02619E84: closes the input form */
void close(u32 self) {
    WWHD_FUNC(0x02619E84, void, self);
    call<void>(0x020063C0, load<u32>(self + 0x38) + 0x18, 0x10494F24u);
    call<void>(0x0286D400, self + 0x110);
    call<void>(0x0286D3F0);
}
VERIFY(0x02619E84, close);

u32 disappearA() { WWHD_FUNC(0x02619ECC, u32); return call<u32>(0x0286D3D0, 0u); }
VERIFY(0x02619ECC, disappearA);
u32 disappearB() { WWHD_FUNC(0x02619ED4, u32); return call<u32>(0x0286D3C0, 0u); }
VERIFY(0x02619ED4, disappearB);
u32 getText(u32 self) { WWHD_FUNC(0x02619EDC, u32, self); return self + 0x10; }
VERIFY(0x02619EDC, getText);
void layoutCall(u32 self, u32 a) { WWHD_FUNC(0x02619EE4, void, self, a); call<void>(0x026733AC, load<u32>(self + 0x38), a); }
VERIFY(0x02619EE4, layoutCall);

/* 02619EEC: destroys the layouts, the keyboard and its work memory */
void destroy(u32 self) {
    WWHD_FUNC(0x02619EEC, void, self);
    for (u32 off = 0x38; off <= 0x3C; off += 4) {
        u32 a = load<u32>(self + off);
        if (!a) continue;
        call_ptr<void>(load<u32>(load<u32>(a + 4) + 0x54), a);
        a = load<u32>(self + off);
        if (a) call_ptr<void>(load<u32>(load<u32>(a + 4) + 0xC), a, 3u);
        store<u32>(self + off, 0);
    }
    call<void>(0x0286D37C);
    call<void>(0x0273AFC8, load<u32>(self + 0x34));
    store<u32>(self + 0x34, 0);
}
VERIFY(0x02619EEC, destroy);

/* 02619FB0: passes a parameter pair to both layouts */
void setBoth(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x02619FB0, void, self, a, b);
    call<void>(0x02673414, load<u32>(self + 0x38), a, b);
    call<void>(0x02673CCC, load<u32>(self + 0x3C), a, b);
}
VERIFY(0x02619FB0, setBoth);

/* 0261A008: keyboard hidden (state 0)? */
u32 isHidden(u32 self) {
    WWHD_FUNC(0x0261A008, u32, self);
    return call<u32>(0x0286D270) == 0;
}
VERIFY(0x0261A008, isHidden);

/* 0261A030: SingletonDisposer deleting destructor (deletes the instance) */
void disposerDtor(u32 p, u32 flags) {
    WWHD_FUNC(0x0261A030, void, p, flags);
    if (!p) return;
    store<u32>(p + 0xC, 0x100E2AF8);
    if (p == load<u32>(0x101F50DC)) {
        const u32 inst = load<u32>(kInstance);
        store<u32>(0x101F50DC, 0);
        call_ptr<void>(load<u32>(load<u32>(inst + 0x1C4) + 0xC), inst, 2u);
        store<u32>(kInstance, 0);
    }
    call<void>(0x02752BEC, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x0261A030, disposerDtor);

/* 0261A0D8: static initialiser */
void staticInit() {
    WWHD_FUNC(0x0261A0D8, void);
    const u32 b = 0x1048DEAC;
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, 0x101F50B4u);
    const f32 lo = load<f32>(0x100E2AEC), hi = load<f32>(0x100E2AF0);
    store<f32>(0x1048DEA0, lo);
    store<f32>(0x1048DEA4, hi);
    call<void>(0x028ED6F8, 0x1048DEA8u);
    call<void>(0x028F026C, 0x101F50C0u);
    call<void>(0x028EAB2C, 0x1048DEA9u);
    call<void>(0x028F026C, 0x101F50CCu);
}
VERIFY(0x0261A0D8, staticInit);

/* 0261A184: SwkbdMgr deleting destructor (trivial) */
void dtor(u32 p, u32 flags) {
    WWHD_FUNC(0x0261A184, void, p, flags);
    if (!p) return;
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x0261A184, dtor);

} // namespace hd_swkbd_mgr

/* hd_input_ctrl: HD controller manager (singleton 101F5088: TV/GamePad/Pro controller mode, pointer,
 * GamePad orientation) and its pointer-controller object, WWHD. HD-only code (no GameCube source): written from the WWHD code.
 * Range 02617590..0261921F (sinits 02618840 and 0261918C). Not verified here (sead::Matrix33 library
 * template instances): 02617590 (setBase column), 026175B4 (setBase), 02618944 (copy),
 * 02618990 (setMul), 02618B04 (makeIdentity), 02618B3C (setInverse).
 *
 * ControllerMgr (0x270): base 020011CC, +4 vtable 100E29BC, +0x18 pointer controller (0x1B8, ctor
 * 02618E74: +0x13C/+0x170 vtables, +0x194/+0x198 pointer (int), +0x19C/+0x1A0 pointer (float),
 * +0x1A4..+0x1B0 calibration, +0x1B4 controller type), +0x1D0 mode (0 TV, 1/2 GamePad, 3 both),
 * +0x1D4 cursor object (0261A198), +0x1D8 GamePad orientation (3 Vec3 + matrix +0x1FC/+0x220
 * calibration / +0x224 / +0x248 inverse), +0x26C u8 pointer enabled, +0x26D u8 recalibration timer.
 * Controllers: sead::ControllerMgr 101F8AE8 (by id 0273CB8C, by order 0273CB44).
 */
#include "gabi.h"
using namespace gabi;

namespace hd_input_ctrl {

static const u32 kCtrlMgr = 0x101F8AE8;
static const u32 kInstance = 0x101F5088;

/* sead RTTI dynamic cast with the type info initialised on first use (vtable at vtOff) */
static void typeInit(u32 guard, u32 info, u32 base) {
    if (!load<u32>(guard)) {
        store<u32>(guard, 1);
        store<u32>(info, base);
    }
}
static bool isA(u32 obj, u32 vtOff, u32 info) {
    return call_ptr<u32>(load<u32>(load<u32>(obj + vtOff) + 0xC), obj, info) != 0;
}
/* the DRC (GamePad) type: guard 101FD698, info 101FD978 */
static u32 castDrc(u32 c) {
    typeInit(0x101FD698, 0x101FD978, 0x100E29AC);
    if (!c || !isA(c, 0x10, 0x101FD978) || !c) return 0;
    return c;
}
static bool drcConnected(u32 c) {
    return s32(load<u32>(c + 0x2018)) > 0 && load<u8>(c + 0x1118 + 0x5C) == 0x1F;
}
/* the controller object cast (guard 101FD66C, info 101FD774, vtable at +0x158) */
static u32 castCtrl(u32 c) {
    typeInit(0x101FD66C, 0x101FDD74, 0x100E299C);
    return isA(c, 0x158, 0x101FDD74) ? c : 0;
}
static void setupPointer(u32 self, u32 ctrl, u32 a) {
    call<void>(0x0273D248, self + 0x18, ctrl, a);
    call<void>(0x0273BDF4, self + 0x18, 0xFF0000u, 0xFu, 5u);
}

/* 02617620: constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x02617620, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x270u);
        if (!p) return 0;
    }
    call<void>(0x020011CC, p, p);
    store<u32>(p + 4, 0x100E29BC);
    call<void>(0x02618E74, p + 0x18);
    store<u32>(p + 0x1D0, 4);
    store<u32>(p + 0x1D4, 0);
    const u32 src[3] = {0x104A0934, 0x104A0940, 0x104A094C};
    for (u32 v = 0; v < 3; v++)
        for (u32 k = 0; k < 3; k++) store<u32>(p + 0x1D8 + 12 * v + 4 * k, load<u32>(src[v] + 4 * k));
    for (u32 k = 0; k < 9; k++) store<u32>(p + 0x1FC + 4 * k, load<u32>(0x104A03C8 + 4 * k));
    store<u32>(p + 0x220, 0);
    for (u32 k = 0; k < 9; k++) store<u32>(p + 0x224 + 4 * k, load<u32>(0x104A03C8 + 4 * k));
    for (u32 k = 0; k < 9; k++) store<u32>(p + 0x248 + 4 * k, load<u32>(0x104A03C8 + 4 * k));
    const f32 w = load<f32>(0x100E29D0), h = load<f32>(0x100E29D4);
    store<u8>(p + 0x26D, 0);
    store<u8>(p + 0x26C, 1);
    call<void>(0x02001924, p + 0x18, load<f32>(0x10143588), load<f32>(0x1014358C), w, h);
    store<u32>(p, p + 0x18);
    return p;
}
VERIFY(0x02617620, ctor);

/* 026177B8: creates the singleton with its disposer */
void createInstance(u32 heap) {
    WWHD_FUNC(0x026177B8, void, heap);
    if (load<u32>(kInstance)) return;
    const u32 p = call<u32>(0x0273B0D4, 0x270u, heap, 4u);
    const u32 d = p + 8;
    if (d) {
        call<void>(0x02752B0C, d, heap, 3u);
        store<u32>(d + 0xC, 0x100E29E0);
    }
    store<u32>(0x101F508C, d);
    store<u32>(kInstance, p ? ctor(p) : 0u);
}
VERIFY(0x026177B8, createInstance);

/* 02617858: initial setup: GamePad + Pro controllers, the pointer and the cursor object */
void init(u32 self, u32 unused, u32 heap) {
    WWHD_FUNC(0x02617858, void, self, unused, heap);
    store<u32>(self + 0x1D0, 3);
    const u32 mgr = load<u32>(kCtrlMgr);
    u32 main = 0;
    if (mgr) {
        main = call<u32>(0x0273CB44, mgr, 9u, 0u);
        if (main) {
            const u32 pad = call<u32>(0x0273CB44, mgr, 8u, 0u);
            if (pad) call<void>(0x0273D394, castCtrl(main), pad);
            const u32 c = castDrc(call<u32>(0x0273CB8C, load<u32>(kCtrlMgr), 7u));
            if (c) {
                const u32 base = c + 0x1118;
                for (u32 i = 0; i < 4; i++) {
                    const u32 e = i < 4 ? base + 0xF08 * i : base;
                    if (s32(load<u32>(e + 0xF00)) <= 0 || load<u8>(e + 0x5C) != 0x1F) continue;
                    const u32 drc = call<u32>(0x0273CB44, mgr, 7u, i);
                    if (!drc) continue;
                    call<void>(0x0273D394, castCtrl(main), drc);
                    setupPointer(self, main, 1);
                    u32 cur = call<u32>(0x0273B050, 0x5Cu, heap, 4u);
                    if (cur) cur = call<u32>(0x0261A198, cur);
                    store<u32>(self + 0x1D4, cur);
                    store<u32>(self + 0x220, self + 0x1FC);
                    return;
                }
            }
        }
    }
    setupPointer(self, main, 1);
    u32 cur = call<u32>(0x0273B050, 0x5Cu, heap, 4u);
    if (cur) cur = call<u32>(0x0261A198, cur);
    store<u32>(self + 0x1D4, cur);
    store<u32>(self + 0x220, self + 0x1FC);
}
VERIFY(0x02617858, init);

/* 02617AE4: mode 0 (TV)? */
u32 isMode0(u32 self) {
    WWHD_FUNC(0x02617AE4, u32, self);
    return load<u32>(self + 0x1D0) == 0;
}
VERIFY(0x02617AE4, isMode0);

/* 02617AF4: per-frame update */
void calc(u32 self) {
    WWHD_FUNC(0x02617AF4, void, self);
    call<void>(0xC00083C0, 1u);
    call<void>(0xC00083C0, 2u);
    call<void>(0xC00083C0, 3u);
    const s32 mode = s32(load<u32>(self + 0x1D0));
    bool tail = true;
    if (mode < 4) {
        if (mode == 0 || u32(mode) < 3) {
            if (mode != 0) call<u32>(0x0273CB44, load<u32>(kCtrlMgr), 8u, 0u);
            const u32 mgr = load<u32>(kCtrlMgr);
            const u32 c = castDrc(call<u32>(0x0273CB8C, mgr, 7u));
            if (c && s32(load<u32>(c + 0x2018)) > 0) {
                const u32 b = load<u8>(c + 0x1118 + 0x5C);
                if (b == 0x1F) {
                    if (call<u32>(0x0273CB44, mgr, 7u, 0u) && !load<u8>(self + 0x15C)) {
                        u32 n = load<u8>(self + 0x26D);
                        if (n > 15) {
                            store<u8>(self + 0x26D, 0);
                            call<void>(0x0273D2C0, self + 0x18, 1u);
                            n = load<u8>(self + 0x26D);
                        }
                        store<u8>(self + 0x26D, u8(n + 1));
                    }
                } else if (b != 0xFF && b != 0xFC) {
                    call<void>(0xC00083C0, 0u);
                }
            }
        } else if (mode == 3) {
            const u32 mgr = load<u32>(kCtrlMgr);
            const u32 main = call<u32>(0x0273CB44, mgr, 9u, 0u);
            typeInit(0x101FD66C, 0x101FDD74, 0x100E299C);
            if (main && isA(main, 0x158, 0x101FDD74)) call<void>(0x0273D42C, main);
            else call<void>(0x0273D42C, 0u);
            if (main) {
                const u32 pad = call<u32>(0x0273CB44, mgr, 8u, 0u);
                if (pad) call<void>(0x0273D394, castCtrl(main), pad);
                const u32 c = castDrc(call<u32>(0x0273CB8C, mgr, 7u));
                if (c && drcConnected(c)) {
                    const u32 drc = call<u32>(0x0273CB44, mgr, 7u, 0u);
                    if (drc) {
                        call<void>(0x0273D2C0, self + 0x18, 1u);
                        call<void>(0x0273D394, castCtrl(main), drc);
                    }
                }
            }
        } else {
            tail = true; /* negative mode (unsigned compare): orientation only */
        }
    }
    call<void>(0x026173B0, self + 0x1D8);
    if (isMode0(self)) return;
    (void)tail;
    Local<f32[2]> pos;
    const f32 z = load<f32>(0x100E2984);
    store<f32>(pos.a, z);
    store<f32>(pos.a + 4, z);
    if (load<u32>(self + 0x124) & 0x8000) {
        const u32 valid = load<u32>(self + 0x24) & 1;
        store<f32>(pos.a, load<f32>(valid ? self + 0x1B4 : 0x1049FFA0));
        store<f32>(pos.a + 4, load<f32>((valid ? self + 0x1B4 : 0x1049FFA0) + 4));
    }
    call<void>(0x0261A9B8, load<u32>(self + 0x1D4), pos.a);
}
VERIFY(0x02617AF4, calc);

/* 02617F14: is the GamePad (controller 8) connected? */
u32 hasPad(u32 self) {
    WWHD_FUNC(0x02617F14, u32, self);
    const u32 c = call<u32>(0x0273CB8C, load<u32>(kCtrlMgr), 8u);
    typeInit(0x101FD694, 0x101FD968, 0x100E29AC);
    if (!c || !isA(c, 0x10, 0x101FD968) || !c) return 0;
    return s32(load<u32>(c + 0xAD4)) > 0;
}
VERIFY(0x02617F14, hasPad);

/* 02617FCC: is the DRC connected? */
u32 hasDrc(u32 self) {
    WWHD_FUNC(0x02617FCC, u32, self);
    const u32 c = castDrc(call<u32>(0x0273CB8C, load<u32>(kCtrlMgr), 7u));
    return c && drcConnected(c);
}
VERIFY(0x02617FCC, hasDrc);

/* 02618094: switches the controller mode */
u32 setMode(u32 self, u32 mode) {
    WWHD_FUNC(0x02618094, u32, self, mode);
    if (s32(mode) >= 4) return 0;
    if (mode != 0 && mode < 3) {
        const u32 pad = call<u32>(0x0273CB44, load<u32>(kCtrlMgr), 8u, 0u);
        if (!pad) return 0;
        setupPointer(self, pad, 1);
        store<u32>(self + 0x1D0, mode);
        call<void>(0x02030AD8, 0u);
        return 1;
    }
    if (mode == 0) {
        const u32 mgr = load<u32>(kCtrlMgr);
        const u32 c = castDrc(call<u32>(0x0273CB8C, mgr, 7u));
        if (!c || !drcConnected(c)) return 0;
        const u32 drc = call<u32>(0x0273CB44, mgr, 7u, 0u);
        if (!drc) return 0;
        setupPointer(self, drc, 1);
        store<u32>(self + 0x1D0, mode);
        call<void>(0x02030AD8, 1u);
        return 1;
    }
    if (mode != 3) return 0;
    u32 done = 0;
    const u32 mgr = load<u32>(kCtrlMgr);
    const u32 main = call<u32>(0x0273CB44, mgr, 9u, 0u);
    typeInit(0x101FD66C, 0x101FDD74, 0x100E299C);
    if (main && isA(main, 0x158, 0x101FDD74)) call<void>(0x0273D42C, main);
    else call<void>(0x0273D42C, 0u);
    if (!main) return 0;
    const u32 pad = call<u32>(0x0273CB44, mgr, 8u, 0u);
    if (pad) {
        call<void>(0x0273D394, castCtrl(main), pad);
        done = 1;
        store<u32>(self + 0x1D0, mode);
    }
    const u32 c = castDrc(call<u32>(0x0273CB8C, mgr, 7u));
    if (c && drcConnected(c)) {
        const u32 drc = call<u32>(0x0273CB44, mgr, 7u, 0u);
        if (drc) {
            call<void>(0x0273D394, castCtrl(main), drc);
            store<u32>(self + 0x1D0, mode);
            setupPointer(self, main, 1);
            return 1;
        }
    }
    if (done) setupPointer(self, main, 1);
    return done;
}
VERIFY(0x02618094, setMode);

u32 isMode1(u32 self) { WWHD_FUNC(0x02618498, u32, self); return load<u32>(self + 0x1D0) == 1; }
VERIFY(0x02618498, isMode1);
u32 isMode2(u32 self) { WWHD_FUNC(0x026184AC, u32, self); return load<u32>(self + 0x1D0) == 2; }
VERIFY(0x026184AC, isMode2);
u32 isMode3(u32 self) { WWHD_FUNC(0x026184C0, u32, self); return load<u32>(self + 0x1D0) == 3; }
VERIFY(0x026184C0, isMode3);

/* 026184D4: are none of the buttons in mask held on the active controllers? */
u32 isFree(u32 self, u32 mask) {
    WWHD_FUNC(0x026184D4, u32, self, mask);
    const u32 mgr = load<u32>(kCtrlMgr);
    if (hasPad(self)) {
        const u32 c = call<u32>(0x0273CB44, mgr, 8u, 0u);
        if (c && (mask & load<u32>(c))) return 1;
    }
    if (!hasDrc(self)) return 1;
    const u32 c = call<u32>(0x0273CB44, mgr, 7u, 0u);
    if (!c || !(mask & load<u32>(c))) return 1;
    return 0;
}
VERIFY(0x026184D4, isFree);

/* 026185A0: cursor state (5 without a cursor) */
u32 cursorState(u32 self) {
    WWHD_FUNC(0x026185A0, u32, self);
    const u32 c = load<u32>(self + 0x1D4);
    return c ? load<u32>(c + 4) : 5;
}
VERIFY(0x026185A0, cursorState);

/* 026185BC: takes the current orientation as the calibration (and its inverse) */
void calibrate(u32 self) {
    WWHD_FUNC(0x026185BC, void, self);
    const u32 src = load<u32>(self + 0x220);
    if (!src) return;
    call<void>(0x02618944, self + 0x224, src);
    call<void>(0x02618B3C, self + 0x248, self + 0x224);
}
VERIFY(0x026185BC, calibrate);

/* 02618604: the calibrated GamePad orientation (identity on the TV or when gyro aiming is off) */
void orientation(u32 out, u32 self) {
    WWHD_FUNC(0x02618604, void, out, self);
    const u32 opt = call<u32>(0x027200D0, load<u32>(0x101F84DC) + 0x12C0);
    if (!call<u32>(0x0271FC30, opt) || isMode0(self)) {
        for (u32 k = 0; k < 9; k++) store<u32>(out + 4 * k, load<u32>(0x104A03C8 + 4 * k));
        return;
    }
    Local<u32[9]> m;
    for (u32 k = 0; k < 9; k++) store<u32>(m.a + 4 * k, load<u32>(0x104A03C8 + 4 * k));
    const u32 cal = load<u32>(self + 0x220);
    if (cal) call<void>(0x02618990, m.a, self + 0x248, cal);
    for (u32 k = 0; k < 9; k++) store<u32>(out + 4 * k, load<u32>(m.a + 4 * k));
}
VERIFY(0x02618604, orientation);

/* 02618720: resets the pointer */
void resetPointer(u32 self, u32 arg) {
    WWHD_FUNC(0x02618720, void, self, arg);
    call<void>(0x0273D2C0, self + 0x18, arg);
    call<void>(0x0273D2C8, self + 0x18);
    store<u8>(self + 0x26D, 0);
}
VERIFY(0x02618720, resetPointer);

/* 02618760..0261878C: pointer parameters */
void pointerParams(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x02618760, void, self, a, b);
    call<void>(0x0273BDF4, self + 0x18, 0xFF0000u, a, b);
}
VERIFY(0x02618760, pointerParams);
void pointerDefault(u32 self) {
    WWHD_FUNC(0x02618774, void, self);
    pointerParams(self, 0xF, 5);
}
VERIFY(0x02618774, pointerDefault);
void pointerParamsMasked(u32 self, u32 a, u32 b, u32 c) {
    WWHD_FUNC(0x02618780, void, self, a, b, c);
    call<void>(0x0273BDF4, self + 0x18, a & 0xFF00FFFFu, b, c);
}
VERIFY(0x02618780, pointerParamsMasked);
void pointerSlow(u32 self, u32 a) {
    WWHD_FUNC(0x0261878C, void, self, a);
    pointerParamsMasked(self, a, 0x1E, 1);
}
VERIFY(0x0261878C, pointerSlow);

/* 02618798: SingletonDisposer deleting destructor (deletes the instance) */
void disposerDtor(u32 p, u32 flags) {
    WWHD_FUNC(0x02618798, void, p, flags);
    if (!p) return;
    store<u32>(p + 0xC, 0x100E29E0);
    if (p == load<u32>(0x101F508C)) {
        const u32 inst = load<u32>(kInstance);
        store<u32>(0x101F508C, 0);
        call_ptr<void>(load<u32>(load<u32>(inst + 4) + 0xC), inst, 2u);
        store<u32>(kInstance, 0);
    }
    call<void>(0x02752BEC, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x02618798, disposerDtor);

static void stdInit(u32 b, u32 r1, u32 lo, u32 hi, u32 f, u32 r2, u32 r3) {
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, r1);
    const f32 a = load<f32>(lo), c = load<f32>(hi);
    store<f32>(f, a);
    store<f32>(f + 4, c);
    call<void>(0x028ED6F8, f + 8);
    call<void>(0x028F026C, r2);
    call<void>(0x028EAB2C, f + 9);
    call<void>(0x028F026C, r3);
}
void staticInit() {
    WWHD_FUNC(0x02618840, void);
    stdInit(0x1048DE74, 0x101F5064, 0x100E29D8, 0x100E29DC, 0x1048DE68, 0x101F5070, 0x101F507C);
}
VERIFY(0x02618840, staticInit);

/* 026188D4: manager deleting destructor */
void dtor(u32 p, u32 flags) {
    WWHD_FUNC(0x026188D4, void, p, flags);
    if (!p) return;
    store<u32>(p + 0x154, 0x100E2A78);
    call<void>(0x02001728, p + 0x18, 0u);
    call<void>(0x020011B8, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x026188D4, dtor);

/* 02618940: empty function */
void nop() { WWHD_FUNC(0x02618940, void); }
VERIFY(0x02618940, nop);

/* 02618D10: thunk of the second base (this - 0x130) */
void thunk(u32 p, u32 a) {
    WWHD_FUNC(0x02618D10, void, p, a);
    call<void>(0x02619220, p - 0x130, a);
}
VERIFY(0x02618D10, thunk);

/* 02618D18: pointer controller type (4 when both controllers are active) */
u32 controllerType(u32 self) {
    WWHD_FUNC(0x02618D18, u32, self);
    if (isMode0(load<u32>(kInstance))) {
        if (!hasDrc(load<u32>(kInstance))) return load<u32>(self + 0x1B4);
        const u32 r = call<u32>(0xC0008420, 0u);
        store<u32>(self + 0x1B4, r);
        return r;
    }
    if (isMode3(load<u32>(kInstance))) return 4;
    if (!hasPad(load<u32>(kInstance))) return load<u32>(self + 0x1B4);
    u32 c = call<u32>(0x0273CB8C, load<u32>(kCtrlMgr), 8u);
    typeInit(0x101FD694, 0x101FD968, 0x100E2A00);
    if (!c || !isA(c, 0x10, 0x101FD968)) c = 0;
    if (s32(load<u32>(c + 0xAD4)) <= 0) return load<u32>(self + 0x1B4);
    const u32 r = load<u32>(0x100E2A10 + 4 * load<u8>(c + 0x14 + 0xA1));
    store<u32>(self + 0x1B4, r);
    return r;
}
VERIFY(0x02618D18, controllerType);

/* 02618E74: pointer controller constructor */
u32 pointerCtor(u32 p) {
    WWHD_FUNC(0x02618E74, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x1B8u);
        if (!p) return 0;
    }
    call<void>(0x02001790, p);
    store<u32>(p + 0x170, 0x100E2A38);
    store<u32>(p + 0x13C, 0x100E2A78);
    store<u32>(p + 0x1B4, 4);
    return p;
}
VERIFY(0x02618E74, pointerCtor);

static void setBit(u32 dst, bool on, u32 bit) {
    const u32 v = load<u32>(dst);
    store<u32>(dst, on ? v | bit : v & ~bit);
}

/* the pointer position: calibration offset and scale, integer copy */
static void pointerPos(u32 self, u32 flags) {
    const f32 c8 = load<f32>(self + 0x1A8), y = load<f32>(self + 0x1A0), c4 = load<f32>(self + 0x1A4);
    const f32 dy = fsubs_ppc(y, c8);
    const f32 x = load<f32>(self + 0x19C), s0 = load<f32>(self + 0x1B0);
    const f32 dx = fsubs_ppc(x, c4);
    const f32 sx = load<f32>(self + 0x1AC);
    const f32 py = fmuls_ppc(dy, s0);
    const f32 px = fmuls_ppc(dx, sx);
    store<u32>(self + 0xC, flags);
    store<f32>(self + 0x19C, px);
    store<u32>(self + 0x194, u32(ftoi(px)));
    store<u32>(self + 0x198, u32(ftoi(py)));
    store<f32>(self + 0x1A0, py);
}

/* 02618EDC: pointer update: the GamePad touch as pointer when enabled, else cleared */
void pointerUpdate(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x02618EDC, void, self, a, b);
    call<void>(0x0200188C, self, a, b);
    const u32 pad = call<u32>(0x0273CB44, load<u32>(kCtrlMgr), 8u, 0u);
    const u32 m = load<u32>(kInstance);
    if (load<u8>(m + 0x26C) && hasPad(m) && pad) {
        store<f32>(self + 0x19C, load<f32>(pad + 0x110));
        store<f32>(self + 0x1A0, load<f32>(pad + 0x114));
        setBit(self + 0x10C, load<u32>(pad + 0x10C) & 0x8000, 0x8000);
        setBit(self, load<u32>(pad) & 0x8000, 0x8000);
        setBit(self + 4, load<u32>(pad + 4) & 0x8000, 0x8000);
        setBit(self + 0xC, load<u32>(pad + 0xC) & 1, 1);
        setBit(self + 0xC, load<u32>(pad + 0xC) & 2, 2);
        const u32 f = load<u32>(self + 0xC);
        if (load<u32>(pad + 0xC) & 4) {
            pointerPos(self, f | 4);
            return;
        }
        pointerPos(self, f & ~4u);
        return;
    }
    const u32 h = load<u32>(self + 0x10C);
    const f32 x = load<f32>(0x1049FFA0);
    const u32 w0 = load<u32>(self);
    store<f32>(self + 0x19C, x);
    const f32 y = load<f32>(0x1049FFA4);
    store<u32>(self + 0x10C, h & ~0x8000u);
    store<f32>(self + 0x1A0, y);
    const u32 w4 = load<u32>(self + 4);
    const u32 fl = load<u32>(self + 0xC);
    store<u32>(self, w0 & ~0x8000u);
    store<u32>(self + 4, w4 & ~0x8000u);
    pointerPos(self, fl & ~7u);
}
VERIFY(0x02618EDC, pointerUpdate);

void staticInit2() {
    WWHD_FUNC(0x0261918C, void);
    stdInit(0x1048DE90, 0x101F5090, 0x100E2A2C, 0x100E2A30, 0x1048DE84, 0x101F509C, 0x101F50A8);
}
VERIFY(0x0261918C, staticInit2);

} // namespace hd_input_ctrl

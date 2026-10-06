/* hd_state_task: HD background task with a worker thread (states Wait / InProgress / Finish; started by
 * an 'updt' message), WWHD. HD-only code (no GameCube source):
 * written from the WWHD code. Range 025F9644..025FA0EF (static initialiser 025F9CD0 builds the three
 * StateIDs). Library companions (not verified): 025F9EE4..025F9EF8, 025F9F58..025F9F60 (empty sead
 * virtuals), 025F9EFC (sead Delegate invoke), 025F9F50 (StateID getter), 025F9F64, 025F9FA4, 025F9FE4
 * (state Delegate invokers: enter/exec/exit), 025FA024, 025FA050, 025FA05C, 025FA068, 025FA070,
 * 025FA08C, 025FA0A8 (sead state-machine glue).
 *
 * Task (0x15B0), singleton 101F4BD8 (disposer at +0x28, holder 101F4BDC; the disposer's vtable
 * 100E0EF8): +0 base (vtables 100E0E00/100E0DC0, +8 self, +0x10 state machine 020062E8 with factory
 * 101FF32C, +0x24 vtable), +0x18/+0x20 current/next state, +0x38 heap, +0x3C message queue,
 * +0x64 thread, +0x1588 worker object, +0x158C shared data, +0x1590 'updt' message Delegate
 * (vtable 100E0DB0, handler 025F9BC4), +0x15A0 object, +0x15A4 progress (0 idle, 1 working,
 * 2 done), +0x15A8 flag, +0x15AC busy counter.
 */
#include "gabi.h"
using namespace gabi;

namespace hd_state_task {

static const u32 kInstance = 0x101F4BD8;

void clearFlag(u32 p) { WWHD_FUNC(0x025F9644, void, p); store<u8>(p + 0x15A8, 0); }
VERIFY(0x025F9644, clearFlag);

/* 025F9650: not working (progress != 1) */
u32 isIdle(u32 p) {
    WWHD_FUNC(0x025F9650, u32, p);
    return load<u32>(p + 0x15A4) != 1 ? 1u : 0u;
}
VERIFY(0x025F9650, isIdle);

/* 025F9664: per-frame: when idle, runs the thread step and the state machine (state Finish) */
void update(u32 p) {
    WWHD_FUNC(0x025F9664, void, p);
    if (!isIdle(p)) return;
    store<u32>(p + 0x15AC, load<u32>(p + 0x15AC) + 1);
    call<void>(0x028B3E8C, p + 0x64);
    store<u32>(p + 0x15AC, load<u32>(p + 0x15AC) - 1);
    call<void>(0x020063C0, p + 0x10, 0x1048D834u);
}
VERIFY(0x025F9664, update);

void setFlag(u32 p) { WWHD_FUNC(0x025F96E0, void, p); store<u8>(p + 0x15A8, 1); }
VERIFY(0x025F96E0, setFlag);

/* 025F96EC: constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x025F96EC, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x15B0u);
        if (!p) return 0;
    }
    u32 q = p;
    if (!q) q = call<u32>(0x0273AD10, 0x28u);
    if (q) {
        store<u32>(q + 8, p);
        store<u32>(q + 0xC, 0);
        store<u32>(q + 4, 0x100E0DC0);
        store<u32>(q, 0x100E0E00);
        store<u32>(q + 0x24, 0x100E0DA0);
        call<void>(0x020062E8, q + 0x10, q, 0x101FF32Cu);
    }
    store<u32>(p + 0x38, 0);
    store<u32>(p + 0x24, 0x100E0F08);
    call<void>(0x028B2204, p + 0x3C);
    call<void>(0x028B28F8, p + 0x64);
    store<u32>(p + 0x1588, 0);
    store<u32>(p + 0x158C, 0);
    u32 d = p + 0x1590;
    if (!d) d = call<u32>(0x0273AD10, 0x10u);
    if (d) {
        store<u32>(d + 4, p);
        store<u16>(d + 8, 0);
        store<u32>(d, 0x100E0DB0);
        store<u32>(d + 0xC, 0x025F9BC4);
        store<u16>(d + 0xA, 0xFFFF);
    }
    store<u32>(p + 0x15A4, 0);
    store<u8>(p + 0x15A8, 0);
    store<u32>(p + 0x15A0, 0);
    u32 c = p + 0x15AC;
    if (!c) {
        c = call<u32>(0x0273AD10, 4u);
        if (!c) return p;
    }
    store<u32>(c, 0);
    return p;
}
VERIFY(0x025F96EC, ctor);

/* 025F9818: creates the singleton with its disposer */
void createInstance(u32 heap) {
    WWHD_FUNC(0x025F9818, void, heap);
    if (load<u32>(kInstance)) return;
    const u32 p = call<u32>(0x0273B0D4, 0x15B0u, heap, 4u);
    const u32 d = p + 0x28;
    if (d) {
        call<void>(0x02752B0C, d, heap, 3u);
        store<u32>(d + 0xC, 0x100E0EF8);
    }
    store<u32>(0x101F4BDC, d);
    store<u32>(kInstance, p ? ctor(p) : 0u);
}
VERIFY(0x025F9818, createInstance);

/* 025F98B8: destructor */
void dtor(u32 p, u32 flags) {
    WWHD_FUNC(0x025F98B8, void, p, flags);
    if (!p) return;
    const u32 w = load<u32>(p + 0x1588);
    store<u32>(p + 0x24, 0x100E0F08);
    call<void>(0x028B3124, p + 0x64, w);
    const u32 t = load<u32>(p + 0x1588);
    if (t) {
        call_ptr<void>(load<u32>(load<u32>(t) + 0x14), t, 3u);
        store<u32>(p + 0x1588, 0);
    }
    const u32 u = load<u32>(p + 0x15A0);
    if (u) {
        call_ptr<void>(load<u32>(load<u32>(u + 0xC) + 0xC), u, 3u);
        store<u32>(p + 0x15A0, 0);
    }
    call<void>(0x028B2A40, p + 0x64, 2u);
    call<void>(0x028B229C, p + 0x3C, 2u);
    if (p) {
        u32 o = load<u32>(p + 0x10);
        call_ptr<void>(load<u32>(load<u32>(o) + 0x1C), o, p + 0x20);
        o = load<u32>(p + 0x10);
        call_ptr<void>(load<u32>(load<u32>(o) + 0x1C), o, p + 0x18);
    }
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x025F98B8, dtor);

/* 025F99BC: starts the worker thread (shared data 2 for its stack, message queue) */
void start(u32 p) {
    WWHD_FUNC(0x025F99BC, void, p);
    Local<u8[0x48]> F; /* +0 message (0xC), +0xC shared-data size, +0x10 word, +0x14 thread params */
    const u32 msg = F.a, size = F.a + 0xC, word = F.a + 0x10, prm = F.a + 0x14;
    call<void>(0xC0009C58, 2u, 0u, p + 0x158C, size);
    store<u32>(word, 0);
    call<void>(0x028B4230, prm);
    const u32 w = load<u32>(p + 0x1588);
    store<u32>(prm + 4, 0x800);
    store<u32>(prm + 0xC, p + 0x158C);
    store<u32>(prm + 0x1C, word);
    store<u32>(prm, 0x400);
    store<u32>(prm + 0x20, 1);
    store<u32>(prm + 8, w);
    store<u32>(prm + 0x10, size);
    store<u32>(prm + 0x2C, 0x1400);
    call<void>(0x028B2B64, p + 0x64, prm);
    call<void>(0x028B289C, msg);
    store<u8>(msg + 0xB, 1);
    store<u16>(msg + 8, 0);
    store<u32>(msg, p + 0x64);
    store<u32>(msg + 4, 0x19);
    call<void>(0x028B239C, p + 0x3C, msg);
}
VERIFY(0x025F99BC, start);

/* 025F9A90: creates the worker object, starts the thread and enters state Wait */
void init(u32 p, u32 heap) {
    WWHD_FUNC(0x025F9A90, void, p, heap);
    store<u32>(p + 0x38, heap);
    u32 m = call<u32>(0x0273B050, 8u, heap, 4u);
    if (m) m = call<u32>(0x020039C0, m, load<u32>(p + 0x38));
    store<u32>(p + 0x1588, m);
    start(p);
    const u32 o = load<u32>(p + 0x10);
    const u32 s = call_ptr<u32>(load<u32>(load<u32>(o) + 0x14), o, 0x1048D7D4u);
    store<u32>(p + 0x18, s);
    call_ptr<void>(load<u32>(load<u32>(s) + 0x1C), s);
}
VERIFY(0x025F9A90, init);

/* 025F9B14: runs the state machine */
void execute(u32 p) {
    WWHD_FUNC(0x025F9B14, void, p);
    call<void>(0x02006364, p + 0x10);
}
VERIFY(0x025F9B14, execute);

/* 025F9B1C / 025F9B68: thread requests when idle (the second only when not busy) */
u32 request1(u32 p) {
    WWHD_FUNC(0x025F9B1C, u32, p);
    if (!isIdle(p)) return 0;
    call<void>(0x028B3540, p + 0x64);
    return 1;
}
VERIFY(0x025F9B1C, request1);
u32 request2(u32 p) {
    WWHD_FUNC(0x025F9B68, u32, p);
    if (load<u32>(p + 0x15AC)) return 0;
    if (!isIdle(p)) return 0;
    call<void>(0x028B3804, p + 0x64);
    return 1;
}
VERIFY(0x025F9B68, request2);

/* 025F9BC4: 'updt' message handler (worker side): marks the work in progress, then done */
void onMessage(u32 p, u32 obj, u32 msg) {
    WWHD_FUNC(0x025F9BC4, void, p, obj, msg);
    if (msg != 0x75706474) return;
    store<u32>(p + 0x15A4, 1);
    call<void>(0x028B3804, p + 0x64);
    call<void>(0x0275FFC8, obj);
    store<u32>(p + 0x15A4, 2);
}
VERIFY(0x025F9BC4, onMessage);

/* 025F9C28: disposer destructor (deletes the instance when it is the registered one) */
void disposerDtor(u32 d, u32 flags) {
    WWHD_FUNC(0x025F9C28, void, d, flags);
    if (!d) return;
    store<u32>(d + 0xC, 0x100E0EF8);
    if (d == load<u32>(0x101F4BDC)) {
        const u32 inst = load<u32>(kInstance);
        store<u32>(0x101F4BDC, 0);
        call_ptr<void>(load<u32>(load<u32>(inst + 0x24) + 0xC), inst, 2u);
        store<u32>(kInstance, 0);
    }
    call<void>(0x02752BEC, d, 0u);
    if (flags & 1) call<void>(0x0273AF40, d);
}
VERIFY(0x025F9C28, disposerDtor);

/* a sead StateID with its three state Delegates (enter/exec/exit) */
static void stateId(u32 s, u32 id, u32 name, u16 a, u16 b, u16 c) {
    store<u16>(s + 0xC, 0);
    store<u32>(s, id);
    store<u32>(s + 4, name);
    store<u32>(s + 0x10, 0x24);
    store<u16>(s + 0xE, a);
    store<u16>(s + 0x14, 0);
    store<u16>(s + 0x16, b);
    store<u32>(s + 0x18, 0x24);
    store<u32>(s + 0x20, 0x24);
    store<u32>(s + 0x28, 0);
    store<u16>(s + 0x1C, 0);
    store<u16>(s + 0x1E, c);
    store<u32>(s + 8, 0x100E0E68);
    store<u16>(s + 0x24, 0);
    store<u32>(s + 0x2C, 0x101FF32C);
    store<u16>(s + 0x26, 0);
}

/* 025F9CD0: static initialiser: header statics and StateID_Wait / InProgress / Finish */
void staticInit() {
    WWHD_FUNC(0x025F9CD0, void);
    const u32 b = 0x1048D7C4;
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, 0x101F4BB4u);
    const f32 lo = load<f32>(0x100E0EB0), hi = load<f32>(0x100E0EB4);
    store<f32>(0x1048D7B8, lo);
    store<f32>(0x1048D7BC, hi);
    call<void>(0x028ED6F8, 0x1048D7C0u);
    call<void>(0x028F026C, 0x101F4BC0u);
    call<void>(0x028EAB2C, 0x1048D7C1u);
    call<void>(0x028F026C, 0x101F4BCCu);
    u32 n = 0;
    if (load<u32>(0x101FD8E4)) n = load<u32>(0x101FDD50);
    else store<u32>(0x101FD8E4, 1);
    stateId(0x1048D7D4, n + 1, 0x100E0EB8, 2, 3, 4);
    stateId(0x1048D804, n + 2, 0x100E0EC8, 5, 6, 7);
    store<u32>(0x101FDD50, n + 3);
    stateId(0x1048D834, n + 3, 0x100E0EE0, 8, 9, 0xA);
}
VERIFY(0x025F9CD0, staticInit);

} // namespace hd_state_task

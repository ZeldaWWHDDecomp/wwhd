/* hd_input_eventmgr: cking::ui::input::EventMgr (GamePad touch / pointer event dispatch to UI
 * receivers), WWHD. HD-only code (no GameCube source):
 * written from the WWHD code. Range 02614BE0..02616ED3; static initialiser 02616DF4.
 * Not verified here (sead library): 02616EB8 (SafeString deleting destructor), 02616ECC
 * (SafeString assureTermination).
 *
 * EventMgr (0xE4): base 02000020 (receiver list at +0: TList {+4 first, +8 count}, +0x14 event
 * queue), +0x58 vtable 100E26D0, +0x5C vtable 100E26C0, +0x60 SingletonDisposer (instance
 * 101F4FF0), +0x70/+0x74 mode, +0x78 / +0xC8 parameters, +0x7C focused receiver, +0x80 grabbed
 * receiver, +0x84 hovered receiver, +0x88..+0xC0 touch positions and velocities (Vec2),
 * +0xA8 inertia frames, +0xC4 u8 mode changed, +0xCC/+0xD0 last/current id, +0xD4 inertia length,
 * +0xD8 friction, +0xDC stop speed, +0xE0 margin.
 * Input event: +0 hold, +4 release, +8 trigger, +0xC flags (bit 0: touch valid), +0x10C,
 * +0x118/+0x11C stick, +0x19C/+0x1A0 touch position.
 */
#include "gabi.h"
using namespace gabi;

namespace hd_input_eventmgr {

static const u32 kInstance = 0x101F4FF0;

static f32 zero() { return load<f32>(0x100E2648); }

/* 02614BE0: constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x02614BE0, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0xE4u);
        if (!p) return 0;
    }
    call<void>(0x02000020, p);
    store<u32>(p + 0x7C, 0);
    store<u32>(p + 0x84, 0);
    store<u32>(p + 0x70, 0);
    store<u32>(p + 0x58, 0x100E26D0);
    store<u32>(p + 0x5C, 0x100E26C0);
    store<u32>(p + 0x74, 0);
    const f32 z = zero();
    store<u32>(p + 0x78, 0);
    store<u32>(p + 0x80, 0);
    const u32 vecs[7] = {0x88, 0x90, 0x98, 0xA0, 0xAC, 0xB4, 0xBC};
    for (u32 i = 0; i < 7; i++) {
        if (i == 4) store<u32>(p + 0xA8, 0xFFFFFFFF);
        u32 v = p + vecs[i];
        if (!v) v = call<u32>(0x0273AD10, 8u);
        if (v) {
            store<f32>(v + 4, z);
            store<f32>(v, z);
        }
    }
    store<u32>(p + 0xCC, 0);
    store<u8>(p + 0xC4, 0);
    store<u32>(p + 0xD4, 0x14);
    store<f32>(p + 0xDC, load<f32>(0x100E2650));
    store<u32>(p + 0xD0, 0);
    store<f32>(p + 0xD8, load<f32>(0x100E264C));
    store<u32>(p + 0xC8, 8);
    store<f32>(p + 0xE0, load<f32>(0x100E2654));
    return p;
}
VERIFY(0x02614BE0, ctor);

/* 02614DAC: creates the singleton with its disposer */
void createInstance(u32 heap) {
    WWHD_FUNC(0x02614DAC, void, heap);
    if (load<u32>(kInstance)) return;
    const u32 p = call<u32>(0x0273B0D4, 0xE4u, heap, 4u);
    const u32 d = p + 0x60;
    if (d) {
        call<void>(0x02752B0C, d, heap, 3u);
        store<u32>(d + 0xC, 0x100E26B0);
    }
    store<u32>(0x101F4FF4, d);
    store<u32>(kInstance, p ? ctor(p) : 0u);
}
VERIFY(0x02614DAC, createInstance);

/* 02614E48: destructor */
void dtor(u32 p, u32 flags) {
    WWHD_FUNC(0x02614E48, void, p, flags);
    if (!p) return;
    store<u32>(p + 0x5C, 0x100E26C0);
    call<void>(0x020000B4, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x02614E48, dtor);

/* 02614EA8: prepare: the event heap */
void prepare(u32 self) {
    WWHD_FUNC(0x02614EA8, void, self);
    Local<u32[2]> name;
    const u32 size = call<u32>(0x0275320C, 4u) + 0x238;
    store<u32>(name.a + 4, 0x100E2610);
    store<u32>(name.a, 0x100E2658);
    const u32 parent = call<u32>(0x0203E8E8);
    const u32 heap = call<u32>(0x02753004, size, name.a, parent, 1u, 0u);
    call<void>(0x02000108, self, heap, 0xAu);
    call_ptr<void>(load<u32>(load<u32>(heap + 0xC) + 0x2C), heap);
}
VERIFY(0x02614EA8, prepare);

/* 02614F40: current id becomes the last id */
void commitId(u32 self) {
    WWHD_FUNC(0x02614F40, void, self);
    const u32 cur = load<u32>(self + 0xD0);
    store<u32>(self + 0xD0, 0);
    store<u32>(self + 0xCC, cur);
}
VERIFY(0x02614F40, commitId);

/* 02614F54: sets the mode (flags a change) */
void setMode(u32 self, u32 mode) {
    WWHD_FUNC(0x02614F54, void, self, mode);
    if (load<u32>(self + 0x70) == mode) return;
    store<u32>(self + 0x70, mode);
    store<u32>(self + 0x74, mode);
    store<u8>(self + 0xC4, 1);
}
VERIFY(0x02614F54, setMode);

/* 02614F74: per-frame update */
void calc(u32 self) {
    WWHD_FUNC(0x02614F74, void, self);
    commitId(self);
    const u32 mode = load<u32>(self + 0x74);
    store<u8>(self + 0xC4, 0);
    setMode(self, mode);
    call<void>(0x02000118, self);
    call<void>(0x020002E0, self);
}
VERIFY(0x02614F74, calc);

/* 02614FBC: forwards to the event queue */
void forward(u32 self) {
    WWHD_FUNC(0x02614FBC, void, self);
    call<void>(0x02000110, self);
}
VERIFY(0x02614FBC, forward);

void setFocus(u32 self, u32 v) { WWHD_FUNC(0x02614FC0, void, self, v); store<u32>(self + 0x7C, v); }
VERIFY(0x02614FC0, setFocus);
void setGrab(u32 self, u32 v) { WWHD_FUNC(0x02614FC8, void, self, v); store<u32>(self + 0x80, v); }
VERIFY(0x02614FC8, setGrab);
void setHover(u32 self, u32 v) { WWHD_FUNC(0x02614FD0, void, self, v); store<u32>(self + 0x84, v); }
VERIFY(0x02614FD0, setHover);

/* 02614FD8: reset: clears the state and unlinks every receiver */
void reset(u32 self) {
    WWHD_FUNC(0x02614FD8, void, self);
    store<u32>(self + 0xC, 0);
    setFocus(self, 0);
    setGrab(self, 0);
    setHover(self, 0);
    store<u32>(self + 0xCC, 0);
    const f32 z = zero();
    u32 node = load<u32>(self + 4);
    store<f32>(self + 0xB0, z);
    store<f32>(self + 0xBC, z);
    store<u32>(self + 0xC8, 8);
    store<u32>(self + 0xD0, 0);
    store<f32>(self + 0xA0, z);
    store<f32>(self + 0x8C, z);
    store<f32>(self + 0x90, z);
    store<f32>(self + 0xAC, z);
    store<f32>(self + 0x98, z);
    store<u8>(self + 0xC4, 0);
    store<f32>(self + 0xA4, z);
    store<u32>(self + 0xA8, 0xFFFFFFFF);
    store<f32>(self + 0x88, z);
    store<f32>(self + 0x9C, z);
    store<f32>(self + 0x94, z);
    store<f32>(self + 0xC0, z);
    while (node != self) {
        const u32 n = node;
        const u32 owner = load<u32>(n + 0xC);
        node = load<u32>(node + 4);
        if (owner) {
            store<u32>(n + 0xC, 0);
            call<void>(0x0273B310, n);
            store<u32>(self + 8, load<u32>(self + 8) - 1);
        }
    }
    call<void>(0x0200209C, self + 0x14);
}
VERIFY(0x02614FD8, reset);

/* 026150C0: TList::pushBack (moves the node from its previous list) */
void listPushBack(u32 list, u32 node) {
    WWHD_FUNC(0x026150C0, void, list, node);
    const u32 owner = load<u32>(node + 0xC);
    if (owner) {
        store<u32>(node + 0xC, 0);
        call<void>(0x0273B310, node);
        store<u32>(owner + 8, load<u32>(owner + 8) - 1);
    }
    store<u32>(node + 0xC, list);
    call<void>(0x0273B2F0, list, node);
    store<u32>(list + 8, load<u32>(list + 8) + 1);
}
VERIFY(0x026150C0, listPushBack);

u32 getMode(u32 self) { WWHD_FUNC(0x02615140, u32, self); return load<u32>(self + 0x70); }
VERIFY(0x02615140, getMode);
void setParams(u32 self, u32 a, u32 b) {
    WWHD_FUNC(0x02615148, void, self, a, b);
    store<u32>(self + 0x78, a);
    store<u32>(self + 0xC8, b);
}
VERIFY(0x02615148, setParams);
void setModeDefault(u32 self, u32 mode) {
    WWHD_FUNC(0x02615154, void, self, mode);
    setMode(self, mode);
    setParams(self, 0, 8);
}
VERIFY(0x02615154, setModeDefault);
void setNextMode(u32 self, u32 v) { WWHD_FUNC(0x02615180, void, self, v); store<u32>(self + 0x74, v); }
VERIFY(0x02615180, setNextMode);
u8 isModeChanged(u32 self) { WWHD_FUNC(0x02615188, u8, self); return load<u8>(self + 0xC4); }
VERIFY(0x02615188, isModeChanged);
u32 getParamA(u32 self) { WWHD_FUNC(0x02615190, u32, self); return load<u32>(self + 0x78); }
VERIFY(0x02615190, getParamA);
u32 getParamB(u32 self) { WWHD_FUNC(0x02615198, u32, self); return load<u32>(self + 0xC8); }
VERIFY(0x02615198, getParamB);
u32 getHover(u32 self) { WWHD_FUNC(0x026151A0, u32, self); return load<u32>(self + 0x84); }
VERIFY(0x026151A0, getHover);

/* touch position delta to (bx, by) as two float words; the default vector 104A0908 when invalid */
static Pair32 touchDelta(u32 src, u32 self, u32 bx, u32 by) {
    const u32 wx = load<u32>(src), wy = load<u32>(src + 4);
    const f32 lim = load<f32>(0x100E267C);
    Local<u32[2]> v;
    store<u32>(v.a, wx);
    store<u32>(v.a + 4, wy);
    const f32 x = load<f32>(v.a), y = load<f32>(v.a + 4);
    if (x > lim && y > lim) {
        Local<f32[2]> d;
        store<f32>(d.a, fsubs_ppc(x, load<f32>(self + bx)));
        store<f32>(d.a + 4, fsubs_ppc(y, load<f32>(self + by)));
        return {load<u32>(d.a), load<u32>(d.a + 4)};
    }
    return {load<u32>(0x104A0908), load<u32>(0x104A090C)};
}

/* 026151A8: touch delta from the touch start (current controller's touch) */
Pair32 touchFromStart(u32 self) {
    WWHD_FUNC(0x026151A8, Pair32, self);
    const u32 c = load<u32>(0x101F5088);
    const u32 src = (load<u32>(c + 0x24) & 1) ? c + 0x18 + 0x19C : 0x1049FFA0;
    return touchDelta(src, self, 0x88, 0x8C);
}
VERIFY(0x026151A8, touchFromStart);

/* 02615260: touch delta from the touch start (GamePad touch position) */
Pair32 padTouchFromStart(u32 self) {
    WWHD_FUNC(0x02615260, Pair32, self);
    return touchDelta(load<u32>(0x101F5088) + 0x1B4, self, 0x88, 0x8C);
}
VERIFY(0x02615260, padTouchFromStart);

/* 026152FC: touch delta from the previous position */
Pair32 padTouchFromPrev(u32 self) {
    WWHD_FUNC(0x026152FC, Pair32, self);
    return touchDelta(load<u32>(0x101F5088) + 0x1B4, self, 0x98, 0x9C);
}
VERIFY(0x026152FC, padTouchFromPrev);

/* 02615398: the current id, else the last one */
u32 getId(u32 self) {
    WWHD_FUNC(0x02615398, u32, self);
    const u32 cur = load<u32>(self + 0xD0);
    return cur ? cur : load<u32>(self + 0xCC);
}
VERIFY(0x02615398, getId);

static u32 vcheck(u32 self, u32 off, u32 ev) {
    return call_ptr<u32>(load<u32>(load<u32>(self + 0x58) + off), self, ev);
}
static void vact(u32 self, u32 off) {
    call_ptr<void>(load<u32>(load<u32>(self + 0x58) + off), self);
}
static void vactEv(u32 self, u32 off, u32 ev) {
    call_ptr<void>(load<u32>(load<u32>(self + 0x58) + off), self, ev);
}

/* shifts the touch history and stores the new position */
static void shiftTouch(u32 self, u32 pos) {
    const f32 a = load<f32>(self + 0xB4);
    const f32 x = load<f32>(pos);
    store<f32>(self + 0xAC, a);
    const f32 b = load<f32>(self + 0xB8);
    store<f32>(self + 0xB4, x);
    const f32 y = load<f32>(pos + 4);
    store<f32>(self + 0xB0, b);
    store<f32>(self + 0xB8, y);
}

/* 026153B4: event dispatch: touch handling, then the button checks in priority order */
u32 dispatch(u32 self, u32 ev) {
    WWHD_FUNC(0x026153B4, u32, self, ev);
    if (!ev) return 1;
    const u32 src = (load<u32>(ev + 0xC) & 1) ? ev + 0x19C : 0x1049FFA0;
    Local<u32[2]> pos;
    store<u32>(pos.a, load<u32>(src));
    const f32 lim = load<f32>(0x100E267C);
    const f32 x = load<f32>(pos.a);
    store<u32>(pos.a + 4, load<u32>(src + 4));
    if (x > lim && load<f32>(pos.a + 4) > lim) {
        if (load<u32>(ev) & 0x8000) {
            vactEv(self, 0x2C, ev);
            shiftTouch(self, pos.a);
            return 1;
        }
        if (load<u32>(ev + 0x10C) & 0x8000) {
            vactEv(self, 0x34, ev);
            shiftTouch(self, pos.a);
            return 1;
        }
    } else {
        if (s32(load<u32>(self + 0xA8)) > 0 || (load<u32>(ev + 4) & 0x8000)) {
            vactEv(self, 0x3C, ev);
            return 1;
        }
        store<u32>(self + 0xC, 0);
        setFocus(self, 0);
        setHover(self, 0);
        const f32 z = zero();
        store<f32>(self + 0xB4, z);
        store<f32>(self + 0xB0, z);
        store<f32>(self + 0xAC, z);
        store<f32>(self + 0xB8, z);
    }
    if (vcheck(self, 0x204, ev)) { vact(self, 0x194); return 1; }
    if (vcheck(self, 0x20C, ev)) { vact(self, 0x19C); return 1; }
    if (load<u8>(load<u32>(0x101F8378) + 0x14)) {
        if (vcheck(self, 0x1A4, ev)) {
            if (vcheck(self, 0x1B4, ev)) vact(self, 0x64);
            else if (vcheck(self, 0x1BC, ev)) vact(self, 0x6C);
            else vact(self, 0x44);
            return 1;
        }
        if (vcheck(self, 0x1AC, ev)) {
            if (vcheck(self, 0x1B4, ev)) vact(self, 0x74);
            else if (vcheck(self, 0x1BC, ev)) vact(self, 0x7C);
            else vact(self, 0x4C);
            return 1;
        }
        static const u16 pairs[][2] = {{0x1B4, 0x54}, {0x1BC, 0x5C}, {0x1D4, 0x164}, {0x1DC, 0x16C}, {0x1EC, 0x17C},
                                       {0x1C4, 0x154}, {0x1F4, 0x184}, {0x1FC, 0x18C}, {0x1CC, 0x15C}};
        for (const auto& pr : pairs)
            if (vcheck(self, pr[0], ev)) { vact(self, pr[1]); return 1; }
    }
    static const u16 tail[][2] = {{0x1E4, 0x174}, {0xEC, 0x8C}, {0x13C, 0x9C}, {0xC4, 0x84}, {0x114, 0x94}};
    for (const auto& pr : tail)
        if (vcheck(self, pr[0], ev)) { vact(self, pr[1]); return 1; }
    return 0;
}
VERIFY(0x026153B4, dispatch);

/* 02615C80: touch down: focuses the first receiver hit by the touch */
u32 touchDown(u32 self, u32 ev) {
    WWHD_FUNC(0x02615C80, u32, self, ev);
    setGrab(self, 0);
    setHover(self, 0);
    const f32 z = zero();
    store<u32>(self + 0xA8, 0xFFFFFFFF);
    store<f32>(self + 0xA4, z);
    store<f32>(self + 0xB4, z);
    store<f32>(self + 0x8C, z);
    store<f32>(self + 0x88, z);
    store<f32>(self + 0xB8, z);
    store<f32>(self + 0x9C, z);
    store<f32>(self + 0xB0, z);
    store<f32>(self + 0xA0, z);
    store<f32>(self + 0x90, z);
    store<f32>(self + 0xAC, z);
    store<f32>(self + 0x94, z);
    store<f32>(self + 0x98, z);
    if (!ev) {
        store<u32>(self + 0xC, 0);
        setFocus(self, 0);
        return 0;
    }
    const u32 src = (load<u32>(ev + 0xC) & 1) ? ev + 0x19C : 0x1049FFA0;
    Local<u32[2]> pos;
    store<u32>(pos.a, load<u32>(src));
    u32 node = load<u32>(self + 4);
    store<u32>(pos.a + 4, load<u32>(src + 4));
    if (node == self) return 0;
    const f32 x = load<f32>(pos.a), y = load<f32>(pos.a + 4);
    do {
        const u32 item = load<u32>(node + 8);
        if (call_ptr<u32>(load<u32>(load<u32>(item + 0x14) + 0x14), item, x, y)) {
            call<void>(0x02000324, self, 2u, load<u32>(item + 0x10), 0u);
            setFocus(self, item);
            store<u32>(self + 0xC, load<u32>(item + 0x10));
            if (call_ptr<u32>(load<u32>(load<u32>(item + 0x14) + 0x44), item)) {
                const f32 px = load<f32>(pos.a);
                store<f32>(self + 0xA0, z);
                store<f32>(self + 0x90, px);
                const f32 py = load<f32>(pos.a + 4);
                store<f32>(self + 0x98, px);
                store<f32>(self + 0x9C, py);
                store<f32>(self + 0x8C, y);
                store<f32>(self + 0xA4, z);
                store<u32>(self + 0xA8, 0xFFFFFFFF);
                store<f32>(self + 0x94, py);
                store<f32>(self + 0x88, x);
            }
            return 1;
        }
        node = load<u32>(node + 4);
    } while (node != self);
    return 0;
}
VERIFY(0x02615C80, touchDown);

/* 02615ED0: the nearest receiver that accepts the focused one (distance 02617174) */
u32 findTarget(u32 self) {
    WWHD_FUNC(0x02615ED0, u32, self);
    u32 focus = load<u32>(self + 0x7C);
    if (!focus) return 0;
    const f32 z = zero();
    f32 bestD = z;
    u32 best = 0;
    u32 node = load<u32>(self + 4);
    if (node == self) return 0;
    for (;;) {
        const u32 item = load<u32>(node + 8);
        if (item != focus && call_ptr<u32>(load<u32>(load<u32>(item + 0x14) + 0x4C), item) &&
            call_ptr<u32>(load<u32>(load<u32>(item + 0x14) + 0x3C), item, load<u32>(self + 0x7C))) {
            const f32 d = call<f32>(0x02617174, load<u32>(self + 0x7C), item);
            if (bestD == z || bestD > d) {
                bestD = d;
                best = item;
            }
        }
        node = load<u32>(node + 4);
        if (node == self) return best;
        focus = load<u32>(self + 0x7C);
    }
}
VERIFY(0x02615ED0, findTarget);

static void zeroTouch(u32 self, const u16* offs, u32 n) {
    const f32 z = zero();
    for (u32 i = 0; i < n; i++) store<f32>(self + offs[i], z);
}

/* 02616020: touch move: drag over targets (events 0x43..0x48, 3) */
u32 touchMove(u32 self, u32 ev) {
    WWHD_FUNC(0x02616020, u32, self, ev);
    if (!ev) {
        store<u32>(self + 0xC, 0);
        setFocus(self, 0);
        setGrab(self, 0);
        setHover(self, 0);
        static const u16 o[] = {0x90, 0xB8, 0xB4, 0x88, 0x94, 0xAC, 0xA4, 0x8C, 0xA0, 0xB0};
        zeroTouch(self, o, 10);
        return 0;
    }
    const u32 src = (load<u32>(ev + 0xC) & 1) ? ev + 0x19C : 0x1049FFA0;
    Local<u32[2]> pos;
    const u32 wx = load<u32>(src);
    const u32 px = load<u32>(self + 0x90);
    store<u32>(pos.a, wx);
    const u32 wy = load<u32>(src + 4);
    store<u32>(self + 0x98, px);
    const u32 grab = load<u32>(self + 0x80);
    const u32 py = load<u32>(self + 0x94);
    store<u32>(pos.a + 4, wy);
    store<u32>(self + 0x94, wy);
    store<u32>(self + 0x90, wx);
    store<u32>(self + 0x9C, py);
    if (grab) {
        const u32 best = findTarget(self);
        const u32 hover = load<u32>(self + 0x84);
        if (best) {
            if (hover == best) {
                call<void>(0x02000324, self, 0x47u, load<u32>(self + 0xC), 0u);
                return 1;
            }
            if (!call_ptr<u32>(load<u32>(load<u32>(best + 0x14) + 0x4C), best)) return 0;
            setHover(self, best);
            call<void>(0x02000324, self, 0x46u, load<u32>(self + 0xC), 0u);
            return 1;
        }
        if (hover) {
            setHover(self, 0);
            call<void>(0x02000324, self, 0x48u, load<u32>(self + 0xC), 0u);
            return 0;
        }
        call<void>(0x02000324, self, 0x44u, load<u32>(load<u32>(self + 0x80) + 0x10), 0u);
        return 0;
    }
    u32 item = load<u32>(self + 0x7C);
    if (!item) return 0;
    if (call_ptr<u32>(load<u32>(load<u32>(item + 0x14) + 0x44), item)) {
        const f32 dy = fsubs_ppc(load<f32>(self + 0x8C), load<f32>(pos.a + 4));
        const f32 dy2 = fmuls_ppc(dy, dy);
        const f32 dx = fsubs_ppc(load<f32>(self + 0x88), load<f32>(pos.a));
        const f32 d2 = fmadds(dx, dx, dy2);
        if (d2 > load<f32>(0x100E2680)) {
            const u32 id = load<u32>(load<u32>(self + 0x7C) + 0x10);
            store<u32>(self + 0xC, id);
            call<void>(0x02000324, self, 0x43u, id, 0u);
            setGrab(self, load<u32>(self + 0x7C));
            return 1;
        }
    }
    item = load<u32>(self + 0x7C);
    bool keep = call_ptr<u32>(load<u32>(load<u32>(item + 0x14) + 0x44), item);
    if (!keep) {
        item = load<u32>(self + 0x7C);
        keep = call_ptr<u32>(load<u32>(load<u32>(item + 0x14) + 0x14), item, load<f32>(pos.a), load<f32>(pos.a + 4));
    }
    if (!keep) {
        store<u32>(self + 0xC, 0);
        setGrab(self, 0);
        setHover(self, 0);
        return 0;
    }
    call<void>(0x02000324, self, 3u, load<u32>(self + 0xC), 0u);
    setHover(self, 0);
    return 1;
}
VERIFY(0x02616020, touchMove);

/* 0261634C: bounces the inertia velocity at the screen edges (640x360 less the margin) */
void bounce(u32 self) {
    WWHD_FUNC(0x0261634C, void, self);
    const u32 grab = load<u32>(self + 0x80);
    if (!grab) return;
    const f32 m = load<f32>(self + 0xE0);
    const u32 mtx = load<u32>(grab + 0x18) + 0x48;
    const f32 h = fsubs_ppc(load<f32>(0x100E2688), m);
    const f32 w = fsubs_ppc(load<f32>(0x100E2684), m);
    Local<u32[12]> copy;
    for (u32 i = 0; i < 12; i++) store<u32>(copy.a + 4 * i, load<u32>(mtx + 4 * i));
    const f32 nw = -w;
    const f32 tx = load<f32>(copy.a + 0xC);
    const f32 z = zero();
    const f32 ty = load<f32>(copy.a + 0x1C);
    if (tx < nw || w < tx) { /* blt / bge */
        const f32 v = -load<f32>(self + 0xA0);
        store<f32>(self + 0xA0, v);
        store<f32>(self + 0x90, v > z ? nw : w);
    }
    const f32 nh = -h;
    if (ty < nh || h < ty) {
        const f32 v = -load<f32>(self + 0xA4);
        store<f32>(self + 0xA4, v);
        store<f32>(self + 0x94, fsubs_ppc(v > z ? nh : h, load<f32>(0x100E268C)));
    }
}
VERIFY(0x0261634C, bounce);

/* 02616444: touch release: swipe events, inertia scrolling of the grabbed receiver, drop */
u32 touchUp(u32 self, u32 ev) {
    WWHD_FUNC(0x02616444, u32, self, ev);
    const f32 z = zero();
    if (!ev) {
        store<u32>(self + 0xC, 0);
        setFocus(self, 0);
        setGrab(self, 0);
        setHover(self, 0);
        store<f32>(self + 0xB8, z);
        store<u32>(self + 0xA8, 0xFFFFFFFF);
        static const u16 o[] = {0x8C, 0xA4, 0x90, 0xAC, 0x94, 0xB0, 0x98, 0x88, 0x9C, 0xB4, 0xA0};
        zeroTouch(self, o, 11);
        return 0;
    }
    Local<u32[6]> F; /* +0 position words, +8 delta from the start */
    const u32 f = F.a;
    store<u32>(f, load<u32>(ev + 0x19C));
    const u32 wy = load<u32>(ev + 0x1A0);
    const f32 sx = load<f32>(f);
    const f32 ax = load<f32>(self + 0xAC);
    store<u32>(f + 4, wy);
    const f32 sy = load<f32>(f + 4);
    const f32 dx = fsubs_ppc(sx, ax);
    const f32 ay = load<f32>(self + 0xB0);
    store<f32>(f + 0x10, dx);
    const f32 dy = fsubs_ppc(sy, ay);
    store<u32>(f + 8, load<u32>(f + 0x10));
    store<f32>(f + 0x14, dy);
    store<u32>(f + 0xC, load<u32>(f + 0x14));
    if (load<u32>(self + 0x80)) {
        if (load<u32>(self + 0x84)) {
            const u32 best = findTarget(self);
            if (best) {
                setHover(self, best);
                call<void>(0x02000324, self, 0x45u, load<u32>(load<u32>(self + 0x80) + 0x10), 0u);
                store<u32>(self + 0xC, 0);
                setFocus(self, 0);
                store<f32>(self + 0x98, z);
                store<u32>(self + 0xA8, 0xFFFFFFFF);
                static const u16 o[] = {0x94, 0xA4, 0x90, 0x8C, 0x9C, 0x88, 0xA0};
                zeroTouch(self, o, 7);
                return 1;
            }
        }
        const u32 item = load<u32>(self + 0x7C);
        if (item && call_ptr<u32>(load<u32>(load<u32>(item + 0x14) + 0x54), item)) {
            const f32 ddx = load<f32>(f + 8);
            if (load<f32>(0x100E2690) < fabsf(ddx)) { /* bge: branch when not less */
                call<void>(0x02000324, self, 0x64u, load<u32>(self + 0xC), 0u);
                const f32 ddy = load<f32>(f + 0xC);
                store<f32>(self + 0xBC, ddx);
                store<f32>(self + 0xC0, ddy);
            }
            const f32 ddy = load<f32>(f + 0xC);
            if (load<f32>(0x100E2694) < fabsf(ddy)) {
                call<void>(0x02000324, self, 0x65u, load<u32>(self + 0xC), 0u);
                const f32 ddx2 = load<f32>(f + 8);
                store<f32>(self + 0xC0, ddy);
                store<f32>(self + 0xBC, ddx2);
            }
            store<u32>(self + 0xC, 0);
            setGrab(self, 0);
            setHover(self, 0);
            return 0;
        }
        const s32 frames = s32(load<u32>(self + 0xA8));
        if (frames == -1) {
            store<u32>(self + 0xA8, load<u32>(self + 0xD4));
            const Pair32 v = call<Pair32>(0x026152FC, self);
            store<u32>(self + 0xA0, v.r3);
            store<u32>(self + 0xA4, v.r4);
            call<void>(0x02000324, self, 0x44u, load<u32>(load<u32>(self + 0x80) + 0x10), 0u);
            return 1;
        }
        bool go = false;
        if (frames > 0) {
            const f32 vy = load<f32>(self + 0xA4), vx = load<f32>(self + 0xA0);
            const f32 l2 = fmadds(vx, vx, fmuls_ppc(vy, vy));
            if (l2 > z) {
                const f64 e = frsqrte(l2); /* the estimate stays double in the FPR */
                const f32 e2 = f32(e * round25(e)); /* fmuls rounds frC to 25 bits */
                const f32 eh = f32(e * f64(load<f32>(0x100E2698)));
                const f32 t = fnmsubs(e2, l2, load<f32>(0x100E269C));
                const f32 len = fmuls_ppc(fmuls_ppc(t, eh), l2);
                go = !(len < load<f32>(self + 0xDC));
            } else {
                go = !(fmuls_ppc(z, l2) < load<f32>(self + 0xDC));
            }
        }
        if (!go) {
            store<u32>(self + 0xA8, 0xFFFFFFFF);
            store<u32>(self + 0xC, 0);
            setGrab(self, 0);
            setHover(self, 0);
            return 0;
        }
        store<u32>(self + 0xA8, load<u32>(self + 0xA8) - 1);
        bounce(self);
        const f32 fr = load<f32>(self + 0xD8);
        const f32 vx = fmuls_ppc(load<f32>(self + 0xA0), fr);
        const f32 px = load<f32>(self + 0x90);
        const f32 vy = fmuls_ppc(load<f32>(self + 0xA4), fr);
        store<f32>(self + 0xA0, vx);
        const f32 nx = fadds_ppc(px, vx);
        const f32 py = load<f32>(self + 0x94);
        store<f32>(self + 0xA4, vy);
        const f32 ny = fadds_ppc(py, vy);
        store<f32>(self + 0x90, nx);
        store<f32>(self + 0x94, ny);
        const u32 best = findTarget(self);
        if (best) {
            store<f32>(self + 0x90, z);
            store<f32>(self + 0xA4, z);
            store<u32>(self + 0xA8, 0xFFFFFFFF);
            store<f32>(self + 0xA0, z);
            store<f32>(self + 0x94, z);
            store<f32>(self + 0x9C, z);
            store<f32>(self + 0x98, z);
            setHover(self, best);
            call<void>(0x02000324, self, 0x45u, load<u32>(load<u32>(self + 0x80) + 0x10), 0u);
            return 1;
        }
        call<void>(0x02000324, self, 0x44u, load<u32>(load<u32>(self + 0x80) + 0x10), 0u);
        return 0;
    }
    const u32 item = load<u32>(self + 0x7C);
    if (item && call_ptr<u32>(load<u32>(load<u32>(item + 0x14) + 0x14), item, load<f32>(f), load<f32>(f + 4))) {
        call<void>(0x02000324, self, 4u, load<u32>(self + 0xC), 0u);
        store<u32>(self + 0xC, 0);
        setFocus(self, 0);
        setGrab(self, 0);
        setHover(self, 0);
        static const u16 o[] = {0x8C, 0xA0, 0x9C, 0x88, 0x98, 0xA4};
        zeroTouch(self, o, 6);
        return 1;
    }
    store<u32>(self + 0xC, 0);
    setGrab(self, 0);
    setHover(self, 0);
    return 0;
}
VERIFY(0x02616444, touchUp);

/* 02616950..02616B44: button event senders (event 0xD with a button code, or 0x62/0x63) */
static u32 sendButton(u32 self, u32 code) {
    Local<u8[8]> b;
    store<u8>(b.a, u8(code));
    call<void>(0x02000324, self, 0xDu, 0xFFFFFFFFu, b.a);
    return 0;
}
u32 sendButton0(u32 self) { WWHD_FUNC(0x02616950, u32, self); return sendButton(self, 0); }
VERIFY(0x02616950, sendButton0);
u32 sendButton1(u32 self) { WWHD_FUNC(0x02616990, u32, self); return sendButton(self, 1); }
VERIFY(0x02616990, sendButton1);
u32 sendButton3(u32 self) { WWHD_FUNC(0x026169C8, u32, self); return sendButton(self, 3); }
VERIFY(0x026169C8, sendButton3);
u32 sendButton4(u32 self) { WWHD_FUNC(0x02616A00, u32, self); return sendButton(self, 4); }
VERIFY(0x02616A00, sendButton4);
u32 sendButtonD(u32 self) { WWHD_FUNC(0x02616A38, u32, self); return sendButton(self, 0xD); }
VERIFY(0x02616A38, sendButtonD);
u32 sendButtonE(u32 self) { WWHD_FUNC(0x02616A6C, u32, self); return sendButton(self, 0xE); }
VERIFY(0x02616A6C, sendButtonE);
u32 sendButton2(u32 self) { WWHD_FUNC(0x02616AA4, u32, self); return sendButton(self, 2); }
VERIFY(0x02616AA4, sendButton2);
u32 sendButton5(u32 self) { WWHD_FUNC(0x02616ADC, u32, self); return sendButton(self, 5); }
VERIFY(0x02616ADC, sendButton5);
u32 sendEvent62(u32 self) {
    WWHD_FUNC(0x02616B14, u32, self);
    call<void>(0x02000324, self, 0x62u, 0xFFFFFFFFu, 0u);
    return 0;
}
VERIFY(0x02616B14, sendEvent62);
u32 sendEvent63(u32 self) {
    WWHD_FUNC(0x02616B44, u32, self);
    call<void>(0x02000324, self, 0x63u, 0xFFFFFFFFu, 0u);
    return 0;
}
VERIFY(0x02616B44, sendEvent63);

/* stick-direction button: the stick bit when the stick is tilted, else the D-pad bit */
static u8 stickOrPad(u32 ev, u32 stickBit, u32 padBit) {
    const f32 z = zero();
    const bool tilted = !(load<f32>(ev + 0x118) == z) || !(load<f32>(ev + 0x11C) == z);
    const u32 b = tilted ? stickBit : padBit;
    return u8(((load<u32>(ev) >> b) | (load<u32>(ev + 8) >> b)) & 1);
}
u8 isLeft(u32 self, u32 ev) { WWHD_FUNC(0x02616B74, u8, self, ev); return stickOrPad(ev, 20, 16); }
VERIFY(0x02616B74, isLeft);
u8 isRight(u32 self, u32 ev) { WWHD_FUNC(0x02616BCC, u8, self, ev); return stickOrPad(ev, 21, 17); }
VERIFY(0x02616BCC, isRight);
u8 isUp(u32 self, u32 ev) { WWHD_FUNC(0x02616C24, u8, self, ev); return stickOrPad(ev, 22, 18); }
VERIFY(0x02616C24, isUp);
u8 isDown(u32 self, u32 ev) { WWHD_FUNC(0x02616C7C, u8, self, ev); return stickOrPad(ev, 23, 19); }
VERIFY(0x02616C7C, isDown);

/* 02616CD4..02616D40: hold bits of the event */
u32 holdBit0(u32 self, u32 ev) { WWHD_FUNC(0x02616CD4, u32, self, ev); return (load<u32>(ev) >> 0) & 1; }
VERIFY(0x02616CD4, holdBit0);
u32 holdBit1(u32 self, u32 ev) { WWHD_FUNC(0x02616CE0, u32, self, ev); return (load<u32>(ev) >> 1) & 1; }
VERIFY(0x02616CE0, holdBit1);
u32 holdBit3(u32 self, u32 ev) { WWHD_FUNC(0x02616CEC, u32, self, ev); return (load<u32>(ev) >> 3) & 1; }
VERIFY(0x02616CEC, holdBit3);
u32 holdBit4(u32 self, u32 ev) { WWHD_FUNC(0x02616CF8, u32, self, ev); return (load<u32>(ev) >> 4) & 1; }
VERIFY(0x02616CF8, holdBit4);
u32 holdBit13(u32 self, u32 ev) { WWHD_FUNC(0x02616D04, u32, self, ev); return (load<u32>(ev) >> 13) & 1; }
VERIFY(0x02616D04, holdBit13);
u32 holdBit14(u32 self, u32 ev) { WWHD_FUNC(0x02616D10, u32, self, ev); return (load<u32>(ev) >> 14) & 1; }
VERIFY(0x02616D10, holdBit14);
u32 holdBit2(u32 self, u32 ev) { WWHD_FUNC(0x02616D1C, u32, self, ev); return (load<u32>(ev) >> 2) & 1; }
VERIFY(0x02616D1C, holdBit2);
u32 holdBit5(u32 self, u32 ev) { WWHD_FUNC(0x02616D28, u32, self, ev); return (load<u32>(ev) >> 5) & 1; }
VERIFY(0x02616D28, holdBit5);
u32 holdBit11(u32 self, u32 ev) { WWHD_FUNC(0x02616D34, u32, self, ev); return (load<u32>(ev) >> 11) & 1; }
VERIFY(0x02616D34, holdBit11);
u32 holdBit12(u32 self, u32 ev) { WWHD_FUNC(0x02616D40, u32, self, ev); return (load<u32>(ev) >> 12) & 1; }
VERIFY(0x02616D40, holdBit12);

/* 02616D4C: SingletonDisposer deleting destructor (deletes the instance) */
void disposerDtor(u32 p, u32 flags) {
    WWHD_FUNC(0x02616D4C, void, p, flags);
    if (!p) return;
    store<u32>(p + 0xC, 0x100E26B0);
    if (p == load<u32>(0x101F4FF4)) {
        const u32 inst = load<u32>(kInstance);
        store<u32>(0x101F4FF4, 0);
        call_ptr<void>(load<u32>(load<u32>(inst + 0x58) + 0xC), inst, 2u);
        store<u32>(kInstance, 0);
    }
    call<void>(0x02752BEC, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x02616D4C, disposerDtor);

/* 02616DF4: static initialiser (shared objects, the default touch limits) */
void staticInit() {
    WWHD_FUNC(0x02616DF4, void);
    const u32 b = 0x1048DE04;
    store<u32>(b + 0xC, 0);
    store<u32>(b + 8, 0);
    store<u32>(b + 4, 0);
    store<u32>(b, 0);
    call<void>(0x028F026C, 0x101F4FCCu);
    const f32 lo = load<f32>(0x100E26A0), hi = load<f32>(0x100E26A4);
    store<f32>(0x1048DDE8, lo);
    store<f32>(0x1048DDEC, hi);
    call<void>(0x028ED6F8, 0x1048DE00u);
    call<void>(0x028F026C, 0x101F4FD8u);
    call<void>(0x028EAB2C, 0x1048DE01u);
    call<void>(0x028F026C, 0x101F4FE4u);
    const f32 a = load<f32>(0x100E26A8), c = load<f32>(0x100E26AC);
    store<f32>(0x1048DDF0, a);
    store<f32>(0x1048DDF8, c);
    store<f32>(0x1048DDF4, a);
    store<f32>(0x1048DDFC, c);
}
VERIFY(0x02616DF4, staticInit);

/* 02616ED0: empty function */
void nop() { WWHD_FUNC(0x02616ED0, void); }
VERIFY(0x02616ED0, nop);

} // namespace hd_input_eventmgr

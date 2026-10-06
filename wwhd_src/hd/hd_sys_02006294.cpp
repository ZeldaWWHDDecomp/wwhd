/* hd_sys_02006294: the HD state machine TU (no GameCube source).
 *
 * TU 02006294..02006583 (__sinit 020064B4 + companions). 02006294 is the root-task entry the application
 * passes to the framework (creates the 0x130 RootTask, 0203E9EC). The 0x14 state machine: owner +0 (its
 * vtable +0 has the state factory at +0x14 and the "on change" hook at +0x1C), requested state id +4,
 * current state object +8, last exit result +0xC, previous state object +0x10. A state object's vtable:
 * +0x14 id / exit, +0x1C enter, +0x24 update, +0x2C leave. The static 101FF32C is the "no state"
 * object {id -1, 0, vtable 10000A88}. 748 callers (actors included).
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_sys_02006294 {

/* 02006294: root-task entry: create the RootTask on the framework's current heap */
static u32 RootTask_entry(u32 fw) {
    WWHD_FUNC(0x02006294, u32, fw);
    u32 tab = ld(fw);
    u32 heap = ld(tab + ld(tab + 0x14) * 4);
    u32 p = gabi::call<u32>(0x0273B050, 0x130, heap, 4);
    if (p == 0) return 0;
    return gabi::call<u32>(0x0203E9EC, p, fw);
}
VERIFY(0x02006294, RootTask_entry);

/* 020062E8: state machine constructor (owner, initial state id pointer) */
static u32 StateMachine_ct(u32 self, u32 owner, u32 init) {
    WWHD_FUNC(0x020062E8, u32, self, owner, init);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x14);
        if (t == 0) return 0;
    }
    st(t + 0, owner);
    st(t + 4, 0);
    st(t + 0xC, 0x101FF32C);
    st(t + 8, 0);
    st(t + 0x10, 0);
    if (ld(init) != 0xFFFFFFFFu) st(t + 4, init);
    return t;
}
VERIFY(0x020062E8, StateMachine_ct);

/* 02006364: update: owner hook, then the current state's update */
static void StateMachine_update(u32 self) {
    WWHD_FUNC(0x02006364, void, self);
    u32 o = ld(self + 0);
    gabi::call_ptr(vfn(o, 0, 0x1C), o, self + 0x10);
    u32 s = ld(self + 8);
    if (s != 0) gabi::call_ptr(vfn(s, 0, 0x24), s);
}
VERIFY(0x02006364, StateMachine_update);

/* 020063C0: change state: leave the current state (remember its exit result and object), create and
 * enter the next one through the owner's factory */
static void StateMachine_change(u32 self, u32 next) {
    WWHD_FUNC(0x020063C0, void, self, next);
    u32 cur = ld(self + 8);
    st(self + 4, next);
    u32 n = next;
    if (cur != 0) {
        if (next == 0) return;
        u32 r = gabi::call_ptr<u32>(vfn(cur, 0, 0x14), cur);
        st(self + 0xC, r);
        u32 c2 = ld(self + 8);
        gabi::call_ptr(vfn(c2, 0, 0x2C), c2);
        u32 prev = ld(self + 8);
        st(self + 8, 0);
        n = ld(self + 4);
        st(self + 0x10, prev);
    }
    if (n == 0) return;
    u32 o = ld(self + 0);
    u32 ns = gabi::call_ptr<u32>(vfn(o, 0, 0x14), o, n);
    st(self + 8, ns);
    gabi::call_ptr(vfn(ns, 0, 0x1C), ns);
    st(self + 4, 0);
}
VERIFY(0x020063C0, StateMachine_change);

/* 02006478: current state id (the requested one or the "no state" object while none runs) */
static u32 StateMachine_current(u32 self) {
    WWHD_FUNC(0x02006478, u32, self);
    u32 s = ld(self + 8);
    if (s == 0) {
        u32 v = ld(self + 4);
        return v == 0 ? 0x101FF32C : v;
    }
    return gabi::call_ptr<u32>(vfn(s, 0, 0x14), s);
}
VERIFY(0x02006478, StateMachine_current);

/* 020064B4: __sinit (header statics + the "no state" object) */
static void sinit_020064B4() {
    WWHD_FUNC(0x020064B4, void);
    header_sinit(0x101FF31C, 0x1018C430, 0x10000AD0);
    st(0x101FF334, 0x10000A88);
    st(0x101FF32C, 0xFFFFFFFF);
    st(0x101FF330, 0);
}
VERIFY(0x020064B4, sinit_020064B4);

static u32 State_get0(u32 self) {
    WWHD_FUNC(0x02006570, u32, self);
    return ld(self);
}
VERIFY(0x02006570, State_get0);

static void State_empty1() {
    WWHD_FUNC(0x02006578, void);
}
VERIFY(0x02006578, State_empty1);

static void State_empty2() {
    WWHD_FUNC(0x0200657C, void);
}
VERIFY(0x0200657C, State_empty2);

static void State_empty3() {
    WWHD_FUNC(0x02006580, void);
}
VERIFY(0x02006580, State_empty3);

}  // namespace hd_sys_02006294

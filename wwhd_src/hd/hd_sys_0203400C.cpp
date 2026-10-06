/* hd_sys_0203400C: the HD exception / panic console (no GameCube source).
 *
 * TU 0203400C..02034B13 (__sinit 02034A80 + companions): a class RTTI check, two sead Delegate
 * invokers (pointer to member: object +4, this adjustment +8, vtable index +0xA (<0: plain function
 * +0xC), vtable offset +0xE), two controller lookups over an offset list at +0xCC (sead::OffsetList,
 * offset +0xD8) using DynamicCast-style isDerived (vtable at +0x10, slot +0xC), and the exception
 * dump manager (0x88: dump-callback singly linked list head +0, a heap +0x10 (0x3C), mutex +0x4C)
 * reached through the function-local static 02034380 (object 1020084C, guard 102008D4); dump entries
 * (8 bytes: next, vtable 0x10004C98) register in their constructor. The OS exception callbacks
 * (DSI 2, ISI 3, program 6) are installed with OSSetExceptionCallback; 02034598 is the console
 * printf (va_list to 0286DB50), 02034974 the panic(file, line, fmt, ...).
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_sys_0203400C {

static constexpr u32 OSSetExceptionCallback = 0xC000A018; /* stub 028FE290 */
static constexpr u32 excPrintf = 0x0286DBB0;  /* varargs */
static constexpr u32 excVPrintf = 0x0286DB50; /* (fmt, va_list) */
static constexpr u32 lock = 0x027601BC, unlock = 0x027601F0, tryLock = 0x027601C4;

/* 0203400C: checkDerivedRuntimeTypeInfo: this class or its parent */
static u32 isDerived(u32 self, u32 ti) {
    WWHD_FUNC(0x0203400C, u32, self, ti);
    if (ld(0x101FD598) == 0) {
        st(0x101FD598, 1);
        st(0x101FD97C, 0x10004AA4);
    }
    if (ti == 0x101FD97C) return 1;
    if (ld(0x101FD594) == 0) {
        st(0x101FD594, 1);
        st(0x101FD980, 0x10004A94);
    }
    return ti == 0x101FD980 ? 1 : 0;
}
VERIFY(0x0203400C, isDerived);

/* sead Delegate invoke (tail call: the target gets the adjusted object and the remaining arguments) */
static inline u32 deleg_invoke(u32 self, u32 a) {
    u32 obj = ld(self + 4);
    if (obj == 0) return self;
    s32 vi = (s16)lhz(self + 0xA);
    if (vi == 0) return self;
    u32 adj = obj + (u32)(s32)(s16)lhz(self + 8);
    if (vi < 0) return gabi::call_ptr<u32>(ld(self + 0xC), adj, a);
    u32 vt = ld(adj + (u32)(s32)(s16)lhz(self + 0xE));
    return gabi::call_ptr<u32>(ld(vt + (u32)vi * 8 + 4), adj, a);
}
static u32 Delegate_invoke1(u32 self, u32 a) {
    WWHD_FUNC(0x02034084, u32, self, a);
    return deleg_invoke(self, a);
}
VERIFY(0x02034084, Delegate_invoke1);
static u32 Delegate_invoke2(u32 self, u32 a) {
    WWHD_FUNC(0x020340D8, u32, self, a);
    return deleg_invoke(self, a);
}
VERIFY(0x020340D8, Delegate_invoke2);

static void empty_0203412C() {
    WWHD_FUNC(0x0203412C, void);
}
VERIFY(0x0203412C, empty_0203412C);

/* the first entry of the offset list at +0xCC deriving from the type info TI (guard G, parent 0x10004AB4) */
static inline u32 find_derived(u32 self, u32 g, u32 ti) {
    u32 off = ld(self + 0xD8);
    u32 head = self + 0xCC;
    u32 end = head - off;
    u32 o = ld(self + 0xD0) - off;
    while (o != end) {
        if (ld(g) == 0) {
            st(g, 1);
            st(ti, 0x10004AB4);
        }
        bool ok = false;
        if (o != 0 && gabi::call_ptr<u32>(vfn(o, 0x10, 0xC), o, ti) != 0) ok = true;
        if (ok) return o;
        o = ld(o + off + 4) - off;
        end = head - ld(head + 0xC);
    }
    return 0;
}
static u32 findController1(u32 self) {
    WWHD_FUNC(0x02034130, u32, self);
    return find_derived(self, 0x101FD698, 0x101FD978);
}
VERIFY(0x02034130, findController1);
static u32 findController2(u32 self) {
    WWHD_FUNC(0x02034200, u32, self);
    return find_derived(self, 0x101FD694, 0x101FD968);
}
VERIFY(0x02034200, findController2);

/* 020342D0: dump manager constructor (0x88) */
static u32 DumpMgr_ct(u32 self) {
    WWHD_FUNC(0x020342D0, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x88);
        if (t == 0) return 0;
    }
    st(t + 0xC, 0);
    st(t + 8, 0);
    st(t + 4, 0);
    st(t + 0, 0);
    u32 h = t + 0x10;
    if (h == 0) h = op_new(0x3C);
    if (h != 0) {
        gabi::call(0x02752A84, h);
        st(h + 0x14, 0);
        st(h + 0x20, 0);
        st(h + 0xC, 0x10004CB0);
        stb(h + 0x38, 0);
        st(h + 0x18, h);
        st(h + 0x10, 0);
        st(h + 0x1C, 0);
    }
    gabi::call(0x02760084, t + 0x4C);
    return t;
}
VERIFY(0x020342D0, DumpMgr_ct);

/* 02034380: the function-local static instance */
static u32 DumpMgr_instance() {
    WWHD_FUNC(0x02034380, u32);
    if (ld(0x102008D4) == 0) {
        st(0x102008D4, 1);
        gabi::call(0x020342D0, 0x1020084C);
        gabi::call(0x028F026C, 0x1018F2C0);
    }
    return 0x1020084C;
}
VERIFY(0x02034380, DumpMgr_instance);

/* 020343D8: push an entry to the front */
static void DumpMgr_add(u32 self, u32 e) {
    WWHD_FUNC(0x020343D8, void, self, e);
    gabi::call(lock, self + 0x4C);
    st(e, ld(self));
    st(self, e);
    gabi::call(unlock, self + 0x4C);
}
VERIFY(0x020343D8, DumpMgr_add);

/* 02034434: dump entry constructor (registers itself) */
static u32 DumpEntry_ct(u32 self) {
    WWHD_FUNC(0x02034434, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(8);
        if (t == 0) return 0;
    }
    st(t + 0, 0);
    st(t + 4, 0x10004C98);
    u32 m = gabi::call<u32>(0x02034380);
    gabi::call(0x020343D8, m, t);
    return t;
}
VERIFY(0x02034434, DumpEntry_ct);

/* 02034494: unlink an entry */
static void DumpMgr_remove(u32 self, u32 e) {
    WWHD_FUNC(0x02034494, void, self, e);
    gabi::call(lock, self + 0x4C);
    u32 nx = ld(e);
    st(e, 0);
    u32 c = ld(self);
    if (c == e) {
        st(self, nx);
        gabi::call(unlock, self + 0x4C);
        return;
    }
    for (; c != 0; c = ld(c)) {
        if (ld(c) == e) {
            st(c, nx);
            gabi::call(unlock, self + 0x4C);
            return;
        }
    }
    gabi::call(unlock, self + 0x4C);
}
VERIFY(0x02034494, DumpMgr_remove);

/* 02034538: dump entry destructor (unregisters itself) */
static void DumpEntry_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02034538, void, self, flags);
    if (self == 0) return;
    st(self + 4, 0x10004C98);
    u32 m = gabi::call<u32>(0x02034380);
    gabi::call(0x02034494, m, self);
    if (flags & 1) op_delete(self);
}
VERIFY(0x02034538, DumpEntry_dt);

/* the variadic prologue: r3..r10 to FRAME+0x18, f1..f8 to FRAME+0x38 when the caller set cr1eq */
static inline void va_save(u32 f, const u32* regs) {
    for (int i = 0; i < 8; i++) st(f + 0x18 + 4 * i, regs[i]);
    if (gabi::cpu->cr[6] != 0) {
        for (int i = 0; i < 8; i++) {
            double d = gabi::cpu->f[1 + i].ps0;
            u64 bits;
            memcpy(&bits, &d, 8);
            st(f + 0x38 + 8 * i, (u32)(bits >> 32));
            st(f + 0x3C + 8 * i, (u32)bits);
        }
    }
}

/* 02034598: console printf(fmt, ...) */
static void excPrint(u32 fmt, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9, u32 a10) {
    WWHD_FUNC(0x02034598, void, fmt, a4, a5, a6, a7, a8, a9, a10);
    u32 entry = gabi::cpu->r[1];
    u32 f = entry - 0x78; /* the original's frame: va_list at +8, register save area at +0x18 */
    gabi::cpu->r[1] = f;
    st(f, entry); /* the back chain (stwu) */
    const u32 regs[8] = {fmt, a4, a5, a6, a7, a8, a9, a10};
    va_save(f, regs);
    st(f + 0xC, entry + 8);
    stb(f + 9, 0);
    st(f + 0x10, f + 0x18);
    stb(f + 8, 1);
    gabi::call(excVPrintf, fmt, f + 8);
    gabi::cpu->r[1] = entry;
}
VERIFY(0x02034598, excPrint);

/* 02034620: dump manager destructor */
static void DumpMgr_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02034620, void, self, flags);
    if (self == 0) return;
    gabi::call(0x02760168, self + 0x4C, 2);
    if (self + 0x10 != 0) {
        u8 b = lbz(self + 0x48);
        st(self + 0x1C, 0x10004CB0);
        if (b != 0) {
            u32 p = ld(self + 0x2C);
            if (p != 0) {
                st(self + 0x2C, 0);
                gabi::call(0x0273B310, self + 0x20);
                st(p + 8, ld(p + 8) - 1);
            }
            stb(self + 0x48, 0);
        }
        gabi::call(0x02752BEC, self + 0x10, 0);
    }
    if (flags & 1) op_delete(self);
}
VERIFY(0x02034620, DumpMgr_dt);

/* 020346DC: run every dump entry (vtable slot 0x14); the mutex is taken with tryLock and kept */
static void DumpMgr_dump(u32 self) {
    WWHD_FUNC(0x020346DC, void, self);
    if (gabi::call<u32>(tryLock, self + 0x4C) == 0) {
        gabi::call(excPrintf, 0x10004CD0);
        return;
    }
    for (u32 e = ld(self); e != 0; e = ld(e)) {
        gabi::call(excPrintf, 0x10004D00, e);
        gabi::call_ptr(vfn(e, 4, 0x14), e);
    }
}
VERIFY(0x020346DC, DumpMgr_dump);

/* 02034784: restore the saved exception callbacks */
static void Exc_restore(u32 self) {
    WWHD_FUNC(0x02034784, void, self);
    gabi::call(OSSetExceptionCallback, 2, ld(self + 4));
    gabi::call(OSSetExceptionCallback, 3, ld(self + 8));
    gabi::call(OSSetExceptionCallback, 6, ld(self + 0xC));
}
VERIFY(0x02034784, Exc_restore);

/* 020347D0: the exception callback: dump, restore the callbacks, continue */
static u32 Exc_handler() {
    WWHD_FUNC(0x020347D0, u32);
    gabi::call(excPrintf, 0x10004D2C);
    gabi::call(0x020346DC, gabi::call<u32>(0x02034380));
    gabi::call(0x02034784, gabi::call<u32>(0x02034380));
    gabi::call(excPrintf, 0x10004D5C);
    return 1;
}
VERIFY(0x020347D0, Exc_handler);

/* 02034820: install the callback for DSI / ISI / program exceptions, keeping the previous ones */
static void Exc_install(u32 self) {
    WWHD_FUNC(0x02034820, void, self);
    st(self + 4, gabi::call<u32>(OSSetExceptionCallback, 2, 0x020347D0));
    st(self + 8, gabi::call<u32>(OSSetExceptionCallback, 3, 0x020347D0));
    st(self + 0xC, gabi::call<u32>(OSSetExceptionCallback, 6, 0x020347D0));
}
VERIFY(0x02034820, Exc_install);

/* 02034884: initialise: callbacks plus a delegate (+0x34, function 02034934) handed to 0273ED58 */
static void Exc_init(u32 self) {
    WWHD_FUNC(0x02034884, void, self);
    gabi::call(0x02034820, self);
    u32 d = self + 0x34;
    if (d != 0) {
        st(d + 4, self);
        sth(d + 8, 0);
        st(d, 0x10004CC0);
        st(d + 0xC, 0x02034934);
        sth(d + 0xA, 0xFFFF);
    }
    st(self + 0x30, d);
    gabi::call(0x0273ED58, self + 0x10);
}
VERIFY(0x02034884, Exc_init);

static void Exc_dump1(u32 self) {
    WWHD_FUNC(0x020348F4, void, self);
    gabi::call(excPrintf, 0x10004D6C);
    gabi::call(0x020346DC, self);
}
VERIFY(0x020348F4, Exc_dump1);

static void Exc_dump2(u32 self) {
    WWHD_FUNC(0x02034934, void, self);
    gabi::call(excPrintf, 0x10004DA8);
    gabi::call(0x020346DC, self);
}
VERIFY(0x02034934, Exc_dump2);

/* 02034974: panic(file, line, fmt, ...) */
static void panic(u32 file, u32 line, u32 fmt, u32 a6, u32 a7, u32 a8, u32 a9, u32 a10) {
    WWHD_FUNC(0x02034974, void, file, line, fmt, a6, a7, a8, a9, a10);
    u32 entry = gabi::cpu->r[1];
    u32 f = entry - 0x90;
    gabi::cpu->r[1] = f;
    /* the prologue's saves inside the frame (compared with the va_list area at the vprintf call) */
    st(f + 0x8C, gabi::cpu->r[31]);
    st(f + 0x88, gabi::cpu->r[30]);
    st(f + 0x84, gabi::cpu->r[29]);
    const u32 regs[8] = {file, line, fmt, a6, a7, a8, a9, a10};
    va_save(f, regs);
    stb(f + 9, 0);
    stb(f + 8, 3);
    u32 w0 = ld(f + 8);
    st(f + 0x10, f + 0x18);
    st(f + 0xC, entry + 8);
    st(f + 0x78, w0);
    st(f + 0x7C, entry + 8);
    st(f + 0x80, f + 0x18);
    gabi::call(excPrintf, 0x10004DDC, file, line);
    gabi::call(excVPrintf, fmt, f + 8);
    gabi::call(excPrintf, 0x10004DD8);
    u32 m = gabi::call<u32>(0x02034380);
    gabi::call(0x020348F4, m, file, line, fmt, f + 0x78);
    gabi::call(0x0286DB54);
    gabi::cpu->r[1] = entry;
}
VERIFY(0x02034974, panic);

static void sinit_02034A80() {
    WWHD_FUNC(0x02034A80, void);
    header_sinit(0x1020083C, 0x1018F2CC, 0x10004DEC);
}
VERIFY(0x02034A80, sinit_02034A80);

}  // namespace hd_sys_0203400C

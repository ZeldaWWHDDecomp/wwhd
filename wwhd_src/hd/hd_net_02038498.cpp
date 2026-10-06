/* hd_net_02038498: the HD SpotPass manager translation unit (cking::net NetBossMgr, 02038498..02038E23).
 * No GameCube source: HD-only code.
 *
 * A sead singleton (0x24: IDisposer +0, work buffer +0x10, heap +0x14, state +0x18, thread +0x1C, last
 * result +0x20) that registers the game's nn::boss SpotPass task ("NetBossMgr" heap, "NetBossThread"
 * sead::DelegateThread): nn::boss initialisation, the NBDL task setting filled from 21 save values,
 * task (re)registration and scheduling; plus the TU's companions (deleting destructors, SafeString
 * terminator, a 0xF4 helper object) and the __sinit (the task id SafeString).
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_net_02038498 {

static constexpr u32 boss_Initialize = 0xC0005338;            /* nn::boss::Initialize() */
static constexpr u32 boss_TaskSetting_Initialize = 0xC0005380; /* NbdlTaskSetting::Initialize(buf, size) */
static constexpr u32 boss_Task_Initialize = 0xC0005398;        /* Task::Initialize(id, slot) */
static constexpr u32 boss_Task_IsRegistered = 0xC00053C8;
static constexpr u32 boss_Task_Register = 0xC0005450;
static constexpr u32 boss_TaskSetting_Set = 0xC0005608;
static constexpr u32 boss_Task_StartScheduling = 0xC0005610;
static constexpr u32 boss_Task_Unregister = 0xC0005638;
static constexpr u32 boss_TaskSetting_ct = 0xC0005728;
static constexpr u32 boss_Task_ct = 0xC0005750;
static constexpr u32 boss_TaskSetting_dt = 0xC0005830;
static constexpr u32 boss_Task_dt = 0xC0005850;
static constexpr u32 SS_VT = 0x100054F0; /* this TU's SafeString vtable */
static constexpr u32 INST = 0x1018F47C, DISP = 0x1018F480;

/* 02038498: deleting destructor of a class with three vtables (+0x20/+0x24/+0x30), base 02008B4C */
static void Dt_02038498(u32 p, u32 flags) {
    WWHD_FUNC(0x02038498, void, p, flags);
    if (p == 0) return;
    st(p + 0x20, 0x10005378);
    st(p + 0x24, 0x10005398);
    st(p + 0x30, 0x10005328);
    gabi::call(0x02008B4C, p + 0x10, 0u);
    if (flags & 1) op_delete(p);
}
VERIFY(0x02038498, Dt_02038498);

static void Dt_02038510(u32 p, u32 flags) {
    WWHD_FUNC(0x02038510, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x02038510, Dt_02038510);

static void Empty_02038524() {
    WWHD_FUNC(0x02038524, void);
}
VERIFY(0x02038524, Empty_02038524);

static void SafeBuf_assureTerminate(u32 self) {
    WWHD_FUNC(0x02038528, void, self);
    stb(ld(self) + ld(self + 8) - 1, 0);
}
VERIFY(0x02038528, SafeBuf_assureTerminate);

static void Dt_02038540(u32 p, u32 flags) {
    WWHD_FUNC(0x02038540, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x02038540, Dt_02038540);

static void Dt_02038554(u32 p, u32 flags) {
    WWHD_FUNC(0x02038554, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x02038554, Dt_02038554);

static void Dt_02038568(u32 p, u32 flags) {
    WWHD_FUNC(0x02038568, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x02038568, Dt_02038568);

/* 0203857C: constructor of a 0xF4 object (flag +0xF0) */
static u32 Obj_ct_0203857C(u32 self) {
    WWHD_FUNC(0x0203857C, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0xF4);
        if (t == 0) return 0;
    }
    stb(t + 0xF0, 0);
    return t;
}
VERIFY(0x0203857C, Obj_ct_0203857C);

static void Dt_020385B8(u32 p, u32 flags) {
    WWHD_FUNC(0x020385B8, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x020385B8, Dt_020385B8);

static void Empty_020385CC() {
    WWHD_FUNC(0x020385CC, void);
}
VERIFY(0x020385CC, Empty_020385CC);

static void Empty_020385D0() {
    WWHD_FUNC(0x020385D0, void);
}
VERIFY(0x020385D0, Empty_020385D0);

/* 020385D4: NetBossMgr constructor (0x24) */
static u32 BossMgr_ct(u32 self) {
    WWHD_FUNC(0x020385D4, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x24);
        if (t == 0) return 0;
    }
    st(t + 0x1C, 0);
    st(t + 0x20, 0xFFFFFFFF);
    st(t + 0x10, 0);
    st(t + 0x18, 0);
    st(t + 0x14, 0);
    return t;
}
VERIFY(0x020385D4, BossMgr_ct);

/* 02038624: sead singleton createInstance(heap) */
static u32 BossMgr_createInstance(u32 heap) {
    WWHD_FUNC(0x02038624, u32, heap);
    u32 cur = ld(INST);
    if (cur != 0) return cur;
    u32 d = gabi::call<u32>(0x0273B0D4, 0x24u, heap, 4u);
    if (d != 0) {
        gabi::call(0x02752B0C, d, heap, 3u);
        st(d + 0xC, 0x100055D0);
    }
    st(DISP, d);
    u32 r = d;
    if (d != 0) r = gabi::call<u32>(0x020385D4, d);
    st(INST, r);
    return r;
}
VERIFY(0x02038624, BossMgr_createInstance);

static void BossMgr_dt(u32 p, u32 flags) {
    WWHD_FUNC(0x020386B8, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x020386B8, BossMgr_dt);

/* 020386CC: sead singleton deleteInstance */
static void BossMgr_deleteInstance() {
    WWHD_FUNC(0x020386CC, void);
    u32 d = ld(DISP);
    if (d == 0) return;
    st(DISP, 0);
    gabi::call_ptr<u32>(vfn(d, 0xC, 0xC), d, 2u);
    gabi::call(0x020386B8, ld(INST), 3u);
    st(INST, 0);
}
VERIFY(0x020386CC, BossMgr_deleteInstance);

/* 02038738: keep the last nn::Result (the third argument, a message, is unused) */
static void BossMgr_setResult(u32 self, u32 res) {
    WWHD_FUNC(0x02038738, void, self, res);
    st(self + 0x20, ld(res));
}
VERIFY(0x02038738, BossMgr_setResult);

/* 02038744: initialise: own heap (0x96000), worker thread (method 02038C7C), the 0x276-byte work
 * buffer, nn::boss::Initialize */
static void BossMgr_init(u32 self, u32 parent) {
    WWHD_FUNC(0x02038744, void, self, parent);
    gabi::Local<be<u32>[2]> nm;
    st(nm.a + 4, SS_VT);
    st(nm.a + 0, 0x10005528);
    u32 h = gabi::call<u32>(0x02753004, 0x96000u, nm.a, parent, 1u, 0u);
    st(self + 0x14, h);
    u32 th = gabi::call<u32>(0x0273B050, 0x98u, h, 4u);
    if (th != 0) {
        st(nm.a + 4, SS_VT);
        st(nm.a + 0, 0x10005518);
        u32 dg = gabi::call<u32>(0x0273B050, 0x10u, ld(self + 0x14), 4u);
        if (dg != 0) {
            st(dg + 4, self);
            sth(dg + 8, 0);
            st(dg + 0xC, 0x02038C7C);
            sth(dg + 0xA, 0xFFFF);
            st(dg + 0, 0x10005508);
        }
        th = gabi::call<u32>(0x0275FBD8, th, nm.a, dg, ld(self + 0x14), ld(0x101F8B9C), 0u, 0x7FFFFFFFu, 0x4C000u, 0x20u);
    }
    st(self + 0x1C, th);
    gabi::call_ptr<u32>(vfn(th, 0xC, 0x2C), th);
    st(self + 0x10, gabi::call<u32>(0x0273B0D4, 0x276u, ld(self + 0x14), 0x40u));
    gabi::Local<be<u32>[1]> res;
    st(res.a, gabi::call<u32>(boss_Initialize));
    gabi::call(0x02038738, self, res.a, 0x10005534);
    stb(0x1018F484, 1);
}
VERIFY(0x02038744, BossMgr_init);

/* 0203899C: start the thread (state 0 -> 2) */
static void BossMgr_start(u32 self) {
    WWHD_FUNC(0x0203899C, void, self);
    if (ld(self + 0x18) != 0) return;
    u32 t = ld(self + 0x1C);
    gabi::call_ptr<u32>(vfn(t, 0xC, 0x4C), t, 0u);
    st(self + 0x18, 2);
}
VERIFY(0x0203899C, BossMgr_start);

/* 020389F0: destroy the heap */
static void BossMgr_fini(u32 self) {
    WWHD_FUNC(0x020389F0, void, self);
    u32 h = ld(self + 0x14);
    gabi::call_ptr<u32>(vfn(h, 0xC, 0x24), h);
    st(self + 0x14, 0);
    st(self + 0x1C, 0);
    st(self + 0x10, 0);
}
VERIFY(0x020389F0, BossMgr_fini);

/* 02038A3C: request the registration job (state 0 -> 1) */
static u32 BossMgr_request(u32 self) {
    WWHD_FUNC(0x02038A3C, u32, self);
    if (ld(self + 0x18) != 0) return self;
    u32 t = ld(self + 0x1C);
    st(self + 0x18, 1);
    return gabi::call_ptr<u32>(vfn(t, 0xC, 0x1C), t, 1u, 1u);
}
VERIFY(0x02038A3C, BossMgr_request);

/* 02038A6C: setting value I of the save-derived parameter object */
static u32 BossMgr_param(u32 self, u32 i, u32 obj) {
    WWHD_FUNC(0x02038A6C, u32, self, i, obj);
    return gabi::call<u32>(0x02726CE0, obj, i);
}
VERIFY(0x02038A6C, BossMgr_param);

/* 02038A74: fill the task setting with the 21 parameters (keys 1..21) */
static void BossMgr_fillSetting(u32 self, u32 setting) {
    WWHD_FUNC(0x02038A74, void, self, setting);
    u32 obj = gabi::call<u32>(0x0273B050, 0x7Cu, ld(self + 0x14), 4u);
    if (obj != 0) obj = gabi::call<u32>(0x02726A34, obj);
    gabi::call(0x02722E88, ld(0x101F852C), obj);
    for (u32 i = 0; i < 0x15; i++) {
        u32 v = gabi::call<u32>(0x02038A6C, self, i, obj);
        gabi::call(boss_TaskSetting_Set, setting, i + 1, v);
    }
}
VERIFY(0x02038A74, BossMgr_fillSetting);

/* 02038B10: (re)register the SpotPass task and start its scheduling */
static void BossMgr_registerTask(u32 self) {
    WWHD_FUNC(0x02038B10, void, self);
    gabi::Local<be<u32>[0x488]> ts; /* nn::boss::NbdlTaskSetting (0x1220) */
    gabi::Local<be<u32>[8]> task;   /* nn::boss::Task (0x20) */
    gabi::Local<be<u32>[1]> res;
    gabi::call(boss_TaskSetting_ct, ts.a);
    gabi::call(boss_TaskSetting_Initialize, ts.a, ld(self + 0x10), 0x276u);
    gabi::call(0x02038A74, self, ts.a);
    st(res.a, 0xFFFFFFFF);
    gabi::call(boss_Task_ct, task.a);
    const u32 id = 0x10200A6C;
    gabi::call_ptr<u32>(ld(ld(id + 4) + 0x14), id);
    u32 r = gabi::call<u32>(boss_Task_Initialize, task.a, ld(id), 0u);
    st(res.a, r);
    gabi::call(0x02038738, self, res.a, 0x10005568);
    if ((ld(res.a) & 0x80000000u) == 0) {
        bool ok = true;
        if (gabi::call<u32>(boss_Task_IsRegistered, task.a) != 0) {
            r = gabi::call<u32>(boss_Task_Unregister, task.a);
            st(res.a, r);
            gabi::call(0x02038738, self, res.a, 0x10005584);
            ok = (ld(res.a) & 0x80000000u) == 0;
        }
        if (ok) {
            r = gabi::call<u32>(boss_Task_Register, task.a, ts.a);
            st(res.a, r);
            gabi::call(0x02038738, self, res.a, 0x100055A0);
            if ((ld(res.a) & 0x80000000u) == 0) {
                r = gabi::call<u32>(boss_Task_StartScheduling, task.a, 1u);
                st(res.a, r);
                gabi::call(0x02038738, self, res.a, 0x10005554);
            }
        }
    }
    gabi::call(boss_Task_dt, task.a, 2u);
    gabi::call(boss_TaskSetting_dt, ts.a, 2u);
}
VERIFY(0x02038B10, BossMgr_registerTask);

/* 02038C7C: thread message handler */
static void BossMgr_threadProc(u32 self, u32 thread, u32 msg) {
    WWHD_FUNC(0x02038C7C, void, self, thread, msg);
    if (msg != 1) return;
    gabi::call(0x02038B10, self);
    st(self + 0x18, 0);
}
VERIFY(0x02038C7C, BossMgr_threadProc);

/* 02038CBC: the singleton disposer's destructor */
static void BossMgr_disposer_dt(u32 p, u32 flags) {
    WWHD_FUNC(0x02038CBC, void, p, flags);
    if (p == 0) return;
    st(p + 0xC, 0x100055D0);
    if (p == ld(DISP)) {
        u32 inst = ld(INST);
        st(DISP, 0);
        gabi::call(0x020386B8, inst, 2u);
        st(INST, 0);
    }
    gabi::call(0x02752BEC, p, 0u);
    if (flags & 1) op_delete(p);
}
VERIFY(0x02038CBC, BossMgr_disposer_dt);

/* 02038D58: __sinit: header statics and the task id */
static void sinit_02038D58() {
    WWHD_FUNC(0x02038D58, void);
    header_sinit(0x10200A5C, 0x1018F458, 0x100055BC);
    st(0x10200A6C + 4, SS_VT);
    st(0x10200A6C, 0x100055C4);
}
VERIFY(0x02038D58, sinit_02038D58);

static void Dt_02038E0C(u32 p, u32 flags) {
    WWHD_FUNC(0x02038E0C, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x02038E0C, Dt_02038E0C);

static void Empty_02038E20() {
    WWHD_FUNC(0x02038E20, void);
}
VERIFY(0x02038E20, Empty_02038E20);

}  // namespace hd_net_02038498

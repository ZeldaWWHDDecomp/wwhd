/* hd_sys_0203E9EC: the HD RootTask translation unit (cking::system, 0203E9EC..0203F8CB). No GameCube
 * source: HD-only code.
 *
 * 0203E9EC RootTask constructor (0x130, sead task base 02748100); 0203EA88 graphics/system heap setup
 * (a 0x50 resource object with a FixedSafeString<32> name, the GX2 command/memory heaps); 0203EE2C
 * RootTask prepare: a function-local static WPAD/system config, process callbacks, four sead archive
 * resources (sarc) bound into one multi-archive, default/system heaps for the managers (sound, save,
 * Miiverse, picture, UI graphics with the built-in shader archives, fonts), then the child tasks
 * (resource/sound/GPU tasks) through the task manager; 0203F670 RootTask enter (creates the SystemTask
 * and the ProfileTask); 0203F810 __sinit; two deleting destructors.
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_sys_0203E9EC {

/* imports (the 028Fxxxx stubs resolve to these) */
static constexpr u32 WPADEnableURCC = 0xC00083D8;
static constexpr u32 WPADEnableWiiRemote = 0xC00083E8;
static constexpr u32 nn_act_Initialize = 0xC00046E8;
static constexpr u32 nn_act_GetSlotNo = 0xC0004660;
static constexpr u32 MEMGetBaseHeapHandle = 0xC0009790;
static constexpr u32 MEMAllocFromFrmHeapEx = 0xC00096A0;
static constexpr u32 SS_VT = 0x100063F8; /* this TU's SafeString vtable */
typedef be<u32> sstr_l[2];               /* sead::SafeString {cstr, vtable} */
typedef be<u32> targ_l[32];              /* sead task construct argument (0x80) */

static inline void sstr(u32 a, u32 s) { st(a + 4, SS_VT); st(a + 0, s); }
static inline u32 exp_heap(u32 size, u32 name, u32 parent, u32 dir, u32 opt) {
    return gabi::call<u32>(0x02753004, size, name, parent, dir, opt);
}
/* heap virtual at vtable(+0xC) slot 0x2C: adjust / finish */
static inline void heap_v2c(u32 h) { gabi::call_ptr<u32>(vfn(h, 0xC, 0x2C), h); }

/* 0203E9EC: RootTask constructor */
static u32 RootTask_ct(u32 self, u32 arg) {
    WWHD_FUNC(0x0203E9EC, u32, self, arg);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x130);
        if (t == 0) return 0;
    }
    gabi::call(0x02748100, t, arg, 0x10006520);
    st(t + 0x70, 0x100066E0);
    st(t + 0x20, 0x100067A0);
    st(t + 0x12C, 0);
    st(t + 0x128, 0);
    st(t + 0x11C, 0);
    st(t + 0x124, 0);
    st(t + 0x120, 0);
    st(0x104612E0, gabi::call<u32>(0x0202BE64, 0u));
    return t;
}
VERIFY(0x0203E9EC, RootTask_ct);

/* 0203EA88: graphics heaps */
static void RootTask_setupGfxHeaps() {
    WWHD_FUNC(0x0203EA88, void);
    u32 root = gabi::call<u32>(0x0203E8F4);
    u32 obj = op_new(0x50);
    gabi::FrameLocal<sstr_l> n(0x8);
    gabi::FrameLocal<be<u32>[80]> cfg(0x40);
    gabi::FrameLocal<be<u32>[11]> desc(0x14);
    gabi::FrameLocal<be<u32>[1]> h(0x10);
    if (obj != 0) {
        /* the list node at +0 (its own null-allocation path is dead: obj != 0) */
        u32 nd = obj;
        st(nd + 4, 0);
        st(nd + 8, obj);
        st(nd + 0, 0);
        st(nd + 0xC, 0);
        gabi::call(0x02752A84, obj + 0x10);
        st(obj + 0x4C, 0x1015DFE0);
        st(obj + 0x1C, 0x1015E018);
        u32 s = obj + 0x20;
        if (s == 0) s = op_new(0x2C);
        if (s != 0) {
            /* FixedSafeString<32> at +0x20: buffer pointer, vtable, capacity, buffer +0xC */
            u32 b = s;
            if (b == 0) b = op_new(0xC);
            if (b != 0) {
                st(b + 0, s + 0xC);
                st(b + 4, 0x10006410);
                st(b + 8, 0x20);
                stb(s + 0x2B, 0);
            }
            u32 buf = ld(s);
            st(s + 4, 0x10006468);
            stb(buf, 0);
            st(s + 4, 0x10006480);
        }
        st(obj + 0x4C, 0x100064D0);
        st(obj + 0x1C, 0x10006510);
    }
    sstr(n.a, 0x1000652C);
    gabi::call(0x0275D6CC, ld(0x101F8B68), obj, n.a);
    gabi::call(0x027B5534, cfg.a);
    st(cfg.a + 0x08, 0);
    st(cfg.a + 0x2C, 0);
    st(cfg.a + 0x00, root);
    st(cfg.a + 0x28, 0);
    st(cfg.a + 0x14, 0x01FFFF80);
    st(cfg.a + 0x04, 0x00800000);
    st(cfg.a + 0x1C, 0x02000000);
    st(cfg.a + 0x24, 0);
    st(cfg.a + 0x0C, 0);
    sstr(n.a, 0x10006534);
    st(cfg.a + 0, exp_heap(0, n.a, root, 1, 0));
    u32 m = gabi::call<u32>(MEMGetBaseHeapHandle, 0u);
    st(cfg.a + 0x10, gabi::call<u32>(MEMAllocFromFrmHeapEx, m, ld(cfg.a + 0x14), 4u));
    u32 hp = ld(cfg.a + 0);
    st(cfg.a + 0x18, gabi::call_ptr<u32>(vfn(hp, 0xC, 0x34), hp, ld(cfg.a + 0x1C), 0x2000u));
    st(cfg.a + 0x20, 0);
    heap_v2c(ld(cfg.a + 0));
    u32 parent = ld(ld(cfg.a + 0) + 0x24);
    u32 mgr = ld(0x101F8B68);
    st(desc.a + 0x18, 0);
    st(desc.a + 0x14, 0x2000);
    st(desc.a + 0x1C, 0);
    st(desc.a + 0x24, 0);
    st(desc.a + 0x04, SS_VT);
    st(cfg.a + 0, parent);
    st(desc.a + 0x08, 0);
    st(desc.a + 0x10, 0x20);
    st(desc.a + 0x00, 0x10006564);
    st(desc.a + 0x20, 0);
    st(desc.a + 0x28, 0);
    st(desc.a + 0x0C, 0);
    u32 res = gabi::call<u32>(0x0275DB94, mgr, desc.a);
    if (ld(0x101FD6F4) == 0) { st(0x101FD6F4, 1); st(0x101FD9D0, 0x100064A8); }
    if (res != 0) {
        if (gabi::call_ptr<u32>(vfn(res, 0x10, 0xC), res, 0x101FD9D0) == 0) res = 0;
    }
    sstr(n.a, 0x10006580);
    st(cfg.a + 0, exp_heap(0, n.a, ld(cfg.a + 0), 1, 0));
    gabi::call(0x027B56D0, cfg.a);
    heap_v2c(ld(cfg.a + 0));
    st(cfg.a + 0, ld(ld(cfg.a + 0) + 0x24));
    gabi::call(0x027B59E4, 0u);
    gabi::call(0x027B59F4, res);
    sstr(n.a, 0x10006558);
    u32 h3 = exp_heap(0, n.a, root, 1, 0);
    gabi::call(0x0278D258, h.a);
    st(h.a, h3);
    gabi::call(0x0278D294, h.a);
    heap_v2c(h3);
}
VERIFY(0x0203EA88, RootTask_setupGfxHeaps);

/* task construct arguments (sead::TaskBase::CreateArg): built by CTOR into SRC, copied, the
 * create callback CB stored at +0x7C of the copy, then created by the task manager */
static inline u32 create_task(u32 self, u32 src, u32 dst, u32 cb) {
    for (u32 i = 0; i < 32; i++) st(dst + i * 4, ld(src + i * 4));
    st(dst + 0x7C, cb);
    return gabi::call<u32>(0x02749ECC, ld(self + 0x5C), dst);
}
static inline void rtti_call(u32 t, u32 guard, u32 obj, u32 val) {
    if (ld(guard) == 0) { st(obj, val); st(guard, 1); }
    if (t != 0) gabi::call_ptr<u32>(vfn(t, 0x70, 0xC), t, obj);
}

/* 0203EE2C: RootTask prepare */
static void RootTask_prepare(u32 self) {
    WWHD_FUNC(0x0203EE2C, void, self);
    const u32 cfg = 0x104612BC;
    u32 fl;
    if (ld(0x104612B8) != 0) {
        fl = ld(cfg + 0x1C);
    } else {
        st(0x104612B8, 1);
        st(cfg + 0x14, 1);
        stb(cfg + 0x20, 1);
        st(cfg + 0, 0x100064B8);
        st(cfg + 0x10, 0x2332);
        st(cfg + 0x18, 0x4D2);
        fl = 0x140D;
    }
    st(cfg + 8, 0x40000);
    st(cfg + 0xC, 0x80000);
    st(cfg + 4, 0x200000);
    st(cfg + 0x1C, (fl & 0xFFFFFFEEu) | 0x20);
    gabi::call(0x02035E2C);
    gabi::call(0x02032B48);
    u32 r = gabi::call<u32>(0x02035D78);
    gabi::call(0x02035ED4, r, 0u);
    gabi::FrameLocal<be<u32>[4]> l70(0x70);
    st(l70.a + 0, cfg);
    st(l70.a + 8, 0);
    st(l70.a + 0xC, 0);
    gabi::call(WPADEnableURCC, 1u);
    gabi::call(WPADEnableWiiRemote, 0u);
    u32 o = ld(ld(self + 0x5C) + 0x40);
    gabi::call_ptr<u32>(vfn(o, 0x24, 0x2C), o, self, l70.a);
    gabi::call(WPADEnableURCC, 1u);
    gabi::call(WPADEnableWiiRemote, 0u);
    gabi::call(0x026177B8, 0u);
    /* four archive resources and the multi-archive over them */
    u32 dev = ld(ld(0x101F8B08) + 0x1C);
    gabi::FrameLocal<sstr_l> s8(0x8);
    gabi::FrameLocal<sstr_l> s10(0x10);
    sstr(s8.a, 0x10006624);
    sstr(s10.a, 0x1000659C);
    st(self + 0x124, gabi::call<u32>(0x0273EFF8, 0u, s10.a, dev, s8.a));
    sstr(s8.a, 0x10006630);
    sstr(s10.a, 0x1000663C);
    st(self + 0x128, gabi::call<u32>(0x0273EFF8, 0u, s8.a, dev, s10.a));
    sstr(s8.a, 0x100065A8);
    sstr(s10.a, 0x10006590);
    st(self + 0x120, gabi::call<u32>(0x0273EFF8, 0u, s8.a, dev, s10.a));
    sstr(s8.a, 0x10006598);
    sstr(s10.a, 0x100065A4);
    st(self + 0x11C, gabi::call<u32>(0x0273EFF8, 0u, s8.a, dev, s10.a));
    u32 multi = gabi::call<u32>(0x0273F9E0, 0u, 4u, 0u, 4u);
    st(self + 0x12C, multi);
    gabi::call(0x0273FBB8, multi, ld(self + 0x124));
    gabi::call(0x0273FBB8, ld(self + 0x12C), ld(self + 0x128));
    gabi::call(0x0273FBB8, ld(self + 0x12C), ld(self + 0x120));
    gabi::call(0x0273FBB8, ld(self + 0x12C), ld(self + 0x11C));
    st(ld(0x101F8B08) + 0x1C, ld(self + 0x12C));
    /* two mount points */
    gabi::FrameLocal<sstr_l> s38(0x38);
    gabi::FrameLocal<sstr_l> s40(0x40);
    u32 d1 = gabi::call<u32>(0x0274468C, 0u, 0xFFu);
    sstr(s38.a, 0x100065C0);
    gabi::call(0x02741770, ld(0x101F8B08), d1, s38.a);
    gabi::call(nn_act_Initialize);
    u32 x = gabi::call<u32>(nn_act_GetSlotNo);
    u32 d2 = gabi::call<u32>(0x0274468C, 0u, x);
    sstr(s40.a, 0x1000664C);
    gabi::call(0x02741770, ld(0x101F8B08), d2, s40.a);
    gabi::call(0x027507E8, 0u);
    st(s8.a + 4, SS_VT);
    u32 m8b28 = ld(0x101F8B28);
    u32 hsel = ld(self + 0x44 + ld(self + 0x58) * 4);
    st(s8.a + 0, 0x10006658);
    gabi::call(0x02750940, m8b28, hsel, s8.a);
    u32 g = gabi::call<u32>(0x02748974, self);
    st(ld(g + 0x1C) + 0x3DC, 0);
    gabi::call(0x0203EA88);
    gabi::call(0x02617858, ld(0x101F5088), 0x104A0CD8, 0u);
    /* manager heaps */
    gabi::FrameLocal<sstr_l> s18(0x18);
    sstr(s18.a, 0x100065B0);
    u32 root = gabi::call<u32>(0x0203E8F4);
    u32 h = exp_heap(0xB00000, s18.a, root, 1, 0);
    gabi::call(0x0260FBF4, h);
    gabi::call(0x0260FC98, ld(0x101F4F7C), h);
    u32 sz = gabi::call<u32>(0x0275320C, 4u) + 0xE00000;
    sstr(s18.a, 0x100065CC);
    root = gabi::call<u32>(0x0203E8F4);
    u32 h25 = exp_heap(sz, s18.a, root, 1, 0);
    u32 sz2 = 0x16C00000 - sz;
    sstr(s18.a, 0x10006688);
    root = gabi::call<u32>(0x0203E8F4);
    u32 h26 = exp_heap(sz2, s18.a, root, 1, 1);
    u32 sz3 = gabi::call<u32>(0x0275320C, 0x2000u) + 0x0D400000;
    sstr(s18.a, 0x100065DC);
    u32 h24 = exp_heap(sz3, s18.a, h26, 0xFFFFFFFFu, 0);
    gabi::call(0x02603680, h26);
    u32 sv = ld(0x101F4F28);
    gabi::call(0x0260ABF8, sv, h26);
    st(sv + 0x2044, h25);
    st(sv + 0x2040, h26);
    st(sv + 0x2128, h24);
    gabi::FrameLocal<sstr_l> s20(0x20);
    gabi::FrameLocal<sstr_l> s48(0x48);
    sstr(s20.a, 0x100065B8);
    root = gabi::call<u32>(0x0203E8DC);
    u32 h6 = exp_heap(0x100000, s20.a, root, 1, 0);
    st(s48.a + 4, SS_VT);
    sstr(s20.a, 0x100065B8);
    st(s48.a + 0, 0x100065F0);
    gabi::call(0x02612E64, ld(0x101F4F7C), s20.a, s48.a, h6, 1u, 0x40000u, 0x2000u);
    sstr(s8.a, 0x100065B8);
    sstr(s10.a, 0x10006694);
    u32 rr = gabi::call<u32>(0x026124B0, ld(0x101F4F7C), s8.a, s10.a, 0u);
    u32 arc = gabi::call<u32>(0x027E2DC0, rr);
    gabi::FrameLocal<sstr_l> s50(0x50);
    sstr(s50.a, 0x10006600);
    root = gabi::call<u32>(0x0203E8E8);
    u32 gh = exp_heap(0x2000000, s50.a, root, 1, 0);
    gabi::call(0x02005B90, gh);
    /* the two built-in shader binaries of the archive: {begin, end} ranges */
    gabi::FrameLocal<be<u32>[2]> r28(0x28);
    gabi::FrameLocal<be<u32>[2]> r30(0x30);
    st(r28.a + 4, 0);
    st(r28.a + 0, 0);
    u32 off = ld(arc + 0x4C);
    u32 base = off != 0 ? arc + 0x4C + off : 0;
    u32 f = gabi::call<u32>(0x027DFA24, base, 0x100066A0);
    u32 fo = ld(f), fs = ld(f + 4);
    u32 b1 = fo != 0 ? f + fo : 0;
    st(r30.a + 4, 0);
    st(r28.a + 4, b1 + fs);
    st(r30.a + 0, 0);
    st(r28.a + 0, b1);
    off = ld(arc + 0x4C);
    base = off != 0 ? arc + 0x4C + off : 0;
    f = gabi::call<u32>(0x027DFA24, base, 0x1000660C);
    fo = ld(f); fs = ld(f + 4);
    u32 b2 = fo != 0 ? f + fo : 0;
    st(r30.a + 0, b2);
    st(r30.a + 4, b2 + fs);
    gabi::call(0x02005C38, ld(0x1018C404), gh, r28.a, r30.a);
    gabi::FrameLocal<sstr_l> s58(0x58);
    sstr(s58.a, 0x100066B8);
    root = gabi::call<u32>(0x0203E8E8);
    u32 h7 = exp_heap(0x5C00000, s58.a, root, 1, 0);
    gabi::call(0x026FB344, h7);
    gabi::call(0x026FB3F0, ld(0x101F7274), h7);
    u32 ph = gabi::call<u32>(0x0203E8B8);
    gabi::call(0x02720A5C, ph);
    gabi::call(0x02720B34, ld(0x101F852C), ph);
    sstr(s8.a, 0x100066C8);
    u32 fh = gabi::call<u32>(0x02754B38, 0u, s8.a, ph, 1u, 0u);
    u32 q = gabi::call<u32>(0x02720400, fh);
    heap_v2c(fh);
    gabi::call(0x02720510, q);
    gabi::call(0x020360FC, 0u);
    gabi::call(0x020361E4, ld(0x1018F3F4), 0u);
    /* child tasks */
    gabi::FrameLocal<be<u32>[2]> id(0x60);
    gabi::FrameLocal<targ_l> src(0x80);
    gabi::FrameLocal<targ_l> dst1(0x100);
    gabi::FrameLocal<targ_l> dst2(0x180);
    st(id.a + 0, 2);
    st(id.a + 4, 0x0203FAF4);
    gabi::call(0x02748CFC, src.a, id.a);
    u32 hp = gabi::call<u32>(0x0203E8AC);
    st(src.a + 8 + ld(src.a + 0x58) * 0x14, hp);
    u32 ent = src.a + 8 + ld(src.a + 0x58) * 0x14;
    u32 k = gabi::call<u32>(0x0275320C, 4u);
    u32 pr = ld(0x104612E0);
    st(ent + 4, 0x04000000 - k);
    st(src.a + 0x5C, self);
    st(src.a + 0x60, ld(pr + 0x14));
    u32 t = create_task(self, src.a, dst1.a, 0x02760FE4);
    rtti_call(t, 0x101FD59C, 0x101FD9D4, 0x10006458);
    gabi::call(0x027488E8, self);
    gabi::FrameLocal<be<u32>[2]> id2(0x68);
    st(id2.a + 0, 2);
    st(id2.a + 4, 0x0203FB48);
    gabi::call(0x02748B98, src.a, id2.a);
    t = create_task(self, src.a, dst2.a, 0x02729488);
    rtti_call(t, 0x101FD71C, 0x101FD9D8, 0x10006448);
    r = gabi::call<u32>(0x02035D78);
    gabi::call(0x02035ED4, r, 1u);
}
VERIFY(0x0203EE2C, RootTask_prepare);

/* 0203F670: RootTask enter: SystemTask (0203FB9C) and ProfileTask (0203FBF0) */
static void RootTask_enter(u32 self) {
    WWHD_FUNC(0x0203F670, void, self);
    gabi::FrameLocal<be<u32>[2]> id(0x8);
    gabi::FrameLocal<targ_l> src(0x118);
    gabi::FrameLocal<targ_l> dst1(0x18);
    st(id.a + 0, 2);
    st(id.a + 4, 0x0203FB9C);
    gabi::call(0x02748CFC, src.a, id.a);
    u32 t = create_task(self, src.a, dst1.a, 0x0203FC78);
    rtti_call(t, 0x101FD598, 0x101FD97C, 0x10006448);
    gabi::FrameLocal<be<u32>[2]> id2(0x10);
    gabi::FrameLocal<targ_l> dst2(0x98);
    st(id2.a + 0, 2);
    st(id2.a + 4, 0x0203FBF0);
    gabi::call(0x02748CFC, src.a, id2.a);
    t = create_task(self, src.a, dst2.a, 0x0203E360);
    rtti_call(t, 0x101FD598, 0x101FD97C, 0x10006448);
    gabi::call(0x0276198C, ld(0x101F8BA8), 0u);
}
VERIFY(0x0203F670, RootTask_enter);

static void sinit_0203F810() {
    WWHD_FUNC(0x0203F810, void);
    header_sinit(0x104612A8, 0x1018F560, 0x100066D4);
}
VERIFY(0x0203F810, sinit_0203F810);

static void Dt_0203F8A4(u32 p, u32 flags) {
    WWHD_FUNC(0x0203F8A4, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203F8A4, Dt_0203F8A4);

static void Dt_0203F8B8(u32 p, u32 flags) {
    WWHD_FUNC(0x0203F8B8, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203F8B8, Dt_0203F8B8);

}  // namespace hd_sys_0203E9EC

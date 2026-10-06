/* hd_sys_0203E584: the HD root-heap translation unit (cking::system, 0203E584..0203E9EB; an earlier map had it
 * start at 0203E498, but 0203E498..0203E583 are the ProfileTask TU's companions after its __sinit).
 * No GameCube source: HD-only code.
 *
 * Thirteen named expanded heaps directly under the sead root heap ("SoundRootHeap", "SaveRootHeap",
 * "MiiverseRootHeap", "PictoRootHeap", ...): reset of the thirteen heap globals 10461268..10461298, the
 * size table (13 words), the create helper and the creation of all heaps, eleven accessors, the TU's
 * __sinit and its companions (a deleting destructor, an empty virtual, two this-adjusting thunks of the
 * SystemTask TU's destructors). */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_sys_0203E584 {

static constexpr u32 HEAPS = 0x10461268; /* 13 heap pointers */

/* 0203E584: reset all heap globals */
static void RootHeap_reset() {
    WWHD_FUNC(0x0203E584, void);
    for (u32 i = 0; i < 13; i++) st(HEAPS + i * 4, 0);
}
VERIFY(0x0203E584, RootHeap_reset);

/* 0203E5F4: the heap size table */
static void RootHeap_getSizes(u32 out) {
    WWHD_FUNC(0x0203E5F4, void, out);
    static const u32 sz[13] = {0x04000000, 0x01100000, 0x01100000, 0x01400000, 0x00400000, 0x1F200000, 0x08000000,
                               0x00100000, 0x00A00000, 0x03000000, 0x00F00000, 0x00200000, 0x00100000};
    for (u32 i = 0; i < 13; i++) st(out + i * 4, sz[i]);
}
VERIFY(0x0203E5F4, RootHeap_getSizes);

/* 0203E67C: create one expanded heap under the sead root heap (sead::HeapMgr::sRootHeaps[0] when set) */
static u32 RootHeap_create(u32 size, u32 name) {
    WWHD_FUNC(0x0203E67C, u32, size, name);
    u32 parent = 0;
    if (ld(0x104A030C) != 0) parent = ld(ld(0x104A0314));
    return gabi::call<u32>(0x02753004, size, name, parent, 1u, 1u);
}
VERIFY(0x0203E67C, RootHeap_create);

/* 0203E6A4: create all thirteen heaps (size index, name, global slot) */
static void RootHeap_createAll() {
    WWHD_FUNC(0x0203E6A4, void);
    static const u32 tab[13][3] = {
        {0, 0x10006320, 0}, {1, 0x10006350, 1}, {2, 0x10006360, 2}, {3, 0x10006330, 3}, {4, 0x10006374, 4},
        {6, 0x10006390, 5}, {5, 0x1000639C, 6}, {7, 0x100063A8, 7}, {8, 0x100063B8, 8}, {9, 0x100063C8, 9},
        {10, 0x10006310, 10}, {11, 0x100063D8, 11}, {12, 0x10006340, 12}};
    gabi::Local<be<u32>[2]> nm;
    gabi::Local<be<u32>[13]> sz;
    gabi::call(0x0203E5F4, sz.a);
    for (u32 i = 0; i < 13; i++) {
        st(nm.a + 4, 0x100062F8);
        st(nm.a + 0, tab[i][1]);
        st(HEAPS + tab[i][2] * 4, gabi::call<u32>(0x0203E67C, ld(sz.a + tab[i][0] * 4), nm.a));
    }
}
VERIFY(0x0203E6A4, RootHeap_createAll);

/* accessors */
static u32 RootHeap_get0() {
    WWHD_FUNC(0x0203E8AC, u32);
    return ld(HEAPS + 0 * 4);
}
VERIFY(0x0203E8AC, RootHeap_get0);
static u32 RootHeap_get1() {
    WWHD_FUNC(0x0203E8B8, u32);
    return ld(HEAPS + 1 * 4);
}
VERIFY(0x0203E8B8, RootHeap_get1);
static u32 RootHeap_get2() {
    WWHD_FUNC(0x0203E8C4, u32);
    return ld(HEAPS + 2 * 4);
}
VERIFY(0x0203E8C4, RootHeap_get2);
static u32 RootHeap_get3() {
    WWHD_FUNC(0x0203E8D0, u32);
    return ld(HEAPS + 3 * 4);
}
VERIFY(0x0203E8D0, RootHeap_get3);
static u32 RootHeap_get4() {
    WWHD_FUNC(0x0203E8DC, u32);
    return ld(HEAPS + 4 * 4);
}
VERIFY(0x0203E8DC, RootHeap_get4);
static u32 RootHeap_get5() {
    WWHD_FUNC(0x0203E8E8, u32);
    return ld(HEAPS + 5 * 4);
}
VERIFY(0x0203E8E8, RootHeap_get5);
static u32 RootHeap_get6() {
    WWHD_FUNC(0x0203E8F4, u32);
    return ld(HEAPS + 6 * 4);
}
VERIFY(0x0203E8F4, RootHeap_get6);
static u32 RootHeap_get7() {
    WWHD_FUNC(0x0203E900, u32);
    return ld(HEAPS + 7 * 4);
}
VERIFY(0x0203E900, RootHeap_get7);
static u32 RootHeap_get8() {
    WWHD_FUNC(0x0203E90C, u32);
    return ld(HEAPS + 8 * 4);
}
VERIFY(0x0203E90C, RootHeap_get8);
static u32 RootHeap_get9() {
    WWHD_FUNC(0x0203E918, u32);
    return ld(HEAPS + 9 * 4);
}
VERIFY(0x0203E918, RootHeap_get9);
static u32 RootHeap_get12() {
    WWHD_FUNC(0x0203E924, u32);
    return ld(HEAPS + 12 * 4);
}
VERIFY(0x0203E924, RootHeap_get12);

static void sinit_0203E930() {
    WWHD_FUNC(0x0203E930, void);
    header_sinit(0x10461258, 0x1018F53C, 0x100063EC);
}
VERIFY(0x0203E930, sinit_0203E930);

static void Dt_0203E9C4(u32 p, u32 flags) {
    WWHD_FUNC(0x0203E9C4, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203E9C4, Dt_0203E9C4);

static void Empty_0203E9D8() {
    WWHD_FUNC(0x0203E9D8, void);
}
VERIFY(0x0203E9D8, Empty_0203E9D8);

/* this-adjusting thunks (secondary bases at +0x10 / +0x14) */
static void Thunk_0203E9DC(u32 p, u32 flags) {
    WWHD_FUNC(0x0203E9DC, void, p, flags);
    gabi::call(0x0203F8CC, p - 0x10, flags);
}
VERIFY(0x0203E9DC, Thunk_0203E9DC);

static void Thunk_0203E9E4(u32 p, u32 flags) {
    WWHD_FUNC(0x0203E9E4, void, p, flags);
    gabi::call(0x0203F92C, p - 0x14, flags);
}
VERIFY(0x0203E9E4, Thunk_0203E9E4);

}  // namespace hd_sys_0203E584

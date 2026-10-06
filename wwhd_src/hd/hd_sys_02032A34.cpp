/* hd_sys_02032A34: two HD system TUs (no GameCube source).
 *
 * 02032A34..02032C77: the screen-dimming switch (0x40: enabled flag +0, sead mutex +4; function-local
 *                     static instance 102007A4 published in 1018F270): ctor/dtor, set (coreinit
 *                     IMEnableDim / IMDisableDim under the mutex), get, the static accessor that
 *                     reads the system state with IMIsDimEnabled; __sinit 02032BD8 + accessor companion
 * 02032C78..02032D0B: initialiser-only TU
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_sys_02032A34 {

static u32 Dim_ct(u32 self) {
    WWHD_FUNC(0x02032A34, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x40);
        if (t == 0) return 0;
    }
    stb(t, 1);
    gabi::call(0x02760084, t + 4);
    return t;
}
VERIFY(0x02032A34, Dim_ct);

static void Dim_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02032A84, void, self, flags);
    if (self == 0) return;
    gabi::call(0x02760168, self + 4, 2);
    if (flags & 1) op_delete(self);
}
VERIFY(0x02032A84, Dim_dt);

/* 02032AD8: enable / disable screen dimming */
static void Dim_set(u32 self, u32 on) {
    WWHD_FUNC(0x02032AD8, void, self, on);
    gabi::call(0x027601BC, self + 4);
    stb(self, (u8)on);
    if (on != 0) gabi::call(0xC0009020);   /* IMEnableDim */
    else gabi::call(0xC0009010);           /* IMDisableDim */
    gabi::call(0x027601F0, self + 4);
}
VERIFY(0x02032AD8, Dim_set);

/* 02032B48: create the instance on first use, then sync it with the system setting */
static void Dim_sync() {
    WWHD_FUNC(0x02032B48, void);
    if (ld(0x102007F4) == 0) {
        st(0x102007F4, 1);
        gabi::call(0x02032A34, 0x102007A4);
        gabi::call(0x028F026C, 0x1018F240);
    }
    st(0x1018F270, 0x102007A4);
    gabi::Local<be<u32>> v;
    gabi::call(0xC0009060, v.a);                /* IMIsDimEnabled */
    u32 on = ld(v.a);
    u32 inst = ld(0x1018F270);
    gabi::call(0x02032AD8, inst, on == 1 ? 1 : 0);
}
VERIFY(0x02032B48, Dim_sync);

static u32 Dim_get(u32 self) {
    WWHD_FUNC(0x02032BD0, u32, self);
    return lbz(self);
}
VERIFY(0x02032BD0, Dim_get);

/* 02032BD8: __sinit (header statics; this TU keeps its -pi/pi pair and byte objects before the dim object) */
static void sinit_02032BD8() {
    WWHD_FUNC(0x02032BD8, void);
    header_sinit_at(0x102007E4, 0x1018F24C, 0x10004A50, 0x10200798, 0x102007A0);
}
VERIFY(0x02032BD8, sinit_02032BD8);

static u32 Dim_instance() {
    WWHD_FUNC(0x02032C6C, u32);
    return ld(0x1018F270);
}
VERIFY(0x02032C6C, Dim_instance);

static void sinit_02032C78() {
    WWHD_FUNC(0x02032C78, void);
    header_sinit(0x10200804, 0x1018F274, 0x10004A58);
}
VERIFY(0x02032C78, sinit_02032C78);

}  // namespace hd_sys_02032A34

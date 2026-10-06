/* hd_snd_020302E4: two HD sound TUs (no GameCube source).
 *
 * 020302E4..0203054B: the HD sound archive (0x40, NW4F file sound archive 027650B4; vtable 100046E4):
 *                     sead RTTI checkDerived, id-by-name forwarder, constructor (path "sound path 0",
 *                     static name SafeString 10200408, 2.0 / 0x28000 / 8 settings), sound-group loading
 *                     by name (GROUP_STATIC, GROUP_BGM_WDS); __sinit 02030480 + companions
 * 0203054C..020306EF: the HD sound heap object (0x1D0, NW4F 0276231C; vtables 10004788 / 10004778)
 *                     constructor and destructor; __sinit 0203065C
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_snd_020302E4 {

/* 020302E4: checkDerivedRuntimeTypeInfo: this class or its parent */
static u32 Archive_isDerived(u32 self, u32 ti) {
    WWHD_FUNC(0x020302E4, u32, self, ti);
    if (ld(0x101FD634) == 0) {
        st(0x101FD634, 1);
        st(0x101FD950, 0x100044A0);
    }
    if (ti == 0x101FD950) return 1;
    if (ld(0x101FD2D8) == 0) {
        st(0x101FD2D8, 1);
        st(0x101FD954, 0x10004490);
    }
    return ti == 0x101FD954 ? 1 : 0;
}
VERIFY(0x020302E4, Archive_isDerived);

static u32 Archive_idByName(u32 self, u32 name) {
    WWHD_FUNC(0x0203035C, u32, self, name);
    return gabi::call<u32>(0x0288E91C, ld(self + 4), name);
}
VERIFY(0x0203035C, Archive_idByName);

static u32 Archive_ct(u32 self) {
    WWHD_FUNC(0x02030364, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x40);
        if (t == 0) return 0;
    }
    gabi::call(0x027650B4, t);
    st(t + 0, 0x100046E4);
    gabi::call(0x02765450, t, 0x10200408);
    u32 path = gabi::call<u32>(0x0202EA80);
    gabi::call(0x0276543C, t, path);
    gabi::call(0x02765424, t, ldf(0x1000471C));
    gabi::call(0x0276542C, t, 0x28000);
    gabi::call(0x02765434, t, 8);
    return t;
}
VERIFY(0x02030364, Archive_ct);

static u32 Archive_soundArchive() {
    WWHD_FUNC(0x02030404, u32);
    return ld(gabi::call<u32>(0x0202DAD0) + 0x144);
}
VERIFY(0x02030404, Archive_soundArchive);

/* 02030428: load the sound group NAME */
static u32 Archive_loadGroup(u32 self, u32 name) {
    WWHD_FUNC(0x02030428, u32, self, name);
    u32 a = gabi::call<u32>(0x02030404);
    return gabi::call<u32>(0x02764B90, a, name, -1, 0, 0);
}
VERIFY(0x02030428, Archive_loadGroup);

static u32 Archive_loadStatic(u32 self) {
    WWHD_FUNC(0x02030468, u32, self);
    return gabi::call<u32>(0x02030428, self, 0x10004720); /* "GROUP_STATIC" */
}
VERIFY(0x02030468, Archive_loadStatic);

static u32 Archive_loadBgmWds(u32 self) {
    WWHD_FUNC(0x02030474, u32, self);
    return gabi::call<u32>(0x02030428, self, 0x10004730); /* "GROUP_BGM_WDS" */
}
VERIFY(0x02030474, Archive_loadBgmWds);

/* 02030480: __sinit (header statics + the archive name SafeString) */
static void sinit_02030480() {
    WWHD_FUNC(0x02030480, void);
    header_sinit(0x1020041C, 0x1018EDC0, 0x10004740);
    st(0x1020040C, 0x100046BC);
    st(0x10200408, 0x10004748);
}
VERIFY(0x02030480, sinit_02030480);

static void Comp_dt1(u32 self, s32 flags) {
    WWHD_FUNC(0x02030534, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02030534, Comp_dt1);

static void Comp_empty() {
    WWHD_FUNC(0x02030548, void);
}
VERIFY(0x02030548, Comp_empty);

static void Heap_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x0203054C, void, self, flags);
    if (self == 0) return;
    gabi::call(0x027651A4, self, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x0203054C, Heap_dt);

struct w40_l { u8 b[0x28]; };

/* 020305A0: sound heap object constructor */
static u32 Heap_ct(u32 self) {
    WWHD_FUNC(0x020305A0, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x1D0);
        if (t == 0) return 0;
    }
    gabi::call(0x0276231C, t);
    st(t + 0, 0x10004788);
    st(t + 4, 0x10004778);
    gabi::call(0x02762CE8, t, 1, 0);
    gabi::call(0x02762CE0, t, 0);
    gabi::call(0x02762D50, t, 0);
    gabi::call(0x02762D48, t, 0);
    gabi::Local<w40_l> l;
    gabi::call(0x02765EF0, l.a);
    st(l.a, 0);
    gabi::call(0x02766F1C, t + 0xFC, l.a);
    return t;
}
VERIFY(0x020305A0, Heap_ct);

static void sinit_0203065C() {
    WWHD_FUNC(0x0203065C, void);
    header_sinit(0x10200438, 0x1018EDE4, 0x10004818);
}
VERIFY(0x0203065C, sinit_0203065C);

}  // namespace hd_snd_020302E4

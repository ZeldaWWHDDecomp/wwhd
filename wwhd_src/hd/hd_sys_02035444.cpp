/* hd_sys_02035444: an HD root-task factory TU (no GameCube source).
 *
 * TU 02035444..020355A3 (__sinit 02035510): the RTTI check of a task class (101FD998 / parent
 * 101FD994) and its sead TaskFactory function (0xE8 bytes from the current heap of the task
 * construction argument, constructor 0273C734).
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_sys_02035444 {

/* 02035444: checkDerivedRuntimeTypeInfo: this class or its parent */
static u32 isDerived(u32 self, u32 ti) {
    WWHD_FUNC(0x02035444, u32, self, ti);
    if (ld(0x101FD658) == 0) {
        st(0x101FD658, 1);
        st(0x101FD998, 0x10004E24);
    }
    if (ti == 0x101FD998) return 1;
    if (ld(0x101FD2E0) == 0) {
        st(0x101FD2E0, 1);
        st(0x101FD994, 0x10004E14);
    }
    return ti == 0x101FD994 ? 1 : 0;
}
VERIFY(0x02035444, isDerived);

/* 020354BC: task factory: new (heap of ARG's heap array at index +0x14, alignment 4) Task(ARG) */
static u32 factory(u32 arg) {
    WWHD_FUNC(0x020354BC, u32, arg);
    u32 c = ld(arg);
    u32 heap = ld(c + ld(c + 0x14) * 4);
    u32 p = gabi::call<u32>(0x0273B050, 0xE8, heap, 4);
    if (p == 0) return 0;
    return gabi::call<u32>(0x0273C734, p, arg);
}
VERIFY(0x020354BC, factory);

static void sinit_02035510() {
    WWHD_FUNC(0x02035510, void);
    header_sinit(0x10200900, 0x1018F314, 0x10004F80);
}
VERIFY(0x02035510, sinit_02035510);

}  // namespace hd_sys_02035444

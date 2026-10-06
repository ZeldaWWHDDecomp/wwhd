/* hd_sinit_02035FD8: an initialiser-only HD TU (no GameCube source).
 *
 * TU 02035FD8..02036083: only the header __sinit (object 10200998) and the two companion stubs.
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_sinit_02035FD8 {

static void sinit_02035FD8() {
    WWHD_FUNC(0x02035FD8, void);
    header_sinit(0x10200998, 0x1018F3AC, 0x1000510C);
}
VERIFY(0x02035FD8, sinit_02035FD8);

static void Comp_dt1(u32 self, s32 flags) {
    WWHD_FUNC(0x0203606C, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0203606C, Comp_dt1);

static void Comp_empty() {
    WWHD_FUNC(0x02036080, void);
}
VERIFY(0x02036080, Comp_empty);

}  // namespace hd_sinit_02035FD8

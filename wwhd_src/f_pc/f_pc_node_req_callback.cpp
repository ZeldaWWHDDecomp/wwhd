/* Supplemental verification view of the same HD Handler, not an additional TU entry.
 * Actual tag-removal callback tests whether callback effects can change its embedded next link. */
#include "bindings.h"
namespace f_pc_node_req_callback_cpp {
static s32 Handler() {
    WWHD_FUNC(0x025E062C, s32, (u32)0);
    u32 node = gabi::load<u32>(0x101F3DA4);
    while (node != 0) {
        u32 req = gabi::load<u32>(node + 0xC);
        u32 methods = gabi::load<u32>(req + 0x3C);
        u32 result = gabi::call_ptr<u32>(gabi::load<u32>(methods), req);
        node = gabi::load<u32>(node + 8);
        if (result == 3 || result == 5) {
            if (gabi::call<s32>(0x025E05C4, req) == 0) return 0;
        } else if (result == 4) {
            if (gabi::call<s32>(0x025E0538, req) == 0) return 0;
        }
    }
    return 1;
}
VERIFY(0x025E062C, Handler);
}

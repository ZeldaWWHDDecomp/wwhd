/* d_a_wall_static.cpp (WWHD): per-TU SafeString termination hook (empty).
 */
#include "gabi.h"
namespace wall {
void assureSafeStringTermination(u32 string) {
    WWHD_FUNC(0x024D4954, void, string);
}
VERIFY(0x024D4954, assureSafeStringTermination);
}

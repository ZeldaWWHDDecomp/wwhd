/* Utility-TU runtime identity query. These uninitialized views borrow the
 * object-base identity owned by the common AGL runtime-type implementation. */
#include "wwhd.h"

static u32 sBorrowedObjectBaseGuard;
static void* sBorrowedObjectBaseNode;
static u32 sBorrowedObjectBaseMethods;

namespace agl { namespace utility_type {
/* 027C4BA8 */
bool matchesObjectBaseType(void*, const void* type) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sBorrowedObjectBaseGuard == 0) {
        sBorrowedObjectBaseGuard = 1;
        sBorrowedObjectBaseNode = &sBorrowedObjectBaseMethods;
    }
    return type == &sBorrowedObjectBaseNode;
}
} }

/* WWHD glue
data sBorrowedObjectBaseGuard 101FD7C0
data sBorrowedObjectBaseNode 101FDCE8
data sBorrowedObjectBaseMethods 10169F3C
*/

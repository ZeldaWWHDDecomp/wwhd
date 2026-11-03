/** JSU list compiler entries; private decomp source. */
#include "bindings.h"
#include "JSystem/JSupport/JSUList_base.h"
/* 027EC4F4 */
static void JSUPtrListBase__dt(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) operator_delete(object);
}
/* 027EC508 */
static void JSUPtrListBase_reserved() {}
/* WWHD glue
*/

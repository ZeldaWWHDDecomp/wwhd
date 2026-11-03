/* Key comparison adapter. Only the fields read by this function are modeled;
 * the prefix bytes do not assert an enclosing class identity. */
#include "agl/UtilityKeyPredicate.h"

/* 027CAA10 */
bool agl::UtilityKeyPredicate_l::matches(const gptr<UtilityKeyRecord_l>* entry) const {
    gabi::AutoFrame frame{gabi::current_function()};
    const UtilityKeyRecord_l* record = *entry;
    u32 key = mKey;
    return static_cast<u32>(record->mKey) == key;
}

/* WWHD glue
*/

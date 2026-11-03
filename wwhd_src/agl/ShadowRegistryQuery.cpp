// The registry query's meaning is left to its external owner; preserve its two-call flow.
#include "agl/ShadowRegistryQuery.h"
#include "bindings.h"
using namespace agl::shadow_registry;

/* 0278D258 */
QuerySlot* QuerySlot::create(QuerySlot* slot) {
    gabi::AutoFrame frame{0x0278D258u};
    if (slot == nullptr) slot = static_cast<QuerySlot*>(operator_new(sizeof(QuerySlot)));
    if (slot != nullptr) slot->mValue = 0;
    return slot;
}

/* 0278D294 */
u32 QuerySlot::queryRegistry() const {
    gabi::AutoFrame frame{0x0278D294u};
    u32 value = mValue;
    u32 result = 1;
    if (value != 0) result = queryShadowRegistryValue(sRegistryManager.get(), value);
    if (result != 1) result = queryShadowRegistryValue(sRegistryManager.get(), result);
    return result;
}

/* WWHD glue
ext 02756170 u32 queryShadowRegistryValue(sead::RegistryManagerState*,u32)
*/

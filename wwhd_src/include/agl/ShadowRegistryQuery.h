// Four-byte argument slot retained with the shadow helpers.
#pragma once
#include "sead/HeapRegistry.h"
namespace agl::shadow_registry {
struct QuerySlot {
    be<u32> mValue;
    static QuerySlot* create(QuerySlot* slot);
    u32 queryRegistry() const;
};
}
WWHD_OFFSET(agl::shadow_registry::QuerySlot, mValue, 0);
WWHD_SIZE(agl::shadow_registry::QuerySlot, 4);
u32 queryShadowRegistryValue(sead::RegistryManagerState*, u32);

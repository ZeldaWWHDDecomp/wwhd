#pragma once
#include "wwhd.h"

namespace agl {
struct UtilityKeyRecord_l {
    u8 mPrefix[8];
    be<u32> mKey;
};
WWHD_OFFSET(UtilityKeyRecord_l, mKey, 8);
WWHD_SIZE(UtilityKeyRecord_l, 12);

struct UtilityKeyPredicate_l {
    u8 mPrefix[0x50];
    be<u32> mKey;
    bool matches(const gptr<UtilityKeyRecord_l>*) const;
};
WWHD_OFFSET(UtilityKeyPredicate_l, mKey, 0x50);
WWHD_SIZE(UtilityKeyPredicate_l, 0x54);
}

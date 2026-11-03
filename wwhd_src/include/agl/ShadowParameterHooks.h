// Parameter callback prefix views derived from WWHD; shared AGL interfaces remain pending.
#pragma once
#include "wwhd.h"
namespace agl { namespace shadow_parameter {
// Scalar roles follow installed type/data/size slots; other hook roles remain neutral.
struct BooleanParameter {
    u8 mPrefix[0xC]; be<u8> mValue; u8 mPadding[3];
    be<u8>* valueAddress(); u32 byteSize();
};
struct IntegerParameter {
    u8 mPrefix[0xC]; be<u32> mValue;
    be<u32>* valueAddress(); be<u32>* alternateValueAddress();
    u32 typeCode(); u32 byteSize();
};
struct FloatParameter {
    u8 mPrefix[0xC]; be<f32> mValue;
    be<f32>* valueAddress(); be<f32>* alternateValueAddress();
    u32 typeCode(); u32 byteSize();
};
struct MatchRecord { u8 mPrefix[8]; be<u32> mComparisonValue; };
struct ObjectHooks_l {
    u8 mPrefix[0x50]; be<u32> mComparisonValue;
    u32 constantOneHook(); u32 alternateConstantOneHook();
    void emptyHook(); void alternateEmptyHook();
    u32 matchesRecord(const gptr<MatchRecord>* record);
};
// Prefix only, not the complete owning class; canonical AGL#8 adoption is pending.
struct OwnerHooks_l {
    u8 mPrefix[0xC]; be<u32> mParameterTable; be<u32> mOwnerTable;
    void destroy(u32 flags);
};
}}
WWHD_OFFSET(agl::shadow_parameter::BooleanParameter, mValue, 0xC);
WWHD_OFFSET(agl::shadow_parameter::IntegerParameter, mValue, 0xC);
WWHD_OFFSET(agl::shadow_parameter::FloatParameter, mValue, 0xC);
WWHD_OFFSET(agl::shadow_parameter::MatchRecord, mComparisonValue, 8);
WWHD_OFFSET(agl::shadow_parameter::ObjectHooks_l, mComparisonValue, 0x50);
WWHD_OFFSET(agl::shadow_parameter::OwnerHooks_l, mParameterTable, 0xC);
WWHD_OFFSET(agl::shadow_parameter::OwnerHooks_l, mOwnerTable, 0x10);

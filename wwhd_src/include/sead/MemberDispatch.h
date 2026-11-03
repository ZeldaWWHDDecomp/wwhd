// WWHD GHS callback descriptors. Field layouts read from local instructions.
#pragma once
#include "wwhd.h"
namespace sead {
struct MemberSlot {
    be<s16> mObjectAdjustment;
    be<s16> mVirtualIndex;
    be<u32> mTarget;
};
struct CallbackSet {
    u8 mOwnerData[12];
    MemberSlot mEnter;
    MemberSlot mExecute;
    MemberSlot mLeave;
};
struct VirtualEntry {
    be<u32> mAdjustment;
    be<u32> mTarget;
};
}
WWHD_OFFSET(sead::MemberSlot,mObjectAdjustment,0);
WWHD_OFFSET(sead::MemberSlot,mVirtualIndex,2);
WWHD_OFFSET(sead::MemberSlot,mTarget,4);
WWHD_SIZE(sead::MemberSlot,8);
WWHD_OFFSET(sead::CallbackSet,mEnter,12);
WWHD_OFFSET(sead::CallbackSet,mExecute,20);
WWHD_OFFSET(sead::CallbackSet,mLeave,28);
WWHD_SIZE(sead::CallbackSet,36);
WWHD_SIZE(sead::VirtualEntry,8);

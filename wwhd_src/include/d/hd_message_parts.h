#pragma once
#include "d/hd_parts.h"
#include "hd/hd_ui_020021FC.h"

struct PartRegistration_l { be<u32> mWords[3]; };
struct PartGlobals_l {
    be<u32> mMinimumAngle; be<u32> mMaximumAngle; be<u8> mPrimaryLock; be<u8> mSecondaryLock;
    u8 mReservedA[2]; be<u32> mOwner; be<u32> mTail; be<u32> mHead; be<u32> mCount;
};
WWHD_OFFSET(PartGlobals_l, mOwner, 0xC);
WWHD_SIZE(PartGlobals_l, 0x1C);



inline gfn* messagePartVTable(hdParts_c* part) { return reinterpret_cast<gfn*>(part->mVtable.get()); }

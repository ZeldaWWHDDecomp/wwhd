// Prefix views consumed by the HD batching callback; no complete foreign class is inferred.
#pragma once
#include "wwhd.h"
namespace j3d {
struct BatchObject_l {
    u8 mPrefix[0xF0];
    gptr<void> mResource;
    u8 mMiddle[0x18];
    be<u8> mDisabled;
    u8 mTrailing[0x13];
    be<u8> mIneligible;
};
struct BatchNode_l { gptr<BatchObject_l> mObject; };
struct BatchChains_l { gptr<BatchNode_l> mPrimary; gptr<BatchNode_l> mAlternate; be<u32> mCount; };
struct BatchSelectorMethods_l { u8 mPrefix[0x6C]; be<u32> mAlternate; };
struct BatchSelector_l { gptr<BatchSelectorMethods_l> mMethods; };
struct BatchPolicy_l { u8 mPrefix[0x3D8]; be<u8> mEnabled; };
struct BatchGroup_l {
    u8 mPrefix[0xC];
    gptr<BatchPolicy_l> mPolicy;
    gptr<BatchSelector_l> mSelector;
    u8 mMiddle[0x10];
    gptr<BatchChains_l> mChains;
};
struct BatchResource_l {
    be<u32> mReserved;
    be<u32> mIdentity[6];
    be<u32> mUsage;
    u8 mMiddle[8];
    be<u32> mLocation;
    be<u32> mPadding;
    be<u32> mAuxiliary;
    be<u32> mLayout;
    be<u32> mAlignment;
    u8 mRegisterPadding[0x98];
    be<u32> mRegisterSlot;
    u8 mTrailing[4];
    be<u32> mRegisterAuxiliary;
};
struct BatchManagerRef_l { gptr<void> mManager; };
struct BatchAdapter { static void combine(void*,BatchGroup_l*); };
}
WWHD_OFFSET(j3d::BatchObject_l,mResource,0xF0);
WWHD_OFFSET(j3d::BatchObject_l,mDisabled,0x10C);
WWHD_OFFSET(j3d::BatchObject_l,mIneligible,0x120);
WWHD_OFFSET(j3d::BatchGroup_l,mChains,0x24);
WWHD_OFFSET(j3d::BatchResource_l,mLocation,0x28);
WWHD_OFFSET(j3d::BatchResource_l,mAuxiliary,0x30);
WWHD_OFFSET(j3d::BatchResource_l,mRegisterSlot,0xD4);
WWHD_OFFSET(j3d::BatchResource_l,mRegisterAuxiliary,0xDC);

// Accessed prefix offsets and value strides; no unknown foreign extent is asserted.
WWHD_OFFSET(j3d::BatchNode_l,mObject,0);
WWHD_OFFSET(j3d::BatchChains_l,mCount,8);
WWHD_OFFSET(j3d::BatchSelectorMethods_l,mAlternate,0x6C);
WWHD_OFFSET(j3d::BatchSelector_l,mMethods,0);
WWHD_OFFSET(j3d::BatchPolicy_l,mEnabled,0x3D8);
WWHD_OFFSET(j3d::BatchManagerRef_l,mManager,0);

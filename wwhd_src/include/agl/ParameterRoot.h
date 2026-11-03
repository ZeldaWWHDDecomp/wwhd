// Parameter-root layout derived from its own WWHD constructor.
#pragma once
#include "agl/ParameterTree.h"
namespace agl {
struct ParameterRootInput_l { u8 mPrefix[12]; be<u32> mState; be<u32> mContext; be<u32> mPayloadOffset; };
struct ParameterRootDispatch { u8 mPrefix[0x64]; be<u32> mContextMismatch; };
struct ParameterRoot {
    parameter_tree::Node mTree;
    sead::FixedSafeText<64> mDisplayName;
    be<u32> mContext;
    be<u32> mFlags;
    sead::FixedSafeText<256> mPath;
    be<u32> mState;
    u32 treeConstantOneHook(); void treeEmptyHook();
    u32 treeMatchesRecord(const gptr<parameter_tree::MatchRecord>* record);
    void treeTrailingEmptyHook(); void treeAlternateTrailingEmptyHook(); void treeFinalEmptyHook();
    u32 constantOneHook();
    void onContextMismatch(const gptr<ParameterRootInput_l>* input);
    void applyInput(const gptr<ParameterRootInput_l>* input);
    void applyInputs(const gptr<ParameterRootInput_l>* first, const gptr<ParameterRootInput_l>* second, f64 weight);
    static ParameterRoot* create(ParameterRoot* root, sead::SafeText* name, u32 context);
};
}
WWHD_OFFSET(agl::ParameterRoot, mDisplayName, 0x64);
WWHD_OFFSET(agl::ParameterRoot, mContext, 0xB0);
WWHD_OFFSET(agl::ParameterRoot, mFlags, 0xB4);
WWHD_OFFSET(agl::ParameterRoot, mPath, 0xB8);
WWHD_OFFSET(agl::ParameterRoot, mState, 0x1C4);
WWHD_SIZE(agl::ParameterRoot, 0x1C8);

WWHD_SIZE(agl::ParameterRootInput_l, 0x18);
WWHD_OFFSET(agl::ParameterRootDispatch, mContextMismatch, 0x64);

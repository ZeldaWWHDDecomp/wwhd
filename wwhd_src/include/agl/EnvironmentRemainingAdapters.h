// Per-table text and callback views reconstructed from local method-table slots.
// Deletion-only views establish the prefix, without asserting a fixed-buffer capacity.
#pragma once
#include "agl/ParameterValue.h"
#include "agl/HierarchyNode.h"
namespace agl { namespace env {
struct ShaderHelperText_l : sead::SafeText { void destroy(u32 flags); void terminateEmpty(); };
struct ManagerTextBufferBase_l : sead::SafeTextBuffer { void destroy(u32 flags); void terminateBuffer(); };
struct ManagerTextBuffer_l : sead::SafeTextBuffer { void destroy(u32 flags); };
struct ManagerFixedText_l : sead::SafeTextBuffer { void destroy(u32 flags); };
struct SetHelperText_l : sead::SafeText { void destroy(u32 flags); void terminateEmpty(); };
struct SetHelperTextBuffer_l : sead::SafeTextBuffer { void destroy(u32 flags); };
struct SetHelperTextBufferBase_l : sead::SafeTextBuffer { void destroy(u32 flags); void terminateBuffer(); };
struct SetFixedText_l : sead::FixedSafeText<32> { void destroy(u32 flags); };
struct SetFormattingBufferBase_l : sead::SafeTextBuffer { void destroy(u32 flags); };
struct SetFormattingBuffer_l : sead::SafeTextBuffer { void destroy(u32 flags); };
struct SetFormattingFixedText_l : sead::FixedSafeText<1024> { void destroy(u32 flags); };
struct ObjectHelperText_l : sead::SafeText { void destroy(u32 flags); };
struct ObjectHelperTextBuffer_l : sead::SafeTextBuffer { void destroy(u32 flags); };
struct ManagerHelperText_l : sead::SafeText { void terminateEmpty(); };
struct ProjectorClassText_l : sead::SafeText { void terminateEmpty(); };
struct ProjectionParameterCallback_l : ParameterBinding { void onValueChanged(); };
struct FogClassText_l : sead::SafeText { void terminateEmpty(); };
struct FogParameterCallback_l : ParameterBinding { void onValueChanged(); };
struct EnvironmentSetInitialView_l { gptr<sead::VtableSlot> mMethods; void destroy(u32 flags); };
// A per-class query: the owning hierarchy node (the manager's) answers it for the class id.
struct EnvironmentIndexedQuery_l {
    u8 mOtherState[0x48];
    gptr<HierarchyNode> mOwner;
    be<u32> mClassId;
    const sead::SafeText* objectName(s32 objectIndex) const;
    u32 classObjectCount() const;
};
}}
WWHD_OFFSET(agl::env::EnvironmentIndexedQuery_l,mOwner,0x48);
WWHD_OFFSET(agl::env::EnvironmentIndexedQuery_l,mClassId,0x4C);

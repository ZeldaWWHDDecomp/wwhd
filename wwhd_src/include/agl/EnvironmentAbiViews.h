// Subobject prefixes used only for observed pointer adjustments and tail dispatch.
#pragma once
#include "agl/EnvironmentManager.h"
namespace agl { namespace env {
// The manager's hierarchy node: its name/count/update entries forward to the manager.
struct EnvironmentManagerParameterView_l : HierarchyNode {
    const sead::SafeText* objectName(s32 objectIndex,u32 typeId) const;
    u32 classObjectCount(u32 typeId) const;
    void update();
};
struct EnvironmentManagerIoApplyView_l {
    be<u32> mReserved;
    void applyResource(const EnvironmentArchiveReference* resource);
};
struct ProjectorIoView_l {
    gptr<sead::VtableSlot> mMethods;
    const sead::SafeText* groupText() const;
    const sead::SafeText* nameData() const;
    s32 typeId() const;
    u32 enabled() const;
};
struct FogIoView_l {
    gptr<sead::VtableSlot> mMethods;
    const sead::SafeText* groupText() const;
    const sead::SafeText* nameData() const;
    s32 typeId() const;
    u32 enabled() const;
};
struct EnvironmentObjectIoView_l {
    gptr<sead::VtableSlot> mMethods;
    const sead::SafeText* groupText() const;
    const sead::SafeText* nameData() const;
    s32 typeId() const;
    u32 enabled() const;
};
struct EnvironmentIconIoDeletionView_l {
    gptr<sead::VtableSlot> mMethods;
    void destroy(u32 flags);
};
}}
WWHD_SIZE(agl::env::EnvironmentManagerParameterView_l,0x3C);
WWHD_SIZE(agl::env::EnvironmentManagerIoApplyView_l,4);
WWHD_SIZE(agl::env::ProjectorIoView_l,4);
WWHD_SIZE(agl::env::FogIoView_l,4);
WWHD_SIZE(agl::env::EnvironmentObjectIoView_l,4);
WWHD_SIZE(agl::env::EnvironmentIconIoDeletionView_l,4);

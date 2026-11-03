// Observed WWHD transition delegates and request descriptors.
#pragma once
#include "sead/TaskMgr.h"
#include "sead/TaskTransitions.h"
namespace sead {
struct TransitionRenderObject : MethodTreeNode { void destroy(u32 flags); };
struct TransitionRegisteredObject : CreateListener { void destroy(u32 flags); };
namespace task_transition_text { struct Text : SafeText { void destroy(u32 flags); }; }
struct TransitionDelegate : MethodDelegate { u32 invoke(); };
struct TransitionEnterDelegate : MethodDelegate { u32 invoke(); };
struct TransitionLeaveDelegate : MethodDelegate { u32 invoke(); };
// Method-only view; the unmerged task owner will supply the complete canonical layout.
struct TransitionHooks_l : TaskBase {
    void idle();
    void enter();
    void leave();
    void prepare();
    void finish();
    void update();
    u32 query();
    u32 matchesType(const RuntimeType* type) const;
};
struct TaskCreationRequest : TaskClassID {
    u32 create(void* context) const;
};
}
WWHD_SIZE(sead::TransitionDelegate,16);
WWHD_SIZE(sead::TransitionEnterDelegate,16);
WWHD_SIZE(sead::TransitionLeaveDelegate,16);
WWHD_SIZE(sead::TaskCreationRequest,8);
WWHD_SIZE(sead::NullFaderTask,0x200);

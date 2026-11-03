/* WWHD sead task with a calc and a draw method node: attaching them to the method trees (below
 * the parent task's node for app tasks), pausing and detaching them, plus the per-TU copies of its
 * translation unit (text, method node, delegates) and the method tree manager base it constructs.
 * Layout from this game's constructor (0x11C bytes) and instructions. */
#pragma once
#include "sead/TaskBase.h"

namespace sead {

/* a task with one calc and one draw method node */
struct Task {
    TaskBase mTaskBase;
    MethodTreeNode mCalcEntry;
    MethodTreeNode mDrawEntry;

    static Task* construct(Task* task, const TaskConstructArg* arg, const be<u8>* name);
    void destroy(u32 flags);
    void attachCalc();
    void attachDraw();
    void detachCalc();
    void detachDraw();
    void pauseCalc(u32 pause);
    void pauseDraw(u32 pause);
    MethodTreeNode* getMethodTreeNode(u32 phase);
    bool isKindOf(const RuntimeType* type) const;
    void calc();
    void draw();
};

/* per-TU copies of the task translation unit */
namespace task_instances {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct MethodTreeNode : sead::MethodTreeNode { void destroy(u32 flags); };
struct NodeDelegate : MethodDelegate { void invoke(); };
struct TaskDelegate : MethodDelegate { void invoke(); };
}

}

WWHD_OFFSET(sead::Task, mCalcEntry, 0x74);
WWHD_OFFSET(sead::Task, mDrawEntry, 0xC8);
WWHD_SIZE(sead::Task, 0x11C);

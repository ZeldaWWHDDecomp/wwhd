/* WWHD sead framework, the rest of its translation unit: the reset, the base framework's empty
 * virtual hooks, the argument constructors and the TU's text and method-node copies. The framework
 * layout, its other functions and the TU-wide statics are SEAD#9's (sead/MethodTree.h,
 * sead/MethodTree.cpp); these are free functions on that class until its header takes them as
 * members. */
#pragma once
#include "sead/MethodTree.h"
#include "sead/MethodTreeNode.h"

namespace sead {

/* a slot of the framework's reset event (the list in Framework::mObjectList): an object that
 * holds a one-argument delegate, which gets the reset argument */
struct ResetDelegateVtable {
    VtableSlot mHeader;
    VtableSlot mInvoke;             // (argument)
};
struct ResetDelegate {
    gptr<ResetDelegateVtable> mVtable;
};
struct ResetEventSlot {
    u8 mOwnerData[0x20];
    gptr<ResetDelegate> mDelegate;
};
struct ResetEventLink : Link {
    gptr<ResetEventSlot> mSlot;
};

/* default size of the heap the framework's heap manager starts with (48 MB) */
static constexpr u32 kDefaultFrameworkHeapSize = 0x03000000;
/* priority word of the run settings: -1 keeps the default */
static constexpr s32 kDefaultPriority = -1;

/* Framework vtable slots this translation unit calls (FrameworkVtable_l names only 0x50..0x60) */
static constexpr u32 kFrameworkProcessFrameSlot = 6;    // mHeader[6], 0x30: pure here

namespace framework {
/* Framework members */
void processReset(Framework* framework);
void createSystemTasks(Framework* framework, void* rootTask, void* arg);
void processFrameEntry(Framework* framework);
void prepareRun(Framework* framework, void* heap);
void runLoop(Framework* framework);
/* constructors of the run arguments */
FrameworkInitArgs* constructInitArgs(FrameworkInitArgs* arg);
FrameworkRunSettings* constructRunSettings(FrameworkRunSettings* settings);
}

/* per-TU copies of the framework translation unit */
namespace framework_instances {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct MethodTreeNode : sead::MethodTreeNode { void destroy(u32 flags); };
}

}

WWHD_OFFSET(sead::ResetDelegateVtable, mInvoke, 8);
WWHD_OFFSET(sead::ResetEventSlot, mDelegate, 0x20);
WWHD_OFFSET(sead::ResetEventLink, mSlot, 8);

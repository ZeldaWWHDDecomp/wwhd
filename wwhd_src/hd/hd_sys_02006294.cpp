/** HD state machine and root-task entry. No GameCube counterpart.
 * Virtual operation names describe observed roles. */
#include "bindings.h"
#include "hd/hd_sys_02006294.h"

static hdStateId sNoStateId;
enum : s32 { HD_NO_STATE_e = -1 };
enum : u32 { ROOT_TASK_SIZE = 0x130, NO_STATE_ID_VTABLE_ADDRESS = 0x10000A88 };

/* 02006294 */
// HD: allocate the root task from the framework argument's current heap.
static void* createStateRootTask(hdTaskConstructArg* framework) {
    hdTaskHeapArray* heaps = framework->mHeapArray;
    void* heap = heaps->mHeaps[heaps->mCurrentHeapIndex];
    void* task = hdStateRootTaskAllocate(ROOT_TASK_SIZE, heap, 4);
    if (task == NULL) {
        return NULL;
    }
    return hdStateRootTaskConstruct(task, framework);
}

/* 020062E8 */
// HD: preserve the initialization order and ignore the sentinel initial state id.
static hdStateMachine* createStateMachine(hdStateMachine* i_this, hdStateOwner* owner, hdStateId* initialState) {
    if (i_this == NULL) {
        i_this = (hdStateMachine*)operator_new(sizeof(hdStateMachine));
        if (i_this == NULL) {
            return NULL;
        }
    }
    i_this->mpOwner = owner;
    i_this->mpRequestedStateId = NULL;
    i_this->mpLastStateId = &sNoStateId;
    i_this->mpCurrentState = NULL;
    i_this->mpPreviousState = NULL;
    if (initialState->mId != HD_NO_STATE_e) {
        i_this->mpRequestedStateId = initialState;
    }
    return i_this;
}

/* 02006364 */
// HD: the owner callback may change the current state, so read it after the callback.
static void updateStateMachine(hdStateMachine* i_this) {
    hdStateOwner* owner = i_this->mpOwner;
    owner->onStateChange(&i_this->mpPreviousState);
    hdStateObject* state = i_this->mpCurrentState;
    if (state != NULL) {
        state->update();
    }
}

/* 020063C0 */
// HD: preserve field rereads after virtual calls, which may change state or the pending request.
static void changeStateMachineState(hdStateMachine* i_this, hdStateId* nextState) {
    hdStateObject* current = i_this->mpCurrentState;
    i_this->mpRequestedStateId = nextState;
    hdStateId* requested = nextState;
    if (current != NULL) {
        if (nextState == NULL) {
            return;
        }
        i_this->mpLastStateId = current->getStateId();
        hdStateObject* stateToLeave = i_this->mpCurrentState;
        stateToLeave->leave();
        hdStateObject* previous = i_this->mpCurrentState;
        i_this->mpCurrentState = NULL;
        requested = i_this->mpRequestedStateId;
        i_this->mpPreviousState = previous;
    }
    if (requested == NULL) {
        return;
    }
    hdStateOwner* owner = i_this->mpOwner;
    hdStateObject* state = owner->createState(requested);
    i_this->mpCurrentState = state;
    state->enter();
    i_this->mpRequestedStateId = NULL;
}

/* 02006478 */
// HD: return the current state's id, the pending request, or the shared no-state id.
static hdStateId* getStateMachineCurrentId(hdStateMachine* i_this) {
    hdStateObject* state = i_this->mpCurrentState;
    if (state == NULL) {
        hdStateId* requested = i_this->mpRequestedStateId;
        return requested == NULL ? &sNoStateId : requested;
    }
    return state->getStateId();
}

/* 020064B4 */
// HD: initialize the header statics and the sentinel state-id object, retaining store order.
static void __sinit_hd_state_machine_cpp() {
    sinit_header_statics_rodata(0x101FF310, 0x1018C430, 0x101FF31C, 0x10000AD0);
    sNoStateId.__vtbl = NO_STATE_ID_VTABLE_ADDRESS;
    sNoStateId.mId = HD_NO_STATE_e;
    sNoStateId.field_0x4 = 0;
}

/* 02006570 */
static u32 getStateIdValue(hdStateId* i_this) {
    return (u32)i_this->mId;
}

/* 02006578 */
// HD: first empty state-id companion; original symbol unknown.
static void stateIdEmptyCompanionFirst() {
}

/* 0200657C */
// HD: second empty state-id companion; original symbol unknown.
static void stateIdEmptyCompanionSecond() {
}

/* 02006580 */
// HD: third empty state-id companion; original symbol unknown.
static void stateIdEmptyCompanionThird() {
}

/* WWHD glue (tools/verify/autoglue.py): data and local HD task bindings
 data sNoStateId 101FF32C
 ext 0273B050 void* hdStateRootTaskAllocate(u32, void*, s32)
 ext 0203E9EC void* hdStateRootTaskConstruct(void*, hdTaskConstructArg*)
*/

/** HD state-machine guest views. */
#pragma once
#include "wwhd.h"
#include "hd/hd_sys_02035444.h"

struct hdStateId {
    be<s32> mId;
    be<u32> field_0x4;
    be<u32> __vtbl;
};
WWHD_SIZE(hdStateId, 0xC);
struct hdStateObject;
struct hdStateObjectVTable {
    u8 _padding00[0x14];
    gfn mGetStateId;
    u8 _padding18[4];
    gfn mEnter;
    u8 _padding20[4];
    gfn mUpdate;
    u8 _padding28[4];
    gfn mLeave;
};
WWHD_OFFSET(hdStateObjectVTable, mGetStateId, 0x14);
WWHD_OFFSET(hdStateObjectVTable, mEnter, 0x1C);
WWHD_OFFSET(hdStateObjectVTable, mUpdate, 0x24);
WWHD_OFFSET(hdStateObjectVTable, mLeave, 0x2C);
struct hdStateObject {
    gptr<hdStateObjectVTable> mpVTable;
    hdStateId* getStateId() { return gabi::call_ptr<hdStateId*>(mpVTable->mGetStateId.get(), this); }
    void enter() { gabi::call_ptr(mpVTable->mEnter.get(), this); }
    void update() { gabi::call_ptr(mpVTable->mUpdate.get(), this); }
    void leave() { gabi::call_ptr(mpVTable->mLeave.get(), this); }
};
struct hdStateOwnerVTable {
    u8 _padding00[0x14];
    gfn mCreateState;
    u8 _padding18[4];
    gfn mOnStateChange;
};
WWHD_OFFSET(hdStateOwnerVTable, mCreateState, 0x14);
WWHD_OFFSET(hdStateOwnerVTable, mOnStateChange, 0x1C);
struct hdStateOwner {
    gptr<hdStateOwnerVTable> mpVTable;
    hdStateObject* createState(hdStateId* id) {
        return gabi::call_ptr<hdStateObject*>(mpVTable->mCreateState.get(), this, id);
    }
    void onStateChange(gptr<hdStateObject>* previousState) {
        gabi::call_ptr(mpVTable->mOnStateChange.get(), this, previousState);
    }
};
struct hdStateMachine {
    gptr<hdStateOwner> mpOwner;
    gptr<hdStateId> mpRequestedStateId;
    gptr<hdStateObject> mpCurrentState;
    gptr<hdStateId> mpLastStateId;
    gptr<hdStateObject> mpPreviousState;
};
WWHD_OFFSET(hdStateMachine, mpRequestedStateId, 4);
WWHD_OFFSET(hdStateMachine, mpCurrentState, 8);
WWHD_OFFSET(hdStateMachine, mpLastStateId, 0xC);
WWHD_OFFSET(hdStateMachine, mpPreviousState, 0x10);
WWHD_SIZE(hdStateMachine, 0x14);

void* hdStateRootTaskAllocate(u32 size, void* heap, s32 alignment);
void* hdStateRootTaskConstruct(void* task, hdTaskConstructArg* argument);

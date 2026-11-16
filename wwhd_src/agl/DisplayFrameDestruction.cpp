// Render view destructor: releases the attachments, the pending list, the job and command tables (each element through
// its own table's destructor, the tables reloaded after every call) and the embedded render buffer.
#include "agl/DisplayFrameState.h"
#include "sead/ElementAt.h"
using namespace agl::render;
static u8 sDestroyedDisplayStateMethods;
// The recorded command's destructor slot (table +0x10, function word +0x14).
struct RecordedCommandMethods_l { u8 mPrefix[8]; sead::VtableSlot mDestroy; };
WWHD_OFFSET(RecordedCommandMethods_l,mDestroy,8);
// The original finds the heap for the table's current storage, loads the heap's release slot and only then reloads the
// table pointer from the state for the release call (lwz r4,0x7A4/0x7E4(r30) right before the bctrl): the storage
// released is the one the state holds after findRegistryHeap (02755FEC), not the one it was looked up with.
template<class T> static void releaseTableStorage(const gptr<T>& storage) {
    sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager,static_cast<T*>(storage));
    u32 release = heap->dispatch()->mRelease.mFunction;
    T* current = static_cast<T*>(storage);
    gabi::call_ptr<void>(release,heap,current);
}
/* 027D5CF4 */
void agl::render::destroyDisplayFrameState(DisplayFrameState* state,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!state) return;
    state->mMethods = gabi::ea(&sDestroyedDisplayStateMethods);
    state->releaseAttachments();
    releaseInputArray(&state->mPending);
    if (state->mPrimaryJobs.mJobs) {
        for (u32 index = 0; (s32)index < (s32)(u32)state->mPrimaryJobs.mCount; ++index) {
            RenderJob* job = sead::elementPtr(static_cast<RenderJob*>(state->mPrimaryJobs.mJobs),index);   /* jobs + index * 0x1C in 32 bits */
            gabi::call_ptr<void>(job->mDispatch->mDestroy,job,2);
        }
        releaseTableStorage(state->mPrimaryJobs.mJobs);
        state->mPrimaryJobs.mCount = 0;
        state->mPrimaryJobs.mJobs = NULL;
    }
    state->mDeferred.releaseStorage();
    if (state->mSubmittedCommands.mCommands) {
        for (u32 index = 0; (s32)index < (s32)(u32)state->mSubmittedCommands.mCount; ++index) {
            agl::gx2util::TimedCounter* command =
                sead::elementPtr(static_cast<agl::gx2util::TimedCounter*>(state->mSubmittedCommands.mCommands),index);   /* + index * 0x40 in 32 bits */
            auto* methods = static_cast<RecordedCommandMethods_l*>(static_cast<void*>(command->mVtable));
            gabi::call_ptr<void>(methods->mDestroy.mFunction,command,2);
        }
        releaseTableStorage(state->mSubmittedCommands.mCommands);
        state->mSubmittedCommands.mCount = 0;
        state->mSubmittedCommands.mCommands = NULL;
    }
    state->mSubmitted.releaseStorage();
    state->mEmbeddedBuffer.destroy(2);
    if (flags & 1u) releaseRegistryStorage(state);
}
/* WWHD glue
data sDestroyedDisplayStateMethods 1016C348
ext 027B9694 void agl::RenderBuffer::destroy(u32)
*/

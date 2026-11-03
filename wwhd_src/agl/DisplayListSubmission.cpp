// Drawing a render view's recorded commands (RenderQueueManager::drawView 027C1AD8). Rebuild: the deferred commands are
// sorted, each one's display list is called (an owned command first finishes its layer's pass when no job is current),
// and a copy of it goes into the next command-table slot, which joins the submitted list. Replay: the submitted
// commands' display lists are called again.
#include "agl/DisplayFrameState.h"
using namespace agl::render;
void callDisplayList(void*,u32);
void synchronizeMemory();
/* 027D6EEC */
void agl::render::DisplayFrameState::submitDisplayLists(u32 rebuild) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (rebuild) {
        sortDeferred();
        u32 counter = gabi::ea(&mSubmitted.mCount);
        do { gabi::load_reserved(counter); } while (!gabi::store_conditional(counter,0));
        GuestRange<gptr<agl::gx2util::TimedCounter>> deferred = pointerRange<agl::gx2util::TimedCounter>(mDeferred);
        gptr<agl::gx2util::TimedCounter>* slot = deferred.first;
        gptr<agl::gx2util::TimedCounter>* end = deferred.end;
        for (; slot != end; ++slot) {
            agl::gx2util::TimedCounter* command = *slot;
            agl::RenderLayerPrefix* layer = commandLayer(command);
            if (layer && (s32)mCurrentJob == -1) {
                finishRenderPass(layer);
                synchronizeMemory();
                command = *slot;
            }
            u32 size = recording(command)->mUsed;
            callDisplayList(recording(command)->mData,size);
            u32 index = (u32)(s32)mSubmitted.mCount;
            u32 count = mSubmittedCommands.mCount;
            agl::gx2util::TimedCounter* copy = mSubmittedCommands.mCommands;
            if (index < count) copy = mSubmittedCommands.at(index);
            (*slot)->copyTo(copy);
            do { index = gabi::load_reserved(counter); } while (!gabi::store_conditional(counter,index + 1u));
            submittedCommands()[index] = copy;
        }
    } else {
        GuestRange<gptr<agl::gx2util::TimedCounter>> submitted = pointerRange<agl::gx2util::TimedCounter>(mSubmitted);
        gptr<agl::gx2util::TimedCounter>* slot = submitted.first;
        gptr<agl::gx2util::TimedCounter>* end = submitted.end;
        for (; slot != end; ++slot) {
            agl::gx2util::TimedCounter* command = *slot;
            callDisplayList(recording(command)->mData,recording(command)->mUsed);
        }
    }
}
/* WWHD glue
ext C0006160 void callDisplayList(void*,u32)
ext C0009E90 void synchronizeMemory()
ext 027DA764 void agl::gx2util::TimedCounter::copyTo(agl::gx2util::TimedCounter*)
*/

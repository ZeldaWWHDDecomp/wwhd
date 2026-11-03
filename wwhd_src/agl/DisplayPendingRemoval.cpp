// RenderQueueManager::removeLayer: the layer leaves the pending list, and its first deferred command (the one whose
// owning layer it is) leaves the deferred list.
#include "agl/DisplayFrameState.h"
using namespace agl::render;
/* 027D650C */
void agl::render::DisplayFrameState::removePending(agl::RenderLayerPrefix* layer) {
    gabi::AutoFrame frame{gabi::current_function()};
    GuestRange<gptr<agl::RenderLayerPrefix>> pending = pointerRange<agl::RenderLayerPrefix>(mPending);
    gptr<agl::RenderLayerPrefix>* slot = pending.first;
    gptr<agl::RenderLayerPrefix>* end = pending.end;
    for (s32 index = 0; slot != end; ++slot,++index) {
        if (*slot == layer) {
            mPending.erase(index,1);
            GuestRange<gptr<agl::gx2util::TimedCounter>> deferred = pointerRange<agl::gx2util::TimedCounter>(mDeferred);
            gptr<agl::gx2util::TimedCounter>* command = deferred.first;
            gptr<agl::gx2util::TimedCounter>* commandEnd = deferred.end;
            for (s32 commandIndex = 0; command != commandEnd; ++command,++commandIndex) {
                if (commandLayer(*command) == layer) {
                    mDeferred.eraseRange(commandIndex,1);
                    return;
                }
            }
            return;
        }
    }
}
/* WWHD glue
*/

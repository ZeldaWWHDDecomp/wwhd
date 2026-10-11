/* HD telescope zoom transition states. Private reconstruction. */
#include "d/hd_hud_b.h"
static HdHudState l_telescopeZoomedState;
static HdHudState l_telescopeWaitState;

/* 026E8050 */
void hdTelescopeZoomIn(HdHudTelescope* self) {
    HdHudPlay* play = hdHudGetPlay();
    u32 frame = self->mFrame+1;
    HdHudActor* actor = play->mActor;
    self->mFrame = frame;
    if ((s32)frame <= 5 || (s32)frame < 10) {
        actor->mStatus = actor->mStatus|HD_HUD_TELESCOPE_ACTOR_STATUS;
        return;
    }
    hdHudChangeState(&self->mState,&l_telescopeZoomedState);
    play = hdHudGetPlay();
    play->mTelescopeMode = 12;
    play = hdHudGetPlay();
    play->mTelescopeNextMode = 13;
}

/* 026E83C4 */
void hdTelescopeZoomOut(HdHudTelescope* self) {
    u32 frame = self->mFrame+1;
    self->mFrame = frame;
    if ((s32)frame < 5) return;
    HdHudPlay* play = hdHudGetPlay();
    play->mTelescopeMode = 18;
    hdHudChangeState(&self->mState,&l_telescopeWaitState);
}

/* WWHD glue (tools/verify/autoglue.py)
data l_telescopeZoomedState 1049CA98
data l_telescopeWaitState 1049CA38
*/

#include "d/hd_msg_window_views.h"
using namespace gabi;
extern void hdMsgStartSave(HdMsgSaveManager*);
extern void hdMsgStartLoad(HdMsgSaveManager*);
extern void hdMsgPlaySound(void*);
extern void hdMsgSelectButton(HdMsgSequence*,u32);
extern void hdMsgUpdateButtons(HdMsgSequence*);
static HdMsgManagerPointer l_saveManager;
static HdMsgState l_sequenceWaitState;
static HdMsgState l_sequenceDoneState;
static u32 l_saveSound;
static u32 l_loadSound;
/* 026DA8AC */
void hdMsgWaitMove(HdMsgSequence* self) {
    const u32 timer = self->mTimer;
    if (timer) self->mTimer = timer - 1;
    else hdMsgChangeState(&self->mState, self->mNextState);
}
/* 026DAA34 */
void hdMsgWaitFlagMove(HdMsgSequence* self) {
    const u32 timer = self->mTimer;
    if (timer) {
        const u32 next = timer - 1;
        self->mTimer = next;
        if (!next) self->mWaitComplete = 1;
    } else if (!self->mWaitComplete) {
        hdMsgChangeState(&self->mState, &l_sequenceWaitState);
    }
}
/* 026DAB18 */
void hdMsgSaveMove(HdMsgSequence* self) {
    u32 timer = self->mTimer;
    const u32 phase = self->mPhase;
    if (timer) { --timer; self->mTimer = timer; }
    if (phase > HD_MSG_IO_WAIT || (phase == HD_MSG_IO_WAIT && timer)) return;
    HdMsgSaveManager* manager = l_saveManager.mManager;
    const u32 status = manager->mStatus;
    if (status == HD_MSG_IO_BUSY || status == HD_MSG_IO_PENDING) return;
    if (phase == HD_MSG_IO_START) {
        hdMsgStartSave(manager);
        self->mPhase = HD_MSG_IO_WAIT;
    } else {
        hdMsgPlaySound(&l_saveSound);
        hdMsgChangeState(&self->mState, &l_sequenceDoneState);
    }
}
/* 026DAC68 */
void hdMsgButtonAdvance(HdMsgSequence* self) {
    self->mTextOffset = self->mTextOffset + 0x28;
    hdMsgSelectButton(self, 1u);
    hdMsgUpdateButtons(self);
}
/* 026DAD14 */
void hdMsgLoadMove(HdMsgSequence* self) {
    u32 timer = self->mTimer;
    const u32 phase = self->mPhase;
    if (timer) { --timer; self->mTimer = timer; }
    if (phase > HD_MSG_IO_WAIT || (phase == HD_MSG_IO_WAIT && timer)) return;
    HdMsgSaveManager* manager = l_saveManager.mManager;
    const u32 status = manager->mStatus;
    if (status == HD_MSG_IO_BUSY || status == HD_MSG_IO_PENDING) return;
    if (phase == HD_MSG_IO_START) {
        hdMsgStartLoad(manager);
        self->mPhase = HD_MSG_IO_WAIT;
    } else {
        hdMsgPlaySound(&l_loadSound);
        hdMsgChangeState(&self->mState, &l_sequenceDoneState);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
data l_saveManager 101F852C
data l_sequenceWaitState 1049B510
data l_sequenceDoneState 1049B660
data l_saveSound 10103FC0
data l_loadSound 10103FD0
ext 02721D48 void hdMsgStartSave(HdMsgSaveManager*)
ext 0272276C void hdMsgStartLoad(HdMsgSaveManager*)
ext 02030B38 void hdMsgPlaySound(void*)
ext 026D9974 void hdMsgSelectButton(HdMsgSequence*,u32)
ext 026D9C48 void hdMsgUpdateButtons(HdMsgSequence*)
*/

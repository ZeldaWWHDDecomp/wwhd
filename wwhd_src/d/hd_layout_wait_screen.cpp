/* HD wait-screen setup companions; existing wait-update bodies remain in their merged TU. */
#include "d/hd_layout_mgr.h"
static u32 sWaitDelegateTable;
static u32 sWaitOwnerTable;
static void* sWaitMessageMgr;
static void* sWaitResourceMgr;
static s8 sWaitRoom;
static hdLayoutDummyString_c sDummy;
static u32 sDummyGuard;
static hdScreenStringTable sDummyInputTable;
static hdScreenStringTable sDummyBaseTable;
static hdScreenStringTable sDummyFinalTable;
void waitDestroyBase(hdParts_c*, u32);
void waitSound(u32);
hdLayoutWaitPlay_c* waitGetPlay();
u32 waitEventActor(void*, u32);
s32 waitReverb(s32);
void waitStartSound(u32, u32, s32);
void waitStopSound();
void waitMessageMode(void*, u32);
void waitLoadMessage(void*, hdLayoutWaitMessage_c*, hdLayoutDummyString_c*, void*);
void waitAssureName(hdScreenString*);
void waitCopyName(be<u8>*, const char*, u32, u32);
void waitRegister(void*);
static hdLayoutRegistration_c sDummyRegistration;
/* 026FC4D8 */
void destroyLayoutWaitScreen(hdParts_c* object, u32 flags) {
    if (!object)
        return;
    object->mStateHolder.mHolderVtable = &sWaitDelegateTable;
    object->mSecondaryVtable = &sWaitOwnerTable;
    waitDestroyBase(object, 0);
    if (flags & 1)
        operator_delete(object);
}
/* 026FC544 */
void hdLayoutWaitScreen_c::enterFirst() {
    gabi::call_ptr(((hdLayoutWaitTable_c*)mVtable.get())->mWaitEnter.get(), this);
    mStatus = 1;
}
/* 026FC610 */
void hdLayoutWaitScreen_c::enterSecond() {
    gabi::call_ptr(((hdLayoutWaitTable_c*)mVtable.get())->mReset.get(), this);
    gabi::call_ptr(((hdLayoutWaitTable_c*)mVtable.get())->mPrepare.get(), this);
    gabi::call_ptr(((hdLayoutWaitTable_c*)mVtable.get())->mWaitEnter.get(), this);
    waitSound(0x80C);
    waitGetPlay()->mAdvanceCount = 0;
    waitGetPlay()->mHandInput = 0;
    hdLayoutWaitInput_c* input = mpMessage->mpInput;
    if (input) {
        u32 mode = input->mMessageMode;
        waitGetPlay()->mMessageMode = mode;
        u32 slot = input->mActorSlot;
        waitGetPlay()->mActorSlots[0] = slot;
        s32 index = input->mEventIndex;
        waitGetPlay()->mEventIndex = index;
        slot = input->mActorSlot;
        if (slot >= 1 && slot <= 5) {
            hdLayoutWaitPlay_c* play = waitGetPlay();
            slot = input->mActorSlot;
            u32 actor = play->mActorSlots[slot];
            if (actor) {
                play = waitGetPlay();
                void* event = play->mEvent;
                u32 pid = waitEventActor(event, actor);
                play->mEventActor = pid;
            }
        }
        if (input->mSound) {
            s32 reverb = waitReverb(sWaitRoom);
            u32 sound = input->mSound;
            waitStartSound(sound, 0, reverb);
        }
    }
    waitMessageMode(sWaitMessageMgr, 5);
    mStatus = 1;
}
/* 026FC7F0 */
void hdLayoutWaitScreen_c::reset() {
    waitMessageMode(sWaitMessageMgr, 2);
    hdLayoutWaitPane_c* pane = mpPane;
    pane->mFlags = (u8(pane->mFlags) & 0xFE) + 1;
    // GHS retains the pane in r5 when entering this reset hook.
    gabi::call_ptr(((hdLayoutWaitTable_c*)mVtable.get())->mReset.get(), this, gabi::cpu->r[4], pane);
    gabi::call_ptr(((hdLayoutWaitTable_c*)mVtable.get())->mPrepare.get(), this);
    gabi::call_ptr(((hdLayoutWaitTable_c*)mVtable.get())->mStop.get(), this);
    waitStopSound();
    waitSound(0x80C);
    if (!sDummyGuard) {
        sDummy.mBuffer[5] = 0;
        hdScreenString name;
        name.mText.v = gabi::guest_addr("dummy");
        name.mVtable = &sDummyInputTable;
        be<u8>* buffer = sDummy.mBuffer;
        sDummy.mText = buffer;
        sDummy.mCapacity = 6;
        sDummy.mpVtable = &sDummyBaseTable;
        sDummyGuard = 1;
        waitAssureName(&name);
        const char* source = name.mText;
        u32 length = 0;
        while (*gabi::at<be<u8>>(gabi::ea(source) + length) != 0) {
            if (++length > 0x40000) {
                length = 0;
                break;
            }
        }
        s32 capacity = sDummy.mCapacity;
        if ((s32)length >= capacity)
            length = u32(capacity) - 1;
        name.assureTermination();
        waitCopyName(buffer, name.mText, length, 0);
        *gabi::at<be<u8>>(gabi::ea(buffer) + length) = 0;
        sDummy.mpVtable = &sDummyFinalTable;
        waitRegister(&sDummyRegistration);
    }
    hdLayoutWaitMessage_c* message = mpMessage;
    waitLoadMessage(sWaitResourceMgr, message, &sDummy, message->mResourceName);
    hdLayoutWaitInput_c* input = mpMessage->mpInput;
    if (input && input->mSound) {
        s32 reverb = waitReverb(sWaitRoom);
        u32 sound = input->mSound;
        waitStartSound(sound, 0, reverb);
    }
    waitGetPlay()->mAdvanceCount = 0;
    waitGetPlay()->mHandInput = 0;
    gabi::call_ptr(((hdLayoutWaitTable_c*)mVtable.get())->mWaitFinish.get(), this);
}
/* WWHD glue
data sWaitDelegateTable 1010B5B8
data sWaitOwnerTable 1010B5C8
data sWaitMessageMgr 101F4B5C
data sWaitResourceMgr 101F4AE8
data sWaitRoom 1047E6C8
data sDummy 1049DE54
data sDummyGuard 1049DE68
data sDummyInputTable 1010AB34
data sDummyBaseTable 1010AB64
data sDummyFinalTable 1010AB7C
data sDummyRegistration 101F72A0
str 1010AC2C "dummy"
ext 026F88F0 void waitDestroyBase(hdParts_c*,u32)
ext 025E1988 void waitSound(u32)
ext 025200D4 hdLayoutWaitPlay_c* waitGetPlay()
ext 0253F124 u32 waitEventActor(void*,u32)
ext 02520540 s32 waitReverb(s32)
ext 025E1C3C void waitStartSound(u32,u32,s32)
ext 025E1C58 void waitStopSound()
ext 025F74D0 void waitMessageMode(void*,u32)
ext 025F50E8 void waitLoadMessage(void*,hdLayoutWaitMessage_c*,hdLayoutDummyString_c*,void*)
ext 02700818 void waitAssureName(hdScreenString*)
ext C0009988 void waitCopyName(be<u8>*,const char*,u32,u32)
ext 028F026C void waitRegister(void*)
*/

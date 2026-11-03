// sead remote-controller speaker audio: effect parameters, the per-channel command queue and its free list.
// Reconstructed from this game's instructions.
#include "bindings.h"
#include "sead/RemoteAudioCommands.h"

static s32 sRemoteAudioChannelCount;
static s32 sRemoteAudioCommandCapacity;
static sead::remote_audio::AudioText sRemoteAudioText;
static sead::remote_audio::CommandText sRemoteCommandText;
static u32 sRemoteQueueTable;
static sead::SafeTextVtable sRemoteAudioTextVtable;
static u32 sRemoteAudioName;
static sead::SafeTextVtable sRemoteCommandTextVtable;
static u32 sRemoteCommandName;

/* 02765424 */
void sead::remote_audio::EffectParameters::setGain(f32 gain) {
    gabi::AutoFrame frame{gabi::current_function()};
    mGain = gain;
}
/* 0276542C */
void sead::remote_audio::EffectParameters::setInterval(u32 interval) {
    gabi::AutoFrame frame{gabi::current_function()};
    mInterval = interval;
}
/* 02765434 */
void sead::remote_audio::EffectParameters::setPriority(u32 priority) {
    gabi::AutoFrame frame{gabi::current_function()};
    mPriority = priority;
}
/* 0276543C */
void sead::remote_audio::EffectParameters::setRequest(const be<u32>* request) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 value = *request;
    mMode = 1;
    mRequest = value;
}
/* 02765450 */
void sead::remote_audio::EffectParameters::setResponse(const be<u32>* response) {
    gabi::AutoFrame frame{gabi::current_function()};
    mResponse = *response;
}
/* 0276545C */
static void destroyEffectParameters(void* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object != NULL && (flags & 1)) operator_delete(object);
}
/* 02765470 */
static void effectParametersReserved() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 02765474 */
static void effectParametersUpdated() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 02765478 */
static sead::remote_audio::CommandQueue* createRemoteAudioQueue(sead::remote_audio::CommandQueue* queue) {
    gabi::AutoFrame frame{gabi::current_function()};
    using namespace sead::remote_audio;
    if (queue == NULL) queue = (CommandQueue*)allocateRemoteAudioObject(sizeof(CommandQueue));
    if (queue != NULL) {
        queue->mReservedWords[0] = 0;
        queue->mTable = gabi::ea(&sRemoteQueueTable);
        queue->mReservedWords[1] = 0;
        queue->mEnabled = 0;
        queue->mPaused = 0;
        // GHS retains the embedded constructor's wrap-to-null allocation path.
        sead::LinkList* commands = gabi::at<sead::LinkList>(gabi::ea(queue) + 0x10);
        if (commands == NULL) commands = (sead::LinkList*)allocateRemoteAudioObject(0x18);
        if (commands != NULL) {
            commands->mCount = 0;
            CommandQueue* base = (CommandQueue*)((u8*)commands - 0x10);
            base->mFree = NULL;
            base->mCapacity = 0;
            base->mStorage = NULL;
            commands->mPrevious = commands;
            commands->mNext = commands;
        }
        s32 capacity = sRemoteAudioCommandCapacity;
        if (capacity > 0) {
            u32 bytes = (u32)capacity * sizeof(Command);
            Command* storage = (Command*)allocateFoundationArray(bytes, NULL, 4);
            if (storage != NULL) {
                queue->mStorage = storage;
                for (s32 i = 0; i < capacity - 1; ++i)
                    storage[i].mNextFree = &storage[i + 1];
                storage[(u32)capacity - 1].mNextFree = NULL;
                queue->mFree = storage;
                queue->mCapacity = capacity;
            }
        }
    }
    return queue;
}
/* 02765584 */
u32 sead::remote_audio::CommandQueue::connected(u32 channel) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u32>> type(8);
    return probeRemoteAudioController(channel, type.get()) != -1;
}
/* 027655B8 */
void sead::remote_audio::CommandQueue::stopChannel(u32 channel) {
    gabi::AutoFrame frame{gabi::current_function()};
    bool stop = connected(channel) != 0;
    if (!stop) {
        void* manager = remoteAudioManager();
        void* speaker = remoteAudioChannel(manager, channel);
        stop = remoteAudioActive(speaker) != 0;
    }
    if (stop) {
        void* manager = remoteAudioManager();
        void* speaker = remoteAudioChannel(manager, channel);
        stopRemoteAudio(speaker, 0);
    }
}
/* 02765618 */
void sead::remote_audio::CommandQueue::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mTable = gabi::ea(&sRemoteQueueTable);
    for (s32 channel = 0; channel < (s32)sRemoteAudioChannelCount; ++channel)
        stopChannel(channel);
    if (flags & 1) operator_delete(this);
}
/* 027656AC */
u32 sead::remote_audio::Command::execute() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u32>> type(8);
    u32 operation = mOperation;
    if (operation == 1) {
        if (probeRemoteAudioController(mChannel, type.get()) == -1) return 1;
        u32 channel = mChannel;
        void* manager = remoteAudioManager();
        void* speaker = remoteAudioChannel(manager, channel);
        u32 result = playRemoteAudio(speaker, mValue);
        if (result != 0) return result;
        s32 remaining = (u32)mRemaining - 1;
        mRemaining = remaining;
        return remaining > 0 ? result : 1;
    }
    if (operation == 2) {
        u32 channel = mChannel;
        void* manager = remoteAudioManager();
        void* speaker = remoteAudioChannel(manager, channel);
        stopRemoteAudio(speaker, mValue);
    }
    return 1;
}
/* 02765764 */
void sead::remote_audio::CommandQueue::recycle(Command* command) {
    gabi::AutoFrame frame{gabi::current_function()};
    command->mLink.detach();
    u32 count = mCommands.mCount;
    Command* storage = mStorage;
    mCommands.mCount = count - 1;
    command->mNextFree = storage;
    mStorage = command;
}
// The queue links commands through Command::mLink; these convert in guest addresses, so a link at 0x10
// yields a null command and a null command yields the link address 0x10, as in the original.
static sead::remote_audio::Command* commandOf(sead::Link* link) {
    return gabi::at<sead::remote_audio::Command>(gabi::ea(link) - offsetof(sead::remote_audio::Command, mLink));
}
static sead::Link* linkOf(sead::remote_audio::Command* command) {
    return gabi::at<sead::Link>(gabi::ea(command) + offsetof(sead::remote_audio::Command, mLink));
}
/* 027657B8 */
void sead::remote_audio::CommandQueue::makeFirst(Command* command) {
    gabi::AutoFrame frame{gabi::current_function()};
    Command* first = NULL;
    if ((s32)mCommands.mCount > 0) {
        Link* link = mCommands.mPrevious;
        if (link != NULL) first = commandOf(link);
    }
    if (first != command) mCommands.moveAfter(linkOf(first), linkOf(command));
}
/* 02765800 */
void sead::remote_audio::CommandQueue::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mPaused || (s32)mCommands.mCount <= 0) return;
    Link* link = mCommands.mNext;
    if (link == NULL) return;
    Command* command = commandOf(link);
    if (command == NULL) return;
    if (command->execute()) recycle(command);
    else makeFirst(command);
}
/* 02765894 */
static void initializeRemoteAudioText() {
    gabi::AutoFrame frame{gabi::current_function()};
    sRemoteAudioText.mVtable = &sRemoteAudioTextVtable;
    sRemoteAudioText.mText = reinterpret_cast<be<u8>*>(&sRemoteAudioName);
}
/* 027658B8 */
void sead::remote_audio::AudioText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 027658CC */
void sead::remote_audio::AudioText::reserved() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 027658D0 */
static void initializeRemoteCommandText() {
    gabi::AutoFrame frame{gabi::current_function()};
    sRemoteCommandText.mVtable = &sRemoteCommandTextVtable;
    sRemoteCommandText.mText = reinterpret_cast<be<u8>*>(&sRemoteCommandName);
}
/* 027658F4 */
void sead::remote_audio::CommandText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 02765908 */
void sead::remote_audio::CommandText::reserved() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* WWHD glue
 data sRemoteQueueTable 1015F100
 data sRemoteAudioTextVtable 1015F0C8
 data sRemoteAudioName 1015F110
 data sRemoteCommandTextVtable 1015F124
 data sRemoteCommandName 1015F16C
 data sRemoteAudioChannelCount 101F8BB4
 data sRemoteAudioCommandCapacity 101F8BB8
 data sRemoteAudioText 104A1200
 data sRemoteCommandText 104A1208
 ext 0273AD10 void* allocateRemoteAudioObject(u32)
 ext 0273B0D4 void* allocateFoundationArray(u32, void*, s32)
 ext 0273B310 sead::Link* sead::Link::detach()
 ext 0273B4E8 void sead::LinkList::moveAfter(sead::Link*, sead::Link*)
 ext C0008558 s32 probeRemoteAudioController(u32, be<u32>*)
 ext 028A2138 void* remoteAudioManager()
 ext 028A2184 void* remoteAudioChannel(void*, u32)
 ext 028A1A74 u32 remoteAudioActive(void*)
 ext 028A1990 u32 playRemoteAudio(void*, u32)
 ext 028A1A00 void stopRemoteAudio(void*, u32)
*/

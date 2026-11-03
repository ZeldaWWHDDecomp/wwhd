/* sead remote-controller (Wii Remote speaker) audio: the speaker effect parameters, a queue of play/stop commands
 * per remote channel with its free-list storage, and the TU's text objects. Reconstructed from this game's
 * instructions. */
#pragma once
#include "sead/Foundation.h"
#include "sead/FileDevice.h"
namespace sead { namespace remote_audio {
struct EffectParameters {
    be<u32> mTable;
    be<u32> mMode;
    u8 mPrefix[12];
    be<f32> mGain;
    be<u32> mInterval;
    be<u32> mPriority;
    u8 mReserved[4];
    be<u32> mRequest;
    u8 mRequestPadding[4];
    be<u32> mResponse;
    void setGain(f32 gain);
    void setInterval(u32 interval);
    void setPriority(u32 priority);
    void setRequest(const be<u32>* request);
    void setResponse(const be<u32>* response);
};
struct Command {
    union { be<u32> mChannel; gptr<Command> mNextFree; };
    be<u32> mOperation;
    be<u32> mValue;
    be<s32> mRemaining;
    Link mLink;
    u32 execute();
};
struct CommandQueue {
    be<u32> mTable;
    be<u8> mEnabled;
    be<u8> mPaused;
    u8 mReserved[2];
    be<u32> mReservedWords[2];
    LinkList mCommands;
    gptr<Command> mStorage;
    gptr<Command> mFree;
    be<s32> mCapacity;
    u32 connected(u32 channel);
    void stopChannel(u32 channel);
    void destroy(u32 flags);
    void recycle(Command* command);
    void makeFirst(Command* command);
    void update();
};
/* SafeTexts with this translation unit's own vtable copies */
struct AudioText : SafeText { void destroy(u32 flags); void reserved(); };
struct CommandText : SafeText { void destroy(u32 flags); void reserved(); };
}
}
WWHD_OFFSET(sead::remote_audio::EffectParameters,mGain,0x14);
WWHD_OFFSET(sead::remote_audio::EffectParameters,mRequest,0x24);
WWHD_OFFSET(sead::remote_audio::EffectParameters,mResponse,0x2C);
WWHD_SIZE(sead::remote_audio::EffectParameters,0x30);
WWHD_OFFSET(sead::remote_audio::Command,mLink,0x10);
WWHD_SIZE(sead::remote_audio::Command,0x18);
WWHD_OFFSET(sead::remote_audio::CommandQueue,mCommands,0x10);
WWHD_OFFSET(sead::remote_audio::CommandQueue,mStorage,0x1C);
WWHD_OFFSET(sead::remote_audio::CommandQueue,mFree,0x20);
WWHD_SIZE(sead::remote_audio::CommandQueue,0x28);
WWHD_SIZE(sead::remote_audio::AudioText,8);
WWHD_SIZE(sead::remote_audio::CommandText,8);
void* allocateRemoteAudioObject(u32 size);
s32 probeRemoteAudioController(u32 channel, be<u32>* type);
void* remoteAudioManager();
void* remoteAudioChannel(void* manager, u32 channel);
u32 remoteAudioActive(void* channel);
u32 playRemoteAudio(void* channel, u32 value);
void stopRemoteAudio(void* channel, u32 value);

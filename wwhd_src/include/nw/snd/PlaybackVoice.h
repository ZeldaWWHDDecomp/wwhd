/* Playback voices (one or two voice handles with their mix, filter and callback state) and
 * the priority-ordered pool that owns them, reconstructed from WWHD. */
#pragma once
#include "wwhd.h"
#include "nw/ut/FrameHeap.h"
#include "nw/snd/ChannelParameters.h"
#include "nw/snd/SoundVoiceHandle.h"
namespace nw { namespace snd {

// Per-speaker gains of one channel after panning, plus the mono down-mix the remote speakers use.
struct SpeakerGains {
    enum Speaker { FrontLeft, FrontRight, RearLeft, RearRight, Center, LowFrequency, SpeakerCount };
    be<f32> speakers[SpeakerCount];
    be<f32> mono;
};
WWHD_SIZE(SpeakerGains, 0x1C);

// Main and auxiliary send levels of the four remote speakers, in the order the voice handle copies them.
struct RemoteSendLevels {
    be<f32> main[4];
    be<f32> auxiliary[4];
    be<u8> enabled;
    u8 padding[3];
};
WWHD_OFFSET(RemoteSendLevels, enabled, 0x20);
WWHD_SIZE(RemoteSendLevels, 0x24);

// Element `index` of a guest array, with the original's 32-bit address arithmetic: the callers
// pass unchecked indices (e.g. -1), which wrap around the guest address space.
template <class T> inline T* guestElement(T* first, u32 index) {
    return gabi::at<T>(gabi::ea(first) + index * u32(sizeof(T)));
}

// This translation unit's copies of the mix-state constructors.
ChannelMixState* constructVoiceMixState(ChannelMixState* state);
ChannelMixGroup* constructVoiceMixGroup(ChannelMixGroup* group);

struct PlaybackVoice {
    // Pending-update bits in mDirty.
    enum Dirty : u16 {
        DirtyStart = 1, DirtyPause = 2, DirtyPitch = 4, DirtyMix = 8,
        DirtyLowPass = 0x10, DirtyFilter = 0x20, DirtyVolume = 0x40, DirtyFilterPreset = 0x80
    };
    // Events passed to the owner's callback (voice, event, argument).
    enum Event { EventFinished = 0, EventStopped = 1, EventStolen = 2, EventVoiceLost = 3 };

    SoundVoiceHandle mChannels[2];
    be<s32> mChannelCount;
    be<u32> mCallback;
    be<u32> mCallbackArgument;
    be<u8> mAllocated, mStartRequested, mRunning, mPauseRequested, mPaused;
    u8 mPadding375[3];
    gptr<VoiceRequest> mCurrentRequest;
    be<u16> mDirty;
    be<u8> mFilterType, mFilterPreset, mGamepadSurround;
    u8 mPadding385[3];
    be<f32> mVolume, mPitch;
    be<u32> mPanMode, mPanCurve;
    be<f32> mLowPass, mFilterAmount;
    be<s32> mPriority;
    be<u32> mOutputMask;
    ChannelMixGroup mMainMix;
    ChannelMixGroup mGamepadMix[2];
    ChannelOutputGain mRemoteMix[4];
    be<u32> mSampleFormat;
    u8 mPadding4CC[4];
    nw::ut::HeapLink mPoolLink;

    static PlaybackVoice* construct(PlaybackVoice* voice);
    void destroy(u32 flags);
    u32 acquire(s32 channels, u32 priority, u32 callback, u32 argument, u32 allocationMode);
    void release();
    void reset(u32 callback, u32 argument);
    void stopChannels();
    void stop();
    u32 hasFinished() const;
    void applyVolume();
    void setPriority(s32 priority);
    void applyMix();
    void applyLowPass();
    void applyFilter();
    void applyFilterPreset();
    void startChannels();
    void pauseChannels();
    void calculateSpeakers(SpeakerGains* output, const ChannelMixGroup* mix, s32 channel, u32 device);
    void calculateSends(MixInput* output, u32 route, const ChannelMixGroup* mix, const SpeakerGains* speakers);
    void calculateMainSends(MixInput* output, const SpeakerGains* speakers);
    void calculateGamepadSends(u32 device, MixInput* output, const SpeakerGains* speakers);
    void calculateRemoteSends(RemoteSendLevels* output, const SpeakerGains* speakers);
    void prepareUpdate();
    void applyUpdate();
    void updatePlayback();
    void requestStart();
    void requestPause(u32 paused);
    void applyPitch();
    void setSampleFormat(u32 format);
    void setSampleRate(u32 rate);
    void setVolume(f64 volume);
    void setPitch(f64 pitch);
    void setPanMode(u32 mode);
    void setPanCurve(u32 curve);
    void setLowPass(f64 lowPass);
    void setFilter(u32 type, f64 amount);
    void setGamepadSurround(u32 enabled);
    void setInterpolationMode(u32 mode);
    void setFilterPreset(u32 preset);
    void setOutputMask(u32 mask);
    void updateMix(const ChannelMixGroup* source, ChannelMixGroup* target);
    void setMainMix(const ChannelMixGroup* source);
    void setGamepadMix(u32 device, const ChannelMixGroup* source);
    void setRemoteMix(u32 remote, const ChannelOutputGain* source);
    u32 sampleFormat() const;
    void appendRequest(u32 channel, VoiceRequest* request, u32 makeCurrent);
    void setAdpcmCoefficients(u32 channel, const be<u16>* coefficients);
};
WWHD_OFFSET(PlaybackVoice, mChannelCount, 0x368);
WWHD_OFFSET(PlaybackVoice, mCurrentRequest, 0x37C);
WWHD_OFFSET(PlaybackVoice, mDirty, 0x380);
WWHD_OFFSET(PlaybackVoice, mGamepadSurround, 0x384);
WWHD_OFFSET(PlaybackVoice, mVolume, 0x388);
WWHD_OFFSET(PlaybackVoice, mPriority, 0x3A0);
WWHD_OFFSET(PlaybackVoice, mMainMix, 0x3A8);
WWHD_OFFSET(PlaybackVoice, mGamepadMix, 0x3F8);
WWHD_OFFSET(PlaybackVoice, mRemoteMix, 0x498);
WWHD_OFFSET(PlaybackVoice, mSampleFormat, 0x4C8);
WWHD_OFFSET(PlaybackVoice, mPoolLink, 0x4D0);
WWHD_SIZE(PlaybackVoice, 0x4D8);

struct PlaybackVoicePool {
    be<u8> mInitialized;
    u8 mPadding[3];
    nw::ut::HeapChildren mActive;   // allocated voices, lowest priority first
    nw::ut::HeapChildren mFree;
    static PlaybackVoicePool* construct(PlaybackVoicePool* pool);
    static PlaybackVoicePool* instance();
    u32 requiredBytes(u32 count) const;
    void initialize(PlaybackVoice* storage, u32 bytes);
    void stopAll();
    void finalize();
    u32 stealVoice(s32 priority);
    void insertByPriority(PlaybackVoice* voice);
    PlaybackVoice* acquire(s32 channels, u32 priority, u32 callback, u32 argument, u32 allocationMode);
    void release(PlaybackVoice* voice);
    void releaseVoice(PlaybackVoice* voice);
    void updateParameters();
    void updatePlayback();
    void markDirty(u32 flags);
    void reorder(PlaybackVoice* voice);
};
WWHD_OFFSET(PlaybackVoicePool, mActive, 4);
WWHD_OFFSET(PlaybackVoicePool, mFree, 0x10);
WWHD_SIZE(PlaybackVoicePool, 0x1C);
} }

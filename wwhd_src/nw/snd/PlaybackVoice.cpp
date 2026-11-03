/* Playback voice lifetime: construction, channel acquisition and release, and the per-frame
 * completion check, reconstructed from WWHD. */
#include "nw/snd/PlaybackVoice.h"
#include "nw/snd/SoundRuntimeSupport.h"
using namespace nw::snd;
static AudioMixRuntime sMixRuntime;
static u32 sMixRuntimeGuard;
void* allocatePlaybackVoice(u32 bytes);
void freePlaybackVoice(void* storage);
void constructVoiceArray(void* storage, u32 count, u32 stride, u32 constructor);
void destroyVoiceArray(void* storage, u32 count, u32 stride, u32 destructor, u32 argument, u32 flags);

/* 0289D254 */
PlaybackVoice* PlaybackVoice::construct(PlaybackVoice* voice) {
    gabi::AutoFrame frame{0x0289D254u};
    if (!voice) voice = static_cast<PlaybackVoice*>(allocatePlaybackVoice(sizeof(PlaybackVoice)));
    if (!voice) return voice;
    constructVoiceArray(voice->mChannels, 2, sizeof(SoundVoiceHandle), gabi::guest_fn((const void*)&SoundVoiceHandle::construct));
    voice->mAllocated = 0;
    voice->mRunning = 0;
    voice->mChannelCount = 0;
    voice->mDirty = 0;
    voice->mCallback = 0;
    voice->mPauseRequested = 0;
    voice->mStartRequested = 0;
    constructVoiceArray(voice->mMainMix.channels, 2, sizeof(ChannelMixState), gabi::guest_fn((const void*)&constructVoiceMixState));
    constructVoiceArray(voice->mGamepadMix, 2, sizeof(ChannelMixGroup), gabi::guest_fn((const void*)&constructVoiceMixGroup));
    voice->mPoolLink.next = nullptr;
    voice->mPoolLink.previous = nullptr;
    return voice;
}

/* 0289D30C */
void PlaybackVoice::destroy(u32 flags) {
    gabi::AutoFrame frame{0x0289D30Cu};
    if (gabi::ea(this) == 0) return;   // GHS destructors accept NULL
    for (s32 channel = 0; channel < mChannelCount; ++channel) mChannels[channel].release();
    destroyVoiceArray(mChannels, 2, sizeof(SoundVoiceHandle), gabi::guest_fn((const void*)&SoundVoiceHandle::destroy), 0, 0);
    if (flags & 1) freePlaybackVoice(this);
}

/* 0289D3B4 */
void PlaybackVoice::reset(u32 callback, u32 argument) {
    mCallbackArgument = argument;
    mCallback = callback;
    mPauseRequested = 0;
    mFilterAmount = 0.0f;
    mVolume = 1.0f;
    mPitch = 1.0f;
    mLowPass = 1.0f;
    mMainMix.gain = 1.0f;
    mMainMix.mainSend = 0.0f;
    mFilterPreset = 0;
    mGamepadSurround = 0;
    mFilterType = 0;
    mStartRequested = 0;
    mDirty = 0;
    mCurrentRequest = nullptr;
    mPaused = 0;
    mRunning = 0;
    mMainMix.mode = 0;
    mPanCurve = 0;
    mMainMix.surroundPan = 0.0f;
    mMainMix.pan = 0.0f;
    mPanMode = 0;
    for (u32 bus = 0; bus < 3; ++bus) mMainMix.auxiliarySend[bus] = 0.0f;
    for (u32 device = 0; device < 2; ++device) {
        ChannelMixGroup& mix = mGamepadMix[device];
        mix.gain = 1.0f;
        mix.mainSend = 0.0f;
        mix.mode = 0;
        mix.pan = 0.0f;
        mix.surroundPan = 0.0f;
        for (u32 bus = 0; bus < 3; ++bus) mix.auxiliarySend[bus] = 0.0f;
    }
    for (u32 remote = 0; remote < 4; ++remote) {
        mRemoteMix[remote].pan = 0.0f;
        mRemoteMix[remote].surroundPan = 0.0f;
        mRemoteMix[remote].gain = 1.0f;
    }
}

/* 0289D4A4 */
u32 PlaybackVoice::acquire(s32 channels, u32 priority, u32 callback, u32 argument, u32 allocationMode) {
    gabi::AutoFrame frame{0x0289D4A4u};
    if (channels > 2) channels = 2;
    else if (channels < 1) channels = 1;
    mChannelCount = 0;
    s32 acquired = 0;
    while (acquired < channels) {
        if (!mChannels[acquired].acquire(priority, allocationMode)) break;
        ++acquired;
    }
    if (acquired != channels) {
        for (s32 channel = 0; channel < acquired; ++channel) mChannels[channel].release();
        return 0;
    }
    mChannelCount = channels;
    reset(callback, argument);
    mAllocated = 1;
    return 1;
}

/* 0289D598 */
void PlaybackVoice::release() {
    gabi::AutoFrame frame{0x0289D598u};
    if (!mAllocated) return;
    for (s32 channel = 0; channel < mChannelCount; ++channel) mChannels[channel].release();
    mChannelCount = 0;
    PlaybackVoicePool::instance()->releaseVoice(this);
    mAllocated = 0;
}

/* 0289D644 */
void PlaybackVoice::stopChannels() {
    gabi::AutoFrame frame{0x0289D644u};
    for (s32 channel = 0; channel < mChannelCount; ++channel) mChannels[channel].setState(1);
}

/* 0289D6B0 */
void PlaybackVoice::stop() {
    gabi::AutoFrame frame{0x0289D6B0u};
    if (mRunning) {
        stopChannels();
        mRunning = 0;
    }
    mPauseRequested = 0;
    mPaused = 0;
    mStartRequested = 0;
}

/* 0289D708 */
u32 PlaybackVoice::hasFinished() const {
    if (mChannelCount <= 0) return 0;
    const VoiceRequest* request = mCurrentRequest;
    if (!request) return 0;
    const u32 state = request->state;
    return state != 1 && state != 2;
}

/* 0289D744 */
void PlaybackVoice::updatePlayback() {
    gabi::AutoFrame frame{0x0289D744u};
    if (!mAllocated || !mRunning) return;
    if (hasFinished()) {
        const u32 callback = mCallback;
        if (callback) gabi::call_ptr(callback, this, u32(EventFinished), u32(mCallbackArgument));
        mRunning = 0;
        mStartRequested = 0;
        return;
    }
    for (s32 channel = 0; channel < mChannelCount; ++channel) {
        if (mChannels[channel].isActive()) continue;
        // A channel lost its voice: give the whole voice back and tell the owner.
        for (s32 other = 0; other < mChannelCount; ++other) mChannels[other].release();
        mChannelCount = 0;
        mAllocated = 0;
        PlaybackVoicePool::instance()->releaseVoice(this);
        const u32 callback = mCallback;
        if (callback) gabi::call_ptr(callback, this, u32(EventVoiceLost), u32(mCallbackArgument));
        mStartRequested = 0;
        mRunning = 0;
        return;
    }
}

/* 0289D910 */
void PlaybackVoice::applyVolume() {
    gabi::AutoFrame frame{0x0289D910u};
    const f32 volume = mVolume;
    if (!sMixRuntimeGuard) {
        sMixRuntimeGuard = 1;
        AudioMixRuntime::create(&sMixRuntime);
    }
    const f32 outputVolume = gabi::fmuls_ppc(volume, outputMasterVolume(&sMixRuntime));
    for (s32 channel = 0; channel < mChannelCount; ++channel) mChannels[channel].parameters.volume = outputVolume;
}

/* 0289EC48 */
void PlaybackVoice::setPriority(s32 priority) {
    gabi::AutoFrame frame{0x0289EC48u};
    mPriority = priority;
    PlaybackVoicePool::instance()->reorder(this);
    for (s32 channel = 0; channel < mChannelCount; ++channel) mChannels[channel].setPriority(mPriority);
}

/* WWHD glue
data sMixRuntime 101FCE00
data sMixRuntimeGuard 101FD8C8
ext 0273AD10 void* allocatePlaybackVoice(u32)
ext 0273AF40 void freePlaybackVoice(void*)
ext 028EFFD0 void constructVoiceArray(void*,u32,u32,u32)
ext 028F0164 void destroyVoiceArray(void*,u32,u32,u32,u32,u32)
ext 028A5CF8 nw::snd::SoundVoiceHandle* nw::snd::SoundVoiceHandle::construct(nw::snd::SoundVoiceHandle*) callback
ext 028A5D5C void nw::snd::SoundVoiceHandle::destroy(nw::snd::SoundVoiceHandle*,u32) callback
ext 028A5DE4 u32 nw::snd::SoundVoiceHandle::acquire(u32,u32)
ext 028A5F00 void nw::snd::SoundVoiceHandle::release()
ext 028A5F9C void nw::snd::SoundVoiceHandle::setState(u32)
ext 028A5F64 u32 nw::snd::SoundVoiceHandle::isActive()
ext 028A6110 void nw::snd::SoundVoiceHandle::setPriority(u32)
ext 02888A50 nw::snd::AudioMixRuntime* nw::snd::AudioMixRuntime::create(nw::snd::AudioMixRuntime*)
ext 02889E48 f32 nw::snd::outputMasterVolume(nw::snd::AudioMixRuntime*)
*/

/* Playback voice requests and parameter setters: each setter stores a changed value and marks
 * it for the next update, reconstructed from WWHD. */
#include "nw/snd/PlaybackVoice.h"
using namespace nw::snd;

// Stores value when it differs from target (a NaN always differs); true when it stored.
static bool storeIfChanged(be<f32>& target, f32 value) {
    if (value == target) return false;
    target = value;
    return true;
}

/* 0289D624 */
void PlaybackVoice::requestStart() {
    const u16 dirty = mDirty;
    mStartRequested = 1;
    mPauseRequested = 0;
    mDirty = dirty | DirtyStart;
}

/* 0289D8BC */
void PlaybackVoice::requestPause(u32 paused) {
    if (mPauseRequested == paused) return;
    const u16 dirty = mDirty;
    mPauseRequested = paused;
    mDirty = dirty | DirtyPause;
}

/* 0289D8DC */
void PlaybackVoice::applyPitch() {
    const f32 pitch = mPitch;
    for (s32 channel = 0; channel < mChannelCount; ++channel) mChannels[channel].parameters.pitch = pitch;
}

/* 0289EAB0 */
void PlaybackVoice::setSampleFormat(u32 format) {
    const s32 count = mChannelCount;
    mSampleFormat = format;
    if (count <= 0) return;
    // The loop re-reads the stored mode after each channel.
    s32 channel = 0;
    while (true) {
        mChannels[channel].sampleFormat = format;
        if (++channel >= mChannelCount) return;
        format = mSampleFormat;
    }
}

/* 0289EAE8 */
void PlaybackVoice::setSampleRate(u32 rate) {
    for (s32 channel = 0; channel < mChannelCount; ++channel) mChannels[channel].sampleRate = rate;
}

/* 0289EB18 */
void PlaybackVoice::setVolume(f64 volume) {
    volume = volume >= 0.0f ? volume : 0.0f;
    if (volume == mVolume) return;
    const u16 dirty = mDirty;
    mVolume = gabi::f32_from_bits(ppc_stfs_bits(volume));
    mDirty = dirty | DirtyVolume;
}

/* 0289EB44 */
void PlaybackVoice::setPitch(f64 pitch) {
    if (pitch == mPitch) return;
    const u16 dirty = mDirty;
    mPitch = gabi::f32_from_bits(ppc_stfs_bits(pitch));
    mDirty = dirty | DirtyPitch;
}

/* 0289EB64 */
void PlaybackVoice::setPanMode(u32 mode) {
    if (mode == mPanMode) return;
    const u16 dirty = mDirty;
    mPanMode = mode;
    mDirty = dirty | DirtyMix;
}

/* 0289EB84 */
void PlaybackVoice::setPanCurve(u32 curve) {
    if (curve == mPanCurve) return;
    const u16 dirty = mDirty;
    mPanCurve = curve;
    mDirty = dirty | DirtyMix;
}

/* 0289EBA4 */
void PlaybackVoice::setLowPass(f64 lowPass) {
    if (lowPass == mLowPass) return;
    const u16 dirty = mDirty;
    mLowPass = gabi::f32_from_bits(ppc_stfs_bits(lowPass));
    mDirty = dirty | DirtyLowPass;
}

/* 0289EBC4 */
void PlaybackVoice::setFilter(u32 type, f64 amount) {
    const u8 oldType = mFilterType;
    if (amount > 1.0f) amount = 1.0f;
    else amount = amount >= 0.0f ? amount : 0.0f;
    const f32 oldAmount = mFilterAmount;
    const bool typeChanged = type != oldType;
    if (typeChanged) mFilterType = type;
    if (amount != oldAmount) {
        const u16 dirty = mDirty;
        mFilterAmount = gabi::f32_from_bits(ppc_stfs_bits(amount));
        mDirty = dirty | DirtyFilter;
    } else if (typeChanged) {
        mDirty = u16(mDirty) | DirtyFilter;
    }
}

/* 0289ECC4 */
void PlaybackVoice::setGamepadSurround(u32 enabled) {
    for (s32 channel = 0; channel < mChannelCount; ++channel) mChannels[channel].parameters.gamepadSurround = enabled;
    mGamepadSurround = enabled;
}

/* 0289ECF8 */
void PlaybackVoice::setInterpolationMode(u32 mode) {
    for (s32 channel = 0; channel < mChannelCount; ++channel) mChannels[channel].parameters.interpolationMode = mode;
}

/* 0289ED28 */
void PlaybackVoice::setFilterPreset(u32 preset) {
    if (preset > 127) preset = 127;
    if (preset == mFilterPreset) return;
    const u16 dirty = mDirty;
    mFilterPreset = preset;
    mDirty = dirty | DirtyFilterPreset;
}

/* 0289ED6C */
void PlaybackVoice::setOutputMask(u32 mask) {
    if (mask == mOutputMask) return;
    const u16 dirty = mDirty;
    mOutputMask = mask;
    mDirty = dirty | DirtyMix;
}

/* 0289ED8C */
void PlaybackVoice::updateMix(const ChannelMixGroup* source, ChannelMixGroup* target) {
    gabi::AutoFrame frame{0x0289ED8Cu};
    f32 gain = source->gain;
    gain = gain >= 0.0f ? gain : 0.0f;
    if (storeIfChanged(target->gain, gain)) mDirty = u16(mDirty) | DirtyMix;
    const u32 mode = source->mode;
    if (mode != target->mode) {
        target->mode = mode;
        mDirty = u16(mDirty) | DirtyMix;
    }
    // The six words of each channel state are its speaker gains (front left/right, rear
    // left/right, centre, low frequency) when the group is in matrix mode.
    for (u32 channel = 0; channel < 2; ++channel) {
        const ChannelMixState& from = source->channels[channel];
        ChannelMixState& to = target->channels[channel];
        if (storeIfChanged(to.gain, from.gain)) mDirty = u16(mDirty) | DirtyMix;
        if (storeIfChanged(to.pitch, from.pitch)) mDirty = u16(mDirty) | DirtyMix;
        if (storeIfChanged(to.pan, from.pan)) mDirty = u16(mDirty) | DirtyMix;
        if (storeIfChanged(to.surroundPan, from.surroundPan)) mDirty = u16(mDirty) | DirtyMix;
        if (storeIfChanged(to.mainSend, from.mainSend)) mDirty = u16(mDirty) | DirtyMix;
        if (storeIfChanged(to.auxiliarySend, from.auxiliarySend)) mDirty = u16(mDirty) | DirtyMix;
    }
    if (storeIfChanged(target->pan, source->pan)) mDirty = u16(mDirty) | DirtyMix;
    if (storeIfChanged(target->surroundPan, source->surroundPan)) mDirty = u16(mDirty) | DirtyMix;
    if (storeIfChanged(target->mainSend, source->mainSend)) mDirty = u16(mDirty) | DirtyMix;
    for (u32 bus = 0; bus < 3; ++bus)
        if (storeIfChanged(target->auxiliarySend[bus], source->auxiliarySend[bus])) mDirty = u16(mDirty) | DirtyMix;
}

/* 0289EEE0 */
void PlaybackVoice::setMainMix(const ChannelMixGroup* source) {
    gabi::AutoFrame frame{0x0289EEE0u};
    updateMix(source, &mMainMix);
}

/* 0289EEE8 */
void PlaybackVoice::setGamepadMix(u32 device, const ChannelMixGroup* source) {
    gabi::AutoFrame frame{0x0289EEE8u};
    updateMix(source, guestElement(mGamepadMix, device));
}

/* 0289EEFC */
void PlaybackVoice::setRemoteMix(u32 remote, const ChannelOutputGain* source) {
    ChannelOutputGain& target = *guestElement(mRemoteMix, remote);
    f32 gain = source->gain;
    gain = gain >= 0.0f ? gain : 0.0f;
    if (storeIfChanged(target.gain, gain)) mDirty = u16(mDirty) | DirtyMix;
    if (storeIfChanged(target.pan, source->pan)) mDirty = u16(mDirty) | DirtyMix;
    if (storeIfChanged(target.surroundPan, source->surroundPan)) mDirty = u16(mDirty) | DirtyMix;
}

/* 0289EF74 */
u32 PlaybackVoice::sampleFormat() const { return mSampleFormat; }

/* WWHD glue
*/

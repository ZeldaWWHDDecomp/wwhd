/* Playback voice frame update: applies the parameters marked dirty and performs the requested
 * start/pause transitions on the voice handles, reconstructed from WWHD. */
#include "nw/snd/PlaybackVoice.h"
using namespace nw::snd;

/* 0289E738 */
void PlaybackVoice::prepareUpdate() {
    gabi::AutoFrame frame{0x0289E738u};
    if (!mStartRequested) return;
    if (u16(mDirty) & DirtyPitch) {
        applyPitch();
        mDirty = u16(mDirty) & ~DirtyPitch;
    }
    if (u16(mDirty) & DirtyVolume) {
        applyVolume();
        mDirty = u16(mDirty) & ~DirtyVolume;
    }
    if (u16(mDirty) & DirtyMix) {
        applyMix();
        mDirty = u16(mDirty) & ~DirtyMix;
    }
    if (u16(mDirty) & DirtyLowPass) {
        applyLowPass();
        mDirty = u16(mDirty) & ~DirtyLowPass;
    }
    if (u16(mDirty) & DirtyFilter) {
        applyFilter();
        mDirty = u16(mDirty) & ~DirtyFilter;
    }
    if (u16(mDirty) & DirtyFilterPreset) {
        applyFilterPreset();
        mDirty = u16(mDirty) & ~DirtyFilterPreset;
    }
}

/* 0289E81C */
void PlaybackVoice::startChannels() {
    gabi::AutoFrame frame{0x0289E81Cu};
    for (s32 channel = 0; channel < mChannelCount; ++channel) mChannels[channel].setState(0);
}

/* 0289E888 */
void PlaybackVoice::pauseChannels() {
    gabi::AutoFrame frame{0x0289E888u};
    for (s32 channel = 0; channel < mChannelCount; ++channel) mChannels[channel].setState(2);
}

/* 0289E8F4 */
void PlaybackVoice::applyUpdate() {
    enum Transition { None, Start, Stop, Pause };
    gabi::AutoFrame frame{0x0289E8F4u};
    if (!mAllocated) return;
    u16 dirty = mDirty;
    Transition transition = None;
    if ((dirty & DirtyStart) && mStartRequested && !mRunning) {
        applyPitch();
        applyMix();
        applyVolume();
        dirty = mDirty;
        transition = Start;
        mRunning = 1;
        dirty &= ~(DirtyStart | DirtyPitch | DirtyMix);
        mDirty = dirty;
    }
    if (mRunning && (dirty & DirtyPause) && mStartRequested) {
        const u8 paused = mPauseRequested;
        dirty = mDirty;
        if (paused) {
            mPaused = 1;
            transition = Pause;
        } else {
            mPaused = 0;
            transition = Start;
        }
        mDirty = dirty & ~DirtyPause;
    }
    for (s32 channel = 0; channel < mChannelCount; ++channel) mChannels[channel].synchronizeParameters();
    switch (transition) {
    case Start: startChannels(); break;
    case Stop: stopChannels(); break;      // kept by the original although nothing sets it
    case Pause: pauseChannels(); break;
    default: break;
    }
}

/* WWHD glue
ext 028A6200 void nw::snd::SoundVoiceHandle::synchronizeParameters()
*/

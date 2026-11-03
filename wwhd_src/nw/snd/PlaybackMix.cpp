/* Playback voice mixing: panning to speaker gains, main/auxiliary/remote sends per output
 * device, and the low-pass, biquad and filter-preset settings of the voice handles,
 * reconstructed from WWHD. */
#include "nw/snd/PlaybackVoice.h"
#include "nw/snd/AudioCurves.h"
#include "nw/snd/BasicSound.h"
#include "nw/snd/AudioMixRuntime.h"
using namespace nw::snd;
static AudioMixRuntime sMixRuntime;
static u32 sMixRuntimeGuard;
// The filter preset objects (AudioMixRuntime::outputPointers, indexed by filter type).
struct FilterPresetVtable { VtableEntry header, destroy, coefficients; };
struct FilterPreset { gptr<const FilterPresetVtable> vtable; };
static u32 routeBit(u32 index);

static void createMixRuntime() {
    if (!sMixRuntimeGuard) {
        sMixRuntimeGuard = 1;
        AudioMixRuntime::create(&sMixRuntime);
    }
}

/* 0289D9B8 */
void PlaybackVoice::calculateSpeakers(SpeakerGains* output, const ChannelMixGroup* mix, s32 channel, u32 device) {
    gabi::AutoFrame frame{0x0289D9B8u};
    createMixRuntime();
    const u32 mode = *guestElement(sMixRuntime.outputMode, device);
    f32 frontLeft, frontRight, rearLeft, rearRight, center = 0.0f, lowFrequency = 0.0f, mono;
    if (!mix->mode) {
        f32 left = 0.0f, right = 0.0f;
        gabi::FrameLocal<PanCurve> curve{8};
        curve->mDisableSurround = 0;
        u32 table = 0;
        u8 normalize = 0, clamp = 0;
        switch (u32(mPanCurve)) {
        case 1: table = 0; normalize = 1; clamp = 0; break;
        case 2: table = 0; normalize = 1; clamp = 1; break;
        case 3: table = 1; normalize = 0; clamp = 0; break;
        case 4: table = 1; normalize = 1; clamp = 0; break;
        case 5: table = 1; normalize = 1; clamp = 1; break;
        case 6: table = 2; normalize = 0; clamp = 0; break;
        case 7: table = 2; normalize = 1; clamp = 0; break;
        case 8: table = 2; normalize = 1; clamp = 1; break;
        default: break;
        }
        curve->mCurve = table;
        curve->mNormalizeCenter = normalize;
        curve->mClampUnity = clamp;
        if (mode == 0) {
            left = curve->panGain(0, 0.0);
            right = left;
        } else if (mode <= 2) {
            if (mChannelCount > 1 && mPanMode == 1) {
                // Balance mode: each channel of a stereo voice keeps its own side.
                if (channel == 0) left = curve->panGain(s32(mode), gabi::fpr_from_f32(mix->pan));
                else if (channel == 1) right = curve->panGain(s32(mode), gabi::fpr_from_f32(-f32(mix->pan)));
            } else {
                f32 pan = mix->pan;
                if (mChannelCount == 2) {
                    if (channel == 0) pan = gabi::fsubs_ppc(pan, 1.0f);
                    else if (channel == 1) pan = gabi::fadds_ppc(pan, 1.0f);
                }
                left = curve->panGain(s32(mode), gabi::fpr_from_f32(pan));
                right = curve->panGain(s32(mode), gabi::fpr_from_f32(-pan));
            }
        }
        f32 front = 0.0f, rear = 0.0f;
        if (mode < 2) {
            front = curve->attenuationGain(0.0);
            rear = curve->attenuationGain(2.0);
        } else if (mode == 2) {
            front = curve->attenuationGain(gabi::fpr_from_f32(mix->surroundPan));
            rear = curve->attenuationGain(gabi::fpr_from_f32(gabi::fsubs_ppc(2.0f, mix->surroundPan)));
        }
        frontLeft = gabi::fmuls_ppc(front, left);
        frontRight = gabi::fmuls_ppc(front, right);
        rearLeft = gabi::fmuls_ppc(rear, left);
        rearRight = gabi::fmuls_ppc(rear, right);
        mono = gabi::fmuls_ppc(gabi::fadds_ppc(left, right), 0.5f);
    } else {
        // Matrix mode: the channel state holds the six speaker gains directly.
        const ChannelMixState& gains = *guestElement(mix->channels, u32(channel));
        frontLeft = gains.gain;
        frontRight = gains.pitch;
        rearLeft = gains.pan;
        rearRight = gains.surroundPan;
        lowFrequency = gains.auxiliarySend;
        center = gains.mainSend;
        const f32 fronts = gabi::fadds_ppc(frontLeft, frontRight);
        const f32 withRearLeft = gabi::fadds_ppc(fronts, rearLeft);
        mono = gabi::fmuls_ppc(gabi::fadds_ppc(withRearLeft, rearRight), 0.25f);
    }
    createMixRuntime();
    // Fold the speakers the output device does not have into the ones it has.
    const u32 requested = *guestElement(sMixRuntime.requestedMode, device);
    if (requested == 0) {
        if (mode == 1) {
            frontLeft = frontRight = gabi::fmuls_ppc(gabi::fadds_ppc(frontLeft, frontRight), 0.5f);
        } else if (mode == 2) {
            const f32 left = gabi::fadds_ppc(frontLeft, rearLeft);
            const f32 right = gabi::fadds_ppc(frontRight, rearRight);
            frontLeft = frontRight = gabi::fmuls_ppc(gabi::fadds_ppc(left, right), 0.5f);
            rearLeft = rearRight = 0.0f;
        }
    } else if (requested == 1 && mode == 2) {
        frontLeft = gabi::fadds_ppc(frontLeft, rearLeft);
        frontRight = gabi::fadds_ppc(frontRight, rearRight);
        rearLeft = rearRight = 0.0f;
    }
    output->speakers[SpeakerGains::FrontLeft] = frontLeft;
    output->speakers[SpeakerGains::FrontRight] = frontRight;
    output->speakers[SpeakerGains::RearLeft] = rearLeft;
    output->speakers[SpeakerGains::RearRight] = rearRight;
    output->speakers[SpeakerGains::Center] = center;
    output->speakers[SpeakerGains::LowFrequency] = lowFrequency;
    output->mono = mono;
}

/* 0289DFE4 */
void PlaybackVoice::calculateSends(MixInput* output, u32 route, const ChannelMixGroup* mix, const SpeakerGains* speakers) {
    gabi::AutoFrame frame{0x0289DFE4u};
    f32 mainSend = 0.0f;
    gabi::FrameLocal<be<f32>[3]> auxiliarySend{8};
    for (u32 bus = 0; bus < 3; ++bus) (*auxiliarySend)[bus] = 0.0f;
    u32 routes = 0;
    for (u32 bit = 0; bit < 32; ++bit) {
        if (u32(mOutputMask) & routeBit(bit)) {
            createMixRuntime();
            routes |= u8(sMixRuntime.outputBits[bit]);
        }
    }
    if (routes & routeBit(route)) {
        const f32 send = gabi::fadds_ppc(mix->mainSend, 1.0f);
        mainSend = send > 1.0f ? 1.0f : send >= 0.0f ? send : 0.0f;
        for (u32 bus = 0; bus < 3; ++bus) {
            const f32 amount = mix->auxiliarySend[bus];
            (*auxiliarySend)[bus] = amount > 1.0f ? 1.0f : amount >= 0.0f ? amount : 0.0f;
        }
    }
    // Bus 0 of the result is the main send, buses 1..3 the auxiliary sends.
    for (u32 speaker = 0; speaker < SpeakerGains::SpeakerCount; ++speaker) {
        const f32 gain = gabi::fmuls_ppc(speakers->speakers[speaker], mix->gain);
        output->bus[0][speaker] = gabi::fmuls_ppc(mainSend, gain);
        for (u32 bus = 0; bus < 3; ++bus) output->bus[bus + 1][speaker] = gabi::fmuls_ppc((*auxiliarySend)[bus], gain);
    }
}

/* 0289E1B0 */
void PlaybackVoice::calculateMainSends(MixInput* output, const SpeakerGains* speakers) {
    gabi::AutoFrame frame{0x0289E1B0u};
    calculateSends(output, 0, &mMainMix, speakers);
}

/* 0289E1C0 */
void PlaybackVoice::calculateRemoteSends(RemoteSendLevels* output, const SpeakerGains* speakers) {
    gabi::AutoFrame frame{0x0289E1C0u};
    output->enabled = 0;
    const f32 mono = speakers->mono;
    u32 routes = 0;
    for (u32 bit = 0; bit < 32; ++bit) {
        if (u32(mOutputMask) & routeBit(bit)) {
            createMixRuntime();
            routes |= u8(sMixRuntime.outputBits[bit]);
        }
    }
    // Remote speaker n is route bit n + 1; its gain, main send and auxiliary send are the three
    // words of mRemoteMix[n].
    for (u32 remote = 0; remote < 4; ++remote) {
        if (!(routes & (2u << remote))) continue;
        const ChannelOutputGain& mix = mRemoteMix[remote];
        const f32 gain = mix.gain;
        const f32 send = gabi::fadds_ppc(mix.pan, 1.0f);
        const f32 scaled = gabi::fmuls_ppc(mono, gain);
        const f32 main = send > 1.0f ? 1.0f : send >= 0.0f ? send : 0.0f;
        output->main[remote] = gabi::fmuls_ppc(main, scaled);
        const f32 auxiliary = mix.surroundPan;
        const f32 clamped = auxiliary > 1.0f ? 1.0f : auxiliary >= 0.0f ? auxiliary : 0.0f;
        output->auxiliary[remote] = gabi::fmuls_ppc(clamped, scaled);
        output->enabled = 1;
    }
}

/* 0289E32C */
void PlaybackVoice::calculateGamepadSends(u32 device, MixInput* output, const SpeakerGains* speakers) {
    gabi::AutoFrame frame{0x0289E32Cu};
    // Gamepad devices are send routes 5 and 6.
    calculateSends(output, device + 5, guestElement(mGamepadMix, device), speakers);
}

/* 0289E34C */
void PlaybackVoice::applyMix() {
    gabi::AutoFrame frame{0x0289E34Cu};
    gabi::FrameLocal<SpeakerGains> mainSpeakers{0x48};
    gabi::FrameLocal<SpeakerGains> gamepadSpeakers{8};
    gabi::FrameLocal<MixInput> sends{0x64};
    gabi::FrameLocal<RemoteSendLevels> remoteSends{0x24};
    for (s32 channel = 0; channel < mChannelCount; ++channel) {
        SoundVoiceHandle& handle = mChannels[channel];
        // mainMix, gamepadMix and secondGamepadMix are consecutive.
        MixInput* deviceMixes = &handle.parameters.mainMix;
        calculateSpeakers(mainSpeakers.get(), &mMainMix, channel, 0);
        calculateMainSends(sends.get(), mainSpeakers.get());
        for (u32 bus = 0; bus < 4; ++bus)
            for (u32 speaker = 0; speaker < SpeakerGains::SpeakerCount; ++speaker)
                deviceMixes[0].bus[bus][speaker] = sends->bus[bus][speaker];
        for (u32 remote = 0; remote < 4; ++remote) {
            remoteSends->main[remote] = 0.0f;
            remoteSends->auxiliary[remote] = 0.0f;
        }
        remoteSends->enabled = 0;
        calculateRemoteSends(remoteSends.get(), mainSpeakers.get());
        handle.setSendLevels(reinterpret_cast<const be<u32>*>(remoteSends.get()));
        for (u32 device = 0; device < 2; ++device) {
            calculateSpeakers(gamepadSpeakers.get(), &mGamepadMix[device], channel, 1);
            calculateGamepadSends(device, sends.get(), gamepadSpeakers.get());
            for (u32 bus = 0; bus < 4; ++bus)
                for (u32 speaker = 0; speaker < SpeakerGains::SpeakerCount; ++speaker)
                    deviceMixes[device + 1].bus[bus][speaker] = sends->bus[bus][speaker];
        }
    }
}

/* 0289E4E4 */
void PlaybackVoice::applyLowPass() {
    gabi::AutoFrame frame{0x0289E4E4u};
    const u32 frequency = cutoffFrequency(gabi::fpr_from_f32(mLowPass));
    for (s32 channel = 0; channel < mChannelCount; ++channel) {
        if (frequency < 16000) mChannels[channel].setLowPass(1, frequency);
        else mChannels[channel].setLowPass(0, 0);
    }
}

/* 0289E594 */
void PlaybackVoice::applyFilter() {
    gabi::AutoFrame frame{0x0289E594u};
    for (s32 channel = 0; channel < mChannelCount; ++channel) {
        createMixRuntime();
        const u32 type = mFilterType;
        FilterPreset* preset = gabi::at<FilterPreset>(sMixRuntime.outputPointers[type]);
        if (!preset) {
            mChannels[channel].setBiquad(0, nullptr);
            continue;
        }
        const f32 amount = mFilterAmount;
        if (!(amount > 0.0f)) {
            mChannels[channel].setBiquad(0, nullptr);
            continue;
        }
        gabi::FrameLocal<FilterCoefficients> coefficients{8};
        gabi::call_ptr(preset->vtable->coefficients.function, preset, type, coefficients.get(), amount);
        mChannels[channel].setBiquad(1, coefficients.get());
    }
}

// PowerPC slw: a shift count of 32 or more gives 0.
static u32 routeBit(u32 index) { return (index & 32) ? 0 : 1u << (index & 31); }

/* 0289E69C */
void PlaybackVoice::applyFilterPreset() {
    gabi::AutoFrame frame{0x0289E69Cu};
    for (s32 channel = 0; channel < mChannelCount; ++channel) {
        const u32 preset = mFilterPreset;
        if (preset) mChannels[channel].setRemoteFilter(1, preset);
        else mChannels[channel].setRemoteFilter(0, 0);
    }
}

/* WWHD glue
data sMixRuntime 101FCE00
data sMixRuntimeGuard 101FD8C8
ext 028A6170 void nw::snd::SoundVoiceHandle::setSendLevels(const be<u32>*)
ext 028A61A0 void nw::snd::SoundVoiceHandle::setLowPass(u32,u32)
ext 028A61B4 void nw::snd::SoundVoiceHandle::setBiquad(u32,const nw::snd::FilterCoefficients*)
ext 028A61EC void nw::snd::SoundVoiceHandle::setRemoteFilter(u32,u32)
*/

/* Per-tick wave-channel envelope, modulation and mixer dispatch. From WWHD only. */
#include "nw/snd/WaveChannel.h"
using namespace nw::snd;
struct WaveAuxiliaryMix { ChannelMixGroup groups[2]; };
WWHD_SIZE(WaveAuxiliaryMix, 0xA0);
f64 wavePitchRatio(s32 steps);
f64 waveDecibelAmplitude(f64 decibels);
void constructUpdateMix(void* storage,u32 count,u32 stride,u32 constructor);

/* inlined into WaveChannel::update */
static void copyWaveMix(ChannelMixGroup* output,const ChannelMixGroup* input) {
    be<u32>* destination = (be<u32>*)output;
    const be<u32>* source = (const be<u32>*)input;
    for (u32 i = 0; i < 20; ++i) destination[i] = source[i];
}

/* 028A4164 */
void WaveChannel::update(u32 ticks) {
    gabi::AutoFrame frame{0x028A4164};
    if (!started) return;
    if (active) ticks = 0;
    u32 elapsed = control.flags;
    u32 duration = control.value;
    u32 currentLevel;
    if (elapsed < duration) {
        elapsed = (elapsed + 1) & 0xFFFF;
        duration = control.value;
        control.flags = elapsed;
    }
    if (elapsed < duration) {
        s32 target = control.bus;
        s32 initial = control.index;
        s32 difference = target - initial;
        s32 product = difference * (s32)elapsed;
        currentLevel = (u8)(initial + product / (s32)duration);
    } else {
        currentLevel = control.bus;
    }
    f32 currentVolume = gabi::fmuls_ppc(volume,mix.gain);
    currentVolume = gabi::fmuls_ppc(currentVolume,(f32)currentLevel);
    currentVolume = gabi::fdivs_ppc(currentVolume,255.0f);
    if (envelope.stage == Envelope::Release) {
        f32 decibels = envelope.decibels();
        if (decibels < -90.40000152587890625f) {
            stop();
            return;
        }
    }
    s32 noteDifference = (s32)(u32)key - (s32)(u32)baseKey;
    f32 notePitch = gabi::fadds_ppc((f32)noteDifference,streamValue);
    f32 release = releaseValue();
    f32 steps = gabi::fadds_ppc(notePitch,release);
    for (u32 i = 0; i < 4; ++i) {
        if (modulationTargets[i] == 0) steps = gabi::fadds_ppc(steps,modulators[i].value());
    }
    f32 currentPitch;
    if (steps == (f32)filter) {
        f32 cached = scale;
        f32 pitchValue = pitch;
        f32 product = gabi::fmuls_ppc(cached,pitchValue);
        currentPitch = gabi::fmuls_ppc(product,mix.pitch);
    } else {
        s32 fixedSteps = gabi::ftoi(gabi::fmuls_ppc(steps,256.0f));
        f64 ratio = wavePitchRatio(fixedSteps);
        f32 pitchValue = pitch;
        f32 mixPitch = mix.pitch;
        f32 product = gabi::fmuls_fpr(ratio,pitchValue);
        filter = steps;
        currentPitch = gabi::fmuls_ppc(product,mixPitch);
        scale = gabi::f32_from_bits(ppc_stfs_bits(ratio));
    }
    gabi::FrameLocal<ChannelMixGroup> main(0x10);
    copyWaveMix(main.get(),&mix.groups[0]);
    f32 mainPan = gabi::fadds_ppc(mix.groups[0].pan,pan);
    main->pan = mainPan;
    for (u32 i = 0; i < 4; ++i) {
        if (modulationTargets[i] == 2) {
            mainPan = gabi::fadds_ppc(mainPan,modulators[i].value());
            main->pan = mainPan;
        }
    }
    main->surroundPan = gabi::fadds_ppc(main->surroundPan,send);
    gabi::FrameLocal<WaveAuxiliaryMix> auxiliary(0x60);
    constructUpdateMix(auxiliary.get(),2,sizeof(ChannelMixGroup),
        gabi::guest_fn((const void*)&ChannelMixGroup::constructForWave));
    for (u32 group = 0; group < 2; ++group) {
        ChannelMixGroup* destination = &auxiliary->groups[group];
        copyWaveMix(destination,&mix.groups[group + 1]);
        f32 groupPan = gabi::fadds_ppc(destination->pan,pan);
        destination->pan = groupPan;
        for (u32 i = 0; i < 4; ++i) {
            if (modulationTargets[i] == 2) {
                groupPan = gabi::fadds_ppc(groupPan,modulators[i].value());
                destination->pan = groupPan;
            }
        }
        destination->surroundPan = gabi::fadds_ppc(destination->surroundPan,send);
    }
    if (ticks) {
        if (autoRelease) advanceRelease(3);
        for (u32 i = 0; i < 4; ++i) modulators[i].update(3);
        envelope.update(3);
    }
    f32 filterValue = gabi::fadds_ppc(mix.pan,1.0f);
    f64 envelopeValue = gabi::fpr_from_f32(envelope.decibels());
    f64 amplitude = waveDecibelAmplitude(envelopeValue);
    currentVolume = gabi::fmuls_fpr(currentVolume,amplitude);
    for (u32 i = 0; i < 4; ++i) {
        if (modulationTargets[i] == 1) {
            f32 amount = gabi::fmuls_ppc(modulators[i].value(),6.0f);
            currentVolume = gabi::fmuls_fpr(currentVolume,waveDecibelAmplitude(amount));
        }
    }
    PlaybackVoice* current = voice;
    if (!current) return;
    current->setVolume(currentVolume);
    voice->setPitch(currentPitch);
    voice->setPanMode(gabi::ea(track.get()));
    voice->setPanCurve(flags);
    voice->setFilterPreset(ending);
    voice->setLowPass(filterValue);
    f64 surround = gabi::fpr_from_f32((f32)mix.surroundPan);
    current = voice;
    u32 enabled = panEnabled;
    current->setFilter(enabled, surround);
    voice->setOutputMask(mix.mode);
    voice->setMainMix(main.get());
    for (u32 i = 0; i < 2; ++i) voice->setGamepadMix(i, &auxiliary->groups[i]);
    for (u32 i = 0; i < 4; ++i) voice->setRemoteMix(i, &mix.outputs[i]);
}

/* WWHD glue
 ext 0289CCCC f64 wavePitchRatio(s32)
 ext 0289CDBC f64 waveDecibelAmplitude(f64)
 ext 028EFFD0 void constructUpdateMix(void*,u32,u32,u32)
 ext 0289EB18 void nw::snd::PlaybackVoice::setVolume(f64)
 ext 0289EB44 void nw::snd::PlaybackVoice::setPitch(f64)
 ext 0289EB64 void nw::snd::PlaybackVoice::setPanMode(u32)
 ext 0289EB84 void nw::snd::PlaybackVoice::setPanCurve(u32)
 ext 0289ED28 void nw::snd::PlaybackVoice::setFilterPreset(u32)
 ext 0289EBA4 void nw::snd::PlaybackVoice::setLowPass(f64)
 ext 0289EBC4 void nw::snd::PlaybackVoice::setFilter(u32,f64)
 ext 0289ED6C void nw::snd::PlaybackVoice::setOutputMask(u32)
 ext 0289EEE0 void nw::snd::PlaybackVoice::setMainMix(const nw::snd::ChannelMixGroup*)
 ext 0289EEE8 void nw::snd::PlaybackVoice::setGamepadMix(u32,const nw::snd::ChannelMixGroup*)
 ext 0289EEFC void nw::snd::PlaybackVoice::setRemoteMix(u32,const nw::snd::ChannelOutputGain*)
*/

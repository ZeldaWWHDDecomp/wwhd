/* Wave-channel sample and ADPCM preparation. From WWHD's instructions only. */
#include "nw/snd/WaveChannel.h"
#include "nw/snd/AdpcmDecode.h"
using namespace nw::snd;
struct WaveAdpcmCoefficients { be<s16> values[16]; };
WWHD_SIZE(WaveAdpcmCoefficients, 32);
void advanceWaveAdpcmState(WaveAdpcmState* state,const WaveAdpcmCoefficients* coefficients,u32 samples,const void* data);

/* 028A4704 */
void WaveChannel::prepareWave(const WaveDescription* wave,u32 sampleOffset) {
    gabi::AutoFrame frame{0x028A4704};
    gabi::FrameLocal<WaveAdpcmCoefficients> coefficients(0x10);
    looping = wave->mLooping;
    u32 loopSample = wave->mLoopStart;
    position = loopSample;
    u32 format = wave->mEncoding;
    if (sampleOffset && format == WaveFormatAdpcm) sampleOffset = (sampleOffset / AdpcmFrameSamples) * AdpcmFrameSamples;
    u32 loopByte = sampleByteOffset(loopSample,format);
    s32 count = voice->mChannelCount;
    if (count <= 0) return;
    for (u32 channel = 0; channel < (u32)count; ++channel) {
        const WaveChannelDescription* source = &wave->mChannels[channel];
        const void* originalData = source->mSamples;
        const void* initialData = originalData;
        if (sampleOffset) {
            u32 offset = sampleByteOffset(sampleOffset,wave->mEncoding);
            initialData = gabi::at<const void>(gabi::ea(originalData) + offset);
        }
        WaveAdpcmState* initial = &initialStates[channel];
        WaveAdpcmState* loop = &loopStates[channel];
        if (wave->mEncoding == WaveFormatAdpcm) {
            for (u32 i = 0; i < 16; ++i) coefficients->values[i] = source->mInitial.mCoefficients[i];
            initial->predictor = source->mInitial.mContext.predictor;
            initial->history[0] = source->mInitial.mContext.history[0];
            initial->history[1] = source->mInitial.mContext.history[1];
            if (sampleOffset) advanceWaveAdpcmState(initial,coefficients.get(),sampleOffset,originalData);
            if (wave->mLooping) {
                loop->predictor = source->mLoop.predictor;
                loop->history[0] = source->mLoop.history[0];
                loop->history[1] = source->mLoop.history[1];
            }
            voice->setAdpcmCoefficients(channel,reinterpret_cast<const be<u16>*>(coefficients->values));
        }
        WaveSampleData* first = &sampleData[channel * 2];
        first->samples = initialData;
        u32 remainingSamples = (u32)wave->mLoopEnd - sampleOffset;
        first->active = 0;
        first->sampleCount = remainingSamples;
        first->adpcm = wave->mEncoding == WaveFormatAdpcm ? initial : nullptr;
        if (wave->mLooping) {
            WaveSampleData* second = &sampleData[channel * 2 + 1];
            second->samples = gabi::at<const void>(gabi::ea(originalData) + loopByte);
            u32 loopSamples = (u32)wave->mLoopEnd - (u32)position;
            second->active = 1;
            second->sampleCount = loopSamples;
            second->adpcm = wave->mEncoding == WaveFormatAdpcm ? loop : nullptr;
            // a WaveSampleData is the first 0x20 bytes of a VoiceRequest (see WaveChannel.h)
            voice->appendRequest(channel,reinterpret_cast<VoiceRequest*>(first),0);
            voice->appendRequest(channel,reinterpret_cast<VoiceRequest*>(second),1);
        } else {
            voice->appendRequest(channel,reinterpret_cast<VoiceRequest*>(first),1);
        }
    }
}

/* 028A4984 */
void WaveChannel::start(const WaveDescription* wave,u32 startFrame,u32 sampleOffset) {
    gabi::AutoFrame frame{0x028A4984};
    sourceType = startFrame;
    for (u32 i = 0; i < 4; ++i) modulators[i].reset();
    envelope.resetLevel(-90.40000152587890625);
    PlaybackVoice* current = voice;
    releaseElapsed = 0;
    current->setSampleFormat(wave->mEncoding);
    u32 sampleRate = wave->mSampleRate;
    voice->setSampleRate(sampleRate);
    voice->setInterpolationMode(outputMode);
    prepareWave(wave,sampleOffset);
    voice->requestStart();
    started = 1;
}

/* WWHD glue
 ext 0289EFAC u32 nw::snd::sampleByteOffset(u32,u32)
 ext 0289EFFC void advanceWaveAdpcmState(nw::snd::WaveAdpcmState*,const WaveAdpcmCoefficients*,u32,const void*)
 ext 0289F1D0 void nw::snd::PlaybackVoice::setAdpcmCoefficients(u32,const be<u16>*)
 ext 0289F174 void nw::snd::PlaybackVoice::appendRequest(u32,nw::snd::VoiceRequest*,u32)
 ext 0289EAB0 void nw::snd::PlaybackVoice::setSampleFormat(u32)
 ext 0289EAE8 void nw::snd::PlaybackVoice::setSampleRate(u32)
 ext 0289ECF8 void nw::snd::PlaybackVoice::setInterpolationMode(u32)
 ext 0289D624 void nw::snd::PlaybackVoice::requestStart()
*/

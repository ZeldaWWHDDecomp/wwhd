/* ADPCM seek history, sample byte offsets, and the per-channel request/routing setters of the
 * playback voice, reconstructed from WWHD. */
#include "nw/snd/AdpcmDecode.h"
#include "nw/snd/PlaybackVoice.h"
using namespace nw::snd;
void AXDecodeAdpcmData(const be<u8>* data, AdpcmDecodeParameters* parameters, u32 samples, be<s16>* output);

/* 0289EF7C */
u32 nw::snd::adpcmNibbleOffset(u32 samples) {
    // 14 samples per 8-byte frame; the first two nibbles of a frame are its header.
    const u32 frames = samples / 14u;
    const u32 remainder = samples - frames * 14u;
    return frames * 16u + (remainder ? remainder + 2u : 0u);
}

/* 0289EFAC */
u32 nw::snd::sampleByteOffset(u32 samples, u32 format) {
    gabi::AutoFrame frame{0x0289EFACu};
    if (format == SamplePcm8) return samples;
    if (format == SamplePcm16) return samples * 2u;
    if (format == SampleAdpcm) return adpcmNibbleOffset(samples) >> 1;
    return 0;
}

/* 0289EFFC */
void nw::snd::advanceAdpcmLoop(AdpcmLoop* loop, const be<s16>* coefficients, u32 samples, const be<u8>* data) {
    gabi::AutoFrame frame{0x0289EFFCu};
    gabi::FrameLocal<AdpcmDecodeParameters> parameters{8};
    parameters->samplesPerFrame = 0;
    parameters->nibblesPerFrame = 0;
    for (u32 index = 0; index < 2; ++index) parameters->reserved[index] = 0;
    parameters->firstNibble = 0;
    parameters->lastNibble = 0;
    parameters->currentNibble = 0;
    for (u32 index = 0; index < 16; ++index) parameters->coefficients[index] = 0;
    parameters->reservedHalfword = 0;
    parameters->predictor = 0;
    parameters->history[0] = 0;
    parameters->history[1] = 0;
    for (u32 index = 0; index < 7; ++index) parameters->tail[index] = 0;
    for (u32 index = 0; index < 16; ++index) parameters->coefficients[index] = coefficients[index];
    parameters->history[0] = loop->history[0];
    const u16 initialPredictor = loop->predictor;
    parameters->predictor = initialPredictor;
    gabi::FrameLocal<be<s16>[14]> output{0x68};
    for (u32 sample = 0; sample < 14; ++sample) (*output)[sample] = 0;
    parameters->firstNibble = 2;
    parameters->currentNibble = 2;
    parameters->samplesPerFrame = 14;
    parameters->nibblesPerFrame = 16;
    parameters->lastNibble = 15;
    parameters->history[1] = loop->history[1];
    if (!samples) {
        loop->history[1] = (*output)[12];
        loop->predictor = initialPredictor;
        loop->history[0] = (*output)[13];
        return;
    }
    // Decode frame by frame: the decoder always gets the start of the data and the nibble
    // cursors select the frame; carry the next frame header's predictor and the last two samples.
    const be<u8>* nextFrame = data;
    u32 decoded = 0;
    u16 predictor = initialPredictor;
    s16 last = 0, beforeLast = 0;
    do {
        AXDecodeAdpcmData(data, parameters.get(), 14, *output);
        nextFrame = gabi::at<const be<u8>>(gabi::ea(nextFrame) + 8);
        predictor = *nextFrame;
        const u32 firstNibble = parameters->firstNibble;
        parameters->predictor = predictor;
        last = (*output)[13];
        const u32 currentNibble = parameters->currentNibble;
        beforeLast = (*output)[12];
        parameters->history[0] = last;
        decoded += 14;
        const u32 lastNibble = parameters->lastNibble;
        parameters->history[1] = beforeLast;
        parameters->currentNibble = currentNibble + 16;
        parameters->firstNibble = firstNibble + 16;
        parameters->lastNibble = lastNibble + 16;
    } while (decoded < samples);
    loop->history[0] = last;
    loop->predictor = predictor;
    loop->history[1] = beforeLast;
}

/* 0289F174 */
void PlaybackVoice::appendRequest(u32 channel, VoiceRequest* request, u32 makeCurrent) {
    gabi::AutoFrame frame{0x0289F174u};
    guestElement(mChannels, channel)->appendRequest(request);
    if (makeCurrent) mCurrentRequest = request;
}

/* 0289F1D0 */
void PlaybackVoice::setAdpcmCoefficients(u32 channel, const be<u16>* coefficients) {
    SoundVoiceHandle* handle = guestElement(mChannels, channel);
    for (u32 index = 0; index < 16; ++index) handle->adpcmCoefficients[index] = coefficients[index];
}

/* WWHD glue
ext C0006C98 void AXDecodeAdpcmData(const be<u8>*,nw::snd::AdpcmDecodeParameters*,u32,be<s16>*)
ext 028A60EC void nw::snd::SoundVoiceHandle::appendRequest(nw::snd::VoiceRequest*)
*/

/* ADPCM/PCM decoding and waveform generation, reconstructed from WWHD.
 * Tables are guest image views; no table or sample assets are copied here. */
#include "JSystem/JAudio/JASBridgeDecoder.h"
static void* decoderHeap;
static JASBridge::AdpcmPredictors adpcmPredictors;
static JASBridge::DecoderWaveTable sineWave;
static JASBridge::DecoderWaveTable triangleWave;
static JASBridge::DecoderWaveTable initialCoupledNoise;
static JASBridge::DecoderWaveTable initialRampNoise;
static JASBridge::DecoderStatics decoderStatics;
static JASBridge::DecoderDestructorRecord decoderDestructorRecord;
/* 0281C504 */
JASBridge::SampleDecoder* JASBridge::constructSampleDecoder(SampleDecoder* object) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (!object) object = static_cast<SampleDecoder*>(allocateSampleDecoder(sizeof(SampleDecoder)));
    if (object) {
        object->feedback = nullptr;
        object->remainingOutput = 0;
        object->unpackedFrame = nullptr;
        object->output = nullptr;
        object->unpackedFrame = static_cast<be<s16>*>(allocateDecoderStorage(32, decoderHeap, 0x40));
        object->coupledNoise = static_cast<be<s16>*>(allocateDecoderStorage(128, decoderHeap, 0x40));
        object->rampNoise = static_cast<be<s16>*>(allocateDecoderStorage(128, decoderHeap, 0x40));
        copyDecoderStorage(object->coupledNoise, initialCoupledNoise.samples, 128);
        copyDecoderStorage(object->rampNoise, initialRampNoise.samples, 128);
    }
    return object;
}
/* 0281C5D4 */
u32 JASBridge::SampleDecoder::outputCount(DSPBuffer_l* channel) {
    return ((u32)channel->pitch * 80 + (u32)channel->phase) >> 12;
}
/* 0281C5EC */
void JASBridge::SampleDecoder::prepareAdpcmFrame(DSPBuffer_l* channel) {
    u32 position = channel->samplePosition;
    u32 end = channel->sampleEnd;
    u32 block = position >> 4;
    u32 stride = channel->format;
    channel->frameIndex = block;
    u32 byteOffset = block * stride;
    be<u8>* source = channel->source;
    channel->samplesLeft = end - position;
    channel->blockOffset = position & 15;
    channel->frameData = source + byteOffset;
}
/* 0281C624 */
void JASBridge::SampleDecoder::stopAdpcm(DSPBuffer_l* channel) {
    for (u32 sample = 0; sample < 16; ++sample) channel->decodedBlock[sample] = 0;
    channel->stopped = 1;
    channel->blockSize = 16;
    channel->blockOffset = 0;
}
/* 0281C658 */
u32 JASBridge::SampleDecoder::parseAdpcmHeader(u32 header, be<u32>* shift, be<u32>* predictor) {
    u32 exponent = (header >> 4) & 15;
    *predictor = header & 15;
    if (exponent > 12) return 0;
    *shift = 12 - exponent;
    return 1;
}
/* 0281C684 */
void JASBridge::SampleDecoder::clearSamples(be<s16>* destination, s32 count) {
    for (s32 sample = 0; sample < count; ++sample) destination[sample] = 0;
}
/* 0281C6A4 */
void JASBridge::SampleDecoder::unpackTwoBit(be<s16>* destination, be<u8>* source) {
    for (u32 group = 0; group < 4; ++group) {
        u32 packed = source[group];
        destination[group * 4] = (packed << 8) & 0xC000;
        destination[group * 4 + 2] = (packed << 12) & 0xC000;
        destination[group * 4 + 1] = (packed << 10) & 0xC000;
        destination[group * 4 + 3] = (packed << 14) & 0xC000;
    }
}
/* 0281C6E0 */
void JASBridge::SampleDecoder::unpackFourBit(be<s16>* destination, be<u8>* source) {
    for (u32 group = 0; group < 8; ++group) {
        u32 packed = source[group];
        destination[group * 2] = (packed << 8) & 0xF000;
        destination[group * 2 + 1] = (packed << 12) & 0xF000;
    }
}
/* 0281C70C */
void JASBridge::SampleDecoder::predictAdpcm(be<s16>* destination, be<s16>* source, s32 first, s32 second,
                                         s32 previous, s32 older, u32 shift) {
    for (u32 sample = 0; sample < 16; ++sample) {
        s32 prediction = (s32)((u32)first * (u32)previous + (u32)second * (u32)older);
        s32 residual = source[sample];
        u32 amount = shift & 63;
        residual = amount >= 32 ? (residual < 0 ? -1 : 0) : residual >> amount;
        s32 value = (s32)((u32)(prediction >> 11) + (u32)residual);
        older = previous;
        previous = value;
        destination[sample] = value;
    }
}
/* 0281C74C */
void JASBridge::SampleDecoder::decodeAdpcmFrame(DSPBuffer_l* channel, be<s16>* destination, be<u8>* encoded,
                                              s32 previous, s32 older, u32 format, be<s16>* unpacked) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 error = channel->decodeError;
    u32 header = encoded[0];
    gabi::FrameLocal<AdpcmScratch> scratch(8);
    scratch->predictor = 0;
    scratch->shift = 0;
    if (error || !parseAdpcmHeader(header, &scratch->shift, &scratch->predictor)) {
        clearSamples(destination, 16);
        return;
    }
    if (format == 5) unpackTwoBit(unpacked, encoded + 1);
    else if (format == 9) unpackFourBit(unpacked, encoded + 1);
    u32 index = scratch->predictor;
    u32 shift = scratch->shift;
    s32 first = adpcmPredictors.values[index * 2];
    s32 second = adpcmPredictors.values[index * 2 + 1];
    predictAdpcm(destination, unpacked, first, second, previous, older, shift);
}
/* 0281C880 */
void JASBridge::SampleDecoder::decodeChannelFrame(DSPBuffer_l* channel) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    be<u8>* encoded = channel->frameData;
    s32 older = channel->decodedBlock[14];
    be<s16>* unpacked = unpackedFrame;
    u32 format = channel->format;
    s32 previous = channel->decodedBlock[15];
    decodeAdpcmFrame(channel, channel->decodedBlock, encoded, previous, older, format, unpacked);
}
/* 0281C89C */
void JASBridge::SampleDecoder::advanceAdpcmFrame(DSPBuffer_l* channel) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (channel->stopped) { stopAdpcm(channel); return; }
    u32 end = channel->sampleEnd;
    u32 frame = channel->frameIndex;
    if ((end >> 4) != frame) {
        decodeChannelFrame(channel);
        channel->blockOffset = 0;
        channel->blockSize = 16;
        return;
    }
    u32 position = channel->samplePosition;
    if (position < end) {
        decodeChannelFrame(channel);
        u32 left = channel->samplesLeft;
        channel->blockOffset = 0;
        channel->blockSize = left;
        return;
    }
    if (!channel->loopEnabled) { stopAdpcm(channel); return; }
    u32 start = channel->loopStart;
    channel->blockSize = 16;
    channel->samplePosition = start;
    prepareAdpcmFrame(channel);
    s32 previous = channel->loopPrevious;
    s32 older = channel->loopOlder;
    channel->decodedBlock[15] = previous;
    channel->decodedBlock[14] = older;
    decodeChannelFrame(channel);
}
/* 0281C9AC */
void JASBridge::SampleDecoder::copyDecodedSamples(be<s16>* source, s32 count) {
    if (count <= 0) return;
    be<s16>* destination = output;
    for (s32 sample = 0; sample < count; ++sample) {
        destination[sample] = source[sample];
        output = destination + sample + 1;
    }
}
/* 0281C9E0 */
void JASBridge::SampleDecoder::appendDecodedSamples(DSPBuffer_l* channel, u32 limit) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 offset = channel->blockOffset;
    u32 count = (u32)channel->blockSize - offset;
    if (limit) {
        u32 available = remainingOutput;
        if (count >= available) count = available;
    }
    copyDecodedSamples(channel->decodedBlock + offset, count);
    channel->samplePosition = (u32)channel->samplePosition + count;
    remainingOutput = (u32)remainingOutput - count;
}
/* 0281CA4C */
void JASBridge::SampleDecoder::appendDecodedLimited(DSPBuffer_l* channel) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    appendDecodedSamples(channel, 1);
}
/* 0281CA54 */
void JASBridge::SampleDecoder::appendDecodedFull(DSPBuffer_l* channel) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    appendDecodedSamples(channel, 0);
}
/* 0281CBC8 */
void JASBridge::SampleDecoder::prepareAdpcmBlocks(DSPBuffer_l* channel) {
    u32 position = channel->samplePosition;
    u32 block = position >> 4;
    u32 stride = channel->format;
    u32 end = channel->sampleEnd;
    channel->loopPending = 0;
    be<u8>* source = channel->source;
    channel->samplesLeft = (end - position) >> 4;
    channel->blockOffset = 0;
    channel->frameData = source + block * stride;
}
/* 0281CC04 */
u32 JASBridge::SampleDecoder::handleAdpcmLoop(DSPBuffer_l* channel) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 enabled = channel->loopEnabled;
    channel->loopPending = 0;
    if (!enabled) {
        be<s16>* destination = output;
        for (u32 sample = 0; sample < (u32)remainingOutput; ++sample) {
            destination[sample] = 0;
            output = destination + sample + 1;
        }
        channel->stopped = 1;
        return 1;
    }
    channel->samplePosition = (u32)channel->loopStart;
    prepareAdpcmBlocks(channel);
    s32 previous = channel->loopPrevious;
    s32 older = channel->loopOlder;
    channel->decodedBlock[15] = previous;
    channel->decodedBlock[14] = older;
    return 0;
}
/* 0281CCA4 */
void JASBridge::SampleDecoder::fillEndSamples(DSPBuffer_l* channel) {
    be<s16>* destination = output;
    for (u32 sample = 0; sample < (u32)remainingOutput; ++sample) {
        destination[sample] = 0;
        output = destination + sample + 1;
    }
    channel->stopped = 1;
}
/* 0281CCE4 */
void JASBridge::SampleDecoder::restartAdpcmBlocks(DSPBuffer_l* channel) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    channel->decodedBlock[14] = 0;
    channel->decodedBlock[15] = 0;
    channel->samplePosition = 0;
    prepareAdpcmBlocks(channel);
}
/* 0281CCF8 */
void JASBridge::SampleDecoder::decodeAdpcmDirectBlock(DSPBuffer_l* channel) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    s32 older = channel->decodedBlock[14];
    be<s16>* destination = output;
    u32 format = channel->format;
    s32 previous = channel->decodedBlock[15];
    be<s16>* unpacked = unpackedFrame;
    be<u8>* encoded = channel->frameData;
    decodeAdpcmFrame(channel, destination, encoded, previous, older, format, unpacked);
    channel->decodedBlock[14] = destination[14];
    channel->decodedBlock[15] = destination[15];
    output = output.get() + 16;
    channel->frameData = channel->frameData.get() + format;
}
/* 0281D010 */
void JASBridge::SampleDecoder::fillStoppedSamples(DSPBuffer_l* channel) {
    be<s16>* destination = output;
    for (u32 sample = 0; sample < (u32)remainingOutput; ++sample) {
        destination[sample] = 0;
        output = destination + sample + 1;
    }
    channel->stopped = 1;
}
/* 0281D050 */
void JASBridge::SampleDecoder::preparePcm(DSPBuffer_l* channel) {
    u32 stride = (u32)channel->format >> 3;
    u32 position = channel->samplePosition;
    u32 end = channel->sampleEnd;
    be<u8>* source = channel->source;
    channel->samplesLeft = end - position;
    channel->loopPending = 0;
    channel->frameData = source + position * stride;
}
/* 0281D084 */
void JASBridge::SampleDecoder::restartPcm(DSPBuffer_l* channel) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    channel->samplePosition = 0;
    preparePcm(channel);
}
/* 0281D090 */
void JASBridge::SampleDecoder::copyPcm16(DSPBuffer_l* channel, be<s16>* source, s32 count) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (channel->decodeError) {
        clearSamples(output, count);
        output = output.get() + count;
        return;
    }
    if (count <= 0) return;
    be<s16>* destination = output;
    for (s32 sample = 0; sample < count; ++sample) {
        destination[sample] = source[sample];
        output = destination + sample + 1;
    }
}
/* 0281D250 */
void JASBridge::SampleDecoder::copyPcm8(DSPBuffer_l* channel, be<u8>* source, s32 count) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (channel->decodeError) {
        clearSamples(output, count);
        output = output.get() + count;
        return;
    }
    if (count <= 0) return;
    be<s16>* destination = output;
    for (s32 sample = 0; sample < count; ++sample) {
        u32 packed = source[sample];
        destination[sample] = packed << 8;
        output = destination + sample + 1;
    }
}
/* 0281D410 */
void JASBridge::SampleDecoder::copyStream(DSPBuffer_l* channel, u32 count) {
    if (!count) return;
    u32 position = channel->samplePosition;
    be<u8>* source = channel->source;
    be<s16>* input = reinterpret_cast<be<s16>*>(source + position * 2 - ((position & 1) * 2));
    /* The assigned instructions use position as the copy count, independently
     * of count, which advances the stored position afterward. */
    if (position) {
        be<s16>* destination = output;
        for (u32 sample = 0; sample < position; ++sample) {
            destination[sample] = input[sample];
            output = destination + sample + 1;
        }
        position = channel->samplePosition;
    }
    channel->samplePosition = position + count;
}
/* 0281CA5C */
void JASBridge::SampleDecoder::processAdpcmSlow(DSPBuffer_l* channel) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    for (;;) {
        prepareAdpcmFrame(channel);
        u32 available = channel->samplesLeft;
        u32 offset = channel->blockOffset;
        if (!available) {
            advanceAdpcmFrame(channel);
            offset = channel->blockOffset;
            if (!offset) {
                appendDecodedLimited(channel);
                if (!remainingOutput) return;
                prepareAdpcmFrame(channel);
                offset = channel->blockOffset;
            }
            available = channel->samplesLeft;
        }
        if (available >= (u32)remainingOutput) {
            for (;;) {
                if (!offset) advanceAdpcmFrame(channel);
                appendDecodedLimited(channel);
                if (!remainingOutput) return;
                prepareAdpcmFrame(channel);
                offset = channel->blockOffset;
            }
        }
        if (!offset) {
            advanceAdpcmFrame(channel);
            if (channel->stopped) {
                be<s16>* destination = output;
                for (u32 sample = 0; sample < (u32)remainingOutput; ++sample) {
                    destination[sample] = 0;
                    output = destination + sample + 1;
                }
                return;
            }
        }
        appendDecodedFull(channel);
    }
}
/* 0281CD88 */
void JASBridge::SampleDecoder::processAdpcmBlocks(DSPBuffer_l* channel) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (channel->stopped) { fillEndSamples(channel); return; }
    if (channel->reset) restartAdpcmBlocks(channel);
    u32 offset = channel->blockOffset;
    if (offset) {
        u32 count = (16 - offset) & 0xFFFF;
        copyDecodedSamples(channel->decodedBlock + offset, count);
        u32 remaining = remainingOutput;
        if (remaining < count) { channel->blockOffset = offset + remaining; return; }
        if (remaining == count) { channel->blockOffset = 0; return; }
        remainingOutput = remaining - count;
    }
    if (channel->loopPending && handleAdpcmLoop(channel)) return;
    for (;;) {
        decodeAdpcmDirectBlock(channel);
        u32 available = (u32)channel->samplesLeft - 1;
        if (!available) channel->loopPending = 1;
        channel->samplesLeft = available;
        u32 remaining = remainingOutput;
        if (remaining <= 16) {
            if (remaining == 16) { remaining = 0; remainingOutput = 0; }
            channel->blockOffset = remaining;
            for (u32 sample = 0; sample < 16; ++sample) {
                be<s16>* destination = output;
                channel->decodedBlock[sample] = destination[(s32)sample - 16];
            }
            return;
        }
        remainingOutput = remaining - 16;
        if (channel->loopPending && handleAdpcmLoop(channel)) return;
    }
}
/* 0281CF98 */
void JASBridge::SampleDecoder::renderAdpcm(DSPBuffer_l* channel, be<s16>* destination) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 count = outputCount(channel);
    output = destination;
    remainingOutput = count;
    if (channel->format == 5) { processAdpcmSlow(channel); return; }
    if (channel->loopEnabled) {
        u32 start = channel->loopStart;
        u32 end = channel->sampleEnd;
        if ((start | end) & 15) { processAdpcmSlow(channel); return; }
    }
    processAdpcmBlocks(channel);
}
/* 0281D10C */
void JASBridge::SampleDecoder::renderPcm16(DSPBuffer_l* channel, be<s16>* destination) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 count = outputCount(channel);
    output = destination;
    remainingOutput = count;
    if (channel->stopped) { fillStoppedSamples(channel); return; }
    if (channel->reset) restartPcm(channel);
    for (;;) {
        if (channel->loopPending) {
            u32 enabled = channel->loopEnabled;
            channel->loopPending = 0;
            if (!enabled) { fillStoppedSamples(channel); return; }
            channel->samplePosition = (u32)channel->loopStart;
            preparePcm(channel);
        }
        u32 needed = remainingOutput;
        u32 available = channel->samplesLeft;
        be<u8>* source = channel->frameData;
        if (available >= needed) {
            copyPcm16(channel, reinterpret_cast<be<s16>*>(source), needed);
            needed = remainingOutput;
            available -= needed;
            source += needed * 2;
            if (!available) channel->loopPending = 1;
            channel->samplesLeft = available;
            channel->frameData = source;
            return;
        }
        copyPcm16(channel, reinterpret_cast<be<s16>*>(source), available);
        remainingOutput = (u32)remainingOutput - available;
        channel->frameData = source + available * 2;
        channel->loopPending = 1;
    }
}
/* 0281D2D4 */
void JASBridge::SampleDecoder::renderPcm8(DSPBuffer_l* channel, be<s16>* destination) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 count = outputCount(channel);
    output = destination;
    remainingOutput = count;
    if (channel->stopped) { fillStoppedSamples(channel); return; }
    if (channel->reset) restartPcm(channel);
    for (;;) {
        if (channel->loopPending) {
            u32 enabled = channel->loopEnabled;
            channel->loopPending = 0;
            if (!enabled) { fillStoppedSamples(channel); return; }
            channel->samplePosition = (u32)channel->loopStart;
            preparePcm(channel);
        }
        u32 needed = remainingOutput;
        u32 available = channel->samplesLeft;
        be<u8>* source = channel->frameData;
        if (available >= needed) {
            copyPcm8(channel, source, needed);
            needed = remainingOutput;
            available -= needed;
            source += needed;
            if (!available) channel->loopPending = 1;
            channel->samplesLeft = available;
            channel->frameData = source;
            return;
        }
        copyPcm8(channel, source, available);
        remainingOutput = (u32)remainingOutput - available;
        channel->frameData = source + available;
        channel->loopPending = 1;
    }
}
/* 0281D468 */
void JASBridge::SampleDecoder::renderStream(DSPBuffer_l* channel) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 available = channel->samplesLeft;
    u32 needed = remainingOutput;
    if (available < needed) {
        copyStream(channel, available);
        u32 tail = (u32)remainingOutput - available;
        be<s16>* destination = output;
        s16 last = destination[-1];
        for (u32 sample = 0; sample < tail; ++sample) {
            destination[sample] = last;
            output = destination + sample + 1;
        }
        channel->samplesLeft = 0;
        channel->stopped = 1;
        return;
    }
    u32 end = channel->sampleEnd;
    u32 position = channel->samplePosition;
    channel->samplesLeft = needed - available;
    u32 span = end - position;
    channel->frameIndex = span;
    needed = remainingOutput;
    if (span >= needed) { copyStream(channel, needed); return; }
    copyStream(channel, span);
    u32 tail = (u32)remainingOutput - span;
    be<u8>* loopSource = channel->loopSource;
    channel->samplePosition = 0;
    channel->source = loopSource;
    copyStream(channel, tail);
}
/* 0281D538 */
void JASBridge::SampleDecoder::renderResampledStream(DSPBuffer_l* channel, be<s16>* destination) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    remainingOutput = outputCount(channel);
    output = destination;
    renderStream(channel);
}
/* 0281D56C */
void JASBridge::SampleDecoder::renderDirectStream(DSPBuffer_l* channel, be<s16>* destination) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    output = destination;
    remainingOutput = 80;
    renderStream(channel);
}
/* 0281D57C */
u32 JASBridge::SampleDecoder::square(u32 phase, u32 step) {
    be<s16>* destination = output;
    phase <<= 1;
    for (u32 sample = 0; sample < 80; ++sample) {
        destination[sample] = phase & 0x10000 ? 0x4000 : -0x4000;
        phase += step;
        output = destination + sample + 1;
    }
    return (phase >> 1) & 0xFFFF;
}
/* 0281D5BC */
u32 JASBridge::SampleDecoder::saw(u32 phase, u32 step) {
    be<s16>* destination = output;
    step >>= 1;
    for (u32 sample = 0; sample < 80; ++sample) {
        destination[sample] = phase;
        phase += step;
        output = destination + sample + 1;
    }
    return phase & 0xFFFF;
}
/* 0281D5EC */
u32 JASBridge::SampleDecoder::modulatedSquare(u32 phase, u32 step) {
    be<u16>* modulation = feedback;
    phase <<= 1;
    for (u32 sample = 0; sample < 80; ++sample) {
        be<s16>* destination = output;
        s32 value = phase & 0x10000 ? 0x4000 : -0x4000;
        destination[0] = value;
        output = destination + 1;
        u32 increment = (u32)modulation[sample] + step;
        modulation[sample] = 0;
        phase += increment & 0xFFFF;
    }
    return (phase >> 1) & 0xFFFF;
}
/* 0281D644 */
u32 JASBridge::SampleDecoder::widePulse(u32 phase, u32 step) {
    be<s16>* destination = output;
    phase <<= 2;
    step <<= 1;
    for (u32 sample = 0; sample < 80; ++sample) {
        destination[sample] = phase & 0x30000 ? 0x4000 : -0x4000;
        phase += step;
        output = destination + sample + 1;
    }
    return (phase >> 2) & 0xFFFF;
}
/* 0281D684 */
u32 JASBridge::SampleDecoder::wave(u32 phase, u32 step, be<s16>* table) {
    be<s16>* destination = output;
    phase <<= 6;
    step <<= 5;
    for (u32 sample = 0; sample < 80; ++sample) {
        destination[sample] = table[(phase >> 16) & 63];
        phase += step;
        output = destination + sample + 1;
    }
    return (phase >> 6) & 0xFFFF;
}
/* 0281D6C0 */
u32 JASBridge::SampleDecoder::triangle(u32 phase, u32 step) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    return wave(phase, step, triangleWave.samples);
}
/* 0281D6CC */
u32 JASBridge::SampleDecoder::fillConstant(u32 phase, u32 value) {
    be<s16>* destination = output;
    for (u32 sample = 0; sample < 80; ++sample) {
        destination[sample] = value;
        output = destination + sample + 1;
    }
    return phase;
}
/* 0281D6F4 */
u32 JASBridge::SampleDecoder::sine(u32 phase, u32 step) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    return wave(phase, step, sineWave.samples);
}
/* 0281D700 */
u32 JASBridge::SampleDecoder::modulatedSine(u32 phase, u32 step) {
    be<u16>* modulation = feedback;
    be<s16>* destination = output;
    phase <<= 6;
    u32 increment = step << 5;
    for (u32 sample = 0; sample < 80; ++sample) {
        destination[sample] = sineWave.samples[(phase >> 16) & 63];
        output = destination + sample + 1;
        u32 factor = modulation[sample];
        phase = ((phase << 10) + step * factor) >> 10;
        phase += increment;
    }
    return (phase >> 6) & 0xFFFF;
}
/* 0281D760 */
u32 JASBridge::SampleDecoder::coupledNoiseWave(u32 phase, u32 step) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    return wave(phase, step, coupledNoise);
}
/* 0281D768 */
u32 JASBridge::SampleDecoder::rampNoiseWave(u32 phase, u32 step) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    return wave(phase, step, rampNoise);
}
/* 0281D770 */
void JASBridge::SampleDecoder::synthesize(DSPBuffer_l* channel, be<s16>* destination) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    output = destination;
    u32 format = channel->format;
    u32 phase = channel->phase;
    u32 step = channel->pitch;
    switch (format) {
    case 0: phase = square(phase, step); break;
    case 1: phase = saw(phase, step); break;
    case 2: phase = modulatedSquare(phase, step); break;
    case 3: phase = widePulse(phase, step); break;
    case 4: phase = triangle(phase, step); break;
    case 6: phase = fillConstant(phase, step); break;
    case 7: phase = sine(phase, step); break;
    case 10: phase = modulatedSine(phase, step); break;
    case 11: phase = coupledNoiseWave(phase, step); break;
    case 12: phase = rampNoiseWave(phase, step); break;
    default: break;
    }
    channel->phase = phase;
}
/* 0281D874 */
void JASBridge::SampleDecoder::updateNoise() {
    be<s16>* first = coupledNoise;
    s32 previous = first[63];
    s32 coefficient = first[62];
    for (u32 pair = 0; pair < 32; ++pair) {
        s32 product = (s32)((u32)previous * (u32)coefficient);
        s32 input = first[pair * 2];
        coefficient = (s32)((u32)(product >> 16) - (u32)input);
        product = (s32)((u32)coefficient * (u32)previous);
        first[pair * 2] = coefficient;
        input = first[pair * 2 + 1];
        previous = (s32)((u32)(product >> 16) + (u32)input);
        first[pair * 2 + 1] = previous;
    }
    be<s16>* second = rampNoise;
    s32 noiseCoefficient = second[62];
    s32 noisePrevious = second[63];
    s32 product = (s32)((u32)noisePrevious * (u32)noiseCoefficient);
    s32 value = (s32)((u32)(product >> 16) + (u32)noiseCoefficient);
    s32 increment = ((u32)value & 0x1FF) | 0x2000;
    if (noisePrevious > 0) increment = -increment;
    for (u32 sample = 0; sample < 64; ++sample) {
        noisePrevious = (s32)((u32)noisePrevious + (u32)increment);
        second[sample] = noisePrevious;
    }
}
/* 0281D904 */
void JASBridge::initializeSampleDecoderStatics() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    decoderStatics.words[2] = 0;
    decoderStatics.words[0] = 0;
    decoderStatics.words[3] = 0;
    decoderStatics.words[1] = 0;
    registerSampleDecoderStatics(&decoderDestructorRecord);
}
/* WWHD glue
data decoderHeap 101F9DA8
data adpcmPredictors 10170AB0
data sineWave 10170AF0
data triangleWave 10170B70
data initialCoupledNoise 10170BF0
data initialRampNoise 10170C70
data decoderStatics 104B5710
data decoderDestructorRecord 101FA040
ext 0273AD10 void* allocateSampleDecoder(u32)
ext 0273B0D4 void* allocateDecoderStorage(u32,void*,s32)
ext 028F026C void registerSampleDecoderStatics(JASBridge::DecoderDestructorRecord*)
ext C000A848 void copyDecoderStorage(void*,const void*,u32)
*/

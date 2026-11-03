/* Command decoder layouts. Names of undeclared driver operations are descriptive and inferred. */
#pragma once
#include "nw/snd/DriverCommandQueue.h"
#include "nw/snd/AudioMixRuntime.h"
namespace nw { namespace snd {
struct DriverOutputWords {
    be<u32> words[20];
};
struct DriverRemoteWords {
    be<u32> words[3];
};
struct DriverPlayerVtable {
    VtableEntry header, destroy, initialize, finalize, start, stop, pause;
};
struct DriverPlayerParameters {
    DriverSoundPlayer common;
    u8 scalarPadding[3];
    be<u32> volumeBits, pitchBits, lpfBits;
    u8 filterState[5];
    be<u8> drcBypass, frontBypass;
    u8 filterPadding;
    be<u32> panCurve, remoteFilter, outputLine, panMode;
    DriverOutputWords outputs[3];
    DriverRemoteWords remotes[4];
    u8 trailingState[4];
    gptr<DriverPlayerVtable> vtable;
};
WWHD_OFFSET(DriverPlayerParameters, volumeBits, 0x2C);
WWHD_OFFSET(DriverPlayerParameters, drcBypass, 0x3D);
WWHD_OFFSET(DriverPlayerParameters, panCurve, 0x40);
WWHD_OFFSET(DriverPlayerParameters, outputs, 0x50);
WWHD_OFFSET(DriverPlayerParameters, remotes, 0x140);
WWHD_OFFSET(DriverPlayerParameters, vtable, 0x174);
WWHD_SIZE(DriverPlayerParameters, 0x178);
struct TargetDriverCommand : DriverCommand {
    gptr<void> target;
};
struct WordDriverCommand : TargetDriverCommand {
    be<u32> value;
};
struct PairDriverCommand : WordDriverCommand {
    be<u32> extra;
};
struct TripleDriverCommand : PairDriverCommand {
    be<u32> duration;
};
struct ByteDriverCommand : TargetDriverCommand {
    be<u8> value, extra;
    u8 padding[2];
};
struct TrackByteCommand : WordDriverCommand {
    be<u8> value;
    u8 padding[3];
    be<u32> duration;
};
struct TrackFloatCommand : WordDriverCommand {
    be<f32> value;
    be<u32> duration, output;
};
struct TrackWordsCommand : WordDriverCommand {
    be<u32> extra, duration, output;
};
struct VariableDriverCommand : WordDriverCommand {
    gptr<void> variable;
    be<u16> reserved;
    be<s16> value;
};
struct PlayerParametersCommand : DriverCommand {
    gptr<DriverPlayerParameters> player;
    be<u32> volumeBits, pitchBits, lpfBits;
    be<u32> biquadType;
    be<f32> biquadValue;
    be<u32> outputLine;
    DriverOutputWords outputs[3];
    DriverRemoteWords remotes[4];
};
struct PrepareDriverCommand : TargetDriverCommand {
    be<u32> descriptor[3];
    be<u8> mode, curve;
    u8 padding[2];
    be<u32> resource, resourceSize;
};
struct TwoDescriptorCommand : TargetDriverCommand {
    be<u32> descriptor[3];
    be<u32> settings[3];
};
struct DriverFloatCommand : TargetDriverCommand {
    be<f32> value;
};
struct StreamBlockCommand : ByteDriverCommand {
    u8 block[0x40];
    be<u16> count;
};
struct StreamPrepareCommand : TargetDriverCommand {
    be<u8> channel;
    u8 padding[3];
    be<u32> source, byteCount, position;
    be<u8> mode;
    u8 modePadding;
    u8 descriptor[0x62];
    be<u32> buffer, bufferSize;
    be<u8> flags;
};
struct StreamMetadataCommand : TrackWordsCommand {
    be<u32> tail[4];
    be<u32> channels;
};
struct StreamStateCommand : PairDriverCommand {
    u8 padding[3];
    be<u8> flags;
};
WWHD_OFFSET(PlayerParametersCommand, outputs, 0x2C);
WWHD_OFFSET(PlayerParametersCommand, remotes, 0x11C);
WWHD_SIZE(PlayerParametersCommand, 0x14C);
WWHD_OFFSET(VariableDriverCommand, value, 0x1E);
WWHD_OFFSET(StreamBlockCommand, count, 0x58);
WWHD_OFFSET(StreamPrepareCommand, descriptor, 0x26);
WWHD_OFFSET(StreamPrepareCommand, buffer, 0x88);
WWHD_OFFSET(StreamStateCommand, flags, 0x1F);

enum DriverDispatchCommandKind {
    DriverDispatch_Signal = 2,
    DriverDispatch_PrepareSequence = 15,
    DriverDispatch_LoadSequence = 16,
    DriverDispatch_StartSequence = 17,
    DriverDispatch_StopSequence = 18,
    DriverDispatch_SetSequenceSpeed = 19,
    DriverDispatch_SetSequenceMode = 20,
    DriverDispatch_WriteSequenceVariable = 21,
    DriverDispatch_WriteGlobalVariable = 22,
    DriverDispatch_WriteTrackVariable = 23,
    DriverDispatch_SetTrackMask = 24,
    DriverDispatch_SetTrackMute = 25,
    DriverDispatch_SetTrackVolume = 26,
    DriverDispatch_SetTrackPitch = 27,
    DriverDispatch_SetTrackPan = 28,
    DriverDispatch_SetTrackSend = 29,
    DriverDispatch_SetTrackPriority = 30,
    DriverDispatch_SetTrackTranspose = 31,
    DriverDispatch_SetTrackFlag = 32,
    DriverDispatch_SetTrackAllocation = 33,
    DriverDispatch_SetSequenceAllocation = 34,
    DriverDispatch_SetTrackLpf = 35,
    DriverDispatch_SetTrackBiquad = 36,
    DriverDispatch_SetTrackSurround = 37,
    DriverDispatch_SetTrackMain = 38,
    DriverDispatch_SetTrackAux = 39,
    DriverDispatch_SetTrackOutputVolume = 40,
    DriverDispatch_SetTrackOutputPan = 41,
    DriverDispatch_SetTrackOutputSurround = 42,
    DriverDispatch_SetTrackOutputMain = 43,
    DriverDispatch_SetTrackOutputAux = 44,
    DriverDispatch_PrepareWave = 45,
    DriverDispatch_LoadWave = 46,
    DriverDispatch_SetWaveMode = 47,
    DriverDispatch_SetWaveModes = 48,
    DriverDispatch_PrepareStream = 49,
    DriverDispatch_LoadStream = 50,
    DriverDispatch_SetStreamBlock = 51,
    DriverDispatch_SetStreamSource = 52,
    DriverDispatch_MarkFinished = 53,
    DriverDispatch_SetStreamVolume = 54,
    DriverDispatch_SetStreamMask = 55,
    DriverDispatch_SetStreamAllocation = 56,
    DriverDispatch_SetStreamPriority = 57,
    DriverDispatch_SetStreamPitch = 58,
    DriverDispatch_SetStreamChannels = 59,
    DriverDispatch_SetStreamLpf = 60,
    DriverDispatch_SetStreamBiquad = 61,
    DriverDispatch_SetStreamPan = 62,
    DriverDispatch_SetStreamSend = 63,
    DriverDispatch_SetStreamOutputVolume = 64,
    DriverDispatch_SetStreamMetadata = 65,
    DriverDispatch_SetStreamOutputPan = 66,
    DriverDispatch_SetStreamOutputSurround = 67,
    DriverDispatch_SetStreamOutputMain = 68,
    DriverDispatch_SetStreamOutputAux = 69,
    DriverDispatch_QueueTask = 70,
    DriverDispatch_CancelTask = 71,
    DriverDispatch_StartTask = 72,
    DriverDispatch_AddEffect = 73,
    DriverDispatch_RemoveEffect = 74,
    DriverDispatch_SetDriverFlags = 75,
    DriverDispatch_ControlVoice = 76,           // start, stop + release, pause, unpause a PlaybackVoice
    DriverDispatch_SetVoiceFormat = 77,         // sample format, rate, interpolation
    DriverDispatch_SetVoiceAdpcmCoefficients = 78,
};
}}

/* Decode the sound thread's immutable command list. Operation names are our own and inferred. */
#include "nw/snd/DriverCommandDispatch.h"
#include "nw/snd/PlaybackVoice.h"
#include "nw/snd/WaveSound.h"
using namespace nw::snd;

void setDriverBiquad(void*, u32, f32);
void prepareSequenceDriver(void*, const void*);
void setSequenceDriverMode(void*, u32);
void setSequenceDriverCurve(void*, u32);
void bindSequenceResource(void*, u32, u32);
void loadSequenceDriver(void*, const void*, const void*);
void startSequenceDriver(void*, const void*);
void stopSequenceDriver(void*, u32, u32);
void setSequenceDriverSpeed(void*, f32);
void writeSequenceVariable(void*, void*, s32);
void writeGlobalSequenceVariable(void*, s32);
void* findSequenceTrack(void*, u32);
void writeTrackVariable(void*, void*, s32);
void setSequenceTrackMask(void*, u32, u32);
void setSequenceTrackMute(void*, u32, u32, u32);
void setSequenceTrackVolume(void*, u32, f32);
void setSequenceTrackPitch(void*, u32, f32);
void setSequenceTrackPan(void*, u32, f32);
void setSequenceTrackSend(void*, u32, u32, f32);
void setSequenceTrackPriority(void*, u32, u32);
void setSequenceTrackTranspose(void*, u32, s32);
void setSequenceTrackFlag(void*, u32, u32);
void setSequenceTrackAllocation(void*, u32, u32);
void setSequenceAllocation(void*, u32);
void setSequenceTrackLpf(void*, u32, f32);
void setSequenceTrackBiquad(void*, u32, f32);
void setSequenceTrackSurround(void*, u32, f32);
void setSequenceTrackMain(void*, u32, f32);
void setSequenceTrackAux(void*, u32, u32, f32);
void setSequenceTrackOutputVolume(void*, u32, u32, f32);
void setSequenceTrackOutputPan(void*, u32, u32, f32);
void setSequenceTrackOutputSurround(void*, u32, u32, f32);
void setSequenceTrackOutputMain(void*, u32, u32, f32);
void setSequenceTrackOutputAux(void*, u32, u32, u32, f32);
void prepareStreamDriver(void*, const void*);
void loadStreamDriver(void*, const void*);
void setStreamDriverBlock(void*, u32, const void*, u32);
void setStreamDriverSource(void*, u32, u32, u32, u32, u32, u32, const void*, u32, u32);
void setStreamDriverVolume(void*, u32, f32);
void setStreamDriverMask(void*, u32, u32);
void setStreamDriverAllocation(void*, u32, u32);
void setStreamDriverPriority(void*, u32);
void setStreamDriverPitch(void*, u32, f32);
void setStreamDriverChannels(void*, u32, u32, const void*);
void setStreamDriverLpf(void*, u32, f32);
void setStreamDriverBiquad(void*, u32, f32);
void setStreamDriverPan(void*, u32, f32);
void setStreamDriverSend(void*, u32, u32, f32);
void setStreamDriverOutputVolume(void*, u32, u32, f32);
void setStreamDriverMetadata(void*, u32, u32, u32, const void*);
void setStreamDriverOutputPan(void*, u32, u32, f32);
void setStreamDriverOutputSurround(void*, u32, u32, f32);
void setStreamDriverOutputMain(void*, u32, u32, f32);
void setStreamDriverOutputAux(void*, u32, u32, u32, f32);
void* soundTaskScheduler();
void queueSoundTask(void*, u32, u32);
void cancelSoundTask(void*, u32);
void startSoundTask(void*, u32);
void queueMixEffect(nw::snd::AudioMixRuntime*, u32, u32, u32);
void releaseMixEffect(nw::snd::AudioMixRuntime*, u32, u32, u32);
/* The voice commands' target is a PlaybackVoice (their handlers call PlaybackVoice methods). */
static inline nw::snd::PlaybackVoice* targetVoice(void* target) { return static_cast<nw::snd::PlaybackVoice*>(target); }

struct StreamStackArguments {
    be<u32> bufferSize, flags;
};

struct SendCommand : WordDriverCommand {
    be<u32> send;
    be<f32> level;
};

/* 02885FC0 */
void dispatchAudioCommandList(DriverCommand* command) {
    gabi::AutoFrame frame{0x02885FC0};
    gabi::FrameLocal<StreamStackArguments> outgoing(8);
    while (command) {
        switch ((u32)command->id) {
        case DriverDispatch_Signal: {
            auto* message = (TargetDriverCommand*)command;
            *(be<u8>*)message->target.get() = 1;
            break;
        }
        case DriverCommand_PlayerInitialize: {
            auto* message = (PlayerInitializeCommand*)command;
            auto* player = (DriverPlayerParameters*)message->player.get();
            gabi::call_ptr<void>(player->vtable->initialize.function, player);
            *message->initializedFlag = 1;
            break;
        }
        case DriverCommand_SetPanMode: {
            auto* message = (PlayerFlagCommand*)command;
            ((DriverPlayerParameters*)message->player.get())->panMode = (u8)message->flag;
            break;
        }
        case DriverCommand_SetPanCurve: {
            auto* message = (PlayerFlagCommand*)command;
            ((DriverPlayerParameters*)message->player.get())->panCurve = (u8)message->flag;
            break;
        }
        case DriverCommand_SetRemoteFilter: {
            auto* message = (PlayerFlagCommand*)command;
            ((DriverPlayerParameters*)message->player.get())->remoteFilter = (u8)message->secondFlag;
            break;
        }
        case DriverCommand_SetFrontBypass: {
            auto* message = (PlayerFlagCommand*)command;
            ((DriverPlayerParameters*)message->player.get())->frontBypass = (u8)message->flag;
            break;
        }
        case DriverCommand_SetDrcBypass: {
            auto* message = (PlayerFlagCommand*)command;
            ((DriverPlayerParameters*)message->player.get())->drcBypass = (u8)message->flag;
            break;
        }
        case DriverCommand_PlayerFinalize: {
            auto* player = (DriverPlayerParameters*)((TargetDriverCommand*)command)->target.get();
            gabi::call_ptr<void>(player->vtable->finalize.function, player);
            break;
        }
        case DriverCommand_PlayerStart: {
            auto* player = (DriverPlayerParameters*)((TargetDriverCommand*)command)->target.get();
            gabi::call_ptr<void>(player->vtable->start.function, player);
            break;
        }
        case DriverCommand_PlayerStop: {
            auto* message = (PlayerFlagCommand*)command;
            if (message->flag) ((DriverPlayerParameters*)message->player.get())->volumeBits = 0;
            auto* player = (DriverPlayerParameters*)message->player.get();
            gabi::call_ptr<void>(player->vtable->stop.function, player);
            break;
        }
        case DriverCommand_PlayerPause: {
            auto* message = (PlayerFlagCommand*)command;
            auto* player = (DriverPlayerParameters*)message->player.get();
            u32 paused = message->flag;
            gabi::call_ptr<void>(player->vtable->pause.function, player, paused);
            break;
        }
        case DriverCommand_PlayerParam: {
            auto* message = (PlayerParametersCommand*)command;
            u32 value = message->volumeBits;
            message->player->volumeBits = value;
            value = message->pitchBits;
            message->player->pitchBits = value;
            value = message->lpfBits;
            message->player->lpfBits = value;
            DriverPlayerParameters* player = message->player;
            u32 kind = message->biquadType;
            f32 filter = message->biquadValue;
            setDriverBiquad(player, kind, filter);
            player = message->player;
            player->outputLine = message->outputLine;
            player = message->player;
            for (u32 word = 0; word < 20; ++word) player->outputs[0].words[word] = message->outputs[0].words[word];
            for (u32 output = 1; output < 3; ++output) {
                player = message->player;
                for (u32 word = 0; word < 20; ++word)
                    player->outputs[output].words[word] = message->outputs[output].words[word];
            }
            for (u32 remote = 0; remote < 4; ++remote) {
                player = message->player;
                player->remotes[remote].words[0] = message->remotes[remote].words[0];
                player->remotes[remote].words[1] = message->remotes[remote].words[1];
                player->remotes[remote].words[2] = message->remotes[remote].words[2];
            }
            break;
        }
        case DriverCommand_ClearStopRequest:
            ((DriverSoundPlayer*)((TargetDriverCommand*)command)->target.get())->stopRequested = 0;
            break;
        case DriverDispatch_PrepareSequence: {
            auto* message = (PrepareDriverCommand*)command;
            prepareSequenceDriver(message->target, message->descriptor);
            setSequenceDriverMode(message->target, (u8)message->mode);
            setSequenceDriverCurve(message->target, (u8)message->curve);
            u32 resource = message->resource;
            void* player = message->target;
            u32 size = message->resourceSize;
            bindSequenceResource(player, resource, size);
            break;
        }
        case DriverDispatch_LoadSequence: {
            auto* message = (TwoDescriptorCommand*)command;
            loadSequenceDriver(message->target, message->descriptor, message->settings);
            break;
        }
        case DriverDispatch_StartSequence: {
            auto* message = (PrepareDriverCommand*)command;
            startSequenceDriver(message->target, message->descriptor);
            break;
        }
        case DriverDispatch_StopSequence: {
            auto* message = (PairDriverCommand*)command;
            u32 fade = message->value;
            void* target = message->target;
            u32 flags = message->extra;
            stopSequenceDriver(target, fade, flags);
            break;
        }
        case DriverDispatch_SetSequenceSpeed: {
            auto* message = (DriverFloatCommand*)command;
            setSequenceDriverSpeed(message->target, message->value);
            break;
        }
        case DriverDispatch_SetSequenceMode: {
            auto* message = (ByteDriverCommand*)command;
            setSequenceDriverMode(message->target, (u8)message->value);
            break;
        }
        case DriverDispatch_WriteSequenceVariable: {
            auto* message = (VariableDriverCommand*)command;
            void* target = message->target;
            s32 value = message->value;
            void* variable = message->variable;
            writeSequenceVariable(target, variable, value);
            break;
        }
        case DriverDispatch_WriteGlobalVariable: {
            auto* message = (VariableDriverCommand*)command;
            writeGlobalSequenceVariable(message->variable, (s16)message->value);
            break;
        }
        case DriverDispatch_WriteTrackVariable: {
            auto* message = (VariableDriverCommand*)command;
            u32 index = message->WordDriverCommand::value;
            void* player = message->target;
            void* track = findSequenceTrack(player, index);
            if (track) {
                void* variable = message->variable;
                s32 value = message->value;
                writeTrackVariable(track, variable, value);
            }
            break;
        }
        case DriverDispatch_SetTrackMask: {
            auto* message = (PairDriverCommand*)command;
            setSequenceTrackMask(message->target, message->value, message->extra);
            break;
        }
        case DriverDispatch_SetTrackMute: {
            auto* message = (TrackByteCommand*)command;
            setSequenceTrackMute(message->target, message->WordDriverCommand::value, (u8)message->value,
                                 message->duration);
            break;
        }
        case DriverDispatch_SetTrackVolume: {
            auto* message = (TrackFloatCommand*)command;
            setSequenceTrackVolume(message->target, message->WordDriverCommand::value, message->value);
            break;
        }
        case DriverDispatch_SetTrackPitch: {
            auto* message = (TrackFloatCommand*)command;
            setSequenceTrackPitch(message->target, message->WordDriverCommand::value, message->value);
            break;
        }
        case DriverDispatch_SetTrackPan: {
            auto* message = (TrackFloatCommand*)command;
            setSequenceTrackPan(message->target, message->WordDriverCommand::value, message->value);
            break;
        }
        case DriverDispatch_SetTrackLpf: {
            auto* message = (TrackFloatCommand*)command;
            setSequenceTrackLpf(message->target, message->WordDriverCommand::value, message->value);
            break;
        }
        case DriverDispatch_SetTrackBiquad: {
            auto* message = (TrackFloatCommand*)command;
            setSequenceTrackBiquad(message->target, message->WordDriverCommand::value, message->value);
            break;
        }
        case DriverDispatch_SetTrackSurround: {
            auto* message = (TrackFloatCommand*)command;
            setSequenceTrackSurround(message->target, message->WordDriverCommand::value, message->value);
            break;
        }
        case DriverDispatch_SetTrackMain: {
            auto* message = (TrackFloatCommand*)command;
            setSequenceTrackMain(message->target, message->WordDriverCommand::value, message->value);
            break;
        }
        case DriverDispatch_SetTrackSend: {
            auto* message = (SendCommand*)command;
            setSequenceTrackSend(message->target, message->value, message->send, message->level);
            break;
        }
        case DriverDispatch_SetTrackPriority: {
            auto* message = (PairDriverCommand*)command;
            setSequenceTrackPriority(message->target, message->value, message->extra);
            break;
        }
        case DriverDispatch_SetTrackTranspose: {
            auto* message = (TrackByteCommand*)command;
            setSequenceTrackTranspose(message->target, message->WordDriverCommand::value, (s8)(u8)message->value);
            break;
        }
        case DriverDispatch_SetTrackFlag: {
            auto* message = (TrackByteCommand*)command;
            setSequenceTrackFlag(message->target, message->WordDriverCommand::value, (u8)message->value);
            break;
        }
        case DriverDispatch_SetTrackAllocation: {
            auto* message = (PairDriverCommand*)command;
            setSequenceTrackAllocation(message->target, message->value, message->extra);
            break;
        }
        case DriverDispatch_SetSequenceAllocation: {
            auto* message = (WordDriverCommand*)command;
            setSequenceAllocation(message->target, message->value);
            break;
        }
        case DriverDispatch_SetTrackAux: {
            auto* message = (TrackFloatCommand*)command;
            setSequenceTrackAux(message->target, message->WordDriverCommand::value, message->duration, message->value);
            break;
        }
        case DriverDispatch_SetTrackOutputVolume: {
            auto* message = (TrackFloatCommand*)command;
            setSequenceTrackOutputVolume(message->target, message->output, message->WordDriverCommand::value,
                                         message->value);
            break;
        }
        case DriverDispatch_SetTrackOutputPan: {
            auto* message = (TrackFloatCommand*)command;
            setSequenceTrackOutputPan(message->target, message->output, message->WordDriverCommand::value,
                                      message->value);
            break;
        }
        case DriverDispatch_SetTrackOutputSurround: {
            auto* message = (TrackFloatCommand*)command;
            setSequenceTrackOutputSurround(message->target, message->output, message->WordDriverCommand::value,
                                           message->value);
            break;
        }
        case DriverDispatch_SetTrackOutputMain: {
            auto* message = (TrackFloatCommand*)command;
            setSequenceTrackOutputMain(message->target, message->output, message->WordDriverCommand::value,
                                       message->value);
            break;
        }
        case DriverDispatch_SetTrackOutputAux: {
            auto* message = (TrackFloatCommand*)command;
            setSequenceTrackOutputAux(message->target, message->output, message->WordDriverCommand::value,
                                      message->duration, message->value);
            break;
        }
        case DriverDispatch_PrepareWave: {
            auto* message = (WaveSetupCommand*)command;
            commandWavePlayer(message->player)->prepareDirect(&message->start, &message->source);
            break;
        }
        case DriverDispatch_LoadWave: {
            auto* message = (WaveConfigurationCommand*)command;
            commandWavePlayer(message->player)->prepareDeferred(&message->start, &message->configuration);
            break;
        }
        case DriverDispatch_SetWaveMode: {
            auto* message = (ByteDriverCommand*)command;
            commandWavePlayer((DriverSoundPlayer*)message->target.get())->setPriority((u8)message->value);
            break;
        }
        case DriverDispatch_SetWaveModes: {
            auto* message = (ByteDriverCommand*)command;
            commandWavePlayer((DriverSoundPlayer*)message->target.get())->setPriority((u8)message->value);
            commandWavePlayer((DriverSoundPlayer*)message->target.get())->setRemote((u8)message->extra);
            break;
        }
        case DriverDispatch_PrepareStream: {
            auto* message = (PrepareDriverCommand*)command;
            prepareStreamDriver(message->target, message->descriptor);
            break;
        }
        case DriverDispatch_LoadStream: {
            auto* message = (PrepareDriverCommand*)command;
            loadStreamDriver(message->target, message->descriptor);
            break;
        }
        case DriverDispatch_SetStreamBlock: {
            auto* message = (StreamBlockCommand*)command;
            setStreamDriverBlock(message->target, (u8)message->value, message->block, (u16)message->count);
            break;
        }
        case DriverDispatch_SetStreamSource: {
            auto* message = (StreamPrepareCommand*)command;
            u32 buffer = message->buffer;
            u32 mode = message->mode;
            u32 channel = message->channel;
            void* target = message->target;
            u32 byteCount = message->byteCount;
            u32 position = message->position;
            u32 bufferSize = message->bufferSize;
            u32 source = message->source;
            outgoing->bufferSize = bufferSize;
            u32 flags = message->flags;
            outgoing->flags = flags;
            setStreamDriverSource(target, channel, source, position, byteCount, buffer, mode, message->descriptor,
                                  bufferSize, flags);
            break;
        }
        case DriverDispatch_MarkFinished:
            ((DriverSoundPlayer*)((TargetDriverCommand*)command)->target.get())->playFinished = 1;
            break;
        case DriverDispatch_SetStreamVolume: {
            auto* message = (TrackFloatCommand*)command;
            setStreamDriverVolume(message->target, message->WordDriverCommand::value, message->value);
            break;
        }
        case DriverDispatch_SetStreamPitch: {
            auto* message = (TrackFloatCommand*)command;
            setStreamDriverPitch(message->target, message->WordDriverCommand::value, message->value);
            break;
        }
        case DriverDispatch_SetStreamLpf: {
            auto* message = (TrackFloatCommand*)command;
            setStreamDriverLpf(message->target, message->WordDriverCommand::value, message->value);
            break;
        }
        case DriverDispatch_SetStreamBiquad: {
            auto* message = (TrackFloatCommand*)command;
            setStreamDriverBiquad(message->target, message->WordDriverCommand::value, message->value);
            break;
        }
        case DriverDispatch_SetStreamPan: {
            auto* message = (TrackFloatCommand*)command;
            setStreamDriverPan(message->target, message->WordDriverCommand::value, message->value);
            break;
        }
        case DriverDispatch_SetStreamMask: {
            auto* message = (PairDriverCommand*)command;
            setStreamDriverMask(message->target, message->value, message->extra);
            break;
        }
        case DriverDispatch_SetStreamAllocation: {
            auto* message = (TripleDriverCommand*)command;
            setStreamDriverAllocation(message->target, message->value, message->duration);
            break;
        }
        case DriverDispatch_SetStreamPriority: {
            auto* message = (WordDriverCommand*)command;
            setStreamDriverPriority(message->target, message->value);
            break;
        }
        case DriverDispatch_SetStreamChannels: {
            auto* message = (TripleDriverCommand*)command;
            setStreamDriverChannels(message->target, message->value, message->extra, &message->duration);
            break;
        }
        case DriverDispatch_SetStreamSend: {
            auto* message = (TrackFloatCommand*)command;
            setStreamDriverSend(message->target, message->WordDriverCommand::value, message->duration, message->value);
            break;
        }
        case DriverDispatch_SetStreamOutputVolume: {
            auto* message = (TrackFloatCommand*)command;
            setStreamDriverOutputVolume(message->target, message->output, message->WordDriverCommand::value,
                                        message->value);
            break;
        }
        case DriverDispatch_SetStreamOutputPan: {
            auto* message = (TrackFloatCommand*)command;
            setStreamDriverOutputPan(message->target, message->output, message->WordDriverCommand::value,
                                     message->value);
            break;
        }
        case DriverDispatch_SetStreamOutputSurround: {
            auto* message = (TrackFloatCommand*)command;
            setStreamDriverOutputSurround(message->target, message->output, message->WordDriverCommand::value,
                                          message->value);
            break;
        }
        case DriverDispatch_SetStreamOutputMain: {
            auto* message = (TrackFloatCommand*)command;
            setStreamDriverOutputMain(message->target, message->output, message->WordDriverCommand::value,
                                      message->value);
            break;
        }
        case DriverDispatch_SetStreamMetadata: {
            auto* message = (StreamMetadataCommand*)command;
            setStreamDriverMetadata(message->target, message->channels, message->value, message->extra,
                                    &message->duration);
            break;
        }
        case DriverDispatch_SetStreamOutputAux: {
            auto* message = (TrackFloatCommand*)command;
            setStreamDriverOutputAux(message->target, message->output, message->WordDriverCommand::value,
                                     message->duration, message->value);
            break;
        }
        case DriverDispatch_QueueTask: {
            auto* message = (WordDriverCommand*)command;
            void* scheduler = soundTaskScheduler();
            u32 value = message->value;
            u32 task = gabi::ea(message->target.get());
            queueSoundTask(scheduler, task, value);
            break;
        }
        case DriverDispatch_CancelTask: {
            void* scheduler = soundTaskScheduler();
            u32 task = gabi::ea(((TargetDriverCommand*)command)->target.get());
            cancelSoundTask(scheduler, task);
            break;
        }
        case DriverDispatch_StartTask: {
            void* scheduler = soundTaskScheduler();
            u32 task = gabi::ea(((TargetDriverCommand*)command)->target.get());
            startSoundTask(scheduler, task);
            break;
        }
        case DriverDispatch_AddEffect: {
            AudioMixRuntime* runtime = commandMixRuntime();
            auto* message = (PairDriverCommand*)command;
            u32 effect = message->extra;
            u32 bus = gabi::ea(message->target.get());
            u32 output = message->value;
            queueMixEffect(runtime, bus, output, effect);
            break;
        }
        case DriverDispatch_RemoveEffect: {
            AudioMixRuntime* runtime = commandMixRuntime();
            auto* message = (TripleDriverCommand*)command;
            u32 effect = message->extra;
            u32 bus = gabi::ea(message->target.get());
            u32 output = message->duration;
            releaseMixEffect(runtime, bus, output, effect);
            break;
        }
        case DriverDispatch_SetDriverFlags: {
            PlaybackVoicePool* voices = PlaybackVoicePool::instance();
            u32 mask = gabi::ea(((TargetDriverCommand*)command)->target.get());
            voices->markDirty(mask);
            break;
        }
        case DriverDispatch_ControlVoice: {
            auto* message = (WordDriverCommand*)command;
            switch ((u32)message->value) {
            case 0: targetVoice(message->target)->requestStart(); break;
            case 1:
                targetVoice(message->target)->stop();
                targetVoice(message->target)->release();
                break;
            case 2: targetVoice(message->target)->requestPause(1); break;
            case 3: targetVoice(message->target)->requestPause(0); break;
            default: break;
            }
            break;
        }
        case DriverDispatch_SetVoiceFormat: {
            auto* message = (StreamStateCommand*)command;
            PlaybackVoice* voice = targetVoice(message->target);
            voice->setSampleFormat(message->value);
            voice->setSampleRate(message->extra);
            voice->setInterpolationMode((u8)message->flags);
            break;
        }
        case DriverDispatch_SetVoiceAdpcmCoefficients: {
            auto* message = (TripleDriverCommand*)command;
            targetVoice(message->target)->setAdpcmCoefficients(message->value, gabi::at<const be<u16>>(message->duration));
            break;
        }
        default: break;
        }
        command = command->next;
    }
}

/* WWHD glue
ext 028A3710 void setDriverBiquad(void*,u32,f32)
ext 0288BC00 void prepareSequenceDriver(void*,const void*)
ext 0288C048 void setSequenceDriverMode(void*,u32)
ext 0288C050 void setSequenceDriverCurve(void*,u32)
ext 0288BF58 void bindSequenceResource(void*,u32,u32)
ext 0288BE2C void loadSequenceDriver(void*,const void*,const void*)
ext 0288BD14 void startSequenceDriver(void*,const void*)
ext 0288AE6C void stopSequenceDriver(void*,u32,u32)
ext 0288C040 void setSequenceDriverSpeed(void*,f32)
ext 0288C580 void writeSequenceVariable(void*,void*,s32)
ext 0288C590 void writeGlobalSequenceVariable(void*,s32)
ext 0288A750 void* findSequenceTrack(void*,u32)
ext 0288E744 void writeTrackVariable(void*,void*,s32)
ext 0288C058 void setSequenceTrackMask(void*,u32,u32)
ext 0288C098 void setSequenceTrackMute(void*,u32,u32,u32)
ext 0288C0DC void setSequenceTrackVolume(void*,u32,f32)
ext 0288C114 void setSequenceTrackPitch(void*,u32,f32)
ext 0288C14C void setSequenceTrackPan(void*,u32,f32)
ext 0288C184 void setSequenceTrackSend(void*,u32,u32,f32)
ext 0288C1C0 void setSequenceTrackPriority(void*,u32,u32)
ext 0288C228 void setSequenceTrackTranspose(void*,u32,s32)
ext 0288C264 void setSequenceTrackFlag(void*,u32,u32)
ext 0288C2A0 void setSequenceTrackAllocation(void*,u32,u32)
ext 0288C2E0 void setSequenceAllocation(void*,u32)
ext 0288C31C void setSequenceTrackLpf(void*,u32,f32)
ext 0288C354 void setSequenceTrackBiquad(void*,u32,f32)
ext 0288C38C void setSequenceTrackSurround(void*,u32,f32)
ext 0288C3C4 void setSequenceTrackMain(void*,u32,f32)
ext 0288C3FC void setSequenceTrackAux(void*,u32,u32,f32)
ext 0288C438 void setSequenceTrackOutputVolume(void*,u32,u32,f32)
ext 0288C478 void setSequenceTrackOutputPan(void*,u32,u32,f32)
ext 0288C4B8 void setSequenceTrackOutputSurround(void*,u32,u32,f32)
ext 0288C4F8 void setSequenceTrackOutputMain(void*,u32,u32,f32)
ext 0288C538 void setSequenceTrackOutputAux(void*,u32,u32,u32,f32)
ext 028A168C void nw::snd::WaveSoundPlayer::prepareDirect(const nw::snd::WaveSoundStart*,const nw::snd::WaveSoundSource*)
ext 028A173C void nw::snd::WaveSoundPlayer::prepareDeferred(const nw::snd::WaveSoundStart*,const nw::snd::WaveSoundLoadConfiguration*)
ext 028A1820 void nw::snd::WaveSoundPlayer::setPriority(u32)
ext 028A1828 void nw::snd::WaveSoundPlayer::setRemote(u32)
ext 0289ACD8 void prepareStreamDriver(void*,const void*)
ext 0289AE20 void loadStreamDriver(void*,const void*)
ext 0289B86C void setStreamDriverBlock(void*,u32,const void*,u32)
ext 0289BC0C void setStreamDriverSource(void*,u32,u32,u32,u32,u32,u32,const void*,u32,u32)
ext 0289B0F8 void setStreamDriverVolume(void*,u32,f32)
ext 0289B13C void setStreamDriverMask(void*,u32,u32)
ext 0289B180 void setStreamDriverAllocation(void*,u32,u32)
ext 0289B1C4 void setStreamDriverPriority(void*,u32)
ext 0289B20C void setStreamDriverPitch(void*,u32,f32)
ext 0289B250 void setStreamDriverChannels(void*,u32,u32,const void*)
ext 0289B2BC void setStreamDriverLpf(void*,u32,f32)
ext 0289B300 void setStreamDriverBiquad(void*,u32,f32)
ext 0289B344 void setStreamDriverPan(void*,u32,f32)
ext 0289B388 void setStreamDriverSend(void*,u32,u32,f32)
ext 0289B3D8 void setStreamDriverOutputVolume(void*,u32,u32,f32)
ext 0289B428 void setStreamDriverMetadata(void*,u32,u32,u32,const void*)
ext 0289B4B0 void setStreamDriverOutputPan(void*,u32,u32,f32)
ext 0289B500 void setStreamDriverOutputSurround(void*,u32,u32,f32)
ext 0289B550 void setStreamDriverOutputMain(void*,u32,u32,f32)
ext 0289B5A0 void setStreamDriverOutputAux(void*,u32,u32,u32,f32)
ext 028A5B00 void* soundTaskScheduler()
ext 028A5B4C void queueSoundTask(void*,u32,u32)
ext 028A5BD0 void cancelSoundTask(void*,u32)
ext 028A5C00 void startSoundTask(void*,u32)
ext 02889B10 void queueMixEffect(nw::snd::AudioMixRuntime*,u32,u32,u32)
ext 02889CAC void releaseMixEffect(nw::snd::AudioMixRuntime*,u32,u32,u32)
ext 0289D624 void nw::snd::PlaybackVoice::requestStart()
ext 0289D6B0 void nw::snd::PlaybackVoice::stop()
ext 0289D598 void nw::snd::PlaybackVoice::release()
ext 0289D8BC void nw::snd::PlaybackVoice::requestPause(u32)
ext 0289EAB0 void nw::snd::PlaybackVoice::setSampleFormat(u32)
ext 0289EAE8 void nw::snd::PlaybackVoice::setSampleRate(u32)
ext 0289ECF8 void nw::snd::PlaybackVoice::setInterpolationMode(u32)
ext 0289F1D0 void nw::snd::PlaybackVoice::setAdpcmCoefficients(u32,const be<u16>*)
*/

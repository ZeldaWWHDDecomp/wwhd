/* NW-SND#6: starting an archive sound: the archive player checks the sound and the player limits, takes
 * a sound from the kind's pool, brackets the preparation with the resource hook, prepares the sequence,
 * stream or wave sound, and attaches the caller's handle.
 * Written from WWHD's instructions; verified by tools/verify (spec lib2_nw_snd_6). */
#include "nw/snd/ArchiveSoundStart.h"
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
using namespace nw::snd;

/* 028952AC */
void ArchivePlayback::applySoundParameters(BasicSound* sound, const ArchiveSoundSummary* info) {
    gabi::AutoFrame frame{gabi::current_function()};
    sound->setInitialVolume((f32)(u8)info->volume / 127.0f);
    // the archive words go to the byte setters whole (canonical summary names: panMode, panCurve)
    setSoundPanCurve(sound, info->panMode);
    setSoundRemoteFilter(sound, info->panCurve);
    sound->setFrontBypass(info->remoteFilter);
}

/* Switches the archive's extension and the resource hook off (while a sound the hook cannot
 * supply is prepared) or back on. */
/* inlined into 0289533C */
static inline void setResourceHooksEnabled(ArchivePlayback* playback, u8 enabled) {
    SoundArchive* archive = playback->archive;
    ArchiveExtension_l* extension = archive->extension;
    extension->enabled = enabled;
    playback->resourceHook->enabled = enabled;
}

/* 0289533C */
u32 ArchivePlayback::startSound(SoundHandle* handle, u32 id, const AmbientInfo* ambient, SoundActor* actor,
                                  u32 hold, SoundStartInfo* info) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u8>> holdFlag(0x10);
    *holdFlag = hold;
    if (!hasArchiveData()) return SoundStart_NotReady;
    if (handle->sound != NULL) handle->detachSound();
    gabi::FrameLocal<ArchiveSoundSummary> soundInfo(0x30);
    if (!archive->readSoundInfo(id, soundInfo.get())) return SoundStart_InvalidSound;

    // the archive's settings, overridden by the caller's
    s32 priority = soundInfo->playerPriority;
    u32 actorPlayerId = soundInfo->actorPlayerId;
    u32 playerId = soundInfo->playerId;
    u32 offsetKind = 0, offset = 0;
    PlaybackSequenceOptions_l* sequenceOptions = NULL;
    PlaybackStreamFile_l* streamFile = NULL;
    gabi::FrameLocal<gptr<const WaveStartOverride>> waveOverride(8);
    gabi::FrameLocal<gptr<SoundPlayer>> targetPlayer(12);
    gabi::FrameLocal<be<u32>> outputMode(0x14);
    *waveOverride = NULL;
    *outputMode = 0;
    if (info != NULL) {
        u32 flags = info->flags;
        if (flags & SoundStart_Offset) { offset = info->offset; offsetKind = info->offsetKind; }
        if (flags & SoundStart_Priority) priority = info->priority;
        if (flags & SoundStart_Player) playerId = info->playerId;
        if (flags & SoundStart_ActorPlayer) actorPlayerId = info->actorPlayerId;
        if (flags & SoundStart_Sequence) sequenceOptions = &info->sequence;
        if (flags & SoundStart_Stream) streamFile = &info->stream;
        if (flags & SoundStart_Wave) *waveOverride = &info->wave;
        if (flags & SoundStart_OutputMode) {
            u32 mode = info->outputMode;
            *outputMode = mode;
            if (mode == 3 && isSoftwareMixReady() == 0) return SoundStart_NoSoftwareMix;
        }
    }

    // room in the player (and in the actor's queue) for this priority
    s32 startPriority = (s32)((u32)priority - (*holdFlag != 0 ? 1 : 0));
    s32 ambientPriority = ambient != NULL ? AmbientInfo::priorityOf(ambient, id) : 0;
    s32 incoming = clampedPriority((s32)((u32)startPriority + (u32)ambientPriority));
    PrioritySoundQueue* actorQueue = NULL;
    if (actor != NULL) {
        if (actorPlayerId >= 4) return SoundStart_InvalidActorPlayer;
        actorQueue = wrappedAddress(&actor->queues[actorPlayerId]);
        if (actorQueue == NULL) return SoundStart_InvalidActorPlayer;
    }
    SoundPlayer* player = playerEntry(playerId);
    *targetPlayer = player;
    if (!player->canStart(incoming)) return SoundStart_LowPriority;
    if (actorQueue != NULL && !actorQueue->canAccept(incoming)) return SoundStart_LowPriority;

    // a sound object of the kind
    BasicSound* sequenceSound = NULL;
    BasicSound* streamSound = NULL;
    BasicSound* waveSound = NULL;
    BasicSound* sound;
    switch (archive->soundKind(id)) {
    case ArchiveSound_Sequence:
        sequenceSound = allocateSequenceSound(&sequences, id, startPriority, ambientPriority, ambient);
        sound = sequenceSound;
        break;
    case ArchiveSound_Stream:
        streamSound = allocateStreamSound(&streams, id, startPriority, ambientPriority, ambient);
        sound = streamSound;
        break;
    case ArchiveSound_Wave:
        waveSound = allocateWaveSound(&waves, id, startPriority, ambientPriority, ambient);
        sound = waveSound;
        break;
    default:
        return SoundStart_InvalidSound;
    }
    if (sound == NULL) return SoundStart_NoSoundSlot;
    player = *targetPlayer;
    if (!player->insertSound(sound)) {
        sound->callFinalize();
        return SoundStart_Rejected;
    }
    if (info != NULL && ((u32)info->flags & SoundStart_FinalizeCallback)) sound->finalizeCallback = info->finalizeCallback;
    sound->resetOutputLine();
    setSoundPanMode(sound, *outputMode);
    sound->setDrcBypass(soundInfo->frontBypass);

    // the resource hook: if it cannot supply this sound, prepare it from the archive with hooks off
    u32 hooksOff = 0;
    u32 kind;
    if (isHookEnabled()) {
        beginHookedStart();
        const char* label = archive->itemLabel(id);
        ArchiveResourceHook* resource = resourceHook;
        u32 accepted = 0;
        if (resource->enabled != 0) accepted = resource->acceptsLabel(label);
        if (!accepted) {
            setResourceHooksEnabled(this, 0);
            hooksOff = 1;
            kind = archive->soundKind(id);
        } else {
            u32 resourceKind = archive->soundKind(id);
            if (resourceKind == ArchiveSound_Sequence) hooksOff = (hasSequenceFile(sequenceSound) ^ 1) & 0xFF;
            else if (resourceKind == ArchiveSound_Stream) hooksOff = (hasSoundLabel(streamSound) ^ 1) & 0xFF;
            else if (resourceKind == ArchiveSound_Wave) hooksOff = (hasWaveSoundFile(waveSound) ^ 1) & 0xFF;
            if (hooksOff) setResourceHooksEnabled(this, 0);
            kind = archive->soundKind(id);
        }
    } else {
        kind = archive->soundKind(id);
    }

    // the kind's preparation; on failure the sound is finalized and the hooks restored
    u32 result;
    if (kind == ArchiveSound_Sequence) {
        gabi::FrameLocal<ArchiveSequenceSummary> sequence(0x48);
        sequence->startOffset = 0;
        sequence->trackFlags = 0;
        sequence->priority = 0;
        sequence->priorityFix = 0;
        for (u32 i = 0; i < 4; ++i) sequence->bankIds[i] = 0xFFFFFFFF;
        if (!archive->readSequenceInfo(id, sequence.get())) {
            sequenceSound->callFinalize();
            if (hooksOff) setResourceHooksEnabled(this, 1);
            endHookedStart();
            return SoundStart_InvalidSound;
        }
        if (sequenceOptions != NULL) {
            for (u32 i = 0; i < 4; ++i) {
                u32 bank = sequenceOptions->banks[i];
                if (bank != 0xFFFFFFFF) sequence->bankIds[i] = bank;
            }
        }
        result = prepareSequence(sequenceSound, soundInfo.get(), sequence.get(), offsetKind, offset, sequenceOptions);
        if (result != SoundStart_Success) {
            sequenceSound->callFinalize();
            if (hooksOff) setResourceHooksEnabled(this, 1);
            endHookedStart();
            return result;
        }
    } else if (kind == ArchiveSound_Stream) {
        gabi::FrameLocal<ArchiveStreamSummary> stream(0x64);
        stream->trackFlags = 0;
        stream->routing[0] = 127;
        stream->channelCount = 0;
        stream->pitch = 1.0f;
        __construct_array(stream->tracks, 8, sizeof(ArchiveStreamTrackSummary), (void* (*)(void*))constructStreamTrack);
        stream->prefetchChannels = 0;
        fillBytes(&stream->routing[1], 0, 3);
        if (!archive->readStreamInfo(id, stream.get())) {
            streamSound->callFinalize();
            if (hooksOff) setResourceHooksEnabled(this, 1);
            endHookedStart();
            return SoundStart_InvalidSound;
        }
        gabi::FrameLocal<ArchivePrefetchSummary> prefetch(0x24);
        prefetch->offset = 0;
        prefetch->enabled = 0;
        prefetch->size = 0;
        ArchivePrefetchSummary* prefetchInfo = NULL;
        if (stream->prefetchChannels == 2) {
            if (!archive->readPrefetchInfo(id, prefetch.get())) {
                streamSound->callFinalize();
                if (hooksOff) setResourceHooksEnabled(this, 1);
                return SoundStart_InvalidSound;
            }
            prefetchInfo = prefetch.get();
        }
        result = prepareStream(streamSound, soundInfo.get(), stream.get(), prefetchInfo,
                               offsetKind, offset, streamFile);
        if (result != SoundStart_Success) {
            streamSound->callFinalize();
            if (hooksOff) setResourceHooksEnabled(this, 1);
            endHookedStart();
            return result;
        }
    } else if (kind == ArchiveSound_Wave) {
        gabi::FrameLocal<ArchiveWaveSummary> wave(0x18);
        wave->trackFlags = 0;
        wave->priority = 0;
        wave->priorityFix = 0;
        if (!archive->readWaveInfo(id, wave.get())) {
            waveSound->callFinalize();
            if (hooksOff) setResourceHooksEnabled(this, 1);
            endHookedStart();
            return SoundStart_InvalidSound;
        }
        result = prepareWaveSound(waveSound, soundInfo.get(), wave.get(), offsetKind, offset, *waveOverride);
        if (result != SoundStart_Success) {
            waveSound->callFinalize();
            if (hooksOff) setResourceHooksEnabled(this, 1);
            endHookedStart();
            return result;
        }
    } else {
        sound->callFinalize();
        if (hooksOff) setResourceHooksEnabled(this, 1);
        endHookedStart();
        return SoundStart_InvalidSound;
    }
    if (hooksOff) setResourceHooksEnabled(this, 1);
    endHookedStart();

    // final settings, the actor's queue, and the handle
    applySoundParameters(sound, soundInfo.get());
    if (actorQueue != NULL && !actorQueue->append(sound)) {
        sound->callFinalize();
        return SoundStart_Rejected;
    }
    if (actor != NULL) sound->attachSoundActor(actor);
    if (*holdFlag != 0) sound->setPlayerPriority(priority);
    handle->attachSound(sound);
    return SoundStart_Success;
}

/* WWHD glue
 ext 02893CE8 nw::snd::SoundPlayer* nw::snd::ArchivePlayback::playerEntry(u32)
 ext 028941A0 u32 nw::snd::ArchivePlayback::prepareSequence(nw::snd::BasicSound*,nw::snd::ArchiveSoundSummary*,nw::snd::ArchiveSequenceSummary*,u32,u32,nw::snd::PlaybackSequenceOptions_l*)
 ext 02894924 u32 nw::snd::ArchivePlayback::prepareStream(nw::snd::BasicSound*,nw::snd::ArchiveSoundSummary*,nw::snd::ArchiveStreamSummary*,nw::snd::ArchivePrefetchSummary*,u32,u32,nw::snd::PlaybackStreamFile_l*)
 ext 0288E934 u32 nw::snd::SoundArchive::readSoundInfo(u32,nw::snd::ArchiveSoundSummary*)
 ext 0288E92C u32 nw::snd::SoundArchive::soundKind(u32)
 ext 0288E9BC u32 nw::snd::SoundArchive::readSequenceInfo(u32,nw::snd::ArchiveSequenceSummary*)
 ext 0288EBCC u32 nw::snd::SoundArchive::readStreamInfo(u32,nw::snd::ArchiveStreamSummary*)
 ext 0288ECDC u32 nw::snd::SoundArchive::readWaveInfo(u32,nw::snd::ArchiveWaveSummary*)
 ext 0288EC54 u32 nw::snd::SoundArchive::readPrefetchInfo(u32,nw::snd::ArchivePrefetchSummary*)
 ext 02885D28 s32 nw::snd::AmbientInfo::priorityOf(const nw::snd::AmbientInfo*,u32)
 ext 02887570 u32 nw::snd::PrioritySoundQueue::canAccept(s32)
 ext 028875F8 u32 nw::snd::PrioritySoundQueue::append(nw::snd::BasicSound*)
 ext 02885900 void nw::snd::BasicSound::setInitialVolume(f32)
 ext 028859B8 void setSoundPanCurve(nw::snd::BasicSound*,u32)
 ext 02885A40 void setSoundRemoteFilter(nw::snd::BasicSound*,u32)
 ext 02885BD8 void setSoundPanMode(nw::snd::BasicSound*,u32)
 ext 02885B50 void nw::snd::BasicSound::setFrontBypass(u8)
 ext 02885AC8 void nw::snd::BasicSound::setDrcBypass(u8)
 ext 028859A8 void nw::snd::BasicSound::resetOutputLine()
 ext 02885C74 void nw::snd::BasicSound::attachSoundActor(nw::snd::SoundActor*)
 ext 02883F54 void nw::snd::BasicSound::setPlayerPriority(s32)
*/

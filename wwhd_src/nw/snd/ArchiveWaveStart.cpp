/* NW-SND#6: wave sound start (wave file and wave lookup, through the resource hook, the address
 * cache or the caller's override; deferred when the file is not in memory and the player has heaps).
 * Written from WWHD's instructions; verified by tools/verify (spec lib2_nw_snd_6). */
#include "nw/snd/ArchiveWaveStart.h"
using namespace nw::snd;

/* 02894D84 */
u32 ArchivePlayback::prepareWaveSound(BasicSound* sound, const ArchiveSoundSummary* soundInfo,
                                        const ArchiveWaveSummary* info, u32 offsetKind, u32 offset,
                                        const WaveStartOverride* override) {
    gabi::AutoFrame frame{gabi::current_function()};
    WaveSound* waveSound = gabi::at<WaveSound>(gabi::ea(sound));   // a sound from the wave pool
    SoundPlayer* player = sound->soundPlayer.get();
    u32 mayDefer = player->heapCount != 0;
    u32 defer = 0;
    const void* wave = NULL;
    const void* file = NULL;
    gabi::FrameLocal<WaveSoundFileReference> fileReference(8);
    gabi::FrameLocal<WaveSoundStart> start(0x10);
    gabi::FrameLocal<WaveSoundLoadRequest> request(0x1C);
    gabi::FrameLocal<WaveArchiveReader> reader(0x2C);
    fileReference->image = 0;
    fileReference->itemId = NoFile;
    if (resourceHook != NULL) {
        const char* label = archive->itemLabel(sound->soundId);
        if (label != NULL) {
            ArchiveResourceHook* resource = resourceHook;
            if (resource->enabled != 0) {
                file = resource->findFile(label, "wsd", "bxwsd", 0);
                resource = resourceHook;
            }
            const void* waveArchive = NULL;
            if (resource->enabled != 0) waveArchive = resource->findFile(label, "wsd", "bxwar", 0);
            if (file != NULL && waveArchive != NULL) {
                WaveArchiveReader::construct(reader.get(), static_cast<const WaveArchiveHeader*>(waveArchive), 0);
                wave = reader->loadedAddress(info->waveIndex);
            }
        }
    }
    if (wave == NULL) file = addressCache->fileAddress(soundInfo->fileId);
    fileReference->image = gabi::ea(file);
    fileReference->itemId = sound->soundId;
    if (file == NULL) {
        if (!mayDefer) return SoundStart_NoWaveFile;
        defer = 1;
    } else if (wave == NULL) {
        if (override != NULL) {
            wave = override->waveFile;
        } else {
            ArchiveAddressCache* cache = addressCache;
            u32 index = info->waveIndex;
            SoundArchive* image = archive;
            ArchiveFileLoader* loader = cache != NULL ? &cache->loader : NULL;
            wave = resolveWaveSoundWave(file, index, image, loader);
        }
        if (wave == NULL) {
            if (!mayDefer) return SoundStart_NoWave;
            defer = 1;
        }
    }
    u32 index = info->waveIndex;
    if (offsetKind == StartOffset_Milliseconds) {
        start->offset = offset;
        start->offsetUnit = OffsetInMilliseconds;
        start->waveIndex = index;
    } else {
        start->offsetUnit = OffsetInSamples;
        start->waveIndex = index;
        start->offset = offsetKind == StartOffset_Samples ? offset : 0;
    }
    if (defer) {
        SoundArchive* image = archive;
        ArchiveAddressCache* cache = addressCache;
        request->file = fileReference.get();
        request->addressCache = cache;
        SoundPlayer* soundPlayer = sound->soundPlayer;
        request->archive = image;
        request->player = soundPlayer;
        waveSound->sendConfiguration(request.get(), start.get());
    } else {
        s32 waveType = override != NULL ? (s8)override->waveType : 0;
        waveSound->sendSetup(gabi::ea(file), gabi::ea(wave), start.get(), waveType);
    }
    u32 priorityFix = info->priorityFix;
    u32 priority = info->priority;
    waveSound->sendPriority(priority, priorityFix);
    return SoundStart_Success;
}

/* WWHD glue
 str 10187FD0 "wsd"
 str 10187FE0 "bxwsd"
 str 10187FF8 "bxwar"
 ext 0289D018 const void* nw::snd::resolveWaveSoundWave(const void*,u32,nw::snd::SoundArchive*,nw::snd::ArchiveFileLoader*)
 ext 0289FF2C void nw::snd::WaveSound::sendConfiguration(const nw::snd::WaveSoundLoadRequest*,const nw::snd::WaveSoundStart*)
 ext 0289FE9C void nw::snd::WaveSound::sendSetup(u32,u32,const nw::snd::WaveSoundStart*,u32)
 ext 028A00BC void nw::snd::WaveSound::sendPriority(u32,u32)
*/

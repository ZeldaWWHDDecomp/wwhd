/* NW-SND#6: the sound-start interface's convenience entries.
 * Written from WWHD's instructions; verified by tools/verify (spec lib2_nw_snd_6). */
#include "nw/snd/SoundStartFacade.h"
#include "nw/snd/ArchiveWaveStart.h"
using namespace nw::snd;

/* 02897C94 */
u32 SoundStarter::startSound(SoundHandle* handle, u32 id, SoundStartInfo* info) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 result = startSoundVirtual(handle, id, 0, info);
    if (result != SoundStart_Success) return result;
    if (handle->sound != NULL) handle->sound->startPrepared();
    return SoundStart_Success;
}

/* 02897D08 */
u32 SoundStarter::startSoundByLabel(SoundHandle* handle, const char* label, SoundStartInfo* info) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 id = findLabelVirtual(label);
    if (id == InvalidSoundId) return SoundStart_NoLabel;
    u32 result = startSoundVirtual(handle, id, 0, info);
    if (result != SoundStart_Success) return result;
    if (handle->sound != NULL) handle->sound->startPrepared();
    return SoundStart_Success;
}

/* 02897DE4 */
u32 SoundStarter::holdSound(SoundHandle* handle, u32 id, SoundStartInfo* info) {
    gabi::AutoFrame frame{gabi::current_function()};
    BasicSound* sound = handle->sound;
    if (sound != NULL && (u32)sound->soundId == id) {
        sound->setAutoStopCounter(1);
    } else {
        u32 result = startSoundVirtual(handle, id, 1, info);
        if (result != SoundStart_Success) return result;
        if (handle->sound != NULL) handle->sound->startPrepared();
        handle->sound->setAutoStopCounter(1);
    }
    if (info != NULL && ((u32)info->flags & SoundStart_FadeOut)) {
        BasicSound* attached = handle->sound;
        s32 fadeFrames = info->fadeFrames;
        if (attached != NULL) handle->sound->stop(fadeFrames);
    }
    return SoundStart_Success;
}

/* 02897F14 */
u32 SoundStarter::prepareSound(SoundHandle* handle, u32 id, SoundStartInfo* info) {
    gabi::AutoFrame frame{gabi::current_function()};
    return startSoundVirtual(handle, id, 0, info);
}

/* 02897F48 */
u32 SoundStarter::prepareSoundByLabel(SoundHandle* handle, const char* label, SoundStartInfo* info) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 id = findLabelVirtual(label);
    if (id == InvalidSoundId) return SoundStart_NoLabel;
    return prepareSound(handle, id, info);
}

/* WWHD glue
 ext 028850D4 void nw::snd::BasicSound::startPrepared()
 ext 028853F4 void nw::snd::BasicSound::setAutoStopCounter(s32)
*/

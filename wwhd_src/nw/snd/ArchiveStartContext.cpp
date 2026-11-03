/* NW-SND#6: the archive player's optional resource hook (queries and the bracket around a start).
 * Written from WWHD's instructions; verified by tools/verify (spec lib2_nw_snd_6). */
#include "nw/snd/ArchiveStartContext.h"
using namespace nw::snd;

/* 02895064 */
u32 ArchivePlayback::isHookEnabled() {
    gabi::AutoFrame frame{gabi::current_function()};
    ArchiveResourceHook* resource = resourceHook;
    return resource != NULL && resource->enabled != 0;
}

/* 0289508C */
void ArchivePlayback::beginHookedStart() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (isHookEnabled()) resourceHook->beginStart();
}

/* 0289526C */
void ArchivePlayback::endHookedStart() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (isHookEnabled()) resourceHook->endStart();
}

/* 028950CC */
u32 ArchivePlayback::hasSequenceFile(BasicSound* sound) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sound == NULL) return 0;
    const char* label = archive->itemLabel(sound->soundId);
    if (label == NULL) return 0;
    ArchiveResourceHook* resource = resourceHook;
    if (resource->enabled == 0) return 0;
    return resource->findFile(label, "seq", "bxseq", 0) != NULL;
}

/* 028951C4 */
u32 ArchivePlayback::hasWaveSoundFile(BasicSound* sound) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sound == NULL) return 0;
    const char* label = archive->itemLabel(sound->soundId);
    if (label == NULL) return 0;
    ArchiveResourceHook* resource = resourceHook;
    if (resource->enabled == 0) return 0;
    return resource->findFile(label, "wsd", "bxwsd", 0) != NULL;
}

/* 02895174 */
u32 ArchivePlayback::hasSoundLabel(BasicSound* sound) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sound == NULL) return 0;
    return archive->itemLabel(sound->soundId) != NULL;
}

/* WWHD glue
 str 10187FD4 "seq"
 str 10187FE8 "bxseq"
 str 10187FD0 "wsd"
 str 10187FE0 "bxwsd"
 ext 0288E914 const char* nw::snd::SoundArchive::itemLabel(u32)
*/

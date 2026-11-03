/* NW-SND#6: sound allocation from the archive player's pools, and the work pools' collection of
 * finished entries. Written from WWHD's instructions; verified by tools/verify (spec lib2_nw_snd_6). */
#include "nw/snd/ArchiveSoundPool.h"
using namespace nw::snd;

/* 0289608C */
void ManagedPlaybackPool::collectFinishedSequences() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<SoundListIterator> position(8);
    SoundListNode* node = loading.next;
    SoundListNode* end = &loading;
    while (node != end) {
        SoundListNode* current = node;
        SequenceLoadTask* task = nodeOwner<SequenceLoadTask>(current, offsetof(SequenceLoadTask, listNode));
        node = node->next;
        if (task->isPending() == 0) {
            task->releaseHeapCallback();
            removeSoundList(&loadingCount, current);
            position->node = &available;
            insertSoundList(&availableCount, position.get(), current);
        }
    }
}

/* 0289612C */
void ManagedPlaybackPool::collectFinishedWaves() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<SoundListIterator> position(8);
    SoundListNode* node = loading.next;
    SoundListNode* end = &loading;
    while (node != end) {
        SoundListNode* current = node;
        WaveSoundLoadTask* entry = nodeOwner<WaveSoundLoadTask>(current, offsetof(WaveSoundLoadTask, mLink));
        node = node->next;
        if (entry->isPending() == 0) {
            entry->releaseAlias();
            removeSoundList(&loadingCount, current);
            position->node = &available;
            insertSoundList(&availableCount, position.get(), current);
        }
    }
}

/* 028961CC */
void emptyWorkPoolHook() {
    gabi::AutoFrame frame{gabi::current_function()};
}

/* The three allocators are copies of one inline function (sequence, stream and wave sounds). A free
 * sound is taken; with none free, the lowest live sound is stopped if the new one's priority is not
 * lower. The new sound joins the live list behind every sound of lower or equal priority. */
static inline BasicSound* soundOfPoolLink(const void* node) {
    return nodeOwner<BasicSound>(node, offsetof(BasicSound, poolLink));
}

/* 028961D0 */
BasicSound* ArchivePlayback::allocateSequenceSound(PlaybackPool* pool, u32 id, s32 priority, s32 ambientPriority,
                                                    const AmbientInfo* ambient) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 incoming = clampedPriority((s32)((u32)priority + (u32)ambientPriority));
    gabi::FrameLocal<SoundListIterator> freePosition(8);
    gabi::FrameLocal<SoundListIterator> livePosition(12);
    BasicSound* sound;
    for (;;) {
        while (pool->freeCount == 0) {
            if (pool->activeCount == 0) return NULL;
            BasicSound* lowest = soundOfPoolLink(pool->active.next);
            if (lowest == NULL) return NULL;
            if (incoming < soundPriority(lowest)) return NULL;
            lowest->stop(0);
        }
        SoundListNode* node = pool->free.next;
        sound = soundOfPoolLink(node);
        freePosition->node = node;
        removeArchiveListIterator(&pool->freeCount, freePosition.get());
        if (sound != NULL) break;
    }
    initializeSoundVirtual(sound);
    sound->setPriority(priority, ambientPriority);
    SoundListNode* node = pool->active.next;
    SoundListNode* end = &pool->active;
    while (node != end) {
        if (incoming < soundPriority(soundOfPoolLink(node))) break;
        node = node->next;
    }
    livePosition->node = node;
    insertSoundList(&pool->activeCount, livePosition.get(), &sound->poolLink);
    sound->setId(id);
    if (ambient != NULL) sound->setAmbientInfo(ambient);
    return sound;
}

/* 028963A0 */
BasicSound* ArchivePlayback::allocateStreamSound(PlaybackPool* pool, u32 id, s32 priority, s32 ambientPriority,
                                                  const AmbientInfo* ambient) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 incoming = clampedPriority((s32)((u32)priority + (u32)ambientPriority));
    gabi::FrameLocal<SoundListIterator> freePosition(8);
    gabi::FrameLocal<SoundListIterator> livePosition(12);
    BasicSound* sound;
    for (;;) {
        while (pool->freeCount == 0) {
            if (pool->activeCount == 0) return NULL;
            BasicSound* lowest = soundOfPoolLink(pool->active.next);
            if (lowest == NULL) return NULL;
            if (incoming < soundPriority(lowest)) return NULL;
            lowest->stop(0);
        }
        SoundListNode* node = pool->free.next;
        sound = soundOfPoolLink(node);
        freePosition->node = node;
        removeArchiveListIterator(&pool->freeCount, freePosition.get());
        if (sound != NULL) break;
    }
    initializeSoundVirtual(sound);
    sound->setPriority(priority, ambientPriority);
    SoundListNode* node = pool->active.next;
    SoundListNode* end = &pool->active;
    while (node != end) {
        if (incoming < soundPriority(soundOfPoolLink(node))) break;
        node = node->next;
    }
    livePosition->node = node;
    insertSoundList(&pool->activeCount, livePosition.get(), &sound->poolLink);
    sound->setId(id);
    if (ambient != NULL) sound->setAmbientInfo(ambient);
    return sound;
}

/* 02896570 */
BasicSound* ArchivePlayback::allocateWaveSound(PlaybackPool* pool, u32 id, s32 priority, s32 ambientPriority,
                                                const AmbientInfo* ambient) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 incoming = clampedPriority((s32)((u32)priority + (u32)ambientPriority));
    gabi::FrameLocal<SoundListIterator> freePosition(8);
    gabi::FrameLocal<SoundListIterator> livePosition(12);
    BasicSound* sound;
    for (;;) {
        while (pool->freeCount == 0) {
            if (pool->activeCount == 0) return NULL;
            BasicSound* lowest = soundOfPoolLink(pool->active.next);
            if (lowest == NULL) return NULL;
            if (incoming < soundPriority(lowest)) return NULL;
            lowest->stop(0);
        }
        SoundListNode* node = pool->free.next;
        sound = soundOfPoolLink(node);
        freePosition->node = node;
        removeArchiveListIterator(&pool->freeCount, freePosition.get());
        if (sound != NULL) break;
    }
    initializeSoundVirtual(sound);
    sound->setPriority(priority, ambientPriority);
    SoundListNode* node = pool->active.next;
    SoundListNode* end = &pool->active;
    while (node != end) {
        if (incoming < soundPriority(soundOfPoolLink(node))) break;
        node = node->next;
    }
    livePosition->node = node;
    insertSoundList(&pool->activeCount, livePosition.get(), &sound->poolLink);
    sound->setId(id);
    if (ambient != NULL) sound->setAmbientInfo(ambient);
    return sound;
}

/* WWHD glue
 ext 0288A86C u32 nw::snd::SequenceLoadTask::isPending()
 ext 0288A8A8 void nw::snd::SequenceLoadTask::releaseHeapCallback()
 ext 028A0C10 u32 nw::snd::WaveSoundLoadTask::isPending()
 ext 028A0C4C void nw::snd::WaveSoundLoadTask::releaseAlias()
 ext 028858F4 void nw::snd::BasicSound::setPriority(s32,s32)
 ext 028856EC void nw::snd::BasicSound::setId(u32)
 ext 02885C90 void nw::snd::BasicSound::setAmbientInfo(const nw::snd::AmbientInfo*)
*/

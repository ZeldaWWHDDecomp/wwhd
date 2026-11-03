/* NW-SND#6: the sound player: its sound lists, sound limit, priority order, output settings and
 * the heaps it lends. Written from WWHD's instructions; verified by tools/verify (spec lib2_nw_snd_6). */
#include "nw/snd/SoundPlayerControl.h"
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
using namespace nw::snd;

static inline BasicSound* soundOfPlayerLink(const void* node) {
    return nodeOwner<BasicSound>(node, offsetof(BasicSound, playerLink));
}
static inline BasicSound* soundOfPriorityLink(const void* node) {
    return nodeOwner<BasicSound>(node, offsetof(BasicSound, priorityLink));
}

/* 028972D4 */
void SoundPlayer::stopAll(s32 fadeFrames) {
    gabi::AutoFrame frame{gabi::current_function()};
    SoundListNode* node = sounds.root.next;
    SoundListNode* end = &sounds.root;
    while (node != end) {
        BasicSound* sound = soundOfPlayerLink(node);
        node = node->next;
        sound->stop(fadeFrames);
    }
}

/* 02897640 */
void SoundPlayer::setVolume(f32 value) {
    gabi::AutoFrame frame{gabi::current_function()};
    volume = value >= 0.0f ? value : 0.0f;
}

/* 02897654 */
void SoundPlayer::setDefaultOutputLine(u32 line) {
    gabi::AutoFrame frame{gabi::current_function()};
    defaultOutputLine = line;
}

/* 0289765C */
void SoundPlayer::setOutputVolume(u32 output, f32 value) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 clamped = value >= 0.0f ? value : 0.0f;
    if (output == 0) outputs[0].volume = clamped;
    else if (output == 1) outputs[1].volume = clamped;
}

/* 02897688 */
f32 SoundPlayer::remoteVolume(s32 remote) {
    gabi::AutoFrame frame{gabi::current_function()};
    // the index is unchecked: the element address wraps in 32 bits as the original computes it
    return *gabi::at<be<f32>>(gabi::ea(remoteVolumes) + (u32)remote * 4u);
}

/* 02897720 */
void SoundPlayer::setMaximumSounds(s32 count) {
    gabi::AutoFrame frame{gabi::current_function()};
    maximumSounds = count;
}

/* 02897728 */
u32 SoundPlayer::canStart(s32 priority) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 limit = soundLimit;
    if (limit == 0) return 0;
    if ((s32)(u32)sounds.count < limit) return 1;
    BasicSound* lowest = soundOfPriorityLink(prioritySounds.root.next);
    if (lowest == NULL) return 0;
    s32 ambient = lowest->ambient.priority;
    u32 own = (u8)lowest->playerPriority;
    s32 total = (s32)(own + (u32)ambient);
    u32 rejectEqual = rejectEqualPriority;
    s32 lowestPriority = clampedPriority(total);
    if (rejectEqual != 0) return priority > lowestPriority;
    return priority >= lowestPriority;
}

/* 02897A58 */
void SoundPlayer::removePriorityLink(BasicSound* sound) {
    gabi::AutoFrame frame{gabi::current_function()};
    removeSoundList(&prioritySounds, &sound->priorityLink);
}

/* 02897A64 */
void SoundPlayer::removeSoundLink(BasicSound* sound) {
    gabi::AutoFrame frame{gabi::current_function()};
    removeSoundList(&sounds, &sound->playerLink);
    sound->detachSoundPlayer(this);
}

/* 02897AAC */
void SoundPlayer::removeSound(BasicSound* sound) {
    gabi::AutoFrame frame{gabi::current_function()};
    removePriorityLink(sound);
    removeSoundLink(sound);
}

/* 028977BC */
void SoundPlayer::insertByPriority(BasicSound* sound) {
    gabi::AutoFrame frame{gabi::current_function()};
    SoundListNode* node = prioritySounds.root.next;
    SoundListNode* end = &prioritySounds.root;
    u32 rejectEqual = rejectEqualPriority;
    if (node != end) {
        s32 incoming = soundPriority(sound);
        do {
            s32 existing = soundPriority(soundOfPriorityLink(node));
            if (rejectEqual ? incoming <= existing : incoming < existing) break;
            node = node->next;
        } while (node != end);
    }
    gabi::FrameLocal<SoundListIterator> position(8);
    gabi::FrameLocal<SoundListIterator> positionCopy(12);
    positionCopy->node = node;
    position->node = node;
    insertSoundList(&prioritySounds, position.get(), &sound->priorityLink);
}

/* 02897AF0 */
void SoundPlayer::updatePriority(BasicSound* sound) {
    gabi::AutoFrame frame{gabi::current_function()};
    removePriorityLink(sound);
    insertByPriority(sound);
}

/* The GHS list constructor, inlined with its NULL-allocation path. */
/* inlined into 02897134 */
static inline void constructPlayerList(SoundPoolList* member) {
    SoundPoolList* list = wrappedAddress(member);
    if (list == NULL) list = (SoundPoolList*)utAllocate(sizeof(SoundPoolList));
    if (list != NULL) {
        list->count = 0;
        list->root.previous = &list->root;
        list->root.next = &list->root;
    }
}

/* 02897134 */
SoundPlayer* SoundPlayer::create(SoundPlayer* player) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (player == NULL) player = (SoundPlayer*)utAllocate(sizeof(SoundPlayer));
    if (player != NULL) {
        constructPlayerList(&player->sounds);
        constructPlayerList(&player->prioritySounds);
        constructPlayerList(&player->freeHeaps);
        constructPlayerList(&player->busyHeaps);
        player->heapCount = 0;
        player->rejectEqualPriority = 0;
        player->biquadValue = 0.0f;
        player->soundLimit = 1;
        player->biquadType = -1;
        player->maximumSounds = 0x7FFFFFFF;
        player->defaultOutputLine = 1;
        player->lpfFrequency = 0.0f;
        player->volume = 1.0f;
        for (u32 i = 0; i < 4; ++i) player->remoteVolumes[i] = 1.0f;
        for (u32 i = 1; i < 3; ++i) {
            player->outputs[i].volume = 1.0f;
            player->outputs[i].mainSend = 0.0f;
            for (u32 j = 0; j < 3; ++j) player->outputs[i].fxSend[j] = 0.0f;
        }
        player->outputs[0].volume = 1.0f;
        player->outputs[0].mainSend = 0.0f;
        for (u32 i = 0; i < 3; ++i) player->outputs[0].fxSend[i] = 0.0f;
    }
    return player;
}

/* 02897338 */
void SoundPlayer::collectFreeHeaps() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<SoundListIterator> position(8);
    SoundListNode* node = busyHeaps.root.next;
    SoundListNode* end = &busyHeaps.root;
    while (node != end) {
        SoundListNode* current = node;
        SoundMemoryArena* heap = nodeOwner<SoundMemoryArena>(current, offsetof(SoundMemoryArena, mLink));
        u32 state = heap->mState;
        node = node->next;
        if (state == PlayerHeap_Free) {
            removeSoundList(&busyHeaps, current);
            position->node = &freeHeaps.root;
            insertSoundList(&freeHeaps, position.get(), current);
        }
    }
}

/* 028975CC */
void SoundPlayer::pauseAll(u32 paused, s32 fadeFrames) {
    gabi::AutoFrame frame{gabi::current_function()};
    SoundListNode* node = sounds.root.next;
    SoundListNode* end = &sounds.root;
    while (node != end) {
        SoundListNode* current = node;
        node = node->next;
        soundOfPlayerLink(current)->pause(paused, fadeFrames);
    }
}

/* 02897698 */
void SoundPlayer::setSoundLimit(s32 limit) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 maximum = maximumSounds;
    s32 count = (s32)(u32)sounds.count;
    s32 clamped = limit > maximum ? maximum : limit < 0 ? 0 : limit;
    soundLimit = clamped;
    while (count > clamped) {
        soundOfPriorityLink(prioritySounds.root.next)->callFinalize();
        count = (s32)(u32)sounds.count;
        clamped = soundLimit;
    }
}

/* 02897B34 */
void SoundPlayer::addHeap(SoundMemoryArena* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<SoundListIterator> position(8);
    heap->mPlayer = this;
    position->node = &freeHeaps.root;
    insertSoundList(&freeHeaps, position.get(), &heap->mLink);
    heapCount = (u32)heapCount + 1;
}

/* 02897B84 */
SoundMemoryArena* SoundPlayer::takeHeap() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (freeHeaps.count == 0) return NULL;
    SoundListNode* node = freeHeaps.root.next;
    SoundMemoryArena* heap = nodeOwner<SoundMemoryArena>(node, offsetof(SoundMemoryArena, mLink));
    gabi::FrameLocal<SoundListIterator> position(8);
    position->node = node;
    removeArchiveListIterator(&freeHeaps, position.get());
    return heap;
}

/* 02897BE8 */
void SoundPlayer::returnHeap(SoundMemoryArena* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<SoundListIterator> position(8);
    if (heap->mState == PlayerHeap_Free) {
        position->node = &freeHeaps.root;
        insertSoundList(&freeHeaps, position.get(), &heap->mLink);
    } else {
        position->node = &busyHeaps.root;
        insertSoundList(&busyHeaps, position.get(), &heap->mLink);
    }
}

/* 02897C4C */
static PlaybackPriorityBucket* constructPlayerPriorityBucket(PlaybackPriorityBucket* bucket) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (bucket == NULL) bucket = (PlaybackPriorityBucket*)utAllocate(sizeof(PlaybackPriorityBucket));
    if (bucket != NULL) {
        bucket->count = 0;
        bucket->sentinel.previous = &bucket->sentinel;
        bucket->sentinel.next = &bucket->sentinel;
    }
    return bucket;
}

/* 028978F8 */
u32 SoundPlayer::insertSound(BasicSound* sound) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 ambient = (u32)(s32)sound->ambient.priority;
    u32 own = (u8)sound->playerPriority;
    s32 incoming = clampedPriority((s32)(own + ambient));
    s32 limit = soundLimit;
    if (limit == 0) return 0;
    s32 count = (s32)(u32)sounds.count;
    while (count >= limit) {
        BasicSound* lowest = soundOfPriorityLink(prioritySounds.root.next);
        if (lowest == NULL) return 0;
        u32 rejectEqual = rejectEqualPriority;
        u32 lowestAmbient = (u32)(s32)lowest->ambient.priority;
        u32 lowestOwn = (u8)lowest->playerPriority;
        s32 existing = clampedPriority((s32)(lowestOwn + lowestAmbient));
        if (rejectEqual ? incoming <= existing : incoming < existing) return 0;
        lowest->callFinalize();
        count = (s32)(u32)sounds.count;
        limit = soundLimit;
    }
    gabi::FrameLocal<SoundListIterator> position(8);
    position->node = &sounds.root;
    insertSoundList(&sounds, position.get(), &sound->playerLink);
    insertByPriority(sound);
    sound->attachSoundPlayer(this);
    return 1;
}

/* Sorting scratch: one list per priority, constructed on first use. */
static PlaybackPriorityBuckets sPriorityBuckets;
static u32 sPriorityBucketsConstructed;

/* 028973C8 */
void SoundPlayer::sortByPriority(u32 newestFirst) {
    gabi::AutoFrame frame{gabi::current_function()};
    if ((u32)prioritySounds.count < 2) return;
    if (sPriorityBucketsConstructed == 0) {
        sPriorityBucketsConstructed = 1;
        __construct_array(sPriorityBuckets.entries, 128, sizeof(PlaybackPriorityBucket), (void* (*)(void*))constructPlayerPriorityBucket);
    }
    gabi::FrameLocal<SoundListIterator> removal(8);
    gabi::FrameLocal<SoundListIterator> insertion(12);
    gabi::FrameLocal<SoundListIterator> lastCopy(0x10);
    gabi::FrameLocal<SoundListIterator> lastRemoval(0x14);
    gabi::FrameLocal<SoundListIterator> lastInsertion(0x18);
    gabi::FrameLocal<SoundListIterator> firstRemoval(0x1C);
    gabi::FrameLocal<SoundListIterator> firstInsertion(0x20);
    // distribute the sounds over the buckets, keeping their order within a priority
    while (prioritySounds.count != 0) {
        SoundListNode* node = prioritySounds.root.next;
        BasicSound* sound = soundOfPriorityLink(node);
        removal->node = node;
        removeArchiveListIterator(&prioritySounds, removal.get());
        u32 ambient = (u32)(s32)sound->ambient.priority;
        u32 own = (u8)sound->playerPriority;
        PlaybackPriorityBucket* bucket = &sPriorityBuckets.entries[clampedPriority((s32)(own + ambient))];
        insertion->node = &bucket->sentinel;
        insertSoundList(bucket, insertion.get(), &sound->priorityLink);
    }
    // collect them back in ascending priority; within a priority newest first or oldest first
    SoundListNode* end = &prioritySounds.root;
    for (u32 i = 0; i < 128; ++i) {
        PlaybackPriorityBucket* bucket = &sPriorityBuckets.entries[i];
        while (bucket->count != 0) {
            if (newestFirst) {
                SoundListNode* node = bucket->sentinel.previous;
                lastRemoval->node = node;
                lastCopy->node = node;
                removeArchiveListIterator(bucket, lastRemoval.get());
                lastInsertion->node = end;
                insertSoundList(&prioritySounds, lastInsertion.get(), node);
            } else {
                SoundListNode* node = bucket->sentinel.next;
                firstRemoval->node = node;
                removeArchiveListIterator(bucket, firstRemoval.get());
                firstInsertion->node = end;
                insertSoundList(&prioritySounds, firstInsertion.get(), node);
            }
        }
    }
}

/* 0289755C */
void SoundPlayer::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    collectFreeHeaps();
    SoundListNode* node = sounds.root.next;
    SoundListNode* end = &sounds.root;
    while (node != end) {
        SoundListNode* current = node;
        node = node->next;
        soundOfPlayerLink(current)->update();
    }
    sortByPriority(0);
}

/* WWHD glue
 data sPriorityBuckets 104B720C
 data sPriorityBucketsConstructed 104B780C
 ext 02883FA8 void nw::snd::BasicSound::stop(s32)
 ext 0288486C void nw::snd::BasicSound::update()
 ext 028850E0 void nw::snd::BasicSound::pause(u32,s32)
 ext 02885C60 void nw::snd::BasicSound::attachSoundPlayer(nw::snd::SoundPlayer*)
 ext 02885C68 void nw::snd::BasicSound::detachSoundPlayer(nw::snd::SoundPlayer*)
 ext 0286DA70 nw::snd::SoundListNode* insertSoundList(void*,nw::snd::SoundListIterator*,nw::snd::SoundListNode*)
 ext 0286DA9C nw::snd::SoundListNode* removeSoundList(void*,nw::snd::SoundListNode*)
 ext 0286DB18 nw::snd::SoundListNode* removeArchiveListIterator(void*,nw::snd::SoundListIterator*)
*/

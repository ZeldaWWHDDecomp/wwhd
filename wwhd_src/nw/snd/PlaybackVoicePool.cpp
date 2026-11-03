/* Playback voice pool: the free list, the active list kept in ascending priority order,
 * voice stealing, and the per-frame passes over the active voices, reconstructed from WWHD. */
#include "nw/snd/PlaybackVoice.h"
using namespace nw::snd;
using nw::ut::HeapChildren;
using nw::ut::HeapLink;
static PlaybackVoicePool sPlaybackVoicePool;
static u32 sPlaybackVoicePoolGuard;
void* allocatePlaybackPool(u32 bytes);
void constructPlaybackArray(void* storage, u32 count, u32 stride, u32 constructor);
HeapLink* insertPlaybackLink(HeapChildren* list, gptr<HeapLink>* position, HeapLink* node);
HeapLink* removePlaybackLink(HeapChildren* list, HeapLink* node);
HeapLink* erasePlaybackLink(HeapChildren* list, gptr<HeapLink>* position);
static PlaybackVoice* voiceOf(HeapLink* link);
static void initializeList(HeapChildren* list);

/* 0289F210 */
ChannelMixState* nw::snd::constructVoiceMixState(ChannelMixState* state) {
    gabi::AutoFrame frame{0x0289F210u};
    if (!state) state = static_cast<ChannelMixState*>(allocatePlaybackPool(sizeof(ChannelMixState)));
    if (!state) return state;
    state->pitch = 1.0f;
    state->auxiliarySend = 0.0f;
    state->pan = 0.0f;
    state->mainSend = 0.0f;
    state->gain = 1.0f;
    state->surroundPan = 0.0f;
    return state;
}

/* 0289F26C */
ChannelMixGroup* nw::snd::constructVoiceMixGroup(ChannelMixGroup* group) {
    gabi::AutoFrame frame{0x0289F26Cu};
    if (!group) group = static_cast<ChannelMixGroup*>(allocatePlaybackPool(sizeof(ChannelMixGroup)));
    if (group) constructPlaybackArray(group->channels, 2, sizeof(ChannelMixState), gabi::guest_fn((const void*)&constructVoiceMixState));
    return group;
}

/* 0289F2C4 */
PlaybackVoicePool* PlaybackVoicePool::construct(PlaybackVoicePool* pool) {
    gabi::AutoFrame frame{0x0289F2C4u};
    if (!pool) pool = static_cast<PlaybackVoicePool*>(allocatePlaybackPool(sizeof(PlaybackVoicePool)));
    if (!pool) return pool;
    pool->mInitialized = 0;
    // The inlined list constructors test the member address for NULL; keep it a guest address.
    initializeList(gabi::at<HeapChildren>(gabi::ea(pool) + offsetof(PlaybackVoicePool, mActive)));
    initializeList(gabi::at<HeapChildren>(gabi::ea(pool) + offsetof(PlaybackVoicePool, mFree)));
    return pool;
}

/* 0289F364 */
PlaybackVoicePool* PlaybackVoicePool::instance() {
    gabi::AutoFrame frame{0x0289F364u};
    if (!sPlaybackVoicePoolGuard) {
        sPlaybackVoicePoolGuard = 1;
        construct(&sPlaybackVoicePool);
    }
    return &sPlaybackVoicePool;
}

/* 0289F3B0 */
u32 PlaybackVoicePool::requiredBytes(u32 count) const { return count * sizeof(PlaybackVoice); }

/* 0289F3B8 */
void PlaybackVoicePool::initialize(PlaybackVoice* storage, u32 bytes) {
    gabi::AutoFrame frame{0x0289F3B8u};
    if (mInitialized) return;
    for (u32 count = bytes / sizeof(PlaybackVoice); count; --count) {
        PlaybackVoice* voice = storage;
        if (voice) voice = PlaybackVoice::construct(voice);
        gabi::FrameLocal<gptr<HeapLink>> position{8};
        *position = &mFree.root;
        insertPlaybackLink(&mFree, position.get(), &voice->mPoolLink);
        storage = gabi::at<PlaybackVoice>(gabi::ea(storage) + sizeof(PlaybackVoice));
    }
    mInitialized = 1;
}

/* 0289F45C */
void PlaybackVoicePool::stopAll() {
    gabi::AutoFrame frame{0x0289F45Cu};
    while (mActive.count) {
        PlaybackVoice* voice = voiceOf(mActive.root.next);
        voice->stop();
        const u32 callback = voice->mCallback;
        if (callback) gabi::call_ptr(callback, voice, u32(PlaybackVoice::EventStopped), u32(voice->mCallbackArgument));
        voice->release();
    }
}

/* 0289F4E4 */
void PlaybackVoicePool::finalize() {
    gabi::AutoFrame frame{0x0289F4E4u};
    if (!mInitialized) return;
    stopAll();
    while (mFree.count) {
        HeapLink* link = mFree.root.next;
        PlaybackVoice* voice = voiceOf(link);
        gabi::FrameLocal<gptr<HeapLink>> position{8};
        *position = link;
        erasePlaybackLink(&mFree, position.get());
        voice->destroy(2);
    }
    mInitialized = 0;
}

/* 0289F56C */
u32 PlaybackVoicePool::stealVoice(s32 priority) {
    gabi::AutoFrame frame{0x0289F56Cu};
    if (!mActive.count) return 0;
    PlaybackVoice* voice = voiceOf(mActive.root.next);
    if (voice->mPriority > priority) return 0;
    const u32 callback = voice->mCallback;
    const u32 argument = voice->mCallbackArgument;
    const u32 channels = voice->mChannelCount;
    voice->stop();
    voice->release();
    if (callback) gabi::call_ptr(callback, voice, u32(PlaybackVoice::EventStolen), argument);
    return channels;
}

/* 0289F628 */
void PlaybackVoicePool::insertByPriority(PlaybackVoice* voice) {
    gabi::AutoFrame frame{0x0289F628u};
    HeapLink* first = mActive.root.next;
    HeapLink* node = &mActive.root;
    gabi::FrameLocal<gptr<HeapLink>> selected{0xC};
    *selected = node;
    // Walk back from the end to the first voice whose priority is not above the new one.
    if (node != first) {
        const s32 priority = voice->mPriority;
        do {
            if (voiceOf(node->previous)->mPriority <= priority) break;
            node = node->previous;
            *selected = node;
        } while (node != first);
    }
    gabi::FrameLocal<gptr<HeapLink>> position{0x10};
    *position = *selected;
    insertPlaybackLink(&mActive, position.get(), &voice->mPoolLink);
}

/* 0289F694 */
PlaybackVoice* PlaybackVoicePool::acquire(s32 channels, u32 priority, u32 callback, u32 argument, u32 allocationMode) {
    gabi::AutoFrame frame{0x0289F694u};
    if (!mFree.count && !stealVoice(s32(priority))) return nullptr;
    PlaybackVoice* voice = voiceOf(mFree.root.next);
    if (!voice->acquire(channels, priority, callback, argument, allocationMode)) return nullptr;
    voice->mPriority = priority & 0xFFu;
    removePlaybackLink(&mFree, &voice->mPoolLink);
    insertByPriority(voice);
    return voice;
}

/* 0289F758 */
void PlaybackVoicePool::release(PlaybackVoice* voice) {
    gabi::AutoFrame frame{0x0289F758u};
    removePlaybackLink(&mActive, &voice->mPoolLink);
    gabi::FrameLocal<gptr<HeapLink>> position{8};
    *position = &mFree.root;
    insertPlaybackLink(&mFree, position.get(), &voice->mPoolLink);
}

/* 0289F7B0 */
void PlaybackVoicePool::releaseVoice(PlaybackVoice* voice) {
    gabi::AutoFrame frame{0x0289F7B0u};
    release(voice);
}

/* 0289F7B4 */
void PlaybackVoicePool::updateParameters() {
    gabi::AutoFrame frame{0x0289F7B4u};
    HeapLink* node = mActive.root.next;
    if (node == &mActive.root) return;
    do {
        PlaybackVoice* voice = voiceOf(node);
        node = node->next;
        voice->prepareUpdate();
    } while (node != &mActive.root);
    node = mActive.root.next;
    while (node != &mActive.root) {
        PlaybackVoice* voice = voiceOf(node);
        node = node->next;
        voice->applyUpdate();
    }
}

/* 0289F82C */
void PlaybackVoicePool::updatePlayback() {
    gabi::AutoFrame frame{0x0289F82Cu};
    HeapLink* node = mActive.root.next;
    while (node != &mActive.root) {
        PlaybackVoice* voice = voiceOf(node);
        node = node->next;
        voice->updatePlayback();
    }
}

/* 0289F880 */
void PlaybackVoicePool::markDirty(u32 flags) {
    HeapLink* node = mActive.root.next;
    while (node != &mActive.root) {
        PlaybackVoice* voice = voiceOf(node);
        node = node->next;
        if (voice->mAllocated) voice->mDirty = u16(voice->mDirty) | flags;
    }
}

static PlaybackVoice* voiceOf(HeapLink* link) {
    return gabi::at<PlaybackVoice>(gabi::ea(link) - offsetof(PlaybackVoice, mPoolLink));
}

/* inlined into PlaybackVoicePool::construct */
static void initializeList(HeapChildren* list) {
    if (!list) list = static_cast<HeapChildren*>(allocatePlaybackPool(sizeof(HeapChildren)));
    if (!list) return;
    list->count = 0;
    list->root.previous = &list->root;
    list->root.next = &list->root;
}

/* 0289F8BC */
void PlaybackVoicePool::reorder(PlaybackVoice* voice) {
    gabi::AutoFrame frame{0x0289F8BCu};
    removePlaybackLink(&mActive, &voice->mPoolLink);
    insertByPriority(voice);
}

/* WWHD glue
data sPlaybackVoicePool 104BBE70
data sPlaybackVoicePoolGuard 104BBE8C
ext 0273AD10 void* allocatePlaybackPool(u32)
ext 028EFFD0 void constructPlaybackArray(void*,u32,u32,u32)
ext 0286DA70 nw::ut::HeapLink* insertPlaybackLink(nw::ut::HeapChildren*,gptr<nw::ut::HeapLink>*,nw::ut::HeapLink*)
ext 0286DA9C nw::ut::HeapLink* removePlaybackLink(nw::ut::HeapChildren*,nw::ut::HeapLink*)
ext 0286DB18 nw::ut::HeapLink* erasePlaybackLink(nw::ut::HeapChildren*,gptr<nw::ut::HeapLink>*)
*/

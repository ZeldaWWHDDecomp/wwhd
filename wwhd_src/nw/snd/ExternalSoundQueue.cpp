/* Priority admission and eviction for a bounded sound list. Own names, binary-derived layouts. */
#include "nw/snd/ExternalSoundQueue.h"
using namespace nw::snd;

/* 0288750C */
BasicSound* PrioritySoundQueue::lowestPriority() {
    gabi::AutoFrame frame{0x0288750C};
    BasicSound* lowest = nullptr;
    s32 lowestValue = 128;
    SoundListNode* node = sentinel.next;
    while (node != &sentinel) {
        BasicSound* sound = gabi::at<BasicSound>(gabi::ea(node) - 0x28C);
        s32 priority = (s32)((u32)(u8)(s8)sound->playerPriority + (u32)(s32)sound->ambient.priority);
        if (priority > 127)
            priority = 127;
        else if (priority < 0)
            priority = 0;
        node = node->next;
        if (lowestValue > priority) {
            lowestValue = priority;
            lowest = sound;
        }
    }
    return lowest;
}

/* 02887570 */
u32 PrioritySoundQueue::canAccept(s32 priority) {
    gabi::AutoFrame frame{0x02887570};
    s32 limit = capacity;
    if (!limit) return 0;
    if (count < limit) return 1;
    BasicSound* sound = lowestPriority();
    if (!sound) return 0;
    s32 lowest = (s32)((u32)(u8)(s8)sound->playerPriority + (u32)(s32)sound->ambient.priority);
    if (lowest > 127)
        lowest = 127;
    else if (lowest < 0)
        lowest = 0;
    return priority >= lowest;
}

/* 028875F8 */
u32 PrioritySoundQueue::append(BasicSound* sound) {
    gabi::AutoFrame frame{0x028875F8};
    s32 priority = (s32)((u32)(s32)sound->ambient.priority + (u32)(u8)(s8)sound->playerPriority);
    if (priority > 127)
        priority = 127;
    else if (priority < 0)
        priority = 0;
    s32 limit = capacity;
    if (!limit) return 0;
    s32 current = count;
    while (current >= limit) {
        BasicSound* victim = lowestPriority();
        if (!victim) return 0;
        s32 lowest = (s32)((u32)(u8)(s8)victim->playerPriority + (u32)(s32)victim->ambient.priority);
        if (lowest > 127)
            lowest = 127;
        else if (lowest < 0)
            lowest = 0;
        if (priority < lowest) return 0;
        victim->callFinalize();
        current = count;
        limit = capacity;
    }
    gabi::FrameLocal<SoundListIterator> end(8);
    end->node = &sentinel;
    insertSoundList(this, end.get(), (SoundListNode*)&sound->actorLink);
    sound->attachExternalSoundPlayer((ExternalSoundPlayer*)this);
    return 1;
}

/* 02887720 */
void ExternalSoundPlayer::removeSound(BasicSound* sound) {
    gabi::AutoFrame frame{0x02887720};
    removeSoundList(this, (SoundListNode*)&sound->actorLink);
    sound->detachExternalSoundPlayer(this);
}

struct ListHead {
    be<u32> count;
    SoundListNode sentinel;
};

/* 02887768 */
SoundHeap* SoundHeap::create(SoundHeap* heap) {
    gabi::AutoFrame frame{0x02887768};
    if (!heap) {
        heap = (SoundHeap*)sndAllocate(sizeof(SoundHeap));
        if (!heap) return heap;
    }
    heap->allocator = nullptr;
    ListHead* list = (ListHead*)&heap->count;
    if (!gabi::ea(list)) { // 32-bit address test: a member address can wrap to zero
        list = (ListHead*)sndAllocate(sizeof(ListHead));
        if (!list) return heap;
    }
    list->count = 0;
    list->sentinel.previous = &list->sentinel;
    list->sentinel.next = &list->sentinel;
    return heap;
}

/* WWHD glue
ext 0286DA70 nw::snd::SoundListNode* insertSoundList(void*,nw::snd::SoundListIterator*,nw::snd::SoundListNode*)
ext 0286DA9C nw::snd::SoundListNode* removeSoundList(void*,nw::snd::SoundListNode*)
ext 02885C7C void nw::snd::BasicSound::attachExternalSoundPlayer(nw::snd::ExternalSoundPlayer*)
ext 02885C84 void nw::snd::BasicSound::detachExternalSoundPlayer(nw::snd::ExternalSoundPlayer*)
*/

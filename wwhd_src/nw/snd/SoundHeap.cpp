/* Audio heap levels and release callbacks. Reconstructed solely from WWHD's instructions. */
#include "nw/snd/ExternalSoundQueue.h"
using namespace nw::snd;

SoundListNode* eraseSoundListRange(void*, SoundListIterator*, SoundListIterator*);
void OSBlockSet(void*, u32, u32);
void sndDelete(void*);

/* 028877E0 */
u32 SoundHeap::releaseTogether(u32 level) {
    gabi::AutoFrame frame{0x028877E0};
    SoundListNode* first = sentinel.next;
    SoundListNode* position = &sentinel;
    u32 remaining = count;
    u32 end = 0;
    u32 callback = 0;
    u32 argument = 0;
    bool sameCallback = true;
    SoundHeapLevel* selected = nullptr;
    while (position != first) {
        position = position->previous;
        --remaining;
        SoundHeapLevel* current = (SoundHeapLevel*)position;
        SoundListNode* allocation = current->sentinel.next;
        if (!end && current->count) {
            SoundHeapAllocation* last = (SoundHeapAllocation*)current->sentinel.previous.get();
            end = gabi::ea(last->data.get()) + (u32)last->size;
            callback = last->callback;
            argument = last->callbackArgument;
        }
        bool mismatch = false;
        while (allocation != &current->sentinel) {
            SoundHeapAllocation* record = (SoundHeapAllocation*)allocation;
            u32 nextCallback = record->callback;
            allocation = allocation->next;
            u32 nextArgument = record->callbackArgument;
            if (callback != nextCallback || argument != nextArgument) {
                sameCallback = false;
                mismatch = true;
                break;
            }
        }
        if (remaining == level) {
            selected = current;
            break;
        }
        if (mismatch) continue;
    }
    if (!end || !sameCallback) return 0;
    if (callback) gabi::call_ptr<void>(callback, selected, end - gabi::ea(selected), argument);
    return 1;
}

/* 02887924 */
void SoundHeapLevel::destroy(u32 flags) {
    gabi::AutoFrame frame{0x02887924};
    if (!this) return;
    SoundListNode* end = &sentinel;
    SoundListNode* first = sentinel.next;
    if (releaseEnabled) {
        SoundListNode* node = end;
        while (node != first) {
            node = node->previous;
            SoundHeapAllocation* allocation = (SoundHeapAllocation*)node;
            if (allocation) {
                u32 callback = allocation->callback;
                if (callback) {
                    u32 argument = allocation->callbackArgument;
                    void* data = allocation->data;
                    u32 size = allocation->size;
                    gabi::call_ptr<void>(callback, data, size, argument);
                    first = sentinel.next;
                }
            }
        }
    }
    gabi::FrameLocal<SoundListIterator> begin(8);
    gabi::FrameLocal<SoundListIterator> finish(12);
    begin->node = first;
    finish->node = end;
    eraseSoundListRange(&count, begin.get(), finish.get());
    if (flags & 1) sndDelete(this);
}

/* 02887A10 */
void SoundHeap::clearLevels() {
    gabi::AutoFrame frame{0x02887A10};
    u32 together = releaseTogether(0);
    while (count) {
        SoundHeapLevel* level = (SoundHeapLevel*)sentinel.previous.get();
        if (together) level->releaseEnabled = 0;
        level->destroy(2);
        removeSoundList(&count, &level->link);
    }
}

/* 02887A8C */
void SoundHeap::finalize() {
    gabi::AutoFrame frame{0x02887A8C};
    if (allocator) {
        clearLevels();
        allocator->reset(3);
        allocator->destroy();
        allocator = nullptr;
    }
}

/* 02887AE4 */
void SoundHeap::destroy(u32 flags) {
    gabi::AutoFrame frame{0x02887AE4};
    if (!this) return;
    if (allocator) finalize();
    if (flags & 1) sndDelete(this);
}

struct AllocationList {
    be<u32> count;
    SoundListNode sentinel;
};

/* 02887B40 */
SoundHeapLevel* SoundHeapLevel::create(SoundHeapLevel* level) {
    gabi::AutoFrame frame{0x02887B40};
    if (!level) {
        level = (SoundHeapLevel*)sndAllocate(sizeof(SoundHeapLevel));
        if (!level) return level;
    }
    level->link.next = nullptr;
    level->link.previous = nullptr;
    AllocationList* list = (AllocationList*)&level->count;
    if (!gabi::ea(list)) list = (AllocationList*)sndAllocate(sizeof(AllocationList));
    if (list) {
        list->count = 0;
        list->sentinel.previous = &list->sentinel;
        list->sentinel.next = &list->sentinel;
    }
    SoundListNode* first = level->sentinel.next;
    level->releaseEnabled = 1;
    gabi::FrameLocal<SoundListIterator> begin(8);
    gabi::FrameLocal<SoundListIterator> end(12);
    begin->node = first;
    end->node = &level->sentinel;
    eraseSoundListRange(&level->count, begin.get(), end.get());
    return level;
}

/* 02887BE4 */
u32 SoundHeap::pushLevel() {
    gabi::AutoFrame frame{0x02887BE4};
    SoundHeapLevel* storage = (SoundHeapLevel*)allocator->allocate(sizeof(SoundHeapLevel), 4);
    if (!storage) return 0;
    OSBlockSet(storage, 0, sizeof(SoundHeapLevel));
    SoundHeapLevel* level = storage;
    if (storage) level = SoundHeapLevel::create(storage);
    gabi::FrameLocal<SoundListIterator> end(8);
    end->node = &sentinel;
    insertSoundList(&count, end.get(), &level->link);
    return 1;
}

/* 02887C84 */
u32 SoundHeap::initialize(u32 buffer, u32 size) {
    gabi::AutoFrame frame{0x02887C84};
    if (allocator) finalize();
    u32 end = buffer + size;
    u32 aligned = (buffer + 3) & ~3u;
    if (aligned > end) return 0;
    nw::ut::FrameHeap* heap = nw::ut::FrameHeap::create(aligned, end - aligned, 0);
    allocator = heap;
    if (!heap) return 0;
    return pushLevel() != 0;
}

/* 02887D3C */
void SoundHeapLevel::append(SoundHeapAllocation* allocation) {
    gabi::AutoFrame frame{0x02887D3C};
    gabi::FrameLocal<SoundListIterator> end(8);
    end->node = &sentinel;
    insertSoundList(&count, end.get(), &allocation->link);
}

/* 02887D70 */
void* SoundHeap::allocate(u32 size, u32 callback, u32 argument) {
    gabi::AutoFrame frame{0x02887D70};
    u32 rounded = (size + 255) & ~255u;
    SoundHeapAllocation* record = (SoundHeapAllocation*)allocator->allocate(rounded + 256, 256);
    if (!record) return nullptr;
    void* data = gabi::at<void>(gabi::ea(record) + 256);
    if (record) {
        record->callback = callback;
        record->data = data;
        record->link.next = nullptr;
        record->size = size;
        record->link.previous = nullptr;
        record->callbackArgument = argument;
    }
    SoundHeapLevel* level = (SoundHeapLevel*)sentinel.previous.get();
    level->append(record);
    return data;
}

/* 02887E10 */
void SoundHeap::reset() {
    gabi::AutoFrame frame{0x02887E10};
    clearLevels();
    allocator->reset(3);
    pushLevel();
}

/* 02887E50 */
u32 SoundHeap::totalSize() {
    gabi::AutoFrame frame{0x02887E50};
    nw::ut::FrameHeap* heap = allocator;
    u32 end = heap->end;
    return end - gabi::ea(heap);
}

/* WWHD glue
ext 0286DACC nw::snd::SoundListNode* eraseSoundListRange(void*,nw::snd::SoundListIterator*,nw::snd::SoundListIterator*)
ext C0009990 void OSBlockSet(void*,u32,u32)
ext 028F64C4 void* nw::ut::FrameHeap::allocate(u32,s32)
ext 028F6604 void nw::ut::FrameHeap::reset(u32)
ext 028F637C nw::ut::FrameHeap* nw::ut::FrameHeap::destroy()
ext 028F626C nw::ut::FrameHeap* nw::ut::FrameHeap::create(u32,u32,u32)
*/

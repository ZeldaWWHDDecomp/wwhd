/* Registered sound-memory observers and range dispatch. From WWHD's instructions only. */
#include "nw/snd/SoundRangeListeners.h"
using namespace nw::snd;

void* sndRangeAllocate(u32 size);
SoundRangeLink* insertRangeListener(SoundRangeListeners* list, gptr<SoundRangeLink>* position, SoundRangeLink* listener);
SoundRangeLink* unlinkRangeListener(SoundRangeListeners* list, SoundRangeLink* listener);
static SoundRangeListeners sRangeListeners;
static u32 sRangeListenersGuard;

/* 028A5A94 */
SoundRangeListeners* SoundRangeListeners::construct(SoundRangeListeners* list) {
    gabi::AutoFrame frame{0x028A5A94};
    if (!list) list = (SoundRangeListeners*)sndRangeAllocate(12);
    if (list) {
        list->count = 0;
        list->root.previous = &list->root;
        list->root.next = &list->root;
    }
    return list;
}

/* 028A5B00 */
SoundRangeListeners* SoundRangeListeners::instance() {
    gabi::AutoFrame frame{0x028A5B00};
    if (!sRangeListenersGuard) {
        sRangeListenersGuard = 1;
        construct(&sRangeListeners);
    }
    return &sRangeListeners;
}

/* 028A5B4C */
void SoundRangeListeners::notify(void* unused, const void* start, u32 size) {
    gabi::AutoFrame frame{0x028A5B4C};
    const void* end = gabi::at<const void>(gabi::ea(start) + size);
    SoundRangeListeners* list = instance();
    SoundRangeLink* listener = list->root.next;
    while (listener != &instance()->root) {
        SoundRangeListener* current = (SoundRangeListener*)listener;
        SoundRangeListenerTable* table = current->table;
        u32 callback = table->invalidateRange;
        listener = listener->next;
        gabi::call_ptr<void>(callback, current, start, end);
    }
}

/* 028A5BD0 */
SoundRangeLink* SoundRangeListeners::append(SoundRangeLink* listener) {
    gabi::AutoFrame frame{0x028A5BD0};
    gabi::FrameLocal<gptr<SoundRangeLink>> position(8);
    *position = &root;
    return insertRangeListener(this, position.get(), listener);
}

/* 028A5C00 */
SoundRangeLink* SoundRangeListeners::remove(SoundRangeLink* listener) {
    gabi::AutoFrame frame{0x028A5C00};
    return unlinkRangeListener(this, listener);
}

/* WWHD glue
 data sRangeListeners 104BC2FC
 data sRangeListenersGuard 104BC308
 ext 0273AD10 void* sndRangeAllocate(u32)
 ext 0286DA70 nw::snd::SoundRangeLink* insertRangeListener(nw::snd::SoundRangeListeners*,gptr<nw::snd::SoundRangeLink>*,nw::snd::SoundRangeLink*)
 ext 0286DA9C nw::snd::SoundRangeLink* unlinkRangeListener(nw::snd::SoundRangeListeners*,nw::snd::SoundRangeLink*)
*/

// Common sound-worker task header lifetime, from WWHD instructions.
#include "nw/snd/AudioWorkQueue.h"
using namespace nw::snd;
void* allocateAudioTask(u32);
void deleteAudioTask(void*);
struct AudioTaskTable { be<u32> words[6]; };
static AudioTaskTable sAudioTaskTable;
/* 0289C050 */
AudioWork* AudioWork::construct(AudioWork* work) {
    if(!work) work=static_cast<AudioWork*>(allocateAudioTask(0x38));
    if(work) {
        work->link.next=nullptr; work->link.previous=nullptr;
        work->vtable=reinterpret_cast<AudioWorkVtable*>(&sAudioTaskTable);
        nw::ut::Event::create(&work->finished,1,1);
        work->state=0; work->owner=0;
    }
    return work;
}
/* 0289C0C8 */
void AudioWork::destroy(u32 flags) {
    if(!this) return;
    vtable=reinterpret_cast<AudioWorkVtable*>(&sAudioTaskTable);
    finished.destroy(2);
    if(flags&1) deleteAudioTask(this);
}
/* WWHD glue
data sAudioTaskTable 101823D0
ext 0273AD10 void* allocateAudioTask(u32)
ext 0273AF40 void deleteAudioTask(void*)
ext 028F69A8 nw::ut::Event* nw::ut::Event::create(nw::ut::Event*,u32,u32)
ext 028F6A10 void nw::ut::Event::destroy(u32)
*/

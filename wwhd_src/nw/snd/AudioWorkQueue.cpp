// Three priority queues for the sound worker; own reconstruction from game instructions.
#include "nw/snd/AudioWorkQueue.h"
using namespace nw::snd;
static AudioWorkQueue sAudioWorkQueue;
static u32 sAudioWorkQueueGuard;
struct AudioWorkRegistration { be<u32> words[3]; };
static AudioWorkRegistration sAudioWorkRegistration;
using AudioWorkListConstructor=AudioWorkList* (*)(AudioWorkList*);
void constructAudioWorkArray(void*,u32,u32,AudioWorkListConstructor);
void initializeAudioWorkMutex(void*);
void registerAudioWorkStatic(void*);
void lockAudioWork(void*);
void unlockAudioWork(void*);
nw::ut::HeapLink* eraseAudioWork(nw::ut::HeapChildren*,gptr<nw::ut::HeapLink>*);
nw::ut::HeapLink* insertAudioWork(nw::ut::HeapChildren*,gptr<nw::ut::HeapLink>*,nw::ut::HeapLink*);
void* allocateAudioWork(u32);
void deleteAudioWork(void*);
void finishAudioMessages(nw::ut::MessageQueue*);

/* 0289C128 */
AudioWorkQueue* AudioWorkQueue::construct(AudioWorkQueue* queue) {
    if(!queue) queue=static_cast<AudioWorkQueue*>(allocateAudioWork(0x298));
    if(queue) {
        constructAudioWorkArray(queue,3,12,&AudioWorkList::construct);
        queue->current=nullptr;
        queue->stopRequested=0;
        queue->queueReady=1;
        initializeAudioWorkMutex(queue->mutex);
        nw::ut::MessageQueue::create(&queue->messages);
    }
    return queue;
}
/* 0289C1A4 */
AudioWorkQueue* AudioWorkQueue::instance() {
    if(sAudioWorkQueueGuard==0) {
        sAudioWorkQueueGuard=1;
        construct(&sAudioWorkQueue);
        registerAudioWorkStatic(&sAudioWorkRegistration);
    }
    return &sAudioWorkQueue;
}

/* 0289C1FC */
void AudioWorkQueue::initializeMessages() { messages.initialize(messageStorage,32); }
/* 0289C20C */
void AudioWorkQueue::finishMessages() { finishAudioMessages(&messages); }
/* 0289C214 */
AudioWork* AudioWorkQueue::select(u32 priority,u32 remove) {
    gabi::AutoFrame frame{gabi::current_function()};
    lockAudioWork(mutex);
    AudioWorkList* list=&priorities[priority];
    if(list->count==0) { unlockAudioWork(mutex); return nullptr; }
    AudioWork* work=reinterpret_cast<AudioWork*>(list->root.next.get());
    if(remove) {
        gabi::FrameLocal<gptr<nw::ut::HeapLink>> position(8);
        *position=&work->link;
        eraseAudioWork(list,position.get());
    }
    unlockAudioWork(mutex);
    return work;
}
/* 0289C2CC */
AudioWork* AudioWorkQueue::take() {
    lockAudioWork(mutex);
    AudioWork* work=select(2,1);
    if(!work) work=select(1,1);
    if(!work) work=select(0,1);
    unlockAudioWork(mutex);
    return work;
}
/* 0289C3D8 */
void AudioWorkQueue::executePending() {
    AudioWork* work=take();
    while(work) {
        current=work;
        gabi::call_ptr<void>(work->vtable->execute,work);
        work->state=3;
        work->finished.signal();
        current=nullptr;
        work=take();
    }
}
/* 0289C464 */
void AudioWorkQueue::submit(AudioWork* work,u32 priority) {
    gabi::AutoFrame frame{gabi::current_function()};
    work->finished.reset();
    work->state=1;
    lockAudioWork(mutex);
    AudioWorkList* list=&priorities[priority];
    gabi::FrameLocal<gptr<nw::ut::HeapLink>> position(8);
    *position=&list->root;
    insertAudioWork(list,position.get(),&work->link);
    unlockAudioWork(mutex);
    messages.send(0,0);
}
/* 0289C4FC */
void AudioWorkQueue::cancelOwner(u32 owner) {
    gabi::AutoFrame frame{gabi::current_function()};
    lockAudioWork(mutex);
    gabi::FrameLocal<gptr<nw::ut::HeapLink>> position(8);
    for(u32 priority=0;priority<3;++priority) {
        AudioWorkList* list=&priorities[priority];
        nw::ut::HeapLink* next=list->root.next;
        while(next!=&list->root) {
            AudioWork* work=reinterpret_cast<AudioWork*>(next);
            u32 workOwner=work->owner;
            next=next->next;
            if(workOwner!=owner) continue;
            *position=&work->link;
            eraseAudioWork(list,position.get());
            work->state=4;
            work->finished.signal();
        }
    }
    unlockAudioWork(mutex);
}
/* 0289C5B4 */
void AudioWorkQueue::cancelOwnerWrapper(u32 owner) { cancelOwner(owner); }
/* 0289C5B8 */
AudioWork* AudioWorkQueue::peek() {
    AudioWork* work=select(2,0);
    if(!work) work=select(1,0);
    if(!work) work=select(0,0);
    return work;
}
/* 0289C61C */
void AudioWorkQueue::waitForWork() {
    gabi::AutoFrame frame{gabi::current_function()};
    stopRequested=0;
    gabi::FrameLocal<be<u32>> message(8);
    while(!peek() && !stopRequested) {
        // HD: frame-exact for cross-build save states (the task thread waits here): the original's
        // return address and r31.
        if(gabi::call_site<u32>(0x0289C64C,{{31,gabi::ea(this)}},{},0x028F6C94,&messages,message.get(),1u) && static_cast<u32>(*message)==0) break;
    }
}
/* 0289C690 */
u32 AudioWorkQueue::requestStop() { stopRequested=1; return messages.send(0,1); }
/* 0289C6A4 */
AudioWorkList* AudioWorkList::construct(AudioWorkList* list) {
    if(!list) list=static_cast<AudioWorkList*>(allocateAudioWork(sizeof(AudioWorkList)));
    if(list) { list->count=0; list->root.previous=&list->root; list->root.next=&list->root; }
    return list;
}
/* 0289C6EC */
void AudioWorkQueue::idleHook() {}
/* 0289C6F0 */
void AudioWorkQueue::destroy(u32 flags) {
    if(!this) return;
    messages.destroy(2);
    queueReady=0;
    if(flags&1) deleteAudioWork(this);
}
/* WWHD glue
data sAudioWorkQueue 104BB4C4
data sAudioWorkQueueGuard 104BB75C
data sAudioWorkRegistration 101FC908
ext 028EFFD0 void constructAudioWorkArray(void*,u32,u32,AudioWorkListConstructor)
ext C0009D68 void initializeAudioWorkMutex(void*)
ext 028F026C void registerAudioWorkStatic(void*)
ext 028F6BEC nw::ut::MessageQueue* nw::ut::MessageQueue::create(nw::ut::MessageQueue*)
ext 028F6CD4 void finishAudioMessages(nw::ut::MessageQueue*)
ext C0009E40 void lockAudioWork(void*)
ext C000A1C8 void unlockAudioWork(void*)
ext 0286DB18 nw::ut::HeapLink* eraseAudioWork(nw::ut::HeapChildren*,gptr<nw::ut::HeapLink>*)
ext 0286DA70 nw::ut::HeapLink* insertAudioWork(nw::ut::HeapChildren*,gptr<nw::ut::HeapLink>*,nw::ut::HeapLink*)
ext 0273AD10 void* allocateAudioWork(u32)
ext 0273AF40 void deleteAudioWork(void*)
ext 028F6A28 void nw::ut::Event::reset()
ext 028F6A24 void nw::ut::Event::signal()
ext 028F6C2C void nw::ut::MessageQueue::initialize(void*,s32)
ext 028F6C30 u32 nw::ut::MessageQueue::send(u32,u32)
ext 028F6C94 u32 nw::ut::MessageQueue::receive(be<u32>*,u32)
ext 028F6C18 void nw::ut::MessageQueue::destroy(u32)
*/

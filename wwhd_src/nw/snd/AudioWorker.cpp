// Background audio worker lifetime, reconstructed from WWHD instructions.
#include "nw/snd/AudioWorkQueue.h"
using namespace nw::snd;
struct WorkerTable { be<u32> words[6]; };
static WorkerTable sAudioWorkerTable;
struct WorkerName { be<u8> bytes[20]; };
static WorkerName sAudioWorkerName;
static AudioWorker sAudioWorker;
static u32 sAudioWorkerGuard;
struct WorkerRegistration { be<u32> words[3]; };
static WorkerRegistration sAudioWorkerRegistration;
void* allocateAudioWorker(u32);
void deleteAudioWorker(void*);
void initializeWorkerMutex(void*);
void registerAudioWorker(void*);
/* 0289C74C */
AudioWorker* AudioWorker::construct(AudioWorker* worker) {
    if(!worker) worker=static_cast<AudioWorker*>(allocateAudioWorker(0x70C));
    if(worker) {
        worker->vtable=gabi::ea(&sAudioWorkerTable);
        nw::ut::Thread::create(&worker->thread,reinterpret_cast<nw::ut::ThreadTarget*>(worker));
        worker->mutexReady=1;
        initializeWorkerMutex(worker->mutex);
        worker->stopRequested=0;
        worker->running=0;
    }
    return worker;
}
/* 0289C7C0 */
void AudioWorker::stop() {
    if(!running) return;
    stopRequested=1;
    AudioWorkQueue::instance()->requestStop();
    thread.join();
    running=0;
}
/* 0289C814 */
void AudioWorker::destroy(u32 flags) {
    if(!this) return;
    u8 wasRunning=running;
    vtable=gabi::ea(&sAudioWorkerTable);
    if(wasRunning) stop();
    mutexReady=0;
    thread.destroy(2);
    if(flags&1) deleteAudioWorker(this);
}
/* 0289C890 */
u32 AudioWorker::start(u32 priority,u32 stack,u32 bytes,u32 attributes) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(running) stop();
    gabi::FrameLocal<nw::ut::ThreadConfig> config(8);
    config->priority=priority;
    config->stack=stack;
    stopRequested=0;
    config->stackSize=bytes;
    config->name=sAudioWorkerName.bytes;
    config->attributes=attributes;
    if(!thread.start(config.get())) return 0;
    thread.resume();
    running=1;
    return 1;
}
/* 0289C940 */
AudioWorker* AudioWorker::instance() {
    if(sAudioWorkerGuard==0) {
        sAudioWorkerGuard=1;
        construct(&sAudioWorker);
        registerAudioWorker(&sAudioWorkerRegistration);
    }
    return &sAudioWorker;
}
/* WWHD glue
data sAudioWorkerTable 101823FC
data sAudioWorkerName 101823E8
data sAudioWorker 104BB760
data sAudioWorkerGuard 104BBE6C
data sAudioWorkerRegistration 101FC914
ext 0273AD10 void* allocateAudioWorker(u32)
ext 0273AF40 void deleteAudioWorker(void*)
ext C0009D68 void initializeWorkerMutex(void*)
ext 028F026C void registerAudioWorker(void*)
ext 028F6A60 nw::ut::Thread* nw::ut::Thread::create(nw::ut::Thread*,nw::ut::ThreadTarget*)
ext 028F6BD8 u32 nw::ut::Thread::join()
ext 028F6ADC void nw::ut::Thread::destroy(u32)
ext 028F6B24 u32 nw::ut::Thread::start(const nw::ut::ThreadConfig*)
ext 028F6BE4 u32 nw::ut::Thread::resume()
*/

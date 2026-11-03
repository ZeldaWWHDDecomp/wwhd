// Audio frame driver worker lifecycle, reconstructed from original WWHD instructions.
#include "nw/snd/AudioFrameDriver.h"
#include "nw/snd/HardwareVoice.h"
#include "nw/snd/AudioEventLog.h"
using namespace nw::snd;
void lockAudioFrame(void*);
void unlockAudioFrame(void*);
nw::ut::HeapLink* insertFrameListener(nw::ut::HeapChildren*,gptr<nw::ut::HeapLink>*,nw::ut::HeapLink*);
nw::ut::HeapLink* removeFrameListener(nw::ut::HeapChildren*,nw::ut::HeapLink*);
void deleteAudioFrameDriver(void*);
struct DriverThreadName { be<u8> bytes[20]; };
static DriverThreadName sDriverThreadName;
// the sound driver's event log (function-local static of an inlined accessor, shared with NW-SND#5/#7/#8)
static AudioEventLog sEventLog;
static u32 sEventLogGuard;
/* 02898708 */
u32 AudioFrameDriver::prepare(u32 option) {
    if(!initialized) { threadOption=option; initialized=1; }
    return 1;
}
/* 02898728 */
u32 AudioFrameDriver::start(u32 priority,u32 stack,u32 bytes,u32 attributes,u32 option) {
    gabi::AutoFrame frame{gabi::current_function()};
    havePreviousTiming=0;
    if(!prepare(option)) return 0;
    gabi::FrameLocal<nw::ut::ThreadConfig> config(8);
    config->stack=stack; config->stackSize=bytes; config->attributes=attributes;
    frameSerial=0; threadAttributes=attributes; config->name=sDriverThreadName.bytes;
    config->priority=priority; receivedSerial=0;
    u32 result=thread.start(config.get());
    if(result) thread.resume();
    return result;
}
/* 028987D8 */
void AudioFrameDriver::stop() {
    if(initialized) { messages.sendFirst(0x20000000,1); thread.join(); initialized=0; }
}
/* 0289882C */
void AudioFrameDriver::finalize() {
    stop();
    nw::ut::HeapLink* end=&listeners.root;
    nw::ut::HeapLink* node=listeners.root.next;
    while(node!=end) {
        FrameListener* listener=reinterpret_cast<FrameListener*>(node);
        u32 target=listener->vtable->end;
        node=node->next;
        gabi::call_ptr<void>(target,listener);
    }
}
/* 02898DB0 */
void AudioFrameDriver::addPlayer(nw::ut::HeapLink* player) {
    gabi::AutoFrame frame{gabi::current_function()};
    lockAudioFrame(mutex);
    gabi::FrameLocal<gptr<nw::ut::HeapLink>> position(8);
    *position=&players.root;
    insertFrameListener(&players,position.get(),player);
    unlockAudioFrame(mutex);
}
/* 02898E18 */
void AudioFrameDriver::removePlayer(nw::ut::HeapLink* player) {
    lockAudioFrame(mutex); removeFrameListener(&players,player); unlockAudioFrame(mutex);
}
/* 02898E74 */
nw::ut::HeapLink* AudioFrameDriver::addListener(nw::ut::HeapLink* listener) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<gptr<nw::ut::HeapLink>> position(8);
    *position=&listeners.root;
    return insertFrameListener(&listeners,position.get(),listener);
}
/* 02898EA8 */
nw::ut::HeapLink* AudioFrameDriver::removeListener(nw::ut::HeapLink* listener) { return removeFrameListener(&listeners,listener); }
/* 02898EB0 */
u32 AudioFrameDriver::wakeCommands() { return messages.send(0x30000000,0); }
/* 02898EC0 */
void AudioFrameDriver::onAudioFrame() {
    frameSerial=(static_cast<u32>(frameSerial)+1)&0x0FFFFFFF;
    HardwareVoice::updateAll();
    if(skipFrameMessages) return;
    if(messages.send(static_cast<u32>(frameSerial)|(1u<<28),0)) return;
    if(sEventLogGuard==0) {
        sEventLog.mWriting=&sEventLog.mBuffers[0];
        sEventLog.mBuffers[1].mCount=0;
        sEventLogGuard=1;
        sEventLog.mBuffers[0].mCount=0;
    }
    sEventLog.append(1,0,0);
}
/* 02898F64 */
void AudioFrameDriver::frameCallback() { AudioFrameDriver::instance()->onAudioFrame(); }
/* 028990A8 */
void AudioFrameDriver::idleHook() {}
/* 028990AC */
void AudioFrameDriver::destroy(u32 flags) {
    if(!this) return;
    mutexReady=0; messages.destroy(2); thread.destroy(2);
    if(flags&1) deleteAudioFrameDriver(this);
}
/* WWHD glue
data sDriverThreadName 10182294
data sEventLog 101FE250
data sEventLogGuard 101FE24C
ext 028986B0 nw::snd::AudioFrameDriver* nw::snd::AudioFrameDriver::instance()
ext C0009E40 void lockAudioFrame(void*)
ext C000A1C8 void unlockAudioFrame(void*)
ext 0286DA70 nw::ut::HeapLink* insertFrameListener(nw::ut::HeapChildren*,gptr<nw::ut::HeapLink>*,nw::ut::HeapLink*)
ext 0286DA9C nw::ut::HeapLink* removeFrameListener(nw::ut::HeapChildren*,nw::ut::HeapLink*)
ext 028A882C void nw::snd::HardwareVoice::updateAll()
ext 0289D1D8 void nw::snd::AudioEventLog::append(u32,u32,u32)
ext 0273AF40 void deleteAudioFrameDriver(void*)
ext 028F6C60 u32 nw::ut::MessageQueue::sendFirst(u32,u32)
*/

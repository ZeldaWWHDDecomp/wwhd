// Audio frame profiling, reconstructed from the game's integer timestamp operations.
#include "nw/snd/AudioFrameDriver.h"
#include "nw/snd/RemoteSpeaker.h"
using namespace nw::snd;
struct AudioFrameReport {
    be<u64> timing[20];
    be<u32> sampleEnd,sampleBegin,sampleDifference,userSamples,sampleCursor,reserved;
    be<u64> hardwareTime,applicationTime;
};
WWHD_SIZE(AudioFrameReport,0xC8);
static_assert(sizeof(AudioFrameReport)==sizeof(RemoteCaptureFrame),"the ring stores the report as words");
static u64 profileTime(const AudioProfileRecord* profile,u32 offset) {
    return (static_cast<u64>(static_cast<u32>(profile->words[offset/4]))<<32) |
        static_cast<u32>(profile->words[offset/4+1]);
}
/* 0289888C */
void AudioFrameDriver::publishProfile(const AudioProfileRecord* profile,const be<u64>* begin,const be<u64>* end,u32 userSamples) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!profileListeners.count) return;
    gabi::FrameLocal<AudioFrameReport> report(8);
    u64 first=*begin, last=*end;
    report->timing[0]=first; report->timing[1]=last;
    report->timing[2]=profileTime(profile,0x20);
    report->timing[3]=profileTime(profile,0x28);
    report->timing[4]=profileTime(profile,0x58);
    report->timing[5]=profileTime(profile,0x60);
    u64 swapBegin=profileTime(profile,0);
    report->timing[8]=swapBegin;
    report->timing[9]=profileTime(profile,0x58);
    report->timing[10]=profileTime(profile,0x38);
    u64 auxiliaryEnd=profileTime(profile,8);
    report->timing[11]=auxiliaryEnd;
    report->timing[6]=profileTime(profile,0x10);
    report->timing[7]=profileTime(profile,0x18);
    u64 auxiliaryBegin=profileTime(profile,0x68);
    report->timing[12]=auxiliaryBegin;
    u32 sampleBegin=profile->words[0x44/4];
    report->timing[13]=profileTime(profile,0x70);
    report->timing[14]=profileTime(profile,0x30);
    report->timing[15]=profileTime(profile,0x38);
    report->timing[17]=auxiliaryEnd; report->timing[16]=swapBegin;
    report->timing[18]=profileTime(profile,0x48);
    report->timing[19]=profileTime(profile,0x50);
    u32 sampleEnd=profile->words[0x40/4];
    report->sampleDifference=sampleEnd-sampleBegin;
    report->sampleBegin=sampleBegin;
    report->userSamples=userSamples;
    report->sampleEnd=sampleEnd+userSamples;
    u64 swapTime=profileTime(profile,0x28)-swapBegin;
    u64 auxiliaryTime=auxiliaryEnd-auxiliaryBegin;
    report->hardwareTime=swapTime+auxiliaryTime;
    u64 application=last-first;
    if(static_cast<s64>(first)<=static_cast<s64>(swapBegin) && static_cast<s64>(swapBegin)<=static_cast<s64>(last)) application-=swapTime;
    if(static_cast<s64>(first)<=static_cast<s64>(auxiliaryBegin) && static_cast<s64>(auxiliaryBegin)<=static_cast<s64>(last)) application-=auxiliaryTime;
    report->applicationTime=application;
    report->sampleCursor=sampleEnd; report->reserved=0;
    nw::ut::HeapLink* node=profileListeners.root.next;
    while(node!=&profileListeners.root) {
        RemoteCaptureRing* ring=gabi::at<RemoteCaptureRing>(gabi::ea(node));   // the listener link is the ring's first member
        ring->push(reinterpret_cast<const RemoteCaptureFrame*>(report.get()));
        node=node->next;
    }
}
/* WWHD glue
ext 028A2344 void nw::snd::RemoteCaptureRing::push(const nw::snd::RemoteCaptureFrame*)
*/

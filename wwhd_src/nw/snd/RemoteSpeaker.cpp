/* Remote speaker commands and finite alarm lifecycle, from WWHD instructions. */
#include "nw/snd/RemoteSpeaker.h"
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
using namespace nw::snd;
static RemoteSpeakerManager sRemoteSpeakerManager;
static u32 sRemoteSpeakerManagerGuard;
static void* remoteSpeakerAllocate(u32);
static void remoteCancelAlarm(RemoteSpeakerAlarm*);
static void remoteCreateAlarm(RemoteSpeakerAlarm*);
static void remoteSetAlarmOwner(RemoteSpeakerAlarm*,RemoteSpeaker*);
static RemoteSpeaker* remoteAlarmOwner(RemoteSpeakerAlarm*);
static OsSystemInfo* remoteSystemInfo();
static Pair32 remoteCurrentTime();
static Pair32 remoteDivideTime(u32,u32,u32,u32);
static void remoteSetAlarm();
static void remoteSetPeriodicAlarm();
static u32 remoteCanSend(u32);
static void remoteEncode(void*,u32,const RemoteSpeakerSamples*,u32,RemoteSpeakerEncoded*);
static s32 remoteSend(u32,const RemoteSpeakerEncoded*,u32);
using RemoteControlCallback = void(*)(u32,s32);
static void remoteControlSpeaker(u32,u32,RemoteControlCallback);
static void remoteClear(void*,u32,u32);
static s32 remoteSamplesLeft();
static void remoteGetSamples(u32,RemoteSpeakerSamples*,u32);
static void remoteAdvanceSamples(u32);
static void remoteRequestTail(RemoteSpeaker*,u32);

/* 028A18F8 */
void RemoteSpeaker::reset() {
    gabi::AutoFrame frame{gabi::current_function()};
    mActive=0; mReady=0;
    remoteCancelAlarm(&mStartAlarm);
    mAlarmPending=0;
    remoteCancelAlarm(&mStopAlarm);
    mAlarmActive=0;
}
/* 028A194C */
void RemoteSpeaker::begin() {
    gabi::AutoFrame frame{gabi::current_function()};
    reset(); mAlarmPending=0; mAlarmActive=0; mReady=1; mActive=0;
}
/* 028A1990 */
u32 RemoteSpeaker::enable(u32 callback) {
    gabi::AutoFrame frame{gabi::current_function()};
    begin();
    u32 previous=mCallback;
    if (previous) { gabi::call_ptr<void>(previous,(u32)mChannel,0u); mNotify=0; }
    mCallback=callback; mEnabled=1; mPendingCommand=1;
    return 1;
}
/* 028A1A00 */
void RemoteSpeaker::disable(u32 callback) {
    gabi::AutoFrame frame{gabi::current_function()};
    reset();
    u32 previous=mCallback;
    if (previous) { gabi::call_ptr<void>(previous,(u32)mChannel,0u); mNotify=0; }
    mEnabled=0; mCallback=callback; mPendingCommand=3;
}
/* 028A1A74 */
u32 RemoteSpeaker::isReady() const { if (!mEnabled) return 0; return mReady; }
/* 028A1A90 */
RemoteSpeaker* RemoteSpeaker::construct(RemoteSpeaker* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) object=static_cast<RemoteSpeaker*>(remoteSpeakerAllocate(sizeof(RemoteSpeaker)));
    if (object) {
        object->mState=0; object->mDeferredCommand=0; object->mFirstEncoding=0;
        object->mPendingCommand=0; object->mActive=0; object->mBusy=0;
        object->mCallback=0; object->mReady=0; object->mNotify=0;
        object->mEnabled=0;
        remoteCreateAlarm(&object->mStartAlarm);
        remoteSetAlarmOwner(&object->mStartAlarm,object);
        remoteCreateAlarm(&object->mStopAlarm);
        remoteSetAlarmOwner(&object->mStopAlarm,object);
    }
    return object;
}
/* 028A1B24 */
u32 RemoteSpeaker::samplesSilent(const RemoteSpeakerSamples* samples) const {
    const be<u32>* words=reinterpret_cast<const be<u32>*>(samples);
    for (u32 i=0;i!=20;++i) if ((u32)words[i]) return 0;
    return 1;
}
/* 028A1B50 */
RemoteSpeaker* RemoteSpeaker::alarmOwner(RemoteSpeakerAlarm* alarm) {
    return gabi::tail_ptr<RemoteSpeaker*>(gabi::guest_fn((const void*)remoteAlarmOwner),alarm);
}
/* 028A1B54 */
void RemoteSpeaker::stopAlarm(RemoteSpeakerAlarm* alarm) {
    gabi::AutoFrame frame{gabi::current_function()};
    RemoteSpeaker* owner=remoteAlarmOwner(alarm);
    if (owner->mAlarmActive) { remoteCancelAlarm(&owner->mStartAlarm); owner->mAlarmPending=0; }
    owner->mAlarmActive=0;
}
/* 028A1BA8 */
void RemoteSpeaker::submitSamples(const RemoteSpeakerSamples* samples) {
    gabi::AutoFrame frame{gabi::current_function()};
    if ((u32)mState!=4) return;
    u8 active=1;
    if (!mReady || samplesSilent(samples)) active=0;
    u8 previous=mActive;
    u8 starting=previous==0 && active!=0;
    u8 stopping=previous!=0 && active==0;
    if (active) {
        if (!remoteCanSend(mChannel)) return;
        u8 first=mFirstEncoding;
        gabi::FrameLocal<RemoteSpeakerEncoded> encoded(8);
        mFirstEncoding=0;
        remoteEncode(mCodecState,(u32)first^1u,samples,40,encoded.get());
        if (remoteSend(mChannel,encoded.get(),20)!=0) {
            mState=0; mDeferredCommand=1; begin(); return;
        }
    }
    if (starting) {
        if (!mAlarmPending) {
            u32 clock=remoteSystemInfo()->mBusClock;
            u64 delay=(u64)(clock>>2)*480u;
            // The SDK's64-bit delay skips r4 in the GHS argument registers.
            gabi::call_site<void>(0x028A1CE8,{{3,gabi::ea(&mStartAlarm)},{5,(u32)(delay>>32)},{6,(u32)delay},{7,gabi::guest_fn((const void*)RemoteSpeaker::alarmOwner)}},{},gabi::guest_fn((const void*)remoteSetAlarm));
            auto now=remoteCurrentTime();
            mStartTimeHigh=now.r3; mStartTimeLow=now.r4; mAlarmPending=1;
        }
        remoteCancelAlarm(&mStopAlarm); mAlarmActive=0;
    }
    if (stopping) {
        mAlarmActive=1; remoteCancelAlarm(&mStopAlarm);
        u32 clock=remoteSystemInfo()->mBusClock;
        gabi::call_site<void>(0x028A1D40,{{3,gabi::ea(&mStopAlarm)},{5,0},{6,clock>>2},{7,gabi::guest_fn((const void*)RemoteSpeaker::stopAlarm)}},{},gabi::guest_fn((const void*)remoteSetAlarm));
    }
    mActive=active;
}
/* 028A1D58 */
void RemoteSpeaker::notify(u32 channel,s32 result) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mNotify) {
        u32 callback=mCallback;
        if (callback) { gabi::call_ptr<void>(callback,channel,result); mCallback=0; }
    }
}

/* 028A1DB0 */
void RemoteSpeaker::onEnable(u32 channel,s32 result) {
    gabi::AutoFrame frame{gabi::current_function()};
    RemoteSpeakerManager* manager=RemoteSpeakerManager::instance();
    RemoteSpeaker* object=manager->speaker(channel);
    if (result==0) {
        object->mFirstEncoding=1;
        remoteClear(object->mCodecState,0,sizeof(object->mCodecState));
        object->mBusy=0; object->mDeferredCommand=2; object->mState=2;
    } else if ((u32)result==0xFFFFFFFEu) {
        object->mBusy=0; object->mDeferredCommand=1;
    } else {
        object->mState=0;
        object->notify(channel,result);
        object->mBusy=0;
    }
}
/* 028A1E84 */
void RemoteSpeaker::onConfigure(u32 channel,s32 result) {
    gabi::AutoFrame frame{gabi::current_function()};
    RemoteSpeakerManager* manager=RemoteSpeakerManager::instance();
    RemoteSpeaker* object=manager->speaker(channel);
    if (result==0) { object->mState=4; object->notify(channel,result); }
    else if ((u32)result==0xFFFFFFFEu) { object->mBusy=0; object->mDeferredCommand=2; return; }
    else { object->mState=0; object->notify(channel,result); }
    object->mBusy=0;
}
/* 028A1F40 */
void RemoteSpeaker::onDisable(u32 channel,s32 result) {
    gabi::AutoFrame frame{gabi::current_function()};
    RemoteSpeakerManager* manager=RemoteSpeakerManager::instance();
    RemoteSpeaker* object=manager->speaker(channel);
    if (result==0) { object->mState=6; object->notify(channel,result); }
    else if ((u32)result==0xFFFFFFFEu) { object->mBusy=0; object->mDeferredCommand=3; return; }
    else { object->mState=0; object->notify(channel,result); }
    object->mBusy=0;
}
/* 028A1FFC */
void RemoteSpeaker::request(u32 command) {
    switch (command) {
    case 1:
        mBusy=1; mState=1; mNotify=1;
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)remoteControlSpeaker),(u32)mChannel,1u,RemoteSpeaker::onEnable);
        break;
    case 2:
        mBusy=1; mNotify=1; mState=3;
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)remoteControlSpeaker),(u32)mChannel,4u,RemoteSpeaker::onConfigure);
        break;
    case 3:
        mBusy=1; mNotify=1; mState=5;
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)remoteControlSpeaker),(u32)mChannel,0u,RemoteSpeaker::onDisable);
        break;
    }
}
/* 028A2088 */
void RemoteSpeaker::flush() {
    if (mBusy) return;
    u32 command=mPendingCommand;
    if (!command) command=mDeferredCommand;
    mDeferredCommand=0; mPendingCommand=0;
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)remoteRequestTail),this,command);
}
/* 028A20B4 */
RemoteSpeakerManager* RemoteSpeakerManager::construct(RemoteSpeakerManager* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) object=static_cast<RemoteSpeakerManager*>(remoteSpeakerAllocate(sizeof(RemoteSpeakerManager)));
    if (object) {
        object->mInitialized=0;
        __construct_array(object->mSpeakers,4,sizeof(RemoteSpeaker),reinterpret_cast<void*(*)(void*)>(RemoteSpeaker::construct));
        for (u32 i=0;i!=4;++i) object->mSpeakers[i].mChannel=i;
    }
    return object;
}
/* 028A2138 */
RemoteSpeakerManager* RemoteSpeakerManager::instance() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!(u32)sRemoteSpeakerManagerGuard) { sRemoteSpeakerManagerGuard=1; construct(&sRemoteSpeakerManager); }
    return &sRemoteSpeakerManager;
}
/* 028A2184 */
RemoteSpeaker* RemoteSpeakerManager::speaker(u32 channel) {
    return gabi::at<RemoteSpeaker>(gabi::ea(mSpeakers)+channel*sizeof(RemoteSpeaker));
}
/* 028A2194 */
void RemoteSpeakerManager::capture() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<RemoteSpeakerSamples> samples(8);
    RemoteSpeakerManager* manager=instance();
    if (remoteSamplesLeft()<40) return;
    for (u32 channel=0;channel!=4;++channel) {
        RemoteSpeaker* object=&manager->mSpeakers[channel];
        if ((u32)object->mState==4) {
            remoteGetSamples(channel,samples.get(),40);
            object->submitSamples(samples.get());
        }
        object->flush();
    }
    remoteAdvanceSamples(40);
}
/* 028A2240 */
void RemoteSpeakerManager::initialize() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mInitialized) return;
    remoteCreateAlarm(&mAlarm);
    auto now=remoteCurrentTime();
    u32 clock=remoteSystemInfo()->mBusClock;
    auto quotient=remoteDivideTime(0,clock>>2,0,0x7A12);
    u64 product=(((u64)quotient.r3<<32)|quotient.r4)*0x65B9ABu;
    auto period=remoteDivideTime((u32)(product>>32),(u32)product,0,0x7D00);
    // GHS aligns both64-bit times to r5:r6 and r7:r8; r4 is padding.
    gabi::call_site<void>(0x028A22DC,{{3,gabi::ea(&mAlarm)},{5,now.r3},{6,now.r4},{7,period.r3},{8,period.r4},{9,gabi::guest_fn((const void*)RemoteSpeakerManager::capture)}},{},gabi::guest_fn((const void*)remoteSetPeriodicAlarm));
    mInitialized=1;
}
/* 028A2300 */
void RemoteSpeakerManager::finalize() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mInitialized) { remoteCancelAlarm(&mAlarm); mInitialized=0; }
}
/* 028A2344 */
void RemoteCaptureRing::push(const RemoteCaptureFrame* source) {
    u32 index=mWriteIndex;
    RemoteCaptureFrame* target=gabi::at<RemoteCaptureFrame>(gabi::ea(mFrames)+index*sizeof(RemoteCaptureFrame));
    for (u32 i=0;i!=25;++i) {
        target->mWords[i*2]=source->mWords[i*2];
        target->mWords[i*2+1]=source->mWords[i*2+1];
    }
    u32 next=(u32)mWriteIndex+1u;
    if ((s32)next>=32) next=0;
    mWriteIndex=next;
}
/* WWHD glue
ext 0273AD10 void* remoteSpeakerAllocate(u32)
ext C00099A8 void remoteCancelAlarm(nw::snd::RemoteSpeakerAlarm*)
ext C0009A38 void remoteCreateAlarm(nw::snd::RemoteSpeakerAlarm*)
ext C0009FF0 void remoteSetAlarmOwner(nw::snd::RemoteSpeakerAlarm*,nw::snd::RemoteSpeaker*)
ext C0009B70 nw::snd::RemoteSpeaker* remoteAlarmOwner(nw::snd::RemoteSpeakerAlarm*) callback
ext C0009C80 nw::snd::OsSystemInfo* remoteSystemInfo()
ext C0009CD8 Pair32 remoteCurrentTime()
ext 028F5A1C Pair32 remoteDivideTime(u32,u32,u32,u32)
ext C0009FD8 void remoteSetAlarm() callback
ext C000A038 void remoteSetPeriodicAlarm() callback
ext C0008348 u32 remoteCanSend(u32)
ext C0008338 void remoteEncode(void*,u32,const nw::snd::RemoteSpeakerSamples*,u32,nw::snd::RemoteSpeakerEncoded*)
ext C00085C0 s32 remoteSend(u32,const nw::snd::RemoteSpeakerEncoded*,u32)
ext C00083A0 void remoteControlSpeaker(u32,u32,RemoteControlCallback) callback
ext C0009990 void remoteClear(void*,u32,u32)
ext C0006F00 s32 remoteSamplesLeft()
ext C0006EF8 void remoteGetSamples(u32,nw::snd::RemoteSpeakerSamples*,u32)
ext C0006EF0 void remoteAdvanceSamples(u32)
ext 028A1FFC void remoteRequestTail(nw::snd::RemoteSpeaker*,u32) callback
data sRemoteSpeakerManager 104BBE98
data sRemoteSpeakerManagerGuard 104BBE90
*/

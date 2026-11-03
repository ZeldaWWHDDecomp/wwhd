/* Wii remote speaker controller and alarm storage, recovered from WWHD. Own names. */
#pragma once
#include "nw/snd/BasicSound.h"
#include "nw/ut/FrameHeap.h"
namespace nw { namespace snd {
struct RemoteSpeakerAlarm { u8 mStorage[0x58]; };
struct RemoteSpeakerSamples { be<s16> mSamples[40]; };
struct RemoteSpeakerEncoded { be<u8> mBytes[20]; };
/* Leading word of the Cafe OS system information record: the bus clock. */
struct OsSystemInfo { be<u32> mBusClock; };
struct RemoteSpeaker {
    be<u8> mEnabled,mActive,mReady,mFirstEncoding;
    be<u8> mNotify,mBusy,mAlarmPending,mAlarmActive;
    be<u32> mState,mPendingCommand,mDeferredCommand;
    u8 mCodecState[0x20];
    be<u32> mChannel,mCallback;
    u8 mAlarmAlignment[4];
    RemoteSpeakerAlarm mStartAlarm,mStopAlarm;
    be<u32> mStartTimeHigh,mStartTimeLow;
    static RemoteSpeaker* construct(RemoteSpeaker*);
    void reset(); void begin(); u32 enable(u32); void disable(u32);
    u32 isReady() const; u32 samplesSilent(const RemoteSpeakerSamples*) const;
    static RemoteSpeaker* alarmOwner(RemoteSpeakerAlarm*);
    static void stopAlarm(RemoteSpeakerAlarm*);
    void submitSamples(const RemoteSpeakerSamples*);
    void notify(u32,s32);
    static void onEnable(u32,s32); static void onConfigure(u32,s32); static void onDisable(u32,s32);
    void request(u32); void flush();
};
WWHD_OFFSET(RemoteSpeaker,mState,8);
WWHD_OFFSET(RemoteSpeaker,mCodecState,0x14);
WWHD_OFFSET(RemoteSpeaker,mChannel,0x34);
WWHD_OFFSET(RemoteSpeaker,mCallback,0x38);
WWHD_OFFSET(RemoteSpeaker,mStartAlarm,0x40);
WWHD_OFFSET(RemoteSpeaker,mStopAlarm,0x98);
WWHD_OFFSET(RemoteSpeaker,mStartTimeHigh,0xF0);
WWHD_SIZE(RemoteSpeaker,0xF8);
struct RemoteSpeakerManager {
    be<u8> mInitialized; u8 mAlignment[7];
    RemoteSpeakerAlarm mAlarm;
    RemoteSpeaker mSpeakers[4];
    static RemoteSpeakerManager* construct(RemoteSpeakerManager*);
    static RemoteSpeakerManager* instance();
    RemoteSpeaker* speaker(u32);
    static void capture();
    void initialize(); void finalize();
};
WWHD_OFFSET(RemoteSpeakerManager,mSpeakers,0x60);
WWHD_SIZE(RemoteSpeakerManager,0x440);
/* One audio frame's profile report as words (AudioFrameDriver::publishProfile's AudioFrameReport, 0xC8 bytes). */
struct RemoteCaptureFrame { be<u32> mWords[50]; };
/* A ring of profile reports: an AudioFrameDriver profile listener (publishProfile 0289888C calls push directly
   with the listener link, which is the ring's first member). */
struct RemoteCaptureRing {
    nw::ut::HeapLink mProfileLink;
    RemoteCaptureFrame mFrames[32];
    be<u32> mReserved,mWriteIndex;
    void push(const RemoteCaptureFrame*);
};
WWHD_OFFSET(RemoteCaptureRing,mWriteIndex,0x190C);
WWHD_SIZE(RemoteCaptureRing,0x1910);
}}

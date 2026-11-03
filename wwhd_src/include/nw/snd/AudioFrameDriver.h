/* nw::snd AudioFrameDriver: the sound thread that runs one step per audio frame (instance 104B7810,
 * built on first use by AudioFrameDriver::instance 028986B0). Reconstructed from WWHD: the layout from
 * its constructor 02898574 (NW-SND#6) and the thread, frame and profile code (NW-SND#7). The object
 * is 0x3CB0 bytes (the static spans 104B7810..104BB4C0, where its construction flag follows). */
#pragma once
#include "nw/ut/Platform.h"
#include "nw/ut/FrameHeap.h"
#include "nw/snd/SoftwareMixer.h"
namespace nw { namespace snd {
struct FrameListenerVtable { u8 prefix[0x14]; be<u32> begin; be<u32> adjustEnd; be<u32> end; };
struct FrameListener { nw::ut::HeapLink link; gptr<FrameListenerVtable> vtable; };
struct AudioProfileRecord { be<u32> words[34]; };
struct AudioProfileBuffer { u8 bytes[0x1100]; };
struct AudioFrameDriver {
    /* 0x0000 */ be<u32> vtable;
    /* 0x0004 */ nw::ut::Thread thread;
    /* 0x06D8 */ nw::ut::MessageQueue messages;
    /* 0x0714 */ nw::ut::QueueMessage messageStorage[32];
    /* 0x0914 */ u8 reserved914[12];
    /* 0x0920 */ be<u32> frameSerial, receivedSerial;
    /* 0x0928 */ u8 mutex[0x2C];
    /* 0x0954 */ be<u8> mutexReady;
    /* 0x0955 */ u8 padding955[3];
    /* 0x0958 */ nw::ut::HeapChildren players;
    /* 0x0964 */ nw::ut::HeapChildren listeners;
    /* 0x0970 */ be<u32> callback, callbackArgument, threadAttributes;
    /* 0x097C */ be<u8> initialized, skipFrameMessages, threadOption;
    /* 0x097F */ u8 padding97F;
    /* 0x0980 */ gptr<SoftwareMixer> mixer;
    /* 0x0984 */ be<u8> havePreviousTiming;
    /* 0x0985 */ u8 padding985[3];
    /* 0x0988 */ be<u64> previousBegin, previousEnd;
    /* 0x0998 */ AudioProfileBuffer profileBuffers[3];
    /* 0x3C98 */ gptr<AudioProfileRecord> previousProfile;
    /* 0x3C9C */ be<s32> profileBufferIndex;
    /* 0x3CA0 */ nw::ut::HeapChildren profileListeners;
    /* 0x3CAC */ u8 padding3CAC[4];

    static AudioFrameDriver* create(AudioFrameDriver* storage);
    static AudioFrameDriver* instance();
    u32 prepare(u32 option);
    u32 start(u32 priority, u32 stack, u32 bytes, u32 attributes, u32 option);
    void stop();
    void finalize();
    void addPlayer(nw::ut::HeapLink*);
    void removePlayer(nw::ut::HeapLink*);
    nw::ut::HeapLink* addListener(nw::ut::HeapLink*);
    nw::ut::HeapLink* removeListener(nw::ut::HeapLink*);
    u32 wakeCommands();
    void onAudioFrame();
    static void frameCallback();
    void run();
    void idleHook();
    void destroy(u32 flags);
    void publishProfile(const AudioProfileRecord*, const be<u64>*, const be<u64>*, u32);
    void processFrame();
};
WWHD_OFFSET(AudioFrameDriver, messages, 0x6D8);
WWHD_OFFSET(AudioFrameDriver, messageStorage, 0x714);
WWHD_OFFSET(AudioFrameDriver, frameSerial, 0x920);
WWHD_OFFSET(AudioFrameDriver, mutex, 0x928);
WWHD_OFFSET(AudioFrameDriver, mutexReady, 0x954);
WWHD_OFFSET(AudioFrameDriver, players, 0x958);
WWHD_OFFSET(AudioFrameDriver, listeners, 0x964);
WWHD_OFFSET(AudioFrameDriver, initialized, 0x97C);
WWHD_OFFSET(AudioFrameDriver, mixer, 0x980);
WWHD_OFFSET(AudioFrameDriver, havePreviousTiming, 0x984);
WWHD_OFFSET(AudioFrameDriver, previousBegin, 0x988);
WWHD_OFFSET(AudioFrameDriver, profileBuffers, 0x998);
WWHD_OFFSET(AudioFrameDriver, previousProfile, 0x3C98);
WWHD_OFFSET(AudioFrameDriver, profileListeners, 0x3CA0);
WWHD_SIZE(AudioFrameDriver, 0x3CB0);
}}

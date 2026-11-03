// Audio work scheduling views reconstructed solely from WWHD instructions.
#pragma once
#include "nw/ut/Platform.h"
#include "nw/ut/FrameHeap.h"
namespace nw { namespace snd {
struct AudioWorkVtable { u8 prefix[0x14]; be<u32> execute; };
struct AudioWork {
    nw::ut::HeapLink link;
    nw::ut::Event finished;
    be<u32> state;
    be<u32> owner;
    gptr<AudioWorkVtable> vtable;
    static AudioWork* construct(AudioWork*);
    void destroy(u32 flags);
};
struct AudioWorkList : nw::ut::HeapChildren { static AudioWorkList* construct(AudioWorkList*); };
struct AudioWorkQueue {
    AudioWorkList priorities[3];
    gptr<AudioWork> current;
    be<u8> stopRequested;
    u8 padding[3];
    u8 mutex[0x2C];
    be<u8> queueReady;
    u8 padding59[3];
    nw::ut::MessageQueue messages;
    be<u32> messageStorage[128];
    static AudioWorkQueue* construct(AudioWorkQueue*);
    static AudioWorkQueue* instance();
    void initializeMessages();
    void finishMessages();
    AudioWork* select(u32 priority,u32 remove);
    AudioWork* take();
    void executePending();
    void submit(AudioWork*,u32 priority);
    void cancelOwner(u32 owner);
    void cancelOwnerWrapper(u32 owner);
    AudioWork* peek();
    void waitForWork();
    u32 requestStop();
    void idleHook();
    void destroy(u32 flags);
};
WWHD_OFFSET(AudioWork,state,0x2C); WWHD_OFFSET(AudioWork,vtable,0x34);
WWHD_OFFSET(AudioWorkQueue,current,0x24); WWHD_OFFSET(AudioWorkQueue,mutex,0x2C);
WWHD_OFFSET(AudioWorkQueue,messages,0x5C); WWHD_OFFSET(AudioWorkQueue,messageStorage,0x98);
}}
namespace nw { namespace snd {
struct AudioWorker {
    be<u32> vtable;
    nw::ut::Thread thread;
    u8 mutex[0x2C];
    be<u8> mutexReady;
    u8 padding705[3];
    be<u8> stopRequested;
    be<u8> running;
    u8 padding70A[2];
    static AudioWorker* construct(AudioWorker*);
    static AudioWorker* instance();
    void stop();
    void destroy(u32 flags);
    u32 start(u32 priority,u32 stack,u32 bytes,u32 attributes);
    void run();
};
WWHD_OFFSET(AudioWorker,mutex,0x6D8); WWHD_OFFSET(AudioWorker,running,0x709);
WWHD_SIZE(AudioWorker,0x70C);
}}

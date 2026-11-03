// HD JAudio driver thread, dispatching DAC and DSP messages through Cafe queues.
#include "JSystem/JAudio/JASAudioThread.h"
#include "JSystem/JAudio/JASSystemHeap.h"
using namespace JASystem;
using namespace JASystem::Kernel;
using namespace gabi;
struct AudioMessage { be<u32> values[4]; };
struct AudioWaitQueue { u8 storage[0x10]; };
struct DriverName { gptr<void> mVtable; gptr<void> mArgument; };
struct DriverLock { be<u32> value; };
struct DriverLockLocals { DriverLock lock; DriverName name; };
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
using ChannelPool = gptr<TPool>;
static ChannelPool sChannelPool;
static AudioWaitQueue sWaitQueue;
static JASThreadVTable sDriverVtable;
static u32 sProbeVtable;
static u32 sProbeArgument;
static Initialization sInitialization;
static Registration sRegistration;
extern void* allocateAudioDriverObject(u32);
extern void* allocateAudioDriverStorage(u32, void*, u32);
extern void resumeAudioDriver(void*);
extern u32 sendAudioDriverMessage(JASMessageQueue*, AudioMessage*, u32);
extern u32 receiveAudioDriverMessage(JASMessageQueue*, AudioMessage*, u32);
extern void reportAudioDriver(const char*);
using DriverCallback = void (*)();
extern void configureAudioHardware(DriverCallback);
extern void setDspSyncCallback(DriverCallback);
extern void initializeDspInterface();
extern void initializeDspChannels();
extern u32 disableAudioInterrupts();
extern void restoreAudioInterrupts(u32);
extern void constructAudioChannelPool(TPool*);
extern void audioMemoryBarrier();
extern void sleepAudioDriver(AudioWaitQueue*);
extern void constructAudioProbe(DriverLock*, DriverName*);
extern void destroyAudioProbe(DriverLock*, u32);
extern void finishAudioDspFrame();
extern void exitAudioDriver(u32);
extern void registerAudioDriverGlobals(void*);
/* 02813AE0 */
JASAudioThread* constructAudioDriverThread(JASAudioThread* thread, s32 priority, u32 affinity, u32 messages, u32 stackSize) {
    if (!thread) {
        thread = static_cast<JASAudioThread*>(allocateAudioDriverObject(sizeof(JASAudioThread)));
        if (!thread)
            return nullptr;
    }
    JASThread_construct(thread, JASDram, stackSize, messages, priority, affinity);
    thread->mVtable = &sDriverVtable;
    return thread;
}
/* 02813B5C */
void JASAudioThread::create(s32 priority, u32 affinity) {
    auto thread = static_cast<JASAudioThread*>(allocateAudioDriverStorage(sizeof(JASAudioThread), JASDram, 32));
    if (thread)
        thread = constructAudioDriverThread(thread, priority, affinity, 0x40, 0x4000);
    audioThreadState.mThread = thread;
    audioThreadState.mReady = 0;
    resumeAudioDriver(thread->mThread);
}
/* 02813BDC */
void JASAudioThread::syncDSP() {
    JASThread* thread = audioThreadState.mThread;
    AudioMessage message;
    message.values[0] = 2;
    if (!sendAudioDriverMessage(&thread->mQueue, &message, 0))
        reportAudioDriver("----- syncDSP : Send Miss\n");
}
/* 02813C30 */
void JASAudioThread::DMACallback() {
    JASThread* thread = audioThreadState.mThread;
    AudioMessage message;
    message.values[0] = 1;
    if (!sendAudioDriverMessage(&thread->mQueue, &message, 0))
        reportAudioDriver("----- DMACallback : Can't send DAC_SYNC message\n");
}
/* 02813C84 */
void* JASAudioThread::run() {
    // HD: frame-exact for cross-build save states (the audio thread waits in the receive): the
    // original's frame (r28..r31 saved, LR), its message at sp+0x14 and probe at sp+8, and at each wait
    // the return address of the original's receive for the path just taken and the registers it holds
    // there (r31 = this, r30 = &sChannelPool, r28/r29 = the pool allocation / interrupt state when
    // the pool was created here).
    gabi::AutoFrame frame{gabi::current_function()};
    // HD: the native entry also configures the four fast-cast quantization registers.
    configureAudioHardware(DMACallback);
    setDspSyncCallback(syncDSP);
    initializeDspInterface();
    initializeDspChannels();
    bool setR29 = false, setR28 = false;
    u32 r28 = 0, r29 = 0;
    if (!sChannelPool) {
        u32 interrupts = disableAudioInterrupts();
        setR29 = true;
        r29 = interrupts;
        if (!sChannelPool) {
            auto pool = static_cast<TPool*>(allocateAudioDriverStorage(sizeof(TPool), JASDram, 4));
            setR28 = true;
            r28 = gabi::ea(pool);
            if (pool)
                constructAudioChannelPool(pool);
            sChannelPool = pool;
        }
        restoreAudioInterrupts(interrupts);
    }
    sChannelPool->newMemPool(0x118, 0x48);
    startAudioBridge();
    audioThreadState.mReady = 1;
    audioMemoryBarrier();
    gabi::FrameLocal<AudioMessage> message(0x14);
    u32 ret = 0x02813D7C;
    for (;;) {
        if (setR28)
            gabi::call_site(ret, {{28, r28}, {29, r29}, {30, gabi::ea(&sChannelPool)}, {31, gabi::ea(this)}}, {}, 0xC0009ED0, &mQueue,
                            message.get(), 1u);
        else if (setR29)
            gabi::call_site(ret, {{29, r29}, {30, gabi::ea(&sChannelPool)}, {31, gabi::ea(this)}}, {}, 0xC0009ED0, &mQueue, message.get(), 1u);
        else
            gabi::call_site(ret, {{30, gabi::ea(&sChannelPool)}, {31, gabi::ea(this)}}, {}, 0xC0009ED0, &mQueue, message.get(), 1u);
        ret = 0x02813DA8;
        switch ((u32)message->values[0]) {
        case 1:
            if (audioThreadState.mSuspended) {
                stopAudioBridge();
                sleepAudioDriver(&sWaitQueue);
            }
            audioThreadState.mFrameCount = (u32)audioThreadState.mFrameCount + 1;
            updateDac();
            finishAudioDspFrame();
            ret = 0x02813E00;
            break;
        case 2:
            audioThreadState.mRemainingSubframes = (u32)audioThreadState.mRemainingSubframes - 1;
            if (audioThreadState.mRemainingSubframes == 0) {
                TDSP_DACBuffer::finishDSPFrame();
                ret = 0x02813E44;
            } else {
                gabi::FrameLocal<DriverLockLocals> locals(8);
                locals->name.mArgument = &sProbeArgument;
                locals->name.mVtable = &sProbeVtable;
                constructAudioProbe(&locals->lock, &locals->name);
                TDSP_DACBuffer::updateDSP();
                destroyAudioProbe(&locals->lock, 2);
                ret = 0x02813E9C;
            }
            break;
        case 3:
            exitAudioDriver(0);
            return nullptr;
        }
    }
}
/* 02813EDC */
void __sinit_JASAudioThread_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerAudioDriverGlobals(&sRegistration);
}
/* WWHD glue
data sChannelPool 104B53E4
data sWaitQueue 104B53D4
data sDriverVtable 101700D4
data sProbeVtable 101700B8
data sProbeArgument 10170050
data sInitialization 104B53C4
data sRegistration 101F9EF8
str 10170068
str 10170084
ext 0273AD10 void* allocateAudioDriverObject(u32)
ext 0273B050 void* allocateAudioDriverStorage(u32, void*, u32)
ext C0009F38 void resumeAudioDriver(void*)
ext C0009FC8 u32 sendAudioDriverMessage(JASMessageQueue*, AudioMessage*, u32)
ext C0009ED0 u32 receiveAudioDriverMessage(JASMessageQueue*, AudioMessage*, u32)
ext C0009EE8 void reportAudioDriver(const char*)
ext 02813428 void configureAudioHardware(DriverCallback)
ext 028161C4 void setDspSyncCallback(DriverCallback)
ext 02816358 void initializeDspInterface()
ext 028159F8 void initializeDspChannels()
ext C0009A60 u32 disableAudioInterrupts()
ext C0009F30 void restoreAudioInterrupts(u32)
ext 028102C0 void constructAudioChannelPool(TPool*)
ext C0009E90 void audioMemoryBarrier()
ext C000A0C8 void sleepAudioDriver(AudioWaitQueue*)
ext 02844088 void constructAudioProbe(DriverLock*, DriverName*)
ext 028440C4 void destroyAudioProbe(DriverLock*, u32)
ext 02815628 void finishAudioDspFrame()
ext C0009B30 void exitAudioDriver(u32)
ext 028F026C void registerAudioDriverGlobals(void*)
*/

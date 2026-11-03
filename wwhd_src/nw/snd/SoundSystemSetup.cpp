/* NW-SND#6: sound system set-up and shut-down, and the audio frame driver singleton.
 * Written from WWHD's instructions; verified by tools/verify (spec lib2_nw_snd_6). */
#include "nw/snd/SoundSystemSetup.h"
#include "gc_api/MSL_C/string.h"
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
using namespace nw::snd;
static_assert(sizeof(SoftwareMixer) == 0xCBA8, "the mixer storage initialize() carves");

/* What initialize() carved out of the caller's buffer, and the set-up flags. */
struct SoundSystemState {
    /* 0x00 */ be<u32> channelCount;
    /* 0x04 */ be<u32> workerStack;
    /* 0x08 */ be<u32> workerStackBytes;
    /* 0x0C */ be<u32> driverStack;
    /* 0x10 */ be<u32> driverStackBytes;
    /* 0x14 */ gptr<SoftwareMixer> softwareMixer;
    /* 0x18 */ be<u8> initialized;
    /* 0x19 */ u8 padding19[2];
    /* 0x1B */ be<u8> commandStorageReady;
    /* 0x1C */ be<u8> softwareMixReady;
    /* 0x1D */ u8 padding1D[3];
};
static SoundSystemState sSoundSystem = {96, 0, 0, 0, 0, 0, 0, {0, 0}, 0, 0, {0, 0, 0}};

static u32 sFrameDriverVtable[6] = {0, 0, 0, 0x028990AC, 0, 0x02898F88};
static AudioFrameDriver sFrameDriver;
static u32 sFrameDriverConstructed;
// the static destructor record: {next, object, destructor}
static StaticExitRecord sFrameDriverExit = {{0, 0x104B7810, 0x028990AC}};

/* The GHS list constructor (count + circular root), inlined with its NULL-allocation path. */
/* inlined into 02898574 and 0289806C */
template <class List>
static inline void constructList(List* member) {
    List* list = wrappedAddress(member);
    if (list == NULL) list = (List*)utAllocate(sizeof(List));
    if (list != NULL) {
        list->count = 0;
        list->root.previous = &list->root;
        list->root.next = &list->root;
    }
}

/* 02898574 */
AudioFrameDriver* AudioFrameDriver::create(AudioFrameDriver* driver) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (driver == NULL) driver = (AudioFrameDriver*)utAllocate(sizeof(AudioFrameDriver));
    if (driver != NULL) {
        driver->vtable = gabi::ea(sFrameDriverVtable);
        nw::ut::Thread::create(&driver->thread, (nw::ut::ThreadTarget*)driver);
        nw::ut::MessageQueue::create(&driver->messages);
        driver->frameSerial = 0;
        driver->mutexReady = 1;
        driver->receivedSerial = 0;
        OSInitMutex(driver->mutex);
        constructList(&driver->players);
        constructList(&driver->listeners);
        driver->threadOption = 0;
        driver->previousProfile = NULL;
        driver->previousBegin = 0;
        driver->profileBufferIndex = -1;
        driver->skipFrameMessages = 0;
        driver->previousEnd = 0;
        driver->mixer = NULL;
        driver->initialized = 0;
        constructList(&driver->profileListeners);
        driver->messages.initialize(driver->messageStorage, 32);
    }
    return driver;
}

/* 028986B0 */
AudioFrameDriver* AudioFrameDriver::instance() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sFrameDriverConstructed == 0) {
        sFrameDriverConstructed = 1;
        create(&sFrameDriver);
        __register_global_object(gabi::ea(&sFrameDriverExit));
    }
    return &sFrameDriver;
}

/* 028984F0 */
SoundSystemConfig* SoundSystemConfig::create(SoundSystemConfig* config) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (config == NULL) config = (SoundSystemConfig*)utAllocate(sizeof(SoundSystemConfig));
    if (config != NULL) {
        config->driverPriority = 4;
        config->driverStackBytes = 0x4000;
        config->driverCommandBytes = 0x20000;
        config->taskCommandBytes = 0x2000;
        config->driverOption = 1;
        config->softwareMixEnabled = 0;
        config->workerStackBytes = 0x4000;
        config->softwareChannelCount = 2;
        config->workerPriority = 3;
    }
    return config;
}

/* 02897FD8 */
u32 SoundSystemConfig::commandStorageBytes() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 driver = driverCommandBytes;
    u32 task = taskCommandBytes;
    return driver + task;
}

/* 02897FE8 */
void SoundSystemConfig::initializeCommandStorage(u32 buffer) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sSoundSystem.commandStorageReady == 0) {
        u32 taskBuffer = buffer + (u32)driverCommandBytes;
        DriverCommandManager* driver = DriverCommandManager::instance();
        driver->prepare(gabi::at<DriverCommand>(buffer), driverCommandBytes);
        DriverCommandManager* task = DriverCommandManager::taskThreadInstance();
        task->prepare(gabi::at<DriverCommand>(taskBuffer), taskCommandBytes);
        sSoundSystem.commandStorageReady = 1;
    }
}

/* 02898568 */
u32 isSoundSystemInitialized() {
    gabi::AutoFrame frame{gabi::current_function()};
    return sSoundSystem.initialized;
}

/* The software mixer's sample buffer per output channel. */
static const u32 kSoftwareChannelBytes = 0x1680;

/* 02898364 */
u32 SoundSystemConfig::requiredStorageBytes() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 total = commandStorageBytes();
    total += (u32)workerStackBytes;
    PlaybackVoicePool* voices = PlaybackVoicePool::instance();
    u32 voiceBytes = voices->requiredBytes(sSoundSystem.channelCount);
    total += voiceBytes;
    WaveChannelPool* channels = WaveChannelPool::instance();
    u32 channelBytes = WaveChannelPool::requiredBytes(channels, sSoundSystem.channelCount);
    u32 software = softwareMixEnabled;
    u32 driverBytes = driverStackBytes;
    total += channelBytes;
    total += driverBytes;
    if (software != 0) total += (u32)softwareChannelCount * kSoftwareChannelBytes + sizeof(SoftwareMixer);
    return total + 31;
}

/* The mix runtime singleton (AudioMixRuntime.cpp owns it; this TU constructs it on first use too). */
static AudioMixRuntime sMixRuntime;
static u32 sMixRuntimeConstructed;
static u32 sSoftwareMixerVtable[6] = {0, 0, 0, 0x028F036C, 0, 0x028AA490};
static u8 sCoreAffinity[3] = {1, 2, 4};

/* The first three words of the software mixer (SoftwareMixer::interfaceState). */
struct SoftwareMixerHeader {
    be<u32> link[2];
    be<u32> vtable;
};

/* The mix runtime singleton accessor. */
/* inlined into 0289806C and 028983F8 */
static inline AudioMixRuntime* mixRuntime() {
    if (sMixRuntimeConstructed == 0) {
        sMixRuntimeConstructed = 1;
        AudioMixRuntime::create(&sMixRuntime);
    }
    return &sMixRuntime;
}

/* 0289806C */
void SoundSystemConfig::initialize(u32 buffer) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sSoundSystem.initialized != 0) return;
    u32 next = (buffer + 31) & ~31u;
    u32 commandReady;
    if (softwareMixEnabled != 0) {
        u32 channelBytes = (u32)softwareChannelCount * kSoftwareChannelBytes;
        u32 channelBuffer = next;
        u32 mixerAddress = next + channelBytes;
        SoftwareMixer* mixer = gabi::at<SoftwareMixer>(mixerAddress);
        next = mixerAddress + sizeof(SoftwareMixer);
        if (mixer != NULL) {
            // the inlined SoftwareMixer constructor (placement new into the buffer)
            bzero(mixer, (u32)sizeof(SoftwareMixer));   // u32: the guest bzero, not the host one
            SoftwareMixerHeader* header = (SoftwareMixerHeader*)mixer;
            header->link[1] = 0;
            header->link[0] = 0;
            header->vtable = gabi::ea(sSoftwareMixerVtable);
            constructList(&mixer->manager.active);
            constructList(&mixer->manager.free);
            __construct_array(mixer->manager.voices, 96, sizeof(SoftwareVoice), (void* (*)(void*))SoftwareVoice::create);
            nw::ut::MessageQueue::create(&mixer->queue.main);
            nw::ut::MessageQueue::create(&mixer->queue.remote);
            __construct_array(mixer->queue.blocks, 16, sizeof(MixBlock), (void* (*)(void*))MixBlock::create);
        }
        sSoundSystem.softwareMixer = mixer;
        initializeSoftwareMixer(mixer, gabi::at<be<f32>>(channelBuffer), channelBytes);
        AudioFrameDriver* driver = AudioFrameDriver::instance();
        driver->mixer = sSoundSystem.softwareMixer;
        commandReady = sSoundSystem.commandStorageReady;
        sSoundSystem.softwareMixReady = 1;
    } else {
        commandReady = sSoundSystem.commandStorageReady;
        sSoundSystem.softwareMixReady = 0;
    }
    if (!commandReady) {
        u32 bytes = commandStorageBytes();
        initializeCommandStorage(next);
        next += bytes;
    }
    mixRuntime()->enable();
    RemoteSpeakerManager* speakers = RemoteSpeakerManager::instance();
    speakers->initialize();
    u32 driverBytes = driverStackBytes;
    sSoundSystem.driverStack = next;
    u32 driverBytesReloaded = driverStackBytes;
    next += driverBytes;
    sSoundSystem.driverStackBytes = driverBytesReloaded;
    AudioFrameDriver* driver = AudioFrameDriver::instance();
    driver->idleHook();
    u32 workerEnd = next + (u32)workerStackBytes;
    AudioWorkQueue::instance()->initializeMessages();
    sSoundSystem.workerStack = next;
    sSoundSystem.workerStackBytes = workerStackBytes;
    PlaybackVoicePool* voices = PlaybackVoicePool::instance();
    u32 voiceBytes = voices->requiredBytes(sSoundSystem.channelCount);
    next = workerEnd + voiceBytes;
    voices = PlaybackVoicePool::instance();
    u32 voiceStorageBytes = voices->requiredBytes(sSoundSystem.channelCount);
    voices = PlaybackVoicePool::instance();
    voices->initialize(gabi::at<PlaybackVoice>(workerEnd), voiceStorageBytes);
    WaveChannelPool* channels = WaveChannelPool::instance();
    WaveChannelPool::requiredBytes(channels, sSoundSystem.channelCount);
    channels = WaveChannelPool::instance();
    u32 channelBytes = WaveChannelPool::requiredBytes(channels, sSoundSystem.channelCount);
    channels = WaveChannelPool::instance();
    channels->initialize(gabi::at<void>(next), channelBytes);
    SequenceRuntimeSupport::initializeGlobalVariables();
    u32 core = OSGetCoreId();
    u32 affinity = core <= 2 ? sCoreAffinity[core] : 0;
    AudioWorker* worker = AudioWorker::instance();
    u32 workerBytes = sSoundSystem.workerStackBytes;
    u32 workerStack = sSoundSystem.workerStack;
    u32 workerPriorityValue = workerPriority;
    worker->start(workerPriorityValue, workerStack, workerBytes, affinity);
    driver = AudioFrameDriver::instance();
    u32 option = driverOption;
    u32 driverPriorityValue = driverPriority;
    u32 driverStack = sSoundSystem.driverStack;
    u32 driverStackSize = sSoundSystem.driverStackBytes;
    driver->start(driverPriorityValue, driverStack, driverStackSize, affinity, option);
    ModulationCurve::initializeTable();
    sSoundSystem.initialized = 1;
}

/* 028983F8 */
void finalizeSoundSystem() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sSoundSystem.initialized == 0) return;
    AudioWorkQueue::instance()->idleHook();
    AudioWorker::instance()->stop();
    AudioWorkQueue::instance()->finishMessages();
    AudioFrameDriver* driver = AudioFrameDriver::instance();
    driver->stop();
    RemoteSpeakerManager* speakers = RemoteSpeakerManager::instance();
    speakers->finalize();
    WaveChannelPool::instance()->finish();
    PlaybackVoicePool* voices = PlaybackVoicePool::instance();
    voices->finalize();
    mixRuntime()->disable();
    if (sSoundSystem.softwareMixReady != 0) {
        driver = AudioFrameDriver::instance();
        driver->mixer = NULL;
        finalizeSoftwareMixer(sSoundSystem.softwareMixer);
        sSoundSystem.softwareMixReady = 0;
    }
    driver = AudioFrameDriver::instance();
    driver->finalize();
    DriverCommandManager* manager = DriverCommandManager::instance();
    manager->finalize();
    manager = DriverCommandManager::taskThreadInstance();
    manager->finalize();
    sSoundSystem.initialized = 0;
    sSoundSystem.commandStorageReady = 0;
}

/* inlined into 0289533C */
u32 isSoftwareMixReady() { return sSoundSystem.softwareMixReady; }

/* WWHD glue
 data sSoundSystem 101FC8D8
 data sFrameDriverVtable 101822AC
 data sFrameDriver 104B7810
 data sFrameDriverConstructed 104BB4C0
 data sFrameDriverExit 101FC8F8
 data sMixRuntime 101FCE00
 data sMixRuntimeConstructed 101FD8C8
 data sSoftwareMixerVtable 10187E4C
 data sCoreAffinity 10182290
 ext 028F6A60 nw::ut::Thread* nw::ut::Thread::create(nw::ut::Thread*,nw::ut::ThreadTarget*)
 ext 028F6BEC nw::ut::MessageQueue* nw::ut::MessageQueue::create(nw::ut::MessageQueue*)
 ext 028F6C2C void nw::ut::MessageQueue::initialize(void*,s32)
 ext C0009BB0 u32 OSGetCoreId()
 ext 02885F68 nw::snd::DriverCommandManager* nw::snd::DriverCommandManager::taskThreadInstance()
 ext 02886D8C void nw::snd::DriverCommandManager::prepare(nw::snd::DriverCommand*,u32)
 ext 02886FC0 void nw::snd::DriverCommandManager::finalize()
 ext 02888A50 nw::snd::AudioMixRuntime* nw::snd::AudioMixRuntime::create(nw::snd::AudioMixRuntime*)
 ext 02889148 void nw::snd::AudioMixRuntime::enable()
 ext 028893F8 void nw::snd::AudioMixRuntime::disable()
 ext 028A8B40 nw::snd::SoftwareVoice* nw::snd::SoftwareVoice::create(nw::snd::SoftwareVoice*) callback
 ext 028A89E4 nw::snd::MixBlock* nw::snd::MixBlock::create(nw::snd::MixBlock*) callback
 ext 028AA080 void initializeSoftwareMixer(nw::snd::SoftwareMixer*,be<f32>*,u32)
 ext 028AA174 void finalizeSoftwareMixer(nw::snd::SoftwareMixer*)
 ext 0289C1A4 nw::snd::AudioWorkQueue* nw::snd::AudioWorkQueue::instance()
 ext 0289C1FC void nw::snd::AudioWorkQueue::initializeMessages()
 ext 0289C6EC void nw::snd::AudioWorkQueue::idleHook()
 ext 0289C20C void nw::snd::AudioWorkQueue::finishMessages()
 ext 0289C940 nw::snd::AudioWorker* nw::snd::AudioWorker::instance()
 ext 0289C890 u32 nw::snd::AudioWorker::start(u32,u32,u32,u32)
 ext 0289C7C0 void nw::snd::AudioWorker::stop()
 ext 02898728 u32 nw::snd::AudioFrameDriver::start(u32,u32,u32,u32,u32)
 ext 028987D8 void nw::snd::AudioFrameDriver::stop()
 ext 0289882C void nw::snd::AudioFrameDriver::finalize()
 ext 028990A8 void nw::snd::AudioFrameDriver::idleHook()
 ext 028A4DE0 nw::snd::WaveChannelPool* nw::snd::WaveChannelPool::instance()
 ext 028A4E2C u32 nw::snd::WaveChannelPool::requiredBytes(void*,u32)
 ext 028A4E38 void nw::snd::WaveChannelPool::initialize(void*,u32)
 ext 028A4EE4 void nw::snd::WaveChannelPool::finish()
 ext 0288B8F4 void nw::snd::SequenceRuntimeSupport::initializeGlobalVariables()
 ext 028A5A1C void nw::snd::ModulationCurve::initializeTable()
 ext 0289F364 nw::snd::PlaybackVoicePool* nw::snd::PlaybackVoicePool::instance()
 ext 0289F3B0 u32 nw::snd::PlaybackVoicePool::requiredBytes(u32) const
 ext 0289F3B8 void nw::snd::PlaybackVoicePool::initialize(nw::snd::PlaybackVoice*,u32)
 ext 0289F4E4 void nw::snd::PlaybackVoicePool::finalize()
 ext 028A2138 nw::snd::RemoteSpeakerManager* nw::snd::RemoteSpeakerManager::instance()
 ext 028A2240 void nw::snd::RemoteSpeakerManager::initialize()
 ext 028A2300 void nw::snd::RemoteSpeakerManager::finalize()
*/

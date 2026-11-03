/* Cafe final-mix bridge, reconstructed from the assigned WWHD instructions.
 * The two device banks and event extent follow the constructor and event call
 * operands; no external SDK or decompilation supplies this layout. */
#include "JSystem/JAudio/JASDeviceMixer.h"

static JASBridge::DeviceMixer* deviceMixerSingleton;
static void* deviceMixerHeap;
static JASBridge::MixCallbackSlot tvMixCallback;
static JASBridge::MixCallbackSlot controllerMixCallback;
static JASBridge::MixHeaderStatics mixerHeaderStatics;
static JASBridge::MixDestructorRecord mixerDestructorRecord;

/* 0281A2D0 */
void JASBridge::DeviceMixer::setVolumes(f32 left, f32 right) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    volume[0] = gabi::ftoi(gabi::fmuls_ppc(256.0f, left));
    volume[1] = gabi::ftoi(gabi::fmuls_ppc(256.0f, right));
}
/* 0281A968 */
u32 JASBridge::DeviceMixer::exchangeCompletion(u32 callback, u32 device) {
    u32 previous = completionCallback[device];
    completionCallback[device] = callback;
    return previous;
}



/* 0281A1FC */
JASBridge::DeviceMixer* JASBridge::constructDeviceMixer(DeviceMixer* object) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (!object) object = static_cast<DeviceMixer*>(allocateDeviceMixer(sizeof(DeviceMixer)));
    if (object) {
        object->initialized = 0;
        object->active = 0;
        deviceMixerSingleton = object;
        for (u32 device = 0; device < 2; ++device) {
            for (u32 bank = 0; bank < 2; ++bank) {
                object->queued[device][bank].samples = nullptr;
                object->queued[device][bank].sampleCount = 0;
            }
            object->activeBuffer[device] = 0;
            object->sampleOffset[device] = 0;
            object->completionCallback[device] = 0;
            object->outputMode[device] = 1;
        }
    }
    return object;
}
/* 0281A28C */
void JASBridge::initializeDeviceMixer() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    auto* object = static_cast<DeviceMixer*>(allocateDeviceMixerOnHeap(sizeof(DeviceMixer), deviceMixerHeap, 4));
    if (object) object = constructDeviceMixer(object);
    deviceMixerSingleton = object;
}
/* 0281A4E0 */
void JASBridge::DeviceMixer::finishBuffer(u32 device) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    s32 next = (s32)((u32)activeBuffer[device] + 1) % 2;
    if (queued[device][next].samples.get()) activeBuffer[device] = next;
    sampleOffset[device] = 0;
    u32 callback = completionCallback[device];
    if (callback) gabi::tail_ptr<void>(callback, this, device);
}
/* 0281A91C */
void JASBridge::DeviceMixer::configureUpsampling(u32 mode) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 enabled = mode == 0;
    configureDeviceUpsampling(0, enabled);
    configureDeviceUpsampling(1, enabled);
}
/* 0281A97C */
void JASBridge::DeviceMixer::start() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    waitDeviceMixerEvent(&event);
    active = 1;
}
/* 0281A9B4 */
void JASBridge::DeviceMixer::stop() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    waitDeviceMixerEvent(&event);
    active = 0;
}
/* 0281A9EC */
void JASBridge::DeviceMixer::submit(be<s16>* samples, u32 bytes, u32 device) {
    s32 current = activeBuffer[device];
    if (!queued[device][current].samples.get()) {
        queued[device][current].samples = samples;
        current = activeBuffer[device];
        queued[device][current].sampleCount = bytes >> 1;
    } else {
        s32 next = (s32)((u32)current + 1) % 2;
        queued[device][next].samples = samples;
        queued[device][next].sampleCount = bytes >> 1;
    }
}
/* 0281AA60 */
void JASBridge::DeviceMixer::setOutputMode(u32 device, u32 mode) {
    outputMode[device] = mode;
}

/* 0281A310 */
void JASBridge::DeviceMixer::downmix(u32 device, be<s16>* left, be<s16>* right,
                                    be<s16>* surroundLeft, be<s16>* surroundRight) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 mode = outputMode[device];
    if (mode == 2) return;
    /* The surround channels fold into the front pair at -3 dB. */
    s32 rearRight = *surroundRight;
    s32 mixedRight = (s32)*right + gabi::ftoi(gabi::fmuls_ppc((f32)rearRight, 0.707f));
    if (mixedRight < -32768) mixedRight = -32768;
    else if (mixedRight > 32767) mixedRight = 32767;
    s32 rearLeft = *surroundLeft;
    s32 mixedLeft = (s32)*left + gabi::ftoi(gabi::fmuls_ppc((f32)rearLeft, 0.707f));
    if (mixedLeft < -32768) mixedLeft = -32768;
    else if (mixedLeft > 32767) mixedLeft = 32767;
    if (mode != 1) {
        /* mono: both outputs carry the 16-bit sum at -3 dB */
        f32 sum = gabi::fadds_ppc((f32)mixedRight, (f32)mixedLeft);
        mixedRight = (s16)gabi::ftoi(gabi::fmuls_ppc(sum, 0.707f));
        mixedLeft = mixedRight;
    }
    *right = mixedRight;
    *surroundRight = 0;
    *left = mixedLeft;
    *surroundLeft = 0;
}
/* 0281A540 */
void JASBridge::DeviceMixer::accumulate(u32 device, be<s32>* planes, u32 format, s32 sampleCount) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (!queued[device][(s32)activeBuffer[device]].samples.get()) return;
    u32 channels = outputMode[device] == 1 || format == 2 ? 2 : 4;
    be<s32>* output[4];
    output[1] = planes;
    output[0] = planes + sampleCount;
    if (channels != 2) {
        output[3] = planes + (s32)((u32)sampleCount * 2);
        output[2] = planes + (s32)((u32)sampleCount * 3);
    }
    for (s32 sample = 0; sample < sampleCount; ++sample) {
        be<s16>* input = queued[device][(s32)activeBuffer[device]].samples.get() + (u32)sampleOffset[device];
        downmix(device, input, input + 1, input + 2, input + 3);
        for (u32 channel = 0; channel < channels; ++channel) {
            s32 gain = volume[device];
            s32 value = input[channel];
            be<s32>* destination = output[channel];
            s32 product = (s32)((u32)value * (u32)gain);
            destination[sample] = (s32)((u32)(s32)destination[sample] + (u32)(product >> 8));
        }
        s32 current = activeBuffer[device];
        u32 offset = (u32)sampleOffset[device] + 4;
        sampleOffset[device] = offset;
        if (offset >= (u32)queued[device][current].sampleCount) finishBuffer(device);
    }
}

/* 0281A6C4 */
void JASBridge::DeviceMixer::processFinalMix(u32 device, FinalMix* mix) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (active) {
        s32 samples = mix->sampleCount;
        gptr<be<s32>>* planes = mix->planes;
        u32 format = mix->format;
        accumulate(device, planes[0], format, samples);
    }
}
/* 0281A6E4 */
void JASBridge::DeviceMixer::consumeTv(FinalMix* mix) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    signalDeviceMixerEvent(&event);
    processFinalMix(0, mix);
    resetDeviceMixerEvent(&event);
}
/* 0281A738 */
void JASBridge::dispatchTvFinalMix(FinalMix* mix) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 previous = tvMixCallback.target;
    if (previous) gabi::call_ptr<void>(previous, mix);
    deviceMixerSingleton->consumeTv(mix);
}
/* 0281A78C */
void JASBridge::installTvFinalMix() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    getDeviceMixCallback(0, &tvMixCallback.target);
    registerDeviceMixCallback(0, dispatchTvFinalMix);
}
/* 0281A7C8 */
void JASBridge::DeviceMixer::consumeController(FinalMix* mix) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    signalDeviceMixerEvent(&event);
    processFinalMix(1, mix);
    resetDeviceMixerEvent(&event);
}
/* 0281A81C */
void JASBridge::dispatchControllerFinalMix(FinalMix* mix) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 previous = controllerMixCallback.target;
    if (previous) gabi::call_ptr<void>(previous, mix);
    deviceMixerSingleton->consumeController(mix);
}
/* 0281A870 */
void JASBridge::installControllerFinalMix() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    getDeviceMixCallback(1, &controllerMixCallback.target);
    registerDeviceMixCallback(1, dispatchControllerFinalMix);
}
/* 0281A8AC */
void JASBridge::DeviceMixer::initialize() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (!initialized) {
        setVolumes(1.0f, 0.0f);
        installTvFinalMix();
        installControllerFinalMix();
        initializeDeviceMixerEvent(&event, 0, 0);
        initialized = 1;
    }
}
/* 0281AA70 */
void JASBridge::initializeDeviceMixerStatics() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    mixerHeaderStatics.words[2] = 0;
    mixerHeaderStatics.words[0] = 0;
    mixerHeaderStatics.words[3] = 0;
    mixerHeaderStatics.words[1] = 0;
    registerDeviceMixerStatics(&mixerDestructorRecord);
}

/* WWHD glue
data deviceMixerSingleton 101F9FF4
data deviceMixerHeap 101F9DA8
data tvMixCallback 101F9FF8
data controllerMixCallback 101F9FFC
data mixerHeaderStatics 104B56B0
data mixerDestructorRecord 101F9FE8
ext 028F026C void registerDeviceMixerStatics(JASBridge::MixDestructorRecord*)
ext 0273AD10 void* allocateDeviceMixer(u32)
ext 0273B050 void* allocateDeviceMixerOnHeap(u32,void*,s32)
ext C000A1F0 void waitDeviceMixerEvent(JASBridge::MixerEvent*)
ext C0006FB0 void configureDeviceUpsampling(u32,u32)
ext C0009F18 void resetDeviceMixerEvent(JASBridge::MixerEvent*)
ext C000A0B0 void signalDeviceMixerEvent(JASBridge::MixerEvent*)
ext C0009D48 void initializeDeviceMixerEvent(JASBridge::MixerEvent*,u32,u32)
ext C0006D38 void getDeviceMixCallback(u32,be<u32>*)
ext C0006ED8 void registerDeviceMixCallback(u32,JASBridge::FinalMixCallback)
*/

#pragma once
#include "bindings.h"

namespace JASBridge {
struct MixBuffer {
    gptr<be<s16>> samples;
    be<u32> sampleCount;
};
struct FinalMix {
    gptr<gptr<be<s32>>> planes;
    be<u16> format;
    be<u16> sampleCount;
};
struct MixCallbackSlot { be<u32> target; };
struct MixHeaderStatics { be<u32> words[4]; };
struct MixDestructorRecord { be<u32> words[3]; };
using FinalMixCallback = void(*)(FinalMix*);
struct MixerEvent { u8 storage[0x24]; };
class DeviceMixer {
public:
    void consumeTv(FinalMix* mix);
    void consumeController(FinalMix* mix);
    void processFinalMix(u32 device, FinalMix* mix);
    void initialize();
    void downmix(u32 device, be<s16>* left, be<s16>* right, be<s16>* surroundLeft, be<s16>* surroundRight);
    void accumulate(u32 device, be<s32>* planes, u32 format, s32 sampleCount);
    void start();
    void stop();
    void submit(be<s16>* samples, u32 bytes, u32 device);
    void setOutputMode(u32 device, u32 mode);
    void finishBuffer(u32 device);
    void configureUpsampling(u32 mode);
    void setVolumes(f32 left, f32 right);
    u32 exchangeCompletion(u32 callback, u32 device);

    be<u8> initialized;
    be<u8> active;
    u8 padding[2];
    MixBuffer queued[2][2];
    be<s32> activeBuffer[2];
    be<u32> sampleOffset[2];
    be<u32> completionCallback[2];
    be<s32> volume[2];
    be<u32> outputMode[2];
    MixerEvent event;
};
DeviceMixer* constructDeviceMixer(DeviceMixer* object);
void initializeDeviceMixer();
void dispatchTvFinalMix(FinalMix* mix);
void dispatchControllerFinalMix(FinalMix* mix);
void installTvFinalMix();
void installControllerFinalMix();
void initializeDeviceMixerStatics();
WWHD_SIZE(FinalMix, 8);
WWHD_SIZE(MixBuffer, 8);
WWHD_OFFSET(DeviceMixer, activeBuffer, 0x24);
WWHD_OFFSET(DeviceMixer, volume, 0x3C);
WWHD_OFFSET(DeviceMixer, event, 0x4C);
WWHD_SIZE(DeviceMixer, 0x70);
}

static void* allocateDeviceMixer(u32 size);
static void* allocateDeviceMixerOnHeap(u32 size, void* heap, s32 alignment);
static void waitDeviceMixerEvent(JASBridge::MixerEvent* event);
static void configureDeviceUpsampling(u32 device, u32 enabled);

static void resetDeviceMixerEvent(JASBridge::MixerEvent* event);
static void signalDeviceMixerEvent(JASBridge::MixerEvent* event);
static void initializeDeviceMixerEvent(JASBridge::MixerEvent* event, u32 initial, u32 mode);
static void getDeviceMixCallback(u32 device, be<u32>* previous);
static void registerDeviceMixCallback(u32 device, JASBridge::FinalMixCallback callback);
static void registerDeviceMixerStatics(JASBridge::MixDestructorRecord* record);

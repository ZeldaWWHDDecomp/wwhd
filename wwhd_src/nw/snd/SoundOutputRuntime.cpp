/* Sound output interpolation and effect chains, reconstructed from WWHD's own code.
 * SDK boundary facts: wut gives the arguments of AXSetAuxReturnVolume and AXRegisterAuxCallback (facts only). */
#include "nw/snd/SoundRuntimeSupport.h"
#include "nw/snd/BasicSound.h"
using namespace nw::snd;
// autoglue ext lines bind namespaced names as members, so the NW-SND#2 helpers nw::snd::selectMixDevice,
// auxDeviceBus and flattenAuxBus are bound here under local names.
u32 resolveMixOutput(u32,MixOutputDevice*);
u32 mixAuxDeviceBus(u32);
u32 mixFlattenAuxBus(u32,u32);
struct RuntimeEffectTable { be<u32> entry; };
static RuntimeEffectTable sEffectBaseVtable;
static RuntimeEffectTable sEffectFrameVtable;
static RuntimeEffectTable sEffectInitVtable;
static RuntimeEffectTable sEffectShutdownVtable;
static RuntimeEffectTable sEffectProcessVtable;
struct EffectTableRoots {
    gptr<RuntimeEffectTable> base,frame,initialize,shutdown,process;
};
static EffectTableRoots sEffectTables;
struct OutputDirtyCommand : DriverCommand { be<u32> mask; };
enum { CommandOutputDirty = 0x4B, MasterVolumeDirty = 0x40 };

/* 02889B10 */
u32 nw::snd::addAuxEffect(AudioMixRuntime* runtime,u32 bus,void* effect,u32 device) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 index = device * 3 + bus;
    MoveValue* ramp = &runtime->auxVolume[index];
    if (ramp->counter < ramp->frames) runtime->resetAuxChain(bus,device);
    f32 current = ramp->value();
    ramp->origin = current;
    ramp->frames = 0;
    ramp->target = 1.0f;
    ramp->counter = 0;
    gabi::FrameLocal<MixOutputDevice> route(8);
    if (!resolveMixOutput(device,route.get())) return 0;
    u32 aux = mixAuxDeviceBus(bus);
    u32 routeIndex = route->index;
    u32 routeDevice = route->device;
    if (AXSetAuxReturnVolume(routeDevice,routeIndex,aux,0x8000) < 0) return 0;
    MixEffectChain* chain = &runtime->auxEffects[index];
    if (!chain->count) {
        u32 context = mixFlattenAuxBus(bus,device);
        routeIndex = route->index;
        routeDevice = route->device;
        if (AXRegisterAuxCallback(routeDevice,routeIndex,aux,gabi::guest_fn((const void*)processAuxMix),context) < 0) return 0;
    }
    gabi::FrameLocal<gptr<SoundListNode>> cursor(0x18);
    *cursor = &chain->sentinel;
    runtimeListInsert(chain,cursor.get(),effect);
    return 1;
}
/* 02889CAC */
void nw::snd::fadeAuxEffect(AudioMixRuntime* runtime,u32 bus,u32 frames,u32 device) {
    MoveValue* ramp = &runtime->auxVolume[device * 3 + bus];
    if (frames == 0) {
        runtime->resetAuxChain(bus,device);
        if (ramp->counter < ramp->frames) return;
        f32 current = ramp->value();
        ramp->target = 0.0f;
        ramp->frames = 0;
        ramp->origin = current;
        ramp->counter = 0;
    } else {
        u32 duration = (frames + 2) / 3;
        f32 current = ramp->value();
        ramp->target = 0.0f;
        ramp->frames = duration;
        ramp->origin = current;
        ramp->counter = 0;
    }
}
/* 02889E48 */
f32 nw::snd::outputMasterVolume(AudioMixRuntime* runtime) {
    f32 base = runtime->mainVolume.value();
    f32 fade = runtime->drcVolume.value();
    return gabi::fmuls_ppc(base,fade);
}
/* 02889F18 */
void nw::snd::setOutputMasterVolume(AudioMixRuntime* runtime,f32 value,u32 frames) {
    u32 duration = (frames + 2) / 3;
    value = value >= 0.0f ? value : 0.0f;
    f32 current = runtime->mainVolume.value();
    runtime->mainVolume.origin = current;
    runtime->mainVolume.target = value;
    runtime->mainVolume.frames = duration;
    runtime->mainVolume.counter = 0;
    if (frames == 0) {
        DriverCommandManager* manager = DriverCommandManager::instance();
        OutputDirtyCommand* command = (OutputDirtyCommand*)manager->allocCommand(sizeof(OutputDirtyCommand));
        command->id = CommandOutputDirty;
        command->mask = MasterVolumeDirty;
        manager->pushCommand(command);
    }
}
/* 0288A004 */
void nw::snd::addOutputFrameCallback(AudioMixRuntime* runtime,void* callback) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<gptr<SoundListNode>> cursor(8);
    *cursor = &runtime->tvEffects.sentinel;
    runtimeListInsert(&runtime->tvEffects,cursor.get(),callback);
}
/* 0288A038 */
void* nw::snd::removeOutputFrameCallback(AudioMixRuntime* runtime,void* callback) {
    return runtimeListRemove(&runtime->tvEffects,callback);
}
/* 0288A040 */
static void initializeEffectTableRoots() {
    sEffectTables.frame = &sEffectFrameVtable;
    sEffectTables.base = &sEffectBaseVtable;
    sEffectTables.initialize = &sEffectInitVtable;
    sEffectTables.shutdown = &sEffectShutdownVtable;
    sEffectTables.process = &sEffectProcessVtable;
}
/* WWHD glue
data sEffectBaseVtable 10186C4C
data sEffectFrameVtable 10186C64
data sEffectInitVtable 10186C7C
data sEffectShutdownVtable 10186C94
data sEffectProcessVtable 10186CAC
data sEffectTables 104B71D8
ext 028894A0 void nw::snd::AudioMixRuntime::resetAuxChain(u32,u32)
ext 0288945C u32 resolveMixOutput(u32,nw::snd::MixOutputDevice*)
ext 0288949C u32 mixAuxDeviceBus(u32)
ext 028898F4 u32 mixFlattenAuxBus(u32,u32)
ext C0006F20 s32 AXSetAuxReturnVolume(u32,u32,u32,u32)
ext C0006EB8 s32 AXRegisterAuxCallback(u32,u32,u32,u32,u32)
ext 02889954 void processAuxMix(be<u32>*,s32,nw::snd::AuxMixFrame*) callback
ext 0286DA70 void* runtimeListInsert(nw::snd::MixEffectChain*,const gptr<nw::snd::SoundListNode>*,void*)
ext 0286DA9C void* runtimeListRemove(nw::snd::MixEffectChain*,void*)
ext 02885F10 nw::snd::DriverCommandManager* nw::snd::DriverCommandManager::instance()
ext 02887248 nw::snd::DriverCommand* nw::snd::DriverCommandManager::allocCommand(u32)
ext 028870F0 u32 nw::snd::DriverCommandManager::pushCommand(nw::snd::DriverCommand*)
*/

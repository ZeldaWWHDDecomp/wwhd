/* nw::snd BasicSound, the game-thread side of a sound, and this translation unit's copies of the
 * driver command manager singletons. Reconstructed from WWHD's code; verified function by function
 * against the recompiled original (tools/verify, spec lib2_nw_snd_1). */

#include "nw/snd/BasicSound.h"
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
using namespace nw::snd;

static BasicSoundVtable sBasicSoundVtable;
struct InstanceCounter { be<u32> last; };
static InstanceCounter sInstanceCounter;
struct SingletonGuard { be<u32> constructed; };
/* GHS destructor registration records of the two singletons */
struct DestructorRecord { be<u32> next; be<u32> destructor; be<u32> object; };
static DestructorRecord sCommandManagerRecord;
static DestructorRecord sTaskThreadCommandManagerRecord;
static SingletonGuard sCommandManagerGuard;
static SingletonGuard sTaskThreadCommandManagerGuard;
static DriverCommandManager sCommandManager;
static DriverCommandManager sTaskThreadCommandManager;

/* A float that is only moved (lfs/stfs without arithmetic) keeps its exact bits. */
static inline void copyFloatBits(be<f32>& destination, const be<f32>& source) {
    reinterpret_cast<be<u32>&>(destination) = (u32) reinterpret_cast<const be<u32>&>(source);
}
static inline void copyWords(void* destination, const void* source, u32 size) {
    be<u32>* to = (be<u32>*)destination;
    const be<u32>* from = (const be<u32>*)source;
    for (u32 i = 0; i < size / 4; i++) to[i] = (u32)from[i];
}

/* GHS member constructor: tests the member's address and allocates when it is NULL. */
static inline void constructMoveValue(MoveValue* value) {
    if (!gabi::ea(value)) value = (MoveValue*)sndAllocate(sizeof(MoveValue));
    if (gabi::ea(value)) {
        value->target = 0.0f;
        value->frames = 0;
        value->origin = 0.0f;
        value->counter = 0;
    }
}

static inline void resetOutputAmbient(OutputAmbientParam* output) {
    output->volume = 1.0f;
    output->surroundPan = 0.0f;
    output->pan = 0.0f;
    for (int i = 0; i < 3; i++) output->fxSend[i] = 0.0f;
}

static inline void resetAmbientState(AmbientState* ambient) {
    ambient->lpfFrequency = 0.0f;
    ambient->priority = 0;
    ambient->userData = 0;
    ambient->biquadValue = 0.0f;
    ambient->outputLine = -1;
    ambient->volume = 1.0f;
    ambient->pitch = 1.0f;
    ambient->biquadType = -1;
    resetOutputAmbient(&ambient->tv);
    for (int i = 0; i < 2; i++) resetOutputAmbient(&ambient->drc[i]);
}

static inline void resetActorParam(ActorParam* param) {
    param->tvVolume = 1.0f;
    param->pitch = 1.0f;
    param->volume = 1.0f;
    param->tvPan = 0.0f;
    for (int i = 0; i < 2; i++) {
        param->drcVolume[i] = 1.0f;
        param->drcPan[i] = 0.0f;
    }
}

static inline void resetOutputParam(OutputParam* output) {
    output->volume = 1.0f;
    output->mainSend = 0.0f;
    output->mixMode = 0;
    output->pan = 0.0f;
    output->surroundPan = 0.0f;
    for (int i = 0; i < 3; i++) output->fxSend[i] = 0.0f;
}

static inline PlayerFlagCommand* allocPlayerCommand(DriverCommandManager* manager, u32 id, BasicSound* sound) {
    PlayerFlagCommand* command = (PlayerFlagCommand*)manager->allocCommand(sizeof(PlayerFlagCommand));
    command->id = id;
    command->player = sound->callGetDriverPlayer();
    return command;
}

/* 02883C64 */
BasicSound* BasicSound::create(BasicSound* sound) {
    if (!sound) {
        sound = (BasicSound*)sndAllocate(sizeof(BasicSound));
        if (!sound) return sound;
    }
    /* AmbientState constructor: the TV output inline, the DRC outputs as an array, then the body. */
    sound->ambient.tv.volume = 1.0f;
    sound->vtable = &sBasicSoundVtable;
    sound->ambient.tv.pan = 0.0f;
    sound->ambient.tv.surroundPan = 0.0f;
    for (int i = 0; i < 3; i++) sound->ambient.tv.fxSend[i] = 0.0f;
    __construct_array(sound->ambient.drc, 2, sizeof(OutputAmbientParam), (void* (*)(void*))OutputAmbientParam::create);
    resetAmbientState(&sound->ambient);
    resetActorParam(&sound->actorParam);
    constructMoveValue(&sound->fadeVolume);
    constructMoveValue(&sound->pauseFadeVolume);
    constructMoveValue(&sound->muteFadeVolume);
    sound->state = SoundState_Constructed;
    constructMoveValue(&sound->volume);
    __construct_array(sound->output, 3, sizeof(OutputParam), (void* (*)(void*))OutputParam::create);
    sound->actorLink.previous = 0;
    sound->actorLink.next = 0;
    sound->poolLink.previous = 0;
    sound->poolLink.next = 0;
    sound->userDataSize = 0;
    sound->userData = nullptr;
    sound->finalizeCallback = 0;
    sound->playerLink.previous = 0;
    sound->playerLink.next = 0;
    sound->priorityLink.previous = 0;
    sound->priorityLink.next = 0;
    return sound;
}

/* 02883EDC */
void BasicSound::sendClearStopRequest() {
    DriverCommandManager* manager = DriverCommandManager::instance();
    PlayerCommand* command = (PlayerCommand*)manager->allocCommand(sizeof(PlayerCommand));
    command->id = DriverCommand_ClearStopRequest;
    command->player = callGetDriverPlayer();
    manager->pushCommand(command);
}

/* 02883F54 */
void BasicSound::setPlayerPriority(s32 priority) {
    SoundPlayer* player = soundPlayer;
    playerPriority = (s8)priority;
    if (player) player->updatePriority(this);
    callOnUpdatePlayerPriority();
}

/* 02883FA8 */
void BasicSound::stop(s32 fadeFrames) {
    if (fadeFrames == 0 || pauseState == PauseState_Paused || (!startRequested && !started)) {
        callFinalize();
        return;
    }
    s32 frames = gabi::ftoi(gabi::fmuls_ppc((f32)fadeFrames, fadeVolume.value()));
    fadeVolume.setTarget(0.0f, frames);
    setPlayerPriority(0);
    autoStopEnabled = false;
    pauseState = PauseState_None;
    unpauseRequested = false;
    stopAfterFade = true;
    muteState = PauseState_None;
}

/* 02884198 */
void BasicSound::addOutputOffset(OutputParam* param) {
    param->mixMode = (u32)outputOffset.mixMode;
    param->pan = gabi::fadds_ppc(param->pan, outputOffset.pan);
    param->surroundPan = gabi::fadds_ppc(param->surroundPan, outputOffset.surroundPan);
    param->mainSend = gabi::fadds_ppc(param->mainSend, outputOffset.mainSend);
    for (int i = 0; i < 3; i++) param->fxSend[i] = gabi::fadds_ppc(param->fxSend[i], outputOffset.fxSend[i]);
}

/* 02884200 */
void BasicSound::updateParam() {
    callOnUpdateParam();
    SoundPlayer* player = soundPlayer;
    f32 totalVolume;
    if (muteState == PauseState_Paused) {
        totalVolume = 0.0f;
    } else {
        totalVolume = gabi::fmuls_ppc(initialVolume, player->volume);
        totalVolume = gabi::fmuls_ppc(totalVolume, volume.value());
        totalVolume = gabi::fmuls_ppc(totalVolume, fadeVolume.value());
        totalVolume = gabi::fmuls_ppc(totalVolume, pauseFadeVolume.value());
        totalVolume = gabi::fmuls_ppc(totalVolume, muteFadeVolume.value());
        totalVolume = gabi::fmuls_ppc(totalVolume, ambient.volume);
        totalVolume = gabi::fmuls_ppc(totalVolume, actorParam.volume);
    }
    f32 totalPitch = gabi::fmuls_ppc(gabi::fmuls_ppc(pitch, ambient.pitch), actorParam.pitch);
    f32 totalLpf = gabi::fadds_ppc(gabi::fadds_ppc(lpfFrequency, ambient.lpfFrequency), player->lpfFrequency);

    s32 filterType = biquadType;
    f32 filterValue = biquadValue;
    if (filterType == -1) {
        filterType = player->biquadType;
        filterValue = player->biquadValue;
        if (filterType == -1) {
            filterValue = ambient.biquadValue;
            filterType = ambient.biquadType;
        }
    }
    u32 line = outputLine;
    if (ambient.outputLine != -1) line = (u32)(s32)ambient.outputLine;

    struct Outputs { OutputParam line[3]; };
    gabi::Local<Outputs> outputs;
    __construct_array(outputs->line, 3, sizeof(OutputParam), (void* (*)(void*))OutputParam::create);
    for (int i = 0; i < 3; i++) {
        copyWords(&outputs->line[i], &output[i], sizeof(OutputParam));
        addOutputOffset(&outputs->line[i]);
    }

    /* TV output */
    OutputParam* tv = &outputs->line[0];
    player = soundPlayer;
    f32 tvScale = gabi::fmuls_ppc(gabi::fmuls_ppc(player->outputs[0].volume, ambient.tv.volume), actorParam.tvVolume);
    tv->volume = gabi::fmuls_ppc(tv->volume, tvScale);
    tv->pan = gabi::fadds_ppc(tv->pan, gabi::fadds_ppc(ambient.tv.pan, actorParam.tvPan));
    tv->surroundPan = gabi::fadds_ppc(tv->surroundPan, ambient.tv.surroundPan);
    tv->mainSend = gabi::fadds_ppc(tv->mainSend, player->outputs[0].mainSend);
    for (int i = 0; i < 3; i++) {
        f32 send = gabi::fadds_ppc(player->outputs[0].fxSend[i], ambient.tv.fxSend[i]);
        tv->fxSend[i] = gabi::fadds_ppc(tv->fxSend[i], send);
    }
    /* DRC outputs */
    for (int drc = 0; drc < 2; drc++) {
        OutputParam* line = &outputs->line[drc + 1];
        OutputAmbientParam* source = &ambient.drc[drc];
        f32 scale = gabi::fmuls_ppc(gabi::fmuls_ppc(soundPlayer->outputs[1].volume, source->volume), actorParam.drcVolume[drc]);
        line->volume = gabi::fmuls_ppc(line->volume, scale);
        line->pan = gabi::fadds_ppc(line->pan, gabi::fadds_ppc(source->pan, actorParam.drcPan[drc]));
        line->surroundPan = gabi::fadds_ppc(line->surroundPan, source->surroundPan);
        line->mainSend = gabi::fadds_ppc(line->mainSend, soundPlayer->outputs[1].mainSend);
        /* WWHD indexes the destination with the DRC number: all three effect sends add into
         * fxSend[drc], the other two stay as copied. */
        for (int i = 0; i < 3; i++) {
            f32 send = gabi::fadds_ppc(soundPlayer->outputs[1].fxSend[i], source->fxSend[i]);
            line->fxSend[drc] = gabi::fadds_ppc(line->fxSend[drc], send);
        }
    }

    /* remote speakers: the stored values (through float registers), volume scaled by the player's
     * remote volume */
    f32 remoteVolume[4], remoteFilter[4], remoteBypass[4];
    for (int i = 0; i < 4; i++) {
        SoundPlayer* owner = soundPlayer;
        f32 storedVolume = remote[i].volume;
        remoteFilter[i] = remote[i].filter;
        remoteBypass[i] = remote[i].bypass;
        remoteVolume[i] = gabi::fmuls_ppc(storedVolume, owner->remoteVolume(i));
    }

    DriverCommandManager* manager = DriverCommandManager::instance();
    PlayerParamCommand* command = (PlayerParamCommand*)manager->allocCommand(sizeof(PlayerParamCommand));
    command->id = DriverCommand_PlayerParam;
    command->player = callGetDriverPlayer();
    command->volume = totalVolume;
    command->pitch = totalPitch;
    command->lpfFrequency = totalLpf;
    command->biquadType = filterType;
    command->biquadValue = filterValue;
    command->outputLine = line;
    for (int i = 0; i < 3; i++) copyWords(&command->output[i], &outputs->line[i], sizeof(OutputParam));
    for (int i = 0; i < 4; i++) {
        command->remote[i].volume = remoteVolume[i];
        command->remote[i].filter = remoteFilter[i];
        command->remote[i].bypass = remoteBypass[i];
    }
    manager->pushCommand(command);
}

/* 0288486C */
void BasicSound::update() {
    DriverSoundPlayer* driver = callGetDriverPlayer();
    if (driverInitialized && callGetDriverPlayer()->playFinished) {
        callFinalize();
        return;
    }
    if (driver->stopRequested) {
        sendClearStopRequest();
        callFinalize();
        return;
    }
    if (!callIsPrepared()) return;

    if (autoStopEnabled) {
        if (autoStopCounter == 0) {
            if (pauseState == PauseState_None || pauseState == PauseState_Unpausing) {
                stop(0);
                return;
            }
        } else {
            autoStopCounter = autoStopCounter - 1;
        }
    }

    bool startNow = false;
    if (!started) {
        if (!startRequested) return;
        if (!callIsPrepared()) return;
        startNow = true;
        playerState = PlayerState_Playing;
    }
    if (playerState == PlayerState_Playing) {
        if (updateCounter < 0xFFFFFFFFu) updateCounter = updateCounter + 1;
    }
    if (playerState == PlayerState_Playing && pauseState != PauseState_Paused) {
        if (playingCounter < 0xFFFFFFFFu) playingCounter = playingCounter + 1;
    }

    bool pauseFadeFinished = pauseFadeVolume.isFinished();
    switch (pauseState) {
    case PauseState_None:
        callUpdateMoveValue();
        break;
    case PauseState_Pausing:
        pauseFadeVolume.update();
        break;
    case PauseState_Unpausing:
        pauseFadeVolume.update();
        callUpdateMoveValue();
        break;
    default:
        break;
    }
    if (AmbientArgCallback* argCallback = ambientInfo.argCallback) {
        gabi::call_ptr<void>(argCallback->vtable->update.function, argCallback, ambientInfo.arg.get(), this);
    }

    if (ambientInfo.paramCallback) {
        gabi::Local<AmbientParam> param;
        param->priority = 0;
        param->userData = 0;
        param->tv.pan = 0.0f;
        param->biquadType = -1;
        param->pitch = 1.0f;
        param->outputLine = -1;
        param->tv.surroundPan = 0.0f;
        param->volume = 1.0f;
        param->tv.volume = 1.0f;
        param->lpfFrequency = 0.0f;
        param->biquadValue = 0.0f;
        for (int i = 0; i < 3; i++) param->tv.fxSend[i] = 0.0f;
        __construct_array(param->drc, 2, sizeof(OutputAmbientParam), (void* (*)(void*))OutputAmbientParam::create);
        resetOutputAmbient(&param->tv);
        for (int i = 0; i < 2; i++) resetOutputAmbient(&param->drc[i]);

        if (updateCounter != 0) {
            copyFloatBits(param->lpfFrequency, ambient.lpfFrequency);
            copyFloatBits(param->volume, ambient.volume);
            copyFloatBits(param->biquadValue, ambient.biquadValue);
            copyFloatBits(param->pitch, ambient.pitch);
            param->outputLine = (s32)ambient.outputLine;
            param->biquadType = (s32)ambient.biquadType;
            param->userData = (u32)ambient.userData;
            param->priority = (s32)ambient.priority;
            copyWords(&param->tv, &ambient.tv, sizeof(OutputAmbientParam));
            for (int i = 0; i < 2; i++) copyWords(&param->drc[i], &ambient.drc[i], sizeof(OutputAmbientParam));
        } else {
            param->userData = 0;
        }
        AmbientParamCallback* callback = ambientInfo.paramCallback;
        gabi::call_ptr<void>(callback->vtable->update.function, callback, ambientInfo.arg.get(), (u32)soundId, param.get());
        ambient.userData = (u32)param->userData;
        ambient.outputLine = (s32)param->outputLine;
        copyFloatBits(ambient.lpfFrequency, param->lpfFrequency);
        copyFloatBits(ambient.volume, param->volume);
        copyFloatBits(ambient.pitch, param->pitch);
        copyFloatBits(ambient.biquadValue, param->biquadValue);
        ambient.biquadType = (s32)param->biquadType;
        ambient.priority = (s32)param->priority;
        copyWords(&ambient.tv, &param->tv, sizeof(OutputAmbientParam));
        for (int i = 0; i < 2; i++) copyWords(&ambient.drc[i], &param->drc[i], sizeof(OutputAmbientParam));
    }

    if (SoundActor* actor = soundActor) {
        copyWords(&actorParam, &actor->param, sizeof(ActorParam));
    }
    updateParam();

    if (stopAfterFade && fadeVolume.isFinished()) {
        stopAfterFade = false;
        callFinalize();
        return;
    }

    DriverCommandManager* manager = DriverCommandManager::instance();
    if (startNow) {
        PlayerCommand* command = (PlayerCommand*)manager->allocCommand(sizeof(PlayerFlagCommand));
        command->id = DriverCommand_PlayerStart;
        command->player = callGetDriverPlayer();
        manager->pushCommand(command);
        startRequested = false;
        started = true;
    }
    if (pauseState == PauseState_Pausing) {
        if (pauseFadeFinished) {
            PlayerFlagCommand* command = (PlayerFlagCommand*)manager->allocCommand(sizeof(PlayerFlagCommand));
            command->id = DriverCommand_PlayerPause;
            command->player = callGetDriverPlayer();
            command->flag = true;
            manager->pushCommand(command);
            pauseState = PauseState_Paused;
        }
    } else if (pauseState == PauseState_Unpausing) {
        if (pauseFadeVolume.isFinished()) pauseState = PauseState_None;
    }
    if (unpauseRequested) {
        PlayerFlagCommand* command = (PlayerFlagCommand*)manager->allocCommand(sizeof(PlayerFlagCommand));
        command->id = DriverCommand_PlayerPause;
        command->player = callGetDriverPlayer();
        command->flag = false;
        manager->pushCommand(command);
        unpauseRequested = false;
    }
    if (muteState == PauseState_Pausing) {
        if (muteFadeVolume.isFinished()) muteState = PauseState_Paused;
    } else if (muteState == PauseState_Unpausing) {
        if (muteFadeVolume.isFinished()) muteState = PauseState_None;
    }
}

/* 028850D4 */
void BasicSound::startPrepared() { startRequested = true; }

/* 028850E0 */
void BasicSound::pause(u32 paused, s32 fadeFrames) {
    if (paused) {
        if (pauseState != PauseState_None && pauseState != PauseState_Pausing && pauseState != PauseState_Unpausing) return;
        s32 frames = gabi::ftoi(gabi::fmuls_ppc((f32)fadeFrames, pauseFadeVolume.value()));
        if (frames <= 0) frames = 1;
        pauseFadeVolume.setTarget(0.0f, frames);
        pauseState = PauseState_Pausing;
        unpauseRequested = false;
    } else {
        if (pauseState < PauseState_Pausing || pauseState > PauseState_Unpausing) return;
        s32 frames = gabi::ftoi(gabi::fmuls_ppc((f32)fadeFrames, gabi::fsubs_ppc(1.0f, pauseFadeVolume.value())));
        if (frames <= 0) frames = 1;
        pauseFadeVolume.setTarget(1.0f, frames);
        pauseState = PauseState_Unpausing;
        unpauseRequested = true;
    }
}

/* 028853F4 */
void BasicSound::setAutoStopCounter(s32 frames) {
    autoStopCounter = frames;
    autoStopEnabled = frames > 0;
}

/* 0288540C */
void BasicSound::initialize() {
    u32 id = sInstanceCounter.last;
    sInstanceCounter.last = id + 1;
    autoStopCounter = 0;
    ambientInfo.argCallback = nullptr;
    externalPlayer = nullptr;
    soundId = 0xFFFFFFFFu;
    pauseFadeVolume.frames = 0;
    started = false;
    playerHeap = 0;
    soundActor = nullptr;
    fadeVolume.counter = 0;
    autoStopEnabled = false;
    ambientInfo.argAllocator = nullptr;
    soundPlayer = nullptr;
    fadeVolume.frames = 0;
    stopAfterFade = false;
    pauseFadeVolume.target = 1.0f;
    playingCounter = 0;
    pauseState = PauseState_None;
    biquadType = -1;
    instanceId = id;
    lpfFrequency = 0.0f;
    biquadValue = 0.0f;
    outputOffset.pan = 0.0f;
    driverInitialized = false;
    updateCounter = 0;
    pauseFadeVolume.counter = 0;
    playerState = PlayerState_Init;
    pauseFadeVolume.origin = 1.0f;
    muteState = PauseState_None;
    generalHandle = nullptr;
    outputOffset.mainSend = 0.0f;
    volume.target = 1.0f;
    startRequested = false;
    volume.frames = 0;
    outputOffset.surroundPan = 0.0f;
    volume.counter = 0;
    muteFadeVolume.frames = 0;
    volume.origin = 1.0f;
    tempGeneralHandle = nullptr;
    outputOffset.mixMode = 0;
    ambientInfo.paramCallback = nullptr;
    ambientInfo.argSize = 0;
    muteFadeVolume.origin = 1.0f;
    unpauseRequested = false;
    fadeVolume.target = 1.0f;
    ambientInfo.arg = nullptr;
    muteFadeVolume.counter = 0;
    pitch = 1.0f;
    muteFadeVolume.target = 1.0f;
    initialVolume = 1.0f;
    fadeVolume.origin = 1.0f;
    outputLine = 1;
    for (int i = 0; i < 3; i++) outputOffset.fxSend[i] = 0.0f;
    for (int i = 0; i < 3; i++) resetOutputParam(&output[i]);
    for (int i = 0; i < 4; i++) {
        remote[i].volume = 1.0f;
        remote[i].filter = 0.0f;
        remote[i].bypass = 0.0f;
    }
    resetAmbientState(&ambient);
    resetActorParam(&actorParam);
    if (userDataSize) sndMemset(userData, 0, userDataSize);

    DriverSoundPlayer* driver = callGetDriverPlayer();
    DriverCommandManager* manager = DriverCommandManager::instance();
    PlayerInitializeCommand* command = (PlayerInitializeCommand*)manager->allocCommand(sizeof(PlayerInitializeCommand));
    command->initializedFlag = &driverInitialized;
    command->player = driver;
    command->id = DriverCommand_PlayerInitialize;
    manager->pushCommand(command);
    driver->event.reset();
    state = SoundState_Initialized;
}

/* 028856EC */
void BasicSound::setId(u32 id) { soundId = id; }

/* 028856F4 */
u32 BasicSound::isAttachedGeneralHandle() { return generalHandle.get() != nullptr; }

/* 02885704 */
void BasicSound::detachGeneralHandle() { generalHandle->detachSound(); }

/* 0288570C */
u32 BasicSound::isAttachedTempGeneralHandle() { return tempGeneralHandle.get() != nullptr; }

/* 0288571C */
void BasicSound::detachTempGeneralHandle() { tempGeneralHandle->detachSound(); }

/* 02885724 */
void BasicSound::finalize() {
    if (state != SoundState_Initialized) return;
    setId(0xFFFFFFFFu);
    if (isAttachedGeneralHandle()) detachGeneralHandle();
    if (isAttachedTempGeneralHandle()) detachTempGeneralHandle();
    if (callIsAttachedTempSpecialHandle()) callDetachTempSpecialHandle();
    if (SoundPlayer* player = soundPlayer) player->removeSound(this);
    if (ExternalSoundPlayer* external = externalPlayer) external->removeSound(this);
    if (AmbientAllocator* allocator = ambientInfo.argAllocator) {
        gabi::call_ptr<void>(allocator->vtable->free.function, allocator, ambientInfo.arg.get(), this);
        ambientInfo.arg = nullptr;
    }
    if (started) {
        DriverCommandManager* manager = DriverCommandManager::instance();
        PlayerFlagCommand* command = (PlayerFlagCommand*)manager->allocCommand(sizeof(PlayerFlagCommand));
        command->id = DriverCommand_PlayerStop;
        command->player = callGetDriverPlayer();
        command->flag = (u8)stopAfterFade;
        manager->pushCommand(command);
        started = false;
    }
    driverInitialized = false;
    playerState = PlayerState_Stopped;
    DriverCommandManager* manager = DriverCommandManager::instance();
    PlayerCommand* command = (PlayerCommand*)manager->allocCommand(sizeof(PlayerFlagCommand));
    command->id = DriverCommand_PlayerFinalize;
    command->player = callGetDriverPlayer();
    manager->pushCommand(command);
    if (u32 callback = finalizeCallback) {
        gabi::call_ptr<void>(callback);
        finalizeCallback = 0;
    }
    stopAfterFade = false;
    state = SoundState_Finalized;
}

/* 028858F4 */
void BasicSound::setPriority(s32 priority, s32 ambientPriority) {
    playerPriority = (s8)priority;
    ambient.priority = ambientPriority;
}

/* 02885900 */
void BasicSound::setInitialVolume(f32 value) { initialVolume = value >= 0.0f ? value : 0.0f; }

/* 02885914 */
void BasicSound::setVolume(f32 value, s32 frames) {
    volume.setTarget(value >= 0.0f ? value : 0.0f, frames);
}

/* 028859A0 */
void BasicSound::setPitch(f32 value) { pitch = value; }

/* 028859A8 */
void BasicSound::resetOutputLine() { outputLine = (u32)soundPlayer->defaultOutputLine; }

/* 028859B8 */
void BasicSound::setPanCurve(u8 value) {
    DriverCommandManager* manager = DriverCommandManager::instance();
    PlayerFlagCommand* command = allocPlayerCommand(manager, DriverCommand_SetPanCurve, this);
    command->flag = value;
    manager->pushCommand(command);
}

/* 02885A40 */
void BasicSound::setRemoteFilter(u8 value) {
    DriverCommandManager* manager = DriverCommandManager::instance();
    PlayerFlagCommand* command = allocPlayerCommand(manager, DriverCommand_SetRemoteFilter, this);
    command->secondFlag = value;
    manager->pushCommand(command);
}

/* 02885AC8 */
void BasicSound::setDrcBypass(u8 value) {
    DriverCommandManager* manager = DriverCommandManager::instance();
    PlayerFlagCommand* command = allocPlayerCommand(manager, DriverCommand_SetDrcBypass, this);
    command->flag = value;
    manager->pushCommand(command);
}

/* 02885B50 */
void BasicSound::setFrontBypass(u8 value) {
    DriverCommandManager* manager = DriverCommandManager::instance();
    PlayerFlagCommand* command = allocPlayerCommand(manager, DriverCommand_SetFrontBypass, this);
    command->flag = value;
    manager->pushCommand(command);
}

/* 02885BD8 */
void BasicSound::setPanMode(u8 value) {
    DriverCommandManager* manager = DriverCommandManager::instance();
    PlayerFlagCommand* command = allocPlayerCommand(manager, DriverCommand_SetPanMode, this);
    command->flag = value;
    manager->pushCommand(command);
}

/* 02885C60 */
void BasicSound::attachSoundPlayer(SoundPlayer* player) { soundPlayer = player; }

/* 02885C68 */
void BasicSound::detachSoundPlayer(SoundPlayer* player) { soundPlayer = nullptr; }

/* 02885C74 */
void BasicSound::attachSoundActor(SoundActor* actor) { soundActor = actor; }

/* 02885C7C */
void BasicSound::attachExternalSoundPlayer(ExternalSoundPlayer* player) { externalPlayer = player; }

/* 02885C84 */
void BasicSound::detachExternalSoundPlayer(ExternalSoundPlayer* player) { externalPlayer = nullptr; }

/* 02885C90 */
void BasicSound::setAmbientInfo(const AmbientInfo* info) {
    AmbientAllocator* allocator = info->argAllocator;
    void* arg = gabi::call_ptr<void*>(allocator->vtable->allocate.function, allocator, (u32)info->argSize);
    if (!arg) return;
    sndMemcpy(arg, info->arg, info->argSize);
    /* struct copy, then the copied argument replaces the caller's */
    ambientInfo.paramCallback = info->paramCallback;
    ambientInfo.argCallback = info->argCallback;
    ambientInfo.argAllocator = info->argAllocator;
    ambientInfo.arg = info->arg.get();
    u32 size = info->argSize;
    ambientInfo.arg = arg;
    ambientInfo.argSize = size;
}

/* 02885D28 */
s32 AmbientInfo::priorityOf(const AmbientInfo* info, u32 soundId) {
    AmbientParamCallback* callback = info->paramCallback;
    if (!callback) return 0;
    return gabi::tail_ptr<s32>(callback->vtable->priority.function, callback, info->arg.get(), soundId);
}

/* 02885D58 */
void BasicSound::updateMoveValue() {
    fadeVolume.update();
    muteFadeVolume.update();
    volume.update();
}

/* inlined into MixVolume arrays of OutputParam */
/* 02885DA4 */
MixVolume* MixVolume::create(MixVolume* volume) {
    if (!volume) {
        volume = (MixVolume*)sndAllocate(sizeof(MixVolume));
        if (!volume) return volume;
    }
    volume->frontRight = 1.0f;
    volume->lfe = 0.0f;
    volume->rearLeft = 0.0f;
    volume->center = 0.0f;
    volume->frontLeft = 1.0f;
    volume->rearRight = 0.0f;
    return volume;
}

/* 02885E00 */
OutputAmbientParam* OutputAmbientParam::create(OutputAmbientParam* param) {
    if (!param) {
        param = (OutputAmbientParam*)sndAllocate(sizeof(OutputAmbientParam));
        if (!param) return param;
    }
    param->volume = 1.0f;
    param->surroundPan = 0.0f;
    param->pan = 0.0f;
    for (int i = 0; i < 3; i++) param->fxSend[i] = 0.0f;
    return param;
}

/* 02885E68 */
OutputParam* OutputParam::create(OutputParam* param) {
    if (!param) {
        param = (OutputParam*)sndAllocate(sizeof(OutputParam));
        if (!param) return param;
    }
    __construct_array(param->mixVolume, 2, sizeof(MixVolume), (void* (*)(void*))MixVolume::create);
    return param;
}

/* 02885EC0 */
void BasicSound::onUpdatePlayerPriority() {}

/* 02885EC4 */
void BasicSound::onUpdateParam() {}

/* HD: compiler-generated and template code of this translation unit */

/* 02885EC8 */
DriverCommandManager* DriverCommandManager::create(DriverCommandManager* manager) {
    if (!manager) {
        manager = (DriverCommandManager*)sndAllocate(sizeof(DriverCommandManager));
        if (!manager) return manager;
    }
    construct(manager);
    return manager;
}

/* 02885F10 */
DriverCommandManager* DriverCommandManager::instance() {
    if (!sCommandManagerGuard.constructed) {
        sCommandManagerGuard.constructed = 1;
        create(&sCommandManager);
        __register_global_object(gabi::ea(&sCommandManagerRecord));
    }
    return &sCommandManager;
}

/* 02885F68 */
DriverCommandManager* DriverCommandManager::taskThreadInstance() {
    if (!sTaskThreadCommandManagerGuard.constructed) {
        sTaskThreadCommandManagerGuard.constructed = 1;
        create(&sTaskThreadCommandManager);
        __register_global_object(gabi::ea(&sTaskThreadCommandManagerRecord));
    }
    return &sTaskThreadCommandManager;
}

/* WWHD glue
data sBasicSoundVtable 10181B60
data sInstanceCounter 101FC8B8
data sCommandManagerGuard 104B71D0
data sTaskThreadCommandManagerGuard 104B71D4
data sCommandManager 104B6858
data sTaskThreadCommandManager 104B6D14
data sCommandManagerRecord 101FC8BC
data sTaskThreadCommandManagerRecord 101FC8C8
ext 0273AD10 void* sndAllocate(u32)
ext C000A858 void sndMemset(void*,u32,u32)
ext C000A848 void sndMemcpy(void*,const void*,u32)
ext 02886F58 nw::snd::DriverCommandManager* nw::snd::DriverCommandManager::construct(nw::snd::DriverCommandManager*)
ext 02887248 nw::snd::DriverCommand* nw::snd::DriverCommandManager::allocCommand(u32)
ext 028870F0 u32 nw::snd::DriverCommandManager::pushCommand(nw::snd::DriverCommand*)
ext 02897AF0 void nw::snd::SoundPlayer::updatePriority(nw::snd::BasicSound*)
ext 02897AAC void nw::snd::SoundPlayer::removeSound(nw::snd::BasicSound*)
ext 02897688 f32 nw::snd::SoundPlayer::remoteVolume(s32)
ext 02887720 void nw::snd::ExternalSoundPlayer::removeSound(nw::snd::BasicSound*)
ext 02896CD8 void nw::snd::SoundHandle::detachSound()
ext 028F6A28 void nw::ut::Event::reset()
*/

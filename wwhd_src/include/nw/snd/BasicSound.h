/* nw::snd BasicSound: the game-thread half of every playing sound (WWHD layout, 0x298 bytes).
 *
 * Reconstructed from WWHD's code. The game thread keeps the parameters here and sends them to the
 * sound driver as commands (DriverCommandManager); the driver-side player is reached
 * through the virtual getDriverPlayer(). Output lines: one TV and two DRC (gamepad) outputs, plus
 * four Wii remote speakers.
 * Names are our own. The five player-option setters (commands 4..8) are named after the order of
 * their command ids; what each option means is probably as named, not proven. */
#pragma once
#include "wwhd.h"
#include "nw/ut/Platform.h"

namespace nw { namespace snd {

/* One entry of a GHS vtable: this-adjustment and function address. */
struct VtableEntry { be<u32> adjust; be<u32> function; };

/* ---- the driver side (other units) ---- */

/* Driver-side player of one sound (returned by getDriverPlayer()). */
struct DriverSoundPlayer {
    /* 0x00 */ nw::ut::Event event;
    /* 0x24 */ be<u8> active;         // the player is running (WaveSoundPlayer 028A0D10/028A0D34)
    /* 0x25 */ be<u8> started;        // playback has started (028A17D0)
    /* 0x26 */ be<u8> paused;         // playback is paused (028A17E4)
    /* 0x27 */ be<u8> playFinished;   // set by the driver when the sound has ended
    /* 0x28 */ be<u8> stopRequested;  // set by the driver; cleared by the ClearStopRequest command
};
WWHD_OFFSET(DriverSoundPlayer, active, 0x24);
WWHD_OFFSET(DriverSoundPlayer, started, 0x25);
WWHD_OFFSET(DriverSoundPlayer, paused, 0x26);
WWHD_OFFSET(DriverSoundPlayer, playFinished, 0x27);
WWHD_OFFSET(DriverSoundPlayer, stopRequested, 0x28);

enum DriverCommandId {
    DriverCommand_PlayerInitialize = 3,
    DriverCommand_SetPanMode = 4,
    DriverCommand_SetPanCurve = 5,
    DriverCommand_SetRemoteFilter = 6,
    DriverCommand_SetFrontBypass = 7,
    DriverCommand_SetDrcBypass = 8,
    DriverCommand_PlayerFinalize = 9,
    DriverCommand_PlayerStart = 10,
    DriverCommand_PlayerStop = 11,
    DriverCommand_PlayerPause = 12,
    DriverCommand_PlayerParam = 13,
    DriverCommand_ClearStopRequest = 14,
};

struct DriverCommand {
    /* 0x00 */ gptr<DriverCommand> next;
    /* 0x04 */ be<u32> id;
    /* 0x08 */ be<u32> serial;    // serial of the list this command was submitted with
    /* 0x0C */ be<u32> arenaEnd;  // arena read position after this command is released
};
struct PlayerCommand : DriverCommand {
    /* 0x10 */ gptr<DriverSoundPlayer> player;
};
struct PlayerFlagCommand : PlayerCommand {
    /* 0x14 */ be<u8> flag;
    /* 0x15 */ be<u8> secondFlag;
    /* 0x16 */ u8 padding[2];
};
struct PlayerInitializeCommand : PlayerCommand {
    /* 0x14 */ gptr<be<u8>> initializedFlag;  // the driver sets it once the player is ready
};
WWHD_SIZE(DriverCommand, 0x10);
WWHD_SIZE(PlayerCommand, 0x14);
WWHD_SIZE(PlayerFlagCommand, 0x18);
WWHD_SIZE(PlayerInitializeCommand, 0x18);

/* Ring arena the command headers are allocated from (in words). */
struct CommandArena {
    gptr<DriverCommand> buffer;
    be<u32> capacityWords;
    be<u32> writeWord;
    be<u32> readWord;
    be<u8> full;
    u8 padding[3];
    static CommandArena* create(CommandArena* arena);
    void destroy(u32 flags);
    void initialize(DriverCommand* storage, u32 bytes);
    DriverCommand* allocate(u32 bytes);
    void release(DriverCommand* last);
};
WWHD_OFFSET(CommandArena, capacityWords, 4);
WWHD_OFFSET(CommandArena, writeWord, 8);
WWHD_OFFSET(CommandArena, readWord, 12);
WWHD_OFFSET(CommandArena, full, 16);
WWHD_SIZE(CommandArena, 0x14);

/* Game-to-driver command manager: commands are built in the arena, collected into a list and sent to the
 * driver as one message; the driver hands the list back through the retired queue. */
struct DriverCommandManager {
    be<u8> initialized;
    u8 padding[3];
    be<u32> dispatch;
    be<u32> idleCallback;
    nw::ut::MessageQueue pending;
    be<u32> pendingMessages[128];
    nw::ut::MessageQueue retired;
    be<u32> retiredMessages[132];
    gptr<DriverCommand> first;
    gptr<DriverCommand> last;
    be<u32> pendingCount;
    be<u32> nextSerial;
    be<u32> releasedSerial;
    CommandArena arena;
    static DriverCommandManager* construct(DriverCommandManager* manager);
    static DriverCommandManager* create(DriverCommandManager* manager);  // this TU's new-expression copy
    static DriverCommandManager* instance();
    static DriverCommandManager* taskThreadInstance();
    void finalize();
    void destroy(u32 flags);
    void initialize(DriverCommand* buffer, u32 size, u32 dispatchFunction);
    DriverCommand* allocCommand(u32 size);
    u32 pushCommand(DriverCommand* command);
    DriverCommand* reserve(u32 size);
    void releaseList(DriverCommand* command);
    void collect();
    void waitFor(u32 serial);
    u32 submit(u32 blocking);
    u32 executePending();
    void prepare(DriverCommand* buffer, u32 size);
    void destroyWrapper(u32 flags);
};
WWHD_OFFSET(DriverCommandManager, pending, 0xC);
WWHD_OFFSET(DriverCommandManager, retired, 0x248);
WWHD_OFFSET(DriverCommandManager, first, 0x494);
WWHD_OFFSET(DriverCommandManager, pendingCount, 0x49C);
WWHD_OFFSET(DriverCommandManager, nextSerial, 0x4A0);
WWHD_OFFSET(DriverCommandManager, arena, 0x4A8);
WWHD_SIZE(DriverCommandManager, 0x4BC);

/* ---- parameter blocks ---- */

/* Mix volumes of one output: six speaker channels. */
struct MixVolume {
    /* 0x00 */ be<f32> frontLeft, frontRight, rearLeft, rearRight, center, lfe;
    static MixVolume* create(MixVolume* volume);
};
WWHD_SIZE(MixVolume, 0x18);

/* Everything sent to the driver for one output line (TV, DRC 0, DRC 1). */
struct OutputParam {
    /* 0x00 */ be<f32> volume;
    /* 0x04 */ be<u32> mixMode;
    /* 0x08 */ MixVolume mixVolume[2];
    /* 0x38 */ be<f32> pan;
    /* 0x3C */ be<f32> surroundPan;
    /* 0x40 */ be<f32> mainSend;
    /* 0x44 */ be<f32> fxSend[3];
    static OutputParam* create(OutputParam* param);
};
WWHD_OFFSET(OutputParam, pan, 0x38);
WWHD_OFFSET(OutputParam, fxSend, 0x44);
WWHD_SIZE(OutputParam, 0x50);

/* Offsets added to every output line's parameters. */
struct OutputOffset {
    /* 0x00 */ be<u32> mixMode;
    /* 0x04 */ be<f32> pan;
    /* 0x08 */ be<f32> surroundPan;
    /* 0x0C */ be<f32> mainSend;
    /* 0x10 */ be<f32> fxSend[3];
};
WWHD_SIZE(OutputOffset, 0x1C);

/* Per-output part of the ambient parameters. */
struct OutputAmbientParam {
    /* 0x00 */ be<f32> volume;
    /* 0x04 */ be<f32> pan;
    /* 0x08 */ be<f32> surroundPan;
    /* 0x0C */ be<f32> fxSend[3];
    static OutputAmbientParam* create(OutputAmbientParam* param);
};
WWHD_SIZE(OutputAmbientParam, 0x18);

/* Ambient parameters as the update callback sees them (a stack object of BasicSound::update). */
struct AmbientParam {
    /* 0x00 */ be<f32> volume;
    /* 0x04 */ be<f32> pitch;
    /* 0x08 */ be<f32> lpfFrequency;
    /* 0x0C */ be<f32> biquadValue;
    /* 0x10 */ be<s32> biquadType;
    /* 0x14 */ be<s32> priority;
    /* 0x18 */ be<u32> userData;
    /* 0x1C */ be<s32> outputLine;  // -1: keep the sound's own output line
    /* 0x20 */ OutputAmbientParam tv;
    /* 0x38 */ OutputAmbientParam drc[2];
};
WWHD_OFFSET(AmbientParam, tv, 0x20);
WWHD_OFFSET(AmbientParam, drc, 0x38);
WWHD_SIZE(AmbientParam, 0x68);

/* The sound's stored copy: the same fields, with outputLine and userData in the other order. */
struct AmbientState {
    /* 0x00 */ be<f32> volume;
    /* 0x04 */ be<f32> pitch;
    /* 0x08 */ be<f32> lpfFrequency;
    /* 0x0C */ be<f32> biquadValue;
    /* 0x10 */ be<s32> biquadType;
    /* 0x14 */ be<s32> priority;
    /* 0x18 */ be<s32> outputLine;
    /* 0x1C */ be<u32> userData;
    /* 0x20 */ OutputAmbientParam tv;
    /* 0x38 */ OutputAmbientParam drc[2];
};
WWHD_OFFSET(AmbientState, tv, 0x20);
WWHD_OFFSET(AmbientState, drc, 0x38);
WWHD_SIZE(AmbientState, 0x68);

/* Callback objects of the ambient information (each has a GHS vtable). */
struct AmbientParamCallbackVtable {
    VtableEntry header, destroy, update, priority;
};
struct AmbientParamCallback { gptr<AmbientParamCallbackVtable> vtable; };
struct AmbientArgCallbackVtable {
    VtableEntry header, destroy, update;
};
struct AmbientArgCallback { gptr<AmbientArgCallbackVtable> vtable; };
struct AmbientAllocatorVtable {
    VtableEntry header, destroy, allocate, free;
};
struct AmbientAllocator { gptr<AmbientAllocatorVtable> vtable; };

struct AmbientInfo {
    /* 0x00 */ gptr<AmbientParamCallback> paramCallback;
    /* 0x04 */ gptr<AmbientArgCallback> argCallback;
    /* 0x08 */ gptr<AmbientAllocator> argAllocator;
    /* 0x0C */ gptr<void> arg;
    /* 0x10 */ be<u32> argSize;
    static s32 priorityOf(const AmbientInfo* info, u32 soundId);
};
WWHD_SIZE(AmbientInfo, 0x14);

/* Parameters a sound actor (a positioned emitter) supplies. */
struct ActorParam {
    /* 0x00 */ be<f32> volume;
    /* 0x04 */ be<f32> pitch;
    /* 0x08 */ be<f32> tvVolume;
    /* 0x0C */ be<f32> tvPan;
    /* 0x10 */ be<f32> drcVolume[2];
    /* 0x18 */ be<f32> drcPan[2];
};
WWHD_SIZE(ActorParam, 0x20);

/* ---- intrusive lists (the list code at 0286DA70 / 0286DA9C) ---- */
struct BasicSound;
struct SoundMemoryArena;
struct SoundListNode {
    gptr<SoundListNode> next, previous;
};
struct SoundListIterator {
    gptr<SoundListNode> node;
};
WWHD_SIZE(SoundListNode, 8);
WWHD_SIZE(SoundListIterator, 4);
/* A counted circular list as GHS lays it out: the count, then the root node. Elements embed a node. */
struct SoundPoolList {
    be<u32> count;
    SoundListNode root;
};
WWHD_OFFSET(SoundPoolList, root, 4);
WWHD_SIZE(SoundPoolList, 12);

/* A bounded list of sounds ordered by priority: a sound actor's external sound player
 * (BasicSound::attachExternalSoundPlayer); sounds link through BasicSound::actorLink. */
struct PrioritySoundQueue {
    be<s32> count;
    SoundListNode sentinel;
    be<s32> capacity;
    BasicSound* lowestPriority();
    u32 canAccept(s32 priority);
    u32 append(BasicSound* sound);
};
WWHD_OFFSET(PrioritySoundQueue, sentinel, 4);
WWHD_OFFSET(PrioritySoundQueue, capacity, 12);
WWHD_SIZE(PrioritySoundQueue, 16);

struct SoundActor {
    /* 0x00 */ u8 reserved[8];
    /* 0x08 */ PrioritySoundQueue queues[4];  // one per actor player id (ArchivePlayback::startSound 0289533C)
    /* 0x48 */ ActorParam param;
};
WWHD_OFFSET(SoundActor, queues, 8);
WWHD_OFFSET(SoundActor, param, 0x48);

/* One output of a sound player: TV (0) or DRC (1, 2). */
struct PlayerOutput { be<f32> volume, mainSend, fxSend[3]; };
WWHD_SIZE(PlayerOutput, 0x14);
enum PlayerHeapState { PlayerHeap_Free = 2 };

/* The sound player a sound belongs to: its sounds, their priority order, its limits, output settings
 * and the heaps it lends to deferred loads. Slots are 0xA0 bytes in ArchivePlayback::players;
 * layout from the constructor 02897134. */
struct SoundPlayer {
    /* 0x00 */ SoundPoolList sounds;          // linked through BasicSound::playerLink
    /* 0x0C */ SoundPoolList prioritySounds;  // ascending priority, through BasicSound::priorityLink
    /* 0x18 */ SoundPoolList freeHeaps;
    /* 0x24 */ SoundPoolList busyHeaps;
    /* 0x30 */ be<s32> soundLimit;
    /* 0x34 */ be<s32> maximumSounds;
    /* 0x38 */ be<u32> heapCount;
    /* 0x3C */ be<f32> volume;
    /* 0x40 */ be<f32> lpfFrequency;
    /* 0x44 */ be<s32> biquadType;
    /* 0x48 */ be<f32> biquadValue;
    /* 0x4C */ be<u32> defaultOutputLine;
    /* 0x50 */ PlayerOutput outputs[3];       // TV, DRC; BasicSound reads outputs[1] for both DRC lines
    /* 0x8C */ be<f32> remoteVolumes[4];
    /* 0x9C */ be<u8> rejectEqualPriority;  // a new sound must beat (not tie) the lowest one to evict it
    /* 0x9D */ u8 padding9D[3];

    static SoundPlayer* create(SoundPlayer* player);
    void stopAll(s32 fadeFrames);
    void collectFreeHeaps();
    void sortByPriority(u32 newestFirst);
    void update();
    void pauseAll(u32 paused, s32 fadeFrames);
    void setVolume(f32 value);
    void setDefaultOutputLine(u32 line);
    void setOutputVolume(u32 output, f32 value);
    f32 remoteVolume(s32 remote);
    void setSoundLimit(s32 limit);
    void setMaximumSounds(s32 count);
    u32 canStart(s32 priority);
    u32 insertSound(BasicSound* sound);
    void removePriorityLink(BasicSound* sound);
    void removeSoundLink(BasicSound* sound);
    void removeSound(BasicSound* sound);
    void insertByPriority(BasicSound* sound);
    void updatePriority(BasicSound* sound);
    void addHeap(SoundMemoryArena* heap);
    SoundMemoryArena* takeHeap();
    void returnHeap(SoundMemoryArena* heap);
};
WWHD_OFFSET(SoundPlayer, prioritySounds, 0xC);
WWHD_OFFSET(SoundPlayer, soundLimit, 0x30);
WWHD_OFFSET(SoundPlayer, volume, 0x3C);
WWHD_OFFSET(SoundPlayer, defaultOutputLine, 0x4C);
WWHD_OFFSET(SoundPlayer, outputs, 0x50);
WWHD_OFFSET(SoundPlayer, remoteVolumes, 0x8C);
WWHD_OFFSET(SoundPlayer, rejectEqualPriority, 0x9C);
WWHD_SIZE(SoundPlayer, 0xA0);

struct ExternalSoundPlayer {
    void removeSound(BasicSound* sound);
};
/* The handle a caller keeps for a started sound. */
struct SoundHandle {
    gptr<BasicSound> sound;
    void detachSound();
    void attachSound(BasicSound* value);
    void attachTempSound(BasicSound* value);
};
WWHD_SIZE(SoundHandle, 4);

/* A value moving linearly to a target over a number of updates. */
struct MoveValue {
    /* 0x00 */ be<f32> origin;
    /* 0x04 */ be<f32> target;
    /* 0x08 */ be<s32> frames;
    /* 0x0C */ be<s32> counter;

    f32 value() const {
        if (counter < frames) {
            f32 from = origin;
            f32 step = gabi::fmuls_ppc(gabi::fsubs_ppc(target, from), (f32)(s32)counter);
            return gabi::fadds_ppc(from, gabi::fdivs_ppc(step, (f32)(s32)frames));
        }
        return target;
    }
    bool isFinished() const { return !(counter < frames); }
    void setTarget(f32 goal, s32 frameCount) {
        f32 current = value();
        origin = current;
        target = goal;
        frames = frameCount;
        counter = 0;
    }
    void update() {
        if (counter < frames) counter = counter + 1;
    }
};
WWHD_SIZE(MoveValue, 0x10);

/* Remote speaker output (one per Wii remote). */
struct RemoteOutputParam {
    /* 0x00 */ be<f32> volume;
    /* 0x04 */ be<f32> filter;
    /* 0x08 */ be<f32> bypass;
};
WWHD_SIZE(RemoteOutputParam, 0xC);

struct PlayerParamCommand : PlayerCommand {
    /* 0x014 */ be<f32> volume;
    /* 0x018 */ be<f32> pitch;
    /* 0x01C */ be<f32> lpfFrequency;
    /* 0x020 */ be<s32> biquadType;
    /* 0x024 */ be<f32> biquadValue;
    /* 0x028 */ be<u32> outputLine;
    /* 0x02C */ OutputParam output[3];
    /* 0x11C */ RemoteOutputParam remote[4];
};
WWHD_OFFSET(PlayerParamCommand, output, 0x2C);
WWHD_OFFSET(PlayerParamCommand, remote, 0x11C);
WWHD_SIZE(PlayerParamCommand, 0x14C);

struct BasicSoundVtable {
    VtableEntry header;
    VtableEntry destroy;
    VtableEntry destroyDeleting;
    VtableEntry initialize;
    VtableEntry finalize;
    VtableEntry isPrepared;
    VtableEntry isAttachedTempSpecialHandle;
    VtableEntry detachTempSpecialHandle;
    VtableEntry getDriverPlayer;
    VtableEntry onUpdatePlayerPriority;
    VtableEntry updateMoveValue;
    VtableEntry onUpdateParam;
};
WWHD_OFFSET(BasicSoundVtable, getDriverPlayer, 0x40);
WWHD_SIZE(BasicSoundVtable, 0x60);

enum PauseState {
    PauseState_None = 0,
    PauseState_Pausing = 1,
    PauseState_Paused = 2,
    PauseState_Unpausing = 3,
};
enum PlayerState {
    PlayerState_Init = 0,
    PlayerState_Playing = 1,
    PlayerState_Stopped = 2,
};
enum SoundState {
    SoundState_Constructed = 0,
    SoundState_Initialized = 1,
    SoundState_Finalized = 2,
};

struct BasicSound {
    /* 0x000 */ be<u32> playerHeap;
    /* 0x004 */ gptr<SoundHandle> generalHandle;
    /* 0x008 */ gptr<SoundHandle> tempGeneralHandle;
    /* 0x00C */ gptr<SoundPlayer> soundPlayer;
    /* 0x010 */ gptr<SoundActor> soundActor;
    /* 0x014 */ gptr<ExternalSoundPlayer> externalPlayer;
    /* 0x018 */ AmbientInfo ambientInfo;
    /* 0x02C */ AmbientState ambient;
    /* 0x094 */ ActorParam actorParam;
    /* 0x0B4 */ MoveValue fadeVolume;
    /* 0x0C4 */ MoveValue pauseFadeVolume;
    /* 0x0D4 */ MoveValue muteFadeVolume;
    /* 0x0E4 */ be<u8> startRequested;
    /* 0x0E5 */ be<u8> started;
    /* 0x0E6 */ be<u8> autoStopEnabled;
    /* 0x0E7 */ be<u8> stopAfterFade;
    /* 0x0E8 */ be<u8> driverInitialized;
    /* 0x0E9 */ be<u8> unpauseRequested;
    /* 0x0EA */ be<s8> playerPriority;
    /* 0x0EB */ be<s8> biquadType;
    /* 0x0EC */ be<u32> state;
    /* 0x0F0 */ be<u8> playerState;
    /* 0x0F1 */ be<u8> pauseState;
    /* 0x0F2 */ be<u8> muteState;
    /* 0x0F3 */ u8 padding;
    /* 0x0F4 */ be<s32> autoStopCounter;
    /* 0x0F8 */ be<u32> updateCounter;
    /* 0x0FC */ be<u32> playingCounter;
    /* 0x100 */ be<u32> soundId;
    /* 0x104 */ be<u32> instanceId;
    /* 0x108 */ be<f32> initialVolume;
    /* 0x10C */ be<f32> pitch;
    /* 0x110 */ be<f32> lpfFrequency;
    /* 0x114 */ be<f32> biquadValue;
    /* 0x118 */ be<u32> outputLine;
    /* 0x11C */ MoveValue volume;
    /* 0x12C */ OutputOffset outputOffset;
    /* 0x148 */ OutputParam output[3];
    /* 0x238 */ RemoteOutputParam remote[4];
    /* 0x268 */ gptr<void> userData;
    /* 0x26C */ be<u32> userDataSize;
    /* 0x270 */ be<u32> finalizeCallback;  // called without arguments when the sound is finalized
    /* 0x274 */ SoundListNode poolLink;      // the archive player's sound pool (allocation 028961D0)
    /* 0x27C */ SoundListNode playerLink;    // SoundPlayer::sounds (removeSoundLink 02897A64)
    /* 0x284 */ SoundListNode priorityLink;  // SoundPlayer::prioritySounds (removePriorityLink 02897A58)
    /* 0x28C */ SoundListNode actorLink;     // the actor's PrioritySoundQueue (append 028875F8)
    /* 0x294 */ gptr<BasicSoundVtable> vtable;

    static BasicSound* create(BasicSound* sound);
    void sendClearStopRequest();
    void setPlayerPriority(s32 priority);
    void stop(s32 fadeFrames);
    void addOutputOffset(OutputParam* param);
    void updateParam();
    void update();
    void startPrepared();
    void pause(u32 paused, s32 fadeFrames);
    void setAutoStopCounter(s32 frames);
    void initialize();
    void setId(u32 id);
    u32 isAttachedGeneralHandle();
    void detachGeneralHandle();
    u32 isAttachedTempGeneralHandle();
    void detachTempGeneralHandle();
    void finalize();
    void setPriority(s32 priority, s32 ambientPriority);
    void setInitialVolume(f32 volume);
    void setVolume(f32 volume, s32 frames);
    void setPitch(f32 pitch);
    void resetOutputLine();
    void setPanCurve(u8 value);
    void setRemoteFilter(u8 value);
    void setDrcBypass(u8 value);
    void setFrontBypass(u8 value);
    void setPanMode(u8 value);
    void attachSoundPlayer(SoundPlayer* player);
    void detachSoundPlayer(SoundPlayer* player);
    void attachSoundActor(SoundActor* actor);
    void attachExternalSoundPlayer(ExternalSoundPlayer* player);
    void detachExternalSoundPlayer(ExternalSoundPlayer* player);
    void setAmbientInfo(const AmbientInfo* info);
    void updateMoveValue();
    void onUpdatePlayerPriority();
    void onUpdateParam();

    /* virtual calls through the GHS vtable */
    void callFinalize() { gabi::call_ptr<void>(vtable->finalize.function, this); }
    u32 callIsPrepared() { return gabi::call_ptr<u32>(vtable->isPrepared.function, this); }
    u32 callIsAttachedTempSpecialHandle() { return gabi::call_ptr<u32>(vtable->isAttachedTempSpecialHandle.function, this); }
    void callDetachTempSpecialHandle() { gabi::call_ptr<void>(vtable->detachTempSpecialHandle.function, this); }
    DriverSoundPlayer* callGetDriverPlayer() { return gabi::call_ptr<DriverSoundPlayer*>(vtable->getDriverPlayer.function, this); }
    void callOnUpdatePlayerPriority() { gabi::call_ptr<void>(vtable->onUpdatePlayerPriority.function, this); }
    void callUpdateMoveValue() { gabi::call_ptr<void>(vtable->updateMoveValue.function, this); }
    void callOnUpdateParam() { gabi::call_ptr<void>(vtable->onUpdateParam.function, this); }
};
WWHD_OFFSET(BasicSound, ambientInfo, 0x18);
WWHD_OFFSET(BasicSound, ambient, 0x2C);
WWHD_OFFSET(BasicSound, actorParam, 0x94);
WWHD_OFFSET(BasicSound, fadeVolume, 0xB4);
WWHD_OFFSET(BasicSound, startRequested, 0xE4);
WWHD_OFFSET(BasicSound, state, 0xEC);
WWHD_OFFSET(BasicSound, autoStopCounter, 0xF4);
WWHD_OFFSET(BasicSound, initialVolume, 0x108);
WWHD_OFFSET(BasicSound, volume, 0x11C);
WWHD_OFFSET(BasicSound, outputOffset, 0x12C);
WWHD_OFFSET(BasicSound, output, 0x148);
WWHD_OFFSET(BasicSound, remote, 0x238);
WWHD_OFFSET(BasicSound, userData, 0x268);
WWHD_OFFSET(BasicSound, finalizeCallback, 0x270);
WWHD_OFFSET(BasicSound, poolLink, 0x274);
WWHD_OFFSET(BasicSound, playerLink, 0x27C);
WWHD_OFFSET(BasicSound, priorityLink, 0x284);
WWHD_OFFSET(BasicSound, actorLink, 0x28C);
WWHD_OFFSET(BasicSound, vtable, 0x294);
WWHD_SIZE(BasicSound, 0x298);

} }

void* sndAllocate(u32 size);
void sndMemset(void* destination, u32 value, u32 size);
void sndMemcpy(void* destination, const void* source, u32 size);

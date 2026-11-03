/* Based on CC0 tww JAISeMgr.cpp; compiler constructor thunks are kept with their array users. */
#include "JSystem/JAudio/JAISeMgr.h"
using namespace JAInter;
using namespace JAInter::SeMgr;
static JAISound* audioSeSequenceHandle;
static BasicSeTrackUpdate* seTrackUpdate;
static LinkSound* seRegist;
using SeCategoryRow = gptr<be<u8>>;
using SeSoundPointer = gptr<JAISound>;
using SeSoundRow = gptr<SeSoundPointer>;
static SeCategoryRow* categoryInfoTable;
static SeSoundRow* sePlaySound;
static u8 seScene;
using SeCategoryVolume = be<f32>;
static SeCategoryVolume* seCategoryVolume;
static JAIBasic* msBasic;
static u32 seqMuteFlagFromSe;
static SeParameter* seParameterFreeStartPointer;
static SeParameter* seParameterUsedEndPointer;
static AudioHeaderStatics seHeaderStatics;
static AudioDestructorRecord seDestructorRecord;
static SeSoundPointer* seRegistBuffer;
using SeCancelByte = be<u8>;
static SeCancelByte* seEntryCancel;
static JKRHeap* seCurrentHeap;
static u32 seArrayCookie;
struct SeDefaultCategoryInfo { be<u8> values[32]; };
static SeDefaultCategoryInfo seDefaultCategoryInfo;
struct SeCandidates { gptr<JAISound> entries[16]; };
struct SeBasicVtable { be<u32> reserved[5]; be<u32> makeSound; };
static JKRHeap* requireSeHeap() {
    JKRHeap* heap = seCurrentHeap;
    if (!heap) {
        seAssert("JAIBasic.h", 169, "msCurrentHeap");
        heap = seCurrentHeap;
    }
    return heap;
}
struct SeRankEntry { be<u8> state; u8 reserved[3]; be<u32> priority; gptr<JAISound> sound; };
struct SeRankBuffer { SeRankEntry entries[16]; };
static cXyz seDefaultPosition;
static u32 seRandomSeed;
struct SePlayingPorts { be<u16> activity, status; };
struct SeBasicPlayingVtable { be<u32> reserved[7]; be<u32> fxline; be<u32> reserved20; be<u32> ground; be<u32> reserved28[3]; be<u32> extend; };
static u32 seFloatToUnsigned(f32 value) {
    return value < 2147483648.0f ? (u32)gabi::ftoi(value) : (u32)gabi::ftoi(value - 2147483648.0f) + 0x80000000u;
}
static JAInter::Actor seNullActor;
// HD: GHS emitted one default-constructor thunk for each of these four array members.
/* 02804D8C */
static void initializeSeTrackParameter(MoveParaSet* parameter) {
    parameter->mMoveCounter = 0;
    parameter->mCurrentValue = 1.0f;
    parameter->mTargetValue = 1.0f;
}
/* 02804DA8 */
static void initializeSeVolumeParameter(MoveParaSet* parameter) {
    parameter->mMoveCounter = 0;
    parameter->mCurrentValue = 1.0f;
    parameter->mTargetValue = 1.0f;
}
/* 02804DC4 */
static void initializeSePitchParameter(MoveParaSet* parameter) {
    parameter->mMoveCounter = 0;
    parameter->mCurrentValue = 1.0f;
    parameter->mTargetValue = 1.0f;
}
/* 02804DE0 */
static void initializeSeDolbyParameter(MoveParaSet* parameter) {
    parameter->mMoveCounter = 0;
    parameter->mCurrentValue = 1.0f;
    parameter->mTargetValue = 1.0f;
}
namespace JAInter {
/* 02804DFC */
SeParameter::SeParameter() {
    constructSeMoveArray(mTrackParameters, 16, 16, initializeSeTrackParameter);
    constructSeMoveArray(mVolumes, 8, 16, initializeSeVolumeParameter);
    constructSeMoveArray(mPans, 8, 16, initializeSeHalfParameter);
    constructSeMoveArray(mPitches, 8, 16, initializeSePitchParameter);
    constructSeMoveArray(mFxmixes, 8, 16, initializeSeZeroParameter);
    constructSeMoveArray(mAuxMixes, 8, 16, initializeSeZeroParameter);
    constructSeMoveArray(mDolbys, 8, 16, initializeSeDolbyParameter);
    f32 dolby = (f32)getSeDolbyCenter() / 127.0f;
    for (u32 i = 0; i < 8; ++i) {
        mVolumes[i].mCurrentValue = 1.0f;
        mVolumes[i].mTargetValue = 1.0f;
        mVolumes[i].mMoveCounter = 0;
        mPans[i].mTargetValue = 0.5f;
        mPans[i].mCurrentValue = 0.5f;
        mPans[i].mMoveCounter = 0;
        mPitches[i].mTargetValue = 1.0f;
        mPitches[i].mMoveCounter = 0;
        mPitches[i].mCurrentValue = 1.0f;
        mFxmixes[i].mCurrentValue = 0.0f;
        mFxmixes[i].mTargetValue = 0.0f;
        mFxmixes[i].mMoveCounter = 0;
        mAuxMixes[i].mCurrentValue = 0.0f;
        mAuxMixes[i].mTargetValue = 0.0f;
        mAuxMixes[i].mMoveCounter = 0;
        mDolbys[i].mCurrentValue = dolby;
        mDolbys[i].mMoveCounter = 0;
        mDolbys[i].mTargetValue = dolby;
    }
    mVolumes[7].mMoveCounter = 0;
    mVolumes[7].mTargetValue = -1.0f;
    mVolumes[7].mCurrentValue = -1.0f;
    mPans[7].mCurrentValue = -1.0f;
    mPans[7].mTargetValue = -1.0f;
    mPans[7].mMoveCounter = 0;
    mPitches[7].mCurrentValue = -1.0f;
    mPitches[7].mTargetValue = -1.0f;
    mPitches[7].mMoveCounter = 0;
    mFxmixes[7].mCurrentValue = -1.0f;
    mFxmixes[7].mTargetValue = -1.0f;
    mFxmixes[7].mMoveCounter = 0;
    mAuxMixes[7].mMoveCounter = 0;
    mAuxMixes[7].mCurrentValue = -1.0f;
    mAuxMixes[7].mTargetValue = -1.0f;
    mDolbys[7].mCurrentValue = -1.0f;
    mDolbys[7].mTargetValue = -1.0f;
    mDolbys[7].mMoveCounter = 0;
    dolbySource = nullptr;
    mPortFlags = 0;
    pitchSource = nullptr;
    panSource = nullptr;
    updateFlags = 0;
    fxmixSource = nullptr;
    volumeSource = nullptr;
    previous = nullptr;
    next = nullptr;
    for (u32 i = 0; i < 16; ++i) mPortValues[i] = 0;
}
namespace SeMgr {
/* 02805028 */
void init() {
    if (msBasic->mSceneTable) {
        setSeTrackMax(0);
        for (u32 scene = 0; scene < getSeSceneMax(); ++scene) {
            u32 sum = 0;
            for (u32 category = 0; category < basicCategoryMax(); ++category) {
                SeCategoryRow* rows = (SeCategoryRow*)(void*)msBasic->mSceneTable;
                sum += rows[scene][category * 2];
            }
            if (getSeTrackMax() < sum) setSeTrackMax(sum);
        }
    }
    JKRHeap* heap = requireSeHeap();
    u32 count = basicCategoryMax();
    void* storage = allocateBasicSoundArray(count * 0x48 + seArrayCookie, heap, 32);
    seRegist = storage ? (LinkSound*)constructSeArray((u8*)storage + seArrayCookie, count, 0x48, initializeSeLink, 0) : nullptr;
    if (!seRegist) seAssert("JAISeMgr.cpp", 63, "seRegist");
    heap = requireSeHeap();
    count = basicCategoryMax();
    sePlaySound = (SeSoundRow*)allocateBasicSoundArray(count * 4, heap, 32);
    if (!sePlaySound) seAssert("JAISeMgr.cpp", 65, "sePlaySound");
    heap = requireSeHeap();
    count = basicCategoryMax();
    seRegistBuffer = (SeSoundPointer*)allocateBasicSoundArray(count * 4, heap, 32);
    if (!seRegistBuffer) seAssert("JAISeMgr.cpp", 67, "seRegistBuffer");
    for (u32 category = 0; category < basicCategoryMax(); ++category) {
        JAIBasic* basic = msBasic;
        SeBasicVtable* table = (SeBasicVtable*)(BasicVtableRecord*)basic->mVtable;
        SeSoundPointer* rows = seRegistBuffer;
        count = getSeRegistMax();
        rows[category] = gabi::call_ptr<JAISound*>(table->makeSound, basic, count);
        JAISound* sounds = seRegistBuffer[category];
        if (!sounds) {
            seAssert("JAISeMgr.cpp", 72, "seRegistBuffer[i]");
            sounds = seRegistBuffer[category];
        }
        count = getSeRegistMax();
        initSeLink(seRegist + category, sounds, count);
        heap = requireSeHeap();
        SeSoundRow* slots = sePlaySound;
        slots[category] = (SeSoundPointer*)allocateBasicSoundArray(64, heap, 32);
        if (!sePlaySound[category]) seAssert("JAISeMgr.cpp", 78, "sePlaySound[i]");
        for (u32 slot = 0; slot < 16; ++slot) sePlaySound[category][slot] = nullptr;
    }
    heap = requireSeHeap();
    count = getSeTrackMax();
    seTrackUpdate = (BasicSeTrackUpdate*)allocateBasicSoundArray(count * 24, heap, 32);
    if (!seTrackUpdate) seAssert("JAISeMgr.cpp", 86, "seTrackUpdate");
    for (u32 index = 0; index < getSeTrackMax(); ++index) {
        BasicSeTrackUpdate* track = seTrackUpdate + index;
        track->pitch = 1.0f;
        track->flags = 255;
        track->pan = 0.5f;
        track->fxmix = 0.0f;
        track->dolby = 0.0f;
        track->volume = 1.0f;
    }
    heap = requireSeHeap();
    count = basicCategoryMax();
    count *= getSeRegistMax();
    storage = allocateBasicSoundArray(count * 0x444 + seArrayCookie, heap, 32);
    SeParameter* parameters = storage ? (SeParameter*)constructSeArray((u8*)storage + seArrayCookie, count, 0x444, initializeSeParameters, 0) : nullptr;
    if (!parameters) seAssert("JAISeMgr.cpp", 99, "parameterObject");
    seParameterFreeStartPointer = parameters;
    seParameterUsedEndPointer = nullptr;
    parameters[0].previous = nullptr;
    parameters[0].next = parameters + 1;
    u32 index = 1;
    count = basicCategoryMax();
    count *= getSeRegistMax();
    parameters[index].previous = parameters;
    while (index < count - 1) {
        parameters[index].next = parameters + index + 1;
        ++index;
        count = basicCategoryMax();
        count *= getSeRegistMax();
        parameters[index].previous = parameters + index - 1;
    }
    parameters[index].next = nullptr;
    SeCategoryRow* sceneTable = (SeCategoryRow*)(void*)msBasic->mSceneTable;
    if (sceneTable) categoryInfoTable = sceneTable;
    else {
        heap = requireSeHeap();
        count = getSeSceneMax();
        categoryInfoTable = (SeCategoryRow*)allocateBasicSoundArray(count * 4, heap, 32);
        if (!categoryInfoTable) seAssert("JAISeMgr.cpp", 124, "categoryInfoTable");
        for (u32 scene = 0; scene < getSeSceneMax(); ++scene) categoryInfoTable[scene] = seDefaultCategoryInfo.values;
    }
    heap = requireSeHeap();
    count = basicCategoryMax();
    seEntryCancel = (be<u8>*)allocateBasicSoundArray(count, heap, 32);
    if (!seEntryCancel) seAssert("JAISeMgr.cpp", 132, "seEntryCancel");
    heap = requireSeHeap();
    count = basicCategoryMax();
    seCategoryVolume = (SeCategoryVolume*)allocateBasicSoundArray(count * 4, heap, 32);
    if (!seCategoryVolume) seAssert("JAISeMgr.cpp", 134, "seCategoryVolume");
    for (u32 category = 0; category < basicCategoryMax(); ++category) {
        seEntryCancel[category] = 0;
        seCategoryVolume[category] = 1.0f;
    }
}
/* 028057D4 */
void startSeSequence() {
    JAIBasic* basic = msBasic;
    audioSeSequenceHandle = nullptr;
    basic->startSoundActor(0x80000800u, (gptr<JAISound>*)&audioSeSequenceHandle, nullptr, 1, 4);
    if (!audioSeSequenceHandle) seAssert("JAISeMgr.cpp", 149, "JAISeMgr::seHandle");
}
/* 02805848 */
void clearSeqMuteFromSeStop(JAISound* sound) {
    if (!seqMuteFlagFromSe || !(getSeSwitchBits(sound) & 8)) return;
    for (u32 index = 0; index < getSeSequenceTrackMax(); ++index) {
        SeSeqUpdateView* update = getSePlayTrackInfo(index);
        u32 seTrack = audioSeSequenceHandle->track;
        JAISound* sequence = update->sequence;
        if (index == seTrack || !sequence || (getSeSwitchBits(sequence) & 8)) continue;
        u32 shift = sound->track & 63;
        u32 bit = shift < 32 ? 1u << shift : 0;
        u32 flags = seqMuteFlagFromSe & ~bit;
        seqMuteFlagFromSe = flags;
        if (!flags) {
            u32 speed = getSeMuteMoveSpeed();
            setSeSequenceVolume(sequence, 9, speed, 1.0f);
        }
    }
}
/* 028059B0 */
void releaseSeRegist(JAISound* sound) {
    if (sound->extra) return;
    if (sound->state != 1) {
        BasicSeqParameterView* sequence = getBasicSeqParameter((JAISound*)(JAISound*)audioSeSequenceHandle);
        u32 track = sound->track;
        u32 route = 0x20000000u + (track >> 4) + ((track << 4) & 0xF0);
        writeBasicTrackPort((BasicTrackView*)sequence->rootTrack, route, 0);
        track = sound->track;
        setSeTrackInterrupt(audioSeSequenceHandle, track, 1);
    }
    clearSeqMuteFromSeStop(sound);
    u32 category = getSeCategory(sound);
    u32 scene = seScene;
    be<u8>* info = categoryInfoTable[scene];
    u8 max = info[category * 2];
    category = getSeCategory(sound);
    for (u8 index = 0; index < max; ++index) {
        SeSoundPointer* row = sePlaySound[category];
        if (row[index] == sound) { u32 slot = index; index = max; row[slot] = nullptr; }
    }
    basicClearSoundOwner((JAISound*)sound);
    sound->state = 0;
    sound->track = 255;
    SeParameter* parameters = getSeSoundParameter(sound);
    releaseSeParameterPointer(parameters);
    releaseSeLinkSound(seRegist + category, sound);
}
/* 02805AFC */
void releaseSeBuffer(JAISound* sound, u32 fade) {
    u32 state = sound->state;
    if (!state) return;
    if (!fade || state == 1) { releaseSeSoundRegistration(sound); return; }
    sound->fadeCounter = fade;
    setSeSoundInterVolume(sound, 6, fade, 0, 0.0f);
}
/* 028064DC */
void setSeqMuteFromSeStart(JAISound* sound) {
    for (u32 index = 0; index < getSeSequenceTrackMax(); ++index) {
        SeSeqUpdateView* update = getSePlayTrackInfo(index);
        u32 seTrack = audioSeSequenceHandle->track;
        JAISound* sequence = update->sequence;
        if (index == seTrack || !sequence || (getSeSwitchBits(sequence) & 8)) continue;
        u32 volume = getSeMuteVolume();
        f32 scale = (f32)volume / 127.0f;
        u32 speed = getSeMuteMoveSpeed();
        setSeSequenceVolume(sequence, 9, speed, scale);
        u32 shift = sound->track & 63;
        u32 bit = shift < 32 ? 1u << shift : 0;
        seqMuteFlagFromSe = seqMuteFlagFromSe | bit;
    }
}
/* 028065D8 */
void checkPlayingSeUpdateMultiplication(JAISound* sound, SeSeqUpdateView* sequence, be<f32>* source, MoveParaSet* parameters, f32 multiplier, u32 parameter, be<f32>* output) {
    f32 value = parameters[7].mCurrentValue;
    if (value == -1.0f) {
        if (source) parameters[0].mCurrentValue = *source;
        value = 1.0f;
        for (u32 i = 0; i < 7; ++i) value = value * parameters[i].mCurrentValue;
    }
    value = value * multiplier;
    if (*output == value) return;
    *output = value;
    if (sound->state == 2) return;
    u32 track = sound->track;
    be<u32>* updates = sequence->trackupdate;
    u32 flags = updates[track];
    u32 shift = (parameter - 2) & 63;
    u32 bit = shift < 32 ? 1u << shift : 0;
    updates[track] = flags | bit;
    SeSeqUpdateView* target = getSePlayTrackInfo(audioSeSequenceHandle->track);
    track = sound->track;
    setSeSequencePortFloat(target, track, parameter, value);
}
/* 028066E4 */
void checkPlayingSeUpdateAddition(JAISound* sound, SeSeqUpdateView* sequence, be<f32>* source, MoveParaSet* parameters, u32 parameter, be<f32>* output, f32 center) {
    f32 value = parameters[7].mCurrentValue;
    if (value == -1.0f) {
        if (source) parameters[0].mCurrentValue = *source;
        value = 0.0f;
        for (u32 i = 0; i < 7; ++i) value = value + ((f32)parameters[i].mCurrentValue - center);
        value = value + center;
        if (value < 0.0f) value = 0.0f;
        else if (value > 1.0f) value = 1.0f;
    }
    if (*output == value) return;
    *output = value;
    if (sound->state == 2) return;
    u32 track = sound->track;
    be<u32>* updates = sequence->trackupdate;
    u32 flags = updates[track];
    u32 shift = (parameter - 2) & 63;
    u32 bit = shift < 32 ? 1u << shift : 0;
    updates[track] = flags | bit;
    SeSeqUpdateView* target = getSePlayTrackInfo(audioSeSequenceHandle->track);
    track = sound->track;
    setSeSequencePortFloat(target, track, parameter, value);
}
/* 0280593C */
void releaseSeParameterPointer(SeParameter* node) {
    if (!node) return;
    SeParameter* used = seParameterUsedEndPointer;
    SeParameter* next = node->next;
    if (used != node) {
        node->previous->next = next;
        next = node->next;
        if (next) next->previous = node->previous;
    } else {
        seParameterUsedEndPointer = next;
        next = node->next;
        if (next) next->previous = nullptr;
    }
    SeParameter* free = seParameterFreeStartPointer;
    node->next = free;
    if (free) free->previous = node;
    seParameterFreeStartPointer = node;
}
/* 02806824 */
void sendSeAllParameter(JAISound* sound) {
    BasicSeTrackUpdate* track = seTrackUpdate + (u8)sound->track;
    SeParameter* parameters = getSeSoundParameter(sound);
    SeSeqUpdateView* sequence = getSePlayTrackInfo(audioSeSequenceHandle->track);
    u32 category = getSeCategory(sound);
    be<f32>* source = parameters->volumeSource;
    f32 multiplier = seCategoryVolume[category];
    checkPlayingSeUpdateMultiplication(sound, sequence, source, parameters->mVolumes, multiplier, 2, &track->volume);
    source = parameters->panSource;
    checkPlayingSeUpdateAddition(sound, sequence, source, parameters->mPans, 4, &track->pan, 0.5f);
    source = parameters->pitchSource;
    checkPlayingSeUpdateMultiplication(sound, sequence, source, parameters->mPitches, 1.0f, 3, &track->pitch);
    source = parameters->fxmixSource;
    checkPlayingSeUpdateAddition(sound, sequence, source, parameters->mFxmixes, 5, &track->fxmix, 0.0f);
    f32 center = (f32)getSeDolbyCenter() / 127.0f;
    source = parameters->dolbySource;
    checkPlayingSeUpdateAddition(sound, sequence, source, parameters->mDolbys, 6, &track->dolby, center);
    u32 channel = sound->track;
    be<u32>* updates = sequence->trackupdate;
    if (updates[channel]) {
        SeSeqUpdateView* target = getSePlayTrackInfo(audioSeSequenceHandle->track);
        channel = sound->track;
        updates = sequence->trackupdate;
        u32 flags = updates[channel];
        setSeSequencePortWord(target, channel, 1, flags);
        channel = sound->track;
        SePlayerParameterView* player = sequence->systemTrackParameter;
        addSePortCommand(player[channel].mCommand);
    }
}
/* 02807158 */
void checkSeMovePara() {
    JAISound* handle = audioSeSequenceHandle;
    if (!handle) return;
    SeSequenceParameterView* sequence = getSeSequenceParameter(handle);
    if (sequence->state == 2) return;
    for (u8 category = 0; category < basicCategoryMax(); ++category) {
        JAISound* sound = (JAISound*)(void*)seRegist[category].usedEnd;
        while (sound) {
            for (u32 parameter = 0; parameter < 8; ++parameter) {
                SeParameter* values = getSeSoundParameter(sound);
                moveSeParameter(&values->mVolumes[parameter]);
                values = getSeSoundParameter(sound);
                moveSeParameter(&values->mPans[parameter]);
                values = getSeSoundParameter(sound);
                moveSeParameter(&values->mFxmixes[parameter]);
                values = getSeSoundParameter(sound);
                moveSeParameter(&values->mAuxMixes[parameter]);
                values = getSeSoundParameter(sound);
                moveSeParameter(&values->mDolbys[parameter]);
                values = getSeSoundParameter(sound);
                moveSeParameter(&values->mPitches[parameter]);
            }
            sound = sound->next;
        }
    }
}
/* 02807274 */
void processGFrameSe() {
    checkSeNextFrame();
    checkSePlaying();
    checkSeParameterMoves();
}
/* 0280729C */
u8 changeIDToCategory(u32 id) { return id >> 12; }
/* 028072A4 */
SeParameter* getSeParametermeterPointer() {
    SeParameter* node = seParameterFreeStartPointer;
    if (!node) return nullptr;
    SeParameter* used = seParameterUsedEndPointer;
    seParameterFreeStartPointer = node->next;
    if (used) {
        node->next = used;
        seParameterUsedEndPointer->previous = node;
        node->previous = nullptr;
    } else {
        node->previous = nullptr;
        node->next = nullptr;
    }
    seParameterUsedEndPointer = node;
    return node;
}
}}
/* 028078A4 */
void __sinit_JAISeMgr_cpp() {
    seHeaderStatics.mWords[2] = 0;
    seHeaderStatics.mWords[0] = 0;
    seHeaderStatics.mWords[3] = 0;
    seHeaderStatics.mWords[1] = 0;
    registerSeStatics(&seDestructorRecord);
}
namespace JAInter { namespace SeMgr {
/* 02807300 */
void storeSeBuffer(gptr<JAISound>* handle, JAInter::Actor* actor, u32 id, u32 fade, u8 camera, JAISoundInfoView* info) {
    if (handle && *handle) {
        JAISound* previous = (JAISound*)(JAISound*)*handle;
        if (getSeSoundID(previous) != id || (getSeSoundID((JAISound*)(JAISound*)*handle) == id && (id & 0xC00) == 0x800)) {
            if (checkSeSoundHandle((JAISound*)(JAISound*)*handle, id, info)) return;
        }
    }
    u8 category = changeIDToCategory(id);
    JAISound* sound = (JAISound*)(void*)seRegist[category].usedEnd;
    if (!actor) actor = &seNullActor;
    cXyz* object = actor->field_0x0;
    SeCandidates buffer;
    gptr<JAISound>* candidates = buffer.entries;
    u8 count = 0;
    u8 max = categoryInfoTable[seScene][((id >> 12) & 255) * 2 + 1];
    while (sound) {
        if (gabi::ea((void*)sound->actor) == gabi::ea((cXyz*)object)) {
            if (getSeSoundID(sound) == id && !(info->mFlag & 0x80000)) {
                if (!(id & 0x800)) {
                    sound->state = sound->track == 255 ? 1 : 4;
                    if (handle && !*handle) {
                        sound->owner = handle;
                        *handle = (JAISound*)sound;
                    }
                    return;
                }
                stopSeSound(sound, 0);
                count = 255;
                break;
            }
            if (!count || getSePriority(candidates[0]) < getSePriority(sound)) {
                candidates[count] = sound;
            } else {
                for (u32 i = 0; i < count; ++i) candidates[i + 1] = candidates[i];
                candidates[0] = sound;
            }
            ++count;
        }
        sound = sound->next;
    }
    if (count == max) {
        if (getSePriority(candidates[0]) > (u8)info->priority) return;
        u32 priority = getSePriority(candidates[0]);
        JAISound* first = candidates[0];
        if (priority == (u8)info->priority && first->state == 5) return;
        releaseSeSoundRegistration(first);
    }
    JAISound* selected = getFreeSeSound(seRegist + category);
    f32 zero = 0.0f;
    if (!selected) {
        f32 maxDistance = zero;
        JAISound* replacement = nullptr;
        for (JAISound* current = (JAISound*)(void*)seRegist[category].usedEnd; current; current = current->next) {
            f32 distance = zero;
            u8 first = current->cameraIndex;
            u8 end;
            if (first == 4) {
                first = 0;
                end = (u8)getSeCameraMax();
            } else end = first + 1;
            for (u32 i = first; i < end; ++i) {
                f32 value = current->positionInfo[i].distance;
                if (!(value < distance)) distance = value;
            }
            if (current->state != 1 && !(maxDistance > distance)) {
                maxDistance = distance;
                replacement = current;
            }
        }
        if (!replacement) {
            if (handle) *handle = nullptr;
            return;
        }
        stopSeSound(replacement, 0);
        selected = getFreeSeSound(seRegist + category);
    }
    SeParameter* parameter = getFreeSeParameters();
    if (!parameter) {
        releaseSeLinkSound(seRegist + category, selected);
        *handle = nullptr;
        return;
    }
    f32 center = (f32)getSeDolbyCenter() / 127.0f;
    for (u32 i = 0; i < 8; ++i) {
        parameter->mVolumes[i].mTargetValue = 1.0f;
        parameter->mVolumes[i].mCurrentValue = 1.0f;
        parameter->mVolumes[i].mMoveCounter = 0;
        parameter->mPans[i].mMoveCounter = 0;
        parameter->mPans[i].mCurrentValue = 0.5f;
        parameter->mPans[i].mTargetValue = 0.5f;
        parameter->mPitches[i].mTargetValue = 1.0f;
        parameter->mPitches[i].mCurrentValue = 1.0f;
        parameter->mPitches[i].mMoveCounter = 0;
        parameter->mFxmixes[i].mCurrentValue = zero;
        parameter->mFxmixes[i].mTargetValue = zero;
        parameter->mFxmixes[i].mMoveCounter = 0;
        parameter->mAuxMixes[i].mCurrentValue = zero;
        parameter->mAuxMixes[i].mTargetValue = zero;
        parameter->mAuxMixes[i].mMoveCounter = 0;
        parameter->mDolbys[i].mCurrentValue = center;
        parameter->mDolbys[i].mMoveCounter = 0;
        parameter->mDolbys[i].mTargetValue = center;
    }
    MoveParaSet* groups[] = {parameter->mVolumes, parameter->mPans, parameter->mPitches, parameter->mFxmixes, parameter->mAuxMixes, parameter->mDolbys};
    for (u32 i = 0; i < 6; ++i) {
        groups[i][7].mCurrentValue = -1.0f;
        groups[i][7].mTargetValue = -1.0f;
        groups[i][7].mMoveCounter = 0;
    }
    parameter->dolbySource = nullptr;
    parameter->fxmixSource = nullptr;
    parameter->updateFlags = 0;
    parameter->mPortFlags = 0;
    parameter->panSource = nullptr;
    parameter->volumeSource = nullptr;
    parameter->pitchSource = nullptr;
    selected->state = 1;
    selected->track = 255;
    selected->parameter = parameter;
    initSeSound(selected, handle, actor, id, fade, camera, info);
}
}}
namespace JAInter { namespace SeMgr {
/* 0280699C */
void checkPlayingSe() {
    u8 trackIndex = 0;
    for (u8 category = 0; category < basicCategoryMax(); ++category) {
        for (u8 slot = 0; slot < categoryInfoTable[seScene][category * 2]; ++slot, ++trackIndex) {
            JAISound* sound = sePlaySound[category][slot];
            if (!sound) continue;
            sound->frameCounter = sound->frameCounter + 1;
            u32 route = 0x20000000 + (trackIndex >> 4) + ((trackIndex << 4) & 0xF0);
            BasicTrackView* track = (BasicTrackView*)getBasicSeqParameter((JAISound*)(JAISound*)audioSeSequenceHandle)->rootTrack;
            SePlayingPorts ports;
            readSeTrackPort(track, route + 0x20000, &ports.activity);
            readSeTrackPort(track, route, &ports.status);
            SePositionInfo* positions = sound->positionInfo;
            for (u8 camera = 0; camera < getSeCameraMax(); ++camera) {
                positions[camera].distance = (f32)sqrtSeDistance(positions[camera].distance);
            }
            if (sound->state == 2) {
                u32 bits = getSeSwitchBits(sound);
                sound->track = trackIndex;
                if (bits & 8) setSeMuteOnStart(sound);
                if (bits & 0xC0) {
                    u32 next = seRandomSeed * 0x19660D + 0x3C6EF35F;
                    f32 random = gabi::f32_from_bits((next >> 9) | 0x3F800000) - 1.0f;
                    u8 value = (u8)gabi::ftoi(random * 255.0f);
                    seRandomSeed = next;
                    switch (bits & 0xC0) {
                    case 0x40: sound->flags = value & 15; break;
                    case 0x80: sound->flags = value & 31; break;
                    case 0xC0: sound->flags = value & 63; break;
                    default: sound->flags = 0; break;
                    }
                }
                SeParameter* parameter = getSeSoundParameter(sound);
                u16 flags = parameter->mPortFlags;
                for (u8 port = 0; flags; ++port) {
                    u32 shift = port & 63;
                    u32 bit = shift < 32 ? 1u << shift : 0;
                    if (flags & bit) {
                        SeParameter* current = getSeSoundParameter(sound);
                        u16 value = current->mPortValues[port];
                        setSeTrackPort(audioSeSequenceHandle, sound->track, port, value);
                        flags = parameter->mPortFlags ^ bit;
                        parameter->mPortFlags = flags;
                    }
                }
                SeSoundVtable* soundTable = sound->vtable;
                gabi::call_ptr<void>(soundTable->distance, sound);
                JAIBasic* basic = msBasic;
                SeBasicPlayingVtable* table = (SeBasicPlayingVtable*)(BasicVtableRecord*)basic->mVtable;
                gabi::call_ptr<void>(table->extend, basic, sound);
                if (sound->fadeCounter > 1) {
                    setSeSoundInterVolume(sound, 6, 0, 0, 0.0f);
                    setSeSoundInterVolume(sound, 6, sound->fadeCounter, 0, 127.0f);
                    sound->fadeCounter = 0;
                }
                sendSeParameters(sound);
                u32 soundID = getSeSoundID(sound);
                u16 offset = getSeOffset(msBasic, soundID);
                if (checkSeSwitch(sound, 0x800)) {
                    basic = msBasic;
                    table = (SeBasicPlayingVtable*)(BasicVtableRecord*)basic->mVtable;
                    offset += gabi::call_ptr<u32>(table->ground, basic, (u32)sound->mapInfo);
                }
                u32 wait = 0;
                if (getSeCameraMax() == 1 && checkSeSwitch(sound, 0x1000)) {
                    SePositionInfo* position = sound->positionInfo;
                    f32 maximum = (f32)getSeDistanceMax();
                    if (position->distance < maximum) {
                        u32 distance = seFloatToUnsigned(sound->positionInfo->distance);
                        u32 limit = seFloatToUnsigned((f32)getSeDistanceMax());
                        u32 scaled = getSeWaitMax() * distance;
                        wait = limit ? (u16)(scaled / limit) : 0;
                    } else wait = getSeWaitMax();
                }
                writeBasicTrackPort(track, route + 0x30000, wait);
                basic = msBasic;
                table = (SeBasicPlayingVtable*)(BasicVtableRecord*)basic->mVtable;
                u32 fx = gabi::call_ptr<u32>(table->fxline, basic, (u32)sound->mapInfo);
                writeBasicTrackPort(track, route + 0x60000, fx);
                writeBasicTrackPort(track, route + 0x40000, offset);
                writeBasicTrackPort(track, route, 1);
                sound->state = (getSeSoundID(sound) & 0xC00) ? 4 : 5;
            } else if (!ports.activity && ports.status != 1) {
                releaseSeSoundRegistration(sound);
            } else if (sound->fadeCounter) {
                if (getSeSoundParameter(sound)->mVolumes[6].mCurrentValue == 0.0f) {
                    releaseSeSoundRegistration(sound);
                } else {
                    SeSoundVtable* table = sound->vtable;
                    gabi::call_ptr<void>(table->distance, sound);
                    sendSeParameters(sound);
                    sound->state = (getSeSoundID(sound) & 0xC00) ? 4 : 5;
                }
            } else if (sound->state == 3) {
                SeSoundVtable* table = sound->vtable;
                gabi::call_ptr<void>(table->distance, sound);
                sendSeParameters(sound);
                sound->state = (getSeSoundID(sound) & 0xC00) ? 4 : 5;
            }
        }
    }
}
}}
namespace JAInter { namespace SeMgr {
/* 02805B38 */
void checkNextFrameSe() {
    JAISound scratch;
    constructSeScratch(&scratch);
    if (audioSeSequenceHandle->state >= 4) {
        f32 distance = (f32)getSeDistanceMax();
        f32 distanceSquared = distance * (f32)getSeDistanceMax();
        f32 distanceScale = (f32)getSeDistanceMax() / 1000.0f;
        if (distanceScale == 0.0f) distanceScale = 1.0f;
        for (u32 category = 0; category < basicCategoryMax(); ++category) {
            SeRankBuffer ranked;
            SeCategoryRow* sceneRow = categoryInfoTable + seScene;
            for (u8 i = 0; i < (*sceneRow)[category * 2]; ++i) {
                ranked.entries[i].state = 255;
                ranked.entries[i].priority = 0x7FFFFFFF;
                ranked.entries[i].sound = nullptr;
            }
            JAISound* sound = (JAISound*)(void*)seRegist[category].usedEnd;
            u8 selectedCount = 0;
            while (sound) {
                if (sound->state == 1 && (getSeSoundID(sound) & 0xC00)) {
                    sound->camera = sound->camera - 1;
                    if (!sound->camera) {
                        scratch.next = sound->next;
                        releaseSeSoundRegistration(sound);
                        sound = scratch.next;
                        continue;
                    }
                } else {
                    if (!(getSeSoundID(sound) & 0xC00) && sound->state == 5) {
                        scratch.next = sound->next;
                        releaseSeSoundRegistration(sound);
                        sound = &scratch;
                    }
                    if (!sound->camera) {
                        scratch.next = sound->next;
                        releaseSeSoundRegistration(sound);
                        sound = scratch.next;
                        continue;
                    }
                }
                if (sound->state) {
                    f32 minDistance = 2147483648.0f;
                    u8 first = sound->cameraIndex;
                    u8 end;
                    if (first == 4) { first = 0; end = (u8)getSeCameraMax(); }
                    else end = first + 1;
                    for (u8 camera = first; camera < end; ++camera) {
                        SePositionInfo* position = sound->positionInfo + camera;
                        position->previous.x = position->current.x;
                        position->previous.y = position->current.y;
                        position->previous.z = position->current.z;
                        cXyz* source = sound->position;
                        if (!source) {
                            position->current.x = seDefaultPosition.x;
                            position->current.y = seDefaultPosition.y;
                            position->current.z = seDefaultPosition.z;
                        } else {
                            Camera* cameras = msBasic->mAudioCamera;
                            transformSePosition(cameras[camera].matrix, source, &position->current);
                        }
                        f32 x = position->current.x, y = position->current.y, z = position->current.z;
                        position->distance = gabi::fmadds(z, z, gabi::fmadds(x, x, y * y));
                        u32 priority = getSePriority(sound);
                        s16 adjustment = sound->priorityAdjustment;
                        if (adjustment) {
                            s16 adjusted = (s16)(priority + adjustment);
                            priority = adjusted < 0 ? 0 : adjusted > 255 ? 255 : adjusted;
                        }
                        u32 distanceRank = seFloatToUnsigned((f32)position->distance / distanceScale);
                        s32 weight = (s32)((255u - priority) * (255u - priority) * 0x1690u);
                        u32 priorityRank = seFloatToUnsigned((f32)weight / distanceScale);
                        u32 rank = distanceRank + priorityRank;
                        sound->elapsed = rank;
                        z = position->current.z;
                        if (z > 0.0f) sound->elapsed = rank + seFloatToUnsigned((z * 6.0f) / distanceScale);
                        f32 currentDistance = position->distance;
                        if (!camera || currentDistance < minDistance) minDistance = currentDistance;
                    }
                    if (sound->cameraIndex == 4) {
                        u32 cameras = getSeCameraMax();
                        sound->elapsed = cameras ? sound->elapsed / cameras : 0;
                    }
                    bool outside;
                    if (getSeSwitchBits(sound) & 0x20) outside = minDistance > distanceSquared;
                    else outside = minDistance > 10000000000.0f;
                    if (outside) {
                        if (!(getSeSoundID(sound) & 0xC00)) {
                            if (sound->state != 1 && sound->track != 255) {
                                BasicTrackView* track = (BasicTrackView*)getBasicSeqParameter((JAISound*)(JAISound*)audioSeSequenceHandle)->rootTrack;
                                u32 channel = sound->track;
                                u32 route = 0x20000000 + (channel >> 4) + ((channel << 4) & 0xF0);
                                writeBasicTrackPort(track, route, 0);
                                setSeTrackInterrupt(audioSeSequenceHandle, sound->track, 1);
                            }
                            sound->track = 255;
                            sound->state = 5;
                            sound = sound->next;
                            continue;
                        }
                        scratch.next = sound->next;
                        releaseSePlayingBuffer(sound, 0);
                        sound = scratch.next;
                        continue;
                    }
                    u32 soundCategory = getSeCategory(sound);
                    u8 max = categoryInfoTable[seScene][soundCategory * 2];
                    for (u8 i = 0; i < max; ++i) {
                        SeRankEntry* entry = &ranked.entries[i];
                        u32 rank = sound->elapsed;
                        if (entry->priority > rank || (entry->priority == rank && entry->state >= sound->state)) {
                            if (selectedCount < max) ++selectedCount;
                            for (u8 j = max - 1; j > i; --j) {
                                ranked.entries[j].priority = ranked.entries[j - 1].priority;
                                ranked.entries[j].sound = ranked.entries[j - 1].sound;
                                // HD's copy loop writes each shifted state into the insertion slot.
                                entry->state = ranked.entries[j - 1].state;
                            }
                            entry->priority = sound->elapsed;
                            entry->sound = sound;
                            entry->state = sound->state;
                            break;
                        }
                    }
                }
                sound = sound->next;
            }
            for (u32 i = 0; i < selectedCount; ++i) {
                JAISound* current = ranked.entries[i].sound;
                if (current->state == 1) current->state = 2;
                else if (current->state == 4) current->state = 3;
            }
            u8 max = categoryInfoTable[seScene][category * 2];
            for (u32 slot = 0; slot < max; ++slot) {
                JAISound* current = sePlaySound[category][slot];
                bool fill = !current;
                if (current) {
                    if (current->state == 4) {
                        if (getSeSoundID(current) & 0xC00) releaseSeSoundRegistration(current);
                        else { current->state = 1; current->camera = 0; }
                        fill = true;
                    } else if (!current->state || current->state == 5) {
                        sePlaySound[category][slot] = nullptr;
                        fill = true;
                    } else {
                        for (u8 i = 0; i < max; ++i) {
                            if (sePlaySound[category][slot] == ranked.entries[i].sound) {
                                ranked.entries[i].sound = nullptr;
                                break;
                            }
                        }
                    }
                }
                if (fill) {
                    bool available = true;
                    u8 i = 0;
                    for (; i < max; ++i) {
                        SeRankEntry* entry = &ranked.entries[i];
                        JAISound* candidate = entry->sound;
                        if (candidate && candidate->state != 3) {
                            for (u8 j = 0; j < max; ++j) {
                                JAISound* existing = sePlaySound[category][j];
                                if (existing && existing == entry->sound) { available = false; break; }
                            }
                            if (available) {
                                sePlaySound[category][slot] = candidate;
                                entry->sound = nullptr;
                                i = max + 1;
                                break;
                            }
                        }
                    }
                    if (i == max) sePlaySound[category][slot] = nullptr;
                }
            }
        }
    }
    destroySeScratch(&scratch, 2);
}
}}
/* WWHD glue
data seDefaultPosition 101F9B84
ext 0280AF40 void constructSeScratch(JAISound*)
ext 0280AFE0 void destroySeScratch(JAISound*, u32)
ext 028E8F64 void transformSePosition(void*, cXyz*, cXyz*)
ext 02805AFC void releaseSePlayingBuffer(JAISound*, u32)
data seRandomSeed 104B4F8C
ext 02818C68 void readSeTrackPort(BasicTrackView*, u32, be<u16>*)
ext 028F4384 f64 sqrtSeDistance(f64)
ext 0280C504 void setSeTrackPort(JAISound*, u32, u32, u32)
ext 028064DC void setSeMuteOnStart(JAISound*)
ext 02806824 void sendSeParameters(JAISound*)
ext 028030B4 u32 getSeOffset(JAIBasic*, u32)
ext 0280D300 u32 checkSeSwitch(JAISound*, u32)
ext 02804718 f64 getSeDistanceMax()
ext 02804808 u32 getSeWaitMax()
data seNullActor 104B4F28
ext 0280D2D8 u32 getSeSoundID(JAISound*)
ext 0280D12C u32 checkSeSoundHandle(JAISound*, u32, JAISoundInfoView*)
ext 0280D2C4 u32 getSePriority(JAISound*)
ext 0280B1D8 void stopSeSound(JAISound*, u32)
ext 0280DB3C JAISound* getFreeSeSound(JAInter::LinkSound*)
ext 028072A4 JAInter::SeParameter* getFreeSeParameters()
ext 028047F0 u32 getSeCameraMax()
ext 0280D18C void initSeSound(JAISound*, gptr<JAISound>*, JAInter::Actor*, u32, u32, u32, JAISoundInfoView*)
data seRegistBuffer 104B5014
data seEntryCancel 104B5020
data seCurrentHeap 101F9B54
data seArrayCookie 101FCD08
data seDefaultCategoryInfo 101F9B64
ext 028EFF8C void* constructSeArray(void*, u32, u32, SeArrayConstructor, u32)
ext 02807964 void* initializeSeLink(void*) callback
ext 02804DFC void* initializeSeParameters(void*) callback
ext 028046C4 u32 getSeRegistMax()
ext 028046D0 u32 getSeTrackMax()
ext 028046B8 u32 getSeSceneMax()
ext 02804478 void setSeTrackMax(u32)
ext 0280D90C void initSeLink(JAInter::LinkSound*, JAISound*, u32)
str 1016F724 "JAIBasic.h" in 02805028
str 1016F6CC "msCurrentHeap" in 02805028
str 1016F730 "JAISeMgr.cpp" in 02805028
str 1016F740 "seRegist"
str 1016F6C0 "sePlaySound"
str 1016F74C "seRegistBuffer"
str 1016F6DC "seRegistBuffer[i]"
str 1016F75C "sePlaySound[i]"
str 1016F6F0 "seTrackUpdate"
str 1016F6B0 "parameterObject"
str 1016F700 "categoryInfoTable"
str 1016F714 "seEntryCancel"
str 1016F76C "seCategoryVolume"
data categoryInfoTable 104B5000
data sePlaySound 104B5004
data seScene 101F9C7C
ext 0280CAD4 void setSeTrackInterrupt(JAISound*, u32, u32)
ext 0280DBE0 void releaseSeLinkSound(JAInter::LinkSound*, JAISound*)
data seTrackUpdate 104B4FFC
data seRegist 104B5010
data seCategoryVolume 104B501C
ext 0280B384 JAInter::SeParameter* getSeSoundParameter(JAISound*)
ext 0280B05C JAInter::SeSequenceParameterView* getSeSequenceParameter(JAISound*)
ext 0280D2D0 u32 getSeCategory(JAISound*)
ext 0280E31C void setSeSequencePortWord(SeSeqUpdateView*, u32, u32, u32)
ext 0280E9AC void addSePortCommand(void*)
ext 0280D8C0 u32 moveSeParameter(JAInter::MoveParaSet*)
data audioSeSequenceHandle 104B5018
data msBasic 104B4F14
data seqMuteFlagFromSe 101F9C78
str 1016F780 "JAISeMgr.cpp" in 028057D4
str 1016F790 "JAISeMgr::seHandle"
ext 0273AA24 void seAssert(const char*, u32, const char*)
ext 0280AD2C SeSeqUpdateView* getSePlayTrackInfo(u32)
ext 0280E300 void setSeSequencePortFloat(SeSeqUpdateView*, u32, u32, f32)
ext 0280B518 void setSeSoundInterVolume(JAISound*, u32, u32, u32, f32)
ext 0280470C u32 getSeSequenceTrackMax()
ext 0280CFF0 u32 getSeSwitchBits(JAISound*)
ext 028047D8 u32 getSeMuteVolume()
ext 028047E4 u32 getSeMuteMoveSpeed()
ext 0280B2AC void setSeSequenceVolume(JAISound*, u32, u32, f32)
ext 028059B0 void releaseSeSoundRegistration(JAISound*)
data seParameterFreeStartPointer 104B5008
data seParameterUsedEndPointer 104B500C
data seHeaderStatics 104B4FEC
data seDestructorRecord 101F9C6C
ext 028EFFD0 void constructSeMoveArray(void*, u32, u32, SeMoveConstructor)
ext 028078CC void initializeSeHalfParameter(JAInter::MoveParaSet*) callback
ext 02807918 void initializeSeZeroParameter(JAInter::MoveParaSet*) callback
ext 02804754 f64 getSeDolbyCenter()
ext 02805B38 void checkSeNextFrame()
ext 0280699C void checkSePlaying()
ext 02807158 void checkSeParameterMoves()
ext 028F026C void registerSeStatics(AudioDestructorRecord*)
*/

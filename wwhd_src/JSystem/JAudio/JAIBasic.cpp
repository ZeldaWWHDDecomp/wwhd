/* Based on CC0 tww JAIBasic.cpp; WWHD extra state and register widths. */
#include "JSystem/JAudio/JAIBasic.h"
#include "JSystem/JAudio/JAIBankWave.h"
#include "JSystem/JAudio/JAIDummyObject.h"
#include "JSystem/JAudio/JAISoundParams.h"
#include "JSystem/JAudio/JAIConst.h"
#include "JSystem/JAudio/JAIFx.h"
#include "JSystem/JAudio/JAISequenceHeap.h"
using BasicByte = be<u8>;
static JAIBasic* audioBasicSingleton;
static JAISound* seHandle;
static JAInter::LinkSound* seRegist;
static JAISound* sequenceUsed;
static JAISound* streamUsed;
using BasicSoundPointer = gptr<JAISound>;
static BasicSoundPointer* FixSeqBufPointer;
static BasicByte* seEntryCancel;
static u16 streamFlags;
static u32 dummyObjectLifeTime;
static u32 arrayCookieSize;
static void* aafPointer;
static u32 interfaceHeapSize;
static u32 audioWorkPool;
static u32 seqPlayTrackMax;
static BasicSeTrackUpdate* seTrackUpdate;
static u8 seScene;
static u32 systemTrackMax;
static u8 audioDvdThreadPriority;
static u8 audioSystemThreadPriority;
static u8 audioThreadReady;
static f32 inputGainDown;
static f32 outputGainUp;
static u8 stayHeapMax;
static u8 autoHeapMax;
static u32 stayHeapSize;
static u32 autoHeapRoomSize;
static BasicByte* audioResPath;
static BasicByte* wavePath;
static BasicByte* streamPath;
static void* audioFileDevice;
static JKRHeap* audioCurrentHeap;
static JKRHeap* sSystemHeap;
static u32 audioCameraMax;
using BasicCategoryVolume = be<f32>;
static BasicCategoryVolume* seCategoryVolume;
static BasicVtableRecord basicVtable;
static JAInter::Camera nullCamera;
static BasicCameraMatrix camMtx;
static BasicVectorBits dummyZeroVec;
static AudioHeaderStatics basicHeaderStatics;
static AudioDestructorRecord basicDestructorRecord;
static BasicHeapName jaiDramName;
static BasicHeapName jasDramName;
static BasicHeapName jasAramName;
struct BasicStringVtable { be<u32> words[2]; };
static BasicStringVtable basicStringVtable;
namespace JAInter {
/* 02802550 */
void Camera::init() {
    position = nullptr;
    matrix = nullptr;
    previousPosition = nullptr;
}
}
/* 02802564 */
JAIBasic::JAIBasic() {
    mVtable = &basicVtable;
    audioBasicSingleton = this;
    u16 flags = mFlags;
    mAudioCamera = nullptr;
    mInitData = nullptr;
    mFlags = flags & 0x7FF;
    mSceneTable = nullptr;
    initLoadFileSw = 2;
    mStreamParameter = 0;
    mFrameCounter = 0;
    mHeap = nullptr;
    audioCurrentHeap = sSystemHeap;
    mDriverHeap = nullptr;
    mStreamBuffer = nullptr;
    mFrameCallback = nullptr;
    mDriverParameters = nullptr;
}
/* 028025FC */
void JAIBasic::startSoundBasic(u32 id, gptr<JAISound>* sound, JAInter::Actor* actor, u32 info, u32 owner, JAISoundInfoView* entry) {
    JAIFrameCallback* callback = mFrameCallback;
    if (callback) {
        u32 target = callback->vtable->remapID;
        id = gabi::call_ptr<u32>(target, callback, id);
        callback = mFrameCallback;
        target = callback->vtable->start;
        if (gabi::call_ptr<u32>(target, callback, id, sound)) return;
    }
    switch (id & 0xC0000000u) {
    case 0x80000000u:
        if (mFlags & 0x4000) return;
        if (seHandle && !((seHandle->mSoundID ^ id) & 0x3FF)) return;
        if (!sound) sound = FixSeqBufPointer + (u8)entry->fixedSequenceSlot;
        storeBasicSequence(sound, actor, id, info, owner, entry);
        return;
    case 0:
        if (!seEntryCancel[id >> 12]) storeBasicSe(sound, actor, id, info, owner, entry);
        else if (sound) *sound = nullptr;
        return;
    case 0xC0000000u:
        if ((streamFlags & 0x4000) || (mFlags & 0x2000)) return;
        storeBasicStream(sound, actor, id, info, owner, entry);
        return;
    default: basicSoundAssert("JAIBasic.cpp", 761, "(0)");
    }
}
/* 02802880 */
void JAIBasic::prepareSoundVec(u32 id, gptr<JAISound>* sound, cXyz* position, u32 info, u32 flags, u32 owner) {
    JAIFrameCallback* callback = mFrameCallback;
    if (callback) {
        u32 target = callback->vtable->remapID;
        id = gabi::call_ptr<u32>(target, callback, id);
        callback = mFrameCallback;
        target = callback->vtable->prepare;
        if (gabi::call_ptr<u32>(target, callback, id, sound)) return;
    }
    startSoundVec(id, sound, position, info, flags, owner);
    JAISound* playing = *sound;
    if (playing) setBasicPrepareFlag(playing, 1);
}
/* 0280293C */
void JAIBasic::stopActorSoundOneBuffer(void* actor, JAISound* sound) {
    JAISound* node = (JAISound*)sound;
    while (node) {
        void* owner = node->actor;
        JAISound* next = node->next;
        if (owner == actor) animeStopSound((JAISound*)node, 1);
        node = next;
    }
}
/* 02802994 */
void JAIBasic::stopAllSound(void* actor) {
    for (u32 category = 0; category < basicCategoryMax(); ++category)
        stopActorSoundOneBuffer(actor, (JAISound*)(void*)seRegist[category].usedEnd);
    stopActorSoundOneBuffer(actor, sequenceUsed);
    stopActorSoundOneBuffer(actor, streamUsed);
}
/* 02802A2C */
void JAIBasic::stopSoundHandle(JAISound* sound, u32 fade) {
    if (!sound) return;
    switch (((JAISound*)sound)->mSoundID & 0xC0000000u) {
    case 0x80000000u: releaseBasicSequence(sound, fade); return;
    case 0: releaseBasicSe(sound, fade); return;
    case 0xC0000000u: releaseBasicStream(sound, fade); return;
    default: basicSoundAssert("JAIBasic.cpp", 811, "(0)");
    }
}
/* 02802A80 */
void JAIBasic::stopIDSoundOneBuffer(u32 id, JAISound* sound) {
    JAISound* node = (JAISound*)sound;
    while (node) {
        u32 playingID = node->mSoundID;
        JAISound* next = node->next;
        if (playingID == id) stopSoundHandle((JAISound*)node, 0);
        node = next;
    }
}
/* 02802AEC */
void JAIBasic::stopAllSound(u32 id) {
    switch (id & 0xC0000000u) {
    case 0: {
        u32 category = basicSoundCategory(id);
        stopIDSoundOneBuffer(id, (JAISound*)(void*)seRegist[category].usedEnd);
        return;
    }
    case 0x80000000u: stopIDSoundOneBuffer(id, sequenceUsed); return;
    case 0xC0000000u: stopIDSoundOneBuffer(id, streamUsed); return;
    default: basicSoundAssert("JAIBasic.cpp", 1053, "(0)");
    }
}
/* 02802BDC */
void JAIBasic::stopIDActorSoundOneBuffer(u32 id, void* actor, JAISound* sound) {
    JAISound* node = (JAISound*)sound;
    while (node) {
        u32 playingID = node->mSoundID;
        JAISound* next = node->next;
        if (playingID == id && node->actor == actor) stopSoundHandle((JAISound*)node, 0);
        node = next;
    }
}
/* 02802C60 */
void JAIBasic::stopAllSound(u32 id, void* actor) {
    switch (id & 0xC0000000u) {
    case 0: {
        u32 category = basicSoundCategory(id);
        stopIDActorSoundOneBuffer(id, actor, (JAISound*)(void*)seRegist[category].usedEnd);
        return;
    }
    case 0x80000000u: stopIDActorSoundOneBuffer(id, actor, sequenceUsed); return;
    case 0xC0000000u: stopIDActorSoundOneBuffer(id, actor, streamUsed); return;
    default: basicSoundAssert("JAIBasic.cpp", 1074, "(0)");
    }
}
/* 02802D70 */
void JAIBasic::stopPlayingCategoryObjectSe(u32 category, void* actor) {
    if (category >= basicCategoryMax()) return;
    JAISound* node = (JAISound*)(void*)seRegist[category].usedEnd;
    while (node) {
        void* owner = node->actor;
        JAISound* next = node->next;
        if (owner == actor) stopSoundHandle((JAISound*)node, 0);
        node = next;
    }
}
/* 028027DC */
void JAIBasic::startSoundDirectID(u32 id, gptr<JAISound>* sound, JAInter::Actor* actor, u32 info, u32 owner) {
    JAISoundInfoView* entry = getBasicSoundInfo(id);
    if (entry) startSoundBasic(id, sound, actor, info, owner, entry);
}
/* 02802844 */
void JAIBasic::startSoundActor(u32 id, gptr<JAISound>* sound, JAInter::Actor* actor, u32 info, u32 owner) {
    startSoundDirectID(id, sound, actor, info, owner);
}
/* 02802848 */
void JAIBasic::startSoundVec(u32 id, gptr<JAISound>* sound, cXyz* position, u32 info, u32 flags, u32 owner) {
    JAInter::Actor actor;
    actor.field_0x0 = position;
    actor.field_0x8 = position;
    actor.mPosition = position;
    actor.mMapInfo = flags;
    startSoundActor(id, sound, &actor, info, owner);
}
/* 02802DFC */
void JAIBasic::stopAllSe(u32 category, void* object) { stopPlayingCategoryObjectSe(category, object); }
/* 02802E00 */
void JAIBasic::deleteObject(void* actor) {
    JAInter::DummyVec* dummy = nullptr;
    for (u32 category = 0; category < basicCategoryMax(); ++category) {
        JAISound* node = (JAISound*)(void*)seRegist[category].usedEnd;
        while (node) {
            void* owner = node->actor;
            JAISound* next = node->next;
            if (owner == actor) {
                if (basicSoundSwitch((JAISound*)node, 0x8000)) {
                    if (!dummy) dummy = JAInter::DummyObjectMgr::getPointer(dummyObjectLifeTime, 0);
                    if (dummy) {
                        be<u32>* input = (be<u32>*)(cXyz*)node->position;
                        be<u32>* output = (be<u32>*)dummy->position;
                        output[0] = input[0];
                        output[1] = input[1];
                        u32 z = input[2];
                        dummy->sound = node;
                        output[2] = z;
                        basicClearSoundOwner((JAISound*)node);
                        node->actor = nullptr;
                        node->owner = (gptr<JAISound>*)&dummy->sound;
                        node->position = (cXyz*)dummy->position;
                    } else stopSoundHandle((JAISound*)node, 0);
                } else stopSoundHandle((JAISound*)node, 0);
            }
            node = next;
        }
    }
}
/* 02802F90 */
JAISound* JAIBasic::makeSound(u32 count) {
    JKRHeap* heap = mHeap;
    u32 cookie = arrayCookieSize;
    if (!heap) heap = sSystemHeap;
    void* storage = allocateBasicSoundArray(count * 72 + cookie, heap, 4);
    if (!storage) return nullptr;
    cookie = arrayCookieSize;
    return constructBasicSoundArray((u8*)storage + cookie, count, 72, constructBasicSound, 0);
}
/* 02802F20 */
void JAIBasic::processFrameWork() {
    if (!canProcessAudioFrame()) return;
    JAInter::DummyObjectMgr::check();
    JAInter::BankWave::loadSecondStayWave();
    processBasicSeFrame();
    processBasicSequenceFrame();
    processBasicStreamFrame();
    JAIFrameCallback* callback = mFrameCallback;
    if (callback) {
        u32 target = callback->vtable->frame;
        gabi::call_ptr<void>(target, callback);
    }
    mFrameCounter = mFrameCounter + 1;
}
/* 0280307C */
void JAIBasic::setCameraInfo(cXyz* position, cXyz* previous, void* matrix, u32 camera) {
    if (audioCameraMax <= camera) return;
    mAudioCamera[camera].position = position;
    mAudioCamera[camera].previousPosition = previous;
    mAudioCamera[camera].matrix = matrix;
}
/* 028030B4 */
u16 JAIBasic::getSoundOffsetNumberFromID(u32 id) {
    if (getBasicSoundFormat(id) & 1) {
        JAISoundInfoView* info = getBasicSoundInfo(id);
        return info->mOffsetNo;
    }
    return id & 0x3FF;
}
/* 02803100 */
u32 JAIBasic::getMapInfoFxline(u32 map) { return map != 0; }
/* 0280310C */
u32 JAIBasic::getMapInfoGround(u32 map) { return map != 0; }
/* 02803118 */
f32 JAIBasic::getMapInfoFxParameter(u32 map) { return map ? 1.0f : 0.0f; }
/* 02803138 */
void JAIBasic::setSeCategoryVolume(u32 category, u32 volume) {
    seCategoryVolume[category] = (f32)volume / 127.0f;
}
/* 02803180 */
u16 JAIBasic::setParameterSeqSync(void* trackPointer, u32 command) {
    BasicTrackView* track = (BasicTrackView*)trackPointer;
    if (command == 0) {
        for (u32 index = 0; index < seqPlayTrackMax; ++index) {
            BasicSeqUpdateView* update = getBasicPlayTrackInfo(index);
            if (!update->sequence) continue;
            update = getBasicPlayTrackInfo(index);
            JAISound* sound = (JAISound*)(JAISound*)update->sequence;
            BasicSeqParameterView* parameter = getBasicSeqParameter(sound);
            BasicTrackView* root = (BasicTrackView*)parameter->rootTrack;
            update = getBasicPlayTrackInfo(index);
            u32 id = update->sequence->mSoundID;
            BasicTrackView* parent = track->parent;
            if (id & 0x800) parent = parent->parent;
            if (root == parent) {
                u32 route = JAInter::routeToTrack(track->route);
                update = getBasicPlayTrackInfo(index);
                id = update->sequence->mSoundID;
                JAISoundInfoView* info = getBasicSoundInfo(id);
                u32 flags = (info->mFlag >> 8) & 0xFFFF;
                update = getBasicPlayTrackInfo(index);
                initBasicOuterParameters(update, track, route, flags, command & 1);
                u32 shift = route & 63;
                u32 bit = shift < 32 ? 1u << shift : 0;
                update = getBasicPlayTrackInfo(index);
                update->updateFlags = update->updateFlags | bit;
                index = seqPlayTrackMax;
            }
        }
    } else if (command == 1) {
        u32 route = track->route & 255;
        BasicSeTrackUpdate* parameters = seTrackUpdate + route;
        void* outer = track->outer;
        setBasicOuterParameter(outer, 1, parameters->volume);
        setBasicOuterParameter(outer, 8, parameters->pan);
        setBasicOuterParameter(outer, 2, parameters->pitch);
        setBasicOuterParameter(outer, 4, parameters->fxmix);
        f32 dolby = audioBasicSingleton->mOutputMode == 2 ? (f32)parameters->dolby : 0.0f;
        setBasicOuterParameter(outer, 16, dolby);
    } else if (command == 127) writeBasicTrackPort(track, 0, seScene);
    return 0;
}
/* 02803378 */
void JAIBasic::setSeExtParameter(JAISound* sound) {
    if (!sound) return;
    JAISound* node = (JAISound*)sound;
    u32 format = getBasicSoundFormat(node->mSoundID);
    if (format & 4) {
        f32 volume = (f32)(u8)node->info->volume / 127.0f;
        animeSetVolume(sound, volume, 0, 1);
    }
    if (format & 8) {
        f32 fx = (f32)(u8)node->info->fxmix / 127.0f;
        basicSetFxmix(sound, fx, 0, 1);
    }
    if (format & 2) animeSetPitch(sound, node->info->pitch, 0, 1);
}
/* 02803618 */
u32 JAIBasic::initReadFile() {
    u32 mode = initLoadFileSw;
    if (mode == 2) {
        if (!checkBasicInitFile()) return 0;
    } else if (mode == 4) {
        if (aafPointer) checkBasicInitMemory();
        else basicSoundAssert("JAIBasic.cpp", 516, "JAIInitData::aafPointer");
    } else if (mode < 5) {
        basicSoundAssert("JAIBasic.cpp", 520, "initLoadFileSw>=JAI_INIT_MODE_MAX");
    }
    return 1;
}
/* 028036C8 */
void JAIBasic::initHeap() {
    u32 size = interfaceHeapSize;
    JKRHeap* parent = sSystemHeap;
    if (size) {
        JKRHeap* heap = createBasicSolidHeap(size, parent, 0);
        mHeap = heap;
        audioCurrentHeap = heap;
        if (!heap) basicSoundAssert("JAIBasic.cpp", 235, "msCurrentHeap");
    } else {
        audioCurrentHeap = parent;
        if (!parent) basicSoundAssert("JAIBasic.cpp", 235, "msCurrentHeap");
    }
}
/* 02803750 */
void JAIBasic::initResourcePath() {
    be<u8>* prefix = audioResPath;
    if (!prefix) return;
    BasicPathHeapView* heap = (BasicPathHeapView*)(JKRHeap*)sSystemHeap;
    u32 prefixLength = 0;
    while (prefix[prefixLength]) ++prefixLength;
    be<u8>* suffix = wavePath;
    u32 suffixLength = 0;
    while (suffix[suffixLength]) ++suffixLength;
    u32 target = heap->vtable->alloc;
    void* wave = gabi::call_ptr<void*>(target, heap, prefixLength + suffixLength + 1, 4);
    prefix = audioResPath;
    suffix = wavePath;
    formatBasicPath(wave, "%s%s%c", prefix, suffix, 0);
    prefix = audioResPath;
    heap = (BasicPathHeapView*)(JKRHeap*)sSystemHeap;
    wavePath = (be<u8>*)wave;
    prefixLength = 0;
    while (prefix[prefixLength]) ++prefixLength;
    suffix = streamPath;
    suffixLength = 0;
    while (suffix[suffixLength]) ++suffixLength;
    target = heap->vtable->alloc;
    void* stream = gabi::call_ptr<void*>(target, heap, prefixLength + suffixLength + 1, 4);
    prefix = audioResPath;
    suffix = streamPath;
    formatBasicPath(stream, "%s%s%c", prefix, suffix, 0);
    streamPath = (be<u8>*)stream;
}
/* 02803870 */
void JAIBasic::initArchive() {
    BasicArchiveName name;
    getBasicArchiveName(name.bytes);
    u32 size = checkAudioFile(audioFileDevice, (const char*)name.bytes);
    void* buffer = allocateBasicSoundArray(size, sSystemHeap, 64);
    mStreamBuffer = buffer;
    loadAudioFile(audioFileDevice, (const char*)name.bytes, buffer);
    setBasicArchive(mStreamBuffer);
}
/* 028038E4 */
void JAIBasic::initCamera() {
    JKRHeap* heap = audioCurrentHeap;
    if (!heap) { basicSoundAssert("JAIBasic.h", 169, "msCurrentHeap"); heap = audioCurrentHeap; }
    u32 count = audioCameraMax;
    void* storage = allocateBasicSoundArray(count * 12 + arrayCookieSize, heap, 32);
    JAInter::Camera* cameras = nullptr;
    if (storage) cameras = constructBasicCameras((u8*)storage + arrayCookieSize, count, 12, basicInitializeCamera, 0);
    mAudioCamera = cameras;
    if (!cameras) {
        basicSoundAssert("JAIBasic.cpp", 458, "mAudioCamera");
        cameras = mAudioCamera;
    }
    if (!cameras->position) {
        nullCamera.position->x = 0.0f;
        nullCamera.position->y = 0.0f;
        nullCamera.position->z = -50.0f;
        nullCamera.previousPosition->x = 0.0f;
        nullCamera.previousPosition->y = 0.0f;
        nullCamera.previousPosition->z = -50.0f;
        cXyz up;
        BasicVectorBits target;
        u32 x = dummyZeroVec.x;
        u32 z = dummyZeroVec.z;
        up.z = 0.0f;
        target.z = z;
        cXyz* eye = nullCamera.position;
        target.x = x;
        up.y = 1.0f;
        u32 y = dummyZeroVec.y;
        up.x = 0.0f;
        target.y = y;
        basicLookAt(&camMtx, eye, &up, &target);
        for (u32 camera = 0; camera < audioCameraMax; ++camera) {
            cXyz* position = nullCamera.position;
            cXyz* previous = nullCamera.previousPosition;
            setCameraInfo(position, previous, &camMtx, camera);
        }
    }
}
/* 02803494 */
void JAIBasic::initAudioThread(JKRHeap* parent, u32 size, u32 flags) {
    u32 overhead = getBasicHeapOverhead(64);
    void* heap = createBasicDramHeap(overhead + 0x400000, &jasDramName, parent, 1, 1);
    mDriverHeap = heap;
    initBasicDramHeap(heap, 8192);
    overhead = getBasicHeapOverhead(64);
    heap = createBasicDramHeap(size + overhead, &jasAramName, parent, 1, 0);
    mDriverParameters = heap;
    initBasicAramHeap(heap, size);
    initBasicAudioBank(0);
    activateAudioWorkPool(audioWorkPool);
    initBasicDvdAudio(0);
    initBasicAudioBridge();
    setBasicAudioBridgeMode(11, 0);
    initBasicTrackPool(systemTrackMax);
    startBasicDvdThread(audioDvdThreadPriority, 128, 4096, 0);
    startBasicAudioThread(audioSystemThreadPriority, 0);
    while (!audioThreadReady) {
        be<u32>* info = getBasicSystemInfo();
        u32 clock = info[0];
        u64 ticks = (u64)(clock >> 2) * 100;
        auto interval = divideBasicTicks(ticks >> 32, (u32)ticks, 0, 1000000);
        sleepBasicTicks(interval.r3, interval.r4);
    }
    BasicVtableRecord* table = mVtable;
    gabi::call_ptr<void>(table->entries[15], this);
    f32 output = outputGainUp;
    f32 input = inputGainDown;
    setBasicMixerLevel(input, output);
}
/* 02803A98 */
void JAIBasic::initInterfaceMain() {
    basicAudioInitializationFence();
    initHeap();
    initResourcePath();
    if (!initReadFile()) return;
    initArchive();
    if (!(mFlags & 0x8000)) JAInter::BankWave::setWaveScene();
    JAInter::DummyObjectMgr::init();
    JAInter::Fx::init();
    initBasicSequenceMgr();
    initBasicSeMgr();
    initBasicStreamMgr();
    u32 stayCount = stayHeapMax;
    u32 autoCount = autoHeapMax;
    u32 staySize = stayHeapSize;
    u32 autoSize = autoHeapRoomSize;
    JAInter::HeapMgr::init(stayCount, staySize, autoCount, autoSize);
    initCamera();
    BasicVtableRecord* table = mVtable;
    gabi::call_ptr<void>(table->entries[17], this);
    be<u32>* info = getBasicSystemInfo();
    u32 clock = info[0];
    u64 ticks = (u64)(clock >> 2) * 200;
    auto interval = divideBasicTicks(ticks >> 32, (u32)ticks, 0, 1000000);
    sleepBasicTicks(interval.r3, interval.r4);
    startBasicSeSequence();
    info = getBasicSystemInfo();
    clock = info[0];
    ticks = (u64)(clock >> 2) * 200;
    interval = divideBasicTicks(ticks >> 32, (u32)ticks, 0, 1000000);
    sleepBasicTicks(interval.r3, interval.r4);
    if (!(mFlags & 0x8000)) checkBasicEnteredSequence();
    basicAudioInitializationFence();
}
/* 02803480 */
void JAIBasic::setRegisterTrackCallback() { registerBasicTrackCallback(setParameterSeqSync); }
/* 0280348C */
void JAIBasic::setFrameCallback(JAIFrameCallback* callback) { mFrameCallback = callback; }
/* 02803614 */
void JAIBasic::initDriver(JKRHeap* heap, u32 size, u32 priority) { initAudioThread(heap, size, priority); }
/* 02803BC4 */
void JAIBasic::initInterface(u32 mode) { initInterfaceMain(); }
// HD: compiler-emitted SafeString helpers for this TU's heap names.
/* 02803C5C */
void BasicHeapName::destroy(u32 flags) {
    if (gabi::ea(this) && (flags & 1)) deleteBasicHeapName(this);
}
/* 02803C74 */
void BasicHeapName::assureTermination() {}
/* 02803C70 */
void JAIBasic::initStream() {}
/* 02803BC8 */
void __sinit_JAIBasic_cpp() {
    basicHeaderStatics.mWords[3] = 0;
    basicHeaderStatics.mWords[2] = 0;
    basicHeaderStatics.mWords[1] = 0;
    basicHeaderStatics.mWords[0] = 0;
    registerBasicStatics(&basicDestructorRecord);
    jaiDramName.vtable = gabi::ea(&basicStringVtable);
    jaiDramName.name = gabi::at<const char>(gabi::guest_str("JAIDram"));
    jasDramName.vtable = gabi::ea(&basicStringVtable);
    jasDramName.name = gabi::at<const char>(gabi::guest_str("JASDram"));
    jasAramName.vtable = gabi::ea(&basicStringVtable);
    jasAramName.name = gabi::at<const char>(gabi::guest_str("JASAram"));
}
using JAInter::Camera;
/* WWHD glue
ext 0273AF40 void deleteBasicHeapName(BasicHeapName*)
data seqPlayTrackMax 101F9BE0
data seTrackUpdate 104B4FFC
data seScene 101F9C7C
ext 0280AD2C BasicSeqUpdateView* getBasicPlayTrackInfo(u32)
ext 0280B05C BasicSeqParameterView* getBasicSeqParameter(JAISound*)
ext 0280E47C void initBasicOuterParameters(BasicSeqUpdateView*, BasicTrackView*, u32, u32, u32)
ext 02816CA4 void setBasicOuterParameter(void*, u32, f32)
ext 02818C20 void writeBasicTrackPort(BasicTrackView*, u32, u32)
data audioWorkPool 101F9D6C
data systemTrackMax 101F9C4C
data audioDvdThreadPriority 101F9C57
data audioSystemThreadPriority 101F9C56
data audioThreadReady 101F9F10
data inputGainDown 101F9C04
data outputGainUp 101F9C08
data stayHeapMax 101F9BDF
data autoHeapMax 101F9BDB
data stayHeapSize 101F9BE8
data autoHeapRoomSize 101F9BE4
ext 0275320C u32 getBasicHeapOverhead(u32)
ext 02753004 void* createBasicDramHeap(u32, BasicHeapName*, JKRHeap*, u32, u32)
ext 028103F0 void initBasicDramHeap(void*, u32)
ext 028104E8 void initBasicAramHeap(void*, u32)
ext 0280F168 void initBasicAudioBank(u32)
ext 0280F2C4 void activateAudioWorkPool(u32)
ext 0280EDC4 void initBasicDvdAudio(u32)
ext 0281A28C void initBasicAudioBridge()
ext 0281AC78 void setBasicAudioBridgeMode(u32, u32)
ext 02819810 void initBasicTrackPool(u32)
ext 0280EC38 void startBasicDvdThread(u32, u32, u32, u32)
ext 02813B5C void startBasicAudioThread(u32, u32)
ext C0009C80 be<u32>* getBasicSystemInfo()
ext 028F5A1C Pair32 divideBasicTicks(u32, u32, u32, u32)
ext C000A0D0 void sleepBasicTicks(u32, u32)
ext 028155B4 void setBasicMixerLevel(f32, f32)
ext C0009E90 void basicAudioInitializationFence()
ext 028085E8 void initBasicSequenceMgr()
ext 02805028 void initBasicSeMgr()
ext 0280E134 void initBasicStreamMgr()
ext 028057D4 void startBasicSeSequence()
ext 02808F50 void checkBasicEnteredSequence()
data nullCamera 104B4F68
data camMtx 104B4F38
data dummyZeroVec 101F9B84
data basicHeaderStatics 104B4F04
data basicDestructorRecord 101F9B48
data jaiDramName 104B4EEC
data jasDramName 104B4EF4
data jasAramName 104B4EFC
data basicStringVtable 1016F3B0
str 1016F4E8 "JAIDram"
str 1016F4F0 "JASDram"
str 1016F4F8 "JASAram"
str 1016F4BC "JAIBasic.h" in 028038E4
str 1016F4AC "msCurrentHeap" in 028038E4
str 1016F4C8 "JAIBasic.cpp" in 028038E4
str 1016F4D8 "mAudioCamera" in 028038E4
ext 02802550 void basicInitializeCamera(JAInter::Camera*) callback
ext 028EFF8C JAInter::Camera* constructBasicCameras(void*, u32, u32, BasicCameraConstructor, u32)
ext 028E9684 void basicLookAt(BasicCameraMatrix*, cXyz*, cXyz*, BasicVectorBits*)
ext 028F026C void registerBasicStatics(AudioDestructorRecord*)
data audioResPath 101F9C00
data wavePath 101F9BF0
data streamPath 101F9BF8
data audioFileDevice 101F9D58
str 1016F4A0 "%s%s%c"
ext 028F186C void formatBasicPath(void*, const char*, void*, void*, u32)
ext 02808C44 void getBasicArchiveName(be<u8>*)
ext 02808D10 void setBasicArchive(void*)
data aafPointer 101F9C68
data interfaceHeapSize 101F9BBC
str 1016F470 "JAIBasic.cpp" in 02803618
str 1016F434 "JAIInitData::aafPointer" in 02803618
str 1016F44C "initLoadFileSw>=JAI_INIT_MODE_MAX" in 02803618
str 1016F490 "JAIBasic.cpp" in 028036C8
str 1016F480 "msCurrentHeap" in 028036C8
ext 0280BFD8 void basicSetFxmix(JAISound*, f32, u32, u32)
ext 02804C38 u32 checkBasicInitFile()
ext 02804814 void checkBasicInitMemory()
ext 027EC464 JKRHeap* createBasicSolidHeap(u32, JKRHeap*, u32)
data dummyObjectLifeTime 101F9C40
data arrayCookieSize 101FCD08
ext 0280D300 u32 basicSoundSwitch(JAISound*, u32)
ext 0280CB4C void basicClearSoundOwner(JAISound*)
ext 0273B0D4 void* allocateBasicSoundArray(u32, JKRHeap*, s32)
ext 0280AF40 JAISound* constructBasicSound(JAISound*) callback
ext 028EFF8C JAISound* constructBasicSoundArray(void*, u32, u32, BasicSoundConstructor, u32)
data seHandle 104B5018
data seRegist 104B5010
data sequenceUsed 104B5050
data streamUsed 104B50E8
data FixSeqBufPointer 104B5098
data seEntryCancel 104B5020
data streamFlags 104B5134
str 1016F3CC "JAIBasic.cpp" in 028025FC
str 1016F3C8 "(0)" in 028025FC
str 1016F3E0 "JAIBasic.cpp" in 02802A2C
str 1016F3DC "(0)" in 02802A2C
str 1016F3F4 "JAIBasic.cpp" in 02802AEC
str 1016F3F0 "(0)" in 02802AEC
str 1016F408 "JAIBasic.cpp" in 02802C60
str 1016F404 "(0)" in 02802C60
ext 0273AA24 void basicSoundAssert(const char*, u32, const char*)
ext 0280A68C void storeBasicSequence(gptr<JAISound>*, JAInter::Actor*, u32, u32, u32, JAISoundInfoView*)
ext 02807300 void storeBasicSe(gptr<JAISound>*, JAInter::Actor*, u32, u32, u32, JAISoundInfoView*)
ext 0280E07C void storeBasicStream(gptr<JAISound>*, JAInter::Actor*, u32, u32, u32, JAISoundInfoView*)
ext 0280AC38 void releaseBasicSequence(JAISound*, u32)
ext 02805AFC void releaseBasicSe(JAISound*, u32)
ext 0280E080 void releaseBasicStream(JAISound*, u32)
ext 0280B110 void setBasicPrepareFlag(JAISound*, u32)
ext 0280468C u32 basicCategoryMax()
ext 0280729C u32 basicSoundCategory(u32)
data audioBasicSingleton 104B4F14
data audioCurrentHeap 101F9B54
data sSystemHeap 101F9DA8
data audioCameraMax 101F9C48
data seCategoryVolume 104B501C
data basicVtable 1016F504
ext 0280DE34 JAISoundInfoView* getBasicSoundInfo(u32)
ext 0280DF98 u32 getBasicSoundFormat(u32)
ext 0280E070 u32 canProcessAudioFrame()
ext 02807274 void processBasicSeFrame()
ext 0280A654 void processBasicSequenceFrame()
ext 0280E138 void processBasicStreamFrame()
ext 02819804 void registerBasicTrackCallback(BasicTrackCallback)
*/

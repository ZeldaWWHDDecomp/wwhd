/** HD JAudio-to-NW4F bridge: paired sound pools, filtering and stage substitutions. */
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "hd/hd_snd_0202C27C.h"
static void* sBridgeSoundManager;
static hdBridgeRoot* sBridgeRoot;
static u32 sBridgePlayerTypeInitialized;
static u32 sBridgePlayerTypeInfo;
static hdBridgeStringTable sBridgeStringTable;
static hdBridgeStringTable sBridgeFixedStringTable;
static u8 sBridgeStageName;
static u8 sBridgeDebugFormat;
static u32 sBridgeSceneFlag;

template<class T> static void constructBridgePool(hdBridgePool<T>* pool) {
    pool->mCount = 0;
    pool->mStorage = NULL;
    pool->mCapacity = 0;
    pool->mFree = NULL;
    pool->mList.mPrevious = pool->sentinel();
    pool->mList.mNext = pool->sentinel();
}
template<class T> static void makeBridgeFreeList(T* objects) {
    for (u32 i = 0; i < BRIDGE_POOL_CAPACITY - 1; ++i) objects[i].mNextFree = &objects[i + 1];
    objects[BRIDGE_POOL_CAPACITY - 1].mNextFree = NULL;
}
/* 0202C27C */
static hdSoundBridge* constructSoundBridge(hdSoundBridge* i_this) {
    if (i_this == NULL) {
        i_this = (hdSoundBridge*)operator_new(sizeof(hdSoundBridge));
        if (i_this == NULL) return NULL;
    }
    i_this->mVtable = 0x100041F8;
    i_this->mSecondaryVtable = 0x10004228;
    hdBridgePool<hdBridgeJaiSound>* jaiPool = i_this->jaiPoolForConstruction();
    if (jaiPool == NULL) jaiPool = (hdBridgePool<hdBridgeJaiSound>*)operator_new(sizeof(hdBridgePool<hdBridgeJaiSound>));
    if (jaiPool != NULL) constructBridgePool(jaiPool);
    hdBridgePool<hdBridgeHandle>* handlePool = i_this->handlePoolForConstruction();
    if (handlePool == NULL) handlePool = (hdBridgePool<hdBridgeHandle>*)operator_new(sizeof(hdBridgePool<hdBridgeHandle>));
    if (handlePool != NULL) constructBridgePool(handlePool);
    i_this->field_0x42 = 0;
    i_this->mDuckingCounter = 0;
    i_this->mSystemEnabled = 1;
    i_this->mFilterStreams = 1;
    i_this->mEnabled = 1;
    i_this->mInspectEntry = 0;
    i_this->mDebugPrint = 0;
    hdBridgeJaiSound* sounds = (hdBridgeJaiSound*)hdBridgeAllocatePool(BRIDGE_POOL_CAPACITY * sizeof(hdBridgeJaiSound), NULL, 4);
    if (sounds != NULL) {
        i_this->mJaiPool.mFree = sounds;
        makeBridgeFreeList(sounds);
        i_this->mJaiPool.mStorage = sounds;
        i_this->mJaiPool.mCapacity = BRIDGE_POOL_CAPACITY;
    }
    hdBridgeHandle* handles = (hdBridgeHandle*)hdBridgeAllocatePool(BRIDGE_POOL_CAPACITY * sizeof(hdBridgeHandle), NULL, 4);
    if (handles == NULL) return i_this;
    i_this->mHandlePool.mFree = handles;
    makeBridgeFreeList(handles);
    i_this->mHandlePool.mStorage = handles;
    i_this->mHandlePool.mCapacity = BRIDGE_POOL_CAPACITY;
    return i_this;
}
static void clearBridgeJaiPool(hdBridgePool<hdBridgeJaiSound>* pool) {
    for (hdBridgeNode* node = pool->mList.mNext; node != pool->sentinel();) {
        hdBridgeNode* next = node->mNext;
        hdBridgeEraseNode(node);
        pool->mCount = (u32)pool->mCount - 1;
        hdBridgeJaiSound* sound = hdBridgeJaiFromNode(node);
        sound->destroy(2);
        sound->mNextFree = pool->mFree;
        node = next;
        pool->mFree = sound;
    }
    hdBridgeFreePool(pool->mStorage);
    pool->mFree = NULL;
    pool->mCapacity = 0;
    pool->mStorage = NULL;
}
static void clearBridgeHandlePool(hdBridgePool<hdBridgeHandle>* pool) {
    for (hdBridgeNode* node = pool->mList.mNext; node != pool->sentinel();) {
        hdBridgeNode* next = node->mNext;
        hdBridgeEraseNode(node);
        pool->mCount = (u32)pool->mCount - 1;
        hdBridgeHandle* handle = hdBridgeHandleFromNode(node);
        handle->destroy(2);
        handle->mNextFree = pool->mFree;
        node = next;
        pool->mFree = handle;
    }
    hdBridgeFreePool(pool->mStorage);
    pool->mFree = NULL;
    pool->mCapacity = 0;
    pool->mStorage = NULL;
}
/* 0202C3F8 */
static void destroySoundBridge(hdSoundBridge* i_this, s32 flags) {
    if (i_this == NULL) return;
    hdBridgeJaiSound* storage = i_this->mJaiPool.mStorage;
    i_this->mVtable = 0x100041F8;
    i_this->mSecondaryVtable = 0x10004228;
    if (storage != NULL) clearBridgeJaiPool(i_this->jaiPoolForConstruction());
    if (i_this->mHandlePool.mStorage != NULL) clearBridgeHandlePool(i_this->handlePoolForConstruction());
    if (flags & 1) operator_delete(i_this);
}
/* 0202C54C */
static void* bridgeArchive() {
    void* manager = sBridgeSoundManager;
    if (manager == NULL) return NULL;
    return hdBridgeManagerArchive(manager);
}
/* 0202C57C */
static void* bridgeNwArchive() {
    hdBridgeRoot* root = sBridgeRoot;
    if (root == NULL) return NULL;
    hdBridgePlayer* player = root->mPlayer;
    if (player == NULL) return NULL;
    if (sBridgePlayerTypeInitialized == 0) {
        sBridgePlayerTypeInitialized = 1;
        sBridgePlayerTypeInfo = 0x10004178;
    }
    if (gabi::call_ptr<u32>(player->mVtable->mIsRuntimeType.get(), player, &sBridgePlayerTypeInfo) == 0) player = NULL;
    void* archive = hdBridgeReadPlayerArchive(player);
    if (archive == NULL) return NULL;
    return hdBridgeGetArchive(archive);
}
static s32 bridgeStringLength(be<u8>* text) {
    s32 length = 0;
    if (*text == 0) return 0;
    for (;;) {
        ++length;
        text = hdBridgeAdvance(text, 1);
        if (length > 0x40000) return 0;
        if (*text == 0) return length;
    }
}
static bool bridgeStringsEqual(be<u8>* first, be<u8>* second) {
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 a = *hdBridgeAdvance(first, i);
        u8 b = *hdBridgeAdvance(second, i);
        if (a != b) return false;
        if (a == 0) return true;
    }
    return false;
}
static void initializeBridgeBuffer(hdBridgeFixedString* buffer) {
    buffer->mText = buffer->mBuffer;
    buffer->mBuffer[0] = 0;
    buffer->mBuffer[63] = 0;
}
static void copyBridgeString(hdBridgeFixedString* buffer, hdBridgeString* source) {
    be<u8>* chars = buffer->mBuffer;
    s32 length = bridgeStringLength(source->mText);
    s32 capacity = buffer->mCapacity;
    if (!(length < capacity)) length = capacity - 1;
    source->assureTermination();
    hdBridgeCopyBytes(chars, source->mText, length, 0);
    *hdBridgeAdvance(chars, length) = 0;
}
/* 0202C630 */
static u32 bridgeNwId(hdSoundBridge* i_this, u32 id) {
    void* archive = hdBridgeArchive();
    if (archive == NULL) return 0;
    be<u8>* entry = hdBridgeLookupName(archive, id);
    if (entry == NULL) return 0;
    hdBridgeString source;
    hdBridgeFixedString buffer;
    initializeBridgeBuffer(&buffer);
    source.mText = entry;
    buffer.mCapacity = 64;
    buffer.mVtable = &sBridgeFixedStringTable;
    source.mVtable = &sBridgeStringTable;
    hdBridgeStringNoTermination(&source);
    copyBridgeString(&buffer, &source);
    buffer.assureTermination();
    be<u8>* text = buffer.mText;
    text[0] = 'N';
    text[1] = 'W';
    void* nwArchive = hdBridgeNwArchive();
    if (nwArchive == NULL) return 0;
    buffer.assureTermination();
    s32 index = hdBridgeArchiveNameIndex(nwArchive, buffer.mText);
    if (index == -1) return 0;
    return hdBridgeArchiveId(nwArchive, index);
}
/* 0202C7C4 */
static be<u8>* bridgeEntry(hdSoundBridge* i_this, u32 id) {
    void* archive = hdBridgeArchive();
    if (archive == NULL) return NULL;
    be<u8>* entry = hdBridgeLookupName(archive, id);
    if (entry == NULL) return NULL;
    if (i_this->mFilterStreams != 0 && hdBridgeIsStream(archive, id) != 0) return NULL;
    return entry;
}
/* 0202C870 */
static u32 bridgeHandled(hdSoundBridge* i_this, gptr<hdBridgeJaiSound>* output) {
    if (output != NULL) *output = NULL;
    return 1;
}
/* 0202C888 */
static u32 bridgeIsSystem(hdSoundBridge* i_this, u32 id) {
    u32 kind = id & BRIDGE_SOUND_KIND_MASK;
    if (kind == BRIDGE_SOUND_SYSTEM_HIGH) return id != BRIDGE_SYSTEM_EXEMPT_HIGH ? 1 : 0;
    if (kind == BRIDGE_SOUND_SYSTEM_LOW) return id != BRIDGE_SYSTEM_EXEMPT_LOW ? 1 : 0;
    return 0;
}
/* 0202C8D4 */
static u32 bridgeGameOver(hdSoundBridge* i_this) {
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    if (player == NULL) return 0;
    return hdBridgeGameOverStarting(player) != 0 ? 1 : 0;
}
/* 0202C924 */
static u32 bridgeIsSe(hdSoundBridge* i_this, u32 id) { return (id & BRIDGE_SOUND_KIND_MASK) == 0 ? 1 : 0; }
/* 0202C934 */
static u32 bridgeIsSeBankZero(hdSoundBridge* i_this, u32 id) {
    if (id & BRIDGE_SOUND_KIND_MASK) return 0;
    if (id & BRIDGE_SOUND_BANK_MASK) return 0;
    return 1;
}
/* 0202C954 */
static u32 bridgeIsLinkVoice(hdSoundBridge* i_this, u32 id) {
    if (id & BRIDGE_SOUND_KIND_MASK) return 0;
    void* archive = hdBridgeArchive();
    if (archive == NULL) return 0;
    be<u8>* entry = hdBridgeLookupName(archive, id);
    hdBridgeString candidate;
    hdBridgeString name;
    hdBridgeFixedString buffer;
    name.mText = entry;
    initializeBridgeBuffer(&buffer);
    name.mVtable = &sBridgeStringTable;
    buffer.mCapacity = 64;
    buffer.mVtable = &sBridgeFixedStringTable;
    hdBridgeStringNoTermination(&name);
    copyBridgeString(&buffer, &name);
    name.mVtable = &sBridgeStringTable;
    name.setText("JA_SE_LK");
    buffer.assureTermination();
    s32 length = bridgeStringLength(buffer.mText);
    name.assureTermination();
    s32 patternLength = bridgeStringLength(name.mText);
    s32 last = length - patternLength;
    if (last < 0) return 0;
    for (s32 i = 0; i <= last; ++i) {
        candidate.mVtable = &sBridgeStringTable;
        candidate.mText = hdBridgeAdvance(buffer.mText, i);
        hdBridgeStringNoTermination(&candidate);
        candidate.assureTermination();
        be<u8>* text = candidate.mText;
        name.assureTermination();
        be<u8>* pattern = name.mText;
        if (text == pattern || patternLength <= 0) return 1;
        bool match = true;
        for (s32 j = 0; j < patternLength; ++j) {
            u8 a = *hdBridgeAdvance(text, j);
            u8 b = *hdBridgeAdvance(pattern, j);
            if (a == 0) { if (b != 0) match = false; break; }
            if (b == 0 || b != a) { match = false; break; }
        }
        if (match) return 1;
    }
    return 0;
}
/* 0202CBF8 */
static u32 bridgeIsPlaying(hdSoundBridge* i_this, u32 id) {
    if (i_this->mHandlePool.mCount == 0) return 0;
    for (hdBridgeNode* node = i_this->mHandlePool.mList.mNext; node != i_this->mHandlePool.sentinel(); node = node->mNext) {
        hdBridgeHandle* handle = hdBridgeHandleFromNode(node);
        if (handle->getId() == id) return 1;
    }
    return 0;
}
/* 0202CCA0 */
static u32 filterBridgeSound(hdSoundBridge* i_this, u32 id, gptr<hdBridgeJaiSound>* output) {
    bool filtered = false;
    if (i_this->mEnabled == 0) {
        filtered = true;
    } else {
        if (i_this->mSystemEnabled == 0 && hdBridgeSystemSound(i_this, id) != 0) filtered = true;
        if (!filtered && hdBridgeGameOver(i_this) != 0 && hdBridgeIsSe(i_this, id) != 0 &&
            hdBridgeIsSeBankZero(i_this, id) == 0 && hdBridgeIsLinkVoice(i_this, id) == 0) filtered = true;
        if (!filtered) {
            bool demo = hdBridgeDemoStateFirst() != 0 || hdBridgeDemoStateSecond() != 0;
            // HD: this exact set of effect IDs is suppressed during these demos.
            if (demo && id >= 0x804 && (id <= 0x805 || (id >= 0x81C && (id <= 0x81D || id == 0x8F8 || id == 0x90A)))) filtered = true;
        }
        if (!filtered) {
            hdBridgeString first;
            hdBridgeString second;
            first.mVtable = &sBridgeStringTable;
            second.mVtable = &sBridgeStringTable;
            first.setText("Demo10");
            second.mText = (be<u8>*)&sBridgeStageName;
            hdBridgeStringNoTermination(&first);
            first.assureTermination();
            hdBridgeStringTable* secondTable = second.mVtable;
            be<u8>* a = first.mText;
            gabi::call_ptr(secondTable->mAssureTermination.get(), &second);
            be<u8>* b = second.mText;
            bool equal = a == b || bridgeStringsEqual(a, b);
            if (equal && sBridgeSceneFlag == 1 && id == BRIDGE_SOUND_DEMO10_FILTERED) {
                filtered = true;
            } else if (id == BRIDGE_SOUND_SINGLE_INSTANCE) {
                if (hdBridgeIsPlaying(i_this, BRIDGE_SOUND_SINGLE_INSTANCE) == 0) return 0;
                filtered = true;
            } else if (id == BRIDGE_SOUND_DUCKED_BGM) {
                if (i_this->mDuckingCounter <= 0) return 0;
                filtered = true;
            } else {
                return 0;
            }
        }
    }
    return hdBridgeHandled(i_this, output);
}
/* 0202CED4 */
static hdBridgeHandle* findBridgeHandle(hdSoundBridge* i_this, gptr<hdBridgeJaiSound>* slot) {
    if (i_this->mHandlePool.mCount == 0) return NULL;
    hdBridgeNode* node = i_this->mHandlePool.mList.mNext;
    hdBridgeHandle* handle = hdBridgeHandleFromNode(node);
    // HD: prefetch next even for an empty list, matching the original load order.
    hdBridgeNode* next = node->mNext;
    while (node != i_this->mHandlePool.sentinel()) {
        if (handle->mSlot == slot) return handle;
        node = next;
        handle = hdBridgeHandleFromNode(node);
        next = next->mNext;
    }
    return NULL;
}
/* 0202CF1C */
static void stopBridgeSlot(hdSoundBridge* i_this, gptr<hdBridgeJaiSound>* slot) {
    if (slot == NULL) return;
    if (*slot != NULL) {
        u32 id = hdBridgeJaiId(*slot);
        void* archive = hdBridgeArchive();
        if (archive != NULL && hdBridgeIsStream(archive, id) == 0) {
            hdBridgeStopJai(*slot, 0);
            *slot = NULL;
        }
    }
    hdBridgeHandle* handle = hdBridgeFindHandle(i_this, slot);
    if (handle == NULL) return;
    handle->stop(0);
    handle->mSlot = NULL;
    *slot = NULL;
}
static void releaseBridgeJai(hdSoundBridge* i_this, hdBridgeJaiSound* sound) {
    hdBridgeEraseNode(sound->node());
    i_this->mJaiPool.mCount = (u32)i_this->mJaiPool.mCount - 1;
    sound->destroy(2);
    sound->mNextFree = i_this->mJaiPool.mFree;
    i_this->mJaiPool.mFree = sound;
}
/* 0202CFDC */
static u32 allocateBridgePair(hdSoundBridge* i_this, hdBridgeJaiOutput* jaiOutput, hdBridgeHandleOutput* handleOutput) {
    if (!((s32)i_this->mJaiPool.mCount < (s32)i_this->mJaiPool.mCapacity)) {
        jaiOutput->mObject = NULL;
        return 0;
    }
    hdBridgeJaiSound* sound = i_this->mJaiPool.mFree;
    if (sound != NULL) i_this->mJaiPool.mFree = sound->mNextFree;
    if (sound != NULL) {
        hdBridgeConstructJai(sound);
        sound->mNode.mNext = NULL;
        sound->mNode.mPrevious = NULL;
    }
    hdBridgeAppendNode(i_this->mJaiPool.sentinel(), sound->node());
    i_this->mJaiPool.mCount = (u32)i_this->mJaiPool.mCount + 1;
    jaiOutput->mObject = sound;
    if (sound == NULL) return 0;
    if (!((s32)i_this->mHandlePool.mCount < (s32)i_this->mHandlePool.mCapacity)) {
        handleOutput->mObject = NULL;
        releaseBridgeJai(i_this, jaiOutput->mObject);
        return 0;
    }
    hdBridgeHandle* handle = i_this->mHandlePool.mFree;
    if (handle != NULL) i_this->mHandlePool.mFree = handle->mNextFree;
    if (handle != NULL) {
        hdBridgeConstructHandle(handle);
        handle->mNode.mNext = NULL;
        handle->mNode.mPrevious = NULL;
    }
    hdBridgeAppendNode(i_this->mHandlePool.sentinel(), handle->node());
    i_this->mHandlePool.mCount = (u32)i_this->mHandlePool.mCount + 1;
    handleOutput->mObject = handle;
    sound = jaiOutput->mObject;
    if (handle == NULL) {
        releaseBridgeJai(i_this, sound);
        return 0;
    }
    sound->mHandle = handle;
    handleOutput->mObject->mJaiSound = jaiOutput->mObject;
    return 1;
}
/* 0202D1CC */
static void freeBridgePair(hdSoundBridge* i_this, hdBridgeJaiSound* sound, hdBridgeHandle* handle) {
    handle->mJaiSound = NULL;
    hdBridgeEraseNode(handle->node());
    i_this->mHandlePool.mCount = (u32)i_this->mHandlePool.mCount - 1;
    handle->destroy(2);
    handle->mNextFree = i_this->mHandlePool.mFree;
    i_this->mHandlePool.mFree = handle;
    releaseBridgeJai(i_this, sound);
}
/* 0202D284 */
static void setBridgeSoundId(hdSoundBridge* i_this, hdBridgeJaiSound* jaiSound, hdBridgeHandle* handle, u32 id) {
    hdBridgeSound* sound = handle->mSound;
    handle->mId = id;
    hdBridgeSoundInfo* info = sound == NULL ? NULL : sound->mInfo.get();
    // HD: missing sound info still writes through guest NULL, as the original does.
    info->mId = id;
    info->mJaiSound = jaiSound;
}
/* 0202D2B4 */
static hdBridgePlayer* bridgePlayer() {
    return (hdBridgePlayer*)hdBridgeManagerPlayer(sBridgeSoundManager);
}
enum hdBridgeStartKind { BRIDGE_START_SE, BRIDGE_START_BGM };
/* inlined into startBridgeSe, startBridgeBgm */
static u32 startBridgeSound(hdSoundBridge* i_this, u32 id, gptr<hdBridgeJaiSound>* slot, hdBridgeStartKind kind) {
    if (i_this->mInspectEntry != 0) hdBridgeEntry(i_this, id);
    hdBridgeStopSlot(i_this, slot);
    u32 nwId = hdBridgeNwId(i_this, id);
    if (nwId == 0) {
        hdBridgePlayer* player = hdBridgePlayerInstance();
        if (player->mSuppressMissing != 0) return 0;
        return hdBridgeHandled(i_this, slot);
    }
    hdBridgeHandleOutput handleOutput;
    hdBridgeJaiOutput jaiOutput;
    jaiOutput.mObject = NULL;
    handleOutput.mObject = NULL;
    if (hdBridgeAllocate(i_this, &jaiOutput, &handleOutput) == 0) {
        if (slot != NULL) *slot = NULL;
        return 1;
    }
    hdBridgePlayer* player = hdBridgeGetNwPlayer();
    hdBridgeHandle* handle = handleOutput.mObject;
    u32 result;
    if (kind == BRIDGE_START_SE) result = hdBridgeStartSoundEffect(player->archivePlayer(), handle->soundHandle(), nwId, 0);
    else result = hdBridgeStartBackgroundMusic(player->archivePlayer(), handle->soundHandle(), nwId, 0);
    hdBridgeJaiSound* sound = jaiOutput.mObject;
    handle = handleOutput.mObject;
    if (result != 0) {
        hdBridgeFree(i_this, sound, handle);
        if (slot != NULL) *slot = NULL;
        return 1;
    }
    hdBridgeSetId(i_this, sound, handle, id);
    if (slot != NULL) {
        handleOutput.mObject->mSlot = slot;
        *slot = jaiOutput.mObject;
    }
    return 1;
}
/* 0202D2C0 */
static u32 startBridgeSe(hdSoundBridge* i_this, u32 id, gptr<hdBridgeJaiSound>* slot) {
    if (hdBridgeFilter(i_this, id, slot) != 0) return 1;
    return startBridgeSound(i_this, id, slot, BRIDGE_START_SE);
}
/* 0202D428 */
static void bridgeForId(hdSoundBridge* i_this, u32 id, u32 frames) {
    if (i_this->mHandlePool.mCount == 0) return;
    for (hdBridgeNode* node = i_this->mHandlePool.mList.mNext; node != i_this->mHandlePool.sentinel(); node = node->mNext) {
        hdBridgeHandle* handle = hdBridgeHandleFromNode(node);
        if (handle->getId() == id) handle->stop(frames);
    }
}
/* 0202D4D0 */
static u32 startBridgeBgm(hdSoundBridge* i_this, u32 id, gptr<hdBridgeJaiSound>* slot) {
    if (hdBridgeFilter(i_this, id, slot) != 0) return 1;
    if (id == BRIDGE_SOUND_DUCKED_BGM) {
        hdBridgeForId(i_this, BRIDGE_SOUND_DUCKED_BGM, 5);
        i_this->mDuckingCounter = 2;
    }
    return startBridgeSound(i_this, id, slot, BRIDGE_START_BGM);
}
/* 0202D658 */
static void updateSoundBridge(hdSoundBridge* i_this) {
    if (i_this->mHandlePool.mCount != 0) {
        hdBridgeNode* node = i_this->mHandlePool.mList.mNext;
        hdBridgeHandle* handle = hdBridgeHandleFromNode(node);
        hdBridgeNode* next = node->mNext;
        while (node != i_this->mHandlePool.sentinel()) {
            if (handle->isPlaying() == 0) {
                gptr<hdBridgeJaiSound>* slot = handle->mSlot;
                hdBridgeJaiSound* sound = handle->mJaiSound;
                handle->mId = 0xFFFFFFFF;
                handle->mJaiSound = NULL;
                if (slot != NULL) { *slot = NULL; handle->mSlot = NULL; }
                releaseBridgeJai(i_this, sound);
                hdBridgeEraseNode(handle->node());
                i_this->mHandlePool.mCount = (u32)i_this->mHandlePool.mCount - 1;
                handle->destroy(2);
                handle->mNextFree = i_this->mHandlePool.mFree;
                i_this->mHandlePool.mFree = handle;
            } else if (i_this->mDebugPrint != 0) {
                hdBridgeSound* sound = handle->mSound;
                if (sound != NULL) hdBridgePrint(0x1C2, 0x190, (const char*)&sBridgeDebugFormat, sound->mStarted);
                else hdBridgePrint(0x1C2, 0x190, (const char*)&sBridgeDebugFormat, -1);
            }
            node = next;
            handle = hdBridgeHandleFromNode(node);
            next = next->mNext;
        }
    }
    s32 counter = i_this->mDuckingCounter;
    i_this->mDuckingCounter = counter > 0 ? counter - 1 : 0;
}
/* 0202D7F8 */
static u32 remapBridgeSound(hdSoundBridge* i_this, u32 id) {
    hdBridgeString first;
    hdBridgeString third;
    hdBridgeString second;
    hdBridgeString fourth;
    first.mVtable = &sBridgeStringTable;
    first.setText("Demo23");
    second.mVtable = &sBridgeStringTable;
    second.mText = (be<u8>*)&sBridgeStageName;
    hdBridgeStringNoTermination(&first);
    first.assureTermination();
    hdBridgeStringTable* secondTable = second.mVtable;
    be<u8>* a = first.mText;
    gabi::call_ptr(secondTable->mAssureTermination.get(), &second);
    be<u8>* b = second.mText;
    if ((a == b || bridgeStringsEqual(a, b)) && id == BRIDGE_SOUND_DEMO23_ORIGINAL) return BRIDGE_SOUND_DEMO23_REPLACEMENT;
    third.mVtable = &sBridgeStringTable;
    third.setText("Demo45");
    fourth.mText = (be<u8>*)&sBridgeStageName;
    fourth.mVtable = &sBridgeStringTable;
    hdBridgeStringNoTermination(&third);
    third.assureTermination();
    hdBridgeStringTable* fourthTable = fourth.mVtable;
    a = third.mText;
    gabi::call_ptr(fourthTable->mAssureTermination.get(), &fourth);
    b = fourth.mText;
    if ((a == b || bridgeStringsEqual(a, b)) && sBridgeSceneFlag == 1 && id == BRIDGE_SOUND_DEMO45_ORIGINAL) return BRIDGE_SOUND_DEMO45_REPLACEMENT;
    return id;
}
// HD: compiler-generated and template companions of this translation unit.
/* 0202D9D0 */
static void __sinit_hd_sound_bridge_cpp() {
    sinit_header_statics_rodata(0x101FFDB8, 0x1018EC90, 0x101FFDC4, 0x100041F0);
}
/* 0202DA64 */
static void destroyBridgeCompanionFirst(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 0202DA78 */
static void assureBridgeStringNone() {}
/* 0202DA7C */
static void terminateBridgeString(hdBridgeFixedString* i_this) { i_this->terminate(); }
/* 0202DA94 */
static void destroyBridgeCompanionSecond(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 0202DAA8 */
static void destroyBridgeCompanionThird(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* 0202DABC */
static void destroyBridgeCompanionFourth(void* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
/* WWHD glue (tools/verify/autoglue.py): HD sound bridge dependencies
 data sBridgeSoundManager 1018EC64
 data sBridgeRoot 101F8BA8
 data sBridgePlayerTypeInitialized 101FD634
 data sBridgePlayerTypeInfo 101FD950
 data sBridgeStringTable 10004118
 data sBridgeFixedStringTable 100041A0
 data sBridgeStageName 1047E6B8
 data sBridgeDebugFormat 100041CC
 data sBridgeSceneFlag 101D6010
 str 100041B8 "JA_SE_LK"
 str 100041C4 "Demo10"
 str 100041E0 "Demo23"
 str 100041E8 "Demo45"
 ext 0273B0D4 void* hdBridgeAllocatePool(u32, void*, u32)
 ext 0273AFC8 void hdBridgeFreePool(void*)
 ext 0273B310 void hdBridgeEraseNode(hdBridgeNode*)
 ext 0273B2F0 void hdBridgeAppendNode(hdBridgeNode*, hdBridgeNode*)
 ext 0202BF90 void* hdBridgeManagerArchive(void*)
 ext 02764774 void* hdBridgeGetArchive(void*)
 ext 0202B87C be<u8>* hdBridgeLookupName(void*, u32)
 ext 0202B8EC u32 hdBridgeIsStream(void*, u32)
 ext 0202DA78 void hdBridgeStringNoTermination(hdBridgeString*)
 ext C0009988 void hdBridgeCopyBytes(void*, const void*, s32, u32)
 ext 0202C54C void* hdBridgeArchive()
 ext 0202C57C void* hdBridgeNwArchive()
 ext 0288E91C s32 hdBridgeArchiveNameIndex(void*, be<u8>*)
 ext 0288E914 u32 hdBridgeArchiveId(void*, s32)
 ext 02443664 u32 hdBridgeGameOverStarting(void*)
 ext 0202C888 u32 hdBridgeSystemSound(hdSoundBridge*, u32)
 ext 0202C8D4 u32 hdBridgeGameOver(hdSoundBridge*)
 ext 0202C924 u32 hdBridgeIsSe(hdSoundBridge*, u32)
 ext 0202C934 u32 hdBridgeIsSeBankZero(hdSoundBridge*, u32)
 ext 0202C954 u32 hdBridgeIsLinkVoice(hdSoundBridge*, u32)
 ext 02030CAC u32 hdBridgeDemoStateFirst()
 ext 02030D20 u32 hdBridgeDemoStateSecond()
 ext 0202CBF8 u32 hdBridgeIsPlaying(hdSoundBridge*, u32)
 ext 0202C870 u32 hdBridgeHandled(hdSoundBridge*, gptr<hdBridgeJaiSound>*)
 ext 0202CED4 hdBridgeHandle* hdBridgeFindHandle(hdSoundBridge*, gptr<hdBridgeJaiSound>*)
 ext 0280D2D8 u32 hdBridgeJaiId(hdBridgeJaiSound*)
 ext 0280B1D8 void hdBridgeStopJai(hdBridgeJaiSound*, u32)
 ext 0280AF40 void hdBridgeConstructJai(hdBridgeJaiSound*)
 ext 0202C0D0 void hdBridgeConstructHandle(hdBridgeHandle*)
 ext 0202BF98 void* hdBridgeManagerPlayer(void*)
 ext 0202CF1C void hdBridgeStopSlot(hdSoundBridge*, gptr<hdBridgeJaiSound>*)
 ext 0202C630 u32 hdBridgeNwId(hdSoundBridge*, u32)
 ext 0202D2B4 hdBridgePlayer* hdBridgePlayerInstance()
 ext 0202C7C4 u32 hdBridgeEntry(hdSoundBridge*, u32)
 ext 0202CFDC u32 hdBridgeAllocate(hdSoundBridge*, hdBridgeJaiOutput*, hdBridgeHandleOutput*)
 ext 0202DAD0 hdBridgePlayer* hdBridgeGetNwPlayer()
 ext 02897F48 u32 hdBridgeStartSoundEffect(void*, gptr<hdBridgeSound>*, u32, u32)
 ext 02897D08 u32 hdBridgeStartBackgroundMusic(void*, gptr<hdBridgeSound>*, u32, u32)
 ext 0202D1CC void hdBridgeFree(hdSoundBridge*, hdBridgeJaiSound*, hdBridgeHandle*)
 ext 0202D284 void hdBridgeSetId(hdSoundBridge*, hdBridgeJaiSound*, hdBridgeHandle*, u32)
 ext 0202CCA0 u32 hdBridgeFilter(hdSoundBridge*, u32, gptr<hdBridgeJaiSound>*)
 ext 0202D428 void hdBridgeForId(hdSoundBridge*, u32, u32)
 ext 027EC9E8 void hdBridgePrint(u32, u32, const char*, s32)
*/

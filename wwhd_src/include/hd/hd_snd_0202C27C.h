/** HD JAudio/NW4F bridge views; no GameCube counterpart. Private decomp source. */
#pragma once
#include "wwhd.h"
struct hdBridgeNode { gptr<hdBridgeNode> mPrevious; gptr<hdBridgeNode> mNext; };
struct hdBridgeJaiSound;
struct hdBridgeHandle;
struct hdBridgeJaiTable { u8 _opaque00[0xC]; gfn mDestroy; };
struct hdBridgeHandleTable {
    u8 _opaque00[0xC]; gfn mDestroy; u8 _opaque10[4]; gfn mStop;
    u8 _opaque18[0x14]; gfn mIsPlaying; u8 _opaque30[4]; gfn mGetId;
};
struct hdBridgeSoundInfo { be<u32> mId; gptr<hdBridgeJaiSound> mJaiSound; };
struct hdBridgeSound {
    u8 _opaque00[0x100]; be<u32> mStarted;
    u8 _opaque104[0x164]; gptr<hdBridgeSoundInfo> mInfo;
};
struct hdBridgeJaiSound {
    gptr<hdBridgeJaiSound> mNextFree; u8 _opaque04[0x3C];
    gptr<hdBridgeHandle> mHandle; gptr<hdBridgeJaiTable> mVtable; hdBridgeNode mNode;
    hdBridgeNode* node() {
        // HD: NULL pooled objects still pass their wrapped embedded-node address.
        return gabi::at<hdBridgeNode>(gabi::ea(this) + offsetof(hdBridgeJaiSound, mNode));
    }
    void destroy(u32 flags) { gabi::call_ptr(mVtable->mDestroy.get(), this, flags); }
};
struct hdBridgeHandle {
    union { gptr<hdBridgeHandleTable> mVtable; gptr<hdBridgeHandle> mNextFree; };
    gptr<hdBridgeSound> mSound; gptr<hdBridgeJaiSound> mJaiSound;
    be<u32> mId; gptr<gptr<hdBridgeJaiSound>> mSlot; hdBridgeNode mNode;
    hdBridgeNode* node() {
        return gabi::at<hdBridgeNode>(gabi::ea(this) + offsetof(hdBridgeHandle, mNode));
    }
    gptr<hdBridgeSound>* soundHandle() {
        return gabi::at<gptr<hdBridgeSound>>(gabi::ea(this) + offsetof(hdBridgeHandle, mSound));
    }
    void destroy(u32 flags) { gabi::call_ptr(mVtable->mDestroy.get(), this, flags); }
    void stop(u32 frames) { gabi::call_ptr(mVtable->mStop.get(), this, frames); }
    u32 isPlaying() { return gabi::call_ptr<u32>(mVtable->mIsPlaying.get(), this); }
    u32 getId() { return gabi::call_ptr<u32>(mVtable->mGetId.get(), this); }
};
template<class T> struct hdBridgePool {
    hdBridgeNode mList; be<u32> mCount; gptr<T> mFree; gptr<T> mStorage; be<u32> mCapacity;
    hdBridgeNode* sentinel() {
        // HD: normalize an embedded list address that wrapped past the guest limit.
        return gabi::at<hdBridgeNode>(gabi::ea(this));
    }
};
struct hdSoundBridge {
    be<u32> mVtable; be<u32> mSecondaryVtable;
    hdBridgePool<hdBridgeJaiSound> mJaiPool; hdBridgePool<hdBridgeHandle> mHandlePool;
    be<u8> mEnabled; be<u8> mSystemEnabled; u8 _padding3A[2]; be<s32> mDuckingCounter;
    be<u8> mInspectEntry; be<u8> mFilterStreams;
    be<u8> field_0x42; // HD: initialized but not otherwise read by this TU.
    be<u8> mDebugPrint;
    hdBridgePool<hdBridgeJaiSound>* jaiPoolForConstruction() {
        // HD: the embedded pool constructors may receive wrapped NULL addresses.
        return gabi::at<hdBridgePool<hdBridgeJaiSound>>(gabi::ea(this) + offsetof(hdSoundBridge, mJaiPool));
    }
    hdBridgePool<hdBridgeHandle>* handlePoolForConstruction() {
        return gabi::at<hdBridgePool<hdBridgeHandle>>(gabi::ea(this) + offsetof(hdSoundBridge, mHandlePool));
    }
};
inline hdBridgeJaiSound* hdBridgeJaiFromNode(hdBridgeNode* node) {
    // HD: intrusive container addresses wrap in the 32-bit guest space.
    return gabi::at<hdBridgeJaiSound>(gabi::ea(node) - offsetof(hdBridgeJaiSound, mNode));
}
inline hdBridgeHandle* hdBridgeHandleFromNode(hdBridgeNode* node) {
    return gabi::at<hdBridgeHandle>(gabi::ea(node) - offsetof(hdBridgeHandle, mNode));
}
struct hdBridgeStringTable { u8 _opaque00[0x14]; gfn mAssureTermination; };
struct hdBridgeString {
    gptr<be<u8>> mText; gptr<hdBridgeStringTable> mVtable;
    void setText(const char* text) {
        // HD: a literal stored in a guest pointer needs its registered guest copy.
        mText = gabi::at<be<u8>>(gabi::guest_addr(text));
    }
    void assureTermination() { gabi::call_ptr(mVtable->mAssureTermination.get(), this); }
};
struct hdBridgeFixedString {
    gptr<be<u8>> mText; gptr<hdBridgeStringTable> mVtable; be<s32> mCapacity; be<u8> mBuffer[64];
    void assureTermination() { gabi::call_ptr(mVtable->mAssureTermination.get(), this); }
    void terminate() {
        // HD: retain the wrapped final-byte write even for invalid capacities.
        *gabi::at<be<u8>>(gabi::ea(mText.get()) + (u32)(s32)mCapacity - 1) = 0;
    }
};
inline be<u8>* hdBridgeAdvance(be<u8>* text, u32 count) {
    // HD: bounded string scans still use wrapping guest byte addresses.
    return gabi::at<be<u8>>(gabi::ea(text) + count);
}
struct hdBridgePlayerTable { u8 _opaque00[0xC]; gfn mIsRuntimeType; };
struct hdBridgePlayer {
    gptr<hdBridgePlayerTable> mVtable; u8 _opaque04[0xC]; be<u8> mSuppressMissing;
    u8 _opaque11[0x133]; gptr<void> mArchive;
    void* archivePlayer() { return gabi::at<void>(gabi::ea(this) + 4); }
};
struct hdBridgeRoot { u8 _opaque00[0x1C]; gptr<hdBridgePlayer> mPlayer; };
inline void* hdBridgeReadPlayerArchive(hdBridgePlayer* player) {
    // HD: a failed runtime type check still reads through guest NULL at this offset.
    return gabi::at<gptr<void>>(gabi::ea(player) + offsetof(hdBridgePlayer, mArchive))->get();
}
struct hdBridgeJaiOutput { gptr<hdBridgeJaiSound> mObject; };
struct hdBridgeHandleOutput { gptr<hdBridgeHandle> mObject; };
enum : u32 {
    BRIDGE_POOL_CAPACITY = 64, BRIDGE_SOUND_KIND_MASK = 0xC0000000,
    BRIDGE_SOUND_SYSTEM_HIGH = 0xC0000000, BRIDGE_SOUND_SYSTEM_LOW = 0x80000000,
    BRIDGE_SOUND_BANK_MASK = 0x000FF000,
    BRIDGE_SYSTEM_EXEMPT_HIGH = 0xC000004A, BRIDGE_SYSTEM_EXEMPT_LOW = 0x80000800,
    BRIDGE_SOUND_SINGLE_INSTANCE = 0x806, BRIDGE_SOUND_DUCKED_BGM = 0x826,
    BRIDGE_SOUND_DEMO10_FILTERED = 0x380A,
    BRIDGE_SOUND_DEMO23_ORIGINAL = 0x5800, BRIDGE_SOUND_DEMO23_REPLACEMENT = 0x586E,
    BRIDGE_SOUND_DEMO45_ORIGINAL = 0x588D, BRIDGE_SOUND_DEMO45_REPLACEMENT = 0x59A3,
};
void* hdBridgeAllocatePool(u32, void*, u32);
void hdBridgeFreePool(void*);
void hdBridgeEraseNode(hdBridgeNode*);
void hdBridgeAppendNode(hdBridgeNode*, hdBridgeNode*);
void* hdBridgeManagerArchive(void*);
void* hdBridgeGetArchive(void*);
be<u8>* hdBridgeLookupName(void*, u32);
u32 hdBridgeIsStream(void*, u32);
void hdBridgeStringNoTermination(hdBridgeString*);
void hdBridgeCopyBytes(void*, const void*, s32, u32);
void* hdBridgeArchive();
void* hdBridgeNwArchive();
s32 hdBridgeArchiveNameIndex(void*, be<u8>*);
u32 hdBridgeArchiveId(void*, s32);
u32 hdBridgeGameOverStarting(void*);
u32 hdBridgeSystemSound(hdSoundBridge*, u32);
u32 hdBridgeGameOver(hdSoundBridge*);
u32 hdBridgeIsSe(hdSoundBridge*, u32);
u32 hdBridgeIsSeBankZero(hdSoundBridge*, u32);
u32 hdBridgeIsLinkVoice(hdSoundBridge*, u32);
u32 hdBridgeDemoStateFirst();
u32 hdBridgeDemoStateSecond();
u32 hdBridgeIsPlaying(hdSoundBridge*, u32);
u32 hdBridgeHandled(hdSoundBridge*, gptr<hdBridgeJaiSound>*);
hdBridgeHandle* hdBridgeFindHandle(hdSoundBridge*, gptr<hdBridgeJaiSound>*);
u32 hdBridgeJaiId(hdBridgeJaiSound*);
void hdBridgeStopJai(hdBridgeJaiSound*, u32);
void hdBridgeConstructJai(hdBridgeJaiSound*);
void hdBridgeConstructHandle(hdBridgeHandle*);
void* hdBridgeManagerPlayer(void*);
void hdBridgeStopSlot(hdSoundBridge*, gptr<hdBridgeJaiSound>*);
u32 hdBridgeNwId(hdSoundBridge*, u32);
hdBridgePlayer* hdBridgePlayerInstance();
u32 hdBridgeEntry(hdSoundBridge*, u32);
u32 hdBridgeAllocate(hdSoundBridge*, hdBridgeJaiOutput*, hdBridgeHandleOutput*);
hdBridgePlayer* hdBridgeGetNwPlayer();
u32 hdBridgeStartSoundEffect(void*, gptr<hdBridgeSound>*, u32, u32);
u32 hdBridgeStartBackgroundMusic(void*, gptr<hdBridgeSound>*, u32, u32);
void hdBridgeFree(hdSoundBridge*, hdBridgeJaiSound*, hdBridgeHandle*);
void hdBridgeSetId(hdSoundBridge*, hdBridgeJaiSound*, hdBridgeHandle*, u32);
u32 hdBridgeFilter(hdSoundBridge*, u32, gptr<hdBridgeJaiSound>*);
void hdBridgeForId(hdSoundBridge*, u32, u32);
void hdBridgePrint(u32, u32, const char*, s32);
WWHD_SIZE(hdBridgeNode, 8);
WWHD_SIZE(hdBridgeJaiTable, 0x10);
WWHD_SIZE(hdBridgeHandleTable, 0x38);
WWHD_SIZE(hdBridgeSoundInfo, 8);
WWHD_SIZE(hdBridgeSound, 0x26C);
WWHD_SIZE(hdBridgeJaiSound, 0x50);
WWHD_SIZE(hdBridgeHandle, 0x1C);
WWHD_SIZE(hdBridgePool<hdBridgeJaiSound>, 0x18);
WWHD_SIZE(hdBridgePool<hdBridgeHandle>, 0x18);
WWHD_SIZE(hdSoundBridge, 0x44);
WWHD_OFFSET(hdSoundBridge, mHandlePool, 0x20);
WWHD_OFFSET(hdSoundBridge, mDuckingCounter, 0x3C);
WWHD_SIZE(hdBridgeStringTable, 0x18);
WWHD_SIZE(hdBridgeString, 8);
WWHD_SIZE(hdBridgeFixedString, 0x4C);
WWHD_SIZE(hdBridgePlayerTable, 0x10);
WWHD_SIZE(hdBridgePlayer, 0x148);
WWHD_SIZE(hdBridgeRoot, 0x20);
WWHD_SIZE(hdBridgeJaiOutput, 4);
WWHD_SIZE(hdBridgeHandleOutput, 4);

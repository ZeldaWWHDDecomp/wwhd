// JAudio wave archive loading; HD uses a named memory file loader and a global monitor.
#include "JSystem/JAudio/JASWaveArcLoader.h"
#include "JSystem/JAudio/JASThread.h"
using namespace JASystem;
using namespace JASystem::WaveArcLoader;
using namespace gabi;
struct ArchiveMonitor { u8 storage[0x3C]; };
struct Directory { be<u8> bytes[0x40]; };
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
using RootHeap = gptr<Kernel::THeap>;
using Loader = gptr<void>;
static ArchiveMonitor sMonitor;
static Directory sCurrentDir;
static RootHeap sRootHeap;
static Loader sLoader;
static WaveArcVTable sArcVtable;
static Initialization sInitialization;
static Registration sRegistration;
static Registration sMonitorRegistration;
extern void* allocateWaveArcObject(u32);
extern void* allocateWaveArcName(u32, void*, u32);
extern void deleteWaveArcName(void*);
extern void deleteWaveArcObject(void*);
extern void destroyWaveArcLink(JSUPtrLink*, u32);
extern void destroyWaveArcList(JSUPtrList*, u32);
extern void lockWaveArc(ArchiveMonitor*);
extern void unlockWaveArc(ArchiveMonitor*);
extern void constructWaveArcMonitor(ArchiveMonitor*);
extern void copyWaveArcString(be<u8>*, const be<u8>*);
extern u32 getWaveFileSize(void*, const be<u8>*);
extern u32 loadWaveFile(void*, const be<u8>*, u32);
extern void* getWaveTaskThread();
using LoadCallback = void (*)(TWaveArc::LoadMessage*);
extern u32 submitWaveTask(void*, LoadCallback, TWaveArc::LoadMessage*, u32);
extern void registerWaveArcGlobals(void*);
namespace JASystem {
/* 02812668 */
TWaveArc* constructWaveArc(TWaveArc* arc) {
    if (!arc) {
        arc = static_cast<TWaveArc*>(allocateWaveArcObject(sizeof(TWaveArc)));
        if (!arc)
            return nullptr;
    }
    arc->mVtable = reinterpret_cast<Kernel::DisposerVTable*>(&sArcVtable);
    Kernel::constructAudioHeap(&arc->mHeap, arc);
    arc->mFileName = nullptr;
    arc->mLoadCount = 0;
    arc->mFileSize = 0;
    arc->mLoaded = 0;
    arc->mState = 0;
    arc->mFileName = static_cast<be<u8>*>(allocateWaveArcName(0x100, JASDram, 4));
    return arc;
}
}
/* 028126F0 */
void destroyWaveArc(TWaveArc* arc, u32 flags) {
    if (!arc)
        return;
    be<u8>* name = arc->mFileName;
    arc->mVtable = reinterpret_cast<Kernel::DisposerVTable*>(&sArcVtable);
    if (name) {
        deleteWaveArcName(name);
        arc->mFileName = nullptr;
    }
    destroyWaveArcLink(&arc->mHeap.mLink, 0);
    destroyWaveArcList(&arc->mHeap.mChildren, 0);
    if (flags & 1)
        deleteWaveArcObject(arc);
}
/* 02812780 */
void TWaveArc::setFileName(const be<u8>* name) {
    copyWaveArcString(mFileName, sCurrentDir.bytes);
    be<u8>* end = mFileName;
    while (*end != 0)
        ++end;
    u8 byte;
    do {
        byte = *name++;
        *end++ = byte;
    } while (byte != 0);
    end = mFileName;
    while (*end != 0)
        ++end;
    // HD: the trailing source string is empty.
    *end = 0;
    void* loader = sLoader;
    be<u8>* file = mFileName;
    mFileSize = getWaveFileSize(loader, file);
}
/* 02812834 */
u32 TWaveArc::loadSetup(u32 loadCount) {
    lockWaveArc(&sMonitor);
    if (mLoadCount != loadCount || mState != 1) {
        unlockWaveArc(&sMonitor);
        return 0;
    }
    mLoaded = 1;
    mState = 2;
    unlockWaveArc(&sMonitor);
    return 1;
}
/* 028128D8 */
void TWaveArc::loadToAramCallback(LoadMessage* message) {
    void* loader = sLoader;
    u32 base = message->mBase;
    const be<u8>* name = message->mFileName;
    if (!loadWaveFile(loader, name, base))
        return;
    TWaveArc* arc = message->mArc;
    u32 count = message->mLoadCount;
    if (arc->loadSetup(count)) {
        auto vtable = reinterpret_cast<WaveArcVTable*>(arc->mVtable.get());
        gabi::call_ptr<void>(vtable->onLoadDone.get(), arc);
    }
}
/* 02812950 */
u32 TWaveArc::sendLoadCmd() {
    lockWaveArc(&sMonitor);
    LoadMessage message;
    message.mFileName = mFileName;
    u32 count = (u32)mLoadCount + 1;
    mState = 1;
    message.mLoadCount = count;
    mLoadCount = count;
    mLoaded = 0;
    u32 base = mHeap.mBase;
    message.mArc = this;
    message.mBase = base;
    void* thread = getWaveTaskThread();
    if (!submitWaveTask(thread, loadToAramCallback, &message, sizeof(message))) {
        mHeap.free();
        unlockWaveArc(&sMonitor);
        return 0;
    }
    unlockWaveArc(&sMonitor);
    return 1;
}
/* 02812A18 */
u32 TWaveArc::load(Kernel::THeap* heap) {
    if (!mFileSize)
        return 0;
    lockWaveArc(&sMonitor);
    if (mState != 0) {
        unlockWaveArc(&sMonitor);
        return 0;
    }
    if (!heap)
        heap = sRootHeap;
    u32 size = mFileSize;
    if (!mHeap.alloc(heap, size)) {
        unlockWaveArc(&sMonitor);
        return 0;
    }
    u32 result = sendLoadCmd();
    unlockWaveArc(&sMonitor);
    return result;
}
/* 02812AE0 */
u32 TWaveArc::erase() {
    return mHeap.free();
}
/* 02812AE8 */
u32 TWaveArc::eraseSetup() {
    lockWaveArc(&sMonitor);
    u32 state = mState;
    if (state != 0)
        mState = 0;
    if (state == 0 || state == 1) {
        unlockWaveArc(&sMonitor);
        return 0;
    }
    mLoaded = 0;
    unlockWaveArc(&sMonitor);
    return 1;
}
/* 02812B74 */
void TWaveArc::onDispose() {
    if (eraseSetup()) {
        auto vtable = reinterpret_cast<WaveArcVTable*>(mVtable.get());
        gabi::call_ptr<void>(vtable->onEraseDone.get(), this);
    }
}
namespace JASystem { namespace WaveArcLoader {
/* 02812BBC */
void init(Kernel::THeap* heap) {
    if (!heap)
        heap = Kernel::getAramHeap();
    sRootHeap = heap;
}
/* 02812BEC */
void setCurrentDir(const be<u8>* directory) {
    copyWaveArcString(sCurrentDir.bytes, directory);
    u32 size = 0;
    while (sCurrentDir.bytes[size] != 0)
        ++size;
    if (sCurrentDir.bytes[size - 1] != '/') {
        sCurrentDir.bytes[size] = '/';
        sCurrentDir.bytes[size + 1] = 0;
    }
}
}}
/* 02812C5C */
void __sinit_JASWaveArcLoader_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerWaveArcGlobals(&sRegistration);
    constructWaveArcMonitor(&sMonitor);
    registerWaveArcGlobals(&sMonitorRegistration);
}
/* WWHD glue
data sMonitor 104B5338
data sCurrentDir 101F9E28
data sRootHeap 101F9E24
data sLoader 101F9D58
data sArcVtable 1016FFE4
data sInitialization 104B5328
data sRegistration 101F9E68
data sMonitorRegistration 101F9E74
ext 0273AD10 void* allocateWaveArcObject(u32)
ext 0273B0D4 void* allocateWaveArcName(u32, void*, u32)
ext 0273AFC8 void deleteWaveArcName(void*)
ext 0273AF40 void deleteWaveArcObject(void*)
ext 027EC62C void destroyWaveArcLink(JSUPtrLink*, u32)
ext 027EC68C void destroyWaveArcList(JSUPtrList*, u32)
ext 027604AC void lockWaveArc(ArchiveMonitor*)
ext 027604B4 void unlockWaveArc(ArchiveMonitor*)
ext 027603F0 void constructWaveArcMonitor(ArchiveMonitor*)
ext 028F040C void copyWaveArcString(be<u8>*, const be<u8>*)
ext 0280EF40 u32 getWaveFileSize(void*, const be<u8>*)
ext 0280EEB8 u32 loadWaveFile(void*, const be<u8>*, u32)
ext 0280ED64 void* getWaveTaskThread()
ext 02810AA4 u32 submitWaveTask(void*, LoadCallback, TWaveArc::LoadMessage*, u32)
ext 028F026C void registerWaveArcGlobals(void*)
*/

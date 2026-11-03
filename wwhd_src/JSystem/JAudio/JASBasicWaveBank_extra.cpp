// Cafe grouped wave bank and publication chains, reconstructed from the WWHD binary.
#include "JSystem/JAudio/JASBasicWaveBank_extra.h"
using namespace JASystem;
using namespace gabi;
struct WaveVtable_l { u8 bytes[0x28]; };
struct GroupVtable_l { u8 prefix[0x14]; gfn destroy; };
struct WaveInfoWords_l { be<u32> words[10]; };
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
using CtrlWaveConstructor = TBasicWaveBank::TWaveInfo* (*)(TBasicWaveBank::TWaveInfo*);
// These control records have no array-element destructor.
using CtrlWaveDestructor = void*;
static WaveVtable_l sBankVtable;
static GroupVtable_l sGroupVtable;
static WaveVtable_l sHandleVtable;
static u32 sArrayCookie;
static Initialization sInitialization;
static Registration sRegistration;
extern void* currentBasicWaveBankHeap();
extern TBasicWaveBank::TWaveGroup* initializeBasicWaveGroup(TBasicWaveBank::TWaveGroup*,TBasicWaveBank*);
extern u32 readBasicWaveId(const TBasicWaveBank::TWaveGroup*,s32);
extern void publishBasicWaveGroup(TBasicWaveBank*,TBasicWaveBank::TWaveGroup*);
extern void unpublishBasicWaveGroup(TBasicWaveBank*,TBasicWaveBank::TWaveGroup*);
extern TBasicWaveBank::TWaveGroup* findBasicWaveGroup(TBasicWaveBank*,s32);
extern void* allocateBasicWaveObject(u32);
extern void* allocateBasicWaveHeapObject(u32,void*,u32);
extern void* allocateBasicWaveArray(u32,void*,u32);
extern void freeBasicWaveObject(void*);
extern void freeBasicWaveArray(void*);
extern TWaveArc* constructBasicWaveArc(TWaveArc*);
extern void destroyBasicWaveArc(TWaveArc*,u32);
extern void destroyCtrlWaveArray(void*,s32,u32,CtrlWaveDestructor,u32,u32);
extern TBasicWaveBank::TWaveInfo* constructCtrlWaveArray(void*,u32,u32,CtrlWaveConstructor,u32);
extern void initializeWaveMutex(Kernel::HeapMutex*);
extern void lockWaveMutex(Kernel::HeapMutex*);
extern void unlockWaveMutex(Kernel::HeapMutex*);
extern void clearWaveTable(void*,u32);
extern void registerBasicWaveGlobals(void*);
namespace JASystem {
/* 028448DC */
TBasicWaveBank* constructBasicWaveBank(TBasicWaveBank* bank) {
    if (!bank) {
        bank = static_cast<TBasicWaveBank*>(allocateBasicWaveObject(sizeof(TBasicWaveBank)));
        if (!bank) return nullptr;
    }
    bank->mWaveGroupArray = nullptr;
    bank->mVtable = reinterpret_cast<WaveBankVTable*>(&sBankVtable);
    bank->mWaveGroupCount = 0;
    bank->mWaveTable = nullptr;
    bank->mWaveCount = 0;
    initializeWaveMutex(&bank->mMutex);
    return bank;
}
/* 02844944 */
TBasicWaveBank::TWaveGroup* constructBasicWaveGroup(TBasicWaveBank::TWaveGroup* group,TBasicWaveBank* bank) {
    if (!group) {
        group = static_cast<TBasicWaveBank::TWaveGroup*>(allocateBasicWaveObject(sizeof(TBasicWaveBank::TWaveGroup)));
        if (!group) return nullptr;
    }
    constructBasicWaveArc(group);
    group->mBank = bank;
    group->mVtable = reinterpret_cast<Kernel::DisposerVTable*>(&sGroupVtable);
    group->mWaveCount = 0;
    group->mCtrlWaveArray = nullptr;
    return group;
}
/* 028449B4 */
void TBasicWaveBank::setGroupCount(u32 count) {
    u32 oldCount = mWaveGroupCount;
    auto groups = mWaveGroupArray.get();
    for (u32 i = 0; i < oldCount; ++i) {
        TWaveGroup* group = groups[i];
        if (group) {
            auto table = reinterpret_cast<GroupVtable_l*>(group->mVtable.get());
            call_ptr<void>(table->destroy.get(),group,3u);
            groups = mWaveGroupArray;
            oldCount = mWaveGroupCount;
        }
    }
    freeBasicWaveArray(groups);
    mWaveGroupCount = count;
    void* heap = currentBasicWaveBankHeap();
    void* storage = allocateBasicWaveArray(count * 4,heap,4);
    u32 groupCount = mWaveGroupCount;
    mWaveGroupArray = static_cast<gptr<TWaveGroup>*>(storage);
    for (u32 i = 0; i < groupCount; ++i) {
        heap = currentBasicWaveBankHeap();
        TWaveGroup* group = static_cast<TWaveGroup*>(allocateBasicWaveHeapObject(sizeof(TWaveGroup),heap,4));
        if (group) group = initializeBasicWaveGroup(group,this);
        mWaveGroupArray[i] = group;
        groupCount = mWaveGroupCount;
    }
}
/* 02844B00 */
void TBasicWaveBank::setWaveTableSize(u32 count) {
    freeBasicWaveArray(mWaveTable);
    u32 bytes = count * 4;
    void* heap = currentBasicWaveBankHeap();
    void* storage = allocateBasicWaveArray(bytes,heap,4);
    mWaveTable = static_cast<gptr<TWaveInfo>*>(storage);
    clearWaveTable(storage,bytes);
    mWaveCount = count;
}
/* 02844B6C */
TBasicWaveBank::TWaveGroup* TBasicWaveBank::getWaveGroup(s32 index) {
    if ((u32)index >= (u32)mWaveGroupCount) return nullptr;
    return mWaveGroupArray[index];
}
/* 02844B90 */
TBasicWaveBank::TWaveHandle* TBasicWaveBank::getWaveHandle(u32 index) const {
    if (index >= (u32)mWaveCount) return nullptr;
    TWaveInfo* wave = mWaveTable[index];
    return wave ? &wave->mWaveHandle : nullptr;
}
/* 02844BB8 */
u32 TBasicWaveBank::TWaveGroup::getWaveID(s32 index) const { return mCtrlWaveArray[index].mWaveID; }
/* 02844BCC */
void TBasicWaveBank::incWaveTable(TWaveGroup* group) {
    lockWaveMutex(&mMutex);
    for (u32 i = 0; i < (u32)group->mWaveCount; ++i) {
        u32 id = readBasicWaveId(group,i);
        TWaveInfo* wave = group->mCtrlWaveArray.get() + i;
        auto table = mWaveTable.get();
        wave->mNext = nullptr;
        wave->mPrev = table[id];
        TWaveInfo* previous = table[id];
        if (previous) previous->mNext = wave;
        table[id] = wave;
    }
    unlockWaveMutex(&mMutex);
}
/* 02844C84 */
void TBasicWaveBank::decWaveTable(TWaveGroup* group) {
    lockWaveMutex(&mMutex);
    u32 count = group->mWaveCount;
    for (u32 i = 0; i < count; ++i) {
        u32 id = readBasicWaveId(group,i);
        auto slot = &mWaveTable[id];
        TWaveInfo* wave = *slot;
        TWaveInfo* target = group->mCtrlWaveArray.get() + i;
        while (wave) {
            TWaveInfo* previous = wave->mPrev;
            if (wave == target) {
                TWaveInfo* next = wave->mNext;
                if (!next) *slot = previous;
                else next->mPrev = previous;
                previous = wave->mPrev;
                if (previous) previous->mNext = wave->mNext;
                count = group->mWaveCount;
                break;
            }
            wave = previous;
        }
    }
    unlockWaveMutex(&mMutex);
}
/* 02844D8C */
void destroyBasicWaveGroup(TBasicWaveBank::TWaveGroup* group,u32 flags) {
    if (!group) return;
    void* waves = group->mCtrlWaveArray;
    group->mVtable = reinterpret_cast<Kernel::DisposerVTable*>(&sGroupVtable);
    destroyCtrlWaveArray(waves,-1,sizeof(TBasicWaveBank::TWaveInfo),nullptr,1,0);
    destroyBasicWaveArc(group,0);
    if (flags & 1) freeBasicWaveObject(group);
}
/* 02844E08 */
void TBasicWaveBank::TWaveGroup::setWaveCount(u32 count) {
    destroyCtrlWaveArray(mCtrlWaveArray,-1,sizeof(TWaveInfo),nullptr,1,0);
    mWaveCount = count;
    void* heap = currentBasicWaveBankHeap();
    u32 bytes = count * (u32)sizeof(TWaveInfo) + (u32)sArrayCookie;
    void* storage = allocateBasicWaveArray(bytes,heap,4);
    if (storage) {
        storage = static_cast<u8*>(storage) + (u32)sArrayCookie;
        mCtrlWaveArray = constructCtrlWaveArray(storage,count,sizeof(TWaveInfo),constructBasicCtrlWave,0);
    } else mCtrlWaveArray = nullptr;
    for (u32 i = 0; i < count; ++i) {
        mCtrlWaveArray[i].mWaveHandle.mHeap = &mHeap;
        mCtrlWaveArray[i].mWaveHandle.mWaveInfo.mAdpcmInfo = &mLoaded;
    }
}
/* 02844EFC */
void TBasicWaveBank::TWaveGroup::setWaveInfo(u32 index,u32 id,const JASystem::TWaveInfo& info) {
    mCtrlWaveArray[index].mWaveID = id;
    auto source = reinterpret_cast<const WaveInfoWords_l*>(&info);
    auto destination = reinterpret_cast<WaveInfoWords_l*>(&mCtrlWaveArray[index].mWaveHandle.mWaveInfo);
    for (u32 i = 0; i < 10; ++i) destination->words[i] = source->words[i];
    mCtrlWaveArray[index].mWaveHandle.mWaveInfo.mAdpcmInfo = &mLoaded;
}
/* 02844F48 */
void TBasicWaveBank::TWaveGroup::onLoadDone() { publishBasicWaveGroup(mBank,this); }
/* 02844F54 */
void TBasicWaveBank::TWaveGroup::onEraseDone() { unpublishBasicWaveGroup(mBank,this); }
/* 02844F88 */
TWaveArc* TBasicWaveBank::getWaveArc(s32 index) {
    TWaveGroup* group = findBasicWaveGroup(this,index);
    return group;
}
/* 02844F8C */
TBasicWaveBank::TWaveInfo* constructBasicCtrlWave(TBasicWaveBank::TWaveInfo* wave) {
    if (!wave) {
        wave = static_cast<TBasicWaveBank::TWaveInfo*>(allocateBasicWaveObject(sizeof(TBasicWaveBank::TWaveInfo)));
        if (!wave) return nullptr;
    }
    wave->mWaveHandle.mWaveInfo.mKey = 60;
    wave->mWaveHandle.mVtable = reinterpret_cast<WaveHandleVtable*>(&sHandleVtable);
    wave->mWaveID = 0;
    wave->mWaveHandle.mHeap = nullptr;
    wave->mWaveHandle.mWaveInfo.mAdpcmInfo = &audioDefaultAdpcmInfo;
    wave->mPrev = nullptr;
    wave->mNext = nullptr;
    return wave;
}
/* 02844FF4 */
const JASystem::TWaveInfo* TBasicWaveBank::TWaveHandle::getWaveInfo() const { return &mWaveInfo; }
/* 02844FFC */
u32 TBasicWaveBank::TWaveHandle::getWavePtr() const {
    u32 base = mHeap->mBase;
    if (!base) return 0;
    return base + (u32)mWaveInfo.mWaveOffset;
}
}
/* 02844F60 */
void __sinit_JASBasicWaveBank_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerBasicWaveGlobals(&sRegistration);
}
/* WWHD glue
data sBankVtable 1017A574
data sGroupVtable 1017A54C
data sHandleVtable 1017A528
data sArrayCookie 101FCD08
data sInitialization 104B6604
data sRegistration 101FC248
ext 0273AD10 void* allocateBasicWaveObject(u32)
ext 0273B050 void* allocateBasicWaveHeapObject(u32,void*,u32)
ext 0273B0D4 void* allocateBasicWaveArray(u32,void*,u32)
ext 0273AF40 void freeBasicWaveObject(void*)
ext 0273AFC8 void freeBasicWaveArray(void*)
ext 02812668 JASystem::TWaveArc* constructBasicWaveArc(JASystem::TWaveArc*)
ext 028126F0 void destroyBasicWaveArc(JASystem::TWaveArc*,u32)
ext 028F0164 void destroyCtrlWaveArray(void*,s32,u32,CtrlWaveDestructor,u32,u32)
ext 028EFF8C JASystem::TBasicWaveBank::TWaveInfo* constructCtrlWaveArray(void*,u32,u32,CtrlWaveConstructor,u32)
ext C0009D68 void initializeWaveMutex(JASystem::Kernel::HeapMutex*)
ext C0009E40 void lockWaveMutex(JASystem::Kernel::HeapMutex*)
ext C000A1C8 void unlockWaveMutex(JASystem::Kernel::HeapMutex*)
ext 02843E34 void clearWaveTable(void*,u32)
ext 028452A4 void* currentBasicWaveBankHeap()
ext 02844944 JASystem::TBasicWaveBank::TWaveGroup* initializeBasicWaveGroup(JASystem::TBasicWaveBank::TWaveGroup*,JASystem::TBasicWaveBank*)
ext 02844BB8 u32 readBasicWaveId(const JASystem::TBasicWaveBank::TWaveGroup*,s32)
ext 02844BCC void publishBasicWaveGroup(JASystem::TBasicWaveBank*,JASystem::TBasicWaveBank::TWaveGroup*)
ext 02844C84 void unpublishBasicWaveGroup(JASystem::TBasicWaveBank*,JASystem::TBasicWaveBank::TWaveGroup*)
ext 02844B6C JASystem::TBasicWaveBank::TWaveGroup* findBasicWaveGroup(JASystem::TBasicWaveBank*,s32)
ext 028F026C void registerBasicWaveGlobals(void*)
*/

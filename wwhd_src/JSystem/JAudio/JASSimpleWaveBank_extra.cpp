// Cafe simple wave bank and array lifecycle, reconstructed from the WWHD binary.
#include "JSystem/JAudio/JASSimpleWaveBank_extra.h"
using namespace JASystem;
using namespace gabi;
struct WaveVtable_l { u8 bytes[0x28]; };
struct WaveInfoWords_l { be<u32> words[10]; };
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
using WaveHandleConstructor = TSimpleWaveBank::TWaveHandle* (*)(TSimpleWaveBank::TWaveHandle*);
using WaveHandleDestructor = void (*)(TSimpleWaveBank::TWaveHandle*,u32);
static WaveVtable_l sInitialVtable;
static WaveVtable_l sBankVtable;
static WaveVtable_l sArcVtable;
static WaveVtable_l sHandleVtable;
static u32 sArrayCookie;
static Initialization sSimpleInitialization;
static Registration sSimpleRegistration;
extern void* currentWaveBankHeap();
extern void* allocateWaveObject(u32);
extern void* allocateWaveArray(u32,void*,u32);
extern void freeWaveObject(void*);
extern TWaveArc* constructSimpleWaveArc(TWaveArc*);
extern void destroyWaveHandleArray(void*,s32,u32,WaveHandleDestructor,u32,u32);
extern TSimpleWaveBank::TWaveHandle* constructWaveHandleArray(void*,u32,u32,WaveHandleConstructor,u32);
extern void registerWaveGlobals(void*);
namespace JASystem {
/* 0284501C */
TSimpleWaveBank* constructSimpleWaveBank(TSimpleWaveBank* bank) {
    if (!bank) {
        bank = static_cast<TSimpleWaveBank*>(allocateWaveObject(sizeof(TSimpleWaveBank)));
        if (!bank) return nullptr;
    }
    bank->TWaveBank::mVtable = reinterpret_cast<WaveBankVTable*>(&sInitialVtable);
    constructSimpleWaveArc(static_cast<TWaveArc*>(bank));
    bank->TWaveBank::mVtable = reinterpret_cast<WaveBankVTable*>(&sBankVtable);
    bank->TWaveArc::mVtable = reinterpret_cast<Kernel::DisposerVTable*>(&sArcVtable);
    bank->mWaveCount = 0;
    bank->mWaveTable = nullptr;
    return bank;
}
/* 02845094 */
void TSimpleWaveBank::setWaveTableSize(u32 count) {
    destroyWaveHandleArray(mWaveTable,-1,sizeof(TWaveHandle),destroySimpleWaveHandle,1,0);
    void* heap = currentWaveBankHeap();
    u32 bytes = count * (u32)sizeof(TWaveHandle) + (u32)sArrayCookie;
    void* storage = allocateWaveArray(bytes,heap,4);
    if (storage) {
        storage = static_cast<u8*>(storage) + (u32)sArrayCookie;
        mWaveTable = constructWaveHandleArray(storage,count,sizeof(TWaveHandle),constructSimpleWaveHandle,0);
        mWaveCount = count;
    } else {
        mWaveCount = count;
        mWaveTable = nullptr;
    }
}
/* 02845150 */
void TSimpleWaveBank::setWaveInfo(u32 index,const TWaveInfo& info) {
    auto source = reinterpret_cast<const WaveInfoWords_l*>(&info);
    auto destination = reinterpret_cast<WaveInfoWords_l*>(&mWaveTable[index].mWaveInfo);
    for (u32 i = 0; i < 10; ++i) destination->words[i] = source->words[i];
    mWaveTable[index].mWaveInfo.mAdpcmInfo = &mLoaded;
    mWaveTable[index].mHeap = &mHeap;
}
/* 028451A0 */
TSimpleWaveBank::TWaveHandle* TSimpleWaveBank::getWaveHandle(u32 index) const {
    if (index >= (u32)mWaveCount) return nullptr;
    return mWaveTable.get() + index;
}
/* 028451C4 */
TWaveArc* TSimpleWaveBank::getWaveArc(s32 index) {
    return index == 0 ? static_cast<TWaveArc*>(this) : nullptr;
}
/* 02845204 */
void destroySimpleWaveHandle(TSimpleWaveBank::TWaveHandle* handle,u32 flags) {
    if (handle && (flags & 1)) freeWaveObject(handle);
}
/* 02845218 */
void TSimpleWaveBank::onLoadDone() {}
/* 0284521C */
void TSimpleWaveBank::onEraseDone() {}
/* 02845220 */
TSimpleWaveBank::TWaveHandle* constructSimpleWaveHandle(TSimpleWaveBank::TWaveHandle* handle) {
    if (!handle) {
        handle = static_cast<TSimpleWaveBank::TWaveHandle*>(allocateWaveObject(sizeof(TSimpleWaveBank::TWaveHandle)));
        if (!handle) return nullptr;
    }
    handle->mWaveInfo.mKey = 60;
    handle->mVtable = reinterpret_cast<WaveHandleVtable*>(&sHandleVtable);
    handle->mHeap = nullptr;
    handle->mWaveInfo.mAdpcmInfo = &audioDefaultAdpcmInfo;
    return handle;
}
/* 0284527C */
const TWaveInfo* TSimpleWaveBank::TWaveHandle::getWaveInfo() const { return &mWaveInfo; }
/* 02845284 */
u32 TSimpleWaveBank::TWaveHandle::getWavePtr() const {
    u32 base = mHeap->mBase;
    if (!base) return 0;
    return base + (u32)mWaveInfo.mWaveOffset;
}

}
/* 028451DC */
void __sinit_JASSimpleWaveBank_cpp() {
    sSimpleInitialization.words[2] = 0;
    sSimpleInitialization.words[0] = 0;
    sSimpleInitialization.words[3] = 0;
    sSimpleInitialization.words[1] = 0;
    registerWaveGlobals(&sSimpleRegistration);
}
/* WWHD glue
data sInitialVtable 1017A598
data sBankVtable 1017A5DC
data sArcVtable 1017A5FC
data sHandleVtable 1017A5B8
data sArrayCookie 101FCD08
data sSimpleInitialization 104B6614
data sSimpleRegistration 101FC254
ext 0273AD10 void* allocateWaveObject(u32)
ext 0273B0D4 void* allocateWaveArray(u32,void*,u32)
ext 0273AF40 void freeWaveObject(void*)
ext 02812668 JASystem::TWaveArc* constructSimpleWaveArc(JASystem::TWaveArc*)
ext 028F0164 void destroyWaveHandleArray(void*,s32,u32,WaveHandleDestructor,u32,u32)
ext 028EFF8C JASystem::TSimpleWaveBank::TWaveHandle* constructWaveHandleArray(void*,u32,u32,WaveHandleConstructor,u32)
ext 028452A4 void* currentWaveBankHeap()
ext 028F026C void registerWaveGlobals(void*)
*/

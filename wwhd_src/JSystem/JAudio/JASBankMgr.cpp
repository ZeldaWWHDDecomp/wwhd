// Instrument-bank registry, reconstructed from the WWHD binary.
#include "JSystem/JAudio/JASBankMgr.h"
#include "sead/ElementAt.h"
#include "JSystem/JAudio/JASChannel.h"
#include "JSystem/JAudio/JASRate.h"
#include "JSystem/JAudio/JASWSParser.h"
using namespace JASystem;
using namespace JASystem::BankMgr;
using BankTable = gptr<gptr<TBank>>;
using PhysicalTable = gptr<be<u16>>;
using HeapPointer = gptr<void>;
static BankTable sBankArray;
static PhysicalTable sVir2PhyTable;
static s32 sTableSize;
static HeapPointer sDram;
struct BankStreamHeader { u8 prefix[8]; be<u32> mVirtualId; };
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
static Initialization sInitialization;
static Registration sRegistration;
extern void* allocateBankRegistry(u32, void*, u32);
extern void clearBankRegistry(void*, u32);
extern TBank* parseInstrumentBank(void*);
extern void registerBankGlobals(void*);
extern TChannel* allocateBankChannel(void*, u32, u32);
extern void setBankChannelMixer(TChannel*, f32, f32, f32);
extern f32 bankPitchRatio(s32);
extern void setBankChannelOscillator(TChannel*, u32, const TOscillator::Osc_*);
extern u32 playBankChannel(TChannel*);
extern void releaseBankChannel(TChannel*, u32);
static TOscillator::Osc_ sDefaultEnvelope;
struct WaveHandleVTable { u8 prefix[0x14]; gfn getInfo; u8 padding[4]; gfn getData; };
struct WaveHandle { gptr<WaveHandleVTable> mVtable; };
struct BankWaveVTable { u8 prefix[0x14]; gfn getHandle; };
struct BankWave { gptr<BankWaveVTable> mVtable; };
// HD: these scalar transfers preserve signaling-NaN payload bits without arithmetic.
struct BankParameterBits_l { be<u32> mValue; };
WWHD_SIZE(BankParameterBits_l, 4);
static void copyBankParameter(be<f32>& output, const be<f32>& input) {
    auto destination = reinterpret_cast<BankParameterBits_l*>(&output);
    auto source = reinterpret_cast<const BankParameterBits_l*>(&input);
    destination->mValue = source->mValue;
}
namespace JASystem { namespace BankMgr {
/* 02810E80 */
void init(s32 size) {
    u32 bytes = (u32)size * 4;
    sBankArray = static_cast<gptr<TBank>*>(allocateBankRegistry(bytes, sDram, 4));
    sVir2PhyTable = static_cast<be<u16>*>(allocateBankRegistry((u32)size * 2, sDram, 4));
    clearBankRegistry(sBankArray, bytes);
    for (s32 i = 0; i < size; ++i)
        sVir2PhyTable.get()[(u32)i] = 0xFFFF;
    sTableSize = size;
}
/* 02810F34 */
u32 registBank(s32 banknum, TBank* bank) {
    sead::elementAt(sBankArray.get(), (u32)banknum) = bank;
    return 1;
}
/* 02810F4C */
void setVir2PhyTable(u32 virtualId, u32 banknum) {
    // HD: the sentinel comparison uses the complete incoming register.
    if (virtualId != 0xFFFF)
        sead::elementAt(sVir2PhyTable.get(), virtualId) = (u16)banknum;
}
/* 02810F68 */
u32 registBankBNK(s32 banknum, void* stream) {
    u32 virtualId = static_cast<BankStreamHeader*>(stream)->mVirtualId;
    setVir2PhyTable(virtualId, (u32)banknum);
    TBank* bank = parseInstrumentBank(stream);
    if (!bank)
        return 0;
    return registBank(banknum, bank);
}
/* 02810FBC */
TBank* getBank(s32 banknum) {
    if (banknum < 0 || banknum >= sTableSize)
        return nullptr;
    return sead::elementAt(sBankArray.get(), (u32)banknum);
}
/* 02810FF0 */
u32 getPhysicalNumber(u32 virtualId) {
    return sead::elementAt(sVir2PhyTable.get(), virtualId);
}
/* 02811004 */
u32 assignWaveBank(s32 banknum, s32 waveBanknum) {
    TBank* bank = getBank(banknum);
    if (!bank)
        return 0;
    TWaveBank* waveBank = WaveBankMgr::getWaveBank(waveBanknum);
    if (!waveBank)
        return 0;
    bank->mWaveBank = waveBank;
    return 1;
}
/* 02811064 */
TChannel* noteOn(s32 banknum, s32 program, s32 key, u32 velocity, u32 channelId, u32 callback, u32 userArg) {
    if (program > 0xEF) {
        TChannel* channel = allocateBankChannel(nullptr, callback, userArg);
        if (!channel) return nullptr;
        setBankChannelMixer(channel, 1.0f, 1.0f, 1.0f);
        channel->mPriority = (u16)channelId;
        channel->mWaveStart = (u32)program - 0xF0;
        channel->mWaveType = 2;
        f32 base = 16736.015625f / Kernel::getDacRate();
        channel->mBasePitch = base;
        channel->mPitch = base;
        f32 pitch = base * bankPitchRatio(key);
        f32 gain = (f32)velocity / 127.0f;
        channel->mPitch = pitch;
        channel->mBaseVolume = 1.0f;
        channel->mVolume = gain * gain;
        setBankChannelOscillator(channel, 0, &sDefaultEnvelope);
        return playBankChannel(channel) ? channel : nullptr;
    }
    TBank* bank = getBank(banknum);
    if (!bank) return nullptr;
    TInst* inst = gabi::call_ptr<TInst*>(bank->mVtable->getInst.get(), bank, program);
    if (!inst) return nullptr;
    TInstParam param;
    param.mVolume = 1.0f; param.mOscCount = 0; param.mPan = 0.5f;
    param.mPitch = 1.0f; param.mOscData = nullptr; param.mType = 0;
    param.mDolby = 0.0f; param.mPercussion = 0; param.mRelease = 0;
    param.mFxMix = 0.0f; param.mWaveId = 0;
    if (!gabi::call_ptr<u32>(inst->mVtable->getParam.get(), inst, key, velocity, &param)) return nullptr;
    BankWave* waves = reinterpret_cast<BankWave*>(bank->mWaveBank.get());
    if (!waves) return nullptr;
    WaveHandle* handle = gabi::call_ptr<WaveHandle*>(waves->mVtable->getHandle.get(), waves, (u32)param.mWaveId);
    if (!handle) return nullptr;
    TWaveInfo* info = gabi::call_ptr<TWaveInfo*>(handle->mVtable->getInfo.get(), handle);
    if (!info) return nullptr;
    u32 data = gabi::call_ptr<u32>(handle->mVtable->getData.get(), handle);
    if (!data) return nullptr;
    TChannel* channel = allocateBankChannel(nullptr, callback, userArg);
    if (!channel) return nullptr;
    setBankChannelMixer(channel, 1.0f, 1.0f, 1.0f);
    channel->mWave = info; channel->mWaveStart = data;
    channel->mPriority = (u16)channelId; channel->mWaveType = param.mType;
    f32 rate = Kernel::getDacRate();
    f32 base = ((f32)info->mSampleRate / rate) * (f32)param.mPitch;
    channel->mBasePitch = base; channel->mPitch = base;
    if (!param.mPercussion) channel->mPitch = base * bankPitchRatio((s32)((u32)key + 60 - (u32)info->mKey));
    f32 gain = (f32)velocity / 127.0f;
    f32 volume = (f32)param.mVolume;
    channel->mBaseVolume = volume; channel->mVolume = (gain * gain) * volume;
    copyBankParameter(channel->mPanBase, param.mPan);
    copyBankParameter(channel->mFxMixBase, param.mFxMix);
    copyBankParameter(channel->mDolbyBase, param.mDolby);
    for (u32 i = 0; i < (u32)param.mOscCount; ++i)
        setBankChannelOscillator(channel, i, param.mOscData.get()[i]);
    releaseBankChannel(channel, (u32)param.mRelease);
    return playBankChannel(channel) ? channel : nullptr;
}
/* 02811438 */
void gateOn(TChannel* channel, s32 key, u32 velocity) {
    if (channel->mWaveType != 2) key = (s32)((u32)key + 60 - (u32)static_cast<TWaveInfo*>(channel->mWave.get())->mKey);
    f32 ratio = bankPitchRatio(key);
    f32 base = channel->mBasePitch;
    f32 gain = (f32)velocity / 127.0f;
    f32 pitch = base * ratio;
    f32 volume = (gain * gain) * (f32)channel->mBaseVolume;
    channel->mPitch = pitch; channel->mVolume = volume;
}

}}
/* 028114D8 */
void __sinit_JASBankMgr_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerBankGlobals(&sRegistration);
}
/* WWHD glue
data sDefaultEnvelope 1016FECC
ext 02814028 TChannel* allocateBankChannel(void*, u32, u32)
ext 028153D0 void setBankChannelMixer(TChannel*, f32, f32, f32)
ext 02815578 f32 bankPitchRatio(s32)
ext 028154A4 void setBankChannelOscillator(TChannel*, u32, const TOscillator::Osc_*)
ext 028152A4 u32 playBankChannel(TChannel*)
ext 02815328 void releaseBankChannel(TChannel*, u32)
data sBankArray 104B52A8
data sVir2PhyTable 104B52AC
data sTableSize 104B52A4
data sDram 101F9DA8
data sInitialization 104B52B0
data sRegistration 101F9DD4
ext 0273B0D4 void* allocateBankRegistry(u32, void*, u32)
ext 02843E34 void clearBankRegistry(void*, u32)
ext 02811518 TBank* parseInstrumentBank(void*)
ext 028F026C void registerBankGlobals(void*)
*/

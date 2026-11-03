// BNKParser offset conversion helpers, reconstructed from WWHD.
#include "JSystem/JAudio/JASBNKParser.h"
#include "JSystem/JAudio/JASDrumSet.h"
#include "JSystem/JAudio/JASInstRand.h"
#include "JSystem/JAudio/JASInstSense.h"
#include "JSystem/JKernel/JKRHeap_base.h"
using namespace JASystem;
using namespace JASystem::BNKParser;
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
static Initialization sInitialization;
static Registration sRegistration;
extern void registerBNKParserGlobals(void*);
static u32 sUsedHeapSize;
static u32 sRandomVtable;
static u32 sSenseVtable;
static u32 sPercussionVtable;
extern JKRHeap_l* getBankParserHeap();
extern void* allocateBankParserObject(u32, void*, u32);
extern void* allocateBankParserArray(u32, void*, u32);
extern TBasicBank* constructParsedBank(void*);
extern TBasicInst* constructParsedInst(void*);
extern void setParsedInstCount(TBasicBank*, u32);
extern void setParsedInst(TBasicBank*, s32, JASystem::TInst*);
extern void setParsedOscCount(TBasicInst*, u32);
extern void setParsedOsc(TBasicInst*, s32, TOscillator::Osc_*);
extern void setParsedEffectCount(TBasicInst*, u32);
extern void setParsedEffect(TBasicInst*, s32, TInstEffect*);
extern void setParsedKeyCount(TBasicInst*, u32);
extern TBasicInst::TKeymap* getParsedKey(TBasicInst*, s32);
extern void setParsedVelocityCount(TBasicInst::TKeymap*, u32);
extern TBasicInst::TVeloRegion* getParsedVelocity(TBasicInst::TKeymap*, s32);
extern void copyBankParserData(const void*, void*, u32);
using ParsedPercussionConstructor = void* (*)(void*);
extern void* constructParsedPercussion(void*);
extern void constructParsedPercussionArray(void*, u32, u32, ParsedPercussionConstructor);
static TInstRand* createRandomEffect(JKRHeap_l* heap, const TRand* random) {
    TInstRand* effect = static_cast<TInstRand*>(allocateBankParserObject(0x10, heap, 4));
    if (effect) {
        effect->mBase = 1.0f; effect->mWidth = 0.0f;
        effect->mTarget = 0; effect->mVtable = &sRandomVtable;
    }
    effect->setTarget(random->mTarget);
    effect->mBase = random->mBase; effect->mWidth = random->mWidth;
    return effect;
}
static void fillVelocity(TBasicInst::TVeloRegion* velocity, const TVmap* raw) {
    velocity->mBaseVel = (u32)raw->mBaseVelocity;
    velocity->mWaveId = (u32)raw->mWaveId & 0xFFFF;
    velocity->mVolume = raw->mVolume; velocity->mPitch = raw->mPitch;
}
namespace JASystem { namespace BNKParser {
/* 02811518 */
TBasicBank* createBasicBank(void* stream) {
    JKRHeap_l* heap = getBankParserHeap();
    u32 freeSize = gabi::call_ptr<u32>(heap->mVtable->mSize.get(), heap);
    THeader* header = static_cast<THeader*>(stream);
    void* storage = allocateBankParserObject(0x10, heap, 4);
    TBasicBank* bank = storage ? constructParsedBank(storage) : nullptr;
    if (!bank) return nullptr;
    setParsedInstCount(bank, 256);
    TInstParam param;
    for (s32 i = 0; i < 128; ++i) {
        TInst* raw = convertInstrumentOffset(header, header->mInstOffsets[i]);
        if (!raw) continue;
        void* instStorage = allocateBankParserObject(0x24, heap, 4);
        TBasicInst* inst = instStorage ? constructParsedInst(instStorage) : nullptr;
        inst->mVolume = raw->mVolume; inst->mPitch = raw->mPitch;
        setParsedOscCount(inst, 2);
        s32 oscIndex = 0;
        for (s32 j = 0; j < 2; ++j) {
            TOsc* oscRaw = convertOscillatorOffset(header, raw->mOscOffsets[j]);
            if (!oscRaw) continue;
            TOscillator::Osc_* osc = nullptr;
            bool found = false;
            for (s32 n = 0; n < 128 && !found; ++n) {
                TInst* previous = convertInstrumentOffset(header, header->mInstOffsets[n]);
                if (!previous) continue;
                for (u32 k = 0; k < 2; ++k) {
                    if (convertOscillatorOffset(header, previous->mOscOffsets[k]) != oscRaw) continue;
                    JASystem::TInst* built = gabi::call_ptr<JASystem::TInst*>(bank->mVtable->getInst.get(), bank, n);
                    if (!built) continue;
                    param.mPercussion = 0; param.mDolby = 0.0f; param.mRelease = 0;
                    param.mVolume = 1.0f; param.mType = 0; param.mFxMix = 0.0f;
                    param.mOscCount = 0; param.mPitch = 1.0f; param.mOscData = nullptr;
                    param.mPan = 0.5f; param.mWaveId = 0;
                    gabi::call_ptr<u32>(built->mVtable->getParam.get(), built, 60, 127, &param);
                    if (k < (u32)param.mOscCount) {
                        osc = const_cast<TOscillator::Osc_*>(param.mOscData.get()[k].get());
                        found = true;
                        break;
                    }
                }
            }
            if (!osc) {
                osc = static_cast<TOscillator::Osc_*>(allocateBankParserObject(0x18, heap, 4));
                // HD stores the byte target as a complete word in the runtime oscillator.
                osc->mTargetWord = (u32)oscRaw->mTarget;
                osc->mRate = oscRaw->mRate;
                be<s16>* table = convertOscTableOffset(header, oscRaw->mTableOffset);
                if (table) {
                    u32 bytes = gabi::ea(getOscTableEndPtr(table)) - gabi::ea(table);
                    be<s16>* copy = static_cast<be<s16>*>(allocateBankParserArray(bytes, heap, 4));
                    copyBankParserData(table, copy, bytes);
                    osc->table = copy;
                } else osc->table = nullptr;
                table = convertOscTableOffset(header, oscRaw->mReleaseOffset);
                if (table) {
                    u32 bytes = gabi::ea(getOscTableEndPtr(table)) - gabi::ea(table);
                    be<s16>* copy = static_cast<be<s16>*>(allocateBankParserArray(bytes, heap, 4));
                    copyBankParserData(table, copy, bytes);
                    osc->rel_table = copy;
                } else osc->rel_table = nullptr;
                osc->mScale = oscRaw->mAmplitude; osc->mOffset = oscRaw->mBias;
            }
            setParsedOsc(inst, oscIndex++, osc);
        }
        setParsedEffectCount(inst, 4);
        for (s32 j = 0; j < 2; ++j) {
            TRand* random = convertRandomOffset(header, raw->mRandomOffsets[j]);
            if (!random) continue;
            TInstRand* effect = createRandomEffect(heap, random);
            setParsedEffect(inst, j, effect);
        }
        for (s32 j = 0; j < 2; ++j) {
            TSense* sense = convertSenseOffset(header, raw->mSenseOffsets[j]);
            if (!sense) continue;
            TInstSense* effect = static_cast<TInstSense*>(allocateBankParserObject(0x14, heap, 4));
            if (effect) {
                effect->mLow = 1.0f; effect->mHigh = 1.0f; effect->mTarget = 0;
                effect->mTrigger = 0; effect->mCenterKey = 60; effect->mVtable = &sSenseVtable;
            }
            effect->setTarget(sense->mTarget);
            effect->setParams(sense->mTrigger, sense->mCenter, sense->mLow, sense->mHigh);
            setParsedEffect(inst, j + 2, effect);
        }
        setParsedKeyCount(inst, raw->mKeyCount);
        for (u32 j = 0; j < (u32)raw->mKeyCount; ++j) {
            TBasicInst::TKeymap* key = getParsedKey(inst, j);
            TKeymap* keyRaw = convertKeymapOffset(header, raw->mKeyOffsets[j]);
            key->mBaseKey = (u32)keyRaw->mBaseKey;
            setParsedVelocityCount(key, keyRaw->mVelocityCount);
            for (u32 k = 0; k < (u32)keyRaw->mVelocityCount; ++k) {
                TBasicInst::TVeloRegion* velocity = getParsedVelocity(key, k);
                TVmap* velocityRaw = convertVelocityMapOffset(header, keyRaw->mVelocityOffsets[k]);
                fillVelocity(velocity, velocityRaw);
            }
        }
        setParsedInst(bank, i, inst);
    }
    for (s32 i = 0; i < 12; ++i) {
        TPerc* raw = convertPercussionOffset(header, header->mPercussionOffsets[i]);
        if (!raw) continue;
        TDrumSet* set = static_cast<TDrumSet*>(allocateBankParserObject(0x1004, heap, 4));
        if (set) {
            set->mVtable = reinterpret_cast<InstVTable*>(&sPercussionVtable);
            constructParsedPercussionArray(set->mPercussion, 128, 0x20, constructParsedPercussion);
        }
        for (s32 j = 0; j < 128; ++j) {
            TPmap* pmap = convertPercussionMapOffset(header, raw->mPercussionOffsets[j]);
            if (!pmap) continue;
            TDrumSet::TPerc* percussion = set->getPerc(j);
            percussion->mVolume = pmap->mVolume; percussion->mPitch = pmap->mPitch;
            if (raw->mMagic == 0x50455232) {
                percussion->mPan = (f32)(s32)raw->mPan[j] / 127.0f;
                percussion->setRelease(raw->mRelease[j]);
            }
            percussion->setEffectCount(2);
            s32 effectIndex = 0;
            for (s32 k = 0; k < 2; ++k) {
                TRand* random = convertRandomOffset(header, pmap->mRandomOffsets[k]);
                if (!random) continue;
                TInstRand* effect = createRandomEffect(heap, random);
                percussion->setEffect(effectIndex++, effect);
            }
            percussion->setVeloRegionCount(pmap->mVelocityCount);
            for (u32 k = 0; k < (u32)pmap->mVelocityCount; ++k) {
                TBasicInst::TVeloRegion* velocity = percussion->getVeloRegion(k);
                TVmap* velocityRaw = convertVelocityMapOffset(header, pmap->mVelocityOffsets[k]);
                fillVelocity(velocity, velocityRaw);
            }
        }
        setParsedInst(bank, i + 0xE4, set);
    }
    sUsedHeapSize += freeSize - gabi::call_ptr<u32>(heap->mVtable->mSize.get(), heap);
    return bank;
}

/* 02811500 */
be<s16>* getOscTableEndPtr(be<s16>* table) {
    while (*table <= 10)
        table += 3;
    return table + 3;
}
/* 02811F6C */
be<s16>* convertOscTableOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<be<s16>*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
/* 02811F80 */
TVmap* convertVelocityMapOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<TVmap*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
/* 02811F94 */
TOsc* convertOscillatorOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<TOsc*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
/* 02811FA8 */
TRand* convertRandomOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<TRand*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
/* 02811FBC */
TSense* convertSenseOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<TSense*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
/* 02811FD0 */
TKeymap* convertKeymapOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<TKeymap*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
/* 02811FE4 */
TPmap* convertPercussionMapOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<TPmap*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
/* 02811FF8 */
TInst* convertInstrumentOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<TInst*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
/* 0281200C */
TPerc* convertPercussionOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<TPerc*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
}}
/* 02811F44 */
void __sinit_JASBNKParser_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerBNKParserGlobals(&sRegistration);
}
/* WWHD glue
data sUsedHeapSize 101F9DE0
data sRandomVtable 1016FF94
data sSenseVtable 1016FFC4
data sPercussionVtable 1016FF54
ext 0284416C JKRHeap_l* getBankParserHeap()
ext 0273B050 void* allocateBankParserObject(u32, void*, u32)
ext 0273B0D4 void* allocateBankParserArray(u32, void*, u32)
ext 028441B0 TBasicBank* constructParsedBank(void*)
ext 028442C8 TBasicInst* constructParsedInst(void*)
ext 02844224 void setParsedInstCount(TBasicBank*, u32)
ext 02844290 void setParsedInst(TBasicBank*, s32, JASystem::TInst*)
ext 02844500 void setParsedOscCount(TBasicInst*, u32)
ext 0284458C void setParsedOsc(TBasicInst*, s32, TOscillator::Osc_*)
ext 02844464 void setParsedEffectCount(TBasicInst*, u32)
ext 028444F0 void setParsedEffect(TBasicInst*, s32, TInstEffect*)
ext 02844384 void setParsedKeyCount(TBasicInst*, u32)
ext 02844440 TBasicInst::TKeymap* getParsedKey(TBasicInst*, s32)
ext 028447F0 void setParsedVelocityCount(TBasicInst::TKeymap*, u32)
ext 02844848 TBasicInst::TVeloRegion* getParsedVelocity(TBasicInst::TKeymap*, s32)
ext 02843D30 void copyBankParserData(const void*, void*, u32)
ext 028EFFD0 void constructParsedPercussionArray(void*, u32, u32, ParsedPercussionConstructor)
ext 02812270 void* constructParsedPercussion(void*) callback
data sInitialization 104B52C0
data sRegistration 101F9DE4
ext 028F026C void registerBNKParserGlobals(void*)
*/

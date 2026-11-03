// Wave-bank registry, reconstructed from WWHD with GameCube JAudio names.
#include "JSystem/JAudio/JASWaveBankMgr.h"
#include "sead/ElementAt.h"
using namespace JASystem;
using namespace JASystem::WaveBankMgr;
using WaveBankTable = gptr<gptr<TWaveBank>>;
using HeapPointer = gptr<void>;
static WaveBankTable sWaveBank;
static s32 sTableSize;
static HeapPointer sDram;
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
static Initialization sInitialization;
static Registration sRegistration;
extern void* allocateWaveBankTable(u32, void*, u32);
extern void clearWaveBankTable(void*, u32);
extern u32 countWaveBankGroups(void*);
extern TWaveBank* parseBasicWaveBank(void*);
extern TWaveBank* parseSimpleWaveBank(void*);
extern u32 loadWaveArchive(TWaveArc*, Kernel::THeap*);
extern u32 eraseWaveArchive(TWaveArc*);
extern void registerWaveBankGlobals(void*);
/* 02812CB8 */
void JASWaveBankMgr_initializeStorage() {}
/* 02812CBC */
void JASWaveBankMgr_finalizeStorage() {}
namespace JASystem { namespace WaveBankMgr {
/* 02812CC0 */
void init(s32 size) {
    u32 bytes = (u32)size * 4;
    auto table = static_cast<gptr<TWaveBank>*>(allocateWaveBankTable(bytes, sDram, 4));
    sWaveBank = table;
    clearWaveBankTable(table, bytes);
    sTableSize = size;
}
/* 02812D20 */
TWaveBank* getWaveBank(s32 banknum) {
    if (banknum < 0 || banknum >= sTableSize)
        return nullptr;
    return sead::elementAt(sWaveBank.get(), (u32)banknum);
}
/* 02812D54 */
u32 registWaveBank(s32 banknum, TWaveBank* bank) {
    // HD: bounds assertions are absent.
    sead::elementAt(sWaveBank.get(), (u32)banknum) = bank;
    return 1;
}
/* 02812D6C */
u32 registWaveBankWS(s32 banknum, void* stream) {
    TWaveBank* bank;
    if (countWaveBankGroups(stream) == 1)
        bank = parseSimpleWaveBank(stream);
    else
        bank = parseBasicWaveBank(stream);
    if (!bank)
        return 0;
    return registWaveBank(banknum, bank);
}
/* 02812DE0 */
TWaveArc* getWaveArc(s32 banknum, s32 arcnum) {
    TWaveBank* bank = getWaveBank(banknum);
    if (!bank)
        return nullptr;
    return gabi::call_ptr<TWaveArc*>(bank->mVtable->getWaveArc.get(), bank, arcnum);
}
/* 02812E2C */
u32 loadWave(s32 banknum, s32 arcnum, Kernel::THeap* heap) {
    TWaveArc* arc = getWaveArc(banknum, arcnum);
    if (!arc)
        return 0;
    return loadWaveArchive(arc, heap);
}
/* 02812E70 */
u32 eraseWave(s32 banknum, s32 arcnum) {
    TWaveArc* arc = getWaveArc(banknum, arcnum);
    if (!arc)
        return 0;
    return eraseWaveArchive(arc) != 0;
}
}}
/* 02812EBC */
void __sinit_JASWaveBankMgr_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerWaveBankGlobals(&sRegistration);
}
/* WWHD glue
data sWaveBank 104B5378
data sTableSize 104B5374
data sDram 101F9DA8
data sInitialization 104B537C
data sRegistration 101F9E80
ext 0273B0D4 void* allocateWaveBankTable(u32, void*, u32)
ext 02843E34 void clearWaveBankTable(void*, u32)
ext 02812EE4 u32 countWaveBankGroups(void*)
ext 02812F0C TWaveBank* parseBasicWaveBank(void*)
ext 02813150 TWaveBank* parseSimpleWaveBank(void*)
ext 02812A18 u32 loadWaveArchive(TWaveArc*, Kernel::THeap*)
ext 02812AE0 u32 eraseWaveArchive(TWaveArc*)
ext 028F026C void registerWaveBankGlobals(void*)
*/

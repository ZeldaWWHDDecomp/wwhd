// WSParser offset conversion helpers, reconstructed from WWHD.
#include "JSystem/JAudio/JASWSParser.h"
#include "JSystem/JKernel/JKRHeap_base.h"
using namespace JASystem;
using namespace JASystem::WSParser;
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
static Initialization sInitialization;
static Registration sRegistration;
extern void registerWSParserGlobals(void*);
static u32 sUsedHeapSize;
static u32 audioDefaultAdpcmInfo;
extern JKRHeap_l* getWaveParserHeap();
extern void* allocateWaveParserObject(u32, void*, u32);
extern TWaveBank* constructParsedBasicWaveBank(void*);
extern TWaveBank* constructParsedSimpleWaveBank(void*);
extern void setParsedWaveGroupCount(TWaveBank*, u32);
extern TWaveArc* getParsedWaveGroup(TWaveBank*, u32);
extern void setParsedGroupWaveCount(TWaveArc*, u32);
extern void setParsedGroupWaveInfo(TWaveArc*, u32, u32, const TWaveInfo*);
extern void setParsedBasicWaveTableSize(TWaveBank*, u32);
extern void setParsedSimpleWaveTableSize(TWaveBank*, u32);
extern void setParsedSimpleWaveInfo(TWaveBank*, u32, const TWaveInfo*);
// Inlined into both archive parsers; the typed stack record has one native slot.
static void fillWaveInfo(TWaveInfo& info, const TWave* wave) {
    info.mKey = 60;
    info.mAdpcmInfo = &audioDefaultAdpcmInfo;
    info.mBlockType = wave->mBlockType;
    info.mKey = wave->mKey;
    info.mSampleRate = wave->mSampleRate;
    info.mWaveOffset = wave->mWaveOffset;
    info.mLoopMode = wave->mLoopMode;
    info.mLoopStart = wave->mLoopStart;
    info.mLoopEnd = wave->mLoopEnd;
    info.mSampleCount = wave->mSampleCount;
    info.mLoopHistory = wave->mLoopHistory;
    info.mYn1 = wave->mYn1;
    info.mYn2 = wave->mYn2;
}
namespace JASystem { namespace WSParser {
/* 02812EE4 */
u32 getGroupCount(void* stream) {
    THeader* header = static_cast<THeader*>(stream);
    return convertControlGroupOffset(stream, header->mCtrlGroupOffset)->mCtrlGroupCount;
}
/* 02812F0C */
TWaveBank* createBasicWaveBank(void* stream) {
    JKRHeap_l* heap = getWaveParserHeap();
    u32 freeSize = gabi::call_ptr<u32>(heap->mVtable->mSize.get(), heap);
    void* allocation = allocateWaveParserObject(0x40, heap, 4);
    if (!allocation)
        return nullptr;
    TWaveBank* bank = constructParsedBasicWaveBank(allocation);
    if (!bank)
        return nullptr;
    THeader* header = static_cast<THeader*>(stream);
    TCtrlGroup* group = convertControlGroupOffset(stream, header->mCtrlGroupOffset);
    setParsedWaveGroupCount(bank, group->mCtrlGroupCount);
    u32 maximum = 0;
    TWaveInfo info;
    for (u32 i = 0; i < group->mCtrlGroupCount; ++i) {
        TCtrlScene* scene = convertControlSceneOffset(stream, group->mCtrlSceneOffsets[i]);
        TCtrl* control = convertControlOffset(stream, scene->mCtrlOffset);
        TWaveArc* waveGroup = getParsedWaveGroup(bank, i);
        TWaveArchiveBank* archiveBank = convertArchiveBankOffset(stream, header->mArchiveBankOffset);
        TWaveArchive* archive = convertWaveArchiveOffset(stream, archiveBank->mArchiveOffsets[i]);
        setParsedGroupWaveCount(waveGroup, control->mWaveCount);
        for (u32 j = 0; j < control->mWaveCount; ++j) {
            TWave* wave = convertWaveOffset(stream, archive->mWaveOffsets[j]);
            fillWaveInfo(info, wave);
            TCtrlWave* ctrlWave = convertControlWaveOffset(stream, control->mCtrlWaveOffsets[j]);
            u32 id = ctrlWave->mWaveId;
            setParsedGroupWaveInfo(waveGroup, j, id, &info);
            if (maximum < id)
                maximum = id;
        }
        waveGroup->setFileName(archive->mFileName);
    }
    setParsedBasicWaveTableSize(bank, maximum + 1);
    u32 after = gabi::call_ptr<u32>(heap->mVtable->mSize.get(), heap);
    sUsedHeapSize = (u32)sUsedHeapSize + (freeSize - after);
    return bank;
}
/* 02813150 */
TWaveBank* createSimpleWaveBank(void* stream) {
    JKRHeap_l* heap = getWaveParserHeap();
    u32 freeSize = gabi::call_ptr<u32>(heap->mVtable->mSize.get(), heap);
    THeader* header = static_cast<THeader*>(stream);
    TCtrlGroup* group = convertControlGroupOffset(stream, header->mCtrlGroupOffset);
    if (group->mCtrlGroupCount != 1)
        return nullptr;
    void* allocation = allocateWaveParserObject(0x7C, heap, 4);
    if (!allocation)
        return nullptr;
    TWaveBank* bank = constructParsedSimpleWaveBank(allocation);
    if (!bank)
        return nullptr;
    TCtrlScene* scene = convertControlSceneOffset(stream, group->mCtrlSceneOffsets[0]);
    TCtrl* control = convertControlOffset(stream, scene->mCtrlOffset);
    TWaveArchiveBank* archiveBank = convertArchiveBankOffset(stream, header->mArchiveBankOffset);
    TWaveArchive* archive = convertWaveArchiveOffset(stream, archiveBank->mArchiveOffsets[0]);
    u32 maximum = 0;
    u32 count = control->mWaveCount;
    for (u32 i = 0; i < count; ++i) {
        TCtrlWave* wave = convertControlWaveOffset(stream, control->mCtrlWaveOffsets[i]);
        u32 id = wave->mWaveId;
        if (maximum < id)
            maximum = id;
    }
    setParsedSimpleWaveTableSize(bank, maximum + 1);
    TWaveInfo info;
    for (u32 i = 0; i < control->mWaveCount; ++i) {
        TWave* wave = convertWaveOffset(stream, archive->mWaveOffsets[i]);
        fillWaveInfo(info, wave);
        TCtrlWave* ctrlWave = convertControlWaveOffset(stream, control->mCtrlWaveOffsets[i]);
        setParsedSimpleWaveInfo(bank, ctrlWave->mWaveId, &info);
    }
    // HD: the simple bank embeds its archive directly after the bank vtable.
    auto arc = reinterpret_cast<TWaveArc*>(bank + 1);
    arc->setFileName(archive->mFileName);
    u32 after = gabi::call_ptr<u32>(heap->mVtable->mSize.get(), heap);
    sUsedHeapSize = (u32)sUsedHeapSize + (freeSize - after);
    return bank;
}
/* 0281338C */
TWave* convertWaveOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<TWave*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
/* 028133A0 */
TWaveArchive* convertWaveArchiveOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<TWaveArchive*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
/* 028133B4 */
TCtrlWave* convertControlWaveOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<TCtrlWave*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
/* 028133C8 */
TCtrl* convertControlOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<TCtrl*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
/* 028133DC */
TCtrlScene* convertControlSceneOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<TCtrlScene*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
/* 028133F0 */
TWaveArchiveBank* convertArchiveBankOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<TWaveArchiveBank*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
/* 02813404 */
TCtrlGroup* convertControlGroupOffset(const void* base, u32 offset) {
    if (!offset)
        return nullptr;
    return reinterpret_cast<TCtrlGroup*>(const_cast<u8*>(static_cast<const u8*>(base)) + offset);
}
}}
/* 02813364 */
void __sinit_JASWSParser_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerWSParserGlobals(&sRegistration);
}
/* WWHD glue
data sUsedHeapSize 101F9E8C
data audioDefaultAdpcmInfo 101F9F9C
ext 028452A4 JKRHeap_l* getWaveParserHeap()
ext 0273B050 void* allocateWaveParserObject(u32, void*, u32)
ext 028448DC TWaveBank* constructParsedBasicWaveBank(void*)
ext 0284501C TWaveBank* constructParsedSimpleWaveBank(void*)
ext 028449B4 void setParsedWaveGroupCount(TWaveBank*, u32)
ext 02844B6C TWaveArc* getParsedWaveGroup(TWaveBank*, u32)
ext 02844E08 void setParsedGroupWaveCount(TWaveArc*, u32)
ext 02844EFC void setParsedGroupWaveInfo(TWaveArc*, u32, u32, const TWaveInfo*)
ext 02844B00 void setParsedBasicWaveTableSize(TWaveBank*, u32)
ext 02845094 void setParsedSimpleWaveTableSize(TWaveBank*, u32)
ext 02845150 void setParsedSimpleWaveInfo(TWaveBank*, u32, const TWaveInfo*)
data sInitialization 104B538C
data sRegistration 101F9E90
ext 028F026C void registerWSParserGlobals(void*)
*/

#ifndef WWHD_JASWSPARSER_H
#define WWHD_JASWSPARSER_H
#include "JSystem/JAudio/JASWaveBankMgr.h"
#include "JSystem/JAudio/JASWaveArcLoader.h"
using namespace gabi;
namespace JASystem {
struct TWaveInfo {
    be<u8> mBlockType, mKey; u8 padding[2];
    be<f32> mSampleRate;
    be<u32> mWaveOffset, mLoopMode, mLoopStart, mLoopEnd, mSampleCount, mLoopHistory;
    be<s16> mYn1, mYn2;
    gptr<void> mAdpcmInfo;
};
namespace WSParser {
struct TCtrl { be<u32> mMagic, mWaveCount; be<u32> mCtrlWaveOffsets[1]; };
struct TCtrlGroup { be<u32> mMagic, mReserved, mCtrlGroupCount; be<u32> mCtrlSceneOffsets[1]; };
struct THeader { u8 prefix[0x10]; be<u32> mArchiveBankOffset, mCtrlGroupOffset; };
u32 getGroupCount(void* stream);
TWaveBank* createBasicWaveBank(void* stream);
TWaveBank* createSimpleWaveBank(void* stream);
struct TCtrlScene { be<u32> mMagic, mReserved[2], mCtrlOffset; };
struct TCtrlWave { be<u16> mReserved, mWaveId; };
struct TWave { be<u8> unused, mBlockType, mKey, padding; be<f32> mSampleRate; be<u32> mWaveOffset, mLoopMode, mLoopStart, mLoopEnd, mSampleCount, mLoopHistory; be<s16> mYn1, mYn2; };
struct TWaveArchive { be<u8> mFileName[0x74]; be<u32> mWaveOffsets[1]; };
struct TWaveArchiveBank { be<u32> mMagic, mReserved; be<u32> mArchiveOffsets[1]; };
TWave* convertWaveOffset(const void* base, u32 offset);
TWaveArchive* convertWaveArchiveOffset(const void* base, u32 offset);
TCtrlWave* convertControlWaveOffset(const void* base, u32 offset);
TCtrl* convertControlOffset(const void* base, u32 offset);
TCtrlScene* convertControlSceneOffset(const void* base, u32 offset);
TWaveArchiveBank* convertArchiveBankOffset(const void* base, u32 offset);
TCtrlGroup* convertControlGroupOffset(const void* base, u32 offset);
}}
WWHD_OFFSET(JASystem::WSParser::THeader, mCtrlGroupOffset, 0x14);
WWHD_SIZE(JASystem::WSParser::THeader, 0x18);
WWHD_OFFSET(JASystem::WSParser::TCtrlGroup, mCtrlGroupCount, 8);
WWHD_SIZE(JASystem::WSParser::TCtrlGroup, 0x10);
WWHD_SIZE(JASystem::TWaveInfo, 0x28);
WWHD_OFFSET(JASystem::TWaveInfo, mAdpcmInfo, 0x24);
WWHD_SIZE(JASystem::WSParser::TWave, 0x24);
WWHD_OFFSET(JASystem::WSParser::TWaveArchive, mWaveOffsets, 0x74);
// The default ADPCM info word (101F9F9C) the wave parser points new waves at; JS-JAudio#5 handle constructors store its address.
extern be<u32>& audioDefaultAdpcmInfo;
#endif

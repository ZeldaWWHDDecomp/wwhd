#ifndef WWHD_JASBNKPARSER_H
#define WWHD_JASBNKPARSER_H
#include "JSystem/JAudio/JASBank.h"
using namespace gabi;
namespace JASystem { namespace BNKParser {
struct TOsc { be<u8> mTarget; u8 padding[3]; be<f32> mRate; be<u32> mTableOffset, mReleaseOffset; be<f32> mAmplitude, mBias; };
struct TRand { be<u8> mTarget; u8 padding[3]; be<f32> mBase, mWidth; };
struct TSense { be<u8> mTarget, mTrigger, mCenter; u8 padding; be<f32> mLow, mHigh; };
struct TVmap { be<u8> mBaseVelocity; u8 padding[3]; be<u32> mWaveId; be<f32> mVolume, mPitch; };
struct TKeymap { be<u8> mBaseKey; u8 padding[3]; be<u32> mVelocityCount; be<u32> mVelocityOffsets[1]; };
struct TInst { u8 prefix[8]; be<f32> mVolume, mPitch; be<u32> mOscOffsets[2], mRandomOffsets[2], mSenseOffsets[2], mKeyCount, mKeyOffsets[1]; };
struct TPmap { be<f32> mVolume, mPitch; be<u32> mRandomOffsets[2], mVelocityCount, mVelocityOffsets[1]; };
struct TPerc { be<u32> mMagic; u8 padding[0x84]; be<u32> mPercussionOffsets[128]; be<s8> mPan[128]; be<u16> mRelease[128]; };
struct THeader { u8 prefix[0x24]; be<u32> mInstOffsets[128]; u8 padding[0x190]; be<u32> mPercussionOffsets[12]; };
TBasicBank* createBasicBank(void* stream);
be<s16>* getOscTableEndPtr(be<s16>* table);
be<s16>* convertOscTableOffset(const void* base, u32 offset);
TVmap* convertVelocityMapOffset(const void* base, u32 offset);
TOsc* convertOscillatorOffset(const void* base, u32 offset);
TRand* convertRandomOffset(const void* base, u32 offset);
TSense* convertSenseOffset(const void* base, u32 offset);
TKeymap* convertKeymapOffset(const void* base, u32 offset);
TPmap* convertPercussionMapOffset(const void* base, u32 offset);
TInst* convertInstrumentOffset(const void* base, u32 offset);
TPerc* convertPercussionOffset(const void* base, u32 offset);
}}
WWHD_OFFSET(JASystem::BNKParser::THeader, mPercussionOffsets, 0x3B4);
WWHD_OFFSET(JASystem::BNKParser::TInst, mKeyCount, 0x28);
WWHD_OFFSET(JASystem::BNKParser::TPerc, mPan, 0x288);
WWHD_OFFSET(JASystem::BNKParser::TPerc, mRelease, 0x308);
#endif

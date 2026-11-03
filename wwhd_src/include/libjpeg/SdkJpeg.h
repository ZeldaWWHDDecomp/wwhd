/* SDK JPEG wrapper views reconstructed from this game's field accesses only. */
#pragma once
#include "wwhd.h"
namespace sdkjpeg {
struct SdkJpegComponentInfo {
    be<u8> mIdentifier; be<u8> mHorizontalSampling; be<u8> mVerticalSampling; be<u8> mQuantizerIndex;
    be<u32> mHorizontalStep; be<u32> mVerticalStep; be<u32> mColumnCount; be<u32> mRowCount;
};
struct SdkJpegImageInfo {
    be<u16> mFrameMarker; be<u16> mFrameLength; be<u8> mSamplePrecision; u8 mReserved; be<u16> mHeight; be<u16> mWidth;
    be<u8> mComponentCount; u8 mAlignment;
    be<u16> mMaxHorizontalSampling; be<u16> mMaxVerticalSampling;
    SdkJpegComponentInfo mComponents[4];
    be<u32> mBlockSize; be<u32> mReservedBlock;
    be<u32> mHuffmanCount; be<u32> mHuffmanMask;
    be<u32> mQuantizerCount; be<u32> mQuantizerMask;
    be<u32> mHorizontalDivisor; be<u32> mVerticalDivisor;
    be<u32> mOutputWidth; be<u32> mOutputHeight; be<u32> mSpectralEnd;
    be<u32> mScanCount; be<u32> mOutputChannels; be<u32> mOutputMode;
    be<u32> mInputColorMode; be<u32> mOutputColorMode; be<u32> mJfifPresent; be<u32> mAdobePresent; be<u32> mColorReady; be<u32> mTableSelector;
    be<u8> mFlags; be<u8> mAuxiliaryTableCount; u8 mTail[2];
    void deriveComponents();
    u32 tableStorageSize() const;
    u32 decoderControlStorageSize() const;
    u32 decoderPlaneStorageSize() const;
    u32 outputStorageSize() const;
    u32 encoderPlaneStorageSize() const;
};
struct SdkJpegFailure;
struct SdkJpegEncodeControl {
    gptr<SdkJpegFailure> mFailure; u8 mPrefix[0x14]; gptr<void> mHeap; u8 mHeapReserved[4];
    be<u32> mWidth; be<u32> mHeight; be<u32> mInputChannels; be<u32> mInputColorMode;
    u8 mBody[0xA8]; be<u32> mScanPosition; be<u32> mReservedScan;
    be<u32> mInputPosition; be<u32> mReservedInput; be<u32> mReady;
    u8 mReadyReserved[0x18]; be<u32> mOutputLine; u8 mTail[0xA8];
    void prepare(u32 quality);
};
struct SdkJpegDecodeControl {
    gptr<SdkJpegFailure> mFailure; u8 mPrefix[0x14]; gptr<void> mHeap; u8 mHeapReserved[0x14];
    be<u32> mMode; be<u32> mScaleNumerator; be<u32> mScaleDenominator;
    u8 mReserved[0x14]; be<u32> mFirstCount; be<u32> mSecondCount; be<u32> mThirdCount;
    u8 mOutputReserved[0x1C]; be<u32> mOutputWidth; be<u32> mOutputHeight; be<u32> mOutputChannels;
    u8 mLineReserved[0x10]; be<u32> mOutputLine; u8 mTail[0x158];
    SdkJpegDecodeControl* setScale(u32 denominator);
};
struct SdkJpegRecovery { u8 mBytes[0x1A0]; };
struct SdkJpegFailure {
    be<u32> mCallback; u8 mPrefix[0x10]; be<u32> mCode; be<u32> mDetail;
    u8 mReserved[0x6C]; SdkJpegRecovery mRecovery; be<s32> mResult;
};
struct SdkJpegRows { gptr<be<u8>> mRows[4]; };
/* Stack state of the encode/decode entry points, placed at sp+8 of the original's frame
 * (below it the linkage words, above it the saved registers). */
struct SdkJpegEncodeFrame {
    be<s32> mResult; be<u32> mCapacity; gptr<be<u8>> mOutput;
    SdkJpegRows mRows; u8 mAlignment[4]; SdkJpegEncodeControl mCodec; SdkJpegFailure mFailure;
    u8 mReserved[4];
};
struct SdkJpegDecodeFrame {
    be<s32> mResult; SdkJpegRows mRows; u8 mAlignment[4];
    SdkJpegDecodeControl mCodec; SdkJpegFailure mFailure; u8 mReserved[8];
};
/* Frame offset of both stack states (sp+8). */
static const u32 kSdkJpegFrameOffset = 8;
WWHD_SIZE(SdkJpegEncodeControl, 0x1B0);
WWHD_OFFSET(SdkJpegEncodeControl, mHeap, 0x18);
WWHD_OFFSET(SdkJpegEncodeControl, mOutputLine, 0x104);
WWHD_SIZE(SdkJpegDecodeControl, 0x1F0);
WWHD_OFFSET(SdkJpegDecodeControl, mHeap, 0x18);
WWHD_OFFSET(SdkJpegDecodeControl, mOutputWidth, 0x78);
WWHD_OFFSET(SdkJpegDecodeControl, mOutputLine, 0x94);
WWHD_SIZE(SdkJpegFailure, 0x22C);
/* Offsets relative to the state at sp+8: frame offset = value + 8. */
WWHD_OFFSET(SdkJpegEncodeFrame, mCapacity, 0x4);
WWHD_OFFSET(SdkJpegEncodeFrame, mOutput, 0x8);
WWHD_OFFSET(SdkJpegEncodeFrame, mRows, 0xC);
WWHD_OFFSET(SdkJpegEncodeFrame, mCodec, 0x20);
WWHD_OFFSET(SdkJpegEncodeFrame, mFailure, 0x1D0);
WWHD_SIZE(SdkJpegEncodeFrame, 0x400);
WWHD_OFFSET(SdkJpegDecodeFrame, mRows, 0x4);
WWHD_OFFSET(SdkJpegDecodeFrame, mCodec, 0x18);
WWHD_OFFSET(SdkJpegDecodeFrame, mFailure, 0x208);
WWHD_SIZE(SdkJpegDecodeFrame, 0x43C);
struct SdkJpegFailureContext { gptr<SdkJpegFailure> mFailure; };
WWHD_SIZE(SdkJpegComponentInfo, 0x14);
WWHD_OFFSET(SdkJpegImageInfo, mComponents, 0x10);
WWHD_OFFSET(SdkJpegImageInfo, mHuffmanCount, 0x68);
WWHD_OFFSET(SdkJpegImageInfo, mQuantizerCount, 0x70);
WWHD_OFFSET(SdkJpegImageInfo, mOutputWidth, 0x80);
WWHD_OFFSET(SdkJpegImageInfo, mTableSelector, 0xAC);
WWHD_SIZE(SdkJpegImageInfo, 0xB4);
WWHD_OFFSET(SdkJpegEncodeControl, mScanPosition, 0xD8);
WWHD_OFFSET(SdkJpegEncodeControl, mReady, 0xE8);
WWHD_OFFSET(SdkJpegDecodeControl, mScaleDenominator, 0x38);
WWHD_OFFSET(SdkJpegDecodeControl, mFirstCount, 0x50);
WWHD_OFFSET(SdkJpegFailure, mRecovery, 0x88);
WWHD_OFFSET(SdkJpegFailure, mResult, 0x228);
}

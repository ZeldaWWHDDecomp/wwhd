/* SDK JPEG wrapper and header utilities; reconstructed from the local binary only.
 * No IJG implementation or external layout reference was used for this unit. */
#include "libjpeg/SdkJpeg.h"
using namespace sdkjpeg;
struct FailureCategoryTable { be<s8> mValues[0x100]; };
static FailureCategoryTable sFailureCategories;
struct MarkerResultRange { be<u16> mFirst; be<u16> mLast; be<u32> mResult; };
struct MarkerResultTable { MarkerResultRange mRanges[16]; };
static MarkerResultTable sMarkerResults;
extern void resumeSdkJpegFailure(SdkJpegRecovery*, s32);
extern void initializeSdkJpegEncoding(SdkJpegEncodeControl*);
extern void selectSdkJpegEncoding(SdkJpegEncodeControl*, u32);
extern void setSdkJpegQuality(SdkJpegEncodeControl*, u32, u32);

/* 0284B9A8 */
static void reportSdkJpegFailure(SdkJpegFailureContext* context) {
    SdkJpegFailure* failure = context->mFailure;
    u32 code = failure->mCode;
    s32 result = -19;
    if (code == 39 || code == 72) result = -2;
    else if (code == 4 || code == 14 || code == 36) result = -3;
    else if ((code >= 16 && code <= 20) || (code >= 24 && code <= 33)) result = sFailureCategories.mValues[code + 8];
    else if (code >= 60 && code <= 70) result = sFailureCategories.mValues[code];
    else if (code >= 41 && code < 56) result = sFailureCategories.mValues[code + 3];
    else if (code >= 6 && code <= 12) result = sFailureCategories.mValues[code + 10];
    else if (code == 56) result = failure->mDetail == 132 ? -7 : -6;
    failure->mResult = result;
    resumeSdkJpegFailure(&failure->mRecovery, failure->mResult);
}

/* 0284BB4C */
void sdkjpeg::SdkJpegEncodeControl::prepare(u32 quality) {
    initializeSdkJpegEncoding(this);
    selectSdkJpegEncoding(this, 3);
    setSdkJpegQuality(this, quality, 1);
    mReady = 1;
    mInputPosition = 0;
    mScanPosition = 0;
}

/* 0284BE64 */
sdkjpeg::SdkJpegDecodeControl* sdkjpeg::SdkJpegDecodeControl::setScale(u32 denominator) {
    mScaleDenominator = denominator;
    mScaleNumerator = 1;
    mThirdCount = 0;
    mMode = 2;
    mSecondCount = 0;
    mFirstCount = 0;
    return this;
}

/* 0284C1C4 */
static s32 validateSdkJpegStart(const be<u8>* data) {
    if (data == NULL) return -1;
    if (data[0] != 0xFF) return -1;
    if (data[1] != 0xD8) return -1;
    return 0;
}

/* 0284C218 */
static u32 sdkJpegMarkerResult(u32 marker) {
    for (u32 index = 0; index < 16; ++index) {
        if (marker >= (u32)sMarkerResults.mRanges[index].mFirst && marker <= (u32)sMarkerResults.mRanges[index].mLast) return sMarkerResults.mRanges[index].mResult;
    }
    return 5;
}

/* 0284D8AC */
u32 sdkjpeg::SdkJpegImageInfo::tableStorageSize() const {
    if (this == NULL) return 0;
    u32 remaining = 0x610;
    u32 size = 0x700;
    for (u32 index = 0; index < (u32)mQuantizerCount; ++index) {
        if (remaining < 0x88) size += 0x98;
        else remaining -= 0x88;
    }
    for (u32 index = 0; index < (u32)mHuffmanCount; ++index) {
        if (remaining < 0x118) size += 0x128;
        else remaining -= 0x118;
    }
    return size;
}

/* 0284DC9C */
u32 sdkjpeg::SdkJpegImageInfo::outputStorageSize() const {
    return (u32)mOutputWidth * (u32)mOutputChannels * 4;
}

/* 0284DD10 */
static u32 sdkJpegEncoderControlSize() { return 0x48A8; }

/* 0284DD18 */
u32 sdkjpeg::SdkJpegImageInfo::encoderPlaneStorageSize() const {
    u32 size = 0x510;
    for (u32 index = 0; index < (u32)mComponentCount; ++index) {
        const SdkJpegComponentInfo* component = &mComponents[index];
        u32 rowSize = (u32)component->mColumnCount * (u32)mHorizontalDivisor;
        rowSize *= (u32)mMaxHorizontalSampling;
        rowSize /= (u32)component->mHorizontalSampling;
        rowSize *= (u32)mMaxVerticalSampling;
        u32 planeSize = (u32)component->mColumnCount * (u32)component->mHorizontalStep;
        planeSize *= (u32)component->mVerticalSampling;
        planeSize *= (u32)component->mVerticalStep;
        size += ((rowSize + 7) & ~7u) + 0x10;
        size += ((planeSize + 7) & ~7u) + 0x10;
    }
    return size;
}

/* 0284DDA8 */
static u32 sdkJpegPixelRowSize(u32 width) { return width * 12; }

struct SdkJpegMarkerPair { be<u16> mLength; be<u16> mMarker; };
struct SdkJpegOffset { be<u32> mValue; };
extern void copySdkJpegBytes(void*, const void*, u32);

/* 0284C260 */
static s32 findSdkJpegMarker(const be<u8>* data, u32 size, u32 start, SdkJpegOffset* output, u32 wanted) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<SdkJpegMarkerPair> pair(0x8);
    pair->mMarker = 0;
    if (data == NULL || output == NULL) return -1;
    output->mValue = 0;
    u32 consumed = 0;
    while (start + consumed < size) {
        u32 chunk = size - (start + consumed);
        if ((s32)chunk > 256) chunk = 256;
        pair->mLength = 0;
        const be<u8>* found = NULL;
        for (u32 index = 0; index < chunk; ++index) {
            const be<u8>* byte = data + start + consumed + index;
            if ((u32)*byte == 0xFF) { found = byte; break; }
        }
        if (found == NULL) { consumed += chunk; continue; }
        u32 offset = (u32)(found - data);
        if (offset + 2 > size) break;
        copySdkJpegBytes(&pair->mMarker, found, 2);
        u32 kind = sdkJpegMarkerResult(pair->mMarker);
        u32 length;
        if (kind == 4) break;
        if (kind == 1) {
            if (offset + 4 > size) break;
            copySdkJpegBytes(&pair->mLength, found + 2, 2);
            length = (u16)((u32)pair->mLength + 2);
            pair->mLength = length;
            if (length < 4 || offset + length > size) break;
        } else {
            length = kind == 2 ? 1 : 2;
            pair->mLength = length;
        }
        u32 marker = pair->mMarker;
        bool skipApplication = false;
        if ((marker & 0xFFF0) == 0xFFE0 && marker == wanted) {
            gabi::FrameLocal<SdkJpegOffset> nested(0xC);
            nested->mValue = 0;
            skipApplication = findSdkJpegMarker(found + 4, length - 2, 0, nested.get(), 0xFFD8) == 0;
        }
        if (!skipApplication) {
            marker = pair->mMarker;
            if (marker == wanted) { output->mValue = offset; return 0; }
            if (marker == 0xFFD9) break;
        }
        consumed = offset + (u32)pair->mLength - start;
    }
    return -1;
}

/* 0284D304 */
static s32 validateSdkJpegEnd(const be<u8>* data, u32 size) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<SdkJpegOffset> offset(0x8);
    offset->mValue = 0;
    return findSdkJpegMarker(data, size, 0, offset.get(), 0xFFD9) == 0 ? 0 : -1;
}

struct SdkJpegFrameMarkers { be<u16> mValues[5]; };
static SdkJpegFrameMarkers sFrameMarkers;

/* 0284D244 */
static s32 findSdkJpegFrame(const be<u8>* data, u32 size, be<u16>* marker) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    *marker = 0;
    gabi::FrameLocal<SdkJpegOffset> offset(0x8);
    for (u32 index = 0; index < 5; ++index) {
        offset->mValue = 0;
        if (findSdkJpegMarker(data, size, 0, offset.get(), sFrameMarkers.mValues[index]) == 0) {
            *marker = sFrameMarkers.mValues[index];
            return 0;
        }
    }
    return -1;
}

extern u32 roundSdkJpegCount(u32, u32);

/* 0284D934 */
u32 sdkjpeg::SdkJpegImageInfo::decoderControlStorageSize() const {
    if (this == NULL) return 0;
    u32 size = mInputColorMode == 2 || mInputColorMode == 4 ? 0x17A4 : 0x7A4;
    size += mOutputMode == 0 ? 0xB8 : 0x30;
    u32 componentBytes = mFrameMarker == 0xFFC0 ? 0x158 : 0x294;
    size += componentBytes * (u32)mComponentCount;
    size += (u32)mAuxiliaryTableCount * 0x84;
    size += (u32)mScanCount * 0x590;
    for (u32 index = 0; index < (u32)mComponentCount; ++index) {
        const SdkJpegComponentInfo* component = &mComponents[index];
        size += (u32)component->mVerticalSampling * (u32)component->mVerticalStep * 4;
        if (mFrameMarker != 0xFFC0) size += roundSdkJpegCount(component->mRowCount, component->mVerticalSampling) * 4;
        u32 horizontal = (u32)component->mHorizontalSampling * (u32)component->mHorizontalStep / (u32)mHorizontalDivisor;
        u32 vertical = (u32)component->mVerticalSampling * (u32)component->mVerticalStep / (u32)mVerticalDivisor;
        u32 maximumVertical = mMaxVerticalSampling;
        u32 outputMode = mOutputMode;
        if (outputMode == 0 && (horizontal != (u32)mMaxHorizontalSampling || vertical != maximumVertical)) size += maximumVertical * 4;
    }
    u32 floorSize = (u32)mComponentCount * 0x58 + 0x3E90;
    if (size < floorSize) return floorSize;
    do { floorSize += 0x1388; } while (floorSize <= size);
    return floorSize;
}

/* 0284DAF4 */
u32 sdkjpeg::SdkJpegImageInfo::decoderPlaneStorageSize() const {
    if (this == NULL) return 0;
    u32 size = 0;
    for (u32 index = 0; index < (u32)mComponentCount; ++index) {
        const SdkJpegComponentInfo* component = &mComponents[index];
        u32 columns = component->mColumnCount;
        u32 horizontalStep = component->mHorizontalStep;
        u32 plane = (u32)component->mVerticalSampling * columns * horizontalStep * (u32)component->mVerticalStep;
        size += ((plane + 7) & ~7u) + 0x10;
        u32 horizontal = (u32)component->mHorizontalSampling * horizontalStep / (u32)mHorizontalDivisor;
        u32 vertical = (u32)component->mVerticalSampling * (u32)component->mVerticalStep / (u32)mVerticalDivisor;
        if (mOutputMode == 0 && (horizontal != (u32)mMaxHorizontalSampling || vertical != (u32)mMaxVerticalSampling)) {
            u32 row = roundSdkJpegCount(mOutputWidth, (u32)mMaxHorizontalSampling) * (u32)mMaxVerticalSampling;
            size += ((row + 7) & ~7u) + 0x10;
        }
    }
    if (mFrameMarker != 0xFFC0) {
        for (u32 index = 0; index < (u32)mComponentCount; ++index) {
            const SdkJpegComponentInfo* component = &mComponents[index];
            u32 columns = roundSdkJpegCount(component->mColumnCount, component->mHorizontalSampling);
            u32 rows = roundSdkJpegCount(component->mRowCount, component->mVerticalSampling);
            u32 coefficients = columns * rows * 128;
            size += ((coefficients + 7) & ~7u) + 0x10;
        }
    } else if (mTableSelector == 0) size += 0x510;
    if (mMaxVerticalSampling == 2 && mOutputMode == 1) {
        u32 output = (u32)mOutputWidth * (u32)mOutputChannels;
        size += ((output + 7) & ~7u) + 0x10;
    }
    return size;
}

struct SdkJpegByte { be<u8> mValue; };

/* 0284C468 */
static s32 readSdkJpegFrame(const be<u8>* data, u32 size, const SdkJpegImageInfo* tables, SdkJpegImageInfo* info, u32 wanted) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<SdkJpegOffset> offset(0x8);
    offset->mValue = 0;
    if (data == NULL || info == NULL) return -1;
    if (findSdkJpegMarker(data, size, 0, offset.get(), wanted) != 0) return -1;
    if ((u32)offset->mValue + 4 > size) return -1;
    copySdkJpegBytes(&info->mFrameMarker, data + (u32)offset->mValue, 2);
    offset->mValue = (u32)offset->mValue + 2;
    copySdkJpegBytes(&info->mFrameLength, data + (u32)offset->mValue, 2);
    if ((u32)info->mFrameLength < 8 || (u32)offset->mValue + (u32)info->mFrameLength > size) return -1;
    offset->mValue = (u32)offset->mValue + 2;
    copySdkJpegBytes(&info->mSamplePrecision, data + (u32)offset->mValue, 1);
    offset->mValue = (u32)offset->mValue + 1;
    copySdkJpegBytes(&info->mHeight, data + (u32)offset->mValue, 2);
    offset->mValue = (u32)offset->mValue + 2;
    copySdkJpegBytes(&info->mWidth, data + (u32)offset->mValue, 2);
    offset->mValue = (u32)offset->mValue + 2;
    copySdkJpegBytes(&info->mComponentCount, data + (u32)offset->mValue, 1);
    offset->mValue = (u32)offset->mValue + 1;
    u32 count = info->mComponentCount;
    if (count == 0 || count > 4 || (u32)info->mFrameLength != count * 3 + 8) return -1;
    info->mMaxHorizontalSampling = 0;
    info->mMaxVerticalSampling = 0;
    for (u32 index = 0; index < (u32)info->mComponentCount; ++index) {
        SdkJpegComponentInfo* component = &info->mComponents[index];
        gabi::FrameLocal<SdkJpegByte> sampling(0xC);
        sampling->mValue = 0;
        copySdkJpegBytes(&component->mIdentifier, data + (u32)offset->mValue, 1);
        offset->mValue = (u32)offset->mValue + 1;
        copySdkJpegBytes(sampling.get(), data + (u32)offset->mValue, 1);
        offset->mValue = (u32)offset->mValue + 1;
        u32 horizontal = (u32)sampling->mValue >> 4;
        u32 vertical = (u32)sampling->mValue & 15;
        component->mHorizontalSampling = horizontal;
        component->mVerticalSampling = vertical;
        if (horizontal == 0 || vertical == 0) return -1;
        if ((u32)info->mMaxHorizontalSampling > horizontal) horizontal = info->mMaxHorizontalSampling;
        u32 maximumVertical = info->mMaxVerticalSampling;
        info->mMaxHorizontalSampling = horizontal;
        if (maximumVertical > vertical) vertical = maximumVertical;
        info->mMaxVerticalSampling = vertical;
        copySdkJpegBytes(&component->mQuantizerIndex, data + (u32)offset->mValue, 1);
        u32 shift = (u32)component->mQuantizerIndex & 63;
        u32 mask = shift < 32 ? 1u << shift : 0;
        u32 available = tables->mQuantizerMask;
        offset->mValue = (u32)offset->mValue + 1;
        if ((available & mask) == 0) return -1;
    }
    return info->mMaxHorizontalSampling != 0 && info->mMaxVerticalSampling != 0 ? 0 : -1;
}

struct SdkJpegQuantizerScratch { SdkJpegByte mFormat; u8 mAlignment; be<u16> mLength; SdkJpegOffset mOffset; };

/* 0284CE5C */
static s32 countSdkJpegQuantizers(const be<u8>* data, u32 size, SdkJpegImageInfo* info) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<SdkJpegQuantizerScratch> scratch(0x8);
    scratch->mLength = 0;
    scratch->mOffset.mValue = 0;
    scratch->mFormat.mValue = 0;
    if (data == NULL || info == NULL) return -1;
    u32 count = 0;
    u32 mask = 0;
    s32 status = findSdkJpegMarker(data, size, 0, &scratch->mOffset, 0xFFDB);
    while (status == 0) {
        if ((u32)scratch->mOffset.mValue + 4 > size) return -1;
        scratch->mOffset.mValue = (u32)scratch->mOffset.mValue + 2;
        copySdkJpegBytes(&scratch->mLength, data + (u32)scratch->mOffset.mValue, 2);
        u32 length = scratch->mLength;
        if (length < 2 || (u32)scratch->mOffset.mValue + length > size) return -1;
        u32 remaining = length - 2;
        scratch->mOffset.mValue = (u32)scratch->mOffset.mValue + 2;
        while (remaining > 1) {
            copySdkJpegBytes(&scratch->mFormat, data + (u32)scratch->mOffset.mValue, 1);
            u32 format = scratch->mFormat.mValue;
            u32 index = format & 15;
            if (index >= 4) return -1;
            u32 bit = 1u << index;
            if ((mask & bit) == 0) { mask |= bit; ++count; }
            u32 tableSize = (format >> 4) * 64 + 65;
            remaining -= tableSize;
            scratch->mOffset.mValue = (u32)scratch->mOffset.mValue + tableSize;
        }
        if (remaining != 0) return -1;
        status = findSdkJpegMarker(data, size, scratch->mOffset.mValue, &scratch->mOffset, 0xFFDB);
    }
    if (count == 0) return -1;
    info->mQuantizerCount = count;
    info->mQuantizerMask = mask;
    return 0;
}

struct SdkJpegScanComponent { be<u8> mIdentifier; be<u8> mTables; };
struct SdkJpegScanHeader {
    be<u16> mMarker; be<u16> mLength; be<u8> mComponentCount;
    SdkJpegScanComponent mComponents[4];
    be<u8> mFirstCoefficient; be<u8> mLastCoefficient;
    be<u8> mApproximationHigh; be<u8> mApproximationLow; u8 mReserved;
};

/* 0284C6E4 */
static s32 readSdkJpegScan(const be<u8>* data, u32 size, u32 start, SdkJpegOffset* output, const SdkJpegImageInfo* info, SdkJpegScanHeader* scan) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<SdkJpegOffset> offset(0x8);
    gabi::FrameLocal<SdkJpegByte> approximation(0xC);
    offset->mValue = 0;
    approximation->mValue = 0;
    if (data == NULL || output == NULL || info == NULL || scan == NULL) return -1;
    if (findSdkJpegMarker(data, size, start, offset.get(), 0xFFDA) != 0) return -1;
    if ((u32)offset->mValue + 4 > size) return -1;
    copySdkJpegBytes(&scan->mMarker, data + (u32)offset->mValue, 2);
    offset->mValue = (u32)offset->mValue + 2;
    copySdkJpegBytes(&scan->mLength, data + (u32)offset->mValue, 2);
    u32 length = scan->mLength;
    if (length < 6 || (u32)offset->mValue + length > size) return -1;
    offset->mValue = (u32)offset->mValue + 2;
    copySdkJpegBytes(&scan->mComponentCount, data + (u32)offset->mValue, 1);
    offset->mValue = (u32)offset->mValue + 1;
    if ((u32)scan->mComponentCount > 4 || (u32)scan->mLength != (u32)scan->mComponentCount * 2 + 6) return -1;
    for (u32 index = 0; index < (u32)scan->mComponentCount; ++index) {
        SdkJpegScanComponent* component = &scan->mComponents[index];
        copySdkJpegBytes(&component->mIdentifier, data + (u32)offset->mValue, 1);
        offset->mValue = (u32)offset->mValue + 1;
        bool found = false;
        for (u32 frame = 0; frame < (u32)info->mComponentCount; ++frame) {
            if ((u32)info->mComponents[frame].mIdentifier == (u32)component->mIdentifier) { found = true; break; }
        }
        copySdkJpegBytes(&component->mTables, data + (u32)offset->mValue, 1);
        offset->mValue = (u32)offset->mValue + 1;
        if (!found) return -1;
    }
    copySdkJpegBytes(&scan->mFirstCoefficient, data + (u32)offset->mValue, 1);
    offset->mValue = (u32)offset->mValue + 1;
    copySdkJpegBytes(&scan->mLastCoefficient, data + (u32)offset->mValue, 1);
    offset->mValue = (u32)offset->mValue + 1;
    copySdkJpegBytes(approximation.get(), data + (u32)offset->mValue, 1);
    u32 high = (u32)approximation->mValue >> 4;
    u32 low = (u32)approximation->mValue & 15;
    scan->mApproximationHigh = high;
    scan->mApproximationLow = low;
    if ((u32)scan->mFirstCoefficient > 63 || (u32)scan->mLastCoefficient > 63 || high > 13 || low > 13) return -1;
    output->mValue = (u32)offset->mValue + 1;
    return 0;
}

struct SdkJpegHuffmanScratch { SdkJpegByte mFormat; SdkJpegByte mCount; be<u16> mLength; SdkJpegOffset mOffset; };

/* 0284C990 */
static s32 countSdkJpegHuffmanTables(const be<u8>* data, u32 size, SdkJpegImageInfo* info) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<SdkJpegHuffmanScratch> scratch(0x8);
    scratch->mCount.mValue = 0;
    scratch->mLength = 0;
    scratch->mOffset.mValue = 0;
    scratch->mFormat.mValue = 0;
    if (data == NULL || info == NULL) return -1;
    u32 count = 0;
    u32 mask = 0;
    s32 status = findSdkJpegMarker(data, size, 0, &scratch->mOffset, 0xFFC4);
    while (status == 0) {
        if ((u32)scratch->mOffset.mValue + 4 > size) return -1;
        scratch->mOffset.mValue = (u32)scratch->mOffset.mValue + 2;
        copySdkJpegBytes(&scratch->mLength, data + (u32)scratch->mOffset.mValue, 2);
        u32 length = scratch->mLength;
        if (length < 2 || (u32)scratch->mOffset.mValue + length > size) return -1;
        u32 remaining = length - 2;
        scratch->mOffset.mValue = (u32)scratch->mOffset.mValue + 2;
        while (remaining > 1) {
            if ((u32)scratch->mOffset.mValue + 17 > size) return -1;
            copySdkJpegBytes(&scratch->mFormat, data + (u32)scratch->mOffset.mValue, 1);
            u32 format = scratch->mFormat.mValue;
            if (((format >> 4) & 14) != 0 || (format & 15) >= 4) return -1;
            u32 bit = 1u << format;
            if ((mask & bit) == 0) { mask |= bit; ++count; }
            u32 tableSize = 17;
            for (u32 index = 0; index < 16; ++index) {
                copySdkJpegBytes(&scratch->mCount, data + (u32)scratch->mOffset.mValue + index + 1, 1);
                tableSize += (u32)scratch->mCount.mValue;
            }
            remaining -= tableSize;
            scratch->mOffset.mValue = (u32)scratch->mOffset.mValue + tableSize;
        }
        if (remaining != 0) return -1;
        status = findSdkJpegMarker(data, size, scratch->mOffset.mValue, &scratch->mOffset, 0xFFC4);
    }
    if (count == 0) return -1;
    info->mHuffmanCount = count;
    info->mHuffmanMask = mask;
    return 0;
}

/* 0284CB54 */
static s32 readSdkJpegBlockSize(const be<u8>* data, u32 size, SdkJpegImageInfo* info) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<SdkJpegOffset> offset(0x8);
    gabi::FrameLocal<SdkJpegScanHeader> scan(0xC);
    offset->mValue = 0;
    if (data == NULL || info == NULL) return -1;
    if (readSdkJpegScan(data, size, 0, offset.get(), info, scan.get()) != 0) return -1;
    u32 count = scan->mComponentCount;
    u32 frameCount = info->mComponentCount;
    info->mAuxiliaryTableCount = count;
    if (count < frameCount) info->mTableSelector = 1;
    if (count != 0) {
        info->mSpectralEnd = 63;
        info->mBlockSize = 8;
        return 0;
    }
    for (u32 dimension = 1; dimension < 16; ++dimension) {
        u32 last = scan->mLastCoefficient;
        if (last == dimension * dimension - 1) {
            info->mSpectralEnd = dimension >= 8 ? 63 : last;
            info->mBlockSize = dimension;
            return 0;
        }
    }
    return -1;
}

extern void fillSdkJpegBytes(void*, u32, u32);

/* 0284CC70 */
static s32 countSdkJpegActiveTables(const be<u8>* data, u32 size, SdkJpegImageInfo* info) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<SdkJpegOffset> offset(0x8);
    gabi::FrameLocal<SdkJpegScanHeader> scan(0xC);
    offset->mValue = 0;
    if (data == NULL || info == NULL) return -1;
    u32 count = 0;
    u32 mask = 0;
    fillSdkJpegBytes(scan.get(), 0, 18);
    u32 status = (u16)readSdkJpegScan(data, size, 0, offset.get(), info, scan.get());
    while (status == 0) {
        u32 marker = info->mFrameMarker;  // read once per scan, before the component loop, as the original does (0284CCDC)
        for (u32 index = 0; index < (u32)scan->mComponentCount; ++index) {
            u32 tables = scan->mComponents[index].mTables;
            if (marker != 0xFFC0) {
                if (scan->mFirstCoefficient == 0) {
                    if (scan->mApproximationHigh != 0) continue;
                    u32 bit = 1u << (tables >> 4);
                    if ((mask & bit) == 0) { mask = (mask | bit) & 255; ++count; }
                } else {
                    u32 bit = 1u << (tables & 15);
                    if ((mask & bit) == 0) { mask = (mask | bit) & 255; ++count; }
                }
            } else {
                u32 bit = 1u << (tables >> 4);
                if ((mask & bit) == 0) { mask = (mask | bit) & 255; ++count; }
                if (info->mSpectralEnd != 0) {
                    bit = 1u << ((tables & 15) + 4);
                    if ((mask & bit) == 0) { mask = (mask | bit) & 255; ++count; }
                }
            }
        }
        fillSdkJpegBytes(scan.get(), 0, 18);
        status = (u16)readSdkJpegScan(data, size, offset->mValue, offset.get(), info, scan.get());
    }
    if (count == 0) return -1;
    info->mScanCount = count;
    return 0;
}

extern u32 divideSdkJpegExtent(u32, u32);

/* 0284CFE0 */
void sdkjpeg::SdkJpegImageInfo::deriveComponents() {
    u32 block = mBlockSize;
    mHorizontalDivisor = block;
    mVerticalDivisor = block;
    for (u32 index = 0; index < (u32)mComponentCount; ++index) {
        SdkJpegComponentInfo* component = &mComponents[index];
        u32 factor = 1;
        u32 horizontal;
        for (;;) {
            horizontal = (u32)mHorizontalDivisor * factor;
            if (horizontal > 4) break;
            u32 denominator = (u32)component->mHorizontalSampling * factor;
            u32 maximum = mMaxHorizontalSampling;
            if ((s32)maximum % (s32)denominator != 0) break;
            factor *= 2;
        }
        component->mHorizontalStep = horizontal;
        factor = 1;
        u32 vertical;
        for (;;) {
            vertical = (u32)mVerticalDivisor * factor;
            if (vertical > 4) break;
            u32 denominator = (u32)component->mVerticalSampling * factor;
            u32 maximum = mMaxVerticalSampling;
            if ((s32)maximum % (s32)denominator != 0) break;
            factor *= 2;
        }
        component->mVerticalStep = vertical;
        if ((s32)horizontal > (s32)(vertical * 2)) component->mHorizontalStep = vertical * 2;
        else if ((s32)vertical > (s32)(horizontal * 2)) component->mVerticalStep = horizontal * 2;
        u32 columns = (u32)mWidth * (u32)component->mHorizontalSampling;
        u32 columnUnit = (u32)mMaxHorizontalSampling * (u32)mBlockSize;
        component->mColumnCount = divideSdkJpegExtent(columns, columnUnit);
        u32 rows = (u32)mHeight * (u32)component->mVerticalSampling;
        u32 rowUnit = (u32)mMaxVerticalSampling * (u32)mBlockSize;
        component->mRowCount = divideSdkJpegExtent(rows, rowUnit);
    }
    bool paired = mOutputColorMode == 3 && mComponentCount == 3 && mInputColorMode == 2 && mOutputChannels == 3;
    if (paired) {
        paired = mComponents[0].mHorizontalSampling == 2 && mComponents[1].mHorizontalSampling == 1 && mComponents[2].mHorizontalSampling == 1;
        paired = paired && (u32)mComponents[0].mVerticalSampling <= 2 && mComponents[1].mVerticalSampling == 1 && mComponents[2].mVerticalSampling == 1;
        paired = paired && (u32)mComponents[0].mHorizontalStep == (u32)mHorizontalDivisor && (u32)mComponents[1].mHorizontalStep == (u32)mHorizontalDivisor && (u32)mComponents[2].mHorizontalStep == (u32)mHorizontalDivisor;
        paired = paired && (u32)mComponents[0].mVerticalStep == (u32)mVerticalDivisor && (u32)mComponents[1].mVerticalStep == (u32)mVerticalDivisor && (u32)mComponents[2].mVerticalStep == (u32)mVerticalDivisor;
    }
    mOutputMode = paired ? 1 : 0;
}

/* 0284DCB0 */
static void prepareSdkJpegImage(u32 width, u32 height, SdkJpegImageInfo* info) {
    info->mHeight = height;
    info->mWidth = width;
    info->mMaxHorizontalSampling = 2;
    info->mSpectralEnd = 63;
    info->mComponents[0].mIdentifier = 1;
    info->mBlockSize = 8;
    info->mComponents[0].mHorizontalSampling = 2;
    info->mComponentCount = 3;
    info->mComponents[0].mVerticalSampling = 2;
    info->mComponents[1].mIdentifier = 2;
    info->mComponents[1].mHorizontalSampling = 1;
    info->mComponents[1].mVerticalSampling = 1;
    info->mComponents[2].mVerticalSampling = 1;
    info->mComponents[2].mIdentifier = 3;
    info->mMaxVerticalSampling = 2;
    info->mComponents[2].mHorizontalSampling = 1;
    info->deriveComponents();
}

struct SdkJpegApplicationId { be<u8> mBytes[5]; };
struct SdkJpegApplicationScratch { be<u16> mLength; u8 mReserved[2]; SdkJpegOffset mOffset; SdkJpegApplicationId mId; };

/* 0284D358 */
static s32 readSdkJpegColorMode(const be<u8>* data, u32 size, SdkJpegImageInfo* info) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<SdkJpegApplicationScratch> scratch(0x8);
    scratch->mLength = 0;
    scratch->mOffset.mValue = 0;
    if (data == NULL || info == NULL || size < 2) return -1;
    info->mJfifPresent = 0;
    info->mAdobePresent = 0;
    if (findSdkJpegMarker(data, size, 0, &scratch->mOffset, 0xFFE0) == 0) {
        scratch->mOffset.mValue = (u32)scratch->mOffset.mValue + 2;
        copySdkJpegBytes(&scratch->mLength, data + (u32)scratch->mOffset.mValue, 2);
        u32 length = scratch->mLength;
        if (length < 2 || (u32)scratch->mOffset.mValue + length > size) return -1;
        scratch->mLength = (u16)(length - 2);
        scratch->mOffset.mValue = (u32)scratch->mOffset.mValue + 2;
        if ((u32)scratch->mLength >= 5) {
            copySdkJpegBytes(&scratch->mId, data + (u32)scratch->mOffset.mValue, 5);
            if (scratch->mId.mBytes[0] == 'J' && scratch->mId.mBytes[1] == 'F' && scratch->mId.mBytes[2] == 'I' && scratch->mId.mBytes[3] == 'F' && scratch->mId.mBytes[4] == 0) info->mJfifPresent = 1;
        }
        scratch->mOffset.mValue = (u32)scratch->mOffset.mValue + (u32)scratch->mLength;
    }
    u32 start = scratch->mOffset.mValue;
    scratch->mOffset.mValue = 0;
    if (findSdkJpegMarker(data, size, start, &scratch->mOffset, 0xFFEE) == 0) {
        scratch->mOffset.mValue = (u32)scratch->mOffset.mValue + 2;
        copySdkJpegBytes(&scratch->mLength, data + (u32)scratch->mOffset.mValue, 2);
        u32 length = scratch->mLength;
        if (length < 2 || (u32)scratch->mOffset.mValue + length > size) return -1;
        scratch->mLength = (u16)(length - 2);
        scratch->mOffset.mValue = (u32)scratch->mOffset.mValue + 2;
        if ((u32)scratch->mLength >= 12) {
            copySdkJpegBytes(&scratch->mId, data + (u32)scratch->mOffset.mValue, 5);
            if (scratch->mId.mBytes[0] == 'A' && scratch->mId.mBytes[1] == 'd' && scratch->mId.mBytes[2] == 'o' && scratch->mId.mBytes[3] == 'b' && scratch->mId.mBytes[4] == 'e') {
                info->mAdobePresent = 1;
                copySdkJpegBytes(&info->mFlags, data + (u32)scratch->mOffset.mValue + 11, 1);
            }
        }
    }
    u32 components = info->mComponentCount;
    if (components == 1) {
        info->mOutputColorMode = 1;
        info->mColorReady = 1;
        info->mInputColorMode = 2;
        info->mOutputChannels = 3;
    } else if (components == 3) {
        bool rgb = false;
        if (info->mJfifPresent == 0) {
            if (info->mAdobePresent != 0) rgb = info->mFlags == 0;
            else {
                u32 first = info->mComponents[0].mIdentifier;
                u32 second = info->mComponents[1].mIdentifier;
                u32 third = info->mComponents[2].mIdentifier;
                rgb = first != 1 && first == 'R' && second == 'G' && third == 'B';
            }
        }
        if (rgb) {
            info->mInputColorMode = 2;
            info->mOutputColorMode = 2;
            info->mOutputChannels = 3;
            info->mColorReady = 1;
        } else {
            info->mOutputChannels = 3;
            info->mOutputColorMode = 3;
            info->mInputColorMode = 2;
            info->mColorReady = 1;
        }
    } else if (components == 4) {
        info->mOutputColorMode = info->mAdobePresent != 0 && info->mFlags != 0 ? 5 : 4;
        info->mOutputChannels = 3;
        info->mInputColorMode = 2;
        info->mColorReady = 1;
    } else { info->mOutputColorMode = 0; return -1; }
    return 0;
}

extern u32 scaleSdkJpegBlock(u32, u32, u32);
extern u32 scaleSdkJpegExtent(u32, u32, u32);

struct SdkJpegMarkerWord { be<u16> mValue; };

/* 0284D704 */
static s32 readSdkJpegImage(const be<u8>* data, u32 size, u32 scale, SdkJpegImageInfo* info) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<SdkJpegMarkerWord> marker(8);
    marker->mValue = 0;
    if (data == NULL || info == NULL || size < 2) return -1;
    if (validateSdkJpegStart(data) != 0 || validateSdkJpegEnd(data, size) != 0) return -1;
    if (countSdkJpegQuantizers(data, size, info) != 0 || findSdkJpegFrame(data, size, &marker->mValue) != 0) return -1;
    if (readSdkJpegFrame(data, size, info, info, marker->mValue) != 0) return -1;
    if (readSdkJpegColorMode(data, size, info) != 0 || countSdkJpegHuffmanTables(data, size, info) != 0 || readSdkJpegBlockSize(data, size, info) != 0 || countSdkJpegActiveTables(data, size, info) != 0) return -1;
    u32 block = scaleSdkJpegBlock(info->mBlockSize, 1, scale);
    info->mOutputWidth = scaleSdkJpegExtent(info->mWidth, info->mBlockSize, block);
    info->mOutputHeight = scaleSdkJpegExtent(info->mHeight, info->mBlockSize, block);
    info->deriveComponents();
    return 0;
}

struct SdkJpegNotice { be<u8> mByte; };
static SdkJpegNotice sCodecNotice;
extern void notifySdkJpegUse(SdkJpegNotice*);
struct SdkJpegDimensions { be<u32> mWidth; be<u32> mHeight; be<u32> mChannels; };

/* 0284DDB0 */
static s32 estimateSdkJpegDecode(const be<u8>* data, u32 size, u32 scale, SdkJpegDimensions* dimensions, be<u32>* requiredSize) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<SdkJpegImageInfo> info(0x8);
    auto words = reinterpret_cast<be<u32>*>(info.get());
    for (u32 index = 0; index < 45; ++index) words[index] = 0;
    if (data == NULL || dimensions == NULL || requiredSize == NULL || (scale != 1 && scale != 2 && scale != 4 && scale != 8)) return -1;
    gabi::cpu->cr[6] = 0;
    notifySdkJpegUse(&sCodecNotice);
    if (validateSdkJpegStart(data) != 0 || readSdkJpegImage(data, size, scale, info.get()) != 0) return -3;
    u32 total = info->tableStorageSize() + 0x54;
    total += info->decoderControlStorageSize();
    total += info->decoderPlaneStorageSize();
    total += info->outputStorageSize();
    u32 width = info->mOutputWidth;
    u32 components = info->mComponentCount;
    dimensions->mWidth = width;
    u32 height = info->mOutputHeight;
    dimensions->mChannels = components;
    dimensions->mHeight = height;
    *requiredSize = total + 0x400;
    return 0;
}

/* 0284DEF8 */
static s32 estimateSdkJpegEncode(const SdkJpegDimensions* dimensions, be<u32>* requiredSize) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<SdkJpegImageInfo> info(0x8);
    auto words = reinterpret_cast<be<u32>*>(info.get());
    for (u32 index = 0; index < 45; ++index) words[index] = 0;
    if (dimensions == NULL) return -1;
    gabi::cpu->cr[6] = 0;
    notifySdkJpegUse(&sCodecNotice);
    *requiredSize = 0;
    prepareSdkJpegImage(dimensions->mWidth, dimensions->mHeight, info.get());
    u32 total = sdkJpegEncoderControlSize() + 0x54;
    total += info->encoderPlaneStorageSize();
    total += sdkJpegPixelRowSize(dimensions->mWidth);
    *requiredSize = total + 0x400;
    return 0;
}

static SdkJpegRows sEncoderRows;
static SdkJpegRows sDecoderRows;
extern void* createSdkJpegHeap(void*, u32, u32);
extern be<u8>* allocateSdkJpegRow(void*, u32, s32);
extern void freeSdkJpegRow(void*, void*);
extern void destroySdkJpegHeap(void*);
extern void fillSdkJpegOutput(void*, u32, u32);
extern SdkJpegFailure* initializeSdkJpegFailure(SdkJpegFailure*);
extern s32 saveSdkJpegRecovery(SdkJpegRecovery*);
extern void createSdkJpegEncoder(SdkJpegEncodeControl*, u32, u32);
extern void bindSdkJpegOutput(SdkJpegEncodeControl*, gptr<be<u8>>*, be<u32>*);
extern void startSdkJpegEncoder(SdkJpegEncodeControl*, u32);
extern u32 writeSdkJpegRows(SdkJpegEncodeControl*, SdkJpegRows*, u32);
extern void finishSdkJpegEncoder(SdkJpegEncodeControl*);
extern void destroySdkJpegEncoder(SdkJpegEncodeControl*);
extern void createSdkJpegDecoder(SdkJpegDecodeControl*, u32, u32);
extern void bindSdkJpegInput(SdkJpegDecodeControl*, const be<u8>*, u32);
extern s32 readSdkJpegHeader(SdkJpegDecodeControl*, u32);
extern s32 startSdkJpegDecoder(SdkJpegDecodeControl*);
extern u32 readSdkJpegRows(SdkJpegDecodeControl*, SdkJpegRows*, u32);
extern s32 finishSdkJpegDecoder(SdkJpegDecodeControl*);
extern void destroySdkJpegDecoder(SdkJpegDecodeControl*);

/* 0284BBB4 */
static s32 encodeSdkJpeg(void* heapStorage, u32 heapSize, const be<u8>* pixels, const SdkJpegDimensions* dimensions, u32 quality, be<u8>* output, u32 capacity, be<u32>* written) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<SdkJpegEncodeFrame> frame(kSdkJpegFrameOffset);
    frame->mOutput = output;
    frame->mResult = 0;
    for (u32 index = 0; index < 4; ++index) frame->mRows.mRows[index] = sEncoderRows.mRows[index].get();
    if ((u32)dimensions->mChannels - 1 >= 4 || quality - 1 >= 100) return -1;
    void* heap = createSdkJpegHeap(heapStorage, heapSize, 0);
    if (heap == NULL) return -18;
    frame->mCodec.mFailure = initializeSdkJpegFailure(&frame->mFailure);
    frame->mCodec.mHeap = heap;
    frame->mFailure.mResult = 0;
    frame->mFailure.mCallback = gabi::guest_fn((const void*)reportSdkJpegFailure);
    if (saveSdkJpegRecovery(&frame->mFailure.mRecovery) == 0) {
        createSdkJpegEncoder(&frame->mCodec, 0x50, 0x1B0);
        frame->mCapacity = capacity;
        bindSdkJpegOutput(&frame->mCodec, &frame->mOutput, &frame->mCapacity);
        u32 width = dimensions->mWidth;
        u32 height = dimensions->mHeight;
        frame->mCodec.mWidth = width;
        frame->mCodec.mInputChannels = 3;
        frame->mCodec.mInputColorMode = 2;
        frame->mCodec.mHeight = height;
        frame->mCodec.prepare(quality);
        startSdkJpegEncoder(&frame->mCodec, 1);
        u32 rowSize = (u32)dimensions->mWidth * 3;
        for (u32 index = 0; index < 4; ++index) {
            be<u8>* row = allocateSdkJpegRow(heap, rowSize, 4);
            frame->mRows.mRows[index] = row;
            if (row == NULL) {
                frame->mFailure.mDetail = 133;
                frame->mFailure.mCode = 56;
                gabi::call_ptr<void>(frame->mFailure.mCallback, reinterpret_cast<SdkJpegFailureContext*>(&frame->mCodec));
            }
        }
        while ((u32)frame->mCodec.mOutputLine < (u32)dimensions->mHeight) {
            u32 line = frame->mCodec.mOutputLine;
            u32 width = dimensions->mWidth;
            u32 remaining = (u32)dimensions->mHeight - line;
            const be<u8>* source = pixels + line * width * 4;
            for (u32 index = 0; index < 4; ++index) {
                be<u8>* destination = frame->mRows.mRows[index];
                for (s32 column = 0; column < (s32)(u32)dimensions->mWidth; ++column) {
                    copySdkJpegBytes(destination, source, 3);
                    source += 4;
                    destination += 3;
                }
            }
            if ((s32)remaining > 4) remaining = 4;
            writeSdkJpegRows(&frame->mCodec, &frame->mRows, remaining);
        }
        finishSdkJpegEncoder(&frame->mCodec);
        *written = frame->mCapacity;
    } else {
        *written = 0;
        frame->mResult = frame->mFailure.mResult;
    }
    for (u32 index = 0; index < 4; ++index) {
        void* row = frame->mRows.mRows[index];
        if (row != NULL) freeSdkJpegRow(heap, row);
    }
    destroySdkJpegEncoder(&frame->mCodec);
    destroySdkJpegHeap(heap);
    return frame->mResult;
}

/* 0284BE8C */
static s32 decodeSdkJpeg(void* heapStorage, u32 heapSize, const be<u8>* data, u32 size, u32 scale, be<u8>* output, u32 capacity) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<SdkJpegDecodeFrame> frame(kSdkJpegFrameOffset);
    frame->mResult = 0;
    for (u32 index = 0; index < 4; ++index) frame->mRows.mRows[index] = sDecoderRows.mRows[index].get();
    if (scale != 1 && scale != 2 && scale != 4 && scale != 8) return -1;
    void* heap = createSdkJpegHeap(heapStorage, heapSize, 0);
    if (heap == NULL) return -18;
    frame->mCodec.mFailure = initializeSdkJpegFailure(&frame->mFailure);
    frame->mCodec.mHeap = heap;
    frame->mFailure.mResult = 0;
    frame->mFailure.mCallback = gabi::guest_fn((const void*)reportSdkJpegFailure);
    if (saveSdkJpegRecovery(&frame->mFailure.mRecovery) == 0) {
        createSdkJpegDecoder(&frame->mCodec, 0x50, 0x1F0);
        bindSdkJpegInput(&frame->mCodec, data, size);
        if (readSdkJpegHeader(&frame->mCodec, 0) != 1) {
            frame->mFailure.mCode = 25;
            gabi::call_ptr<void>(frame->mFailure.mCallback, reinterpret_cast<SdkJpegFailureContext*>(&frame->mCodec));
        }
        if (startSdkJpegDecoder(frame->mCodec.setScale(scale)) == 0) {
            frame->mFailure.mCode = 25;
            gabi::call_ptr<void>(frame->mFailure.mCallback, reinterpret_cast<SdkJpegFailureContext*>(&frame->mCodec));
        }
        u32 channels = frame->mCodec.mOutputChannels;
        u32 width = frame->mCodec.mOutputWidth;
        u32 height = frame->mCodec.mOutputHeight;
        u32 rowSize = channels * width;
        if (channels != 3) {
            frame->mFailure.mCode = 28;
            gabi::call_ptr<void>(frame->mFailure.mCallback, reinterpret_cast<SdkJpegFailureContext*>(&frame->mCodec));
        }
        if (width * height * channels > capacity) {
            frame->mFailure.mDetail = 133;
            frame->mFailure.mCode = 56;
            gabi::call_ptr<void>(frame->mFailure.mCallback, reinterpret_cast<SdkJpegFailureContext*>(&frame->mCodec));
        }
        for (u32 index = 0; index < 4; ++index) {
            be<u8>* row = allocateSdkJpegRow(heap, rowSize, 4);
            frame->mRows.mRows[index] = row;
            if (row == NULL) {
                frame->mFailure.mDetail = 133;
                frame->mFailure.mCode = 56;
                gabi::call_ptr<void>(frame->mFailure.mCallback, reinterpret_cast<SdkJpegFailureContext*>(&frame->mCodec));
            }
        }
        fillSdkJpegOutput(output, 255, capacity);
        u32 line = frame->mCodec.mOutputLine;
        while (line < height) {
            u32 remaining = height - line;
            if ((s32)remaining > 4) remaining = 4;
            u32 rows = readSdkJpegRows(&frame->mCodec, &frame->mRows, remaining);
            for (u32 index = 0; index < rows; ++index) {
                const be<u8>* source = frame->mRows.mRows[index];
                for (s32 column = 0; column < (s32)width; ++column) {
                    copySdkJpegBytes(output, source, 3);
                    output += 4;
                    source += 3;
                }
            }
            line = frame->mCodec.mOutputLine;
        }
        if (finishSdkJpegDecoder(&frame->mCodec) == 0) {
            frame->mFailure.mCode = 25;
            gabi::call_ptr<void>(frame->mFailure.mCallback, reinterpret_cast<SdkJpegFailureContext*>(&frame->mCodec));
        }
    } else frame->mResult = frame->mFailure.mResult;
    for (u32 index = 0; index < 4; ++index) {
        void* row = frame->mRows.mRows[index];
        if (row != NULL) freeSdkJpegRow(heap, row);
    }
    destroySdkJpegDecoder(&frame->mCodec);
    destroySdkJpegHeap(heap);
    return frame->mResult;
}

/* WWHD glue
 data sEncoderRows 1017B77C
 data sDecoderRows 1017B78C
 ext C00096D8 void* createSdkJpegHeap(void*, u32, u32)
 ext C0009698 be<u8>* allocateSdkJpegRow(void*, u32, s32)
 ext C0009748 void freeSdkJpegRow(void*, void*)
 ext C0009700 void destroySdkJpegHeap(void*)
 ext C0009990 void fillSdkJpegOutput(void*, u32, u32)
 ext 02858238 sdkjpeg::SdkJpegFailure* initializeSdkJpegFailure(sdkjpeg::SdkJpegFailure*)
 ext 028F325C s32 saveSdkJpegRecovery(sdkjpeg::SdkJpegRecovery*)
 ext 0284E0CC void createSdkJpegEncoder(sdkjpeg::SdkJpegEncodeControl*, u32, u32)
 ext 02852D7C void bindSdkJpegOutput(sdkjpeg::SdkJpegEncodeControl*, gptr<be<u8>>*, be<u32>*)
 ext 0284E424 void startSdkJpegEncoder(sdkjpeg::SdkJpegEncodeControl*, u32)
 ext 0284E4FC u32 writeSdkJpegRows(sdkjpeg::SdkJpegEncodeControl*, sdkjpeg::SdkJpegRows*, u32)
 ext 0284E280 void finishSdkJpegEncoder(sdkjpeg::SdkJpegEncodeControl*)
 ext 0284E214 void destroySdkJpegEncoder(sdkjpeg::SdkJpegEncodeControl*)
 ext 028520E8 void createSdkJpegDecoder(sdkjpeg::SdkJpegDecodeControl*, u32, u32)
 ext 02852FA0 void bindSdkJpegInput(sdkjpeg::SdkJpegDecodeControl*, const be<u8>*, u32)
 ext 0285272C s32 readSdkJpegHeader(sdkjpeg::SdkJpegDecodeControl*, u32)
 ext 02852A88 s32 startSdkJpegDecoder(sdkjpeg::SdkJpegDecodeControl*)
 ext 02852BF8 u32 readSdkJpegRows(sdkjpeg::SdkJpegDecodeControl*, sdkjpeg::SdkJpegRows*, u32)
 ext 028527E0 s32 finishSdkJpegDecoder(sdkjpeg::SdkJpegDecodeControl*)
 ext 02852210 void destroySdkJpegDecoder(sdkjpeg::SdkJpegDecodeControl*)
 ext C000A858 void fillSdkJpegBytes(void*, u32, u32)
 ext 0284DFDC u32 roundSdkJpegCount(u32, u32)
 ext 0284DFCC u32 divideSdkJpegExtent(u32, u32)
 ext 0284DFF8 u32 scaleSdkJpegBlock(u32, u32, u32)
 ext 0284E040 u32 scaleSdkJpegExtent(u32, u32, u32)
 ext 0286D5F0 void notifySdkJpegUse(SdkJpegNotice*)
 data sCodecNotice 101FCD40
 data sFrameMarkers 1017B79C
 ext C000A848 void copySdkJpegBytes(void*, const void*, u32)
 data sFailureCategories 1017B734
 data sMarkerResults 101FC818
 ext 028F3394 void resumeSdkJpegFailure(sdkjpeg::SdkJpegRecovery*, s32)
 ext 02850DF0 void initializeSdkJpegEncoding(sdkjpeg::SdkJpegEncodeControl*)
 ext 028509C8 void selectSdkJpegEncoding(sdkjpeg::SdkJpegEncodeControl*, u32)
 ext 02850844 void setSdkJpegQuality(sdkjpeg::SdkJpegEncodeControl*, u32, u32)
*/

// The HD build retains separate color-conversion and matrix-copy bodies.
#pragma once
#include "wwhd.h"
namespace j3d {
struct ByteColor { be<u8> mChannels[4]; };
struct NormalizedColor {
    be<f32> mChannels[4];
    void assignBytes(const ByteColor*);
    void assignBytesAlternate(const ByteColor*);
};
struct MatrixWords {
    be<u32> mWords[12];
    void assign(const MatrixWords*);
};
struct ColorConversionWords { be<u32> mHigh; be<u32> mLow; };
struct ColorConversionResult { be<f32> mChannels[4]; };
struct ColorConversionScale { be<f32> mDivisor; };
struct ColorConversionBias { be<f64> mBias; };
}
WWHD_SIZE(j3d::ByteColor,4);
WWHD_SIZE(j3d::NormalizedColor,16);
WWHD_SIZE(j3d::MatrixWords,48);
WWHD_SIZE(j3d::ColorConversionWords,8);

// Accessed prefix offsets and value strides; no unknown foreign extent is asserted.
WWHD_SIZE(j3d::ColorConversionResult,16);
WWHD_SIZE(j3d::ColorConversionScale,4);
WWHD_SIZE(j3d::ColorConversionBias,8);

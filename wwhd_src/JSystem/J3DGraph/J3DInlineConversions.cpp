// Value conversions and overlap-safe copies, from the retained HD instructions.
#include "JSystem/J3DGraph/J3DInlineConversions.h"
using namespace j3d;
static ColorConversionScale sColorScale;
static ColorConversionBias sColorBias;
static ColorConversionScale sAlternateScale;
static ColorConversionBias sAlternateBias;
/* 02727F7C */
void NormalizedColor::assignBytes(const ByteColor* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ColorConversionWords> conversion(8);
    gabi::FrameLocal<ColorConversionResult> result(0x10);
    u32 red=source->mChannels[0];
    u32 green=source->mChannels[1];
    conversion->mLow=red;
    conversion->mHigh=0x43300000;
    f64 rawRed=gabi::load<f64>(gabi::ea(conversion.get()));
    f64 bias=sColorBias.mBias;
    conversion->mLow=green;
    f64 redValue=gabi::fsub_ppc(rawRed,bias);
    f64 rawGreen=gabi::load<f64>(gabi::ea(conversion.get()));
    f32 redSingle=f32(redValue);
    f32 divisor=sColorScale.mDivisor;
    f32 greenSingle=f32(gabi::fsub_ppc(rawGreen,bias));
    u32 blue=source->mChannels[2];
    conversion->mLow=blue;
    f32 normalizedRed=gabi::fdivs_ppc(redSingle,divisor);
    f64 rawBlue=gabi::load<f64>(gabi::ea(conversion.get()));
    u32 alpha=source->mChannels[3];
    conversion->mLow=alpha;
    f64 rawAlpha=gabi::load<f64>(gabi::ea(conversion.get()));
    f64 alphaValue=gabi::fsub_ppc(rawAlpha,bias);
    f64 blueValue=gabi::fsub_ppc(rawBlue,bias);
    f32 normalizedGreen=gabi::fdivs_ppc(greenSingle,divisor);
    f32 alphaSingle=f32(alphaValue);
    result->mChannels[1]=normalizedGreen;
    f32 blueSingle=f32(blueValue);
    f32 normalizedBlue=gabi::fdivs_ppc(blueSingle,divisor);
    f32 normalizedAlpha=gabi::fdivs_ppc(alphaSingle,divisor);
    result->mChannels[2]=normalizedBlue;
    result->mChannels[0]=normalizedRed;
    mChannels[0]=result->mChannels[0];
    mChannels[1]=result->mChannels[1];
    mChannels[2]=result->mChannels[2];
    result->mChannels[3]=normalizedAlpha;
    mChannels[3]=result->mChannels[3];
}
/* 02728498 */
void NormalizedColor::assignBytesAlternate(const ByteColor* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ColorConversionWords> conversion(8);
    gabi::FrameLocal<ColorConversionResult> result(0x10);
    u32 red=source->mChannels[0];
    u32 green=source->mChannels[1];
    conversion->mLow=red;
    conversion->mHigh=0x43300000;
    f64 rawRed=gabi::load<f64>(gabi::ea(conversion.get()));
    f64 bias=sAlternateBias.mBias;
    conversion->mLow=green;
    f64 redValue=gabi::fsub_ppc(rawRed,bias);
    f64 rawGreen=gabi::load<f64>(gabi::ea(conversion.get()));
    f32 redSingle=f32(redValue);
    f32 divisor=sAlternateScale.mDivisor;
    f32 greenSingle=f32(gabi::fsub_ppc(rawGreen,bias));
    u32 blue=source->mChannels[2];
    conversion->mLow=blue;
    f32 normalizedRed=gabi::fdivs_ppc(redSingle,divisor);
    f64 rawBlue=gabi::load<f64>(gabi::ea(conversion.get()));
    u32 alpha=source->mChannels[3];
    conversion->mLow=alpha;
    f64 rawAlpha=gabi::load<f64>(gabi::ea(conversion.get()));
    f64 alphaValue=gabi::fsub_ppc(rawAlpha,bias);
    f64 blueValue=gabi::fsub_ppc(rawBlue,bias);
    f32 normalizedGreen=gabi::fdivs_ppc(greenSingle,divisor);
    f32 alphaSingle=f32(alphaValue);
    result->mChannels[1]=normalizedGreen;
    f32 blueSingle=f32(blueValue);
    f32 normalizedBlue=gabi::fdivs_ppc(blueSingle,divisor);
    f32 normalizedAlpha=gabi::fdivs_ppc(alphaSingle,divisor);
    result->mChannels[2]=normalizedBlue;
    result->mChannels[0]=normalizedRed;
    mChannels[0]=result->mChannels[0];
    mChannels[1]=result->mChannels[1];
    mChannels[2]=result->mChannels[2];
    result->mChannels[3]=normalizedAlpha;
    mChannels[3]=result->mChannels[3];
}
/* 02732C9C */
void MatrixWords::assign(const MatrixWords* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<MatrixWords> saved(8);
    saved->mWords[5]=u32(source->mWords[5]);
    saved->mWords[4]=u32(source->mWords[4]);
    saved->mWords[6]=u32(source->mWords[6]);
    saved->mWords[7]=u32(source->mWords[7]);
    saved->mWords[8]=u32(source->mWords[8]);
    saved->mWords[9]=u32(source->mWords[9]);
    saved->mWords[11]=u32(source->mWords[11]);
    u32 first=source->mWords[0];
    u32 fourth=source->mWords[3];
    u32 second=source->mWords[1];
    saved->mWords[3]=fourth;
    saved->mWords[1]=second;
    u32 eleventh=source->mWords[10];
    u32 third=source->mWords[2];
    saved->mWords[0]=first;
    saved->mWords[2]=third;
    saved->mWords[10]=eleventh;
    for(u32 row=0;row<3;++row)
        for(u32 column=0;column<4;++column)
            mWords[row*4+column]=u32(saved->mWords[row*4+column]);
}
/* WWHD glue
 data sColorScale 10140FD8
 data sColorBias 10140FE0
 data sAlternateScale 10141060
 data sAlternateBias 10141068
*/

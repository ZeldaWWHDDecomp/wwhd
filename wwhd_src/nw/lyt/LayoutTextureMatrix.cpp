/* Quantized material texture transform, reconstructed from WWHD instructions. */
#include "nw/lyt/LayoutTextureMatrix.h"
using namespace nw::lyt;
struct TextureTrigRecord { be<f32> sine,sineSlope,cosine,cosineSlope; };
struct TextureTrigTable { TextureTrigRecord records[256]; };
struct TextureAngleUnits { be<u32> value; };
struct TextureMatrixConversion { be<u32> high,low; };
static TextureTrigTable sTextureTrigTable;
static TextureAngleUnits sTextureAngleUnits;
/* 02882708 */
void LayoutTextureMatrix::configure(const MaterialTextureTransform* input) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 angleUnits=sTextureAngleUnits.value;
    gabi::FrameLocal<TextureMatrixConversion> conversion(8);
    conversion->high=0x43300000;
    conversion->low=angleUnits;
    f32 unitsPerDegree=gabi::fdivs_ppc(static_cast<f32>(angleUnits),180.0f);
    f32 rotation=input->rotation;
    auto converted=textureMatrixConvertAngle(static_cast<f64>(gabi::fmuls_ppc(rotation,unitsPerDegree)));
    conversion->high=0x43300000;
    u32 fractionBits=converted.r4&0xFFFFFF;
    conversion->low=fractionBits;
    u32 index=converted.r4>>24;
    TextureTrigRecord* entry=&sTextureTrigTable.records[index];
    f32 cosineSlope=entry->cosineSlope;
    f32 fraction=gabi::fmuls_ppc(static_cast<f32>(fractionBits),0x1p-24f);
    f32 cosineBase=entry->cosine;
    f32 sineSlope=entry->sineSlope;
    f32 sineBase=entry->sine;
    f32 cosine=gabi::fmadds(cosineSlope,fraction,cosineBase);
    f32 xScale=input->scaleX;
    f32 sine=gabi::fmadds(sineSlope,fraction,sineBase);
    f32 yScale=input->scaleY;
    f32 cosineX=gabi::fmuls_ppc(cosine,xScale);
    f32 sineY=gabi::fmuls_ppc(sine,yScale);
    f32 negativeSine=-sineY;
    horizontalCross=negativeSine;
    horizontalScale=cosineX;
    f32 x=input->translateX;
    f32 xOffset=gabi::fadds_ppc(x,0.5f);
    xOffset=gabi::fmadds(cosineX,-0.5f,xOffset);
    xOffset=gabi::fmadds(negativeSine,-0.5f,xOffset);
    horizontalOffset=xOffset;
    xScale=input->scaleX;
    yScale=input->scaleY;
    f32 sineX=gabi::fmuls_ppc(sine,xScale);
    f32 cosineY=gabi::fmuls_ppc(cosine,yScale);
    verticalCross=sineX;
    verticalScale=cosineY;
    f32 y=input->translateY;
    f32 yOffset=gabi::fadds_ppc(y,0.5f);
    yOffset=gabi::fmadds(sineX,-0.5f,yOffset);
    yOffset=gabi::fmadds(cosineY,-0.5f,yOffset);
    verticalCross=-sineX;
    verticalScale=-cosineY;
    verticalOffset=gabi::fsubs_ppc(1.0f,yOffset);
}
/* 02882FD0 */
void LayoutTextureMatrix::configureIndirect(const PaneSize* scale,f32 angle) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 units=sTextureAngleUnits.value;
    gabi::FrameLocal<TextureMatrixConversion> conversion(8);
    conversion->high=0x43300000;
    conversion->low=units;
    f32 unitScale=gabi::fdivs_ppc(static_cast<f32>(units),180.0f);
    auto converted=textureMatrixConvertAngle(static_cast<f64>(gabi::fmuls_ppc(angle,unitScale)));
    conversion->high=0x43300000;
    u32 fractionBits=converted.r4&0xFFFFFF;
    conversion->low=fractionBits;
    auto* entry=&sTextureTrigTable.records[converted.r4>>24];
    f32 sineBase=entry->sine;
    f32 sineSlope=entry->sineSlope;
    f32 cosineSlope=entry->cosineSlope;
    f32 cosineBase=entry->cosine;
    f32 fraction=gabi::fmuls_ppc(static_cast<f32>(fractionBits),0x1p-24f);
    f32 sine=gabi::fmadds(sineSlope,fraction,sineBase);
    f32 height=scale->height;
    f32 cosine=gabi::fmadds(cosineSlope,fraction,cosineBase);
    f32 sineY=gabi::fmuls_ppc(sine,height);
    f32 width=scale->width;
    f32 doubledSineY=gabi::fadds_ppc(sineY,sineY);
    f32 cosineX=gabi::fmuls_ppc(cosine,width);
    f32 cross=-doubledSineY;
    f32 doubledCosineX=gabi::fadds_ppc(cosineX,cosineX);
    f32 offset=gabi::fmuls_ppc(cross,-0.5f);
    horizontalScale=doubledCosineX;
    offset=gabi::fmadds(doubledCosineX,-0.5f,offset);
    horizontalCross=cross;
    horizontalOffset=offset;
    width=scale->width;
    height=scale->height;
    f32 sineX=gabi::fmuls_ppc(sine,width);
    f32 cosineY=gabi::fmuls_ppc(cosine,height);
    f32 doubledCosineY=gabi::fadds_ppc(cosineY,cosineY);
    f32 doubledSineX=gabi::fadds_ppc(sineX,sineX);
    offset=gabi::fmuls_ppc(doubledCosineY,-0.5f);
    verticalCross=doubledSineX;
    offset=gabi::fmadds(doubledSineX,-0.5f,offset);
    verticalScale=doubledCosineY;
    verticalOffset=offset;
}
/* WWHD glue
data sTextureTrigTable 1017E9C4
data sTextureAngleUnits 1017FE70
ext 028F5680 Pair32 textureMatrixConvertAngle(f64)
*/

// Own spherical parameter writer; external buffer and SDK implementations stay API boundaries.
#include "agl/DrawParameterPreparation.h"
#include "sead/ElementAt.h"
void transformDrawParameterPosition(const sead::RenderMatrix34* matrix, const void* input, sead::RenderVector3* output);
void prepareDrawParameterBuffer(agl::UniformBuffer* buffer);
void writeDrawParameter(agl::UniformBuffer* buffer, void* data, u32 field, const void* source, u32 offset, u32 count);
namespace agl {
struct DrawParameterVectorWords_l { be<u32> x, y, z; };
struct DrawParameterVector4_l { be<f32> x, y, z, w; };
struct DrawParameterScalar_l { be<f32> mValue; };
// Data-only owner of the immutable image-provided scale presets.
struct DrawParameterScalePresets { be<f32> mValues[3]; };
WWHD_SIZE(DrawParameterScalePresets, 12);
static const DrawParameterScalePresets sDrawParameterScaleTable;
static f32 sDrawParameterConservativeScale;
/* 027CC3D0 */
void agl::DrawParameterPreparation_l::prepareParameters(DrawParameterRecord_l* record, u32 slot,
                                                        const DrawParameterFrame_l* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderVector3> projection(8);
    gabi::FrameLocal<DrawParameterVector4_l> positionRadius(0x14);
    gabi::FrameLocal<sead::ColorChannels> alternateColor(0x24);
    gabi::FrameLocal<DrawParameterScalar_l> inverseRadius(0x34);
    gabi::FrameLocal<DrawParameterScalar_l> positiveValue(0x38);
    gabi::FrameLocal<DrawParameterScalar_l> threshold(0x3C);
    gabi::FrameLocal<DrawParameterVectorWords_l> input(0x40);
    gabi::FrameLocal<sead::RenderVector3> viewPosition(0x4C);
    f32 projection01 = context->mProjection.mRows[0][1];
    f32 positionY = record->mPosition.y;
    f32 positionX = record->mPosition.x;
    f32 positionZ = record->mPosition.z;
    f32 clipX = gabi::fmuls_ppc(projection01, positionY);
    clipX = gabi::fmadds(context->mProjection.mRows[0][0], positionX, clipX);
    clipX = gabi::fmadds(context->mProjection.mRows[0][2], positionZ, clipX);
    clipX = gabi::fadds_ppc(clipX, context->mProjection.mRows[0][3]);
    f32 clipY = gabi::fmuls_ppc(context->mProjection.mRows[1][1], positionY);
    clipY = gabi::fmadds(context->mProjection.mRows[1][0], positionX, clipY);
    clipY = gabi::fmadds(context->mProjection.mRows[1][2], positionZ, clipY);
    clipY = gabi::fadds_ppc(clipY, context->mProjection.mRows[1][3]);
    f32 clipW = gabi::fmuls_ppc(context->mProjection.mRows[3][1], positionY);
    clipW = gabi::fmadds(context->mProjection.mRows[3][0], positionX, clipW);
    clipW = gabi::fmadds(context->mProjection.mRows[3][2], positionZ, clipW);
    clipW = gabi::fadds_ppc(clipW, context->mProjection.mRows[3][3]);
    DrawParameterVectorWords_l* sourcePosition = gabi::at<DrawParameterVectorWords_l>(gabi::ea(&record->mPosition));
    input->z = sourcePosition->z;
    input->x = sourcePosition->x;
    input->y = sourcePosition->y;
    transformDrawParameterPosition(&context->mView, input.get(), viewPosition);
    f32 normalizedX = gabi::fdivs_ppc(clipX, clipW);
    f32 verticalMultiplier = context->mVerticalMultiplier;
    f32 radius = record->mScale;
    f32 projectionHeight = context->mProjectionHeight;
    f32 depthSquared = gabi::fmuls_ppc(clipW, clipW);
    f32 radiusSquared = gabi::fmuls_ppc(radius, radius);
    f32 verticalScale = gabi::fmuls_ppc(projectionHeight, verticalMultiplier);
    u32 alternateRed = record->mAlternateColor.r.bits;
    projection->z = gabi::fdivs_ppc(depthSquared, radiusSquared);
    u32 alternateAlpha = record->mAlternateColor.a.bits;
    DrawResourceConfiguration_l* configuration = mConfiguration;
    f32 normalizedY = gabi::fdivs_ppc(clipY, clipW);
    f32 horizontalMultiplier = context->mHorizontalMultiplier;
    f32 offsetX = context->mOffsetX;
    f32 projectedX = -gabi::fmuls_ppc(normalizedX, horizontalMultiplier);
    u32 alternateGreen = record->mAlternateColor.g.bits;
    u32 alternateBlue = record->mAlternateColor.b.bits;
    f32 projectedY = -gabi::fmuls_ppc(normalizedY, verticalScale);
    f32 offsetY = context->mOffsetY;
    alternateColor->b.bits = alternateBlue;
    alternateColor->a.bits = alternateAlpha;
    projection->x = gabi::fsubs_ppc(projectedX, offsetX);
    f32 projectedHeight = gabi::fmsubs(projectedY, projectionHeight, offsetY);
    u16 flags = record->mFlags;
    alternateColor->g.bits = alternateGreen;
    alternateColor->r.bits = alternateRed;
    projection->y = projectedHeight;
    if ((flags & 2) && !(u32(configuration->mFlags) & 0x100)) {
        sead::ColorChannels* color = alternateColor.get();
        f32 green = color->g.value;
        f32 greenTerm = gabi::fmuls_ppc(green, 0.5866109728813171f);
        f32 red = color->r.value;
        f32 luminance = gabi::fmadds(red, 0.298911988735199f, greenTerm);
        f32 blue = color->b.value;
        luminance = gabi::fmadds(blue, 0.11447799950838089f, luminance);
        f32 alpha = color->a.value;
        color->a.value = gabi::fmuls_ppc(alpha, luminance);
    }
    u32 configurationFlags = configuration->mFlags;
    u32 format = configuration->mFormatIndex;
    f32 factor = configurationFlags & 0x20 ? f32(sDrawParameterConservativeScale) : 1.0f;
    f32 scale = gabi::fmuls_ppc(factor, sDrawParameterScaleTable.mValues[format]);
    f32 diameterScale = gabi::fadds_ppc(scale, scale);
    u32 valueCount = record->mValueCount;
    SelectionValue_l* value = record->mValues;
    if (slot < valueCount) /* guest EA arithmetic: wraps at 32 bits as on the console */
        value = &sead::elementAt<SelectionValue_l>(gabi::ea(value), slot);
    UniformBuffer* buffer = &value->mBuffer;
    prepareDrawParameterBuffer(buffer);
    f32 currentRadius = record->mScale;
    f32 currentX = record->mPosition.x;
    f32 scaledRadius = gabi::fmuls_ppc(currentRadius, diameterScale);
    f32 currentY = record->mPosition.y;
    f32 currentZ = record->mPosition.z;
    positionRadius->w = scaledRadius;
    positionRadius->y = currentY;
    positionRadius->z = currentZ;
    positionRadius->x = currentX;
    writeDrawParameter(buffer, buffer->cpuData, 0, positionRadius.get(), 0, 1);
    writeDrawParameter(buffer, buffer->cpuData, 1, &record->mColor, 0, 1);
    inverseRadius->mValue = gabi::fdivs_ppc(1.0f, record->mScale);
    writeDrawParameter(buffer, buffer->cpuData, 3, inverseRadius.get(), 0, 1);
    positiveValue->mValue = record->mPositiveValue;
    writeDrawParameter(buffer, buffer->cpuData, 4, positiveValue.get(), 0, 1);
    threshold->mValue = record->mThreshold;
    writeDrawParameter(buffer, buffer->cpuData, 5, threshold.get(), 0, 1);
    if (u16(record->mFlags) & 2)
        writeDrawParameter(buffer, buffer->cpuData, 2, alternateColor.get(), 0, 1);
    writeDrawParameter(buffer, buffer->cpuData, 6, viewPosition.get(), 0, 1);
    writeDrawParameter(buffer, buffer->cpuData, 7, projection.get(), 0, 1);
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
/* 027CFA10 */
void agl::DrawParameterPreparation_l::preparePairParameters(DrawPairParameterRecord_l* record, u32 slot,
                                                            const DrawParameterFrame_l* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderVector2> projection(8);
    gabi::FrameLocal<DrawParameterVector4_l> positionRadius(0x10);
    gabi::FrameLocal<sead::ColorChannels> firstColor(0x20), secondColor(0x30);
    gabi::FrameLocal<DrawParameterScalar_l> firstInverse(0x40), secondInverse(0x44);
    gabi::FrameLocal<DrawParameterScalar_l> firstPositive(0x48), secondPositive(0x4C);
    gabi::FrameLocal<DrawParameterScalar_l> firstDepth(0x50), secondDepth(0x54);
    gabi::FrameLocal<DrawParameterVectorWords_l> input(0x58);
    gabi::FrameLocal<sead::RenderVector3> viewPosition(0x64);
    DrawParameterState_l* selected = record->mSelected;
    f32 positionY = selected->mPosition.y;
    f32 positionX = selected->mPosition.x;
    f32 positionZ = selected->mPosition.z;
    f32 clipX = gabi::fmuls_ppc(context->mProjection.mRows[0][1], positionY);
    clipX = gabi::fmadds(context->mProjection.mRows[0][0], positionX, clipX);
    clipX = gabi::fmadds(context->mProjection.mRows[0][2], positionZ, clipX);
    clipX = gabi::fadds_ppc(clipX, context->mProjection.mRows[0][3]);
    f32 clipY = gabi::fmuls_ppc(context->mProjection.mRows[1][1], positionY);
    clipY = gabi::fmadds(context->mProjection.mRows[1][0], positionX, clipY);
    clipY = gabi::fmadds(context->mProjection.mRows[1][2], positionZ, clipY);
    clipY = gabi::fadds_ppc(clipY, context->mProjection.mRows[1][3]);
    f32 clipW = gabi::fmuls_ppc(context->mProjection.mRows[3][1], positionY);
    clipW = gabi::fmadds(context->mProjection.mRows[3][0], positionX, clipW);
    clipW = gabi::fmadds(context->mProjection.mRows[3][2], positionZ, clipW);
    clipW = gabi::fadds_ppc(clipW, context->mProjection.mRows[3][3]);
    DrawParameterVectorWords_l* sourcePosition = gabi::at<DrawParameterVectorWords_l>(gabi::ea(&selected->mPosition));
    input->x = sourcePosition->x;
    input->y = sourcePosition->y;
    input->z = sourcePosition->z;
    transformDrawParameterPosition(&context->mView, input.get(), viewPosition);
    f32 normalizedX = gabi::fdivs_ppc(clipX, clipW);
    f32 horizontalMultiplier = context->mHorizontalMultiplier;
    f32 verticalMultiplier = context->mVerticalMultiplier;
    f32 projectionHeight = context->mProjectionHeight;
    f32 projectedX = -gabi::fmuls_ppc(normalizedX, horizontalMultiplier);
    f32 verticalScale = gabi::fmuls_ppc(projectionHeight, verticalMultiplier);
    f32 normalizedY = gabi::fdivs_ppc(clipY, clipW);
    f32 projectedY = -gabi::fmuls_ppc(normalizedY, verticalScale);
    DrawParameterState_l* second = record->mSecond;
    f32 offsetX = context->mOffsetX;
    f32 offsetY = context->mOffsetY;
    projection->x = gabi::fsubs_ppc(projectedX, offsetX);
    DrawParameterState_l* first = record->mFirst;
    projection->y = gabi::fmsubs(projectedY, projectionHeight, offsetY);
    f32 secondRadius = second->mScale;
    f32 depthSquared = gabi::fmuls_ppc(clipW, clipW);
    f32 firstRadius = first->mScale;
    firstColor->r.bits = first->mAlternateColor.r.bits;
    firstColor->g.bits = first->mAlternateColor.g.bits;
    f32 firstRadiusSquared = gabi::fmuls_ppc(firstRadius, firstRadius);
    firstColor->b.bits = first->mAlternateColor.b.bits;
    f32 secondRadiusSquared = gabi::fmuls_ppc(secondRadius, secondRadius);
    f32 firstDepthValue = gabi::fdivs_ppc(depthSquared, firstRadiusSquared);
    firstColor->a.bits = first->mAlternateColor.a.bits;
    u16 firstFlags = first->mFlags;
    DrawResourceConfiguration_l* configuration = mConfiguration;
    f32 secondDepthValue = gabi::fdivs_ppc(depthSquared, secondRadiusSquared);
    if ((firstFlags & 2) && !(u32(configuration->mFlags) & 0x100)) {
        sead::ColorChannels* color = firstColor.get();
        f32 greenTerm = gabi::fmuls_ppc(color->g.value, 0.5866109728813171f);
        f32 luminance = gabi::fmadds(color->r.value, 0.298911988735199f, greenTerm);
        luminance = gabi::fmadds(color->b.value, 0.11447799950838089f, luminance);
        color->a.value = gabi::fmuls_ppc(color->a.value, luminance);
    }
    secondColor->r.bits = second->mAlternateColor.r.bits;
    secondColor->g.bits = second->mAlternateColor.g.bits;
    secondColor->b.bits = second->mAlternateColor.b.bits;
    secondColor->a.bits = second->mAlternateColor.a.bits;
    u16 secondFlags = second->mFlags;
    if ((secondFlags & 2) && !(u32(configuration->mFlags) & 0x100)) {
        sead::ColorChannels* color = secondColor.get();
        f32 greenTerm = gabi::fmuls_ppc(color->g.value, 0.5866109728813171f);
        f32 luminance = gabi::fmadds(color->r.value, 0.298911988735199f, greenTerm);
        luminance = gabi::fmadds(color->b.value, 0.11447799950838089f, luminance);
        color->a.value = gabi::fmuls_ppc(color->a.value, luminance);
    }
    u32 format = configuration->mFormatIndex;
    u32 valueCount = record->mValueCount;
    f32 scale = sDrawParameterScaleTable.mValues[format];
    DrawPairParameterValue_l* value = record->mValues;
    f32 diameterScale = gabi::fadds_ppc(scale, scale);
    if (slot < valueCount)
        value += slot;
    UniformBuffer* buffer = &value->mBuffer;
    prepareDrawParameterBuffer(buffer);
    selected = record->mSelected;
    f32 selectedRadius = selected->mScale;
    f32 selectedX = selected->mPosition.x;
    f32 selectedZ = selected->mPosition.z;
    f32 scaledRadius = gabi::fmuls_ppc(selectedRadius, diameterScale);
    f32 selectedY = selected->mPosition.y;
    positionRadius->x = selectedX;
    positionRadius->y = selectedY;
    positionRadius->z = selectedZ;
    positionRadius->w = scaledRadius;
    writeDrawParameter(buffer, buffer->cpuData, 0, positionRadius.get(), 0, 1);
    first = record->mFirst;
    writeDrawParameter(buffer, buffer->cpuData, 1, &first->mColor, 0, 1);
    second = record->mSecond;
    writeDrawParameter(buffer, buffer->cpuData, 3, &second->mColor, 0, 1);
    selected = record->mSelected;
    firstInverse->mValue = gabi::fdivs_ppc(1.0f, selected->mScale);
    writeDrawParameter(buffer, buffer->cpuData, 5, firstInverse.get(), 0, 1);
    second = record->mSecond;
    secondInverse->mValue = gabi::fdivs_ppc(1.0f, second->mScale);
    writeDrawParameter(buffer, buffer->cpuData, 6, secondInverse.get(), 0, 1);
    selected = record->mSelected;
    firstPositive->mValue = selected->mPositiveValue;
    writeDrawParameter(buffer, buffer->cpuData, 7, firstPositive.get(), 0, 1);
    second = record->mSecond;
    secondPositive->mValue = second->mPositiveValue;
    writeDrawParameter(buffer, buffer->cpuData, 8, secondPositive.get(), 0, 1);
    first = record->mFirst;
    if (u16(first->mFlags) & 2)
        writeDrawParameter(buffer, buffer->cpuData, 2, firstColor.get(), 0, 1);
    second = record->mSecond;
    if (u16(second->mFlags) & 2)
        writeDrawParameter(buffer, buffer->cpuData, 4, secondColor.get(), 0, 1);
    writeDrawParameter(buffer, buffer->cpuData, 9, viewPosition.get(), 0, 1);
    writeDrawParameter(buffer, buffer->cpuData, 10, projection.get(), 0, 1);
    firstDepth->mValue = firstDepthValue;
    writeDrawParameter(buffer, buffer->cpuData, 11, firstDepth.get(), 0, 1);
    secondDepth->mValue = secondDepthValue;
    writeDrawParameter(buffer, buffer->cpuData, 12, secondDepth.get(), 0, 1);
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}

}
/* WWHD glue
data sDrawParameterScaleTable 101F9614
data sDrawParameterConservativeScale 101F960C
ext 028E8F64 void transformDrawParameterPosition(const sead::RenderMatrix34* matrix, const void* input, sead::RenderVector3* output)
ext 027BEF54 void prepareDrawParameterBuffer(agl::UniformBuffer* buffer)
ext 027BF048 void writeDrawParameter(agl::UniformBuffer* buffer, void* data, u32 field, const void* source, u32 offset, u32 count)
*/

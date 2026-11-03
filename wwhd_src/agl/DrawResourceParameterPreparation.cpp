// Own directional-resource parameter matrices and buffer writes, preserving full FPR math returns.
#include "agl/DrawParameterPreparation.h"
void transformResourcePoint(const sead::RenderMatrix34* matrix, const sead::RenderVector3* input, sead::RenderVector3* output);
void transformResourceDirection(const sead::RenderMatrix34* matrix, const sead::RenderVector3* input, sead::RenderVector3* output);
void translateResourceMatrix(sead::RenderMatrix34* matrix, f32 x, f32 y, f32 z);
void scaleResourceMatrix(sead::RenderMatrix34* matrix, f32 x, f32 y, f32 z);
void concatenateResourceMatrix(const sead::RenderMatrix34* left, const sead::RenderMatrix34* right, sead::RenderMatrix34* output);
u32 invertResourceMatrix(const sead::RenderMatrix34* input, sead::RenderMatrix34* output);
void rotateResourceDirection(sead::RenderMatrix34* output, const sead::RenderVector3* from, const sead::RenderVector3* to);
f64 cosineResourceAngle(f64 angle);
f64 evaluateResourceAngle(f64 angle);
void multiplyResourceProjection(sead::RenderMatrix44* output, const sead::RenderMatrix44* projection, const sead::RenderMatrix34* matrix);
void prepareDrawParameterBuffer(agl::UniformBuffer* buffer);
void writeDrawParameter(agl::UniformBuffer* buffer, void* data, u32 field, const void* source, u32 offset, u32 count);
namespace agl {
struct ResourceScalar_l { be<f32> mValue; };
struct ResourceIntegerPack_l { be<u32> mHigh, mLow; };
struct ResourceMatrixWords_l { be<u32> mWords[12]; };
static sead::RenderVector3 sResourceParameterDefaultAxis;
static f32 sResourceParameterVerticalBias;
static DrawResourceValue_l* selectedResourceValue(DrawResourceRecord_l* record, u32 slot) {
    u32 count=record->mValueCount;
    DrawResourceValue_l* value=record->mValues;
    if (slot<count) value+=slot;
    return value;
}
/* 027CD1E4 */
void agl::DrawResourceDirectionState_l::prepareResourceParameters(DrawResourceRecord_l* record, u32 slot,
                                                                 const DrawParameterFrame_l* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderVector2> texelScale(8);
    gabi::FrameLocal<ResourceIntegerPack_l> integerPack(0x10);
    gabi::FrameLocal<sead::RenderVector3> screenProjection(0x18), negativeAxis(0x24);
    gabi::FrameLocal<ResourceScalar_l> reciprocalCosine(0x30), inverseFalloff(0x34), inverseRadius(0x38);
    gabi::FrameLocal<ResourceScalar_l> scale(0x3C), falloff(0x40), cosineWeight(0x44);
    gabi::FrameLocal<sead::ColorChannels> alternateColor(0x48);
    gabi::FrameLocal<sead::RenderMatrix34> model(0x58), temporaryScale(0xAC), rotation(0xDC);
    gabi::FrameLocal<sead::RenderVector3> copiedAxis(0x88), viewPosition(0x94), viewDirection(0xA0);
    gabi::FrameLocal<sead::RenderMatrix34> radiusScale(0x10C), angularScale(0x13C), translation(0x16C), biasScale(0x19C), viewModel(0x1CC);
    gabi::FrameLocal<sead::RenderMatrix44> projectedModel(0x1FC), sourceProjection(0x23C);
    transformResourcePoint(&context->mWorldToView, &record->mPosition, viewPosition);
    transformResourceDirection(&context->mWorldToView, &record->mDirection, viewDirection);
    f32 translationY = record->mPosition.y;
    f32 translationZ = record->mPosition.z;
    f32 translationX = record->mPosition.x;
    translateResourceMatrix(model, translationX, translationY, translationZ);
    f32 axisY=sResourceParameterDefaultAxis.y;
    f32 axisX=sResourceParameterDefaultAxis.x;
    f32 axisZ=sResourceParameterDefaultAxis.z;
    negativeAxis->y=-axisY;
    negativeAxis->z=-axisZ;
    negativeAxis->x=-axisX;
    copiedAxis->x=negativeAxis->x;
    copiedAxis->y=negativeAxis->y;
    copiedAxis->z=negativeAxis->z;
    rotateResourceDirection(rotation, copiedAxis, &record->mDirection);
    concatenateResourceMatrix(model, rotation, model);
    f32 halfAngle=gabi::fmuls_ppc(0.5f,record->mAngle);
    f64 halfCosine=cosineResourceAngle(gabi::fpr_from_f32(halfAngle));
    f32 inverseHalfCosine=gabi::fdivs_fpr(1.0,halfCosine);
    f64 angleCosine=cosineResourceAngle(gabi::fpr_from_f32(record->mAngle));
    f32 angularWidth=gabi::fmuls_fpr(f64(inverseHalfCosine),angleCosine);
    scaleResourceMatrix(temporaryScale,angularWidth,angularWidth,angularWidth);
    concatenateResourceMatrix(model,temporaryScale,model);
    f32 radius=record->mRadius;
    scaleResourceMatrix(radiusScale,radius,radius,radius);
    concatenateResourceMatrix(model,radiusScale,model);
    f64 angleValue=evaluateResourceAngle(gabi::fpr_from_f32(record->mAngle));
    f32 width=gabi::fadds_fpr(angleValue,angleValue);
    scaleResourceMatrix(angularScale,width,1.0f,width);
    concatenateResourceMatrix(model,angularScale,model);
    translateResourceMatrix(translation,0.0f,-0.5f,0.0f);
    concatenateResourceMatrix(model,translation,model);
    DrawResourceConfiguration_l* configuration=mConfiguration;
    u32 flags=configuration->mFlags;
    f32 stateScale=mScale;
    f32 bias=(flags&0x20)?f32(sResourceParameterVerticalBias):1.0f;
    scaleResourceMatrix(biasScale,stateScale,bias,stateScale);
    concatenateResourceMatrix(model,biasScale,model);
    concatenateResourceMatrix(&context->mWorldToView,model,viewModel);
    multiplyResourceProjection(projectedModel,&context->mViewProjection,viewModel);
    DrawResourceValue_l* value=selectedResourceValue(record,slot);
    TextureData* source=value->mSource;
    if(gabi::ea(source)!=0){
        const ResourceMatrixWords_l* from=gabi::at<ResourceMatrixWords_l>(gabi::ea(&context->mWorldToView));
        ResourceMatrixWords_l* to=gabi::at<ResourceMatrixWords_l>(gabi::ea(temporaryScale.get()));
        for(u32 index=0;index<12;++index)to->mWords[index]=from->mWords[index];
        invertResourceMatrix(temporaryScale,temporaryScale);
        value=selectedResourceValue(record,slot);
        multiplyResourceProjection(sourceProjection,&value->mMatrix,temporaryScale);
    }
    f32 positionY = record->mPosition.y;
    f32 positionX = record->mPosition.x;
    f32 positionZ = record->mPosition.z;
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
    f32 normalizedX=gabi::fdivs_ppc(clipX,clipW);
    f32 normalizedY=gabi::fdivs_ppc(clipY,clipW);
    f32 projectionHeight=context->mProjectionHeight;
    f32 currentRadius=record->mRadius;
    f32 verticalMultiplier=context->mVerticalMultiplier;
    f32 verticalScale=gabi::fmuls_ppc(projectionHeight,verticalMultiplier);
    f32 radiusSquared=gabi::fmuls_ppc(currentRadius,currentRadius);
    f32 projectedY=-gabi::fmuls_ppc(normalizedY,verticalScale);
    f32 horizontalMultiplier=context->mHorizontalMultiplier;
    f32 depthSquared=gabi::fmuls_ppc(clipW,clipW);
    f32 projectedX=-gabi::fmuls_ppc(normalizedX,horizontalMultiplier);
    f32 depthRatio=gabi::fdivs_ppc(depthSquared,radiusSquared);
    f32 angle=record->mAngle;
    f32 offsetX=context->mOffsetX;
    f32 offsetY=context->mOffsetY;
    screenProjection->z=depthRatio;
    screenProjection->x=gabi::fsubs_ppc(projectedX,offsetX);
    screenProjection->y=gabi::fmsubs(projectedY,projectionHeight,offsetY);
    f64 cosine=cosineResourceAngle(gabi::fpr_from_f32(angle));
    f32 falloffDenominator=gabi::fsubs_fpr(1.0,cosine);
    angle=record->mAngle;
    f32 inverseDenominator=gabi::fdivs_ppc(1.0f,falloffDenominator);
    f64 secondCosine=cosineResourceAngle(gabi::fpr_from_f32(angle));
    value=selectedResourceValue(record,slot);
    f32 inverseCosine=gabi::fdivs_fpr(1.0,secondCosine);
    f32 cosineOffset=gabi::fsubs_ppc(inverseCosine,1.0f);
    UniformBuffer* buffer=&value->mBuffer;
    prepareDrawParameterBuffer(buffer);
    reciprocalCosine->mValue=cosineOffset;
    writeDrawParameter(buffer,buffer->cpuData,0,reciprocalCosine.get(),0,1);
    inverseFalloff->mValue=inverseDenominator;
    writeDrawParameter(buffer,buffer->cpuData,1,inverseFalloff.get(),0,1);
    inverseRadius->mValue=gabi::fdivs_ppc(1.0f,record->mRadius);
    writeDrawParameter(buffer,buffer->cpuData,2,inverseRadius.get(),0,1);
    scale->mValue=record->mScale;
    writeDrawParameter(buffer,buffer->cpuData,3,scale.get(),0,1);
    falloff->mValue=record->mFalloff;
    writeDrawParameter(buffer,buffer->cpuData,4,falloff.get(),0,1);
    writeDrawParameter(buffer,buffer->cpuData,6,&record->mPrimaryColor,0,1);
    cosineWeight->mValue=gabi::fmuls_fpr(cosine,f64(inverseDenominator));
    writeDrawParameter(buffer,buffer->cpuData,5,cosineWeight.get(),0,1);
    if(u8(record->mFlags)&2){
        u32 blue=record->mAlternateColor.b.bits;
        alternateColor->r.bits=record->mAlternateColor.r.bits;
        alternateColor->g.bits=record->mAlternateColor.g.bits;
        u32 alpha=record->mAlternateColor.a.bits;
        configuration=mConfiguration;
        alternateColor->b.bits=blue;
        alternateColor->a.bits=alpha;
        if(!(u32(configuration->mFlags)&0x100)){
            sead::ColorChannels* color = alternateColor.get();
            f32 green=gabi::fmuls_ppc(color->g.value,0.5866109728813171f);
            f32 luminance=gabi::fmadds(color->r.value,0.298911988735199f,green);
            luminance=gabi::fmadds(color->b.value,0.11447799950838089f,luminance);
            color->a.value=gabi::fmuls_ppc(color->a.value,luminance);
        }
        writeDrawParameter(buffer,buffer->cpuData,7,alternateColor.get(),0,1);
    }
    writeDrawParameter(buffer,buffer->cpuData,8,projectedModel.get(),0,4);
    writeDrawParameter(buffer,buffer->cpuData,9,viewPosition.get(),0,1);
    writeDrawParameter(buffer,buffer->cpuData,10,viewDirection.get(),0,1);
    value=selectedResourceValue(record,slot);
    source=value->mSource;
    if(gabi::ea(source)!=0){
        writeDrawParameter(buffer,buffer->cpuData,11,sourceProjection.get(),0,4);
        value=selectedResourceValue(record,slot);
        source=value->mSource;
        u32 width=source->minimumWidth;
        s32 actualWidth=s32(u32(source->surface.mWidth));
        if(actualWidth>s32(width))width=u32(actualWidth);
        integerPack->mLow=width;
        integerPack->mHigh=0x43300000;
        f64 packedWidth=*gabi::at<be<f64>>(gabi::ea(integerPack.get()));
        f32 widthValue=f32(packedWidth-4503599627370496.0);
        source=value->mSource;
        f32 textureScale=record->mTextureScale;
        u32 height=source->minimumHeight;
        s32 actualHeight=s32(u32(source->surface.mHeight));
        f32 widthScale=gabi::fdivs_ppc(textureScale,widthValue);
        if(actualHeight>s32(height))height=u32(actualHeight);
        integerPack->mLow=height;
        f64 packedHeight=*gabi::at<be<f64>>(gabi::ea(integerPack.get()));
        f32 heightValue=f32(packedHeight-4503599627370496.0);
        f32 heightScale=gabi::fdivs_ppc(textureScale,heightValue);
        texelScale->x=widthScale;
        texelScale->y=heightScale;
        writeDrawParameter(buffer,buffer->cpuData,13,texelScale.get(),0,1);
    }
    writeDrawParameter(buffer,buffer->cpuData,12,screenProjection.get(),0,1);
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
}
/* WWHD glue
data sResourceParameterDefaultAxis 104A0940
data sResourceParameterVerticalBias 101F9610
ext 028E8F64 void transformResourcePoint(const sead::RenderMatrix34* matrix, const sead::RenderVector3* input, sead::RenderVector3* output)
ext 028E9044 void transformResourceDirection(const sead::RenderMatrix34* matrix, const sead::RenderVector3* input, sead::RenderVector3* output)
ext 028E93CC void translateResourceMatrix(sead::RenderMatrix34* matrix, f32 x, f32 y, f32 z)
ext 028E945C void scaleResourceMatrix(sead::RenderMatrix34* matrix, f32 x, f32 y, f32 z)
ext 028E9108 void concatenateResourceMatrix(const sead::RenderMatrix34* left, const sead::RenderMatrix34* right, sead::RenderMatrix34* output)
ext 028E91EC u32 invertResourceMatrix(const sead::RenderMatrix34* input, sead::RenderMatrix34* output)
ext 027AAFF4 void rotateResourceDirection(sead::RenderMatrix34* output, const sead::RenderVector3* from, const sead::RenderVector3* to)
ext 028F4BE0 f64 cosineResourceAngle(f64 angle)
ext 028F423C f64 evaluateResourceAngle(f64 angle)
ext 0272D680 void multiplyResourceProjection(sead::RenderMatrix44* output, const sead::RenderMatrix44* projection, const sead::RenderMatrix34* matrix)
*/

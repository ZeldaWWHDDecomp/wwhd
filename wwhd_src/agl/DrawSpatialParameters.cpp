// Own spatial parameter writer. Canonical SEAD camera/projection prefixes are borrowed through their APIs.
#include "agl/DrawParameterPreparation.h"
#include "sead/ProjectionState.h"
void prepareDrawParameterBuffer(agl::UniformBuffer* buffer);
void writeDrawParameter(agl::UniformBuffer* buffer, void* data, u32 field, const void* source, u32 offset, u32 count);
void transformResourcePoint(const sead::RenderMatrix34* matrix, const sead::RenderVector3* input, sead::RenderVector3* output);
void transformResourceDirection(const sead::RenderMatrix34* matrix, const sead::RenderVector3* input, sead::RenderVector3* output);
void normalizeDrawResourceDirection(sead::RenderVector3* direction);
void translateResourceMatrix(sead::RenderMatrix34* matrix, f32 x, f32 y, f32 z);
void scaleResourceMatrix(sead::RenderMatrix34* matrix, f32 x, f32 y, f32 z);
void concatenateResourceMatrix(const sead::RenderMatrix34* left, const sead::RenderMatrix34* right, sead::RenderMatrix34* output);
u32 invertResourceMatrix(const sead::RenderMatrix34* input, sead::RenderMatrix34* output);
void multiplyResourceProjection(sead::RenderMatrix44* output, const sead::RenderMatrix44* projection, const sead::RenderMatrix34* matrix);
sead::ViewCamera* constructSpatialCamera(sead::ViewCamera* object, const sead::RenderVector3* eye, const sead::RenderVector3* target, const sead::RenderVector3* up);
void destroySpatialCamera(sead::ViewTransform* object, u32 flags);
void copySpatialCameraMatrix(const sead::ViewTransform* source, sead::RenderMatrix34* target);
sead::ProjectionState* constructSpatialOrtho(sead::ProjectionState* object, f32 nearPlane, f32 farPlane, f32 top, f32 bottom, f32 left, f32 right);
sead::ProjectionState* constructSpatialPerspective(sead::ProjectionState* object, f32 nearPlane, f32 farPlane, f32 angle, f32 aspect);
sead::ProjectionMatrix* spatialProjectionMatrix(sead::ProjectionState* object);
void destroySpatialOrtho(sead::ProjectionState* object, u32 flags);
void destroySpatialPerspective(sead::ProjectionState* object, u32 flags);
void concatenateSpatialProjection(const sead::RenderMatrix44* left, const sead::RenderMatrix44* right, sead::RenderMatrix44* output);
namespace agl {
struct SpatialScalar_l { be<f32> mValue; };
struct SpatialIntegerPack_l { be<u32> mHigh,mLow; };
struct SpatialMatrixWords_l { be<u32> mWords[12]; };
struct SpatialCameraDispatch_l { u8 mPrefix[0x20]; sead::VtableSlot mBuild; };
WWHD_OFFSET(SpatialCameraDispatch_l,mBuild,0x20);
static DrawResourceValue_l* spatialParameterValue(LargeDrawResourceRecord_l* record,u32 slot) {
    u32 count=record->mValueCount;
    DrawResourceValue_l* value=record->mValues;
    if(slot<count)value+=slot;
    return value;
}
static sead::RenderMatrix34* spatialMatrixPrefix(sead::RenderMatrix44* matrix) {
    return gabi::at<sead::RenderMatrix34>(gabi::ea(matrix));
}
static void copySpatialMatrixBits(void* destination,const void* source) {
    SpatialMatrixWords_l* to=gabi::at<SpatialMatrixWords_l>(gabi::ea(destination));
    const SpatialMatrixWords_l* from=gabi::at<SpatialMatrixWords_l>(gabi::ea(source));
    for(u32 index=0;index<12;++index)to->mWords[index]=from->mWords[index];
}
static void buildSpatialCamera(sead::ViewCamera* camera) {
    void* table=camera->mVtable;
    SpatialCameraDispatch_l* dispatch=gabi::at<SpatialCameraDispatch_l>(gabi::ea(table));
    gabi::call_ptr<void>(dispatch->mBuild.mFunction,camera,&camera->mTransform);
}
/* 027CEB24 */
void agl::DrawResourceDirectionState_l::prepareLargeParameters(LargeDrawResourceRecord_l* record,u32 slot,
                                                              const DrawParameterFrame_l* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderVector2> inverseTextureScale(8),negativeOffsets(0x10),texelScale(0x18);
    gabi::FrameLocal<SpatialIntegerPack_l> integerPack(0x20);
    gabi::FrameLocal<sead::RenderMatrix44> working(0x28);
    gabi::FrameLocal<SpatialScalar_l> nearRatio(0x68),inverseRange(0x6C),positiveScale(0x70),nearOffset(0x74);
    gabi::FrameLocal<sead::ColorChannels> alternateColor(0x78);
    gabi::FrameLocal<sead::RenderVector3> viewPosition(0x88),viewDirection(0x94),worldTargetScratch(0xA0),viewTargetScratch(0xAC);
    gabi::FrameLocal<sead::RenderVector2> copiedOffsets(0xB8);
    gabi::FrameLocal<sead::RenderVector3> worldTarget(0xC0),viewTarget(0xCC);
    gabi::FrameLocal<sead::RenderMatrix44> projected(0xD8);
    gabi::FrameLocal<sead::ViewCamera> firstCamera(0x108),secondCamera(0x118);
    gabi::FrameLocal<sead::RenderMatrix34> radiusScale(0x160),viewCopy(0x170),aspectScale(0x190),translation(0x1C0),viewModel(0x1F0);
    // This is the canonical base-prefix view inside the original caller's reserved frame.
    gabi::FrameLocal<sead::ProjectionState> projection(0x1A0);
    gabi::FrameLocal<sead::RenderMatrix44> projectedModel(0x220);
    DrawResourceValue_l* value=spatialParameterValue(record,slot);
    UniformBuffer* buffer=&value->mBuffer;
    prepareDrawParameterBuffer(buffer);
    f32 farPlane=record->mFar;
    f32 nearPlane=record->mNear;
    f32 range=gabi::fsubs_ppc(farPlane,nearPlane);
    f32 ratio=gabi::fdivs_ppc(nearPlane,farPlane);
    f32 inverse=gabi::fdivs_ppc(1.0f,range);
    writeDrawParameter(buffer,buffer->cpuData,0,&record->mColor,0,1);
    nearRatio->mValue=ratio;
    writeDrawParameter(buffer,buffer->cpuData,2,nearRatio.get(),0,1);
    inverseRange->mValue=inverse;
    writeDrawParameter(buffer,buffer->cpuData,3,inverseRange.get(),0,1);
    positiveScale->mValue=record->mPositiveScale;
    writeDrawParameter(buffer,buffer->cpuData,4,positiveScale.get(),0,1);
    nearOffset->mValue=gabi::fmadds(record->mNear,inverse,1.0f);
    writeDrawParameter(buffer,buffer->cpuData,5,nearOffset.get(),0,1);
    if(u8(record->mFlags)&2){
        alternateColor->r.bits=record->mAlternateColor.r.bits;
        u32 alpha=record->mAlternateColor.a.bits;
        alternateColor->g.bits=record->mAlternateColor.g.bits;
        alternateColor->b.bits=record->mAlternateColor.b.bits;
        DrawResourceConfiguration_l* configuration=mConfiguration;
        alternateColor->a.bits=alpha;
        if(!(u32(configuration->mFlags)&0x100) && u8(record->mEnabled)==0){
            sead::ColorChannels* color = alternateColor.get();
            f32 green=gabi::fmuls_ppc(color->g.value,0.5866109728813171f);
            f32 luminance=gabi::fmadds(color->r.value,0.298911988735199f,green);
            luminance=gabi::fmadds(color->b.value,0.11447799950838089f,luminance);
            color->a.value=gabi::fmuls_ppc(color->a.value,luminance);
        }
        writeDrawParameter(buffer,buffer->cpuData,1,alternateColor.get(),0,1);
    }
    transformResourcePoint(&context->mWorldToView,&record->mCenter,viewPosition);
    transformResourceDirection(&context->mWorldToView,&record->mRenderDirection,viewDirection);
    normalizeDrawResourceDirection(viewDirection);
    if(u8(record->mFlags)&4)
        writeDrawParameter(buffer,buffer->cpuData,9,viewDirection.get(),0,1);
    else
        writeDrawParameter(buffer,buffer->cpuData,8,viewPosition.get(),0,1);
    sead::RenderMatrix34* model=spatialMatrixPrefix(working);
    translateResourceMatrix(model,0.0f,0.0f,0.0f);
    worldTargetScratch->z=gabi::fadds_ppc(record->mCenter.z,record->mRenderDirection.z);
    worldTargetScratch->x=gabi::fadds_ppc(record->mCenter.x,record->mRenderDirection.x);
    worldTargetScratch->y=gabi::fadds_ppc(record->mCenter.y,record->mRenderDirection.y);
    worldTarget->x=worldTargetScratch->x;
    worldTarget->y=worldTargetScratch->y;
    worldTarget->z=worldTargetScratch->z;
    constructSpatialCamera(firstCamera,&record->mCenter,worldTarget,&record->mUp);
    buildSpatialCamera(firstCamera);
    copySpatialMatrixBits(projected.get(),&firstCamera->mTransform);
    sead::RenderMatrix34* inverseView=spatialMatrixPrefix(projected);
    invertResourceMatrix(inverseView,inverseView);
    concatenateResourceMatrix(model,inverseView,model);
    destroySpatialCamera(&firstCamera->mTransform,2);
    farPlane=record->mFar;
    scaleResourceMatrix(radiusScale,farPlane,farPlane,farPlane);
    concatenateResourceMatrix(model,radiusScale,model);
    f32 slope=record->mSlope;
    f32 aspect=record->mAspectRatio;
    f32 height=gabi::fadds_ppc(slope,slope);
    f32 width=gabi::fmuls_ppc(height,aspect);
    scaleResourceMatrix(aspectScale,width,height,1.0f);
    concatenateResourceMatrix(model,aspectScale,model);
    translateResourceMatrix(translation,0.0f,0.0f,-0.5f);
    concatenateResourceMatrix(model,translation,model);
    concatenateResourceMatrix(&context->mWorldToView,model,viewModel);
    multiplyResourceProjection(projectedModel,&context->mViewProjection,viewModel);
    writeDrawParameter(buffer,buffer->cpuData,6,projectedModel.get(),0,4);
    transformResourceDirection(&context->mWorldToView,&record->mUp,gabi::at<sead::RenderVector3>(gabi::ea(working.get())));
    viewTargetScratch->x=gabi::fadds_ppc(viewPosition->x,viewDirection->x);
    viewTargetScratch->y=gabi::fadds_ppc(viewPosition->y,viewDirection->y);
    viewTargetScratch->z=gabi::fadds_ppc(viewPosition->z,viewDirection->z);
    viewTarget->x=viewTargetScratch->x;
    viewTarget->y=viewTargetScratch->y;
    viewTarget->z=viewTargetScratch->z;
    constructSpatialCamera(secondCamera,viewPosition,viewTarget,gabi::at<sead::RenderVector3>(gabi::ea(working.get())));
    buildSpatialCamera(secondCamera);
    copySpatialCameraMatrix(&secondCamera->mTransform,viewCopy);
    destroySpatialCamera(&secondCamera->mTransform,2);
    u8 flags=record->mFlags;
    nearPlane=record->mNear;
    farPlane=record->mFar;
    if(flags&4){
        f32 top=record->mVerticalExtent;
        f32 left=record->mHorizontalExtent;
        constructSpatialOrtho(projection,nearPlane,farPlane,top,-top,left,-left);
        sead::ProjectionMatrix* matrix=spatialProjectionMatrix(projection);
        multiplyResourceProjection(projected,gabi::at<sead::RenderMatrix44>(gabi::ea(matrix)),viewCopy);
        destroySpatialOrtho(projection,2);
    }else{
        constructSpatialPerspective(projection,nearPlane,farPlane,record->mVerticalAngle,record->mAspectRatio);
        sead::ProjectionMatrix* matrix=spatialProjectionMatrix(projection);
        multiplyResourceProjection(projected,gabi::at<sead::RenderMatrix44>(gabi::ea(matrix)),viewCopy);
        destroySpatialPerspective(projection,2);
    }
    working->mRows[3][1]=0.0f;
    working->mRows[3][2]=0.0f;
    working->mRows[1][2]=0.0f;
    working->mRows[1][3]=0.5f;
    working->mRows[0][0]=0.5f;
    working->mRows[2][1]=0.0f;
    working->mRows[2][2]=1.0f;
    working->mRows[2][3]=0.0f;
    working->mRows[3][0]=0.0f;
    working->mRows[2][0]=0.0f;
    working->mRows[1][0]=0.0f;
    working->mRows[0][2]=0.0f;
    working->mRows[1][1]=-0.5f;
    working->mRows[3][3]=1.0f;
    working->mRows[0][3]=0.5f;
    working->mRows[0][1]=0.0f;
    concatenateSpatialProjection(working,projected,projected);
    writeDrawParameter(buffer,buffer->cpuData,7,projected.get(),0,4);
    if(u8(record->mFlags)&0x10){
        f32 x=record->mOffsetX;
        f32 y=record->mOffsetY;
        negativeOffsets->x=-x;
        copiedOffsets->x=negativeOffsets->x;
        negativeOffsets->y=-y;
        copiedOffsets->y=negativeOffsets->y;
        f32 inverseX=gabi::fdivs_ppc(1.0f,record->mScaleX);
        f32 inverseY=gabi::fdivs_ppc(1.0f,record->mScaleY);
        inverseTextureScale->x=inverseX;
        inverseTextureScale->y=inverseY;
        writeDrawParameter(buffer,buffer->cpuData,12,copiedOffsets.get(),0,1);
        writeDrawParameter(buffer,buffer->cpuData,13,inverseTextureScale.get(),0,1);
    }
    value=spatialParameterValue(record,slot);
    TextureData* source=value->mSource;
    if(gabi::ea(source)!=0){
        copySpatialMatrixBits(working.get(),&context->mWorldToView);
        model=spatialMatrixPrefix(working);
        invertResourceMatrix(model,model);
        value=spatialParameterValue(record,slot);
        multiplyResourceProjection(projected,&value->mMatrix,model);
        writeDrawParameter(buffer,buffer->cpuData,10,projected.get(),0,4);
        value=spatialParameterValue(record,slot);
        source=value->mSource;
        u32 width=source->minimumWidth;
        s32 actualWidth=s32(u32(source->surface.mWidth));
        if(actualWidth>s32(width))width=u32(actualWidth);
        integerPack->mLow=width;
        integerPack->mHigh=0x43300000;
        f64 packed=*gabi::at<be<f64>>(gabi::ea(integerPack.get()));
        f32 widthValue=f32(packed-4503599627370496.0);
        source=value->mSource;
        f32 textureScale=record->mTextureScale;
        u32 heightValue=source->minimumHeight;
        s32 actualHeight=s32(u32(source->surface.mHeight));
        f32 widthScale=gabi::fdivs_ppc(textureScale,widthValue);
        if(actualHeight>s32(heightValue))heightValue=u32(actualHeight);
        integerPack->mLow=heightValue;
        packed=*gabi::at<be<f64>>(gabi::ea(integerPack.get()));
        f32 height=gabi::fdivs_ppc(textureScale,f32(packed-4503599627370496.0));
        texelScale->x=widthScale;
        texelScale->y=height;
        writeDrawParameter(buffer,buffer->cpuData,11,texelScale.get(),0,1);
    }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
}
/* WWHD glue
ext 0274CBAC sead::ViewCamera* constructSpatialCamera(sead::ViewCamera* object, const sead::RenderVector3* eye, const sead::RenderVector3* target, const sead::RenderVector3* up)
ext 0274CCB0 void destroySpatialCamera(sead::ViewTransform* object, u32 flags)
ext 028E90D4 void copySpatialCameraMatrix(const sead::ViewTransform* source, sead::RenderMatrix34* target)
ext 0274E25C sead::ProjectionState* constructSpatialOrtho(sead::ProjectionState* object, f32 nearPlane, f32 farPlane, f32 top, f32 bottom, f32 left, f32 right)
ext 0274DF4C sead::ProjectionState* constructSpatialPerspective(sead::ProjectionState* object, f32 nearPlane, f32 farPlane, f32 angle, f32 aspect)
ext 0274D80C sead::ProjectionMatrix* spatialProjectionMatrix(sead::ProjectionState* object)
ext 0274E474 void destroySpatialOrtho(sead::ProjectionState* object, u32 flags)
ext 0274E038 void destroySpatialPerspective(sead::ProjectionState* object, u32 flags)
ext 028E89B4 void concatenateSpatialProjection(const sead::RenderMatrix44* left, const sead::RenderMatrix44* right, sead::RenderMatrix44* output)
*/

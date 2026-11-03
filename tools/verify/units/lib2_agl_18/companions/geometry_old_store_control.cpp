// Own orthographic/perspective spatial bounds. Typed temporaries preserve the original frame's vector stages.
#include "agl/DrawResourceCleanup.h"
void crossDrawResourceVectors(const sead::RenderVector3* left,const sead::RenderVector3* right,sead::RenderVector3* output);
void normalizeDrawResourceDirection(sead::RenderVector3* direction);
f64 evaluateResourceAngle(f64 angle);
namespace agl {
struct GeometryVectorWords_l { be<u32> x,y,z; };
union GeometryCornerScalar_l { be<u32> mBits; be<s32> mInteger; be<f32> mFloat; };
struct GeometryCornerTable_l { GeometryCornerScalar_l mRows[8][3]; };
static GeometryCornerTable_l sOrthoGeometryCorners;
static GeometryCornerTable_l sPerspectiveGeometryCorners;
WWHD_SIZE(GeometryCornerTable_l,0x60);
static f32 geometryStoredFloat(f32 value) {
    return gabi::f32_from_fpr(gabi::fpr_from_f32(value));
}
static void copyGeometryVector(sead::RenderVector3* output,const sead::RenderVector3* input) {
    GeometryVectorWords_l* to=gabi::at<GeometryVectorWords_l>(gabi::ea(output));
    const GeometryVectorWords_l* from=gabi::at<GeometryVectorWords_l>(gabi::ea(input));
    to->x=from->x;
    to->y=from->y;
    to->z=from->z;
}
static void scaleGeometryVector(sead::RenderVector3* output,const sead::RenderVector3* input,f32 scale) {
    f32 x=gabi::fmuls_ppc(input->x,scale);
    f32 y=gabi::fmuls_ppc(input->y,scale);
    f32 z=gabi::fmuls_ppc(input->z,scale);
    output->x=x;
    output->y=y;
    output->z=z;
}
static void addGeometryVector(sead::RenderVector3* output,const sead::RenderVector3* left,const sead::RenderVector3* right) {
    f32 x=gabi::fadds_ppc(left->x,right->x);
    f32 y=gabi::fadds_ppc(left->y,right->y);
    f32 z=gabi::fadds_ppc(left->z,right->z);
    output->x=x;
    output->y=y;
    output->z=z;
}
static void subtractGeometryVector(sead::RenderVector3* output,const sead::RenderVector3* left,const sead::RenderVector3* right) {
    f32 x=gabi::fsubs_ppc(left->x,right->x);
    f32 y=gabi::fsubs_ppc(left->y,right->y);
    f32 z=gabi::fsubs_ppc(left->z,right->z);
    output->x=x;
    output->y=y;
    output->z=z;
}
static void copyGeometryCorners(GeometryCornerTable_l* output,const GeometryCornerTable_l* input) {
    for(u32 row=0;row<8;++row)
        for(u32 column=0;column<3;++column)
            output->mRows[row][column].mBits=input->mRows[row][column].mBits;
}
static void initializeGeometryBounds(LargeDrawResourceRecord_l* record,const sead::RenderVector3* point) {
    f32 z=geometryStoredFloat(point->z);
    f32 x=geometryStoredFloat(point->x);
    f32 y=geometryStoredFloat(point->y);
    record->mBounds.mMinimum.z=z;
    record->mBounds.mMinimum.x=x;
    record->mBounds.mMaximum.z=z;
    record->mBounds.mMaximum.x=x;
    record->mBounds.mMinimum.y=y;
    record->mBounds.mMaximum.y=y;
}
static void extendGeometryBounds(LargeDrawResourceRecord_l* record,const sead::RenderVector3* point) {
    f32 x=point->x,y=point->y,z=point->z;
    if(record->mBounds.mMinimum.x>x)record->mBounds.mMinimum.x=geometryStoredFloat(x);
    if(record->mBounds.mMinimum.y>y)record->mBounds.mMinimum.y=geometryStoredFloat(y);
    if(record->mBounds.mMinimum.z>z)record->mBounds.mMinimum.z=geometryStoredFloat(z);
    if(record->mBounds.mMaximum.x<x)record->mBounds.mMaximum.x=geometryStoredFloat(x);
    if(record->mBounds.mMaximum.y<y)record->mBounds.mMaximum.y=geometryStoredFloat(y);
    if(record->mBounds.mMaximum.z<z)record->mBounds.mMaximum.z=geometryStoredFloat(z);
}
/* 027CE130 */
void agl::DrawResourceDirectionState_l::prepareLargeGeometry(LargeDrawResourceRecord_l* record) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderVector3> corner(8),perspectiveCorner(0x14),nearScratch(0x20),farScratch(0x2C);
    gabi::FrameLocal<sead::RenderVector3> nearRight(0x38),nearUp(0x44),farRight(0x50),farUp(0x5C);
    gabi::FrameLocal<sead::RenderVector3> baseCorner(0x68),rightEdge(0x74),upEdge(0x80),depthEdge(0x8C),stage(0x98);
    gabi::FrameLocal<sead::RenderVector3> right(0xA4),up(0xB0),centerRight(0xBC),positionRight(0xC8),centerUp(0xD4),centerSum(0xE0);
    gabi::FrameLocal<sead::RenderVector3> nearTranslation(0xEC),halfRight(0xF8),halfUp(0x104),doubleRight(0x110),doubleUp(0x11C),depthTranslation(0x128);
    gabi::FrameLocal<sead::RenderVector3> perspectiveNear(0x134),perspectiveRight(0x140),perspectiveUp(0x14C),perspectiveDepth(0x158);
    gabi::FrameLocal<sead::RenderVector3> finalCenter(0x164),positionStage(0x170),rightStage(0x17C),upStage(0x188),nearStage(0x194);
    gabi::FrameLocal<GeometryCornerTable_l> masks(0x1A0);
    if(u8(record->mValuesEnabled)!=0){
        crossDrawResourceVectors(&record->mRenderDirection,&record->mUp,right);
        normalizeDrawResourceDirection(right);
        crossDrawResourceVectors(right,&record->mRenderDirection,up);
        normalizeDrawResourceDirection(up);
        if(u8(record->mFlags)&4){
            f32 rightBound=record->mRight;
            f32 leftBound=record->mLeft;
            f32 topBound=record->mTop;
            f32 bottomBound=record->mBottom;
            f32 halfWidth=gabi::fmuls_ppc(gabi::fsubs_ppc(rightBound,leftBound),0.5f);
            f32 halfHeight=gabi::fmuls_ppc(gabi::fsubs_ppc(topBound,bottomBound),0.5f);
            record->mAspectRatio=gabi::fdivs_ppc(halfWidth,halfHeight);
            f32 farPlane=record->mFar;
            f32 horizontalCenter=gabi::fadds_ppc(halfWidth,leftBound);
            record->mHorizontalExtent=halfWidth;
            record->mVerticalExtent=halfHeight;
            f32 slope=gabi::fdivs_ppc(halfHeight,farPlane);
            scaleGeometryVector(centerRight,right,horizontalCenter);
            record->mSlope=slope;
            copyGeometryVector(rightStage,centerRight);
            f32 verticalCenter=gabi::fadds_ppc(halfHeight,bottomBound);
            addGeometryVector(positionRight,&record->mPosition,rightStage);
            scaleGeometryVector(centerUp,up,verticalCenter);
            copyGeometryVector(positionStage,positionRight);
            copyGeometryVector(upStage,centerUp);
            addGeometryVector(centerSum,positionStage,upStage);
            copyGeometryVector(finalCenter,centerSum);
            record->mCenter.x=geometryStoredFloat(finalCenter->x);
            record->mCenter.z=geometryStoredFloat(finalCenter->z);
            record->mCenter.y=geometryStoredFloat(finalCenter->y);
            copyGeometryVector(baseCorner,&record->mCenter);
            f32 nearPlane=record->mNear;
            scaleGeometryVector(nearTranslation,&record->mRenderDirection,nearPlane);
            copyGeometryVector(stage,nearTranslation);
            f32 nearX=stage->x,nearY=stage->y,nearZ=stage->z;
            halfWidth=record->mHorizontalExtent;
            halfHeight=record->mVerticalExtent;
            scaleGeometryVector(halfRight,right,halfWidth);
            copyGeometryVector(stage,halfRight);
            f32 rightX=stage->x,rightY=stage->y,rightZ=stage->z;
            scaleGeometryVector(halfUp,up,halfHeight);
            copyGeometryVector(stage,halfUp);
            f32 x=gabi::fadds_ppc(baseCorner->x,nearX);
            f32 y=gabi::fadds_ppc(baseCorner->y,nearY);
            f32 z=gabi::fadds_ppc(baseCorner->z,nearZ);
            x=gabi::fsubs_ppc(x,rightX);
            y=gabi::fsubs_ppc(y,rightY);
            z=gabi::fsubs_ppc(z,rightZ);
            baseCorner->x=gabi::fsubs_ppc(x,stage->x);
            baseCorner->y=gabi::fsubs_ppc(y,stage->y);
            baseCorner->z=gabi::fsubs_ppc(z,stage->z);
            f32 width=gabi::fadds_ppc(halfWidth,halfWidth);
            f32 height=gabi::fadds_ppc(halfHeight,halfHeight);
            farPlane=record->mFar;
            scaleGeometryVector(doubleRight,right,width);
            copyGeometryVector(rightEdge,doubleRight);
            scaleGeometryVector(doubleUp,up,height);
            copyGeometryVector(upEdge,doubleUp);
            f32 depth=gabi::fsubs_ppc(farPlane,nearPlane);
            scaleGeometryVector(depthTranslation,&record->mRenderDirection,depth);
            copyGeometryVector(depthEdge,depthTranslation);
            initializeGeometryBounds(record,baseCorner);
            copyGeometryCorners(masks,&sOrthoGeometryCorners);
            for(u32 index=1;index<8;++index){
                copyGeometryVector(corner,baseCorner);
                if(s32(masks->mRows[index][0].mInteger)>0)addGeometryVector(corner,corner,rightEdge);
                if(s32(masks->mRows[index][1].mInteger)>0)addGeometryVector(corner,corner,upEdge);
                if(s32(masks->mRows[index][2].mInteger)>0)addGeometryVector(corner,corner,depthEdge);
                extendGeometryBounds(record,corner);
            }
            record->mValuesEnabled=0;
        }else{
            f32 halfAngle=gabi::fmuls_ppc(record->mVerticalAngle,0.5f);
            f64 angleValue=evaluateResourceAngle(f64(halfAngle));
            record->mCenter.x=geometryStoredFloat(record->mPosition.x);
            record->mCenter.y=geometryStoredFloat(record->mPosition.y);
            f32 farPlane=record->mFar;
            f32 positionZ=record->mPosition.z;
            f32 aspect=record->mAspectRatio;
            record->mCenter.z=geometryStoredFloat(positionZ);
            f32 halfHeight=gabi::fmuls_fpr(f64(farPlane),angleValue);
            copyGeometryVector(baseCorner,&record->mCenter);
            record->mVerticalExtent=halfHeight;
            f32 halfWidth=gabi::fmuls_ppc(halfHeight,aspect);
            f32 nearPlane=record->mNear;
            record->mHorizontalExtent=halfWidth;
            scaleGeometryVector(perspectiveNear,&record->mRenderDirection,nearPlane);
            copyGeometryVector(nearStage,perspectiveNear);
            addGeometryVector(baseCorner,baseCorner,nearStage);
            scaleGeometryVector(perspectiveRight,right,record->mHorizontalExtent);
            copyGeometryVector(rightEdge,perspectiveRight);
            f32 depth=gabi::fsubs_ppc(farPlane,nearPlane);
            halfHeight=record->mVerticalExtent;
            scaleGeometryVector(perspectiveUp,up,halfHeight);
            // STFS truncates a nonsingle FPR; this is not an FRSP/native float conversion.
            record->mSlope=gabi::f32_from_fpr(angleValue);
            copyGeometryVector(upEdge,perspectiveUp);
            scaleGeometryVector(perspectiveDepth,&record->mRenderDirection,depth);
            copyGeometryVector(depthEdge,perspectiveDepth);
            initializeGeometryBounds(record,baseCorner);
            farPlane=record->mFar;
            f32 nearScale=gabi::fdivs_ppc(nearPlane,farPlane);
            copyGeometryCorners(masks,&sPerspectiveGeometryCorners);
            for(u32 index=0;index<8;++index){
                copyGeometryVector(perspectiveCorner,baseCorner);
                f32 maskZ=masks->mRows[index][2].mFloat;
                f32 maskX=masks->mRows[index][0].mFloat;
                if(maskZ==0.0f){
                    f32 horizontal=gabi::fmuls_ppc(maskX,nearScale);
                    scaleGeometryVector(nearRight,rightEdge,horizontal);
                    copyGeometryVector(nearScratch,nearRight);
                    f32 maskY=masks->mRows[index][1].mFloat;
                    f32 vertical=gabi::fmuls_ppc(maskY,nearScale);
                    scaleGeometryVector(nearUp,upEdge,vertical);
                    f32 x=gabi::fadds_ppc(perspectiveCorner->x,nearScratch->x);
                    f32 y=gabi::fadds_ppc(perspectiveCorner->y,nearScratch->y);
                    f32 z=gabi::fadds_ppc(perspectiveCorner->z,nearScratch->z);
                    copyGeometryVector(nearScratch,nearUp);
                    perspectiveCorner->x=gabi::fadds_ppc(x,nearScratch->x);
                    perspectiveCorner->y=gabi::fadds_ppc(y,nearScratch->y);
                    perspectiveCorner->z=gabi::fadds_ppc(z,nearScratch->z);
                }else{
                    scaleGeometryVector(farRight,rightEdge,maskX);
                    copyGeometryVector(farScratch,farRight);
                    f32 maskY=masks->mRows[index][1].mFloat;
                    scaleGeometryVector(farUp,upEdge,maskY);
                    f32 x=gabi::fadds_ppc(perspectiveCorner->x,farScratch->x);
                    f32 y=gabi::fadds_ppc(perspectiveCorner->y,farScratch->y);
                    f32 z=gabi::fadds_ppc(perspectiveCorner->z,farScratch->z);
                    copyGeometryVector(farScratch,farUp);
                    x=gabi::fadds_ppc(x,farScratch->x);
                    y=gabi::fadds_ppc(y,farScratch->y);
                    z=gabi::fadds_ppc(z,farScratch->z);
                    perspectiveCorner->x=gabi::fadds_ppc(x,depthEdge->x);
                    perspectiveCorner->y=gabi::fadds_ppc(y,depthEdge->y);
                    perspectiveCorner->z=gabi::fadds_ppc(z,depthEdge->z);
                }
                extendGeometryBounds(record,perspectiveCorner);
            }
            record->mValuesEnabled=0;
        }
    }
    f32 positiveScale=record->mPositiveScale;
    u8 flags=record->mFlags;
    if(!(positiveScale>0.0f))record->mPositiveScale=0.0010000000474974513f;
    if(flags&0x10){
        f32 scaleX=record->mScaleX;
        f32 scaleY=record->mScaleY;
        if(scaleX==0.0f)record->mScaleX=9.999999747378752e-05f;
        if(scaleY==0.0f)record->mScaleY=9.999999747378752e-05f;
    }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
}
/* WWHD glue
data sOrthoGeometryCorners 1016ACB8
data sPerspectiveGeometryCorners 1016AD18
ext 028E8D4C void crossDrawResourceVectors(const sead::RenderVector3* left,const sead::RenderVector3* right,sead::RenderVector3* output)
*/
